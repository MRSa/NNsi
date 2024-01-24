/*============================================================================*
 *
 *  $Id: msgmgr.c,v 1.97 2012/01/10 16:27:41 mrsa Exp $
 *
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
            if (*nofMsg < (NNshGlobal->NNsiParam)->msgNumLimit)
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
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }

    // �X���f�[�^���擾
    ret = GetRecord_NNsh(dbRef, index, sizeof(NNshSubjectDatabase), subjDB);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", " ", ret);
    }
    CloseDatabase_NNsh(dbRef);
    return (ret);
}

/*=========================================================================*/
/*   Function : MoveMessageToReadOnly                                      */
/*                                  (���b�Z�[�W���Q�ƃ��O�X���Ɉړ�������) */
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
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()"," ", ret);
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
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()"," ", ret);
        return (ret);
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : checkLogChargeThread                                       */
/*                                        (���O�`���[�W�����X�����`�F�b�N) */
/*-------------------------------------------------------------------------*/
static void checkLogChargeThread(Char *fileName, NNshGetLogDatabase *dbData, UInt32 *tokenId) 
{
    DmOpenRef  dbRef;
    Char      *chkPtr;
    UInt16     recNum, depth;

    // ���O�`���[�W�������O���ǂ����m�F����
    depth = 1;
    chkPtr = StrStr(fileName, FILE_LOGCHARGE_PREFIX);
    if (chkPtr != NULL)
    {
        // ���O�`���[�W�������O�Ɗm�F�A���R�[�h�ԍ���؂�o��
        chkPtr = chkPtr +  (sizeof(FILE_LOGCHARGE_PREFIX) - 1);
        recNum = StrAToI(chkPtr);
        if (recNum == NNSH_MSGNUM_UNKNOWN)
        {
            // �f�[�^�����O�`���[�W�������O�ł͂Ȃ�
            // (�����Ńt�@�C������t���������O)
            MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);
            *tokenId = 0;
            return;
        }

        // �Q�ƃ��O�`���[�W�pDB�̎擾
        OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
        if (dbRef != 0)
        {
            // ���ODB�̃I�[�v�������A�؂�o�������R�[�h�ԍ��f�[�^���擾
            MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);

            // ���R�[�h�̓ǂݏo��
            GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), dbData);

            // �f�[�^�x�[�X�����
            CloseDatabase_NNsh(dbRef);
        }

        // �擾�[�x���m�F
        while (*chkPtr != '-')
        {
            chkPtr++;
        }
        chkPtr++;
        depth = StrAToI(chkPtr);
    }
    
    if (depth != 0)
    {
        // �擾�[�x���Ⴄ�ꍇ�A�f�[�^���N���A����
        *tokenId = dbData->subTokenId;   
        MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);
    }
    else
    {
        *tokenId = dbData->tokenId;   
    }
    return;
}


