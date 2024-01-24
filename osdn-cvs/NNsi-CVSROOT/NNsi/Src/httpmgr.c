/*============================================================================*
 *
 *  $Id: httpmgr.c,v 1.45 2011/08/22 16:01:04 mrsa Exp $
 *
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
static void NNshHttp_divideHostName(Char *url, Char *host, Char **loc, UInt16 *portNo)
{
    Char   *start;

    // �z�X�g���̍ŏ��ƍŌ�̏ꏊ�����߂�("://" �� "/" �Ɉ͂܂�Ă��钆�g)
    start = StrStr(url, "://");
    if (start != NULL)
    {
        start = start + StrLen("://");
    }
    else
    {
        // �w�肪�Ȃ������ꍇ�͐擪����A�A�A���肦�Ȃ��񂾂��ǁA�A�A
        start = url;
    }
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
    
    if (portNo != NULL)
    {
        start = StrStr(host, ":");
        if (start != NULL)
        {
            *start = '\0';
            start++;
            *portNo = StrAToI(start);
        }
   }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   http_createPostMsg                                       */
/*                                            HTTP���M���b�Z�[�W���쐬���� */
/*-------------------------------------------------------------------------*/
static Err NNshHttp_createPostMsg(UInt16 type, Char *url, Char *host,
                                   Char *extendUserAgent, Char *cookie, 
                                   Char *appendData, UInt16 *portNo,
                                   Char *buffer, UInt32 bufSize, Char *proxy)
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
    buf = (Char *) MEMALLOC_PTR(readSize + 8);
    if (buf == NULL)
    {
        return (~errNone);
    }
    MemSet(buf, (readSize + 8), 0x00);
    (void) ReadFile_NNsh (&sendRef, 0, readSize, buf, &dataSize);
    (void) CloseFile_NNsh(&sendRef);

    // URL���z�X�g���ƃt�@�C���ɕ���
    loc = NULL;
    NNshHttp_divideHostName(url, host, &loc, portNo);

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
    switch (type)
    {
      case HTTP_SENDTYPE_POST_SHITARABAJBBS:
        StrCopy(buffer, "POST ");

        // ���t�@�����炻�̂܂�CGI�t�@�C�����𐶐�����
        StrCat (buffer,  loc);
        StrCat (buffer, " HTTP/1.0\r\nHost: ");
        StrCat (buffer,  host);
        break;

      case HTTP_SENDTYPE_POST_MACHIBBS:
        StrCopy(buffer, "POST /bbs/write.cgi HTTP/1.0\r\nHost: ");
        StrCat (buffer,  host);
        break;

      case HTTP_SENDTYPE_POST_SHITARABA:
        StrCopy(buffer, "POST /cgi-bin/bbs.cgi HTTP/1.0\r\nHost: cgi.shitaraba.com");
        StrCopy(host, "cgi.shitaraba.com");
        break;

      case HTTP_SENDTYPE_POST_OTHERBBS:
        StrCopy(buffer, "POST ");
        if ((proxy != NULL)&&(*proxy != '\0'))
        {
            StrCat (buffer,  url);
        }
        else
        {
            StrCat (buffer,  loc);
        }
        StrCat (buffer, "../test/bbs.cgi HTTP/1.0\r\nHost: ");
        StrCat (buffer,  host);
        break;

      case HTTP_SENDTYPE_POST:
      case HTTP_SENDTYPE_POST_SEQUENCE2:
        StrCopy(buffer, "POST ");
        if ((proxy != NULL)&&(*proxy != '\0'))
        {
            StrCat (buffer, "http://");
            StrCat (buffer, host);
            
        }
        StrCat (buffer, "/test/bbs.cgi");
        StrCat (buffer, " HTTP/1.0\r\nHost: ");
        StrCat (buffer,  host);
        break;

      case HTTP_SENDTYPE_POST_2chLOGIN:
      case HTTP_SENDTYPE_POST_Be2chLOGIN:
        extendUserAgent = "DOLIB/1.00";
        // not break!!

      case HTTP_SENDTYPE_POST_OTHERMETHOD:
      default:
        StrCopy(buffer, "POST ");
        if ((proxy != NULL)&&(*proxy != '\0'))
        {
            StrCat (buffer,  url);
        }
        else
        {
            StrCat (buffer,  loc);
        }
        StrCat (buffer, " HTTP/1.0\r\nHost: ");
        StrCat (buffer,  host);
        break;
    }
    NNsh_DebugMessage(ALTID_INFO, " ", buffer, 0);

    if (type == HTTP_SENDTYPE_GET_NOTMONA)
    {
        StrCat (buffer, "\r\nAccept: text/html, image/jpeg, */*\r\nReferer: ");
    }
    else
    {
        StrCat (buffer, "\r\nAccept: text/html, */*\r\nReferer: ");
    }
    StrCat (buffer,  url);
    StrCat (buffer, "\r\nContent-Length: ");
    if ((type == HTTP_SENDTYPE_POST_2chLOGIN)||
        (type == HTTP_SENDTYPE_POST_Be2chLOGIN)||
        (extendUserAgent != NULL))
    {
        // �����O�C�����ɂ́A�Ō�� \r\n �����Ȃ��B
        StrIToA(&buffer[StrLen(buffer)], (dataSize + readSize));
        
        if (type == HTTP_SENDTYPE_POST_Be2chLOGIN)
        {
            // Be2ch���O�C�����ɂ́AURL�G���R�[�h�����f�[�^�ł��邱�Ƃ𑗐M
            StrCat(buffer, "\r\nContent-Type: application/x-www-form-urlencoded");
        }        
    }
    else
    {
        StrIToA(&buffer[StrLen(buffer)], (dataSize + readSize + 2));
    }
    StrCat (buffer, "\r\nPragma: no-cache\r\n");

    // URL�G���R�[�h�����f�[�^�ł��邱�Ƃ𑗐M
