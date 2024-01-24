/*============================================================================*
 *  FILE: 
 *     windows.c
 *
 *  Description: 
 *     window control functions for NNsh.
 *
 *===========================================================================*/
#define WINDOWS_C

#include "local.h"

/*=========================================================================*/
/*   Function : NNsi_FrmDrawForm                                           */
/*                                                        �t�H�[���̍ĕ`�� */
/*=========================================================================*/
void NNsi_FrmDrawForm(FormType *frm)
{
    UInt16     formId;
    FieldType *fldP;

    // �n�C���]�\�����[�h�������ꍇ�́A����ɉ�ʍX�V
    if (NNshParam->useSonyTinyFont != 0)
    {
        // �t�H�[���̂h�c���擾����
        formId = FrmGetActiveFormID();
        if (formId != FRMID_MESSAGE)
        {
            // �X���Q�Ɖ�ʂłȂ���Δ�����B
            goto FUNC_END;
	}

        // ���b�Z�[�W�\���̈�͔�\���ɂ��ĉ�ʂ��ĕ`�悷��
        fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        FldSetUsable(fldP, false);
#ifdef USE_CLIE
        if (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY)
        {
            // CLIE�ŃV���N�̃T�C�Y���ύX���ꂽ�Ƃ��ɂ͈�U�t�H�[��������
            FrmEraseForm(frm);
        }
#endif
        FrmDrawForm(frm);
        if (NNshGlobal->totalLine != 0)
        {
            // �t�B�[���h�̕`��(�n�C���]���)
            NNsi_HRFldDrawField(fldP);

            // �X�N���[���o�[�̍X�V
            NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                             FrmGetObjectIndex(frm, SCLID_MESSAGE)));
        }
        return;
    }

FUNC_END:
    // �t�H�[����(���ʂ�)�`�悷��
    FrmDrawForm(frm);
    return;
}

/*=========================================================================*/
/*   Function : NNsi_HRSclUpdate                                           */
/*                                      �X�N���[���o�[�̕`��(�n�C���]�\��) */
/*=========================================================================*/
void NNsi_HRSclUpdate(ScrollBarType *barP)
{
    SclSetScrollBar(barP, (NNshGlobal->currentPage + 1), 1,
                    (NNshGlobal->nofPage + 1), 1);
    return;
}

