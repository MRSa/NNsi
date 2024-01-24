/*============================================================================*
 *  FILE: 
 *     nnshset.c
 *
 *  Description: 
 *     NNsh parameter setting functions for NNsh.
 *
 *===========================================================================*/
#define NNSHSET_C
#define GLOBAL_REAL
#include "local.h"

extern Err setOfflineLogDir(Char *fileName);

/*=========================================================================*/
/*   Function :   SetControlValue                                          */
/*                                       �p�����[�^�����ʂɔ��f���鏈��  */
/*=========================================================================*/
void SetControlValue(FormType *frm, UInt16 objID, UInt16 *value)
{
    ControlType *chkObj;

    chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objID));
    CtlSetValue(chkObj, (UInt16) *value);

    return;
}

/*=========================================================================*/
/*   Function :   UpdateParameter                                          */
/*                                       ��ʂ���p�����[�^�ɔ��f���鏈��  */
/*=========================================================================*/
void UpdateParameter(FormType *frm, UInt16 objID, UInt16 *value)
{
    ControlType *chkObj;

    chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objID));
    *value = CtlGetValue(chkObj);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting7                                       */
/*                              NNsi�ݒ�-7(�f�o�C�X�֘A)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting7(FormType *frm)
{
    ControlType *chkObj, *chkObjSub;
    UInt16       butID;

#ifdef USE_TSPATCH
    Err        ret;
    UInt32     fontVer;

    // TsPatch�@�\���g�p���Ȃ�
    ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
    if (ret == errNone)
    {    
        UpdateParameter(frm, CHKID_NOTUSE_TSPATCH, &(butID));
        if (butID != 0)
        {
            (NNshGlobal->NNsiParam)->notUseTsPatch = 1;
        }
        else
        {
            (NNshGlobal->NNsiParam)->notUseTsPatch = 0;
        }
    }
    else
#endif
    {
        (NNshGlobal->NNsiParam)->notUseTsPatch = 0;
    }


    // ARMlet���g�p����
    UpdateParameter(frm, CHKID_USE_ARMLET, &((NNshGlobal->NNsiParam)->useARMlet));
    if ((NNshGlobal->NNsiParam)->useARMlet != 0)
    {
        // ArmLet��PalmOS5��p�̋@�\�Ȃ��߁A�{���Ɏ��{���邩�m�F��\������)
        butID = NNsh_WarningMessage(ALTID_CONFIRM, MSG_WARNING_OS5, "", 0);
        if (butID != 0)
        {
            (NNshGlobal->NNsiParam)->useARMlet = 0;
        }
    }

#ifdef USE_CLIE
    // SONY�n�C���]�@�\���g��Ȃ�
    UpdateParameter(frm, CHKID_NOTUSE_SONYHIRES, &((NNshGlobal->NNsiParam)->disableSonyHR));
    if (NNshGlobal->notUseHR != (NNshGlobal->NNsiParam)->disableSonyHR)
    {
        // SONY�n�C���]�@�\�̎g�p�ېݒ�́A����N�������L���ƂȂ�
        NNsh_WarningMessage(ALTID_WARN, MSG_HR_NEXT_LAUNCH, "", 0);
    }

    // Silk������s��Ȃ�
    UpdateParameter(frm, CHKID_NOTUSE_SILK, &NNshGlobal->notUseSilk);
    if (NNshGlobal->notUseSilk != (NNshGlobal->NNsiParam)->notUseSilk)
    {
        // SILK����͍s��Ȃ��Ƃ��́A�x����\������
        NNsh_WarningMessage(ALTID_WARN, MSG_SILK_NEXT_LAUNCH, "", 0);
    }
#endif

    // �g�pVFS�w��
    butID  = LstGetSelection(FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, LSTID_VFS_LOCATION)));
    (NNshGlobal->NNsiParam)->vfsUseCompactFlash = (UInt8) butID;

    // VFS ���p�\���Ɏ����I��VFS ON
    butID = 0;
    UpdateParameter(frm, CHKID_VFSON_AUTOMATIC, &butID);
    (NNshGlobal->NNsiParam)->vfsOnAutomatic = (UInt8) butID;

    // ����VFS ON�ł�DB�`�F�b�N�Ȃ�
    UpdateParameter(frm, CHKID_VFSON_NOTDBCHK, &((NNshGlobal->NNsiParam)->vfsOnNotDBCheck));

    // VFS�̎g�p�Ɖ���t���O�AOFFLINE���O�g�p�t���O�̐ݒ�
    if ((NNshGlobal->NNsiParam)->useVFS != NNSH_NOTSUPPORT_VFS)
    {
        chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_USE_VFS));
        (NNshGlobal->NNsiParam)->useVFS = CtlGetValue(chkObj);

        chkObjSub = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_WORKAROUND;
        }

        chkObjSub = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_USEOFFLINE;
        }

        chkObjSub =
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_DBBACKUP;
        }

        chkObjSub =
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_DBIMPORT;
        }
        
        chkObjSub =
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_DIROFFLINE;
        }

        if ((NNshGlobal->NNsiParam)->useVFS != 0)
        {
            //  VFS�֘A�p�����[�^�̐ݒ肪����Ă����Ƃ��A����N�������
            // �L���ƂȂ邱�Ƃ�\������B
            NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_VFSWARN, "", 0);

            // �f�t�H���g�̐ݒ�l���X�V����
            (NNshGlobal->NNsiParam)->vfsOnDefault
                                             = (NNshGlobal->NNsiParam)->useVFS;
        }

        // logdir.txt��ǂ݁AOFFLINE���O�x�[�X�f�B���N�g�����擾
        (void) setOfflineLogDir(LOGDIR_FILE);
    }

    // DB���X�V�������邵������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting8                                       */
/*                                  NNsi�ݒ�-8(�ʐM�֘A)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting8(FormType *frm)
{
    FieldType   *fldP;
    UInt32       bufSize;
    UInt16       timeout;
    Char        *logBuf, *numP;
    MemHandle    txtH;


    // GZIP���g�p����ݒ�...
    timeout = 0;
    UpdateParameter(frm, CHKID_USE_GZIP, &(timeout));
    (NNshGlobal->NNsiParam)->useGZIP = timeout;

    // �ʐM�^�C���A�E�g�l�̕ύX
    timeout = SysTicksPerSecond();
    timeout = (timeout == 0) ? 1 : timeout;  // �[�����Z�̉��(�ی�)
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_COMM_TIMEOUT));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);

        // �ő厞�Ԃ����傫���ꍇ...
        if (bufSize >= MAX_TIMEOUT_VALUE)
        {
            // �����厞�ԑ҂悤�ɂ���
            (NNshGlobal->NNsiParam)->netTimeout = -1;
        }
        else 
        {
            if ((NNshGlobal->NNsiParam)->netTimeout != timeout * bufSize)
            {
                (NNshGlobal->NNsiParam)->netTimeout = timeout * bufSize;

                // TIMEOUT�̍X�V����\��
                NNsh_DebugMessage(ALTID_INFO, MSG_TIMEOUT_UPDATED, 
                                  "", (NNshGlobal->NNsiParam)->netTimeout);
            }
        }
    }

    // �����o�b�t�@�T�C�Y�̕ύX
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_BUFFER_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);
        if ((bufSize < NNSH_WORKBUF_MIN)||(bufSize > NNSH_WORKBUF_MAX))
        {
            // �f�[�^�l�ُ�A���ُ͔͈͈��\�����Č��ɖ߂�
            logBuf = MEMALLOC_PTR(BUFSIZE);
            if (logBuf != NULL)
            {
                MemSet (logBuf, BUFSIZE, 0x00);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE1);
                NUMCATI(logBuf, bufSize);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE2);
                NUMCATI(logBuf, NNSH_WORKBUF_MIN);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE3);
                NUMCATI(logBuf, NNSH_WORKBUF_MAX);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE4);
                NUMCATI(logBuf, NNSH_WORKBUF_DEFAULT);
                NNsh_ErrorMessage(ALTID_ERROR, logBuf, "", 0);
                MEMFREE_PTR(logBuf);
            }
            return (~errNone);
        }
        else
        {
            if (bufSize == (NNshGlobal->NNsiParam)->bufferSize)
            {
                // �o�b�t�@�T�C�Y�͕ύX���Ȃ������|�񍐂��A�I������
                NNsh_DebugMessage(ALTID_INFO, MSG_WARN_NOTMODIFIED,
                                  "buffer size:", (NNshGlobal->NNsiParam)->bufferSize);
            }
            else
            {
                // �o�b�t�@�T�C�Y��ύX���ďI������
                (NNshGlobal->NNsiParam)->bufferSize = bufSize;
                NNsh_DebugMessage(ALTID_INFO, MSG_INFO_MODIFIED,
                                  "buffer size:", (NNshGlobal->NNsiParam)->bufferSize);
            }
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "0", 0);
    }

    // �����_�E�����[�h�w����擾����
    UpdateParameter(frm, CHKID_DL_PART, &((NNshGlobal->NNsiParam)->enablePartGet));

    // BBS�ꗗ���㏑������
    UpdateParameter(frm, CHKID_BBS_OVERWRITE, &((NNshGlobal->NNsiParam)->bbsOverwrite));

    // �����_�E�����[�h�̃T�C�Y���擾
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_DL_PARTSIZE));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);

        if (bufSize < NNSH_DOWNLOADSIZE_MIN)
        {
            // �_�E�����[�h�T�C�Y�́A�o�b�t�@�T�C�Y��������������
            NNsh_ErrorMessage(ALTID_WARN, MSG_DOWNLOADSIZE_UNDER,
                              "", NNSH_DOWNLOADSIZE_MIN);
            return (~errNone);
        }
        if (bufSize > (NNshGlobal->NNsiParam)->bufferSize)
        {
            // �_�E�����[�h�T�C�Y�́A���[�N�o�b�t�@�T�C�Y����������
            NNsh_ErrorMessage(ALTID_WARN, MSG_DOWNLOADSIZE_OVER, "", 0);
            return (~errNone);
        }
        if (bufSize != (NNshGlobal->NNsiParam)->partGetSize)
        {
            (NNshGlobal->NNsiParam)->partGetSize = bufSize;

            // �_�E�����[�h�T�C�Y�̍X�V��\��
            NNsh_DebugMessage(ALTID_INFO, MSG_DOWNLOADSIZE_UPDATE, "",
                              (NNshGlobal->NNsiParam)->partGetSize);
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "1", 0);
    }

    // BBS�ꗗ�擾��̐ݒ�
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_URL_BBS));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // �t�B�[���h����URL���擾����
        numP = MemHandleLock(txtH);
        MemSet  ((NNshGlobal->NNsiParam)->bbs_URL, MAX_URL, 0x00);
        StrNCopy((NNshGlobal->NNsiParam)->bbs_URL, numP, (MAX_URL - 1));
        MemHandleUnlock(txtH);
    }

    // Proxy�o�R�ł̃A�N�Z�X
    UpdateParameter(frm, CHKID_USE_PROXY, &((NNshGlobal->NNsiParam)->useProxy));

    // Proxy URL
    fldP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, FLDID_USE_PROXY));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP = MemHandleLock(txtH);
        MemSet((NNshGlobal->NNsiParam)->proxyURL, MAX_URL, 0x00);
        if (*numP != '\0')
        {
            StrNCopy((NNshGlobal->NNsiParam)->proxyURL, numP, (MAX_URL - 1));
        }
        MemHandleUnlock(txtH);
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "PROXY URL", 0);
    }

    // Proxy Port #
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_PROXY_PORT));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);

        (NNshGlobal->NNsiParam)->proxyPort = bufSize;
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "PROXY PORT", 0);
    }

    // �ʐM�^�C���A�E�g���̃��g���C��
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_TIMEOUT_RETRY));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP     = MemHandleLock(txtH);
        timeout  = StrAToI(numP);
        MemHandleUnlock(txtH);

        // ���g���C�񐔂��K����傫����΁A�K��l�Ŋۂ߂�
        timeout = (timeout > NNSH_LIMIT_RETRY)? NNSH_LIMIT_RETRY : timeout;
        (NNshGlobal->NNsiParam)->nofRetry = timeout;
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "RETRY COUNT", 0);
    }

    // ZLIB�̃T�|�[�g�ۊm�F���s��
    SetZLIBsupport_NNsh();

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting5                                       */
/*                              NNsi�ݒ�-5(�������݊֘A)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting5(FormType *frm)
{
    UInt16       item;
    UInt32       bufSize;
    FieldType   *fldP;
    Char        *nameP, *logBuf;
    MemHandle    txtH;

    // �������ݎ��o�b�t�@�T�C�Y�̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_WRITEBUF_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // �t�B�[���h���珑�����݃o�b�t�@�T�C�Y���擾����
        nameP = MemHandleLock(txtH);
        if (nameP != NULL)
        {
            bufSize = StrAToI(nameP);
            MemHandleUnlock(txtH);
        }
        else
        {
            bufSize = NNSH_WRITEBUF_DEFAULT;
        }
        if ((bufSize > NNSH_WRITEBUF_MAX)||(bufSize < NNSH_WRITEBUF_MIN))
        {
            // �f�[�^�l�ُ�A���ُ͔͈͈��\�����Č��ɖ߂�
            logBuf = MEMALLOC_PTR(BUFSIZE);
            if (logBuf != NULL)
            {
                MemSet (logBuf, BUFSIZE, 0x00);
                StrCat (logBuf, MSG_WRITEBUF_ILLEGAL);
                NUMCATI(logBuf, bufSize);
                StrCat (logBuf, MSG_WRITEBUF_ILLEGAL2);
                NUMCATI(logBuf, NNSH_WRITEBUF_MIN);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE3);
                NUMCATI(logBuf, NNSH_WRITEBUF_MAX);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE4);
                NUMCATI(logBuf, NNSH_WRITEBUF_DEFAULT);
                NNsh_ErrorMessage(ALTID_ERROR, logBuf, "", 0);
                MEMFREE_PTR(logBuf);
            }
            return (~errNone);
        }
        else
        {
            // �T�C�Y���m�F����
            if (bufSize == (NNshGlobal->NNsiParam)->writeBufferSize)
            {
                // �T�C�Y�͕ύX���Ȃ������|�񍐂��A�I������
                NNsh_DebugMessage(ALTID_INFO, MSG_WARN_NOTMODIFIED,
                                  "writeBufferSize:", (NNshGlobal->NNsiParam)->writeBufferSize);
            }
            else
            {
                (NNshGlobal->NNsiParam)->writeBufferSize = bufSize;
                NNsh_DebugMessage(ALTID_INFO, MSG_INFO_MODIFIED,
                                  "writeBufferSize:", (NNshGlobal->NNsiParam)->writeBufferSize);
            }
        }
    }

    // �������ݎ�PUSH�{�^���𖳌�
    UpdateParameter(frm, CHKID_WRITE_AUTOSAVE,
                                       &((NNshGlobal->NNsiParam)->writeMessageAutoSave));

    // �������ݎ�PUSH�{�^���𖳌�
    item = 0;
    UpdateParameter(frm, CHKID_WRITEPUSH_DISABLE, &(item));
    (NNshGlobal->NNsiParam)->writeJogPushDisable = item;

    // sage��������
    UpdateParameter(frm, CHKID_WRITE_SAGE,  &((NNshGlobal->NNsiParam)->writeAlwaysSage));

    // �R�e�n���@�\���g�p����ݒ�
    UpdateParameter(frm, CHKID_FIXED_HANDLE, &((NNshGlobal->NNsiParam)->useFixedHandle));

    // �n���h�����̐ݒ�(�n���h�������ݒ�ł��Ȃ��Ă����퉞������)
    MemSet((NNshGlobal->NNsiParam)->handleName, BUFSIZE, 0x00);
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_HANDLENAME));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        nameP = MemHandleLock(txtH);
        MemSet((NNshGlobal->NNsiParam)->handleName, BUFSIZE, 0x00);
        if (*nameP != '\0')
        {
            StrNCopy((NNshGlobal->NNsiParam)->handleName, nameP, (BUFSIZE - 1));
        }
        MemHandleUnlock(txtH);
    }

    // ��MCookie���g�p���ď�������
    UpdateParameter(frm, CHKID_WRITE_USE_COOKIE, &((NNshGlobal->NNsiParam)->useCookieWrite));

    // �������ݎ��A�������݃t�B�[���h�Ɏ����I�Ƀt�H�[�J�X���ړ����Ȃ�
    item = 0;
    UpdateParameter(frm, CHKID_NOT_AUTOFOCUSSET, &(item));
    (NNshGlobal->NNsiParam)->notAutoFocus = item;

    // �������ݎ��A�V���N���g�債�Ȃ�
    item = 0;
    UpdateParameter(frm, CHKID_NOTSILK_WRITE, &(item));
    (NNshGlobal->NNsiParam)->notOpenSilkWrite = item;

    // �������ݎ��A�A���_�[���C����OFF�ɂ���
    UpdateParameter(frm, CHKID_UNDERLINE_WRITE, &((NNshGlobal->NNsiParam)->disableUnderlineWrite));

    // �������ݎ����p�J�L�R�ɂ���
    UpdateParameter(frm, CHKID_INSERT_REPLYMSG, &item);
    (NNshGlobal->NNsiParam)->writingReplyMsg = item;    

    // �������ݎ��Q�ƃX���ԍ���}������
    UpdateParameter(frm, CHKID_INSERT_REPLYNUM, &((NNshGlobal->NNsiParam)->insertReplyNum));

    // �������݃V�[�P���XPART2
    UpdateParameter(frm, CHKID_WRITE_SEQUENCE2, &((NNshGlobal->NNsiParam)->writeSequence2));

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting6                                       */
/*                                NNsi�ݒ�-6(�W���O�֘A)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting6(FormType *frm)
{
    UInt16 item;

    // �Q�Ǝ�JumpList�Ɂu�ꗗ�ցv
    UpdateParameter(frm, CHKID_ADD_RTNTOLIST, &((NNshGlobal->NNsiParam)->addReturnToList));

    // �Q�Ǝ�JumpList�Ɂu�ЂƂ߂�v
    UpdateParameter(frm, CHKID_ADD_BACKMESSAGE, &((NNshGlobal->NNsiParam)->addMenuBackRtnMsg));

    // �Q�Ǝ�JumpList�Ƀ��j���[
    UpdateParameter(frm, CHKID_ADD_OPENMENU, &((NNshGlobal->NNsiParam)->addMenuMsg));

    // �Q�Ǝ�JumpList�Ɂu�����擾�v
    UpdateParameter(frm, CHKID_ADD_GETPART, &((NNshGlobal->NNsiParam)->addLineGetPart));

    // �Q�Ǝ�JumpList�Ɂu����ؒf�v
    UpdateParameter(frm, CHKID_ADD_LINEDISCONN, &((NNshGlobal->NNsiParam)->addLineDisconn));

    // �Q�Ǝ�JumpList�ɐ擪��
    UpdateParameter(frm, CHKID_ADD_JUMPTOP, &((NNshGlobal->NNsiParam)->addJumpTopMsg));

    // �Q�Ǝ�JumpList�ɖ�����
    UpdateParameter(frm, CHKID_ADD_JUMPBOTTOM, &((NNshGlobal->NNsiParam)->addJumpBottomMsg));

    // �Q�Ǝ�JumpList�ɑS�I��+Web�u���E�U�ŊJ��
    UpdateParameter(frm, CHKID_ADD_OPENWEB, &((NNshGlobal->NNsiParam)->addMenuSelAndWeb));

    // �Q�Ǝ�JumpList�ɕ`�惂�[�h�ؑ�
    UpdateParameter(frm, CHKID_ADD_GRAPHVIEW, &((NNshGlobal->NNsiParam)->addMenuGraphView));

    // �Q�Ǝ�JumpList�ɂ��C�ɓ���ݒ�؂�ւ�
    UpdateParameter(frm, CHKID_ADD_FAVORITE, &item);
    (NNshGlobal->NNsiParam)->addMenuFavorite = item;

    // �Q�Ǝ�JumpList�ɃX���Ԏw��擾
    UpdateParameter(frm, CHKID_ADD_GETTHREADNUM, &item);
    (NNshGlobal->NNsiParam)->addMenuGetThreadNum = item;

    // �Q�Ǝ�JumpList�Ƀ������o��
    UpdateParameter(frm, CHKID_ADD_OUTPUTMEMO, &item);
    (NNshGlobal->NNsiParam)->addMenuOutputMemo = item;

    // �ꗗ��Menu�Ƀ��j���[�\��
    UpdateParameter(frm, CHKID_ADD_MENUOPEN, &item);
    (NNshGlobal->NNsiParam)->addMenuTitle = item;

    // �ꗗ��Menu�ɎQ��COPY
    UpdateParameter(frm, CHKID_ADD_MENUCOPY, &(NNshGlobal->NNsiParam)->addMenuCopyMsg);

    // �ꗗ��Menu��MSG�폜
    UpdateParameter(frm, CHKID_ADD_MENUDELETE, &(NNshGlobal->NNsiParam)->addMenuDeleteMsg);

    // �ꗗ��Menu�ɕ`�惂�[�h�ύX
    UpdateParameter(frm, CHKID_ADD_MENUGRAPHMODE, &(NNshGlobal->NNsiParam)->addMenuGraphTitle);

    // �ꗗ��Menu�ɑ��ړI�X�C�b�`�P
    UpdateParameter(frm, CHKID_ADD_MENUMULTISW1, &(NNshGlobal->NNsiParam)->addMenuMultiSW1);

    // �ꗗ��Menu�ɑ��ړI�X�C�b�`�Q
    UpdateParameter(frm, CHKID_ADD_MENUMULTISW2, &(NNshGlobal->NNsiParam)->addMenuMultiSW2);

    // �ꗗ��Menu��NNsi�I��
    UpdateParameter(frm, CHKID_ADD_NNSIEND, &(NNshGlobal->NNsiParam)->addMenuNNsiEnd);

    // �ꗗ��Menu�Ƀf�o�C�X���
    UpdateParameter(frm, CHKID_ADD_MENUDEVICEINFO, &(NNshGlobal->NNsiParam)->addMenuDeviceInfo);

    // �Q�Ǝ�Menu�Ƀf�o�C�X���
    UpdateParameter(frm, CHKID_ADD_DEVICEINFO, &item);
    (NNshGlobal->NNsiParam)->addDeviceInfo = item;

    // �Q�Ǝ�Menu��NG3�ݒ�
    UpdateParameter(frm, CHKID_ADD_NG3SET, &item);
    (NNshGlobal->NNsiParam)->addNgSetting3 = item;

    // �ꗗ��Menu��Dir�I��
    UpdateParameter(frm, CHKID_ADD_MENUDIRSELECT, &(NNshGlobal->NNsiParam)->addMenuDirSelect);

    // Menu��Bt On/Off�\��
    UpdateParameter(frm, CHKID_ADD_BT_ONOFF, &item);
    (NNshGlobal->NNsiParam)->addBtOnOff = item;

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effect_NNshSetting                                       */
/*                   NNsi�ݒ���(NNsi����)����p�����[�^�l�ɔ��f���鏈��  */
/*-------------------------------------------------------------------------*/
static Err effect_NNshSetting(FormType *frm)
{
    ListType *lstP;
    UInt16   item;

    /***  NNsh�ݒ��(�O���[�o���ϐ���)���f������  ***/

    // ��URL�擾���A�L���`�F�b�N�Ȃ�
    UpdateParameter(frm, CHKID_NOTCHECK_BBSURL,&((NNshGlobal->NNsiParam)->notCheckBBSURL));

    // �ړ����ɖ��擾�X���폜
    UpdateParameter(frm, CHKID_AUTODEL_NOT_YET,&((NNshGlobal->NNsiParam)->autoDeleteNotYet));

    // �J�i�𔼊p���S�p�ϊ�
    UpdateParameter(frm,CHKID_CONVERT_HANZEN, &((NNshGlobal->NNsiParam)->convertHanZen));

    // Offline�X���������ɃX�����������{���Ȃ�
    UpdateParameter(frm, CHKID_NOT_DELOFFLINE, &item);
    (NNshGlobal->NNsiParam)->notDelOffline = item;

    // ���C�ɓ���\���Œ�X�����x���̐ݒ���p�����[�^�֔��f
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_FAVOR_LEVEL));
    item  = LstGetSelection(lstP);
    switch (item)
    {
      case 1:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR_L2;
        break;

      case 2:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR_L3;
        break;

      case 3:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR_L4;
        break;

      case 4:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR;
        break;

      case 0:
      default:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR_L1;
        break;
    }

    // �m�F���b�Z�[�W���ȗ�
    item = 0;
    UpdateParameter(frm, CHKID_OMIT_DIALOG, &item);
    if (item == 0)
    {
        // �m�F���b�Z�[�W�ȗ��ݒ���N���A����
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_NOTHING;
    }
    if ((item != 0)&&((NNshGlobal->NNsiParam)->confirmationDisable == 0))
    {
        // �m�F���b�Z�[�W�ȗ��ݒ��S�Đݒ肷��
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
    }

    // �������ɁA�啶������������ʂ��Ȃ�
    UpdateParameter(frm, CHKID_SEARCH_CASELESS, &((NNshGlobal->NNsiParam)->searchCaseless));

    // �N������Offline�X��������
    UpdateParameter(frm, CHKID_OFFCHK_LAUNCH,   &((NNshGlobal->NNsiParam)->offChkLaunch));

    // �Q�ƃ��O�փR�s�[�����Ƃ��A�����ɃX���폜�����{
    UpdateParameter(frm, CHKID_COPYDEL_READONLY,
                    &((NNshGlobal->NNsiParam)->copyDelReadOnly));

    // i-mode�p��URL���g�p����
    UpdateParameter(frm, CHKID_USE_IMODEURL, &((NNshGlobal->NNsiParam)->useImodeURL));


    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting2                                       */
