/*============================================================================*
 *
 *  $Id: main.c,v 1.65 2004/11/25 13:42:33 mrsa Exp $
 *
 *  FILE:  
 *     main.c
 *
 *  Description: 
 *     main source code for Palm application.
 *     (This file includes in "skeleton.c".)
 *
 *===========================================================================*/
#define MAIN_C

#include "machine.c"

// external functions
extern Err     OpenForm_NNshMessage (FormType  *frm);
extern void   OpenForm_ThreadMain  (FormType  *frm);
extern void   OpenForm_WriteMessage(FormType  *frm);
extern void   OpenForm_SelectBBS   (FormType  *frm);
extern void   OpenForm_newThread   (FormType  *frm);
extern Err     OpenForm_NNshSetting (FormType  *frm);
extern Err     OpenForm_NNsiSetting2(FormType  *frm);
extern Err     OpenForm_NNsiSetting3(FormType  *frm);
extern Err     OpenForm_NNsiSetting4(FormType  *frm);
extern Err     OpenForm_NNsiSetting5(FormType  *frm);
extern Err     OpenForm_NNsiSetting6(FormType  *frm);
extern Err     OpenForm_NNsiSetting7(FormType  *frm);
extern Err     OpenForm_NNsiSetting8(FormType  *frm);
extern Err     OpenForm_NNsiSetting9(FormType  *frm);
extern Err     OpenForm_NNsiSettingA(FormType  *frm);
extern Err     OpenForm_FavorTabSet (FormType  *frm);
extern Err     OpenForm_NNsiDummy   (FormType  *frm);
extern Err     OpenForm_GetLogList  (FormType  *frm);

extern Boolean Handler_ThreadMain   (EventType *event);
extern Boolean Handler_NNshMessage  (EventType *event);
extern Boolean Handler_WriteMessage (EventType *event);
extern Boolean Handler_SelectBBS    (EventType *event);
extern Boolean Handler_NNshSetting  (EventType *event);
extern Boolean Handler_FavorTabSet  (EventType *event);
extern Boolean Handler_newThread    (EventType *event);
extern Boolean Handler_NNsiDummy    (EventType *event);
extern Boolean Handler_GetLogList   (EventType *event);


