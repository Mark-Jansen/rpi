
TARGETS = modules test

.PHONY: all clean

all:
	for T in $(TARGETS); do  \
		cd $$T; ${MAKE};  \
	done;

clean:
	for T in $(TARGETS); do  \
		cd $$T; ${MAKE} clean;  \
	done