//    StrCat(buffer, "Content-Type: application/x-www-form-urlencoded\r\n");

    StrCat (buffer,  "User-Agent: ");
    if (extendUserAgent == NULL)
    {
        if (type == HTTP_SENDTYPE_POST_SEQUENCE2)
        {
            StrCat(buffer, "NNsi/1.60(" SOFT_REVISION ")\r\n");
        }
        else
        {
            // ���[�U�[�G�[�W�F���g���o�͂���
            StrCat(buffer, "Monazilla/1.00  NNsi/1.5(" SOFT_REVISION ")\r\n");
        }
        if ((cookie != NULL)&&(*cookie != 0x00))
        {
            StrCat (buffer, cookie);
            StrCat (buffer, "; NAME=NNsi(PalmOS); Mail=");
        }
        else
        {
            StrCat(buffer, "Cookie: NAME=����������; Mail=");
        }
        if (((NNshGlobal->NNsiParam)->useBe2chInfo != 0)&&
             (NNshGlobal->be2chCookie != NULL))
        {
            // Be@2ch�p��Cookie��ǉ�����B�B�B
            StrCat(buffer, "; ");
            StrCat(buffer, NNshGlobal->be2chCookie);
        }
    }
    else
    {
        StrCat(buffer, extendUserAgent);
        StrCat(buffer, "\r\nX-2ch-UA: NNsi/1.04");
        if (cookie != NULL)
        {
            StrCat (buffer, "\r\n");
            StrCat (buffer, cookie);
            StrCat (buffer, "; NAME=NNsi(PalmOS); Mail=");
        }
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
        if ((type == HTTP_SENDTYPE_POST_2chLOGIN)||
            (type == HTTP_SENDTYPE_POST_Be2chLOGIN)||
            (extendUserAgent != NULL))
        {
            // ���O�C�����ɂ́A�Ō�� \r\n �����Ȃ��B
        }
        else
        {
            StrCat (buffer, "\r\n");
        }
        ret = errNone;
    }
    else
    {
        // �o�b�t�@�T�C�Y�I�[�o�t���[
        ret = (~errNone - 1);
    }
    MEMFREE_PTR(buf);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   http_createGetMsg                                        */
