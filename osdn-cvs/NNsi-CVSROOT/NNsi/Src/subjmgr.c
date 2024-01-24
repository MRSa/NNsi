/*============================================================================*
 *
 *  $Id: subjmgr.c,v 1.39 2005/02/10 14:56:42 mrsa Exp $
 *
 *  FILE: 
 *     subjmgr.c
 *
 *  Description: 
 *     スレ一覧取得サブ
 *===========================================================================*/
#define  SUBJMGR_C
#include "local.h"

/*=========================================================================*/
/*   Function :   DeleteSubjectList                                        */
/*                                                  該当するスレ一覧を削除 */
/*=========================================================================*/
Err DeleteSubjectList(Char *boardNick, UInt16 delState, UInt16 *thrCnt)
{
    UInt16              cnt, loop, isOffline;
    DmOpenRef           dbRef;
    NNshSubjectDatabase tmpDb;
    Char                fileName[MAXLENGTH_FILENAME];

    isOffline = StrLen(FILE_LOGCHARGE_PREFIX);

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

    // GetRecord_NNsh の返り値を用いる事で高速化させる
    // 初期化すらいらないはずだが、念のため、１回初期化させる
    MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
    
    for (loop = cnt; loop > 0; loop--)
    {
        if (GetRecord_NNsh(dbRef, (loop - 1), sizeof(NNshSubjectDatabase), &tmpDb) == errNone)
        {
            // 指定されたBoardIDだったらレコード削除
            if ((delState == NNSH_SUBJSTATUS_DELETE_OFFLINE)&&
                (StrNCompare(tmpDb.threadFileName, FILE_LOGCHARGE_PREFIX, isOffline) == 0))
            {
                // 削除しなかったときは、カウントアップ
                (*thrCnt)++;
            }
            else if ((delState == NNSH_SUBJSTATUS_UNKNOWN)||
                      (StrCompare(tmpDb.boardNick, boardNick) == 0))
            {
                //  スレ取得予約されていなくて、未読データ or 
                // 全削除指示だったら削除
                if ((delState == NNSH_SUBJSTATUS_DELETE)||
                     (delState == NNSH_SUBJSTATUS_DELETE_OFFLINE)||
                    (((tmpDb.msgAttribute & NNSH_MSGATTR_GETRESERVE) == 0)&&
                     (tmpDb.state == NNSH_SUBJSTATUS_NOT_YET)))
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
    }
    Hide_BusyForm(true);
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));
    return (errNone);
}

/*==========================================================================*/
/*  SortSubjectList : SUBJECTデータベースをソートし上書きする               */
/*                                                                          */
/*==========================================================================*/
void SortSubjectList(void)
{
    DmOpenRef            dbRef;

    // スレ名管理DBオープンし、ソートする
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_SUBJECT, 0);
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  entryNotExistSubject                                      */
/*                                        スレタイトルが存在しなければ登録 */
/*-------------------------------------------------------------------------*/
static void entryNotExistSubject(DmOpenRef dbRef, NNshSubjectDatabase *subjDb)
{
    UInt16               loop, nofRecords;
    MemHandle            recH;
    NNshSubjectDatabase *chkDb;

    // 登録済みレコード数を獲得
    GetDBCount_NNsh(dbRef, &nofRecords);

    // 登録済みか全件(総当りで)チェックする
    for (loop = 0; loop < nofRecords; loop++)
    {
        (void) GetRecordReadOnly_NNsh(dbRef, loop, &recH, (void **) &chkDb);

        // 登録済みかどうかチェック 
        if (StrCompare(chkDb->threadFileName, subjDb->threadFileName) == 0)
        {
            if (StrCompare(chkDb->boardNick, subjDb->boardNick) == 0)
            {
                // ボードファイル名とボードニックネームが同じ、すでに登録済み
                ReleaseRecordReadOnly_NNsh(dbRef, recH);
                return;
            }
        }
        ReleaseRecordReadOnly_NNsh(dbRef, recH);
    }

    // スレが登録されていなかった、レコードを新規登録する
    (void) EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), subjDb);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   parse_SubjectList                                        */