/*-------------------------------------------------------------------------*/
/*   Function : createThreadIndexSub_HTML                                  */
/*                                            (HTML�p�C���f�b�N�X��������) */
/*-------------------------------------------------------------------------*/
static Err createThreadIndexSub_HTML(Char   *buffer,
                                      UInt32 *bufSize, UInt32 offset,
                                      UInt32 *nofMsg,  UInt32 *msgOffset, 
                                      Char *fileName,  UInt16 kanjiCode, 
                                      UInt16 *status,  UInt32 tokenId)
{
    Char *chkPtr, *ptr, *start, *end, *token, dummy, *orgPtr;
    NNshLogTokenDatabase *rec;
    UInt32 size;

    size = *bufSize;   
    NNsh_DebugMessage(ALTID_INFO, "createThreadIndexSub_HTML", "", 0);

    // �f�[�^�̖�����������
    if (*status == NNSH_MSGTOKENSTATE_END)
    {
        // ��؂�f�[�^�̖����܂ŌĂяo���Ă����A���b�Z�[�W��͏��������Ȃ�
        return (errNone);
    }

    rec = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (rec == NULL)
    {
        // �f�[�^���w�肳��Ă��Ȃ��Ƃ��A�f�[�^���_�~�[�ɒu��������
        dummy  = 0;
        start = &dummy;
        end   = &dummy;
        token = &dummy;
    }
    else
    {
        MemSet(rec, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);
#ifdef USE_LOGCHARGE
        if (GetLogToken(tokenId, rec) != errNone)
        {
            // �f�[�^���w�肳��Ă��Ȃ��Ƃ��A�f�[�^���_�~�[�ɒu��������
            dummy  = 0;
            start = &dummy;
            end   = &dummy;
            token = &dummy;
            MEMFREE_PTR(rec);
            rec = NULL;
        }
        else
        {
            start = rec->startToken;
            end   = rec->endToken;
            token = rec->dataToken;
        }
#else   // #ifdef USE_LOGCHARGE
        {
            // �f�[�^���w�肳��Ă��Ȃ��Ƃ��A�f�[�^���_�~�[�ɒu��������
            dummy  = 0;
            start = &dummy;
            end   = &dummy;
            token = &dummy;
            MEMFREE_PTR(rec);
            rec = NULL;
        }
#endif   // #ifdef USE_LOGCHARGE
    }

    // �f�[�^�̐擪��������
    chkPtr = buffer;
    if (*status == NNSH_MSGTOKENSTATE_NONE)
    {
        //  �擪�̋�؂蕶�����T��
        if (*start != 0)
        {
            ptr = StrStr(buffer, start);
            if (ptr == NULL)
            {
                // �f�[�^�擪�̋�؂肪������Ȃ������A��͏I��
                MEMFREE_PTR(rec);
                *bufSize = *bufSize - StrLen(start); // ��؂蕶�����߂�...
                return (errNone);
            }
            ptr = ptr + StrLen(start);
            msgOffset[0] = ((UInt32) (ptr - buffer)) + offset;
            chkPtr = ptr;
            *status = NNSH_MSGTOKENSTATE_MESSAGE;
        }
        else
        {
            *status = NNSH_MSGTOKENSTATE_MESSAGE;
        }
    }
    if (*status == NNSH_MSGTOKENSTATE_MESSAGE)
    {
        // �����̋�؂蕶���������Ă��邩�T��
        if (*end != 0)
        {
            ptr = StrStr(buffer, end);
            if (ptr != NULL)
            {
                // �����f�[�^�𔭌��I �f�[�^�͂����܂łƂ���
                *status = NNSH_MSGTOKENSTATE_END;
                size = (ptr - buffer) + StrLen(end);
            }
        }
    }

    // �f�[�^�̋�؂蕶�����T��
    if (*token != 0)
    {
        while (chkPtr < buffer + size)
        {
            // ���b�Z�[�W�̋�؂��T��
            ptr = StrStr(chkPtr, token);
            if (ptr == NULL)
            {
                // ��؂肪�Ȃ�����A��͏I��
                *bufSize = *bufSize - StrLen(token); // ��؂蕶�����߂�...
                break;
            }
            ptr = ptr + StrLen(token);
            if (*nofMsg < (NNshGlobal->NNsiParam)->msgNumLimit)
            {
                (*nofMsg)++;
                msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
                chkPtr = ptr;
            }
            else
            {
                // ���b�Z�[�W�̊i�[�\�ȍő吔�𒴂���(�I�[�o�t���[�\��)
                NNsh_WarningMessage(ALTID_WARN, "Index Number overflow", "", 0);
                MEMFREE_PTR(rec);
                return (~errNone - 1);
            }
        }
    }
    else
    {
        // ���̃p�[�X����(BIGBUF * 2(1280bytes)�ŁA�C���f�b�N�X���쐬����)
        // �������A�g�[�N��'>'�ŋ�؂�B
        ptr = chkPtr;
        while(chkPtr < buffer + size)
        {
            ptr = ptr + BIGBUF * 2;
            if (ptr > buffer + size)
            {
                ptr = buffer + size;
            }
            orgPtr = ptr;
            if (kanjiCode == NNSH_KANJICODE_JIS)
            {
                // JIS�����R�[�h�̂Ƃ��A�A�A
                while ((*ptr != '\0')&&(ptr > buffer))
                {
                    // JIS�����R�[�h�ł́A�^�O�̃R�[�h�������邽�߁A���s�R�[�h�Ő؂�
                    if (*ptr == 0x0a)
                    {
                        ptr++;
                        break;
                    }
                    // �G�X�P�[�v�����ł���؂�
                    if (*ptr == 0x1b)
                    {
                        break;
                    }
                    ptr--;
                }
            }
            else
            {
                // �����R�[�h��JIS�ȊO
                while ((*ptr != '\0')&&(ptr > chkPtr))
                {
                    // �^�O�܂��͉��s�R�[�h�Ő؂� 
                    if ((*ptr == '>')||(*ptr == '\x0a'))
                    {
                        ptr++;
                        break;
                    }
                    ptr--;
                }
                if (ptr == chkPtr)
                {
                    // �^�O���܂܂�Ă��Ȃ�����...
                    ptr = orgPtr;
                }
            }

            if (*nofMsg < (NNshGlobal->NNsiParam)->msgNumLimit)
            {
                (*nofMsg)++;
                msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
                chkPtr = ptr;
            }
            else
            {
                // ���b�Z�[�W�̊i�[�\�ȍő吔�𒴂���(�I�[�o�t���[�\��)
                NNsh_WarningMessage(ALTID_WARN, "Index Number overflow", "", 0);
                MEMFREE_PTR(rec);
                return (~errNone - 1);
            }
        }
    }
    MEMFREE_PTR(rec);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : createThreadIndexSub_MachiBBS                              */
/*                            (�C���f�b�N�X�擾�̎�����:�܂�BBS�p�`���[��) */
/*-------------------------------------------------------------------------*/
static Err createThreadIndexSub_MachiBBS(Char  *buffer,    UInt32  size,
                                         UInt32  offset,    UInt32 *nofMsg, 
                                         UInt32 *msgOffset, UInt16 endTokenType, 
                                         Char   *start,     Char   *end)
{
    Char   *chkPtr, *ptr, *stPtr;

    chkPtr = buffer;

    // �擪����ǂݏo�����Ƃ��A���b�Z�[�W�̐擪����肷��
    if (*nofMsg == 0)
    {
        ptr = StrStr(chkPtr, start);
        if (ptr == NULL)
        {
            // ��؂肪������Ȃ������A��͏I��(�G���[����)
            NNsh_DebugMessage(ALTID_ERROR, "Not Found Message Top :", start, 0);
            return (~errNone - 2);
        }
        ptr = ptr + StrLen(start);
        msgOffset[0] = ((UInt32) (ptr - buffer)) + offset;
        chkPtr = ptr;
    }
    while (chkPtr < buffer + size)
    {
        // ���b�Z�[�W�̋�؂��T��
        ptr = StrStr(chkPtr, end);
        if ((ptr == NULL)||(ptr > (buffer + size)))
        {
            // ��؂肪�Ȃ�����A��͏I��

            // �ł��A�R�����g�J�n�̕�����(��؂�̌���)�������ꍇ�ɂ́A�������ŏI�̃��X�ʒu�Ƃ���
            stPtr = StrStr(chkPtr, NNSH_MACHITOKEN_BEGINCOMMENT);
            if ((stPtr != NULL)&&(stPtr <= (buffer + size)))
            {
                (*nofMsg)++;
                msgOffset[(*nofMsg)] = (UInt32) stPtr - (UInt32) buffer + offset;
            }
            break;
        }

        // ��؂�g�[�N���^�C�v�̎w����m�F
        if (endTokenType != NNSH_DISABLE)
        {
            // ��؂�g�[�N���^�C�v�̎w�肪�������ꍇ...

            // ��؂�g�[�N���̑O�ɂ���f�[�^���m�F����
            if ((endTokenType & NNSH_TOKEN_BEFORE_NUMBER) == NNSH_TOKEN_BEFORE_NUMBER)
            {
                // ��؂�g�[�N���̑O�f�[�^�͐����łȂ���΂Ȃ�Ȃ�
                if ((*(ptr - 1) < '0')||(*(ptr - 1) > '9'))
                {
                    // ��؂�g�[�N���ł͂Ȃ������A�������
                    chkPtr = ptr + StrLen(end);
                    continue;
                }
            }
            if ((endTokenType & NNSH_TOKEN_BEFORE_LETTER) == NNSH_TOKEN_BEFORE_LETTER)
            {
                // ��؂�g�[�N���̑O�f�[�^�͕����łȂ���΂Ȃ�Ȃ�
                if (*(ptr - 1) <= ' ')
                {
                    // ��؂�g�[�N���ł͂Ȃ������A�������
                    chkPtr = ptr + StrLen(end);
                    continue;
                }
            }

            // ��؂�g�[�N���̎��Ƀ|�C���^��i�߂�
            ptr = ptr + StrLen(end);

            // ��؂�g�[�N���̌�ɂ���f�[�^���m�F����
            if ((endTokenType & NNSH_TOKEN_AFTER_NUMBER) == NNSH_TOKEN_AFTER_NUMBER)
            {
                // ��؂�g�[�N���̌�f�[�^�͐����łȂ���΂Ȃ�Ȃ�
                if ((*(ptr + 1) < '0')||(*(ptr + 1) > '9'))
                {
                    // ��؂�g�[�N���ł͂Ȃ������A�������
                    chkPtr = ptr;
                    continue;
                }
            }
            if ((endTokenType & NNSH_TOKEN_AFTER_LETTER) == NNSH_TOKEN_AFTER_LETTER)
            {
                // ��؂�g�[�N���̌�f�[�^�͕����łȂ���΂Ȃ�Ȃ�
                if (*(ptr + 1) <= ' ')
                {
                    // ��؂�g�[�N���ł͂Ȃ������A�������
                    chkPtr = ptr;
                    continue;
                }
            }
        }
        else
        {      
            // ��؂�f�[�^�^�C�v�̎w�肪�Ȃ������ꍇ�ɂ́A
            // �������ɋ�؂�Ɣ��f���A��؂�g�[�N���̎��Ƀ|�C���^��i�߂�
            ptr = ptr + StrLen(end);

            // ��؂蕶���̎��ɊJ�n�����񂪂Ȃ��ꍇ�ɂ͂܂��P�̃��X�Ƃ��Ĕ��肵�Ă݂�B
            stPtr = StrStr((ptr - 1), start);
            if (stPtr != (ptr - 1))
            {
                // ��؂�g�[�N���ł͂Ȃ������A�������
                chkPtr = ptr;
                continue;
            }
        }

        if (*nofMsg < (NNshGlobal->NNsiParam)->msgNumLimit)
        {
            (*nofMsg)++;
            msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
            chkPtr = ptr;
        }
        else
        {
            // ���b�Z�[�W�̊i�[�\�ȍő吔�𒴂���(�I�[�o�t���[�\��)
            NNsh_WarningMessage(ALTID_WARN, "Index Number overflow", "", 0);
            return (~errNone - 1);
        }
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : createMessageThreadIndex                                   */
/*                                (���b�Z�[�W�X���b�h�̃C���f�b�N�X�쐬)   */
/*=========================================================================*/
Err CreateMessageThreadIndex(NNshMessageIndex *index, UInt32 *nofMsg,
                             UInt32 *msgOffset, UInt16 bbsType)
{
    Err          ret  = errNone;
    UInt32       fileSize, readSize, chkPoint;
    Char        *bufPtr, *ptr;
    NNshGetLogDatabase *dbData;
#ifdef USE_ARMLET
    UInt32       armArgs[7];
    MemHandle    armH;
    void        *armP;

    // ARMlet�̊֐��|�C���^���擾����
    armH = 0;
    armP = NULL;
    if ((NNshGlobal->NNsiParam)->useARMlet != 0)
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

    dbData = NULL;
    if ((bbsType & NNSH_BBSTYPE_MASK)  == NNSH_BBSTYPE_HTML)
    {
        // html�^�C�v�f�[�^�̂Ƃ��́ADB��������擾���Ă���
        dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
        if (dbData != NULL)
        {
            MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);

            // ���O�`���[�W�̃X�����ǂ����`�F�b�N����
            checkLogChargeThread(index->threadFile, dbData, &fileSize);
            if (NNshGlobal->tokenId == 0)
            {
                // �����̃C���f�b�N�X��̓f�[�^���p�����[�^�ɔ��f������
                NNshGlobal->tokenId = fileSize;
            }

            // �x�[�X�ɂȂ�URL�̐؂�o�������s
            if (dbData->dataURL[0] != 0x00)
            {
                StrNCopy(index->baseURL, dbData->dataURL, MAX_URL);
                for (ptr = index->baseURL + StrLen(index->baseURL) - 1;
                     ((*ptr != '/')&&(ptr > index->baseURL)); ptr--)
                {
                    // �������Ȃ�
                }
                ptr++;
                *ptr = '\0';
            }
        }
    }

    // �ꎞ�I�ɓǂݍ��ޗ̈�̏�����
    bufPtr   = index->buffer;
    *nofMsg  = 0;
    fileSize = 0;
    while (fileSize < index->fileSize)
    {
        MemSet(bufPtr, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
        readSize = 0;
        ret     = ReadFile_NNsh(&(index->fileRef), fileSize,
                                 (NNshGlobal->NNsiParam)->bufferSize, bufPtr, &readSize);
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
        switch (bbsType & NNSH_BBSTYPE_MASK)
        {
          case NNSH_BBSTYPE_MACHIBBS:
            //  �܂�BBS/������΁��i�a�a�r�̓g�[�N�����������߁A�g�[�N����
            // �r���Ńo�b�t�@���s�����Ƃ��̑΍����ꂽ(����)�B
            if (ret == errNone)
            {
                chkPoint = readSize - sizeof(NNSH_MACHITOKEN_END) + 2;
            }
            else
            {
                chkPoint = readSize;
            }
            //  �܂�BBS�̃C���f�b�N�X��͏���(html���)
            ret = createThreadIndexSub_MachiBBS(index->buffer, readSize,
                                                fileSize, nofMsg, msgOffset,
                                                NNSH_DISABLE,
                                                NNSH_MACHITOKEN_START,
                                                NNSH_MACHITOKEN_END);
            readSize = chkPoint;
            if (readSize < 10)
            {
                // �ŏI�o�C�g���E���Ȃ��̂ŁA�������...
                readSize = 10;
            }
            break;

          case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
            //  �܂�BBS/������΁��i�a�a�r�̓g�[�N�����������߁A�g�[�N����
            // �r���Ńo�b�t�@���s�����Ƃ��̑΍����ꂽ(����)�B
            if (ret == errNone)
            {
                chkPoint = readSize - sizeof(NNSH_JBBSTOKEN_END) + 2;
            }
            else
            {
                chkPoint = readSize;
            }
            //  �܂�BBS�̃C���f�b�N�X��͏���(html���)
            ret = createThreadIndexSub_MachiBBS(index->buffer, readSize,
                                                fileSize, nofMsg, msgOffset,
                                                NNSH_DISABLE,
                                                NNSH_JBBSTOKEN_START,
                                                NNSH_JBBSTOKEN_END);
            readSize = chkPoint;
            if (readSize < 10)
            {
                // �ŏI�o�C�g���E���Ȃ��̂ŁA�������...
                readSize = 10;
            }
            break;

          case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
            //  �܂�BBS/������΁��i�a�a�r�̓g�[�N�����������߁A�g�[�N����
            // �r���Ńo�b�t�@���s�����Ƃ��̑΍����ꂽ(����)�B
            if (ret == errNone)
            {
                chkPoint = readSize - sizeof(NNSH_JBBSTOKEN_RAW_END_RES) + 2;
            }
            else
            {
                chkPoint = readSize;
            }
            //  �܂�BBS�̃C���f�b�N�X��͏���(html���)
            ret = createThreadIndexSub_MachiBBS(index->buffer, readSize,
                                                fileSize, nofMsg, msgOffset,
                                                (NNSH_TOKEN_BEFORE_LETTER),
                                                NNSH_JBBSTOKEN_RAW_START_RES,
                                                NNSH_JBBSTOKEN_RAW_END_RES);
            readSize = chkPoint;
            if (readSize < 10)
            {
                // �ŏI�o�C�g���E���Ȃ��̂ŁA�������...
                readSize = 10;
            }
            break;

          case NNSH_BBSTYPE_HTML:
            //  chkPoint���[���̂Ƃ��ɂ́A��؂蕶������Ńo�b�t�@���s����
            // �Ƃ��̑΍�����{���� (��؂蕶���񒷂��ς̂���...)
            if (ret == errNone)
            {
                chkPoint = 0;
            }
            else
            {
                chkPoint = readSize;
            }
            // HTML�^�C�v�̃C���f�b�N�X�������
            ret = createThreadIndexSub_HTML(index->buffer, &readSize, 
                                            fileSize, nofMsg, msgOffset,
                                            index->threadFile, 
                                            ((bbsType & NNSH_BBSTYPE_CHARSETMASK) >> NNSH_BBSTYPE_CHAR_SHIFT),
                                            &(index->msgStatus), NNshGlobal->tokenId);
           if (chkPoint != 0)
           {
               readSize = chkPoint;
           }
           break;

#if 0
          case NNSH_BBSTYPE_2ch:
          case NNSH_BBSTYPE_2ch_EUC:
          case NNSH_BBSTYPE_SHITARABA:
          case NNSH_BBSTYPE_OTHERBBS:
          case NNSH_BBSTYPE_OTHERBBS_2:
#endif
          default:
            //  2�����˂�/������΂̃C���f�b�N�X��͏���(dat���ǂ�)
#ifdef USE_ARMLET
            if (armP != NULL)
            {
                // ARMlet���g�p���ẴC���f�b�N�X��͏���
                MemSet(armArgs, sizeof(armArgs), 0x00);
                armArgs[0] = (UInt32) msgOffset;
                armArgs[1] = (UInt32) nofMsg;
                armArgs[2] = (UInt32) fileSize;
                armArgs[3] = (UInt32) (NNshGlobal->NNsiParam)->msgNumLimit;
                armArgs[4] = (UInt32) readSize;
                armArgs[5] = (UInt32) index->buffer;
                ret        = (Err) PceNativeCall(armP, armArgs);
            }
            else
#endif
            {
                // ARMlet���g�p���Ȃ��ꍇ�̃C���f�b�N�X��͏���
                ret = createThreadIndexSub(index->buffer, readSize,
                                           fileSize, nofMsg, msgOffset);
            }
            break;
        }
        if (ret != errNone)
        {
            // �C���f�b�N�X�I�[�o�t���[����(���肦�Ȃ��͂�...)�A�r���ł�߂�
            // ���b�Z�[�W�̊i�[�\�ȍő吔�𒴂���(�I�[�o�t���[�\��)
            NNsh_WarningMessage(ALTID_WARN, "Index Number overflow", "", 0);

            // �Ƃ肠�����ǂ߂�悤�ɂ���(errNone�ɒu��)
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
    MEMFREE_PTR(dbData);
    Hide_BusyForm(true);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : getLastMessageNumShitaraba                                 */
/*                                 �X���Ɋi�[����Ă��郌�X�̍ő吔���擾  */
/*-------------------------------------------------------------------------*/
Err getLastMessageNumShitaraba(UInt16 *resNum)
{
    Err           ret;
    UInt32        fileSize, offset, readSize;
    Char         *realBuf, *ptr, *bottom;
    NNshFileRef   fileRef;

    // ������
    *resNum = 1;

    // "���b�Z�[�W�ϊ���" �\�����s��
    Show_BusyForm(MSG_MESSAGE_CHECK_LAST);

    // �ꎞ�o�b�t�@�̈���m��
    realBuf = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        // �ꎞ�̈�̊m�ۂɎ��s����
        Hide_BusyForm(false);
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        return (~errNone);
    }
    MemSet(realBuf, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    // �ǂݏo���t�@�C���̃I�[�v��
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        Hide_BusyForm(false);
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_RECVMSG, ret);
        return (ret);
    }

    // ��M���b�Z�[�W�T�C�Y���擾
    GetFileSize_NNsh(&fileRef, &fileSize);

    // �t�@�C���̓ǂݏo���ʒu��ݒ肷��
    if (fileSize <= (NNshGlobal->NNsiParam)->bufferSize)
    {
        offset = 0;
    }
    else
    {
        offset = fileSize - (NNshGlobal->NNsiParam)->bufferSize;
    }

    // �t�@�C������f�[�^��ǂݏo��(�t�@�C���̖�������)
    ret = ReadFile_NNsh(&fileRef, offset, (NNshGlobal->NNsiParam)->bufferSize, realBuf, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :",  FILE_RECVMSG, ret);
        goto END_FUNC;
    }

    //////////////////////////////////////////// ���X�ԍ��̌�������
    ptr = realBuf + readSize;
    while ((ptr - 1) > realBuf)
    {
        // ���X�ԍ����L������Ă������T��(�o�b�t�@�����ɂ���/\x0a[0-9]+<>/)
        if (((*ptr >= '0')&&(*ptr <= '9'))&&(*(ptr - 1) == '\x0a'))
        {
            // ���l�f�[�^��؂�o��
            bottom = ptr;
            while ((*bottom >= '0')&&(*bottom <= '9'))
            {
                bottom++;
            }
            if ((*bottom == '<')&&(*(bottom + 1) == '>'))
            {
                *bottom = '\0';
                *resNum = StrAToI(ptr);
                goto END_FUNC;
            }
        }
        ptr--;         
    }
    ////////////////////////////////////////////

END_FUNC:    
    MEMFREE_PTR(realBuf);
    Hide_BusyForm(false);

    NNsh_DebugMessage(ALTID_INFO, "RES NUM :",  "", *resNum);
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
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }

    // �X���f�[�^���X�V
    ret = UpdateRecord_NNsh(dbRef,index,sizeof(NNshSubjectDatabase),subjDB);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", " ", ret);
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
            NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()"," ", ret);
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
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // �X���L�^���VFS
            subjDb.msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // �X���L�^���Palm�{��
            subjDb.msgState = FILEMGR_STATE_OPENED_STREAM;
        }

        // ������΁�JBBS(rawmode)�̂Ƃ��́A�������X�ԍ����擾����
        if (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)
        {
            // �����̃��X�ԍ����擾����
            (void) getLastMessageNumShitaraba(&(subjDb.maxLoc));
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
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()","",ret);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_entryMessage                                     */
/*                                                  ���b�Z�[�W��ϊ��A�o�^ */
/*-------------------------------------------------------------------------*/
static Err message_entryMessage(Char *targetFile, UInt32 *dataSize, UInt16 bbsType)
{
    Err           ret;
    Boolean       deleteLogFile;
    UInt32        fileSize, offset, readSize, writeSize, chkSize;
    Char         *buffer, *ptr, *realBuf, *bakPtr;
    NNshFileRef  fileRefR, fileRefW;

    deleteLogFile = false;
    fileSize  = 0;
    offset    = 0;

    // "���b�Z�[�W�ϊ���" �\�����s��
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    // �f�[�^�T�C�Y��������
    *dataSize = 0;

    // �ǂݏo���t�@�C���̃I�[�v��
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        Hide_BusyForm(false);
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_RECVMSG, ret);
        return (ret);
    }

    // �ꎞ�o�b�t�@�̈���m��
    realBuf = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        // �ꎞ�̈�̊m�ۂɎ��s����
        Hide_BusyForm(false);
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        CloseFile_NNsh(&fileRefR);
        return (~errNone);
    }
    buffer = realBuf;
    MemSet(buffer, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    // ��M���b�Z�[�W�T�C�Y���擾
    GetFileSize_NNsh(&fileRefR, &fileSize);

    // �󂫗e�ʂ̊m�F(VFS�̂�)
    if (CheckFileSpace_NNsh(NNSH_DISABLE, fileSize) != errNone)
    {
        // �󂫗e�ʕs���A�I������B
        ret = ~errNone - 10;
        goto END_FUNC;
    }

    // ����f�[�^�̓ǂݍ���
    ret = ReadFile_NNsh(&fileRefR, offset,
                        (NNshGlobal->NNsiParam)->bufferSize, buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_DebugMessage(ALTID_ERROR, "ReadFile_NNsh :",  FILE_RECVMSG, ret);
        goto END_FUNC;
    }
    ret = errNone;

    // �����_�E�����[�h���́A�t�@�C���̍Ō���׍H����(������Ǝ��Ԃ����邯��)
    if ((NNshGlobal->NNsiParam)->enablePartGet != 0)
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
    ptr = StrStr(buffer, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // �{�f�B�f�[�^�Ȃ�
        NNsh_DebugMessage(ALTID_ERROR, "Wrong data(not found separator,",
                          "[\xd\xa\xd\xa])", 0);
        ret = ~errNone + 3;
        goto END_FUNC;
    }

    // HTTP�̉����R�[�h���`�F�b�N����(HTTP/1.[01] 20[06]��������)
    //   StrLen("HTTP/1.1 200") ==> 12 �ɒu��(�������̂���)
#ifdef USE_STRNCOMPARE
    if ((StrNCompare(buffer,"HTTP/1.1 200",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.1 206",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.0 206",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.0 200",12) != 0))
#else
    if ((buffer[0] != 'H')||(buffer[1] != 'T')||(buffer[2] != 'T')||
        (buffer[3] != 'P')||(buffer[4] != '/')||(buffer[5] != '1')||
        (buffer[6] != '.')||((buffer[7] != '1')&&(buffer[7] != '0'))||
        (buffer[8] != ' ')||(buffer[9] != '2')||(buffer[10] != '0')||
        ((buffer[11] != '0')&&(buffer[11] != '6')))
#endif
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
#ifdef USE_ZLIB
        // �T�[�o���X�|���X�ُ�
        if ((NNshGlobal->useGzip != 0)&&(NNshGlobal->loginDateTime != 0)&&
            (NNshGlobal->getRetry == 0))
        {
            NNsh_InformMessage(ALTID_WARN, MSG_RETRY_OYSTERLOG, buffer, 0);
        }
        else
#endif // #ifdef USE_ZLIB
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_RECV_NOTACCEPTED, buffer, 0);
        }
        ret = (NNSH_ERRORCODE_NOTACCEPTEDMSG);
        goto END_FUNC;
    }
    ptr = ptr + 4;
    while ((*ptr == '\x0a')||(*ptr == '\x0d'))  // �ߋ����O�p�ɕ␳...
    {
        ptr++;
    }
    *dataSize = fileSize - (ptr - buffer);

    switch (bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
#if 0
        //  �܂�BBS�̂Ƃ��ɂ́A���b�Z�[�W�̐擪�܂Ńf�[�^��
        // �ǂݔ�΂����s��
        bakPtr = ptr;
        ptr = StrStr(ptr, NNSH_MACHITOKEN_START);
        if (ptr == NULL)
        {
            ptr = bakPtr;
        }
#endif
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
#if 0
        bakPtr = ptr;
        ptr = StrStr(ptr, NNSH_JBBSTOKEN_START);
        if (ptr == NULL)
        {
            ptr = bakPtr;
        }
#endif
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        bakPtr = ptr;
        ptr = StrStr(ptr, NNSH_JBBSTOKEN_RAW_START);
        if (ptr == NULL)
        {
            ptr = bakPtr;
        }
        else
        {
            // ptr = ptr + StrLen(NNSH_JBBSTOKEN_RAW_START);
            // �f�[�^�̐擪��<>�ɒu��������B
            *ptr       = '<';
            *(ptr + 1) = '>';
        }

        // [MEMO]
        // ��M�����t�@�C���̃T�C�Y�����炷�H(������7�o�C�g���������Ă���͗l...)
        //
        break;

#if 0
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_SHITARABA:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // �Ȃɂ����Ȃ�
        break;
    }

    // ��f�[�^�������݂̃`�F�b�N(�ނ�������ɒ�������)
    if (((UInt32) ptr & 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
    {
        ptr--;
        *ptr = ' ';
    }

    chkSize = readSize - (ptr - buffer);
    if (((chkSize % 2) == 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
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

    // ���Ƀt�@�C�������݂����ꍇ�͈�U�폜���Ă���I�[�v������B
    (void) DeleteFile_NNsh(targetFile, NNSH_VFS_ENABLE);
    ret = OpenFile_NNsh(targetFile,
                        NNSH_FILEMODE_APPEND, &fileRefW);
    if (ret != errNone)
    {
        // �������݃t�@�C���̃I�[�v���Ɏ��s����
        NNsh_InformMessage(ALTID_ERROR,
                           MSG_WRITEFILE_OPENFAIL, targetFile, ret);
        deleteLogFile = false;
        *dataSize = 0;
        goto END_FUNC;
    }

    // �f�[�^�̏�������(�ǋL)
    ret = AppendFile_NNsh(&fileRefW, chkSize, ptr ,&writeSize);
    if (ret != errNone)
    {
        CloseFile_NNsh(&fileRefW);
        NNsh_InformMessage(ALTID_ERROR, "WriteFile_NNsh(1st)", "", ret);
        deleteLogFile = true;
        *dataSize = 0;
        ret = ~errNone + 11;
        goto END_FUNC;
    }
    offset = readSize;

    // �f�[�^���Ō�ɂȂ�܂ŒǋL����
    while (offset < *dataSize)
    {
        MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize + MARGIN, 0x00);
        readSize = 0;
        ret = ReadFile_NNsh(&fileRefR, offset, (NNshGlobal->NNsiParam)->bufferSize,
                            buffer, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh()", "", ret);
            deleteLogFile = true;
            *dataSize = 0;
            break;
        }
        ret = errNone;
        chkSize = 0;
        if (((readSize % 2) == 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
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
            deleteLogFile = true;
            *dataSize = 0;
            break;
        }
        offset = offset + readSize;
    }
    CloseFile_NNsh(&fileRefW);

END_FUNC:
    CloseFile_NNsh(&fileRefR);
    MEMFREE_PTR(realBuf);
    if (deleteLogFile != false)
    {
        // ���r���[�ȃ��O�t�@�C�����c���Ă���Ƃ��ɂ́A�t�@�C�����폜����B
        (void) DeleteFile_NNsh(targetFile, NNSH_VFS_ENABLE);
    }
    Hide_BusyForm(false);

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
    Char        *buffer, *ptr, *realBuf, tmpBuf[TINYBUF + MARGIN], *tmpPtr;
    NNshFileRef  fileRefR, fileRefW;

    // �f�[�^�o�^���\��
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :",FILE_RECVMSG, ret);
    }

    // �ꎞ�o�b�t�@�̈���m��
    realBuf = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR()", " size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        ret = errNone - 10;
        goto END_FUNC;
    }
    buffer = realBuf;

    // ��M���b�Z�[�W�̃T�C�Y���擾(�w�b�_����)
    GetFileSize_NNsh(&fileRefR, &fileSize);

    // �󂫗e�ʂ̊m�F(VFS�̂�)
    if (CheckFileSpace_NNsh(NNSH_DISABLE, fileSize) != errNone)
    {
        // �󂫗e�ʕs���A�I������B
        ret = ~errNone;
        goto END_FUNC;
    }

    parseSize = 0;
    readSize  = 0;
    offset    = 0;

    // ����f�[�^�̓ǂݍ���
    MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize + MARGIN, 0x00);
    ret = ReadFile_NNsh(&fileRefR, offset, (NNshGlobal->NNsiParam)->bufferSize,
                        buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh()", "", ret);

        goto END_FUNC;
    }

    // �����_�E�����[�h���́A�t�@�C���̍Ō���׍H����(������Ǝ��Ԃ����邯��)
    if ((NNshGlobal->NNsiParam)->enablePartGet != 0)
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
    ptr = StrStr(buffer, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // �f�[�^�ُ�
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(" NNSH_HTTP_HEADER_ENDTOKEN ")", 0);
        goto END_FUNC;
    }
    parseSize = ptr - buffer;
    ptr       = ptr + 4;
    parseSize = parseSize + 4;

    // �������@JBBS(rawmode)�ȊO�́A�����`�F�b�N������
    if (mesDB->bbsType != NNSH_BBSTYPE_SHITARABAJBBS_RAW)
    {
        // HTTP�̉����R�[�h�`�F�b�N (����𐔎������݂̂ɂ���)
        // StrLen("HTTP/1.1 200") ==> 12 �ɒu��(�������̂���)
        // if ((StrNCompare(buffer,"HTTP/1.1 200 OK", 12) == 0)||
        //    (StrNCompare(buffer,"HTTP/1.0 200 OK", 12) == 0))
        if ((buffer[9] == '2')&&(buffer[10] == '0')&&(buffer[11] == '0'))
        {
            // �Ȃ񂩒m��񂯂ǁA�S���b�Z�[�W���ł���(���ȁH)�B
            CloseFile_NNsh(&fileRefR);
            MEMFREE_PTR(realBuf);
            Hide_BusyForm(false);

            // ���b�Z�[�W���ׂĂ����ł������Ƃ�DEBUG�\��
            NNsh_DebugMessage(ALTID_INFO, "Received ALL MESSAGE!! ", targetFile, 0);

            // ���ׂẴf�[�^����M���鏈���ɐ؂�ւ���
            ret = errNone;
            ret = message_entryMessage(targetFile, &fileSize, NNSH_BBSTYPE_2ch);

            mesDB->state      = NNSH_SUBJSTATUS_NEW;
            mesDB->fileSize   = fileSize;

            // �X���̋L�^���o�^
            if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
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

        // �����擾�����f�[�^�ŁA�{�f�B�����ł��Ȃ������ꍇ...
        if ((parseSize + 1 == readSize)&&(*ptr == '\x0a'))
        {
            // �X���X�V�Ȃ�
            CloseFile_NNsh(&fileRefR);
            MEMFREE_PTR(realBuf);
            Hide_BusyForm(false);
            return (NNSH_MSGSTATE_NOTMODIFIED);
        }
        if (*ptr != '\x0a')
        {
            // �f�[�^�G���[�H(�G���[�̏ڍׂ͖��`�F�b�N)
            CloseFile_NNsh(&fileRefR);
            MEMFREE_PTR(realBuf);
            Hide_BusyForm(false);

            return (NNSH_MSGSTATE_ERROR);
        }
        // �X�V�`�F�b�N�p�̕���(1�o�C�g)�������߂�B
        ptr++;
        parseSize++;
    }
    else
    {
        // �������@JBBS(rawmode)�̍����擾
        MemSet (tmpBuf, sizeof(tmpBuf), 0x00);
        NUMCATI(tmpBuf, (mesDB->maxLoc + 1));
        StrCat (tmpBuf, "<>");
        tmpPtr = StrStr(ptr, tmpBuf);
        if (tmpPtr == NULL)
        {
            // �X���X�V�Ȃ�
            CloseFile_NNsh(&fileRefR);
            MEMFREE_PTR(realBuf);
            Hide_BusyForm(false);
            return (NNSH_MSGSTATE_NOTMODIFIED);
        }

        // �f�[�^�𒲐�
#if 0
        *(tmpPtr - 3) = '<';
        *(tmpPtr - 2) = '>';
        *(tmpPtr - 1) = '\x0a';
        *(tmpPtr - 3) = '<';
        *(tmpPtr - 2) = '>';
        *(tmpPtr - 1) = '\x0a';
#endif
        parseSize = parseSize + (tmpPtr - 3) - ptr;
    }

    // �w�b�_������������t�@�C���T�C�Y�ɕύX����
    fileSize = fileSize - (ptr - buffer);

    if (((UInt32) ptr & 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
    {
        ptr--;
        *ptr = ' ';
    }
     // �f�[�^�̏�������(�ǋL����)
    chkSize  = readSize - (ptr - buffer);
    if (((chkSize % 2) == 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
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

    // �������݃t�@�C���I�[�v��
    ret = OpenFile_NNsh(targetFile, NNSH_FILEMODE_APPEND, &fileRefW);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "ERR>OpenFile_NNsh", targetFile, ret);
        ret = errNone;
        goto END_FUNC;
    }

    ret = AppendFile_NNsh(&fileRefW, chkSize, ptr, &writeSize);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "WRITEFile_NNsh(1st)", "", ret);
        ret = errNone;
        goto END_FUNC;
    }
    offset = readSize;

    while (offset < fileSize)
    {
        MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize + MARGIN, 0x00);
        readSize = 0;
        ret = ReadFile_NNsh(&fileRefR, offset, (NNshGlobal->NNsiParam)->bufferSize,
                            buffer, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            NNsh_InformMessage(ALTID_ERROR, "ERR>ReadFile_NNsh", "", ret);
            break;
        }
        chkSize = 0;
        if (((readSize % 2) == 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
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
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
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
    CloseFile_NNsh(&fileRefW);
    ret = errNone;

END_FUNC:    
    CloseFile_NNsh(&fileRefR);
    MEMFREE_PTR(realBuf);
    Hide_BusyForm(false);

    return (ret);
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
    if ((NNshGlobal->NNsiParam)->enablePartGet != 0)
    {
        range = (UInt32) (NNshGlobal->NNsiParam)->partGetSize - NNSH_HTTPGET_HEADERMAX;
    }
    else
    {
        range = HTTP_RANGE_OMIT;
    }

    // ���b�Z�[�W����M
    ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                        HTTP_GETSUBJECT_START, range, NULL);
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
    ret = message_entryMessage (fileName, &fileSize, bbsType);
    if (ret == errNone)
    {
        // ��M�G���[�łȂ������Ƃ��ɂ́A�擾�������f�[�^�x�[�X�ɓo�^
        message_entryDatabase(idxMES, fileSize, bbsType);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   Get_PartMessage                                          */
/*                                                      ���b�Z�[�W�����擾 */
/*=========================================================================*/
Err Get_PartMessage(Char *url, Char *boardNick, NNshSubjectDatabase *mesDB, 
                    UInt16 idxMES, Char *message)
{
    Err    ret, ret1;
    Char   fileName[MAXLENGTH_FILENAME];
    UInt32 range;

    // ���b�Z�[�W���i�[����t�@�C�������쐬
    MemSet (fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, boardNick);
    StrCat (fileName, mesDB->threadFileName);

    // �����_�E�����[�h�w�莞�ɂ̓����W�ݒ���s��
    if ((NNshGlobal->NNsiParam)->enablePartGet != 0)
    {
        range = (UInt32) (NNshGlobal->NNsiParam)->partGetSize - 
                         NNSH_HTTPGET_HEADERMAX + (mesDB->fileSize - 1);
    }
    else
    {
        range = HTTP_RANGE_OMIT;
    }

    // http�ŁA���X�������擾
    if (mesDB->bbsType != NNSH_BBSTYPE_SHITARABAJBBS_RAW)
    {
        // ���b�Z�[�W�𑗎�M(�������@JBBS�ȊO�̂Ƃ�)
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                            (mesDB->fileSize - 1), range, message);
    }
    else
    {
        // �������@JBBS�̂Ƃ�... rawmode �Ȃ̂ŁAURL�Ɏ擾���X�ԍ���ǉ�
        NUMCATI(url, (mesDB->maxLoc + 1));
        StrCat (url, "-");
        NNsh_DebugMessage(ALTID_INFO, " SHITARABA URL:", url, 0);

        // ���b�Z�[�W�𑗎�M
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                            HTTP_RANGE_OMIT, range, message);
    }

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
    ret1 = message_appendMessage(fileName, mesDB);
    switch (ret1)
    {
      case errNone:
        // �f�[�^�ϊ������A���ւ����ށB
        // �������A������΁�JBBS(rawmode)�̂Ƃ��́A�������X�ԍ����擾����
        if (mesDB->bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)
        {
            // �����̃��X�ԍ����擾����
            (void) getLastMessageNumShitaraba(&(mesDB->maxLoc));
        }
        break;

      case NNSH_MSGSTATE_NOTMODIFIED:
        // �f�[�^���X�V
        NNsh_InformMessage(ALTID_INFO, 
                           MSG_RECV_NOTMODIFY, mesDB->threadTitle, 0);
        goto FUNC_END;
        break;

      case NNSH_MSGSTATE_ERROR:
        // �f�[�^��M���s
        NNsh_InformMessage(ALTID_ERROR, 
                          MSG_RECV_PARTMSG_ERROR, mesDB->threadTitle, ret);
        // mesDB->state        = NNSH_SUBJSTATUS_UNKNOWN;

        // �X���ɃG���[����ݒ肷��(���擾�\��X�������N���A����)
        mesDB->msgAttribute = (mesDB->msgAttribute | NNSH_MSGATTR_ERROR);
        mesDB->msgAttribute = (mesDB->msgAttribute & NNSH_MSGATTR_NOTRESERVE);
        (void) update_subject_database(idxMES, mesDB);
        return (NNSH_ERRORCODE_PARTGET);
        break;

      default:
        break;
    }
FUNC_END:
    // �X���̃G���[�����N���A����
    mesDB->msgAttribute = ((mesDB->msgAttribute)&
                           (NNSH_MSGATTR_NOTERROR)&(NNSH_MSGATTR_NOTRESERVE));

    // �X�����b�Z�[�W�f�[�^�x�[�X�̓��e���X�V����B
    ret = update_subject_database(idxMES, mesDB);
    if (ret != errNone)
    {
        // ���肦�Ȃ��͂�...
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()"," ", ret);
    }
    return (ret1);
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
void MarkMessageIndex(UInt16 index, UInt16 favor, UInt16 ngWordCheck)
{
    Err                  ret;
    NNshSubjectDatabase  subjDB;

    ret = Get_Subject_Database(index, &subjDB);
    if (ret == errNone)
    {
        if ((NNshGlobal->msgIndex) != NULL)
        {
            subjDB.currentLoc = (NNshGlobal->msgIndex)->openMsg;
            subjDB.maxLoc     = (NNshGlobal->msgIndex)->nofMsg;    
            if ((NNshGlobal->msgIndex)->nofMsg == 
                                             (NNshGlobal->msgIndex)->openMsg)
            {
                subjDB.state = NNSH_SUBJSTATUS_ALREADY;
            }
            else
            {
                subjDB.state = NNSH_SUBJSTATUS_REMAIN;
            }

            // MSG�̏ꏊ���s���������ꍇ�ɂ́A�L�^����L������B
            if ((subjDB.msgState == 0)&&
                (subjDB.maxLoc != 0)&&(subjDB.currentLoc != 0))
            {
                if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
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
            // �w�肳�ꂽ���C�ɓ���ݒ�����s����
            subjDB.msgAttribute 
                  = ((subjDB.msgAttribute & NNSH_MSGATTR_NOTFAVOR)| favor);
            break;
        }

        // �X������NG�ݒ���g��/�g��Ȃ����w�肷��ꍇ...
        if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD_EACHTHREAD) == NNSH_USE_NGWORD_EACHTHREAD)
        {
            if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
            {
                // NG�ݒ�-3�̐ݒ��ON�ɂ���B�B�B
                subjDB.msgAttribute  = (subjDB.msgAttribute | NNSH_MSGATTR_NGWORDSET);

            }
            else
            {
                // NG�ݒ�-3�̐ݒ���N���A����B�B�B
                subjDB.msgAttribute  = (subjDB.msgAttribute & NNSH_MSGATTR_CLRNGWORD);
            }
        }
        (void) update_subject_database(index, &subjDB);
    }
    return;
}

/*=========================================================================*/
/*   Function :   GetThreadDataFromMesNum                                  */
/*                                            �X���ԍ��w��̃��b�Z�[�W�擾 */
/*=========================================================================*/
Boolean GetThreadDataFromMesNum(Boolean *updateBBS, Char *buf, UInt16 bufLen, UInt16 *recNum)
{
    Boolean              rep = false;
    Err                  ret;
    UInt16               btnId, index, dataIndex;
    UInt32               size;
    DmOpenRef            dbRef;
    Char                *ptr, *nPtr;
    NNshBoardDatabase    bbsData;
    NNshSubjectDatabase  subjDb;

    // �̈�̏�����
    index = 0;
    MemSet(&subjDb,  sizeof(NNshSubjectDatabase), 0x00);
    MemSet(&bbsData, sizeof(NNshBoardDatabase),   0x00);
    *updateBBS = false;
    *recNum = 0;

    // ���̓f�[�^�����Nick��؂�o��
    ptr  = buf;
    nPtr = subjDb.boardNick;
    while (*ptr != '/')
    { 
        *nPtr = *ptr;
        ptr++;
        nPtr++;
        if (*ptr == '\0')
        {
            // �f�[�^�擾�G���[�A�I������
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_GETFAILURE, "", 0);
            return (false);
        }
    }
    *nPtr = '/';    // ��Nick�̍Ō�� '/'��t�����Ă���

    // �X���ԍ���؂�o��
    ptr++;
    nPtr = subjDb.threadFileName;
    while ((*ptr != '/')&&(*ptr != '\0'))
    {
        *nPtr = *ptr;
        nPtr++;
        ptr++;
    }

    // BBS�ꗗ�ǂݍ��ݒ��̃��b�Z�[�W��\������
    Show_BusyForm(MSG_READ_BBSLIST_WAIT);

    // ���̎擾
    ret = Get_BBS_Database(subjDb.boardNick, &bbsData, &index);
    if (ret != errNone)
    {
        // ���擾�Ɏ��s�A�G���[�\�����ďI��
        NNsh_InformMessage(ALTID_ERROR, subjDb.boardNick,
                          MSG_WRONG_BOARDNICK, ret);
        Hide_BusyForm(false);
        return (false);
    }

    // ���b�Z�[�W�̋L��
    MemSet (buf, bufLen, 0x00);
    StrCopy(buf, bbsData.boardName);
    StrCat (buf, MSG_CONFIRM_GETTHREAD);
    StrCat (buf, subjDb.threadFileName);
    StrCat (buf, ")");

    // ���b�Z�[�W�̍폜
    Hide_BusyForm(false);

    // ���b�Z�[�W�擾��(URL)�̍쐬
    switch (bbsData.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // �܂�BBS����f�[�^�擾(HTML��ǂ�)
        StrCat (subjDb.threadFileName, ".cgi");

        //////////////////////////////////////////////////////////////////////
        // ���݁A�܂��a�a�r�ɂ͂��̋@�\�͎g�p�ł��Ȃ��|��\�����ďI������B
        NNsh_InformMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI,"",0);
        return (false);
        //////////////////////////////////////////////////////////////////////
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // ������΁�JBBS����f�[�^�擾(HTML��ǂ�)
        StrCat (subjDb.threadFileName, ".cgi");   // �g���q������

        //////////////////////////////////////////////////////////////////////
        // ���݁A���̋@�\�͎g�p�ł��Ȃ��|��\�����ďI������B
        NNsh_InformMessage(ALTID_WARN, MSG_NOTSUPPORT_BBS,"",0);
        return (false);
        //////////////////////////////////////////////////////////////////////
        break;

#if 0
      case NNSH_BBSTYPE_SHITARABA:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // 2ch����f�[�^�擾(dat��ǂ�)
        StrCat (subjDb.threadFileName, ".dat");   // �g���q������
        break;
    }

    // �{���ɃX�����擾���邩�m�F���s��
    btnId = NNsh_ConfirmMessage(ALTID_CONFIRM, buf, "", 0);
    if (btnId != 0)
    {
        // �擾���L�����Z������
        NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
        return (false);
    }

    // �f�[�^�擾��URL�𐶐�����
    CreateThreadURL(NNSH_DISABLE, buf, bufLen, &bbsData, &subjDb);

    // �X���f�[�^���擾�ς݂��m�F����B
    if (Check_same_thread(NNSH_NOTENTRY_THREAD, &subjDb, &subjDb, &dataIndex)
                                                                    == errNone)
    {
        // �擾�ς݂Ɣ����A�����擾�ɐ؂�ւ���
        ret = Get_PartMessage(buf,bbsData.boardNick,&subjDb,dataIndex,NULL);

        // �擾�I���̕\��(DEBUG�I�v�V�����L�����̂�)
        NNsh_InformMessage(ALTID_INFO, MSG_GET_PARTMESSAGE_END,
                           subjDb.threadTitle, ret);

        // �擾�����X���̃��R�[�h�ԍ����i�[����
        *recNum = dataIndex;
        rep = (ret == errNone) ? true : false;
        return (rep);
    }

    ////////////  ���b�Z�[�W�S���擾�̏���  ///////////

    // �X�����擾���̃��b�Z�[�W��\������
    Show_BusyForm(MSG_MESSAGE_INFO_WAIT);

    /////////////////////////////////////////////////////////////////////////
    // �f�[�^�x�[�X�ɃX���f�[�^��(��)�ǉ����� ���X���^�C�g���͌�Œǉ����遦
    /////////////////////////////////////////////////////////////////////////
    StrCopy(subjDb.threadTitle, "(Unknown)");

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // ���b�Z�[�W�̍폜
        Hide_BusyForm(false);

        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh()", "", 0);
        return (false);
    }
    EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &subjDb);
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    /** (VFS���)�f�B���N�g�����쐬���� **/
    (void) CreateDir_NNsh(bbsData.boardNick);
    
    // ���b�Z�[�W�̍폜
    Hide_BusyForm(false);

    // ���O�擾����ыL�^����
#ifdef USE_ZLIB
    NNshGlobal->getRetry = 0;
GET_ALLMESSAGE:
    ret = Get_AllMessage(buf, bbsData.boardNick, subjDb.threadFileName,
                         0, (UInt16) bbsData.bbsType);

    if ((NNshGlobal->useGzip != 0)&&(NNshGlobal->loginDateTime != 0)&&
         (ret == NNSH_ERRORCODE_NOTACCEPTEDMSG)&&
         (NNshGlobal->getRetry == 0))
    {
        //  �����O�C�����ŁA�T�[�o����̉������b�Z�[�W��
        // �ُ킾�����Ƃ��́A�ߋ����O�q�ɂ�������
        MemSet(buf, bufLen, 0x00);

        // ���b�Z�[�W�擾��URL�̍쐬
        (void) CreateThreadURL(NNSH_ENABLE, buf, bufLen, &bbsData, &subjDb);
        NNshGlobal->getRetry = 1;
        goto GET_ALLMESSAGE;
    }
    else if (ret != errNone)
#else // #ifdef USE_ZLIB
    ret = Get_AllMessage(buf, bbsData.boardNick, subjDb.threadFileName,
                         0, (UInt16) bbsData.bbsType);
    if (ret != errNone)
#endif // #ifdef USE_ZLIB
    {
        // �X���t�@�C������ �ϐ�buf�ɏ����o���A(�����O��)�X���t�@�C�����폜
        MemSet (buf, bufLen, 0x00);
        StrCopy(buf, bbsData.boardNick);
        StrCat (buf, subjDb.threadFileName);
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
        {
            // Palm�{�̏�̃��O���폜����
            ret = DeleteFile_NNsh(buf, NNSH_VFS_DISABLE);
        }
        else
        {
            // VFS��̃��O���폜����
            ret = DeleteFile_NNsh(buf, NNSH_VFS_ENABLE);
        }

        // �G���[����(���ǉ������f�[�^�x�[�X�f�[�^���폜����)
        (void) OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
        DeleteRecordIDX_NNsh(dbRef, 0);
        CloseDatabase_NNsh(dbRef);
        NNsh_DebugMessage(ALTID_ERROR, "Get_AllMessage()", "", ret);
        return (false);
    }
#ifdef USE_ZLIB
    NNshGlobal->getRetry = 0;
#endif // #ifdef USE_ZLIB

    //////////////////�X���^�C�g���擾����/////////////////////////

    // �X�����擾���̃��b�Z�[�W��\������
    Show_BusyForm(MSG_MESSAGE_INFO_WAIT);

    // �X�������擾����
    (void) Get_Subject_Database(0, &subjDb);

    // �t�@�C���T�C�Y���ꎞ�I�ɋL��
    size = subjDb.fileSize;

    // �X���t�@�C������ �ϐ�buf�ɏ����o��
    MemSet (buf, bufLen, 0x00);
    StrCopy(buf, bbsData.boardNick);
    StrCat (buf, subjDb.threadFileName);

    // BBS���ɐݒ肳�ꂽ�W���X�����x���𔽉f������
    switch ((bbsData.state)&(NNSH_BBSSTATE_LEVELMASK))
    {
      case NNSH_BBSSTATE_FAVOR_L1:
        // L���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L1));
        break;

      case NNSH_BBSSTATE_FAVOR_L2:
        // 1���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L2));
        break;

      case NNSH_BBSSTATE_FAVOR_L3:
        // 2���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L3));
        break;

      case NNSH_BBSSTATE_FAVOR_L4:
        // 3���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L4));
        break;

      case NNSH_BBSSTATE_FAVOR:
        // H���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR));
        break;

      default:
        // �������Ȃ�
        break;
    }

    // �t�@�C������X�������擾����
    ret = SetThreadInformation(buf, (NNSH_FILEMODE_READONLY), NULL, &subjDb);
    if (ret != errNone)
    {
        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR, "SetThreadInformation()\nFileName:",
                          subjDb.threadFileName, ret);
        // ���^�[�������ɏ������p������
    }

    // �t�@�C���T�C�Y�������߂�
    subjDb.fileSize = size;

    // ���R�[�h�̓o�^
    ret = update_subject_database(0, &subjDb);

    // ���b�Z�[�W�̍폜
    Hide_BusyForm(false);

    if (ret != errNone)
    {
        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()", "", ret);
        return (false);
    }

    // �擾�I���̕\��
    NNsh_InformMessage(ALTID_INFO,MSG_GET_MESSAGE_END,subjDb.threadTitle,ret);

#ifdef UPDATE_BBS_AUTOMATICALLY
    // BBS���g�p����ݒ�ɂȂ��Ă��Ȃ������ꍇ...
    if ((bbsData.state & NNSH_BBSSTATE_VISIBLE) != NNSH_BBSSTATE_VISIBLE)
    {
        // BBS���g�p����ݒ�ɂ��čX�V����
        bbsData.state = ((NNSH_BBSSTATE_LEVELMASK)&(bbsData.state))|(NNSH_BBSSTATE_VISIBLE);

        // BBS���̍X�V
        Update_BBS_Database(index, &bbsData);

        *updateBBS = true;

        // �g�pBBS�ꗗ���ǉ����ꂽ���Ƃ�ʒm
        NNsh_InformMessage(ALTID_WARN,bbsData.boardName,MSG_WARN_BBSUPDATE,0);

        // BBS�^�C�g���C���f�b�N�X�p�̗̈����U�������
        if (NNshGlobal->boardIdxH != 0)
        {
            MemHandleUnlock(NNshGlobal->boardIdxH);
            MemHandleFree(NNshGlobal->boardIdxH);
            NNshGlobal->boardIdxH = 0;
        }
        MEMFREE_PTR((NNshGlobal->bbsTitles));
        NNshGlobal->bbsTitles = 0; // �O�̂���...
    }
#endif  // #ifdef UPDATE_BBS_AUTOMATICALLY
    return (true);
}


