/*============================================================================*
 *  FILE: 
 *     nnshset.c
 *
 *  Description: 
 *     NNsh parameter setting functions for NNsh.
 *
 *===========================================================================*/
#define NNSHMAIN_C
#include "local.h"


/*=========================================================================*/
/*   Function : saveMacroData                                              */
/*                                                 �}�N���X�N���v�g�̏o��  */
/*=========================================================================*/
Boolean  SaveMacroData(Char *name)
{
    Err    ret;
    UInt16 butID;

    // �}�N�����o�͂��邩�ǂ����m�F����
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_WRITE_DB, name, 0);
    if (butID != 0)
    {
        // �}�N���f�[�^�̏o�͂𒆎~
        return (true);
    }

    // �}�N���f�[�^�̕ۑ�
    ret = WriteMacroDBData(name,
                           DBVERSION_MACROSCRIPT,
                           SOFT_CREATOR_ID,
                           DATA_DBTYPE_ID, 
                           NNshGlobal->currentScriptLine,
                           NNshGlobal->scriptArea);
    if (ret == errNone)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_WRITEDB_OK, name, ret);
        NNshGlobal->editFlag = false;
        return (true);
    }
    NNsh_DebugMessage(ALTID_ERROR, "Write Failure :", name, ret);
    return (false);
}

/*=========================================================================*/
/*   Function : LoadMacroData                                              */
/*                                             �}�N���X�N���v�g�̓ǂ݂���  */
/*=========================================================================*/
Boolean  LoadMacroData(Char *name)
{
    Err    ret;
    UInt16 butID;

    // �}�N�����o�͂��邩�ǂ����m�F����
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_READ_DB, name, 0);
    if (butID != 0)
    {
        // �}�N���f�[�^�̏o�͂𒆎~
        return (true);
    }

    // �}�N���f�[�^�̓ǂݏo��
    MemSet(NNshGlobal->scriptArea,
           (sizeof(NNshMacroRecord) * (NNshGlobal->maxScriptLine) + MARGIN),
           0x00);
    ret = ReadMacroDBData(name, DBVERSION_MACROSCRIPT,
                          SOFT_CREATOR_ID, DATA_DBTYPE_ID,
                          &(NNshGlobal->currentScriptLine), 
                          NNshGlobal->maxScriptLine,
                          NNshGlobal->scriptArea);
    if (ret == errNone)
    {
        NNshGlobal->editFlag = true;
        return (true);
    }
    NNsh_DebugMessage(ALTID_ERROR, "Read Failure :", name, ret);
    return (false);
}

/*=========================================================================*/
/*   Function : DeleteMacroData                                            */
/*                                                 �}�N���X�N���v�g�̍폜  */
/*=========================================================================*/
Boolean  DeleteMacroData(Char *name)
{
    UInt16 butID;
    LocalID dbId;

    // �}�N�����폜���邩�ǂ����m�F����
    butID = NNsh_ConfirmMessage(ALTID_WARN, name, MSG_CONFIRM_DELETE_DB, 0);
    if (butID != 0)
    {
        // �}�N���f�[�^�̍폜�𒆎~
        return (true);
    }
    dbId = DmFindDatabase(0, name);
    if (dbId != 0)
    {
        // DB���폜����
        (void) DmDeleteDatabase(0, dbId);
        NNsh_InformMessage(ALTID_INFO, MSG_DB_DELETED, name, 0);
        return (true);
    }
    // DB���Ȃ�����...�폜�𒆎~����
    return (true);
}


/*=========================================================================*/
/*   Function :   EffectData_NNshFiler                                     */
/*                                        �t�H�[������ݒ肳�ꂽ�����擾 */
/*=========================================================================*/
void EffectData_NNshFiler(FormType *frm, Char *fileName)
{
    MemHandle  txtH;
    FieldType *fldP;
    Char      *txtP;
    
    // �t�@�C�������擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_FILENAME));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        if ((txtP != NULL)&&(*txtP != '\0'))
        {
            StrCopy(fileName, txtP);
        }
        MemHandleUnlock(txtH);
    }
    return; 
}


