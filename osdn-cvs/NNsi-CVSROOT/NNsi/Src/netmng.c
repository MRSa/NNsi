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

    timeout = (NNshGlobal->NNsiParam)->netTimeout;

    // �ʏ��NetLib���ƁA�ʐM��A�h���X�̎w��͕s�v����...
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

    timeout = (NNshGlobal->NNsiParam)->netTimeout;

    *readSize = 0;
    // �ʏ��NetLib���ƒʐM��A�h���X�̎w��͕s�v����...
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

    timeout = (NNshGlobal->NNsiParam)->netTimeout;

    (void) NetLibSocketShutdown(netRef, socketRef, netSocketDirBoth,
                                timeout, &err);
    (void) NetLibSocketClose(netRef, socketRef, timeout, &err);

    return (errNone);
}

/*==========================================================================*/
/*  net_connect() : �z�X�g�ɐڑ�                                            */
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

    // �����ڑ������X�V(�ǉ�...)
    err = NetLibConnectionRefresh(netRef, true,  &openFlag, &errInfo);
    if (err != errNone)
    {
        // �ڑ����̍X�V�Ɏ��s
        NNsh_DebugMessage(ALTID_ERROR, "NetLibConnectionRefresh()", " ", err);
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass,
                          "NetLibConnectionRefresh(): flag:%hd, info:%hu/0x%hx",
                          (UInt16) openFlag, errInfo, errInfo);
#endif  // #ifdef USE_REPORTER
        // �I�����Ȃ�...
    }
#endif  // #ifdef SET_CONNECT_REFRESH

    // �o�b�t�@�̈���m��
    infoBuff = MEMALLOC_PTR(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        return (~errNone);
    }
    MemSet(infoBuff, sizeof(NetHostInfoBufType), 0x00);
    timeout = (NNshGlobal->NNsiParam)->netTimeout;

    // �z�X�g������IP�A�h���X���擾
    hostIP = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);
    if (hostIP == 0)
    {
        // �z�X�gIP�̎擾���s
        NNsh_DebugMessage(ALTID_ERROR,"NetLibGetHostByName:", hostName, err);
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "HOST :%s => IP:0x%lx",
                      hostName, NetHToNS(infoBuff->address[0]));
    HostTraceOutputB(appErrorClass, infoBuff, sizeof(NetHostInfoBufType));
#endif // #ifdef USE_REPORTER

    // �\�P�b�gOPEN
    *socketRef = NetLibSocketOpen(netRef, netSocketAddrINET, 
                                  netSocketTypeStream, netSocketProtoIPTCP,
                                  timeout, &err);
    if (*socketRef == -1)
    {
        // �\�P�b�gOPEN�ُ�
        *socketRef = 0;
        NNsh_DebugMessage(ALTID_ERROR,"NetLibSocketOpen()", "", err);
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

#ifdef SET_SOCKET_OPTION
    // �\�P�b�g�I�v�V�����̐ݒ�...
    flag = true;
    ret  = NetLibSocketOptionSet(netRef, *socketRef, 
                                 netSocketOptLevelSocket,
                                 netSocketOptSockNonBlocking,
                                 &flag, sizeof(flag), 
                                 timeout, &err);
    if (ret == -1)
    {
        // �\�P�b�g�I�v�V�����ݒ莸�s
        NNsh_DebugMessage(ALTID_ERROR,
                          "NetLibSocketOptionSet()", 
                          "netSocketOptSockNonBlocking == TRUE", err);
        // ������...
    }

    // �\�P�b�g�I�v�V�����̐ݒ�...
    flag = true;
    ret  = NetLibSocketOptionSet(netRef, *socketRef, 
                                 netSocketOptLevelTCP,
                                 netSocketOptTCPNoDelay,
                                 &flag, sizeof(flag), 
                                 timeout, &err);
    if (ret == -1)
    {
        // �\�P�b�g�I�v�V�����ݒ莸�s
        NNsh_DebugMessage(ALTID_ERROR,
                          "NetLibSocketOptionSet()", 
                          "netSocketOptTCPNoDelay == TRUE", err);
        // ������...
    }
#endif // #ifdef SET_SOCKET_OPTION

    // �\�P�b�g�R�l�N�g
    MemSet(&addrBuf, sizeof(NetSocketAddrType), 0x00);
    addrType = (NetSocketAddrINType *) &addrBuf;
    addrType->family = netSocketAddrINET;
    addrType->port   = NetHToNS(port);
    //addrType->addr  = NetHToNS(infoBuff->address[0]);
    MemMove(&(addrType->addr), *(hostIP->addrListP), hostIP->addrLen); 
    addrLen         = sizeof(NetSocketAddrType);
    err             = errNone;

    // NetLib�̏ꍇ�ɂ́ANetLibSocketConnect()���g�p����
#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "CONNECTING TO %s(IP:0x%lx), port:%d ",
                      hostName, NetHToNS(addrType->addr), addrType->port);
