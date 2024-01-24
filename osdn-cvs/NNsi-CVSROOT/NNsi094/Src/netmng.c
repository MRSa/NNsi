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
/*  net_open() : ネットのオープン                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_open(UInt16 *netRef)
{
    UInt16 ifErr;
    Err    ret;

    // 変数の初期化
    ifErr = 0;

    // ネットライブラリのオープン
    (void) SysLibFind("Net.lib", netRef);

    // ネットワークＯＰＥＮ
    ret   = NetLibOpen(*netRef, &ifErr);
    if ((ret == errNone)||(ret == netErrAlreadyOpen))
    {
        if (ifErr == 0)
        {
            // open成功
            return (errNone);
        }
        (void) NetLibClose(*netRef, false);
        *netRef = 0;
        ret = ~errNone;
    }
    return (ret);
}

/*==========================================================================*/
/*  net_close() : ネットのクローズ                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_close(UInt16 netRef)
{
    return (NetLibClose(netRef, false));
}

/*==========================================================================*/
/*  net_write() : データを送信する                                          */
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
/*  net_read() : データを受信する                                           */
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
/*  net_disconnect() : ホストから切り離す                                   */
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
/*  net_connect() : ホストに接続                                            */
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

    // バッファ領域を確保
    infoBuff = MemPtrNew(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        return (~errNone);
    }
    timeout = NNshParam->netTimeout;

    // ホスト名からIPアドレスを取得
    hostIP = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);
    if (hostIP == 0)
    {
        // ホストIPの取得失敗
        NNsh_DebugMessage(ALTID_ERROR,"NetLibGetHostByName:", hostName, err);
        err = ~errNone - 5;
        goto FUNC_END;
    }

    // ソケットOPEN
    *socketRef = NetLibSocketOpen(netRef, netSocketAddrINET, 
                                  netSocketTypeStream, netSocketProtoIPTCP,
                                  timeout, &err);
    if (*socketRef == -1)
    {
        // ソケットOPEN異常
        *socketRef = 0;
        NNsh_DebugMessage(ALTID_ERROR,"NetLibSocketOpen()", "", err);
        goto FUNC_END;
    }

    // ソケットコネクト
    addrType.family = netSocketAddrINET;
    addrType.port   = NetHToNS(port);
    addrType.addr   = infoBuff->address[0];
    ret = NetLibSocketConnect(netRef, *socketRef,
                              (NetSocketAddrType *) &addrType,
                              sizeof(addrType), timeout, &err);
    if (ret == -1)
    {
        // ソケットコネクト異常
        NNsh_DebugMessage(ALTID_ERROR,"NetLibSocketConnect()", "", err);
        (void) NetLibSocketClose(netRef, *socketRef, timeout, &err);
        goto FUNC_END;
    }
    err = errNone;

FUNC_END:
    // バッファ領域を開放
    (void) MemPtrFree(infoBuff);

    return (err);
}
