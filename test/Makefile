
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: adc_test test_ultra gyro_test testpwm motor_driver_test

.PHONY: all, clean

adc_test: adc_test.cpp
	@echo "Building adc_test"; \
	$(CROSS_COMPILE)g++ adc_test.cpp ../application/generic/File.cpp -o ../build/adc_test -I../modules -I../application -std=c++11
	
test_ultra: test_ultrasoon.cpp
	@echo "Building test_ultrasoon"; \
	$(CROSS_COMPILE)g++ test_ultrasoon.cpp -o ../build/test_ultrasoon -I../modules

gyro_test: gyro_test.cpp
	@echo "Building gyro_test"; \
	$(CROSS_COMPILE)g++ gyro_test.cpp ../application/generic/File.cpp -o ../build/gyro_test -I../modules -I../application -lpthread -std=c++11

testpwm: testpwm.cpp
	@echo "Building testpwm"; \
	$(CROSS_COMPILE)g++ testpwm.cpp -o ../build/testpwm -I../modules

motor_driver_test: motor_driver_test.cpp
	@echo "Building motor_driver_test"; \
	$(CROSS_COMPILE)g++ motor_driver_test.cpp -o ../build/motor_driver_test -I../modules

clean:
	@echo "Cleaning test"; \
	rm -f *.o ../build/adc_test ../build/testpwm ../build/gyro_test ../build/test_ultrasoon ../build/motor_driver_test

