/*============================================================================*
 *  $Id: damain.c,v 1.5 2004/05/09 05:34:35 mrsa Exp $
 *
 *  FILE: 
 *     damain.c
 *
 *  Description: 
 *     DA�A�v���P�[�V����
 *
 *===========================================================================*/
/********** Include Files **********/
#include "local.h"

void DAmain(void);

/*=========================================================================*/
/*   Function : startDA  (DA�X�^�[�g�A�b�v)                                */
/*                                                                         */
/*=========================================================================*/
void startDA(void)
{
    FormType *frm;

    frm = FrmGetActiveForm();
    DAmain();
    FrmSetActiveForm(frm);

    return;
}

/*=========================================================================*/
/*   Function : DAmain  (DA���C������)                                     */
/*                                                                         */
/*=========================================================================*/
void DAmain(void)
{
    UInt8            *msgBuf;
    UInt16            size;
    NNshSavedPref    *NNshParam;
    NNshWorkingInfo  *NNshGlobal;

    //////////////////////////////////////////////////////////////////////////
    /////     �N�������i�ݒ�̕ۑ����j
    //////////////////////////////////////////////////////////////////////////
    msgBuf = MemPtrNew(DIALOG_BUFFER);
    if (msgBuf == NULL)
    {
        // �̈�m�ۂɎ��s�I(�N�����Ȃ�)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " dlgBuf");
        return;
    }
    MemSet(msgBuf, DIALOG_BUFFER, 0x00);

    // NNsi�ݒ�p�i�[�̈�̊m��
    size      = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    NNshParam = (NNshSavedPref *) MemPtrNew(size);
    if (NNshParam == NULL)
    {
        // �̈�m�ۂɎ��s�I(�N�����Ȃ�)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " param");
        MEMFREE_PTR(msgBuf);
        return;
    }        
    MemSet(NNshParam, size, 0x00);

    // NNsi�O���[�o���̈�p�i�[�̈�̊m��
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // �̈�m�ۂɎ��s�I(�N�����Ȃ�)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " global");
        MEMFREE_PTR(msgBuf);
        MEMFREE_PTR(NNshParam);
        return;
    }        
    MemSet(NNshGlobal, size, 0x00);

#ifdef USE_OSVERSION
    // OS�o�[�W�����̊m�F
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &(NNshGlobal->palmOSVersion));
#endif

    // NNsi�ݒ�𕜋�������
    RestoreSetting_NNsh(sizeof(NNshSavedPref), NNshParam);

    //////////////////////////////////////////////////////////////////////////
    /////     ��ʃI�[�v�������i�c�`�����̃��C���j
    //////////////////////////////////////////////////////////////////////////

    //   Auto Exec.(�������s)�Ƀ`�F�b�N�������Ă��Ȃ��ꍇ�ɂ́A�ݒ�E�B���h�E��
    // �\������
    if (NNshParam->confirmationDisable == 0)
    {
        if (FrmGetActiveFormID() != 0)
        {
            NNshGlobal->previousForm = FrmGetActiveForm();
        }
        else
        {
            NNshGlobal->previousForm = NULL;
        }
        NNshGlobal->currentForm  = FrmInitForm(FRMID_MAIN);
        // FrmSetActiveForm(NNshGlobal->currentForm);

        if (StrLen(NNshParam->password) == 0)
        {
            CtlSetLabel(FrmGetObjectPtr(NNshGlobal->currentForm, 
                                   FrmGetObjectIndex(NNshGlobal->currentForm, 
                                                           SELTRID_PASSWORD)), 
                        "(Empty)");
        }
        else
        {
            CtlSetLabel(FrmGetObjectPtr(NNshGlobal->currentForm, 
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           SELTRID_PASSWORD)),
                        "* * *");
        }
        NNshWinSetPopItems (NNshGlobal->currentForm,
                            POPTRID_PROTOCOL, LSTID_PROTOCOL,
                            NNshParam->protocol);
        NNshWinSetFieldText(NNshGlobal->currentForm,
                            FLDID_HOST,     NNshParam->hostName, BUFSIZE);
        NNshWinSetFieldText(NNshGlobal->currentForm,
                            FLDID_USER,     NNshParam->userName, BUFSIZE);
        NNshWinSetFieldText(NNshGlobal->currentForm,
                            FLDID_PASSWORD, NNshParam->password, BUFSIZE);

        // ����ؒf�̐ݒ�
        CtlSetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                  FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           CHKID_DISCONNECT)),
                    (UInt16) (NNshParam->disconnect));

        // ���b�Z�[�W�w�b�_�̎擾
        CtlSetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                  FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           CHKID_GETHEADER)),
                    (UInt16) (NNshParam->getMessageHeader));

        // �C�x���g�n���h���̐ݒ�
        FrmSetEventHandler(NNshGlobal->currentForm, Handler_MainForm);
        NNshGlobal->btnId = FrmDoDialog(NNshGlobal->currentForm);
        if (NNshGlobal->btnId == BTNID_DIALOG_OK)
        {
            // �ݒ�l�̔��f
            NNshWinGetFieldText(NNshGlobal->currentForm, FLDID_HOST,
                                NNshParam->hostName, BUFSIZE);
            NNshWinGetFieldText(NNshGlobal->currentForm, FLDID_USER,
                                NNshParam->userName, BUFSIZE);
            NNshWinGetFieldText(NNshGlobal->currentForm, FLDID_PASSWORD,
                                NNshParam->password, BUFSIZE);

            // ���b�Z�[�W�w�b�_�̎擾
            NNshParam->getMessageHeader = (UInt8)
                 CtlGetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                             CHKID_GETHEADER)));

            // �������s�̃`�F�b�N
            NNshParam->confirmationDisable = (UInt8)
                 CtlGetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                              CHKID_CONFIRM)));

            // ����ؒf
            NNshParam->disconnect =
                 CtlGetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           CHKID_DISCONNECT)));

            // �ʐM�v���g�R���̔��f
            NNshParam->protocol = 
                 LstGetSelection(FrmGetObjectPtr(NNshGlobal->currentForm,
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                             LSTID_PROTOCOL)));
        }
        // �t�H�[���̍폜
        if (NNshGlobal->previousForm != NULL)
        {
            FrmSetActiveForm(NNshGlobal->previousForm);
        }
        FrmDeleteForm(NNshGlobal->currentForm);
    }
    else
    {
        // �������s���ɂ́A���[���`�F�b�N�����s����
        NNshGlobal->btnId = BTNID_DIALOG_OK;
    }

    //////////////////////////////////////////////////////////////////////////
    // ���[���`�F�b�N�̎��s������
    //////////////////////////////////////////////////////////////////////////
    if (NNshGlobal->btnId == BTNID_DIALOG_OK)
    {
        // ����M�o�b�t�@�̊m��
        NNshGlobal->sendBuf = MemPtrNew(NNshParam->bufferSize);
        if (NNshGlobal->sendBuf == NULL)
        {
            // �̈�m�ێ��s�A�I��������
            goto FUNC_END;
        }
        MemSet(NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        NNshGlobal->recvBuf = MemPtrNew(NNshParam->bufferSize);
        if (NNshGlobal->recvBuf == NULL)
        {
            // �̈�m�ێ��s�A�I��������
            MEMFREE_PTR(NNshGlobal->sendBuf);
            goto FUNC_END;
        }
        MemSet(NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);

        // BUSY�E�B���h�E��\������
        Show_BusyForm(NNshParam->hostName, NNshGlobal);    

        MemSet (NNshGlobal->tempBuf, MINIBUF, 0x00);
        switch (NNshParam->protocol)
        {
          case CHECKPROTOCOL_POP3:
          case CHECKPROTOCOL_APOP:
            // POP3/APOP�̂Ƃ�
            size = checkMail_POP3(NNshParam->protocol, NNshParam, NNshGlobal,
                                                  NNshGlobal->tempBuf, msgBuf);
            break;

          default:
            // �w��l�ُ�A�������Ȃ�
            size = ~errNone;
            break;
        }

        // ����ؒf(�w�莞)
        if (NNshParam->disconnect != 0)
        {
            // BUSY�E�B���h�E�̕\���ύX
            SetMsg_BusyForm(MSG_DISCONNECT);
            NNshNet_LineHangup();
        }

        // �o�b�t�@�N���A
        MemSet(NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);

        // ���[����M�����̉��
        if (size == ~errNone)
        {
            // �G���[����
            StrCopy(NNshGlobal->sendBuf, MSG_ERROR);
        }
        else if (size == 0)
        {
            // �V�����[�����Ȃ������Ƃ�
            StrCopy(NNshGlobal->sendBuf, MSG_NOMAIL);
            StrCat (NNshGlobal->sendBuf, "\n   (");
            StrCat (NNshGlobal->sendBuf, NNshParam->hostName);
            StrCat (NNshGlobal->sendBuf, ")");
        }
        else
        {
            // ���[�����������Ƃ��A���[������\������
            StrCopy(NNshGlobal->sendBuf, NNshGlobal->tempBuf);
            StrCat (NNshGlobal->sendBuf, MSG_MAILARRIVAL);
            StrCat (NNshGlobal->sendBuf, "\n   (");
            StrCat (NNshGlobal->sendBuf, NNshParam->hostName);
            StrCat (NNshGlobal->sendBuf, ")");

            // ���b�Z�[�W�̏ڍׂ�\���������ꍇ
            if (msgBuf != NULL)
            {
                if (msgBuf[0] != '\0')
                {
                    StrCat (NNshGlobal->sendBuf, "\n---\n");
                    StrCat (NNshGlobal->sendBuf, msgBuf);
                }
            }
        }

        // ���ʃ_�C�A���O��\������
        if (FrmGetActiveFormID() != 0)
        {
            NNshGlobal->previousForm = FrmGetActiveForm();
        }
        else
        {
            NNshGlobal->previousForm = NULL;
        }
        NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
        // FrmSetActiveForm   (NNshGlobal->currentForm);
        NNshWinSetFieldText(NNshGlobal->currentForm, FLDID_INFOFIELD,
                            NNshGlobal->sendBuf, StrLen(NNshGlobal->sendBuf));

        FrmSetEventHandler(NNshGlobal->currentForm, Handler_MainForm);
        (void) FrmDoDialog(NNshGlobal->currentForm);

        // �t�H�[���̍폜
        if (NNshGlobal->previousForm != NULL)
        {
            FrmSetActiveForm(NNshGlobal->previousForm);
        }
        FrmDeleteForm(NNshGlobal->currentForm);

        // ����M�o�b�t�@�̉��
        MEMFREE_PTR(NNshGlobal->recvBuf);
        MEMFREE_PTR(NNshGlobal->sendBuf);

        // BUSY�E�B���h�E���B��
        Hide_BusyForm(NNshGlobal);
    }
FUNC_END:

    //////////////////////////////////////////////////////////////////////////
    /////     �I�������i�ݒ�̕ۑ����j
    //////////////////////////////////////////////////////////////////////////

    // �ݒ�̕ۑ�
    SaveSetting_NNsh(sizeof(NNshSavedPref), NNshParam);

    // �̈���J������
    MEMFREE_PTR(msgBuf);
    MEMFREE_PTR(NNshParam);    
    MEMFREE_PTR(NNshGlobal);

    return;
}

