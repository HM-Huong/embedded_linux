#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

//Chan GPIO0_31 chuc nang la button
//Chan GPIO0_20 chuc nang la den led

#define GPIO0_31 (31)
#define GPIO0_20 (20)

#define BUTTON_DEV "example_of_irq"

int irq = -1;

static irqreturn_t button_isr(int irq, void *data)
{
    static int count;

    if (count % 2 == 0)
        gpio_set_value(GPIO0_20, 1);
    else
        gpio_set_value(GPIO0_20, 0);

    count++;

    return IRQ_HANDLED;
}

int init_module(void)
{
    int ret = -1;

    pr_info("Hello world driver is loaded\n");
    //Tra cuu so hieu ngat cua chan gpio
    gpio_request(GPIO0_31,"GPIO0_31");
    irq = gpio_to_irq(GPIO0_31);
    ret = request_irq(irq, button_isr, IRQF_TRIGGER_RISING, "button_gpio20", BUTTON_DEV);

    ret = gpio_request(GPIO0_20,"GPIO_20");
    gpio_direction_output(GPIO0_20, 0);

    return 0;
}

void cleanup_module(void)
{
    pr_info("hello world driver is unloaded\n");
    gpio_set_value(GPIO0_20, 0);
    free_irq(irq, BUTTON_DEV);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ta Manh Tuyen");
MODULE_DESCRIPTION("GPIO led kernel module");