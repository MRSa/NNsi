/*============================================================================*
 *  $Id: damain.c,v 1.7 2004/09/17 15:16:31 mrsa Exp $
 *
 *  FILE: 
 *     damain.c
 *
 *  Description: 
 *     DAアプリケーション
 *
 *===========================================================================*/
/********** Include Files **********/
#include "local.h"

void DAmain(void);
Err  SendToLocalMemopad(Char *title, Char *data);

static void ParseMessage(Char *buf, Char *src, UInt32 dstSize, UInt32 size, UInt16 kanjiCode);

// UTF8 -> Shift JIS変換テーブル (AMsoftさんのＤＢを使用する)
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, LocalID dbId);

static Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr);
static Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte);
static UInt16  Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L);

static Boolean analysis_URL(NNshWorkingInfo *NNshGlobal);
static void    get_html_contents(NNshWorkingInfo *NNshGlobal);

static void NNsh_checkRefreshConnectionRequest(NNshWorkingInfo *NNshGlobal);

static void analysis_formTag  (NNshWorkingInfo *NNshGlobal);
static void pickup_formTag    (NNshWorkingInfo *NNshGlobal);
static void pickup_nameTag    (NNshWorkingInfo *NNshGlobal);
static void pickup_valueTag   (NNshWorkingInfo *NNshGlobal);
static void set_name_and_value(NNshWorkingInfo *NNshGlobal);
static void ParseTextArea(Char *buf, Char *src, UInt32 dstSize, UInt32 size);

static void StrNCopy_URLEncode(UInt8 *dst, UInt32 bufSize, UInt8 *src, UInt32 size);

#ifdef DEBUG_LOCALHOST_ADDRESS
#define  DEBUG_ADDRESS  "localhost"
#endif  // #ifdef DEBUG_LOCALHOST_ADDRESS
 
/*=========================================================================*/
/*   Function : startDA  (DAスタートアップ)                                */
/*                                                                         */
/*=========================================================================*/
void startDA(void)
{
    FormType *frm;

    frm = FrmGetActiveForm();
    DAmain();
    FrmSetActiveForm(frm);

    return;
}

/*=========================================================================*/
/*   Function : DAmain  (DAメイン処理)                                     */
/*                                                                         */
/*=========================================================================*/
void DAmain(void)
{
    Boolean           isURL;
    UInt16            size, autoOffTime, btnId;
    NNshWorkingInfo  *NNshGlobal;

    // URLの指定が行われたかどうか
    isURL = false;

    // NNsiグローバル領域用格納領域の確保
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // 領域確保に失敗！(起動しない)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " global");
        return;
    }        
    MemSet(NNshGlobal, size, 0x00);

    // 通信タイムアウト値を設定
    NNshGlobal->timeout = SysTicksPerSecond() * NNSH_GETMAIL_TIMEOUT;

    // Parameter pointerをfeatureにセット
    FtrSet(SOFT_CREATOR_ID, SOFT_DB_ID, (UInt32) NNshGlobal);

    // OSバージョンの取得
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &(NNshGlobal->palmOSVersion));

    // URLの取得(選択中フィールドをクリップボードにコピー)
    // ！！！注意！！！
    //    消費メモリを気にしたため、変数を流用している！ (よくない！)
    //      fldId は NNshGlobal->isChecking
    //      start は NNshGlobal->portNum
    //      end   は NNshGlobal->kanjiCode
    NNshGlobal->isChecking = FrmGetFocus(FrmGetActiveForm());
    if (NNshGlobal->isChecking != noFocus)
    {
        if (FrmGetObjectType(FrmGetActiveForm(), NNshGlobal->isChecking) == frmFieldObj)
        {
            FldGetSelection(FrmGetObjectPtr(FrmGetActiveForm(), 
                                            NNshGlobal->isChecking),
                            &(NNshGlobal->portNum),
                            &(NNshGlobal->kanjiCode));
            if (NNshGlobal->portNum != NNshGlobal->kanjiCode)
            {
                // 選択文字列をコピーする
                FldCopy(FrmGetObjectPtr(FrmGetActiveForm(),
                                        NNshGlobal->isChecking));
	    }
        }
    }
    NNshGlobal->isChecking = 0;
    NNshGlobal->portNum    = 0;
    NNshGlobal->kanjiCode  = 0;

#if 0
    // URLの取得(クリップボードから)
    if (isURL == false)
    {
        NNshGlobal->memH = ClipboardGetItem(clipboardText,
                                            &(NNshGlobal->fieldLen));
        if ((NNshGlobal->memH != 0)&&(NNshGlobal->fieldLen != 0))
        {
            if (NNshGlobal->fieldLen > (BUFSIZE * 2))
            {
                NNshGlobal->fieldLen = (BUFSIZE * 2) - 1;
            }
            MemMove(NNshGlobal->reqURL, 
                    MemHandleLock(NNshGlobal->memH),
                    NNshGlobal->fieldLen);
            MemHandleUnlock(NNshGlobal->memH);
        }
    }
    isURL = analysis_URL(NNshGlobal);

    // URLが指定されていない場合には、ダイアログを表示する
    if (isURL == false)
    {
        MemSet(NNshGlobal->reqURL, (BUFSIZE * 2), 0x00);
        isURL = InputDialog("Input URL", NNshGlobal->reqURL, (BUFSIZE * 2));
    }
    if (isURL != false)
    {
        // 入力されたデータにURLが含まれているか解析する
        isURL = analysis_URL(NNshGlobal);
    }
#else
    // クリップボード内に文字列があるかどうかチェックする
    NNshGlobal->memH = ClipboardGetItem(clipboardText,
                                        &(NNshGlobal->fieldLen));
    if ((NNshGlobal->memH == 0)||(NNshGlobal->fieldLen == 0))
    {
        // クリップボード内に文字列が無かった、終了する
        FrmCustomAlert(ALTID_INFO, "String is nothing...","[ejDA]"," global");
        return;
    }

    // 英語→日本語の翻訳フォームがあるURLの設定
    MemSet(NNshGlobal->reqURL, (BUFSIZE * 2),  0x00);
    StrCopy(NNshGlobal->reqURL, "http://www.excite.co.jp/world/english/");
    isURL = analysis_URL(NNshGlobal);
#endif
    if (isURL == false)
    {
        // 取得先URLが指定されなかった、終了処理へ
        goto FUNC_END;
    }
    //////////////////////////////////////////////////////////////////////////
    // HTTP取得実処理
    //////////////////////////////////////////////////////////////////////////
    // 送信バッファの確保
    NNshGlobal->sendBuf = MemPtrNew(SENDBUFFER_DEFAULT);
    if (NNshGlobal->sendBuf == NULL)
    {
        // 領域確保失敗、終了処理へ
        goto FUNC_END;
    }
    MemSet(NNshGlobal->sendBuf, (SENDBUFFER_DEFAULT), 0x00);

    // 受信バッファの確保
    NNshGlobal->recvBuf = MemPtrNew(RECEIVEBUFFER_DEFAULT + MARGIN);
    if (NNshGlobal->recvBuf == NULL)
    {
        // 領域確保失敗、終了処理へ
        MEMFREE_PTR(NNshGlobal->sendBuf);
        goto FUNC_END;
    }
    MemSet(NNshGlobal->recvBuf, (RECEIVEBUFFER_DEFAULT + MARGIN), 0x00);

    NNshGlobal->workBuf = MemPtrNew(WORKBUFFER_DEFAULT);
    if (NNshGlobal->workBuf == NULL)
    {
        // 領域確保失敗、終了処理へ
        MEMFREE_PTR(NNshGlobal->sendBuf);
        MEMFREE_PTR(NNshGlobal->recvBuf);
        goto FUNC_END;
    }
    MemSet(NNshGlobal->workBuf, (WORKBUFFER_DEFAULT), 0x00);

    // BUSYウィンドウを表示する
    Show_BusyForm(NNshGlobal->hostName, NNshGlobal);    

    // 自動電源OFFタイマを停止させる
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    // 送信バッファデータをつめる(空にする)
    StrCopy(NNshGlobal->workBuf, " ");

    // ＨＴＭＬデータを受信する
    get_html_contents(NNshGlobal);

    // 翻訳フォームを解析して再度データ送信
    SetMsg_BusyForm("Sending...");
    pickup_formTag  (NNshGlobal);
    analysis_formTag(NNshGlobal);

    // ＨＴＭＬデータ(翻訳結果)を受信する
    get_html_contents(NNshGlobal);

    // 自動電源OFFタイマを復旧させる
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    SetMsg_BusyForm("Parsing...");

    // バッファクリア
    MemSet(NNshGlobal->workBuf, (WORKBUFFER_DEFAULT), 0x00);

    // タイトルに変換結果を表示する
    MemSet(NNshGlobal->showBuf, (MINIBUF + MARGIN), 0x00);
    StrCopy(NNshGlobal->showBuf, "Result(English to Japanese)");

    // HTTPヘッダの末尾を探す
    NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "\r\n\r\n");
    if (NNshGlobal->tempPtr == NULL)
    {
        // 受信バッファの先頭から表示する
        NNshGlobal->tempPtr = NNshGlobal->recvBuf;
    }
    else
    {
        // 改行コード("\r\n\r\n")分ポインタを進める
        NNshGlobal->tempPtr = NNshGlobal->tempPtr + 4;
    }


    // 表示サイズがフィールド表示用バッファに収まるか？
    if (StrLen(NNshGlobal->tempPtr) < (WORKBUFFER_DEFAULT - 1))
        
    {
        NNshGlobal->fieldLen = StrLen(NNshGlobal->tempPtr);
    }
    else
    {
        NNshGlobal->fieldLen = (WORKBUFFER_DEFAULT - 2);
    }

    // 翻訳結果の表示...
    NNshGlobal->kanjiCode = ejDA_KANJICODE_DATAOUTPUT;

HTML_PARSE:
    MemSet(NNshGlobal->workBuf, (WORKBUFFER_DEFAULT), 0x00);
    if (NNshGlobal->kanjiCode == nnDA_KANJICODE_RAW)
    {
        // 受信データをまったく解析せずに表示する
        MemMove(NNshGlobal->workBuf,NNshGlobal->tempPtr,NNshGlobal->fieldLen);
    }
    else if (NNshGlobal->kanjiCode == ejDA_KANJICODE_DATAOUTPUT)
    {
        // 翻訳結果を表示する
        ParseTextArea(NNshGlobal->workBuf,
                      NNshGlobal->tempPtr,
                      WORKBUFFER_DEFAULT,
                      NNshGlobal->fieldLen);
    }
    else
    {
        // 受信データをHTMLデータファイルとして(簡単に解析して)表示する
        ParseMessage(NNshGlobal->workBuf,NNshGlobal->tempPtr,
                WORKBUFFER_DEFAULT,NNshGlobal->fieldLen,NNshGlobal->kanjiCode);
    }

    // BUSYウィンドウを隠す
    Hide_BusyForm(NNshGlobal);

    // 結果ダイアログを表示する
    if (FrmGetActiveFormID() != 0)
    {
        NNshGlobal->previousForm = FrmGetActiveForm();
    }
    else
    {
        NNshGlobal->previousForm = NULL;
    }

#ifdef USE_WIDEWINDOW
    NNshGlobal->ret = FtrGet(pinCreator, pinFtrAPIVersion, &(NNshGlobal->silkVer));
    if (NNshGlobal->ret != errNone)
    {
        // DIA 非サポートデバイス、ダイアログサイズの拡大はしない
        NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
        FrmSetActiveForm   (NNshGlobal->currentForm);
        goto RESIZE_WINDOW;
    }

    // DIAサポートデバイス    
    NNshGlobal->ret = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &(NNshGlobal->silkVer));
    if ((NNshGlobal->ret != errNone)|
        ((NNshGlobal->silkVer & grfFtrInputAreaFlagDynamic) == 0)||
        ((NNshGlobal->silkVer & grfFtrInputAreaFlagCollapsible) == 0))
    {
        // エラーになる場合、もしくはDIA非サポートの場合、何もせず抜ける
        NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
        FrmSetActiveForm   (NNshGlobal->currentForm);
        goto RESIZE_WINDOW;
    }
    // 現在のシルク領域状態を取得する
    // btnId = PINGetInputAreaState();

    // フォームをオーブンする
    NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
    FrmSetActiveForm   (NNshGlobal->currentForm);

    // シルク制御を有効にする
    FrmSetDIAPolicyAttr(NNshGlobal->currentForm, frmDIAPolicyCustom);
    PINSetInputTriggerState(pinInputTriggerEnabled);

    // えらいベタ、、、ウィンドウサイズの最大値最小値を設定
    WinSetConstraintsSize(FrmGetWindowHandle(NNshGlobal->currentForm),
                          160, 225, 225, 160, 225, 225);

    // シルク領域状態を前画面と同じにする...
    // PINSetInputAreaState(btnId);
    PINSetInputAreaState(pinInputAreaUser);


