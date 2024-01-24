/*============================================================================*
 *
 *  $Id: subjmgr.c,v 1.39 2005/02/10 14:56:42 mrsa Exp $
 *
 *  FILE: 
 *     subjmgr.c
 *
 *  Description: 
 *     �X���ꗗ�擾�T�u
 *===========================================================================*/
#define  SUBJMGR_C
#include "local.h"

/*=========================================================================*/
/*   Function :   DeleteSubjectList                                        */
/*                                                  �Y������X���ꗗ���폜 */
/*=========================================================================*/
Err DeleteSubjectList(Char *boardNick, UInt16 delState, UInt16 *thrCnt)
{
    UInt16              cnt, loop, isOffline;
    DmOpenRef           dbRef;
    NNshSubjectDatabase tmpDb;
    Char                fileName[MAXLENGTH_FILENAME];

    isOffline = StrLen(FILE_LOGCHARGE_PREFIX);

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

    // GetRecord_NNsh �̕Ԃ�l��p���鎖�ō�����������
    // ���������炢��Ȃ��͂������A�O�̂��߁A�P�񏉊���������
    MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
    
    for (loop = cnt; loop > 0; loop--)
    {
        if (GetRecord_NNsh(dbRef, (loop - 1), sizeof(NNshSubjectDatabase), &tmpDb) == errNone)
        {
            // �w�肳�ꂽBoardID�������烌�R�[�h�폜
            if ((delState == NNSH_SUBJSTATUS_DELETE_OFFLINE)&&
                (StrNCompare(tmpDb.threadFileName, FILE_LOGCHARGE_PREFIX, isOffline) == 0))
            {
                // �폜���Ȃ������Ƃ��́A�J�E���g�A�b�v
                (*thrCnt)++;
            }
            else if ((delState == NNSH_SUBJSTATUS_UNKNOWN)||
                      (StrCompare(tmpDb.boardNick, boardNick) == 0))
            {
                //  �X���擾�\�񂳂�Ă��Ȃ��āA���ǃf�[�^ or 
                // �S�폜�w����������폜
                if ((delState == NNSH_SUBJSTATUS_DELETE)||
                     (delState == NNSH_SUBJSTATUS_DELETE_OFFLINE)||
                    (((tmpDb.msgAttribute & NNSH_MSGATTR_GETRESERVE) == 0)&&
                     (tmpDb.state == NNSH_SUBJSTATUS_NOT_YET)))
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
    }
    Hide_BusyForm(true);
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));
    return (errNone);
}

/*==========================================================================*/
/*  SortSubjectList : SUBJECT�f�[�^�x�[�X���\�[�g���㏑������               */
/*                                                                          */
/*==========================================================================*/
void SortSubjectList(void)
{
    DmOpenRef            dbRef;

    // �X�����Ǘ�DB�I�[�v�����A�\�[�g����
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_SUBJECT, 0);
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  entryNotExistSubject                                      */
/*                                        �X���^�C�g�������݂��Ȃ���Γo�^ */
/*-------------------------------------------------------------------------*/
static void entryNotExistSubject(DmOpenRef dbRef, NNshSubjectDatabase *subjDb)
{
    UInt16               loop, nofRecords;
    MemHandle            recH;
    NNshSubjectDatabase *chkDb;

    // �o�^�ς݃��R�[�h�����l��
    GetDBCount_NNsh(dbRef, &nofRecords);

    // �o�^�ς݂��S��(�������)�`�F�b�N����
    for (loop = 0; loop < nofRecords; loop++)
    {
        (void) GetRecordReadOnly_NNsh(dbRef, loop, &recH, (void **) &chkDb);

        // �o�^�ς݂��ǂ����`�F�b�N 
        if (StrCompare(chkDb->threadFileName, subjDb->threadFileName) == 0)
        {
            if (StrCompare(chkDb->boardNick, subjDb->boardNick) == 0)
            {
                // �{�[�h�t�@�C�����ƃ{�[�h�j�b�N�l�[���������A���łɓo�^�ς�
                ReleaseRecordReadOnly_NNsh(dbRef, recH);
                return;
            }
        }
        ReleaseRecordReadOnly_NNsh(dbRef, recH);
    }

    // �X�����o�^����Ă��Ȃ������A���R�[�h��V�K�o�^����
    (void) EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), subjDb);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   parse_SubjectList                                        */
