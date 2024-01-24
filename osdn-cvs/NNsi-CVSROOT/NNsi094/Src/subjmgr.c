/*============================================================================*
 *  FILE: 
 *     subjmgr.c
 *
 *  Description: 
 *     スレ一覧取得サブ
 *===========================================================================*/
#define  SUBJMGR_C
#include "local.h"

/*=========================================================================*/
/*   Function : Count_Thread_BBS                                           */
/*                                    指定した板のスレがいくつあるか調べる */
/*=========================================================================*/
Err Count_Thread_BBS(Char *boardNick, UInt16 *count)
{
    UInt16               lp, thrCnt;
    DmOpenRef            dbRef;
    NNshSubjectDatabase  subj;

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &thrCnt);
    *count = 0;
    for (lp = 0; lp < thrCnt; lp++)
    {
        // スレデータを取得
        if (GetRecord_NNsh(dbRef, lp, 
                           sizeof(NNshSubjectDatabase), &subj) == errNone)
        {
            if (StrCompare(subj.boardNick, boardNick) == 0)
            {
                (*count)++;
            }
        }
    }
    CloseDatabase_NNsh(dbRef);
    return (errNone);
}


/*-------------------------------------------------------------------------*/
/*   Function :   DeleteSubjectList                                        */
/*                                                  該当するスレ一覧を削除 */
/*-------------------------------------------------------------------------*/
Err DeleteSubjectList(Char *boardNick, UInt16 delState, UInt16 *thrCnt)
{
    UInt16              cnt, loop;
    DmOpenRef           dbRef;
    NNshSubjectDatabase tmpDb;
    Char                fileName[MAXLENGTH_FILENAME];

    // DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生
        return (~errNone);
    }
    GetDBCount_NNsh(dbRef, &cnt);

    Show_BusyForm(MSG_DELETE_LIST_WAIT);
    *thrCnt  = 0;
    for (loop = cnt; loop > 0; loop--)
    {
        MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
        GetRecord_NNsh(dbRef, (loop - 1), sizeof(NNshSubjectDatabase), &tmpDb);

        // 指定されたBoardIDだったらレコード削除
        if (StrCompare(tmpDb.boardNick, boardNick) == 0)
        {
            if ((delState == NNSH_SUBJSTATUS_DELETE)||
                (tmpDb.state == NNSH_SUBJSTATUS_NOT_YET))
            {
                // 実データファイルがあれば、あわせて削除
                if (tmpDb.state != NNSH_SUBJSTATUS_NOT_YET)
                {
                    MemSet (fileName, sizeof(fileName), 0x00);
                    StrCopy(fileName, tmpDb.boardNick);
                    StrCat (fileName, tmpDb.threadFileName);
                    (void) DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
                }
                (void) DeleteRecordIDX_NNsh(dbRef, (loop - 1));
            }
            else
            {
                // 削除しなかったときは、カウントアップ
                (*thrCnt)++;
            }
        }
    }
    Hide_BusyForm();
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   parse_SubjectList                                        */
/*                                                    スレ一覧のDB取り込み */
/*-------------------------------------------------------------------------*/
static Err parse_SubjectList(Char *buffer, UInt32 readSize, Char *boardNick,
                             Char *fieldSeparator, UInt16 sepaLen, UInt16 *cnt)
{
    UInt16              size;
    Char               *ptr, *dataEnd, *dataSepa, *ppp;
    DmOpenRef           dbRef;
    NNshSubjectDatabase tmpDb;

    // DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生
        return (~errNone);
    }
    *cnt = 0;

    // スレ名称の解釈メイン(HTTPヘッダを読み飛ばし)
    ptr = StrStr(buffer, "\x0d\x0a\x0d\0a");
    if (ptr == NULL)
    {
        // データ異常
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(\x0d\x0a\x0d\x0a))", 0);
        CloseDatabase_NNsh(dbRef);
        return (~errNone);
    }
    ptr = ptr + 4;

    // 「リスト登録中」の表示を行う
    Show_BusyForm(MSG_THREADLIST_CREATE);

    while (ptr < buffer + readSize)
    {
        dataEnd = StrStr(ptr, NNSH_RECORD_SEPARATOR);
        if (dataEnd == NULL)
        {
            break;
        }

        dataSepa = StrStr(ptr, fieldSeparator);
        if ((dataSepa == NULL)||(dataSepa > dataEnd))
        {
            break;
        }
        /** スレ数格納部分を削除(でも遅くなるなー) **/
        ppp = dataEnd;
        while (*ppp != ')')
        {
            ppp--;
        }
        *ppp = '\0';
        while (*ppp != '(')
        {
            ppp--;
        }
        *ppp = '\0';
        MemSet  (&tmpDb, sizeof(tmpDb), 0x00);
        tmpDb.maxLoc = (UInt16) StrAToI(ppp + 1);
        StrNCopy(tmpDb.threadFileName, ptr, (dataSepa - ptr));
        size = ((dataEnd - (dataSepa + 2)) > MAX_THREADNAME) ? 
                             (MAX_THREADNAME - 1) : (dataEnd - (dataSepa + 2));
        StrNCopy(tmpDb.threadTitle, (dataSepa + sepaLen), size);
        StrCopy (tmpDb.boardNick, boardNick);
        (void) EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &tmpDb);

        // 次データの先頭へ移動
        ptr = dataEnd + 1;
        (*cnt)++;
    }
    Hide_BusyForm();
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   convertListIndexToMsgIndex                               */
/*                                   リスト番号からスレindex番号に変換する */
/*=========================================================================*/
UInt16 convertListIndexToMsgIndex(UInt16 listIdx)
{
    UInt16 *idxP, index = 0;

    // リスト番号からスレindexに変換して応答する
    if (NNshGlobal->threadIdxH != 0)
    {
        idxP  = MemHandleLock(NNshGlobal->threadIdxH);
        if (idxP != NULL)
        {
            index = idxP[listIdx];
        }
        MemHandleUnlock(NNshGlobal->threadIdxH);
    }
    return (index);
}

