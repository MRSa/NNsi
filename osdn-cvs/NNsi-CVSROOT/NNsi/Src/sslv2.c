/*============================================================================*
 *
 *  $Id: sslv2.c,v 1.27 2011/08/22 13:30:34 mrsa Exp $
 *
 *  FILE: 
 *     sslv2.c
 *
 *  Description: 
 *     SSL v2�ʐM�p
 *
 *===========================================================================*/
#define SSLV2_C
#include "local.h"

#ifdef USE_SSL
#ifdef USE_SSL_V2

/*=========================================================================*/
/*   Function : NNshSSLv2_Open                                             */
/*                                                          NNshSSL v2���� */
/*=========================================================================*/
Err NNshSSLv2_Open(NNshSSLv2Ref **sslRef)
{
    NNsh_DebugMessage(ALTID_INFO, "NNshSSLv2_Open()", " ", 0);

    *sslRef = MEMALLOC_PTR(sizeof(NNshSSLv2Ref) + MARGIN);
    if (*sslRef == NULL)    
    {
        // �̈�m�ێ��s�A�A�A�G���[��������
        NNsh_DebugMessage(ALTID_ERROR, "NNshSSLv2_Open()",
                          " size:", (sizeof(NNshSSLv2Ref) + MARGIN));
        return (~errNone);
    }
    MemSet(*sslRef, (sizeof(NNshSSLv2Ref) + MARGIN), 0x00);

    // �V�[�P���X�ԍ���������(...�Ȃ񂩈Ⴄ���������A�������킹...)
    (*sslRef)->seqNum = 1;

    // �o�b�t�@�̊m�ۂƏ�����
    (*sslRef)->buffer = MEMALLOC_PTR(NNSHSSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER + MARGIN);
    if ((*sslRef)->buffer == NULL)
    {
        // ����M�p�o�b�t�@���m�ۂł��Ȃ������A�̈���J������
        MEMFREE_PTR(*sslRef);
        return (~errNone);
    }
    MemSet((*sslRef)->buffer, (NNSHSSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER + MARGIN), 0x00);

    return (errNone); 
}

/*=========================================================================*/
/*   Function : NNshSSLv2_Close                                            */
/*                                                          NNshSSL v2�I�� */
/*=========================================================================*/
Err NNshSSLv2_Close(NNshSSLv2Ref **sslRef)
{
    NNsh_DebugMessage(ALTID_INFO, "NNshSSLv2_Close()", " ", 0);

    if (*sslRef != NULL)
    {
        MEMFREE_PTR((*sslRef)->connectionId);
        MEMFREE_PTR((*sslRef)->serverPublicKey);
        MEMFREE_PTR((*sslRef)->buffer);

        MEMFREE_PTR(*sslRef);
    }
    return (errNone); 
}

