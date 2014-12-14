
# try to use the environment variable of BUILDROOT if it's defined,
# otherwise use a hardcoded path
ifeq "$(BUILDROOT)" ""
	L_BUILDROOT:=$(HOME)/el32-buildroot/
else
	L_BUILDROOT:=$(BUILDROOT)
endif

CROSS_COMPILE:=$(L_BUILDROOT)/output/host/usr/bin/arm-linux-

all: generic hardware balancer

.PHONY: generic hardware all clean

generic: generic/*.cpp
	@echo "Building generic"; \
	cd generic; \
	$(CROSS_COMPILE)g++ -c *.cpp -I../modules -std=c++11; \
	cd ..; \
	ar -cq libgeneric.a generic/*.o;

hardware: hardware/*.cpp
	@echo "Building hardware"; \
	cd hardware; \
	$(CROSS_COMPILE)g++ -c *.cpp -I../../modules -I.. -std=c++11; \
	cd ..; \
	ar -cq libhardware.a hardware/*.o;
	

balancer: main.cpp Ultrasoon_controller.cpp hardware generic
	@echo "Building balancer"; \
	$(CROSS_COMPILE)g++ main.cpp libhardware.a libgeneric.a -o ../build/balancer -I../modules -I. -std=c++11 -lpthread

clean:
	@echo "Cleaning generic / balancer"; \
	rm -f *.o generic/*.o libgeneric.a libhardware.a ../build/balancer