/*=========================================================================*/
/*   Function :   NNsh_GetSubjectList                                      */
/*                                                         スレ一覧の取得  */
/*=========================================================================*/
Err NNsh_GetSubjectList(UInt16 index)
{
    Err                 ret;
    UInt16              cnt, cnt2;
    UInt32              fileSize, readSize;
    Char                getURL[MAX_URL + MAX_URL], *buffer;
    NNshFileRef         fileRef;
    NNshBoardDatabase   bbsData;

    // BBS-DBからBBS情報を取得
    ret = Get_BBS_Info(index, &bbsData);
    if (ret != errNone)
    {
        // エラー発生
        NNsh_DebugMessage(ALTID_ERROR,  "Get_BBS_Info() ", "", ret);
        return (ret);
    }

    // データ取得先の設定
    MemSet (getURL, sizeof(getURL), 0x00);
    StrCopy(getURL, bbsData.boardURL);
    if (bbsData.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCat(getURL, bbsData.boardNick);
    }
    StrCat (getURL, FILE_THREADLIST);

    // デバッグ表示
    NNsh_DebugMessage(ALTID_INFO, bbsData.boardNick, getURL, 0);

    // ホストからスレ一覧データを取得
    if (NNshParam->getAllThread == HTTP_GETSUBJECT_PART)
    {
        // 一部取得
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, getURL, NULL, NULL,
                            HTTP_GETSUBJECT_START, HTTP_GETSUBJECT_LEN);
    }
    else
    {
        // 全部取得
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, getURL, NULL, NULL,
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    }
    if (ret != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,MSG_ERROR_HTTP_COMM, "[SUBJ]", ret);
            return (ret);
        }

        // 通信タイムアウト発生(受信しているところまで、強制的に解釈を行う)
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[SUBJ]", ret);
    }

    // データベースから、(取得する)スレデータを未読のもののみ消去する
    DeleteSubjectList(bbsData.boardNick, NNSH_SUBJSTATUS_NOT_YET, &cnt);

    // 取得スレ数を更新(cnt == 削除されなかったスレの数)
    // Get_BBS_Info   (index, &bbsData); 既に関数の先頭で取得済み...
    bbsData.threadCnt = cnt;
    Update_BBS_Info(index, &bbsData);

    ////////// スレ一覧の解析 /////////

    // 受信データ格納用バッファの獲得
    buffer = (Char *) MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR,
                           "MemPtrNew()", " size:", NNshParam->bufferSize);
        return (~errNone);
    }
    MemSet(buffer, (NNshParam->bufferSize + MARGIN), 0x00);

    OpenFile_NNsh(FILE_RECVMSG, 
                  (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                  &fileRef);
    GetFileSize_NNsh(&fileRef, &fileSize);
    if (fileSize > NNshParam->bufferSize)
    {
        // スレ一覧が大きすぎたときは(バッファサイズに)丸める
        fileSize = NNshParam->bufferSize;
    }

    // スレ一覧の読みこみ
    ret = ReadFile_NNsh(&fileRef, 0, fileSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        // ファイル読みこみエラー
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :", FILE_RECVMSG, ret);
    }
    else
    {
        // 板情報データベースの更新(スレの登録)
        switch (bbsData.bbsType)
        {
          case NNSH_BBSTYPE_MACHIBBS:
            ret = parse_SubjectList(buffer, readSize, bbsData.boardNick,
                                    NNSH_FIELD_SEPARATOR_MACHI, 
                                    StrLen(NNSH_FIELD_SEPARATOR_MACHI), &cnt2);
            break;
          case NNSH_BBSTYPE_2ch:
          default:
            ret = parse_SubjectList(buffer, readSize, bbsData.boardNick, 
                                    NNSH_FIELD_SEPARATOR,
                                    StrLen(NNSH_FIELD_SEPARATOR), &cnt2);
            break;
        }
        cnt = cnt + cnt2;
        // Get_BBS_Info   (index, &bbsData); // 既に取得済み...
        bbsData.threadCnt = cnt;
        Update_BBS_Info(index, &bbsData);

        // デバッグ表示
        NNsh_DebugMessage(ALTID_INFO, "parsed thread :", "", cnt);
    }
    MemPtrFree(buffer);
    return (ret);   
}

