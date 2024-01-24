/*============================================================================*
 *
 *  $Id: getlog.c,v 1.90 2005/12/24 12:26:25 mrsa Exp $
 *
 *  FILE: 
 *     getlog.c
 *
 *  Description: 
 *     �Q�ƃ��O�̃��O�擾
 *===========================================================================*/
#define GETLOG_C
#include "local.h"

#ifdef USE_LOGCHARGE
/*--------------------------------------------------------------------------*/
/*  getLogFileList() : ���O�t�@�C������擾���ׂ�URL�̈ꗗ���擾����        */
/*                             ���� �v���t�@�N�^�����O�I�I (Phase 2) ����   */
/*                                                                          */
/*           �� URL�̈ꗗ�́ADBNAME_TEMPURL �Ƃ��� DB�Ɋi�[����B           */
/*                                                                          */
/*                    fileName :                                            */
/*                    baseURL  :                                            */
/*                    tokenId  :                                            */
/*                    tempURL  :  (�f�[�^���ꎞ�I�ɐ؂�o�����[�N�o�b�t�@)  */
/*--------------------------------------------------------------------------*/
static Err getLogFileList(Char *fileName, Char *baseURL, UInt32 tokenId, NNshGetLogTempURLDatabase *tempURL)
{
    NNshFileRef                 fileRef;
    NNshLogTokenDatabase       *logToken;
    Char                       *buffer,  *ptr, *ptr2, *chkPtr;
    Char                       *tempPtr, *tempBuf;
    DmOpenRef                   dbRef;
    UInt32                      offset, readSize;
    UInt16                      count,  status, bufCnt;
    Err                         ret;

    // �e���|�����̃��������m��
    tempBuf = MEMALLOC_PTR(MAX_GETLOG_URL + MARGIN);
    if (tempBuf == NULL)
    {
        return (~errNone - 9);
    }
    MemSet(tempBuf, (MAX_GETLOG_URL + MARGIN), 0x00);

    // �e���|�����f�[�^�x�[�X���폜����
    DeleteDB_wCreator(DBNAME_TEMPURL, DBVERSION_TEMPURL, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);

    // �e���|����DB�I�[�v��(�V�K�I�[�v��)
    OpenDatabase_NNsh(DBNAME_TEMPURL, DBVERSION_TEMPURL, &dbRef);
    if (dbRef == 0)
    {
        // DB�I�[�v�����s(���肦�Ȃ��͂�����...)
        MEMFREE_PTR(tempBuf);
        return (~errNone - 10);
    }
    count = 0;
    GetDBCount_NNsh(dbRef, &count);
    if (count != 0)
    {
        // �c�a�̃��R�[�h��(�S��)�폜 (�ł������Ȃ��͂�)
        for (; count != 0; count--)
        {
            DeleteRecordIDX_NNsh(dbRef, (count - 1));
        }
    }

    // �t�@�C���ǂݍ��ݗ̈���m�ۂ���
    buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (buffer) ", " size:", 
                          (NNshGlobal->NNsiParam)->bufferSize);
        CloseDatabase_NNsh(dbRef);
        MEMFREE_PTR(tempBuf);
        return (~errNone);
    }
    MemSet(buffer, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    // �g�[�N���f�[�^�̗̈���m�ۂ���
    logToken = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (logToken == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (logToken) ", "", 0);
        MEMFREE_PTR(buffer);
        CloseDatabase_NNsh(dbRef);
        MEMFREE_PTR(tempBuf);
        return (~errNone);
    }
    MemSet(logToken, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);

    // �g�[�N���f�[�^�̎擾
    if (GetLogToken(tokenId, logToken) != errNone)
    {
        // �g�[�N���f�[�^�̎擾�Ɏ��s�����ꍇ...
        MemSet(logToken, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, " startToken : %s ", logToken->startToken);
    HostTraceOutputTL(appErrorClass, " endToken : %s ",   logToken->endToken);
#endif // #ifdef USE_REPORTER

    // �t�@�C���̃I�[�v��
    ret = OpenFile_NNsh(fileName, NNSH_FILEMODE_READONLY, &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", fileName, ret);
        MEMFREE_PTR(logToken);
        MEMFREE_PTR(buffer);
        CloseDatabase_NNsh(dbRef);
        MEMFREE_PTR(tempBuf);
        return (ret);
    }

    // �t�@�C�����f�[�^�u���b�N���ƂɎ擾����
    status = NNSH_MSGTOKENSTATE_NONE;
    offset = 0;
    while ((ret == errNone)&&(status != NNSH_MSGTOKENSTATE_END))
    {
        readSize = 0;
        MemSet(buffer, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

        ret  = ReadFile_NNsh(&fileRef, offset, (NNshGlobal->NNsiParam)->bufferSize, buffer, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            // �t�@�C���ǂ݂��݃G���[�A�I������
            NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh() :", fileName, ret);
            break;
        }

        chkPtr = buffer;
        if ((status == NNSH_MSGTOKENSTATE_NONE)&&(logToken->startToken[0] != '\0'))
        {
            //  �擪�̋�؂蕶�����T��
            ptr = StrStr(buffer, logToken->startToken);
            if (ptr == NULL)
            {
                // �f�[�^�擪�̋�؂肪������Ȃ������A���o�b�t�@��ǂݍ���
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " CANNOT FIND DATA HEADER... ");
#endif // #ifdef USE_REPORTER
                // ����ǂݍ��ސ擪����肷��
                offset = offset + readSize - StrLen(logToken->startToken);
                continue;
            }
            chkPtr = ptr + StrLen(logToken->startToken);
        }
        status = NNSH_MSGTOKENSTATE_MESSAGE;

        // �ǂݍ��񂾃o�b�t�@���ɁA�����̋�؂蕶���������Ă��邩���ׂ�
        if (logToken->endToken[0] != '\0')
        {
            ptr = StrStr(buffer, logToken->endToken);
            if (ptr != NULL)
            {
                // �����f�[�^�𔭌��I �f�[�^�͂����܂łƂ���
                // (�����f�[�^�̃|�C���^�ʒu�v�Z�����܂��ł��Ȃ����߁A�������
                //  �ςȌv�Z���@�i�o�b�t�@�̐擪����΁[���ƃC���N�������g����j
                //  �ɂ���...)
                readSize = 0;
                while (ptr > buffer)
                {
                    ptr--;
                    readSize++;
                }
                status = NNSH_MSGTOKENSTATE_END;
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " DETECT END SEPARATOR ");
#endif // #ifdef USE_REPORTER
            }
        }

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " FILE readSize : %lu, buffer : %p ", readSize, buffer);
#endif // #ifdef USE_REPORTER

        // �������� Anchor��T��... (chkPtr�Ɍ����J�n�̐擪�A�ǂݍ��݃T�C�Y��readSize)
        while (chkPtr < (buffer + readSize))
        {
            MemSet (tempURL,  (sizeof(NNshGetLogTempURLDatabase)), 0x00);
            ptr = StrStr(chkPtr, "<link>");
            if (ptr != NULL)
            {
                // <link> �����o����...�����N��tempURL�ɐ؂�o��
                ptr = StrStr(ptr, "http://");
                if (ptr != NULL)
                {
                    // URL�̖�������肷��...
                    ptr2 = StrStr(ptr, "<");
                    while (ptr < ptr2)
                    {
                        ptr2--;
                        if (*ptr2 > ' ')
                        {
                            break;
                        }
                    }

                    // �؂�o����URL���o�͗p�̈�ɃR�s�[����
                    MemMove(tempURL->dataURL, ptr, ((ptr2 - ptr) + 1));

                    // �؂�o����URL���Ó����ǂ����`�F�b�N���郍�W�b�N��...
                    goto VALIDATE_URL;
                }
            }

            // �啶���A���J�[�Ə������A���J�[�̂ǂ��炩���܂܂�邩�`�F�b�N����
            ptr = StrStr (chkPtr, "<a");
            ptr2 = StrStr(chkPtr, "<A");
            if ((ptr == NULL)&&(ptr2 == NULL))
            {
                // �A���J�[��������Ȃ������A�Ƃ肠�����A������Ƃ����߂��Ă݂�B
                if (readSize > MARGIN)
                {
                    readSize = readSize - MARGIN;
                }
                // �t�@�C���̎��̃u���b�N�ֈړ�����
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " not found anchor : readSize :%d", readSize);
#endif // #ifdef USE_REPORTER
                break;
            }
            if ((ptr == NULL)||((ptr2 != NULL)&&(ptr > ptr2)))
            {
                // �啶���A���J�[�̕����������A���J�[�����O�ɂ������ꍇ�ɂ́Aptr��
                // �啶���A���J�[�̂��̂ɓ���ւ���
                ptr = ptr2;
            }
            ptr = ptr + 2;            // "<a"����ɂ����߂�
            ptr2 = StrStr(ptr, ">");  // �A���J�[�̖������擾�ł��邩�m�F
            if (ptr2 == NULL)
            {
                // �A���J�[�̖������m�F�ł��Ȃ������A�t�@�C���̎��u���b�N�ֈړ�����
                if (ptr != buffer)
                {
                    readSize = (ptr - buffer);
                }
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " NOT FOUND TAIL OF ANCHOR readSize: %d ", readSize);
#endif // #ifdef USE_REPORTER
                break;
            }

            // URL�� ptr �` ptr2 �̊Ԃ�(�m����)����B
            while ((ptr < ptr2)&&(*ptr != '\0'))
            {
                if (((*ptr == 'h')||(*ptr == 'H'))&&
                    ((*(ptr + 1) == 'r')||(*(ptr + 1) == 'R'))&&
                    ((*(ptr + 2) == 'e')||(*(ptr + 2) == 'E'))&&
                    ((*(ptr + 3) == 'f')||(*(ptr + 3) == 'F')))
                {
                    // ������URL�̐擪�����肷��...
                    ptr = ptr + 4;
                    while ((*ptr != '"')&&(*ptr != '\'')&&
                            (*ptr != '.')&&(*ptr != '/')&&
                            (ptr < ptr2)&&(*ptr != '\0'))
                    {
                        // ����(alnum)���������ꍇ�́A���[�v�𔲂���
                        if (((*ptr >= '0')&&(*ptr <= '9'))||
                            ((*ptr >= 'A')&&(*ptr <= 'Z'))||
                            ((*ptr >= 'a')&&(*ptr <= 'z')))
                        {
                            break;
                        }
                        ptr++;
                    }
                    break;
                }
                ptr++;
            }
            if ((ptr >= ptr2)||(*ptr == '\0'))
            {
                // �f�[�^�ُ�A���̃A���J�[��͂�
                chkPtr = ptr2 + 1;
                continue;
            }
            if ((*ptr == '"')||(*ptr == '\''))
            {
                // �擪�� " �܂��� ' �������΂����ɂ͂P���������߂�
                ptr++;
            }
            // tempURL�փA���J�[������؂�o��
            bufCnt = 0;
            while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != ' ')&&
                    (*ptr != '>')&&(*ptr != 0x00)&&(ptr < ptr2))
            {
                if (bufCnt < MAX_GETLOG_URL)
                {
                    // URL��URL�i�[�o�b�t�@�Ɏ��܂�Ƃ��͕������R�s�[����
                    tempURL->dataURL[bufCnt] = *ptr;
                    bufCnt++;
                }
                ptr++;
            }

