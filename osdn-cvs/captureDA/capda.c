//////////////////////////////////////////////////////////////////////////////
//  $Id: capda.c,v 1.1.1.1 2005/01/08 15:53:41 mrsa Exp $
//
// capda.c   --- capD main
//
// (C) NNsi project, 2004, all rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
#include <PalmOS.h>
#include "capda-rsc-defines.h"
#include "capda-defines.h"

///// �v���g�^�C�v�錾�Q /////
Boolean checkDisplayMode(capDADataType *adtP);
Boolean capDA_EventHandler(EventPtr evP);

void CheckClipboard(capDADataType *adtP);

void setDisp_capDA  (FormType *frm, capDADataType *adtP);
void resetDisp_capDA(FormType *frm, capDADataType *adtP);

void   resizeDialogWindow(FormType *diagFrm, capDADataType *adtP);
Boolean createDialogWindow(FormType **diagFrm, capDADataType *adtP);

void displayJPEGdata(FormType *frm, capDADataType *adtP);
Boolean checkVFSdevice(UInt32 aMediaType, capDADataType *adtP);

Boolean checkCaptureDevice  (capDADataType *adtP);
Boolean prepareCaptureDevice(capDADataType *adtP);
void   startCaptureDevice  (capDADataType *adtP);
void   stopCaptureDevice   (capDADataType *adtP);
void   endCaptureDevice    (capDADataType *adtP);
Boolean DoCapture           (capDADataType *adtP);

