/*============================================================================*
 *  FILE: 
 *     machine.c
 *
 *  Description: 
 *     Palm device specific functions.
 *
 *===========================================================================*/
#define MACHINE_C
#include "local.h"

// prototypes
void CheckVFSdevice_NNsh(void);

/*-------------------------------------------------------------------------*/
/*   Function :   VFSUnmountNotify                                         */
/*                               VFS��Unmount���ꂽ����Notify����          */
/*-------------------------------------------------------------------------*/
Err VFSUnmountNotify(SysNotifyParamType *notifyParamsP)
{
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    NNsiParam->useVFS = 0;

    // VFS��unmount���ꂽ���Ƃ�ʒm
    NNsiGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED; 
     
    EvtWakeup();

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   VFSMountNotify                                           */
/*                               VFS��Mount���ꂽ����Notify����            */
/*-------------------------------------------------------------------------*/
Err VFSMountNotify(SysNotifyParamType *notifyParamsP)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // Remount����
    CheckVFSdevice_NNsh();

    // start.prc�����s&AppSwitch�����̂����
    notifyParamsP->handled |= vfsHandledStartPrc | vfsHandledUIAppSwitch;

    // VFS��mount���ꂽ���Ƃ�ʒm
    NNsiGlobal->notifyUpdate = NNSH_VFS_MOUNTED; 

    // nilEvent���s
    EvtWakeup();

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   CheckVFSdevice_NNsh                                      */
/*                               VFS�̃T�|�[�g�󋵂��p�����[�^�ɔ��f������ */
/*-------------------------------------------------------------------------*/
void CheckVFSdevice_NNsh(void)
{
    Err            err;
    Char           dirName[sizeof(DATAFILE_PREFIX)+ MARGIN];
    UInt32         vfsMgrVersion, volIterator;
    VolumeInfoType volInfo;  
    LocalID        appDbID;
    UInt16         cardNo;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // VFS�̃T�|�[�g�L�����m�F����B
    vfsMgrVersion = 0;
    err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion);
    if (err == errNone)
    {
        // VFS Mount & Unmount Notification�̐ݒ�
        // (�Ƃ肠����VFSMgr�������VFS���h�����ĂȂ��Ă��ォ��mount�ł���悤�ɓo�^���Ă���)
        err = SysCurAppDatabase(&cardNo, &appDbID);
        if (err == errNone)
        {
            // VFS��unmount & remount���ꂽ����notify handler��ݒ�
            // shinhei 03/06/07 - PilotMain����Handle����悤�ɕύX
            err = SysNotifyRegister(cardNo, appDbID, sysNotifyVolumeUnmountedEvent,
                                    NULL, sysNotifyNormalPriority, 0);
            err = SysNotifyRegister(cardNo, appDbID, sysNotifyVolumeMountedEvent,
                                    NULL, sysNotifyNormalPriority, 0);

            // Remount�̎���sysNotifyErrDuplicateEntry�͖���
            if (err == sysNotifyErrDuplicateEntry)
            {
                err = errNone;
            }
        }

        //  VFS���g�p�\���m�F����B(���t�@�����X�}�j���A���Ƃ���ɂ���)
        volIterator = vfsIteratorStart;
        while (volIterator != vfsIteratorStop)
        {
            err = VFSVolumeEnumerate(&(NNsiGlobal->vfsVol), &volIterator);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "VFSVolumeEnumerate()", "", err);
                break;
            }
            err = VFSVolumeInfo(NNsiGlobal->vfsVol, &volInfo);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "VFSVolumeInfo()", "", err);
                break;
            }
            // "VFS�̓R���p�N�g�t���b�V�����g�p����"�ݒ�̏ꍇ�A�A�A
            switch (NNsiParam->vfsUseCompactFlash)
            {
              case NNSH_VFS_USEMEDIA_CF:             
                if (volInfo.mediaType != ExpMediaType_CompactFlash)
                {
                    // VFS��CF�łȂ���΁A������x��������
                    err = ~errNone;
                    continue;
                }
                break;

              case NNSH_VFS_USEMEDIA_MS:             
                if (volInfo.mediaType != ExpMediaType_MemoryStick)
                {
                    // VFS��MS�łȂ���΁A������x��������
                    err = ~errNone;
                    continue;
                }
                break;

              case NNSH_VFS_USEMEDIA_SD:             
                if ((volInfo.mediaType != ExpMediaType_SecureDigital)&&
                    (volInfo.mediaType != ExpMediaType_MultiMediaCard))
                {
                    // VFS��SD/MMC�łȂ���΁A������x��������
                    err = ~errNone;
                    continue;
                }
                break;

              case NNSH_VFS_USEMEDIA_SM:             
                if (volInfo.mediaType != ExpMediaType_SmartMedia)
                {
                    // VFS��SmartMedia�łȂ���΁A������x��������
                    err = ~errNone;
                    continue;
                }
                break;
                
              case NNSH_VFS_USEMEDIA_RD:             
                if (volInfo.mediaType != ExpMediaType_RAMDisk)
                {
                    // VFS��RamDisk�łȂ���΁A������x��������
                    err = ~errNone;
                    continue;
                }
                break;

              case NNSH_VFS_USEMEDIA_DoC:
                // ����FS
                if (volInfo.mediaType != 'Tffs')
                {
                    // VFS���������f�B�A�łȂ���΁A������x��������
                    err = ~errNone;
                    continue;
                }
                break;

              default:
                // �w�肵�Ȃ�
                break;
            }
            NNsh_DebugMessage(ALTID_WARN, "mediaType:", "", volInfo.mediaType);
            NNsh_DebugMessage(ALTID_WARN, "fsType:", "",    volInfo.fsType);

            // �t�@�C���V�X�e���̊m�F
            if (volInfo.fsType == vfsFilesystemType_VFAT)
            {                    
                // VFS ON�ɂ���
                NNsiParam->useVFS = ((NNSH_VFS_ENABLE)|(NNSH_VFS_WORKAROUND));

                // NNsh�p�f�[�^�i�[�p�f�B���N�g���������쐬(VFS ON��)
                CreateDir_NNsh("/Palm/");
                CreateDir_NNsh("/Palm/Programs/");
                StrCopy(dirName, DATAFILE_PREFIX);
                dirName[StrLen(dirName) - 1] = '\0';
                err = CreateDir_NNsh(dirName);
                NNsh_DebugMessage(ALTID_INFO, "Created :", dirName, err);
                err = errNone;
                break;
            }
        }
    }
    if (err != errNone)
    {
        // �f�o�C�X��VFS�T�|�[�g���Ȃ��ꍇ�́ANNsh�ݒ��VFS�g�p�ݒ���N���A
        NNsiParam->useVFS = NNSH_NOTSUPPORT_VFS;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   ResetVFSNotifications                                    */
/*                                          VFS Notification �� unregister */
/*-------------------------------------------------------------------------*/
void ResetVFSNotifications(void)
{
    UInt16 cardNo;
    LocalID appDbID;
  
    // Get application's cardNo and app database localID
    (void) SysCurAppDatabase(&cardNo, &appDbID);
  
    // Unregister unmount & mount notification    
    (void) SysNotifyUnregister(cardNo, appDbID, sysNotifyVolumeUnmountedEvent,
                               sysNotifyNormalPriority);
    (void) SysNotifyUnregister(cardNo, appDbID, sysNotifyVolumeMountedEvent,
                               sysNotifyNormalPriority);

    return;
}

#ifdef USE_PIN_DIA
#ifdef USE_PIN_GARMIN
/*-------------------------------------------------------------------------*/
/*   Function :  DisplayResizeNotify                                       */
/*                                                        �f�B�X�v���C�X�V */
/*-------------------------------------------------------------------------*/
Err DisplayResizeNotify(SysNotifyParamType *notifyP)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // ��ʕ`��̍X�V�́Amain�ōs���̂ŁA�����ł͍X�V�t���O�����Ă�̂�
    NNsiGlobal->notifyUpdate = NNSH_DISP_RESIZED;
    EvtWakeup();

    return (errNone);
}
#endif // #ifdef USE_PIN_GARMIN
#endif // #ifdef USE_PIN_DIA

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :   CLIEdisplayNotify                                        */
/*                                                CLIE�f�B�X�v���C�X�V(��) */
/*-------------------------------------------------------------------------*/
Err CLIEdisplayNotify(SysNotifyParamType *notifyP)
{
    FormType *frm;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // ---------------------------------------------------
    //    ���ӁFnotifyP �́ANULL�ŃR�[�������
    // ---------------------------------------------------
    if (NNsiGlobal->silkRef != 0)
    {
        frm = FrmGetActiveForm();
        HandEraResizeForm(frm);

        //   ��ʕ\���̍X�V�́A�W���̃C�x���g���[�v�ōs���B���̂��߂ɁA
        // �`��w���t���O��ύX���AnilEvent�𔭍s����B
        NNsiGlobal->updateHR = NNSH_UPDATE_DISPLAY;
        EvtWakeup();
    }
    return (errNone);
}
#endif

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :   setCLIESupport_NNsh                                      */
/*                                              CLIE�T�|�[�g�@�\��ݒ肷�� */
/*-------------------------------------------------------------------------*/
void SetCLIESupport_NNsh(void)
{
    SonySysFtrSysInfoP     infoP;
    Boolean                isColor;
    UInt32                 vskVer, width, height, depth;
    UInt16                 vskState;
    Err                    ret;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    infoP                        = NULL;
    NNsiGlobal->jogAssistSetBack = NULL;

    // SONY�f�o�C�X���ǂ����̃`�F�b�N
    if ((FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &depth) != 0)||
        (depth != sonyHwrOEMCompanyID_Sony))
    {
        // SONY�f�o�C�X�łȂ���΁A���������I������
        return;
    }

    // JOG�A�V�X�g�@�\�̐ݒ�
    if (!FtrGet(sonySysFtrCreator, sonySysFtrNumSysInfoP, (UInt32 *)&infoP))
    {
        // JOG�A�V�X�g�@�\�������Ă��邩�ǂ����`�F�b�N����
        if (infoP && (infoP->extn & sonySysFtrSysInfoExtnJogAst))
        {
            // ���݂̃W���O�A�V�X�g�����擾
            if(!FtrGet(sonySysFtrCreator, sonySysFtrNumJogAstMaskP,
                       (UInt32 *) &(NNsiGlobal->jogAssistSetBack)))
            {
                // �W���O�_�C�����̐���ݒ�
                NNsiGlobal->sonyJogMask[0] = sonyJogAstMaskType1; // �t�H�[�����ɌʂɃ}�X�N��ݒ肷��
                NNsiGlobal->sonyJogMask[1] = NUMBER_OF_JOGMASKS;  // �}�X�N����t�H�[����
                NNsiGlobal->sonyJogMask[2] = FRMID_MAIN;
                NNsiGlobal->sonyJogMask[3] = sonyJogAstMaskAll;
                NNsiGlobal->sonyJogMask[4] = FRMID_FILE_SAVELOAD;
                NNsiGlobal->sonyJogMask[5] = sonyJogAstMaskAll;
                NNsiGlobal->sonyJogMask[6] = 0;
                NNsiGlobal->sonyJogMask[7] = 0;    // �|������]�T...

                // ���݂̃W���O�A�V�X�g����ۊǁANNsi�p�̐ݒ�
                NNsiGlobal->jogInfoBack = *(NNsiGlobal->jogAssistSetBack);
                *(NNsiGlobal->jogAssistSetBack) =  NNsiGlobal->sonyJogMask;

                // �W���O�A�V�X�g�I�[�i�n�̐ݒ�͏ȗ�(����񂾂�)
            }
        }
    }

    // SILK�}�l�[�W���̎Q�Ɣԍ����擾����
    ret = SysLibFind(sonySysLibNameSilk, &(NNsiGlobal->silkRef));
    if (ret == sysErrLibNotFound)
    {
        // SILK�}�l�[�W���������[�h�̏ꍇ�A���[�h�ł��邩Try���Ă݂�B
        ret = SysLibLoad('libr',sonySysFileCSilkLib,&(NNsiGlobal->silkRef));
    }
    if (ret != errNone)
    {
        // 281�����Silk�}�l�[�W������R�[�h���}�[�W����
        ret = SysLibLoad(sonySysFileTSilkLib,
                         sonySysFileCSilkLib,
                         &(NNsiGlobal->silkRef));    // OS5
        if (ret != errNone)
        {
            ret = SysLibLoad('libr','SlSi',&(NNsiGlobal->silkRef));
        }
        if (ret != errNone)
        {
            // SILK�}�l�[�W���͎g�p�ł��Ȃ��A�����ɂ���SONY HRLIB���[�h��
            NNsiGlobal->silkRef = 0;
            goto SONY_HRLIB_LOAD;
        }
    }

    // SILK���g�p���Ȃ��ꍇ�ɂ͎��ɐi��
    if (NNsiGlobal->notUseSilk != 0)
    {
        NNsiGlobal->silkRef = 0;
        NNsiGlobal->silkVer = 1;
        goto SONY_HRLIB_LOAD;
    }

    // SILK�}�l�[�W���̃o�[�W�����m�F
    ret = FtrGet(sonySysFtrCreator, sonySysFtrNumVskVersion, &vskVer);
    if (ret != errNone)
    {
        // SILK�}�l�[�W����VERSION1�A���T�C�Y�̂݃T�|�[�g(NR�p)
        if (SilkLibOpen(NNsiGlobal->silkRef) == errNone)
        {
            // SILK�̃��T�C�Y���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
            SilkLibEnableResize(NNsiGlobal->silkRef);
            NNsiGlobal->silkVer = 1;
            goto SONY_HRLIB_LOAD;
        }

        // SILK�o�[�W�����̎擾�Ɏ��s�������ɂ́ASILK���䂵�Ȃ��B
        NNsiGlobal->silkRef = 0;
        NNsiGlobal->silkVer = 1;
        goto SONY_HRLIB_LOAD;
    }
    else
    {
        // �}�W�ł����A�t�B�[�`���[�ԍ���Ver1���G���[�ɂȂ�Ȃ����Ă̂�,,,
        switch (vskVer)
        {
          case vskVersionNum1:
            // SILK�}�l�[�W����VERSION1�A���T�C�Y�̂݃T�|�[�g(NR�p)
            if (SilkLibOpen(NNsiGlobal->silkRef) == errNone)
            {
                // SILK�̃��T�C�Y���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
                SilkLibEnableResize(NNsiGlobal->silkRef);
                NNsiGlobal->silkVer = 1;
            }
            break;

          case vskVersionNum2:
            // SILK�}�l�[�W����VERSION2�A(NX�ȍ~)
            if (VskOpen(NNsiGlobal->silkRef) == errNone)
            {
                // SILK�̃��T�C�Y(�c����)���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
                VskSetState(NNsiGlobal->silkRef, vskStateEnable, vskResizeVertically);
                NNsiGlobal->silkVer = 2;
            }
            break;

          case vskVersionNum3:
          default:
            // SILK�}�l�[�W����VERSION3�A(UX�ȍ~)�A���̑��̃f�o�C�X
            if (VskOpen(NNsiGlobal->silkRef) == errNone)
            {
                // SILK�̃��T�C�Y(������)���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
                ret = VskGetState(NNsiGlobal->silkRef, vskStateResizeDirection, &vskState);
                if (ret != errNone)
                {
                    vskState = vskResizeDisable;
                }
                VskSetState(NNsiGlobal->silkRef, vskStateEnable, vskState);
                NNsiGlobal->silkVer = 3;
            }
            break;
        }
    }

    // sysNotifyDisplayChangeEvent��subscribe����
    ret = SysNotifyRegister(0, SOFT_CREATOR_ID, sysNotifyDisplayChangeEvent,
                            CLIEdisplayNotify, sysNotifyNormalPriority,
                            NULL);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_WARN, "SysNotifyRegister()", "", ret);
    }

