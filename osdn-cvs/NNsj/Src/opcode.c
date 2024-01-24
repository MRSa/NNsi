/*============================================================================*
 *  FILE: 
 *     opcode.c
 *
 *  Description: 
 *     �I�y�R�[�h���̓_�C�A���O
 *
 *===========================================================================*/
#define OPCODE_C

#include "local.h"



typedef struct
{
   UInt16  opCode;
   Boolean showDst;
   Boolean showSrc;
   UInt16  listID;
   Boolean showNumeric;
   Boolean showString;
} OpCodeShowTbl;

OpCodeShowTbl NNsiMacroOpTbl[] = 
{
  {  NNSH_MACROCMD_MACROEND,  false, false, 0, false, false },
  {  NNSH_MACROCMD_MOVE,      true,  true,  0, true,  false },
  {  NNSH_MACROCMD_ADD,       true,  true,  0, true,  false },
  {  NNSH_MACROCMD_SUB,       true,  true,  0, true,  false },
  {  NNSH_MACROCMD_AND,       true,  true,  0, true,  false },
  {  NNSH_MACROCMD_OR,        true,  true,  0, true,  false },
  {  NNSH_MACROCMD_XOR,       true,  true,  0, true,  false },
  {  NNSH_MACROCMD_MESSAGE,   false, true,  0, false, true  },
  {  NNSH_MACROCMD_EXECUTE,   false, false, MULTIBTN_FEATUREMSG_STRINGS, false, false },
  {  NNSH_MACROCMD_CMP,       true,  true,  0, true,  false },
  {  NNSH_MACROCMD_JMP,       false, true,  0, true,  false },
  {  NNSH_MACROCMD_JZ,        true,  true,  0, true,  false },
  {  NNSH_MACROCMD_JNZ,       true,  true,  0, true,  false },
  {  NNSH_MACROCMD_BEEP,      false, true,  0, true,  true  },
  {  NNSH_MACROCMD_BBSTYPE,   true,  false, 0, false, false },
  {  NNSH_MACROCMD_STRSTR,    true,  false, 0, false, true  },
  {  NNSH_MACROCMD_STRCMP,    true,  false, 0, false, true  },
  {  NNSH_MACROCMD_SETTABNUM, true,  false, 0, false, false },
  {  NNSH_MACROCMD_GETMESNUM, true,  false, 0, false, false },
  {  NNSH_MACROCMD_MESSTATUS, true,  false, 0, false, false },
  {  NNSH_MACROCMD_IMPORT,    false, false, 0, false, false },
  {  NNSH_MACROCMD_EXPORT,    false, false, 0, false, false },
  {  NNSH_MACROCMD_OPERATE,   true,  true,  FEATURE_NNSISETTING_STRINGS, true,   false },
  {  NNSH_MACROCMD_STATUS,    true,  false, FEATURE_NNSISETTING_STRINGS, false,  false },
  {  NNSH_MACROCMD_LAUNCHDA,  false, false, 0, false, true  },
  {  NNSH_MACROCMD_CLIPCOPY,  false, false, 0, false, true  },
  {  NNSH_MACROCMD_CLIPADD,   false, false, 0, false, true  },
  {  NNSH_MACROCMD_CLIPINSTR, true,  false, 0, false, true  },
  {  NNSH_MACROCMD_TITLECLIP, false, false, 0, false, false },
  {  NNSH_MACROCMD_SET_FINDBBSSTR, false, false, 0, false, false },
  {  NNSH_MACROCMD_SET_FINDTHREADSTR, false, false, 0, false, false },
  {  NNSH_MACROCMD_UPDATE_RESNUM, true, false, 0, false, false },
  {  NNSH_MACROCMD_JPL,       true,  true,  0, true,  false },
  {  NNSH_MACROCMD_JMI,       true,  true,  0, true,  false },
  {  NNSH_MACROCMD_OUTPUTTITLE, false, false, 0, false, false },
  {  NNSH_MACROCMD_OUTPUTCLIP, false, false, 0, false, false },
  {  NNSH_MACROCMD_REMARK,     false, false, 0, false, true },
  {  NNSH_MACROCMD_PUSH,       false, true,  0, true,  false }, 
  {  NNSH_MACROCMD_POP,        true,  false, 0, false,  false },
  {  NNSH_MACROCMD_GETRECNUM,  true,  false, 0, false, false },
  {  NNSH_MACROCMD_FINDTHREADSTRCOPY, false, false, 0, false, false },
  {  NNSH_MACROCMD_INPUTDIALOG,   false, false, 0, false, true },
  {  NNSH_MACROCMD_OUTPUTDIALOG,  false, false, 0, false, true },
  {  NNSH_MACROCMD_CONFIRMDIALOG, true,  false, 0, false, false },
  {  NNSH_MACROCMD_MESATTRIBUTE,  true,  false, 0, false, false },
  {  NNSH_MACROCMD_JUMPLAST,      false, true,  0, true,  false },
  {  0xffff, false, false, 0, false, false }
};

