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

static void setBBSNameAndTime(UInt16 bbsType, Char *boardNick,
                              Char *buffer, UInt16 bufSize);
static void setFormParameters(Char *nick, Char *buffer, UInt16 bufSize,
                            Char *tempArea, UInt16 areaSize, Char *defSubmit);
static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command, UInt16 bbsType, Err *ret);
static Err create_outputMessageFile(FormType *diagFrm, Char *buffer,
                                    UInt32 bufSize, Char *boardNick, 
                                    Char *key, UInt16 bbsType);
static Err threadWrite_Message(void);


/*-------------------------------------------------------------------------*/
/*   Function : setBBSNameAndTime                                          */
/*                                               BBS名と現在時刻を設定する */
/*-------------------------------------------------------------------------*/
static void setBBSNameAndTime(UInt16 bbsType, Char *boardNick,
                              Char *buffer, UInt16 bufSize)
{
    Int16    timeZone, dayLight;
    UInt32   dummy;

    // BBS名の出力
    MemSet (buffer, bufSize, 0x00);
    if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
    {
        StrCopy(buffer, "&BBS=");
    }
    else
    {
        StrCopy(buffer, "&bbs=");
    }
    StrCat (buffer, boardNick);

    // BBS一覧の末尾に '/' がついていたら外す。
    if (buffer[StrLen(buffer) - 1] == '/')
    {
        buffer[StrLen(buffer) - 1] = '\0';
    }

    // 現在時刻の出力
    if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
    {
        StrCat (buffer, "&TIME=");
    }
    else
    {
        StrCat (buffer, "&time=");
    }

    // PalmOS version 4以上のときは、時刻をUTCに変換してから計算する
    if (NNshGlobal->palmOSVersion >= 0x04003000)
    {
        timeZone = PrefGetPreference(prefTimeZone);
        dayLight = PrefGetPreference(prefDaylightSavingAdjustment);
        dummy    = TimTimeZoneToUTC(TimGetSeconds(), timeZone, dayLight);
        NUMCATI(buffer, dummy - TIME_CONVERT_1904_1970_UTC);
    }
    else
    {
        NUMCATI(buffer, TimGetSeconds() - TIME_CONVERT_1904_1970);
    }

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : setFormParameters                                          */
/*                                        フォームからパラメータを取得する */
/*-------------------------------------------------------------------------*/
static void setFormParameters(Char *nick, Char *buffer,   UInt16 bufSize,
                              Char *tempArea, UInt16 areaSize, Char *defSubmit)
{
    Err          ret;
    Char        *ptr, *data, numBuf[16];
    NNshFileRef  fileRef;
    UInt32       readSize, fileSize, offset, num;

    // 受信したフォームのパラメータを解析する
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_RECVMSG, ret);
        goto USE_DEFAULT;
    }

    // 受信メッセージサイズを取得
    GetFileSize_NNsh(&fileRef, &fileSize);

    // ファイルの読み出し位置を設定する
    if (fileSize <= bufSize)
    {
        offset = 0;
    }
    else
    {
        offset = fileSize - bufSize;
    }
    ret = ReadFile_NNsh(&fileRef, offset, bufSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :",  FILE_RECVMSG, ret);
        goto USE_DEFAULT;
    }

    // パラメータを引っ張り出す
    ptr  = buffer;
    data = &(tempArea[StrLen(tempArea)]);
    while (ptr < (buffer + bufSize))
    {
        // 先頭を探す
        ptr = StrStr(ptr, "<input type=hidden name=");
        if (ptr == NULL)
        {
            // パラメータが見つからなかった、ループを抜ける
            break;
        }
        ptr = ptr + StrLen("<input type=hidden name=");
        *data++ = '&';
        while ((*ptr != ' ')&&(*ptr != '>'))
        {
            *data++ = *ptr;
            ptr++;
        }
        // '=' が出るまで読み飛ばし
        while ((*ptr != '=')&&(*ptr != '>'))
        {
            ptr++;
        }
        *data++ = *ptr;
        ptr++;

        //   valueの先頭が'"'(ダブルクオーテーション)だった場合は
        // パラメータとして認識させない(本当にこんなんで良いのか？)
        // ...よくなかった...(おーーい、hanaちゃーん...)
        if (*ptr == '"')
        {
            do {
                *data ='\0';
                data--;
            } while (*data != '&');
            continue;
        }
        
        // 取得したパラメータ値をコピーする
        while ((*ptr != ' ')&&(*ptr != '>')&&(*ptr != '"'))
        {
            *data++ = *ptr;
            ptr++;
        }
    }
    *data = '\0';

    if (*tempArea == '\0')
    {
        // パラメータ抽出失敗、標準のパラメータを格納する
        goto USE_DEFAULT;
    }
    StrCat(tempArea, "&submit=");

    // submitボタンのバリューを取得
    ptr = StrStr(buffer, "<input type=submit value=");
    if (ptr == NULL)
    {
        //  パラメータが見つからなかった、デフォルトのsubmitボタンの値を
        // コピーしてループを抜ける
        StrCat(tempArea, defSubmit);
    }
    else
    {
        //////// submitボタンの内容を(URLエンコードして)反映させる //////
        // (dataは、コピー先データの先頭 、ptrはコピー元バッファ)
        data = tempArea + StrLen(tempArea);
        ptr = ptr + StrLen("<input type=submit value=");
        if (*ptr == '"')
        {
            // ダブルクオーテーションなら１つすすめる
            ptr++;
        }
        // 取得したパラメータ値をコピーする
        while ((ptr < (buffer + bufSize))&&
               ((*ptr != ' ')&&(*ptr != '>')&&(*ptr != '"')))
        {
            if ((*ptr == '.')||(*ptr == '_')||
                (*ptr == '-')||(*ptr == '*')||
                ((*ptr >= '0')&&(*ptr <= '9'))||
                ((*ptr >= 'A')&&(*ptr <= 'Z'))||
                ((*ptr >= 'a')&&(*ptr <= 'z')))
            {
                //  そのままコピーする
                *data++ = *ptr;
                ptr++;
            }
            else
            {
                // URLエンコードする(0x88 => '%88' に変換する)
                *data = '%';
                data++;
                *data = '\0';
                num = (UInt32) *ptr;
                MemSet(numBuf, sizeof(numBuf), 0x00);
                StrIToH(numBuf, num);

                // 数値の下２桁が欲しいので...
                StrCat(data, &numBuf[6]);
                data = data + 2; // 2 == StrLen(data);
                ptr++;
            }
        }
    }
    *data = '\0';

    NNsh_DebugMessage(ALTID_INFO, "Param :",  tempArea, 0);
    return;

USE_DEFAULT:
    // 標準のBBS名と時間を取得する
    MemSet(tempArea, areaSize, 0x00);
    setBBSNameAndTime(NNSH_BBSTYPE_2ch, nick, tempArea, areaSize);

    NNsh_DebugMessage(ALTID_INFO, "Param(Default):",  tempArea, 0);
    return;
}

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

/*=========================================================================*/
/*   Function : SaveWritingMessage                                         */
/*                                            編集中のメッセージを保存する */
/*=========================================================================*/
void SaveWritingMessage(void)
{
    Err          ret;
    Char        *buffer, *ptr;
    UInt32       writeSize;
    FormType    *frm;
    NNshFileRef  fileRef;

    // 一時バッファ領域を確保
    buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR() ", " size:",
                          (NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        return;
    }
    MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);

    // 編集中メッセージを取得する
    frm = FrmGetActiveForm();
    NNshWinGetFieldText(frm,FLDID_WRITE_MESSAGE,buffer,(NNshGlobal->NNsiParam)->writeBufferSize);
    if (StrLen(buffer) == 0)
    {
        // 編集中文書がない場合は終了する
        MEMFREE_PTR(buffer);
        return;
    }
    // レス番号のみ入力されていた場合は、保存しないためのチェック
    if ((buffer[0] == '>')&&(buffer[1] == '>'))
    {
        // 参照番号が先頭に記入されていた場合
        ptr = buffer + 2;   // 2は、StrLen(">>") の意味
        while ((*ptr >= '0')&&(*ptr <= '9'))
        {        
            ptr++;
        }
        if (*ptr == '\n')
        {
            ptr++;
            if (StrLen(ptr) == 0)
            {
                // レス番号のみ記入されていた、保存せず終了する
                MEMFREE_PTR(buffer);
                return;
            }
        }
    }

    // 書き込み画面終了時レスを自動で保存がOFFのとき
    if ((NNshGlobal->NNsiParam)->writeMessageAutoSave == 0)
    {
        // 編集中文書を保存するか確認する
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_SAVEMSG, "", 0) != 0)
        {
            // Cancelボタンが押された、保存せずに終了する
            MEMFREE_PTR(buffer);
            return;
        }
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

    // 編集中文書の名前欄を(Palmデバイス内の)ファイル(writeName.txt)に書き込む
    MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);

    // 編集中メッセージを取得する
    frm = FrmGetActiveForm();
    NNshWinGetFieldText(frm, FLDID_WRITE_NAME, buffer, (NNshGlobal->NNsiParam)->writeBufferSize);
    if (StrLen(buffer) == 0)
    {
        // 編集中文書がない場合は終了する
        goto FUNC_END;

    }

    ret = OpenFile_NNsh(FILE_WRITENAME,
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

    // 編集中文書のMail欄を(Palmデバイス内の)ファイル(writeMail.txt)に書き込む
    MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);

    // 編集中メッセージを取得する
    frm = FrmGetActiveForm();
    NNshWinGetFieldText(frm, FLDID_WRITE_EMAIL, buffer, (NNshGlobal->NNsiParam)->writeBufferSize);
    if (StrLen(buffer) == 0)
    {
        // 編集中文書がない場合は終了する
        goto FUNC_END;

    }
    ret = OpenFile_NNsh(FILE_WRITEMAIL,
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
    MEMFREE_PTR(buffer);
    return;
}

