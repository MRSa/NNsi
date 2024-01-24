/*============================================================================*
 *  FILE: 
 *     macroview.c
 *
 *  Description: 
 *     �}�N���f�[�^�̕\���p���H����
 *
 *===========================================================================*/
#define MACROVIEW_C
#include "local.h"

/*=========================================================================*/
/*   Function : setSourceDataDisplay                                       */
/*                                        �\�[�X�f�[�^���e���o�b�t�@�ɏo�� */
/*=========================================================================*/
static UInt16 setSourceDataDisplay(Char *buffer, NNshMacroRecord *lineData)
{
    if ((lineData->src >= NNSH_MACROOP_GR_OFFSET)&&
         (lineData->src <= NNSH_MACROOP_GR_MAX))
    {
        StrCat (buffer, "gr");
        NUMCATI(buffer, lineData->src - NNSH_MACROOP_GR_OFFSET);
        StrCat (buffer, " ");
        return (NNSH_MACROOPCMD_DISABLE);
    }

    // ��R��������l���擾����
    NUMCATI(buffer, lineData->MacroData.numData.operator);
    return (lineData->MacroData.numData.operator);
}

/*=========================================================================*/
/*   Function : setExecFuncDataDisplay                                     */
/*                                                        �@�\�֐����̎擾 */
/*=========================================================================*/
static void setExecFuncDataDisplay(Char *buffer, UInt16 command)
{
    Char        *ptr, **kw, buf[MAX_FUNCCHAR_BUF];
    MemHandle    memH;
    NNshWordList wordList;

    // �o�b�t�@�̃N���A
    MemSet(buf, sizeof(buf), 0x00);
    MemSet(&wordList, sizeof(wordList), 0x00);
    
    // �ꗗ�@�\���X�g�̎擾�i�X�g�����O���\�[�X���擾�j
    memH = DmGetResource('tSTR', MULTIBTN_FEATUREMSG_STRINGS);
    if (memH == 0)
    {
        return;
    }
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        return;
    }

    // �L�[���[�h��؂�o���āA�ꎞ�o�b�t�@�ɃR�s�[����
    SeparateWordList(ptr, &wordList);
    kw = MemHandleLock(wordList.wordmemH);
    StrNCopy(buf, kw[command], MAX_FUNCCHAR_BUF - 1);
    MemHandleUnlock(wordList.wordmemH);
    ReleaseWordList(&wordList);
    
    // ��������R�s�[
    StrCat(buffer, buf);

    MemHandleUnlock(memH);
    DmReleaseResource(memH);
    return;
}

/*=========================================================================*/
/*   Function : printMacroOpeR                                             */
/*                                              �}�N���f�[�^�̃f�[�^�ݒ蕔 */
/*=========================================================================*/
static void printMacroOpeR(Char *buf, NNshMacroRecord *data, Char *operator)
{
    // NNsi�ݒ�l�̎擾/�ݒ�
    switch (data->MacroData.numData.subCommand)
    {
      case NNSH_MACROOPCMD_AUTOSTARTMACRO:
        // �}�N���������s�̒l���擾
        StrCat(buf, " (autoMacro) ");
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
        // �V���m�F���s��ɉ���ؒf
        StrCat(buf, " (chkLINE) ");
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_DA:
        // �V���m�F���s���DA�N��
        StrCat(buf, " (chkDA) ");
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
        // �Q�Ɛ�p���O�擾���x��
        StrCat(buf, " (ROlogLvl) ");
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
        // �V���m�F�I����Ƀr�[�v��
        StrCat(buf, " (chkBEEP) ");
        break;

      case NNSH_MACROOPCMD_COPY_AND_DELETE:
        // �Q�Ɛ�p���O�R�s�[��ɍ폜�����{
        StrCat(buf, " (cp&rm) ");
        break;

      default:
        StrCat(buf, "nop ");
        break;
    }
    // ��f�[�^���o��
    if (operator != NULL)
    {
        StrCat(buf, operator);
    }
    return;
}

