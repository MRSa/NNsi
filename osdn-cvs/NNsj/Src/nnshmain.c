/*============================================================================*
 *  FILE: 
 *     nnshset.c
 *
 *  Description: 
 *     NNsh parameter setting functions for NNsh.
 *
 *===========================================================================*/
#define NNSHMAIN_C
#include "local.h"


/*=========================================================================*/
/*   Function : saveMacroData                                              */
/*                                                 マクロスクリプトの出力  */
/*=========================================================================*/
Boolean  SaveMacroData(Char *name)
{
    Err    ret;
    UInt16 butID;

    // マクロを出力するかどうか確認する
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_WRITE_DB, name, 0);
    if (butID != 0)
    {
        // マクロデータの出力を中止
        return (true);
    }

    // マクロデータの保存
    ret = WriteMacroDBData(name,
                           DBVERSION_MACROSCRIPT,
                           SOFT_CREATOR_ID,
                           DATA_DBTYPE_ID, 
                           NNshGlobal->currentScriptLine,
                           NNshGlobal->scriptArea);
    if (ret == errNone)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_WRITEDB_OK, name, ret);
        NNshGlobal->editFlag = false;
        return (true);
    }
    NNsh_DebugMessage(ALTID_ERROR, "Write Failure :", name, ret);
    return (false);
}

/*=========================================================================*/
/*   Function : LoadMacroData                                              */
/*                                             マクロスクリプトの読みだし  */
/*=========================================================================*/
Boolean  LoadMacroData(Char *name)
{
    Err    ret;
    UInt16 butID;

    // マクロを出力するかどうか確認する
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_READ_DB, name, 0);
    if (butID != 0)
    {
        // マクロデータの出力を中止
        return (true);
    }

    // マクロデータの読み出し
    MemSet(NNshGlobal->scriptArea,
           (sizeof(NNshMacroRecord) * (NNshGlobal->maxScriptLine) + MARGIN),
           0x00);
    ret = ReadMacroDBData(name, DBVERSION_MACROSCRIPT,
                          SOFT_CREATOR_ID, DATA_DBTYPE_ID,
                          &(NNshGlobal->currentScriptLine), 
                          NNshGlobal->maxScriptLine,
                          NNshGlobal->scriptArea);
    if (ret == errNone)
    {
        NNshGlobal->editFlag = true;
        return (true);
    }
    NNsh_DebugMessage(ALTID_ERROR, "Read Failure :", name, ret);
    return (false);
}

/*=========================================================================*/
/*   Function : DeleteMacroData                                            */
/*                                                 マクロスクリプトの削除  */
/*=========================================================================*/
Boolean  DeleteMacroData(Char *name)
{
    UInt16 butID;
    LocalID dbId;

    // マクロを削除するかどうか確認する
    butID = NNsh_ConfirmMessage(ALTID_WARN, name, MSG_CONFIRM_DELETE_DB, 0);
    if (butID != 0)
    {
        // マクロデータの削除を中止
        return (true);
    }
    dbId = DmFindDatabase(0, name);
    if (dbId != 0)
    {
        // DBを削除する
        (void) DmDeleteDatabase(0, dbId);
        NNsh_InformMessage(ALTID_INFO, MSG_DB_DELETED, name, 0);
        return (true);
    }
    // DBがなかった...削除を中止する
    return (true);
}


/*=========================================================================*/
/*   Function :   EffectData_NNshFiler                                     */
/*                                        フォームから設定された情報を取得 */
/*=========================================================================*/
void EffectData_NNshFiler(FormType *frm, Char *fileName)
{
    MemHandle  txtH;
    FieldType *fldP;
    Char      *txtP;
    
    // ファイル名を取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_FILENAME));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        if ((txtP != NULL)&&(*txtP != '\0'))
        {
            StrCopy(fileName, txtP);
        }
        MemHandleUnlock(txtH);
    }
    return; 
}