/*=========================================================================*/
/*   Function : NNsi_HRFldDrawField                                        */
/*                                          �t�B�[���h�̕`��(�n�C���]�\��) */
/*=========================================================================*/
void NNsi_HRFldDrawField(FieldType *fldP)
{
    Char          *ptr,   *top;
    UInt16         fontID, lineLen, i, nlines, fontHeight = 0;
    Int16          len, line, startLine;
    MemHandle      txtH;
    RectangleType  dimF;

    // �\��������̎擾
    txtH = FldGetTextHandle(fldP);
    if (txtH == 0)
    {
        // �̈�擾�Ɏ��s
        goto END_FUNC;
    }
    ptr  = MemHandleLock(txtH);
    if (ptr == NULL)
    {
        // �̈�擾�Ɏ��s�A�ʏ�̃t�B�[���h�\�������ďI������
        goto END_FUNC;
    }
    top = ptr;

    // �t�B�[���h�̗̈�T�C�Y������Ă���
    FldGetBounds(fldP, &dimF);

    // �t�B�[���h���\���ɂ���
    FldReleaseFocus(fldP);
    FldSetUsable(fldP, false);

    // �t�H���g�̐ݒ�
#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        // �P���ɂQ�{�ɂ��ėǂ��̂��ȁH (���ƂŌ���...)
        dimF.extent.x   = dimF.extent.x  * 2;
        dimF.extent.y   = dimF.extent.y  * 2;
        dimF.topLeft.x  = dimF.topLeft.x * 2;
        dimF.topLeft.y  = dimF.topLeft.y * 2;

        // �\���̈���N���A����
        HRWinEraseRectangle(NNshGlobal->hrRef, &dimF, 0);

        // �`��t�H���g�̐ݒ�
        fontID = HRFntSetFont(NNshGlobal->hrRef, NNshParam->sonyHRFont);

        // CLIE NX�ȍ~�ł́A�t�H���g�̍��������߂�֐����قȂ�̂Œ���
        if (NNshGlobal->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
        {
            fontHeight   = FntCharHeight();
        }
        else
        {
            fontHeight = HRFntCharHeight(NNshGlobal->hrRef);
        }
    }
    else
#endif
    {
        // �\���̈���N���A���āA�`��t�H���g��ݒ肷��
        WinEraseRectangle(&dimF, 0);
        fontID       = FntSetFont(NNshParam->sonyHRFont);
        fontHeight   = FntCharHeight();
    }

    // �\���\�s��(nlines)���v�Z
    nlines = dimF.extent.y / fontHeight;
        
    // �\������s�������v�Z�H
    if (NNshGlobal->totalLine == 0)
    {
        NNshGlobal->currentPage = 0;
        NNshGlobal->prevHRLines = nlines;

#ifdef USE_CLIE
        // �n�C���]�`�惂�[�h�ł��ACLIE NX�̂Ƃ�
        if ((NNshGlobal->hrRef != 0)&&
            (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE))
        {

            // �\�����郁�b�Z�[�W�̍s���𐔂��AgNumTotalLine�Ɋi�[����
            while((StrLen(ptr) > 0))
            {
                len = HRFntWordWrap(NNshGlobal->hrRef, ptr, dimF.extent.x);
                (NNshGlobal->totalLine)++;
                ptr = ptr + len;
            }
        }
        else
#endif
        {
            // �\�����郁�b�Z�[�W�̍s���𐔂��AgNumTotalLine�Ɋi�[����
            while((StrLen(ptr) > 0))
            {
                len = FntWordWrap(ptr, dimF.extent.x);
                (NNshGlobal->totalLine)++;
                ptr = ptr + len;
            }
        }

        // ���b�Z�[�W�̑��y�[�W�����v�Z���AnofPage�Ɋi�[����
        NNshGlobal->nofPage = (NNshGlobal->totalLine / (nlines - 1));
        ptr = top;
    }

    // �\���J�n���C���̌v�Z
    if (NNshGlobal->currentPage == 0)
    {
        // �ŏ��̃y�[�W
        startLine = 0;
    }
    else
    {
        // 2�y�[�W�ڈȍ~�̌v�Z
        startLine = NNshGlobal->currentPage * 
                       (NNshGlobal->prevHRLines - 1); /* winDown or winUp */
    }

    // �\���\�s����ۑ�
    NNshGlobal->prevHRLines = nlines;

    // �\������y�[�W�̍s�̐擪�܂œǂݔ�΂�
    line = 0;

#ifdef USE_CLIE
    // �n�C���]�`�惂�[�h�ł��ACLIE NX�̂Ƃ�
    if ((NNshGlobal->hrRef != 0)&&
        (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE))
    {
        while((StrLen(ptr) > 0)&&(line != startLine))
        {
            len = HRFntWordWrap(NNshGlobal->hrRef, ptr, dimF.extent.x);
            line++;
            ptr = ptr + len;
        }
    }
    else
#endif
    {
        while((StrLen(ptr) > 0)&&(line != startLine))
        {
            len = FntWordWrap(ptr, dimF.extent.x);
            line++;
            ptr = ptr + len;
        }
    }
 
    // ��ʂɕ����������Ă���(�\�����镶��������A�����������C�����܂ŕ\��)
    i       = 0;
    lineLen = StrLen(ptr);
    while ((lineLen > 0)&&(i < nlines))
    {
#ifdef USE_CLIE
        // �n�C���]�`�惂�[�h�ł��ACLIE NX�̂Ƃ�
        if ((NNshGlobal->hrRef != 0)&&
            (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE))
        {
            len = HRFntWordWrap(NNshGlobal->hrRef, ptr, dimF.extent.x);
        }
        else
#endif
        {
            len = FntWordWrap(ptr, dimF.extent.x);
        }

#ifdef USE_CLIE
        if (NNshGlobal->hrRef != 0)
        {
            if (lineLen >= len)
            {
                // �s���܂ŕ�����\������ꍇ
                if ((*(ptr + (len - 3)) == '\x0a')&&
                    (*(ptr + (len - 2)) == '\x0d')&&
                    (*(ptr + (len - 1)) == '\x0a'))
                {
                    // �s�������s�R�[�h�������ꍇ�ɂ́A���s�R�[�h��\�����Ȃ�
                    HRWinDrawChars(NNshGlobal->hrRef, ptr, (len - 3),
                                   dimF.topLeft.x, dimF.topLeft.y);
                }
                else if ((*(ptr + (len - 2)) == '\x0d')&&
                         (*(ptr + (len - 1)) == '\x0a'))
                {
                    // �s�������s�R�[�h�������ꍇ�ɂ́A���s�R�[�h��\�����Ȃ�
                    HRWinDrawChars(NNshGlobal->hrRef, ptr, (len - 2),
                                   dimF.topLeft.x, dimF.topLeft.y);
                }
                else if (*(ptr + (len - 1)) == '\x0a')
                {
                    // �s�������s�R�[�h�������ꍇ�ɂ́A���s�R�[�h��\�����Ȃ�
                    HRWinDrawChars(NNshGlobal->hrRef, ptr, (len -1),
                                   dimF.topLeft.x, dimF.topLeft.y);
                }
                else
                {
                    HRWinDrawChars(NNshGlobal->hrRef, ptr, len,
                                   dimF.topLeft.x, dimF.topLeft.y);
                }
            }
            else
            {
                // ���[�h���b�v�ōs���܂ŕ\�����Ȃ��ꍇ
                HRWinDrawChars(NNshGlobal->hrRef, ptr, lineLen,
                               dimF.topLeft.x,dimF.topLeft.y);
            }
        }
        else
#endif
        {
            if (lineLen >= len)
            {
                // �s���܂ŕ�����\������ꍇ
                if ((*(ptr + (len - 2)) == '\x0d')&&
                    (*(ptr + (len - 1)) == '\x0a'))
                {
                    // �s�������s�R�[�h�������ꍇ�ɂ́A���s�R�[�h��\�����Ȃ�
                    WinDrawChars(ptr,(len - 2),dimF.topLeft.x,dimF.topLeft.y);
                }
                else if (*(ptr + (len - 1)) == '\x0a')
                {
                    // �s�������s�R�[�h�������ꍇ�ɂ́A���s�R�[�h��\�����Ȃ�
                    WinDrawChars(ptr,(len -1),dimF.topLeft.x,dimF.topLeft.y);
                }
                else
                {
                    WinDrawChars(ptr, len, dimF.topLeft.x, dimF.topLeft.y);
                }
            }
            else
            {
                // ���[�h���b�v�ōs���܂ŕ\�����Ȃ��ꍇ
                WinDrawChars(ptr, lineLen,dimF.topLeft.x,dimF.topLeft.y);
            }
        }

        // ���̍s�ɕ`��ʒu���ړ�������
        dimF.topLeft.y = dimF.topLeft.y + fontHeight;
        
        // ���̍s�ɕ\�����镶���ւ�����
        i++;
        ptr     = ptr + len;
        lineLen = StrLen(ptr);
    }
    MemHandleUnlock(txtH);

    return;

END_FUNC:
    // �ʏ�̃t�B�[���h�\�������ďI������
    FldDrawField(fldP);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  NNsi_HRFieldPageScroll                                    */
/*                              SONY�n�C���]�`�惂�[�h�ł̃y�[�W�X�N���[�� */
/*-------------------------------------------------------------------------*/
static Boolean NNsi_HRFieldPageScroll(UInt16 fldID, UInt16 sclID,
                                      UInt16 lines, WinDirectionType direction)
{
    FormType  *frm;
    FieldType *fldP;

    // �y�[�W�X�V�̕������m�F
    if (direction == winDown)
    {
        // �P�y�[�W��
        if (NNshGlobal->currentPage >= NNshGlobal->nofPage)
        {
            // �ア���ς��A�\���͈ړ����Ȃ�
            return (false);
        }
        (NNshGlobal->currentPage)++;
    }
    else
    {
        // �P�y�[�W��
        if (NNshGlobal->currentPage == 0)
        {
            // �������ς��A�\���͈ړ����Ȃ�
            return (false);
        }
        (NNshGlobal->currentPage)--;
    }

    // �I�u�W�F�N�g�̎擾
    frm  = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    // �n�C���]�`��
    NNsi_HRFldDrawField(fldP);

    // �X�N���[���o�[�̕\���X�V
    NNsi_HRSclUpdate(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, sclID)));

    return (true);
}

