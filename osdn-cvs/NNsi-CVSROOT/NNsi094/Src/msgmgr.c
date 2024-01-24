/*============================================================================*
 *  FILE: 
 *     msgmgr.c
 *
 *  Description: 
 *     Message(Thread) manager for NNsh. 
 *
 *===========================================================================*/
#define MSGMGR_C
#include "local.h"
extern UInt32 CreateThreadIndexSub_ARM();

/*=========================================================================*/
/*   Function : CreateThreadIndexSub                                       */
/*                                              (インデックス取得の実処理) */
/*=========================================================================*/
static Err createThreadIndexSub(Char   *buffer, UInt32 size, UInt32 offset,
                                UInt32 *nofMsg, UInt32 *msgOffset)
{
    Char *chkPtr, *ptr;
    
    chkPtr = buffer;
    while (chkPtr < buffer + size)
    {
        // メッセージの区切りを探す
        ptr = chkPtr;
        while ((*ptr != '\x0a')&&(*ptr != '\0'))
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            ptr = ptr + 1;
            if (*nofMsg < NNSH_MESSAGE_MAXNUMBER)
            {
                (*nofMsg)++;
                msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
                chkPtr = ptr;
            }
            else
            {
                return (~errNone - 1);
            }
        }
        else
        {
            // とりあえず次にすすむ
            chkPtr = ptr + 1;
        }
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : get_subject_database                                       */
/*                                                         スレ情報を取得  */
/*=========================================================================*/
Err Get_Subject_Database(UInt16 index, NNshSubjectDatabase *subjDB)
{
    Err       ret;
    DmOpenRef dbRef;

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_INFO, "ERR>OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }

    // スレデータを取得
    ret = GetRecord_NNsh(dbRef, index, sizeof(NNshSubjectDatabase), subjDB);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_INFO, "ERR>GetRecord_NNsh() ", " ", ret);
    }
    CloseDatabase_NNsh(dbRef);
    return (ret);
}