/*=========================================================================*/
/*   Function : NNshSSLv2_Connect                                          */
/*                                                          NNshSSL v2�ڑ� */
/*=========================================================================*/
Err NNshSSLv2_Connect(NNshSSLv2Ref *sslRef, NetSocketRef socketRef, UInt16 netRef)
{
    Err     ret;

    NNsh_DebugMessage(ALTID_INFO, "NNshSSLv2_Connect()", " ", 0);

    // SSL�g�p�ݒ�łȂ����(���[�N�̈悪�m�ۂ���Ă��Ȃ����)���������I������
    if (sslRef == NULL)
    {
        return (~errNone);
    }

    // �e�̈�/�ϐ��̏�����
    ret = errNone;
    sslRef->netRef    = netRef;
    sslRef->socketRef = socketRef;
    sslRef->state     = NNSHSSL_STATE_BEFORE;
    ret               = errNone;
    while (ret == errNone)
    {
        switch (sslRef->state)
        {
          case NNSHSSL_STATE_BEFORE:
          case NNSHSSL_STATE_CONNECT:
          case ((NNSHSSL_STATE_BEFORE)|(NNSHSSL_STATE_CONNECT)):
          case ((NNSHSSL_STATE_OK)|(NNSHSSL_STATE_CONNECT)):
            sslRef->state = SSL2_ST_SEND_CLIENT_HELLO;
            break;

          case SSL2_ST_SEND_CLIENT_HELLO:
            // Client Hello�̐����Ƒ��M
            client_hello_NNshSSLv2(sslRef);
            ret = NNshNet_write(netRef, socketRef, sslRef->bufLen, sslRef->buffer);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "NNshNet_write():CLIENT HELLO", " ret:", ret);
                break;
            }
            NNsh_DebugMessage(ALTID_INFO, "CLIENT HELLO", " ", 0);
            sslRef->state = SSL2_ST_GET_SERVER_HELLO;
            // �������񂾂��ƁA�ǂݏo�������Ɉڂ�̂��͂₷����̂���... 1�b��wait������...
            SysTaskDelay(1 * SysTicksPerSecond());
            break;

          case SSL2_ST_GET_SERVER_HELLO:
            // Server Hello�̎擾�Ɠ��e�m�F(�v���g�R�����T�|�[�g����Ă��邩�H)
            ret = NNshNet_read(netRef, socketRef, 
                               (NNSHSSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER),
                               sslRef->buffer, &(sslRef->bufLen));
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "NNshNet_read():SEVER HELLO", " ret:", ret);
                break;
            }
            ret = get_server_hello_NNshSSLv2(sslRef);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "get server hello", " ret:", ret);
                break;
            }
            sslRef->state = SSL2_ST_SEND_CLIENT_MASTER_KEY;
            NNsh_DebugMessage(ALTID_INFO, "SERVER HELLO", " ", 0);
            break;

          case SSL2_ST_SEND_CLIENT_MASTER_KEY:
            (void) client_master_key_NNshSSLv2(sslRef);
            ret = NNshNet_write(netRef, socketRef, sslRef->bufLen, sslRef->buffer);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "NNshNet_write():client master key", " ret:", ret);
                break;
            }
            sslRef->state = SSL2_ST_CREATE_ENCRYPTION_KEY;
            NNsh_DebugMessage(ALTID_INFO, "CLIENT MASTER KEY", " ", 0);

            // �������񂾂��ƁA�ǂݏo�������Ɉڂ�̂��͂₷����̂���... 1�b��wait������...
            SysTaskDelay(1 * SysTicksPerSecond());
            break;

          case SSL2_ST_CREATE_ENCRYPTION_KEY:
            (void) create_encryption_key_NNshSSLv2(sslRef);
            sslRef->state = SSL2_ST_GET_SERVER_VERIFY;
            break;

          case SSL2_ST_GET_SERVER_VERIFY:
            // Server Hello�̎擾�Ɠ��e�m�F(�v���g�R�����T�|�[�g����Ă��邩�H)
            ret = NNshNet_read(netRef, socketRef, 
                               (NNSHSSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER),
                               sslRef->buffer, &(sslRef->bufLen));
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "NNshNet_read:SERVER VERIFY", " ret:", ret);
                break;
            }

#ifdef USE_REPORTER
            HostTraceOutputB(appErrorClass, sslRef->buffer, sslRef->bufLen);
#endif // #ifdef USE_REPORTER

            ret = get_server_verify_NNshSSLv2(sslRef);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "get server verify", " ret", ret);
                OutputHexStringDebug(NNSH_LEVEL_INFO, "DATA:", sslRef->buffer, sslRef->bufLen);
                sslRef->state = SSL2_ST_SEND_CLIENT_HELLO;
                break;
            }
            sslRef->state = SSL2_ST_SEND_CLIENT_FINISHED;
            NNsh_DebugMessage(ALTID_INFO, "SERVER VERIFY", " ", 0);
            break;

          case SSL2_ST_SEND_CLIENT_FINISHED:
            (void) client_finished_NNshSSLv2(sslRef);
            ret = NNshNet_write(netRef, socketRef, sslRef->bufLen, sslRef->buffer);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "NNshNet_write():client finished", " ret:", ret);
                break;
            }
            sslRef->state = SSL2_ST_GET_SERVER_FINISHED;
            NNsh_DebugMessage(ALTID_INFO, "CLIENT FINISHED", " ", 0);
            // �������񂾂��ƁA�ǂݏo�������Ɉڂ�̂��͂₷����̂���... 1�b��wait������...
            SysTaskDelay(1 * SysTicksPerSecond());
            break;

          case SSL2_ST_GET_SERVER_FINISHED:
            ret = NNshNet_read(netRef, socketRef, 
                               (NNSHSSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER + MARGIN),
                               sslRef->buffer, &(sslRef->bufLen));
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "NNshNet_read:SERVER FINISHED", " ret:", ret);
                break;
            }
            ret = get_server_finished_NNshSSLv2(sslRef);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_INFO, "get server finished", " ret:", ret);
                break;
            }
            sslRef->state = SSL2_ST_START_ENCRYPTION;
            NNsh_DebugMessage(ALTID_INFO, "SERVER FINISHED", " ", 0);
            break;

          case SSL2_ST_START_ENCRYPTION:
            // �Í����ʐM...
            NNsh_DebugMessage(ALTID_INFO, "<< SSL HANDSHAKE DONE >>", " ", 0);
            goto FUNC_END;
            break;

          default:
            NNsh_DebugMessage(ALTID_INFO, "default", " ret:", ret);
            ret = ~errNone;
            break;
        }
    }   // while (ret == errNone)

