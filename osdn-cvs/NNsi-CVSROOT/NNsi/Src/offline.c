/*============================================================================*
 *
 *  $Id: offline.c,v 1.14 2005/01/23 15:21:33 mrsa Exp $
 *
 *  FILE: 
 *     offline.c
 *
 *  Description: 
 *     Offline Thread manager for NNsh. 
 *
 *===========================================================================*/
#define OFFLINE_C
#include "local.h"

static void setVFSOfflineThreadInfo(DmOpenRef dbRef, DmOpenRef dirDBRef,
                                    UInt16 nofItems, UInt16 *recList,
                                    UInt16 depth, UInt16 index, Char *dirName,
                                    NNshSubjectDatabase *subDb, UInt16 *cnt);

/*-------------------------------------------------------------------------*/
/*   Function : checkOfflineEntryAvailable                                 */
/*                           OFFLINEスレとしてレコード登録すべきか判定する */
/*-------------------------------------------------------------------------*/
static Err checkOfflineEntryAvailable(DmOpenRef dbRef, UInt16 *recNum,
                                      UInt16 nofItems, UInt16 *recList,
                                      Char *threadFileName, UInt16 msgState,
                                      UInt16 offset)
{
    UInt16               loop, msgStatus;
    MemHandle            mesH;
    NNshSubjectDatabase *mesInfo;

    *recNum = 0;
    for (loop = 0; loop < nofItems; loop++)
    {
        // 参照ログリストを読み出して、登録済みか確認する
        GetRecordReadOnly_NNsh(dbRef, (recList[loop] + offset),
                               &mesH, (void **)&mesInfo);
        msgStatus = mesInfo->msgState;

        // メッセージの格納場所とスレファイル名が同じかどうか確認する
        if ((msgStatus == msgState)&&
            (StrCompare(threadFileName, mesInfo->threadFileName) == 0))
        {
            ReleaseRecordReadOnly_NNsh(dbRef, mesH);
            // すでに登録されていた！エラー応答する
            *recNum = recList[loop] + offset;
            return (~errNone);
        }
        ReleaseRecordReadOnly_NNsh(dbRef, mesH);
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : getOfflineThreadRecords                                    */
/*                                   OFFLINEスレのレコード番号をとってくる */
/*-------------------------------------------------------------------------*/
static Err getOfflineThreadRecords(UInt16 *nofItems, UInt16 **offThread)
{
    Err                  ret;
    DmOpenRef            dbRef;
    UInt16               max, loop, *buffer;
    MemHandle            mesH;
    NNshSubjectDatabase *mesInfo;

    *nofItems  = 0;

    // BUSYウィンドウ表示
    Show_BusyForm(MSG_CHECKOFFLINE_LIST);

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        ret = ~errNone;
        goto FUNC_END;
    }
    // 件数取得
    GetDBCount_NNsh(dbRef, &max);

    // レコード確認域を確保する
    buffer = MEMALLOC_PTR(sizeof(UInt16) * max + MARGIN);
    if (buffer == NULL)
    {
        // 領域確保失敗
        ret = ~errNone - 10;
        goto FUNC_END;
    }
    MemSet(buffer, (sizeof(UInt16) * max + MARGIN), 0x00);

    // OFFLINEスレのレコード番号を検索
    for (loop = 0; loop < max; loop++)
    {
        (void) GetRecordReadOnly_NNsh(dbRef, loop, &mesH, (void **) &mesInfo);

#ifdef USE_STRSTR
        if (StrCompare(mesInfo->boardNick, OFFLINE_THREAD_NICK) == 0)
#else
        if ((mesInfo->boardNick[0]  == '!')&&
            (mesInfo->boardNick[1]  == 'G')&&
            (mesInfo->boardNick[2]  == 'i')&&
            (mesInfo->boardNick[3]  == 'k'))
#endif
        {
            // 参照ログ発見、このレコード番号を記憶する
            buffer[*nofItems] = loop;
            (*nofItems)++;
        }
        ReleaseRecordReadOnly_NNsh(dbRef, mesH);
    }
    CloseDatabase_NNsh(dbRef);

    // 何件かの参照ログを発見、コピーする。
    if (*nofItems != 0)
    {
        *offThread = MEMALLOC_PTR(sizeof(UInt16) * (*nofItems) + MARGIN);
        if (*offThread != NULL)
        {
            for (loop = 0; loop < (*nofItems); loop++)
            {
                (*offThread)[loop] = buffer[loop];
            }
        }
        else
        {
            *nofItems = 0;
        }
    }
    MEMFREE_PTR(buffer);
    ret = errNone;

FUNC_END:
    // BUSYウィンドウを閉じて関数を終了する
    Hide_BusyForm(false);
    return (ret);
}

/*=========================================================================*/
/*   Function : create_offline_database                                    */
/*                                                  OFFLINEスレ情報を作成  */
/*=========================================================================*/
Err create_offline_database(void)
{
    Err                  ret, err;
    DmOpenRef            dbRef, dirRef;
    NNshSubjectDatabase *subjDB;
    NNshBoardDatabase   *bbsDB;
    DmSearchStateType    state;
    UInt16               autoOffTime, cardNo, idxCnt, index, cnt = 0;
    LocalID              dbLocalID;
    Char                *area, buf[4];
    UInt16               nofItems, *offThread, dummy;

    // 領域の確保
    subjDB = MEMALLOC_PTR(sizeof(NNshSubjectDatabase));
    if (subjDB == NULL)
    {
        return (~errNone);
    }

    bbsDB = MEMALLOC_PTR(sizeof(NNshBoardDatabase));
    if (bbsDB == NULL)
    {
        MEMFREE_PTR(subjDB);
        return (~errNone);
    }
    MemSet(bbsDB, sizeof(NNshBoardDatabase), 0x00);

    // 自動電源ＯＦＦタイマを無効にする
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    ret       = ~errNone;
    nofItems  = 0;
    offThread = NULL;
    if ((NNshGlobal->NNsiParam)->notDelOffline != 0)
    {
        //  NNshパラメータで"OFFLINEスレを消さない"が指定されていたときは、
        // 現在登録されているOFFLINEスレのレコード番号を取得する。
        ret = getOfflineThreadRecords(&nofItems, &offThread);
    }
    if (ret != errNone)
    {
        // 最初にOFFLINEのスレを全部消す
        DeleteSubjectList(OFFLINE_THREAD_NICK, NNSH_SUBJSTATUS_DELETE_OFFLINE, &cnt);
    }

    // スレ名管理DBオープン
    Show_BusyForm(MSG_CREATEOFFLINE_LIST);
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        Hide_BusyForm(true);

        ret = ~errNone;
        goto FUNC_END;
    }
    MemSet(&state, sizeof(state), 0x00);
    cnt = 0;

    // OFFLINEスレがVFSに記録されているかどうか確認する
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) == 0)
    {
        /*** OFFLINEスレがVFSに記録されていない場合 ... ***/
        // 初回のデータを取得
        ret = DmGetNextDatabaseByTypeCreator(true, &state, OFFLINE_DBTYPE_ID,
                                             SOFT_CREATOR_ID, false, &cardNo,
                                             &dbLocalID);
        while ((ret == errNone)&&(dbLocalID != 0))
        {
            // 取得したデータを「OFFLINEデータベース」に格納する
            MemSet(subjDB, sizeof(NNshSubjectDatabase), 0x00);
            err = DmDatabaseInfo(cardNo, dbLocalID, subjDB->threadFileName,
                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL);
            if (err == errNone)
            {
                // スレ情報(スレタイトル等)を獲得
                err = SetThreadInformation(subjDB->threadFileName,
                                                  ((NNSH_FILEMODE_READONLY)|
                                                  (NNSH_FILEMODE_TEMPFILE)),
                                                  NULL, subjDB);
                if (err == errNone)
                {
                    // スレデータを登録してもよいか確認する
                    err = 
                       checkOfflineEntryAvailable(dbRef, 
                                                  &dummy,
                                                  nofItems,
                                                  offThread,
                                                  subjDB->threadFileName,
                                                  FILEMGR_STATE_OPENED_STREAM,
                                                  cnt);
                    if (err == errNone)
                    {
                        // その他のスレ情報を設定、データを登録する
                        StrCopy(subjDB->boardNick, OFFLINE_THREAD_NICK);
                        subjDB->state    = NNSH_SUBJSTATUS_UNKNOWN;
                        subjDB->bbsType  = NNSH_BBSTYPE_2ch;
                        subjDB->dirIndex = 0;
                        subjDB->msgState = FILEMGR_STATE_OPENED_STREAM;
                        (void) EntryRecord_NNsh(dbRef, 
                                                sizeof(NNshSubjectDatabase),
                                                subjDB);
                        cnt++;
                    }
                }
            }

            // 二回目以降のデータ取得
            ret = DmGetNextDatabaseByTypeCreator(false, &state, 
                                            OFFLINE_DBTYPE_ID, SOFT_CREATOR_ID,
                                            false, &cardNo, &dbLocalID);
        }
    }
    else
    {
        // ディレクトリインデックスをオープンしてみる
        OpenDatabase_NNsh(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, &dirRef);
        GetDBCount_NNsh(dirRef, &idxCnt);
        while (idxCnt != 0)
        {
            //  すでにディレクトリインデックスが記録されていた場合、
            // とりあえず全レコードを削除する
            (void) DeleteRecordIDX_NNsh(dirRef, (idxCnt - 1));
            idxCnt--;
        }
        index = 0;
        cnt   = 0;

        // OFFLINEスレがVFSに記録されている場合の検索
        if ((NNshGlobal->logPrefixH == 0)||
            ((area = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
        {
            MemSet (buf, sizeof(buf), 0x00);
            StrCopy(buf, "/");
            area = buf;
        }
        setVFSOfflineThreadInfo(dbRef, dirRef, nofItems, offThread,
                                idxCnt, index, area, subjDB, &cnt);
        if (NNshGlobal->logPrefixH != 0)
        {
            MemHandleUnlock(NNshGlobal->logPrefixH);
        }

        // ディレクトリインデックス情報が存在したときには､ＤＢを並べ替える
        GetDBCount_NNsh(dirRef, &idxCnt);
        if (idxCnt != 0)
        {
            QsortRecord_NNsh(dirRef, NNSH_KEYTYPE_UINT16, 0);
        }
        CloseDatabase_NNsh(dirRef);

        if (((NNshGlobal->NNsiParam)->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            // Dir Index DBをVFSにバックアップする
            ret = BackupDatabaseToVFS_NNsh(DBNAME_DIRINDEX);
            if (ret != errNone)
            {
                // エラー表示
                NNsh_DebugMessage(ALTID_ERROR, "BackupDatabaseToVFS()",
                                  DBNAME_DIRINDEX, ret);
            }
        }
        SetMsg_BusyForm(MSG_CREATEOFFLINE_LIST);
    }
    CloseDatabase_NNsh(dbRef);

    Hide_BusyForm(true);

    ret = errNone;

FUNC_END:

    // 自動電源OFFタイマを復旧させる
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    // 領域の解放
    MEMFREE_PTR(offThread);
    MEMFREE_PTR(bbsDB);
    MEMFREE_PTR(subjDB);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : getThreadTitle                                             */
/*                     　   (１番目のメッセージのけつにあるタイトルの取得) */
/*-------------------------------------------------------------------------*/
static Char *getThreadTitle(Char *msgP, UInt32 *size)
{
    Char *lastP, *ptr, *endP;

    *size = 0;

    // データのおしり("\x0a")を見つける
#ifdef USE_STRSTR
    endP  = StrStr(msgP, "\x0a");
    if (endP == NULL)
#else
    endP = msgP;
    while ((*endP != '\x0a')&&(*endP != '\0'))
    {
        endP++;
    }
    if (*endP == '\0')
#endif
    {
        NNsh_DebugMessage(ALTID_ERROR, "Cannot find Separator :", msgP, 0);
        return (msgP);
    }
    // 改行コードが "\x0d\x0a" だった場合の処理(互換用)
    if (*(endP - 1) == '\x0d')
    {
        endP--;
    }

    // タイトルの先頭を見つける
    *endP = '\0';
    lastP = msgP;
    ptr   = msgP;
    while (ptr < endP)
    {
        lastP = ptr;
        ptr   = StrStr(ptr, DATA_SEPARATOR);
        if (ptr == NULL)
        {
            // セパレータが見つからなかった、ループを抜ける
            break;
        }
        ptr   = ptr + 2; // 2 == StrLen(DATA_SEPARATOR);
    }
    if (lastP != msgP)
    {
        // 
        *size = endP - lastP;
    }
    return (lastP);
}

/*=========================================================================*/
/*   Function : setThreadInformation                                       */
/*                                               (OFFLINE) スレ情報を作成  */
/*=========================================================================*/
Err SetThreadInformation(Char *fileName, UInt16 fileMode, UInt16 *kanjiCode,
                                                 NNshSubjectDatabase *subDB)
{
    Err          ret;
    Char        *buffer, *titleP;
    Char         tempBuffer[MAX_THREADNAME + MARGIN];
    UInt32       readSize, titleSize;
    NNshFileRef  fileRef;

    // ファイルを読み出す一時的な領域を確保
    buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        return (~errNone);
    }
    MemSet(buffer, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "File Open :", fileName, ret);
        MEMFREE_PTR(buffer);
        return (ret);
    }

    // スレファイルサイズの取得
    GetFileSize_NNsh(&fileRef, &(subDB->fileSize));

    // ファイルの先頭からざっくり読み出す
    ret = ReadFile_NNsh(&fileRef,0 ,(NNshGlobal->NNsiParam)->bufferSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_DebugMessage(ALTID_ERROR, "ReadFile_NNsh :",  fileName, ret);
        MEMFREE_PTR(buffer);
        return (ret);
    }

    if (kanjiCode != NULL)
    {
        // 漢字コード初期設定
        *kanjiCode = NNSH_KANJICODE_SHIFTJIS;

        // 漢字コードのチェック
        titleP = StrStr(buffer, "charset=");
        if (titleP != NULL)
        {
            // 文字列を読み飛ばす
            titleP = titleP + StrLen("charset=");
        
            // 空白文字を読み飛ばし
            while ((*titleP == ' ')||(*titleP == '\t'))
            {
                titleP++;
            }
            if (StrNCaselessCompare(titleP, "iso-2022-jp", StrLen("iso-2022-jp")) == 0)
            {
                *kanjiCode = NNSH_KANJICODE_JIS;
            }
            else if (StrNCaselessCompare(titleP, "euc-jp", StrLen("euc-jp")) == 0)
            {
                *kanjiCode = NNSH_KANJICODE_EUC;
            }
            else if (StrNCaselessCompare(titleP, "x-euc-jp", StrLen("x-euc-jp")) == 0)
            {
                *kanjiCode = NNSH_KANJICODE_EUC;
            }
            else if (StrNCaselessCompare(titleP, "shift_jis", StrLen("shift_jis")) == 0)
            {
                *kanjiCode = NNSH_KANJICODE_SHIFTJIS;
            }
            else if (StrNCaselessCompare(titleP, "x-sjis", StrLen("x-sjis")) == 0)
            {
                *kanjiCode = NNSH_KANJICODE_SHIFTJIS;
            }
            else if (StrNCaselessCompare(titleP, "shift-jis", StrLen("shift-jis")) == 0)
            {
                *kanjiCode = NNSH_KANJICODE_SHIFTJIS;
            }
        }

        // <title> タグから文字列を取得する(漢字コードはEUC)
        titleSize = 0;
        titleP = StrStr(buffer, "<title>");
        if (titleP == NULL)
        {
            // タイトルタグが小文字でなかった場合...
            titleP = StrStr(buffer, "<TITLE>");
        }
        if (titleP != NULL)
        {
            titleP = titleP + StrLen("<TITLE>");

            // タイトルデータの末尾を探す
            while ((titleP[titleSize] != '\0')&&(titleP[titleSize] != '<'))
            {
                titleSize++;
            }
            titleP[titleSize] = '\0';
        }

        // 一度テンポラリ領域にコピーして漢字コード変換を行う
        MemSet(tempBuffer, sizeof(tempBuffer), 0x00);
        switch (*kanjiCode)
        {
          case NNSH_KANJICODE_EUC:
            // EUC -> SJ変換して格納
            StrCopySJ(tempBuffer, titleP);
            break;

          case NNSH_KANJICODE_JIS:
            // JIS -> SJ変換して格納(これはJIS...)
            StrCopyJIStoSJ(tempBuffer, titleP);
             break;
 
          case NNSH_KANJICODE_SHIFTJIS:
          default:
            // そのまま格納
            StrCopy(tempBuffer, titleP);
            break;
        }
        titleP = tempBuffer;
    }
    else
    {
        // スレタイトルを取得
        titleP = getThreadTitle(buffer, &titleSize);
    }

    // スレタイトルがちゃんと取得できたかチェックする
    if (titleSize != 0)
    {
        // タイトルの区切りが '\x0a'なので、'\0'に置換してコピーする
        titleP[titleSize] = '\0';
        StrNCopy(subDB->threadTitle, titleP, (MAX_THREADNAME - 1));
        ret = errNone;
    }
    else
    {
        // スレタイトル取得失敗
        ret = ~errNone;
    }
    MEMFREE_PTR(buffer);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : setVFSOfflineThreadInfo                                    */
/*                                          OFFLINEスレ情報を作成(for VFS) */
/*-------------------------------------------------------------------------*/
static void setVFSOfflineThreadInfo(DmOpenRef dbRef, DmOpenRef dirDBRef,
                                    UInt16 nofItems, UInt16 *recList,
                                    UInt16 depth, UInt16 index, Char *dirName,
                                    NNshSubjectDatabase *subDb, UInt16 *cnt)
{
    Err                    ret = errNone;
    FileRef                dirRef;
    FileInfoType           info;
    UInt16                 recNum, kanji;
    UInt32                 dirIterator;
    Char                  *chkPtr, *fileName;
    NNshDirectoryDatabase *dirDb;

    // ディレクトリDB格納用の領域を確保する
    dirDb = MEMALLOC_PTR(sizeof(NNshDirectoryDatabase));
    if (dirDb == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc(bufName) size:",
                          "", sizeof(NNshDirectoryDatabase));
        return;
    }

    // ファイル名の領域を確保する
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc. Failure size:",
                          "", MAXLENGTH_FILENAME + MARGIN);
        MEMFREE_PTR(dirDb);
        return;
    }
    MemSet (fileName, MAXLENGTH_FILENAME + MARGIN, 0x00);

    // ディレクトリのオープン
    ret = VFSFileOpen(NNshGlobal->vfsVol, dirName, vfsModeRead, &dirRef);
    if (ret != errNone)
    {
        goto FUNC_END;
    }
    SetMsg_BusyForm(dirName);

    // オープンできたら、オープンしたディレクトリ内にあるファイル名を取得
    StrNCopy(fileName, dirName, MAXLENGTH_FILENAME);
    info.nameP      = fileName + StrLen(fileName);
    info.nameBufLen = MAXLENGTH_FILENAME - StrLen(fileName);
    dirIterator     = vfsIteratorStart;

    while (dirIterator != vfsIteratorStop)
    {
        ret = VFSDirEntryEnumerate(dirRef, &dirIterator, &info);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "VFSDirEntryEnumerate()", "", ret);
            break;
        }

        // 取得したファイル名がディレクトリならば、(その下を)再スキャン
        if ((info.attributes & vfsFileAttrDirectory) == vfsFileAttrDirectory)
        {
            //  ディレクトリデータベースにデータを記録する
            StrCat(fileName, "/");   // その前にディレクトリなんで末尾に '/'
            MemSet(dirDb, sizeof(NNshDirectoryDatabase), 0x00);
            StrNCopy(dirDb->dirName, info.nameP, MAX_DIRNAME);
            StrCopy(dirDb->boardNick, OFFLINE_THREAD_NICK);
            dirDb->previousIndex = index;
            dirDb->depth         = depth + 1;
            GetDBCount_NNsh(dirDBRef, &(dirDb->dirIndex));
            (dirDb->dirIndex)++;

            ret = 
              EntryRecord_NNsh(dirDBRef, sizeof(NNshDirectoryDatabase), dirDb);
            NNsh_DebugMessage(ALTID_INFO, dirDb->dirName, " ret:", ret);

            // 自分自身を再帰呼び出しして、サブディレクトリにあるログを検索
            setVFSOfflineThreadInfo(dbRef, dirDBRef, nofItems, recList,
                                    dirDb->depth, dirDb->dirIndex, fileName,
                                    subDb, cnt);
            fileName[StrLen(fileName) - 1] = '\0';
            continue;
        }

        // データファイル(*.dat)かどうかチェック
        chkPtr = StrStr(info.nameP, DATAFILE_SUFFIX);
        if ((chkPtr != NULL)&&(*(chkPtr + StrLen(DATAFILE_SUFFIX)) == '\0'))
        {
            MemSet(subDb, sizeof(NNshSubjectDatabase), 0x00);

            // スレファイル名を取得する
            StrNCopy(subDb->threadFileName, info.nameP, MAX_THREADFILENAME);

            // スレ情報(スレタイトル等)を獲得
            ret = SetThreadInformation(fileName, (NNSH_FILEMODE_READONLY), 
                                                                 NULL, subDb);
            // データファイル発見!
            NNsh_DebugMessage(ALTID_INFO,
                              subDb->threadFileName, subDb->threadTitle, *cnt);
            if (ret == errNone)
            {
                ret = checkOfflineEntryAvailable(dbRef, &recNum, nofItems,
                                                 recList,
                                                 subDb->threadFileName,
                                                 FILEMGR_STATE_OPENED_VFS,
                                                 *cnt);
                if (ret != errNone)
                {
                    // すでにある、既存レコードに対してアップデートをかける
                    GetRecord_NNsh(dbRef, recNum, 
                                   sizeof(NNshSubjectDatabase), subDb);
                    subDb->dirIndex = index;
                    UpdateRecord_NNsh(dbRef, recNum, 
                                      sizeof(NNshSubjectDatabase), subDb);
                }
                else
                {
                    // その他のスレ情報を設定する
                    StrCopy(subDb->boardNick, OFFLINE_THREAD_NICK);
                    subDb->state    = NNSH_SUBJSTATUS_UNKNOWN;
                    subDb->bbsType  = NNSH_BBSTYPE_2ch;
                    subDb->dirIndex = index;
                    subDb->msgState = FILEMGR_STATE_OPENED_VFS;
                    ret = EntryRecord_NNsh(dbRef,
                                           sizeof(NNshSubjectDatabase), 
                                           subDb);
                    (*cnt)++;
                }
            }
        }
        // まちＢＢＳのデータファイル(*.cgi)かどうかチェック
        chkPtr = StrStr(info.nameP, CGIFILE_SUFFIX);
        if ((chkPtr != NULL)&&(*(chkPtr + StrLen(CGIFILE_SUFFIX)) == '\0'))
        {
            MemSet(subDb, sizeof(NNshSubjectDatabase), 0x00);

            // スレファイル名を取得する
            StrNCopy(subDb->threadFileName, info.nameP, MAX_THREADFILENAME);

            // スレ情報(スレタイトル等)を獲得
            ret = SetThreadInformation(fileName, (NNSH_FILEMODE_READONLY), 
                                                                  &kanji, subDb);
            // データファイル発見!
            NNsh_DebugMessage(ALTID_INFO,
                              subDb->threadFileName, subDb->threadTitle, *cnt);
            if (ret == errNone)
            {
                ret = checkOfflineEntryAvailable(dbRef, &recNum, nofItems,
                                                 recList,
                                                 subDb->threadFileName,
                                                 FILEMGR_STATE_OPENED_VFS,
                                                 *cnt);
                if (ret != errNone)
                {
                    // すでにある、既存レコードに対してアップデートをかける
                    GetRecord_NNsh(dbRef, recNum, 
                                   sizeof(NNshSubjectDatabase), subDb);
                    subDb->dirIndex = index;
                    UpdateRecord_NNsh(dbRef, recNum, 
                                      sizeof(NNshSubjectDatabase), subDb);
                }
                else
                {
                    // その他のスレ情報を設定する
                    StrCopy(subDb->boardNick, OFFLINE_THREAD_NICK);
                    subDb->state    = NNSH_SUBJSTATUS_UNKNOWN;
                    switch (kanji)
                    {
                      case NNSH_KANJICODE_SHIFTJIS:
                        // 漢字コードがSHIFT JISならまちＢＢＳ
                        subDb->bbsType  = NNSH_BBSTYPE_MACHIBBS;
                        break;

                      case NNSH_KANJICODE_EUC:
                        // 漢字コードがEUCなら、したらば＠ＪＢＢＳ
                        subDb->bbsType  = NNSH_BBSTYPE_SHITARABAJBBS_OLD;
                        break;

                      default:
                        // 漢字コードがそれ以外ならHTML
                         subDb->bbsType = (UInt8) (NNSH_BBSTYPE_HTML |
                           (NNSH_BBSTYPE_CHAR_JIS << NNSH_BBSTYPE_CHAR_SHIFT));
                    }
                    subDb->dirIndex = index;
                    subDb->msgState = FILEMGR_STATE_OPENED_VFS;
                    ret = EntryRecord_NNsh(dbRef,
                                           sizeof(NNshSubjectDatabase), 
                                           subDb);
                    (*cnt)++;
                }
            }
        }
    }
    VFSFileClose(dirRef);

