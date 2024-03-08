# Các hàm đọc ghi file

## `open`

`open` mở một file và trả về file descriptor. File descriptor này sẽ được sử dụng trong các hàm đọc ghi file khác.

Prototype:

```c
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

Trong đó:

- `pathname`: đường dẫn đến file cần mở.
- `flags`: các cờ mở file. Các cờ mở file có thể kết hợp với nhau bằng toán tử `|`. Các cờ mở file phổ biến nhất là `O_RDONLY`, `O_WRONLY`, `O_RDWR`, `O_CREAT`, `O_APPEND`, `O_TRUNC`, `O_EXCL`, `O_SYNC`, `O_NONBLOCK`, ...
- `mode`: chỉ định quyền truy cập cho file nếu file chưa tồn tại. Đây là một số nguyên 8 bit, 3 bit đầu chỉ định quyền truy cập của owner, 3 bit tiếp theo chỉ định quyền truy cập của group và 3 bit cuối chỉ định quyền truy cập của others. Ví dụ: Để tạo ra file mới có quyền truy cập là `rw-r--r--`, ta sẽ sử dụng `0644` hoặc `S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH`.

## `close`

Để đóng một file có file descriptor là `fd`, ta sử dụng hàm `close`:

```c
int close(int fd);
```

## `read`

Hàm `read` đọc `nbyte` bytes từ file có file descriptor là `fd` vào buffer `buf`. Giá trị trả về của hàm là số bytes thực sự đọc được. Nếu giá trị trả về là `0` thì có thể là đã đọc hết file hoặc file không có dữ liệu nào để đọc.

```c
ssize_t read(int fd, void *buf, size_t nbyte);
```

## `write`

Hàm `write` ghi `nbyte` bytes từ buffer `buf` vào file có file descriptor là `fd`. Giá trị trả về của hàm là số bytes thực sự ghi được.

```c
ssize_t write(int fd, const void *buf, size_t nbyte);
```

## `lseek`

Hàm `lseek` thay đổi vị trí con trỏ đọc/ghi file của file có file descriptor là `fd`. Giá trị trả về của hàm là vị trí mới của con trỏ đọc/ghi file.

```c
off_t lseek(int fd, off_t offset, int whence);
```

Trong đó:

- `offset`: số bytes cần thay đổi vị trí con trỏ đọc/ghi file. Nếu `offset` là một số dương, con trỏ sẽ được di chuyển về phía cuối file. Nếu `offset` là một số âm, con trỏ sẽ được di chuyển về phía đầu file.
- `whence`: chỉ định cách tính toán vị trí mới của con trỏ. Có 3 giá trị có thể sử dụng:
	- `SEEK_SET`: vị trí mới của con trỏ sẽ là `offset` bytes từ đầu file.
	- `SEEK_CUR`: vị trí mới của con trỏ sẽ là `offset` bytes từ vị trí hiện tại của con trỏ.
	- `SEEK_END`: vị trí mới của con trỏ sẽ là `offset` bytes từ cuối file.

## `sync`

Hàm `sync` đảm bảo rằng tất cả các dữ liệu trong bộ nhớ cache của hệ thống sẽ được đẩy xuống ổ đĩa.

```c
void sync(void);
```

## `fsync`

Hàm `fsync` đảm bảo rằng tất cả các dữ liệu trong bộ nhớ cache của file có file descriptor là `fd` sẽ được đẩy xuống ổ đĩa.

```c
int fsync(int fd);
```

## `stat`

Hàm `stat` lấy thông tin về file từ đường dẫn `pathname` và lưu thông tin đó vào struct `stat`.

```c
int stat(const char *pathname, struct stat *buf);
```

struct `stat`:

```c
struct stat {
	dev_t     st_dev;     /* ID of device containing file */
	ino_t     st_ino;     /* inode number */
	mode_t    st_mode;    /* protection */
	nlink_t   st_nlink;   /* number of hard links */
	uid_t     st_uid;     /* user ID of owner */
	gid_t     st_gid;     /* group ID of owner */
	dev_t     st_rdev;    /* device ID (if special file) */
	off_t     st_size;    /* total size, in bytes */
	blksize_t st_blksize; /* blocksize for file system I/O */
	blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
	time_t    st_atime;   /* time of last access */
	time_t    st_mtime;   /* time of last modification */
	time_t    st_ctime;   /* time of last status change */
};
```

## `chmod`

Hàm `chmod` thay đổi quyền truy cập của file có đường dẫn `pathname` thành `mode`.

```c
int chmod(const char *pathname, mode_t mode);
```

## `chown`

Hàm `chown` thay đổi owner và group của file có đường dẫn `pathname` thành `owner` và `group`.

```c
int chown(const char *pathname, uid_t owner, gid_t group);
```

## `remove`

Hàm `remove` xóa file có đường dẫn `pathname`. Hàm trả về `0` nếu xóa file thành công và trả về `-1` nếu xóa file thất bại.

```c
int remove(const char *pathname);
```

## `rename`

Hàm `rename` đổi tên file từ `old` thành `new`. Hàm trả về `0` nếu đổi tên file thành công và trả về `-1` nếu đổi tên file thất bại.

```c
int rename(const char *old, const char *new);
```

## `link`

Hàm `link` tạo ra một hard link từ `old` tới `new`. Hàm trả về `0` nếu tạo hard link thành công và trả về `-1` nếu tạo hard link thất bại.

```c
int link(const char *old, const char *new);
```

# Thư mục

## `mkdir`

Hàm `mkdir` tạo ra một thư mục có đường dẫn `pathname` với quyền truy cập `mode`.

```c
int mkdir(const char *pathname, mode_t mode);
```

## `opendir`

Hàm `opendir` mở một thư mục có đường dẫn `pathname` và trả về con trỏ `DIR` cho thư mục đó.

```c
DIR *opendir(const char *pathname);
```

DIR là một kiểu dữ liệu đặc biệt chỉ có thể được sử dụng với hàm `readdir`.

## `readdir`

Hàm `readdir` đọc một entry từ thư mục mà con trỏ `dir` trỏ tới và trả về con trỏ `dirent` cho entry đó. Nếu không còn entry nào nữa, hàm trả về `NULL`.

```c
struct dirent *readdir(DIR *dir);
```

struct `dirent`:

```c
struct dirent {
	ino_t          d_ino;       /* inode number */
	off_t          d_off;       /* offset to the next dirent */
	unsigned short d_reclen;    /* length of this record */
	unsigned char  d_type;      /* type of file */
	char           d_name[256]; /* filename */
};
```

## `closedir`

Hàm `closedir` đóng thư mục mà con trỏ `dir` trỏ tới.

```c
int closedir(DIR *dir);
```

## `rmdir`

Hàm `rmdir` xóa thư mục có đường dẫn `pathname`. Hàm trả về `0` nếu xóa thư mục thành công và trả về `-1` nếu xóa thư mục thất bại.

```c
int rmdir(const char *pathname);
```

# Example

## Các thao tác với file

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
	// Open file test.txt in read and write mode
	// If file does not exist, create it with permission 0644
	int fd = open("test.txt", O_RDWR | O_CREAT, 0644);
	if (fd == -1) {
		perror("open");
		exit(1);
	}

	// Write "Hello" to file
	char buf[6] = "Hello";
	if (write(fd, buf, 5) == -1) {
		perror("write");
		exit(1);
	}

	// Move file pointer to the beginning of file
	if (lseek(fd, 0, SEEK_SET) == -1) {
		perror("lseek");
		exit(1);
	}

	// Read 5 bytes from file
	char read_buf[6];
	if (read(fd, read_buf, 5) == -1) {
		perror("read");
		exit(1);
	}

	read_buf[5] = '\0';
	printf("%s\n", read_buf);

	// Close file
	if (close(fd) == -1) {
		perror("close");
		exit(1);
	}

	// rename file
	if (rename("test.txt", "test2.txt") == -1) {
		perror("rename");
		exit(1);
	}

	// Change permission of file
	if (chmod("test2.txt", 0777) == -1) {
		perror("chmod");
		exit(1);
	}

	// Get file status
	struct stat st;
	if (stat("test2.txt", &st) == -1) {
		perror("stat");
		exit(1);
	}
	printf("File size: %ld\n", st.st_size);
	printf("File mode: %o\n", st.st_mode);

	// create hard link
	if (link("test2.txt", "test3.txt") == -1) {
		perror("link");
		exit(1);
	}

	// remove hard link
	if (remove("test3.txt") == -1) {
		perror("remove");
		exit(1);
	}

	// remove file
	if (remove("test2.txt") == -1) {
		perror("remove");
		exit(1);
	}

	return 0;
}
```

