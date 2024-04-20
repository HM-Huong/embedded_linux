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

### Triển khai

1. Dùng một chuỗi string để tạo định danh cho shared memory bằng hàm `shm_open()`. Hàm này trả về một file descriptor cho shared memory. Chú ý: 2 tiến trình cần phải có cùng tên định danh shared memory để có thể truy cập vào cùng một shared memory.

	```c
	int shm_fd = shm_open("Identifier name", O_CREAT | O_RDWR, 0666);
	```

2. Đặt kích thước cho shared memory bằng hàm `ftruncate()`. Kích thước nên bằng với kích thước của một page (4096 bytes)  vì shared memory được cấp phát theo page.

	```c
	ftruncate(shm_fd, SIZE);
	```

3. Map shared memory vào vùng nhớ của process bằng hàm [`mmap()`](https://man7.org/linux/man-pages/man2/mmap.2.html):

	```c
	void *mmap (void *addr, size_t length, int prot, int flags, int fd, off_t offset);

	// Ví dụ:
	void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	```

	Trong đó các tham số:
	- `addr`: (Gợi ý cho kernel) địa chỉ bắt đầu của vùng nhớ mà shared memory sẽ được map vào. Nếu `addr` là 0, kernel sẽ tự chọn một địa chỉ phù hợp.
	- `length`: kích thước của shared memory.
	- `prot`: quyền truy cập của shared memory. Có thể là `PROT_READ`, `PROT_WRITE`, `PROT_EXEC` hoặc kết hợp của chúng.
	- `flags`: cờ chỉ định cách thức map. Có thể là `MAP_SHARED`, `MAP_PRIVATE`, `MAP_ANONYMOUS`, `MAP_FIXED` hoặc kết hợp của chúng.
	- `fd`: file descriptor của shared memory.
	- `offset`: offset của shared memory kể từ đầu file.

4. Sử dụng shared memory:

	Sau khi đã map shared memory vào vùng nhớ của process, ta có thể sử dụng con trỏ `ptr` để đọc hoặc ghi dữ liệu vào shared memory:

	```c
	sprintf(ptr, "%s", "Hello, shared memory!");
	```

5. Giải phóng shared memory:

	Khác với `malloc`, shared memory **không tự giải phóng khi process kết thúc** (vì có thể có nhiều process sử dụng shared memory). Do đó, cần phải giải phóng shared memory bằng hàm `shm_unlink()`:

	```c
	shm_unlink("Identifier name");
	```

	Khi tất cả các process sử dụng shared memory gọi hàm `shm_unlink()`, thì shared memory mới thực sự được giải phóng. Nói cách khác nếu còn ít nhất một process sử dụng shared memory, thì shared memory vẫn tồn tại.

### Demo

Chương trình sau sẽ tạo một shared memory và ghi dữ liệu vào shared memory:

```c
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	const int SIZE = 4096;
	const char *name = "OS";
	const char *message = "Hello, shared memory!";

	// 1. Create shared memory
	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	// 2. Set the size of shared memory
	ftruncate(shm_fd, SIZE);

	// 3. Map shared memory into the address space of the process
	void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

	// 4. Write to shared memory
	sprintf(ptr, "%s", message);

	return 0;
}
```

Chương trình sau sẽ đọc dữ liệu từ shared memory:

```c
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	const int SIZE = 4096;
	const char *name = "OS";

	// 1. Open shared memory
	int shm_fd = shm_open(name, O_RDONLY, 0666);

	// 2. Map shared memory into the address space of the process
	void *ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

	// 3. Read from shared memory
	printf("%s\n", (char *)ptr);

	// 4. remove the shared memory
	shm_unlink(name);

	return 0;
}
```

**Notes**: Biên dịch 2 chương trình trên với lệnh sau:

```bash
gcc -o writer writer.c -lrt
gcc -o reader reader.c -lrt
```

## Socket

## User space helper API
