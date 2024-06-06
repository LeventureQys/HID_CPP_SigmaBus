# HIDAPI 库头文件分析

## 概述
HIDAPI提供了一系列接口用于与HID（人机交互设备）进行通信。这包括枚举设备、打开和关闭设备、读写数据等功能。

## 主要功能与API
### 初始化与结束
- `hid_init()`: 初始化HIDAPI库。这个函数通常在应用程序开始时调用，用于准备库的使用。
- `hid_exit()`: 清理HIDAPI库使用的资源，通常在程序结束时调用。

### 设备操作
- `hid_enumerate(unsigned short vendor_id, unsigned short product_id)`: 枚举系统中匹配给定VID和PID的所有HID设备。
- `hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t* serial_number)`: 根据VID、PID和可选的序列号打开HID设备。
- `hid_open_path(const char* path)`: 通过设备路径打开HID设备。
- `hid_close(hid_device* dev)`: 关闭已打开的HID设备。

### 数据传输
- `hid_write(hid_device* dev, const unsigned char* data, size_t length)`: 向设备写入输出报告。
- `hid_read(hid_device* dev, unsigned char* data, size_t length)`: 读取设备的输入报告。
- `hid_read_timeout(hid_device* dev, unsigned char* data, size_t length, int milliseconds)`: 带超时的读取输入报告。
- `hid_set_nonblocking(hid_device* dev, int nonblock)`: 设置设备的阻塞/非阻塞读模式。

### 特性报告
- `hid_send_feature_report(hid_device* dev, const unsigned char* data, size_t length)`: 向设备发送特性报告。
- `hid_get_feature_report(hid_device* dev, unsigned char* data, size_t length)`: 获取设备的特性报告。

### 错误处理
- `hid_error(hid_device* dev)`: 获取与最后一次操作相关的错误信息。

### 设备信息获取
- `hid_get_manufacturer_string(hid_device* dev, wchar_t* string, size_t maxlen)`: 获取制造商字符串。
- `hid_get_product_string(hid_device* dev, wchar_t* string, size_t maxlen)`: 获取产品字符串。
- `hid_get_serial_number_string(hid_device* dev, wchar_t* string, size_t maxlen)`: 获取序列号字符串。
- `hid_get_indexed_string(hid_device* dev, int string_index, wchar_t* string, size_t maxlen)`: 根据索引获取设备字符串。

### 版本信息
- `hid_version()`: 获取库的版本信息。
- `hid_version_str()`: 获取库的版本字符串。

### 事件通知
- `hid_register_device_notification(hid_pnp_callback fn, void* user_data)`: 注册设备拔插事件的回调函数。

### 其他工具
- `hid_max_input_report_len(hid_device* device)`: 获取设备的最大输入报告长度。

## 使用环境和宏定义
HIDAPI 支持Windows、MacOS和Linux等多种平台。文件中包含了多个宏定义来处理API的导出和调用约定，以便于在不同的操作系统上使用。

## 结构体定义
- `hid_device_info`: 包含了设备的路径、VID、PID、序列号、制造商和产品字符串等信息。

## 用法示例
```c
hid_device *handle;
handle = hid_open(0x046D, 0xC077, NULL);
if (handle) {
    unsigned char data[256];
    hid_read(handle, data, sizeof(data));
    hid_close(handle);
}