FUNC_END:
    // �n���h�V�F�[�N�I���I
    return (ret);
}

/*=========================================================================*/
/*   Function : NNshSSLv2_Disconnect                                       */
/*                                                          NNshSSL v2�ؒf */
/*=========================================================================*/
Err NNshSSLv2_Disconnect(NNshSSLv2Ref *sslRef)
{
    // ���ɉ������Ȃ�
    return (errNone);
}

/*==========================================================================*/
/*  Function : NNshSSLv2_Read                                               */
/*                                                       NNshSSL v2�ǂݏo�� */
/*==========================================================================*/
Err NNshSSLv2_Read(NNshSSLv2Ref *sslRef, UInt16 size, void *data, UInt16 *readSize)
{
    Err     ret;
    UInt8  *ptr, *nextPtr;
    UInt16 *slen, len ,len2;

    // SSL�g�p�ݒ�łȂ����(���[�N�̈悪�m�ۂ���Ă��Ȃ����)���������I������
    *readSize = 0;
    if (sslRef == NULL)
    {
        return (~errNone);
    }

    // �f�[�^��ǂݍ���
    ret = NNshNet_read(sslRef->netRef, sslRef->socketRef, 
                       (NNSHSSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER + MARGIN),
                       sslRef->buffer, &(sslRef->bufLen));
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "NNshSSLv2_Read():read", " ", ret);
        return (ret);
    }
    if (sslRef->bufLen < MARGIN)
    {
        // ��M���s�i��M�f�[�^���Ȃ����̂Ƃ��ĉ�������j
        return (errNone);
    }
    NNsh_DebugMessage(ALTID_INFO, "NNshSSLv2_Read() rxSize:", " ", sslRef->bufLen);   // ��M�T�C�Y��\��

    // ����ȂɃ��b�Z�[�W�͑傫���Ȃ����̂Ɖ��肷��(2�o�C�g�w�b�_�Ɖ���)
    slen = (UInt16 *) sslRef->buffer;
    len  = ((0x7fff)&(*slen));
    if (size < len)
    {
        // �ő�f�[�^���𒴂��Ă����ꍇ�ɂ́A�^����ꂽ�o�b�t�@�T�C�Y�Ŋۂ߂�
        len = size;
    }
    NNsh_DebugMessage(ALTID_INFO, "NNshSSLv2_Read() len(1):", " ", len);

    // �f�[�^�̐擪��ptr�ɐݒ�
    ptr  = (UInt8 *) (slen + 1);

    // �{�f�B���ۂ��ƕ���...
    client_decode_NNshSSLv2(sslRef, len, ptr, data);

    // ���̕����Ƀf�[�^�����邩�ǂ����H �i�Í����f�[�^�������u���b�N����ꍇ�̑Ή�...�j
    nextPtr = ptr + len;
    slen = (UInt16 *) nextPtr;
    len2 = ((0x7fff)&(*slen));

    // hash�L�[���f�[�^��O�Ɋ񂹂�
    ptr = data;
    MemMove(ptr, &ptr[SSL2_MD5_HASH_LENGTH], (len - SSL2_MD5_HASH_LENGTH));

    *readSize = len - SSL2_MD5_HASH_LENGTH;

    NNsh_DebugMessage(ALTID_INFO, "NNshSSLv2_Read() len(2):", " ", len2);
    if ((len2 > MARGIN)&&(len2 < (size - len)))
    {
        // �f�[�^�̑�����ǂݍ���
        nextPtr = (UInt8 *) (slen + 1);
        client_decode_NNshSSLv2(sslRef, len2, nextPtr, (data + len));
        
        //MemMove(nextPtr, &nextPtr[SSL2_MD5_HASH_LENGTH], (len2 - SSL2_MD5_HASH_LENGTH));
        
        *readSize = *readSize + len2;
    }

    // ��M�f�[�^��\�����Ă݂�
    NNsh_DebugMessage(ALTID_INFO, "NNshSSLv2_Read() ", data, *readSize);

    return (errNone);
}

