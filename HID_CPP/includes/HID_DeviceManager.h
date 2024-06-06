#pragma once
#include "IHID.h"
#include "HID_Device.h"
#include "hidapi.h"
#include "HID_Global.h"


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
private:
	bool blnInit = false;
	int vid, pid;
	int index_device;

protected:


	bool SendRandom(DataRandom* send, int nTimes, int index = 0);
	bool SendContinuous(DataContinuous* send, int nTimes, int index = 0);

	////继承的类需要维护这两个函数
	virtual void AnalyseRandomData(const DataRandom& recv, int index) = 0 ;
	virtual void AnalyseContinuousData(const DataContinuous& recv, int index) = 0;

	void AnalyseRawData(unsigned char* buff, int data_size, const char* path);

	void HandleConfirmMessage(unsigned char* buff, size_t data_size, int index);
	void HandleRandomMessage(unsigned char* buff, size_t data_size, int index);
	void HandleContinuousMessage(unsigned char* buff, size_t data_size, int index);
};