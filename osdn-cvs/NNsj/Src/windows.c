/*============================================================================*
 *  FILE: 
 *     windows.c
 *
 *  Description: 
 *     window control functions for NNsh.
 *
 *===========================================================================*/
#define WINDOWS_C

#include "local.h"
/*=========================================================================*/
/*   Function : NNsi_ConvertLocation                                       */
/*                                                    座標位置をコンバート */
/*=========================================================================*/
void NNsi_ConvertLocation(Coord *screenX, Coord *screenY)
{
    // フォントの設定
#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        // 単純に２倍にして良いのかな？ (あとで検討...)
        *screenX = *screenX * 2;
        *screenY = *screenY * 2;
    }
#endif

#ifdef USE_HIGHDENSITY
    // 解像度にあわせて描画領域を調整する
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
        *screenX = *screenX * 2;
        *screenY = *screenY * 2;
        break;

      case kDensityTriple:
        *screenX = *screenX * 3;
        *screenY = *screenY * 3;
        break;

      case kDensityQuadruple:
        *screenX = *screenX * 4;
        *screenY = *screenY * 4;
        break;

      case kDensityOneAndAHalf:
        *screenX = *screenX * 3 / 2;
        *screenY = *screenY * 3 / 2;
        break;

      case kDensityLow:
      default:
        break;
    }
#endif    // #ifdef USE_HIGHDENSITY

    return;
}

/*=========================================================================*/
/*   Function : NNsi_EraseRectangle                                        */
/*                                                    描画領域をクリアする */
/*=========================================================================*/
void NNsi_EraseRectangle(RectangleType *dimF)
{
    RectangleType area;

#ifdef USE_CLIE 
    // CLIEハイレゾモードのときは、そのまま与えられた領域をクリアする
    if (NNshGlobal->hrRef != 0)
    {
        area.topLeft.x  = dimF->topLeft.x;
        area.extent.x   = dimF->extent.x;
        area.extent.y   = dimF->extent.y;
        area.topLeft.y  = dimF->topLeft.y;
        HRWinEraseRectangle(NNshGlobal->hrRef, dimF, 0);
        return;
    }
#endif

#ifdef USE_HIGHDENSITY
    // 解像度にあわせて消去領域を調整する
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
        area.topLeft.x  = dimF->topLeft.x / 2;
        area.extent.x   = dimF->extent.x  / 2;
        area.extent.y   = dimF->extent.y  / 2;
        area.topLeft.y  = dimF->topLeft.y / 2;
        break;

      case kDensityTriple:
        area.topLeft.x  = dimF->topLeft.x / 3;
        area.extent.x   = dimF->extent.x  / 3;
        area.extent.y   = dimF->extent.y  / 3;
        area.topLeft.y  = dimF->topLeft.y / 3;
        break;

      case kDensityQuadruple:
        area.topLeft.x  = dimF->topLeft.x / 4;
        area.extent.x   = dimF->extent.x  / 4;
        area.extent.y   = dimF->extent.y  / 4;
        area.topLeft.y  = dimF->topLeft.y / 4;
        break;

      case kDensityOneAndAHalf:
        area.topLeft.x  = dimF->topLeft.x * 3 / 2;
        area.extent.x   = dimF->extent.x  * 3 / 2;
        area.extent.y   = dimF->extent.y  * 3 / 2;
        area.topLeft.y  = dimF->topLeft.y * 3 / 2;
        break;

      case kDensityLow:
      default:
        area.topLeft.x  = dimF->topLeft.x;
        area.extent.x   = dimF->extent.x;
        area.extent.y   = dimF->extent.y;
        area.topLeft.y  = dimF->topLeft.y;
        break;
    }
#else
    area.topLeft.x  = dimF->topLeft.x;
    area.extent.x   = dimF->extent.x;
    area.extent.y   = dimF->extent.y;
    area.topLeft.y  = dimF->topLeft.y;
#endif // #ifdef USE_HIGHDENSITY
    WinEraseRectangle(&area, 0);
    return;
}

/*=========================================================================*/
/*   Function : NNsi_UpdateRectangle                                       */
/*                                          描画領域とフォントの情報を更新 */
/*=========================================================================*/
void NNsi_UpdateRectangle(RectangleType *dimF, UInt16 fontType, UInt16 *fontID,
                         UInt16 *fontHeight, UInt16 *nlines)
{
    // フォントの設定
#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        // 単純に２倍にして良いのかな？ (あとで検討...)
        dimF->extent.x   = dimF->extent.x  * 2;
        dimF->extent.y   = dimF->extent.y  * 2;
        dimF->topLeft.x  = dimF->topLeft.x * 2;
        dimF->topLeft.y  = dimF->topLeft.y * 2;

        // 表示領域をクリアする
        // HRWinEraseRectangle(NNshGlobal->hrRef, dimF, 0);

        // フォントの設定
        if (fontType == 0)
        {
            // CLIEハイレゾモードフォントでなければ、倍解像度フォントを使用
            *fontID = HRFntSetFont(NNshGlobal->hrRef, 
                                   (NNshParam->currentFont + hrStdFont));
        }
        else
        {
            // 描画フォントの設定
            *fontID = HRFntSetFont(NNshGlobal->hrRef, *fontID);
        }

        // CLIE NX以降では、フォントの高さを求める関数が異なるので注意
        if (NNshGlobal->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
        {
            *fontHeight = FntCharHeight();
        }
        else
        {
            *fontHeight = HRFntCharHeight(NNshGlobal->hrRef);
        }
    }
    else
#endif
    {
        // 描画フォントを設定する
        // WinEraseRectangle(dimF, 0);
        *fontID       = FntSetFont(*fontID);
        *fontHeight   = FntCharHeight();
    }

#ifdef USE_HIGHDENSITY
    // 解像度にあわせて描画領域を調整する
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
        dimF->extent.x   = dimF->extent.x  * 2;
        dimF->extent.y   = dimF->extent.y  * 2;
        dimF->topLeft.x  = dimF->topLeft.x * 2;
        dimF->topLeft.y  = dimF->topLeft.y * 2;
        break;

      case kDensityTriple:
        dimF->extent.x   = dimF->extent.x  * 3;
        dimF->extent.y   = dimF->extent.y  * 3;
        dimF->topLeft.x  = dimF->topLeft.x * 3;
        dimF->topLeft.y  = dimF->topLeft.y * 3;
        break;

      case kDensityQuadruple:
        dimF->extent.x   = dimF->extent.x  * 4;
        dimF->extent.y   = dimF->extent.y  * 4;
        dimF->topLeft.x  = dimF->topLeft.x * 4;
        dimF->topLeft.y  = dimF->topLeft.y * 4;
        break;

      case kDensityOneAndAHalf:
        dimF->extent.x   = dimF->extent.x  * 3 / 2;
        dimF->extent.y   = dimF->extent.y  * 3 / 2;
        dimF->topLeft.x  = dimF->topLeft.x * 3 / 2;
        dimF->topLeft.y  = dimF->topLeft.y * 3 / 2;
        break;

      case kDensityLow:
      default:
        break;
    }