/*=========================================================================*/
/*   Function :   Handler_NNshFiler                                        */
/*                                   イベントハンドラ（ファイルOPEN/CLOSE) */
/*=========================================================================*/
Boolean Handler_NNshFiler(EventType *event)
{
    Char    **fnameP, *txtP;
    UInt16    keyCode, cnt, max;
    FormType *frm;
    ListType *lstP;

    frm = FrmGetActiveForm();
    switch (event->eType)
    { 
      case keyDownEvent:
        // キー入力
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          // 「下」(ジョグダイヤル下)を押した時の処理
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrNextField:
          case vchrThumbWheelDown:
          case chrDownArrow:
          case vchrPageDown:
            // 何もしない
            lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
            max  = LstGetNumberOfItems(lstP);
            cnt  = LstGetSelection(lstP);
            if (cnt == noListSelection)
            {
                cnt = 0;
            }
            else if (cnt < (max - 1))
            {
                cnt++;
            }
            LstSetSelection(lstP, cnt);
            txtP = LstGetSelectionText(lstP, cnt);
            NNshWinSetFieldText(frm, FLDID_FILENAME, true, txtP, StrLen(txtP));
            break;

          // 「上」(ジョグダイヤル上)を押した時の処理
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrPrevField:
          case vchrThumbWheelUp:
          case chrUpArrow:
          case vchrPageUp:
            // 何もしない
            lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
            cnt  = LstGetSelection(lstP);
            if (cnt == noListSelection)
            {
                cnt = 0;
            }
            else if (cnt > 0)
            {
                cnt--;
            }
            LstSetSelection(lstP, cnt);
            txtP = LstGetSelectionText(lstP, cnt);
            NNshWinSetFieldText(frm, FLDID_FILENAME, true, txtP, StrLen(txtP));
            break;

          case vchrJogRelease:
          case vchrThumbWheelPush:
          case chrCarriageReturn:
          case chrLineFeed:
          case vchrJogLeft:
            // 改行コードが入力された、OKボタンを押したことにする
            if ((StrLen(NNshParam->fileName) == 0)||(NNshParam->fileName[0] == ' '))
            {
                //  ファイル名が入力されていない、このときは選択されている
                // リストのファイル名を使用する
                lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
                cnt = LstGetSelection(lstP);
                if (cnt == noListSelection)
                {
                    cnt = 0;
                }
                txtP = LstGetSelectionText(lstP, cnt);
                if (*txtP != ' ')
                {
                    StrCopy(NNshParam->fileName, txtP);
                }
            }
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_FILE_OK)));
            break;

          case vchrJogBack:
          case chrEscape:
          case vchrThumbWheelBack:
          case vchrJogRight:
            // ESCコードが入力された、CANCELボタンを押したことにする
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_FILE_CANCEL)));
            break;

          case chrLeftArrow:         // カーソルキー左(5way左)
          case chrRightArrow:         // カーソルキー右(5way右)
          default:
            // 何もしない
            break;
        }        
        break;

      case ctlSelectEvent:
        // ボタンが押された
        break;

      case popSelectEvent:
        // ポップアップが選択された
        break;

      case lstSelectEvent:
        // リストを選択
        // (ファイル名欄に選択したアイテムを表示する)
        MemHandleUnlock(NNshGlobal->fileListH);
        fnameP = MemHandleLock(NNshGlobal->fileListH);
        txtP = (Char *) fnameP[event->data.lstSelect.selection];
        if (*txtP != ' ')
        {
            MemSet(NNshParam->fileName, (MAXLENGTH_FILENAME + MARGIN), 0x00);
            StrCopy(NNshParam->fileName, txtP);        
            NNshWinSetFieldText(frm, FLDID_FILENAME, true,NNshParam->fileName,
                                StrLen(NNshParam->fileName));
        }
        break;

      case menuEvent:
      case sclRepeatEvent:
      case fldChangedEvent:
      default:
        // 何もしない 
        break;
    }
    return (false);
}


