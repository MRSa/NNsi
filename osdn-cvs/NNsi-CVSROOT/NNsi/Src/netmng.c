/*============================================================================*
 *
 *  $Id: netmng.c,v 1.29 2004/05/20 08:41:07 mrsa Exp $
 *
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

    timeout = (NNshGlobal->NNsiParam)->netTimeout;

    // 通常のNetLibだと、通信先アドレスの指定は不要だが...
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

    timeout = (NNshGlobal->NNsiParam)->netTimeout;

    *readSize = 0;
    // 通常のNetLibだと通信先アドレスの指定は不要だが...
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

    timeout = (NNshGlobal->NNsiParam)->netTimeout;

    (void) NetLibSocketShutdown(netRef, socketRef, netSocketDirBoth,
                                timeout, &err);
    (void) NetLibSocketClose(netRef, socketRef, timeout, &err);

    return (errNone);
}

/*==========================================================================*/
/*  net_connect() : ホストに接続                                            */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_connect(NetSocketRef *socketRef, UInt16 netRef, Char *hostName, UInt16 port)
{
    Err                  err;
    Int16                ret, addrLen;
    Int32                timeout;
    NetHostInfoBufType  *infoBuff;
    NetHostInfoPtr       hostIP;
    NetSocketAddrINType *addrType;
    NetSocketAddrType    addrBuf;

#ifdef SET_SOCKET_OPTION
    Boolean              flag;
#endif  // #ifdef SET_SOCKET_OPTION

#ifdef SET_CONNECT_REFRESH
    UInt8                openFlag;
    UInt16               errInfo;

    // 物理接続情報を更新(追加...)
    err = NetLibConnectionRefresh(netRef, true,  &openFlag, &errInfo);
    if (err != errNone)
    {
        // 接続情報の更新に失敗
        NNsh_DebugMessage(ALTID_ERROR, "NetLibConnectionRefresh()", " ", err);
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass,
                          "NetLibConnectionRefresh(): flag:%hd, info:%hu/0x%hx",
                          (UInt16) openFlag, errInfo, errInfo);
#endif  // #ifdef USE_REPORTER
        // 終了しない...
    }
#endif  // #ifdef SET_CONNECT_REFRESH

    // バッファ領域を確保
    infoBuff = MEMALLOC_PTR(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        return (~errNone);
    }
    MemSet(infoBuff, sizeof(NetHostInfoBufType), 0x00);
    timeout = (NNshGlobal->NNsiParam)->netTimeout;

    // ホスト名からIPアドレスを取得
    hostIP = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);
    if (hostIP == 0)
    {
        // ホストIPの取得失敗
        NNsh_DebugMessage(ALTID_ERROR,"NetLibGetHostByName:", hostName, err);
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "HOST :%s => IP:0x%lx",
                      hostName, NetHToNS(infoBuff->address[0]));
    HostTraceOutputB(appErrorClass, infoBuff, sizeof(NetHostInfoBufType));
