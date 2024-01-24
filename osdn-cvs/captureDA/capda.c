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

///// プロトタイプ宣言群 /////
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
/*                           画面モード(PalmOS5高解像度サポート)のチェック */
/*-------------------------------------------------------------------------*/
Boolean checkDisplayMode(capDADataType *adtP)
{
    UInt32 version;

    // 解像度の初期データを格納する
    adtP->os5density = (UInt32) kDensityLow;

    // 高解像度フィーチャーの取得
    if (FtrGet(sysFtrCreator, sysFtrNumWinVersion, &(version)) != 0)
    {
        // 高解像度フィーチャーのサポートなし
        return (false);
    }
    if (version < 4)
    {
        //　高解像度のサポートがない...
        return (false);
    }

    // Windowの解像度を取得する
    WinScreenGetAttribute(winScreenDensity, &(adtP->os5density));

    // とりあえず、標準解像度にしておく
    WinSetCoordinateSystem(kCoordinatesStandard);

    return (true);
}


/*-------------------------------------------------------------------------*/
/*   Function :  resizeDialogWindow                                        */
/*                                        ダイアログウィンドウサイズの調整 */
/*-------------------------------------------------------------------------*/
void resizeDialogWindow(FormType *diagFrm, capDADataType *adtP)
{
    UInt16         objIndex;
    Coord          x, y, diffX, diffY;
    RectangleType  r;

    /* 全体のWindowサイズ取得 */
    WinGetDisplayExtent(&x, &y);

    /* dialogのサイズ取得 */
    FrmGetFormBounds(diagFrm, &r);

    /* ウィンドウの拡大サイズを求める */
    r.topLeft.x = 2;
    r.topLeft.y = 3;
    diffX       = r.extent.x;
    diffY       = r.extent.y;
    r.extent.y  = y - 6;
    r.extent.x  = x - 4;
    
    /* オブジェクトの移動サイズを求める */
    diffX = r.extent.x - diffX;
    diffY = r.extent.y - diffY;

    /* dialogの拡大 */
    WinSetWindowBounds(FrmGetWindowHandle(diagFrm), &r);

    /* OK ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DA_OK);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* CAPTURE ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_CAPTURE);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* About ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DA_ABOUT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* gadget(描画領域)の拡大 */
    objIndex = FrmGetObjectIndex(diagFrm, GADID_DA_VIEW);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    r.extent.x = r.extent.x + diffX;
    r.extent.y = r.extent.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  createDialogWindow                                        */
/*                                 DIAサポートのチェックとダイアログの生成 */
/*-------------------------------------------------------------------------*/
Boolean createDialogWindow(FormType **diagFrm, capDADataType *adtP)
{
    UInt32 diaVar;
    UInt16 objIndex, result;

    // DIAサポートがあるかどうか確認する
    result = FtrGet(pinCreator, pinFtrAPIVersion, &diaVar);
    if (result != errNone)
    {
        // DIA 非サポートデバイス、ダイアログサイズの拡大はしない
        *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
        FrmSetActiveForm(*diagFrm);
        return (false);
    }

    // DIAサポートデバイス 、シルクが動的に動作可能か確認する
    result = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &diaVar);
    if ((result != errNone)|
        ((diaVar & grfFtrInputAreaFlagDynamic) == 0)||
        ((diaVar & grfFtrInputAreaFlagCollapsible) == 0))
    {
        // エラーになる場合、もしくは動的シルク非サポートの場合、何もせず抜ける
        *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
        FrmSetActiveForm(*diagFrm);
        return (false);
    }

    // 現在のシルク状態を記憶する
    objIndex = PINGetInputAreaState();

    // ダイアログのオープン
    *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
    FrmSetActiveForm(*diagFrm);

    // シルク制御を有効にする
    FrmSetDIAPolicyAttr(*diagFrm, frmDIAPolicyCustom);
    PINSetInputTriggerState(pinInputTriggerEnabled);

    // えらいベタ、、、ウィンドウサイズの最大値最小値を設定
    WinSetConstraintsSize(FrmGetWindowHandle(*diagFrm),
                          160, 225, 225, 160, 225, 225);

    // シルク領域を保存していたものにする
    PINSetInputAreaState(objIndex);

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :  capDA_EventHandler                                        */
/*                                                   capDA の event 処理   */
/*-------------------------------------------------------------------------*/
Boolean capDA_EventHandler(EventPtr evP)
{
    UInt16         btnId;
    capDADataType *adtP = NULL;

    // global pointerを取得する
    if (FtrGet(capDA_Creator, ADT_FTRID, (UInt32 *) &adtP) != 0)
    {
        // グローバルポインタ取得失敗
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
            // キャプチャ
            btnId = BTNID_CAPTURE;
            break;

          case vchrHard2:
          case vchrHard3:
          case chrCapital_Q:
          case chrSmall_Q:
            // ＯＫを押したことにする
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                          FrmGetObjectIndex(FrmGetActiveForm(),
                                          BTNID_DA_OK)));
            return (true);
            break;
            
          case chrBackspace:
          case chrEscape:
            // About表示
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                          FrmGetObjectIndex(FrmGetActiveForm(),
                                          BTNID_DA_ABOUT)));
            return (true);
            break;

          default:
            // 何もしない
            return (false);
            break;
        }
        break;

      case ctlSelectEvent:
        // あとで処理する
        btnId = evP->data.ctlSelect.controlID;
        break;
        
      case ctlRepeatEvent:
        // あとで処理する
        btnId = evP->data.ctlRepeat.controlID;

      default:
        // 何もしない
        return (false);
        break;
    }

    // ボタンが押されたときの処理分岐
    switch (btnId)
    {
      case BTNID_DA_ABOUT:
        // About表示
        FrmAlert(ALERTID_DA_ABOUT);
        break;

      case BTNID_CAPTURE:
        // 画面キャプチャ処理の実行
        DoCapture(adtP);
        break;

      case BTNID_DA_OK:
        // カメラデバイスの使用終了
        stopCaptureDevice(adtP);
        return (false);
        break;

      default:
        // 何もしない
        return (false);
        break;
    }
    return (true);
}


