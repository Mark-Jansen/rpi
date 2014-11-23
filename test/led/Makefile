
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: test adc_test test_ultrasoon

<<<<<<< HEAD:test/led/Makefile
all:
	$(CROSS_COMPILE)g++ led_driver_test.cpp -o led_driver_test -I../modules

clean:
	rm -rf *.o led_driver_test
=======
.PHONY: all, clean

test: test.cpp
	$(CROSS_COMPILE)g++ test.cpp -o ../build/test -I../modules

adc_test: adc_test.cpp
	$(CROSS_COMPILE)g++ adc_test.cpp -o ../build/adc_test -I../modules

clean:
	rm -f *.o ../build/test ../build/adc_test
>>>>>>> 4aaffe8147fadc0cf2b37a3df12bf171cfc960ee:test/Makefile

