/*============================================================================*
 *  $Id: damain.c,v 1.20 2006/09/09 05:01:00 mrsa Exp $
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
Err  SendToLocalMemopad(Char *title, Char *data);

static void TrimingMessage(Char *buf, UInt32 bufSize);
static void ParseMessage(Char *buf, Char *src, UInt32 dstSize, UInt32 size, UInt16 kanjiCode);

// UTF8 -> Shift JIS�ϊ��e�[�u�� (AMsoft����̂c�a���g�p����)
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, LocalID dbId);

static Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr);
static Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte);
static UInt16  Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L);

static Boolean analysis_URL(NNshWorkingInfo *NNshGlobal);
static void    get_html_contents(NNshWorkingInfo *NNshGlobal);

static void NNsh_checkRefreshConnectionRequest(NNshWorkingInfo *NNshGlobal);
static void CheckClipboard(NNshWorkingInfo *adtP);

void StrNCopy_URLEncode(UInt8 *dst, UInt32 bufSize, UInt8 *src, UInt32 size);

#ifdef DEBUG_LOCALHOST_ADDRESS
#define  DEBUG_ADDRESS  "localhost"
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS
 
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
    Boolean           isURL;
    UInt16            size, autoOffTime, btnId, ref;
    UInt32            index;
    NNshWorkingInfo  *NNshGlobal;
    FieldType        *fldP;
    Char             *tempPtr;
    LocalID           dbId;

    // URL�̎w�肪�s��ꂽ���ǂ���
    isURL = false;

    // NNsi�O���[�o���̈�p�i�[�̈�̊m��
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // �̈�m�ۂɎ��s�I(�N�����Ȃ�)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " global");
        return;
    }        
    MemSet(NNshGlobal, size, 0x00);

    // �ʐM�^�C���A�E�g�l��ݒ�
    NNshGlobal->timeout = SysTicksPerSecond() * NNSH_GETMAIL_TIMEOUT;

    // Parameter pointer��feature�ɃZ�b�g
    FtrSet(SOFT_CREATOR_ID, SOFT_DB_ID, (UInt32) NNshGlobal);

    // OS�o�[�W�����̎擾
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &(NNshGlobal->palmOSVersion));

    // URL�̎擾(�I�𒆃t�B�[���h���N���b�v�{�[�h�ɃR�s�[)
    // �I�I�I���ӁI�I�I
    //    ����������C�ɂ������߁A�ϐ��𗬗p���Ă���I (�悭�Ȃ��I)
    //      fldId �� NNshGlobal->isChecking
    //      start �� NNshGlobal->portNum
    //      end   �� NNshGlobal->kanjiCode
    NNshGlobal->isChecking = FrmGetFocus(FrmGetActiveForm());
    if (NNshGlobal->isChecking != noFocus)
    {
        if (FrmGetObjectType(FrmGetActiveForm(), NNshGlobal->isChecking) == frmFieldObj)
        {
            FldGetSelection(FrmGetObjectPtr(FrmGetActiveForm(), 
                                            NNshGlobal->isChecking),
                            &(NNshGlobal->portNum),
                            &(NNshGlobal->kanjiCode));
            if (NNshGlobal->portNum != NNshGlobal->kanjiCode)
            {
                // �I�𕶎�����R�s�[����
                FldCopy(FrmGetObjectPtr(FrmGetActiveForm(),
                                        NNshGlobal->isChecking));
            }
        }
    }
    NNshGlobal->isChecking = 0;
    NNshGlobal->portNum    = 0;
    NNshGlobal->kanjiCode  = 0;

    // �N���b�v�{�[�h���ɂ���R�}���h�̉��...
    CheckClipboard(NNshGlobal);

    // �N���b�v�{�[�h���ɃR�}���h���m�F�ł��Ȃ������ꍇ�A�A�A
    //  (�N���b�v�{�[�h����URL�������擾����)
    if (NNshGlobal->sendMethod == NNSHLOGIN_METHOD_NOTSPECIFY)
    {
        NNshGlobal->memH = ClipboardGetItem(clipboardText,
                                            &(NNshGlobal->fieldLen));
        if ((NNshGlobal->memH != 0)&&(NNshGlobal->fieldLen != 0))
        {
            if (NNshGlobal->fieldLen > (BUFSIZE * 2))
            {
                NNshGlobal->fieldLen = (BUFSIZE * 2) - 1;
            }
            MemMove(NNshGlobal->reqURL, 
                    MemHandleLock(NNshGlobal->memH),
                    NNshGlobal->fieldLen);
            MemHandleUnlock(NNshGlobal->memH);
        }
        NNshGlobal->sendMethod = NNSILOGIN_GET_METHOD_HTTP;        
    }

    // URL�̉��
    isURL = analysis_URL(NNshGlobal);

    // URL���w�肳��Ă��Ȃ��ꍇ�ɂ́AURL���͂̃_�C�A���O��\������
    if (isURL == false)
    {
        MemSet(NNshGlobal->reqURL, (BUFSIZE * 2), 0x00);
        isURL = InputDialog("Input URL", NNshGlobal->reqURL, (BUFSIZE * 2));
        if (isURL != false)
        {
            // ���͂��ꂽ�f�[�^��URL���܂܂�Ă��邩��͂���
            isURL = analysis_URL(NNshGlobal);
            NNshGlobal->sendMethod = NNSILOGIN_GET_METHOD_HTTP;
        }
    }

    // URL���i�[����Ă����H
    if (isURL == false)
    {
        // �擾��URL���w�肳��Ȃ������A�I��������
        goto FUNC_END;
    }

    //////////////////////////////////////////////////////////////////////////
    // HTTP�擾������
    //////////////////////////////////////////////////////////////////////////
    // ���M�o�b�t�@�̊m��
    NNshGlobal->sendBuf = MemPtrNew(SENDBUFFER_DEFAULT);
    if (NNshGlobal->sendBuf == NULL)
    {
        // �̈�m�ێ��s�A�I��������
        goto FUNC_END;
    }
    MemSet(NNshGlobal->sendBuf, (SENDBUFFER_DEFAULT), 0x00);

    // ��M�o�b�t�@�̊m��
    NNshGlobal->recvBuf = MemPtrNew(RECEIVEBUFFER_DEFAULT + MARGIN);
    if (NNshGlobal->recvBuf == NULL)
    {
        // �̈�m�ێ��s�A�I��������
        MEMFREE_PTR(NNshGlobal->sendBuf);
        goto FUNC_END;
    }
    MemSet(NNshGlobal->recvBuf, (RECEIVEBUFFER_DEFAULT + MARGIN), 0x00);

    NNshGlobal->workBuf = MemPtrNew(WORKBUFFER_DEFAULT);
    if (NNshGlobal->workBuf == NULL)
    {
        // �̈�m�ێ��s�A�I��������
        MEMFREE_PTR(NNshGlobal->sendBuf);
        MEMFREE_PTR(NNshGlobal->recvBuf);
        goto FUNC_END;
    }
    MemSet(NNshGlobal->workBuf, (WORKBUFFER_DEFAULT), 0x00);

    // BUSY�E�B���h�E��\������
    Show_BusyForm(NNshGlobal->hostName, NNshGlobal);    

    // �����d��OFF�^�C�}���~������
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    // �g�s�l�k�f�[�^����M����
    get_html_contents(NNshGlobal);

    // �����d��OFF�^�C�}�𕜋�������
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    SetMsg_BusyForm("Parsing...");

    // �o�b�t�@�N���A
    MemSet(NNshGlobal->workBuf, (WORKBUFFER_DEFAULT), 0x00);

    // �^�C�g���ɗv������URL��\������悤�ɃR�s�[����
    MemSet(NNshGlobal->showBuf, (MINIBUF + MARGIN), 0x00);
    NNshGlobal->tempPtr = StrStr(NNshGlobal->reqURL, "://");
    if (NNshGlobal->tempPtr == NULL)
    {
        NNshGlobal->tempPtr = NNshGlobal->reqURL;
    }
    else
    {
        NNshGlobal->tempPtr = NNshGlobal->tempPtr + sizeof("://") - 1; 
    }
    MemMove(NNshGlobal->showBuf, NNshGlobal->tempPtr, DIALOGTITLE_LENGTH);

    // HTTP�w�b�_�̖�����T��
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

    // �\���T�C�Y���t�B�[���h�\���p�o�b�t�@�Ɏ��܂邩�H
    if (StrLen(NNshGlobal->tempPtr) < (WORKBUFFER_DEFAULT - 1))
        
    {
        NNshGlobal->fieldLen = StrLen(NNshGlobal->tempPtr);
    }
    else
    {
        NNshGlobal->fieldLen = (WORKBUFFER_DEFAULT - 2);
    }

    // �g�p�����R�[�h�̉��(�Ԃ����Ⴏ���\...)
    NNshGlobal->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
    if (StrStr(NNshGlobal->tempPtr, "iso-2022-jp") != NULL)
    {
        NNshGlobal->kanjiCode = NNSH_KANJICODE_JIS;
        goto HTML_PARSE;
    }
    if (StrStr(NNshGlobal->tempPtr, "ISO-2022-JP") != NULL)
    {
        NNshGlobal->kanjiCode = NNSH_KANJICODE_JIS;
        goto HTML_PARSE;
    }
    if (StrStr(NNshGlobal->tempPtr, "euc-jp") != NULL)
    {
        NNshGlobal->kanjiCode = NNSH_KANJICODE_EUC;
        goto HTML_PARSE;
    }
    if (StrStr(NNshGlobal->tempPtr, "EUC-JP") != NULL)
    {
        NNshGlobal->kanjiCode = NNSH_KANJICODE_EUC;
        goto HTML_PARSE;
    }
    if (StrStr(NNshGlobal->tempPtr, "utf-8") != NULL)
    {
        NNshGlobal->kanjiCode = NNSH_KANJICODE_UTF8;
        goto HTML_PARSE;
    }
    if (StrStr(NNshGlobal->tempPtr, "UTF-8") != NULL)
    {
        NNshGlobal->kanjiCode = NNSH_KANJICODE_UTF8;
        goto HTML_PARSE;
    }

HTML_PARSE:
    if (NNshGlobal->kanjiCode == NNSH_KANJICODE_UTF8)
    {
        // UTF8�`���̏ꍇ�A��x�ʂ̃G���A�Ƀf�[�^���R�s�[���Ă���...
        dbId = DmFindDatabase(0, "UnicodeToJis0208");
        if (dbId != 0)
        {
            //
            index   = NNshGlobal->tempPtr - NNshGlobal->recvBuf;
            tempPtr = NNshGlobal->recvBuf;
            NNshGlobal->recvBuf = MemPtrNew(RECEIVEBUFFER_DEFAULT + MARGIN);
            if (NNshGlobal->recvBuf != NULL)
            {
                MemSet(NNshGlobal->recvBuf, (RECEIVEBUFFER_DEFAULT + MARGIN), 0x00);
                StrNCopyUTF8toSJ(NNshGlobal->recvBuf, tempPtr, NNshGlobal->fieldLen, dbId);

                // �ϊ���̕����񂪍쐬�ł�����A�̈���J������
                MEMFREE_PTR(tempPtr);

                NNshGlobal->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
                NNshGlobal->tempPtr = NNshGlobal->recvBuf + index;
            }
            else
            {
                // �̈�m�ێ��s...RAW���[�h�ŕ\������
                NNshGlobal->recvBuf = tempPtr;
                NNshGlobal->kanjiCode = nnDA_KANJICODE_RAW;
            }
        }
        else
        {
            // UTF8 -> SHIFT JIS�ϊ�DB�����炸... RAW���[�h�ŕ\������
            NNshGlobal->kanjiCode = nnDA_KANJICODE_RAW;
        }
    }
    if (NNshGlobal->kanjiCode == nnDA_KANJICODE_RAW)
    {
        // ��M�f�[�^���܂�������͂����ɕ\������
        MemMove(NNshGlobal->workBuf,NNshGlobal->tempPtr,NNshGlobal->fieldLen);
    }
    else
    {
        // ��M�f�[�^��HTML�f�[�^�t�@�C���Ƃ���(�ȒP�ɉ�͂���)�\������
        ParseMessage(NNshGlobal->workBuf,NNshGlobal->tempPtr,
                WORKBUFFER_DEFAULT,NNshGlobal->fieldLen,NNshGlobal->kanjiCode);

        // �\���f�[�^�̐��䕶�����폜����i�g���~���O�������̂Q�j
        TrimingMessage(NNshGlobal->workBuf, StrLen(NNshGlobal->workBuf));
    }

    // BUSY�E�B���h�E���B��
    Hide_BusyForm(NNshGlobal);


    // ���ʃ_�C�A���O��\������
    if (FrmGetActiveFormID() != 0)
    {
        NNshGlobal->previousForm = FrmGetActiveForm();
    }
    else
    {
        NNshGlobal->previousForm = NULL;
    }

#ifdef USE_WIDEWINDOW
    NNshGlobal->ret = FtrGet(pinCreator, pinFtrAPIVersion, &(NNshGlobal->silkVer));
    if (NNshGlobal->ret != errNone)
    {
        // DIA ��T�|�[�g�f�o�C�X�A�_�C�A���O�T�C�Y�̊g��͂��Ȃ�
        NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
        FrmSetActiveForm   (NNshGlobal->currentForm);
        goto RESIZE_WINDOW;
    }

    // DIA�T�|�[�g�f�o�C�X    
    NNshGlobal->ret = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &(NNshGlobal->silkVer));
   if ((NNshGlobal->ret != errNone)|
       ((NNshGlobal->silkVer & grfFtrInputAreaFlagDynamic) == 0)||
       ((NNshGlobal->silkVer & grfFtrInputAreaFlagCollapsible) == 0))
   {
        // �G���[�ɂȂ�ꍇ�A��������DIA��T�|�[�g�̏ꍇ�A��������������
        NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
        FrmSetActiveForm   (NNshGlobal->currentForm);
        goto RESIZE_WINDOW;
   }
   // ���݂̃V���N�̈��Ԃ��擾����
   // btnId = PINGetInputAreaState();

   // �t�H�[�����I�[�u������
   NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
   FrmSetActiveForm   (NNshGlobal->currentForm);

   // �V���N�����L���ɂ���
   FrmSetDIAPolicyAttr(NNshGlobal->currentForm, frmDIAPolicyCustom);
   PINSetInputTriggerState(pinInputTriggerEnabled);

   // ���炢�x�^�A�A�A�E�B���h�E�T�C�Y�̍ő�l�ŏ��l��ݒ�
   WinSetConstraintsSize(FrmGetWindowHandle(NNshGlobal->currentForm),
                         160, 225, 225, 160, 225, 225);

   // �V���N�̈��Ԃ�O��ʂƓ����ɂ���...
   // PINSetInputAreaState(btnId);
   PINSetInputAreaState(pinInputAreaUser);


RESIZE_WINDOW:
    /* �S�̂�Window�T�C�Y�擾 */
    WinGetDisplayExtent(&(NNshGlobal->x), &(NNshGlobal->y));

    /* dialog�̃T�C�Y���擾 */
    FrmGetFormBounds(NNshGlobal->currentForm, &(NNshGlobal->r));

    /* dialog(�E�C���h�E)�̊g�� */
    NNshGlobal->r.topLeft.x = 2;
    NNshGlobal->r.topLeft.y = 4;

    NNshGlobal->diffX       = NNshGlobal->r.extent.x;
    NNshGlobal->diffY       = NNshGlobal->r.extent.y;

    NNshGlobal->r.extent.y  = NNshGlobal->y - 6;
    NNshGlobal->r.extent.x  = NNshGlobal->x - 4;

    NNshGlobal->diffX       = NNshGlobal->r.extent.x - NNshGlobal->diffX;
    NNshGlobal->diffY       = NNshGlobal->r.extent.y - NNshGlobal->diffY;

    WinSetWindowBounds(FrmGetWindowHandle(NNshGlobal->currentForm),
                       &NNshGlobal->r);

    /* OK �{�^���̈ړ� */
    ref = FrmGetObjectIndex(NNshGlobal->currentForm, BTNID_DIALOG_OK);
    FrmGetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);
    // NNshGlobal->r.topLeft.x = NNshGlobal->r.topLeft.x + NNshGlobal->diffX;
    NNshGlobal->r.topLeft.y = NNshGlobal->r.topLeft.y + NNshGlobal->diffY;
    FrmSetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);

    /* REDRAW �{�^���̈ړ� */
    ref = FrmGetObjectIndex(NNshGlobal->currentForm, BTNID_DIALOG_REDRAW);
    FrmGetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);
    // NNshGlobal->r.topLeft.x = NNshGlobal->r.topLeft.x + NNshGlobal->diffX;
    NNshGlobal->r.topLeft.y = NNshGlobal->r.topLeft.y + NNshGlobal->diffY;
    FrmSetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);
    // FrmHideObject(NNshGlobal->currentForm, ref);                

    /* UP �{�^���̈ړ� */
    ref = FrmGetObjectIndex(NNshGlobal->currentForm, BTNID_UP);
    FrmGetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);
    // NNshGlobal->r.topLeft.x = NNshGlobal->r.topLeft.x + NNshGlobal->diffX;
    NNshGlobal->r.topLeft.y = NNshGlobal->r.topLeft.y + NNshGlobal->diffY;
    FrmSetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);

    /* DOWN �{�^���̈ړ� */
    ref = FrmGetObjectIndex(NNshGlobal->currentForm, BTNID_DOWN);
    FrmGetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);
    // NNshGlobal->r.topLeft.x = NNshGlobal->r.topLeft.x + NNshGlobal->diffX;
    NNshGlobal->r.topLeft.y = NNshGlobal->r.topLeft.y + NNshGlobal->diffY;
    FrmSetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);

    /* Field�̈ړ�(�g��) */
    ref = FrmGetObjectIndex(NNshGlobal->currentForm, FLDID_INFOFIELD);
    FrmGetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);
    NNshGlobal->r.extent.x = NNshGlobal->r.extent.x + NNshGlobal->diffX;
    NNshGlobal->r.extent.y = NNshGlobal->r.extent.y + NNshGlobal->diffY;
    FrmSetObjectBounds(NNshGlobal->currentForm, ref, &NNshGlobal->r);