/*=========================================================================*/
/*   Function : MoveMessageToReadOnly                                      */
/*                                  (メッセージを参照専用スレに移動させる) */
/*=========================================================================*/
Err MoveMessageToReadOnly(UInt16 index)
{
    Err                 ret;
    NNshSubjectDatabase subjDb;

    ////////////////////  データベースの所属スレ名更新  ////////////////////

    // 領域の初期化
    MemSet(&subjDb, sizeof(subjDb), 0x00);

    // データベースからデータを取得
    ret = Get_Subject_Database(index, &subjDb);
    if (ret != errNone)
    {
        // ありえないはずだが？
        NNsh_DebugMessage(ALTID_INFO, "get_subject_database()"," ", ret);
        return (ret);
    }

    // スレ名を「読みこみ専用」に更新
    MemSet (subjDb.boardNick, MAX_NICKNAME, 0x00);
    StrCopy(subjDb.boardNick, OFFLINE_THREAD_NICK);

    // データを更新
    ret = update_subject_database(index, &subjDb);
    if (ret != errNone)
    {
        // ありえないはず...
        NNsh_DebugMessage(ALTID_INFO, "update_subject_database()"," ", ret);
        return (ret);
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : createThreadIndexSub_MachiBBS                              */
/*                            (インデックス取得の実処理:まちBBS用チューン) */
/*-------------------------------------------------------------------------*/
static Err createThreadIndexSub_MachiBBS(Char   *buffer, UInt32 size,
                           UInt32 offset, UInt32 *nofMsg, UInt32 *msgOffset)
                                
{
    Char *chkPtr, *ptr;
    
    chkPtr = buffer;

    // 先頭から読み出したとき、メッセージの先頭を特定する
    if (*nofMsg == 0)
    {
        ptr = StrStr(chkPtr, NNSH_MACHITOKEN_START);
	if (ptr == NULL)
        {
            // 区切りが見つからなかった、解析終了(エラー応答)
            NNsh_DebugMessage(ALTID_ERROR, "Not Found Message Top :",
                              NNSH_MACHITOKEN_START, 0);
            return (~errNone - 2);
        }
        ptr = ptr + sizeof(NNSH_MACHITOKEN_START) - 1;
        msgOffset[0] = ((UInt32) (ptr - buffer)) + offset;
        chkPtr = ptr;
        NNsh_DebugMessage(ALTID_INFO, "FIND MESSAGE TOP :", "", msgOffset[0]);
    }
    while (chkPtr < buffer + size)
    {
        // メッセージの区切りを探す
        ptr = StrStr(chkPtr, NNSH_MACHITOKEN_END);
        if (ptr == NULL)
        {
            // 区切りがないから、解析終了
            break;
        }
        ptr = ptr + sizeof(NNSH_MACHITOKEN_END) - 1;
        if (*nofMsg < NNSH_MESSAGE_MAXNUMBER)
        {
            (*nofMsg)++;
            msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
            chkPtr = ptr;
        }
        else
        {
            // メッセージの格納可能な最大数を超えた(オーバフロー表示)
            NNsh_InformMessage(ALTID_WARN, "Index Number overflow", "", 0);
            return (~errNone - 1);
        }
    }
    NNsh_DebugMessage(ALTID_INFO, "Parsed Token :", "", *nofMsg);
    return (errNone);
}

/*=========================================================================*/
/*   Function : createMessageThreadIndex                                   */
/*                                (メッセージスレッドのインデックス作成)   */
/*=========================================================================*/
Err CreateMessageThreadIndex(NNshMessageIndex *index, UInt32 *nofMsg,
                             UInt32 *msgOffset, UInt16 bbsType)
{
    Err          ret = errNone;
    UInt32       fileSize, readSize, chkPoint;
    Char        *bufPtr;
#ifdef USE_ARMLET
    UInt32       armArgs[6];
    MemHandle    armH;
    void        *armP;

    // ARMletの関数ポインタを取得する
    armH = 0;
    armP = NULL;
    if (NNshParam->useARMlet != 0)
    {
        armH = DmGetResource('armc', RSCID_createThreadIndexSub_ARM);
        if (armH != 0)
        {
            armP = MemHandleLock(armH);
        }
    }
#endif

    // インデックス作成中表示
    Show_BusyForm(MSG_CREATE_MESSAGE_IDX);

    // 一時的に読み込む領域の初期化
    bufPtr   = index->buffer;
    *nofMsg  = 0;
    fileSize = 0;
    while (fileSize < index->fileSize)
    {
        MemSet(bufPtr, NNshParam->bufferSize, 0x00);
        readSize = 0;
        ret      = ReadFile_NNsh(&(index->fileRef), fileSize,
                                 NNshParam->bufferSize, bufPtr, &readSize);
        switch (ret)
        {
            case vfsErrFileEOF:
            case fileErrEOF:
            case errNone:
              // read OK, create Index!
              break;

            default:
              // read failure!
              goto FUNC_END;
              break;
        }
        if (bbsType == NNSH_BBSTYPE_MACHIBBS)
        {
            //  まちBBSはトークンが長いため、トークン途中でバッファが尽きた
            // ときの対策を入れた(つもり)。
            if (ret == errNone)
            {
                chkPoint = readSize - sizeof(NNSH_MACHITOKEN_END) + 2;
            }
            else
            {
                chkPoint = readSize;
            }
            //  まちBBSのインデックス解析処理
            ret = createThreadIndexSub_MachiBBS(index->buffer, readSize,
                                                fileSize, nofMsg, msgOffset);
            readSize = chkPoint;
        }
        else
        {
#ifdef USE_ARMLET
            //  2ちゃんねるのインデックス解析処理
            if (armP != NULL)
            {
                // ARMletを使用してのインデックス解析処理
                MemSet(armArgs, sizeof(armArgs), 0x00);
                armArgs[0] = (UInt32) msgOffset;
                armArgs[1] = (UInt32) nofMsg;
                armArgs[2] = (UInt32) fileSize;
                armArgs[3] = (UInt32) readSize;
                armArgs[4] = (UInt32) index->buffer;
                ret        = (Err) PceNativeCall(armP, armArgs);
            }
            else
#endif
            {
                // ARMletを使用しない場合のインデックス解析処理
                ret = createThreadIndexSub(index->buffer, readSize,
                                           fileSize, nofMsg, msgOffset);
            }
        }
        if (ret != errNone)
        {
            // インデックスオーバフロー発生(ありえないはず...)、途中でやめる
            // メッセージの格納可能な最大数を超えた(オーバフロー表示)
            NNsh_InformMessage(ALTID_WARN, "Index Number overflow", "", 0);

            // 次にすすむよう、エラーを解釈しないようにする。
            ret = errNone;
            break;
        }

        // 次回読み込み位置を fileSizeに設定する。
        fileSize = fileSize + readSize;        
    }

FUNC_END:
#ifdef USE_ARMLET
    // ARMletの関数を解放
    if (armP != NULL)
    {
        MemHandleUnlock(armH);
    }
    if (armH != 0)
    {
        DmReleaseResource(armH);
    }
#endif
    Hide_BusyForm();
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : update_subject_database                                    */
/*                                                         スレ情報を更新  */
/*-------------------------------------------------------------------------*/
Err update_subject_database(UInt16 index, NNshSubjectDatabase *subjDB)
{
    Err       ret;
    DmOpenRef dbRef;

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_INFO, "ERR>OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }

    // スレデータを更新
    ret = UpdateRecord_NNsh(dbRef,index,sizeof(NNshSubjectDatabase),subjDB);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_INFO, "ERR>GetRecord_NNsh() ", " ", ret);
    }
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_entryDatabase                                    */
/*                                                メッセージ情報をDBに登録 */
/*-------------------------------------------------------------------------*/
static void message_entryDatabase(UInt16 idxMES,UInt32 fileSize,UInt16 bbsType)
{
    Err                 ret;
    NNshSubjectDatabase subjDb;

    // 領域の初期化
    MemSet(&subjDb, sizeof(subjDb), 0x00);

    if (idxMES != NNSH_NOTENTRY_THREAD)
    {
        // データベースからデータを取得
        ret = Get_Subject_Database(idxMES, &subjDb);
        if (ret != errNone)
        {
            // ありえないはずだが？
            NNsh_DebugMessage(ALTID_INFO, "get_subject_database()"," ", ret);
            return;
        }
    }

    ///// DBに書きこむデータを作成する /////

    // 状態値ほか
    if (fileSize != 0)
    {
        subjDb.state        = NNSH_SUBJSTATUS_NEW;
        subjDb.currentLoc   = 1;
        subjDb.fileSize     = fileSize;
        subjDb.bbsType      = (UInt8) bbsType;
        subjDb.msgAttribute = (subjDb.msgAttribute & NNSH_MSGATTR_NOTERROR);

        // スレの記録先を登録
        if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // スレ記録先はVFS
            subjDb.msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // スレ記録先はPalm本体
            subjDb.msgState = FILEMGR_STATE_OPENED_STREAM;
        }
    }
    else
    {
        subjDb.state      = NNSH_SUBJSTATUS_NOT_YET;
        subjDb.currentLoc = 0;
        subjDb.fileSize   = 0;
        subjDb.bbsType    = (UInt8) bbsType;
    }

    // データを更新
    ret = update_subject_database(idxMES, &subjDb);
    if (ret != errNone)
    {
        // ありえないはず...
        NNsh_DebugMessage(ALTID_INFO, "update_subject_database()","",ret);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_entryMessage                                     */
/*                                                  メッセージを変換、登録 */
/*-------------------------------------------------------------------------*/
static Err message_entryMessage(Char *targetFile, UInt32 *dataSize)
{
    Err           ret;
    UInt32        fileSize, offset, readSize, writeSize, chkSize;
    Char         *buffer, *ptr, *realBuf;
    NNshFileRef  fileRefR, fileRefW;

    fileSize  = 0;
    offset    = 0;
    *dataSize = 0;

    // "メッセージ変換中" 表示を行う
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_INFO, "Read File Open :", FILE_RECVMSG, ret);
    }

    // 既にファイルが存在した場合は一旦削除してからオープンする。
    (void) DeleteFile_NNsh(targetFile, NNSH_VFS_ENABLE);
    ret = OpenFile_NNsh(targetFile,
                        NNSH_FILEMODE_APPEND, &fileRefW);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Write File Open :", targetFile, ret);
        CloseFile_NNsh(&fileRefR);
        Hide_BusyForm();
        return (ret);
    }

    // 受信メッセージサイズを取得
    GetFileSize_NNsh(&fileRefR, &fileSize);

    // 一時バッファ領域を確保
    realBuf = MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MemPtrNew()", " size:",
                          NNshParam->bufferSize + MARGIN);
        ret = ~errNone;
        goto END_FUNC;
    }
    buffer = realBuf;

    // 初回データの読み込み
    MemSet(buffer, (NNshParam->bufferSize + MARGIN), 0x00);
    ret = ReadFile_NNsh(&fileRefR, offset,
                        NNshParam->bufferSize, buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :",  FILE_RECVMSG, ret);
        goto END_FUNC;
    }
    ret = errNone;

    // 分割ダウンロード時は、ファイルの最後を細工する(ちょっと時間かかるけど)
    if (NNshParam->enablePartGet != 0)
    {
        while ((readSize != 0)&&
               (*((Char *) (buffer + readSize - 1)) != '\x0a'))
        {
            readSize--;
        }
        // 強制的に受信ファイルサイズをメッセージの最後に調整する。
        fileSize = readSize;
    }

    // HTTPヘッダの終了部を見つける
    ptr = StrStr(buffer, "\x0d\x0a\x0d\x0a");
    if (ptr == NULL)
    {
        // ボディデータなし
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "[\xd\xa\xd\xa])", 0);
        ret = ~errNone + 3;
        goto END_FUNC;
    }

    // HTTPの応答コードをチェックする(HTTP/1.[01] 20[06]を受入れる)
    //   StrLen("HTTP/1.1 200") ==> 12 に置換(高速化のため)
    if ((StrNCompare(buffer,"HTTP/1.1 200",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.1 206",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.0 206",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.0 200",12) != 0))
    {
#ifdef USE_STRSTR
        ptr = StrStr(buffer, "\x0a");
#else
        ptr = buffer;
        while ((*ptr != '\x0a')&&(*ptr != '\0'))
        {
            ptr++;
        }
#endif
        *ptr = '\0';
        if (*(ptr - 1) == '\x0d')
        {
            *(ptr - 1) = '\0';
        }
        // サーバレスポンス異常
        NNsh_InformMessage(ALTID_ERROR, MSG_RECV_NOTACCEPTED, buffer, 0);
        ret = ~errNone + 10;
        goto END_FUNC;
    }
    ptr = ptr + 4;
    *dataSize = fileSize - (ptr - buffer);

    // 奇数データ書き込みのチェック(むりやり偶数に調整する)
    if (((UInt32) ptr & 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
    {
        ptr--;
        *ptr = ' ';
    }

    // データの書き込み(追記)
    chkSize = readSize - (ptr - buffer);
    if (((chkSize % 2) == 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
    {
        if (ptr[chkSize - 1] == '\x0a')
        {
            ptr[chkSize]     = ptr[chkSize - 1];
            ptr[chkSize - 1] = ' ';
            NNsh_DebugMessage(ALTID_WARN, "APPEND SPACE(pre)", "", chkSize);
        }
        else
        {
            ptr[chkSize]     = ' ';
            NNsh_DebugMessage(ALTID_WARN, "APPEND SPACE(post)", "", chkSize);
        }
        chkSize++;
    }
    ret = AppendFile_NNsh(&fileRefW, chkSize, ptr ,&writeSize);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "WriteFile_NNsh(1st)", "", ret);
        *dataSize = 0;
        ret = ~errNone + 11;
        goto END_FUNC;
    }
    offset = readSize;

    // データが最後になるまで追記する
    while (offset < *dataSize)
    {
        MemSet(buffer, NNshParam->bufferSize + MARGIN, 0x00);
        readSize = 0;
        ret = ReadFile_NNsh(&fileRefR, offset, NNshParam->bufferSize,
                            buffer, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh()", "", ret);
            *dataSize = 0;
            break;
        }
        ret = errNone;
        chkSize = 0;
        if (((readSize % 2) == 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
        {
            if (ptr[readSize - 1] == '\x0a')
            {
                buffer[readSize]     = buffer[readSize - 1];
                buffer[readSize - 1] = ' ';
                NNsh_DebugMessage(ALTID_WARN, "APPEND spase(pre)", "",
                                  readSize);
            }
            else
            {
                buffer[readSize] = ' ';
                NNsh_DebugMessage(ALTID_WARN, "APPEND spase(post)",
                                  "", readSize);
            }
            chkSize++;
        }
        ret = AppendFile_NNsh(&fileRefW, readSize+chkSize, buffer, &writeSize);
        if (ret != errNone)
        {
            NNsh_InformMessage(ALTID_ERROR, "AppendFile_NNsh()", "", ret);
            *dataSize = 0;
            break;
        }
        offset = offset + readSize;
    }

END_FUNC:    
    CloseFile_NNsh(&fileRefR);
    CloseFile_NNsh(&fileRefW);
    if (realBuf != NULL)
    {
        MemPtrFree(realBuf);
    }
    if (*dataSize == 0)
    {
        (void) DeleteFile_NNsh(targetFile, NNSH_VFS_ENABLE);
    }
    Hide_BusyForm();

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_appendMessage                                    */
/*                                            メッセージを変換、登録(追記) */
/*-------------------------------------------------------------------------*/
static Err message_appendMessage(Char *targetFile, NNshSubjectDatabase *mesDB)
{
    Err          ret;
    UInt32       fileSize, offset, readSize, writeSize, parseSize,chkSize;
    Char        *buffer, *ptr, *realBuf;
    NNshFileRef  fileRefR, fileRefW;

    // データ登録中表示
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_INFO, "Read File Open :",FILE_RECVMSG, ret);
    }

    ret = OpenFile_NNsh(targetFile, NNSH_FILEMODE_APPEND, &fileRefW);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_INFO, "Write File Open :", targetFile, ret);
    }

    // 一時バッファ領域を確保
    realBuf = MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MemPtrNew()", " size:",
                          NNshParam->bufferSize + MARGIN);
        goto END_FUNC;
    }
    buffer = realBuf;

    // 受信メッセージのサイズを取得(ヘッダ込み)
    GetFileSize_NNsh(&fileRefR, &fileSize);
    parseSize = 0;
    readSize  = 0;
    offset    = 0;

    // 初回データの読み込み
    MemSet(buffer, NNshParam->bufferSize + MARGIN, 0x00);
    ret = ReadFile_NNsh(&fileRefR, offset, NNshParam->bufferSize,
                        buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh()", "", ret);

        goto END_FUNC;
    }

    // 分割ダウンロード時は、ファイルの最後を細工する(ちょっと時間かかるけど)
    if (NNshParam->enablePartGet != 0)
    {
        while ((readSize != 0)&&
               (*((Char *) (buffer + readSize - 1)) != '\x0a'))
        {
            readSize--;
        }
        // 強制的に受信ファイルサイズをメッセージの最後に調整する。
        fileSize = readSize;
    }

    // HTTPボディ部を見つける
    ptr = StrStr(buffer, "\x0d\x0a\x0d\x0a");
    if (ptr == NULL)
    {
        // データ異常
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(\x0d\x0a\x0d\x0a))", 0);
        goto END_FUNC;
    }
    parseSize = ptr - buffer;
    ptr       = ptr + 4;
    parseSize = parseSize + 4;

    // HTTPの応答コードチェック
    // StrLen("HTTP/1.1 200") ==> 12 に置換(高速化のため)
    if ((StrNCompare(buffer,"HTTP/1.1 200 OK", 12) == 0)||
        (StrNCompare(buffer,"HTTP/1.0 200 OK", 12) == 0))
    {
        // なんか知らんけど、全メッセージ飛んできた(かな？)。
        CloseFile_NNsh(&fileRefR);
        CloseFile_NNsh(&fileRefW);
        if (realBuf != NULL)
        {
            MemPtrFree(realBuf);
        }
        Hide_BusyForm();

        // メッセージすべてが飛んできたことをDEBUG表示
        NNsh_DebugMessage(ALTID_INFO, "Received ALL MESSAGE!! ", 
                          targetFile, 0);

        // すべてのデータを受信する処理に切り替える
        ret = errNone;
        ret = message_entryMessage(targetFile, &fileSize);
        mesDB->state      = NNSH_SUBJSTATUS_NEW;
        mesDB->fileSize   = fileSize;

        // スレの記録先を登録
        if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // スレ記録先はVFS
            mesDB->msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // スレ記録先はPalm本体
            mesDB->msgState = FILEMGR_STATE_OPENED_STREAM;
        }
        return (ret);
    }

    if ((parseSize + 1 == readSize)&&(*ptr == '\x0a'))
    {
        // スレ更新なし
        CloseFile_NNsh(&fileRefR);
        CloseFile_NNsh(&fileRefW);
        if (realBuf != NULL)
        {
            MemPtrFree(realBuf);
        }
        Hide_BusyForm();
        return (NNSH_MSGSTATE_NOTMODIFIED);
    }
    if (*ptr != '\x0a')
    {
        // データエラー？(エラーの詳細は未チェック)
        CloseFile_NNsh(&fileRefR);
        CloseFile_NNsh(&fileRefW);
        if (realBuf != NULL)
        {
            MemPtrFree(realBuf);
        }
        Hide_BusyForm();

        return (NNSH_MSGSTATE_ERROR);
    }
    // 更新チェック用の部分(1バイト)をすすめる。
    ptr++;
    parseSize++;
    fileSize = fileSize - (ptr - buffer);

    if (((UInt32) ptr & 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
    {
        ptr--;
        *ptr = ' ';
    }
     // データの書き込み(追記処理)
    chkSize  = readSize - (ptr - buffer);
    if (((chkSize % 2) == 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
    {
        //  最後の文字を記載しないのではなく、最後の文字の前に空白を挿入する
        // ことで、書き込みバイト数を偶数にする。(v0.80r003) これでいい？
        if (ptr[chkSize - 1] == '\x0a')
        {
            ptr[chkSize]     = ptr[chkSize - 1];
            ptr[chkSize - 1] = ' ';
            NNsh_DebugMessage(ALTID_WARN, "append spc(pre)", "", chkSize);
        }
        else
        {
            buffer[chkSize] = ' ';
            NNsh_DebugMessage(ALTID_WARN, "append spc(post)", "", chkSize);
        }
        chkSize++;
    }
    ret = AppendFile_NNsh(&fileRefW, chkSize, ptr, &writeSize);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "WRITEFile_NNsh(1st)", "", ret);
        goto END_FUNC;
    }
    offset = readSize;

    while (offset < fileSize)
    {
        MemSet(buffer, NNshParam->bufferSize + MARGIN, 0x00);
        readSize = 0;
        ret = ReadFile_NNsh(&fileRefR, offset, NNshParam->bufferSize,
                            buffer, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            NNsh_InformMessage(ALTID_ERROR, "ERR>ReadFile_NNsh", "", ret);
            break;
        }
        chkSize = 0;
        if (((readSize % 2) == 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
        {
            if (ptr[readSize - 1] == '\x0a')
            {
                buffer[readSize]     = buffer[readSize - 1];
                buffer[readSize - 1] = ' ';
                NNsh_DebugMessage(ALTID_WARN, "Append space(pre)", "",
                                  readSize);
            }
            else
            {
                buffer[readSize] = ' ';
                NNsh_DebugMessage(ALTID_WARN, "Append space(post)", "",
                                  readSize);
            }
            chkSize++;
        }
        ret = AppendFile_NNsh(&fileRefW, readSize+chkSize, buffer, &writeSize);
        if (ret != errNone)
        {
            NNsh_InformMessage(ALTID_ERROR, "ERR>WriteFile_NNsh()", "", ret);
            break;
        }
        offset = offset + readSize;
    }
    mesDB->fileSize = mesDB->fileSize + fileSize;
    if (fileSize != 0)
    {
        // update成功
        mesDB->state = NNSH_SUBJSTATUS_UPDATE;

        // スレの記録先を登録
        if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // スレ記録先はVFS
            mesDB->msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // スレ記録先はPalm本体
            mesDB->msgState = FILEMGR_STATE_OPENED_STREAM;
        }
    }

END_FUNC:    
    CloseFile_NNsh(&fileRefR);
    CloseFile_NNsh(&fileRefW);
    if (realBuf != NULL)
    {
        MemPtrFree(realBuf);
    }
    Hide_BusyForm();

    return (errNone);
}

/*=========================================================================*/
/*   Function :   Get_AllMessage                                           */
/*                                                        メッセージ全取得 */
/*=========================================================================*/
Err Get_AllMessage(Char *url, Char *boardNick, Char *file, UInt16 idxMES,
                   UInt16 bbsType)
{
    Err                  ret;
    UInt32               fileSize, range;
    Char                 fileName[MAXLENGTH_FILENAME];

    // メッセージを格納するファイル名を作成
    MemSet (fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, boardNick);
    StrCat (fileName, file);

    // 分割ダウンロード指定時にはレンジ設定を行う
    if (NNshParam->enablePartGet != 0)
    {
        range = (UInt32) NNshParam->partGetSize - NNSH_HTTPGET_HEADERMAX;
    }
    else
    {
        range = HTTP_RANGE_OMIT;
    }

    // メッセージを受信
    ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                        HTTP_GETSUBJECT_START, range);
    if (ret != errNone)
    {
        // 通信エラー発生
        if (ret == netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "", ret);
        }
        else
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "", ret);
        }
        return (ret);
    }

    // 受信したデータを変換、登録する。
    ret = message_entryMessage (fileName, &fileSize);
    message_entryDatabase(idxMES, fileSize, bbsType);

    return (ret);
}