/*=========================================================================*/
/*   Function : printMacroOpeC                                             */
/*                                              �}�N���f�[�^�̕���(�W�J) */
/*=========================================================================*/
static void printMacroOpeC(Char *buf, NNshMacroRecord *data, Char *operator)
{
    if (operator != NULL)
    {
        StrCat (buf, "IF gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, operator);
        StrCat (buf, " THEN ");
    }
    StrCat (buf, "GOTO ");
    setSourceDataDisplay(buf, data);

    return;
}

/*=========================================================================*/
/*   Function : printMacroDst                                              */
/*                                            �}�N���f�[�^�̊i�[�ꏊ�\���� */
/*=========================================================================*/
static void printMacroDst(Char *buf, NNshMacroRecord *data, Char *operator)
{
    StrCat (buf, "gr");
    NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
    StrCat (buf, " := ");
    if (operator != NULL)
    {
        StrCat (buf, operator);
    }
    return;
}

/*=========================================================================*/
/*   Function : printMacroOpeA                                             */
/*                                          �}�N���f�[�^�̎Z�p���Z��(�W�J) */
/*=========================================================================*/
static void printMacroOpeA(Char *buf, NNshMacroRecord *data, Char *operator)
{
    printMacroDst(buf, data, NULL);
    if (operator != NULL)
    {
        // ���Z
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, operator);
    }
    setSourceDataDisplay(buf, data);

    return;
}

