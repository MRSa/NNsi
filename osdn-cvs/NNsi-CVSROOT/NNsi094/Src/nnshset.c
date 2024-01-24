/*============================================================================*
 *  FILE: 
 *     nnshset.c
 *
 *  Description: 
 *     NNsh parameter setting functions for NNsh.
 *
 *===========================================================================*/
#define NNSHSET_C
#define GLOBAL_REAL
#include "local.h"

extern Err setOfflineLogDir(Char *fileName);
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

/*-------------------------------------------------------------------------*/
/*   Function :   effect_NNshSetting                                       */
/*                             NNsi設定画面からパラメータ値に反映する処理  */
/*-------------------------------------------------------------------------*/
static Err effect_NNshSetting(FormType *frm)
{
    ControlType *chkObj, *chkObjSub1, *chkObjSub2, *chkObjSub3;

    /***  NNsh設定を(グローバル変数に)反映させる  ***/

    // スレ一覧(subject.txt)全取得
    UpdateParameter(frm, CHKID_SUBJECT_ALL, &(NNshParam->getAllThread));

    // 確認メッセージを省略
    UpdateParameter(frm, CHKID_OMIT_DIALOG, &(NNshParam->confirmationDisable));

    // 取得後にスレ参照
    UpdateParameter(frm, CHKID_OPEN_AUTO,   &(NNshParam->openAutomatic));

    // sage書き込み
    UpdateParameter(frm, CHKID_WRITE_SAGE,  &(NNshParam->writeAlwaysSage));

    // 検索時に、大文字小文字を区別しない
    UpdateParameter(frm, CHKID_SEARCH_CASELESS, &(NNshParam->searchCaseless));

    // 起動時にOfflineスレを検索
    UpdateParameter(frm, CHKID_OFFCHK_LAUNCH,   &(NNshParam->offChkLaunch));

    // アンダーラインOFF
    UpdateParameter(frm, CHKID_MSG_LINEOFF, &(NNshParam->disableUnderline));

    // DEBUG
    UpdateParameter(frm, CHKID_DEBUG_DIALOG,&(NNshParam->debugMessageON));

    // スレ一覧にMSG数
    UpdateParameter(frm, CHKID_PRINT_NOFMSG,&(NNshParam->printNofMessage));

    // VFS 利用可能時に自動的にVFS ON
    UpdateParameter(frm, CHKID_VFSON_AUTOMATIC,&(NNshParam->vfsOnAutomatic));

    // スレ番号を【】にする
    UpdateParameter(frm, CHKID_BOLD_MSGNUM, &(NNshParam->boldMessageNum));

    // VFSの使用と回避フラグ、OFFLINEログ使用フラグの設定
    if (NNshParam->useVFS != NNSH_NOTSUPPORT_VFS)
    {
        chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_USE_VFS));
        chkObjSub1 = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
        chkObjSub2 = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
        chkObjSub3 = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
        NNshParam->useVFS = CtlGetValue(chkObj);
        if (CtlGetValue(chkObjSub1) != 0)
        {
            NNshParam->useVFS = NNshParam->useVFS | NNSH_VFS_WORKAROUND;
        }
        if (CtlGetValue(chkObjSub2) != 0)
        {
            NNshParam->useVFS = NNshParam->useVFS | NNSH_VFS_USEOFFLINE;
        }
        if (CtlGetValue(chkObjSub3) != 0)
        {
            NNshParam->useVFS = NNshParam->useVFS | NNSH_VFS_DBBACKUP;
        }

        if (NNshParam->useVFS != 0)
        {
            //  VFS関連パラメータの設定がされていたとき、次回起動時より
            // 有効となることを表示する。
            NNsh_InformMessage(ALTID_WARN, MSG_NNSISET_VFSWARN, "", 0);
        }

        // logdir.txtを読み、OFFLINEログベースディレクトリを取得
        (void) setOfflineLogDir(LOGDIR_FILE);
    }

    // しおりを使う場合の設定
    if (NNshParam->useBookmark == 0)
    {
        // しおりを使わない場合、しおり設定のクリア
        NNshParam->lastFrmID     = FRMID_THREAD;
        NNshParam->bookMsgNumber = 0;
        NNshParam->bookMsgIndex  = 0;
    }
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting2                                       */
/*                                             NNsi設定2の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting2(FormType *frm)
{
    FieldType   *fldP;
    UInt32       bufSize;
    UInt16       timeout;
    Char        *logBuf, *numP;
    MemHandle    txtH;

    // 通信タイムアウト値の変更
    timeout = SysTicksPerSecond();
    timeout = (timeout == 0) ? 1 : timeout;  // ゼロ除算の回避(保険)
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_COMM_TIMEOUT));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);
        if (NNshParam->netTimeout != timeout * bufSize)
        {
            NNshParam->netTimeout = timeout * bufSize;

            // TIMEOUTの更新情報を表示
            NNsh_DebugMessage(ALTID_INFO, MSG_TIMEOUT_UPDATED, 
                              "", NNshParam->netTimeout);
        }
    }

    // 内部バッファサイズの変更
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_BUFFER_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);
        if ((bufSize < NNSH_WORKBUF_MIN)||(bufSize > NNSH_WORKBUF_MAX))
        {
            // データ値異常、入力範囲異常を表示して元に戻る
            logBuf = MemPtrNew(BUFSIZE);
            if (logBuf != NULL)
            {
                MemSet (logBuf, BUFSIZE, 0x00);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE1);
                NUMCATI(logBuf, bufSize);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE2);
                NUMCATI(logBuf, NNSH_WORKBUF_MIN);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE3);
                NUMCATI(logBuf, NNSH_WORKBUF_MAX);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE4);
                NUMCATI(logBuf, NNSH_WORKBUF_DEFAULT);
                NNsh_ErrorMessage(ALTID_ERROR, logBuf, "", 0);
                MemPtrFree(logBuf);
            }
            return (~errNone);
        }
        else
        {
            if (bufSize == NNshParam->bufferSize)
            {
                // バッファサイズは変更しなかった旨報告し、終了する
                NNsh_DebugMessage(ALTID_INFO, MSG_WARN_NOTMODIFIED,
                                  "buffer size:", NNshParam->bufferSize);
            }
            else
            {
                // バッファサイズを変更して終了する
                NNshParam->bufferSize = bufSize;
                NNsh_InformMessage(ALTID_INFO, MSG_INFO_MODIFIED,
                                   "buffer size:", NNshParam->bufferSize);
            }
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "0", 0);
    }

    // 分割ダウンロード指定を取得する
    UpdateParameter(frm, CHKID_DL_PART, &(NNshParam->enablePartGet));

    // 分割ダウンロードのサイズを取得
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_DL_PARTSIZE));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);

        if (bufSize < NNSH_DOWNLOADSIZE_MIN)
        {
            // ダウンロードサイズは、バッファサイズ下限よりも小さい
            NNsh_ErrorMessage(ALTID_WARN, MSG_DOWNLOADSIZE_UNDER,
                              "", NNSH_DOWNLOADSIZE_MIN);
            return (~errNone);
        }
        if (bufSize > NNshParam->bufferSize)
        {
            // ダウンロードサイズは、ワークバッファサイズよりも小さい
            NNsh_ErrorMessage(ALTID_WARN, MSG_DOWNLOADSIZE_OVER, "", 0);
            return (~errNone);
        }
        if (bufSize != NNshParam->partGetSize)
        {
            NNshParam->partGetSize = bufSize;

            // ダウンロードサイズの更新を表示
            NNsh_DebugMessage(ALTID_INFO, MSG_DOWNLOADSIZE_UPDATE, "",
                              NNshParam->partGetSize);
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "1", 0);
    }

    // コテハン機能を使用する設定
    UpdateParameter(frm, CHKID_FIXED_HANDLE, &(NNshParam->useFixedHandle));

    // ハンドル名の設定(ハンドル名が設定できなくても正常応答する)
    MemSet(NNshParam->handleName, BUFSIZE, 0x00);
    fldP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, FLDID_HANDLENAME));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        MemSet(NNshParam->handleName, BUFSIZE, 0x00);
        if (*numP != '\0')
        {
            StrNCopy(NNshParam->handleName, numP, (BUFSIZE - 1));
        }
        MemHandleUnlock(txtH);
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "3", 0);
    }

    // BBS一覧取得先の設定
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_URL_BBS));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // フィールドからURLを取得する
        numP = MemHandleLock(txtH);
        MemSet  (NNshParam->bbs_URL, MAX_URL, 0x00);
        StrNCopy(NNshParam->bbs_URL, numP, (MAX_URL - 1));
        MemHandleUnlock(txtH);
    }

    // BBS一覧取得先の設定
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_TITLE_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // フィールドからスレ文字列数を取得する
        numP = MemHandleLock(txtH);
        if (numP != NULL)
        {
            bufSize = StrAToI(numP);
            MemHandleUnlock(txtH);
        }
        else
        {
            bufSize = LIMIT_TITLENAME_DEFAULT;
        }
        if ((bufSize > MAX_THREADNAME)||(bufSize < LIMIT_TITLENAME_MINIMUM))
        {
            // データ値異常、入力範囲異常を表示して元に戻る
            logBuf = MemPtrNew(BUFSIZE);
            if (logBuf != NULL)
            {
                MemSet (logBuf, BUFSIZE, 0x00);
                StrCat (logBuf, MSG_THREADNAME_ILLEGAL);
                NUMCATI(logBuf, bufSize);
                StrCat (logBuf, MSG_THREADNAME_ILLEGAL2);
                NUMCATI(logBuf, LIMIT_TITLENAME_MINIMUM);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE3);
                NUMCATI(logBuf, MAX_THREADNAME);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE4);
                NUMCATI(logBuf, LIMIT_TITLENAME_DEFAULT);
                NNsh_ErrorMessage(ALTID_ERROR, logBuf, "", 0);
                MemPtrFree(logBuf);
            }
            return (~errNone);
        }
        else
        {
            // サイズを確認する
            if (bufSize == NNshParam->titleListSize)
            {
                // サイズは変更しなかった旨報告し、終了する
                NNsh_DebugMessage(ALTID_INFO, MSG_WARN_NOTMODIFIED,
                                  "title length:", NNshParam->titleListSize);
            }
            else
            {
                NNshParam->titleListSize = bufSize;
                NNsh_InformMessage(ALTID_INFO, MSG_INFO_MODIFIED,
                                   "title length:", NNshParam->titleListSize);
            }
        }
    }

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting3                                       */
/*                                             NNsi設定3の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting3(FormType *frm)
{
    FieldType   *fldP;
    UInt32       portNum;
    Char        *proxyP;
    MemHandle    txtH;

    // NNsi終了時に回線を切断する
    UpdateParameter(frm, CHKID_END_OFFLINE, &(NNshParam->disconnectNNsiEnd));

    // 「新着確認」終了時に回線を切断する
    UpdateParameter(frm, CHKID_DISCONN_ARRIVAL, &(NNshParam->disconnArrivalEnd));

    // 「新着確認」終了後に未読一覧を表示する
    UpdateParameter(frm, CHKID_ARRIVAL_NOTREAD, &(NNshParam->autoOpenNotRead));

    // 受信Cookieを使用して書き込み
    UpdateParameter(frm, CHKID_WRITE_USE_COOKIE, &(NNshParam->useCookieWrite));

    // 回線接続後画面を再描画
    UpdateParameter(frm,CHKID_REDRAW_CONNECT,&(NNshParam->redrawAfterConnect));

    // 書きこみ時、書きこみフィールドに自動的にフォーカスを移動しない
    UpdateParameter(frm, CHKID_NOT_AUTOFOCUSSET,&(NNshParam->notAutoFocus));

    // 参照専用へコピーしたとき、同時にスレ削除を実施
    UpdateParameter(frm, CHKID_COPYDEL_READONLY,&(NNshParam->copyDelReadOnly));

    // 取得済み全ては参照専用ログに表示しない
    UpdateParameter(frm, CHKID_NOT_READONLY, &(NNshParam->notListReadOnly));

    // 書き込み時参照スレ番号を挿入する
    UpdateParameter(frm, CHKID_INSERT_REPLYNUM, &(NNshParam->insertReplyNum));

    // Proxy経由でのアクセス
    UpdateParameter(frm, CHKID_USE_PROXY, &(NNshParam->useProxy));

    // Proxy URL
    fldP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, FLDID_USE_PROXY));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        proxyP    = MemHandleLock(txtH);
        MemSet(NNshParam->proxyURL, MAX_URL, 0x00);
        if (*proxyP != '\0')
        {
            StrNCopy(NNshParam->proxyURL, proxyP, (MAX_URL - 1));
        }
        MemHandleUnlock(txtH);
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "PROXY URL", 0);
    }

    // Proxy Port #
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_PROXY_PORT));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        proxyP  = MemHandleLock(txtH);
        portNum = StrAToI(proxyP);
        MemHandleUnlock(txtH);

        NNshParam->proxyPort = portNum;
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "PROXY PORT", 0);
    }

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting4                                       */
/*                                             NNsi設定4の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting4(FormType *frm)
{
    UInt16 butID;

    // Bookmark(しおり設定)
    UpdateParameter(frm, CHKID_USE_BOOKMARK,&(NNshParam->useBookmark));

    // PUSH ONダイヤルで±１
    UpdateParameter(frm, CHKID_NOT_CURSOR, &(NNshParam->notCursor));

    // 参照時BackボタンでJumpList
    UpdateParameter(frm, CHKID_BACKBTN_JUMP, &(NNshParam->jogBackBtnAsGo));

    // 参照時JumpListに「一覧へ」
    UpdateParameter(frm, CHKID_ADD_RTNTOLIST, &(NNshParam->addReturnToList));

    // PalmOS5用機能を使用する
    UpdateParameter(frm, CHKID_USE_ARMLET, &(NNshParam->useARMlet));

    // WebBrowserを開くとき最新50
    UpdateParameter(frm, CHKID_WEBBROWSE_LAST50, &butID);
    if (butID != 0)
    {
        NNshParam->browseMesNum = 50;
    }
    else
    {
        NNshParam->browseMesNum = 0;
    }

    // スレ参照画面でtiny/smallフォントを使用する
    UpdateParameter(frm,CHKID_CLIE_USE_TINYFONT,
                                      &(NNshParam->useSonyTinyFont));

    // この機能はOS5専用の機能なため、本当に実施するか確認を表示する
    if (NNshParam->useARMlet != 0)
    {
        butID = NNsh_ErrorMessage(ALTID_CONFIRM, MSG_WARNING_OS5, "", 0);
        if (butID != 0)
        {
            NNshParam->useARMlet = 0;
        }
    }

    // 一覧時Backボタンでメニュー表示
    UpdateParameter(frm, CHKID_BACKBTN_LIST, &NNshParam->jogBackBtnAsMenu);

#ifdef USE_CLIE
    // Silk制御を行わない
    UpdateParameter(frm, CHKID_NOTUSE_SILK, &NNshGlobal->notUseSilk);

    // SILK制御は行わないときは、警告を表示する
    if (NNshGlobal->notUseSilk != NNshParam->notUseSilk)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_SILK_NEXT_LAUNCH, "", 0);
    }
#endif

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*=========================================================================*/
/*   Function :   Hander_NNshSetting                                       */
/*                                     NNsi設定系の(総合)イベントハンドラ  */
/*=========================================================================*/
Boolean Handler_NNshSetting(EventType *event)
{
    FormType    *frm;
    ControlType *chkObj, *chkObjSub1, *chkObjSub2, *chkObjSub3;
    UInt16       formID;
    Err          ret;
    void        *effectFunc;

    // ボタンチェック以外のイベントは即戻り
    switch (event->eType)
    { 
      case menuEvent:
        // メニュー選択
        return (NNsh_MenuEvt_Edit(event));
        break;

      case ctlSelectEvent:
        // ボタンが押された(次へ進む)
        break;

      default: 
        return (false);
        break;
    }

    effectFunc = NULL;
    formID     = FRMID_THREAD;
    frm        = FrmGetActiveForm();
    switch (event->data.ctlSelect.controlID)
    {
      // "OK"ボタンがおされたとき(NNsi設定)
      case BTNID_CONFIG_NNSH_OK:
        formID     = FRMID_THREAD;
        effectFunc = &effect_NNshSetting;
        break;
      // "2"ボタンが押されたとき
      case NNSHSET_2:
        formID     = FRMID_NNSI_SETTING2; 
        effectFunc = &effect_NNshSetting;
        break;
      // "3"ボタンが押されたとき
      case NNSHSET_3:
        formID     = FRMID_NNSI_SETTING3; 
        effectFunc = &effect_NNshSetting;
        break;
      // "4"ボタンが押されたとき
      case NNSHSET_4:
        formID     = FRMID_NNSI_SETTING4; 
        effectFunc = &effect_NNshSetting;
        break;

      // "OK"ボタンがおされたとき(NNsi設定-2)
      case BTNID_NNSI_SET_OK:
        formID     = FRMID_THREAD;
        effectFunc = &effectNNsiSetting2;
        break;
      // "1"ボタンが押されたとき
      case NNSHSET2_1:
        formID     = FRMID_CONFIG_NNSH;
        effectFunc = &effectNNsiSetting2;
        break;
      // "3"ボタンが押されたとき
      case NNSHSET2_3:
        formID     = FRMID_NNSI_SETTING3; 
        effectFunc = &effectNNsiSetting2;
        break;
      // "4"ボタンが押されたとき
      case NNSHSET2_4:
        formID     = FRMID_NNSI_SETTING4; 
        effectFunc = &effectNNsiSetting2;
        break;

      // "OK"ボタンがおされたとき(NNsi設定-3)
      case BTNID_NNSISET3_OK:
        formID     = FRMID_THREAD;
        effectFunc = &effectNNsiSetting3;
        break;
      // "1"ボタンが押されたとき
      case NNSHSET3_1:
        formID     = FRMID_CONFIG_NNSH;
        effectFunc = &effectNNsiSetting3;
        break;
      // "2"ボタンが押されたとき
      case NNSHSET3_2:
        formID     = FRMID_NNSI_SETTING2; 
        effectFunc = &effectNNsiSetting3;
        break;
      // "4"ボタンが押されたとき
      case NNSHSET3_4:
        formID     = FRMID_NNSI_SETTING4;
        effectFunc = &effectNNsiSetting3;
        break;

      // "OK"ボタンがおされたとき(NNsi設定-4)
      case BTNID_NNSISET4_OK:
        formID     = FRMID_THREAD;
        effectFunc = &effectNNsiSetting4;
        break;
      // "1"ボタンが押されたとき
      case NNSHSET4_1:
        formID     = FRMID_CONFIG_NNSH;
        effectFunc = &effectNNsiSetting4;
        break;
      // "2"ボタンが押されたとき
      case NNSHSET4_2:
        formID     = FRMID_NNSI_SETTING2; 
        effectFunc = &effectNNsiSetting4;
        break;
      // "3"ボタンが押されたとき
      case NNSHSET4_3:
        formID     = FRMID_NNSI_SETTING3;
        effectFunc = &effectNNsiSetting4;
        break;

      // "Cancel"ボタンがおされたとき
      case BTNID_CONFIG_NNSH_CANCEL:
      case BTNID_NNSI_SET_CANCEL:
      case BTNID_NNSISET3_CANCEL:
      case BTNID_NNSISET4_CANCEL:
        effectFunc = NULL;
        break;

      case CHKID_USE_VFS:
        // VFS切り替え設定を反映させる
        if (NNshParam->useVFS != NNSH_NOTSUPPORT_VFS)
        {
            // VFS の ON/OFFが切り替わったら、画面上のVFSアイテムを更新する。
            chkObj   = FrmGetObjectPtr(frm,
                                       FrmGetObjectIndex(frm, CHKID_USE_VFS));
            chkObjSub1 = FrmGetObjectPtr(frm, 
                                 FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
            chkObjSub2 = FrmGetObjectPtr(frm, 
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
            chkObjSub3 = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
            if (CtlGetValue(chkObj) == 0)
            {
                CtlSetValue  (chkObjSub1, 0);
                CtlSetEnabled(chkObjSub1, false);
                CtlSetUsable (chkObjSub1, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));

                CtlSetValue  (chkObjSub2, 0);
                CtlSetEnabled(chkObjSub2, false);
                CtlSetUsable (chkObjSub2, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));

                CtlSetValue  (chkObjSub3, 0);
                CtlSetEnabled(chkObjSub3, false);
                CtlSetUsable (chkObjSub3, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));

                FrmEraseForm (frm);
            }
            else
            {
                CtlSetEnabled(chkObjSub1, true);
                CtlSetUsable (chkObjSub1, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));

                CtlSetEnabled(chkObjSub2, true);
                CtlSetUsable (chkObjSub2, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));

                CtlSetEnabled(chkObjSub3, true);
                CtlSetUsable (chkObjSub3, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
            }
            FrmDrawForm(frm);
        }
        return (false);
        break;

      case CHKID_USE_BBS_NEWURL:
        // 標準URLチェックボックスが押された時、BBS一覧取得先を標準URLに変更
        NNshWinSetFieldText(frm, FLDID_URL_BBS, URL_BBSTABLE,
                                                        StrLen(URL_BBSTABLE));
        NNsh_InformMessage(ALTID_INFO, MSG_CHANGED_URL_NEW, "", 0);
        chkObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_BBS_NEWURL));
        CtlSetValue(chkObj, 0);
        return (false);
        break;

      case CHKID_USE_BBS_OLDURL:
        // 旧URLチェックボックスが押されたとき、BBS一覧取得先を旧URLに変更する
        NNshWinSetFieldText(frm, FLDID_URL_BBS, URL_BBSTABLE_OLD,
                                                    StrLen(URL_BBSTABLE_OLD));
        NNsh_InformMessage(ALTID_INFO, MSG_CHANGED_URL_OLD, "", 0);
        chkObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_BBS_OLDURL));
        CtlSetValue(chkObj, 0);
        return (false);
        break;

      default:
        // 上記以外(何もしない)
        return (false);
        break;
    }

    // 設定されたデータを反映させる処理
    if (effectFunc != NULL)
    {
        ret =  ((Err (*)())effectFunc)(frm);
        if (ret != errNone)
        {
            // 設定エラー(画面はそのまま)
            return (false);
        }
    }

    // 画面遷移する
    FrmEraseForm(frm);
    FrmGotoForm(formID);
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNshSetting                                     */
/*                                                NNsi設定のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNshSetting(FormType *frm)
{
    ControlType *chkObj, *chkObjSub1, *chkObjSub2, *chkObjSub3;

    /***  NNsh設定を(グローバル変数から)反映させる  ***/

    // スレ一覧(subject.txt)全取得
    SetControlValue(frm, CHKID_SUBJECT_ALL, &(NNshParam->getAllThread));

    // 確認メッセージを省略
    SetControlValue(frm, CHKID_OMIT_DIALOG, &(NNshParam->confirmationDisable));

    // 取得後にスレ参照
    SetControlValue(frm, CHKID_OPEN_AUTO,   &(NNshParam->openAutomatic));

    // sage書き込み
    SetControlValue(frm, CHKID_WRITE_SAGE,  &(NNshParam->writeAlwaysSage));

    // 検索時に、大文字小文字を区別しない
    SetControlValue(frm, CHKID_SEARCH_CASELESS, &(NNshParam->searchCaseless));

    // 起動時にOfflineスレを検索
    SetControlValue(frm, CHKID_OFFCHK_LAUNCH,   &(NNshParam->offChkLaunch));

    // VFS 利用可能時に自動的にVFS ON
    SetControlValue(frm, CHKID_VFSON_AUTOMATIC,&(NNshParam->vfsOnAutomatic));

    // アンダーラインOFF
    SetControlValue(frm, CHKID_MSG_LINEOFF, &(NNshParam->disableUnderline));

    // DEBUG
    SetControlValue(frm, CHKID_DEBUG_DIALOG,&(NNshParam->debugMessageON));

    // スレ一覧にMSG数
    SetControlValue(frm, CHKID_PRINT_NOFMSG,&(NNshParam->printNofMessage));

    // スレ番号を【】にする
    SetControlValue(frm, CHKID_BOLD_MSGNUM, &(NNshParam->boldMessageNum));

    // 「VFSの使用」は１つのパラメータで複数の意味合いを共有しているため
    chkObj     = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_USE_VFS));
    chkObjSub1 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
    chkObjSub2 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
    chkObjSub3 = 
                 FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
    switch (NNshParam->useVFS)
    {
      // VFS非サポート機の場合、「VFSの使用」設定は表示しない。
      case NNSH_NOTSUPPORT_VFS:
        CtlSetUsable(chkObj,     false);
        CtlSetUsable(chkObjSub1, false);
        CtlSetUsable(chkObjSub2, false);
        CtlSetUsable(chkObjSub3, false);
        break;

      // VFSのチェック
      case NNSH_VFS_DISABLE:
        CtlSetValue  (chkObj,     NNSH_VFS_DISABLE);
        CtlSetValue  (chkObjSub1, 0);
        CtlSetUsable (chkObjSub1, false);

        CtlSetValue  (chkObjSub2, 0);
        CtlSetUsable (chkObjSub2, false);

        CtlSetValue  (chkObjSub3, 0);
        CtlSetUsable (chkObjSub3, false);
        break;

      case NNSH_VFS_ENABLE:
      default:
        CtlSetValue  (chkObj,     NNSH_VFS_ENABLE);
        CtlSetUsable (chkObjSub1, true);
        CtlSetValue  (chkObjSub1, (NNSH_VFS_WORKAROUND & NNshParam->useVFS));

        CtlSetUsable (chkObjSub2, true);
        CtlSetValue  (chkObjSub2, (NNSH_VFS_USEOFFLINE & NNshParam->useVFS));

        CtlSetUsable (chkObjSub3, true);
        CtlSetValue  (chkObjSub3, (NNSH_VFS_DBBACKUP & NNshParam->useVFS));
        break;
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting2                                    */
/*                                               NNsi設定2のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting2(FormType  *frm)
{
    UInt16       fieldIndex, timeout;
    Char         bufSize[MINIBUF];

    // フィールドに現在のタイムアウト値を反映させる
    MemSet(bufSize, sizeof(bufSize), 0x00);
    timeout = SysTicksPerSecond();
    timeout = (timeout == 0) ? 1 : timeout;  // ゼロ除算の回避(保険)
    StrIToA(bufSize, (NNshParam->netTimeout / timeout));
    NNshWinSetFieldText(frm, FLDID_COMM_TIMEOUT, bufSize, StrLen(bufSize));

    // フィールドに現在のバッファサイズを反映させる
    MemSet(bufSize, sizeof(bufSize), 0x00);
    StrIToA(bufSize, NNshParam->bufferSize);
    NNshWinSetFieldText(frm, FLDID_BUFFER_SIZE, bufSize, StrLen(bufSize));

    // フィールドに現在の分割取得サイズを反映させる
    MemSet(bufSize, sizeof(bufSize), 0x00);
    StrIToA(bufSize, NNshParam->partGetSize);
    NNshWinSetFieldText(frm, FLDID_DL_PARTSIZE, bufSize, StrLen(bufSize));

    // フィールドに現在のスレタイトル文字数を反映させる
    MemSet(bufSize, sizeof(bufSize), 0x00);
    StrIToA(bufSize, NNshParam->titleListSize);
    NNshWinSetFieldText(frm, FLDID_TITLE_SIZE, bufSize, StrLen(bufSize));

    // 分割ダウンロード指定を画面に反映する
    SetControlValue(frm, CHKID_DL_PART, &(NNshParam->enablePartGet));

    // BBS一覧取得先URLの設定
    NNshWinSetFieldText(frm, FLDID_URL_BBS, NNshParam->bbs_URL, MAX_URL);

    // コテハン機能を使用する設定を画面に反映する
    SetControlValue(frm, CHKID_FIXED_HANDLE, &(NNshParam->useFixedHandle));

    // ハンドル名の設定
    if (NNshParam->handleName[0] != '\0')
    { 
        NNshWinSetFieldText(frm, FLDID_HANDLENAME,
                            NNshParam->handleName, BUFSIZE);
    }

    // フォーカスをバッファサイズ入力へ移動
    fieldIndex = FrmGetObjectIndex(frm, FLDID_BUFFER_SIZE);
    FrmSetFocus(frm, fieldIndex);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting3                                    */
/*                                               NNsi設定3のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting3(FormType  *frm)
{
    Char         bufSize[MINIBUF];

    // NNsi終了時に回線を切断する
    SetControlValue(frm, CHKID_END_OFFLINE, &(NNshParam->disconnectNNsiEnd));

    // 「新着確認」終了時に回線を切断する
    SetControlValue(frm, CHKID_DISCONN_ARRIVAL, &(NNshParam->disconnArrivalEnd));

    // 「新着確認」終了後に未読一覧を表示する
    SetControlValue(frm, CHKID_ARRIVAL_NOTREAD, &(NNshParam->autoOpenNotRead));

    // 受信Cookieを使用して書き込み
    SetControlValue(frm, CHKID_WRITE_USE_COOKIE, &(NNshParam->useCookieWrite));

    // 回線接続後画面を再描画
    SetControlValue(frm,CHKID_REDRAW_CONNECT,&(NNshParam->redrawAfterConnect));

    // 書きこみ時、書きこみフィールドに自動的にフォーカスを移動しない
    SetControlValue(frm, CHKID_NOT_AUTOFOCUSSET,&(NNshParam->notAutoFocus));

    // 参照専用へコピーしたとき、同時にスレ削除を実施
    SetControlValue(frm, CHKID_COPYDEL_READONLY,&(NNshParam->copyDelReadOnly));

    // 取得済み全ては参照専用ログに表示しない
    SetControlValue(frm, CHKID_NOT_READONLY, &(NNshParam->notListReadOnly));

    // 書き込み時参照スレ番号を挿入する
    SetControlValue(frm, CHKID_INSERT_REPLYNUM, &(NNshParam->insertReplyNum));

    // Proxy経由でのアクセス
    SetControlValue(frm, CHKID_USE_PROXY, &(NNshParam->useProxy));

    // Proxy URL
    if (NNshParam->proxyURL[0] != '\0')
    { 
        NNshWinSetFieldText(frm, FLDID_USE_PROXY,
                            NNshParam->proxyURL, MAX_URL);
    }

    // Proxy Port #
    if (NNshParam->proxyPort != 0)
    {
        MemSet (bufSize, sizeof(bufSize), 0x00);
        StrIToA(bufSize, NNshParam->proxyPort);
        NNshWinSetFieldText(frm, FLDID_PROXY_PORT, bufSize, StrLen(bufSize));
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting4                                    */
/*                                               NNsi設定4のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting4(FormType  *frm)
{
    ControlType *chkObjSub1;
    UInt16       dum;
#ifdef USE_ARMLET
    UInt32       processorType;

    // PalmOS5用機能を使用する
    FtrGet(sysFileCSystem, sysFtrNumProcessorID, &processorType);
    if (sysFtrNumProcessorIsARM(processorType))
    {
        // ARMletの使用
        SetControlValue(frm, CHKID_USE_ARMLET, &(NNshParam->useARMlet));
    }
    else
#endif
    {
        // ARMletの使用について、設定項目自体を画面に表示しない
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_USE_ARMLET));
        CtlSetUsable(chkObjSub1, false);

        NNshParam->useARMlet           = 0;
    }

    // グラフィック描画モードを使用する
    SetControlValue(frm, CHKID_CLIE_USE_TINYFONT,
                                               &(NNshParam->useSonyTinyFont));

    // bookmark
    SetControlValue(frm, CHKID_USE_BOOKMARK,&(NNshParam->useBookmark));

    // PUSH ONダイヤルで±１
    SetControlValue(frm, CHKID_NOT_CURSOR, &(NNshParam->notCursor));

    // 参照時BackボタンでJumpList
    SetControlValue(frm, CHKID_BACKBTN_JUMP, &(NNshParam->jogBackBtnAsGo));

    // 参照時JumpListに「一覧へ」
    SetControlValue(frm, CHKID_ADD_RTNTOLIST, &(NNshParam->addReturnToList));

    // 一覧時Backボタンでメニュー表示
    SetControlValue(frm, CHKID_BACKBTN_LIST, &NNshParam->jogBackBtnAsMenu);

    // WebBrowserを開くとき最新50
    if (NNshGlobal->browserCreator != 0)
    {
        if (NNshParam->browseMesNum != 0)
        {
            dum = 1;
            SetControlValue(frm, CHKID_WEBBROWSE_LAST50, &dum);
        }
    }
    else
    {
        // (設定項目自体を画面に表示しない)
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_WEBBROWSE_LAST50));
        CtlSetUsable(chkObjSub1, false);
    }

#ifdef USE_CLIE
    // Silk制御を行わないの表示可否チェック
    if (NNshGlobal->silkVer != 0)
    {
        if (NNshGlobal->notUseSilk != 0)
        {
            dum = 1;
            SetControlValue(frm, CHKID_NOTUSE_SILK, &dum);
        }
    }
    else
#endif
    {
        // (設定項目自体を画面に表示しない)
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_NOTUSE_SILK));
        CtlSetUsable(chkObjSub1, false);
    }
    return (errNone);
}