/*                                   NNsi�ݒ�2(�ꗗ���)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting2(FormType *frm)
{
    UInt16     item;
    ListType  *lstP;

    // �X���ꗗ(subject.txt)�S�擾
    UpdateParameter(frm, CHKID_SUBJECT_ALL, &((NNshGlobal->NNsiParam)->getAllThread));

    // �X���ꗗ��ʂ�tiny/small�t�H���g���g�p����
    UpdateParameter(frm,CHKID_CLIE_USE_TITLE,
                                     &((NNshGlobal->NNsiParam)->useSonyTinyFontTitle));

    // �d���m�F
    UpdateParameter(frm, CHKID_CHECK_DUPLICATE,
                    &((NNshGlobal->NNsiParam)->checkDuplicateThread));

    // ���ړI�X�C�b�`�ݒ�P�̐ݒ���p�����[�^�֔��f
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_PRINT_HEADER));
    item  = LstGetSelection(lstP);
    
    switch (item)
    {
      case 0:
        // �X���̐擪�ɂ͉����\�����Ȃ�
        (NNshGlobal->NNsiParam)->printNofMessage        = 0;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 0;
        break;

      case 2:
        // �X���̐擪�ɖ��ǐ���\������
        (NNshGlobal->NNsiParam)->printNofMessage        = 1;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 1;
        break;

      case 3:
        // �X���̐擪�Ƀ��x��-���X����\������
        (NNshGlobal->NNsiParam)->printNofMessage        = 1;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 2;
        break;

      case 4:
        // �X���̐擪�Ƀ��x��-���ǐ���\������
        (NNshGlobal->NNsiParam)->printNofMessage        = 1;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 3;
        break;

      case 1:
      default:
        // �X���̐擪�Ƀ��X����\������
        (NNshGlobal->NNsiParam)->printNofMessage        = 1;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 0;
        break;
    }

    // �擾�ςݑS�Ă͎Q�ƃ��O�ɕ\�����Ȃ�
    item = 0;
    UpdateParameter(frm, CHKID_NOT_READONLY_NEW, &(item));
    (NNshGlobal->NNsiParam)->notListReadOnlyNew = item;

    // �擾�ςݑS�Ăɂ͎Q�ƃ��O�ɕ\�����Ȃ�
    item = 0;
    UpdateParameter(frm, CHKID_NOT_READONLY, &(item));
    (NNshGlobal->NNsiParam)->notListReadOnly = item;

    // �擾��ɃX���Q��
    UpdateParameter(frm, CHKID_OPEN_AUTO,   &((NNshGlobal->NNsiParam)->openAutomatic));

#ifdef USE_COLOR
    // �ꗗ�\�����ɃJ���[
    UpdateParameter(frm, CHKID_USE_COLORMODE, &((NNshGlobal->NNsiParam)->useColor));
#endif

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting3                                       */
/*                                NNsi�ݒ�-3(�ꗗ�{�^��)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting3(FormType *frm)
{
    Char      *txtP;
    FieldType *fldP;
    ListType  *lstP;
    UInt16     item;
    MemHandle  txtH;

    // ���ړI�X�C�b�`�ݒ�P�̐ݒ���p�����[�^�֔��f
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_TITLE_FUNCSW1));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1 = item | NNSH_SWITCHUSAGE_TITLE;

    // ���ړI�X�C�b�`�ݒ�Q�̐ݒ���p�����[�^�֔��f
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_TITLE_FUNCSW2));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2 = item | NNSH_SWITCHUSAGE_TITLE;

    if ((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1 ==
        (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2)
    {
        // ���ړI�X�C�b�`�P�ƂQ�̐ݒ�l�������������ꍇ�X�C�b�`�Q���g�p���Ȃ�
        (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2 = NNSH_SWITCHUSAGE_TITLE;
    }

    // ���ړI�{�^���P�̐ݒ�
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN1));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn1Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN1));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn1Feature = item | MULTIBTN_FEATURE;

    // ���ړI�{�^���Q�̐ݒ�
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN2));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn2Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN2));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn2Feature = item | MULTIBTN_FEATURE;

    // ���ړI�{�^���R�̐ݒ�
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN3));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn3Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN3));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn3Feature = item | MULTIBTN_FEATURE;

    // ���ړI�{�^���S�̐ݒ�
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN4));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn4Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN4));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn4Feature = item | MULTIBTN_FEATURE;

    // ���ړI�{�^���T�̐ݒ�
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN5));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn5Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN5));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn5Feature = item | MULTIBTN_FEATURE;

    // ���ړI�{�^���U�̐ݒ�
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN6));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn6Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN6));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn6Feature = item | MULTIBTN_FEATURE;

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting9                                       */
/*                             NNsi�ݒ�-9(�V��/����֘A)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting9(FormType *frm)
{
    ListType  *lstP;
    UInt16     item;

    // NNsi�I�����ɉ����ؒf����
    UpdateParameter(frm, CHKID_END_OFFLINE, &((NNshGlobal->NNsiParam)->disconnectNNsiEnd));

    // �u�V���m�F�v�I�����ɉ����ؒf����
    item = 0;
    UpdateParameter(frm, CHKID_DISCONN_ARRIVAL, &(item));
    (NNshGlobal->NNsiParam)->disconnArrivalEnd = item;

    // �u�V���m�F�v�O��DA���N������
    item = 0;
    UpdateParameter(frm, CHKID_PREPARE_DA, &(item));
    (NNshGlobal->NNsiParam)->preOnDAnewArrival = item;

    // �u�V���m�F�v�I����ɖ��ǈꗗ��\������
    item = 0;
    UpdateParameter(frm, CHKID_ARRIVAL_NOTREAD, &(item));
    (NNshGlobal->NNsiParam)->autoOpenNotRead = item;

    // �u�V���m�F�v�I����Ƀr�[�v��
    item = 0;
    UpdateParameter(frm, CHKID_DONE_BEEP, &(item));
    (NNshGlobal->NNsiParam)->autoBeep = item;

    // �u�V���m�F�v�I����Ƀ}�N�����s
    item = 0;
    UpdateParameter(frm, CHKID_EXECUTE_MACRO, &(item));
    (NNshGlobal->NNsiParam)->startMacroArrivalEnd = item;

    //  NNsi�N�����Ƀ}�N�������s
    item = 0;
    UpdateParameter(frm, CHKID_AUTOSTARTMACRO, &(item));
    (NNshGlobal->NNsiParam)->autostartMacro = item;
    if ((NNshGlobal->NNsiParam)->autostartMacro != 0)
    {
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_MACROENABLE, "", 0);
    }

    // ������ڑ����͎擾�\��
    UpdateParameter(frm,CHKID_GETRESERVEFEATURE,&(NNshGlobal->NNsiParam)->getReserveFeature);

    // ����ڑ����ʂ��ĕ`��
    UpdateParameter(frm,CHKID_REDRAW_CONNECT,&((NNshGlobal->NNsiParam)->redrawAfterConnect));

    // �܂�BBS/�������@JBBS���V���m�F
    item = 0;
    UpdateParameter(frm, CHKID_HTMLBBS_ENABLE, &(item));
    (NNshGlobal->NNsiParam)->enableNewArrivalHtml = item;
    if ((NNshGlobal->NNsiParam)->enableNewArrivalHtml != 0)
    {
        //  �܂�BBS/�������@JBBS�ŐV���m�F����ꍇ�́A�����擾�ł͂Ȃ�
        // �Ď擾�����{���邱�Ƃ��x������
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNMACHINEW, "", 0);
    }

    // BBS�ꗗ�̃��X�g��Ԃ𔽉f������
    lstP  = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,LSTID_CHECK_NEWARRIVAL));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->newArrivalNotRead = item;

#ifdef USE_LOGCHARGE
    // �Q�ƃ��O�擾���x�����p�����[�^�ɔ��f������
    lstP  = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, LSTID_CHECK_READONLY));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->getROLogLevel = item;
#endif

    // �V�����G���[�ɂȂ�΍Ď擾
    UpdateParameter(frm, CHKID_ALLUPDATE_ABORT, &((NNshGlobal->NNsiParam)->autoUpdateGetError));

    // WebBrowser���J���Ƃ��ŐV50
    UpdateParameter(frm, CHKID_WEBBROWSE_LAST50, &item);
    if (item != 0)
    {
        (NNshGlobal->NNsiParam)->browseMesNum = 50;
    }
    else
    {
        (NNshGlobal->NNsiParam)->browseMesNum = 0;
    }

    // �ꗗ�擾���ɐV���m�F
    UpdateParameter(frm, CHKID_UPDATEMSG_LIST, &((NNshGlobal->NNsiParam)->listAndUpdate));
    if ((NNshGlobal->NNsiParam)->listAndUpdate != 0)
    {
        // �ꗗ�擾���ɐV���m�F����ꍇ�́u�ꗗ���㏑���v�Ɏ����Ń`�F�b�N������
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNLISTUPDATE, "", 0);
        (NNshGlobal->NNsiParam)->checkDuplicateThread = 1;
    }

    // �V���m�F���Ƀ��[���`�F�b�N
    UpdateParameter(frm, CHKID_cMDA_EXECUTE, &((NNshGlobal->NNsiParam)->use_DAplugin));
    if ((NNshGlobal->NNsiParam)->use_DAplugin != 0)
    {
        // �V���m�F���Ƀ��[���`�F�b�N����ꍇ�̒��ӎ�����\������
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNcMDA, MSG_NNSISET_WARNcMDA2, 0);
    }

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting4                                       */
/*                                  NNsi�ݒ�-4(�Q�Ɖ��)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting4(FormType *frm)
{
    FieldType *fldP;
    MemHandle txtH;
    Char      *numP;
    UInt16     resNum, item;

    // ���X���̍ő�T�C�Y
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_RESNUM_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // �t�B�[���h���烌�X���̍ő�T�C�Y���擾����
        numP = MemHandleLock(txtH);
        if (numP != NULL)
        {
            resNum = StrAToI(numP);
            MemHandleUnlock(txtH);
        }
        else
        {
            resNum = NNSH_MESSAGE_MAXNUMBER;
        }

        // ���X���̍ő�T�C�Y�w�肪�ُ킾�����ꍇ�ɂ́A�����l�ɖ߂�
        if ((resNum < NNSH_MESSAGE_MAXNUMBER)||(resNum > NNSH_MESSAGE_MAXLIMIT))
        {
            resNum = NNSH_MESSAGE_MAXNUMBER;
        }
        (NNshGlobal->NNsiParam)->msgNumLimit = resNum;
    }

    // RAW�\�����[�h
    UpdateParameter(frm, CHKID_USE_RAWDISPLAY, &resNum);
    (NNshGlobal->NNsiParam)->rawDisplayMode = resNum;

    // Bookmark(������ݒ�)
    UpdateParameter(frm, CHKID_USE_BOOKMARK,&((NNshGlobal->NNsiParam)->useBookmark));

    // ���O�����ȗ�������
    item = 0;
    UpdateParameter(frm, CHKID_INFO_ONELINE, &item);
    (NNshGlobal->NNsiParam)->useNameOneLine = item;
    if ((NNshGlobal->NNsiParam)->useNameOneLine != 0)
    {
//        // ���O�����ȗ�������Ƃ��́A�O���t�B�b�N�`�惂�[�h�ɂȂ��ĂȂ��ƗL���łȂ�
//        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNcMDA, MSG_NNSISET_WARNcMDA2, 0);
    }

    // �X���ԍ����y�z�ɂ���
    item = 0;
    UpdateParameter(frm, CHKID_BOLD_MSGNUM, &item);
    (NNshGlobal->NNsiParam)->boldMessageNum = item;

    // �X���Q�Ɖ�ʂ�tiny/small�t�H���g���g�p����
    UpdateParameter(frm,CHKID_CLIE_USE_TINYFONT,
                                      &((NNshGlobal->NNsiParam)->useSonyTinyFont));

    // �Q�Ǝ��A���_�[���C��OFF
    UpdateParameter(frm, CHKID_MSG_LINEOFF, &((NNshGlobal->NNsiParam)->disableUnderline));

    // PUSH ON�_�C�����Ł}�P
    UpdateParameter(frm, CHKID_NOT_CURSOR, &((NNshGlobal->NNsiParam)->notCursor));

    // �X���Q�Ɖ�ʂŃJ�[�\���\�����Ȃ�
    item = 0;
    UpdateParameter(frm,CHKID_NOT_FOCUSSET_NUM, &item);
    (NNshGlobal->NNsiParam)->notFocusNumField = item;

    // �X���Q�Ɖ�ʂ�Go���j���[�̃A���J�[���\�[�g������
    item = 0;
    UpdateParameter(frm,CHKID_GO_SORT, &item);
    (NNshGlobal->NNsiParam)->sortGoMenuAnchor = item;

    // �Q�Ǝ��O�Ɉړ��őO���X����
    UpdateParameter(frm, CHKID_DISPLAY_BOTTOMMES, &((NNshGlobal->NNsiParam)->dispBottom));

    // �Q�Ǝ����X��A���\��
    UpdateParameter(frm,CHKID_BLOCK_DISP_MODE, &((NNshGlobal->NNsiParam)->blockDispMode));
    if ((NNshGlobal->NNsiParam)->blockDispMode != 0)
    {
        // ���X��A���\������Ƃ��́A�O�Ɉړ��őO���X�������n�m�ɂ���
        (NNshGlobal->NNsiParam)->blockDispMode = NNSH_BLOCKDISP_NOFMSG;
        (NNshGlobal->NNsiParam)->dispBottom    = 1;
    }
/**
    // NG�m�F
    (NNshGlobal->NNsiParam)->hideMessage = 0;    
    UpdateParameter(frm, CHKID_HIDE_MESSAGE, &resNum);
    if (resNum != 0)
    {
        // NG�m�F-3�����{����
        (NNshGlobal->NNsiParam)->hideMessage = NNSH_USE_NGWORD1AND2;
    }
**/
    // NG�m�F(NG-3)
    (NNshGlobal->NNsiParam)->hideMessage = 0;    
    UpdateParameter(frm, CHKID_HIDE3_MESSAGE, &resNum);
    if (resNum != 0)
    {
        // NG�m�F-3�����{����
        (NNshGlobal->NNsiParam)->hideMessage = (NNshGlobal->NNsiParam)->hideMessage | NNSH_USE_NGWORD3;
    }

    // NG���[�h�ݒ��","�ŕ�����NG���[�h���g�p����
    (NNshGlobal->NNsiParam)->useRegularExpression = 1;
#if 0
    // NG���[�h�ݒ�ŕ�����NG���[�h���g�p����
    UpdateParameter(frm, CHKID_USE_REGEXPRESS,
                                        &((NNshGlobal->NNsiParam)->useRegularExpression));
    if ((NNshGlobal->NNsiParam)->useRegularExpression != 0)
    {
        //  ���K�\�����g�p����ꍇ�ɂ́A�p�t�H�[�}���X���ۏ؂ł��Ȃ����Ƃ�
        // �x������
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNREGEXP, "", 0);
    }
