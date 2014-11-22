
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/el32-buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

.PHONY: all clean

all:
	$(CROSS_COMPILE)g++ test.cpp -o test -I../modules
	$(CROSS_COMPILE)g++ testpwm.cpp -o testpwm -I../modules

clean:
	rm -rf *.o test testpwm 

