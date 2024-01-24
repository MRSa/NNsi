/*============================================================================*
 *  $Id: damain.c,v 1.28 2008/03/13 14:22:50 mrsa Exp $
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

#define NUMCATH(buf,num)   StrIToH(&(buf)[StrLen((buf))], (num))

void DAmain(void);
Err  SendToLocalMemopad(Char *title, Char *data);

static void ParseMessage(Char *buf, Char *src, UInt32 size, UInt16 kanjiCode);

// UTF8 -> Shift JIS�ϊ��e�[�u�� (AMsoft����̂c�a���g�p����)
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, LocalID dbId);

static Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr);
static Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte);
static UInt16  Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L);


static void NNsh_checkRefreshConnectionRequest(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal);
#ifdef SUPPORT_BIZPOTAL
static void pickup_hostNameAndPortNum(Char *buf, NNshSavedPref *NNshParam,NNshWorkingInfo *NNshGlobal);
#endif // #ifdef SUPPORT_BIZPOTAL

#ifdef USE_REFERER
static void NNsh_MakeReferer(NNshSavedPref *NNshParam,NNshWorkingInfo *NNshGlobal);
#endif  // #ifdef USE_REFERER

#ifdef DEBUG_LOCALHOST_ADDRESS
#define  DEBUG_ADDRESS  "localhost"
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS
 
void doLogin_YahooBBmobile(NNshSavedPref *NNshParam,NNshWorkingInfo *NNshGlobal);
void Analysis_YahooBBLoginLink(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal);
 
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
    UInt16            size;
    NNshSavedPref    *NNshParam;
    NNshWorkingInfo  *NNshGlobal;

    //////////////////////////////////////////////////////////////////////////
    /////     �N�������i�ݒ�̕ۑ����j
    //////////////////////////////////////////////////////////////////////////
    // NNsi�ݒ�p�i�[�̈�̊m��
    size      = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    NNshParam = (NNshSavedPref *) MemPtrNew(size);
    if (NNshParam == NULL)
    {
        // �̈�m�ۂɎ��s�I(�N�����Ȃ�)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " param");
        return;
    }        
    MemSet(NNshParam, size, 0x00);

    // Parameter pointer��feature�ɃZ�b�g
    FtrSet(SOFT_CREATOR_ID, SOFT_DB_ID, (UInt32) NNshParam);

    // NNsi�O���[�o���̈�p�i�[�̈�̊m��
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // �̈�m�ۂɎ��s�I(�N�����Ȃ�)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " global");
        MEMFREE_PTR(NNshParam);
        return;
    }        
    MemSet(NNshGlobal, size, 0x00);

    // OS�o�[�W�����̊m�F
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &(NNshGlobal->palmOSVersion));

    // PalmOS v5.20�����������ꍇ�ɂ͋N�����Ȃ�
    if (NNshGlobal->palmOSVersion < 0x05200000)
    {
        FrmCustomAlert(ALTID_INFO, "USE a PalmOS 5.2(or later)."," ","[mlDA]");
        return;
    }

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

        if (StrLen(NNshParam->password[NNshParam->protocol]) == 0)
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
                            FLDID_USER,
                            NNshParam->userName[NNshParam->protocol],
                            BUFSIZE0);
        NNshWinSetFieldText(NNshGlobal->currentForm,
                            FLDID_PASSWORD, 
                            NNshParam->password[NNshParam->protocol],
                            BUFSIZE0);

        // ����ؒf�̐ݒ�
        CtlSetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                  FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           CHKID_DISCONNECT)),
                    (UInt16) (NNshParam->disconnect));

        // �C�x���g�n���h���̐ݒ�
        FrmSetEventHandler(NNshGlobal->currentForm, Handler_MainForm);
        NNshGlobal->btnId = FrmDoDialog(NNshGlobal->currentForm);
        if (NNshGlobal->btnId == BTNID_DIALOG_OK)
        {
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
            NNshWinGetFieldText(NNshGlobal->currentForm, FLDID_USER,
                                NNshParam->userName[NNshParam->protocol],
                                BUFSIZE0);

            NNshWinGetFieldText(NNshGlobal->currentForm, FLDID_PASSWORD,
                                NNshParam->password[NNshParam->protocol],
                                BUFSIZE0);
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
        // �������s���ɂ́A���O�C�����������s����
        NNshGlobal->btnId = BTNID_DIALOG_OK;
    }

    //////////////////////////////////////////////////////////////////////////
    // ���O�C���̎��s������
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

        NNshGlobal->workBuf = MemPtrNew(((NNshParam->bufferSize) / 2));
        if (NNshGlobal->workBuf == NULL)
        {
            // �̈�m�ێ��s�A�I��������
            MEMFREE_PTR(NNshGlobal->sendBuf);
            MEMFREE_PTR(NNshGlobal->recvBuf);
            goto FUNC_END;
        }
        MemSet(NNshGlobal->workBuf, ((NNshParam->bufferSize) / 2), 0x00);

        // BUSY�E�B���h�E��\������
        Show_BusyForm(NNshParam->hostName, NNshGlobal);    

        MemSet (NNshGlobal->tempBuf, BUFSIZE, 0x00);

        switch (NNshParam->protocol)
        {
          case ISP_YAHOO_BB_MOBILE:
            // Yahoo! BB Moblie �Ƀ��O�C�����鏈�������s����
            doLogin_YahooBBmobile(NNshParam, NNshGlobal);
            break;

          default:
            // �z�b�g�X�|�b�g(Yahoo! BB Moblie�ȊO)�Ƀ��O�C�����鏈�������s����
            doLogin_HotSpot(NNshParam, NNshGlobal);
            break;
        }

        // ����ؒf(�w�莞)
        if (NNshParam->disconnect != 0)
        {
            // BUSY�E�B���h�E�̕\���ύX
            SetMsg_BusyForm(MSG_DISCONNECT);
            NNshNet_LineHangup();
        }
        // BUSY�E�B���h�E���B��
        Hide_BusyForm(NNshGlobal);

        // �o�b�t�@�N���A
        MemSet(NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);

        // �����R�[�h�̕\��
        if (NNshGlobal->err == errNone)
        {
            // ���O�C���ɐ���
            StrCopy(NNshGlobal->sendBuf, MSG_SUCCESSLOGIN);
        }
        else
        {
            // �G���[����
            StrCopy(NNshGlobal->sendBuf, MSG_ERROR);
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

        // (���O�C�����ʂ�t������)
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "\r\n\r\n");
        if (NNshGlobal->tempPtr == NULL)
        {
            // ��M�o�b�t�@�̐擪����\������
            NNshGlobal->tempPtr = NNshGlobal->recvBuf;
        }
        else
        {
            // ���s�R�[�h("\r\n\r\n")���|�C���^��i�߂�
            NNshGlobal->tempPtr = NNshGlobal->tempPtr + 4;
        }
        // �t�B�[���h�Ɏ�M���ʂ�\������
        if ((StrLen(NNshGlobal->tempPtr) + StrLen(NNshGlobal->sendBuf)) <
            (NNshParam->bufferSize - 1))
        {
            NNshGlobal->err = StrLen(NNshGlobal->tempPtr);
        }
        else
        {
            NNshGlobal->err =
                     NNshParam->bufferSize - (StrLen(NNshGlobal->sendBuf) - 2);
        }
#ifdef USE_RAW_RECVDATA
        // ��M�f�[�^���܂�������͂����ɕ\������
        StrNCopy(&NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)],
                 NNshGlobal->tempPtr, NNshGlobal->err);
#else
        // ��M�f�[�^��HTML�f�[�^�t�@�C���Ƃ���(�ȒP�ɉ�͂���)�\������
        ParseMessage(&NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)],
                     NNshGlobal->tempPtr, NNshGlobal->err,
                     NNSH_KANJICODE_SHIFTJIS);
#endif
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
        MEMFREE_PTR(NNshGlobal->workBuf);

    }
FUNC_END:
    //////////////////////////////////////////////////////////////////////////
    /////     �I�������i�ݒ�̕ۑ����j
    //////////////////////////////////////////////////////////////////////////

    // �ݒ�̕ۑ�
    SaveSetting_NNsh(sizeof(NNshSavedPref), NNshParam);

    // �̈���J������
    MEMFREE_PTR(NNshParam);    
    MEMFREE_PTR(NNshGlobal);

    // Global�̓o�^����
    FtrUnregister(SOFT_CREATOR_ID, SOFT_DB_ID);

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

    // �p�����[�^�̎擾
    NNshSavedPref *NNshParam;
    FtrGet(SOFT_CREATOR_ID, SOFT_DB_ID, (UInt32 *)&NNshParam);

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

      // ISP���ύX
      case popSelectEvent:
        itemId = event->data.popSelect.listID;
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
        area = MemPtrNew(BUFSIZE);
        if (area == NULL)
        {
            return (true);
        }
        MemSet(area, (BUFSIZE), 0x00);
        NNshWinGetFieldText(FrmGetActiveForm(), FLDID_PASSWORD,
                                                          area, (BUFSIZE));
        if (InputDialog("Input Password", area, (BUFSIZE)) == true)
        {
            NNshWinSetFieldText(FrmGetActiveForm(), FLDID_PASSWORD, area,
                                                                (BUFSIZE));

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


      case LSTID_PROTOCOL:
        // �I������ISP���X�V
        NNshWinGetFieldText(FrmGetActiveForm(), FLDID_USER,
                            NNshParam->userName[NNshParam->protocol],
                            BUFSIZE0);

        NNshWinGetFieldText(FrmGetActiveForm(), FLDID_PASSWORD,
                            NNshParam->password[NNshParam->protocol],
                            BUFSIZE0);

        NNshParam->protocol = event->data.popSelect.selection;

        // �I������ISP�̏�����ʂɕ\������
        NNshWinSetFieldText(FrmGetActiveForm(),
                            FLDID_USER,
                            NNshParam->userName[NNshParam->protocol],
                            BUFSIZE0);

        NNshWinSetFieldText(FrmGetActiveForm(),
                            FLDID_PASSWORD, 
                            NNshParam->password[NNshParam->protocol],
                            BUFSIZE0);

        // �p�X���[�h���̕\�����X�V
        if (StrLen(NNshParam->password[NNshParam->protocol]) == 0)
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
        return (false);
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
    UInt8 *ptr;

    *readSize = 0;
    ptr       = data;
    while (*readSize < size)
    {
        ret = NetLibReceive(netRef, socketRef, ptr, (size - *readSize),
                            0, NULL, 0, timeout, &err);
        if (ret == -1)
        {
            return (err);
        }
        if (ret == 0)
        {
            break;
        }

        *readSize = *readSize + ret;
        ptr = ptr + ret;
    }
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
    UInt8                openFlag;
    Int16                ret, addrLen;
    NetHostInfoPtr       hostIP;
    NetSocketAddrINType *addrType;
    NetSocketAddrType    addrBuf;
    NetHostInfoBufType  *infoBuff;

    // �����ڑ������X�V(�ǉ�...)
    NetLibConnectionRefresh(netRef, true,  &openFlag, &ret);

    // �o�b�t�@�̈���m��
    infoBuff = MemPtrNew(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "MemPtrNew", "");
        return (~errNone);
    }

    // �z�X�g������IP�A�h���X���擾
    hostIP = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);
    if (hostIP == 0)
    {
        // �z�X�gIP�̎擾���s
        err = NNSH_ERRORCODE_FAILURECONNECT;
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "NetLibGetHostByName :",  hostName);
        goto FUNC_END;
    }

    // �\�P�b�gOPEN
    *socketRef = NetLibSocketOpen(netRef, netSocketAddrINET, 
                                  netSocketTypeStream, netSocketProtoIPTCP,
                                  timeout, &err);
    if (*socketRef == -1)
    {
        // �\�P�b�gOPEN�ُ�
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "NetLibSocketOpen", "");
        *socketRef = 0;
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

    // �\�P�b�g�R�l�N�g
    MemSet(&addrBuf, sizeof(NetSocketAddrType), 0x00);
    addrType = (NetSocketAddrINType *) &addrBuf;
    addrType->family = netSocketAddrINET;
    addrType->port   = NetHToNS(port);
    //addrType->addr   = infoBuff->address[0];
    MemMove(&(addrType->addr), *(hostIP->addrListP), hostIP->addrLen); 
    addrLen         = sizeof(NetSocketAddrType);
    err             = errNone;
    ret = NetLibSocketConnect(netRef, *socketRef,
                              (NetSocketAddrType *) &addrBuf,
                              addrLen, timeout, &err);
    if (ret == -1)
    {
        // �\�P�b�g�R�l�N�g�ُ�
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "NetLibSocketConnect", "");
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

/*==========================================================================*/
/*  NNshSSL_callbackFunc() : SSL�̃R�[���o�b�N�֐�                          */
/*                                                                          */
/*==========================================================================*/
Int32 NNshSSL_callbackFunc(SslCallback *callbackStruct, Int32 command,
                           Int32 flavor, void *info)
{
  //  �Ƃ肠�����A�������Ȃ��B�B�B
#ifdef DEBUG
    Char buffer[128];
    MemSet(buffer, sizeof(buffer), 0x00);
    NUMCATH(buffer, command);
    StrCat(buffer, " ");
    NUMCATH(buffer, flavor);    
    // �R�[���o�b�N���Ă΂ꂽ...
    FrmCustomAlert(ALTID_INFO, "NNshSSL_callbackFunc()", " ", buffer);
    return (0);
#endif
#if 0
    SslVerify *verifyData;

    switch (command)
    {
      case sslCmdVerify:
        // 'verify callback'
        verifyData = (SslVerify *) info;
        // ���̊֐��̌��ŋᖡ����
        break;

      case sslCmdInfo:
        // FrmCustomAlert(ALTID_INFO,"SSL_callback"," Info:","sslCmdInfo");
        return (errNone);
        break;
      case sslCmdNew:
        // FrmCustomAlert(ALTID_INFO,"SSL_callback"," New:","sslCmdNew");
        return (errNone);
        break;
      case sslCmdReset:
        // FrmCustomAlert(ALTID_INFO,"SSL_callback"," Reset:","sslCmdReset");
        return (errNone);
        break;

      case sslCmdFree:
        // FrmCustomAlert(ALTID_INFO,"SSL_callback"," Free:","sslCmdFree");
        return (errNone);
        break;

      default:
        // FrmCustomAlert(ALTID_INFO, "SSL_callback", " cmd:", "???");
        return (errNone);
        break;
    }

    // Verify(certificate)
    // FrmCustomAlert(ALTID_INFO, "SSL_callback", " code:", "flavor");
    switch (flavor)
    {
      case sslErrBadDecode:
        // ���܂��ؖ������f�R�[�h�ł��Ȃ�����
      case sslErrCert:
        // ��ʓI�ȏؖ��G���[
      case sslErrCertDecodeError:
        // �ؖ����̃f�R�[�h�Ɏ��s����
      case sslErrUnsupportedCertType:
        // �T�[�o���킯�̂킩���i�T�|�[�g���Ă��Ȃ��j�ؖ����𑗂��Ă���
      case sslErrUnsupportedSignatureType:
        // �T�[�o���킯�̂킩���V�O�l�`���𑗂��Ă���
      case sslErrVerifyBadSignature:
        // �ؖ����̃V�O�l�`��������
      case sslErrVerifyNoTrustedRoot:
        // �ؖ������Ȃ�
      case sslErrVerifyNotAfter:
        // �ؖ����̗L���������؂�Ă���
      case sslErrVerifyNotBefore:
        // �ؖ����������̓��t�ɂȂ��Ă���
      case sslErrVerifyConstraintViolation:
        // �ؖ�����X509�̊g���Ɉᔽ���Ă���
      case sslErrVerifyUnknownCriticalExtension:
        // X509�g�����ؖ����`�F�b�N���[�`���ŉ��߂ł��Ȃ�����
      case sslErrOk:
      default:
        // NULL
        break;
    }
#endif
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_open() : SSL���C�u�����̃I�[�v��                                */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_open(UInt16 sslMode, UInt16 *netRef,
                 UInt16 *sslRef, SslLib **template, SslContext **context)
{
    Err ret;

    // �ϐ��̏�����
    *netRef   = 0;
    *sslRef   = 0;
    *template = NULL;
    *context  = NULL;

    // Net���C�u�����̃I�[�v��
    ret = NNshNet_open(netRef);
    if (ret != errNone)
    {
#ifdef DEBUG
        // �f�o�b�O�\���B�B�B
        FrmCustomAlert(ALTID_INFO, "ERR>NNshNet_open()", " ", " ");
#endif
        *netRef = 0;
        return (ret);
    }

    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        // SSL���[�h�łȂ��A�܂�Ԃ�
#ifdef DEBUG
        // �f�o�b�O�\���B�B�B
        FrmCustomAlert(ALTID_INFO, "ERR>NOT SSL", " ", " ");
#endif
        return (ret);
    }

    // SSL���C�u�����̃I�[�v��
    if (SysLibFind(kSslDBName, sslRef) != 0)
    {
        if (SysLibLoad(kSslLibType, kSslLibCreator, sslRef) != 0)
        {
#ifdef DEBUG
        // �f�o�b�O�\���B�B�B
        FrmCustomAlert(ALTID_INFO, "ERR>SysLibLoad()", " ", " ");
#endif
            // ���[�h���s�A�G���[��������
            *sslRef = 0;
            return (~errNone);
        }
    }
    ret = SslLibOpen(*sslRef);
    if (ret != errNone)
    {
#ifdef DEBUG
        // �f�o�b�O�\���B�B�B
        FrmCustomAlert(ALTID_INFO, "ERR>SslLibOpen()", " ", " ");
#endif
        // SSL���C�u�����I�[�v�����s�A�A�A
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // generic SSL context�̍쐬
    ret = SslLibCreate (*sslRef, template);
    if (ret != errNone)
    {
#ifdef DEBUG
        // �f�o�b�O�\���B�B�B
        FrmCustomAlert(ALTID_INFO, "ERR>SslLibCreate()", " ", " ");
#endif
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // �R���p�`�t���O�̃Z�b�g
    // SslLibSet_Compat(*sslRef, *template, sslCompatAll);

    // SSL context�̍쐬
    ret = SslContextCreate(*sslRef, *template, context);
    if (ret != errNone)
    {
#ifdef DEBUG
        // �f�o�b�O�\���B�B�B
        FrmCustomAlert(ALTID_INFO, "ERR>SslContextCreate()", " ", " ");
#endif
        // generic SSL context�̔p��
        (void) SslLibDestroy(*sslRef, *template);
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        *template = NULL;
        return (ret);
    }
    return (ret);
}

/*==========================================================================*/
/*  NNshSSL_close() : SSL���C�u�����̃N���[�Y                               */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_close(UInt16 sslMode, UInt16 netRef,
                         UInt16 sslRef, SslLib *template, SslContext *context)
{
    if ((sslRef == 0)||(sslMode == NNSH_SSLMODE_NOTSSL))
    {
        NNshNet_close(netRef);
        return (errNone);
    }

    if (context != NULL)
    {
        (void) SslContextDestroy(sslRef, context);
    }

    if (template != NULL)
    {
        (void) SslLibDestroy(sslRef, template);
    }
    (void) SslLibClose(sslRef);

    (void) NNshNet_close(netRef);
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_connect() : SSL�ڑ�                                             */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_connect(UInt16 sslMode, UInt16 sslRef, SslContext *context,
                    NetSocketRef *socketRef, UInt16 netRef,
                    Char *hostName, UInt16 port, Int32 timeout)
{
    Err         ret;
    UInt16      sslFlags;
    SslCallback verifyCallback;

    // TCP/IP�Ńz�X�g�ɐڑ�����
    ret = NNshNet_connect(socketRef, netRef, hostName, port, timeout);
    if ((ret != errNone)||(sslMode == NNSH_SSLMODE_NOTSSL))
    {
#ifdef DEBUG
            // HTTP�ŒʐM���邱�Ƃ�\��
            FrmCustomAlert(ALTID_INFO, "NNshNet_connect()", " ", " ");
#endif
        return (ret);
    }

    // // Verify�R�[���o�b�N�֐���ݒ肷��
    MemSet(&verifyCallback, sizeof(SslCallback), 0x00);
    verifyCallback.callback = NNshSSL_callbackFunc;
    SslContextSet_VerifyCallback(sslRef, context, &verifyCallback);

    // context�����Z�b�g����
//    SslContextSet_Mode(sslRef, context, SslContextGet_Mode(sslRef, context));


    // �\�P�b�g��Context�ɐݒ�A�z�X�g�ɐڑ�����
    //SslContextSet_Compat(sslRef, context, sslCompatAll);
    SslContextSet_Compat(sslRef, context, sslCompatBigRecords);
    SslContextSet_Socket(sslRef, context, *socketRef);
    ret = netErrTimeout;
    sslFlags = (sslOpenModeSsl)|(sslOpenUseDefaultTimeout);
//  sslFlags = (sslOpenModeSsl)|(sslOpenNewConnection)|(sslOpenUseDefaultTimeout);
    while (ret == netErrTimeout)
    {
        ret = SslOpen(sslRef, context, sslFlags, 0);
#ifdef DEBUG
        // �f�o�b�O�\���B�B�B
        FrmCustomAlert(ALTID_INFO, "SslOpen()", " ", " ");
#endif
//        sslFlags = (sslOpenModeSsl)|(sslOpenUseDefaultTimeout);
//        sslFlags = sslOpenUseDefaultTimeout | sslOpenBufferedReuse;  // 2��ڂ̌Ăяo�����ɂ́A�t���O���N���A����A�ƁB
    }
    return (ret);
}

/*==========================================================================*/
/*  NNshSSL_disconnect() : SSL�ؒf                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_disconnect(UInt16 sslMode, UInt16 sslRef, SslContext *context,
                       NetSocketRef socketRef, UInt16 netRef, Int32 timeout)
{
    Err ret;
    if (sslMode != NNSH_SSLMODE_NOTSSL)
    {
        ret = SslClose(sslRef, context, sslCloseUseDefaultTimeout, timeout);
    }
    ret = NNshNet_disconnect(netRef, socketRef, timeout);

    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_write() : �f�[�^���M(SSL)                                       */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_write(UInt16 sslMode, UInt16  netRef, NetSocketRef socketRef,
                  UInt16 sslRef, SslContext *context, UInt16 size, void *data,
                  Int32  timeout)
{
    Err   err;
    Int16 ret;

    // SSL���[�h�łȂ���΁A�ʏ�̏������ݏ������s��
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        return (NNshNet_write(netRef, socketRef, size, data, timeout));
    }

    // �f�[�^����������
    ret = SslWrite(sslRef, context, data, size, &err);
    if (ret  == -1)
    {
        return (err);
    }
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_read() : �f�[�^��M(SSL)                                        */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_read(UInt16 sslMode, UInt16  netRef, NetSocketRef socketRef,
                 UInt16 sslRef, SslContext *context,
                 UInt16 size, void *data, UInt16 *readSize, Int32 timeout)
{
    Err    err;
    Int16  ret;

    // SSL���[�h�łȂ���΁A�ʏ�̓ǂݏo���������s��
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        return (NNshNet_read(netRef, socketRef, size, data, readSize, timeout));
    }
    
    // �f�[�^��ǂݍ���
    ret = SslRead(sslRef, context, data, size, &err);
    if (ret == -1)
    {
        return (err);
    }
    *readSize = ret;
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_flush() : �����f�[�^�o�b�t�@�̃N���A(SSL)                       */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_flush(UInt16 sslMode, UInt16  sslRef, SslContext *context)
{
    if (sslMode != NNSH_SSLMODE_NOTSSL)
    {
        // �����f�[�^�o�b�t�@�̃N���A�w��
        SslContextSet_Mode(sslRef, context, (sslModeFlush));
    }
    return (errNone);
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
    Char          exgName[] = "?_local:Msg.txt";
    UInt32        size, creatorId;
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
    exgSocket->name        = exgName;
    exgSocket->type        = ".TXT";

    creatorId = 0;
    ExgGetDefaultApplication(&creatorId,  exgRegExtensionID , "txt");
    if (creatorId == 'DTGP')
    {
        // Palm TX�́A�Ȃ��� Docs To Go �ɓn���Ă��܂��̂�...
        // (������� memo (�V�ł̃�����)�֓n���悤�ɂ���B
        exgSocket->target = 'PMem';
    }

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

/**----------------------------------------------------------------------**
 **   doLogin_HotSpot 
 **     (HotSpot�Ƀ��O�C�����������)
 **                                            type : �I��ISP
 **                                            msg  : ���O�C�����{����
 **----------------------------------------------------------------------**/
void doLogin_HotSpot(NNshSavedPref *NNshParam,NNshWorkingInfo *NNshGlobal)
{
    // �����ڑ��́Ahttp://sourceforge.jp/ �Ƃ���  (�x�^�ɋL��...)
    MemSet(NNshParam->hostName, BUFSIZE, 0x00);
    StrCopy(NNshParam->hostName, "sourceforge.jp");

#ifdef DEBUG_LOCALHOST_ADDRESS
    // ���M����f�o�b�O�z�X�g�ɕύX����...
    StrCopy(NNshParam->hostName, DEBUG_ADDRESS);
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS

    // �����|�[�g�ԍ����i�[����
    NNshParam->portNum = 80;

    // Cookie������������
    MemSet(NNshGlobal->cookie, (BUFSIZE + MARGIN), 0x00);

    // �����t�@�C�������i�[����
    MemSet (NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
    StrCopy(NNshGlobal->reqAddress, "/");

#ifdef SUPPORT_BIZPOTAL
    // Bizportal�Ƀ��O�C���������ꍇ...
    if (NNshParam->protocol == ISP_BIZPORTAL)
    {
        // ������� bizportal �̃��O�C���t�H�[���ɔ�΂�...
        StrCopy(NNshParam->hostName, "www.bizportal.jp");
        StrCopy(NNshGlobal->reqAddress, "/en/login/login-01.php");
        NNshParam->portNum = 443;
    }
#endif // #ifdef SUPPORT_BIZPOTAL

    // ���b�Z�[�W�{�f�B�Ȃ�
    MemSet (NNshGlobal->workBuf, ((NNshParam->bufferSize) / 2), 0x00);
    StrCopy(NNshGlobal->workBuf, " ");

    // HTTP, GET���\�b�h���g�p����
    NNshGlobal->sendMethod = NNSILOGIN_GET_METHOD_HTTP;

    // ���b�Z�[�W���쐬
    NNsh_MakeHTTPmsg(NNshParam, NNshGlobal);

#ifdef DEBUG_SENDLOG
    // �f�o�b�O�p�ɑ��M�f�[�^���������֏o�͂���
    SendToLocalMemopad("FIRST CONNECTION", NNshGlobal->sendBuf);
#endif

    // HTTP�ʐM�����{
    NNshHTTP_comm(NNshParam, NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        // �ʐM���s�A�G���[��������
        FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", " ", " ");
#endif
        return;
    }

#ifdef DEBUG
    // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
    StrCat(NNshGlobal->recvBuf, "\n\n>< >< ><\n(FIRST REPLY)\n");
    SendToLocalMemopad("FIRST REPLY(RX)", NNshGlobal->recvBuf);
#endif

    // ��M���b�Z�[�W�Ƀ��_�C���N�g�̎w�������邩�A�`�F�b�N���s��
    NNshGlobal->err = ~errNone - 9;
    if ((StrStr(NNshGlobal->recvBuf, "HTTP/1.0 302") != NULL)||
        (StrStr(NNshGlobal->recvBuf, "HTTP/1.1 302") != NULL))
    {
        // ���_�C���N�g�̃z�X�g���ƃT�C�g�𒊏o����
        NNsh_check302URL(NNshParam, NNshGlobal);
    }
    else
    {
#ifdef DEBUG
        // ���_�C���N�g���Ȃ��������Ƃ𑗐M����
        FrmCustomAlert(ALTID_INFO, "DO NOT REDIRECT...", " ", " ");
#endif
    }

REDIRECT_AGAIN:
    if (NNshGlobal->err == errNone)
    {
#ifdef DEBUG_LOCALHOST_ADDRESS
    // ���_�C���N�g�̃A�h���X���擾�ł���
        // ���M����f�o�b�O�z�X�g�ɕύX����...
        StrCopy(NNshParam->hostName, DEBUG_ADDRESS);
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS

        // ���b�Z�[�W���쐬
        NNsh_MakeHTTPmsg(NNshParam, NNshGlobal);

#ifdef DEBUG
        // �f�o�b�O�p�ɑ��M�f�[�^���������֏o�͂���
        SendToLocalMemopad("SEND(REDIRECT)", NNshGlobal->sendBuf);
#endif

        // ���_�C���N�g��Ɂi���M�v���g�R���ɍ��킹�āj����
        if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) ==
                                                        NNSHLOGIN_SSL_PROTOCOL)
        {
#ifdef DEBUG
            // HTTPS�ŒʐM���邱�Ƃ�\��
            FrmCustomAlert(ALTID_INFO, "HTTPS(SSL) CONNECTION...", " ", " ");
#endif

            // https�ʐM��v��
            NNshHTTPS_comm(NNshParam, NNshGlobal);
        }
        else
        {
#ifdef DEBUG
            // HTTPS�ŒʐM���邱�Ƃ�\��
            FrmCustomAlert(ALTID_INFO, "HTTP CONNECTION...", " ", " ");
#endif

            // http�ʐM��v��
            NNshHTTP_comm(NNshParam, NNshGlobal);
        }
        if (NNshGlobal->err != errNone)
        {
#ifdef DEBUG
            // �ʐM���s�A�G���[�������� (1212)
            NUMCATH(NNshGlobal->referer, NNshGlobal->err);
            FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", NNshGlobal->referer, "(redirect)");
#endif
            return;
        }
#ifdef DEBUG
        // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
        StrCat(NNshGlobal->recvBuf, "\n\n>< >< ><\n(REDIRECT)\n");
        SendToLocalMemopad("REDIRECT(RX)", NNshGlobal->recvBuf);
#endif
    }

#if 1
    // ����񃊃_�C���N�g�H
    if ((StrStr(NNshGlobal->recvBuf, "HTTP/1.0 302") != NULL)||
        (StrStr(NNshGlobal->recvBuf, "HTTP/1.1 302") != NULL))
    {
        if (NNshGlobal->isRedirect < 2)
        {
            // ���_�C���N�g�̃z�X�g���ƃT�C�g�𒊏o����
            NNsh_check302URL(NNshParam, NNshGlobal);
            (NNshGlobal->isRedirect)++;  // ���_�C���N�g�͂Q��܂�...
            goto REDIRECT_AGAIN;
        }
    }
#endif    //#if 0

    // ����LAN��y���ȂǁA�ēx�y�[�W���J���Ȃ��Ă͂Ȃ�Ȃ��ꍇ...
    NNsh_checkRefreshConnectionRequest(NNshParam, NNshGlobal);
    if (NNshGlobal->err == errNone)
    {
#ifdef DEBUG
        // �ēx���_�C���N�g���邱�Ƃ𑗐M����
        FrmCustomAlert(ALTID_INFO, 
                       "REDIRECT AGAIN...(", NNshGlobal->reqAddress, ")");
#endif
        goto REDIRECT_AGAIN;
    }

ANALYSIS_LOGIN:
    // ���O�C���t�H�[���̉�͂Ƒ��M�{�f�B�̍쐬
    Analysis_LoginForm(NNshParam, NNshGlobal);

#ifdef DEBUG_LOCALHOST_ADDRESS
    // ���M����f�o�b�O�z�X�g�ɕύX����...
    StrCopy(NNshParam->hostName, DEBUG_ADDRESS);
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS

#ifdef DEBUG
    FrmCustomAlert(ALTID_INFO, "MAKE HTTP TO LOGIN", " ", NNshGlobal->workBuf);
#endif

    // ���O�C���p�̃��b�Z�[�W���쐬
    NNsh_MakeHTTPmsg(NNshParam, NNshGlobal);

#ifdef DEBUG_SENDLOG
    // �f�o�b�O�p�ɑ��M�f�[�^���������֏o�͂���
    SendToLocalMemopad("SEND(DO LOGIN)", NNshGlobal->sendBuf);
#endif

    // ���O�C�����b�Z�[�W��(���M�v���g�R���ɍ��킹��)���M����
    if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) == 
                                                        NNSHLOGIN_SSL_PROTOCOL)
    {
        // https�ʐM��v��
        NNshHTTPS_comm(NNshParam, NNshGlobal);
    }
    else
    {
        // http�ʐM��v��
        NNshHTTP_comm(NNshParam, NNshGlobal);
    }
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        // �ʐM���s�A�G���[��������
        NUMCATH(NNshGlobal->referer, NNshGlobal->err);
        FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", NNshGlobal->referer, "(redirect)");
#endif
        return;
    }
#ifdef DEBUG
    // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
    StrCat(NNshGlobal->recvBuf, "\n\n>< >< ><\n(LOGIN DETAIL)\n");
    SendToLocalMemopad("LOGIN DETAIL(RX)", NNshGlobal->recvBuf);
#endif

    if (NNshGlobal->setPass == 0)
    {
        // �p�X���[�h��ݒ�ł����܂ł����A�ēx���O�C���ցB�B�B
        // MLC�����C��...
        NNshGlobal->setPass = 1;
#ifdef DEBUG
        // �ʐM���s�A�G���[��������
        FrmCustomAlert(ALTID_INFO, "NOT PASSWORD...", " ", "(redirect again)");
#endif
        goto ANALYSIS_LOGIN;
    }

#ifdef LOGIN_AFTER_REDIRECT
    // bizportal�ȂǁA���O�C����̃y�[�W�𓥂�ł���O�ɏo���悤�ɂȂ�ꍇ...
    if ((StrStr(NNshGlobal->recvBuf, "HTTP/1.0 302") != NULL)||
        (StrStr(NNshGlobal->recvBuf, "HTTP/1.1 302") != NULL))
    {
        // ���_�C���N�g�̃z�X�g���ƃT�C�g�𒊏o����
        NNsh_check302URL(NNshParam, NNshGlobal);

        // ���M�f�[�^���N���A����
        NNshGlobal->workBuf[0] = '\0';

        // ���b�Z�[�W���쐬
        NNsh_MakeHTTPmsg(NNshParam, NNshGlobal);

#ifdef DEBUG
        // �f�o�b�O�p�ɑ��M�f�[�^���������֏o�͂���
        SendToLocalMemopad("SEND(FINAL REDIRECT)", NNshGlobal->sendBuf);
#endif

        // ���_�C���N�g��Ɂi���M�v���g�R���ɍ��킹�āj����
        if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) ==
                                                        NNSHLOGIN_SSL_PROTOCOL)
        {
#ifdef DEBUG
            // HTTPS�ŒʐM���邱�Ƃ�\��
            FrmCustomAlert(ALTID_INFO, "HTTPS(SSL) CONNECTION...", " ", " ");
#endif
            // https�ʐM��v��
            NNshHTTPS_comm(NNshParam, NNshGlobal);
        }
        else
        {
#ifdef DEBUG
            // HTTP�ŒʐM���邱�Ƃ�\��
            FrmCustomAlert(ALTID_INFO, "HTTP CONNECTION...", " ", " ");
#endif
            // http�ʐM��v��
            NNshHTTP_comm(NNshParam, NNshGlobal);
       }
#ifdef DEBUG
       // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
       StrCat(NNshGlobal->recvBuf, "\n\n>< >< ><\n(FINAL LOGIN DETAIL)\n");
       SendToLocalMemopad("FINAL REDIRECT DETAIL(RX)", NNshGlobal->recvBuf);
#endif
    }
#endif  // #ifdef LOGIN_AFTER_REDIRECT

    // ���O�C����"���s"�������ǂ����m�F����
    if (StrStr(NNshGlobal->recvBuf, "���s") != NULL)
    {
        // ���s�Ƃ��������񂪂������ꍇ�ɂ́A���O�C���Ɏ��s�����ƔF������
        NNshGlobal->err = ~errNone;
    }
    if (StrStr(NNshGlobal->recvBuf, "res=failed") != NULL)
    {
        // "res=failed"�Ƃ��������񂪂������ꍇ�ɂ́A���O�C���Ɏ��s�����ƔF������
        NNshGlobal->err = ~errNone;
    }
    return;
}

/**----------------------------------------------------------------------**
 **    ���O�C���t�H�[���̉�͂Ƒ��M�{�f�B�̉��
 ** 
 **                NNshGlobal->recvBuf �F ��M�{�f�B
 **                NNshGlobal->workBuf �F ���M�p�{�f�B(�����ō쐬����)
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void Analysis_LoginForm(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    NNshGlobal->err = errNone;

    // NNshGlobal->sendBuf �����[�N�o�b�t�@�Ƃ��ė��p����
    MemSet(NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);

    // Cookie���E��
    pickup_cookie(NNshParam, NNshGlobal);

    // POST��y�[�W�����擾����
    pickup_formTag(NNshParam, NNshGlobal);

    // �f�[�^�̉�͈ʒu�����肷��
    NNshGlobal->tempPtr = NNshGlobal->recvBuf;

    // ���M�p�o�b�t�@���N���A����
    MemSet (NNshGlobal->workBuf, ((NNshParam->bufferSize) / 2), 0x00);

    // �t�H�[������f�[�^�𑗐M���邽�߂̃^�O�𒊏o����
    while (NNshGlobal->tempPtr < (NNshGlobal->recvBuf + NNshParam->bufferSize))
    {

        NNshGlobal->tempPtr2 = StrStr(NNshGlobal->tempPtr,"<INPUT");
        if (NNshGlobal->tempPtr2 == NULL)
        {
            NNshGlobal->tempPtr2 = StrStr(NNshGlobal->tempPtr,"<input");
        }
        if ((NNshGlobal->tempPtr2 == NULL)||
            (NNshGlobal->tempPtr2 >= (NNshGlobal->recvBuf + NNshParam->bufferSize)))
        {
            // INPUT�^�O�����o�ł��Ȃ������A�I������
            break;
        }

        // INPUT�^�O�̓��e����͂���
        pickup_nameTag (NNshParam, NNshGlobal);
        pickup_valueTag(NNshParam, NNshGlobal);

        // key �� value ���i�[����
        set_name_and_value(NNshParam, NNshGlobal);

        // ����input�t�H�[���ֈړ�
        NNshGlobal->tempPtr = NNshGlobal->tempPtr2 + sizeof("<INPUT");
    }
    return;
}

/**----------------------------------------------------------------------**
 **    ��M���b�Z�[�W�̃��_�C���N�g��(���O�C���y�[�W)����͂���
 ** 
 **                    NNshGlobal->recvBuf �F ��M�{�f�B
 **
 **
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void NNsh_check302URL(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    // Cookie���E��
    pickup_cookie(NNshParam, NNshGlobal);

    NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "http");
    if (NNshGlobal->tempPtr == NULL)
    {
        // Redirect�z�X�g���̓ǂݏo�����s (URL�w��̌��o���s)
        NNshGlobal->err = ~errNone;
        return;
    }

#ifdef USE_REFERER
    // ���_�C���N�g���ꂽ�A�h���X��Referer�ɐݒ肷��
    NNsh_MakeReferer(NNshParam, NNshGlobal);
#endif  // #ifdef USE_REFERER

    // �v���g�R���̃`�F�b�N
    NNshGlobal->sendMethod = 0;
    if (NNshGlobal->tempPtr[4] == 's')
    {
        // https (SSL)
        NNshGlobal->sendMethod = NNSILOGIN_GET_METHOD_OTHERSSL;
        if ((NNshParam->portNum == 0)||(NNshParam->portNum == 80))
        {
            // �|�[�g�ԍ����w�肳��Ă��Ȃ������ꍇ�A�|�[�g�ԍ�������������
            NNshParam->portNum = 443;
#ifdef DEBUG
            FrmCustomAlert(ALTID_INFO, "Port ", " : ", "443");
#endif
        }
    }
    NNshGlobal->tempPtr = StrStr(NNshGlobal->tempPtr, "://");
    if (NNshGlobal->tempPtr == NULL)
    {
        // �z�X�g�����o�Ɏ��s
        NNshGlobal->err = ~errNone + 100;
        return;
    }
    NNshGlobal->tempPtr = NNshGlobal->tempPtr + 3;  // 3�́AStrLen("://");

    if (NNshParam->portNum == 0)
    {
        // �|�[�g�ԍ����w�肳��Ă��Ȃ������ꍇ�A�|�[�g�ԍ�������������
        NNshParam->portNum = 80;
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Port ", " : ", "80");
#endif
    }

    // �z�X�g���̃R�s�[
    MemSet(NNshParam->hostName, BUFSIZE, 0x00);
    for (NNshGlobal->loopCount = 0; 
         NNshGlobal->loopCount < (BUFSIZE - 1); 
         (NNshGlobal->loopCount)++)
    {
        if (*(NNshGlobal->tempPtr) == '/')
        {
            // �z�X�g���̏I�[
            // (NNshGlobal->tempPtr)++;  // /����͂��߂�...
            break;
        }
        if (*(NNshGlobal->tempPtr) == ':')
        {
            // �|�[�g�ԍ����w�肳��Ă���...
            NNshGlobal->sendMethod =
                       (NNshGlobal->sendMethod)|(NNSHLOGIN_CONNECT_PORT);
            (NNshGlobal->tempPtr)++;

            // �|�[�g�ԍ��̓���
            NNshParam->portNum = 0;
            while ((*(NNshGlobal->tempPtr) >= '0')&&
                   (*(NNshGlobal->tempPtr) <= '9'))
            {
                NNshParam->portNum = NNshParam->portNum * 10 + 
                                         (*(NNshGlobal->tempPtr) - '0');
                (NNshGlobal->tempPtr)++;
            }
            //(NNshGlobal->tempPtr)++;  // /����͂��߂�...
            break;
        }
        NNshParam->hostName[NNshGlobal->loopCount] = *(NNshGlobal->tempPtr);
        (NNshGlobal->tempPtr)++;
    }

    // �ʐM��t�@�C�������擾����
    NNshGlobal->loopCount = 0; 
    if (*(NNshGlobal->tempPtr) != '/')
    {
        // ���΃A�h���X�Ȃ̂ŁA���݂̗v��path�𐶂����悤�ɂ���
        NNshGlobal->loopCount = StrLen(NNshGlobal->reqAddress);
        while (NNshGlobal->loopCount != 0)
        {
            (NNshGlobal->loopCount)--;
            if (NNshGlobal->reqAddress[NNshGlobal->loopCount] == '/')
            {
                break;
            }
        }
        NNshGlobal->reqAddress[NNshGlobal->loopCount] = '/';
        (NNshGlobal->loopCount)++;
    }
    else
    {
        MemSet(NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
    }
    while (NNshGlobal->loopCount < (BUFSIZE - 1))
    {
        if (*(NNshGlobal->tempPtr) <= ' ')
        {
            break;
        }
        NNshGlobal->reqAddress[NNshGlobal->loopCount] = *(NNshGlobal->tempPtr);
        (NNshGlobal->tempPtr)++;
        (NNshGlobal->loopCount)++;
    }

    // �A�h���X�������Ȃ������� / �ɂ���
    if (StrLen(NNshGlobal->reqAddress) == 0)
    {
        StrCopy(NNshGlobal->reqAddress, "/");
    }
#ifdef DEBUG
    // ��͌��ʂ̃��O���o�͂���
    FrmCustomAlert(ALTID_INFO, NNshParam->hostName, " : ", NNshGlobal->reqAddress);
#endif

    // ����I������
    NNshGlobal->err = errNone;
    return;
}

/**----------------------------------------------------------------------**
 **    ��M���b�Z�[�W�̃��_�C���N�g��(���O�C���y�[�W)���ēx��͂���
 ** 
 **                    NNshGlobal->recvBuf �F ��M�{�f�B
 **
 **
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **                    (NnshGlobal->err��errNone�̂Ƃ��͍ēxURL���擾����)
 **----------------------------------------------------------------------**/
static void NNsh_checkRefreshConnectionRequest(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    Char   *ptr;

    // Cookie���E��
    pickup_cookie(NNshParam, NNshGlobal);

    // ���t���b�V���w��������H
    if (StrStr(NNshGlobal->recvBuf, "http-equiv=\"Refresh\"") == NULL)
    {
        // �Ȃ��ꍇ�ɂ́A�Ȃɂ����Ȃ�
        NNshGlobal->err = ~errNone;
        return;
    }
    if (NNshGlobal->isChecking != 0)
    {
        // ��x�`�F�b�N�ς݁A�Q�x�ڂ͂Ȃɂ����Ȃ�
        NNshGlobal->err = ~errNone;
        return;
    }

    ptr = StrStr(NNshGlobal->recvBuf, "URL=");
    if (ptr == NULL)
    {
        ptr = StrStr(NNshGlobal->recvBuf, "url=");
    }
    if (ptr == NULL)
    {
        // URL���E���Ȃ������A�I������
        NNshGlobal->err = ~errNone;
        return;
    }
    ptr = ptr + sizeof("url");

    // �v���A�h���X�̐擪��T��
    while ((*ptr != '/')&&(*ptr != '\0')&&(*ptr != '"'))
    {
        ptr++;
    }
    // �X���b�V���͐摗��...
    while (*ptr == '/')
    {
        ptr++;
    }

    // URL�w��Ńt���Z�b�g�w�肩�ǂ����`�F�b�N����
    NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "://");
    if ((NNshGlobal->tempPtr == NULL)||(NNshGlobal->tempPtr > ptr))
    {
        //  �t���Z�b�g�w��ł͂Ȃ��A�z�X�g��(�ƃ|�[�g�ԍ�)�̐؂�o����
        // �s��Ȃ�...
        goto GET_CONTENT_NAME;
    }

    // �z�X�g���̂��肾�����s��(�z�X�g���̃R�s�[)
    MemSet(NNshParam->hostName, BUFSIZE, 0x00);
    for (NNshGlobal->isChecking = 0; 
         NNshGlobal->isChecking < (BUFSIZE - 1); 
         (NNshGlobal->isChecking)++)
    {
        if (*ptr == '/')
        {
            // �z�X�g���̏I�[
            break;
        }
        if (*ptr == ':')
        {
            // �|�[�g�ԍ����w�肳��Ă���...
            NNshGlobal->sendMethod =
                       (NNshGlobal->sendMethod)|(NNSHLOGIN_CONNECT_PORT);
            ptr++;

            // �|�[�g�ԍ��̓���
            NNshParam->portNum = 0;
            while ((*ptr >= '0')&&(*ptr <= '9'))
            {
                NNshParam->portNum = NNshParam->portNum * 10 + (*ptr - '0');
                ptr++;
            }
            break;
        }
        NNshParam->hostName[NNshGlobal->isChecking] = *ptr;
        ptr++;
    }

GET_CONTENT_NAME:
    // �v���A�h���X���R�s�[����
    NNshGlobal->isChecking = 0;
    while ((*ptr != '\0')&&(*ptr != '>')&&(*ptr != '"'))
    {
        NNshGlobal->reqAddress[NNshGlobal->isChecking] = *ptr;
        (NNshGlobal->isChecking)++;
        ptr++;
    }
    NNshGlobal->reqAddress[NNshGlobal->isChecking] = '\0';

    // ����ł�����x���O�C���t�H�[�����擾���Ȃ���
    NNshGlobal->err = errNone;
    return;
}