#ifdef USE_REPORTER
            if (bufCnt == MAX_GETLOG_URL)
            {
                HostTraceOutputTL(appErrorClass, " ...DATA OVERFLOW... %d(%s)", bufCnt, tempURL->dataURL);
            }
#endif // #ifdef USE_REPORTER

            /////  �����Ń^�C�g���������؂�o��  /////
            /////   tempPtr  : �؂�o�����o�b�t�@
            /////   tempPtr2 : �؂�o����o�b�t�@
            // �^�C�g��������̐擪��ݒ肷��
            tempPtr = ptr2 + 1;

            ////////////// �^�C�g���𔲂��o��... ///////////
            /// (�^�C�g���̐擪��tempTitle, tempPtr2�͎��̐擪�i�[�̈�)
            bufCnt = 0;
            while (((*tempPtr != '<')||(*(tempPtr + 1) != '/'))&&
                    ((UInt8) *tempPtr >= (UInt8) ' ')&&(bufCnt < MAX_THREADNAME))
            {
                if (*tempPtr == '<')
                {
                    // �A���J�[��(�^�C�g�������ɑg�ݍ��܂�)�ǂݔ�΂�...
                    while ((*tempPtr != '\0')&&(*tempPtr != '>'))
                    {
                        tempPtr++;
                    }
                    tempPtr++;
                }
                tempURL->titleName[bufCnt] = *tempPtr;
                tempPtr++;
                bufCnt++;
            }
#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " xx tempTitle: %s", tempTitle);
#endif // #ifdef USE_REPORTER
            
            // tempURL�ɐ؂�o����URL������A���ꂪ(�擾����̂�)�Ó���URL���`�F�b�N
            // (�L���͈̔͊O�������肵�Ȃ����m�F����)
            if (ptr >= buffer + readSize)
            {
                // �L���͈̔͊O�������AURL�o�^�͂��Ȃ�
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " RANGE OVER : %s ", tempURL);
#endif // #ifdef USE_REPORTER
                // break���A���̃o�b�t�@�̈��ǂݍ���...
                break;
            }

            // ���x����URL�������ꍇ...
            ptr = StrStr(tempURL->dataURL, "#");
            if (ptr != NULL)
            {
                // #���t���Ă���A�Ó���URL�ł͂Ȃ�����
                // (���̃A���J�[��͂�)
                chkPtr = ptr2 + 1;
                continue;
            }

VALIDATE_URL:
            ptr = tempURL->dataURL;
            if (((*ptr == 'h')||(*ptr == 'H'))&&
                ((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
                ((*(ptr + 3) == 'p')||(*(ptr + 3) == 'P')))
            {
                //////////////////////////////////////////////////////////////
                // http�v���g�R��(URL)�t���A�Ó���URL�AURL���X�g�ɓo�^����
                // (�ő僊���N���ȉ��̏ꍇ)
                //////////////////////////////////////////////////////////////
                EntryRecord_NNsh(dbRef, sizeof(NNshGetLogTempURLDatabase), (void *) tempURL);
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " ENTRY URL:%s ", tempURL->dataURL);
#endif // #ifdef USE_REPORTER
                chkPtr = ptr2 + 1;
                continue;  // ���̃A���J�[��T���ɂ���
            }

            ///////////////////////////////////////////////////////////////////
            // �x�[�XURL���ʂ̂Ƃ���Ŏw�肳��Ă����ꍇ
            // �i���Έʒu�ŃA���J�[���w�肳��Ă����ꍇ...�j
            // �x�[�XURL�����炩���߃R�s�[����
            ///////////////////////////////////////////////////////////////////
            MemSet (tempBuf, (MAX_GETLOG_URL + MARGIN), 0x00);
            MemMove(tempBuf, tempURL->dataURL, MAX_GETLOG_URL);
            StrCopy(tempURL->dataURL, baseURL);
            if (tempBuf[0] == '/')
            {
                // ��΃A�h���X�p�X�w��
                // (�ŏ��� '/' �܂ł𒊏o����)
                ptr = StrStr(tempURL->dataURL, "//");
                if (ptr != NULL)
                {
                	ptr = ptr + 2;
                    ptr = StrStr(ptr, "/");
                    if (ptr == NULL)
                    {
                        // �Ō�� / ��t������
                        StrCat(tempURL->dataURL, "/");
                        ptr = tempURL->dataURL+ StrLen(tempURL->dataURL) - 1;
                    }
                }
                else
                {
					ptr = tempURL->dataURL + StrLen(tempURL->dataURL) - 1;
                }
            }
            else
            {
                // ������'/'��T��
                ptr = tempURL->dataURL + StrLen(tempURL->dataURL) - 1;
                while ((ptr > tempURL->dataURL)&&(*ptr != '/'))
                {
                    ptr--;
                }
            }
            ptr++;

            // �擪�̕�������m�F���R�s�[����
            MemSet(ptr, (StrLen(tempBuf) + 1), 0x00);
            if ((tempBuf[0] == '.')&&(tempBuf[1] == '/'))
            {
                // ���΃A�h���X�p�X�H
                StrCopy(ptr, &tempBuf[2]);
            }
            else if (tempBuf[0] == '/')
            {
                // ��΃A�h���X�p�X�H
                StrCopy(ptr, &tempBuf[1]);
            }
            else
            {
                // ����ȊO�͂��̂܂܃R�s�[
                StrCopy(ptr, tempBuf);
            }

            // URL��\������
            NNsh_DebugMessage(ALTID_INFO, "tempURL > ", tempBuf, 0);

            // URL��\������
            NNsh_DebugMessage(ALTID_INFO, "URL > ", tempURL->dataURL, 0);

            // DB�ɓo�^����
            EntryRecord_NNsh(dbRef, sizeof(NNshGetLogTempURLDatabase), (void *) tempURL);
#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " entry URL:%s ", tempURL->dataURL);
#endif // #ifdef USE_REPORTER

            chkPtr = ptr2 + 1;
        }             // while (chkPtr < buffer + readSize)
        offset = offset + readSize;   // ����ǂݍ��ސ擪����肷��
    }                 // while ((ret == errNone)&&(status != NNSH_MSGTOKENSTATE_END))
    ret = errNone;

    // ���Ƃ��܂�
    CloseFile_NNsh(&fileRef);
    CloseDatabase_NNsh(dbRef);
    MEMFREE_PTR(logToken);
    MEMFREE_PTR(buffer);
    MEMFREE_PTR(tempBuf);

    return (ret);
}