RESIZE_WINDOW:
    /* 全体のWindowサイズ取得 */
    WinGetDisplayExtent(&(NNshGlobal->x), &(NNshGlobal->y));

    /* dialogのサイズを取得 */
    FrmGetFormBounds(NNshGlobal->currentForm, &(NNshGlobal->r));

    /* dialog(ウインドウ)の拡大 */
    NNshGlobal->r.topLeft.x = 2;
    NNshGlobal->r.topLeft.y = 2;

    NNshGlobal->diffX       = NNshGlobal->r.extent.x;
    NNshGlobal->diffY       = NNshGlobal->r.extent.y;

    NNshGlobal->r.extent.y  = NNshGlobal->y - 4;
    NNshGlobal->r.extent.x  = NNshGlobal->x - 4;

    NNshGlobal->diffX       = NNshGlobal->r.extent.x - NNshGlobal->diffX;
    NNshGlobal->diffY       = NNshGlobal->r.extent.y - NNshGlobal->diffY;

    WinSetWindowBounds(FrmGetWindowHandle(NNshGlobal->currentForm),
                       &NNshGlobal->r);

    /* OK ボタンの移動 */
    btnId = FrmGetObjectIndex(NNshGlobal->currentForm, BTNID_DIALOG_OK);
    FrmGetObjectBounds(NNshGlobal->currentForm, btnId, &NNshGlobal->r);
    // NNshGlobal->r.topLeft.x = NNshGlobal->r.topLeft.x + NNshGlobal->diffX;
    NNshGlobal->r.topLeft.y = NNshGlobal->r.topLeft.y + NNshGlobal->diffY;
    FrmSetObjectBounds(NNshGlobal->currentForm, btnId, &NNshGlobal->r);

    /* REDRAW ボタンの移動 */
    btnId = FrmGetObjectIndex(NNshGlobal->currentForm, BTNID_DIALOG_REDRAW);
    FrmGetObjectBounds(NNshGlobal->currentForm, btnId, &NNshGlobal->r);
    // NNshGlobal->r.topLeft.x = NNshGlobal->r.topLeft.x + NNshGlobal->diffX;
    NNshGlobal->r.topLeft.y = NNshGlobal->r.topLeft.y + NNshGlobal->diffY;
    FrmSetObjectBounds(NNshGlobal->currentForm, btnId, &NNshGlobal->r);
    // FrmHideObject(NNshGlobal->currentForm, btnId);                

    /* Fieldの移動(拡大) */
    btnId = FrmGetObjectIndex(NNshGlobal->currentForm, FLDID_INFOFIELD);
    FrmGetObjectBounds(NNshGlobal->currentForm, btnId, &NNshGlobal->r);
    NNshGlobal->r.extent.x = NNshGlobal->r.extent.x + NNshGlobal->diffX;
    NNshGlobal->r.extent.y = NNshGlobal->r.extent.y + NNshGlobal->diffY;
    FrmSetObjectBounds(NNshGlobal->currentForm, btnId, &NNshGlobal->r);

#else  // #ifdef USE_WIDEWINDOW
    // ウィンドウのオープン
    NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
    FrmSetActiveForm   (NNshGlobal->currentForm);
#endif // #ifdef USE_WIDEWINDOW

    // ウィンドウタイトルを設定する
    FrmCopyTitle(NNshGlobal->currentForm, NNshGlobal->showBuf);

    // 受信データをフィールドにセットする
    NNshWinSetFieldText(NNshGlobal->currentForm, FLDID_INFOFIELD,
                        NNshGlobal->workBuf, StrLen(NNshGlobal->workBuf));

    // 取得結果ダイアログを表示する
    FrmSetEventHandler(NNshGlobal->currentForm, Handler_MainForm);
    btnId = FrmDoDialog(NNshGlobal->currentForm);

    // フォームの削除
    if (NNshGlobal->previousForm != NULL)
    {
        FrmSetActiveForm(NNshGlobal->previousForm);
    }
    FrmDeleteForm(NNshGlobal->currentForm);

    if (btnId == BTNID_DIALOG_REDRAW)
    {
        //  "画面再表示"の場合...BUSYウィンドウを表示して、
        // データ再解析からやりなおす
        Show_BusyForm("Parsing...", NNshGlobal);    

        goto HTML_PARSE;
    }

    // 送受信バッファの解放
    MEMFREE_PTR(NNshGlobal->recvBuf);
    MEMFREE_PTR(NNshGlobal->sendBuf);
    MEMFREE_PTR(NNshGlobal->workBuf);

FUNC_END:
    //////////////////////////////////////////////////////////////////////////
    /////     終了処理（設定の保存等）
    //////////////////////////////////////////////////////////////////////////
    // Globalの登録解除
    FtrUnregister(SOFT_CREATOR_ID, SOFT_DB_ID);

    // 確保した領域を開放する
    MEMFREE_PTR(NNshGlobal);

    return;
}

/*=========================================================================*/
/*   Function : Handler_EditAction                                         */
/*                                        編集メニューが選択された時の処理 */
/*=========================================================================*/
Boolean Handler_EditAction(EventType *event)
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

    if (event->eType != menuEvent)
    {
        // キー入力のチェック
        if (event->eType == keyDownEvent)
        {
            switch (event->data.keyDown.chr)
            {
              case vchrJogRelease:
              case vchrThumbWheelPush:
                // OKボタンを押したことにする
                CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                       FrmGetObjectIndex(FrmGetActiveForm(),
                                                         BTNID_DIALOG_OK)));
                return (true);
                break;

              default:
                // 何もしない 
                break;
            }
        }
        return (false);
    }

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
/*   Function :   Hander_MainForm                                          */
/*                                       イベントハンドラ(設定フォーム用)  */
/*=========================================================================*/
Boolean Handler_MainForm(EventType *event)
{
    FieldType *fldP;
    UInt16    itemId;

    // パラメータの取得
    NNshWorkingInfo *NNshGlobal;
    FtrGet(SOFT_CREATOR_ID, SOFT_DB_ID, (UInt32 *)&NNshGlobal);

    // 現在のフォームを取得
    switch (event->eType)
    { 
      // メニュー選択
      case menuEvent:
        itemId = event->data.menu.itemID;
        break;

      // キー入力
      case keyDownEvent:
        itemId = event->data.keyDown.chr;
        break;

      // 画面タップ
      case ctlSelectEvent:
        itemId = event->data.ctlSelect.controlID;
        break;

      // ISP名変更
      case popSelectEvent:
        itemId = event->data.popSelect.listID;
        break;

      case penDownEvent:
      default: 
        return (false);
        break;
    }
    switch (itemId)
    {
      case MNUID_VERSION:
        // バージョン情報表示
        ShowVersion_NNsh();
        break;

      case vchrPageDown:
      case chrDownArrow:
      case vchrJogDown:
        // 下スクロール
        if (FrmGetActiveFormID() == FRMID_RESULT)
        {
          fldP = FrmGetObjectPtr(FrmGetActiveForm(), 
                                 FrmGetObjectIndex(FrmGetActiveForm(),
                                                   FLDID_INFOFIELD));
          FldScrollField(fldP, 5, winDown);
        }
        break;

      case vchrPageUp:
      case chrUpArrow:
      case vchrJogUp:
        // 上スクロール
        if (FrmGetActiveFormID() == FRMID_RESULT)
        {
          fldP = FrmGetObjectPtr(FrmGetActiveForm(), 
                                 FrmGetObjectIndex(FrmGetActiveForm(),
                                                   FLDID_INFOFIELD));
          FldScrollField(fldP, 5, winUp);
        }
        break;

      case MNUID_CODE_RAW:
      case chrCapital_R:
      case chrSmall_R:
        // "ベタ表示"モード
        NNshGlobal->kanjiCode = nnDA_KANJICODE_RAW;

        // REDRAWボタンを押したことにする
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_SHIFTJIS:
      case chrCapital_S:
      case chrSmall_S:
        // SHIFT JIS表示モード
        NNshGlobal->kanjiCode = NNSH_KANJICODE_SHIFTJIS;

        // REDRAWボタンを押したことにする
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_EUC:
      case chrCapital_E:
      case chrSmall_E:
        // EUC表示モード
        NNshGlobal->kanjiCode = NNSH_KANJICODE_EUC;

        // REDRAWボタンを押したことにする
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_JIS:
      case chrCapital_J:
      case chrSmall_J:
        // JIS表示モード
        NNshGlobal->kanjiCode = NNSH_KANJICODE_JIS;

        // REDRAWボタンを押したことにする
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_UTF8:
      case chrCapital_U:
      case chrSmall_U:
        // UTF8表示モード
        NNshGlobal->kanjiCode = NNSH_KANJICODE_UTF8;

        // REDRAWボタンを押したことにする
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_CODE_PARSE:
      case chrCapital_P:
      case chrSmall_P:
        // 翻訳データ表示モード
        NNshGlobal->kanjiCode = ejDA_KANJICODE_DATAOUTPUT;

        // REDRAWボタンを押したことにする
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_DIALOG_REDRAW)));
        break;

      case MNUID_DISCONNECT:
      case chrCapital_Q:
      case chrSmall_Q:
        // 回線切断...
        NNshNet_LineHangup();
        break;

      case MNUID_SENDMEMO:
      case chrCapital_M:
      case chrSmall_M:
        // メモ帳へ出力
        SendToLocalMemopad("html contents", NNshGlobal->workBuf);
        break;

      case BTNID_DIALOG_OK:
      case BTNID_DIALOG_CANCEL:
      case BTNID_DIALOG_REDRAW:
      default:
        return (false);
        break;
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF                                                */
/*                                                (ソーティング用比較関数) */
/*                        ※ DBからotherバイト目に「文字列で」keyが並んで  */
/*                                                         いるものとする。*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_Char(void *rec1, void *rec2, Int16 other,
                              SortRecordInfoPtr       rec1SortInfo,
                              SortRecordInfoPtr       rec2SortInfo, 
                              MemHandle               appInfoH)
{
    Char *str1, *str2;

    str1 = ((Char *) rec1) + other;
    str2 = ((Char *) rec2) + other;
    return (StrCompare(str1, str2));
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF_UInt32                                         */
/*                                                (ソーティング用比較関数) */
/*                        ※ DBからotherバイト目に「UInt32で」keyが並んで  */
/*                                                         いるものとする。*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_UInt32(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH)
{
    Char   *loc1,  *loc2;
    UInt32 *data1, *data2;

    loc1  = ((Char *) rec1) + other;
    loc2  = ((Char *) rec2) + other;

    data1 = (UInt32 *) loc1;
    data2 = (UInt32 *) loc2;

    return (*data1 - *data2);
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF_UInt16                                         */
/*                                                (ソーティング用比較関数) */
/*                    ※ keyが先頭よりotherバイト目から「UInt16で」並んで  */
/*                                                         いるものとする。*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_UInt16(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH)
{
    Char   *loc1,  *loc2;
    UInt16 *data1, *data2;

    loc1  = ((Char *) rec1) + other;
    loc2  = ((Char *) rec2) + other;

    data1 = (UInt16 *) loc1;
    data2 = (UInt16 *) loc2;

    return (*data1 - *data2);
}

/*-------------------------------------------------------------------------*/
/*   Function : searchRecordSub                                            */
/*                                                        (レコードを検索) */
/*-------------------------------------------------------------------------*/
static Err searchRecordSub(DmOpenRef dbRef, void *target, UInt16 keyType, 
                           UInt16   offset, UInt16 *idx,  MemHandle *hnd)
{
    UInt16     nofData, start, end, mid;
    Int16      chk;
    MemHandle  dataH;
    Char      *data;

    // 検索する文字列をキャストする
    data = (Char *)target;
    data = data + offset;

    // データ数がゼロだったら検索失敗
    nofData = DmNumRecords(dbRef);
    if (nofData == 0)
    {
        return (~errNone - 5);
    }

    // ２分検索で指定されたデータが登録されているかチェック
    mid   = 0;
    start = 0;
    end   = nofData;
    while (start <= end)
    {
        mid = start + (end - start) / 2;
        dataH = DmQueryRecord(dbRef, mid);
        if (dataH == 0)
        {
            // 見つからなかった！ (ここで抜ける模様)
            return (~errNone);
        }

        // 指定されたキーによって、チェック方法を変える
        data = (Char *) MemHandleLock(dataH);
        switch (keyType)
        {
          case NNSH_KEYTYPE_UINT32:
            chk  = *((UInt32 *) data) - *((UInt32 *) target);
            break;

          case NNSH_KEYTYPE_UINT16:
            chk  = *((UInt16 *) data) - *((UInt16 *) target);
            break;

          case NNSH_KEYTYPE_CHAR:
          default:
            chk  = StrCompare(data, (Char *) target);
            break;
        }
        if (chk == 0)
        {
            // 見つかった！(データをまるごとコピー)
            MemHandleUnlock(dataH);
            *idx  = mid;
            *hnd  = dataH;
            return (errNone);
        }
        MemHandleUnlock(dataH);
        if (chk < 0)
        {
            start = mid + 1;
        }
        else
        {
            end   = mid - 1;
        }
    }
    return (~errNone - 4);
}

/*==========================================================================*/
/*  OpenDatabase_NNsh : データベースアクセスの初期化                        */
/*                                                                          */
/*==========================================================================*/
void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef)
{
    LocalID dbId;
    UInt16  dbVersion;

    // データベースが存在するか確認する。
    *dbRef    = 0;
    dbVersion = 0;
    dbId      = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // データベースのバージョン番号を取得
        (void) DmDatabaseInfo(0, dbId, NULL, NULL, &dbVersion, NULL, 
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (dbVersion < chkVersion)
        {
            // データベースのバージョンが古いときは、データベースを削除する
            (void) DmDeleteDatabase(0, dbId);
            dbId = 0;
        }
    }

    // データベースが存在しないとき
    if (dbId == 0)
    {
        // データベースを新規作成
        (void) DmCreateDatabase(0, dbName, 
                                SOFT_CREATOR_ID, SOFT_DBTYPE_ID, false);

        dbId      = DmFindDatabase(0, dbName);
        if (dbId != 0)
        {
            // DBにバージョン番号を設定
            dbVersion = chkVersion;
            (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, &dbVersion, NULL,
                                     NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        }
    }

    // データベースのオープン
    *dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);

    return;
}

