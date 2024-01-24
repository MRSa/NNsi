/*============================================================================*
 *
 *  $Id: modifylog.c,v 1.3 2005/07/18 16:26:31 mrsa Exp $
 *
 *  FILE: 
 *     modifylog.c
 *
 *  Description: 
 *     参照ログのログ取得後加工
 *===========================================================================*/
#define MODIFYLOG_C
#include "local.h"

#ifdef USE_LOGCHARGE

/*--------------------------------------------------------------------------*/
/*   launch_viewJPEG() : JPEGファイルの閲覧(DA起動処理の予定...)            */
/*                                      fileLoc  : ファイルのありか         */
/*                                          FILEMGR_STATE_OPENED_STREAM     */
/*                                          FILEMGR_STATE_OPENED_VFS        */
/*                                      fileName : ファイル名(full-path)    */
/*--------------------------------------------------------------------------*/
static void launch_viewJPEG(UInt16 fileLoc, Char *fileName)
{
    Char   fileType, *cmdMsg;

    NNsh_DebugMessage(ALTID_INFO, "OPEN JPEG : ", fileName, fileLoc);

    // nnDAがインストールされているか確認する
    if (CheckInstalledResource_NNsh('DAcc','nnDA') == false)
    {
        // nnDA未インストール、終了する
        return;
    }

    if (fileLoc == FILEMGR_STATE_OPENED_VFS)
    {
        // "VFSはコンパクトフラッシュを使用する"設定の場合、、、
        switch ((NNshGlobal->NNsiParam)->vfsUseCompactFlash)
        {
          case NNSH_VFS_USEMEDIA_CF:             
            fileType = 'c';
            break;

          case NNSH_VFS_USEMEDIA_MS: 
            fileType = 'm';
            break;

          case NNSH_VFS_USEMEDIA_SD:
            fileType = 's';             
            break;

          case NNSH_VFS_USEMEDIA_SM:
            fileType = 't';
            break;
                
          case NNSH_VFS_USEMEDIA_RD:             
            fileType = 'r';
            break;

          case NNSH_VFS_USEMEDIA_DoC:
            fileType = 'd';
            break;

          default:
            // 指定しない
            fileType = 'v';
            break;
        }
    }
    else
    {
        fileType = 'f';
    }

    // クリップボード用の領域を確保する
    cmdMsg = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (cmdMsg == NULL)
    {
        return;
    }
    MemSet(cmdMsg, BIGBUF + MARGIN, 0x00);

    StrCopy(cmdMsg, nnDA_NNSIEXT_VIEWSTART);
    StrCat (cmdMsg, nnDA_NNSIEXT_INFONAME);
    StrCat (cmdMsg, nnDA_NNSIEXT_SHOWJPEG);
    cmdMsg[StrLen(cmdMsg)] = fileType;
    StrCat (cmdMsg, "NNsi:");
    StrCat (cmdMsg, fileName);
    StrCat (cmdMsg, nnDA_NNSIEXT_ENDINFONAME);
    StrCat (cmdMsg, nnDA_NNSIEXT_ENDVIEW);
    ClipboardAddItem(clipboardText, cmdMsg, StrLen(cmdMsg) + 1); 

    MEMFREE_PTR(cmdMsg);

    // nnDAを起動する
    (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
    return;
}

/*--------------------------------------------------------------------------*/
/*  ReadOnlyLogDataPickup() : 参照ログのログ取得実処理                      */
/*                                                                          */
/*                                                                          */
/*                    この関数の引数で、更新がかかるデータ : url, fileName  */
/*--------------------------------------------------------------------------*/
Err ReadOnlyLogDataPickup(Boolean isImage, NNshGetLogDatabase *dbData,
                          Char *url, Char *fileName, UInt16 recNum,
                          UInt32 recId, UInt16 depth, UInt16 number, 
                          Char *dateTime)
{
    Err                       ret;
    Boolean                   workAround, updateRecord;
    UInt16                    index, bufSize, kanjiCode, redirectCnt, titleSize, subIndex;
    UInt32                    readSize, offset, dummy, offsetW, currentTime;
    DmOpenRef                 subjRef;
    Char                      *ptr, *file, *realBuf, *chk, *dateBuf, *titlePrefix;
    NNshSubjectDatabase       *subjDb;
    NNshFileRef                fileRefR, fileRefW;
    NNshGetLogSubFileDatabase *subDbData;

    // スレタイトルが上位(呼び出し元)で指定されているか？
    if ((depth != 0)&&(dateTime != NULL))
    {
        // スレタイトルは、第２階層以下で指定されている
        // (第一階層での変数dateTimeは、If-Modified-Since の引数で利用している)
        // 強烈な変数の流用なので注意すること、、。
        titlePrefix = dateTime;
    }
    else
    {
        titlePrefix = NULL;
    }

    // 領域を確保する
    subIndex = NNSH_DATABASE_BLANK;
    bufSize = sizeof(NNshSubjectDatabase) + MARGIN;
    subjDb = MEMALLOC_PTR(bufSize);
    if (subjDb == NULL)
    {
        // 一時領域の確保に失敗した
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", bufSize);
        return (~errNone);
    }
    MemSet(subjDb, sizeof(NNshSubjectDatabase) + MARGIN, 0x00);
  
    dateBuf = MEMALLOC_PTR(sizeof("If-Modified-Since: ") + MAX_GETLOG_DATETIME + MARGIN);
    if (dateBuf == NULL)
    {
        // 一時領域の確保に失敗...
        MEMFREE_PTR(subjDb);
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", (sizeof("If-Modified-Since: ") + MAX_GETLOG_DATETIME + MARGIN));
        return (~errNone);
    } 
    MemSet(dateBuf, (sizeof("If-Modified-Since: ") + MAX_GETLOG_DATETIME + MARGIN), 0x00);
  
    subDbData = MEMALLOC_PTR(sizeof(NNshGetLogSubFileDatabase) + MARGIN);
    if (subDbData == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (subDbData) ", "", 0);
        MEMFREE_PTR(subjDb);
        MEMFREE_PTR(dateBuf);
        return (~errNone);
    }
    MemSet(subDbData, (sizeof(NNshGetLogSubFileDatabase) + MARGIN), 0x00);
  
    // 指定されたURLを表示する
    NNsh_DebugMessage(ALTID_INFO, "url:", url, 0);

    // 漢字コード
    kanjiCode = dbData->kanjiCode;

    // 一時バッファ領域を確保
    bufSize = (NNshGlobal->NNsiParam)->bufferSize + MARGIN;
    realBuf = MEMALLOC_PTR(bufSize);
    if (realBuf == NULL)
    {
        // 一時領域の確保に失敗した
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", bufSize);
        MEMFREE_PTR(dateBuf);
        MEMFREE_PTR(subDbData);
        MEMFREE_PTR(subjDb);
        return (~errNone);
    }
    MemSet(realBuf, bufSize, 0x00);
    bufSize = bufSize - MARGIN;

    // スレデータベースのオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &subjRef);
    if (subjRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        MEMFREE_PTR(subjDb);
        MEMFREE_PTR(subDbData);
        MEMFREE_PTR(dateBuf);
        MEMFREE_PTR(realBuf);
        return (~errNone);
    }

    // 書き出しファイル名の特定と、スレＤＢに書き込むデータを特定
    if (StrLen(fileName) != 0)
    {
        // 引数にファイル名が指定されていた場合には、そのファイル名を使用する
        //                                      (ファイル名をスレDBに格納する)
        StrNCopy(subjDb->threadFileName, fileName, MAX_THREADFILENAME);
    }
    MemSet(fileName, sizeof(fileName), 0x00);

    // OFFLINEスレがVFSに格納される場合には、ディレクトリを付加する。
    workAround = false;
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
    {
        // ログベースディレクトリを付加する
        if ((NNshGlobal->logPrefixH == 0)||
            ((ptr = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
        {
            StrCopy(fileName, LOGDATAFILE_PREFIX);
        }
        else
        {
            StrCopy(fileName, ptr);
            MemHandleUnlock(NNshGlobal->logPrefixH);
        }
        subjDb->msgState = FILEMGR_STATE_OPENED_VFS; 

        // CLIE4用不具合回避フラグが立っているかチェックする
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND) != 0)
        {
            workAround = true;
        }       
    }
    else
    {
        subjDb->msgState = FILEMGR_STATE_OPENED_STREAM;        
    }
    if (StrStr(dbData->dataTitlePrefix, "%t") == NULL)
    {
        StrCopy(subjDb->threadTitle, dbData->dataTitlePrefix);
    }
    else
    {
        //  とりあえず、１つ下ではない場合、"%t"がスレタイヘッダに
        // 含まれているときは、その前までの文字列をヘッダに入れる
        chk = dbData->dataTitlePrefix;
        ptr = subjDb->threadTitle;
        while ((*chk != '\0')&&
                ((*chk != '%')||(*(chk + 1) != 't')))
        {
            *ptr = *chk;
            ptr++;
            chk++;
        }
        *ptr = '\0';
    }
    StrCopy(subjDb->boardNick, OFFLINE_THREAD_NICK);
    subjDb->state = NNSH_SUBJSTATUS_UNKNOWN;

    // 現在時刻を取得
    currentTime = TimGetSeconds();

    // ログファイル名の構築
    file = &fileName[StrLen(fileName)];
    if (StrLen(subjDb->threadFileName) != 0)
    {
        // 引数にファイル名が指定されていた場合(引数のファイル名を利用する)
        StrCat(fileName, subjDb->threadFileName);
    }
    else
    {
        // 引数にファイル名が指定されていなかった場合(自動的にファイル名を生成)
        StrCat (fileName, FILE_LOGCHARGE_PREFIX);
        NUMCATI(fileName, recNum);
        StrCat (fileName, "-");
        NUMCATI(fileName, depth);
        StrCat (fileName, "-");
        NUMCATI(fileName, number);
        if ((depth != 0)&&(dbData->getAfterHours != 0))
        {
            //  depth指定されていた場合でかつ、n時間後再取得の指定がされていた
            // 場合、ファイル名に日時を16進数で付加する
            // (2005年1月1日からの経過秒数を付加する)
            StrCat (fileName, "-");
            NUMCATH(fileName, (currentTime 
                           - TIME_CONVERT_1904_1970 - TIME_CONVERT_1970_2005));
        }
        StrCat (fileName, FILE_LOGCHARGE_SUFFIX);    
        StrNCopy(subjDb->threadFileName, file, MAX_THREADFILENAME);
    }

    redirectCnt = 0;

    // 取得済みログが存在するかどうかのチェック
    // (スレデータのＤＢは、ファイル名でのソートかけてないので動かない、、、)
    if (IsExistRecordRR_NNsh(subjRef, subjDb->threadFileName, NNSH_KEYTYPE_CHAR, 0, &index) == errNone)
    {
        // データが既にある、データの更新チェックに切り替える
        if (dbData->getDateTime[0] != '\0')
        {
            StrCopy(dateBuf, "If-Modified-Since: ");
            StrCat (dateBuf, dbData->getDateTime);
        }
        updateRecord = true;   // 取得済みログが存在する
    }
    else
    {
        // 取得済みログが存在しない
        updateRecord = false;

        // 階層下ではなく、かつ更新時刻が入っていた場合...
        if ((depth == 0)&&(dbData->lastUpdate != 0))
        {
            // 現在時刻を取得
            currentTime = TimGetSeconds();
            currentTime = currentTime - dbData->lastUpdate;
            dummy       = ((UInt32) dbData->getAfterHours) * 60 * 60;
            if ((dbData->getAfterHours == MAX_GETLOG_REGETHOUR)||
                 (currentTime < dummy))
            {
                // 再取得時間に到達していないので、更新チェックに切り替える
                if (dbData->getDateTime[0] != '\0')
                {
                    StrCopy(dateBuf, "If-Modified-Since: ");
                    StrCat (dateBuf, dbData->getDateTime);
                }
            }
        }
    }

    // 「１つ下も取得」で、「ｎ時間更新確認」が指定されていたとき...
    if ((depth != 0)&&(dbData->getAfterHours != 0))
    {
        // 指定されたＵＲＬが取得済みかどうか確認する
        subIndex = GetSubFileDatabase(NNSH_GETLOGDB_CHECKURL, 0, url, subDbData);
        if (subIndex != NNSH_DATABASE_BLANK)
        {
            // 指定されたURLは取得済み...
            if (subDbData->lastUpdate != 0)
            {
                currentTime = TimGetSeconds();
                currentTime = currentTime - dbData->lastUpdate;
                dummy       = ((UInt32) dbData->getAfterHours) * 60 * 60;
                if ((dbData->getAfterHours == MAX_GETLOG_REGETHOUR)||
                     (currentTime < dummy))
                {
                    // 再取得時間に到達していない、、、ログ取得は行わない
                    ret = NNSH_DATABASE_PENDING;
                    goto FUNC_END;
                }
            }
            // (古い)実ログファイルを消去する...
            if (subDbData->logDbId != 0)
            {
                DeleteThreadMessage_RecordID(subDbData->logDbId);
            }
            subDbData->logDbId = 0;
        }
    }

////////  リダイレクト指示があった場合には、ここに戻ってとりなおす  ////////
GET_URL_DATA:

    // データの取得
    // ホストからスレ一覧データを取得
    if (dbData->wwwcMode == 1)
    {
        // 一部(ヘッダのみ)取得
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET_NOTMONA, url, NULL, dateBuf,
                            HTTP_GETLOG_START, HTTP_GETLOG_LEN, NULL);
    }
    else
    {
        // 通常取得モード
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET_NOTMONA, url, NULL, dateBuf,
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    }

    if (ret != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,MSG_ERROR_HTTP_COMM, "[logCharge]", ret);
            goto FUNC_END;
        }

        // 通信タイムアウト発生
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[logCharge]", ret);
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "logGET URL:%s [DONE]", url);
#endif // #ifdef USE_REPORTER

    // 読み出しファイルのオープン
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_RECVMSG, ret);
        goto FUNC_END;
    }

    // BUSYウィンドウを表示する
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    // データの読み出し
    readSize = 0;
    MemSet(realBuf, bufSize, 0x00);
    ret  = ReadFile_NNsh(&fileRefR, 0, bufSize, realBuf, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read Failure :", FILE_RECVMSG, ret);
        goto CLOSE_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "++++++++++HTTP MESSAGE++++++++++");
    HostTraceOutputTL(appErrorClass, "%s", realBuf);
    HostTraceOutputTL(appErrorClass, "++++++++++++++++++++++++++++++++");