/*-------------------------------------------------------------------------*/
/*   Function :  checkDisplayMode                                          */
/*                           ��ʃ��[�h(PalmOS5���𑜓x�T�|�[�g)�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
Boolean checkDisplayMode(capDADataType *adtP)
{
    UInt32 version;

    // �𑜓x�̏����f�[�^���i�[����
    adtP->os5density = (UInt32) kDensityLow;

    // ���𑜓x�t�B�[�`���[�̎擾
    if (FtrGet(sysFtrCreator, sysFtrNumWinVersion, &(version)) != 0)
    {
        // ���𑜓x�t�B�[�`���[�̃T�|�[�g�Ȃ�
        return (false);
    }
    if (version < 4)
    {
        //�@���𑜓x�̃T�|�[�g���Ȃ�...
        return (false);
    }

    // Window�̉𑜓x���擾����
    WinScreenGetAttribute(winScreenDensity, &(adtP->os5density));

    // �Ƃ肠�����A�W���𑜓x�ɂ��Ă���
    WinSetCoordinateSystem(kCoordinatesStandard);

    return (true);
}


/*-------------------------------------------------------------------------*/
/*   Function :  resizeDialogWindow                                        */
/*                                        �_�C�A���O�E�B���h�E�T�C�Y�̒��� */
/*-------------------------------------------------------------------------*/
void resizeDialogWindow(FormType *diagFrm, capDADataType *adtP)
{
    UInt16         objIndex;
    Coord          x, y, diffX, diffY;
    RectangleType  r;

    /* �S�̂�Window�T�C�Y�擾 */
    WinGetDisplayExtent(&x, &y);

    /* dialog�̃T�C�Y�擾 */
    FrmGetFormBounds(diagFrm, &r);

    /* �E�B���h�E�̊g��T�C�Y�����߂� */
    r.topLeft.x = 2;
    r.topLeft.y = 3;
    diffX       = r.extent.x;
    diffY       = r.extent.y;
    r.extent.y  = y - 6;
    r.extent.x  = x - 4;
    
    /* �I�u�W�F�N�g�̈ړ��T�C�Y�����߂� */
    diffX = r.extent.x - diffX;
    diffY = r.extent.y - diffY;

    /* dialog�̊g�� */
    WinSetWindowBounds(FrmGetWindowHandle(diagFrm), &r);

    /* OK �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DA_OK);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* CAPTURE �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_CAPTURE);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* About �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DA_ABOUT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* gadget(�`��̈�)�̊g�� */
    objIndex = FrmGetObjectIndex(diagFrm, GADID_DA_VIEW);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    r.extent.x = r.extent.x + diffX;
    r.extent.y = r.extent.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  createDialogWindow                                        */
/*                                 DIA�T�|�[�g�̃`�F�b�N�ƃ_�C�A���O�̐��� */
/*-------------------------------------------------------------------------*/
Boolean createDialogWindow(FormType **diagFrm, capDADataType *adtP)
{
    UInt32 diaVar;
    UInt16 objIndex, result;

    // DIA�T�|�[�g�����邩�ǂ����m�F����
    result = FtrGet(pinCreator, pinFtrAPIVersion, &diaVar);
    if (result != errNone)
    {
        // DIA ��T�|�[�g�f�o�C�X�A�_�C�A���O�T�C�Y�̊g��͂��Ȃ�
        *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
        FrmSetActiveForm(*diagFrm);
        return (false);
    }

    // DIA�T�|�[�g�f�o�C�X �A�V���N�����I�ɓ���\���m�F����
    result = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &diaVar);
    if ((result != errNone)|
        ((diaVar & grfFtrInputAreaFlagDynamic) == 0)||
        ((diaVar & grfFtrInputAreaFlagCollapsible) == 0))
    {
        // �G���[�ɂȂ�ꍇ�A�������͓��I�V���N��T�|�[�g�̏ꍇ�A��������������
        *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
        FrmSetActiveForm(*diagFrm);
        return (false);
    }

    // ���݂̃V���N��Ԃ��L������
    objIndex = PINGetInputAreaState();

    // �_�C�A���O�̃I�[�v��
    *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
    FrmSetActiveForm(*diagFrm);

    // �V���N�����L���ɂ���
    FrmSetDIAPolicyAttr(*diagFrm, frmDIAPolicyCustom);
    PINSetInputTriggerState(pinInputTriggerEnabled);

    // ���炢�x�^�A�A�A�E�B���h�E�T�C�Y�̍ő�l�ŏ��l��ݒ�
    WinSetConstraintsSize(FrmGetWindowHandle(*diagFrm),
                          160, 225, 225, 160, 225, 225);

    // �V���N�̈��ۑ����Ă������̂ɂ���
    PINSetInputAreaState(objIndex);

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :  capDA_EventHandler                                        */
/*                                                   capDA �� event ����   */
/*-------------------------------------------------------------------------*/
Boolean capDA_EventHandler(EventPtr evP)
{
    UInt16         btnId;
    capDADataType *adtP = NULL;

    // global pointer���擾����
    if (FtrGet(capDA_Creator, ADT_FTRID, (UInt32 *) &adtP) != 0)
    {
        // �O���[�o���|�C���^�擾���s
        return (false);
    }

    switch (evP->eType)
    {
      case keyDownEvent:
        switch (evP->data.keyDown.chr)
        {
          case chrCarriageReturn:
          case chrLineFeed:
          case vchrRockerCenter:
          case vchrHardRockerCenter:
          case vchrThumbWheelPush:
          case vchrJogRelease:
          case chrSpace:
          case chrSJISIdeographicSpace:
          case vchrCapture:
          case vchrVoiceRec:
            // �L���v�`��
            btnId = BTNID_CAPTURE;
            break;

          case vchrHard2:
          case vchrHard3:
          case chrCapital_Q:
          case chrSmall_Q:
            // �n�j�����������Ƃɂ���
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                          FrmGetObjectIndex(FrmGetActiveForm(),
                                          BTNID_DA_OK)));
            return (true);
            break;
            
          case chrBackspace:
          case chrEscape:
            // About�\��
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                          FrmGetObjectIndex(FrmGetActiveForm(),
                                          BTNID_DA_ABOUT)));
            return (true);
            break;

          default:
            // �������Ȃ�
            return (false);
            break;
        }
        break;

      case ctlSelectEvent:
        // ���Ƃŏ�������
        btnId = evP->data.ctlSelect.controlID;
        break;
        
      case ctlRepeatEvent:
        // ���Ƃŏ�������
        btnId = evP->data.ctlRepeat.controlID;

      default:
        // �������Ȃ�
        return (false);
        break;
    }

    // �{�^���������ꂽ�Ƃ��̏�������
    switch (btnId)
    {
      case BTNID_DA_ABOUT:
        // About�\��
        FrmAlert(ALERTID_DA_ABOUT);
        break;

      case BTNID_CAPTURE:
        // ��ʃL���v�`�������̎��s
        DoCapture(adtP);
        break;

      case BTNID_DA_OK:
        // �J�����f�o�C�X�̎g�p�I��
        stopCaptureDevice(adtP);
        return (false);
        break;

      default:
        // �������Ȃ�
        return (false);
        break;
    }
    return (true);
}


