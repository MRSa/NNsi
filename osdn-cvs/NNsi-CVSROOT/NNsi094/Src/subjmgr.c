/*============================================================================*
 *  FILE: 
 *     subjmgr.c
 *
 *  Description: 
 *     �X���ꗗ�擾�T�u
 *===========================================================================*/
#define  SUBJMGR_C
#include "local.h"

/*=========================================================================*/
/*   Function : Count_Thread_BBS                                           */
/*                                    �w�肵���̃X�����������邩���ׂ� */
/*=========================================================================*/
Err Count_Thread_BBS(Char *boardNick, UInt16 *count)
{
    UInt16               lp, thrCnt;
    DmOpenRef            dbRef;
    NNshSubjectDatabase  subj;

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &thrCnt);
    *count = 0;
    for (lp = 0; lp < thrCnt; lp++)
    {
        // �X���f�[�^���擾
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
/*                                                  �Y������X���ꗗ���폜 */
/*-------------------------------------------------------------------------*/
Err DeleteSubjectList(Char *boardNick, UInt16 delState, UInt16 *thrCnt)
{
    UInt16              cnt, loop;
    DmOpenRef           dbRef;
    NNshSubjectDatabase tmpDb;
    Char                fileName[MAXLENGTH_FILENAME];

    // DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����
        return (~errNone);
    }
    GetDBCount_NNsh(dbRef, &cnt);

    Show_BusyForm(MSG_DELETE_LIST_WAIT);
    *thrCnt  = 0;
    for (loop = cnt; loop > 0; loop--)
    {
        MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
        GetRecord_NNsh(dbRef, (loop - 1), sizeof(NNshSubjectDatabase), &tmpDb);

        // �w�肳�ꂽBoardID�������烌�R�[�h�폜
        if (StrCompare(tmpDb.boardNick, boardNick) == 0)
        {
            if ((delState == NNSH_SUBJSTATUS_DELETE)||
                (tmpDb.state == NNSH_SUBJSTATUS_NOT_YET))
            {
                // ���f�[�^�t�@�C��������΁A���킹�č폜
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
                // �폜���Ȃ������Ƃ��́A�J�E���g�A�b�v
                (*thrCnt)++;
            }
        }
    }
    Hide_BusyForm();
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   parse_SubjectList                                        */
/*                                                    �X���ꗗ��DB��荞�� */
/*-------------------------------------------------------------------------*/
static Err parse_SubjectList(Char *buffer, UInt32 readSize, Char *boardNick,
                             Char *fieldSeparator, UInt16 sepaLen, UInt16 *cnt)
{
    UInt16              size;
    Char               *ptr, *dataEnd, *dataSepa, *ppp;
    DmOpenRef           dbRef;
    NNshSubjectDatabase tmpDb;

    // DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����
        return (~errNone);
    }
    *cnt = 0;

    // �X�����̂̉��߃��C��(HTTP�w�b�_��ǂݔ�΂�)
    ptr = StrStr(buffer, "\x0d\x0a\x0d\0a");
    if (ptr == NULL)
    {
        // �f�[�^�ُ�
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(\x0d\x0a\x0d\x0a))", 0);
        CloseDatabase_NNsh(dbRef);
        return (~errNone);
    }
    ptr = ptr + 4;

    // �u���X�g�o�^���v�̕\�����s��
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
        /** �X�����i�[�������폜(�ł��x���Ȃ�ȁ[) **/
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

        // ���f�[�^�̐擪�ֈړ�
        ptr = dataEnd + 1;
        (*cnt)++;
    }
    Hide_BusyForm();
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   convertListIndexToMsgIndex                               */
/*                                   ���X�g�ԍ�����X��index�ԍ��ɕϊ����� */
/*=========================================================================*/
UInt16 convertListIndexToMsgIndex(UInt16 listIdx)
{
    UInt16 *idxP, index = 0;

    // ���X�g�ԍ�����X��index�ɕϊ����ĉ�������
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
/*                                                         �X���ꗗ�̎擾  */
/*=========================================================================*/
Err NNsh_GetSubjectList(UInt16 index)
{
    Err                 ret;
    UInt16              cnt, cnt2;
    UInt32              fileSize, readSize;
    Char                getURL[MAX_URL + MAX_URL], *buffer;
    NNshFileRef         fileRef;
    NNshBoardDatabase   bbsData;

    // BBS-DB����BBS�����擾
    ret = Get_BBS_Info(index, &bbsData);
    if (ret != errNone)
    {
        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR,  "Get_BBS_Info() ", "", ret);
        return (ret);
    }

    // �f�[�^�擾��̐ݒ�
    MemSet (getURL, sizeof(getURL), 0x00);
    StrCopy(getURL, bbsData.boardURL);
    if (bbsData.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCat(getURL, bbsData.boardNick);
    }
    StrCat (getURL, FILE_THREADLIST);

    // �f�o�b�O�\��
    NNsh_DebugMessage(ALTID_INFO, bbsData.boardNick, getURL, 0);

    // �z�X�g����X���ꗗ�f�[�^���擾
    if (NNshParam->getAllThread == HTTP_GETSUBJECT_PART)
    {
        // �ꕔ�擾
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, getURL, NULL, NULL,
                            HTTP_GETSUBJECT_START, HTTP_GETSUBJECT_LEN);
    }
    else
    {
        // �S���擾
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, getURL, NULL, NULL,
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    }
    if (ret != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,MSG_ERROR_HTTP_COMM, "[SUBJ]", ret);
            return (ret);
        }

        // �ʐM�^�C���A�E�g����(��M���Ă���Ƃ���܂ŁA�����I�ɉ��߂��s��)
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[SUBJ]", ret);
    }

    // �f�[�^�x�[�X����A(�擾����)�X���f�[�^�𖢓ǂ̂��̂̂ݏ�������
    DeleteSubjectList(bbsData.boardNick, NNSH_SUBJSTATUS_NOT_YET, &cnt);

    // �擾�X�������X�V(cnt == �폜����Ȃ������X���̐�)
    // Get_BBS_Info   (index, &bbsData); ���Ɋ֐��̐擪�Ŏ擾�ς�...
    bbsData.threadCnt = cnt;
    Update_BBS_Info(index, &bbsData);

    ////////// �X���ꗗ�̉�� /////////

    // ��M�f�[�^�i�[�p�o�b�t�@�̊l��
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
        // �X���ꗗ���傫�������Ƃ���(�o�b�t�@�T�C�Y��)�ۂ߂�
        fileSize = NNshParam->bufferSize;
    }

    // �X���ꗗ�̓ǂ݂���
    ret = ReadFile_NNsh(&fileRef, 0, fileSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        // �t�@�C���ǂ݂��݃G���[
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :", FILE_RECVMSG, ret);
    }
    else
    {
        // ���f�[�^�x�[�X�̍X�V(�X���̓o�^)
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
        // Get_BBS_Info   (index, &bbsData); // ���Ɏ擾�ς�...
        bbsData.threadCnt = cnt;
        Update_BBS_Info(index, &bbsData);

        // �f�o�b�O�\��
        NNsh_DebugMessage(ALTID_INFO, "parsed thread :", "", cnt);
    }
    MemPtrFree(buffer);
    return (ret);   
}

