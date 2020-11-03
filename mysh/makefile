MODULE_BIN = myShell
MODULE_OBJS += $(patsubst %.c,%.o ,$(wildcard ./*.c))
CC = gcc
OUT_DIR = .
PUBLIC_FLAGS:=-g -O0 -Wall
LDFLAGS = -lm -lz -lpthread
MODULE_INCLUDE = -I.

.PHONY:all clean

%.o: %.c
	$(CC) $(PUBLIC_FLAGS) $(MODULE_INCLUDE) -c $< -o $@

all : clean $(MODULE_OBJS)
	$(CC) $(MODULE_OBJS) $(PUBLIC_FLAGS) $(LDFLAGS) -o $(OUT_DIR)/$(MODULE_BIN)

clean:
	-rm -f $(OUT_DIR)/$(MODULE_BIN)
	-rm -f $(MODULE_OBJS)