#endif // #ifdef USE_REPORTER

    // ソケットOPEN
    *socketRef = NetLibSocketOpen(netRef, netSocketAddrINET, 
                                  netSocketTypeStream, netSocketProtoIPTCP,
                                  timeout, &err);
    if (*socketRef == -1)
    {
        // ソケットOPEN異常
        *socketRef = 0;
        NNsh_DebugMessage(ALTID_ERROR,"NetLibSocketOpen()", "", err);
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

#ifdef SET_SOCKET_OPTION
    // ソケットオプションの設定...
    flag = true;
    ret  = NetLibSocketOptionSet(netRef, *socketRef, 
                                 netSocketOptLevelSocket,
                                 netSocketOptSockNonBlocking,
                                 &flag, sizeof(flag), 
                                 timeout, &err);
    if (ret == -1)
    {
        // ソケットオプション設定失敗
        NNsh_DebugMessage(ALTID_ERROR,
                          "NetLibSocketOptionSet()", 
                          "netSocketOptSockNonBlocking == TRUE", err);
        // 続ける...
    }

    // ソケットオプションの設定...
    flag = true;
    ret  = NetLibSocketOptionSet(netRef, *socketRef, 
                                 netSocketOptLevelTCP,
                                 netSocketOptTCPNoDelay,
                                 &flag, sizeof(flag), 
                                 timeout, &err);
    if (ret == -1)
    {
        // ソケットオプション設定失敗
        NNsh_DebugMessage(ALTID_ERROR,
                          "NetLibSocketOptionSet()", 
                          "netSocketOptTCPNoDelay == TRUE", err);
        // 続ける...
    }
#endif // #ifdef SET_SOCKET_OPTION

    // ソケットコネクト
    MemSet(&addrBuf, sizeof(NetSocketAddrType), 0x00);
    addrType = (NetSocketAddrINType *) &addrBuf;
    addrType->family = netSocketAddrINET;
    addrType->port   = NetHToNS(port);
    //addrType->addr  = NetHToNS(infoBuff->address[0]);
    MemMove(&(addrType->addr), *(hostIP->addrListP), hostIP->addrLen); 
    addrLen         = sizeof(NetSocketAddrType);
    err             = errNone;

    // NetLibの場合には、NetLibSocketConnect()を使用する
#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "CONNECTING TO %s(IP:0x%lx), port:%d ",
                      hostName, NetHToNS(addrType->addr), addrType->port);
#endif // #ifdef USE_REPORTE

    ret = NetLibSocketConnect(netRef, *socketRef,
                              (NetSocketAddrType *) &addrBuf,
                              addrLen, timeout, &err);
    if (ret == -1)
    {
        // ソケットコネクト異常
        NNsh_DebugMessage(ALTID_ERROR,"NetLibSocketConnect()", "", err);
        (void) NetLibSocketClose(netRef, *socketRef, timeout, &err);
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }
    err = errNone;

FUNC_END:
    // バッファ領域を開放
    MEMFREE_PTR(infoBuff);

    return (err);
}

/*=========================================================================*/
/*   Function :   NNshNet_CheckHostAlive                                   */
/*                                               ホストとの接続を確認する  */
/*=========================================================================*/
void NNshNet_CheckHostAlive(UInt16 netRef, Char *hostName,
                                               UInt16 count, Char *resultMsg)
{
    Err                  err; 
    UInt32               dmmy;
    Int32                timeout;
    Char                 tempBuf[BUFSIZE + MARGIN], *ptr;
    NetHostInfoBufType  *infoBuff;
    NetHostInfoPtr       hostIP;
    NNshFileRef          fileRef;

    // 回線接続中でなければ、何もしない
    if (netRef == 0)
    {
        return;
    }

    // バッファ領域を確保
    infoBuff = MEMALLOC_PTR(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        StrCat(resultMsg, "\n(MemAlloc Fail...)");
        return;
    }

    // ホスト名からIPアドレスを取得
    timeout = (NNshGlobal->NNsiParam)->netTimeout;
    MemSet(infoBuff, sizeof(NetHostInfoBufType), 0x00);
    hostIP  = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);

    // IPアドレスを文字列に変換する
    MemSet(tempBuf, sizeof(tempBuf), 0x00);
    if (hostIP != 0)
    {
        if (infoBuff->address[0] != 0)
        {
            NetLibAddrINToA(netRef, infoBuff->address[0], tempBuf);
        }
        else
        {
            // エラー表示する
            if (resultMsg != NULL)
            {
                StrCopy(resultMsg, hostName);
                StrCat(resultMsg, "\n    (Wrong IP)");
            }
            goto FUNC_END;
        }
    }
    else
    {
        // エラー応答、それを画面表示する
        if (resultMsg != NULL)
        {
            StrCopy(resultMsg, hostName);
            StrCat (resultMsg, "\n    (Unknown IP)");
            StrCat (resultMsg, " Err:0x");
            NUMCATH(resultMsg, err);
        }
        goto FUNC_END;
    }
    if (resultMsg != NULL)
    {
        // ホスト名から取得したIPアドレスを表示
        StrCat(resultMsg, "Host : ");
        StrCat(resultMsg, hostName);
        StrCat(resultMsg, "(");
        StrCat(resultMsg, tempBuf);
        StrCat(resultMsg, ")\n---\n");
    }

    // ホストとの接続確認(HTTPサーバとの接続確認)
    MemSet(tempBuf, sizeof(tempBuf), 0x00);
    StrCopy(tempBuf, "http://");
    StrCat (tempBuf, hostName);
    StrCat (tempBuf, "/index.html");    
    err = NNshHttp_comm(HTTP_SENDTYPE_GET, tempBuf, NULL, NULL,
                        HTTP_RANGE_OMIT, sizeof(tempBuf), NULL);
    if (err != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (err != netErrTimeout)
        {
            StrCat (resultMsg, "Communication Error ret:0x");
            NUMCATH(resultMsg, err);
        }
        else
        {
            // 通信タイムアウト発生(受信しているところまで、強制的に解釈を行う)
            StrCat (resultMsg, "Connection Timeout ret:0x");
            NUMCATH(resultMsg, err);
        }
    }
    else
    {    
        StrCat (resultMsg, "Response: ");
        
        // 通信成功! ヘッダ部分を表示する
        OpenFile_NNsh(FILE_RECVMSG, 
                      (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                      &fileRef);
        err = ReadFile_NNsh(&fileRef, 0, sizeof(tempBuf), tempBuf, &dmmy);
        CloseFile_NNsh(&fileRef);
        if ((err != errNone)&&(err != vfsErrFileEOF)&&(err != fileErrEOF))
        {        
            // ファイル読みこみエラー
            StrCat (resultMsg, "Read Error ret:0x");
            NUMCATH(resultMsg, err);
        }
        else
        {
            // ヘッダ部分を表示する
            tempBuf[BUFSIZE] = '\0';
            ptr = tempBuf;
            while ((*ptr != '\x0a')&&(*ptr != '\x0d')&&(*ptr != '\0'))
            {
                ptr++;
            }
            *ptr = '\0';
            StrCat(resultMsg, tempBuf);
        }
    }

FUNC_END:
    MEMFREE_PTR(infoBuff);
    return;    
}

