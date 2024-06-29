#include "HID_Device.h"

CHID_Device::CHID_Device()
{
    memset(this->name, 0, sizeof(this->name));
    memset(this->device_path, 0, sizeof(this->device_path));
    this->handle = 0;
    this->input_report_length = 0;
    this->enum_device = false;
    this->device_vid = 0;
    this->device_pid = 0;
}

CHID_Device::~CHID_Device()
{
    this->blnRunning = false;
    if(this->readThread.joinable())
    this->readThread.join();
}

bool CHID_Device::Init()
{
    return false;
}

bool CHID_Device::IsDeviceOpen()
{

    if (this->handle)
    {
        return true;
    }
    else
    {
        return false;
    }
}


void CHID_Device::CloseDevice()
{
    this->blnRunning = false;
    if (this->handle)
    {
        hid_close(this->handle);
        this->handle = 0;
    }
    this->name[0] = '\0';
    this->input_report_length = 0;
}

const char* CHID_Device::GetDeviceName()const
{
    return this->name;
}

bool CHID_Device::OpenDevice(const char* dev_path, uint32_t vid, uint32_t pid, void* param3)
{
    if (!dev_path) {
        return false;
    }

    if (this->handle) return true;

    this->handle = hid_open_path(dev_path);


    if (!this->handle)
    {
        std::cout << __FUNCTION__ << " handle not exist!";
        return false;
    }

    wchar_t wstr[256] = { 0 };
    int res;
    res = hid_get_product_string(this->handle, wstr, 256);
    if (0 == res)
    {
        //wcharתchar
        size_t i;
        char* pMBBuffer = (char*)malloc(256);
#if defined(Q_OS_WIN32)
        wcstombs_s(&i, pMBBuffer, (size_t)256,
            wstr, (size_t)256);
        strcpy_s(this->name, strlen(pMBBuffer) + 1, pMBBuffer);
#else
        wcstombs(pMBBuffer, wstr, (size_t)256);
        //        wcstombs_s(&i, pMBBuffer, (size_t)256,
        //                      wstr, (size_t)256 );
        strcpy(this->name, pMBBuffer);
#endif
        free(pMBBuffer);
        pMBBuffer = NULL;
    }
#if defined(Q_OS_WIN32)
    strcpy_s(this->device_path, strlen(dev_path) + 1, dev_path);
#else
    strcpy(this->device_path, dev_path);
#endif


    this->input_report_length = 64;

    this->device_vid = vid;
    this->device_pid = pid;

    //初始化成功后，创建一个线程，让它监听设备的数据
    readThread = std::thread(&CHID_Device::LoopRead, this);
    this->blnRunning = true;
    return true;
}


int CHID_Device::GetMaxReportLength()
{
    return this->input_report_length;
}



bool CHID_Device::Read(unsigned char* pBuff, size_t& res, int waitTime)
{
    //std::lock_guard<std::mutex> lock(mtx);
    if (!this->handle) {
        return  false;
    }

    res = hid_read_timeout(this->handle, pBuff, this->GetMaxReportLength(), 1024);
    //这里计算一下长度
    int index = 0;
    unsigned char* pBuff_t = pBuff;
    bool blnWithHeader = false;
    if (pBuff_t[index] != 0xc4) {
        index++;
        //第一行不是帧头
        blnWithHeader = true;
    }
    if (pBuff_t[index] != 0xc4) return false;
    res = ((pBuff_t[index + 2] << 8) | pBuff_t[index + 1]);
    //计算长度
    if (blnWithHeader) {
        res++;
    }




    if (-1 == res)
        return false;
    else if (0 == res)
        return false;

        return true;
}

void CHID_Device::LoopRead()
{
    while (blnRunning) {
        const int buffer_size = this->GetMaxReportLength();
        unsigned char* buffer = new unsigned char[buffer_size];
        size_t data_size = 0;
        if (!Read(buffer, data_size, 1024)) {
            std::cerr << "Failed to read data or no data available." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 添加一些延迟
            continue;
        }
        else {
			std::cout << "RecvSuccess !" << std::endl;
        }

        // 此处处理读取到的数据，这里主要是通过回调发出去
        if (this->ReadCallback != nullptr) {
            this->ReadCallback(buffer, data_size);
        }

        // 可选的小延迟以防止CPU占用过高
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        delete[] buffer; // 释放分配的缓冲区
    }
}

bool CHID_Device::Write(unsigned char* data, size_t data_bytes)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (!this->handle) {
        return  false;
    }
    //还要加上report id
    int res = hid_write(this->handle, data, data_bytes + 1);
    std::cout << __FUNCTION__<<" Get Data: " << res << data_bytes<<std::endl;
    if (res != this->input_report_length)
        return false;

    return true;
}

uint32_t CHID_Device::GetDeviceParam1() const
{
    return this->device_vid;
}

uint32_t CHID_Device::GetDeviceParam2() const
{
    return this->device_pid;
}


const char* CHID_Device::GetDevicePath() const
{
    return this->device_path;
}