/*=========================================================================*/
/*   Function : Handler_EditAction                                         */
/*                                        �ҏW���j���[���I�����ꂽ���̏��� */
/*=========================================================================*/
Boolean Handler_EditAction(EventType *event)
{
    FormType  *frm;
    FieldType *fldP;
    UInt16     frmId;
    
    // �I������Ă�������t�B�[���h���ǂ����`�F�b�N����
    frm   = FrmGetActiveForm();
    frmId = FrmGetFocus(frm);
    if (frmId == noFocus)
    { 
        // �t�H�[�J�X���I������Ă��Ȃ��̂ŁA���������Ԃ�
        return (false);
    }
    if (FrmGetObjectType(frm, frmId) != frmFieldObj)
    {
        // �I�������t�B�[���h�ł͂Ȃ��̂ŁA���������Ԃ�
        return (false);
    }
    fldP = FrmGetObjectPtr(frm, frmId);

    // �ҏW���j���[�����{
    switch (event->data.menu.itemID)
    {
      case MNUID_EDIT_UNDO:
        FldUndo(fldP);
        break;
      case MNUID_EDIT_CUT:
        FldCut(fldP);
        break;
      case MNUID_EDIT_COPY:
        FldCopy(fldP);
        break;
      case MNUID_EDIT_PASTE:
        FldPaste(fldP);
        break;
      case MNUID_SELECT_ALL:
        FldSetSelection(fldP, 0, FldGetTextLength(fldP));
        break;
      default:
        // �Ȃɂ����Ȃ��A�Ƃ������R�R�ɂ͗��Ȃ�
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Hander_MainForm                                          */
/*                                       �C�x���g�n���h��(�ݒ�t�H�[���p)  */
/*=========================================================================*/
Boolean Handler_MainForm(EventType *event)
{
    FieldType *fldP;
    UInt16    itemId;
    Char     *area;

    // ���݂̃t�H�[�����擾
    switch (event->eType)
    { 
      // ���j���[�I��
      case menuEvent:
        itemId = event->data.menu.itemID;
        break;

      // �L�[����
      case keyDownEvent:
        itemId = event->data.keyDown.chr;
        break;

      // ��ʃ^�b�v
      case ctlSelectEvent:
        itemId = event->data.ctlSelect.controlID;
        break;

     case penDownEvent:
     default: 
        return (false);
        break;
    }
    switch (itemId)
    {
      case MNUID_VERSION:
        // �o�[�W�������\��
        ShowVersion_NNsh();
        break;

      case SELTRID_PASSWORD:
        // �p�X���[�h����
        area = MemPtrNew(BUFSIZE * 2);
        if (area == NULL)
        {
            return (true);
        }
        MemSet(area, (BUFSIZE * 2), 0x00);
        NNshWinGetFieldText(FrmGetActiveForm(), FLDID_PASSWORD,
                                                          area, (BUFSIZE * 2));
        if (InputDialog("Input Password", area, (BUFSIZE * 2)) == true)
        {
            NNshWinSetFieldText(FrmGetActiveForm(), FLDID_PASSWORD, area,
                                                                (BUFSIZE * 2));

        }
        if (StrLen(area) == 0)
        {
            CtlSetLabel(FrmGetObjectPtr(FrmGetActiveForm(), 
                                        FrmGetObjectIndex(FrmGetActiveForm(),
                                                          SELTRID_PASSWORD)), 
                        "(Empty)");
        }
        else
        {
            CtlSetLabel(FrmGetObjectPtr(FrmGetActiveForm(), 
                                        FrmGetObjectIndex(FrmGetActiveForm(),
                                                          SELTRID_PASSWORD)), 
                        "* * *");
        }
        MEMFREE_PTR(area);
        break;

      case vchrPageDown:
      case chrDownArrow:
      case vchrJogDown:
        // ���X�N���[��
        if (FrmGetActiveFormID() == FRMID_RESULT)
        {
	  fldP = FrmGetObjectPtr(FrmGetActiveForm(), 
                                 FrmGetObjectIndex(FrmGetActiveForm(),
                                                   FLDID_INFOFIELD));
          FldScrollField(fldP, 5, winDown);
        }
        break;

      case vchrPageUp:
      case chrUpArrow:
      case vchrJogUp:
        // ��X�N���[��
        if (FrmGetActiveFormID() == FRMID_RESULT)
        {
	  fldP = FrmGetObjectPtr(FrmGetActiveForm(), 
                                 FrmGetObjectIndex(FrmGetActiveForm(),
                                                   FLDID_INFOFIELD));
          FldScrollField(fldP, 5, winUp);
        }
        break;

      case BTNID_DIALOG_OK:
      case BTNID_DIALOG_CANCEL:
      default:
        return (false);
        break;
    }
    return (true);
}

/*=========================================================================*/
/*   Function :   NNshRestoreNNsiSetting                                   */
/*                                                     NNsi�ݒ�̓ǂݍ���  */
/*=========================================================================*/
void RestoreSetting_NNsh(UInt16 size, NNshSavedPref *prm)
{
    Err           ret;
    UInt32        offset;
    UInt16        cnt, nofRec;
    DmOpenRef     dbRef;
    Char         *ptr;

    // NNsi�ݒ�̗̈������������
    nofRec = 0;

    if (size == sizeof(NNshSavedPref))
    {
        //  �ݒ肳�ꂽ�o�[�W�����ƍ���N�����ꂽ�o�[�W��������э\���̃T�C�Y��
        // �����������ꍇ�̂݁ANNsi�ݒ�𕜋�������B

        // �ϐ��������� 
        offset = 0;
        ptr    = (Char *) prm;

        // �c�a���ANNsi�ݒ��ǂݏo��(���R�[�h�̓o�^���̌��ˍ����ŁA�A�A����)
        OpenDatabase_NNsh(DBNAME_SETTING, DBVERSION_SETTING, &dbRef);
        GetDBCount_NNsh(dbRef, &nofRec);
        cnt = nofRec;
        while ((cnt != 0)&&(offset < sizeof(NNshSavedPref)))
        {
            ret = GetRecord_NNsh(dbRef,(cnt - 1),DBSIZE_SETTING, &ptr[offset]);
            if (ret != errNone)
            {
                // �f�[�^�ǂݏo���Ɏ��s(���̏ꍇ�ANNsi�ݒ�͏���������)
                nofRec = 0;
                break;
            }
            cnt--;
            offset = offset + DBSIZE_SETTING;
        }
        CloseDatabase_NNsh(dbRef);
    }
    if (nofRec == 0)
    {
        // �ݒ������������
        prm->useKey.key1      = vchrHard1;
        prm->useKey.key2      = vchrHard2;
        prm->useKey.key3      = vchrHard3;
        prm->useKey.key4      = vchrHard4;
        prm->bufferSize       = WORKBUF_DEFAULT;
        prm->portNum          = 110;     // POP3�|�[�g�ԍ�
        prm->timeout          = SysTicksPerSecond() * NNSH_GETMAIL_TIMEOUT;
    }
    return;
}
/*-------------------------------------------------------------------------*/
/*   Function :   NNshSaveNNsiSetting                                      */
/*                                                         NNsi�ݒ�̋L��  */
/*-------------------------------------------------------------------------*/
void SaveSetting_NNsh(UInt16 size, NNshSavedPref *param)
{
    Err       ret;
    UInt32    offset;
    UInt16    nofRec;
    DmOpenRef dbRef;
    Char      *ptr;

    OpenDatabase_NNsh(DBNAME_SETTING, DBVERSION_SETTING, &dbRef);
    GetDBCount_NNsh(dbRef, &nofRec);
    while (nofRec != 0)
    {
        //  ���ł�NNsi�ݒ肪�L�^����Ă����ꍇ�A�S���R�[�h���폜����
        (void) DeleteRecordIDX_NNsh(dbRef, (nofRec - 1));
        nofRec--;
    }

    // NNsi�ݒ��DB�ɓo�^����(���R�[�h�o�^���̌��ˍ����ɒ��ӁI)
    offset = 0;
    ptr    = (Char *) param;
    while (offset < sizeof(NNshSavedPref))
    {
        ret = EntryRecord_NNsh(dbRef, DBSIZE_SETTING, &ptr[offset]);
        if (ret != errNone)
        {
            break;
        }
        offset = offset + DBSIZE_SETTING;
    }
    CloseDatabase_NNsh(dbRef);

    return;
}


/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF                                                */
/*                                                (�\�[�e�B���O�p��r�֐�) */
/*                        �� DB����other�o�C�g�ڂɁu������Łvkey�������  */
/*                                                         ������̂Ƃ���B*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_Char(void *rec1, void *rec2, Int16 other,
                              SortRecordInfoPtr       rec1SortInfo,
                              SortRecordInfoPtr       rec2SortInfo, 
                              MemHandle               appInfoH)
{
    Char *str1, *str2;

    str1 = ((Char *) rec1) + other;
    str2 = ((Char *) rec2) + other;
    return (StrCompare(str1, str2));
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF_UInt32                                         */
/*                                                (�\�[�e�B���O�p��r�֐�) */
/*                        �� DB����other�o�C�g�ڂɁuUInt32�Łvkey�������  */
/*                                                         ������̂Ƃ���B*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_UInt32(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH)
{
    Char   *loc1,  *loc2;
    UInt32 *data1, *data2;

    loc1  = ((Char *) rec1) + other;
    loc2  = ((Char *) rec2) + other;

    data1 = (UInt32 *) loc1;
    data2 = (UInt32 *) loc2;

    return (*data1 - *data2);
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF_UInt16                                         */
/*                                                (�\�[�e�B���O�p��r�֐�) */
/*                    �� key���擪���other�o�C�g�ڂ���uUInt16�Łv�����  */
/*                                                         ������̂Ƃ���B*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_UInt16(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH)
{
    Char   *loc1,  *loc2;
    UInt16 *data1, *data2;

    loc1  = ((Char *) rec1) + other;
    loc2  = ((Char *) rec2) + other;

    data1 = (UInt16 *) loc1;
    data2 = (UInt16 *) loc2;

    return (*data1 - *data2);
}

/*-------------------------------------------------------------------------*/
/*   Function : searchRecordSub                                            */
/*                                                        (���R�[�h������) */
/*-------------------------------------------------------------------------*/
static Err searchRecordSub(DmOpenRef dbRef, void *target, UInt16 keyType, 
                           UInt16   offset, UInt16 *idx,  MemHandle *hnd)
{
    UInt16     nofData, start, end, mid;
    Int16      chk;
    MemHandle  dataH;
    Char      *data;

    // �������镶������L���X�g����
    data = (Char *)target;
    data = data + offset;

    // �f�[�^�����[���������猟�����s
    nofData = DmNumRecords(dbRef);
    if (nofData == 0)
    {
        return (~errNone - 5);
    }

    // �Q�������Ŏw�肳�ꂽ�f�[�^���o�^����Ă��邩�`�F�b�N
    mid   = 0;
    start = 0;
    end   = nofData;
    while (start <= end)
    {
        mid = start + (end - start) / 2;
        dataH = DmQueryRecord(dbRef, mid);
        if (dataH == 0)
        {
            // ������Ȃ������I (�����Ŕ�����͗l)
            return (~errNone);
        }

        // �w�肳�ꂽ�L�[�ɂ���āA�`�F�b�N���@��ς���
        data = (Char *) MemHandleLock(dataH);
        switch (keyType)
        {
          case NNSH_KEYTYPE_UINT32:
            chk  = *((UInt32 *) data) - *((UInt32 *) target);
            break;

          case NNSH_KEYTYPE_UINT16:
            chk  = *((UInt16 *) data) - *((UInt16 *) target);
            break;

          case NNSH_KEYTYPE_CHAR:
          default:
            chk  = StrCompare(data, (Char *) target);
            break;
        }
        if (chk == 0)
        {
            // ���������I(�f�[�^���܂邲�ƃR�s�[)
            MemHandleUnlock(dataH);
            *idx  = mid;
            *hnd  = dataH;
            return (errNone);
        }
        MemHandleUnlock(dataH);
        if (chk < 0)
        {
            start = mid + 1;
        }
        else
        {
            end   = mid - 1;
        }
    }
    return (~errNone - 4);
}

/*==========================================================================*/
/*  OpenDatabase_NNsh : �f�[�^�x�[�X�A�N�Z�X�̏�����                        */
/*                                                                          */
/*==========================================================================*/
void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef)
{
    LocalID dbId;
    UInt16  dbVersion;

    // �f�[�^�x�[�X�����݂��邩�m�F����B
    *dbRef    = 0;
    dbVersion = 0;
    dbId      = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // �f�[�^�x�[�X�̃o�[�W�����ԍ����擾
        (void) DmDatabaseInfo(0, dbId, NULL, NULL, &dbVersion, NULL, 
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (dbVersion < chkVersion)
        {
            // �f�[�^�x�[�X�̃o�[�W�������Â��Ƃ��́A�f�[�^�x�[�X���폜����
            (void) DmDeleteDatabase(0, dbId);
            dbId = 0;
        }
    }

    // �f�[�^�x�[�X�����݂��Ȃ��Ƃ�
    if (dbId == 0)
    {
        // �f�[�^�x�[�X��V�K�쐬
        (void) DmCreateDatabase(0, dbName, 
                                SOFT_CREATOR_ID, SOFT_DBTYPE_ID, false);

        dbId      = DmFindDatabase(0, dbName);
        if (dbId != 0)
        {
            // DB�Ƀo�[�W�����ԍ���ݒ�
            dbVersion = chkVersion;
            (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, &dbVersion, NULL,
                                     NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        }
    }

    // �f�[�^�x�[�X�̃I�[�v��
    *dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);

    return;
}

/*==========================================================================*/
/*  closeDatabase_NNsh() : �f�[�^�x�[�X���N���[�Y����                       */
/*                                                                          */
/*==========================================================================*/
void CloseDatabase_NNsh(DmOpenRef dbRef)
{
    // DB���I�[�v���ł��Ă�����CLOSE����B
    if (dbRef != 0)
    {
        (void) DmCloseDatabase(dbRef);
        dbRef = 0;
    }
    return;
}

/*=========================================================================*/
/*   Function : QsortRecord_NNsh                                           */
/*                                                (���R�[�h�̃\�[�e�B���O) */
/*=========================================================================*/
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset)
{

    // �N�C�b�N�\�[�g���d�|����
    switch (keyType)
    {

      case NNSH_KEYTYPE_UINT32:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_UInt32, offset);
        break;

      case NNSH_KEYTYPE_UINT16:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_UInt16, offset);
        break;

      case NNSH_KEYTYPE_CHAR:
      default:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_Char, offset);
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function : IsortRecord_NNsh                                           */
/*                                                (���R�[�h�̃\�[�e�B���O) */
/*=========================================================================*/
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset)
{

    // �}���\�[�g���d�|����
    switch (keyType)
    {
      case NNSH_KEYTYPE_UINT32:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_UInt32, offset);
        break;

      case NNSH_KEYTYPE_UINT16:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_UInt16, offset);
        break;

      case NNSH_KEYTYPE_CHAR:
      default:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_Char, offset);
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function : EntryRecord_NNsh                                           */
/*                                               (�c�a�̐擪�Ƀf�[�^�o�^)  */
/*=========================================================================*/
Err EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData)
{
    MemHandle  newRecH;
    UInt16     index;
    void      *dbP;

    // ���R�[�h�̐V�K�ǉ�
    index   = 0;
    newRecH = DmNewRecord(dbRef, &index, size + sizeof(UInt32));
    if (newRecH == 0)
    {
        // ���R�[�h�ǉ��G���[
        return (DmGetLastErr());
    }

    // ���R�[�h�Ƀf�[�^��������
    dbP = MemHandleLock(newRecH);
    DmWrite(dbP, 0, recordData, size);
    MemHandleUnlock(newRecH);

    // ���R�[�h������i�ύX�I���j
    DmReleaseRecord(dbRef, index, true);

    return (errNone);
}

