/*============================================================================*
 *  FILE: 
 *     nnshset.c
 *
 *  Description: 
 *     NNsh parameter setting functions for NNsh.
 *
 *===========================================================================*/
#define NNSHSET_C
#include "local.h"

extern void NNshWinSetPopItems(FormType *frm, UInt16 popId, UInt16 lstId,
                                                                  UInt16 item);

/*=========================================================================*/
/*   Function :  StrAToI_NNsh                                              */
/*                                                  ������𐔒l�ɕϊ����� */
/*=========================================================================*/
static UInt32 StrAToI_NNsh(Char *num)
{
    UInt32 size = 0;
    while ((*num >= '0')&&(*num <= '9'))
    {
        size = size * 10 + (*num - '0');
        num++;
    }

    return (size);
}

/*=========================================================================*/
/*   Function :  StrAToI_NNsh                                              */
/*                                                  ���l�𕶎���ɕϊ����� */
/*=========================================================================*/
void StrIToA_NNsh(Char *dst, UInt32 data)
{
    Char buf[40], *ptr;
    UInt32 temp;
    UInt16  num;
    
    MemSet(buf, sizeof(buf), 0x00);
    ptr  = buf;
    temp = data;

    // buf�Ɉ�ԉ��̌����當����ɕϊ����đ������
    while (temp != 0)
    {
        num  = temp % 10;
        *ptr = '0' + num;
        temp = temp / 10;
        ptr++;
    }
    // dst�Ƀf�[�^���i�[����
    while (ptr != buf)
    {
        ptr--;
        *dst = *ptr;
        dst++;
    }
    return;
}


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
    FieldType   *fldP;
    UInt16       item;
    UInt32       bufSize;
    Char        *logBuf, *numP;
    MemHandle    txtH;
    NNshSavedPref *NNsiParam;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    /***  NNsh�ݒ��(�O���[�o���ϐ���)���f������  ***/

    // �t�H�[�J�X�������ݒ肵�Ȃ�
    UpdateParameter(frm, CHKID_NOTAUTOFOCUS, &(NNsiParam->notAutoFocus));

    // �V���N���g�傷��
    UpdateParameter(frm, CHKID_SILKMAX, &(NNsiParam->silkMax));

    // TsPatch���g�p���Ȃ�
    UpdateParameter(frm, CHKID_NOTUSE_TSPATCH, &(NNsiParam->notUseTsPatch));

    // CLIE small�t�H���g���g�p����
    UpdateParameter(frm, CHKID_USE_CLIESMALL, &(NNsiParam->sonyHRFont));

    // �I�����ۑ��_�C�A���O��\��
    UpdateParameter(frm, CHKID_ENDSAVE_DIALOG, &(NNsiParam->useEndSaveDialog));

    // �N�����O��̕ҏW�t�@�C���𕜋A
    UpdateParameter(frm, CHKID_AUTORESTORE, &(NNsiParam->autoRestore));

    // �g�pVFS�w��
    item  = LstGetSelection(FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, LSTID_VFS_LOCATION)));
    NNsiParam->vfsUseCompactFlash = item;

    // DEBUG
    UpdateParameter(frm, CHKID_DEBUG_DIALOG,&(NNsiParam->debugMessageON));

    // �����o�b�t�@�T�C�Y�̕ύX
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_BUFFER_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP = MemHandleLock(txtH);
        bufSize = StrAToI_NNsh(numP);
        MemHandleUnlock(txtH);
        if ((NNsiParam->debugMessageON == 0)&&((bufSize < NNSH_WORKBUF_MIN)||(bufSize > NNSH_WORKBUF_MAX)))
        {
            // �f�[�^�l�ُ�A���ُ͔͈͈��\�����Č��ɖ߂�
            logBuf = MemPtrNew_NNsh(BUFSIZE);
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
            // �o�b�t�@�T�C�Y��ύX���ďI������
            if (NNsiParam->bufferSize != bufSize)
            {
                NNsiParam->bufferSize = bufSize;
            }
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "0", 0);
    }

    return (errNone);
}

/*=========================================================================*/
/*   Function :   Hander_NNshSetting                                       */
/*                                     NNsi�ݒ�n��(����)�C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_NNshSetting(EventType *event)
{
    Err          ret;
    FormType    *frm;
    void        *effectFunc;

    // �{�^���`�F�b�N�ȊO�̃C�x���g�͑��߂�
    switch (event->eType)
    { 
      case ctlSelectEvent:
        // �{�^���������ꂽ(���֐i��)
        break;

      default: 
        return (false);
        break;
    }

    effectFunc = NULL;
    frm = FrmGetActiveForm();
    switch (event->data.ctlSelect.controlID)
    {
      // "OK"�{�^���������ꂽ�Ƃ�(NNsi�ݒ�)
      case BTNID_CONFIG_NNSH_OK:
        effectFunc = &effect_NNshSetting;
        break;

      // "Cancel"�{�^���������ꂽ�Ƃ�
      case BTNID_CONFIG_NNSH_CANCEL:
        effectFunc = NULL;
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
            return (true);
        }
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNshSetting                                     */
/*                                                NNsi�ݒ�̃t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_NNshSetting(FormType *frm)
{
    Char      bufSize[MINIBUF];
#ifdef USE_TSPATCH
    Err       ret;
    UInt32    fontVer;
#endif
    NNshSavedPref *NNsiParam;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    /***  NNsh�ݒ��(�O���[�o���ϐ�����)���f������  ***/

    // �t�H�[�J�X�������ݒ肵�Ȃ�
    SetControlValue(frm, CHKID_NOTAUTOFOCUS, &(NNsiParam->notAutoFocus));

    // �V���N���g�傷��
    SetControlValue(frm, CHKID_SILKMAX, &(NNsiParam->silkMax));

    // �I�����ۑ��_�C�A���O��\��
    SetControlValue(frm, CHKID_ENDSAVE_DIALOG, &(NNsiParam->useEndSaveDialog));

    // �N�����O��̕ҏW�t�@�C���𕜋A
    SetControlValue(frm, CHKID_AUTORESTORE, &(NNsiParam->autoRestore));

    // CLIE small�t�H���g���g�p����
    SetControlValue(frm, CHKID_USE_CLIESMALL, &(NNsiParam->sonyHRFont));

    // TsPatch���g�p���Ȃ�
#ifdef USE_TSPATCH
    ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
    if (ret == errNone)
    {    
        SetControlValue(frm, CHKID_NOTUSE_TSPATCH, &(NNsiParam->notUseTsPatch));
    }
    else
#endif
    {
        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        CtlSetUsable(FrmGetObjectPtr(frm, 
                          FrmGetObjectIndex(frm, CHKID_NOTUSE_TSPATCH)), false);
    }

    // �g�pVFS�̎w��
    NNshWinSetPopItems(frm, POPTRID_VFS_LOCATION, LSTID_VFS_LOCATION,
                                                 NNsiParam->vfsUseCompactFlash);

    // DEBUG
    SetControlValue(frm, CHKID_DEBUG_DIALOG,&(NNsiParam->debugMessageON));

    // �t�B�[���h�Ɍ��݂̃o�b�t�@�T�C�Y�𔽉f������
    MemSet(bufSize, sizeof(bufSize), 0x00);
    StrIToA_NNsh(bufSize, NNsiParam->bufferSize);
    NNshWinSetFieldText(frm, FLDID_BUFFER_SIZE, true, bufSize, StrLen(bufSize));

    return (errNone);
}