/*==========================================================================*/
/*  Function : NNshSSLv2_Write                                              */
/*                                                       NNshSSL v2�������� */
/*==========================================================================*/
Err NNshSSLv2_Write(NNshSSLv2Ref *sslRef, UInt16 size, void *data)
{
    Err ret;

    // ���M�f�[�^��\�����Ă݂�...
    NNsh_DebugMessage(ALTID_INFO, "NNshSSLv2_Write() ", data, size);

    // SSL�g�p�ݒ�łȂ����(���[�N�̈悪�m�ۂ���Ă��Ȃ����)���������I������
    if (sslRef == NULL)
    {
        return (~errNone);
    }

    // �Í����f�[�^���쐬����
    client_encode_NNshSSLv2(sslRef, size, data);
    if (sslRef->bufLen == 0)
    {
        NNsh_DebugMessage(ALTID_INFO, "encode length is wrong ", "", 0);
        return (~errNone);
    }
    NNsh_DebugMessage(ALTID_INFO, "SEND DATA ", " size:", sslRef->bufLen);

    // ���b�Z�[�W�𑗐M����
    ret = NNshNet_write(sslRef->netRef, sslRef->socketRef,
                        sslRef->bufLen, sslRef->buffer);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "NNshSSLv2_Write()", " ", ret);
    }
    return (ret);
}

/*--------------------------------------------------------------------------*/
/*  Function : client_hello_NNshSSLv2                                       */
/*                                                  SSL v2 client hello���M */
/*--------------------------------------------------------------------------*/
static void client_hello_NNshSSLv2(NNshSSLv2Ref *sslRef)
{
    UInt8   *ptr;
    UInt16  *size, loc, cnt;

    // Client Hello�̃f�[�^���쐬����
    loc  = 0;
    size = (UInt16 *) sslRef->buffer;
    ptr  = (UInt8 *) (size + 1);

    /* data type */
    ptr[loc] = SSL2_MT_CLIENT_HELLO;
    loc++;
    
    /* protocol version(high) */
    ptr[loc] = SSL2_VERSION_MAJOR;
    loc++;
    
    /* protocol version(low) */
    ptr[loc] = SSL2_VERSION_MINOR;
    loc++;

    /** Cipher Spec. length **/ // Cipher Spec.�́A�P�����L���ɂ���...
    ptr[loc] = 0x00;
    loc++;
    ptr[loc] = 0x03;
    loc++;

    /** session ID length **/
    ptr[loc] = 0x00;
    loc++;
    ptr[loc] = 0x00;
    loc++;

    /** challenge length **/
    ptr[loc] = 0x00;
    loc++;
    ptr[loc] = SSL2_CHALLENGE_LENGTH;
    loc++;
    
    /** Cipher spec. **/
    // SSL_RC4_128_WITH_MD5(0x010080)������L���ɂ���
    ptr[loc] = 0x01;
    loc++;
    ptr[loc] = 0x00;
    loc++;
    ptr[loc] = 0x80;
    loc++;

    /** challenge string **/
    // �`�������W������̐����A16�o�C�g�����A���炢���������Ȑ�����...
    for (cnt = 0; cnt < SSL2_CHALLENGE_LENGTH; cnt++)
    {
        ptr[loc] = (0xff & SysRandom(TimGetTicks() * (cnt + 1)))|(0x20);
        sslRef->challenge[cnt] = ptr[loc];
        loc++;
    }

    *size = 0x8000 | loc;
    sslRef->bufLen = loc + sizeof(UInt16);

    return;
}