#else  // #ifdef USE_WIDEWINDOW
    // �E�B���h�E�̃I�[�v��
    NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
    FrmSetActiveForm   (NNshGlobal->currentForm);
#endif // #ifdef USE_WIDEWINDOW

    // �^�C�g����ݒ肷��
    FrmCopyTitle(NNshGlobal->currentForm, NNshGlobal->showBuf);

    // ��M�f�[�^���t�B�[���h�ɃZ�b�g����
    NNshWinSetFieldText(NNshGlobal->currentForm, FLDID_INFOFIELD,
                        NNshGlobal->workBuf, StrLen(NNshGlobal->workBuf));

    // 
    fldP = FrmGetObjectPtr(NNshGlobal->currentForm,
                           FrmGetObjectIndex(NNshGlobal->currentForm,
                                             FLDID_INFOFIELD));
    if (FldScrollable(fldP, winDown) != true)
    {
        // ���X�N���[�����ł��Ȃ��ꍇ�A���X�N���[���{�^���������B
        FrmHideObject(NNshGlobal->currentForm,
                      FrmGetObjectIndex(NNshGlobal->currentForm,
                                        BTNID_DOWN));
    }
    if (FldScrollable(fldP, winUp) != true)
    {
        // ��X�N���[�����ł��Ȃ��ꍇ�A��X�N���[���{�^���������B
        FrmHideObject(NNshGlobal->currentForm,
                      FrmGetObjectIndex(NNshGlobal->currentForm,
                                        BTNID_UP));
    }

    // �擾���ʃ_�C�A���O��\������
    FrmSetEventHandler(NNshGlobal->currentForm, Handler_MainForm);
    btnId = FrmDoDialog(NNshGlobal->currentForm);

    // �t�H�[���̍폜
    if (NNshGlobal->previousForm != NULL)
    {
        FrmSetActiveForm(NNshGlobal->previousForm);
    }
    FrmDeleteForm(NNshGlobal->currentForm);

    if (btnId == BTNID_DIALOG_REDRAW)
    {
        //  "��ʍĕ\��"�̏ꍇ...BUSY�E�B���h�E��\�����āA
        // �f�[�^�ĉ�͂�����Ȃ���
        Show_BusyForm("Parsing...", NNshGlobal);    

        goto HTML_PARSE;
    }

    // ����M�o�b�t�@�̉��
    MEMFREE_PTR(NNshGlobal->recvBuf);
    MEMFREE_PTR(NNshGlobal->sendBuf);
    MEMFREE_PTR(NNshGlobal->workBuf);