/*                                                    スレ一覧のDB取り込み */
/*-------------------------------------------------------------------------*/
static Err parse_SubjectList(Char *buffer, UInt32 readSize, UInt32 *parseSize,
                              UInt16 bbsType, Char *boardNick, UInt16 *cnt)
{
    UInt16              size, len, sepaLen;
    Char               *ptr, *dataEnd, *dataSepa, *ppp, *src, *dst;
    Char               *fieldSeparator, *titleBuf;
    DmOpenRef           dbRef, threadDbRef;
    NNshSubjectDatabase tmpDb;
    MemHandle           threadListH = NULL;
    Char                *threadListP;
    Boolean             haveThreadList = false;
    UInt16              threadNum, checkDuplicateMethod, mesNum;
    
    titleBuf = MEMALLOC_PTR(BUFSIZE + MARGIN);
    if (titleBuf == NULL)
    {
        // タイトルバッファ格納領域確保失敗
        *parseSize = readSize;
        return (~errNone);
    }

    // BBSタイプによって、フィールドセパレータを選択する
    switch (bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // まちBBS または したらば@JBBS
        fieldSeparator = NNSH_FIELD_SEPARATOR_MACHI;
        break;

#if 0
      case NNSH_BBSTYPE_SHITARABA:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // ２ちゃんねる または したらば
        fieldSeparator = NNSH_FIELD_SEPARATOR;
        break;
    }
    sepaLen = StrLen(fieldSeparator);

    // 初回のparse時のみHTTPヘッダを読み飛ばす
    if (*parseSize == 0)
    {
        // スレ名称の解釈メイン(HTTPヘッダを読み飛ばし)
        ptr = StrStr(buffer, NNSH_HTTP_HEADER_ENDTOKEN);
        if (ptr == NULL)
        {
            // データ異常
            NNsh_DebugMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                               NNSH_HTTP_HEADER_ENDTOKEN, 0);
            MEMFREE_PTR(titleBuf);
            *parseSize = readSize;
            return (~errNone);
        }
        ptr = ptr + 4;   // 4は、"\x0d\x0a\x0d\0a"
    }
    else
    {
        // 2回目以降のparse...
        ptr = buffer;
    }

    // 「リスト登録中」の表示を行う
    Show_BusyForm(MSG_THREADLIST_CREATE);

    // DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生
        MEMFREE_PTR(titleBuf);
        *parseSize = readSize;
        return (~errNone);
    }

    checkDuplicateMethod = NNSH_DUP_NOCHECK;
    if ((NNshGlobal->NNsiParam)->checkDuplicateThread != 0)
    {
        // スレの重複チェックありの場合...
        // スレのリストを取得する
        haveThreadList = 
                GetExistRecordList(dbRef, boardNick, &threadListH, &threadNum);
        if (haveThreadList == false)
        {
            // 登録済みＤＢのレコード件数を確認
            if (threadNum != 0)
            {
                //  メモリ不足発生、遅くなっても重複チェックするかどうか
                // 確認を求めるメッセージを表示する
                if (NNsh_ErrorMessage(ALTID_CONFIRM, MSG_SELECT_DUPLICATE_METHOD, "", 0) == 0)
                {
                    // 遅くなっても、重複チェックする(低速重複チェック
                    checkDuplicateMethod = NNSH_DUP_CHECK_SLOW;
                }
                else
                {
                    // 遅くなるため、重複チェックしない
                    checkDuplicateMethod = NNSH_DUP_NOCHECK;
                }
            }
            else
            {
                // DBに１件もデータがないため、重複チェックしない
                checkDuplicateMethod = NNSH_DUP_NOCHECK;
            }
        }
        else 
        {
            // 重複チェックの実施
            checkDuplicateMethod = NNSH_DUP_CHECK_HIGH;
        }
    }

    // スレタイが登録されていた、登録と同時に新着確認する？
    if ((NNshGlobal->NNsiParam)->listAndUpdate != 0)
    {
        // スレ番指定取得用DBをオープンしておく
        OpenDatabase_NNsh(DBNAME_GETRESERVE,DBVERSION_GETRESERVE, &threadDbRef);
    }
    else
    {
        threadDbRef = 0;
    }

    *cnt = 0;
    *parseSize = 0;
    while (ptr < buffer + readSize)
    {
        //  レコードデータの末尾を検索する（NNSH_RECORD_SEPARATOR == "\x0a"）
        dataEnd = ptr;
        while ((*dataEnd != '\x0a')&&(*dataEnd != '\0'))
        {
            dataEnd++;
        }
        if (*dataEnd == '\0')
        {
            // データ終了、ループを抜ける
            dataEnd = NULL;
            break;
        }

        // フィールドのセパレータを検索する
        dataSepa = StrStr(ptr, fieldSeparator);
        if ((dataSepa == NULL)||(dataSepa > dataEnd))
        {
            // データ異常、ループを抜ける
            break;
        }

        // レス数格納部分を抽出する
        MemSet  (&tmpDb, sizeof(tmpDb), 0x00);

        // BBSタイプ単位でレス数、スレタイ、ファイル名を格納する
        switch (bbsType)
        {
          case NNSH_BBSTYPE_SHITARABA:
            // レス数(したらばだけ他とレス数の格納方法が違う、注意)
            ppp = StrStr(dataSepa, fieldSeparator);
            ppp = ppp + StrLen(fieldSeparator);
            ppp = StrStr(ppp, fieldSeparator);
            if (ppp != NULL)
            {
                *ppp = '\0';
                ppp = ppp + StrLen(fieldSeparator);
            }
            while ((dataEnd > ppp)&&(*ppp < '0')&&(*ppp > '9'))
            {
                ppp++;
            }
            tmpDb.maxLoc = (UInt16) StrAToI(ppp);

            // スレファイル名
            StrNCopy(tmpDb.threadFileName, ptr, (dataSepa - ptr));
            size = ((dataEnd - (dataSepa + 2)) > MAX_THREADNAME) ? 
                             (MAX_THREADNAME - 1) : (dataEnd - (dataSepa + 2));

            // したらばのsubject.txtには拡張子が入っていないので、ここで付加
            StrCat(tmpDb.threadFileName, ".dat");

            // スレタイトル（EUC漢字コードをSHIFT JIS漢字コードに変換する）
            MemSet  (titleBuf, (BUFSIZE + MARGIN), 0x00);
            StrNCopy(titleBuf, (dataSepa + sepaLen), size);
            src = titleBuf;
            dst = tmpDb.threadTitle;
            while (*src != '\0')
            {
                if (ConvertEUCtoSJ(dst, src, &len) == true)
                {
                    src = src + 2;
                    dst = dst + len;
                }
                else
                {
                    *dst = *src;
                    dst++;
                    src++;
                }
            }
            break;

          case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
          case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
          case NNSH_BBSTYPE_2ch_EUC:
            /** レス数格納部分を削除(でも遅くなるなー) **/
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
            tmpDb.maxLoc = (UInt16) StrAToI(ppp + 1);

            // ファイル名
            StrNCopy(tmpDb.threadFileName, ptr, (dataSepa - ptr));
            size = ((dataEnd - (dataSepa + 2)) > MAX_THREADNAME) ? 
                             (MAX_THREADNAME - 1) : (dataEnd - (dataSepa + 2));

            // スレタイトル(EUC漢字コードだった場合は、SHIFT JISに変換)
            MemSet(titleBuf, (BUFSIZE + MARGIN), 0x00);
            StrNCopy(titleBuf, (dataSepa + sepaLen), size);
            src = titleBuf;
            dst = tmpDb.threadTitle;
            while (*src != '\0')
            {
                if (ConvertEUCtoSJ(dst, src, &len) == true)
                {
                    src = src + 2;
                    dst = dst + len;
                }
                else
                {
                    *dst = *src;
                    dst++;
                    src++;
                }
            }
            break;

#if 0
          case NNSH_BBSTYPE_MACHIBBS:
          case NNSH_BBSTYPE_2ch:
          case NNSH_BBSTYPE_OTHERBBS:
          case NNSH_BBSTYPE_OTHERBBS_2:
#endif
          default:
            /** レス数格納部分を削除(でも遅くなるなー) **/
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
            tmpDb.maxLoc = (UInt16) StrAToI(ppp + 1);

            // ファイル名
            StrNCopy(tmpDb.threadFileName, ptr, (dataSepa - ptr));
            size = ((dataEnd - (dataSepa + 2)) > MAX_THREADNAME) ? 
                             (MAX_THREADNAME - 1) : (dataEnd - (dataSepa + 2));

            // スレタイトル（SHIFT JIS漢字コードだった場合は、そのまま格納）
            StrNCopy(tmpDb.threadTitle, (dataSepa + sepaLen), size);
            break;
        }

        // スレタイトル一覧にBBS TYPEを格納する。
        tmpDb.bbsType = (UInt8) bbsType; 
        StrCopy (tmpDb.boardNick, boardNick);

        // 重複チェックの実施
        switch(checkDuplicateMethod)
        {
          case NNSH_DUP_CHECK_SLOW:
            // 重複チェック実施(低速版)、スレタイが存在しなければ登録する
            (void) entryNotExistSubject(dbRef, &tmpDb);
            break;

          case NNSH_DUP_CHECK_HIGH:
            // 重複チェック(高速版)を実施
            threadListP = MemHandleLock(threadListH);
            ppp = StrStr(threadListP, tmpDb.threadFileName);
            if(ppp == NULL)
            {
                // スレタイ未登録、新規登録する
                (void)
                  EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &tmpDb);
            }
            else
            {
                // スレタイが登録されていた、登録と同時に新着確認する？
                if ((NNshGlobal->NNsiParam)->listAndUpdate != 0)
                {
                    // 登録数と今回のレス数をチェックする
                    mesNum = StrAToI(ppp + StrLen(tmpDb.threadFileName));
                    if ((mesNum != 0)&&(mesNum != tmpDb.maxLoc))
                    {
                        // 新着確認を実施するスレ発見！(スレ番抽出)
                        MemSet (titleBuf, (BUFSIZE + MARGIN), 0x00);
                        StrCopy(titleBuf, tmpDb.boardNick);
                        StrCat (titleBuf, tmpDb.threadFileName);
                        ppp = titleBuf + StrLen(titleBuf);
                        while ((ppp > titleBuf)&&(*ppp != '.'))
                        {
                            ppp--;
                        }
                        if (ppp != titleBuf)
                        {
                            *ppp = '\0';
                        }             
                        if (threadDbRef != 0)
                        {
                            // スレ番指定取得予約ＤＢにエントリする
                            (void) EntryRecord_NNsh(threadDbRef, BUFSIZE, titleBuf);
                        }
                    }                    
                }
            }
            MemHandleUnlock(threadListH);
            break;

          case NNSH_DUP_NOCHECK:
          default:
            // 重複チェックは行わず、スレタイをすぐに(新規)登録する
            (void)EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &tmpDb);
            break;
        }

        // 次データの先頭へ移動
        ptr = dataEnd + 1;
        (*cnt)++;

        *parseSize = (ptr - buffer);
    }
    // 重複確認用が生成されていた場合、領域を開放する。
    if (haveThreadList != false)
    {
        MemHandleFree(threadListH);
    }
    Hide_BusyForm(true);
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    // スレ番指定取得ＤＢがオープンされていたときにはクローズする
    if (threadDbRef != 0)
    {
        CloseDatabase_NNsh(threadDbRef);
    }

    MEMFREE_PTR(titleBuf);
    return (errNone);
}