/*--------------------------------------------------------------------------*/
/*  Function : get_server_hello_NNshSSLv2                                   */
/*                                                  SSL v2 Server hello��M */
/*--------------------------------------------------------------------------*/
static Err get_server_hello_NNshSSLv2(NNshSSLv2Ref *sslRef)
{
    UInt16 dataLen, sslVer, certLen, ciphLen, connLen, tmp, len;
    UInt8  mesType, certType, sessId, *ptr, *base;

    ptr    = sslRef->buffer;
    dataLen = 0x7fff & *((UInt16 *) sslRef->buffer);    
    ptr = ptr + 2;

    mesType = *ptr;
    ptr++;

    sessId  = *ptr;
    ptr++;

    certType = *ptr;
    ptr++;

    tmp      = (*ptr) << 8;
    ptr++;

    sslVer  = tmp + *ptr;
    ptr++;

    tmp      = (*ptr) << 8;
    ptr++;

    certLen = tmp + *ptr;
    ptr++;

    tmp      = (*ptr) << 8;
    ptr++;
    
    ciphLen = tmp + *ptr;
    ptr++;

    if (ciphLen == 0)
    {
        // �T�|�[�g�Í�������v���Ȃ��A�G���[�I������B
        return (~errNone);
    }

    tmp      = (*ptr) << 8;
    ptr++;

    connLen = *ptr;
    ptr++;

    // �ؖ����f�[�^(server_sertificate)
    base = ptr;
    while (ptr < base + certLen)
    {
        // �T�[�o�̌��J������������(�O�i�K�ɁAOBJ IDENTIFIER��T��)
        if ((*ptr == 0x06)&&(*(ptr + 1) == 0x03)&&(*(ptr + 2) == 0x55)&&
            (*(ptr + 3) == 0x04)&&(*(ptr + 4) == 0x03))
        {
            // "X.520 id-at, commonName"��������
            ptr = ptr + 4 + 1 + 1;
            len = *ptr;  // �f�[�^�o�C�g�����擾�Alen�ɑ��
            // ���ӁF�����O�X�o�C�g�͂P�o�C�g���Ə���ɉ��肵�Ă���
            ptr++;
            ptr = ptr + len;
            
            // ���́A0x06 (Object Identifier)�܂œǂݔ�΂�
            while (*ptr != 0x06)
            {
                if (ptr >= base + certLen)
                {
                    // �f�[�^���Ȃ��Ȃ����A�I������
                    goto END_CERT;
                }
                ptr++;
            }

            if ((*ptr == 0x06)&&(*(ptr + 1) == 0x09)&&(*(ptr + 2) == 0x2a)&&
                (*(ptr + 3) == 0x86)&&(*(ptr + 4) == 0x48)&&(*(ptr + 5) == 0x86)&&
                (*(ptr + 6) == 0xf7)&&(*(ptr + 7) == 0x0d)&&(*(ptr + 8) == 0x01)&&
                (*(ptr + 9) == 0x01)&&(*(ptr + 10) == 0x01)&&(*(ptr + 11) == 0x05)&&
                (*(ptr + 12) == 0x00))
            {
                // (PKCS #1, rsaEncryption) ��������
                ptr = ptr + 13;
                if ((*ptr == 0x03)&&(*(ptr + 1) == 0x81))
                {
                    // ���ӁF�����O�X�o�C�g�P�o�C�g�A�����g�p�r�b�g�Ȃ��Ɖ���
                    while ((*ptr != 0x02)||(*(ptr + 1) != 0x81))
                    {
                        if (ptr >= base + certLen)
                        {
                            // �f�[�^���Ȃ��Ȃ����A�I������
                            goto END_CERT;
                        }
                        ptr++;
                    }
                    // ���ӁF�����O�X�o�C�g�P�o�C�g�A�����g�p�r�b�g�Ȃ��Ɖ���
                    sslRef->serverPublicLen = *(ptr + 2) - 1;

                    ptr = ptr + 3 + 1;
                    sslRef->serverPublicKey = 
                                 MEMALLOC_PTR(sslRef->serverPublicLen + MARGIN);
                    if (sslRef->serverPublicKey == NULL)
                    {
                        // �f�[�^�̈�̊m�ێ��s�A�I������
                        break;
                    }
                    // �T�[�o��PublicKey���擾����
                    MemSet(sslRef->serverPublicKey, 
                           (sslRef->serverPublicLen + MARGIN), 0x00);
                    MemMove(sslRef->serverPublicKey, ptr, sslRef->serverPublicLen);
                    break;
                }
            }
        }
        ptr++;
    }
END_CERT:
    ptr = base + certLen;
    
    ptr = ptr + ciphLen;

    // �Z�b�V����ID�f�[�^(connection-Id?)
    MEMFREE_PTR(sslRef->connectionId);
    sslRef->connectionId = MEMALLOC_PTR(connLen + MARGIN);
    if (sslRef->connectionId == NULL)
    {
        return (~errNone);
    }
    MemSet (sslRef->connectionId, (connLen + MARGIN), 0x00);
    MemMove(sslRef->connectionId, ptr, connLen);
    sslRef->connectionIdLen = connLen;

    ptr = ptr + connLen;

    return (errNone);
}

