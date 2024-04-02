# Start
hàm main là điểm đầu vào của chương trình, trước khi vào hàm main, kernel sẽ chạy trước mốt số tác vụ start routine dùng để khởi tạo bộ nhớ cho process, khi hàm main start nó sẽ nhận được hai tham số truyền vào từ kernel
    -argc: là số lượng tham số truyền qua hàm main(), nó đại diện cho số lượng  tham số dòng lệnh khi chạy chương trình
    -argv: đây là một mảng chứa các con trỏ kí tự đến các chuỗi kí tự, argv[0] thường là tên của chương trình

    ví dụ: khi chạy chương trình ./my-program arg1 arg2 thì 
        -argc: là 3 bao gồm tên chương trình và hai tham số
        -argv: chứa con trỏ đến chuỗi "my_program" "arg1" và "arg2"
=> tham số kernel truyền vào giúp truy cập thôgn tin về tham số dòng lênh và thực hiện xử lí 
có nhiều cách để kết thúc một chương trình: 
    -kết thúc chủ động: return, exit
    -kết thúc bị động
**Note**: Sự khác nhau giữa exit và return
    -exit(0): khi gọi đến hàm này thì chương trình sẽ kết thúc ngay lập tức, nó không chỉ thoát khỏi hàm hiện tại mà còn thoát toàn bộ process đang thực thi
    -return 0: nó sẽ kết thúc hàm hiện tạii và trả về 0 cho hàm gọi nó
# Tham số môi trường ảnh hưởng đến process
    chính là 2 biến argc và argv
    ví dụ: quyền truy cập file ảnh hưởng đến process khi thực thi
 ```c
    #include <stdio.h>
#include <string.h>

extern char **environ;

int main() {
    FILE *fp = fopen("./data", "r");
    int i =0;
    for (i =0; environ[i] !=NULL; i++) {
        printf("environ[%d]: %s\n", i, environ[i]);
    };
    if (fp!= NULL) {
        printf("Success");
        fclose(fp);
    } else {
        if (strcmp(environ[19], "USER=ducky") == 0) {
            printf("file cannot open due to priviledge");
        }
        printf("file cannot open");
    }
    return 0;
}
```
## Environ
là một biến toàn cục lưu trữ nhưng thông tin về môi trường của hệ thống như đường dẫn, tên người dùng, biến path...
**char environ: là một mảng các con trỏ tới chuỗi kí tự, environ trỏ tói con trỏ **char

# Kiến trúc bộ nhớ của một process
## Vùng text
    - lưu trữ các biến const, câu lệnh mã máy, được đọc từ tệp chương trình (là tệp mà chứa các mã lệnh được sử dụng để thực thi chương trình, thông thường có định dạng :exe, dll, bat, cmd... và chứa các câu lệnh được viết bằng nn lập trình, khi chạy thì mã lệnh trong tệp sẽ được thực thi)  và tệp chươnh trình này được đọc bởi hàm exec() đây là một hàm gọi, khi một chương trình gọi exec() thì nó sẽ dừng hoàn toàn process hiện tại và thực thi chương trình mới.
    - Khi hàm exec() thực hiện mã máy trong vùng nhớ text thì chương trình mới sẽ chạy và sử dụng vùng nhớ này cho việc thực thi mã máy đó. Các chương trình khác không bị ảnh hưởng trừ khi nó là chương trình con của chương trình đang thực thi exec()
    - vùng nhớ này thường là readonly
## Initialize data
    - chứa các biến đã được khởi tạo giá trị, biến global
## Uninitialize data
    - các biến chưa được gán giá trị, các biến global chưa được gán gtri

## Heap
    - khu vực dành cho cấp phát bộ nhớ động, heap phát triển theo hướng từ địa chỉ thấp đến địa chỉ cao
## Stack
    - chứa biến cục bộ ví dụ như các biến local của hàm khi hàm cha gọi hàm con thì các biến local của hàm cha sẽ được lưu vào stack và khi hàm con thực thi xong thì các biến này được nạp lại vào thanh ghi để tiếp tục thực hiện hàm cha
    - chứa các thông tin điều khiển luồng, khi hàm con được gọi thì các thông tin này của hàm cha sẽ được lưu trên stack tạo thành một bản ghi cuộc gọi hàm mới, khi hàm con hoàn thành, thông tin này được dùng để nạp lại trạng thái của hàm cha
## High address
    - lưu trữ các biến môi trường như **char environ hay các command line argument

