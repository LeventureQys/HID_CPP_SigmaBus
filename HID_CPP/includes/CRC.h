#ifndef _H_CRC_8_THREAD_H_
#define _H_CRC_8_THREAD_H_
#include "HID_Global.h"
#include <iostream>
enum class HID_API CRCType {
    crc_8,
    crc_16,
    crc_32
};
// CRC8校验
// 都是静态函数，不需要实例化，可以直接调用
class HID_API CCRC
{
public:

    /**
     @brief 获得CRC的校验和
     @param[in] buf:要计算CRC校验和的数据
     @param[in] size:buf中数据的字节数
     @return  uint8_t
     @sa
     @retval 返回数据的CRC校验和
     @note
     @author hzf
    */
    static uint32_t Crc(const uint8_t* buf, uint32_t size, CRCType type);

    static uint8_t Crc8(const uint8_t* buf, uint32_t size);

    static uint32_t Crc32(const uint8_t* buf, uint32_t size);

    static uint16_t Crc16(const uint8_t* buf, uint32_t size);
};

#endif //_H_CRC_8_THREAD_H_