/**----------------------------------------------------------------------**
 **    HTTPS�ʐM(SSL)�����{
 **                    NNshGlobal->workBuf �F ���M�������f�[�^
 **                    NNshGlobal->sendBuf �F ���M�{�f�B(HTTP�w�b�_��)
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void NNsh_MakeHTTPmsg(NNshSavedPref *NNshParam,NNshWorkingInfo *NNshGlobal)
{
#ifdef USE_RESOURCE
    MemHandle  strH;
#endif
    Char *src, *dst;

    MemSet(NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
    switch (NNshGlobal->sendMethod)
    {
      case NNSILOGIN_POST_METHOD_SSL:
        StrCopy(NNshGlobal->sendBuf, "POST ");
        NNshParam->portNum = 443;
        break;

      case NNSILOGIN_POST_METHOD_HTTP:
        StrCopy(NNshGlobal->sendBuf, "POST ");
        NNshParam->portNum = 80;
        break;

      case NNSILOGIN_GET_METHOD_SSL:
        StrCopy(NNshGlobal->sendBuf, "GET ");
        NNshParam->portNum = 443;
        break;

      case NNSILOGIN_GET_METHOD_HTTP:
        StrCopy(NNshGlobal->sendBuf, "GET ");
        NNshParam->portNum = 80;
        break;

      case NNSILOGIN_POST_METHOD_OTHER:
      case NNSILOGIN_POST_METHOD_OTHERSSL:
        StrCopy(NNshGlobal->sendBuf, "POST ");
        break;

      case NNSILOGIN_GET_METHOD_OTHER:
      case NNSILOGIN_GET_METHOD_OTHERSSL:
      default:
        StrCopy(NNshGlobal->sendBuf, "GET ");
        break;
    }

    if (NNshGlobal->reqAddress[0] != '/')
    {
        StrCat(NNshGlobal->sendBuf, "/");
    }

    dst = NNshGlobal->sendBuf + StrLen(NNshGlobal->sendBuf);

    // ���M�|�[�g�ԍ�...
    src = NNshGlobal->reqAddress;
    while (*src != '\0')
    {
        // �f�[�^���P�����R�s�[����
        *dst = *src;
        dst++;
        src++;
    }
    //StrCat(NNshGlobal->sendBuf, NNshGlobal->reqAddress);
    StrCat(NNshGlobal->sendBuf, " HTTP/1.0\r\nHost: ");
    StrCat(NNshGlobal->sendBuf, NNshParam->hostName);

    // referer���ݒ肳��Ă����ꍇ�ɂ͕t������
#ifdef USE_REFERER
    if (StrLen(NNshGlobal->referer) != 0)
    {
        StrCat(NNshGlobal->sendBuf, "\r\nReferer: ");
        StrCat(NNshGlobal->sendBuf, NNshGlobal->referer);
    }
#endif

    // �{�f�B�����Ȃ��ꍇ�ɂ́AContent-Length, Content-Type�����Ȃ�
    if (StrLen(NNshGlobal->workBuf) > 1)
    {
        StrCat(NNshGlobal->sendBuf, "\r\nContent-Length: ");
        StrIToA(&NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)], 
                                      StrLen(NNshGlobal->workBuf));
        StrCat(NNshGlobal->sendBuf, "\r\nContent-Type: application/x-www-form-urlencoded");
    }

    // Cookie��ǉ�
    if (NNshGlobal->cookie[0] != '\0')
    {
        StrCat(NNshGlobal->sendBuf, "\r\n");
        StrCat(NNshGlobal->sendBuf, NNshGlobal->cookie);
    }

    // �w�b�_�̎c�蕔����t��
    StrCat(NNshGlobal->sendBuf, "\r\nAccept: text/html, */*\r\nUser-Agent: mlDA/1.02 (PDA; PalmOS/NNsi family)\r\nAccept-Encoding: identity\r\nAccept-Language: ja, en\r\nConnection: close\r\nPragma: no-cache\r\n\r\n");

    // StrCat(NNshGlobal->sendBuf, "\r\n\r\n");   // �� ���\�[�X�ŕt������
    StrCat(NNshGlobal->sendBuf, NNshGlobal->workBuf);

    return;
}