/*-------------------------------------------------------------------------*/
/*   Function :  setDisp_capDA                                             */
/*                                                        ��ʕ`��̑O���� */
/*-------------------------------------------------------------------------*/
void setDisp_capDA(FormType *frm, capDADataType *adtP)
{
    // draw���Ȃ��ƁAWindow��ɗ̈悪�\������Ȃ��A�A�A
    FrmDrawForm(frm);

    // �K�W�F�b�g�̗̈�T�C�Y���擾����
    FrmGetObjectBounds(frm,FrmGetObjectIndex(frm, GADID_DA_VIEW),
                                                               &(adtP->dimF));

    // �𑜓x���[�h���m�F���A�̈���N���A����(area�Ɏ��𑜓x���i�[)
    switch (adtP->os5density)
    {
      case kDensityDouble:
        // 320x320
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 2;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 2;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2 + 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityTriple:
        // 480x480
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 3;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 3;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 3;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 3 + 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityQuadruple:
        // 640x640
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 4;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 4;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 4;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 4 + 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityOneAndAHalf:
        // 240x240
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2 / 3;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 2 / 3;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 2 / 3;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2 / 3 + 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityLow:
      default:
        // 160x160
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x;
        adtP->area.extent.x   = adtP->dimF.extent.x;
        adtP->area.extent.y   = adtP->dimF.extent.y;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y;
        break;
    }
    // �`��̈�̃N���A
    WinEraseRectangle(&(adtP->area), 0);
}

