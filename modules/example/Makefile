
DEBUG = n
MODULE_NAME = example

CROSS_COMPILE=$(HOME)/el32-buildroot/output/host/usr/bin/arm-linux-
KDIR:= $(HOME)/el32-buildroot/output/build/linux-rpi-3.6.y
PWD := $(shell pwd)

ARCH=arm
export CROSS_CC=$(CROSS_COMPILE)gcc
CC=$(CROSS_CC)

EXTRA_CFLAGS += -DEXPORT_SYMTAB
obj-m += $(MODULE_NAME).o

all:
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) SUBDIRS=$(PWD) clean