/*                                            HTTP���M���b�Z�[�W���쐬���� */
/*-------------------------------------------------------------------------*/
static void NNshHttp_createGetMsg(UInt16 type, Char *url, Char *host,
                                   Char *extendUserAgent, UInt16 *portNo,
                                   UInt32 getRange, UInt32 endRange, 
                                   Char *buffer, Char *proxy, Char *appendData)
{
    Char *loc;

    // URL���z�X�g���ƃt�@�C���ɕ���
    loc = NULL;
    NNshHttp_divideHostName(url, host, &loc, portNo);
    if ((loc == NULL)||(*loc == '\0'))
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
#if 0
    if (type == HTTP_SENDTYPE_GET_NOTMONA)
    {
        StrCat (buffer,  " HTTP/1.0\r\nHOST: ");
    }
    else
    {
        StrCat (buffer,  " HTTP/1.1\r\nHOST: ");
    }
#else
    // "chunked" �𔭐������Ȃ�����...
    StrCat (buffer,  " HTTP/1.0\r\nHOST: ");
#endif
    StrCat (buffer,  host);
    StrCat (buffer,  "\r\nACCEPT: text/html, */*\r\nUser-Agent: ");
    if (extendUserAgent == NULL)
    {
        if (type !=  HTTP_SENDTYPE_GET_NOTMONA)
        {
            StrCat (buffer, "Monazilla/1.00  ");
        }
        StrCat (buffer,  "NNsi/1.50 for PalmOS (" SOFT_VERSION ")\r\n");
    }
    else
    {
        StrCat(buffer, extendUserAgent);
        StrCat(buffer, "\r\nX-2ch-UA: NNsi/1.4\r\n");
    }

#ifdef USE_ZLIB
    // gzip�Ή� by Nickle
    // RFC2616: Field names are case-insensitive. �炵���B��a������񂾂���
    if ((NNshGlobal->useGzip != 0)&&
         ((getRange == HTTP_RANGE_OMIT)||(getRange == 0)))
    {
        //  GZIP �g�p�\�ȏꍇ�A�擪����擾�\�ȂƂ��A�������͔͈͎w��Ȃ���
        // �ꍇ�ɂ� gzip��L���ɂ���
        StrCat(buffer, "ACCEPT-ENCODING: gzip");
    }
    else
#endif
    {
        StrCat(buffer, "ACCEPT-ENCODING: identity");
    }
    StrCat(buffer, "\r\nACCEPT-LANGUAGE: ja, en");

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
    if ((appendData != NULL)&&(*appendData != '\0'))
    {
        StrCat(buffer, "\r\n");
        StrCat(buffer, appendData);
    }
    StrCat(buffer, "\r\nConnection: close");
    StrCat(buffer, "\r\nPRAGMA: no-cache\r\n\r\n");

    return;
}

/*=========================================================================*/
/*   Function :   NNshHttp_commMain                                        */
/*                                                 HTTP�f�[�^�]��(GET)�v�� */
/*                                            (��M�f�[�^��recv.txt�ɋL�^) */
/*=========================================================================*/
Err NNshHttp_commMain(UInt16 type, Char *url, Char *cookie, Char *appendData,
                      UInt32 range, UInt32 endRange, UInt16 port, Char *proxy,
                      Char   *message, UInt16 sslMode)
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
    if ((message != NULL)&&(*message != '\0'))
    {
       StrCat (logMes, "[");
       StrCat (logMes, message);
       StrCat (logMes, "]");
    }
    StrCat (logMes, " ssl:");
    NUMCATI(logMes, sslMode);
    NNsh_InformMessage(ALTID_INFO, url, logMes, 0);

    return (errNone);
