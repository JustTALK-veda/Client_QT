#ifndef AUDIO_CONTROL_H
#define AUDIO_CONTROL_H

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-media.h>

// 마이크 볼륨 및 mute 제어
void set_mic_enabled(bool enable);
void set_mic_volume(double volume);  // 0.0 ~ 1.0

// 내부적으로 mic_vol 포인터를 세팅하기 위한 콜백
void audio_on_media_prepared(GstRTSPMedia* media, gpointer user_data);

#endif // AUDIO_CONTROL_H
