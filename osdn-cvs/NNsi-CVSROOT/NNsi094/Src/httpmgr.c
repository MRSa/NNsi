/*============================================================================*
 *  FILE: 
 *     httpMgr.c
 *
 *  Description: 
 *     HTTP�R�~���j�P�[�V�����}�l�[�W��
 *===========================================================================*/
#define  HTTPMGR_C
#include "local.h"

/*-------------------------------------------------------------------------*/
/*   Function :   http_divideHostName                                      */
/*                                               URL����z�X�g���𒊏o���� */
/*-------------------------------------------------------------------------*/
static void NNshHttp_divideHostName(Char *url, Char *host, Char **loc)
{
    Char *start;

    // �z�X�g���̍ŏ��ƍŌ�̏ꏊ�����߂�("://" �� "/" �Ɉ͂܂�Ă��钆�g)
    start = StrStr(url, "://");
    start = start + StrLen("://");
#ifdef USE_STRSTR
    *loc  = StrStr(start, "/");
    // "/" ���Ȃ������ꍇ�ɂ́A�s�̍Ōォ��R�s�[����
    if (*loc == NULL)
#else
    *loc = start;
    while ((*(*loc) != '/')&&(*(*loc) != '\0'))
    {
        (*loc)++;
    }
    if (*(*loc) == '\0')
#endif
    {
        *loc = start + StrLen(url);
        StrNCopy(host, start, StrLen(url));
    }
    else
    {
        // �z�X�g���𒊏o����
        StrNCopy(host, start, (*loc - start));
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   http_createPostMsg                                       */
/*                                            HTTP���M���b�Z�[�W���쐬���� */
/*-------------------------------------------------------------------------*/
static Err NNshHttp_createPostMsg(UInt16 type, Char *url, Char *host,
                                  Char *cookie, Char *appendData,
                                  Char *buffer, UInt32 bufSize)
{
    NNshFileRef sendRef;
    UInt32      dataSize, readSize;
    Char       *loc, *buf;
    Err         ret;

    // �t�@�C�����瑗�M���郁�b�Z�[�W��ǂݏo���B
    dataSize = 0;
    readSize = 0;
    buf      = NULL;

    // ���M�t�@�C������f�[�^�擾
    ret = OpenFile_NNsh(FILE_SENDMSG, 
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &sendRef);
    if (ret != errNone)
    {
        return (ret);
    }
    (void) GetFileSize_NNsh(&sendRef, &readSize);
    buf = (Char *) MemPtrNew(readSize + 8);
    if (buf == NULL)
    {
        return (~errNone);
    }
    MemSet(buf, (readSize + 8), 0x00);
    (void) ReadFile_NNsh (&sendRef, 0, readSize, buf, &dataSize);
    (void) CloseFile_NNsh(&sendRef);

    // URL���z�X�g���ƃt�@�C���ɕ���
    loc = NULL;
    NNshHttp_divideHostName(url, host, &loc);

    // �t���f�[�^�T�C�Y���m�F����
    if (appendData != NULL)
    {
        readSize = StrLen(appendData);
    }
    else
    {
        readSize = 0;
    }

    // HTTP�̃w�b�_�쐬(�܂�BBS�Ή�������)
    if (type == HTTP_SENDTYPE_POST_MACHIBBS)
    {
        StrCopy(buffer, "POST /bbs/write.cgi HTTP/1.0\r\nHost: ");
    }
    else
    {
        StrCopy(buffer, "POST /test/bbs.cgi HTTP/1.0\r\nHost: ");
    }
    StrCat (buffer,  host);
    StrCat (buffer,  "\r\nAccept: text/html, */*\r\nReferer: ");
    StrCat (buffer,  url);
    StrCat (buffer,  "\r\nUser-Agent: Monazilla/1.00  NNsi/1.1" SOFT_REVISION
            "\r\nContent-Length: ");
    StrIToA(&buffer[StrLen(buffer)], (dataSize + readSize + 2));
    StrCat (buffer, "\r\nPragma: no-cache"
            "\r\nCookie: NAME=����������; Mail=");
    if (cookie != NULL)
    {
        StrCat (buffer, "; ");
        StrCat (buffer, cookie);
    }
    StrCat (buffer, "\r\n\r\n");

    // �f�[�^�o�b�t�@�ɃR�s�[������邩�m�F
    if (bufSize >= (StrLen(buffer) + dataSize + readSize + 2))
    {
        // �f�[�^�̃R�s�[
        StrCat (buffer, buf);
        if (appendData != NULL)
        {
            StrCat (buffer, appendData);
        }
        StrCat (buffer, "\r\n");
        ret = errNone;
    }
    else
    {
        // �o�b�t�@�T�C�Y�I�[�o�t���[
        ret = (~errNone - 1);
    }
    MemPtrFree(buf);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   http_createGetMsg                                        */
/*                                            HTTP���M���b�Z�[�W���쐬���� */
/*-------------------------------------------------------------------------*/
static void NNshHttp_createGetMsg(Char *url, Char *host, UInt32 getRange,
                                  UInt32 endRange, Char *buffer, Char *proxy)
{
    Char *loc;

    // URL���z�X�g���ƃt�@�C���ɕ���
    loc = NULL;
    NNshHttp_divideHostName(url, host, &loc);
    if (loc == NULL)
    {
        loc = "/";
    }

    // �f�[�^���M�p�o�b�t�@�Ƀ��b�Z�[�W���L�ڂ���
    StrCopy(buffer, "GET ");
    if ((proxy != NULL)&&(*proxy != '\0'))
    {
        StrCat (buffer,  url);
    }
    else
    {
        StrCat (buffer,  loc);
    }
    StrCat (buffer,  " HTTP/1.1\r\nHOST: ");
    StrCat (buffer,  host);
    StrCat (buffer,  "\r\nACCEPT: text/html, */*\r\nUser-Agent: "
           "Monazilla/1.00  NNsi/1.1(" SOFT_REVISION
           ")\r\nACCEPT-ENCODING: identity"
           "\r\nACCEPT-LANGUAGE: ja, en");
    if (getRange != HTTP_RANGE_OMIT)
    {
        StrCat(buffer, "\r\nRANGE: bytes=");
        StrIToA(&buffer[StrLen(buffer)], getRange);
        StrCat(buffer, "-");
    }
    if (endRange != HTTP_RANGE_OMIT)
    {
        StrIToA(&buffer[StrLen(buffer)], endRange);
    }
    StrCat(buffer, "\r\nConnection: close");
    StrCat(buffer, "\r\nPRAGMA: no-cache\r\n\r\n");

    return;
}

/*=========================================================================*/
/*   Function :   NNshHttp_commMain                                        */
/*                                                 HTTP�f�[�^�]��(GET)�v�� */
/*                                            (��M�f�[�^��recv.log�ɋL�^) */
/*=========================================================================*/
Err NNshHttp_commMain(UInt16 type, Char *url, Char *cookie, Char *appendData,
                      UInt32 range, UInt32 endRange, UInt16 port, Char *proxy)
{
#ifdef OFFLINE_DEBUG

    Char logMes[BUFSIZE];

    // ���O�����o�͂��āA�ʐM�����ӂ������B
    MemSet (logMes, sizeof(logMes), 0x00);
    StrCopy(logMes, "\n<< This is OFFLINE DEBUG, NOT CONNECT >>\n");
    if (range != HTTP_RANGE_OMIT)
    {
        StrCat (logMes, "Range:");
        NUMCATI(logMes, range);
        StrCat (logMes, "-");
    }
    if (endRange != HTTP_RANGE_OMIT)
    {
        NUMCATI(logMes, endRange);
    }
    StrCat (logMes, " PORT:");
    NUMCATI(logMes, port);

    if ((proxy != NULL)&&(*proxy != '\0'))
    {
       StrCat (logMes, " (proxy:");
       StrCat (logMes, proxy);
       StrCat (logMes, ")");
    }
    NNsh_InformMessage(ALTID_INFO, url, logMes, 0);

    return (errNone);
#else   // #ifdef OFFLINE_DEBUG

    Err             ret, err;
    Char           *bufPtr, host[MAX_URL], logBuf[MINIBUF];
    UInt16          dataSize, autoOffTime;
    UInt32          writeSize, totalSize;
    NetSocketRef    socketRef;
    NNshFileRef     fileRef;
    FormType       *frm;

    // (�ꎞ)����M�o�b�t�@�̊m��
    bufPtr = MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (bufPtr == NULL)
    {
        // �̈�̊m�ێ��s�I
        NNsh_DebugMessage(ALTID_ERROR, "HTTP(temp)", "MemPtrNew(), size:",
                          NNshParam->bufferSize + MARGIN);
        return (~errNone);
    }
    MemSet(bufPtr, (NNshParam->bufferSize + MARGIN), 0x00);

    // ���M�p���b�Z�[�W�̍쐬�Ƃt�q�k����z�X�g�����擾
    MemSet(host, sizeof(host), 0x00);
    if ((type == HTTP_SENDTYPE_POST)||(type == HTTP_SENDTYPE_POST_MACHIBBS))
    {
        ret = NNshHttp_createPostMsg(type, url, host, cookie, appendData, 
                                     bufPtr, NNshParam->bufferSize);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "createPostMsg()", "", ret);
            MemPtrFree(bufPtr);
            return (ret);
        }
    }
    else
    {
        // HTTP_SENDTYPE_GET
        NNshHttp_createGetMsg(url, host, range, endRange, bufPtr, proxy);
    }

    // Proxy���ݒ肳��Ă���΁A�擾�����z�X�g����override
    if ((proxy != NULL)&&(*proxy != '\0'))
    {
        MemSet(host, sizeof(host), 0x00);
        StrNCopy(host, proxy, (MAX_URL - 1));
    }

    // �l�b�g���C�u�������I�[�v������ĂȂ��Ƃ��̓I�[�v������B
    ret = NNshNet_open(&(NNshGlobal->netRef));
    if (ret != errNone)
    {
        // �I�[�v�����s
        NNsh_DebugMessage(ALTID_ERROR, "failure ", "net_open()", ret);
        NNshGlobal->netRef = 0;
        MemPtrFree(bufPtr);
        return (ret);
    }

    // �p�����[�^���ݒ肳��Ă���Ƃ��́A��ʂ��ĕ`�悷��
    if (NNshParam->redrawAfterConnect != 0)
    {
        frm  = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm);
    }

    // �����d���n�e�e�^�C�}�𖳌��ɂ���
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    // BUSY�\��
    MemSet (logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, "Connecting to ");
    StrCat (logBuf, host);
    Show_BusyForm(logBuf);

    // (HTTP��)�z�X�g�֐ڑ�
    ret = NNshNet_connect(&socketRef, NNshGlobal->netRef, host, port);
    if (ret != errNone)
    {
        // �ڑ��Ɏ��s
        Hide_BusyForm();
        NNsh_DebugMessage(ALTID_ERROR,"connection failure HOST:", host, ret);
        MemPtrFree(bufPtr);
        NNshNet_close(NNshGlobal->netRef);

        // �����d��OFF�^�C�}�𕜋�������
        (void) SysSetAutoOffTime(autoOffTime);
        (void) EvtResetAutoOffTimer();

        return (ret);
    }

    // HTTP�v�����b�Z�[�W�̑��M
    SetMsg_BusyForm("Sending a Message...");
    NNshNet_write(NNshGlobal->netRef, socketRef, StrLen(bufPtr), bufPtr);

    // HTTP�f�[�^�̎�M(��M�����f�[�^��FILE_RECVMSG�ɏ����o��)
    SetMsg_BusyForm("Waiting for a Reply...");
    MemSet(&fileRef, sizeof(fileRef), 0x00);
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
    }

    dataSize  = 0;
    totalSize = 0;
    ret       = errNone;
    while (ret == errNone)
    {
        dataSize = 0;
        // (�ꎞ)����M�o�b�t�@�̃N���A
        MemSet(bufPtr, NNshParam->bufferSize, 0x00);
        ret = NNshNet_read(NNshGlobal->netRef, socketRef,
                           NNshParam->bufferSize, bufPtr, &dataSize);
			   
        if (ret != errNone)
        {
            break;
        }
        if (dataSize == 0)
        {
            break;
        }
        totalSize = totalSize + dataSize;
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, "Received  ");
        NUMCATI(logBuf, totalSize);
        StrCat (logBuf, " Bytes...");
        SetMsg_BusyForm(logBuf);
        err = AppendFile_NNsh(&fileRef,dataSize, bufPtr, &writeSize);
        if (err != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", err);
        }
        if (writeSize == 0)
        {
            NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", 
                              " write size == 0", writeSize);
        }
    }
    err = AppendFile_NNsh(&fileRef,dataSize, bufPtr, &writeSize);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile(end) ", " ret:", err);
    }
    err = CloseFile_NNsh (&fileRef);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "CloseFile() ", " ret:", err);
    }
    (void) MemPtrFree(bufPtr);


    // ��M�f�[�^�T�C�Y�̕\��
    MemSet (logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, "Total  ");
    NUMCATI(logBuf, totalSize);
    StrCat (logBuf, " Bytes.");
    SetMsg_BusyForm(logBuf);

    // �z�X�g����ڑ���؂�ABUSY�E�B���h�E���폜����B
    NNshNet_disconnect(NNshGlobal->netRef, socketRef);
    NNshNet_close(NNshGlobal->netRef);
    Hide_BusyForm();

    // �����d��OFF�^�C�}�𕜋�������
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    return (ret);

#endif     // #ifdef OFFLINE_DEBUG
}
