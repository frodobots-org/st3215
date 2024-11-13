/*
 * Copyright 2023 Frodobots.ai. All rights reserved.
 */

#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include "ST/SCServo.h"
#include "hal_stream.h"
#include "agora.h"

volatile static bool b_exit = false;

static void signal_handler(int sig)
{
	b_exit = true;
}

static void hal_frame_cb(int ch, hal_frame_t *frame, const void *ctx)
{
	printf("hal_frame_cb len[%d] is_key[%d]\n", frame->m_len, frame->m_frame_type);

	agora_frame_send(ch + 1, frame);
}

static void agora_msg_cb(const char *msg, int msg_len)
{
	printf("agora_msg_cb msg[%s] len[%d]\n", msg, msg_len);
}

static void agora_conn_cb(int uid)
{
	printf("agora_conn_cb uid[%d]\n", uid);
	media_device_start(uid % 1000, NULL);
}

int main(int argc, const char *argv[]) 
{
	int ret = 0;
	if (argc < 2) {
		printf("argc error! Please provide the serial port as an argument.\n");
		return 1;
	}

	signal(SIGINT, signal_handler);

	std::cout << "Serial: " << argv[1] << std::endl;

	media_device_init(hal_frame_cb);
	
	agora_init(argv[2], agora_conn_cb, agora_msg_cb);

	while (!b_exit) {
		usleep(1000 * 1000);
	}

	agora_final();

	return ret;
}

#if 0
uint8_t ID[6] = {1, 2, 3, 4, 5, 6};
int16_t Position[6];
uint16_t Speed[6] = {400, 400, 400, 400, 400, 400};
uint8_t ACC[6] = {50, 50, 50, 50, 50, 50};

int main(int argc, const char *argv[]) 
{
	uint8_t rxPacket[4];
	int ret = 0;
	if (argc < 2) {
		printf("argc error! Please provide the serial port as an argument.\n");
		return 1;
	}

	signal(SIGINT, signal_handler);

	std::cout << "Serial: " << argv[1] << std::endl;

	SMS_STS sm_st;
	if (!sm_st.begin(1000000, argv[1])) {
		return 1;
	}

	sm_st.SyncWritePosEx(ID, sizeof(ID), Position, Speed, ACC);

	sm_st.syncReadBegin(sizeof(ID), sizeof(rxPacket));
	while (!b_exit) {
		usleep(1000 * 1000);
		
		printf("sync read...\n");

		sm_st.syncReadPacketTx(ID, sizeof(ID), SMS_STS_PRESENT_POSITION_L,
				sizeof(rxPacket)); // Sync read command packet send

		for (uint8_t i = 0; i < 6; i++) {
			if (!sm_st.syncReadPacketRx(ID[i], rxPacket)) {
				printf("ID: %d sync read error!\n", (int)ID[i]);
				continue; // Failed to receive or decode
			}

			int16_t Positionx = sm_st.syncReadRxPacketToWrod(15); // Decode position
			int16_t Speedx = sm_st.syncReadRxPacketToWrod(15);    // Decode speed
			printf("Pose[%d] speed[%d]\n", Positionx, Speedx);
		}

	}

	sm_st.syncReadEnd();
	sm_st.end();
	return ret;
}
#endif
