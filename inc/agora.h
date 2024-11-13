/*
 * Copyright 2023 Frodobots.ai. All rights reserved.
 */
#ifndef __AGORA_H__
#define __AGORA_H__
#include <string>

typedef void (*agora_connnected_cb_t)(int conn_id);

int agora_init(std::string room);

void agora_final();

#endif /*__AGORA_H__*/
