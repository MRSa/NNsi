/*============================================================================*
 *  FILE: 
 *     main.c
 *
 *  Description: 
 *     main source code for Palm application.
 *     (This file includes in "skeleton.c".)
 *
 *===========================================================================*/
#define MAIN_C

/********** Include Files **********/
#include "local.h"

// external functions
extern Err     OpenForm_MainForm    (FormType           *frm);
extern Err     OpenForm_NNshSetting (FormType           *frm);
extern Boolean Handler_MainForm     (EventType          *event);
extern Boolean Handler_NNshSetting  (EventType          *event);
extern Err     VFSUnmountNotify     (SysNotifyParamType *notifyParamsP);
extern Err     VFSMountNotify       (SysNotifyParamType *notifyParamsP);
extern void   SetDoubleDensitySupport_NNsh(void);
extern void   ResetVFSNotifications(void);

/*=========================================================================*/
/*   Function :   MyApplicationDispatchEvent                               */
/*                                                ����f�B�X�p�b�`�C�x���g */
/*=========================================================================*/
Boolean MyApplicationDispatchEvent(EventType *event)
{
    NNshSavedPref *NNsiParam;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    //  "�n�[�h�L�[���䂷��"�@�w�肪����A���L�[���̓C�x���g�Ȃ�
    // �t�H�[���̏��������{����
    if ((NNsiParam->useHardKeyControl != 0)&&(event->eType == keyDownEvent))
    {
        if (event->data.keyDown.chr >= vchrHardKeyMin)
        {
            // �n�[�h�L�[�R�[�h�̊m�F
            switch (event->data.keyDown.chr)
            {
              case vchrHard1:
                // �n�[�h�L�[�P
                event->data.keyDown.chr = NNsiParam->useKey.key1;
                break;

              case vchrHard2:
                // �n�[�h�L�[�Q
                event->data.keyDown.chr = NNsiParam->useKey.key2;
                break;

              case vchrHard3:
                // �n�[�h�L�[�R
                event->data.keyDown.chr = NNsiParam->useKey.key3;
                break;

              case vchrHard4:
                // �n�[�h�L�[�S
                event->data.keyDown.chr = NNsiParam->useKey.key4;
                break;

              default:
                //  �i�m��Ȃ��j�n�[�h�L�[���͂��s��ꂽ�Ƃ��A�L�[�R�[�h��
                // �\������
                NNsh_DebugMessage(ALTID_INFO, "Hard Key Event ", " chrID:",
                                  event->data.keyDown.chr);
                break;
            }
        }
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   MyFrmLoadEvent                                           */
/*                                               �t�H�[���I�[�v�����̏���  */
/*-------------------------------------------------------------------------*/
static Boolean MyFrmLoadEvent(EventType *event)
{
    FormType  *frm;
    UInt16     fieldFont, objFont;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // �g�p�t�H���g��ݒ肷��
    fieldFont = NNsiParam->currentFont;
    FntSetFont(fieldFont);
    objFont   = NNsiParam->currentFont;

    // �t�H�[���������� 
    frm = FrmInitForm(event->data.frmLoad.formID);
#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(frm, vgaFormModify160To240);
    }
#endif
#ifdef USE_PIN_DIA
        if (NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)
        {
            VgaFormModify_DIA(frm, vgaFormModifyNormalToWide);
        }
        else if (NNsiGlobal->device == NNSH_DEVICE_GARMIN)
        {
            // ��ʂ��I�[�v�������Ƃ��́A�ĕ`�悷��(Garmin�@)
            VgaFormModify_DIA(frm, vgaFormModifyNormalToWide);
            NNsiGlobal->notifyUpdate = NNSH_DISP_RESIZED;
            EvtWakeup();
        }
#endif
#ifdef USE_CLIE
    NNsiGlobal->tempStatus = 0;
#endif

    // �t�H�[�����J���ăC�x���g�n���h����ݒ肷��
    FrmSetActiveForm(frm);

    // ��ʂ�`�悷��
#ifdef USE_DEBUG_ROM
    NNsi_FrmDrawForm(frm, true);
#endif

    switch (event->data.frmLoad.formID)
    {
      // ���C���t�H�[��
      case FRMID_MAIN:
        // �O���t�B�e�B�G���A���ǂ����邩�A�A�A
        if (NNsiParam->silkMax == 0)
        {
            // �O���t�B�e�B�G���A���ŏ���
            (void) NNshSilkMinimize(frm);
        }
        else
        {
            // �O���t�B�e�B�G���A���ő剻
            (void) NNshSilkMaximize(frm);
        }
        FldSetFont(FrmGetObjectPtr(frm,
                             FrmGetObjectIndex(frm, FLDID_SCREEN)), fieldFont);

        // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
        OpenForm_MainForm(frm);
        FrmSetEventHandler(frm, Handler_MainForm);
        break;

      default:
        // ����"�t�H�[���J������C�x���g"���\�����Ă݂�B(�f�o�b�O)
        NNsh_DebugMessage(ALTID_INFO, "frmLoadEvent()", " formID:",
                          event->data.frmLoad.formID);
        break;
    }

    // ��ʂ�`�悷��
    NNsi_FrmDrawForm(frm, true);
    return (false);
}


/*=========================================================================*/
/*   Function :   MyApplicationHandleEvent                                 */
/*                                                                         */
/*=========================================================================*/
Boolean MyApplicationHandleEvent(EventType *event)
{
    FormType  *frm;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // SONY�V���N������s���ꍇ�̉�ʍX�V
#ifdef USE_CLIE
    if ((NNsiGlobal->silkRef != 0)&&
        (NNsiGlobal->updateHR == NNSH_UPDATE_DISPLAY))
    {
         // SILK�}�l�[�W���̕\���ɂ��킹����ʂ̍X�V���s��
        frm = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm, true);
        NNsiGlobal->updateHR = NNSH_NOT_EFFECTIVE;
    }
#endif

    // Notify�C�x���g�̎�M������ʉ߂�����
    if (NNsiGlobal->notifyUpdate != 0)
    {
        frm = FrmGetActiveForm();
        if (frm != NULL)
        {
            switch (NNsiGlobal->notifyUpdate)
            {
              case NNSH_VFS_MOUNTED:
                // VFS��mount���ꂽ���Ƃ����[�U�ɒʒm����
                NNsh_InformMessage(ALTID_WARN, "Mounted VFS", "", 0);
                break;

              case NNSH_VFS_UNMOUNTED:
                // VFS��unmount���ꂽ���Ƃ����[�U�ɒʒm����
                NNsh_InformMessage(ALTID_WARN, "Unmounted VFS", "", 0);
                break;

              case NNSH_DISP_RESIZED:
                // ��ʂ̃T�C�Y���ύX�ɂȂ���...
                HandEraResizeForm(frm);
                NNsh_DebugMessage(ALTID_INFO, "[[<<DISPLAY RESIZED>>]]", "", 0);

                FrmEraseForm(frm);
                NNsi_FrmDrawForm(frm, true);
                break;

              default:
                break;
            }
        }
        NNsiGlobal->notifyUpdate = 0;
    }


    // �����ŁA(�t�H�[�����[�h����)�C�x���g��M���̏������L�q����B
    switch (event->eType)
    {
      // �t�H�[�����J������event
      case frmLoadEvent:
        (void) MyFrmLoadEvent(event);
        break;

#ifdef USE_PIN_DIA
      case winEnterEvent:
        if ((NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNsiGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();

            // ��ʂ�`�悳�ꂽ�Ƃ��A�V���N�ŏ���/�ő剻��L���ɂ���
            if (event->data.winEnter.enterWindow == FrmGetWindowHandle(frm))
            {
                PINSetInputTriggerState(pinInputTriggerEnabled);
            }
        }
        break;
#endif  // #ifdef USE_PIN_DIA
#if 0
      // �t�H�[�����J������event
      case frmOpenEvent:
        break;

      // �t�H�[���������event
      case frmCloseEvent:
        break;
#endif
      default:
        // �T�|�[�g���Ă��Ȃ��C�x���g����M�����B
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   MyApplicationHandleNotifications                         */
/*                                Notification����                         */
/*=========================================================================*/
UInt32  MyApplicationHandleNotifications(SysNotifyParamType *notifyParamsP)
{ 
  // PilotMain�����Ă�Code Resource�̏ꍇ�͂�������Notification��
  // ��������̂���ʓI�A���ĂȂ��ꍇ��SysNotifyRegister��function pointer��
  // �n���Ă��

  switch(notifyParamsP->notifyType)
  {
    case sysNotifyVolumeUnmountedEvent:
      VFSUnmountNotify(notifyParamsP);
      break;

    case sysNotifyVolumeMountedEvent:
      VFSMountNotify(notifyParamsP);
      break;     
  }
  return 0;
}

/*=========================================================================*/
/*   Function :   myStartApplication                                       */
/*                                 �X�^�[�g����(�v���O������)����������  */
/*=========================================================================*/
UInt16 MyStartApplication(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    Int16                   ret;
    UInt16                  size;
    NNshSoftwareVersionInfo verInfo;
#ifdef USE_ARMLET
    UInt32                  processorType;
#endif
    UInt32                  osVersion;
#ifdef USE_TSPATCH
    UInt32                  fontId, fontVer;
#endif
    NNshSavedPref          *nnsiParam;     // NNsi�ݒ�
    NNshWorkingInfo        *nnsiGlobal;    // NNsi�O���[�o���ݒ�

    // NNsi�ݒ�o�[�W�����̊m�F
    //   (Version�m�F�pPreference���m�F���A�ύX���Ȃ���Ε���)
    size = sizeof(NNshSoftwareVersionInfo);
    MemSet(&verInfo, sizeof(NNshSoftwareVersionInfo), 0x00);
    ret  = PrefGetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, 
                                 &verInfo, &size, false);

    // NNsi�ݒ�p�i�[�̈�̊m��
    size = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    nnsiParam = (NNshSavedPref *) MemPtrNew(size);
    if (nnsiParam == NULL)
    {
        // �̈�m�ۂɎ��s�I(NNsi�͋N�����Ȃ�)
        NNsh_ErrorMessage(ALTID_ERROR,MSG_CANNOT_LAUNCH,"(Prm)\nsize:",size);
        return (0);
    }        
    MemSet(nnsiParam, size, 0x00);

    // pointer��feature�ɃZ�b�g
    FtrSet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32)nnsiParam);

    // NNsi�O���[�o���̈�p�i�[�̈�̊m��
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    nnsiGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (nnsiGlobal == NULL)
    {
        // �̈�m�ۂɎ��s�I(NNsi�͋N�����Ȃ�)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_CANNOT_LAUNCH,"(Gbl)\nsize:", size);
        return (0);
    }        
    MemSet(nnsiGlobal, size, 0x00);

    // pointer��feature�ɃZ�b�g
    FtrSet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32)nnsiGlobal);

    // OS�o�[�W�����̊m�F
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osVersion) == errNone)
    {
        nnsiGlobal->palmOSVersion = osVersion;
    }

    // NNsi�ݒ�𕜋�������
    (void) RestoreSetting_NNsh(ret, verInfo.prefSize, nnsiParam);

    // �f�o�b�O���[�h�̏��𕜋�������
    nnsiParam->debugMessageON = verInfo.debugMode;


    // VFS�@�\���Ȃ��ݒ肾�����ꍇ�ɂ́A��UVFS�p�����[�^���N���A����B
    if (nnsiParam->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        nnsiParam->useVFS = NNSH_VFS_DISABLE;
    }
    // VFS�T�|�[�g�̐ݒ�
    CheckVFSdevice_NNsh();

