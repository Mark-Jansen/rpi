
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

.PHONY: all clean

all:
	$(CROSS_COMPILE)g++ gpio_driver_test.cpp -o gpio_driver_test -I../modules

clean:
	rm -rf *.o gpio_driver_test