/*=========================================================================*/
/*   Function :   NNshNet_GetNetIFInfo                                     */
/*                                                       IF情報を取得する  */
/*=========================================================================*/
void NNshNet_GetNetIFInfo(UInt16 netRef, Char *buffer)
{
    Err                 ret;
    Char                tempBuf[BUFSIZE * 2 + MARGIN];
    UInt16              index, instance;
    UInt32              creatorId;
    NetMasterPBType     cmdParam;
    UInt16              size;
    UInt32              ipAddr;
    Int32               timeout;    
    NetHostInfoBufType  infoBuff;
    NetHostInfoPtr      hostIP;

    // とりあえず、区切り線を入れておく
    StrCat(buffer, "--- NetLib Information ---\n");
    
    // 回線情報を表示する。
    if (netRef != 0)
    {
        // 自ホスト名を取得して表示する
        // ネットワーク情報を取得する
        index = 0;
        ret = errNone;
        while (ret == errNone)
        {
            // ネットワークのインスタンスを取得する
            ret = NetLibIFGet(netRef, index, &creatorId, &instance);
            switch (ret)
            {
              case errNone:
               // インスタンスの取得に成功
               break;

              case netErrInterfaceNotFound:
              case netErrInvalidInterface:
              case netErrTooManyInterfaces:
                // 終了
                goto NEXT_STEP;
                break;

              case netErrPrefNotFound:
              default:
                // エラー発生...
                StrCat (buffer, "  NetLibIFGet():0x");
                NUMCATH(buffer, ret);
                StrCat (buffer, ", index:");
                NUMCATI(buffer, index);
                ret = errNone;
                goto AGAIN;
                break;
            }

            // インタフェース情報を取得する
            MemSet(&cmdParam, sizeof(cmdParam), 0x00);
            cmdParam.param.interfaceInfo.index = index;
            ret = NetLibMaster(netRef, netMasterInterfaceInfo,
                               &cmdParam, 1000);
            if (ret == errNone)
            {
                // インタフェース名とドライバ名を表示する
                StrCat(buffer, "  ");
                StrCat(buffer, cmdParam.param.interfaceInfo.ifName);
                StrCat(buffer, "(");
                StrCat(buffer, cmdParam.param.interfaceInfo.drvrName);
                StrCat(buffer, "): ");
                StrCat(buffer, cmdParam.param.interfaceInfo.hwName);

                if (cmdParam.param.interfaceInfo.ifUp != false)
                {
                    // IPアドレスからホスト名を取得
                    ipAddr  = cmdParam.param.interfaceInfo.ipAddr;
                    timeout = (NNshGlobal->NNsiParam)->netTimeout;
                    MemSet(&infoBuff, sizeof(infoBuff), 0x00);
                    hostIP = NetLibGetHostByAddr(netRef, (UInt8 *)&ipAddr,
                                                 sizeof(UInt32),
                                                 netSocketAddrINET, &infoBuff,
                                                 timeout, &ret);
                    if (hostIP != 0)
                    {
                        // ホスト名を表示
                        StrCat(buffer, "\n    Name:");
                        StrCat(buffer, infoBuff.hostInfo.nameP);
                    }

                    // IPアドレス
                    StrCat(buffer, "\n    IP Addr:");
                    MemSet(tempBuf, sizeof(tempBuf), 0x00);
                    NetLibAddrINToA(netRef, 
                                    cmdParam.param.interfaceInfo.ipAddr,
                                    tempBuf);
                    StrCat(buffer, tempBuf);

                    // 通信速度
                    StrCat(buffer, "\n    speed: ");
                    NUMCATI(buffer, cmdParam.param.interfaceInfo.speed);
                    StrCat(buffer, "bps");
#if DISPLAY_DETAIL_NETINFO
                    // サブネットマスク
                    StrCat(buffer, "\n    mask: ");
                    MemSet(tempBuf, sizeof(tempBuf), 0x00);
                    NetLibAddrINToA(netRef, 
                                    cmdParam.param.interfaceInfo.subnetMask,
                                    tempBuf);
                    StrCat(buffer, tempBuf);

                    // ブロードキャスト
                    StrCat(buffer, "\n    broad: ");
                    MemSet(tempBuf, sizeof(tempBuf), 0x00);
                    NetLibAddrINToA(netRef, 
                                    cmdParam.param.interfaceInfo.broadcast,
                                    tempBuf);
                    StrCat(buffer, tempBuf);

                    // MTU
                    StrCat(buffer, "\n    MTU: ");
                    NUMCATI(buffer, cmdParam.param.interfaceInfo.mtu);

                    // local net header
                    StrCat(buffer, "\n    locNetHdr: ");
                    NUMCATI(buffer,
                                  cmdParam.param.interfaceInfo.localNetHdrLen);

                    // local net trailer
                    StrCat(buffer, "\n    locNetTrail: ");
                    NUMCATI(buffer,
                              cmdParam.param.interfaceInfo.localNetTrailerLen);
                    
                    // local net max. frame size
                    StrCat(buffer, "\n    locNetMaxFrame: ");
                    NUMCATI(buffer,
                                cmdParam.param.interfaceInfo.localNetMaxFrame);
#endif
                }
                else
                {
                    // I/F がオフのとき
                    StrCat(buffer, "\n    (Media Disconnected)");
                }
            }
            else
            {
                StrCat (buffer, "  NetLibMaster():0x");
                NUMCATH(buffer, ret);
                StrCat (buffer, " (NetRef:0x");
                NUMCATH(buffer, netRef);
                StrCat (buffer, ", creatorId:0x");
                NUMCATH(buffer, creatorId);
                StrCat (buffer, ", instance:0x");
                NUMCATH(buffer, instance);
                StrCat (buffer, ", index:");
                NUMCATI(buffer, index);
                StrCat (buffer, ")");
            }

AGAIN:
            StrCat(buffer, "\n---\n");
            index++;
        } // while (NetLibIFGet(... と対応
NEXT_STEP:                                       
        // Default Gatewayの表示
        ipAddr = 0;
        size   = sizeof(ipAddr);
        ret = NetLibSettingGet(netRef, netSettingDefaultRouter, &ipAddr,&size);
        if (ret == errNone)
        {
            // Default Gatewayを表示する
            StrCat(buffer, "  Default Gateway:");
            MemSet(tempBuf, sizeof(tempBuf), 0x00);
            NetLibAddrINToA(netRef, ipAddr, tempBuf);
            StrCat(buffer, tempBuf);
        }                                    
#if DISPLAY_DETAIL_NETINFO
        // ドメイン名の表示
        MemSet(tempBuf, sizeof(tempBuf), 0x00);
        size   = sizeof(tempBuf);
        ret = NetLibSettingGet(netRef, netSettingDomainName, tempBuf, &size);
        if (ret == errNone)
        {
            // 表示する
            StrCat(buffer, "  Domain:");
            StrCat(buffer, tempBuf);
            StrCat(buffer, "\n");
        }                                    

        // Primary DNSの表示
        ipAddr = 0;
        size   = sizeof(ipAddr);
        ret = NetLibSettingGet(netRef, netSettingPrimaryDNS, &ipAddr, &size);
        if (ret == errNone)
        {
            // 表示する
            StrCat(buffer, "  Primary DNS:");
            MemSet(tempBuf, sizeof(tempBuf), 0x00);
            NetLibAddrINToA(netRef, ipAddr, tempBuf);
            StrCat(buffer, tempBuf);
            StrCat(buffer, "\n");
        }                                    

        // Secondary DNSの表示
        ipAddr = 0;
        size   = sizeof(ipAddr);
        ret = NetLibSettingGet(netRef, netSettingSecondaryDNS, &ipAddr, &size);
        if (ret == errNone)
        {
            // 表示する
            StrCat(buffer, "  Secondary DNS:");
            MemSet(tempBuf, sizeof(tempBuf), 0x00);
            NetLibAddrINToA(netRef, ipAddr, tempBuf);
            StrCat(buffer, tempBuf);
            StrCat(buffer, "\n");
        }
#endif
    }
    else
    {
        StrCat(buffer, "Line status: Unknown\n");
    }
    return;
}