#endif    // #ifdef USE_HIGHDENSITY

    // 解像度にあわせて表示可能行数(nlines)を計算
    *nlines = ((dimF->extent.y) / (*fontHeight));
    return;
}

/*=========================================================================*/
/*   Function : NNsi_FrmDrawForm                                           */
/*                                                        フォームの再描画 */
/*=========================================================================*/
void NNsi_FrmDrawForm(FormType *frm, Boolean redraw)
{
    // フォームを(普通に)描画する
    FrmDrawForm(frm);

    return;
}

#if 0
/*=========================================================================*/
/*   Function : NNsi_HRSclUpdate                                           */
/*                                      スクロールバーの描画(ハイレゾ表示) */
/*=========================================================================*/
void NNsi_HRSclUpdate(ScrollBarType *barP)
{
    SclSetScrollBar(barP, (NNshGlobal->currentPage + 1), 1,
                        (NNshGlobal->nofPage + 1), 1);
    return;
}

/*=========================================================================*/
/*   Function : NNsi_HRFldDrawField                                        */
/*                                          フィールドの描画(ハイレゾ表示) */
/*=========================================================================*/
void NNsi_HRFldDrawField(FieldType *fldP, UInt16 isBottom)
{
    Char          *ptr,   *top;
    UInt16         fontID, lineLen, i, nlines, fontHeight = 0;
    Int16          len, line, startLine;
    MemHandle      txtH;
    RectangleType  dimF;

    // 表示文字列の取得
    txtH = FldGetTextHandle(fldP);
    if (txtH == 0)
    {
        // 領域取得に失敗
        goto END_FUNC;
    }
    ptr  = MemHandleLock(txtH);
    if (ptr == NULL)
    {
        // 領域取得に失敗、通常のフィールド表示をして終了する
        goto END_FUNC;
    }
    top = ptr;

    // フィールドの領域サイズを取ってくる
    FldGetBounds(fldP, &dimF);

    // フィールドを非表示にする
    FldReleaseFocus(fldP);
    FldSetUsable(fldP, false);

    // 描画領域とフォントの情報、表示可能行数を更新
    fontID = NNshParam->sonyHRFont;
    NNsi_UpdateRectangle(&dimF, NNshParam->useSonyTinyFont,
                         &fontID, &fontHeight, &nlines);

    // 表示領域をクリアする
    NNsi_EraseRectangle(&dimF);

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
        // FntSetFont(NNshParam->sonyHRFont);   // UpdateRectangle()内で実施
        break;

      case kDensityLow:
      default:
        break;
    }
#endif    // #ifdef USE_HIGHDENSITY

    // 表示する行数が未計算？
    if (NNshGlobal->totalLine == 0)
    {
#ifdef STRIP_MEANINGLESS_LINE
        /////// 無駄な(末尾の)空白を削る処理(ここから) ///////
        /////// (処理速度が気になるので、とりあえずコメントで) ///// 
        Char *tmpP;

        len  = StrLen(ptr);
        tmpP = ptr + len - 1;
            
        while ((len > 1)&&
               ((*tmpP == ' ')||(*tmpP == 0xd)||(*tmpP == 0xa)))
        {
            tmpP--;
            len--;
        }
        *(tmpP + 1) = NULL;
        lineLen = len;
        ///////// 無駄な(末尾の)空白を削る処理(ここまで) ///////
#else /// #ifdef STRIP_MEANINGLESS_LINE
        lineLen = StrLen(ptr);
#endif  /// #ifdef STRIP_MEANINGLESS_LINE

        NNshGlobal->currentPage = 0;
        NNshGlobal->prevHRLines = nlines;

#ifdef USE_CLIE
        // ハイレゾ描画モードでかつ、CLIE NXのとき
        if ((NNshGlobal->hrRef != 0)&&
            (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE))
        {
            // 表示するメッセージの行数を数え、gNumTotalLineに格納する
            while(lineLen > 0)
            {
                len = HRFntWordWrap(NNshGlobal->hrRef, ptr, dimF.extent.x);
                (NNshGlobal->totalLine)++;
                ptr = ptr + len;
                lineLen -= len;
            }
        }
        else
#endif
        {
            // 表示するメッセージの行数を数え、gNumTotalLineに格納する
            while(lineLen > 0)
            {
                len = FntWordWrap(ptr, dimF.extent.x);
                (NNshGlobal->totalLine)++;
                ptr = ptr + len;
                lineLen -= len;
            }
        }

        // メッセージの総ページ数を計算し、nofPageに格納する
        // 使用BBS選択画面から持ってきた計算方法
        NNshGlobal->nofPage = ((NNshGlobal->totalLine) / (nlines - 1));
        if (NNshGlobal->nofPage != 0)
        {
            NNshGlobal->nofPage--;

            if (((NNshGlobal->totalLine) % (nlines - 1)) > 1)
            {
                //  スクロールを考慮すると、1ページに実質(nlines - 1)行
                // 表示できるが、最初のページだけは nlines行表示できるため、
                // その部分を考慮してあまりページを求める。
                (NNshGlobal->nofPage)++;
            }
        }
        ptr = top;
    }

    // メッセージの最後から表示する指示があった場合
    if (isBottom != 0)
    {
        // レスを末尾ページから表示するように
        NNshGlobal->currentPage = NNshGlobal->nofPage;
        NNshGlobal->prevHRLines = nlines;
    }

    // 表示開始ラインの計算
    if (NNshGlobal->currentPage == 0)
    {
        // 最初のページ
        startLine = 0;
    }
    else
    {
        // 2ページ目以降の計算
        startLine = NNshGlobal->currentPage * 
                       (NNshGlobal->prevHRLines - 1); /* winDown or winUp */
    }

    // 表示可能行数を保存
    NNshGlobal->prevHRLines = nlines;

    // 表示するページの行の先頭まで読み飛ばす
    line = 0;

    lineLen = StrLen(ptr);
