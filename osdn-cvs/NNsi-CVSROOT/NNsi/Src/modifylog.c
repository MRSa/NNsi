/*============================================================================*
 *
 *  $Id: modifylog.c,v 1.3 2005/07/18 16:26:31 mrsa Exp $
 *
 *  FILE: 
 *     modifylog.c
 *
 *  Description: 
 *     �Q�ƃ��O�̃��O�擾����H
 *===========================================================================*/
#define MODIFYLOG_C
#include "local.h"

#ifdef USE_LOGCHARGE

/*--------------------------------------------------------------------------*/
/*   launch_viewJPEG() : JPEG�t�@�C���̉{��(DA�N�������̗\��...)            */
/*                                      fileLoc  : �t�@�C���̂��肩         */
/*                                          FILEMGR_STATE_OPENED_STREAM     */
/*                                          FILEMGR_STATE_OPENED_VFS        */
/*                                      fileName : �t�@�C����(full-path)    */
/*--------------------------------------------------------------------------*/
static void launch_viewJPEG(UInt16 fileLoc, Char *fileName)
{
    Char   fileType, *cmdMsg;

    NNsh_DebugMessage(ALTID_INFO, "OPEN JPEG : ", fileName, fileLoc);

    // nnDA���C���X�g�[������Ă��邩�m�F����
    if (CheckInstalledResource_NNsh('DAcc','nnDA') == false)
    {
        // nnDA���C���X�g�[���A�I������
        return;
    }

    if (fileLoc == FILEMGR_STATE_OPENED_VFS)
    {
        // "VFS�̓R���p�N�g�t���b�V�����g�p����"�ݒ�̏ꍇ�A�A�A
        switch ((NNshGlobal->NNsiParam)->vfsUseCompactFlash)
        {
          case NNSH_VFS_USEMEDIA_CF:             
            fileType = 'c';
            break;

          case NNSH_VFS_USEMEDIA_MS: 
            fileType = 'm';
            break;

          case NNSH_VFS_USEMEDIA_SD:
            fileType = 's';             
            break;

          case NNSH_VFS_USEMEDIA_SM:
            fileType = 't';
            break;
                
          case NNSH_VFS_USEMEDIA_RD:             
            fileType = 'r';
            break;

          case NNSH_VFS_USEMEDIA_DoC:
            fileType = 'd';
            break;

          default:
            // �w�肵�Ȃ�
            fileType = 'v';
            break;
        }
    }
    else
    {
        fileType = 'f';
    }

    // �N���b�v�{�[�h�p�̗̈���m�ۂ���
    cmdMsg = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (cmdMsg == NULL)
    {
        return;
    }
    MemSet(cmdMsg, BIGBUF + MARGIN, 0x00);

    StrCopy(cmdMsg, nnDA_NNSIEXT_VIEWSTART);
    StrCat (cmdMsg, nnDA_NNSIEXT_INFONAME);
    StrCat (cmdMsg, nnDA_NNSIEXT_SHOWJPEG);
    cmdMsg[StrLen(cmdMsg)] = fileType;
    StrCat (cmdMsg, "NNsi:");
    StrCat (cmdMsg, fileName);
    StrCat (cmdMsg, nnDA_NNSIEXT_ENDINFONAME);
    StrCat (cmdMsg, nnDA_NNSIEXT_ENDVIEW);
    ClipboardAddItem(clipboardText, cmdMsg, StrLen(cmdMsg) + 1); 

    MEMFREE_PTR(cmdMsg);

    // nnDA���N������
    (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
    return;
}

/*--------------------------------------------------------------------------*/
/*  ReadOnlyLogDataPickup() : �Q�ƃ��O�̃��O�擾������                      */
/*                                                                          */
/*                                                                          */
/*                    ���̊֐��̈����ŁA�X�V��������f�[�^ : url, fileName  */
/*--------------------------------------------------------------------------*/
Err ReadOnlyLogDataPickup(Boolean isImage, NNshGetLogDatabase *dbData,
                          Char *url, Char *fileName, UInt16 recNum,
                          UInt32 recId, UInt16 depth, UInt16 number, 
                          Char *dateTime)
{
    Err                       ret;
    Boolean                   workAround, updateRecord;
    UInt16                    index, bufSize, kanjiCode, redirectCnt, titleSize, subIndex;
    UInt32                    readSize, offset, dummy, offsetW, currentTime;
    DmOpenRef                 subjRef;
    Char                      *ptr, *file, *realBuf, *chk, *dateBuf, *titlePrefix;
    NNshSubjectDatabase       *subjDb;
    NNshFileRef                fileRefR, fileRefW;
    NNshGetLogSubFileDatabase *subDbData;

    // �X���^�C�g�������(�Ăяo����)�Ŏw�肳��Ă��邩�H
    if ((depth != 0)&&(dateTime != NULL))
    {
        // �X���^�C�g���́A��Q�K�w�ȉ��Ŏw�肳��Ă���
        // (���K�w�ł̕ϐ�dateTime�́AIf-Modified-Since �̈����ŗ��p���Ă���)
        // ����ȕϐ��̗��p�Ȃ̂Œ��ӂ��邱�ƁA�A�B
        titlePrefix = dateTime;
    }
    else
    {
        titlePrefix = NULL;
    }

    // �̈���m�ۂ���
    subIndex = NNSH_DATABASE_BLANK;
    bufSize = sizeof(NNshSubjectDatabase) + MARGIN;
    subjDb = MEMALLOC_PTR(bufSize);
    if (subjDb == NULL)
    {
        // �ꎞ�̈�̊m�ۂɎ��s����
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", bufSize);
        return (~errNone);
    }
    MemSet(subjDb, sizeof(NNshSubjectDatabase) + MARGIN, 0x00);
  
    dateBuf = MEMALLOC_PTR(sizeof("If-Modified-Since: ") + MAX_GETLOG_DATETIME + MARGIN);
    if (dateBuf == NULL)
    {
        // �ꎞ�̈�̊m�ۂɎ��s...
        MEMFREE_PTR(subjDb);
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", (sizeof("If-Modified-Since: ") + MAX_GETLOG_DATETIME + MARGIN));
        return (~errNone);
    } 
    MemSet(dateBuf, (sizeof("If-Modified-Since: ") + MAX_GETLOG_DATETIME + MARGIN), 0x00);
  
    subDbData = MEMALLOC_PTR(sizeof(NNshGetLogSubFileDatabase) + MARGIN);
    if (subDbData == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (subDbData) ", "", 0);
        MEMFREE_PTR(subjDb);
        MEMFREE_PTR(dateBuf);
        return (~errNone);
    }
    MemSet(subDbData, (sizeof(NNshGetLogSubFileDatabase) + MARGIN), 0x00);
  
    // �w�肳�ꂽURL��\������
    NNsh_DebugMessage(ALTID_INFO, "url:", url, 0);

    // �����R�[�h
    kanjiCode = dbData->kanjiCode;

    // �ꎞ�o�b�t�@�̈���m��
    bufSize = (NNshGlobal->NNsiParam)->bufferSize + MARGIN;
    realBuf = MEMALLOC_PTR(bufSize);
    if (realBuf == NULL)
    {
        // �ꎞ�̈�̊m�ۂɎ��s����
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", bufSize);
        MEMFREE_PTR(dateBuf);
        MEMFREE_PTR(subDbData);
        MEMFREE_PTR(subjDb);
        return (~errNone);
    }
    MemSet(realBuf, bufSize, 0x00);
    bufSize = bufSize - MARGIN;

    // �X���f�[�^�x�[�X�̃I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &subjRef);
    if (subjRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        MEMFREE_PTR(subjDb);
        MEMFREE_PTR(subDbData);
        MEMFREE_PTR(dateBuf);
        MEMFREE_PTR(realBuf);
        return (~errNone);
    }

    // �����o���t�@�C�����̓���ƁA�X���c�a�ɏ������ރf�[�^�����
    if (StrLen(fileName) != 0)
    {
        // �����Ƀt�@�C�������w�肳��Ă����ꍇ�ɂ́A���̃t�@�C�������g�p����
        //                                      (�t�@�C�������X��DB�Ɋi�[����)
        StrNCopy(subjDb->threadFileName, fileName, MAX_THREADFILENAME);
    }
    MemSet(fileName, sizeof(fileName), 0x00);

    // OFFLINE�X����VFS�Ɋi�[�����ꍇ�ɂ́A�f�B���N�g����t������B
    workAround = false;
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
    {
        // ���O�x�[�X�f�B���N�g����t������
        if ((NNshGlobal->logPrefixH == 0)||
            ((ptr = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
        {
            StrCopy(fileName, LOGDATAFILE_PREFIX);
        }
        else
        {
            StrCopy(fileName, ptr);
            MemHandleUnlock(NNshGlobal->logPrefixH);
        }
        subjDb->msgState = FILEMGR_STATE_OPENED_VFS; 

        // CLIE4�p�s�����t���O�������Ă��邩�`�F�b�N����
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND) != 0)
        {
            workAround = true;
        }       
    }
    else
    {
        subjDb->msgState = FILEMGR_STATE_OPENED_STREAM;        
    }
    if (StrStr(dbData->dataTitlePrefix, "%t") == NULL)
    {
        StrCopy(subjDb->threadTitle, dbData->dataTitlePrefix);
    }
    else
    {
        //  �Ƃ肠�����A�P���ł͂Ȃ��ꍇ�A"%t"���X���^�C�w�b�_��
        // �܂܂�Ă���Ƃ��́A���̑O�܂ł̕�������w�b�_�ɓ����
        chk = dbData->dataTitlePrefix;
        ptr = subjDb->threadTitle;
        while ((*chk != '\0')&&
                ((*chk != '%')||(*(chk + 1) != 't')))
        {
            *ptr = *chk;
            ptr++;
            chk++;
        }
        *ptr = '\0';
    }
    StrCopy(subjDb->boardNick, OFFLINE_THREAD_NICK);
    subjDb->state = NNSH_SUBJSTATUS_UNKNOWN;

    // ���ݎ������擾
    currentTime = TimGetSeconds();

    // ���O�t�@�C�����̍\�z
    file = &fileName[StrLen(fileName)];
    if (StrLen(subjDb->threadFileName) != 0)
    {
        // �����Ƀt�@�C�������w�肳��Ă����ꍇ(�����̃t�@�C�����𗘗p����)
        StrCat(fileName, subjDb->threadFileName);
    }
    else
    {
        // �����Ƀt�@�C�������w�肳��Ă��Ȃ������ꍇ(�����I�Ƀt�@�C�����𐶐�)
        StrCat (fileName, FILE_LOGCHARGE_PREFIX);
        NUMCATI(fileName, recNum);
        StrCat (fileName, "-");
        NUMCATI(fileName, depth);
        StrCat (fileName, "-");
        NUMCATI(fileName, number);
        if ((depth != 0)&&(dbData->getAfterHours != 0))
        {
            //  depth�w�肳��Ă����ꍇ�ł��An���Ԍ�Ď擾�̎w�肪����Ă���
            // �ꍇ�A�t�@�C�����ɓ�����16�i���ŕt������
            // (2005�N1��1������̌o�ߕb����t������)
            StrCat (fileName, "-");
            NUMCATH(fileName, (currentTime 
                           - TIME_CONVERT_1904_1970 - TIME_CONVERT_1970_2005));
        }
        StrCat (fileName, FILE_LOGCHARGE_SUFFIX);    
        StrNCopy(subjDb->threadFileName, file, MAX_THREADFILENAME);
    }

    redirectCnt = 0;

    // �擾�ς݃��O�����݂��邩�ǂ����̃`�F�b�N
    // (�X���f�[�^�̂c�a�́A�t�@�C�����ł̃\�[�g�����ĂȂ��̂œ����Ȃ��A�A�A)
    if (IsExistRecordRR_NNsh(subjRef, subjDb->threadFileName, NNSH_KEYTYPE_CHAR, 0, &index) == errNone)
    {
        // �f�[�^�����ɂ���A�f�[�^�̍X�V�`�F�b�N�ɐ؂�ւ���
        if (dbData->getDateTime[0] != '\0')
        {
            StrCopy(dateBuf, "If-Modified-Since: ");
            StrCat (dateBuf, dbData->getDateTime);
        }
        updateRecord = true;   // �擾�ς݃��O�����݂���
    }
    else
    {
        // �擾�ς݃��O�����݂��Ȃ�
        updateRecord = false;

        // �K�w���ł͂Ȃ��A���X�V�����������Ă����ꍇ...
        if ((depth == 0)&&(dbData->lastUpdate != 0))
        {
            // ���ݎ������擾
            currentTime = TimGetSeconds();
            currentTime = currentTime - dbData->lastUpdate;
            dummy       = ((UInt32) dbData->getAfterHours) * 60 * 60;
            if ((dbData->getAfterHours == MAX_GETLOG_REGETHOUR)||
                 (currentTime < dummy))
            {
                // �Ď擾���Ԃɓ��B���Ă��Ȃ��̂ŁA�X�V�`�F�b�N�ɐ؂�ւ���
                if (dbData->getDateTime[0] != '\0')
                {
                    StrCopy(dateBuf, "If-Modified-Since: ");
                    StrCat (dateBuf, dbData->getDateTime);
                }
            }
        }
    }

    // �u�P�����擾�v�ŁA�u�����ԍX�V�m�F�v���w�肳��Ă����Ƃ�...
    if ((depth != 0)&&(dbData->getAfterHours != 0))
    {
        // �w�肳�ꂽ�t�q�k���擾�ς݂��ǂ����m�F����
        subIndex = GetSubFileDatabase(NNSH_GETLOGDB_CHECKURL, 0, url, subDbData);
        if (subIndex != NNSH_DATABASE_BLANK)
        {
            // �w�肳�ꂽURL�͎擾�ς�...
            if (subDbData->lastUpdate != 0)
            {
                currentTime = TimGetSeconds();
                currentTime = currentTime - dbData->lastUpdate;
                dummy       = ((UInt32) dbData->getAfterHours) * 60 * 60;
                if ((dbData->getAfterHours == MAX_GETLOG_REGETHOUR)||
                     (currentTime < dummy))
                {
                    // �Ď擾���Ԃɓ��B���Ă��Ȃ��A�A�A���O�擾�͍s��Ȃ�
                    ret = NNSH_DATABASE_PENDING;
                    goto FUNC_END;
                }
            }
            // (�Â�)�����O�t�@�C������������...
            if (subDbData->logDbId != 0)
            {
                DeleteThreadMessage_RecordID(subDbData->logDbId);
            }
            subDbData->logDbId = 0;
        }
    }

////////  ���_�C���N�g�w�����������ꍇ�ɂ́A�����ɖ߂��ĂƂ�Ȃ���  ////////
GET_URL_DATA:

    // �f�[�^�̎擾
    // �z�X�g����X���ꗗ�f�[�^���擾
    if (dbData->wwwcMode == 1)
    {
        // �ꕔ(�w�b�_�̂�)�擾
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET_NOTMONA, url, NULL, dateBuf,
                            HTTP_GETLOG_START, HTTP_GETLOG_LEN, NULL);
    }
    else
    {
        // �ʏ�擾���[�h
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET_NOTMONA, url, NULL, dateBuf,
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    }

    if (ret != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,MSG_ERROR_HTTP_COMM, "[logCharge]", ret);
            goto FUNC_END;
        }

        // �ʐM�^�C���A�E�g����
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[logCharge]", ret);
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "logGET URL:%s [DONE]", url);
#endif // #ifdef USE_REPORTER

    // �ǂݏo���t�@�C���̃I�[�v��
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_RECVMSG, ret);
        goto FUNC_END;
    }

    // BUSY�E�B���h�E��\������
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    // �f�[�^�̓ǂݏo��
    readSize = 0;
    MemSet(realBuf, bufSize, 0x00);
    ret  = ReadFile_NNsh(&fileRefR, 0, bufSize, realBuf, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read Failure :", FILE_RECVMSG, ret);
        goto CLOSE_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "++++++++++HTTP MESSAGE++++++++++");
    HostTraceOutputTL(appErrorClass, "%s", realBuf);
    HostTraceOutputTL(appErrorClass, "++++++++++++++++++++++++++++++++");
