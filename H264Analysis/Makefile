MODULE_BIN = decode
MODULE_OBJS += $(patsubst %.c, %.o, $(wildcard ./%.c))
MODULE_OBJS += decode.o nalu.o stream.o parset.o slice.o header.o

OUT_DIR = .

PUBLIC_FLAGS = -g -O0 -Wall -DTRACE=1
LDFLAGS = -lm -lz -lpthread
MODULE_INC = -I.

.PHONY: all clean

%.o : %.c
	gcc $(PUBLIC_FLAGS) $(MODULE_INC) -c $< -o $@

all : clean $(MODULE_OBJS)
	gcc $(MODULE_OBJS) $(PUBLIC_FLAGS) $(LDFLAGS) -o $(OUT_DIR)/$(MODULE_BIN)

clean:
	-rm -f $(OUT_DIR)/$(MODULE_BIN)
	-rm -f $(MODULE_OBJS)