FUNC_END:
    //////////////////////////////////////////////////////////////////////////
    /////     �I�������i�ݒ�̕ۑ����j
    //////////////////////////////////////////////////////////////////////////
    // Global�̓o�^����
    FtrUnregister(SOFT_CREATOR_ID, SOFT_DB_ID);

    // �m�ۂ����̈���J������
    if (NNshGlobal->clipboardInfo != NULL)
    {
        MEMFREE_PTR(NNshGlobal->clipboardInfo);
    }
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

    if (event->eType != menuEvent)
    {
        // �L�[���͂̃`�F�b�N
        if (event->eType == keyDownEvent)
        {
            switch (event->data.keyDown.chr)
            {
              case vchrJogRelease:
              case vchrThumbWheelPush:
                // OK�{�^�������������Ƃɂ���
                CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                       FrmGetObjectIndex(FrmGetActiveForm(),
                                                         BTNID_DIALOG_OK)));
                return (true);
                break;

              default:
                // �������Ȃ� 
                break;
            }
        }
        return (false);
    }

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
    UInt32    version;

    // �p�����[�^�̎擾
    NNshWorkingInfo *NNshGlobal;
    FtrGet(SOFT_CREATOR_ID, SOFT_DB_ID, (UInt32 *)&NNshGlobal);

    // 5way-nav�𖳌��ɂ���(PalmTX�Ή�...)
    if (FtrGet(sysFtrCreator, sysFtrNumFiveWayNavVersion, &version) == errNone)
    {
        FrmSetNavState(FrmGetActiveForm(), kFrmNavStateFlagsInteractionMode);
    }

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

      case BTNID_DOWN:
      case vchrPageDown:
      case chrDownArrow:
      case vchrJogDown:
      case vchrRockerRight:
      case vchrJogRight:
      case chrRightArrow:
        // ���X�N���[��
        if (FrmGetActiveFormID() == FRMID_RESULT)
        {
          fldP = FrmGetObjectPtr(FrmGetActiveForm(), 
                                 FrmGetObjectIndex(FrmGetActiveForm(),
                                                   FLDID_INFOFIELD));
          FldScrollField(fldP, 5, winDown);
        }
        break;

      case BTNID_UP:
      case vchrPageUp:
      case chrUpArrow:
      case vchrJogUp:
      case vchrRockerLeft:
      case vchrJogLeft:
      case chrLeftArrow:
        // ��X�N���[��
        if (FrmGetActiveFormID() == FRMID_RESULT)
        {
          fldP = FrmGetObjectPtr(FrmGetActiveForm(), 
                                 FrmGetObjectIndex(FrmGetActiveForm(),
                                                   FLDID_INFOFIELD));
          FldScrollField(fldP, 5, winUp);
        }
        break;

      case MNUID_CODE_RAW:
      case chrCapital_R:
      case chrSmall_R:
        // "�x�^�\��"���[�h
        NNshGlobal->kanjiCode = nnDA_KANJICODE_RAW;

        // REDRAW�{�^�������������Ƃɂ���
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_SHIFTJIS:
      case chrCapital_S:
      case chrSmall_S:
        // SHIFT JIS�\�����[�h
        NNshGlobal->kanjiCode = NNSH_KANJICODE_SHIFTJIS;

        // REDRAW�{�^�������������Ƃɂ���
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_EUC:
      case chrCapital_E:
      case chrSmall_E:
        // EUC�\�����[�h
        NNshGlobal->kanjiCode = NNSH_KANJICODE_EUC;

        // REDRAW�{�^�������������Ƃɂ���
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_JIS:
      case chrCapital_J:
      case chrSmall_J:
        // JIS�\�����[�h
        NNshGlobal->kanjiCode = NNSH_KANJICODE_JIS;

        // REDRAW�{�^�������������Ƃɂ���
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_UTF8:
      case chrCapital_U:
      case chrSmall_U:
        // UTF8�\�����[�h
        NNshGlobal->kanjiCode = NNSH_KANJICODE_UTF8;

        // REDRAW�{�^�������������Ƃɂ���
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_DISCONNECT:
      case chrCapital_Q:
      case chrSmall_Q:
        // ����ؒf...
        NNshNet_LineHangup();
        break;

      case MNUID_SENDMEMO:
      case chrCapital_M:
      case chrSmall_M:
        // �������֏o��
        SendToLocalMemopad("html contents", NNshGlobal->workBuf);
        break;

      case vchrRockerCenter:
        // �n�j�����������Ƃɂ���
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                      BTNID_DIALOG_OK)));
        return (true);

      case BTNID_DIALOG_OK:
      case BTNID_DIALOG_CANCEL:
      case BTNID_DIALOG_REDRAW:
      default:
        return (false);
        break;
    }
    fldP = FrmGetObjectPtr(FrmGetActiveForm(), 
                           FrmGetObjectIndex(FrmGetActiveForm(),
                                             FLDID_INFOFIELD));
    if (FldScrollable(fldP, winUp) == true)
    {
        // ������X�N���[���\�ȏꍇ�A�{�^����L���ɂ���
        FrmShowObject(FrmGetActiveForm(),
                      FrmGetObjectIndex(FrmGetActiveForm(), BTNID_UP));
    }
    else
    {
        // �����ɂ���
        FrmHideObject(FrmGetActiveForm(),
                      FrmGetObjectIndex(FrmGetActiveForm(), BTNID_UP));
    }

    if (FldScrollable(fldP, winDown) == true)
    {
        // �������X�N���[���\�ȏꍇ�A�{�^����L���ɂ���
        FrmShowObject(FrmGetActiveForm(),
                      FrmGetObjectIndex(FrmGetActiveForm(), BTNID_DOWN));
    }
    else
    {
        // �����ɂ���
        FrmHideObject(FrmGetActiveForm(),
                      FrmGetObjectIndex(FrmGetActiveForm(), BTNID_DOWN));
    }
    return (true);
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
Err NNshNet_write(NNshWorkingInfo *NNshGlobal)
{
    NNshGlobal->ret = NetLibSend(NNshGlobal->netRef, 
                                 NNshGlobal->socketRef,
                                 NNshGlobal->sendBuf, 
                                 StrLen(NNshGlobal->sendBuf),
                                 0,
                                 NULL,
                                 0,
                                 NNshGlobal->timeout,
                                 &(NNshGlobal->err));
    if (NNshGlobal->ret != -1)
    {
        NNshGlobal->err = errNone;
    }
    return (NNshGlobal->err);
}

