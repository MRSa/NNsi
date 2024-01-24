/*============================================================================*
 *  FILE: 
 *     machine.c
 *
 *  Description: 
 *     Palm device specific functions.
 *
 *===========================================================================*/
#define MACHINE_C

#ifdef USE_CLIE
// SONY�p�W���O�A�V�X�g�}�X�N
#define NUMBER_OF_MASKS   3
UInt16 NNshSonyJogMask[] = {
     sonyJogAstMaskType1, // �t�H�[�����ɌʂɃ}�X�N��ݒ肷��
     NUMBER_OF_MASKS,     // �}�X�N����t�H�[����
     FRMID_MESSAGE,       sonyJogAstMaskAll,
     FRMID_MANAGEBBS,     sonyJogAstMaskAll,
     FRMID_THREAD,        sonyJogAstMaskAll,
     0, 0, 0, 0           // �|������]�T...
};
#endif

extern Err setOfflineLogDir(Char *fileName);

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

    // VFS�̃T�|�[�g�L�����m�F����B
    vfsMgrVersion = 0;
    err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion);
    if (err == errNone)
    {
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
            if (volInfo.fsType == vfsFilesystemType_VFAT)
            {
                // "VFS�g�p�\���Ɏ����I��VFS ON" �ݒ���m�F����
                if (NNshParam->vfsOnAutomatic != 0)
                {
                    // VFS ON�ɂ���
                    NNshParam->useVFS = ((NNSH_VFS_ENABLE)|
                                         (NNSH_VFS_WORKAROUND)|
                                         (NNSH_VFS_USEOFFLINE)|
                                         (NNSH_VFS_DBBACKUP));
                }
                if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
                {
                    // NNsh�p�f�[�^�i�[�p�f�B���N�g���������쐬(VFS ON��)
                    StrCopy(dirName, DATAFILE_PREFIX);
                    dirName[StrLen(dirName) - 1] = '\0';
                    err = CreateDir_NNsh(dirName);
                    NNsh_DebugMessage(ALTID_INFO, "Created :", dirName, err);
                    err = errNone;
		}
                if ((NNshParam->useVFS & NNSH_VFS_USEOFFLINE) != 0)
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
        NNshParam->useVFS = NNSH_NOTSUPPORT_VFS;
    }
    return;
}

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
                *(NNshGlobal->jogAssistSetBack) =  NNshSonyJogMask;

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
        // SILK�}�l�[�W���͎g�p�ł��Ȃ��A�����ɂ���SONY HRLIB���[�h��
        NNshGlobal->silkRef = 0;
        goto SONY_HRLIB_LOAD;
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
        }
    }
    else
    {
        // SILK�}�l�[�W����VERSION2�A(NX�ȍ~)
        if (VskOpen(NNshGlobal->silkRef) == errNone)
        {
            // SILK�̃��T�C�Y���T�|�[�g���Ă��邱�Ƃ�(SILK�}�l�[�W����)�ʒm
            VskSetState(NNshGlobal->silkRef, vskStateEnable, 1);
            NNshGlobal->silkVer = 2;
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
        VskSetState(NNshGlobal->silkRef, vskStateEnable, 0);
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

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraMoveObject                                        */
/*                                                     �I�u�W�F�N�g�̈ړ�  */
/*-------------------------------------------------------------------------*/
static void HandEraMoveObject(FormPtr frmP, UInt16 objIndex,
                              Coord x_diff, Coord y_diff)
{
    Coord   old_x, old_y;

    FrmHideObject(frmP, objIndex);
    FrmGetObjectPosition(frmP, objIndex, &old_x, &old_y);
    if (old_y < 110)
    {
        // ��̕��ɂ���I�u�W�F�N�g�͈ړ������Ȃ�
        // (�����[�x�^����...������ƃJ�b�R���...)
        // x_diff = 0;
        y_diff = 0;
    }
    FrmSetObjectPosition(frmP, objIndex, old_x + x_diff, old_y + y_diff);

    // ���ꂾ�ƁAinvisible�̂��������...�v����
    FrmShowObject(frmP, objIndex);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeObject                                      */
/*                                                 �I�u�W�F�N�g�̃��T�C�Y  */
/*-------------------------------------------------------------------------*/
static void HandEraResizeObject(FormPtr frmP, UInt16 objIndex,
                                Coord x_diff, Coord y_diff)
{
    RectangleType r;
    Coord   old_x, old_y;

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

    FrmShowObject(frmP, objIndex);
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeForm                                        */
/*                                                     �t�H�[���̃��T�C�Y  */
/*-------------------------------------------------------------------------*/
void HandEraResizeForm(FormPtr frmP)
{
    Coord           x, y, xDiff, yDiff;
    RectangleType   r;
    UInt16          objectIndex, numObjects, rowNum, nofRow, height;
    Int16           fldIndex, sclIndex;
    TableType      *tableP;
    ControlType    *ctlPtr;
    FormObjectKind  objType;

    fldIndex = -1;
    sclIndex = -1;

    /* �V�T�C�Y */
    WinGetDisplayExtent(&x, &y);
    
    /* ���T�C�Y */
    FrmGetFormBounds(frmP, &r);

    /* �Ȃ����ȉ��̔��������K�v�i�����s���j */
    r.topLeft.x++;
    r.topLeft.y++;
    r.extent.x -= 2;
    r.extent.y -= 2;

    xDiff = x - (r.topLeft.x + r.extent.x);
    yDiff = y - (r.topLeft.y + r.extent.y);

    if ((xDiff == 0)&&(yDiff == 0))
    {
        return;
    }

    /* ���T�C�Y */
    r.extent.x += xDiff;
    r.extent.y += yDiff;
    WinSetWindowBounds(FrmGetWindowHandle(frmP), &r);
        
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
            // �{�^���̈ړ�
            ctlPtr = (ControlType *)FrmGetObjectPtr(frmP, objectIndex);
#ifdef USE_GLUE
            if ((CtlGlueGetControlStyle(ctlPtr) == buttonCtl)||
                (CtlGlueGetControlStyle(ctlPtr) == checkboxCtl)||
                (CtlGlueGetControlStyle(ctlPtr) == repeatingButtonCtl))
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
          case frmListObj:
            // ���X�g�̃��T�C�Y
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
    return;
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

    NNsi_FrmDrawForm(frmP);

    return;
}

/*=========================================================================*/
/*   Function :   NNshRestoreNNsiSetting                                   */
/*                                                     NNsi�ݒ�̓ǂݍ���  */
/*=========================================================================*/
void NNshRestoreNNsiSetting(Int16 version, UInt16 size, NNshSavedPref *prm)
{
    Err        ret;
    UInt32     offset;
    UInt16     cnt, nofRec;
    DmOpenRef  dbRef;
    Char      *ptr;

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
                NNsh_InformMessage(ALTID_WARN, MSG_NNSISET_READFAIL, "", ret);
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
        prm->lastFrmID        = FRMID_THREAD;
        prm->bufferSize       = NNSH_WORKBUF_DEFAULT;
        prm->partGetSize      = NNSH_WORKBUF_DEFAULT;
        prm->netTimeout       = NNSH_NET_TIMEOUT;
        prm->currentFont      = NNSH_DEFAULT_FONT;
        prm->sonyHRFont       = NNSH_DEFAULT_FONT;
        prm->useCookieWrite   = NNSH_ENABLE;
        prm->disableUnderline = NNSH_ENABLE;
        prm->titleListSize    = LIMIT_TITLENAME_DEFAULT;

        // �W����BBS�ꗗ�擾��(URL)�𔽉f������
        StrCopy(prm->bbs_URL, URL_BBSTABLE);
        NNsh_InformMessage(ALTID_INFO, MSG_NNSISET_REFRESH, "", 0);
    }
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

    MEMFREE_HANDLE(workBuf->searchTitleH);
    MEMFREE_HANDLE(workBuf->threadIdxH);
    MEMFREE_HANDLE(workBuf->boardIdxH);
    MEMFREE_HANDLE(workBuf->logPrefixH);

    if (workBuf->threadTitles != NULL)
    {
        MemHandleUnlock(workBuf->msgTTLStringH);
        workBuf->threadTitles = NULL;
    }
    MEMFREE_HANDLE(workBuf->msgTTLStringH);

    MEMFREE_HANDLE(workBuf->bbsTitleH);
    MEMFREE_HANDLE(workBuf->threadTitleH);

    MEMFREE_PTR(workBuf->bbsTitles);

    if (workBuf->msgIndex != NULL)
    {
        idxP = workBuf->msgIndex;
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(workBuf->msgIndex);
    }

    MEMFREE_PTR(workBuf->tempInfo);

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
    buf = MemPtrNew(BUFSIZE);
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

    // BUSY�\������������
    Hide_BusyForm();
    MemPtrFree(buf);

    if (flag != 0)
    {
        // �o�b�N�A�b�v�G���[�������������Ƃ�ʒm����
        NNsh_InformMessage(ALTID_ERROR, MSG_NNSI_DBBACKUP_FAIL, "", flag);
    }
    NNshGlobal->updateDatabaseInfo = 0;
    return;
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

    NNsh_DebugMessage(ALTID_INFO, MSG_CANNOTFIND_BROWSER, "", ret);
    return;
}

/*=========================================================================*/
/*   Function : KeyConvertFiveWayToJogChara                                */
/*                                  5Way�̓��̓R�[�h���W���O�̃R�[�h�ɕϊ� */
/*=========================================================================*/
UInt16 KeyConvertFiveWayToJogChara(EventType *event)
{
    // Tungsten T 5way navigator�Ή�(281����A���ӁI)
    if (event->data.keyDown.chr == vchrNavChange)
    {
        // 5way PUSH
        if (((event->data.keyDown.keyCode) & (navBitSelect | navChangeSelect))
             == navChangeSelect)
        {
            // �W���O�����Ɠ����ɂ���
            return (vchrJogRelease);
        }

        // LEFT 
        if ((event->data.keyDown.keyCode) & navBitLeft)
        {
            // �W���O������񂵂Ɠ����ɂ���
            return (vchrJogPushedUp);
        }

        // RIGHT
        if ((event->data.keyDown.keyCode) & navBitRight)
        {
            // �W���O�������񂵂Ɠ����ɂ���
            return (vchrJogPushedDown);
        }
    }

    // ��L�ȊO�͂��̂܂܉�������
    return (event->data.keyDown.chr);
}