#endif

    // NG-3�ݒ���X�����ɐؑւ���ݒ�
    resNum = 0;
    UpdateParameter(frm, CHKID_USE_NG_EACHTHREAD, &resNum);
    if (resNum != 0)
    {
        (NNshGlobal->NNsiParam)->hideMessage =
                 (((NNshGlobal->NNsiParam)->hideMessage)|(NNSH_USE_NGWORD_EACHTHREAD));
    }
    else
    {
        (NNshGlobal->NNsiParam)->hideMessage =
                 (((NNshGlobal->NNsiParam)->hideMessage)&(NNSH_USE_NGWORD_CLEAREACHTHREAD));
    }

    // ��������g���ꍇ�̐ݒ�
    if ((NNshGlobal->NNsiParam)->useBookmark == 0)
    {
        // ��������g��Ȃ��ꍇ�A������ݒ�̃N���A
        (NNshGlobal->NNsiParam)->lastFrmID     = NNSH_FRMID_THREAD;
        (NNshGlobal->NNsiParam)->bookMsgNumber = 0;
        (NNshGlobal->NNsiParam)->bookMsgIndex  = 0;
        MEMFREE_PTR(NNshGlobal->bookmarkFileName);
        MEMFREE_PTR(NNshGlobal->bookmarkNick);
    }
    else
    {
        if (NNshGlobal->bookmarkFileName == NULL)
        {
            // ������ݒ�L���̈���m�ۂ���
            NNshGlobal->bookmarkFileName = MEMALLOC_PTR(MAX_THREADFILENAME);
            if (NNshGlobal->bookmarkFileName != NULL)
            {
                NNshGlobal->bookmarkNick = MEMALLOC_PTR(MAX_NICKNAME);
                if (NNshGlobal->bookmarkNick == NULL)
                {
                    MEMFREE_PTR(NNshGlobal->bookmarkFileName);
                }
            }
        }
    }

    // NG�ݒ�1�����2�̕�����`�F�b�N
    SeparateWordList((NNshGlobal->NNsiParam)->hideWord1, &(NNshGlobal->hide1));
    SeparateWordList((NNshGlobal->NNsiParam)->hideWord2, &(NNshGlobal->hide2));

    // �X�����x���̃Z���N�^�g���K�Ɋ��蓖�Ă�
    (NNshGlobal->NNsiParam)->viewMultiBtnFeature = 
          LstGetSelection(FrmGetObjectPtr(frm,
                                     FrmGetObjectIndex(frm, LSTID_LVLSELTRIG)));

    // �X���^�C�g���̃Z���N�^�g���K�Ɋ��蓖�Ă�
    (NNshGlobal->NNsiParam)->viewTitleSelFeature = 
          LstGetSelection(FrmGetObjectPtr(frm,
                                     FrmGetObjectIndex(frm, LSTID_TTLSELTRIG)));

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSettingA                                       */
/*                                        NNsi�ݒ�(�T��)�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSettingA(FormType *frm)
{
    UInt16   item;

    // DEBUG���[�h�̐ݒ�
    UpdateParameter(frm, CHKID_DEBUG_DIALOG,&((NNshGlobal->NNsiParam)->debugMessageON));

    // �X���ꗗ(subject.txt)�S�擾
    UpdateParameter(frm, CHKID_SUBJECT_ALL, &((NNshGlobal->NNsiParam)->getAllThread));

#ifdef USE_COLOR
    // �ꗗ�\�����ɃJ���[
    UpdateParameter(frm, CHKID_USE_TITLE_COLOR, &((NNshGlobal->NNsiParam)->useColor));
#endif

    // �g�pVFS�w��
    item  = LstGetSelection(FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, LSTID_VFS_LOCATION)));
    (NNshGlobal->NNsiParam)->vfsUseCompactFlash = (UInt8) item;

    // VFS ���p�\���Ɏ����I��VFS ON
    item = 0;
    UpdateParameter(frm, CHKID_VFSON_AUTOMATIC, &item);
    if (item != 0)
    {
        // ����VFS ON��NNsi�ݒ�(�T��)�Őݒ肵���ꍇ�ɂ́ADB�`�F�b�N��OFF�ɂ���
        if (((NNshGlobal->NNsiParam)->vfsOnAutomatic == 0)&&((NNshGlobal->NNsiParam)->vfsOnNotDBCheck == 0))
        {
            (NNshGlobal->NNsiParam)->vfsOnNotDBCheck = 1;
        }
    }
    (NNshGlobal->NNsiParam)->vfsOnAutomatic = (UInt8) item;

    // �Q�Ǝ����X��A���\��
    UpdateParameter(frm,CHKID_BLOCK_DISP_MODE, &((NNshGlobal->NNsiParam)->blockDispMode));
    if ((NNshGlobal->NNsiParam)->blockDispMode != 0)
    {
        // ���X��A���\������Ƃ��́A�O�Ɉړ��őO���X�������n�m�ɂ���
        (NNshGlobal->NNsiParam)->blockDispMode = NNSH_BLOCKDISP_NOFMSG;
        (NNshGlobal->NNsiParam)->dispBottom    = 1;
    }

    // ������ڑ����͎擾�\��
    UpdateParameter(frm,CHKID_GETRESERVEFEATURE,&(NNshGlobal->NNsiParam)->getReserveFeature);

    // �܂�BBS/�������@JBBS���V���m�F
    item = 0;
    UpdateParameter(frm, CHKID_HTMLBBS_ENABLE, &(item));
    (NNshGlobal->NNsiParam)->enableNewArrivalHtml = item;
    if ((NNshGlobal->NNsiParam)->enableNewArrivalHtml != 0)
    {
        //  �܂�BBS/�������@JBBS�ŐV���m�F����ꍇ�́A�����擾�ł͂Ȃ�
        // �Ď擾�����{���邱�Ƃ��x������
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNMACHINEW, "", 0);

    }

    // �m�F���b�Z�[�W���ȗ�
    item = 0;
    UpdateParameter(frm, CHKID_OMIT_DIALOG, &item);
    if (item == 0)
    {
        // �m�F���b�Z�[�W�ȗ��ݒ���N���A����
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_NOTHING;
    }
    if ((item != 0)&&((NNshGlobal->NNsiParam)->confirmationDisable == 0))
    {
        // �m�F���b�Z�[�W�ȗ��ݒ��S�Đݒ肷��
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
    }

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}


/*-------------------------------------------------------------------------*/
/*   Function :   clearFeatureList                                         */
/*                                            ���X�g������̈���N���A���� */
/*-------------------------------------------------------------------------*/
static void clearFeatureList(void)
{
    if (NNshGlobal->featureList != NULL)
    {
        // ���x���̗̈惍�b�N���͂����B
        while (NNshGlobal->featureLockCnt != 0)
        {
            MemHandleUnlock((NNshGlobal->featureList)->wordmemH);
            (NNshGlobal->featureLockCnt)--;
        }

        // �@�\�ݒ胊�X�g����`����Ă����ꍇ�͗̈���������
        ReleaseWordList(NNshGlobal->featureList);
        MEMFREE_PTR(NNshGlobal->featureList);
        NNshGlobal->featureList = NULL;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   closeForm_NNsiSettings                                   */
/*                                        NNsi�ݒ�n��ʂ̃t�H�[������� */
/*-------------------------------------------------------------------------*/
static void closeForm_NNsiSettings(UInt16 nextFormID)
{
    // ���X�g�̈���N���A
    clearFeatureList();

    // ��ʑJ�ڂ���
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm (nextFormID);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   createFeatureListStrings                                 */
/*                              �X�g�����O���\�[�X����ꗗ���X�g�𐶐����� */
/*-------------------------------------------------------------------------*/
static Err createFeatureListStrings(NNshWordList **lst, UInt16 strRscId)
{
    Err        ret;
    MemHandle  memH;
    Char      *ptr;

    // �@�\�ꗗ���X�g���쐬�ς݂Ȃ瑦����
    if (*lst != NULL)
    {
        return (errNone);
    }

    // �@�\�ꗗ���X�g�̊Ǘ��̈���m�ۂ���
    *lst = MEMALLOC_PTR(sizeof(NNshWordList));
    if (*lst == NULL)
    {
        // �̈�m�ۂɎ��s
        return (errNone);
    }
    MemSet(*lst, sizeof(NNshWordList), 0x00);

    // �ꗗ�@�\���X�g�̎擾�i�X�g�����O���\�[�X���擾�j
    memH = DmGetResource('tSTR', strRscId);
    if (memH == 0)
    {
        return (~errNone);
    }
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        return (~errNone);
    }

    // �X�g�����O���\�[�X�����X�g�`���ɕ�������
    ret = SeparateWordList(ptr, *lst);
    if (ret != errNone)
    {
        // ������̕����Ɏ��s�����A�Ǘ��̈���J������B
        MEMFREE_PTR(*lst);
    }
    MemHandleUnlock(memH);
    DmReleaseResource(memH);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   keyDownEvt_NNshSetting                                   */
/*                                                        �L�[���͎��̏��� */
/*-------------------------------------------------------------------------*/
static Boolean keyDownEvt_NNshSetting(EventType *event)
{
    FormType *frm;
    UInt16    formID;
    Int16     move;

    // ���݂̃t�H�[��ID���擾
    formID = FrmGetActiveFormID();
    move   = 0;

    // 5way navigator�p�L�[�R�[�h�R���o�[�g��A�L�[�R�[�h�𔻒肷��
    switch (KeyConvertFiveWayToJogChara(event))
    {
      // �n�[�h�L�[�������������̏���
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case chrDownArrow:
        move = 1;
        break;

      // �n�[�h�L�[������������̏���
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case chrUpArrow:
        move = -1;
        break;

      // �o�b�N�{�^��/ESC�L�[/BS�L�[���������Ƃ��̏���
      // case chrBackspace:
      case vchrJogBack:
      case chrEscape:
      case vchrThumbWheelBack:
      case vchrGarminEscape:
        if (formID != FRMID_NNSI_SETTING_ABSTRACT)
        {
            // (�ύX��ۑ�����)NNsi�ݒ��ʂ֑J�ڂ���
            closeForm_NNsiSettings(FRMID_NNSI_SETTING_ABSTRACT);
        }
        return (false);
        break;

      case chrCapital_D:    // D�L�[�̓���
      case chrSmall_D:
      case chrDigitZero:    // �[���L�[�̓���
        if (formID != FRMID_NNSI_SETTING_ABSTRACT)
        {
            // NNsi�ݒ�(�T�v�ȊO)�͉��������I������
            return (false);
        }
        break;

      default: 
        return (false);
        break;
    }

    // ���݊J���Ă���t�H�[���h�c����A���ɊJ���t�H�[�������肷��
    frm = FrmGetActiveForm();
    switch (formID)
    {
      case FRMID_CONFIG_NNSH:
        effect_NNshSetting(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING2 : FRMID_NNSI_SETTING9;
        break;
      case FRMID_NNSI_SETTING2:
        effectNNsiSetting2(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING3 : FRMID_CONFIG_NNSH;
        break;
      case FRMID_NNSI_SETTING3:
        effectNNsiSetting3(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING4 : FRMID_NNSI_SETTING2;
        break;
      case FRMID_NNSI_SETTING4:
        effectNNsiSetting4(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING5 : FRMID_NNSI_SETTING3;
        break;
      case FRMID_NNSI_SETTING5:
        effectNNsiSetting5(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING6 : FRMID_NNSI_SETTING4;
        break;
      case FRMID_NNSI_SETTING6:
        effectNNsiSetting6(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING7 : FRMID_NNSI_SETTING5;
        break;
      case FRMID_NNSI_SETTING7:
        effectNNsiSetting7(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING8 : FRMID_NNSI_SETTING6;
        break;

      case FRMID_NNSI_SETTING8:
        effectNNsiSetting8(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING9 : FRMID_NNSI_SETTING7;
        break;

      case FRMID_NNSI_SETTING9:
        effectNNsiSetting9(frm);
        formID = (move > 0) ? FRMID_CONFIG_NNSH   : FRMID_NNSI_SETTING8;
        break;

      case FRMID_NNSI_SETTING_ABSTRACT:
        effectNNsiSettingA(frm);
        formID = FRMID_CONFIG_NNSH;
        break;

      default:
        return (false);
        break;
    }

    // ��ʑJ�ڂ���
    closeForm_NNsiSettings(formID);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetTitleHardKey                                          */
/*                                                        �n�[�h�L�[�̐ݒ� */
/*-------------------------------------------------------------------------*/
static Boolean SetTitleHardKey(Char *title, UInt16 data, UInt16 mask, NNshHardkeyControl *info)
{
    Boolean    ret = false;
    FormType  *frm, *prevFrm;
    ListType  *lstP;
    UInt16     items[MINIBUF], *dataP, item;
    Char      *ptr;

    NNshListItem popList[] = {
        { POPTRID_LIST_MULTIUP,   LSTID_MULTIUP   },
        { POPTRID_LIST_MULTIDOWN, LSTID_MULTIDOWN },
        { POPTRID_LIST_HKEY1,     LSTID_HKEY1     },
        { POPTRID_LIST_HKEY2,     LSTID_HKEY2     },
        { POPTRID_LIST_HKEY3,     LSTID_HKEY3     },
        { POPTRID_LIST_HKEY4,     LSTID_HKEY4     },
        { POPTRID_LIST_JOGPUSH,   LSTID_JOGPUSH   },
        { POPTRID_LIST_JOGBACK,   LSTID_JOGBACK   },
        { POPTRID_LIST_CAPTURE,   LSTID_CAPTURE   },
        { POPTRID_LIST_MULTILEFT, LSTID_MULTILEFT   },
        { POPTRID_LIST_MULTIRIGHT, LSTID_MULTIRIGHT },
        { 0, 0 }
    };                 

    // ���݂̃t�H�[�����擾
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // �_�C�A���O�t�H�[�����A�N�e�B�u�ɂ���
    frm = FrmInitForm(FRMID_HARDKEY_SETTING);
    FrmSetActiveForm(frm);

    // �E�B���h�E�^�C�g���̐ݒ�
    FrmCopyTitle(frm, title);

    // ���݂̃L�[�@�\�ݒ���e�[�u���Ɋi�[
    MemSet(items, sizeof(items), 0x00);
    dataP = items;
    *dataP = (info->up)&(mask);            // �n�[�h�L�[��̐ݒ�
    dataP++;
    *dataP = (info->down)&(mask);          // �n�[�h�L�[���̐ݒ�
    dataP++;
    *dataP = (info->key1)&(mask);          // �n�[�h�L�[�P�̐ݒ�
    dataP++;
    *dataP = (info->key2)&(mask);          // �n�[�h�L�[�Q�̐ݒ�
    dataP++;
    *dataP = (info->key3)&(mask);          // �n�[�h�L�[�R�̐ݒ�
    dataP++;
    *dataP = (info->key4)&(mask);          // �n�[�h�L�[�S�̐ݒ�
    dataP++;
    *dataP = (info->jogPush)&(mask);       // JOG PUSH�̐ݒ�
    dataP++;
    *dataP = (info->jogBack)&(mask);       // JOG BACK�̐ݒ�
    dataP++;
    *dataP = (info->clieCapture)&(mask);   // CLIE NX�L���v�`���{�^���̐ݒ�
    dataP++;
    *dataP = (info->left)&(mask);          // �n�[�h�{�^�����̐ݒ�
    dataP++;
    *dataP = (info->right)&(mask);         // �n�[�h�{�^���E�̐ݒ�
    dataP++;

    // �@�\�ꗗ���X�g�����b�N����
    ptr = MemHandleLock((NNshGlobal->featureList)->wordmemH);
    NNshWinSetPopItemsWithListArray(frm, popList, ptr,
                                    (NNshGlobal->featureList)->nofWord, items);

    // �_�C�A���O�̕\��
    if (FrmDoDialog(frm) == BTNID_HARDKEY_OK)
    {
        // �n�[�h�L�[��̐ݒ�
        lstP     = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIUP));
        item     = LstGetSelection(lstP);
        info->up = item | data;

        // �n�[�h�L�[���̐ݒ�
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIDOWN));
        item       = LstGetSelection(lstP);
        info->down = item | data;

        // �n�[�h�L�[�P�̐ݒ�
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_HKEY1));
        item       = LstGetSelection(lstP);
        info->key1 = item | data;

        // �n�[�h�L�[�Q�̐ݒ�
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_HKEY2));
        item       = LstGetSelection(lstP);
        info->key2 = item | data;

        // �n�[�h�L�[�R�̐ݒ�
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_HKEY3));
        item       = LstGetSelection(lstP);
        info->key3 = item | data;

        // �n�[�h�L�[�S�̐ݒ�
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_HKEY4));
        item       = LstGetSelection(lstP);
        info->key4 = item | data;

        // �W���O�_�C����Push�{�^��
        lstP          = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_JOGPUSH));
        item          = LstGetSelection(lstP);
        info->jogPush = item | data;

        // �W���O�_�C����Back�{�^��
        lstP          = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_JOGBACK));
        item          = LstGetSelection(lstP);
        info->jogBack = item | data;

        // NX Capture�{�^��
        lstP              = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_CAPTURE));
        item              = LstGetSelection(lstP);
        info->clieCapture = item | data;

        // �n�[�h�L�[���̐ݒ�
        lstP     = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTILEFT));
        item     = LstGetSelection(lstP);
        info->left = item | data;

        // �n�[�h�L�[�E�̐ݒ�
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIRIGHT));
        item       = LstGetSelection(lstP);
        info->right = item | data;

        // DB���X�V�������邵������
        NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

        ret = true;
    }

    // �@�\�ꗗ���X�g�̃��b�N����������
    MemHandleUnlock((NNshGlobal->featureList)->wordmemH);

    // �_�C�A���O�����
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(frm);
    FrmDrawForm(prevFrm);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Handler_NGwordInput                                      */
