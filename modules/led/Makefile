
MODULE_NAME = led

# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-
KDIR:= $(L_BUILDROOT)/output/build/linux-rpi-3.6.y

ARCH=arm
export CROSS_CC=$(CROSS_COMPILE)gcc

CC=$(CROSS_CC)

PWD:=$(shell pwd)
EXTRA_CFLAGS += -DEXPORT_SYMTAB
obj-m += $(MODULE_NAME).o


all:
	$(MAKE) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) SUBDIRS=$(PWD) clean

install:
	install -m 644 $(MODULE_NAME).ko $(PREFIX)/