/*=========================================================================*/
/*   Function :   Get_PartMessage                                          */
/*                                                      メッセージ差分取得 */
/*=========================================================================*/
Err Get_PartMessage(Char *url, Char *boardNick, NNshSubjectDatabase *mesDB, 
                    UInt16 idxMES)
{
    Err    ret;
    Char   fileName[MAXLENGTH_FILENAME];
    UInt32 range;

    // メッセージを格納するファイル名を作成
    MemSet (fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, boardNick);
    StrCat (fileName, mesDB->threadFileName);

    // 分割ダウンロード指定時にはレンジ設定を行う
    if (NNshParam->enablePartGet != 0)
    {
        range = (UInt32) NNshParam->partGetSize - 
                         NNSH_HTTPGET_HEADERMAX + (mesDB->fileSize - 1);
    }
    else
    {
        range = HTTP_RANGE_OMIT;
    }
    // メッセージを送受信
    ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                        (mesDB->fileSize - 1), range);
    if (ret != errNone)
    {
        // 通信エラー発生
        if (ret == netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "", ret);
        }
        else
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "", ret);
        }
        return (ret);
    }

    // 受信したデータを変換、登録する。
    ret = message_appendMessage(fileName, mesDB);
    switch (ret)
    {
      case errNone:
        // データ変換成功、次へすすむ。
        break;

      case NNSH_MSGSTATE_NOTMODIFIED:
        // データ未更新
        NNsh_InformMessage(ALTID_INFO, 
                           MSG_RECV_NOTMODIFY, mesDB->threadTitle, 0);
        return (~errNone - 11);
        break;

      case NNSH_MSGSTATE_ERROR:
        // データ受信失敗
        NNsh_InformMessage(ALTID_ERROR, 
                          MSG_RECV_PARTMSG_ERROR, mesDB->threadTitle, ret);
        // mesDB->state        = NNSH_SUBJSTATUS_UNKNOWN;
        mesDB->msgAttribute = (mesDB->msgAttribute | NNSH_MSGATTR_ERROR);
        (void) update_subject_database(idxMES, mesDB);
        return (~errNone - 12);
        break;
      default:
        break;
    }
    mesDB->msgAttribute = (mesDB->msgAttribute & NNSH_MSGATTR_NOTERROR);

    // スレメッセージデータベースの内容を更新する。
    ret = update_subject_database(idxMES, mesDB);
    if (ret != errNone)
    {
        // ありえないはず...
        NNsh_DebugMessage(ALTID_INFO, "update_subject_database()"," ", ret);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function : MoveSubject_BBS                                            */
/*                                              スレの所属する板を変更する */
/*=========================================================================*/
Err MoveSubject_BBS(Char *dest, Char *src)
{
    Err                 ret;
    UInt16              index, cnt;
    DmOpenRef           dbRef;
    NNshSubjectDatabase subjDB;

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }
    GetDBCount_NNsh(dbRef, &cnt);
    for (index = 0; index < cnt; cnt++)
    {
        // スレデータを取得
        ret = GetRecord_NNsh(dbRef, index,
                             sizeof(NNshSubjectDatabase), &subjDB);
        if (ret != errNone)
        {
            // エラー発生(デバッグ表示)
            NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", " ", ret);
            continue;
        }

        // 指定したボードニックネームかチェックする
        if (StrCompare(subjDB.boardNick, src) == 0)
        {
            // スレを移動させる
            MemSet (subjDB.boardNick, MAX_NICKNAME, 0x00);
            StrCopy(subjDB.boardNick, dest);
            UpdateRecord_NNsh(dbRef, index,
                              sizeof(NNshSubjectDatabase), &subjDB);
        }
    }
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));


    return (errNone);
}