#ifdef USE_CLIE
    // ハイレゾ描画モードでかつ、CLIE NXのとき
    if ((NNshGlobal->hrRef != 0)&&
        (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE))
    {
        while((lineLen > 0)&&(line != startLine))
        {
            len = HRFntWordWrap(NNshGlobal->hrRef, ptr, dimF.extent.x);
            line++;
            ptr = ptr + len;
            lineLen -= len;
        }
    }
    else
#endif
    {
        while((lineLen > 0)&&(line != startLine))
        {
            len = FntWordWrap(ptr, dimF.extent.x);
            line++;
            ptr = ptr + len;
            lineLen -= len;
        }
    }
 
    // 画面に文字を書いていく(表示する文字があり、かつ数えたライン内まで表示)
    i       = 0;
/*     lineLen = StrLen(ptr); */
    while ((lineLen > 0)&&(i < nlines))
    {
#ifdef USE_CLIE
        // ハイレゾ描画モードでかつ、CLIE NXのとき
        if ((NNshGlobal->hrRef != 0)&&
            (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE))
        {
            len = HRFntWordWrap(NNshGlobal->hrRef, ptr, dimF.extent.x);
        }
        else
#endif
        {
            len = FntWordWrap(ptr, dimF.extent.x);
        }

#ifdef USE_CLIE
        if (NNshGlobal->hrRef != 0)
        {
            if (lineLen >= len)
            {
                // 行末まで文字を表示する場合
                if ((*(ptr + (len - 3)) == '\x0a')&&
                    (*(ptr + (len - 2)) == '\x0d')&&
                    (*(ptr + (len - 1)) == '\x0a'))
                {
                    // 行末が改行コードだった場合には、改行コードを表示しない
                    HRWinDrawChars(NNshGlobal->hrRef, ptr, (len - 3),
                                   dimF.topLeft.x, dimF.topLeft.y);
                }
                else if ((*(ptr + (len - 2)) == '\x0d')&&
                         (*(ptr + (len - 1)) == '\x0a'))
                {
                    // 行末が改行コードだった場合には、改行コードを表示しない
                    HRWinDrawChars(NNshGlobal->hrRef, ptr, (len - 2),
                                   dimF.topLeft.x, dimF.topLeft.y);
                }
                else if (*(ptr + (len - 1)) == '\x0a')
                {
                    // 行末が改行コードだった場合には、改行コードを表示しない
                    HRWinDrawChars(NNshGlobal->hrRef, ptr, (len -1),
                                   dimF.topLeft.x, dimF.topLeft.y);
                }
                else
                {
                    HRWinDrawChars(NNshGlobal->hrRef, ptr, len,
                                   dimF.topLeft.x, dimF.topLeft.y);
                }
            }
            else
            {
                // ワードラップで行末まで表示しない場合
                HRWinDrawChars(NNshGlobal->hrRef, ptr, lineLen,
                               dimF.topLeft.x,dimF.topLeft.y);
            }
        }
        else
#endif
        {
            if (lineLen >= len)
            {
                // 行末まで文字を表示する場合
                if ((*(ptr + (len - 2)) == '\x0d')&&
                    (*(ptr + (len - 1)) == '\x0a'))
                {
                    // 行末が改行コードだった場合には、改行コードを表示しない
                    WinDrawChars(ptr,(len - 2),dimF.topLeft.x,dimF.topLeft.y);
                }
                else if (*(ptr + (len - 1)) == '\x0a')
                {
                    // 行末が改行コードだった場合には、改行コードを表示しない
                    WinDrawChars(ptr,(len -1),dimF.topLeft.x,dimF.topLeft.y);
                }
                else
                {
                    WinDrawChars(ptr, len, dimF.topLeft.x, dimF.topLeft.y);
                }
            }
            else
            {
                // ワードラップで行末まで表示しない場合
                WinDrawChars(ptr, lineLen,dimF.topLeft.x,dimF.topLeft.y);
            }
        }

        // 次の行に描画位置を移動させる
        dimF.topLeft.y = dimF.topLeft.y + fontHeight;
        
        // 次の行に表示する文字へすすむ
        i++;
        ptr     = ptr + len;
        lineLen = StrLen(ptr);
    }
    MemHandleUnlock(txtH);

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
#endif  // #ifdef USE_HIGHDENSITY
    return;

END_FUNC:
    // 通常のフィールド表示をして終了する
    FldDrawField(fldP);
    return;
}
#endif
#if 0
/*-------------------------------------------------------------------------*/
/*   Function :  NNsi_HRFieldPageScroll                                    */
/*                              SONYハイレゾ描画モードでのページスクロール */
/*-------------------------------------------------------------------------*/
static Boolean NNsi_HRFieldPageScroll(UInt16 fldID, UInt16 sclID,
                                      UInt16 lines, WinDirectionType direction)
{
    FormType  *frm;
    FieldType *fldP;

    // ページ更新の方向を確認
    if (direction == winDown)
    {
        // １ページ上
        if (NNshGlobal->currentPage >= NNshGlobal->nofPage)
        {
            // 上いっぱい、表示は移動しない
            return (false);
        }
        (NNshGlobal->currentPage)++;
    }
    else
    {
        // １ページ下
        if (NNshGlobal->currentPage == 0)
        {
            // 下いっぱい、表示は移動しない
            return (false);
        }
        (NNshGlobal->currentPage)--;
    }

    // オブジェクトの取得
    frm  = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    // ハイレゾ描画
    NNsi_HRFldDrawField(fldP, false);

    // スクロールバーの表示更新
    NNsi_HRSclUpdate(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, sclID)));

    return (true);
}
#endif

/*=========================================================================*/
/*   Function :   NNshWinClearList                                         */
/*                                                     リストデータを消去  */
/*=========================================================================*/
void NNshWinClearList(FormType *frm, UInt16 lstID)
{
    ListType  *lstP;

    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstID));
    if (lstP != NULL)
    {
        LstEraseList(lstP);
    }
    return;
}