#ifdef USE_CLIE
    // CLIE�T�|�[�g�̐ݒ�
    SetCLIESupport_NNsh();
#endif  // #ifdef USE_CLIE

#ifdef USE_HANDERA
    // HandEra�T�|�[�g�̐ݒ�
    SetHandEraSupport_NNsh();
#endif // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
    // pen input manager�̐ݒ�
    SetDIASupport_NNsh();
#endif    // #ifdef USE_PIN_DIA

#ifdef USE_HIGHDENSITY
    // ���𑜓x�T�|�[�g/�J���[�T�|�[�g�̃`�F�b�N
    SetDoubleDensitySupport_NNsh();
#else
    // ���𑜓x�T�|�[�g�ݒ��������
    nnsiGlobal->os5HighDensity = (UInt32) kDensityLow;
#endif    // #ifdef USE_HIGHDENSITY

#ifdef USE_TSPATCH
    // TsPatch�Ή��R�[�h...(TsPatch�K�p�����ǂ����`�F�b�N����)
    if (nnsiParam->notUseTsPatch == 0)
    {
        ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
        if (ret == errNone)
        {    
            // TsPatch�K�p��...�g�p�t�H���g������������B
            switch (nnsiParam->currentFont)
            {
              case stdFont:
                FtrGet(SmallFontAppCreator, SMF_FTR_TINY_FONT, &fontId);
                break;

              case boldFont:
                FtrGet(SmallFontAppCreator, SMF_FTR_TINY_BOLD_FONT, &fontId);
                break;

              case largeFont:
                FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT, &fontId);
                break;

              case largeBoldFont:
                FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_BOLD_FONT, &fontId);
                break;

              default:
                fontId = nnsiParam->currentFont;
                break;
            }
            nnsiParam->currentFont = fontId;
        }
    }
    else
    {
        // TsPatch�@�\���g��Ȃ��ꍇ,,,
        switch (nnsiParam->currentFont)
        {
          case stdFont:
          case boldFont:
          case largeFont:
          case largeBoldFont:
            // ���̂܂܎g��
            break;

          default:
            // �W���t�H���g�ɕύX����
            nnsiParam->currentFont = stdFont;
            break;
        }
    }
