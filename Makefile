obj-m += kernel_user.o

all:
	# Compile the kernel module (tab required before `make`)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	# Clean the build files (tab required before `make`)
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
