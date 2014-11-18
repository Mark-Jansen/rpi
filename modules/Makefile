
# add each new target to 'TARGETS'
# order is important, if battery depends on adc, adc should be first!
<<<<<<< HEAD
TARGETS = gpio pwm motor_driver led
=======
TARGETS = example adc battery
>>>>>>> 4aaffe8147fadc0cf2b37a3df12bf171cfc960ee
PREFIX = $(shell echo `pwd`)/../build/modules/

.PHONY: all clean

all:
	for T in $(TARGETS); do \
		cp $(PREFIX)/Module.symvers $$T; \
		${MAKE} -C $$T;  \
		${MAKE} -C $$T PREFIX=$(PREFIX) install; \
		cp $$T/Module.symvers $(PREFIX); \
	done;

clean:
	rm $(PREFIX)/Module.symvers; \
	for T in $(TARGETS); do  \
		${MAKE} -C $$T clean; \
	done