/*-------------------------------------------------------------------------*/
/*   function :   create_ThreadList                                        */
/*                       指定された板に所属するスレのINDEXと名前一覧を作成 */
/*-------------------------------------------------------------------------*/
Err Create_SubjectList(UInt16 index, UInt16 threadCnt, 
                       Char *buffer, UInt32 bufSize, UInt16 *cnt)
{
    Err                  ret;
    Char                *ptr, *areaLimit;
    UInt16               dataCnt, loop, allocSize, *chrIdxP;
    DmOpenRef            dbRef;
    NNshSubjectDatabase  tmpDb;
    NNshBoardDatabase    bbsData;

    // 「お気に入り」/「全スレ取得」以外のときだけ、BBS情報を取得する
    if ((index != NNSH_SELBBS_FAVORITE)&&
        (index != NNSH_SELBBS_GETALL)&&
        (index != NNSH_SELBBS_NOTREAD))
    {
        if (Get_BBS_Info(index, &bbsData) != errNone)
        {
            ret = ~errNone;
            goto FUNC_END;
        }
    }

    // インデックスの初期化
    if (NNshGlobal->threadIdxH != 0)
    {
        MemHandleFree(NNshGlobal->threadIdxH);
        NNshGlobal->threadIdxH = 0;
    }
    *cnt = 0;

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &dataCnt);
    ptr       = buffer;
    areaLimit = buffer + bufSize;

    // スレインデックス領域を確保する
    allocSize = sizeof(UInt16) * (threadCnt + MARGIN);
    NNshGlobal->threadIdxH = MemHandleNew(allocSize);
    if (NNshGlobal->threadIdxH == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_INFO, "ERR>MemPtrNew(ThreadIndex) ",
                          "size :", allocSize);
        CloseDatabase_NNsh(dbRef);
        ret = (~errNone - 1);
        goto FUNC_END;
    }
    chrIdxP = MemHandleLock(NNshGlobal->threadIdxH);
    MemSet(chrIdxP, allocSize, 0x00);

    // 該当する板に所属するスレを(データベースから順番に)抽出する。
    for (loop = dataCnt; loop > 0; loop--)
    {
        // GetRecord_NNsh()内でゼロクリアしているため不要
        // MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
        GetRecord_NNsh(dbRef, (loop - 1), sizeof(NNshSubjectDatabase), &tmpDb);

        // 表示する/しないの判定条件(板情報によって場合分け)
        switch (index)
        { 
          case NNSH_SELBBS_GETALL:
            // 取得済み全てを表示
            if ((tmpDb.state == NNSH_SUBJSTATUS_NOT_YET)||
#ifdef USE_STRSTR
                (!(StrCompare(tmpDb.boardNick, OFFLINE_THREAD_NICK))))
#else
                ((NNshParam->notListReadOnly != 0)&&
                 ((tmpDb.boardNick[0]  == '!')&&
                  (tmpDb.boardNick[1]  == 'G')&&
                  (tmpDb.boardNick[2]  == 'i')&&
                  (tmpDb.boardNick[3]  == 'k')&&
                  (tmpDb.boardNick[4]  == 'o')&&
                  (tmpDb.boardNick[5]  == 'S')&&
                  (tmpDb.boardNick[6]  == 'h')&&
                  (tmpDb.boardNick[7]  == 'i')&&
                  (tmpDb.boardNick[8]  == 'p')&&
                  (tmpDb.boardNick[9]  == 'p')&&
                  (tmpDb.boardNick[10] == 'o'))))
#endif
            {
                //  「未取得」かもしくは参照ログかつ「参照ログは表示しない」の
	        // 場合には表示しない
                continue;
            }
            break;
          case NNSH_SELBBS_FAVORITE:
            // お気に入り指定を表示
            if ((tmpDb.msgAttribute & NNSH_MSGATTR_FAVOR)!= NNSH_MSGATTR_FAVOR)
            {
                // 「お気に入り」ではないので表示しない
                continue;
            }
            break;

          case NNSH_SELBBS_NOTREAD:
            // 未読ありを表示
            if ((tmpDb.state != NNSH_SUBJSTATUS_NEW)&&
                (tmpDb.state != NNSH_SUBJSTATUS_UPDATE)&&
                (tmpDb.state != NNSH_SUBJSTATUS_REMAIN))
            {
                // 未読があるスレではないので表示しない
                continue;
            }
            break;

          default:
            // 「お気に入り」でも「取得済み全て」でも「未読あり」でもない場合
            if (StrCompare(tmpDb.boardNick, bbsData.boardNick))
            {
                // スレの所属が違うので表示しない
                continue;
            }
            break;
        }

        // 次のタイトルをコピーする領域が余っている場合、タイトルをコピーする
        // (8は、最初の "[xxxx]" 部分(バイト数))
        if ((ptr + (NNshParam->titleListSize + 8)) < areaLimit)
        {
            // メッセージ取得状態を表示
            switch (tmpDb.state)
            {
              case NNSH_SUBJSTATUS_NEW:
                /** 新規取得(再取得) **/
                *ptr = '*';
                break;

              case NNSH_SUBJSTATUS_UPDATE:
                if ((tmpDb.msgAttribute & NNSH_MSGATTR_ERROR) ==
                     NNSH_MSGATTR_ERROR)
                {
                    /** メッセージ取得時にエラーが発生した **/
                    *ptr = '#';
                    break;
                }
                /** メッセージ更新 **/
                *ptr = '+';
                break;

              case NNSH_SUBJSTATUS_REMAIN:
                if ((tmpDb.msgAttribute & NNSH_MSGATTR_ERROR) ==
                     NNSH_MSGATTR_ERROR)
                {
                    /** メッセージ取得時にエラーが発生した **/
                    *ptr = '#';
                    break;
                }
                /** 未読あり **/
                *ptr = '-';
                break;

              case NNSH_SUBJSTATUS_ALREADY:
                if ((tmpDb.msgAttribute & NNSH_MSGATTR_ERROR) ==
                     NNSH_MSGATTR_ERROR)
                {
                    /** メッセージ取得時にエラーが発生した **/
                    *ptr = '#';
                    break;
                }
                /** メッセージ全部読んだ **/
                *ptr = ':';
                break;

              case NNSH_SUBJSTATUS_NOT_YET:
              default:                
                /** メッセージ未取得 **/
                *ptr = ' ';
                break;
            }
            ptr++;
            if (NNshParam->printNofMessage != 0)
            {
                if ((tmpDb.state != NNSH_SUBJSTATUS_NEW)&&
                    (tmpDb.state != NNSH_SUBJSTATUS_UPDATE)&&
                    (tmpDb.state != NNSH_SUBJSTATUS_UNKNOWN))
                {
                    // 取得メッセージ数を表示する
                    StrCat(ptr, "[");
                    (void) NUMCATI(ptr, tmpDb.maxLoc);
                    StrCat(ptr, "]");
                }
                else
                {
                    StrCat(ptr, "[???]");
                }
            }
            // タイトルをコピーする
            StrNCopy(&ptr[StrLen(ptr)],
                     tmpDb.threadTitle, NNshParam->titleListSize);

            // 次の領域へポインタを移動させる
            ptr = ptr + StrLen(ptr) + 1;
            chrIdxP[*cnt] = (loop - 1);
            (*cnt)++;
        }
    }
    MemHandleUnlock(NNshGlobal->threadIdxH);
    CloseDatabase_NNsh(dbRef);
    ret = errNone;