/*=========================================================================*/
/*   Function :   convertListIndexToMsgIndex                               */
/*                                   リスト番号からスレindex番号に変換する */
/*=========================================================================*/
UInt16 convertListIndexToMsgIndex(UInt16 listIdx)
{
    // リスト番号をDBのレコード番号に変換して応答する
    return (NNshGlobal->msgListIndex[listIdx]);
}

/*=========================================================================*/
/*   Function :   NNsh_GetSubjectList                                      */
/*                                                         スレ一覧の取得  */
/*=========================================================================*/
Err NNsh_GetSubjectList(UInt16 index)
{
    Err                 ret, ret2;
    UInt16              cnt, cnt2;
    UInt32              fileSize, readSize, topSize, parseSize, getSize;
    Char                getURL[MAX_URL + MAX_URL], *buffer;
    NNshFileRef         fileRef;
    NNshBoardDatabase  *bbsData;

    // BBS情報格納領域を確保する
    bbsData = MEMALLOC_PTR(sizeof(NNshBoardDatabase) + MARGIN);
    if (bbsData == NULL)
    {
        return (~errNone);
    }

    // BBS-DBからBBS情報を取得
    ret = Get_BBS_Info(index, bbsData);
    if (ret != errNone)
    {
        // エラー発生
        NNsh_DebugMessage(ALTID_ERROR,  "Get_BBS_Info() ", "", ret);
        MEMFREE_PTR(bbsData);
        return (ret);
    }

    // データ取得先の設定
    MemSet (getURL, sizeof(getURL), 0x00);
    StrCopy(getURL, bbsData->boardURL);
    switch (bbsData->bbsType)
    {
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
      case NNSH_BBSTYPE_MACHIBBS:
        // したらば@JBBS/まちBBSのときは、BoardNickを付加
        StrCat(getURL, bbsData->boardNick);
        break;

      case NNSH_BBSTYPE_SHITARABA:
        // したらばのときは、bbs/BoardNickを付加
        StrCat(getURL, "bbs/");
        StrCat(getURL, bbsData->boardNick);
        break;

#if 0
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // ２ちゃんねるの時は、なにもしない
        break;
    }
    StrCat (getURL, FILE_THREADLIST);

    // デバッグ表示
    NNsh_DebugMessage(ALTID_INFO, bbsData->boardNick, getURL, 0);

    // ホストからスレ一覧データを取得
    if ((NNshGlobal->NNsiParam)->getAllThread == HTTP_GETSUBJECT_PART)
    {
        // 一部取得
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, getURL, NULL, NULL,
                            HTTP_GETSUBJECT_START, HTTP_GETSUBJECT_LEN, NULL);
    }
    else
    {
        // 全部取得
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, getURL, NULL, NULL,
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    }
    if (ret != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,MSG_ERROR_HTTP_COMM, "[SUBJ]", ret);
            MEMFREE_PTR(bbsData);
            return (ret);
        }

        // 通信タイムアウト発生(受信しているところまで、強制的に解釈を行う)
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[SUBJ]", ret);
    }

    // データベースから、(取得する)スレデータを未読のもののみ消去する
    DeleteSubjectList(bbsData->boardNick, NNSH_SUBJSTATUS_NOT_YET, &cnt);

    ////////// スレ一覧の解析 /////////

    // 受信データ格納用バッファの獲得
    buffer = (Char *) MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR,
                           "MEMALLOC_PTR()", " size:", (NNshGlobal->NNsiParam)->bufferSize);
        MEMFREE_PTR(bbsData);
        return (~errNone);
    }
    MemSet(buffer, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    OpenFile_NNsh(FILE_RECVMSG, 
                  (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                  &fileRef);
    GetFileSize_NNsh(&fileRef, &fileSize);

    // スレ読み込みサイズ三兄弟の初期化
    topSize   = 0;
    parseSize = topSize;
    getSize   = 0;

    // スレ一覧の読みこみ
READ_AGAIN:
    ret = ReadFile_NNsh(&fileRef, topSize, (NNshGlobal->NNsiParam)->bufferSize,
                        buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        // ファイル読みこみエラー
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :", FILE_RECVMSG, ret);
        ret2 = ret;
        getSize = fileSize;
    }
    else
    {
        // 板情報データベースの更新(スレの登録)
        ret2 = parse_SubjectList(buffer, readSize, &parseSize,
                                bbsData->bbsType, bbsData->boardNick, &cnt2);
        cnt = cnt + cnt2;
    }
    topSize = topSize + parseSize;
    getSize = getSize + readSize;
    if (getSize < fileSize)
    {
        // レス数解析がすべて済んでいない場合には、もう一度読み直し
        MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
        parseSize = topSize;
        goto READ_AGAIN;
    }
    CloseFile_NNsh(&fileRef);
    MEMFREE_PTR(buffer);
    MEMFREE_PTR(bbsData);
    return (ret2);   
}

/*=========================================================================*/
/*   Function :   Check_same_thread                                        */
/*                                      同じスレがDBに含まれているか調べる */
/*=========================================================================*/
Err Check_same_thread(UInt16 selMES, NNshSubjectDatabase *data, 
                      NNshSubjectDatabase *matchedData, UInt16 *idx)
{
    Err                  err = ~errNone;
    Int16                step;
    UInt16               lp, index, count;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subj;

    // 取得したデータベースの情報を格納する領域を確保
    subj = MEMALLOC_PTR(sizeof(NNshSubjectDatabase));
    if (subj == NULL)
    {
        NNsh_DebugMessage(ALTID_INFO, "MEMALLOC_PTR()", "", 
                          sizeof(NNshSubjectDatabase));
        return (~errNone);
    }

    // 「重複スレのチェック中...」表示
    Show_BusyForm(MSG_CHECK_SAME_THREAD);

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // DBオープン失敗(ありえないはずだが...)
        goto FUNC_END;
    }
    GetDBCount_NNsh(dbRef, &count);

    // 取得予定の番号を取得
    if (selMES != NNSH_NOTENTRY_THREAD)
    {
        index = convertListIndexToMsgIndex(selMES);
        step  = 1;
        lp    = index + 1;
    }
    else
    {
        index = count;
        step  = -1;
        lp    = count - 1;
    }
    *idx  = index;

    // 重複スレかどうか検索する
    for (;((lp >= 0)&&(lp < count)); lp = lp + step)
    {
        // スレデータを取得
        if ((GetRecord_NNsh(dbRef, lp,
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
    CloseDatabase_NNsh(dbRef);

    // 「重複スレのチェック中...」表示を消す
    Hide_BusyForm(false);

FUNC_END:
    MEMFREE_PTR(subj);
    return (err);
}

/*=========================================================================*/
/*   Function :   GetSubjectIndex                                          */
/*                                        スレＩＤのインデックス番号を取得 */
/*=========================================================================*/
Err GetSubjectIndex(Char *boardNick, Char *threadFileName, UInt16 *index)
{
    Err                 err = ~errNone;
    NNshSubjectDatabase subj;
    DmOpenRef           dbRef;
    Int16               lp;
    UInt16              count;

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // DBオープン失敗(ありえないはずだが...)
        return (~errNone - 10);
    }
    GetDBCount_NNsh(dbRef, &count);

    // スレが存在するか確認する
    for (lp = (count - 1); lp >= 0; lp--)
    {
        // スレデータを取得
        if ((GetRecord_NNsh(dbRef, lp,
                            sizeof(NNshSubjectDatabase), &subj)) != errNone)
        {
            // エラー発生
            err = ~errNone;
            break;
        }
        if ((StrCompare(subj.boardNick,      boardNick)      == 0)&&
            (StrCompare(subj.threadFileName, threadFileName) == 0))
        {
            // スレが存在した！
            err    = errNone;
            *index = lp;
            break;
        }
    }
    CloseDatabase_NNsh(dbRef);
    return (err);
}


/*=========================================================================*/
/*   Function : CheckIfCustomTabIsValid                                    */
/*                                              ユーザタブが有効かチェック */
/*=========================================================================*/
Boolean CheckIfCustomTabIsValid(Char *bbsName, NNshSubjectDatabase *subjP,
                                NNshCustomTab *customTab)
{
    Boolean subRet, ret;
    UInt16  condition, thLevel, favorLvl, status;
    UInt32  currentTime, thrTime;

    ret = true;

    if ((customTab->condition & NNSH_CONDITION_NOTREADONLY) != 0)
    {
        // 参照ログは除外する場合
#ifdef USE_STRSTR
        if (!(StrCompare(subjP->boardNick, OFFLINE_THREAD_NICK)))
#else
        if ((subjP->boardNick[0]  == '!')&&
            (subjP->boardNick[1]  == 'G'))
#endif
        {
            // 参照ログだった場合(条件から除外)
            return (false);
        }
    }

    // 全て一致 or どれか一致 の条件を取得
    condition = (customTab->condition & NNSH_CONDITION_ALL);

    ////////////////////////////////////////////////////////////
    // 板設定情報の確認
    ////////////////////////////////////////////////////////////
    if ((customTab->boardCondition != 0)&&(bbsName != NULL))
    {
        status = customTab->boardCondition - 1;
        switch (status)
        {
          case NNSH_BOARD_ELSE:
            // 板が一致しない(以外)
            if ((StrCompare(bbsName, subjP->boardNick)) == 0)
            {
                // 板名が一致、除外
                ret = false;
                goto CHECK_LEVEL;
            }
            break;

          case NNSH_BOARD_MATCH:
          default:
            // 板が一致(内)
            if ((StrCompare(bbsName, subjP->boardNick)) != 0)
            {
                // 板名が一致しない、除外
                ret = false;
                goto CHECK_LEVEL;
            }
            break;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            return (true);
        }
    }

CHECK_LEVEL:
    ////////////////////////////////////////////////////////////
    // スレレベルの確認
    ////////////////////////////////////////////////////////////
    status = 
      (customTab->threadLevel & NNSH_THREADCOND_MASK) >> NNSH_THREADCOND_SHIFT;
    if (status != 0)
    {
        favorLvl  = (subjP->msgAttribute & NNSH_MSGATTR_FAVOR);
        thLevel   = customTab->threadLevel & NNSH_THREADLEVEL_MASK;

        status--;
        switch (status)
        {
          case NNSH_THREADLEVEL_LOWER:
            // 以下
            if (favorLvl > thLevel)
            {
                // レベルが大きければ除外
                ret = false;
                goto CHECK_LOGLOC;
            }
            break;
          case NNSH_THREADLEVEL_ELSE:
            // 以外
            if (favorLvl == thLevel)
            {
                // レベルが同じならば除外
                ret = false;
                goto CHECK_LOGLOC;
            }
            break;
          case NNSH_THREADLEVEL_MATCH:
            // 一致
            if (favorLvl != thLevel)
            {
                // レベルが違ったら除外
                ret = false;
                goto CHECK_LOGLOC;
            }
            break;
          case NNSH_THREADLEVEL_UPPER:
          default:
            // 以上
            if (favorLvl < thLevel)
            {
                // レベルが小さければ除外
                ret = false;
                goto CHECK_LOGLOC;
            }
            break;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            // "どれか一致" の時
            return (true);
        }
    }

CHECK_LOGLOC:
    ////////////////////////////////////////////////////////////
    // ログ位置の確認
    ////////////////////////////////////////////////////////////
    status = (customTab->condition & NNSH_CONDITION_LOGLOC_ALL);
    if (status != 0)
    {
        if ((status == NNSH_CONDITION_LOGLOC_PALM)&&
            (subjP->msgState != FILEMGR_STATE_OPENED_STREAM))
        {
            // ログファイルがPalmデバイス内に存在しないとき
            ret = false;
            goto CHECK_GETERR;
        }
        else if ((status == NNSH_CONDITION_LOGLOC_VFS)&&
                 (subjP->msgState != FILEMGR_STATE_OPENED_VFS))
        {
            // ログファイルがＶＦＳ内に存在しないとき
            ret = false;
            goto CHECK_GETERR;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            // "どれか一致" の時
            return (true);
        }
    }

CHECK_GETERR:
    ////////////////////////////////////////////////////////////
    // 取得エラーの確認
    ////////////////////////////////////////////////////////////
    status = (customTab->condition & NNSH_CONDITION_GETERR_ALL);
    if (status != 0)
    {
        if ((status == NNSH_CONDITION_GETERR_NONE)&&
            ((subjP->msgAttribute & NNSH_MSGATTR_ERROR) != 0))
        {
            // エラー状態のとき
            ret = false;
            goto CHECK_GETRESERVE;
        }
        else if ((status == NNSH_CONDITION_GETERR_ERROR)&&
                 ((subjP->msgAttribute & NNSH_MSGATTR_ERROR) == 0))
        {
            // エラー状態でないとき
            ret = false;
            goto CHECK_GETRESERVE;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            // "どれか一致" の時
            return (true);
        }
    }

CHECK_GETRESERVE:
    ////////////////////////////////////////////////////////////
    // 取得保留状態の確認
    ////////////////////////////////////////////////////////////
    status = (customTab->condition & NNSH_CONDITION_GETRSV_ALL);
    if (status != 0)
    {
        if ((status == NNSH_CONDITION_GETRSV_RSV)&&
            (subjP->msgAttribute & NNSH_MSGATTR_GETRESERVE) != NNSH_MSGATTR_GETRESERVE)
        {
            // 取得保留中
            ret = false;
            goto CHECK_STATUS;
        }
        else if ((status == NNSH_CONDITION_GETRSV_NONE)&&
                 (subjP->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
        {
            // 非取得保留中
            ret = false;
            goto CHECK_STATUS;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            // "どれか一致" の時
            return (true);
        }
    }

CHECK_STATUS:
    ////////////////////////////////////////////////////////////
    // スレ状態の確認
    ////////////////////////////////////////////////////////////
    if (customTab->threadStatus != 0)
    {
        subRet = true;

        // 新規取得スレ
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_NEW) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_NEW))
        {
            goto CHECK_THREADSTATUS;
        }

        // 更新スレ
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_UPDATE) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_UPDATE))
        {
            goto CHECK_THREADSTATUS;
        }

        // 未読ありスレ
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_REMAIN) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_REMAIN))
        {
            goto CHECK_THREADSTATUS;
        }

        // 既読スレ
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_ALREADY) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_ALREADY))
        {
            goto CHECK_THREADSTATUS;
        }

        // 最大を超えたスレ
        if (((customTab->threadStatus >> NNSH_SUBJSTATUS_OVER) & 1)!= 0)
        {
            if ((subjP->maxLoc > NNSH_MESSAGE_LIMIT)&&
                (subjP->state == NNSH_SUBJSTATUS_ALREADY))
            {
                goto CHECK_THREADSTATUS;
            }
        }

        // まだ取得していないスレ
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_NOT_YET) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_NOT_YET))
        {
            goto CHECK_THREADSTATUS;
        }

        // 取得エラー？
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_GETERROR) & 1)!= 0)&&
            ((subjP->msgAttribute & NNSH_MSGATTR_ERROR) != 0))
        {
            goto CHECK_THREADSTATUS;
        }

        // その他スレ
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_UNKNOWN) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_UNKNOWN))
        {
            goto CHECK_THREADSTATUS;
        }

        subRet = false;

