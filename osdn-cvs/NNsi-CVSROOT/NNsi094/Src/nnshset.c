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
/*   Function :   effect_NNshSetting                                       */
/*                             NNsi�ݒ��ʂ���p�����[�^�l�ɔ��f���鏈��  */
/*-------------------------------------------------------------------------*/
static Err effect_NNshSetting(FormType *frm)
{
    ControlType *chkObj, *chkObjSub1, *chkObjSub2, *chkObjSub3;

    /***  NNsh�ݒ��(�O���[�o���ϐ���)���f������  ***/

    // �X���ꗗ(subject.txt)�S�擾
    UpdateParameter(frm, CHKID_SUBJECT_ALL, &(NNshParam->getAllThread));

    // �m�F���b�Z�[�W���ȗ�
    UpdateParameter(frm, CHKID_OMIT_DIALOG, &(NNshParam->confirmationDisable));

    // �擾��ɃX���Q��
    UpdateParameter(frm, CHKID_OPEN_AUTO,   &(NNshParam->openAutomatic));

    // sage��������
    UpdateParameter(frm, CHKID_WRITE_SAGE,  &(NNshParam->writeAlwaysSage));

    // �������ɁA�啶������������ʂ��Ȃ�
    UpdateParameter(frm, CHKID_SEARCH_CASELESS, &(NNshParam->searchCaseless));

    // �N������Offline�X��������
    UpdateParameter(frm, CHKID_OFFCHK_LAUNCH,   &(NNshParam->offChkLaunch));

    // �A���_�[���C��OFF
    UpdateParameter(frm, CHKID_MSG_LINEOFF, &(NNshParam->disableUnderline));

    // DEBUG
    UpdateParameter(frm, CHKID_DEBUG_DIALOG,&(NNshParam->debugMessageON));

    // �X���ꗗ��MSG��
    UpdateParameter(frm, CHKID_PRINT_NOFMSG,&(NNshParam->printNofMessage));

    // VFS ���p�\���Ɏ����I��VFS ON
    UpdateParameter(frm, CHKID_VFSON_AUTOMATIC,&(NNshParam->vfsOnAutomatic));

    // �X���ԍ����y�z�ɂ���
    UpdateParameter(frm, CHKID_BOLD_MSGNUM, &(NNshParam->boldMessageNum));

    // VFS�̎g�p�Ɖ���t���O�AOFFLINE���O�g�p�t���O�̐ݒ�
    if (NNshParam->useVFS != NNSH_NOTSUPPORT_VFS)
    {
        chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_USE_VFS));
        chkObjSub1 = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
        chkObjSub2 = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
        chkObjSub3 = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
        NNshParam->useVFS = CtlGetValue(chkObj);
        if (CtlGetValue(chkObjSub1) != 0)
        {
            NNshParam->useVFS = NNshParam->useVFS | NNSH_VFS_WORKAROUND;
        }
        if (CtlGetValue(chkObjSub2) != 0)
        {
            NNshParam->useVFS = NNshParam->useVFS | NNSH_VFS_USEOFFLINE;
        }
        if (CtlGetValue(chkObjSub3) != 0)
        {
            NNshParam->useVFS = NNshParam->useVFS | NNSH_VFS_DBBACKUP;
        }

        if (NNshParam->useVFS != 0)
        {
            //  VFS�֘A�p�����[�^�̐ݒ肪����Ă����Ƃ��A����N�������
            // �L���ƂȂ邱�Ƃ�\������B
            NNsh_InformMessage(ALTID_WARN, MSG_NNSISET_VFSWARN, "", 0);
        }

        // logdir.txt��ǂ݁AOFFLINE���O�x�[�X�f�B���N�g�����擾
        (void) setOfflineLogDir(LOGDIR_FILE);
    }

    // ��������g���ꍇ�̐ݒ�
    if (NNshParam->useBookmark == 0)
    {
        // ��������g��Ȃ��ꍇ�A������ݒ�̃N���A
        NNshParam->lastFrmID     = FRMID_THREAD;
        NNshParam->bookMsgNumber = 0;
        NNshParam->bookMsgIndex  = 0;
    }
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting2                                       */
/*                                             NNsi�ݒ�2�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting2(FormType *frm)
{
    FieldType   *fldP;
    UInt32       bufSize;
    UInt16       timeout;
    Char        *logBuf, *numP;
    MemHandle    txtH;

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
        if (NNshParam->netTimeout != timeout * bufSize)
        {
            NNshParam->netTimeout = timeout * bufSize;

            // TIMEOUT�̍X�V����\��
            NNsh_DebugMessage(ALTID_INFO, MSG_TIMEOUT_UPDATED, 
                              "", NNshParam->netTimeout);
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
            logBuf = MemPtrNew(BUFSIZE);
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
                MemPtrFree(logBuf);
            }
            return (~errNone);
        }
        else
        {
            if (bufSize == NNshParam->bufferSize)
            {
                // �o�b�t�@�T�C�Y�͕ύX���Ȃ������|�񍐂��A�I������
                NNsh_DebugMessage(ALTID_INFO, MSG_WARN_NOTMODIFIED,
                                  "buffer size:", NNshParam->bufferSize);
            }
            else
            {
                // �o�b�t�@�T�C�Y��ύX���ďI������
                NNshParam->bufferSize = bufSize;
                NNsh_InformMessage(ALTID_INFO, MSG_INFO_MODIFIED,
                                   "buffer size:", NNshParam->bufferSize);
            }
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "0", 0);
    }

    // �����_�E�����[�h�w����擾����
    UpdateParameter(frm, CHKID_DL_PART, &(NNshParam->enablePartGet));

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
        if (bufSize > NNshParam->bufferSize)
        {
            // �_�E�����[�h�T�C�Y�́A���[�N�o�b�t�@�T�C�Y����������
            NNsh_ErrorMessage(ALTID_WARN, MSG_DOWNLOADSIZE_OVER, "", 0);
            return (~errNone);
        }
        if (bufSize != NNshParam->partGetSize)
        {
            NNshParam->partGetSize = bufSize;

            // �_�E�����[�h�T�C�Y�̍X�V��\��
            NNsh_DebugMessage(ALTID_INFO, MSG_DOWNLOADSIZE_UPDATE, "",
                              NNshParam->partGetSize);
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "1", 0);
    }

    // �R�e�n���@�\���g�p����ݒ�
    UpdateParameter(frm, CHKID_FIXED_HANDLE, &(NNshParam->useFixedHandle));

    // �n���h�����̐ݒ�(�n���h�������ݒ�ł��Ȃ��Ă����퉞������)
    MemSet(NNshParam->handleName, BUFSIZE, 0x00);
    fldP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, FLDID_HANDLENAME));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        MemSet(NNshParam->handleName, BUFSIZE, 0x00);
        if (*numP != '\0')
        {
            StrNCopy(NNshParam->handleName, numP, (BUFSIZE - 1));
        }
        MemHandleUnlock(txtH);
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "3", 0);
    }

    // BBS�ꗗ�擾��̐ݒ�
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_URL_BBS));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // �t�B�[���h����URL���擾����
        numP = MemHandleLock(txtH);
        MemSet  (NNshParam->bbs_URL, MAX_URL, 0x00);
        StrNCopy(NNshParam->bbs_URL, numP, (MAX_URL - 1));
        MemHandleUnlock(txtH);
    }

    // BBS�ꗗ�擾��̐ݒ�
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_TITLE_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // �t�B�[���h����X�������񐔂��擾����
        numP = MemHandleLock(txtH);
        if (numP != NULL)
        {
            bufSize = StrAToI(numP);
            MemHandleUnlock(txtH);
        }
        else
        {
            bufSize = LIMIT_TITLENAME_DEFAULT;
        }
        if ((bufSize > MAX_THREADNAME)||(bufSize < LIMIT_TITLENAME_MINIMUM))
        {
            // �f�[�^�l�ُ�A���ُ͔͈͈��\�����Č��ɖ߂�
            logBuf = MemPtrNew(BUFSIZE);
            if (logBuf != NULL)
            {
                MemSet (logBuf, BUFSIZE, 0x00);
                StrCat (logBuf, MSG_THREADNAME_ILLEGAL);
                NUMCATI(logBuf, bufSize);
                StrCat (logBuf, MSG_THREADNAME_ILLEGAL2);
                NUMCATI(logBuf, LIMIT_TITLENAME_MINIMUM);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE3);
                NUMCATI(logBuf, MAX_THREADNAME);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE4);
                NUMCATI(logBuf, LIMIT_TITLENAME_DEFAULT);
                NNsh_ErrorMessage(ALTID_ERROR, logBuf, "", 0);
                MemPtrFree(logBuf);
            }
            return (~errNone);
        }
        else
        {
            // �T�C�Y���m�F����
            if (bufSize == NNshParam->titleListSize)
            {
                // �T�C�Y�͕ύX���Ȃ������|�񍐂��A�I������
                NNsh_DebugMessage(ALTID_INFO, MSG_WARN_NOTMODIFIED,
                                  "title length:", NNshParam->titleListSize);
            }
            else
            {
                NNshParam->titleListSize = bufSize;
                NNsh_InformMessage(ALTID_INFO, MSG_INFO_MODIFIED,
                                   "title length:", NNshParam->titleListSize);
            }
        }
    }

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting3                                       */
/*                                             NNsi�ݒ�3�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting3(FormType *frm)
{
    FieldType   *fldP;
    UInt32       portNum;
    Char        *proxyP;
    MemHandle    txtH;

    // NNsi�I�����ɉ����ؒf����
    UpdateParameter(frm, CHKID_END_OFFLINE, &(NNshParam->disconnectNNsiEnd));

    // �u�V���m�F�v�I�����ɉ����ؒf����
    UpdateParameter(frm, CHKID_DISCONN_ARRIVAL, &(NNshParam->disconnArrivalEnd));

    // �u�V���m�F�v�I����ɖ��ǈꗗ��\������
    UpdateParameter(frm, CHKID_ARRIVAL_NOTREAD, &(NNshParam->autoOpenNotRead));

    // ��MCookie���g�p���ď�������
    UpdateParameter(frm, CHKID_WRITE_USE_COOKIE, &(NNshParam->useCookieWrite));

    // ����ڑ����ʂ��ĕ`��
    UpdateParameter(frm,CHKID_REDRAW_CONNECT,&(NNshParam->redrawAfterConnect));

    // �������ݎ��A�������݃t�B�[���h�Ɏ����I�Ƀt�H�[�J�X���ړ����Ȃ�
    UpdateParameter(frm, CHKID_NOT_AUTOFOCUSSET,&(NNshParam->notAutoFocus));

    // �Q�Ɛ�p�փR�s�[�����Ƃ��A�����ɃX���폜�����{
    UpdateParameter(frm, CHKID_COPYDEL_READONLY,&(NNshParam->copyDelReadOnly));

    // �擾�ςݑS�Ă͎Q�Ɛ�p���O�ɕ\�����Ȃ�
    UpdateParameter(frm, CHKID_NOT_READONLY, &(NNshParam->notListReadOnly));

    // �������ݎ��Q�ƃX���ԍ���}������
    UpdateParameter(frm, CHKID_INSERT_REPLYNUM, &(NNshParam->insertReplyNum));

    // Proxy�o�R�ł̃A�N�Z�X
    UpdateParameter(frm, CHKID_USE_PROXY, &(NNshParam->useProxy));

    // Proxy URL
    fldP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, FLDID_USE_PROXY));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        proxyP    = MemHandleLock(txtH);
        MemSet(NNshParam->proxyURL, MAX_URL, 0x00);
        if (*proxyP != '\0')
        {
            StrNCopy(NNshParam->proxyURL, proxyP, (MAX_URL - 1));
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
        proxyP  = MemHandleLock(txtH);
        portNum = StrAToI(proxyP);
        MemHandleUnlock(txtH);

        NNshParam->proxyPort = portNum;
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "PROXY PORT", 0);
    }

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting4                                       */
/*                                             NNsi�ݒ�4�̏��𔽉f������ */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting4(FormType *frm)
{
    UInt16 butID;

    // Bookmark(������ݒ�)
    UpdateParameter(frm, CHKID_USE_BOOKMARK,&(NNshParam->useBookmark));

    // PUSH ON�_�C�����Ł}�P
    UpdateParameter(frm, CHKID_NOT_CURSOR, &(NNshParam->notCursor));

    // �Q�Ǝ�Back�{�^����JumpList
    UpdateParameter(frm, CHKID_BACKBTN_JUMP, &(NNshParam->jogBackBtnAsGo));

    // �Q�Ǝ�JumpList�Ɂu�ꗗ�ցv
    UpdateParameter(frm, CHKID_ADD_RTNTOLIST, &(NNshParam->addReturnToList));

    // PalmOS5�p�@�\���g�p����
    UpdateParameter(frm, CHKID_USE_ARMLET, &(NNshParam->useARMlet));

    // WebBrowser���J���Ƃ��ŐV50
    UpdateParameter(frm, CHKID_WEBBROWSE_LAST50, &butID);
    if (butID != 0)
    {
        NNshParam->browseMesNum = 50;
    }
    else
    {
        NNshParam->browseMesNum = 0;
    }

    // �X���Q�Ɖ�ʂ�tiny/small�t�H���g���g�p����
    UpdateParameter(frm,CHKID_CLIE_USE_TINYFONT,
                                      &(NNshParam->useSonyTinyFont));

    // ���̋@�\��OS5��p�̋@�\�Ȃ��߁A�{���Ɏ��{���邩�m�F��\������
    if (NNshParam->useARMlet != 0)
    {
        butID = NNsh_ErrorMessage(ALTID_CONFIRM, MSG_WARNING_OS5, "", 0);
        if (butID != 0)
        {
            NNshParam->useARMlet = 0;
        }
    }

    // �ꗗ��Back�{�^���Ń��j���[�\��
    UpdateParameter(frm, CHKID_BACKBTN_LIST, &NNshParam->jogBackBtnAsMenu);

#ifdef USE_CLIE
    // Silk������s��Ȃ�
    UpdateParameter(frm, CHKID_NOTUSE_SILK, &NNshGlobal->notUseSilk);

    // SILK����͍s��Ȃ��Ƃ��́A�x����\������
    if (NNshGlobal->notUseSilk != NNshParam->notUseSilk)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_SILK_NEXT_LAUNCH, "", 0);
    }
