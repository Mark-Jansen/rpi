
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/el32-buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: generic balancer

.PHONY: generic all clean

generic: generic/*.cpp
	@echo "Building generic"; \
	cd generic; \
	$(CROSS_COMPILE)g++ -c *.cpp -I../modules -std=c++11; \
	cd ..; \
	ar -cq libgeneric.a generic/*.o;

balancer: main.cpp Gyro.cpp Led.cpp Ultrasoon.cpp Motor.cpp Battery.cpp Ultrasoon_controller.cpp generic
	@echo "Building balancer"; \
	$(CROSS_COMPILE)g++ main.cpp Gyro.cpp libgeneric.a -o ../build/balancer -I../modules -std=c++11 -lpthread

clean:
	@echo "Cleaning generic / balancer"; \
	rm -f *.o generic/*.o libgeneric.a ../build/balancer