/*==========================================================================*/
/*  net_read() : �f�[�^����M����                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_read(NNshWorkingInfo *NNshGlobal)
{
    UInt16 length, copyLength;

    length     = WORKBUFFER_DEFAULT - MARGIN;
    copyLength = 0;

    NNshGlobal->ret      = 0;
    NNshGlobal->fieldLen = 0;

    // �f�[�^�͂��ׂĎ�M����悤�ɕύX����
    while (1)
    {
        MemSet(NNshGlobal->workBuf, (WORKBUFFER_DEFAULT), 0x00);
        NNshGlobal->fieldLen = NetLibReceive(NNshGlobal->netRef, 
                                      NNshGlobal->socketRef,
                                      NNshGlobal->workBuf,
                                      length,
                                      0, NULL, 0, 
                                      NNshGlobal->timeout,
                                      &(NNshGlobal->err));
        if (NNshGlobal->fieldLen != -1)
        {
            NNshGlobal->err = errNone;
        }
        if ((NNshGlobal->fieldLen == 0)||(NNshGlobal->fieldLen == -1))
        {
            break;
        }

        if (copyLength < (RECEIVEBUFFER_DEFAULT - MARGIN))
        {
            // �R�s�[����f�[�^(�Q�Ɖ\�ȃf�[�^)�́A�擪����L���o�b�t�@���̂�
            if ((NNshGlobal->fieldLen + copyLength) < (RECEIVEBUFFER_DEFAULT - MARGIN))
            {
                MemMove(&NNshGlobal->recvBuf[copyLength],
                        NNshGlobal->workBuf, 
                        NNshGlobal->fieldLen);
                copyLength = copyLength + NNshGlobal->fieldLen;
            }
            else
            {
                MemMove(&NNshGlobal->recvBuf[copyLength],
                        NNshGlobal->workBuf,
                        (RECEIVEBUFFER_DEFAULT - MARGIN) - copyLength);
                copyLength = RECEIVEBUFFER_DEFAULT;
            }
        }
        NNshGlobal->ret = NNshGlobal->ret + NNshGlobal->fieldLen;

        // BUSY�E�B���h�E���X�V����
        MemSet (NNshGlobal->showBuf, MINIBUF, 0x00);
        StrIToA(NNshGlobal->showBuf, NNshGlobal->ret);
        StrCat (NNshGlobal->showBuf, " bytes...");
        SetMsg_BusyForm(NNshGlobal->showBuf);
    }
    return (NNshGlobal->err);
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
Err NNshNet_connect(NNshWorkingInfo *NNshGlobal)
{
    UInt8                openFlag;
    Int16                addrLen;
    NetHostInfoPtr       hostIP;
    NetSocketAddrINType *addrType;
    NetSocketAddrType    addrBuf;
    NetHostInfoBufType  *infoBuff;

    // �����ڑ������X�V(�ǉ�...)
    NetLibConnectionRefresh(NNshGlobal->netRef,
                            true,
                            &openFlag,
                            &(NNshGlobal->ret));

    // �o�b�t�@�̈���m��
    infoBuff = MemPtrNew(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "MemPtrNew", "");
        return (~errNone);
    }
    MemSet(infoBuff, sizeof(NetHostInfoBufType), 0x00);

    // �z�X�g������IP�A�h���X���擾
    hostIP = NetLibGetHostByName(NNshGlobal->netRef,
                                 NNshGlobal->hostName,
                                 infoBuff,
                                 NNshGlobal->timeout, 
                                 &(NNshGlobal->err));
    if (hostIP == 0)
    {
        // �z�X�gIP�̎擾���s
        NNshGlobal->err = NNSH_ERRORCODE_FAILURECONNECT;
        FrmCustomAlert(ALTID_INFO,
                       "NNshNet_connect",  
                       "NetLibGetHostByName :",
                       NNshGlobal->hostName);
        goto FUNC_END;
    }

    // �\�P�b�gOPEN
    NNshGlobal->socketRef = NetLibSocketOpen(NNshGlobal->netRef,
                                             netSocketAddrINET, 
                                             netSocketTypeStream,
                                             netSocketProtoIPTCP,
                                             NNshGlobal->timeout,
                                             &(NNshGlobal->err));
    if (NNshGlobal->socketRef == -1)
    {
        // �\�P�b�gOPEN�ُ�
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "NetLibSocketOpen", "");
        NNshGlobal->socketRef = 0;
        NNshGlobal->err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

    // �\�P�b�g�R�l�N�g
    MemSet(&addrBuf, sizeof(NetSocketAddrType), 0x00);
    addrType = (NetSocketAddrINType *) &addrBuf;
    addrType->family = netSocketAddrINET;
    addrType->port   = NetHToNS(NNshGlobal->portNum);
    //addrType->addr   = infoBuff->address[0];
    MemMove(&(addrType->addr), *(hostIP->addrListP), hostIP->addrLen); 
    addrLen         = sizeof(NetSocketAddrType);
    NNshGlobal->err = errNone;
    NNshGlobal->ret = NetLibSocketConnect(NNshGlobal->netRef, 
                                          NNshGlobal->socketRef,
                                          (NetSocketAddrType *) &addrBuf,
                                          addrLen,
                                          NNshGlobal->timeout,
                                          &(NNshGlobal->err));
    if (NNshGlobal->ret == -1)
    {
        // �\�P�b�g�R�l�N�g�ُ�
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "NetLibSocketConnect", "");
        (void) NetLibSocketClose(NNshGlobal->netRef,
                                 NNshGlobal->socketRef,
                                 NNshGlobal->timeout,
                                 &(NNshGlobal->err));
        NNshGlobal->err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }
    NNshGlobal->err = errNone;

FUNC_END:
    // �o�b�t�@�̈���J��
    (void) MemPtrFree(infoBuff);

    return (NNshGlobal->err);
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
        *netRef = 0;
        return (ret);
    }

    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        // SSL���[�h�łȂ��A�܂�Ԃ�
        return (ret);
    }

    // SSL���C�u�����̃I�[�v��
    if (SysLibFind(kSslDBName, sslRef) != 0)
    {
        if (SysLibLoad(kSslLibType, kSslLibCreator, sslRef) != 0)
        {
            // ���[�h���s�A�G���[��������
            *sslRef = 0;
            return (~errNone);
        }
    }
    ret = SslLibOpen(*sslRef);
    if (ret != errNone)
    {
        // SSL���C�u�����I�[�v�����s�A�A�A
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // generic SSL context�̍쐬
    ret = SslLibCreate (*sslRef, template);
    if (ret != errNone)
    {
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
Err NNshSSL_connect(UInt16 sslMode, NNshWorkingInfo *NNshGlobal)
{
    Err ret;
    SslCallback verifyCallback;

    // TCP/IP�Ńz�X�g�ɐڑ�����
    ret = NNshNet_connect(NNshGlobal);
    if ((ret != errNone)||(sslMode == NNSH_SSLMODE_NOTSSL))
    {
        return (ret);
    }

    // Verify�R�[���o�b�N�֐���ݒ肷��
    MemSet(&verifyCallback, sizeof(SslCallback), 0x00);
    verifyCallback.callback = NNshSSL_callbackFunc;
    SslContextSet_VerifyCallback(NNshGlobal->sslRef,
                                 NNshGlobal->sslContext,
                                 &verifyCallback);

    // �\�P�b�g��Context�ɐݒ�A�z�X�g�ɐڑ�����
    //SslContextSet_Compat(sslRef, context, sslCompatAll);
    SslContextSet_Socket(NNshGlobal->sslRef,
                         NNshGlobal->sslContext,
                         NNshGlobal->socketRef);
    ret = SslOpen(NNshGlobal->sslRef, NNshGlobal->sslContext,
                  ((sslOpenModeSsl)|(sslOpenUseDefaultTimeout)), 0);
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
Err NNshSSL_write(UInt16 sslMode, NNshWorkingInfo *NNshGlobal)
{
    // SSL���[�h�łȂ���΁A�ʏ�̏������ݏ������s��
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        return (NNshNet_write(NNshGlobal));
    }

    // �f�[�^����������
    NNshGlobal->ret = SslWrite(NNshGlobal->sslRef,
                               NNshGlobal->sslContext,
                               NNshGlobal->sendBuf,
                               StrLen(NNshGlobal->sendBuf),
                               &(NNshGlobal->err));
    if (NNshGlobal->ret  != -1)
    {
        NNshGlobal->err = errNone;
    }
    return (NNshGlobal->err);
}

/*==========================================================================*/
/*  NNshSSL_read() : �f�[�^��M(SSL)                                        */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_read(UInt16 sslMode, NNshWorkingInfo *NNshGlobal)
{
    UInt16 length, copyLength;

    // SSL���[�h�łȂ���΁A�ʏ�̓ǂݏo���������s��
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        return (NNshNet_read(NNshGlobal));
    }

    length     = WORKBUFFER_DEFAULT - MARGIN;
    copyLength = 0;

    MemSet(NNshGlobal->workBuf, length, 0x00);

    // �f�[�^��(�I���܂�)�ǂݍ���
    while (1)
    {
        NNshGlobal->ret = SslRead(NNshGlobal->sslRef,
                                  NNshGlobal->sslContext,
                                  NNshGlobal->workBuf, 
                                  length,
                                  &(NNshGlobal->err));
        if (NNshGlobal->ret == -1)
        {
            return (NNshGlobal->err);
        }
        if (NNshGlobal->ret == 0)
        {
            return (errNone);
        }
        if (copyLength < (RECEIVEBUFFER_DEFAULT - MARGIN))
        {
            // �R�s�[����f�[�^(�Q�Ɖ\�ȃf�[�^)�́A�擪����L���o�b�t�@���̂�
            if ((NNshGlobal->ret + copyLength) < (RECEIVEBUFFER_DEFAULT - MARGIN))
            {
                MemMove(&NNshGlobal->recvBuf[copyLength],
                        NNshGlobal->workBuf, 
                        NNshGlobal->ret);
                copyLength = copyLength + NNshGlobal->ret;
            }
            else
            {
                MemMove(&NNshGlobal->recvBuf[copyLength],
                        NNshGlobal->workBuf,
                        (RECEIVEBUFFER_DEFAULT - MARGIN) - copyLength);
                copyLength = RECEIVEBUFFER_DEFAULT;
            }
        }
    }
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

/**----------------------------------------------------------------------**
 **   get_html_contents   : HTML�f�[�^����M����
 **
 **----------------------------------------------------------------------**/
static void get_html_contents(NNshWorkingInfo *NNshGlobal)
{
    // ���M�o�b�t�@�f�[�^���߂�
    if (NNshGlobal->clipboardInfo == NULL)
    {
        StrCopy(NNshGlobal->workBuf, " ");
    }
    else
    {
        MemSet(NNshGlobal->workBuf, (WORKBUFFER_DEFAULT), 0x00);
        if ((NNshGlobal->sendMethod & NNSHLOGIN_URLENCODE_DATA) != NNSHLOGIN_URLENCODE_DATA)
        {
            // ���M�f�[�^��URL�G���R�[�h���Ă߂�...
            StrNCopy_URLEncode(NNshGlobal->workBuf,
                               (WORKBUFFER_DEFAULT - MARGIN),
                               NNshGlobal->clipboardInfo,
                               StrLen(NNshGlobal->clipboardInfo));
        }
        else
        {
            // ���ł�URL�G���R�[�h����Ă���f�[�^�A���̂܂܂߂�...
            StrNCopy(NNshGlobal->workBuf,
                     NNshGlobal->clipboardInfo,
                     (WORKBUFFER_DEFAULT - MARGIN));
        }
    }

    // ���b�Z�[�W���쐬
    NNsh_MakeHTTPmsg(NNshGlobal);

    // �i���M�v���g�R���ɍ��킹�ājHTML�v�����b�Z�[�W�𑗂�
    if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) ==
                                                        NNSHLOGIN_SSL_PROTOCOL)
    {
        // https�ʐM��v��
        NNshHTTPS_comm(NNshGlobal);
    }
    else
    {
        // http�ʐM��v��
        NNshHTTP_comm(NNshGlobal);
    }
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
    StrCat(NNshGlobal->recvBuf, "\n\n---\n\n1st RECEIVE CONTENT\n\n");
    SendToLocalMemopad("RECEIVED DATA(1)", NNshGlobal->recvBuf);
