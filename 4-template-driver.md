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