/*=========================================================================*/
/*   Function :   NNshWinSetListItems                                      */
/*                                                   リストにデータを設定  */
/*=========================================================================*/
void NNshWinSetListItems(FormType *frm, UInt16 lstID, Char *list,
                         UInt16 cnt, UInt16 selItem, 
                         MemHandle *memH, Char **listP)
{
    Char      *ptr;
    MemHandle  oldMemH;
    ListType  *lstP;

    if (cnt == 0)
    {
        // リストのアイテム数がゼロのとき、リスト生成は行わない。
        return;
    }
    else if (cnt == 1)
    {
        // リストが１つしかない場合、選択アイテム数を１に限定する。
        selItem = 0;
    }
    else if (cnt <= selItem)
    {
        // アイテム選択アイテム数が異常な場合、末尾にフォーカスをあわせる
        selItem = cnt - 1;
    }

    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstID));
    if (lstP == NULL)
    {
        return;
    }

    oldMemH = *memH;
    *memH   = SysFormPointerArrayToStrings(list, cnt);
    if (*memH == 0)
    {
        return;
    }

    ptr = MemHandleLock(*memH);
    LstSetListChoices(lstP, (Char **) ptr, cnt);
    LstSetSelection  (lstP, selItem);
    LstSetTopItem    (lstP, selItem);

    LstDrawList(lstP);
    if (oldMemH != 0)
    {
        MemHandleUnlock(oldMemH);
        MemHandleFree(oldMemH);
    }
    if (listP != NULL)
    {
        *listP = ptr;
    }
    return;
}

/*=========================================================================*/
/*   Function :   NNshWinUpdateListItems                                   */
/*                                   リストにデータを設定(リスト構築済み)  */
/*=========================================================================*/
void NNshWinUpdateListItems(FormType *frm, UInt16 lstID, Char *ptr,
                            UInt16 cnt, UInt16 selItem)
{
    ListType  *lstP;

    if (cnt == 0)
    {
        // リストのアイテム数がゼロのとき、リスト生成は行わない。
        return;
    }
    else if (cnt == 1)
    {
        // リストが１つしかない場合、選択アイテム数を１に限定する。
        selItem = 0;
    }
    else if (cnt <= selItem)
    {
        // アイテム選択アイテム数が異常な場合、末尾にフォーカスをあわせる
        selItem = cnt - 1;
    }

    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstID));
    if (lstP == NULL)
    {
        return;
    }

    LstSetListChoices(lstP, (Char **) ptr, cnt);
    LstSetSelection  (lstP, selItem);
    LstSetTopItem    (lstP, selItem);
    LstDrawList(lstP);

    return;
}



/*=========================================================================*/
/*   Function :  NNshWinSetFieldText                                       */
/*                                                フィールドテキストの更新 */
/*=========================================================================*/
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Boolean redraw, 
                         Char *msg, UInt32 size)
{
    MemHandle  txtH, oldTxtH;
    Char      *txtP;
    FieldType *fldP;

    // メッセージを表示領域にコピーする
    txtH = MemHandleNew(size + MARGIN);
    if (txtH == 0)
    {
        // 領域の確保に失敗
        return;
    }
    txtP = (Char *) MemHandleLock(txtH);
    if (txtP == NULL)
    {
        // 領域の確保に失敗
        MemHandleFree(txtH);
        return;
    }
    (void) MemSet(txtP,  size + MARGIN, 0x00);
    if (size != 0)
    {
        (void) StrNCopy(txtP, msg, size);
    }
    MemHandleUnlock(txtH);

    // フィールドのオブジェクトポインタを取得
    fldP = (FieldType *) FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    // フィールドのテキストハンドルを入れ替え、古いのを削除する
    oldTxtH = FldGetTextHandle(fldP);
    FldSetTextHandle(fldP, NULL);
    FldSetTextHandle(fldP, txtH);
    if (oldTxtH != 0)
    {
        (void) MemHandleFree(oldTxtH);
    }

    // フォームの再描画
    NNsi_FrmDrawForm(frm, redraw);
    return;
}


/*=========================================================================*/
/*   Function : NNshWinSetPopItemsWithList                                 */
/*                    ポップアップトリガのラベル設定(リストもあわせて設定) */
/*=========================================================================*/
void NNshWinSetPopItemsWithList(FormType *frm, UInt16 popId, UInt16 lstId, 
                                NNshWordList *wordList, UInt16 item)
{
    ListType    *lstP;
    ControlType *ctlP;
    Char        *ptr;

    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, popId));
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstId));
    if ((lstP == NULL)||(wordList == NULL)||(wordList->nofWord == 0)||
        (wordList->wordString == NULL)||(ctlP == NULL))
    {
        // リスト指定でエラーになりそうな状態は排除する
        return;
    }

    if (wordList->nofWord < item)
    {
        // 選択番号がハミ出していた場合には、先頭にあわせる
        item = 0;
    }

    ptr = MemHandleLock(wordList->wordmemH);
    LstSetListChoices(lstP, (Char **) ptr, wordList->nofWord);

    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstId));

    LstSetTopItem    (lstP, item);
    LstSetSelection  (lstP, item);
    CtlSetLabel      (ctlP, LstGetSelectionText(lstP, item));

    return;
}

/*=========================================================================*/
/*   Function : NNshWinSetPopItemsWithListArray                            */
/*                ポップアップトリガのラベル連続設定(リストもあわせて設定) */
/*=========================================================================*/
void NNshWinSetPopItemsWithListArray(FormType *frm, NNshListItem *target,
                                     Char *listArray, UInt16 num, UInt16 *items)
{
    ListType    *lstP;
    ControlType *ctlP;

    while ((target != NULL)&&(target->popId != 0)&&(target->lstId != 0))
    {
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, target->popId));
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, target->lstId));

        if (num < *items)
        {
            // 選択番号がハミ出していた場合には、先頭にあわせる
            *items = 0;
        }
        LstSetListChoices(lstP, (Char **) listArray, num);
        LstSetTopItem    (lstP, *items);
        LstSetSelection  (lstP, *items);
        CtlSetLabel      (ctlP, LstGetSelectionText(lstP, *items));

        // 次の設定データに移動する
        target++;
        items++;
    }
    return;
}


