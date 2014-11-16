
# add each new target to 'TARGETS'
TARGETS = example gpio
PREFIX = $(shell echo `pwd`)/../build/modules/

.PHONY: all clean

all:
	for T in $(TARGETS); do  \
		${MAKE} -C $$T;  \
		${MAKE} -C $$T PREFIX=$(PREFIX) install;  \
	done;

clean:
	for T in $(TARGETS); do  \
		${MAKE} -C $$T clean; \
	done
