CC=arm-linux-androideabi-gcc
SYSROOT=$(NDK)/platforms/$(NDK_API)/arch-arm

rtps: rtps.c
	$(CC) --sysroot=$(SYSROOT) rtps.c -o rtps
clean:
	rm rtps
