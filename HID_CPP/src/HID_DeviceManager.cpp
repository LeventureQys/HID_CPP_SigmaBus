#include "HID_DeviceManager.h"

CHID_DeviceManager::CHID_DeviceManager() : pid(0), vid(0), index_device(0)
{

}

CHID_DeviceManager::~CHID_DeviceManager()
{

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

	//ö������vid ��pid�Ķ���
	hid_device_info* devs;
	devs = hid_enumerate(vid, pid);
	
	while (devs != NULL) {
		//���map�µĶ����Ƿ�ͬ·�����������ͬ·������������֮��ע��
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
			//����һ�����ݴ���ĺ��� 
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
	return this->map_device.size() > 0 ? true : false;
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
	//����ʹ��·����һ���豸
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
	hid_device_info* current = devs;  // ���������豸����

	while (current) {
		std::shared_ptr<DeviceInfo> info = std::make_shared<DeviceInfo>();

		// ��ȫ�ؿ����ַ�����ȷ���������
		info->path = std::string(current->path);
		info->vendor_id = current->vendor_id;
		info->product_id = current->product_id;
		// ��ȫ�ؿ������ַ���
		info->serial_number = std::wstring(current->serial_number);
		info->release_number = current->release_number;
		info->manufacturer_string = std::wstring(current->manufacturer_string);
		info->product_string = std::wstring(current->product_string);
		info->usage_page = current->usage_page;
		info->usage = current->usage;
		info->interface_number = current->interface_number;

		this->vec_device_info.push_back(info);

		current = current->next;  // �ƶ�����һ��Ԫ��
	}

	hid_free_enumeration(devs);  // �ͷ��� hid_enumerate ���ص�����

	return this->vec_device_info;
}

std::shared_ptr<DeviceInfo> CHID_DeviceManager::GetDeviceInfo(int index)
{
	//����path��Ϣ
	if (!this->map_device.count(index)) return nullptr;
	//�ҵ�path��Ϣ
	const char* path = this->map_device[index]->GetDevicePath();
	//������vec�в���
	for (auto item : this->vec_device_info) {
		if (!item->path.compare(path)) {
			return item;
		}
	}
	return nullptr;
}


bool CHID_DeviceManager::SendRandom(DataRandom* send, int nTimes, int index)
{
	if (!send) return false;
	//�������ָ��ͳ���
	//TODO:�����data size��һ����64��Ŀǰ��64����ʵ���Ͽ��Ը����ֵ
	unsigned char* data = new unsigned char[64];
	size_t size = 0;
	send->toData(data, size);
	int retry = 0;
	//�Ƿ��������е��豸
	if (index == 0) {
		for (auto item : this->map_device) {
			ICommunicateDevice* device = item.second;
			while (!device->Write(data, size) && retry++ < nTimes) {
				device->Write(data, size);
			}
		}
	}
	else {
		if (!this->map_device.count(index)) return false;
		while (!this->map_device[index]->Write(data, size) && retry++ < nTimes) {
			this->map_device[index]->Write(data, size);
		}
	}
	delete data;
	return false;
}

bool CHID_DeviceManager::SendContinuous(DataContinuous* send, int nTimes, int index)
{
	if (!send) return false;
	//�������ָ��ͳ���
	//TODO:�����data size��һ����64��Ŀǰ��64����ʵ���Ͽ��Ը����ֵ
	unsigned char* data = new unsigned char[64];
	size_t size = 0;
	send->toData(data, size);
	int retry = 0;
	//�Ƿ��������е��豸
	if (index == 0) {
		for (auto item : this->map_device) {
			ICommunicateDevice* device = item.second;
			while (!device->Write(data, size) && retry < nTimes) {
				device->Write(data, size);
			}
		}
	}
	else {
		if (!this->map_device.count(index)) return false;
		while (!this->map_device[index]->Write(data, size) && retry < nTimes) {
			this->map_device[index]->Write(data, size);
		}
	}
	return false;
}


void CHID_DeviceManager::HandleConfirmMessage(unsigned char* buff, size_t data_size, int index)
{
	//TODO:Ӧ����Ϣ�ݲ�����
}

void CHID_DeviceManager::HandleContinuousMessage(unsigned char* buff, size_t data_size, int index)
{
	DataContinuous data;
	ParseDataContinuous(buff, data_size, data);
	this->AnalyseContinuousData(data,index);
}

void CHID_DeviceManager::HandleRandomMessage(unsigned char* buff, size_t data_size, int index)
{
	DataRandom data;
	ParseDataRandom(buff, data_size, data);
	this->AnalyseRandomData(data,index);
}

void CHID_DeviceManager::AnalyseRawData(unsigned char* buff, int data_size, const char* path)
{
	if (buff == nullptr) return;
	//����Ϊֹ������ͻ���˵��� usb�豸�����ݣ������￪ʼ����Ҫ���н�����
	//�����ҵ�����˭
	int index_ = -1;
	for (auto item : this->map_device) {
		if (!strcmp(path, item.second->GetDevicePath())) {
			index_ = item.first;
			break;
		}
	}

	if (index_ == -1) return;

	size_t index = 0;
	uint8_t report = buff[index++];
	uint8_t header = buff[index++];
	uint8_t length_l = buff[index++];
	uint8_t length_h = buff[index++];
	//����һ�³���
	int length = length_h << 8 | length_l;
	//if (length != data_size) return;

	uint8_t addr = buff[index++];
	uint8_t cmd = buff[index++];

	switch (cmd) {
	case (int)ReplyType::Confirm: {
		//����Ǳ�׼Ӧ��
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


