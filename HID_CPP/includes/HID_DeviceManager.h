#pragma once
#include "IHID.h"
#include "HID_Device.h"
#include "hidapi.h"
#include "HID_Global.h"
#include <thread>
#include <set>
#include <string>
#include <unordered_set>
#include <mutex>

enum class WriteErrorType {
	timeout,
	reply_timeout,
	reply_error,
	reply_wrong
};
class HID_API CHID_DeviceManager : public ICommDeviceMgmt {

public:
	CHID_DeviceManager();
	~CHID_DeviceManager();

	bool Init(int vid, int pid);
	bool AutoConnectDevice();
	int IsAvaliableDevice(const char* path);
	int GetDeviceCount();
	bool IsDeviceConnect(int index);
	bool PathConnectDevice(char* path);
	void DisconnectDevice(int index = 0);
	std::vector<std::shared_ptr<DeviceInfo>> GetDevicesInfo();
	std::shared_ptr<DeviceInfo> GetDeviceInfo(int index);

	/// <summary>
	/// 随机地址写入请求
	/// </summary>
	/// <param name="send">发送写入请求</param>
	/// <param name="nTimes">发送失败重试次数</param>
	/// <param name="index">设备index</param>
	/// <returns>返回发送成功与否</returns>
	bool SendRandom(DataRandom* send, int nTimes, int index = 0);

	/// <summary>
	/// 随机地址读取请求
	/// </summary>
	/// <param name="send">发送读取请求</param>
	/// <param name="nTimes">发送失败重试次数</param>
	/// <param name="index">设备index</param>
	/// <returns>返回发送成功与否</returns>
	bool SendReadRandom(ReadRandomDataPack* send, int nTimes, int index = 0);

	/// <summary>
	/// 连续地址写入请求
	/// </summary>
	/// <param name="send">发送写入请求</param>
	/// <param name="nTimes">发送失败重试次数</param>
	/// <param name="index">设备index</param>
	/// <returns>返回发送成功与否</returns>
	bool SendContinuous(DataContinuous* send, int nTimes, int index = 0);

	/// <summary>
	/// 连续地址读取请求 
	/// </summary>
	/// <param name="send">发送写入请求</param>
	/// <param name="nTimes">发送失败重试次数</param>
	/// <param name="index">设备index</param>
	/// <returns>返回发送成功与否</returns>
	bool SendReadContinuous(ReadDataContinuous* send, int nTimes, int index = 0);

private:
	bool blnInit = false;
	int vid, pid;
	int index_device;
private:

	//这里写一个监控，控制当前管理者所对应所有设备的拔插消息
	//之前本来是想改装hidapi的，但是那个窗口消息返回始终是做的，而且hidapi限制了c99协议，修改太困难了，所以放弃了。
	//这个方案也更跨平台，所以决定使用这个方案了

	struct STDeviceInfo {
		std::string path;
		std::wstring serial_number;

		STDeviceInfo(const char* p, const wchar_t* sn)
			: path(p ? p : ""), serial_number(sn ? sn : L"") {}

		bool operator==(const STDeviceInfo& other) const {
			return path == other.path && serial_number == other.serial_number;
		}
	};

	struct STDeviceInfoHash {
		size_t operator()(const STDeviceInfo& dev) const {
			std::hash<std::string> hashString;
			std::hash<std::wstring> hashWString;
			return hashString(dev.path) ^ hashWString(dev.serial_number);
		}
	};

	void printDeviceInfo(const STDeviceInfo& dev);

	hid_device_info* devices;

	//线程
	std::thread loopThread;
	/// <summary>
	/// 线程启动标志
	/// </summary>
	bool blnRunning = false;


	void LoopCheckHidDevice();
	
protected:


	////继承的类需要维护这两个函数
	virtual void AnalyseRandomData(const DataRandom& recv, int index) = 0;
	virtual void AnalyseContinuousData(const DataContinuous& recv, int index) = 0;

	/// <summary>
	/// 处理拔插事件函数，可以自行安排回调或者别的
	/// </summary>
	/// <param name="device_info"></param>
	/// <param name="blnPlugin"></param>
	void HandleDevicePlugin(const STDeviceInfo device_info, bool blnPlugin);

	virtual void HandleDeviceWriteMessageError(int index,const std::string& strMessage, WriteErrorType type);
	
private:
	//保存了数据的根本结构
	using Sizet_DataCombine = std::pair<size_t, unsigned char*>;
	//添加了一个地址列表，用于判断是否得到答复的结构
	using AddrCode_DataCombine = std::pair<std::list<uint8_t>, Sizet_DataCombine>;
	//保存对于输入设备index以及数据之间的关系列表
	using Index_DataCombine = std::pair<int, AddrCode_DataCombine>;
	// 第一层,一个输入list
	using DataList = std::list<Index_DataCombine>;

	std::mutex mtx;  // 互斥锁
	std::thread writeThread;
	bool blnWriteThreadRunning = false;

	std::list<uint8_t> list_wait_addr;
	//key:index bool:是否成功获得返回值
	std::map<int, bool> map_blnPass;

	DataList list_datas;  // 输入队列



	void StartWriteThread();
	/// <summary>
	/// 循环写入数据线程函数
	/// </summary>
	/// <param name="data"></param>
	/// <param name="data_size"></param>
	/// <param name="index"></param>
	void LoopWriteData();

	void AnalyseRawData(unsigned char* buff, int data_size, const char* path);
	void HandleConfirmMessage(unsigned char* buff, size_t data_size, int index);
	void HandleRandomMessage(unsigned char* buff, size_t data_size, int index);
	void HandleContinuousMessage(unsigned char* buff, size_t data_size, int index);
};