#else   // #ifdef OFFLINE_DEBUG

    Err             ret, err;
    Char           *bufPtr, host[MAX_URL], logBuf[MINIBUF + MARGIN], *userAgent;
    UInt16          dataSize, autoOffTime;
    UInt32          writeSize, totalSize;
#ifdef USE_CONTENT_LENGTH_INFO
    UInt32          contentSize, progress;
    Char           *tmpPtr;
#else  // #ifdef USE_CONTENT_LENGTH_INFO
#ifdef USE_ZLIB
    Char           *tmpPtr;
#endif // #ifdef USE_ZLIB
#endif // #ifdef USE_CONTENT_LENGTH_INFO
    NetSocketRef    socketRef;
    NNshFileRef     fileRef;
    FormType       *frm;

#ifdef USE_SSL
    UInt16          sslRef;
    SslLib         *sslTemplate;
    SslContext     *sslContext;
#endif

#ifdef USE_ZLIB // by Nickle
    Boolean compressed = false; // �󂯎����content��gzip�`�����ǂ���
    Boolean firstBlock = false; // ���݂̃f�[�^��content�̍ŏ��̕������ǂ���
#endif

#ifdef USE_CONTENT_LENGTH_INFO
    // content-length�̏�����
    contentSize = 0;
#endif

    // ���[�U�G�[�W�F���g�̐ݒ�
    userAgent = NNshGlobal->customizedUserAgent;

    // (�ꎞ)����M�o�b�t�@�̊m��
    bufPtr = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (bufPtr == NULL)
    {
        // �̈�̊m�ێ��s�I
        NNsh_DebugMessage(ALTID_ERROR, "HTTP(temp)", "MEMALLOC_PTR(), size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        return (~errNone);
    }
    MemSet(bufPtr, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    // ���M�p���b�Z�[�W�̍쐬�Ƃt�q�k����z�X�g�����擾
    MemSet(host, sizeof(host), 0x00);
    switch (type)
    {
      case HTTP_SENDTYPE_POST_2chLOGIN:
#ifdef USE_SSL_V2
        // SSL v2���g�p���ă��O�C������(tiger2.he.net�́ASSL v2�Ή��Ȃ̂�...)
        if (NNshGlobal->palmOSVersion < 0x05200000)
        {
            // PalmOS V5.2���O�̋@��́A�Ǝ���SSL�l�S�V�G�[�V�������g�p����
            sslMode = NNSH_SSLMODE_SSL_V2SPECIAL;
        }
        else
#endif  // #ifdef USE_SSL_V2
        {
            // PalmOS 5.2�ȍ~�T�|�[�g��SslLib���g�p����
            sslMode = NNSH_SSLMODE_SSLV3;
        }
        // not break!!!;

      case HTTP_SENDTYPE_POST_Be2chLOGIN:
      case HTTP_SENDTYPE_POST:
      case HTTP_SENDTYPE_POST_SEQUENCE2:
      case HTTP_SENDTYPE_POST_MACHIBBS:
      case HTTP_SENDTYPE_POST_SHITARABA:
      case HTTP_SENDTYPE_POST_SHITARABAJBBS:
      case HTTP_SENDTYPE_POST_OTHERBBS:
      case HTTP_SENDTYPE_POST_OTHERMETHOD:
        ret = NNshHttp_createPostMsg(type, url, host, userAgent, cookie, 
                                     appendData, &port,
                                     bufPtr, (NNshGlobal->NNsiParam)->bufferSize, proxy);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "createPostMsg()", "", ret);
            MEMFREE_PTR(bufPtr);
            return (ret);
        }
        break;

      default:
        // HTTP_SENDTYPE_GET
        NNshHttp_createGetMsg(type, url, host, userAgent, &port,
                              range, endRange, bufPtr, proxy, appendData);
        break;
    }

    // Proxy���ݒ肳��Ă���΁A�擾�����z�X�g����override
    if ((proxy != NULL)&&(*proxy != '\0'))
    {
        MemSet(host, sizeof(host), 0x00);
        StrNCopy(host, proxy, (MAX_URL - 1));
    }

    // �ʐM���\��...
    NNsh_DebugMessage(ALTID_INFO, host, ":", port);

    // �l�b�g���C�u�������I�[�v������ĂȂ��Ƃ��̓I�[�v������B
