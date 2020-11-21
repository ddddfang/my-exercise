
CONFIG_DEBUG = y
CONFIG_DEBUG_ETH = y

MODULE_BIN = myTcpip

MODULE_SRCS = $(shell find . -name "*.c")
MODULE_OBJS += $(patsubst %.c,%.o ,$(MODULE_SRCS))
CC = gcc
CFLAGS:= -Wall -I./include
LDFLAGS = -lm -lz -lpthread

ifeq ($(CONFIG_DEBUG), y)
    CFLAGS += -g -O0
endif

ifeq ($(CONFIG_DEBUG_ETH), y)
    CFLAGS += -DDEBUG_ETH
endif


.PHONY:all clean

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all : clean $(MODULE_OBJS)
	$(CC) $(MODULE_OBJS) $(LDFLAGS) -o $(MODULE_BIN)

clean:
	-rm -f $(MODULE_BIN)
	-rm -f $(MODULE_OBJS)
