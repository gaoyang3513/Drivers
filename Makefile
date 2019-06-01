KERNEL_DIR = /home/gaoyang/source_code/JZ2440/kernel/linux-2.6.22.6

obj-m += jz2440_led.o

all: demo
	make modules -C $(KERNEL_DIR) M=`pwd`

clean:
	@rm -rf *.o *.ko *mod.* .order *.symvers demo_leds

demo:
	arm-linux-gcc test_jz2440_leds.c -o demo_leds
	@chmod +x  demo_leds
