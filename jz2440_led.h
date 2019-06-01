#ifndef __JZ2440_LED_H__
#define __JZ2440_LED_H__

#define DEVICE_NAME     "led"
#define LEDS_DEV_MINOR  0
#define LEDS_DEV_CNT    3


#define S3C2410_GPACON      0x56000000
#define S3C2410_GPFCON      (S3C2410_GPACON + 0x50)
#define S3C2410_GPFDAT      (S3C2410_GPACON + 0x54)

#define S3C2410_GPF4_SHT    4
#define S3C2410_GPF5_SHT    5
#define S3C2410_GPF6_SHT    6


#define S3C2410_GPF4_OUTP   (0x01 << (2*S3C2410_GPF4_SHT))
#define S3C2410_GPF5_OUTP   (0x01 << (2*S3C2410_GPF5_SHT))
#define S3C2410_GPF6_OUTP   (0x01 << (2*S3C2410_GPF6_SHT))


enum led_sta {
        LED_ON = 0,
        LED_OFF,
};

typedef struct led_dev {
        struct cdev dev;
        unsigned int off;
        unsigned long *pGpioCon;
        unsigned long *pGpioDat;
        unsigned long phyAddr_Con;
        unsigned long phyAddr_Dat;
}led_dev_t;


#endif