/*=========================================================================*/
/*   Function :   Handler_NNshFiler                                        */
/*                                   �C�x���g�n���h���i�t�@�C��OPEN/CLOSE) */
/*=========================================================================*/
Boolean Handler_NNshFiler(EventType *event)
{
    Char    **fnameP, *txtP;
    UInt16    keyCode, cnt, max;
    FormType *frm;
    ListType *lstP;

    frm = FrmGetActiveForm();
    switch (event->eType)
    { 
      case keyDownEvent:
        // �L�[����
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          // �u���v(�W���O�_�C������)�����������̏���
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrNextField:
          case vchrThumbWheelDown:
          case chrDownArrow:
          case vchrPageDown:
            // �������Ȃ�
            lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
            max  = LstGetNumberOfItems(lstP);
            cnt  = LstGetSelection(lstP);
            if (cnt == noListSelection)
            {
                cnt = 0;
            }
            else if (cnt < (max - 1))
            {
                cnt++;
            }
            LstSetSelection(lstP, cnt);
            txtP = LstGetSelectionText(lstP, cnt);
            NNshWinSetFieldText(frm, FLDID_FILENAME, true, txtP, StrLen(txtP));
            break;

          // �u��v(�W���O�_�C������)�����������̏���
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrPrevField:
          case vchrThumbWheelUp:
          case chrUpArrow:
          case vchrPageUp:
            // �������Ȃ�
            lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
            cnt  = LstGetSelection(lstP);
            if (cnt == noListSelection)
            {
                cnt = 0;
            }
            else if (cnt > 0)
            {
                cnt--;
            }
            LstSetSelection(lstP, cnt);
            txtP = LstGetSelectionText(lstP, cnt);
            NNshWinSetFieldText(frm, FLDID_FILENAME, true, txtP, StrLen(txtP));
            break;

          case vchrJogRelease:
          case vchrThumbWheelPush:
          case chrCarriageReturn:
          case chrLineFeed:
          case vchrJogLeft:
            // ���s�R�[�h�����͂��ꂽ�AOK�{�^�������������Ƃɂ���
            if ((StrLen(NNshParam->fileName) == 0)||(NNshParam->fileName[0] == ' '))
            {
                //  �t�@�C���������͂���Ă��Ȃ��A���̂Ƃ��͑I������Ă���
                // ���X�g�̃t�@�C�������g�p����
                lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
                cnt = LstGetSelection(lstP);
                if (cnt == noListSelection)
                {
                    cnt = 0;
                }
                txtP = LstGetSelectionText(lstP, cnt);
                if (*txtP != ' ')
                {
                    StrCopy(NNshParam->fileName, txtP);
                }
            }
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_FILE_OK)));
            break;

          case vchrJogBack:
          case chrEscape:
          case vchrThumbWheelBack:
          case vchrJogRight:
            // ESC�R�[�h�����͂��ꂽ�ACANCEL�{�^�������������Ƃɂ���
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_FILE_CANCEL)));
            break;

          case chrLeftArrow:         // �J�[�\���L�[��(5way��)
          case chrRightArrow:         // �J�[�\���L�[�E(5way�E)
          default:
            // �������Ȃ�
            break;
        }        
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ
        break;

      case popSelectEvent:
        // �|�b�v�A�b�v���I�����ꂽ
        break;

      case lstSelectEvent:
        // ���X�g��I��
        // (�t�@�C�������ɑI�������A�C�e����\������)
        MemHandleUnlock(NNshGlobal->fileListH);
        fnameP = MemHandleLock(NNshGlobal->fileListH);
        txtP = (Char *) fnameP[event->data.lstSelect.selection];
        if (*txtP != ' ')
        {
            MemSet(NNshParam->fileName, (MAXLENGTH_FILENAME + MARGIN), 0x00);
            StrCopy(NNshParam->fileName, txtP);        
            NNshWinSetFieldText(frm, FLDID_FILENAME, true,NNshParam->fileName,
                                StrLen(NNshParam->fileName));
        }
        break;

      case menuEvent:
      case sclRepeatEvent:
      case fldChangedEvent:
      default:
        // �������Ȃ� 
        break;
    }
    return (false);
}