/*=========================================================================*/
/*   Function : NNshWinSetPopItems                                         */
/*                                          ポップアップトリガのラベル設定 */
/*=========================================================================*/
void NNshWinSetPopItems(FormType *frm, UInt16 popId, UInt16 lstId, UInt16 item)
{
    ListType    *lstP;
    ControlType *ctlP;

    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, popId));
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, lstId));
                          
    LstSetTopItem  (lstP, item);
    LstSetSelection(lstP, item);
    CtlSetLabel    (ctlP, LstGetSelectionText(lstP, item));

    return;
}

/*=========================================================================*/
/*   Function :  NNshWinGetFieldText                                       */
/*                                                フィールドテキストの取得 */
/*=========================================================================*/
void NNshWinGetFieldText(FormType *frm, UInt16 fldID, Char *area, UInt32 len)
{
    MemHandle  txtH;
    Char      *txtP;
    FieldType *fldP;

    // フィールド格納領域を初期化する
    MemSet(area, len, 0x00);

    // フィールドのオブジェクトポインタを取得
    fldP = (FieldType *) FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    // フィールドのテキストハンドルを取得
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        if (txtP != NULL)
        {
            StrNCopy(area, txtP, (len - 1));
            MemHandleUnlock(txtH);
        }
    }
    return;
}


/*=========================================================================*/
/*   Function : SetMsg_BusyForm                                            */
/*                                                     BUSYテキストの設定  */
/*=========================================================================*/
void SetMsg_BusyForm(Char *msg)
{
#ifdef USE_ORIGINAL_WINDOW
    FormType *busyFrm;
    UInt16    length;

    // 表示する文字列長のチェック
    length = StrLen(msg);
    length = (length > NNSH_BUSYWIN_MAXLEN) ? NNSH_BUSYWIN_MAXLEN : length;

    busyFrm = FrmGetActiveForm();
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, true, msg, length);
    return;
#else  // #ifdef USE_ORIGINAL_WINDOW
    UInt16         len;
    RectangleType  r;

#ifdef USE_COLOR
    RGBColorType  color;
#endif  // #ifdef USE_COLOR
    
    // 画面中央部を消去する
    r.topLeft.x = 8;
    r.topLeft.y = 70;
    r.extent.x  = 140;
    r.extent.y  = 22;

    // 画面を消す
    WinEraseRectangle(&r, 0);

#ifdef USE_COLOR
    // OS 3.5以上の場合
    if (NNshGlobal->palmOSVersion >= 0x03503000)
    {
        // 色状態を保存
        WinPushDrawState();

        // 文字は黒
        MemSet(&color, sizeof(color), 0x00);
        WinSetTextColor(WinRGBToIndex(&color));

        // 枠は青
        color.b = 140;
        WinSetForeColor(WinRGBToIndex(&color));
    
        // 枠を表示
        WinDrawRectangleFrame(dialogFrame, &r);

        // 文字列を表示する
        len = FntWordWrap(msg, (r.extent.x - (MARGIN * 2)));
        WinDrawChars(msg, len, (r.topLeft.x + MARGIN), (r.topLeft.y + MARGIN - 1));

        // 色状態を復帰
        WinPopDrawState();
    }
    else
#else  // #ifdef USE_COLOR
    {
        // 枠を表示
        WinDrawRectangleFrame(dialogFrame, &r);

        // 文字列を表示する
        len = FntWordWrap(msg, (r.extent.x - (MARGIN * 2)));
        WinDrawChars(msg, len, (r.topLeft.x + MARGIN), (r.topLeft.y + MARGIN - 1));
    }
#endif // #ifdef USE_COLOR
    return;
#endif // #ifdef USE_ORIGINAL_WINDOW
}

/*=========================================================================*/
/*   Function : Show_BusyForm                                              */
/*                                                      BUSYフォームの表示 */
/*=========================================================================*/
void Show_BusyForm(Char *msg)
{
#ifdef USE_ORIGINAL_WINDOW
    FormType  *busyFrm;
    UInt16    length;

    // すでにBUSYウィンドウが表示されていた場合
    if (FrmGetActiveFormID() == FRMID_BUSY)
    {
        NNsh_DebugMessage(ALTID_ERROR, "ALREADY SHOW BUSY WINDOW!", "", 0);
        return (SetMsg_BusyForm(msg));
    }

    // 現在のフォームを保存
    NNshGlobal->prevBusyForm = FrmGetActiveForm();

    // BUSYダイアログフォームを生成、アクティブにする
    busyFrm = FrmInitForm(FRMID_BUSY);
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(busyFrm, vgaFormModify160To240);
    }
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
    if (NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)
    {
        VgaFormModify_DIA(busyFrm, vgaFormModifyNormalToWide);
        PINSetInputAreaState(pinInputAreaClosed);  // SILK最小化...いいのかな？
    }
#endif // #ifdef USE_PIN_DIA
    FrmSetActiveForm(busyFrm);

    // 表示する文字列長のチェック
    length = StrLen(msg);
    length = (length > NNSH_BUSYWIN_MAXLEN) ? NNSH_BUSYWIN_MAXLEN : length;

    // フィールドに文字列を設定
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, true, msg, length);
    return;
#else  // #ifdef USE_ORIGINAL_WINDOW
    // 文字列表示パターン
    SetMsg_BusyForm(msg);
    return;
#endif // #ifdef USE_ORIGINAL_WINDOW
}

/*=========================================================================*/
/*   Function : Hide_BusyForm                                              */
/*                                                      BUSYフォームの削除 */
/*=========================================================================*/
void Hide_BusyForm(Boolean redraw)
{
#ifdef USE_ORIGINAL_WINDOW
    FormType *busyFrm;

    // BUSYフォームを削除し、BUSYフォームの前に表示していたフォームを開く
    if (NNshGlobal->prevBusyForm != NULL)
    {
        busyFrm = FrmGetActiveForm();
        FrmEraseForm(busyFrm);
        FrmSetActiveForm(NNshGlobal->prevBusyForm);
        NNsi_FrmDrawForm(NNshGlobal->prevBusyForm, redraw);
        FrmDeleteForm   (busyFrm);
    }
    NNshGlobal->prevBusyForm = NULL;

    return;
#else  // #ifdef USE_ORIGINAL_WINDOW
    NNsi_FrmDrawForm(FrmGetActiveForm(), redraw);
    return;
#endif // #ifdef USE_ORIGINAL_WINDOW
}

