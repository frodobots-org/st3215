/*
 * Copyright 2023 Frodobots.ai. All rights reserved.
 */

#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "SCServo.h"

volatile static bool b_exit = false;

static void signal_handler(int sig)
{
	b_exit = true;
}

uint8_t ID[6] = {1, 2, 3, 4, 5, 6};
int16_t Position[6];
uint16_t Speed[6] = {400, 400, 400, 400, 400, 400};
uint8_t ACC[6] = {50, 50, 50, 50, 50, 50};


int main(int argc, const char *argv[]) 
{
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

	while (b_exit) {
		usleep(1000 * 1000);
	}

	sm_st.end();
	return ret;
}