/*                                  NG���[�h�ݒ�̎擾�ݒ�C�x���g�n���h�� */
/*                 �g�p����O���[�o���ȕϐ�                                */
/*                                      NNshGlobal->work1  : �S���R�[�h��  */
/*                                      NNshGlobal->work2  : �\�����R�[�h  */
/*                        NNshGlobal->jumpSelection : �ҏW�����R�[�h�̑��� */
/*                                                        (�V�K>1, �ҏW>0) */
/*-------------------------------------------------------------------------*/
Boolean Handler_NGwordInput(EventType *event)
{
    UInt16              keyCode;
    NNshNGwordDatabase  dbData;
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
          case chrRightArrow:     // �J�[�\���L�[�E(5way�E)
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
            effectNGwordInput(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->jumpSelection = 0;
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrTapWaveSpecificKeyRUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
          case chrLeftArrow:         // �J�[�\���L�[��(5way��)
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
            effectNGwordInput(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->jumpSelection = 0;
            break;

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
        // NG���[�h�i�[�pDB�̎擾
        OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
        if (dbRef == 0)
        {
            // NG���[�h�pDB�̃I�[�v�����s�A�I������
            return (false);
        }

        // �f�[�^��ǂݏo��
        MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
        GetRecord_NNsh(dbRef, NNshGlobal->work2, sizeof(NNshNGwordDatabase),
                       &dbData);

        // �ǂݏo�����f�[�^����ʂɔ��f������
        displayNGwordInfo(frm, &dbData);

        // DB���N���[�Y����
        CloseDatabase_NNsh(dbRef);
        return (true);
        break;

      case ctlSelectEvent:
        frm = FrmGetActiveForm();
        switch (event->data.ctlSelect.controlID)
        {
          case BTNID_NGWORD_PREV:
            // �O���R�[�h�Ɉړ�
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // �f�[�^�̐擪�A�X�V���Ȃ�
                return (true);
            }
            // �f�[�^���X�V
            effectNGwordInput(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->jumpSelection = 0;
            break;

          case BTNID_NGWORD_NEXT:
            // �����R�[�h�Ɉړ�
            if (NNshGlobal->work2 == 0)
            {
                // �f�[�^�̖����A�X�V���Ȃ�
                return (true);
            }

            // �f�[�^���X�V
            effectNGwordInput(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->jumpSelection = 0;
            break;

          case BTNID_NGWORD_NEW:
            // �V�K�擾��쐬(�f�[�^�𖖔��Ɉړ�����)
            // �f�[�^���X�V
            effectNGwordInput(frm);

            // ��f�[�^����ʂɔ��f������
            MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
            displayNGwordInfo(frm, &dbData);
            NNshGlobal->jumpSelection = 1;
            NNshGlobal->work2 = 0;
            return (true);
            break;

          case BTNID_NGWORD_DELETE:
            // ���R�[�h�폜
            if ((NNshGlobal->work1 != 0)&&(NNshGlobal->jumpSelection == 0))
            {
                // �f�[�^��{���ɍ폜���Ă悢���m�F����
                if (NNsh_ConfirmMessage(ALTID_CONFIRM, "",
                                        MSG_CONFIRM_DELETE_DATA, 0) == 0)
                {
                    // NG���[�h�i�[�pDB�̎擾
                    OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
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
        // NG���[�h�i�[�pDB�̎擾
        OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
        if (dbRef == 0)
        {
            // ���ODB�̃I�[�v�����s�A�I������
            return (false);
        }

        // �f�[�^��ǂݏo��
        MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
        GetRecord_NNsh(dbRef, NNshGlobal->work2, sizeof(NNshNGwordDatabase),
                       &dbData);

        // �ǂݏo�����f�[�^����ʂɔ��f������
        displayNGwordInfo(frm, &dbData);

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
/*   Function :   effectNGwordInput                                        */
/*                                              �\���f�[�^�̃��R�[�h�𔽉f */
/*                 �g�p����O���[�o���ȕϐ�                                */
/*                                      NNshGlobal->work1  : �S���R�[�h��  */
/*                                      NNshGlobal->work2  : �\�����R�[�h  */
/*                        NNshGlobal->jumpSelection : �ҏW�����R�[�h�̑��� */
/*                                                        (�V�K>1, �ҏW>0) */
/*-------------------------------------------------------------------------*/
static Boolean effectNGwordInput(FormType *frm)
{
    Boolean             ret;
    UInt16              item;
    Char               *ptr;
    NNshNGwordDatabase  dbData;
    FieldType          *fldP;
    MemHandle           txtH;
    
    MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
    
    // NG���[�h�̎擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_NGWORD_INPUT));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        MemSet(dbData.ngWord, NGWORD_LEN, 0x00);
        if (*ptr != '\0')
        {
            StrNCopy(dbData.ngWord, ptr, NGWORD_LEN - 1);
            MemHandleUnlock(txtH);
        }
        else
        {
            // NG���[�h���w�肳��Ȃ������A�I������
            MemHandleUnlock(txtH);
            return (false);
        }
    }

    // ���X�g�ԍ�(NG���`�F�b�N����͈�)�̎擾�Ɣ��f
    dbData.checkArea =
       (UInt8) LstGetSelection(FrmGetObjectPtr(frm, 
                                   FrmGetObjectIndex(frm, LSTID_NGWORD_AREA)));
    // �������ځ[��ݒ�̎擾�Ɣ��f
    item = 0;
    UpdateParameter(frm, CHKID_REPLACEWORD, &item);
    dbData.matchedAction = item;

    // �f�[�^�x�[�X�ւ̃f�[�^�o�^
    ret = EntryNGword3(NNshGlobal->jumpSelection, NNshGlobal->work2, &dbData);
    if ((ret == true)&&(NNshGlobal->jumpSelection != 0))
    {
        (NNshGlobal->work1)++;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   displayNGwordInfo                                        */
/*                                                          NG���[�h�̕\�� */
/*                 �g�p����O���[�o���ȕϐ�                                */
/*                                      NNshGlobal->work1  : �S���R�[�h��  */
/*                                      NNshGlobal->work2  : �\�����R�[�h  */
/*                        NNshGlobal->jumpSelection : �ҏW�����R�[�h�̑��� */
/*                                                        (�V�K>1, �ҏW>0) */
/*-------------------------------------------------------------------------*/
static Boolean displayNGwordInfo(FormType *frm, NNshNGwordDatabase *dbData)
{
    UInt16 item;
    Char   data[MINIBUF];
    
    // ���ݕ\�����Ă��郌�R�[�h�ԍ��̕\�� (���݌��� / �S����)
    MemSet (data, sizeof(data), 0x00);
    if (NNshGlobal->work1 != 0)
    {
        NUMCATI(data, NNshGlobal->work1 - NNshGlobal->work2);
    }
    else
    {
        StrCopy(data, "1");
    }
    StrCat (data, "/");
    NUMCATI(data, NNshGlobal->work1);
    NNshWinSetFieldText(frm, FLDID_NGWORD_INFO, false, data, MINIBUF);

    // �|�b�v�A�b�v�g���K(NG���[�h�̗L���͈�)�̒l��ݒ�
    item = dbData->checkArea;
    NNshWinSetPopItems(frm, POPTRID_NGWORD_AREA, LSTID_NGWORD_AREA, item);

    item = dbData->matchedAction;
    SetControlValue(frm, CHKID_REPLACEWORD, &item);

    // NG�L�[���[�h�̕\��
    NNshWinSetFieldText(frm,FLDID_NGWORD_INPUT,true,dbData->ngWord,NGWORD_LEN);

   return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : EntryNGword3                                               */
/*                                                NG�ݒ�-3�f�[�^��o�^���� */
/*-------------------------------------------------------------------------*/
Boolean EntryNGword3(UInt16 entryMode, UInt16 recNum, NNshNGwordDatabase *ngData)
{
    DmOpenRef           dbRef;

    // NG���[�h�i�[�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
    if (dbRef == 0)
    {
        // �i�[�pDB�̃I�[�v�����s�A�I������
        return (false);
    }

    // ��Ԃɂ��A�X�V/�o�^�����肷��
    if (entryMode == 0)
    {
        // ���R�[�h���X�V����
        UpdateRecord_NNsh(dbRef, recNum, sizeof(NNshNGwordDatabase), ngData);
    }
    else
    {
        // ���R�[�h��o�^����
        EntryRecord_NNsh(dbRef, sizeof(NNshNGwordDatabase), ngData);
    }

    // �f�[�^�x�[�X�����
    CloseDatabase_NNsh(dbRef);

    return (true);    
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetNGword3                                               */
/*                                                          NG�ݒ�-3���C�� */
/*-------------------------------------------------------------------------*/
Boolean SetNGword3(void)
{
    Boolean              ret = false;
    FormType            *frm, *prevFrm;
    DmOpenRef            dbRef;
    NNshNGwordDatabase   dbData;

    // NG���[�h�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
    if (dbRef == 0)
    {
        // NG���[�h�pDB�̃I�[�v�����s�A�I������
        return (false);
    }

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &(NNshGlobal->work1));

    // �f�[�^�̏�����
    NNshGlobal->work2 = 0;
    MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);

    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // �_�C�A���O�t�H�[�����A�N�e�B�u�ɂ���
    frm = FrmInitForm(FRMID_NGWORD_INPUT);
    FrmSetActiveForm(frm);
   
    // ���R�[�h���P�����Ȃ������ꍇ
    if (NNshGlobal->work1 == 0)
    {
        NNshGlobal->jumpSelection = 1;
    }
    else
    {
        // �Ō�̃��R�[�h�f�[�^����ʂɕ\������
        NNshGlobal->work2 = NNshGlobal->work1 - 1;
        GetRecord_NNsh(dbRef, NNshGlobal->work2,
                       sizeof(NNshNGwordDatabase), &dbData);
        NNshGlobal->jumpSelection = 0;
    }
    displayNGwordInfo(frm, &dbData);

    // �f�[�^�x�[�X�����
    CloseDatabase_NNsh(dbRef);

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(frm, Handler_NGwordInput);

    // �Q�ƃ��O�擾�̐ݒ�_�C�A���O��\������
    if (FrmDoDialog(frm) == BTNID_NGWORD_OK)
    {
        // �\���f�[�^�̃��R�[�h�𔽉f������
        ret = effectNGwordInput(frm);
    }

    // �_�C�A���O�����
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(frm);
    FrmDrawForm(prevFrm);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   selectOmitDialog                                         */
/*                                          �m�F�_�C�A���O�̏ȗ�(�ڍאݒ�) */
/*-------------------------------------------------------------------------*/
static Boolean selectOmitDialog(void)
{
    Boolean   ret = false;
    FormType *frm, *prevFrm;
    UInt16    status;

    // ���݂̃`�F�b�N�{�b�N�X�ݒ�ƁANNsi�ݒ�ϐ��̊m�F���s��
    UpdateParameter(FrmGetActiveForm(), CHKID_OMIT_DIALOG, &status);
    if ((status != 0)&&((NNshGlobal->NNsiParam)->confirmationDisable == 0))
    {
        // �m�F���b�Z�[�W�ȗ��ݒ��S�Đݒ肷��
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
    }


    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // �_�C�A���O�t�H�[�����A�N�e�B�u�ɂ���
    frm = FrmInitForm(FRMID_OMITDIALOG_DETAIL);
    FrmSetActiveForm(frm);

    // �m�F�_�C�A���O���ȗ�����
    if (((NNshGlobal->NNsiParam)->confirmationDisable & (NNSH_OMITDIALOG_CONFIRM)) != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_OMITDIALOG_CONFIRM, &status);
    }

    // �x���_�C�A���O��\�����Ȃ�
    if (((NNshGlobal->NNsiParam)->confirmationDisable & (NNSH_OMITDIALOG_WARNING)) != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_OMITDIALOG_WARNING, &status);
    }

    // ���_�C�A���O��\�����Ȃ�
    if (((NNshGlobal->NNsiParam)->confirmationDisable & (NNSH_OMITDIALOG_INFORMATION)) != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_OMITDIALOG_INFORMATION, &status);
    }

    // �ڍ׃_�C�A���O��\������
    if (FrmDoDialog(frm) == BTNID_OMITDIALOG_OK)
    {
        (NNshGlobal->NNsiParam)->confirmationDisable = 0;

        // �m�F���b�Z�[�W���ȗ�����
        status = 0;
        UpdateParameter(frm, CHKID_OMITDIALOG_CONFIRM, &status);
        if (status != 0)
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = 
                     (NNshGlobal->NNsiParam)->confirmationDisable | NNSH_OMITDIALOG_CONFIRM;
        }

        // �x�����b�Z�[�W��\�����Ȃ�
        status = 0;
        UpdateParameter(frm, CHKID_OMITDIALOG_WARNING, &status);
        if (status != 0)
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = 
                     (NNshGlobal->NNsiParam)->confirmationDisable | NNSH_OMITDIALOG_WARNING;
        }

        // ��񃁃b�Z�[�W��\�����Ȃ�
        status = 0;
        UpdateParameter(frm, CHKID_OMITDIALOG_INFORMATION, &status);
        if (status != 0)
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = 
                 (NNshGlobal->NNsiParam)->confirmationDisable | NNSH_OMITDIALOG_INFORMATION;
        }
        ret = true;
    }

    // �_�C�A���O�����
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(frm);
    FrmDrawForm(prevFrm);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetRoundTripDelayTime                                    */
/*                                                      ����҂����Ԃ̐ݒ� */
/*-------------------------------------------------------------------------*/
Boolean SetRoundTripDelayTime(void)
{
    Boolean ret;
    Char    buffer[MINIBUF];
    
    MemSet(buffer, sizeof(buffer), 0x00);

    // ����҂����Ԃ̏����l�𔽉f������
    NUMCATI(buffer, (NNshGlobal->NNsiParam)->roundTripDelay);
    
    // ����҂����Ԃ̓���...
    ret = DataInputDialog(MSG_TIMEMS_INPUT, buffer, (sizeof(buffer) - 1),
                          NNSH_DIALOG_USE_OTHER, NULL);
    if (ret == true)
    {
        // �ݒ莞�Ԃ�NNsi�ݒ�ɔ��f������B�B�B
        (NNshGlobal->NNsiParam)->roundTripDelay = StrAToI(buffer);
    }
    return (ret);
}


/*=========================================================================*/
/*   Function :   Hander_NNshSetting                                       */
/*                                     NNsi�ݒ�n��(����)�C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_NNshSetting(EventType *event)
{
    FormType    *frm;
    ControlType *chkObj, *chkObjSub1, *chkObjSub2, *chkObjSub3, *chkObjSub4;
    ControlType *chkObjSub5;
#ifdef USE_COLOR
    IndexedColorType color;
#endif
#ifdef USE_XML_OUTPUT
    Char        *locP;
    UInt16       bakVFS, btnId;
    EventType   *dummyEvent;
#endif
    UInt16       formID;
    Err          ret;
    void        *effectFunc;

    // �{�^���`�F�b�N�ȊO�̃C�x���g�͑��߂�
    switch (event->eType)
    { 
      case menuEvent:
        // ���j���[�I��
        switch (event->data.menu.itemID)
        {
          case MNUID_SHOW_HELP:
            // ����w���v�̕\��
            // (�N���b�v�{�[�h�ɁAnnDA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����)
            ClipboardAddItem(clipboardText, 
                             nnDA_NNSIEXT_VIEWSTART
                             nnDA_NNSIEXT_INFONAME
                             nnDA_NNSIEXT_HELPNNSISET
                             nnDA_NNSIEXT_ENDINFONAME
                             nnDA_NNSIEXT_ENDVIEW,
                             sizeof(nnDA_NNSIEXT_VIEWSTART
                                     nnDA_NNSIEXT_INFONAME
                                     nnDA_NNSIEXT_HELPNNSISET
                                     nnDA_NNSIEXT_ENDINFONAME
                                     nnDA_NNSIEXT_ENDVIEW));
            break;

          case MNUID_SHOW_RECVFILE:
            // ��M�f�[�^�t�@�C���̕\��
            // (�N���b�v�{�[�h�ɁAnnDA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����)
            ClipboardAddItem(clipboardText, 
                             nnDA_NNSIEXT_VIEWSTART
                             nnDA_NNSIEXT_INFONAME
                             nnDA_NNSIEXT_FILERECVFILE
                             nnDA_NNSIEXT_ENDINFONAME
                             nnDA_NNSIEXT_ENDVIEW,
                             sizeof(nnDA_NNSIEXT_VIEWSTART
                                     nnDA_NNSIEXT_INFONAME
                                     nnDA_NNSIEXT_FILERECVFILE
                                     nnDA_NNSIEXT_ENDINFONAME
                                     nnDA_NNSIEXT_ENDVIEW));
            break;

          case MNUID_SHOW_SENDFILE:
            // ���M�f�[�^�t�@�C���̕\��
            // (�N���b�v�{�[�h�ɁAnnDA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����)
            ClipboardAddItem(clipboardText, 
                             nnDA_NNSIEXT_VIEWSTART
                             nnDA_NNSIEXT_INFONAME
                             nnDA_NNSIEXT_FILESENDFILE
                             nnDA_NNSIEXT_ENDINFONAME
                             nnDA_NNSIEXT_ENDVIEW,
                             sizeof(nnDA_NNSIEXT_VIEWSTART
                                     nnDA_NNSIEXT_INFONAME
                                     nnDA_NNSIEXT_FILESENDFILE
                                     nnDA_NNSIEXT_ENDINFONAME
                                     nnDA_NNSIEXT_ENDVIEW));
            break;

          default:
            // �ҏW���j���[�����s����
            return (NNsh_MenuEvt_Edit(event));
            break;
        }
        // nnDA���N������
        (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ(���֐i��)
        break;

      case keyDownEvent:
        // �L�[���͂�������
        return (keyDownEvt_NNshSetting(event));
        break;

#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        // �f�B�X�v���C�T�C�Y�ύX�C�x���g
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                FrmDrawForm(frm);        
            }
        }
        break;
#endif

      default: 
        return (false);
        break;
    }

    effectFunc = NULL;
    formID     = FRMID_NNSI_SETTING_ABSTRACT;
    frm        = FrmGetActiveForm();

    // ���ɊJ���t�H�[��ID�̐ݒ�
    switch (event->data.ctlSelect.controlID)
    {
      // "OK"�{�^���������ꂽ�Ƃ�(NNsi�ݒ��ʂɖ߂�)
      case BTNID_CONFIG_NNSH_OK:
      case BTNID_NNSI_SET_OK:
      case BTNID_NNSISET3_OK:
      case BTNID_NNSISET4_OK:
      case BTNID_NNSISET5_OK:
      case BTNID_NNSISET6_OK:
      case BTNID_NNSISET7_OK:
      case BTNID_NNSISET8_OK:
      case BTNID_NNSISET9_OK:
        formID     = FRMID_NNSI_SETTING_ABSTRACT;
        break;

      // NNsi�ݒ��ʂŁA"OK"/"Cancel"�{�^���������ꂽ�Ƃ�(�ꗗ��ʂɖ߂�)
      case BTNID_NNSISETA_OK:
      case BTNID_NNSISETA_CANCEL:
        formID     = NNshGlobal->backFormId;
        break;

      // "1"�{�^���������ꂽ�Ƃ�
      case NNSHSET2_1:
      case NNSHSET3_1:
      case NNSHSET4_1:
      case NNSHSET5_1:
      case NNSHSET6_1:
      case NNSHSET7_1:
      case NNSHSET8_1:
      case NNSHSET9_1:
      case SELTRID_NNSISET_NNSI:
        formID     = FRMID_CONFIG_NNSH;
        break;

      // "2"�{�^���������ꂽ�Ƃ�
      case NNSHSET_2:
      case NNSHSET3_2:
      case NNSHSET4_2:
      case NNSHSET5_2:
      case NNSHSET6_2:
      case NNSHSET7_2:
      case NNSHSET8_2:
      case NNSHSET9_2:
      case SELTRID_NNSISET_THREAD:
        formID     = FRMID_NNSI_SETTING2; 
        break;

      // "3"�{�^���������ꂽ�Ƃ�
      case NNSHSET_3:
      case NNSHSET2_3:
      case NNSHSET4_3:
      case NNSHSET5_3:
      case NNSHSET6_3:
      case NNSHSET7_3:
      case NNSHSET8_3:
      case NNSHSET9_3:
      case SELTRID_NNSISET_BUTTON:
        formID     = FRMID_NNSI_SETTING3; 
        break;

      // "4"�{�^���������ꂽ�Ƃ�
      case NNSHSET_4:
      case NNSHSET2_4:
      case NNSHSET3_4:
      case NNSHSET5_4:
      case NNSHSET6_4:
      case NNSHSET7_4:
      case NNSHSET8_4:
      case NNSHSET9_4:
      case SELTRID_NNSISET_MESSAGE:
        formID     = FRMID_NNSI_SETTING4; 
        break;

      // "5"�{�^���������ꂽ�Ƃ�
      case NNSHSET_5:
      case NNSHSET2_5:
      case NNSHSET3_5:
      case NNSHSET4_5:
      case NNSHSET6_5:
      case NNSHSET7_5:
      case NNSHSET8_5:
      case NNSHSET9_5:
      case SELTRID_NNSISET_WRITE:
        formID     = FRMID_NNSI_SETTING5;
        break;

      // "6"�{�^���������ꂽ�Ƃ�
      case NNSHSET_6:
      case NNSHSET2_6:
      case NNSHSET3_6:
      case NNSHSET4_6:
      case NNSHSET5_6:
      case NNSHSET7_6:
      case NNSHSET8_6:
      case NNSHSET9_6:
      case SELTRID_NNSISET_JOG:
        formID     = FRMID_NNSI_SETTING6;
        break;

      // "7"�{�^���������ꂽ�Ƃ�
      case NNSHSET_7:
      case NNSHSET2_7:
      case NNSHSET3_7:
      case NNSHSET4_7:
      case NNSHSET5_7:
      case NNSHSET6_7:
      case NNSHSET8_7:
      case NNSHSET9_7:
      case SELTRID_NNSISET_DEVICE:
        formID     = FRMID_NNSI_SETTING7;
        break;

      // "8"�{�^���������ꂽ�Ƃ�
      case NNSHSET_8:
      case NNSHSET2_8:
      case NNSHSET3_8:
      case NNSHSET4_8:
      case NNSHSET5_8:
      case NNSHSET6_8:
      case NNSHSET7_8:
      case NNSHSET9_8:
      case SELTRID_NNSISET_LINE:
        formID     = FRMID_NNSI_SETTING8;
        break;

      // "9"�{�^���������ꂽ�Ƃ�
      case NNSHSET_9:
      case NNSHSET2_9:
      case NNSHSET3_9:
      case NNSHSET4_9:
      case NNSHSET5_9:
      case NNSHSET6_9:
      case NNSHSET7_9:
      case NNSHSET8_9:
      case SELTRID_NNSISET_COMM:
        formID     = FRMID_NNSI_SETTING9;
        break;

#ifdef USE_COLOR
      case  SELTRID_COLOR_UNKNOWN:
        color = (NNshGlobal->NNsiParam)->colorUnknown;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorUnknown = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_BACKGROUND:
        color = (NNshGlobal->NNsiParam)->colorBackGround;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorBackGround = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_OVER:
        color = (NNshGlobal->NNsiParam)->colorOver;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorOver = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_NEW:
        color = (NNshGlobal->NNsiParam)->colorNew;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorNew = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_UPDATE:
        color = (NNshGlobal->NNsiParam)->colorUpdate;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorUpdate = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_REMAIN:
        color = (NNshGlobal->NNsiParam)->colorRemain;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorRemain = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_ALREADY:
        color = (NNshGlobal->NNsiParam)->colorAlready;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorAlready = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_NOTYET:
        color = (NNshGlobal->NNsiParam)->colorNotYet;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorNotYet = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_ERROR:
        color = (NNshGlobal->NNsiParam)->colorError;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorError = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_UPDOWN:
        color = (NNshGlobal->NNsiParam)->colorButton;

        // �F��I������
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorButton = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case SELTRID_VIEWCOLOR_DETAIL:
        // �Q�Ɖ�ʂ̐F�ݒ�_�C�A���O��\��
        setViewColorDetail();
        return (false);
        break;
#endif

      // �m�F�_�C�A���O���ȗ�
      case SELTRID_OMIT_DIALOG:
        // �ڍאݒ�
        if (selectOmitDialog() == true)
        {
            if ((NNshGlobal->NNsiParam)->confirmationDisable != 0)
            {
                formID = 1;
            }
            else
            {
                formID = 0;
            }
            SetControlValue(FrmGetActiveForm(), CHKID_OMIT_DIALOG, &formID);
            FrmDrawForm(FrmGetActiveForm());
        }
        return (false);
        break;
/**
      case SELTRID_HIDE1_WORDSET:
        // NG���[�h�P�ݒ�
        DataInputDialog(NNSH_INPUTWIN_NGWORD1,
                        (NNshGlobal->NNsiParam)->hideWord1, HIDEBUFSIZE,
                        NNSH_DIALOG_USE_SETNGWORD, NULL);
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case SELTRID_HIDE2_WORDSET:
        // NG���[�h�Q�ݒ�
        DataInputDialog(NNSH_INPUTWIN_NGWORD2,
                        (NNshGlobal->NNsiParam)->hideWord2, HIDEBUFSIZE,
                        NNSH_DIALOG_USE_SETNGWORD, NULL);
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;
**/
      case SELTRID_HIDE3_WORDSET:
        // NG���[�h�R�ݒ�
        SetNGword3();
        return (false);
        break;

      case SELTRID_GETLOG_SEPARATOR:
        // �Q�ƃ��O��؂�ݒ�
#ifdef USE_LOGCHARGE
        SetLogCharge_LogToken(0);
        return (false);
#endif // #ifdef USE_LOGCHARGE
        break;

      case SELTRID_GETLOG_DETAIL:
#ifdef USE_LOGCHARGE
        // �Q�ƃ��O�擾�ݒ�
        SetLogGetURL(NNSH_ITEM_BOTTOMITEM);
        return (false);
#endif
        break;

      case SELTRID_SET_LAUNCH_WORD:
        // �N�����p�X���[�h�ݒ�
        DataInputDialog(NNSH_INPUTWIN_SETPASS, (NNshGlobal->NNsiParam)->launchPass, PASSSIZE,
                        NNSH_DIALOG_USE_PASS, NULL);
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case SELTRID_NNSISET_IMPORT:
#ifdef USE_XML_OUTPUT
        // NNsi�ݒ���O����XML�t�@�C��(NNsiSet.xml)����ǂݏo������
        if ((NNshGlobal->NNsiParam)->useVFS == NNSH_NOTSUPPORT_VFS)
        {
            // VFS���T�|�[�g�@��̏ꍇ�A�{���ɓǂݏo�����ݒ���s��
            if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_IMPORT_XML, "", 0) == 0)
            {
                // ��荞�݂̎��s������
                ret = Input_NNsiSet_XML(FILE_XML_NNSISET);
                if (ret == errNone)
                {
                    // ��荞�ݐ���I��
                    NNsh_InformMessage(ALTID_INFO, MSG_INPUT_XML_DONE, "", 0);

                    // ��荞�݂����������ꍇ�ANNsi�I���C�x���g��ς�
                    dummyEvent = &(NNshGlobal->dummyEvent);
                    MemSet(dummyEvent, sizeof(EventType), 0x00);
                    dummyEvent->eType = appStopEvent;
                    EvtAddEventToQueue(dummyEvent);
                }
                else
                {
                    // �f�[�^�ǂݏo�����s
                    NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", ret);
                }
            }
            // �ǂݍ��ݏI���A��ʂ��J���Ȃ���
            FrmDrawForm(FrmGetActiveForm());
            break;
        }

        // �ȍ~��VFS�T�|�[�g�@��̏ꍇ�A�A�A
        bakVFS = (NNshGlobal->NNsiParam)->useVFS;
        btnId = NNsh_ConfirmMessage(ALTID_CONFIRM_DATALOC, MSG_IMPORT_XML_LOC, "", 0);
        switch (btnId)
        {
          case 0:
            // VFS���ǂݏo��(VFS�{�^���������ꂽ�Ƃ�)
            if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
            {
                // NNsi�ݒ肪�AVFS����ǂݏo���ݒ�ɂȂ��Ă��Ȃ������ꍇ
                // (�������VFS�ݒ�ɍX�V����)
                (NNshGlobal->NNsiParam)->useVFS = ((NNSH_VFS_ENABLE)|
                                     (NNSH_VFS_WORKAROUND)|
                                     (NNSH_VFS_USEOFFLINE)|
                                     (NNSH_VFS_DIROFFLINE)|
                                     (NNSH_VFS_DBBACKUP));
                
            }
            locP = " (VFS)";
            break;

          case 1:
            // �������Palm���ǂݏo���ݒ�ɍX�V����(Palm�{�^���������ꂽ�Ƃ�)
            (NNshGlobal->NNsiParam)->useVFS = 0;
            locP = " (Palm)";
            break;

          default:
            // �f�[�^�ǂݏo�����~�A�֐��𔲂���
            NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
            return (false);
            break;
        }
        // btnId�Ɏ�荞�ݏ����O��VFS�ݒ���L������
        btnId = (NNshGlobal->NNsiParam)->useVFS;

        // ��荞�݂̎��s������
        ret = Input_NNsiSet_XML(FILE_XML_NNSISET);
        if (ret == errNone)
        {
            // ��荞�ݐ���I��
            NNsh_InformMessage(ALTID_INFO, MSG_INPUT_XML_DONE, locP, 0);

            // ��荞�݂����������ꍇ�ANNsi�I���C�x���g��ς�
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType = appStopEvent;
            EvtAddEventToQueue(dummyEvent);
        }
        else
        {
            // �f�[�^�ǂݏo�����s
            NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, locP, 0);
        }

        // VFS�ݒ��ۊǂ��Ă����l�ɖ߂�
        // (�������AXML�`���̃f�[�^�ǂݍ��݂Œl���X�V����Ȃ������ꍇ�̂�)
        if (btnId == (NNshGlobal->NNsiParam)->useVFS)
        {
            (NNshGlobal->NNsiParam)->useVFS = bakVFS;
        }
        FrmDrawForm(FrmGetActiveForm());
#endif
        // ��ʂ��J���Ȃ���
        break;

      case SELTRID_NNSISET_EXPORT:
#ifdef USE_XML_OUTPUT
        // XML�t�@�C���`����NNsi�ݒ�t�@�C�����o�͂���
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // �o�͐��VFS
            locP = "VFS";
        }
        else
        {
            // �o�͐��Palm��
            locP = "Palm";
        }
        // �{���ɏo�͂��邩�m�F���s���B
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_EXPORT_XML, locP, 0) == 0)
        {
            // �o�͂̎��s������
            ret = Output_NNsiSet_XML(FILE_XML_NNSISET);
            if (ret == errNone)
            {
                // �o�͏I���ʒm
                NNsh_InformMessage(ALTID_INFO, MSG_OUTPUT_XML_DONE, "", 0);
            }
            else
            {
                // �G���[�����ʒm
                NNsh_InformMessage(ALTID_ERROR, MSG_OUTPUT_XML_ERROR, " Code:", ret);
            }
        }
        FrmDrawForm(FrmGetActiveForm());