/*=========================================================================*/
/*   Function :   NNshWinClearList                                         */
/*                                                     ���X�g�f�[�^������  */
/*=========================================================================*/
void NNshWinClearList(FormType *frm, UInt16 lstID)
{
    ListType  *lstP;

    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstID));
    if (lstP != NULL)
    {
        LstEraseList(lstP);
    }
    return;
}

/*=========================================================================*/
/*   Function :   NNshWinSetListItems                                      */
/*                                                   ���X�g�Ƀf�[�^��ݒ�  */
/*=========================================================================*/
void NNshWinSetListItems(FormType *frm, UInt16 lstID, Char *list,
                         UInt16 cnt, UInt16 selItem, MemHandle *memH)
{
    MemHandle oldMemH;
    ListType  *lstP;

    if (cnt == 0)
    {
        // ���X�g�̃A�C�e�������[���̂Ƃ��A���X�g�����͍s��Ȃ��B
        return;
    }
    else if (cnt == 1)
    {
        // ���X�g���P�����Ȃ��ꍇ�A�I���A�C�e�������P�Ɍ��肷��B
        selItem = 0;
    }
    else if (cnt <= selItem)
    {
        // �A�C�e���I���A�C�e�������ُ�ȏꍇ�A�����Ƀt�H�[�J�X�����킹��
        selItem = cnt - 1;
    }

    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstID));
    if (lstP == NULL)
    {
        return;
    }

    oldMemH = *memH;
    *memH   = SysFormPointerArrayToStrings(list, cnt);
    if (*memH == 0)
    {
        return;
    }
    LstSetListChoices(lstP, (Char **) MemHandleLock(*memH), cnt);
    LstSetSelection  (lstP, selItem);
    LstSetTopItem    (lstP, selItem);

    LstDrawList(lstP);

    if (oldMemH != 0)
    {
        MemHandleUnlock(oldMemH);
        MemHandleFree(oldMemH);
    }
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
    NNsi_FrmDrawForm(frm);
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
/*   Function : SetMsg_BusyForm                                            */
/*                                                     BUSY�e�L�X�g�̐ݒ�  */
/*=========================================================================*/
void SetMsg_BusyForm(Char *msg)
{
    FormType *busyFrm;

    busyFrm = FrmGetActiveForm();
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, msg, StrLen(msg));
    return;
}

