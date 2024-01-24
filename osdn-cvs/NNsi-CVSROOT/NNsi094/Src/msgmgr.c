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
/*                                              (�C���f�b�N�X�擾�̎�����) */
/*=========================================================================*/
static Err createThreadIndexSub(Char   *buffer, UInt32 size, UInt32 offset,
                                UInt32 *nofMsg, UInt32 *msgOffset)
{
    Char *chkPtr, *ptr;
    
    chkPtr = buffer;
    while (chkPtr < buffer + size)
    {
        // ���b�Z�[�W�̋�؂��T��
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
            // �Ƃ肠�������ɂ�����
            chkPtr = ptr + 1;
        }
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : get_subject_database                                       */
/*                                                         �X�������擾  */
/*=========================================================================*/
Err Get_Subject_Database(UInt16 index, NNshSubjectDatabase *subjDB)
{
    Err       ret;
    DmOpenRef dbRef;

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_INFO, "ERR>OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }

    // �X���f�[�^���擾
    ret = GetRecord_NNsh(dbRef, index, sizeof(NNshSubjectDatabase), subjDB);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_INFO, "ERR>GetRecord_NNsh() ", " ", ret);
    }
    CloseDatabase_NNsh(dbRef);
    return (ret);
}

/*=========================================================================*/
/*   Function : MoveMessageToReadOnly                                      */
/*                                  (���b�Z�[�W���Q�Ɛ�p�X���Ɉړ�������) */
/*=========================================================================*/
Err MoveMessageToReadOnly(UInt16 index)
{
    Err                 ret;
    NNshSubjectDatabase subjDb;

    ////////////////////  �f�[�^�x�[�X�̏����X�����X�V  ////////////////////

    // �̈�̏�����
    MemSet(&subjDb, sizeof(subjDb), 0x00);

    // �f�[�^�x�[�X����f�[�^���擾
    ret = Get_Subject_Database(index, &subjDb);
    if (ret != errNone)
    {
        // ���肦�Ȃ��͂������H
        NNsh_DebugMessage(ALTID_INFO, "get_subject_database()"," ", ret);
        return (ret);
    }

    // �X�������u�ǂ݂��ݐ�p�v�ɍX�V
    MemSet (subjDb.boardNick, MAX_NICKNAME, 0x00);
    StrCopy(subjDb.boardNick, OFFLINE_THREAD_NICK);

    // �f�[�^���X�V
    ret = update_subject_database(index, &subjDb);
    if (ret != errNone)
    {
        // ���肦�Ȃ��͂�...
        NNsh_DebugMessage(ALTID_INFO, "update_subject_database()"," ", ret);
        return (ret);
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : createThreadIndexSub_MachiBBS                              */
/*                            (�C���f�b�N�X�擾�̎�����:�܂�BBS�p�`���[��) */
/*-------------------------------------------------------------------------*/
static Err createThreadIndexSub_MachiBBS(Char   *buffer, UInt32 size,
                           UInt32 offset, UInt32 *nofMsg, UInt32 *msgOffset)
                                
{
    Char *chkPtr, *ptr;
    
    chkPtr = buffer;

    // �擪����ǂݏo�����Ƃ��A���b�Z�[�W�̐擪����肷��
    if (*nofMsg == 0)
    {
        ptr = StrStr(chkPtr, NNSH_MACHITOKEN_START);
	if (ptr == NULL)
        {
            // ��؂肪������Ȃ������A��͏I��(�G���[����)
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
        // ���b�Z�[�W�̋�؂��T��
        ptr = StrStr(chkPtr, NNSH_MACHITOKEN_END);
        if (ptr == NULL)
        {
            // ��؂肪�Ȃ�����A��͏I��
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
            // ���b�Z�[�W�̊i�[�\�ȍő吔�𒴂���(�I�[�o�t���[�\��)
            NNsh_InformMessage(ALTID_WARN, "Index Number overflow", "", 0);
            return (~errNone - 1);
        }
    }
    NNsh_DebugMessage(ALTID_INFO, "Parsed Token :", "", *nofMsg);
    return (errNone);
}

/*=========================================================================*/
/*   Function : createMessageThreadIndex                                   */
/*                                (���b�Z�[�W�X���b�h�̃C���f�b�N�X�쐬)   */
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

    // ARMlet�̊֐��|�C���^���擾����
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

    // �C���f�b�N�X�쐬���\��
    Show_BusyForm(MSG_CREATE_MESSAGE_IDX);

    // �ꎞ�I�ɓǂݍ��ޗ̈�̏�����
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
            //  �܂�BBS�̓g�[�N�����������߁A�g�[�N���r���Ńo�b�t�@���s����
            // �Ƃ��̑΍����ꂽ(����)�B
            if (ret == errNone)
            {
                chkPoint = readSize - sizeof(NNSH_MACHITOKEN_END) + 2;
            }
            else
            {
                chkPoint = readSize;
            }
            //  �܂�BBS�̃C���f�b�N�X��͏���
            ret = createThreadIndexSub_MachiBBS(index->buffer, readSize,
                                                fileSize, nofMsg, msgOffset);
            readSize = chkPoint;
        }
        else
        {
#ifdef USE_ARMLET
            //  2�����˂�̃C���f�b�N�X��͏���
            if (armP != NULL)
            {
                // ARMlet���g�p���ẴC���f�b�N�X��͏���
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
                // ARMlet���g�p���Ȃ��ꍇ�̃C���f�b�N�X��͏���
                ret = createThreadIndexSub(index->buffer, readSize,
                                           fileSize, nofMsg, msgOffset);
            }
        }
        if (ret != errNone)
        {
            // �C���f�b�N�X�I�[�o�t���[����(���肦�Ȃ��͂�...)�A�r���ł�߂�
            // ���b�Z�[�W�̊i�[�\�ȍő吔�𒴂���(�I�[�o�t���[�\��)
            NNsh_InformMessage(ALTID_WARN, "Index Number overflow", "", 0);

            // ���ɂ����ނ悤�A�G���[�����߂��Ȃ��悤�ɂ���B
            ret = errNone;
            break;
        }

        // ����ǂݍ��݈ʒu�� fileSize�ɐݒ肷��B
        fileSize = fileSize + readSize;        
    }

FUNC_END:
#ifdef USE_ARMLET
    // ARMlet�̊֐������
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
/*                                                         �X�������X�V  */
/*-------------------------------------------------------------------------*/
Err update_subject_database(UInt16 index, NNshSubjectDatabase *subjDB)
{
    Err       ret;
    DmOpenRef dbRef;

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_INFO, "ERR>OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }

    // �X���f�[�^���X�V
    ret = UpdateRecord_NNsh(dbRef,index,sizeof(NNshSubjectDatabase),subjDB);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_INFO, "ERR>GetRecord_NNsh() ", " ", ret);
    }
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_entryDatabase                                    */
/*                                                ���b�Z�[�W����DB�ɓo�^ */
/*-------------------------------------------------------------------------*/
static void message_entryDatabase(UInt16 idxMES,UInt32 fileSize,UInt16 bbsType)
{
    Err                 ret;
    NNshSubjectDatabase subjDb;

    // �̈�̏�����
    MemSet(&subjDb, sizeof(subjDb), 0x00);

    if (idxMES != NNSH_NOTENTRY_THREAD)
    {
        // �f�[�^�x�[�X����f�[�^���擾
        ret = Get_Subject_Database(idxMES, &subjDb);
        if (ret != errNone)
        {
            // ���肦�Ȃ��͂������H
            NNsh_DebugMessage(ALTID_INFO, "get_subject_database()"," ", ret);
            return;
        }
    }

    ///// DB�ɏ������ރf�[�^���쐬���� /////

    // ��Ԓl�ق�
    if (fileSize != 0)
    {
        subjDb.state        = NNSH_SUBJSTATUS_NEW;
        subjDb.currentLoc   = 1;
        subjDb.fileSize     = fileSize;
        subjDb.bbsType      = (UInt8) bbsType;
        subjDb.msgAttribute = (subjDb.msgAttribute & NNSH_MSGATTR_NOTERROR);

        // �X���̋L�^���o�^
        if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // �X���L�^���VFS
            subjDb.msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // �X���L�^���Palm�{��
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

    // �f�[�^���X�V
    ret = update_subject_database(idxMES, &subjDb);
    if (ret != errNone)
    {
        // ���肦�Ȃ��͂�...
        NNsh_DebugMessage(ALTID_INFO, "update_subject_database()","",ret);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_entryMessage                                     */
/*                                                  ���b�Z�[�W��ϊ��A�o�^ */
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

    // "���b�Z�[�W�ϊ���" �\�����s��
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_INFO, "Read File Open :", FILE_RECVMSG, ret);
    }

    // ���Ƀt�@�C�������݂����ꍇ�͈�U�폜���Ă���I�[�v������B
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

    // ��M���b�Z�[�W�T�C�Y���擾
    GetFileSize_NNsh(&fileRefR, &fileSize);

    // �ꎞ�o�b�t�@�̈���m��
    realBuf = MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MemPtrNew()", " size:",
                          NNshParam->bufferSize + MARGIN);
        ret = ~errNone;
        goto END_FUNC;
    }
    buffer = realBuf;

    // ����f�[�^�̓ǂݍ���
    MemSet(buffer, (NNshParam->bufferSize + MARGIN), 0x00);
    ret = ReadFile_NNsh(&fileRefR, offset,
                        NNshParam->bufferSize, buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :",  FILE_RECVMSG, ret);
        goto END_FUNC;
    }
    ret = errNone;

    // �����_�E�����[�h���́A�t�@�C���̍Ō���׍H����(������Ǝ��Ԃ����邯��)
    if (NNshParam->enablePartGet != 0)
    {
        while ((readSize != 0)&&
               (*((Char *) (buffer + readSize - 1)) != '\x0a'))
        {
            readSize--;
        }
        // �����I�Ɏ�M�t�@�C���T�C�Y�����b�Z�[�W�̍Ō�ɒ�������B
        fileSize = readSize;
    }

    // HTTP�w�b�_�̏I������������
    ptr = StrStr(buffer, "\x0d\x0a\x0d\x0a");
    if (ptr == NULL)
    {
        // �{�f�B�f�[�^�Ȃ�
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "[\xd\xa\xd\xa])", 0);
        ret = ~errNone + 3;
        goto END_FUNC;
    }

    // HTTP�̉����R�[�h���`�F�b�N����(HTTP/1.[01] 20[06]��������)
    //   StrLen("HTTP/1.1 200") ==> 12 �ɒu��(�������̂���)
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
        // �T�[�o���X�|���X�ُ�
        NNsh_InformMessage(ALTID_ERROR, MSG_RECV_NOTACCEPTED, buffer, 0);
        ret = ~errNone + 10;
        goto END_FUNC;
    }
    ptr = ptr + 4;
    *dataSize = fileSize - (ptr - buffer);

    // ��f�[�^�������݂̃`�F�b�N(�ނ�������ɒ�������)
    if (((UInt32) ptr & 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
    {
        ptr--;
        *ptr = ' ';
    }

    // �f�[�^�̏�������(�ǋL)
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

    // �f�[�^���Ō�ɂȂ�܂ŒǋL����
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
/*                                            ���b�Z�[�W��ϊ��A�o�^(�ǋL) */
/*-------------------------------------------------------------------------*/
static Err message_appendMessage(Char *targetFile, NNshSubjectDatabase *mesDB)
{
    Err          ret;
    UInt32       fileSize, offset, readSize, writeSize, parseSize,chkSize;
    Char        *buffer, *ptr, *realBuf;
    NNshFileRef  fileRefR, fileRefW;

    // �f�[�^�o�^���\��
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

    // �ꎞ�o�b�t�@�̈���m��
    realBuf = MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MemPtrNew()", " size:",
                          NNshParam->bufferSize + MARGIN);
        goto END_FUNC;
    }
    buffer = realBuf;

    // ��M���b�Z�[�W�̃T�C�Y���擾(�w�b�_����)
    GetFileSize_NNsh(&fileRefR, &fileSize);
    parseSize = 0;
    readSize  = 0;
    offset    = 0;

    // ����f�[�^�̓ǂݍ���
    MemSet(buffer, NNshParam->bufferSize + MARGIN, 0x00);
    ret = ReadFile_NNsh(&fileRefR, offset, NNshParam->bufferSize,
                        buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh()", "", ret);

        goto END_FUNC;
    }

    // �����_�E�����[�h���́A�t�@�C���̍Ō���׍H����(������Ǝ��Ԃ����邯��)
    if (NNshParam->enablePartGet != 0)
    {
        while ((readSize != 0)&&
               (*((Char *) (buffer + readSize - 1)) != '\x0a'))
        {
            readSize--;
        }
        // �����I�Ɏ�M�t�@�C���T�C�Y�����b�Z�[�W�̍Ō�ɒ�������B
        fileSize = readSize;
    }

    // HTTP�{�f�B����������
    ptr = StrStr(buffer, "\x0d\x0a\x0d\x0a");
    if (ptr == NULL)
    {
        // �f�[�^�ُ�
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(\x0d\x0a\x0d\x0a))", 0);
        goto END_FUNC;
    }
    parseSize = ptr - buffer;
    ptr       = ptr + 4;
    parseSize = parseSize + 4;

    // HTTP�̉����R�[�h�`�F�b�N
    // StrLen("HTTP/1.1 200") ==> 12 �ɒu��(�������̂���)
    if ((StrNCompare(buffer,"HTTP/1.1 200 OK", 12) == 0)||
        (StrNCompare(buffer,"HTTP/1.0 200 OK", 12) == 0))
    {
        // �Ȃ񂩒m��񂯂ǁA�S���b�Z�[�W���ł���(���ȁH)�B
        CloseFile_NNsh(&fileRefR);
        CloseFile_NNsh(&fileRefW);
        if (realBuf != NULL)
        {
            MemPtrFree(realBuf);
        }
        Hide_BusyForm();

        // ���b�Z�[�W���ׂĂ����ł������Ƃ�DEBUG�\��
        NNsh_DebugMessage(ALTID_INFO, "Received ALL MESSAGE!! ", 
                          targetFile, 0);

        // ���ׂẴf�[�^����M���鏈���ɐ؂�ւ���
        ret = errNone;
        ret = message_entryMessage(targetFile, &fileSize);
        mesDB->state      = NNSH_SUBJSTATUS_NEW;
        mesDB->fileSize   = fileSize;

        // �X���̋L�^���o�^
        if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // �X���L�^���VFS
            mesDB->msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // �X���L�^���Palm�{��
            mesDB->msgState = FILEMGR_STATE_OPENED_STREAM;
        }
        return (ret);
    }

    if ((parseSize + 1 == readSize)&&(*ptr == '\x0a'))
    {
        // �X���X�V�Ȃ�
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
        // �f�[�^�G���[�H(�G���[�̏ڍׂ͖��`�F�b�N)
        CloseFile_NNsh(&fileRefR);
        CloseFile_NNsh(&fileRefW);
        if (realBuf != NULL)
        {
            MemPtrFree(realBuf);
        }
        Hide_BusyForm();

        return (NNSH_MSGSTATE_ERROR);
    }
    // �X�V�`�F�b�N�p�̕���(1�o�C�g)�������߂�B
    ptr++;
    parseSize++;
    fileSize = fileSize - (ptr - buffer);

    if (((UInt32) ptr & 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
    {
        ptr--;
        *ptr = ' ';
    }
     // �f�[�^�̏�������(�ǋL����)
    chkSize  = readSize - (ptr - buffer);
    if (((chkSize % 2) == 1)&&(NNshParam->useVFS & NNSH_VFS_WORKAROUND))
    {
        //  �Ō�̕������L�ڂ��Ȃ��̂ł͂Ȃ��A�Ō�̕����̑O�ɋ󔒂�}������
        // ���ƂŁA�������݃o�C�g���������ɂ���B(v0.80r003) ����ł����H
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
        // update����
        mesDB->state = NNSH_SUBJSTATUS_UPDATE;

        // �X���̋L�^���o�^
        if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // �X���L�^���VFS
            mesDB->msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // �X���L�^���Palm�{��
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
/*                                                        ���b�Z�[�W�S�擾 */
/*=========================================================================*/
Err Get_AllMessage(Char *url, Char *boardNick, Char *file, UInt16 idxMES,
                   UInt16 bbsType)
{
    Err                  ret;
    UInt32               fileSize, range;
    Char                 fileName[MAXLENGTH_FILENAME];

    // ���b�Z�[�W���i�[����t�@�C�������쐬
    MemSet (fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, boardNick);
    StrCat (fileName, file);

    // �����_�E�����[�h�w�莞�ɂ̓����W�ݒ���s��
    if (NNshParam->enablePartGet != 0)
    {
        range = (UInt32) NNshParam->partGetSize - NNSH_HTTPGET_HEADERMAX;
    }
    else
    {
        range = HTTP_RANGE_OMIT;
    }

    // ���b�Z�[�W����M
    ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                        HTTP_GETSUBJECT_START, range);
    if (ret != errNone)
    {
        // �ʐM�G���[����
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

    // ��M�����f�[�^��ϊ��A�o�^����B
    ret = message_entryMessage (fileName, &fileSize);
    message_entryDatabase(idxMES, fileSize, bbsType);

    return (ret);
}

/*=========================================================================*/
/*   Function :   Get_PartMessage                                          */
/*                                                      ���b�Z�[�W�����擾 */
/*=========================================================================*/
Err Get_PartMessage(Char *url, Char *boardNick, NNshSubjectDatabase *mesDB, 
                    UInt16 idxMES)
{
    Err    ret;
    Char   fileName[MAXLENGTH_FILENAME];
    UInt32 range;

    // ���b�Z�[�W���i�[����t�@�C�������쐬
    MemSet (fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, boardNick);
    StrCat (fileName, mesDB->threadFileName);

    // �����_�E�����[�h�w�莞�ɂ̓����W�ݒ���s��
    if (NNshParam->enablePartGet != 0)
    {
        range = (UInt32) NNshParam->partGetSize - 
                         NNSH_HTTPGET_HEADERMAX + (mesDB->fileSize - 1);
    }
    else
    {
        range = HTTP_RANGE_OMIT;
    }
    // ���b�Z�[�W�𑗎�M
    ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                        (mesDB->fileSize - 1), range);
    if (ret != errNone)
    {
        // �ʐM�G���[����
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

    // ��M�����f�[�^��ϊ��A�o�^����B
    ret = message_appendMessage(fileName, mesDB);
    switch (ret)
    {
      case errNone:
        // �f�[�^�ϊ������A���ւ����ށB
        break;

      case NNSH_MSGSTATE_NOTMODIFIED:
        // �f�[�^���X�V
        NNsh_InformMessage(ALTID_INFO, 
                           MSG_RECV_NOTMODIFY, mesDB->threadTitle, 0);
        return (~errNone - 11);
        break;

      case NNSH_MSGSTATE_ERROR:
        // �f�[�^��M���s
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

    // �X�����b�Z�[�W�f�[�^�x�[�X�̓��e���X�V����B
    ret = update_subject_database(idxMES, mesDB);
    if (ret != errNone)
    {
        // ���肦�Ȃ��͂�...
        NNsh_DebugMessage(ALTID_INFO, "update_subject_database()"," ", ret);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function : MoveSubject_BBS                                            */
/*                                              �X���̏��������ύX���� */
/*=========================================================================*/
Err MoveSubject_BBS(Char *dest, Char *src)
{
    Err                 ret;
    UInt16              index, cnt;
    DmOpenRef           dbRef;
    NNshSubjectDatabase subjDB;

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }
    GetDBCount_NNsh(dbRef, &cnt);
    for (index = 0; index < cnt; cnt++)
    {
        // �X���f�[�^���擾
        ret = GetRecord_NNsh(dbRef, index,
                             sizeof(NNshSubjectDatabase), &subjDB);
        if (ret != errNone)
        {
            // �G���[����(�f�o�b�O�\��)
            NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", " ", ret);
            continue;
        }

        // �w�肵���{�[�h�j�b�N�l�[�����`�F�b�N����
        if (StrCompare(subjDB.boardNick, src) == 0)
        {
            // �X�����ړ�������
            MemSet (subjDB.boardNick, MAX_NICKNAME, 0x00);
            StrCopy(subjDB.boardNick, dest);
            UpdateRecord_NNsh(dbRef, index,
                              sizeof(NNshSubjectDatabase), &subjDB);
        }
    }
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));


    return (errNone);
}

/*=========================================================================*/
/*   Function : MarkMessageIndex                                           */
/*                                           ���b�Z�[�W�ǂݏo���󋵂�ݒ�  */
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
                // ���C�ɓ���ݒ�͕ύX���Ȃ�
                break;
              case 0:
                // ���C�ɓ���ݒ���N���A����
                subjDB.msgAttribute 
                               = (subjDB.msgAttribute & NNSH_MSGATTR_NOTFAVOR);
                break;
              default:
                // ���C�ɓ���ݒ�����s����
                subjDB.msgAttribute 
                                  = (subjDB.msgAttribute | NNSH_MSGATTR_FAVOR);
                break;
            }

            // MSG�̏ꏊ���s���������ꍇ�ɂ́A�L�^����L������B
            if ((subjDB.msgState == 0)&&
                (subjDB.maxLoc != 0)&&(subjDB.currentLoc != 0))
            {
                if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
                {
                    // �X���L�^���VFS
                    subjDB.msgState = FILEMGR_STATE_OPENED_VFS;
                }
                else
                {    
                    // �X���L�^���Palm�{��
                    subjDB.msgState = FILEMGR_STATE_OPENED_STREAM;
                }
            }
            (void) update_subject_database(index, &subjDB);
        }
    }
    return;
}
