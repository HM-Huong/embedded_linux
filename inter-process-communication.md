# Inter-process communication (IPC)

Inter-process communication hay giao tiếp liên tiến trình là một cơ chế hệ điều hành cung cấp để cho phép các tiến trình (process) quản lý dữ liệu được chia sẻ. Thông thường các ứng dụng sử dụng IPC có thể được phân loại là máy khách và máy chủ, trong đó máy khách (client) yêu cầu dữ liệu và máy chủ (server) đáp ứng yêu cầu của máy khách. Trong IPC, các ứng dụng cũng có thể vừa có thể là máy khách vừa là máy chủ.

Ta có thể sử dụng IPC để truyền dữ liệu giữa 2 hoặc nhiều process với nhau. Có nhiều cách để thực hiện IPC, mỗi cách có ưu nhược điểm riêng. Dưới đây là một số phương pháp IPC thường được sử dụng để giao tiếp giữa user space và kernel space:

## Device file

Là phương pháp giao tiếp 2 chiều giữa user space và kernel space thông qua các file trong `/dev`. Khi một ứng dụng mở/đọc/ghi/... một file trong `/dev`, kernel sẽ gọi các hàm mà driver đã đăng ký với kernel. Do đó user space là người chủ động giao tiếp với kernel space vì kernel không thể tự gọi hàm của user space.

## Signal

Application và driver giao tiếp với nhau thông qua cơ chế gửi nhận signal.

**Ưu điểm**:

- Dễ dàng triển khai.
- Hỗ trợ cơ chế interrupt. Khi nhận được signal, thì application dừng các công việc đang thực hiện để xử lý signal.
- Tốc độ truyền dữ liệu nhanh.
- Có thể đính kèm các loại dữ liệu phức tạp.

**Nhược điểm**:

- Chỉ có thể chuyền dữ liệu theo một chiều từ driver đến application.
- Cần phải có PID của process để gửi signal. Điều này có thể gây khó khăn vì driver có thể được khởi động trước khi application khởi động -> không có PID để gửi signal.
- Chỉ hỗ trợ một vài loại user defined signal.

**Ví dụ**:

Code nhận signal:

```c
/*
 * This program demonstrates how to catch a signal sent to a process.
 * In this case, we are catching SIGINT signal.
 *
 * Compile and run this program with the following commands:
 * 	gcc -o receive receive.c
 * 	./receive
 *
 * To send a signal to the process, open another terminal and run one of the following commands:
 * 	kill -SIGINT <pid>
 * 	kill -s INT <pid>
 * 	kill -2 <pid>
 * 	./send <pid>
 * 
 * Or you can send a signal by pressing Ctrl+C in the terminal where the process is running.
 *
 * Replace <pid> with the process id of the process you want to send the signal to.
 * You can get the process id of the process by running:
 * 	ps -axu | grep receive
 *
 * The process id is the second column in the output.
 *
 * The process will print "received SIGINT" when it receives the signal.
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void sig_handler(int signo) {
	if (signo == SIGINT)
		printf("received SIGINT\n");
}

int main(void) {
	if (signal(SIGINT, sig_handler) == SIG_ERR)
		printf("\ncan't catch SIGINT\n");

	// A long long wait so that we can easily issue a signal to this process
	while (1)
		sleep(1);

	return 0;
}
```

Code gửi signal:

```c
/*
* Use this program to send a SIGINT signal to a process by passing the process id as an argument.
* gcc -o send send.c
* ./send <pid>
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char **argv) {
	int pid = atoi(argv[1]);

	// Send a SIGINT signal to the process
	if (kill(pid, SIGINT)) {
		perror("Cannot send signal");
	}

	return 0;
}
```

## Shared memory

Driver và application chia sẻ và sử dụng một vùng nhớ chung. Thường được sử dụng khi truyền dữ liệu theo một chiều.

**Ưu điểm**:

- Tốc độ truyền dữ liệu nhanh.
- Có thể chia sẻ được các loại dữ liệu phức tạp

**Nhược điểm**:

- Không có cơ chế bảo vệ dữ liệu khi bị truy cập đồng thời
- Chỉ có thể sử dụng phương pháp polling để kiểm tra dữ liệu mới

## Socket

## User space helper API