#ifdef USE_SSL
    ret = NNshSSL_open(sslMode, &(NNshGlobal->netRef), &sslRef, &sslTemplate, &sslContext);
#else
    ret = NNshNet_open(&(NNshGlobal->netRef));
#endif
    if (ret != errNone)
    {
        // �I�[�v�����s
        NNsh_DebugMessage(ALTID_ERROR, "failure ", "net_open()", ret);
        NNshGlobal->netRef = 0;
        MEMFREE_PTR(bufPtr);
        return (ret);
    }

    // �p�����[�^���ݒ肳��Ă���Ƃ��́A��ʂ��ĕ`�悷��
    if ((NNshGlobal->NNsiParam)->redrawAfterConnect != 0)
    {
        frm  = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm, true);
    }

    // �����d���n�e�e�^�C�}�𖳌��ɂ���
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    // BUSY�\��
    MemSet (logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, MSG_CONNECTING_TO);
    StrCat (logBuf, host);
    if ((message != NULL)&&(*message != '\0'))
    {
        // �\�����b�Z�[�W�������Ɏw�肳��Ă����ꍇ�A����𖖔��ɕt������
        // (���̕\�����b�Z�[�W�́A�Ƃ肠�����z�X�g�֐ڑ�����Ƃ��̂ݕt��)
        StrCat(logBuf, message);
    }
    Show_BusyForm(logBuf);

    // (HTTP��)�z�X�g�֐ڑ�
#ifdef USE_SSL
    ret = NNshSSL_connect(sslMode, sslRef, sslContext, &socketRef, NNshGlobal->netRef, host, port);
#else
    ret = NNshNet_connect(&socketRef, NNshGlobal->netRef, host, port);
#endif
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR,"connection failure HOST:", host, ret);

        // �ڑ��Ɏ��s
        Hide_BusyForm(false);
        MEMFREE_PTR(bufPtr);
#ifdef USE_SSL
        NNshSSL_close(sslMode, NNshGlobal->netRef, sslRef, sslTemplate, sslContext);
#else
        NNshNet_close(NNshGlobal->netRef);
#endif

        // �����d��OFF�^�C�}�𕜋�������
        (void) SysSetAutoOffTime(autoOffTime);
        (void) EvtResetAutoOffTimer();

        return (NNSH_ERRORCODE_FAILURECONNECT);
    }

    // HTTP�v�����b�Z�[�W�̑��M
    SetMsg_BusyForm(MSG_BUSY_SENDING);
#ifdef USE_SSL
    NNshSSL_write(sslMode, NNshGlobal->netRef, socketRef, sslRef, sslContext, StrLen(bufPtr), bufPtr);
#else
    NNshNet_write(NNshGlobal->netRef, socketRef, StrLen(bufPtr), bufPtr);