/*-------------------------------------------------------------------------*/
/*   Function :  setDisp_capDA                                             */
/*                                                        画面描画の前準備 */
/*-------------------------------------------------------------------------*/
void setDisp_capDA(FormType *frm, capDADataType *adtP)
{
    // drawしないと、Window上に領域が表示されない、、、
    FrmDrawForm(frm);

    // ガジェットの領域サイズを取得する
    FrmGetObjectBounds(frm,FrmGetObjectIndex(frm, GADID_DA_VIEW),
                                                               &(adtP->dimF));

    // 解像度モードを確認し、領域をクリアする(areaに実解像度を格納)
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
    // 描画領域のクリア
    WinEraseRectangle(&(adtP->area), 0);
}

/*-------------------------------------------------------------------------*/
/*   Function :  resetDisp_capDA                                           */
/*                                                        画面描画の後処理 */
/*-------------------------------------------------------------------------*/
void resetDisp_capDA(FormType *frm, capDADataType *adtP)
{

    // 高解像度の設定を解除
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
/*                                                        DAデータ表示処理 */
/*=========================================================================*/
void capDAMain(void)
{
    UInt16         result,  size;
    FormType      *diagFrm, *prevFrm;
    capDADataType *adtP;
    UInt32         osVersion;

    // 現在のフォームを取得
    prevFrm = FrmGetActiveForm();

    // 共有データの確保
    size = sizeof(capDADataType) + MARGIN;
    adtP = MemPtrNew(size);
    if (adtP == NULL)
    {
        // 領域確保失敗、終了する
        FrmCustomAlert(ALTID_ERROR,
                       "Memory Allocation ",
                       "failure",
                       ".");
        return;
    }
    MemSet(adtP, size, 0x00);

    // global pointerをfeatureとしてセットする
    FtrSet(capDA_Creator, ADT_FTRID, (UInt32) adtP);

    // OSバージョンの確認
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osVersion) == errNone)
    {
        adtP->palmOSVersion = osVersion;
    }

    // クリップボードのコマンドを確認する
    CheckClipboard(adtP);

    // カメラデバイスが搭載された機械かチェックする
    if (checkCaptureDevice(adtP) == false)
    {
        // カメラデバイスを持っていない、終了する
        FrmCustomAlert(ALTID_ERROR,
                       "This device does not have any ",
                       "capture device",
                       ".");
        goto FUNC_END;
    }

    //  画面モードのチェック
    if (checkDisplayMode(adtP) == false)
    {
        // 高解像度(OS5)非対応機、なにもしない
        FrmCustomAlert(ALTID_ERROR,
                       "This device does not support ",
                       "a high-density screen",
                       ".");
        goto FUNC_END;
    }

    // ダイアログウィンドウの生成
    createDialogWindow(&diagFrm, adtP);

    // ダイアログウィンドウのリサイズ
    resizeDialogWindow(diagFrm, adtP);

    // Viewer Formのイベントハンドラ登録
    FrmSetEventHandler(diagFrm, capDA_EventHandler);

    // 画面(描画領域)のクリア
    setDisp_capDA  (diagFrm, adtP);

    // カメラデバイスの使用準備
    if (prepareCaptureDevice(adtP) != true)
    {
        // カメラデバイスがうまく使えない旨報告
        FrmCustomAlert(ALTID_ERROR,
                       "You can not use any capture device...",
                       "",
                       "");

        // フォームの後始末
        FrmSetActiveForm(prevFrm);
        FrmDeleteForm(diagFrm);

        // 終了する
        goto FUNC_END;
    }

    // カメラデバイスの使用(preview)開始
    startCaptureDevice(adtP);

    // 画面モードを元に戻す
    resetDisp_capDA(diagFrm, adtP);

    // Dialog表示
    result = FrmDoDialog(diagFrm);
    if (result == BTNID_DA_OK)
    {
        // OKボタンが押された、、、けど、今回は必ずOKで抜けるはず
    }

    // カメラ使用後処理
    endCaptureDevice(adtP);

    // フォームの後始末
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // 確保したメモリの後始末
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
/*                                                        JPEGデータ表示   */
/*-------------------------------------------------------------------------*/
void displayJPEGdata(FormType *frm, capDADataType *adtP)
{
    Err          err;
    UInt16       jpegRef;

    // SONY JPEGライブラリの使用可否確認
    err = SysLibFind(sonySysLibNameJpegUtil, &jpegRef);
    if (err != errNone)
    {
        err = SysLibLoad(sonySysFileTJpegUtilLib,
                         sonySysFileCJpegUtilLib, &jpegRef);
    }
    if (err != errNone)
    {
        // SONY JPEGライブラリも使用できない...(エラー終了する)...
        FrmCustomAlert(ALTID_ERROR, "Could not find ", "JPEG library", ".");
        return;
    }

    // SONY JPEGライブラリのオープン
    err = jpegUtilLibOpen(jpegRef);
    if (err != errNone)
    {
        // SONY JPEGライブラリのオープンに失敗、終了する
        FrmCustomAlert(ALTID_ERROR,
                       "Could not open",
                       "Sony JPEG library",
                       ".");
        return;
    }

    // メモリ内部にあるJPEGデータを画面に表示する
    if (adtP->jpegData != 0)
    {
        (void) jpegUtilLibDecodeImageToWindow(jpegRef,
                                               0,
                                               adtP->jpegData,
                                               jpegDecModeNormal,
                                               &(adtP->area),
                                               NULL);
    }

    // SONY JPEGライブラリのクローズ
    jpegUtilLibClose(jpegRef);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkVFSdevice                                           */
/*                 VFSのサポートを確認(adtP->vfsVolにVFS volumeを格納する) */
/*-------------------------------------------------------------------------*/
Boolean checkVFSdevice(UInt32 aMediaType, capDADataType *adtP)
{
    Err err;

    // VFSのサポート有無を確認する。
    adtP->vfsVolumeIterator = 0;
    err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &(adtP->vfsVolumeIterator));
    if (err != errNone)
    {
        adtP->vfsVol = 0;
        return (false);
    }

    //  VFSが使用可能か確認する。
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
            // ファイルシステムの確認
            if (adtP->volInfo.fsType == vfsFilesystemType_VFAT)
            {                    
                // VFSデバイスを発見！
                return (true);                
            }
        }
    }
    // VFSデバイスが見つからなかった...
    adtP->vfsVol = 0;
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkCaptureDevice                                       */
/*                                  キャプチャデバイスのサポートを確認する */
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
        // SONY Captureライブラリのロードに失敗した...
        adtP->capLibRef = 0;
        return (false);
    }

    // SONY Captureライブラリのロードに成功
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

    // Capture Libraryを初期化する
    (void) CapLibInit(adtP->capLibRef);

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   prepareCaptureDevice                                     */
/*                                           キャプチャデバイスの使用準備  */
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
/*                                           キャプチャデバイスの使用開始  */
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
/*                                           キャプチャデバイスの使用終了  */
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
/*                                         キャプチャデバイスの使用後処理  */
/*-------------------------------------------------------------------------*/
void endCaptureDevice(capDADataType *adtP)
{
    // キャプチャライブラリをクローズする
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
/*                                                       キャプチャの実行  */
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
        // ビットマップ用の領域を確保する
        adtP->bitmapPP = BmpCreate(320, 240, 16, NULL, &error);
        if (adtP->bitmapPP == NULL)
        {
            // メモリ確保失敗、エラー終了する...
            MemSet (buffer, sizeof(buffer), 0x00);
            StrIToA(buffer, error);
            FrmCustomAlert(ALTID_ERROR,"BmpCreate() ", "ret : 0x", buffer);
            return (false);
        }

        // イメージの取得...
#if 0
        ret = CapLibCaptureImage(adtP->capLibRef, 
                                 adtP->capDevPortId, 
                                 (UInt16 *) adtP->bitmapPP);
        if (ret != errNone)
        {
            // エラー表示する...
            MemSet (buffer, sizeof(buffer), 0x00);
            StrIToH(buffer, ret);
            FrmCustomAlert(ALTID_ERROR,"CapLibCaptureImage() ", "code : 0x",
                           buffer);
        }
#endif
        FrmCustomAlert(ALTID_ERROR,"STAB:::CapLibCaptureImage() ", " ", " ");

        // 終了する...
        return (false);
    }

    // VFSが使用可能な場合。。。
    if (checkVFSdevice(0, adtP) == true)
    {
        //////// データをJPEGとして記録する ////////
        if ((StrLen(adtP->clipboardInfo) == 0)||(adtP->clipboardInfo[0] != '/'))
        {
            StrCopy(adtP->clipboardInfo, "/Palm/Images/capD.jpg");                
        }

        // JPEGデータ保存を実施
        ret = CapLibCaptureJPEGImage(adtP->capLibRef, adtP->capDevPortId, 
                                     adtP->vfsVol, adtP->clipboardInfo,
                                     NULL, 85);
        if (ret != errNone)
        {
            // エラー表示する...
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
        // VFSが使用できない旨を通知する。
        FrmCustomAlert(ALTID_ERROR, "No VFS medium...",
                       " ", "(save failure)");
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   CheckClipboard                                           */
/*                                                   クリップボードの確認  */
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
        // クリップボードにデータがない、終了する
        return;
    }
    areaP = MemHandleLock(memH);
    if (areaP == NULL)
    {
        // クリップボードのポインタ取得失敗、終了する
        goto FUNC_END;
    }

    ///////////////////// NNsiとの連携処理 ここから
    // if (StrNCompare(areaP, NNSIEXT_VIEWSTART, StrLen(NNSIEXT_VIEWSTART)) == 0)
    if (StrNCompare(areaP, NNSIEXT_INPUTSTART, StrLen(NNSIEXT_INPUTSTART)) != 0)
    {
        // NNsiとの連携はしない、終了する
        goto FUNC_END;
    }

    // データ文字列の先頭を検索
    ptr = StrStr(areaP, NNSIEXT_INFONAME);
    if ((ptr == NULL)||(ptr > (areaP + len)))
    {
        // データ文字列の先頭の検出に失敗、終了する
        goto FUNC_END;
    }

    // データの先頭に移動する
    ptr = ptr + StrLen(NNSIEXT_INFONAME);


    // コマンドの解析...
    cmdP = StrStr(ptr, capDA_NNSIEXT_OUTJPEG);
    if (cmdP != NULL)
    {
        // JPEGファイルを出力する
        adtP->actionCommand = ACTION_OUTPUTJPEG;
        ptr = cmdP + sizeof(capDA_NNSIEXT_OUTJPEG) - 1;
    }
    else
    {
        cmdP = StrStr(ptr, capDA_NNSIEXT_OUTBMP);
        if (cmdP != NULL)
        {
            // BMPファイルを出力する
            adtP->actionCommand = ACTION_OUTPUTBMP;
            ptr = cmdP + sizeof(capDA_NNSIEXT_OUTBMP) - 1;
        }
    }

    // ファイル名のコピー...
    for (loop = 0; ((loop < len)&&(loop < capDA_BUFFERSIZE)); loop++)
    {
        // 文字列をコピー(抽出)する
        adtP->clipboardInfo[loop] = *ptr;
        ptr++;
        if (*ptr == '<')
        {
            // コピー終了、、、
            break;
        }        
    }
    ///////////////////// NNsiとの連携処理 ここまで

FUNC_END:
    // クリップボード領域を開放する
    if (memH != 0)
    {
        MemHandleUnlock(memH);
    }
    return;
}
