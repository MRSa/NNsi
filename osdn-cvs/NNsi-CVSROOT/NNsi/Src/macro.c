/*============================================================================*
 *
 *  $Id: macro.c,v 1.34 2006/07/30 15:31:45 mrsa Exp $
 *
 *  FILE: 
 *     macro.c
 *
 *  Description: 
 *     NNsi Macro Executing
 *
 *===========================================================================*/
#define MACRO_C
#include "local.h"

#ifdef USE_MACRO
static UInt16 getMacroValue(NNshMacroRecord *lineData, UInt16 *gr);
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : outputMacroLog                                             */
/*                                                �}�N���p�̃��O���o�͂��� */
/*=========================================================================*/
static void outputMacroLog(DmOpenRef dbRefW, UInt8 level, UInt16 code, Char *data)
{
    NNshMacroResult  logRec;

    MemSet(&logRec, sizeof(logRec), 0x00);
    logRec.dateTime = TimGetSeconds();
    logRec.logLevel = level;
    logRec.errCode  = code;
    StrNCopy(logRec.logData, data, MAX_LOGRECLEN);
    EntryRecord_NNsh(dbRefW, sizeof(NNshMacroResult), &logRec);
    return;
}
#endif


#ifdef USE_MACRO
/*=========================================================================*/
/*   Function :  macroDataOperation                                        */
/*                                                      �}�N���̃f�[�^���� */
/*=========================================================================*/
static UInt16 *macroSelectGR(NNshMacroRecord *lineData, UInt16 *gr)
{
    if ((lineData->dst >= NNSH_MACROOP_GR_OFFSET)&&
        (lineData->dst <= NNSH_MACROOP_GR_MAX))
    {
        // �w�肳�ꂽ GR �ɑ��
        return (&gr[lineData->dst - NNSH_MACROOP_GR_OFFSET]);
    }
    // GR0 �� ���
    return (gr);
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function :  macroDataOperation                                        */
/*                                                        NNsi�ݒ�̒l���� */
/*=========================================================================*/
static void macroDataOperation(NNshMacroRecord *lineData, UInt16 *gr, Boolean setter)
{
    UInt16 value, setValue, *data;

    value    = 0;
    setValue = getMacroValue(lineData, gr);
    switch (lineData->MacroData.numData.subCommand)
    {
      case NNSH_MACROOPCMD_AUTOSTARTMACRO:
        // �}�N���������s
        value = (NNshGlobal->NNsiParam)->autostartMacro;
        if (setter == true)
        {
            // �l�ݒ肪�w������Ă����ꍇ�A�A�A
            (NNshGlobal->NNsiParam)->autostartMacro = setValue;
        }
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
        // �V���m�F���s���ɉ���ؒf
        value = (NNshGlobal->NNsiParam)->disconnArrivalEnd;
        if (setter == true)
        {
            // �l�ݒ肪�w������Ă����ꍇ�A�A�A
            (NNshGlobal->NNsiParam)->disconnArrivalEnd = setValue;
        }
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_DA:
        // �V���m�F���s���DA�N��
        value = (NNshGlobal->NNsiParam)->use_DAplugin;
        if (setter == true)
        {
            // �l�ݒ肪�w������Ă����ꍇ�A�A�A
            (NNshGlobal->NNsiParam)->use_DAplugin = setValue;
        }
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
        // �Q�ƃ��O�̎擾���x��
        value = (NNshGlobal->NNsiParam)->getROLogLevel;
        if (setter == true)
        {
            // �l�ݒ肪�w������Ă����ꍇ�A�A�A
            (NNshGlobal->NNsiParam)->getROLogLevel = setValue;
        }
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
        // �V���m�F�I�����Ƀr�[�v��
        value = (NNshGlobal->NNsiParam)->autoBeep;
        if (setter == true)
        {
            // �l�ݒ肪�w������Ă����ꍇ�A�A�A
            (NNshGlobal->NNsiParam)->autoBeep = setValue;
        }
        break;
        
      case NNSH_MACROOPCMD_COPY_AND_DELETE:
        // �Q�ƃ��O�ɃR�s�[����Ƃ��A�폜�����{
        value = (NNshGlobal->NNsiParam)->copyDelReadOnly;
        if (setter == true)
        {
            // �l�ݒ肪�w������Ă����ꍇ�A�A�A
            (NNshGlobal->NNsiParam)->copyDelReadOnly = setValue;
        }
        break;

      default:
        // �������Ȃ�
        break;
    }
    data  = macroSelectGR(lineData, gr);
    *data = value;
    return;
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : getMacroValue                                              */
/*                                            �}�N���̃I�y�����h����l�擾 */
/*=========================================================================*/
static UInt16 getMacroValue(NNshMacroRecord *lineData, UInt16 *gr)
{
    if ((lineData->src >= NNSH_MACROOP_GR_OFFSET)&&
        (lineData->src <= NNSH_MACROOP_GR_MAX))
    {
        // ���W�X�^����ǂݍ��񂾒l����
        return (gr[lineData->src - NNSH_MACROOP_GR_OFFSET]);
    }

    // ��R��������l���擾����
    return (lineData->MacroData.numData.operator);
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : checkMacroCommandAvailable                                 */
/*                                            �}�N���R�}���h�̎��s���m�F */
/*=========================================================================*/
static Boolean checkMacroCommandAvailable(UInt16 command, UInt16 halt)
{
    // ���s���R�}���h���X�g
    UInt16  okCmdList[] = {
        MULTIBTN_FEATURE_GET,    MULTIBTN_FEATURE_ALLGET,    MULTIBTN_FEATURE_PARTGET,
        MULTIBTN_FEATURE_DELETE, MULTIBTN_FEATURE_SORT,      MULTIBTN_FEATURE_SEARCH,
        MULTIBTN_FEATURE_NEXT,   MULTIBTN_FEATURE_PREV,      MULTIBTN_FEATURE_TOP,
        MULTIBTN_FEATURE_BOTTOM, MULTIBTN_FEATURE_COPYGIKO,  MULTIBTN_FEATURE_DELNOTGET,
        MULTIBTN_FEATURE_DBCOPY, MULTIBTN_FEATURE_GRAPHMODE, MULTIBTN_FEATURE_DISCONN,
        MULTIBTN_FEATURE_GETLIST,MULTIBTN_FEATURE_NEWARRIVAL,MULTIBTN_FEATURE_SELECTNEXT,
        MULTIBTN_FEATURE_SELECTPREV,MULTIBTN_FEATURE_PREVPAGE,MULTIBTN_FEATURE_NEXTPAGE,
        MULTIBTN_FEATURE_TO_NOTREAD,MULTIBTN_FEATURE_TO_GETALL,MULTIBTN_FEATURE_LOGCHARGE,
        MULTIBTN_FEATURE_SETLOGCHRG,MULTIBTN_FEATURE_BT_ON,  MULTIBTN_FEATURE_BT_OFF,
        MULTIBTN_FEATURE_TO_OFFLINE,MULTIBTN_FEATURE_TO_FAVOR,MULTIBTN_FEATURE_TO_USER1,
        MULTIBTN_FEATURE_TO_USER2,MULTIBTN_FEATURE_TO_USER3, MULTIBTN_FEATURE_TO_USER4,
        MULTIBTN_FEATURE_TO_USER5,MULTIBTN_FEATURE_OYSTERLOGIN, MULTIBTN_FEATURE_NEXTSEARCHBBS, 
        MULTIBTN_FEATURE_UPDATERESNUM, MULTIBTN_FEATURE_BE_LOGIN, 0,
        0,0
    };
    UInt16  check, *cmdPtr;

    // �}�N�����s�����`�F�b�N
    if (halt > MAX_HALT_OPERATION)
    {
        // ���s�����ɂЂ�������
        return (false);
    }

    // �ꗗ��ʋ@�\�����s�ł��邩�ǂ����̃`�F�b�N
    if (((command)&(MULTIBTN_FEATURE)) == (MULTIBTN_FEATURE))
    {
        check = (command)&(MULTIBTN_FEATURE_MASK);
        cmdPtr = okCmdList;
        while (*cmdPtr != 0)
        {
            if (check == *cmdPtr)
            {
                // ���s�\�R�}���h���X�g�ɔ����A���s����
                return (true);
            }
            cmdPtr++;
        }
        // ���s�\�R�}���h���X�g�ɂȂ������A���s�֎~
        return (false);
    }

    // ���s����
    return (true);
}
#endif

#ifdef USE_MACRO
/*-------------------------------------------------------------------------*/
/*   Function : setFileListItems                                           */
/*                                                �t�@�C���ꗗ�̃��X�g�ݒ� */
/*-------------------------------------------------------------------------*/
static UInt16 setFileListItems(Char *fnameP)
{
    Err                  ret, err;
    UInt16               cardNo, cnt;
    LocalID              dbLocalID;
    DmSearchStateType    state;
    Char                 fileName[MINIBUF + MARGIN], *ptr;

    // VFS�̎g�p�ݒ�ɂ��킹�ăt���O��ݒ�A�t�@�C�������ƁA�ꗗ�\��
    ptr = fnameP;
    cnt = 0;

    // Palm�{�̓��ɂ���t�@�C���̈ꗗ���擾
    ret = DmGetNextDatabaseByTypeCreator(true, &state, 
                                         DATA_DBTYPE_ID, 
                                         DBSOFT_CREATOR_ID,
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
            if ((fnameP != NULL)&&(cnt < MAX_MACRO_DBFILES))
            {
                StrCopy(ptr, fileName);
                ptr = ptr + StrLen(ptr) + 1;
                cnt++;
            }
        }

        // ���ڈȍ~�̃f�[�^�擾
        ret = DmGetNextDatabaseByTypeCreator(false, &state,
                                             DATA_DBTYPE_ID, 
                                             DBSOFT_CREATOR_ID,
                                             false, &cardNo, &dbLocalID);
    }
    return (cnt);
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : NNsi_ExecuteMacroSelection                                 */
/*                                                          �}�N���I�����s */
/*=========================================================================*/
Boolean NNsi_ExecuteMacroSelection(void)
{
    Char   *ptr, *dbFileNamePtr, dbFileName[MINIBUF + MARGIN];
    UInt16  cnt, btnId;

    // ����
    MemSet(dbFileName, sizeof(dbFileName), 0x00);    
    cnt = 0;
    dbFileNamePtr = MEMALLOC_PTR((MINIBUF) * (MAX_MACRO_DBFILES) + MARGIN);
    if (dbFileNamePtr == NULL)
    {
        NNsh_DebugMessage(ALTID_INFO, " memory allocation failure... ", "", 0);
        return (false);   
    }
    MemSet(dbFileNamePtr, ((MINIBUF) * (MAX_MACRO_DBFILES) + MARGIN), 0x00);

    // �}�N���t�@�C�����𒊏o����
    cnt = setFileListItems(dbFileNamePtr);
    if (cnt == 0)
    {
        NNsh_DebugMessage(ALTID_INFO, " macro count is zero... ", "", 0);
        return (false);
    }    

    // �I���E�B���h�E���J��
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, dbFileNamePtr, cnt, 0);
    if (btnId == BTNID_JUMPCANCEL)
    {
        // �L�����Z��
        MEMFREE_PTR(dbFileNamePtr);
        NNsh_DebugMessage(ALTID_INFO, "selection canceled ", "", 0);
        return (false);    
    }

    // �I�������X�N���v�g�t�@�C�����𒊏o���� 
    ptr = dbFileNamePtr;
    for (cnt = 0; cnt < (MAX_MACRO_DBFILES); cnt++)
    {
        if (cnt == NNshGlobal->jumpSelection)
        {
            StrCopy(dbFileName, ptr);
            break;        
        } 
        ptr = ptr + StrLen(ptr) + 1;
    }

    // �m�ۂ����������̊J��
    MEMFREE_PTR(dbFileNamePtr);

    // �f�o�b�O�\��...
    NNsh_DebugMessage(ALTID_INFO, " Macro Name: ", dbFileName, 0);

    // �I�������}�N�������s����...
    if (StrLen(dbFileName) != 0)
    {
        NNsi_ExecuteMacroMain(dbFileName, DBVERSION_MACRO, DBSOFT_CREATOR_ID, DATA_DBTYPE_ID);
        return (true);
    }
    return (false);
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : NNsi_ExecuteMacroMain                                      */
/*                                                        �}�N�����s���C�� */
/*=========================================================================*/
void NNsi_ExecuteMacroMain(Char *macroDBName, UInt16 macroDBVersion, UInt32 creator, UInt32 type)
{
    Err                  ret;
    Boolean              penState;
    Char                *buf, *grBuf;
    UInt16              *gr, recQnt, cnt, command, *data, *stack, *top;
    UInt16               halt, savedParam, autoOffTime, y;
    UInt32               code;
    NNshMacroRecord     *lineData;
    DmOpenRef            dbRefR, dbRefW;
    MemHandle            dataH, tmpH;
    NNshSubjectDatabase *subjDB;
    
    // �f�[�^������
    halt     = 0;
    code     = 0;
    lineData = NULL;

    // �̈�̊m��
    stack = MEMALLOC_PTR(sizeof(UInt16) * NOF_REGISTER + MARGIN);
    if (stack == NULL)
    {
        // �̈�m�ێ��s�A�I������
        return;
    }
    MemSet(stack, ((sizeof(UInt16) * NOF_REGISTER) + MARGIN), 0x00);
    top = stack;

    // �̈�̊m��
    gr = MEMALLOC_PTR((sizeof(UInt16) * NOF_REGISTER) + MARGIN);
    if (gr == NULL)
    {
        // �̈�m�ێ��s�A�I������
        return;
    }
    MemSet(gr, (sizeof(UInt16) * NOF_REGISTER + MARGIN), 0x00);

    // �̈�̊m��
    grBuf = MEMALLOC_PTR(sizeof(Char) * TEXTBUFSIZE + MARGIN);
    if (grBuf == NULL)
    {
        // �̈�m�ێ��s�A�I������
        MEMFREE_PTR(gr);
        return;
    }
    MemSet(grBuf, (sizeof(Char) * TEXTBUFSIZE + MARGIN), 0x00);

    // �̈�̊m��
    subjDB = MEMALLOC_PTR(sizeof(NNshSubjectDatabase) + MARGIN);
    if (subjDB == NULL)
    {
        // �̈�m�ێ��s�A�I������
        MEMFREE_PTR(grBuf);
        MEMFREE_PTR(gr);
        return;
    }
    MemSet(subjDB, (sizeof(NNshSubjectDatabase) + MARGIN), 0x00);

    // �̈�̊m��
    buf = MEMALLOC_PTR(MAX_STRLEN + MARGIN);
    if (buf == NULL)
    {
        // �̈�m�ێ��s�A�I������
        MEMFREE_PTR(subjDB);
        MEMFREE_PTR(grBuf);
        MEMFREE_PTR(gr);
        return;
    }
    MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);

    // �}�N�����s���A�m�F�_�C�A���O���ȗ�����
    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

    // �f�[�^�x�[�X���J��(�X�N���v�g�t�@�C��)
    OpenDB_wCreator(macroDBName, macroDBVersion, creator, type, false, &dbRefR);
    if (dbRefR == 0)
    {
        // �G���[�����A�I������
        goto FUNC_END;
    }

    // �f�[�^�x�[�X���J��(���O�t�@�C��)
    OpenDatabase_NNsh(DBNAME_MACRORESULT, DBVERSION_MACRO, &dbRefW);
    if (dbRefW == 0)
    {
        // �G���[�����A�I������
        CloseDatabase_NNsh(dbRefR);
        goto FUNC_END;
    }
    GetDBCount_NNsh(dbRefW, &recQnt);
    while (recQnt != 0)
    {
        //  ���łɃ��O�t�@�C�����L�^����Ă����ꍇ�A�S���R�[�h���폜����
        (void) DeleteRecordIDX_NNsh(dbRefW, (recQnt - 1));
        recQnt--;
    }

    // �X�N���v�g�̃��R�[�h�����m�F����
    GetDBCount_NNsh(dbRefR, &recQnt);

    // �����d���n�e�e�^�C�}�𖳌��ɂ���
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    cnt = 0;
    while (cnt < recQnt)
    {
        // ���R�[�h��ǂݏo��
        ret = GetRecordReadOnly_NNsh(dbRefR, cnt, &dataH, (void **) &lineData);
        if (ret != errNone)
        {
            // ���R�[�h�ǂݏo�����s�A���̃��R�[�h�ֈړ�����
            dataH = 0;
            goto NEXT_COMMAND;
        }

        switch (lineData->opCode)
        {
          case NNSH_MACROCMD_XOR:
            // �r���I�_���a���Ƃ�
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data ^ command;
            break;

          case NNSH_MACROCMD_AND:
            // �l��_���ς���
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data & command;
            break;

          case NNSH_MACROCMD_OR:
            // �l��_���a����
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data | command;
            break;

          case NNSH_MACROCMD_ADD:
            // �l�����Z����
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data + command;
            break;

          case NNSH_MACROCMD_CMP:
          case NNSH_MACROCMD_SUB:
            // �l���r���āA���̌��ʂ���
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data - command;
            break;

          case NNSH_MACROCMD_JNZ:
            // �l���[���łȂ���΃W�����v
            data    = macroSelectGR(lineData, gr);
            if (*data != 0)
            {
                // �W�����v��́Asrc or ���l����擾����
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // �R�}���h���s�s����(�W�����v�s��)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                                   "CANNOT JUMP");
                }
            }
            break;

          case NNSH_MACROCMD_JZ:
            // �l���[���Ȃ�W�����v
            data    = macroSelectGR(lineData, gr);
            if (*data == 0)
            {
                // �W�����v��́Asrc or ���l����擾����
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // �R�}���h���s�s����(�W�����v�s��)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                                   "CANNOT JUMP");
                }
            }
            break;

          case NNSH_MACROCMD_MOVE:
            // �f�[�^(�l)�̑��
            data    = macroSelectGR(lineData, gr);
            command = getMacroValue(lineData, gr);
            *data   = command;
            break;

          case NNSH_MACROCMD_JMP:
            // �V�[�P���X�̖������W�����v
            command = getMacroValue(lineData, gr);
            if ((command > 0)&&(command <= recQnt))
            {
                cnt = command - 1 - 1;
            }
            else
            {
                // �R�}���h���s�s����(�W�����v�s��)
                outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                               "CANNOT JUMP");
            }
            break;

          case NNSH_MACROCMD_EXECUTE:
            // �@�\�̎��s
            command = getMacroValue(lineData, gr);

            // �R�}���h���s�����m�F
            if (checkMacroCommandAvailable(((command)|(MULTIBTN_FEATURE)), halt) == true)
            {
                // �R�}���h�����s����
                ExecuteAction(command);
            }
            else
            {
                // �R�}���h���s�s����(���O�o��)
                outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, command,
                               "PROHIBIT EXECUTION.");
            }
            break;

          case NNSH_MACROCMD_MESSAGE:
            // ���O�o��
            if ((lineData->src >= NNSH_MACROOP_GR_OFFSET)&&
                (lineData->src <= NNSH_MACROOP_GR_MAX))
            {
                // src��GRx���w�肳��Ă����ꍇ
                MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
                StrCopy(buf, "[gr");
                NUMCATI(buf, lineData->src - NNSH_MACROOP_GR_OFFSET);
                StrCat (buf, " = ");
                command = gr[lineData->src - NNSH_MACROOP_GR_OFFSET];
                NUMCATI(buf, command);
                StrCat (buf, "] ");
                // ����������ɒǉ�����
                StrNCopy(&buf[StrLen(buf)], lineData->MacroData.strData.data,
                         (MAX_STRLEN - StrLen(buf)));
                outputMacroLog(dbRefW, lineData->MacroData.strData.operator, 0,
                               buf);
            }
            else
            {
                // �^����ꂽ�f�[�^�����̂܂ܕ\������
                outputMacroLog(dbRefW, lineData->MacroData.strData.operator, 0,
                               lineData->MacroData.strData.data);
            }
            break;

          case NNSH_MACROCMD_BEEP:
            // �r�[�v����炷
            command = getMacroValue(lineData, gr);
            switch (command)
            {
              case 0:
                // Info ��
                NNsh_BeepSound(sndInfo);
                break;

              case 1:
                // Alarm��(NNsh_BeepSound()��sndAlarm�ɕϊ�����)
                NNsh_BeepSound(sndClick);
                break;

              case 2:
                // Error ��
                NNsh_BeepSound(sndError);
                break;

              case 3:
                // Confirmation ��
                NNsh_BeepSound(sndConfirmation);
                break;

              case 0xffff:
                // �A���[�����ƃ��O���o��
                NNsh_BeepSound(sndAlarm);
                outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, 0,
                                             lineData->MacroData.strData.data);
                break;

              default:
                // �A���[����
                NNsh_BeepSound(sndAlarm);
                break;
            }
            break;

          case NNSH_MACROCMD_MACROEND:
            // �}�N���I��(�I���̃��O���o�͂���)
            outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_DEBUG, 0, "<< MACRO END >>");

            // ���R�[�h�ԍ����I�[�o�[�����A���[�v�������I�ɔ�����
            cnt = recQnt;
            break;

          case NNSH_MACROCMD_OPERATE:
            // NNsi�ݒ�ύX
            macroDataOperation(lineData, gr, true);
            break;

          case NNSH_MACROCMD_STATUS:
            // NNsi�ݒ�̒l�擾
            macroDataOperation(lineData, gr, false);
            break;

          case NNSH_MACROCMD_SETTABNUM:
            // ���݂̃^�u�����i�[
            data  = macroSelectGR(lineData, gr);
            *data = Show_tab_info(false);
            break;

          case NNSH_MACROCMD_STRSTR:
            // �����񂪊܂܂�邩�`�F�b�N
            data  = macroSelectGR(lineData, gr);
            *data = 1;
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
                if (Get_Subject_Database(command, subjDB) == errNone)
                {
                    if (StrStr(subjDB->threadTitle, 
                                         lineData->MacroData.strData.data) != NULL)
                    {
                        // �����񂪊܂܂�Ă���
                        *data = 0;
                    }
                }
            }
            break;

          case NNSH_MACROCMD_STRCMP:
            // �����񂪈�v���邩�`�F�b�N
            data  = macroSelectGR(lineData, gr);
            *data = 1;
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
                if (Get_Subject_Database(command, subjDB) == errNone)
                {
                    if (StrCompare(subjDB->threadTitle, 
                                         lineData->MacroData.strData.data) != NULL)
                    {
                        // �����񂪈�v����
                        *data = 0;
                    }
                }
            }
            break;

          case NNSH_MACROCMD_GETMESNUM:
            // �擾���X���𔽉f
            data  = macroSelectGR(lineData, gr);
            *data = 0;
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                // �擾���X����������
                *data = subjDB->maxLoc;
            }
            break;

          case NNSH_MACROCMD_MESSTATUS:
            // �擾�X���̏�Ԃ𔽉f
            data  = macroSelectGR(lineData, gr);
            *data = 0;
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                // �X����Ԃ�������
                *data = subjDB->state;
            }
            break;

          case NNSH_MACROCMD_BBSTYPE:
            // BBS�^�C�v�𔽉f
            data  = macroSelectGR(lineData, gr);
            *data = 0;
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                // BBS�^�C�v��������
                *data = subjDB->bbsType;
            }
            break;

          case NNSH_MACROCMD_IMPORT:
#ifdef USE_XML_OUTPUT
            if ((NNshGlobal->NNsiParam)->useVFS == NNSH_NOTSUPPORT_VFS)
            {
                // VFS ��T�|�[�g�@��͂��̂܂�NNsi�ݒ�C���|�[�g
                Input_NNsiSet_XML(FILE_XML_NNSISET);
                break;
            }
            // VFS�T�|�[�g�@��
            command = (NNshGlobal->NNsiParam)->useVFS;   // VFS�ݒ�̈ꎞ�L��
            data    = macroSelectGR(lineData, gr);
            if (*data == 0)
            {
                // (�������Palm����ǂݏo���悤�ɂ���)
                (NNshGlobal->NNsiParam)->useVFS = 0;
            }
            else
            {
                // (�������VFS�ݒ�ɍX�V����)
                (NNshGlobal->NNsiParam)->useVFS = ((NNSH_VFS_ENABLE)|
                                     (NNSH_VFS_WORKAROUND)|
                                     (NNSH_VFS_USEOFFLINE)|
                                     (NNSH_VFS_DIROFFLINE)|
                                     (NNSH_VFS_DBBACKUP));
                
            }
            // NNsi�ݒ�C���|�[�g
            Input_NNsiSet_XML(FILE_XML_NNSISET);
            (NNshGlobal->NNsiParam)->useVFS = command;  // VFS�ݒ�̕���
#endif
            break;

          case NNSH_MACROCMD_EXPORT:
#ifdef USE_XML_OUTPUT
            // NNsi�ݒ�̃G�N�X�|�[�g
            Output_NNsiSet_XML(FILE_XML_NNSISET);
#endif
            break;

          case NNSH_MACROCMD_LAUNCHDA:
            // DA�N��
            code = lineData->MacroData.numData.data1;
            code = code << 16;
            code = code + lineData->MacroData.numData.data2;

            ///////////////////// DA �N���m�F�p���O  //////////////////////////
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            StrCopy(buf, "DA:0x");
            NUMCATH(buf, code);
            outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_INFO, 0, buf);
            ///////////////////////////////////////////////////////////////////

            // �����d��OFF�^�C�}���ꎞ�I�ɕ���������
            (void) SysSetAutoOffTime(autoOffTime);
            (void) EvtResetAutoOffTimer();

            // DA���N������
            LaunchDA_NNsh(code);

            // �����d��OFF�^�C�}���~������
            (void) SysSetAutoOffTime(0);
            (void) EvtResetAutoOffTimer();
            break;

          case NNSH_MACROCMD_CLIPCOPY:
             // ��������N���b�v�{�[�h�ɃR�s�[
             ClipboardAddItem(clipboardText, lineData->MacroData.strData.data, 
                              (StrLen(lineData->MacroData.strData.data) + 1));
            break;

          case NNSH_MACROCMD_CLIPADD:
             // ��������N���b�v�{�[�h�ɒǉ�
             ClipboardAppendItem(clipboardText, 
                                 lineData->MacroData.strData.data, 
                                 (StrLen(lineData->MacroData.strData.data) + 1));
            break;

          case NNSH_MACROCMD_CLIPINSTR:
            // �N���b�v�{�[�h�̕�����Ɋ܂܂�邩�m�F
            data  = macroSelectGR(lineData, gr);
            *data = 1;
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                // �N���b�v�{�[�h�̓��e����������o�b�t�@�ɃR�s�[
                if (command > TEXTBUFSIZE)
                {
                    command = TEXTBUFSIZE;
                }
                MemSet(grBuf, (sizeof(Char) * TEXTBUFSIZE + MARGIN), 0x00);
                MemMove(grBuf, MemHandleLock(tmpH), command);
                MemHandleUnlock(tmpH);
                if (StrStr(grBuf, lineData->MacroData.strData.data) != NULL)
                {
                    // �f�[�^���܂܂�Ă���
                    *data = 0;
                }
            }
            break;

          case NNSH_MACROCMD_TITLECLIP:
            // �X���^�C�g�����N���b�v�{�[�h�ɃR�s�[
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
                if (Get_Subject_Database(command, subjDB) == errNone)
                {
                    ClipboardAddItem(clipboardText, subjDB->threadTitle, 
                                            (StrLen(subjDB->threadTitle) + 1));
                }
            }
            break;

          case NNSH_MACROCMD_SET_FINDBBSSTR:
            // �^�u�����p������ɃN���b�v�{�[�h�̕�������R�s�[����
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                // �N���b�v�{�[�h�̓��e����������o�b�t�@�ɃR�s�[
                if (command > BUFSIZE)
                {
                    command = BUFSIZE - 1;
                }
                if (NNshGlobal->searchBBSH == 0)
                {
                    NNshGlobal->searchBBSH = MemHandleNew(BUFSIZE);
                    if (NNshGlobal->searchBBSH == 0)
                    {
                        // �̈�m�ێ��s�A�Ȃɂ����Ȃ�
                        break;
                    }
                }
                MemSet(MemHandleLock(NNshGlobal->searchBBSH), BUFSIZE, 0x00);
                MemHandleUnlock(NNshGlobal->searchBBSH);
                MemMove(MemHandleLock(NNshGlobal->searchBBSH), MemHandleLock(tmpH), command);
                MemHandleUnlock(tmpH);
                MemHandleUnlock(NNshGlobal->searchBBSH);
            }
            break;

          case NNSH_MACROCMD_SET_FINDTHREADSTR:
            // �X���^�C�����p������ɃN���b�v�{�[�h�̕�������R�s�[����
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                // �N���b�v�{�[�h�̓��e����������o�b�t�@�ɃR�s�[
                if (command > BUFSIZE)
                {
                    command = BUFSIZE - 1;
                }
                if (NNshGlobal->searchTitleH == 0)
                {
                    NNshGlobal->searchTitleH = MemHandleNew(BUFSIZE);
                    if (NNshGlobal->searchTitleH == 0)
                    {
                        // �̈�m�ێ��s�A�Ȃɂ����Ȃ�
                        break;
                    }
                }
                MemSet(MemHandleLock(NNshGlobal->searchTitleH), BUFSIZE, 0x00);
                MemHandleUnlock(NNshGlobal->searchTitleH);
                MemMove(MemHandleLock(NNshGlobal->searchTitleH), MemHandleLock(tmpH), command);
                MemHandleUnlock(tmpH);
                MemHandleUnlock(NNshGlobal->searchTitleH);
            }
            break;

          case NNSH_MACROCMD_UPDATE_RESNUM:
            // �X���̃��X�����X�V���i�[(1.03)
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
                data    = macroSelectGR(lineData, gr);
                *data   = UpdateThreadResponseNumber(command);
            }
            break;

          case NNSH_MACROCMD_JPL:
            // �l�����̏ꍇ�ɂ̓W�����v(1.03)
            data    = macroSelectGR(lineData, gr);
            if ((*data > 0)&&(*data <= 0x7fff))
            {
                // �W�����v��́Asrc or ���l����擾����
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // �R�}���h���s�s����(�W�����v�s��)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                                   "CANNOT JUMP");
                }
            }
            break;

          case NNSH_MACROCMD_JMI:
            // �l��������(��)�ꍇ�ɂ̓W�����v(1.03)
            data    = macroSelectGR(lineData, gr);
            if (*data > 0x7fff)
            {
                // �W�����v��́Asrc or ���l����擾����
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // �R�}���h���s�s����(�W�����v�s��)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                                   "CANNOT JUMP");
                }
            }
            break;

          case NNSH_MACROCMD_OUTPUTTITLE:
            // �X���^�C�g�������O�o��(1.03)
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                StrNCopy(buf, subjDB->threadTitle, MAX_STRLEN);
                outputMacroLog(dbRefW, lineData->MacroData.strData.operator, 0,
                               buf);
            }
            break;

          case NNSH_MACROCMD_OUTPUTCLIP:
            // �N���b�v�{�[�h����������O�o��(1.03)
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                if (command > MAX_STRLEN)
                {
                    command = MAX_STRLEN;
                }
                StrNCopy(buf, MemHandleLock(tmpH), command);
                outputMacroLog(dbRefW, lineData->MacroData.strData.operator, 0,
                               buf);
                MemHandleUnlock(tmpH);
            }                
            break;

          case NNSH_MACROCMD_REMARK:
            // ���ߕ��A�������Ȃ�
            break;

          case NNSH_MACROCMD_PUSH:
            // �X�^�b�N��PUSH(�X�^�b�N�T�C�Y�̌��E)
            if ((stack + sizeof(UInt16) * NOF_REGISTER) != top)
            {
                command = getMacroValue(lineData, gr);
                *top    = command;
                top++;
            }
            break;
         
          case NNSH_MACROCMD_POP:
            // �X�^�b�N����POP(1.04)
            if (top != stack)
            {
                data  = macroSelectGR(lineData, gr);
                *data = *top;
                top--;
            }
            break;

          case NNSH_MACROCMD_GETRECNUM:
            // ���R�[�h�ԍ����擾����(1.04)
            data    = macroSelectGR(lineData, gr);
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            }
            else
            {
                // ���Ȃ�傫�Ȑ����i�[...
                command = 0xfffd;
            }
            *data = command;
            break;

          case NNSH_MACROCMD_FINDTHREADSTRCOPY:
            // �X���^�C������������N���b�v�{�[�h�ɃR�s�[
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            if (NNshGlobal->searchTitleH != 0)
            {
                StrNCopy(buf, MemHandleLock(NNshGlobal->searchTitleH), MAX_STRLEN);
                MemHandleUnlock(NNshGlobal->searchTitleH);
            }
            else
            {
                StrCopy(buf, " ");
            }
            ClipboardAddItem(clipboardText, buf, StrLen(buf));
            break;

          case NNSH_MACROCMD_INPUTDIALOG:
            // ��������̓_�C�A���O��\������ (���͕�������N���b�v�{�[�h�ɃR�s�[)
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            DataInputDialog(lineData->MacroData.strData.data, buf, MAX_STRLEN, 
                            NNSH_DIALOG_USE_MACROINPUT, NULL);
            ClipboardAddItem(clipboardText, buf, StrLen(buf));
            break;
            
          case NNSH_MACROCMD_OUTPUTDIALOG:
            // ��������_�C�A���O�\������
            NNsh_ErrorMessage(ALTID_INFO, lineData->MacroData.strData.data, "", 0);
            break;

          case NNSH_MACROCMD_CONFIRMDIALOG:
            // �N���b�v�{�[�h�̕������\�����A�m�F�_�C�A���O��\������
            //  (�{�^���������ꂽ����(OK/Cancel)�����W�X�^�ɔ��f������j
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                // �N���b�v�{�[�h���當������擾����
                if (command > MAX_STRLEN)
                {
                    command = MAX_STRLEN;
                }
                StrNCopy(buf, MemHandleLock(tmpH), command);
                MemHandleUnlock(tmpH);
            }
            else
            {
                // �����_�C�A���O�ɕ\�����Ȃ�
                StrNCopy(buf, " Are you OK? ", StrLen(" "));
            }
            data    = macroSelectGR(lineData, gr);
            command = NNsh_ErrorMessage(ALTID_CONFIRM, buf, "", 0);
            *data   = command;
            break;

          case NNSH_MACROCMD_MESATTRIBUTE:
            // �擾�X���̑����𔽉f (1.06)
            data  = macroSelectGR(lineData, gr);
            *data = 0;
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                // �X����Ԃ�������
                *data = subjDB->msgAttribute;
            }
            break;

          case NNSH_MACROCMD_JUMPLAST:
            // �X���̍ŉ��s�ɂ���ꍇ�ɂ̓W�����v (1.06)
            // (���Ȃ薳�����Ȕ���...)
            if ((((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_LOWERLIMIT)||
                 ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_ALL))&&
                 (NNshGlobal->nofTitleItems == ((NNshGlobal->NNsiParam)->selectedTitleItem + 1)))
            {
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // �R�}���h���s�s����(�W�����v�s��)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1), "CANNOT JUMP");
                }
            }
            break;

          default:
            // �R�}���h�R�[�h�ُ�A�Ȃɂ����s���Ȃ�
            break;
        }