#endif

    // DB���X�V���ꂽ�������
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*=========================================================================*/
/*   Function :   Hander_NNshSetting                                       */
/*                                     NNsi�ݒ�n��(����)�C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_NNshSetting(EventType *event)
{
    FormType    *frm;
    ControlType *chkObj, *chkObjSub1, *chkObjSub2, *chkObjSub3;
    UInt16       formID;
    Err          ret;
    void        *effectFunc;

    // �{�^���`�F�b�N�ȊO�̃C�x���g�͑��߂�
    switch (event->eType)
    { 
      case menuEvent:
        // ���j���[�I��
        return (NNsh_MenuEvt_Edit(event));
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ(���֐i��)
        break;

      default: 
        return (false);
        break;
    }

    effectFunc = NULL;
    formID     = FRMID_THREAD;
    frm        = FrmGetActiveForm();
    switch (event->data.ctlSelect.controlID)
    {
      // "OK"�{�^���������ꂽ�Ƃ�(NNsi�ݒ�)
      case BTNID_CONFIG_NNSH_OK:
        formID     = FRMID_THREAD;
        effectFunc = &effect_NNshSetting;
        break;
      // "2"�{�^���������ꂽ�Ƃ�
      case NNSHSET_2:
        formID     = FRMID_NNSI_SETTING2; 
        effectFunc = &effect_NNshSetting;
        break;
      // "3"�{�^���������ꂽ�Ƃ�
      case NNSHSET_3:
        formID     = FRMID_NNSI_SETTING3; 
        effectFunc = &effect_NNshSetting;
        break;
      // "4"�{�^���������ꂽ�Ƃ�
      case NNSHSET_4:
        formID     = FRMID_NNSI_SETTING4; 
        effectFunc = &effect_NNshSetting;
        break;

      // "OK"�{�^���������ꂽ�Ƃ�(NNsi�ݒ�-2)
      case BTNID_NNSI_SET_OK:
        formID     = FRMID_THREAD;
        effectFunc = &effectNNsiSetting2;
        break;
      // "1"�{�^���������ꂽ�Ƃ�
      case NNSHSET2_1:
        formID     = FRMID_CONFIG_NNSH;
        effectFunc = &effectNNsiSetting2;
        break;
      // "3"�{�^���������ꂽ�Ƃ�
      case NNSHSET2_3:
        formID     = FRMID_NNSI_SETTING3; 
        effectFunc = &effectNNsiSetting2;
        break;
      // "4"�{�^���������ꂽ�Ƃ�
      case NNSHSET2_4:
        formID     = FRMID_NNSI_SETTING4; 
        effectFunc = &effectNNsiSetting2;
        break;

      // "OK"�{�^���������ꂽ�Ƃ�(NNsi�ݒ�-3)
      case BTNID_NNSISET3_OK:
        formID     = FRMID_THREAD;
        effectFunc = &effectNNsiSetting3;
        break;
      // "1"�{�^���������ꂽ�Ƃ�
      case NNSHSET3_1:
        formID     = FRMID_CONFIG_NNSH;
        effectFunc = &effectNNsiSetting3;
        break;
      // "2"�{�^���������ꂽ�Ƃ�
      case NNSHSET3_2:
        formID     = FRMID_NNSI_SETTING2; 
        effectFunc = &effectNNsiSetting3;
        break;
      // "4"�{�^���������ꂽ�Ƃ�
      case NNSHSET3_4:
        formID     = FRMID_NNSI_SETTING4;
        effectFunc = &effectNNsiSetting3;
        break;

      // "OK"�{�^���������ꂽ�Ƃ�(NNsi�ݒ�-4)
      case BTNID_NNSISET4_OK:
        formID     = FRMID_THREAD;
        effectFunc = &effectNNsiSetting4;
        break;
      // "1"�{�^���������ꂽ�Ƃ�
      case NNSHSET4_1:
        formID     = FRMID_CONFIG_NNSH;
        effectFunc = &effectNNsiSetting4;
        break;
      // "2"�{�^���������ꂽ�Ƃ�
      case NNSHSET4_2:
        formID     = FRMID_NNSI_SETTING2; 
        effectFunc = &effectNNsiSetting4;
        break;
      // "3"�{�^���������ꂽ�Ƃ�
      case NNSHSET4_3:
        formID     = FRMID_NNSI_SETTING3;
        effectFunc = &effectNNsiSetting4;
        break;

      // "Cancel"�{�^���������ꂽ�Ƃ�
      case BTNID_CONFIG_NNSH_CANCEL:
      case BTNID_NNSI_SET_CANCEL:
      case BTNID_NNSISET3_CANCEL:
      case BTNID_NNSISET4_CANCEL:
        effectFunc = NULL;
        break;

      case CHKID_USE_VFS:
        // VFS�؂�ւ��ݒ�𔽉f������
        if (NNshParam->useVFS != NNSH_NOTSUPPORT_VFS)
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
            }
            FrmDrawForm(frm);
        }
        return (false);
        break;

      case CHKID_USE_BBS_NEWURL:
        // �W��URL�`�F�b�N�{�b�N�X�������ꂽ���ABBS�ꗗ�擾���W��URL�ɕύX
        NNshWinSetFieldText(frm, FLDID_URL_BBS, URL_BBSTABLE,
                                                        StrLen(URL_BBSTABLE));
        NNsh_InformMessage(ALTID_INFO, MSG_CHANGED_URL_NEW, "", 0);
        chkObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_BBS_NEWURL));
        CtlSetValue(chkObj, 0);
        return (false);
        break;

      case CHKID_USE_BBS_OLDURL:
        // ��URL�`�F�b�N�{�b�N�X�������ꂽ�Ƃ��ABBS�ꗗ�擾�����URL�ɕύX����
        NNshWinSetFieldText(frm, FLDID_URL_BBS, URL_BBSTABLE_OLD,
                                                    StrLen(URL_BBSTABLE_OLD));
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
    FrmEraseForm(frm);
    FrmGotoForm(formID);
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNshSetting                                     */
/*                                                NNsi�ݒ�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNshSetting(FormType *frm)
{
    ControlType *chkObj, *chkObjSub1, *chkObjSub2, *chkObjSub3;

    /***  NNsh�ݒ��(�O���[�o���ϐ�����)���f������  ***/

    // �X���ꗗ(subject.txt)�S�擾
    SetControlValue(frm, CHKID_SUBJECT_ALL, &(NNshParam->getAllThread));

    // �m�F���b�Z�[�W���ȗ�
    SetControlValue(frm, CHKID_OMIT_DIALOG, &(NNshParam->confirmationDisable));

    // �擾��ɃX���Q��
    SetControlValue(frm, CHKID_OPEN_AUTO,   &(NNshParam->openAutomatic));

    // sage��������
    SetControlValue(frm, CHKID_WRITE_SAGE,  &(NNshParam->writeAlwaysSage));

    // �������ɁA�啶������������ʂ��Ȃ�
    SetControlValue(frm, CHKID_SEARCH_CASELESS, &(NNshParam->searchCaseless));

    // �N������Offline�X��������
    SetControlValue(frm, CHKID_OFFCHK_LAUNCH,   &(NNshParam->offChkLaunch));

    // VFS ���p�\���Ɏ����I��VFS ON
    SetControlValue(frm, CHKID_VFSON_AUTOMATIC,&(NNshParam->vfsOnAutomatic));

    // �A���_�[���C��OFF
    SetControlValue(frm, CHKID_MSG_LINEOFF, &(NNshParam->disableUnderline));

    // DEBUG
    SetControlValue(frm, CHKID_DEBUG_DIALOG,&(NNshParam->debugMessageON));

    // �X���ꗗ��MSG��
    SetControlValue(frm, CHKID_PRINT_NOFMSG,&(NNshParam->printNofMessage));

    // �X���ԍ����y�z�ɂ���
    SetControlValue(frm, CHKID_BOLD_MSGNUM, &(NNshParam->boldMessageNum));

    // �uVFS�̎g�p�v�͂P�̃p�����[�^�ŕ����̈Ӗ����������L���Ă��邽��
    chkObj     = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_USE_VFS));
    chkObjSub1 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
    chkObjSub2 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
    chkObjSub3 = 
                 FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
    switch (NNshParam->useVFS)
    {
      // VFS��T�|�[�g�@�̏ꍇ�A�uVFS�̎g�p�v�ݒ�͕\�����Ȃ��B
      case NNSH_NOTSUPPORT_VFS:
        CtlSetUsable(chkObj,     false);
        CtlSetUsable(chkObjSub1, false);
        CtlSetUsable(chkObjSub2, false);
        CtlSetUsable(chkObjSub3, false);
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
        break;

      case NNSH_VFS_ENABLE:
      default:
        CtlSetValue  (chkObj,     NNSH_VFS_ENABLE);
        CtlSetUsable (chkObjSub1, true);
        CtlSetValue  (chkObjSub1, (NNSH_VFS_WORKAROUND & NNshParam->useVFS));

        CtlSetUsable (chkObjSub2, true);
        CtlSetValue  (chkObjSub2, (NNSH_VFS_USEOFFLINE & NNshParam->useVFS));

        CtlSetUsable (chkObjSub3, true);
        CtlSetValue  (chkObjSub3, (NNSH_VFS_DBBACKUP & NNshParam->useVFS));
        break;
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting2                                    */
/*                                               NNsi�ݒ�2�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting2(FormType  *frm)
{
    UInt16       fieldIndex, timeout;
    Char         bufSize[MINIBUF];

    // �t�B�[���h�Ɍ��݂̃^�C���A�E�g�l�𔽉f������
    MemSet(bufSize, sizeof(bufSize), 0x00);
    timeout = SysTicksPerSecond();
    timeout = (timeout == 0) ? 1 : timeout;  // �[�����Z�̉��(�ی�)
    StrIToA(bufSize, (NNshParam->netTimeout / timeout));
    NNshWinSetFieldText(frm, FLDID_COMM_TIMEOUT, bufSize, StrLen(bufSize));

    // �t�B�[���h�Ɍ��݂̃o�b�t�@�T�C�Y�𔽉f������
    MemSet(bufSize, sizeof(bufSize), 0x00);
    StrIToA(bufSize, NNshParam->bufferSize);
    NNshWinSetFieldText(frm, FLDID_BUFFER_SIZE, bufSize, StrLen(bufSize));

    // �t�B�[���h�Ɍ��݂̕����擾�T�C�Y�𔽉f������
    MemSet(bufSize, sizeof(bufSize), 0x00);
    StrIToA(bufSize, NNshParam->partGetSize);
    NNshWinSetFieldText(frm, FLDID_DL_PARTSIZE, bufSize, StrLen(bufSize));

    // �t�B�[���h�Ɍ��݂̃X���^�C�g���������𔽉f������
    MemSet(bufSize, sizeof(bufSize), 0x00);
    StrIToA(bufSize, NNshParam->titleListSize);
    NNshWinSetFieldText(frm, FLDID_TITLE_SIZE, bufSize, StrLen(bufSize));

    // �����_�E�����[�h�w�����ʂɔ��f����
    SetControlValue(frm, CHKID_DL_PART, &(NNshParam->enablePartGet));

    // BBS�ꗗ�擾��URL�̐ݒ�
    NNshWinSetFieldText(frm, FLDID_URL_BBS, NNshParam->bbs_URL, MAX_URL);

    // �R�e�n���@�\���g�p����ݒ����ʂɔ��f����
    SetControlValue(frm, CHKID_FIXED_HANDLE, &(NNshParam->useFixedHandle));

    // �n���h�����̐ݒ�
    if (NNshParam->handleName[0] != '\0')
    { 
        NNshWinSetFieldText(frm, FLDID_HANDLENAME,
                            NNshParam->handleName, BUFSIZE);
    }

    // �t�H�[�J�X���o�b�t�@�T�C�Y���͂ֈړ�
    fieldIndex = FrmGetObjectIndex(frm, FLDID_BUFFER_SIZE);
    FrmSetFocus(frm, fieldIndex);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting3                                    */
/*                                               NNsi�ݒ�3�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting3(FormType  *frm)
{
    Char         bufSize[MINIBUF];

    // NNsi�I�����ɉ����ؒf����
    SetControlValue(frm, CHKID_END_OFFLINE, &(NNshParam->disconnectNNsiEnd));

    // �u�V���m�F�v�I�����ɉ����ؒf����
    SetControlValue(frm, CHKID_DISCONN_ARRIVAL, &(NNshParam->disconnArrivalEnd));

    // �u�V���m�F�v�I����ɖ��ǈꗗ��\������
    SetControlValue(frm, CHKID_ARRIVAL_NOTREAD, &(NNshParam->autoOpenNotRead));

    // ��MCookie���g�p���ď�������
    SetControlValue(frm, CHKID_WRITE_USE_COOKIE, &(NNshParam->useCookieWrite));

    // ����ڑ����ʂ��ĕ`��
    SetControlValue(frm,CHKID_REDRAW_CONNECT,&(NNshParam->redrawAfterConnect));

    // �������ݎ��A�������݃t�B�[���h�Ɏ����I�Ƀt�H�[�J�X���ړ����Ȃ�
    SetControlValue(frm, CHKID_NOT_AUTOFOCUSSET,&(NNshParam->notAutoFocus));

    // �Q�Ɛ�p�փR�s�[�����Ƃ��A�����ɃX���폜�����{
    SetControlValue(frm, CHKID_COPYDEL_READONLY,&(NNshParam->copyDelReadOnly));

    // �擾�ςݑS�Ă͎Q�Ɛ�p���O�ɕ\�����Ȃ�
    SetControlValue(frm, CHKID_NOT_READONLY, &(NNshParam->notListReadOnly));

    // �������ݎ��Q�ƃX���ԍ���}������
    SetControlValue(frm, CHKID_INSERT_REPLYNUM, &(NNshParam->insertReplyNum));

    // Proxy�o�R�ł̃A�N�Z�X
    SetControlValue(frm, CHKID_USE_PROXY, &(NNshParam->useProxy));

    // Proxy URL
    if (NNshParam->proxyURL[0] != '\0')
    { 
        NNshWinSetFieldText(frm, FLDID_USE_PROXY,
                            NNshParam->proxyURL, MAX_URL);
    }

    // Proxy Port #
    if (NNshParam->proxyPort != 0)
    {
        MemSet (bufSize, sizeof(bufSize), 0x00);
        StrIToA(bufSize, NNshParam->proxyPort);
        NNshWinSetFieldText(frm, FLDID_PROXY_PORT, bufSize, StrLen(bufSize));
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting4                                    */
/*                                               NNsi�ݒ�4�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNsiSetting4(FormType  *frm)
{
    ControlType *chkObjSub1;
    UInt16       dum;
#ifdef USE_ARMLET
    UInt32       processorType;

    // PalmOS5�p�@�\���g�p����
    FtrGet(sysFileCSystem, sysFtrNumProcessorID, &processorType);
    if (sysFtrNumProcessorIsARM(processorType))
    {
        // ARMlet�̎g�p
        SetControlValue(frm, CHKID_USE_ARMLET, &(NNshParam->useARMlet));
    }
    else
#endif
    {
        // ARMlet�̎g�p�ɂ��āA�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_USE_ARMLET));
        CtlSetUsable(chkObjSub1, false);

        NNshParam->useARMlet           = 0;
    }

    // �O���t�B�b�N�`�惂�[�h���g�p����
    SetControlValue(frm, CHKID_CLIE_USE_TINYFONT,
                                               &(NNshParam->useSonyTinyFont));

    // bookmark
    SetControlValue(frm, CHKID_USE_BOOKMARK,&(NNshParam->useBookmark));

    // PUSH ON�_�C�����Ł}�P
    SetControlValue(frm, CHKID_NOT_CURSOR, &(NNshParam->notCursor));

    // �Q�Ǝ�Back�{�^����JumpList
    SetControlValue(frm, CHKID_BACKBTN_JUMP, &(NNshParam->jogBackBtnAsGo));

    // �Q�Ǝ�JumpList�Ɂu�ꗗ�ցv
    SetControlValue(frm, CHKID_ADD_RTNTOLIST, &(NNshParam->addReturnToList));

    // �ꗗ��Back�{�^���Ń��j���[�\��
    SetControlValue(frm, CHKID_BACKBTN_LIST, &NNshParam->jogBackBtnAsMenu);

    // WebBrowser���J���Ƃ��ŐV50
    if (NNshGlobal->browserCreator != 0)
    {
        if (NNshParam->browseMesNum != 0)
        {
            dum = 1;
            SetControlValue(frm, CHKID_WEBBROWSE_LAST50, &dum);
        }
    }
    else
    {
        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_WEBBROWSE_LAST50));
        CtlSetUsable(chkObjSub1, false);
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
    return (errNone);
}