/*=========================================================================*/
/*   Function : Handler_WriteConfigulation                                 */
/*                                                       イベントハンドラ  */
/*=========================================================================*/
Boolean Handler_WriteConfigulation(EventType *event)
{
    // 現在のフォームを取得
    switch (event->eType)
    {
      case ctlRepeatEvent:
        // リピートボタンを押したときの処理...
        // command = event->data.ctlRepeat.controlID;
        break;

      case ctlSelectEvent:
        // ボタンを押したときの処理...
        // command = event->data.ctlSelect.controlID;
        break;

      case keyDownEvent:
        // キーコードによって処理を分岐させる
        switch (KeyConvertFiveWayToJogChara(event))
        {
          default:
            // 上記以外のキーでは何もしない
            return (false);
            break;
        }
        break;

      case menuEvent:
      default: 
        // 何もしない
        return (false);
        break;
    }

    return (false);
}


/*-------------------------------------------------------------------------*/
/*   Function : openWriteConfigulation                                     */
/*                                          書き込み用設定項目の表示と設定 */
/*-------------------------------------------------------------------------*/
static void openWriteConfigulation(void)
{
    Boolean       ret = false;
    UInt16        btnId, sequenceType2, useBe2chLogin;
    FormType     *prevFrm, *diagFrm;

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // ダイアログ表示準備
    PrepareDialog_NNsh(FRMID_SETTING_WRITE_DIALOG, &diagFrm, false);
    if (diagFrm == NULL)
    {
        return;
    }
    HandEraMoveDialog(diagFrm);

    // 一度画面を描画する(大丈夫かな...)
    FrmDrawForm(diagFrm);

    // データの表示設定
    useBe2chLogin = (NNshGlobal->NNsiParam)->useBe2chInfo;
    sequenceType2 = (NNshGlobal->NNsiParam)->writeSequence2;
    SetControlValue(diagFrm, CHKID_USE_BE2chLOGIN,  &useBe2chLogin);
    SetControlValue(diagFrm, CHKID_WRITE_SEQUENCE2, &sequenceType2);

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_WriteConfigulation);

    // ダイアログの表示
    btnId = FrmDoDialog(diagFrm);
    
    // ダイアログの表示データをフォームから取得する
    useBe2chLogin = 0;
    sequenceType2 = 0;
    UpdateParameter(diagFrm, CHKID_USE_BE2chLOGIN,  &useBe2chLogin);
    UpdateParameter(diagFrm, CHKID_WRITE_SEQUENCE2, &sequenceType2);

    // ダイアログ表示の後片付け
    PrologueDialog_NNsh(prevFrm, diagFrm, false);

    // コマンド
    switch (btnId)
    {
      case BTNID_LOGIN_OYSTER:
#ifdef USE_SSL
        // '●'ログイン処理を実施...
        ProceedOysterLogin();
#else
        // "現在サポートしていません" 表示を行う
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif        
        break;
      
      case BTNID_LOGIN_Be2ch:
        // Be＠2chログイン処理を実施...
        ProceedBe2chLogin();
        break;

      case BTNID_DIALOG_OK:
        // OKボタンが押されたとき...(データを反映させる)
        (NNshGlobal->NNsiParam)->useBe2chInfo   = useBe2chLogin;
        (NNshGlobal->NNsiParam)->writeSequence2 = sequenceType2;
        break;      

      case BTNID_DIALOG_CANCEL:
      default:
        // Cancelボタンが押されたとき...(何もしない)
        break;
    }
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
        SaveWritingMessage();
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

      case MNUID_NET_CONFIG:
        // キーA (ネットワーク設定)
        // ネットワーク設定を開く
        // NNshGlobal->work1 = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenNetworkPreferences();
        break;

#if 0
      case MNUID_BT_ON:
        // Bluetooth ON
#ifdef USE_BT_CONTROL
        NNshGlobal->work1 = (NNshGlobal->NNsiParam)->lastFrmID;
        NNsiParam->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenBluetoothPreferences(true);
        ret = true;
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MNUID_BT_OFF:
        // Bluetooth OFF
#ifdef USE_BT_CONTROL
        (NNshGlobal->NNsiParam)->backupFormId = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenBluetoothPreferences(true);
        ret = true;
#endif  // #ifdef USE_BT_CONTROL
        break;
#endif

      case MNUID_SHOW_DEVICEINFO:
        // バージョン情報の表示
        ShowDeviceInfo_NNsh();
        break;

      case MNUID_PREVIEW:
        // 書き込み内容のプレビュー
        previewWritingForm_NNsh();
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : previewWritingForm_NNsh                                    */
/*                                書き込みデータのプレビュー機能(a2DA使用) */
/*-------------------------------------------------------------------------*/
static void previewWritingForm_NNsh(void)
{
    UInt16       ret;
    UInt32       size;
    Char         logMsg[BUFSIZE * 2], *txtP;
    FormType    *frm;
    FieldType   *fld;
    MemHandle    txtH;
    NNshFileRef  fileRef;

    // フィールドから、データを出力
    frm = FrmGetActiveForm();
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
    txtH    = FldGetTextHandle(fld);
    if (txtH == 0)
    {
        // フィールドのファイルハンドル取得失敗、、、終了する
        return;
    }
    txtP = MemHandleLock(txtH);
    if (StrLen(txtP) == 0)
    {
        MemHandleUnlock(txtH);
        return;
    }

    // ファイルにデータを出力する
    ret = OpenFile_NNsh(FILE_SENDMSG,
                        (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret == errNone)
    {
        (void) WriteFile_NNsh(&fileRef, 0, StrLen(txtP), txtP, &size);
        CloseFile_NNsh(&fileRef);
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
    }
    MemHandleUnlock(txtH);

    // クリップボードに、a2DAに渡すデータについての指示を格納する。
    MemSet (logMsg, sizeof(logMsg), 0x00);
    StrCopy(logMsg, A2DA_NNSIEXT_AADATASTART);
    StrCat (logMsg, A2DA_NNSIEXT_NNSICREATOR);
    StrCat (logMsg, A2DA_NNSIEXT_SRC_STREAM);
    StrCat (logMsg, A2DA_NNSIEXT_DATANAME);
    StrCat (logMsg, FILE_SENDMSG);
    StrCat (logMsg, A2DA_NNSIEXT_ENDDATANAME);
    StrCat (logMsg, A2DA_NNSIEXT_ENDDATA);
    ClipboardAddItem(clipboardText, logMsg, (StrLen(logMsg) + 1));

    // a2DAを起動する
    (void) LaunchResource_NNsh('DAcc','a2DA','code',1000);

    return;
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
        StrNCopy(data, (NNshGlobal->NNsiParam)->handleName, BUFSIZE - 1);
        fieldId = FLDID_WRITE_NAME;
        chkBox  = &((NNshGlobal->NNsiParam)->useFixedHandle);
        break;

      case CHKID_SAGE:
      default:
        StrCopy(data, "sage");
        fieldId = FLDID_WRITE_EMAIL;
        chkBox  = &((NNshGlobal->NNsiParam)->writeAlwaysSage);
        break;
    }

    // チェックボックスにチェックがつけられていた場合
    if (CtlGetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,chkId))) == 0)
    {
        // データクリア
        MemSet(data, BUFSIZE, 0x00);
        *chkBox = 0;
    }
    else
    {
        // データ設定
        *chkBox = 1;
    }
    NNshWinSetFieldText(frm, fieldId, true, data, BUFSIZE);
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
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_MSGCLEAR, "", 0) == 0)
        {
            NNshWinSetFieldText(FrmGetActiveForm(), FLDID_WRITE_MESSAGE,
                                true, "", (NNshGlobal->NNsiParam)->writeBufferSize);
        }
        break;

        // Cancelボタン ほか(スレ参照画面に戻る)
      case BTNID_WRITE_CANCEL:
        // 編集中メッセージをバックアップ
        SaveWritingMessage();
        returnToMessageView();
        break;

        // 'AA...' ボタンが押されたとき
      case BTNID_WRITE_AA:
        // AADAを起動する
        LaunchResource_NNsh('DAcc','moAA','code',1000);
        break;

        // 名前欄/Email欄横のチェックが更新されたとき
      case CHKID_HANDLENAME:
      case CHKID_SAGE:
        checkCheckBoxUpdate(event->data.ctlSelect.controlID);
        break;

      case SELID_SETTING:
        // 右上のセレクタトリガが押された...
        openWriteConfigulation();
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

    //tungsten T 5way navigator (281さん、感謝！)
    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // 「下」(ジョグダイヤル下)を押した時の処理
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
        NNshWinViewPageScroll(FLDID_WRITE_MESSAGE,
                              SCLID_WRITE_MESSAGE, 0, winDown); 
        ret = true;
        break;

      // 「上」(ジョグダイヤル上)を押した時の処理
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
        NNshWinViewPageScroll(FLDID_WRITE_MESSAGE,
                              SCLID_WRITE_MESSAGE, 0, winUp); 
        ret = true;
        break;

      // CLIEでCtrl+Enter もしくは Jogを押したとき
      case vchrJogPush:
        if ((NNshGlobal->NNsiParam)->writeJogPushDisable != 0)
        {
            // 書き込み時PUSHボタンを無効がONなら無効にする
            break;
        }
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