/*=========================================================================*/
/*   Function : IsExistRecord_NNsh                                         */
/*                                                (���R�[�h�̑��݃`�F�b�N) */
/*=========================================================================*/
Err IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index)
{
    MemHandle  dataH;

    *index  = 0;
    dataH   = 0;
    return (searchRecordSub(dbRef, target, keyType, offset, index, &dataH));
}

/*=========================================================================*/
/*   Function : SearchRecord_NNsh                                          */
/*                                                        (���R�[�h������) */
/*=========================================================================*/
Err SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,
                      UInt16 offset, UInt16 size, void *matchedData,
                      UInt16 *index)
{
    Err        ret;
    UInt16     start;
    MemHandle  dataH;
    Char      *sc,  *ds;
    UInt32    *src, *dest;

    *index = 0;
    dataH  = 0;

    ret    = searchRecordSub(dbRef, target, keyType, offset, index, &dataH);
    if (ret != errNone)
    {
        // �f�[�^�����炸
        return (ret);
    }
    if (dataH == 0)
    {
        // �������n���h���ُ�
        return ((~errNone) - 2);
    }

    src = (UInt32 *) MemHandleLock(dataH);
    if (src == NULL)
    {
        // �������|�C���^�ُ�
        return ((~errNone) - 3);
    }

    // �f�[�^���܂邲�ƃR�s�[���ĉ���
    MemSet(matchedData, size, 0x00);
    dest   = (UInt32 *) matchedData;
    for (start = 0; start < (size - sizeof(UInt32));
         start = start + sizeof(UInt32))
    {
        *dest++ = *src++;
    }
    for (sc = (Char *) src, ds = (Char *) dest;start < size;
         start = start + sizeof(Char))
    {
         *ds++ = *sc++;
    }

    MemHandleUnlock(dataH);
    return (errNone);
}

