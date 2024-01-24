/*============================================================================*
 *  $Id: damain.c,v 1.5 2004/05/09 05:34:35 mrsa Exp $
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
    UInt8            *msgBuf;
    UInt16            size;
    NNshSavedPref    *NNshParam;
    NNshWorkingInfo  *NNshGlobal;

    //////////////////////////////////////////////////////////////////////////
    /////     起動処理（設定の保存等）
    //////////////////////////////////////////////////////////////////////////
    msgBuf = MemPtrNew(DIALOG_BUFFER);
    if (msgBuf == NULL)
    {
        // 領域確保に失敗！(起動しない)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " dlgBuf");
        return;
    }
    MemSet(msgBuf, DIALOG_BUFFER, 0x00);

    // NNsi設定用格納領域の確保
    size      = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    NNshParam = (NNshSavedPref *) MemPtrNew(size);
    if (NNshParam == NULL)
    {
        // 領域確保に失敗！(起動しない)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " param");
        MEMFREE_PTR(msgBuf);
        return;
    }        
    MemSet(NNshParam, size, 0x00);

    // NNsiグローバル領域用格納領域の確保
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // 領域確保に失敗！(起動しない)
        FrmCustomAlert(ALTID_INFO, "Cannot Launch ",  "(DA)", " global");
        MEMFREE_PTR(msgBuf);
        MEMFREE_PTR(NNshParam);
        return;
    }        
    MemSet(NNshGlobal, size, 0x00);

#ifdef USE_OSVERSION
    // OSバージョンの確認
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &(NNshGlobal->palmOSVersion));
#endif

    // NNsi設定を復旧させる
    RestoreSetting_NNsh(sizeof(NNshSavedPref), NNshParam);

    //////////////////////////////////////////////////////////////////////////
    /////     画面オープン処理（ＤＡ処理のメイン）
    //////////////////////////////////////////////////////////////////////////

    //   Auto Exec.(自動実行)にチェックが入っていない場合には、設定ウィンドウを
    // 表示する
    if (NNshParam->confirmationDisable == 0)
    {
        if (FrmGetActiveFormID() != 0)
        {
            NNshGlobal->previousForm = FrmGetActiveForm();
        }
        else
        {
            NNshGlobal->previousForm = NULL;
        }
        NNshGlobal->currentForm  = FrmInitForm(FRMID_MAIN);
        // FrmSetActiveForm(NNshGlobal->currentForm);

        if (StrLen(NNshParam->password) == 0)
        {
            CtlSetLabel(FrmGetObjectPtr(NNshGlobal->currentForm, 
                                   FrmGetObjectIndex(NNshGlobal->currentForm, 
                                                           SELTRID_PASSWORD)), 
                        "(Empty)");
        }
        else
        {
            CtlSetLabel(FrmGetObjectPtr(NNshGlobal->currentForm, 
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           SELTRID_PASSWORD)),
                        "* * *");
        }
        NNshWinSetPopItems (NNshGlobal->currentForm,
                            POPTRID_PROTOCOL, LSTID_PROTOCOL,
                            NNshParam->protocol);
        NNshWinSetFieldText(NNshGlobal->currentForm,
                            FLDID_HOST,     NNshParam->hostName, BUFSIZE);
        NNshWinSetFieldText(NNshGlobal->currentForm,
                            FLDID_USER,     NNshParam->userName, BUFSIZE);
        NNshWinSetFieldText(NNshGlobal->currentForm,
                            FLDID_PASSWORD, NNshParam->password, BUFSIZE);

        // 回線切断の設定
        CtlSetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                  FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           CHKID_DISCONNECT)),
                    (UInt16) (NNshParam->disconnect));

        // メッセージヘッダの取得
        CtlSetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                  FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           CHKID_GETHEADER)),
                    (UInt16) (NNshParam->getMessageHeader));

        // イベントハンドラの設定
        FrmSetEventHandler(NNshGlobal->currentForm, Handler_MainForm);
        NNshGlobal->btnId = FrmDoDialog(NNshGlobal->currentForm);
        if (NNshGlobal->btnId == BTNID_DIALOG_OK)
        {
            // 設定値の反映
            NNshWinGetFieldText(NNshGlobal->currentForm, FLDID_HOST,
                                NNshParam->hostName, BUFSIZE);
            NNshWinGetFieldText(NNshGlobal->currentForm, FLDID_USER,
                                NNshParam->userName, BUFSIZE);
            NNshWinGetFieldText(NNshGlobal->currentForm, FLDID_PASSWORD,
                                NNshParam->password, BUFSIZE);

            // メッセージヘッダの取得
            NNshParam->getMessageHeader = (UInt8)
                 CtlGetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                             CHKID_GETHEADER)));

            // 自動実行のチェック
            NNshParam->confirmationDisable = (UInt8)
                 CtlGetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                              CHKID_CONFIRM)));

            // 回線切断
            NNshParam->disconnect =
                 CtlGetValue(FrmGetObjectPtr(NNshGlobal->currentForm,
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                           CHKID_DISCONNECT)));

            // 通信プロトコルの反映
            NNshParam->protocol = 
                 LstGetSelection(FrmGetObjectPtr(NNshGlobal->currentForm,
                                    FrmGetObjectIndex(NNshGlobal->currentForm,
                                                             LSTID_PROTOCOL)));
        }
        // フォームの削除
        if (NNshGlobal->previousForm != NULL)
        {
            FrmSetActiveForm(NNshGlobal->previousForm);
        }
        FrmDeleteForm(NNshGlobal->currentForm);
    }
    else
    {
        // 自動実行時には、メールチェックを実行する
        NNshGlobal->btnId = BTNID_DIALOG_OK;
    }

    //////////////////////////////////////////////////////////////////////////
    // メールチェックの実行実処理
    //////////////////////////////////////////////////////////////////////////
    if (NNshGlobal->btnId == BTNID_DIALOG_OK)
    {
        // 送受信バッファの確保
        NNshGlobal->sendBuf = MemPtrNew(NNshParam->bufferSize);
        if (NNshGlobal->sendBuf == NULL)
        {
            // 領域確保失敗、終了処理へ
            goto FUNC_END;
        }
        MemSet(NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        NNshGlobal->recvBuf = MemPtrNew(NNshParam->bufferSize);
        if (NNshGlobal->recvBuf == NULL)
        {
            // 領域確保失敗、終了処理へ
            MEMFREE_PTR(NNshGlobal->sendBuf);
            goto FUNC_END;
        }
        MemSet(NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);

        // BUSYウィンドウを表示する
        Show_BusyForm(NNshParam->hostName, NNshGlobal);    

        MemSet (NNshGlobal->tempBuf, MINIBUF, 0x00);
        switch (NNshParam->protocol)
        {
          case CHECKPROTOCOL_POP3:
          case CHECKPROTOCOL_APOP:
            // POP3/APOPのとき
            size = checkMail_POP3(NNshParam->protocol, NNshParam, NNshGlobal,
                                                  NNshGlobal->tempBuf, msgBuf);
            break;

          default:
            // 指定値異常、何もしない
            size = ~errNone;
            break;
        }

        // 回線切断(指定時)
        if (NNshParam->disconnect != 0)
        {
            // BUSYウィンドウの表示変更
            SetMsg_BusyForm(MSG_DISCONNECT);
            NNshNet_LineHangup();
        }

        // バッファクリア
        MemSet(NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);

        // メール受信処理の解析
        if (size == ~errNone)
        {
            // エラー発生
            StrCopy(NNshGlobal->sendBuf, MSG_ERROR);
        }
        else if (size == 0)
        {
            // 新着メールがなかったとき
            StrCopy(NNshGlobal->sendBuf, MSG_NOMAIL);
            StrCat (NNshGlobal->sendBuf, "\n   (");
            StrCat (NNshGlobal->sendBuf, NNshParam->hostName);
            StrCat (NNshGlobal->sendBuf, ")");
        }
        else
        {
            // メールがあったとき、メール数を表示する
            StrCopy(NNshGlobal->sendBuf, NNshGlobal->tempBuf);
            StrCat (NNshGlobal->sendBuf, MSG_MAILARRIVAL);
            StrCat (NNshGlobal->sendBuf, "\n   (");
            StrCat (NNshGlobal->sendBuf, NNshParam->hostName);
            StrCat (NNshGlobal->sendBuf, ")");

            // メッセージの詳細を表示したい場合
            if (msgBuf != NULL)
            {
                if (msgBuf[0] != '\0')
                {
                    StrCat (NNshGlobal->sendBuf, "\n---\n");
                    StrCat (NNshGlobal->sendBuf, msgBuf);
                }
            }
        }

        // 結果ダイアログを表示する
        if (FrmGetActiveFormID() != 0)
        {
            NNshGlobal->previousForm = FrmGetActiveForm();
        }
        else
        {
            NNshGlobal->previousForm = NULL;
        }
        NNshGlobal->currentForm  = FrmInitForm(FRMID_RESULT);
        // FrmSetActiveForm   (NNshGlobal->currentForm);
        NNshWinSetFieldText(NNshGlobal->currentForm, FLDID_INFOFIELD,
                            NNshGlobal->sendBuf, StrLen(NNshGlobal->sendBuf));

        FrmSetEventHandler(NNshGlobal->currentForm, Handler_MainForm);
        (void) FrmDoDialog(NNshGlobal->currentForm);

        // フォームの削除
        if (NNshGlobal->previousForm != NULL)
        {
            FrmSetActiveForm(NNshGlobal->previousForm);
        }
        FrmDeleteForm(NNshGlobal->currentForm);

        // 送受信バッファの解放
        MEMFREE_PTR(NNshGlobal->recvBuf);
        MEMFREE_PTR(NNshGlobal->sendBuf);

        // BUSYウィンドウを隠す
        Hide_BusyForm(NNshGlobal);
    }
FUNC_END:

    //////////////////////////////////////////////////////////////////////////
    /////     終了処理（設定の保存等）
    //////////////////////////////////////////////////////////////////////////

    // 設定の保存
    SaveSetting_NNsh(sizeof(NNshSavedPref), NNshParam);

    // 領域を開放する
    MEMFREE_PTR(msgBuf);
    MEMFREE_PTR(NNshParam);    
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
    Char     *area;

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

      case SELTRID_PASSWORD:
        // パスワード入力
        area = MemPtrNew(BUFSIZE * 2);
        if (area == NULL)
        {
            return (true);
        }
        MemSet(area, (BUFSIZE * 2), 0x00);
        NNshWinGetFieldText(FrmGetActiveForm(), FLDID_PASSWORD,
                                                          area, (BUFSIZE * 2));
        if (InputDialog("Input Password", area, (BUFSIZE * 2)) == true)
        {
            NNshWinSetFieldText(FrmGetActiveForm(), FLDID_PASSWORD, area,
                                                                (BUFSIZE * 2));

        }
        if (StrLen(area) == 0)
        {
            CtlSetLabel(FrmGetObjectPtr(FrmGetActiveForm(), 
                                        FrmGetObjectIndex(FrmGetActiveForm(),
                                                          SELTRID_PASSWORD)), 
                        "(Empty)");
        }
        else
        {
            CtlSetLabel(FrmGetObjectPtr(FrmGetActiveForm(), 
                                        FrmGetObjectIndex(FrmGetActiveForm(),
                                                          SELTRID_PASSWORD)), 
                        "* * *");
        }
        MEMFREE_PTR(area);
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

      case BTNID_DIALOG_OK:
      case BTNID_DIALOG_CANCEL:
      default:
        return (false);
        break;
    }
    return (true);
}

/*=========================================================================*/
/*   Function :   NNshRestoreNNsiSetting                                   */
/*                                                     NNsi設定の読み込み  */
/*=========================================================================*/
void RestoreSetting_NNsh(UInt16 size, NNshSavedPref *prm)
{
    Err           ret;
    UInt32        offset;
    UInt16        cnt, nofRec;
    DmOpenRef     dbRef;
    Char         *ptr;

    // NNsi設定の領域を初期化する
    nofRec = 0;

    if (size == sizeof(NNshSavedPref))
    {
        //  設定されたバージョンと今回起動されたバージョンおよび構造体サイズが
        // 同じだった場合のみ、NNsi設定を復旧させる。

        // 変数を初期化 
        offset = 0;
        ptr    = (Char *) prm;

        // ＤＢより、NNsi設定を読み出す(レコードの登録順の兼ね合いで、、、注意)
        OpenDatabase_NNsh(DBNAME_SETTING, DBVERSION_SETTING, &dbRef);
        GetDBCount_NNsh(dbRef, &nofRec);
        cnt = nofRec;
        while ((cnt != 0)&&(offset < sizeof(NNshSavedPref)))
        {
            ret = GetRecord_NNsh(dbRef,(cnt - 1),DBSIZE_SETTING, &ptr[offset]);
            if (ret != errNone)
            {
                // データ読み出しに失敗(この場合、NNsi設定は初期化する)
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
        // 設定を初期化する
        prm->useKey.key1      = vchrHard1;
        prm->useKey.key2      = vchrHard2;
        prm->useKey.key3      = vchrHard3;
        prm->useKey.key4      = vchrHard4;
        prm->bufferSize       = WORKBUF_DEFAULT;
        prm->portNum          = 110;     // POP3ポート番号
        prm->timeout          = SysTicksPerSecond() * NNSH_GETMAIL_TIMEOUT;
    }
    return;
}
/*-------------------------------------------------------------------------*/
/*   Function :   NNshSaveNNsiSetting                                      */
/*                                                         NNsi設定の記憶  */
/*-------------------------------------------------------------------------*/
void SaveSetting_NNsh(UInt16 size, NNshSavedPref *param)
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
        //  すでにNNsi設定が記録されていた場合、全レコードを削除する
        (void) DeleteRecordIDX_NNsh(dbRef, (nofRec - 1));
        nofRec--;
    }

    // NNsi設定をDBに登録する(レコード登録順の兼ね合いに注意！)
    offset = 0;
    ptr    = (Char *) param;
    while (offset < sizeof(NNshSavedPref))
    {
        ret = EntryRecord_NNsh(dbRef, DBSIZE_SETTING, &ptr[offset]);
        if (ret != errNone)
        {
            break;
        }
        offset = offset + DBSIZE_SETTING;
    }
    CloseDatabase_NNsh(dbRef);

    return;
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
Err NNshNet_write(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, Int32 timeout)
{
    Err    err;
    Int16  ret;

    ret = NetLibSend(netRef, socketRef, data, size, 0, NULL, 0, timeout, &err);
    if (ret == -1)
    {
        return (err);
    }
    return (errNone);
}

/*==========================================================================*/
/*  net_read() : データを受信する                                           */
/*                                                                          */
/*==========================================================================*/
Err NNshNet_read(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, UInt16 *readSize, Int32 timeout)
{
    Err    err;
    Int16  ret;

    *readSize = 0;
    ret = NetLibReceive(netRef, socketRef, data, size,0,NULL,0,timeout, &err);
    if (ret == -1)
    {
        return (err);
    }

    *readSize = ret;
    return (errNone);
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
Err NNshNet_connect(NetSocketRef *socketRef, UInt16 netRef, Char *hostName,
                    UInt16 port, Int32 timeout)
{
    Err                  err;
    Int16                ret;
    NetHostInfoPtr       hostIP;
    NetSocketAddrINType  addrType;
    NetHostInfoBufType  *infoBuff;

    // バッファ領域を確保
    infoBuff = MemPtrNew(sizeof(NetHostInfoBufType));
    if (infoBuff == NULL)
    {
        return (~errNone);
    }

    // ホスト名からIPアドレスを取得
    hostIP = NetLibGetHostByName(netRef, hostName, infoBuff, timeout, &err);
    if (hostIP == 0)
    {
        // ホストIPの取得失敗
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

    // ソケットOPEN
    *socketRef = NetLibSocketOpen(netRef, netSocketAddrINET, 
                                  netSocketTypeStream, netSocketProtoIPTCP,
                                  timeout, &err);
    if (*socketRef == -1)
    {
        // ソケットOPEN異常
        *socketRef = 0;
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }

    // ソケットコネクト
    addrType.family = netSocketAddrINET;
    addrType.port   = NetHToNS(port);
    addrType.addr   = infoBuff->address[0];
    ret = NetLibSocketConnect(netRef, *socketRef,
                              (NetSocketAddrType *) &addrType,
                              sizeof(addrType), timeout, &err);
    if (ret == -1)
    {
        // ソケットコネクト異常
        (void) NetLibSocketClose(netRef, *socketRef, timeout, &err);
        err = NNSH_ERRORCODE_FAILURECONNECT;
        goto FUNC_END;
    }
    err = errNone;

FUNC_END:
    // バッファ領域を開放
    (void) MemPtrFree(infoBuff);

    return (err);
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

/*=========================================================================*/
/*   Function : checkMailMessage_POP3                                      */
/*                                 メールヘッダのチェック本処理(POP3/APOP) */
/*=========================================================================*/
void checkMailMessage_POP3(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal, 
                           Char *buff, UInt8 *msgBuf)
{
    UInt8  *bufPtr;
    UInt16 lp, lim, size;

    /** ヘッダの取得件数分、メールボックスにメールがあるか... **/
    if (NNshGlobal->err < GET_MESSAGE_HEADER_NUM)
    {
        // ない場合...メールボックスにあるメール分ヘッダを取得
        lim = NNshGlobal->err;
    }
    else
    {
        lim = GET_MESSAGE_HEADER_NUM;
    }

    bufPtr = msgBuf;
    for (lp = 0; lp < lim; lp++)
    {
        // ヘッダー取得中の表示に変更する
        SetMsg_BusyForm(MSG_PROCESS_GETHEADER_SEND);

        // ヘッダのみ受信
        StrCopy(NNshGlobal->sendBuf, "TOP ");
        StrIToA(&NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)], (NNshGlobal->err - lp));
        StrCat (NNshGlobal->sendBuf, " 0");
        StrCat (NNshGlobal->sendBuf, "\x0d\x0a");

        if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                          StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                 NNshParam->timeout) != errNone)
        {
            // ヘッダ要求の送信エラー
            return;
        }

        // 応答メッセージを拾う
        MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
        NNshGlobal->tempPtr = NNshGlobal->recvBuf;
        NNshGlobal->ret  = errNone;
        while (NNshGlobal->ret == errNone)
        {
            // ヘッダー取得中の表示に変更する
            SetMsg_BusyForm(MSG_PROCESS_GETHEADER_RECV);

            NNshGlobal->size = 0;
            NNshGlobal->ret = NNshNet_read(NNshGlobal->netRef, 
                                           NNshGlobal->socketRef,
                                           NNshParam->bufferSize - (NNshGlobal->tempPtr - NNshGlobal->recvBuf) - 1, 
                                           NNshGlobal->tempPtr,
                                           &(NNshGlobal->size), 
                                           NNshParam->timeout);
            if (NNshGlobal->ret != errNone)
            {
                break;
            }
            if (NNshGlobal->size == 0)
            {
                break;
            }
            if (StrStr(NNshGlobal->recvBuf, "\x0d\x0a\x0d\x0a\x2e\x0d\x0a") != NULL)
            {
                break;
            }
            NNshGlobal->tempPtr = NNshGlobal->tempPtr + NNshGlobal->size;
        }

        // 応答チェック
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "-ERR");
        if (NNshGlobal->tempPtr != NULL)
        {
            // 読み出し失敗
            StrCat(bufPtr, "ERROR>");
            StrCat(bufPtr, NNshGlobal->sendBuf);
            bufPtr = bufPtr + sizeof("ERROR>") - 1 + StrLen(NNshGlobal->sendBuf) - 2;
            *bufPtr = '\n';
            bufPtr++;
            continue;
        }

        // 応答チェック
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "+OK");
        if (NNshGlobal->tempPtr == NULL)
        {
            // 読み出し失敗、次へ行く
            continue;
        }

        // ヘッダー解析中表示に変更する
        SetMsg_BusyForm(MSG_PROCESS_PARSING);

        // 応答データの抜き出し(送信元)
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "From:");
        if (NNshGlobal->tempPtr != NULL)
        {
            NNshGlobal->tempPtr = NNshGlobal->tempPtr + sizeof("From:") - 1;
            while ((*(NNshGlobal->tempPtr) != '\x00')&&
                   ((*(NNshGlobal->tempPtr) == ':')||
		    (*(NNshGlobal->tempPtr) <= ' ')))
            {
                NNshGlobal->tempPtr++;
            }
            while ((*(NNshGlobal->tempPtr) != '\x0d')&&
                   (*(NNshGlobal->tempPtr) != '\x0a')&&
                   (*(NNshGlobal->tempPtr) != '\x00'))
            {
                if ((*(NNshGlobal->tempPtr) == '=')&&(*(NNshGlobal->tempPtr + 1) == '?'))
                {
                    // MIMEエンコードを検出、、、
                    size = StrCopyMIME64ToSJ(bufPtr, NNshGlobal->tempPtr);

                    // 次の領域に移動する
                    NNshGlobal->tempPtr = NNshGlobal->tempPtr + size;
                    bufPtr = bufPtr + StrLen(bufPtr);

                    break;
                }
                else
                {
                    // MIMEエンコードされてないときは、通常コピー
                    *bufPtr = *(NNshGlobal->tempPtr);
                    bufPtr++;
                    NNshGlobal->tempPtr++;
                }
            }
        }
        *bufPtr = ' ';
        bufPtr++;

        // 応答データの抜き出し(受信日時)
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "Date:");
        if (NNshGlobal->tempPtr != NULL)
        {
            *bufPtr = '[';
            bufPtr++;

            NNshGlobal->tempPtr = NNshGlobal->tempPtr + sizeof("Date:") - 1;
            while ((*(NNshGlobal->tempPtr) != '\x00')&&
                   ((*(NNshGlobal->tempPtr) == ':')||
		    (*(NNshGlobal->tempPtr) <= ' ')))
            {
                NNshGlobal->tempPtr++;
            }
            while ((*(NNshGlobal->tempPtr) != '\x0d')&&
                   (*(NNshGlobal->tempPtr) != '\x0a')&&
                   (*(NNshGlobal->tempPtr) != '\x00'))
            {
                *bufPtr = *(NNshGlobal->tempPtr);
                bufPtr++;
                NNshGlobal->tempPtr++;
            }
            *bufPtr = ']';
            bufPtr++;
        }

        // 応答データの抜き出し(タイトル)
        NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "Subject:");
        if (NNshGlobal->tempPtr != NULL)
        {
            *bufPtr = '\n';
            bufPtr++;

            NNshGlobal->tempPtr = NNshGlobal->tempPtr + sizeof("Subject:") - 1;
            while ((*(NNshGlobal->tempPtr) != '\x00')&&
                   ((*(NNshGlobal->tempPtr) == ':')||
                   (*(NNshGlobal->tempPtr) <= ' ')))
            {
                NNshGlobal->tempPtr++;
            }
            while ((*(NNshGlobal->tempPtr) != '\x0d')&&
                   (*(NNshGlobal->tempPtr) != '\x0a')&&
                   (*(NNshGlobal->tempPtr) != '\x00'))
            {
                if ((*(NNshGlobal->tempPtr) == '=')&&(*(NNshGlobal->tempPtr + 1) == '?'))
                {
                    // MIMEエンコードを検出、、、
                    size = StrCopyMIME64ToSJ(bufPtr, NNshGlobal->tempPtr);

                    // 次の領域に移動する
                    NNshGlobal->tempPtr = NNshGlobal->tempPtr + size;
                    bufPtr = bufPtr + StrLen(bufPtr);

                    break;
                }
                else
                {
                    // MIMEエンコードされてないときは、通常コピー
                    *bufPtr = *(NNshGlobal->tempPtr);
                    bufPtr++;
                    NNshGlobal->tempPtr++;
                }
            }
        }

        // next data..
        *bufPtr = '\n';
        bufPtr++;

        *bufPtr = '-';
        bufPtr++;
        *bufPtr = '-';
        bufPtr++;
        *bufPtr = '-';
        bufPtr++;

        *bufPtr = '\n';
        bufPtr++;
    }
    if (*(bufPtr - 5) == '\n')
    {
        *(bufPtr - 5) = '\0';
    }
    return;
}