/*=========================================================================*/
/*   Function :   MyApplicationDispatchEvent                               */
/*                                                ����f�B�X�p�b�`�C�x���g */
/*=========================================================================*/
static Boolean MyApplicationDispatchEvent(EventType *event)
{
    UInt16 *command;

    //  "�n�[�h�L�[���䂷��"�@�w�肪����A���L�[���̓C�x���g�Ȃ�
    // �t�H�[���̏��������{����
    if (((NNshGlobal->NNsiParam)->useHardKeyControl != 0)&&(event->eType == keyDownEvent))
    {
        if (event->data.keyDown.chr >= vchrHardKeyMin)
        {
            // �n�[�h�L�[�R�[�h�̊m�F
            switch (event->data.keyDown.chr)
            {
              case vchrHard1:
                // �n�[�h�L�[�P
                command = &((NNshGlobal->NNsiParam)->useKey.key1);
                break;

              case vchrHard2:
                // �n�[�h�L�[�Q
                command = &((NNshGlobal->NNsiParam)->useKey.key2);
                break;

              case vchrHard3:
                // �n�[�h�L�[�R
                command = &((NNshGlobal->NNsiParam)->useKey.key3);
                break;

              case vchrHard4:
                // �n�[�h�L�[�S
                command = &((NNshGlobal->NNsiParam)->useKey.key4);
                break;

              case vchrCapture:
              case vchrVoiceRec:
              case vchrVZ90functionMenu:      
              case vchrGarminRecord:
              case vchrHard5:
                // CLIE NX �L���v�`���{�^���ACLIE TG�{�C�X���R�{�^��
                command = &((NNshGlobal->NNsiParam)->useKey.clieCapture);
                break;

              case chrLeftArrow:         // �J�[�\���L�[��(5way��)
              case vchrRockerLeft:
              case vchrJogLeft:
              case vchrTapWaveSpecificKeyRLeft:
                // �n�[�h�L�[��
                command = &((NNshGlobal->NNsiParam)->useKey.left);
                break;

              case chrRightArrow:     // �J�[�\���L�[�E(5way�E)
              case vchrRockerRight:
              case vchrJogRight:
              case vchrTapWaveSpecificKeyRRight:
                // �n�[�h�L�[�E
                command = &((NNshGlobal->NNsiParam)->useKey.right);
                break;

              case vchrHWKeyboardPush:
                // CLIE �n�[�h�L�[����(�������Ȃ�)
                return (false);
                break;

              default:
                //  �i�m��Ȃ��j�n�[�h�L�[���͂��s��ꂽ�Ƃ��A�L�[�R�[�h��
                // �\������
                NNsh_DebugMessage(ALTID_INFO, "Hard Key Event ", " chrID:",
                                  event->data.keyDown.chr);
                return (false);
                break;
            }
            // �R�}���h�̔���
            switch (*command)
            {
              case HARDKEY_FEATURE_CONTROLNNSI:
                // NNsi�Ő���(�n�[�h�L�[�������ꂽ�Ƃ��̏��������s)
                FrmDispatchEvent(event);
                return (true);
                break;

              case HARDKEY_FEATURE_TRANSJOGPUSH:
                // �C�x���g�̃L�[�R�[�h�𖳗����JOG PUSH(LineFeed)�ɏ���������
                event->data.keyDown.chr = chrLineFeed;
                break;

              case HARDKEY_FEATURE_TRANSJOGBACK:
                // �C�x���g�̃L�[�R�[�h�𖳗����JOG BACK(ESC)�ɏ���������
                event->data.keyDown.chr = chrEscape;
                break;

              case HARDKEY_FEATURE_DONOTHING:
              default:
                // �������Ȃ�
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
static Boolean MyApplicationHandleEvent(EventType *event)
{
    FormType  *frm;
    TableType *tableP;
    UInt16     rowNum, fieldFont, objFont;

    // SONY�V���N������s���ꍇ�̉�ʍX�V
    if (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY)
    {
         // SILK�}�l�[�W���̕\���ɂ��킹����ʂ̍X�V���s��
        frm = FrmGetActiveForm();
        if (frm != NULL)
        {
            FrmEraseForm(frm);
            NNsi_FrmDrawForm(frm, true);
        }
        NNshGlobal->updateHR = NNSH_NOT_EFFECTIVE;
    }

    // Notify�C�x���g�̎�M������ʉ߂�����
    if (NNshGlobal->notifyUpdate != 0)
    {
        frm = FrmGetActiveForm();
        if (frm != NULL)
        {
            switch (NNshGlobal->notifyUpdate)
            {
              case NNSH_VFS_MOUNTED:
                // VFS��mount���ꂽ���Ƃ����[�U�ɒʒm����
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_MOUNTED, MSG_WARN_VFS_UPDATE, 0);
                break;

              case NNSH_VFS_UNMOUNTED:
                // VFS��unmount���ꂽ���Ƃ����[�U�ɒʒm����
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_UNMOUNTED, " ", 0);
                break;

              case NNSH_VFS_UNMOUNTED_WRITE:
                // VFS��unmount���ꂽ���Ƃ����[�U�ɒʒm����
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_UNMOUNTED, MSG_WARN_BACK_THREAD, 0);

                // WRITE��ʂ���ꗗ��ʂɈړ�����
                SaveWritingMessage();
                FrmEraseForm(frm);
                FrmGotoForm (NNshGlobal->backFormId);
                break;

              case NNSH_VFS_UNMOUNTED_OTHER:
                // VFS��unmount���ꂽ���Ƃ����[�U�ɒʒm����
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_UNMOUNTED, MSG_WARN_BACK_THREAD, 0);

                // �O�ɊJ���Ă������(�ꗗ��� or �_�~�[��� or �Q�Ɖ��)�Ɉړ�����
                FrmEraseForm(frm);
                FrmGotoForm (ConvertFormID((NNshGlobal->NNsiParam)->lastFrmID));
                break;

              case NNSH_VFS_UNMOUNTED_MESSAGE:
                // VFS��unmount���ꂽ���Ƃ����[�U�ɒʒm����
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_UNMOUNTED, MSG_WARN_BACK_THREAD, 0);

                // �Q�Ɖ�ʂ���ꗗ��ʂɈړ�����
                ReadMessage_Close(NNshGlobal->backFormId);
                break;

              case NNSH_DISP_RESIZED:
                // ��ʂ̃T�C�Y���ύX�ɂȂ���...
                HandEraResizeForm(frm);
                UpdateFieldRegion();
                NNsh_DebugMessage(ALTID_INFO, "[[<<DISPLAY RESIZED>>]]", "", 0);

                FrmEraseForm(frm);
                NNsi_FrmDrawForm(frm, true);
                break;

              default:
                break;
            }
        }
        NNshGlobal->notifyUpdate = 0;
    }

    // �����ŁA(�t�H�[�����[�h����)�C�x���g��M���̏������L�q����B
    switch (event->eType)
    {
      // �t�H�[�����J������event
      case frmLoadEvent:
        // �g�p�t�H���g��ݒ肷��
        fieldFont = (NNshGlobal->NNsiParam)->currentFont;
        FntSetFont(fieldFont);
        objFont   = (NNshGlobal->NNsiParam)->currentFont;

        // �t�H�[���������� 
        frm = FrmInitForm(event->data.frmLoad.formID);
#ifdef USE_HANDERA
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaFormModify(frm, vgaFormModify160To240);
        }
#endif
#ifdef USE_PIN_DIA
        if (NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)
        {
            VgaFormModify_DIA(frm, vgaFormModifyNormalToWide);
        }
        else if (NNshGlobal->device == NNSH_DEVICE_GARMIN)
        {
            // ��ʂ��I�[�v�������Ƃ��́A�ĕ`�悷��(Garmin�@)
            VgaFormModify_DIA(frm, vgaFormModifyNormalToWide);
            NNshGlobal->notifyUpdate = NNSH_DISP_RESIZED;
            EvtWakeup();
        }
#endif

#ifdef USE_MACRO
        if (NNshGlobal->tempStatus != NNSH_MACROCMD_EXECUTE)
        {
            NNshGlobal->tempStatus = 0;
        }
#else
        NNshGlobal->tempStatus = 0;