#endif // #ifdef USE_REPORTER
    
    // if ((StrNCompare(realBuf,"HTTP/1.1 3", 10) == 0)||
    //     (StrNCompare(realBuf,  "HTTP/1.0 3", 10) == 0))
    if ((redirectCnt < MAX_REDIRECT_COUNT)&&(((realBuf[0] == 'H')&&
        (realBuf[1] == 'T')&&(realBuf[2] == 'T')&&
        (realBuf[3] == 'P')&&(realBuf[9] == '3'))))
    {
        // �����R�[�h(304, Not Modified)�̂Ƃ�...
        if ((realBuf[10] == '0')&&(realBuf[11] == '4'))
        {
            // �f�[�^�̍X�V�͍s�킸�I������B
            NNsh_DebugMessage(ALTID_INFO, "HTTP/1.x 304 Not Modified :", url, 0);
            goto CLOSE_END;
        }

        // �{���Ȃ�A"Location:" ��T���ׂ������A
        ptr = StrStr(realBuf, "http://");
        if (ptr != NULL)
        {
            // ���_�C���N�g����Ă����A�A�A�ʂ�URL����ēx�f�[�^����蒼��
            Hide_BusyForm(false);
            CloseFile_NNsh(&fileRefR);
        
            chk = ptr;
            // �f�[�^�̖�����T��
            while ((*chk != 0x0d)&&(chk < ptr + (MAX_GETLOG_URL - 1)))
            {
                chk++;
            }
            *chk = 0x00;

            // �V����URL���R�s�[����
            MemSet(url, MAX_GETLOG_URL, 0x00);
            StrNCopy(url, ptr, MAX_GETLOG_URL);

            redirectCnt++;

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "REDIRECT(%d) TO URL:%s ", redirectCnt, url);
#endif // #ifdef USE_REPORTER

            // �f�[�^�̍ēx��蒼��
            goto GET_URL_DATA;
        }
        
        // "Location:" ��T���A�A�A
        ptr = StrStr(realBuf, "Location");
        if (ptr != NULL)
        {
            // ���_�C���N�g����Ă����A�A�A�ʂ�URL����ēx�f�[�^����蒼��
            Hide_BusyForm(false);
            CloseFile_NNsh(&fileRefR);
            
            // �f�[�^��ǂݔ�΂�...
            ptr = ptr + sizeof("Location") - 1;
            while ((*ptr == ':')||(*ptr == ' '))
            {
                ptr++;   
            }

            chk = ptr;
            // �f�[�^�̖�����T��
            while ((*chk != 0x0d)&&(chk < ptr + (MAX_GETLOG_URL - 1)))
            {
                chk++;
            }
            *chk = 0x00;

            // �V����URL���R�s�[����...���AURL�����Ύw�肳��Ă���B�B
            //   (�؂�� ( http://host.name/ )�����c���悤�ɂ���)
            chk = StrStr(url, "://");
            if (chk != NULL)
            {
                chk = chk + sizeof("://") - 1;
                while ((*chk != '/')&&(*chk != '\0'))
                {
                    chk++;
                }
                *chk = 0x00;
                dummy = MAX_GETLOG_URL - StrLen(url);
            }
            else
            {
                chk = url;
                dummy = MAX_GETLOG_URL;
            }
            StrNCopy(chk, ptr, dummy);

            redirectCnt++;

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "REDIRECT(%d) TO URL:%s ", redirectCnt, url);
#endif // #ifdef USE_REPORTER

            // �f�[�^�̍ēx��蒼��
            goto GET_URL_DATA;        
        } 
    }

    // �T�[�o�̍X�V�������擾���i�[����
    chk = "Last-modified";
    ptr = StrStr(realBuf, chk);
    if (ptr == NULL)
    {
        chk = "Last-Modified";
        ptr = StrStr(realBuf, chk);
        if (ptr == NULL)
        {
            // �T�[�o�̑��M�������擾���i�[����
            chk = "Date";
            ptr = StrStr(realBuf, chk);
        }
    }
    if ((depth == 0)&&((dateTime != NULL)&&(ptr != NULL)))
    {
        ptr = ptr + StrLen(chk);
        while ((*ptr != '\x0d')&&((*ptr == ':')||(*ptr <= ' ')))
        {
            ptr++;
        }
        dummy = 0;
        while ((dummy < MAX_GETLOG_DATETIME)&&
                (*(ptr + dummy) != '\x0d')&&
                (*(ptr + dummy) != '\x0a')&&
                (*(ptr + dummy) != '\0'))
        {
            dummy++;
        }
        MemSet(dateTime, MAX_GETLOG_DATETIME, 0x00);
        StrNCopy(dateTime, ptr, dummy);

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "DateTime>>>%s", dateTime);
#endif // #ifdef USE_REPORTER
    }

    if (dbData->wwwcMode == 1)
    {
        // �X�V�`�F�b�N�I���A�����Ő܂�Ԃ�...
        // (subjDB������Ȃ�...�����̂��A����ŁH)
        NNsh_DebugMessage(ALTID_INFO, "SITE UPDATE :", url, 0);
        if (updateRecord == true)
        {
            // ���Ƀ��O������ꍇ�A�x�����b�Z�[�W���o�������ɂ���
            NNsh_DebugMessage(ALTID_WARN, "LOG DATA EXIST, but... ", "", 0);
        }
        goto CLOSE_END;
    }

    // �G���R�[�h�̃`�F�b�N(JPEG������M�\�ɂ��邩�H)
    // ptr = StrStr(realBuf, "image/jpeg");
    ptr = StrStr(realBuf, "image/");
    if (ptr != NULL)
    {
        file = StrStr(realBuf, NNSH_HTTP_HEADER_ENDTOKEN);
        if ((file != NULL)&&(ptr < file))
        {
            // JPEG�t�@�C������M�I
            file       = file + 4;  // �g�[�N�����ǂݔ�΂�...
            offset     = (UInt32) (file - realBuf);
            kanjiCode  = NNSH_KANJICODE_NOT_TEXT;

            // �t�@�C���������H���� ("/Palm/Images"�f�B���N�g���ɕۑ�)
            ptr = fileName + StrLen(fileName);
            while ((ptr > fileName)&&(*ptr != '/'))
            {
                ptr--;
            }
            dummy = StrLen(ptr) + 1;  // +1 �́A'\0'�����킹�Ĉړ������邽��

            // �C���[�W�ۑ��p�̃f�B���N�g����(�ꉞ�A�R�s�[�ł���悤��)�쐬����
            if (subjDb->msgState == FILEMGR_STATE_OPENED_VFS)
            {
                // VFS�ɃR�s�[����Ƃ��̃f�B���N�g���w��...
                MemMove(&fileName[12], ptr, dummy); // 12��"/Palm/Images"�̒���
                StrCopy(fileName, "/Palm/Image");
                fileName[11] = 's';   // StrCopy�̎d�l��Y�ꂽ�̂ŁA�A�A���S��
                CreateDir_NNsh("/Palm/Images");
            }
            else
            {
                // file stream�Ɏ�M����Ƃ��́A�t�@�C���������ɂ��Ă��܂�
                MemMove(fileName, ptr, dummy);

                // �t�@�C�����̃T�C�Y�������ꍇ�ɂ͐؂�B
                fileName[MAX_DBNAMELEN - 1] = '\0';
            }
            redirectCnt = subjDb->msgState;

            // �t�@�C���o�̓��W�b�N��...
            goto OUTPUT_DATAFILE;
        }
    }

    // zip�t�@�C������M�A�܂��͋����I�ɃC���[�W��M���w������...
    ptr = StrStr(realBuf, "application/zip");
    if ((ptr != NULL)||(isImage == true))
    {
        file = StrStr(realBuf, NNSH_HTTP_HEADER_ENDTOKEN);
        if ((file != NULL)&&(ptr < file))
        {
            file        = file + 4;  // �g�[�N�����ǂݔ�΂�...
            offset      = (UInt32) (file - realBuf);
            redirectCnt = FILEMGR_STATE_DISABLED;
            kanjiCode   = NNSH_KANJICODE_NOT_TEXT;

            // �t�@�C���o�̓��W�b�N��...
            goto OUTPUT_DATAFILE;
        }
    }

    // �����R�[�h�̃`�F�b�N
    ptr = StrStr(realBuf, "charset=");
    dummy = StrLen("charset=");
    if (ptr == NULL)
    {
        // charset��������Ȃ��ꍇ�ɂ́Aencoding���g�p����
        ptr   = StrStr(realBuf, "encoding=");
        dummy = StrLen("encoding=");
    }
    if (ptr != NULL)
    {
        // �������ǂݔ�΂�
        ptr = ptr + dummy;
        
        // �󔒕�����ǂݔ�΂�
        while ((*ptr == ' ')||(*ptr == '\t')||(*ptr == '"'))
        {
            ptr++;
        }

        if (StrNCaselessCompare(ptr, "iso-2022-jp", StrLen("iso-2022-jp")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_JIS;
        }
        else if (StrNCaselessCompare(ptr, "euc-jp", StrLen("euc-jp")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_EUC;
        }
        else if (StrNCaselessCompare(ptr, "x-euc-jp", StrLen("x-euc-jp")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_EUC;
        }
        else if (StrNCaselessCompare(ptr, "shift_jis", StrLen("shift_jis")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        }
        else if (StrNCaselessCompare(ptr, "x-sjis", StrLen("x-sjis")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        }
        else if (StrNCaselessCompare(ptr, "shift-jis", StrLen("shift-jis")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        }
        else if (StrNCaselessCompare(ptr, "utf-8", StrLen("utf-8")) == 0)
        {
            kanjiCode = NNSH_KANJICODE_UTF8;
        }
    }

    // RSS�Ƀ`�F�b�N�������Ă����ꍇ�ɂ́A(�����I��)UTF-8�Ƃ���
    // (�������A�ŏ�ʂ̊K�w����...)
    if ((dbData->rssData != 0)&&(depth == 0))
    {
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "kanji :%d, utf-8:%d", 
                                               kanjiCode, NNSH_KANJICODE_UTF8);
#endif // #ifdef USE_REPORTER
        kanjiCode = NNSH_KANJICODE_UTF8;
    }

    // ���f�[�^�̐擪��T��
    ptr = StrStr(realBuf, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // �擪���Ȃ���΁A�f�[�^�u���b�N�̍ŏ������͂���
        NNsh_DebugMessage(ALTID_ERROR, "Data Failure :", FILE_RECVMSG, ret);
        ptr = realBuf;
    }

    // �R�s�[�f�[�^�̕␳����
    //     ptr���f�[�^�{�f�B�̐擪�Ɏ����Ă���B
    //     offset��ǂݏo���t�@�C���̐擪����̃I�t�Z�b�g�������Ă���B
    //     (offset�́A�t�@�C���̏����o���̐擪�����߂�Ƃ��ɗ��p����)
    ptr = ptr + 4;   // 4�́AStrLen(NNSH_HTTP_HEADER_ENDTOKEN) �Ɠ����B
    offset = (UInt32) (ptr - realBuf);

    // ��ʂ���^�C�g�������w�肳��Ă��邩�H
    if (titlePrefix != NULL)
    {
        // �^�C�g�����̂��w�肳��Ă����ꍇ�ɂ́A����𗘗p����
        file = titlePrefix;
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "##### Title : %s", file);
#endif // #ifdef USE_REPORTER
        goto SET_TITLE_STRING;
    }

    // �^�C�g���̎擾
    file = StrStr(ptr, "<title");
    if (file == NULL)
    {
        // �^�C�g���^�O���������łȂ������ꍇ...
        file = StrStr(ptr, "<TITLE");
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "<TITLE(kanji:%d)>", kanjiCode);
#endif // #ifdef USE_REPORTER

    if (file != NULL)
    {
        // �^�O�̖�����T��...
        file = file + StrLen("<TITLE");
        while (*file != '>')
        {
            file++;
        }
        file++;

        // �^�C�g���f�[�^�̖�����T��
        chk = StrStr(file, "</TITLE");
        if (chk == NULL)
        {
            chk = StrStr(file, "</title");
        }
        if (chk == NULL)
        {
            chk = file;
            *chk = ' ';
            chk++;
        }
        *chk = '\0';

SET_TITLE_STRING:
        // �X���^�C�i�[�̈�Ƀf�[�^���i�[(�����R�[�h�̕ϊ����Y��Ȃ��I)
        dummy = StrLen(subjDb->threadTitle);
        titleSize = StrLen(file);

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "</TITLE(len:%lu, size:%d)>", dummy, titleSize);
#endif // #ifdef USE_REPORTER

        switch (kanjiCode)
        {
          case NNSH_KANJICODE_EUC:
            // EUC -> SJ�ϊ����Ċi�[
            chk = MEMALLOC_PTR((titleSize * 2) + MARGIN);
            if (chk != NULL)
            {
                StrCopySJ(chk, file);
                StrNCopy(&subjDb->threadTitle[dummy], chk, MAX_THREADNAME - dummy - 1);
                MEMFREE_PTR(chk);
            }
            break;

          case NNSH_KANJICODE_JIS:
            // JIS -> SJ�ϊ����Ċi�[(�����JIS...)
            chk = MEMALLOC_PTR((titleSize * 2) + MARGIN);
            if (chk != NULL)
            {
                StrCopyJIStoSJ(chk, file);
                StrNCopy(&subjDb->threadTitle[dummy], chk, MAX_THREADNAME - dummy - 1);
                MEMFREE_PTR(chk);
            }
             break;

          case NNSH_KANJICODE_UTF8:
            chk = MEMALLOC_PTR((titleSize * 2) + MARGIN);
            if (chk != NULL)
            {
                StrNCopyUTF8toSJ(chk, file, (titleSize * 2));
                StrNCopy(&subjDb->threadTitle[dummy], chk, MAX_THREADNAME - dummy - 1);
                MEMFREE_PTR(chk);
            }
            else
            {
                // UTF8�̕�����ϊ����ł��Ȃ������ꍇ...
                StrNCopy(&subjDb->threadTitle[dummy], file, MAX_THREADNAME - dummy - 1);
            }
            break;                

          case NNSH_KANJICODE_SHIFTJIS:
          default:
            // ���̂܂܊i�[
            StrNCopy(&subjDb->threadTitle[dummy], file, MAX_THREADNAME - dummy - 1);
            break;
        }
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "TITLE : %s ", subjDb->threadTitle);
#endif // #ifdef USE_REPORTER

OUTPUT_DATAFILE:
    // �������݃t�@�C���̃I�[�v��
    ret = OpenFile_NNsh(fileName, NNSH_FILEMODE_CREATE, &fileRefW);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Write File Open :", fileName, ret);
        goto CLOSE_END;
    }

    // ����ɓǂݍ��߂��Ƃ��́A�f�[�^�̃{�f�B�������t�@�C���ɃR�s�[����
    ret = errNone;
    offsetW = 0;
    while (ret == errNone)
    {
        readSize = 0;
        MemSet(realBuf, bufSize, 0x00);
        ret  = ReadFile_NNsh(&fileRefR, offset, bufSize, realBuf, &readSize);

        offset  = offset + readSize;
        if ((readSize % 2) != 0)
        {
            // �������݃T�C�Y�������ɕ␳����(��CLIE4�p�̕ی�)
            if (workAround == true)
            {
                realBuf[readSize] = ' ';
                readSize++;
            }
        }
        (void) WriteFile_NNsh(&fileRefW, offsetW, readSize, realBuf, &dummy);
        offsetW = offsetW + readSize;
    }

    // ������url���L�^����(�e�L�X�g�t�@�C���̂Ƃ��̂�)
    if (kanjiCode != NNSH_KANJICODE_NOT_TEXT)
    {
        MemSet (realBuf, bufSize, 0x00);
        StrCopy(realBuf, "<br><hr><a href=\"");
        StrCat (realBuf, url);
        StrCat (realBuf, "\">");
        StrCat (realBuf, url);
        StrCat (realBuf, "</a>");
    }

    readSize = StrLen(realBuf);
    if ((readSize % 2) != 0)
    {
        // �������݃T�C�Y�������ɕ␳����(��CLIE4�p�̕ی�)
        if (workAround == true)
        {
            realBuf[readSize] = ' ';
            readSize++;
        }
    }
    (void) WriteFile_NNsh(&fileRefW, offsetW, readSize, realBuf, &dummy);
    offsetW = offsetW + readSize;

    CloseFile_NNsh(&fileRefW);

    ////////// �擾�ネ�O���H �o  //////////
    if (dbData->wwwcMode == 2)
    {
        // �擾�ネ�O�̊K�w�`�F�b�N
        if (depth != 0)
        {
            // �P���̃��O�̂Ƃ�...
            (void) TrimGetDataLog(fileName, url, dbData->subTokenId, realBuf, bufSize);
        }
        else
        {
            // �ʏ�̃��O�̂Ƃ�...
            (void) TrimGetDataLog(fileName, url, dbData->tokenId, realBuf, bufSize);
        }
    }
    ////////// �p �擾�ネ�O���H  //////////

    // �C���[�W�t�@�C����M���ɂ́A�t�@�C���N���[�Y������
    // (DB�̍X�V���������{���Ȃ�...)
    if (kanjiCode == NNSH_KANJICODE_NOT_TEXT)
    {
        goto CLOSE_END;
    }

    // ���O�t�@�C���T�C�Y���L�^
    subjDb->fileSize = offsetW;
    
    // BBS�^�C�v�Ɋ����R�[�h��ǉ�����
    subjDb->bbsType = NNSH_BBSTYPE_HTML | (kanjiCode << NNSH_BBSTYPE_CHAR_SHIFT);

    // �V���X���Ƃ��čX�V����
    subjDb->state = NNSH_SUBJSTATUS_NEW;

    // �X���f�[�^���o�^�ς݂̏ꍇ�A��x�f�[�^���폜����
    // (�X���f�[�^�̂c�a�́A�t�@�C�����ł̃\�[�g�����ĂȂ��̂œ����Ȃ��A�A�A)
    if (updateRecord == true)
    {
        // �f�[�^�����ɂ���A�f�[�^���X�V����
        ret = UpdateRecord_NNsh(subjRef, index, sizeof(NNshSubjectDatabase), subjDb);
    }
    else
    {
        // �f�[�^���Ȃ��A�f�[�^��o�^����
        ret = EntryRecord_NNsh(subjRef, sizeof(NNshSubjectDatabase), subjDb);
        index = 0;
    }

    // �u�P�����擾�v�ŁA�u�����ԍX�V�m�F�v���w�肳��Ă����Ƃ�...
    if ((depth != 0)&&(dbData->getAfterHours != 0))
    {
        // �T�uDB��o�^����
        // URL���L�q����Ă��邩�H (�f�[�^���o�^�ς݂��ǂ����m�F����)
        if (subIndex == NNSH_DATABASE_BLANK)
        {
            // �V�K�o�^���K�v...
            StrNCopy(subDbData->dataURL, url, MAX_GETLOG_URL);
            StrNCopy(subDbData->dataFileName, subjDb->threadFileName, MAX_THREADFILENAME);
            subDbData->lastUpdate = TimGetSeconds();
            subDbData->parentDbId = recId;

            // ���O�f�[�^�̃��R�[�hID���擾����
            if (GetRecordID_NNsh(subjRef, index, &(subDbData->logDbId)) != errNone)
            {
                subDbData->logDbId = 0;
                NNsh_DebugMessage(ALTID_ERROR, "Cannot get record id ", " ", 0);
            }
            EntryLogSubFileDatabase(subDbData);
        }
        else
        {
            // (�Â�)�����O�t�@�C������������...
            if (subDbData->logDbId != 0)
            {
                DeleteThreadMessage_RecordID(subDbData->logDbId);
            }
            // ���O�f�[�^�̃��R�[�hID���擾����
            if (GetRecordID_NNsh(subjRef, index, &(subDbData->logDbId)) != errNone)
            {
                subDbData->logDbId = 0;
                NNsh_DebugMessage(ALTID_ERROR, "cannot get record id ", " ", 0);
            }

            // ���R�[�h�X�V�������K�v(���������X�V)
            subDbData->lastUpdate = TimGetSeconds();
            subDbData->parentDbId = recId;
            UpdateSubFileDatabase(subIndex, subDbData);
            
        }
    }

CLOSE_END:
    // �t�@�C���̃N���[�Y
    Hide_BusyForm(false);
    CloseFile_NNsh(&fileRefR);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "CLOSE END>>ReadOnlyLogDataPickup()");
#endif // #ifdef USE_REPORTER

FUNC_END:
    // �f�[�^�x�[�X�̃N���[�Y
    CloseDatabase_NNsh(subjRef);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "FUNC END>>ReadOnlyLogDataPickup"
                      "(subjDB:%x, dateBuf:%x, realBuf:%x)",
                      &subjDb, &dateBuf, &realBuf);