#endif  // #ifdef USE_TSPATCH

    // CLIE small�t�H���g���g�p����
    if (nnsiParam->sonyHRFont != 0)
    {
        // �������킹�A�A�A
        nnsiParam->currentFont = 52;
    }

#ifdef USE_ARMLET
    // ARMlet�g�p�ۂ̃`�F�b�N
    FtrGet(sysFileCSystem, sysFtrNumProcessorID, &processorType);
    if (sysFtrNumProcessorIsARM(processorType))
    {
        // ARMlet�g�p�\�f�o�C�X�B
        // nnsiParam->useARMlet = NNSH_ENABLE;  // ARMlet�͎g�p����ݒ�ɕύX
    }
    else
#endif
    {
        // ARMlet�͎g�p���Ȃ��ݒ�ɂ���
        nnsiParam->useARMlet = NNSH_DISABLE;
    }

    // VFS��OFF�������ꍇ�ɂ́A�m�F���b�Z�[�W��\������B
    if (nnsiParam->useVFS == NNSH_VFS_DISABLE)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_USE_VFS_WARNING, "", 0);
    }

#ifdef USE_UTF8
    // UTF8�̃f�[�^�e�[�u����ݒ肷��
    OpenUtf8Database(&(nnsiGlobal->jis2unicodeDB), &(nnsiGlobal->unicode2jisDB));
