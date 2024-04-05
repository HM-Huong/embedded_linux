# Tổng quan về hệ thống Linux
[Hình ảnh về hệ thống Linux đơn giản hoá](../attachments/simple-linux-system.png)
[Hình ảnh hoàn thiện về hệ thống Linux ](../attachments/complete-linux-system.png)
- Hệ thống Linux chia làm hai thành phần chính là: User space và Kernel space
## Kernel space
- là nơi kernel chạy và thực thi
- Kernel mode là chế độ thực thi CPU của kernel và được chạy dưới đặc quyền chế độ root-access
- khi một ứng dụng người dùng muốn sử dụng services bởi kernel thì nó sẽ gửi tín hiệu đến kernel để kernel thực thi một syscall, chuyển sang chế độ kernel trong suốt thời gian thực thi syscall
## User space
- được thực thi bởi người dùng trong OS chứ không phải một phần chính của OS. Không gian người dùng là vùng bộ nhớ mà các ứng dụng không phải một phần của kernel chạy trong đó.
- các user process chạy trong user space and as user role - đầy là chế độ thực thi không có đặc quyền. Các process ở chế độ người dùng muốn sử dụng services do kernel cung cấp thì cần chuyển sang chế độ kernel. Khi chuyển sang chế độ nảy tức cần phải trigger một syscall được thực thi bởi kernel.
- User mode này đảm bảo rẳng các user process không can thiệp được vùng nhớ được quản lí bởi kernel hay can thiệp vào OS

## Syscall
- là một func trong kernel, nó cung cấp các dịch vụ cho ứng dụng user space.
- chính là những API mà kernel cung cấp cho chương trình ngươ9ì dùng, giúp chương trình đó sử dụng những tính năng kernel cung cấp, vd: new processes, dick i/o, networking...
 
# Khái niệm về sysfs trong hệ thống Linux
- là hệ thống tập tin giả (pseudo filesystem) được cung cấp bởi kernel linux
- /sys cung cấp thông tin về devices, drivers và buses
# gpio_is_valid(int number)\
- kiểm tra xem số chân GPIO có hợp lệ trước khi cấp phát nó. Trả về giá trị boolean

# gpio_request(unsigned gpio, const char *label)
- yêu cầu cấp phát một GPIO cụ thể và label là nhãn được kernel sử dụng để 
- trả về 0 nếu thành công và một số âm nếu có lỗi 

# gpio_direction_output(unsigned int gpio, int value)
- thiết lập GPIO là đầu ra và `value` là giá trị cần xuất
- trả về 0 nếu success và âm nếu có lỗi

# gpio_export(unsigned gpio, bool direction_may_change)