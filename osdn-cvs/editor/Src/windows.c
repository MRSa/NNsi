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
void NNsi_FrmDrawForm(FormType *frm, Boolean redraw)
{
    // �t�H�[����(���ʂ�)�`�悷��
    FrmDrawForm(frm);

    return;
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
                         UInt16 cnt, UInt16 selItem, 
                         MemHandle *memH, Char **listP)
{
    Char      *ptr;
    MemHandle  oldMemH;
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

    ptr = MemHandleLock(*memH);
    LstSetListChoices(lstP, (Char **) ptr, cnt);
    LstSetSelection  (lstP, selItem);
    LstSetTopItem    (lstP, selItem);

    LstDrawList(lstP);
    if (oldMemH != 0)
    {
        MemHandleUnlock(oldMemH);
        MemHandleFree(oldMemH);
    }
    if (listP != NULL)
    {
        *listP = ptr;
    }
    return;
}

/*=========================================================================*/
/*   Function :   NNshWinUpdateListItems                                   */
/*                                   ���X�g�Ƀf�[�^��ݒ�(���X�g�\�z�ς�)  */
/*=========================================================================*/
void NNshWinUpdateListItems(FormType *frm, UInt16 lstID, Char *ptr,
                            UInt16 cnt, UInt16 selItem)
{
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

    LstSetListChoices(lstP, (Char **) ptr, cnt);
    LstSetSelection  (lstP, selItem);
    LstSetTopItem    (lstP, selItem);
    LstDrawList(lstP);

    return;
}

/*=========================================================================*/
/*   Function :  NNshWinSetFieldText                                       */
/*                                                �t�B�[���h�e�L�X�g�̍X�V */
/*=========================================================================*/
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Boolean redraw, 
                         Char *msg, UInt32 size)
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
    NNsi_FrmDrawForm(frm, redraw);
    return;
}

/*=========================================================================*/
/*   Function : NNshWinSetPopItemsWithList                                 */
/*                    �|�b�v�A�b�v�g���K�̃��x���ݒ�(���X�g�����킹�Đݒ�) */
/*=========================================================================*/
void NNshWinSetPopItemsWithList(FormType *frm, UInt16 popId, UInt16 lstId, 
                                NNshWordList *wordList, UInt16 item)
{
    ListType    *lstP;
    ControlType *ctlP;
    Char        *ptr;

    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, popId));
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstId));
    if ((lstP == NULL)||(wordList == NULL)||(wordList->nofWord == 0)||
        (wordList->wordString == NULL)||(ctlP == NULL))
    {
        // ���X�g�w��ŃG���[�ɂȂ肻���ȏ�Ԃ͔r������
        return;
    }

    if (wordList->nofWord < item)
    {
        // �I��ԍ����n�~�o���Ă����ꍇ�ɂ́A�擪�ɂ��킹��
        item = 0;
    }

    ptr = MemHandleLock(wordList->wordmemH);
    LstSetListChoices(lstP, (Char **) ptr, wordList->nofWord);

    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstId));

    LstSetTopItem    (lstP, item);
    LstSetSelection  (lstP, item);
    CtlSetLabel      (ctlP, LstGetSelectionText(lstP, item));

    return;
}

/*=========================================================================*/
/*   Function : NNshWinSetPopItemsWithListArray                            */
/*                �|�b�v�A�b�v�g���K�̃��x���A���ݒ�(���X�g�����킹�Đݒ�) */
/*=========================================================================*/
void NNshWinSetPopItemsWithListArray(FormType *frm, NNshListItem *target,
                                     Char *listArray, UInt16 num, UInt16 *items)
{
    ListType    *lstP;
    ControlType *ctlP;

    while ((target != NULL)&&(target->popId != 0)&&(target->lstId != 0))
    {
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, target->popId));
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, target->lstId));

        if (num < *items)
        {
            // �I��ԍ����n�~�o���Ă����ꍇ�ɂ́A�擪�ɂ��킹��
            *items = 0;
        }
        LstSetListChoices(lstP, (Char **) listArray, num);
        LstSetTopItem    (lstP, *items);
        LstSetSelection  (lstP, *items);
        CtlSetLabel      (ctlP, LstGetSelectionText(lstP, *items));

        // ���̐ݒ�f�[�^�Ɉړ�����
        target++;
        items++;
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
    UInt16         len;
    RectangleType  r;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