#endif // #ifdef USE_REPORTER

    // �������J��...
    MEMFREE_PTR(realBuf);
    MEMFREE_PTR(subDbData);
    MEMFREE_PTR(dateBuf);
    MEMFREE_PTR(subjDb);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "GET URL:%s DONE.", url);
#endif // #ifdef USE_REPORTER

    // �C���[�W�t�@�C����M���ɂ́ADA�N���������s��...
    if (kanjiCode == NNSH_KANJICODE_NOT_TEXT)
    {
        if (redirectCnt != FILEMGR_STATE_DISABLED)
        {
            // JPEG�t�@�C������M�����ꍇ�ADA�N�����s���B
            launch_viewJPEG(redirectCnt, fileName);
        }
        else
        {
            // �t�@�C����M�񍐂��s��
            NNsh_WarningMessage(ALTID_WARN, fileName, MSG_RECEIVE_IMAGEFILE, 0);
        }
    }
    return (ret);
}


/*==========================================================================*/
/*  TrimGetDataLog() : �Q�ƃ��O�̃��O�f�[�^�����H����                       */
/*                                                                          */
/*==========================================================================*/
Err TrimGetDataLog(Char *fileName, Char *url, UInt32 tokenId, Char *realBuf, UInt16 bufSize)
{
    Err                   ret;
    NNshFileRef           fileRefR, fileRefW;
    NNshLogTokenDatabase *tokenData;
    UInt32                offsetW, offsetR, readSize, dummy;
    Char                 *ptr, status;

    // ���݂̃��O����UPalm���փR�s�[����
    ret = CopyFile_NNsh(FILE_SENDMSG, fileName, NNSH_COPY_VFS_TO_PALM);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "CopyFile_NNsh()", " :", ret);
        return (ret);
    }

    // ��؂�f�[�^�x�[�X�̃f�[�^�̈���m��
    tokenData = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (tokenData == NULL)
    {
        // ��؂�f�[�^�x�[�X�̗̈�m�ۂɎ��s...
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN)", " :", ret);
        return (~errNone);
    }

    // ��؂�f�[�^�x�[�X�̃f�[�^���擾����
    ret = GetLogToken(tokenId, tokenData);
    if (ret != errNone)
    {
        // ��؂�f�[�^�x�[�X�̎擾�Ɏ��s...
        NNsh_DebugMessage(ALTID_ERROR, "GetLogToken(tokenId, tokenData)", " :", ret);
        MEMFREE_PTR(tokenData);
        return (ret);
    }
    NNsh_DebugMessage(ALTID_INFO, "tokenData :", tokenData->startToken, 0);

    // �������݃t�@�C���̃I�[�v��
    ret = OpenFile_NNsh(fileName, NNSH_FILEMODE_CREATE, &fileRefW);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Write File Open :", fileName, ret);
        MEMFREE_PTR(tokenData);
        (void) CopyFile_NNsh(fileName, FILE_SENDMSG, NNSH_COPY_PALM_TO_VFS);
        return (ret);
    }

    // �ǂݍ��݃t�@�C���I�[�v��
    ret = OpenFile_NNsh(FILE_SENDMSG, (NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE), &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_SENDMSG, ret);
        MEMFREE_PTR(tokenData);
        (void) CopyFile_NNsh(fileName, FILE_SENDMSG, NNSH_COPY_PALM_TO_VFS);
        return (ret);
    }

    // ����ɓǂݍ��߂��Ƃ��́A�t�@�C�����R�s�[����
    status  = NNSH_COPYSTATUS_SEARCHTOP;
    offsetW = 0;
    offsetR = 0;
    while (ret == errNone)
    {
        readSize = 0;
        MemSet(realBuf, bufSize, 0x00);
        ret  = ReadFile_NNsh (&fileRefR, offsetR, bufSize, realBuf, &readSize);
        switch (status)
        {        
          case NNSH_COPYSTATUS_SEARCHTOP:
            ptr = StrStr(realBuf, tokenData->startToken);
            if (ptr != NULL)
            {
                status = NNSH_COPYSTATUS_COPYING;
                offsetR = offsetR + (ptr - realBuf);

                NNsh_DebugMessage(ALTID_INFO, "tokenData :", tokenData->startToken, (offsetR));
                ret = errNone;
            }
            else
            {
                // ���̓ǂݏo���ʒu�𒲐�����
                offsetR = offsetR + readSize;
                dummy = StrLen(tokenData->startToken);
                if ((dummy % 2) != 0)
                {
                    dummy = dummy + 1;
                }
                if (readSize > dummy)
                {
                    offsetR = offsetR - dummy;
                }
            }
            break;

          case NNSH_COPYSTATUS_COPYING:
            // ���H�������O���R�s�[����...
            ptr = StrStr(realBuf, tokenData->endToken);
            if (ptr != NULL)
            {
                status = NNSH_COPYSTATUS_COPYDONE;
                readSize = (ptr - realBuf) + StrLen(tokenData->endToken) + 1;
            }
            if ((readSize % 2) != 0)
            {
                // �������݃T�C�Y�������ɕ␳����(��CLIE�p�̕ی�)
                if ((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)&&
                    (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND) != 0))
                {
                    realBuf[readSize] = ' ';
                    readSize++;
                }
            }
            (void) WriteFile_NNsh(&fileRefW, offsetW, readSize, realBuf, &dummy);
            offsetW = offsetW + readSize;
            break;

          case NNSH_COPYSTATUS_COPYDONE:
          default:
            // �������Ȃ�... (�R�s�[�I���Ȃ̂�)
            ret = fileErrEOF;
            break;
        }
    }

    // �I�[�v�������t�@�C�������...
    CloseFile_NNsh(&fileRefW);
    CloseFile_NNsh(&fileRefR);

    MEMFREE_PTR(tokenData);

    // �t�@�C���̍Ō�܂ŃR�s�[��������errNone�ŉ�������
    if ((ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        ret = errNone;
    }
    return (ret);
}

#endif    // #ifdef USE_LOGCHARGE