#if 0
      case chrCapital_A:
      case chrSmall_A:
        // キーA (ネットワーク設定)
        // ネットワーク設定を開く
        (NNshGlobal->NNsiParam)->backupFormId = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenNetworkPreferences();
        ret = true;
        break;

      case chrCapital_Y:
      case chrSmall_Y:
        // Bluetooth ON
#ifdef USE_BT_CONTROL
        (NNshGlobal->NNsiParam)->backupFormId = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenBluetoothPreferences(true);
        ret = true;
#endif  // #ifdef USE_BT_CONTROL
        break;

      case chrCapital_I:
      case chrSmall_I:
        // Bluetooth OFF
#ifdef USE_BT_CONTROL
        (NNshGlobal->NNsiParam)->backupFormId = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenBluetoothPreferences(true);
        ret = true;
#endif  // #ifdef USE_BT_CONTROL
        break;

#endif

      // その他のボタン群
      case vchrJogPushedUp:
      case vchrJogPushedDown:
      case chrLeftArrow:
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
      case chrRightArrow:
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
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
#if defined(USE_HANDERA) || defined(USE_PIN_DIA)
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
#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                FrmDrawForm(frm);        
            }
        }
        break;
#endif

      case fldHeightChangedEvent:
      case fldEnterEvent:
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
    Err                  ret, subRet, dbRet;
    Char                 msg[BUFSIZE], *buffer;
    Char                 title[NNSH_WRITELBL_MAX + 1];
    UInt32               fileSize, readSize;
    UInt16               alwaysSage, useFixedHandle, titleLen;
    NNshFileRef          fileRef;
    NNshSubjectDatabase  subjDB;
    FieldAttrType        attr;
    FieldType           *fldP;

    // （Bt切り替えやネット設定により）一時的に画面を閉じていた場合には、それを戻す
    if ((NNshGlobal->NNsiParam)->lastFrmID == NNSH_FRMID_WRITE_MESSAGE)
    {
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_MESSAGE;
    }

    // フォームタイトルの設定
    MemSet (title, sizeof(title), 0x00);
    StrCopy(title, NNSH_WRITELBL_HEAD);
    dbRet = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
    if (dbRet == errNone)
    {
        // 書き込み先スレ名をフォームタイトルに設定する
        StrCat(title, ":");
        titleLen = (sizeof(NNSH_WRITELBL_HEAD)); // ":"を追加したのでsizeof()
        StrNCopy(&title[titleLen],
                 subjDB.threadTitle,
                 (NNSH_WRITELBL_MAX - titleLen));
    }
    FrmCopyTitle(frm, title);

    alwaysSage     = (NNshGlobal->NNsiParam)->writeAlwaysSage;
    useFixedHandle = (NNshGlobal->NNsiParam)->useFixedHandle;

    // AADAがインストールされているか確認する
    if (CheckInstalledResource_NNsh('DAcc', 'moAA') == false)
    {
        // AADAがインストールされていない場合には、AA挿入ボタンを消去する
        CtlSetUsable(FrmGetObjectPtr(frm, 
                               FrmGetObjectIndex(frm, BTNID_WRITE_AA)), false);
    }

    // チェックボックス設定(ハンドル欄)
    CtlSetValue(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,CHKID_HANDLENAME)),
                useFixedHandle);

    // チェックボックス設定(E-Mail欄)
    CtlSetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,CHKID_SAGE)),
                alwaysSage);

    // アンダーラインの表示設定
    MemSet(&attr, sizeof(FieldAttrType), 0x00);
    fldP  = FrmGetObjectPtr(frm, 
                            FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
    if ((NNshGlobal->NNsiParam)->disableUnderlineWrite != 0)
    {
        FldGetAttributes(fldP, &attr);
        attr.underlined = noUnderline;
        FldSetAttributes(fldP, &attr);
    }

    // FILE_WRITEMSGが存在するかチェックする
    fileSize = 0;
    ret = OpenFile_NNsh(FILE_WRITEMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        goto SET_FIELD;
    }

    // 受信メッセージサイズを取得
    GetFileSize_NNsh(&fileRef, &fileSize);

    if (fileSize != 0)
    {
        // 一時バッファ領域を確保
        buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        if (buffer != NULL)
        {
            MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
            // ファイルからデータを読み出す
            ret = ReadFile_NNsh(&fileRef, 0, (NNshGlobal->NNsiParam)->writeBufferSize,
                                buffer, &readSize);
            if ((ret == errNone)||(ret == fileErrEOF))
            {
                NNshWinSetFieldText(frm, FLDID_WRITE_MESSAGE, false,
                                    buffer, (NNshGlobal->NNsiParam)->writeBufferSize);
                ret = errNone;
            }
            MEMFREE_PTR(buffer);
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

    ///// NAME欄を復旧
    MemSet(msg, sizeof(msg), 0x00);
    subRet = OpenFile_NNsh(FILE_WRITENAME,
                           (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                           &fileRef);
    if (subRet == errNone)
    {
        subRet = ReadFile_NNsh(&fileRef, 0, sizeof(msg), msg, &readSize);
        if ((subRet == errNone)||(subRet == fileErrEOF))
        {
            NNshWinSetFieldText(frm, FLDID_WRITE_NAME, true,msg, BUFSIZE);
        }
    }
    CloseFile_NNsh(&fileRef);

    ///// E-MAIL欄を復旧
    MemSet(msg, sizeof(msg), 0x00);
    subRet = OpenFile_NNsh(FILE_WRITEMAIL,
                           (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                           &fileRef);
    if (subRet == errNone)
    {
        subRet = ReadFile_NNsh(&fileRef, 0, sizeof(msg), msg, &readSize);
        if ((subRet == errNone)||(subRet == fileErrEOF))
        {
            NNshWinSetFieldText(frm, FLDID_WRITE_EMAIL, true,msg, BUFSIZE);
            alwaysSage = 0;
        }
    }
    CloseFile_NNsh(&fileRef);

SET_FIELD:
    // 保存してあったファイルを削除する
    (void) DeleteFile_NNsh(FILE_WRITEMSG, NNSH_VFS_DISABLE);
    (void) DeleteFile_NNsh(FILE_WRITENAME,NNSH_VFS_DISABLE);
    (void) DeleteFile_NNsh(FILE_WRITEMAIL,NNSH_VFS_DISABLE);

    // 常にsage書き込み(パラメータ設定時)
    if (alwaysSage != 0)
    { 
        NNshWinSetFieldText(frm, FLDID_WRITE_EMAIL, true,
                            "sage", BUFSIZE);
    }

    // コテハン機能を使用する(パラメータ設定時)
    if ((useFixedHandle != 0)&&((NNshGlobal->NNsiParam)->handleName[0] != '\0'))
    {
        NNshWinSetFieldText(frm, FLDID_WRITE_NAME, true, 
                            (NNshGlobal->NNsiParam)->handleName,
                            BUFSIZE);
    }

    // レス番号をフィールドの先頭に追加する。
    if ((ret != errNone)&&((NNshGlobal->NNsiParam)->insertReplyNum != 0)&&(dbRet == errNone))
    {
        MemSet(msg, sizeof(msg), 0x00);
        StrCopy(msg, ">>");
        NUMCATI(msg, subjDB.currentLoc);
        StrCat (msg, "\n");
        NNshWinSetFieldText(frm, FLDID_WRITE_MESSAGE,false, msg,
                                                   (NNshGlobal->NNsiParam)->writeBufferSize);
    }
    // フィールドの最大サイズを設定する
    FldSetMaxChars(fldP, (NNshGlobal->NNsiParam)->writeBufferSize);

    // フォーカスをメッセージフィールドに設定(パラメータが設定されてない場合)
    if ((NNshGlobal->NNsiParam)->notAutoFocus == 0)
    {
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
        FldSetInsPtPosition(fldP, (NNshGlobal->NNsiParam)->writeBufferSize);

        // FEPをONにする(PalmOS 4.0以上の場合)
        if (NNshGlobal->palmOSVersion > 0x04000000)
        {
            (void) TsmSetFepMode(NULL, tsmFepModeDefault);
        }
    }    

    // スクロールバーを更新
    NNshWinViewUpdateScrollBar(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkWriteReply                                          */
/*                                                                         */
/*             書き込み後の応答確認(設定要求Cookie or 書き込み結果 を抽出) */
/*-------------------------------------------------------------------------*/
static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command, UInt16 bbsType, Err *ret)
{
     NNshFileRef  fileRef;
     UInt32       readSize;
     Char         *start, *end, *ptr, *buf;

    // 受信メッセージを開いて、サーバからの応答を読み出す
    MemSet(buffer, bufSize, 0x00);
    *ret = OpenFile_NNsh(FILE_RECVMSG,
                         (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                         &fileRef);
    if (*ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Open HTTP Reply Message ", "", *ret);
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

        if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
            (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
            (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)||
            (bbsType == NNSH_BBSTYPE_2ch_EUC))
        {
            // EUCをSJに変換
            buf = MEMALLOC_PTR((StrLen(start) + MARGIN) * 2);
            if (buf != NULL)
            {
                MemSet(buf, ((StrLen(start) + MARGIN) * 2), 0x00);
                StrCopySJ(buf, start);
                StrCopy(start, buf);
            }
        }
        else
        {
            buf = NULL;
        }
        // タイトルがエラーかどうか確認する
        ptr = StrStr(start, NNSH_SYMBOL_WRITE_NG);
        if (ptr != NULL)
        {
            // エラー設定
            *ret = ~errNone;
            
            // エラーが発生していた、詳細情報を探る(本文内から文字列を取得)
            ptr = StrStr((end + 1), NNSH_SYMBOL_WRITE_NG);
            if (ptr != NULL)
            {
                // 応答する文字列の先頭を変更する
                start = ptr;

                // 文字列の最後を取得する
                end = StrStr(start, "</");
                if (end != NULL)
                {
                    *end = '\0';
                }

                if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
                    (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
                    (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)||
                    (bbsType == NNSH_BBSTYPE_2ch_EUC))
                {
                    // EUCをSJに変換
                    if (buf != NULL)
                    {
                        MEMFREE_PTR(buf);
                    }
                    buf = MEMALLOC_PTR((StrLen(start) + MARGIN) * 2);
                    if (buf != NULL)
                    {
                        MemSet(buf, ((StrLen(start) + MARGIN) * 2), 0x00);
                        StrCopySJ(buf, start);
                        StrCopy(start, buf);
                    }
                }
            }
            else
            {
                // 本文の解析...本文の先頭を探す
                ptr = end + StrLen("</title>");
                while (*ptr != '\0')
                {
                    // タグデータは読み飛ばす
                    if (*ptr == '<')
                    {
                        ptr++;
                        while (*ptr != '>')
                        {
                            if (*ptr == '\0')
                            {
                                // 文字列が終了したら抜ける
                                break;
                            }
                            ptr++;
                        }
                        ptr++;
                        continue;
                    }
                    // 改行コードも無視
                    if (*ptr == '\x0a')
                    {
                        ptr++;
                        continue;
                    }
                    break;
                }
                // 先頭データが見つかった場合には、先頭をその文字列の場所にする
                if (*ptr != '\0')
                {
                    start = ptr;
                }
                // 文字列の末尾を(てきとーに)探す。
                while ((ptr < buffer + bufSize)&&(*ptr != '<'))
                {
                    ptr++;
                }
                *ptr = '\0';                                
            }
        }
        if (buf != NULL)
        {
            MEMFREE_PTR(buf);
        }
    }
    else
    {
        // 書き込み指示 Cookieの位置を探す
        start = pickupCookie(ret, buffer, bufSize);
        NNsh_DebugMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, start, 0);
    }
    return (start);
}

/*-------------------------------------------------------------------------*/
/*   Function : pickupCookie                                               */
/*                                                   Cookieを切り出しする  */
/*-------------------------------------------------------------------------*/
static Char *pickupCookie(Err *ret, Char *start, UInt32 bufSize)
{
    Char *ptr, *cpyPtr, *top, *mid;

    top = start;
    ptr = StrStr(top, "Set-Cookie: ");
    if (ptr == NULL)
    {
        // Cookieが見つからなかった...終了する。
        *ret = ~errNone;
        return (NULL);        
    }
    top = ptr + StrLen("Set-");
    
    // 書き込み指示Cookieの最後尾を探す
    cpyPtr = top;
    while ((cpyPtr <= start + bufSize)&&(*cpyPtr != ';'))
    {
        cpyPtr++;
    }
    mid = cpyPtr;
    while ((ptr = StrStr((mid + 1), "Set-Cookie: ")) != NULL)
    {
        *cpyPtr = ';';
        cpyPtr++;
        *cpyPtr = ' ';
        cpyPtr++;
        ptr = ptr + StrLen("Set-Cookie: ");
        while ((ptr <= start + bufSize)&&(*ptr != ';'))
        {
            *cpyPtr = *ptr;
            cpyPtr++;
            ptr++;
        }
        mid = ptr;
    }
    *cpyPtr = '\0';
    return (top);
}

/*-------------------------------------------------------------------------*/
/*   Function : threadWrite_Message                                        */
/*                                                           書き込み処理  */
/*-------------------------------------------------------------------------*/
static Err threadWrite_Message(void)
{
    UInt32               dateTime;
    UInt16               index, subBufSize;
#ifdef PREVENT_DUPLICATE_POST
    UInt16               nofRetry;
#endif  // #ifdef PREVENT_DUPLICATE_POST
    NNshSubjectDatabase  subjDB;
    NNshBoardDatabase    bbsData;
    Err                  ret;
    Char                *subBuf, msgID[MAX_THREADFILENAME];
    Char                 spId[BUFSIZE], *param;
    Char                *url, *buffer, *ptr, *start;
    FormType            *frm;

    // (サブ)テンポラリ領域を確保
    subBufSize = 3 * BIGBUF;
    subBuf = MEMALLOC_PTR(subBufSize + MARGIN);
    if (subBuf == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR, "SUB AREA MEMALLOC_PTR()",
                           " size:", subBufSize + MARGIN);
        return (~errNone);
    }

    param = MEMALLOC_PTR(BIGBUF + BUFSIZE + MARGIN);
    if (param == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR, "SUB AREA2 MEMALLOC_PTR()",
                           " size:", (BIGBUF + MARGIN));
        MEMFREE_PTR(subBuf);
        return (~errNone);
    }
    MemSet(param, (BIGBUF + BUFSIZE + MARGIN), 0x00);

#ifdef PREVENT_DUPLICATE_POST
    // 二重投稿防止対策(暫定)
    nofRetry = (NNshGlobal->NNsiParam)->nofRetry;
    (NNshGlobal->NNsiParam)->nofRetry = 0;
#endif // #ifdef PREVENT_DUPLICATE_POST

    // メッセージ情報をデータベースから取得
    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_INFO, "get_subject_database()", "", ret);
        goto FUNC_END;
    }

    // BBS情報をデータベースから取得
    ret = Get_BBS_Database(subjDB.boardNick, &bbsData, &index);

    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          subjDB.boardNick, ret);
        goto FUNC_END;
    }

    // 書き込みするかを確認するメッセージ
    if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_WRITEMSG,
                            subjDB.threadTitle, 0) != 0)
    {
        ret = ~errNone;
        goto FUNC_END;
    }

    // データ送信用ワークバッファの獲得
    buffer = (Char *) MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
    if (buffer == NULL)
    {
        // 領域の確保失敗！
        NNsh_InformMessage(ALTID_ERROR, "SEND AREA MEMALLOC_PTR()",
                           " size:", (NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        goto FUNC_END;
    }
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    MemSet (subBuf, (subBufSize + MARGIN), 0x00);
    StrCopy(subBuf, subjDB.boardNick);
    subBuf[StrLen(subjDB.boardNick) - 1] = '\0';

    MemSet (msgID, sizeof(msgID), 0x00);
    StrCopy(msgID, subjDB.threadFileName);
    ptr = StrStr (msgID, ".");
    if (ptr != NULL)
    {
        *ptr = '\0';
    }

    // 送信するメッセージをsend.txtへ出力する
    frm = FrmGetActiveForm();
    create_outputMessageFile(frm, buffer,
                             ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN),
                             subBuf, msgID, (UInt16) subjDB.bbsType);

    //　送信先URL(というよりリファラ)と時刻の作成
    MemSet(subBuf, (subBufSize + MARGIN), 0x00);
    MemSet(spId,   sizeof(spId),   0x00);
    url = spId;
    StrCopy(url, bbsData.boardURL);
    switch (subjDB.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // まちBBSの場合
        StrCat(url, URL_PREFIX_MACHIBBS);
        StrCat(url, bbsData.boardNick);
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        }
        StrCat(url, "&KEY=");
        StrCat(url, msgID); 

        // BBS名と現在時刻、ボタンの出力
        setBBSNameAndTime(subjDB.bbsType,bbsData.boardNick,subBuf,subBufSize);
        StrCat(subBuf, "&submit=%8F%91%82%AB%8D%9E%82%DE");
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // したらば＠ＪＢＢＳのとき
        // 書き込み先URLを新仕様にあわせて変更
        //  "http://jbbs.shitaraba.com/bbs/write.cgi/(category)/(nick)/(key)/"
        StrCopy(url, "http://jbbs.livedoor.jp/bbs/write.cgi");

        // URLからカテゴリ名を抽出する( '/computer/' みたいなところ)
        StrCopy(param, bbsData.boardURL);
        start = &param[StrLen(param) - 1];
        if (*start == '/')
        {
            *start = '\0';
        }
        while ((param < start)&&(*start != '/'))
        {
            start--;
        }
        StrCat (url, start);
        StrCat (url, "/");

        // nickを追加 (たとえば '351/')
        StrCat (url, bbsData.boardNick);

        // スレ番号を追加 ("1015506694/" とか)
        StrCat (url, msgID); 
        StrCat (url, "/");

        // BBS名と現在時刻、ボタンの出力
        setBBSNameAndTime(subjDB.bbsType,bbsData.boardNick,subBuf,subBufSize);
        //StrCat(subBuf, "&KEY=");
        //StrCat(subBuf, msgID);
        start++;
        StrCat(subBuf, "&DIR=");
        StrCat(subBuf, start);
        StrCat(subBuf, "&submit=%BD%F1%A4%AD%B9%FE%A4%E0");
        break;


      case NNSH_BBSTYPE_SHITARABA:
        // したらばのとき 
        StrCopy(url, "http://www.shitaraba.com/");
        StrCat (url, URL_PREFIX_SHITARABA);
        StrCat (url, bbsData.boardNick);
        // 末尾に '/' がついていたら外す。
        if (buffer[StrLen(url) - 1] == '/')
        {
            buffer[StrLen(url) - 1] = '\0';
        }
        StrCat (url, "&key=");
        StrCat (url, msgID);

        // BBS名、ボタンの出力(なんか変...)
        MemSet(subBuf, subBufSize, 0x00);
        StrCat (subBuf, "&bbs=");
        StrCat (subBuf, bbsData.boardNick);
        // 末尾に '/' がついていたら外す。
        if (subBuf[StrLen(subBuf) - 1] == '/')
        {
            subBuf[StrLen(subBuf) - 1] = '\0';
        }
        StrCat(subBuf, "&submit=%A4%AB%A4%AD%A4%B3%A4%81");
        break;

      case NNSH_BBSTYPE_OTHERBBS:
        // 2ch互換BBSの場合は、BBS名と現在時刻、ボタンを生成して出力
        setBBSNameAndTime(subjDB.bbsType, bbsData.boardNick,
                          subBuf, subBufSize);
        StrCat(subBuf, "&submit=%8F%91%82%AB%8D%9E%82%DE");
        break;

      case NNSH_BBSTYPE_OTHERBBS_2:
        // URLからBBS名を抽出する
        start = &bbsData.boardURL[StrLen(bbsData.boardURL) - 1];
        if (*start == '/')
        {
            start--;
        }
        while ((bbsData.boardURL < start)&&(*start != '/'))
        {
            start--;
        }
        start++;
        MemSet (param, (BIGBUF + BUFSIZE + MARGIN), 0x00);
        StrCopy(param, start);
        
        // 2ch互換BBSの場合、BBS名は、板URLの末尾から取得する
        // (BBS名、現在時刻、ボタンを生成して出力)
        setBBSNameAndTime(subjDB.bbsType, param,
                          subBuf, subBufSize);
        StrCat(subBuf, "&submit=%8F%91%82%AB%8D%9E%82%DE");
        break;

      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      default:
        // 2chの場合
        StrCopy(&url[StrLen(url) - StrLen(bbsData.boardNick)],  URL_PREFIX);
        StrCat (url, bbsData.boardNick);
        StrCat (url, msgID);
        StrCat (url,  "/");

        // 送信シーケンス２のとき、フォームのパラメータを一度取得し追加する。
        if ((NNshGlobal->NNsiParam)->writeSequence2 != 0)
        {
            // 書き込みフォームを取得する
            MemSet (param, (BIGBUF + BUFSIZE + MARGIN), 0x00);
            StrCopy(param, url);
            StrCat (param, "1n");
            NNsh_DebugMessage  (ALTID_INFO, "Get Param. (URL)", param, 0);
            ret = NNshHttp_comm(HTTP_SENDTYPE_GET_NOTMONA, param,
                                NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                                NULL);
            if (ret != errNone)
            {
                // (タイムアウト以外の)通信エラー発生
                if (ret != netErrTimeout)
                {
                    NNsh_InformMessage(ALTID_ERROR,
                                       MSG_ERROR_HTTP_COMM, "[FORM]", ret);
                }
                else
                {
                    // 通信タイムアウト発生
                    NNsh_InformMessage(ALTID_ERROR,
                                       MSG_OCCUR_TIMEOUT, "[FORM]", ret);
                }
            }

            // 受信したフォームから、パラメータ部分を切り出す
            MemSet(subBuf, subBufSize, 0x00);
            setFormParameters(bbsData.boardNick,
                              buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                              subBuf, subBufSize, "%8F%91%82%AB%8D%9E%82%DE");
        }
        else
        {
            // BBS名と現在時刻、ボタンの出力
            setBBSNameAndTime(subjDB.bbsType, bbsData.boardNick,
                              subBuf, subBufSize);
            StrCat(subBuf, "&hana=mogera&submit=%8F%91%82%AB%8D%9E%82%DE");
        }
#ifdef USE_SSL
        // OysterのセッションIDを追記する
        if ((NNshGlobal->connectedSessionId != NULL)&&
            (*(NNshGlobal->connectedSessionId) != '\0')) 
        {
            StrCat(subBuf, "&sid=");
            StrCatURLencode(subBuf, NNshGlobal->connectedSessionId);
        }
#endif
        break;
    }
    MEMFREE_PTR(buffer);

    // 書き込みシーケンスタイプ２のときは、ちょっと待機する
    if ((NNshGlobal->NNsiParam)->writeSequence2 != 0)
    {
        // 「待機中」を表示
        Show_BusyForm(MSG_INFO_WRITEWAIT);

        SysTaskDelay(NNSH_WRITE_DELAYTIME * SysTicksPerSecond());

        Hide_BusyForm(false);
    }

    // メッセージを送信する
    NNsh_DebugMessage(ALTID_INFO, "Send(1) ", url, 0);
    NNsh_DebugMessage(ALTID_INFO, "Param:  ", subBuf, 0);
    switch (subjDB.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_MACHIBBS, url,
                            NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                            NULL);
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_SHITARABAJBBS, url,
                            NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                            NULL);
        break;

      case NNSH_BBSTYPE_SHITARABA:
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_SHITARABA, url,
                            NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                            NULL);
        break;

      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_OTHERBBS, url,
                            NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                            NULL);
        break;

      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      default:
        if ((NNshGlobal->NNsiParam)->writeSequence2 != 0)
        {
            ret = NNshHttp_comm(HTTP_SENDTYPE_POST_SEQUENCE2, url,
                                NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                                NULL);
        }
        else
        {
            ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url,
                                NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                                NULL);
        }
        break;
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
        goto FUNC_END;
    }

    /////////////////////////////////////////////////////////////////////////
    //   受信したCookieを使用して書き込みを行わない場合、またホストから
    // 「書きこみました。」と応答があった場合には、Cookieを使用した書きこみは
    // 行わないようにする
    // (まちBBSの場合もCookie使用の書き込みを行わないようにする)
    /////////////////////////////////////////////////////////////////////////

    // サーバからの応答を解析
    MemSet(subBuf, subBufSize + MARGIN, 0x00);
    start = checkWriteReply(subBuf, subBufSize, NNSH_WRITECHK_REPLY,
                            subjDB.bbsType, &ret);
    if (((NNshGlobal->NNsiParam)->useCookieWrite == 0)||
        (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (subjDB.bbsType == NNSH_BBSTYPE_SHITARABA)||
        (subjDB.bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (subjDB.bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)||
        (subjDB.bbsType == NNSH_BBSTYPE_OTHERBBS_2)||
        (StrCompare(start, NNSH_SYMBOL_WRITE_DONE) == 0))
    {
        // スレメッセージの再送信は行わない、差分取得チェックへ
        goto WRITE_NEXT;
    }

    // Cookieを使用して書きこみメッセージを再送信する
    NNsh_DebugMessage(ALTID_INFO, "PICK UP COOKIE...", "", 0);

    // データ解析用のバッファを確保する
    buffer = (Char *) MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
    if (buffer == NULL)
    {
        // 領域の確保失敗！
        NNsh_InformMessage(ALTID_ERROR, "ANALYSIS AREA MEMALLOC_PTR()",
                           " size:", (NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        ret = ~errNone;
        goto FUNC_END;
    }

    // 飛んできたフォームのパラメータデータを取得
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    MemSet (param, (BIGBUF + BUFSIZE + MARGIN), 0x00);
    StrCopy(param, "&hana=mogera");
    setFormParameters(bbsData.boardNick, buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                      param, (BIGBUF + BUFSIZE),
                      "%91S%90%D3%94C%82%F0%95"
                      "%89%82%A4%82%B1%82%C6%82%F0%8F%B3"
                      "%91%F8%82%B5%82%C4%8F%91%82%AB%8D"
                      "%9E%82%DE");

    // サーバからの応答を解析、SPIDを取得する
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    MemSet(spId, sizeof(spId), 0x00);
    ptr = checkWriteReply(buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                          NNSH_WRITECHK_SPID, subjDB.bbsType, &ret);
    if (ptr == NULL)
    {
        if ((NNshGlobal->NNsiParam)->writeSequence2 == 0)
        {
            // サーバからの応答が異常だった
            NNsh_ErrorMessage(ALTID_ERROR, MSG_WRITEREPLY_WRONG, url, 0);
            ret = ~errNone;
            goto FUNC_END;
        }
        // 書き込みシーケンスその２のときには、Cookie pick up を実行せずに次にすすむ...
        StrCopy(spId, "Cookie: a=b");
    }
    else
    {
        // SPIDを取得する
        while ((*ptr != 'C')&&(*ptr != '\0'))
        {
            ptr++;
        }
        StrNCopy(spId, ptr, sizeof(spId) - 1);
    }

    // paramに時刻が入っていたときは、-45した値に変換する
    ptr = StrStr(param, "&time=");
    if (ptr != NULL)
    {
        ptr      = ptr + StrLen("&time=");
        dateTime = StrAToI(ptr);
        dateTime = (dateTime > NNSH_WRITE_DELAYSECOND) ? (dateTime - NNSH_WRITE_DELAYSECOND) : 1;
        StrIToA(ptr, dateTime);

        NNsh_DebugMessage(ALTID_INFO, ptr, ":", dateTime);
    }

#ifdef USE_SSL
    // OysterのセッションIDを追記する
    if ((NNshGlobal->connectedSessionId != NULL)&&
        (*(NNshGlobal->connectedSessionId) != '\0')) 
    {
        StrCat(ptr, "&sid=");
        StrCatURLencode(ptr, NNshGlobal->connectedSessionId);
    }
#endif

    // 解析した領域を開放する
    MEMFREE_PTR(buffer);

    //　送信先URLの作成
    MemSet(subBuf, subBufSize + MARGIN, 0x00);
    url = subBuf;
    StrCopy(url, bbsData.boardURL);
    StrCopy(&url[StrLen(url) - StrLen(bbsData.boardNick)],  URL_PREFIX);
    StrCat (url, bbsData.boardNick);
    StrCat (url, msgID);
    StrCat (url,  "/");

    // (parameterとcookieを再設定して)メッセージを送信する
    NNsh_DebugMessage(ALTID_INFO, "Send(2) ", url, 0);

    // 書き込みシーケンスタイプ２のときは、ここでもちょっと待機する
    if ((NNshGlobal->NNsiParam)->writeSequence2 != 0)
    {
        // 「待機中」を表示
        Show_BusyForm(MSG_INFO_WRITEWAIT);

        SysTaskDelay(NNSH_WRITE_DELAYTIME * SysTicksPerSecond());

        Hide_BusyForm(false);

        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_SEQUENCE2, url, spId, param, 
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    }
    else
    {
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url, spId, param, 
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    }
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
        goto FUNC_END;
    }

    // サーバからの応答を解析
    MemSet(subBuf, (subBufSize + MARGIN), 0x00);
    start = checkWriteReply(subBuf, subBufSize, NNSH_WRITECHK_REPLY,
                            subjDB.bbsType, &ret);

WRITE_NEXT:

    // 書き込み先が差分取得できるかどうかチェックする
    MemSet (url, sizeof(url), 0x00);
    switch (subjDB.bbsType)
    {
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_MACHIBBS:
        //  まちBBS/したらば＠JBBSに書き込んだときは、スレ差分取得が
        // できないのでここで折り返す
        NNsh_InformMessage(ALTID_INFO, MSG_NOTSUPPORT_MACHI2, "", 0);

        ret = errNone;
        goto FUNC_END;
        break;

      case NNSH_BBSTYPE_SHITARABA:
        // したらばは、応答解析を行わない
        ret    = errNone;
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // したらば@JBBSは、応答解析を行わない
        ret    = errNone;
        //goto FUNC_END;     // なぜか差分取得がうまくいかない。。。
        break;

#if 0
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // 応答解析の結果を表示する
        if (ret == errNone)
        {
            // 正常に書き込まれたとき
            NNsh_InformMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, start, 0);
        }
        else
        {
            // 書き込みエラーのとき
            NNsh_InformMessage(ALTID_WARN, MSG_RECV_SERVERRESPONSE, start, 0);
        }
        break;
    }

    // メッセージ取得先URLの作成
    (void) CreateThreadURL(NNSH_DISABLE, url, (BUFSIZE), &bbsData, &subjDB);

    // スレ差分取得を行う
    (void) Get_PartMessage(url, bbsData.boardNick, &subjDB, 
                           (NNshGlobal->NNsiParam)->openMsgIndex, NULL);

FUNC_END:
#ifdef PREVENT_DUPLICATE_POST
    // 二重投稿防止対策
    (NNshGlobal->NNsiParam)->nofRetry = nofRetry;
#endif // #ifdef PREVENT_DUPLICATE_POST

    if (ret != errNone)
    {
        // 画面を再描画する
        NNsi_FrmDrawForm(FrmGetActiveForm(), true);
    }
    MEMFREE_PTR(param);
    MEMFREE_PTR(subBuf);
    return (ret);
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

    MemSet (buffer, bufSize, 0x00);
    if (key != NULL)
    {
        // key部分の出力
        if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
            (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
            (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
        {
            StrCopy(buffer, "KEY=");
        }
        else
        {
            StrCopy(buffer, "key=");
        }
        StrCat (buffer, key);
    }
    else
    {
        // keyが未指定の場合にはスレ立て(タイトル欄からデータ取得)    
        StrCopy(buffer, "&subject=");
        ptr = buffer;
        (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
        outputTextField(diagFrm, FLDID_NEWTHREAD_TITLE, buffer, bufSize, bbsType, &sendRef);
        MemSet (buffer, bufSize, 0x00);
    }

    // 名前欄の出力
    if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
    {
        StrCat (buffer, "&NAME=");
    }
    else
    {
        StrCat (buffer, "&FROM=");
    }
    ptr = buffer;
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    outputTextField(diagFrm, FLDID_WRITE_NAME, buffer, bufSize, bbsType, &sendRef);

    // E-Mail欄の出力
    if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
    {
        ptr = "&MAIL=";
    }
    else
    {
        ptr = "&mail=";
    }
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    outputTextField(diagFrm, FLDID_WRITE_EMAIL, buffer, bufSize, bbsType, &sendRef);

    // メッセージ欄の出力
    ptr = "&MESSAGE=";
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    if (key != NULL)
    {
        outputTextField(diagFrm, FLDID_WRITE_MESSAGE, buffer, bufSize, bbsType, &sendRef);
    }
    else
    {
        // スレ立ての場合
        outputTextField(diagFrm, FLDID_NEWTHREAD_MESSAGE, buffer, bufSize, bbsType, &sendRef);
    }
    
    // ファイルクローズ
    CloseFile_NNsh(&sendRef);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : outputTextField                                            */
/*                                                      テキスト領域の出力 */
/*-------------------------------------------------------------------------*/
static void outputTextField(FormType *frm, UInt16 fldID, Char *buf, UInt16 size, UInt16 bbsType, NNshFileRef *fileRef)
{
    Char   *dat;
    UInt32  dum;
    UInt16  len;

    NNshWinGetFieldText(frm, fldID, buf, size);
    len = StrLen(buf);
    if (len == 0)
    {
        // 値が記入されていなかった
        return;
    }

    if ((bbsType == NNSH_BBSTYPE_SHITARABA)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)||
        (bbsType == NNSH_BBSTYPE_2ch_EUC))
    {
        // したらば、したらば＠ＪＢＢＳのときは漢字コード変換
        dat = MEMALLOC_PTR(len * 2);
        if (dat == NULL)
        {
            // 領域確保に失敗したので、そのまま出力
            (void) AppendFileAsURLEncode_NNsh(fileRef, len, buf, &dum);
        }
        else
        {
            // EUC漢字コードに変換して出力
            MemSet(dat, (len * 2), 0x00);
            StrCopyEUC(dat, buf);
            (void) AppendFileAsURLEncode_NNsh(fileRef, StrLen(dat), dat, &dum);
                                                  
            MEMFREE_PTR(dat);
        }
    }
    else
    {
        // そのまま出力
        (void) AppendFileAsURLEncode_NNsh(fileRef, len, buf, &dum);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////
/////       スレ立て画面系の処理
//////////////////////////////////////////////////////////////////////////////

/*=========================================================================*/
/*   Function : OpenForm_newThread                                         */
/*                                                      スレ立て画面を開く */
/*=========================================================================*/
void OpenForm_newThread(FormType *frm)
{
#ifdef USE_NEWTHREAD_FEATURE
    Err                ret;
    UInt16             len, selBBS, alwaysSage, useFixedHandle;
    Char               title[NNSH_NEWTHREAD_MAX + MARGIN];
    NNshBoardDatabase  bbsData;

    // スレ立てする板名を取得する
    selBBS = (NNshGlobal->NNsiParam)->lastBBS;

    // BBS情報を取得する
    MemSet(&bbsData, sizeof(bbsData), 0x00);
    ret = Get_BBS_Info(selBBS, &bbsData);
    if (ret != errNone)
    {
        // BBS情報取得失敗、スレ一覧画面を開く
        NNsh_DebugMessage(ALTID_ERROR, MSG_NOTAVAIL_BBS, "(Get_BBS_Info())", ret);
        FrmGotoForm(NNshGlobal->backFormId);
        return;
    }

    // タイトルをコピーする
    MemSet (title, sizeof(title), 0x00);
    StrCopy(title, NNSH_NEWTHREAD_LBLHEAD);
    len = StrLen(title);
    StrNCopy(&title[len], bbsData.boardName, (NNSH_NEWTHREAD_MAX - len));
    FrmCopyTitle(frm, title);

    alwaysSage     = (NNshGlobal->NNsiParam)->writeAlwaysSage;
    useFixedHandle = (NNshGlobal->NNsiParam)->useFixedHandle;

    // チェックボックス設定(ハンドル欄)
    CtlSetValue(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,CHKID_HANDLENAME)), useFixedHandle);
    checkCheckBoxUpdate(CHKID_HANDLENAME);

    // チェックボックス設定(E-Mail欄)
    CtlSetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,CHKID_SAGE)), alwaysSage);
    checkCheckBoxUpdate(CHKID_SAGE);

    // スクロールバーを更新
    NNshWinViewUpdateScrollBar(FLDID_NEWTHREAD_MESSAGE, SCLID_NEWTHREAD_MESSAGE);

    // フォーカスをタイトルフィールドに設定(パラメータが設定されてない場合)
    if ((NNshGlobal->NNsiParam)->notAutoFocus == 0)
    {
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_NEWTHREAD_TITLE));
    }

    // FEPをONにする(PalmOS 4.0以上の場合)
    if (NNshGlobal->palmOSVersion > 0x04000000)
    {
        (void) TsmSetFepMode(NULL, tsmFepModeDefault);
    }

    // nilEvent発行
    EvtWakeup();

