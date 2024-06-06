
#if !defined(HID_BASE)
#define HID_BASE

#include <iostream>
#include "IHID.h"
#include "hidapi.h"
#include <mutex>
#include <thread>
/**
 * @class CHidBase
 * @brief HID设备基础类，实现ICommunicateDevice接口，只提供最基本的HID设备操作，其余所有关于设备的管理都需要在上层调用中提供
 * 注：目前是线程安全的
 *
 * 该类用于实现HID设备的初始化、枚举、打开、关闭、读写操作以及设备变化的通知回调。
 * 枚举设备应该在上层执行，本层只实例化特定对象
 */
class CHID_Device : public ICommunicateDevice
{
public:
	/**
	 * @brief 构造函数
	 */
	CHID_Device();
	/**
   * @brief 析构函数
   */
	~CHID_Device();
	/**
  * @brief 初始化HID设备
  * @return 成功返回true，否则返回false
  */
	bool Init();
	/**
	* @brief 打开设备
	* @param dev_path 设备路径
	* @param vid vID
	* @param pid pID
	* @param param3 参数3
	* @return 成功返回true，否则返回false
	*/
	bool OpenDevice(const char* dev_path, uint32_t vid, uint32_t pid, void* param3);
	/**
	* @brief 获取设备vid
	* @return 返回设备vid
	*/
	uint32_t GetDeviceParam1() const;
	/**
	* @brief 获取设备pid
	* @return 返回设备pid
	*/
	uint32_t GetDeviceParam2() const;
	/**
	* @brief 判断设备是否打开
	* @return 设备打开返回true，否则返回false
	*/
	bool IsDeviceOpen();
	/**
	* @brief 获取设备名称
	* @return 返回设备名称
	*/
	const char* GetDeviceName()const;

	/**
	* @brief 获取设备路径
	* @return 返回设备路径
	*/
	const char* GetDevicePath() const;


	/**
	* @brief 关闭设备
	*/
	void CloseDevice();

	/**
	 * @brief 获取最大报告长度
	 * @return 返回最大报告长度
	 */
	int GetMaxReportLength();
	/**
	* @brief 写数据到设备
	* @param data 数据
	* @param data_bytes 数据字节数
	* @return 成功返回true，否则返回false
	*/
	bool Write(unsigned char* data, size_t data_bytes);

	/// <summary>
	/// 读取HID中缓冲区数据到buff
	/// </summary>
	/// <param name="buff">读取数据的uchar数字</param>
	/// <param name="data_size">字节长度</param>
	/// <param name="waitTime">超时时间</param>
	/// <returns></returns>
	bool Read(unsigned char* buff, size_t& data_size, int waitTime);

	/// <summary>
	/// 线程读取数据
	/// </summary>
	void LoopRead();
	
private:
	/**
	 * hid设备路径
	 */
	char device_path[256];

	/**
	 * 设备名称
	 */
	char name[128];

	/**
	 * hid设备句柄
	 */
	hid_device* handle;

	/**
	 * 接收数据报文的长度
	 */
	int input_report_length;

	//设备拔插事件通知回调函数的用户传入的数据

	void* user_data;


	/**
	 * 是否正在枚举设备
	 * Mac电脑上枚举设备时，会进入回调函数device_change_callback
	 */
	bool enum_device;

	/**
	 * hid设备的vid
	 */
	uint32_t device_vid;

	/**
	 * hid设备的pid
	 */
	uint32_t device_pid;

	bool blnRunning = false;
	std::mutex mtx;  // 互斥锁
	std::thread readThread;
};

#endif // !defined(AFX_HID_H__A5700CF5_9580_4B1F_B9A6_5FC546E0EF3B__INCLUDED_)