/**----------------------------------------------------------------------**
 **    ���M�����A�h���X���烊�t�@�����쐬����
 ** 
 **----------------------------------------------------------------------**/
#ifdef USE_REFERER
static void NNsh_MakeReferer(NNshSavedPref *NNshParam,NNshWorkingInfo *NNshGlobal)
{
    MemSet(NNshGlobal->referer, BUFSIZE, 0x00);
    
    switch (NNshGlobal->sendMethod)
    {
      case NNSILOGIN_POST_METHOD_SSL:
      case NNSILOGIN_GET_METHOD_SSL:
      case NNSILOGIN_POST_METHOD_OTHERSSL:
      case NNSILOGIN_GET_METHOD_OTHERSSL:
        // SSL�ڑ��̏ꍇ...
        StrCopy(NNshGlobal->referer, "https://");
        StrCat (NNshGlobal->referer, NNshParam->hostName);
        if (NNshParam->portNum != 443)
        {
            // �|�[�g�ԍ����f�t�H���g�Ƃ͈�����ꍇ�ɂ̓z�X�g���̖����ɂ���
            StrCopy(&(NNshGlobal->referer[StrLen(NNshGlobal->referer)]), ":");
            StrIToA(&(NNshGlobal->referer[StrLen(NNshGlobal->referer)]), NNshParam->portNum);
        }
        break;

      case NNSILOGIN_POST_METHOD_HTTP:
      case NNSILOGIN_GET_METHOD_HTTP:
      case NNSILOGIN_POST_METHOD_OTHER:
      case NNSILOGIN_GET_METHOD_OTHER:
      default:
        StrCopy(NNshGlobal->referer, "http://");
        StrCat (NNshGlobal->referer, NNshParam->hostName);
        if (NNshParam->portNum != 80)
        {
            // �|�[�g�ԍ����f�t�H���g�Ƃ͈�����ꍇ�ɂ̓z�X�g���̖����ɂ���
            StrCat(NNshGlobal->referer, ":");
            StrIToA(&(NNshGlobal->referer[StrLen(NNshGlobal->referer)]), NNshParam->portNum);
        }
        break;
    }

    // ���M��A�h���X��ݒ肷��
    if (NNshGlobal->reqAddress[0] != '/')
    {
        StrCat(NNshGlobal->referer, "/");
    }
    StrCat(NNshGlobal->referer, NNshGlobal->reqAddress);

    return;   
}
#endif // #ifdef USE_REFERER