/*-------------------------------------------------------------------------*/
/*   Function : setFileListItems                                           */
/*                                                ファイル一覧のリスト設定 */
/*-------------------------------------------------------------------------*/
static void setFileListItems(FormType *frm, Char **fnameP)
{
    Err                  ret, err;
    UInt16               cardNo, cnt;
    LocalID              dbLocalID;
    DmSearchStateType    state;
    Char                 fileName[MINIBUF + MINIBUF + MARGIN], **ptr;
    ListType            *lstP;

    // VFSの使用設定にあわせてフラグを設定、ファイル検索と、一覧表示
    ptr = fnameP;
    cnt = 0;

    // Palm本体内にあるファイルの一覧を取得
    ret = DmGetNextDatabaseByTypeCreator(true, &state, 
                                         DATA_DBTYPE_ID, 
                                         SOFT_CREATOR_ID,
                                         false, &cardNo, &dbLocalID);
    while ((ret == errNone)&&(dbLocalID != 0))
    {
        // ファイル名を取得
        MemSet(fileName, sizeof(fileName), 0x00);
        err = DmDatabaseInfo(cardNo, dbLocalID, fileName,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL);
        if (err == errNone)
        {
            // 取得したファイル名をリストアイテムにコピー
            if ((fnameP != NULL)&&(cnt < MAX_FILELIST))
            {
                *ptr = MemPtrNew_NNsh(StrLen(fileName) + MARGIN);
                if (*ptr != NULL)
                {
                    MemSet(*ptr, (StrLen(fileName) + MARGIN), 0x00);
                    StrCopy(*ptr, fileName);
                    cnt++;
                    ptr++;
                }
            }
        }
        // 二回目以降のデータ取得
        ret = DmGetNextDatabaseByTypeCreator(false, &state,
                                             DATA_DBTYPE_ID, 
                                             SOFT_CREATOR_ID,
                                             false, &cardNo, &dbLocalID);
    }

    // ファイルが１件もない場合には、スペースを表示する
    if (cnt == 0)
    {
        *ptr = MemPtrNew_NNsh(StrLen("  ") + MARGIN);
        StrCopy(*ptr, "  ");
        cnt++;
        ptr++;
    }

    // ファイル一覧をリスト設定(ただし、無選択にする)
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_FILESELECTION));   
    LstSetListChoices(lstP, fnameP, cnt);
    LstSetTopItem    (lstP, 0);
    LstSetSelection  (lstP, noListSelection);
    LstDrawList(lstP);

    return;
}

/*=========================================================================*/
/*   Function : OpenForm_NNshFiler                                         */
/*                                        設定項目(モーダルフォーム)の表示 */
/*=========================================================================*/
Err OpenForm_NNshFiler(FormType *frm, UInt16 command, Char **fnameP)
{
    switch (command)
    {
      case BTNID_DB_SAVE:
        // データの保管
         FrmCopyTitle(frm, "Save Macro...");
        break;

      case BTNID_DB_READ:
      default:
        // データの読み出し
         FrmCopyTitle(frm, "Read Macro...");
        break;
    }

    // ファイル名リストを設定する
    setFileListItems(frm, fnameP);

    // ファイル名入力欄に前回入力したファイル名を入れ、フォーカスを設定する
    if (StrLen(NNshParam->fileName) != 0)
    {
        NNshWinSetFieldText(frm, FLDID_FILENAME, true,
                            NNshParam->fileName, StrLen(NNshParam->fileName));
    }
    FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_FILENAME));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   NNsh_FileOperation                                       */
