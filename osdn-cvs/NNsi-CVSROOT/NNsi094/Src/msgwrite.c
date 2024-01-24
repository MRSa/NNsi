/*============================================================================*
 *  FILE: 
 *     msgwrite.c
 *
 *  Description: 
 *     Message write functions for NNsh. 
 *
 *===========================================================================*/
#define MSGWRITE_C
#include "local.h"

static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command);
static Err create_outputMessageFile(FormType *diagFrm, Char *buffer,
                                    UInt32 bufSize, Char *boardNick, 
                                    Char *key, UInt16 bbsType);
static Err threadWrite_Message(void);

/*-------------------------------------------------------------------------*/
/*   Function : returnToMessageView                                        */
/*                                                          参照画面に戻る */
/*-------------------------------------------------------------------------*/
static void returnToMessageView(void)
{
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(FRMID_MESSAGE);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : saveWritingMessage                                         */
/*                                            編集中のメッセージを保存する */
/*-------------------------------------------------------------------------*/
static void saveWritingMessage(void)
{
    Err          ret;
    Char        *buffer;
    UInt32       writeSize;
    FormType    *frm;
    NNshFileRef  fileRef;

    // 一時バッファ領域を確保
    buffer = MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MemPtrNew() ", " size:",
                          NNshParam->bufferSize + MARGIN);
        return;
    }
    MemSet(buffer, (NNshParam->bufferSize + MARGIN), 0x00);

    // 編集中メッセージを取得する
    frm = FrmGetActiveForm();
    NNshWinGetFieldText(frm,FLDID_WRITE_MESSAGE,buffer,NNshParam->bufferSize);
    if (StrLen(buffer) == 0)
    {
        // 編集中文書がない場合は終了する
        goto FUNC_END;

    }

    // 編集中文書を保存するか確認する
    if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_SAVEMSG, "", 0) != 0)
    {
        // Cancelボタンが押された、終了する
        goto FUNC_END;
    }

    // 編集中文書を(Palmデバイス内の)ファイル(write.txt)に書き込む
    ret = OpenFile_NNsh(FILE_WRITEMSG,
                        (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
        goto FUNC_END;
    }
    ret = AppendFile_NNsh(&fileRef, StrLen(buffer), buffer, &writeSize);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", ret);
    }
    ret = CloseFile_NNsh (&fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "CloseFile() ", " ret:", ret);
    }
    else
    {
        NNsh_DebugMessage(ALTID_INFO, "Save Message", " bytes:", writeSize);
    }

FUNC_END:
    MemPtrFree(buffer);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : menuEvt_WriteMessage                                       */