NEXT_COMMAND:
        // ���R�[�h��������A���ւ�����
        ReleaseRecordReadOnly_NNsh(dbRefR, dataH);
        cnt++;

        // �y���̏�Ԃ��E��(��ʃ^�b�v����Ă��邩�m�F����)
        EvtGetPen(&command, &y, &penState);
        if (penState == true)
        {
            // �y�����_�E������Ă�����A���~���邩�m�F���s��
            if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                  MSG_CONFIRM_ABORT_MACRO, 
                                  MSG_LINE_NUMBER, cnt) == 0)
            {
                // OK�������ꂽ�Abreak����
                break;
            }
        }
    }

    // �����d��OFF�^�C�}�𕜋�������
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    // �f�[�^�x�[�X����ďI������    
    CloseDatabase_NNsh(dbRefW);
    CloseDatabase_NNsh(dbRefR);

FUNC_END:
    MEMFREE_PTR(buf);
    MEMFREE_PTR(subjDB);
    MEMFREE_PTR(grBuf);
    MEMFREE_PTR(gr);
    MEMFREE_PTR(stack);
    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

    // �I���u�U�[��炷
    // SndPlaySystemSound(sndAlarm);

    // �}�N���I����ʒm����
    NNsh_InformMessage(ALTID_INFO, MSG_MACROEXEC_DONE, "", 0);
    return;
}
#endif

