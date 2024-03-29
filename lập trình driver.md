# Phân loại

## Platform Drivers / Controller Drivers

Là những driver điều khiển các module trong SoC thông qua việc thao tác trực tiếp với thanh ghi. Platform drivers cung cấp các API cho kernel và device drivers khác để truy cập vào các thiết bị ngoại vi.

## Device Drivers / Protocol Drivers

Device drivers thao tác với các thiết bị được kết nối với SoC thông qua các API của platform drivers.

# Các bước viết driver

1. Tìm hiểu về mặt phần cứng của thiết bị ngoại vi cần điều khiển.
2. Tìm hiểu chuẩn giao tiếp giữa thiết bị ngoại vi và máy tính.
3. Tìm hiểu template driver cho thiết bị ngoại vi cũng như [example code](https://elixir.bootlin.com/linux/latest/source). Có thể tham khảo tại [Linux Device Drivers, Third Edition](https://lwn.net/Kernel/LDD3/).
4. Viết driver cho thiết bị ngoại vi.

# Misc (or miscellaneous) Drivers

Misc drivers là các driver đa năng, không thuộc về một loại thiết bị cụ thể.

[miscdevice.h](https://elixir.bootlin.com/linux/latest/source/include/linux/miscdevice.h)

Misc drivers sẽ tạo ra các device file trong `/dev/`. Khi ứng dụng thao tác với device file này, kernel sẽ gọi và truyền dữ liệu vào các hàm đã được định nghĩa trong driver.

![-](./attachments/misc%20drivers.drawio.png)

## Bài toán: Điều khiển đèn LED thông qua GPIO trên BeagleBone Black

Ứng dụng trên hệ điều hành Linux điều khiển đèn led thông qua device file `/dev/led_example` theo chuẩn như sau:

- Ghi `'1'` vào file để bật đèn led.
- Ghi `'0'` vào file để tắt đèn led.
- Đọc file để lấy trạng thái đèn led (`'1'` là bật, `'0'` là tắt).
- Có thể thiết lập và đọc số lần nháy đèn trước khi bật. Ví dụ khi đặt số lần nháy đèn là 10 thì đèn phải nháy 10 lân trước khi được bật.

### Bước 1: Tìm hiểu về mặt phần cứng của thiết bị ngoại vi cần điều khiển

Xem lại bài [loadable kernel module](./loadable-kernel-module.md)

### Bước 2: Tìm hiểu chuẩn giao tiếp giữa thiết bị ngoại vi và máy tính

Ứng dụng trên hệ điều hành Linux điều khiển đèn led thông qua device file `/dev/led_example` theo chuẩn như sau:

- Ghi `'1'` vào file để bật đèn led.
- Ghi `'0'` vào file để tắt đèn led.
- Đọc file để lấy trạng thái đèn led (`'1'` là bật, `'0'` là tắt).

### Bước 3: Tìm hiểu template driver cho thiết bị ngoại vi

<https://elixir.bootlin.com/linux/latest/source/include/linux/miscdevice.h#L79>
<https://elixir.bootlin.com/linux/latest/source/include/linux/fs.h#L1983>

### Bước 4: Viết driver cho thiết bị ngoại vi

TODO: tìm hiểu macro `container_of` trong C.

File `config.h`:

```c
#pragma once

struct LedConfig {
	int times;
	int delay;
};

#define MAGIC_NO 100
#define SEND_DATA_CMD _IOW(MAGIC_NO, sizeof(struct LedConfig), struct LedConfig *)
#define GET_DATA_CMD _IOR(MAGIC_NO, sizeof(struct LedConfig), struct LedConfig *)
```

```c
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/timer.h>

#include "config.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leo");
MODULE_DESCRIPTION("A simple Hello world LKM!");
MODULE_VERSION("0.1");

#define GPIO0_BASE_ADDR 0x44E07000
#define GPIO0_SIZE 0x1000
#define GPIO_OE 0x134
#define GPIO_DATAOUT 0x13C
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194

#define LED (1 << 31)

struct LedTimerConfig {
	struct LedConfig ledConfig;
	struct timer_list timer;
};

void __iomem *gpio0_base;
struct LedTimerConfig *g_led_timer;

static int misc_open(struct inode *inode, struct file *file);
static int misc_release(struct inode *inode, struct file *file);
static ssize_t misc_read(struct file *file, char __user *buf, size_t len, loff_t *ppos);
static ssize_t misc_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos);
static long misc_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static void blink(struct timer_list *t);

static struct file_operations misc_fops = {
	.owner = THIS_MODULE,
	.open = misc_open,			  // Enable hardware
	.release = misc_release,	  // disable hardware, synchronize data xuong hardware
	.read = misc_read,			  // Doc du lieu tu hardware, luu vao buffer cua kernel
	.write = misc_write,		  // Ghi du lieu tu buffer cua kernel xuong hardware
	.unlocked_ioctl = misc_ioctl, // Chua cau hinh ve thoi gian sang cua den
};

static struct miscdevice misc_example = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "misc_example",
	.fops = &misc_fops,
};

int init_module(void) {
	pr_info("Loading misc module...\n");

	gpio0_base = ioremap(GPIO0_BASE_ADDR, GPIO0_SIZE);
	misc_register(&misc_example);
	g_led_timer = kmalloc(sizeof(struct LedTimerConfig), GFP_KERNEL);

	return 0;
}

void cleanup_module(void) {
	pr_info("Unloading misc module...\n");
	misc_deregister(&misc_example);
}

static int misc_open(struct inode *inode, struct file *file) {
	uint32_t reg;
	pr_info("Line %d, function %s\n", __LINE__, __func__);

	reg = readl_relaxed(gpio0_base + GPIO_OE);
	reg &= ~LED; // set LED pin as output
	writel_relaxed(reg, gpio0_base + GPIO_OE);

	return 0;
}

static int misc_release(struct inode *inode, struct file *file) {
	pr_info("Line %d, function %s\n", __LINE__, __func__);
	return 0;
}

static ssize_t misc_read(struct file *file, char __user *buf, size_t len, loff_t *ppos) {
	uint32_t reg = readl_relaxed(gpio0_base + GPIO_DATAOUT);
	char status = (reg & LED) ? '1' : '0';
	int ret = 0;

	pr_info("Line %d, function %s\n", __LINE__, __func__);
	pr_info("len: %zu, ppos: %lld\n", len, *ppos);

	ret = copy_to_user(buf, &status, sizeof(status));
	pr_info("\tstatus: %c, ret = %d\n", status, ret);

	return ret;
}

static ssize_t misc_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos) {
	uint32_t reg;
	char local_buf[128];
	int ret = 0;

	pr_info("Line %d, function %s\n", __LINE__, __func__);
	pr_info("len: %zu, ppos: %lld\n", len, *ppos);

	memset(&local_buf, 0, sizeof(local_buf));
	ret = copy_from_user(&local_buf, buf, len > sizeof(local_buf) ? sizeof(local_buf) : len);
	pr_info("\tlocal_buf: %s\n", local_buf);

	switch (local_buf[0]) {
	case '0':
		reg = readl_relaxed(gpio0_base + GPIO_DATAOUT);
		reg &= ~LED;
		writel_relaxed(reg, gpio0_base + GPIO_DATAOUT);
		break;
	case '1':
		if (g_led_timer->ledConfig.times > 0) {
			timer_setup(&g_led_timer->timer, blink, 0);
			mod_timer(&g_led_timer->timer, jiffies);
		} else {
			reg = readl_relaxed(gpio0_base + GPIO_DATAOUT);
			reg |= LED;
			writel_relaxed(reg, gpio0_base + GPIO_DATAOUT);
		}
		break;
	default:
		pr_info("\tInvalid command\n");
		break;
	}

	return len;
}

static void blink(struct timer_list *t) {
	static int count = 0;
	uint32_t reg = readl_relaxed(gpio0_base + GPIO_DATAOUT);
	struct LedTimerConfig *config = container_of(t, struct LedTimerConfig, timer);
	if (count < config->ledConfig.times) {
		count++;
		reg ^= LED;
		writel_relaxed(reg, gpio0_base + GPIO_DATAOUT);
		mod_timer(t, jiffies + msecs_to_jiffies(config->ledConfig.delay));
	} else {
		reg |= LED;
		writel_relaxed(reg, gpio0_base + GPIO_DATAOUT);
		count = 0;
		del_timer_sync(t);
	}
}

static long misc_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
	int ret = 0;
	pr_info("Line %d, function %s\n", __LINE__, __func__);
	switch (cmd) {
	case SEND_DATA_CMD:
		ret = copy_from_user(&g_led_timer->ledConfig, (struct LedConfig *)arg, sizeof(struct LedConfig));
		pr_info("\tconfig.times: %d\n", g_led_timer->ledConfig.times);
		break;
	case GET_DATA_CMD:
		ret = copy_to_user((struct LedConfig *)arg, &g_led_timer->ledConfig, sizeof(struct LedConfig));
		pr_info("\tconfig.times: %d\n", g_led_timer->ledConfig.times);
		break;
	default:
		pr_info("\tInvalid command\n");
		return -1;
	}
	return 0;
}
```

### Test driver

```c
/*
Build command:
/home/leo/bbb/gcc-arm-10.3-2021.07-x86_64-arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-gcc -g -o main main.c
*/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "config.h"

int main() {
	int fd = 0;
	char data[8];
	struct LedConfig config_data;

	config_data.times = 10;
	config_data.delay = 500;

	// điểu khiển led thông qua việc đọc ghi device file
	fd = open("/dev/misc_example", O_RDWR);
	if (fd > 0) {
		memset(data, 0, sizeof(data));

		ioctl(fd, SEND_DATA_CMD, &config_data);
		write(fd, "1", strlen("1"));

		read(fd, data, 8);
		printf("data: %s\n", data);
		write(fd, "0", strlen("0"));
		close(fd);
	}
	return 0;
}
```

Note sửa file `c_cpp_properties.json` để tránh lỗi IntelliSense:

```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/home/leo/bbb/kernelbuildscripts/KERNEL/include/**",
                "/home/leo/bbb/kernelbuildscripts/KERNEL/arch/arm/include/**",
                "/home/leo/bbb/kernelbuildscripts/dl/gcc-11.4.0-nolibc/arm-linux-gnueabi/lib/gcc/arm-linux-gnueabi/11.4.0/include/**",
                "/usr/include/**"
            ],
            "defines": [
                "KBUILD_MODNAME=\"BBB_MODULE\"",
                "CONFIG_GPIOLIB",
                "__GNUC__",
                "__KERNEL__",
                "MODULE"
            ],
            "compilerPath": "",
            "cStandard": "c17",
            "cppStandard": "gnu++14",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
```