/*==========================================================================*/
/*  closeDatabase_NNsh() : データベースをクローズする                       */
/*                                                                          */
/*==========================================================================*/
void CloseDatabase_NNsh(DmOpenRef dbRef)
{
    // DBがオープンできていたらCLOSEする。
    if (dbRef != 0)
    {
        (void) DmCloseDatabase(dbRef);
        dbRef = 0;
    }
    return;
}

/*=========================================================================*/
/*   Function : QsortRecord_NNsh                                           */
/*                                                (レコードのソーティング) */
/*=========================================================================*/
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset)
{

    // クイックソートを仕掛ける
    switch (keyType)
    {

      case NNSH_KEYTYPE_UINT32:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_UInt32, offset);
        break;

      case NNSH_KEYTYPE_UINT16:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_UInt16, offset);
        break;

      case NNSH_KEYTYPE_CHAR:
      default:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_Char, offset);
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function : IsortRecord_NNsh                                           */
/*                                                (レコードのソーティング) */
/*=========================================================================*/
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset)
{

    // 挿入ソートを仕掛ける
    switch (keyType)
    {
      case NNSH_KEYTYPE_UINT32:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_UInt32, offset);
        break;

      case NNSH_KEYTYPE_UINT16:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_UInt16, offset);
        break;

      case NNSH_KEYTYPE_CHAR:
      default:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_Char, offset);
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function : EntryRecord_NNsh                                           */
/*                                               (ＤＢの先頭にデータ登録)  */
/*=========================================================================*/
Err EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData)
{
    MemHandle  newRecH;
    UInt16     index;
    void      *dbP;

    // レコードの新規追加
    index   = 0;
    newRecH = DmNewRecord(dbRef, &index, size + sizeof(UInt32));
    if (newRecH == 0)
    {
        // レコード追加エラー
        return (DmGetLastErr());
    }

    // レコードにデータ書き込み
    dbP = MemHandleLock(newRecH);
    DmWrite(dbP, 0, recordData, size);
    MemHandleUnlock(newRecH);

    // レコードを解放（変更終了）
    DmReleaseRecord(dbRef, index, true);

    return (errNone);
}

/*=========================================================================*/
/*   Function : IsExistRecord_NNsh                                         */
/*                                                (レコードの存在チェック) */
/*=========================================================================*/
Err IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index)
{
    MemHandle  dataH;

    *index  = 0;
    dataH   = 0;
    return (searchRecordSub(dbRef, target, keyType, offset, index, &dataH));
}

/*=========================================================================*/
/*   Function : SearchRecord_NNsh                                          */
/*                                                        (レコードを検索) */
/*=========================================================================*/
Err SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,
                      UInt16 offset, UInt16 size, void *matchedData,
                      UInt16 *index)
{
    Err        ret;
    UInt16     start;
    MemHandle  dataH;
    Char      *sc,  *ds;
    UInt32    *src, *dest;

    *index = 0;
    dataH  = 0;

    ret    = searchRecordSub(dbRef, target, keyType, offset, index, &dataH);
    if (ret != errNone)
    {
        // データ見つからず
        return (ret);
    }
    if (dataH == 0)
    {
        // メモリハンドラ異常
        return ((~errNone) - 2);
    }

    src = (UInt32 *) MemHandleLock(dataH);
    if (src == NULL)
    {
        // メモリポインタ異常
        return ((~errNone) - 3);
    }

    // データをまるごとコピーして応答
    MemSet(matchedData, size, 0x00);
    dest   = (UInt32 *) matchedData;
    for (start = 0; start < (size - sizeof(UInt32));
         start = start + sizeof(UInt32))
    {
        *dest++ = *src++;
    }
    for (sc = (Char *) src, ds = (Char *) dest;start < size;
         start = start + sizeof(Char))
    {
         *ds++ = *sc++;
    }

    MemHandleUnlock(dataH);
    return (errNone);
}

/*=========================================================================*/
/*   Function : DeleteRecord_NNsh                                          */
/*                                                    (該当レコードを削除) */
/*=========================================================================*/
Err DeleteRecord_NNsh(DmOpenRef dbRef, void  *target,
                      UInt16 keyType,  UInt16 offset)
{
    Err       ret;
    UInt16    index;
    MemHandle dataH;

    index = 0;
    dataH = 0;
    ret = searchRecordSub(dbRef, target, keyType, offset, &index, &dataH);
    if (ret != errNone)
    {
        return (errNone + 1);
    }

    return (DmRemoveRecord(dbRef, index));
}

/*=========================================================================*/
/*   Function : DeleteRecordIDX_NNsh                                       */
/*                                                    (該当レコードを削除) */
/*=========================================================================*/
Err DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index)
{
    // データ数をオーバしているか確認
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone);
    }
    return (DmRemoveRecord(dbRef, index));
}

/*=========================================================================*/
/*   Function : GetRecord_NNsh                                             */
/*                                          (レコードをインデックスで獲得) */
/*=========================================================================*/
Err GetRecord_NNsh(DmOpenRef dbRef, UInt16 index,
                   UInt16 size, void *matchedData)
{
    MemHandle  dataH;
    UInt16     start;
    UInt32    *src, *dest;
    Char      *sc,  *ds;

    // データ数をオーバしているか確認
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone - 6);
    }

    dataH = DmQueryRecord(dbRef, index);
    if (dataH == 0)
    {
        // 見つからなかった！
        return (~errNone);
    }
    src = (UInt32 *) MemHandleLock(dataH);
    if (src == NULL)
    {
        // メモリポインタ異常
        return ((~errNone) - 3);
    }

    // データをまるごとコピーして応答
    MemSet(matchedData, size, 0x00);
    dest   = (UInt32 *) matchedData;
    for (start = 0; start < (size - sizeof(UInt32));
         start = start + sizeof(UInt32))
    {
        *dest++ = *src++;
    }
    for (sc = (Char *) src, ds = (Char *) dest;start < size;
         start = start + sizeof(Char))
    {
         *ds++ = *sc++;
    }
    MemHandleUnlock(dataH);
    return (errNone);
}