#endif
        return (false);
        break;

      case SELTRID_USE_HARDKEY_VIEW:
        // ���X�g������̃N���A
        clearFeatureList();
      
        // �ꗗ��ʋ@�\���X�g�̐ݒ�
        createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREVIEWMSG_STRINGS);

        // �ꗗ��ʃn�[�h�L�[�ڍאݒ�
        (void) SetTitleHardKey(NNSH_TITLE_HARDKEYVIEW_TITLE, MULTIVIEWBTN_FEATURE, 
                                MULTIVIEWBTN_FEATURE_MASK, &((NNshGlobal->NNsiParam)->viewFtr));

        //  �ꕔ�n�[�h�L�[�̐���́ANNsi�����ݒ�̃n�[�h�L�[����ݒ肪�K�v��
        // ���Ƃ��x������B
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_SETTING_HARDKEY, "", 0);
        return (true);
        break;

      case SELTRID_USE_HARDKEY_DETAIL:
        // ���X�g������̃N���A
        clearFeatureList();
      
        // �ꗗ��ʋ@�\���X�g�̐ݒ�
        createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREMSG_STRINGS);

        // �ꗗ��ʃn�[�h�L�[�ڍאݒ�
        (void) SetTitleHardKey(NNSH_TITLE_HARDKEYSET_TITLE, MULTIBTN_FEATURE, 
                                MULTIBTN_FEATURE_MASK, &((NNshGlobal->NNsiParam)->ttlFtr));

        //  �ꕔ�n�[�h�L�[�̐���́ANNsi�����ݒ�̃n�[�h�L�[����ݒ肪�K�v��
        // ���Ƃ��x������B
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_SETTING_HARDKEY, "", 0);
        return (true);
        break;

      case SELTRID_HARDKEY_CONTROL:
        // ���X�g������̃N���A
        clearFeatureList();

        // �n�[�h�L�[����ݒ�
        createFeatureListStrings(&(NNshGlobal->featureList), HARDKEY_CONTROL_STRINGS);

        // �S�f�[�^��(�\������) "NNsi�Ő���" �ɂ��Ă����B
        (NNshGlobal->NNsiParam)->useKey.up      = 1;
        (NNshGlobal->NNsiParam)->useKey.down    = 1;
        (NNshGlobal->NNsiParam)->useKey.jogPush = 1;
        (NNshGlobal->NNsiParam)->useKey.jogBack = 1;

        (void) SetTitleHardKey(NNSH_HARDKEYCONTROL_TITLE, HARDKEY_FEATURE, 
                               HARDKEY_FEATURE_MASK, &((NNshGlobal->NNsiParam)->useKey));
        if (((NNshGlobal->NNsiParam)->useKey.up      != 1)||
            ((NNshGlobal->NNsiParam)->useKey.down    != 1)||
            ((NNshGlobal->NNsiParam)->useKey.jogPush != 1)||
            ((NNshGlobal->NNsiParam)->useKey.jogBack != 1))
        {
            // �ݒ�s�\�ȍ��ڂ��n�m�ɂȂ��Ă����̂ł����̓N���A����
            NNsh_WarningMessage(ALTID_WARN, MSG_NOTSET_KEYITEM, "", 0);
        }

        // �S�f�[�^���N���A����
        (NNshGlobal->NNsiParam)->useKey.up      = 0;
        (NNshGlobal->NNsiParam)->useKey.down    = 0;
        (NNshGlobal->NNsiParam)->useKey.jogPush = 0;
        (NNshGlobal->NNsiParam)->useKey.jogBack = 0;

        // �S�̓I�Ƀn�[�h�L�[�̐��������̂����Ȃ��̂����f����
        if (((NNshGlobal->NNsiParam)->useKey.key1 != 0)||((NNshGlobal->NNsiParam)->useKey.key2 != 0)||
            ((NNshGlobal->NNsiParam)->useKey.key3 != 0)||((NNshGlobal->NNsiParam)->useKey.key4 != 0)||
            ((NNshGlobal->NNsiParam)->useKey.clieCapture != 0)||
            ((NNshGlobal->NNsiParam)->useKey.left != 0)||((NNshGlobal->NNsiParam)->useKey.right != 0))
        {
            // �n�[�h�L�[��������{����ݒ�ɕύX
            (NNshGlobal->NNsiParam)->useHardKeyControl = 1;
        }
        else
        {
            // �n�[�h�L�[��������{���Ȃ��ݒ�ɕύX
            (NNshGlobal->NNsiParam)->useHardKeyControl = 0;
        }
        // DB���X�V���ꂽ�������
        NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
        return (true);
        break;

      // �ڍו\�����[�h�ɐ؂�ւ����ꍇ
      case NNSHSET_SHOW_DETAIL:
        formID = FrmGetActiveFormID();
        // NNsi�ݒ�(�T�v)��������ANNsi�̏ڍאݒ�֘A�̃Z���N�^�g���K��\������
        if (formID == FRMID_NNSI_SETTING_ABSTRACT)
        {
            FrmShowObject(frm,  FrmGetObjectIndex(frm, LBL_NNSISET_DETAIL));

            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_NNSI));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_THREAD));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_BUTTON));

            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_MESSAGE));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_WRITE));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_JOG));

            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_DEVICE));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_LINE));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_COMM));
        }
        FrmDrawForm(frm);
        return (true);
        break;

      case SELTRID_ROUNDTRIP_DELAYTIME:
        // ����҂����Ԃ̐ݒ�...
        if (SetRoundTripDelayTime() == true)
        {
            // DB���X�V���ꂽ�������
            NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
        }

        // ��ʍĕ`��...
        FrmDrawForm(FrmGetActiveForm());
        return (true);
        break;

      // �t�H�[���J�ڂ��Ȃ��Ƃ�
      default:
        // �Ȃɂ����Ȃ�
        break;
    }

    // ���f������֐���ݒ�
    switch (event->data.ctlSelect.controlID)
    {
      // NNsi�ݒ�̊m��
      case BTNID_CONFIG_NNSH_OK:
      case NNSHSET_2:
      case NNSHSET_3:
      case NNSHSET_4:
      case NNSHSET_5:
      case NNSHSET_6:
      case NNSHSET_7:
      case NNSHSET_8:
      case NNSHSET_9:
        effectFunc = &effect_NNshSetting;
        break;

      // NNsi�ݒ�-2�̊m��
      case BTNID_NNSI_SET_OK:
      case NNSHSET2_1:
      case NNSHSET2_3:
      case NNSHSET2_4:
      case NNSHSET2_5:
      case NNSHSET2_6:
      case NNSHSET2_7:
      case NNSHSET2_8:
      case NNSHSET2_9:
        effectFunc = &effectNNsiSetting2;
        break;

      // NNsi�ݒ�-3�̊m��
      case BTNID_NNSISET3_OK:
      case NNSHSET3_1:
      case NNSHSET3_2:
      case NNSHSET3_4:
      case NNSHSET3_5:
      case NNSHSET3_6:
      case NNSHSET3_7:
      case NNSHSET3_8:
      case NNSHSET3_9:
        effectFunc = &effectNNsiSetting3;
        break;

      // NNsi�ݒ�-4�̊m��
      case BTNID_NNSISET4_OK:
      case NNSHSET4_1:
      case NNSHSET4_2:
      case NNSHSET4_3:
      case NNSHSET4_5:
      case NNSHSET4_6:
      case NNSHSET4_7:
      case NNSHSET4_8:
      case NNSHSET4_9:
        effectFunc = &effectNNsiSetting4;
        break;

      // NNsi�ݒ�-5�̊m��
      case BTNID_NNSISET5_OK:
      case NNSHSET5_1:
      case NNSHSET5_2:
      case NNSHSET5_3:
      case NNSHSET5_4:
      case NNSHSET5_6:
      case NNSHSET5_7:
      case NNSHSET5_8:
      case NNSHSET5_9:
        effectFunc = &effectNNsiSetting5;
        break;

      // NNsi�ݒ�-6�̊m��
      case BTNID_NNSISET6_OK:
      case NNSHSET6_1:
      case NNSHSET6_2:
      case NNSHSET6_3:
      case NNSHSET6_4:
      case NNSHSET6_5:
      case NNSHSET6_7:
      case NNSHSET6_8:
      case NNSHSET6_9:
        effectFunc = &effectNNsiSetting6;
        break;

      // NNsi�ݒ�-7�̊m��
      case BTNID_NNSISET7_OK:
      case NNSHSET7_1:
      case NNSHSET7_2:
      case NNSHSET7_3:
      case NNSHSET7_4:
      case NNSHSET7_5:
      case NNSHSET7_6:
      case NNSHSET7_8:
      case NNSHSET7_9:
        effectFunc = &effectNNsiSetting7;
        break;

      // NNsi�ݒ�-8�̊m��
      case BTNID_NNSISET8_OK:
      case NNSHSET8_1:
      case NNSHSET8_2:
      case NNSHSET8_3:
      case NNSHSET8_4:
      case NNSHSET8_5:
      case NNSHSET8_6:
      case NNSHSET8_7:
      case NNSHSET8_9:
        effectFunc = &effectNNsiSetting8;
        break;

      // NNsi�ݒ�-9�̊m��
      case BTNID_NNSISET9_OK:
      case NNSHSET9_1:
      case NNSHSET9_2:
      case NNSHSET9_3:
      case NNSHSET9_4:
      case NNSHSET9_5:
      case NNSHSET9_6:
      case NNSHSET9_7:
      case NNSHSET9_8:
        effectFunc = &effectNNsiSetting9;
        break;

      // NNsi�ݒ�(�T��)�̊m��
      case BTNID_NNSISETA_OK:
      case SELTRID_NNSISET_COMM:
      case SELTRID_NNSISET_LINE:
      case SELTRID_NNSISET_DEVICE:
      case SELTRID_NNSISET_JOG:
      case SELTRID_NNSISET_WRITE:
      case SELTRID_NNSISET_MESSAGE:
      case SELTRID_NNSISET_BUTTON:
      case SELTRID_NNSISET_THREAD:
      case SELTRID_NNSISET_NNSI:
        effectFunc = &effectNNsiSettingA;
        break;

      // "Cancel"�{�^���������ꂽ�Ƃ�
      case BTNID_CONFIG_NNSH_CANCEL:
      case BTNID_NNSI_SET_CANCEL:
      case BTNID_NNSISET3_CANCEL:
      case BTNID_NNSISET4_CANCEL:
      case BTNID_NNSISET5_CANCEL:
      case BTNID_NNSISET6_CANCEL:
      case BTNID_NNSISET7_CANCEL:
      case BTNID_NNSISET8_CANCEL:
      case BTNID_NNSISET9_CANCEL:
      case BTNID_NNSISETA_CANCEL:
        effectFunc = NULL;
        break;

      case CHKID_USE_VFS:
        // VFS�؂�ւ��ݒ�𔽉f������
        if ((NNshGlobal->NNsiParam)->useVFS != NNSH_NOTSUPPORT_VFS)
        {
            // VFS �� ON/OFF���؂�ւ������A��ʏ��VFS�A�C�e�����X�V����B
            chkObj   = FrmGetObjectPtr(frm,
                                       FrmGetObjectIndex(frm, CHKID_USE_VFS));
            chkObjSub1 = FrmGetObjectPtr(frm, 
                                 FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
            chkObjSub2 = FrmGetObjectPtr(frm, 
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
            chkObjSub3 = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
            chkObjSub4 = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));
            chkObjSub5 = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));
            if (CtlGetValue(chkObj) == 0)
            {
                CtlSetValue  (chkObjSub1, 0);
                CtlSetEnabled(chkObjSub1, false);
                CtlSetUsable (chkObjSub1, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));

                CtlSetValue  (chkObjSub2, 0);
                CtlSetEnabled(chkObjSub2, false);
                CtlSetUsable (chkObjSub2, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));

                CtlSetValue  (chkObjSub3, 0);
                CtlSetEnabled(chkObjSub3, false);
                CtlSetUsable (chkObjSub3, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));

                CtlSetValue  (chkObjSub4, 0);
                CtlSetEnabled(chkObjSub4, false);
                CtlSetUsable (chkObjSub4, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));
                
                CtlSetValue  (chkObjSub5, 0);
                CtlSetEnabled(chkObjSub5, false);
                CtlSetUsable (chkObjSub5, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));

                FrmEraseForm (frm);
            }
            else
            {
                CtlSetEnabled(chkObjSub1, true);
                CtlSetUsable (chkObjSub1, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));

                CtlSetEnabled(chkObjSub2, true);
                CtlSetUsable (chkObjSub2, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));

                CtlSetEnabled(chkObjSub3, true);
                CtlSetUsable (chkObjSub3, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));

                CtlSetEnabled(chkObjSub4, true);
                CtlSetUsable (chkObjSub4, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));

                CtlSetEnabled(chkObjSub5, true);
                CtlSetUsable (chkObjSub5, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));
            }
            FrmDrawForm(frm);
        }
        return (false);
        break;

      case CHKID_USE_BBS_NEWURL:
        // �W��URL�`�F�b�N�{�b�N�X�������ꂽ���ABBS�ꗗ�擾���W��URL�ɕύX
        NNshWinSetFieldText(frm, FLDID_URL_BBS, true, URL_BBSTABLE, MAX_URL);
        NNsh_InformMessage(ALTID_INFO, MSG_CHANGED_URL_NEW, "", 0);
        chkObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_BBS_NEWURL));
        CtlSetValue(chkObj, 0);
        return (false);
        break;

      case CHKID_USE_BBS_OLDURL:
        // ��URL�`�F�b�N�{�b�N�X�������ꂽ�Ƃ��ABBS�ꗗ�擾�����URL�ɕύX����
        NNshWinSetFieldText(frm,FLDID_URL_BBS,true,URL_BBSTABLE_OLD, MAX_URL);
        NNsh_InformMessage(ALTID_INFO, MSG_CHANGED_URL_OLD, "", 0);
        chkObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_BBS_OLDURL));
        CtlSetValue(chkObj, 0);
        return (false);
        break;

      default:
        // ��L�ȊO(�������Ȃ�)
        return (false);
        break;
    }

    // �ݒ肳�ꂽ�f�[�^�𔽉f�����鏈��
    if (effectFunc != NULL)
    {
        ret =  ((Err (*)())effectFunc)(frm);
        if (ret != errNone)
        {
            // �ݒ�G���[(��ʂ͂��̂܂�)
            return (false);
        }
    }

    // ��ʑJ�ڂ���
    closeForm_NNsiSettings(formID);
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting7                                    */
/*                                NNsi�ݒ�-7(�f�o�C�X�֘A)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting7(FormType  *frm)
{
    ControlType *chkObj, *chkObjSub1, *chkObjSub2;
    ControlType *chkObjSub3, *chkObjSub4, *chkObjSub5;
    UInt16       dum;
#ifdef USE_TSPATCH
    UInt32       fontVer;
    Err          ret;
#endif

#ifdef USE_ARMLET
    UInt32       processorType;

    // PalmOS5�p�@�\���g�p����
    FtrGet(sysFileCSystem, sysFtrNumProcessorID, &processorType);
    if (sysFtrNumProcessorIsARM(processorType))
    {
        // ARMlet�̎g�p
        SetControlValue(frm, CHKID_USE_ARMLET, &((NNshGlobal->NNsiParam)->useARMlet));
    }
    else
#endif
    {
        // ARMlet�̎g�p�ɂ��āA�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_USE_ARMLET));
        CtlSetUsable(chkObjSub1, false);

        (NNshGlobal->NNsiParam)->useARMlet           = 0;
    }

#ifdef USE_CLIE
    // Silk������s��Ȃ��̕\���ۃ`�F�b�N
    if (NNshGlobal->silkVer != 0)
    {
        if (NNshGlobal->notUseSilk != 0)
        {
            dum = 1;
            SetControlValue(frm, CHKID_NOTUSE_SILK, &dum);
        }
    }
    else
#endif
    {
        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_NOTUSE_SILK));
        CtlSetUsable(chkObjSub1, false);
    }

    // SONY�n�C���]������s��Ȃ�
#ifdef USE_CLIE
    if ((NNshGlobal->notUseHR != 0)||(NNshGlobal->hrRef != 0))
    {
        if ((NNshGlobal->NNsiParam)->disableSonyHR != 0)
        {
            dum = 1;
        }
        else
        {
            dum = 0;
        }
        SetControlValue(frm, CHKID_NOTUSE_SONYHIRES, &dum);
    }
    else
#endif
    {
        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        chkObjSub1 = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, 
                                                  CHKID_NOTUSE_SONYHIRES));
        CtlSetUsable(chkObjSub1, false);
    }

    // TsPatch�@�\���s��Ȃ�
#ifdef USE_TSPATCH
    ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
    if (ret == errNone)
    {    
        if ((NNshGlobal->NNsiParam)->notUseTsPatch != 0)
        {
            dum = 1;
        }
        else
        {
            dum = 0;
        }
        SetControlValue(frm, CHKID_NOTUSE_TSPATCH, &dum);
    }
    else
#endif
    {
        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        chkObjSub1 = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, 
                                                        CHKID_NOTUSE_TSPATCH));
        CtlSetUsable(chkObjSub1, false);
    }

    // �g�pVFS�̎w��
    dum = (UInt16) (NNshGlobal->NNsiParam)->vfsUseCompactFlash;
    NNshWinSetPopItems(frm, POPTRID_VFS_LOCATION, LSTID_VFS_LOCATION, dum);

    // VFS ���p�\���Ɏ����I��VFS ON
    dum = (UInt16) (NNshGlobal->NNsiParam)->vfsOnAutomatic;
    SetControlValue(frm, CHKID_VFSON_AUTOMATIC, &dum);

    // �����I��VFS ON�ł�DB�`�F�b�N���Ȃ�
    SetControlValue(frm, CHKID_VFSON_NOTDBCHK,&((NNshGlobal->NNsiParam)->vfsOnNotDBCheck));

    // �uVFS�̎g�p�v�͂P�̃p�����[�^�ŕ����̈Ӗ����������L���Ă��邽��
    chkObj     = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_USE_VFS));
    chkObjSub1 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
    chkObjSub2 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
    chkObjSub3 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
    chkObjSub4 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));
    chkObjSub5 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));
    switch ((NNshGlobal->NNsiParam)->useVFS)
    {
      // VFS��T�|�[�g�@�̏ꍇ�A�uVFS�̎g�p�v�ݒ�͕\�����Ȃ��B
      case NNSH_NOTSUPPORT_VFS:
        CtlSetUsable(chkObj,     false);
        CtlSetUsable(chkObjSub1, false);
        CtlSetUsable(chkObjSub2, false);
        CtlSetUsable(chkObjSub3, false);
        CtlSetUsable(chkObjSub4, false);
        CtlSetUsable(chkObjSub5, false);
        break;

      // VFS�̃`�F�b�N
      case NNSH_VFS_DISABLE:
        CtlSetValue  (chkObj,     NNSH_VFS_DISABLE);
        CtlSetValue  (chkObjSub1, 0);
        CtlSetUsable (chkObjSub1, false);

        CtlSetValue  (chkObjSub2, 0);
        CtlSetUsable (chkObjSub2, false);

        CtlSetValue  (chkObjSub3, 0);
        CtlSetUsable (chkObjSub3, false);

        CtlSetValue  (chkObjSub4, 0);
        CtlSetUsable (chkObjSub4, false);

        CtlSetValue  (chkObjSub5, 0);
        CtlSetUsable (chkObjSub5, false);
        break;

      case NNSH_VFS_ENABLE:
      default:
        CtlSetValue  (chkObj,     NNSH_VFS_ENABLE);
        CtlSetUsable (chkObjSub1, true);
        CtlSetValue  (chkObjSub1, (NNSH_VFS_WORKAROUND & (NNshGlobal->NNsiParam)->useVFS));

        CtlSetUsable (chkObjSub2, true);
        CtlSetValue  (chkObjSub2, (NNSH_VFS_USEOFFLINE & (NNshGlobal->NNsiParam)->useVFS));

        CtlSetUsable (chkObjSub3, true);
        CtlSetValue  (chkObjSub3, (NNSH_VFS_DBBACKUP & (NNshGlobal->NNsiParam)->useVFS));

        CtlSetUsable (chkObjSub4, true);
        CtlSetValue  (chkObjSub4, (NNSH_VFS_DBIMPORT & (NNshGlobal->NNsiParam)->useVFS));

        CtlSetUsable (chkObjSub5, true);
        CtlSetValue  (chkObjSub5, (NNSH_VFS_DIROFFLINE & (NNshGlobal->NNsiParam)->useVFS));
        break;
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting8                                    */
/*                                    NNsi�ݒ�-8(�ʐM�֘A)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting8(FormType  *frm)
{
    UInt16       timeout;
    Char         numBuf[TINYBUF];

    // GZIP���g�p����ݒ�
    timeout = (NNshGlobal->NNsiParam)->useGZIP;
    SetControlValue(frm, CHKID_USE_GZIP, &(timeout));

    // Proxy�o�R�ł̃A�N�Z�X
    SetControlValue(frm, CHKID_USE_PROXY, &((NNshGlobal->NNsiParam)->useProxy));

    // Proxy URL
    if ((NNshGlobal->NNsiParam)->proxyURL[0] != '\0')
    { 
        NNshWinSetFieldText(frm, FLDID_USE_PROXY, false,
                            (NNshGlobal->NNsiParam)->proxyURL, MAX_URL);
    }

    // Proxy Port #
    if ((NNshGlobal->NNsiParam)->proxyPort != 0)
    {
        MemSet (numBuf, sizeof(numBuf), 0x00);
        StrIToA(numBuf, (NNshGlobal->NNsiParam)->proxyPort);
        NNshWinSetFieldText(frm,FLDID_PROXY_PORT, false, numBuf, TINYBUF);
    }

    // �t�B�[���h�Ɍ��݂̃^�C���A�E�g�l�𔽉f������
    MemSet(numBuf, sizeof(numBuf), 0x00);
    if ((NNshGlobal->NNsiParam)->netTimeout == -1)
    {
        // �^�C���A�E�g������Ȃ�΁A�ő�l�ɐݒ肷��
        StrIToA(numBuf, MAX_TIMEOUT_VALUE);
    }
    else
    {
        // �p�����[�^�ɂ� Tick���܂�ł���̂ŏ��Z�����{���ċ��߂�
        timeout = SysTicksPerSecond();
        timeout = (timeout == 0) ? 1 : timeout;  // �[�����Z�̉��(�ی�)
        StrIToA(numBuf, ((NNshGlobal->NNsiParam)->netTimeout / timeout));
    }
    NNshWinSetFieldText(frm,FLDID_COMM_TIMEOUT, false, numBuf, TINYBUF);

    // �t�B�[���h�Ɍ��݂̃o�b�t�@�T�C�Y�𔽉f������
    MemSet(numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, (NNshGlobal->NNsiParam)->bufferSize);
    NNshWinSetFieldText(frm,FLDID_BUFFER_SIZE,false,numBuf, TINYBUF);

    // �t�B�[���h�Ɍ��݂̕����擾�T�C�Y�𔽉f������
    MemSet(numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, (NNshGlobal->NNsiParam)->partGetSize);
    NNshWinSetFieldText(frm,FLDID_DL_PARTSIZE,false,numBuf, TINYBUF);


    // �t�B�[���h�ɒʐM�^�C���A�E�g�����g���C�񐔂𔽉f������
    MemSet (numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, (NNshGlobal->NNsiParam)->nofRetry);
    NNshWinSetFieldText(frm,FLDID_TIMEOUT_RETRY,false,numBuf,sizeof(numBuf));

    // �����_�E�����[�h�w�����ʂɔ��f����
    SetControlValue(frm, CHKID_DL_PART, &((NNshGlobal->NNsiParam)->enablePartGet));

    // BBS�ꗗ���㏑������
    SetControlValue(frm, CHKID_BBS_OVERWRITE, &((NNshGlobal->NNsiParam)->bbsOverwrite));

    // BBS�ꗗ�擾��URL�̐ݒ�
    NNshWinSetFieldText(frm, FLDID_URL_BBS, true, (NNshGlobal->NNsiParam)->bbs_URL, MAX_URL);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting5                                    */