#endif

    // �N���R�[�h�̊m�F
    switch (cmd)
    {
      case kaniEditCmdOpenFileStream:
        // �t�@�C���X�g���[�����J��
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_STREAM;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, DATAFILE_PREFIX, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFS:
        // VFS�t�@�C�����J��
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSdos:
        // VFS�t�@�C�����J��(DOS)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_DOS;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSmac:
        // VFS�t�@�C�����J��(MAC)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_MAC;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSjis:
        // VFS�t�@�C�����J��(JIS)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_JIS;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSeuc:
        // VFS�t�@�C�����J��(EUC)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_EUC;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSutf8:
        // VFS�t�@�C�����J��(UTF8)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_UTF8;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSreadOnly:
        // VFS�t�@�C����ǂݍ��ݐ�p�ŊJ��
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_READONLY;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenFileStreamReadOnly:
        // �t�@�C���X�g���[����ǂݍ��ݐ�p�ŊJ��
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_STREAM_READONLY;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      default:
        // �������Ȃ�
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_DISABLED;
        StrNCopy(nnsiGlobal->dirName, DATAFILE_PREFIX, MAXLENGTH_FILENAME);
        break;
    }

    // �f�t�H���g�f�B���N�g���̐ݒ�
    size = StrLen(nnsiGlobal->dirName) - 1;
    while (size > 0)
    {
        if (nnsiGlobal->dirName[size] == '/')
        {
            nnsiGlobal->dirName[size + 1] = '\0';
            break;
        }
        size--;
    }

    // �f�t�H���g�f�B���N�g�����w�肾�����ꍇ
    if (nnsiGlobal->dirName[0] != '/')
    {
        StrCopy(nnsiGlobal->dirName, DATAFILE_PREFIX);
    }

    // �ŏ��ɃI�[�v������t�H�[����ID����������B
    return (FRMID_MAIN);
}

