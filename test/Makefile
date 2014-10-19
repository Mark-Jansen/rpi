
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/el32-buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: test i2c_adc

.PHONY: all, clean

test: test.cpp
	$(CROSS_COMPILE)g++ test.cpp -o ../build/test -I../modules

i2c_adc: i2c_adc.cpp
	$(CROSS_COMPILE)g++ i2c_adc.cpp -o ../build/i2c_adc -I../modules

clean:
	rm -f *.o ../build/test ../build/i2c_adc