/*                                            メニューが選択された時の処理 */
/*-------------------------------------------------------------------------*/
static Boolean menuEvt_WriteMessage(EventType *event)
{
    switch (event->data.menu.itemID)
    {
        // 回線切断
      case MNUID_NET_DISCONN:
        NNshNet_LineHangup();
        break;

        // 書き込み指示
      case MNUID_WRITE_START:
        if (threadWrite_Message() != errNone)
        {
            // 書き込み失敗(接続失敗)時、書き込み画面に戻る。
            return (false);
            break;
	}
        returnToMessageView();
        break;

        // 書き込み中止 ほか(スレ参照画面に戻る)
      case MNUID_WRITE_ABORT:
        // 編集中メッセージをバックアップ
        saveWritingMessage();
        returnToMessageView();
        break;

        // 編集メニュー群
      case MNUID_EDIT_UNDO:
      case MNUID_EDIT_CUT:
      case MNUID_EDIT_COPY:
      case MNUID_EDIT_PASTE:
      case MNUID_SELECT_ALL:
        (void) NNsh_MenuEvt_Edit(event);
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : checkCheckBoxUpdate                                        */
/*                                  チェックボックスが更新されたときの処理 */
/*-------------------------------------------------------------------------*/
static void checkCheckBoxUpdate(UInt16 chkId)
{
    Char         data[BUFSIZE];
    UInt16       fieldId, *chkBox;
    FormType    *frm;
    frm = FrmGetActiveForm();

    MemSet(data, BUFSIZE, 0x00);
    switch (chkId)
    {
      case CHKID_HANDLENAME:
        StrNCopy(data, NNshParam->handleName, BUFSIZE - 1);
        fieldId = FLDID_WRITE_NAME;
        chkBox  = &(NNshParam->useFixedHandle);
        break;

      case CHKID_SAGE:
      default:
        StrCopy(data, "sage");
        fieldId = FLDID_WRITE_EMAIL;
        chkBox  = &(NNshParam->writeAlwaysSage);
        break;
    }

    // チェックボックスにチェックがつけられていた場合
    if (CtlGetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,chkId))) != 0)
    {
        // データ設定
        NNshWinSetFieldText(frm, fieldId, data, StrLen(data));
        *chkBox = 1;
    }
    else
    {
        // データクリア
        MemSet(data, BUFSIZE, 0x00);
        NNshWinSetFieldText(frm, fieldId, data, 2);
        *chkBox = 0;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : selEvt_Control_WriteMessage                                */
/*                                               ボタンが押された時の処理  */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_Control_WriteMessage(EventType *event)
{
    Err ret;
    switch (event->data.ctlSelect.controlID)
    {
        // 書き込みボタン(書き込み後、スレ参照画面に戻る)
      case BTNID_WRITE_MESSAGE:
        ret = threadWrite_Message();
        if (ret != errNone)
        {
            // 書き込み失敗(接続失敗)時、書き込み画面に戻る。
            return (false);
            break;
	}
        returnToMessageView();
        break;

        // Clearボタン(書き込み領域をクリアする)
      case BTNID_WRITEMSG_CLR:
        NNshWinSetFieldText(FrmGetActiveForm(),
                            FLDID_WRITE_MESSAGE,"", 0);
        break;

        // Cancelボタン ほか(スレ参照画面に戻る)
      case BTNID_WRITE_CANCEL:
        // 編集中メッセージをバックアップ
        saveWritingMessage();
        returnToMessageView();
        break;

        // 名前欄/Email欄横のチェックが更新されたとき
      case CHKID_HANDLENAME:
      case CHKID_SAGE:
        checkCheckBoxUpdate(event->data.ctlSelect.controlID);
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectKeyDownEvent                                         */
/*                                                 キーが押された時の処理  */
/*                                (ジョグアシストＯＦＦ時のジョグ処理追加) */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_KeyDown_WriteMessage(EventType *event)
{
    Err       err;
    FormType *frm;
    Boolean   ret = false;
    UInt16    keyCode;

    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // 「下」(ジョグダイヤル下)を押した時の処理
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
        NNshWinViewPageScroll(FLDID_WRITE_MESSAGE,
                              SCLID_WRITE_MESSAGE, 0, winDown); 
        ret = true;
        break;

      // 「上」(ジョグダイヤル上)を押した時の処理
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
        NNshWinViewPageScroll(FLDID_WRITE_MESSAGE,
                              SCLID_WRITE_MESSAGE, 0, winUp); 
        ret = true;
        break;

      // CLIEでCtrl+Enter もしくは Jogを押したとき
      case vchrJogPush:
        err = threadWrite_Message();
        if (err != errNone)
        {
            // 書き込み失敗(接続失敗)時、書き込み画面に戻る。
            break;
	}
        returnToMessageView();
        ret = true;
        break;
 
      // Tabキーを押したときの処理
      case chrHorizontalTabulation:
        // フォーカスをメッセージにあてる
        frm = FrmGetActiveForm();
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
        ret = true;
        break;

      // その他のボタン群
      case vchrJogPushedUp:
      case vchrJogPushedDown:
      case vchrJogBack:
      default:
        break;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : sclRepEvt_WriteMessage                                     */
/*                                       スクロールバー更新イベントの処理  */
/*-------------------------------------------------------------------------*/
static Boolean sclRepEvt_WriteMessage(EventType *event)
{
    Int16   lines;

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
      (void) NNshWinViewPageScroll(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE,
                                   -lines, winUp);
    }
    else if (lines > 0)
    {
      (void) NNshWinViewPageScroll(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE,
                                   lines, winDown);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : fldChgEvt_WriteMessage                                     */
/*                                                     スクロールバー更新  */
/*-------------------------------------------------------------------------*/
static Boolean fldChgEvt_WriteMessage(EventType *event)
{
    NNshWinViewUpdateScrollBar(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Hander_WriteMessage                                      */
/*                                                       イベントハンドラ  */
/*-------------------------------------------------------------------------*/
Boolean Handler_WriteMessage(EventType *event)
{
    Boolean   ret = false;
#ifdef USE_HANDERA
    FormType *frm;
#endif

    switch (event->eType)
    {
      case keyDownEvent:
        // キー入力
        return (selEvt_KeyDown_WriteMessage(event));
        break;

      case ctlSelectEvent:
        // ボタン押下
        return (selEvt_Control_WriteMessage(event));
        break;

      case fldChangedEvent:
        // フィールドに文字入力
        return (fldChgEvt_WriteMessage(event));
        break;

      case sclRepeatEvent:
        // スクロールバー更新
        return (sclRepEvt_WriteMessage(event));
        break;

      case menuEvent:
        // メニュー選択
        return (menuEvt_WriteMessage(event));
       break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent :
	if(NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
	    HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
	break;
#endif // #ifdef USE_HANDERA

      case fldEnterEvent:
      case fldHeightChangedEvent:
      case frmOpenEvent:
      case frmUpdateEvent:
      case popSelectEvent:
      case lstSelectEvent:
      case lstEnterEvent:
      case lstExitEvent:
      case sclExitEvent:
      default:
        break;
    }
    return (ret);
}

/*=========================================================================*/
/*   Function : OpenForm_WriteMessage                                      */
/*                                                    書き込み指示時の処理 */
/*=========================================================================*/
void OpenForm_WriteMessage(FormType *frm)
{
    Err                  ret;
    Char                 msg[BUFSIZE], *buffer;
    UInt32               fileSize, readSize;
    NNshFileRef          fileRef;
    NNshSubjectDatabase  subjDB;
#if 0
    FieldAttrType        attrP;
    FieldType           *fldP;

    // アンダーラインの表示設定
    MemSet(&attrP, sizeof(attrP), 0x00);
    fldP  = FrmGetObjectPtr(frm, 
                            FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
    FldGetAttributes(fldP, &attrP);
    if (NNshParam->disableUnderline != 0)
    {
        attrP.underlined = noUnderline;
    }
    else
    {
        attrP.underlined = grayUnderline;
    }
    FldSetAttributes(fldP, &attrP);
#endif

    // FILE_WRITEMSGが存在するかチェックする
    ret = OpenFile_NNsh(FILE_WRITEMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret == errNone)
    {
        // 受信メッセージサイズを取得
        GetFileSize_NNsh(&fileRef, &fileSize);
        if (fileSize != 0)
        {
            // 一時バッファ領域を確保
            buffer = MemPtrNew(NNshParam->bufferSize + MARGIN);
            if (buffer != NULL)
            {
                MemSet(buffer, (NNshParam->bufferSize + MARGIN), 0x00);
                // ファイルからデータを読み出す
                ret = ReadFile_NNsh(&fileRef, 0, NNshParam->bufferSize,
                                    buffer, &readSize);
                if ((ret == errNone)||(ret == fileErrEOF))
                {
                    NNshWinSetFieldText(frm, FLDID_WRITE_MESSAGE,
                                        buffer, StrLen(buffer));
                    ret = errNone;
                }
                MemPtrFree(buffer);
            }
            else
            {
                NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc. ", "", ret);
                ret = ~errNone;
            }
        }
        else
        {
            NNsh_DebugMessage(ALTID_ERROR, "file size is 0. ", "", ret);
            ret = ~errNone;
        }
        CloseFile_NNsh(&fileRef);
    }
    // 保存してあったファイルを削除する
    (void) DeleteFile_NNsh(FILE_WRITEMSG, NNSH_VFS_DISABLE);

    // レス番号をフィールドの先頭に追加する。
    if ((ret != errNone)&&(NNshParam->insertReplyNum != 0)&&
        (Get_Subject_Database(NNshParam->openMsgIndex, &subjDB) == errNone))
    {
        MemSet(msg, sizeof(msg), 0x00);
        StrCopy(msg, ">>");
        NUMCATI(msg, subjDB.currentLoc);
        StrCat (msg, "\n");
        NNshWinSetFieldText(frm, FLDID_WRITE_MESSAGE,msg, StrLen(msg));
    }

    // 常にsage書き込み(パラメータ設定時)
    if (NNshParam->writeAlwaysSage != 0)
    { 
        NNshWinSetFieldText(frm, FLDID_WRITE_EMAIL,"sage", StrLen("sage"));

        // チェックボックスも設定する
        CtlSetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,CHKID_SAGE)),1);
    }

    // コテハン機能を使用する(パラメータ設定時)
    if ((NNshParam->useFixedHandle != 0)&&(NNshParam->handleName[0] != '\0'))
    {
        NNshWinSetFieldText(frm, FLDID_WRITE_NAME, NNshParam->handleName,
                            StrLen(NNshParam->handleName));

        // チェックボックスも設定する
        CtlSetValue(FrmGetObjectPtr(frm,
                                  FrmGetObjectIndex(frm,CHKID_HANDLENAME)),1);
    }

    // スクロールバーを更新
    NNshWinViewUpdateScrollBar(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE);

    // フォーカスをメッセージフィールドに設定(パラメータが設定されてない場合)
    if (NNshParam->notAutoFocus == 0)
    {
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkWriteReply                                          */
/*                                                                         */
/*             書き込み後の応答確認(設定要求Cookie or 書き込み結果 を抽出) */
/*-------------------------------------------------------------------------*/
static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command)
{
     NNshFileRef  fileRef;
     UInt32       readSize;
     Char         *start, *end, *ptr;
     Err          ret;

     // 受信メッセージを開いて、サーバからの応答を読み出す
     MemSet(buffer, bufSize, 0x00);
     ret = OpenFile_NNsh(FILE_RECVMSG,
                         (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                         &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Open HTTP Reply Message ", "", ret);
        return (NULL);
    }
    (void) ReadFile_NNsh(&fileRef, 0, bufSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);

    if (command == NNSH_WRITECHK_REPLY)
    {
        // サーバからの応答について、タイトル部分のみ抽出する。(暫定？)
        ptr = buffer;
        start = StrStr(ptr, "<title>");
        if (start == NULL)
        {
            start = StrStr(ptr, "<TITLE>");
        }
        if (start != NULL)
        {
            start = start + StrLen("<TITLE>");
        }
        else
        {
            start = ptr;
        }
        end   = StrStr(start, "</title>");
        if (end == NULL)
        {
            end = StrStr(start, "</TITLE>");
        }
        if (end != NULL)
        {
            *end = '\0';
        }
    }
    else
    {
        // 書き込み指示 Cookieの位置を探す
        ptr   = buffer;
        start = StrStr(ptr, "Set-Cookie: ");
        if (start == NULL)
        {
            return (NULL);
        }

        // 書き込み指示Cookieの最後尾を探す
        end = start;
        while ((end <= start + bufSize)&&(*end != ';'))
        {
            end++;
        }
        *end = '\0';
        NNsh_DebugMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, start, 0);
    }
    return (start);
}

/*-------------------------------------------------------------------------*/
/*   Function : threadWrite_Message                                        */
/*                                                           書き込み処理  */
/*-------------------------------------------------------------------------*/
static Err threadWrite_Message(void)
{
    UInt16               index;
    NNshSubjectDatabase  subjDB;
    NNshBoardDatabase    bbsData;
    Err                  ret;
    Char                 subBuf[BIGBUF], msgID[MAX_THREADFILENAME];
    Char                 spId[BUFSIZE], *url, *buffer, *ptr, *start;
    FormType            *frm;

    // メッセージ情報をデータベースから取得
    ret = Get_Subject_Database(NNshParam->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_INFO, "get_subject_database()", "", ret);
        return (ret);
    }

    // BBS情報をデータベースから取得
    ret = Get_BBS_Database(subjDB.boardNick, &bbsData, &index);

    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          subjDB.boardNick, ret);
        return (ret);
    }

    // 書き込みするかを確認するメッセージ
    if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_WRITEMSG,
                            subjDB.threadTitle, 0) != 0)
    {
        return (~errNone);
    }

    // データ送信用ワークバッファの獲得
    buffer = (Char *) MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        // 領域の確保失敗！
        NNsh_InformMessage(ALTID_ERROR, "SEND AREA MemPtrNew()",
                           " size:", NNshParam->bufferSize + MARGIN);
        return (~errNone);
    }
    MemSet (buffer, (NNshParam->bufferSize + MARGIN), 0x00);
    MemSet (subBuf, sizeof(subBuf), 0x00);
    StrCopy(subBuf, subjDB.boardNick);
    subBuf[StrLen(subjDB.boardNick) - 1] = '\0';

    MemSet (msgID, sizeof(msgID), 0x00);
    StrCopy(msgID, subjDB.threadFileName);
    ptr = StrStr (msgID, ".");
    if (ptr != NULL)
    {
        *ptr = '\0';
    }
    frm = FrmGetActiveForm();
    // 送信するメッセージをsend.txtへ出力する
    create_outputMessageFile(frm, buffer, NNshParam->bufferSize,
                             subBuf, msgID, (UInt16) subjDB.bbsType);
    MemPtrFree(buffer);

    //　送信先URLの作成
    MemSet(subBuf, sizeof(subBuf), 0x00);
    url = subBuf;
    StrCopy(url, bbsData.boardURL);
    if (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // まちBBSの場合
        StrCat(url, URL_PREFIX_MACHIBBS);
        StrCat(url, bbsData.boardNick);
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        }
        StrCat(url, "&KEY=");
        StrCat(url, msgID); 
    }
    else
    {
        // 2chの場合
        StrCopy(&url[StrLen(url) - StrLen(bbsData.boardNick)],  URL_PREFIX);
        StrCat (url, bbsData.boardNick);
        StrCat (url, msgID);
        StrCat (url,  "/");
    }

    // メッセージを送信
    NNsh_DebugMessage(ALTID_INFO, "Send URL(1)>", url, 0);
    if (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_MACHIBBS, url,
                            NULL, "&submit=%8F%91%82%AB%8D%9E%82%DE",
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    }
    else
    {
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url,
                            NULL, "&submit=%8F%91%82%AB%8D%9E%82%DE",
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    }
    if (ret != errNone)
    {
        if (ret != netErrTimeout)
        {
            // (タイムアウト以外の)通信エラー発生
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_ERROR_HTTP_COMM, "(pre-Write)", ret);
        }
        else
        {
            // タイムアウト発生
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_OCCUR_TIMEOUT, "(pre-Write)", ret);
        }
        return (ret);
    }

    /////////////////////////////////////////////////////////////////////////
    //   受信したCookieを使用して書き込みを行わない場合、またホストから
    // 「書きこみました。」と応答があった場合には、Cookieを使用した書きこみは
    // 行わないようにする
    // (まちBBSの場合もCookie使用の書き込みを行わないようにする)
    /////////////////////////////////////////////////////////////////////////

    // サーバからの応答を解析
    MemSet(subBuf, sizeof(subBuf), 0x00);
    start = checkWriteReply(subBuf, sizeof(subBuf), NNSH_WRITECHK_REPLY);
    if ((NNshParam->useCookieWrite == 0)||
        (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (StrCompare(start, NNSH_SYMBOL_WRITE_DONE) == 0))
    {
        // スレメッセージの再送信は行わない、差分取得チェックへ
        goto WRITE_NEXT;
    }

    // Cookieを使用して書きこみメッセージを再送信 

    // サーバからの応答を解析、SPIDを取得する
    MemSet(subBuf, sizeof(subBuf), 0x00);
    ptr = checkWriteReply(subBuf, sizeof(subBuf), NNSH_WRITECHK_SPID);
    if (ptr == NULL)
    {
        // サーバからの応答が異常だった
        return (~errNone);
    }

    // SPIDを取得する
    MemSet(spId, sizeof(spId), 0x00);
    StrNCopy(spId, ptr, sizeof(spId) - 1);

    //　送信先URLの作成
    MemSet(subBuf, sizeof(subBuf), 0x00);
    url = subBuf;
    StrCopy(url, bbsData.boardURL);
    StrCopy(&url[StrLen(url) - StrLen(bbsData.boardNick)],  URL_PREFIX);
    StrCat (url, bbsData.boardNick);
    StrCat (url, msgID);
    StrCat (url,  "/");

    // (cookieを再設定して)メッセージ送信
    NNsh_DebugMessage(ALTID_INFO, "Send URL(2)>", url, 0);
    ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url,
                        spId,
                        "&submit=%91S%90%D3%94C%82%F0%95"
                        "%89%82%A4%82%B1%82%C6%82%F0%8F%B3"
                        "%91%F8%82%B5%82%C4%8F%91%82%AB%8D"
                        "%9E%82%DE",
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    if (ret != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_ERROR_HTTP_COMM, "(Write)", ret);
        }
        else
        {
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_OCCUR_TIMEOUT, "(Write)", ret);
        }
        return (ret);
    }
    // サーバからの応答を解析
    MemSet(subBuf, sizeof(subBuf), 0x00);
    start = checkWriteReply(subBuf, sizeof(subBuf), NNSH_WRITECHK_REPLY);