/*                                NNsi�ݒ�-5(�������݉��)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting5(FormType  *frm)
{
    UInt16       item;
    ControlType  *objP;
    Char         numBuf[TINYBUF];

    // �������݉�ʏI�������X�������ŕۑ�
    SetControlValue(frm, CHKID_WRITE_AUTOSAVE,
                                       &((NNshGlobal->NNsiParam)->writeMessageAutoSave));

    // �������ݎ�PUSH�{�^���𖳌�
    item = (NNshGlobal->NNsiParam)->writeJogPushDisable;
    SetControlValue(frm, CHKID_WRITEPUSH_DISABLE, &(item));

    // ��MCookie���g�p���ď�������
    SetControlValue(frm, CHKID_WRITE_USE_COOKIE, &((NNshGlobal->NNsiParam)->useCookieWrite));

    // �������ݎ��A�������݃t�B�[���h�Ɏ����I�Ƀt�H�[�J�X���ړ����Ȃ�
    item = (NNshGlobal->NNsiParam)->notAutoFocus;
    SetControlValue(frm, CHKID_NOT_AUTOFOCUSSET, &item);

#ifdef USE_CLIE
    // Silk������s��Ȃ��̕\���ۃ`�F�b�N
    if (NNshGlobal->silkVer != 0)
    {
        // �������ݎ��A�V���N���g�債�Ȃ�
        item = (NNshGlobal->NNsiParam)->notOpenSilkWrite;
        SetControlValue(frm, CHKID_NOTSILK_WRITE, &item);
    }
    else
#endif
    {
        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        objP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_NOTSILK_WRITE));
        CtlSetUsable(objP, false);
    }

    // �������ݎ��A�A���_�[���C����OFF�ɂ���
    SetControlValue(frm, CHKID_UNDERLINE_WRITE,
                    &((NNshGlobal->NNsiParam)->disableUnderlineWrite));

    // �������ݎ��Q�ƃX���ԍ���}������
    SetControlValue(frm, CHKID_INSERT_REPLYNUM, &((NNshGlobal->NNsiParam)->insertReplyNum));

    // �������ݎ����p�J�L�R�ɂ���
    item = (NNshGlobal->NNsiParam)->writingReplyMsg;
    SetControlValue(frm, CHKID_INSERT_REPLYMSG, &item);

    // �������݃V�[�P���XPART2
    SetControlValue(frm, CHKID_WRITE_SEQUENCE2, &((NNshGlobal->NNsiParam)->writeSequence2));

    // sage��������
    SetControlValue(frm, CHKID_WRITE_SAGE,  &((NNshGlobal->NNsiParam)->writeAlwaysSage));

    // �R�e�n���@�\���g�p����ݒ����ʂɔ��f����
    SetControlValue(frm, CHKID_FIXED_HANDLE, &((NNshGlobal->NNsiParam)->useFixedHandle));

    // �n���h�����̐ݒ�
    if ((NNshGlobal->NNsiParam)->handleName[0] != '\0')
    { 
      NNshWinSetFieldText(frm, FLDID_HANDLENAME, false,
                            (NNshGlobal->NNsiParam)->handleName, BUFSIZE);
    }

    // �t�B�[���h�ɏ������݃o�b�t�@�T�C�Y
    MemSet (numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, (NNshGlobal->NNsiParam)->writeBufferSize);
    NNshWinSetFieldText(frm,FLDID_WRITEBUF_SIZE, true, numBuf, TINYBUF);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting6                                    */
/*                                  NNsi�ݒ�-6(�W���O�֘A)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting6(FormType  *frm)
{
    UInt16 item;

    // �Q�Ǝ�JumpList�Ɂu�ꗗ�ցv
    SetControlValue(frm, CHKID_ADD_RTNTOLIST, &((NNshGlobal->NNsiParam)->addReturnToList));

    // �Q�Ǝ�JumpList�Ɂu�ЂƂ߂�v
    SetControlValue(frm,CHKID_ADD_BACKMESSAGE,&((NNshGlobal->NNsiParam)->addMenuBackRtnMsg));

    // �Q�Ǝ�JumpList�Ɂu���j���[�v
    SetControlValue(frm, CHKID_ADD_OPENMENU, &((NNshGlobal->NNsiParam)->addMenuMsg));

    // �Q�Ǝ�JumpList�Ɂu�����擾�v
    SetControlValue(frm, CHKID_ADD_GETPART, &((NNshGlobal->NNsiParam)->addLineGetPart));

    // �Q�Ǝ�JumpList�Ɂu����ؒf�v
    SetControlValue(frm, CHKID_ADD_LINEDISCONN, &((NNshGlobal->NNsiParam)->addLineDisconn));

    // �Q�Ǝ�JumpList�ɐ擪��
    SetControlValue(frm, CHKID_ADD_JUMPTOP, &((NNshGlobal->NNsiParam)->addJumpTopMsg));

    // �Q�Ǝ�JumpList�ɖ�����
    SetControlValue(frm, CHKID_ADD_JUMPBOTTOM, &((NNshGlobal->NNsiParam)->addJumpBottomMsg));

    // �Q�Ǝ�JumpList�ɑS�I��+Web�u���E�U�ŊJ��
    SetControlValue(frm, CHKID_ADD_OPENWEB, &((NNshGlobal->NNsiParam)->addMenuSelAndWeb));

    // �Q�Ǝ�JumpList�ɕ`�惂�[�h�ؑ�
    SetControlValue(frm, CHKID_ADD_GRAPHVIEW, &((NNshGlobal->NNsiParam)->addMenuGraphView));

    // �Q�Ǝ�JumpList�ɂ��C�ɓ���ݒ�؂�ւ�
    item = (NNshGlobal->NNsiParam)->addMenuFavorite;
    SetControlValue(frm, CHKID_ADD_FAVORITE, &item);

    // �Q�Ǝ�JumpList�ɃX���ԍ��w��擾
    item = (NNshGlobal->NNsiParam)->addMenuGetThreadNum;
    SetControlValue(frm,CHKID_ADD_GETTHREADNUM,&item);

    // �Q�Ǝ�JumpList�Ƀ������o��
    item = (NNshGlobal->NNsiParam)->addMenuOutputMemo;
    SetControlValue(frm,CHKID_ADD_OUTPUTMEMO,&item);

    // �ꗗ��Menu�Ƀ��j���[��ǉ�
    item = (NNshGlobal->NNsiParam)->addMenuTitle;
    SetControlValue(frm, CHKID_ADD_MENUOPEN, &item);

    // �ꗗ��Menu�ɎQ��COPY
    SetControlValue(frm, CHKID_ADD_MENUCOPY, &(NNshGlobal->NNsiParam)->addMenuCopyMsg);

    // �ꗗ��Menu��MSG�폜
    SetControlValue(frm, CHKID_ADD_MENUDELETE, &(NNshGlobal->NNsiParam)->addMenuDeleteMsg);

    // �ꗗ��Menu�ɕ`�惂�[�h�ύX
    SetControlValue(frm,CHKID_ADD_MENUGRAPHMODE,&(NNshGlobal->NNsiParam)->addMenuGraphTitle);

    // �ꗗ��Menu�ɑ��ړI�X�C�b�`�P
    SetControlValue(frm, CHKID_ADD_MENUMULTISW1, &(NNshGlobal->NNsiParam)->addMenuMultiSW1);

    // �ꗗ��Menu�ɑ��ړI�X�C�b�`�Q
    SetControlValue(frm, CHKID_ADD_MENUMULTISW2, &(NNshGlobal->NNsiParam)->addMenuMultiSW2);

    // �ꗗ��Menu��NNsi�I��
    SetControlValue(frm, CHKID_ADD_NNSIEND, &(NNshGlobal->NNsiParam)->addMenuNNsiEnd);

    // �ꗗ��Menu�Ƀf�o�C�X���
    SetControlValue(frm, CHKID_ADD_MENUDEVICEINFO, &(NNshGlobal->NNsiParam)->addMenuDeviceInfo);

    // �Q�Ǝ�Menu�Ƀf�o�C�X���
    item = (NNshGlobal->NNsiParam)->addDeviceInfo;
    SetControlValue(frm, CHKID_ADD_DEVICEINFO, &item);

    // �Q�Ǝ�Menu��NG3�ݒ�
    item = (NNshGlobal->NNsiParam)->addNgSetting3;
    SetControlValue(frm, CHKID_ADD_NG3SET, &item);

    // �ꗗ��Menu��Dir�I��
    SetControlValue(frm, CHKID_ADD_MENUDIRSELECT, &(NNshGlobal->NNsiParam)->addMenuDirSelect);

    // Menu��Bt On/Off�\��
    item = (NNshGlobal->NNsiParam)->addBtOnOff;
    SetControlValue(frm, CHKID_ADD_BT_ONOFF, &item);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNshSetting                                     */
/*                                      NNsi�ݒ�(NNsi����)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNshSetting(FormType *frm)
{
    ControlType *ctlObj;
    ListType    *lstP;
    UInt16       item;
    Char        *ptr;

    // �Q�ƃ��O�փR�s�[�����Ƃ��A�����ɃX���폜�����{
    SetControlValue(frm, CHKID_COPYDEL_READONLY,&((NNshGlobal->NNsiParam)->copyDelReadOnly));


    // ��URL�擾���A�L���`�F�b�N�Ȃ�
    SetControlValue(frm, CHKID_NOTCHECK_BBSURL,&((NNshGlobal->NNsiParam)->notCheckBBSURL));

    // �ړ����ɖ��擾�X���폜
    SetControlValue(frm, CHKID_AUTODEL_NOT_YET,&((NNshGlobal->NNsiParam)->autoDeleteNotYet));

    // �J�i�𔼊p���S�p�ϊ�
    SetControlValue(frm,CHKID_CONVERT_HANZEN, &((NNshGlobal->NNsiParam)->convertHanZen));

    // Offline�X���������ɃX�����������{���Ȃ�
    item = (NNshGlobal->NNsiParam)->notDelOffline;
    SetControlValue(frm, CHKID_NOT_DELOFFLINE, &item);

    // ���C�ɓ����\������Œ�X�����x���̃��x���ݒ�(�ƃ��X�g�A�C�e���ݒ�)
    switch ((NNshGlobal->NNsiParam)->displayFavorLevel)
    {
      case NNSH_MSGATTR_FAVOR_L2:
        item = 1;
        ptr  = NNSH_ATTRLABEL_FAVOR_L2;
        break;

      case NNSH_MSGATTR_FAVOR_L3:
        item = 2;
        ptr  = NNSH_ATTRLABEL_FAVOR_L3;
        break;

      case NNSH_MSGATTR_FAVOR_L4:
        item = 3;
        ptr  = NNSH_ATTRLABEL_FAVOR_L4;
        break;

      case NNSH_MSGATTR_FAVOR:
        item = 4;
        ptr  = NNSH_ATTRLABEL_FAVOR;
        break;

      case NNSH_MSGATTR_FAVOR_L1:
      default:
        item = 0;
        ptr  = NNSH_ATTRLABEL_FAVOR_L1;
        break;
    }

    ctlObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, POPTRID_FAVOR_LEVEL));
    CtlSetLabel(ctlObj, ptr);

    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_FAVOR_LEVEL));
    LstSetSelection(lstP, item);
    LstSetTopItem  (lstP, item);


    /***  NNsh�ݒ��(�O���[�o���ϐ�����)���f������  ***/

    // �m�F���b�Z�[�W���ȗ�
    SetControlValue(frm, CHKID_OMIT_DIALOG, &((NNshGlobal->NNsiParam)->confirmationDisable));

    // �������ɁA�啶������������ʂ��Ȃ�
    SetControlValue(frm, CHKID_SEARCH_CASELESS, &((NNshGlobal->NNsiParam)->searchCaseless));

    // �N������Offline�X��������
    SetControlValue(frm, CHKID_OFFCHK_LAUNCH,   &((NNshGlobal->NNsiParam)->offChkLaunch));

    // i-mode�p��URL���g�p����
    SetControlValue(frm, CHKID_USE_IMODEURL, &((NNshGlobal->NNsiParam)->useImodeURL));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting2                                    */
/*                                    NNsi�ݒ�-2(�ꗗ���)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting2(FormType  *frm)
{
    UInt16       item;
    ControlType *ctlObj;

    // �X���ꗗ(subject.txt)�S�擾
    SetControlValue(frm, CHKID_SUBJECT_ALL, &((NNshGlobal->NNsiParam)->getAllThread));

    // �X���ꗗ��ʂ�tiny/small�t�H���g���g�p����
    SetControlValue(frm,CHKID_CLIE_USE_TITLE,
                                     &((NNshGlobal->NNsiParam)->useSonyTinyFontTitle));

    // �X���^�C�g�����d�������Ȃ�
    SetControlValue(frm, CHKID_CHECK_DUPLICATE,&((NNshGlobal->NNsiParam)->checkDuplicateThread));

    
    // �h�X���擪�̕\���h�ݒ�
    if ((NNshGlobal->NNsiParam)->printNofMessage == 0)
    {
        // �X���擪�Ƀ��X���\�����Ȃ�
        item = 0;
    }
    else
    {
        switch ((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum)
        {
          case 1:
            item = 2;  // ���ǐ��\��
            break;
            
          case 2:
            item = 3;  // ���x�� + ���X��
            break;

          case 3:
            item = 4;  // ���x�� + ���ǐ�
            break;
            
          default:
            item = 1;  // ���X��
            break;
        }
    }
    
    // �\�����[�h�𔽉f������
    NNshWinSetPopItems(frm, POPTRID_PRINT_HEADER, LSTID_PRINT_HEADER, item);

    // �擾�ςݑS�Ăɂ͎Q�ƃ��O��\�����Ȃ�
    item = (NNshGlobal->NNsiParam)->notListReadOnly;
    SetControlValue(frm, CHKID_NOT_READONLY, &(item));

    // ���ǂ���ɂ͎Q�ƃ��O��\�����Ȃ�
    item = (NNshGlobal->NNsiParam)->notListReadOnlyNew;
    SetControlValue(frm, CHKID_NOT_READONLY_NEW, &(item));

    // �X���擾��Ƀ��X�Q��
    SetControlValue(frm, CHKID_OPEN_AUTO,   &((NNshGlobal->NNsiParam)->openAutomatic));


#ifdef USE_COLOR
    // �ꗗ�\�����ɃJ���[(OS3.5�ȏ�Őݒ荀�ڂ�L���ɂ���)
    if (NNshGlobal->palmOSVersion >= 0x03503000)
    {
        // �J���[�̎g�p�ݒ�
        SetControlValue(frm, CHKID_USE_COLORMODE, &((NNshGlobal->NNsiParam)->useColor));

        // �{���́A�Z���N�^�g���K�̃L���v�V�����ɐF����������...
        // �i�悭�킩���̂ŏȗ��j
    }
    else
#endif
    {
        (NNshGlobal->NNsiParam)->useColor = 0;

        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        FrmCopyLabel(frm, LBL_SEL_COLOR, "  ");

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_COLORMODE));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm, SELTRID_COLOR_UNKNOWN));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                            FrmGetObjectIndex(frm, SELTRID_COLOR_BACKGROUND));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_OVER));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_NEW));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_UPDATE));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_REMAIN));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, SELTRID_COLOR_ALREADY));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_NOTYET));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_ERROR));
        CtlSetUsable(ctlObj, false);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting3                                    */
/*                                  NNsi�ݒ�-3(�ꗗ�{�^��)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting3(FormType  *frm)
{
    UInt16       selItem;

    // �ꗗ��ʋ@�\���X�g�̐ݒ�
    createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREMSG_STRINGS);

    // ���݂̃t�H�[���ւ̃|�C���^���擾
    frm = FrmGetActiveForm();

    // ���ړI�X�C�b�`�P(�ꗗ���)�̃��x���ݒ�(�ƃ��X�g�A�C�e���ݒ�)
    selItem = 
             (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1 & (NNSH_SWITCHUSAGE_FUNCMASK);
    NNshWinSetPopItems(frm, POPTRID_LIST_FUNCSW1, LSTID_TITLE_FUNCSW1, selItem);

    // ���ړI�X�C�b�`�Q(�ꗗ���)�̃��x���ݒ�(�ƃ��X�g�A�C�e���ݒ�)
    selItem = 
             (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2 & (NNSH_SWITCHUSAGE_FUNCMASK);
    NNshWinSetPopItems(frm, POPTRID_LIST_FUNCSW2,LSTID_TITLE_FUNCSW2,selItem);

    // ���ړI�{�^���P�̐ݒ����ʂɔ��f������
    selItem = ((NNshGlobal->NNsiParam)->multiBtn1Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN1, LSTID_MULTIBTN1,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN1, false,
                        (NNshGlobal->NNsiParam)->multiBtn1Caption, MAX_CAPTION);

    // ���ړI�{�^���Q�̐ݒ����ʂɔ��f������
    selItem = ((NNshGlobal->NNsiParam)->multiBtn2Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN2, LSTID_MULTIBTN2,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN2, false,
                        (NNshGlobal->NNsiParam)->multiBtn2Caption, MAX_CAPTION);

    // ���ړI�{�^���R�̐ݒ����ʂɔ��f������
    selItem = ((NNshGlobal->NNsiParam)->multiBtn3Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN3, LSTID_MULTIBTN3,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN3, false,
                        (NNshGlobal->NNsiParam)->multiBtn3Caption, MAX_CAPTION);

    // ���ړI�{�^���S�̐ݒ����ʂɔ��f������
    selItem = ((NNshGlobal->NNsiParam)->multiBtn4Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN4, LSTID_MULTIBTN4,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN4, false,
                        (NNshGlobal->NNsiParam)->multiBtn4Caption, MAX_CAPTION);

    // ���ړI�{�^���T�̐ݒ����ʂɔ��f������
    selItem = ((NNshGlobal->NNsiParam)->multiBtn5Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN5, LSTID_MULTIBTN5,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN5, false,
                        (NNshGlobal->NNsiParam)->multiBtn5Caption, MAX_CAPTION);

    // ���ړI�{�^���U�̐ݒ����ʂɔ��f������
    selItem = ((NNshGlobal->NNsiParam)->multiBtn6Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN6, LSTID_MULTIBTN6,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN6, true,
                        (NNshGlobal->NNsiParam)->multiBtn6Caption, MAX_CAPTION);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting9                                    */