/*=========================================================================*/
/*   Function : GetRecordReadOnly_NNsh                                     */
/*                                          (レコードをインデックスで獲得) */
/*=========================================================================*/
Err GetRecordReadOnly_NNsh(DmOpenRef dbRef, UInt16 index,
                           MemHandle *dataH, void **record)
{
    // データ数をオーバしているか確認
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone - 6);
    }

    *dataH = DmQueryRecord(dbRef, index);
    if (*dataH == 0)
    {
        // 見つからなかった！
        return (~errNone);
    }

    // 領域をロックする
    *record = MemHandleLock(*dataH);
    if (record == NULL)
    {
        // メモリポインタ異常
        return ((~errNone) - 3);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : ReleaseRecordReadOnly_NNsh                                 */
/*                                                (レコード読み出しを終了) */
/*=========================================================================*/
Err ReleaseRecordReadOnly_NNsh(DmOpenRef dbRef, MemHandle dataH)
{
    if (dataH != 0)
    {
        MemHandleUnlock(dataH);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : UpdateRecord_NNsh                                          */
/*                                          (レコードをインデックスで更新) */
/*=========================================================================*/
Err UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, 
                      UInt16 size, void *updateData)
{
    MemHandle recH;
    void     *dbP;

    recH = DmGetRecord(dbRef, index);
    if (recH == 0)
    {
        // レコード参照エラー
        return (DmGetLastErr());
    }

    // レコードにデータ書き込み
    dbP = MemHandleLock(recH);
    DmWrite(dbP, 0, updateData, size);
    MemHandleUnlock(recH);

    // レコードを解放（変更終了）
    DmReleaseRecord(dbRef, index, true);

    return (errNone);
}

/*==========================================================================*/
/*  GetDBCount_NNsh : データベースに登録されているデータ件数を応答する      */
/*                                                                          */
/*==========================================================================*/
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count)
{
    *count = DmNumRecords(dbRef);
    return;
}

/*==========================================================================*/
/*  GetDBInfo_NNsh : データベースの情報の取得                               */
/*                                                                          */
/*==========================================================================*/
Err GetDBInfo_NNsh(Char *dbName, NNshDBInfo *info)
{
    LocalID dbId;
    
    dbId = DmFindDatabase(0, dbName);

    // データベースが存在しないとき
    if (dbId == 0)
        return(~errNone);

    return(DmDatabaseInfo(0, dbId,
                          info->nameP, info->attributesP,
                          info->versionP, info->crDateP,
                          info->modDateP, info->bckUpDateP,
                          info->modNumP, info->appInfoIDP,
                          info->sortInfoIDP, info->typeP,
                          info->creatorP));
}


/*==========================================================================*/
/*  net_open() : ネットのオープン                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_open(UInt16 *netRef)
{
    UInt16 ifErr;
    Err    ret;

    // 変数の初期化
    ifErr = 0;

    // ネットライブラリのオープン
    (void) SysLibFind("Net.lib", netRef);

    // ネットワークＯＰＥＮ
    ret   = NetLibOpen(*netRef, &ifErr);
    if ((ret == errNone)||(ret == netErrAlreadyOpen))
    {
        if (ifErr == 0)
        {
            // open成功
            return (errNone);
        }
        (void) NetLibClose(*netRef, false);
        *netRef = 0;
        ret = ~errNone;
    }
    return (ret);
}

/*==========================================================================*/
/*  net_close() : ネットのクローズ                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_close(UInt16 netRef)
{
    return (NetLibClose(netRef, false));
}

/*==========================================================================*/
/*  net_write() : データを送信する                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_write(NNshWorkingInfo *NNshGlobal)
{
    NNshGlobal->ret = NetLibSend(NNshGlobal->netRef, 
                                 NNshGlobal->socketRef,
                                 NNshGlobal->sendBuf, 
                                 StrLen(NNshGlobal->sendBuf),
                                 0,
                                 NULL,
                                 0,
                                 NNshGlobal->timeout,
                                 &(NNshGlobal->err));
    if (NNshGlobal->ret != -1)
    {
        NNshGlobal->err = errNone;
    }
    return (NNshGlobal->err);
}

/*==========================================================================*/
/*  net_read() : データを受信する                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_read(NNshWorkingInfo *NNshGlobal)
{
    NNshGlobal->ret = 0;
    NNshGlobal->fieldLen = 0;
    while (NNshGlobal->ret < RECEIVEBUFFER_DEFAULT)
    {
        NNshGlobal->fieldLen = NetLibReceive(NNshGlobal->netRef, 
                                      NNshGlobal->socketRef,
                                      &(NNshGlobal->recvBuf[NNshGlobal->ret]),
                                      (RECEIVEBUFFER_DEFAULT
                                               - MARGIN - NNshGlobal->ret),
                                      0, NULL, 0, 
                                      NNshGlobal->timeout,
                                      &(NNshGlobal->err));
        if (NNshGlobal->fieldLen != -1)
        {
            NNshGlobal->err = errNone;
        }
        if (NNshGlobal->fieldLen == 0)
        {
            break;
        }

        NNshGlobal->ret = NNshGlobal->ret + NNshGlobal->fieldLen;

        // BUSYウィンドウを更新する
        MemSet (NNshGlobal->showBuf, MINIBUF, 0x00);
        StrIToA(NNshGlobal->showBuf, NNshGlobal->ret);
        StrCat (NNshGlobal->showBuf, " bytes...");
        SetMsg_BusyForm(NNshGlobal->showBuf);
    }
    return (NNshGlobal->err);
}

/*==========================================================================*/
/*  net_disconnect() : ホストから切り離す                                   */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_disconnect(UInt16 netRef, NetSocketRef socketRef, Int32 timeout)
{
    Err    err;

    (void) NetLibSocketShutdown(netRef, socketRef, netSocketDirBoth, timeout, &err);
    (void) NetLibSocketClose(netRef, socketRef, timeout, &err);
    if (err == errNone)
    {
        socketRef = 0;
    }
    return (errNone);
}

/*==========================================================================*/
/*  net_connect() : ホストに接続                                            */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_connect(NNshWorkingInfo *NNshGlobal)
{
    UInt8                openFlag;
    Int16                addrLen;
    NetHostInfoPtr       hostIP;
    NetSocketAddrINType *addrType;
    NetSocketAddrType    addrBuf;
    NetHostInfoBufType  *infoBuff;

    // 物理接続情報を更新(追加...)
    NetLibConnectionRefresh(NNshGlobal->netRef,
                            true,
                            &openFlag,
                            &(NNshGlobal->ret));

    // バッファ領域を確保
    infoBuff = MemPtrNew(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "MemPtrNew", "");
        return (~errNone);
    }
    MemSet(infoBuff, sizeof(NetHostInfoBufType), 0x00);

    // ホスト名からIPアドレスを取得
    hostIP = NetLibGetHostByName(NNshGlobal->netRef,
                                 NNshGlobal->hostName,
                                 infoBuff,
                                 NNshGlobal->timeout, 
                                 &(NNshGlobal->err));
    if (hostIP == 0)
    {
        // ホストIPの取得失敗
        NNshGlobal->err = NNSH_ERRORCODE_FAILURECONNECT;
        FrmCustomAlert(ALTID_INFO,
                       "NNshNet_connect",  
                       "NetLibGetHostByName :",
                       NNshGlobal->hostName);
        goto FUNC_END;
    }

    // ソケットOPEN
    NNshGlobal->socketRef = NetLibSocketOpen(NNshGlobal->netRef,
                                             netSocketAddrINET, 
                                             netSocketTypeStream,
                                             netSocketProtoIPTCP,
                                             NNshGlobal->timeout,
                                             &(NNshGlobal->err));
    if (NNshGlobal->socketRef == -1)
    {
        // ソケットOPEN異常
        FrmCustomAlert(ALTID_INFO, "NNshNet_connect",  "NetLibSocketOpen", "");
        NNshGlobal->socketRef = 0;
        NNshGlobal->err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

    // ソケットコネクト
    MemSet(&addrBuf, sizeof(NetSocketAddrType), 0x00);
    addrType = (NetSocketAddrINType *) &addrBuf;
    addrType->family = netSocketAddrINET;
    addrType->port   = NetHToNS(NNshGlobal->portNum);
    //addrType->addr   = infoBuff->address[0];
    MemMove(&(addrType->addr), *(hostIP->addrListP), hostIP->addrLen); 
    addrLen         = sizeof(NetSocketAddrType);
    NNshGlobal->err = errNone;
    NNshGlobal->ret = NetLibSocketConnect(NNshGlobal->netRef, 
                                          NNshGlobal->socketRef,
                                          (NetSocketAddrType *) &addrBuf,
                                          addrLen,
                                          NNshGlobal->timeout,
                                          &(NNshGlobal->err));
    if (NNshGlobal->ret == -1)
    {
        // ソケットコネクト異常
        FrmCustomAlert(ALTID_INFO,"NNshNet_connect","NetLibSocketConnect","");
        (void) NetLibSocketClose(NNshGlobal->netRef,
                                 NNshGlobal->socketRef,
                                 NNshGlobal->timeout,
                                 &(NNshGlobal->err));
        NNshGlobal->err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }
    NNshGlobal->err = errNone;

FUNC_END:
    // バッファ領域を開放
    (void) MemPtrFree(infoBuff);

    return (NNshGlobal->err);
}

/*==========================================================================*/
/*  NNshNet_LineHangup() : 回線切断                                         */
/*                                                                          */
/*==========================================================================*/
void NNshNet_LineHangup(void)
{
   UInt16 netRef;

   (void) SysLibFind("Net.lib", &(netRef));

    // 回線切断の確認
    if (netRef != 0)
    {
        (void) NetLibClose(netRef, true);
        (void) NetLibFinishCloseWait(netRef);
    }
    return;
}

/*==========================================================================*/
/*  NNshSSL_callbackFunc() : SSLのコールバック関数                          */
/*                                                                          */
/*==========================================================================*/
Int32 NNshSSL_callbackFunc(SslCallback *callbackStruct, Int32 command,
                           Int32 flavor, void *info)
{
  //  とりあえず、何もしない。。。
#if 0
    SslVerify *verifyData;

    switch (command)
    {
      case sslCmdVerify:
        // 'verify callback'
        verifyData = (SslVerify *) info;
        // この関数の後ろで吟味する
        break;

      case sslCmdInfo:
        // FrmCustomAlert(ALTID_INFO,"SSL_callback"," Info:","sslCmdInfo");
        return (errNone);
        break;
      case sslCmdNew:
        // FrmCustomAlert(ALTID_INFO,"SSL_callback"," New:","sslCmdNew");
        return (errNone);
        break;
      case sslCmdReset:
        // FrmCustomAlert(ALTID_INFO,"SSL_callback"," Reset:","sslCmdReset");
        return (errNone);
        break;

      case sslCmdFree:
        // FrmCustomAlert(ALTID_INFO,"SSL_callback"," Free:","sslCmdFree");
        return (errNone);
        break;

      default:
        // FrmCustomAlert(ALTID_INFO, "SSL_callback", " cmd:", "???");
        return (errNone);
        break;
    }

    // Verify(certificate)
    // FrmCustomAlert(ALTID_INFO, "SSL_callback", " code:", "flavor");
    switch (flavor)
    {
      case sslErrBadDecode:
        // うまく証明書がデコードできなかった
      case sslErrCert:
        // 一般的な証明エラー
      case sslErrCertDecodeError:
        // 証明書のデコードに失敗した
      case sslErrUnsupportedCertType:
        // サーバがわけのわからん（サポートしていない）証明書を送ってきた
      case sslErrUnsupportedSignatureType:
        // サーバがわけのわからんシグネチャを送ってきた
      case sslErrVerifyBadSignature:
        // 証明書のシグネチャが無効
      case sslErrVerifyNoTrustedRoot:
        // 証明書がない
      case sslErrVerifyNotAfter:
        // 証明書の有効期限が切れている
      case sslErrVerifyNotBefore:
        // 証明書が未来の日付になっている
      case sslErrVerifyConstraintViolation:
        // 証明書がX509の拡張に違反している
      case sslErrVerifyUnknownCriticalExtension:
        // X509拡張が証明書チェックルーチンで解釈できなかった
      case sslErrOk:
      default:
        // NULL
        break;
    }
#endif
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_open() : SSLライブラリのオープン                                */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_open(UInt16 sslMode, UInt16 *netRef,
                 UInt16 *sslRef, SslLib **template, SslContext **context)
{
    Err ret;

    // 変数の初期化
    *netRef   = 0;
    *sslRef   = 0;
    *template = NULL;
    *context  = NULL;

    // Netライブラリのオープン
    ret = NNshNet_open(netRef);
    if (ret != errNone)
    {
        *netRef = 0;
        return (ret);
    }

    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        // SSLモードでない、折り返す
        return (ret);
    }

    // SSLライブラリのオープン
    if (SysLibFind(kSslDBName, sslRef) != 0)
    {
        if (SysLibLoad(kSslLibType, kSslLibCreator, sslRef) != 0)
        {
            // ロード失敗、エラー応答する
            *sslRef = 0;
            return (~errNone);
        }
    }
    ret = SslLibOpen(*sslRef);
    if (ret != errNone)
    {
        // SSLライブラリオープン失敗、、、
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // generic SSL contextの作成
    ret = SslLibCreate (*sslRef, template);
    if (ret != errNone)
    {
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        return (ret);
    }

    // コンパチフラグのセット
    // SslLibSet_Compat(*sslRef, *template, sslCompatAll);

    // SSL contextの作成
    ret = SslContextCreate(*sslRef, *template, context);
    if (ret != errNone)
    {
        // generic SSL contextの廃棄
        (void) SslLibDestroy(*sslRef, *template);
        (void) SslLibClose(*sslRef);
        *sslRef = 0;
        *template = NULL;
        return (ret);
    }
    return (ret);
}

/*==========================================================================*/
/*  NNshSSL_close() : SSLライブラリのクローズ                               */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_close(UInt16 sslMode, UInt16 netRef,
                         UInt16 sslRef, SslLib *template, SslContext *context)
{
    if ((sslRef == 0)||(sslMode == NNSH_SSLMODE_NOTSSL))
    {
        NNshNet_close(netRef);
        return (errNone);
    }

    if (context != NULL)
    {
        (void) SslContextDestroy(sslRef, context);
    }

    if (template != NULL)
    {
        (void) SslLibDestroy(sslRef, template);
    }
    (void) SslLibClose(sslRef);

    (void) NNshNet_close(netRef);
    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_connect() : SSL接続                                             */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_connect(UInt16 sslMode, NNshWorkingInfo *NNshGlobal)
{
    Err ret;
    SslCallback verifyCallback;

    // TCP/IPでホストに接続する
    ret = NNshNet_connect(NNshGlobal);
    if ((ret != errNone)||(sslMode == NNSH_SSLMODE_NOTSSL))
    {
        return (ret);
    }

    // Verifyコールバック関数を設定する
    MemSet(&verifyCallback, sizeof(SslCallback), 0x00);
    verifyCallback.callback = NNshSSL_callbackFunc;
    SslContextSet_VerifyCallback(NNshGlobal->sslRef,
                                 NNshGlobal->sslContext,
                                 &verifyCallback);

    // ソケットをContextに設定、ホストに接続する
    //SslContextSet_Compat(sslRef, context, sslCompatAll);
    SslContextSet_Socket(NNshGlobal->sslRef,
                         NNshGlobal->sslContext,
                         NNshGlobal->socketRef);
    ret = SslOpen(NNshGlobal->sslRef, NNshGlobal->sslContext,
                  ((sslOpenModeSsl)|(sslOpenUseDefaultTimeout)), 0);
    return (ret);
}

/*==========================================================================*/
/*  NNshSSL_disconnect() : SSL切断                                          */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_disconnect(UInt16 sslMode, UInt16 sslRef, SslContext *context,
                       NetSocketRef socketRef, UInt16 netRef, Int32 timeout)
{
    Err ret;
    if (sslMode != NNSH_SSLMODE_NOTSSL)
    {
        ret = SslClose(sslRef, context, sslCloseUseDefaultTimeout, timeout);
    }
    ret = NNshNet_disconnect(netRef, socketRef, timeout);

    return (errNone);
}

/*==========================================================================*/
/*  NNshSSL_write() : データ送信(SSL)                                       */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_write(UInt16 sslMode, NNshWorkingInfo *NNshGlobal)
{
    // SSLモードでなければ、通常の書き込み処理を行う
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        return (NNshNet_write(NNshGlobal));
    }

    // データを書き込む
    NNshGlobal->ret = SslWrite(NNshGlobal->sslRef,
                               NNshGlobal->sslContext,
                               NNshGlobal->sendBuf,
                               StrLen(NNshGlobal->sendBuf),
                               &(NNshGlobal->err));
    if (NNshGlobal->ret  != -1)
    {
        NNshGlobal->err = errNone;
    }
    return (NNshGlobal->err);
}

/*==========================================================================*/
/*  NNshSSL_read() : データ受信(SSL)                                        */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_read(UInt16 sslMode, NNshWorkingInfo *NNshGlobal)
{
    // SSLモードでなければ、通常の読み出し処理を行う
    if (sslMode == NNSH_SSLMODE_NOTSSL)
    {
        return (NNshNet_read(NNshGlobal));
    }
    
    // データを読み込む
    NNshGlobal->ret = SslRead(NNshGlobal->sslRef,
                              NNshGlobal->sslContext,
                              NNshGlobal->recvBuf, 
                              NNshGlobal->ret,
                              &(NNshGlobal->err));
    if (NNshGlobal->ret != -1)
    {
        NNshGlobal->err = errNone;
    }
    return (NNshGlobal->err);
}

/*==========================================================================*/
/*  NNshSSL_flush() : 内部データバッファのクリア(SSL)                       */
/*                                                                          */
/*==========================================================================*/
Err NNshSSL_flush(UInt16 sslMode, UInt16  sslRef, SslContext *context)
{
    if (sslMode != NNSH_SSLMODE_NOTSSL)
    {
        // 内部データバッファのクリア指示
        SslContextSet_Mode(sslRef, context, (sslModeFlush));
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : SendToLocalMemopad                                         */
/*                    Exchangeマネージャを使用してメモ帳にデータを転送する */
/*                    (http://www.palmos.com/dev/support/docs/recipes/     */
/*                                    recipe_exg_mgr_send_local.html より) */
/*=========================================================================*/
Err SendToLocalMemopad(Char *title, Char *data)
{
    ExgSocketType *exgSocket;
    UInt32        size;
    Err           err;

    // データの初期化
    exgSocket = MemPtrNew(sizeof(ExgSocketType));
    if (exgSocket == NULL)
    {
        // 領域確保失敗
        return (~errNone);
    }
    MemSet(exgSocket, sizeof(ExgSocketType), 0x00);
    err  = errNone;
    size = StrLen(data) + 1;

    // レス転送後、メモ帳を起動しないようにする
    exgSocket->noGoTo      = 1;

    // レス転送中表示を行わない
    exgSocket->noStatus    = true;

    // will comm. with memopad app
    exgSocket->description = title;
    exgSocket->name        = "Message.txt";

    // set to comm. with local machine (PIM) only
    exgSocket->localMode = 1;

    err = ExgPut(exgSocket);
    if (err == errNone)
    {
        ExgSend(exgSocket, data, size, &err);
        ExgDisconnect(exgSocket, err);
    }

    // 確保領域の解放
    MEMFREE_PTR(exgSocket);

    return (err);
}

/*=========================================================================*/
/*   Function : ShowVersion_NNsh                                           */
/*                                                    バージョン情報の表示 */
/*=========================================================================*/
void ShowVersion_NNsh(void)
{
    Char *buffer;
    
    buffer = MemPtrNew(BUFSIZE * 2);
    if (buffer == NULL)
    {
        // 領域確保失敗
        return;
    }
    MemSet (buffer, (BUFSIZE * 2), 0x00);
    StrCopy(buffer, SOFT_NAME);
    StrCat (buffer, "\n");
    StrCat (buffer, SOFT_VERSION);
    StrCat (buffer, SOFT_REVISION);
    StrCat (buffer, "\n(");
    StrCat (buffer, SOFT_DATE);
    StrCat (buffer, ")\n    ");
    StrCat (buffer, SOFT_INFO);
    FrmCustomAlert(ALTID_INFO, buffer, "", "");

    MEMFREE_PTR(buffer);
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
    FrmDrawForm(frm);
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
/*   Function : DataInputDialog                                            */
/*                                                NNsi共通の入力ダイアログ */
/*=========================================================================*/
Boolean InputDialog(Char *title, Char *area, UInt16 size)
{
    Boolean       ret = false;
    FormType     *prevFrm, *diagFrm;
    FieldType    *fldP;

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // ダイアログフォームをアクティブにする
    diagFrm  = FrmInitForm(FRMID_INPUTDIALOG);
    if (diagFrm == NULL)
    {
        return (false);
    }
    FrmSetActiveForm(diagFrm);

    // タイトルをコピーする
    FrmCopyTitle(diagFrm, title);

    // 指定された文字列をウィンドウに反映させる
    NNshWinSetFieldText(diagFrm, FLDID_INPUTFIELD, area, size);
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_INPUTFIELD));

    fldP = FrmGetObjectPtr(diagFrm,
                           FrmGetObjectIndex(diagFrm, FLDID_INPUTFIELD));

    // 文字列を選択しておく
    FldSetSelection(fldP, 0, FldGetTextLength(fldP));

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_EditAction);

    if (FrmDoDialog(diagFrm) == BTNID_DIALOG_OK)
    {
        // OKボタンが押されたときには、文字列を取得する
        NNshWinGetFieldText(diagFrm, FLDID_INPUTFIELD, area, size);
        ret = true;
    }
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    return (ret);
}

/*=========================================================================*/
/*   Function : SetMsg_BusyForm                                            */
/*                                                     BUSYテキストの設定  */
/*=========================================================================*/
void SetMsg_BusyForm(Char *msg)
{
    FormType *busyFrm;
    UInt16    length;

    // 表示する文字列長のチェック
    length = StrLen(msg);
    length = (length > BUSYWIN_MAXLEN) ? BUSYWIN_MAXLEN : length;

    busyFrm = FrmGetActiveForm();
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, msg, length);

    return;
}

/*=========================================================================*/
/*   Function : Show_BusyForm                                              */
/*                                                      BUSYフォームの表示 */
/*=========================================================================*/
void Show_BusyForm(Char *msg, NNshWorkingInfo *NNshGlobal)
{
    FormType  *busyFrm;
    UInt16    length;

    switch (FrmGetActiveFormID())
    {
      case FRMID_BUSY:
        // すでにBUSYウィンドウが表示されていた場合
        return (SetMsg_BusyForm(msg));
        break;

      case 0:
        // ウィンドウが開かれていない
        NNshGlobal->prevBusyForm = NULL;
        break;

      default:
        // 現在のウィンドウを記憶する
        NNshGlobal->prevBusyForm = FrmGetActiveForm();
        break;
    }

    // フォームの現在の状態を記憶する
    // MemSet(&(NNshGlobal->formState), sizeof(FormActiveStateType), 0x00);
    // FrmSaveActiveState(&(NNshGlobal->formState));
    // NNshGlobal->prevBusyForm = FrmGetActiveForm();

    // BUSYダイアログフォームを生成、アクティブにする
    busyFrm = FrmInitForm(FRMID_BUSY);
    FrmSetActiveForm(busyFrm);

    // 表示する文字列長のチェック
    length = StrLen(msg);
    length = (length > BUSYWIN_MAXLEN) ? BUSYWIN_MAXLEN : length;

    // フィールドに文字列を設定
    NNshWinSetFieldText(busyFrm, FLDID_BUSY, msg, length);

    return;
}

/*=========================================================================*/
/*   Function : Hide_BusyForm                                              */
/*                                                      BUSYフォームの削除 */
/*=========================================================================*/
void Hide_BusyForm(NNshWorkingInfo *NNshGlobal)
{
    FormType *frm;

    if (FrmGetActiveFormID() != FRMID_BUSY)
    {
        // 何もしない
        return;
    }

    // BUSYフォームを削除する
    frm = FrmGetActiveForm();
    if (NNshGlobal->prevBusyForm != NULL)
    {
        // こんなんでいいのか...
        FrmEraseForm (frm);
        FrmSetActiveForm(NNshGlobal->prevBusyForm);
    }
    FrmDeleteForm(frm);

    // FrmRestoreActiveState(&(NNshGlobal->formState));

    return;
}

/**----------------------------------------------------------------------**
 **   get_html_contents   : HTMLデータを受信する
 **
 **----------------------------------------------------------------------**/
static void get_html_contents(NNshWorkingInfo *NNshGlobal)
{
    // メッセージを作成
    NNsh_MakeHTTPmsg(NNshGlobal);

    // （送信プロトコルに合わせて）HTML要求メッセージを送る
    if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) ==
                                                        NNSHLOGIN_SSL_PROTOCOL)
    {
        // https通信を要求
        NNshHTTPS_comm(NNshGlobal);
    }
    else
    {
        // http通信を要求
        NNshHTTP_comm(NNshGlobal);
    }
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        // 通信失敗、エラー応答する
        FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", " ", " ");
