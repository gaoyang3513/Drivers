#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>         // cdev
#include <linux/io.h>           // ioremap
#include <asm/uaccess.h>        // copy_from_user
#include "jz2440_led.h"

static dev_t gDev = 0;

struct led_dev Leds_tab[LEDS_DEV_CNT] = {
    {.off = 4, .phyAddr_Con = S3C2410_GPFCON, .phyAddr_Dat = S3C2410_GPFDAT},
    {.off = 5, .phyAddr_Con = S3C2410_GPFCON, .phyAddr_Dat = S3C2410_GPFDAT},
    {.off = 6, .phyAddr_Con = S3C2410_GPFCON, .phyAddr_Dat = S3C2410_GPFDAT},
};

static int set_led (led_dev_t *led, enum led_sta sta)
{
        unsigned int val, off;
        unsigned long *pGpio_Con;
        unsigned long *pGpio_Dat;

        off       = led->off;
        pGpio_Con = led->pGpioCon;
        pGpio_Dat = led->pGpioDat;

        val = readl(pGpio_Con);
        val |= (1 << (2 * off));
        writel(val, pGpio_Con);

        val = readl(pGpio_Dat);
        val &= ~(1 << off);             
        val |= sta << off;
        writel(val, pGpio_Dat);
        printk("Set led%d %s\n", off, sta ? "off" : "on");

        return 0;
}

static int s3c24xx_led_open(struct inode *inode , struct file *file)
{
        unsigned int minor;

        minor = MINOR(inode->i_rdev);
        file->private_data = (void *)&Leds_tab[minor];

        printk("Open leds succeed.\n");

        return 0;
}

static ssize_t s3c24xx_led_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
        char val;
        struct led_dev *led;

        led = (struct led_dev* )file->private_data;
   
        if (copy_from_user(&val, buf, 1)) {
                printk("Error: an error occurred when run  copy_from_user.\n");
   
                return -1;
        }

        switch (val) {
        case 0:
                set_led(led, LED_OFF);

                break;
        case 1:
                set_led(led, LED_ON);

                break;
        default :
                printk("Error: not support parameter.\n");
                break;
        }

        return 0;        
}

struct file_operations s3c24xx_led_fops = {
    .owner   = THIS_MODULE,
    .open  = s3c24xx_led_open,
    .write = s3c24xx_led_write,
};

static int __init jz24xx_led_init(void)
{
        int i, Ret = 0;

        Ret = alloc_chrdev_region(&gDev, LEDS_DEV_MINOR, LEDS_DEV_CNT, DEVICE_NAME);
        if (Ret) {
                printk("[%s]: Register chrdev LED failed.\n", DEVICE_NAME);
                return -1;
        }

        printk("Register chrdev LED success, with Numer=(%3d, %3d).\n", MAJOR(gDev), MINOR(gDev));

        for (i = 0; i < LEDS_DEV_CNT; i++) {
                /* cdev_init初始化cdev其中最重要的->ops、->kobj */
                cdev_init(&Leds_tab[i].dev, &s3c24xx_led_fops);
                Leds_tab[i].dev.owner = THIS_MODULE;
                /* 
                 * cdev_add将cdev抽象为probe探针并保存到全局变量cdev_map，
                 * cdev_map是使用主设备号Major为索引的数组，共有255个元素，每个元素为一个probe链表
                 */
                cdev_add(&Leds_tab[i].dev, gDev + i, 1);
        }

        printk("Leds regitster succeed.\n");

        for (i = 0; i < LEDS_DEV_CNT; i++) {
                Leds_tab[i].pGpioCon = (unsigned long *)ioremap(Leds_tab[i].phyAddr_Con, 4);
                Leds_tab[i].pGpioDat = (unsigned long *)ioremap(Leds_tab[i].phyAddr_Dat, 4);

                printk("IOremap succeed,LED_%d, gpGpioF_Con=0x%p, gpGpioF_Dat=0x%p\n",
                       i, Leds_tab[i].pGpioCon, Leds_tab[i].pGpioDat);

                set_led(&Leds_tab[i], LED_ON);
        }

        printk("Leds init succeed.\n");
        printk("[%s]: Module init success.\n", DEVICE_NAME);
        return 0;
}

static void __exit jz24xx_led_exit(void)
{
        int i;

        for (i = 0; i < LEDS_DEV_CNT; i++) {
                set_led(&Leds_tab[i], LED_OFF);

                iounmap(Leds_tab[i].pGpioCon);
                iounmap(Leds_tab[i].pGpioDat);

                printk("IOunmap succeed, LED_%d, gpGpioF_Con=0x%p, gpGpioF_Dat=0x%p\n",
                       i, Leds_tab[i].pGpioCon, Leds_tab[i].pGpioDat);
        }

        printk("Leds deinit succeed.\n");

        for (i = 0; i < LEDS_DEV_CNT; i++)
                cdev_del(&Leds_tab[i].dev);
        unregister_chrdev_region(gDev, LEDS_DEV_CNT);

        printk("[%s]: Module exit success.\n", DEVICE_NAME);
}

module_init(jz24xx_led_init);
module_exit(jz24xx_led_exit);

MODULE_AUTHOR("Gaoyang <gaoyang3513@163.com>");
MODULE_DESCRIPTION("S3C24XX LED driver");
MODULE_LICENSE("GPL");