/*-------------------------------------------------------------------------*/
/*   function :   create_ThreadList                                        */
/*                       �w�肳�ꂽ�ɏ�������X����INDEX�Ɩ��O�ꗗ���쐬 */
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

    // �u���C�ɓ���v/�u�S�X���擾�v�ȊO�̂Ƃ������ABBS�����擾����
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

    // �C���f�b�N�X�̏�����
    if (NNshGlobal->threadIdxH != 0)
    {
        MemHandleFree(NNshGlobal->threadIdxH);
        NNshGlobal->threadIdxH = 0;
    }
    *cnt = 0;

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &dataCnt);
    ptr       = buffer;
    areaLimit = buffer + bufSize;

    // �X���C���f�b�N�X�̈���m�ۂ���
    allocSize = sizeof(UInt16) * (threadCnt + MARGIN);
    NNshGlobal->threadIdxH = MemHandleNew(allocSize);
    if (NNshGlobal->threadIdxH == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_INFO, "ERR>MemPtrNew(ThreadIndex) ",
                          "size :", allocSize);
        CloseDatabase_NNsh(dbRef);
        ret = (~errNone - 1);
        goto FUNC_END;
    }
    chrIdxP = MemHandleLock(NNshGlobal->threadIdxH);
    MemSet(chrIdxP, allocSize, 0x00);

    // �Y������ɏ�������X����(�f�[�^�x�[�X���珇�Ԃ�)���o����B
    for (loop = dataCnt; loop > 0; loop--)
    {
        // GetRecord_NNsh()���Ń[���N���A���Ă��邽�ߕs�v
        // MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
        GetRecord_NNsh(dbRef, (loop - 1), sizeof(NNshSubjectDatabase), &tmpDb);

        // �\������/���Ȃ��̔������(���ɂ���ďꍇ����)
        switch (index)
        { 
          case NNSH_SELBBS_GETALL:
            // �擾�ςݑS�Ă�\��
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
                //  �u���擾�v���������͎Q�ƃ��O���u�Q�ƃ��O�͕\�����Ȃ��v��
	        // �ꍇ�ɂ͕\�����Ȃ�
                continue;
            }
            break;
          case NNSH_SELBBS_FAVORITE:
            // ���C�ɓ���w���\��
            if ((tmpDb.msgAttribute & NNSH_MSGATTR_FAVOR)!= NNSH_MSGATTR_FAVOR)
            {
                // �u���C�ɓ���v�ł͂Ȃ��̂ŕ\�����Ȃ�
                continue;
            }
            break;

          case NNSH_SELBBS_NOTREAD:
            // ���ǂ����\��
            if ((tmpDb.state != NNSH_SUBJSTATUS_NEW)&&
                (tmpDb.state != NNSH_SUBJSTATUS_UPDATE)&&
                (tmpDb.state != NNSH_SUBJSTATUS_REMAIN))
            {
                // ���ǂ�����X���ł͂Ȃ��̂ŕ\�����Ȃ�
                continue;
            }
            break;

          default:
            // �u���C�ɓ���v�ł��u�擾�ςݑS�āv�ł��u���ǂ���v�ł��Ȃ��ꍇ
            if (StrCompare(tmpDb.boardNick, bbsData.boardNick))
            {
                // �X���̏������Ⴄ�̂ŕ\�����Ȃ�
                continue;
            }
            break;
        }

        // ���̃^�C�g�����R�s�[����̈悪�]���Ă���ꍇ�A�^�C�g�����R�s�[����
        // (8�́A�ŏ��� "[xxxx]" ����(�o�C�g��))
        if ((ptr + (NNshParam->titleListSize + 8)) < areaLimit)
        {
            // ���b�Z�[�W�擾��Ԃ�\��
            switch (tmpDb.state)
            {
              case NNSH_SUBJSTATUS_NEW:
                /** �V�K�擾(�Ď擾) **/
                *ptr = '*';
                break;

              case NNSH_SUBJSTATUS_UPDATE:
                if ((tmpDb.msgAttribute & NNSH_MSGATTR_ERROR) ==
                     NNSH_MSGATTR_ERROR)
                {
                    /** ���b�Z�[�W�擾���ɃG���[���������� **/
                    *ptr = '#';
                    break;
                }
                /** ���b�Z�[�W�X�V **/
                *ptr = '+';
                break;

              case NNSH_SUBJSTATUS_REMAIN:
                if ((tmpDb.msgAttribute & NNSH_MSGATTR_ERROR) ==
                     NNSH_MSGATTR_ERROR)
                {
                    /** ���b�Z�[�W�擾���ɃG���[���������� **/
                    *ptr = '#';
                    break;
                }
                /** ���ǂ��� **/
                *ptr = '-';
                break;

              case NNSH_SUBJSTATUS_ALREADY:
                if ((tmpDb.msgAttribute & NNSH_MSGATTR_ERROR) ==
                     NNSH_MSGATTR_ERROR)
                {
                    /** ���b�Z�[�W�擾���ɃG���[���������� **/
                    *ptr = '#';
                    break;
                }
                /** ���b�Z�[�W�S���ǂ� **/
                *ptr = ':';
                break;

              case NNSH_SUBJSTATUS_NOT_YET:
              default:                
                /** ���b�Z�[�W���擾 **/
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
                    // �擾���b�Z�[�W����\������
                    StrCat(ptr, "[");
                    (void) NUMCATI(ptr, tmpDb.maxLoc);
                    StrCat(ptr, "]");
                }
                else
                {
                    StrCat(ptr, "[???]");
                }
            }
            // �^�C�g�����R�s�[����
            StrNCopy(&ptr[StrLen(ptr)],
                     tmpDb.threadTitle, NNshParam->titleListSize);

            // ���̗̈�փ|�C���^���ړ�������
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
/*                                          �Y����������̃X�������擾���� */
/*=========================================================================*/
Err Get_Thread_Count(UInt16 index, UInt16 *count)
{
    Err                  ret;
    UInt16               dataCnt, loop;
    DmOpenRef            dbRef;
    NNshSubjectDatabase  tmpDb;
    NNshBoardDatabase    bbsData;

    // �X������������
    *count     = 0;

    // "���C�ɓ���"�ł͂Ȃ��X���Ȃ̂ŁA�a�a�r���Ɋi�[����Ă���X����������
    if ((index != NNSH_SELBBS_FAVORITE)&&
        (index != NNSH_SELBBS_GETALL)&&
        (index != NNSH_SELBBS_NOTREAD))
    {
        ret = Get_BBS_Info(index, &bbsData);
        if (ret != errNone)
        {
            goto FUNC_END;
        }

        // �X�������A�u�v�Čv�Z�v�ƂȂ��Ă����ꍇ�ɂ́A�Čv�Z����
        *count = bbsData.threadCnt;
        if (bbsData.threadCnt == NNSH_BBSLIST_AGAIN)
        {
            // �X�����𐔂��A�X�V
            (void) Count_Thread_BBS(bbsData.boardNick, count);
            if (*count != NNSH_BBSLIST_AGAIN)
            {
                bbsData.threadCnt = *count;
                Update_BBS_Info(index, &bbsData);
            }
        }
        goto FUNC_END;
    }

    // ...���C�ɓ���/�S�X���擾�̏ꍇ...

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &dataCnt);

    // ���C�ɓ���X��/�擾�ς݃X���������邩������
    for (loop = dataCnt; loop > 0; loop--)
    {
        // MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
        GetRecord_NNsh(dbRef, (loop - 1), sizeof(NNshSubjectDatabase), &tmpDb);
        switch (index)
        {
          case NNSH_SELBBS_GETALL:
            // �擾�ς݃X���̐��𐔂���
            if (tmpDb.state != NNSH_SUBJSTATUS_NOT_YET)
            {
                (*count)++;
            }
            break;

          case NNSH_SELBBS_NOTREAD:
            // ���ǂ���X���̐��𐔂���
            if ((tmpDb.state == NNSH_SUBJSTATUS_NEW)||
                (tmpDb.state == NNSH_SUBJSTATUS_UPDATE)||
                (tmpDb.state == NNSH_SUBJSTATUS_REMAIN))
            {
                (*count)++;
            }
            break;

          case NNSH_SELBBS_FAVORITE:
          default:
            // ���C�ɓ���X���̐��𐔂���
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
/*                                      �����X����DB�Ɋ܂܂�Ă��邩���ׂ� */
/*-------------------------------------------------------------------------*/
Err check_same_thread(UInt16 selMES, NNshSubjectDatabase *data, 
                      NNshSubjectDatabase *matchedData, UInt16 *idx)
{
    Err                  err = ~errNone;
    UInt16               lp, *idxP;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subj;

    // �擾�����f�[�^�x�[�X�̏����i�[����̈���m��
    subj = MemPtrNew(sizeof(NNshSubjectDatabase));
    if (subj == NULL)
    {
        NNsh_DebugMessage(ALTID_INFO, "MemPtrNew()", "", 
                          sizeof(NNshSubjectDatabase));
        return (~errNone);
    }

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
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
        // �X���f�[�^���擾
        if ((GetRecord_NNsh(dbRef, idxP[lp], 
                            sizeof(NNshSubjectDatabase), subj)) != errNone)
        {
            // �G���[����
            err = ~errNone;
            break;
        }
        if ((StrCompare(subj->boardNick,      data->boardNick)      == 0)&&
            (StrCompare(subj->threadFileName, data->threadFileName) == 0))
        {
            // ����̃X�������݂����I
            err  = errNone;
            NNsh_DebugMessage(ALTID_INFO, subj->threadFileName, 
                              data->threadFileName, lp);
            if (matchedData != NULL)
            {
                // �Y�������f�[�^���R�s�[���ĉ�������
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
