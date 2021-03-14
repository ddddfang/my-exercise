CURRENT_DIR := $(PWD)

obj-m := hello.o

src_obj := \
	./hello_drv.o
#demod_zx2000_obj := \
#	demod_zx2000/demod_zx2000.o \
#	demod_zx2000/demod_zx2000_hal.o

hello-objs := $(src_obj)
#hello-objs += $(demod_zx2000_obj)


EXTRA_CFLAGS := \
    -g -O0 \
	-I$(CURRENT_DIR)/ \
	#-I$(CURRENT_DIR)/demod_zx2000 \


LINUXDIR ?= /home/fang/桌面/qemu/qemu_fang/linux-4.19.168

all:
	make -C $(LINUXDIR) M=$(CURRENT_DIR) LDDINCDIR=$(CURRENT_DIR) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-

clean:
	make -C $(LINUXDIR) M=$(CURRENT_DIR) LDDINCDIR=$(CURRENT_DIR) clean ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
	find . -regex '.*\.o\|.*\.mod.c\|.*\.symvers\|.*\.o.c\|.*\.ko\|.*\.ko\|.*\.order\|.*\.ko.cmd\|.*\.o.cmd'|xargs rm -f
	rm -rf .tmp_versions


#LINUXDIR ?= /lib/modules/$(shell uname -r)/build
#
#all:
#	make -C $(LINUXDIR) M=$(CURRENT_DIR) LDDINCDIR=$(CURRENT_DIR) modules
#
#clean:
#	make -C $(LINUXDIR) M=$(CURRENT_DIR) LDDINCDIR=$(CURRENT_DIR) clean
#	find . -regex '.*\.o\|.*\.mod.c\|.*\.symvers\|.*\.o.c\|.*\.ko\|.*\.ko\|.*\.order\|.*\.ko.cmd\|.*\.o.cmd'|xargs rm -f
#	rm -rf .tmp_versions