FUNC_END:
    return (ret);
}

/*=========================================================================*/
/*   Function :   Get_Thread_Count                                         */
/*                                          該当する条件のスレ数を取得する */
/*=========================================================================*/
Err Get_Thread_Count(UInt16 index, UInt16 *count)
{
    Err                  ret;
    UInt16               dataCnt, loop;
    DmOpenRef            dbRef;
    NNshSubjectDatabase  tmpDb;
    NNshBoardDatabase    bbsData;

    // スレ数を初期化
    *count     = 0;

    // "お気に入り"ではないスレなので、ＢＢＳ情報に格納されているスレ数を応答
    if ((index != NNSH_SELBBS_FAVORITE)&&
        (index != NNSH_SELBBS_GETALL)&&
        (index != NNSH_SELBBS_NOTREAD))
    {
        ret = Get_BBS_Info(index, &bbsData);
        if (ret != errNone)
        {
            goto FUNC_END;
        }

        // スレ数が、「要再計算」となっていた場合には、再計算する
        *count = bbsData.threadCnt;
        if (bbsData.threadCnt == NNSH_BBSLIST_AGAIN)
        {
            // スレ数を数え、更新
            (void) Count_Thread_BBS(bbsData.boardNick, count);
            if (*count != NNSH_BBSLIST_AGAIN)
            {
                bbsData.threadCnt = *count;
                Update_BBS_Info(index, &bbsData);
            }
        }
        goto FUNC_END;
    }

    // ...お気に入り/全スレ取得の場合...

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &dataCnt);

    // お気に入りスレ/取得済みスレが何個あるか数える
    for (loop = dataCnt; loop > 0; loop--)
    {
        // MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
        GetRecord_NNsh(dbRef, (loop - 1), sizeof(NNshSubjectDatabase), &tmpDb);
        switch (index)
        {
          case NNSH_SELBBS_GETALL:
            // 取得済みスレの数を数える
            if (tmpDb.state != NNSH_SUBJSTATUS_NOT_YET)
            {
                (*count)++;
            }
            break;

          case NNSH_SELBBS_NOTREAD:
            // 未読ありスレの数を数える
            if ((tmpDb.state == NNSH_SUBJSTATUS_NEW)||
                (tmpDb.state == NNSH_SUBJSTATUS_UPDATE)||
                (tmpDb.state == NNSH_SUBJSTATUS_REMAIN))
            {
                (*count)++;
            }
            break;

          case NNSH_SELBBS_FAVORITE:
          default:
            // お気に入りスレの数を数える
            if ((tmpDb.msgAttribute & NNSH_MSGATTR_FAVOR) == 
                                                            NNSH_MSGATTR_FAVOR)
            {
                (*count)++;
            }
            break;
        }
    }
    CloseDatabase_NNsh(dbRef);
    ret = errNone;