/*=========================================================================*/
/*   Function : Show_BusyForm                                              */
/*                                                      BUSY�t�H�[���̕\�� */
/*=========================================================================*/
void Show_BusyForm(Char *msg)
{
    FormType  *busyFrm;

    // ���݂̃t�H�[����ۑ�
    NNshGlobal->prevBusyForm = FrmGetActiveForm();

    // BUSY�_�C�A���O�t�H�[���𐶐��A�A�N�e�B�u�ɂ���
    busyFrm = FrmInitForm(FRMID_BUSY);
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(busyFrm, vgaFormModify160To240);
    }
#endif // #ifdef USE_HANDERA
    FrmSetActiveForm(busyFrm);

    // �t�B�[���h�ɕ������ݒ�
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, msg, StrLen(msg));
    return;
}

/*=========================================================================*/
/*   Function : Hide_BusyForm                                              */
/*                                                      BUSY�t�H�[���̍폜 */
/*=========================================================================*/
void Hide_BusyForm(void)
{
    FormType *busyFrm;

    // BUSY�t�H�[�����폜���ABUSY�t�H�[���̑O�ɕ\�����Ă����t�H�[�����J��
    if (NNshGlobal->prevBusyForm != NULL)
    {
        busyFrm = FrmGetActiveForm();
        FrmEraseForm(busyFrm);
        FrmSetActiveForm(NNshGlobal->prevBusyForm);
        NNsi_FrmDrawForm(NNshGlobal->prevBusyForm);
        FrmDeleteForm   (busyFrm);
    }
    NNshGlobal->prevBusyForm = NULL;

    return;
}

