
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/el32-buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: adc_test gyro_test

.PHONY: all, clean

adc_test: adc_test.cpp
	@echo "Building adc_test"; \
	$(CROSS_COMPILE)g++ adc_test.cpp ../application/generic/File.cpp -o ../build/adc_test -I../modules -I../application -std=c++11

gyro_test: gyro_test.cpp
	@echo "Building gyro_test"; \
	$(CROSS_COMPILE)g++ gyro_test.cpp ../application/generic/File.cpp -o ../build/gyro_test -I../modules -I../application -lpthread -std=c++11

clean:
	rm -f *.o ../build/adc_test ../build/gyro_test