/*=========================================================================*/
/*   Function : checkMail_POP3                                             */
/*                                         メールチェック本処理(POP3/APOP) */
/*=========================================================================*/
UInt16 checkMail_POP3(UInt16 type, NNshSavedPref *NNshParam,
                      NNshWorkingInfo *NNshGlobal, Char *buff, UInt8 *msgBuf)
{
    NNshGlobal->err = ~errNone;

    // ネットライブラリOPEN
    if (NNshNet_open(&(NNshGlobal->netRef)) != errNone)
    {
        return (~errNone);
    }

    // ソケット接続
    if (NNshNet_connect(&(NNshGlobal->socketRef), NNshGlobal->netRef,
                        NNshParam->hostName, NNshParam->portNum,
                        NNshParam->timeout) != errNone)
    {
        // 接続に失敗
        goto NET_CLOSE;
    }

    // Welcomeメッセージを拾う
    if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef, 
                     NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
    {
        // 読み出し失敗
        NNshGlobal->err = ~errNone;
        goto NET_DISCONNECT;
    }

    // 応答チェック
    if (StrStr(NNshGlobal->recvBuf, "+OK") == NULL)
    {
        // 読み出し失敗
        NNshGlobal->err = ~errNone;
        goto SEND_QUIT;
    }

    // POP3の認証
    if (type == CHECKPROTOCOL_POP3)
    {
        // USER送信
        MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        StrCopy(NNshGlobal->sendBuf, "USER ");
        StrCat (NNshGlobal->sendBuf, NNshParam->userName);
        StrCat (NNshGlobal->sendBuf, "\x0d\x0a");
        if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                          StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf, 
                          NNshParam->timeout) != errNone)
        {
            // USER送信エラー
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // USERの応答メッセージを拾う
        MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
        if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef, 
                         NNshParam->bufferSize, NNshGlobal->recvBuf, 
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
        {
            // 読み出し失敗
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // QUIT応答チェック
        if (StrStr(NNshGlobal->recvBuf, "+OK") == NULL)
        {
            // 読み出し失敗
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // PASS送信
        MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        StrCopy(NNshGlobal->sendBuf, "PASS ");
        if (StrLen(NNshParam->password) != 0)
        { 
            StrCat (NNshGlobal->sendBuf, NNshParam->password);
        }
        StrCat (NNshGlobal->sendBuf, "\x0d\x0a");
        if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                          StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                NNshParam->timeout) != errNone)
        {
            // PASS送信エラー
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // PASSの応答メッセージを拾う
        MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
        if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef, 
                         NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
        {
            // 読み出し失敗
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // PASS応答チェック
        if (StrStr(NNshGlobal->recvBuf, "+OK") == NULL)
        {
            // 読み出し失敗
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }
    }
    else
    {
        // APOP認証
        NNshGlobal->tempPtr  = StrStr(NNshGlobal->recvBuf, "<");
        NNshGlobal->tempPtr2 = NNshGlobal->tempPtr;
        while (*(NNshGlobal->tempPtr2) != '>')
        {
            if (*(NNshGlobal->tempPtr2) == '\0')
            {
                // 末尾が見つからなかった
                NNshGlobal->err = ~errNone;
                goto SEND_QUIT;
            }
            (NNshGlobal->tempPtr2)++;
        }
        *(NNshGlobal->tempPtr2 + 1) = '\0';

        // MD5に変換する文字列を作成
        MemSet (NNshGlobal->tempBuf, (BUFSIZE * 2), 0x00);
        StrCopy(NNshGlobal->tempBuf, NNshGlobal->tempPtr);
        StrCat (NNshGlobal->tempBuf, NNshParam->password);

        // APOP送信
        MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        StrCopy(NNshGlobal->sendBuf, "APOP ");
        StrCat (NNshGlobal->sendBuf, NNshParam->userName);
        StrCat (NNshGlobal->sendBuf, " ");
        CalcMD5(&(NNshGlobal->sendBuf[StrLen(NNshGlobal->sendBuf)]), 
                NNshGlobal->tempBuf, NNshGlobal);
        StrCat (NNshGlobal->sendBuf, "\x0d\x0a");

        MemSet (NNshGlobal->tempBuf, (BUFSIZE * 2), 0x00);
        StrCopy(NNshGlobal->tempBuf, NNshGlobal->sendBuf);
        MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
        StrToLower(NNshGlobal->sendBuf, NNshGlobal->tempBuf);

        if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                          StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                NNshParam->timeout) != errNone)
        {
            // APOP送信エラー
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // APOPの応答メッセージを拾う
        MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
        if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef, 
                         NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
        {
            // 読み出し失敗
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }

        // APOP応答チェック
        if (StrStr(NNshGlobal->recvBuf, "+OK") == NULL)
        {
            // 読み出し失敗
            NNshGlobal->err = ~errNone;
            goto SEND_QUIT;
        }
    }

    // STAT送信
    MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
    StrCopy(NNshGlobal->sendBuf, "STAT ");
    StrCat (NNshGlobal->sendBuf, "\x0d\x0a");
    if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef, 
                      StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                NNshParam->timeout) != errNone)
    {
        // STAT送信エラー
        NNshGlobal->err = ~errNone;
        goto SEND_QUIT;
    }

    // STATの応答メッセージを拾う
    MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
    if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef,
                     NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
    {
        // 読み出し失敗
        NNshGlobal->err = ~errNone;
        goto SEND_QUIT;
    }

    // STAT応答チェック
    NNshGlobal->tempPtr = StrStr(NNshGlobal->recvBuf, "+OK");
    if (NNshGlobal->tempPtr == NULL)
    {
        // 読み出し失敗
        NNshGlobal->err = ~errNone;
        goto SEND_QUIT;
    }

    // 新着メール数を応答バッファに格納
    NNshGlobal->err = 0;
    while ((*NNshGlobal->tempPtr < '0')||(*(NNshGlobal->tempPtr) > '9'))
    {
        (NNshGlobal->tempPtr)++;
    }
    StrCopy(buff, NNshGlobal->tempPtr);
    NNshGlobal->tempPtr = buff;
    while ((*(NNshGlobal->tempPtr) >= '0')&&(*(NNshGlobal->tempPtr) <= '9'))
    {
        (NNshGlobal->tempPtr)++;
    }
    *(NNshGlobal->tempPtr) = '\0';
    NNshGlobal->err = StrAToI(buff);

    if (NNshParam->getMessageHeader != 0)
    {
        // メッセージヘッダの取得を実施する場合...
        checkMailMessage_POP3(NNshParam, NNshGlobal, buff, msgBuf);
    }

