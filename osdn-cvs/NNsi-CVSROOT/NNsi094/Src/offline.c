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
/*                                                  OFFLINE�X�������쐬  */
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

    // �̈�̊m��
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

    // �����d���n�e�e�^�C�}�𖳌��ɂ���
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    // �ŏ���OFFLINE�̃X����S������
    DeleteSubjectList(OFFLINE_THREAD_NICK, NNSH_SUBJSTATUS_DELETE, &cnt);

    // �X�����Ǘ�DB�I�[�v��
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

    // OFFLINE�X����VFS�ɋL�^����Ă��邩�ǂ����m�F����
    if ((NNshParam->useVFS & NNSH_VFS_USEOFFLINE) == 0)
    {
        /*** OFFLINE�X����VFS�ɋL�^����Ă��Ȃ��ꍇ ... ***/
        // ����̃f�[�^���擾
        ret = DmGetNextDatabaseByTypeCreator(true, &state, OFFLINE_DBTYPE_ID,
                                             SOFT_CREATOR_ID, false, &cardNo,
                                             &dbLocalID);
        while ((ret == errNone)&&(dbLocalID != 0))
        {
            // �擾�����f�[�^���uOFFLINE�f�[�^�x�[�X�v�Ɋi�[����
            MemSet(subjDB, sizeof(NNshSubjectDatabase), 0x00);
            err = DmDatabaseInfo(cardNo, dbLocalID, subjDB->threadFileName,
                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL);
            if (err == errNone)
            {
                // �X�����(�X���^�C�g����)���l��
                err = SetThreadInformation(subjDB->threadFileName,
                                                  ((NNSH_FILEMODE_READONLY)|
                                                  (NNSH_FILEMODE_TEMPFILE)),
                                                  subjDB);
                if (err == errNone)
                {
                    // ���̑��̃X������ݒ肷��
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

            // ���ڈȍ~�̃f�[�^�擾
            ret = DmGetNextDatabaseByTypeCreator(false, &state, 
                                            OFFLINE_DBTYPE_ID, SOFT_CREATOR_ID,
                                            false, &cardNo, &dbLocalID);
        }
        NNsh_DebugMessage(ALTID_INFO, "Thread count", "", cnt);
    }
    else
    {
        // �f�B���N�g���C���f�b�N�X���I�[�v�����Ă݂�
        OpenDatabase_NNsh(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, &dirRef);
        GetDBCount_NNsh(dirRef, &idxCnt);
        while (idxCnt != 0)
        {
            //  ���łɃf�B���N�g���C���f�b�N�X���L�^����Ă����ꍇ�A
            // �Ƃ肠�����S���R�[�h���폜����
            (void) DeleteRecordIDX_NNsh(dirRef, (idxCnt - 1));
            idxCnt--;
        }
        index = 0;
        cnt   = 0;

        // OFFLINE�X����VFS�ɋL�^����Ă���ꍇ�̌���
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
            // Dir Index DB��VFS�Ƀo�b�N�A�b�v����
            ret = BackupDatabaseToVFS_NNsh(DBNAME_DIRINDEX);
            if (ret != errNone)
            {
                // �G���[�\��
                NNsh_DebugMessage(ALTID_ERROR, "BackupDatabaseToVFS()",
                                  DBNAME_DIRINDEX, ret);
            }
        }
        SetMsg_BusyForm(MSG_CREATEOFFLINE_LIST);
    }
    CloseDatabase_NNsh(dbRef);

    // BBS���ɁA�����X�����𔽉f������B
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

    // �����d��OFF�^�C�}�𕜋�������
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    MemPtrFree(bbsDB);
    MemPtrFree(subjDB);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : getThreadTitle                                             */
/*                     �@   (�P�Ԗڂ̃��b�Z�[�W�̂��ɂ���^�C�g���̎擾) */
/*-------------------------------------------------------------------------*/
static Char *getThreadTitle(Char *msgP, UInt32 *size)
{
    Char *lastP, *ptr, *endP;

    *size = 0;

    // �f�[�^�̂�����("\x0a")��������
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
    // ���s�R�[�h�� "\x0d\x0a" �������ꍇ�̏���(�݊��p)
    if (*(endP - 1) == '\x0d')
    {
        endP--;
    }

    // �^�C�g���̐擪��������
    *endP = '\0';
    lastP = msgP;
    ptr   = msgP;
    while (ptr < endP)
    {
        lastP = ptr;
        ptr   = StrStr(ptr, DATA_SEPARATOR);
        if (ptr == NULL)
        {
            // �Z�p���[�^��������Ȃ������A���[�v�𔲂���
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
/*                                               (OFFLINE) �X�������쐬  */
/*-------------------------------------------------------------------------*/
Err SetThreadInformation(Char *fileName, UInt16 fileMode,
                                                 NNshSubjectDatabase *subDB)
{
    Err          ret;
    Char        *buffer, *titleP;
    UInt32       readSize, titleSize;
    NNshFileRef  fileRef;

    // �t�@�C����ǂݏo���ꎞ�I�ȗ̈���m��
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

    // �X���t�@�C���T�C�Y�̎擾
    GetFileSize_NNsh(&fileRef, &(subDB->fileSize));

    // �t�@�C���̐擪���炴������ǂݏo��
    ret = ReadFile_NNsh(&fileRef,0 ,NNshParam->bufferSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_DebugMessage(ALTID_ERROR, "ReadFile_NNsh :",  fileName, ret);
        MemPtrFree(buffer);
        return (ret);
    }
    // �X���^�C�g�����擾
    titleP = getThreadTitle(buffer, &titleSize);

    // �X���^�C�g���������Ǝ擾�ł������`�F�b�N����
    if (titleSize != 0)
    {
        // �^�C�g���̋�؂肪 '\x0a'�Ȃ̂ŁA'\0'�ɒu�����ăR�s�[����
        titleP[titleSize] = '\0';
        StrNCopy(subDB->threadTitle, titleP, (MAX_THREADNAME - 1));
        ret = errNone;
    }
    else
    {
        // �X���^�C�g���擾���s
        ret = ~errNone;
    }
    MemPtrFree(buffer);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : setVFSOfflineThreadInfo                                    */
/*                                          OFFLINE�X�������쐬(for VFS) */
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

    // �f�B���N�g��DB�i�[�p�̗̈���m�ۂ���
    dirDb = MemPtrNew(sizeof(NNshDirectoryDatabase));
    if (dirDb == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc(bufName) size:",
                          "", sizeof(NNshDirectoryDatabase));
        return;
    }

    // �t�@�C�����̗̈���m�ۂ���
    fileName = MemPtrNew(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc. Failure size:",
                          "", MAXLENGTH_FILENAME + MARGIN);
        MemPtrFree(dirDb);
        return;
    }
    MemSet (fileName, MAXLENGTH_FILENAME + MARGIN, 0x00);

    // �f�B���N�g���̃I�[�v��
    ret = VFSFileOpen(NNshGlobal->vfsVol, dirName, vfsModeRead, &dirRef);
    if (ret != errNone)
    {
        goto FUNC_END;
    }
    SetMsg_BusyForm(dirName);

    // �I�[�v���ł�����A�I�[�v�������f�B���N�g�����ɂ���t�@�C�������擾
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

        // �擾�����t�@�C�������f�B���N�g���Ȃ�΁A(���̉���)�ăX�L����
        if ((info.attributes & vfsFileAttrDirectory) == vfsFileAttrDirectory)
        {
            //  �f�B���N�g���f�[�^�x�[�X�Ƀf�[�^���L�^����
            StrCat(fileName, "/");   // ���̑O�Ƀf�B���N�g���Ȃ�Ŗ����� '/'
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

            // �������g���ċA�Ăяo�����āA�T�u�f�B���N�g���ɂ��郍�O������
            setVFSOfflineThreadInfo(dbRef, dirDBRef, dirDb->depth, 
                                    dirDb->dirIndex, fileName, subDb, cnt);
            fileName[StrLen(fileName) - 1] = '\0';
            continue;
        }

        // �f�[�^�t�@�C��(*.dat)���ǂ����`�F�b�N
        chkPtr = StrStr(info.nameP, DATAFILE_SUFFIX);
        if ((chkPtr != NULL)&&(*(chkPtr + StrLen(DATAFILE_SUFFIX)) == '\0'))
        {
            MemSet(subDb, sizeof(NNshSubjectDatabase), 0x00);

            // �X���t�@�C�������擾����
            StrNCopy(subDb->threadFileName, info.nameP, MAX_THREADFILENAME);

            // �X�����(�X���^�C�g����)���l��
            ret = SetThreadInformation(fileName,
                                       (NNSH_FILEMODE_READONLY), subDb);
            // �f�[�^�t�@�C������!
            NNsh_DebugMessage(ALTID_INFO,
                              subDb->threadFileName, subDb->threadTitle, *cnt);
            if (ret == errNone)
            {
                // ���̑��̃X������ݒ肷��
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
/*                                 Offline���O�̃x�[�X�f�B���N�g�������߂� */
/*=========================================================================*/
Err setOfflineLogDir(Char *fileName)
{
    Err        ret;
    FileRef    ref;
    Char       fnBuf[MAXLENGTH_FILENAME], *ptr, *area;
    UInt32     size;

    // �i�[����̈���擾�A������
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

    // �f�[�^�Z�p���[�^���������ꍇ�ɂ́Alogdir.txt�𔽉f������
    *ptr = '\0';
    (void) StrCopy(area, fnBuf);
    if (area[StrLen(area) - 1] != '/')
    {
        // �f�B���N�g���̍Ō�� '/'���Ȃ������ꍇ�ɂ͒ǉ�����B
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