/*--------------------------------------------------------------------------*/
/*  Function : client_master_key_NNshSSLv2                                  */
/*                                  SSL v2 client master key ���M�f�[�^�쐬 */
/*--------------------------------------------------------------------------*/
static Err client_master_key_NNshSSLv2(NNshSSLv2Ref *sslRef)
{
    UInt8   *ptr, *keyPtr;
    UInt16  *size, loc, lp;

    // Client Hello�̃f�[�^���쐬����
    loc  = 0;
    size = (UInt16 *) sslRef->buffer;
    ptr  = (UInt8 *) (size + 1);

    /** key ���� **/
    ptr[loc] = SSL2_MT_CLIENT_MASTER_KEY;
    loc++;
    
    /** Cipher spec. **/
    // SSL_RC4_128_WITH_MD5(0x010080)������L���ɂ���
    ptr[loc] = 0x01;
    loc++;
    ptr[loc] = 0x00;
    loc++;
    ptr[loc] = 0x80;
    loc++;

    // clear key data length == 0
    ptr[loc] = 0x00;
    loc++;
    ptr[loc] = 0x00;
    loc++;

    // encrypted data length(���Ƃœ����)
    keyPtr = &ptr[loc];
    loc++;
    loc++;

    // key argument length == 0
    ptr[loc] = 0x00;
    loc++;
    ptr[loc] = 0x00;
    loc++;

    // master key���T�[�o�̌��J���ňÍ�������
    MemMove(sslRef->clientMasterKey, "NNsi_for_PalmOS_(SSLv2FakeConn.)", SSL2_MASTERKEY_LENGTH);
    lp = StrLen(sslRef->clientMasterKey);
    lp = NNshPublicEncryptRsa(lp, sslRef->clientMasterKey, &ptr[loc], sslRef);

    // �Í����������̒������i�[����
    *keyPtr       = (0xff00 & lp) >> 8;
    *(keyPtr + 1) = (0x00ff & lp);

    loc = loc + lp;

    *size = 0x8000 | loc;
    sslRef->bufLen = loc + sizeof(UInt16);

    return (errNone);
}

/*--------------------------------------------------------------------------*/
/*  Function : create_encryption_key_NNshSSLv2                              */
/*                                     �f�[�^�ǂݏ����p�̈Í��L�[�𐶐����� */
/*--------------------------------------------------------------------------*/
static Err create_encryption_key_NNshSSLv2(NNshSSLv2Ref *sslRef)
{
    UInt16 loc;
    UInt8 *buf;

    loc = SSL2_MASTERKEY_LENGTH + 1 + SSL2_CHALLENGE_LENGTH + sslRef->connectionIdLen;
    buf = MEMALLOC_PTR(loc + MARGIN);
    if (buf == NULL)
    {
        // memory allocation failure...
        return (~errNone);
    }
    MemSet(buf, (loc + MARGIN), 0x00);

    // �f�[�^�̑O����
    MemSet(&(sslRef->digest),  sizeof(sslRef->digest), 0x00);
    MemSet(&(sslRef->PADDING), sizeof(sslRef->PADDING), 0x00);
    sslRef->PADDING[0] = 0x80;

    // MD5��hash�l���v�Z���ā@CLIENT-READ-KEY�����߂�
    MD5Init  (&(sslRef->context));
    MD5Update(&(sslRef->context), sslRef->clientMasterKey, SSL2_MASTERKEY_LENGTH);
    MD5Update(&(sslRef->context), "0", 1);
    MD5Update(&(sslRef->context), sslRef->challenge, SSL2_CHALLENGE_LENGTH);
    MD5Update(&(sslRef->context), sslRef->connectionId, sslRef->connectionIdLen);
    MD5Final (sslRef->digest, &(sslRef->context), sslRef->PADDING);
    MemMove  (sslRef->clientReadKey, sslRef->digest, SSL2_MD5_HASH_LENGTH);

    // �f�[�^�̑O����(��)
    MemSet(&(sslRef->digest),  sizeof(sslRef->digest), 0x00);
    MemSet(&(sslRef->PADDING), sizeof(sslRef->PADDING), 0x00);
    sslRef->PADDING[0] = 0x80;

    // MD5��hash�l���v�Z���ā@CLIENT-WRITE-KEY�����߂�
    MD5Init  (&(sslRef->context));
    MD5Update(&(sslRef->context), sslRef->clientMasterKey, SSL2_MASTERKEY_LENGTH);
    MD5Update(&(sslRef->context), "1", 1);
    MD5Update(&(sslRef->context), sslRef->challenge, SSL2_CHALLENGE_LENGTH);
    MD5Update(&(sslRef->context), sslRef->connectionId, sslRef->connectionIdLen);
    MD5Final (sslRef->digest, &(sslRef->context), sslRef->PADDING);
    MemMove  (sslRef->clientWriteKey, sslRef->digest, SSL2_MD5_HASH_LENGTH);

    // �m�ۂ����̈�̊J��
    MEMFREE_PTR(buf);

    // �ǂ݃L�[��������
    MemSet(&(sslRef->workReadKey), sizeof(RC4_KEY), 0x00);
    RC4_SetKey(&(sslRef->workReadKey), SSL2_RWKEY_LENGTH, sslRef->clientReadKey);

    // �����L�[��������
    MemSet(&(sslRef->workWriteKey), sizeof(RC4_KEY), 0x00);
    RC4_SetKey(&(sslRef->workWriteKey), SSL2_RWKEY_LENGTH, sslRef->clientWriteKey);

    return (errNone);
}

