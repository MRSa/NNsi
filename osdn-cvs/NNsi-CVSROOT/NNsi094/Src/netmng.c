/*============================================================================*
 *  FILE: 
 *     netmng.c
 *
 *  Description: 
 *     NETLIB manager functions for NNsh.
 *
 *===========================================================================*/
#define NETMNG_C
#include "local.h"

/*==========================================================================*/
/*  net_open() : �l�b�g�̃I�[�v��                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_open(UInt16 *netRef)
{
    UInt16 ifErr;
    Err    ret;

    // �ϐ��̏�����
    ifErr = 0;

    // �l�b�g���C�u�����̃I�[�v��
    (void) SysLibFind("Net.lib", netRef);

    // �l�b�g���[�N�n�o�d�m
    ret   = NetLibOpen(*netRef, &ifErr);
    if ((ret == errNone)||(ret == netErrAlreadyOpen))
    {
        if (ifErr == 0)
        {
            // open����
            return (errNone);
        }
        (void) NetLibClose(*netRef, false);
        *netRef = 0;
        ret = ~errNone;
    }
    return (ret);
}

/*==========================================================================*/
/*  net_close() : �l�b�g�̃N���[�Y                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_close(UInt16 netRef)
{
    return (NetLibClose(netRef, false));
}

/*==========================================================================*/
/*  net_write() : �f�[�^�𑗐M����                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_write(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data)
{
    Err    err;
    Int16  ret;
    Int32  timeout;

    timeout = NNshParam->netTimeout;

    ret = NetLibSend(netRef, socketRef, data, size, 0, NULL, 0, timeout, &err);
    if (ret == -1)
    {
        return (err);
    }
    return (errNone);
}

/*==========================================================================*/
/*  net_read() : �f�[�^����M����                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_read(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, UInt16 *readSize)
{
    Err    err;
    Int32  timeout;
    Int16  ret;

    timeout = NNshParam->netTimeout;

    *readSize = 0;
    ret = NetLibReceive(netRef, socketRef, data, size,0,NULL,0,timeout, &err);
    if (ret == -1)
    {
        return (err);
    }

    *readSize = ret;
    return (errNone);
}

/*==========================================================================*/
/*  net_disconnect() : �z�X�g����؂藣��                                   */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_disconnect(UInt16 netRef, NetSocketRef socketRef)
{
    Err    err;
    Int32  timeout;

    timeout = NNshParam->netTimeout;

    (void) NetLibSocketShutdown(netRef, socketRef, netSocketDirBoth,
                                timeout, &err);
    (void) NetLibSocketClose(netRef, socketRef, timeout, &err);
    if (err == errNone)
    {
        socketRef = 0;
    }
    return (errNone);
}

/*==========================================================================*/
/*  net_connect() : �z�X�g�ɐڑ�                                            */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_connect(NetSocketRef *socketRef, UInt16 netRef, Char *hostName, UInt16 port)
{
    Err                  err;
    Int16                ret;
    Int32                timeout;
    NetHostInfoBufType  *infoBuff;
    NetHostInfoPtr       hostIP;
    NetSocketAddrINType  addrType;

    // �o�b�t�@�̈���m��
    infoBuff = MemPtrNew(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        return (~errNone);
    }
    timeout = NNshParam->netTimeout;

    // �z�X�g������IP�A�h���X���擾
    hostIP = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);
    if (hostIP == 0)
    {
        // �z�X�gIP�̎擾���s
        NNsh_DebugMessage(ALTID_ERROR,"NetLibGetHostByName:", hostName, err);
        err = ~errNone - 5;
        goto FUNC_END;
    }

    // �\�P�b�gOPEN
    *socketRef = NetLibSocketOpen(netRef, netSocketAddrINET, 
                                  netSocketTypeStream, netSocketProtoIPTCP,
                                  timeout, &err);
    if (*socketRef == -1)
    {
        // �\�P�b�gOPEN�ُ�
        *socketRef = 0;
        NNsh_DebugMessage(ALTID_ERROR,"NetLibSocketOpen()", "", err);
        goto FUNC_END;
    }

    // �\�P�b�g�R�l�N�g
    addrType.family = netSocketAddrINET;
    addrType.port   = NetHToNS(port);
    addrType.addr   = infoBuff->address[0];
    ret = NetLibSocketConnect(netRef, *socketRef,
                              (NetSocketAddrType *) &addrType,
                              sizeof(addrType), timeout, &err);
    if (ret == -1)
    {
        // �\�P�b�g�R�l�N�g�ُ�
        NNsh_DebugMessage(ALTID_ERROR,"NetLibSocketConnect()", "", err);
        (void) NetLibSocketClose(netRef, *socketRef, timeout, &err);
        goto FUNC_END;
    }
    err = errNone;

FUNC_END:
    // �o�b�t�@�̈���J��
    (void) MemPtrFree(infoBuff);

    return (err);
}