SEND_QUIT:
    // QUIT送信
    MemSet (NNshGlobal->sendBuf, NNshParam->bufferSize, 0x00);
    StrCopy(NNshGlobal->sendBuf, "QUIT\x0d\x0a");
    if (NNshNet_write(NNshGlobal->netRef, NNshGlobal->socketRef,
                      StrLen(NNshGlobal->sendBuf), NNshGlobal->sendBuf,
                                                NNshParam->timeout) != errNone)
    {
        // QUIT送信エラー
        goto NET_DISCONNECT;
    }

    // QUITの応答メッセージを拾う
    MemSet (NNshGlobal->recvBuf, NNshParam->bufferSize, 0x00);
    if (NNshNet_read(NNshGlobal->netRef, NNshGlobal->socketRef,
                     NNshParam->bufferSize, NNshGlobal->recvBuf,
                            &(NNshGlobal->ret), NNshParam->timeout) != errNone)
    {
        // 読み出し失敗
        goto NET_DISCONNECT;
    }

NET_DISCONNECT:
    // ソケット切断
    NNshNet_disconnect(NNshGlobal->netRef, NNshGlobal->socketRef, 
                                                           NNshParam->timeout);

NET_CLOSE:
    // ネットライブラリクローズ
    NNshNet_close(NNshGlobal->netRef);
    return (NNshGlobal->err);
}

