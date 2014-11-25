
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
<<<<<<< HEAD
#ifeq "$(BUILDROOT)" ""
#	L_BUILDROOT:=$(HOME)/el32-buildroot/
#else
#	L_BUILDROOT:=$(BUILDROOT)
#endif
L_BUILDROOT:=$(HOME)/buildroot/
CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: motor_driver_test

.PHONY: all, clean

motor_driver_test: motor_driver_test.cpp
	$(CROSS_COMPILE)g++ motor_driver_test.cpp -o ../build/motor_driver_test_RPI -I../modules
=======
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
>>>>>>> master

clean:
	rm -f *.o ../build/motor_driver_test_RPI