/*                                                    �X���ꗗ��DB��荞�� */
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
        // �^�C�g���o�b�t�@�i�[�̈�m�ێ��s
        *parseSize = readSize;
        return (~errNone);
    }

    // BBS�^�C�v�ɂ���āA�t�B�[���h�Z�p���[�^��I������
    switch (bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // �܂�BBS �܂��� �������@JBBS
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
        // �Q�����˂� �܂��� �������
        fieldSeparator = NNSH_FIELD_SEPARATOR;
        break;
    }
    sepaLen = StrLen(fieldSeparator);

    // �����parse���̂�HTTP�w�b�_��ǂݔ�΂�
    if (*parseSize == 0)
    {
        // �X�����̂̉��߃��C��(HTTP�w�b�_��ǂݔ�΂�)
        ptr = StrStr(buffer, NNSH_HTTP_HEADER_ENDTOKEN);
        if (ptr == NULL)
        {
            // �f�[�^�ُ�
            NNsh_DebugMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                               NNSH_HTTP_HEADER_ENDTOKEN, 0);
            MEMFREE_PTR(titleBuf);
            *parseSize = readSize;
            return (~errNone);
        }
        ptr = ptr + 4;   // 4�́A"\x0d\x0a\x0d\0a"
    }
    else
    {
        // 2��ڈȍ~��parse...
        ptr = buffer;
    }

    // �u���X�g�o�^���v�̕\�����s��
    Show_BusyForm(MSG_THREADLIST_CREATE);

    // DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����
        MEMFREE_PTR(titleBuf);
        *parseSize = readSize;
        return (~errNone);
    }

    checkDuplicateMethod = NNSH_DUP_NOCHECK;
    if ((NNshGlobal->NNsiParam)->checkDuplicateThread != 0)
    {
        // �X���̏d���`�F�b�N����̏ꍇ...
        // �X���̃��X�g���擾����
        haveThreadList = 
                GetExistRecordList(dbRef, boardNick, &threadListH, &threadNum);
        if (haveThreadList == false)
        {
            // �o�^�ς݂c�a�̃��R�[�h�������m�F
            if (threadNum != 0)
            {
                //  �������s�������A�x���Ȃ��Ă��d���`�F�b�N���邩�ǂ���
                // �m�F�����߂郁�b�Z�[�W��\������
                if (NNsh_ErrorMessage(ALTID_CONFIRM, MSG_SELECT_DUPLICATE_METHOD, "", 0) == 0)
                {
                    // �x���Ȃ��Ă��A�d���`�F�b�N����(�ᑬ�d���`�F�b�N
                    checkDuplicateMethod = NNSH_DUP_CHECK_SLOW;
                }
                else
                {
                    // �x���Ȃ邽�߁A�d���`�F�b�N���Ȃ�
                    checkDuplicateMethod = NNSH_DUP_NOCHECK;
                }
            }
            else
            {
                // DB�ɂP�����f�[�^���Ȃ����߁A�d���`�F�b�N���Ȃ�
                checkDuplicateMethod = NNSH_DUP_NOCHECK;
            }
        }
        else 
        {
            // �d���`�F�b�N�̎��{
            checkDuplicateMethod = NNSH_DUP_CHECK_HIGH;
        }
    }

    // �X���^�C���o�^����Ă����A�o�^�Ɠ����ɐV���m�F����H
    if ((NNshGlobal->NNsiParam)->listAndUpdate != 0)
    {
        // �X���Ԏw��擾�pDB���I�[�v�����Ă���
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
        //  ���R�[�h�f�[�^�̖�������������iNNSH_RECORD_SEPARATOR == "\x0a"�j
        dataEnd = ptr;
        while ((*dataEnd != '\x0a')&&(*dataEnd != '\0'))
        {
            dataEnd++;
        }
        if (*dataEnd == '\0')
        {
            // �f�[�^�I���A���[�v�𔲂���
            dataEnd = NULL;
            break;
        }

        // �t�B�[���h�̃Z�p���[�^����������
        dataSepa = StrStr(ptr, fieldSeparator);
        if ((dataSepa == NULL)||(dataSepa > dataEnd))
        {
            // �f�[�^�ُ�A���[�v�𔲂���
            break;
        }

        // ���X���i�[�����𒊏o����
        MemSet  (&tmpDb, sizeof(tmpDb), 0x00);

        // BBS�^�C�v�P�ʂŃ��X���A�X���^�C�A�t�@�C�������i�[����
        switch (bbsType)
        {
          case NNSH_BBSTYPE_SHITARABA:
            // ���X��(������΂������ƃ��X���̊i�[���@���Ⴄ�A����)
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

            // �X���t�@�C����
            StrNCopy(tmpDb.threadFileName, ptr, (dataSepa - ptr));
            size = ((dataEnd - (dataSepa + 2)) > MAX_THREADNAME) ? 
                             (MAX_THREADNAME - 1) : (dataEnd - (dataSepa + 2));

            // ������΂�subject.txt�ɂ͊g���q�������Ă��Ȃ��̂ŁA�����ŕt��
            StrCat(tmpDb.threadFileName, ".dat");

            // �X���^�C�g���iEUC�����R�[�h��SHIFT JIS�����R�[�h�ɕϊ�����j
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
            /** ���X���i�[�������폜(�ł��x���Ȃ�ȁ[) **/
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

            // �t�@�C����
            StrNCopy(tmpDb.threadFileName, ptr, (dataSepa - ptr));
            size = ((dataEnd - (dataSepa + 2)) > MAX_THREADNAME) ? 
                             (MAX_THREADNAME - 1) : (dataEnd - (dataSepa + 2));

            // �X���^�C�g��(EUC�����R�[�h�������ꍇ�́ASHIFT JIS�ɕϊ�)
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
            /** ���X���i�[�������폜(�ł��x���Ȃ�ȁ[) **/
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

            // �t�@�C����
            StrNCopy(tmpDb.threadFileName, ptr, (dataSepa - ptr));
            size = ((dataEnd - (dataSepa + 2)) > MAX_THREADNAME) ? 
                             (MAX_THREADNAME - 1) : (dataEnd - (dataSepa + 2));

            // �X���^�C�g���iSHIFT JIS�����R�[�h�������ꍇ�́A���̂܂܊i�[�j
            StrNCopy(tmpDb.threadTitle, (dataSepa + sepaLen), size);
            break;
        }

        // �X���^�C�g���ꗗ��BBS TYPE���i�[����B
        tmpDb.bbsType = (UInt8) bbsType; 
        StrCopy (tmpDb.boardNick, boardNick);

        // �d���`�F�b�N�̎��{
        switch(checkDuplicateMethod)
        {
          case NNSH_DUP_CHECK_SLOW:
            // �d���`�F�b�N���{(�ᑬ��)�A�X���^�C�����݂��Ȃ���Γo�^����
            (void) entryNotExistSubject(dbRef, &tmpDb);
            break;

          case NNSH_DUP_CHECK_HIGH:
            // �d���`�F�b�N(������)�����{
            threadListP = MemHandleLock(threadListH);
            ppp = StrStr(threadListP, tmpDb.threadFileName);
            if(ppp == NULL)
            {
                // �X���^�C���o�^�A�V�K�o�^����
                (void)
                  EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &tmpDb);
            }
            else
            {
                // �X���^�C���o�^����Ă����A�o�^�Ɠ����ɐV���m�F����H
                if ((NNshGlobal->NNsiParam)->listAndUpdate != 0)
                {
                    // �o�^���ƍ���̃��X�����`�F�b�N����
                    mesNum = StrAToI(ppp + StrLen(tmpDb.threadFileName));
                    if ((mesNum != 0)&&(mesNum != tmpDb.maxLoc))
                    {
                        // �V���m�F�����{����X�������I(�X���Ԓ��o)
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
                            // �X���Ԏw��擾�\��c�a�ɃG���g������
                            (void) EntryRecord_NNsh(threadDbRef, BUFSIZE, titleBuf);
                        }
                    }                    
                }
            }
            MemHandleUnlock(threadListH);
            break;

          case NNSH_DUP_NOCHECK:
          default:
            // �d���`�F�b�N�͍s�킸�A�X���^�C��������(�V�K)�o�^����
            (void)EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &tmpDb);
            break;
        }

        // ���f�[�^�̐擪�ֈړ�
        ptr = dataEnd + 1;
        (*cnt)++;

        *parseSize = (ptr - buffer);
    }
    // �d���m�F�p����������Ă����ꍇ�A�̈���J������B
    if (haveThreadList != false)
    {
        MemHandleFree(threadListH);
    }
    Hide_BusyForm(true);
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    // �X���Ԏw��擾�c�a���I�[�v������Ă����Ƃ��ɂ̓N���[�Y����
    if (threadDbRef != 0)
    {
        CloseDatabase_NNsh(threadDbRef);
    }

    MEMFREE_PTR(titleBuf);
    return (errNone);
}