#endif

    // ��M���b�Z�[�W�Ƀ��_�C���N�g�̎w�������邩�A�`�F�b�N���s��
    NNshGlobal->err = ~errNone - 9;
    if ((NNshGlobal->recvBuf[0]  == 'H')&&
        (NNshGlobal->recvBuf[1]  == 'T')&&
        (NNshGlobal->recvBuf[2]  == 'T')&&
        (NNshGlobal->recvBuf[3]  == 'P')&&
        (NNshGlobal->recvBuf[9]  == '3')&&
        (NNshGlobal->recvBuf[10] == '0')&&
        (NNshGlobal->recvBuf[11] == '2'))
    {
        // ���_�C���N�g�̃z�X�g���ƃT�C�g�𒊏o����
        NNsh_check302URL(NNshGlobal);
    }

    // ���_�C���N�g�̃A�h���X���擾�ł����Ƃ�...
REDIRECT_AGAIN:
    if (NNshGlobal->err == errNone)
    {
        // ���b�Z�[�W���쐬
        NNsh_MakeHTTPmsg(NNshGlobal);
        // ���_�C���N�g��Ɂi���M�v���g�R���ɍ��킹�āj����
        if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) ==
                                                        NNSHLOGIN_SSL_PROTOCOL)
        {
            // https�ʐM��v��
            NNshHTTPS_comm(NNshGlobal);
        }
        else
        {
            // http�ʐM��v��
            NNshHTTP_comm(NNshGlobal);
        }
        if (NNshGlobal->err != errNone)
        {
#ifdef DEBUG
            // �ʐM���s�A�G���[��������
            FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", " ", "(redirect)");
#endif
            return;
        }
    }

    // �ēx�y�[�W���J���Ȃ��Ă͂Ȃ�Ȃ��ꍇ...
    NNsh_checkRefreshConnectionRequest(NNshGlobal);
    if (NNshGlobal->err == errNone)
    {
#ifdef DEBUG
        // �ēx���_�C���N�g���邱�Ƃ𑗐M����
        FrmCustomAlert(ALTID_INFO, 
                       "REDIRECT AGAIN...(", NNshGlobal->reqAddress, ")");
#endif
        goto REDIRECT_AGAIN;
    }

#ifdef DEBUG
    // �f�o�b�O�p�Ɏ�M�f�[�^���������֏o�͂���
    StrCat(NNshGlobal->recvBuf, "\n\n---\n\nRECEIVE CONTENT\n\n");
    SendToLocalMemopad("RECEIVED DATA", NNshGlobal->recvBuf);
#endif

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
void NNsh_check302URL(NNshWorkingInfo *NNshGlobal)
{
    NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "http");
    if (NNshGlobal->tempPtr == NULL)
    {
        // Redirect�z�X�g���̓ǂݏo�����s
        NNshGlobal->err = ~errNone;
        return;
    }

#ifdef USE_REFERER
    // ���_�C���N�g���ꂽ�A�h���X��Referer�ɐݒ肷��
    MemSet(NNshGlobal->referer, BUFSIZE, 0x00);
    for (NNshGlobal->loopCount = 0; 
         NNshGlobal->loopCount < (BUFSIZE - 1); 
         (NNshGlobal->loopCount)++)
    {
        if (NNshGlobal->tempPtr[(NNshGlobal->loopCount)] <= ' ')
        {
            // URL�̖����ɓ��B(���Ԃ�A�{���� \r �Ŕ�����...)
            break;
        }
        NNshGlobal->referer[(NNshGlobal->loopCount)] =
                                  NNshGlobal->tempPtr[(NNshGlobal->loopCount)];
    }
#endif

    // �v���g�R���̃`�F�b�N
    NNshGlobal->sendMethod = 0;
    if (NNshGlobal->tempPtr[4] == 's')
    {
        // https (SSL)
        NNshGlobal->sendMethod = NNSHLOGIN_SSL_PROTOCOL;
    }
    NNshGlobal->tempPtr = StrStr(NNshGlobal->tempPtr, "://");
    if (NNshGlobal->tempPtr == NULL)
    {
        // �z�X�g�����o�Ɏ��s
        NNshGlobal->err = ~errNone + 100;
        return;
    }
    NNshGlobal->tempPtr = NNshGlobal->tempPtr + 3;  // 3�́AStrLen("://");

    // �z�X�g���̃R�s�[
    MemSet(NNshGlobal->hostName, BUFSIZE, 0x00);
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
            NNshGlobal->portNum = 0;
            while ((*(NNshGlobal->tempPtr) >= '0')&&
                   (*(NNshGlobal->tempPtr) <= '9'))
            {
                NNshGlobal->portNum = NNshGlobal->portNum * 10 + 
                                         (*(NNshGlobal->tempPtr) - '0');
                (NNshGlobal->tempPtr)++;
            }
            //(NNshGlobal->tempPtr)++;  // /����͂��߂�...
            break;
        }
        NNshGlobal->hostName[NNshGlobal->loopCount] = *(NNshGlobal->tempPtr);
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
    FrmCustomAlert(ALTID_INFO, NNshGlobal->hostName, ":", NNshGlobal->reqAddress);
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
static void NNsh_checkRefreshConnectionRequest(NNshWorkingInfo *NNshGlobal)
{
    Char   *ptr;

    ///////////// ���t���b�V���w��������H //////////
    NNshGlobal->err = ~errNone;
    ptr = StrStr(NNshGlobal->recvBuf, "http-equiv=\"Refresh\"");
    if (ptr == NULL)
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
    MemSet(NNshGlobal->hostName, BUFSIZE, 0x00);
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
            NNshGlobal->portNum = 0;
            while ((*ptr >= '0')&&(*ptr <= '9'))
            {
                NNshGlobal->portNum = NNshGlobal->portNum * 10 + (*ptr - '0');
                ptr++;
            }
            break;
        }
        NNshGlobal->hostName[NNshGlobal->isChecking] = *ptr;
        ptr++;
    }
GET_CONTENT_NAME:
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
void NNsh_MakeHTTPmsg(NNshWorkingInfo *NNshGlobal)
{
#ifdef USE_RESOURCE
    MemHandle  strH;
#endif

    MemSet(NNshGlobal->sendBuf, (SENDBUFFER_DEFAULT), 0x00);

    // �擾���\�b�h������
    switch ((NNshGlobal->sendMethod)&(NNSHLOGIN_METHOD_MASK))
    {
      case NNSHLOGIN_POST_METHOD:
        StrCopy(NNshGlobal->sendBuf, "POST ");
        break;
      
      case NNSHLOGIN_GET_METHOD:
      default:
        StrCopy(NNshGlobal->sendBuf, "GET ");
        break;
    }

    // �|�[�g�ԍ�������
    switch ((NNshGlobal->sendMethod)&(NNSHLOGIN_PORTNUM_MASK))
    {
      case NNSHLOGIN_SSL_PROTOCOL:
        NNshGlobal->portNum = 443;
        break;

      case NNSHLOGIN_CONNECT_PORT:
        // port�ԍ��͕ʓr�w�肳��Ă���...
        break;

      default:
        NNshGlobal->portNum = 80;
        break;
    }     

    if (NNshGlobal->reqAddress[0] != '/')
    {
        // ��΃p�X�Ŏw������΂Ȃ��ꍇ�̕⑫...
        StrCat(NNshGlobal->sendBuf, "/");
    }
    StrCat(NNshGlobal->sendBuf, NNshGlobal->reqAddress);
    StrCat(NNshGlobal->sendBuf, " HTTP/1.0\r\nHost: ");
    StrCat(NNshGlobal->sendBuf, NNshGlobal->hostName);

    // referer���ݒ肳��Ă����ꍇ�ɂ͕t������
#ifdef USE_REFERER
    if (StrLen(NNshGlobal->referer) != 0)
    {
        StrCat(NNshGlobal->sendBuf, "\r\nReferer: ");
        StrCat(NNshGlobal->sendBuf, NNshGlobal->referer);
    }
#endif

    //  �{�f�B�����Ȃ��ꍇ(��萳�m�ɂ́A1�������M���悤�Ƃ����ꍇ)�ɂ́A
    // Content-Length, Content-Type�����Ȃ�
    if (StrLen(NNshGlobal->workBuf) > 1)
    {
        StrCat(NNshGlobal->sendBuf, "\r\nContent-Length: ");
        StrIToA(&NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)], 
                                      StrLen(NNshGlobal->workBuf));
#ifdef USE_FORM_SEND
        //  1�����ȏ�f�[�^�����݂����ꍇ�ɂ́AFORM��URL�G���R�[�h�������e����
        // �����đ��M����
        StrCat(NNshGlobal->sendBuf, "\r\nContent-Type: application/x-www-form-urlencoded");
#endif
    }

#if 0   // ��M�f�[�^�͂��ׂāA�\���͐擪�݂̂Ƃ���...
    // ���M�f�[�^�T�C�Y�����肳����
    StrCat(NNshGlobal->sendBuf, "\r\nRange: bytes=0-");
    StrIToA(&(NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)]),
            (RECEIVEBUFFER_DEFAULT - HEADER_BUFFER));
#endif

    // Cookie��ǉ�
    if (NNshGlobal->cookie[0] != '\0')
    {
        StrCat(NNshGlobal->sendBuf, "\r\n");
        StrCat(NNshGlobal->sendBuf, NNshGlobal->cookie);
    }

    // �w�b�_�̎c�蕔����t��
    StrCat(NNshGlobal->sendBuf, "\r\nAccept: text/html, */*\r\nUser-Agent: WebDA/1.01 (PalmOS; NNsi family) [ja]\r\nAccept-Encoding: identity\r\nAccept-Language: ja, en\r\nConnection: close\r\nPragma: no-cache\r\n\r\n");

    //  �{�f�B��������ꍇ(��萳�m�ɂ́A2�����ȏ㑗�M���悤�Ƃ����ꍇ)�A�A�A
    if (StrLen(NNshGlobal->workBuf) > 1)
    {
        // �f�[�^�{�f�B������ǉ�����
        StrCat(NNshGlobal->sendBuf, NNshGlobal->workBuf);
    }
    return;
}