#endif
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   Handler_newThread                                        */
/*                                         スレ立て画面のイベントハンドラ  */
/*-------------------------------------------------------------------------*/
Boolean Handler_newThread(EventType *event)
{
#ifdef USE_NEWTHREAD_FEATURE

    Boolean   ret = false;
#if defined(USE_HANDERA) || defined(USE_PIN_DIA)
    FormType *frm;
#endif

    switch (event->eType)
    {
      case keyDownEvent:
        // キー入力
        selEvt_KeyDown_newThread(event);
        break;

      case ctlSelectEvent:
        // ボタン押下
        selEvt_Control_newThread(event);
        break;

      case fldChangedEvent:
        // フィールドに文字入力
       NNshWinViewUpdateScrollBar(FLDID_NEWTHREAD_MESSAGE, SCLID_NEWTHREAD_MESSAGE);
       break;

      case sclRepeatEvent:
        // スクロールバー更新
        sclRepEvt_newThread(event);
        break;

      case menuEvent:
        // メニュー選択
        NNsh_MenuEvt_Edit(event);
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent :
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
        break;
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                FrmDrawForm(frm);        
            }
        }
        break;
#endif

      case fldHeightChangedEvent:
      case fldEnterEvent:
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
#else
    return (false);
#endif
}

/////  スレ立て画面のサブ関数群
#ifdef USE_NEWTHREAD_FEATURE
/*-------------------------------------------------------------------------*/
/*   Function : sclRepEvt_newThread                                        */
/*                                       スクロールバー更新イベントの処理  */
/*-------------------------------------------------------------------------*/
static Boolean sclRepEvt_newThread(EventType *event)
{
    Int16   lines;

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
        (void) NNshWinViewPageScroll(FLDID_NEWTHREAD_MESSAGE,
                                      SCLID_NEWTHREAD_MESSAGE, -lines, winUp);
    }
    else if (lines > 0)
    {
        (void) NNshWinViewPageScroll(FLDID_NEWTHREAD_MESSAGE, 
                                      SCLID_NEWTHREAD_MESSAGE,lines, winDown);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selEvt_Control_newThread                                   */
/*                                           実行ボタンが押された時の処理  */
/*-------------------------------------------------------------------------*/
static Err execute_newThread(FormType *frm)
{ 
    Err                ret = ~errNone;
    UInt16             selBBS, subBufSize;
#ifdef PREVENT_DUPLICATE_POST
    UInt16               nofRetry;
#endif  // #ifdef PREVENT_DUPLICATE_POST
    NNshBoardDatabase  bbsData;
    Char              *buffer, *subBuf, *ptr;
    Char               url[MAX_URL + MARGIN], *spId;

    // 一時バッファ領域を確保
    spId = MEMALLOC_PTR(BIGBUF + BUFSIZE + MARGIN);
    if (spId == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR() ", " size:",
                          BIGBUF + BUFSIZE + MARGIN);
        return (~errNone);
    }
    MemSet(spId, (BIGBUF + BUFSIZE + MARGIN), 0x00);

    // 一時バッファ領域を確保
    buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR() ", " size:",
                          (NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        MEMFREE_PTR(spId);
        return (~errNone);
    }
    MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);

    // (サブ)テンポラリ領域を確保
    subBufSize = 3 * BIGBUF;
    subBuf = MEMALLOC_PTR(subBufSize + MARGIN);
    if (subBuf == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "SUB AREA MEMALLOC_PTR()", " size:", 
                          subBufSize + MARGIN);
        MEMFREE_PTR(spId);
        MEMFREE_PTR(buffer);
        return (~errNone);
    }
    MemSet(subBuf, (subBufSize + MARGIN), 0x00);

    // スレ立てする板名を取得する
    selBBS = (NNshGlobal->NNsiParam)->lastBBS;