/*=========================================================================*/
/*   Function : DeleteRecord_NNsh                                          */
/*                                                    (�Y�����R�[�h���폜) */
/*=========================================================================*/
Err DeleteRecord_NNsh(DmOpenRef dbRef, void  *target,
                      UInt16 keyType,  UInt16 offset)
{
    Err       ret;
    UInt16    index;
    MemHandle dataH;

    index = 0;
    dataH = 0;
    ret = searchRecordSub(dbRef, target, keyType, offset, &index, &dataH);
    if (ret != errNone)
    {
        return (errNone + 1);
    }

    return (DmRemoveRecord(dbRef, index));
}

/*=========================================================================*/
/*   Function : DeleteRecordIDX_NNsh                                       */
/*                                                    (�Y�����R�[�h���폜) */
/*=========================================================================*/
Err DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index)
{
    // �f�[�^�����I�[�o���Ă��邩�m�F
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone);
    }
    return (DmRemoveRecord(dbRef, index));
}

/*=========================================================================*/
/*   Function : GetRecord_NNsh                                             */
/*                                          (���R�[�h���C���f�b�N�X�Ŋl��) */
/*=========================================================================*/
Err GetRecord_NNsh(DmOpenRef dbRef, UInt16 index,
                   UInt16 size, void *matchedData)
{
    MemHandle  dataH;
    UInt16     start;
    UInt32    *src, *dest;
    Char      *sc,  *ds;

    // �f�[�^�����I�[�o���Ă��邩�m�F
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone - 6);
    }

    dataH = DmQueryRecord(dbRef, index);
    if (dataH == 0)
    {
        // ������Ȃ������I
        return (~errNone);
    }
    src = (UInt32 *) MemHandleLock(dataH);
    if (src == NULL)
    {
        // �������|�C���^�ُ�
        return ((~errNone) - 3);
    }

    // �f�[�^���܂邲�ƃR�s�[���ĉ���
    MemSet(matchedData, size, 0x00);
    dest   = (UInt32 *) matchedData;
    for (start = 0; start < (size - sizeof(UInt32));
         start = start + sizeof(UInt32))
    {
        *dest++ = *src++;
    }
    for (sc = (Char *) src, ds = (Char *) dest;start < size;
         start = start + sizeof(Char))
    {
         *ds++ = *sc++;
    }
    MemHandleUnlock(dataH);
    return (errNone);
}

/*=========================================================================*/
/*   Function : GetRecordReadOnly_NNsh                                     */
/*                                          (���R�[�h���C���f�b�N�X�Ŋl��) */
/*=========================================================================*/
Err GetRecordReadOnly_NNsh(DmOpenRef dbRef, UInt16 index,
                           MemHandle *dataH, void **record)
{
    // �f�[�^�����I�[�o���Ă��邩�m�F
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone - 6);
    }

    *dataH = DmQueryRecord(dbRef, index);
    if (*dataH == 0)
    {
        // ������Ȃ������I
        return (~errNone);
    }

    // �̈�����b�N����
    *record = MemHandleLock(*dataH);
    if (record == NULL)
    {
        // �������|�C���^�ُ�
        return ((~errNone) - 3);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : ReleaseRecordReadOnly_NNsh                                 */
/*                                                (���R�[�h�ǂݏo�����I��) */
/*=========================================================================*/
Err ReleaseRecordReadOnly_NNsh(DmOpenRef dbRef, MemHandle dataH)
{
    if (dataH != 0)
    {
        MemHandleUnlock(dataH);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : UpdateRecord_NNsh                                          */
/*                                          (���R�[�h���C���f�b�N�X�ōX�V) */
/*=========================================================================*/
Err UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, 
                      UInt16 size, void *updateData)
{
    MemHandle recH;
    void     *dbP;

    recH = DmGetRecord(dbRef, index);
    if (recH == 0)
    {
        // ���R�[�h�Q�ƃG���[
        return (DmGetLastErr());
    }

    // ���R�[�h�Ƀf�[�^��������
    dbP = MemHandleLock(recH);
    DmWrite(dbP, 0, updateData, size);
    MemHandleUnlock(recH);

    // ���R�[�h������i�ύX�I���j
    DmReleaseRecord(dbRef, index, true);

    return (errNone);
}

/*==========================================================================*/
/*  GetDBCount_NNsh : �f�[�^�x�[�X�ɓo�^����Ă���f�[�^��������������      */
/*                                                                          */
/*==========================================================================*/
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count)
{
    *count = DmNumRecords(dbRef);
    return;
}

/*==========================================================================*/
/*  GetDBInfo_NNsh : �f�[�^�x�[�X�̏��̎擾                               */
/*                                                                          */
/*==========================================================================*/
Err GetDBInfo_NNsh(Char *dbName, NNshDBInfo *info)
{
    LocalID dbId;
    
    dbId = DmFindDatabase(0, dbName);

    // �f�[�^�x�[�X�����݂��Ȃ��Ƃ�
    if (dbId == 0)
        return(~errNone);

    return(DmDatabaseInfo(0, dbId,
                          info->nameP, info->attributesP,
                          info->versionP, info->crDateP,
                          info->modDateP, info->bckUpDateP,
                          info->modNumP, info->appInfoIDP,
                          info->sortInfoIDP, info->typeP,
                          info->creatorP));
}


/*==========================================================================*/
/*  net_open() : �l�b�g�̃I�[�v��                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_open(UInt16 *netRef)
{
    UInt16 ifErr;
    Err    ret;

    // �ϐ��̏�����
    ifErr = 0;

    // �l�b�g���C�u�����̃I�[�v��
    (void) SysLibFind("Net.lib", netRef);

    // �l�b�g���[�N�n�o�d�m
    ret   = NetLibOpen(*netRef, &ifErr);
    if ((ret == errNone)||(ret == netErrAlreadyOpen))
    {
        if (ifErr == 0)
        {
            // open����
            return (errNone);
        }
        (void) NetLibClose(*netRef, false);
        *netRef = 0;
        ret = ~errNone;
    }
    return (ret);
}

/*==========================================================================*/
/*  net_close() : �l�b�g�̃N���[�Y                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_close(UInt16 netRef)
{
    return (NetLibClose(netRef, false));
}

/*==========================================================================*/
/*  net_write() : �f�[�^�𑗐M����                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_write(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, Int32 timeout)
{
    Err    err;
    Int16  ret;

    ret = NetLibSend(netRef, socketRef, data, size, 0, NULL, 0, timeout, &err);
    if (ret == -1)
    {
        return (err);
    }
    return (errNone);
}

/*==========================================================================*/
/*  net_read() : �f�[�^����M����                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_read(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, UInt16 *readSize, Int32 timeout)
{
    Err    err;
    Int16  ret;

    *readSize = 0;
    ret = NetLibReceive(netRef, socketRef, data, size,0,NULL,0,timeout, &err);
    if (ret == -1)
    {
        return (err);
    }

    *readSize = ret;
    return (errNone);
}

/*==========================================================================*/
/*  net_disconnect() : �z�X�g����؂藣��                                   */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_disconnect(UInt16 netRef, NetSocketRef socketRef, Int32 timeout)
{
    Err    err;

    (void) NetLibSocketShutdown(netRef, socketRef, netSocketDirBoth, timeout, &err);
    (void) NetLibSocketClose(netRef, socketRef, timeout, &err);
    if (err == errNone)
    {
        socketRef = 0;
    }
    return (errNone);
}

/*==========================================================================*/
/*  net_connect() : �z�X�g�ɐڑ�                                            */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_connect(NetSocketRef *socketRef, UInt16 netRef, Char *hostName,
                    UInt16 port, Int32 timeout)
{
    Err                  err;
    Int16                ret;
    NetHostInfoPtr       hostIP;
    NetSocketAddrINType  addrType;
    NetHostInfoBufType  *infoBuff;

    // �o�b�t�@�̈���m��
    infoBuff = MemPtrNew(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        return (~errNone);
    }

    // �z�X�g������IP�A�h���X���擾
    hostIP = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);
    if (hostIP == 0)
    {
        // �z�X�gIP�̎擾���s
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

    // �\�P�b�gOPEN
    *socketRef = NetLibSocketOpen(netRef, netSocketAddrINET, 
                                  netSocketTypeStream, netSocketProtoIPTCP,
                                  timeout, &err);
    if (*socketRef == -1)
    {
        // �\�P�b�gOPEN�ُ�
        *socketRef = 0;
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

    // �\�P�b�g�R�l�N�g
    addrType.family = netSocketAddrINET;
    addrType.port   = NetHToNS(port);
    addrType.addr   = infoBuff->address[0];
    ret = NetLibSocketConnect(netRef, *socketRef,
                              (NetSocketAddrType *) &addrType,
                              sizeof(addrType), timeout, &err);
    if (ret == -1)
    {
        // �\�P�b�g�R�l�N�g�ُ�
        (void) NetLibSocketClose(netRef, *socketRef, timeout, &err);
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }
    err = errNone;

FUNC_END:
    // �o�b�t�@�̈���J��
    (void) MemPtrFree(infoBuff);

    return (err);
}

/*==========================================================================*/
/*  NNshNet_LineHangup() : ����ؒf                                         */
/*                                                                          */
/*==========================================================================*/
void NNshNet_LineHangup(void)
{
   UInt16 netRef;

   (void) SysLibFind("Net.lib", &(netRef));

    // ����ؒf�̊m�F
    if (netRef != 0)
    {
        (void) NetLibClose(netRef, true);
        (void) NetLibFinishCloseWait(netRef);
    }
    return;
}

/*=========================================================================*/
/*   Function : SendToLocalMemopad                                         */
/*                    Exchange�}�l�[�W�����g�p���ă������Ƀf�[�^��]������ */
/*                    (http://www.palmos.com/dev/support/docs/recipes/     */
/*                                    recipe_exg_mgr_send_local.html ���) */
/*=========================================================================*/
Err SendToLocalMemopad(Char *title, Char *data)
{
    ExgSocketType *exgSocket;
    UInt32        size;
    Err           err;

    // �f�[�^�̏�����
    exgSocket = MemPtrNew(sizeof(ExgSocketType));
    if (exgSocket == NULL)
    {
        // �̈�m�ێ��s
        return (~errNone);
    }
    MemSet(exgSocket, sizeof(ExgSocketType), 0x00);
    err  = errNone;
    size = StrLen(data) + 1;

    // ���X�]����A���������N�����Ȃ��悤�ɂ���
    exgSocket->noGoTo      = 1;

    // ���X�]�����\�����s��Ȃ�
    exgSocket->noStatus    = true;

    // will comm. with memopad app
    exgSocket->description = title;
    exgSocket->name        = "Message.txt";

    // set to comm. with local machine (PIM) only
    exgSocket->localMode = 1;

    err = ExgPut(exgSocket);
    if (err == errNone)
    {
        ExgSend(exgSocket, data, size, &err);
        ExgDisconnect(exgSocket, err);
    }

    // �m�ۗ̈�̉��
    MEMFREE_PTR(exgSocket);

    return (err);
}

/*=========================================================================*/
/*   Function : ShowVersion_NNsh                                           */
/*                                                    �o�[�W�������̕\�� */
/*=========================================================================*/
void ShowVersion_NNsh(void)
{
    Char *buffer;
    
    buffer = MemPtrNew(BUFSIZE * 2);
    if (buffer == NULL)
    {
        // �̈�m�ێ��s
        return;
    }
    MemSet (buffer, (BUFSIZE * 2), 0x00);
    StrCopy(buffer, SOFT_NAME);
    StrCat (buffer, "\n");
    StrCat (buffer, SOFT_VERSION);
    StrCat (buffer, SOFT_REVISION);
    StrCat (buffer, "\n(");
    StrCat (buffer, SOFT_DATE);
    StrCat (buffer, ")\n    ");
    StrCat (buffer, SOFT_INFO);
    FrmCustomAlert(ALTID_INFO, buffer, "", "");

    MEMFREE_PTR(buffer);
    return;
}

/*=========================================================================*/
/*   Function :  NNshWinSetFieldText                                       */
/*                                                �t�B�[���h�e�L�X�g�̍X�V */
/*=========================================================================*/
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Char *msg, UInt32 size)
{
    MemHandle  txtH, oldTxtH;
    Char      *txtP;
    FieldType *fldP;

    // ���b�Z�[�W��\���̈�ɃR�s�[����
    txtH = MemHandleNew(size + MARGIN);
    if (txtH == 0)
    {
        // �̈�̊m�ۂɎ��s
        return;
    }
    txtP = (Char *) MemHandleLock(txtH);
    if (txtP == NULL)
    {
        // �̈�̊m�ۂɎ��s
        MemHandleFree(txtH);
        return;
    }
    (void) MemSet(txtP,  size + MARGIN, 0x00);
    if (size != 0)
    {
        (void) StrNCopy(txtP, msg, size);
    }
    MemHandleUnlock(txtH);

    // �t�B�[���h�̃I�u�W�F�N�g�|�C���^���擾
    fldP = (FieldType *) FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    // �t�B�[���h�̃e�L�X�g�n���h�������ւ��A�Â��̂��폜����
    oldTxtH = FldGetTextHandle(fldP);
    FldSetTextHandle(fldP, NULL);
    FldSetTextHandle(fldP, txtH);
    if (oldTxtH != 0)
    {
        (void) MemHandleFree(oldTxtH);
    }

    // �t�H�[���̍ĕ`��
    FrmDrawForm(frm);
    return;
}

/*=========================================================================*/
/*   Function :  NNshWinGetFieldText                                       */
/*                                                �t�B�[���h�e�L�X�g�̎擾 */
/*=========================================================================*/
void NNshWinGetFieldText(FormType *frm, UInt16 fldID, Char *area, UInt32 len)
{
    MemHandle  txtH;
    Char      *txtP;
    FieldType *fldP;

    // �t�B�[���h�i�[�̈������������
    MemSet(area, len, 0x00);

    // �t�B�[���h�̃I�u�W�F�N�g�|�C���^���擾
    fldP = (FieldType *) FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    // �t�B�[���h�̃e�L�X�g�n���h�����擾
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        if (txtP != NULL)
        {
            StrNCopy(area, txtP, (len - 1));
            MemHandleUnlock(txtH);
        }
    }
    return;
}