/*=========================================================================*/
/*   Function :   NNshNet_TraceBitSet                                      */
/*                                               トレースビットを設定する  */
/*=========================================================================*/
void NNshNet_TraceBitSet(UInt16 netRef, UInt32 traceBits)
{
    // トレースビットを設定する
    if (netRef != 0)
    {
        (void) NetLibSettingSet(netRef, netSettingTraceBits,
                                 &traceBits, sizeof(UInt32));
    }
    return;
}

// そのうち使う、、かも？
#if 0
/*-------------------------------------------------------------------------*/
/*   Function :   nnshNet_calcIPcheckSum                                   */
/*                                              IPのチェックサムを計算する */
/*                                                                         */
/*                 - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*                  [計算方法]                                             */
/*                     パケット全体を2オクテットごとの16ビット列にして、   */
/*                     それぞれを1の補数として和を求め、それの1の補数を    */
/*                     チェックサムとする。ただし、チェックサムのエリアは  */
/*                     0として計算する。                                   */
/*                 - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*-------------------------------------------------------------------------*/
static UInt16 nnshNet_calcIPcheckSum(UInt16 *header, UInt16 length)
{
    UInt32 checkSum;
    UInt16 *ptr, answer, loop;
    UInt8  *odd;

    // ヘッダレングスが指定されていないときにはゼロを応答する
    if (length == 0)
    {
        return (0);
    }

    // 16ビット分のチェックサムを計算する
    checkSum = 0;
    ptr      = header;
    loop     = (length / 2);
    do
    {
        checkSum = checkSum + *ptr;
        ptr++;
        loop = loop - 1;
    } while (loop > 0);

    // あまりがある場合
    if ((length % 2) != 0)
    {
        odd = (UInt8 *)ptr;
        checkSum = checkSum + *odd;
    }


    // 上位１６ビットと下位１６ビットを足し合わせる
    loop     = (checkSum >> 16);
    checkSum = loop + (checkSum & 0xffff);
    checkSum = loop + checkSum;
    answer   = ~checkSum;
    return (answer);
}
#endif