#ifdef PREVENT_DUPLICATE_POST
    // 二重投稿防止対策(暫定)
    nofRetry = (NNshGlobal->NNsiParam)->nofRetry;
    (NNshGlobal->NNsiParam)->nofRetry = 0;
#endif // #ifdef PREVENT_DUPLICATE_POST

    // BBS情報を取得する
    MemSet(&bbsData, sizeof(bbsData), 0x00);
    ret = Get_BBS_Info(selBBS, &bbsData);
    if (ret != errNone)
    {
        // BBS情報取得失敗、スレ一覧画面を開く
        NNsh_InformMessage(ALTID_ERROR, MSG_NOTAVAIL_BBS, "(Get_BBS_Info())", ret);
        goto NOT_SUPPORT;
    }
 
    // subBufに板Nickを格納 
    StrCopy(subBuf, bbsData.boardNick);
    subBuf[StrLen(bbsData.boardNick) - 1] = '\0';
 
    // スレ立て用送信メッセージをフォームから取得
    create_outputMessageFile(frm, buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN),
                             subBuf, NULL, (UInt16) bbsData.bbsType);

    // BBS名と現在時刻、ボタンの出力
    setBBSNameAndTime(bbsData.bbsType, bbsData.boardNick,
                      buffer, (NNshGlobal->NNsiParam)->writeBufferSize);
    StrCat(buffer, "&hana=mogera&submit=%90V%8BK%83X%83%8C%83b%83h%8D%EC%90%AC%89%E6%96%CA%82%D6");