/*=========================================================================*/
/*   Function :   NNsh_DialogMessage                                       */
/*                                             メッセージダイアログの表示  */
/*=========================================================================*/
UInt16 NNsh_DialogMessage(UInt16 level, UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
    Char logBuf[MINIBUF];

    switch (level)
    {
      case NNSH_LEVEL_ERROR:
        // エラーメッセージ(必ず表示する)
        break;

      case NNSH_LEVEL_WARN:
        // 警告メッセージ
        if ((NNshParam->confirmationDisable & (NNSH_OMITDIALOG_WARNING)) != 0)
        {
            // 警告表示をしない場合には、"OK"(== 0)を返す
            return (0);
        }
        break;

      case NNSH_LEVEL_CONFIRM:
        // 確認メッセージ
        if ((NNshParam->confirmationDisable & (NNSH_OMITDIALOG_CONFIRM)) != 0)
        {
            // 確認表示をしない場合には、"OK"(== 0)を返す
            return (0);
        }
        break;

      case NNSH_LEVEL_DEBUG:
#ifdef DEBUG
        // デバッグオプション付きでコンパイルした場合には、必ず表示する。
#else
        if (NNshParam->debugMessageON == 0)
        {
            // DEBUG表示しない場合には、"OK"(== 0)を返す
            return (0);
        }
#endif
        break;

      case NNSH_LEVEL_INFO:
      default:
        // 情報メッセージ
        if ((NNshParam->confirmationDisable & (NNSH_OMITDIALOG_INFORMATION))
                                                                          != 0)
        {
            // 情報表示をしない場合には、"OK"(== 0)を返す
            return (0);
        }
        break;
    }

    // ダイアログを表示する
    MemSet (logBuf, sizeof(logBuf), 0x00);
    if (num != 0)
    {
        StrCopy(logBuf, "(0x");
        NUMCATH(logBuf, num);
        StrCat (logBuf, "/");
        NUMCATI(logBuf, num);
        StrCat (logBuf, ")");
    }
    return (FrmCustomAlert(altID, mes1, mes2, logBuf));
}

// ページスクロール表示：MEMO(SDK 4.0付属)のサンプルから持ってきて変更
/***********************************************************************
 *
 * FUNCTION:    EditViewUpdateScrollBar
 *
 * DESCRIPTION: This routine update the scroll bar.
 *
 * PARAMETERS:
 *              fldP      field object
 *              barP      scroll bar object
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *
 *  Name        Date                Description
 *  ----        ----                -----------
 *   art        07/01/96        Initial Revision
 *   gap        11/02/96        Fix case where field and scroll bars
 *                              get out of sync.
 *  nnsh        25/04/02        tunes for nanashi.
 ***********************************************************************/
void NNshWinViewUpdateScrollBar(UInt16 fldID, UInt16 sclID)
{
    FormType *frm;
    void     *fldP, *barP;
    UInt16    scrollPos;
    UInt16    textHeight;
    UInt16    fieldHeight;
    Int16     maxValue;

    frm  = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));
    barP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, sclID));

    FldGetScrollValues(fldP, &scrollPos, &textHeight, &fieldHeight);
    if (textHeight > fieldHeight)
    {
       // On occasion, such as after deleting a multi-line selection of text,
       // the display might be the last few lines of a field followed by some
       // blank lines. To keep the current position in place and allow the user
       // to "gracefully" scroll out of the blank area, the number of blank
       // lines visible needs to be added to max value.  Otherwise the scroll
       // position may be greater than maxValue, get pinned to maxvalue
       // in SclSetScrollBar resulting in the scroll bar and the display being
       // out of sync.
       maxValue = (textHeight - fieldHeight) + FldGetNumberOfBlankLines(fldP);
    }
    else
    {
        // １画面に表示できる場合、スクロールバーを表示しないよう細工する
        maxValue = (scrollPos == 0) ? 0 : scrollPos;
    }
    SclSetScrollBar (barP, scrollPos, 0, maxValue, fieldHeight - 1);

    return;
}

// ページスクロール制御：MEMO(SDK 4.0付属)のサンプルから持ってきて変更
/***********************************************************************
 *
 * FUNCTION:    EditViewPageScroll
 *
 * DESCRIPTION: This routine scrolls the message a page winUp or winDown.
 *              When the top of a memo is visible, scrolling up will
 *              display the display the botton of the previous memo.
 *              If the bottom of a memo is visible, scrolling down will
 *              display the top of the next memo.
 *
 * PARAMETERS:
 *              fldP          field object
 *              sclP          scroll object
 *              direction     winUp or winDown
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *  Name      Date        Description
 *  ----      -------        -----------
 *  art       7/1/96        Initial Revision
 *  grant     2/4/99        Use EditViewScroll() to do actual scrolling.
 *  nnsh     25/4/02    tunes for nanashi.
 ***********************************************************************/
Boolean NNshWinViewPageScroll(UInt16 fldID, UInt16 sclID, 
                              UInt16 lines, WinDirectionType direction)
{
    FormType *frm;
    void     *fldP, *sclP;
    UInt16    linesToScroll;
    Int16     value, min, max, pageSize;
    Boolean   ret;

#if 0
    //  グラフィック描画モードで、メッセージ参照時にフィールド更新の
    // 場合には別関数で処理する。
    if (NNshParam->useSonyTinyFont != 0)
    {
        return (NNsi_HRFieldPageScroll(fldID, sclID, lines, direction));
    }
#endif

    // オブジェクトの取得
    frm  = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    if (sclID == 0)
    {
        // フィールドをスクロール
        FldScrollField(fldP, lines, direction);
        return (true);
    }

    // スクロール行数が指定されているか確認
    if (lines == 0)
    {
        // スクロール行数が指定されていない場合、スクロール可能かチェックする
        ret = FldScrollable(fldP, direction);
        if (ret != true)
        {
            return (ret);
        }

        // スクロールする行数を取得(表示行数 - 1)
        linesToScroll = FldGetVisibleLines(fldP) - 1;

        // スクロールバーの更新
        sclP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, sclID));
        SclGetScrollBar(sclP, &value, &min, &max, &pageSize);  
        if (direction == winUp)
        {
            value = value - linesToScroll;
        }
        else
        {
            value = value + linesToScroll;
        }
        SclSetScrollBar(sclP, value, min, max, pageSize);
    }
    else
    {
        // 指定された行数をスクロールバーの行数に割り当てる
        linesToScroll = lines;
        ret = true;
    }

    // フィールドをスクロール
    FldScrollField(fldP, linesToScroll, direction);
    return (ret);
}

