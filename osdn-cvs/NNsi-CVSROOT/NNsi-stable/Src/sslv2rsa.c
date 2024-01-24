/*============================================================================*
 *  FILE: 
 *     sslv2rc4.c
 *
 *  Description: 
 *     SSL v2通信用RSA暗号化プログラム (SSLeayより)
 *
 *===========================================================================*/
#define SSLV2RSA_C

#ifdef GENERATE_CRYPTED_KEY_USING_OPENSSL
  // キー暗号化用
#else
#include "local.h"
#endif

#ifdef USE_SSL
#ifdef USE_SSL_V2

/*=========================================================================*/
/*   Function :   NNshPublicEncryptRsa                                     */
/*                                                 RSA公開鍵を使った暗号化 */
/*=========================================================================*/
UInt16 NNshPublicEncryptRsa(UInt16 flen, UInt8 *from, UInt8 *to, NNshSSLv2Ref *sslRef)
{
    UInt8 *ptr;

    // あらかじめ MASTER KEYをhe.netの公開鍵でencodeしたデータ...
    // (いやーこれはひじょーにカッコわるくて胡散臭い...)
    ptr = to;

    /////////////////////////////////////////////////////// ここから出力データ
    #include "rsaoutput.dat"
    /////////////////////////////////////////////////////// ここまで出力データ
    return ((ptr - to));
}
#endif // #ifdef USE_SSL_V2
#endif // #ifdef USE_SSL
