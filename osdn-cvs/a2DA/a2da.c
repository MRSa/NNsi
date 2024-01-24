//////////////////////////////////////////////////////////////////////////////
//  $Id: a2da.c,v 1.17 2006/12/19 15:19:47 mrsa Exp $
//
// a2da.c   --- A2DA main
//
// (C) NNsi project, 2003, all rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
#include <PalmOS.h>
#include "a2da-rsc-defines.h"
#include "a2da-defines.h"

void SetDoubleByteCharDB(UInt16 *nofData, A2DAcharFontWidth **contents);
void SetSingleByteCharDB(UInt16 *nofData, A2DAcharFontWidth **contents);

static UInt16 getFontWidth(UInt16 charCode, UInt16 defaultWidth, UInt16 dbCount, A2DAcharFontWidth *data);

static void setA2DA_AAData       (FormType *frm, A2DADataType *adtP);
static void setDisp_A2DA_AAData  (FormType *frm, A2DADataType *adtP);
static void resetDisp_A2DA_AAData(FormType *frm, A2DADataType *adtP);
static void endA2DA_AAData       (FormType *frm, A2DADataType *adtP);

#ifdef USE_CLIE
static void setClieA2DA_AAData       (FormType *frm, A2DADataType *adtP);
static void setClieDisp_A2DA_AAData  (FormType *frm, A2DADataType *adtP);
static void resetClieDisp_A2DA_AAData(FormType *frm, A2DADataType *adtP);
static void endClieA2DA_AAData       (FormType *frm, A2DADataType *adtP);
#endif // #ifdef USE_CLIE

/*-------------------------------------------------------------------------*/
/*   Function :  checkDisplayMode                                          */
/*                           画面モード(PalmOS5高解像度サポート)のチェック */
/*-------------------------------------------------------------------------*/
static void checkDisplayMode(A2DADataType *adtP)
{
    UInt32 version;
#ifdef USE_CLIE
    Err     ret;
    Boolean isColor;
    UInt32  width, height, depth;
#endif

    adtP->os5density = (UInt32) kDensityLow;

    // Yomeru5 を利用しているか？
    if (FtrGet('Ymru', 5000, &version) == 0)
    {
        // Yomeru5を利用中、、、高解像度モードは使用しないことにする
        return;
    }

    // 高解像度フィーチャーの取得
    if (FtrGet(sysFtrCreator, sysFtrNumWinVersion, &(version)) != 0)
    {
        // 高解像度フィーチャーのサポートなし
        return;
    }
    if (version < 4)
    {
        //　高解像度のサポートがない場合の処理
        //  (これで正しいんだろうか...マニュアルにはそう書いてあったが...)
#ifdef USE_CLIE
        // CLIE用高解像度がサポートされているかチェックする    
        // HRLIBの参照番号を取得する
        ret = SysLibFind(sonySysLibNameHR, &(adtP->hrRef));
        if (ret == sysErrLibNotFound)
        {
            // HRLIBが未ロードの場合、ロードできるかTryしてみる。
            ret = SysLibLoad('libr', sonySysFileCHRLib, &(adtP->hrRef));
        }
        if (ret != errNone)
        {
            // HRLIBは使用できない、標準解像度を使う
            adtP->hrRef = 0;
            return;
        }

        // HRLIBの使用宣言とHRLIBのバージョン番号取得
        HROpen(adtP->hrRef);
        HRGetAPIVersion(adtP->hrRef, &(adtP->hrVer));
        if (adtP->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
        {
            // 旧バージョンのハイレゾ...画面モードを確認する...
            ret = HRWinScreenMode(adtP->hrRef, winScreenModeGet, 
                                  &width, &height, &depth, &isColor);
            if ((ret != errNone)||(width != hrWidth))
            {
                // ハイレゾモードではないので、HRLIBは使用しない
                HRClose(adtP->hrRef);
                adtP->hrRef = 0;
                return;
            }
        }
#endif // #ifdef USE_CLIE
        return;
    }

    // Windowの解像度を取得する
    WinScreenGetAttribute(winScreenDensity, &(adtP->os5density));

    // とりあえず、標準解像度にしておく
    WinSetCoordinateSystem(kCoordinatesStandard);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  A2DA_EventHandler                                         */
/*                                                   A2DA の event 処理    */
/*-------------------------------------------------------------------------*/
static Boolean A2DA_EventHandler(EventPtr evP)
{
    UInt16        btnId = 0;
    Boolean       mov;
    A2DADataType *adtP = NULL;
    FormType     *frm;
    ControlType  *obj;
    UInt32 version;

    // global pointerを取得する
    if (FtrGet(A2DA_Creator, ADT_FTRID, (UInt32 *) &adtP) != 0)
    {
        // グローバルポインタ取得失敗
        return (false);
    }

    // シフトインジケータを確認する
    frm = FrmGetActiveForm();
    obj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));
    mov = CtlEnabled(obj);

    // 5way-navを無効にする
    if (FtrGet(sysFtrCreator, sysFtrNumFiveWayNavVersion, &version) == errNone)
    {
        FrmSetNavState(frm, kFrmNavStateFlagsInteractionMode);
    }

    switch (evP->eType)
    {
      case keyDownEvent:
        switch (evP->data.keyDown.chr)
        {
          // JOG PUSH/Zキーを押したときの処理
          case vchrJogRelease:
          case vchrThumbWheelPush:
          case chrCapital_Z:
          case chrSmall_Z:
            // 上下移動モード・左右移動モードの切り替えを実行
            btnId = BTNID_SHIFT;
            break;

          case vchrPageDown:
          case chrDownArrow:
          case vchrJogDown:
          case chrCapital_J:
          case chrSmall_J:
            // 下スクロール
            if (mov == true)
            {
                // 左右移動モード(右スクロール)
                btnId = BTNID_NEXT;
            }
            else
            {
                // 上下移動モード(下スクロール)
                btnId = BTNID_DOWN;
            }
            break;

          case vchrPageUp:
          case chrUpArrow:
          case vchrJogUp:
          case chrCapital_K:
          case chrSmall_K:
            if (mov == true)
            {
                // 左右移動モード(右スクロール)
                btnId = BTNID_PREV;
            }
            else
            {
                // 上下移動モード(上スクロール)
                btnId = BTNID_UP;
            }
            break;

          case vchrJogLeft:
          case chrLeftArrow:
          case vchrJogPushedUp:
          case chrCapital_H:
          case chrSmall_H:
          case vchrRockerLeft:
            // 左スクロール
            btnId = BTNID_PREV;
            break;

          case vchrJogRight:
          case chrRightArrow:
          case vchrJogPushedDown:
          case chrCapital_L:
          case chrSmall_L:
          case vchrRockerRight:
            // 右スクロール
            btnId = BTNID_NEXT;
            break;

          case vchrHard2:
          case vchrHard3:
          case chrCapital_Q:
          case chrSmall_Q:
          case vchrRockerCenter:
            // ＯＫを押したことにする
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                          FrmGetObjectIndex(FrmGetActiveForm(),
                                          BTNID_A2DA_OK)));
            return (true);
            break;
            
          case chrBackspace:
          case chrEscape:
            // About表示
            btnId = BTNID_A2DA_ABOUT;
            break;

          case vchrNavChange:
            // 5way navigator対応
            if ((evP->data.keyDown.keyCode) & navBitLeft)
            {
                // カーソルキー左と等価(左スクロール)
                btnId = BTNID_PREV;
                break;
            }
            if ((evP->data.keyDown.keyCode) & navBitRight)
            {
                // カーソルキー右と等価(右スクロール)
                btnId = BTNID_NEXT;
                break;
            }
            break;
           
          case chrSpace:
          case chrCarriageReturn:
          case chrLineFeed:
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
      case BTNID_A2DA_ABOUT:
        // About表示
        FrmAlert(ALERTID_A2DA_ABOUT);
        break;

      case BTNID_UP:
        // 上移動
        if (adtP->startY > 0)
        {
            adtP->startY = adtP->startY - (adtP->area.extent.y / 2);
        }
        break;      

      case BTNID_DOWN:
        // 下移動
        if (adtP->startY < adtP->maxY)
        {
            adtP->startY = adtP->startY + (adtP->area.extent.y / 2);
        }
        break;

      case BTNID_PREV:
        // 前(左移動)
        if (adtP->startX > 0)
        {
            adtP->startX = adtP->startX - (adtP->area.extent.x / 2);
        }
        break;

      case BTNID_NEXT:
        // 次(右移動)
        if (adtP->startX < adtP->maxX)
        {
            adtP->startX = adtP->startX + (adtP->area.extent.x / 2);
        }
        break;

      case BTNID_SHIFT:
        // シフト状態切り替え
        // ON/OFFが切り替わったら、"!"の表示を切り替える
        if (mov == true)
        {
            // 左右移動モード→上下移動モード(!を消す)
            CtlSetEnabled(obj, false);
            CtlSetUsable (obj, false);
            FrmHideObject(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));                
        }
        else
        {
            // 上下移動モード→左右移動モード(!を表示する)
            CtlSetEnabled(obj, true);
            CtlSetUsable (obj, true);
            FrmShowObject(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));   
        }
        break;

      default:
        // 何もしない
        return (false);
        break;
    }
    // 画面を再描画
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        setClieDisp_A2DA_AAData  (FrmGetActiveForm(), adtP);
        setClieA2DA_AAData       (FrmGetActiveForm(), adtP);
        resetClieDisp_A2DA_AAData(FrmGetActiveForm(), adtP);
    }
    else
#endif
    {
        setDisp_A2DA_AAData  (FrmGetActiveForm(), adtP);
        setA2DA_AAData       (FrmGetActiveForm(), adtP);
        resetDisp_A2DA_AAData(FrmGetActiveForm(), adtP);
    }
    return (true);
}

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :  setClieDisp_A2DA_AAData                                   */
/*                                        画面にAAを貼り付ける処理(CLIE用) */
/*-------------------------------------------------------------------------*/
static void setClieDisp_A2DA_AAData(FormType *frm, A2DADataType *adtP)
{

    // drawしないと、Window上に領域が表示されない、、、
    FrmDrawForm(frm);

    // ガジェットの領域サイズを取得する
    FrmGetObjectBounds(frm,
                       FrmGetObjectIndex(frm, GADID_A2DA_VIEW),
                       &(adtP->dimF));

    // CLIE高解像度用に描画エリアを拡大する
    adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2;
    adtP->area.extent.x   = adtP->dimF.extent.x  * 2;
    adtP->area.extent.y   = adtP->dimF.extent.y  * 2;
    adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2;

    // 描画領域のクリア
    HRWinEraseRectangle(adtP->hrRef, &(adtP->area), 0);

    return;
}
#endif  // #ifdef USE_CLIE


