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
/*   Function : NNsi_FrmDrawForm                                           */
/*                                                        フォームの再描画 */
/*=========================================================================*/
void NNsi_FrmDrawForm(FormType *frm)
{
    UInt16     formId;
    FieldType *fldP;

    // ハイレゾ表示モードだった場合は、さらに画面更新
    if (NNshParam->useSonyTinyFont != 0)
    {
        // フォームのＩＤを取得する
        formId = FrmGetActiveFormID();
        if (formId != FRMID_MESSAGE)
        {
            // スレ参照画面でなければ抜ける。
            goto FUNC_END;
	}

        // メッセージ表示領域は非表示にして画面を再描画する
        fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        FldSetUsable(fldP, false);
#ifdef USE_CLIE
        if (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY)
        {
            // CLIEでシルクのサイズが変更されたときには一旦フォームを消去
            FrmEraseForm(frm);
        }
#endif
        FrmDrawForm(frm);
        if (NNshGlobal->totalLine != 0)
        {
            // フィールドの描画(ハイレゾ画面)
            NNsi_HRFldDrawField(fldP);

            // スクロールバーの更新
            NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                             FrmGetObjectIndex(frm, SCLID_MESSAGE)));
        }
        return;
    }

FUNC_END:
    // フォームを(普通に)描画する
    FrmDrawForm(frm);
    return;
}

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
void NNsi_HRFldDrawField(FieldType *fldP)
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

    // フォントの設定
#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        // 単純に２倍にして良いのかな？ (あとで検討...)
        dimF.extent.x   = dimF.extent.x  * 2;
        dimF.extent.y   = dimF.extent.y  * 2;
        dimF.topLeft.x  = dimF.topLeft.x * 2;
        dimF.topLeft.y  = dimF.topLeft.y * 2;

        // 表示領域をクリアする
        HRWinEraseRectangle(NNshGlobal->hrRef, &dimF, 0);

        // 描画フォントの設定
        fontID = HRFntSetFont(NNshGlobal->hrRef, NNshParam->sonyHRFont);

        // CLIE NX以降では、フォントの高さを求める関数が異なるので注意
        if (NNshGlobal->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
        {
            fontHeight   = FntCharHeight();
        }
        else
        {
            fontHeight = HRFntCharHeight(NNshGlobal->hrRef);
        }
    }
    else
#endif
    {
        // 表示領域をクリアして、描画フォントを設定する
        WinEraseRectangle(&dimF, 0);
        fontID       = FntSetFont(NNshParam->sonyHRFont);
        fontHeight   = FntCharHeight();
    }

    // 表示可能行数(nlines)を計算
    nlines = dimF.extent.y / fontHeight;
        
    // 表示する行数が未計算？
    if (NNshGlobal->totalLine == 0)
    {
        NNshGlobal->currentPage = 0;
        NNshGlobal->prevHRLines = nlines;

#ifdef USE_CLIE
        // ハイレゾ描画モードでかつ、CLIE NXのとき
        if ((NNshGlobal->hrRef != 0)&&
            (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE))
        {

            // 表示するメッセージの行数を数え、gNumTotalLineに格納する
            while((StrLen(ptr) > 0))
            {
                len = HRFntWordWrap(NNshGlobal->hrRef, ptr, dimF.extent.x);
                (NNshGlobal->totalLine)++;
                ptr = ptr + len;
            }
        }
        else
#endif
        {
            // 表示するメッセージの行数を数え、gNumTotalLineに格納する
            while((StrLen(ptr) > 0))
            {
                len = FntWordWrap(ptr, dimF.extent.x);
                (NNshGlobal->totalLine)++;
                ptr = ptr + len;
            }
        }

        // メッセージの総ページ数を計算し、nofPageに格納する
        NNshGlobal->nofPage = (NNshGlobal->totalLine / (nlines - 1));
        ptr = top;
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

#ifdef USE_CLIE
    // ハイレゾ描画モードでかつ、CLIE NXのとき
    if ((NNshGlobal->hrRef != 0)&&
        (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE))
    {
        while((StrLen(ptr) > 0)&&(line != startLine))
        {
            len = HRFntWordWrap(NNshGlobal->hrRef, ptr, dimF.extent.x);
            line++;
            ptr = ptr + len;
        }
    }
    else
#endif
    {
        while((StrLen(ptr) > 0)&&(line != startLine))
        {
            len = FntWordWrap(ptr, dimF.extent.x);
            line++;
            ptr = ptr + len;
        }
    }
 
    // 画面に文字を書いていく(表示する文字があり、かつ数えたライン内まで表示)
    i       = 0;
    lineLen = StrLen(ptr);
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

    return;

END_FUNC:
    // 通常のフィールド表示をして終了する
    FldDrawField(fldP);
    return;
}

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
    NNsi_HRFldDrawField(fldP);

    // スクロールバーの表示更新
    NNsi_HRSclUpdate(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, sclID)));

    return (true);
}

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
                         UInt16 cnt, UInt16 selItem, MemHandle *memH)
{
    MemHandle oldMemH;
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
    LstSetListChoices(lstP, (Char **) MemHandleLock(*memH), cnt);
    LstSetSelection  (lstP, selItem);
    LstSetTopItem    (lstP, selItem);

    LstDrawList(lstP);

    if (oldMemH != 0)
    {
        MemHandleUnlock(oldMemH);
        MemHandleFree(oldMemH);
    }
    return;
}

