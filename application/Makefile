
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
	@cd generic; \
	$(CROSS_COMPILE)g++ -c *.cpp -I../modules -std=c++11; \
	cd ..; \
	ar -cvq libgeneric.a generic/*.o;

balancer: main.cpp Gyro.cpp generic
	$(CROSS_COMPILE)g++ main.cpp Gyro.cpp libgeneric.a -o ../build/balancer -I../modules -std=c++11 -lpthread

clean:
	rm -f *.o generic/*.o libgeneric.a ../build/balancer