/*=========================================================================*/
/*   Function : StrCopyMIME64ToSJ                                          */
/*                           MIME64ヘッダをSHIFT JISに変換して文字列コピー */
/*=========================================================================*/
UInt16 StrCopyMIME64ToSJ(UInt8 *dst, UInt8 *src)
{
    UInt8  *ptr;
    UInt16  size;

    // エンコードタイプの確認
    if ((*(src + 15) == 'q')||(*(src + 15) == 'Q'))
    {
        // quoted-printableの場合には変換しない。。。
        *dst = *src;
        dst++;
        src++;
        *dst = *src;
        dst++;
        src++;
        *dst ='\0';
        return (2);
    }

    // ヘッダ部分を読み飛ばす
    src = src + 16;  // '=?iso-2022-jp?B?'   
    size = 0;
    while ((*(src + size) != '\x0d')&&(*(src + size) != '\x0a')&&
            (*(src + size) != '\x00')&&(*(src + size) != '?'))
    {
        size++;
    }
    ptr = MemPtrNew(size + 16);
    if (ptr == NULL)
    {
        MemMove(dst, src, size);
        return (size);
    }
    MemSet(ptr, (size + 16), 0x00);

    // BASE64 -> JISに変換
    ConvertFromBASE64(ptr, src, size);

    // JIS -> SHIFT JISに変換
    StrCopyJIStoSJ(dst, ptr);

    MEMFREE_PTR(ptr);
    return (size);
}