/*-------------------------------------------------------------------------*/
/*   Function : setFileListItems                                           */
/*                                                �t�@�C���ꗗ�̃��X�g�ݒ� */
/*-------------------------------------------------------------------------*/
static void setFileListItems(FormType *frm, Char **fnameP)
{
    Err                  ret, err;
    UInt16               cardNo, cnt;
    LocalID              dbLocalID;
    DmSearchStateType    state;
    Char                 fileName[MINIBUF + MINIBUF + MARGIN], **ptr;
    ListType            *lstP;

    // VFS�̎g�p�ݒ�ɂ��킹�ăt���O��ݒ�A�t�@�C�������ƁA�ꗗ�\��
    ptr = fnameP;
    cnt = 0;

    // Palm�{�̓��ɂ���t�@�C���̈ꗗ���擾
    ret = DmGetNextDatabaseByTypeCreator(true, &state, 
                                         DATA_DBTYPE_ID, 
                                         SOFT_CREATOR_ID,
                                         false, &cardNo, &dbLocalID);
    while ((ret == errNone)&&(dbLocalID != 0))
    {
        // �t�@�C�������擾
        MemSet(fileName, sizeof(fileName), 0x00);
        err = DmDatabaseInfo(cardNo, dbLocalID, fileName,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL);
        if (err == errNone)
        {
            // �擾�����t�@�C���������X�g�A�C�e���ɃR�s�[
            if ((fnameP != NULL)&&(cnt < MAX_FILELIST))
            {
                *ptr = MemPtrNew_NNsh(StrLen(fileName) + MARGIN);
                if (*ptr != NULL)
                {
                    MemSet(*ptr, (StrLen(fileName) + MARGIN), 0x00);
                    StrCopy(*ptr, fileName);
                    cnt++;
                    ptr++;
                }
            }
        }
        // ���ڈȍ~�̃f�[�^�擾
        ret = DmGetNextDatabaseByTypeCreator(false, &state,
                                             DATA_DBTYPE_ID, 
                                             SOFT_CREATOR_ID,
                                             false, &cardNo, &dbLocalID);
    }

    // �t�@�C�����P�����Ȃ��ꍇ�ɂ́A�X�y�[�X��\������
    if (cnt == 0)
    {
        *ptr = MemPtrNew_NNsh(StrLen("  ") + MARGIN);
        StrCopy(*ptr, "  ");
        cnt++;
        ptr++;
    }

    // �t�@�C���ꗗ�����X�g�ݒ�(�������A���I���ɂ���)
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_FILESELECTION));   
    LstSetListChoices(lstP, fnameP, cnt);
    LstSetTopItem    (lstP, 0);
    LstSetSelection  (lstP, noListSelection);
    LstDrawList(lstP);

    return;
}

/*=========================================================================*/
/*   Function : OpenForm_NNshFiler                                         */
/*                                        �ݒ荀��(���[�_���t�H�[��)�̕\�� */
/*=========================================================================*/
Err OpenForm_NNshFiler(FormType *frm, UInt16 command, Char **fnameP)
{
    switch (command)
    {
      case BTNID_DB_SAVE:
        // �f�[�^�̕ۊ�
         FrmCopyTitle(frm, "Save Macro...");
        break;

      case BTNID_DB_READ:
      default:
        // �f�[�^�̓ǂݏo��
         FrmCopyTitle(frm, "Read Macro...");
        break;
    }

    // �t�@�C�������X�g��ݒ肷��
    setFileListItems(frm, fnameP);

    // �t�@�C�������͗��ɑO����͂����t�@�C���������A�t�H�[�J�X��ݒ肷��
    if (StrLen(NNshParam->fileName) != 0)
    {
        NNshWinSetFieldText(frm, FLDID_FILENAME, true,
                            NNshParam->fileName, StrLen(NNshParam->fileName));
    }
    FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_FILENAME));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   NNsh_FileOperation                                       */