/*=========================================================================*/
/*   Function :   Hander_MessageInfo                                       */
/*                                 ���b�Z�[�W��񑀍쎞�̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_MessageInfo(EventType *event)
{
    FormType *frm;
    UInt16    itemId, status;

    // �C�x���g��؂�ւ���
    itemId = 0;
    switch (event->eType)
    { 
      case menuEvent:
        break;

      case keyDownEvent:
        // �L�[���͂�������
        itemId = KeyConvertFiveWayToJogChara(event);
        break;

      case ctlSelectEvent:
        break;

      default: 
        break;
    }

    status = 1;
    frm = FrmGetActiveForm();
    switch (itemId)
    {
      case chrDigitOne:
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitTwo:
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitThree:
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitFour:
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitFive:
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitSix:
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        break;

      case chrCapital_W:
      case chrSmall_W:
        // �u���E�U�ŊJ��
        CtlHitControl(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_OPEN_BROWSER)));
        break;

      case chrCapital_V:
      case chrSmall_V:
        // webDA�ŊJ��
        CtlHitControl(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_OPEN_WEBDA)));
        break;

      default:
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function : DisplayMessageInformations                                 */
/*                                                     �X������\������  */
/*=========================================================================*/
Err DisplayMessageInformations(UInt16 msgIndex, UInt16 *msgAttribute)
{
    Err                 ret;
    Char                logBuf[BIGBUF], *urlTop;
    UInt16              index, btnId, status;
    FormType           *prevFrm, *diagFrm;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���b�Z�[�W�����f�[�^�x�[�X����擾
    ret = Get_Subject_Database(msgIndex, &mesInfo);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()", "", ret);
        return (ret);
    }

    // BBS�����f�[�^�x�[�X����擾
    ret = Get_BBS_Database(mesInfo.boardNick, &bbsInfo, &index);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          mesInfo.boardNick, ret);
        return (ret);
    }
    // index�ԍ���work1�Ɋi�[����
    NNshGlobal->work1 = index;

    /////////////////// �X�����b�Z�[�W�����\�z����  ///////////////////////

    // ���݂̉�ʃ|�C���^���擾
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // �X������ʂ��J��
    diagFrm  = FrmInitForm(FRMID_MESSAGEINFO);
    if (diagFrm != NULL)
    {
        // �t�H�[�����A�N�e�B�u�ɂ���
        FrmSetActiveForm(diagFrm);

        // PalmOS5.3DIA������SILK�����錾����
        if (NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)
        {
            FrmSetDIAPolicyAttr(diagFrm, frmDIAPolicyCustom);
            PINSetInputTriggerState(pinInputTriggerEnabled);

            // ���炢�x�^�A�A�A�E�B���h�E�T�C�Y�̍ő�l�ŏ��l��ݒ�
            WinSetConstraintsSize(FrmGetWindowHandle(diagFrm),
                                  160, 225, 225, 160, 225, 225);
            // �V���N�����L���ɂ���
            FrmSetDIAPolicyAttr(diagFrm, frmDIAPolicyCustom);

            // �V���N�����L���ɂ���
            // PINSetInputTriggerState(pinInputTriggerEnabled);

            // �V���N��Ԃ��X�V����
            // PINSetInputAreaState(current);
            PINSetInputAreaState(pinInputAreaUser);
        }

        // �C�x���g�n���h���̐ݒ�
        FrmSetEventHandler(diagFrm, Handler_MessageInfo);

        // ���b�Z�[�W���̐���
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCat (logBuf, NNSH_MSGINFO_MSGNUM);
        NUMCATI(logBuf, mesInfo.currentLoc);
        StrCat (logBuf, "/");
        NUMCATI(logBuf, mesInfo.maxLoc);

        // ���b�Z�[�W���̕\��
        NNshWinSetFieldText(diagFrm,FLDID_MSGSTATUS,false,logBuf,StrLen(logBuf));

        // �t�@�C�����̍쐬
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, NNSH_MSGINFO_FILELOC);

        // ���b�Z�[�W�̋L�^�ӏ����o�͂���
        switch (mesInfo.msgState)
        {
          case FILEMGR_STATE_OPENED_VFS:
            StrCat(logBuf, "VFS (");
            break;
          case FILEMGR_STATE_OPENED_STREAM:
            StrCat(logBuf, "Palm (");
            break;
          default:
            StrCat(logBuf, "??? (");
            break;
        }
        NUMCATI(logBuf, mesInfo.fileSize);
        StrCat (logBuf, " bytes)");

        // �t�@�C�����̕\��
        NNshWinSetFieldText(diagFrm,FLDID_FILESTATUS,false,logBuf,StrLen(logBuf));

        // ���̑����̍쐬
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, NNSH_MSGINFO_STATUS);

        // �擾��~���a�a�r���ǂ����̕\��
        if (((bbsInfo.state) & (NNSH_BBSSTATE_SUSPEND)) == NNSH_BBSSTATE_SUSPEND)
        {
            StrCat(logBuf, NNSH_MSGINFO_SUSPEND);
        }

        // NG�ݒ�-3�̐ݒ��
        if ((mesInfo.msgAttribute & NNSH_MSGATTR_NGWORDSET) == NNSH_MSGATTR_NGWORDSET)
        {
            StrCat(logBuf, NNSH_MSGINFO_NG_3); 
        }

        // �X����Ԃ̕\��
        switch(mesInfo.state)
        {
          case NNSH_SUBJSTATUS_NOT_YET:
            // ���擾
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_NOT_YET);
            break;
          case NNSH_SUBJSTATUS_NEW:
            // �V�K�擾
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_NEW);
            break;
          case NNSH_SUBJSTATUS_UPDATE:
            // �X�V�擾
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_UPDATE);
            break;
          case NNSH_SUBJSTATUS_REMAIN:
            // ���ǂ���
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_REMAIN);
            break;
          case NNSH_SUBJSTATUS_ALREADY:
            // ����
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_ALREADY);
            break;
          case NNSH_SUBJSTATUS_UNKNOWN:
          default:
            // ��ԕs��
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_UNKNOWN);
            break;
        }
        if ((mesInfo.msgAttribute & NNSH_MSGATTR_ERROR) != 0)
        {
            // ��M�G���[������\��
            StrCat(logBuf, NNSH_MSGINFO_RECVERR);
        }
        if ((mesInfo.msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
        {
            // �擾�\���\��
            StrCat(logBuf, NNSH_MSGINFO_RECVRESERVE);
        }

        // ���̑����(���b�Z�[�W�擾���)�̕\��
        NNshWinSetFieldText(diagFrm, FLDID_ETCSTATUS,
                            false, logBuf, StrLen(logBuf));

        // �X���^�C�g���̐ݒ�
        // (���̃^�C�g�����ɋL������URL���ė��p�����邽�߁A�\�����Ԃ�ύX����)
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, mesInfo.threadTitle);
        StrCat (logBuf, "\n");

        // �X��URL�̐擪�|�C���^���L������
        urlTop = &logBuf[StrLen(logBuf)];

        // �X����URL����
        CreateThreadBrowserURL(NNSH_DISABLE, urlTop, &bbsInfo, &mesInfo);

        // �X��URL�̐ݒ�
        NNshWinSetFieldText(diagFrm,FLDID_MSGURL,true,logBuf,StrLen(logBuf));

        // ���݂̃X�����C�ɓ��背�x����ݒ�
        status = 1;
        switch(mesInfo.msgAttribute & NNSH_MSGATTR_FAVOR)
        {
          case NNSH_MSGATTR_FAVOR_L1:
            // ���C�ɓ��背�x��(��)
            SetControlValue(diagFrm, PBTNID_FAVOR_L1, &status);
            break;

          case NNSH_MSGATTR_FAVOR_L2:
            // ���C�ɓ��背�x��(����)
            SetControlValue(diagFrm, PBTNID_FAVOR_L2, &status);
            break;

          case NNSH_MSGATTR_FAVOR_L3:
            // ���C�ɓ��背�x��(��)
            SetControlValue(diagFrm, PBTNID_FAVOR_L3, &status);
            break;

          case NNSH_MSGATTR_FAVOR_L4:
            // ���C�ɓ��背�x��(����)
            SetControlValue(diagFrm, PBTNID_FAVOR_L4, &status);
            break;

          case NNSH_MSGATTR_FAVOR:
            // ���C�ɓ��背�x��(��)
            SetControlValue(diagFrm, PBTNID_FAVOR_L5, &status);
            break;

          default:
            // ���C�ɓ���ł͂Ȃ�
            SetControlValue(diagFrm, PBTNID_FAVOR_L0, &status);
            break;
        }

        // �_�C�A���O���I�[�v������
        btnId = FrmDoDialog(diagFrm);

        // �X�����x���̐ݒ茋�ʊm�F
        status = 0;
        UpdateParameter(diagFrm, PBTNID_FAVOR_L0, &status);
        if (status != 0)
        {
            // ���C�ɓ���ݒ肳��Ă��Ȃ�
            *msgAttribute = 0;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L1, &status);
        if (status != 0)
        {
            // ���C�ɓ���ݒ�(��)
            *msgAttribute = NNSH_MSGATTR_FAVOR_L1;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L2, &status);
        if (status != 0)
        {
            // ���C�ɓ���ݒ�(����)
            *msgAttribute = NNSH_MSGATTR_FAVOR_L2;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L3, &status);
        if (status != 0)
        {
            // ���C�ɓ���ݒ�(��)
            *msgAttribute = NNSH_MSGATTR_FAVOR_L3;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L4, &status);
        if (status != 0)
        {
            // ���C�ɓ���ݒ�(����)
            *msgAttribute = NNSH_MSGATTR_FAVOR_L4;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L5, &status);
        if (status != 0)
        {
            // ���C�ɓ���ݒ�(��)
            *msgAttribute = NNSH_MSGATTR_FAVOR;
            goto DELETE_DIALOG;
        }

DELETE_DIALOG:
        // �_�C�A���O�����
        FrmSetActiveForm(prevFrm);
        FrmDeleteForm(diagFrm);

        if ((btnId == BTNID_OPEN_BROWSER)||(btnId == BTNID_OPEN_WEBDA))
        {
            // �u�u���E�U�ŊJ���v�{�^���������ꂽ�Ƃ�

            // ���C�ɓ���ݒ�ɂ���(�f�[�^�x�[�X��)�X�V����
            MarkMessageIndex(msgIndex, *msgAttribute, (NNshGlobal->NNsiParam)->hideMessage);

            // WebBrowser�ŊJ���Ƃ��A�ŐV�T�O�����J���悤�ɂ���
            if ((NNshGlobal->NNsiParam)->browseMesNum != 0)
            {
                switch (bbsInfo.bbsType)
                {
                  case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
                  case NNSH_BBSTYPE_MACHIBBS:
                    StrCat(logBuf, "&LAST=");
                    break;

                  case NNSH_BBSTYPE_SHITARABA:
                    StrCat(logBuf, "&ls=");
                    break;

#if 0
                  case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
                  case NNSH_BBSTYPE_2ch:
                  case NNSH_BBSTYPE_2ch_EUC:
                  case NNSH_BBSTYPE_OTHERBBS:
                  case NNSH_BBSTYPE_OTHERBBS_2:
#endif
                  default:
                    if ((NNshGlobal->NNsiParam)->useImodeURL != 0)
                    {
                        StrCat(logBuf, "/n");
                        goto OPEN_WEB;
                    }
                    else
                    {
                        StrCat(logBuf, "/l");
                    }
                    break;
                }
                NUMCATI(logBuf, (NNshGlobal->NNsiParam)->browseMesNum);
            }
            else
            {
                // i-mode�p��URL�������ꍇ�AURL�𒲐�����
                if ((NNshGlobal->NNsiParam)->useImodeURL != 0)
                {
                    switch (bbsInfo.bbsType)
                    {
                      case NNSH_BBSTYPE_2ch_EUC:
                      case NNSH_BBSTYPE_2ch:
                        StrCat(logBuf, "/i");
                        break;

#if 0
                      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
                      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
                      case NNSH_BBSTYPE_MACHIBBS:
                      case NNSH_BBSTYPE_SHITARABA:
                      case NNSH_BBSTYPE_OTHERBBS:
                      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
                      default:
                        break;
                    }
                }
            }

OPEN_WEB:
            // webDA�ŊJ���w���������ꍇ...
            if (btnId == BTNID_OPEN_WEBDA)
            {
                if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
                {
                    // �N���b�v�{�[�h��URL���R�s�[��webDA���N������
                    ClipboardAddItem(clipboardText, urlTop, StrLen(urlTop));
                    (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
                }
                return (errNone);
            }

            // Web�u���E�U���C���X�g�[������Ă��邩�m�F
            if (NNshGlobal->browserCreator == 0)
            {
                // install����Ă��Ȃ��Ƃ���"�T�|�[�g���Ă��܂���" �\�����s��
                NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
                return (errNone);
            }

            // Web�u���E�U�ŊJ�����m�F����
            if (NNsh_ConfirmMessage(ALTID_CONFIRM, urlTop,
                                    MSG_WEBLAUNCH_CONFIRM, 0) == 0)
            {
                // Web Browser���N������(NNsi�I����ɋN��)
                (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                         0, 0, 
                                         NNshGlobal->browserLaunchCode,
                                         urlTop, StrLen(urlTop), NULL);
            }
            return (errNone);
        }        
    }

    // ���C�ɓ���ݒ�ɂ���(�f�[�^�x�[�X��)�X�V����
    NNshGlobal->updateHR = NNSH_UPDATE_DISPLAY;
    MarkMessageIndex(msgIndex, *msgAttribute, (NNshGlobal->NNsiParam)->hideMessage);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   Get_MessageFromMesNum                                    */
/*                                            �X���ԍ��w��̃��b�Z�[�W�擾 */
/*=========================================================================*/
Boolean Get_MessageFromMesNum(Boolean *updateBBS, UInt16 *recNum, Boolean useClipboard)
{
    DmOpenRef dbRef;
    Boolean   ret;
    Char     *url, *clipP;
    MemHandle memH;
    UInt16    len;
    
    // ���[�N�o�b�t�@�̊m�ۂƏ�����
    url     = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (url == NULL)
    {
        return (false);
    }
    MemSet(url, (BIGBUF + MARGIN), 0x00);

    // �N���b�v�{�[�h�̃f�[�^���g�p����ꍇ
    if (useClipboard == true)
    {
        // �N���b�v�{�[�h�̃f�[�^���擾����
        memH = ClipboardGetItem(clipboardText, &len);
        if ((len != 0)&&(memH != 0))
        {
            len = (len > (BUFSIZE - 1)) ? (BUFSIZE - 1) : len;
            clipP = MemHandleLock(memH);
            if (clipP != NULL)
            {
                // �N���b�v�{�[�h�̃f�[�^���f�t�H���g�f�[�^�Ƃ��ăR�s�[
                StrNCopy(url, clipP, len);
            }
            MemHandleUnlock(memH);                
        }
    }

    // ���͕�����̎擾
    ret = DataInputDialog(NNSH_INPUTWIN_GETMESNUM, url, BUFSIZE,
                          NNSH_DIALOG_USE_GETMESNUM, NULL);
    if (ret != false)
    {
        //  �擾�\��@�\���L���ł��l�b�g�ɐڑ�����Ă��Ȃ��ꍇ�A
        // �擾�\�񂷂邩�m�F����
        if (((NNshGlobal->NNsiParam)->getReserveFeature != 0)&&(NNshGlobal->netRef == 0))
        {
            if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                    NNSH_MSGINFO_RESERVE, "", 0) == 0)
            {
                // �擾�\�񂪎w�����ꂽ
                OpenDatabase_NNsh(DBNAME_GETRESERVE,
                                  DBVERSION_GETRESERVE, &dbRef);
                if (dbRef == 0)
                {
                    // DB�I�[�v���G���[
                    NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh():",
                                      DBNAME_GETRESERVE, 0);
                    return (false);
                }
                // �c�a�ɓo�^���A�I������
                (void) EntryRecord_NNsh(dbRef, BUFSIZE, url);
                CloseDatabase_NNsh(dbRef);
                return (true);
            }
        }

        // �X���ԍ��w��̃��b�Z�[�W�擾�����s����
        ret = GetThreadDataFromMesNum(updateBBS, url, BIGBUF, recNum);
    }

    // �m�ۂ����̈�����
    MEMFREE_PTR(url);
    return (ret);
}