/*==========================================================================*/
/*  GetReadOnlyLogData_NNsh() : �Q�ƃ��O�̃��O�擾                          */
/*                                                                          */
/*==========================================================================*/
Err GetReadOnlyLogData_NNsh(void)
{
    Char                      *fileName, *titlePtr, dateTime[MAX_GETLOG_DATETIME];
    Boolean                    penState;
    UInt16                     dbCount, loop, x, y, number, urlCnt;
    UInt32                     recordId;
    DmOpenRef                  dbRef, tempDbRef;
    NNshGetLogDatabase        *dbData;
    NNshGetLogTempURLDatabase *tempDbURL;

    // �e���|�����f�[�^�x�[�X���I�[�v������
    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef != 0)
    {
        // �������擾���ăN���[�Y����
        GetDBCount_NNsh(dbRef, &dbCount);
        CloseDatabase_NNsh(dbRef);
        
        if (dbCount > NNSH_GETLOGDBMAX_SUBDATA)
        {
            // �����p�[�W���������{����
            Show_BusyForm(MSG_EXECUTE_PURGE);
            PurgeSubFileDatabase(NNSH_GETLOGDB_FORCE_DELETE,
                                 recordId, 0,
                                 (dbCount - NNSH_GETLOGDBMAX_SUBDATA));
            Hide_BusyForm(false);
        }

        // �g�p�f�[�^���N���A����
        dbRef   = 0;
        dbCount = 0;
    }

    // �e���|�����G���A�̊m��
    tempDbURL = MEMALLOC_PTR(sizeof(NNshGetLogTempURLDatabase) + MARGIN); 
    if (tempDbURL == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (tempDbURL) ", "", 0);
        return (~errNone);
    }
    MemSet(tempDbURL, (sizeof(NNshGetLogTempURLDatabase) + MARGIN), 0x00);

    // �t�@�C�����m�ۃo�b�t�@�̎擾
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (fileName) ", "", 0);
        MEMFREE_PTR(tempDbURL);
        return (~errNone);
    }
    MemSet(fileName, (MAXLENGTH_FILENAME + MARGIN), 0x00);

    // DB�i�[�̈���m��
    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (logDatabase) ", "", 0);
        MEMFREE_PTR(fileName);
        MEMFREE_PTR(tempDbURL);
        return (~errNone);
    }
    MemSet(dbData, (sizeof(NNshGetLogDatabase) + MARGIN), 0x00);

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        NNsh_DebugMessage(ALTID_ERROR, "DB open(permanentlog) ", "", 0);
        MEMFREE_PTR(dbData);
        MEMFREE_PTR(fileName);
        MEMFREE_PTR(tempDbURL);
        return (~errNone);
    }

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &dbCount);

    // �p�[�}�l���g�Q�ƃ��O����f�[�^��ǂݏo��
    for (loop = 0; loop < dbCount; loop++)
    {
        // ���R�[�h�̓ǂݏo��
        GetRecord_NNsh(dbRef, loop, sizeof(NNshGetLogDatabase), dbData);

        // DB�̃��x�����A�擾���x���Ɠ����������ꍇ�A�A�A
        // (���x���[���̎��͑S�Ă��擾����)
        if (((NNshGlobal->NNsiParam)->getROLogLevel == 0)||
            (dbData->usable == (NNshGlobal->NNsiParam)->getROLogLevel))
        {
            // ���O�f�[�^�̒��o
            MemSet(tempDbURL->dataURL, (MAX_GETLOG_URL), 0x00);
            StrNCopy(tempDbURL->dataURL, dbData->dataURL, (MAX_GETLOG_URL));
            MemSet(fileName, MAXLENGTH_FILENAME, 0x00);
            MemSet(dateTime, MAX_GETLOG_DATETIME, 0x00);
            (void) ReadOnlyLogDataPickup(false, dbData, tempDbURL->dataURL, fileName, loop, 0, 0, 0, dateTime);
            if (dbData->getDepth != 0)
            {
                // �P�K�w���̃��O�f�[�^���擾����ꍇ...
                if (GetRecordID_NNsh(dbRef, loop, &(recordId)) != errNone)
                {
                    recordId = 0;
                }

                // BUSY�E�B���h�E��\������
                Show_BusyForm(MSG_EXECUTE_PURGE);

                if (dbData->getSubData > NNSH_GETLOGDBMAX_SITEDATA)
                {
                    // �p�[�W���������s����...
                    PurgeSubFileDatabase(NNSH_GETLOGDB_CHECKPARENT,
                                         recordId, 0, 
                                         (dbData->getSubData - NNSH_GETLOGDBMAX_SITEDATA));
                    dbData->getSubData = NNSH_GETLOGDBMAX_SITEDATA;
                }

                // �����p�[�W���������{����
                if (dbData->getAfterHours != 255)
                {
/**
                    x = PurgeSubFileDatabase(NNSH_GETLOGDB_CHECK_PARENT_DATE,
                                             recordId, dbData->getAfterHours,
                                             (NNSH_GETLOGDBMAX_SUBDATA));
                    if (dbData->getSubData >= x)
                    {
                        dbData->getSubData = dbData->getSubData - x;
                    }
                    else
                    {
                        dbData->getSubData = 0;
                    }
**/
                }

                // BUSY�E�B���h�E��\������
                SetMsg_BusyForm(MSG_CHECK_URLLIST);

                // ���O�t�@�C������URL�ꗗ��DB�ɓW�J����
                (void) getLogFileList(fileName, dbData->dataURL, dbData->tokenId, tempDbURL);

                Hide_BusyForm(false);

                // �e���|����DB�I�[�v��
                urlCnt    = 0;
                tempDbRef = 0;
                OpenDatabase_NNsh(DBNAME_TEMPURL, DBVERSION_TEMPURL, &tempDbRef);
                if (tempDbRef == 0)
                {
                    // �c�a�t�@�C���̃I�[�v���Ɏ��s�����A����...
                    continue;
                }

                // ���R�[�h�����擾����
                GetDBCount_NNsh(tempDbRef, &urlCnt);

                for (number = urlCnt; number != 0; number--)
                {
                    // x �Ɏ擾���郌�R�[�h�ԍ���ݒ肷��
                    // (stack����ߖ�ŕϐ��𗬗p����...)
                    x = number - 1;

                    MemSet(tempDbURL, sizeof(NNshGetLogTempURLDatabase) + MARGIN, 0x00);
                    GetRecord_NNsh(tempDbRef, (x), sizeof(NNshGetLogTempURLDatabase), tempDbURL);

                    // ���O���擾����(�̈�URL�͉��̊֐��Ŕj�󂳂�邽��)
                    MemSet(fileName, MAXLENGTH_FILENAME, 0x00);
#ifdef USE_REPORTER
                    HostTraceOutputTL(appErrorClass, "SUB get URL(%d):%s ", (x), tempDbURL->dataURL);
#endif // #ifdef USE_REPORTER

                    // �f�o�b�O��URL��\������
                    NNsh_DebugMessage(ALTID_INFO, "SUB URL : ", tempDbURL->dataURL, (x));

                    // �X���^�C�g���w�b�_����ʂ̂�𗘗p���邩�H
                    if (StrStr(dbData->dataTitlePrefix, "%t") != NULL)
                    {
                        // URL�m�F�̂Ƃ��ɒ��o�����^�C�g������������
                        titlePtr = tempDbURL->titleName;
                    }
                    else
                    {
                        titlePtr = NULL;
                    }
                    if (ReadOnlyLogDataPickup(false, dbData, tempDbURL->dataURL,
                                              fileName, loop, recordId, 1, (x), 
                                              titlePtr) == errNone)
                    {
                        // �T�u�f�[�^�̃J�E���g���X�V����
                        (dbData->getSubData)++;
                    }

                    // �y���̏�Ԃ��E��(��ʃ^�b�v����Ă��邩�m�F����)
                    EvtGetPen(&x, &y, &penState);
                    if (penState == true)
                    {
                        // �y�����_�E������Ă�����A���ʑw�擾�𒆎~���邩�m�F
                        if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                              MSG_CONFIRM_ABORT_SUBLAYER, 
                                              MSG_NOTCHECK_RECORDS, 
                                              urlCnt - number) == 0)
                        {
                            // OK�������ꂽ�Abreak����
                            break;
                        }
                    }
                }

                // �f�[�^�x�[�X�����
                CloseDatabase_NNsh(tempDbRef);
           }
            // �擾�������ς���Ă����ꍇ�ɂ́A�X�V����
            if (dateTime[0] != '\0')
            {
                if (StrCompare(dbData->getDateTime, dateTime) != 0)
                {
                    MemMove(dbData->getDateTime, dateTime, MAX_GETLOG_DATETIME);
                    dbData->lastUpdate = TimGetSeconds();     // �X�V�������i�[
                    dbData->state      = NNSH_SUBJSTATUS_NEW; // �X�V�}�[�N������
#ifdef USE_REPORTER
                    HostTraceOutputTL(appErrorClass, "Update DateTime:%s(%lu)", dbData->getDateTime, dbData->lastUpdate);
#endif // #ifdef USE_REPORTER
                    UpdateRecord_NNsh(dbRef, loop, sizeof(NNshGetLogDatabase), dbData);
                }
#if 0
                else
                {
                    // �擾�������ς���Ă��Ȃ��ꍇ�ɂ́A�X�V�}�[�N������
                    dbData->state      = NNSH_SUBJSTATUS_NOT_YET;
                    UpdateRecord_NNsh(dbRef, loop, sizeof(NNshGetLogDatabase), dbData);
                }
#endif
            }
        }

        // �y���̏�Ԃ��E��(��ʃ^�b�v����Ă��邩�m�F����)
        EvtGetPen(&x, &y, &penState);
        if (penState == true)
        {
            // �y�����_�E������Ă�����A���~���邩�m�F���s��
            if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                  MSG_CONFIRM_ABORT_GETLOG, 
                                  MSG_NOTCHECK_RECORDS, dbCount - loop) == 0)
            {
                // OK�������ꂽ�Abreak����
                break;
            }
        }
    }

    // �f�[�^�x�[�X�����
    CloseDatabase_NNsh(dbRef);
 
    // �e���|�����f�[�^�x�[�X���폜����
    DeleteDB_wCreator(DBNAME_TEMPURL, DBVERSION_TEMPURL, 0, 0);

    // DB���R�[�h�̈���J������
    MEMFREE_PTR(dbData);
    MEMFREE_PTR(fileName);
    MEMFREE_PTR(tempDbURL);

    return (errNone); 
}

