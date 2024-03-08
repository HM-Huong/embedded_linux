# Tạo một thread

```c
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
```

Trong đó:

- `pthread_create` tạo một thread mới và thực thi hàm `start_routine` với tham số `arg`. Giá trị trả về của hàm này là 0 nếu tạo thread thành công, ngược lại là một giá trị khác 0.
- `thread` là một con trỏ tới một biến kiểu `pthread_t` để lưu trữ id của thread mới.
- `attr` là một con trỏ tới một biến kiểu `pthread_attr_t` để chỉ định thuộc tính của thread mới. Nếu `attr` là `NULL`, thread mới sẽ được tạo với các thuộc tính mặc định.
- `start_routine` là một con trỏ tới một hàm mà thread mới sẽ thực thi.
- `arg` là tham số truyền vào hàm `start_routine`.

# Chờ đợi một thread kết thúc

```c
int pthread_join(pthread_t thread, void **retval);
```

Trong đó:

- `pthread_join` chờ đợi cho đến khi thread với id `thread` kết thúc. Nếu thread này đã kết thúc, hàm này trả về ngay lập tức. Nếu không, hàm này sẽ chờ đợi cho đến khi thread kết thúc. Hàm này trả về 0 nếu thành công, ngược lại trả về một giá trị khác 0.
- `retval` là một con trỏ tới một biến kiểu `void *` để lưu trữ giá trị trả về của thread. Nếu `retval` là `NULL`, giá trị trả về của thread sẽ không được lưu trữ.

# Kết thúc một thread

## `return`

Nếu hàm thực thi của thread kết thúc, thread cũng kết thúc. Giá trị trả về của hàm thực thi sẽ là giá trị trả về của thread.

## `pthread_exit`

`pthread_exit` kết thúc thread hiện tại và trả về giá trị `retval` cho thread gọi hàm `pthread_join`:

```c
void pthread_exit(void *retval);
```

## `pthread_cancel`

`pthread_cancel` hủy bỏ thread với id `thread`. Hàm này trả về 0 nếu thành công, ngược lại trả về một giá trị khác 0.

```c
int pthread_cancel(pthread_t thread);
```

# Ví dụ

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *print_message_function(void *ptr);

int main() {
	pthread_t thread1, thread2;
	char *message1 = "Thread 1";
	char *message2 = "Thread 2";

	/* Create independent threads each of which will execute function */
	if (pthread_create(&thread1, NULL, print_message_function, (void *)message1) != 0) {
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&thread2, NULL, print_message_function, (void *)message2) != 0) {
		perror("pthread_create");
		exit(1);
	}

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	return 0;
}

void *print_message_function(void *ptr) {
	char *message;
	message = (char *)ptr;
	printf("%s \n", message);
}
```

> Lưu ý:

- khi biên dịch chương trình, bạn cần thêm tham số `-lpthread` để link với thư viện `pthread`:

	```sh
	gcc -o main main.c -lpthread
	```

- Nếu 2 thread trên cùng in ra màn hình một lúc thì chữ trên màn hình có thể bị trộn lẫn. Để tránh trường hợp này, bạn có thể sử dụng mutex hoặc semaphore.

# Các cơ chế đồng bộ trong multi-threading

## Mutex

Mutex (viết tắt của Mutual Exclusion) là một cơ chế đồng bộ hóa dùng để ngăn chặn việc truy cập đồng thời vào một tài nguyên chia sẻ giữa các thread. Mutex có hai trạng thái: khóa và mở. Khi một thread khóa một mutex, các thread khác sẽ không thể truy cập vào tài nguyên chia sẻ cho đến khi mutex được mở.

```c
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

Trong đó:

- `pthread_mutex_init` khởi tạo một mutex với các thuộc tính được chỉ định bởi `attr`. Nếu `attr` là `NULL`, mutex sẽ được khởi tạo với các thuộc tính mặc định.
- `pthread_mutex_destroy` hủy bỏ một mutex đã được khởi tạo.
- `pthread_mutex_lock` khóa một mutex. Nếu mutex đã được khóa bởi một thread khác, hàm này sẽ chờ đợi cho đến khi mutex được mở.
- `pthread_mutex_trylock` thử khóa một mutex. Nếu mutex đã được khóa bởi một thread khác, hàm này sẽ trả về ngay lập tức với giá trị khác 0. Ngược lại, hàm này sẽ khóa mutex và trả về 0. -> Đây là hàm không chờ đợi.
- `pthread_mutex_unlock` mở một mutex.