/*=========================================================================*/
/*   Function : NNshWinSetPopItems                                         */
/*                                          �|�b�v�A�b�v�g���K�̃��x���ݒ� */
/*=========================================================================*/
void NNshWinSetPopItems(FormType *frm, UInt16 popId, UInt16 lstId, UInt16 item)
{
    ListType    *lstP;
    ControlType *ctlP;

    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, popId));
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstId));
                          
    LstSetTopItem  (lstP, item);
    LstSetSelection(lstP, item);
    CtlSetLabel    (ctlP, LstGetSelectionText(lstP, item));

    return;
}

/*=========================================================================*/
/*   Function : DataInputDialog                                            */
/*                                                NNsi���ʂ̓��̓_�C�A���O */
/*=========================================================================*/
Boolean InputDialog(Char *title, Char *area, UInt16 size)
{
    Boolean       ret = false;
    FormType     *prevFrm, *diagFrm;
    FieldType    *fldP;

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // �_�C�A���O�t�H�[�����A�N�e�B�u�ɂ���
    diagFrm  = FrmInitForm(FRMID_INPUTDIALOG);
    if (diagFrm == NULL)
    {
        return (false);
    }
    FrmSetActiveForm(diagFrm);

    // �^�C�g�����R�s�[����
    FrmCopyTitle(diagFrm, title);

    // �w�肳�ꂽ��������E�B���h�E�ɔ��f������
    NNshWinSetFieldText(diagFrm, FLDID_INPUTFIELD, area, size);
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_INPUTFIELD));

    fldP = FrmGetObjectPtr(diagFrm,
                           FrmGetObjectIndex(diagFrm, FLDID_INPUTFIELD));

    // �������I�����Ă���
    FldSetSelection(fldP, 0, FldGetTextLength(fldP));

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_EditAction);

    if (FrmDoDialog(diagFrm) == BTNID_DIALOG_OK)
    {
        // OK�{�^���������ꂽ�Ƃ��ɂ́A��������擾����
        NNshWinGetFieldText(diagFrm, FLDID_INPUTFIELD, area, size);
        ret = true;
    }
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    return (ret);
}

/*=========================================================================*/
/*   Function : SetMsg_BusyForm                                            */
/*                                                     BUSY�e�L�X�g�̐ݒ�  */
/*=========================================================================*/
void SetMsg_BusyForm(Char *msg)
{
    FormType *busyFrm;
    UInt16    length;

    // �\�����镶���񒷂̃`�F�b�N
    length = StrLen(msg);
    length = (length > BUSYWIN_MAXLEN) ? BUSYWIN_MAXLEN : length;

    busyFrm = FrmGetActiveForm();
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, msg, length);

    return;
}

/*=========================================================================*/
/*   Function : Show_BusyForm                                              */
/*                                                      BUSY�t�H�[���̕\�� */
/*=========================================================================*/
void Show_BusyForm(Char *msg, NNshWorkingInfo *NNshGlobal)
{
    FormType  *busyFrm;
    UInt16    length;

    switch (FrmGetActiveFormID())
    {
      case FRMID_BUSY:
        // ���ł�BUSY�E�B���h�E���\������Ă����ꍇ
        return (SetMsg_BusyForm(msg));
        break;

      case 0:
        // �E�B���h�E���J����Ă��Ȃ�
        NNshGlobal->prevBusyForm = NULL;
        break;

      default:
        // ���݂̃E�B���h�E���L������
        NNshGlobal->prevBusyForm = FrmGetActiveForm();
        break;
    }

    // �t�H�[���̌��݂̏�Ԃ��L������
    // MemSet(&(NNshGlobal->formState), sizeof(FormActiveStateType), 0x00);
    // FrmSaveActiveState(&(NNshGlobal->formState));
    // NNshGlobal->prevBusyForm = FrmGetActiveForm();

    // BUSY�_�C�A���O�t�H�[���𐶐��A�A�N�e�B�u�ɂ���
    busyFrm = FrmInitForm(FRMID_BUSY);
    FrmSetActiveForm(busyFrm);

    // �\�����镶���񒷂̃`�F�b�N
    length = StrLen(msg);
    length = (length > BUSYWIN_MAXLEN) ? BUSYWIN_MAXLEN : length;

    // �t�B�[���h�ɕ������ݒ�
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, msg, length);

    return;
}