#endif // #ifdef USE_REPORTER
    
    // if ((StrNCompare(realBuf,"HTTP/1.1 3", 10) == 0)||
    //     (StrNCompare(realBuf,  "HTTP/1.0 3", 10) == 0))
    if ((redirectCnt < MAX_REDIRECT_COUNT)&&(((realBuf[0] == 'H')&&
        (realBuf[1] == 'T')&&(realBuf[2] == 'T')&&
        (realBuf[3] == 'P')&&(realBuf[9] == '3'))))
    {
        // 応答コード(304, Not Modified)のとき...
        if ((realBuf[10] == '0')&&(realBuf[11] == '4'))
        {
            // データの更新は行わず終了する。
            NNsh_DebugMessage(ALTID_INFO, "HTTP/1.x 304 Not Modified :", url, 0);
            goto CLOSE_END;
        }

        // 本来なら、"Location:" を探すべきだが、
        ptr = StrStr(realBuf, "http://");
        if (ptr != NULL)
        {
            // リダイレクトされていた、、、別のURLから再度データを取り直す
            Hide_BusyForm(false);
            CloseFile_NNsh(&fileRefR);
        
            chk = ptr;
            // データの末尾を探す
            while ((*chk != 0x0d)&&(chk < ptr + (MAX_GETLOG_URL - 1)))
            {
                chk++;
            }
            *chk = 0x00;

            // 新たなURLをコピーする
            MemSet(url, MAX_GETLOG_URL, 0x00);
            StrNCopy(url, ptr, MAX_GETLOG_URL);

            redirectCnt++;

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "REDIRECT(%d) TO URL:%s ", redirectCnt, url);
#endif // #ifdef USE_REPORTER

            // データの再度取り直し
            goto GET_URL_DATA;
        }
        
        // "Location:" を探す、、、
        ptr = StrStr(realBuf, "Location");
        if (ptr != NULL)
        {
            // リダイレクトされていた、、、別のURLから再度データを取り直す
            Hide_BusyForm(false);
            CloseFile_NNsh(&fileRefR);
            
            // データを読み飛ばす...
            ptr = ptr + sizeof("Location") - 1;
            while ((*ptr == ':')||(*ptr == ' '))
            {
                ptr++;   
            }

            chk = ptr;
            // データの末尾を探す
            while ((*chk != 0x0d)&&(chk < ptr + (MAX_GETLOG_URL - 1)))
            {
                chk++;
            }
            *chk = 0x00;

            // 新たなURLをコピーする...が、URLが相対指定されている。。
            //   (切れ目 ( http://host.name/ )だけ残すようにする)
            chk = StrStr(url, "://");
            if (chk != NULL)
            {
                chk = chk + sizeof("://") - 1;
                while ((*chk != '/')&&(*chk != '\0'))
                {
                    chk++;
                }
                *chk = 0x00;
                dummy = MAX_GETLOG_URL - StrLen(url);
            }
            else
            {
                chk = url;
                dummy = MAX_GETLOG_URL;
            }
            StrNCopy(chk, ptr, dummy);

            redirectCnt++;

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "REDIRECT(%d) TO URL:%s ", redirectCnt, url);
#endif // #ifdef USE_REPORTER

            // データの再度取り直し
            goto GET_URL_DATA;        
        } 
    }

    // サーバの更新時刻を取得し格納する
    chk = "Last-modified";
    ptr = StrStr(realBuf, chk);
    if (ptr == NULL)
    {
        chk = "Last-Modified";
        ptr = StrStr(realBuf, chk);
        if (ptr == NULL)
        {
            // サーバの送信時刻を取得し格納する
            chk = "Date";
            ptr = StrStr(realBuf, chk);
        }
    }
    if ((depth == 0)&&((dateTime != NULL)&&(ptr != NULL)))
    {
        ptr = ptr + StrLen(chk);
        while ((*ptr != '\x0d')&&((*ptr == ':')||(*ptr <= ' ')))
        {
            ptr++;
        }
        dummy = 0;
        while ((dummy < MAX_GETLOG_DATETIME)&&
                (*(ptr + dummy) != '\x0d')&&
                (*(ptr + dummy) != '\x0a')&&
                (*(ptr + dummy) != '\0'))
        {
            dummy++;
        }
        MemSet(dateTime, MAX_GETLOG_DATETIME, 0x00);
        StrNCopy(dateTime, ptr, dummy);

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "DateTime>>>%s", dateTime);
#endif // #ifdef USE_REPORTER
    }

    if (dbData->wwwcMode == 1)
    {
        // 更新チェック終了、ここで折り返す...
        // (subjDBすら作らない...いいのか、それで？)
        NNsh_DebugMessage(ALTID_INFO, "SITE UPDATE :", url, 0);
        if (updateRecord == true)
        {
            // 既にログがある場合、警告メッセージを出すだけにする
            NNsh_DebugMessage(ALTID_WARN, "LOG DATA EXIST, but... ", "", 0);
        }
        goto CLOSE_END;
    }

    // エンコードのチェック(JPEGだけ受信可能にするか？)
    // ptr = StrStr(realBuf, "image/jpeg");
    ptr = StrStr(realBuf, "image/");
    if (ptr != NULL)
    {
        file = StrStr(realBuf, NNSH_HTTP_HEADER_ENDTOKEN);
        if ((file != NULL)&&(ptr < file))
        {
            // JPEGファイルを受信！
            file       = file + 4;  // トークン分読み飛ばす...
            offset     = (UInt32) (file - realBuf);
            kanjiCode  = NNSH_KANJICODE_NOT_TEXT;

            // ファイル名を加工する ("/Palm/Images"ディレクトリに保存)
            ptr = fileName + StrLen(fileName);
            while ((ptr > fileName)&&(*ptr != '/'))
            {
                ptr--;
            }
            dummy = StrLen(ptr) + 1;  // +1 は、'\0'も合わせて移動させるため

            // イメージ保存用のディレクトリを(一応、コピーできるように)作成する
            if (subjDb->msgState == FILEMGR_STATE_OPENED_VFS)
            {
                // VFSにコピーするときのディレクトリ指定...
                MemMove(&fileName[12], ptr, dummy); // 12は"/Palm/Images"の長さ
                StrCopy(fileName, "/Palm/Image");
                fileName[11] = 's';   // StrCopyの仕様を忘れたので、、、安全策
                CreateDir_NNsh("/Palm/Images");
            }
            else
            {
                // file streamに受信するときは、ファイル名だけにしてしまう
                MemMove(fileName, ptr, dummy);

                // ファイル名のサイズが長い場合には切る。
                fileName[MAX_DBNAMELEN - 1] = '\0';
            }
            redirectCnt = subjDb->msgState;

            // ファイル出力ロジックへ...
            goto OUTPUT_DATAFILE;
        }
    }

    // zipファイルを受信、または強制的にイメージ受信を指示した...
    ptr = StrStr(realBuf, "application/zip");
    if ((ptr != NULL)||(isImage == true))
    {
        file = StrStr(realBuf, NNSH_HTTP_HEADER_ENDTOKEN);
        if ((file != NULL)&&(ptr < file))
        {
            file        = file + 4;  // トークン分読み飛ばす...
            offset      = (UInt32) (file - realBuf);
            redirectCnt = FILEMGR_STATE_DISABLED;
            kanjiCode   = NNSH_KANJICODE_NOT_TEXT;

            // ファイル出力ロジックへ...
            goto OUTPUT_DATAFILE;
        }
    }

    // 漢字コードのチェック
    ptr = StrStr(realBuf, "charset=");
    dummy = StrLen("charset=");
    if (ptr == NULL)
    {
        // charsetが見つからない場合には、encodingを使用する
        ptr   = StrStr(realBuf, "encoding=");
        dummy = StrLen("encoding=");
    }
    if (ptr != NULL)
    {
        // 文字列を読み飛ばす
        ptr = ptr + dummy;
        
        // 空白文字を読み飛ばし
        while ((*ptr == ' ')||(*ptr == '\t')||(*ptr == '"'))
        {
            ptr++;
        }

        if (StrNCaselessCompare(ptr, "iso-2022-jp", StrLen("iso-2022-jp")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_JIS;
        }
        else if (StrNCaselessCompare(ptr, "euc-jp", StrLen("euc-jp")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_EUC;
        }
        else if (StrNCaselessCompare(ptr, "x-euc-jp", StrLen("x-euc-jp")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_EUC;
        }
        else if (StrNCaselessCompare(ptr, "shift_jis", StrLen("shift_jis")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        }
        else if (StrNCaselessCompare(ptr, "x-sjis", StrLen("x-sjis")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        }
        else if (StrNCaselessCompare(ptr, "shift-jis", StrLen("shift-jis")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        }
        else if (StrNCaselessCompare(ptr, "utf-8", StrLen("utf-8")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_UTF8;
        }
    }

    // RSSにチェックが入っていた場合には、(強制的に)UTF-8とする
    // (ただし、最上位の階層だけ...)
    if ((dbData->rssData != 0)&&(depth == 0))
    {
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "kanji :%d, utf-8:%d", 
                                               kanjiCode, NNSH_KANJICODE_UTF8);
#endif // #ifdef USE_REPORTER
        kanjiCode = NNSH_KANJICODE_UTF8;
    }

    // 実データの先頭を探す
    ptr = StrStr(realBuf, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // 先頭がなければ、データブロックの最初から解析する
        NNsh_DebugMessage(ALTID_ERROR, "Data Failure :", FILE_RECVMSG, ret);
        ptr = realBuf;
    }

    // コピーデータの補正処理
    //     ptrをデータボディの先頭に持ってくる。
    //     offsetを読み出しファイルの先頭からのオフセットを持ってくる。
    //     (offsetは、ファイルの書き出しの先頭を決めるときに利用する)
    ptr = ptr + 4;   // 4は、StrLen(NNSH_HTTP_HEADER_ENDTOKEN) と等価。
    offset = (UInt32) (ptr - realBuf);

    // 上位からタイトル名が指定されているか？
    if (titlePrefix != NULL)
    {
        // タイトル名称が指定されていた場合には、それを利用する
        file = titlePrefix;
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "##### Title : %s", file);
#endif // #ifdef USE_REPORTER
        goto SET_TITLE_STRING;
    }

    // タイトルの取得
    file = StrStr(ptr, "<title");
    if (file == NULL)
    {
        // タイトルタグが小文字でなかった場合...
        file = StrStr(ptr, "<TITLE");
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "<TITLE(kanji:%d)>", kanjiCode);
#endif // #ifdef USE_REPORTER

    if (file != NULL)
    {
        // タグの末尾を探す...
        file = file + StrLen("<TITLE");
        while (*file != '>')
        {
            file++;
        }
        file++;

        // タイトルデータの末尾を探す
        chk = StrStr(file, "</TITLE");
        if (chk == NULL)
        {
            chk = StrStr(file, "</title");
        }
        if (chk == NULL)
        {
            chk = file;
            *chk = ' ';
            chk++;
        }
        *chk = '\0';

SET_TITLE_STRING:
        // スレタイ格納領域にデータを格納(漢字コードの変換も忘れない！)
        dummy = StrLen(subjDb->threadTitle);
        titleSize = StrLen(file);

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "</TITLE(len:%lu, size:%d)>", dummy, titleSize);
#endif // #ifdef USE_REPORTER

        switch (kanjiCode)
        {
          case NNSH_KANJICODE_EUC:
            // EUC -> SJ変換して格納
            chk = MEMALLOC_PTR((titleSize * 2) + MARGIN);
            if (chk != NULL)
            {
                StrCopySJ(chk, file);
                StrNCopy(&subjDb->threadTitle[dummy], chk, MAX_THREADNAME - dummy - 1);
                MEMFREE_PTR(chk);
            }
            break;

          case NNSH_KANJICODE_JIS:
            // JIS -> SJ変換して格納(これはJIS...)
            chk = MEMALLOC_PTR((titleSize * 2) + MARGIN);
            if (chk != NULL)
            {
                StrCopyJIStoSJ(chk, file);
                StrNCopy(&subjDb->threadTitle[dummy], chk, MAX_THREADNAME - dummy - 1);
                MEMFREE_PTR(chk);
            }
             break;

          case NNSH_KANJICODE_UTF8:
            chk = MEMALLOC_PTR((titleSize * 2) + MARGIN);
            if (chk != NULL)
            {
                StrNCopyUTF8toSJ(chk, file, (titleSize * 2));
                StrNCopy(&subjDb->threadTitle[dummy], chk, MAX_THREADNAME - dummy - 1);
                MEMFREE_PTR(chk);
            }
            else
            {
                // UTF8の文字列変換ができなかった場合...
                StrNCopy(&subjDb->threadTitle[dummy], file, MAX_THREADNAME - dummy - 1);
            }
            break;                

          case NNSH_KANJICODE_SHIFTJIS:
          default:
            // そのまま格納
            StrNCopy(&subjDb->threadTitle[dummy], file, MAX_THREADNAME - dummy - 1);
            break;
        }
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "TITLE : %s ", subjDb->threadTitle);
#endif // #ifdef USE_REPORTER

OUTPUT_DATAFILE:
    // 書き込みファイルのオープン
    ret = OpenFile_NNsh(fileName, NNSH_FILEMODE_CREATE, &fileRefW);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Write File Open :", fileName, ret);
        goto CLOSE_END;
    }

    // 正常に読み込めたときは、データのボディ部分をファイルにコピーする
    ret = errNone;
    offsetW = 0;
    while (ret == errNone)
    {
        readSize = 0;
        MemSet(realBuf, bufSize, 0x00);
        ret  = ReadFile_NNsh(&fileRefR, offset, bufSize, realBuf, &readSize);

        offset  = offset + readSize;
        if ((readSize % 2) != 0)
        {
            // 書き込みサイズを偶数に補正する(対CLIE4用の保険)
            if (workAround == true)
            {
                realBuf[readSize] = ' ';
                readSize++;
            }
        }
        (void) WriteFile_NNsh(&fileRefW, offsetW, readSize, realBuf, &dummy);
        offsetW = offsetW + readSize;
    }

    // 末尾にurlを記録する(テキストファイルのときのみ)
    if (kanjiCode != NNSH_KANJICODE_NOT_TEXT)
    {
        MemSet (realBuf, bufSize, 0x00);
        StrCopy(realBuf, "<br><hr><a href=\"");
        StrCat (realBuf, url);
        StrCat (realBuf, "\">");
        StrCat (realBuf, url);
        StrCat (realBuf, "</a>");
    }

    readSize = StrLen(realBuf);
    if ((readSize % 2) != 0)
    {
        // 書き込みサイズを偶数に補正する(対CLIE4用の保険)
        if (workAround == true)
        {
            realBuf[readSize] = ' ';
            readSize++;
        }
    }
    (void) WriteFile_NNsh(&fileRefW, offsetW, readSize, realBuf, &dummy);
    offsetW = offsetW + readSize;

    CloseFile_NNsh(&fileRefW);

    ////////// 取得後ログ加工 ｛  //////////
    if (dbData->wwwcMode == 2)
    {
        // 取得後ログの階層チェック
        if (depth != 0)
        {
            // １つ下のログのとき...
            (void) TrimGetDataLog(fileName, url, dbData->subTokenId, realBuf, bufSize);
        }
        else
        {
            // 通常のログのとき...
            (void) TrimGetDataLog(fileName, url, dbData->tokenId, realBuf, bufSize);
        }
    }
    ////////// ｝ 取得後ログ加工  //////////

    // イメージファイル受信時には、ファイルクローズ処理へ
    // (DBの更新処理を実施しない...)
    if (kanjiCode == NNSH_KANJICODE_NOT_TEXT)
    {
        goto CLOSE_END;
    }

    // ログファイルサイズを記録
    subjDb->fileSize = offsetW;
    
    // BBSタイプに漢字コードを追加する
    subjDb->bbsType = NNSH_BBSTYPE_HTML | (kanjiCode << NNSH_BBSTYPE_CHAR_SHIFT);

    // 新着スレとして更新する
    subjDb->state = NNSH_SUBJSTATUS_NEW;

    // スレデータが登録済みの場合、一度データを削除する
    // (スレデータのＤＢは、ファイル名でのソートかけてないので動かない、、、)
    if (updateRecord == true)
    {
        // データが既にある、データを更新する
        ret = UpdateRecord_NNsh(subjRef, index, sizeof(NNshSubjectDatabase), subjDb);
    }
    else
    {
        // データがない、データを登録する
        ret = EntryRecord_NNsh(subjRef, sizeof(NNshSubjectDatabase), subjDb);
        index = 0;
    }

    // 「１つ下も取得」で、「ｎ時間更新確認」が指定されていたとき...
    if ((depth != 0)&&(dbData->getAfterHours != 0))
    {
        // サブDBを登録する
        // URLが記述されているか？ (データが登録済みかどうか確認する)
        if (subIndex == NNSH_DATABASE_BLANK)
        {
            // 新規登録が必要...
            StrNCopy(subDbData->dataURL, url, MAX_GETLOG_URL);
            StrNCopy(subDbData->dataFileName, subjDb->threadFileName, MAX_THREADFILENAME);
            subDbData->lastUpdate = TimGetSeconds();
            subDbData->parentDbId = recId;

            // ログデータのレコードIDを取得する
            if (GetRecordID_NNsh(subjRef, index, &(subDbData->logDbId)) != errNone)
            {
                subDbData->logDbId = 0;
                NNsh_DebugMessage(ALTID_ERROR, "Cannot get record id ", " ", 0);
            }
            EntryLogSubFileDatabase(subDbData);
        }
        else
        {
            // (古い)実ログファイルを消去する...
            if (subDbData->logDbId != 0)
            {
                DeleteThreadMessage_RecordID(subDbData->logDbId);
            }
            // ログデータのレコードIDを取得する
            if (GetRecordID_NNsh(subjRef, index, &(subDbData->logDbId)) != errNone)
            {
                subDbData->logDbId = 0;
                NNsh_DebugMessage(ALTID_ERROR, "cannot get record id ", " ", 0);
            }

            // レコード更新処理が必要(時刻だけ更新)
            subDbData->lastUpdate = TimGetSeconds();
            subDbData->parentDbId = recId;
            UpdateSubFileDatabase(subIndex, subDbData);
            
        }
    }

CLOSE_END:
    // ファイルのクローズ
    Hide_BusyForm(false);
    CloseFile_NNsh(&fileRefR);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "CLOSE END>>ReadOnlyLogDataPickup()");
#endif // #ifdef USE_REPORTER

FUNC_END:
    // データベースのクローズ
    CloseDatabase_NNsh(subjRef);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "FUNC END>>ReadOnlyLogDataPickup"
                      "(subjDB:%x, dateBuf:%x, realBuf:%x)",
                      &subjDb, &dateBuf, &realBuf);
#endif // #ifdef USE_REPORTER

    // メモリ開放...
    MEMFREE_PTR(realBuf);
    MEMFREE_PTR(subDbData);
    MEMFREE_PTR(dateBuf);
    MEMFREE_PTR(subjDb);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "GET URL:%s DONE.", url);