/*=========================================================================*/
/*   Function : CreateThreadURL                                            */
/*                                               �f�[�^�擾���URL�𐶐�   */
/*=========================================================================*/
Err CreateThreadURL(UInt16 urlType, Char *url, UInt16 bufSize, 
                    NNshBoardDatabase *bbsInfo, NNshSubjectDatabase *mesInfo)
{
    Char   *ptr;
    UInt16  size;

    MemSet (url, bufSize, 0x00);
    StrCopy(url, bbsInfo->boardURL);

    switch (bbsInfo->bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // JBBS/�܂�BBS����f�[�^�擾(HTML��ǂ�)
        StrCat (url, URL_PREFIX_MACHIBBS);
        StrCat (url, mesInfo->boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo->threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileName�̊g���q���폜����
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '\0';
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        // �������@JBBS �Vread.cgi�Ή�
        size = 0;
        ptr = &url[StrLen(url) - 1];
        ptr--;
        size++;
        while ((ptr > url)&&(*ptr != '/'))
        {
            ptr--;
            size++;
        }

        // "/bbs/read.cgi" ���J�e�S���̑O�ɑ}��
        MemMove(ptr + (sizeof("/bbs/read.cgi") - 1), ptr, (size + 1));

        StrCopy(ptr, "/bbs/read.cgi");
        url[StrLen(url)] = '/';

        StrCat(url, mesInfo->boardNick);
        StrCat(url, mesInfo->threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileName�̊g���q���폜����
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '/';
        ptr++;
        *ptr = '\0';

        NNsh_DebugMessage(ALTID_INFO, "URL:", url, 0);
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // �������@JBBS(rawmode.cgi)�Ή�
        size = 0;
        ptr = &url[StrLen(url) - 1];
        ptr--;
        size++;
        while ((ptr > url)&&(*ptr != '/'))
        {
            ptr--;
            size++;
        }

        // "/bbs/rawmode.cgi" ���J�e�S���̑O�ɑ}��
        MemMove(ptr + (sizeof("/bbs/rawmode.cgi") - 1), ptr, (size + 1));

        StrCopy(ptr, "/bbs/rawmode.cgi");
        url[StrLen(url)] = '/';

        StrCat(url, mesInfo->boardNick);
        StrCat(url, mesInfo->threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileName�̊g���q���폜����
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '/';
        ptr++;
        *ptr = '\0';

        NNsh_DebugMessage(ALTID_INFO, "URL:", url, 0);
        break;

      case NNSH_BBSTYPE_SHITARABA:
        // ������΂���f�[�^�擾(dat��ǂ�)
        StrCat (url, "bbs/");
        StrCat (url, mesInfo->boardNick);
        StrCat (url, "dat/");
        StrCat (url, mesInfo->threadFileName);
        break;

#if 0
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // �Q�����˂邩��f�[�^���擾(dat��ǂ�)
#ifdef USE_ZLIB
        // gzip�ʐM�g�p���ŁA�������O�C�������ǂ������`�F�b�N����
        if ((urlType != NNSH_DISABLE)&&
            (NNshGlobal->useGzip != 0)&&
            (NNshGlobal->loginDateTime != 0))
        {
            // �����O�C�����Ȃ̂Ł��Z�b�V����ID�����Ď擾 by Nickle
            char serverName[64];
            char threadNum[16];

            // �T�[�o�����擾����
            // ex. http://pc5.2ch.net/hard/ --> http://pc5.2ch.net/
            {
                Char* end;

                StrNCopy(serverName, bbsInfo->boardURL, 64);
                serverName[63] = '\0';

                end = StrChr(serverName + 7, '/'); // +7 : http://���΂�
                if (end == NULL)
                {
                    NNsh_DebugMessage(ALTID_ERROR, "Can't determine server name", "", 0);
                    break;
                }
                *(end + 1) = '\0';
            }

            // �X���ԍ��擾(threadFileName����.dat���폜)
            {
                Char* end;

                StrNCopy(threadNum, mesInfo->threadFileName, 16);
                threadNum[15] = '\0';

                end = StrChr(threadNum, '.');
                if (end == NULL)
                {
                    NNsh_DebugMessage(ALTID_ERROR, "Can't determine thread number", "", 0);
                    break;
                }
                *end = '\0';
            }
            // ���Z�b�V����ID��URL�𐶐�
            // raw=.0& �ƁA���N�G�X�g�w�b�_�� ACCEPT-ENCODING: gzip �������炤�܂����������ۂ��B
            // ��͂�gzip�Ή����K�v�Ȃ悤��
            // URL�G���R�[�h���ĂȂ���gzip���Ă��������̂ŁAURL�G���R�[�h���K�{�̂悤��
            // StrPrintF(url, "%stest/offlaw.cgi/%s/%s/?raw=.0&sid=", serverName, mesInfo->boardNick, threadNum);
            StrPrintF(url, "%stest/offlaw.cgi/%s%s/?raw=.0&sid=", serverName, mesInfo->boardNick, threadNum);
            StrCatURLencode(url, NNshGlobal->connectedSessionId);

            NNsh_DebugMessage(ALTID_INFO, "URL:", url, 0);
        }
        else
#endif
        {
            // ���O�C�����Ă��Ȃ������A���Ȃ��Ŏ擾(Original)
            StrCat (url, "dat/");
            StrCat (url, mesInfo->threadFileName);
        }
        break;
    }
    return (errNone);
}


/*=========================================================================*/
/*   Function : CreateThreadBrowserURL                                     */
/*                                 �f�[�^�擾���URL(WebBrowser�p)�𐶐�   */
/*=========================================================================*/
void CreateThreadBrowserURL(UInt16 urlType, Char *url,
                            NNshBoardDatabase    *bbsInfo,
                            NNshSubjectDatabase  *mesInfo)
{
    Char *ptr;

    StrCat(url, bbsInfo->boardURL);
    switch (bbsInfo->bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // �܂��a�a�r�̂t�q�k���o�͂���
        if ((NNshGlobal->NNsiParam)->useImodeURL == 0)
        {
            StrCat (url, URL_PREFIX_MACHIBBS);
        }
        else
        {
            StrCat (url, URL_PREFIX_MACHIBBS_IMODE);
        }
        StrCat (url, mesInfo->boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo->threadFileName);
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // ������΁��i�a�a�r�̂t�q�k���o�͂���
        if ((NNshGlobal->NNsiParam)->useImodeURL == 0)
        {
            StrCat (url, URL_PREFIX_SHITARABAJBBS);
        }
        else
        {
            StrCat (url, URL_PREFIX_SHITARABAJBBS_IMODE);
        }
        StrCat (url, mesInfo->boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo->threadFileName);
        NNsh_DebugMessage(ALTID_INFO, "URL:", url, 0);
        break;

      case NNSH_BBSTYPE_SHITARABA:
        // ������΂̂t�q�k���o�͂���
        if ((NNshGlobal->NNsiParam)->useImodeURL == 0)
        {
            StrCat (url, URL_PREFIX_SHITARABA);
        }
        else
        {
            StrCat (url, URL_PREFIX_SHITARABA_IMODE);
        }
        StrCat (url, mesInfo->boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&key=");
        StrCat (url, mesInfo->threadFileName);
        break;

#if 0
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default: 
        // 2ch��URL�ɂ́A�{�[�h�����܂܂�Ă���̂ŁA���̕��������B
        ptr = url + StrLen(url) - 2;
        while (*ptr != '/')
        {
            ptr--;
        }
        *ptr = '\0';
        StrCat (url, "/");
        if ((NNshGlobal->NNsiParam)->useImodeURL == 0)
        {
            StrCat (url, URL_PREFIX);
        }
        else
        {
            StrCopy(url, URL_PREFIX_MOBILE);
            //StrCat (url, URL_PREFIX_IMODE);
        }
        StrCat (url, bbsInfo->boardNick);
        StrCat (url, mesInfo->threadFileName);
        break;
    }

    // ������".dat" �������� ".cgi " �͍폜
    ptr = url + StrLen(url) - 1;
    while (*ptr != '.')
    {
        ptr--;
    }
    *ptr = '\0';

    return;
}

/*=========================================================================*/
/*   Function : MoveLogMessage                                             */
/*                                (���O���b�Z�[�W��Palm��/VFS�ֈړ�������) */
/*=========================================================================*/
Err MoveLogMessage(UInt16 index)
{
    Err                 ret;
    UInt16              copyMode, delMode;
    Char                logFileName[MAX_NICKNAME + MAX_THREADFILENAME + MARGIN];
    NNshSubjectDatabase subjDb;

    // VFS�p�����[�^��ON�̂Ƃ��̂ݎ��{����
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        // VFS�̃��[�h�����������̂Ŏ��{���Ȃ�
        return (NNSH_ERRORCODE_WRONGVFSMODE);
    }

    // �̈�̏�����
    MemSet(&subjDb, sizeof(subjDb), 0x00);

    // �f�[�^�x�[�X����f�[�^���擾
    ret = Get_Subject_Database(index, &subjDb);
    if (ret != errNone)
    {
        // ���肦�Ȃ��͂������H
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()"," ", ret);
        return (ret);
    }

    switch (subjDb.msgState)
    {
      case FILEMGR_STATE_OPENED_STREAM:
        // Palm������VFS�փ��O�f�[�^�ړ�
        copyMode = NNSH_COPY_PALM_TO_VFS;
        delMode  = NNSH_VFS_DISABLE;
        subjDb.msgState = FILEMGR_STATE_OPENED_VFS;

        // (VFS���)�f�B���N�g�����쐬����
        (void) CreateDir_NNsh(subjDb.boardNick);
        break;

      case FILEMGR_STATE_OPENED_VFS:
        // VFS����Palm���փ��O�f�[�^�ړ�
        copyMode = NNSH_COPY_VFS_TO_PALM;
        delMode  = NNSH_VFS_ENABLE;
        subjDb.msgState = FILEMGR_STATE_OPENED_STREAM;
        break;

      default:
        // ���肦�Ȃ��͂��A�A�A�������Ȃ��ŏI������
        // ���肦�Ȃ��͂������H
        NNsh_DebugMessage(ALTID_ERROR, "wrong msgState :", subjDb.threadFileName, subjDb.msgState);
        return (~errNone);
        break;
    }

    // ���O�f�[�^�̃t�@�C�������쐬���A�R�s�[�����{
    MemSet (logFileName, sizeof(logFileName), 0x00);
    StrCopy(logFileName, subjDb.boardNick);
    StrCat (logFileName, subjDb.threadFileName);
    ret = CopyFile_NNsh(logFileName, logFileName, copyMode);
    if (ret != errNone)
    {
        // �t�@�C���̃R�s�[�Ɏ��s
        // �R�s�[���悤�Ƃ����t�@�C��(���r���[�ȃt�@�C��)���폜����
        delMode = (delMode == NNSH_VFS_DISABLE) ?
                                            NNSH_VFS_ENABLE : NNSH_VFS_DISABLE;
        DeleteFile_NNsh(logFileName, delMode);
        return (NNSH_ERRORCODE_COPYFAIL);
    }

    // �R�s�[�����O�t�@�C�����폜����
    DeleteFile_NNsh(logFileName, delMode);

    // �f�[�^���X�V
    ret = update_subject_database(index, &subjDb);
    if (ret != errNone)
    {
        // ���肦�Ȃ��͂�...
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()"," ", ret);
        return (ret);
    }
    return (errNone);
}

#ifdef USE_SSL
/*=========================================================================*/
/*   Function : ProceedOysterLogin                                         */
/*                                                        ���̃��O�C������ */
/*=========================================================================*/
Err ProceedOysterLogin(void)
{
    Err          err;
    Boolean      ret;
    UInt32       dummy;
    Int32        timeData, sample;
    DateTimeType dateBuf;
    NNshFileRef  fileRef;
    Char         *buffer, *buf, *ptr, *sId;

    buffer = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (buffer == NULL)
    {
        // ���O�C�������{���Ȃ�
        return (errNone);
    }
    MemSet (buffer, (BIGBUF + MARGIN),  0x00);
    if (NNshGlobal->loginDateTime != 0)
    {
        // ���O�C��������\��
        StrCopy(buffer, MSG_LOGINDATETIME);

        MemSet(&dateBuf, sizeof(dateBuf), 0x00);
        TimSecondsToDateTime(NNshGlobal->loginDateTime, &dateBuf);

        DateToAscii((UInt8)  dateBuf.month,
                    (UInt8)  dateBuf.day,
                    (UInt16) dateBuf.year, 
                    dfYMDLongWithDot,
                    &buffer[StrLen(buffer)]);

        TimeToAscii((UInt8) dateBuf.hour,
                    (UInt8) dateBuf.minute,
                     tfColon24h,
                    &buffer[StrLen(buffer)]);
    }
    else
    {
        // ���O�C�����Ă��Ȃ����Ƃ�\��
        StrCopy(buffer, MSG_NOTLOGIN);
    }
    dummy    = TimGetSeconds() - NNshGlobal->loginDateTime;
    timeData = (Int32) dummy;
    sample   = ((TIME_24HOUR_SECOND) - (Int32) 10);
    if (timeData > sample)
    {
        // �ă��O�C�����K�v�Ȃ��Ƃ�\������
        StrCat(buffer, MSG_LOGIN_INFO);
    }

    ret = DataInputPassword("2ch LOGIN", (NNshGlobal->NNsiParam)->oysterUserId,
                            sizeof((NNshGlobal->NNsiParam)->oysterUserId) - 1,
                            (NNshGlobal->NNsiParam)->oysterPassword,
                            sizeof((NNshGlobal->NNsiParam)->oysterPassword) - 1,
                            buffer);
    // Cancel�{�^���������ꂽ�Ƃ�
    if (ret == false)
    {
        // ���O�C�������{���Ȃ�
        MEMFREE_PTR(buffer);
        return (errNone);
    }

    // ���O�C�����������s���邩�m�F
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_LOGIN_CONFIRM,
                            (NNshGlobal->NNsiParam)->oysterUserId, 0) != 0) 
    {
        // ���O�C�������{���Ȃ�
        MEMFREE_PTR(buffer);
        return (errNone);
    }

    MemSet(buffer, (BIGBUF + MARGIN),  0x00);
    StrCopy(buffer, "ID=");
    StrCat(buffer, (NNshGlobal->NNsiParam)->oysterUserId);
    StrCat(buffer, "&PW=");
    StrCat(buffer, (NNshGlobal->NNsiParam)->oysterPassword);
    ptr = buffer;

    // �o�͗p�t�@�C���̏���
    (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
    if (OpenFile_NNsh(FILE_SENDMSG, 
                      (NNSH_FILEMODE_APPEND|NNSH_FILEMODE_TEMPFILE),
                      &fileRef) != errNone)
    {
        // �t�@�C��open���s
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    // �f�[�^�������o���ăt�@�C���N���[�Y
    (void) AppendFile_NNsh(&fileRef, StrLen(ptr), ptr, &dummy);
    CloseFile_NNsh(&fileRef);

    // �f�[�^�̑��M
#ifdef USE_OLD_2ch_LOGINSERVER
    err = NNshHttp_comm(HTTP_SENDTYPE_POST_2chLOGIN,
                        "https://tiger2.he.net/~tora3n2c/futen.cgi",
                        NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
#else
    err = NNshHttp_comm(HTTP_SENDTYPE_POST_2chLOGIN,
                        "https://2chv.tora3.net/futen.cgi",
                        NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
#endif
    if (err != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (err != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "[LOGIN]", err);
            MEMFREE_PTR(buffer);
            return (err);
        }

        // �ʐM�^�C���A�E�g����
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[LOGIN]", err);
        MEMFREE_PTR(buffer);
        return (err);
    }

#if 0
    // �������񂾂��ƁA�ǂݏo�������Ɉڂ�̂��͂₷����̂���... 1�b��wait������...
    SysTaskDelay(1 * SysTicksPerSecond());
#endif

    // ��M�f�[�^����͂���
    err = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "HTTP Reply Message ", "", err);
        MEMFREE_PTR(buffer);
        return (err);
    }
    buf = MEMALLOC_PTR(NNSH_WORKBUF_MIN);
    if (buf == NULL)
    {
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    (void) ReadFile_NNsh(&fileRef, 0, NNSH_WORKBUF_MIN, buf, &dummy);
    CloseFile_NNsh(&fileRef);

    // �����R���e���c�̐擪��T��
    ptr = StrStr(buf, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // �f�[�^�ُ�
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(" NNSH_HTTP_HEADER_ENDTOKEN ")", 0);
        MEMFREE_PTR(buf);
        MEMFREE_PTR(buffer);
        return (~errNone - 10);
    }

    // �������ꂽUA��SessionID�i�ƔF��OK�ƂȂ��������j���L������
    sId = StrStr(ptr, "SESSION-ID=");
    if (sId == NULL)
    {
        // �T�[�o���G���[�������A�I������
        // (���O�C���Ɏ��s�������Ƃ�ʒm����)
        NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_FAILURE, "(abnormal)", 0);
        goto FUNC_END;
    }
    // if (sId != NULL)
    {
        // �f�[�^�̖�����T��
        sId = sId + StrLen("SESSION-ID=");

        // �G���[����...
        if ((*(sId + 0) == 'E')&&(*(sId + 1) == 'R')&&(*(sId + 2) == 'R')&&
            (*(sId + 3) == 'O')&&(*(sId + 4) == 'R'))
        {
            // �T�[�o���G���[�������A�I������
            // (���O�C���Ɏ��s�������Ƃ�ʒm����)
            NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_FAILURE, "", 0);
            goto FUNC_END;
        }

        ptr = sId;
        while (*ptr > ' ')
        {
            ptr++;
        }
        *ptr = '\0';
        
        dummy = StrLen(sId);
        ptr = MEMALLOC_PTR(dummy + MARGIN);
        if (ptr != NULL)
        {
            // �Z�b�V�����h�c���R�s�[����
            MemSet(ptr, (dummy + MARGIN), 0x00);
            StrCopy(ptr, sId);
            NNshGlobal->connectedSessionId = ptr;
        }
        ptr = sId;
        while ((*ptr != ':')&&(*ptr != '\0'))
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            *ptr = '\0';
            dummy = StrLen(sId);
            ptr = MEMALLOC_PTR(dummy + MARGIN);
            if (ptr != NULL)
            {
                // ���[�U�[�G�[�W�F���g�����R�s�[����
                MemSet(ptr, (dummy + MARGIN), 0x00);
                StrCopy(ptr, sId);
                NNshGlobal->customizedUserAgent = ptr;
            }
        }            
    }
    // ���O�C���������L�^���Ă���
    NNshGlobal->loginDateTime = TimGetSeconds();

    // ���O�C���ɐ�����������\������
    NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_SUCCESS, (NNshGlobal->NNsiParam)->oysterUserId, 0);