/*=========================================================================*/
/*   Function :  convertFromNNsiSetString                                  */
/*                                ���X�g�ԍ�����NNsi�ݒ�擾�̃f�[�^�ɕϊ� */
/*=========================================================================*/
static UInt16 convertFromNNsiSetString(UInt16 item)
{
    UInt16 num;

    switch (item)
    {
      case 0:
        num = NNSH_MACROOPCMD_DISABLE;
        break;

      case 1:
        num = NNSH_MACROOPCMD_AUTOSTARTMACRO;
        break;

      case 2:
        num = NNSH_MACROOPCMD_NEWARRIVAL_LINE;
        break;
   
      case 3:
        num = NNSH_MACROOPCMD_NEWARRIVAL_DA;
        break;   

      case 4:
        num = NNSH_MACROOPCMD_NEWARRIVAL_ROLOG; 
        break;   

      case 5:
        num = NNSH_MACROOPCMD_NEWARRIVAL_BEEP;
        break;

      case 6:
        num = NNSH_MACROOPCMD_COPY_AND_DELETE;
        break;  

      default:
        num = 0;
        break;
    }
    return (num);
}

/*=========================================================================*/
/*   Function :  convertNNsiSetString                                      */
/*                                  NNsi�ݒ�擾�̃f�[�^�����X�g�ԍ��ɕϊ� */
/*=========================================================================*/
static UInt16 convertNNsiSetString(UInt16 item)
{
    UInt16 num;

    switch (item)
    {
      case NNSH_MACROOPCMD_DISABLE:
        num = 0; 
        break;   

      case NNSH_MACROOPCMD_AUTOSTARTMACRO:
        num = 1; 
        break;   

      case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
        num = 2; 
        break;
   
      case NNSH_MACROOPCMD_NEWARRIVAL_DA:
        num = 3; 
        break;   

      case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
        num = 4; 
        break;   

      case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
        num = 5; 
        break;   

      case NNSH_MACROOPCMD_COPY_AND_DELETE:
        num = 6; 
        break;   

      default:
        num = 0; 
        break;   
    }
    return (num);
}

