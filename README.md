
# 前言
关于HID通信协议，这里具体操作不细说了，主要说说这个框架
框架本身比较简单，通过总分的方式来进行的，即 DeviceManager 管理很多个Device。
其中一个Manager只对应一种设备，即一个pid和vid的组合，下属每一个Device对象控制一个单独的设备，不同的设备之间仅通过path进行区分，在Manager中会为每一个path 或者说Device对象分配一个index，以供更上层做区分。

对于不同的设备，用户需要重写DeviceManager类，其余的接口不需要修改，主要是重写AnalyseRandomData 和 AnalyseContinuousData 这两个纯虚接口，所有的消息都会通过回调走到这两个接口里面来，用于应该根据不同设备的协议参数内容重写一个新的DeviceManager类。

可能之后会提供一个上层的管理所有DeviceManager类，不过现在暂时未提供。

本工程不包含Qt内容，所有的数据传递均使用回调函数 。

**注：没有提供Windows导出符号，如果需要可以修改工程提issue，建议是直接使用CMake开发，就没有这种问题了**

 **注：目前暂未有4位通信的支持** 

**注：本项目可能会修改，但是文档并不是实时修改，文档仅作参考，以实际代码为准
简单说下每个文件的作用**
## 每个文件的作用

### 1. CRC.h
此头文件中的主要是用于计算CRC数值的，具体使用都在内部，不需要上层用户去验证CRC

- **CRC 类型枚举**：
 - `CRCType` 枚举定义了三种CRC类型：`crc_8`、`crc_16`、`crc_32`，用于指定计算CRC时使用的具体类型。 
 
 - **类方法**： 

1. `static uint32_t Crc(const uint8_t* buf, uint32_t size, CRCType type)` - 计算并返回指定类型的CRC校验和。 - 参数： - `buf`: 数据缓冲区的指针。 - `size`: 缓冲区中的数据字节数。 - `type`: 指定的CRC类型（`CRCType` 枚举值）。 - 返回值：根据指定的CRC类型返回相应的校验和。
 2. `static uint8_t Crc8(const uint8_t* buf, uint32_t size)` - 计算并返回8位CRC校验和。 
 3. `static uint32_t Crc32(const uint8_t* buf, uint32_t size)` - 计算并返回32位CRC校验和。 
 4. `static uint16_t Crc16(const uint8_t* buf, uint32_t size)` - 计算并返回16位CRC校验和。


### 2. HID_Params.h HID 设备通信头文件

#### 概览
该头文件定义了一系列与HID设备进行通信所需的数据结构和函数。主要涉及数据的打包和解析，以支持不同类型的CRC校验和多种通信命令。

#### 命名空间定义

##### `HIDParam`
在`HIDParam`命名空间中定义了多个常用的帧头和命令类型，这些常量用于标识不同类型的数据包：

- `Header`：固定的帧头字节。
- `CC_ID`：保留的命令码，不应使用。
- `CC_ERROR_CODE`：错误响应帧的命令码。
- `CC_PARAM_RAND_WR`：随机写参数命令。
- `CC_PARAM_RAND_RD`：随机读参数命令。
- `CC_PARAM_SEQU_WR`：连续写参数命令。
- `CC_PARAM_SEQU_RD`：连续读参数命令。
- `CC_PARAM_RAND_WR_EXT`：扩展随机写参数命令。
- `CC_PARAM_RAND_RD_EXT`：扩展随机读参数命令。
- `CC_PARAM_SEQU_WR_EXT`：扩展连续写参数命令。
- `CC_PARAM_SEQU_RD_EXT`：扩展连续读参数命令。
- `CC_DFU`：固件升级命令。
- `CC_CTRL`：线控通信命令。

#### 数据结构

##### `DataRandom`
描述随机读写数据包的结构体，包含以下字段：
- `report`：报文ID。
- `frame_header`：帧头。
- `frame_len_L` & `frame_len_H`：帧长度的低字节和高字节。
- `cp_addr`：设备地址。
- `command`：命令编码。
- `param_addr` & `param_value`：参数地址和值。
- `CRC_L` & `CRC_H`：CRC校验的低字节和高字节。
- `toData()`：将结构体转换成字节数组的方法。

##### `ReadRandomDataPack`
描述读取随机参数请求的结构体，字段与`DataRandom`相似但专用于读取操作。