/*=========================================================================*/
/*   Function :   NNsh_DebugMessage                                        */
/*                                                �f�o�b�O���b�Z�[�W�̕\�� */
/*=========================================================================*/
void NNsh_DebugMessage(UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
#ifdef DEBUG
    // �f�o�b�O�I�v�V�����t���ŃR���p�C�������ꍇ�ɂ́A�K���\������B
#else
    if (NNshParam->debugMessageON != 0)
#endif
    {
        NNsh_ErrorMessage(altID, mes1, mes2, num);
    }
    return;
}

/*=========================================================================*/
/*   Function :   NNsh_ErrorMessage                                        */
/*                                                       ���b�Z�[�W�̕\��  */
/*=========================================================================*/
UInt16 NNsh_ErrorMessage(UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
    Char logBuf[MINIBUF];
    MemSet (logBuf, sizeof(logBuf), 0x00);
    if (num != 0)
    {
        StrCopy(logBuf, "(0x");
        NUMCATH(logBuf, num);
        StrCat (logBuf, "/");
        NUMCATI(logBuf, num);
        StrCat (logBuf, ")");
    }
    return (FrmCustomAlert(altID, mes1, mes2, logBuf));
}

/*=========================================================================*/
/*   Function :   NNsh_ConfirmMessage                                      */
/*                                                   �m�F���b�Z�[�W�̕\��  */
/*=========================================================================*/
UInt16 NNsh_ConfirmMessage(UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
    if (NNshParam->confirmationDisable != 0)
    {
        // �m�F�\�������Ȃ��ꍇ�ɂ́A"OK"(== 0)��Ԃ�
        return (0);
    }
    return (NNsh_ErrorMessage(altID, mes1, mes2, num));
}

// �y�[�W�X�N���[���\���FMEMO(SDK 4.0�t��)�̃T���v�����玝���Ă��ĕύX
/***********************************************************************
 *
 * FUNCTION:    EditViewUpdateScrollBar
 *
 * DESCRIPTION: This routine update the scroll bar.
 *
 * PARAMETERS:
 *              fldP      field object
 *              barP      scroll bar object
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *
 *  Name	Date		Description
 *  ----	----		-----------
 *   art	07/01/96	Initial Revision
 *   gap	11/02/96	Fix case where field and scroll bars
 *                              get out of sync.
 *  nnsh        25/04/02        tunes for nanashi.
 ***********************************************************************/
void NNshWinViewUpdateScrollBar(UInt16 fldID, UInt16 sclID)
{
    FormType *frm;
    void     *fldP, *barP;
    UInt16    scrollPos;
    UInt16    textHeight;
    UInt16    fieldHeight;
    Int16     maxValue;

    frm  = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));
    barP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, sclID));

    FldGetScrollValues(fldP, &scrollPos, &textHeight, &fieldHeight);
    if (textHeight > fieldHeight)
    {
       // On occasion, such as after deleting a multi-line selection of text,
       // the display might be the last few lines of a field followed by some
       // blank lines. To keep the current position in place and allow the user
       // to "gracefully" scroll out of the blank area, the number of blank
       // lines visible needs to be added to max value.  Otherwise the scroll
       // position may be greater than maxValue, get pinned to maxvalue
       // in SclSetScrollBar resulting in the scroll bar and the display being
       // out of sync.
       maxValue = (textHeight - fieldHeight) + FldGetNumberOfBlankLines(fldP);
    }
    else
    {
        // �P��ʂɕ\���ł���ꍇ�A�X�N���[���o�[��\�����Ȃ��悤�׍H����
        maxValue = (scrollPos == 0) ? 0 : scrollPos;
    }
    SclSetScrollBar (barP, scrollPos, 0, maxValue, fieldHeight - 1);

    return;
}