/*-------------------------------------------------------------------------*/
/*   Function :   Handler_LogGetURL                                        */
/*                                      �Q�ƃ��O�̎擾�ݒ�C�x���g�n���h�� */
/*                 �g�p����O���[�o���ȕϐ�                                */
/*                                      NNshGlobal->work1  : �S���R�[�h��  */
/*                                      NNshGlobal->work2  : �\�����R�[�h  */
/*                        NNshGlobal->jumpSelection : �ҏW�����R�[�h�̑��� */
/*                                                        (�V�K>1, �ҏW>0) */
/*-------------------------------------------------------------------------*/
Boolean Handler_LogGetURL(EventType *event)
{
    UInt16              keyCode;
    NNshGetLogDatabase  dbData;
    DmOpenRef           dbRef;
    FormType           *frm;

    // ���݂̃t�H�[�����擾
    switch (event->eType)
    { 
      // ���j���[�I��
      case menuEvent:
        // �ҏW�C�x���g�����s
        return (NNsh_MenuEvt_Edit(event));
        break;

      case keyDownEvent:
        frm = FrmGetActiveForm();

        // �L�[�R�[�h�ɂ���ď����𕪊򂳂���
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrTapWaveSpecificKeyRDown:
          case vchrThumbWheelDown:
          case vchrRockerDown:
          case vchrRockerRight:
          case vchrJogRight:
          case vchrTapWaveSpecificKeyRRight:
          case chrUnitSeparator:  // �J�[�\���L�[(��)�������̏���
            // �����R�[�h�Ɉړ�
            if (NNshGlobal->work2 == 0)
            {
                // �f�[�^�̖����A�X�V���Ȃ�
                return (true);
            }
            // �f�[�^���X�V
            effectLogGetURL(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->jumpSelection = 0;
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrTapWaveSpecificKeyRUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
          case vchrRockerLeft:
          case vchrJogLeft:
          case vchrTapWaveSpecificKeyRLeft:
          case chrRecordSeparator:   // �J�[�\���L�[(��)�������̏���
            // �O���R�[�h�Ɉړ�
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // �f�[�^�̐擪�A�X�V���Ȃ�
                return (true);
            }
            // �f�[�^���X�V
            effectLogGetURL(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->jumpSelection = 0;
            break;

          case chrRightArrow:     // �J�[�\���L�[�E(5way�E)
          case chrLeftArrow:         // �J�[�\���L�[��(5way��)
          case vchrRockerCenter:
          case vchrHardRockerCenter:
          case vchrThumbWheelPush:
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
          case chrEscape:
          default:
            return (false);
            break;
        }
        // �Q�ƃ��O�`���[�W�pDB�̎擾
        OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
        if (dbRef == 0)
        {
            // ���ODB�̃I�[�v�����s�A�I������
            return (false);
        }

        // �f�[�^��ǂݏo��
        MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
        GetRecord_NNsh(dbRef, NNshGlobal->work2, sizeof(NNshGetLogDatabase),
                       &dbData);

        // �ǂݏo�����f�[�^����ʂɔ��f������
        displayLogDataInfo(frm, &dbData);

        // DB���N���[�Y����
        CloseDatabase_NNsh(dbRef);
        return (true);
        break;

      case ctlSelectEvent:
        frm = FrmGetActiveForm();
        switch (event->data.ctlSelect.controlID)
        {
          case BTNID_GETLOG_PREV:
            // �O���R�[�h�Ɉړ�
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // �f�[�^�̐擪�A�X�V���Ȃ�
                return (true);
            }
            // �f�[�^���X�V
            effectLogGetURL(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->jumpSelection = 0;
            break;

          case BTNID_GETLOG_NEXT:
            // �����R�[�h�Ɉړ�
            if (NNshGlobal->work2 == 0)
            {
                // �f�[�^�̖����A�X�V���Ȃ�
                return (true);
            }

            // �f�[�^���X�V
            effectLogGetURL(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->jumpSelection = 0;
            break;

          case BTNID_GETLOG_NEW:
            // �V�K�擾��쐬(�f�[�^�𖖔��Ɉړ�����)
            // �f�[�^���X�V
            effectLogGetURL(frm);

            // ��f�[�^����ʂɔ��f������
            MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
            dbData.usable = 1;
            displayLogDataInfo(frm, &dbData);
            NNshGlobal->jumpSelection = 1;
            NNshGlobal->work2 = 0;
            return (true);
            break;

          case BTNID_GETLOG_DELETE:
            // ���R�[�h�폜
            if ((NNshGlobal->work1 != 0)&&(NNshGlobal->jumpSelection == 0))
            {
                // �f�[�^��{���ɍ폜���Ă悢���m�F����
                if (NNsh_ConfirmMessage(ALTID_CONFIRM, "",
                                        MSG_CONFIRM_DELETE_DATA, 0) == 0)
                {
                    // �Q�ƃ��O�`���[�W�pDB�̎擾
                    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
                    if (dbRef == 0)
                    {
                        // ���ODB�̃I�[�v�����s�A�I������
                        return (false);
                    }
                    // ���R�[�h���폜
                    DeleteRecordIDX_NNsh(dbRef, NNshGlobal->work2);
                    (NNshGlobal->work1)--;

                    if (NNshGlobal->work1 == NNshGlobal->work2)
                    {
                        // �������R�[�h���폜�����ꍇ...
                        (NNshGlobal->work2)--;
                    }

                    // DB���N���[�Y����
                    CloseDatabase_NNsh(dbRef);
                }
            }            
            break;

          default:
            // �������Ȃ�
            return (false);
            break;
        }
        // �Q�ƃ��O�`���[�W�pDB�̎擾
        OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
        if (dbRef == 0)
        {
            // ���ODB�̃I�[�v�����s�A�I������
            return (false);
        }

        // �f�[�^��ǂݏo��
        MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
        GetRecord_NNsh(dbRef, NNshGlobal->work2, sizeof(NNshGetLogDatabase),
                       &dbData);

        // �ǂݏo�����f�[�^����ʂɔ��f������
        displayLogDataInfo(frm, &dbData);

        // DB���N���[�Y����
        CloseDatabase_NNsh(dbRef);
        return (true);
        break;

      default: 
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectLogGetURL                                          */
/*                                              �\���f�[�^�̃��R�[�h�𔽉f */
/*                 �g�p����O���[�o���ȕϐ�                                */
/*                                      NNshGlobal->work1  : �S���R�[�h��  */
/*                                      NNshGlobal->work2  : �\�����R�[�h  */
/*                        NNshGlobal->jumpSelection : �ҏW�����R�[�h�̑��� */
/*                                                        (�V�K>1, �ҏW>0) */
/*-------------------------------------------------------------------------*/
static Boolean effectLogGetURL(FormType *frm)
{
    DmOpenRef           dbRef;
    UInt16              data;
    Char               *ptr;
    NNshGetLogDatabase  dbData;
    FieldType          *fldP;
    MemHandle           txtH;
    
    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return (false);
    }
    
    MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
    
    // ���OURL�̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_URLSET));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        MemSet(dbData.dataURL, MAX_GETLOG_URL, 0x00);
        if (*ptr != '\0')
        {
            StrNCopy(dbData.dataURL, ptr, MAX_GETLOG_URL);
            MemHandleUnlock(txtH);
        }
        else
        {
            // URL���w�肳��Ȃ������A�I������
            MemHandleUnlock(txtH);
            CloseDatabase_NNsh(dbRef);
            return (false);
        }
    }

    // ���񃌃x���̐ݒ�
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_GETLOG_USE)));
    dbData.usable = (UInt8) data;

    // RSS���[�h���ǂ���
    data = 0;
    UpdateParameter(frm, CHKID_GETLOG_RSSDATA, &(data));
    dbData.rssData = (UInt8) data;

    // ���O�擾���Ȃ����ǂ���(���O�擾���Ȃ��ꍇ�ɂ͂P�K�w�����擾���Ȃ�)
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_GETLOGMODE)));
    dbData.wwwcMode = (UInt8) data;

    // �P�K�w�����擾����
    UpdateParameter(frm, CHKID_GETLOG_DEPTH, &(data));
    if ((dbData.wwwcMode != 1)&&(data != 0))
    {
        dbData.getDepth = 1;
    }
    else
    {
        dbData.getDepth = 0;
    }

    // ���O�w�b�_�̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_HEADER));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        MemSet(dbData.dataTitlePrefix, MAX_DATAPREFIX, 0x00);
        if (*ptr != '\0')
        {
            StrNCopy(dbData.dataTitlePrefix, ptr, MAX_DATAPREFIX);
        }
        MemHandleUnlock(txtH);
    }

    // �擾�於�̂̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_TITLE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        MemSet(dbData.dataTitle, MAX_TITLE_LABEL, 0x00);
        if (*ptr != '\0')
        {
            StrNCopy(dbData.dataTitle, ptr, MAX_TITLE_LABEL);
        }
        MemHandleUnlock(txtH);
    }

    // n���Ԍ�Ď擾����
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_REGET_HOUR));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            data = (UInt16) StrAToI(ptr);
            if (data > MAX_GETLOG_REGETHOUR)
            {
                data = MAX_GETLOG_REGETHOUR;
            }
            dbData.getAfterHours = data;
        }
        MemHandleUnlock(txtH);
    }

    // ��؂蕶�����ID�̎擾
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_NORMTOKEN)));
    if (NNshGlobal->work4 != NULL)
    {
        dbData.tokenId = NNshGlobal->work4[data];
    }

    // �T�u��؂蕶�����ID�擾
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_SUBTOKEN)));
    if (NNshGlobal->work4 != NULL)
    {
        dbData.subTokenId = NNshGlobal->work4[data];
    }

    // �����R�[�h�̎擾
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_KANJICODE)));
    dbData.kanjiCode = (UInt8) data;

    if (NNshGlobal->jumpSelection == 0)
    {
        // ���R�[�h���X�V����
        UpdateRecord_NNsh(dbRef, NNshGlobal->work2,
                          sizeof(NNshGetLogDatabase), &dbData);
    }
    else
    {
        // ���R�[�h��o�^����
        EntryRecord_NNsh(dbRef, sizeof(NNshGetLogDatabase), &dbData);
        (NNshGlobal->work1)++;
    }

    // �f�[�^�x�[�X�����
    CloseDatabase_NNsh(dbRef);

    // �Q�ƃ��O�f�[�^�x�[�X���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_LOGCHARGE));

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   displayLogDataInfo                                       */
/*                                                      �Q�ƃ��O�擾�̐ݒ� */
/*                 �g�p����O���[�o���ȕϐ�                                */
/*                                      NNshGlobal->work1  : �S���R�[�h��  */
/*                                      NNshGlobal->work2  : �\�����R�[�h  */
/*                        NNshGlobal->jumpSelection : �ҏW�����R�[�h�̑��� */
/*                                                        (�V�K>1, �ҏW>0) */
/*-------------------------------------------------------------------------*/
static Boolean displayLogDataInfo(FormType *frm, NNshGetLogDatabase *dbData)
{
    UInt16 data;
    Char   numBuf[TINYBUF];

    // �`�F�b�N�{�b�N�X�̔��f(�P�K�w�����擾)
    data = dbData->getDepth;

    SetControlValue(frm, CHKID_GETLOG_DEPTH, &data);

    // RSS�f�[�^���ǂ���
    data = dbData->rssData;
    SetControlValue(frm, CHKID_GETLOG_RSSDATA, &data);

    // ���O�擾���Ȃ����ǂ���
    data = dbData->wwwcMode;
    NNshWinSetPopItems(frm, POPTRID_GETLOGMODE, LSTID_GETLOGMODE, data);

    // �^�C�g���v���t�B�b�N�X
    NNshWinSetFieldText(frm, FLDID_GETLOG_HEADER, false,
                        dbData->dataTitlePrefix, MAX_DATAPREFIX);

    // �擾URL
    NNshWinSetFieldText(frm, FLDID_GETLOG_URLSET, false, 
                        dbData->dataURL, MAX_GETLOG_URL);

    // n���Ԍ�Ď擾
    MemSet (numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, dbData->getAfterHours);
    NNshWinSetFieldText(frm,FLDID_REGET_HOUR, false, numBuf, TINYBUF);

    // �擾��̖���
    NNshWinSetFieldText(frm, FLDID_GETLOG_TITLE, true, 
                        dbData->dataTitle, MAX_TITLE_LABEL);

    // ���񃌃x���ݒ�
    data = dbData->usable;
    NNshWinSetPopItems(frm, POPTRID_GETLOG_USE, LSTID_GETLOG_USE, data);

    // �����R�[�h�ݒ�
    data = dbData->kanjiCode;
    NNshWinSetPopItems(frm, POPTRID_KANJICODE, LSTID_KANJICODE, data);

    // �|�b�v�A�b�v�g���K�̐ݒ�

    // (��؂�p�^�[��1)
    data = ConvertTokenListNum(dbData->tokenId);
    NNshWinSetPopItems(frm, POPTRID_NORMTOKEN, LSTID_NORMTOKEN, data);

    // (��؂�p�^�[��2)
    data = ConvertTokenListNum(dbData->subTokenId);
    NNshWinSetPopItems(frm, POPTRID_SUBTOKEN,  LSTID_SUBTOKEN, data);

   return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetLogGetURL                                             */
/*                                                      �Q�ƃ��O�擾�̐ݒ� */
/*                 �g�p����O���[�o���ȕϐ�                                */
/*                                      NNshGlobal->work1  : �S���R�[�h��  */
/*                                      NNshGlobal->work2  : �\�����R�[�h  */
/*                        NNshGlobal->jumpSelection : �ҏW�����R�[�h�̑��� */
/*                                                        (�V�K>1, �ҏW>0) */
/*-------------------------------------------------------------------------*/
Boolean SetLogGetURL(UInt16 recNum)
{
    Boolean              ret = false;
    FormType            *frm, *prevFrm;
    DmOpenRef            dbRef;
    NNshGetLogDatabase  *dbData;
    UInt16               labelCnt;
    UInt32              *idList;
    NNshWordList         separatorChoice;

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return (false);
    }

    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        return (false);
    }
    MemSet(dbData, (sizeof(NNshGetLogDatabase) + MARGIN), 0x00);

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &(NNshGlobal->work1));

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " SetLogGetURL(%d) : recNum:%d", 
                          recNum, NNshGlobal->work1);
