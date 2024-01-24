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

#include "machine.c"

// external functions
extern Err     OpenForm_NNshMessage (FormType  *frm);
extern void    OpenForm_ThreadMain  (FormType  *frm);
extern void    OpenForm_WriteMessage(FormType  *frm);
extern Err     OpenForm_SelectBBS   (FormType  *frm);
extern Err     OpenForm_NNshSetting (FormType  *frm);
extern Err     OpenForm_NNsiSetting2(FormType  *frm);
extern Err     OpenForm_NNsiSetting3(FormType  *frm);
extern Err     OpenForm_NNsiSetting4(FormType  *frm);

extern Boolean Handler_ThreadMain   (EventType *event);
extern Boolean Handler_NNshMessage  (EventType *event);
extern Boolean Handler_WriteMessage (EventType *event);
extern Boolean Handler_SelectBBS    (EventType *event);
extern Boolean Handler_NNshSetting  (EventType *event);

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
#ifdef USE_CLIE
    if ((NNshGlobal->silkRef != 0)&&
        (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY))
    {
         // SILK�}�l�[�W���̕\���ɂ��킹����ʂ̍X�V���s��
        frm = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm);
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
          // �X���ꗗ���
          case FRMID_THREAD:
            // �O���t�B�e�B�G���A���ŏ������ĉ�ʂ����T�C�Y�A�t�H���g��ύX
            (void) NNshSilkMinimize(frm);
#ifdef USE_GLUE
            LstGlueSetFont(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, 
                                           LSTID_SELECTION_THREAD)), objFont);
#endif
            // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
            OpenForm_ThreadMain(frm);
            FrmSetEventHandler(frm, Handler_ThreadMain);
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
            return (false);
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
            (void) NNshSilkMaximize(frm);

            // �t�H�[�����J���āA�C�x���g�n���h����ݒ肷��
            OpenForm_WriteMessage(frm);
            FrmSetEventHandler(frm, Handler_WriteMessage);
            break;

          // BBS�Ǘ����
          case FRMID_MANAGEBBS:
            // �O���t�B�e�B�G���A���ŏ������ĉ�ʂ����T�C�Y����
            if (NNshSilkMinimize(frm) == errNone)
            {
                // �̈�T�C�Y�����߂�
                tableP = FrmGetObjectPtr(frm, 
                                 FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
                for (rowNum = 0; rowNum < TblGetNumberOfRows(tableP); rowNum++)
                {
                    TblSetRowHeight(tableP, rowNum, 15);
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

          default:
            // ����"�t�H�[���J������C�x���g"���\�����Ă݂�B(�f�o�b�O)
            NNsh_DebugMessage(ALTID_INFO, "frmLoadEvent()", " formID:",
                              event->data.frmLoad.formID);
            break;
        }
        // ��ʂ�`�悷��
        NNsi_FrmDrawForm(frm);
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
/*   Function :   myStartApplication                                       */
/*                                 �X�^�[�g����(�v���O������)����������  */
/*=========================================================================*/
static UInt16 MyStartApplication(void)
{
    Int16                   ret;
    UInt16                  size;
    NNshSoftwareVersionInfo verInfo;
#ifdef USE_ARMLET
    UInt32                  processorType;
#endif

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
        NNsh_ErrorMessage(ALTID_ERROR, MSG_NNSI_CANNOT_LAUNCH,
                          "(NNsiSet)\nsize:", size);
        return (0);
    }
    MemSet(NNshParam, size, 0x00);

    // NNsi�O���[�o���̈�p�i�[�̈�̊m��
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // �̈�m�ۂɎ��s�I(NNsi�͋N�����Ȃ�)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_NNSI_CANNOT_LAUNCH,
                          "(NNsiGbl)\nsize:", size);
        return (0);
    }        
    MemSet (NNshGlobal, size, 0x00);
    StrCopy(NNshGlobal->dummyBuffer, " ");

    // NNsi�ݒ�𕜋�������
    (void) NNshRestoreNNsiSetting(ret, verInfo.prefSize, NNshParam);

    // �f�o�b�O���[�h�̏��𕜋�������
    NNshParam->debugMessageON = verInfo.debugMode;

    // VFS�@�\���Ȃ��ݒ肾�����ꍇ�ɂ́A��UVFS�p�����[�^���N���A����B
    if (NNshParam->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        NNshParam->useVFS = NNSH_VFS_DISABLE;
    }
    // VFS�T�|�[�g�̐ݒ�
    checkVFSdevice_NNsh();