// そのうち使うかも、、
#if 0
/*-------------------------------------------------------------------------*/
/*   Function :   setICMPmessage                                           */
/*                                              ICMPのタイプを文字列に変換 */
/*-------------------------------------------------------------------------*/
static void setICMPmessage(ICMP_packet *icp, Char *buffer)
{
    switch(icp->icmp_type)
    {
      case ICMP_ECHOREPLY:
        /* This is not used - we only call this for non-replies. */
        StrCopy(buffer, "Echo Reply (!?)");
        break;

      case ICMP_DEST_UNREACH:
        switch(icp->icmp_code)
        {
          case ICMP_NET_UNREACH:
            StrCopy(buffer, "Destination Net Unreachable");
            break;

          case ICMP_HOST_UNREACH:
            StrCopy(buffer, "Destination Host Unreachable");
            break;

          case ICMP_PROT_UNREACH:
            StrCopy(buffer, "Destination Protocol Unreachable");
            break;

          case ICMP_PORT_UNREACH:
            StrCopy(buffer, "Destination Port Unreachable");
            break;
            
          case ICMP_FRAG_NEEDED:
            StrCopy(buffer, "frag needed and DF set");
            break;

          case ICMP_SR_FAILED:
            StrCopy(buffer, "Source Route Failed");
            break;

          case ICMP_NET_UNKNOWN:
            StrCopy(buffer, "Network Unknown");
            break;

          case ICMP_HOST_UNKNOWN:
            StrCopy(buffer, "Host Unknown");
            break;

          case ICMP_HOST_ISOLATED:
            StrCopy(buffer, "Host Isolated");
            break;

          case ICMP_NET_UNR_TOS:
            StrCopy(buffer, "Destination Network Unreachable at this TOS");
            break;

          case ICMP_HOST_UNR_TOS:
            StrCopy(buffer, "Destination Host Unreachable at this TOS");
            break;

          case ICMP_PKT_FILTERED:
            StrCopy(buffer, "Packet Filtered");
            break;

          case ICMP_PREC_VIOLATION:
            StrCopy(buffer, "Precedence Violation");
            break;

          case ICMP_PREC_CUTOFF:
            StrCopy(buffer, "Precedence Cutoff");
            break;

          default:
            StrCopy(buffer, "Dest Unreachable, Unknown Code:0x");
            NUMCATH(buffer, icp->icmp_code);
            break;
        }
        /* Print returned IP header information */
        //pr_retip((struct ip *)icp->icmp_data);
        break;

      case ICMP_SOURCE_QUENCH:
        StrCopy(buffer, "Source Quench");
        break;

      case ICMP_REDIRECT:
        StrCopy(buffer, "Redirect");
        break;

      case ICMP_ECHO:
        StrCopy(buffer, "Echo Request");
        /* XXX ID + Seq + Data */
        break;

      case ICMP_TIME_EXCEEDED:
        StrCopy(buffer, "Time exceeded, Bad Code: 0x");
        NUMCATH(buffer, icp->icmp_code);
        // pr_retip((struct ip *)icp->icmp_data);
        break;
        
      case ICMP_PARAMETERPROB:
        StrCopy(buffer, "Parameter problem ");
        //pr_retip((struct ip *)icp->icmp_data);
        break;
        
      default:
        StrCopy(buffer, "Unknown ICMP type:0x");
        NUMCATH(buffer, icp->icmp_type);
        break;
    }
    return;
}
#endif

