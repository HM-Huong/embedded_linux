# C

## Declaration specifier

### Storage classes

#### `auto`

#### `static`

Biến `static` có thể được khai báo trong một hàm hoặc ngoài hàm.

- Nếu biến `static` được khai báo trong một hàm, nó sẽ không bị hủy khi hàm kết thúc. Giá trị của biến `static` trong lần gọi hàm tiếp theo sẽ là giá trị của biến `static` trong lần gọi hàm trước đó. Biến `static` trong hàm chỉ được khởi tạo một lần duy nhất và ta phải khởi tạo nó ngay khi khai báo.

	```c
	void func() {
		static int cnt = 0;
		cnt++;
		printf("call %d times\n", cnt);
	}

	int main() {
		func(); // call 1 times
		func(); // call 2 times
		func(); // call 3 times
		return 0;
	}
	```

- Nếu biến `static` được khai báo ngoài hàm, nó sẽ có phạm vi truy cập là file scope. Điều này có nghĩa là biến `static` chỉ có thể được truy cập trong file mà nó được khai báo.

#### `extern`

Ta có thể sử dụng từ khóa `extern` để khai báo một biến hoặc hàm mà đã được định nghĩa ở một file khác. Điều này có nghĩa là biến hoặc hàm đã được định nghĩa ở một chỗ khác và ta muốn sử dụng nó trong file hiện tại.

```c
extern int x; // thông báo rằng biến x đã được định nghĩa ở một chỗ khác (có thể là trong file hiện tại hoặc file khác)
```

Khi ta khai báo một hàm, từ khóa `extern` được sử dụng một cách ngầm định do đó ta có thể bỏ qua nó:

```c
int func(); // tương đương với extern int func();
```

##### Declaration vs Definition

Khai báo hàm hoặc biến có tác dụng thông báo rằng hàm hoặc biến đó tồn tại ở đâu đó trong chương trình.Tác dụng chủ yếu của khai báo biến là để cho biết kiểu dữ liệu của biến hoặc hàm (kiểu dữ liệu của giá trị trả về cũng như số lượng tham số và kiểu dữ liệu của mỗi tham số).

Định nghĩa hàm hoặc biến ngoài tác dụng khai báo thì nó còn có tác dụng cấp phát bộ nhớ cho biến hoặc hàm. Nói cách khác khai báo hàm là một tập con của định nghĩa hàm.

#### `register`

### Type qualifiers

#### `const`

#### `volatile`

