#include "RtspServer.h"
#include "audio_control.h"
#include "thread"

std::atomic<bool> running(true);
std::atomic<bool> frame_enabled(true);
cv::Mat* shared_frame_ptr = nullptr;
std::mutex frame_mutex;
cv::Mat img_camoff;

void enable_streaming(bool enabled) {
    g_print("enable_streaming: %s\n", enabled ? "ON" : "OFF");
    frame_enabled = enabled;
}

void push_frame_loop(GstElement* appsrc) {
    guint64 timestamp = 0;

    while (running) {
        cv::Mat frame;
        {
            std::lock_guard<std::mutex> lock(frame_mutex);

            if (frame_enabled && shared_frame_ptr && !shared_frame_ptr->empty()) {
                frame = shared_frame_ptr->clone();
            } else {
                if (img_camoff.empty()) {
                    g_printerr("img_camoff is empty\n");
                    continue;
                }
                frame = img_camoff.clone();
            }
        }

        if (!frame.isContinuous()) frame = frame.clone();

        int size = frame.total() * frame.elemSize();
        GstBuffer* buffer = gst_buffer_new_allocate(NULL, size, NULL);
        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_WRITE);
        memcpy(map.data, frame.data, size);
        gst_buffer_unmap(buffer, &map);

        GST_BUFFER_PTS(buffer) = timestamp;
        GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale_int(1, GST_SECOND, 30);
        timestamp += GST_BUFFER_DURATION(buffer);

        GstFlowReturn ret;
        g_signal_emit_by_name(appsrc, "push-buffer", buffer, &ret);
        gst_buffer_unref(buffer);

        g_usleep(1000 * 30);
    }
}

void on_media_configure(GstRTSPMediaFactory* factory, GstRTSPMedia* media, gpointer user_data) {
    GstElement* element = gst_rtsp_media_get_element(media);
    GstElement* appsrc = gst_bin_get_by_name_recurse_up(GST_BIN(element), "mysrc");

    if (appsrc) {
        std::thread(push_frame_loop, appsrc).detach();
        gst_object_unref(appsrc);
    }

    // 오디오 volume element : audio_control.cpp의 콜백
    g_signal_connect(media, "prepared", G_CALLBACK(audio_on_media_prepared), nullptr);

    gst_object_unref(element);
}


void start_rtsp_server() {
    gst_init(nullptr, nullptr);

    GstRTSPServer* server = gst_rtsp_server_new();
    GstRTSPMountPoints* mounts = gst_rtsp_server_get_mount_points(server);
    GstRTSPMediaFactory* factory = gst_rtsp_media_factory_new();

#ifdef _WIN32
    const gchar* vedio_convert_format = "format=I420 ";
    const gchar* audio_src = "wasapisrc low-latency=true ";
#elif __APPLE__
    const gchar* vedio_convert_format = "format=NV12 ";
    const gchar* audio_src = "osxaudiosrc device=100 ";
#endif

    std::string launch_pipeline =
        "( appsrc name=mysrc is-live=true block=true format=TIME "
        "caps=video/x-raw,format=BGR,width=640,height=480,framerate=30/1 "
        "! queue ! videoconvert ! video/x-raw,"+ std::string(vedio_convert_format) +
        "! x264enc tune=zerolatency bitrate=1000 speed-preset=ultrafast key-int-max=30 "
        "! rtph264pay name=pay0 pt=96 ) ( " +
        std::string(audio_src) +
        "! queue "
        "! audioconvert ! audioresample "
        "! audio/x-raw,rate=48000,channels=1 "
        "! volume name=mic_vol volume=1.0 mute=false "
        "! avenc_aac bitrate=128000 ! aacparse ! rtpmp4apay name=pay1 pt=97 )";
    gst_rtsp_media_factory_set_launch(factory, launch_pipeline.c_str());



    // std::string launch_pipeline =
    //     "( appsrc name=mysrc is-live=true block=true format=TIME "
    //     "caps=video/x-raw,format=BGR,width=640,height=480,framerate=30/1 "
    //     "! queue ! videoconvert ! video/x-raw,format=I420 "
    //     "! x264enc tune=zerolatency bitrate=500 speed-preset=ultrafast key-int-max=30 "
    //     "! rtph264pay name=pay0 pt=96 ) ( " +
    //     std::string(audio_src) +
    //     "! queue "
    //     "! audioconvert ! audioresample "
    //     "! audio/x-raw,rate=48000,channels=1 "
    //     "! volume name=mic_vol volume=1.0 mute=false "
    //     "! avenc_aac bitrate=128000 ! aacparse ! rtpmp4apay name=pay1 pt=97 )";
    // gst_rtsp_media_factory_set_launch(factory, launch_pipeline.c_str());

    // gst_rtsp_media_factory_set_launch(factory,
    //                                   "( appsrc name=mysrc is-live=true block=true format=TIME "
    //                                   "caps=video/x-raw,format=BGR,width=640,height=480,framerate=30/1 "
    //                                   "! queue "
    //                                   "! videoconvert ! video/x-raw,format=I420 "
    //                                   "! x264enc tune=zerolatency bitrate=500 speed-preset=ultrafast key-int-max=30 "
    //                                   "! rtph264pay name=pay0 pt=96 ) "
    //                                   "( wasapisrc low-latency=true "
    //                                   "! queue "
    //                                   "! audioconvert ! audioresample "
    //                                   "! audio/x-raw,rate=48000,channels=2 "
    //                                   "! volume name=mic_vol volume=1.0 mute=false "
    //                                   "! avenc_aac bitrate=128000 "
    //                                   "! aacparse "
    //                                   "! rtpmp4apay name=pay1 pt=97 )"
    //                                   );
    gst_rtsp_media_factory_set_shared(factory, TRUE);
    g_signal_connect(factory, "media-configure", G_CALLBACK(on_media_configure), nullptr);

    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);
    g_object_unref(mounts);
    gst_rtsp_server_attach(server, nullptr);

    const gchar* address = gst_rtsp_server_get_address(server);   // 기본값: 0.0.0.0
    const gchar* port = gst_rtsp_server_get_service(server);      // 기본값: 8554
    g_print("RTSP server ready: rtsp://%s:%s/test\n", address, port);

    GMainLoop* loop = g_main_loop_new(nullptr, FALSE);
    g_main_loop_run(loop);
}