#endif // #ifdef USE_REPORTER

    // ���x�������擾����
    labelCnt = GetLogTokenLabelList(&separatorChoice, &idList);
    NNshGlobal->work4 = idList;

    // �f�[�^�̏�����
    //  work1 �F ���R�[�h����
    //  work2 �F �\�������R�[�h
    //  jumpSelection �F �V�K���R�[�h�t���O(�P�Ȃ�V�K)
    //  work4 �F ��؂�f�[�^���X�g��ID���X�g�|�C���^
    if ((recNum != NNSH_ITEM_LASTITEM)&&(recNum < NNshGlobal->work1))
    {
        // ���郌�R�[�h���|�C���g���ĊJ���ꍇ
        NNshGlobal->work2 = recNum;
        NNshGlobal->jumpSelection = 0;
    }
    else
    {
        // �V�K���R�[�h�ǉ��̏ꍇ...
        NNshGlobal->work2 = 0;
        NNshGlobal->jumpSelection = 1;        
    }

    // �_�C�A���O�\������
    prevFrm = FrmGetActiveForm();
    PrepareDialog_NNsh(FRMID_GETLOG_INPUT, &frm, true);
    if (frm == NULL)
    {
        // �f�[�^�x�[�X�����
        CloseDatabase_NNsh(dbRef);
        ReleaseWordList(&separatorChoice);
        MEMFREE_PTR(idList);
        MEMFREE_PTR(dbData);
        return (false);
    }

    // �|�b�v�A�b�v�g���K�̐ݒ�
    if (labelCnt != 0)
    {
        NNshWinSetPopItemsWithList(frm, POPTRID_NORMTOKEN, LSTID_NORMTOKEN, &separatorChoice, 0);
        NNshWinSetPopItemsWithList(frm, POPTRID_SUBTOKEN,  LSTID_SUBTOKEN,  &separatorChoice, 0);
    }

    // ���R�[�h���P�����Ȃ������ꍇ�A�������͐V�K���R�[�h�ǉ��̏ꍇ
    if ((NNshGlobal->work1 == 0)||(NNshGlobal->jumpSelection == 1))
    {
        // ��̃��R�[�h�f�[�^����ʂɕ\������
        dbData->usable = (NNshGlobal->NNsiParam)->getROLogLevel;
    }
    else
    {
        // �Ō�̃��R�[�h�f�[�^����ʂɕ\������ꍇ...
        if (NNshGlobal->work2 == NNSH_ITEM_BOTTOMITEM)
        {
            NNshGlobal->work2 = NNshGlobal->work1 - 1;
        }
        GetRecord_NNsh(dbRef, NNshGlobal->work2,
                       sizeof(NNshGetLogDatabase), dbData);
        NNshGlobal->jumpSelection = 0;
    }

    displayLogDataInfo(frm, dbData);

    // �f�[�^�x�[�X�����
    CloseDatabase_NNsh(dbRef);

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(frm, Handler_LogGetURL);

    // �Q�ƃ��O�擾�̐ݒ�_�C�A���O��\������
    if (FrmDoDialog(frm) == BTNID_GETLOG_OK)
    {
        // �\���f�[�^�̃��R�[�h�𔽉f������
        ret = effectLogGetURL(frm);
    }

    // �_�C�A���O�\���̌�Еt��
    PrologueDialog_NNsh(prevFrm, frm, true);

    // ���x���̈���J������
    ReleaseWordList(&separatorChoice);
    MEMFREE_PTR(idList);
    MEMFREE_PTR(dbData);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :  effectLogToken                                            */
