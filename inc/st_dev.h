/*
 * Copyright 2023 Ethan. All rights reserved.
 */
#ifndef _ST_DEV_H__
#define __ST_DEV_H__

#include <vector>
#include <string>

int st_device_init(std::string dev_name);

int st_device_ctl(std::vector<int> angles);

void st_device_final();
#endif /*__ST_DEV_H__*/