/*=========================================================================*/
/*   Function :   showHideRegister                                         */
/*                                                      ���W�X�^�ԍ���ݒ� */
/*=========================================================================*/
static void showHideRegister(FormType *frm, Boolean disp, UInt16 regNum, UInt16 lblId, UInt16 popId, UInt16 lstId)
{
    UInt16 objIndex;

    if (disp == true)
    {
        objIndex = FrmGetObjectIndex(frm, popId);
        FrmShowObject(frm, objIndex);

        objIndex = FrmGetObjectIndex(frm, lblId);
        FrmShowObject(frm, objIndex);

        // ���W�X�^�ԍ����Ó����m�F
        if ((regNum >= NNSH_MACROOP_GR_OFFSET)&&(regNum <= NNSH_MACROOP_GR_MAX))
        {
            regNum = regNum - NNSH_MACROOP_GR_OFFSET + 1;
        }
        else
        {
            regNum = 0;
        }    
        NNshWinSetPopItems(frm, popId, lstId, regNum);
    }
    else
    {
        // ���W�X�^�ԍ���\�����Ȃ�
        objIndex = FrmGetObjectIndex(frm, popId);
        FrmHideObject(frm, objIndex);

        objIndex = FrmGetObjectIndex(frm, lblId);
        FrmHideObject(frm, objIndex);
    }
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
    *lst = MemPtrNew(sizeof(NNshWordList));
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
/*=========================================================================*/
/*   Function :   ControlOpSetDataItem                                     */
/*                                        OP�R�[�h�p�œ��͉\�f�[�^��ݒ� */
/*=========================================================================*/
static void ControlOpSetDataItem(FormType *frm, OpCodeShowTbl *item, NNshMacroRecord *dataRec)
{
    ListType *lstP;
    Int32     num;
    UInt16    data;
    Char      bufNum[MINIBUF + MARGIN];
    
    // dst���W�X�^
    if (item->showDst == true)
    {
        lstP   = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, LSTID_DESTINATION));
        dataRec->dst = LstGetSelection(lstP);
        if (dataRec->dst != 0)
        {
            dataRec->dst = dataRec->dst + NNSH_MACROOP_GR_OFFSET - 1;
        }
    }

    // src���W�X�^
    if (item->showSrc == true)
    {
        lstP   = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, LSTID_SOURCE));
        dataRec->src = LstGetSelection(lstP);
        if (dataRec->src != 0)
        {
            dataRec->src = dataRec->src + NNSH_MACROOP_GR_OFFSET - 1;
        }
    }

    // ���l��ݒ�
    MemSet(bufNum, sizeof(bufNum), 0x00);
    NNshWinGetFieldText(frm, FLDID_VALUEFIELD, bufNum, MINIBUF);
    num = StrAToI(bufNum);
    dataRec->MacroData.numData.operator = num;
    
    // �������ݒ�
    MemSet(bufNum, sizeof(bufNum), 0x00);
    NNshWinGetFieldText(frm, FLDID_INPUTFIELD, bufNum, MINIBUF);
    StrNCopy(dataRec->MacroData.strData.data, bufNum, MAX_STRLEN);

    // DA�̐ݒ���擾
    if (item->opCode == NNSH_MACROCMD_LAUNCHDA)
    {
        dataRec->MacroData.strData.data[2] = bufNum[0];
        dataRec->MacroData.strData.data[3] = bufNum[1];
        dataRec->MacroData.strData.data[4] = bufNum[2];
        dataRec->MacroData.strData.data[5] = bufNum[3];
    }

    // ����
    switch (item->listID)
    {
      case FEATURE_NNSISETTING_STRINGS:
        lstP   = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, LSTID_OPEFUNC));
        data   = LstGetSelection(lstP);
        dataRec->MacroData.numData.subCommand = convertFromNNsiSetString(data);
        break;

      case MULTIBTN_FEATUREMSG_STRINGS:
        lstP   = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, LSTID_OPEFUNC));
        data   = LstGetSelection(lstP);
        dataRec->MacroData.numData.operator = data;
        break;

      case 0:
      default:
        // �������Ȃ�
        break;
    }

    return;
}