#ifdef USE_MACRO
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
#endif

#ifdef USE_MACRO
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
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : NNsi_MacroDataView                                         */
/*                                                        �}�N�����e��\�� */
/*=========================================================================*/
void NNsi_MacroDataView(Char *macroDBName, UInt16 macroDBVersion)
{
    Err              ret;
    Char           **script, logData[TINYBUF];
    UInt16           macroCnt, lp, command;
    MemHandle        dataH;
    NNshMacroRecord *lineData;
    DmOpenRef        dbRefR;

    // �f�[�^�̏�����
    script   = NULL;
    macroCnt = 0;

    // �G���[�\���p
    MemSet (logData, sizeof(logData), 0x00);
    StrCopy(logData, "MEMERR");

    // �f�[�^�x�[�X���J��(�X�N���v�g�t�@�C��)
    OpenDatabase_NNsh(macroDBName, macroDBVersion, &dbRefR);
    if (dbRefR == 0)
    {
        // �G���[�����A�I������
        return;
    }

    // �X�N���v�g�̃��R�[�h�����m�F����
    GetDBCount_NNsh(dbRefR, &macroCnt);

    script = MEMALLOC_PTR(sizeof(Char *) * macroCnt + MARGIN);
    if (script == NULL)
    {
        // �������m�ۃG���[�����A�I������
        CloseDatabase_NNsh(dbRefR);
        return;
    }
    MemSet(script, (sizeof(Char *) * macroCnt + MARGIN), 0x00);

    // ���O�f�[�^�o��    
    for (lp = 0; lp < macroCnt; lp++)
    {
        // ���R�[�h��ǂݏo��
        ret = GetRecordReadOnly_NNsh(dbRefR, lp, &dataH, (void **) &lineData);
        if (ret != errNone)
        {
            // ���R�[�h�ǂݏo�����s�A���̃��R�[�h�ֈړ�����
            dataH = 0;
            script[lp] = logData;
            goto NEXT_COMMAND;
        }
        script[lp] = MEMALLOC_PTR(MAX_LOGVIEW_BUFFER + MARGIN);
        if (script[lp] == NULL)
        {
            // �̈�m�ۃG���[
            script[lp] = logData;
            goto NEXT_COMMAND;
        }
        MemSet(script[lp], (MAX_LOGVIEW_BUFFER + MARGIN), 0x00);

        // �s�ԍ���\��
        NUMCATI(script[lp], (lp + 1));
        StrCat (script[lp], "  ");

        switch (lineData->opCode)
        {
          case NNSH_MACROCMD_XOR:
            // �l�����Z����
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " XOR ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_AND:
            // �l�����Z����
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " AND ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_OR:
            // �l�����Z����
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " OR ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_ADD:
            // �l�����Z����
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " + ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_CMP:
          case NNSH_MACROCMD_SUB:
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " - ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_JNZ:
            StrCat (script[lp], "IF gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], "<> 0 THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_JZ:
            StrCat (script[lp], "IF gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " = 0 THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_MOVE:
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_JMP:
            StrCat (script[lp], "GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_EXECUTE:
            // �@�\�̎��s
            StrCat (script[lp], "CALL (");
            command = setSourceDataDisplay(script[lp], lineData);
            if (command != NNSH_MACROOPCMD_DISABLE)
            {
                StrCat (script[lp], ":");
                setExecFuncDataDisplay(script[lp], command);
                StrCat (script[lp], " ");
            }
            StrCat (script[lp], ") ");
            break;

          case NNSH_MACROCMD_MESSAGE:
            // ���O�o��
            StrCat (script[lp], "PRINT ");
            if ((lineData->src >= NNSH_MACROOP_GR_OFFSET)&&
                (lineData->src <= NNSH_MACROOP_GR_MAX))
            {
                // GRx ��\��
                StrCat (script[lp], "(gr");
                NUMCATI(script[lp], lineData->src - NNSH_MACROOP_GR_OFFSET);
                StrCat (script[lp], ") ");
            }
            else
            {
                // �������\��
                StrCat (script[lp], "\"");
                StrCat (script[lp], lineData->MacroData.strData.data);
                StrCat (script[lp], "\" ");
            }
            break;

          case NNSH_MACROCMD_BEEP:
            // �r�[�v����炷
            StrCat (script[lp], "BEEP ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_MACROEND:
            // �}�N���I��
            StrCat (script[lp], "END ");
            break;

          case NNSH_MACROCMD_SETTABNUM:
            // �^�u�����i�[
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := ");
            StrCat (script[lp], "(tabNum) ");
            break;

          case NNSH_MACROCMD_STRSTR:
            // �w�肵�������񂪃X���^�C�g���Ɋ܂܂�邩
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := strstr(title, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;            

          case NNSH_MACROCMD_STRCMP:
            // �w�肵�������񂪃X���^�C�g���Ɋ܂܂�邩
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := strstr(title, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;            

          case NNSH_MACROCMD_OPERATE:
            // NNsi�ݒ�l�̎擾
            switch (lineData->MacroData.numData.subCommand)
            {
              case NNSH_MACROOPCMD_AUTOSTARTMACRO:
                // �}�N���������s�̒l���擾
                StrCat(script[lp], "(autoMacro) := ");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
                // �V���m�F���s��ɉ���ؒf
                StrCat(script[lp], "(chkLINE) := ");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_DA:
                // �V���m�F���s���DA�N��
                StrCat(script[lp], "(chkDA) := ");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
                // �Q�ƃ��O�擾���x��
                StrCat(script[lp], "(ROlogLvl) := ");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
                // �V���m�F�I����Ƀr�[�v��
                StrCat(script[lp], "(chkBEEP) := ");
                break;

              case NNSH_MACROOPCMD_COPY_AND_DELETE:
                // �Q�ƃ��O�R�s�[��ɍ폜�����{
                StrCat(script[lp], "(cp&rm) := ");
                break;

              default:
                StrCat(script[lp], "nop ");
                break;
            }
            setSourceDataDisplay(script[lp], lineData);
            StrCat(script[lp], ", save ");
            /** not break; **/

          case NNSH_MACROCMD_STATUS:
            // NNsi�ݒ�l�̎擾
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := ");
            switch (lineData->MacroData.numData.subCommand)
            {
              case NNSH_MACROOPCMD_AUTOSTARTMACRO:
                // �}�N���������s�̒l���擾
                StrCat(script[lp], "(autoMacro)");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
                // �V���m�F���s��ɉ���ؒf
                StrCat(script[lp], "(chkLINE)");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_DA:
                // �V���m�F���s���DA�N��
                StrCat(script[lp], "(chkDA)");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
                // �Q�ƃ��O�擾���x��
                StrCat(script[lp], "(ROlogLvl)");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
                // �V���m�F�I����Ƀr�[�v��
                StrCat(script[lp], "(chkBEEP)");
                break;

              case NNSH_MACROOPCMD_COPY_AND_DELETE:
                // �Q�ƃ��O�R�s�[��ɍ폜�����{
                StrCat(script[lp], "(cp&rm)");
                break;

              default:
                StrCat(script[lp], "0");
                break;
            }
            break;

          case NNSH_MACROCMD_GETMESNUM:
            // �X�����𔽉f
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := mesNum(title) ");
            break;            

          case NNSH_MACROCMD_MESSTATUS:
            // �X����Ԃ𔽉f
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := mesStatus(title) ");
            break;

          case NNSH_MACROCMD_BBSTYPE:
            // �X����BBS�^�C�v�𔽉f
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := bbsType(title) ");
            break;

          case NNSH_MACROCMD_IMPORT:
            // NNsi�ݒ�C���|�[�g
#ifdef USE_XML_OUTPUT
            StrCat (script[lp], "IMPORT '");
            StrCat (script[lp], FILE_XML_NNSISET);
            if (setSourceDataDisplay(script[lp], lineData) == 0)
            {
                StrCat (script[lp], "' from VFS");
            }
            else
            {
                StrCat (script[lp], "' from Palm");
            }
#else
            StrCat (script[lp], "NOP ");
#endif
            break;

          case NNSH_MACROCMD_EXPORT:
            // NNsi�ݒ�G�N�X�|�[�g
#ifdef USE_XML_OUTPUT
            StrCat (script[lp], "EXPORT '");
            StrCat (script[lp], FILE_XML_NNSISET);
            StrCat (script[lp], "' ");
#else
            StrCat (script[lp], "NOP ");
#endif
            break;

          case NNSH_MACROCMD_LAUNCHDA:
            // DA�N��
            StrCat (script[lp], "LAUNCH '");
            logData[0] = ((0xff00 & lineData->MacroData.numData.data1) >> 8); 
            logData[1] = ((0x00ff & lineData->MacroData.numData.data1));
            logData[2] = ((0xff00 & lineData->MacroData.numData.data2) >> 8);
            logData[3] = ((0x00ff & lineData->MacroData.numData.data2));
            logData[4] = '\0';
            StrCat (script[lp], logData);            
            StrCat (script[lp], "' as DA");
            break;

          case NNSH_MACROCMD_CLIPCOPY:
            // �N���b�v�{�[�h�ɃR�s�[
            StrCat (script[lp], "COPY(clip, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;

          case NNSH_MACROCMD_CLIPADD:
             // �N���b�v�{�[�h�ɒǉ�
            StrCat (script[lp], "APPEND(clip, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;

          case NNSH_MACROCMD_CLIPINSTR:
            // �N���b�v�{�[�h�̕�����Ɋ܂܂�邩�m�F
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := strstr(clip, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;

          case NNSH_MACROCMD_TITLECLIP:
            // �X���^�C�g�����N���b�v�{�[�h�ɃR�s�[
            StrCat(script[lp], "COPY(clip, title)");
            break;

          case NNSH_MACROCMD_SET_FINDBBSSTR:
            // �N���b�v�{�[�h�̓��e���a�a�r�����p�ɐݒ�
            StrCat(script[lp], "COPY(findBBS, clip)");
            break;

          case NNSH_MACROCMD_SET_FINDTHREADSTR:
            // �N���b�v�{�[�h�̓��e���X�������p�ɐݒ�
            StrCat(script[lp], "COPY(findTHREAD, clip)");
            break;

          case NNSH_MACROCMD_UPDATE_RESNUM:
            // ���X�����X�V���Ċi�[
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := updateMesNum(title) ");
            break;

          case NNSH_MACROCMD_JPL:
            // ���̒l�Ȃ�W�����v����
            StrCat (script[lp], "IF gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], "> 0 THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_JMI:
            // ���̒l�Ȃ�W�����v����
            StrCat (script[lp], "IF gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], "< 0 THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_OUTPUTTITLE:
            // �X���^�C�g�������O�o��
            StrCat (script[lp], "PRINT title");
            break;

          case NNSH_MACROCMD_OUTPUTCLIP:
            // �N���b�v�{�[�h�̓��e�����O�o��
            StrCat (script[lp], "PRINT clip");
            break;

          case NNSH_MACROCMD_REMARK:
            // ����(�����\�����邾��)
            StrCat (script[lp], "' ");
            StrCat (script[lp], lineData->MacroData.strData.data);
            break;

          case NNSH_MACROCMD_PUSH:
            // �X�^�b�N��PUSH
            StrCat (script[lp], "PUSH");
            setSourceDataDisplay(script[lp], lineData);
            break;

          case NNSH_MACROCMD_POP:
            // �X�^�b�N����POP
            StrCat (script[lp], "POP ");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            break;

          case NNSH_MACROCMD_GETRECNUM:
            // ���R�[�h�ԍ����i�[����
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := RecordIndex(title)");
            break;

          case NNSH_MACROCMD_FINDTHREADSTRCOPY:
            // �X��������������N���b�v�{�[�h�ɃR�s�[
            StrCat (script[lp], "COPY(clip, findTHREAD)");
            break;
          case NNSH_MACROCMD_INPUTDIALOG:
            // ���������͂��� (1.05)
            StrCat (script[lp], "clip := INPUT \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            break;

          case NNSH_MACROCMD_OUTPUTDIALOG:
            // ��������o�͂��� (1.05)
            StrCat (script[lp], "OUTPUT \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\"");
            break;

          case NNSH_MACROCMD_CONFIRMDIALOG:
            // �N���b�v�{�[�h�̕������\�����m�F���� (1.05)
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := CONFIRM (clip)");
            break;

          case NNSH_MACROCMD_MESATTRIBUTE:
            // �X�������𔽉f
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := mesAttribute(title) ");
            break;

          case NNSH_MACROCMD_JUMPLAST:
            // �X���̍ŉ��s�Ȃ�W�����v����
            StrCat (script[lp], "IF isLast(threadList) THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          default:
            // �R�}���h�R�[�h�ُ�A�Ȃɂ����s���Ȃ�
            StrCat (script[lp], "  ");
            break;
        }

NEXT_COMMAND:
        // ���R�[�h��������A���ւ�����
        ReleaseRecordReadOnly_NNsh(dbRefR, dataH);
    }
    CloseDatabase_NNsh(dbRefR);

    // �}�N���f�[�^�̕\��
    DataViewForm(NNSH_MACROCMD_VIEWTITLE, macroCnt, script);

    // �m�ۂ����̈���J������
    for (lp = 0; lp < macroCnt; lp++)
    {
        if ((script[lp] != NULL)&&(script[lp] != logData))
        {
            MEMFREE_PTR(script[lp]);
        }
    }
    MEMFREE_PTR(script);
    return;
}
#endif
        
#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : NNsi_MacroExecLogView                                      */
/*                                                          �}�N�����s���� */
/*=========================================================================*/
void NNsi_MacroExecLogView(Char *macroDBName, UInt16 macroDBVersion)
{
    Err              ret;
    Char           **script, logData[TINYBUF];
    UInt16           macroCnt, lp;
    MemHandle        dataH;
    NNshMacroResult *lineData;
    DmOpenRef        dbRefR;
    DateTimeType     dateTime;

    // �f�[�^�̏�����
    script   = NULL;
    macroCnt = 0;

    // �G���[�\���p
    MemSet (logData, sizeof(logData), 0x00);
    StrCopy(logData, "MEMERR");

    // �f�[�^�x�[�X���J��(�X�N���v�g�t�@�C��)
    OpenDatabase_NNsh(macroDBName, macroDBVersion, &dbRefR);
    if (dbRefR == 0)
    {
        // �G���[�����A�I������
        return;
    }

    // �X�N���v�g�̃��R�[�h�����m�F����
    GetDBCount_NNsh(dbRefR, &macroCnt);

    script = MEMALLOC_PTR(sizeof(Char *) * macroCnt + MARGIN);
    if (script == NULL)
    {
        // �������m�ۃG���[�����A�I������
        CloseDatabase_NNsh(dbRefR);
        return;
    }
    MemSet(script, (sizeof(Char *) * macroCnt + MARGIN), 0x00);

    // ���O�f�[�^�o��    
    for (lp = 0; lp < macroCnt; lp++)
    {
        // ���R�[�h��(��������)�ǂݏo�� �����R�[�h�̎Ⴂ�ԍ����ŐV�̃f�[�^�Ȃ���
        ret = GetRecordReadOnly_NNsh(dbRefR, (macroCnt - (1 + lp)), &dataH, (void **) &lineData);
        if (ret != errNone)
        {
            // ���R�[�h�ǂݏo�����s�A���̃��R�[�h�ֈړ�����
            dataH = 0;
            script[lp] = logData;
            goto NEXT_COMMAND;
        }
        script[lp] = MEMALLOC_PTR(MAX_LOGOUTPUT_BUFFER + MARGIN);
        if (script[lp] == NULL)
        {
            // �̈�m�ۃG���[
            script[lp] = logData;
            goto NEXT_COMMAND;
        }
        MemSet(script[lp], (MAX_LOGOUTPUT_BUFFER + MARGIN), 0x00);

        // ������\��
        MemSet(&dateTime, sizeof(dateTime), 0x00);
        TimSecondsToDateTime(lineData->dateTime, &dateTime);
        DateToAscii(dateTime.month, dateTime.day, dateTime.year,
                    dfYMDWithDashes, script[lp]);
        StrCat(script[lp], " ");
        TimeToAscii(dateTime.hour, dateTime.minute, tfColon24h, 
                    &script[lp][StrLen(script[lp])]);
        StrCat(script[lp], ":");
        NUMCATI(script[lp], dateTime.second);      

        // ���O���x���ƃG���[�R�[�h��\��(�G���[�R�[�h�̓G���[�������̂�)
        StrCat (script[lp], "[");
        NUMCATI(script[lp], lineData->logLevel);
        if (lineData->errCode != 0)
        {
            StrCat (script[lp], ":0x");
            NUMCATH(script[lp], lineData->errCode);
        }
        StrCat (script[lp], "] ");

        // ���O�f�[�^��\��
        StrCat (script[lp], lineData->logData);

NEXT_COMMAND:
        // ���R�[�h��������A���ւ�����
        ReleaseRecordReadOnly_NNsh(dbRefR, dataH);
    }
    CloseDatabase_NNsh(dbRefR);

    // �\���_�C�A���O���J��
    DataViewForm(NNSH_MACROLOG_VIEWTITLE, macroCnt, script);

    // �m�ۂ����̈���J������
    for (lp = 0; lp < macroCnt; lp++)
    {
        if ((script[lp] != NULL)&&(script[lp] != logData))
        {
            MEMFREE_PTR(script[lp]);
        }
    }
    MEMFREE_PTR(script);
    return;
}        
#endif