/**----------------------------------------------------------------------**
 **    HTTPS�ʐM(SSL)�����{
 **                                NNshGlobal->sendBuf �F ���M�f�[�^
 **                                NNshGlobal->recvBuf �F ��M�f�[�^
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void NNshHTTPS_comm(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    // ��M�o�b�t�@�̑|��
    MemSet(NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);

#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_START", "(open)", NNshParam->hostName);
#endif

    // SSL�̃I�[�v��
    NNshGlobal->err = 
          NNshSSL_open(NNSH_SSLMODE_DOSSL,    &(NNshGlobal->netRef), 
                       &(NNshGlobal->sslRef), &(NNshGlobal->sslTemplate),
                       &(NNshGlobal->sslContext));
    if (NNshGlobal->err != errNone)
    {
        // �ڑ��Ɏ��s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_open", " ", NNshParam->hostName);
#endif
        return;
    }

#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_CONNECT", "(connect)", NNshParam->hostName);
#endif

    // SSL�̃R�l�N�g
    NNshGlobal->err = 
          NNshSSL_connect(NNSH_SSLMODE_DOSSL,     NNshGlobal->sslRef, 
                          NNshGlobal->sslContext, &(NNshGlobal->socketRef),
                          NNshGlobal->netRef,     NNshParam->hostName,
                          NNshParam->portNum,     NNshParam->timeout);
    if (NNshGlobal->err != errNone)
    {
        // �ڑ��Ɏ��s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_connect", " ", NNshParam->hostName);
#endif
        goto HTTPS_CLOSE;
    }

#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_WRITE", "(write)", NNshParam->hostName);
#endif

    // �v���̑��M
    NNshGlobal->err = 
        NNshSSL_write(NNSH_SSLMODE_DOSSL,       NNshGlobal->netRef, 
                      NNshGlobal->socketRef,    NNshGlobal->sslRef,
                      NNshGlobal->sslContext,   StrLen(NNshGlobal->sendBuf),
			NNshGlobal->sendBuf,      NNshParam->timeout);
    if (NNshGlobal->err != errNone)
    {
        // �ڑ��Ɏ��s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_write", " ", NNshParam->hostName);
#endif
        goto HTTPS_DISCONNECT;
    }

#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_READ", "(read)", NNshParam->hostName);
#endif

    // �ǂݍ���
    NNshGlobal->err = 
          NNshSSL_read(NNSH_SSLMODE_DOSSL,
                       NNshGlobal->netRef, NNshGlobal->socketRef, 
                       NNshGlobal->sslRef, NNshGlobal->sslContext, 
                       NNshParam->bufferSize, NNshGlobal->recvBuf,
		       &(NNshGlobal->ret), NNshParam->timeout);
    if (NNshGlobal->err != errNone)
    {
        // �ڑ��Ɏ��s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_read", " ", NNshParam->hostName);
#endif
        goto HTTPS_DISCONNECT;
    }

HTTPS_DISCONNECT:

#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_DISCONNECT", "(disconnect)", NNshParam->hostName);
#endif
    // �ڑ���؂�
    NNshSSL_disconnect(NNSH_SSLMODE_DOSSL, NNshGlobal->sslRef,
                       NNshGlobal->sslContext,  NNshGlobal->socketRef,
                       NNshGlobal->netRef,      NNshParam->timeout);
HTTPS_CLOSE:

#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_CLOSE", "(close)", NNshParam->hostName);
#endif
    // �N���[�Y
    NNshSSL_close(NNSH_SSLMODE_DOSSL, NNshGlobal->netRef,
                  NNshGlobal->sslRef, NNshGlobal->sslTemplate,
                  NNshGlobal->sslContext);

    return;
}

/**----------------------------------------------------------------------**
 **    HTTP�ʐM�����{
 **                                NNshGlobal->sendBuf �F ���M�f�[�^
 **                                NNshGlobal->recvBuf �F ��M�f�[�^
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void NNshHTTP_comm(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    // ��M�o�b�t�@�̑|��
    MemSet(NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);

    // �l�b�g���C�u����OPEN
    NNshGlobal->err = NNshNet_open(&(NNshGlobal->netRef));
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Net_open", " ", NNshParam->hostName);
#endif
        return;
    }
    // �R�l�N�g
    NNshGlobal->err = NNshNet_connect(&(NNshGlobal->socketRef), 
                                      NNshGlobal->netRef,
                                      NNshParam->hostName, 
                                      NNshParam->portNum,
                                      NNshParam->timeout);
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Net_connect", " ", NNshParam->hostName);
#endif
        goto HTTP_CLOSE;
    }

    // ��������
    NNshGlobal->err = NNshNet_write(NNshGlobal->netRef, 
                                    NNshGlobal->socketRef,
                                    StrLen(NNshGlobal->sendBuf), 
                                    NNshGlobal->sendBuf, 
                                    NNshParam->timeout);
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "NNshNet_write",  "", "");
#endif
        goto HTTP_CLOSE;
    }

    // �ǂݏo��
    NNshGlobal->err = NNshNet_read(NNshGlobal->netRef,
                                   NNshGlobal->socketRef, 
                                   NNshParam->bufferSize, 
                                   NNshGlobal->recvBuf,
                                   &(NNshGlobal->ret), 
                                   NNshParam->timeout);
    if (NNshGlobal->err != errNone)
    {
        // �ǂݏo�����s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "NNshNet_read",  "", "");
#endif
        goto HTTP_DISCONNECT;
    }
HTTP_DISCONNECT:
    // �\�P�b�g�ؒf
    NNshNet_disconnect(NNshGlobal->netRef, NNshGlobal->socketRef, 
                                                        NNshParam->timeout);
HTTP_CLOSE:
    NNshNet_close(NNshGlobal->netRef);

    return;
}

/**----------------------------------------------------------------------**
 **    NAME�^�O�����o�ł��邩�`�F�b�N����
 **
 **                  NNshGlobal->tempBuf�Ɍ��o���ʂ��i�[����
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void pickup_nameTag(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    Char  *ptr;
    UInt16 loop;

    // �f�[�^������������
    MemSet(NNshGlobal->tempBuf,  BUFSIZE, 0x00);
    loop = 0;
    ptr  = NNshGlobal->tempPtr2;

    // INPUT�^�O�̓��e����͂���
    while ((ptr < (NNshGlobal->recvBuf + NNshParam->bufferSize))&&(*ptr != '>'))
    { 
        // �L�[���[�hname��������
        if (((*(ptr + 0) == 'n')||(*(ptr + 0) == 'N'))&&
            ((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&
            ((*(ptr + 2) == 'm')||(*(ptr + 2) == 'M'))&&
            ((*(ptr + 3) == 'e')||(*(ptr + 3) == 'E')))
        {
            // name�f�[�^�̐擪�܂Ń|�C���^�𑗂�
            ptr = ptr + 5;
            if ((*ptr == '"')||(*ptr == '\'')||(*ptr == ' ')||(*ptr == '='))
            {
                ptr++;
            }
            // �f�[�^��tempBuf�ɃR�s�[����
            while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != ' ')&&
                   (*ptr != '>')&&(loop < (BUFSIZE - 1)))
            {
                NNshGlobal->tempBuf[loop] = *ptr;
                loop++;
                ptr++;
            }
            return;
        }
        ptr++;
    }
    return;
}

/**----------------------------------------------------------------------**
 **    VALUE�^�O�����o�ł��邩�`�F�b�N����
 **
 **                  NNshGlobal->tempBuf2�Ɍ��o���ʂ��i�[����
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void pickup_valueTag(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    Char  *ptr;
    UInt16 loop;

    // �f�[�^������������
    MemSet(NNshGlobal->tempBuf2,  BUFSIZE, 0x00);
    loop = 0;
    ptr  = NNshGlobal->tempPtr2;

    // INPUT�^�O�̓��e����͂���
    while ((ptr < (NNshGlobal->recvBuf + NNshParam->bufferSize))&&(*ptr != '>'))
    { 
        // �L�[���[�hvalue��T��
        if (((*(ptr + 0) == 'v')||(*(ptr + 0) == 'V'))&&
            ((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&
            ((*(ptr + 2) == 'l')||(*(ptr + 2) == 'L'))&&
            ((*(ptr + 3) == 'u')||(*(ptr + 3) == 'U'))&&
            ((*(ptr + 4) == 'e')||(*(ptr + 4) == 'E')))
        {
            // value �f�[�^�̐擪�܂Ń|�C���^�𑗂�
            ptr = ptr + 6;
            if ((*ptr == '"')||(*ptr == '\'')||(*ptr == ' ')||(*ptr == '='))
            {
                ptr++;
            }
            // �f�[�^��tempBuf2�ɃR�s�[����
            while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != ' ')&&
                   (*ptr != '>')&&(loop < (BUFSIZE - 1)))
            {
                NNshGlobal->tempBuf2[loop] = *ptr;
                loop++;
                ptr++;
            }
            return;
        }
        ptr++;
    }
    return;
}

/**----------------------------------------------------------------------**
 **    name �� value��workbuf�Ɋi�[����
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void set_name_and_value(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    Char *value;

    // �f�[�^�l�̏����l�� tempBuf2
    value = NNshGlobal->tempBuf2;

    // ���[�U�����H
    if (StrCaselessCompare(NNshGlobal->tempBuf, "username") == 0)
    {
        value = NNshParam->userName[NNshParam->protocol];
        goto ENTRY_DATA;
    }

    // ���[�U�����H (M-zone�����̑Ή�)
    if (StrCaselessCompare(NNshGlobal->tempBuf, "user") == 0)
    {
        value = NNshParam->userName[NNshParam->protocol];
        goto ENTRY_DATA;
    }

    // ���[�U�����H (HotSpot�����̑Ή�)
    if (StrCaselessCompare(NNshGlobal->tempBuf, "login_name") == 0)
    {
        value = NNshParam->userName[NNshParam->protocol];
        goto ENTRY_DATA;
    }

    // ���[�U�����H (FON�����̑Ή�)
    if (StrCaselessCompare(NNshGlobal->tempBuf, "login_email") == 0)
    {
        value = NNshParam->userName[NNshParam->protocol];
        goto ENTRY_DATA;
    }

    // ���[�U�����H (YahooBB mobile�����̑Ή�)
    if (StrCaselessCompare(NNshGlobal->tempBuf, "login") == 0)
    {
#ifdef SUPPORT_BIZPOTAL
        if (StrLen(value) == 0)
#endif  // #ifdef SUPPORT_BIZPOTAL
        {
            // bizportal�������...
            value = NNshParam->userName[NNshParam->protocol];
        }
        goto ENTRY_DATA;
    }

    // �p�X���[�h���H
    if (StrCaselessCompare(NNshGlobal->tempBuf, "password") == 0)
    {
        value = NNshParam->password[NNshParam->protocol];
        NNshGlobal->setPass = 1;
        goto ENTRY_DATA;
    }

    // �p�X���[�h���H (FON�����̑Ή�)
    if (StrCaselessCompare(NNshGlobal->tempBuf, "login_password") == 0)
    {
        value = NNshParam->password[NNshParam->protocol];
        NNshGlobal->setPass = 1;
        goto ENTRY_DATA;
    }

    // �p�X���[�h���H (YahooBB mobile�����̑Ή�)
    if (StrCaselessCompare(NNshGlobal->tempBuf, "pw") == 0)
    {
        value = NNshParam->password[NNshParam->protocol];
        NNshGlobal->setPass = 1;
        goto ENTRY_DATA;
    }

ENTRY_DATA:
    if ((NNshGlobal->tempBuf[0] == '\0')||(*value == '\0'))
    {
        return;
    }

    // �f�[�^���擪�ł͂Ȃ���΁A "&" ��t������
    if (StrLen(NNshGlobal->workBuf) != 0)
    {
        StrCat(NNshGlobal->workBuf, "&");
    }

    // name & value �� �i�[����
    StrCat(NNshGlobal->workBuf, NNshGlobal->tempBuf);
    if (*value != '\0')
    {
        StrCat(NNshGlobal->workBuf, "=");
        StrCat(NNshGlobal->workBuf, value);
    }
    return;
}


/**----------------------------------------------------------------------**
 **    FORM�^�O�����o�ł��邩�`�F�b�N����
 **
 **                  NNshGlobal->tempBuf�Ɍ��o���ʂ��i�[����
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void pickup_formTag(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    Char  *ptr;
    UInt16 loop;

    // FORM�^�O�̐擪��T��
    NNshGlobal->tempPtr2 = StrStr(NNshGlobal->recvBuf, "<FORM");
    if (NNshGlobal->tempPtr2 == NULL)
    {
        NNshGlobal->tempPtr2 = StrStr(NNshGlobal->recvBuf, "<form");
    }
    if (NNshGlobal->tempPtr2 == NULL)
    {
        // FORM�^�O�𔭌��ł��Ȃ������A�A�A
        StrCopy(NNshGlobal->reqAddress, "/Authentication.php"); 
#ifdef DEBUG
        // �����I�Ƀt�H�[����ύX�������Ƃ�ʒm����
        FrmCustomAlert(ALTID_INFO, "FORCE Authentication.php", " ", " ");
#endif
        NNshGlobal->sendMethod = 
                               ((NNshGlobal->sendMethod)|(NNSHLOGIN_POST_METHOD));
        return;
    }
#ifdef DEBUG
        // ���o�����t�H�[����\������
        FrmCustomAlert(ALTID_INFO, "FRM : ", NNshGlobal->tempPtr2, " ");
#endif

#ifdef FORCE_LOGINPAGE_MOBILEPOINT
    // �����I��mobilepoint�̃��O�C���y�[�W�ݒ�Ƃ���
    StrCopy(NNshGlobal->reqAddress, "/Authentication.php"); 
    NNshGlobal->sendMethod = 
                           ((NNshGlobal->sendMethod)|(NNSHLOGIN_POST_METHOD));
    return;
#endif

    // �f�[�^������������
    MemSet(NNshGlobal->tempBuf, BUFSIZE, 0x00);

    loop = 0;
    ptr  = NNshGlobal->tempPtr2;

    // form�^�O�̓��e����͂���
    while ((ptr < (NNshGlobal->recvBuf + NNshParam->bufferSize))&&(*ptr != '>'))
    { 
        // �L�[���[�hname��������
        if (((*(ptr + 0) == 'a')||(*(ptr + 0) == 'A'))&&
            ((*(ptr + 1) == 'c')||(*(ptr + 1) == 'C'))&&
            ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
            ((*(ptr + 3) == 'i')||(*(ptr + 3) == 'I'))&&
            ((*(ptr + 4) == 'o')||(*(ptr + 4) == 'O'))&&
            ((*(ptr + 5) == 'n')||(*(ptr + 5) == 'N')))
        {
#ifdef DEBUG
            FrmCustomAlert(ALTID_INFO, "find action", " ", " ");
#endif

            // action�f�[�^�̐擪�܂Ń|�C���^�𑗂�
            ptr = ptr + 6;
            while (((*ptr == '"')||(*ptr == '\'')||(*ptr <= ' ')||
                   (*ptr == '='))&&(*ptr != '>')&&
                   ((ptr < (NNshGlobal->recvBuf + NNshParam->bufferSize))))
            {
                ptr++;
            }

            // �f�[�^��tempBuf�ɃR�s�[����
            while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr > ' ')&&
                   (*ptr != '>')&&(loop < (BUFSIZE - 1)))
            {
                NNshGlobal->tempBuf[loop] = *ptr;
                loop++;
                ptr++;
            }
            NNshGlobal->tempBuf[loop] = '\0';
            loop++;
#ifdef DEBUG
            FrmCustomAlert(ALTID_INFO, "TMP BUFF:", NNshGlobal->tempBuf, " ");
#endif
            break;
        }
        ptr++;
    }

    // ���\�b�h��POST�Ɍ��肷��(����̂��肩��...�K�v�������GET���Ή�...)
    NNshGlobal->sendMethod = 
                           ((NNshGlobal->sendMethod)|(NNSHLOGIN_POST_METHOD));

    // POST����y�[�W�������肷��
    if (*(NNshGlobal->tempBuf) == '/')
    {
        MemSet (NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
        StrNCopy(NNshGlobal->reqAddress, NNshGlobal->tempBuf, (BUFSIZE * 2) - 1);
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Req Addr:", NNshGlobal->reqAddress, " ");
#endif
        return;
    }
#ifdef DEBUG
    FrmCustomAlert(ALTID_INFO, "NEXT STEP :", NNshGlobal->reqAddress, " ");
#endif

#ifdef SUPPORT_BIZPOTAL
    // �Ȃ���action�� HTTP�������ꍇ...
    ptr = StrStr(NNshGlobal->tempBuf, "http://");
    if (ptr != NULL)
    {
        // �z�X�g���ƃ|�[�g�ԍ��𒊏o����...
        pickup_hostNameAndPortNum(ptr, NNshParam, NNshGlobal);
        
    }
    else
    {
        ptr = StrStr(NNshGlobal->tempBuf, "https://");
        if (ptr != NULL)
        {
            // �z�X�g���ƃ|�[�g�ԍ��𒊏o����...
            pickup_hostNameAndPortNum(ptr, NNshParam, NNshGlobal);
        }
    }
#endif // #ifdef SUPPORT_BIZPOTAL

    // action��url���ׂĂ��w�肳��Ă����ꍇ�B�B�B
    ptr = StrStr(NNshGlobal->tempBuf, "://");
    if (ptr != NULL)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "[FIND] ", ptr, " ");
#endif
        // �z�X�g���ƃ|�[�g�ԍ������͍��...
        ptr = ptr + sizeof("://") - 1;
        while ((*ptr != '/')&&(*ptr != '"')&&(*ptr != '\0'))
        {
            ptr++;
        }
        while (*ptr == '/')
        {
            ptr++;
        }
        MemSet  (NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
        StrNCopy(NNshGlobal->reqAddress, ptr, (BUFSIZE * 2) - 1);
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Req ADDR:", NNshGlobal->reqAddress, " ");
#endif
        return;
    }
    else
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "COULD NOT FIND URL =>", NNshGlobal->tempBuf, " ");
#endif
    }

    // �y�[�W���͑��΃A�h���X�Ȃ̂ŁA���݂̗v��path�𐶂����悤�ɂ���
    NNshGlobal->loopCount = StrLen(NNshGlobal->reqAddress);
    while (NNshGlobal->loopCount != 0)
    {
        (NNshGlobal->loopCount)--;
        if (NNshGlobal->reqAddress[NNshGlobal->loopCount] == '/')
        {
            break;
        }
    }
    NNshGlobal->reqAddress[NNshGlobal->loopCount] = '/';
    (NNshGlobal->loopCount)++;
    NNshGlobal->reqAddress[NNshGlobal->loopCount] = '\0';
    StrCat(NNshGlobal->reqAddress, NNshGlobal->tempBuf);
#ifdef DEBUG
    FrmCustomAlert(ALTID_INFO, "REQUEST ADDR:", NNshGlobal->reqAddress, " ");
#endif
    return;
}


/**************************************************************************/
/*   Cookie���E��                                                         */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
void pickup_cookie(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    Char *ptr;

    //MemSet(NNshGlobal->cookie, (BUFSIZE + MARGIN), 0x00);
    ptr = StrStr(NNshGlobal->recvBuf, "Set-Cookie");
    if (ptr == NULL)
    {
        // cookie�̎w�肪�Ȃ������A�I������
        return;
    }

    // Cookie�f�[�^���R�s�[����
    ptr = ptr + StrLen("Set-");
    MemMove(NNshGlobal->cookie, ptr, BUFSIZE);
    ptr = NNshGlobal->cookie;
    while ((*ptr != ';')&&(*ptr != '\0'))
    {
        ptr++;
    }
    *ptr = '\0';
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : ParseMessage                                               */
/*                                                  (���b�Z�[�W�̐��`����) */
/*                    buf : �R�s�[��̈�                                   */
/*                    src : �R�s�[���̈�                                   */
/*                   size : �R�s�[���o�b�t�@�T�C�Y                         */
/*              kanjiCode : �g�p���銿���R�[�h                             */
/*-------------------------------------------------------------------------*/
static void ParseMessage(Char *buf, Char *src, UInt32 size, UInt16 kanjiCode)
{
    Boolean kanjiMode;
    UInt16  len;
    Char   *ptr, *dst, dataStatus;

    dataStatus = MSGSTATUS_NAME;
    kanjiMode = false;

    // ���x�����߃��[�`������... (�P�����Âp�[�X����)
    dst  = buf;
    ptr  = src;

    while (ptr < (src + size))
    {
        // JIS�����R�[�h�̕ϊ����W�b�N
        if (kanjiCode == NNSH_KANJICODE_JIS)
        {
            // JIS��SHIFT JIS�̃R�[�h�ϊ�
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x24')&&(*(ptr + 2) == '\x42'))
            {
                // �������[�h�ɐ؂�ւ�
                kanjiMode = true;
                ptr = ptr + 3;
                continue;
            }
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x28')&&(*(ptr + 2) == '\x42'))
            {
                // ANK���[�h�ɐ؂�ւ�
                kanjiMode = false;
                ptr = ptr + 3;
                continue;
            }
            if (kanjiMode == true)
            {
                // JIS > SJIS �����ϊ�
                ConvertJIStoSJ(dst, ptr);
                ptr = ptr + 2;
                dst = dst + 2;
                continue;
            }
        }
        if (*ptr == '&')
        {
            // "&gt;" �� '>' �ɒu��
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&lt;" �� '<' �ɒu��
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '<';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&quot;" �� '"' �ɒu��
            if ((*(ptr + 1) == 'q')&&(*(ptr + 2) == 'u')&&
                (*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&(*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" �� '    ' �ɒu��
            if ((*(ptr + 1) == 'n')&&(*(ptr + 2) == 'b')&&
                (*(ptr + 3) == 's')&&(*(ptr + 4) == 'p')&&(*(ptr + 5) == ';'))
            {
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&amp;" �� '&' �ɒu��
            if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
            {
                *dst++ = '&';
                ptr = ptr + 5;    // StrLen(TAG_AMP);
                continue;
            }
            // ����͂��肦�Ȃ��͂�����...�ꉞ�B
            *dst++ = '&';
            ptr++;
            continue;
        }
        if (*ptr == '<')
        {
            //  "<>" �́A�Z�p���[�^(��Ԃɂ���ĕς��)
            if (*(ptr + 1) == '>')
            {
                ptr = ptr + 2;   // StrLen(DATA_SEPARATOR);
                switch (dataStatus)
                {
                  case MSGSTATUS_NAME:
                    // ���O���̋�؂�
                    *dst++ = ' ';
                    *dst++ = ' ';
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_EMAIL;
                    break;

                  case MSGSTATUS_EMAIL:
                    // e-mail���̋�؂�
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_DATE;
                    *dst++ = '\n';
                    break;

                  case MSGSTATUS_DATE:
                    // �N�����E���Ԃ���тh�c���̋�؂�
                    *dst++ = '\n';
                    *dst++ = '\n';
                    dataStatus = MSGSTATUS_NORMAL;
                    if (*ptr == ' ')
                    {
                        // �X�y�[�X�͓ǂݔ�΂�
                        ptr++;
                    }
                    break;

                  case MSGSTATUS_NORMAL:
                  default:
                    // ���b�Z�[�W�̏I�[
                    *dst++ = ' ';
                    *dst++ = '\0';
                    return;
                    break;
                }
                continue;
            }
            //  "<br>" �́A���s�ɒu��
            if (((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                // �s���ƍs���̃X�y�[�X������Ă݂�ꍇ
                if ((ptr > src)&&(*(ptr - 1) == ' '))
                {
                    dst--;
                }
                if (*(ptr + 4) == ' ')
                {
                    *dst++ = '\n';
                    ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                    continue;
                }
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<p*>" �́A���s2�ɒu��
            if ((*(ptr + 1) == 'p')||(*(ptr + 1) == 'P'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 2;
                while ((*ptr != '>')&&(ptr < (src + size)))
                {
                    ptr++;
                }
                ptr++;
                continue;
            }

            // <li>�^�O�����s�R�[�h��:�ɒu������
            if (((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'i')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'L')&&(*(ptr + 2) == 'I')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '�';
                ptr = ptr + 4;
                continue;
            }   

            //  "<hr>" �́A���s === ���s �ɒu��
            if (((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'H')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dt>"�́A���s�ɒu��
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "</tr>" ����� "</td>" �́A���s�ɒu��
            if (((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'r')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'R')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'd')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'D')&&(*(ptr + 4) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dd>" �́A���s�Ƌ󔒂Q�ɒu��
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'd')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'D')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<h?" �́A���s + ���s �ɒu��
            if ((*(ptr + 1) == 'h')&&(*(ptr + 1) == 'H'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "</h?>" �́A���s + ���s �ɒu��
            if (((*(ptr + 2) == 'h')||(*(ptr + 2) == 'H'))&&(*(ptr + 1) == '/'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 3;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "</title>" �́A���s + ���s �ɒu��
            if ((*(ptr + 1) == '/')&&
                ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
                ((*(ptr + 3) == 'i')||(*(ptr + 3) == 'I'))&&
                ((*(ptr + 4) == 't')||(*(ptr + 4) == 'T'))&&
                ((*(ptr + 5) == 'l')||(*(ptr + 5) == 'L'))&&
                ((*(ptr + 6) == 'e')||(*(ptr + 6) == 'E')))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 7;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            // <tr>, <td> �̓X�y�[�X�P�ɕϊ�
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R')||
                 (*(ptr + 2) == 'd')||(*(ptr + 2) == 'D')))
            {
                *dst++ = ' ';
                // continue�͂Ȃ��B�B (�^�O�͓ǂݔ�΂�����)
            }
SKIP_TAG:
            // ���̑��̃^�O�͓ǂݔ�΂�
#ifdef USE_STRSTR
            ptr = StrStr(ptr, ">");
#else
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
#endif
            ptr++;
            // *dst++ = ' ';   // �^�O�͊��S�����Ƃ���B
            continue;
        }
        // �����R�[�h��EUC�������ꍇ...
        if (kanjiCode == NNSH_KANJICODE_EUC)
        {
            if (ConvertEUCtoSJ((UInt8 *) dst, (UInt8 *) ptr, &len) == true)
            {
                // EUC > SHIFT JIS�ϊ������s����
                dst = dst + len;
                ptr = ptr + 2;
                continue;
            }
            if (*ptr != '\0')
            {
                // ���ʂ̈ꕶ���]��
                *dst++ = *ptr;
            }
            ptr++;
            continue;
        }

        // �X�y�[�X���A�����Ă����ꍇ�A�P�Ɍ��炷
        if ((*ptr == ' ')&&(*(ptr + 1) == ' '))
        {
            ptr++;
            while ((*ptr == ' ')&&(ptr < (src + size)))
            {
                ptr++;
            }
            ptr--;
        }

        // NULL ����� 0x0a, 0x0d, 0x09(�^�u) �͖�������
        if ((*ptr != '\0')&&(*ptr != 0x0a)&&(*ptr != 0x0d)&&(*ptr != 0x09))
        {
            // ���ʂ̈ꕶ���]��
            *dst++ = *ptr;
        }
        ptr++;
    }
    *dst++ = '\0';
    return;
}

/* --------------------------------------------------------------------------
 *   AMsoft�����UTF8�ϊ��e�[�u���𗘗p���Ď�������
 *       (http://amsoft.minidns.net/palm/gfmsg_chcode.html)
 * --------------------------------------------------------------------------*/

/*=========================================================================*/
/*   Function : StrNCopyUTF8toSJ                                           */
/*                     ������̃R�s�[(UTF8����SHIFT JIS�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, LocalID dbId)
{
    UInt8     buffer[2];
    UInt16    cnt, codes, ucode;
    Boolean   kanji;
    DmOpenRef dbRef;   

    // �ϊ��e�[�u�������邩�m�F����
    // dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        return;
    }
    dbRef = DmOpenDatabase(0 , dbId, dmModeReadOnly);

    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt <size))
    {
        if (*src < 0x80)
        {
            // ���p���[�h...
            *dst = *src;
            dst++;
            cnt++;
            src++;
            continue;
        }

        ucode = ((*src & 0x0f) << 12);
        ucode = ucode | ((*(src + 1) & 0x3f) << 6);
        ucode = ucode | ((*(src + 2) & 0x3f));

        buffer[0] = ((ucode & 0xff00) >> 8);
        buffer[1] = ((ucode & 0x00ff));

        // �����R�[�h�̕ϊ�
        codes = Unicode11ToJisx0208(dbRef, buffer[0], buffer[1]);
        buffer[0] = ((codes & 0xff00) >> 8);
        buffer[1] = (codes & 0x00ff);
        ConvertJIStoSJ(dst, buffer);

        dst = dst + 2;
        src = src + 3;
        cnt = cnt + 2;
    }
    *dst = '\0';

    DmCloseDatabase(dbRef);
    return;
}

/* --------------------------------------------------------------------------
 *  Unicode11ToJisx0208()
 *                                                       (Unicode > JIS0208)
 * --------------------------------------------------------------------------*/
static UInt16 Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L)
{
    MemHandle            recH;
    UInt16               num, col, code = 0x0000;
    UnicodeToJis0208Rec *rec;

    // �f�[�^�x�[�X�̃��R�[�h���Ȃ������Ƃ�...
    if (dbRef == 0)
    {
        // �G���[...
        return (0x2121);
    }
    num  = utf8H;

    // �������킹���
    if (utf8L < 0x80)
    {
        num = (utf8H * 2);
        col = utf8L;
    }
    else
    {
        num = (utf8H * 2) + 1;
        col = utf8L - 0x80;
    }
    
    recH = DmQueryRecord(dbRef, num);
    if (recH == 0)
    {
        // �G���[...
        return (0x2122);
    }
    rec = (UnicodeToJis0208Rec *) MemHandleLock(recH);
    if (rec->size > 0)
    {
        // �f�[�^�L��
        code  = rec->table[col];
    }
    else
    {
        // �f�[�^�Ȃ�
        code = 0x2121;
    }
    MemHandleUnlock(recH);
    return (code);
}

/*=========================================================================*/
/*   Function : ConvertJIStoSJ                                             */
/*                            JIS�����R�[�h��SHIFT JIS�����R�[�h�ɕϊ����� */
/*=========================================================================*/
static Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr)
{
    UInt8 upper, lower;

    // ���8�r�b�g/����8�r�b�g��ϐ��ɃR�s�[����
    upper = *ptr;
    lower = *(ptr + 1);

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

    return (true);
}

/*=========================================================================*/
/*   Function : ConvertEUCtoSJ                                             */
/*                                  �����R�[�h��EUC����SHIFT JIS�ɕϊ����� */
/*=========================================================================*/
static Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte)
{
    UInt8 temp[2];

    *copyByte = 0;
    if (*ptr == 0x8e)
    {
        // ���p�J�i�R�[�h
        ptr++;
        *dst = *ptr;
        *copyByte = 1;
        return (true); 
    }
    if ((*ptr >= ((0x80)|(0x21)))&&(*ptr <= ((0x80)|(0x7e))))
    {
        // EUC�����R�[�h�Ɣ���AJIS�����R�[�h�Ɉ�x�ϊ����Ă���SHIFT JIS�ɕϊ�
        temp[0] = ((*ptr)&(0x7f));
        ptr++;
        temp[1] = ((*ptr)&(0x7f));
        (void) ConvertJIStoSJ(dst, temp);
        *copyByte = 2;
        return (true);
    }
    return (false);
}


/**----------------------------------------------------------------------**
 **   doLogin_YahooBBmobile 
 **     (YahooBB mobile�Ƀ��O�C�����������)
 **                                            type : �I��ISP
 **                                            msg  : ���O�C�����{����
 **----------------------------------------------------------------------**/
void doLogin_YahooBBmobile(NNshSavedPref *NNshParam,NNshWorkingInfo *NNshGlobal)
{
    // �����ڑ��́Ahttp://sourceforge.jp/ �Ƃ���  (�x�^�ɋL��...)
    MemSet(NNshParam->hostName, BUFSIZE, 0x00);
    StrCopy(NNshParam->hostName, "sourceforge.jp");

#ifdef DEBUG_LOCALHOST_ADDRESS
    // ���M����f�o�b�O�z�X�g�ɕύX����...
    StrCopy(NNshParam->hostName, DEBUG_ADDRESS);
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS

    // �����|�[�g�ԍ����i�[����
    NNshParam->portNum = 80;

    // Cookie������������
    MemSet(NNshGlobal->cookie, (BUFSIZE + MARGIN), 0x00);

    // �����t�@�C�������i�[����
    MemSet (NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
    StrCopy(NNshGlobal->reqAddress, "/");

    // ���b�Z�[�W�{�f�B�Ȃ�
    MemSet (NNshGlobal->workBuf, ((NNshParam->bufferSize) / 2), 0x00);
    StrCopy(NNshGlobal->workBuf, " ");

    // HTTP, GET���\�b�h���g�p����
    NNshGlobal->sendMethod = NNSILOGIN_GET_METHOD_HTTP;

    // ���b�Z�[�W���쐬
    NNsh_MakeHTTPmsg(NNshParam, NNshGlobal);

#ifdef DEBUG_SENDLOG
    // �f�o�b�O�p�ɑ��M�f�[�^���������֏o�͂���
    SendToLocalMemopad("FIRST CONNECTION", NNshGlobal->sendBuf);
#endif

    // HTTP�ʐM�����{
    NNshHTTP_comm(NNshParam, NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        // �ʐM���s�A�G���[��������
        FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", " ", " ");
#endif
        return;
    }

#ifdef DEBUG
    // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
    StrCat(NNshGlobal->recvBuf, "\n\n>< >< ><\n(FIRST REPLY)\n");
    SendToLocalMemopad("FIRST REPLY(RX)", NNshGlobal->recvBuf);
#endif

    // ��M���b�Z�[�W�Ƀ��_�C���N�g�̎w�������邩�A�`�F�b�N���s��
    NNshGlobal->err = ~errNone - 9;
    if ((StrStr(NNshGlobal->recvBuf, "HTTP/1.0 302") != NULL)||
        (StrStr(NNshGlobal->recvBuf, "HTTP/1.1 302") != NULL))
    {
        // ���_�C���N�g�̃z�X�g���ƃT�C�g�𒊏o����
        NNshParam->portNum = 0;
        NNsh_check302URL(NNshParam, NNshGlobal);
    }
    else
    {
#ifdef DEBUG
        // ���_�C���N�g���Ȃ��������Ƃ𑗐M����
        FrmCustomAlert(ALTID_INFO, "DO NOT REDIRECT...", " ", " ");
#endif
    }

    // ���t�@�����N���A����
    MemSet(NNshGlobal->referer, BUFSIZE, 0x00);

REDIRECT_AGAIN:
    if (NNshGlobal->err == errNone)
    {
#ifdef DEBUG_LOCALHOST_ADDRESS
    // ���_�C���N�g�̃A�h���X���擾�ł���
        // ���M����f�o�b�O�z�X�g�ɕύX����...
        StrCopy(NNshParam->hostName, DEBUG_ADDRESS);
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS

        // ���b�Z�[�W���쐬
        NNsh_MakeHTTPmsg(NNshParam, NNshGlobal);

#ifdef DEBUG
        // �f�o�b�O�p�ɑ��M�f�[�^���������֏o�͂���
        SendToLocalMemopad("SEND(REDIRECT)", NNshGlobal->sendBuf);
#endif

        // ���_�C���N�g��Ɂi���M�v���g�R���ɍ��킹�āj����
        if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) ==
                                                        NNSHLOGIN_SSL_PROTOCOL)
        {
#ifdef DEBUG
            // HTTPS�ŒʐM���邱�Ƃ�\��
            FrmCustomAlert(ALTID_INFO, "HTTPS(SSL) CONNECTION...", " ", " ");
#endif

            // https�ʐM��v��
            NNshHTTPS_comm(NNshParam, NNshGlobal);
        }
        else
        {
#ifdef DEBUG
            // HTTPS�ŒʐM���邱�Ƃ�\��
            FrmCustomAlert(ALTID_INFO, "HTTP CONNECTION...", " ", " ");
#endif

            // http�ʐM��v��
            NNshHTTP_comm(NNshParam, NNshGlobal);
        }
        if (NNshGlobal->err != errNone)
        {
#ifdef DEBUG
            // �ʐM���s�A�G���[��������
            FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", " ", "(redirect)");
#endif
            return;
        }
#ifdef DEBUG
        // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
        StrCat(NNshGlobal->recvBuf, "\n\n>< >< ><\n(REDIRECT)\n");
        SendToLocalMemopad("REDIRECT(RX)", NNshGlobal->recvBuf);
#endif
    }

#if 1
    // ����񃊃_�C���N�g�H
    if ((StrStr(NNshGlobal->recvBuf, "HTTP/1.0 302") != NULL)||
        (StrStr(NNshGlobal->recvBuf, "HTTP/1.1 302") != NULL))
    {
        if (NNshGlobal->isRedirect < 2)
        {
            // ���_�C���N�g�̃z�X�g���ƃT�C�g�𒊏o����
            NNsh_check302URL(NNshParam, NNshGlobal);
            (NNshGlobal->isRedirect)++;  // ���_�C���N�g�͂Q��܂�...
            goto REDIRECT_AGAIN;
        }
    }
#endif    //#if 0

    // ����LAN��y���ȂǁA�ēx�y�[�W���J���Ȃ��Ă͂Ȃ�Ȃ��ꍇ...
    NNsh_checkRefreshConnectionRequest(NNshParam, NNshGlobal);
    if (NNshGlobal->err == errNone)
    {
#ifdef DEBUG
        // �ēx���_�C���N�g���邱�Ƃ𑗐M����
        FrmCustomAlert(ALTID_INFO, 
                       "REDIRECT AGAIN...(", NNshGlobal->reqAddress, ")");
#endif
        goto REDIRECT_AGAIN;
    }

    Analysis_YahooBBLoginLink(NNshParam, NNshGlobal);
    if (NNshGlobal->err == errNone)
    {
#ifdef DEBUG
        // ���O�C���t�H�[�����J���悤���M����
        FrmCustomAlert(ALTID_INFO, 
                       "OPEN LOGIN FORM...(", NNshGlobal->reqAddress, ")");
#endif
        goto REDIRECT_AGAIN;
    }

ANALYSIS_LOGIN:
    // ���O�C���t�H�[���̉�͂Ƒ��M�{�f�B�̍쐬
    Analysis_LoginForm(NNshParam, NNshGlobal);

#ifdef DEBUG_LOCALHOST_ADDRESS
    // ���M����f�o�b�O�z�X�g�ɕύX����...
    StrCopy(NNshParam->hostName, DEBUG_ADDRESS);
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS

#ifdef DEBUG
    FrmCustomAlert(ALTID_INFO, "MAKE HTTP TO LOGIN", " ", NNshGlobal->workBuf);
#endif

    // ���O�C���p�̃��b�Z�[�W���쐬
    NNsh_MakeHTTPmsg(NNshParam, NNshGlobal);

#ifdef DEBUG_SENDLOG
    // �f�o�b�O�p�ɑ��M�f�[�^���������֏o�͂���
    SendToLocalMemopad("SEND(DO LOGIN 1)", NNshGlobal->sendBuf);
#endif

    // ���O�C�����b�Z�[�W��(���M�v���g�R���ɍ��킹��)���M����
    if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) == 
                                                        NNSHLOGIN_SSL_PROTOCOL)
    {
        // https�ʐM��v��
        NNshHTTPS_comm(NNshParam, NNshGlobal);
    }
    else
    {
        // http�ʐM��v��
        NNshHTTP_comm(NNshParam, NNshGlobal);
    }
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        // �ʐM���s�A�G���[��������
        FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", " ", "(redirect)");
#endif
        return;
    }
#ifdef DEBUG
    // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
    StrCat(NNshGlobal->recvBuf, "\n\n>< >< ><\n(LOGIN DETAIL)\n");
    SendToLocalMemopad("LOGIN DETAIL(RX)", NNshGlobal->recvBuf);
#endif

    if (NNshGlobal->setPass == 0)
    {
        // �p�X���[�h��ݒ�ł����܂ł����A�ēx���O�C���ցB�B�B
        // MLC�����C��...
        NNshGlobal->setPass = 1;
#ifdef DEBUG
        // �ʐM���s�A�G���[��������
        FrmCustomAlert(ALTID_INFO, "NOT PASSWORD...", " ", "(redirect again)");
#endif
        goto ANALYSIS_LOGIN;
    }


    if ((StrStr(NNshGlobal->recvBuf, "HTTP/1.0 302") != NULL)||
        (StrStr(NNshGlobal->recvBuf, "HTTP/1.1 302") != NULL))
    {
        // ���_�C���N�g�̃z�X�g���ƃT�C�g�𒊏o����
        NNshParam->portNum = 0;
        NNsh_check302URL(NNshParam, NNshGlobal);
    }
    else
    {
#ifdef DEBUG
        // ���_�C���N�g���Ȃ��������Ƃ𑗐M����
        FrmCustomAlert(ALTID_INFO, "DO NOT REDIRECT...", " ", " ");
#endif
    }

    // ���b�Z�[�W���쐬
    NNsh_MakeHTTPmsg(NNshParam, NNshGlobal);

#ifdef DEBUG
    // �f�o�b�O�p�ɑ��M�f�[�^���������֏o�͂���
    SendToLocalMemopad("SEND(FINAL REDIRECT)", NNshGlobal->sendBuf);
#endif

    // ���_�C���N�g��Ɂi���M�v���g�R���ɍ��킹�āj����
    if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) ==
                                                        NNSHLOGIN_SSL_PROTOCOL)
    {
#ifdef DEBUG
        // HTTPS�ŒʐM���邱�Ƃ�\��
        FrmCustomAlert(ALTID_INFO, "HTTPS(SSL) CONNECTION...", " ", " ");
#endif

        // https�ʐM��v��
        NNshHTTPS_comm(NNshParam, NNshGlobal);
    }
    else
    {
#ifdef DEBUG
        // HTTPS�ŒʐM���邱�Ƃ�\��
        FrmCustomAlert(ALTID_INFO, "HTTP CONNECTION...", " ", " ");
#endif

        // http�ʐM��v��
        NNshHTTP_comm(NNshParam, NNshGlobal);
    }

#ifdef DEBUG
    // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
    StrCat(NNshGlobal->recvBuf, "\n\n>< >< ><\n(--LOGIN--)\n");
    SendToLocalMemopad("LOGIN DETAIL(Final RedirectRX)", NNshGlobal->recvBuf);
#endif

    // ���O�C����"���s"�������ǂ����m�F����
    if (StrStr(NNshGlobal->recvBuf, "���s") != NULL)
    {
        // ���s�Ƃ��������񂪂������ꍇ�ɂ́A���O�C���Ɏ��s�����ƔF������
        NNshGlobal->err = ~errNone;
    }
    return;
}

#ifdef SUPPORT_BIZPOTAL
/**----------------------------------------------------------------------**
 **   URL�w�肩��z�X�g����URL�𒊏o����
 ** 
 **----------------------------------------------------------------------**/
static void pickup_hostNameAndPortNum(Char *buf, NNshSavedPref *NNshParam,NNshWorkingInfo *NNshGlobal)
{
    Char *ptr;

    ptr = StrStr(buf, "://");
    if (ptr == NULL)
    {
        // URL�w�肪������Ȃ������A�I������
        return;
    }
    ptr = ptr + sizeof("://") - 1;

    // �z�X�g���̃R�s�[
    MemSet(NNshParam->hostName, BUFSIZE, 0x00);
    for (NNshGlobal->loopCount = 0; 
         NNshGlobal->loopCount < (BUFSIZE - 1); 
         (NNshGlobal->loopCount)++)
    {
        if (*ptr == '/')
        {
            // �z�X�g���̏I�[
            break;
        }
        if (*ptr == ':')
        {
            // �|�[�g�ԍ����w�肳��Ă���...
            NNshGlobal->sendMethod =
                       (NNshGlobal->sendMethod)|(NNSHLOGIN_CONNECT_PORT);
            ptr++;

            // �|�[�g�ԍ��̓���
            NNshParam->portNum = 0;
            while ((*ptr >= '0')&&(*ptr <= '9'))
            {
                NNshParam->portNum = NNshParam->portNum * 10 + (*ptr - '0');
                ptr++;
            }
            break;
        }
        NNshParam->hostName[NNshGlobal->loopCount] = *ptr;
        ptr++;
    }
    return;
}
#endif // #ifdef SUPPORT_BIZPOTAL

/**----------------------------------------------------------------------**
 **    ���O�C���t�H�[���̉�͂Ƒ��M�{�f�B�̉��
 ** 
 **                NNshGlobal->recvBuf �F ��M�{�f�B
 **                NNshGlobal->workBuf �F ���M�p�{�f�B(�����ō쐬����)
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void Analysis_YahooBBLoginLink(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal)
{
    Char *ptr, *dst;

    // �����R�[�h������������
    NNshGlobal->err = errNone;

    // NNshGlobal->sendBuf �����[�N�o�b�t�@�Ƃ��ė��p����
    MemSet(NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);

    // Cookie���E��
    pickup_cookie(NNshParam, NNshGlobal);

    ptr = StrStr(NNshGlobal->recvBuf, "href=\"mobile_login.jsp");
    if (ptr == NULL)
    {
        // �w�肪�Ȃ������A�I������
        NNshGlobal->err = ~errNone;
        return;
    }
    ptr = ptr + sizeof("href=\"") - 1;

    dst = NNshGlobal->sendBuf;
    while ((*ptr != '\"')&&(*ptr != '\0'))
    {
        *dst = *ptr;
        dst++;
        ptr++;
    }

    // Yahoo! BB Mobile�̐ݒ��...
    ptr = StrStr(NNshGlobal->sendBuf, "?ID=");
    if (ptr != NULL)
    {
        StrCopy(ptr, "?ID=ybbm&oID=1");    
    }

    // �ʐM��t�@�C�������擾����
    NNshGlobal->tempPtr = NNshGlobal->sendBuf;
    NNshGlobal->loopCount = 0; 
    if (*(NNshGlobal->tempPtr) != '/')
    {
        // ���΃A�h���X�Ȃ̂ŁA���݂̗v��path�𐶂����悤�ɂ���
        NNshGlobal->loopCount = StrLen(NNshGlobal->reqAddress);
        while (NNshGlobal->loopCount != 0)
        {
            (NNshGlobal->loopCount)--;
            if (NNshGlobal->reqAddress[NNshGlobal->loopCount] == '/')
            {
                break;
            }
        }
        NNshGlobal->reqAddress[NNshGlobal->loopCount] = '/';
        (NNshGlobal->loopCount)++;
    }
    else
    {
        MemSet(NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
    }
    while (NNshGlobal->loopCount < (BUFSIZE - 1))
    {
        if (*(NNshGlobal->tempPtr) <= ' ')
        {
            break;
        }
        NNshGlobal->reqAddress[NNshGlobal->loopCount] = *(NNshGlobal->tempPtr);
        (NNshGlobal->tempPtr)++;
        (NNshGlobal->loopCount)++;
    }

    // �A�h���X�������Ȃ������� / �ɂ���
    if (StrLen(NNshGlobal->reqAddress) == 0)
    {
        StrCopy(NNshGlobal->reqAddress, "/");
    }
#ifdef DEBUG
    // ��͌��ʂ̃��O���o�͂���
    FrmCustomAlert(ALTID_INFO, NNshParam->hostName, " : ", NNshGlobal->reqAddress);
#endif

    return;
}
