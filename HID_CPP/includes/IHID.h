#pragma once
#if !defined(IHID)
#define IHID

#include <iostream>
#include <vector>
#include <map>
#include "HID_Params.h"
#include <CRC.h>
#include "hidapi.h"
#include <memory>
#include <functional>



/**
 * 通讯设备接口，满足其与一个单独设备通信的最基本要求
 */
class ICommunicateDevice
{
public:
	virtual ~ICommunicateDevice() {
	
	}
	/**
	 @brief 初始化
	 @return  bool_t 成功返回TRUE，否则返回FALSE
	 @note
	 @author toyouko
	*/
	virtual bool Init() = 0;
	/**
	 @brief 获取设备的参数1，如设备vid
	 @note
	 @author toyouko
	*/
	virtual uint32_t GetDeviceParam1() const = 0;
	/**
	 @brief 获取设备的参数2，如设备pid
	 @note
	 @author toyouko
	*/
	virtual uint32_t GetDeviceParam2() const = 0;
	/**
	 @brief 判断是否已经打开设备
	 @return  bool_t 已经打开设备返回TRUE，否则返回FALSE
	 @note
	 @author toyouko
	*/
	virtual bool IsDeviceOpen() = 0;
	/**
	 @brief 获取设备名称
	 @return  const char* 未打开设备返回NULL
	 @note
	 @author toyouko
	*/
	virtual const char* GetDeviceName() const = 0;
	/**
	 @brief 打开设备
	 @param[in] dev_path:设备的路径
	 @param[out] param1  参数1，如设备vid
	 @param[out] param2  参数2，如设备pid
	 @param[in]  param3  打开设备可能需要的参数，没有参数可以为NULL
	 @return  bool_t 失败返回FALSE,发送成功返回TRUE
	 @note
	 @author toyouko
	*/
	virtual bool OpenDevice(const char* dev_path, uint32_t param1, uint32_t param2, void* param3) = 0;
	/**
	 @brief 关闭设备
	 @return  void
	 @sa
	 @retval
	 @note
	 @author toyouko
	*/
	virtual void CloseDevice() = 0;
	/**
	 @brief 将数据发送到已打开的设备
	 @param[in] buff:要写入设备的数据
	 @param[in] nBuffBytes:buff中数据的字节数, 要与GetWriteMaxLength()返回的字节数相等
	 @return  bool_t 失败返回FALSE,但返回TRUE不表示发送成功
	 @note 此函数是异步发送，不会等待返回结果
	 @author toyouko
	*/
	virtual bool Write(unsigned char* buff, size_t nBuffBytes) = 0;
	/// <summary>
	/// 从已打开的设备接收数据
	/// </summary>
	/// <param name="buff">[out]接收数据的缓存，不能小于GetReadMaxLength()返回的字节数</param>
	/// <param name="data_size">[out]接收到数据的字节数量</param>
	/// <param name="waitTime">[in]读取超时时间</param>
	/// <returns></returns>
	virtual bool Read(unsigned char* buff, size_t& data_size, int waitTime) = 0;
	/**
	 @brief 获取设备报文数据的长度
	 @return  int 返回可读数据的字节数
	 @note 调用此函数之前，需要先打开设备
	 @author toyouko
	*/
	virtual int GetMaxReportLength() = 0;
	/**
	 @brief 获取设备路径
	 @return  const char*
	 @note
	 @author toyouko
	*/
	virtual const char* GetDevicePath() const = 0;

	//读取回调函数
	std::function<void(unsigned char* buff, int data_size)> ReadCallback = nullptr;
	//注册读取回调函数
	virtual void registerReadCallback(const std::function<void(unsigned char* buff, int data_size)>& callback) {
		this->ReadCallback = callback;
	}
	virtual void unregisterReadCallback() {
		ReadCallback = nullptr;
	}
protected:
	ICommunicateDevice() {}
};

/// <summary>
/// 更好用的数据结构，因为从hidapi获取的DeviceInfo是一个结构体，我不好发给外部
/// </summary>
struct DeviceInfo {
	/** Platform-specific device path */
	std::string path;
	/** Device Vendor ID */
	unsigned short vendor_id;
	/** Device Product ID */
	unsigned short product_id;
	/** Serial Number */
	std::wstring serial_number;
	//wchar_t* serial_number;
	/** Device Release Number in binary-coded decimal,
		also known as Device Version Number */
	unsigned short release_number;
	/** Manufacturer String */
	std::wstring manufacturer_string;  // 使用 std::wstring
	std::wstring product_string;  // 使用 std::wstring
	/** Usage Page for this Device/Interface
		(Windows/Mac/hidraw only) */
	unsigned short usage_page;
	/** Usage for this Device/Interface
		(Windows/Mac/hidraw only) */
	unsigned short usage;
	/** The USB interface which this logical device
		represents.
		* Valid on both Linux implementations in all cases.
		* Valid on the Windows implementation only if the device
		  contains more than one interface.
		* Valid on the Mac implementation if and only if the device
		  is a USB HID device. */
	int interface_number;

};