// �y�[�W�X�N���[������FMEMO(SDK 4.0�t��)�̃T���v�����玝���Ă��ĕύX
/***********************************************************************
 *
 * FUNCTION:    EditViewPageScroll
 *
 * DESCRIPTION: This routine scrolls the message a page winUp or winDown.
 *              When the top of a memo is visible, scrolling up will
 *              display the display the botton of the previous memo.
 *              If the bottom of a memo is visible, scrolling down will
 *              display the top of the next memo.
 *
 * PARAMETERS:
 *              fldP          field object
 *              sclP          scroll object
 *              direction     winUp or winDown
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *  Name      Date	Description
 *  ----      -------	-----------
 *  art       7/1/96	Initial Revision
 *  grant     2/4/99	Use EditViewScroll() to do actual scrolling.
 *  nnsh     25/4/02    tunes for nanashi.
 ***********************************************************************/
Boolean NNshWinViewPageScroll(UInt16 fldID, UInt16 sclID, 
                              UInt16 lines, WinDirectionType direction)
{
    FormType *frm;
    void     *fldP, *sclP;
    UInt16    linesToScroll;
    Int16     value, min, max, pageSize;
    Boolean   ret;

    //  �O���t�B�b�N�`�惂�[�h�ŁA���b�Z�[�W�Q�Ǝ��Ƀt�B�[���h�X�V��
    // �ꍇ�ɂ͕ʊ֐��ŏ�������B
    if ((NNshParam->useSonyTinyFont != 0)&&(fldID == FLDID_MESSAGE))
    {
        return (NNsi_HRFieldPageScroll(fldID, sclID, lines, direction));
    }

    // �I�u�W�F�N�g�̎擾
    frm  = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    if (sclID == 0)
    {
        // �t�B�[���h���X�N���[��
        FldScrollField(fldP, lines, direction);
        return (true);
    }

    // �X�N���[���\���`�F�b�N����
    ret = FldScrollable(fldP, direction);
    if (ret == true)
    {
        // �X�N���[������s�����擾(�\���s�� - 1)
        linesToScroll = FldGetVisibleLines(fldP) - 1;

        // �t�B�[���h���X�N���[��
        FldScrollField(fldP, linesToScroll, direction);

        // �X�N���[���o�[�̍X�V
        sclP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, sclID));
        SclGetScrollBar(sclP, &value, &min, &max, &pageSize);  
        if (direction == winUp)
        {
            value = value - linesToScroll;
        }
        else
        {
            value = value + linesToScroll;
        }
        SclSetScrollBar(sclP, value, min, max, pageSize);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function : NNsh_MenuEvt_Edit                                          */
/*                                        �ҏW���j���[���I�����ꂽ���̏��� */
/*=========================================================================*/
Boolean NNsh_MenuEvt_Edit(EventType *event)
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

