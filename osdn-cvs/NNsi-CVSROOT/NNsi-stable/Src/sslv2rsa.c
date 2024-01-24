/*============================================================================*
 *  FILE: 
 *     sslv2rc4.c
 *
 *  Description: 
 *     SSL v2�ʐM�pRSA�Í����v���O���� (SSLeay���)
 *
 *===========================================================================*/
#define SSLV2RSA_C

#ifdef GENERATE_CRYPTED_KEY_USING_OPENSSL
  // �L�[�Í����p
#else
#include "local.h"
#endif

#ifdef USE_SSL
#ifdef USE_SSL_V2

/*=========================================================================*/
/*   Function :   NNshPublicEncryptRsa                                     */
/*                                                 RSA���J�����g�����Í��� */
/*=========================================================================*/
UInt16 NNshPublicEncryptRsa(UInt16 flen, UInt8 *from, UInt8 *to, NNshSSLv2Ref *sslRef)
{
    UInt8 *ptr;

    // ���炩���� MASTER KEY��he.net�̌��J����encode�����f�[�^...
    // (����[����͂Ђ���[�ɃJ�b�R��邭�ČӎU�L��...)
    ptr = to;

    /////////////////////////////////////////////////////// ��������o�̓f�[�^
    #include "rsaoutput.dat"
    /////////////////////////////////////////////////////// �����܂ŏo�̓f�[�^
    return ((ptr - to));
}
#endif // #ifdef USE_SSL_V2
#endif // #ifdef USE_SSL