/*                                                  データの読み込み/保管  */
/*=========================================================================*/
Boolean NNsh_FileOperation(UInt16 command)
{
    Boolean   ret;
    Char     *txtP, **area;
    UInt16    btnId, size, cnt;
    FormType *prevFrm, *diagFrm;

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // ファイルリストをクリアする
    MEMFREE_HANDLE(NNshGlobal->fileListH);
    area = NULL;
    size = sizeof(Char *) * (MAX_FILELIST + MARGIN);
    NNshGlobal->fileListH = MemHandleNew(size);
    if (NNshGlobal->fileListH != 0)
    {
        area = MemHandleLock(NNshGlobal->fileListH);
        MemSet(area, size, 0x00);
    }

    // ダイアログ表示の準備
    PrepareDialog_NNsh(FRMID_FILE_SAVELOAD, &diagFrm, false);
    if (diagFrm == NULL)
    {
        // 表示の準備失敗
        return (false);
    }

    // 設定初期値の反映
    OpenForm_NNshFiler(diagFrm, command, area);
    
    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_NNshFiler);

    // 画面表示
    btnId = FrmDoDialog(diagFrm);
    switch (btnId)
    {
      case BTNID_FILE_OK:
      case BTNID_FILE_DELETE:
        // フォーム上にあるファイル名を取得する
        MemSet(NNshParam->fileName, (MAXLENGTH_FILENAME + MARGIN), 0x00);
        EffectData_NNshFiler(diagFrm, NNshParam->fileName);
        break;

      case BTNID_FILE_CANCEL:
      default:
        // 何もしない(実行キャンセル)
        break;
    }
    
    // 表示画面を元に戻す
    PrologueDialog_NNsh(prevFrm, diagFrm);
    FrmDrawForm(FrmGetActiveForm());

    if (area != NULL)
    {
        // ファイルリストをクリアする
        for (cnt = 0; cnt < (MAX_FILELIST + MARGIN); cnt++)
        {
            txtP = (Char *) area[cnt];
            MEMFREE_PTR(txtP);
        }
        MemSet(area, sizeof(Char *) * (MAX_FILELIST + MARGIN), 0x00);
    }
    if (NNshGlobal->fileListH != 0)
    {
        MemHandleUnlock(NNshGlobal->fileListH);
        MEMFREE_HANDLE (NNshGlobal->fileListH);
    }

    // 実処理への分岐
    switch (btnId)
    {
      case BTNID_FILE_OK:
        switch (command)
        {
          case BTNID_DB_SAVE:
            // 保存
            SaveMacroData(NNshParam->fileName);
            break;

          case BTNID_DB_READ:
          default:
            // 読み込み
            LoadMacroData(NNshParam->fileName);
            break;
        }
        break;

      case BTNID_FILE_DELETE:
        // 削除
        DeleteMacroData(NNshParam->fileName);
        break;

      default:
        break;
    }
    return (false);
}


/*=========================================================================*/
/*   Function : insertOneLine                                              */
/*                                                         １行挿入モード  */
/*=========================================================================*/
static void insertOneLine(void)
{
    NNshGlobal->editMode = NNSH_MODE_INSERT;
    NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DB_INFO, true, MSG_INSERTLINE_POINT, MINIBUF);
    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                        BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
    return;
}

/*=========================================================================*/
/*   Function : deleteOneLine                                              */
/*                                                         １行削除モード  */
/*=========================================================================*/
static void deleteOneLine(void)
{
    NNshGlobal->editMode = NNSH_MODE_DELETE;
    NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DB_INFO, true, MSG_DELETELINE_POINT, MINIBUF);
    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                        BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
    return;
}

/*=========================================================================*/
/*   Function : changeDispFont                                             */
/*                                                         フォントの変更  */
/*=========================================================================*/
static Boolean changeDispFont()
{
    NNshParam->currentFont = FontSelect(NNshParam->currentFont);
    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                        BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
    return (true);
}
/*=========================================================================*/
/*   Function : extractMacroData                                           */
/*                                       マクロスクリプトの実行領域へ出力  */
/*=========================================================================*/
Boolean  extractMacroData(void)
{
    Err          ret;
    UInt16       butID;
    EventType   *dummyEvent;

    // マクロを出力するかどうか確認する
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_OUTPUT_MACRO, "", 0);
    if (butID != 0)
    {
        // マクロデータの出力を中止
        // (NNsjを終了させるかどうか確認する)
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_APLEND, "", 0);
        if (butID == 0)
        {
            // アプリケーションを終了させる
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType = appStopEvent;
            EvtAddEventToQueue(dummyEvent);
        }
        return (true);
    }

    // マクロデータの保存
    ret = WriteMacroDBData(DBNAME_MACROSCRIPT,
                           DBVERSION_MACROSCRIPT,
                           DATA_CREATOR_ID,
                           0, 
                           NNshGlobal->currentScriptLine,
                           NNshGlobal->scriptArea);
    if (ret == errNone)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_WRITE_OK, DBNAME_MACROSCRIPT, ret);

        // NNsjを終了させるかどうか確認する
