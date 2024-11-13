#/*
# * Copyright 2023 Ethan. All rights reserved.
# */

EXEC := app
TARGET_LDLIBS = -lpthread
LIB :=
DIR := .
SRC := .
CPP_SOURCES := \
	./src/ST/SCS.cpp \
	./src/ST/SCSCL.cpp \
	./src/ST/SCSerial.cpp \
	./src/ST/SMS_STS.cpp \
	./src/ST/SMSBL.cpp \
	./src/ST/SMSCL.cpp \
	./src/hal_stream.cpp \
	./main.cpp

INC := -I \
	./inc

TARGET_CFLAGS :=  \
	$(INC)

TARGET_CXXFLAGS :=  \
	$(INC) \
	-Wall $(shell pkg-config --cflags gstreamer-1.0)

TARGET_LDFLAGS := $(shell pkg-config --libs gstreamer-1.0)

.PHONY: make build inc src

-include $(SRC)/make/defs.mk

-include $(SRC)/make/cc.mk