FUNC_END:
    MEMFREE_PTR(buf);
    MEMFREE_PTR(buffer);
    return (errNone);
}
#endif

/*=========================================================================*/
/*   Function : ProceedBe2chLogin                                          */
/*                                                    Be@2ch�̃��O�C������ */
/*=========================================================================*/
Err ProceedBe2chLogin(void)
{
    Err          err;
    Boolean      ret;
    UInt32       dummy;
    Int32        timeData, sample;
    DateTimeType dateBuf;
    NNshFileRef  fileRef;
    Char         *buffer, *buf, *ptr, *sId, *uId, *btm;

    buffer = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (buffer == NULL)
    {
        // ���O�C�������{���Ȃ�
        return (errNone);
    }
    MemSet (buffer, (BIGBUF + MARGIN),  0x00);
    if (NNshGlobal->be2chLoginDateTime != 0)
    {
        // ���O�C��������\��
        StrCopy(buffer, MSG_LOGINDATETIME);

        MemSet(&dateBuf, sizeof(dateBuf), 0x00);
        TimSecondsToDateTime(NNshGlobal->be2chLoginDateTime, &dateBuf);

        DateToAscii((UInt8)  dateBuf.month,
                    (UInt8)  dateBuf.day,
                    (UInt16) dateBuf.year, 
                    dfYMDLongWithDot,
                    &buffer[StrLen(buffer)]);

        TimeToAscii((UInt8) dateBuf.hour,
                    (UInt8) dateBuf.minute,
                     tfColon24h,
                    &buffer[StrLen(buffer)]);
    }
    else
    {
        // ���O�C�����Ă��Ȃ����Ƃ�\��
        StrCopy(buffer, MSG_NOTLOGIN);
    }
    dummy    = TimGetSeconds() - NNshGlobal->loginDateTime;
    timeData = (Int32) dummy;
    sample   = ((TIME_24HOUR_SECOND) - (Int32) 10);

    // ���[�UID�ƃp�X���[�h�̓���
    ret = DataInputPassword("Be@2ch LOGIN", (NNshGlobal->NNsiParam)->be2chId,
                            sizeof((NNshGlobal->NNsiParam)->be2chId) - 1,
                            (NNshGlobal->NNsiParam)->be2chPw,
                            sizeof((NNshGlobal->NNsiParam)->be2chPw) - 1,
                            buffer);
    // Cancel�{�^���������ꂽ�Ƃ�
    if (ret == false)
    {
        // ���O�C�������{���Ȃ�
        MEMFREE_PTR(buffer);
        return (errNone);
    }

    // ���O�C�����������s���邩�m�F
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_LOGIN_CONFIRM,
                            (NNshGlobal->NNsiParam)->be2chId, 0) != 0) 
    {
        // ���O�C�������{���Ȃ�
        MEMFREE_PTR(buffer);
        return (errNone);
    }

    // �o�͗p�t�@�C���̏���
    (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
    if (OpenFile_NNsh(FILE_SENDMSG, 
                      (NNSH_FILEMODE_APPEND|NNSH_FILEMODE_TEMPFILE),
                      &fileRef) != errNone)
    {
        // �t�@�C��open���s
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    // ���[�UID���p�X���[�h�f�[�^�������o���ăt�@�C���N���[�Y
    MemSet(buffer, (BIGBUF + MARGIN),  0x00);
    StrCopy(buffer, "m=");
    (void) AppendFile_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    StrCopy(buffer, (NNshGlobal->NNsiParam)->be2chId);
    (void) AppendFileAsURLEncode_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    StrCopy(buffer, "&p=");
    (void) AppendFile_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    StrCopy(buffer, (NNshGlobal->NNsiParam)->be2chPw);
    (void) AppendFileAsURLEncode_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    StrCopy(buffer, "&submit=%C5%D0%CF%BF");
    (void) AppendFile_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    CloseFile_NNsh(&fileRef);


    // LOGIN�t�H�[���̎�M
    err = NNshHttp_comm(HTTP_SENDTYPE_GET,
                        "http://be.2ch.net/test/login.php",
                        NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    if (err != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (err != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "[GET FORM]", err);
            MEMFREE_PTR(buffer);
            return (err);
        }

        // �ʐM�^�C���A�E�g����
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[GET FORM]", err);
        MEMFREE_PTR(buffer);
        return (err);
    }

    // LOGIN�f�[�^�̑��M
    err = NNshHttp_comm(HTTP_SENDTYPE_POST_Be2chLOGIN,
                        "http://be.2ch.net/test/login.php",
                        NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    if (err != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (err != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "[LOGIN]", err);
            MEMFREE_PTR(buffer);
            return (err);
        }

        // �ʐM�^�C���A�E�g����
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[LOGIN]", err);
        MEMFREE_PTR(buffer);
        return (err);
    }

#if 0
    // �������񂾂��ƁA�ǂݏo�������Ɉڂ�̂��͂₷����̂���... 1�b��wait������...
    SysTaskDelay(1 * SysTicksPerSecond());
#endif

    // ��M�f�[�^����͂���
    err = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "HTTP Reply Message ", "", err);
        MEMFREE_PTR(buffer);
        return (err);
    }
    buf = MEMALLOC_PTR(NNSH_WORKBUF_MIN);
    if (buf == NULL)
    {
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    (void) ReadFile_NNsh(&fileRef, 0, NNSH_WORKBUF_MIN, buf, &dummy);
    CloseFile_NNsh(&fileRef);

    // �����R���e���c�̐擪��T��
    ptr = StrStr(buf, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // �f�[�^�ُ�
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(" NNSH_HTTP_HEADER_ENDTOKEN ")", 0);
        MEMFREE_PTR(buf);
        MEMFREE_PTR(buffer);
        return (~errNone - 10);
    }

    // �������ꂽCookie�f�[�^���L������
    sId = StrStr(ptr, "document.cookie");
    if (sId == NULL)
    {
        // �T�[�o���G���[�������A�I������
        // (���O�C���Ɏ��s�������Ƃ�ʒm����)
        NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_FAILURE, "(abnormal/cookie1)", 0);
        goto FUNC_END;
    }
    // if (sId != NULL)
    {
        // ���f�[�^�̐擪�܂ňړ�����
        sId = sId + StrLen("document.cookie");
        while ((*sId != '\'')&&(*sId != '\0'))
        {
            sId++;
        }       
        if (*sId == '\0')
        {
            // ���f�[�^���Ȃ�...   
            NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_FAILURE, "(abnormal/cookie2)", 0);
            goto FUNC_END;
        }
        sId++;

        // �f�[�^�̖�����T��
        ptr = sId;
        while ((*ptr != ';')&&(*ptr != '\x0a'))
        {
            ptr++;
        }
        ptr++;
        *ptr = ' ';
        ptr++;
        *ptr = '\0';
        btm  = ptr;

        // �������ꂽCookie�f�[�^���L������(part2)
        ptr++;
        uId = StrStr(ptr, "document.cookie");
        if (uId != NULL)
        {
            // ���f�[�^�̐擪�܂ňړ�����
            uId = uId + StrLen("document.cookie");
            while ((*uId != '\'')&&(*uId != '\0'))
            {
                uId++;
            }
            // ���f�[�^���Ȃ�����...
            if (*uId == '\0')
            {
                goto ENTRY_COOKIE;   
            }

            // �f�[�^�̖�����T��
            uId++;
            ptr = uId;
            while ((*ptr != ';')&&(*ptr != '\x0a')&&(*ptr != '\0'))
            {
                if (*ptr == '\'')
                {
                    ptr++;
                    while ((*ptr != '\'')&&(*ptr != '\x0a')&&(*ptr != '\0'))
                    {
                        ptr++;    
                    }
                    if ((*ptr == '\x0a')||(*ptr == '\0'))
                    {
                        break;
                    }
                    ptr++;
                }
                *btm = *ptr;
                btm++;
                ptr++;
            }
            *btm = '\0';
        }
