
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/el32-buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: adc_test test_ultra

.PHONY: all, clean

adc_test: adc_test.cpp
	$(CROSS_COMPILE)g++ adc_test.cpp -o ../build/adc_test -I../modules
	
test_ultra: test_ultrasoon.cpp
	$(CROSS_COMPILE)g++ test_ultrasoon.cpp -o ../build/test_ultrasoon -I../modules

clean:
	rm -f *.o ../build/adc_test