#endif // #ifdef USE_REPORTER

    // イメージファイル受信時には、DA起動処理を行う...
    if (kanjiCode == NNSH_KANJICODE_NOT_TEXT)
    {
        if (redirectCnt != FILEMGR_STATE_DISABLED)
        {
            // JPEGファイルを受信した場合、DA起動を行う。
            launch_viewJPEG(redirectCnt, fileName);
        }
        else
        {
            // ファイル受信報告を行う
            NNsh_WarningMessage(ALTID_WARN, fileName, MSG_RECEIVE_IMAGEFILE, 0);
        }
    }
    return (ret);
}


/*==========================================================================*/
/*  TrimGetDataLog() : 参照ログのログデータを加工する                       */
/*                                                                          */
/*==========================================================================*/
Err TrimGetDataLog(Char *fileName, Char *url, UInt32 tokenId, Char *realBuf, UInt16 bufSize)
{
    Err                   ret;
    NNshFileRef           fileRefR, fileRefW;
    NNshLogTokenDatabase *tokenData;
    UInt32                offsetW, offsetR, readSize, dummy;
    Char                 *ptr, status;

    // 現在のログを一旦Palm内へコピーする
    ret = CopyFile_NNsh(FILE_SENDMSG, fileName, NNSH_COPY_VFS_TO_PALM);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "CopyFile_NNsh()", " :", ret);
        return (ret);
    }

    // 区切りデータベースのデータ領域を確保
    tokenData = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (tokenData == NULL)
    {
        // 区切りデータベースの領域確保に失敗...
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN)", " :", ret);
        return (~errNone);
    }

    // 区切りデータベースのデータを取得する
    ret = GetLogToken(tokenId, tokenData);
    if (ret != errNone)
    {
        // 区切りデータベースの取得に失敗...
        NNsh_DebugMessage(ALTID_ERROR, "GetLogToken(tokenId, tokenData)", " :", ret);
        MEMFREE_PTR(tokenData);
        return (ret);
    }
    NNsh_DebugMessage(ALTID_INFO, "tokenData :", tokenData->startToken, 0);

    // 書き込みファイルのオープン
    ret = OpenFile_NNsh(fileName, NNSH_FILEMODE_CREATE, &fileRefW);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Write File Open :", fileName, ret);
        MEMFREE_PTR(tokenData);
        (void) CopyFile_NNsh(fileName, FILE_SENDMSG, NNSH_COPY_PALM_TO_VFS);
        return (ret);
    }

    // 読み込みファイルオープン
    ret = OpenFile_NNsh(FILE_SENDMSG, (NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE), &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_SENDMSG, ret);
        MEMFREE_PTR(tokenData);
        (void) CopyFile_NNsh(fileName, FILE_SENDMSG, NNSH_COPY_PALM_TO_VFS);
        return (ret);
    }

    // 正常に読み込めたときは、ファイルをコピーする
    status  = NNSH_COPYSTATUS_SEARCHTOP;
    offsetW = 0;
    offsetR = 0;
    while (ret == errNone)
    {
        readSize = 0;
        MemSet(realBuf, bufSize, 0x00);
        ret  = ReadFile_NNsh (&fileRefR, offsetR, bufSize, realBuf, &readSize);
        switch (status)
        {        
          case NNSH_COPYSTATUS_SEARCHTOP:
            ptr = StrStr(realBuf, tokenData->startToken);
            if (ptr != NULL)
            {
                status = NNSH_COPYSTATUS_COPYING;
                offsetR = offsetR + (ptr - realBuf);

                NNsh_DebugMessage(ALTID_INFO, "tokenData :", tokenData->startToken, (offsetR));
                ret = errNone;
            }
            else
            {
                // 次の読み出し位置を調整する
                offsetR = offsetR + readSize;
                dummy = StrLen(tokenData->startToken);
                if ((dummy % 2) != 0)
                {
                    dummy = dummy + 1;
                }
                if (readSize > dummy)
                {
                    offsetR = offsetR - dummy;
                }
            }
            break;

          case NNSH_COPYSTATUS_COPYING:
            // 加工したログをコピーする...
            ptr = StrStr(realBuf, tokenData->endToken);
            if (ptr != NULL)
            {
                status = NNSH_COPYSTATUS_COPYDONE;
                readSize = (ptr - realBuf) + StrLen(tokenData->endToken) + 1;
            }
            if ((readSize % 2) != 0)
            {
                // 書き込みサイズを偶数に補正する(対CLIE用の保険)
                if ((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)&&
                    (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND) != 0))
                {
                    realBuf[readSize] = ' ';
                    readSize++;
                }
            }
            (void) WriteFile_NNsh(&fileRefW, offsetW, readSize, realBuf, &dummy);
            offsetW = offsetW + readSize;
            break;

          case NNSH_COPYSTATUS_COPYDONE:
          default:
            // 何もしない... (コピー終了なので)
            ret = fileErrEOF;
            break;
        }
    }

    // オープンしたファイルを閉じる...
    CloseFile_NNsh(&fileRefW);
    CloseFile_NNsh(&fileRefR);

    MEMFREE_PTR(tokenData);

    // ファイルの最後までコピーした時はerrNoneで応答する
    if ((ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        ret = errNone;
    }
    return (ret);
}

#endif    // #ifdef USE_LOGCHARGE