#endif
        return;
    }

#ifdef DEBUG
    // デバッグ用に受信データをメモ帳へ出力する
    StrCat(NNshGlobal->recvBuf, "\n\n---\n\n1st RECEIVE CONTENT\n\n");
    SendToLocalMemopad("RECEIVED DATA(1)", NNshGlobal->recvBuf);
#endif

    // 受信メッセージにリダイレクトの指示があるか、チェックを行う
    NNshGlobal->err = ~errNone - 9;
    if ((NNshGlobal->recvBuf[0]  == 'H')&&
        (NNshGlobal->recvBuf[1]  == 'T')&&
        (NNshGlobal->recvBuf[2]  == 'T')&&
        (NNshGlobal->recvBuf[3]  == 'P')&&
        (NNshGlobal->recvBuf[9]  == '3')&&
        (NNshGlobal->recvBuf[10] == '0')&&
        (NNshGlobal->recvBuf[11] == '2'))
    {
        // リダイレクトのホスト名とサイトを抽出する
        NNsh_check302URL(NNshGlobal);
    }

    // リダイレクトのアドレスを取得できたとき...
REDIRECT_AGAIN:
    if (NNshGlobal->err == errNone)
    {
        // メッセージを作成
        NNsh_MakeHTTPmsg(NNshGlobal);
        // リダイレクト先に（送信プロトコルに合わせて）送る
        if ((NNshGlobal->sendMethod & NNSHLOGIN_SSL_PROTOCOL) ==
                                                        NNSHLOGIN_SSL_PROTOCOL)
        {
            // https通信を要求
            NNshHTTPS_comm(NNshGlobal);
        }
        else
        {
            // http通信を要求
            NNshHTTP_comm(NNshGlobal);
        }
        if (NNshGlobal->err != errNone)
        {
#ifdef DEBUG
            // 通信失敗、エラー応答する
            FrmCustomAlert(ALTID_INFO, "NNshHTTP_comm", " ", "(redirect)");
#endif
            return;
        }
    }

    // 再度ページを開かなくてはならない場合...
    NNsh_checkRefreshConnectionRequest(NNshGlobal);
    if (NNshGlobal->err == errNone)
    {
#ifdef DEBUG
        // 再度リダイレクトすることを送信する
        FrmCustomAlert(ALTID_INFO, 
                       "REDIRECT AGAIN...(", NNshGlobal->reqAddress, ")");
#endif
        goto REDIRECT_AGAIN;
    }

#ifdef DEBUG
    // デバッグ用に受信データをメモ帳へ出力する
    StrCat(NNshGlobal->recvBuf, "\n\n---\n\nRECEIVE CONTENT\n\n");
    SendToLocalMemopad("RECEIVED DATA", NNshGlobal->recvBuf);
#endif

}

/**----------------------------------------------------------------------**
 **    受信メッセージのリダイレクト先(ログインページ)を解析する
 ** 
 **                    NNshGlobal->recvBuf ： 受信ボディ
 **
 **
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
void NNsh_check302URL(NNshWorkingInfo *NNshGlobal)
{
    NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "http");
    if (NNshGlobal->tempPtr == NULL)
    {
        // Redirectホスト名の読み出し失敗
        NNshGlobal->err = ~errNone;
        return;
    }

#ifdef USE_REFERER
    // リダイレクトされたアドレスをRefererに設定する
    MemSet(NNshGlobal->referer, BUFSIZE, 0x00);
    for (NNshGlobal->loopCount = 0; 
         NNshGlobal->loopCount < (BUFSIZE - 1); 
         (NNshGlobal->loopCount)++)
    {
        if (NNshGlobal->tempPtr[(NNshGlobal->loopCount)] <= ' ')
        {
            // URLの末尾に到達(たぶん、本来は \r で抜ける...)
            break;
        }
        NNshGlobal->referer[(NNshGlobal->loopCount)] =
                                  NNshGlobal->tempPtr[(NNshGlobal->loopCount)];
    }
#endif

    // プロトコルのチェック
    NNshGlobal->sendMethod = 0;
    if (NNshGlobal->tempPtr[4] == 's')
    {
        // https (SSL)
        NNshGlobal->sendMethod = NNSHLOGIN_SSL_PROTOCOL;
    }
    NNshGlobal->tempPtr = StrStr(NNshGlobal->tempPtr, "://");
    if (NNshGlobal->tempPtr == NULL)
    {
        // ホスト名抽出に失敗
        NNshGlobal->err = ~errNone + 100;
        return;
    }
    NNshGlobal->tempPtr = NNshGlobal->tempPtr + 3;  // 3は、StrLen("://");

    // ホスト名のコピー
    MemSet(NNshGlobal->hostName, BUFSIZE, 0x00);
    for (NNshGlobal->loopCount = 0; 
         NNshGlobal->loopCount < (BUFSIZE - 1); 
         (NNshGlobal->loopCount)++)
    {
        if (*(NNshGlobal->tempPtr) == '/')
        {
            // ホスト名の終端
            // (NNshGlobal->tempPtr)++;  // /からはじめる...
            break;
        }
        if (*(NNshGlobal->tempPtr) == ':')
        {
            // ポート番号が指定されている...
            NNshGlobal->sendMethod =
                       (NNshGlobal->sendMethod)|(NNSHLOGIN_CONNECT_PORT);
            (NNshGlobal->tempPtr)++;

            // ポート番号の特定
            NNshGlobal->portNum = 0;
            while ((*(NNshGlobal->tempPtr) >= '0')&&
                   (*(NNshGlobal->tempPtr) <= '9'))
            {
                NNshGlobal->portNum = NNshGlobal->portNum * 10 + 
                                         (*(NNshGlobal->tempPtr) - '0');
                (NNshGlobal->tempPtr)++;
            }
            //(NNshGlobal->tempPtr)++;  // /からはじめる...
            break;
        }
        NNshGlobal->hostName[NNshGlobal->loopCount] = *(NNshGlobal->tempPtr);
        (NNshGlobal->tempPtr)++;
    }

    // 通信先ファイル名を取得する
    NNshGlobal->loopCount = 0; 
    if (*(NNshGlobal->tempPtr) != '/')
    {
        // 相対アドレスなので、現在の要求pathを生かすようにする
        NNshGlobal->loopCount = StrLen(NNshGlobal->reqAddress);
        while (NNshGlobal->loopCount != 0)
        {
            (NNshGlobal->loopCount)--;
            if (NNshGlobal->reqAddress[NNshGlobal->loopCount] == '/')
            {
                break;
            }
        }
        NNshGlobal->reqAddress[NNshGlobal->loopCount] = '/';
        (NNshGlobal->loopCount)++;
    }
    else
    {
        MemSet(NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
    }
    while (NNshGlobal->loopCount < (BUFSIZE - 1))
    {
        if (*(NNshGlobal->tempPtr) <= ' ')
        {
            break;
        }
        NNshGlobal->reqAddress[NNshGlobal->loopCount] = *(NNshGlobal->tempPtr);
        (NNshGlobal->tempPtr)++;
        (NNshGlobal->loopCount)++;
    }

    // アドレスが何もなかったら / にする
    if (StrLen(NNshGlobal->reqAddress) == 0)
    {
        StrCopy(NNshGlobal->reqAddress, "/");
    }
#ifdef DEBUG
    // 解析結果のログを出力する
    FrmCustomAlert(ALTID_INFO, NNshGlobal->hostName, ":", NNshGlobal->reqAddress);
#endif

    // 正常終了する
    NNshGlobal->err = errNone;
    return;
}

/**----------------------------------------------------------------------**
 **    受信メッセージのリダイレクト先(ログインページ)を再度解析する
 ** 
 **                    NNshGlobal->recvBuf ： 受信ボディ
 **
 **
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **                    (NnshGlobal->errがerrNoneのときは再度URLを取得する)
 **----------------------------------------------------------------------**/
static void NNsh_checkRefreshConnectionRequest(NNshWorkingInfo *NNshGlobal)
{
    Char   *ptr;

    ///////////// リフレッシュ指示がある？ //////////
    NNshGlobal->err = ~errNone;
    ptr = StrStr(NNshGlobal->recvBuf, "http-equiv=\"Refresh\"");
    if (ptr == NULL)
    {
        // ない場合には、なにもしない
        NNshGlobal->err = ~errNone;
        return;
    }

    if (NNshGlobal->isChecking != 0)
    {
        // 一度チェック済み、２度目はなにもしない
        NNshGlobal->err = ~errNone;
        return;
    }

    ptr = StrStr(NNshGlobal->recvBuf, "URL=");
    if (ptr == NULL)
    {
        ptr = StrStr(NNshGlobal->recvBuf, "url=");
    }
    if (ptr == NULL)
    {
        // URLを拾えなかった、終了する
        NNshGlobal->err = ~errNone;
        return;
    }
    ptr = ptr + sizeof("url");


    // 要求アドレスの先頭を探す
    while ((*ptr != '/')&&(*ptr != '\0')&&(*ptr != '"'))
    {
        ptr++;
    }
    // スラッシュは先送り...
    while (*ptr == '/')
    {
        ptr++;
    }

    // URL指定でフルセット指定かどうかチェックする
    NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "://");
    if ((NNshGlobal->tempPtr == NULL)||(NNshGlobal->tempPtr > ptr))
    {
        //  フルセット指定ではない、ホスト名(とポート番号)の切り出しは
        // 行わない...
        goto GET_CONTENT_NAME;
    }

    // ホスト名のきりだしを行う(ホスト名のコピー)
    MemSet(NNshGlobal->hostName, BUFSIZE, 0x00);
    for (NNshGlobal->isChecking = 0; 
         NNshGlobal->isChecking < (BUFSIZE - 1); 
         (NNshGlobal->isChecking)++)
    {
        if (*ptr == '/')
        {
            // ホスト名の終端
            break;
        }
        if (*ptr == ':')
        {
            // ポート番号が指定されている...
            NNshGlobal->sendMethod =
                       (NNshGlobal->sendMethod)|(NNSHLOGIN_CONNECT_PORT);
            ptr++;

            // ポート番号の特定
            NNshGlobal->portNum = 0;
            while ((*ptr >= '0')&&(*ptr <= '9'))
            {
                NNshGlobal->portNum = NNshGlobal->portNum * 10 + (*ptr - '0');
                ptr++;
            }
            break;
        }
        NNshGlobal->hostName[NNshGlobal->isChecking] = *ptr;
        ptr++;
    }