## Các thao tác với thư mục

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

int main() {
	// Create directory
	if (mkdir("test_dir", 0777) == -1) {
		perror("mkdir");
		exit(1);
	}

	// make some files in directory
	int fd = open("test_dir/test.txt", O_RDWR | O_CREAT, 0644);
	if (fd == -1) {
		perror("open");
		exit(1);
	}
	if (close(fd) == -1) {
		perror("close");
		exit(1);
	}

	int fd2 = open("test_dir/test2.txt", O_RDWR | O_CREAT, 0644);
	if (fd2 == -1) {
		perror("open");
		exit(1);
	}
	if (close(fd2) == -1) {
		perror("close");
		exit(1);
	}

	// make sub directories
	if (mkdir("test_dir/sub_dir", 0777) == -1) {
		perror("mkdir");
		exit(1);
	}

	// Create a file in sub directory
	int fd3 = open("test_dir/sub_dir/test3.txt", O_RDWR | O_CREAT, 0644);
	if (fd3 == -1) {
		perror("open");
		exit(1);
	}
	if (close(fd3) == -1) {
		perror("close");
		exit(1);
	}

	// Open directory
	DIR *dir = opendir("test_dir");
	if (dir == NULL) {
		perror("opendir");
		exit(1);
	}

	// Read directory
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL) {
		printf("%s\n", entry->d_name);
	}

	// Close directory
	if (closedir(dir) == -1) {
		perror("closedir");
		exit(1);
	}

	// Remove directory (it must be empty)
	if (rmdir("test_dir") == -1) {
		perror("rmdir");
		exit(1);
	}

	return 0;
}
```
