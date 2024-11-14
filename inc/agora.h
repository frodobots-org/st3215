/*
 * Copyright 2023 Frodobots.ai. All rights reserved.
 */
#ifndef __AGORA_H__
#define __AGORA_H__
#include <string>
#include "hal_media.h"

typedef void (*agora_connnected_cb_t)(int uid);
typedef void (*agora_msg_cb_t)(const char *msg, int msg_len);

int agora_init(std::string room, agora_connnected_cb_t ccb, agora_msg_cb_t mcb);

void agora_final();

int agora_frame_send(int conn_id, const hal_frame_t *frame);

#endif /*__AGORA_H__*/