#ifdef USE_COLOR
    RGBColorType  color;
#endif  // #ifdef USE_COLOR

    if (FrmGetActiveForm() == NULL)
    {
        // �E�B���h�E�\���O�́A�\�����Ȃ�
        return;
    }
    
    // ��ʒ���������������
    r.topLeft.x = 8;
    r.topLeft.y = 70;
    r.extent.x  = 140;
    r.extent.y  = 22;

    // �f�o�b�O�p��ʕ\��
#ifdef USE_DEBUG_ROM
    if (WinGetActiveWindow() == NULL)
    {
        FrmDrawForm(FrmGetActiveForm());
    }
#endif

    // ��ʂ�����
    WinEraseRectangle(&r, 0);

#ifdef USE_COLOR
    // OS 3.5�ȏ�̏ꍇ
    if (NNsiGlobal->palmOSVersion >= 0x03503000)
    {
        // �F��Ԃ�ۑ�
        WinPushDrawState();

        // �����͍�
        MemSet(&color, sizeof(color), 0x00);
        WinSetTextColor(WinRGBToIndex(&color));

        // �g�͐�
        color.b = 140;
        WinSetForeColor(WinRGBToIndex(&color));
    
        // �g��\��
        WinDrawRectangleFrame(dialogFrame, &r);

        // �������\������
        len = FntWordWrap(msg, (r.extent.x - (MARGIN * 2)));
        WinDrawChars(msg, len, (r.topLeft.x + MARGIN), (r.topLeft.y + MARGIN - 1));

        // �F��Ԃ𕜋A
        WinPopDrawState();
    }
    else
#else  // #ifdef USE_COLOR
    {
        // �g��\��
        WinDrawRectangleFrame(dialogFrame, &r);

        // �������\������
        len = FntWordWrap(msg, (r.extent.x - (MARGIN * 2)));
        WinDrawChars(msg, len, (r.topLeft.x + MARGIN), (r.topLeft.y + MARGIN - 1));
    }
#endif // #ifdef USE_COLOR

    return;
}

/*=========================================================================*/
/*   Function : Show_BusyForm                                              */
/*                                                      BUSY�t�H�[���̕\�� */
/*=========================================================================*/
void Show_BusyForm(Char *msg)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // ���݂̃t�H�[����ۑ�
    NNsiGlobal->prevBusyForm = FrmGetActiveForm();
    if (NNsiGlobal->prevBusyForm != NULL)
    {
        // ������\���p�^�[��
        SetMsg_BusyForm(msg);
    }
    return;
}

/*=========================================================================*/
/*   Function : Hide_BusyForm                                              */
/*                                                      BUSY�t�H�[���̍폜 */
/*=========================================================================*/
void Hide_BusyForm(Boolean redraw)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    if (NNsiGlobal->prevBusyForm != NULL)
    {
        NNsi_FrmDrawForm(NNsiGlobal->prevBusyForm, redraw);
    }
    return;
}

/*=========================================================================*/
/*   Function :  PrepareDialog_NNsh                                        */
/*                                              �_�C�A���O�\�������ݒ荀�� */
/*=========================================================================*/
void PrepareDialog_NNsh(UInt16 formID, FormType **diagFrm, Boolean silk)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if (NNsiGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(FrmGetActiveForm());
    }

    // �_�C�A���O�t�H�[�����A�N�e�B�u�ɂ���
    *diagFrm  = FrmInitForm(formID);
    if (*diagFrm == NULL)
    {
        return;
    }
    FrmSetActiveForm(*diagFrm);

#ifdef USE_DEBUG_ROM
    FrmDrawForm(*diagFrm);
#endif

    // �V���N��������Ȃ��ꍇ�ɂ͏I������
    if (silk != true)
    {
        return;
    }

    // �V���N�\�����ő剻
#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(*diagFrm, vgaFormModify160To240);
        SilkMaximizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMaximize(*diagFrm);
    }
    HandEraMoveDialog(*diagFrm);

    return;
}