/**----------------------------------------------------------------------**
 **    HTTPS�ʐM(SSL)�����{
 **                                NNshGlobal->sendBuf �F ���M�f�[�^
 **                                NNshGlobal->recvBuf �F ��M�f�[�^
 **
 **                  �� NNshGlobal->err �ɉ����R�[�h���i�[���ďI������ ��
 **----------------------------------------------------------------------**/
void NNshHTTPS_comm(NNshWorkingInfo *NNshGlobal)
{
    // ��M�o�b�t�@�̑|��
    MemSet(NNshGlobal->recvBuf, RECEIVEBUFFER_DEFAULT, 0x00);

    // PalmOS v5.20�����������ꍇ�ɂ͋N�����Ȃ�
    if (NNshGlobal->palmOSVersion < 0x05200000)
    {
        FrmCustomAlert(ALTID_INFO, "a https-request is need a SslLib."
                       "(You should USE a PalmOS 5.2 or later).","","[webDA]");
        return;
    }

    // SSL�̃I�[�v��
    NNshGlobal->err = 
          NNshSSL_open(NNSH_SSLMODE_DOSSL,    &(NNshGlobal->netRef), 
                       &(NNshGlobal->sslRef), &(NNshGlobal->sslTemplate),
                       &(NNshGlobal->sslContext));
    if (NNshGlobal->err != errNone)
    {
        // �ڑ��Ɏ��s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_open", " ", NNshGlobal->hostName);
#endif
        return;
    }

    // SSL�̃R�l�N�g
    NNshGlobal->err = NNshSSL_connect(NNSH_SSLMODE_DOSSL, NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
        // �ڑ��Ɏ��s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_connect", " ", NNshGlobal->hostName);
#endif
        goto HTTPS_CLOSE;
    }

    // �v���̑��M
    NNshGlobal->err = NNshSSL_write(NNSH_SSLMODE_DOSSL, NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
        // �ڑ��Ɏ��s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_write", " ", NNshGlobal->hostName);
#endif
        goto HTTPS_DISCONNECT;
    }

    // �ǂݍ���
    NNshGlobal->err = NNshSSL_read(NNSH_SSLMODE_DOSSL, NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
        // �ڑ��Ɏ��s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_read", " ", NNshGlobal->hostName);
#endif
        goto HTTPS_DISCONNECT;
    }

HTTPS_DISCONNECT:
    // �ڑ���؂�
    NNshSSL_disconnect(NNSH_SSLMODE_DOSSL, NNshGlobal->sslRef,
                       NNshGlobal->sslContext,  NNshGlobal->socketRef,
                       NNshGlobal->netRef,      NNshGlobal->timeout);
HTTPS_CLOSE:
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
void NNshHTTP_comm(NNshWorkingInfo *NNshGlobal)
{
    // ��M�o�b�t�@�̑|��
    MemSet(NNshGlobal->recvBuf, RECEIVEBUFFER_DEFAULT, 0x00);

    // �l�b�g���C�u����OPEN
    NNshGlobal->err = NNshNet_open(&(NNshGlobal->netRef));
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Net_open", " ", NNshGlobal->hostName);
#endif
        return;
    }
    // �R�l�N�g
    NNshGlobal->err = NNshNet_connect(NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Net_connect", " ", NNshGlobal->hostName);
#endif
        goto HTTP_CLOSE;
    }
    SetMsg_BusyForm("Sending a request...");

    // ��������
    NNshGlobal->err = NNshNet_write(NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "NNshNet_write",  "", "");
#endif
        goto HTTP_CLOSE;
    }
    SetMsg_BusyForm("Receiving a message...");

    // �ǂݏo��
    NNshGlobal->err = NNshNet_read(NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
        // �ǂݏo�����s
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "NNshNet_read",  "", "");
#endif
        goto HTTP_DISCONNECT;
    }

    SetMsg_BusyForm("Disconnecting...");

HTTP_DISCONNECT:
    // �\�P�b�g�ؒf
    NNshNet_disconnect(NNshGlobal->netRef, NNshGlobal->socketRef, 
                                                        NNshGlobal->timeout);
HTTP_CLOSE:
    NNshNet_close(NNshGlobal->netRef);

    return;
}