/*                                                  �f�[�^�̓ǂݍ���/�ۊ�  */
/*=========================================================================*/
Boolean NNsh_FileOperation(UInt16 command)
{
    Boolean   ret;
    Char     *txtP, **area;
    UInt16    btnId, size, cnt;
    FormType *prevFrm, *diagFrm;

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // �t�@�C�����X�g���N���A����
    MEMFREE_HANDLE(NNshGlobal->fileListH);
    area = NULL;
    size = sizeof(Char *) * (MAX_FILELIST + MARGIN);
    NNshGlobal->fileListH = MemHandleNew(size);
    if (NNshGlobal->fileListH != 0)
    {
        area = MemHandleLock(NNshGlobal->fileListH);
        MemSet(area, size, 0x00);
    }

    // �_�C�A���O�\���̏���
    PrepareDialog_NNsh(FRMID_FILE_SAVELOAD, &diagFrm, false);
    if (diagFrm == NULL)
    {
        // �\���̏������s
        return (false);
    }

    // �ݒ菉���l�̔��f
    OpenForm_NNshFiler(diagFrm, command, area);
    
    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_NNshFiler);

    // ��ʕ\��
    btnId = FrmDoDialog(diagFrm);
    switch (btnId)
    {
      case BTNID_FILE_OK:
      case BTNID_FILE_DELETE:
        // �t�H�[����ɂ���t�@�C�������擾����
        MemSet(NNshParam->fileName, (MAXLENGTH_FILENAME + MARGIN), 0x00);
        EffectData_NNshFiler(diagFrm, NNshParam->fileName);
        break;

      case BTNID_FILE_CANCEL:
      default:
        // �������Ȃ�(���s�L�����Z��)
        break;
    }
    
    // �\����ʂ����ɖ߂�
    PrologueDialog_NNsh(prevFrm, diagFrm);
    FrmDrawForm(FrmGetActiveForm());

    if (area != NULL)
    {
        // �t�@�C�����X�g���N���A����
        for (cnt = 0; cnt < (MAX_FILELIST + MARGIN); cnt++)
        {
            txtP = (Char *) area[cnt];
            MEMFREE_PTR(txtP);
        }
        MemSet(area, sizeof(Char *) * (MAX_FILELIST + MARGIN), 0x00);
    }
    if (NNshGlobal->fileListH != 0)
    {
        MemHandleUnlock(NNshGlobal->fileListH);
        MEMFREE_HANDLE (NNshGlobal->fileListH);
    }

    // �������ւ̕���
    switch (btnId)
    {
      case BTNID_FILE_OK:
        switch (command)
        {
          case BTNID_DB_SAVE:
            // �ۑ�
            SaveMacroData(NNshParam->fileName);
            break;

          case BTNID_DB_READ:
          default:
            // �ǂݍ���
            LoadMacroData(NNshParam->fileName);
            break;
        }
        break;

      case BTNID_FILE_DELETE:
        // �폜
        DeleteMacroData(NNshParam->fileName);
        break;

      default:
        break;
    }
    return (false);
}


/*=========================================================================*/
/*   Function : insertOneLine                                              */
/*                                                         �P�s�}�����[�h  */
/*=========================================================================*/
static void insertOneLine(void)
{
    NNshGlobal->editMode = NNSH_MODE_INSERT;
    NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DB_INFO, true, MSG_INSERTLINE_POINT, MINIBUF);
    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                        BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
    return;
}

/*=========================================================================*/
/*   Function : deleteOneLine                                              */
/*                                                         �P�s�폜���[�h  */
/*=========================================================================*/
static void deleteOneLine(void)
{
    NNshGlobal->editMode = NNSH_MODE_DELETE;
    NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DB_INFO, true, MSG_DELETELINE_POINT, MINIBUF);
    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                        BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
    return;
}

/*=========================================================================*/
/*   Function : changeDispFont                                             */
/*                                                         �t�H���g�̕ύX  */
/*=========================================================================*/
static Boolean changeDispFont()
{
    NNshParam->currentFont = FontSelect(NNshParam->currentFont);
    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                        BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
    return (true);
}
/*=========================================================================*/
/*   Function : extractMacroData                                           */
/*                                       �}�N���X�N���v�g�̎��s�̈�֏o��  */
/*=========================================================================*/
Boolean  extractMacroData(void)
{
    Err          ret;
    UInt16       butID;
    EventType   *dummyEvent;

    // �}�N�����o�͂��邩�ǂ����m�F����
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_OUTPUT_MACRO, "", 0);
    if (butID != 0)
    {
        // �}�N���f�[�^�̏o�͂𒆎~
        // (NNsj���I�������邩�ǂ����m�F����)
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_APLEND, "", 0);
        if (butID == 0)
        {
            // �A�v���P�[�V�������I��������
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType = appStopEvent;
            EvtAddEventToQueue(dummyEvent);
        }
        return (true);
    }

    // �}�N���f�[�^�̕ۑ�
    ret = WriteMacroDBData(DBNAME_MACROSCRIPT,
                           DBVERSION_MACROSCRIPT,
                           DATA_CREATOR_ID,
                           0, 
                           NNshGlobal->currentScriptLine,
                           NNshGlobal->scriptArea);
    if (ret == errNone)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_WRITE_OK, DBNAME_MACROSCRIPT, ret);

        // NNsj���I�������邩�ǂ����m�F����
