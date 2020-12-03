
CONFIG_GDB = y
CONFIG_DEBUG = y

MODULE_BIN = mysh

#MODULE_SRCS = $(shell find . -name "*.c")
#MODULE_OBJS += $(patsubst %.c,%.o ,$(MODULE_SRCS))
MODULE_OBJS += $(patsubst %.c,%.o ,$(wildcard ./shell/*.c))
MODULE_OBJS += $(patsubst %.c,%.o ,$(wildcard ./fs/*.c))
MODULE_OBJS += $(patsubst %.c,%.o ,$(wildcard ./common/*.c))
MODULE_OBJS += main.o

CC = gcc
CFLAGS:= -Wall -I./
LDFLAGS = -lm -lz -lpthread

ifeq ($(CONFIG_GDB), y)
    CFLAGS += -g -O0
endif

ifeq ($(CONFIG_DEBUG), y)
    CFLAGS += -DDEBUG
endif

.PHONY:all clean

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all : clean $(MODULE_OBJS)
	$(CC) $(MODULE_OBJS) $(LDFLAGS) -o $(MODULE_BIN)

clean:
	-rm -f $(MODULE_BIN)
	-rm -f $(MODULE_OBJS)