/*--------------------------------------------------------------------------*/
/*  Function : client_finished_NNshSSLv2                                    */
/*                                 �l�S�V�G�[�V�����I���̃��b�Z�[�W(client) */
/*--------------------------------------------------------------------------*/
static Err client_finished_NNshSSLv2(NNshSSLv2Ref *sslRef)
{
    UInt8   *ptr;

    // ���[�N�o�b�t�@���m��
    ptr = MEMALLOC_PTR((sslRef->connectionIdLen + 1) + MARGIN);
    if (ptr == NULL)
    {
        return (~errNone);
    }
    MemSet(ptr, ((sslRef->connectionIdLen + 1) + MARGIN), 0x00);
    
    // ���M�f�[�^���o�b�t�@�ɃR�s�[����
    *ptr = SSL2_MT_CLIENT_FINISHED;
    MemMove((ptr + 1), sslRef->connectionId, sslRef->connectionIdLen);

    // Client Finished�̃f�[�^���쐬����
    client_encode_NNshSSLv2(sslRef, (sslRef->connectionIdLen + 1), ptr);

    // ���[�N�o�b�t�@�����
    MEMFREE_PTR(ptr);

    return (errNone);
}

/*--------------------------------------------------------------------------*/
/*  Function : get_server_verify_NNshSSLv2                                  */
/*                                            SSL v2 server�����R�[�h�̌��� */
/*--------------------------------------------------------------------------*/
static Err get_server_verify_NNshSSLv2(NNshSSLv2Ref *sslRef)
{
    Int16    match;
    UInt16  *size, len, bufSize;
    UInt8   *ptr,  *workP;

    bufSize = sslRef->bufLen;
    if (bufSize > (SSL2_MD5_HASH_LENGTH + SSL2_CHALLENGE_LENGTH + 1 + 2))
    {
        bufSize = (SSL2_MD5_HASH_LENGTH + SSL2_CHALLENGE_LENGTH + 1 + 2);
        return (~errNone + 15);
    }

    // �̈���m�ۂ���
    workP = MEMALLOC_PTR(bufSize + MARGIN);
    if (workP == NULL)
    {
        return (~errNone + 15);
    }
    MemSet(workP, (bufSize + MARGIN), 0x00);

    // ������
    size = (UInt16 *) sslRef->buffer;
    len  = ((0x7fff)&(*size));
    ptr  = (UInt8 *) (size + 1);

    // �{�f�B���ۂ��ƕ���...
    if (len > bufSize)
    {
        len = bufSize;
        return (~errNone + 3);
    }
    client_decode_NNshSSLv2(sslRef, len, ptr, workP);

    // HASH�l�̌��؂͍s��Ȃ�
    ptr = workP + SSL2_MD5_HASH_LENGTH;

    // �V�[�P���X�R�[�h�̊m�F
    if (*ptr != SSL2_MT_SERVER_VERIFY)
    {
        NNsh_DebugMessage(ALTID_INFO, "server verify[NG:CODE]", " id:", *ptr);
        MEMFREE_PTR(workP);
        return (~errNone);
    }

    // CHALLENGE�̊m�F
    match = MemCmp((ptr + 1), sslRef->challenge, SSL2_CHALLENGE_LENGTH);
    if (match != 0)
    {
        NNsh_DebugMessage(ALTID_INFO, "message does not match challenge str.",
                          " ", 0);
        MEMFREE_PTR(workP);
        return (~errNone);
    }
    MEMFREE_PTR(workP);
    return (errNone);
}