#ifdef USE_CONFIRM_NNSJ_END
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_APLEND, "", 0);
        if (butID == 0)
#endif
        {
            // �A�v���P�[�V�������I��������
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType = appStopEvent;
            EvtAddEventToQueue(dummyEvent);
        }
        NNshGlobal->editFlag = false;
        return (true);
    }
    NNsh_DebugMessage(ALTID_ERROR, "Write Failure :", DBNAME_MACROSCRIPT, ret);
    return (false);
}


/*=========================================================================*/
/*   Function : deleteAllScript                                            */
/*                                                     �X�N���v�g�̑S�폜  */
/*=========================================================================*/
Boolean deleteAllScript(void)
{
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_ALLDELETE, "", 0) == 0)
    {
        NNshGlobal->currentScriptLine = 0;
        
        MemSet(NNshGlobal->scriptArea,
           (sizeof(NNshMacroRecord) * (NNshGlobal->maxScriptLine) + MARGIN),
           0x00);

        // ��ʂ��ĕ`�悷��
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        NNshGlobal->editFlag = true;
    }
    return (true);
}

/*=========================================================================*/
/*   Function : outputMemoPad                                              */
/*                                                       �������֏o�͂���  */
/*=========================================================================*/
void outputMemoPad(void)
{
    UInt16           loop,    len;
    Char            *dataPtr, *ptr;
    NNshMacroRecord *lineData;

    // �f�[�^�̏�����
    dataPtr = MemPtrNew_NNsh(NNSH_WORKBUF_DEFAULT + MARGIN);
    if (dataPtr == NULL)
    {
        // �������m�ۃG���[�����A�A
        return;
    }
    MemSet(dataPtr, (NNSH_WORKBUF_DEFAULT + MARGIN), 0x00);

    // �ꎞ�I�Ƀ��[�N�o�b�t�@���o�͂���
    ptr = dataPtr;
    len = NNSH_WORKBUF_DEFAULT;
    lineData = (NNshMacroRecord *) NNshGlobal->scriptArea;
    for (loop = 0; loop <= NNshGlobal->currentScriptLine; loop++)
    {
        PrintMacroLine(ptr, len, (loop + 1), &lineData[loop]);
        StrCat(ptr, "\n");
        ptr = dataPtr + StrLen(dataPtr);
        len = NNSH_WORKBUF_DEFAULT - (ptr - dataPtr);       
    }
    SendToLocalMemopad("Script", dataPtr);

    MemPtrFree(dataPtr);
    dataPtr = NULL;
    return;
}

