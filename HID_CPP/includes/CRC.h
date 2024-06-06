#ifndef _H_CRC_8_THREAD_H_
#define _H_CRC_8_THREAD_H_
#include "HID_Global.h"
#include <iostream>
enum class HID_API CRCType {
    crc_8,
    crc_16,
    crc_32
};
// CRC8У��
// ���Ǿ�̬����������Ҫʵ����������ֱ�ӵ���
class HID_API CCRC
{
public:

    /**
     @brief ���CRC��У���
     @param[in] buf:Ҫ����CRCУ��͵�����
     @param[in] size:buf�����ݵ��ֽ���
     @return  uint8_t
     @sa
     @retval �������ݵ�CRCУ���
     @note
     @author hzf
    */
    static uint32_t Crc(const uint8_t* buf, uint32_t size, CRCType type);

    static uint8_t Crc8(const uint8_t* buf, uint32_t size);

    static uint32_t Crc32(const uint8_t* buf, uint32_t size);

    static uint16_t Crc16(const uint8_t* buf, uint32_t size);
};

#endif //_H_CRC_8_THREAD_H_