/*=========================================================================*/
/*   Function : Hide_BusyForm                                              */
/*                                                      BUSY�t�H�[���̍폜 */
/*=========================================================================*/
void Hide_BusyForm(NNshWorkingInfo *NNshGlobal)
{
    FormType *frm;

    if (FrmGetActiveFormID() != FRMID_BUSY)
    {
        // �������Ȃ�
        return;
    }

    // BUSY�t�H�[�����폜����
    frm = FrmGetActiveForm();
    if (NNshGlobal->prevBusyForm != NULL)
    {
        // ����Ȃ�ł����̂�...
        FrmEraseForm (frm);
        FrmSetActiveForm(NNshGlobal->prevBusyForm);
    }
    FrmDeleteForm(frm);

    // FrmRestoreActiveState(&(NNshGlobal->formState));

    return;
}

/*=========================================================================*/
/*   Function : checkMailMessage_POP3                                      */
/*                                 ���[���w�b�_�̃`�F�b�N�{����(POP3/APOP) */
/*=========================================================================*/
void checkMailMessage_POP3(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal, 
                           Char *buff, UInt8 *msgBuf)
{
    UInt8  *bufPtr;
    UInt16 lp, lim, size;

    /** �w�b�_�̎擾�������A���[���{�b�N�X�Ƀ��[�������邩... **/
    if (NNshGlobal->err < GET_MESSAGE_HEADER_NUM)
    {
        // �Ȃ��ꍇ...���[���{�b�N�X�ɂ��郁�[�����w�b�_���擾
        lim = NNshGlobal->err;
    }
    else
    {
        lim = GET_MESSAGE_HEADER_NUM;
    }

    bufPtr = msgBuf;
    for (lp = 0; lp < lim; lp++)
    {
        // �w�b�_�[�擾���̕\���ɕύX����
        SetMsg_BusyForm(MSG_PROCESS_GETHEADER_SEND);

        // �w�b�_�̂ݎ�M
        StrCopy(NNshGlobal->sendBuf, "TOP ");
        StrIToA(&NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)], (NNshGlobal->err - lp));
        StrCat (NNshGlobal->sendBuf, " 0");
        StrCat (NNshGlobal->sendBuf, "\x0d\x0a");

        if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                          StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                 NNshParam->timeout) != errNone)
        {
            // �w�b�_�v���̑��M�G���[
            return;
        }

        // �������b�Z�[�W���E��
        MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
        NNshGlobal->tempPtr = NNshGlobal->recvBuf;
        NNshGlobal->ret  = errNone;
        while (NNshGlobal->ret == errNone)
        {
            // �w�b�_�[�擾���̕\���ɕύX����
            SetMsg_BusyForm(MSG_PROCESS_GETHEADER_RECV);

            NNshGlobal->size = 0;
            NNshGlobal->ret = NNshNet_read(NNshGlobal->netRef, 
                                           NNshGlobal->socketRef,
                                           NNshParam->bufferSize - (NNshGlobal->tempPtr - NNshGlobal->recvBuf) - 1, 
                                           NNshGlobal->tempPtr,
                                           &(NNshGlobal->size), 
                                           NNshParam->timeout);
            if (NNshGlobal->ret != errNone)
            {
                break;
            }
            if (NNshGlobal->size == 0)
            {
                break;
            }
            if (StrStr(NNshGlobal->recvBuf, "\x0d\x0a\x0d\x0a\x2e\x0d\x0a") != NULL)
            {
                break;
            }
            NNshGlobal->tempPtr = NNshGlobal->tempPtr + NNshGlobal->size;
        }

        // �����`�F�b�N
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "-ERR");
        if (NNshGlobal->tempPtr != NULL)
        {
            // �ǂݏo�����s
            StrCat(bufPtr, "ERROR>");
            StrCat(bufPtr, NNshGlobal->sendBuf);
            bufPtr = bufPtr + sizeof("ERROR>") - 1 + StrLen(NNshGlobal->sendBuf) - 2;
            *bufPtr = '\n';
            bufPtr++;
            continue;
        }

        // �����`�F�b�N
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "+OK");
        if (NNshGlobal->tempPtr == NULL)
        {
            // �ǂݏo�����s�A���֍s��
            continue;
        }

        // �w�b�_�[��͒��\���ɕύX����
        SetMsg_BusyForm(MSG_PROCESS_PARSING);

        // �����f�[�^�̔����o��(���M��)
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "From:");
        if (NNshGlobal->tempPtr != NULL)
        {
            NNshGlobal->tempPtr = NNshGlobal->tempPtr + sizeof("From:") - 1;
            while ((*(NNshGlobal->tempPtr) != '\x00')&&
                   ((*(NNshGlobal->tempPtr) == ':')||
		    (*(NNshGlobal->tempPtr) <= ' ')))
            {
                NNshGlobal->tempPtr++;
            }
            while ((*(NNshGlobal->tempPtr) != '\x0d')&&
                   (*(NNshGlobal->tempPtr) != '\x0a')&&
                   (*(NNshGlobal->tempPtr) != '\x00'))
            {
                if ((*(NNshGlobal->tempPtr) == '=')&&(*(NNshGlobal->tempPtr + 1) == '?'))
                {
                    // MIME�G���R�[�h�����o�A�A�A
                    size = StrCopyMIME64ToSJ(bufPtr, NNshGlobal->tempPtr);

                    // ���̗̈�Ɉړ�����
                    NNshGlobal->tempPtr = NNshGlobal->tempPtr + size;
                    bufPtr = bufPtr + StrLen(bufPtr);

                    break;
                }
                else
                {
                    // MIME�G���R�[�h����ĂȂ��Ƃ��́A�ʏ�R�s�[
                    *bufPtr = *(NNshGlobal->tempPtr);
                    bufPtr++;
                    NNshGlobal->tempPtr++;
                }
            }
        }
        *bufPtr = ' ';
        bufPtr++;

        // �����f�[�^�̔����o��(��M����)
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "Date:");
        if (NNshGlobal->tempPtr != NULL)
        {
            *bufPtr = '[';
            bufPtr++;

            NNshGlobal->tempPtr = NNshGlobal->tempPtr + sizeof("Date:") - 1;
            while ((*(NNshGlobal->tempPtr) != '\x00')&&
                   ((*(NNshGlobal->tempPtr) == ':')||
		    (*(NNshGlobal->tempPtr) <= ' ')))
            {
                NNshGlobal->tempPtr++;
            }
            while ((*(NNshGlobal->tempPtr) != '\x0d')&&
                   (*(NNshGlobal->tempPtr) != '\x0a')&&
                   (*(NNshGlobal->tempPtr) != '\x00'))
            {
                *bufPtr = *(NNshGlobal->tempPtr);
                bufPtr++;
                NNshGlobal->tempPtr++;
            }
            *bufPtr = ']';
            bufPtr++;
        }

        // �����f�[�^�̔����o��(�^�C�g��)
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "Subject:");
        if (NNshGlobal->tempPtr != NULL)
        {
            *bufPtr = '\n';
            bufPtr++;

            NNshGlobal->tempPtr = NNshGlobal->tempPtr + sizeof("Subject:") - 1;
            while ((*(NNshGlobal->tempPtr) != '\x00')&&
                   ((*(NNshGlobal->tempPtr) == ':')||
                   (*(NNshGlobal->tempPtr) <= ' ')))
            {
                NNshGlobal->tempPtr++;
            }
            while ((*(NNshGlobal->tempPtr) != '\x0d')&&
                   (*(NNshGlobal->tempPtr) != '\x0a')&&
                   (*(NNshGlobal->tempPtr) != '\x00'))
            {
                if ((*(NNshGlobal->tempPtr) == '=')&&(*(NNshGlobal->tempPtr + 1) == '?'))
                {
                    // MIME�G���R�[�h�����o�A�A�A
                    size = StrCopyMIME64ToSJ(bufPtr, NNshGlobal->tempPtr);

                    // ���̗̈�Ɉړ�����
                    NNshGlobal->tempPtr = NNshGlobal->tempPtr + size;
                    bufPtr = bufPtr + StrLen(bufPtr);

                    break;
                }
                else
                {
                    // MIME�G���R�[�h����ĂȂ��Ƃ��́A�ʏ�R�s�[
                    *bufPtr = *(NNshGlobal->tempPtr);
                    bufPtr++;
                    NNshGlobal->tempPtr++;
                }
            }
        }

        // next data..
        *bufPtr = '\n';
        bufPtr++;

        *bufPtr = '-';
        bufPtr++;
        *bufPtr = '-';
        bufPtr++;
        *bufPtr = '-';
        bufPtr++;

        *bufPtr = '\n';
        bufPtr++;
    }
    if (*(bufPtr - 5) == '\n')
    {
        *(bufPtr - 5) = '\0';
    }
    return;
}

