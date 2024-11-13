/*
 * Copyright 2023 Frodobots.ai . All rights reserved.
 */

#ifndef __HAL_STREAM_H__
#define __HAL_STREAM_H__

#include "hal_media.h"

typedef void (*hal_frame_cb_t)(int ch, hal_frame_t *frame, const void *ctx);

int media_device_init(hal_frame_cb_t cb);

void meida_device_final();

int media_device_start(int ch, const void *ctx);

int media_device_stop(int ch);

int media_device_stop_all();
#endif /*__HAL_STREAM_H__*/

