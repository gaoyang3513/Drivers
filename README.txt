Build
	KO
		make

	App
		arm-linux-gcc test_jz2440_led.c -o test_jz2440_led

Run
	insmod jz2440_led.ko
	mknod /dev/led0 c 252 0
	mknod /dev/led1 c 252 1
	mknod /dev/led2 c 252 2
	./demo_leds /dev/led1 on
