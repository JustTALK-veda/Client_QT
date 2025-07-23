#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#pragma push_macro("signals")
#undef signals

#include <glib.h>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#pragma pop_macro("signals")

#include <opencv2/opencv.hpp>
#include <QCoreApplication>
#include <QWidget>

extern std::atomic<bool> running;
extern cv::Mat* shared_frame_ptr;
extern std::mutex frame_mutex;
extern cv::Mat img_camoff;

void enable_streaming(bool enabled);
void push_frame_loop(GstElement* appsrc);
void on_media_configure(GstRTSPMediaFactory* factory, GstRTSPMedia* media, gpointer user_data);
void start_rtsp_server();


#endif // RTSPSERVER_H