/*=========================================================================*/
/*   Function : NNsh_MenuEvt_Edit                                          */
/*                                        編集メニューが選択された時の処理 */
/*=========================================================================*/
Boolean NNsh_MenuEvt_Edit(EventType *event)
{
    FormType  *frm;
    FieldType *fldP;
    UInt16     frmId;
    
    // 選択されている個所がフィールドかどうかチェックする
    frm   = FrmGetActiveForm();
    frmId = FrmGetFocus(frm);
    if (frmId == noFocus)
    { 
        // フォーカスが選択されていないので、何もせず返る
        return (false);
    }
    if (FrmGetObjectType(frm, frmId) != frmFieldObj)
    {
        // 選択個所がフィールドではないので、何もせず返る
        return (false);
    }
    fldP = FrmGetObjectPtr(frm, frmId);

    // 編集メニューを実施
    switch (event->data.menu.itemID)
    {
      case MNUID_EDIT_UNDO:
        FldUndo(fldP);
        break;
      case MNUID_EDIT_CUT:
        FldCut(fldP);
        break;
      case MNUID_EDIT_COPY:
        FldCopy(fldP);
        break;
      case MNUID_EDIT_PASTE:
        FldPaste(fldP);
        break;
      case MNUID_SELECT_ALL:
        FldSetSelection(fldP, 0, FldGetTextLength(fldP));
        break;
      default:
        // なにもしない、というかココには来ない
        break;
    }
    return (false);
}


/*=========================================================================*/
/*   Function :   Hander_EditAction                                        */
/*                                         編集操作専用のイベントハンドラ  */
/*=========================================================================*/
Boolean Handler_EditAction(EventType *event)
{
    UInt16 keyCode;

    // 現在のフォームを取得
    switch (event->eType)
    { 
      // メニュー選択
      case menuEvent:
        return (NNsh_MenuEvt_Edit(event));
        break;
      case keyDownEvent:
        // キーコードによって処理を分岐させる
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          case vchrJogRelease:
          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrThumbWheelUp:
          case vchrThumbWheelDown:
            return (true);
            break;

          case chrCarriageReturn:
          case chrLineFeed:
          case chrLeftArrow:
          case chrEscape:
          default:
            break;
        }
        break;

      case ctlSelectEvent:
      default: 
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Hander_JumpSelection                                     */
/*                                           ジャンプ用のイベントハンドラ  */
/*-------------------------------------------------------------------------*/
Boolean Handler_JumpSelection(EventType *event)
{
    FormType         *frm;
    ListType         *lstP;
    EventType        *dummyEvent;
    UInt16            max, keyCode;

    // ダミーイベント
    dummyEvent = &(NNshGlobal->dummyEvent);

    // 現在のフォームを取得
    switch (event->eType)
    { 
      // リスト選択イベント
      case lstSelectEvent:
        if (NNshGlobal->jumpSelection == event->data.lstSelect.selection)
        {
            // ダブルタップを検出(OKボタンを押したことにする)
            goto JUMP_SELECTION;
        }
        NNshGlobal->jumpSelection = event->data.lstSelect.selection;
        break;

      // キーダウン(ボタン)の選択
      case keyDownEvent:
        // 現在選択されているリストアイテム番号を取得
        frm  = FrmGetActiveForm();
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_JUMPLIST));
        NNshGlobal->jumpSelection = LstGetSelection(lstP);
        max  = LstGetNumberOfItems(lstP);

        //tungsten T 5way navigator (281さん、感謝！)
        keyCode = KeyConvertFiveWayToJogChara(event);

        // キーコードによって処理を分岐させる
        switch (keyCode)
        {
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
          case vchrThumbWheelPush:
            // Jog Push/Enterキーを押されたとき(OKボタンを押したことにする)
            goto JUMP_SELECTION;
            break;

          case chrLeftArrow:
          case chrEscape:
          case chrCapital_X:
          case chrSmall_X:
          case vchrThumbWheelBack:
            // ESCキー/Xキー/5wayの左ボタンが押されたとき
            // (Cancelボタンを押したことにする)
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->data.ctlSelect.pControl  
                = (ControlType*) FrmGetObjectPtr(frm,
                                     FrmGetObjectIndex(frm, BTNID_JUMPCANCEL));
            dummyEvent->data.ctlSelect.controlID = BTNID_JUMPCANCEL;
            dummyEvent->eType                    = ctlSelectEvent;
            EvtAddEventToQueue(dummyEvent);
            return (true);
            break;

          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrNextField:
          case vchrThumbWheelDown:
            // ハードキー下、ジョグ下送り
            if (NNshGlobal->jumpSelection < (max - 1))
            {
                (NNshGlobal->jumpSelection)++;
                LstSetSelection(lstP, NNshGlobal->jumpSelection);
            }
            else
            {
                // 選択をリストの先頭へ移動
                NNshGlobal->jumpSelection = 0;
                LstSetSelection(lstP, NNshGlobal->jumpSelection);
                LstSetTopItem  (lstP, 0);
            }
            return (true);
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrPrevField:
          case vchrThumbWheelUp:
            // ハードキー上、ジョグ上送り
            if (NNshGlobal->jumpSelection > 0)
            {
                (NNshGlobal->jumpSelection)--;
                LstSetSelection(lstP, NNshGlobal->jumpSelection);
            }
            else
            {
                // 選択をリストの末尾へ移動
                NNshGlobal->jumpSelection = max - 1;
                LstSetSelection(lstP, NNshGlobal->jumpSelection);
            }
            return (true);
            break;

          default:
            // 上記以外のキーでは何もしない
            break;

        }
        return (false);
        break;

      // コントロール(ボタン)の選択
      case ctlSelectEvent:
        // 何もしない
        break;

      default: 
        // 何もしない
        break;
    }
    return (false);