[Đọc thêm](https://stackoverflow.com/questions/18695120/volatile-and-cache-behaviour)

Volatile báo cho complier biết rằng giá trị của biến sẽ giữ nguyên giữa các lần đọc/ghi dữ liệu. Nói cách khác giá trị của biến có thể thay đổi bất kỳ lúc nào mà không cần phải thông qua mã nguồn của chương trình (memory-mapped I/O, DMA, ...). Do đó complier không được phép tối ưu hóa code chẳng hạn `v = 1; r = v` thành `v = 1; r = 1`.

Ta cũng cần phải lưu ý rằng từ khóa `volatile` chỉ đảm bảo việc complier không tối ưu hóa code. Nó không đảm bảo việc đọc/ghi dữ liệu an toàn giữa các thread (vì dữ liệu có thể bị cache trên phần cứng). Do đó để đảm bảo việc đọc/ghi dữ liệu an toàn giữa các thread, ta cần sử dụng các cơ chế như mutex, semaphore, atomic, ...

#### `restrict`

[Đọc thêm](https://en.wikipedia.org/wiki/Restrict#:~:text=In%20the%20C%20programming%20language,object%20to%20which%20it%20points.)

In the C programming language, restrict is a keyword, introduced by the C99 standard, that can be used in pointer declarations. By adding this type qualifier, a programmer **hints to the compiler that for the lifetime of the pointer, no other pointer will be used to access the object to which it points**.

### Type specifiers

like keywords: void, char, short, int, etc.

## Biên dịch

Biên dịch là một quá trình chuyển đổi mã nguồn từ một ngôn ngữ lập trình sang mã máy. Có nhiều công cụ để biên dịch mã nguồn C, một trong những công cụ phổ biến nhất là `gcc`. Để cài đặt `gcc` và các công cụ hữu ích khác, sử dụng lệnh sau:

```bash
sudo apt install build-essential
```

Để biên dịch một file C ra một file thực thi (`a.out` trên Linux hoặc `a.exe` trên Windows), sử dụng lệnh sau:

```bash
gcc file_name.c
```

Để chỉ định tên file thực thi, sử dụng `-o` flag:

```bash
gcc file_name.c -o executable_name
```

### Các giai đoạn biên dịch

[Đọc thêm](https://www.thegeekstuff.com/2011/10/c-program-to-an-executable/)

#### Preprocessing

Đây là giai đoạn đầu tiên trong quá trình biên dịch. Một số công việc được thực thi trong giai đoạn này là:

1. Thay thế tất cả các macro bằng nội dung của chúng.
2. Xóa tất cả các comment.
3. Thêm tất cả các file header mà chúng ta sử dụng vào file code.

Để chỉ thực hiện giai đoạn này, sử dụng `-E` flag:

```bash
gcc -E file_name.c -o output_name.i
# hoặc
gcc -E file_name.c
```

Ngoài ra ta cũng có thể sử dụng flag `--save-temps` để lưu tất cả các file trung gian trong quá trình biên dịch:

```bash
gcc --save-temps file_name.c -o executable_name
```

> Chú ý: đầu ra của quá trình tiền xử lý được lưu trữ trong file có đuôi `.i`.

#### Compiling

Trình biên dịch sẽ tổng hợp các file `.i` của quá trình tiền xử lý thành các file assembly (`.s`).

```bash
gcc -S file_name.c
```

#### Assembling

Trình biên dịch (assembler) sẽ chuyển đổi các file assembly thành các file object (`.o`) chứa mã máy (dưới dạng ELF - executable and linkable format).

```bash
gcc -c file_name.c
```

#### Linking

Trình liên kết sẽ kết hợp các file object thành một file thực thi. Trong giai đoạn này, linker sẽ liên kết các lời gọi hàm với các địa chỉ của hàm thực tế (trước giai đoạn này các lời gọi hàm chỉ là các tham chiếu tương đối và ta không biết vị trí của định nghĩa của hàm đó ở đâu. Ví dụ như các hàm trong thư viện chuẩn `printf`, `scanf`, ...)

```bash
gcc file_name.o -o executable_name
```

Ngoài ra linker cũng thêm một số code khi chương trình bắt đầu và kết thúc. Ví dụ như khi chương trình bắt đầu, hàm `main` sẽ nhận được các đối số khi chương trình được gọi từ terminal và khi chương trình kết thúc thì giá trị trả về của hàm `main` sẽ được trả về cho hệ điều hành.

### Các flag hữu ích

[Đọc thêm](https://www.thegeekstuff.com/2012/10/gcc-compiler-options/)

#### `-Wall` - Hiển thị tất cả các cảnh báo

#### `-Werror` - Chuyển tất cả các cảnh báo thành lỗi

#### `-g` - Thêm thông tin gỡ lỗi vào file thực thi

#### `-std` - Chỉ định phiên bản của ngôn ngữ C

#### `-I` - Thêm thư mục chứa file header

```bash
gcc file_name.c -o executable_name -I/path/to/header
```

#### `-l` - Liên kết với shared library

```bash
gcc file_name.c -o executable_name -lpthread
```

Câu lệnh trên sẽ liên kết chương trình với thư viện `pthread`.

#### `-fPIC` (Position Independent Code) và `-shared`

Shared library là loại thư viện mà nhiều chương trình có thể sử dụng chung và được load vào bộ nhớ khi chương trình chạy. Khi update một shared library, tất cả các chương trình sử dụng thư viện đó sẽ được cập nhật mà không cần phải biên dịch lại chương trình.

Sử dụng thư viện động sẽ tiết kiệm bộ nhớ hơn so với static lib. Vì static lib sẽ include toàn bộ source code của các hàm mà chương trình gọi đến vào source code của chương trình. Như vậy trong hệ thống có thể có nhiều vùng nhớ cùng chứa 1 đoạn source code của 1 thư viện tính nào đó.

Để tạo ra một file thư viện động, ta cần tạo ra một file thư viện có mã máy không phụ thuộc vào vị trí của nó trong bộ nhớ. Điều này có nghĩa là khi chương trình chạy, nó có thể load file thư viện ở bất kỳ vị trí nào trong bộ nhớ. Để tạo ra file thư viện động, ta cần thêm flag `-fPIC` khi biên dịch file thư viện.

```bash
gcc -shared -fPIC -o libname.so file_name.c
```

Để sử dụng file thư viện động, ta cần chỉ định đường dẫn đến file thư viện khi biên dịch chương trình:

```bash
gcc file_name.c -o executable_name -L/path/to/library -lname -Wl,-rpath,.
```

Ví dụ như:

```bash
gcc file_name.c -o executable_name -L. -lname -Wl,-rpath,.
```

Trong đó:

- `-L.` chỉ định compiler tìm file thư viện trong thư mục hiện tại.
- `-lname` chỉ định tên của file thư viện có dạng `libname.so`. Ví dụ như nếu file thư viện có tên là `libmylib.so` thì ta sẽ sử dụng `-lmylib`.
- `-Wl,-rpath,.`: `Wl` (Warning linker???) chỉ định các flag cho linker. Các flag này sẽ được ngăn cách với nhau bởi dấu `,`. Trong trường hợp này, ta chỉ định rằng khi chạy chương trình, linker sẽ tìm file thư viện trong thư mục chứa file thực thi. Nếu không chỉ định thì mặc định linker sẽ tìm file thư viện trong các thư mục được chỉ định trong biến môi trường `LD_LIBRARY_PATH`.

#### `-v` (verbose) - Hiển thị thông tin chi tiết về quá trình biên dịch

#### `-D` - Định nghĩa macro

```bash
gcc -DDEBUG file_name.c -o executable_name
```

Câu lệnh trên sẽ định nghĩa macro `DEBUG` (và giá trị của nó sẽ là `1`)

## Gỡ lỗi

[Đọc thêm](https://www.thegeekstuff.com/2010/03/debug-c-program-using-gdb/)

## Makefile

Makefile có khả năng kiểm tra sự thay đổi trong source và chỉ build lại những file source có thay đổi.

[Đọc thêm](https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/)