/*=========================================================================*/
/*   Function :   myStopApplication                                        */
/*                                 �X�g�b�v����(�v���O������)����������  */
/*=========================================================================*/
void MyStopApplication(void)
{
    UInt16                  savedParam;
    Char                   *txtP;
    FieldType              *fldP;
    FormType               *prevFrm;
    MemHandle               txtH;
    NNshSoftwareVersionInfo verInfo;

    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    savedParam = NNsiParam->confirmationDisable;
    NNsiParam->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

    // ���C���t�H�[�����J���Ă���Ƃ�
    if (FrmGetActiveFormID() == FRMID_MAIN)
    {
        // �f�[�^�ۑ��_�C�A���O���J�� (�p�����[�^�ݒ莞)
        if (NNsiParam->useEndSaveDialog != 0)
        {
            (void) NNsh_FileData(BTNID_DATA_SAVE);
        }

        // �O��̃t�@�C���𕜋A����ݒ�̏ꍇ�A�A�A
        if (NNsiParam->autoRestore != 0)
        {
            prevFrm = FrmGetActiveForm();
            fldP = FrmGetObjectPtr(prevFrm, FrmGetObjectIndex(prevFrm, FLDID_SCREEN));
            txtH = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                txtP = MemHandleLock(txtH);
                if ((txtP != NULL)&&(*txtP != '\0'))
                {
                    // �f�[�^�̕ۊ�
                    (void) DataSaveToFile(prevFrm, txtP, FILE_TEMPFILE, 0, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_LF);
                }
                MemHandleUnlock(txtH);
            }
        }
    }

    // UTF8�̃f�[�^�e�[�u�����N���[�Y����
#ifdef USE_UTF8
    CloseUtf8Database(NNsiGlobal->jis2unicodeDB, NNsiGlobal->unicode2jisDB);
#endif

    NNsiParam->confirmationDisable = savedParam;

    // NNsi�ݒ��Version�m�F�pPreference�̕ۑ�(Debug���[�h�͂�����ŋL��)
    verInfo.NNsiVersion = SOFT_PREFVERSION;
    verInfo.prefSize    = sizeof(NNshSavedPref);
    verInfo.prefType    = NNSH_PREFERTYPE;
    verInfo.debugMode   = (NNsiParam != NULL) ? NNsiParam->debugMessageON : 0;
    PrefSetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, SOFT_PREFVERSION,
                          &verInfo, sizeof(NNshSoftwareVersionInfo), false);

    // NNsi�ݒ�̕ۑ�(NNsi�̋N���ɐ����������̂�)
    if (NNsiParam != NULL)
    {
        SaveSetting_NNsh(NNSH_PREFERTYPE, sizeof(NNshSavedPref), NNsiParam);
        MemPtrFree(NNsiParam);
        NNsiParam = NULL;
    }

#ifdef USE_CLIE
    // CLIE�T�|�[�g���J������
    ResetCLIESupport_NNsh();
#endif

#ifdef USE_PIN_DIA
    // pen input manager�̐ݒ����
    ResetDIASupport_NNsh();
#endif    // #ifdef USE_PIN_DIA

    // VFS��Notify���I��������
    ResetVFSNotifications();

    // NNsi�̃O���[�o���̈���J������
    if (NNsiGlobal != NULL)
    {
        MemPtrFree(NNsiGlobal);
        NNsiGlobal = NULL;
    }

    // feature���������
    FtrUnregister(SOFT_CREATOR_ID, FTRID_GLOBALPTR);
    FtrUnregister(SOFT_CREATOR_ID, FTRID_PARAMPTR);

    return;
}

/*=========================================================================*/
/*   Function :   checkLaunchCode                                          */
/*                                                       �N���R�[�h�̊m�F  */
/*=========================================================================*/
Boolean checkLaunchCode(UInt16 cmd)
{

    switch (cmd)
    {
      case kaniEditCmdOpenFileStreamReadOnly:
      case kaniEditCmdOpenFileStream:
      case kaniEditCmdOpenVFSreadOnly:
      case kaniEditCmdOpenVFS:
      case kaniEditCmdOpenVFSdos:
      case kaniEditCmdOpenVFSmac:
      case kaniEditCmdOpenVFSjis:
      case kaniEditCmdOpenVFSeuc:
      case kaniEditCmdOpenVFSutf8:
        // KaniEdit�p�N���R�[�h...
        return (true);
        break;

      default:
        break;
    }

    // ����ȊO�̋N���R�[�h...
    return (false);
}