GET_CONTENT_NAME:
    NNshGlobal->isChecking = 0;
    while ((*ptr != '\0')&&(*ptr != '>')&&(*ptr != '"'))
    {
        NNshGlobal->reqAddress[NNshGlobal->isChecking] = *ptr;
        (NNshGlobal->isChecking)++;
        ptr++;
    }
    NNshGlobal->reqAddress[NNshGlobal->isChecking] = '\0';

    // これでもう一度ログインフォームを取得しなおす
    NNshGlobal->err = errNone;
    return;
}

/**----------------------------------------------------------------------**
 **    HTTPS通信(SSL)を実施
 **                    NNshGlobal->workBuf ： 送信したいデータ
 **                    NNshGlobal->sendBuf ： 送信ボディ(HTTPヘッダつき)
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
void NNsh_MakeHTTPmsg(NNshWorkingInfo *NNshGlobal)
{
#ifdef USE_RESOURCE
    MemHandle  strH;
#endif

    MemSet(NNshGlobal->sendBuf, (SENDBUFFER_DEFAULT), 0x00);
    switch (NNshGlobal->sendMethod)
    {
      case NNSILOGIN_POST_METHOD_SSL:
        StrCopy(NNshGlobal->sendBuf, "POST ");
        NNshGlobal->portNum = 443;
        break;

      case NNSILOGIN_POST_METHOD_HTTP:
        StrCopy(NNshGlobal->sendBuf, "POST ");
        NNshGlobal->portNum = 80;
        break;

      case NNSILOGIN_GET_METHOD_SSL:
        StrCopy(NNshGlobal->sendBuf, "GET ");
        NNshGlobal->portNum = 443;
        break;

      case NNSILOGIN_GET_METHOD_HTTP:
        StrCopy(NNshGlobal->sendBuf, "GET ");
        NNshGlobal->portNum = 80;
        break;

      case NNSILOGIN_POST_METHOD_OTHER:
      case NNSILOGIN_POST_METHOD_OTHERSSL:
        StrCopy(NNshGlobal->sendBuf, "POST ");
        break;

      case NNSILOGIN_GET_METHOD_OTHER:
      case NNSILOGIN_GET_METHOD_OTHERSSL:
      default:
        StrCopy(NNshGlobal->sendBuf, "GET ");
        break;
    }

    if (NNshGlobal->reqAddress[0] != '/')
    {
        // 絶対パスで指示が飛ばない場合の補足...
        StrCat(NNshGlobal->sendBuf, "/");
    }
    StrCat(NNshGlobal->sendBuf, NNshGlobal->reqAddress);
    StrCat(NNshGlobal->sendBuf, " HTTP/1.0\r\nHost: ");
    StrCat(NNshGlobal->sendBuf, NNshGlobal->hostName);

    // refererが設定されていた場合には付加する
#ifdef USE_REFERER
    if (StrLen(NNshGlobal->referer) != 0)
    {
        StrCat(NNshGlobal->sendBuf, "\r\nReferer: ");
        StrCat(NNshGlobal->sendBuf, NNshGlobal->referer);
    }
#endif

    // ボディ部がない場合には、Content-Length, Content-Typeをつけない
    if (StrLen(NNshGlobal->workBuf) > 1)
    {
        StrCat(NNshGlobal->sendBuf, "\r\nContent-Length: ");
        StrIToA(&NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)], 
                                      StrLen(NNshGlobal->workBuf));
#ifdef USE_FORM_SEND
        StrCat(NNshGlobal->sendBuf, "\r\nContent-Type: application/x-www-form-urlencoded");
#endif
    }

    // 送信データサイズを限定させる
    StrCat(NNshGlobal->sendBuf, "\r\nRange: bytes=0-");
    StrIToA(&(NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)]),
            (RECEIVEBUFFER_DEFAULT - HEADER_BUFFER));

    // Cookieを追加
    if (NNshGlobal->cookie[0] != '\0')
    {
        StrCat(NNshGlobal->sendBuf, "\r\n");
        StrCat(NNshGlobal->sendBuf, NNshGlobal->cookie);
    }

    // ヘッダの残り部分を付加
    StrCat(NNshGlobal->sendBuf, "\r\nAccept: text/html, */*\r\nUser-Agent: ejDA/1.00 (PDA; PalmOS/NNsi family)\r\nAccept-Encoding: identity\r\nAccept-Language: ja, en\r\nConnection: close\r\nPragma: no-cache\r\n\r\n");

    StrCat(NNshGlobal->sendBuf, NNshGlobal->workBuf);

    return;
}