/*=========================================================================*/
/*   Function : checkMail_POP3                                             */
/*                                         ���[���`�F�b�N�{����(POP3/APOP) */
/*=========================================================================*/
UInt16 checkMail_POP3(UInt16 type, NNshSavedPref *NNshParam,
                      NNshWorkingInfo *NNshGlobal, Char *buff, UInt8 *msgBuf)
{
    NNshGlobal->err = ~errNone;

    // �l�b�g���C�u����OPEN
    if (NNshNet_open(&(NNshGlobal->netRef)) != errNone)
    {
        return (~errNone);
    }

    // �\�P�b�g�ڑ�
    if (NNshNet_connect(&(NNshGlobal->socketRef), NNshGlobal->netRef,
                        NNshParam->hostName, NNshParam->portNum,
                        NNshParam->timeout) != errNone)
    {
        // �ڑ��Ɏ��s
        goto NET_CLOSE;
    }

    // Welcome���b�Z�[�W���E��
    if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef, 
                     NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
    {
        // �ǂݏo�����s
        NNshGlobal->err = ~errNone;
        goto NET_DISCONNECT;
    }

    // �����`�F�b�N
    if (StrStr(NNshGlobal->recvBuf, "+OK") == NULL)
    {
        // �ǂݏo�����s
        NNshGlobal->err = ~errNone;
        goto SEND_QUIT;
    }

    // POP3�̔F��
    if (type == CHECKPROTOCOL_POP3)
    {
        // USER���M
        MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        StrCopy(NNshGlobal->sendBuf, "USER ");
        StrCat (NNshGlobal->sendBuf, NNshParam->userName);
        StrCat (NNshGlobal->sendBuf, "\x0d\x0a");
        if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                          StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf, 
                          NNshParam->timeout) != errNone)
        {
            // USER���M�G���[
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // USER�̉������b�Z�[�W���E��
        MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
        if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef, 
                         NNshParam->bufferSize, NNshGlobal->recvBuf, 
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
        {
            // �ǂݏo�����s
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // QUIT�����`�F�b�N
        if (StrStr(NNshGlobal->recvBuf, "+OK") == NULL)
        {
            // �ǂݏo�����s
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // PASS���M
        MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        StrCopy(NNshGlobal->sendBuf, "PASS ");
        if (StrLen(NNshParam->password) != 0)
        { 
            StrCat (NNshGlobal->sendBuf, NNshParam->password);
        }
        StrCat (NNshGlobal->sendBuf, "\x0d\x0a");
        if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                          StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                NNshParam->timeout) != errNone)
        {
            // PASS���M�G���[
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // PASS�̉������b�Z�[�W���E��
        MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
        if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef, 
                         NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
        {
            // �ǂݏo�����s
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // PASS�����`�F�b�N
        if (StrStr(NNshGlobal->recvBuf, "+OK") == NULL)
        {
            // �ǂݏo�����s
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }
    }
    else
    {
        // APOP�F��
        NNshGlobal->tempPtr  = StrStr(NNshGlobal->recvBuf, "<");
        NNshGlobal->tempPtr2 = NNshGlobal->tempPtr;
        while (*(NNshGlobal->tempPtr2) != '>')
        {
            if (*(NNshGlobal->tempPtr2) == '\0')
            {
                // ������������Ȃ�����
                NNshGlobal->err = ~errNone;
                goto SEND_QUIT;
            }
            (NNshGlobal->tempPtr2)++;
        }
        *(NNshGlobal->tempPtr2 + 1) = '\0';

        // MD5�ɕϊ����镶������쐬
        MemSet (NNshGlobal->tempBuf, (BUFSIZE * 2), 0x00);
        StrCopy(NNshGlobal->tempBuf, NNshGlobal->tempPtr);
        StrCat (NNshGlobal->tempBuf, NNshParam->password);

        // APOP���M
        MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        StrCopy(NNshGlobal->sendBuf, "APOP ");
        StrCat (NNshGlobal->sendBuf, NNshParam->userName);
        StrCat (NNshGlobal->sendBuf, " ");
        CalcMD5(&(NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)]), 
                NNshGlobal->tempBuf, NNshGlobal);
        StrCat (NNshGlobal->sendBuf, "\x0d\x0a");

        MemSet (NNshGlobal->tempBuf, (BUFSIZE * 2), 0x00);
        StrCopy(NNshGlobal->tempBuf, NNshGlobal->sendBuf);
        MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        StrToLower(NNshGlobal->sendBuf, NNshGlobal->tempBuf);

        if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                          StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                NNshParam->timeout) != errNone)
        {
            // APOP���M�G���[
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // APOP�̉������b�Z�[�W���E��
        MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
        if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef, 
                         NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
        {
            // �ǂݏo�����s
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // APOP�����`�F�b�N
        if (StrStr(NNshGlobal->recvBuf, "+OK") == NULL)
        {
            // �ǂݏo�����s
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }
    }

    // STAT���M
    MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
    StrCopy(NNshGlobal->sendBuf, "STAT ");
    StrCat (NNshGlobal->sendBuf, "\x0d\x0a");
    if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                      StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                NNshParam->timeout) != errNone)
    {
        // STAT���M�G���[
        NNshGlobal->err = ~errNone;
        goto SEND_QUIT;
    }

    // STAT�̉������b�Z�[�W���E��
    MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
    if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef,
                     NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
    {
        // �ǂݏo�����s
        NNshGlobal->err = ~errNone;
        goto SEND_QUIT;
    }

    // STAT�����`�F�b�N
    NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "+OK");
    if (NNshGlobal->tempPtr == NULL)
    {
        // �ǂݏo�����s
        NNshGlobal->err = ~errNone;
        goto SEND_QUIT;
    }

    // �V�����[�����������o�b�t�@�Ɋi�[
    NNshGlobal->err = 0;
    while ((*NNshGlobal->tempPtr < '0')||(*(NNshGlobal->tempPtr) > '9'))
    {
        (NNshGlobal->tempPtr)++;
    }
    StrCopy(buff, NNshGlobal->tempPtr);
    NNshGlobal->tempPtr = buff;
    while ((*(NNshGlobal->tempPtr) >= '0')&&(*(NNshGlobal->tempPtr) <= '9'))
    {
        (NNshGlobal->tempPtr)++;
    }
    *(NNshGlobal->tempPtr) = '\0';
    NNshGlobal->err = StrAToI(buff);

    if (NNshParam->getMessageHeader != 0)
    {
        // ���b�Z�[�W�w�b�_�̎擾�����{����ꍇ...
        checkMailMessage_POP3(NNshParam, NNshGlobal, buff, msgBuf);
    }

SEND_QUIT:
    // QUIT���M
    MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
    StrCopy(NNshGlobal->sendBuf, "QUIT\x0d\x0a");
    if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef,
                      StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                NNshParam->timeout) != errNone)
    {
        // QUIT���M�G���[
        goto NET_DISCONNECT;
    }

    // QUIT�̉������b�Z�[�W���E��
    MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
    if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef,
                     NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
    {
        // �ǂݏo�����s
        goto NET_DISCONNECT;
    }

NET_DISCONNECT:
    // �\�P�b�g�ؒf
    NNshNet_disconnect(NNshGlobal->netRef, NNshGlobal->socketRef, 
                                                           NNshParam->timeout);

NET_CLOSE:
    // �l�b�g���C�u�����N���[�Y
    NNshNet_close(NNshGlobal->netRef);
    return (NNshGlobal->err);
}

/*=========================================================================*/
/*   Function : StrCopyMIME64ToSJ                                          */
/*                           MIME64�w�b�_��SHIFT JIS�ɕϊ����ĕ�����R�s�[ */
/*=========================================================================*/
UInt16 StrCopyMIME64ToSJ(UInt8 *dst, UInt8 *src)
{
    UInt8  *ptr;
    UInt16  size;

    // �G���R�[�h�^�C�v�̊m�F
    if ((*(src + 15) == 'q')||(*(src + 15) == 'Q'))
    {
        // quoted-printable�̏ꍇ�ɂ͕ϊ����Ȃ��B�B�B
        *dst = *src;
        dst++;
        src++;
        *dst = *src;
        dst++;
        src++;
        *dst ='\0';
        return (2);
    }

    // �w�b�_������ǂݔ�΂�
    src = src + 16;  // '=?iso-2022-jp?B?'   
    size = 0;
    while ((*(src + size) != '\x0d')&&(*(src + size) != '\x0a')&&
            (*(src + size) != '\x00')&&(*(src + size) != '?'))
    {
        size++;
    }
    ptr = MemPtrNew(size + 16);
    if (ptr == NULL)
    {
        MemMove(dst, src, size);
        return (size);
    }
    MemSet(ptr, (size + 16), 0x00);

    // BASE64 -> JIS�ɕϊ�
    ConvertFromBASE64(ptr, src, size);

    // JIS -> SHIFT JIS�ɕϊ�
    StrCopyJIStoSJ(dst, ptr);

    MEMFREE_PTR(ptr);
    return (size);
}

/*=========================================================================*/
/*   Function : MD5(������)�����߂�                                        */
/*                                                                         */
/*=========================================================================*/
void CalcMD5(Char *buffer, Char *string, NNshWorkingInfo *NNshGlobal)
{
    Char   buff[14];
    UInt16 loop;
    UInt32 data;

    // �f�[�^�̑O����
    MemSet(&(NNshGlobal->digest),  sizeof(NNshGlobal->digest), 0x00);
    MemSet(&(NNshGlobal->PADDING), sizeof(NNshGlobal->PADDING), 0x00);
    NNshGlobal->PADDING[0] = 0x80;

    // MD5���v�Z
    MD5Init  (&(NNshGlobal->context));
    MD5Update(&(NNshGlobal->context), string, StrLen(string));
    MD5Final (NNshGlobal->digest, &(NNshGlobal->context), NNshGlobal->PADDING);

    NNshGlobal->tempPtr2 = buffer;
    for (loop = 0; loop < 16; loop++)
    {
        MemSet (buff, sizeof(buff), 0x00);
        data = ((UInt8) NNshGlobal->digest[loop]);
        StrIToH(buff, data);
        StrCat(buffer, &buff[6]);
    }

    // FrmCustomAlert(ALTID_INFO, string, " >> MD5 >> ",  buffer);
    return;
}

