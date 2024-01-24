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

/*=========================================================================*/
/*   Function :  MyGetFormID                                               */
/*                                        �I�[�v������t�H�[��ID���������� */
/*=========================================================================*/
UInt16 GetFormID_NNsh(UInt16 frmID)
{
    UInt16 formID;

    switch (frmID)
    {
#if 0
      case NNSH_FRMID_CONFIG_NNSH:
        formID = FRMID_CONFIG_NNSH;
        break;
#endif
      case NNSH_FRMID_MAIN:
      default:
        formID = FRMID_MAIN;
        break;
    }
    return (formID);
}

/*=========================================================================*/
/*   Function :   MyApplicationDispatchEvent                               */
/*                                                ����f�B�X�p�b�`�C�x���g */
/*=========================================================================*/
Boolean MyApplicationDispatchEvent(EventType *event)
{
    //  "�n�[�h�L�[���䂷��"�@�w�肪����A���L�[���̓C�x���g�Ȃ�
    // �t�H�[���̏��������{����
    if ((NNshParam->useHardKeyControl != 0)&&(event->eType == keyDownEvent))
    {
        if (event->data.keyDown.chr >= vchrHardKeyMin)
        {
            // �n�[�h�L�[�R�[�h�̊m�F
            switch (event->data.keyDown.chr)
            {
              case vchrHard1:
                // �n�[�h�L�[�P
                event->data.keyDown.chr = NNshParam->useKey.key1;
                break;

              case vchrHard2:
                // �n�[�h�L�[�Q
                event->data.keyDown.chr = NNshParam->useKey.key2;
                break;

              case vchrHard3:
                // �n�[�h�L�[�R
                event->data.keyDown.chr = NNshParam->useKey.key3;
                break;

              case vchrHard4:
                // �n�[�h�L�[�S
                event->data.keyDown.chr = NNshParam->useKey.key4;
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

/*=========================================================================*/
/*   Function :   MyApplicationHandleEvent                                 */
/*                                                                         */
/*=========================================================================*/
Boolean MyApplicationHandleEvent(EventType *event)
{
    FormType  *frm;
    UInt16     fieldFont, objFont;

    // SONY�V���N������s���ꍇ�̉�ʍX�V
#ifdef USE_CLIE
    if ((NNshGlobal->silkRef != 0)&&
        (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY))
    {
         // SILK�}�l�[�W���̕\���ɂ��킹����ʂ̍X�V���s��
        frm = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm, true);
        NNshGlobal->updateHR = NNSH_NOT_EFFECTIVE;
    }
#endif

    // �����ŁA(�t�H�[�����[�h����)�C�x���g��M���̏������L�q����B
    switch (event->eType)
    {
      // �t�H�[�����J������event
      case frmLoadEvent:
        // �g�p�t�H���g��ݒ肷��
        fieldFont = NNshParam->currentFont;
        FntSetFont(fieldFont);
        objFont   = NNshParam->currentFont;

        // �t�H�[���������� 
        frm = FrmInitForm(event->data.frmLoad.formID);
#ifdef USE_HANDERA
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaFormModify(frm, vgaFormModify160To240);
        }
#endif
#ifdef USE_CLIE
        NNshGlobal->tempStatus = 0;
#endif
        // �t�H�[�����J���ăC�x���g�n���h����ݒ肷��
        FrmSetActiveForm(frm);
        switch (event->data.frmLoad.formID)
        {
          // ���C���t�H�[��
          case FRMID_MAIN:
            // �O���t�B�e�B�G���A���ŏ������ĉ�ʂ����T�C�Y�A�t�H���g��ύX
            (void) NNshSilkMinimize(frm);

            // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
            OpenForm_MainForm(frm);
            FrmSetEventHandler(frm, Handler_MainForm);
            break;
#if 0
          // NNsi�ݒ���
          case FRMID_CONFIG_NNSH:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNshSetting(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;
#endif

          default:
            // ����"�t�H�[���J������C�x���g"���\�����Ă݂�B(�f�o�b�O)
            NNsh_DebugMessage(ALTID_INFO, "frmLoadEvent()", " formID:",
                              event->data.frmLoad.formID);
            break;
        }
        // ��ʂ�`�悷��
        //�@NNsi_FrmDrawForm(frm, true);
        break;
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
UInt16 MyStartApplication(void)
{
    Int16                   ret;
    UInt16                  size;
    NNshSoftwareVersionInfo verInfo;
#ifdef USE_ARMLET
    UInt32                  processorType;
#endif
    UInt32                  osVersion;

    // NNsi�ݒ�o�[�W�����̊m�F
    //   (Version�m�F�pPreference���m�F���A�ύX���Ȃ���Ε���)
    size = sizeof(NNshSoftwareVersionInfo);
    MemSet(&verInfo, sizeof(NNshSoftwareVersionInfo), 0x00);
    ret  = PrefGetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, 
                                 &verInfo, &size, false);

    // NNsi�ݒ�p�i�[�̈�̊m��
    size = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    NNshParam = (NNshSavedPref *) MemPtrNew(size);
    if (NNshParam == NULL)
    {
        // �̈�m�ۂɎ��s�I(NNsi�͋N�����Ȃ�)
        NNsh_ErrorMessage(ALTID_ERROR,MSG_CANNOT_LAUNCH,"(Prm)\nsize:",size);
        return (0);
    }        
    MemSet(NNshParam, size, 0x00);

    // NNsi�O���[�o���̈�p�i�[�̈�̊m��
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // �̈�m�ۂɎ��s�I(NNsi�͋N�����Ȃ�)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_CANNOT_LAUNCH,"(Gbl)\nsize:", size);
        return (0);
    }        
    MemSet(NNshGlobal, size, 0x00);

    // OS�o�[�W�����̊m�F
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osVersion) == errNone)
    {
        NNshGlobal->palmOSVersion = osVersion;
    }

    // NNsi�ݒ�𕜋�������
    (void) RestoreSetting_NNsh(ret, verInfo.prefSize, NNshParam);

    // �f�o�b�O���[�h�̏��𕜋�������
    NNshParam->debugMessageON = verInfo.debugMode;

    // VFS�@�\���Ȃ��ݒ肾�����ꍇ�ɂ́A��UVFS�p�����[�^���N���A����B
    if (NNshParam->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        NNshParam->useVFS = NNSH_VFS_DISABLE;
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

#ifdef USE_HIGHDENSITY
    // ���𑜓x�T�|�[�g/�J���[�T�|�[�g�̃`�F�b�N
    SetDoubleDensitySupport_NNsh();
#else
    // ���𑜓x�T�|�[�g�ݒ��������
    NNshGlobal->os5HighDensity = (UInt32) kDensityLow;
#endif    // #ifdef USE_HIGHDENSITY

#ifdef USE_TSPATCH
    // TsPatch�Ή��R�[�h...(TsPatch�K�p�����ǂ����`�F�b�N����)
    if (NNshParam->notUseTsPatch == 0)
    {
        ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
        if (ret == errNone)
        {    
            // TsPatch�K�p��...�g�p�t�H���g������������B
            switch (NNshParam->currentFont)
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
                fontId = NNshParam->currentFont;
                break;
            }
            NNshParam->currentFont = fontId;
        }
    }
    else
    {
        // TsPatch�@�\���g��Ȃ��ꍇ,,,
        switch (NNshParam->currentFont)
        {
          case stdFont:
          case boldFont:
          case largeFont:
          case largeBoldFont:
            // ���̂܂܎g��
            break;

          default:
            // �W���t�H���g�ɕύX����
            NNshParam->currentFont = stdFont;
            break;
        }
    }
#endif  // #ifdef USE_TSPATCH

#ifdef USE_ARMLET
    // ARMlet�g�p�ۂ̃`�F�b�N
    FtrGet(sysFileCSystem, sysFtrNumProcessorID, &processorType);
    if (sysFtrNumProcessorIsARM(processorType))
    {
        // ARMlet�g�p�\�f�o�C�X�B
        // NNshParam->useARMlet = NNSH_ENABLE;  // ARMlet�͎g�p����ݒ�ɕύX
    }
    else
#endif
    {
        // ARMlet�͎g�p���Ȃ��ݒ�ɂ���
        NNshParam->useARMlet = NNSH_DISABLE;
    }

#if 0
    // VFS��OFF�������ꍇ�ɂ́A�m�F���b�Z�[�W��\������B
    if (NNshParam->useVFS == NNSH_VFS_DISABLE)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_USE_VFS_WARNING, "", 0);
    }