/*=========================================================================*/
/*   Function : MD5(文字列)を求める                                        */
/*                                                                         */
/*=========================================================================*/
void CalcMD5(Char *buffer, Char *string, NNshWorkingInfo *NNshGlobal)
{
    Char   buff[14];
    UInt16 loop;
    UInt32 data;

    // データの前準備
    MemSet(&(NNshGlobal->digest),  sizeof(NNshGlobal->digest), 0x00);
    MemSet(&(NNshGlobal->PADDING), sizeof(NNshGlobal->PADDING), 0x00);
    NNshGlobal->PADDING[0] = 0x80;

    // MD5を計算
    MD5Init  (&(NNshGlobal->context));
    MD5Update(&(NNshGlobal->context), string, StrLen(string));
    MD5Final (NNshGlobal->digest, &(NNshGlobal->context), NNshGlobal->PADDING);

    NNshGlobal->tempPtr2 = buffer;
    for (loop = 0; loop < 16; loop++)
    {
        MemSet (buff, sizeof(buff), 0x00);
        data = ((UInt8) NNshGlobal->digest[loop]);
        StrIToH(buff, data);
        StrCat(buffer, &buff[6]);
    }

    // FrmCustomAlert(ALTID_INFO, string, " >> MD5 >> ",  buffer);
    return;
}