//////////////////////////////////////////////////////////////////////////////
//   RFC1321���A�A�A���̂܂�܂�����Ă��܂���(w�B
//////////////////////////////////////////////////////////////////////////////

/* Constants for MD5Transform routine. */

#define S11  7
#define S12 12
#define S13 17
#define S14 22
#define S21  5
#define S22  9
#define S23 14
#define S24 20
#define S31  4
#define S32 11
#define S33 16
#define S34 23
#define S41  6
#define S42 10
#define S43 15
#define S44 21

/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
 FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/*---------------------------------------------------------------------------*/
/* MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm            */
/*---------------------------------------------------------------------------
   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. 
  All rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    MD5 initialization. Begins an MD5 operation, writing a new context.
 *---------------------------------------------------------------------------*/
void MD5Init (MD5_CTX *context)
{
    context->count[0] = context->count[1] = 0;

    /* Load magic initialization constants. */
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;

    return;
}

/*---------------------------------------------------------------------------*
    MD5 block update operation. Continues an MD5 message-digest
    operation, processing another message block, and updating the
    context.
 *---------------------------------------------------------------------------*/
void MD5Update(MD5_CTX *context, UInt8 *input, UInt16 inputLen)
{
  UInt16 i, index, partLen;

  /* Compute number of bytes mod 64 */
  index = (UInt16)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((UInt32)inputLen << 3)) < ((UInt32)inputLen << 3))
  {
     context->count[1]++;
  }
  context->count[1] += ((UInt32)inputLen >> 29);

  partLen = 64 - index;

  /* Transform as many times as possible. */
  if (inputLen >= partLen) 
  {
     MD5_memcpy(&(context->buffer[index]), input, partLen);
     MD5Transform (context->state, context->buffer);

     for (i = partLen; i + 63 < inputLen; i += 64)
     {
         MD5Transform (context->state, &input[i]);
     }
     index = 0;
  }
  else
  {
      i = 0;
  }

  /* Buffer remaining input */
  MD5_memcpy(&(context->buffer[index]), &input[i], inputLen - i);

  return;
}

/*---------------------------------------------------------------------------*
     MD5 finalization. Ends an MD5 message-digest operation, writing the
    the message digest and zeroizing the context.
 *---------------------------------------------------------------------------*/
void MD5Final(UInt8 digest[16], MD5_CTX *context, UInt8 *PADDING)
{
  UInt8  bits[8];
  UInt16 index, padLen;

  /* Save number of bits */
  MD5sub_Encode(bits, context->count, 8);

  /* Pad out to 56 mod 64. */
  index  = (UInt16)((context->count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  MD5Update (context, PADDING, padLen);

  /* Append length (before padding) */
  MD5Update (context, bits, 8);

  /* Store state in digest */
  MD5sub_Encode (digest, context->state, 16);

  /* Zeroize sensitive information.*/
  MemSet(context, sizeof(*context), 0x00);

  return;
}

/*---------------------------------------------------------------------------*/
/*   MD5 basic transformation. Transforms state based on block.              */
/*---------------------------------------------------------------------------*/
void MD5Transform (UInt32 state[4], UInt8 block[64])
{
  UInt32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

  MD5sub_Decode(x, block, 64);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

 /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  /* Zeroize sensitive information. */
  MemSet(x, sizeof(x), 0x00);
}

/*---------------------------------------------------------------------------*
    Encodes input (UInt32) into output (UInt8). Assumes len is
    a multiple of 4.
 *---------------------------------------------------------------------------*/
void MD5sub_Encode(UInt8 *output, UInt32 *input, UInt16 len)
{
  UInt16  i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
  {
      output[j]   = (UInt8)(input[i] & 0xff);
      output[j+1] = (UInt8)((input[i] >> 8) & 0xff);
      output[j+2] = (UInt8)((input[i] >> 16) & 0xff);
      output[j+3] = (UInt8)((input[i] >> 24) & 0xff);
  }
}

/*---------------------------------------------------------------------------*
    Decodes input (UInt8) into output (UInt32). Assumes len is
    a multiple of 4.
 *---------------------------------------------------------------------------*/
void MD5sub_Decode(UInt32 *output, UInt8 *input, UInt16 len)
{
  UInt16 i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
  {
     output[i] = ((UInt32)input[j]) | (((UInt32)input[j+1]) << 8) |
       (((UInt32)input[j+2]) << 16) | (((UInt32)input[j+3]) << 24);
  }
  return;
}

/*---------------------------------------------------------------------------*
 *   Note: Replace "for loop" with standard memcpy if possible.
 *---------------------------------------------------------------------------*/
void MD5_memcpy(UInt8 *output, UInt8 *input, UInt16 len)
{
    UInt16 i;

    for (i = 0; i < len; i++)
    {
        output[i] = input[i];
    }
    return;
}


/*=========================================================================*/
/*   Function : ConvertJIStoSJ                                             */
/*                            JIS�����R�[�h��SHIFT JIS�����R�[�h�ɕϊ����� */
/*=========================================================================*/
void ConvertJIStoSJ(UInt8 *dst, UInt8 *src)
{
    UInt8 upper, lower;

    // ���8�r�b�g/����8�r�b�g��ϐ��ɃR�s�[����
    upper = *src;
    lower = *(src + 1);

    // ���������C����ɂ��A���S���Y�����T(p.110)�̃R�[�h���g�킹�Ă�������
    if ((upper & 1) != 0)
    {
        if (lower < 0x60)
        {
            lower = lower + 0x1f;
        }
        else
        {
            lower = lower + 0x20;
        }
    }
    else
    {
        lower = lower + 0x7e;
    }   
    if (upper < 0x5f)
    {
        upper = (upper + 0xe1) >> 1;
    }
    else
    {
        upper = (((upper + 0x61) >> 1)|(0x80));
    }
    *dst = upper;
    dst++;
    *dst = lower;

    return;
}

/*=========================================================================*/
/*   Function : StrCopyJIStoSJ                                             */
/*                          JIS�����R�[�h��SHIFT JIS�ɕϊ����ĕ�����R�s�[ */
/*=========================================================================*/
void StrCopyJIStoSJ(UInt8 *dst, UInt8 *src)
{
    Boolean kanji;
    
    kanji = false; 
    while (*src != '\0')
    {
        if ((*src == '\x1b')&&(*(src + 1) == '\x24')&&(*(src + 2) == '\x42'))
        {
            // �������[�h�ɐ؂�ւ�
            kanji = true;
            src = src + 3;
            continue;
        }
        if ((*src == '\x1b')&&(*(src + 1) == '\x28')&&(*(src + 2) == '\x42'))
        {
            // ANK���[�h�ɐ؂�ւ�
            kanji = false;
            src = src + 3;
            continue;
        }

        // �f�[�^�̂P�����R�s�[
        if (kanji == true)
        {
            ConvertJIStoSJ(dst, src);
            dst = dst + 2;
            src = src + 2;
        }
        else
        {
            // �ʏ탂�[�h...
            *dst = *src;
            dst++;
            src++;
        }
    }
    *dst = '\0';
    return;
}


/*----------------------------*/
/* conv64Ascii                */
/*        BASE64�p�ϊ��e�[�u�� */
/*----------------------------*/
UInt8 conv64Ascii(UInt8 data)
{
    if ((data >= 'A')&&(data <= 'Z'))
    {
        return (data - 'A');
    }
    if ((data >= 'a')&&(data <= 'z'))
    {
        return ((data - 'a') + 0x1a);
    }
    if ((data >= '0')&&(data <= '9'))
    {
        return ((data - '0') + 0x34);
    }
    switch (data)
    {
      case '+':
        return (0x3e);
        break;

      case '/':
        return (0x3f);
        break;

      case '=':
      default:
        break;
    }
    return (0x40);
}

/*=========================================================================*/
/*   Function : ConvertFromBASE64                                          */
/*                                                          BASE64�f�R�[�h */
/*=========================================================================*/
void ConvertFromBASE64(UInt8 *dst, UInt8 *src, UInt16 size)
{
    UInt8 *ptr, temp0, temp1, temp2, temp3;
    ptr = src;
    while (ptr < (src + size))
    {
        temp0 = conv64Ascii(*ptr);
        temp1 = conv64Ascii(*(ptr + 1));
        temp2 = conv64Ascii(*(ptr + 2));
        temp3 = conv64Ascii(*(ptr + 3));

        *dst       = ((((0x3f & temp0) << 2) & 0xfc) | (((0x30 & temp1) >> 4) & 0x03));
        *(dst + 1) = ((((0x0f & temp1) << 4) & 0xf0) | (((0x3c & temp2) >> 2) & 0x0f));
        *(dst + 2) = ((((0x03 & temp2) << 6) & 0xc0) | (((0x3f & temp3)     ) & 0x3f));
        ptr = ptr + 4;
        dst = dst + 3;
    }
    *dst = '\0';

    return;
}