/**----------------------------------------------------------------------**
 **    HTTPS通信(SSL)を実施
 **                                NNshGlobal->sendBuf ： 送信データ
 **                                NNshGlobal->recvBuf ： 受信データ
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
void NNshHTTPS_comm(NNshWorkingInfo *NNshGlobal)
{
    // 受信バッファの掃除
    MemSet(NNshGlobal->recvBuf, RECEIVEBUFFER_DEFAULT, 0x00);

    // PalmOS v5.20よりも小さい場合には起動しない
    if (NNshGlobal->palmOSVersion < 0x05200000)
    {
        FrmCustomAlert(ALTID_INFO, "a https-request is need a SslLib."
                       "(You should USE a PalmOS 5.2 or later).","","[webDA]");
        return;
    }

    // SSLのオープン
    NNshGlobal->err = 
          NNshSSL_open(NNSH_SSLMODE_DOSSL,    &(NNshGlobal->netRef), 
                       &(NNshGlobal->sslRef), &(NNshGlobal->sslTemplate),
                       &(NNshGlobal->sslContext));
    if (NNshGlobal->err != errNone)
    {
        // 接続に失敗
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_open", " ", NNshGlobal->hostName);
#endif
        return;
    }

    // SSLのコネクト
    NNshGlobal->err = NNshSSL_connect(NNSH_SSLMODE_DOSSL, NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
        // 接続に失敗
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_connect", " ", NNshGlobal->hostName);
#endif
        goto HTTPS_CLOSE;
    }

    // 要求の送信
    NNshGlobal->err = NNshSSL_write(NNSH_SSLMODE_DOSSL, NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
        // 接続に失敗
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_write", " ", NNshGlobal->hostName);
#endif
        goto HTTPS_DISCONNECT;
    }

    // 読み込み
    NNshGlobal->err = NNshSSL_read(NNSH_SSLMODE_DOSSL, NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
        // 接続に失敗
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "SSL_read", " ", NNshGlobal->hostName);
#endif
        goto HTTPS_DISCONNECT;
    }

HTTPS_DISCONNECT:
    // 接続を切る
    NNshSSL_disconnect(NNSH_SSLMODE_DOSSL, NNshGlobal->sslRef,
                       NNshGlobal->sslContext,  NNshGlobal->socketRef,
                       NNshGlobal->netRef,      NNshGlobal->timeout);
HTTPS_CLOSE:
    // クローズ
    NNshSSL_close(NNSH_SSLMODE_DOSSL, NNshGlobal->netRef,
                  NNshGlobal->sslRef, NNshGlobal->sslTemplate,
                  NNshGlobal->sslContext);

    return;
}

/**----------------------------------------------------------------------**
 **    HTTP通信を実施
 **                                NNshGlobal->sendBuf ： 送信データ
 **                                NNshGlobal->recvBuf ： 受信データ
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
void NNshHTTP_comm(NNshWorkingInfo *NNshGlobal)
{
    // 受信バッファの掃除
    MemSet(NNshGlobal->recvBuf, RECEIVEBUFFER_DEFAULT, 0x00);

    // ネットライブラリOPEN
    NNshGlobal->err = NNshNet_open(&(NNshGlobal->netRef));
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Net_open", " ", NNshGlobal->hostName);
#endif
        return;
    }
    // コネクト
    NNshGlobal->err = NNshNet_connect(NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "Net_connect", " ", NNshGlobal->hostName);
#endif
        goto HTTP_CLOSE;
    }
    SetMsg_BusyForm("Sending a request...");

    // 書き込み
    NNshGlobal->err = NNshNet_write(NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "NNshNet_write",  "", "");
#endif
        goto HTTP_CLOSE;
    }
    SetMsg_BusyForm("Receiving a message...");

    // 読み出し
    NNshGlobal->err = NNshNet_read(NNshGlobal);
    if (NNshGlobal->err != errNone)
    {
        // 読み出し失敗
#ifdef DEBUG
        FrmCustomAlert(ALTID_INFO, "NNshNet_read",  "", "");
#endif
        goto HTTP_DISCONNECT;
    }

    SetMsg_BusyForm("Disconnecting...");

HTTP_DISCONNECT:
    // ソケット切断
    NNshNet_disconnect(NNshGlobal->netRef, NNshGlobal->socketRef, 
                                                        NNshGlobal->timeout);
HTTP_CLOSE:
    NNshNet_close(NNshGlobal->netRef);

    return;
}

/**************************************************************************/
/*   Cookieを拾う                                                         */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
void pickup_cookie(NNshWorkingInfo *NNshGlobal)
{
    Char *ptr;

    MemSet(NNshGlobal->cookie, (BUFSIZE + MARGIN), 0x00);
    ptr = StrStr(NNshGlobal->recvBuf, "Set-Cookie");
    if (ptr == NULL)
    {
        // cookieの指定がなかった、終了する
        return;
    }

    // Cookieデータをコピーする
    ptr = ptr + StrLen("Set-");
    MemMove(NNshGlobal->cookie, ptr, BUFSIZE);
    ptr = NNshGlobal->cookie;
    while ((*ptr != ';')&&(*ptr != '\0'))
    {
        ptr++;
    }
    *ptr = '\0';
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : ParseMessage                                               */
/*                                                  (メッセージの整形処理) */
/*                    buf : コピー先領域                                   */
/*                    src : コピー元領域                                   */
/*                   size : コピー元バッファサイズ                         */
/*              kanjiCode : 使用する漢字コード                             */
/*-------------------------------------------------------------------------*/
static void ParseMessage(Char *buf, Char *src, UInt32 dstSize, UInt32 size, UInt16 kanjiCode)
{
    Boolean kanjiMode;
    UInt16  len;
    Char   *ptr, *dst, dataStatus;

    dataStatus = MSGSTATUS_NAME;
    kanjiMode = false;

    // 超遅い解釈ルーチンかも... (１文字づつパースする)
    dst  = buf;
    ptr  = src;

    while (ptr < (src + size))
    {
        // コピー先バッファが不足しそうだったら終了する
        if ((dst + MARGIN) > (buf + dstSize))
        {
            break;
	}

        // JIS漢字コードの変換ロジック
        if (kanjiCode == NNSH_KANJICODE_JIS)
        {
            // JIS→SHIFT JISのコード変換
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x24')&&(*(ptr + 2) == '\x42'))
            {
                // 漢字モードに切り替え
                kanjiMode = true;
                ptr = ptr + 3;
                continue;
            }
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x28')&&(*(ptr + 2) == '\x42'))
            {
                // ANKモードに切り替え
                kanjiMode = false;
                ptr = ptr + 3;
                continue;
            }
            if (kanjiMode == true)
            {
                // JIS > SJIS 漢字変換
                ConvertJIStoSJ(dst, ptr);
                ptr = ptr + 2;
                dst = dst + 2;
                continue;
            }
        }
        if (*ptr == '&')
        {
            // "&gt;" を '>' に置換
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&lt;" を '<' に置換
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '<';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&quot;" を '"' に置換
            if ((*(ptr + 1) == 'q')&&(*(ptr + 2) == 'u')&&
                (*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&(*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" を '    ' に置換
            if ((*(ptr + 1) == 'n')&&(*(ptr + 2) == 'b')&&
                (*(ptr + 3) == 's')&&(*(ptr + 4) == 'p')&&(*(ptr + 5) == ';'))
            {
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&amp;" を '&' に置換
            if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
            {
                *dst++ = '&';
                ptr = ptr + 5;    // StrLen(TAG_AMP);
                continue;
            }
            // これはありえないはずだが...一応。
            *dst++ = '&';
            ptr++;
            continue;
        }
        if (*ptr == '<')
        {
            //  "<>" は、セパレータ(状態によって変わる)
            if (*(ptr + 1) == '>')
            {
                ptr = ptr + 2;   // StrLen(DATA_SEPARATOR);
                switch (dataStatus)
                {
                  case MSGSTATUS_NAME:
                    // 名前欄の区切り
                    *dst++ = ' ';
                    *dst++ = ' ';
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_EMAIL;
                    break;

                  case MSGSTATUS_EMAIL:
                    // e-mail欄の区切り
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_DATE;
                    *dst++ = '\n';
                    break;

                  case MSGSTATUS_DATE:
                    // 年月日・時間およびＩＤ欄の区切り
                    *dst++ = '\n';
                    *dst++ = '\n';
                    dataStatus = MSGSTATUS_NORMAL;
                    if (*ptr == ' ')
                    {
                        // スペースは読み飛ばす
                        ptr++;
                    }
                    break;

                  case MSGSTATUS_NORMAL:
                  default:
                    // メッセージの終端
                    *dst++ = ' ';
                    *dst++ = '\0';
                    return;
                    break;
                }
                continue;
            }
            //  "<br>" は、改行に置換
            if (((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                // 行末と行頭のスペースを削ってみる場合
                if ((ptr > src)&&(*(ptr - 1) == ' '))
                {
                    dst--;
                }
                if (*(ptr + 4) == ' ')
                {
                    *dst++ = '\n';
                    ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                    continue;
                }
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<p*>" は、改行2つに置換
            if ((*(ptr + 1) == 'p')||(*(ptr + 1) == 'P'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 2;
                while ((*ptr != '>')&&(ptr < (src + size)))
                {
                    ptr++;
                }
                ptr++;
                continue;
            }

            // <li>タグを改行コードと:に置換する
            if (((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'i')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'L')&&(*(ptr + 2) == 'I')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '･';
                ptr = ptr + 4;
                continue;
            }   

            //  "<hr>" は、改行 === 改行 に置換
            if (((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'H')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dt>"は、改行に置換
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "</tr>" および "</td>" は、改行に置換
            if (((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'r')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'R')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'd')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'D')&&(*(ptr + 4) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dd>" は、改行と空白２つに置換
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'd')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'D')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<h?" は、改行 + 改行 に置換
            if ((*(ptr + 1) == 'h')&&(*(ptr + 1) == 'H'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "</h?>" は、改行 + 改行 に置換
            if (((*(ptr + 2) == 'h')||(*(ptr + 2) == 'H'))&&(*(ptr + 1) == '/'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 3;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "</title>" は、改行 + 改行 に置換
            if ((*(ptr + 1) == '/')&&
                ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
                ((*(ptr + 3) == 'i')||(*(ptr + 3) == 'I'))&&
                ((*(ptr + 4) == 't')||(*(ptr + 4) == 'T'))&&
                ((*(ptr + 5) == 'l')||(*(ptr + 5) == 'L'))&&
                ((*(ptr + 6) == 'e')||(*(ptr + 6) == 'E')))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 7;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            // <tr>, <td> はスペース１つに変換
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R')||
                 (*(ptr + 2) == 'd')||(*(ptr + 2) == 'D')))
            {
                *dst++ = ' ';
                // continueはなし。。 (タグは読み飛ばすため)
            }
SKIP_TAG:
            // その他のタグは読み飛ばす
#ifdef USE_STRSTR
            ptr = StrStr(ptr, ">");
#else
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
#endif
            ptr++;
            // *dst++ = ' ';   // タグは完全無視とする。
            continue;
        }
        // 漢字コードがEUCだった場合...
        if (kanjiCode == NNSH_KANJICODE_EUC)
        {
            if (ConvertEUCtoSJ((UInt8 *) dst, (UInt8 *) ptr, &len) == true)
            {
                // EUC > SHIFT JIS変換を実行した
                dst = dst + len;
                ptr = ptr + 2;
                continue;
            }
            if (*ptr != '\0')
            {
                // 普通の一文字転写
                *dst++ = *ptr;
            }
            ptr++;
            continue;
        }

        // スペースが連続していた場合、１つに減らす
        if ((*ptr == ' ')&&(*(ptr + 1) == ' '))
        {
            ptr++;
            while ((*ptr == ' ')&&(ptr < (src + size)))
            {
                ptr++;
            }
            ptr--;
        }

        // NULL および 0x0a, 0x0d, 0x09(タブ) は無視する
        if ((*ptr != '\0')&&(*ptr != 0x0a)&&(*ptr != 0x0d)&&(*ptr != 0x09))
        {
            // 普通の一文字転写
            *dst++ = *ptr;
        }
        ptr++;
    }
    *dst++ = '\0';
    return;
}

/* --------------------------------------------------------------------------
 *   AMsoftさんのUTF8変換テーブルを利用して実現する
 *       (http://amsoft.minidns.net/palm/gfmsg_chcode.html)
 * --------------------------------------------------------------------------*/

/*=========================================================================*/
/*   Function : StrNCopyUTF8toSJ                                           */
/*                     文字列のコピー(UTF8からSHIFT JISコードへの変換実施) */
/*=========================================================================*/
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, LocalID dbId)
{
    UInt8     buffer[2];
    UInt16    cnt, codes, ucode;
    Boolean   kanji;
    DmOpenRef dbRef;   

    // 変換テーブルがあるか確認する
    // dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        return;
    }
    dbRef = DmOpenDatabase(0 , dbId, dmModeReadOnly);

    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt <size))
    {
        if (*src < 0x80)
        {
            // 半角モード...
            *dst = *src;
            dst++;
            cnt++;
            src++;
            continue;
        }

        ucode = ((*src & 0x0f) << 12);
        ucode = ucode | ((*(src + 1) & 0x3f) << 6);
        ucode = ucode | ((*(src + 2) & 0x3f));

        buffer[0] = ((ucode & 0xff00) >> 8);
        buffer[1] = ((ucode & 0x00ff));

        // 漢字コードの変換
        codes = Unicode11ToJisx0208(dbRef, buffer[0], buffer[1]);
        buffer[0] = ((codes & 0xff00) >> 8);
        buffer[1] = (codes & 0x00ff);
        ConvertJIStoSJ(dst, buffer);

        dst = dst + 2;
        src = src + 3;
        cnt = cnt + 2;
    }
    *dst = '\0';

    DmCloseDatabase(dbRef);
    return;
}

/* --------------------------------------------------------------------------
 *  Unicode11ToJisx0208()
 *                                                       (Unicode > JIS0208)
 * --------------------------------------------------------------------------*/
static UInt16 Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L)
{
    MemHandle            recH;
    UInt16               num, col, code = 0x0000;
    UnicodeToJis0208Rec *rec;

    // データベースのレコードがなかったとき...
    if (dbRef == 0)
    {
        // エラー...
        return (0x2121);
    }
    num  = utf8H;

    // 現物あわせ､､､
    if (utf8L < 0x80)
    {
        num = (utf8H * 2);
        col = utf8L;
    }
    else
    {
        num = (utf8H * 2) + 1;
        col = utf8L - 0x80;
    }
    
    recH = DmQueryRecord(dbRef, num);
    if (recH == 0)
    {
        // エラー...
        return (0x2122);
    }
    rec = (UnicodeToJis0208Rec *) MemHandleLock(recH);
    if (rec->size > 0)
    {
        // データ有り
        code  = rec->table[col];
    }
    else
    {
        // データなし
        code = 0x2121;
    }
    MemHandleUnlock(recH);
    return (code);
}

/*=========================================================================*/
/*   Function : ConvertJIStoSJ                                             */
/*                            JIS漢字コードをSHIFT JIS漢字コードに変換する */
/*=========================================================================*/
static Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr)
{
    UInt8 upper, lower;

    // 上位8ビット/下位8ビットを変数にコピーする
    upper = *ptr;
    lower = *(ptr + 1);

    // 奥村さんのC言語によるアルゴリズム辞典(p.110)のコードを使わせていただく
    if ((upper & 1) != 0)
    {
        if (lower < 0x60)
        {
            lower = lower + 0x1f;
        }
        else
        {
            lower = lower + 0x20;
        }
    }
    else
    {
        lower = lower + 0x7e;
    }   
    if (upper < 0x5f)
    {
        upper = (upper + 0xe1) >> 1;
    }
    else
    {
        upper = (((upper + 0x61) >> 1)|(0x80));
    }
    *dst = upper;
    dst++;
    *dst = lower;

    return (true);
}

/*=========================================================================*/
/*   Function : ConvertEUCtoSJ                                             */
/*                                  漢字コードをEUCからSHIFT JISに変換する */
/*=========================================================================*/
static Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte)
{
    UInt8 temp[2];

    *copyByte = 0;
    if (*ptr == 0x8e)
    {
        // 半角カナコード
        ptr++;
        *dst = *ptr;
        *copyByte = 1;
        return (true); 
    }
    if ((*ptr >= ((0x80)|(0x21)))&&(*ptr <= ((0x80)|(0x7e))))
    {
        // EUC漢字コードと判定、JIS漢字コードに一度変換してからSHIFT JISに変換
        temp[0] = ((*ptr)&(0x7f));
        ptr++;
        temp[1] = ((*ptr)&(0x7f));
        (void) ConvertJIStoSJ(dst, temp);
        *copyByte = 2;
        return (true);
    }
    return (false);
}

/*#------------------------------------------------------------------------#*/
/*#   analysis_URL (URLの解析)                                             #*/
/*#                                      true: 解析成功  false: 解析失敗   #*/
/*#------------------------------------------------------------------------#*/
static Boolean analysis_URL(NNshWorkingInfo *NNshGlobal)
{
    UInt16  len;
    UInt8  *ptr;

    NNshGlobal->sendMethod = NNSILOGIN_GET_METHOD_HTTP;
    NNshGlobal->portNum = 80;
    len = sizeof("http://") - 1;
    ptr = StrStr(NNshGlobal->reqURL, "http://");
    if (ptr == NULL)
    {
        ptr = StrStr(NNshGlobal->reqURL, "https://");
        if (ptr == NULL)
        {
            return (false);
	}
        // SSLプロトコルだった...
        len = sizeof("https://") - 1;
        NNshGlobal->portNum = 443;
        NNshGlobal->sendMethod = NNSHLOGIN_SSL_PROTOCOL;

        // PalmOS v5.20よりも小さい場合にはNG応答(SslLibが必要なので)
        if (NNshGlobal->palmOSVersion < 0x05200000)
        {
            FrmCustomAlert(ALTID_INFO, "a https-request is need a SslLib."
                           "(You should USE a PalmOS 5.2 or later).","",
                           "[webDA]");
            return (false);
        }
    }
    ptr = ptr + len;

    // HOST名抽出
    len = 0;
    MemSet(NNshGlobal->hostName, BUFSIZE, 0x00);
    while ((*ptr != '/')&&(*ptr != ':')&&(*ptr != '\0'))
    {
        NNshGlobal->hostName[len] = *ptr;
        len++;
        ptr++;
    }
    NNshGlobal->hostName[len] = '\0';

    // port番号抽出
    if (*ptr == ':')
    {
        NNshGlobal->portNum = 0;
        while ((*ptr >= '0')&&
               (*ptr <= '9'))
        {
            NNshGlobal->portNum = NNshGlobal->portNum * 10 + (*ptr - '0');
            ptr++;
        }
        NNshGlobal->sendMethod
                        = (NNshGlobal->sendMethod) | (NNSHLOGIN_CONNECT_PORT);
    }

    // 要求アドレス抽出
    if ((*ptr == '/')&&(*ptr == '/'))
    {
        ptr++;
    }

    len = 0;
    while ((*ptr != '>')&&(*ptr > ' ')&&(*ptr < 0x80))
    {
        NNshGlobal->reqAddress[len] = *ptr;
        len++;
        ptr++;
    }
    NNshGlobal->reqAddress[len] = '\0';
    if (len == 0)
    {
        NNshGlobal->reqAddress[len] = '/';
        len++;
        NNshGlobal->reqAddress[len] = '\0';
    }
    return (true);
}

/**----------------------------------------------------------------------**
 **    FORMタグの場所を取得する
 **
 **                  NNshGlobal->tempBufに検出結果を格納する
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
void pickup_formTag(NNshWorkingInfo *NNshGlobal)
{
    Char  *ptr;
    UInt16 loop;

    // FORMタグの先頭を探す
    NNshGlobal->tempPtr2 = StrStr(NNshGlobal->recvBuf, "<FORM");
    if (NNshGlobal->tempPtr2 == NULL)
    {
        NNshGlobal->tempPtr2 = StrStr(NNshGlobal->recvBuf, "<form");
    }
    if (NNshGlobal->tempPtr2 == NULL)
    {
        // FORMタグを発見できなかった、、、
        NNshGlobal->err = ~errNone;
        return;
    }
    NNshGlobal->err = errNone;

    // FORMタグの末尾を探す
    NNshGlobal->tempPtr3 = StrStr(NNshGlobal->recvBuf, "</FORM");
    if (NNshGlobal->tempPtr3 == NULL)
    {
        NNshGlobal->tempPtr3 = StrStr(NNshGlobal->recvBuf, "</form");
    }
    if (NNshGlobal->tempPtr3 == NULL)
    {
        NNshGlobal->tempPtr3 =
                            NNshGlobal->recvBuf + StrLen(NNshGlobal->recvBuf);
    }

    // データを初期化する
    MemSet(NNshGlobal->tempBuf, BUFSIZE, 0x00);

    loop = 0;
    ptr  = NNshGlobal->tempPtr2;

    // formタグの内容を解析する
    while ((ptr < NNshGlobal->tempPtr3)&&(*ptr != '>'))
    { 
        // キーワードnameをさがす
        if (((*(ptr + 0) == 'a')||(*(ptr + 0) == 'A'))&&
            ((*(ptr + 1) == 'c')||(*(ptr + 1) == 'C'))&&
            ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
            ((*(ptr + 3) == 'i')||(*(ptr + 3) == 'I'))&&
            ((*(ptr + 4) == 'o')||(*(ptr + 4) == 'O'))&&
            ((*(ptr + 5) == 'n')||(*(ptr + 5) == 'N')))
        {
            // nameデータの先頭までポインタを送る
            ptr = ptr + 6;
            while ((*ptr == '"')||(*ptr == '\'')||(*ptr <= ' ')||
                   (*ptr == '='))
            {
                ptr++;
            }

            // データをtempBufにコピーする
            while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr > ' ')&&
                   (*ptr != '>')&&(loop < (BUFSIZE - 1)))
            {
                NNshGlobal->tempBuf[loop] = *ptr;
                loop++;
                ptr++;
            }
            break;
        }
        ptr++;
    }

    // メソッドはPOSTに限定する(現状のつくりから...必要があればGETも対応...)
    NNshGlobal->sendMethod = 
                           ((NNshGlobal->sendMethod)|(NNSHLOGIN_POST_METHOD));

    // POSTするページ名を決定する
    if (*(NNshGlobal->tempBuf) == '/')
    {
        MemSet (NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
        StrCopy(NNshGlobal->reqAddress, NNshGlobal->tempBuf);
        return;
    }

    // actionにurlすべてが指定されていた場合。。。
    ptr = StrStr(NNshGlobal->tempBuf, "://");
    if (ptr != NULL)
    {
        // ホスト名とポート番号部分は削る...
        ptr = ptr + sizeof("://") - 1;
        while ((*ptr != '/')||(*ptr != '"')||(*ptr != '\0'))
        {
            ptr++;
	}
        if (*(ptr + 1) == '/')
        {
        }
        MemSet (NNshGlobal->reqAddress, (BUFSIZE * 2), 0x00); 
        StrCopy(NNshGlobal->reqAddress, ptr);
        return;
    }

    // ページ名は相対アドレスなので、現在の要求pathを生かすようにする
    NNshGlobal->loopCount = StrLen(NNshGlobal->reqAddress);
    while (NNshGlobal->loopCount != 0)
    {
        (NNshGlobal->loopCount)--;
        if (NNshGlobal->reqAddress[NNshGlobal->loopCount] == '/')
        {
            break;
        }
    }
    NNshGlobal->reqAddress[NNshGlobal->loopCount] = '/';
    (NNshGlobal->loopCount)++;
    NNshGlobal->reqAddress[NNshGlobal->loopCount] = '\0';
    StrCat(NNshGlobal->reqAddress, NNshGlobal->tempBuf);

    return;
}

/**----------------------------------------------------------------------**
 **    フォームタグを解析する
 ** 
 **                NNshGlobal->recvBuf ： 受信ボディ
 **                NNshGlobal->workBuf ： 送信用ボディ(ここで作成する)
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
static void analysis_formTag(NNshWorkingInfo *NNshGlobal)
{
    Char *ptr;

    NNshGlobal->err = errNone;

#if 0
    // Cookieを拾う
    pickup_cookie(NNshParam, NNshGlobal);

    // POST先ページ名を取得する
    pickup_formTag(NNshParam, NNshGlobal);

    // データの解析位置を決定する
    NNshGlobal->tempPtr = NNshGlobal->recvBuf;
#endif

    // 送信用バッファをクリアする
    MemSet (NNshGlobal->workBuf, (WORKBUFFER_DEFAULT), 0x00);

    // フォームからデータを送信するためのタグを抽出する
    ptr = NNshGlobal->tempPtr2;
    while (ptr < NNshGlobal->tempPtr3)
    {
        // 末尾になった場合には、終了する
        if (*ptr == '\0')
        {
            break;
        }

        // textAreaを確保する
        NNshGlobal->tempPtr = StrStr(ptr, "<TEXTAREA");
        if (NNshGlobal->tempPtr == NULL)
        {
            NNshGlobal->tempPtr = StrStr(ptr, "<textarea");
        }
        if (NNshGlobal->tempPtr != NULL)
        {
            // textareaタグを検出
            NNshGlobal->topPtr = NNshGlobal->tempPtr;
            pickup_nameTag (NNshGlobal);

            // value をクリップボードから取得
            NNshGlobal->memH = ClipboardGetItem(clipboardText,
                                                &(NNshGlobal->fieldLen));
            if ((NNshGlobal->memH != 0)&&(NNshGlobal->fieldLen != 0))
            {
                if (NNshGlobal->fieldLen > VALUESIZE)
                {
                    NNshGlobal->fieldLen = VALUESIZE - 1;
                }

                // データが先頭ではなければ、 "&" を付加する
                if (StrLen(NNshGlobal->workBuf) != 0)
                {
                    StrCat(NNshGlobal->workBuf, "&");
                }

                // name & value を 格納する
                StrCat(NNshGlobal->workBuf, NNshGlobal->tempBuf);
                StrCat(NNshGlobal->workBuf, "=");

                // 文字列をURLエンコードしてコピーする
                StrNCopy_URLEncode(
                          &NNshGlobal->workBuf[StrLen(NNshGlobal->workBuf)], 
                          (WORKBUFFER_DEFAULT -  StrLen(NNshGlobal->workBuf)),
                          MemHandleLock(NNshGlobal->memH),
                          NNshGlobal->fieldLen);

                MemHandleUnlock(NNshGlobal->memH);
            }

            // 無理矢理、英語→日本語変換に設定する
            StrCat(NNshGlobal->workBuf, "&wb_lp=ENJA");

            ptr = NNshGlobal->topPtr + sizeof("<textarea");
            continue;
        }

        NNshGlobal->tempPtr = StrStr(ptr, "<INPUT");
        if (NNshGlobal->tempPtr == NULL)
        {
            NNshGlobal->tempPtr = StrStr(ptr, "<input");
        }
        if ((NNshGlobal->tempPtr == NULL)||
            (NNshGlobal->tempPtr >= (NNshGlobal->tempPtr3)))
        {
            // INPUTタグが検出できなかった、終了する
            break;
        }
        NNshGlobal->topPtr = NNshGlobal->tempPtr;

        // INPUTタグの内容を解析する
        pickup_nameTag (NNshGlobal);
        pickup_valueTag(NNshGlobal);

        // key と value を格納する
        set_name_and_value(NNshGlobal);

        // 次のinputフォームへ移動
        ptr = NNshGlobal->topPtr + sizeof("<INPUT");
    }
    return;
}

/**----------------------------------------------------------------------**
 **    NAMEタグを検出できるかチェックする
 **
 **                  NNshGlobal->tempBufに検出結果を格納する
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
static void pickup_nameTag(NNshWorkingInfo *NNshGlobal)
{
    Char  *ptr;
    UInt16 loop;

    // データを初期化する
    loop = 0;
    MemSet(NNshGlobal->tempBuf,  BUFSIZE, 0x00);
    ptr  = NNshGlobal->topPtr;

    // INPUTタグの内容を解析する
    while ((ptr < (NNshGlobal->tempPtr3))&&(*ptr != '>'))
    { 
        // キーワードnameをさがす
        if (((*(ptr + 0) == 'n')||(*(ptr + 0) == 'N'))&&
            ((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&
            ((*(ptr + 2) == 'm')||(*(ptr + 2) == 'M'))&&
            ((*(ptr + 3) == 'e')||(*(ptr + 3) == 'E')))
        {
            // nameデータの先頭までポインタを送る
            ptr = ptr + 5;
            if ((*ptr == '"')||(*ptr == '\'')||(*ptr == ' ')||(*ptr == '='))
            {
                ptr++;
            }
            // データをtempBufにコピーする
            while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != ' ')&&
                   (*ptr != '>')&&(loop < (BUFSIZE - 1)))
            {
                NNshGlobal->tempBuf[loop] = *ptr;
                loop++;
                ptr++;
            }
            return;
        }
        ptr++;
    }
    return;
}

/**----------------------------------------------------------------------**
 **    VALUEタグを検出できるかチェックする
 **
 **                  NNshGlobal->tempBuf2に検出結果を格納する
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
static void pickup_valueTag(NNshWorkingInfo *NNshGlobal)
{
    Char  *ptr;
    UInt16 loop;

    // データを初期化する
    loop = 0;
    MemSet(NNshGlobal->tempBuf2,  BUFSIZE, 0x00);
    ptr  = NNshGlobal->topPtr;

    // INPUTタグの内容を解析する
    while ((ptr < (NNshGlobal->tempPtr3))&&(*ptr != '>'))
    { 
        // キーワードvalueを探す
        if (((*(ptr + 0) == 'v')||(*(ptr + 0) == 'V'))&&
            ((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&
            ((*(ptr + 2) == 'l')||(*(ptr + 2) == 'L'))&&
            ((*(ptr + 3) == 'u')||(*(ptr + 3) == 'U'))&&
            ((*(ptr + 4) == 'e')||(*(ptr + 4) == 'E')))
        {
            // value データの先頭までポインタを送る
            ptr = ptr + 6;
            if ((*ptr == '"')||(*ptr == '\'')||(*ptr == ' ')||(*ptr == '='))
            {
                ptr++;
            }
            // データをtempBuf2にコピーする
            while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != ' ')&&
                   (*ptr != '>')&&(loop < (BUFSIZE - 1)))
            {
                NNshGlobal->tempBuf2[loop] = *ptr;
                loop++;
                ptr++;
            }
            return;
        }
        ptr++;
    }
    return;
}

/**----------------------------------------------------------------------**
 **    name と valueをworkbufに格納する
 **
 **                  ※ NNshGlobal->err に応答コードを格納して終了する ※
 **----------------------------------------------------------------------**/
void set_name_and_value(NNshWorkingInfo *NNshGlobal)
{
    Char *value;

    // データ値の初期値は tempBuf2
    value = NNshGlobal->tempBuf2;

    if ((NNshGlobal->tempBuf[0] == '\0')||(*value == '\0'))
    {
        // name & value 両方とものデータが揃っていなかった...終了する
        return;
    }

    // データが先頭ではなければ、 "&" を付加する
    if (StrLen(NNshGlobal->workBuf) != 0)
    {
        StrCat(NNshGlobal->workBuf, "&");
    }

    // name & value を 格納する
    StrCat(NNshGlobal->workBuf, NNshGlobal->tempBuf);
    StrCat(NNshGlobal->workBuf, "=");
    StrCat(NNshGlobal->workBuf, value);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : ParseTextArea                                              */
/*                                          (翻訳結果のメッセージ整形処理) */
/*                    buf : コピー先領域                                   */
/*                    src : コピー元領域                                   */
/*                   size : コピー元バッファサイズ                         */
/*-------------------------------------------------------------------------*/
static void ParseTextArea(Char *buf, Char *src, UInt32 dstSize, UInt32 size)
{
    Char *srcPtr, *dstPtr, *ptr;

    dstPtr = buf;
    srcPtr = src;

    while ((dstPtr < (buf + dstSize))&&(srcPtr < (src + size)))
    {
        ptr = StrStr(srcPtr, "<textarea");
        if (ptr == NULL)
        {
            ptr = StrStr(srcPtr, "<TEXTAREA");
        }
        if (ptr == NULL)
        {
            // テキスト領域発見できず、解析終了...
            return;
        }
        srcPtr = ptr;
        while ((*srcPtr != '>')&&(*srcPtr != '\0'))
        {
            srcPtr++;
        }
        if (*srcPtr == '\0')
        {
            // 解析終了...
            return;
        }

        // 表示データのコピー(実処理)
        while (!((*srcPtr == '<')&&(*(srcPtr + 1) == '/')))
        {
            if (*srcPtr == '\0')
            {
                break;
            }
            *dstPtr = *srcPtr;
            dstPtr++;
            srcPtr++;
        }

        // データの区切りを表示する
        *dstPtr = '\n';
        dstPtr++;
        *dstPtr = '-';
        dstPtr++;
        *dstPtr = '-';
        dstPtr++;
        *dstPtr = '-';
        dstPtr++;
        *dstPtr = '\n';
        dstPtr++;
    }
    return;
}

/*==========================================================================*/
/*  StrNCopy_URLEncode() : 文字列をURLエンコードコピー                      */
/*                                                                          */
/*==========================================================================*/
static void StrNCopy_URLEncode(UInt8 *dst, UInt32 bufSize, UInt8 *src, UInt32 size)
{
    UInt8  *locP, *data, hexBuf[20];
    UInt32  num;

    data = dst;
    locP = src;
    while ((*locP != '\0')&&(locP < (src + size))&&(data < (dst + bufSize)))
    {
        // スペースは＋に変換
        if (*locP == ' ')
        {
            *data = '+';
            data++;
            locP++;
            continue;
        }

        // 改行は、CR + LFに変換
        if (*locP == '\n')
        {
            StrCopy(data, "%0D%0A");
            data = data + 6; // 6 == StrLen("%0D%0A");
            locP++;
            continue;
        }

        // そのまま出力(その１)
        if ((*locP == '.')||(*locP == '_')||(*locP == '-')||(*locP == '*'))
        {
            *data = *locP;
            data++;
            locP++;
            continue;
        }

        // そのまま出力(その２)
        if (((*locP >= '0')&&(*locP <= '9'))||
            ((*locP >= 'A')&&(*locP <= 'Z'))||
            ((*locP >= 'a')&&(*locP <= 'z')))
        {
            *data = *locP;
            data++;
            locP++;
            continue;
        }

        // 上の条件のどれにも当てはまらない場合には、コンバートする。
        *data = '%';
        data++;
        
        num = (UInt32) *locP;
        MemSet(hexBuf, sizeof(hexBuf), 0x00);
        StrIToH(hexBuf, num);

        // 数値の下２桁が欲しいので...
        StrCat(data, &hexBuf[6]);
        data = data + 2; // 2 == StrLen(data);
        locP++;
    }
}