FUNC_END:
    MEMFREE_PTR(fileName);
    MEMFREE_PTR(dirDb);
    return;
}

/*=========================================================================*/
/*   Function : setOfflineLogDir                                           */
/*                                 Offlineログのベースディレクトリを求める */
/*=========================================================================*/
Err setOfflineLogDir(Char *fileName)
{
    Err        ret;
    FileRef    ref;
    Char       fnBuf[MAXLENGTH_FILENAME], *ptr, *area;
    UInt32     size;

    // 格納する領域を取得、初期化
    if (NNshGlobal->logPrefixH != 0)
    {
        MemHandleFree(NNshGlobal->logPrefixH);
        NNshGlobal->logPrefixH = 0;
    }
    NNshGlobal->logPrefixH = MemHandleNew(MAXLENGTH_FILENAME);
    if (NNshGlobal->logPrefixH == 0)
    {
        return (~errNone);
    }
    area = MemHandleLock(NNshGlobal->logPrefixH);
    if (area == NULL)
    {
        MemHandleFree(NNshGlobal->logPrefixH);
        NNshGlobal->logPrefixH = 0;
        return (~errNone);
    }
    MemSet(area, MAXLENGTH_FILENAME, 0x00);

    ret = VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &ref);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    MemSet(fnBuf, sizeof(fnBuf), 0x00);
    ret = VFSFileRead(ref, (sizeof(fnBuf) - 1), fnBuf, &size);
    VFSFileClose(ref);
    if ((ret != errNone)&&(ret != vfsErrFileEOF))
    {
        goto FUNC_END;
    }
    ptr  = StrStr(fnBuf, DATA_SEPARATOR);
    if (ptr == NULL)
    {
        ret = ~errNone;
        goto FUNC_END;
    }

    // データセパレータがあった場合には、logdir.txtを反映させる
    *ptr = '\0';
    (void) StrCopy(area, fnBuf);
    if (area[StrLen(area) - 1] != '/')
    {
        // ディレクトリの最後に '/'がなかった場合には追加する。
        NNsh_DebugMessage(ALTID_INFO, "Append '/' :", area, 0);
        StrCat(area, "/");
    }
    ret = errNone;

FUNC_END:
    if (ret != errNone)
    {
        StrCopy(area, LOGDATAFILE_PREFIX);
    }
    MemHandleUnlock(NNshGlobal->logPrefixH);
    return (ret);
}
