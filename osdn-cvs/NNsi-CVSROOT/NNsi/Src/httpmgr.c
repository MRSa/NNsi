/*============================================================================*
 *
 *  $Id: httpmgr.c,v 1.45 2011/08/22 16:01:04 mrsa Exp $
 *
 *  FILE: 
 *     httpMgr.c
 *
 *  Description: 
 *     HTTPコミュニケーションマネージャ
 *===========================================================================*/
#define  HTTPMGR_C
#include "local.h"

/*-------------------------------------------------------------------------*/
/*   Function :   http_divideHostName                                      */
/*                                               URLからホスト名を抽出する */
/*-------------------------------------------------------------------------*/
static void NNshHttp_divideHostName(Char *url, Char *host, Char **loc, UInt16 *portNo)
{
    Char   *start;

    // ホスト名の最初と最後の場所を求める("://" と "/" に囲まれている中身)
    start = StrStr(url, "://");
    if (start != NULL)
    {
        start = start + StrLen("://");
    }
    else
    {
        // 指定がなかった場合は先頭から、、、ありえないんだけど、、、
        start = url;
    }
#ifdef USE_STRSTR
    *loc  = StrStr(start, "/");
    // "/" がなかった場合には、行の最後からコピーする
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
        // ホスト名を抽出する
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
/*                                            HTTP送信メッセージを作成する */
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

    // ファイルから送信するメッセージを読み出す。
    dataSize = 0;
    readSize = 0;
    buf      = NULL;

    // 送信ファイルからデータ取得
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

    // URLをホスト名とファイルに分割
    loc = NULL;
    NNshHttp_divideHostName(url, host, &loc, portNo);

    // 付加データサイズを確認する
    if (appendData != NULL)
    {
        readSize = StrLen(appendData);
    }
    else
    {
        readSize = 0;
    }

    // HTTPのヘッダ作成(まちBBS対応もする)
    switch (type)
    {
      case HTTP_SENDTYPE_POST_SHITARABAJBBS:
        StrCopy(buffer, "POST ");

        // リファラからそのままCGIファイル名を生成する
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
        // ●ログイン時には、最後に \r\n をつけない。
        StrIToA(&buffer[StrLen(buffer)], (dataSize + readSize));
        
        if (type == HTTP_SENDTYPE_POST_Be2chLOGIN)
        {
            // Be2chログイン時には、URLエンコードしたデータであることを送信
            StrCat(buffer, "\r\nContent-Type: application/x-www-form-urlencoded");
        }        
    }
    else
    {
        StrIToA(&buffer[StrLen(buffer)], (dataSize + readSize + 2));
    }
    StrCat (buffer, "\r\nPragma: no-cache\r\n");

    // URLエンコードしたデータであることを送信
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
            // ユーザーエージェントを出力する
            StrCat(buffer, "Monazilla/1.00  NNsi/1.5(" SOFT_REVISION ")\r\n");
        }
        if ((cookie != NULL)&&(*cookie != 0x00))
        {
            StrCat (buffer, cookie);
            StrCat (buffer, "; NAME=NNsi(PalmOS); Mail=");
        }
        else
        {
            StrCat(buffer, "Cookie: NAME=名無しさん; Mail=");
        }
        if (((NNshGlobal->NNsiParam)->useBe2chInfo != 0)&&
             (NNshGlobal->be2chCookie != NULL))
        {
            // Be@2ch用のCookieを追加する。。。
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

    // データバッファにコピーしきれるか確認
    if (bufSize >= (StrLen(buffer) + dataSize + readSize + 2))
    {
        // データのコピー
        StrCat (buffer, buf);
        if (appendData != NULL)
        {
            StrCat (buffer, appendData);
        }
        if ((type == HTTP_SENDTYPE_POST_2chLOGIN)||
            (type == HTTP_SENDTYPE_POST_Be2chLOGIN)||
            (extendUserAgent != NULL))
        {
            // ログイン時には、最後に \r\n をつけない。
        }
        else
        {
            StrCat (buffer, "\r\n");
        }
        ret = errNone;
    }
    else
    {
        // バッファサイズオーバフロー
        ret = (~errNone - 1);
    }
    MEMFREE_PTR(buf);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   http_createGetMsg                                        */
/*                                            HTTP送信メッセージを作成する */
/*-------------------------------------------------------------------------*/
static void NNshHttp_createGetMsg(UInt16 type, Char *url, Char *host,
                                   Char *extendUserAgent, UInt16 *portNo,
                                   UInt32 getRange, UInt32 endRange, 
                                   Char *buffer, Char *proxy, Char *appendData)
{
    Char *loc;

    // URLをホスト名とファイルに分割
    loc = NULL;
    NNshHttp_divideHostName(url, host, &loc, portNo);
    if ((loc == NULL)||(*loc == '\0'))
    {
        loc = "/";
    }

    // データ送信用バッファにメッセージを記載する
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
    // "chunked" を発生させないため...
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
    // gzip対応 by Nickle
    // RFC2616: Field names are case-insensitive. らしい。違和感あるんだけど
    if ((NNshGlobal->useGzip != 0)&&
         ((getRange == HTTP_RANGE_OMIT)||(getRange == 0)))
    {
        //  GZIP 使用可能な場合、先頭から取得可能なとき、もしくは範囲指定なしの
        // 場合には gzipを有効にする
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
/*                                                 HTTPデータ転送(GET)要求 */
/*                                            (受信データをrecv.txtに記録) */
/*=========================================================================*/
Err NNshHttp_commMain(UInt16 type, Char *url, Char *cookie, Char *appendData,
                      UInt32 range, UInt32 endRange, UInt16 port, Char *proxy,
                      Char   *message, UInt16 sslMode)
{
#ifdef OFFLINE_DEBUG

    Char logMes[BUFSIZE];

    // ログだけ出力して、通信したふりをする。
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
    Boolean compressed = false; // 受け取ったcontentがgzip形式かどうか
    Boolean firstBlock = false; // 現在のデータがcontentの最初の部分かどうか
#endif

#ifdef USE_CONTENT_LENGTH_INFO
    // content-lengthの初期化
    contentSize = 0;
#endif

    // ユーザエージェントの設定
    userAgent = NNshGlobal->customizedUserAgent;

    // (一時)送受信バッファの確保
    bufPtr = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (bufPtr == NULL)
    {
        // 領域の確保失敗！
        NNsh_DebugMessage(ALTID_ERROR, "HTTP(temp)", "MEMALLOC_PTR(), size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        return (~errNone);
    }
    MemSet(bufPtr, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    // 送信用メッセージの作成とＵＲＬからホスト名を取得
    MemSet(host, sizeof(host), 0x00);
    switch (type)
    {
      case HTTP_SENDTYPE_POST_2chLOGIN:
#ifdef USE_SSL_V2
        // SSL v2を使用してログインする(tiger2.he.netは、SSL v2対応なので...)
        if (NNshGlobal->palmOSVersion < 0x05200000)
        {
            // PalmOS V5.2より前の機種は、独自のSSLネゴシエーションを使用する
            sslMode = NNSH_SSLMODE_SSL_V2SPECIAL;
        }
        else
#endif  // #ifdef USE_SSL_V2
        {
            // PalmOS 5.2以降サポートのSslLibを使用する
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

    // Proxyが設定されていれば、取得したホスト名をoverride
    if ((proxy != NULL)&&(*proxy != '\0'))
    {
        MemSet(host, sizeof(host), 0x00);
        StrNCopy(host, proxy, (MAX_URL - 1));
    }

    // 通信先を表示...
    NNsh_DebugMessage(ALTID_INFO, host, ":", port);

    // ネットライブラリがオープンされてないときはオープンする。
#ifdef USE_SSL
    ret = NNshSSL_open(sslMode, &(NNshGlobal->netRef), &sslRef, &sslTemplate, &sslContext);
#else
    ret = NNshNet_open(&(NNshGlobal->netRef));
#endif
    if (ret != errNone)
    {
        // オープン失敗
        NNsh_DebugMessage(ALTID_ERROR, "failure ", "net_open()", ret);
        NNshGlobal->netRef = 0;
        MEMFREE_PTR(bufPtr);
        return (ret);
    }

    // パラメータが設定されているときは、画面を再描画する
    if ((NNshGlobal->NNsiParam)->redrawAfterConnect != 0)
    {
        frm  = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm, true);
    }

    // 自動電源ＯＦＦタイマを無効にする
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    // BUSY表示
    MemSet (logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, MSG_CONNECTING_TO);
    StrCat (logBuf, host);
    if ((message != NULL)&&(*message != '\0'))
    {
        // 表示メッセージが引数に指定されていた場合、それを末尾に付加する
        // (この表示メッセージは、とりあえずホストへ接続するときのみ付加)
        StrCat(logBuf, message);
    }
    Show_BusyForm(logBuf);

    // (HTTPで)ホストへ接続
#ifdef USE_SSL
    ret = NNshSSL_connect(sslMode, sslRef, sslContext, &socketRef, NNshGlobal->netRef, host, port);
#else
    ret = NNshNet_connect(&socketRef, NNshGlobal->netRef, host, port);
#endif
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR,"connection failure HOST:", host, ret);

        // 接続に失敗
        Hide_BusyForm(false);
        MEMFREE_PTR(bufPtr);
#ifdef USE_SSL
        NNshSSL_close(sslMode, NNshGlobal->netRef, sslRef, sslTemplate, sslContext);
#else
        NNshNet_close(NNshGlobal->netRef);
#endif

        // 自動電源OFFタイマを復旧させる
        (void) SysSetAutoOffTime(autoOffTime);
        (void) EvtResetAutoOffTimer();

        return (NNSH_ERRORCODE_FAILURECONNECT);
    }

    // HTTP要求メッセージの送信
    SetMsg_BusyForm(MSG_BUSY_SENDING);
#ifdef USE_SSL
    NNshSSL_write(sslMode, NNshGlobal->netRef, socketRef, sslRef, sslContext, StrLen(bufPtr), bufPtr);
#else
    NNshNet_write(NNshGlobal->netRef, socketRef, StrLen(bufPtr), bufPtr);
#endif

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, ")))SEND : %s ", bufPtr);
#endif // #ifdef USE_REPORTER

    // HTTPデータの受信(受信したデータをFILE_RECVMSGに書き出す)
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
        // (一時)送受信バッファのクリア
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
        // 受信データの先頭のとき、受信データの総バイト数を求める
        if (totalSize == 0)
        {
            tmpPtr = StrStr(bufPtr, NNSH_HTTP_CONTENT_LENGTH);
            if (tmpPtr != NULL)
            {
                // 数字の先頭を求める
                tmpPtr = tmpPtr + StrLen(NNSH_HTTP_CONTENT_LENGTH);
                while ((*tmpPtr != '\x0d')&&((*tmpPtr < '0')||(*tmpPtr > '9')))
                {
                    tmpPtr++;
                }

                // 総バイト数の計算実処理
                while ((*tmpPtr >= '0')&&(*tmpPtr <= '9'))
                {
                    contentSize = (contentSize * 10) + (*tmpPtr - '0');
                    tmpPtr++;
                }
            }
        }
#endif
#ifdef USE_ZLIB // by Nickle
        // 受信データの先頭のとき(＝httpヘッダがついてるとき)
        // gzip圧縮されてるか調べる
        if ((NNshGlobal->useGzip != 0)&&(totalSize == 0))
        {
            firstBlock = true; // 受信データの先頭

            // 受信ヘッダに "Content-Encoding: gzip" が含まれているか？
            tmpPtr = StrStr(bufPtr, NNSH_HTTP_CONTENT_GZIPED);
            if (tmpPtr == NULL)
            {
                tmpPtr = StrStr(bufPtr, NNSH_HTTP_CONTENT_XGZIPED);
            }
            if (tmpPtr != NULL)
            {
                // 圧縮されていた...
                compressed = true;
                if (OpenZLib() == false)
                {
                    // Zlibのオープンに失敗、接続失敗として扱う
                    Hide_BusyForm(false);
                    MEMFREE_PTR(bufPtr);
#ifdef USE_SSL
                    NNshSSL_close(sslMode, NNshGlobal->netRef, sslRef, sslTemplate, sslContext);
#else
                    NNshNet_close(NNshGlobal->netRef);
#endif
                    // 自動電源OFFタイマを復旧させる
                    (void) SysSetAutoOffTime(autoOffTime);
                    (void) EvtResetAutoOffTimer();
                    return (NNSH_ERRORCODE_FAILURECONNECT);
                }
            }
        }
        else
        {
            // 受信データが先頭ではない...
            firstBlock = false;
        }
#endif  // #ifdef USE_ZLIB 

        totalSize = totalSize + dataSize;
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, MSG_BUSY_RECV_WAIT);
        NUMCATI(logBuf, totalSize);

#ifdef USE_CONTENT_LENGTH_INFO
        // 総受信バイト数を表示するようにしてみる
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
            // gzipedなので、デコードしながらファイルに書き込む
            int headerSize = 0;

            // デコード対象の先頭。とりあえずデータの先頭を指す
            // httpヘッダがあれば、そこを除いたcontent本体を指す
            Char* decodePtr = bufPtr;
 
            // httpヘッダ部があれば、そこは圧縮されていないのでそのまま書き込む
            if (firstBlock != false)
            {
                tmpPtr = StrStr(bufPtr, "\r\n\r\n");
                if (tmpPtr != NULL)
                {
                    // ヘッダあった
                    tmpPtr = tmpPtr + 4; // +4 : \r\n\r\nのぶん
                    decodePtr = tmpPtr;

                    // (ヘッダ部分は)無圧縮なのでそのまま書き込む
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
          
            // gzip解凍＆ファイルに書き込む
            // ToDo: HTTP/1.1 applications MUST be able to receive and decode the "chunked" transfer-coding
            //       RFC 2616 - Hypertext Transfer Protocol -- HTTP/1.1
            ParseGZip(&fileRef, decodePtr, (dataSize - headerSize), firstBlock);
        }
        else // if(compressed)
#endif
        {
            // 非圧縮データは直接ファイルに出力する
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
        // SSL通信データバッファのクリア指示(実は不要かな？)
        // NNshSSL_flush(sslMode, sslRef, sslContext);
#endif
    }

    // 残りデータの出力
#ifdef USE_ZLIB // by Nickle
    if (compressed != false)
    {
        // gzipedなので、デコードしながらファイルに書き込む
        // gzip解凍＆ファイルに書き込
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
        CloseZLib(); // ZLIBを使ったのでクローズ
    }
#endif

    // 受信データサイズの表示
    MemSet (logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, MSG_BUSY_RECV_END);
    NUMCATI(logBuf, totalSize);
    StrCat (logBuf, MSG_BUSY_RECV_BYTES);
    SetMsg_BusyForm(logBuf);

    // ホストから接続を切り、BUSYウィンドウを削除する。
#ifdef USE_SSL
    NNshSSL_disconnect(sslMode, sslRef, sslContext, socketRef, NNshGlobal->netRef);
    NNshSSL_close(sslMode, NNshGlobal->netRef, sslRef, sslTemplate, sslContext);
#else
    NNshNet_disconnect(NNshGlobal->netRef, socketRef);
    NNshNet_close(NNshGlobal->netRef);
#endif
    Hide_BusyForm(false);

    // 自動電源OFFタイマを復旧させる
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    return (ret);

#endif     // #ifdef OFFLINE_DEBUG
}