/*                                    ���O�̋�؂�ݒ���f�[�^�x�[�X�ɔ��f */
/*-------------------------------------------------------------------------*/
static Boolean effectLogToken(FormType *frm)
{
    NNshLogTokenDatabase *dbData;
    DmOpenRef             dbRef;
    Char                 *ptr;
    FieldType            *fldP;
    MemHandle             txtH;
    UInt16                cnt;
    
    dbData = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (dbData == NULL)
    {
        // �̈�m�ێ��s�A�I������
        return (false);
    }

    // �f�[�^�̏�����
    MemSet(dbData, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        MEMFREE_PTR(dbData);
        return (false);
    }

    // �f�[�^���V�K�o�^���m�F����
    if ((NNshGlobal->work3 == 0)&&(cnt != 0))
    {
        // ���R�[�h�h�c���擾�A���R�[�hID�Ɋi�[����
        GetRecordID_NNsh(dbRef, (NNshGlobal->work2), &(dbData->id));
    }

    // ���O�w�b�_�̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_HEADER));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            StrNCopy(dbData->tokenPatternName, ptr, MAX_DATAPREFIX);
        }
        MemHandleUnlock(txtH);
    }

    // �J�n������̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_START));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            StrNCopy(dbData->startToken, ptr, MAX_DATATOKEN);
        }
        MemHandleUnlock(txtH);
    }

    // �I��������̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_END));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            StrNCopy(dbData->endToken, ptr, MAX_DATATOKEN);
        }
        MemHandleUnlock(txtH);
    }
    
    // �g�[�N��������̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_INDEX));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            StrNCopy(dbData->dataToken, ptr, MAX_DATATOKEN);
        }
        MemHandleUnlock(txtH);
    }

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &(cnt));

    // �f�[�^���V�K�o�^���m�F����
    if ((NNshGlobal->work3 != 0)||(cnt == 0))
    {
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " CREATE NEW RECORD : %d ", cnt);
#endif // #ifdef USE_REPORTER

        // ���R�[�h��o�^����
        EntryRecord_NNsh(dbRef, sizeof(NNshLogTokenDatabase), dbData);

        // ���R�[�h�h�c���擾�A���R�[�hID�Ɋi�[����
        GetRecordID_NNsh(dbRef, 0, &(dbData->id));

        NNshGlobal->work2 = 0;
        (NNshGlobal->work1)++;
    }

    // ���R�[�h���X�V����
    UpdateRecord_NNsh(dbRef, NNshGlobal->work2,
                      sizeof(NNshLogTokenDatabase), dbData);

    // DB���N���[�Y����
    CloseDatabase_NNsh(dbRef);

    // �Q�ƃ��O�g�[�N���f�[�^�x�[�X���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_LOGTOKEN));

    MEMFREE_PTR(dbData);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :  displayLogToken                                           */
/*                                                  ���O�̋�؂�ݒ�𔽉f */
/*-------------------------------------------------------------------------*/
static Boolean displayLogToken(FormType *frm, UInt16 recNum)
{
    NNshLogTokenDatabase *dbData;
    DmOpenRef            dbRef;


    dbData = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (dbData == NULL)
    {
        // �̈�m�ێ��s�A�I������
        return (false);
    }

    // �f�[�^�̏�����
    MemSet(dbData, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);
    
    // recNum �� 0xffff �̎��́A�󕶎����\������(DB����f�[�^�͎擾���Ȃ�)
    if (recNum != NNSH_DATABASE_BLANK)
    {
        // �Q�ƃ��O�`���[�W�pDB�̎擾
        OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
        if (dbRef == 0)
        {
            // ���ODB�̃I�[�v�����s�A�I������
            MEMFREE_PTR(dbData);
            return (false);
        }

        // �f�[�^��ǂݏo��
        GetRecord_NNsh(dbRef, recNum, sizeof(NNshLogTokenDatabase), dbData);

        // DB���N���[�Y����
        CloseDatabase_NNsh(dbRef);
    }

    // �擾�����f�[�^����ʂɔ��f������

    // ��؂�p�^�[����
    NNshWinSetFieldText(frm, FLDID_GETLOG_HEADER, false,
                        dbData->tokenPatternName, MAX_DATAPREFIX);

    // �J�n������
    NNshWinSetFieldText(frm, FLDID_GETLOG_START, false, 
                        dbData->startToken, MAX_DATATOKEN);

    // �I��������
    NNshWinSetFieldText(frm, FLDID_GETLOG_END, false, 
                        dbData->endToken, MAX_DATATOKEN);

    // ��؂蕶����
    NNshWinSetFieldText(frm, FLDID_GETLOG_INDEX, true, 
                        dbData->dataToken, MAX_DATATOKEN);

    MEMFREE_PTR(dbData);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Handler_setLogToken                                      */
/*                              �Q�ƃ��O�̋�؂�ݒ��ʗp�C�x���g�n���h�� */
/*-------------------------------------------------------------------------*/
Boolean Handler_setLogToken(EventType *event)
{
    UInt16              keyCode;
    DmOpenRef           dbRef;
    FormType           *frm;

    // ���݂̃t�H�[�����擾
    switch (event->eType)
    { 
      // ���j���[�I��
      case menuEvent:
        // �ҏW�C�x���g�����s
        return (NNsh_MenuEvt_Edit(event));
        break;

      case keyDownEvent:
        frm = FrmGetActiveForm();

        // �L�[�R�[�h�ɂ���ď����𕪊򂳂���
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrTapWaveSpecificKeyRUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
          case vchrRockerLeft:
          case vchrJogLeft:
          case vchrTapWaveSpecificKeyRLeft:
          case chrRecordSeparator:   // �J�[�\���L�[(��)�������̏���
            // �����R�[�h�Ɉړ�
            if (NNshGlobal->work2 == 0)
            {
                // �f�[�^�̖����A�X�V���Ȃ�
                return (true);
            }
            // �f�[�^���X�V
            effectLogToken(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->work3 = 0;
            break;

          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrTapWaveSpecificKeyRDown:
          case vchrThumbWheelDown:
          case vchrRockerDown:
          case vchrRockerRight:
          case vchrJogRight:
          case vchrTapWaveSpecificKeyRRight:
          case chrUnitSeparator:  // �J�[�\���L�[(��)�������̏���
            // �O���R�[�h�Ɉړ�
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // �f�[�^�̐擪�A�X�V���Ȃ�
                return (true);
            }
            // �f�[�^���X�V
            effectLogToken(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->work3 = 0;
            break;

          case chrLeftArrow:         // �J�[�\���L�[��(5way��)
          case chrRightArrow:     // �J�[�\���L�[�E(5way�E)
          case vchrRockerCenter:
          case vchrHardRockerCenter:
          case vchrThumbWheelPush:
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
          case chrEscape:
          default:
            return (false);
            break;
        }

        // �ǂݏo�����f�[�^����ʂɔ��f������
        displayLogToken(frm, NNshGlobal->work2);
        return (true);
        break;

      case ctlSelectEvent:
        frm = FrmGetActiveForm();
        switch (event->data.ctlSelect.controlID)
        {
          case BTNID_GETLOG_NEXT:
            // �O���R�[�h�Ɉړ�(�����ǎ��{�^�����������Ƃ�...)
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // �f�[�^�̐擪�A�X�V���Ȃ�
                return (true);
            }
            // �f�[�^���X�V
            effectLogToken(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->work3 = 0;
            break;

          case BTNID_GETLOG_PREV:
            // �����R�[�h�Ɉړ�(�����ǑO�{�^�����������Ƃ�...)
            if (NNshGlobal->work2 == 0)
            {
                // �f�[�^�̖����A�X�V���Ȃ�
                return (true);
            }

            // �f�[�^���X�V
            effectLogToken(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->work3 = 0;
            break;

          case BTNID_GETLOG_NEW:
            // �V�K�擾��쐬(�f�[�^�𖖔��Ɉړ�����)
            // �f�[�^���X�V
            effectLogToken(frm);

            // ��f�[�^����ʂɔ��f������
            NNshGlobal->work3 = 1;
            NNshGlobal->work2 = 0;
            displayLogToken(frm, NNSH_DATABASE_BLANK);
            return (true);
            break;

          case BTNID_GETLOG_DELETE:
            // ���R�[�h�폜
            if ((NNshGlobal->work1 != 0)&&(NNshGlobal->work3 == 0))
            {
                // �f�[�^��{���ɍ폜���Ă悢���m�F����
                if (NNsh_ConfirmMessage(ALTID_CONFIRM, "",
                                        MSG_CONFIRM_DELETE_DATA, 0) == 0)
                {
                    // �Q�ƃ��O�`���[�W�pDB�̎擾
                    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
                    if (dbRef == 0)
                    {
                        // ���ODB�̃I�[�v�����s�A�I������
                        return (false);
                    }
                    // ���R�[�h���폜
                    DeleteRecordIDX_NNsh(dbRef, NNshGlobal->work2);
                    (NNshGlobal->work1)--;

                    if (NNshGlobal->work1 == NNshGlobal->work2)
                    {
                        // �������R�[�h���폜�����ꍇ...
                        (NNshGlobal->work2)--;
                    }

                    // DB���N���[�Y����
                    CloseDatabase_NNsh(dbRef);
                }
            }            
            break;

          default:
            // �������Ȃ�
            return (false);
            break;
        }

        // �ǂݏo�����f�[�^����ʂɔ��f������
        displayLogToken(frm, NNshGlobal->work2);

        return (true);
        break;

      default: 
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   SetLogCharge_LogToken                                    */
/*                                                    ��؂�p�^�[���̓o�^ */
/*=========================================================================*/
Boolean SetLogCharge_LogToken(UInt16 recNum)
{
    Boolean       ret = false;
    FormType     *prevFrm, *diagFrm;
    UInt16        btnId;
    DmOpenRef     dbRef;

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // �Q�ƃ��O��؂�DB�̎擾
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "LOGTOKEN DB Open Failure.");
#endif // #ifdef USE_REPORTER
        return (false);
    }

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &(NNshGlobal->work1));

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "TOKEN DB count : %d", NNshGlobal->work1);
#endif // #ifdef USE_REPORTER

    // �f�[�^�x�[�X�����
    CloseDatabase_NNsh(dbRef);

    // �\���n�����f�[�^�̔��f�i���[�N�̈�̎g�p���@�j
    //   work1�F���݂̃��R�[�h����
    //   work2�F�\�����̃��R�[�h�ԍ�
    //   work3�F�V�K���R�[�h�ǉ��i�P�Ȃ�ǉ��j
    NNshGlobal->work2 = recNum;
    NNshGlobal->work3 = (NNshGlobal->work1 == 0) ? 1 : 0;

    // �_�C�A���O�\������
    PrepareDialog_NNsh(FRMID_GETLOG_TOKENSET, &diagFrm, true);
    if (diagFrm == NULL)
    {
        return (false);
    }

    // �����f�[�^�̔��f
    displayLogToken(diagFrm, NNshGlobal->work2);

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_setLogToken);

    // �E�B���h�E�̕\��
    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_GETLOG_OK)
    {
         // �\�����f�[�^�����R�[�h�ɔ��f������
         effectLogToken(diagFrm);
         ret = true;
    }

    // �_�C�A���O�\���̌�Еt��
    PrologueDialog_NNsh(prevFrm, diagFrm, true);

    // �f�[�^�x�[�X�̃\�[�g���s���Ă���
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_UINT32, 0);
    CloseDatabase_NNsh(dbRef);

    return (ret);
}

