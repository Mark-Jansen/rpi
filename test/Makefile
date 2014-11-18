# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/buildroot
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: motor_driver_test

.PHONY: all, clean

motor_driver_test: motor_driver_test.cpp
	$(CROSS_COMPILE)g++ motor_driver_test.cpp -o motor_driver_test_RPI -I../modules	
	
clean:
	
	rm motor_driver_test