/*=========================================================================*/
/*   Function :   ControlOpDataItem                                        */
/*                                        OP�R�[�h�p�œ��͉\�f�[�^��ݒ� */
/*=========================================================================*/
static void ControlOpDataItem(FormType *frm, OpCodeShowTbl *item, NNshMacroRecord *dataRec)
{
    UInt16 objIndex, selection;
    Char   bufNum[MINIBUF + MARGIN];

    showHideRegister(frm, item->showDst, dataRec->dst, LABEL_DESTINATION, POPTRID_DESTINATION, LSTID_DESTINATION);
    showHideRegister(frm, item->showSrc, dataRec->src, LABEL_SOURCE,      POPTRID_SOURCE,      LSTID_SOURCE);

    if (NNshGlobal->featureList != NULL)
    {
        // �@�\�ݒ胊�X�g�����ɒ�`����Ă����ꍇ�͂�������̈���������
        ReleaseWordList(NNshGlobal->featureList);
        MEMFREE_PTR(NNshGlobal->featureList);
        NNshGlobal->featureList = NULL;
    }

    if (item->showNumeric == true)
    {
        objIndex = FrmGetObjectIndex(frm, LABEL_VARIABLE_VALUE);
        FrmShowObject(frm, objIndex);

        objIndex = FrmGetObjectIndex(frm, FLDID_VALUEFIELD);
        FrmShowObject(frm, objIndex);

        // ���l
        MemSet (bufNum, sizeof(bufNum), 0x00);
        NUMCATI(bufNum, dataRec->MacroData.numData.operator);
        
        NNshWinSetFieldText(frm, FLDID_VALUEFIELD, false, bufNum, (StrLen(bufNum) + MARGIN));
    }
    else
    {
        // ���l����͂��Ȃ�
        objIndex = FrmGetObjectIndex(frm, LABEL_VARIABLE_VALUE);
        FrmHideObject(frm, objIndex);

        objIndex = FrmGetObjectIndex(frm, FLDID_VALUEFIELD);
        FrmHideObject(frm, objIndex);
    }
    
    if (item->showString == true)
    {
        objIndex = FrmGetObjectIndex(frm, LABEL_VARIABLE_STRING);
        FrmShowObject(frm, objIndex);

        objIndex = FrmGetObjectIndex(frm, FLDID_INPUTFIELD);
        FrmShowObject(frm, objIndex);

        // �������\��
        if (item->opCode == NNSH_MACROCMD_LAUNCHDA)
        {
            // DA�̃N���G�[�^ID��ݒ�
            NNshWinSetFieldText(frm, FLDID_INPUTFIELD, false, 
                                &dataRec->MacroData.strData.data[2],
                                (MAX_STRLEN + MARGIN));
        }
        else
        {
            NNshWinSetFieldText(frm, FLDID_INPUTFIELD, false, 
                            dataRec->MacroData.strData.data, (MAX_STRLEN + MARGIN));
        }
    }
    else
    {
        // ���������͂��Ȃ�
        objIndex = FrmGetObjectIndex(frm, LABEL_VARIABLE_STRING);
        FrmHideObject(frm, objIndex);

        objIndex = FrmGetObjectIndex(frm, FLDID_INPUTFIELD);
        FrmHideObject(frm, objIndex);
    }

    if ((item->showNumeric == false)&&(item->showString == false))
    {
        // ���l�w��ł��Ȃ��ꍇ�A���x��������
        objIndex = FrmGetObjectIndex(frm, LABEL_VARIABLE_SET);
        FrmHideObject(frm, objIndex);
    }
    else
    {
        objIndex = FrmGetObjectIndex(frm, LABEL_VARIABLE_SET);
        FrmShowObject(frm, objIndex);
    }

    selection = dataRec->MacroData.numData.operator;
    switch (item->listID)
    {
      case FEATURE_NNSISETTING_STRINGS:
        selection = convertNNsiSetString(dataRec->MacroData.numData.subCommand);
        // not break!!

      case MULTIBTN_FEATUREMSG_STRINGS:
        objIndex = FrmGetObjectIndex(frm, LABEL_OPEFUNC);
        FrmShowObject(frm, objIndex);

        objIndex = FrmGetObjectIndex(frm, POPTRID_OPEFUNC);
        FrmShowObject(frm, objIndex);

        // �@�\�ݒ胊�X�g���`����
        createFeatureListStrings(&(NNshGlobal->featureList), item->listID);
        if (NNshGlobal->featureList != NULL)
        {
            NNshWinSetPopItemsWithList(frm, POPTRID_OPEFUNC, LSTID_OPEFUNC,
                                       NNshGlobal->featureList, selection);
        }
        break;

      case 0:
      default:
        // ���샊�X�g��\�����Ȃ�
        objIndex = FrmGetObjectIndex(frm, LABEL_OPEFUNC);
        FrmHideObject(frm, objIndex);

        objIndex = FrmGetObjectIndex(frm, POPTRID_OPEFUNC);
        FrmHideObject(frm, objIndex);
        break;
    }

    return;
}

/*=========================================================================*/
/*   Function : prepareOpCodeDialog                                        */
/*                                                 �t�H�[���Ƀf�[�^�𔽉f  */
/*=========================================================================*/
void prepareOpCodeDialog(FormType *frm, NNshMacroRecord *dataRec, UInt16 opCode)
{
    OpCodeShowTbl *dataPtr;

    // ���߂ɂ���āA��ʂɕ\������A�C�e����ύX����
    dataPtr = NNsiMacroOpTbl;
    while (dataPtr->opCode != 0xffff)
    {
        if (dataPtr->opCode == opCode)
        {
            ControlOpDataItem(frm, dataPtr, dataRec);
            return;
        }
        dataPtr++;
    }
    return;
}