FUNC_END:
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   check_same_thread                                        */
/*                                      同じスレがDBに含まれているか調べる */
/*-------------------------------------------------------------------------*/
Err check_same_thread(UInt16 selMES, NNshSubjectDatabase *data, 
                      NNshSubjectDatabase *matchedData, UInt16 *idx)
{
    Err                  err = ~errNone;
    UInt16               lp, *idxP;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subj;

    // 取得したデータベースの情報を格納する領域を確保
    subj = MemPtrNew(sizeof(NNshSubjectDatabase));
    if (subj == NULL)
    {
        NNsh_DebugMessage(ALTID_INFO, "MemPtrNew()", "", 
                          sizeof(NNshSubjectDatabase));
        return (~errNone);
    }

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_INFO, "OpenDatabase_NNsh()\n:", "", 0);
        goto FUNC_END;
    }
    if (NNshGlobal->threadIdxH == 0)
    {
        goto FUNC_END;
    }
    idxP = MemHandleLock(NNshGlobal->threadIdxH);
    *idx = selMES;
    for (lp = 0; lp < selMES; lp++)
    {
        // スレデータを取得
        if ((GetRecord_NNsh(dbRef, idxP[lp], 
                            sizeof(NNshSubjectDatabase), subj)) != errNone)
        {
            // エラー発生
            err = ~errNone;
            break;
        }
        if ((StrCompare(subj->boardNick,      data->boardNick)      == 0)&&
            (StrCompare(subj->threadFileName, data->threadFileName) == 0))
        {
            // 同一のスレが存在した！
            err  = errNone;
            NNsh_DebugMessage(ALTID_INFO, subj->threadFileName, 
                              data->threadFileName, lp);
            if (matchedData != NULL)
            {
                // 該当したデータをコピーして応答する
	        MemSet(matchedData, sizeof(NNshSubjectDatabase), 0x00);
                StrCopy(matchedData->threadFileName, subj->threadFileName);
                StrCopy(matchedData->threadTitle,    subj->threadTitle);
                StrCopy(matchedData->boardNick,      subj->boardNick);
                matchedData->dirIndex     = subj->dirIndex;
                matchedData->reserved     = subj->reserved;
                matchedData->state        = subj->state;
                matchedData->msgAttribute = subj->msgAttribute;
                matchedData->msgState     = subj->msgState;
                matchedData->bbsType      = subj->bbsType;
                matchedData->maxLoc       = subj->maxLoc;
                matchedData->currentLoc   = subj->currentLoc;
                matchedData->fileSize     = subj->fileSize;
            }
            *idx = lp;
            break;
        }
    }
    MemHandleUnlock(NNshGlobal->threadIdxH);
    CloseDatabase_NNsh(dbRef);
FUNC_END:
    MemPtrFree(subj);
    return (err);
}