//////////////////////////////////////////////////////////////////////////////
//   RFC1321より、、、そのまんまもらってきました(w。
//////////////////////////////////////////////////////////////////////////////

/* Constants for MD5Transform routine. */

#define S11  7
#define S12 12
#define S13 17
#define S14 22
#define S21  5
#define S22  9
#define S23 14
#define S24 20
#define S31  4
#define S32 11
#define S33 16
#define S34 23
#define S41  6
#define S42 10
#define S43 15
#define S44 21

/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
 FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/*---------------------------------------------------------------------------*/
/* MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm            */
/*---------------------------------------------------------------------------
   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. 
  All rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 ---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    MD5 initialization. Begins an MD5 operation, writing a new context.
 *---------------------------------------------------------------------------*/
void MD5Init (MD5_CTX *context)
{
    context->count[0] = context->count[1] = 0;

    /* Load magic initialization constants. */
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;

    return;
}

/*---------------------------------------------------------------------------*
    MD5 block update operation. Continues an MD5 message-digest
    operation, processing another message block, and updating the
    context.
 *---------------------------------------------------------------------------*/
void MD5Update(MD5_CTX *context, UInt8 *input, UInt16 inputLen)
{
  UInt16 i, index, partLen;

  /* Compute number of bytes mod 64 */
  index = (UInt16)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((UInt32)inputLen << 3)) < ((UInt32)inputLen << 3))
  {
     context->count[1]++;
  }
  context->count[1] += ((UInt32)inputLen >> 29);

  partLen = 64 - index;

  /* Transform as many times as possible. */
  if (inputLen >= partLen) 
  {
     MD5_memcpy(&(context->buffer[index]), input, partLen);
     MD5Transform (context->state, context->buffer);

     for (i = partLen; i + 63 < inputLen; i += 64)
     {
         MD5Transform (context->state, &input[i]);
     }
     index = 0;
  }
  else
  {
      i = 0;
  }

  /* Buffer remaining input */
  MD5_memcpy(&(context->buffer[index]), &input[i], inputLen - i);

  return;
}