/*=========================================================================*/
/*   Function : PrintMacroLine                                             */
/*                                          �}�N���f�[�^(��s)�̃f�[�^�ݒ� */
/*=========================================================================*/
void PrintMacroLine(Char *buf, UInt16 size, UInt16 num, NNshMacroRecord *data)
{
    Char   logData[8];
    UInt16 command;

    // �̈�̏�����
    MemSet(buf, size, 0x00);
 
    // �s�ԍ���\��
    NUMCATI(buf, num);
    StrCat (buf, "  ");

    // op�R�[�h�̔���
    switch (data->opCode)
    {
      case NNSH_MACROCMD_XOR:
        // �l�� XOR ����
        printMacroOpeA(buf, data, " XOR ");
        break;

      case NNSH_MACROCMD_AND:
        // �l�� AND ����
        printMacroOpeA(buf, data, " AND ");
        break;

      case NNSH_MACROCMD_OR:
        // �l�� OR ����
        printMacroOpeA(buf, data, " OR ");
        break;

      case NNSH_MACROCMD_ADD:
        // �l�����Z����
        printMacroOpeA(buf, data, " + ");
        break;

      case NNSH_MACROCMD_CMP:
      case NNSH_MACROCMD_SUB:
        // �l�����Z����
        printMacroOpeA(buf, data, " - ");
        break;

      case NNSH_MACROCMD_MOVE:
        // �l��������
        printMacroOpeA(buf, data, NULL);
        break;

      case NNSH_MACROCMD_JNZ:
        // �[���łȂ������番�򂷂�
        printMacroOpeC(buf, data, "<> 0");
        break;

      case NNSH_MACROCMD_JZ:
        // �[���������番�򂷂�
        printMacroOpeC(buf, data, "= 0");
        break;

      case NNSH_MACROCMD_JMP:
        // �������ɕ��򂷂�
        printMacroOpeC(buf, data, NULL);
        break;

      case NNSH_MACROCMD_EXECUTE:
        // �@�\�̎��s
        StrCat (buf, "CALL (");
        command = setSourceDataDisplay(buf, data);
        if (command != NNSH_MACROOPCMD_DISABLE)
        {
            StrCat (buf, ":");
            setExecFuncDataDisplay(buf, command);
            StrCat (buf, " ");
        }
        StrCat (buf, ") ");
        break;

      case NNSH_MACROCMD_MESSAGE:
        // ���O�o��
        StrCat (buf, "PRINT ");
        if ((data->src >= NNSH_MACROOP_GR_OFFSET)&&
            (data->src <= NNSH_MACROOP_GR_MAX))
        {
            // GRx ��\��
            StrCat (buf, "(gr");
            NUMCATI(buf, data->src - NNSH_MACROOP_GR_OFFSET);
            StrCat (buf, ") ");
        }
        else
        {
            // �������\��
            StrCat (buf, "\"");
            StrCat (buf, data->MacroData.strData.data);
            StrCat (buf, "\" ");
        }
        break;

      case NNSH_MACROCMD_BEEP:
        // �r�[�v����炷
        StrCat (buf, "BEEP ");
        setSourceDataDisplay(buf, data);
        StrCat (buf, "  ");
        break;

      case NNSH_MACROCMD_MACROEND:
        // �}�N���I��
        StrCat (buf, "END ");
        break;

      case NNSH_MACROCMD_SETTABNUM:
        // �^�u�����i�[
        printMacroDst(buf, data, "(tabNum)");
        break;

      case NNSH_MACROCMD_STRSTR:
        // �w�肵�������񂪃X���^�C�g���Ɋ܂܂�邩
        printMacroDst(buf, data, "strstr(title, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\")");
        break;            

      case NNSH_MACROCMD_STRCMP:
        // �w�肵�������񂪃X���^�C�g���ƈ�v���邩
        printMacroDst(buf, data, "strcmp(title, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\")");
        break;            

      case NNSH_MACROCMD_OPERATE:
        // NNsi�ݒ�l�̐ݒ�
        printMacroOpeR(buf, data, ":= ");
        setSourceDataDisplay(buf, data);
        StrCat(buf, ", save ");
        /** not break; **/

      case NNSH_MACROCMD_STATUS:
        // NNsi�ݒ�l�̎擾
        printMacroDst(buf, data, NULL);
        printMacroOpeR(buf, data, NULL);
        break;

      case NNSH_MACROCMD_GETMESNUM:
        // �X�����𔽉f
        printMacroDst(buf, data, "mesNum(title)");
        break;

      case NNSH_MACROCMD_MESSTATUS:
        // �X����Ԃ𔽉f
        printMacroDst(buf, data, "mesStatus(title)");
        break;

      case NNSH_MACROCMD_BBSTYPE:
        // �X����BBS�^�C�v�𔽉f
        printMacroDst(buf, data, "bbsType(title)");
        break;

      case NNSH_MACROCMD_IMPORT:
        // NNsi�ݒ�C���|�[�g
        StrCat (buf, "IMPORT '");
        StrCat (buf, FILE_XML_NNSISET);
        if (setSourceDataDisplay(buf, data) == 0)
        {
            StrCat (buf, "' from VFS");
        }
        else
        {
            StrCat (buf, "' from Palm");
        }
        break;

      case NNSH_MACROCMD_EXPORT:
        // NNsi�ݒ�G�N�X�|�[�g
        StrCat (buf, "EXPORT '" FILE_XML_NNSISET "' ");
        break;

      case NNSH_MACROCMD_LAUNCHDA:
        // DA�N��
        StrCat (buf, "LAUNCH '");
        logData[0] = ((0xff00 & data->MacroData.numData.data1) >> 8); 
        logData[1] = ((0x00ff & data->MacroData.numData.data1));
        logData[2] = ((0xff00 & data->MacroData.numData.data2) >> 8);
        logData[3] = ((0x00ff & data->MacroData.numData.data2));
        logData[4] = '\0';
        StrCat (buf, logData);            
        StrCat (buf, "' as DA");
        break;

      case NNSH_MACROCMD_CLIPCOPY:
        // �N���b�v�{�[�h�ɃR�s�[
        StrCat (buf, "COPY(clip, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\") ");
        break;

      case NNSH_MACROCMD_CLIPADD:
        // �N���b�v�{�[�h�ɒǉ�
        StrCat (buf, "APPEND(clip, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\") ");
        break;

      case NNSH_MACROCMD_CLIPINSTR:
        // �N���b�v�{�[�h�̕�����Ɋ܂܂�邩�m�F
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, " := strstr(clip, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\") ");
        break;

      case NNSH_MACROCMD_TITLECLIP:
        // �X���^�C�g�����N���b�v�{�[�h�ɃR�s�[
        StrCat(buf, "COPY(clip, title)");
        break;

      case NNSH_MACROCMD_SET_FINDBBSSTR:
        // �N���b�v�{�[�h����������������ɃR�s�[
        StrCat(buf, "COPY(findBBS, clip)");
        break;

      case NNSH_MACROCMD_SET_FINDTHREADSTR:
        // �N���b�v�{�[�h��������X���^�C����������ɃR�s�[
        StrCat(buf, "COPY(findTHREAD, clip)");
        break;

      case NNSH_MACROCMD_UPDATE_RESNUM:
        // ���X�����X�V���Ċi�[
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, " := updateMesNum(title) ");
        break;

      case NNSH_MACROCMD_JPL:
        // ���̒l�Ȃ�W�����v����
        printMacroOpeC(buf, data, "> 0");
        break;

      case NNSH_MACROCMD_JMI:
        // ���̒l�Ȃ�W�����v����
        printMacroOpeC(buf, data, "< 0");
        break;

      case NNSH_MACROCMD_OUTPUTTITLE:
        // �X���^�C�g���̃��O�o��
        StrCat(buf, "PRINT title");
        break;
        
      case NNSH_MACROCMD_OUTPUTCLIP:
        // �N���b�v�{�[�h�̃��O�o��
        StrCat(buf, "PRINT clip");
        break;

      case NNSH_MACROCMD_REMARK:
        // ���ߕ�
        StrCat(buf, "' ");
        StrCat (buf,  data->MacroData.strData.data);
        break;

      case NNSH_MACROCMD_PUSH:
        // �X�^�b�N�Ƀf�[�^PUSH
        StrCat (buf, "PUSH ");
        setSourceDataDisplay(buf, data);
        break;

      case NNSH_MACROCMD_POP:
        // �X�^�b�N����f�[�^POP
        StrCat(buf, "POP gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        break;

      case NNSH_MACROCMD_GETRECNUM:
        // ���R�[�h�ԍ����擾����
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, " := RecordIndex(title) ");
        break;

      case NNSH_MACROCMD_FINDTHREADSTRCOPY:
        // �X��������������N���b�v�{�[�h�ɃR�s�[
        StrCat (buf, "COPY(clip, findTHREAD)");
        break;

      case NNSH_MACROCMD_INPUTDIALOG:
        // ���������͂��� (1.05)
        StrCat (buf, "clip := INPUT \"");
        StrCat (buf, data->MacroData.strData.data);
        break;

      case NNSH_MACROCMD_OUTPUTDIALOG:
        // ��������o�͂��� (1.05)
        StrCat (buf, "OUTPUT \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\"");
        break;

      case NNSH_MACROCMD_CONFIRMDIALOG:
        // �N���b�v�{�[�h�̕������\�����m�F���� (1.05)
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, " := CONFIRM (clip)");
        break;

      case NNSH_MACROCMD_MESATTRIBUTE:
        // �X�������l�𔽉f (1.06)
        printMacroDst(buf, data, "mesAttribute(title)");
        break;

      case NNSH_MACROCMD_JUMPLAST:
        // �X���̍ŉ��s�Ȃ�W�����v���� (1.06)
        StrCat(buf, "IF isLast(threadList) THEN GOTO ");
        setSourceDataDisplay(buf, data);
        break;

      default:
        // �R�}���h�R�[�h�ُ�A�Ȃɂ����s���Ȃ�
        StrCat (buf, "  ");
        break;
    }
    return;
}
