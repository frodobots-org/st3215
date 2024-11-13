#/*
# * Copyright 2023 Ethan. All rights reserved.
# */

#
# Build type, defualt is RELEASE
#
TYPE ?= RELEASE

ARCH ?= x86
BUILD_DIR_PREFIX ?= $(SRC)/build

ifeq ($(ARCH), x86)
    #Linux x86_64
    DEFINES += LINUX_X86_64
    export CC=gcc
    export CXX=g++
    export AR=ar
endif

ifneq ($(TOOLCHAIN_DIR),)
	TOOLCHAIN_DIR := $(abspath $(TOOLCHAIN_DIR))
	TOOL_DIR = $(TOOLCHAIN_DIR)/bin/
	TARGET_CFLAGS += -I$(TOOLCHAIN_DIR)/include
	TARGET_LDFLAGS += -L$(TOOLCHAIN_DIR)/lib
endif

ifneq ($(ARCH), x86)
	export ARCH
	export TOOL_ROOT = $(TOOL_DIR)$(ARCH)
	export CC = $(TOOL_ROOT)-gcc
	export CXX = $(TOOL_ROOT)-g++
	export AR = $(TOOL_ROOT)-ar
endif

#
# Install directory
#
# INSTALL_ROOT can be defined externally to
# customize the make install behavior
#
INSTALL_ROOT ?= $(BUILD_DIR_PREFIX)/$(ARCH)
INSTALL_ROOT := $(abspath $(INSTALL_ROOT))

INSTALL := install -D

#
# Build directories
#
BUILD_ROOT = $(BUILD_DIR_PREFIX)/$(ARCH)/obj
BUILD = $(BUILD_ROOT)/$(DIR)

DEFINES +=

ifeq ($(TYPE),RELEASE)
DEFINES += NDEBUG
TARGET_CFLAGS += -O2
TARGET_CXXFLAGS += -O2
else
TARGET_CFLAGS += -O0 -g -ggdb
TARGET_CXXFLAGS += -O0 -g -ggdb
endif

TARGET_CFLAGS += -Wall -std=gnu99
TARGET_CXXFLAGS += -Wall -std=c++11

#
# Build files
#
OBJS = $(SOURCES:%.c=$(BUILD)/%.o)
OBJS += $(CPP_SOURCES:%.cpp=$(BUILD)/%.o)

#
# SW version tracking: define pre-processor variables
#
include $(SRC)/version.mk

BUILD_DATE := $(shell date '+%Y-%m-%d')
BUILD_TIME := $(shell date '+%H:%M:%S')
BUILD_USER := $(USER)
ifeq ($(TYPE),RELEASE)
DEFINES += BUILD_RELEASE
else
BUILD_TAG := eng
endif

BUILD_VERSION := $(MAJOR_VERSION).$(MINOR_VERSION)
ifneq ($(MAINTENANCE_VERSION),)
BUILD_VERSION := $(BUILD_VERSION).$(MAINTENANCE_VERSION)
endif
ifneq ($(BUILD_TAG),)
BUILD_VERSION := $(BUILD_VERSION)-$(BUILD_TAG)
endif

DEFINES += BUILD_VERSION=\"$(BUILD_VERSION)\"
DEFINES += BUILD_DATE=\"$(BUILD_DATE)\"
DEFINES += BUILD_TIME=\"$(BUILD_TIME)\"
DEFINES += BUILD_USER=\"$(BUILD_USER)\"

#Build Product ID
ifneq ($(PRODUCT_ID),)
DEFINES += PRODUCT_ID=\"$(PRODUCT_ID)\"
endif

#Macro defs.
TARGET_CFLAGS += $(addprefix -D,$(sort $(DEFINES)))
TARGET_CXXFLAGS += -Wall -std=c++11
TARGET_CXXFLAGS += $(addprefix -D,$(sort $(DEFINES)))

#
# Override common implicit variables to preserve defined values
#
override CFLAGS += $(TARGET_CFLAGS)
override CXXFLAGS += $(TARGET_CXXFLAGS)
override LDFLAGS += $(TARGET_LDFLAGS)
override LDLIBS += $(TARGET_LDLIBS)