/*--------------------------------------------------------------------------*/
/*  Function : get_server_finished_NNshSSLv2                                */
/*                                 �l�S�V�G�[�V�����I���̃��b�Z�[�W(server) */
/*--------------------------------------------------------------------------*/
static Err get_server_finished_NNshSSLv2(NNshSSLv2Ref *sslRef)
{
    UInt16  *size, len;
    UInt8   *ptr,  *workP;

    // ������
    workP = MEMALLOC_PTR(SSL2_MAX_SSL_SESSION_ID_LENGTH + SSL2_MD5_HASH_LENGTH + MARGIN);
    if (workP == NULL)
    {
        // �̈�m�ێ��s
        return (~errNone + 10);
    }
    MemSet(workP, (SSL2_MAX_SSL_SESSION_ID_LENGTH + SSL2_MD5_HASH_LENGTH + MARGIN), 0x00);

    size = (UInt16 *) sslRef->buffer;
    len  = ((0x7fff)&(*size));

    ptr  = (UInt8 *) (size + 1);

    // �{�f�B���ۂ��ƕ���...
    client_decode_NNshSSLv2(sslRef, len, ptr, workP);

    // HASH�l�̌��؂͍s��Ȃ�
    ptr = workP + SSL2_MD5_HASH_LENGTH;

    // �V�[�P���X�R�[�h�̊m�F
    if (*ptr != SSL2_MT_SERVER_FINISHED)
    {
        NNsh_DebugMessage(ALTID_INFO, "server finished[NG]", " RX:", *ptr);
        OutputHexStringDebug(NNSH_LEVEL_DEBUG, "DECODED  DATA :", workP, len);
        return (~errNone);
    }

    // �{���Ȃ�A�����ŃZ�b�V�����ĊJ�p�̃Z�b�V����ID���E���K�v�����邪�ȗ�

    MEMFREE_PTR(workP);
    return (errNone);
}

/*--------------------------------------------------------------------------*/
/*  Function : client_encode_NNshSSLv2                                      */
/*                                                 ���M�f�[�^�̈Í������s�� */
/*--------------------------------------------------------------------------*/
static void client_encode_NNshSSLv2(NNshSSLv2Ref *sslRef, UInt16 dataLen, Char *data)
{
    UInt8   *ptr, *orgPtr;
    UInt16  *size, lp;

    // Client Finished�̃f�[�^���쐬����
    size  = (UInt16 *) sslRef->buffer;
    ptr   = (UInt8 *) (size + 1);
    *size = 0;

    orgPtr = MEMALLOC_PTR(SSL2_MD5_HASH_LENGTH + dataLen + MARGIN);
    if (orgPtr == NULL)
    {
        // �̈�m�ێ��s�A�I������
        NNsh_DebugMessage(ALTID_INFO, "memory alloc. failure"," size:",
                          (SSL2_MD5_HASH_LENGTH + dataLen + MARGIN));
        sslRef->bufLen = 0;
        return;
    }
    MemSet(orgPtr, (SSL2_MD5_HASH_LENGTH + dataLen + MARGIN), 0x00);

    // �f�[�^�̑O����
    MemSet(&(sslRef->digest),  sizeof(sslRef->digest), 0x00);
    MemSet(&(sslRef->PADDING), sizeof(sslRef->PADDING), 0x00);
    sslRef->PADDING[0] = 0x80;

    // MD5��hash�l���v�Z���� MAC�����߂�
    sslRef->seqNum++;
    MD5Init  (&(sslRef->context));
    MD5Update(&(sslRef->context), sslRef->clientWriteKey, SSL2_RWKEY_LENGTH);
    MD5Update(&(sslRef->context), data, dataLen);
    MD5Update(&(sslRef->context), (UInt8 *) &(sslRef->seqNum), sizeof(UInt32));
    MD5Final (sslRef->digest, &(sslRef->context), sslRef->PADDING);
    MemMove  (orgPtr, sslRef->digest, SSL2_MD5_HASH_LENGTH);

    // ���b�Z�[�W�{�f�B���R�s�[����
    MemMove  ((orgPtr + SSL2_MD5_HASH_LENGTH), data, dataLen);

    // �{�f�B���ۂ��ƈÍ���...
    for (lp = 0; lp < (dataLen + SSL2_MD5_HASH_LENGTH); lp++)
    {
        // 1�o�C�g�ÂÍ���...
        RC4_Calculate(&(sslRef->workWriteKey), 1, &orgPtr[lp], &ptr[lp]);
    }
    MEMFREE_PTR(orgPtr);
    *size = 0x8000 | (SSL2_MD5_HASH_LENGTH + dataLen);
    sslRef->bufLen = (SSL2_MD5_HASH_LENGTH + dataLen) + sizeof(UInt16);

    return;
}

/*--------------------------------------------------------------------------*/
/*  Function : client_decode_NNshSSLv2                                      */
/*                                                   ��M�f�[�^�̕������s�� */
/*--------------------------------------------------------------------------*/
static void client_decode_NNshSSLv2(NNshSSLv2Ref *sslRef, UInt16 size, UInt8 *from, UInt8 *to)
{
    UInt16 lp;

    for (lp = 0; lp < size; lp++)
    {
        // 1�o�C�g�Â���...
        RC4_Calculate(&(sslRef->workReadKey), 1, &from[lp], &to[lp]);
    }
    return;
}

#endif  //#ifdef USE_SSL_V2
#endif  //#ifdef USE_SSL