/**************************************************************************/
/*   Cookie���E��                                                         */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
void pickup_cookie(NNshWorkingInfo *NNshGlobal)
{
    Char *ptr;

    MemSet(NNshGlobal->cookie, (BUFSIZE + MARGIN), 0x00);
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
/*   Function : TrimingMessage                                             */
/*                                          (���b�Z�[�W�̐��`����:2�p�X��) */
/*                                          �� ���䕶���̘A�����폜���� �� */
/*                    buf : �f�[�^�̈�                                     */
/*                   size : �o�b�t�@�T�C�Y                                 */
/*-------------------------------------------------------------------------*/
static void TrimingMessage(Char *buf, UInt32 bufSize)
{



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
static void ParseMessage(Char *buf, Char *src, UInt32 dstSize, UInt32 size, UInt16 kanjiCode)
{
    Boolean kanjiMode;
    UInt16  len;
    Char   *ptr, *dst, *keepP, dataStatus;

    dataStatus = MSGSTATUS_NAME;
    kanjiMode = false;

    // ���x�����߃��[�`������... (�P�����Âp�[�X����)
    keepP = NULL;
    dst   = buf;
    ptr   = src;

    while (ptr < (src + size))
    {
        // �R�s�[��o�b�t�@���s����������������I������
        if ((dst + MARGIN) > (buf + dstSize))
        {
            break;
        }

        // JIS�����R�[�h�̕ϊ����W�b�N
        if (kanjiCode == NNSH_KANJICODE_JIS)
        {
            // JIS��SHIFT JIS�̃R�[�h�ϊ�
            if ((*ptr == '\x1b')&&
                (*(ptr + 1) == '\x24')&&
                (*(ptr + 2) == '\x42'))
            {
                // �������[�h�ɐ؂�ւ�
                kanjiMode = true;
                ptr = ptr + 3;
                continue;
            }
            if ((*ptr == '\x1b')&&
                (*(ptr + 1) == '\x28')&&
                (*(ptr + 2) == '\x42'))
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

            //  "<SCRIPT>�`</SCRIPT>" �͕\�����Ȃ�(�ǂݔ�΂�)
            if ((*(ptr + 1) == 'S')&&(*(ptr + 2) == 'C')&&
                (*(ptr + 3) == 'R')&&(*(ptr + 4) == 'I')&&
                (*(ptr + 5) == 'P')&&(*(ptr + 6) == 'T'))
            {
                ptr = ptr + 7;
                keepP = ptr; 
                while (*ptr != '\0')
                {
                    if ((*(ptr + 0) == '<')&&(*(ptr + 1) == '/')&&
                        (*(ptr + 2) == 'S')&&(*(ptr + 3) == 'C')&&
                        (*(ptr + 4) == 'R')&&(*(ptr + 5) == 'I')&&
                        (*(ptr + 6) == 'P')&&(*(ptr + 7) == 'T'))
                    {
                        // �X�N���v�g�^�O�̖����𔭌��A�^�O�ǂݔ�΂����[�h��
                        goto SKIP_TAG;
                    }
                    ptr++;
                }
                // </SCRIPT>�̖�����������Ȃ������A�A�A�^�O�ʒu��߂�..
                ptr = keepP;                
                // continue�͂Ȃ��B�B (�^�O�͓ǂݔ�΂�����)
                goto SKIP_TAG;
            }

            //  "<script>�`</script>" �͕\�����Ȃ�(�ǂݔ�΂�)
            if ((*(ptr + 1) == 's')&&(*(ptr + 2) == 'c')&&
                (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'i')&&
                (*(ptr + 5) == 'p')&&(*(ptr + 6) == 't'))
            {
                ptr = ptr + 7;
                keepP = ptr; 
                while (*ptr != '\0')
                {
                    if ((*(ptr + 0) == '<')&&(*(ptr + 1) == '/')&&
                        (*(ptr + 2) == 's')&&(*(ptr + 3) == 'c')&&
                        (*(ptr + 4) == 'r')&&(*(ptr + 5) == 'i')&&
                        (*(ptr + 6) == 'p')&&(*(ptr + 7) == 't'))
                    {
                        // �X�N���v�g�^�O�̖����𔭌��A�^�O�ǂݔ�΂����[�h��
                        goto SKIP_TAG;
                    }
                    ptr++;
                }
                // </script>�̖�����������Ȃ������A�A�A�^�O�ʒu��߂�..
                ptr = keepP;                
                // continue�͂Ȃ��B�B (�^�O�͓ǂݔ�΂�����)
                goto SKIP_TAG;
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
 *   AMsoft�����UTF8�ϊ��e�[�u���𗘗p����UTF8�ϊ�����������
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

/*#------------------------------------------------------------------------#*/
/*#   analysis_URL (URL�̉��)                                             #*/
/*#                                      true: ��͐���  false: ��͎��s   #*/
/*#------------------------------------------------------------------------#*/
static Boolean analysis_URL(NNshWorkingInfo *NNshGlobal)
{
    UInt16  len;
    UInt8  *ptr;

    NNshGlobal->portNum = 80;
    len = sizeof("ttp://") - 1;
    ptr = StrStr(NNshGlobal->reqURL, "ttp://");
    if (ptr == NULL)
    {
        ptr = StrStr(NNshGlobal->reqURL, "ttps://");
        if (ptr == NULL)
        {
            return (false);
    }
        // SSL�v���g�R��������...
        len = sizeof("ttps://") - 1;
        NNshGlobal->portNum = 443;
        NNshGlobal->sendMethod
                           = (NNshGlobal->sendMethod)|(NNSHLOGIN_SSL_PROTOCOL);

        // PalmOS v5.20�����������ꍇ�ɂ�NG����(SslLib���K�v�Ȃ̂�)
        if (NNshGlobal->palmOSVersion < 0x05200000)
        {
            FrmCustomAlert(ALTID_INFO, "a https-request is need a SslLib."
                           "(You should USE a PalmOS 5.2 or later).","",
                           "[webDA]");
            return (false);
        }
    }
    ptr = ptr + len;

    // HOST�����o
    len = 0;
    MemSet(NNshGlobal->hostName, BUFSIZE, 0x00);
    while ((*ptr != '/')&&(*ptr != ':')&&(*ptr != '\0'))
    {
        NNshGlobal->hostName[len] = *ptr;
        len++;
        ptr++;
    }
    NNshGlobal->hostName[len] = '\0';

    // port�ԍ����o
    if (*ptr == ':')
    {
        NNshGlobal->portNum = 0;
        while ((*ptr >= '0')&&
               (*ptr <= '9'))
        {
            NNshGlobal->portNum = NNshGlobal->portNum * 10 + (*ptr - '0');
            ptr++;
        }
        NNshGlobal->sendMethod
                        = (NNshGlobal->sendMethod) | (NNSHLOGIN_CONNECT_PORT);
    }

    // �v���A�h���X���o
    if ((*ptr == '/')&&(*ptr == '/'))
    {
        ptr++;
    }

    len = 0;
    while ((*ptr != '>')&&(*ptr > ' ')&&(*ptr < 0x80))
    {
        NNshGlobal->reqAddress[len] = *ptr;
        len++;
        ptr++;
    }
    NNshGlobal->reqAddress[len] = '\0';
    if (len == 0)
    {
        NNshGlobal->reqAddress[len] = '/';
        len++;
        NNshGlobal->reqAddress[len] = '\0';
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   CheckClipboard                                           */
/*                                                   �N���b�v�{�[�h�̊m�F  */
/*-------------------------------------------------------------------------*/
static void CheckClipboard(NNshWorkingInfo *adtP)
{
    MemHandle  memH;
    Char      *areaP, *ptr, *ptr2;

    // �A�g�w��������������
    adtP->sendMethod = NNSHLOGIN_METHOD_NOTSPECIFY;

    adtP->fieldLen  = 0;
    memH = ClipboardGetItem(clipboardText, &adtP->fieldLen);
    if ((memH == 0)||(adtP->fieldLen == 0))
    {
        // �N���b�v�{�[�h�Ƀf�[�^���Ȃ��A�I������
        return;
    }
    areaP = MemHandleLock(memH);
    if (areaP == NULL)
    {
        // �N���b�v�{�[�h�̃|�C���^�擾���s�A�I������
        return;
    }

    ////////////////////////////////////////// NNsi�Ƃ̘A�g���� ��������
    if (StrNCompare(areaP, NNSIEXT_POSTSTART, StrLen(NNSIEXT_POSTSTART)) == 0)
    {
        //  POST���\�b�h�̑��M�w��������
        adtP->sendMethod = NNSHLOGIN_POST_METHOD;
        goto ANALYSIS_STRING;
    }
    if (StrNCompare(areaP, NNSIEXT_GETSTART, StrLen(NNSIEXT_GETSTART)) == 0)
    {
        //  GET���\�b�h�̑��M�w��������
        adtP->sendMethod = NNSHLOGIN_GET_METHOD;
        goto ANALYSIS_STRING;
    }
    if (StrNCompare(areaP, NNSIEXT_BINGETSTART, StrLen(NNSIEXT_BINGETSTART)) == 0)
    {
        //  GET���\�b�h�̑��M�w��������(BINARY��M)
        adtP->sendMethod = NNSHLOGIN_GET_METHOD;
        goto ANALYSIS_STRING;
    }
    if (StrNCompare(areaP, NNSIEXT_URLENCODE_POSTSTART, StrLen(NNSIEXT_URLENCODE_POSTSTART)) == 0)
    {
        //  POST���\�b�h�̑��M�w��������(URL�G���R�[�h�ς݃f�[�^)
        adtP->sendMethod = ((NNSHLOGIN_POST_METHOD)|(NNSHLOGIN_URLENCODE_DATA));
        goto ANALYSIS_STRING;
    }
    if (StrNCompare(areaP, NNSIEXT_URLENCODE_GETSTART, StrLen(NNSIEXT_URLENCODE_GETSTART)) == 0)
    {
        //  GET���\�b�h�̑��M�w��������(URL�G���R�[�h�ς݃f�[�^)
        adtP->sendMethod = ((NNSHLOGIN_GET_METHOD)|(NNSHLOGIN_URLENCODE_DATA));
        goto ANALYSIS_STRING;
    }

    // ��L�ȊO�̏ꍇ�A�R�}���h�w��Ȃ�(�֐��𔲂���)
    goto FUNC_END;

ANALYSIS_STRING:
    // �f�[�^������̐擪������
    ptr = StrStr(areaP, NNSIEXT_INFONAME);
    if ((ptr == NULL)||(ptr > (areaP + adtP->fieldLen)))
    {
        // �f�[�^������̐擪�̌��o�Ɏ��s�A�I������
        adtP->sendMethod = NNSHLOGIN_METHOD_NOTSPECIFY;
        goto FUNC_END;
    }

    // �f�[�^�̐擪�Ɉړ�����
    ptr = ptr + StrLen(NNSIEXT_INFONAME);

    // �f�[�^(GET/POST���\�b�h�̃f�[�^�̈�)�̃R�s�[...
    adtP->loopCount = 0;
    while ((adtP->loopCount < (BUFSIZE * 4))&&(ptr[adtP->loopCount] != '\0')&&(ptr[adtP->loopCount] != '<'))
    {
        (adtP->loopCount)++;
    }
    if (adtP->loopCount != 0)
    {
        // �̈���m�ۂ��ăR�s�[����
        adtP->clipboardInfo = MemPtrNew(adtP->loopCount + MARGIN);
        if (adtP->clipboardInfo != NULL)
        {
            MemSet (adtP->clipboardInfo, (adtP->loopCount + MARGIN), 0x00);
            MemMove(adtP->clipboardInfo, ptr, adtP->loopCount);
        }       
    }

    // �f�[�^�̐擪(URL�̐擪)���擾����    
    ptr = StrStr(areaP, NNSIEXT_DATANAME);
    if (ptr != NULL)
    {
        ptr = ptr + sizeof(NNSIEXT_DATANAME) - 1;
    }
    
    // �f�[�^�̖���(URL�̖���)���擾����
    ptr2 = StrStr(areaP, NNSIEXT_ENDDATANAME);
    if (ptr2 == NULL)
    {
        // �f�[�^�̖������Ȃ�����... 
        // (�N���b�v�{�[�h�̖������f�[�^�̖����Ƃ���)      
        ptr2 = areaP + adtP->fieldLen;
    }
    if ((ptr2 - ptr) > (BUFSIZE * 2 - MARGIN))
    {
        adtP->loopCount = BUFSIZE * 2 - MARGIN;       
    }
    else
    {
        adtP->loopCount =  ptr2 - ptr;
    }

    // URL�f�[�^�̃R�s�[�����s
    MemSet (adtP->reqURL, (BUFSIZE * 2), 0x00);
    MemMove(adtP->reqURL, ptr, (ptr2 - ptr));

    ////////////////////////////////////////// NNsi�Ƃ̘A�g���� �����܂�

FUNC_END:
    // �N���b�v�{�[�h�̈���J������
    if (memH != 0)
    {
        MemHandleUnlock(memH);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////
/////   Capture DA���t�B�[�h�o�b�N...
//////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------------*/
/*   Function :  checkDisplayMode                                          */
/*                           ��ʃ��[�h(PalmOS5���𑜓x�T�|�[�g)�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
Boolean checkDisplayMode(NNshWorkingInfo *adtP)
{
#ifdef USE_WIDEWINDOW
    UInt32 version;

    // �𑜓x�̏����f�[�^���i�[����
    adtP->os5density = (UInt32) kDensityLow;

    // ���𑜓x�t�B�[�`���[�̎擾
    if (FtrGet(sysFtrCreator, sysFtrNumWinVersion, &(version)) != 0)
    {
        // ���𑜓x�t�B�[�`���[�̃T�|�[�g�Ȃ�
        return (false);
    }
    if (version < 4)
    {
        //�@���𑜓x�̃T�|�[�g���Ȃ�...
        return (false);
    }

    // Window�̉𑜓x���擾����
    WinScreenGetAttribute(winScreenDensity, &(adtP->os5density));

    // �Ƃ肠�����A�W���𑜓x�ɂ��Ă���
    WinSetCoordinateSystem(kCoordinatesStandard);

    return (true);
#else  // #ifdef USE_WIDEWINDOW
    return (false);
#endif  // #ifdef USE_WIDEWINDOW
}

/*-------------------------------------------------------------------------*/
/*   Function :  resizeDialogWindow                                        */
/*                                        �_�C�A���O�E�B���h�E�T�C�Y�̒��� */
/*-------------------------------------------------------------------------*/
void resizeDialogWindow(FormType *diagFrm, NNshWorkingInfo *adtP)
{
#ifdef USE_WIDEWINDOW
    UInt16         objIndex;

    /* �S�̂�Window�T�C�Y�擾 */
    WinGetDisplayExtent(&(adtP->x), &(adtP->y));

    /* dialog�̃T�C�Y�擾 */
    FrmGetFormBounds(diagFrm, &(adtP->r));

    /* �E�B���h�E�̊g��T�C�Y�����߂� */
    (adtP->r).topLeft.x = 2;
    (adtP->r).topLeft.y = 3;

    (adtP->diffX)       = (adtP->r).extent.x;
    (adtP->diffY)       = (adtP->r).extent.y;
    (adtP->r).extent.y  = (adtP->y) - 6;
    (adtP->r).extent.x  = (adtP->x) - 4;
    
    /* �I�u�W�F�N�g�̈ړ��T�C�Y�����߂� */
    (adtP->diffX) = (adtP->r).extent.x - (adtP->diffX);
    (adtP->diffY) = (adtP->r).extent.y - (adtP->diffY);

    /* dialog�̊g�� */
    WinSetWindowBounds(FrmGetWindowHandle(diagFrm), &(adtP->r));

    /* OK �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DIALOG_OK);
    FrmGetObjectBounds(diagFrm, objIndex, &(adtP->r));
    // (adtP->r).topLeft.x = (adtP->r).topLeft.x + adtP->diffX;
    (adtP->r).topLeft.y = (adtP->r).topLeft.y + adtP->diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &(adtP->r));

    /* REDRAW �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DIALOG_REDRAW);
    FrmGetObjectBounds(diagFrm, objIndex, &(adtP->r));
    (adtP->r).topLeft.x = (adtP->r).topLeft.x + adtP->diffX;
    (adtP->r).topLeft.y = (adtP->r).topLeft.y + adtP->diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &(adtP->r));

    /* �� �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_UP);
    FrmGetObjectBounds(diagFrm, objIndex, &(adtP->r));
    (adtP->r).topLeft.x = (adtP->r).topLeft.x + adtP->diffX;
    (adtP->r).topLeft.y = (adtP->r).topLeft.y + adtP->diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &(adtP->r));

    /* �� �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DOWN);
    FrmGetObjectBounds(diagFrm, objIndex, &(adtP->r));
    (adtP->r).topLeft.x = (adtP->r).topLeft.x + adtP->diffX;
    (adtP->r).topLeft.y = (adtP->r).topLeft.y + adtP->diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &(adtP->r));

    /* gadget(�`��̈�)�̊g�� */
    objIndex = FrmGetObjectIndex(diagFrm, FLDID_INFOFIELD);
    FrmGetObjectBounds(diagFrm, objIndex, &(adtP->r));
    (adtP->r).extent.x = (adtP->r).extent.x + (adtP->diffX);
    (adtP->r).extent.y = (adtP->r).extent.y + (adtP->diffY);
    FrmSetObjectBounds(diagFrm, objIndex, &(adtP->r));

#endif  // #ifdef USE_WIDEWINDOW
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  createDialogWindow                                        */
/*                                 DIA�T�|�[�g�̃`�F�b�N�ƃ_�C�A���O�̐��� */
/*-------------------------------------------------------------------------*/
Boolean createDialogWindow(FormType **diagFrm, NNshWorkingInfo *adtP)
{
#ifdef USE_WIDEWINDOW
    UInt16 status;

    // DIA�T�|�[�g�����邩�ǂ����m�F����
    status = FtrGet(pinCreator, pinFtrAPIVersion, &(adtP->silkVer));
    if (status != errNone)
    {
        // DIA ��T�|�[�g�f�o�C�X�A�_�C�A���O�T�C�Y�̊g��͂��Ȃ�
        goto NOT_SUPPORT_DIA;
    }

    // DIA�T�|�[�g�f�o�C�X �A�V���N�����I�ɓ���\���m�F����
    status = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &(adtP->silkVer));
    if ((status != errNone)|
        (((adtP->silkVer) & grfFtrInputAreaFlagDynamic) == 0)||
        (((adtP->silkVer) & grfFtrInputAreaFlagCollapsible) == 0))
    {
        // �G���[�ɂȂ�ꍇ�A�������͓��I�V���N��T�|�[�g�̏ꍇ�A��������������
        goto NOT_SUPPORT_DIA;
    }

    // ���݂̃V���N��Ԃ��L������
    status = PINGetInputAreaState();

    // �_�C�A���O�̃I�[�v��
    *diagFrm = FrmInitForm(FRMID_RESULT);
    FrmSetActiveForm(*diagFrm);

    // �V���N�����L���ɂ���
    FrmSetDIAPolicyAttr(*diagFrm, frmDIAPolicyCustom);
    PINSetInputTriggerState(pinInputTriggerEnabled);

    // ���炢�x�^�A�A�A�E�B���h�E�T�C�Y�̍ő�l�ŏ��l��ݒ�
    WinSetConstraintsSize(FrmGetWindowHandle(*diagFrm),
                          160, 225, 225, 160, 225, 225);

    // �V���N�̈��ۑ����Ă������̂ɂ���
    PINSetInputAreaState(status);

    // �V���N����ɐ����I
    return (true);

NOT_SUPPORT_DIA:
#endif // #ifdef USE_WIDEWINDOW
    // �V���N�����T�|�[�g�̏ꍇ...
    *diagFrm = FrmInitForm(FRMID_RESULT);
    FrmSetActiveForm(*diagFrm);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :  setDisp_webDA                                             */
/*                                                        ��ʕ`��̑O���� */
/*-------------------------------------------------------------------------*/
void setDisp_webDA(FormType *frm, NNshWorkingInfo *adtP)
{
    // draw���Ȃ��ƁAWindow��ɗ̈悪�\������Ȃ��A�A�A
    FrmDrawForm(frm);

    // �K�W�F�b�g�̗̈�T�C�Y���擾����
    FrmGetObjectBounds(frm,FrmGetObjectIndex(frm, FLDID_INFOFIELD),
                                                               &(adtP->dimF));

    // �𑜓x���[�h���m�F���A�̈���N���A����(area�Ɏ��𑜓x���i�[)
    switch (adtP->os5density)
    {
      case kDensityDouble:
        // 320x320
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 2;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 2;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2 + 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityTriple:
        // 480x480
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 3;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 3;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 3;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 3 + 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityQuadruple:
        // 640x640
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 4;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 4;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 4;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 4 + 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityOneAndAHalf:
        // 240x240
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2 / 3;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 2 / 3;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 2 / 3;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2 / 3 + 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityLow:
      default:
        // 160x160
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x;
        adtP->area.extent.x   = adtP->dimF.extent.x;
        adtP->area.extent.y   = adtP->dimF.extent.y;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y;
        break;
    }

    // �`��̈�̃N���A
    WinEraseRectangle(&(adtP->area), 0);
}

/*-------------------------------------------------------------------------*/
/*   Function :  resetDisp_webDA                                           */
/*                                                        ��ʕ`��̌㏈�� */
/*-------------------------------------------------------------------------*/
void resetDisp_webDA(FormType *frm, NNshWorkingInfo *adtP)
{

    // ���𑜓x�̐ݒ������
    switch (adtP->os5density)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()), adtP->os5density);
        WinSetCoordinateSystem(kCoordinatesStandard);
        break;

      case kDensityLow:
      default:
        // �Ȃɂ����Ȃ�...
        break;
    }
    return;
}

/*==========================================================================*/
/*  StrNCopy_URLEncode() : �������URL�G���R�[�h�R�s�[                      */
/*                                                                          */
/*==========================================================================*/
void StrNCopy_URLEncode(UInt8 *dst, UInt32 bufSize, UInt8 *src, UInt32 size)
{
    UInt8  *locP, *data, hexBuf[20];
    UInt32  num;

    data = dst;
    locP = src;
    while ((*locP != '\0')&&(locP < (src + size))&&(data < (dst + bufSize)))
    {
        // �X�y�[�X�́{�ɕϊ�
        if (*locP == ' ')
        {
            *data = '+';
            data++;
            locP++;
            continue;
        }

        // ���s�́ACR + LF�ɕϊ�
        if (*locP == '\n')
        {
            StrCopy(data, "%0D%0A");
            data = data + 6; // 6 == StrLen("%0D%0A");
            locP++;
            continue;
        }

        // ���̂܂܏o��(���̂P)
        if ((*locP == '.')||(*locP == '_')||(*locP == '-')||(*locP == '*'))
        {
            *data = *locP;
            data++;
            locP++;
            continue;
        }

        // ���̂܂܏o��(���̂Q)
        if (((*locP >= '0')&&(*locP <= '9'))||
            ((*locP >= 'A')&&(*locP <= 'Z'))||
            ((*locP >= 'a')&&(*locP <= 'z')))
        {
            *data = *locP;
            data++;
            locP++;
            continue;
        }

        // ��̏����̂ǂ�ɂ����Ă͂܂�Ȃ��ꍇ�ɂ́A�R���o�[�g����B
        *data = '%';
        data++;
        
        num = (UInt32) *locP;
        MemSet(hexBuf, sizeof(hexBuf), 0x00);
        StrIToH(hexBuf, num);

        // ���l�̉��Q�����~�����̂�...
        StrCat(data, &hexBuf[6]);
        data = data + 2; // 2 == StrLen(data);
        locP++;
    }
    return;
}

/*!
 * ------------------------------------------------------------
 *    POST���\�b�h�̑��M
 * ------------------------------------------------------------
 *    <NNsi:NNsiExt type=WEBPOST">
 *       <NNsi:Info>(POST�f�[�^...)</NNsi:Info>
 *       <NNsi:Data>(URL�A�h���X...)</NNsi:Data>
 *    </NNsi:NNsiExt>
 * ------------------------------------------------------------
 * 
 * ------------------------------------------------------------
 *    GET���\�b�h�̑��M
 * ------------------------------------------------------------
 *    <NNsi:NNsiExt type=WEBGET">
 *       <NNsi:Info>(GET�f�[�^...)</NNsi:Info>
 *       <NNsi:Data>(URL�A�h���X...)</NNsi:Data>
 *    </NNsi:NNsiExt>
 * ------------------------------------------------------------
 *
 *
 */
