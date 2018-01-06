CC=arm-linux-androideabi-gcc
SYSROOT=$(NDK)/platforms/$(NDK_API)/arch-arm

hello: sloppyapp1.c
	$(CC) --sysroot=$(SYSROOT) sloppyapp1.c -o sloppyapp1
clean:
	rm sloppyapp1