/*=========================================================================*/
/*   Function :   GetLogToken                                              */
/*                                                    ��؂�p�^�[���̎擾 */
/*=========================================================================*/
Err GetLogToken(UInt32 key, NNshLogTokenDatabase *record)
{
   Err       ret;
   DmOpenRef dbRef;
   UInt16    index;

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return (~errNone);
    }

    // id��key�ɂ��Č�������
    ret = SearchRecord_NNsh(dbRef, &key, NNSH_KEYTYPE_UINT32, 0,
                            sizeof(NNshLogTokenDatabase), record, &index);

    // DB���N���[�Y����
    CloseDatabase_NNsh(dbRef);

    return (ret);
}

/*=========================================================================*/
/*   Function :  ConvertTokenListNum                                       */
/*                                      ��؂�p�^�[��ID�����X�g�ԍ��ɕϊ� */
/*=========================================================================*/
UInt16 ConvertTokenListNum(UInt32 key)
{
   Err       ret;
   DmOpenRef dbRef;
   UInt16    index;
   NNshLogTokenDatabase record;

    // key���w��̏ꍇ�ɂ́A0����������
    if (key == 0)
    {
        return (0);
    }

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return (0);
    }

    // id��key�ɂ��Č�������
    index = 0;
    ret = SearchRecord_NNsh(dbRef, &key, NNSH_KEYTYPE_UINT32, 0,
                            sizeof(NNshLogTokenDatabase), &record, &index);
    if (ret != errNone)
    {
        index = 0;
    }

    // DB���N���[�Y����
    CloseDatabase_NNsh(dbRef);

    return (index);
}

/*=========================================================================*/
/*   Function :   GetLogTokenLabelList                                     */
/*                                                ��؂�p�^�[���̈ꗗ�擾 */
/*=========================================================================*/
UInt16 GetLogTokenLabelList(NNshWordList *wordList, UInt32 **idList)
{
    Err                   ret;
    Char                 *nameP, *strBuf;
    UInt32               *idP;
    DmOpenRef             dbRef;
    UInt16                index, count, recCount;
    MemHandle             dataH;
    NNshLogTokenDatabase *data;

    // �ϐ��̏�����
    MemSet(wordList, sizeof(NNshWordList), 0x00);
    *idList = NULL;

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return (0);
    }

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &count);
    if (count == 0)
    {
        return (0);
    }

    // ID�i�[�̈���m��
    *idList = MEMALLOC_PTR(sizeof(UInt32) * count + MARGIN);
    if (*idList == NULL)
    {
        return (0);
    }
    MemSet(*idList, (sizeof(UInt32) * count + MARGIN), 0x00);
    idP = *idList;

    // ���x���i�[�̈���m��
    strBuf = MEMALLOC_PTR((sizeof(Char) * (MAX_STRLEN)) * count + MARGIN);
    if (strBuf == NULL)
    {
        MEMFREE_PTR(*idList);
        return (0);
    }
    MemSet(strBuf, ((sizeof(Char) * (MAX_STRLEN)) * count + MARGIN), 0x00);
    nameP = strBuf;

    recCount = 0;
    for (index = 0; index < count; index++)
    {
        // ���R�[�h�f�[�^�̎擾
        ret = GetRecordReadOnly_NNsh(dbRef, index, &dataH, (void **) &data);
        if (ret == errNone)
        {
            if (recCount != 0)
            {
                StrCat(nameP, ",");
            }

            // �ǂݏo�����f�[�^���m�ۂ����̈�ɔ��f������
            *idP = data->id;
            idP++;
            if (data->tokenPatternName[0] != '\0')
            {
                // ��������擾
                StrCat(nameP, data->tokenPatternName);
            }
            else
            {
                StrCat(nameP, "???");
            }

            // ���R�[�h�f�[�^�̕\��
            nameP = nameP + StrLen(nameP);

            recCount++;
        }
        // ���R�[�h��������A���ւ�����
        ReleaseRecordReadOnly_NNsh(dbRef, dataH);
    }

    // ���x���ꗗ�����X�g�ɕ\������\���֕ϊ�...
    SeparateWordList(strBuf, wordList);
    MEMFREE_PTR(strBuf);

    // DB���N���[�Y����
    CloseDatabase_NNsh(dbRef);

    return (recCount);
}

/*=========================================================================*/
/*   Function :   SelectLogToken                                           */
/*                                                    ��؂�p�^�[���̑I�� */
/*=========================================================================*/
UInt16 SelectLogToken(UInt32 *selection)
{
    Char         *ptr;
    FormType     *prevFrm, *diagFrm;
    UInt32       *idList;
    UInt16        btnId, nofItems;
    MemHandle     listH;
    ListType     *lstP;
    NNshWordList  wordList;
    
    // ������
    listH     = 0;
    *selection = 0;
    btnId     = BTNID_JUMPCANCEL;
    prevFrm   = FrmGetActiveForm();
    MemSet(&wordList, sizeof(NNshWordList), 0x00);

    // ���O�g�[�N�����Ƃ肾��
    nofItems = GetLogTokenLabelList(&wordList, &idList);

    // �A�C�e�����̊m�F(�[����������A���������߂�)
    if (nofItems == 0)
    {
        // ��؂�p�^�[���Ȃ�
        return (BTNID_JUMPCANCEL);
    }

    // �_�C�A���O�\������
    PrepareDialog_NNsh(FRMID_DIRSELECTION, &diagFrm, false);
    if (diagFrm == NULL)
    {
        return (BTNID_JUMPCANCEL);
    }

    // ���X�g�̐ݒ�
    lstP  = FrmGetObjectPtr(diagFrm,
                                FrmGetObjectIndex(diagFrm, LSTID_JUMPLIST));

    ptr = MemHandleLock(wordList.wordmemH);
    LstSetListChoices(lstP, (Char **) ptr, wordList.nofWord);
    LstSetSelection  (lstP, 0);
    LstSetTopItem    (lstP, 0);

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_JumpSelection);

    // �W�����v����N���A
    NNshGlobal->jumpSelection = 0;

    // �_�C�A���O�̃I�[�v���A������̓��͂�҂�
    btnId = FrmDoDialog(diagFrm);

    // �_�C�A���O�\���̌�Еt��
    PrologueDialog_NNsh(prevFrm, diagFrm, false);

    // �n���h���A�����b�N
    MemHandleUnlock(wordList.wordmemH);

    if (btnId == BTNID_JUMPEXEC)
    {
        // �I�����ꂽ
        *selection = idList[NNshGlobal->jumpSelection];        
    }

    // ���Ƃ��܂�
    MEMFREE_PTR(idList);
    ReleaseWordList(&wordList);

    return (btnId);
}