/*=========================================================================*/
/*   Function :   Handler_opCodeAction                                     */
/*                                            OP�R�[�h�p�̃C�x���g�n���h�� */
/*=========================================================================*/
Boolean Handler_opCodeAction(EventType *event)
{
    FormType        *frm;
    NNshMacroRecord *dataRec;

    switch (event->eType)
    { 
      case menuEvent:
        // ���j���[��I��
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ
        break;

      case ctlRepeatEvent:
        // �{�^���������ꂽ
        break;

      case keyDownEvent:
        // key�������ꂽ
        break;

      case popSelectEvent:
        // �|�b�v�A�b�v�g���K�[���I�����ꂽ
        if (event->data.popSelect.listID == LSTID_OP_CODE)
        {
            frm = FrmGetActiveForm();
            dataRec = NNshGlobal->dataRec;
            prepareOpCodeDialog(frm, dataRec, event->data.popSelect.selection);
            return (false);
        }
        break;

      case lstSelectEvent:
        break;

      case lstEnterEvent:
        break;

      default: 
        return (false);
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function : effectOpCodeData                                           */
/*                                                �t�H�[������f�[�^���擾 */
/*=========================================================================*/
void effectOpCodeData(FormType *frm, NNshMacroRecord *dataRec)
{
    UInt16         opCode;
    ListType      *lstP;
    OpCodeShowTbl *dataPtr;

    // OP�R�[�h���擾����
    lstP   = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, LSTID_OP_CODE));
    opCode = LstGetSelection(lstP);


    // ���߂ɂ���āA��ʂɕ\�����Ă���A�C�e������肷��
    dataPtr = NNsiMacroOpTbl;
    while (dataPtr->opCode != 0xffff)
    {
        if (dataPtr->opCode == opCode)
        {
            MemSet(dataRec, sizeof(NNshMacroRecord), 0x00);
            dataRec->opCode = opCode;
            ControlOpSetDataItem(frm, dataPtr, dataRec);
            return;
        }
        dataPtr++;
    }
    return;
}


/*=========================================================================*/
/*   Function : OpCodeInputDialog                                          */
/*                                                ��s�f�[�^���̓_�C�A���O */
/*=========================================================================*/
Boolean OpCodeInputDialog(UInt16 lineNum, NNshMacroRecord *dataRec)
{
    Boolean       ret = false;
    UInt16        btnId;
    FormType     *prevFrm, *diagFrm;
    Char         *lineData, title[MINIBUF + MARGIN];

    // �\���s�̃f�[�^�����H
    MemSet(title, sizeof(title), 0x00);
    lineData = MemPtrNew(BUFSIZE + MARGIN);
    if (lineData == NULL)
    {
        return (false);
    }
    MemSet(lineData, (BUFSIZE + MARGIN), 0x00);
    PrintMacroLine(lineData, BUFSIZE, lineNum, dataRec);

    // �^�C�g���̐ݒ�
    StrCat(title, "Edit line No.");
    NUMCATI(title, lineNum);

    // �ҏW�Ώۃf�[�^���O���[�o���̈�ɒu��
    NNshGlobal->dataRec = dataRec;

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // �_�C�A���O�t�H�[�����A�N�e�B�u�ɂ���
    diagFrm  = FrmInitForm(FRMID_OPCODEINPUT);
    if (diagFrm == NULL)
    {
        return (false);
    }
    FrmSetActiveForm(diagFrm);

    // ���C���̏���\������
    NNshWinSetFieldText(diagFrm, FLDID_OPECODE, false, lineData, BUFSIZE);

    // ���߂��|�b�v�A�b�v�g���K�[�A�C�e���ɔ��f������
    NNshWinSetPopItems(diagFrm, POPTRID_OP_CODE, LSTID_OP_CODE, dataRec->opCode);

    // �f�[�^�̉�ʕ\���ݒ�
    prepareOpCodeDialog(diagFrm, dataRec, dataRec->opCode);

    // �V���N�\�����ő剻
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(diagFrm, vgaFormModify160To240);
        SilkMaximizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMaximize(diagFrm);
    }
    HandEraMoveDialog(diagFrm);

    // �^�C�g�����R�s�[����
    FrmCopyTitle(diagFrm, title);

    FrmSetEventHandler(diagFrm, Handler_opCodeAction);

    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_VIEW_OK)
    {
        // OK�{�^���������ꂽ�Ƃ��ɂ́A�f�[�^�𔽉f������
        effectOpCodeData(diagFrm, dataRec);
        ret = true;
    }
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // �V���N�\�����ŏ���
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMinimize(prevFrm);
    }

    MemPtrFree(lineData);
    return (ret);
}
