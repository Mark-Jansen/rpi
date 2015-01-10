
TARGETS = modules \
		application \
		test

.PHONY: all \
		clean

all:
	@for T in $(TARGETS); do  \
		${MAKE} -s -C $$T; \
	done;

clean:
	@for T in $(TARGETS); do  \
		${MAKE} -s -C $$T clean; \
	done

