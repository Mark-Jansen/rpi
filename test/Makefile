
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/el32-buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: test adc_test gyro_test

.PHONY: all, clean

test: test.cpp
	$(CROSS_COMPILE)g++ test.cpp -o ../build/test -I../modules

adc_test: adc_test.cpp
	$(CROSS_COMPILE)g++ adc_test.cpp -o ../build/adc_test -I../modules

gyro_test: gyro_test.cpp
	$(CROSS_COMPILE)g++ gyro_test.cpp -o ../build/gyro_test -I../modules

clean:
	rm -f *.o ../build/test ../build/adc_test ../build/gyro_test

