#include "ST/SCServo.h"
#include "st_dev.h"
#include <queue>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <unistd.h>
#include <string.h>

#define JOINT_NUMBER 6
typedef struct st_device {
	SMS_STS		sm_st;
	std::mutex	mtx;
	std::thread	tid;
	bool		b_exit;
	std::queue<std::vector<int>> cmd_queue;
	uint8_t		id[JOINT_NUMBER];
	uint16_t	speed[JOINT_NUMBER];
	uint8_t		acc[JOINT_NUMBER];
} st_dev_t;

static st_dev_t *st_dev = nullptr;

static void st_device_cmd_proc()
{
	while(!st_dev->b_exit) {
		usleep(40 * 1000);

		std::vector<int> angles;
		{	
			std::lock_guard<std::mutex> lg(st_dev->mtx);
			if (st_dev->cmd_queue.size() > 0) {
				angles = st_dev->cmd_queue.front();
				st_dev->cmd_queue.pop();		
			}
		}

		if (angles.size() != JOINT_NUMBER) {
			continue;
		}
		
		continue;
		int16_t pos[JOINT_NUMBER];
		for (int i = 0; i < JOINT_NUMBER; i++) {
			pos[i] = angles[i];
		}

		st_dev->sm_st.SyncWritePosEx(st_dev->id, sizeof(st_dev->id), pos, st_dev->speed, st_dev->acc);
	}

	printf("thread exit.\n");
}

int st_device_init(std::string dev_name)
{
	st_dev = new st_dev_t;
	memset(st_dev, 0, sizeof(st_dev_t));

	for (int i = 0; i < JOINT_NUMBER; i++) {
		st_dev->id[i] = i + 1;
		st_dev->speed[i] = 400;
		st_dev->acc[i] = 50;
	}

	if (!st_dev->sm_st.begin(1000000, dev_name.c_str())) {
		return 1;
	}

	st_dev->b_exit = false;
	
	st_dev->tid = std::thread(st_device_cmd_proc);
	return 0;
}

int st_device_ctl(std::vector<int> angles)
{
	if (!st_dev)
		return 0;

	if (st_dev->b_exit) {
		return 0;
	}

	std::lock_guard<std::mutex> lg(st_dev->mtx);
	st_dev->cmd_queue.push(angles);
	return 0;
}

void st_device_final()
{
	st_dev->b_exit = true;

	usleep(1000 * 500);

	if (st_dev->tid.joinable()) {
		st_dev->tid.join();
	}

	st_dev->sm_st.end();

	delete st_dev;
}
