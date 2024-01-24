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
/*                           OFFLINE�X���Ƃ��ă��R�[�h�o�^���ׂ������肷�� */
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
        // �Q�ƃ��O���X�g��ǂݏo���āA�o�^�ς݂��m�F����
        GetRecordReadOnly_NNsh(dbRef, (recList[loop] + offset),
                               &mesH, (void **)&mesInfo);
        msgStatus = mesInfo->msgState;

        // ���b�Z�[�W�̊i�[�ꏊ�ƃX���t�@�C�������������ǂ����m�F����
        if ((msgStatus == msgState)&&
            (StrCompare(threadFileName, mesInfo->threadFileName) == 0))
        {
            ReleaseRecordReadOnly_NNsh(dbRef, mesH);
            // ���łɓo�^����Ă����I�G���[��������
            *recNum = recList[loop] + offset;
            return (~errNone);
        }
        ReleaseRecordReadOnly_NNsh(dbRef, mesH);
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : getOfflineThreadRecords                                    */
/*                                   OFFLINE�X���̃��R�[�h�ԍ����Ƃ��Ă��� */
/*-------------------------------------------------------------------------*/
static Err getOfflineThreadRecords(UInt16 *nofItems, UInt16 **offThread)
{
    Err                  ret;
    DmOpenRef            dbRef;
    UInt16               max, loop, *buffer;
    MemHandle            mesH;
    NNshSubjectDatabase *mesInfo;

    *nofItems  = 0;

    // BUSY�E�B���h�E�\��
    Show_BusyForm(MSG_CHECKOFFLINE_LIST);

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        ret = ~errNone;
        goto FUNC_END;
    }
    // �����擾
    GetDBCount_NNsh(dbRef, &max);

    // ���R�[�h�m�F����m�ۂ���
    buffer = MEMALLOC_PTR(sizeof(UInt16) * max + MARGIN);
    if (buffer == NULL)
    {
        // �̈�m�ێ��s
        ret = ~errNone - 10;
        goto FUNC_END;
    }
    MemSet(buffer, (sizeof(UInt16) * max + MARGIN), 0x00);

    // OFFLINE�X���̃��R�[�h�ԍ�������
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
            // �Q�ƃ��O�����A���̃��R�[�h�ԍ����L������
            buffer[*nofItems] = loop;
            (*nofItems)++;
        }
        ReleaseRecordReadOnly_NNsh(dbRef, mesH);
    }
    CloseDatabase_NNsh(dbRef);

    // �������̎Q�ƃ��O�𔭌��A�R�s�[����B
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
    // BUSY�E�B���h�E����Ċ֐����I������
    Hide_BusyForm(false);
    return (ret);
}

