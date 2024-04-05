#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/timer.h>

#define GPIO_ADDR_BASE 0x44E07000
#define ADDR_SIZE (0x1000)
#define GPIO_SETDATAOUT_OFFSET 0x194
#define GPIO_CLEARDATAOUT_OFFSET 0x190
#define GPIO_OE_OFFSET 0x134
#define GPIO_DATAOUT 0x13C
#define LED ~(1 << 31)
#define DATA_OUT (1 << 31)

struct config
{
    int times;
};

struct timer_config
{
    struct config app_config;
    struct timer_list my_timer;
};

void __iomem *base_addr;
// struct config g_config;
struct timer_config *g_timer_data;

int misc_open(struct inode *node, struct file *filep)
{
    // uint32_t reg_data = 0;

    if (gpio_is_valid(31) == false)
    {
        pr_err("GPIO %d is not valid\n", 31);
        return 0;
    }
    if(gpio_request(31 ,"GPIO_31") < 0) {
        pr_err("ERROR: GPIO %d request\n", 31);
        return 0;
    }

    gpio_direction_output(31, 0);
    gpio_export(31, false);

    pr_info("%s, %d\n", __func__, __LINE__);
    // reg_data = readl_relaxed(base_addr + GPIO_OE_OFFSET);
    // reg_data &= LED;
    // writel_relaxed(reg_data, base_addr + GPIO_OE_OFFSET);

    return 0;
}

int misc_release(struct inode *node, struct file *filep)
{
    pr_info("%s, %d\n", __func__, __LINE__);

    return 0;
}

static ssize_t misc_read(struct file *filp, char __user *buf, size_t count,
                         loff_t *f_pos)
{
    uint32_t reg_data = 0;
    char status = 0;
    int ret = 0;

    reg_data = readl_relaxed(base_addr + GPIO_DATAOUT);
    reg_data &= DATA_OUT;

    if (reg_data == 0) {
        status = '0';
    } else {
        status = '1';
    }

    ret = copy_to_user(buf, &status, 1);
    pr_info("%s, %d, buf: %c\n", __func__, __LINE__, status);
    return 1;
}

static void timer_function(struct timer_list *t)
{
    struct timer_config *timer_data = container_of(t, struct timer_config, my_timer);
    static int count = 0;

    pr_info("delay time is %d\n", timer_data->app_config.times);
    count = timer_data->app_config.times;

    if (count % 2 == 0)
        // writel_relaxed(DATA_OUT, base_addr + GPIO_SETDATAOUT_OFFSET);
        gpio_set_value(31, 1);
    else
        // writel_relaxed(DATA_OUT, base_addr + GPIO_CLEARDATAOUT_OFFSET);
        gpio_set_value(31, 0);

    count++;

    if (count < 20)
        mod_timer(t, jiffies + HZ);
}

static ssize_t misc_write(struct file *filp, const char __user *buf,
                          size_t count, loff_t *f_pos)
{
    int ret = 0;
    char local_data[128];

    memset(local_data, 0, sizeof(local_data));
    ret = copy_from_user(local_data, buf, count < 128 ? count : 128);
    pr_info("%s, %d, buf: %s\n", __func__, __LINE__, local_data);

    switch (local_data[0])
    {
    case '1': //bat led
        // writel_relaxed(DATA_OUT, base_addr + GPIO_SETDATAOUT_OFFSET);
        timer_setup(&(g_timer_data->my_timer), timer_function, 0);
        add_timer(&(g_timer_data->my_timer));
        break;
    
    case '0': //tat led
        writel_relaxed(DATA_OUT, base_addr + GPIO_CLEARDATAOUT_OFFSET);
        break;
    default:
        pr_info("send invalid command\n");
        break;
    }

    return count;
}

#define MAGIC_NO	100
#define SEND_DATA_CMD	_IOW(MAGIC_NO, sizeof(struct config), struct config *) //=> unsigned int
#define GET_DATA_CMD	_IOR(MAGIC_NO, sizeof(struct config), struct config *)

static long misc_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    g_timer_data = kmalloc(sizeof(struct timer_config), GFP_KERNEL);

    switch (cmd)
    {
    case SEND_DATA_CMD:
        ret = copy_from_user(&(g_timer_data->app_config), (struct config *)arg, sizeof(struct config));
        pr_info("%s, %d, times: %d\n", __func__, __LINE__, g_timer_data->app_config.times);
        break;

    case GET_DATA_CMD:
        ret = copy_to_user((struct config *)arg, &(g_timer_data->app_config), sizeof(struct config));
        break;
    
    default:
        pr_info("invalid command\n");
        return -1;
    }

    return 0;
}
// Chua thong tin API de lam viec voi hardware
struct file_operations misc_fops = {
    .owner = THIS_MODULE,
    .open = misc_open, //Enable hardware, this func is call when kernel want to connect with hardware
    .release = misc_release, //disable hardware, synchronize data xuong hardware
    .read = misc_read, //OS se cap cho driver buffer va yeu cau doc du lieu tu hardware, luu vao buffer cua kernel
    .write = misc_write, //Ghi du lieu tu buffer cua kernel xuong hardware
    .unlocked_ioctl = misc_ioctl, //Chua cau hinh ve thoi gian sang cua den
};

// chua thong tin cua hardware, gom cac API lam viec voi device nhu khai bao o tren va thong tin device nhu ID, name...
static struct miscdevice misc_example = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "misc_example",
    .fops = &misc_fops,
};

static int misc_init(void)
{
    pr_info("misc module init\n");
    base_addr = ioremap(GPIO_ADDR_BASE, ADDR_SIZE);
    misc_register(&misc_example);

    return 0;
}

static void misc_exit(void)
{
    pr_info("misc module exit\n");
    misc_deregister(&misc_example);
}

module_init(misc_init);
module_exit(misc_exit);

MODULE_AUTHOR("Phu Luu An");
MODULE_DESCRIPTION("Example misc driver.");
MODULE_LICENSE("GPL");