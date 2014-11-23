
# add each new target to 'TARGETS'
# order is important, if battery depends on adc, adc should be first!
TARGETS = adc battery gpio ultrasoon
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
