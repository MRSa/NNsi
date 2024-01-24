/*============================================================================*
 *  FILE: 
 *     machine.c
 *
 *  Description: 
 *     Palm device specific functions.
 *
 *===========================================================================*/
#define MACHINE_C

extern Err setOfflineLogDir(Char *fileName);

static Err VFSUnmountNotify(SysNotifyParamType *notifyParamsP);
static Err VFSMountNotify  (SysNotifyParamType *notifyParamsP);

/*-------------------------------------------------------------------------*/
/*   Function :   checkVFSdevice_NNsh                                      */
/*                               VFS�̃T�|�[�g�󋵂��p�����[�^�ɔ��f������ */
/*-------------------------------------------------------------------------*/
static void checkVFSdevice_NNsh(void)
{
    Err            err;
    Char           dirName[sizeof(DATAFILE_PREFIX)+ MARGIN];
    UInt32         vfsMgrVersion, volIterator;
    VolumeInfoType volInfo;  
    LocalID        appDbID;
    UInt16         cardNo;

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
            err = VFSVolumeEnumerate(&(NNshGlobal->vfsVol), &volIterator);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "VFSVolumeEnumerate()", "", err);
                break;
            }
            err = VFSVolumeInfo(NNshGlobal->vfsVol, &volInfo);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "VFSVolumeInfo()", "", err);
                break;
            }
            // "VFS�̓R���p�N�g�t���b�V�����g�p����"�ݒ�̏ꍇ�A�A�A
            switch ((NNshGlobal->NNsiParam)->vfsUseCompactFlash)
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
                if ((volInfo.mediaType != 'Tffs')&&
                    (volInfo.mediaType != 'Isdr'))
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

            if (volInfo.fsType == vfsFilesystemType_VFAT)
            {                    
                // "VFS�g�p�\���Ɏ����I��VFS ON" �ݒ���m�F����
                if ((NNshGlobal->NNsiParam)->vfsOnAutomatic != 0)
                {
                    // VFS ON�ɂ���
                    (NNshGlobal->NNsiParam)->useVFS =
                                         (NNshGlobal->NNsiParam)->vfsOnDefault;
#if 0
                    // VFS ON�ɂ���
                    (NNshGlobal->NNsiParam)->useVFS = ((NNSH_VFS_ENABLE)|
                                         (NNSH_VFS_WORKAROUND)|
                                         (NNSH_VFS_USEOFFLINE)|
                                         (NNSH_VFS_DIROFFLINE)|
                                         (NNSH_VFS_DBBACKUP));
#endif
                                         
                    if ((NNshGlobal->NNsiParam)->vfsOnNotDBCheck == 0)
                    {
                        //  "����VFS ON�ł�DB�`�F�b�N�Ȃ�" �� OFF�̂Ƃ�
                        //  (�N����DB�`�F�b�N���n�m�ɂ���)
                        (NNshGlobal->NNsiParam)->useVFS = 
                                      ((NNshGlobal->NNsiParam)->useVFS|(NNSH_VFS_DBIMPORT));
                    }
                }
                if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
                {
                    // NNsh�p�f�[�^�i�[�p�f�B���N�g���������쐬(VFS ON��)
                    StrCopy(dirName, DATAFILE_PREFIX);
                    dirName[StrLen(dirName) - 1] = '\0';
                    err = CreateDir_NNsh(dirName);
                    NNsh_DebugMessage(ALTID_INFO, "Created :", dirName, err);
                    err = errNone;
                }
                if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
                {
                    // NNsh�pOFFLINE�f�[�^�i�[�f�B���N�g�����쐬(VFS ON��)
                    StrCopy(dirName, LOGDATAFILE_PREFIX);
                    dirName[StrLen(dirName) - 1] = '\0';
                    err = CreateDir_NNsh(dirName);
                    NNsh_DebugMessage(ALTID_INFO,"Created :",dirName,err);

                    // logdir.txt��ǂ�OFFLINE���O�x�[�X�f�B���N�g�����擾
                    (void) setOfflineLogDir(LOGDIR_FILE);
                    err = errNone;
                }
                break;
            }
        }
    }
    if (err != errNone)
    {
        // �f�o�C�X��VFS�T�|�[�g���Ȃ��ꍇ�́ANNsh�ݒ��VFS�g�p�ݒ���N���A
        (NNshGlobal->NNsiParam)->useVFS = NNSH_NOTSUPPORT_VFS;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   VFSUnmountNotify                                         */
/*                                       VFS��Unmount���ꂽ����Notify����  */
/*-------------------------------------------------------------------------*/
static Err VFSUnmountNotify(SysNotifyParamType *notifyParamsP)
{
    UInt16 unmountedVol;

    unmountedVol = ((UInt16) (notifyParamsP->notifyDetailsP));

    // �A���}�E���g���ꂽ�{�����[���͉����`�F�b�N����
    if (NNshGlobal->vfsVol != unmountedVol)
    {
        // ����NNsi���g�p���̃{�����[���ł͂Ȃ������A�I������
        return (errNone);
    }

    // VFS�t���O���N���A����
    (NNshGlobal->NNsiParam)->useVFS = 0;

    // Unmount�������ɊJ���Ă�����ʂɂ���āA�t���O��ݒ肳����
    switch(FrmGetFormId(FrmGetActiveForm()))
    {
      case FRMID_WRITE_MESSAGE:
        // �������݉��
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED_WRITE;  
        break;
        
      case FRMID_MESSAGE:
        // �Q�Ɖ��
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED_MESSAGE;  
        break;
        
      case FRMID_THREAD:
        // �ꗗ���
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED; 
        break; 

      case FRMID_INPUTDIALOG:
      case FRMID_DBIMPORT:
      case FRMID_MESSAGEINFO:
      case FRMID_HARDKEY_SETTING:
      case FRMID_OMITDIALOG_DETAIL:
      case FRMID_FAVORSET_THREADSTATE:
      case FRMID_FAVORSET_THREADTITLE:
        // FrmDoDialog���Ă���ꍇ�͈�U�O��Form�ɖ߂�
        // �����ADoDialog�̕Ԃ�l���ǂ����邩����...�����vchrLaunch�����ŗ������Ɠ����Ȃ̂ŋC�ɂ��Ȃ��Ă�������...
        FrmReturnToForm(0);
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED_OTHER;  
        break;
      
      default:
        // ���̑��̉��
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED_OTHER;  
        break;
    }

    // nilEvent���s
    // shinehi 03/06/07 <- FrmDoDialog()���Ă���Œ����ƑO��Form�ɖ߂�܂ŕߑ����Ă��炦�Ȃ�...
    EvtWakeup();
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   VFSMountNotify                                           */
/*                               VFS��Mount���ꂽ����Notify����            */
/*-------------------------------------------------------------------------*/
static Err VFSMountNotify(SysNotifyParamType *notifyParamsP)
{
    UInt16 frmID;

    // Remount����
    checkVFSdevice_NNsh();

    // start.prc�����s&AppSwitch�����̂����
    notifyParamsP->handled |= vfsHandledStartPrc | vfsHandledUIAppSwitch;

    // VFS��mount���ꂽ���Ƃ�ʒm
    NNshGlobal->notifyUpdate = NNSH_VFS_MOUNTED; 

    // Dialog��������O�̃t�H�[���ɖ߂��Ă���
    frmID = FrmGetFormId(FrmGetActiveForm());
    switch(frmID)
    {
      case FRMID_INPUTDIALOG:
      case FRMID_DBIMPORT:
      case FRMID_MESSAGEINFO:
      case FRMID_HARDKEY_SETTING:
      case FRMID_OMITDIALOG_DETAIL:
      case FRMID_FAVORSET_THREADSTATE:
      case FRMID_FAVORSET_THREADTITLE:
        // FrmDoDialog���Ă���ꍇ�͈�U�O��Form�ɖ߂�
        // �����ADoDialog�̕Ԃ�l���ǂ����邩����...�����vchrLaunch�����ŗ������Ɠ����Ȃ̂ŋC�ɂ��Ȃ��Ă�������...
        FrmReturnToForm(0);
        break;
        
    default:
        // do nothing
        break;
    }

    // nilEvent���s
    EvtWakeup();

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   resetVFSNotifications                                    */
/*                                          VFS Notification �� unregister */
/*-------------------------------------------------------------------------*/
static void resetVFSNotifications(void)
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
}

#ifdef USE_PIN_DIA
#ifdef USE_PIN_GARMIN
/*-------------------------------------------------------------------------*/
/*   Function :  DisplayResizeNotify                                       */
/*                                                        �f�B�X�v���C�X�V */
/*-------------------------------------------------------------------------*/
Err DisplayResizeNotify(SysNotifyParamType *notifyP)
{
    // ��ʕ`��̍X�V�́Amain�ōs���̂ŁA�����ł͍X�V�t���O�����Ă�̂�
    NNshGlobal->notifyUpdate = NNSH_DISP_RESIZED;
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

    // ---------------------------------------------------
    //    ���ӁFnotifyP �́ANULL�ŃR�[�������
    // ---------------------------------------------------
    if (NNshGlobal->silkRef != 0)
    {
        frm = FrmGetActiveForm();
        HandEraResizeForm(frm);

        //   ��ʕ\���̍X�V�́A�W���̃C�x���g���[�v�ōs���B���̂��߂ɁA
        // �`��w���t���O��ύX���AnilEvent�𔭍s����B
        NNshGlobal->updateHR = NNSH_UPDATE_DISPLAY;
        UpdateFieldRegion();
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
static void setCLIESupport_NNsh(void)
{
    SonySysFtrSysInfoP     infoP;
    Boolean                isColor;
    UInt32                 vskVer, width, height, depth;
    UInt16                 vskState;
    Err                    ret;

    infoP                        = NULL;
    NNshGlobal->jogAssistSetBack = NULL;

    // SONY�f�o�C�X���ǂ����̃`�F�b�N
    if ((FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &depth) != 0)||
        (depth != sonyHwrOEMCompanyID_Sony))
    {
        // SONY�f�o�C�X�łȂ���΁A���������I������
        return;
    }

    // JOG�A�V�X�g�̐ݒ����ݒ肷��
    NNshGlobal->sonyJogMask[0]  = sonyJogAstMaskType1;
    NNshGlobal->sonyJogMask[1]  = NUMBER_OF_JOGMASKS;
    NNshGlobal->sonyJogMask[2]  = FRMID_MESSAGE;
    NNshGlobal->sonyJogMask[3]  = sonyJogAstMaskAll;
    NNshGlobal->sonyJogMask[4]  = FRMID_MANAGEBBS;
    NNshGlobal->sonyJogMask[5]  = sonyJogAstMaskAll;
    NNshGlobal->sonyJogMask[6]  = FRMID_THREAD;
    NNshGlobal->sonyJogMask[7]  = sonyJogAstMaskAll;
    NNshGlobal->sonyJogMask[8]  = FRMID_LISTGETLOG;
    NNshGlobal->sonyJogMask[9]  = sonyJogAstMaskAll;
    NNshGlobal->sonyJogMask[10] = 0;
    NNshGlobal->sonyJogMask[11] = 0;

    // JOG�A�V�X�g�@�\�̐ݒ�
    if (!FtrGet(sonySysFtrCreator, sonySysFtrNumSysInfoP, (UInt32 *)&infoP))
    {
        // JOG�A�V�X�g�@�\�������Ă��邩�ǂ����`�F�b�N����
        if (infoP && (infoP->extn & sonySysFtrSysInfoExtnJogAst))
        {
            // ���݂̃W���O�A�V�X�g�����擾
            if(!FtrGet(sonySysFtrCreator, sonySysFtrNumJogAstMaskP,
                       (UInt32 *) &(NNshGlobal->jogAssistSetBack)))
            {
                // ���݂̃W���O�A�V�X�g����ۊǁANNsi�p�̐ݒ�
                 NNshGlobal->jogInfoBack =
                                            *(NNshGlobal->jogAssistSetBack);
                *(NNshGlobal->jogAssistSetBack) =  NNshGlobal->sonyJogMask;

                // �W���O�A�V�X�g�I�[�i�n�̐ݒ�͏ȗ�(����񂾂�)
            }
        }
    }

    // SILK�}�l�[�W���̎Q�Ɣԍ����擾����
    ret = SysLibFind(sonySysLibNameSilk, &(NNshGlobal->silkRef));
    if (ret == sysErrLibNotFound)
    {
        // SILK�}�l�[�W���������[�h�̏ꍇ�A���[�h�ł��邩Try���Ă݂�B
        ret = SysLibLoad('libr',sonySysFileCSilkLib,&(NNshGlobal->silkRef));
    }
    if (ret != errNone)
    {
        // 281�����Silk�}�l�[�W������R�[�h���}�[�W����
        ret = SysLibLoad(sonySysFileTSilkLib,
                         sonySysFileCSilkLib,
                         &(NNshGlobal->silkRef));    // OS5
        if (ret != errNone)
        {
            ret = SysLibLoad('libr','SlSi',&(NNshGlobal->silkRef));
        }
        if (ret != errNone)
        {
            // SILK�}�l�[�W���͎g�p�ł��Ȃ��A�����ɂ���SONY HRLIB���[�h��
            NNshGlobal->silkRef = 0;
            goto SONY_HRLIB_LOAD;
        }
    }

    // SILK���g�p���Ȃ��ꍇ�ɂ͎��ɐi��
    if (NNshGlobal->notUseSilk != 0)
    {
        NNshGlobal->silkRef = 0;
        NNshGlobal->silkVer = 1;
        goto SONY_HRLIB_LOAD;
    }

    // SILK�}�l�[�W���̃o�[�W�����m�F
    ret = FtrGet(sonySysFtrCreator, sonySysFtrNumVskVersion, &vskVer);
    if (ret != errNone)
    {
        // SILK�}�l�[�W����VERSION1�A���T�C�Y�̂݃T�|�[�g(NR�p)
        if (SilkLibOpen(NNshGlobal->silkRef) == errNone)
        {
            // SILK�̃��T�C�Y���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
            SilkLibEnableResize(NNshGlobal->silkRef);
            NNshGlobal->silkVer = 1;
            goto SONY_HRLIB_LOAD;
        }

        // SILK�o�[�W�����̎擾�Ɏ��s�������ɂ́ASILK���䂵�Ȃ��B
        NNshGlobal->silkRef = 0;
        NNshGlobal->silkVer = 1;
        goto SONY_HRLIB_LOAD;
    }
    else
    {
        // �}�W�ł����A�t�B�[�`���[�ԍ���Ver1���G���[�ɂȂ�Ȃ����Ă̂�,,,
        switch (vskVer)
        {
          case vskVersionNum1:
            // SILK�}�l�[�W����VERSION1�A���T�C�Y�̂݃T�|�[�g(NR�p)
            if (SilkLibOpen(NNshGlobal->silkRef) == errNone)
            {
                // SILK�̃��T�C�Y���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
                SilkLibEnableResize(NNshGlobal->silkRef);
                NNshGlobal->silkVer = 1;
            }
            break;

          case vskVersionNum2:
            // SILK�}�l�[�W����VERSION2�A(NX�ȍ~)
            if (VskOpen(NNshGlobal->silkRef) == errNone)
            {
                // SILK�̃��T�C�Y(�c����)���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
                VskSetState(NNshGlobal->silkRef, vskStateEnable, vskResizeVertically);
                NNshGlobal->silkVer = 2;
            }
            break;

          case vskVersionNum3:
          default:
            // SILK�}�l�[�W����VERSION3�A(UX�ȍ~)�A���̑��̃f�o�C�X
            if (VskOpen(NNshGlobal->silkRef) == errNone)
            {
                // SILK�̃��T�C�Y(������)���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
                ret = VskGetState(NNshGlobal->silkRef, vskStateResizeDirection, &vskState);
                if (ret != errNone)
                {
                    vskState = vskResizeDisable;
                }
                VskSetState(NNshGlobal->silkRef, vskStateEnable, vskState);
                NNshGlobal->silkVer = 3;
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
    if ((NNshGlobal->NNsiParam)->disableSonyHR != 0)
    {
        // HRLIB�͎g�p���Ȃ�
        NNshGlobal->hrRef    = 0;
        NNshGlobal->notUseHR = (NNshGlobal->NNsiParam)->disableSonyHR;
        return;
    }

    // HRLIB�̎Q�Ɣԍ����擾����
    ret = SysLibFind(sonySysLibNameHR, &(NNshGlobal->hrRef));
    if (ret == sysErrLibNotFound)
    {
        // HRLIB�������[�h�̏ꍇ�A���[�h�ł��邩Try���Ă݂�B
        ret = SysLibLoad('libr', sonySysFileCHRLib, &(NNshGlobal->hrRef));
    }
    if (ret != errNone)
    {
        // HRLIB�͎g�p�ł��Ȃ�
        NNshGlobal->hrRef = 0;
        return;
    }
    // HRLIB�̎g�p�錾��HRLIB�̃o�[�W�����ԍ��擾
    HROpen(NNshGlobal->hrRef);
    HRGetAPIVersion(NNshGlobal->hrRef, &(NNshGlobal->hrVer));
    if (NNshGlobal->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
    {
        // ���o�[�W�����̃n�C���]...��ʃ��[�h���m�F����...
        ret = HRWinScreenMode(NNshGlobal->hrRef, winScreenModeGet, 
                              &width, &height, &depth, &isColor);
        if (ret != errNone)
        {
            // HRLIB�͎g�p���Ȃ�
            HRClose(NNshGlobal->hrRef);
            NNshGlobal->hrRef = 0;
            return;
        }

        NNshGlobal->prevHRWidth = width;
        if (width != hrWidth)
        {
            // ��ʃ��[�h���n�C���]�łȂ��̂ŁA�n�C���]���[�h�ɐ؂�ւ���
            width  = hrWidth;
            height = hrHeight;
            ret    = HRWinScreenMode(NNshGlobal->hrRef, winScreenModeSet,
                                     &width, &height, &depth, &isColor);
            if (ret != errNone)
            {
                // HRLIB�͎g�p���Ȃ�
                HRClose(NNshGlobal->hrRef);
                NNshGlobal->hrRef = 0;
                return;
            }
        }
    }
    else
    {
        // ��ʃ��[�h�͕ύX���Ȃ��B
        NNshGlobal->prevHRWidth = hrWidth;
    }
    return;
}
#endif

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :   resetCLIESupport_NNsh                                    */
/*                                                  CLIE�T�|�[�g��������� */
/*-------------------------------------------------------------------------*/
static void resetCLIESupport_NNsh(void)
{
    Boolean                isColor;
    UInt32                 width, height, depth;

    // JOG�A�V�X�g�@�\�����ɖ߂�
    if (NNshGlobal->jogAssistSetBack != NULL)
    {
        *(NNshGlobal->jogAssistSetBack) = NNshGlobal->jogInfoBack;
    }

    if (NNshGlobal->silkRef == 0)
    {
        // SONY�p�V���N����̋@�\���g�p���Ȃ��ꍇ�ɂ�HRLIB�̉����
        goto SONY_HRLIB_UNLOAD;
    }

    // sysNotifyDisplayChangeEvent��unsubscribe����
    (void) SysNotifyUnregister(0, SOFT_CREATOR_ID, sysNotifyDisplayChangeEvent,
                               sysNotifyNormalPriority);

    // SILK�}�l�[�W���̉��
    if (NNshGlobal->silkVer == 1)
    {
        // ��SILK�}�l�[�W��(NR�p)
        SilkLibDisableResize(NNshGlobal->silkRef);
        SilkLibClose(NNshGlobal->silkRef);
    }
    else
    {
        // �VSILK�}�l�[�W��(NX�ȍ~)
        VskSetState(NNshGlobal->silkRef, vskStateEnable, vskResizeDisable);
        VskClose(NNshGlobal->silkRef);
    }

SONY_HRLIB_UNLOAD:
    if (NNshGlobal->hrRef != 0)
    {
        // �A�v���N�����ɁA��ʃ��[�h��������猳�ɖ߂�
        if (NNshGlobal->prevHRWidth != hrWidth)
        {
            HRWinScreenMode(NNshGlobal->hrRef, winScreenModeGet,
                            &width, &height, &depth, &isColor);
            width      = stdWidth;
            height     = stdHeight;
            (void) HRWinScreenMode(NNshGlobal->hrRef, winScreenModeSet,
                                   &width, &height, &depth, &isColor);
        }
        // Sony HRLIB�̎g�p�I��
        HRClose(NNshGlobal->hrRef);
    }
    return;
}
#endif

#ifdef USE_HANDERA
/*-------------------------------------------------------------------------*/
/*   Function :   setHandEraSupport_NNsh                                   */
/*                                               HandEra�f�o�C�X�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
static void setHandEraSupport_NNsh(void)
{
    UInt32              handEraOsVersion;
    VgaScreenModeType   handEraScreenMode;     // �X�N���[�����[�h
    VgaRotateModeType   handEraRotateMode;     // ��]���[�h

    // HandEra QVGA �T�|�[�g�L�����m�F����B
    handEraOsVersion = 0;
    if (_TRGVGAFeaturePresent(&handEraOsVersion))          
    {
        NNshGlobal->device = NNSH_DEVICE_HANDERA;
    }

    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
        VgaSetScreenMode(screenMode1To1, handEraRotateMode);
    }
}
#endif // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
/*-------------------------------------------------------------------------*/
/*   Function :   setDIASupport_NNsh                                       */
/*                                    Dynamic Input Area�f�o�C�X�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
static void setDIASupport_NNsh(void)
{
    Err    err;
    UInt32 ver, iFlag;

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
            NNshGlobal->device = NNSH_DEVICE_GARMIN;
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
         NNshGlobal->device = NNSH_DEVICE_DIASUPPORT;
         NNsh_DebugMessage(ALTID_INFO, "enable DIA", "", 0);
    }
    return;
}
#endif  // #ifdef USE_PIN_DIA

#ifdef USE_PIN_DIA
/*-------------------------------------------------------------------------*/
/*   Function :   resetDIASupport_NNsh                                     */
/*                                            Dynamic Input Area�̎g�p�J�� */
/*-------------------------------------------------------------------------*/
void resetDIASupport_NNsh(void)
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
static void HandEraMoveObject(FormPtr frmP, UInt16 objIndex,
                              Coord x_diff, Coord y_diff)
{
    Coord   old_x, old_y;

    // �ꏊ�̎擾
    FrmGetObjectPosition(frmP, objIndex, &old_x, &old_y);
    if (old_y < 110)
    {
        // ��̕��ɂ���I�u�W�F�N�g�͈ړ������Ȃ�
        // (�����[�x�^����...������ƃJ�b�R���...)
        // x_diff = 0;
        y_diff = 0;
    }
    FrmSetObjectPosition(frmP, objIndex, old_x + x_diff, old_y + y_diff);

#ifdef USE_GLUE
    if (FrmGlueGetObjectUsable(frmP, objIndex) != true)
    {
        // �B��Ă���I�u�W�F�N�g�͕\�����Ȃ�
        FrmHideObject(frmP, objIndex);
    }
    else
#endif
    {
        // �ړ������I�u�W�F�N�g��\������
        FrmShowObject(frmP, objIndex);
    }

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeObject                                      */
/*                                                 �I�u�W�F�N�g�̃��T�C�Y  */
/*-------------------------------------------------------------------------*/
static void HandEraResizeObject(FormPtr frmP, UInt16 objIndex,
                                Coord x_diff, Coord y_diff)
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
    if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNshGlobal->device == NNSH_DEVICE_GARMIN))
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

#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
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
    if (NNshGlobal->silkRef != 0)
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
void NNshRestoreNNsiSetting(Int16 version, UInt16 size, NNshSavedPref *prm)
{
    Err           ret;
    UInt32        offset;
    UInt16        cnt, nofRec;
    DmOpenRef     dbRef;
    Char         *ptr;
    RGBColorType  color;

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
                NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_READFAIL, "", ret);
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
        prm->lastFrmID            = NNSH_FRMID_THREAD;
        prm->bufferSize           = NNSH_WORKBUF_DEFAULT;
        prm->partGetSize          = NNSH_WORKBUF_DEFAULT;
        prm->netTimeout           = NNSH_NET_TIMEOUT;
        prm->currentFont          = NNSH_DEFAULT_FONT;
        prm->sonyHRFont           = NNSH_DEFAULT_FONT_HR;
        prm->sonyHRFontTitle      = NNSH_DEFAULT_FONT_HR;
        prm->useCookieWrite       = NNSH_ENABLE;
        prm->disableUnderline     = NNSH_ENABLE;
        prm->writeBufferSize      = NNSH_WRITEBUF_DEFAULT;
        prm->writeAlwaysSage      = NNSH_ENABLE;
        prm->printNofMessage      = NNSH_ENABLE;
        prm->notListReadOnly      = NNSH_ENABLE;
        prm->autoOpenNotRead      = NNSH_ENABLE;
        prm->addReturnToList      = NNSH_ENABLE;
        prm->addMenuGraphView     = NNSH_ENABLE;
        prm->addMenuBackRtnMsg    = NNSH_ENABLE;
        prm->addMenuGraphTitle    = NNSH_ENABLE;
        prm->addMenuDirSelect     = NNSH_ENABLE;
        prm->newArrivalNotRead    = NNSH_SELBBS_NOTREAD;
        prm->displayFavorLevel    = NNSH_MSGATTR_FAVOR_L1;
        prm->useSonyTinyFontTitle = NNSH_ENABLE;
        prm->useSonyTinyFont      = NNSH_ENABLE;
        prm->msgNumLimit          = NNSH_MESSAGE_MAXNUMBER;
        prm->viewMultiBtnFeature  = NNSH_DEFAULT_VIEW_LVLBTNFEATURE;
        prm->viewTitleSelFeature  = NNSH_DEFAULT_VIEW_TITLESELFEATURE;
        prm->dispBottom           = NNSH_ENABLE;
        prm->bbsOverwrite         = NNSH_ENABLE;
        prm->browseMesNum         = 50;
        prm->prepareDAforNewArrival = NNSH_CREATORID_PREPARE_NEWARRIVAL;
        StrCopy(prm->messageSeparator, "\n------\n");

#ifdef USE_COLOR
        // OS 3.5�ȏ�̏ꍇ
        if (NNshGlobal->palmOSVersion >= 0x03503000)
        {
            // �ԐF(�G���[����)
            MemSet(&color, sizeof(color), 0x00);
            color.r = 255;
            prm->colorError       = WinRGBToIndex(&color);

            // ��(�S�ēǂ񂾁A1000���A�㉺�{�^��)
            MemSet(&color, sizeof(color), 0x00);
            prm->colorAlready        = WinRGBToIndex(&color);
            prm->colorOver           = prm->colorAlready;
            prm->colorButton         = prm->colorOver;
            prm->colorViewFG         = prm->colorButton;
            prm->colorMsgHeader      = prm->colorButton;
            prm->colorMsgHeaderBold  = prm->colorButton;
            prm->colorMsgFooter      = prm->colorButton;
            prm->colorMsgFooterBold  = prm->colorButton;
            prm->colorMsgInform      = prm->colorButton;
            prm->colorMsgInformBold  = prm->colorButton;

            // ������(���ǂ���)
            // MemSet(&color, sizeof(color), 0x00);
            color.b = 100;
            prm->colorRemain      = WinRGBToIndex(&color);

            // ��(�V�K�擾�A�����擾)
            MemSet(&color, sizeof(color), 0x00);
            color.b = 255;
            prm->colorNew         = WinRGBToIndex(&color);
            prm->colorUpdate      = prm->colorNew;

            // �D�F(���擾�A���̑�)
            MemSet(&color, sizeof(color), 0x00);
            color.r = 128;
            color.g = 128;
            color.b = 128;
            prm->colorNotYet      = WinRGBToIndex(&color);
            prm->colorUnknown     = prm->colorNotYet;
            prm->colorOver        = prm->colorNotYet;
        }
#endif
        // ���ړI�X�C�b�`�P(�ꗗ���)
        prm->usageOfTitleMultiSwitch1 = NNSH_SWITCH1USAGE_DEFAULT;

        // ���ړI�X�C�b�`�Q(�ꗗ���)
        prm->usageOfTitleMultiSwitch2 = NNSH_SWITCH2USAGE_DEFAULT;

        // ���ړI�{�^���P(�ꗗ���)
        StrCopy(prm->multiBtn1Caption, MULTIBTN1_CAPTION_DEFAULT);
        prm->multiBtn1Feature = ((MULTIBTN1_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // ���ړI�{�^���Q(�ꗗ���)
        StrCopy(prm->multiBtn2Caption, MULTIBTN2_CAPTION_DEFAULT);
        prm->multiBtn2Feature = ((MULTIBTN2_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // ���ړI�{�^���R(�ꗗ���)
        StrCopy(prm->multiBtn3Caption, MULTIBTN3_CAPTION_DEFAULT);
        prm->multiBtn3Feature = ((MULTIBTN3_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // ���ړI�{�^���S(�ꗗ���)
        StrCopy(prm->multiBtn4Caption, MULTIBTN4_CAPTION_DEFAULT);
        prm->multiBtn4Feature = ((MULTIBTN4_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // ���ړI�{�^���T(�ꗗ���)
        StrCopy(prm->multiBtn5Caption, MULTIBTN5_CAPTION_DEFAULT);
        prm->multiBtn5Feature = ((MULTIBTN5_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // ���ړI�{�^���U(�ꗗ���)
        StrCopy(prm->multiBtn6Caption, MULTIBTN6_CAPTION_DEFAULT);
        prm->multiBtn6Feature = ((MULTIBTN6_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // �ꗗ���n�[�h�L�[��ݒ�
        prm->ttlFtr.up = ((UPBTN_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // �ꗗ���n�[�h�L�[���ݒ�
        prm->ttlFtr.down = ((DOWNBTN_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // �ꗗ��JOG Push�{�^���ݒ�
        prm->ttlFtr.jogPush = ((JOGPUSH_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // �ꗗ��JOG Back�{�^���ݒ�
        prm->ttlFtr.jogBack = ((JOGBACK_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // �ꗗ��Capture�{�^���ݒ�
        prm->ttlFtr.clieCapture = ((CAPTURE_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // �ꗗ�����{�^���ݒ�
        prm->ttlFtr.left = ((LEFTBTN_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // �ꗗ���E�{�^���ݒ�
        prm->ttlFtr.right = ((RIGHTBTN_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // �Q�Ǝ��n�[�h�L�[��ݒ�
        prm->viewFtr.up   = ((UPBTN_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // �Q�Ǝ��n�[�h�L�[���ݒ�
        prm->viewFtr.down = ((DOWNBTN_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // �Q�Ǝ� JOG Push�{�^���ݒ�
        prm->viewFtr.jogPush     = ((JOGPUSH_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // �Q�Ǝ� JOG Back�{�^���ݒ�
        prm->viewFtr.jogBack     = ((JOGBACK_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // �Q�Ǝ� Capture�{�^���ݒ�
        prm->viewFtr.clieCapture = ((CAPTURE_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // �Q�Ǝ����{�^���ݒ�
        prm->viewFtr.left = ((LEFTBTN_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // �Q�Ǝ��E�{�^���ݒ�
        prm->viewFtr.right = ((RIGHTBTN_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // ���E�{�^���̕W���ݒ�
        prm->useKey.left   = HARDKEY_FEATURE_CONTROLNNSI;
        prm->useKey.right  = HARDKEY_FEATURE_CONTROLNNSI;

        // ���[�U�ݒ�P�̊m�F
        prm->custom1.newArrival = NNSH_SELBBS_CUSTOM1;
        prm->custom1.getList    = NNSH_SELBBS_CUSTOM1;
        prm->custom1.boardNick  = NNSH_SELBBS_CUSTOM1;
        StrCopy(prm->custom1.tabName, CUSTOM1_THREAD_NAME);
        prm->custom1.condition  = NNSH_CONDITION_GETERR_ERROR;

        // ���[�U�ݒ�Q�̊m�F(Palm�֘A�̃X�����o�ݒ�)
        prm->custom2.newArrival = NNSH_SELBBS_CUSTOM2;
        prm->custom2.getList    = NNSH_SELBBS_CUSTOM2;
        prm->custom2.boardNick  = NNSH_SELBBS_CUSTOM2;

        StrCopy(prm->custom2.tabName, CUSTOM2_THREAD_NAME);
        StrCopy(prm->custom2.string1, NNSH_STRING_CUSTOM2_DEFAULT1);
        StrCopy(prm->custom2.string2, NNSH_STRING_CUSTOM2_DEFAULT2);
        StrCopy(prm->custom2.string3, NNSH_STRING_CUSTOM2_DEFAULT3);
        prm->custom2.stringSet  = NNSH_STRING_SELECTION | NNSH_STRING_OR;

        // ���[�U�ݒ�R�̊m�F(�X�����ĂR���ȓ�)
        prm->custom3.newArrival = NNSH_SELBBS_CUSTOM3;
        prm->custom3.getList    = NNSH_SELBBS_CUSTOM3;
        prm->custom3.boardNick  = NNSH_SELBBS_CUSTOM3;
        StrCopy(prm->custom3.tabName, CUSTOM3_THREAD_NAME);
        prm->custom3.threadCreate = 3;

        // ���[�U�ݒ�S�̊m�F(�ő咴��)
        prm->custom4.newArrival = NNSH_SELBBS_CUSTOM4;
        prm->custom4.getList    = NNSH_SELBBS_CUSTOM4;
        prm->custom4.boardNick  = NNSH_SELBBS_CUSTOM4;
        StrCopy(prm->custom4.tabName, CUSTOM4_THREAD_NAME);
        prm->custom4.threadStatus = (1 << NNSH_SUBJSTATUS_OVER);
        
        // ���[�U�ݒ�T�̊m�F
        prm->custom5.newArrival = NNSH_SELBBS_CUSTOM5;
        prm->custom5.getList    = NNSH_SELBBS_CUSTOM5;
        prm->custom5.boardNick  = NNSH_SELBBS_CUSTOM5;
        StrCopy(prm->custom5.tabName, CUSTOM5_THREAD_NAME);
        prm->custom5.condition  = NNSH_CONDITION_GETRSV_RSV;
        
        // �ꗗ�\�����ɏd���̊m�F�����{����悤�ύX����
        prm->checkDuplicateThread = NNSH_ENABLE;

        // �ʐM�^�C���A�E�g�������A�P��Ď��s����悤�ύX����
        prm->nofRetry = 1;

        // �V�����G���[����������ƍĎ擾����悤�ύX����
        prm->autoUpdateGetError   = NNSH_ENABLE;

        // �Q�ƃ��O�̎擾���x�����P�ɐݒ�
        prm->getROLogLevel = 1;

        // �Q�ƃ��O�ꗗ��� Jog PUSH�́A�I�����j���[���J��
        prm->getLogFeature.jogPush   = NNSH_GETLOG_FEATURE_OPENMENU;

        // �Q�ƃ��O�ꗗ��� Jog Back�́A�ꗗ��ʂɖ߂�
        prm->getLogFeature.jogBack   = NNSH_GETLOG_FEATURE_CLOSE_GETLOG;

        // �Q�ƃ��O�ꗗ��� Jog Left�́A�O�y�[�W��\������
        prm->getLogFeature.left      = NNSH_GETLOG_FEATURE_PREVPAGE;

        // �Q�ƃ��O�ꗗ��� Jog Right�́A���y�[�W��\������
        prm->getLogFeature.right     = NNSH_GETLOG_FEATURE_NEXTPAGE;

        // �Q�ƃ��O�ꗗ��� Up�́A�O�A�C�e����\������
        prm->getLogFeature.up        = NNSH_GETLOG_FEATURE_PREVITEM;

        // �Q�ƃ��O�ꗗ��� Down�́A���A�C�e����\������
        prm->getLogFeature.down      = NNSH_GETLOG_FEATURE_NEXTITEM;

        // VFS������ ON�ɂ���ݒ�(�����l)...
        prm->vfsOnDefault            = ((NNSH_VFS_ENABLE)|
                                        (NNSH_VFS_WORKAROUND)|
                                        (NNSH_VFS_USEOFFLINE)|
                                        (NNSH_VFS_DIROFFLINE)|
                                        (NNSH_VFS_DBBACKUP));

        // �������Ɏw�肷�镶����w�b�_
        StrCopy(prm->viewSearchStrHeader, NNSH_VIEW_SEARCHSTRING_HEADER);

        // �W����BBS�ꗗ�擾��(URL)�𔽉f������
        StrCopy(prm->bbs_URL, URL_BBSTABLE);
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_REFRESH, "", 0);
    }

    // �i���݌������[�h���N���A����...
    prm->searchPickupMode = 0;

    // NG-1�����NG-2�̐ݒ��","�ŋ�؂�ƕ�����NG���[�h�ƔF������
    prm->useRegularExpression = 1;

    // ���O��؂�DB���I�[�v������
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    GetDBCount_NNsh(dbRef, &nofRec);
    if (nofRec == 0)
    {
        // ���R�[�h���P�����Ȃ������ꍇ�ɂ́A�P���f�[�^��ǉ�����
        NNshLogTokenDatabase  dmyData;
        MemSet(&dmyData, sizeof(NNshLogTokenDatabase), 0x00);
        dmyData.usable = 1;
        StrCopy(dmyData.tokenPatternName, "(none)");
        EntryRecord_NNsh(dbRef, sizeof(NNshLogTokenDatabase), &dmyData);
    }
    CloseDatabase_NNsh(dbRef);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshSaveNNsiSetting                                      */
/*                                                         NNsi�ݒ�̋L��  */
/*-------------------------------------------------------------------------*/
static void NNshSaveNNsiSetting(Int16 version, UInt16 size,
                                NNshSavedPref *param)
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
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
#ifdef USE_PIN_DIA
    if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNshGlobal->device == NNSH_DEVICE_GARMIN))
    {
#if 0
        if (PINSetInputAreaState(pinInputAreaClosed) != errNone)
        {
            HandEraResizeForm(frm);
        }
#else
        // �K�����T�C�Y����悤�ɂ��Ă݂�...
        PINSetInputAreaState(pinInputAreaClosed);
        HandEraResizeForm(frm);
#endif
        return (errNone);
    }
#endif
#ifdef USE_CLIE
    // SILK�}�l�[�W�����n�o�d�m����Ă��邩�H
    if (NNshGlobal->silkRef != 0)
    {
        // SILK�̍ŏ������s��
        if (NNshGlobal->silkVer == 1)
        {
            // Version1(NR�p)
            SilkLibResizeDispWin(NNshGlobal->silkRef, silkResizeToStatus);
        }
        else
        {
            // Version2(NX�ȍ~)
            VskSetState(NNshGlobal->silkRef, vskStateResize, vskResizeMin);
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
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMaximizeWindow();
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
#ifdef USE_PIN_DIA
    if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNshGlobal->device == NNSH_DEVICE_GARMIN))
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
    if (NNshGlobal->silkRef != 0)
    {
        // SILK�̍ő剻���s��
        if (NNshGlobal->silkVer == 1)
        {
            // Version1(NR�p)
            SilkLibResizeDispWin(NNshGlobal->silkRef, silkResizeNormal);
        }
        else
        {
            // Version2(NX�ȍ~)
            VskSetState(NNshGlobal->silkRef, vskStateResize, vskResizeMax);
        }
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
    return (~errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshClearGlobal                                          */
/*                                          NNsi�̃O���[�o���̈���J������ */
/*-------------------------------------------------------------------------*/
static void NNshClearGlobal(NNshWorkingInfo *workBuf)
{
    NNshMessageIndex *idxP;

#ifdef USE_PIN_DIA
    // pen input manager�̐ݒ����
    resetDIASupport_NNsh();
#endif    // #ifdef USE_PIN_DIA

#ifdef USE_LOGCHARGE
    // (�m�ۍς݂Ȃ�)�Q�ƃ��O�ꗗ��ʂ̗̈���J������
    if (workBuf->dispList != NULL)
    {
        ClearList_getLogList(workBuf->dispList);
        MEMFREE_PTR(workBuf->dispList);
    }
#endif  // #ifdef USE_LOGCHARGE

    // (�m�ۍς݂Ȃ�)NG�ݒ�̗̈���J������
    ReleaseWordList(&(workBuf->hide1));
    ReleaseWordList(&(workBuf->hide2));

    // (�m�ۍς݂Ȃ�)������ݒ�֘A�̗̈���N���A����
    MEMFREE_PTR(workBuf->bookmarkFileName);
    MEMFREE_PTR(workBuf->bookmarkNick);

    // �O���[�o���̈�̃n���h�����N���A����
    MEMFREE_HANDLE(workBuf->searchTitleH);
    MEMFREE_HANDLE(workBuf->boardIdxH);
    MEMFREE_HANDLE(workBuf->logPrefixH);
    MEMFREE_HANDLE(workBuf->bbsTitleH);

    MEMFREE_PTR(workBuf->bbsTitles);
    MEMFREE_PTR(workBuf->customizedUserAgent);
    MEMFREE_PTR(workBuf->connectedSessionId);
    MEMFREE_PTR(workBuf->be2chCookie);

    if (workBuf->msgIndex != NULL)
    {
        MEMFREE_PTR((workBuf->msgIndex)->msgOffset);
        idxP = workBuf->msgIndex;
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(workBuf->msgIndex);
    }

    if (workBuf->tempInfo != NULL)
    {
        MEMFREE_PTR((workBuf->tempInfo)->checkBoxState);
        MEMFREE_PTR((workBuf->tempInfo)->BBSName_Temp);
        MEMFREE_PTR(workBuf->tempInfo);
    }

    return;
}

/*=========================================================================*/
/*   Function :   BackupDatabaseToVFS                                      */
/*                                         NNsi��DB��VFS�Ƀo�b�N�A�b�v���� */
/*=========================================================================*/
void BackupDBtoVFS(UInt16 backupAllDB)
{
    Err   ret;
    Char *buf, flag;

    flag = 0;

    // BUSY�E�B���h�E�̕\���̈�m��
    buf = MEMALLOC_PTR(BUFSIZE);
    if (buf == NULL)
    {

        // �G���[�\��
        NNsh_DebugMessage(ALTID_ERROR,"Cannot Alloc BUFFER"," size:",BUFSIZE);
        return;
    }
    MemSet(buf, BUFSIZE, 0x00);

    // (BBS-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_BBSLIST);
    Show_BusyForm(buf);
    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_BBSLIST);
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 1;
        }
    }

    // (Subject-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_SUBJECT);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_SUBJECT)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_SUBJECT);
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 2;
        }
    }

    // (Settings-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_SETTING);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_SETTING)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_SETTING);
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 4;
        }
    }

#ifdef USE_LOGCHARGE
    // (LogPermanent-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_PERMANENTLOG);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_LOGCHARGE)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_PERMANENTLOG);
#if 0
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 8;
        }
#endif
    }
#endif // #ifdef USE_LOGCHARGE

    // (GetReserve-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_GETRESERVE);
    SetMsg_BusyForm(buf);

    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_GETRESERVE);
#if 0
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 16;
        }
#endif
    }

    // (DirIndex-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_DIRINDEX);
    SetMsg_BusyForm(buf);

    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_DIRINDEX);
#if 0
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 32;
        }
#endif
    }

    // (NGWord-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_NGWORD);
    SetMsg_BusyForm(buf);

    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_NGWORD);
#if 0
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 64;
        }
#endif
    }

#ifdef USE_MACRO
    // (Macro-NNsi �̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf,DBNAME_MACROSCRIPT);
    SetMsg_BusyForm(buf);

    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_MACROSCRIPT);
#if 0
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 128;
        }
#endif
    }
#endif // #ifdef USE_MACRO

#ifdef USE_LOGCHARGE
    // (LogToken-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_LOGTOKEN);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_LOGTOKEN)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_LOGTOKEN);
#if 0
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 256;
        }
#endif
    }
#endif // #ifdef USE_LOGCHARGE

#ifdef USE_LOGCHARGE
    // (LogTemporary-NNsi�̃o�b�N�A�b�v)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_TEMPORARYLOG);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_LOGSUBDB)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_TEMPORARYLOG);
#if 0
        if (ret != errNone)
        {
            // �G���[�ł������Ȃ�
            flag = flag + 512;
        }
#endif
    }
#endif // #ifdef USE_LOGCHARGE

    // BUSY�\������������
    Hide_BusyForm(false);

    MEMFREE_PTR(buf);
    if (flag != 0)
    {
        // �o�b�N�A�b�v�G���[�������������Ƃ�ʒm����
        NNsh_WarningMessage(ALTID_ERROR, MSG_NNSI_DBBACKUP_FAIL, "", flag);
    }
    NNshGlobal->updateDatabaseInfo = 0;

    return;
}

/*=========================================================================*/
/*   Function :   RestoreDBtoVFS                                           */
/*                                           NNsi��DB��VFS���烊�X�g�A���� */
/*=========================================================================*/
Err RestoreDBtoVFS(Boolean forceRead)
{
    Err      ret, ret1, ret2;
    FileRef  fileRef;
    Char     fileName[MAXLENGTH_FILENAME];
    UInt32   dummy;
    UInt16   butId;
    NNshVFSDBInfo vfsDB[3];
    UInt16 idx;
    ControlType  *chkObj, *lblObj;
    UInt8 fileExistCnt = 0;
    UInt8 importFileNum, deleteFileNum;
    NNshDBInfo dbInfo;
    DateTimeType date;
    
    struct {
        UInt16 chkIdImport;
        UInt16 chkIdDelete;
        UInt16 idx;
        UInt16 lblIdMEM;
        UInt16 lblIdVFS;
    } importInfo[] = {
          {CHKID_IMPORT_SETTING, CHKID_DELETE_SETTING, NNSH_VFSDB_SETTING, LBLID_DBMEM_SETTING, LBLID_DBVFS_SETTING},
          {CHKID_IMPORT_SUBJECT, CHKID_DELETE_SUBJECT, NNSH_VFSDB_SUBJECT, LBLID_DBMEM_SUBJECT, LBLID_DBVFS_SUBJECT},
          {CHKID_IMPORT_BBSLIST, CHKID_DELETE_BBSLIST, NNSH_VFSDB_BBSLIST, LBLID_DBMEM_BBSLIST, LBLID_DBVFS_BBSLIST},
    };
    int i;
            
    
    importFileNum = 0;
    deleteFileNum = 0;
    MemSet(vfsDB, sizeof(vfsDB), 0x00);
    MemSet(&dbInfo, sizeof(dbInfo), 0x00);
    
    // VFS�̃T�|�[�g�L�����m�F����B
    dummy = 0;
    ret = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &dummy);
    if (ret != errNone)
    {
        // VFS��T�|�[�g�̏ꍇ�ɂ́A���I���B
        NNsh_DebugMessage(ALTID_INFO, "Not Support VFS :", "", ret);
        return (~errNone);
    }

    // VFS���g�p�ł���Ƃ��ABackup���ꂽDB�����邩(FILE��OPEN����)�m�F
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(fileName, DATAFILE_PREFIX);
    StrCat (fileName, DBNAME_SUBJECT);
    StrCat (fileName, ".pdb");
    if (VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &fileRef) !=
        errNone)
    {
        if (forceRead == true)
        {
            // BACKUP���ꂽ�t�@�C�����Ȃ��̂ŏI������
            NNsh_DebugMessage(ALTID_INFO, "Backup DB files do not exist", "", 0);
            return (~errNone);
        }
        vfsDB[NNSH_VFSDB_SUBJECT].fileExist = 0;
    }
    else
    {
        vfsDB[NNSH_VFSDB_SUBJECT].fileExist = 1;
        VFSFileDBInfo(fileRef, NULL, NULL, NULL, NULL, &vfsDB[NNSH_VFSDB_SUBJECT].modDateVFS,
                      NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        fileExistCnt++;
    }
    // File Open����(== DB������)�A���X�g�A���邩�m�F����
    VFSFileClose(fileRef);
    
    if (forceRead == true)
    {
        butId = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_RESTORE, "", 0);
        if (butId != 0)
        {
            // Cancel���I�����ꂽ���߁A���X�g�A���Ȃ��B
            NNsh_DebugMessage(ALTID_INFO, "DB-Restore is canceled.", "", 0);
            return (~errNone);
        }
    }
    else
    {
        FormPtr frm, curFrm;
        Char *p;
        
        // �t�@�C�������݂��邩�m�F����
        StrCopy(fileName, DATAFILE_PREFIX);
        StrCat (fileName, DBNAME_SETTING);
        StrCat (fileName, ".pdb");

        if (VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &fileRef) !=
            errNone)
            vfsDB[NNSH_VFSDB_SETTING].fileExist = 0;
        else
        {
            vfsDB[NNSH_VFSDB_SETTING].fileExist = 1;
            VFSFileDBInfo(fileRef, NULL, NULL, NULL, NULL, &vfsDB[NNSH_VFSDB_SETTING].modDateVFS,
                          NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            fileExistCnt++;
        }
        VFSFileClose(fileRef);

        StrCopy(fileName, DATAFILE_PREFIX);
        StrCat (fileName, DBNAME_BBSLIST);
        StrCat (fileName, ".pdb");

        if (VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &fileRef) !=
            errNone)
            vfsDB[NNSH_VFSDB_BBSLIST].fileExist = 0;
        else
        {
            vfsDB[NNSH_VFSDB_BBSLIST].fileExist = 1;
            VFSFileDBInfo(fileRef, NULL, NULL, NULL, NULL, &vfsDB[NNSH_VFSDB_BBSLIST].modDateVFS,
                          NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            fileExistCnt++;
        }
        VFSFileClose(fileRef);

        // VFS ��� DB �t�@�C�����Ȃ����߁A���X�g�A���Ȃ��B
        if (!fileExistCnt)
            return(~errNone);

        // �{�̓��ɂ���DB�̏����擾
        dbInfo.modDateP = &vfsDB[NNSH_VFSDB_SETTING].modDateMEM;
        StrCopy(fileName, DBNAME_SETTING);
        GetDBInfo_NNsh(fileName, &dbInfo);

        dbInfo.modDateP = &vfsDB[NNSH_VFSDB_SUBJECT].modDateMEM;
        StrCopy(fileName, DBNAME_SUBJECT);
        GetDBInfo_NNsh(fileName, &dbInfo);
        
        dbInfo.modDateP = &vfsDB[NNSH_VFSDB_BBSLIST].modDateMEM;
        StrCopy(fileName, DBNAME_BBSLIST);
        GetDBInfo_NNsh(fileName, &dbInfo);
        
        curFrm = FrmGetActiveForm();
        frm = FrmInitForm(FRMID_DBIMPORT);

        // �t�@�C�������݂��Ȃ����ɂ��Ă̓`�F�b�N�{�b�N�X���B��
        for (i = 0; i < sizeof(importInfo) / sizeof(importInfo[0]); i++)
        {
            if (vfsDB[importInfo[i].idx].fileExist == 0)
            {
                idx = FrmGetObjectIndex(frm, importInfo[i].chkIdImport);
                chkObj = FrmGetObjectPtr(frm, idx);
                
                CtlSetValue(chkObj, 0);
                FrmHideObject(frm, idx);

                idx = FrmGetObjectIndex(frm, importInfo[i].chkIdDelete);
                chkObj = FrmGetObjectPtr(frm, idx);

                CtlSetValue(chkObj, 0);
                FrmHideObject(frm, idx);
            }
            else
            {
                idx = FrmGetObjectIndex(frm, importInfo[i].lblIdVFS);
                lblObj = FrmGetObjectPtr(frm, idx);
                TimSecondsToDateTime(vfsDB[importInfo[i].idx].modDateVFS, &date);
                        
                p = vfsDB[importInfo[i].idx].dateStrVFS;

                StrIToA(p, date.year);
                StrCat(p, "/");
                StrIToA(p + StrLen(p), date.month);
                StrCat(p, "/");
                StrIToA(p + StrLen(p), date.day);
                StrCat(p, " ");
                StrIToA(p + StrLen(p), date.hour);
                StrCat(p, ":");
                StrIToA(p + StrLen(p), date.minute);
                StrCat(p, ":");
                StrIToA(p + StrLen(p), date.second);
                
                CtlSetLabel(lblObj, p);
            }

            idx = FrmGetObjectIndex(frm, importInfo[i].lblIdMEM);
            lblObj = FrmGetObjectPtr(frm, idx);
            TimSecondsToDateTime(vfsDB[importInfo[i].idx].modDateMEM, &date);
            
            p = vfsDB[importInfo[i].idx].dateStrMEM;
            StrIToA(p, date.year);
            StrCat(p, "/");
            StrIToA(p + StrLen(p), date.month);
            StrCat(p, "/");
            StrIToA(p + StrLen(p), date.day);
            StrCat(p, " ");
            StrIToA(p + StrLen(p), date.hour);
            StrCat(p, ":");
            StrIToA(p + StrLen(p), date.minute);
            StrCat(p, ":");
            StrIToA(p + StrLen(p), date.second);
                
            CtlSetLabel(lblObj, p);
        }
        
        if(FrmDoDialog(frm) == BTNID_DIALOG_OK)
        {
            for(i = 0; i < sizeof(importInfo) / sizeof(importInfo[0]); i++)
            {
                chkObj = FrmGetObjectPtr(frm,
                                         FrmGetObjectIndex(frm, importInfo[i].chkIdImport));
                vfsDB[importInfo[i].idx].importDB = CtlGetValue(chkObj);
                if (vfsDB[importInfo[i].idx].importDB)
                    importFileNum++;

                chkObj = FrmGetObjectPtr(frm,
                                         FrmGetObjectIndex(frm, importInfo[i].chkIdDelete));
                vfsDB[importInfo[i].idx].deleteFile = CtlGetValue(chkObj);
                if (vfsDB[importInfo[i].idx].deleteFile)
                    deleteFileNum++;
            
            }
            FrmDeleteForm(frm);
            FrmSetActiveForm(curFrm);
        }
        else
        {
            FrmDeleteForm(frm);
            FrmSetActiveForm(curFrm);
            return(~errNone);
        }
    }

    if (importFileNum || forceRead == true)
    {
        // "���X�g�A��"�\��
        Show_BusyForm(MSG_DBRESTORE_BUSY);
    }
    else
    {
        if (deleteFileNum)
        {
            // "�폜��"�\��
            Show_BusyForm(MSG_DBDELETE_BUSY);
        }
        else
        {
            // ���X�g�A�A�폜���Ȃ�
            return(~errNone);
        }
    }

    // DB���X�g�A���C��
    if (forceRead == true)
        (void) RestoreDatabaseFromVFS_NNsh(DBNAME_DIRINDEX);
        
    if (vfsDB[NNSH_VFSDB_SETTING].importDB || forceRead == true)
    {
        ret2 = RestoreDatabaseFromVFS_NNsh(DBNAME_SETTING);
        NNshGlobal->restoreSetting = 1;
    }
    else
    {
        NNshGlobal->restoreSetting = 0;
        ret2 = errNone;
    }
        
    if (forceRead == false && vfsDB[NNSH_VFSDB_SETTING].deleteFile && ret2 == errNone)
    {
        StrCopy(fileName, DBNAME_SETTING);
        StrCat(fileName, ".pdb");
        
        // �t�@�C�����폜����
        (void)DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }
    
    if (vfsDB[NNSH_VFSDB_SUBJECT].importDB || forceRead == true)
        ret1 = (RestoreDatabaseFromVFS_NNsh(DBNAME_SUBJECT));
    else
        ret1 = errNone;
    
    if (forceRead == false && vfsDB[NNSH_VFSDB_SUBJECT].deleteFile && ret1 == errNone)
    {
        StrCopy(fileName, DBNAME_SUBJECT);
        StrCat(fileName, ".pdb");
        
        // �t�@�C�����폜����
        (void)DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }
    
    if (vfsDB[NNSH_VFSDB_BBSLIST].importDB || forceRead == true)
        ret = RestoreDatabaseFromVFS_NNsh(DBNAME_BBSLIST);
    else
        ret = errNone;
    
    if (forceRead == false && vfsDB[NNSH_VFSDB_BBSLIST].deleteFile && ret == errNone)
    {
        StrCopy(fileName, DBNAME_BBSLIST);
        StrCat(fileName, ".pdb");
        
        // �t�@�C�����폜����
        (void)DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }

    // "���X�g�A��" or "�폜��" �\���̍폜
    Hide_BusyForm(true);

    if (ret2 != errNone)
    {
        // ���X�g�A���s(�X���ꗗ)
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_NG, DBNAME_SETTING, 0);
    }

    if (ret1 != errNone)
    {
        // ���X�g�A���s(�X���ꗗ)
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_NG, DBNAME_SUBJECT, 0);
    }

    if (ret != errNone)
    {
        // ���X�g�A���s(BBS�ꗗ)
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_NG, DBNAME_BBSLIST, 0);
    }

    if (importFileNum || forceRead == true)
    {
        // ���X�g�A����
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_OK, "", 0);
    }

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetZLIBsupport_NNsh                                      */
/*                                              ZLIB�̃C���X�g�[���`�F�b�N */
/*-------------------------------------------------------------------------*/
void SetZLIBsupport_NNsh(void)
{
#ifdef USE_ZLIB
    if ((NNshGlobal->NNsiParam)->useGZIP != 0)
    {
        // ZLIB���I�[�v�����Ă݂�
        if(OpenZLib() == false)
        {
            // �I�[�v�����s�A�G���[�\��
            NNsh_ErrorMessage(ALTID_ERROR, MSG_TO_INSTALL_ZLIB, "", 0);
            NNshGlobal->useGzip = 0;
        }
        else
        {
            // GZIP���k��L���ɂ���
            NNshGlobal->useGzip = NNSH_ENABLE;
            CloseZLib();
        }
    }
#endif
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkInstalledWebBrowser                                 */
/*                                        WebBrowser�̃C���X�g�[���`�F�b�N */
/*-------------------------------------------------------------------------*/
static void checkInstalledWebBrowser(void)
{
    UInt16            cardNo;
    LocalID           dbID;
    DmSearchStateType searchState;
    Err               ret;

    // Xiino3���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'Xii3', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino3���C���X�g�[������Ă���
        NNshGlobal->browserCreator    = 'Xii3';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // NetFront���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NF3T', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // NetFront���C���X�g�[������Ă���
        NNshGlobal->browserCreator    = 'NF3T';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // WebBrowser 2.0���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NF3P', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // WebBrowser 2.0���C���X�g�[������Ă���
        NNshGlobal->browserCreator    = 'NF3P';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Xiino(PalmScape)���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'PScp', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino���C���X�g�[������Ă���
        NNshGlobal->browserCreator    = 'PScp';
        NNshGlobal->browserLaunchCode = 0xfa21;
        return;
    }

    // Xiino(LEM)���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'dXin', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino(LEM)���C���X�g�[������Ă���
        NNshGlobal->browserCreator    = 'dXin';
        NNshGlobal->browserLaunchCode = 0xfa21;
        return;
    }


    // Blazer���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'BLZ1', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Blazer���C���X�g�[������Ă���
        NNshGlobal->browserCreator    = 'BLZ1';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Blazer���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'BLZ5', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Blazer���C���X�g�[������Ă���
        NNshGlobal->browserCreator    = 'BLZ5';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Eudora Web���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'QCwb', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Eudora Web���C���X�g�[������Ă���
        NNshGlobal->browserCreator    = 'QCwb';
        NNshGlobal->browserLaunchCode = 0xfa0;
        // NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
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
        NNshGlobal->browserCreator    = 'PLNK';
        // NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdOpenDB;
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
        NNshGlobal->browserCreator    = 'NOVR';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
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
    UInt16 *command;

    // 5way�̓��͂łȂ���΁A�L�[�R�[�h�����̂܂܉�������
    if (event->data.keyDown.chr != vchrNavChange)
    {
        // �n�[�h�L�[����̊m�F�B�B�B
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // �n�[�h�L�[��ύX
            if (event->data.keyDown.chr >= vchrHardKeyMin)
            {
                // �n�[�h�L�[�R�[�h�̊m�F
                switch (event->data.keyDown.chr)
                {
                  case vchrHard1:
                  case vchrTapWaveSpecificKeyULeft:
                    // �n�[�h�L�[�P
                    command = &((NNshGlobal->NNsiParam)->useKey.key1);
                    break;

                  case vchrHard2:
                  case vchrTapWaveSpecificKeyURight:
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
                    // CLIE NX �L���v�`���{�^���ACLIE TG �{�C�X���R�{�^��
                    command = &((NNshGlobal->NNsiParam)->useKey.clieCapture);
                    event->data.keyDown.chr = vchrHard5;
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

                  default:
                    return (event->data.keyDown.chr);
                    break;
                }

                // �R�}���h�̔���
                switch (*command)
                {
                  case HARDKEY_FEATURE_CONTROLNNSI:
                    // NNsi�Ő���(�n�[�h�L�[�������ꂽ�Ƃ��̏��������s)
                    return (event->data.keyDown.chr);
                    break;

                  case HARDKEY_FEATURE_TRANSJOGPUSH:
                    // �L�[�R�[�h��JOG PUSH(LineFeed)�ɏ���������
                    return (chrLineFeed);
                    break;

                  case HARDKEY_FEATURE_TRANSJOGBACK:
                    // �L�[�R�[�h��JOG BACK(ESC)�ɏ���������
                    return (chrEscape);
                    break;

                  case HARDKEY_FEATURE_DONOTHING:
                  default:
                    // �������Ȃ�
                    break;
                }
            }

        }
        return (event->data.keyDown.chr);
    }
/**
//    if (((event->data.keyDown.keyCode) == 0)&&(event->data.keyDown.chr == vchrRockerCenter))
    if ((event->data.keyDown.keyCode) == 0)
    {
        // �J�[�\���̐^�񒆃L�[�𗣂����ꍇ... (Treo 650/Tungsten T5/Palm TX�Ή�)
        return (0);
    }
**/

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
/*   Function :   setDoubleDensitySupport_NNsh                             */
/*                                              ���𑜓x�T�|�[�g�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
static void setDoubleDensitySupport_NNsh(void)
{
    UInt32  version;

    // ���𑜓x�T�|�[�g�ݒ��������
    NNshGlobal->os5HighDensity = (UInt32) kDensityLow;

    // OS 3.5�����̏ꍇ
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        // �J���[���T�|�[�g�̒[��(�J���[�T�|�[�g�̐ݒ��s�ɂ���
        (NNshGlobal->NNsiParam)->useColor = 0;
    }
#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        // CLIE��������A���𑜓x�̃`�F�b�N�����Ȃ�(CLIE�n�C���]���g��)
        return;
    }
#endif

#ifdef USE_TSPATCH
    if ((NNshGlobal->NNsiParam)->notUseTsPatch != 0)
    {
        // TsPatch���g��Ȃ����́A���𑜓x�̃`�F�b�N�����Ȃ�(�W���𑜓x�ɂ���)
        return;
    }
#endif

    // Yomeru5 �𗘗p���Ă��邩�H
    if (FtrGet('Ymru', 5000, &version) == 0)
    {
        // Yomeru5�𗘗p���A�A�A���𑜓x���[�h�͎g�p���Ȃ����Ƃɂ���
        NNsh_DebugMessage(ALTID_INFO, "Yomeru5 Version", ": ", version);
        return;
    }

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
    WinScreenGetAttribute(winScreenDensity, &(NNshGlobal->os5HighDensity));

    // �Ƃ肠�����A�W���𑜓x�ɂ���
    WinSetCoordinateSystem(kCoordinatesStandard);

    return;
}
#endif   // #ifdef USE_HIGHDENSITY

/*=========================================================================*/
/*   Function : convertFormID                                              */
/*                                                �t�H�[���h�c�̃R���o�[�g */
/*=========================================================================*/
UInt16 ConvertFormID(UInt16 lastFrmID)
{
    UInt16 frmId;

    // lastFrmID ������ۂ̃t�H�[��ID�փR���o�[�g����
    switch (lastFrmID)
    {
      case NNSH_FRMID_MESSAGE:
        // �X���Q�Ɖ��
        frmId = FRMID_MESSAGE;
        break;

      case NNSH_FRMID_DUMMY:
        // �_�~�[���
        frmId = FRMID_NNSI_DUMMY;
        break;

      case NNSH_FRMID_WRITE_MESSAGE:
        // �������݉��
        frmId = FRMID_WRITE_MESSAGE;
        break;

      case NNSH_FRMID_GETLOGLIST:
        // �Q�ƃ��O�ꗗ���
        frmId = FRMID_LISTGETLOG;
        break;
#if 0
      case NNSH_FRMID_BOARDINPUT:
      case NNSH_FRMID_MESSAGEINFO:
      case NNSH_FRMID_MANAGEBBS:
      case NNSH_FRMID_INPUTDIALOG:
      case NNSH_FRMID_BUSY:
      case NNSH_FRMID_NNSI_SETTING7:
      case NNSH_FRMID_NNSI_SETTING6:
      case NNSH_FRMID_NNSI_SETTING5:
      case NNSH_FRMID_NNSI_SETTING4:
      case NNSH_FRMID_NNSI_SETTING3:
      case NNSH_FRMID_NNSI_SETTING2:
      case NNSH_FRMID_CONFIG_NNSH:
#endif
      case NNSH_FRMID_THREAD:
      default:
        // �X���ꗗ���
        frmId = FRMID_THREAD;
        break;
    }
    return (frmId);
}

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

        // �V���N�̗̈��Ԃ�O��ʂƓ����ɂ���...
        // PINSetInputAreaState(pinInputAreaUser);
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
