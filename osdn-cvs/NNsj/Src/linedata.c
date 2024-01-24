/*============================================================================*
 *  FILE: 
 *     linedata.c
 *
 *  Description: 
 *     データの出力処理
 *
 *===========================================================================*/
#define LINEDATA_C
#include "local.h"


/*=========================================================================*/
/*   Function :   NNsi_putLine                                             */
/*                                                                画面表示 */
/*=========================================================================*/
void NNsi_putLine(FormType *frm, UInt16 gadId, UInt16 upBtnID, UInt16 downBtnID, UInt16 command)
{
    RectangleType dimF;
    UInt16        fontID, nlines, fontHeight;
    UInt16        dataLines, startLine, lineStatus, lp, len;

    Char             lineData[BUFSIZE + MARGIN];
    NNshMacroRecord *source;

    dataLines = NNshGlobal->currentScriptLine;
    source    = (NNshMacroRecord *) NNshGlobal->scriptArea;

    // 状態の初期化
    lineStatus = NNSH_DISP_ALL;
    
    // 画面の大きさと表示可能行数を取得する
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, gadId), &dimF);
    fontID = NNshParam->currentFont;
    NNsi_UpdateRectangle(&dimF, NNshParam->currentFont,
                                                &fontID, &fontHeight, &nlines);

    // コマンドから表示位置の先頭と表示状態を決定する
    switch (command)
    {
      case NNSH_STEP_TO_TOP:
        // 先頭ページを表示する
        startLine  = 0;
        lineStatus = NNSH_DISP_UPPERLIMIT;
        break;

      case NNSH_STEP_PAGEUP:
        // １ページ上を表示する
        if (NNshGlobal->pageTopLine <= nlines)
        {
            startLine  = 0;
            lineStatus = NNSH_DISP_UPPERLIMIT;
        }
        else
        {
            startLine = NNshGlobal->pageTopLine - nlines;
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_PAGEDOWN:
        // １ページ下を表示する
        if ((NNshGlobal->pageTopLine + nlines) >= dataLines)
        {
            startLine  = NNshGlobal->pageTopLine;
            lineStatus = NNSH_DISP_LOWERLIMIT;
        }
        else
        {
            startLine  = NNshGlobal->pageTopLine + nlines;
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_REDRAW:
        // 現在のところを再描画
        startLine  = NNshGlobal->pageTopLine;
        if (NNshGlobal->pageTopLine == 0)
        {
            lineStatus = NNSH_DISP_UPPERLIMIT;
        }
        else
        {
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_TO_BOTTOM:
      default:
        // 末尾ページを表示する
        startLine  = (dataLines >= nlines) ? 0 : dataLines - nlines;
        lineStatus = NNSH_DISP_LOWERLIMIT;
        break;
    }

    // ページ状態を反映
    if (lineStatus != NNSH_DISP_UPPERLIMIT)
    {
        if (startLine + nlines >= dataLines)
        {
            lineStatus = NNSH_DISP_LOWERLIMIT;
        }
        else
        {
            lineStatus = NNSH_DISP_HALFWAY;
        }
    }
    if (dataLines <= nlines)
    {
        lineStatus = NNSH_DISP_ALL;
    }

    // 表示領域をクリアする
    NNsi_EraseRectangle(&dimF);

    // 上下ボタンの表示設定
    NNsh_UpDownButtonControl(FrmGetActiveForm(), lineStatus, upBtnID, downBtnID);

#ifdef USE_HIGHDENSITY
    // 高解像度の設定
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        // FntSetFont(NNshParam->sonyHRFontTitle);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

    // 文字列の描画（行単位で描画する）
    for (lp = 0; lp < nlines; lp++)
    {
        if ((lp + startLine) > NNshGlobal->currentScriptLine)
        {
            // 最終行を超えたら、そこにはデータを表示しない
            break;
        }
        MemSet(lineData, sizeof(lineData), 0x00);
        PrintMacroLine(lineData, BUFSIZE, (lp + startLine + 1), &source[lp + startLine]);

#ifdef USE_CLIE
        if (NNshGlobal->hrRef != 0)
        {
            // フォントのワードラップ(描画領域内に内容を抑えるため)
            if (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE)
            {
                len = HRFntWordWrap(NNshGlobal->hrRef, lineData, dimF.extent.x);
            }
            else
            {
                len = FntWordWrap(lineData, dimF.extent.x);
            }
            // CLIEハイレゾ描画(Truncateして表示)
            HRWinDrawTruncChars(NNshGlobal->hrRef, lineData, StrLen(lineData),
                                dimF.topLeft.x, dimF.topLeft.y, dimF.extent.x);
        }
        else
#endif
        {
            // 通常モード描画(Truncateして表示)
            len = FntWordWrap(lineData, dimF.extent.x);
            WinDrawTruncChars(lineData,len,dimF.topLeft.x,
                              dimF.topLeft.y, dimF.extent.x);
        }

        // 次の行に描画位置を移動させる
        dimF.topLeft.y = dimF.topLeft.y + fontHeight;
    }

#ifdef USE_HIGHDENSITY
    // 高解像度の設定を解除
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),
                      NNshGlobal->os5HighDensity);
        WinSetCoordinateSystem(kCoordinatesStandard);
        // FntSetFont(NNshParam->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

    // 先頭行をpageTopLineに記憶する
    NNshGlobal->pageTopLine = startLine;

    return;
}

/*=========================================================================*/
/*   Function :   NNsh_UpDownButtonControl                                 */
/*                                             上下ボタンの表示/非表示設定 */
/*=========================================================================*/
void NNsh_UpDownButtonControl(FormType *frm, UInt16 btnState, UInt16 upBtnID, UInt16 downBtnID)
{
    // 上限/下限のボタンを表示する
    // (上下ボタンを表示/非表示にする)
    switch (btnState)
    {
      case NNSH_DISP_NOTHING:
        // 一覧なし
        FrmHideObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmHideObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;

      case NNSH_DISP_UPPERLIMIT:
        // 上限
        FrmHideObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmShowObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;

      case NNSH_DISP_LOWERLIMIT:
        // 下限
        FrmShowObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmHideObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;

      case NNSH_DISP_ALL:
        // 全部
        FrmHideObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmHideObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;

      case NNSH_DISP_HALFWAY:
      default:
        // 途中
        FrmShowObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmShowObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;
    }
    return;
}