WRITE_NEXT:

    // 応答解析の結果を表示する
    NNsh_InformMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, start, 0);

    // 書き込み先がまちBBSかどうかチェックする
    if (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // まちBBSに書き込んだときは、スレ差分取得ができないので折り返す。
        NNsh_InformMessage(ALTID_INFO, MSG_NOTSUPPORT_MACHI2, "", 0);
        return (errNone);
    }

    // スレ差分取得を行う
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsData.boardURL);
    StrCat (url, "dat/");
    StrCat (url, subjDB.threadFileName);
    (void) Get_PartMessage(url, bbsData.boardNick, &subjDB, 
                           NNshParam->openMsgIndex);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : create_outputMessageFile                                   */
/*                                                書き込みメッセージの出力 */
/*-------------------------------------------------------------------------*/
static Err create_outputMessageFile(FormType *diagFrm, Char *buffer,
                                    UInt32 bufSize, Char *boardNick, 
                                    Char *key, UInt16 bbsType)
{
    Err          ret;
    Char        *ptr;
    Int16        timeZone, dayLight;
    UInt32       dummy;
    NNshFileRef  sendRef;

    // 出力用ファイルの準備
    (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
    ret = OpenFile_NNsh(FILE_SENDMSG, 
                        (NNSH_FILEMODE_APPEND|NNSH_FILEMODE_TEMPFILE),
                        &sendRef);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "OpenFile_NNsh() ", FILE_SENDMSG, ret);
        return (ret);
    }

    // key部分の出力
    MemSet (buffer, bufSize, 0x00);
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCopy(buffer, "KEY=");
    }
    else
    {
        StrCopy(buffer, "key=");
    }
    StrCat (buffer, key);
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCat (buffer, "&NAME=");
    }
    else
    {
        StrCat (buffer, "&FROM=");
    }
    ptr = buffer;
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);

    // 名前欄の出力
    NNshWinGetFieldText(diagFrm, FLDID_WRITE_NAME, buffer, bufSize);
    ptr = buffer;
    if (StrLen(ptr) != 0)
    {
        (void) AppendFileAsURLEncode_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    }

    // E-Mail欄の出力
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        ptr = "&MAIL=";
    }
    else
    {
        ptr = "&mail=";
    }
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    NNshWinGetFieldText(diagFrm, FLDID_WRITE_EMAIL, buffer, bufSize);
    ptr = buffer;
    if (StrLen(ptr) != 0)
    {
        (void) AppendFileAsURLEncode_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    }

    // メッセージ欄の出力
    ptr = "&MESSAGE=";
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    NNshWinGetFieldText(diagFrm, FLDID_WRITE_MESSAGE, buffer, bufSize);
    ptr = buffer;
    if (StrLen(ptr) != 0)
    {
        (void) AppendFileAsURLEncode_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    }

    // BBS名および現在時刻の出力
    MemSet (buffer, bufSize, 0x00);
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCopy(buffer, "&BBS=");
    }
    else
    {
        StrCopy(buffer, "&bbs=");
    }
    StrCat (buffer, boardNick);

    // 時刻を設定する(今後、2chの仕様にあわせて変更する予定あり)
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCat (buffer, "&TIME=");
    }
    else
    {
        StrCat (buffer, "&time=");
    }

    // PalmOS version 4以上のときは、時刻をUTCに変換してから計算する
    ret = FtrGet(sysFtrCreator, sysFtrNumROMVersion, &dummy);
    if ((ret == errNone)&&(dummy >= 0x04003000))
    {
        timeZone = PrefGetPreference(prefTimeZone);
        dayLight = PrefGetPreference(prefDaylightSavingAdjustment);
        dummy    = TimUTCToTimeZone(TimGetSeconds(), timeZone, dayLight);
        NUMCATI(buffer, dummy - TIME_CONVERT_1904_1970_UTC);
    }
    else
    {
        NUMCATI(buffer, TimGetSeconds() - TIME_CONVERT_1904_1970);
    }
    (void) AppendFile_NNsh(&sendRef, StrLen(buffer), buffer, &dummy);

    // ファイルクローズ
    CloseFile_NNsh(&sendRef);

    return (errNone);
}