#ifdef USE_SSL
/*==========================================================================*/
/*  NNshSSL_open() : SSLライブラリのオープン                                */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_open(UInt16 sslMode, UInt16 *netRef,
                       UInt16 *sslRef, SslLib **template, SslContext **context)
{
    Err ret;

    // 変数の初期化
    *netRef   = 0;
    *sslRef   = 0;
    *template = NULL;
    *context  = NULL;

    // Netライブラリのオープン
    ret = NNshNet_open(netRef);
    if (ret != errNone)
    {
        *netRef = 0;
        return (ret);
    }
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        // SSLモードでない、折り返す
        return (ret);
    }

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        // SSL v2モード、折り返す
        return (NNshSSLv2_Open((NNshSSLv2Ref **) context));
    }
#endif

    // SSLライブラリのオープン
    if (SysLibFind(kSslDBName, sslRef) != 0)
    {
        if (SysLibLoad(kSslLibType, kSslLibCreator, sslRef) != 0)
        {
            // ロード失敗、エラー応答する
            *sslRef = 0;
            return (~errNone);
        }
    }
    ret = SslLibOpen(*sslRef);
    if (ret != errNone)
    {
        // SSLライブラリオープン失敗、、、
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // generic SSL contextの作成
    ret = SslLibCreate (*sslRef, template);
    if (ret != errNone)
    {
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // コンパチフラグのセット
    // SslLibSet_Compat(*sslRef, *template, sslCompatAll);

    // SSL contextの作成
    ret = SslContextCreate(*sslRef, *template, context);
    if (ret != errNone)
    {
        // generic SSL contextの廃棄
        (void) SslLibDestroy(*sslRef, *template);
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        *template = NULL;
        return (ret);
    }
    return (ret);
}

/*==========================================================================*/
/*  NNshSSL_close() : SSLライブラリのクローズ                               */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_close(UInt16 sslMode, UInt16 netRef,
                         UInt16 sslRef, SslLib *template, SslContext *context)
{

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        // SSL v2モード、折り返す
        NNshSSLv2_Close((NNshSSLv2Ref **) &context);
        NNshNet_close(netRef);
        return (errNone);
    }
#endif

    if ((sslRef == 0)||(sslMode == NNSH_SSLMODE_NOTSSL))
    {
        NNshNet_close(netRef);
        return (errNone);
    }

    if (context != NULL)
    {
        (void) SslContextDestroy(sslRef, context);
    }

    if (template != NULL)
    {
        (void) SslLibDestroy(sslRef, template);
    }
    (void) SslLibClose(sslRef);

    (void) NNshNet_close(netRef);
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_connect() : SSL接続                                             */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_connect(UInt16 sslMode, UInt16 sslRef, SslContext *context,
                    NetSocketRef *socketRef, UInt16 netRef,
                    Char *hostName, UInt16 port)
{
    Err ret;
    SslCallback verifyCallback;

    // TCP/IPでホストに接続する
    ret = NNshNet_connect(socketRef, netRef, hostName, port);
    if ((ret != errNone)||(sslMode == NNSH_SSLMODE_NOTSSL))
    {
        return (ret);
    }

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        // SSL v2モード、折り返す
        ret = NNshSSLv2_Connect((NNshSSLv2Ref *) context, *socketRef, netRef);
        return (ret);
    }
#endif

    // // Verifyコールバック関数を設定する
    MemSet(&verifyCallback, sizeof(SslCallback), 0x00);
    verifyCallback.callback = NNshSSL_callbackFunc;
    SslContextSet_VerifyCallback(sslRef, context, &verifyCallback);

    // ソケットをContextに設定、ホストに接続する
    //SslContextSet_Compat(sslRef, context, sslCompatAll);
    SslContextSet_Socket(sslRef, context, *socketRef);
    ret = SslOpen(sslRef, context,
                  ((sslOpenModeSsl)|(sslOpenUseDefaultTimeout)), 0);
#if 0
    ret = SslOpen(sslRef, context,
                  ((sslOpenNewConnection)|(sslOpenModeSsl)|
                   (sslOpenUseDefaultTimeout)), 0);
#endif
    return (ret);
}

