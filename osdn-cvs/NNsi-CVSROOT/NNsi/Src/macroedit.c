/*============================================================================*
 *
 *  $Id: macroedit.c,v 1.6 2005/02/16 15:43:27 mrsa Exp $
 *
 *  FILE: 
 *     macroedit.c
 *
 *  Description: 
 *     NNsi Macro Setting
 *
 *===========================================================================*/
#define MACROEDIT_C
#include "local.h"

#ifdef USE_MACROEDIT
/*=========================================================================*/
/*   Function : NNsi_MacroDataEdit                                         */
/*                                                        �}�N���f�[�^�ݒ� */
/*=========================================================================*/
void NNsi_MacroDataEdit(Char *macroDBName, UInt16 macroDBVersion)
{
#ifdef MACRO_TEST

    // �e�X�g�p�̃}�N���f�[�^��f���o��
    Err              ret;
    UInt16           lp;
    NNshMacroRecord  lineData;
    DmOpenRef        dbRef;

    // NNsj (NNsiMacroEdit)���C���X�g�[������Ă��邩�m�F����    
    if (CheckInstalledResource_NNsh('appl', 'NNsj') != false)
    {
        // NNsj���N������(NNsi�I����ɋN��)
        (void) WebBrowserCommand('NNsj', 0, 0, sysAppLaunchCmdNormalLaunch, 
                                  NULL, 0, NULL);
        return;
    }



    // �}�N���f�[�^�x�[�X�̃I�[�v��
    OpenDatabase_NNsh(macroDBName, macroDBVersion, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return;
    }
    
    ////////////////////////////////////////////////////////////////////////
    lp = 0;
    GetDBCount_NNsh(dbRef, &lp);
    while (lp != 0)
    {
        //  ���ł�NNsi�ݒ肪�L�^����Ă����ꍇ�A�S���R�[�h���폜����
        (void) DeleteRecordIDX_NNsh(dbRef, (lp - 1));
        lp--;
    }
    ////////////////////////////////////////////////////////////////////////
    
    // 1:�}�N�����s���O�ɕ�������o��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "SAMPLE MACRO START", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 2:�V���m�F�I�����DA�����N������̐ݒ�l���N���A(GRf�ɕۑ�)
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GRf;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_DA;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 3:�V���m�F�I����Ƀr�[�v����炷�ݒ�l���N���A(GRe�ɕۑ�)
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GRe;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_BEEP;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 4:�V���m�F�I����ɉ���ؒf����ݒ�l���N���A(GRd�ɕۑ�)
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GRd;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_LINE;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 5:'�擾�ςݑS��'�ֈړ�
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_TO_GETALL;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 6: �}�N�����s���O�ɕ�������o��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "START NEWARRIVAL CHECK", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 7:�V���m�F�����s
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_NEWARRIVAL;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 8:�}�N�����s���O�ɕ�������o��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "END   NEWARRIVAL CHECK", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 9:�Q�ƃ��O�擾
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_LOGCHARGE;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 10:�}�N�����s���O�ɕ�������o��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "END   LOGCHARGE", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 11:����ؒf
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_DISCONN;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 12:BT OFF
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_BT_OFF;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 13:�������\��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "unread thread:", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 14:�^�u���� gr0 �� �Z�b�g����
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_SETTABNUM;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 15:GR0��\��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = NNSH_MACROOP_GR0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 16:GR0���[��(���ǂȂ�)��������A18�ɃW�����v����
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_JZ;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator   = 18;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 17:���ǂ���ֈړ�
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_TO_NOTREAD;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 18:NNsi�ݒ���G�N�X�|�[�g
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXPORT;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 19:�}�N�������N���ݒ��OFF�ɂ���
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_AUTOSTARTMACRO;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 20:�V���m�F�I�����DA�����N������̐ݒ�l���擾���AGRf�Ɋi�[
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = NNSH_MACROOP_GRf;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_DA;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 21:�V���m�F�I����Ƀr�[�v����炷�ݒ�l��GRe���畜��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = NNSH_MACROOP_GRe;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_BEEP;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 22:�V���m�F�I����ɉ���ؒf����ݒ�l��GRd���畜��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = NNSH_MACROOP_GRd;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_LINE;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 23:BEEP����炷
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_BEEP;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 1;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 24:�}�N�����I��
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MACROEND;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // ���בւ������{
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_UINT16, 0);

    // �f�[�^�x�[�X�̃N���[�Y
    CloseDatabase_NNsh(dbRef);

    // "�_�~�[�}�N�����o�͂���" �\�����s��
    NNsh_InformMessage(ALTID_INFO, MSG_SAMPLEMACRO_EFFECTED, "", 0);

#else

    // "���݃T�|�[�g���Ă��܂���" �\�����s��
    NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);

#endif
    return;
}
#endif  // #ifdef USE_MACROEDIT