/*=========================================================================*/
/*   Function :  PrologueDialog_NNsh                                       */
/*                                        �ݒ荀��(���[�_���t�H�[��)�̕\�� */
/*=========================================================================*/
void PrologueDialog_NNsh(FormType *prevFrm, FormType *diagFrm)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // �V���N�\�����ŏ���
#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMinimize(prevFrm);
    }
    return;
}

/*=========================================================================*/
/*   Function :   NNsh_DialogMessage                                       */
/*                                             ���b�Z�[�W�_�C�A���O�̕\��  */
/*=========================================================================*/
UInt16 NNsh_DialogMessage(UInt16 level, UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
#ifdef USE_MYALERT
    UInt16  size;
    Char   *buf;
#endif
    Char           logBuf[MINIBUF];
    NNshSavedPref *NNsiParam;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    switch (level)
    {
      case NNSH_LEVEL_ERROR:
        // �G���[���b�Z�[�W(�K���\������)
        break;

      case NNSH_LEVEL_WARN:
        // �x�����b�Z�[�W
        if ((NNsiParam->confirmationDisable & (NNSH_OMITDIALOG_WARNING)) != 0)
        {
            // �x���\�������Ȃ��ꍇ�ɂ́A"OK"(== 0)��Ԃ�
            return (0);
        }
        break;

      case NNSH_LEVEL_CONFIRM:
        // �m�F���b�Z�[�W
        if ((NNsiParam->confirmationDisable & (NNSH_OMITDIALOG_CONFIRM)) != 0)
        {
            // �m�F�\�������Ȃ��ꍇ�ɂ́A"OK"(== 0)��Ԃ�
            return (0);
        }
        break;

      case NNSH_LEVEL_DEBUG:
#ifdef DEBUG
        // �f�o�b�O�I�v�V�����t���ŃR���p�C�������ꍇ�ɂ́A�K���\������B
#else
        if (NNsiParam->debugMessageON == 0)
        {
            // DEBUG�\�����Ȃ��ꍇ�ɂ́A"OK"(== 0)��Ԃ�
            return (0);
        }
#endif
        break;

      case NNSH_LEVEL_INFO:
      default:
        // ��񃁃b�Z�[�W
        if ((NNsiParam->confirmationDisable & (NNSH_OMITDIALOG_INFORMATION))
                                                                          != 0)
        {
            // ���\�������Ȃ��ꍇ�ɂ́A"OK"(== 0)��Ԃ�
            return (0);
        }
        break;
    }

    // �_�C�A���O��\������
    MemSet (logBuf, sizeof(logBuf), 0x00);
    if (num != 0)
    {
        StrCopy(logBuf, "(0x");
        NUMCATH(logBuf, num);
        StrCat (logBuf, "/");
        NUMCATI(logBuf, num);
        StrCat (logBuf, ")");
    }

#ifdef USE_MYALERT
    size = StrLen(mes1) + StrLen(mes2) + StrLen(logBuf) + MARGIN;
    buf  = MemPtrNew_NNsh(size);
    if (buf == NULL)
    {
        return (FrmCustomAlert(altID, mes1, mes2, logBuf));
    }
    MemSet (buf, size, 0x00);
    StrCopy(buf, mes1);
    StrCat (buf, mes2);
    StrCat (buf, logBuf);
    
    MEMFREE_PTR(buf);
#else
    return (FrmCustomAlert(altID, mes1, mes2, logBuf));
#endif
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
 *  Name        Date                Description
 *  ----        ----                -----------
 *   art        07/01/96        Initial Revision
 *   gap        11/02/96        Fix case where field and scroll bars
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
 *  Name      Date        Description
 *  ----      -------        -----------
 *  art       7/1/96        Initial Revision
 *  grant     2/4/99        Use EditViewScroll() to do actual scrolling.
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

    // �I�u�W�F�N�g�̎擾
    frm  = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    if (sclID == 0)
    {
        // �t�B�[���h���X�N���[��
        FldScrollField(fldP, lines, direction);
        return (true);
    }

    // �X�N���[���s�����w�肳��Ă��邩�m�F
    if (lines == 0)
    {
        // �X�N���[���s�����w�肳��Ă��Ȃ��ꍇ�A�X�N���[���\���`�F�b�N����
        ret = FldScrollable(fldP, direction);
        if (ret != true)
        {
            return (ret);
        }

        // �X�N���[������s�����擾(�\���s�� - 1)
        linesToScroll = FldGetVisibleLines(fldP) - 1;

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
    else
    {
        // �w�肳�ꂽ�s�����X�N���[���o�[�̍s���Ɋ��蓖�Ă�
        linesToScroll = lines;
        ret = true;
    }

    // �t�B�[���h���X�N���[��
    FldScrollField(fldP, linesToScroll, direction);
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


/*=========================================================================*/
/*   Function :   Hander_EditAction                                        */
/*                                         �ҏW�����p�̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_EditAction(EventType *event)
{
    UInt16 keyCode;

    // ���݂̃t�H�[�����擾
    switch (event->eType)
    { 
      // ���j���[�I��
      case menuEvent:
        return (NNsh_MenuEvt_Edit(event));
        break;
      case keyDownEvent:
        // �L�[�R�[�h�ɂ���ď����𕪊򂳂���
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          case vchrJogRelease:
          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
          case vchrRockerDown:
          case vchrThumbWheelDown:
            return (true);
            break;

          case chrCarriageReturn:
          case chrLineFeed:
          case chrLeftArrow:
          case chrEscape:
          default:
            break;
        }
        break;

      case ctlSelectEvent:
      default: 
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
    EventType        *dummyEvent;
    UInt16            max, keyCode;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �_�~�[�C�x���g
    dummyEvent = &(NNsiGlobal->dummyEvent);

    // ���݂̃t�H�[�����擾
    switch (event->eType)
    { 
      // ���X�g�I���C�x���g
      case lstSelectEvent:
        if (NNsiGlobal->jumpSelection == event->data.lstSelect.selection)
        {
            // �_�u���^�b�v�����o(OK�{�^�������������Ƃɂ���)
            goto JUMP_SELECTION;
        }
        NNsiGlobal->jumpSelection = event->data.lstSelect.selection;
        break;

      // �L�[�_�E��(�{�^��)�̑I��
      case keyDownEvent:
        // ���ݑI������Ă��郊�X�g�A�C�e���ԍ����擾
        frm  = FrmGetActiveForm();
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_JUMPLIST));
        NNsiGlobal->jumpSelection = LstGetSelection(lstP);
        max  = LstGetNumberOfItems(lstP);

        //tungsten T 5way navigator (281����A���ӁI)
        keyCode = KeyConvertFiveWayToJogChara(event);

        // �L�[�R�[�h�ɂ���ď����𕪊򂳂���
        switch (keyCode)
        {
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
          case vchrThumbWheelPush:
            // Jog Push/Enter�L�[�������ꂽ�Ƃ�(OK�{�^�������������Ƃɂ���)
            goto JUMP_SELECTION;
            break;

          case chrLeftArrow:
          case chrEscape:
          case chrCapital_X:
          case chrSmall_X:
          case vchrThumbWheelBack:
            // ESC�L�[/X�L�[/5way�̍��{�^���������ꂽ�Ƃ�
            // (Cancel�{�^�������������Ƃɂ���)
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->data.ctlSelect.pControl  
                = (ControlType*) FrmGetObjectPtr(frm,
                                     FrmGetObjectIndex(frm, BTNID_JUMPCANCEL));
            dummyEvent->data.ctlSelect.controlID = BTNID_JUMPCANCEL;
            dummyEvent->eType                    = ctlSelectEvent;
            EvtAddEventToQueue(dummyEvent);
            return (true);
            break;

          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrNextField:
          case vchrThumbWheelDown:
          case vchrRockerDown:
            // �n�[�h�L�[���A�W���O������
            if (NNsiGlobal->jumpSelection < (max - 1))
            {
                (NNsiGlobal->jumpSelection)++;
                LstSetSelection(lstP, NNsiGlobal->jumpSelection);
            }
            else
            {
                // �I�������X�g�̐擪�ֈړ�
                NNsiGlobal->jumpSelection = 0;
                LstSetSelection(lstP, NNsiGlobal->jumpSelection);
                LstSetTopItem  (lstP, 0);
            }
            return (true);
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrPrevField:
          case vchrThumbWheelUp:
          case vchrRockerUp:
            // �n�[�h�L�[��A�W���O�㑗��
            if (NNsiGlobal->jumpSelection > 0)
            {
                (NNsiGlobal->jumpSelection)--;
                LstSetSelection(lstP, NNsiGlobal->jumpSelection);
            }
            else
            {
                // �I�������X�g�̖����ֈړ�
                NNsiGlobal->jumpSelection = max - 1;
                LstSetSelection(lstP, NNsiGlobal->jumpSelection);
            }
            return (true);
            break;

          default:
            // ��L�ȊO�̃L�[�ł͉������Ȃ�
            break;

        }
        return (false);
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

JUMP_SELECTION:
    // OK�{�^�������������Ƃɂ���
    frm = FrmGetActiveForm();
    MemSet(dummyEvent, sizeof(EventType), 0x00);
    dummyEvent->data.ctlSelect.pControl  
                = (ControlType*) FrmGetObjectPtr(frm,
                                       FrmGetObjectIndex(frm, BTNID_JUMPEXEC));
    dummyEvent->data.ctlSelect.controlID = BTNID_JUMPEXEC;
    dummyEvent->eType                    = ctlSelectEvent;
    EvtAddEventToQueue(dummyEvent);
    return (true);
}

/*=========================================================================*/
/*   Function :  NNshWinSelectionWindow                                    */
/*                                                    �I���E�B���h�E�̕\�� */
/*=========================================================================*/
UInt16 NNshWinSelectionWindow(UInt16 frmId, Char *listItemP, UInt16 nofItems, UInt16 selection)
{
    FormType  *prevFrm, *diagFrm;
    UInt16     btnId;
    MemHandle  listH;
    ListType  *lstP;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �A�C�e�����̊m�F(�[����������A���������߂�)
    if (nofItems == 0)
    {
        // �L�����Z�����ꂽ�̂Ɠ����̉����R�[�h��Ԃ�
        return (BTNID_JUMPCANCEL);
    }

    // ������
    listH   = 0;
    btnId   = BTNID_JUMPCANCEL;
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if (NNsiGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // �I��ԍ����ُ킾������A�擪�̔ԍ��ɂ���
    if (nofItems <= selection)
    {
        selection = 0;
    }

    diagFrm  = FrmInitForm(frmId);
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
            LstSetSelection  (lstP, selection);
            LstSetTopItem    (lstP, selection);
        }

        // �C�x���g�n���h���̐ݒ�
        FrmSetEventHandler(diagFrm, Handler_JumpSelection);

        // �W�����v����N���A
        NNsiGlobal->jumpSelection = 0;

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

/*=========================================================================*/
/*   Function : DataInputDialog                                            */
/*                                                NNsi���ʂ̓��̓_�C�A���O */
/*=========================================================================*/
Boolean DataInputDialog(Char *title, Char *area, UInt16 size, UInt16 usage, void *valueP)
{
    Boolean       ret = false;
    UInt16        btnId;
    FormType     *prevFrm, *diagFrm;
    FieldType    *fldP;

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // �_�C�A���O�\������
    PrepareDialog_NNsh(FRMID_INPUTDIALOG, &diagFrm, true);
    if (diagFrm == NULL)
    {
        return (false);
    }

    // �^�C�g�����R�s�[����
    FrmCopyTitle(diagFrm, title);

    // �w�肳�ꂽ��������E�B���h�E�ɔ��f������
    NNshWinSetFieldText(diagFrm, FLDID_INPUTFIELD, true, area, size);
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_INPUTFIELD));

    fldP = FrmGetObjectPtr(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_INPUTFIELD));
    // �������I�����Ă���
    FldSetSelection(fldP, 0, FldGetTextLength(fldP));

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_EditAction);

    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_DIALOG_OK)
    {
        // OK�{�^���������ꂽ�Ƃ��ɂ́A��������擾����
        NNshWinGetFieldText(diagFrm, FLDID_INPUTFIELD, area, size);
        ret = true;
        NNsh_DebugMessage(ALTID_INFO,"String:", area, 0);
    }

    // �_�C�A���O�\���̌�Еt��
    PrologueDialog_NNsh(prevFrm, diagFrm);

    return (ret);
}