/*==========================================================================*/
/*  NNshSSL_disconnect() : SSL切断                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_disconnect(UInt16 sslMode, UInt16 sslRef, SslContext *context,
                       NetSocketRef socketRef, UInt16 netRef)
{
    Err ret;
#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        NNshSSLv2_Disconnect((NNshSSLv2Ref *) context);
    }
    else
#endif
    if (sslMode != NNSH_SSLMODE_NOTSSL)
    {
        ret = SslClose(sslRef, context,
                           sslCloseUseDefaultTimeout, (NNshGlobal->NNsiParam)->netTimeout);
    }
    ret = NNshNet_disconnect(netRef, socketRef);

    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_write() : データ送信(SSL)                                       */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_write(UInt16 sslMode, UInt16  netRef, NetSocketRef socketRef,
                  UInt16 sslRef, SslContext *context, UInt16 size, void *data)
{
    Err   err;
    Int16 ret;

    // SSLモードでなければ、通常の書き込み処理を行う
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        return (NNshNet_write(netRef, socketRef, size, data));
    }

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        return (NNshSSLv2_Write((NNshSSLv2Ref *) context, size, data));
    }
#endif

    // データを書き込む
    ret = SslWrite(sslRef, context, data, size, &err);
    if (ret  == -1)
    {
        return (err);
    }
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_read() : データ受信(SSL)                                        */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_read(UInt16 sslMode, UInt16  netRef, NetSocketRef socketRef,
                 UInt16 sslRef, SslContext *context,
                 UInt16 size, void *data, UInt16 *readSize)
{
    Err    err;
    Int16  ret;

    // SSLモードでなければ、通常の読み出し処理を行う
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        return (NNshNet_read(netRef, socketRef, size, data, readSize));
    }

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        return (NNshSSLv2_Read((NNshSSLv2Ref *) context, size, data, readSize));
    }