ENTRY_COOKIE:    
        dummy = StrLen(sId);
        ptr = MEMALLOC_PTR(dummy + MARGIN);
        if (ptr != NULL)
        {
            // Cookie���R�s�[����
            MemSet(ptr, (dummy + MARGIN), 0x00);
            StrCopy(ptr, sId);

            // ���ł�Cookie�������Ă���Ƃ��ɂ́A��x�̈���N���A����
            if (NNshGlobal->be2chCookie != NULL)
            {
                MEMFREE_PTR(NNshGlobal->be2chCookie);
                NNshGlobal->be2chCookie = NULL;
            }
            NNshGlobal->be2chCookie = ptr;
        }
    }
    // ���O�C���������L�^���Ă���
    NNshGlobal->be2chLoginDateTime = TimGetSeconds();

    // ���O�C���ɐ�����������\������
    NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_SUCCESS, (NNshGlobal->NNsiParam)->be2chId, 0);
    NNsh_DebugMessage (ALTID_INFO, "Cookie : ", NNshGlobal->be2chCookie, 0);

FUNC_END:
    MEMFREE_PTR(buf);
    MEMFREE_PTR(buffer);
    return (errNone);
}

/*=========================================================================*/
/*   Function : ShowReserveGetLogList                                      */
/*                             �X���ԍ��w��\�񂵂Ă���URL�̈ꗗ��\������ */
/*=========================================================================*/
void ShowReserveGetLogList(void)
{
    Char    **dataList;
    UInt16    nofList, loop;
    DmOpenRef dbRef;

    // ������...
    nofList = 0;
    dataList = NULL;

    // �X���Ԏw��擾�Ŏ擾��ۗ��������̂��擾����
    OpenDatabase_NNsh(DBNAME_GETRESERVE, DBVERSION_GETRESERVE, &dbRef);
    if (dbRef == 0)
    {
        // DB�I�[�v���G���[
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh():", 
                          DBNAME_GETRESERVE, 0);
        return;
    }

    // �����擾
    GetDBCount_NNsh(dbRef, &nofList);
    if (nofList != 0)
    {
        dataList = MEMALLOC_PTR(sizeof(Char *) * nofList + MARGIN);
        if (dataList == NULL)
        {
            // �c�a�I�[�v���G���[�����A�I������
            CloseDatabase_NNsh(dbRef);
            NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR():", 
                              DBNAME_GETRESERVE, nofList);
            return;
        }
        MemSet(dataList, (sizeof(Char *) * nofList + MARGIN), 0x00);

        // �S���A�X���Ԏw��擾�����{����
        for (loop = 0; loop < nofList; loop++)
        {
            dataList[loop] = MEMALLOC_PTR(BUFSIZE + MARGIN);
            if (dataList[loop] == NULL)
            {
                // �̈�m�ۃG���[�A�m�ۂ����Ƃ���܂�...
                nofList = loop;
                break;
            }
            MemSet(dataList[loop], (BUFSIZE + MARGIN), 0x00);

            // �f�[�^�擾
            GetRecord_NNsh(dbRef, loop, BUFSIZE, dataList[loop]);
        }
    }
    else
    {
        // �\��Ȃ��A�ƕ\������
        nofList = 1;
        dataList = MEMALLOC_PTR(sizeof(Char *) + MARGIN);
        dataList[0] = MEMALLOC_PTR(BUFSIZE + MARGIN);
        MemSet(dataList[0], (BUFSIZE + MARGIN), 0x00);
        StrCopy(dataList[0], NNSH_GETLOGMSG_NONE);
    }
    CloseDatabase_NNsh(dbRef);

    // �f�[�^�̕\��
    DataViewForm(NNSH_GETLOGMSG_VIEWTITLE, nofList, dataList);

    // �m�ۂ����̈���J������
    for (loop = 0; loop < nofList; loop++)
    {
        if (dataList[loop] != NULL)
        {
            MEMFREE_PTR(dataList[loop]);
        }
    }
    MEMFREE_PTR(dataList);

    return;
}