#endif

        // �t�H�[�����J���ăC�x���g�n���h����ݒ肷��
        FrmSetActiveForm(frm);
        switch (event->data.frmLoad.formID)
        {
          // �X���ꗗ���
          case FRMID_THREAD:
            // �O���t�B�e�B�G���A���ŏ������ĉ�ʂ����T�C�Y�A�t�H���g��ύX
            (void) NNshSilkMinimize(frm);

            // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
            OpenForm_ThreadMain(frm);
            FrmSetEventHandler(frm, Handler_ThreadMain);

            // OpenForm_ThreadMain()�ŉ�ʂ�`�悷�邽�߁A�����ɔ�����
            return (true);
            break;

          // �X���Q�Ɖ��
          case FRMID_MESSAGE:
            // �O���t�B�e�B�G���A���ŏ������ĉ�ʂ����T�C�Y�A�t�H���g��ύX
            (void) NNshSilkMinimize(frm);
            FldSetFont(FrmGetObjectPtr(frm, 
                            FrmGetObjectIndex(frm, FLDID_MESSAGE)), fieldFont);
#ifdef USE_GLUE
            CtlGlueSetFont(FrmGetObjectPtr(frm,
                       FrmGetObjectIndex(frm, SELTRID_THREAD_TITLE)), objFont);
#endif
            // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
            OpenForm_NNshMessage(frm);
            FrmSetEventHandler(frm, Handler_NNshMessage);

            // OpenForm_NNshMessage()�ŉ�ʂ�`�悷�邽�߁A�����ɔ�����
            return (true);
            break;
 
          // �X���������݉��
          case FRMID_WRITE_MESSAGE:
            // �O���t�B�e�B�G���A���ő剻���ĉ�ʂ����T�C�Y�A�t�H���g��ύX
            FldSetFont(FrmGetObjectPtr(frm,
                         FrmGetObjectIndex(frm, FLDID_WRITE_NAME)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                        FrmGetObjectIndex(frm, FLDID_WRITE_EMAIL)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                      FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE)), fieldFont);
            if ((NNshGlobal->NNsiParam)->notOpenSilkWrite == 0)
            {
                (void) NNshSilkMaximize(frm);
            }
            else
            {
                (void) NNshSilkMinimize(frm);
            }

            // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
            OpenForm_WriteMessage(frm);
            FrmSetEventHandler(frm, Handler_WriteMessage);
            break;

          // BBS�Ǘ����
          case FRMID_MANAGEBBS:
            // �O���t�B�e�B�G���A���ŏ������ĉ�ʂ����T�C�Y����
            if (NNshSilkMinimize(frm) == errNone)
            {
                // CLIE UX�ȊO�̂Ƃ��͊g��A�A�A�i�����[�x�^�A�A�A�j
#ifdef USE_CLIE
                if (NNshGlobal->silkVer != 3)
#endif
                {
                    // �̈�T�C�Y�����߂�
                    tableP = FrmGetObjectPtr(frm, 
                                   FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
                    for (rowNum = 0;
                         rowNum < TblGetNumberOfRows(tableP); rowNum++)
                    {
                        TblSetRowHeight(tableP, rowNum, 15);
                    }
                }
            }
            // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
            OpenForm_SelectBBS(frm);
            FrmSetEventHandler(frm, Handler_SelectBBS);
            break;

          // NNsi�ݒ���
          case FRMID_CONFIG_NNSH:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNshSetting(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�-2���
          case FRMID_NNSI_SETTING2:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting2(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�-3���
          case FRMID_NNSI_SETTING3:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting3(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�-4���
          case FRMID_NNSI_SETTING4:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting4(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�-5���
          case FRMID_NNSI_SETTING5:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting5(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�-6���
          case FRMID_NNSI_SETTING6:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting6(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�-7���
          case FRMID_NNSI_SETTING7:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting7(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�-8���
          case FRMID_NNSI_SETTING8:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting8(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�-9���
          case FRMID_NNSI_SETTING9:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting9(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi�ݒ�(�T��)���
          case FRMID_NNSI_SETTING_ABSTRACT:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSettingA(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // ���C�ɓ�������ݒ���
          case FRMID_FAVORSET_DETAIL:
            // �O���t�B�e�B�̈���ő剻�A�t�H�[�����J���ăC�x���g�n���h����ݒ�
            (void) NNshSilkMaximize(frm);
            OpenForm_FavorTabSet(frm);
            FrmSetEventHandler(frm, Handler_FavorTabSet);
            break;

          // NNsi�N��(DUMMY)���
          case FRMID_NNSI_DUMMY:
            // �O���t�B�e�B�G���A���ŏ������ĉ�ʂ����T�C�Y�A�t�H���g��ύX
            (void) NNshSilkMinimize(frm);
            OpenForm_NNsiDummy(frm);
            FrmSetEventHandler(frm, Handler_NNsiDummy);
            break;

          // �X�����ĉ��
          case FRMID_NEW_THREAD:
            // �O���t�B�e�B�G���A���ő剻���ĉ�ʂ����T�C�Y�A�t�H���g��ύX
            FldSetFont(FrmGetObjectPtr(frm,
                         FrmGetObjectIndex(frm, FLDID_WRITE_NAME)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                        FrmGetObjectIndex(frm, FLDID_WRITE_EMAIL)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                      FrmGetObjectIndex(frm, FLDID_NEWTHREAD_TITLE)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                      FrmGetObjectIndex(frm, FLDID_NEWTHREAD_MESSAGE)), fieldFont);
            if ((NNshGlobal->NNsiParam)->notOpenSilkWrite == 0)
            {
                (void) NNshSilkMaximize(frm);
            }
            else
            {
                (void) NNshSilkMinimize(frm);
            }

            // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
            OpenForm_newThread(frm);
            FrmSetEventHandler(frm, Handler_newThread);
            break;

#ifdef USE_LOGCHARGE
          // �Q�ƃ��O�ꗗ���
          case FRMID_LISTGETLOG:
            // �O���t�B�e�B�G���A���ŏ������ĉ�ʂ����T�C�Y�A�t�H���g��ύX
            (void) NNshSilkMinimize(frm);
            OpenForm_GetLogList(frm);
            FrmSetEventHandler(frm, Handler_GetLogList);

            // OpenForm_GetLogList()�ŉ�ʂ�`�悷�邽�߁A�����ɔ�����
            return (true);
            break;
#endif // #ifdef USE_LOGCHARGE

          default:
            // ����"�t�H�[���J������C�x���g"���\�����Ă݂�B(�f�o�b�O)
            NNsh_DebugMessage(ALTID_INFO, "frmLoadEvent()", " formID:",
                              event->data.frmLoad.formID);
            break;
        }
        // ��ʂ�`�悷��
        NNsi_FrmDrawForm(frm, true);
        break;

#ifdef USE_PIN_DIA
      case winEnterEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
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
static UInt32  MyApplicationHandleNotifications(SysNotifyParamType *notifyParamsP)
{ 
  // PilotMain�����Ă�Code Resource�̏ꍇ�͂�������Notification����������̂���ʓI
  // ���ĂȂ��ꍇ��SysNotifyRegister��function pointer��n���Ă��

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
static UInt16 MyStartApplication(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
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

    // �r�[����M�o�^������...
    ExgRegisterData(SOFT_CREATOR_ID, exgRegExtensionID, NNSI_EXCHANGEINFO_SUFFIX); 

    // NNsi�ݒ�o�[�W�����̊m�F
    //   (Version�m�F�pPreference���m�F���A�ύX���Ȃ���Ε���)
    size = sizeof(NNshSoftwareVersionInfo);
    MemSet(&verInfo, sizeof(NNshSoftwareVersionInfo), 0x00);
    ret  = PrefGetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, 
                                 &verInfo, &size, false);
    if (ret == noPreferenceFound)
    {
        //  preference�����݂��Ȃ������ꍇ�ɂ́A�o�[�W�����ԍ���
        // NNsi�ݒ�̃T�C�Y�͐��������̂Ƃ��ď�������B
        ret              = SOFT_PREFVERSION;
        verInfo.prefSize = sizeof(NNshSavedPref);
    }

    // NNsi�O���[�o���̈�p�i�[�̈�̊m��
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MEMALLOC_PTR(size);
    if (NNshGlobal == NULL)
    {
        // �̈�m�ۂɎ��s�I(NNsi�͋N�����Ȃ�)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_NNSI_CANNOT_LAUNCH,
                          "(NNsiGbl)\nsize:", size);
        return (0);
    }        
    MemSet (NNshGlobal, size, 0x00);
    StrCopy(NNshGlobal->dummyBuffer, " ");

    // �J���������́A�X���ꗗ��ʂɖ߂�悤�ɂ���B
    // �� ���� ��
    //    �Q�ƃ��O�ꗗ���Q�Ɖ�ʂŊJ�������ƁANNsi���ċN�����Ė߂�ƁA
    //   ���̂܂܂��ƁA�ʏ�̈ꗗ��ʂɑJ�ڂ��Ă��܂��B
    NNshGlobal->backFormId = FRMID_THREAD;

    // NNsi�ݒ�p�i�[�̈�̊m��
    size = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    (NNshGlobal->NNsiParam) = (NNshSavedPref *) MEMALLOC_PTR(size);
    if ((NNshGlobal->NNsiParam) == NULL)
    {
        // �̈�m�ۂɎ��s�I(NNsi�͋N�����Ȃ�)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_NNSI_CANNOT_LAUNCH,
                          "(NNsiSet)\nsize:", size);
        return (0);
    }
    MemSet((NNshGlobal->NNsiParam), size, 0x00);

    // OS�o�[�W�����̊m�F
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osVersion) == errNone)
    {
        NNshGlobal->palmOSVersion = osVersion;
    }

    // NNsi�ݒ�𕜋�������
    (void) NNshRestoreNNsiSetting(ret, verInfo.prefSize, (NNshGlobal->NNsiParam));

    // �f�o�b�O���[�h�̏��𕜋�������
    (NNshGlobal->NNsiParam)->debugMessageON = verInfo.debugMode;

    // VFS�@�\���Ȃ��ݒ肾�����ꍇ�ɂ́A��UVFS�p�����[�^���N���A����B
    if ((NNshGlobal->NNsiParam)->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        (NNshGlobal->NNsiParam)->useVFS = NNSH_VFS_DISABLE;
    }
    // VFS�T�|�[�g�̐ݒ�
    checkVFSdevice_NNsh();

    // VFS ��Ƀo�b�N�A�b�v���ꂽDB������΁A������R�s�[���邩�ǂ���
    if (((NNshGlobal->NNsiParam)->useVFS & (NNSH_VFS_ENABLE | NNSH_VFS_DBIMPORT)) ==
        (NNSH_VFS_ENABLE | NNSH_VFS_DBIMPORT))
    {
        UInt16 backup;
        
        backup = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_NOTHING;

        // VFS ���� DB ���R�s�[����
        RestoreDBtoVFS(false);
        if (NNshGlobal->restoreSetting)
        {
            // NNsi�ݒ�𕜋�������
            (void) NNshRestoreNNsiSetting(ret, verInfo.prefSize, (NNshGlobal->NNsiParam));
        }
        else
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = backup;
        }
    }

#ifdef USE_CLIE
    // Silk����̐ݒ�𔽉f
    NNshGlobal->notUseSilk = (NNshGlobal->NNsiParam)->notUseSilk;

    // CLIE�T�|�[�g�̐ݒ�
    setCLIESupport_NNsh();
#endif  // #ifdef USE_CLIE

#ifdef USE_HANDERA
    // HandEra�T�|�[�g�̐ݒ�
    setHandEraSupport_NNsh();
#endif // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
    // pen input manager�̐ݒ�
    setDIASupport_NNsh();
#endif    // #ifdef USE_PIN_DIA

#ifdef USE_HIGHDENSITY
    // ���𑜓x�T�|�[�g/�J���[�T�|�[�g�̃`�F�b�N
    setDoubleDensitySupport_NNsh();
#else
    // ���𑜓x�T�|�[�g�ݒ��������
    NNshGlobal->os5HighDensity = (UInt32) kDensityLow;
#endif    // #ifdef USE_HIGHDENSITY

#ifdef USE_TSPATCH
    // TsPatch�Ή��R�[�h...(TsPatch�K�p�����ǂ����`�F�b�N����)
    if ((NNshGlobal->NNsiParam)->notUseTsPatch == 0)
    {
        ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
        if (ret == errNone)
        {    
            // TsPatch�K�p��...�g�p�t�H���g������������B
            switch ((NNshGlobal->NNsiParam)->currentFont)
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
                fontId = (NNshGlobal->NNsiParam)->currentFont;
                break;
            }
            (NNshGlobal->NNsiParam)->currentFont = fontId;
        }
    }
    else
    {
        // TsPatch�@�\���g��Ȃ��ꍇ,,,
        switch ((NNshGlobal->NNsiParam)->currentFont)
        {
          case stdFont:
          case boldFont:
          case largeFont:
          case largeBoldFont:
            // ���̂܂܎g��
            break;

          default:
            // �W���t�H���g�ɕύX����
            (NNshGlobal->NNsiParam)->currentFont = stdFont;
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
        // (NNshGlobal->NNsiParam)->useARMlet = NNSH_ENABLE;  // ARMlet�͎g�p����ݒ�ɕύX
    }
    else
#endif
    {
        // ARMlet�͎g�p���Ȃ��ݒ�ɂ���
        (NNshGlobal->NNsiParam)->useARMlet = NNSH_DISABLE;
    }

    // WebBrowser�̃C���X�g�[���`�F�b�N
    checkInstalledWebBrowser();

    // ������ݒ肳��Ă����ꍇ
    if ((NNshGlobal->NNsiParam)->useBookmark != 0)
    {
        // ������ݒ�L���̈���m�ۂ���
        NNshGlobal->bookmarkFileName = MEMALLOC_PTR(MAX_THREADFILENAME);
        if (NNshGlobal->bookmarkFileName != NULL)
        {
            NNshGlobal->bookmarkNick = MEMALLOC_PTR(MAX_NICKNAME);
            if (NNshGlobal->bookmarkNick == NULL)
            {
                MEMFREE_PTR(NNshGlobal->bookmarkFileName);
            }
        }
        if ((NNshGlobal->NNsiParam)->lastFrmID == NNSH_FRMID_MESSAGE)
        {
            // �����肪�ݒ肳��Ă����ꍇ�ɂ́A�J���ꏊ���w��
            if ((NNshGlobal->NNsiParam)->bookMsgNumber != 0)
            {
                (NNshGlobal->NNsiParam)->openMsgIndex = (NNshGlobal->NNsiParam)->bookMsgIndex;
            }
        }
    }

    // NG�ݒ�̕�����`�F�b�N
    if ((NNshGlobal->NNsiParam)->useRegularExpression != 0)
    {
        SeparateWordList((NNshGlobal->NNsiParam)->hideWord1, &(NNshGlobal->hide1));
        SeparateWordList((NNshGlobal->NNsiParam)->hideWord2, &(NNshGlobal->hide2));
    }

    // �^�C�g�����R�[�h�̎w�肪�ُ�̏ꍇ�A���R�[�h�ԍ�������������
    if (((NNshGlobal->NNsiParam)->startTitleRec == (NNshGlobal->NNsiParam)->endTitleRec)&&
        ((NNshGlobal->NNsiParam)->startTitleRec == 0))
    {
        (NNshGlobal->NNsiParam)->startTitleRec = 0xffff;
    }

    // �N�����ɋN�����[�h���ݒ肳��Ă����ꍇ�A�_�~�[��ʂ��J��
    if (StrLen((NNshGlobal->NNsiParam)->launchPass) != 0)
    {
        return (FRMID_NNSI_DUMMY);
    }

    // VFS��OFF�������ꍇ�ɂ́A�m�F���b�Z�[�W��\������B
    if ((NNshGlobal->NNsiParam)->useVFS == NNSH_VFS_DISABLE)
    {
        NNsh_WarningMessage(ALTID_WARN, MSG_USE_VFS_WARNING1,
                            MSG_USE_VFS_WARNING2, 0);
    }

    // ZLIB�̃T�|�[�g�ۊm�F���s��
    SetZLIBsupport_NNsh();
        
#ifdef USE_MACRO
    // �N����1�񂾂����s����}�N�������݂����ꍇ�A�A�A
    if (DmFindDatabase(0, DBNAME_RUNONCEMACRO) != 0)
    {
        NNshGlobal->tempStatus = NNSH_MACROCMD_OPERATE;
        return (FRMID_THREAD);
    }

    // �}�N���̎������s���ݒ肳��Ă����ꍇ�ɂ́A�ꗗ��ʂ��J��
    if ((NNshGlobal->NNsiParam)->autostartMacro != 0)
    {
        NNshGlobal->tempStatus = NNSH_MACROCMD_EXECUTE;
        return (FRMID_THREAD);
    }
#endif

    // �ŏ��ɃI�[�v������t�H�[����ID����������B
    return (ConvertFormID((NNshGlobal->NNsiParam)->lastFrmID));
}

/*=========================================================================*/
/*   Function :   myStopApplication                                        */
/*                                 �X�g�b�v����(�v���O������)����������  */
/*=========================================================================*/
static void MyStopApplication(void)
{
    UInt16                  savedParam, frmId;
    NNshSoftwareVersionInfo verInfo;

    frmId = FrmGetActiveFormID();
    switch (frmId)
    {
      case FRMID_WRITE_MESSAGE:
        //  �������݉�ʂŃA�v������悤�Ƃ����ꍇ
        // (�������ݓ��e��ۑ����邩�m�F����)
        SaveWritingMessage();
        break;
      case FRMID_MESSAGE:
        //  �Q�ƃ��O�ꗗ��ʂ���J�����X���Q�ƒ��̏ꍇ�ɂ́A
        // ����N�����̓X���Q�Ɖ�ʂ��J���悤�ɂ���B(����̍�...)
        if (NNshGlobal->backFormId == FRMID_LISTGETLOG)
        {
            if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&
                 ((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
            {
                // ������ݒ蒆�Ȃ̂ŁA�������Ȃ�
            }
            else
            {
                (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_GETLOGLIST;
            }
        }
        break;

      default:
        break;
    }

    // VFS Notification�̉��
    resetVFSNotifications();

    // NNsi�ݒ��Version�m�F�pPreference�̕ۑ�(Debug���[�h�͂�����ŋL��)
    verInfo.NNsiVersion = SOFT_PREFVERSION;
    verInfo.prefSize    = sizeof(NNshSavedPref);
    verInfo.prefType    = NNSH_PREFERTYPE;
    verInfo.debugMode   = ((NNshGlobal->NNsiParam) != NULL) ? (NNshGlobal->NNsiParam)->debugMessageON : 0;
    PrefSetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, SOFT_PREFVERSION,
                          &verInfo, sizeof(NNshSoftwareVersionInfo), false);

    // NNsi�̃O���[�o���̈悪�m�ۂ���Ă��Ȃ��Ƃ��͏I������
    if (NNshGlobal == NULL)
    {
        return;
    }

    // �X���^�C�g���\�������폜(�ꗗ���)
    ClearMsgTitleInfo();

#ifdef USE_CLIE
    // CLIE�T�|�[�g���J������
    resetCLIESupport_NNsh();
#endif

    // NNsi�ݒ�̕ۑ�(NNsi�̋N���ɐ����������̂�)
    if ((NNshGlobal->NNsiParam) != NULL)
    {
        // �e���|�����t�@�C���̍폜(DEBUG���[�h���ɂ͎c���Ă���)
        if ((NNshGlobal->NNsiParam)->debugMessageON == 0)
        {
            (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
            (void) DeleteFile_NNsh(FILE_RECVMSG, NNSH_VFS_DISABLE);
        }

        // NNsi�I�����ɉ����ؒf����
        if ((NNshGlobal->NNsiParam)->disconnectNNsiEnd != 0)
        {
            // �v���O�����I�����ɂ́A�u����ؒf�v�̊m�F���b�Z�[�W�͏o���Ȃ��B
            savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            NNshNet_LineHangup();
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
        }

        // Silk����̐ݒ�𔽉f
#ifdef USE_CLIE
        if ((NNshGlobal->NNsiParam)->notUseSilk != NNshGlobal->notUseSilk)
        {
            (NNshGlobal->NNsiParam)->notUseSilk = NNshGlobal->notUseSilk;
        }
#endif  // #ifdef USE_CLIE
        NNshSaveNNsiSetting(NNSH_PREFERTYPE, sizeof(NNshSavedPref), (NNshGlobal->NNsiParam));

        // NNsi�I�����Ƀf�[�^�x�[�X��VFS�Ƀo�b�N�A�b�v����
        if (((NNshGlobal->NNsiParam)->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            BackupDBtoVFS(NNSH_DISABLE);
        }

        // �u������ݒ�v�ݒ蒆�̏ꍇ
        if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&
            ((NNshGlobal->NNsiParam)->bookMsgNumber != 0)&&
            (NNshGlobal->bookmarkFileName != NULL))
        {
            // ������ݒ�(index�ԍ�)���L������
            (void) GetSubjectIndex(NNshGlobal->bookmarkNick,
                                    NNshGlobal->bookmarkFileName,
                                    &((NNshGlobal->NNsiParam)->bookMsgIndex));
        }
    }

    // NNsi�O���[�o���̈�i�̒��g�j���J������
    NNshClearGlobal(NNshGlobal);

    // NNsi�ݒ�̎g�p�̈���J������
#ifdef RELEASE_NNSISETTING
    //MEMFREE_PTR((NNshGlobal->NNsiParam));
    MemPtrFree((NNshGlobal->NNsiParam));
#endif

    // NNsi�̃O���[�o���̈���J������
#ifdef RELEASE_NNSIGLOBAL
    // MEMFREE_PTR(NNshGlobal);
    MemPtrFree(NNshGlobal);
#endif

    return;
}

/*=========================================================================*/
/*   Function : MyApplicationReceiveData                                   */
/*                            Exchange�}�l�[�W�����g�p���ăf�[�^����M���� */
/*=========================================================================*/
UInt32 MyApplicationReceiveData(void *cmdPBP, UInt16 launchFlags)
{
    ExgSocketType      *exgSocket;
    NNshGetLogDatabase *logP;
    Err                 err;
    UInt8              *dbName, *token, entried;
    UInt8              *buff, *ptr, *data;
    UInt16              len;
    UInt32              size, ret;
    MemHandle           strH;
    DmOpenRef           dbRef;

    // �A�v���P�[�V�������N�������ǂ������m�F����t���O
    entried    = 0;

    // ��M�f�[�^�T�C�Y�̊m��
    exgSocket = (ExgSocketType *) cmdPBP;
    size = (BUFSIZE > exgSocket->length) ? BUFSIZE : exgSocket->length;

    buff = MEMALLOC_PTR(size + MARGIN);
    if (buff == NULL)
    {
        // �G���[...
        return (false);
    }
    MemSet(buff, (size + MARGIN), 0x00);
    
    // DB���̈�̊m��
    dbName = MEMALLOC_PTR(BUFSIZE + MARGIN);
    if (dbName == NULL)
    {
        MEMFREE_PTR(buff);
        return (false);
    }
    MemSet(dbName, (BUFSIZE + MARGIN), 0x00);

    // �g�[�N���̈�̊m��
    token = MEMALLOC_PTR(BUFSIZE + MARGIN);
    if (token == NULL)
    {
        MEMFREE_PTR(dbName);
        MEMFREE_PTR(buff);
        return (false);
    }
    MemSet(dbName, (BUFSIZE + MARGIN), 0x00);

    // �Q�ƃ��O�p�c�a���m�ۂ���
    logP = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (logP == NULL)
    {
        MEMFREE_PTR(token);
        MEMFREE_PTR(dbName);
        MEMFREE_PTR(buff);
        return (false);
    }
    
    // ��M�f�[�^�̎󂯕t��
    err = ExgAccept(exgSocket);
    if (err != errNone)
    {
        // ��M���s...
        goto FUNC_END;
    }

    // �f�[�^�������ɓ��B����܂Ŏ�M����
    ptr = buff;
    while (1)
    {
        ret = ExgReceive(exgSocket, ptr, size, &err);
        if ((ret == 0)||(err == exgErrUserCancel)||(err == exgErrNotSupported))
        {
            // �]���I���A�������̓G���[�I���Ȃ̂Ŕ�����
            break;
        }

        // �f�[�^�̊m�F
        MemSet(token, sizeof(token), 0x00);
        strH = DmGetResource('tSTR', NNSI_STRINGTAG_START_NNSIEXT_THREADNUMBER);
        if (strH != 0)
        {
            StrCopy(token, MemHandleLock(strH));
            MemHandleUnlock(strH);
        }
        else
        {
            token[0] = '<';
            token[1] = 'N';
            token[2] = 'N';
            token[3] = 's';
            token[4] = 'i';
            token[5] = ':';
        }
        data = StrStr(ptr, token);
        if (data != NULL)
        {
            // �X���ԍ��̒��o
            data = data + 28; // 28�́AStrLen(NNSI_NNSIEXT_THREADNUMBER_START);
            ptr = data;
            while ((*ptr != '<')&&(*ptr != '\0'))
            {
                ptr++;
            }
            *ptr = '\0';

            // DB����o�^����
            MemSet(dbName, sizeof(dbName), 0x00);
            dbName[0] = 'G';
            dbName[1] = 'e';
            dbName[2] = 't';
            dbName[3] = 'R';
            dbName[4] = 'e';
            dbName[5] = 's';
            dbName[6] = 'e';
            dbName[7] = 'r';
            dbName[8] = 'v';
            dbName[9] = 'e';
            dbName[10] = '-';
            dbName[11] = 'N';
            dbName[12] = 'N';
            dbName[13] = 's';
            dbName[14] = 'i';

            // �X���ԍ���o�^����...
            //  (OpenDatabase_NNsh(dbName,DBVERSION_GETRESERVE,&dbRef); �Ɠ���)
           {
                LocalID dbId;

                dbRef = 0;
                dbId  = DmFindDatabase(0, dbName);
                if (dbId == 0)
                {
                    // �f�[�^�x�[�X��V�K�쐬
                    (void) DmCreateDatabase(0, dbName, 
                                       SOFT_CREATOR_ID, SOFT_DBTYPE_ID, false);
                    dbId      = DmFindDatabase(0, dbName);
                    if (dbId != 0)
                    {
                        UInt16 dbVersion;
                        dbVersion = DBVERSION_GETRESERVE;
                        (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, 
                                                      &dbVersion, NULL,
                                                      NULL,NULL,NULL,NULL,
                                                      NULL,NULL,NULL);
                    }
                }

                if (dbId != 0)
                {
                    dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);
                }
            }
            if (dbRef != 0)
            {
                // �c�a�ɓo�^����
                // (void) EntryRecord_NNsh(dbRef, BUFSIZE, data); �Ɠ���
                {
                        MemHandle  newRecH;
                        UInt16     index;

                        // ���R�[�h�̐V�K�ǉ�
                        index   = 0;
                        newRecH = DmNewRecord(dbRef, &index, 
                                                    BUFSIZE + sizeof(UInt32));
                        if (newRecH != 0)
                        {
                            // ���R�[�h�Ƀf�[�^��������
                            DmWrite(MemHandleLock(newRecH), 0, data, BUFSIZE);
                            MemHandleUnlock(newRecH);

                            // ���R�[�h������i�ύX�I���j
                            DmReleaseRecord(dbRef, index, true);
                        }
                }
                // CloseDatabase_NNsh(dbRef); �Ɠ���
                {
                    DmCloseDatabase(dbRef);
                    dbRef = 0;
                }
                entried = 1;
            }
        }
        else
        {
            // DB����o�^����
            MemSet(dbName, sizeof(dbName), 0x00);
            dbName[0] = 'L';
            dbName[1] = 'o';
            dbName[2] = 'g';
            dbName[3] = 'P';
            dbName[4] = 'e';
            dbName[5] = 'r';
            dbName[6] = 'm';
            dbName[7] = 'a';
            dbName[8] = 'n';
            dbName[9] = 'e';
            dbName[10] = 'n';
            dbName[11] = 't';
            dbName[12] = '-';
            dbName[13] = 'N';
            dbName[14] = 'N';
            dbName[15] = 's';
            dbName[16] = 'i';

            // �f�[�^�̐擪��������...
            MemSet(token, sizeof(token), 0x00);
            strH = DmGetResource('tSTR', NNSI_STRINGTAG_START_NNSIEXT_URL);
            if (strH != 0)
            {
                StrCopy(token, MemHandleLock(strH));
                MemHandleUnlock(strH);
            }
            else
            {
                token[0] = '<';
                token[1] = 'N';
                token[2] = 'N';
                token[3] = 's';
                token[4] = 'i';
                token[5] = ':';
            }

            // URL�̊m�F
            data = StrStr(ptr, token);
            if (data != NULL)
            {
                // URL�̒��o
                data = data + 25; // 25�́AStrLen(NNSI_NNSIEXT_URL_START);

                MemSet(token, sizeof(token), 0x00);
                strH = DmGetResource('tSTR', NNSI_STRINGTAG_END);
                if (strH != 0)
                {
                    StrCopy(token, MemHandleLock(strH));
                    MemHandleUnlock(strH);

                    // �f�[�^�̖�����T��
                    ptr = StrStr(data, token);
                    if (ptr != NULL)
                    {
                        *ptr = '\0';
                    }
                }

                // URL���Q�ƃ��O�pDB�ɓo�^����...
                // OpenDatabase_NNsh(dbName, DBVERSION_PERMANENTLOG, &dbRef);
                {
                    LocalID dbId;

                    dbRef = 0;
                    dbId  = DmFindDatabase(0, dbName);
                    if (dbId == 0)
                    {
                        // �f�[�^�x�[�X��V�K�쐬
                        (void) DmCreateDatabase(0, dbName, 
                                       SOFT_CREATOR_ID, SOFT_DBTYPE_ID, false);
                        dbId      = DmFindDatabase(0, dbName);
                        if (dbId != 0)
                        {
                            UInt16 dbVersion;
                            dbVersion = DBVERSION_PERMANENTLOG;
                            (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, 
                                                      &dbVersion, NULL,
                                                      NULL,NULL,NULL,NULL,
                                                      NULL,NULL,NULL);
                        }
                    } 
                    if (dbId != 0)
                    {
                        dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);
                    }
                }
                if (dbRef != 0)
                {
                    // �f�[�^�̈�̏�����
                    MemSet(logP, (sizeof(NNshGetLogDatabase) + MARGIN), 0x00);

                    // �A�h���X(URL)�����o��
                    ptr = logP->dataURL;
                    len = 0;
                    while ((*data != '"')&&(len < MAX_GETLOG_URL))
                    {
                        *ptr = *data;
                        ptr++;
                        data++;
                        len++;
                    }
                    data++;

                    // �^�C�g���𒊏o����
                    ptr = logP->dataTitle;
                    len = 0;
                    while ((*data != '\0')&&(*data != '"')&&(len < MAX_TITLE_LABEL))
                    {
                        *ptr = *data;
                        ptr++;
                        data++;
                        len++;
                    }

                    // �Q�ƃ��O�̂c�a�ɓo�^����
                    // EntryRecord_NNsh(dbRef,sizeof(NNshGetLogDatabase),logP);
                    {
                        MemHandle  newRecH;
                        UInt16     index;

                        // ���R�[�h�̐V�K�ǉ�
                        index   = 0;
                        newRecH = DmNewRecord(dbRef, &index, 
                                 sizeof(NNshGetLogDatabase) + sizeof(UInt32));
                        if (newRecH != 0)
                        {
                            // ���R�[�h�Ƀf�[�^��������
                            DmWrite(MemHandleLock(newRecH), 0, logP,
                                                   sizeof(NNshGetLogDatabase));
                            MemHandleUnlock(newRecH);

                            // ���R�[�h������i�ύX�I���j
                            DmReleaseRecord(dbRef, index, true);
                        }
                    }
                    // CloseDatabase_NNsh(dbRef);
                    {
                        DmCloseDatabase(dbRef);
                        dbRef = 0;
                    }
                    entried = 2;
                }
            }
        }
        ptr = buff;
    }
    // ����ؒf
    err = ExgDisconnect(exgSocket, err);

FUNC_END:
    // URL��M���̏��_�C�A���O
    if (entried == 2)
    {
        strH = DmGetResource('tSTR', MSG_ENTRIED_URL);
        if (strH != 0)
        {
            // URL����M�����Ƃ��̃_�C�A���O��\������
            FrmCustomAlert(ALTID_WARN, MemHandleLock(strH), "", "");
        }
        MemHandleUnlock(strH);
    }
    
    // �X���ԍ���M���̏��_�C�A���O
    if (entried == 1)
    {
        strH = DmGetResource('tSTR', MSG_ENTRIED_THREADNUMBER);
        if (strH != 0)
        {
            // �X���ԍ�����M�����Ƃ��̃_�C�A���O��\������
            FrmCustomAlert(ALTID_WARN, MemHandleLock(strH), "", "");
        }
        MemHandleUnlock(strH);
    }

    // �̈�̊J��
    MEMFREE_PTR(token);
    MEMFREE_PTR(dbName);
    MEMFREE_PTR(logP);
    MEMFREE_PTR(buff);
    return (false);
}

/*=========================================================================*/
/*   Function :   checkLaunchCode                                          */
/*                                                       �N���R�[�h�̊m�F  */
/*=========================================================================*/
Boolean checkLaunchCode(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    switch (cmd)
    {
      default:
        // �N���R�[�h�̃`�F�b�N...
        break;
    }

    // ����ȊO�̋N���R�[�h...

    // �Ƃ肠�����́A�r�[����M�o�^������...
    ExgRegisterData(SOFT_CREATOR_ID, exgRegExtensionID, NNSI_EXCHANGEINFO_SUFFIX); 
    return (false);
}
