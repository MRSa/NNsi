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
void NNsi_FrmDrawForm(FormType *frm, Boolean redraw)
{
    // フォームを(普通に)描画する
    FrmDrawForm(frm);

    return;
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
    UInt16         len;
    RectangleType  r;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

#ifdef USE_COLOR
    RGBColorType  color;
#endif  // #ifdef USE_COLOR

    if (FrmGetActiveForm() == NULL)
    {
        // ウィンドウ表示前は、表示しない
        return;
    }
    
    // 画面中央部を消去する
    r.topLeft.x = 8;
    r.topLeft.y = 70;
    r.extent.x  = 140;
    r.extent.y  = 22;

    // デバッグ用画面表示
#ifdef USE_DEBUG_ROM
    if (WinGetActiveWindow() == NULL)
    {
        FrmDrawForm(FrmGetActiveForm());
    }
#endif

    // 画面を消す
    WinEraseRectangle(&r, 0);

#ifdef USE_COLOR
    // OS 3.5以上の場合
    if (NNsiGlobal->palmOSVersion >= 0x03503000)
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
}

/*=========================================================================*/
/*   Function : Show_BusyForm                                              */
/*                                                      BUSYフォームの表示 */
/*=========================================================================*/
void Show_BusyForm(Char *msg)
{
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // 現在のフォームを保存
    NNsiGlobal->prevBusyForm = FrmGetActiveForm();
    if (NNsiGlobal->prevBusyForm != NULL)
    {
        // 文字列表示パターン
        SetMsg_BusyForm(msg);
    }
    return;
}

/*=========================================================================*/
/*   Function : Hide_BusyForm                                              */
/*                                                      BUSYフォームの削除 */
/*=========================================================================*/
void Hide_BusyForm(Boolean redraw)
{
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    if (NNsiGlobal->prevBusyForm != NULL)
    {
        NNsi_FrmDrawForm(NNsiGlobal->prevBusyForm, redraw);
    }
    return;
}

/*=========================================================================*/
/*   Function :  PrepareDialog_NNsh                                        */
/*                                              ダイアログ表示準備設定項目 */
/*=========================================================================*/
void PrepareDialog_NNsh(UInt16 formID, FormType **diagFrm, Boolean silk)
{
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNsiGlobal->palmOSVersion < 0x03503000)
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
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
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
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // シルク表示を最小化
#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
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

/*=========================================================================*/
/*   Function :   NNsh_DialogMessage                                       */
/*                                             メッセージダイアログの表示  */
/*=========================================================================*/
UInt16 NNsh_DialogMessage(UInt16 level, UInt16 altID, Char *mes1, Char *mes2, UInt32 num)
{
#ifdef USE_MYALERT
    UInt16  size;
    Char   *buf;
#endif
    Char           logBuf[MINIBUF];
    NNshSavedPref *NNsiParam;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    switch (level)
    {
      case NNSH_LEVEL_ERROR:
        // エラーメッセージ(必ず表示する)
        break;

      case NNSH_LEVEL_WARN:
        // 警告メッセージ
        if ((NNsiParam->confirmationDisable & (NNSH_OMITDIALOG_WARNING)) != 0)
        {
            // 警告表示をしない場合には、"OK"(== 0)を返す
            return (0);
        }
        break;

      case NNSH_LEVEL_CONFIRM:
        // 確認メッセージ
        if ((NNsiParam->confirmationDisable & (NNSH_OMITDIALOG_CONFIRM)) != 0)
        {
            // 確認表示をしない場合には、"OK"(== 0)を返す
            return (0);
        }
        break;

      case NNSH_LEVEL_DEBUG:
#ifdef DEBUG
        // デバッグオプション付きでコンパイルした場合には、必ず表示する。
#else
        if (NNsiParam->debugMessageON == 0)
        {
            // DEBUG表示しない場合には、"OK"(== 0)を返す
            return (0);
        }
#endif
        break;

      case NNSH_LEVEL_INFO:
      default:
        // 情報メッセージ
        if ((NNsiParam->confirmationDisable & (NNSH_OMITDIALOG_INFORMATION))
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

#ifdef USE_MYALERT
    size = StrLen(mes1) + StrLen(mes2) + StrLen(logBuf) + MARGIN;
    buf  = MemPtrNew_NNsh(size);
    if (buf == NULL)
    {
        return (FrmCustomAlert(altID, mes1, mes2, logBuf));
    }
    MemSet (buf, size, 0x00);
    StrCopy(buf, mes1);
    StrCat (buf, mes2);
    StrCat (buf, logBuf);
    
    MEMFREE_PTR(buf);
#else
    return (FrmCustomAlert(altID, mes1, mes2, logBuf));
#endif
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
          case vchrRockerUp:
          case vchrRockerDown:
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
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // ダミーイベント
    dummyEvent = &(NNsiGlobal->dummyEvent);

    // 現在のフォームを取得
    switch (event->eType)
    { 
      // リスト選択イベント
      case lstSelectEvent:
        if (NNsiGlobal->jumpSelection == event->data.lstSelect.selection)
        {
            // ダブルタップを検出(OKボタンを押したことにする)
            goto JUMP_SELECTION;
        }
        NNsiGlobal->jumpSelection = event->data.lstSelect.selection;
        break;

      // キーダウン(ボタン)の選択
      case keyDownEvent:
        // 現在選択されているリストアイテム番号を取得
        frm  = FrmGetActiveForm();
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_JUMPLIST));
        NNsiGlobal->jumpSelection = LstGetSelection(lstP);
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
          case vchrRockerDown:
            // ハードキー下、ジョグ下送り
            if (NNsiGlobal->jumpSelection < (max - 1))
            {
                (NNsiGlobal->jumpSelection)++;
                LstSetSelection(lstP, NNsiGlobal->jumpSelection);
            }
            else
            {
                // 選択をリストの先頭へ移動
                NNsiGlobal->jumpSelection = 0;
                LstSetSelection(lstP, NNsiGlobal->jumpSelection);
                LstSetTopItem  (lstP, 0);
            }
            return (true);
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrPrevField:
          case vchrThumbWheelUp:
          case vchrRockerUp:
            // ハードキー上、ジョグ上送り
            if (NNsiGlobal->jumpSelection > 0)
            {
                (NNsiGlobal->jumpSelection)--;
                LstSetSelection(lstP, NNsiGlobal->jumpSelection);
            }
            else
            {
                // 選択をリストの末尾へ移動
                NNsiGlobal->jumpSelection = max - 1;
                LstSetSelection(lstP, NNsiGlobal->jumpSelection);
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
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

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
    if (NNsiGlobal->palmOSVersion < 0x03503000)
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
        NNsiGlobal->jumpSelection = 0;

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

    // ダイアログ表示準備
    PrepareDialog_NNsh(FRMID_INPUTDIALOG, &diagFrm, true);
    if (diagFrm == NULL)
    {
        return (false);
    }

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

    // ダイアログ表示の後片付け
    PrologueDialog_NNsh(prevFrm, diagFrm);

    return (ret);
}