/*-------------------------------------------------------------------------*/
/*   Function :   Hander_JumpSelection                                     */
/*                                           �W�����v�p�̃C�x���g�n���h��  */
/*-------------------------------------------------------------------------*/
Boolean Handler_JumpSelection(EventType *event)
{
    FormType         *frm;
    ListType         *lstP;
    NNshMessageIndex *idxP;
    EventType        *dummyEvent;
    UInt16            max, keyCode;

    // ���b�Z�[�W�C���f�b�N�X
    idxP       = NNshGlobal->msgIndex;
    dummyEvent = &(NNshGlobal->dummyEvent);

    // ���݂̃t�H�[�����擾
    switch (event->eType)
    { 
      // ���X�g�I���C�x���g
      case lstSelectEvent:
        NNshGlobal->jumpSelection = event->data.lstSelect.selection;
        break;

      // �L�[�_�E��(�{�^��)�̑I��
      case keyDownEvent:
        // ���ݑI������Ă��郊�X�g�A�C�e���ԍ����擾
        frm  = FrmGetActiveForm();
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_JUMPLIST));
        NNshGlobal->jumpSelection = LstGetSelection(lstP);
        max  = LstGetNumberOfItems(lstP);

        // �L�[�R�[�h�ɂ���ď����𕪊򂳂���
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
            // Jog Push/Enter�L�[�������ꂽ�Ƃ�(OK�{�^�������������Ƃɂ���)
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->data.ctlSelect.pControl  
                = (ControlType*) FrmGetObjectPtr(frm,
                                       FrmGetObjectIndex(frm, BTNID_JUMPEXEC));
            dummyEvent->data.ctlSelect.controlID = BTNID_JUMPEXEC;
            dummyEvent->eType                    = ctlSelectEvent;
            EvtAddEventToQueue(dummyEvent);
            return (true);
            break;

          case chrEscape:
          case chrCapital_X:
          case chrSmall_X:
            // ESC�L�[/X�L�[�������ꂽ�Ƃ�(Cancel�{�^�������������Ƃɂ���)
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->data.ctlSelect.controlID = BTNID_JUMPCANCEL;
            dummyEvent->eType                    = ctlSelectEvent;
            EvtAddEventToQueue(dummyEvent);
            return (true);
            break;

          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
            // �n�[�h�L�[���A�W���O������
            if (NNshGlobal->jumpSelection < (max - 1))
            {
                (NNshGlobal->jumpSelection)++;
                LstSetSelection(lstP, NNshGlobal->jumpSelection);
                return (true);
	    }
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
            // �n�[�h�L�[���A�W���O������
            if (NNshGlobal->jumpSelection > 0)
            {
                (NNshGlobal->jumpSelection)--;
                LstSetSelection(lstP, NNshGlobal->jumpSelection);
                return (true);
	    }
            break;

          default:
            // ��L�ȊO�̃L�[�ł͉������Ȃ�
            break;

        }
        break;

      // �R���g���[��(�{�^��)�̑I��
      case ctlSelectEvent:
        // �������Ȃ�
        break;

      default: 
        // �������Ȃ�
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :  NNshWinSelectionWindow                                    */
/*                                                    �I���E�B���h�E�̕\�� */
/*=========================================================================*/
UInt16 NNshWinSelectionWindow(Char *listItemP, UInt16 nofItems)
{
    FormType  *prevFrm, *diagFrm;
    UInt32     ver;
    UInt16     btnId;
    MemHandle  listH;
    ListType  *lstP;

    // ������
    listH   = 0;
    btnId   = BTNID_JUMPCANCEL;
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if ((FtrGet(sysFtrCreator, sysFtrNumROMVersion, &ver) == errNone)
         &&(ver < 0x03503000))
    {
        FrmEraseForm(prevFrm);
    }

    diagFrm  = FrmInitForm(FRMID_JUMPMSG);
    if (diagFrm != NULL)
    {
        // �t�H�[�����A�N�e�B�u�ɂ���
        FrmSetActiveForm(diagFrm);

        // ���X�g�̐ݒ�
        lstP  = FrmGetObjectPtr(diagFrm,
                                FrmGetObjectIndex(diagFrm, LSTID_JUMPLIST));
        listH = SysFormPointerArrayToStrings(listItemP, nofItems);
        if (listH != 0)
        {
            LstSetListChoices(lstP, (Char **) MemHandleLock(listH), nofItems);
            LstSetSelection  (lstP, 0);
            LstSetTopItem    (lstP, 0);
        }

        // �C�x���g�n���h���̐ݒ�
        FrmSetEventHandler(diagFrm, Handler_JumpSelection);

        // �W�����v����N���A
        NNshGlobal->jumpSelection = 0;

        // �_�C�A���O�̃I�[�v���A������̓��͂�҂�
        btnId = FrmDoDialog(diagFrm);

        // �_�C�A���O�����
        FrmSetActiveForm(prevFrm);
        FrmDeleteForm(diagFrm);
    }

    if (listH != 0)
    {
        MemHandleUnlock(listH);
        MEMFREE_HANDLE (listH);
    }
    // FrmEraseForm(prevFrm);
    FrmDrawForm(prevFrm);
    return (btnId);
}