#ifdef USE_CONFIRM_NNSJ_END
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_APLEND, "", 0);
        if (butID == 0)
#endif
        {
            // アプリケーションを終了させる
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType = appStopEvent;
            EvtAddEventToQueue(dummyEvent);
        }
        NNshGlobal->editFlag = false;
        return (true);
    }
    NNsh_DebugMessage(ALTID_ERROR, "Write Failure :", DBNAME_MACROSCRIPT, ret);
    return (false);
}


/*=========================================================================*/
/*   Function : deleteAllScript                                            */
/*                                                     スクリプトの全削除  */
/*=========================================================================*/
Boolean deleteAllScript(void)
{
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_ALLDELETE, "", 0) == 0)
    {
        NNshGlobal->currentScriptLine = 0;
        
        MemSet(NNshGlobal->scriptArea,
           (sizeof(NNshMacroRecord) * (NNshGlobal->maxScriptLine) + MARGIN),
           0x00);

        // 画面を再描画する
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        NNshGlobal->editFlag = true;
    }
    return (true);
}

/*=========================================================================*/
/*   Function : outputMemoPad                                              */
/*                                                       メモ帳へ出力する  */
/*=========================================================================*/
void outputMemoPad(void)
{
    UInt16           loop,    len;
    Char            *dataPtr, *ptr;
    NNshMacroRecord *lineData;

    // データの初期化
    dataPtr = MemPtrNew_NNsh(NNSH_WORKBUF_DEFAULT + MARGIN);
    if (dataPtr == NULL)
    {
        // メモリ確保エラー発生、、
        return;
    }
    MemSet(dataPtr, (NNSH_WORKBUF_DEFAULT + MARGIN), 0x00);

    // 一時的にワークバッファを出力する
    ptr = dataPtr;
    len = NNSH_WORKBUF_DEFAULT;
    lineData = (NNshMacroRecord *) NNshGlobal->scriptArea;
    for (loop = 0; loop <= NNshGlobal->currentScriptLine; loop++)
    {
        PrintMacroLine(ptr, len, (loop + 1), &lineData[loop]);
        StrCat(ptr, "\n");
        ptr = dataPtr + StrLen(dataPtr);
        len = NNSH_WORKBUF_DEFAULT - (ptr - dataPtr);       
    }
    SendToLocalMemopad("Script", dataPtr);

    MemPtrFree(dataPtr);
    dataPtr = NULL;
    return;
}