#endif
    
    // データを読み込む
    ret = SslRead(sslRef, context, data, size, &err);
    if (ret == -1)
    {
        return (err);
    }
    *readSize = ret;
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_flush() : 内部データバッファのクリア(SSL)                       */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_flush(UInt16 sslMode, UInt16  sslRef, SslContext *context)
{

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        return (errNone);
    }
#endif

    if (sslMode != NNSH_SSLMODE_NOTSSL)
    {
        // 内部データバッファのクリア指示
        SslContextSet_Mode(sslRef, context, (sslModeFlush));
    }
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_callbackFunc() : SSLのコールバック関数                          */
/*                                                                          */
/*==========================================================================*/
Int32 NNshSSL_callbackFunc(SslCallback *callbackStruct, Int32 command,
                           Int32 flavor, void *info)
{
    SslVerify *verifyData;

    switch (command)
    {
      case sslCmdVerify:
        // 'verify callback'
        verifyData = (SslVerify *) info;
        // この関数の後ろで吟味する
        break;

      case sslCmdInfo:
        NNsh_DebugMessage(ALTID_INFO, "NNshSSL_callback", " Info:", command);
        return (errNone);
        break;
      case sslCmdNew:
        NNsh_DebugMessage(ALTID_INFO, "NNshSSL_callback", " New:", command);
        return (errNone);
        break;
      case sslCmdReset:
        NNsh_DebugMessage(ALTID_INFO, "NNshSSL_callback", " Reset:", command);
        return (errNone);
        break;

      case sslCmdFree:
        NNsh_DebugMessage(ALTID_INFO, "NNshSSL_callback", " Free:", command);
        return (errNone);
        break;

      default:
        NNsh_DebugMessage(ALTID_INFO, "NNshSSL_callback", " cmd:", command);
        return (errNone);
        break;
    }

    // Verify(certificate)
    NNsh_DebugMessage(ALTID_INFO, "NNshSSL_callback", " code:", flavor);
    switch (flavor)
    {
      case sslErrBadDecode:
        // うまく証明書がデコードできなかった
      case sslErrCert:
        // 一般的な証明エラー
      case sslErrCertDecodeError:
        // 証明書のデコードに失敗した
      case sslErrUnsupportedCertType:
        // サーバがわけのわからん（サポートしていない）証明書を送ってきた
      case sslErrUnsupportedSignatureType:
        // サーバがわけのわからんシグネチャを送ってきた
      case sslErrVerifyBadSignature:
        // 証明書のシグネチャが無効
      case sslErrVerifyNoTrustedRoot:
        // 証明書がない
      case sslErrVerifyNotAfter:
        // 証明書の有効期限が切れている
      case sslErrVerifyNotBefore:
        // 証明書が未来の日付になっている
      case sslErrVerifyConstraintViolation:
        // 証明書がX509の拡張に違反している
      case sslErrVerifyUnknownCriticalExtension:
        // X509拡張が証明書チェックルーチンで解釈できなかった
      case sslErrOk:
      default:
        // NULL
        break;
    }
    return (errNone);
}
#endif  // #ifdef USE_SSL