/*=========================================================================*/
/*   Function :  NNshWinSetFieldText                                       */
/*                                                フィールドテキストの更新 */
/*=========================================================================*/
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Char *msg, UInt32 size)
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
    NNsi_FrmDrawForm(frm);
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
    FormType *busyFrm;

    busyFrm = FrmGetActiveForm();
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, msg, StrLen(msg));
    return;
}

/*=========================================================================*/
/*   Function : Show_BusyForm                                              */
/*                                                      BUSYフォームの表示 */
/*=========================================================================*/
void Show_BusyForm(Char *msg)
{
    FormType  *busyFrm;

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
    FrmSetActiveForm(busyFrm);

    // フィールドに文字列を設定
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, msg, StrLen(msg));
    return;
}

/*=========================================================================*/
/*   Function : Hide_BusyForm                                              */
/*                                                      BUSYフォームの削除 */
/*=========================================================================*/
void Hide_BusyForm(void)
{
    FormType *busyFrm;

    // BUSYフォームを削除し、BUSYフォームの前に表示していたフォームを開く
    if (NNshGlobal->prevBusyForm != NULL)
    {
        busyFrm = FrmGetActiveForm();
        FrmEraseForm(busyFrm);
        FrmSetActiveForm(NNshGlobal->prevBusyForm);
        NNsi_FrmDrawForm(NNshGlobal->prevBusyForm);
        FrmDeleteForm   (busyFrm);
    }
    NNshGlobal->prevBusyForm = NULL;

    return;
}

/*=========================================================================*/
/*   Function :   NNsh_DebugMessage                                        */
/*                                                デバッグメッセージの表示 */
/*=========================================================================*/
void NNsh_DebugMessage(UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
#ifdef DEBUG
    // デバッグオプション付きでコンパイルした場合には、必ず表示する。
#else
    if (NNshParam->debugMessageON != 0)
#endif
    {
        NNsh_ErrorMessage(altID, mes1, mes2, num);
    }
    return;
}