/*-------------------------------------------------------------------------*/
/*   Function :  resetDisp_capDA                                           */
/*                                                        ��ʕ`��̌㏈�� */
/*-------------------------------------------------------------------------*/
void resetDisp_capDA(FormType *frm, capDADataType *adtP)
{

    // ���𑜓x�̐ݒ������
    switch (adtP->os5density)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()), adtP->os5density);
        WinSetCoordinateSystem(kCoordinatesStandard);
        break;

      case kDensityLow:
      default:
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function :  capDAMain                                                  */
/*                                                        DA�f�[�^�\������ */
/*=========================================================================*/
void capDAMain(void)
{
    UInt16         result,  size;
    FormType      *diagFrm, *prevFrm;
    capDADataType *adtP;
    UInt32         osVersion;

    // ���݂̃t�H�[�����擾
    prevFrm = FrmGetActiveForm();

    // ���L�f�[�^�̊m��
    size = sizeof(capDADataType) + MARGIN;
    adtP = MemPtrNew(size);
    if (adtP == NULL)
    {
        // �̈�m�ێ��s�A�I������
        FrmCustomAlert(ALTID_ERROR,
                       "Memory Allocation ",
                       "failure",
                       ".");
        return;
    }
    MemSet(adtP, size, 0x00);

    // global pointer��feature�Ƃ��ăZ�b�g����
    FtrSet(capDA_Creator, ADT_FTRID, (UInt32) adtP);

    // OS�o�[�W�����̊m�F
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osVersion) == errNone)
    {
        adtP->palmOSVersion = osVersion;
    }

    // �N���b�v�{�[�h�̃R�}���h���m�F����
    CheckClipboard(adtP);

    // �J�����f�o�C�X�����ڂ��ꂽ�@�B���`�F�b�N����
    if (checkCaptureDevice(adtP) == false)
    {
        // �J�����f�o�C�X�������Ă��Ȃ��A�I������
        FrmCustomAlert(ALTID_ERROR,
                       "This device does not have any ",
                       "capture device",
                       ".");
        goto FUNC_END;
    }

    //  ��ʃ��[�h�̃`�F�b�N
    if (checkDisplayMode(adtP) == false)
    {
        // ���𑜓x(OS5)��Ή��@�A�Ȃɂ����Ȃ�
        FrmCustomAlert(ALTID_ERROR,
                       "This device does not support ",
                       "a high-density screen",
                       ".");
        goto FUNC_END;
    }

    // �_�C�A���O�E�B���h�E�̐���
    createDialogWindow(&diagFrm, adtP);

    // �_�C�A���O�E�B���h�E�̃��T�C�Y
    resizeDialogWindow(diagFrm, adtP);

    // Viewer Form�̃C�x���g�n���h���o�^
    FrmSetEventHandler(diagFrm, capDA_EventHandler);

    // ���(�`��̈�)�̃N���A
    setDisp_capDA  (diagFrm, adtP);

    // �J�����f�o�C�X�̎g�p����
    if (prepareCaptureDevice(adtP) != true)
    {
        // �J�����f�o�C�X�����܂��g���Ȃ��|��
        FrmCustomAlert(ALTID_ERROR,
                       "You can not use any capture device...",
                       "",
                       "");

        // �t�H�[���̌�n��
        FrmSetActiveForm(prevFrm);
        FrmDeleteForm(diagFrm);

        // �I������
        goto FUNC_END;
    }

    // �J�����f�o�C�X�̎g�p(preview)�J�n
    startCaptureDevice(adtP);

    // ��ʃ��[�h�����ɖ߂�
    resetDisp_capDA(diagFrm, adtP);

    // Dialog�\��
    result = FrmDoDialog(diagFrm);
    if (result == BTNID_DA_OK)
    {
        // OK�{�^���������ꂽ�A�A�A���ǁA����͕K��OK�Ŕ�����͂�
    }

    // �J�����g�p�㏈��
    endCaptureDevice(adtP);

    // �t�H�[���̌�n��
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // �m�ۂ����������̌�n��
FUNC_END:
    if (adtP->bitmapPP != NULL)
    {
        BmpDelete(adtP->bitmapPP);
    }
    if (adtP->bitmapBuffer != NULL)
    {
        MemPtrFree(adtP->bitmapBuffer);
        adtP->bitmapBuffer = NULL;
    }
    FtrUnregister(capDA_Creator, ADT_FTRID);
    MemPtrFree(adtP);
}