SONY_HRLIB_LOAD:
    // SONY HiRes���g�p���Ȃ��ꍇ�ɂ́A�����ŏI������
    if (NNsiParam->disableSonyHR != 0)
    {
        // HRLIB�͎g�p���Ȃ�
        NNsiGlobal->hrRef    = 0;
        NNsiGlobal->notUseHR = NNsiParam->disableSonyHR;
        return;
    }

    // HRLIB�̎Q�Ɣԍ����擾����
    ret = SysLibFind(sonySysLibNameHR, &(NNsiGlobal->hrRef));
    if (ret == sysErrLibNotFound)
    {
        // HRLIB�������[�h�̏ꍇ�A���[�h�ł��邩Try���Ă݂�B
        ret = SysLibLoad('libr', sonySysFileCHRLib, &(NNsiGlobal->hrRef));
    }
    if (ret != errNone)
    {
        // HRLIB�͎g�p�ł��Ȃ�
        NNsiGlobal->hrRef = 0;
        return;
    }
    // HRLIB�̎g�p�錾��HRLIB�̃o�[�W�����ԍ��擾
    HROpen(NNsiGlobal->hrRef);
    HRGetAPIVersion(NNsiGlobal->hrRef, &(NNsiGlobal->hrVer));
    if (NNsiGlobal->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
    {
        // ���o�[�W�����̃n�C���]...��ʃ��[�h���m�F����...
        ret = HRWinScreenMode(NNsiGlobal->hrRef, winScreenModeGet, 
                              &width, &height, &depth, &isColor);
        if (ret != errNone)
        {
            // HRLIB�͎g�p���Ȃ�
            HRClose(NNsiGlobal->hrRef);
            NNsiGlobal->hrRef = 0;
            return;
        }

        NNsiGlobal->prevHRWidth = width;
        if (width != hrWidth)
        {
            // ��ʃ��[�h���n�C���]�łȂ��̂ŁA�n�C���]���[�h�ɐ؂�ւ���
            width  = hrWidth;
            height = hrHeight;
            ret    = HRWinScreenMode(NNsiGlobal->hrRef, winScreenModeSet,
                                     &width, &height, &depth, &isColor);
            if (ret != errNone)
            {
                // HRLIB�͎g�p���Ȃ�
                HRClose(NNsiGlobal->hrRef);
                NNsiGlobal->hrRef = 0;
                return;
            }
        }
    }
    else
    {
        // ��ʃ��[�h�͕ύX���Ȃ��B
        NNsiGlobal->prevHRWidth = hrWidth;
    }
    return;
}
#endif

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :   resetCLIESupport_NNsh                                    */
/*                                                  CLIE�T�|�[�g��������� */
/*-------------------------------------------------------------------------*/
void ResetCLIESupport_NNsh(void)
{
    Boolean                isColor;
    UInt32                 width, height, depth;
    NNshWorkingInfo       *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // JOG�A�V�X�g�@�\�����ɖ߂�
    if (NNsiGlobal->jogAssistSetBack != NULL)
    {
        *(NNsiGlobal->jogAssistSetBack) = NNsiGlobal->jogInfoBack;
    }

    if (NNsiGlobal->silkRef == 0)
    {
        // SONY�p�V���N����̋@�\���g�p���Ȃ��ꍇ�ɂ�HRLIB�̉����
        goto SONY_HRLIB_UNLOAD;
    }

    // sysNotifyDisplayChangeEvent��unsubscribe����
    (void) SysNotifyUnregister(0, SOFT_CREATOR_ID, sysNotifyDisplayChangeEvent,
                               sysNotifyNormalPriority);

    // SILK�}�l�[�W���̉��
    if (NNsiGlobal->silkVer == 1)
    {
        // ��SILK�}�l�[�W��(NR�p)
        SilkLibDisableResize(NNsiGlobal->silkRef);
        SilkLibClose(NNsiGlobal->silkRef);
    }
    else
    {
        // �VSILK�}�l�[�W��(NX�ȍ~)
        VskSetState(NNsiGlobal->silkRef, vskStateEnable, 0);
        VskClose(NNsiGlobal->silkRef);
    }

SONY_HRLIB_UNLOAD:
    if (NNsiGlobal->hrRef != 0)
    {
        // �A�v���N�����ɁA��ʃ��[�h��������猳�ɖ߂�
        if (NNsiGlobal->prevHRWidth != hrWidth)
        {
            HRWinScreenMode(NNsiGlobal->hrRef, winScreenModeGet,
                            &width, &height, &depth, &isColor);
            width      = stdWidth;
            height     = stdHeight;
            (void) HRWinScreenMode(NNsiGlobal->hrRef, winScreenModeSet,
                                   &width, &height, &depth, &isColor);
        }
        // Sony HRLIB�̎g�p�I��
        HRClose(NNsiGlobal->hrRef);
    }
    return;
}
#endif