/*=========================================================================*/
/*   Function :   GetImageFile                                             */
/*                                             �C���[�W�t�@�C�����擾����  */
/*=========================================================================*/
Boolean GetImageFile(Char *url)
{
    Err                  ret;
    Char                *fileName, *ptr;
    NNshGetLogDatabase  *dbData;

    // �t�@�C���擾�̊m�F...
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, url, MSG_COMFIRM_GETFILE, 0) != 0)
    {
        // �f�[�^�̎擾�m�F���L�����Z�����ꂽ�ꍇ
        return (false);
    }

    // �t�@�C�����i�[�̈���m��
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        return (false);
    }
    MemSet(fileName, MAXLENGTH_FILENAME + MARGIN, 0x00);

    // DB�̃_�~�[�̈���m��
    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        MEMFREE_PTR(fileName);
        return (false);
    }
    MemSet(dbData, sizeof(NNshGetLogDatabase) + MARGIN, 0x00);

    // �f�[�^��ۑ�����X���t�@�C�����̓���
    ptr = url + StrLen(url) - 1;
    while ((*ptr != '/')&&(*ptr != '?')&&(*ptr != '*'))
    {
        ptr--;
    }

    // �f�[�^�̍Ō�ɃX���b�V�����������ꍇ�ɂ̓t�@�C�������R�s�[���Ȃ�
    if (ptr < url + StrLen(url) - 1)
    {
        StrNCopy(fileName, (ptr + 1), (MAXLENGTH_FILENAME - 1));
    }

    // �t�q�k�f�[�^�̎擾������...
    ret = ReadOnlyLogDataPickup(true, dbData, url, fileName, NNSH_MSGNUM_UNKNOWN, 0, 0, 0, NULL);

    // �m�ۗ̈���J��
    MEMFREE_PTR(dbData);
    MEMFREE_PTR(fileName);
    if (ret != errNone)
    {
        return (false);
    }
    return (true);
}

/*=========================================================================*/
/*   Function :   EntryLogSubFileDatabase                                  */
/*                           �u�P�����擾�v�̎擾���Ƀf�[�^��o�^����  */
/*=========================================================================*/
Err EntryLogSubFileDatabase(NNshGetLogSubFileDatabase *subDb)
{
    Err       ret;
    DmOpenRef dbRef;

    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", DBNAME_TEMPORARYLOG, 0);
        return (~errNone);
    }

    // �X���f�[�^���X�V
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshGetLogSubFileDatabase), subDb);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "EntryRecord_NNsh() ", DBNAME_TEMPORARYLOG, ret);
    }
    else
    {
        // �T�u���O�f�[�^�x�[�X���X�V�������Ƃɂ���
        NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_LOGSUBDB));
    }
    CloseDatabase_NNsh(dbRef);

    return (ret);
}

/*=========================================================================*/
/*   Function :   GetSubFileDatabase                                       */
/*                   �u�P�����擾�v�̎擾��񂪓o�^�ς݂��ǂ����m�F����  */
/*                             (����΁AsubDb�Ƀf�[�^���R�s�[���ĉ�������) */
/*                         �� ���s���ɂ́ANNSH_DATABASE_BLANK����������    */
/*=========================================================================*/
UInt16 GetSubFileDatabase(UInt16 command, UInt32 recId, 
                             Char *dataURL, NNshGetLogSubFileDatabase *subDb)
{
    Err                        ret;
    UInt16                     dbCount, loop, len, matched;
    MemHandle                  dataH;
    DmOpenRef                  dbRef;
    NNshGetLogSubFileDatabase *subDbRec;

    len = StrLen(dataURL);
    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", DBNAME_TEMPORARYLOG, 0);
        return (NNSH_DATABASE_BLANK);
    }
    // ���R�[�h�������擾
    GetDBCount_NNsh(dbRef, &dbCount);
    for (loop = dbCount; loop != 0; loop--)
    {
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &dataH, (void **)&subDbRec);
        if (ret == errNone)
        {
            matched = NNSH_DATABASE_BLANK;
            switch (command)
            {
              case NNSH_GETLOGDB_CHECKPARENT:
                // �e���R�[�h���w�肳�ꂽ����...
                if (recId == subDbRec->parentDbId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECKTOKEN:
                // ��؂�̃��R�[�h���w�肳�ꂽ����...
                if (recId == subDbRec->tokenId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECKURL:
                // URL���w�肳�ꂽ����...
                matched = StrNCompare(subDbRec->dataURL, dataURL, len);
                break;
              
              default:
                // ��L�ȊO...
                break;
            }
            if (matched == 0)
            {
                // ���R�[�h���������B�B
                if (subDb != NULL)
                {
                    // ���R�[�h�̃f�[�^���R�s�[����
                    MemMove(subDb, subDbRec, sizeof(NNshGetLogSubFileDatabase));
                }
                // ���R�[�h�̃��b�N���J������
                ReleaseRecordReadOnly_NNsh(dbRef, dataH);

                // DB���N���[�Y����
                CloseDatabase_NNsh(dbRef);
                return (loop - 1);
            }
            // ���R�[�h�̃��b�N���J������
            ReleaseRecordReadOnly_NNsh(dbRef, dataH);
        }
    }
    // DB���N���[�Y���ĉ�������
    CloseDatabase_NNsh(dbRef);
    return (NNSH_DATABASE_BLANK);
}

/*=========================================================================*/
/*   Function :   UpdateSubFileDatabase                                    */
/*                                   �u�P�����擾�v�̎擾�����X�V����  */
/*=========================================================================*/
Err UpdateSubFileDatabase(UInt16 index, NNshGetLogSubFileDatabase *subDb)
{
    Err       ret;
    DmOpenRef dbRef;

    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", DBNAME_TEMPORARYLOG, 0);
        return (~errNone);
    }

    // �X���f�[�^���X�V
    ret = UpdateRecord_NNsh(dbRef,index,sizeof(NNshGetLogSubFileDatabase), subDb);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", DBNAME_TEMPORARYLOG, ret);
    }
    CloseDatabase_NNsh(dbRef);

    // �T�u���O�f�[�^�x�[�X���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_LOGSUBDB));

    return (ret);
}

/*=========================================================================*/
/*   Function :   PurgeSubFileDatabase                                     */
/*                                 �u�P�����擾�v�̎擾�����p�[�W����  */
/*=========================================================================*/
UInt16 PurgeSubFileDatabase(UInt16 command, UInt32 recId, UInt8 argument, UInt16 purgeCount)
{
    Err                        ret;
    UInt32                     diffTime;
    UInt16                     dbCount, loop, matched, count;
    MemHandle                  dataH;
    DmOpenRef                  dbRef;
    NNshGetLogSubFileDatabase *subDbRec;

    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", DBNAME_TEMPORARYLOG, 0);
        return (0);
    }

    count = 0;
    // ���R�[�h�������擾
    GetDBCount_NNsh(dbRef, &dbCount);
    for (loop = dbCount; ((loop != 0)&&(count < purgeCount)); loop--)
    {
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &dataH, (void **)&subDbRec);
        if (ret == errNone)
        {
            matched = NNSH_DATABASE_BLANK;
            switch (command)
            {
              case NNSH_GETLOGDB_CHECKPARENT:
                // �e���R�[�h�ԍ�����v����...
                if (recId == subDbRec->parentDbId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECKTOKEN:
                // �g�[�N���ԍ�����v����...
                if (recId == subDbRec->tokenId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECKDATE:
                // n�b���Â��T�u���O�͏�������...
                diffTime = TimGetSeconds() - subDbRec->lastUpdate;
                if (diffTime > recId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECK_PARENT_DATE:
                // �e���R�[�h�ԍ�����v���An�b���Â��T�u���O�͏�������...
                diffTime = TimGetSeconds() - subDbRec->lastUpdate;
                if ((diffTime > (((UInt32) argument) * 60 * 60))&&(recId == subDbRec->parentDbId))
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_FORCE_DELETE:
                // �K������...
                matched = 0;
                break;

              case NNSH_GETLOGDB_CHECKURL:
              default:
                // �p�[�W���Ȃ�...
                break;
            }
            // ���R�[�h�̃��b�N���J������
            ReleaseRecordReadOnly_NNsh(dbRef, dataH);

            // purge�ɊY�����郌�R�[�h�������ꍇ...
            if (matched == 0)
            {
                // DB���폜����...
                DeleteRecordIDX_NNsh(dbRef, (loop - 1));
                
                // �����O�t�@�C������������...
                if (subDbRec->logDbId != 0)
                {
                    DeleteThreadMessage_RecordID(subDbRec->logDbId);
                }
                count++;
            }
        }
    }
    // DB���N���[�Y���ĉ������� (purge����)
    CloseDatabase_NNsh(dbRef);
    return (count);
}
#endif  // #ifdef USE_LOGCHARGE