#endif // #ifdef USE_REPORTE

    ret = NetLibSocketConnect(netRef, *socketRef,
                              (NetSocketAddrType *) &addrBuf,
                              addrLen, timeout, &err);
    if (ret == -1)
    {
        // �\�P�b�g�R�l�N�g�ُ�
        NNsh_DebugMessage(ALTID_ERROR,"NetLibSocketConnect()", "", err);
        (void) NetLibSocketClose(netRef, *socketRef, timeout, &err);
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }
    err = errNone;

FUNC_END:
    // �o�b�t�@�̈���J��
    MEMFREE_PTR(infoBuff);

    return (err);
}

/*=========================================================================*/
/*   Function :   NNshNet_CheckHostAlive                                   */
/*                                               �z�X�g�Ƃ̐ڑ����m�F����  */
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

    // ����ڑ����łȂ���΁A�������Ȃ�
    if (netRef == 0)
    {
        return;
    }

    // �o�b�t�@�̈���m��
    infoBuff = MEMALLOC_PTR(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        StrCat(resultMsg, "\n(MemAlloc Fail...)");
        return;
    }

    // �z�X�g������IP�A�h���X���擾
    timeout = (NNshGlobal->NNsiParam)->netTimeout;
    MemSet(infoBuff, sizeof(NetHostInfoBufType), 0x00);
    hostIP  = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);

    // IP�A�h���X�𕶎���ɕϊ�����
    MemSet(tempBuf, sizeof(tempBuf), 0x00);
    if (hostIP != 0)
    {
        if (infoBuff->address[0] != 0)
        {
            NetLibAddrINToA(netRef, infoBuff->address[0], tempBuf);
        }
        else
        {
            // �G���[�\������
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
        // �G���[�����A�������ʕ\������
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
        // �z�X�g������擾����IP�A�h���X��\��
        StrCat(resultMsg, "Host : ");
        StrCat(resultMsg, hostName);
        StrCat(resultMsg, "(");
        StrCat(resultMsg, tempBuf);
        StrCat(resultMsg, ")\n---\n");
    }

    // �z�X�g�Ƃ̐ڑ��m�F(HTTP�T�[�o�Ƃ̐ڑ��m�F)
    MemSet(tempBuf, sizeof(tempBuf), 0x00);
    StrCopy(tempBuf, "http://");
    StrCat (tempBuf, hostName);
    StrCat (tempBuf, "/index.html");    
    err = NNshHttp_comm(HTTP_SENDTYPE_GET, tempBuf, NULL, NULL,
                        HTTP_RANGE_OMIT, sizeof(tempBuf), NULL);
    if (err != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (err != netErrTimeout)
        {
            StrCat (resultMsg, "Communication Error ret:0x");
            NUMCATH(resultMsg, err);
        }
        else
        {
            // �ʐM�^�C���A�E�g����(��M���Ă���Ƃ���܂ŁA�����I�ɉ��߂��s��)
            StrCat (resultMsg, "Connection Timeout ret:0x");
            NUMCATH(resultMsg, err);
        }
    }
    else
    {    
        StrCat (resultMsg, "Response: ");
        
        // �ʐM����! �w�b�_������\������
        OpenFile_NNsh(FILE_RECVMSG, 
                      (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                      &fileRef);
        err = ReadFile_NNsh(&fileRef, 0, sizeof(tempBuf), tempBuf, &dmmy);
        CloseFile_NNsh(&fileRef);
        if ((err != errNone)&&(err != vfsErrFileEOF)&&(err != fileErrEOF))
        {        
            // �t�@�C���ǂ݂��݃G���[
            StrCat (resultMsg, "Read Error ret:0x");
            NUMCATH(resultMsg, err);
        }
        else
        {
            // �w�b�_������\������
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
/*                                                       IF�����擾����  */
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

    // �Ƃ肠�����A��؂�������Ă���
    StrCat(buffer, "--- NetLib Information ---\n");
    
    // �������\������B
    if (netRef != 0)
    {
        // ���z�X�g�����擾���ĕ\������
        // �l�b�g���[�N�����擾����
        index = 0;
        ret = errNone;
        while (ret == errNone)
        {
            // �l�b�g���[�N�̃C���X�^���X���擾����
            ret = NetLibIFGet(netRef, index, &creatorId, &instance);
            switch (ret)
            {
              case errNone:
               // �C���X�^���X�̎擾�ɐ���
               break;

              case netErrInterfaceNotFound:
              case netErrInvalidInterface:
              case netErrTooManyInterfaces:
                // �I��
                goto NEXT_STEP;
                break;

              case netErrPrefNotFound:
              default:
                // �G���[����...
                StrCat (buffer, "  NetLibIFGet():0x");
                NUMCATH(buffer, ret);
                StrCat (buffer, ", index:");
                NUMCATI(buffer, index);
                ret = errNone;
                goto AGAIN;
                break;
            }

            // �C���^�t�F�[�X�����擾����
            MemSet(&cmdParam, sizeof(cmdParam), 0x00);
            cmdParam.param.interfaceInfo.index = index;
            ret = NetLibMaster(netRef, netMasterInterfaceInfo,
                               &cmdParam, 1000);
            if (ret == errNone)
            {
                // �C���^�t�F�[�X���ƃh���C�o����\������
                StrCat(buffer, "  ");
                StrCat(buffer, cmdParam.param.interfaceInfo.ifName);
                StrCat(buffer, "(");
                StrCat(buffer, cmdParam.param.interfaceInfo.drvrName);
                StrCat(buffer, "): ");
                StrCat(buffer, cmdParam.param.interfaceInfo.hwName);

                if (cmdParam.param.interfaceInfo.ifUp != false)
                {
                    // IP�A�h���X����z�X�g�����擾
                    ipAddr  = cmdParam.param.interfaceInfo.ipAddr;
                    timeout = (NNshGlobal->NNsiParam)->netTimeout;
                    MemSet(&infoBuff, sizeof(infoBuff), 0x00);
                    hostIP = NetLibGetHostByAddr(netRef, (UInt8 *)&ipAddr,
                                                 sizeof(UInt32),
                                                 netSocketAddrINET, &infoBuff,
                                                 timeout, &ret);
                    if (hostIP != 0)
                    {
                        // �z�X�g����\��
                        StrCat(buffer, "\n    Name:");
                        StrCat(buffer, infoBuff.hostInfo.nameP);
                    }

                    // IP�A�h���X
                    StrCat(buffer, "\n    IP Addr:");
                    MemSet(tempBuf, sizeof(tempBuf), 0x00);
                    NetLibAddrINToA(netRef, 
                                    cmdParam.param.interfaceInfo.ipAddr,
                                    tempBuf);
                    StrCat(buffer, tempBuf);

                    // �ʐM���x
                    StrCat(buffer, "\n    speed: ");
                    NUMCATI(buffer, cmdParam.param.interfaceInfo.speed);
                    StrCat(buffer, "bps");
#if DISPLAY_DETAIL_NETINFO
                    // �T�u�l�b�g�}�X�N
                    StrCat(buffer, "\n    mask: ");
                    MemSet(tempBuf, sizeof(tempBuf), 0x00);
                    NetLibAddrINToA(netRef, 
                                    cmdParam.param.interfaceInfo.subnetMask,
                                    tempBuf);
                    StrCat(buffer, tempBuf);

                    // �u���[�h�L���X�g
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
                    // I/F ���I�t�̂Ƃ�
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
        } // while (NetLibIFGet(... �ƑΉ�
NEXT_STEP:                                       
        // Default Gateway�̕\��
        ipAddr = 0;
        size   = sizeof(ipAddr);
        ret = NetLibSettingGet(netRef, netSettingDefaultRouter, &ipAddr,&size);
        if (ret == errNone)
        {
            // Default Gateway��\������
            StrCat(buffer, "  Default Gateway:");
            MemSet(tempBuf, sizeof(tempBuf), 0x00);
            NetLibAddrINToA(netRef, ipAddr, tempBuf);
            StrCat(buffer, tempBuf);
        }                                    
#if DISPLAY_DETAIL_NETINFO
        // �h���C�����̕\��
        MemSet(tempBuf, sizeof(tempBuf), 0x00);
        size   = sizeof(tempBuf);
        ret = NetLibSettingGet(netRef, netSettingDomainName, tempBuf, &size);
        if (ret == errNone)
        {
            // �\������
            StrCat(buffer, "  Domain:");
            StrCat(buffer, tempBuf);
            StrCat(buffer, "\n");
        }                                    

        // Primary DNS�̕\��
        ipAddr = 0;
        size   = sizeof(ipAddr);
        ret = NetLibSettingGet(netRef, netSettingPrimaryDNS, &ipAddr, &size);
        if (ret == errNone)
        {
            // �\������
            StrCat(buffer, "  Primary DNS:");
            MemSet(tempBuf, sizeof(tempBuf), 0x00);
            NetLibAddrINToA(netRef, ipAddr, tempBuf);
            StrCat(buffer, tempBuf);
            StrCat(buffer, "\n");
        }                                    

        // Secondary DNS�̕\��
        ipAddr = 0;
        size   = sizeof(ipAddr);
        ret = NetLibSettingGet(netRef, netSettingSecondaryDNS, &ipAddr, &size);
        if (ret == errNone)
        {
            // �\������
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
/*                                               �g���[�X�r�b�g��ݒ肷��  */
/*=========================================================================*/
void NNshNet_TraceBitSet(UInt16 netRef, UInt32 traceBits)
{
    // �g���[�X�r�b�g��ݒ肷��
    if (netRef != 0)
    {
        (void) NetLibSettingSet(netRef, netSettingTraceBits,
                                 &traceBits, sizeof(UInt32));
    }
    return;
}

// ���̂����g���A�A�����H
#if 0
/*-------------------------------------------------------------------------*/
/*   Function :   nnshNet_calcIPcheckSum                                   */
/*                                              IP�̃`�F�b�N�T�����v�Z���� */
/*                                                                         */
/*                 - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*                  [�v�Z���@]                                             */
/*                     �p�P�b�g�S�̂�2�I�N�e�b�g���Ƃ�16�r�b�g��ɂ��āA   */
/*                     ���ꂼ���1�̕␔�Ƃ��Ęa�����߁A�����1�̕␔��    */
/*                     �`�F�b�N�T���Ƃ���B�������A�`�F�b�N�T���̃G���A��  */
/*                     0�Ƃ��Čv�Z����B                                   */
/*                 - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/*-------------------------------------------------------------------------*/
static UInt16 nnshNet_calcIPcheckSum(UInt16 *header, UInt16 length)
{
    UInt32 checkSum;
    UInt16 *ptr, answer, loop;
    UInt8  *odd;

    // �w�b�_�����O�X���w�肳��Ă��Ȃ��Ƃ��ɂ̓[������������
    if (length == 0)
    {
        return (0);
    }

    // 16�r�b�g���̃`�F�b�N�T�����v�Z����
    checkSum = 0;
    ptr      = header;
    loop     = (length / 2);
    do
    {
        checkSum = checkSum + *ptr;
        ptr++;
        loop = loop - 1;
    } while (loop > 0);

    // ���܂肪����ꍇ
    if ((length % 2) != 0)
    {
        odd = (UInt8 *)ptr;
        checkSum = checkSum + *odd;
    }


    // ��ʂP�U�r�b�g�Ɖ��ʂP�U�r�b�g�𑫂����킹��
    loop     = (checkSum >> 16);
    checkSum = loop + (checkSum & 0xffff);
    checkSum = loop + checkSum;
    answer   = ~checkSum;
    return (answer);
}
#endif

// ���̂����g�������A�A
#if 0
/*-------------------------------------------------------------------------*/
/*   Function :   setICMPmessage                                           */
/*                                              ICMP�̃^�C�v�𕶎���ɕϊ� */
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
/*  NNshSSL_open() : SSL���C�u�����̃I�[�v��                                */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_open(UInt16 sslMode, UInt16 *netRef,
                       UInt16 *sslRef, SslLib **template, SslContext **context)
{
    Err ret;

    // �ϐ��̏�����
    *netRef   = 0;
    *sslRef   = 0;
    *template = NULL;
    *context  = NULL;

    // Net���C�u�����̃I�[�v��
    ret = NNshNet_open(netRef);
    if (ret != errNone)
    {
        *netRef = 0;
        return (ret);
    }
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        // SSL���[�h�łȂ��A�܂�Ԃ�
        return (ret);
    }

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        // SSL v2���[�h�A�܂�Ԃ�
        return (NNshSSLv2_Open((NNshSSLv2Ref **) context));
    }
#endif

    // SSL���C�u�����̃I�[�v��
    if (SysLibFind(kSslDBName, sslRef) != 0)
    {
        if (SysLibLoad(kSslLibType, kSslLibCreator, sslRef) != 0)
        {
            // ���[�h���s�A�G���[��������
            *sslRef = 0;
            return (~errNone);
        }
    }
    ret = SslLibOpen(*sslRef);
    if (ret != errNone)
    {
        // SSL���C�u�����I�[�v�����s�A�A�A
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // generic SSL context�̍쐬
    ret = SslLibCreate (*sslRef, template);
    if (ret != errNone)
    {
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // �R���p�`�t���O�̃Z�b�g
    // SslLibSet_Compat(*sslRef, *template, sslCompatAll);

    // SSL context�̍쐬
    ret = SslContextCreate(*sslRef, *template, context);
    if (ret != errNone)
    {
        // generic SSL context�̔p��
        (void) SslLibDestroy(*sslRef, *template);
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        *template = NULL;
        return (ret);
    }
    return (ret);
}

/*==========================================================================*/
/*  NNshSSL_close() : SSL���C�u�����̃N���[�Y                               */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_close(UInt16 sslMode, UInt16 netRef,
                         UInt16 sslRef, SslLib *template, SslContext *context)
{

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        // SSL v2���[�h�A�܂�Ԃ�
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
/*  NNshSSL_connect() : SSL�ڑ�                                             */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_connect(UInt16 sslMode, UInt16 sslRef, SslContext *context,
                    NetSocketRef *socketRef, UInt16 netRef,
                    Char *hostName, UInt16 port)
{
    Err ret;
    SslCallback verifyCallback;

    // TCP/IP�Ńz�X�g�ɐڑ�����
    ret = NNshNet_connect(socketRef, netRef, hostName, port);
    if ((ret != errNone)||(sslMode == NNSH_SSLMODE_NOTSSL))
    {
        return (ret);
    }

#ifdef USE_SSL_V2
    if (sslMode == NNSH_SSLMODE_SSL_V2SPECIAL)
    {
        // SSL v2���[�h�A�܂�Ԃ�
        ret = NNshSSLv2_Connect((NNshSSLv2Ref *) context, *socketRef, netRef);
        return (ret);
    }
#endif

    // // Verify�R�[���o�b�N�֐���ݒ肷��
    MemSet(&verifyCallback, sizeof(SslCallback), 0x00);
    verifyCallback.callback = NNshSSL_callbackFunc;
    SslContextSet_VerifyCallback(sslRef, context, &verifyCallback);

    // �\�P�b�g��Context�ɐݒ�A�z�X�g�ɐڑ�����
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
/*  NNshSSL_disconnect() : SSL�ؒf                                          */
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
/*  NNshSSL_write() : �f�[�^���M(SSL)                                       */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_write(UInt16 sslMode, UInt16  netRef, NetSocketRef socketRef,
                  UInt16 sslRef, SslContext *context, UInt16 size, void *data)
{
    Err   err;
    Int16 ret;

    // SSL���[�h�łȂ���΁A�ʏ�̏������ݏ������s��
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

    // �f�[�^����������
    ret = SslWrite(sslRef, context, data, size, &err);
    if (ret  == -1)
    {
        return (err);
    }
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_read() : �f�[�^��M(SSL)                                        */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_read(UInt16 sslMode, UInt16  netRef, NetSocketRef socketRef,
                 UInt16 sslRef, SslContext *context,
                 UInt16 size, void *data, UInt16 *readSize)
{
    Err    err;
    Int16  ret;

    // SSL���[�h�łȂ���΁A�ʏ�̓ǂݏo���������s��
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
    
    // �f�[�^��ǂݍ���
    ret = SslRead(sslRef, context, data, size, &err);
    if (ret == -1)
    {
        return (err);
    }
    *readSize = ret;
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_flush() : �����f�[�^�o�b�t�@�̃N���A(SSL)                       */
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
        // �����f�[�^�o�b�t�@�̃N���A�w��
        SslContextSet_Mode(sslRef, context, (sslModeFlush));
    }
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_callbackFunc() : SSL�̃R�[���o�b�N�֐�                          */
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
        // ���̊֐��̌��ŋᖡ����
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
        // ���܂��ؖ������f�R�[�h�ł��Ȃ�����
      case sslErrCert:
        // ��ʓI�ȏؖ��G���[
      case sslErrCertDecodeError:
        // �ؖ����̃f�R�[�h�Ɏ��s����
      case sslErrUnsupportedCertType:
        // �T�[�o���킯�̂킩���i�T�|�[�g���Ă��Ȃ��j�ؖ����𑗂��Ă���
      case sslErrUnsupportedSignatureType:
        // �T�[�o���킯�̂킩���V�O�l�`���𑗂��Ă���
      case sslErrVerifyBadSignature:
        // �ؖ����̃V�O�l�`��������
      case sslErrVerifyNoTrustedRoot:
        // �ؖ������Ȃ�
      case sslErrVerifyNotAfter:
        // �ؖ����̗L���������؂�Ă���
      case sslErrVerifyNotBefore:
        // �ؖ����������̓��t�ɂȂ��Ă���
      case sslErrVerifyConstraintViolation:
        // �ؖ�����X509�̊g���Ɉᔽ���Ă���
      case sslErrVerifyUnknownCriticalExtension:
        // X509�g�����ؖ����`�F�b�N���[�`���ŉ��߂ł��Ȃ�����
      case sslErrOk:
      default:
        // NULL
        break;
    }
    return (errNone);
}
#endif  // #ifdef USE_SSL
