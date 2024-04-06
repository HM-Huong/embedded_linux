# Phân loại

## Platform Drivers / Controller Drivers

Là những driver điều khiển các module trong SoC thông qua việc thao tác trực tiếp với thanh ghi. Platform drivers cung cấp các API cho kernel và device drivers khác để truy cập vào các thiết bị ngoại vi.

## Device Drivers / Protocol Drivers

Device drivers thao tác với các thiết bị được kết nối với SoC thông qua các API của platform drivers.

## Không biết loại nào

- Character drivers: đọc/ghi dữ liệu theo từng byte.
- Block drivers: đọc/ghi dữ liệu theo từng block.

# Các bước viết driver

1. Tìm hiểu về mặt phần cứng của thiết bị ngoại vi cần điều khiển.
2. Tìm hiểu chuẩn giao tiếp giữa thiết bị ngoại vi và máy tính.
3. Tìm hiểu template driver cho thiết bị ngoại vi cũng như [example code](https://elixir.bootlin.com/linux/latest/source). Có thể tham khảo tại [Linux Device Drivers, Third Edition](https://lwn.net/Kernel/LDD3/).
4. Viết driver cho thiết bị ngoại vi.

# Misc (or miscellaneous) Drivers
Một template driver thường có hai phần: API giao tiếp với phần cứng struct dữ liệu chung chuyển giữa OS vs phần cứng
Để bắt đầu viết một driver cho một thiết bị phần cứng thì cần
- Hiểu về mặt phần cứng của thiết bị
- Tìm hiểu chuẩn giao tiếp giữa application của linux vs peripheral
- Tìm hiểu template - example code
- Thực hiện viết driver
# Flow giao tiếp giữa application với hardware
- application không thể trực tiếp thao tác với hardware mà thường mỗi phần cứng sẽ có một device file (file đại diện cho phần cứng đó) để application đọc ghi dữ liệu. Và yêu cầu từ device file được chuyển xuống cho OS, OS xử lí yêu cầu.
- OS nhận được yêu cầu tuy nhiên OS ko thể nhận biết được đâu là peripheral đó, vậy nên cần driver của phần cứng đó
# MISC Driver (driver đa năng)
## Struct của MISC
- struct chứa thông tin của file được tạo ra: tên device file, fops: danh sách các hàm thao tác với file mà driver hỗ trợ, mode
```c
struct miscdriver {
    int minor;
    const char *name;
    const struct file_operations *fops; // hàm chứa API thao tác với phần cứng
    struct list_head list;
    struct device *parent;
    struct device *this_device;
    const struct attribute_group **groups;
    const char *nodename;
    unode_t node;
}
```
- `minor`: là số nhận dạng duy nhất được gán cho mỗi thiết bị cụ thể được quản lí bởi cùng một driver => phân biệt các thiết bị dùng chung driver
- `name`: tên của driver, thường được hiện thị trong hệ thống file `/dev/`
- `file_operations`: con trỏ đến struct file_operations chứa các hàm callback để thao tác với phần cứng như: read, write, ioctl...
- `list`: liên kết với các misc driver với nhau trong một danh sách
- `parent`: trỏ đến thiết bị cha trong hệ thống phân cấp
- `this_device`: con trỏ đến đối tượng device mà driver này đại diện
- `sttribute_group`: con trỏ đến một mảng các nhóm thuộc tính, cho phép tạo các sysfs entries
- `nodename`: tên nút thiết bị (nếu khác với name) sẽ được dùng để tạo nút thiết bị trong sysfs
- `node`: đại diện cho số node thiết bị, được sử dụng bởi file system để quản lí node thiết bị
**Note**: Các misc driver liên kết với nhau thành một danh sách driver. Khi một thiết bị được mở, hàm `misc_open` đầu tiên sẽ được gọi, nó sẽ tìm kiếm driver phù hợp trong danh sách và chuyển quyền điều khiển cho driver đó.
**VD**: Một trường hợp cụ thể cần liên kết các misc driver là khi có nhiều thiết bị phần cứng khác nhau, không rõ ràng thuộc loại nào và không cần một số major cố định. Ví dụ, có thể có các thiết bị như đồng hồ thời gian thực (RTC), các cảm biến, hoặc các thiết bị đầu vào khác nhau. Mỗi thiết bị này sẽ có một driver riêng biệt, nhưng tất cả đều sẽ được quản lý dưới dạng misc drivers với số major là 10 và số minor khác nhau. Việc liên kết giúp hệ thống có thể dễ dàng quản lý và truy cập đến driver phù hợp dựa trên số minor khi thiết bị được truy cập.

Misc drivers là các driver đa năng, không thuộc về một loại thiết bị cụ thể.

[miscdevice.h](https://elixir.bootlin.com/linux/latest/source/include/linux/miscdevice.h)

Misc drivers sẽ tạo ra các device file trong `/dev/`. Khi ứng dụng thao tác với device file này, kernel sẽ gọi và truyền dữ liệu vào các hàm đã được định nghĩa trong driver.

![-](<./attachments/misc drivers.drawio.png>)

## Bài toán: Điều khiển đèn LED thông qua GPIO trên BeagleBone Black

Ứng dụng trên hệ điều hành Linux điều khiển đèn led thông qua device file `/dev/led_example` theo chuẩn như sau:

- Ghi `1` vào file để bật đèn led.
- Ghi `0` vào file để tắt đèn led.
- Đọc file để lấy trạng thái đèn led (`1` là bật, `0` là tắt).
- Có thể thiết lập và đọc số lần nháy đèn trước khi bật. Ví dụ khi đặt số lần nháy đèn là 10 thì đèn phải nháy 10 lân trước khi được bật.

### Bước 1: Tìm hiểu về mặt phần cứng của thiết bị ngoại vi cần điều khiển

Xem lại bài [loadable kernel module](./3-loadable-kernel-module.md)

### Bước 2: Tìm hiểu chuẩn giao tiếp giữa thiết bị ngoại vi và máy tính

Ứng dụng trên hệ điều hành Linux điều khiển đèn led thông qua device file `/dev/led_example` theo chuẩn như sau:

- Ghi `1` vào file để bật đèn led.
- Ghi `0` vào file để tắt đèn led.
- Đọc file để lấy trạng thái đèn led (`1` là bật, `0` là tắt).

### Bước 3: Tìm hiểu template driver cho thiết bị ngoại vi

<https://elixir.bootlin.com/linux/latest/source/include/linux/miscdevice.h#L79>
<https://elixir.bootlin.com/linux/latest/source/include/linux/fs.h#L1983>

### Bước 4: Viết driver cho thiết bị ngoại vi

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
	g_led_timer->ledConfig.delay = 100;
	g_led_timer->ledConfig.times = 10;
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

	return sizeof(status);
}

static ssize_t misc_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos) {
	uint32_t reg;
	char local_buf[128];
	int ret = 0;

	pr_info("Line %d, function %s\n", __LINE__, __func__);
	pr_info("len: %zu, ppos: %lld\n", len, *ppos);
	len = len > sizeof(local_buf) ? sizeof(local_buf) : len;
	memset(&local_buf, 0, len);
	ret = copy_from_user(&local_buf, buf, len);
	pr_info("\tlocal_buf: %s\n", local_buf);

	switch (local_buf[0]) {
	case '0':
		reg = readl_relaxed(gpio0_base + GPIO_DATAOUT);
		reg &= ~LED;
		writel_relaxed(reg, gpio0_base + GPIO_DATAOUT);
		break;
	case '1':
		timer_setup(&g_led_timer->timer, blink, 0);
		mod_timer(&g_led_timer->timer, jiffies);
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

Một số hàm khác:

- `dump_stack()`: in ra stack trace theo thứ tự từ hàm được gọi gần nhất đến hàm được gọi xa nhất.

### Test driver

Thao tác với device file `/dev/misc_example` bằng các lệnh sau:
FIXME: cat file không có ký tự kết thúc file.

```bash
echo "1" > /dev/misc_example
cat /dev/misc_example
echo "0" > /dev/misc_example
cat /dev/misc_example
```

Hoặc sử dụng code tầng ứng dụng được viết bằng C:

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