/*=========================================================================*/
/*   Function :   NNsh_ErrorMessage                                        */
/*                                                       メッセージの表示  */
/*=========================================================================*/
UInt16 NNsh_ErrorMessage(UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
    Char logBuf[MINIBUF];
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

/*=========================================================================*/
/*   Function :   NNsh_ConfirmMessage                                      */
/*                                                   確認メッセージの表示  */
/*=========================================================================*/
UInt16 NNsh_ConfirmMessage(UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
    if (NNshParam->confirmationDisable != 0)
    {
        // 確認表示をしない場合には、"OK"(== 0)を返す
        return (0);
    }
    return (NNsh_ErrorMessage(altID, mes1, mes2, num));
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
 *  Name	Date		Description
 *  ----	----		-----------
 *   art	07/01/96	Initial Revision
 *   gap	11/02/96	Fix case where field and scroll bars
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
 *  Name      Date	Description
 *  ----      -------	-----------
 *  art       7/1/96	Initial Revision
 *  grant     2/4/99	Use EditViewScroll() to do actual scrolling.
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

    //  グラフィック描画モードで、メッセージ参照時にフィールド更新の
    // 場合には別関数で処理する。
    if ((NNshParam->useSonyTinyFont != 0)&&(fldID == FLDID_MESSAGE))
    {
        return (NNsi_HRFieldPageScroll(fldID, sclID, lines, direction));
    }

    // オブジェクトの取得
    frm  = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldID));

    if (sclID == 0)
    {
        // フィールドをスクロール
        FldScrollField(fldP, lines, direction);
        return (true);
    }

    // スクロール可能かチェックする
    ret = FldScrollable(fldP, direction);
    if (ret == true)
    {
        // スクロールする行数を取得(表示行数 - 1)
        linesToScroll = FldGetVisibleLines(fldP) - 1;

        // フィールドをスクロール
        FldScrollField(fldP, linesToScroll, direction);

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

/*-------------------------------------------------------------------------*/
/*   Function :   Hander_JumpSelection                                     */
/*                                           ジャンプ用のイベントハンドラ  */
/*-------------------------------------------------------------------------*/
Boolean Handler_JumpSelection(EventType *event)
{
    FormType         *frm;
    ListType         *lstP;
    NNshMessageIndex *idxP;
    EventType        *dummyEvent;
    UInt16            max, keyCode;

    // メッセージインデックス
    idxP       = NNshGlobal->msgIndex;
    dummyEvent = &(NNshGlobal->dummyEvent);

    // 現在のフォームを取得
    switch (event->eType)
    { 
      // リスト選択イベント
      case lstSelectEvent:
        NNshGlobal->jumpSelection = event->data.lstSelect.selection;
        break;

      // キーダウン(ボタン)の選択
      case keyDownEvent:
        // 現在選択されているリストアイテム番号を取得
        frm  = FrmGetActiveForm();
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_JUMPLIST));
        NNshGlobal->jumpSelection = LstGetSelection(lstP);
        max  = LstGetNumberOfItems(lstP);

        // キーコードによって処理を分岐させる
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
            // Jog Push/Enterキーを押されたとき(OKボタンを押したことにする)
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->data.ctlSelect.pControl  
                = (ControlType*) FrmGetObjectPtr(frm,
                                       FrmGetObjectIndex(frm, BTNID_JUMPEXEC));
            dummyEvent->data.ctlSelect.controlID = BTNID_JUMPEXEC;
            dummyEvent->eType                    = ctlSelectEvent;
            EvtAddEventToQueue(dummyEvent);
            return (true);
            break;

          case chrEscape:
          case chrCapital_X:
          case chrSmall_X:
            // ESCキー/Xキーが押されたとき(Cancelボタンを押したことにする)
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->data.ctlSelect.controlID = BTNID_JUMPCANCEL;
            dummyEvent->eType                    = ctlSelectEvent;
            EvtAddEventToQueue(dummyEvent);
            return (true);
            break;

          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
            // ハードキー下、ジョグ下送り
            if (NNshGlobal->jumpSelection < (max - 1))
            {
                (NNshGlobal->jumpSelection)++;
                LstSetSelection(lstP, NNshGlobal->jumpSelection);
                return (true);
	    }
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
            // ハードキー下、ジョグ下送り
            if (NNshGlobal->jumpSelection > 0)
            {
                (NNshGlobal->jumpSelection)--;
                LstSetSelection(lstP, NNshGlobal->jumpSelection);
                return (true);
	    }
            break;

          default:
            // 上記以外のキーでは何もしない
            break;

        }
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
}

/*=========================================================================*/
/*   Function :  NNshWinSelectionWindow                                    */
/*                                                    選択ウィンドウの表示 */
/*=========================================================================*/
UInt16 NNshWinSelectionWindow(Char *listItemP, UInt16 nofItems)
{
    FormType  *prevFrm, *diagFrm;
    UInt32     ver;
    UInt16     btnId;
    MemHandle  listH;
    ListType  *lstP;

    // 初期化
    listH   = 0;
    btnId   = BTNID_JUMPCANCEL;
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if ((FtrGet(sysFtrCreator, sysFtrNumROMVersion, &ver) == errNone)
         &&(ver < 0x03503000))
    {
        FrmEraseForm(prevFrm);
    }

    diagFrm  = FrmInitForm(FRMID_JUMPMSG);
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
            LstSetSelection  (lstP, 0);
            LstSetTopItem    (lstP, 0);
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
    FrmDrawForm(prevFrm);
    return (btnId);
}
