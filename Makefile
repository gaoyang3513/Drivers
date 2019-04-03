KERNEL_DIR = /home/gaoyang/source_code/JZ2440/kernel/linux-2.6.22.6

obj-m += jz2440_led.o

all:
	make modules -C $(KERNEL_DIR) M=`pwd`

clean:
	@rm -rf *.o *.ko *mod.* .order *.symvers