/*                                   NNsi�ݒ�-9(�V��/���)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting9(FormType  *frm)
{
    Err          ret;
    UInt16       lp;
    ControlType *chkObj;

    // NNsi�I�����ɉ����ؒf����
    lp = (NNshGlobal->NNsiParam)->disconnectNNsiEnd;
    SetControlValue(frm, CHKID_END_OFFLINE, &(lp));

    // �u�V���m�F�v�I�����ɉ����ؒf����
    lp = (NNshGlobal->NNsiParam)->disconnArrivalEnd;
    SetControlValue(frm,CHKID_DISCONN_ARRIVAL,&(lp));

    // �u�V���m�F�v�I����ɖ��ǈꗗ��\������
    lp = (NNshGlobal->NNsiParam)->autoOpenNotRead;
    SetControlValue(frm, CHKID_ARRIVAL_NOTREAD, &(lp));

    // �u�V���m�F�v�O��DA���N������
    lp = (NNshGlobal->NNsiParam)->preOnDAnewArrival;
    SetControlValue(frm, CHKID_PREPARE_DA, &(lp));

    //  NNsi�N�����Ƀ}�N�������s
#ifdef USE_MACRO
    lp = (NNshGlobal->NNsiParam)->autostartMacro;
    SetControlValue(frm, CHKID_AUTOSTARTMACRO, &(lp));
#else
    // �}�N���@�\���L���łȂ��ꍇ�ɂ̓A�C�e��������
    CtlSetUsable(FrmGetObjectPtr(frm, 
                         FrmGetObjectIndex(frm, CHKID_AUTOSTARTMACRO)), false);
#endif

    // �u�V���m�F�v�I����Ƀr�[�v����炷
    lp = (NNshGlobal->NNsiParam)->autoBeep;
    SetControlValue(frm, CHKID_DONE_BEEP, &(lp));

    // �u�V���m�F�v�I����Ƀ}�N�����s
#ifdef USE_MACRO
    lp = (NNshGlobal->NNsiParam)->startMacroArrivalEnd;
    SetControlValue(frm, CHKID_EXECUTE_MACRO, &(lp));
#else
    // �}�N���@�\���L���łȂ��ꍇ�ɂ̓A�C�e��������
    CtlSetUsable(FrmGetObjectPtr(frm, 
                         FrmGetObjectIndex(frm, CHKID_EXECUTE_MACRO)), false);
#endif

    // ������ڑ����͎擾�\��
    SetControlValue(frm,CHKID_GETRESERVEFEATURE,&(NNshGlobal->NNsiParam)->getReserveFeature);

    // ����ڑ����ʂ��ĕ`��
    SetControlValue(frm,CHKID_REDRAW_CONNECT,&((NNshGlobal->NNsiParam)->redrawAfterConnect));

    // �܂�BBS/�������@JBBS���V���m�F
    lp = (NNshGlobal->NNsiParam)->enableNewArrivalHtml;
    SetControlValue(frm, CHKID_HTMLBBS_ENABLE, &(lp));

    // �V�����G���[�ɂȂ�΍Ď擾
    SetControlValue(frm, CHKID_ALLUPDATE_ABORT,
                                             &((NNshGlobal->NNsiParam)->autoUpdateGetError));

    // �ꗗ�擾���ɐV���m�F
    SetControlValue(frm, CHKID_UPDATEMSG_LIST, &((NNshGlobal->NNsiParam)->listAndUpdate));

    // �V���m�F���Ƀ��[���`�F�b�N
    if (CheckInstalledResource_NNsh('DAcc', 'cMDA') == false)
    {
        // cMDA���C���X�g�[������Ă��Ȃ��ꍇ�ɂ́A�ݒ荀�ڂ���������
        CtlSetUsable(FrmGetObjectPtr(frm, 
                         FrmGetObjectIndex(frm, CHKID_cMDA_EXECUTE)), false);
    }
    else
    {
        // �ݒ������ʂɔ��f������
        SetControlValue(frm, CHKID_cMDA_EXECUTE, &((NNshGlobal->NNsiParam)->use_DAplugin));
    }

    // BBS�ꗗ�̍쐬
    ret = Create_BBS_INDEX(&(NNshGlobal->bbsTitles), &lp);
    if (ret == errNone)
    {
        // BBS�ꗗ�̃��X�g��Ԃ𔽉f����B
        NNshWinSetListItems(frm, LSTID_CHECK_NEWARRIVAL,
                            NNshGlobal->bbsTitles, lp,
                            (NNshGlobal->NNsiParam)->newArrivalNotRead,
                            &(NNshGlobal->bbsTitleH), NULL);

        // �|�b�v�A�b�v�g���K�̃��x����(���X�g��Ԃɍ��킹)�X�V����
        NNshWinSetPopItems(frm, POPTRID_NEWARRIVAL, LSTID_CHECK_NEWARRIVAL,
                           (NNshGlobal->NNsiParam)->newArrivalNotRead);
    }


    // WebBrowser���J���Ƃ��ŐV50
    if (NNshGlobal->browserCreator != 0)
    {
        if ((NNshGlobal->NNsiParam)->browseMesNum != 0)
        {
            lp = 1;
            SetControlValue(frm, CHKID_WEBBROWSE_LAST50, &lp);
        }
    }
    else
    {
        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        chkObj = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_WEBBROWSE_LAST50));
        CtlSetUsable(chkObj, false);
    }

#ifdef USE_LOGCHARGE
    // �Q�ƃ��O�擾���x���̐ݒ�(�|�b�v�A�b�v�g���K�̃��x�����X�V����)
    lp = (NNshGlobal->NNsiParam)->getROLogLevel;
    NNshWinSetPopItems(frm, POPTRID_CHECK_READONLY, LSTID_CHECK_READONLY, lp);

#else
    // ���O�`���[�W�@�\���g�p���Ȃ��ꍇ�́A���ڂ�\�����Ȃ�
    chkObj = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, SELTRID_GETLOG_DETAIL));
    CtlSetUsable(chkObj, false);

    chkObj = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, SELTRID_GETLOG_SEPARATOR));
    CtlSetUsable(chkObj, false);

    chkObj = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, POPTRID_CHECK_READONLY));
    CtlSetUsable(chkObj, false);

#endif
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting4                                    */
/*                                    NNsi�ݒ�-4(�Q�Ɖ��)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting4(FormType  *frm)
{
    Char         numBuf[TINYBUF];
    UInt16       item;

    // ���X��
    if ((NNshGlobal->NNsiParam)->msgNumLimit != 0)
    {
        MemSet (numBuf, sizeof(numBuf), 0x00);
        StrIToA(numBuf, (NNshGlobal->NNsiParam)->msgNumLimit);
        NNshWinSetFieldText(frm, FLDID_RESNUM_SIZE, false, numBuf, TINYBUF);
    }

    // RAW�\�����[�h
    item = (NNshGlobal->NNsiParam)->rawDisplayMode;
    SetControlValue(frm, CHKID_USE_RAWDISPLAY, &item);

    // ������ݒ�@�\���g�p����
    SetControlValue(frm, CHKID_USE_BOOKMARK,&((NNshGlobal->NNsiParam)->useBookmark));

    // ���O�����ȗ�������
    item = (NNshGlobal->NNsiParam)->useNameOneLine;
    SetControlValue(frm, CHKID_INFO_ONELINE, &item);

    // �X���ԍ����y�z�ɂ���
    item = (NNshGlobal->NNsiParam)->boldMessageNum;
    SetControlValue(frm, CHKID_BOLD_MSGNUM, &item);

    // �Q�Ǝ��O���t�B�b�N�`�惂�[�h���g�p����
    SetControlValue(frm,CHKID_CLIE_USE_TINYFONT,&((NNshGlobal->NNsiParam)->useSonyTinyFont));

    // �Q�Ǝ��A���_�[���C��OFF
    SetControlValue(frm, CHKID_MSG_LINEOFF, &((NNshGlobal->NNsiParam)->disableUnderline));

    // PUSH ON�_�C�����Ł}�P
    SetControlValue(frm, CHKID_NOT_CURSOR, &((NNshGlobal->NNsiParam)->notCursor));

    // �X���Q�Ɖ�ʂŃJ�[�\���\�����Ȃ�
    item = (NNshGlobal->NNsiParam)->notFocusNumField;
    SetControlValue(frm, CHKID_NOT_FOCUSSET_NUM, &item);

    // �X���Q�Ɖ�ʂ�Go���j���[�A���J�[�𐮗񂳂���
    item = (NNshGlobal->NNsiParam)->sortGoMenuAnchor;
    SetControlValue(frm, CHKID_GO_SORT, &item);

    // �Q�Ǝ����X���]�[���ŕ\��
    SetControlValue(frm,CHKID_BLOCK_DISP_MODE, &((NNshGlobal->NNsiParam)->blockDispMode));

    // �Q�Ǝ��O�Ɉړ��őO���X����
    SetControlValue(frm, CHKID_DISPLAY_BOTTOMMES, &((NNshGlobal->NNsiParam)->dispBottom));

/**
    // NG�m�F(NG1, 2)
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD1AND2) == NNSH_USE_NGWORD1AND2)
    {
        item = 1;
    }
    else
    {
        item = 0;
    }
    SetControlValue(frm, CHKID_HIDE_MESSAGE, &item);
**/

    // NG�m�F(NG3)
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
    {
        item = 1;
    }
    else
    {
        item = 0;
    }
    SetControlValue(frm, CHKID_HIDE3_MESSAGE, &item);

    // NG���[�h�ݒ���X�����ɐݒ肷��ݒ�
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD_EACHTHREAD) == NNSH_USE_NGWORD_EACHTHREAD)
    {
        item = 1;
    }
    else
    {
        item = 0;
    }
    SetControlValue(frm, CHKID_USE_NG_EACHTHREAD, &item);


#if 0
    // NG���[�h�ݒ�Ő��K�\�����g�p����
    SetControlValue(frm, CHKID_USE_REGEXPRESS,
                                        &((NNshGlobal->NNsiParam)->useRegularExpression));
#endif

#ifdef USE_COLOR
    // �ꗗ�\�����ɃJ���[(OS3.5�ȏ�Őݒ荀�ڂ�L���ɂ���)
    if (NNshGlobal->palmOSVersion >= 0x03503000)
    {
        // �J���[�̎g�p�ݒ�
        // SetControlValue(frm, CHKID_USE_COLORMODE, &((NNshGlobal->NNsiParam)->useColor));

        // �{���́A�Z���N�^�g���K�̃L���v�V�����ɐF����������...
        // �i�悭�킩���̂ŏȗ��j
    }
    else
#endif
    {
        (NNshGlobal->NNsiParam)->useColor = 0;
    }

    // ���X�g������̃N���A
    clearFeatureList();
      
    // �Q�Ɖ�ʋ@�\���X�g�̐ݒ�
    createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREVIEWMSG_STRINGS);

    // �@�\�ꗗ���X�g�����b�N����(���x���̃Z���N�^�g���K)
    NNshWinSetPopItemsWithList(frm, POPTRID_LVLSELTRIG, LSTID_LVLSELTRIG,
                               NNshGlobal->featureList, 
                               (NNshGlobal->NNsiParam)->viewMultiBtnFeature);

    // �@�\�ꗗ���X�g�����b�N����(���x���̃Z���N�^�g���K)
    NNshWinSetPopItemsWithList(frm, POPTRID_TTLSELTRIG, LSTID_TTLSELTRIG,
                               NNshGlobal->featureList, 
                               (NNshGlobal->NNsiParam)->viewTitleSelFeature);
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSettingA                                    */
/*                                          NNsi�ݒ�(�T��)�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSettingA(FormType  *frm)
{
    ControlType *ctlObj;
    UInt16       item;

#ifdef USE_COLOR
    // �ꗗ�\�����ɃJ���[(OS3.5�ȏ�Őݒ荀�ڂ�L���ɂ���)
    if (NNshGlobal->palmOSVersion >= 0x03503000)
    {
        // �J���[�̎g�p�ݒ�
        SetControlValue(frm, CHKID_USE_TITLE_COLOR, &((NNshGlobal->NNsiParam)->useColor));
    }
    else
#endif
    {
        // �J���[�̐ݒ�͕\�����Ȃ�
        (NNshGlobal->NNsiParam)->useColor = 0;
        ctlObj = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_USE_TITLE_COLOR));
        CtlSetUsable(ctlObj, false);
    }

#ifdef USE_XML_OUTPUT
#else
    // XML�`�����o�͂��Ȃ��ꍇ�́ANNsi�ݒ�� Import/Export�w��͕\�������Ȃ��B
    FrmHideObject(frm, FrmGetObjectIndex(frm, LBL_NNSISET_XML));

    ctlObj = FrmGetObjectPtr(frm,
                             FrmGetObjectIndex(frm, SELTRID_NNSISET_IMPORT));
    CtlSetUsable(ctlObj, false);

    ctlObj = FrmGetObjectPtr(frm,
                             FrmGetObjectIndex(frm, SELTRID_NNSISET_EXPORT));
    CtlSetUsable(ctlObj, false);
#endif

    // DEBUG
    SetControlValue(frm, CHKID_DEBUG_DIALOG,&((NNshGlobal->NNsiParam)->debugMessageON));

    // �X���ꗗ(subject.txt)�S�擾
    SetControlValue(frm, CHKID_SUBJECT_ALL, &((NNshGlobal->NNsiParam)->getAllThread));

    // VFS ���p�\���Ɏ����I��VFS ON
    item = (UInt16) (NNshGlobal->NNsiParam)->vfsOnAutomatic;
    SetControlValue(frm, CHKID_VFSON_AUTOMATIC, &item);

    // �Q�Ǝ����X���]�[���ŕ\��
    SetControlValue(frm,CHKID_BLOCK_DISP_MODE, &((NNshGlobal->NNsiParam)->blockDispMode));

    // ������ڑ����͎擾�\��
    SetControlValue(frm,CHKID_GETRESERVEFEATURE,&(NNshGlobal->NNsiParam)->getReserveFeature);

    // �܂�BBS/�������@JBBS���V���m�F
    item = (NNshGlobal->NNsiParam)->enableNewArrivalHtml;
    SetControlValue(frm, CHKID_HTMLBBS_ENABLE, &(item));

    // �m�F���b�Z�[�W���ȗ�
    SetControlValue(frm, CHKID_OMIT_DIALOG, &((NNshGlobal->NNsiParam)->confirmationDisable));

    // �g�pVFS�̎w��
    item = (UInt16) (NNshGlobal->NNsiParam)->vfsUseCompactFlash;
    NNshWinSetPopItems(frm, POPTRID_VFS_LOCATION, LSTID_VFS_LOCATION, item);

#ifdef USE_USERTAB_DETAIL
         // �������Ȃ�...
#else
    if ((NNshGlobal->NNsiParam)->debugMessageON == 0)
    {
        // NNsi�̏ڍאݒ�֘A�̃Z���N�^�g���K��\�����Ȃ�
        FrmHideObject(frm,  FrmGetObjectIndex(frm, LBL_NNSISET_DETAIL));

        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_NNSI));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_THREAD));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_BUTTON));

        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_MESSAGE));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_WRITE));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_JOG));

        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_DEVICE));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_LINE));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_COMM));
    }
#endif  // #ifdef USE_USERTAB_DETAIL

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   closeForm_FavorTabSet                                    */
/*                                    ���C�ɓ�������ݒ�̃t�H�[������� */
/*-------------------------------------------------------------------------*/
static void closeForm_FavorTabSet(UInt16 nextForm)
{
    if (NNshGlobal->listHandle != 0)
    {
        MemHandleUnlock(NNshGlobal->listHandle);
        MemHandleFree  (NNshGlobal->listHandle);
        NNshGlobal->listHandle = 0;
    }
    FrmGotoForm(nextForm);
    return;
}