#endif

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, ")))SEND : %s ", bufPtr);
#endif // #ifdef USE_REPORTER

    // HTTP�f�[�^�̎�M(��M�����f�[�^��FILE_RECVMSG�ɏ����o��)
    SetMsg_BusyForm(MSG_BUSY_WAITREPLY);
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
        MemSet(bufPtr, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
#ifdef USE_SSL
        ret = NNshSSL_read(sslMode, NNshGlobal->netRef, socketRef,
                           sslRef, sslContext,
                           (NNshGlobal->NNsiParam)->bufferSize, bufPtr, &dataSize);
#else
        ret = NNshNet_read(NNshGlobal->netRef, socketRef,
                           (NNshGlobal->NNsiParam)->bufferSize, bufPtr, &dataSize);
#endif
        if (ret != errNone)
        {
            break;
        }
        if (dataSize == 0)
        {
            break;
        }
#ifdef USE_CONTENT_LENGTH_INFO
        // ��M�f�[�^�̐擪�̂Ƃ��A��M�f�[�^�̑��o�C�g�������߂�
        if (totalSize == 0)
        {
            tmpPtr = StrStr(bufPtr, NNSH_HTTP_CONTENT_LENGTH);
            if (tmpPtr != NULL)
            {
                // �����̐擪�����߂�
                tmpPtr = tmpPtr + StrLen(NNSH_HTTP_CONTENT_LENGTH);
                while ((*tmpPtr != '\x0d')&&((*tmpPtr < '0')||(*tmpPtr > '9')))
                {
                    tmpPtr++;
                }

                // ���o�C�g���̌v�Z������
                while ((*tmpPtr >= '0')&&(*tmpPtr <= '9'))
                {
                    contentSize = (contentSize * 10) + (*tmpPtr - '0');
                    tmpPtr++;
                }
            }
        }
#endif
#ifdef USE_ZLIB // by Nickle
        // ��M�f�[�^�̐擪�̂Ƃ�(��http�w�b�_�����Ă�Ƃ�)
        // gzip���k����Ă邩���ׂ�
        if ((NNshGlobal->useGzip != 0)&&(totalSize == 0))
        {
            firstBlock = true; // ��M�f�[�^�̐擪

            // ��M�w�b�_�� "Content-Encoding: gzip" ���܂܂�Ă��邩�H
            tmpPtr = StrStr(bufPtr, NNSH_HTTP_CONTENT_GZIPED);
            if (tmpPtr == NULL)
            {
                tmpPtr = StrStr(bufPtr, NNSH_HTTP_CONTENT_XGZIPED);
            }
            if (tmpPtr != NULL)
            {
                // ���k����Ă���...
                compressed = true;
                if (OpenZLib() == false)
                {
                    // Zlib�̃I�[�v���Ɏ��s�A�ڑ����s�Ƃ��Ĉ���
                    Hide_BusyForm(false);
                    MEMFREE_PTR(bufPtr);
#ifdef USE_SSL
                    NNshSSL_close(sslMode, NNshGlobal->netRef, sslRef, sslTemplate, sslContext);
#else
                    NNshNet_close(NNshGlobal->netRef);
#endif
                    // �����d��OFF�^�C�}�𕜋�������
                    (void) SysSetAutoOffTime(autoOffTime);
                    (void) EvtResetAutoOffTimer();
                    return (NNSH_ERRORCODE_FAILURECONNECT);
                }
            }
        }
        else
        {
            // ��M�f�[�^���擪�ł͂Ȃ�...
            firstBlock = false;
        }
#endif  // #ifdef USE_ZLIB 

        totalSize = totalSize + dataSize;
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, MSG_BUSY_RECV_WAIT);
        NUMCATI(logBuf, totalSize);

#ifdef USE_CONTENT_LENGTH_INFO
        // ����M�o�C�g����\������悤�ɂ��Ă݂�
        if (contentSize != 0)
        {
            StrCat(logBuf, "/");
            NUMCATI(&logBuf[StrLen(logBuf)], contentSize);
            StrCat (logBuf, MSG_BUSY_RECV_BYTES);
            StrCat (logBuf, " (");
            progress = ((totalSize * 100) / contentSize);
            NUMCATI(&logBuf[StrLen(logBuf)], progress);
            StrCat (logBuf, "%)");
        }
        else
#endif
        {
            StrCat (logBuf, MSG_BUSY_RECV_BYTES);
        }
        SetMsg_BusyForm(logBuf);