#ifdef USE_CLIE
    // Silk����̐ݒ�𔽉f
    NNshGlobal->notUseSilk = NNshParam->notUseSilk;

    // CLIE�T�|�[�g�̐ݒ�
    setCLIESupport_NNsh();
#endif  // #ifdef USE_CLIE

#ifdef USE_HANDERA
    // HandEra�T�|�[�g�̐ݒ�
    setHandEraSupport_NNsh();
#endif // #ifdef USE_HANDERA

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

    // WebBrowser�̃C���X�g�[���`�F�b�N
    checkInstalledWebBrowser();

    // VFS��OFF�������ꍇ�ɂ́A�m�F���b�Z�[�W��\������B
    if (NNshParam->useVFS == NNSH_VFS_DISABLE)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_USE_VFS_WARNING1,
                           MSG_USE_VFS_WARNING2, 0);
    }

    // ������ݒ肳��Ă����ꍇ�A�J���ꏊ���w��
    if ((NNshParam->useBookmark != 0)&&(NNshParam->lastFrmID == FRMID_MESSAGE))
    {
        NNshParam->openMsgIndex = NNshParam->bookMsgIndex;
    }

    // �ŏ��ɃI�[�v������t�H�[����ID����������B
    return (NNshParam->lastFrmID);
}

/*=========================================================================*/
/*   Function :   myStopApplication                                        */
/*                                 �X�g�b�v����(�v���O������)����������  */
/*=========================================================================*/
static void MyStopApplication(void)
{
    UInt16                  savedParam;
    NNshSoftwareVersionInfo verInfo;

    // �e���|�����t�@�C���̍폜(DEBUG���[�h���ɂ͎c���Ă���)
    if (NNshParam->debugMessageON == 0)
    {
        (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
        (void) DeleteFile_NNsh(FILE_RECVMSG, NNSH_VFS_DISABLE);
    }

    // NNsi�I�����ɉ����ؒf����
    if (NNshParam->disconnectNNsiEnd != 0)
    {
        // �v���O�����I�����ɂ́A�u����ؒf�v�̊m�F���b�Z�[�W�͏o���Ȃ��B
        savedParam = NNshParam->confirmationDisable;
        NNshParam->confirmationDisable = 1;
        NNshNet_LineHangup();
        NNshParam->confirmationDisable = savedParam;
    }

#ifdef USE_CLIE
    // CLIE�T�|�[�g���J������
    resetCLIESupport_NNsh();
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
        // Silk����̐ݒ�𔽉f
#ifdef USE_CLIE
        NNshParam->notUseSilk = NNshGlobal->notUseSilk;
#else
        NNshParam->notUseSilk = 0;
#endif
        NNshSaveNNsiSetting(NNSH_PREFERTYPE, sizeof(NNshSavedPref), NNshParam);

        // NNsi�I�����Ƀf�[�^�x�[�X��VFS�Ƀo�b�N�A�b�v����
        if ((NNshParam->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            BackupDBtoVFS(NNSH_DISABLE);
        }
    }

    // NNsi�̃O���[�o���̈���J������
    if (NNshGlobal != NULL)
    {
        NNshClearGlobal(NNshGlobal);
        MEMFREE_PTR(NNshGlobal);
        NNshGlobal = NULL;
    }

    // �g�p�̈�̊J��
    MEMFREE_PTR(NNshParam);
    NNshParam = NULL;

    return;
}
