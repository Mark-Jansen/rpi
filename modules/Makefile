
# add each new target to 'TARGETS'
<<<<<<< HEAD
=======

>>>>>>> 5ef1ffc02771326e2a35de72a119c917f935b977
# order is important, if battery depends on adc, adc should be first!
<<<<<<< HEAD
TARGETS = adc \ battery \ gpio \ pwm \ motor_driver
=======
TARGETS = adc \
		battery \
		gpio \
		led \
<<<<<<< HEAD
		ultrasoon
>>>>>>> master
=======
		ultrasoon \
                pwm
>>>>>>> 5ef1ffc02771326e2a35de72a119c917f935b977

PREFIX = $(shell echo `pwd`)/../build/modules/

.PHONY: all clean

all:
	@for T in $(TARGETS); do \
		cp $(PREFIX)/Module.symvers $$T; \
		${MAKE} -s -C $$T;  \
		${MAKE} -s -C $$T PREFIX=$(PREFIX) install; \
		cp $$T/Module.symvers $(PREFIX); \
	done;

clean:
	@rm -f $(PREFIX)/Module.symvers; \
	for T in $(TARGETS); do  \
		${MAKE} -s -C $$T clean; \
		rm -f $(PREFIX)/$$T.ko; \
	done
