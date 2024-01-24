/*============================================================================*
 *  FILE: 
 *     offline.c
 *
 *  Description: 
 *     Offline Thread manager for NNsh. 
 *
 *===========================================================================*/
#define OFFLINE_C
#include "local.h"

static void setVFSOfflineThreadInfo(DmOpenRef dbRef, DmOpenRef dirRef, 
                                    UInt16 depth, UInt16 index, Char *dirName,
                                    NNshSubjectDatabase *subDb, UInt16 *cnt);

/*-------------------------------------------------------------------------*/
/*   Function : create_offline_database                                    */
/*                                                  OFFLINEスレ情報を作成  */
/*-------------------------------------------------------------------------*/
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

    // 領域の確保
    subjDB = MemPtrNew(sizeof(NNshSubjectDatabase));
    if (subjDB == NULL)
    {
        return (~errNone);
    }

    bbsDB = MemPtrNew(sizeof(NNshBoardDatabase));
    if (bbsDB == NULL)
    {
        MemPtrFree(subjDB);
        return (~errNone);
    }
    MemSet(bbsDB, sizeof(NNshBoardDatabase), 0x00);

    // 自動電源ＯＦＦタイマを無効にする
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    // 最初にOFFLINEのスレを全部消す
    DeleteSubjectList(OFFLINE_THREAD_NICK, NNSH_SUBJSTATUS_DELETE, &cnt);

    // スレ名管理DBオープン
    Show_BusyForm(MSG_CREATEOFFLINE_LIST);
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        Hide_BusyForm();

        ret = ~errNone;
        goto FUNC_END;
    }
    MemSet(&state, sizeof(state), 0x00);
    cnt = 0;

    // OFFLINEスレがVFSに記録されているかどうか確認する
    if ((NNshParam->useVFS & NNSH_VFS_USEOFFLINE) == 0)
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
                                                  subjDB);
                if (err == errNone)
                {
                    // その他のスレ情報を設定する
                    StrCopy(subjDB->boardNick, OFFLINE_THREAD_NICK);
                    subjDB->state    = NNSH_SUBJSTATUS_UNKNOWN;
                    subjDB->bbsType  = NNSH_BBSTYPE_2ch;
                    subjDB->dirIndex = 0;
                    subjDB->msgState = FILEMGR_STATE_OPENED_STREAM;
                    (void) EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase),
                                            subjDB);
                    cnt++;
                }
            }

            // 二回目以降のデータ取得
            ret = DmGetNextDatabaseByTypeCreator(false, &state, 
                                            OFFLINE_DBTYPE_ID, SOFT_CREATOR_ID,
                                            false, &cardNo, &dbLocalID);
        }
        NNsh_DebugMessage(ALTID_INFO, "Thread count", "", cnt);
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
        setVFSOfflineThreadInfo(dbRef, dirRef, idxCnt, index, 
                                area, subjDB, &cnt);
        MemHandleUnlock(NNshGlobal->logPrefixH);
        QsortRecord_NNsh(dirRef, NNSH_KEYTYPE_UINT16, 0);
        CloseDatabase_NNsh(dirRef);

        if ((NNshParam->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
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

    // BBS情報に、所属スレ数を反映させる。
    ret = Get_BBS_Database(OFFLINE_THREAD_NICK, bbsDB, &index);
    if (ret == errNone)
    {
        bbsDB->threadCnt = cnt;
        ret =  Update_BBS_Info(NNSH_SELBBS_OFFLINE, bbsDB);
        NNsh_DebugMessage(ALTID_INFO, "Update_BBS_Info()", "", ret);
    }
    Hide_BusyForm();

    ret = errNone;

FUNC_END:

    // 自動電源OFFタイマを復旧させる
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    MemPtrFree(bbsDB);
    MemPtrFree(subjDB);

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

/*-------------------------------------------------------------------------*/
/*   Function : setThreadInformation                                       */
/*                                               (OFFLINE) スレ情報を作成  */
/*-------------------------------------------------------------------------*/
Err SetThreadInformation(Char *fileName, UInt16 fileMode,
                                                 NNshSubjectDatabase *subDB)
{
    Err          ret;
    Char        *buffer, *titleP;
    UInt32       readSize, titleSize;
    NNshFileRef  fileRef;

    // ファイルを読み出す一時的な領域を確保
    buffer = MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        return (~errNone);
    }
    MemSet(buffer, (NNshParam->bufferSize + MARGIN), 0x00);

    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "File Open :", fileName, ret);
        MemPtrFree(buffer);
        return (ret);
    }

    // スレファイルサイズの取得
    GetFileSize_NNsh(&fileRef, &(subDB->fileSize));

    // ファイルの先頭からざっくり読み出す
    ret = ReadFile_NNsh(&fileRef,0 ,NNshParam->bufferSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_DebugMessage(ALTID_ERROR, "ReadFile_NNsh :",  fileName, ret);
        MemPtrFree(buffer);
        return (ret);
    }
    // スレタイトルを取得
    titleP = getThreadTitle(buffer, &titleSize);

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
    MemPtrFree(buffer);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : setVFSOfflineThreadInfo                                    */
/*                                          OFFLINEスレ情報を作成(for VFS) */
/*-------------------------------------------------------------------------*/
static void setVFSOfflineThreadInfo(DmOpenRef dbRef, DmOpenRef dirDBRef,
                                    UInt16 depth, UInt16 index, Char *dirName,
                                    NNshSubjectDatabase *subDb, UInt16 *cnt)
{
    Err                    ret = errNone;
    FileRef                dirRef;
    FileInfoType           info;
    UInt32                 dirIterator;
    Char                  *chkPtr, *fileName;
    NNshDirectoryDatabase *dirDb;

    // ディレクトリDB格納用の領域を確保する
    dirDb = MemPtrNew(sizeof(NNshDirectoryDatabase));
    if (dirDb == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc(bufName) size:",
                          "", sizeof(NNshDirectoryDatabase));
        return;
    }

    // ファイル名の領域を確保する
    fileName = MemPtrNew(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc. Failure size:",
                          "", MAXLENGTH_FILENAME + MARGIN);
        MemPtrFree(dirDb);
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
            setVFSOfflineThreadInfo(dbRef, dirDBRef, dirDb->depth, 
                                    dirDb->dirIndex, fileName, subDb, cnt);
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
            ret = SetThreadInformation(fileName,
                                       (NNSH_FILEMODE_READONLY), subDb);
            // データファイル発見!
            NNsh_DebugMessage(ALTID_INFO,
                              subDb->threadFileName, subDb->threadTitle, *cnt);
            if (ret == errNone)
            {
                // その他のスレ情報を設定する
                StrCopy(subDb->boardNick, OFFLINE_THREAD_NICK);
                subDb->state    = NNSH_SUBJSTATUS_UNKNOWN;
                subDb->bbsType  = NNSH_BBSTYPE_2ch;
                subDb->dirIndex = index;
                subDb->msgState = FILEMGR_STATE_OPENED_VFS;
                ret =
                   EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), subDb);
                (*cnt)++;
            }
        }
    }
    VFSFileClose(dirRef);

FUNC_END:
    MemPtrFree(fileName);
    MemPtrFree(dirDb);
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