/*=========================================================================*/
/*   Function : MarkMessageIndex                                           */
/*                                           メッセージ読み出し状況を設定  */
/*=========================================================================*/
void MarkMessageIndex(UInt16 index, UInt16 favor)
{
    Err                  ret;
    NNshSubjectDatabase  subjDB;

    if ((NNshGlobal->msgIndex) != NULL)
    {
        ret = Get_Subject_Database(index, &subjDB);
        if (ret == errNone)
        {
            subjDB.currentLoc = (NNshGlobal->msgIndex)->openMsg;
            subjDB.maxLoc     = (NNshGlobal->msgIndex)->nofMsg;    
            if ((NNshGlobal->msgIndex)->nofMsg == (NNshGlobal->msgIndex)->openMsg)
            {
                subjDB.state = NNSH_SUBJSTATUS_ALREADY;
            }
            else
            {
                subjDB.state = NNSH_SUBJSTATUS_REMAIN;
            }
            switch (favor)
            {
              case NNSH_MSGATTR_NOTSPECIFY:
                // お気に入り設定は変更しない
                break;
              case 0:
                // お気に入り設定をクリアする
                subjDB.msgAttribute 
                               = (subjDB.msgAttribute & NNSH_MSGATTR_NOTFAVOR);
                break;
              default:
                // お気に入り設定を実行する
                subjDB.msgAttribute 
                                  = (subjDB.msgAttribute | NNSH_MSGATTR_FAVOR);
                break;
            }

            // MSGの場所が不明だった場合には、記録先を記入する。
            if ((subjDB.msgState == 0)&&
                (subjDB.maxLoc != 0)&&(subjDB.currentLoc != 0))
            {
                if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
                {
                    // スレ記録先はVFS
                    subjDB.msgState = FILEMGR_STATE_OPENED_VFS;
                }
                else
                {    
                    // スレ記録先はPalm本体
                    subjDB.msgState = FILEMGR_STATE_OPENED_STREAM;
                }
            }
            (void) update_subject_database(index, &subjDB);
        }
    }
    return;
}
