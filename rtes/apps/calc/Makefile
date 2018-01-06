CC=arm-linux-androideabi-gcc
SYSROOT=$(NDK)/platforms/$(NDK_API)/arch-arm

calc: calc.c
	$(CC) --sysroot=$(SYSROOT) calc.c -o calc
clean:
	rm calc