##### `DataContinuous` 和 `ReadDataContinuous`
描述连续地址读写请求的结构体。除了基本的帧头、长度、设备地址等，还包含了：
- `param_addr`：参数的地址。
- `param_num`：参数数量或长度。
- `param_value`：参数的值（连续地址的数据）。
- `crc_L` & `crc_H`：CRC校验的低字节和高字节。
- `toData()`：同样用于将结构体数据转换成字节数组。

#### 函数实现

##### `CreateDataRandom` 和类似的创建函数
这些函数用于初始化相应的数据结构体实例，设置必要的参数，并计算CRC。

##### `ParseDataRandom` 和类似的解析函数
用于从字节数组中解析出相应的数据结构体实例，验证数据完整性，并执行必要的数据提取操作。

##### `FreeDataRandom` 和类似的释放函数
用于释放分配的动态内存，清理资源。


### 3. HID 设备通信接口头文件概述

#### 概览
此头文件包含了定义通信设备接口 `ICommunicateDevice` 和设备管理类 `ICommDeviceMgmt` 的声明。这些接口和类用于处理与 HID 设备的基本通信需求。

#### 接口 `ICommunicateDevice`

##### 功能描述
定义了与单个 HID 设备通信所需的基本功能。

##### 主要方法

- `Init()`: 初始化设备。
- `GetDeviceParam1()` 和 `GetDeviceParam2()`: 获取设备的参数，如 vendor ID 和 product ID。
- `IsDeviceOpen()`: 判断设备是否已经打开。
- `GetDeviceName()`: 获取设备名称。
- `OpenDevice()`: 打开设备，需要设备路径和相关参数。
- `CloseDevice()`: 关闭设备。
- `Write()`: 向设备发送数据。
- `Read()`: 从设备接收数据。
- `GetMaxReportLength()`: 获取设备报文数据的最大长度。
- `GetDevicePath()`: 获取设备的路径。
- `registerReadCallback()` 和 `unregisterReadCallback()`: 注册和注销数据接收的回调函数。

##### 通信方式
提供了回调函数机制来处理异步数据接收。

#### 类 `ICommDeviceMgmt`

##### 功能描述
管理一类设备，区分设备种类通过 vendor ID 和 product ID。

##### 主要方法

- `Init()`: 初始化设备管理器。
- `AutoConnectDevice()`: 自动枚举和连接现有的设备。
- `GetDeviceCount()`: 获取同种设备的数量。
- `IsDeviceConnect()`: 判断指定设备是否连接。
- `PathConnectDevice()`: 根据路径连接设备。
- `DisconnectDevice()`: 断开设备连接。
- `GetDevicesInfo()` 和 `GetDeviceInfo()`: 获取设备插入的信息。
- `SendRandom()` 和 `SendContinuous()`: 发送数据包并接收应答。
- `AnalyseRandomData()` 和 `AnalyseContinuousData()`: 分析接收到的数据包。

##### 设备信息结构 `DeviceInfo`
提供了设备的详细信息，包括设备路径、vendor ID、product ID、序列号、制造商等。

#### 使用场景

此接口和类的定义适用于需要与多种 HID 设备进行通信的应用程序，例如设备配置软件、设备监控系统等。

#### 注意事项

- 设备通信通常涉及硬件级别的交互，因此实现时需要考虑操作系统的兼容性和设备驱动的支持。
- 异步通信和回调函数的使用需要特别注意线程安全和数据同步问题。

### 4.CHID_Device 类概述



#### 类功能
`CHID_Device` 类提供了对HID设备的基本操作支持，包括设备的初始化、打开、关闭、读写操作等。该类是实现 `ICommunicateDevice` 接口的具体类。

#### 构造函数和析构函数
- `CHID_Device()`：构造函数，初始化设备相关的成员变量。
- `~CHID_Device()`：析构函数，确保设备被正确关闭，并清理相关资源。

#### 公共成员函数
- `bool Init()`：初始化设备，配置必要的设置。
- `bool OpenDevice(const char* dev_path, uint32_t vid, uint32_t pid, void* param3)`：打开指定路径和ID的设备。
- `uint32_t GetDeviceParam1() const`：返回设备的vendor ID（VID）。
- `uint32_t GetDeviceParam2() const`：返回设备的product ID（PID）。
- `bool IsDeviceOpen()`：检查设备是否已经打开。
- `const char* GetDeviceName() const`：获取设备的名称。
- `const char* GetDevicePath() const`：获取设备的路径。
- `void CloseDevice()`：关闭设备，释放相关资源。
- `int GetMaxReportLength()`：获取设备能接收的最大报告长度。
- `bool Write(unsigned char* data, size_t data_bytes)`：向设备写入数据。
- `bool Read(unsigned char* buff, size_t& data_size, int waitTime)`：从设备读取数据。
- `void LoopRead()`：在一个循环中持续读取数据，通常运行在一个独立的线程。

