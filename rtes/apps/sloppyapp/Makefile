CC=arm-linux-androideabi-gcc
SYSROOT=$(NDK)/platforms/$(NDK_API)/arch-arm

hello: sloppyapp.c
	$(CC) --sysroot=$(SYSROOT) sloppyapp.c -o sloppyapp
clean:
	rm sloppyapp