CHECK_THREADSTATUS:
        if (subRet == true)
        {
            if (condition == NNSH_CONDITION_OR)
            {
                // "どれか一致" の時
                return (true);
            }
        }
        else
        {
            ret = false;
        }
    }

    // スレ立てｎ日以内のチェック
    if (customTab->threadCreate != 0)
    {
        // チェック時刻の取得
        currentTime = TimGetSeconds()
                             - TIME_CONVERT_1904_1970
                             - StrAToI(subjP->threadFileName);
        thrTime = customTab->threadCreate;
        thrTime = thrTime * 60 * 60 * 24;
        if (currentTime < thrTime)
        {
                // n日以内に立ったスレと認識
            // NNsh_DebugMessage(ALTID_INFO, "Time:", "", currentTime);
            if (condition == NNSH_CONDITION_OR)
            {
                // "どれか一致" の時
                return (true);
            }
        }
        else
        {
            ret = false;
        }
    }

    ////////////////////////////////////////////////////////////
    // 検索文字列のチェック
    ////////////////////////////////////////////////////////////
    if ((customTab->stringSet & NNSH_STRING_SELECTION) != 0)
    {
        status = customTab->stringSet & NNSH_STRING_SETMASK;
        subRet = true;

        // 検索文字列のチェック
        if (customTab->string1[0] != '\0')
        {
            if (StrStr(subjP->threadTitle, customTab->string1) != NULL)
            {
                if (status == NNSH_CONDITION_OR)
                {
                    // "どれか一致" の時
                    goto CHECK_STRINGSET;
                }
            }
            else
            {
                    subRet = false;
            }
        }

        // 検索文字列のチェック
        if (customTab->string2[0] != '\0')
        {
            if (StrStr(subjP->threadTitle, customTab->string2) != NULL)
            {
                if (status == NNSH_CONDITION_OR)
                {
                    // "どれか一致" の時
                    subRet = true;
                    goto CHECK_STRINGSET;
                }
            }
            else
            {
                    subRet = false;
            }
        }

        // 検索文字列のチェック
        if (customTab->string3[0] != '\0')
        {
            if (StrStr(subjP->threadTitle, customTab->string3) != NULL)
            {
                if (status == NNSH_CONDITION_OR)
                {
                    // "どれか一致" の時
                    subRet = true;
                    goto CHECK_STRINGSET;
                }
            }
            else
            {
                subRet = false;
            }
        }
        
CHECK_STRINGSET:
        if (subRet == true)
        {
            if (condition == NNSH_CONDITION_OR)
            {
                // "どれか一致" の時
                return (true);
            }
        }
        else
        {
            ret = false;
        }
    }
    return (ret);
}
