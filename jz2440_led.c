#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/io.h>     // ioremap
#include <asm/uaccess.h>  // copy_from_user

/* 加载模式后，执行"cat /proc/devices"命令看到的设备名称 */
#define DEVICE_NAME     "leds"

/* 高电平灭 */
#define LED_ON    0
#define LED_OFF   1

/* 用来指定LED所用的GPIO引脚 */
#define S3C2410_GPACON      0x56000000
#define S3C2410_GPFCON      (S3C2410_GPACON + 0x50)
#define S3C2410_GPFDAT      (S3C2410_GPACON + 0x54)

#define S3C2410_GPF4_SHT    4
#define S3C2410_GPF5_SHT    5
#define S3C2410_GPF6_SHT    6

/* 用来指定GPIO引脚的功能：输出 */
#define S3C2410_GPF4_OUTP   (0x01 << (2*S3C2410_GPF4_SHT))
#define S3C2410_GPF5_OUTP   (0x01 << (2*S3C2410_GPF5_SHT))
#define S3C2410_GPF6_OUTP   (0x01 << (2*S3C2410_GPF6_SHT))

static int gMajor = 0;
static volatile unsigned long *gpGpioF_Con = 0;
static volatile unsigned long *gpGpioF_Dat = 0;

static int s3c24xx_led_open(struct inode *inode , struct file *file)
{
    printk("Open leds succeed.\n");

    return 0;
}

static ssize_t s3c24xx_led_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
        char val;
        
        if (copy_from_user(&val, buf, 1)) {
                printk("Error: an error occurred when run  copy_from_user.\n");
                return -1;
        }
        switch (val) {
        case 1:
                val = readl(gpGpioF_Dat);
                val &= ~(0x01 << S3C2410_GPF4_SHT);                
                val |= LED_ON << S3C2410_GPF4_SHT;
                writel(val, gpGpioF_Dat);
                printk("Set led0 on\n");

                break;
        case 0:
                val = readl(gpGpioF_Dat);
                val &= ~(0x1 << S3C2410_GPF4_SHT);
                val |= LED_OFF << S3C2410_GPF4_SHT;
                writel(val, gpGpioF_Dat);
                printk("Set led0 off\n");

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
        volatile unsigned val_con, val_dat;

        /* GPACON 0x56000000 ~ MSLCON 0x560000CC */
        gpGpioF_Con = (unsigned long *)ioremap(S3C2410_GPFCON, 4);
        gpGpioF_Dat = gpGpioF_Con + 1;

        printk("ioremap succeed. gpGpioF_Con=0x%p, gpGpioF_Dat=0x%p\n", gpGpioF_Con, gpGpioF_Dat);
        
        gMajor = register_chrdev(0, DEVICE_NAME, &s3c24xx_led_fops);
        if (gMajor < 0) {
                printk("[%s]: Register chrdev LED failed.\n", DEVICE_NAME);
                return -1;
        }
        printk("register_chrdev\n");
        
        val_con = readl(gpGpioF_Con);
        val_con |= S3C2410_GPF4_OUTP | S3C2410_GPF5_OUTP | S3C2410_GPF6_OUTP;
        writel(val_con, gpGpioF_Con);

        val_dat = readl(gpGpioF_Dat);
        val_dat |= ((0x01 << S3C2410_GPF4_SHT) | (0x01 << S3C2410_GPF5_SHT) | (0x01 << S3C2410_GPF6_SHT));
        writel(val_dat, gpGpioF_Dat);
        printk("Leds init succeed.\n");

        printk("[%s]: Module init success.\n", DEVICE_NAME);
        return 0;
}

static void __exit jz24xx_led_exit(void)
{
    int ret = 0;

    iounmap(gpGpioF_Con);

    ret = unregister_chrdev(gMajor, DEVICE_NAME);
    if ( ret < 0) {
        printk("[%s]: Unregister chrdev failed.", DEVICE_NAME);
    }

    printk("[%s]: Module exit success.\n", DEVICE_NAME);
}

module_init(jz24xx_led_init);
module_exit(jz24xx_led_exit);

MODULE_AUTHOR("Gaoyang <gaoyang3513@163.com>");
MODULE_DESCRIPTION("S3C24XX LED driver");
MODULE_LICENSE("GPL");