/*-------------------------------------------------------------------------*/
/*   Function :  displayJPEGdata                                           */
/*                                                        JPEG�f�[�^�\��   */
/*-------------------------------------------------------------------------*/
void displayJPEGdata(FormType *frm, capDADataType *adtP)
{
    Err          err;
    UInt16       jpegRef;

    // SONY JPEG���C�u�����̎g�p�ۊm�F
    err = SysLibFind(sonySysLibNameJpegUtil, &jpegRef);
    if (err != errNone)
    {
        err = SysLibLoad(sonySysFileTJpegUtilLib,
                         sonySysFileCJpegUtilLib, &jpegRef);
    }
    if (err != errNone)
    {
        // SONY JPEG���C�u�������g�p�ł��Ȃ�...(�G���[�I������)...
        FrmCustomAlert(ALTID_ERROR, "Could not find ", "JPEG library", ".");
        return;
    }

    // SONY JPEG���C�u�����̃I�[�v��
    err = jpegUtilLibOpen(jpegRef);
    if (err != errNone)
    {
        // SONY JPEG���C�u�����̃I�[�v���Ɏ��s�A�I������
        FrmCustomAlert(ALTID_ERROR,
                       "Could not open",
                       "Sony JPEG library",
                       ".");
        return;
    }

    // �����������ɂ���JPEG�f�[�^����ʂɕ\������
    if (adtP->jpegData != 0)
    {
        (void) jpegUtilLibDecodeImageToWindow(jpegRef,
                                               0,
                                               adtP->jpegData,
                                               jpegDecModeNormal,
                                               &(adtP->area),
                                               NULL);
    }

    // SONY JPEG���C�u�����̃N���[�Y
    jpegUtilLibClose(jpegRef);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkVFSdevice                                           */
/*                 VFS�̃T�|�[�g���m�F(adtP->vfsVol��VFS volume���i�[����) */
/*-------------------------------------------------------------------------*/
Boolean checkVFSdevice(UInt32 aMediaType, capDADataType *adtP)
{
    Err err;

    // VFS�̃T�|�[�g�L�����m�F����B
    adtP->vfsVolumeIterator = 0;
    err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &(adtP->vfsVolumeIterator));
    if (err != errNone)
    {
        adtP->vfsVol = 0;
        return (false);
    }

    //  VFS���g�p�\���m�F����B
    adtP->vfsVolumeIterator = vfsIteratorStart;
    while (adtP->vfsVolumeIterator != vfsIteratorStop)
    {
        err = VFSVolumeEnumerate(&(adtP->vfsVol), &(adtP->vfsVolumeIterator));
        if (err != errNone)
        {
            adtP->vfsVol = 0;
	    return (false);
        }
        err = VFSVolumeInfo(adtP->vfsVol, &(adtP->volInfo));
        if (err != errNone)
        {
            adtP->vfsVol = 0;
            return (false);
        }
        if ((aMediaType == 0)||(aMediaType == adtP->volInfo.mediaType))
        {
            // �t�@�C���V�X�e���̊m�F
            if (adtP->volInfo.fsType == vfsFilesystemType_VFAT)
            {                    
                // VFS�f�o�C�X�𔭌��I
                return (true);                
            }
        }
    }
    // VFS�f�o�C�X��������Ȃ�����...
    adtP->vfsVol = 0;
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkCaptureDevice                                       */
/*                                  �L���v�`���f�o�C�X�̃T�|�[�g���m�F���� */
/*-------------------------------------------------------------------------*/
Boolean checkCaptureDevice(capDADataType *adtP)
{
    Err error;
    UInt16 refNum = 0;
    
    error = SysLibFind(sonySysLibNameCapture, &refNum);
    if (error != errNone)
    {
        error = SysLibLoad(sonySysFileTCaptureLib,
                           sonySysFileCCaptureLib,
                           &refNum);
    }
    if ((error != errNone)||(refNum == 0))
    {
        // SONY Capture���C�u�����̃��[�h�Ɏ��s����...
        adtP->capLibRef = 0;
        return (false);
    }

    // SONY Capture���C�u�����̃��[�h�ɐ���
    adtP->capLibRef = refNum;

    error = CapLibOpen(adtP->capLibRef);
    switch (error)
    {
      case capLibErrNoMemory:
        FrmCustomAlert(ALTID_ERROR,
                       "Insufficient memory.",
                       " ",
                       " ");
        adtP->capLibRef = 0;
        return (false);
        break;

      case capLibErrNotSupported:
        FrmCustomAlert(ALTID_ERROR,
                       "The Capture Library ",
                       "is not supported ",
                       "on this device.");
        adtP->capLibRef = 0;
        return (false);
        break;

      default:
        // OPEN OK!
        break;
    }

    // Capture Library������������
    (void) CapLibInit(adtP->capLibRef);

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   prepareCaptureDevice                                     */
/*                                           �L���v�`���f�o�C�X�̎g�p����  */
/*-------------------------------------------------------------------------*/
Boolean prepareCaptureDevice(capDADataType *adtP)
{
    Err            error;
    CapDevInfoType capDevInfo;
    if (adtP->capLibRef == 0)
    {
        return (false);
    }
    MemSet(&capDevInfo, sizeof(capDevInfo), 0x00);

    error = CapLibDevSelect(adtP->capLibRef, cameraClass, 0, &capDevInfo);
    if (error != errNone)
    {
        return (false);
    }

    adtP->capDevPortId = capDevInfo.devPortID;
    error = CapLibDevOpen(adtP->capLibRef, adtP->capDevPortId);
    if (error != errNone)
    {
        return (false);
    }
    error = CapLibDevPowerOn(adtP->capLibRef, adtP->capDevPortId);
    if (error != errNone)
    {
        (void) CapLibDevClose(adtP->capLibRef, adtP->capDevPortId);
        return (false);
    }
    CapLibSetCaptureSize(adtP->capLibRef, adtP->capDevPortId, w320xh240);
    CapLibSetPreviewSize(adtP->capLibRef, adtP->capDevPortId, w320xh240);

    CapLibSetPreviewArea(adtP->capLibRef, adtP->capDevPortId, &(adtP->area));
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   startCaptureDevice                                       */
/*                                           �L���v�`���f�o�C�X�̎g�p�J�n  */
/*-------------------------------------------------------------------------*/
void startCaptureDevice(capDADataType *adtP)
{
    if (adtP->capLibRef == 0)
    {
        return;
    }
    CapLibPreviewStart(adtP->capLibRef, adtP->capDevPortId);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   stopCaptureDevice                                        */
/*                                           �L���v�`���f�o�C�X�̎g�p�I��  */
/*-------------------------------------------------------------------------*/
void stopCaptureDevice(capDADataType *adtP)
{
    if (adtP->capLibRef == 0)
    {
        return;
    }
    (void) CapLibPreviewStop(adtP->capLibRef, adtP->capDevPortId);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   endCaptureDevice                                         */
/*                                         �L���v�`���f�o�C�X�̎g�p�㏈��  */
/*-------------------------------------------------------------------------*/
void endCaptureDevice(capDADataType *adtP)
{
    // �L���v�`�����C�u�������N���[�Y����
    if (adtP->capLibRef == 0)
    {
        return;
    }
    (void) CapLibDevClose(adtP->capLibRef, adtP->capDevPortId);
    (void) CapLibClose(adtP->capLibRef);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   DoCapture                                                */
/*                                                       �L���v�`���̎��s  */
/*-------------------------------------------------------------------------*/
Boolean DoCapture(capDADataType *adtP)
{
    UInt16  error;
    Err     ret;
    Char    buffer[16];
    if (adtP->capLibRef == 0)
    {
        return (false);
    }

    if (adtP->actionCommand == ACTION_OUTPUTBMP)
    {
        // �r�b�g�}�b�v�p�̗̈���m�ۂ���
        adtP->bitmapPP = BmpCreate(320, 240, 16, NULL, &error);
        if (adtP->bitmapPP == NULL)
        {
            // �������m�ێ��s�A�G���[�I������...
            MemSet (buffer, sizeof(buffer), 0x00);
            StrIToA(buffer, error);
            FrmCustomAlert(ALTID_ERROR,"BmpCreate() ", "ret : 0x", buffer);
            return (false);
        }

        // �C���[�W�̎擾...
#if 0
        ret = CapLibCaptureImage(adtP->capLibRef, 
                                 adtP->capDevPortId, 
                                 (UInt16 *) adtP->bitmapPP);
        if (ret != errNone)
        {
            // �G���[�\������...
            MemSet (buffer, sizeof(buffer), 0x00);
            StrIToH(buffer, ret);
            FrmCustomAlert(ALTID_ERROR,"CapLibCaptureImage() ", "code : 0x",
                           buffer);
        }
#endif
        FrmCustomAlert(ALTID_ERROR,"STAB:::CapLibCaptureImage() ", " ", " ");

        // �I������...
        return (false);
    }

    // VFS���g�p�\�ȏꍇ�B�B�B
    if (checkVFSdevice(0, adtP) == true)
    {
        //////// �f�[�^��JPEG�Ƃ��ċL�^���� ////////
        if ((StrLen(adtP->clipboardInfo) == 0)||(adtP->clipboardInfo[0] != '/'))
        {
            StrCopy(adtP->clipboardInfo, "/Palm/Images/capD.jpg");                
        }

        // JPEG�f�[�^�ۑ������{
        ret = CapLibCaptureJPEGImage(adtP->capLibRef, adtP->capDevPortId, 
                                     adtP->vfsVol, adtP->clipboardInfo,
                                     NULL, 85);
        if (ret != errNone)
        {
            // �G���[�\������...
            MemSet (buffer, sizeof(buffer), 0x00);
            StrIToH(buffer, ret);
            FrmCustomAlert(ALTID_ERROR,
                           adtP->clipboardInfo,
                           "\n CapLibCaptureJPEGImage() code : 0x",
                           buffer);
        }
    }
    else
    {
        // VFS���g�p�ł��Ȃ��|��ʒm����B
        FrmCustomAlert(ALTID_ERROR, "No VFS medium...",
                       " ", "(save failure)");
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   CheckClipboard                                           */
/*                                                   �N���b�v�{�[�h�̊m�F  */
/*-------------------------------------------------------------------------*/
void CheckClipboard(capDADataType *adtP)
{
    MemHandle  memH;
    UInt16     len, loop;
    Char      *areaP, *ptr, *cmdP;

    adtP->actionCommand = ACTION_NOTSPECIFY;

    len  = 0;
    memH = ClipboardGetItem(clipboardText, &len);
    if ((memH == 0)||(len == 0))
    {
        // �N���b�v�{�[�h�Ƀf�[�^���Ȃ��A�I������
        return;
    }
    areaP = MemHandleLock(memH);
    if (areaP == NULL)
    {
        // �N���b�v�{�[�h�̃|�C���^�擾���s�A�I������
        goto FUNC_END;
    }

    ///////////////////// NNsi�Ƃ̘A�g���� ��������
    // if (StrNCompare(areaP, NNSIEXT_VIEWSTART, StrLen(NNSIEXT_VIEWSTART)) == 0)
    if (StrNCompare(areaP, NNSIEXT_INPUTSTART, StrLen(NNSIEXT_INPUTSTART)) != 0)
    {
        // NNsi�Ƃ̘A�g�͂��Ȃ��A�I������
        goto FUNC_END;
    }

    // �f�[�^������̐擪������
    ptr = StrStr(areaP, NNSIEXT_INFONAME);
    if ((ptr == NULL)||(ptr > (areaP + len)))
    {
        // �f�[�^������̐擪�̌��o�Ɏ��s�A�I������
        goto FUNC_END;
    }

    // �f�[�^�̐擪�Ɉړ�����
    ptr = ptr + StrLen(NNSIEXT_INFONAME);


    // �R�}���h�̉��...
    cmdP = StrStr(ptr, capDA_NNSIEXT_OUTJPEG);
    if (cmdP != NULL)
    {
        // JPEG�t�@�C�����o�͂���
        adtP->actionCommand = ACTION_OUTPUTJPEG;
        ptr = cmdP + sizeof(capDA_NNSIEXT_OUTJPEG) - 1;
    }
    else
    {
        cmdP = StrStr(ptr, capDA_NNSIEXT_OUTBMP);
        if (cmdP != NULL)
        {
            // BMP�t�@�C�����o�͂���
            adtP->actionCommand = ACTION_OUTPUTBMP;
            ptr = cmdP + sizeof(capDA_NNSIEXT_OUTBMP) - 1;
        }
    }

    // �t�@�C�����̃R�s�[...
    for (loop = 0; ((loop < len)&&(loop < capDA_BUFFERSIZE)); loop++)
    {
        // ��������R�s�[(���o)����
        adtP->clipboardInfo[loop] = *ptr;
        ptr++;
        if (*ptr == '<')
        {
            // �R�s�[�I���A�A�A
            break;
        }        
    }
    ///////////////////// NNsi�Ƃ̘A�g���� �����܂�

FUNC_END:
    // �N���b�v�{�[�h�̈���J������
    if (memH != 0)
    {
        MemHandleUnlock(memH);
    }
    return;
}