/*=========================================================================*/
/*   Function : Handler_ViewLineAction                                     */
/*                                                       イベントハンドラ  */
/*=========================================================================*/
Boolean NNsh_KeyDownEvt(EventType *event)
{
    UInt16 command;

    // コマンドを初期化
    command = NNSH_STEP_REDRAW;

    switch (KeyConvertFiveWayToJogChara(event))
    {
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrNextField:
      case vchrThumbWheelDown:
      case chrDownArrow:
        // 1ページ下
        command = NNSH_STEP_PAGEDOWN;
        break;

      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrPrevField:
      case vchrThumbWheelUp:
      case chrUpArrow:
        // 1ページ上
        command = NNSH_STEP_PAGEUP;
        break;

      case chrCapital_R:  // Rキーの入力
      case chrSmall_R:
      case chrCapital_L:  // Lキーの入力
      case chrSmall_L:
        // マクロデータの読み込み
        NNsh_FileOperation(BTNID_DB_READ);
        // 画面を再描画する
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        break;

      case chrCapital_S:  // Sキーの入力
      case chrSmall_S:
        // マクロデータの書き込み
        NNsh_FileOperation(BTNID_DB_SAVE);
        // 画面を再描画する
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        break;

      case chrCapital_F:  // Fキーの入力
      case chrSmall_F:
        // 表示フォントの変更
        return (changeDispFont());
        break;

      // HandEra JOG Push選択/Enterキー入力時
      case vchrJogRelease:
      case chrCarriageReturn:
      case chrLineFeed:
      case vchrThumbWheelPush:
        // マクロデータの設定
        return (extractMacroData());
        break;

      default:
        // 上記以外のキーでは何もしない
        break;
    }
    if (command != NNSH_STEP_REDRAW)
    {
        // ページを移動させる
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, BTNID_FIELDUP, BTNID_FIELDDOWN, command);
        return (true);
    }
    // 何もしない
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_RctPointEvt                                         */
/*                                       メインフォームのイベントハンドラ  */
/*=========================================================================*/
Boolean NNsh_RctPointEvt(EventType *event, RectangleType *dimF)
{
    Boolean          edit = false;
    UInt16           fontHeight, nlines, line;
    Coord            pointX, pointY, dimX, dimY;
    NNshMacroRecord *source;

    // フォント高さを取得する
    fontHeight   = FntCharHeight();
    if (fontHeight == 0)
    {
        // フォント情報異常、終了する
        return (false);
    }

    // 座標系の変換(ポイント)
    pointY = event->screenY - dimF->topLeft.y;
    pointX = event->screenX - dimF->topLeft.x;
    NNsi_ConvertLocation(&pointX, &pointY);

    // 座標系の変換
    dimX   = dimF->extent.x;
    dimY   = dimF->extent.y;
    NNsi_ConvertLocation(&dimX, &dimY);
    
    // 解像度にあわせて表示可能行数(nlines)を計算
    nlines = ((dimY) / (fontHeight));

    // データの格納されている領域を取得する
    source = (NNshMacroRecord *) NNshGlobal->scriptArea;

    // タップされたラインが何行目か計算する
    line = (pointY / fontHeight) +  NNshGlobal->pageTopLine;
    if (line <= NNshGlobal->currentScriptLine)
    {
        // 編集モードにより動作を変更する
        switch (NNshGlobal->editMode)
        {
          case NNSH_MODE_NORMAL:
          default:
            // 通常の編集モード、タップされた行の編集ダイアログを開く
            edit = OpCodeInputDialog((line + 1), &(source[line]));
            // 一番最後の行を編集したら、１行編集領域を追加する。
            if ((edit == true)&&(line == NNshGlobal->currentScriptLine))
            {
                (NNshGlobal->currentScriptLine)++;
            }
            break;

          case NNSH_MODE_INSERT:
            // 挿入モード
            if (NNshGlobal->currentScriptLine != NNshGlobal->maxScriptLine)
            {
                MemMove(&source[line + 1], &source[line], 
                        sizeof(NNshMacroRecord) * (NNshGlobal->currentScriptLine - line));
                MemSet(&source[line], sizeof(NNshMacroRecord), 0x00);
                (NNshGlobal->currentScriptLine)++;
                edit = true;
            }
            break;

          case NNSH_MODE_DELETE:
            // 削除モード
            if (NNshGlobal->currentScriptLine != 0)
            {
                MemMove(&source[line], &source[line + 1], 
                    sizeof(NNshMacroRecord) * (NNshGlobal->currentScriptLine - line));
                (NNshGlobal->currentScriptLine)--;
                if (NNshGlobal->pageTopLine > NNshGlobal->currentScriptLine)
                {
                    // 画面を１ページ上にする
                    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                                 BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_PAGEUP);
                }
                edit = true;
            }
            break;
        }
    }
    if (NNshGlobal->editMode != NNSH_MODE_NORMAL)
    {
        // フィールドに通常データ書き込み
        NNshGlobal->editMode = NNSH_MODE_NORMAL;
        NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DB_INFO, true, NNshGlobal->infoArea, MINIBUF);
    }
    if (edit == true)
    {
        // 画面を編集したことにする
        NNshGlobal->editFlag = true;
    }

    // 画面を再描画する
    NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
    return (edit);
}