## Cấp phát bộ nhớ động
    OS sinh ra và quản lí các process như: địa chỉ process, bộ nhớ đã cấp phát cho process... nhờ và các API mà process đó gọi qua thư viện của OS, nhờ đó mà OS lưu vết được lại địa chỉ không gian bộ nhớ đã câp phát cho process.

# Virtual memory
    - mỗi process khi chạy sẽ được cấp một không gian bộ nhớ ảo để lưu trữ địa chỉ các biến của process đó. Mỗi virtual memory của mỗi process đó sẽ có một bảng phân trang để định địa chỉ của biến trên virtual memory đó sẽ tương ứng với địa chỉ nào trên physical memory
    - thường OS ko cho phép truy cập bộ nhớ physical trừ khi xin cấp phát
# Process identifier
## fork()
    - một process có thể được tạo bởi hàm fork(). Quy trình mới được tạo bao gồm một bản sao không gian địa chỉ của quy trình ban đầu (nghĩa là nó là một bản sao của toàn bộ bộ nhớ mà parent đang sử dụng, bản sao đó bao gồm text, data, stack...Nhưng để tối ưu thì bản sao này chỉ thực sự được tạo khi có sự thay đổi trong bộ nhớ của child). Fork() tạo quy trình mới từ quy trình hiện tại. Tiến trình hiện tại là tiến trình cha và process được tạo là child. Hàm này được gọi từ parent, cả parent và child đều tiếp tục thực thi sau hàm fork. Mẫ trả về của fork() bằng 0 đối với new process, và mã định danh của tiến trình con trả về cho parent
```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main() {
    int pid;
    pid = fork();
    if (pid==0) {
        printf("child");
    } else {
        printf("parent");
    }
    return 0;
}
```
=> kết quả trả về: "parent" -> "child"
**Giải thích**: giả sử tiến trình cha A gọi hàm fork() và tạo ra một tiến trình B, B sẽ sao chép toàn bộ bộ nhớ của A và khi đó có hai biến pid và chúng đều nhận giá trị của hàm fork(). Sự khác nhau ở đây là A gọi hàm fork() thì giá trị của hàm này trả về sẽ là ID của B, và trong B thì hàm fork này trả về trạng thái tạo thành công tức là 0. Thực chất hàm If else kia chỉ chạy vào một trường hợp nhưng do cả hai tiến trình này cùng duplicate stdout nên in ra cả hai kq.
```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main() {
    int pid;
    pid = fork();
    if (pid==0) {
        printf("child\n");
        execl("./child-program", "child-program", NULL);
    } else {
        printf("parent\n");
    }
    return 0;
}
```
**Giải thích**: vì hàm fork() chỉ tạo và sao chép lại oàn bộ chương trình cha. Để tạo một process mới hoàn toàn thì cần sử dụng thêm hàm execl(), hàm này dùng để thực thi một chương trình mới được chỉ định
```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

int main() {
    int pid;
    int wstatus =0;

    pid = fork();
    if (pid==0) {
        printf("child\n");
        execl("./child-program", "child-program", NULL);
    } else {
        printf("parent\n");
    }
    wait(&wstatus);
    return 0;
}

```
hàm execl() đã thay thế hoàn toàn không gian bộ nhớ và chương trình của process con bằng một chương trình khác
tuy nhiên khi hàm con đã thực thi xong và return signal cho hàm cha để xác nhận kết thúc, nếu ko nhận được phản hồi thì hàm con rơi vào trạng thái zombie và phải dùng hàm wait() để đợi tín hiệu từ hàm con return

# Ưu nhược điểm khi dùng thread và process
- **Thread:**
  - **Ưu điểm:**
    - **Hiệu quả về bộ nhớ:** Các thread chia sẻ cùng một không gian bộ nhớ, giúp giảm bớt tài nguyên bộ nhớ cần thiết.
    - **Giao tiếp dễ dàng:** Việc trao đổi dữ liệu giữa các thread trong cùng một process dễ dàng hơn do chúng chia sẻ bộ nhớ.
  - **Nhược điểm:**
    - **An toàn:** Nếu một thread gặp sự cố và bị crash, nó có thể làm sập cả process, ảnh hưởng đến tất cả các thread khác.

- **Process:**
  - **Ưu điểm:**
    - **Độc lập:** Mỗi process có không gian bộ nhớ riêng biệt, nên một process bị crash không ảnh hưởng đến các process khác.
  - **Nhược điểm:**
    - **Tài nguyên:** Việc tạo và quản lý nhiều process đòi hỏi nhiều tài nguyên hơn so với thread.
    - **Giao tiếp phức tạp:** Việc giao tiếp giữa các process khó khăn hơn do không gian bộ nhớ riêng biệt.
