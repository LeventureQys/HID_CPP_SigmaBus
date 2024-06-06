#pragma once

#include <iostream>
#include "IHID.h"

/*
 * CCM���ݰ����ܽ����㷨
 */
class CcmEasyEncrypt : public ICommEncrypt
{
public:

    /**
     @brief ���캯��
     @param[in] key:�����㷨�õ���key
     @param[in] mKey:�����㷨�õ���key
     @param[in] iaKey:�����㷨�õ���key
     @param[in] varKey:�����㷨�õ���key
     @param[in] nDataLen:���ݰ�����Ҫ���ܵ���������ֽ���
     @return
     @sa
     @retval
     @note
     @author hzf
    */
    CcmEasyEncrypt(uint16_t key, uint8_t mKey, uint16_t iaKey, uint8_t varKey, int nDataLen);

    /**
     @brief ��������
     @param[in out] data:Ҫ���ܵ����ݰ�
     @return  void
     @sa
     @retval
     @note
     @author hzf
    */
    virtual void Encrypt(PACKDATA& data);

    /**
     @brief ��������
     @param[in out] data:Ҫ���ܵ����ݰ�
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