#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :  resetClieDisp_A2DA_AAData                                 */
/*                                画面にAAを貼り付ける処理の後処理(CLIE用) */
/*-------------------------------------------------------------------------*/
static void resetClieDisp_A2DA_AAData(FormType *frm, A2DADataType *adtP)
{

    return;
}
#endif // #ifdef USE_CLIE

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :  endClieA2DA_AAData                                        */
/*                                画面にAAを貼り付ける処理の後処理(CLIE用) */
/*-------------------------------------------------------------------------*/
static void endClieA2DA_AAData(FormType *frm, A2DADataType *adtP)
{

    return;
}
#endif // #ifdef USE_CLIE


/*-------------------------------------------------------------------------*/
/*   Function :  setDisp_A2DA_AAData                                       */
/*                                                画面にAAを貼り付ける処理 */
/*-------------------------------------------------------------------------*/
static void setDisp_A2DA_AAData(FormType *frm, A2DADataType *adtP)
{

    // drawしないと、Window上に領域が表示されない、、、
    FrmDrawForm(frm);

    // ガジェットの領域サイズを取得する
    FrmGetObjectBounds(frm,FrmGetObjectIndex(frm, GADID_A2DA_VIEW),
                                                               &(adtP->dimF));

    // 解像度モードを確認し、領域をクリアする(areaに実解像度を格納)
    switch (adtP->os5density)
    {
      case kDensityDouble:
        // 320x320
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 2;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 2;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityTriple:
        // 480x480
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 3;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 3;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 3;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 3;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityQuadruple:
        // 640x640
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 4;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 4;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 4;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityOneAndAHalf:
        // 240x240
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2 / 3;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 2 / 3;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 2 / 3;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2 / 3;
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

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  resetDisp_A2DA_AAData                                     */
/*                                        画面にAAを貼り付ける処理の後処理 */
/*-------------------------------------------------------------------------*/
static void resetDisp_A2DA_AAData(FormType *frm, A2DADataType *adtP)
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

/*-------------------------------------------------------------------------*/
/*   Function :  endA2DA_AAData                                            */
/*                                        画面にAAを貼り付ける処理の後処理 */
/*-------------------------------------------------------------------------*/
static void endA2DA_AAData(FormType *frm, A2DADataType *adtP)
{
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  setA2DA_AAData                                            */
/*                                                画面にAAを貼り付ける処理 */
/*-------------------------------------------------------------------------*/
static void setA2DA_AAData(FormType *frm, A2DADataType *adtP)
{
    UInt16  letter;
    UInt8  *ptr, upper, lower;
    Coord   x, y, topX;

    ptr  = adtP->areaP;
    topX = 0;
    x    = 0;
    y    = 0;
    while ((UInt8 *) ptr < adtP->areaP + adtP->length)
    {
        // 指定されたコードを表示する
        if ((*ptr == 0x0a)||(*ptr == 0x0d))
        {
            if ((*ptr == 0x0d)&&(*(ptr + 1) == 0x0a))
            {
                // 改行コードがＣＲ＋ＬＦだった場合
                ptr++;
            }

            // 改行コードのとき
            y = y + FntCharHeight();
            adtP->maxY = y;
            x = topX;
            ptr++;
            if (y >= adtP->startY + adtP->area.extent.y)
            {
                // 描画領域幅を超えた、終了する
                break;
            }
            continue;
        }

        // NNsiのカラー制御コードが入っていた？
        if (*ptr == NNSH_CHARCOLOR_ESCAPE)
        {
            // カラー制御コードを読み飛ばす
            ptr++;
            ptr++;
            continue;
        }

        // 漢字 or ANK ?
        if ((*ptr <= 0x80)||((*ptr >= 0xa1)&&(*ptr <= 0xdf)))
        {            
            // シングルバイトの場合
            letter = *ptr;
            ptr++;
            if ((letter >= 0x20)&&(x >= adtP->startX)&&(y >= adtP->startY)&&
                (x < adtP->startX + adtP->area.extent.x)&&
                (y < adtP->startY + adtP->area.extent.y))
            {
                WinDrawChar(letter, 
                            (x - adtP->startX + adtP->area.topLeft.x),
                            (y - adtP->startY + adtP->area.topLeft.y));
            }
            x = x + getFontWidth(letter, 6, adtP->singleDbSize, adtP->singleDB);
            adtP->maxX = x;
        }
        else
        {            
            // ダブルバイトの場合、JISコードに変換してから
            // 上位8ビット/下位8ビットを変数にコピーする
            upper   = *ptr;
            letter  = *ptr << 8;
            ptr++;
            lower   = *ptr;
            letter  = letter + *ptr;
            ptr++;
            if (upper <= 0x9f)
            {
                if (lower < 0x9f)
                {
                    upper = (upper << 1) - 0xe1;
                }
                else
                {
                    upper = (upper << 1) - 0xe0;
                }
            }
            else
            {
                if (lower < 0x9f)
                {
                    upper = ((upper - 0xb0) << 1) - 1;
                }
                else
                {
                    upper = ((upper - 0xb0) << 1);
                }
            }
            if (lower < 0x7f)
            {
                lower = lower - 0x1f;
            }
            else
            {
                if (lower < 0x9f)
                {
                    lower = lower - 0x20;
                }
                else
                {
                    lower = lower - 0x7e;
                }
            }

            if ((x >= adtP->startX)&&(y >= adtP->startY)&&
                (x < adtP->startX + adtP->area.extent.x)&&
                (y < adtP->startY + adtP->area.extent.y))
            {
                WinDrawChar(letter, 
                             (x - adtP->startX + adtP->area.topLeft.x),
                             (y - adtP->startY + adtP->area.topLeft.y));
            }            

            letter = upper << 8;
            letter = lower + letter;
            x = x + getFontWidth(letter, 12, adtP->doubleDbSize, adtP->doubleDB);
            adtP->maxX = x;
        }
    }

    return;
}
#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :  setClieA2DA_AAData                                        */
/*                                        画面にAAを貼り付ける処理(CLIE用) */
/*-------------------------------------------------------------------------*/
static void setClieA2DA_AAData(FormType *frm, A2DADataType *adtP)
{
    UInt16  letter;
    UInt8  *ptr, upper, lower;
    Coord   x, y, topX;

    ptr  = adtP->areaP;
    topX = 0;
    x    = 0;
    y    = 0;
    while ((UInt8 *) ptr < adtP->areaP + adtP->length)
    {
        // 指定されたコードを表示する
        if ((*ptr == 0x0a)||(*ptr == 0x0d))
        {
            if ((*ptr == 0x0d)&&(*(ptr + 1) == 0x0a))
            {
                // 改行コードがＣＲ＋ＬＦだった場合
                ptr++;
            }

            // 改行コードのとき
            y = y + FntCharHeight();
            adtP->maxY = y;
            x = topX;
            ptr++;
            if (y >= adtP->startY + adtP->area.extent.y)
            {
                // 描画領域幅を超えた、終了する
                break;
            }
            continue;
        }

        // NNsiのカラー制御コードが入っていた？
        if (*ptr == NNSH_CHARCOLOR_ESCAPE)
        {
            // カラー制御コードを読み飛ばす
            ptr++;
            ptr++;
            continue;
        }

        // 漢字 or ANK ?
        if ((*ptr <= 0x80)||((*ptr >= 0xa1)&&(*ptr <= 0xdf)))
        {            
            // シングルバイトの場合
            letter = *ptr;
            ptr++;
            if ((letter >= 0x20)&&(x >= adtP->startX)&&(y >= adtP->startY)&&
                (x < adtP->startX + adtP->area.extent.x)&&
                (y < adtP->startY + adtP->area.extent.y))
            {
                HRWinDrawChar(adtP->hrRef,
                              letter, 
                              (x - adtP->startX + adtP->area.topLeft.x),
                              (y - adtP->startY + adtP->area.topLeft.y));
            }
            x = x + getFontWidth(letter, 6, adtP->singleDbSize, adtP->singleDB);
            adtP->maxX = x;
        }
        else
        {            
            // ダブルバイトの場合、JISコードに変換してから
            // 上位8ビット/下位8ビットを変数にコピーする
            upper   = *ptr;
            letter  = *ptr << 8;
            ptr++;
            lower   = *ptr;
            letter  = letter + *ptr;
            ptr++;
            if (upper <= 0x9f)
            {
                if (lower < 0x9f)
                {
                    upper = (upper << 1) - 0xe1;
                }
                else
                {
                    upper = (upper << 1) - 0xe0;
                }
            }
            else
            {
                if (lower < 0x9f)
                {
                    upper = ((upper - 0xb0) << 1) - 1;
                }
                else
                {
                    upper = ((upper - 0xb0) << 1);
                }
            }
            if (lower < 0x7f)
            {
                lower = lower - 0x1f;
            }
            else
            {
                if (lower < 0x9f)
                {
                    lower = lower - 0x20;
                }
                else
                {
                    lower = lower - 0x7e;
                }
            }

            if ((x >= adtP->startX)&&(y >= adtP->startY)&&
                (x < adtP->startX + adtP->area.extent.x)&&
                (y < adtP->startY + adtP->area.extent.y))
            {
                HRWinDrawChar(adtP->hrRef,
                              letter, 
                              (x - adtP->startX + adtP->area.topLeft.x),
                              (y - adtP->startY + adtP->area.topLeft.y));
            }            

            letter = upper << 8;
            letter = lower + letter;
            x = x + getFontWidth(letter, 12, adtP->doubleDbSize, adtP->doubleDB);
            adtP->maxX = x;
        }
    }

    return;
}
#endif // #ifdef USE_CLIE

/*=========================================================================*/
/*   Function :  A2DAMain                                                  */
/*                                                             AA 表示処理 */
/*=========================================================================*/
void A2DAMain(void)
{
    Err           ret;
    FontID        fontId;
    UInt16        result, size, len;
    UInt32        dataSize;
    MemHandle     memH;
    FileHand      filRef;
    Char         *areaP, *ptr, *fileName, *ptr2;
    FormType     *diagFrm, *prevFrm;
    A2DADataType *adtP;

    UInt16        objIndex;
    Coord         x, y, diffX, diffY;
    RectangleType r;

    // 現在のフォームを取得
    prevFrm = FrmGetActiveForm();

    // 共有データの確保
    size = sizeof(A2DADataType) + MARGIN;
    adtP = MemPtrNew(size);
    if (adtP == NULL)
    {
        // 領域確保失敗、終了する
        return;
    }
    MemSet(adtP, size, 0x00);
    ptr2  = NULL;
    areaP = NULL;
    ptr   = NULL;

    // フォントサイズを一時的に変更する
    fontId = FntGetFont();

#if 0
    // Yomeru5 を利用しているか？
    if (FtrGet('Ymru', 5000, &dataSize) == 0)
    {
        // Yomeru5を利用中、、、フォントをスモールにする
        FntSetFont(0x32);
    }
    else
#endif
    {
        FntSetFont(stdFont);
    }

    // global pointerをfeatureとしてセットする
    FtrSet(A2DA_Creator, ADT_FTRID, (UInt32) adtP);

    //  画面モードのチェック
    checkDisplayMode(adtP);


#ifdef USE_DIA
    result = FtrGet(pinCreator, pinFtrAPIVersion, &(dataSize));
    if (result != errNone)
    {
        // DIA 非サポートデバイス、ダイアログサイズの拡大はしない
        // AA Viewer Formの初期化
        diagFrm = FrmInitForm(FRMID_AA_VIEWER);
        FrmSetActiveForm(diagFrm);
        goto RESIZE_WINDOW;
    }

    // DIAサポートデバイス    
    result = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &(dataSize));
    if ((result != errNone)|
        ((dataSize & grfFtrInputAreaFlagDynamic) == 0)||
        ((dataSize & grfFtrInputAreaFlagCollapsible) == 0))
    {
        // エラーになる場合、もしくはDIA非サポートの場合、何もせず抜ける
        // AA Viewer Formの初期化
        diagFrm = FrmInitForm(FRMID_AA_VIEWER);
        FrmSetActiveForm(diagFrm);
        goto RESIZE_WINDOW;
    }
    // 現在のシルク状態を取得する...
    // (この1文をここに書きたいために、 FrmInitForm()がいっぱい出てくる...)
    objIndex = PINGetInputAreaState();

    // AA Viewer Formの初期化
    diagFrm = FrmInitForm(FRMID_AA_VIEWER);
    FrmSetActiveForm(diagFrm);

    // シルク制御を有効にする
    FrmSetDIAPolicyAttr(diagFrm, frmDIAPolicyCustom);
    PINSetInputTriggerState(pinInputTriggerEnabled);

    // えらいベタ、、、ウィンドウサイズの最大値最小値を設定
    WinSetConstraintsSize(FrmGetWindowHandle(diagFrm),
                          160, 225, 225, 160, 225, 225);

    // シルク領域を記憶していたものに変更する...
    PINSetInputAreaState(objIndex);

    result   = 0;
    dataSize = 0;
    objIndex = 0;

RESIZE_WINDOW:
#else   // #ifdef USE_DIA
    // AA Viewer Formの初期化
    diagFrm = FrmInitForm(FRMID_AA_VIEWER);
    FrmSetActiveForm(diagFrm);
#endif  // #ifdef USE_DIA

    /* 全体のWindowサイズ取得 */
    WinGetDisplayExtent(&x, &y);

    /* dialogのサイズ */
    FrmGetFormBounds(diagFrm, &r);

    /* dialogの拡大 */
    r.topLeft.x = 2;
    r.topLeft.y = 4;
    diffX = r.extent.x;
    diffY = r.extent.y;
    r.extent.y = y - 8;
    r.extent.x = x - 4;
    
    diffX = r.extent.x - diffX;
    diffY = r.extent.y - diffY;
    WinSetWindowBounds(FrmGetWindowHandle(diagFrm), &r);

    /* OK ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_A2DA_OK);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* 上 ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_UP);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* 左 ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_PREV);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* 右 ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_NEXT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* 下 ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DOWN);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* SHIFT ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_SHIFT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    // 初期値は、上下移動モードに設定
    CtlSetEnabled(FrmGetObjectPtr(diagFrm, objIndex), false);
    CtlSetUsable (FrmGetObjectPtr(diagFrm, objIndex), false);
    FrmHideObject(diagFrm, objIndex);                

    /* About ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_A2DA_ABOUT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* gadgetの拡大 */
    objIndex = FrmGetObjectIndex(diagFrm, GADID_A2DA_VIEW);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    r.extent.x = r.extent.x + diffX;
    r.extent.y = r.extent.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    // AA Viewer Formのイベントハンドラ登録
    FrmSetEventHandler(diagFrm, A2DA_EventHandler);

    ////////////////////////////////////////////////////////////
    //  AAデータの画面設定処理
    //   (クリップボードにあるデータを取得後表示する)
    memH = ClipboardGetItem(clipboardText, &len);
    if ((len != 0)&&(memH != 0))
    {
        areaP = MemHandleLock(memH);
        if (areaP != NULL)
        {
            // フォント表示幅ＤＢを構築する(モナーフォントのフォント幅を参考に...)
            SetSingleByteCharDB(&(adtP->singleDbSize), &(adtP->singleDB));
            SetDoubleByteCharDB(&(adtP->doubleDbSize), &(adtP->doubleDB));

            /////////////////////  (えらいいい加減な) NNsiとの連携処理 ここから
            if (StrNCompare(areaP, A2DA_NNSIEXT_AADATASTART, StrLen(A2DA_NNSIEXT_AADATASTART)) == 0)
            {
                if (StrStr(areaP, A2DA_NNSIEXT_NNSICREATOR) != NULL)
                {
                    ptr = StrStr(areaP, A2DA_NNSIEXT_DATANAME);
                    if (ptr != NULL)
                    {
                        // データからファイル名を切り出す
                        ptr = ptr + StrLen(A2DA_NNSIEXT_DATANAME);
                        fileName = MemPtrNew(BUFSIZE);
                        if (fileName != NULL)
                        {
                            MemSet(fileName, BUFSIZE, 0x00);
                            ptr2 = fileName;
                            while ((*ptr != '<')&&(ptr < areaP + len))
                            {
                                *ptr2 = *ptr;
                                ptr++;
                                ptr2++;
                            }

                            // ファイルをオープンし、データをコピーする
                            filRef = FileOpen(0, fileName, 0, 'NNsi', fileModeReadOnly, &ret);
                            // ファイル名領域を解放する
                            if (fileName != NULL)
                            {
                                MemPtrFree(fileName);
                            }
                            ptr2 = NULL;
                            if (filRef != 0)
                            {
                                if (FileTell(filRef, &dataSize, &ret) != -1)
                                {
                                    ptr2 = MemPtrNew(dataSize + MARGIN);
                                    if (ptr2 != NULL)
                                    {
                                        MemSet(ptr2, (dataSize + MARGIN), 0x00);
                                        if (FileRead(filRef, ptr2, 1, dataSize, &ret) > 0)
                                        {
                                            areaP = ptr2;
                                            len   = dataSize;                                
                                        }
                                    }
                                }
                                FileClose(filRef);
                            }          
                        }                        
                    }
                }
            }
            /////////////////////  (えらいいい加減な) NNsiとの連携処理 ここまで

            // 描画データのポインタを記憶する
            adtP->areaP  = areaP;
            adtP->length = len;

#ifdef USE_CLIE
            if (adtP->hrRef != 0)
            {
                setClieDisp_A2DA_AAData  (diagFrm, adtP);  //  画面表示準備処理
                setClieA2DA_AAData       (diagFrm, adtP);  //  データ表示実処理
                resetClieDisp_A2DA_AAData(diagFrm, adtP);  //  画面表示後処理
            }
            else
#endif  // #ifdef USE_CLIE
            {
                setDisp_A2DA_AAData  (diagFrm, adtP);      //  画面表示準備処理
                setA2DA_AAData       (diagFrm, adtP);      //  データ表示実処理
                resetDisp_A2DA_AAData(diagFrm, adtP);      //  画面表示後処理
            }
        }
    }
    ////////////////////////////////////////////////////////////

    // Dialog表示
    result = FrmDoDialog(diagFrm);
    if (result == BTNID_A2DA_OK)
    {
        // OKボタンが押された、、、けど、今回は必ずOKで抜けるはず
    }
    
    ////////////////////////////////////////////////////////////
    //  AAデータの画面設定の後処理
    //
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        endClieA2DA_AAData(diagFrm, adtP);
    }
    else
#endif  // #ifdef USE_CLIE
    {
        endA2DA_AAData(diagFrm, adtP);
    }
    //
    ////////////////////////////////////////////////////////////

    // フォントサイズを元に戻す
    FntSetFont(fontId);            

    // クリップボード領域を開放する
    if (memH != 0)
    {
        MemHandleUnlock(memH);
    }

    // テンポラリデータバッファを解放する
    if (ptr2 != NULL)
    {
        MemPtrFree(ptr2);
    }
    // ANKコード幅ＤＢを解放する
    if (adtP->singleDB != NULL)
    {
        MemPtrFree(adtP->singleDB);
    }
    // 漢字コード幅ＤＢを解放する
    if (adtP->doubleDB != NULL)
    {
        MemPtrFree(adtP->doubleDB);
    }

    // フォームとglobal pointerの後始末
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    FtrUnregister(A2DA_Creator, ADT_FTRID);
    MemPtrFree(adtP);
}

/*-------------------------------------------------------------------------*/
/*   Function :  SetSingleByteCharDB                                       */
/*                                            ANK フォントの幅データを設定 */
/*-------------------------------------------------------------------------*/
void SetSingleByteCharDB(UInt16 *nofData, A2DAcharFontWidth **contents)
{
    UInt16             allocSize;
    A2DAcharFontWidth *data;

    allocSize = sizeof(A2DAcharFontWidth) * (170 + 1) + MARGIN;
    *contents = MemPtrNew(allocSize);
    if (*contents == NULL)
    {
        *nofData = 0;
    }
    MemSet(*contents, allocSize, 0x00);
    *nofData = 160;

    data = *contents;

    data->charCode=32;
    data->fontWidth=4;
    data++;

    data->charCode=33;
    data->fontWidth=3;
    data++;

    data->charCode=34;
    data->fontWidth=6;
    data++;

    data->charCode=35;
    data->fontWidth=6;
    data++;

    data->charCode=36;
    data->fontWidth=6;
    data++;

    data->charCode=37;
    data->fontWidth=6;
    data++;

    data->charCode=38;
    data->fontWidth=7;
    data++;

    data->charCode=39;
    data->fontWidth=2;
    data++;

    data->charCode=40;
    data->fontWidth=4;
    data++;

    data->charCode=41;
    data->fontWidth=4;
    data++;

    data->charCode=42;
    data->fontWidth=6;
    data++;

    data->charCode=43;
    data->fontWidth=6;
    data++;

    data->charCode=44;
    data->fontWidth=2;
    data++;

    data->charCode=45;
    data->fontWidth=6;
    data++;

    data->charCode=46;
    data->fontWidth=2;
    data++;

    data->charCode=47;
    data->fontWidth=6;
    data++;

    data->charCode=48;
    data->fontWidth=6;
    data++;

    data->charCode=49;
    data->fontWidth=6;
    data++;

    data->charCode=50;
    data->fontWidth=6;
    data++;

    data->charCode=51;
    data->fontWidth=6;
    data++;

    data->charCode=52;
    data->fontWidth=6;
    data++;

    data->charCode=53;
    data->fontWidth=6;
    data++;

    data->charCode=54;
    data->fontWidth=6;
    data++;

    data->charCode=55;
    data->fontWidth=6;
    data++;

    data->charCode=56;
    data->fontWidth=6;
    data++;

    data->charCode=57;
    data->fontWidth=6;
    data++;

    data->charCode=58;
    data->fontWidth=2;
    data++;

    data->charCode=59;
    data->fontWidth=2;
    data++;

    data->charCode=60;
    data->fontWidth=6;
    data++;

    data->charCode=61;
    data->fontWidth=6;
    data++;

    data->charCode=62;
    data->fontWidth=6;
    data++;

    data->charCode=63;
    data->fontWidth=5;
    data++;

    data->charCode=64;
    data->fontWidth=8;
    data++;

    data->charCode=65;
    data->fontWidth=8;
    data++;

    data->charCode=66;
    data->fontWidth=8;
    data++;

    data->charCode=67;
    data->fontWidth=8;
    data++;

    data->charCode=68;
    data->fontWidth=8;
    data++;

    data->charCode=69;
    data->fontWidth=7;
    data++;

    data->charCode=70;
    data->fontWidth=7;
    data++;

    data->charCode=71;
    data->fontWidth=8;
    data++;

    data->charCode=72;
    data->fontWidth=8;
    data++;

    data->charCode=73;
    data->fontWidth=3;
    data++;

    data->charCode=74;
    data->fontWidth=7;
    data++;

    data->charCode=75;
    data->fontWidth=7;
    data++;

    data->charCode=76;
    data->fontWidth=6;
    data++;

    data->charCode=77;
    data->fontWidth=9;
    data++;

    data->charCode=78;
    data->fontWidth=8;
    data++;

    data->charCode=79;
    data->fontWidth=8;
    data++;

    data->charCode=80;
    data->fontWidth=7;
    data++;

    data->charCode=81;
    data->fontWidth=8;
    data++;

    data->charCode=82;
    data->fontWidth=8;
    data++;

    data->charCode=83;
    data->fontWidth=7;
    data++;

    data->charCode=84;
    data->fontWidth=7;
    data++;

    data->charCode=85;
    data->fontWidth=8;
    data++;

    data->charCode=86;
    data->fontWidth=8;
    data++;

    data->charCode=87;
    data->fontWidth=9;
    data++;

    data->charCode=88;
    data->fontWidth=7;
    data++;

    data->charCode=89;
    data->fontWidth=7;
    data++;

    data->charCode=90;
    data->fontWidth=7;
    data++;

    data->charCode=91;
    data->fontWidth=4;
    data++;

    data->charCode=92;
    data->fontWidth=6;
    data++;

    data->charCode=93;
    data->fontWidth=4;
    data++;

    data->charCode=94;
    data->fontWidth=5;
    data++;

    data->charCode=95;
    data->fontWidth=4;
    data++;

    data->charCode=96;
    data->fontWidth=5;
    data++;

    data->charCode=97;
    data->fontWidth=6;
    data++;

    data->charCode=98;
    data->fontWidth=6;
    data++;

    data->charCode=99;
    data->fontWidth=6;
    data++;

    data->charCode=100;
    data->fontWidth=6;
    data++;

    data->charCode=101;
    data->fontWidth=6;
    data++;

    data->charCode=102;
    data->fontWidth=4;
    data++;

    data->charCode=103;
    data->fontWidth=6;
    data++;

    data->charCode=104;
    data->fontWidth=6;
    data++;

    data->charCode=105;
    data->fontWidth=3;
    data++;

    data->charCode=106;
    data->fontWidth=3;
    data++;

    data->charCode=107;
    data->fontWidth=6;
    data++;

    data->charCode=108;
    data->fontWidth=3;
    data++;

    data->charCode=109;
    data->fontWidth=9;
    data++;

    data->charCode=110;
    data->fontWidth=6;
    data++;

    data->charCode=111;
    data->fontWidth=6;
    data++;

    data->charCode=112;
    data->fontWidth=6;
    data++;

    data->charCode=113;
    data->fontWidth=6;
    data++;

    data->charCode=114;
    data->fontWidth=4;
    data++;

    data->charCode=115;
    data->fontWidth=6;
    data++;

    data->charCode=116;
    data->fontWidth=4;
    data++;

    data->charCode=117;
    data->fontWidth=6;
    data++;

    data->charCode=118;
    data->fontWidth=6;
    data++;

    data->charCode=119;
    data->fontWidth=8;
    data++;

    data->charCode=120;
    data->fontWidth=6;
    data++;

    data->charCode=121;
    data->fontWidth=6;
    data++;

    data->charCode=122;
    data->fontWidth=5;
    data++;

    data->charCode=123;
    data->fontWidth=3;
    data++;

    data->charCode=124;
    data->fontWidth=3;
    data++;

    data->charCode=125;
    data->fontWidth=3;
    data++;

    data->charCode=126;
    data->fontWidth=5;
    data++;

    data->charCode=127;
    data->fontWidth=5;
    data++;

    data->charCode=160;
    data->fontWidth=5;
    data++;

    data->charCode=161;
    data->fontWidth=5;
    data++;

    data->charCode=162;
    data->fontWidth=5;
    data++;

    data->charCode=163;
    data->fontWidth=5;
    data++;

    data->charCode=164;
    data->fontWidth=5;
    data++;

    data->charCode=165;
    data->fontWidth=5;
    data++;

    data->charCode=166;
    data->fontWidth=7;
    data++;

    data->charCode=167;
    data->fontWidth=6;
    data++;

    data->charCode=168;
    data->fontWidth=5;
    data++;

    data->charCode=169;
    data->fontWidth=6;
    data++;

    data->charCode=170;
    data->fontWidth=6;
    data++;

    data->charCode=171;
    data->fontWidth=6;
    data++;

    data->charCode=172;
    data->fontWidth=6;
    data++;

    data->charCode=173;
    data->fontWidth=6;
    data++;

    data->charCode=174;
    data->fontWidth=5;
    data++;

    data->charCode=175;
    data->fontWidth=6;
    data++;

    data->charCode=176;
    data->fontWidth=8;
    data++;

    data->charCode=177;
    data->fontWidth=7;
    data++;

    data->charCode=178;
    data->fontWidth=7;
    data++;

    data->charCode=179;
    data->fontWidth=8;
    data++;

    data->charCode=180;
    data->fontWidth=7;
    data++;

    data->charCode=181;
    data->fontWidth=8;
    data++;

    data->charCode=182;
    data->fontWidth=7;
    data++;

    data->charCode=183;
    data->fontWidth=8;
    data++;

    data->charCode=184;
    data->fontWidth=6;
    data++;

    data->charCode=185;
    data->fontWidth=8;
    data++;

    data->charCode=186;
    data->fontWidth=6;
    data++;

    data->charCode=187;
    data->fontWidth=8;
    data++;

    data->charCode=188;
    data->fontWidth=7;
    data++;

    data->charCode=189;
    data->fontWidth=8;
    data++;

    data->charCode=190;
    data->fontWidth=8;
    data++;

    data->charCode=191;
    data->fontWidth=7;
    data++;

    data->charCode=192;
    data->fontWidth=6;
    data++;

    data->charCode=193;
    data->fontWidth=7;
    data++;

    data->charCode=194;
    data->fontWidth=7;
    data++;

    data->charCode=195;
    data->fontWidth=7;
    data++;

    data->charCode=196;
    data->fontWidth=6;
    data++;

    data->charCode=197;
    data->fontWidth=8;
    data++;

    data->charCode=198;
    data->fontWidth=7;
    data++;

    data->charCode=199;
    data->fontWidth=7;
    data++;

    data->charCode=200;
    data->fontWidth=8;
    data++;

    data->charCode=201;
    data->fontWidth=6;
    data++;

    data->charCode=202;
    data->fontWidth=8;
    data++;

    data->charCode=203;
    data->fontWidth=6;
    data++;

    data->charCode=204;
    data->fontWidth=7;
    data++;

    data->charCode=205;
    data->fontWidth=7;
    data++;

    data->charCode=206;
    data->fontWidth=8;
    data++;

    data->charCode=207;
    data->fontWidth=7;
    data++;

    data->charCode=208;
    data->fontWidth=5;
    data++;

    data->charCode=209;
    data->fontWidth=8;
    data++;

    data->charCode=210;
    data->fontWidth=6;
    data++;

    data->charCode=211;
    data->fontWidth=7;
    data++;

    data->charCode=212;
    data->fontWidth=8;
    data++;

    data->charCode=213;
    data->fontWidth=8;
    data++;

    data->charCode=214;
    data->fontWidth=6;
    data++;

    data->charCode=215;
    data->fontWidth=7;
    data++;

    data->charCode=216;
    data->fontWidth=6;
    data++;

    data->charCode=217;
    data->fontWidth=8;
    data++;

    data->charCode=218;
    data->fontWidth=7;
    data++;

    data->charCode=219;
    data->fontWidth=7;
    data++;

    data->charCode=220;
    data->fontWidth=7;
    data++;

    data->charCode=221;
    data->fontWidth=7;
    data++;

    data->charCode=222;
    data->fontWidth=3;
    data++;

    data->charCode=223;
    data->fontWidth=3;
    data++;

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  SetDoubleByteCharDB                                       */
/*                                            漢字フォントの幅データを設定 */
/*-------------------------------------------------------------------------*/
void SetDoubleByteCharDB(UInt16 *nofData, A2DAcharFontWidth **contents)
{
    UInt16             allocSize;
    A2DAcharFontWidth *data;

    allocSize = sizeof(A2DAcharFontWidth) * (570 + 1) + MARGIN;
    *contents = MemPtrNew(allocSize);
    if (*contents == NULL)
    {
        *nofData = 0;
    }
    MemSet(*contents, allocSize, 0x00);
    *nofData = 566;  // + 8480

    data = *contents;

    data->charCode=0;
    data->fontWidth=12;
    data++;

    data->charCode=8481;
    data->fontWidth=8;
    data++;

    data->charCode=8482;
    data->fontWidth=8;
    data++;

    data->charCode=8483;
    data->fontWidth=8;
    data++;

    data->charCode=8484;
    data->fontWidth=8;
    data++;

    data->charCode=8485;
    data->fontWidth=8;
    data++;

    data->charCode=8486;
    data->fontWidth=6;
    data++;

    data->charCode=8487;
    data->fontWidth=6;
    data++;

    data->charCode=8488;
    data->fontWidth=6;
    data++;

    data->charCode=8489;
    data->fontWidth=12;
    data++;

    data->charCode=8490;
    data->fontWidth=12;
    data++;

    data->charCode=8491;
    data->fontWidth=6;
    data++;

    data->charCode=8492;
    data->fontWidth=6;
    data++;

    data->charCode=8493;
    data->fontWidth=6;
    data++;

    data->charCode=8494;
    data->fontWidth=6;
    data++;

    data->charCode=8495;
    data->fontWidth=6;
    data++;

    data->charCode=8496;
    data->fontWidth=6;
    data++;

    data->charCode=8497;
    data->fontWidth=12;
    data++;

    data->charCode=8498;
    data->fontWidth=12;
    data++;

    data->charCode=8499;
    data->fontWidth=9;
    data++;

    data->charCode=8500;
    data->fontWidth=9;
    data++;

    data->charCode=8501;
    data->fontWidth=9;
    data++;

    data->charCode=8502;
    data->fontWidth=8;
    data++;

    data->charCode=8503;
    data->fontWidth=12;
    data++;

    data->charCode=8504;
    data->fontWidth=12;
    data++;

    data->charCode=8505;
    data->fontWidth=12;
    data++;

    data->charCode=8506;
    data->fontWidth=12;
    data++;

    data->charCode=8507;
    data->fontWidth=12;
    data++;

    data->charCode=8508;
    data->fontWidth=12;
    data++;

    data->charCode=8509;
    data->fontWidth=12;
    data++;

    data->charCode=8510;
    data->fontWidth=6;
    data++;

    data->charCode=8511;
    data->fontWidth=12;
    data++;

    data->charCode=8512;
    data->fontWidth=12;
    data++;

    data->charCode=8513;
    data->fontWidth=12;
    data++;

    data->charCode=8514;
    data->fontWidth=12;
    data++;

    data->charCode=8515;
    data->fontWidth=12;
    data++;

    data->charCode=8516;
    data->fontWidth=12;
    data++;

    data->charCode=8517;
    data->fontWidth=12;
    data++;

    data->charCode=8518;
    data->fontWidth=6;
    data++;

    data->charCode=8519;
    data->fontWidth=6;
    data++;

    data->charCode=8520;
    data->fontWidth=6;
    data++;

    data->charCode=8521;
    data->fontWidth=6;
    data++;

    data->charCode=8522;
    data->fontWidth=6;
    data++;

    data->charCode=8523;
    data->fontWidth=6;
    data++;

    data->charCode=8524;
    data->fontWidth=6;
    data++;

    data->charCode=8525;
    data->fontWidth=6;
    data++;

    data->charCode=8526;
    data->fontWidth=6;
    data++;

    data->charCode=8527;
    data->fontWidth=6;
    data++;

    data->charCode=8528;
    data->fontWidth=6;
    data++;

    data->charCode=8529;
    data->fontWidth=6;
    data++;

    data->charCode=8530;
    data->fontWidth=6;
    data++;

    data->charCode=8531;
    data->fontWidth=6;
    data++;

    data->charCode=8532;
    data->fontWidth=6;
    data++;

    data->charCode=8533;
    data->fontWidth=6;
    data++;

    data->charCode=8534;
    data->fontWidth=6;
    data++;

    data->charCode=8535;
    data->fontWidth=6;
    data++;

    data->charCode=8536;
    data->fontWidth=6;
    data++;

    data->charCode=8537;
    data->fontWidth=6;
    data++;

    data->charCode=8538;
    data->fontWidth=6;
    data++;

    data->charCode=8539;
    data->fontWidth=6;
    data++;

    data->charCode=8540;
    data->fontWidth=12;
    data++;

    data->charCode=8541;
    data->fontWidth=12;
    data++;

    data->charCode=8542;
    data->fontWidth=12;
    data++;

    data->charCode=8543;
    data->fontWidth=12;
    data++;

    data->charCode=8544;
    data->fontWidth=12;
    data++;

    data->charCode=8545;
    data->fontWidth=12;
    data++;

    data->charCode=8546;
    data->fontWidth=12;
    data++;

    data->charCode=8547;
    data->fontWidth=12;
    data++;

    data->charCode=8548;
    data->fontWidth=12;
    data++;

    data->charCode=8549;
    data->fontWidth=12;
    data++;

    data->charCode=8550;
    data->fontWidth=12;
    data++;

    data->charCode=8551;
    data->fontWidth=12;
    data++;

    data->charCode=8552;
    data->fontWidth=12;
    data++;

    data->charCode=8553;
    data->fontWidth=12;
    data++;

    data->charCode=8554;
    data->fontWidth=12;
    data++;

    data->charCode=8555;
    data->fontWidth=12;
    data++;

    data->charCode=8556;
    data->fontWidth=12;
    data++;

    data->charCode=8557;
    data->fontWidth=12;
    data++;

    data->charCode=8558;
    data->fontWidth=12;
    data++;

    data->charCode=8559;
    data->fontWidth=12;
    data++;

    data->charCode=8560;
    data->fontWidth=12;
    data++;

    data->charCode=8561;
    data->fontWidth=12;
    data++;

    data->charCode=8562;
    data->fontWidth=12;
    data++;

    data->charCode=8563;
    data->fontWidth=12;
    data++;

    data->charCode=8564;
    data->fontWidth=12;
    data++;

    data->charCode=8565;
    data->fontWidth=12;
    data++;

    data->charCode=8566;
    data->fontWidth=12;
    data++;

    data->charCode=8567;
    data->fontWidth=12;
    data++;

    data->charCode=8568;
    data->fontWidth=12;
    data++;

    data->charCode=8569;
    data->fontWidth=12;
    data++;

    data->charCode=8570;
    data->fontWidth=12;
    data++;

    data->charCode=8571;
    data->fontWidth=12;
    data++;

    data->charCode=8572;
    data->fontWidth=12;
    data++;

    data->charCode=8573;
    data->fontWidth=12;
    data++;

    data->charCode=8574;
    data->fontWidth=12;
    data++;

    data->charCode=8737;
    data->fontWidth=12;
    data++;

    data->charCode=8738;
    data->fontWidth=12;
    data++;

    data->charCode=8739;
    data->fontWidth=12;
    data++;

    data->charCode=8740;
    data->fontWidth=12;
    data++;

    data->charCode=8741;
    data->fontWidth=12;
    data++;

    data->charCode=8742;
    data->fontWidth=12;
    data++;

    data->charCode=8743;
    data->fontWidth=12;
    data++;

    data->charCode=8744;
    data->fontWidth=12;
    data++;

    data->charCode=8745;
    data->fontWidth=12;
    data++;

    data->charCode=8746;
    data->fontWidth=12;
    data++;

    data->charCode=8747;
    data->fontWidth=12;
    data++;

    data->charCode=8748;
    data->fontWidth=12;
    data++;

    data->charCode=8749;
    data->fontWidth=12;
    data++;

    data->charCode=8750;
    data->fontWidth=12;
    data++;

    data->charCode=8751;
    data->fontWidth=12;
    data++;

    data->charCode=8752;
    data->fontWidth=12;
    data++;

    data->charCode=8753;
    data->fontWidth=12;
    data++;

    data->charCode=8754;
    data->fontWidth=12;
    data++;

    data->charCode=8755;
    data->fontWidth=12;
    data++;

    data->charCode=8756;
    data->fontWidth=12;
    data++;

    data->charCode=8757;
    data->fontWidth=12;
    data++;

    data->charCode=8758;
    data->fontWidth=12;
    data++;

    data->charCode=8759;
    data->fontWidth=12;
    data++;

    data->charCode=8760;
    data->fontWidth=12;
    data++;

    data->charCode=8761;
    data->fontWidth=12;
    data++;

    data->charCode=8762;
    data->fontWidth=12;
    data++;

    data->charCode=8763;
    data->fontWidth=12;
    data++;

    data->charCode=8764;
    data->fontWidth=12;
    data++;

    data->charCode=8765;
    data->fontWidth=12;
    data++;

    data->charCode=8766;
    data->fontWidth=12;
    data++;

    data->charCode=8767;
    data->fontWidth=12;
    data++;

    data->charCode=8768;
    data->fontWidth=12;
    data++;

    data->charCode=8769;
    data->fontWidth=12;
    data++;

    data->charCode=8770;
    data->fontWidth=12;
    data++;

    data->charCode=8771;
    data->fontWidth=12;
    data++;

    data->charCode=8772;
    data->fontWidth=12;
    data++;

    data->charCode=8773;
    data->fontWidth=12;
    data++;

    data->charCode=8774;
    data->fontWidth=12;
    data++;

    data->charCode=8775;
    data->fontWidth=12;
    data++;

    data->charCode=8776;
    data->fontWidth=12;
    data++;

    data->charCode=8777;
    data->fontWidth=12;
    data++;

    data->charCode=8778;
    data->fontWidth=12;
    data++;

    data->charCode=8779;
    data->fontWidth=12;
    data++;

    data->charCode=8780;
    data->fontWidth=12;
    data++;

    data->charCode=8781;
    data->fontWidth=12;
    data++;

    data->charCode=8782;
    data->fontWidth=12;
    data++;

    data->charCode=8783;
    data->fontWidth=12;
    data++;

    data->charCode=8784;
    data->fontWidth=12;
    data++;

    data->charCode=8785;
    data->fontWidth=12;
    data++;

    data->charCode=8786;
    data->fontWidth=12;
    data++;

    data->charCode=8787;
    data->fontWidth=12;
    data++;

    data->charCode=8788;
    data->fontWidth=12;
    data++;

    data->charCode=8789;
    data->fontWidth=12;
    data++;

    data->charCode=8790;
    data->fontWidth=12;
    data++;

    data->charCode=8791;
    data->fontWidth=12;
    data++;

    data->charCode=8792;
    data->fontWidth=12;
    data++;

    data->charCode=8793;
    data->fontWidth=12;
    data++;

    data->charCode=8794;
    data->fontWidth=12;
    data++;

    data->charCode=8795;
    data->fontWidth=12;
    data++;

    data->charCode=8796;
    data->fontWidth=12;
    data++;

    data->charCode=8797;
    data->fontWidth=12;
    data++;

    data->charCode=8798;
    data->fontWidth=12;
    data++;

    data->charCode=8799;
    data->fontWidth=12;
    data++;

    data->charCode=8800;
    data->fontWidth=12;
    data++;

    data->charCode=8801;
    data->fontWidth=12;
    data++;

    data->charCode=8802;
    data->fontWidth=12;
    data++;

    data->charCode=8803;
    data->fontWidth=12;
    data++;

    data->charCode=8804;
    data->fontWidth=12;
    data++;

    data->charCode=8805;
    data->fontWidth=12;
    data++;

    data->charCode=8806;
    data->fontWidth=12;
    data++;

    data->charCode=8807;
    data->fontWidth=12;
    data++;

    data->charCode=8808;
    data->fontWidth=12;
    data++;

    data->charCode=8809;
    data->fontWidth=12;
    data++;

    data->charCode=8810;
    data->fontWidth=12;
    data++;

    data->charCode=8811;
    data->fontWidth=12;
    data++;

    data->charCode=8812;
    data->fontWidth=12;
    data++;

    data->charCode=8813;
    data->fontWidth=12;
    data++;

    data->charCode=8814;
    data->fontWidth=12;
    data++;

    data->charCode=8815;
    data->fontWidth=12;
    data++;

    data->charCode=8816;
    data->fontWidth=12;
    data++;

    data->charCode=8817;
    data->fontWidth=12;
    data++;

    data->charCode=8818;
    data->fontWidth=12;
    data++;

    data->charCode=8819;
    data->fontWidth=12;
    data++;

    data->charCode=8820;
    data->fontWidth=12;
    data++;

    data->charCode=8821;
    data->fontWidth=12;
    data++;

    data->charCode=8822;
    data->fontWidth=12;
    data++;

    data->charCode=8823;
    data->fontWidth=6;
    data++;

    data->charCode=8824;
    data->fontWidth=6;
    data++;

    data->charCode=8825;
    data->fontWidth=6;
    data++;

    data->charCode=8826;
    data->fontWidth=12;
    data++;

    data->charCode=8827;
    data->fontWidth=12;
    data++;

    data->charCode=8828;
    data->fontWidth=12;
    data++;

    data->charCode=8829;
    data->fontWidth=12;
    data++;

    data->charCode=8830;
    data->fontWidth=12;
    data++;

    data->charCode=8993;
    data->fontWidth=12;
    data++;

    data->charCode=8994;
    data->fontWidth=12;
    data++;

    data->charCode=8995;
    data->fontWidth=12;
    data++;

    data->charCode=8996;
    data->fontWidth=12;
    data++;

    data->charCode=8997;
    data->fontWidth=12;
    data++;

    data->charCode=8998;
    data->fontWidth=12;
    data++;

    data->charCode=8999;
    data->fontWidth=12;
    data++;

    data->charCode=9000;
    data->fontWidth=12;
    data++;

    data->charCode=9001;
    data->fontWidth=12;
    data++;

    data->charCode=9002;
    data->fontWidth=12;
    data++;

    data->charCode=9003;
    data->fontWidth=12;
    data++;

    data->charCode=9004;
    data->fontWidth=12;
    data++;

    data->charCode=9005;
    data->fontWidth=12;
    data++;

    data->charCode=9006;
    data->fontWidth=12;
    data++;

    data->charCode=9007;
    data->fontWidth=12;
    data++;

    data->charCode=9008;
    data->fontWidth=8;
    data++;

    data->charCode=9009;
    data->fontWidth=8;
    data++;

    data->charCode=9010;
    data->fontWidth=8;
    data++;

    data->charCode=9011;
    data->fontWidth=8;
    data++;

    data->charCode=9012;
    data->fontWidth=8;
    data++;

    data->charCode=9013;
    data->fontWidth=8;
    data++;

    data->charCode=9014;
    data->fontWidth=8;
    data++;

    data->charCode=9015;
    data->fontWidth=8;
    data++;

    data->charCode=9016;
    data->fontWidth=8;
    data++;

    data->charCode=9017;
    data->fontWidth=8;
    data++;

    data->charCode=9018;
    data->fontWidth=12;
    data++;

    data->charCode=9019;
    data->fontWidth=12;
    data++;

    data->charCode=9020;
    data->fontWidth=12;
    data++;

    data->charCode=9021;
    data->fontWidth=12;
    data++;

    data->charCode=9022;
    data->fontWidth=12;
    data++;

    data->charCode=9023;
    data->fontWidth=12;
    data++;

    data->charCode=9024;
    data->fontWidth=12;
    data++;

    data->charCode=9025;
    data->fontWidth=9;
    data++;

    data->charCode=9026;
    data->fontWidth=9;
    data++;

    data->charCode=9027;
    data->fontWidth=9;
    data++;

    data->charCode=9028;
    data->fontWidth=9;
    data++;

    data->charCode=9029;
    data->fontWidth=9;
    data++;

    data->charCode=9030;
    data->fontWidth=8;
    data++;

    data->charCode=9031;
    data->fontWidth=9;
    data++;

    data->charCode=9032;
    data->fontWidth=9;
    data++;

    data->charCode=9033;
    data->fontWidth=3;
    data++;

    data->charCode=9034;
    data->fontWidth=7;
    data++;

    data->charCode=9035;
    data->fontWidth=9;
    data++;

    data->charCode=9036;
    data->fontWidth=8;
    data++;

    data->charCode=9037;
    data->fontWidth=11;
    data++;

    data->charCode=9038;
    data->fontWidth=9;
    data++;

    data->charCode=9039;
    data->fontWidth=10;
    data++;

    data->charCode=9040;
    data->fontWidth=9;
    data++;

    data->charCode=9041;
    data->fontWidth=10;
    data++;

    data->charCode=9042;
    data->fontWidth=9;
    data++;

    data->charCode=9043;
    data->fontWidth=9;
    data++;

    data->charCode=9044;
    data->fontWidth=7;
    data++;

    data->charCode=9045;
    data->fontWidth=9;
    data++;

    data->charCode=9046;
    data->fontWidth=9;
    data++;

    data->charCode=9047;
    data->fontWidth=12;
    data++;

    data->charCode=9048;
    data->fontWidth=8;
    data++;

    data->charCode=9049;
    data->fontWidth=8;
    data++;

    data->charCode=9050;
    data->fontWidth=8;
    data++;

    data->charCode=9051;
    data->fontWidth=12;
    data++;

    data->charCode=9052;
    data->fontWidth=12;
    data++;

    data->charCode=9053;
    data->fontWidth=12;
    data++;

    data->charCode=9054;
    data->fontWidth=12;
    data++;

    data->charCode=9055;
    data->fontWidth=12;
    data++;

    data->charCode=9056;
    data->fontWidth=12;
    data++;

    data->charCode=9057;
    data->fontWidth=7;
    data++;

    data->charCode=9058;
    data->fontWidth=7;
    data++;

    data->charCode=9059;
    data->fontWidth=7;
    data++;

    data->charCode=9060;
    data->fontWidth=7;
    data++;

    data->charCode=9061;
    data->fontWidth=7;
    data++;

    data->charCode=9062;
    data->fontWidth=4;
    data++;

    data->charCode=9063;
    data->fontWidth=7;
    data++;

    data->charCode=9064;
    data->fontWidth=7;
    data++;

    data->charCode=9065;
    data->fontWidth=3;
    data++;

    data->charCode=9066;
    data->fontWidth=3;
    data++;

    data->charCode=9067;
    data->fontWidth=7;
    data++;

    data->charCode=9068;
    data->fontWidth=3;
    data++;

    data->charCode=9069;
    data->fontWidth=11;
    data++;

    data->charCode=9070;
    data->fontWidth=7;
    data++;

    data->charCode=9071;
    data->fontWidth=7;
    data++;

    data->charCode=9072;
    data->fontWidth=7;
    data++;

    data->charCode=9073;
    data->fontWidth=7;
    data++;

    data->charCode=9074;
    data->fontWidth=5;
    data++;

    data->charCode=9075;
    data->fontWidth=7;
    data++;

    data->charCode=9076;
    data->fontWidth=4;
    data++;

    data->charCode=9077;
    data->fontWidth=7;
    data++;

    data->charCode=9078;
    data->fontWidth=6;
    data++;

    data->charCode=9079;
    data->fontWidth=9;
    data++;

    data->charCode=9080;
    data->fontWidth=6;
    data++;

    data->charCode=9081;
    data->fontWidth=6;
    data++;

    data->charCode=9082;
    data->fontWidth=6;
    data++;

    data->charCode=9083;
    data->fontWidth=12;
    data++;

    data->charCode=9084;
    data->fontWidth=12;
    data++;

    data->charCode=9085;
    data->fontWidth=12;
    data++;

    data->charCode=9086;
    data->fontWidth=12;
    data++;

    data->charCode=9249;
    data->fontWidth=9;
    data++;

    data->charCode=9250;
    data->fontWidth=11;
    data++;

    data->charCode=9251;
    data->fontWidth=10;
    data++;

    data->charCode=9252;
    data->fontWidth=11;
    data++;

    data->charCode=9253;
    data->fontWidth=7;
    data++;

    data->charCode=9254;
    data->fontWidth=8;
    data++;

    data->charCode=9255;
    data->fontWidth=9;
    data++;

    data->charCode=9256;
    data->fontWidth=11;
    data++;

    data->charCode=9257;
    data->fontWidth=10;
    data++;

    data->charCode=9258;
    data->fontWidth=11;
    data++;

    data->charCode=9259;
    data->fontWidth=12;
    data++;

    data->charCode=9260;
    data->fontWidth=12;
    data++;

    data->charCode=9261;
    data->fontWidth=10;
    data++;

    data->charCode=9262;
    data->fontWidth=11;
    data++;

    data->charCode=9263;
    data->fontWidth=7;
    data++;

    data->charCode=9264;
    data->fontWidth=10;
    data++;

    data->charCode=9265;
    data->fontWidth=11;
    data++;

    data->charCode=9266;
    data->fontWidth=12;
    data++;

    data->charCode=9267;
    data->fontWidth=10;
    data++;

    data->charCode=9268;
    data->fontWidth=11;
    data++;

    data->charCode=9269;
    data->fontWidth=9;
    data++;

    data->charCode=9270;
    data->fontWidth=11;
    data++;

    data->charCode=9271;
    data->fontWidth=9;
    data++;

    data->charCode=9272;
    data->fontWidth=9;
    data++;

    data->charCode=9273;
    data->fontWidth=12;
    data++;

    data->charCode=9274;
    data->fontWidth=12;
    data++;

    data->charCode=9275;
    data->fontWidth=12;
    data++;

    data->charCode=9276;
    data->fontWidth=12;
    data++;

    data->charCode=9277;
    data->fontWidth=11;
    data++;

    data->charCode=9278;
    data->fontWidth=12;
    data++;

    data->charCode=9279;
    data->fontWidth=11;
    data++;

    data->charCode=9280;
    data->fontWidth=11;
    data++;

    data->charCode=9281;
    data->fontWidth=10;
    data++;

    data->charCode=9282;
    data->fontWidth=11;
    data++;

    data->charCode=9283;
    data->fontWidth=10;
    data++;

    data->charCode=9284;
    data->fontWidth=11;
    data++;

    data->charCode=9285;
    data->fontWidth=11;
    data++;

    data->charCode=9286;
    data->fontWidth=11;
    data++;

    data->charCode=9287;
    data->fontWidth=11;
    data++;

    data->charCode=9288;
    data->fontWidth=9;
    data++;

    data->charCode=9289;
    data->fontWidth=11;
    data++;

    data->charCode=9290;
    data->fontWidth=11;
    data++;

    data->charCode=9291;
    data->fontWidth=11;
    data++;

    data->charCode=9292;
    data->fontWidth=12;
    data++;

    data->charCode=9293;
    data->fontWidth=12;
    data++;

    data->charCode=9294;
    data->fontWidth=12;
    data++;

    data->charCode=9295;
    data->fontWidth=12;
    data++;

    data->charCode=9296;
    data->fontWidth=12;
    data++;

    data->charCode=9297;
    data->fontWidth=12;
    data++;

    data->charCode=9298;
    data->fontWidth=12;
    data++;

    data->charCode=9299;
    data->fontWidth=12;
    data++;

    data->charCode=9300;
    data->fontWidth=12;
    data++;

    data->charCode=9301;
    data->fontWidth=12;
    data++;

    data->charCode=9302;
    data->fontWidth=12;
    data++;

    data->charCode=9303;
    data->fontWidth=12;
    data++;

    data->charCode=9304;
    data->fontWidth=12;
    data++;

    data->charCode=9305;
    data->fontWidth=12;
    data++;

    data->charCode=9306;
    data->fontWidth=12;
    data++;

    data->charCode=9307;
    data->fontWidth=12;
    data++;

    data->charCode=9308;
    data->fontWidth=12;
    data++;

    data->charCode=9309;
    data->fontWidth=12;
    data++;

    data->charCode=9310;
    data->fontWidth=11;
    data++;

    data->charCode=9311;
    data->fontWidth=12;
    data++;

    data->charCode=9312;
    data->fontWidth=12;
    data++;

    data->charCode=9313;
    data->fontWidth=12;
    data++;

    data->charCode=9314;
    data->fontWidth=10;
    data++;

    data->charCode=9315;
    data->fontWidth=10;
    data++;

    data->charCode=9316;
    data->fontWidth=12;
    data++;

    data->charCode=9317;
    data->fontWidth=10;
    data++;

    data->charCode=9318;
    data->fontWidth=12;
    data++;

    data->charCode=9319;
    data->fontWidth=9;
    data++;

    data->charCode=9320;
    data->fontWidth=10;
    data++;

    data->charCode=9321;
    data->fontWidth=10;
    data++;

    data->charCode=9322;
    data->fontWidth=9;
    data++;

    data->charCode=9323;
    data->fontWidth=10;
    data++;

    data->charCode=9324;
    data->fontWidth=12;
    data++;

    data->charCode=9325;
    data->fontWidth=10;
    data++;

    data->charCode=9326;
    data->fontWidth=10;
    data++;

    data->charCode=9327;
    data->fontWidth=12;
    data++;

    data->charCode=9328;
    data->fontWidth=12;
    data++;

    data->charCode=9329;
    data->fontWidth=12;
    data++;

    data->charCode=9330;
    data->fontWidth=10;
    data++;

    data->charCode=9331;
    data->fontWidth=12;
    data++;

    data->charCode=9332;
    data->fontWidth=12;
    data++;

    data->charCode=9333;
    data->fontWidth=12;
    data++;

    data->charCode=9334;
    data->fontWidth=12;
    data++;

    data->charCode=9335;
    data->fontWidth=12;
    data++;

    data->charCode=9336;
    data->fontWidth=12;
    data++;

    data->charCode=9337;
    data->fontWidth=12;
    data++;

    data->charCode=9338;
    data->fontWidth=12;
    data++;

    data->charCode=9339;
    data->fontWidth=12;
    data++;

    data->charCode=9340;
    data->fontWidth=12;
    data++;

    data->charCode=9341;
    data->fontWidth=12;
    data++;

    data->charCode=9342;
    data->fontWidth=12;
    data++;

    data->charCode=9505;
    data->fontWidth=9;
    data++;

    data->charCode=9506;
    data->fontWidth=11;
    data++;

    data->charCode=9507;
    data->fontWidth=8;
    data++;

    data->charCode=9508;
    data->fontWidth=10;
    data++;

    data->charCode=9509;
    data->fontWidth=9;
    data++;

    data->charCode=9510;
    data->fontWidth=11;
    data++;

    data->charCode=9511;
    data->fontWidth=9;
    data++;

    data->charCode=9512;
    data->fontWidth=11;
    data++;

    data->charCode=9513;
    data->fontWidth=10;
    data++;

    data->charCode=9514;
    data->fontWidth=11;
    data++;

    data->charCode=9515;
    data->fontWidth=10;
    data++;

    data->charCode=9516;
    data->fontWidth=11;
    data++;

    data->charCode=9517;
    data->fontWidth=12;
    data++;

    data->charCode=9518;
    data->fontWidth=12;
    data++;

    data->charCode=9519;
    data->fontWidth=10;
    data++;

    data->charCode=9520;
    data->fontWidth=11;
    data++;

    data->charCode=9521;
    data->fontWidth=11;
    data++;

    data->charCode=9522;
    data->fontWidth=12;
    data++;

    data->charCode=9523;
    data->fontWidth=10;
    data++;

    data->charCode=9524;
    data->fontWidth=11;
    data++;

    data->charCode=9525;
    data->fontWidth=12;
    data++;

    data->charCode=9526;
    data->fontWidth=12;
    data++;

    data->charCode=9527;
    data->fontWidth=11;
    data++;

    data->charCode=9528;
    data->fontWidth=11;
    data++;

    data->charCode=9529;
    data->fontWidth=11;
    data++;

    data->charCode=9530;
    data->fontWidth=12;
    data++;

    data->charCode=9531;
    data->fontWidth=11;
    data++;

    data->charCode=9532;
    data->fontWidth=12;
    data++;

    data->charCode=9533;
    data->fontWidth=10;
    data++;

    data->charCode=9534;
    data->fontWidth=11;
    data++;

    data->charCode=9535;
    data->fontWidth=9;
    data++;

    data->charCode=9536;
    data->fontWidth=11;
    data++;

    data->charCode=9537;
    data->fontWidth=11;
    data++;

    data->charCode=9538;
    data->fontWidth=12;
    data++;

    data->charCode=9539;
    data->fontWidth=9;
    data++;

    data->charCode=9540;
    data->fontWidth=10;
    data++;

    data->charCode=9541;
    data->fontWidth=11;
    data++;

    data->charCode=9542;
    data->fontWidth=11;
    data++;

    data->charCode=9543;
    data->fontWidth=12;
    data++;

    data->charCode=9544;
    data->fontWidth=8;
    data++;

    data->charCode=9545;
    data->fontWidth=8;
    data++;

    data->charCode=9546;
    data->fontWidth=11;
    data++;

    data->charCode=9547;
    data->fontWidth=11;
    data++;

    data->charCode=9548;
    data->fontWidth=10;
    data++;

    data->charCode=9549;
    data->fontWidth=11;
    data++;

    data->charCode=9550;
    data->fontWidth=8;
    data++;

    data->charCode=9551;
    data->fontWidth=12;
    data++;

    data->charCode=9552;
    data->fontWidth=12;
    data++;

    data->charCode=9553;
    data->fontWidth=12;
    data++;

    data->charCode=9554;
    data->fontWidth=9;
    data++;

    data->charCode=9555;
    data->fontWidth=10;
    data++;

    data->charCode=9556;
    data->fontWidth=10;
    data++;

    data->charCode=9557;
    data->fontWidth=10;
    data++;

    data->charCode=9558;
    data->fontWidth=11;
    data++;

    data->charCode=9559;
    data->fontWidth=11;
    data++;

    data->charCode=9560;
    data->fontWidth=11;
    data++;

    data->charCode=9561;
    data->fontWidth=11;
    data++;

    data->charCode=9562;
    data->fontWidth=11;
    data++;

    data->charCode=9563;
    data->fontWidth=11;
    data++;

    data->charCode=9564;
    data->fontWidth=11;
    data++;

    data->charCode=9565;
    data->fontWidth=11;
    data++;

    data->charCode=9566;
    data->fontWidth=11;
    data++;

    data->charCode=9567;
    data->fontWidth=8;
    data++;

    data->charCode=9568;
    data->fontWidth=12;
    data++;

    data->charCode=9569;
    data->fontWidth=9;
    data++;

    data->charCode=9570;
    data->fontWidth=11;
    data++;

    data->charCode=9571;
    data->fontWidth=10;
    data++;

    data->charCode=9572;
    data->fontWidth=12;
    data++;

    data->charCode=9573;
    data->fontWidth=10;
    data++;

    data->charCode=9574;
    data->fontWidth=12;
    data++;

    data->charCode=9575;
    data->fontWidth=8;
    data++;

    data->charCode=9576;
    data->fontWidth=9;
    data++;

    data->charCode=9577;
    data->fontWidth=10;
    data++;

    data->charCode=9578;
    data->fontWidth=9;
    data++;

    data->charCode=9579;
    data->fontWidth=12;
    data++;

    data->charCode=9580;
    data->fontWidth=10;
    data++;

    data->charCode=9581;
    data->fontWidth=10;
    data++;

    data->charCode=9582;
    data->fontWidth=9;
    data++;

    data->charCode=9583;
    data->fontWidth=11;
    data++;

    data->charCode=9584;
    data->fontWidth=12;
    data++;

    data->charCode=9585;
    data->fontWidth=12;
    data++;

    data->charCode=9586;
    data->fontWidth=10;
    data++;

    data->charCode=9587;
    data->fontWidth=10;
    data++;

    data->charCode=9588;
    data->fontWidth=12;
    data++;

    data->charCode=9589;
    data->fontWidth=9;
    data++;

    data->charCode=9590;
    data->fontWidth=9;
    data++;

    data->charCode=9591;
    data->fontWidth=12;
    data++;

    data->charCode=9592;
    data->fontWidth=12;
    data++;

    data->charCode=9593;
    data->fontWidth=12;
    data++;

    data->charCode=9594;
    data->fontWidth=12;
    data++;

    data->charCode=9595;
    data->fontWidth=12;
    data++;

    data->charCode=9596;
    data->fontWidth=12;
    data++;

    data->charCode=9597;
    data->fontWidth=12;
    data++;

    data->charCode=9598;
    data->fontWidth=12;
    data++;

    data->charCode=9761;
    data->fontWidth=12;
    data++;

    data->charCode=9762;
    data->fontWidth=12;
    data++;

    data->charCode=9763;
    data->fontWidth=12;
    data++;

    data->charCode=9764;
    data->fontWidth=12;
    data++;

    data->charCode=9765;
    data->fontWidth=12;
    data++;

    data->charCode=9766;
    data->fontWidth=12;
    data++;

    data->charCode=9767;
    data->fontWidth=12;
    data++;

    data->charCode=9768;
    data->fontWidth=12;
    data++;

    data->charCode=9769;
    data->fontWidth=12;
    data++;

    data->charCode=9770;
    data->fontWidth=12;
    data++;

    data->charCode=9771;
    data->fontWidth=12;
    data++;

    data->charCode=9772;
    data->fontWidth=12;
    data++;

    data->charCode=9773;
    data->fontWidth=12;
    data++;

    data->charCode=9774;
    data->fontWidth=12;
    data++;

    data->charCode=9775;
    data->fontWidth=12;
    data++;

    data->charCode=9776;
    data->fontWidth=12;
    data++;

    data->charCode=9777;
    data->fontWidth=12;
    data++;

    data->charCode=9778;
    data->fontWidth=12;
    data++;

    data->charCode=9779;
    data->fontWidth=12;
    data++;

    data->charCode=9780;
    data->fontWidth=12;
    data++;

    data->charCode=9781;
    data->fontWidth=12;
    data++;

    data->charCode=9782;
    data->fontWidth=12;
    data++;

    data->charCode=9783;
    data->fontWidth=12;
    data++;

    data->charCode=9784;
    data->fontWidth=12;
    data++;

    data->charCode=9785;
    data->fontWidth=12;
    data++;

    data->charCode=9786;
    data->fontWidth=12;
    data++;

    data->charCode=9787;
    data->fontWidth=12;
    data++;

    data->charCode=9788;
    data->fontWidth=12;
    data++;

    data->charCode=9789;
    data->fontWidth=12;
    data++;

    data->charCode=9790;
    data->fontWidth=12;
    data++;

    data->charCode=9791;
    data->fontWidth=12;
    data++;

    data->charCode=9792;
    data->fontWidth=12;
    data++;

    data->charCode=9793;
    data->fontWidth=12;
    data++;

    data->charCode=9794;
    data->fontWidth=12;
    data++;

    data->charCode=9795;
    data->fontWidth=12;
    data++;

    data->charCode=9796;
    data->fontWidth=12;
    data++;

    data->charCode=9797;
    data->fontWidth=12;
    data++;

    data->charCode=9798;
    data->fontWidth=12;
    data++;

    data->charCode=9799;
    data->fontWidth=12;
    data++;

    data->charCode=9800;
    data->fontWidth=12;
    data++;

    data->charCode=9801;
    data->fontWidth=12;
    data++;

    data->charCode=9802;
    data->fontWidth=12;
    data++;

    data->charCode=9803;
    data->fontWidth=12;
    data++;

    data->charCode=9804;
    data->fontWidth=12;
    data++;

    data->charCode=9805;
    data->fontWidth=12;
    data++;

    data->charCode=9806;
    data->fontWidth=12;
    data++;

    data->charCode=9807;
    data->fontWidth=12;
    data++;

    data->charCode=9808;
    data->fontWidth=12;
    data++;

    data->charCode=9809;
    data->fontWidth=12;
    data++;

    data->charCode=9810;
    data->fontWidth=12;
    data++;

    data->charCode=9811;
    data->fontWidth=12;
    data++;

    data->charCode=9812;
    data->fontWidth=12;
    data++;

    data->charCode=9813;
    data->fontWidth=12;
    data++;

    data->charCode=9814;
    data->fontWidth=12;
    data++;

    data->charCode=9815;
    data->fontWidth=12;
    data++;

    data->charCode=9816;
    data->fontWidth=12;
    data++;

    data->charCode=9817;
    data->fontWidth=12;
    data++;

    data->charCode=9818;
    data->fontWidth=12;
    data++;

    data->charCode=9819;
    data->fontWidth=12;
    data++;

    data->charCode=9820;
    data->fontWidth=12;
    data++;

    data->charCode=9821;
    data->fontWidth=12;
    data++;

    data->charCode=9822;
    data->fontWidth=12;
    data++;

    data->charCode=9823;
    data->fontWidth=12;
    data++;

    data->charCode=9824;
    data->fontWidth=12;
    data++;

    data->charCode=9825;
    data->fontWidth=12;
    data++;

    data->charCode=9826;
    data->fontWidth=12;
    data++;

    data->charCode=9827;
    data->fontWidth=12;
    data++;

    data->charCode=9828;
    data->fontWidth=12;
    data++;

    data->charCode=9829;
    data->fontWidth=12;
    data++;

    data->charCode=9830;
    data->fontWidth=12;
    data++;

    data->charCode=9831;
    data->fontWidth=12;
    data++;

    data->charCode=9832;
    data->fontWidth=12;
    data++;

    data->charCode=9833;
    data->fontWidth=12;
    data++;

    data->charCode=9834;
    data->fontWidth=12;
    data++;

    data->charCode=9835;
    data->fontWidth=12;
    data++;

    data->charCode=9836;
    data->fontWidth=12;
    data++;

    data->charCode=9837;
    data->fontWidth=12;
    data++;

    data->charCode=9838;
    data->fontWidth=12;
    data++;

    data->charCode=9839;
    data->fontWidth=12;
    data++;

    data->charCode=9840;
    data->fontWidth=12;
    data++;

    data->charCode=9841;
    data->fontWidth=12;
    data++;

    data->charCode=9842;
    data->fontWidth=12;
    data++;

    data->charCode=9843;
    data->fontWidth=12;
    data++;

    data->charCode=9844;
    data->fontWidth=12;
    data++;

    data->charCode=9845;
    data->fontWidth=12;
    data++;

    data->charCode=9846;
    data->fontWidth=12;
    data++;

    data->charCode=9847;
    data->fontWidth=12;
    data++;

    data->charCode=9848;
    data->fontWidth=12;
    data++;

    data->charCode=9849;
    data->fontWidth=12;
    data++;

    data->charCode=9850;
    data->fontWidth=12;
    data++;

    data->charCode=9851;
    data->fontWidth=12;
    data++;

    data->charCode=9852;
    data->fontWidth=12;
    data++;

    data->charCode=9853;
    data->fontWidth=12;
    data++;

    data->charCode=9854;
    data->fontWidth=12;
    data++;

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  getFontWidth                                              */
/*                                                  フォントの幅を取得する */
/*-------------------------------------------------------------------------*/
static UInt16 getFontWidth(UInt16 charCode, UInt16 defaultWidth, UInt16 dbCount, A2DAcharFontWidth *data)
{
    Int16              chk;
    UInt16             mid, start, end;
    A2DAcharFontWidth *ptr;

    // ２分検索で指定されたキャラクタコードがDBにあるかチェック
    mid   = 0;
    start = 0;
    end   = dbCount;
    while (start <= end)
    {
        mid = start + (end - start) / 2;
        ptr = data + mid;

        chk = ptr->charCode - charCode;
        if (chk == 0)
        {
            // データが見つかった、フォント幅を応答して終了する
            return (ptr->fontWidth);
        }
        if (chk < 0)
        {
            start = mid + 1;
        }
        else
        {
            end   = mid - 1;
        }
    }

    // データがみつからなかった、デフォルトのフォント幅を応答して終了する
    return (defaultWidth);
}