/*=========================================================================*/
/*   Function : create_offline_database                                    */
/*                                                  OFFLINE�X�������쐬  */
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

    // �̈�̊m��
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

    // �����d���n�e�e�^�C�}�𖳌��ɂ���
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    ret       = ~errNone;
    nofItems  = 0;
    offThread = NULL;
    if ((NNshGlobal->NNsiParam)->notDelOffline != 0)
    {
        //  NNsh�p�����[�^��"OFFLINE�X���������Ȃ�"���w�肳��Ă����Ƃ��́A
        // ���ݓo�^����Ă���OFFLINE�X���̃��R�[�h�ԍ����擾����B
        ret = getOfflineThreadRecords(&nofItems, &offThread);
    }
    if (ret != errNone)
    {
        // �ŏ���OFFLINE�̃X����S������
        DeleteSubjectList(OFFLINE_THREAD_NICK, NNSH_SUBJSTATUS_DELETE_OFFLINE, &cnt);
    }

    // �X�����Ǘ�DB�I�[�v��
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

    // OFFLINE�X����VFS�ɋL�^����Ă��邩�ǂ����m�F����
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) == 0)
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
                                                  NULL, subjDB);
                if (err == errNone)
                {
                    // �X���f�[�^��o�^���Ă��悢���m�F����
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
                        // ���̑��̃X������ݒ�A�f�[�^��o�^����
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

            // ���ڈȍ~�̃f�[�^�擾
            ret = DmGetNextDatabaseByTypeCreator(false, &state, 
                                            OFFLINE_DBTYPE_ID, SOFT_CREATOR_ID,
                                            false, &cardNo, &dbLocalID);
        }
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
        setVFSOfflineThreadInfo(dbRef, dirRef, nofItems, offThread,
                                idxCnt, index, area, subjDB, &cnt);
        if (NNshGlobal->logPrefixH != 0)
        {
            MemHandleUnlock(NNshGlobal->logPrefixH);
        }

        // �f�B���N�g���C���f�b�N�X��񂪑��݂����Ƃ��ɂͤ�c�a����בւ���
        GetDBCount_NNsh(dirRef, &idxCnt);
        if (idxCnt != 0)
        {
            QsortRecord_NNsh(dirRef, NNSH_KEYTYPE_UINT16, 0);
        }
        CloseDatabase_NNsh(dirRef);

        if (((NNshGlobal->NNsiParam)->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
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

    Hide_BusyForm(true);

    ret = errNone;

FUNC_END:

    // �����d��OFF�^�C�}�𕜋�������
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    // �̈�̉��
    MEMFREE_PTR(offThread);
    MEMFREE_PTR(bbsDB);
    MEMFREE_PTR(subjDB);

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

/*=========================================================================*/
/*   Function : setThreadInformation                                       */
/*                                               (OFFLINE) �X�������쐬  */
/*=========================================================================*/
Err SetThreadInformation(Char *fileName, UInt16 fileMode, UInt16 *kanjiCode,
                                                 NNshSubjectDatabase *subDB)
{
    Err          ret;
    Char        *buffer, *titleP;
    Char         tempBuffer[MAX_THREADNAME + MARGIN];
    UInt32       readSize, titleSize;
    NNshFileRef  fileRef;

    // �t�@�C����ǂݏo���ꎞ�I�ȗ̈���m��
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

    // �X���t�@�C���T�C�Y�̎擾
    GetFileSize_NNsh(&fileRef, &(subDB->fileSize));

    // �t�@�C���̐擪���炴������ǂݏo��
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
        // �����R�[�h�����ݒ�
        *kanjiCode = NNSH_KANJICODE_SHIFTJIS;

        // �����R�[�h�̃`�F�b�N
        titleP = StrStr(buffer, "charset=");
        if (titleP != NULL)
        {
            // �������ǂݔ�΂�
            titleP = titleP + StrLen("charset=");
        
            // �󔒕�����ǂݔ�΂�
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

        // <title> �^�O���當������擾����(�����R�[�h��EUC)
        titleSize = 0;
        titleP = StrStr(buffer, "<title>");
        if (titleP == NULL)
        {
            // �^�C�g���^�O���������łȂ������ꍇ...
            titleP = StrStr(buffer, "<TITLE>");
        }
        if (titleP != NULL)
        {
            titleP = titleP + StrLen("<TITLE>");

            // �^�C�g���f�[�^�̖�����T��
            while ((titleP[titleSize] != '\0')&&(titleP[titleSize] != '<'))
            {
                titleSize++;
            }
            titleP[titleSize] = '\0';
        }

        // ��x�e���|�����̈�ɃR�s�[���Ċ����R�[�h�ϊ����s��
        MemSet(tempBuffer, sizeof(tempBuffer), 0x00);
        switch (*kanjiCode)
        {
          case NNSH_KANJICODE_EUC:
            // EUC -> SJ�ϊ����Ċi�[
            StrCopySJ(tempBuffer, titleP);
            break;

          case NNSH_KANJICODE_JIS:
            // JIS -> SJ�ϊ����Ċi�[(�����JIS...)
            StrCopyJIStoSJ(tempBuffer, titleP);
             break;
 
          case NNSH_KANJICODE_SHIFTJIS:
          default:
            // ���̂܂܊i�[
            StrCopy(tempBuffer, titleP);
            break;
        }
        titleP = tempBuffer;
    }
    else
    {
        // �X���^�C�g�����擾
        titleP = getThreadTitle(buffer, &titleSize);
    }

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
    MEMFREE_PTR(buffer);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : setVFSOfflineThreadInfo                                    */
/*                                          OFFLINE�X�������쐬(for VFS) */
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

    // �f�B���N�g��DB�i�[�p�̗̈���m�ۂ���
    dirDb = MEMALLOC_PTR(sizeof(NNshDirectoryDatabase));
    if (dirDb == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc(bufName) size:",
                          "", sizeof(NNshDirectoryDatabase));
        return;
    }

    // �t�@�C�����̗̈���m�ۂ���
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc. Failure size:",
                          "", MAXLENGTH_FILENAME + MARGIN);
        MEMFREE_PTR(dirDb);
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
            setVFSOfflineThreadInfo(dbRef, dirDBRef, nofItems, recList,
                                    dirDb->depth, dirDb->dirIndex, fileName,
                                    subDb, cnt);
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
            ret = SetThreadInformation(fileName, (NNSH_FILEMODE_READONLY), 
                                                                 NULL, subDb);
            // �f�[�^�t�@�C������!
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
                    // ���łɂ���A�������R�[�h�ɑ΂��ăA�b�v�f�[�g��������
                    GetRecord_NNsh(dbRef, recNum, 
                                   sizeof(NNshSubjectDatabase), subDb);
                    subDb->dirIndex = index;
                    UpdateRecord_NNsh(dbRef, recNum, 
                                      sizeof(NNshSubjectDatabase), subDb);
                }
                else
                {
                    // ���̑��̃X������ݒ肷��
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
        // �܂��a�a�r�̃f�[�^�t�@�C��(*.cgi)���ǂ����`�F�b�N
        chkPtr = StrStr(info.nameP, CGIFILE_SUFFIX);
        if ((chkPtr != NULL)&&(*(chkPtr + StrLen(CGIFILE_SUFFIX)) == '\0'))
        {
            MemSet(subDb, sizeof(NNshSubjectDatabase), 0x00);

            // �X���t�@�C�������擾����
            StrNCopy(subDb->threadFileName, info.nameP, MAX_THREADFILENAME);

            // �X�����(�X���^�C�g����)���l��
            ret = SetThreadInformation(fileName, (NNSH_FILEMODE_READONLY), 
                                                                  &kanji, subDb);
            // �f�[�^�t�@�C������!
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
                    // ���łɂ���A�������R�[�h�ɑ΂��ăA�b�v�f�[�g��������
                    GetRecord_NNsh(dbRef, recNum, 
                                   sizeof(NNshSubjectDatabase), subDb);
                    subDb->dirIndex = index;
                    UpdateRecord_NNsh(dbRef, recNum, 
                                      sizeof(NNshSubjectDatabase), subDb);
                }
                else
                {
                    // ���̑��̃X������ݒ肷��
                    StrCopy(subDb->boardNick, OFFLINE_THREAD_NICK);
                    subDb->state    = NNSH_SUBJSTATUS_UNKNOWN;
                    switch (kanji)
                    {
                      case NNSH_KANJICODE_SHIFTJIS:
                        // �����R�[�h��SHIFT JIS�Ȃ�܂��a�a�r
                        subDb->bbsType  = NNSH_BBSTYPE_MACHIBBS;
                        break;

                      case NNSH_KANJICODE_EUC:
                        // �����R�[�h��EUC�Ȃ�A������΁��i�a�a�r
                        subDb->bbsType  = NNSH_BBSTYPE_SHITARABAJBBS_OLD;
                        break;

                      default:
                        // �����R�[�h������ȊO�Ȃ�HTML
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
