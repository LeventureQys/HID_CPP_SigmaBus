#pragma once

#include <iostream>
#include "IHID.h"

/*
 * CCM数据包加密解密算法
 */
class CcmEasyEncrypt : public ICommEncrypt
{
public:

    /**
     @brief 构造函数
     @param[in] key:加密算法用到的key
     @param[in] mKey:加密算法用到的key
     @param[in] iaKey:加密算法用到的key
     @param[in] varKey:加密算法用到的key
     @param[in] nDataLen:数据包中需要加密的数据域的字节数
     @return
     @sa
     @retval
     @note
     @author hzf
    */
    CcmEasyEncrypt(uint16_t key, uint8_t mKey, uint16_t iaKey, uint8_t varKey, int nDataLen);

    /**
     @brief 加密数据
     @param[in out] data:要加密的数据包
     @return  void
     @sa
     @retval
     @note
     @author hzf
    */
    virtual void Encrypt(PACKDATA& data);

    /**
     @brief 解密数据
     @param[in out] data:要解密的数据包
     @return  void
     @sa
     @retval
     @note
     @author hzf
    */
    virtual void Decrypt(PACKDATA& data);

private:
    uint16_t m_wKey;
    uint8_t m_byMKey;
    uint16_t m_wIaKey;
    uint16_t m_wVarKey;
    int  m_nDataLen;
};