/*=========================================================================*/
/*   Function :   convertListIndexToMsgIndex                               */
/*                                   ���X�g�ԍ�����X��index�ԍ��ɕϊ����� */
/*=========================================================================*/
UInt16 convertListIndexToMsgIndex(UInt16 listIdx)
{
    // ���X�g�ԍ���DB�̃��R�[�h�ԍ��ɕϊ����ĉ�������
    return (NNshGlobal->msgListIndex[listIdx]);
}

/*=========================================================================*/
/*   Function :   NNsh_GetSubjectList                                      */
/*                                                         �X���ꗗ�̎擾  */
/*=========================================================================*/
Err NNsh_GetSubjectList(UInt16 index)
{
    Err                 ret, ret2;
    UInt16              cnt, cnt2;
    UInt32              fileSize, readSize, topSize, parseSize, getSize;
    Char                getURL[MAX_URL + MAX_URL], *buffer;
    NNshFileRef         fileRef;
    NNshBoardDatabase  *bbsData;

    // BBS���i�[�̈���m�ۂ���
    bbsData = MEMALLOC_PTR(sizeof(NNshBoardDatabase) + MARGIN);
    if (bbsData == NULL)
    {
        return (~errNone);
    }

    // BBS-DB����BBS�����擾
    ret = Get_BBS_Info(index, bbsData);
    if (ret != errNone)
    {
        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR,  "Get_BBS_Info() ", "", ret);
        MEMFREE_PTR(bbsData);
        return (ret);
    }

    // �f�[�^�擾��̐ݒ�
    MemSet (getURL, sizeof(getURL), 0x00);
    StrCopy(getURL, bbsData->boardURL);
    switch (bbsData->bbsType)
    {
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
      case NNSH_BBSTYPE_MACHIBBS:
        // �������@JBBS/�܂�BBS�̂Ƃ��́ABoardNick��t��
        StrCat(getURL, bbsData->boardNick);
        break;

      case NNSH_BBSTYPE_SHITARABA:
        // ������΂̂Ƃ��́Abbs/BoardNick��t��
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
        // �Q�����˂�̎��́A�Ȃɂ����Ȃ�
        break;
    }
    StrCat (getURL, FILE_THREADLIST);

    // �f�o�b�O�\��
    NNsh_DebugMessage(ALTID_INFO, bbsData->boardNick, getURL, 0);

    // �z�X�g����X���ꗗ�f�[�^���擾
    if ((NNshGlobal->NNsiParam)->getAllThread == HTTP_GETSUBJECT_PART)
    {
        // �ꕔ�擾
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, getURL, NULL, NULL,
                            HTTP_GETSUBJECT_START, HTTP_GETSUBJECT_LEN, NULL);
    }
    else
    {
        // �S���擾
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, getURL, NULL, NULL,
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    }
    if (ret != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,MSG_ERROR_HTTP_COMM, "[SUBJ]", ret);
            MEMFREE_PTR(bbsData);
            return (ret);
        }

        // �ʐM�^�C���A�E�g����(��M���Ă���Ƃ���܂ŁA�����I�ɉ��߂��s��)
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[SUBJ]", ret);
    }

    // �f�[�^�x�[�X����A(�擾����)�X���f�[�^�𖢓ǂ̂��̂̂ݏ�������
    DeleteSubjectList(bbsData->boardNick, NNSH_SUBJSTATUS_NOT_YET, &cnt);

    ////////// �X���ꗗ�̉�� /////////

    // ��M�f�[�^�i�[�p�o�b�t�@�̊l��
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

    // �X���ǂݍ��݃T�C�Y�O�Z��̏�����
    topSize   = 0;
    parseSize = topSize;
    getSize   = 0;

    // �X���ꗗ�̓ǂ݂���
READ_AGAIN:
    ret = ReadFile_NNsh(&fileRef, topSize, (NNshGlobal->NNsiParam)->bufferSize,
                        buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        // �t�@�C���ǂ݂��݃G���[
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :", FILE_RECVMSG, ret);
        ret2 = ret;
        getSize = fileSize;
    }
    else
    {
        // ���f�[�^�x�[�X�̍X�V(�X���̓o�^)
        ret2 = parse_SubjectList(buffer, readSize, &parseSize,
                                bbsData->bbsType, bbsData->boardNick, &cnt2);
        cnt = cnt + cnt2;
    }
    topSize = topSize + parseSize;
    getSize = getSize + readSize;
    if (getSize < fileSize)
    {
        // ���X����͂����ׂčς�ł��Ȃ��ꍇ�ɂ́A������x�ǂݒ���
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
/*                                      �����X����DB�Ɋ܂܂�Ă��邩���ׂ� */
/*=========================================================================*/
Err Check_same_thread(UInt16 selMES, NNshSubjectDatabase *data, 
                      NNshSubjectDatabase *matchedData, UInt16 *idx)
{
    Err                  err = ~errNone;
    Int16                step;
    UInt16               lp, index, count;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subj;

    // �擾�����f�[�^�x�[�X�̏����i�[����̈���m��
    subj = MEMALLOC_PTR(sizeof(NNshSubjectDatabase));
    if (subj == NULL)
    {
        NNsh_DebugMessage(ALTID_INFO, "MEMALLOC_PTR()", "", 
                          sizeof(NNshSubjectDatabase));
        return (~errNone);
    }

    // �u�d���X���̃`�F�b�N��...�v�\��
    Show_BusyForm(MSG_CHECK_SAME_THREAD);

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // DB�I�[�v�����s(���肦�Ȃ��͂�����...)
        goto FUNC_END;
    }
    GetDBCount_NNsh(dbRef, &count);

    // �擾�\��̔ԍ����擾
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

    // �d���X�����ǂ�����������
    for (;((lp >= 0)&&(lp < count)); lp = lp + step)
    {
        // �X���f�[�^���擾
        if ((GetRecord_NNsh(dbRef, lp,
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
    CloseDatabase_NNsh(dbRef);

    // �u�d���X���̃`�F�b�N��...�v�\��������
    Hide_BusyForm(false);

FUNC_END:
    MEMFREE_PTR(subj);
    return (err);
}

/*=========================================================================*/
/*   Function :   GetSubjectIndex                                          */
/*                                        �X���h�c�̃C���f�b�N�X�ԍ����擾 */
/*=========================================================================*/
Err GetSubjectIndex(Char *boardNick, Char *threadFileName, UInt16 *index)
{
    Err                 err = ~errNone;
    NNshSubjectDatabase subj;
    DmOpenRef           dbRef;
    Int16               lp;
    UInt16              count;

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // DB�I�[�v�����s(���肦�Ȃ��͂�����...)
        return (~errNone - 10);
    }
    GetDBCount_NNsh(dbRef, &count);

    // �X�������݂��邩�m�F����
    for (lp = (count - 1); lp >= 0; lp--)
    {
        // �X���f�[�^���擾
        if ((GetRecord_NNsh(dbRef, lp,
                            sizeof(NNshSubjectDatabase), &subj)) != errNone)
        {
            // �G���[����
            err = ~errNone;
            break;
        }
        if ((StrCompare(subj.boardNick,      boardNick)      == 0)&&
            (StrCompare(subj.threadFileName, threadFileName) == 0))
        {
            // �X�������݂����I
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
/*                                              ���[�U�^�u���L�����`�F�b�N */
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
        // �Q�ƃ��O�͏��O����ꍇ
#ifdef USE_STRSTR
        if (!(StrCompare(subjP->boardNick, OFFLINE_THREAD_NICK)))
#else
        if ((subjP->boardNick[0]  == '!')&&
            (subjP->boardNick[1]  == 'G'))
#endif
        {
            // �Q�ƃ��O�������ꍇ(�������珜�O)
            return (false);
        }
    }

    // �S�Ĉ�v or �ǂꂩ��v �̏������擾
    condition = (customTab->condition & NNSH_CONDITION_ALL);

    ////////////////////////////////////////////////////////////
    // �ݒ���̊m�F
    ////////////////////////////////////////////////////////////
    if ((customTab->boardCondition != 0)&&(bbsName != NULL))
    {
        status = customTab->boardCondition - 1;
        switch (status)
        {
          case NNSH_BOARD_ELSE:
            // ����v���Ȃ�(�ȊO)
            if ((StrCompare(bbsName, subjP->boardNick)) == 0)
            {
                // ������v�A���O
                ret = false;
                goto CHECK_LEVEL;
            }
            break;

          case NNSH_BOARD_MATCH:
          default:
            // ����v(��)
            if ((StrCompare(bbsName, subjP->boardNick)) != 0)
            {
                // ������v���Ȃ��A���O
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
    // �X�����x���̊m�F
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
            // �ȉ�
            if (favorLvl > thLevel)
            {
                // ���x�����傫����Ώ��O
                ret = false;
                goto CHECK_LOGLOC;
            }
            break;
          case NNSH_THREADLEVEL_ELSE:
            // �ȊO
            if (favorLvl == thLevel)
            {
                // ���x���������Ȃ�Ώ��O
                ret = false;
                goto CHECK_LOGLOC;
            }
            break;
          case NNSH_THREADLEVEL_MATCH:
            // ��v
            if (favorLvl != thLevel)
            {
                // ���x����������珜�O
                ret = false;
                goto CHECK_LOGLOC;
            }
            break;
          case NNSH_THREADLEVEL_UPPER:
          default:
            // �ȏ�
            if (favorLvl < thLevel)
            {
                // ���x������������Ώ��O
                ret = false;
                goto CHECK_LOGLOC;
            }
            break;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            // "�ǂꂩ��v" �̎�
            return (true);
        }
    }

CHECK_LOGLOC:
    ////////////////////////////////////////////////////////////
    // ���O�ʒu�̊m�F
    ////////////////////////////////////////////////////////////
    status = (customTab->condition & NNSH_CONDITION_LOGLOC_ALL);
    if (status != 0)
    {
        if ((status == NNSH_CONDITION_LOGLOC_PALM)&&
            (subjP->msgState != FILEMGR_STATE_OPENED_STREAM))
        {
            // ���O�t�@�C����Palm�f�o�C�X���ɑ��݂��Ȃ��Ƃ�
            ret = false;
            goto CHECK_GETERR;
        }
        else if ((status == NNSH_CONDITION_LOGLOC_VFS)&&
                 (subjP->msgState != FILEMGR_STATE_OPENED_VFS))
        {
            // ���O�t�@�C�����u�e�r���ɑ��݂��Ȃ��Ƃ�
            ret = false;
            goto CHECK_GETERR;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            // "�ǂꂩ��v" �̎�
            return (true);
        }
    }

CHECK_GETERR:
    ////////////////////////////////////////////////////////////
    // �擾�G���[�̊m�F
    ////////////////////////////////////////////////////////////
    status = (customTab->condition & NNSH_CONDITION_GETERR_ALL);
    if (status != 0)
    {
        if ((status == NNSH_CONDITION_GETERR_NONE)&&
            ((subjP->msgAttribute & NNSH_MSGATTR_ERROR) != 0))
        {
            // �G���[��Ԃ̂Ƃ�
            ret = false;
            goto CHECK_GETRESERVE;
        }
        else if ((status == NNSH_CONDITION_GETERR_ERROR)&&
                 ((subjP->msgAttribute & NNSH_MSGATTR_ERROR) == 0))
        {
            // �G���[��ԂłȂ��Ƃ�
            ret = false;
            goto CHECK_GETRESERVE;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            // "�ǂꂩ��v" �̎�
            return (true);
        }
    }

CHECK_GETRESERVE:
    ////////////////////////////////////////////////////////////
    // �擾�ۗ���Ԃ̊m�F
    ////////////////////////////////////////////////////////////
    status = (customTab->condition & NNSH_CONDITION_GETRSV_ALL);
    if (status != 0)
    {
        if ((status == NNSH_CONDITION_GETRSV_RSV)&&
            (subjP->msgAttribute & NNSH_MSGATTR_GETRESERVE) != NNSH_MSGATTR_GETRESERVE)
        {
            // �擾�ۗ���
            ret = false;
            goto CHECK_STATUS;
        }
        else if ((status == NNSH_CONDITION_GETRSV_NONE)&&
                 (subjP->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
        {
            // ��擾�ۗ���
            ret = false;
            goto CHECK_STATUS;
        }
        if (condition == NNSH_CONDITION_OR)
        {
            // "�ǂꂩ��v" �̎�
            return (true);
        }
    }

CHECK_STATUS:
    ////////////////////////////////////////////////////////////
    // �X����Ԃ̊m�F
    ////////////////////////////////////////////////////////////
    if (customTab->threadStatus != 0)
    {
        subRet = true;

        // �V�K�擾�X��
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_NEW) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_NEW))
        {
            goto CHECK_THREADSTATUS;
        }

        // �X�V�X��
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_UPDATE) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_UPDATE))
        {
            goto CHECK_THREADSTATUS;
        }

        // ���ǂ���X��
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_REMAIN) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_REMAIN))
        {
            goto CHECK_THREADSTATUS;
        }

        // ���ǃX��
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_ALREADY) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_ALREADY))
        {
            goto CHECK_THREADSTATUS;
        }

        // �ő�𒴂����X��
        if (((customTab->threadStatus >> NNSH_SUBJSTATUS_OVER) & 1)!= 0)
        {
            if ((subjP->maxLoc > NNSH_MESSAGE_LIMIT)&&
                (subjP->state == NNSH_SUBJSTATUS_ALREADY))
            {
                goto CHECK_THREADSTATUS;
            }
        }

        // �܂��擾���Ă��Ȃ��X��
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_NOT_YET) & 1)!= 0)&&
            (subjP->state == NNSH_SUBJSTATUS_NOT_YET))
        {
            goto CHECK_THREADSTATUS;
        }

        // �擾�G���[�H
        if ((((customTab->threadStatus >> NNSH_SUBJSTATUS_GETERROR) & 1)!= 0)&&
            ((subjP->msgAttribute & NNSH_MSGATTR_ERROR) != 0))
        {
            goto CHECK_THREADSTATUS;
        }

        // ���̑��X��
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
                // "�ǂꂩ��v" �̎�
                return (true);
            }
        }
        else
        {
            ret = false;
        }
    }

    // �X�����Ă����ȓ��̃`�F�b�N
    if (customTab->threadCreate != 0)
    {
        // �`�F�b�N�����̎擾
        currentTime = TimGetSeconds()
                             - TIME_CONVERT_1904_1970
                             - StrAToI(subjP->threadFileName);
        thrTime = customTab->threadCreate;
        thrTime = thrTime * 60 * 60 * 24;
        if (currentTime < thrTime)
        {
                // n���ȓ��ɗ������X���ƔF��
            // NNsh_DebugMessage(ALTID_INFO, "Time:", "", currentTime);
            if (condition == NNSH_CONDITION_OR)
            {
                // "�ǂꂩ��v" �̎�
                return (true);
            }
        }
        else
        {
            ret = false;
        }
    }

    ////////////////////////////////////////////////////////////
    // ����������̃`�F�b�N
    ////////////////////////////////////////////////////////////
    if ((customTab->stringSet & NNSH_STRING_SELECTION) != 0)
    {
        status = customTab->stringSet & NNSH_STRING_SETMASK;
        subRet = true;

        // ����������̃`�F�b�N
        if (customTab->string1[0] != '\0')
        {
            if (StrStr(subjP->threadTitle, customTab->string1) != NULL)
            {
                if (status == NNSH_CONDITION_OR)
                {
                    // "�ǂꂩ��v" �̎�
                    goto CHECK_STRINGSET;
                }
            }
            else
            {
                    subRet = false;
            }
        }

        // ����������̃`�F�b�N
        if (customTab->string2[0] != '\0')
        {
            if (StrStr(subjP->threadTitle, customTab->string2) != NULL)
            {
                if (status == NNSH_CONDITION_OR)
                {
                    // "�ǂꂩ��v" �̎�
                    subRet = true;
                    goto CHECK_STRINGSET;
                }
            }
            else
            {
                    subRet = false;
            }
        }

        // ����������̃`�F�b�N
        if (customTab->string3[0] != '\0')
        {
            if (StrStr(subjP->threadTitle, customTab->string3) != NULL)
            {
                if (status == NNSH_CONDITION_OR)
                {
                    // "�ǂꂩ��v" �̎�
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
                // "�ǂꂩ��v" �̎�
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