#ifdef USE_HANDERA
/*-------------------------------------------------------------------------*/
/*   Function :   setHandEraSupport_NNsh                                   */
/*                                               HandEra�f�o�C�X�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
void SetHandEraSupport_NNsh(void)
{
    UInt32              handEraOsVersion;
    VgaScreenModeType   handEraScreenMode;     // �X�N���[�����[�h
    VgaRotateModeType   handEraRotateMode;     // ��]���[�h
    NNshWorkingInfo    *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // HandEra QVGA �T�|�[�g�L�����m�F����B
    handEraOsVersion = 0;
    if (_TRGVGAFeaturePresent(&handEraOsVersion))          
    {
        NNsiGlobal->device = NNSH_DEVICE_HANDERA;
    }

    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
        VgaSetScreenMode(screenMode1To1, handEraRotateMode);
    }
}
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
/*-------------------------------------------------------------------------*/
/*   Function :   SetDIASupport_NNsh                                       */
/*                                    Dynamic Input Area�f�o�C�X�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
void SetDIASupport_NNsh(void)
{
    Err    err;
    UInt32 ver, iFlag;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    err = FtrGet(pinCreator, pinFtrAPIVersion, &ver);
    if (err == errNone)
    {
         // DIA �t�B�[�`���[�m�F
         NNsh_DebugMessage(ALTID_INFO, "DIA version:", "", ver);

#ifdef USE_PIN_GARMIN
        // Garmin DIA�t�B�[�`���[�m�F
        err = FtrGet(garminFtrCreator, garminFtrNumPenInputServices, &ver);
        if (err != ftrErrNoSuchFeature)
        {
            // sysNotifyDisplayResizedEvent��subscribe����
            err = SysNotifyRegister(0, SOFT_CREATOR_ID, 
                                    sysNotifyDisplayResizedEvent,
                                    DisplayResizeNotify, 
                                    sysNotifyNormalPriority,
                                    NULL);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "SysNotifyRegister()", "", err);
                return;
            }
            
            // DIA�T�|�[�g�@(Garmin)�̐ݒ�
            NNsiGlobal->device = NNSH_DEVICE_GARMIN;
            NNsh_DebugMessage(ALTID_INFO, "enable GarminDIA", "", 0);

            return;
        }
#endif // #ifdef USE_PIN_GARMIN

         // DIA�T�|�[�g�f�o�C�X    
         err = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &iFlag);
         NNsh_DebugMessage(ALTID_INFO, "DIA flag:", "", iFlag);
         if ((err != errNone)|
             ((iFlag & grfFtrInputAreaFlagDynamic) == 0)||
             ((iFlag & grfFtrInputAreaFlagCollapsible) == 0))
         {
             // �G���[�ɂȂ�ꍇ�A��������DIA��T�|�[�g�̏ꍇ�A��������������
             return;
         }

         // DIA�T�|�[�g�@�̐ݒ�
         NNsiGlobal->device = NNSH_DEVICE_DIASUPPORT;
         NNsh_DebugMessage(ALTID_INFO, "enable DIA", "", 0);
    }
    return;
}
#endif //  #ifdef USE_PIN_DIA

#ifdef USE_PIN_DIA
/*-------------------------------------------------------------------------*/
/*   Function :   resetDIASupport_NNsh                                     */
/*                                            Dynamic Input Area�̎g�p�J�� */
/*-------------------------------------------------------------------------*/
void ResetDIASupport_NNsh(void)
{
    Err    err;
    UInt32 ver;

    // DIA �t�B�[�`���[�m�F
    err = FtrGet(pinCreator, pinFtrAPIVersion, &ver);
    if (err == errNone)
    {

#ifdef USE_PIN_GARMIN
        // Garmin DIA�t�B�[�`���[�m�F
        err = FtrGet(garminFtrCreator, garminFtrNumPenInputServices, &ver);
        if (err != ftrErrNoSuchFeature)
        {
            // sysNotifyDisplayResizedEvent��unsubscribe����
            (void) SysNotifyUnregister(0, SOFT_CREATOR_ID,
                                        sysNotifyDisplayResizedEvent,
                                        sysNotifyNormalPriority);
            return;
        }
#endif // #ifdef USE_PIN_GARMIN
    }
    return;
}
#endif //  #ifdef USE_PIN_DIA

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraMoveObject                                        */
/*                                                     �I�u�W�F�N�g�̈ړ�  */
/*-------------------------------------------------------------------------*/
void HandEraMoveObject(FormPtr frmP, UInt16 objIndex,
                              Coord x_diff, Coord y_diff)
{
    Boolean move = true;
    Coord   old_x, old_y;

#ifdef USE_GLUE
    if (FrmGlueGetObjectUsable(frmP, objIndex) != true)
    {
        // �B��Ă���I�u�W�F�N�g�͈ړ����Ȃ�
            move = false;
    }
    else
#endif
    {
        FrmHideObject(frmP, objIndex);
    }
    FrmGetObjectPosition(frmP, objIndex, &old_x, &old_y);
    if (old_y < 110)
    {
        // ��̕��ɂ���I�u�W�F�N�g�͈ړ������Ȃ�
        // (�����[�x�^����...������ƃJ�b�R���...)
        // x_diff = 0;
        y_diff = 0;
    }
    FrmSetObjectPosition(frmP, objIndex, old_x + x_diff, old_y + y_diff);

    // �ړ������I�u�W�F�N�g��\������
    if (move == true)
    {
        FrmShowObject(frmP, objIndex);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeObject                                      */
/*                                                 �I�u�W�F�N�g�̃��T�C�Y  */
/*-------------------------------------------------------------------------*/
void HandEraResizeObject(FormPtr frmP, UInt16 objIndex, Coord x_diff, Coord y_diff)
{
    Boolean       disp = true;
    RectangleType r;
    Coord         old_x, old_y;

#ifdef USE_GLUE
    if (FrmGlueGetObjectUsable(frmP, objIndex) != true)
    {
        // �B��Ă���I�u�W�F�N�g
        disp = false;
    }
#endif
    FrmHideObject(frmP, objIndex);

    // �I�u�W�F�N�g�̃��T�C�Y
    FrmGetObjectPosition(frmP, objIndex, &old_x, &old_y);
    if (old_y < 11)
    {
        // ��̕��ɂ���I�u�W�F�N�g�͑���ɂ��Ȃ�
        // (�Ƃ肠����...���l�������...)
        return;
    }

    FrmGetObjectBounds(frmP, objIndex, &r);
    r.extent.x += x_diff;
    r.extent.y += y_diff;
    FrmSetObjectBounds(frmP, objIndex, &r);

    // �O�\�����Ă����ꍇ�ɂ́A�\������
    if (disp != false)
    {
        FrmShowObject(frmP, objIndex);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeForm                                        */
/*                                                     �t�H�[���̃��T�C�Y  */
/*-------------------------------------------------------------------------*/
Boolean HandEraResizeForm(FormPtr frmP)
{
    Coord           x, y, xDiff, yDiff;
    RectangleType   r;
    UInt16          objectIndex, numObjects, rowNum, nofRow, height;
    Int16           fldIndex, sclIndex;
    TableType      *tableP;
    ControlType    *ctlPtr;
    FormObjectKind  objType;
    WinHandle       winH;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    winH = FrmGetWindowHandle(frmP);
    if (WinModal(winH) == true)
    {
        /** ���[�_���t�H�[����������A�E�B���h�E�̃��T�C�Y�͎��{���Ȃ� **/
        return (false);
    }

    /** ������ **/
    fldIndex = -1;
    sclIndex = -1;

#ifdef USE_PIN_DIA
    if (NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)
    {
        // DIA�̏ꍇ�A�A�A

        /* �V�T�C�Y */
        WinGetDisplayExtent(&x, &y);

        /* ���T�C�Y */
        WinGetBounds(FrmGetWindowHandle(frmP), &r);
    }
    else
#endif
    {
        /* �V�T�C�Y */
        WinGetDisplayExtent(&x, &y);
    
        /* ���T�C�Y */
        FrmGetFormBounds(frmP, &r);

        /* ���[�_���t�H�[���łȂ��ꍇ�A�Ȃ����ȉ��̔��������K�v�i�����s���j */
        r.topLeft.x = r.topLeft.x + 1;
        r.topLeft.y = r.topLeft.y + 1;
        r.extent.x  = r.extent.x  - 2;
        r.extent.y  = r.extent.y  - 2;
    }

    xDiff = x - (r.topLeft.x + r.extent.x);
    yDiff = y - (r.topLeft.y + r.extent.y);

    if ((xDiff == 0)&&(yDiff == 0))
    {
        return (false);
    }

    /* ���T�C�Y */
    r.extent.x = r.extent.x + xDiff;
    r.extent.y = r.extent.y + yDiff;
    WinSetWindowBounds(winH, &r);
        
    /* �I�u�W�F�N�g�̈ړ��A���T�C�Y */
    numObjects = FrmGetNumberOfObjects(frmP);
    for (objectIndex = 0; objectIndex < numObjects; objectIndex++)
    {
        objType = FrmGetObjectType(frmP, objectIndex);
        switch (objType)
        {
          case frmFieldObj:
            // �t�B�[���h�̈ړ��i�X���ԓ���Field�j�A���T�C�Y�i���X�\��Field�j
            if (FldGetVisibleLines((FieldType *) 
                                     FrmGetObjectPtr(frmP, objectIndex)) == 1)
            {
                // �t�B�[���h�̈ړ�
                HandEraMoveObject(frmP, objectIndex, 0, yDiff);
            }
            else
            {
                // �t�B�[���h�̃��T�C�Y
                HandEraResizeObject(frmP, objectIndex, xDiff, yDiff);
                fldIndex = objectIndex;
            }
            break;
          case frmControlObj:
            // �{�^���֘A�̈ړ�
            ctlPtr = (ControlType *)FrmGetObjectPtr(frmP, objectIndex);
#ifdef USE_GLUE
            if (CtlGlueGetControlStyle(ctlPtr) == repeatingButtonCtl)
            {
                // ���s�[�g�{�^���̂�x�����ɂ��ړ�������
                HandEraMoveObject(frmP, objectIndex, xDiff, yDiff);
            }
            else
#endif
            {
                HandEraMoveObject(frmP, objectIndex, 0, yDiff);
            }
            break;

          case frmTableObj:
            HandEraResizeObject(frmP, objectIndex, xDiff, yDiff);
            tableP = FrmGetObjectPtr(frmP, objectIndex);
            nofRow = TblGetNumberOfRows(tableP);
            TblGetBounds(tableP, &r);
            height = (r.extent.y - r.topLeft.y) / nofRow + 1;
            for (rowNum = 0; rowNum < nofRow; rowNum++)
            {
                TblSetRowHeight(tableP, rowNum, height);
            }
            break;

          case frmGadgetObj:
            // �K�W�F�b�g�̃��T�C�Y
            HandEraResizeObject(frmP, objectIndex, xDiff, yDiff);
            break;

          case frmLabelObj:
          case frmGraffitiStateObj:
            // GSI�̈ړ�
            HandEraMoveObject(frmP, objectIndex, 0, yDiff);
            break;

          case frmScrollBarObj:
            // �X�N���[���o�[�̃��T�C�Y�A�ړ�
            HandEraResizeObject(frmP, objectIndex, 0, yDiff);
            HandEraMoveObject(frmP, objectIndex, xDiff, 0);
            sclIndex = objectIndex;
            break;

          case frmListObj:
            // ���X�g�I�u�W�F�N�g�͉������Ȃ��B

          default:
            // ???(�������Ȃ�)
            break;
        }
    }
    if ((fldIndex >= 0) && (sclIndex >= 0))
    {
        // �X�N���[���o�[�̃A�b�v�f�[�g
        NNshWinViewUpdateScrollBar(
              FrmGetObjectId(frmP, fldIndex), FrmGetObjectId(frmP, sclIndex)); 
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraMoveDialog                                        */
/*                                                   �_�C�����O�̈ړ�      */
/*-------------------------------------------------------------------------*/
void HandEraMoveDialog(FormPtr frmP)
{
    Coord         x, y;
    RectangleType r;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        /* �S�̂̃T�C�Y */
        WinGetDisplayExtent(&x, &y);
    
        /* dialog�̃T�C�Y */
        FrmGetFormBounds(frmP, &r);

        /* �Ȃ����ȉ��̔��������K�v�i�����s���j */
        r.topLeft.x += 2; 
        r.topLeft.y += 2; 
        r.extent.x -= 4;
        r.extent.y -= 4;
        goto MOVE_OBJECT;
    }
#endif
#ifdef USE_CLIE
    // SILK�}�l�[�W�����n�o�d�m����Ă��邩�H
    if (NNsiGlobal->silkRef != 0)
    {
        /* �S�̂̃T�C�Y */
        WinGetDisplayExtent(&x, &y);
    
        /* dialog�̃T�C�Y */
        FrmGetFormBounds(frmP, &r);

        /* �Ȃ����ȉ��̔��������K�v�i�����s���j */
        // r.topLeft.x -=2;
        // r.topLeft.y -=2;
        r.extent.x  -= 4;
        r.extent.y  -= 4;
        goto MOVE_OBJECT;
    }
#endif
    return;

MOVE_OBJECT:
    /* dialog�̈ړ� */
    if (y - (r.topLeft.y + r.extent.y) < 0)
    {
        r.topLeft.y = y - r.extent.y;
    }
    WinSetWindowBounds(FrmGetWindowHandle(frmP), &r);

    NNsi_FrmDrawForm(frmP, true);

    return;
}

/*=========================================================================*/
/*   Function :   NNshRestoreNNsiSetting                                   */
/*                                                     NNsi�ݒ�̓ǂݍ���  */
/*=========================================================================*/
void RestoreSetting_NNsh(Int16 version, UInt16 size, NNshSavedPref *prm)
{
    Err           ret;
    UInt32        offset;
    UInt16        cnt, nofRec;
    DmOpenRef     dbRef;
    Char         *ptr;

    // NNsi�ݒ�̗̈������������
    nofRec = 0;

    if ((version == SOFT_PREFVERSION)&&(size == sizeof(NNshSavedPref)))
    {
        //  �ݒ肳�ꂽ�o�[�W�����ƍ���N�����ꂽ�o�[�W��������э\���̃T�C�Y��
        // �����������ꍇ�̂݁ANNsi�ݒ�𕜋�������B

        // �ϐ��������� 
        offset = 0;
        ptr    = (Char *) prm;

        // �c�a���ANNsi�ݒ��ǂݏo��(���R�[�h�̓o�^���̌��ˍ����ŁA�A�A����)
        OpenDatabase_NNsh(DBNAME_SETTING, DBVERSION_SETTING, &dbRef);
        GetDBCount_NNsh(dbRef, &nofRec);
        cnt = nofRec;
        while ((cnt != 0)&&(offset < sizeof(NNshSavedPref)))
        {
            ret = GetRecord_NNsh(dbRef,(cnt - 1),DBSIZE_SETTING, &ptr[offset]);
            if (ret != errNone)
            {
                // �f�[�^�ǂݏo���Ɏ��s(���̏ꍇ�ANNsi�ݒ�͏���������)
                NNsh_WarningMessage(ALTID_WARN, "Preference read failure", "", ret);
                nofRec = 0;
                break;
            }
            cnt--;
            offset = offset + DBSIZE_SETTING;
        }
        CloseDatabase_NNsh(dbRef);
    }
    if (nofRec == 0)
    {
        // NNsi�ݒ������������
        prm->bufferSize       = NNSH_WORKBUF_DEFAULT;
        prm->currentFont      = NNSH_DEFAULT_FONT;
        prm->autoRestore      = 1;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshSaveNNsiSetting                                      */
/*                                                         NNsi�ݒ�̋L��  */
/*-------------------------------------------------------------------------*/
void SaveSetting_NNsh(Int16 version, UInt16 size, NNshSavedPref *param)
{
    Err       ret;
    UInt32    offset;
    UInt16    nofRec;
    DmOpenRef dbRef;
    Char      *ptr;

    OpenDatabase_NNsh(DBNAME_SETTING, DBVERSION_SETTING, &dbRef);
    GetDBCount_NNsh(dbRef, &nofRec);
    while (nofRec != 0)
    {
        //  ���ł�NNsi�ݒ肪�L�^����Ă����ꍇ�A�S���R�[�h���폜����
        (void) DeleteRecordIDX_NNsh(dbRef, (nofRec - 1));
        nofRec--;
    }

    // NNsi�ݒ��DB�ɓo�^����(���R�[�h�o�^���̌��ˍ����ɒ��ӁI)
    offset = 0;
    ptr    = (Char *) param;
    while (offset < sizeof(NNshSavedPref))
    {
        ret = EntryRecord_NNsh(dbRef, DBSIZE_SETTING, &ptr[offset]);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_WARN, "EntryRecord_NNsh()", "", ret);
            NNsh_DebugMessage(ALTID_WARN, "    ", "offset:", offset);
            NNsh_DebugMessage(ALTID_WARN, "    ", "size:", DBSIZE_SETTING);
            break;
        }
        offset = offset + DBSIZE_SETTING;
    }
    CloseDatabase_NNsh(dbRef);

    return;
}

/*=========================================================================*/
/*   Function :   NNshSilkMinimize                                         */
/*                                          �O���t�B�e�B�G���A���ŏ������� */
/*=========================================================================*/
Err NNshSilkMinimize(FormType *frm)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
#ifdef USE_PIN_DIA
    if ((NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNsiGlobal->device == NNSH_DEVICE_GARMIN))
    {
        if (PINSetInputAreaState(pinInputAreaClosed) != errNone)
        {
            HandEraResizeForm(frm);
        }
        return (errNone);
    }
#endif
#ifdef USE_CLIE
    // SILK�}�l�[�W�����n�o�d�m����Ă��邩�H
    if (NNsiGlobal->silkRef != 0)
    {
        // SILK�̍ŏ������s��
        if (NNsiGlobal->silkVer == 1)
        {
            // Version1(NR�p)
            SilkLibResizeDispWin(NNsiGlobal->silkRef, silkResizeToStatus);
        }
        else
        {
            // Version2(NX�ȍ~)
            VskSetState(NNsiGlobal->silkRef, vskStateResize, vskResizeMin);
        }
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
    return (~errNone);
}

/*=========================================================================*/
/*   Function :   NNshSilkMaximize                                         */
/*                                          �O���t�B�e�B�G���A���ő剻���� */
/*=========================================================================*/
Err NNshSilkMaximize(FormType *frm)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMaximizeWindow();
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
#ifdef USE_PIN_DIA
    if ((NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNsiGlobal->device == NNSH_DEVICE_GARMIN))
    {
        if (PINSetInputAreaState(pinInputAreaOpen) == errNone)
        {
            HandEraResizeForm(frm);
        }
        return (errNone);
    }
#endif
#ifdef USE_CLIE
    // SILK�}�l�[�W�����n�o�d�m����Ă��邩�H
    if (NNsiGlobal->silkRef != 0)
    {
        // SILK�̍ő剻���s��
        if (NNsiGlobal->silkVer == 1)
        {
            // Version1(NR�p)
            SilkLibResizeDispWin(NNsiGlobal->silkRef, silkResizeNormal);
        }
        else
        {
            // Version2(NX�ȍ~)
            VskSetState(NNsiGlobal->silkRef, vskStateResize, vskResizeMax);
        }
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
    return (~errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkInstalledWebBrowser                                 */
/*                                        WebBrowser�̃C���X�g�[���`�F�b�N */
/*-------------------------------------------------------------------------*/
void CheckInstalledWebBrowser(void)
{
    UInt16            cardNo;
    LocalID           dbID;
    DmSearchStateType searchState;
    Err               ret;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // Xiino3���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'Xii3', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino3���C���X�g�[������Ă���
        NNsiGlobal->browserCreator    = 'Xii3';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }


    // NetFront���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NF3T', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // NetFront���C���X�g�[������Ă���
        NNsiGlobal->browserCreator    = 'NF3T';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // WebBrowser 2.0���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NF3P', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // WebBrowser 2.0���C���X�g�[������Ă���
        NNsiGlobal->browserCreator    = 'NF3P';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Xiino(PalmScape)���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'PScp', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino���C���X�g�[������Ă���
        NNsiGlobal->browserCreator    = 'PScp';
        NNsiGlobal->browserLaunchCode = 0xfa21;
        return;
    }

    // Xiino(LEM)���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'dXin', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino(LEM)���C���X�g�[������Ă���
        NNsiGlobal->browserCreator    = 'dXin';
        NNsiGlobal->browserLaunchCode = 0xfa21;
        return;
    }

    // Blazer���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'BLZ1', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Blazer���C���X�g�[������Ă���
        NNsiGlobal->browserCreator    = 'BLZ1';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Eudora Web���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'QCwb', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Eudora Web���C���X�g�[������Ă���
        NNsiGlobal->browserCreator    = 'QCwb';
        NNsiGlobal->browserLaunchCode = 0xfa0;
        // NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

#if 0
    // PocketLink���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'PLNK', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // PocketLink���C���X�g�[������Ă���(but does not work...)
        NNsiGlobal->browserCreator    = 'PLNK';
        // NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdOpenDB;
        return;
    }
#endif

    // Novarra Web Pro browser���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NOVR', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Novarra Web Pro browser���C���X�g�[������Ă���
        NNsiGlobal->browserCreator    = 'NOVR';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    NNsh_DebugMessage(ALTID_INFO, MSG_CANNOTFIND_BROWSER, "", ret);
    return;
}

/*=========================================================================*/
/*   Function : KeyConvertFiveWayToJogChara                                */
/*                                  5Way�̓��̓R�[�h���W���O�̃R�[�h�ɕϊ� */
/*=========================================================================*/
UInt16 KeyConvertFiveWayToJogChara(EventType *event)
{
    NNshSavedPref *NNsiParam;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // 5way�̓��͂łȂ���΁A�L�[�R�[�h�����̂܂܉�������
    if (event->data.keyDown.chr != vchrNavChange)
    {
        // �n�[�h�L�[����̊m�F�B�B�B
        if ((NNsiParam->useHardKeyControl != 0)&&
            (event->data.keyDown.chr >= vchrHardKeyMin))
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

              case vchrCapture:
              case vchrVoiceRec:
              case vchrGarminRecord:
              case chrLeftArrow:         // �J�[�\���L�[��(5way��)
              case vchrJogLeft:
              case chrRightArrow:     // �J�[�\���L�[�E(5way�E)
              case vchrJogRight:
              default:
                return (event->data.keyDown.chr);
                break;
            }
        }
        return (event->data.keyDown.chr);
    }

    if (((event->data.keyDown.keyCode) & (navBitSelect | navChangeSelect))
         == navChangeSelect)
    {
        // �W���O�����Ɠ����ɂ���
        return (vchrJogRelease);
    }
    if ((event->data.keyDown.keyCode) & navBitLeft)
    {
        // �J�[�\���L�[���Ɠ����ɂ���
        return (chrLeftArrow);
    }
    if ((event->data.keyDown.keyCode) & navBitRight)
    {
        // �J�[�\���L�[�E�Ɠ����ɂ���
        return (chrRightArrow);
    }

    // ��L�ȊO�͂��̂܂܉�������
    return (event->data.keyDown.chr);
}

#ifdef USE_HIGHDENSITY
/*-------------------------------------------------------------------------*/
/*   Function :   SetDoubleDensitySupport_NNsh                             */
/*                                              ���𑜓x�T�|�[�g�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
void SetDoubleDensitySupport_NNsh(void)
{
    UInt32  version;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // ���𑜓x�T�|�[�g�ݒ��������
    NNsiGlobal->os5HighDensity = (UInt32) kDensityLow;

#ifdef USE_CLIE
    if (NNsiGlobal->hrRef != 0)
    {
        // CLIE��������A���𑜓x�̃`�F�b�N�����Ȃ�(CLIE�n�C���]���g��)
        return;
    }
#endif

    // ���𑜓x�t�B�[�`���[�̎擾
    if (FtrGet(sysFtrCreator, sysFtrNumWinVersion, &version) != 0)
    {
        // ���𑜓x�t�B�[�`���[�̃T�|�[�g�Ȃ�
        return;
    }
    NNsh_DebugMessage(ALTID_INFO, "Window Version", ": ", version);
    if (version < 4)
    {
        //�@���𑜓x�̃T�|�[�g���Ȃ��A�I������
        //  (����Ő������񂾂낤��...�}�j���A���ɂ͂��������Ă�������...)
        return;
    }

    // Window�̉𑜓x���擾����
    WinScreenGetAttribute(winScreenDensity, &(NNsiGlobal->os5HighDensity));

    // �Ƃ肠�����A�W���𑜓x�ɂ���
    WinSetCoordinateSystem(kCoordinatesStandard);

    return;
}
#endif   // #ifdef USE_HIGHDENSITY
#ifdef USE_PIN_DIA
/*=========================================================================*/
/*   Function : VgaFormModify_DIA                                          */
/*                                                     DIA�p�̃t�H�[������ */
/*=========================================================================*/
Err VgaFormModify_DIA(FormType *frmP, VgaFormModifyType_DIA type)
{

    switch (type)
    {
      case vgaFormModifyNormalToWide:
        // DIA�T�|�[�g�̐ݒ�
        FrmSetDIAPolicyAttr(frmP, frmDIAPolicyCustom);

        // �V���N�����L����
        PINSetInputTriggerState(pinInputTriggerEnabled);
//        PINSetInputAreaState(pinInputAreaUser);

        // ���炢�x�^�A�A�A�E�B���h�E�T�C�Y�̍ő�l�ŏ��l��ݒ�
        WinSetConstraintsSize(FrmGetWindowHandle(frmP), 160, 225, 225, 160, 225, 225);
        break;

      default:
        // �������Ȃ�
        break;
    }
    return (errNone);
}
#endif

#ifdef USE_PIN_DIA
/*=========================================================================*/
/*   Function : VgaFormRotate_DIA                                          */
/*                                               DIA�p�̃t�H�[������(��]) */
/*=========================================================================*/
void VgaFormRotate_DIA(void)
{
#ifdef USE_SYSEXIST_GLUE
    // �֐������邩�ǂ����̃`�F�b�N������
    if ((SysGlueTrapExists(pinSysGetOrientation))&&
        (SysGlueTrapExists(pinSysSetOrientation)))
    {
#endif  // #ifdef USE_SYSEXIST_GLUE

        switch (SysGetOrientation())
        {
          case sysOrientationPortrait:
            SysSetOrientation(sysOrientationLandscape);
            break;

          case sysOrientationLandscape:
            SysSetOrientation(sysOrientationPortrait);
            break;

          case sysOrientationUser:
          default:
            // ���肦�Ȃ��͂������A�ꉞ
            SysSetOrientation(sysOrientationPortrait);
            break;
        }
#ifdef USE_SYSEXIST_GLUE
    }
#endif   // #ifdef USE_SYSEXIST_GLUE
    return;
}
#endif