/*=========================================================================*/
/*   Function : Handler_ViewLineAction                                     */
/*                                                       �C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_KeyDownEvt(EventType *event)
{
    UInt16 command;

    // �R�}���h��������
    command = NNSH_STEP_REDRAW;

    switch (KeyConvertFiveWayToJogChara(event))
    {
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrNextField:
      case vchrThumbWheelDown:
      case chrDownArrow:
        // 1�y�[�W��
        command = NNSH_STEP_PAGEDOWN;
        break;

      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrPrevField:
      case vchrThumbWheelUp:
      case chrUpArrow:
        // 1�y�[�W��
        command = NNSH_STEP_PAGEUP;
        break;

      case chrCapital_R:  // R�L�[�̓���
      case chrSmall_R:
      case chrCapital_L:  // L�L�[�̓���
      case chrSmall_L:
        // �}�N���f�[�^�̓ǂݍ���
        NNsh_FileOperation(BTNID_DB_READ);
        // ��ʂ��ĕ`�悷��
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        break;

      case chrCapital_S:  // S�L�[�̓���
      case chrSmall_S:
        // �}�N���f�[�^�̏�������
        NNsh_FileOperation(BTNID_DB_SAVE);
        // ��ʂ��ĕ`�悷��
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        break;

      case chrCapital_F:  // F�L�[�̓���
      case chrSmall_F:
        // �\���t�H���g�̕ύX
        return (changeDispFont());
        break;

      // HandEra JOG Push�I��/Enter�L�[���͎�
      case vchrJogRelease:
      case chrCarriageReturn:
      case chrLineFeed:
      case vchrThumbWheelPush:
        // �}�N���f�[�^�̐ݒ�
        return (extractMacroData());
        break;

      default:
        // ��L�ȊO�̃L�[�ł͉������Ȃ�
        break;
    }
    if (command != NNSH_STEP_REDRAW)
    {
        // �y�[�W���ړ�������
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, BTNID_FIELDUP, BTNID_FIELDDOWN, command);
        return (true);
    }
    // �������Ȃ�
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_RctPointEvt                                         */
/*                                       ���C���t�H�[���̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_RctPointEvt(EventType *event, RectangleType *dimF)
{
    Boolean          edit = false;
    UInt16           fontHeight, nlines, line;
    Coord            pointX, pointY, dimX, dimY;
    NNshMacroRecord *source;

    // �t�H���g�������擾����
    fontHeight   = FntCharHeight();
    if (fontHeight == 0)
    {
        // �t�H���g���ُ�A�I������
        return (false);
    }

    // ���W�n�̕ϊ�(�|�C���g)
    pointY = event->screenY - dimF->topLeft.y;
    pointX = event->screenX - dimF->topLeft.x;
    NNsi_ConvertLocation(&pointX, &pointY);

    // ���W�n�̕ϊ�
    dimX   = dimF->extent.x;
    dimY   = dimF->extent.y;
    NNsi_ConvertLocation(&dimX, &dimY);
    
    // �𑜓x�ɂ��킹�ĕ\���\�s��(nlines)���v�Z
    nlines = ((dimY) / (fontHeight));

    // �f�[�^�̊i�[����Ă���̈���擾����
    source = (NNshMacroRecord *) NNshGlobal->scriptArea;

    // �^�b�v���ꂽ���C�������s�ڂ��v�Z����
    line = (pointY / fontHeight) +  NNshGlobal->pageTopLine;
    if (line <= NNshGlobal->currentScriptLine)
    {
        // �ҏW���[�h�ɂ�蓮���ύX����
        switch (NNshGlobal->editMode)
        {
          case NNSH_MODE_NORMAL:
          default:
            // �ʏ�̕ҏW���[�h�A�^�b�v���ꂽ�s�̕ҏW�_�C�A���O���J��
            edit = OpCodeInputDialog((line + 1), &(source[line]));
            // ��ԍŌ�̍s��ҏW������A�P�s�ҏW�̈��ǉ�����B
            if ((edit == true)&&(line == NNshGlobal->currentScriptLine))
            {
                (NNshGlobal->currentScriptLine)++;
            }
            break;

          case NNSH_MODE_INSERT:
            // �}�����[�h
            if (NNshGlobal->currentScriptLine != NNshGlobal->maxScriptLine)
            {
                MemMove(&source[line + 1], &source[line], 
                        sizeof(NNshMacroRecord) * (NNshGlobal->currentScriptLine - line));
                MemSet(&source[line], sizeof(NNshMacroRecord), 0x00);
                (NNshGlobal->currentScriptLine)++;
                edit = true;
            }
            break;

          case NNSH_MODE_DELETE:
            // �폜���[�h
            if (NNshGlobal->currentScriptLine != 0)
            {
                MemMove(&source[line], &source[line + 1], 
                    sizeof(NNshMacroRecord) * (NNshGlobal->currentScriptLine - line));
                (NNshGlobal->currentScriptLine)--;
                if (NNshGlobal->pageTopLine > NNshGlobal->currentScriptLine)
                {
                    // ��ʂ��P�y�[�W��ɂ���
                    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                                 BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_PAGEUP);
                }
                edit = true;
            }
            break;
        }
    }
    if (NNshGlobal->editMode != NNSH_MODE_NORMAL)
    {
        // �t�B�[���h�ɒʏ�f�[�^��������
        NNshGlobal->editMode = NNSH_MODE_NORMAL;
        NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DB_INFO, true, NNshGlobal->infoArea, MINIBUF);
    }
    if (edit == true)
    {
        // ��ʂ�ҏW�������Ƃɂ���
        NNshGlobal->editFlag = true;
    }

    // ��ʂ��ĕ`�悷��
    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
    return (edit);
}

/*=========================================================================*/
/*   Function :   NNsh_MainCtlEvt                                          */
/*                                       ���C���t�H�[���̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_MainCtlEvt(EventType *event)
{
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_SET_NNSIMACRO:
        // �}�N���f�[�^�̐ݒ�
        return (extractMacroData());
        break;

      case BTNID_DB_SAVE:
        // �}�N���ۑ�
        NNsh_FileOperation(BTNID_DB_SAVE);
        // ��ʂ��ĕ`�悷��
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        break;

      case BTNID_DB_READ:
        // �}�N���ǂݍ���
        NNsh_FileOperation(BTNID_DB_READ);
        // ��ʂ��ĕ`�悷��
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        break;

      case BTNID_INSERTLINE:
        // 1�s�}��
        insertOneLine();
        break;

      case BTNID_DELETELINE:
        // 1�s�폜
        deleteOneLine();
        break;

  
      default:
        // ���̑�
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_MainCtlRepEvt                                       */
/*                                       ���C���t�H�[���̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_MainCtlRepEvt(EventType *event)
{
    UInt16 command;

    // �R�}���h��������
    command = NNSH_STEP_REDRAW;

    switch (event->data.ctlRepeat.controlID)
    {
      case BTNID_FIELDUP:
        // 1�y�[�W��
        command = NNSH_STEP_PAGEUP;
        break;

      case BTNID_FIELDDOWN:
        // 1�y�[�W��
        command = NNSH_STEP_PAGEDOWN;
        break;
  
      default:
        // ���̑�
        break;
    }
    if (command != NNSH_STEP_REDRAW)
    {
        // �y�[�W���ړ�������
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                                      BTNID_FIELDUP, BTNID_FIELDDOWN, command);
        return (true);
    }
    return (false);

}

/*=========================================================================*/
/*   Function :   NNsh_MainMenuEvt                                         */
/*                                       ���C���t�H�[���̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_MainMenuEvt(EventType *event)
{

    switch (event->data.menu.itemID)
    {
      case MNUID_VERSION:
        // �o�[�W�����̕\��
        ShowVersion_NNsh();
        break;
      case MNUID_CHANGEFONT:
        // �\���t�H���g�̕ύX
        return (changeDispFont());
        break;

      case MNUID_ALLDELETE:
        // �S�폜
        return (deleteAllScript());
        break;

      case MNUID_INSERT:
        // 1�s�}��
        insertOneLine();
        break;

      case MNUID_DELETE:
        // 1�s�폜
        deleteOneLine();
        break;

      case MNUID_OUTPUT_MEMOPAD:
        // �������֏o��
        outputMemoPad();
        break;

      default:
        // �������Ȃ�
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Hander_MainForm                                          */
/*                                     NNsi�ݒ�n��(����)�C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_MainForm(EventType *event)
{
    FormType      *frm;
    RectangleType  dimF;

    switch (event->eType)
    { 
      case menuEvent:
        // ���j���[��I��
        return (NNsh_MainMenuEvt(event));
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ
        return (NNsh_MainCtlEvt(event));
        break;

      case ctlRepeatEvent:
        // �{�^���������ꂽ
        return (NNsh_MainCtlRepEvt(event));
        break;

      case penDownEvent:
        frm = FrmGetActiveForm();
        FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_GADGET_FIELD),&dimF);
        if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
        {
            // Gadget�����^�b�v����
            return (NNsh_RctPointEvt(event, &dimF));
        }
        return (false);
        break;
      case keyDownEvent:
        // key�������ꂽ
        return (NNsh_KeyDownEvt(event));
        break;

      default: 
        return (false);
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_MainForm                                        */
/*                                                      MAIN�t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_MainForm(FormType *frm)
{
    RectangleType dimF;    

    // �t�B�[���h�Ƀ_�~�[�f�[�^��������
    NNshWinSetFieldText(frm, FLDID_DB_INFO, true, NNshGlobal->infoArea, MINIBUF);
    
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, GADID_GADGET_FIELD), &dimF);

    NNsi_putLine(frm, GADID_GADGET_FIELD, BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_TO_TOP);

    return (errNone);
}