/*=========================================================================*/
/*   Function :   OpenForm_FavorTabSet                                     */
/*                                      ���C�ɓ�������ݒ�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_FavorTabSet(FormType *frm)
{
    Err            ret;
    Char          *ptr;
    UInt16         status, lp, bbsCond, thLevel, thrCond, allCond;
    NNshCustomTab *customTab;

    switch (NNshGlobal->jumpSelection)
    {
      case MULTIBTN_FEATURE_USER1TAB:
        lp        = PBTNID_FAVORSET_TAB1;
        customTab = &((NNshGlobal->NNsiParam)->custom1);
        break;

      case MULTIBTN_FEATURE_USER2TAB:
        lp        = PBTNID_FAVORSET_TAB2;
        customTab = &((NNshGlobal->NNsiParam)->custom2);
        break;

      case MULTIBTN_FEATURE_USER3TAB:
        lp        = PBTNID_FAVORSET_TAB3;
        customTab = &((NNshGlobal->NNsiParam)->custom3);
        break;

      case MULTIBTN_FEATURE_USER4TAB:
        lp        = PBTNID_FAVORSET_TAB4;
        customTab = &((NNshGlobal->NNsiParam)->custom4);
        break;

      case MULTIBTN_FEATURE_USER5TAB:
      default:
        lp        = PBTNID_FAVORSET_TAB5;
        customTab = &((NNshGlobal->NNsiParam)->custom5);
        break;
    }

    // ���[�U�^�u(�v�b�V���{�^��)�ݒ�
    status = 1;
    SetControlValue(frm, lp, &status);

    // �f�[�^�̒��o
    thLevel = customTab->threadLevel & NNSH_THREADLEVEL_MASK;
    thrCond = 
      (customTab->threadLevel & NNSH_THREADCOND_MASK) >> NNSH_THREADCOND_SHIFT;
    allCond = customTab->condition & NNSH_CONDITION_ALL;
    bbsCond = customTab->boardCondition;

    // �Q�ƃ��O�͏��O
    status  = customTab->condition & NNSH_CONDITION_NOTREADONLY;
    if (status != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_NOT_READONLY, &status);
    }

    // ���[�U�^�u�����t�B�[���h�ɔ��f������
    NNshWinSetFieldText(frm, FLDID_FAVORSET_TABTITLE, false,
                        customTab->tabName, StrLen(customTab->tabName));

    // BBS�ꗗ�̍쐬
    ret = Create_BBS_INDEX(&(NNshGlobal->bbsTitles), &lp);
    if (ret == errNone)
    {
        // BBS�ꗗ�̃��X�g��Ԃ𔽉f����B(�V���m�F���{�^�u)
        NNshWinSetListItems(frm, LSTID_FAVORSET_CHECKNEW,
                            NNshGlobal->bbsTitles, lp,
                            customTab->newArrival, 
                            &(NNshGlobal->listHandle),
                            &ptr);

        NNshWinSetPopItems(frm, POPTRID_FAVORSET_CHECKNEW,
                           LSTID_FAVORSET_CHECKNEW, customTab->newArrival);

        // BBS�ꗗ�̃��X�g��Ԃ𔽉f����B(�ꗗ�擾�^�u)
        NNshWinUpdateListItems(frm, LSTID_FAVORSET_GETLIST, ptr, lp,
                               customTab->getList);

        NNshWinSetPopItems(frm, POPTRID_FAVORSET_GETLIST,
                           LSTID_FAVORSET_GETLIST, customTab->getList);


        // BBS�ꗗ�̃��X�g��Ԃ𔽉f����B(�ݒ�^�u)
        NNshWinUpdateListItems(frm, LSTID_FAVORSET_BBSNICK, ptr, lp,
                               customTab->boardNick);

        NNshWinSetPopItems(frm, POPTRID_FAVORSET_BBSNICK,
                           LSTID_FAVORSET_BBSNICK, customTab->boardNick);
    }

    // �ݒ�̏����l�𔽉f
    if (bbsCond != 0)
    {
        // �ݒ�̃`�F�b�N��߂�
        SetControlValue(frm, CHKID_FAVORSET_BBSNICK, &(bbsCond));
        bbsCond--;
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_BBSNICK_SET,
                           LSTID_FAVORSET_BBSNICK_SET, bbsCond);
    }

    status = 1;
    if (thrCond != 0)
    {
        // �X�����x���̃`�F�b�N�{�b�N�X���f
        SetControlValue(frm, CHKID_FAVORSET_LEVEL, &(status));

        // �X�����x���̏������f
        thrCond--;
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_LVL_SET, 
                           LSTID_FAVORSET_LVL_SET, thrCond);
    }

    // �X�����x���̔��f
    switch (thLevel)
    {
      case NNSH_MSGATTR_FAVOR_L1:
        SetControlValue(frm, PBTNID_FAVORSET_LVL1, &(status));
        break;

      case NNSH_MSGATTR_FAVOR_L2:
        SetControlValue(frm, PBTNID_FAVORSET_LVL2, &(status));
        break;

      case NNSH_MSGATTR_FAVOR_L3:
        SetControlValue(frm, PBTNID_FAVORSET_LVL3, &(status));
        break;

      case NNSH_MSGATTR_FAVOR_L4:
        SetControlValue(frm, PBTNID_FAVORSET_LVL4, &(status));
        break;

      case NNSH_MSGATTR_FAVOR:
        SetControlValue(frm, PBTNID_FAVORSET_LVL5, &(status));
        break;

      case NNSH_MSGATTR_NOTFAVOR:
      default:
        // �w��Ȃ�
        SetControlValue(frm, PBTNID_FAVORSET_LVL0, &(status));
        break;
    }

    // ������AND/OR�ݒ�
    NNshWinSetPopItems(frm, POPTRID_FAVORSET_CONDITIONSET,
                       LSTID_FAVORSET_CONDITIONSET, allCond);

    // ���O�ʒu�̎w��
    status = (customTab->condition & NNSH_CONDITION_LOGLOC_ALL);
    if (status != 0)
    {

        if (status == NNSH_CONDITION_LOGLOC_PALM)
        {
            // Palm��
            allCond = 0;
        }
        else
        {
            // VFS��
            allCond = 1;
        }

        // ���O�ʒu�̃`�F�b�N�{�b�N�X�𔽉f
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_LOGLOC, &(status));

        // ���X�g���X�V
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_LOGLOC_SET,
                           LSTID_FAVORSET_LOGLOC_SET, allCond);
    }

    // �擾�G���[�̐ݒ�
    status = (customTab->condition & NNSH_CONDITION_GETERR_ALL);
    if (status != 0)
    {

        if (status == NNSH_CONDITION_GETERR_NONE)
        {
            // �G���[�Ȃ�
            allCond = 0;
        }
        else
        {
            // �G���[����
            allCond = 1;
        }

        // ���O�ʒu�̃`�F�b�N�{�b�N�X�𔽉f
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_GETERROR, &(status));

        // ���X�g���X�V
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_GETERROR_SET,
                           LSTID_FAVORSET_GETERROR_SET, allCond);
    }

    // �擾�ۗ����̐ݒ�
    status = (customTab->condition & NNSH_CONDITION_GETRSV_ALL);
    if (status != 0)
    {

        if (status == NNSH_CONDITION_GETRSV_RSV)
        {
            // �擾�ۗ����X��
            allCond = 0;
        }
        else
        {
            // ��擾�ۗ����X��
            allCond = 1;
        }

        // �擾�ۗ��̃`�F�b�N�{�b�N�X�𔽉f
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_GETRESERVE, &(status));

        // ���X�g���X�V
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_GETRESERVE_SET,
                           LSTID_FAVORSET_GETRESERVE_SET, allCond);
    }

    // �X����Ԃ̃`�F�b�N�{�b�N�X�ݒ�
    if ((customTab->threadStatus != 0)||(customTab->threadCreate))
    {
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_GETSTATUS, &(status));
    }
    // �X����Ԃ̃`�F�b�N�{�b�N�X�ݒ�
    if ((customTab->stringSet & NNSH_STRING_SELECTION) != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_TITLESET, &(status));
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   Hander_FavorTabSet                                       */
/*                                     ���C�ɓ�������ݒ�C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_FavorTabSet(EventType *event)
{
    UInt16         btnId, status, keyId;
    MemHandle      txtH;
    Char          *ptr, numBuf[TINYBUF];
    FormType      *frm, *diagFrm;
    NNshCustomTab *custom;
    ListType      *lstP;
    FieldType     *fldP;

    // �{�^���`�F�b�N�ȊO�̃C�x���g�͑��߂�
    keyId = 0;
    switch (event->eType)
    { 
      case ctlSelectEvent:
        // �{�^���������ꂽ(���֐i��)
        keyId = event->data.ctlSelect.controlID;
        break;

      case keyDownEvent:
        // �L�[���͂��ꂽ(���֐i��)
        switch (KeyConvertFiveWayToJogChara(event))
        {
          // �n�[�h�L�[�������������̏���
          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrTapWaveSpecificKeyRDown:
          case vchrThumbWheelDown:
          case vchrRockerDown:
          case chrRightArrow:
          case vchrRockerRight:
          case vchrJogRight:
          case vchrTapWaveSpecificKeyRRight:
            keyId = chrRightArrow;
            break;

          // �n�[�h�L�[������������̏���
          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrTapWaveSpecificKeyRUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
          case chrLeftArrow:
          case vchrRockerLeft:
          case vchrJogLeft:
          case vchrTapWaveSpecificKeyRLeft:
            keyId = chrLeftArrow;
            break;

          // �o�b�N�{�^��/ESC�L�[/BS�L�[���������Ƃ��̏���
          // case chrBackspace:
          case vchrJogBack:
          case chrEscape:
          case vchrThumbWheelBack:
          case vchrGarminEscape:
            keyId = BTNID_FAVORSET_CANCEL;
            break;

          // HandEra JOG Push�I��/Enter�L�[���͎��̏���(���X�g�I��)
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
          case vchrRockerCenter:
          case vchrHardRockerCenter:
          case vchrThumbWheelPush:
            keyId = BTNID_FAVORSET_OK;
            break;

#ifdef USE_PIN_DIA
          case winDisplayChangedEvent:
            // �f�B�X�v���C�T�C�Y�ύX�C�x���g
            if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
                (NNshGlobal->device == NNSH_DEVICE_GARMIN))
            {
                frm = FrmGetActiveForm();
                if (HandEraResizeForm(frm) == true)
                {
                    FrmDrawForm(frm);        
                }
            }
            return (false);
            break;
#endif

          default: 
            return (false);
            break;
        }
        break;

      case menuEvent:
        // ���j���[���J���ꂽ
        if (event->data.menu.itemID == MNUID_SHOW_HELP)
        {
            // ����w���v�̕\��
            // (�N���b�v�{�[�h�ɁAnnDA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����)
            ClipboardAddItem(clipboardText, 
                             nnDA_NNSIEXT_VIEWSTART
                             nnDA_NNSIEXT_INFONAME
                             nnDA_NNSIEXT_HELPFAVORTAB
                             nnDA_NNSIEXT_ENDINFONAME
                             nnDA_NNSIEXT_ENDVIEW,
                             sizeof(nnDA_NNSIEXT_VIEWSTART
                                     nnDA_NNSIEXT_INFONAME
                                     nnDA_NNSIEXT_HELPFAVORTAB
                                     nnDA_NNSIEXT_ENDINFONAME
                                     nnDA_NNSIEXT_ENDVIEW));
            // nnDA���N������
            (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        }
        return (false);
        break;

      default: 
        return (false);
        break;
    }

    // ���݊J���Ă����ʂ̐ݒ荀�ڂ��m�F����
    switch (NNshGlobal->jumpSelection)
    {
      case MULTIBTN_FEATURE_USER1TAB:
        custom = &((NNshGlobal->NNsiParam)->custom1);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB5;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB2;
        }
        break;

      case MULTIBTN_FEATURE_USER2TAB:
        custom = &((NNshGlobal->NNsiParam)->custom2);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB1;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB3;
        }
        break;

      case MULTIBTN_FEATURE_USER3TAB:
        custom = &((NNshGlobal->NNsiParam)->custom3);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB2;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB4;
        }
        break;

      case MULTIBTN_FEATURE_USER4TAB:
        custom = &((NNshGlobal->NNsiParam)->custom4);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB3;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB5;
        }
        break;

      case MULTIBTN_FEATURE_USER5TAB:
      default:
        custom = &((NNshGlobal->NNsiParam)->custom5);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB4;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB1;
        }
        break;
    }

    frm = FrmGetActiveForm();
    switch (keyId)
    {
      // �擾��Ԃ̐ݒ�
      case SELTRID_FAVORSET_GETSTATUS:
        diagFrm  = FrmInitForm(FRMID_FAVORSET_THREADSTATE);
        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_NEW) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_NEWGET,     &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_UPDATE) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_PARTGET,    &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_REMAIN) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_NOTREAD,    &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_ALREADY) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_ALREADY,    &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_OVER) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_UPPERLIMIT, &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_NOT_YET) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_NOTYET,     &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_GETERROR) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_ERROR,      &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_UNKNOWN) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_ETC,        &status);

        // �X�����ē� n ���ȍ~�̃f�[�^���f
        status = custom->threadCreate;
        SetControlValue(diagFrm, CHKID_THREAD_DATE, &status);
        if (status != 0)
        {
            MemSet (numBuf, TINYBUF, 0x00);
            NUMCATI(numBuf, status);
            NNshWinSetFieldText(diagFrm, 
                                FLDID_THREAD_DATE, false, numBuf, TINYBUF);
        }

        FrmSetActiveForm(diagFrm);
        btnId = FrmDoDialog(diagFrm);
        if (btnId == BTNID_FAVORSET_ST_OK)
        {
            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_NEWGET,     &status);
            custom->threadStatus = (status << NNSH_SUBJSTATUS_NEW);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_PARTGET,    &status);
            custom->threadStatus = 
                       custom->threadStatus|(status << NNSH_SUBJSTATUS_UPDATE);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_NOTREAD,    &status);
            custom->threadStatus =
                       custom->threadStatus|(status << NNSH_SUBJSTATUS_REMAIN);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_ALREADY,    &status);
            custom->threadStatus = 
                      custom->threadStatus|(status << NNSH_SUBJSTATUS_ALREADY);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_UPPERLIMIT, &status);
            custom->threadStatus = 
                         custom->threadStatus|(status << NNSH_SUBJSTATUS_OVER);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_NOTYET,     &status);
            custom->threadStatus = 
                      custom->threadStatus|(status << NNSH_SUBJSTATUS_NOT_YET);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_ERROR,      &status);
            custom->threadStatus =
                     custom->threadStatus|(status << NNSH_SUBJSTATUS_GETERROR);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_ETC,        &status);
            custom->threadStatus =
                      custom->threadStatus|(status << NNSH_SUBJSTATUS_UNKNOWN);

            // �X�����ē��̎擾
            custom->threadCreate = 0;
            fldP    = FrmGetObjectPtr(diagFrm, 
                                FrmGetObjectIndex(diagFrm, FLDID_THREAD_DATE));
            txtH = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                ptr    = MemHandleLock(txtH);
                if (StrLen(ptr) != 0)
                {
                    custom->threadCreate = StrAToI(ptr);
                }
                MemHandleUnlock(txtH);
            }
        }
        FrmSetActiveForm(frm);
        FrmDeleteForm(diagFrm);

        // ��Ԑݒ���X�V
        status = ((custom->threadStatus != 0)||(custom->threadCreate)) ? 1 : 0;
        SetControlValue(frm, CHKID_FAVORSET_GETSTATUS, &(status));
        break;

      // �^�C�g��������̏ڍאݒ�
      case SELTRID_FAVORSET_TITLESET:
        diagFrm  = FrmInitForm(FRMID_FAVORSET_THREADTITLE);
        FrmSetActiveForm(diagFrm);
        if (custom->string1[0] != '\0')
        {
            NNshWinSetFieldText(diagFrm, FLDID_FAVORSET_TTL_STR1, false,
                                custom->string1, sizeof(custom->string1));
        }
        if (custom->string2[0] != '\0')
        {
            NNshWinSetFieldText(diagFrm, FLDID_FAVORSET_TTL_STR2, false,
                                custom->string2, sizeof(custom->string2));
        }
        if (custom->string3[0] != '\0')
        {
            NNshWinSetFieldText(diagFrm, FLDID_FAVORSET_TTL_STR3, false, 
                                custom->string3, sizeof(custom->string2));
        }

        // ������̏����ݒ�
        status = custom->stringSet & NNSH_STRING_SETMASK;
        NNshWinSetPopItems(diagFrm, 
                           POPTRID_FAVORSET_TTL_CONDSET,
                           LSTID_FAVORSET_TTL_CONDSET, status);

        btnId    = FrmDoDialog(diagFrm);
        if (btnId == BTNID_FAVORSET_TTL_OK)
        {
            // ������P�̎擾
            fldP    = FrmGetObjectPtr(diagFrm, 
                          FrmGetObjectIndex(diagFrm, FLDID_FAVORSET_TTL_STR1));
            txtH    = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                ptr = MemHandleLock(txtH);
                MemSet(custom->string1, MINIBUF, 0x00);
                StrNCopy(custom->string1, ptr, (MINIBUF - 1));
                MemHandleUnlock(txtH);
            }

            // ������Q�̎擾
            fldP    = FrmGetObjectPtr(diagFrm,
                          FrmGetObjectIndex(diagFrm, FLDID_FAVORSET_TTL_STR2));
            txtH    = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                ptr = MemHandleLock(txtH);
                MemSet(custom->string2, MINIBUF, 0x00);
                StrNCopy(custom->string2, ptr, (MINIBUF - 1));
                MemHandleUnlock(txtH);
            }

            // ������R�̎擾
            fldP    =  FrmGetObjectPtr(diagFrm,
                          FrmGetObjectIndex(diagFrm, FLDID_FAVORSET_TTL_STR3));
            txtH    = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                ptr = MemHandleLock(txtH);
                MemSet(custom->string3, MINIBUF, 0x00);
                StrNCopy(custom->string3, ptr, (MINIBUF - 1));
                MemHandleUnlock(txtH);
            }
            lstP  = FrmGetObjectPtr(diagFrm,
                       FrmGetObjectIndex(diagFrm, LSTID_FAVORSET_TTL_CONDSET));
            custom->stringSet = custom->stringSet & NNSH_STRING_SELECTION;
            custom->stringSet = custom->stringSet | LstGetSelection(lstP);
        }
        FrmSetActiveForm(frm);
        FrmDeleteForm(diagFrm);
        break;

      // "Cancel"�{�^���������ꂽ�Ƃ�
      case BTNID_FAVORSET_CANCEL:
        // �l��j��������A�ꗗ��ʂɉ�ʑJ�ڂ���
        FrmEraseForm(frm);
        closeForm_FavorTabSet(NNshGlobal->backFormId);
        break;

      // "OK"�{�^���������ꂽ�Ƃ�
      case BTNID_FAVORSET_OK:
        // �l�𔽉f��A�ꗗ��ʂɉ�ʑJ�ڂ���
        effect_FavorTabSet(frm, custom);
        FrmEraseForm(frm);
        closeForm_FavorTabSet(NNshGlobal->backFormId);
        break;

      case PBTNID_FAVORSET_TAB1:
        // ��ʏ�̒l��ۑ���A��ʑJ��
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER1TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      case PBTNID_FAVORSET_TAB2:
        // ��ʏ�̒l��ۑ���A��ʑJ��
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER2TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      case PBTNID_FAVORSET_TAB3:
        // ��ʏ�̒l��ۑ���A��ʑJ��
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER3TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      case PBTNID_FAVORSET_TAB4:
        // ��ʏ�̒l��ۑ���A��ʑJ��
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER4TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      case PBTNID_FAVORSET_TAB5:
        // ��ʏ�̒l��ۑ���A��ʑJ��
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER5TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      default:
        // ��L�ȊO�i�������Ȃ��j
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effect_FavorTabSet                                       */
/*                         ���C�ɓ�������ݒ���p�����[�^�l�ɔ��f���鏈��  */
/*-------------------------------------------------------------------------*/
static Err effect_FavorTabSet(FormType *frm, NNshCustomTab *custom)
{
    UInt16     status;
    ListType  *lstP;
    FieldType *fldP;
    MemHandle  txtH;
    Char      *txtP;

    // �^�u�^�C�g���̎擾
    fldP    = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, FLDID_FAVORSET_TABTITLE));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        MemSet(custom->tabName, MAX_BOARDNAME, 0x00);
        StrNCopy(custom->tabName, txtP, (MAX_BOARDNAME - 1));
        MemHandleUnlock(txtH);

        // BBS�^�C�g���C���f�b�N�X�p�̗̈�����
        if (NNshGlobal->boardIdxH != 0)
        {
            MemHandleFree(NNshGlobal->boardIdxH);
            NNshGlobal->boardIdxH = 0;
        }
        MEMFREE_PTR((NNshGlobal->bbsTitles));
    }

    // �V���m�F���{�^�u
    lstP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, LSTID_FAVORSET_CHECKNEW));
    custom->newArrival = LstGetSelection(lstP);

    // �ꗗ�擾���{�^�u
    lstP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, LSTID_FAVORSET_GETLIST));
    custom->getList = LstGetSelection(lstP);

    /////////////////////////////////////////////////////////////////////////

    // �����`�F�b�N
    lstP   = FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, LSTID_FAVORSET_CONDITIONSET));
    status = LstGetSelection(lstP);
    if (status != 0)
    {
        // �ǂꂩ��v
        custom->condition = NNSH_CONDITION_OR;
    }
    else
    {
        // �S�Ĉ�v
        custom->condition = NNSH_CONDITION_AND;
    }

    // �Q�ƃ��O�͏��O
    UpdateParameter(frm, CHKID_FAVORSET_NOT_READONLY, &status);
    if (status != 0)
    {
        custom->condition = custom->condition | NNSH_CONDITION_NOTREADONLY;
    }

    // �X�����x���̃`�F�b�N
    UpdateParameter(frm, CHKID_FAVORSET_LEVEL, &(status));
    if (status != 0)
    {
        // �X�����x���𔽉f������
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm, LSTID_FAVORSET_LVL_SET));
        custom->threadLevel =
                          (LstGetSelection(lstP) + 1) << NNSH_THREADCOND_SHIFT;
        UpdateParameter(frm, PBTNID_FAVORSET_LVL1, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR_L1;
            goto SET_CONDITION;
        }
        UpdateParameter(frm, PBTNID_FAVORSET_LVL2, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR_L2;
            goto SET_CONDITION;
        }
        UpdateParameter(frm, PBTNID_FAVORSET_LVL3, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR_L3;
            goto SET_CONDITION;
        }
        UpdateParameter(frm, PBTNID_FAVORSET_LVL4, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR_L4;
            goto SET_CONDITION;
        }
        UpdateParameter(frm, PBTNID_FAVORSET_LVL5, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR;
            goto SET_CONDITION;
        }
#if 0
        UpdateParameter(frm, PBTNID_FAVORSET_LVL0, &status);
        if (status != 0)
        {
            status = 0;
            goto SET_CONDITION;
        }
        status = 0;
#endif

SET_CONDITION:
        custom->threadLevel = custom->threadLevel|(status);
    }
    else
    {
        custom->threadLevel = 0;
    }
     
    // �ݒ�̃`�F�b�N
    UpdateParameter(frm, CHKID_FAVORSET_BBSNICK, &(status));
    if (status != 0)
    {
        // �ݒ�𔽉f������
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm, LSTID_FAVORSET_BBSNICK));
        custom->boardNick = LstGetSelection(lstP);
        if (custom->boardNick < NNSH_NOF_SPECIAL_BBS)
        {
            // �ݒ�́A�ʏ�̔��w�肳��Ă��邱��
            NNsh_WarningMessage(ALTID_WARN, MSG_WARN_NOTEFFECT_BBS, "", 0);

            // �ݒ���N���A
            custom->boardCondition = 0;
            goto FUNC_END;
        }
        lstP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, LSTID_FAVORSET_BBSNICK_SET));
        custom->boardCondition = LstGetSelection(lstP) + 1;
    }
    else
    {
        custom->boardCondition = 0;
    }

    // ���O�ʒu�̎w��
    UpdateParameter(frm, CHKID_FAVORSET_LOGLOC, &(status));
    if (status != 0)
    {
        // ���O�ʒu�f�[�^�𔽉f������
        lstP = FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, LSTID_FAVORSET_LOGLOC_SET));
        status = LstGetSelection(lstP);
        if (status == 0)
        {
            custom->condition = custom->condition | NNSH_CONDITION_LOGLOC_PALM;
        }
        else
        {
            custom->condition = custom->condition | NNSH_CONDITION_LOGLOC_VFS;
        }
    }

    // �擾�G���[�̎w��
    UpdateParameter(frm, CHKID_FAVORSET_GETERROR, &(status));
    if (status != 0)
    {
        // �擾�G���[�f�[�^�𔽉f������
        lstP = FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, LSTID_FAVORSET_GETERROR_SET));
        status = LstGetSelection(lstP);
        if (status == 0)
        {
            custom->condition = custom->condition|NNSH_CONDITION_GETERR_NONE;
        }
        else
        {
            custom->condition = custom->condition|NNSH_CONDITION_GETERR_ERROR;
        }
    }

    // �擾�ۗ��̎w��
    UpdateParameter(frm, CHKID_FAVORSET_GETRESERVE, &(status));
    if (status != 0)
    {
        // �擾�ۗ��f�[�^�𔽉f������
        lstP = FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, LSTID_FAVORSET_GETRESERVE_SET));
        status = LstGetSelection(lstP);
        if (status == 0)
        {
            custom->condition = custom->condition | NNSH_CONDITION_GETRSV_RSV;
        }
        else
        {
            custom->condition = custom->condition | NNSH_CONDITION_GETRSV_NONE;
        }
    }

    // �X����Ԃ̊m�F 
    UpdateParameter(frm, CHKID_FAVORSET_GETSTATUS, &(status));
    if (status == 0)
    {
        // �X����Ԃ��N���A����
        custom->threadStatus = 0;    
    }

    // ������̐ݒ�
    UpdateParameter(frm, CHKID_FAVORSET_TITLESET, &(status));
    if (status != 0)
    {
        custom->stringSet = custom->stringSet | NNSH_STRING_SELECTION;
    }
    else
    {
        custom->stringSet = custom->stringSet & NNSH_STRING_SETMASK;
    }

FUNC_END:
    return (errNone);
}

#ifdef USE_COLOR
/*-------------------------------------------------------------------------*/
/*   Function :   Handler_setViewColorDetail                               */
/*                                      �Q�Ɖ�ʂ̐F�ݒ�p�C�x���g�n���h�� */
/*-------------------------------------------------------------------------*/
Boolean Handler_setViewColorDetail(EventType *event)
{
    UInt8            *paramPtr;
    IndexedColorType  color;

    if (event->eType != ctlSelectEvent)
    {
        // �������Ȃ�
        return (false);
    }

    switch (event->data.ctlSelect.controlID)
    {
      case SELTRID_NNSISET_COLOR_LETTER:
        // �����̐F
        paramPtr = &((NNshGlobal->NNsiParam)->colorViewFG);
        break;

      case SELTRID_NNSISET_COLOR_HEADER:
        // �w�b�_�̐F
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgHeader);
        break;

      case SELTRID_NNSISET_COLOR_BOLDHEADER:
        // �w�b�_�i�����j�̐F
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgHeaderBold);
        break;

      case SELTRID_NNSISET_COLOR_BACKGROUND:
        // �w�i�̐F
        paramPtr = &((NNshGlobal->NNsiParam)->colorViewBG);
        break;

      case SELTRID_NNSISET_COLOR_FOOTER:
        // �t�b�^�̐F
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgFooter);
        break;

      case SELTRID_NNSISET_COLOR_BOLDFOOTER:
        //�@�t�b�^(����)�̐F
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgFooterBold);
        break;

      case SELTRID_NNSISET_COLOR_MARKING:
        // �}�[�N�̐F
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgInform);
        break;

      case SELTRID_NNSISET_COLOR_BOLDMARKING:
        // �}�[�N�i�����j�̐F
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgInformBold);
        break;
        
      default:
        // �������Ȃ�
        return (false);
        break;
    }
    // �F��I������
    color = *paramPtr;
    if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
    {
        // �ݒ肵���F�𔽉f������
        *paramPtr = color;
    }
    FrmDrawForm(FrmGetActiveForm());
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   selectOmitDialog                                         */
/*                                          �m�F�_�C�A���O�̏ȗ�(�ڍאݒ�) */
/*-------------------------------------------------------------------------*/
static Boolean setViewColorDetail(void)
{
    Boolean       ret = false;
    FormType     *prevFrm, *diagFrm;
    UInt16        btnId;
    FieldType    *fldP;
    MemHandle     txtH;
    Char         *nameP, separator[SEPARATOR_LEN];

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // �_�C�A���O�\������
    PrepareDialog_NNsh(FRMID_NNSISET_VIEWCOLOR, &diagFrm, true);
    if (diagFrm == NULL)
    {
        return (false);
    }

    // �����f�[�^�̐ݒ�
    // �w�b�_/�t�b�^�̐F�����\�������{
    btnId = (NNshGlobal->NNsiParam)->useColorMessageToken;
    SetControlValue(diagFrm, CHKID_NNSISET_USEVIEWCOLOR, &btnId);

    // nnDA�g�p���A���X��pop up �\������
    btnId = (NNshGlobal->NNsiParam)->showResPopup;
    SetControlValue(diagFrm, CHKID_NNSISET_USE_POPUP, &btnId);

    // ���݂̃��b�Z�[�W��؂蕶������擾
    MemSet (separator, sizeof(separator), 0x00);
    StrCopy(separator, &((NNshGlobal->NNsiParam)->messageSeparator[1]));
    
    // �����̕���������(���s�R�[�h�������Ă��邩��)
    nameP = separator;
    while ((*nameP != '\n')&&(*nameP != '\0'))
    {
        nameP++;
    }
    *nameP = '\0';

    // ���b�Z�[�W�̋�؂蕶����̌��ݐݒ�l���E�B���h�E�ɔ��f������
    NNshWinSetFieldText(diagFrm, FLDID_ZONE_SEPARATOR, true, separator, sizeof(separator));
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_ZONE_SEPARATOR));

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_setViewColorDetail);

    // �E�B���h�E�̕\��
    btnId = FrmDoDialog(diagFrm);

    // �f�[�^�̃w�b�_/�t�b�^�̐F�����\��
    UpdateParameter(diagFrm, CHKID_NNSISET_USEVIEWCOLOR, &btnId);
    (NNshGlobal->NNsiParam)->useColorMessageToken = btnId;

    // �f�[�^�̃w�b�_/�t�b�^�̋����\��
    UpdateParameter(diagFrm, CHKID_NNSISET_USE_POPUP, &btnId);
    (NNshGlobal->NNsiParam)->showResPopup = btnId;

    // ��؂蕶����̎擾
    fldP = FrmGetObjectPtr(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_ZONE_SEPARATOR));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        nameP = MemHandleLock(txtH);
        if (*nameP != '\0')
        {
            (NNshGlobal->NNsiParam)->messageSeparator[0] = '\n';
            StrNCopy(&((NNshGlobal->NNsiParam)->messageSeparator[1]),
                     nameP, (SEPARATOR_LEN - MARGIN));
            StrCat((NNshGlobal->NNsiParam)->messageSeparator, "\n");
        }
        MemHandleUnlock(txtH);        
    }

    // �_�C�A���O�\���̌�Еt��
    PrologueDialog_NNsh(prevFrm, diagFrm, true);

    return (ret);
}
#endif  // #ifdef USE_COLOR

#ifdef USE_LOGCHARGE
/*=========================================================================*/
/*   Function :  GetLog_SetHardKeyFeature                                  */
/*                                    �Q�ƃ��O�ꗗ��ʂ̃n�[�h�L�[�@�\�ݒ� */
/*=========================================================================*/
void GetLog_SetHardKeyFeature(void)
{
    // ���X�g������̃N���A
    clearFeatureList();
      
    // �Q�ƃ��O�ꗗ��ʋ@�\���X�g�̐ݒ�
    createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREGETLOG_STRINGS);

    // �n�[�h�L�[�ڍאݒ�
    (void) SetTitleHardKey(NNSH_TITLE_GETLOGLIST_TITLE, GETLOGBTN_FEATURE, 
                            GETLOGBTN_FEATURE_MASK, &((NNshGlobal->NNsiParam)->getLogFeature));

    //  �ꕔ�n�[�h�L�[�̐���́ANNsi�����ݒ�̃n�[�h�L�[����ݒ肪�K�v��
    // ���Ƃ��x������B
    NNsh_WarningMessage(ALTID_WARN, MSG_WARN_SETTING_HARDKEY, "", 0);

    // ���X�g������̃N���A
    clearFeatureList();

    return;
}
#endif // #ifdef USE_LOGCHARGE
