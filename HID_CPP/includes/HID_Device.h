
#if !defined(HID_BASE)
#define HID_BASE

#include <iostream>
#include "IHID.h"
#include "hidapi.h"
#include <mutex>
#include <thread>
/**
 * @class CHidBase
 * @brief HID�豸�����࣬ʵ��ICommunicateDevice�ӿڣ�ֻ�ṩ�������HID�豸�������������й����豸�Ĺ�����Ҫ���ϲ�������ṩ
 * ע��Ŀǰ���̰߳�ȫ��
 *
 * ��������ʵ��HID�豸�ĳ�ʼ����ö�١��򿪡��رա���д�����Լ��豸�仯��֪ͨ�ص���
 * ö���豸Ӧ�����ϲ�ִ�У�����ֻʵ�����ض�����
 */
class CHID_Device : public ICommunicateDevice
{
public:
	/**
	 * @brief ���캯��
	 */
	CHID_Device();
	/**
   * @brief ��������
   */
	~CHID_Device();
	/**
  * @brief ��ʼ��HID�豸
  * @return �ɹ�����true�����򷵻�false
  */
	bool Init();
	/**
	* @brief ���豸
	* @param dev_path �豸·��
	* @param vid vID
	* @param pid pID
	* @param param3 ����3
	* @return �ɹ�����true�����򷵻�false
	*/
	bool OpenDevice(const char* dev_path, uint32_t vid, uint32_t pid, void* param3);
	/**
	* @brief ��ȡ�豸vid
	* @return �����豸vid
	*/
	uint32_t GetDeviceParam1() const;
	/**
	* @brief ��ȡ�豸pid
	* @return �����豸pid
	*/
	uint32_t GetDeviceParam2() const;
	/**
	* @brief �ж��豸�Ƿ��
	* @return �豸�򿪷���true�����򷵻�false
	*/
	bool IsDeviceOpen();
	/**
	* @brief ��ȡ�豸����
	* @return �����豸����
	*/
	const char* GetDeviceName()const;

	/**
	* @brief ��ȡ�豸·��
	* @return �����豸·��
	*/
	const char* GetDevicePath() const;


	/**
	* @brief �ر��豸
	*/
	void CloseDevice();

	/**
	 * @brief ��ȡ��󱨸泤��
	 * @return ������󱨸泤��
	 */
	int GetMaxReportLength();
	/**
	* @brief д���ݵ��豸
	* @param data ����
	* @param data_bytes �����ֽ���
	* @return �ɹ�����true�����򷵻�false
	*/
	bool Write(unsigned char* data, size_t data_bytes);

	/// <summary>
	/// ��ȡHID�л��������ݵ�buff
	/// </summary>
	/// <param name="buff">��ȡ���ݵ�uchar����</param>
	/// <param name="data_size">�ֽڳ���</param>
	/// <param name="waitTime">��ʱʱ��</param>
	/// <returns></returns>
	bool Read(unsigned char* buff, size_t& data_size, int waitTime);

	/// <summary>
	/// �̶߳�ȡ����
	/// </summary>
	void LoopRead();
	
private:
	/**
	 * hid�豸·��
	 */
	char device_path[256];

	/**
	 * �豸����
	 */
	char name[128];

	/**
	 * hid�豸���
	 */
	hid_device* handle;

	/**
	 * �������ݱ��ĵĳ���
	 */
	int input_report_length;

	//�豸�β��¼�֪ͨ�ص��������û����������

	void* user_data;


	/**
	 * �Ƿ�����ö���豸
	 * Mac������ö���豸ʱ�������ص�����device_change_callback
	 */
	bool enum_device;

	/**
	 * hid�豸��vid
	 */
	uint32_t device_vid;

	/**
	 * hid�豸��pid
	 */
	uint32_t device_pid;

	bool blnRunning = false;
	std::mutex mtx;  // ������
	std::thread readThread;
};

#endif // !defined(AFX_HID_H__A5700CF5_9580_4B1F_B9A6_5FC546E0EF3B__INCLUDED_)

