CC=arm-linux-androideabi-gcc
SYSROOT=$(NDK)/platforms/$(NDK_API)/arch-arm

helloworld: helloworld.c
	$(CC) --sysroot=$(SYSROOT) helloworld.c -o helloworld
clean:
	rm helloworld
