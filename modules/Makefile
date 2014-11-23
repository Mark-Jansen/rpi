
# add each new target to 'TARGETS'
# order is important, if battery depends on adc, adc should be first!
TARGETS = adc \
		battery \
		gyro

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
