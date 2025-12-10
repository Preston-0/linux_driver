# All object files that are behind "obj-m" will be built as kernel modules.
# The compilation from waitqueue.c to waitqueue.o is done automatically by the make file's Linux kernel headers.
obj-m += waitqueue.o

# Default target. Calls a make file.
# "shell uname -r" gets the version number of the currently-running kernel.
# "build" is where the kernel headers are located.
# "M=$(PWD)" means the make file should look in the current path for buildable kernel modules.
# "modules" is the target, which will build the kernel modules.
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# Clean target. This will clean all the compiled and generated files.
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