#ifdef USE_ZLIB // by Nickle
        if (compressed != false)
        {
            // gziped�Ȃ̂ŁA�f�R�[�h���Ȃ���t�@�C���ɏ�������
            int headerSize = 0;

            // �f�R�[�h�Ώۂ̐擪�B�Ƃ肠�����f�[�^�̐擪���w��
            // http�w�b�_������΁A������������content�{�̂��w��
            Char* decodePtr = bufPtr;
 
            // http�w�b�_��������΁A�����͈��k����Ă��Ȃ��̂ł��̂܂܏�������
            if (firstBlock != false)
            {
                tmpPtr = StrStr(bufPtr, "\r\n\r\n");
                if (tmpPtr != NULL)
                {
                    // �w�b�_������
                    tmpPtr = tmpPtr + 4; // +4 : \r\n\r\n�̂Ԃ�
                    decodePtr = tmpPtr;

                    // (�w�b�_������)�����k�Ȃ̂ł��̂܂܏�������
                    headerSize = tmpPtr - bufPtr;
                    err = AppendFile_NNsh(&fileRef, headerSize, bufPtr, &writeSize);
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
            }
            
            // NNsh_DebugMessage(ALTID_INFO, "info", " dataSize:", dataSize);
            // NNsh_DebugMessage(ALTID_INFO, "info", " headerSize:", headerSize);
            // NNsh_DebugMessage(ALTID_INFO, "bodyPtr:", decodePtr, 0);
          
            // gzip�𓀁��t�@�C���ɏ�������
            // ToDo: HTTP/1.1 applications MUST be able to receive and decode the "chunked" transfer-coding
            //       RFC 2616 - Hypertext Transfer Protocol -- HTTP/1.1
            ParseGZip(&fileRef, decodePtr, (dataSize - headerSize), firstBlock);
        }
        else // if(compressed)
#endif
        {
            // �񈳏k�f�[�^�͒��ڃt�@�C���ɏo�͂���
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
#ifdef USE_SSL
        // SSL�ʐM�f�[�^�o�b�t�@�̃N���A�w��(���͕s�v���ȁH)
        // NNshSSL_flush(sslMode, sslRef, sslContext);
#endif
    }

    // �c��f�[�^�̏o��
#ifdef USE_ZLIB // by Nickle
    if (compressed != false)
    {
        // gziped�Ȃ̂ŁA�f�R�[�h���Ȃ���t�@�C���ɏ�������
        // gzip�𓀁��t�@�C���ɏ�����
        // ToDo: HTTP/1.1 applications MUST be able to receive and decode the "chunked" transfer-coding
        //       RFC 2616 - Hypertext Transfer Protocol -- HTTP/1.1
        ParseGZip(&fileRef, bufPtr, dataSize, false);
    }
    else
#endif // #ifdef USE_ZLIB
    {
        err = AppendFile_NNsh(&fileRef,dataSize, bufPtr, &writeSize);
        if (err != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "AppendFile(end) ", " ret:", err);
        }
    }

    err = CloseFile_NNsh (&fileRef);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "CloseFile() ", " ret:", err);
    }
    MEMFREE_PTR(bufPtr);

#ifdef USE_ZLIB // by Nickle
    if (compressed != false)
    {
        CloseZLib(); // ZLIB���g�����̂ŃN���[�Y
    }
#endif

    // ��M�f�[�^�T�C�Y�̕\��
    MemSet (logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, MSG_BUSY_RECV_END);
    NUMCATI(logBuf, totalSize);
    StrCat (logBuf, MSG_BUSY_RECV_BYTES);
    SetMsg_BusyForm(logBuf);

    // �z�X�g����ڑ���؂�ABUSY�E�B���h�E���폜����B
#ifdef USE_SSL
    NNshSSL_disconnect(sslMode, sslRef, sslContext, socketRef, NNshGlobal->netRef);
    NNshSSL_close(sslMode, NNshGlobal->netRef, sslRef, sslTemplate, sslContext);
#else
    NNshNet_disconnect(NNshGlobal->netRef, socketRef);
    NNshNet_close(NNshGlobal->netRef);
#endif
    Hide_BusyForm(false);

    // �����d��OFF�^�C�}�𕜋�������
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    return (ret);

#endif     // #ifdef OFFLINE_DEBUG
}
