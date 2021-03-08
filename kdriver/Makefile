CURRENT_DIR := $(strip $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST)))))
ifeq ($(CURRENT_DIR),.)
CURRENT_DIR := $(PWD)
endif

obj-m := zx_fe.o

src_obj := \
	src/zx_fe_drv.o \
	src/zx_fe_mgr.o

demod_zx2000_obj := \
	demod_zx2000/demod_zx2000.o \
	demod_zx2000/demod_zx2000_hal.o

tuner_nxp_TDA18250B_obj := \
	tuner_nxp_TDA18250B/src/tmbslTDA18250A.o \
	tuner_nxp_TDA18250B/src/tmbslTDA18250A_Advanced.o \
	tuner_nxp_TDA18250B/src/tuner_nxp_TDA18250B.o

zx_fe-objs := $(src_obj)
zx_fe-objs += $(demod_zx2000_obj)
zx_fe-objs += $(tuner_nxp_TDA18250B_obj)

EXTRA_CFLAGS := \
	-I$(CURRENT_DIR)/include \
	-I$(CURRENT_DIR)/demod_zx2000 \
	-I$(CURRENT_DIR)/tuner_nxp_TDA18250B/include \

LINUXDIR ?= $(CURRENT_DIR)/../../../kernel

all:
	make -C $(LINUXDIR) M=$(CURRENT_DIR) LDDINCDIR=$(CURRENT_DIR) modules ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-

clean:
	find . -regex '.*\.o\|.*\.mod.c\|.*\.symvers\|.*\.o.c\|.*\.ko\|.*\.ko\|.*\.order\|.*\.ko.cmd\|.*\.o.cmd'|xargs rm -f
	rm -rf .tmp_versions

