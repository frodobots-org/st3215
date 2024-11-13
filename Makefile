#/*
# * Copyright 2023 Ethan. All rights reserved.
# */

EXEC := app
TARGET_LDLIBS = -lpthread
LIB :=
DIR := .
SRC := .
CPP_SOURCES := \
	./src/SCS.cpp \
	./src/SCSCL.cpp \
	./src/SCSerial.cpp \
	./src/SMS_STS.cpp \
	./src/SMSBL.cpp \
	./src/SMSCL.cpp \
	./main.cpp

INC := -I \
	./inc

TARGET_CFLAGS :=  \
	$(INC)

TARGET_CXXFLAGS :=  \
	$(INC)

.PHONY: make build inc src

-include $(SRC)/make/defs.mk

-include $(SRC)/make/cc.mk
