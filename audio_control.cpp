#include "audio_control.h"
#include <mutex>
#include <iostream>

static GstElement* g_mic_vol = nullptr;
static std::mutex g_mic_vol_mutex;

void set_mic_enabled(bool enable) {
    std::lock_guard<std::mutex> lock(g_mic_vol_mutex);
    if (!g_mic_vol) {
        g_printerr("set_mic_enabled: mic_vol not ready yet.\n");
        return;
    }
    g_object_set(g_mic_vol, "mute", enable ? FALSE : TRUE, NULL);
    g_print("[MIC] %s\n", enable ? "ON" : "OFF (muted)");
}

void set_mic_volume(double volume) {
    if (volume < 0.0) volume = 0.0;
    if (volume > 1.0) volume = 1.0;

    std::lock_guard<std::mutex> lock(g_mic_vol_mutex);
    if (!g_mic_vol) {
        g_printerr("set_mic_volume: mic_vol not ready yet.\n");
        return;
    }
    g_object_set(g_mic_vol, "volume", volume, NULL);
    g_print("[MIC] Volume set to %.2f\n", volume);
}

void audio_on_media_prepared(GstRTSPMedia* media, gpointer) {
    GstElement* element = gst_rtsp_media_get_element(media);
    if (!element) return;

    std::lock_guard<std::mutex> lock(g_mic_vol_mutex);
    if (!g_mic_vol) {
        g_mic_vol = gst_bin_get_by_name_recurse_up(GST_BIN(element), "mic_vol");
        if (g_mic_vol) {
            gst_object_ref(g_mic_vol);
            g_print("mic_vol element captured.\n");
        } else {
            g_printerr("mic_vol element not found!\n");
        }
    }
    gst_object_unref(element);
}
