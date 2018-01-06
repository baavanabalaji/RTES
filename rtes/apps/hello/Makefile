CC=arm-linux-androideabi-gcc
SYSROOT=$(NDK)/platforms/$(NDK_API)/arch-arm

hello: hello.c
	$(CC) --sysroot=$(SYSROOT) hello.c -o hello
clean:
	rm hello