/*---------------------------------------------------------------------------*
     MD5 finalization. Ends an MD5 message-digest operation, writing the
    the message digest and zeroizing the context.
 *---------------------------------------------------------------------------*/
void MD5Final(UInt8 digest[16], MD5_CTX *context, UInt8 *PADDING)
{
  UInt8  bits[8];
  UInt16 index, padLen;

  /* Save number of bits */
  MD5sub_Encode(bits, context->count, 8);

  /* Pad out to 56 mod 64. */
  index  = (UInt16)((context->count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  MD5Update (context, PADDING, padLen);

  /* Append length (before padding) */
  MD5Update (context, bits, 8);

  /* Store state in digest */
  MD5sub_Encode (digest, context->state, 16);

  /* Zeroize sensitive information.*/
  MemSet(context, sizeof(*context), 0x00);

  return;
}

/*---------------------------------------------------------------------------*/
/*   MD5 basic transformation. Transforms state based on block.              */
/*---------------------------------------------------------------------------*/
void MD5Transform (UInt32 state[4], UInt8 block[64])
{
  UInt32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

  MD5sub_Decode(x, block, 64);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

 /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  /* Zeroize sensitive information. */
  MemSet(x, sizeof(x), 0x00);
}

/*---------------------------------------------------------------------------*
    Encodes input (UInt32) into output (UInt8). Assumes len is
    a multiple of 4.
 *---------------------------------------------------------------------------*/
void MD5sub_Encode(UInt8 *output, UInt32 *input, UInt16 len)
{
  UInt16  i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
  {
      output[j]   = (UInt8)(input[i] & 0xff);
      output[j+1] = (UInt8)((input[i] >> 8) & 0xff);
      output[j+2] = (UInt8)((input[i] >> 16) & 0xff);
      output[j+3] = (UInt8)((input[i] >> 24) & 0xff);
  }
}

/*---------------------------------------------------------------------------*
    Decodes input (UInt8) into output (UInt32). Assumes len is
    a multiple of 4.
 *---------------------------------------------------------------------------*/
void MD5sub_Decode(UInt32 *output, UInt8 *input, UInt16 len)
{
  UInt16 i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
  {
     output[i] = ((UInt32)input[j]) | (((UInt32)input[j+1]) << 8) |
       (((UInt32)input[j+2]) << 16) | (((UInt32)input[j+3]) << 24);
  }
  return;
}

/*---------------------------------------------------------------------------*
 *   Note: Replace "for loop" with standard memcpy if possible.
 *---------------------------------------------------------------------------*/
void MD5_memcpy(UInt8 *output, UInt8 *input, UInt16 len)
{
    UInt16 i;

    for (i = 0; i < len; i++)
    {
        output[i] = input[i];
    }
    return;
}


/*=========================================================================*/
/*   Function : ConvertJIStoSJ                                             */
/*                            JIS漢字コードをSHIFT JIS漢字コードに変換する */
/*=========================================================================*/
void ConvertJIStoSJ(UInt8 *dst, UInt8 *src)
{
    UInt8 upper, lower;

    // 上位8ビット/下位8ビットを変数にコピーする
    upper = *src;
    lower = *(src + 1);

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

    return;
}

/*=========================================================================*/
/*   Function : StrCopyJIStoSJ                                             */
/*                          JIS漢字コードをSHIFT JISに変換して文字列コピー */
/*=========================================================================*/
void StrCopyJIStoSJ(UInt8 *dst, UInt8 *src)
{
    Boolean kanji;
    
    kanji = false; 
    while (*src != '\0')
    {
        if ((*src == '\x1b')&&(*(src + 1) == '\x24')&&(*(src + 2) == '\x42'))
        {
            // 漢字モードに切り替え
            kanji = true;
            src = src + 3;
            continue;
        }
        if ((*src == '\x1b')&&(*(src + 1) == '\x28')&&(*(src + 2) == '\x42'))
        {
            // ANKモードに切り替え
            kanji = false;
            src = src + 3;
            continue;
        }

        // データの１文字コピー
        if (kanji == true)
        {
            ConvertJIStoSJ(dst, src);
            dst = dst + 2;
            src = src + 2;
        }
        else
        {
            // 通常モード...
            *dst = *src;
            dst++;
            src++;
        }
    }
    *dst = '\0';
    return;
}


/*----------------------------*/
/* conv64Ascii                */
/*        BASE64用変換テーブル */
/*----------------------------*/
UInt8 conv64Ascii(UInt8 data)
{
    if ((data >= 'A')&&(data <= 'Z'))
    {
        return (data - 'A');
    }
    if ((data >= 'a')&&(data <= 'z'))
    {
        return ((data - 'a') + 0x1a);
    }
    if ((data >= '0')&&(data <= '9'))
    {
        return ((data - '0') + 0x34);
    }
    switch (data)
    {
      case '+':
        return (0x3e);
        break;

      case '/':
        return (0x3f);
        break;

      case '=':
      default:
        break;
    }
    return (0x40);
}

/*=========================================================================*/
/*   Function : ConvertFromBASE64                                          */
/*                                                          BASE64デコード */
/*=========================================================================*/
void ConvertFromBASE64(UInt8 *dst, UInt8 *src, UInt16 size)
{
    UInt8 *ptr, temp0, temp1, temp2, temp3;
    ptr = src;
    while (ptr < (src + size))
    {
        temp0 = conv64Ascii(*ptr);
        temp1 = conv64Ascii(*(ptr + 1));
        temp2 = conv64Ascii(*(ptr + 2));
        temp3 = conv64Ascii(*(ptr + 3));

        *dst       = ((((0x3f & temp0) << 2) & 0xfc) | (((0x30 & temp1) >> 4) & 0x03));
        *(dst + 1) = ((((0x0f & temp1) << 4) & 0xf0) | (((0x3c & temp2) >> 2) & 0x0f));
        *(dst + 2) = ((((0x03 & temp2) << 6) & 0xc0) | (((0x3f & temp3)     ) & 0x3f));
        ptr = ptr + 4;
        dst = dst + 3;
    }
    *dst = '\0';

    return;
}