#ifdef USE_SSL
        // OysterのセッションIDを追記する
        if ((NNshGlobal->connectedSessionId != NULL)&&
            (*(NNshGlobal->connectedSessionId) != '\0')) 
        {
            StrCat(buffer, "&sid=");
            StrCatURLencode(buffer, NNshGlobal->connectedSessionId);
        }
#endif

    // 2chの板URLに問い合わせ...
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsData.boardURL);
    ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url, NULL, buffer, 
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    if (ret != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "[#1]", ret);
        }
        else
        {
            // 通信タイムアウト発生
            NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[#1]", ret);
        }

        // 画面を再描画する
        NNsi_FrmDrawForm(FrmGetActiveForm(), true);

        // 関数を抜ける
        goto NOT_SUPPORT;
    }

    // 飛んできたフォームのパラメータデータを取得
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    MemSet (subBuf, subBufSize, 0x00);
    StrCopy(subBuf, "&hana=mogera");
    setFormParameters(bbsData.boardNick, buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                      subBuf,  subBufSize,
                      "%91S%90%D3%94C%82%F0%95"
                      "%89%82%A4%82%B1%82%C6%82%F0%8F%B3"
                      "%91%F8%82%B5%82%C4%8F%91%82%AB%8D"
                      "%9E%82%DE");

    // サーバからの応答を解析、SPIDを取得する
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    ptr = checkWriteReply(buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                          NNSH_WRITECHK_SPID, bbsData.bbsType, &ret);
    if (ptr == NULL)
    {
        // サーバからの応答が異常だった
        NNsh_ErrorMessage(ALTID_ERROR, MSG_WRITEREPLY_WRONG, url, 0);
        ret = ~errNone;
        goto NOT_SUPPORT;
    }

    // SPIDを取得する
    MemSet(spId, (BIGBUF + BUFSIZE + MARGIN), 0x00);
    StrNCopy(spId, ptr, (BIGBUF + BUFSIZE));