/*=========================================================================*/
/*   Function : UpdateThreadResponseNumber                                 */
/*                                              �X���Ɋ܂܂�郌�X�����X�V */
/*=========================================================================*/
UInt16  UpdateThreadResponseNumber(UInt16 dbIndex)
{
    Err                  ret;
    NNshSubjectDatabase *subjDB;
    NNshMessageIndex    *msgIndex;
    UInt16               bufferSize, fileMode, nofMsg;
    Char                *fileName, *area;

    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        // �̈�m�ێ��s...
        return (0);
    }

    subjDB = MEMALLOC_PTR(sizeof(NNshSubjectDatabase) + MARGIN);
    if (subjDB == NULL)
    {
        // �̈�m�ێ��s...
        MEMFREE_PTR(fileName);
        return (0);
    }

    // �X�������܂��擾
    ret = Get_Subject_Database(dbIndex, subjDB);
    if (ret != errNone)
    {
        // subject�f�[�^�x�[�X����f�[�^�擾�Ɏ��s
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        MEMFREE_PTR(fileName);
        MEMFREE_PTR(subjDB);
        return (0);
    }

    bufferSize = sizeof(NNshMessageIndex) + (NNshGlobal->NNsiParam)->bufferSize + MARGIN * 2;
    msgIndex   = MEMALLOC_PTR(bufferSize);
    if (msgIndex == NULL)
    {
        MEMFREE_PTR(subjDB);
        MEMFREE_PTR(fileName);
        return (0);
    }
    MemSet(msgIndex, bufferSize, 0x00);

    // ���b�Z�[�W�I�t�Z�b�g�̈���m�ۂ���
    bufferSize = sizeof(UInt32) * ((NNshGlobal->NNsiParam)->msgNumLimit + MARGIN);
    (msgIndex)->msgOffset = MEMALLOC_PTR(bufferSize);
    if ((msgIndex)->msgOffset == NULL)
    {
        // ���b�Z�[�W�I�t�Z�b�g�̈�̊m�ێ��s
        MEMFREE_PTR(msgIndex);
        MEMFREE_PTR(subjDB);
        MEMFREE_PTR(fileName);
        NNsh_InformMessage(ALTID_ERROR, MSG_ERR_MEMPTR_NEW_BUFF, " size:",
                           bufferSize);
        return (0);
    }
    MemSet((msgIndex)->msgOffset, bufferSize, 0x00);

    msgIndex->bbsType = (UInt16) subjDB->bbsType;

    // �ǂݍ��ރt�@�C���̏�����
    MemSet (fileName, sizeof(fileName), 0x00);

    // OFFLINE�X���̂Ƃ��́AboardNick��t�����Ȃ��悤�ɂ���
    if (StrCompare(subjDB->boardNick, OFFLINE_THREAD_NICK) != 0)
    {
        StrCopy(fileName, subjDB->boardNick);
    }
    else
    {
        // OFFLINE�X����VFS�Ɋi�[����Ă���ꍇ�ɂ́A�f�B���N�g����t������B
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
        {
            // ���O�x�[�X�f�B���N�g����t������
            if ((NNshGlobal->logPrefixH == 0)||
                ((area = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
            {
                StrCopy(fileName, LOGDATAFILE_PREFIX);
            }
            else
            {
                StrCopy(fileName, area);
                MemHandleUnlock(NNshGlobal->logPrefixH);
            }

            // ����ɃT�u�f�B���N�g�����������ĕt������
            GetSubDirectoryName(subjDB->dirIndex, &fileName[StrLen(fileName)]);
        }
    }
    StrCat (fileName, subjDB->threadFileName);

    // �t�@�C���̂��肩�ŁA�J�����[�h��؂�ւ���
    if (subjDB->msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        fileMode = ((NNSH_FILEMODE_TEMPFILE)|(NNSH_FILEMODE_READONLY));
    }
    else
    {
        fileMode = (NNSH_FILEMODE_READONLY);
    }
    nofMsg = 0;
    ret = OpenFile_NNsh(fileName,fileMode,&(msgIndex->fileRef));
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Message File Open :", fileName, ret);
        goto FUNC_END;
    }
    GetFileSize_NNsh(&(msgIndex->fileRef), &(msgIndex->fileSize));

    //  ���b�Z�[�W�X���b�h�̃C���f�b�N�X���쐬����
    ret = CreateMessageThreadIndex(msgIndex, &(msgIndex->nofMsg), 
                                   msgIndex->msgOffset,
                                   (UInt16) subjDB->bbsType);
    if (ret != errNone)
    {
        // �X���̔ԍ��擾�Ɏ��s�A�X�V���Ȃ�
        msgIndex->nofMsg = 0;
        NNsh_DebugMessage(ALTID_ERROR, "ERR>index creation failure", "", ret);
    }
    nofMsg = msgIndex->nofMsg;
    CloseFile_NNsh(&(msgIndex->fileRef));

FUNC_END:
    // �I������
    MEMFREE_PTR((msgIndex)->msgOffset);
    MEMFREE_PTR(msgIndex);
    MEMFREE_PTR(fileName);

    // ��M���X���ɍX�V�����������ǂ����m�F����
    if (nofMsg != 0)
    {
        if (subjDB->maxLoc != nofMsg)
        {
            // ���X���̍X�V�����m�A�ő僌�X���ƃ��X��Ԃ�ύX����
            subjDB->maxLoc = nofMsg;
            subjDB->state  = NNSH_SUBJSTATUS_REMAIN;
        }
        else
        {
            // ���X��S���ǂ�łȂ��Ƃ��A�u�V���v�}�[�N�����Ă���
            // ���ɁA���ǃ}�[�N�ɕύX����
            if ((subjDB->maxLoc > subjDB->currentLoc)&&
                (subjDB->state == NNSH_SUBJSTATUS_NEW))
            {
                // New�}�[�N����������
                subjDB->state = NNSH_SUBJSTATUS_REMAIN;
            }
        }

        // �X���c�a���X�V����
        update_subject_database(dbIndex, subjDB); 
    }

    MEMFREE_PTR(subjDB);
    return (nofMsg);
}

/*=========================================================================*/
/*   Function :   DeleteThreadMessage                                      */
/*                                                           �X���폜����  */
/*=========================================================================*/
Boolean DeleteThreadMessage(UInt16 msgIndex)
{
    Err                 ret;
    Char                fileName[MAXLENGTH_FILENAME], *area;
    DmOpenRef           dbRef;
    NNshSubjectDatabase mesInfo;

    // �X�������܂��擾
    ret = Get_Subject_Database(msgIndex, &mesInfo);
    if (ret != errNone)
    {
        // subject�f�[�^�x�[�X����f�[�^�擾�Ɏ��s
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        return (false);
    }

    // �X�����DB�̃I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh(SUBJECT)", "", 0);
        return (false);
    }

    // �X���t�@�C��(�{�̂̃��O�t�@�C��)���폜
    MemSet (fileName, sizeof(fileName), 0x00);
    // OFFLINE�X���̂Ƃ��́AboardNick��t�����Ȃ��悤�ɂ���
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) != 0)
    {
        // OFFLINE���O�̂Ƃ��́AboardNick��t�����Ȃ�
        // (OFFLINE���O�̂Ƃ��A���̃��W�b�N�͒ʂ�Ȃ�)
        StrCopy(fileName, mesInfo.boardNick);
    }
    else
    {
        // OFFLINE�X����VFS�Ɋi�[����Ă���ꍇ�ɂ́A�f�B���N�g����t������B
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
        {
            // ���O�x�[�X�f�B���N�g�����擾����
            if ((NNshGlobal->logPrefixH == 0)||
                ((area = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
            {
                StrCopy(fileName, LOGDATAFILE_PREFIX);
            }
            else
            {
                StrCopy(fileName, area);
                MemHandleUnlock(NNshGlobal->logPrefixH);
            }
            GetSubDirectoryName(mesInfo.dirIndex, &fileName[StrLen(fileName)]);
        }
    }
    StrCat (fileName, mesInfo.threadFileName);

    if (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        // Palm�{�̏�̃��O���폜����
        ret = DeleteFile_NNsh(fileName, NNSH_VFS_DISABLE);
    }
    else
    {
        // VFS��̃��O���폜����
        ret = DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }
    if((ret != errNone)&&(ret != fileErrNotFound)&&(ret != vfsErrFileNotFound))
    {
        NNsh_InformMessage(ALTID_ERROR, "File Delete: ", fileName, ret);
    }

    // �X���C���f�b�N�X�t�@�C�����폜���ăN���[�Y
    (void) DeleteRecordIDX_NNsh(dbRef, msgIndex);
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return (true);
}

/*=========================================================================*/
/*   Function :   DeleteThreadMessage_RecordID                             */
/*                                                           �X���폜����  */
/*=========================================================================*/
Boolean DeleteThreadMessage_RecordID(UInt32 recId)
{
    Boolean   ret;
    DmOpenRef dbRef;
    UInt16    msgIndex = 0;

    // �X�����DB�̃I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����
        return (false);
    }

    ret = false;
    if (GetRecordIndexFromRecordID_NNsh(dbRef, recId, &msgIndex) == errNone)
    {
        CloseDatabase_NNsh(dbRef);
        ret = DeleteThreadMessage(msgIndex);
    }
    else
    {
        CloseDatabase_NNsh(dbRef);
    }
    return (ret);
}