#endif

    // �i�[�o�b�t�@�̈���擾����
    NNshGlobal->maxScriptLine = MAX_MACRO_STEP;
    NNshGlobal->scriptArea    = AllocateMacroDBData(NNshGlobal->maxScriptLine);
    if (NNshGlobal->scriptArea != NULL)
    {
        // DB����NNsi�ݒ�ŋL�^����Ă���X�N���v�g���Ăяo��
        ret = ReadMacroDBData(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT,
                              DATA_CREATOR_ID, 0,
                              &(NNshGlobal->currentScriptLine), 
                              NNshGlobal->maxScriptLine,
                              NNshGlobal->scriptArea);
    }
    NNshGlobal->editFlag = false;
    NNshGlobal->editMode = NNSH_MODE_NORMAL;

    // �ŏ��ɃI�[�v������t�H�[����ID����������B
    return (GetFormID_NNsh(NNshParam->lastFrmID));
}

/*=========================================================================*/
/*   Function :   myStopApplication                                        */
/*                                 �X�g�b�v����(�v���O������)����������  */
/*=========================================================================*/
void MyStopApplication(void)
{
    NNshSoftwareVersionInfo verInfo;
    UInt16                  savedParam;
    
    savedParam = NNshParam->confirmationDisable;
    if (NNshGlobal->editFlag == true)
    {
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_OUTPUT_MACRO, "", 0) == 0)
        {
            // �}�N���ݒ�f�[�^���o�͂���
            NNshParam->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            extractMacroData();
        }
    }
    NNshParam->confirmationDisable = savedParam;

    // �X�N���v�g�̈�̊J��
    if (NNshGlobal->scriptArea != NULL)
    {
        ReleaseMacroDBData(NNshGlobal->scriptArea);
    }

#ifdef USE_CLIE
    // CLIE�T�|�[�g���J������
    ResetCLIESupport_NNsh();
#endif

    // NNsi�ݒ��Version�m�F�pPreference�̕ۑ�(Debug���[�h�͂�����ŋL��)
    verInfo.NNsiVersion = SOFT_PREFVERSION;
    verInfo.prefSize    = sizeof(NNshSavedPref);
    verInfo.prefType    = NNSH_PREFERTYPE;
    verInfo.debugMode   = (NNshParam != NULL) ? NNshParam->debugMessageON : 0;
    PrefSetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, SOFT_PREFVERSION,
                          &verInfo, sizeof(NNshSoftwareVersionInfo), false);

    // NNsi�ݒ�̕ۑ�(NNsi�̋N���ɐ����������̂�)
    if (NNshParam != NULL)
    {
        SaveSetting_NNsh(NNSH_PREFERTYPE, sizeof(NNshSavedPref), NNshParam);
        MemPtrFree(NNshParam);
        NNshParam = NULL;
    }

    // NNsi�̃O���[�o���̈���J������
    MEMFREE_PTR(NNshGlobal);
    return;
}