#ifdef USE_SSL
    // OysterのセッションIDを追記する
    if ((NNshGlobal->connectedSessionId != NULL)&&
        (*(NNshGlobal->connectedSessionId) != '\0')) 
    {
        StrCat(spId, "&sid=");
        StrCatURLencode(spId, NNshGlobal->connectedSessionId);
    }
#endif

    // スレ立て実行(実処理)
    ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url, spId, subBuf, 
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    if (ret != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_ERROR_HTTP_COMM, "(#2)", ret);
        }
        else
        {
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_OCCUR_TIMEOUT, "(#2)", ret);
        }

        // 画面を再描画する
        NNsi_FrmDrawForm(FrmGetActiveForm(), true);

        goto NOT_SUPPORT;
    }

    // サーバからの応答を解析
    MemSet(subBuf, (subBufSize + MARGIN), 0x00);
    ptr = checkWriteReply(subBuf, subBufSize, NNSH_WRITECHK_REPLY, bbsData.bbsType, &ret);

    // 応答解析の結果を表示する
    if (ret == errNone)
    {
        // 正常にスレ立てできたとき
        NNsh_InformMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, ptr, 0);
    }
    else
    {
        // スレ立てに失敗したとき
        NNsh_InformMessage(ALTID_WARN, MSG_RECV_SERVERRESPONSE, ptr, 0);
    }