#### 私有成员变量
- `char device_path[256]`：存储设备的路径。
- `char name[128]`：设备的名称。
- `hid_device* handle`：指向底层HID API的设备句柄。
- `int input_report_length`：设备的输入报告长度。
- `void* user_data`：用户数据，用于回调函数。
- `bool enum_device`：是否正在枚举设备。
- `uint32_t device_vid`：设备的vendor ID。
- `uint32_t device_pid`：设备的product ID。
- `bool blnRunning`：线程运行控制标志。
- `std::mutex mtx`：互斥锁，用于同步。
- `std::thread readThread`：读取数据的线程。

#### 设备操作
设备操作包括通过 `OpenDevice` 打开设备，通过 `Write` 和 `Read` 与设备进行通信，通过 `CloseDevice` 关闭设备。设备的所有交互操作都应确保设备已经通过 `OpenDevice` 成功打开。

#### 设备读取线程
`LoopRead` 方法在一个独立的线程中持续读取设备数据，适用于需要持续监控设备状态或数据的场景。

#### 设计理念
该类的设计旨在提供一个简洁的接口，用于基本的设备操作，而复杂的设备管理和高级功能应由上层应用实现。

### 5. CHID_DeviceManager 类概述

需要注意的是，这里只对同样vid和pid的设备进行管理，如果需要管理不同vid和pid的设备，则需要在更上层进行

#### 类功能
`CHID_DeviceManager` 类负责管理一组HID设备，提供设备的初始化、枚举、连接、断开连接以及数据通信功能。该类实现了设备管理的高层逻辑，包括设备的自动连接和断开，以及数据的发送和接收。

#### 构造函数和析构函数
- `CHID_DeviceManager()`：构造函数，初始化类成员变量。
- `~CHID_DeviceManager()`：析构函数，确保所有资源被正确释放，包括断开所有连接的设备。

#### 公共成员函数
- `bool Init(int vid, int pid)`：根据提供的vendor ID (vid) 和 product ID (pid) 初始化设备管理器。
- `bool AutoConnectDevice()`：自动检测并连接所有符合当前设置的HID设备。
- `int GetDeviceCount()`：返回当前连接的设备数量。
- `bool IsDeviceConnect(int index)`：根据索引检查指定的设备是否已连接。
- `bool PathConnectDevice(char* path)`：通过设备的路径连接设备。
- `void DisconnectDevice(int index = 0)`：断开指定索引的设备连接，如果索引为0，则断开所有设备。
- `std::vector<std::shared_ptr<DeviceInfo>> GetDevicesInfo()`：获取所有已连接设备的详细信息列表。
- `std::shared_ptr<DeviceInfo> GetDeviceInfo(int index)`：获取指定索引设备的详细信息。

#### 私有成员变量
- `bool blnInit`：标识是否已经初始化。
- `int vid, pid`：存储初始化时设置的vendor ID和product ID。
- `int index_device`：用于管理设备索引。

#### 设备通信与数据处理
- `bool SendRandom(DataRandom* send, int nTimes, int index = 0)`：发送随机地址的数据包，并等待响应。
- `bool SendContinuous(DataContinuous* send, int nTimes, int index = 0)`：发送连续地址的数据包，并等待响应。
- `void AnalyseRandomData(DataRandom recv, size_t index)`：分析接收到的随机地址数据包。
- `void AnalyseContinuousData(DataContinuous recv, size_t index)`：分析接收到的连续地址数据包。
- `void AnalyseRawData(unsigned char* buff, int data_size, const char* path)`：分析接收到的原始数据。
- `void HandleConfirmMessage(unsigned char* buff, int data_size, size_t index)`：处理确认消息。
- `void HandleRandomMessage(unsigned char* buff, int data_size, size_t index)`：处理随机地址消息。
- `void HandleContinuousMessage(unsigned char* buff, int data_size, size_t index)`：处理连续地址消息。

#### 设计理念
该类设计为高层的设备管理器，用于管理和协调多个HID设备的状态和通信。通过封装低级的HID API调用，`CHID_DeviceManager` 提供了一个更加友好和易于使用的接口来管理HID设备。

#### 注意事项
- 该类的实例应确保在多线程环境下的线程安全，特别是在进行设备连接和断开操作时。
- 数据发送和接收函数的实现需要确保在设备通信频繁的环境下的性能和响应性。