### Ví dụ

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t print_mutex;

void *print_message_function(void *ptr);

int main() {
	pthread_t thread1, thread2;
	char *message1 = "Thread 1";
	char *message2 = "Thread 2";

	pthread_mutex_init(&print_mutex, NULL);

	/* Create independent threads each of which will execute function */
	if (pthread_create(&thread1, NULL, print_message_function, (void *)message1) != 0) {
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&thread2, NULL, print_message_function, (void *)message2) != 0) {
		perror("pthread_create");
		exit(1);
	}

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	pthread_mutex_destroy(&print_mutex);

	return 0;
}

void *print_message_function(void *ptr) {
	char *message;
	message = (char *)ptr;

	pthread_mutex_lock(&print_mutex);
	printf("%s \n", message);
	pthread_mutex_unlock(&print_mutex);
}
```

## Deadlock

Là trạng thái trong hệ thống có 2 hoặc nhiều luồng xử lý đi vào vòng lặp vô tận để chờ đợi tài nguyên của nhau. Ví dụ: deadlock xảy ra khi một luồng xử lý không thể tiếp tục thực thi vì nó đang chờ đợi một tài nguyên mà một luồng khác đang sở hữu, và luồng đang sở hữu tài nguyên đó cũng đang chờ đợi một tài nguyên khác mà luồng đầu tiên đang sở hữu.

Cách tránh deadlock:

- Giảm số lượng lock trong source code.
- Sử dụng mutex theo thứ tự cố định.
- Sử dụng hàm `pthread_mutex_trylock` thay vì `pthread_mutex_lock` để tránh chờ đợi vô hạn.
- Sử dụng hàm `pthread_mutex_timedlock` để tránh chờ đợi vô hạn.

## Semaphore

Semaphore là một cơ chế đồng bộ hóa dùng để kiểm soát việc truy cập vào một tài nguyên chia sẻ giữa các thread. Khác với mutex, semaphore có thể cho phép nhiều thread truy cập vào tài nguyên chia sẻ cùng một lúc. Nói cách khác, tại một thời điểm có thể có một hoặc nhiều thread có được khóa.

```c
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem, int *sval);
```

Trong đó:

- `sem_init` khởi tạo một semaphore với giá trị ban đầu là `value`. Nếu `pshared` là 0, semaphore chỉ có thể được sử dụng trong một process. Nếu `pshared` là 1, semaphore có thể được sử dụng trong nhiều process.
- `sem_destroy` hủy bỏ một semaphore đã được khởi tạo.
- `sem_wait` giảm giá trị của semaphore đi 1. Nếu giá trị của semaphore là 0, hàm này sẽ chờ đợi cho đến khi giá trị của semaphore khác 0.
- `sem_trywait` giảm giá trị của semaphore đi 1. Nếu giá trị của semaphore là 0, hàm này sẽ trả về ngay lập tức với giá trị khác 0. Ngược lại, hàm này sẽ giảm giá trị của semaphore và trả về 0.
- `sem_post` tăng giá trị của semaphore lên 1.
- `sem_getvalue` lấy giá trị của semaphore.

### Ví dụ

```c
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

sem_t print_semaphore;

void *print_message_function(void *ptr);

int main() {
	pthread_t thread1, thread2;
	char *message1 = "Thread 1";
	char *message2 = "Thread 2";

	sem_init(&print_semaphore, 0, 1);

	/* Create independent threads each of which will execute function */
	if (pthread_create(&thread1, NULL, print_message_function, (void *)message1) != 0) {
		perror("pthread_create");
		exit(1);
	}

	if (pthread_create(&thread2, NULL, print_message_function, (void *)message2) != 0) {
		perror("pthread_create");
		exit(1);
	}

	/* Wait till threads are complete before main continues. Unless we  */
	/* wait we run the risk of executing an exit which will terminate   */
	/* the process and all threads before the threads have completed.   */
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	sem_destroy(&print_semaphore);

	return 0;
}

void *print_message_function(void *ptr) {
	char *message;
	message = (char *)ptr;

	sem_wait(&print_semaphore);
	printf("%s \n", message);
	sem_post(&print_semaphore);
}
```