//设备管理类，管理某一类设备，根据vid和pid区别设备种类
class ICommDeviceMgmt
{

public:
	virtual ~ICommDeviceMgmt() {
		vec_device_info.clear();
		for (auto item : this->map_device) {
			delete item.second;
			item.second = nullptr;
		}
	}

	/**
	 @brief 初始化
	 @return  bool_t 成功返回TRUE, 失败返回FALSE
	 @note
	 @author toyouko
	*/
	virtual bool Init(int vid, int pid) = 0;

	/// <summary>
	/// 检查是否是当前可用的设备 
	/// </summary>
	/// <param name="path">设备路径</param>
	/// <returns>对应的index，如果不存在，则返回-1</returns>
	virtual int IsAvaliableDevice(const char* path) = 0;
	/**
	@brief 自动枚举和连接现有的设备
	@return  bool_t  连接成功返回TRUE, 失败返回FALSE
	@note
	@author toyouko
	*/
	virtual bool AutoConnectDevice() = 0;

	/// <summary>
	/// 获得设备的数量
	/// </summary>
	/// <returns>同种设备数量</returns>
	virtual int GetDeviceCount() = 0;

	/// <summary>
	/// 判断指定设备是否连接
	/// </summary>
	/// <param name="index">指定设备id</param>
	/// <returns>是否连接</returns>
	virtual bool IsDeviceConnect(int index) = 0;

	/**
	 @brief 根据地址连接
	 @return  bool_t  连接成功返回TRUE, 失败返回FALSE
	 @note
	 @author toyouko
	 */

	virtual bool PathConnectDevice(char* path) = 0;

	/// <summary>
	/// 断开设备连接，index=0则默认断开所有连接的设备
	/// </summary>
	/// <param name="index"></param>
	virtual void DisconnectDevice(int index = 0) = 0;



	/// <summary>
	/// 获得设备插入的信息，如果index=0则直接返回所有设备列表
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	virtual std::vector<std::shared_ptr<DeviceInfo>> GetDevicesInfo() = 0;

	/// <summary>
	/// 获得指定设备的插入信息
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	virtual std::shared_ptr<DeviceInfo> GetDeviceInfo(int index) = 0;
	/**
	 @brief 发送一个随机地址的数据包，并接收应答数据。不支持多线程调用。
	 @param[in] send -- 待发送的数据包
	 @param[in] recv -- 接收到的应答数据
	 @param[in] nTimes -- 发送失败时的重复次数
	 @return  bool_t  发送并得到返回成功则返回true,否则返回 false
	 @note 发送数据包后，要等待接收到应答数据或者等待超时，函数才会返回
	 @author toyouko
	 */
	virtual bool SendRandom(DataRandom* send, int nTimes, int index = 0) = 0;

	/// <summary>
	/// 发送连续地址读写的数据包并接受应答数据
	/// </summary>
	/// <param name="send">待发送的数据包</param>
	/// <param name="recv">接收到的应答数据</param>
	/// <param name="nTimes">发送失败时的重复次数</param>
	/// <param name="dwWaitTime">每一次发送数据包，等待多久超时</param>
	/// <returns>发送并得到返回成功则返回true,否则返回 false</returns>
	virtual bool SendContinuous(DataContinuous* send, int nTimes, int index = 0) = 0;

	/// <summary>
	/// 子类需要所有非应答随机地址数据包的处理函数，需要子类继承来实现
	/// </summary>
	/// <param name="recv"></param>
	virtual void AnalyseRandomData(const DataRandom& recv,int index) = 0;


	/// <summary>
	/// 子类需要所有非应答连续地址数据包的处理函数，需要子类继承来实现
	/// </summary>
	/// <param name="recv"></param>
	virtual void AnalyseContinuousData(const DataContinuous& recv, int index) = 0;


protected:

	ICommDeviceMgmt() {}
	std::vector<std::shared_ptr<DeviceInfo>> vec_device_info;
	//设备管理对象，index和对象指针一一对应
	std::map<int, ICommunicateDevice*> map_device;
};




#endif // !defined(HID_BASE)