/*=========================================================================*/
/*   Function :   NNsh_MainCtlEvt                                          */
/*                                       メインフォームのイベントハンドラ  */
/*=========================================================================*/
Boolean NNsh_MainCtlEvt(EventType *event)
{
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_SET_NNSIMACRO:
        // マクロデータの設定
        return (extractMacroData());
        break;

      case BTNID_DB_SAVE:
        // マクロ保存
        NNsh_FileOperation(BTNID_DB_SAVE);
        // 画面を再描画する
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        break;

      case BTNID_DB_READ:
        // マクロ読み込み
        NNsh_FileOperation(BTNID_DB_READ);
        // 画面を再描画する
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                             BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_REDRAW);
        break;

      case BTNID_INSERTLINE:
        // 1行挿入
        insertOneLine();
        break;

      case BTNID_DELETELINE:
        // 1行削除
        deleteOneLine();
        break;

  
      default:
        // その他
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_MainCtlRepEvt                                       */
/*                                       メインフォームのイベントハンドラ  */
/*=========================================================================*/
Boolean NNsh_MainCtlRepEvt(EventType *event)
{
    UInt16 command;

    // コマンドを初期化
    command = NNSH_STEP_REDRAW;

    switch (event->data.ctlRepeat.controlID)
    {
      case BTNID_FIELDUP:
        // 1ページ上
        command = NNSH_STEP_PAGEUP;
        break;

      case BTNID_FIELDDOWN:
        // 1ページ下
        command = NNSH_STEP_PAGEDOWN;
        break;
  
      default:
        // その他
        break;
    }
    if (command != NNSH_STEP_REDRAW)
    {
        // ページを移動させる
        NNsi_putLine(FrmGetActiveForm(), GADID_GADGET_FIELD, 
                                      BTNID_FIELDUP, BTNID_FIELDDOWN, command);
        return (true);
    }
    return (false);

}

/*=========================================================================*/
/*   Function :   NNsh_MainMenuEvt                                         */
/*                                       メインフォームのイベントハンドラ  */
/*=========================================================================*/
Boolean NNsh_MainMenuEvt(EventType *event)
{

    switch (event->data.menu.itemID)
    {
      case MNUID_VERSION:
        // バージョンの表示
        ShowVersion_NNsh();
        break;
      case MNUID_CHANGEFONT:
        // 表示フォントの変更
        return (changeDispFont());
        break;

      case MNUID_ALLDELETE:
        // 全削除
        return (deleteAllScript());
        break;

      case MNUID_INSERT:
        // 1行挿入
        insertOneLine();
        break;

      case MNUID_DELETE:
        // 1行削除
        deleteOneLine();
        break;

      case MNUID_OUTPUT_MEMOPAD:
        // メモ帳へ出力
        outputMemoPad();
        break;

      default:
        // 何もしない
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Hander_MainForm                                          */
/*                                     NNsi設定系の(総合)イベントハンドラ  */
/*=========================================================================*/
Boolean Handler_MainForm(EventType *event)
{
    FormType      *frm;
    RectangleType  dimF;

    switch (event->eType)
    { 
      case menuEvent:
        // メニューを選択
        return (NNsh_MainMenuEvt(event));
        break;

      case ctlSelectEvent:
        // ボタンが押された
        return (NNsh_MainCtlEvt(event));
        break;

      case ctlRepeatEvent:
        // ボタンが押された
        return (NNsh_MainCtlRepEvt(event));
        break;

      case penDownEvent:
        frm = FrmGetActiveForm();
        FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_GADGET_FIELD),&dimF);
        if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
        {
            // Gadget内をタップした
            return (NNsh_RctPointEvt(event, &dimF));
        }
        return (false);
        break;
      case keyDownEvent:
        // keyが押された
        return (NNsh_KeyDownEvt(event));
        break;

      default: 
        return (false);
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_MainForm                                        */
/*                                                      MAINフォームを開く */
/*=========================================================================*/
Err OpenForm_MainForm(FormType *frm)
{
    RectangleType dimF;    

    // フィールドにダミーデータ書き込み
    NNshWinSetFieldText(frm, FLDID_DB_INFO, true, NNshGlobal->infoArea, MINIBUF);
    
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, GADID_GADGET_FIELD), &dimF);

    NNsi_putLine(frm, GADID_GADGET_FIELD, BTNID_FIELDUP, BTNID_FIELDDOWN, NNSH_STEP_TO_TOP);

    return (errNone);
}
