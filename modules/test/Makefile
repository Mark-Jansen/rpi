
CROSS_COMPILE=$(HOME)/el32-buildroot/output/host/usr/bin/arm-linux-
export CROSS_CC=$(CROSS_COMPILE)gcc
CC=$(CROSS_CC)

all:
	$(CC) test.c -o test

clean:
	rm -rf *.o test