NOT_SUPPORT:
#ifdef PREVENT_DUPLICATE_POST
    // 二重投稿防止対策
    (NNshGlobal->NNsiParam)->nofRetry = nofRetry;
#endif // #ifdef PREVENT_DUPLICATE_POST

    // 領域を解放し、終了する
    MEMFREE_PTR(subBuf);
    MEMFREE_PTR(buffer);
    MEMFREE_PTR(spId);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : selEvt_Control_newThread                                   */
/*                                               ボタンが押された時の処理  */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_Control_newThread(EventType *event)
{
    Err       ret;
    FormType *frm;
    
    frm = FrmGetActiveForm();
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_NEWTHREAD_EXECUTE:
        // "実行ボタン"が押されたとき（スレ立てを実行）
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_NEWTHREAD, "", 0) == 0)
        {
            // スレ立てを実行する
            ret = execute_newThread(frm);
            if (ret != errNone)
            {
                // 書き込み失敗(接続失敗)時、スレ立て画面に戻る。
                return (false);
                break;
            }
            // 画面を再描画し、一覧画面に戻る
            FrmEraseForm(frm);
            FrmGotoForm(NNshGlobal->backFormId);
            return (true);
        }
        break;

      case BTNID_NEWTHREAD_CLEAR:
        // Clearボタン(タイトルおよび書き込み領域をクリアする)
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_MSGCLEAR, "", 0) == 0)
        {
            // メッセージ領域をクリア
            NNshWinSetFieldText(frm, FLDID_NEWTHREAD_MESSAGE, true, "", 
                                (NNshGlobal->NNsiParam)->writeBufferSize);

            // タイトルをクリア
            NNshWinSetFieldText(frm, FLDID_NEWTHREAD_TITLE, true, "", 
                                MAX_THREADNAME);

            // スクロールバーを更新
            NNshWinViewUpdateScrollBar(FLDID_NEWTHREAD_MESSAGE,
                                       SCLID_NEWTHREAD_MESSAGE);
        }
        break;

      case BTNID_NEWTHREAD_CANCEL:
        // Cancelボタン(一覧画面に戻る)
        FrmEraseForm(frm);
        FrmGotoForm(NNshGlobal->backFormId);
        break;

      case SELID_SETTING:
        // 右上のセレクタトリガが押された...
        openWriteConfigulation();
        break;

      case CHKID_HANDLENAME:
      case CHKID_SAGE:
        // 名前欄/Email欄横のチェックが更新されたとき
        checkCheckBoxUpdate(event->data.ctlSelect.controlID);
        break;

      default:
        // これら以外(なにもしない)
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectKeyDownEvent                                         */
/*                                                 キーが押された時の処理  */
/*                                (ジョグアシストＯＦＦ時のジョグ処理追加) */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_KeyDown_newThread(EventType *event)
{
    Boolean   ret = false;
    FormType *frm;
    UInt16    keyCode;

    //tungsten T 5way navigator (281さん、感謝！)
    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // 「下」(ジョグダイヤル下)を押した時の処理
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
        NNshWinViewPageScroll(FLDID_NEWTHREAD_MESSAGE,
                              SCLID_NEWTHREAD_MESSAGE, 0, winDown); 
        ret = true;
        break;

      // 「上」(ジョグダイヤル上)を押した時の処理
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
        NNshWinViewPageScroll(FLDID_NEWTHREAD_MESSAGE,
                              SCLID_NEWTHREAD_MESSAGE, 0, winUp); 
        ret = true;
        break;
 
      // Tabキーを押したときの処理
      case chrHorizontalTabulation:
        // フォーカスをメッセージにあてる
        frm = FrmGetActiveForm();
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_NEWTHREAD_MESSAGE));
        ret = true;
        break;

      // その他のボタン群
      case vchrJogPushedUp:
      case vchrJogPushedDown:
      case chrLeftArrow:
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
      case chrRightArrow:
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
      case vchrJogBack:
      default:
        break;
    }
    return (ret);
}

#endif // #ifdef USE_NEWTHREAD_FEATURE