JUMP_SELECTION:
    // OKボタンを押したことにする
    frm = FrmGetActiveForm();
    MemSet(dummyEvent, sizeof(EventType), 0x00);
    dummyEvent->data.ctlSelect.pControl  
                = (ControlType*) FrmGetObjectPtr(frm,
                                       FrmGetObjectIndex(frm, BTNID_JUMPEXEC));
    dummyEvent->data.ctlSelect.controlID = BTNID_JUMPEXEC;
    dummyEvent->eType                    = ctlSelectEvent;
    EvtAddEventToQueue(dummyEvent);
    return (true);
}

/*=========================================================================*/
/*   Function :  NNshWinSelectionWindow                                    */
/*                                                    選択ウィンドウの表示 */
/*=========================================================================*/
UInt16 NNshWinSelectionWindow(UInt16 frmId, Char *listItemP, UInt16 nofItems, UInt16 selection)
{
    FormType  *prevFrm, *diagFrm;
    UInt16     btnId;
    MemHandle  listH;
    ListType  *lstP;

    // アイテム数の確認(ゼロだったら、何もせず戻る)
    if (nofItems == 0)
    {
        // キャンセルされたのと同等の応答コードを返す
        return (BTNID_JUMPCANCEL);
    }

    // 初期化
    listH   = 0;
    btnId   = BTNID_JUMPCANCEL;
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // 選択番号が異常だったら、先頭の番号にする
    if (nofItems <= selection)
    {
        selection = 0;
    }

    diagFrm  = FrmInitForm(frmId);
    if (diagFrm != NULL)
    {
        // フォームをアクティブにする
        FrmSetActiveForm(diagFrm);

        // リストの設定
        lstP  = FrmGetObjectPtr(diagFrm,
                                FrmGetObjectIndex(diagFrm, LSTID_JUMPLIST));
        listH = SysFormPointerArrayToStrings(listItemP, nofItems);
        if (listH != 0)
        {
            LstSetListChoices(lstP, (Char **) MemHandleLock(listH), nofItems);
            LstSetSelection  (lstP, selection);
            LstSetTopItem    (lstP, selection);
        }

        // イベントハンドラの設定
        FrmSetEventHandler(diagFrm, Handler_JumpSelection);

        // ジャンプ先をクリア
        NNshGlobal->jumpSelection = 0;

        // ダイアログのオープン、文字列の入力を待つ
        btnId = FrmDoDialog(diagFrm);

        // ダイアログを閉じる
        FrmSetActiveForm(prevFrm);
        FrmDeleteForm(diagFrm);
    }

    if (listH != 0)
    {
        MemHandleUnlock(listH);
        MEMFREE_HANDLE (listH);
    }
    // FrmEraseForm(prevFrm);
    // FrmDrawForm(prevFrm);
    return (btnId);
}

/*=========================================================================*/
/*   Function : DataInputDialog                                            */
/*                                                NNsi共通の入力ダイアログ */
/*=========================================================================*/
Boolean DataInputDialog(Char *title, Char *area, UInt16 size, UInt16 usage, void *valueP)
{
    Boolean       ret = false;
    UInt16        btnId;
    FormType     *prevFrm, *diagFrm;
    FieldType    *fldP;

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // ダイアログフォームをアクティブにする
    diagFrm  = FrmInitForm(FRMID_INPUTDIALOG);
    if (diagFrm == NULL)
    {
        return (false);
    }
    FrmSetActiveForm(diagFrm);

    // シルク表示を最大化
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(diagFrm, vgaFormModify160To240);
        SilkMaximizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMaximize(diagFrm);
    }
    HandEraMoveDialog(diagFrm);

    // タイトルをコピーする
    FrmCopyTitle(diagFrm, title);

    // 指定された文字列をウィンドウに反映させる
    NNshWinSetFieldText(diagFrm, FLDID_INPUTFIELD, true, area, size);
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_INPUTFIELD));

    fldP = FrmGetObjectPtr(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_INPUTFIELD));
    // 文字列を選択しておく
    FldSetSelection(fldP, 0, FldGetTextLength(fldP));

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_EditAction);

    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_DIALOG_OK)
    {
        // OKボタンが押されたときには、文字列を取得する
        NNshWinGetFieldText(diagFrm, FLDID_INPUTFIELD, area, size);
        ret = true;
        NNsh_DebugMessage(ALTID_INFO,"String:", area, 0);
    }
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // シルク表示を最小化
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMinimize(prevFrm);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   SetControlValue                                          */
/*                                       パラメータから画面に反映する処理  */
/*=========================================================================*/
void SetControlValue(FormType *frm, UInt16 objID, UInt16 *value)
{
    ControlType *chkObj;

    chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objID));
    CtlSetValue(chkObj, (UInt16) *value);

    return;
}

/*=========================================================================*/
/*   Function :   UpdateParameter                                          */
/*                                       画面からパラメータに反映する処理  */
/*=========================================================================*/
void UpdateParameter(FormType *frm, UInt16 objID, UInt16 *value)
{
    ControlType *chkObj;

    chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objID));
    *value = CtlGetValue(chkObj);

    return;
}
/*=========================================================================*/
/*   Function :  PrepareDialog_NNsh                                        */
/*                                              ダイアログ表示準備設定項目 */
/*=========================================================================*/
void PrepareDialog_NNsh(UInt16 formID, FormType **diagFrm, Boolean silk)
{
    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(FrmGetActiveForm());
    }

    // ダイアログフォームをアクティブにする
    *diagFrm  = FrmInitForm(formID);
    if (*diagFrm == NULL)
    {
        return;
    }
    FrmSetActiveForm(*diagFrm);

#ifdef USE_DEBUG_ROM
    FrmDrawForm(*diagFrm);
#endif

    // シルク制御をしない場合には終了する
    if (silk != true)
    {
        return;
    }

    // シルク表示を最大化
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(*diagFrm, vgaFormModify160To240);
        SilkMaximizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMaximize(*diagFrm);
    }
    HandEraMoveDialog(*diagFrm);

    return;
}

/*=========================================================================*/
/*   Function :  PrologueDialog_NNsh                                       */
/*                                        設定項目(モーダルフォーム)の表示 */
/*=========================================================================*/
void PrologueDialog_NNsh(FormType *prevFrm, FormType *diagFrm)
{
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // シルク表示を最小化
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMinimize(prevFrm);
    }
    return;
}
