
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/el32-buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: balancer

.PHONY: all, clean

balancer: main.cpp
	$(CROSS_COMPILE)g++ main.cpp -o ../build/balancer -I../modules

clean:
	rm -f *.o ../build/balancer

