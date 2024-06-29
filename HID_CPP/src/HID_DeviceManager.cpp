#include "HID_DeviceManager.h"

CHID_DeviceManager::CHID_DeviceManager() : pid(0), vid(0), index_device(0)
{

}

CHID_DeviceManager::~CHID_DeviceManager()
{
	this->blnRunning = false;
	this->blnWriteThreadRunning = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

bool CHID_DeviceManager::Init(int vid, int pid)
{
	this->vid = vid;
	this->pid = pid;
	if (hid_init() == 0) this->blnInit = true;



	if (blnInit)
		return this->AutoConnectDevice();
	else
		return false;
}

bool CHID_DeviceManager::AutoConnectDevice()
{

	//枚举所有vid 和pid的对象
	hid_device_info* devs;
	devs = hid_enumerate(vid, pid);

	while (devs != NULL) {
		//检查map下的对象是否同路径，如果存在同路径则跳过，反之则注册
		bool blnFound = false;
		for (auto item : this->map_device) {
			if (strcmp(item.second->GetDevicePath(), devs->path) == 0) {
				blnFound = true;
				break;
			}
		}

		if (blnFound) {
			continue;
		}

		ICommunicateDevice* pDevice = new CHID_Device();
		if (pDevice->OpenDevice(devs->path, devs->vendor_id, devs->product_id, nullptr)) {
			this->map_device[index_device++] = pDevice;
			//连接一下数据处理的函数 
			pDevice->registerReadCallback([this, pDevice](unsigned char* buff, int data_size) {
				this->AnalyseRawData(buff, data_size, pDevice->GetDevicePath());
				});
		}
		else {
			delete pDevice;
			pDevice = nullptr;
		}
		devs = devs->next;
	}
	hid_free_enumeration(devs);

	bool blnret = this->map_device.size() > 0 ? true : false;
	if (blnret) {
		this->blnRunning = true;
		loopThread = std::thread(&CHID_DeviceManager::LoopCheckHidDevice, this);
	}
	return blnret;
}

int CHID_DeviceManager::IsAvaliableDevice(const char* path)
{
	for (auto item : this->map_device) {
		if (!strcmp(path, item.second->GetDevicePath())) {
			return item.first;
		}
	}
	return -1;
}

int CHID_DeviceManager::GetDeviceCount()
{
	return this->map_device.size();
}

bool CHID_DeviceManager::IsDeviceConnect(int index)
{
	if (!this->map_device.count(index))
		return false;

	return this->map_device[index]->IsDeviceOpen();
}

bool CHID_DeviceManager::PathConnectDevice(char* path)
{
	//尝试使用路径打开一个设备
	ICommunicateDevice* pDevice = new CHID_Device();
	if (pDevice->OpenDevice(path, vid, pid, nullptr)) {
		this->map_device[index_device++] = pDevice;
	}
	else {
		delete pDevice;
		pDevice = nullptr;
	}
	return pDevice != nullptr;
}

void CHID_DeviceManager::DisconnectDevice(int index)
{
	if (!this->map_device.count(index)) return;
	ICommunicateDevice* device = this->map_device[index];
	device->CloseDevice();
	device->unregisterReadCallback();
	delete device;
	device = nullptr;
	this->map_device.erase(index);
}

std::vector<std::shared_ptr<DeviceInfo>> CHID_DeviceManager::GetDevicesInfo() {
	if (this->map_device.size() == 0) return {};

	this->vec_device_info.clear();
	hid_device_info* devs = hid_enumerate(vid, pid);
	hid_device_info* current = devs;  // 用来遍历设备链表

	while (current) {
		std::shared_ptr<DeviceInfo> info = std::make_shared<DeviceInfo>();

		// 安全地拷贝字符串，确保不会溢出
		info->path = std::string(current->path);
		info->vendor_id = current->vendor_id;
		info->product_id = current->product_id;
		// 安全地拷贝宽字符串
		info->serial_number = std::wstring(current->serial_number);
		info->release_number = current->release_number;
		info->manufacturer_string = std::wstring(current->manufacturer_string);
		info->product_string = std::wstring(current->product_string);
		info->usage_page = current->usage_page;
		info->usage = current->usage;
		info->interface_number = current->interface_number;

		this->vec_device_info.push_back(info);

		current = current->next;  // 移动到下一个元素
	}

	hid_free_enumeration(devs);  // 释放由 hid_enumerate 返回的链表

	return this->vec_device_info;
}

std::shared_ptr<DeviceInfo> CHID_DeviceManager::GetDeviceInfo(int index)
{
	//先找path信息
	if (!this->map_device.count(index)) return nullptr;
	//找到path信息
	const char* path = this->map_device[index]->GetDevicePath();
	//尝试在vec中查找
	for (auto item : this->vec_device_info) {
		if (!item->path.compare(path)) {
			return item;
		}
	}
	return nullptr;
}


bool CHID_DeviceManager::SendRandom(DataRandom* send, int nTimes, int index)
{

	this->StartWriteThread();
	try {
		if (!send) {
			throw std::exception("send is null");
		}
		std::list<uint8_t> list_funcAddrCode;

		//获得地址列表
		//1.计算地址总长
		unsigned char* data = new unsigned char[64];
		size_t size = 0;
		send->toData(data, size);
		Sizet_DataCombine sizet_combine = { size,data };

		//2. 计算地址长度，获取地址信息，用于后续判别
		int size_addr = (static_cast<uint16_t>(send->frame_len_H) << 8) | send->frame_len_L;
		size_addr = (size_addr - 7) / 2;
		for (int i = 0; i < size_addr; ++i) {
			list_funcAddrCode.push_back(send->param_addr[i]);
		}
		if (list_funcAddrCode.size() == 0) {
			throw std::exception("list_funcAddrCode is empty");
		}

		//3.组装数据
		AddrCode_DataCombine addr_combine = { list_funcAddrCode,sizet_combine };
		Index_DataCombine index_combine = { index,addr_combine };
		this->list_datas.push_back(index_combine);
	}
	catch (const std::exception& err) {
		std::cout << __FUNCTION__ << err.what();
		return false;
	}
	return true;
}

bool CHID_DeviceManager::SendReadRandom(ReadRandomDataPack* send, int nTimes, int index)
{
	this->StartWriteThread();
	try {
		if (!send) {
			throw std::exception("send is null");
		}
		std::list<uint8_t> list_funcAddrCode;

		//获得地址列表
		//1.计算地址总长
		unsigned char* data = new unsigned char[64];
		size_t size = 0;
		send->toData(data, size);
		Sizet_DataCombine sizet_combine = { size,data };

		//2. 计算地址长度，获取地址信息，用于后续判别
		int size_addr = (static_cast<uint16_t>(send->frame_len_H) << 8) | send->frame_len_L;
		size_addr = (size_addr - 7);
		for (int i = 0; i < size_addr; ++i) {
			list_funcAddrCode.push_back(send->param_addr[i]);
		}
		if (list_funcAddrCode.size() == 0) {
			throw std::exception("list_funcAddrCode is empty");
		}

		//3.组装数据
		AddrCode_DataCombine addr_combine = { list_funcAddrCode,sizet_combine };
		Index_DataCombine index_combine = { index,addr_combine };
		this->list_datas.push_back(index_combine);
	}
	catch (const std::exception& err) {
		std::cout << __FUNCTION__ << err.what();
		return false;
	}
	return true;
}

bool CHID_DeviceManager::SendContinuous(DataContinuous* send, int nTimes, int index)
{
	this->StartWriteThread();
	try {
		if (!send) {
			throw std::exception("send is null");
		}
		std::list<uint8_t> list_funcAddrCode;

		//获得地址列表
		//1.计算地址总长
		unsigned char* data = new unsigned char[64];
		size_t size = 0;
		send->toData(data, size);
		Sizet_DataCombine sizet_combine = { size,data };

		//2. 对于连续读写，只记录前面四个值
		list_funcAddrCode.push_back(send->param_addr[0]);
		list_funcAddrCode.push_back(send->param_addr[1]);
		list_funcAddrCode.push_back(send->param_num[0]);
		list_funcAddrCode.push_back(send->param_num[1]);

		//3.组装数据
		AddrCode_DataCombine addr_combine = { list_funcAddrCode,sizet_combine };
		Index_DataCombine index_combine = { index,addr_combine };
		this->list_datas.push_back(index_combine);
	}
	catch (const std::exception& err) {
		std::cout << __FUNCTION__ << err.what();
		return false;
	}
	return true;
}


void CHID_DeviceManager::HandleConfirmMessage(unsigned char* buff, size_t data_size, int index)
{
	try {
		//应答消息需要处理看是否是正确的应答消息
		//1. 解析数据包
		ReplyFrame data;
		data.FromData(buff, data_size);

		//2.解析结果信息
		if (data.ERR_CODE_L == 0x00) {
			//将其插入到map_blnPass的信息中去
			this->map_blnPass[index] = true;
			this->map_blnPass.erase(index);
		}
		else {
			//非正常应答，则直接抛出
			switch (data.ERR_CODE_L) {
			case 0x01: {
				throw std::exception("device path error,not suit current device path");
				break;
			}
			case 0x02: {
				throw std::exception("length error");
				break;
			}
			case 0x03: {
				throw std::exception("CRC error");
				break;
			}
			case 0x04: {
				throw std::exception("only read param");
				break;
			}
			case 0x05: {
				throw std::exception("param out of range");
				break;
			}
			}
		}
	}
	catch (const std::exception& err) {
		std::cout << err.what();
		this->HandleDeviceWriteMessageError(index, err.what(), WriteErrorType::reply_error);
	}



}

void CHID_DeviceManager::HandleContinuousMessage(unsigned char* buff, size_t data_size, int index)
{
	try {
		DataContinuous data;
		if (!ParseDataContinuous(buff, data_size, data)) {
			throw std::exception("parse data continuous error");
		}
		//这里我只需要读前四个值就可以了
		std::list<uint8_t> list_wait_addr;
		list_wait_addr.push_back(data.param_addr[0]);
		list_wait_addr.push_back(data.param_addr[1]);
		list_wait_addr.push_back(data.param_num[0]);
		list_wait_addr.push_back(data.param_num[1]);

		if (list_wait_addr == this->list_wait_addr) {
			this->map_blnPass[index] = true;
			this->map_blnPass.erase(index);
		}

		this->AnalyseContinuousData(data, index);
	}
	catch (const std::exception& err) {

	}
	
}

void CHID_DeviceManager::printDeviceInfo(const STDeviceInfo& dev)
{
	std::wcout << L"Path: " << dev.path.c_str()
		<< L", Serial: " << (dev.serial_number.empty() ? L"" : dev.serial_number.c_str()) << std::endl;
}

void CHID_DeviceManager::LoopCheckHidDevice()
{
	//线程内部自己维护这个previousDevices
	std::unordered_set<STDeviceInfo, STDeviceInfoHash> previousDevices;

	while (this->blnRunning) {
		hid_device_info* devices = hid_enumerate(this->vid, this->pid);
		std::unordered_set<STDeviceInfo, STDeviceInfoHash> currentDevices;

		for (hid_device_info* dev = devices; dev != nullptr; dev = dev->next) {
			currentDevices.insert(STDeviceInfo(dev->path, dev->serial_number));
		}

		// Find newly added devices
		for (const auto& dev : currentDevices) {
			if (previousDevices.find(dev) == previousDevices.end()) {
				std::wcout << L"New Device: ";
				this->HandleDevicePlugin(dev, true);
			}
		}

		// Find removed devices
		for (const auto& dev : previousDevices) {
			if (currentDevices.find(dev) == currentDevices.end()) {
				std::wcout << L"Removed Device: ";
				this->HandleDevicePlugin(dev, false);
			}
		}

		// Update the previous devices set
		previousDevices = std::move(currentDevices);

		// Free the device list
		hid_free_enumeration(devices);

		// Wait for 500ms
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void CHID_DeviceManager::HandleDevicePlugin(const STDeviceInfo device_info, bool blnPlugin)
{
	printDeviceInfo(device_info);
}

void CHID_DeviceManager::HandleDeviceWriteMessageError(int index, const std::string& strMessage, WriteErrorType type)
{
	std::wcout << L"Write Error: " << strMessage.c_str() << std::endl;
}

void CHID_DeviceManager::StartWriteThread()
{
	if (this->blnWriteThreadRunning) return;
	this->blnWriteThreadRunning = true;
	this->writeThread = std::thread(&CHID_DeviceManager::LoopWriteData, this);

}
//见文档说明本函数
//https://gitee.com/maonogit/hid_-cpp_-sigmabus/blob/master/HID_CPP/Document/HID_DeviceManager%E4%B8%AD%E5%A6%82%E4%BD%95%E8%BF%90%E8%A1%8C%E7%9A%84%E7%BA%BF%E7%A8%8B%E5%86%99%E5%85%A5%E6%B5%81%E7%A8%8B.md
void CHID_DeviceManager::LoopWriteData()
{
	while (this->blnWriteThreadRunning) {

		try {
			//std::lock_guard<std::mutex> lock(mtx);
			if (this->list_datas.size() == 0) {
				//throw std::exception("list_empty");
				continue;
			}
			//每次重新开始的话，都需要重置标志位
			this->map_blnPass.clear();
			std::map<int, ICommunicateDevice*> map_targets;
			//std::list<ICommunicateDevice*> list_targets;
			std::list<std::pair<int, ICommunicateDevice*>> list_resend_targets;
			Index_DataCombine first_dataPack = this->list_datas.front();
			int index = first_dataPack.first;

			//找到指定设备
			if (index != 0) {
				if (!this->map_device.count(index))
					throw std::exception("map_device list_empty");
				map_targets.insert(*this->map_device.find(index));
			}
			else {
				for (auto item : this->map_device) {
					map_targets.insert(item);
				}
			}
			//尝试发送数据，获取地址列表
			AddrCode_DataCombine addr_combine = first_dataPack.second;
			//等待应答的地址列表
			this->list_wait_addr = addr_combine.first;
			Sizet_DataCombine sizet_combine = addr_combine.second;
			//发送数据
			for (auto target : map_targets) {
				//第一次写入，需要将标识符只为false
				this->map_blnPass.insert(std::pair<int, bool>(target.first, false));
				bool ret = target.second->Write(sizet_combine.second, sizet_combine.first);
				if (!ret)
					list_resend_targets.push_front(target);
				//如果有发送失败的设备，则尝试重新发送 
				int resent_times = 0;

				//等待重发函数
				while (list_resend_targets.size() > 0) {
					try {
						if (resent_times >= 3) {
							throw std::exception("resent_time_out");
						}
						bool blnRet = list_resend_targets.front().second->Write(sizet_combine.second, sizet_combine.first);
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
						if (blnRet) {
							list_resend_targets.pop_front();
							resent_times = 0;
							//写入成功的对象
						}
						else {
							resent_times++;
						}
					}
					catch (std::exception& error) {
						//找到index
						int index_err = -1;
						for (auto item : this->map_device) {
							if (item.second == list_resend_targets.front().second) {
								index_err = item.first;
							}
						}
						//去掉这个list_resend_targets.front以及resent_times
						list_resend_targets.pop_front();
						resent_times = 0;
						HandleDeviceWriteMessageError(index_err, error.what(), WriteErrorType::timeout);
					}
				}
				//等待回执信息
				const int maxRetries = 3;
				const int waitTimeMs = 500;
				bool blnCheckPass = false;
				//直接操作其实是线程不安全的，所以这边考虑是loop三遍进行检查返回值是否成功返回了，每次都拷贝这个map_blnPass
				for (int i = 0; i < maxRetries; ++i) {
					std::map<int, bool> map_blnPass_t = this->map_blnPass;
					for (auto item : map_blnPass_t) {
						if (item.second) {
							//如果判别成功了，则删去这个map_blnPass_t的对象
							map_blnPass_t.erase(item.first);
						}
					}
					if (map_blnPass_t.size() == 0) {
						blnCheckPass = true;
						break;
					}
					//等待
					std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));
				}
				std::map<int, bool> map_blnPass_t = this->map_blnPass;
				for (auto item : map_blnPass_t) {
					if (!blnCheckPass) this->HandleDeviceWriteMessageError(item.first, "reply_timeout", WriteErrorType::reply_timeout);
				}
				this->list_datas.pop_front();			
			}
		}
		catch (std::exception& error) {
			std::cout << error.what()<<std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void CHID_DeviceManager::HandleRandomMessage(unsigned char* buff, size_t data_size, int index)
{
	//此函数读取应答读取请求
	try {
		DataRandom data;
		if (!ParseDataRandom(buff, data_size, data)) {
			throw std::exception("can't parse DataRandom");
		}

		int size_arr = (data_size - 7) / 2;

		std::list<uint8_t> list_temp_addrs;
		for (int index = 0; index < size_arr; index++) {
			list_temp_addrs.push_back(data.param_addr[index]);
		}

		if (list_wait_addr == list_temp_addrs) {
			map_blnPass[index] = true;
			this->map_blnPass.erase(index);
		}
		
		this->AnalyseRandomData(data, index);
	}
	catch (const std::exception& err) {
		std::cout << err.what();
		this->HandleDeviceWriteMessageError(index, err.what(), WriteErrorType::reply_error);
	}
	
}

bool CHID_DeviceManager::SendReadContinuous(ReadDataContinuous* send, int nTimes, int index)
{
	this->StartWriteThread();
	try {
		if (!send) {
			throw std::exception("send is null");
		}
		std::list<uint8_t> list_funcAddrCode;

		//获得地址列表
		//1.计算地址总长
		unsigned char* data = new unsigned char[64];
		size_t size = 0;
		send->toData(data, size);
		Sizet_DataCombine sizet_combine = { size,data };

		//2. 对于连续读写，只记录前面四个值
		list_funcAddrCode.push_back(send->param_addr[0]);
		list_funcAddrCode.push_back(send->param_addr[1]);
		list_funcAddrCode.push_back(send->param_num[0]);
		list_funcAddrCode.push_back(send->param_num[1]);

		//3.组装数据
		AddrCode_DataCombine addr_combine = { list_funcAddrCode,sizet_combine };
		Index_DataCombine index_combine = { index,addr_combine };
		this->list_datas.push_back(index_combine);
	}
	catch (const std::exception& err) {
		std::cout << __FUNCTION__ << err.what();
		return false;
	}
	return true;
}

void CHID_DeviceManager::AnalyseRawData(unsigned char* buff, int data_size, const char* path)
{
	try {
		//std::lock_guard<std::mutex> lock(mtx);
		if (buff == nullptr) {
			throw std::exception("buff is null");
		}
		//if(data_size < )
		//到此为止，这里就获得了单个 usb设备的数据，在这里开始就需要进行解析了
		//首先找到这是谁
		int index_ = -1;
		for (auto item : this->map_device) {
			if (!strcmp(path, item.second->GetDevicePath())) {
				index_ = item.first;
				break;
			}
		}
		if (index_ == -1) throw std::exception("can't find the device!");

		//检查头字节是不是0xc4，如果不是，则检查第二个字节是不是0xc4字节，如果不是，则不处理
		if (buff[0] != 0xc4 && buff[1] != 0xc4) { throw std::exception("Message does not conform to protocol specifications."); };
		size_t index = 0;
		uint8_t report;
		//如果头字节是0xc4，则代表没有reportid，需要补一个reportid，如果头字节不是0xc4但是第二个字节是0xc4，则代表有reportid
		bool blnWithReportId = false;
		if (buff[0] == 0xc4) {
			report = 0x4b;
		}
		else {
			report = buff[index++];
			blnWithReportId = true;
		}
		uint8_t header = buff[index++];
		uint8_t length_l = buff[index++];
		uint8_t length_h = buff[index++];
		//计算一下长度
		int length = length_h << 8 | length_l;
		//if (length != data_size) return;

		uint8_t addr = buff[index++];
		uint8_t cmd = buff[index++];
		size_t ret_size;
		//if (blnWithReportId) {
		//	ret_size = data_size - 1;
		//}
		//else {
		//	ret_size = data_size;
		//}
		switch (cmd) {
		case (int)ReplyType::Confirm: {
			//如果是标准应答
			this->HandleConfirmMessage(buff, data_size, index_);
			break;
		}
		case (int)ReplyType::Random: {
			this->HandleRandomMessage(buff, data_size, index_);
			break;
		}
		case (int)ReplyType::Continuous: {
			this->HandleContinuousMessage(buff, data_size, index_);
			break;
		}
		}

	}
	catch (const std::exception& error) {
		std::cout << error.what();
	}
}


