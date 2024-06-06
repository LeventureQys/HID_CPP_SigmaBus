#include "HID_CcmEasyEncrypt.h"


CcmEasyEncrypt::CcmEasyEncrypt(uint16_t key, uint8_t mKey, uint16_t iaKey, uint8_t varKey, int nDataLen)
    : m_wKey(key)
    , m_byMKey(mKey)
    , m_wIaKey(iaKey)
    , m_wVarKey(varKey)
    , m_nDataLen(nDataLen)
{
}


void CcmEasyEncrypt::Encrypt(PACKDATA& data)
{
    //    uint8_t m = m_byMKey;
    //    uint16_t key = m_wKey;
    //    uint16_t iaKey = m_wIaKey;
    //    uint16_t varKey = m_wVarKey;

    //    if ( 0 == m )
    //        m = 20;
    //    if ( 0 == key )
    //        key = 100;

    //    uint8_t *buf = (uint8_t*)data.data;

    //    int i = 0;
    //    while ( i < m_nDataLen )
    //    {
    //        key = iaKey * (key % m ) + varKey;
    //        buf[i++] ^= (uint8_t)((key>>4) & 0xff);
    //    }
}

void CcmEasyEncrypt::Decrypt(PACKDATA& data)
{
    Encrypt(data);
}


//int HidRand(int min/*=0*/, int max/*=32767*/)
//{
//	static bool bInit = false;
//	if ( !bInit )
//	{
//		srand( (unsigned)time( NULL ) );
//		bInit = true;
//	}
//
//	return (int)((double)rand() / (RAND_MAX + 1) * (max - min) + min);
//}

