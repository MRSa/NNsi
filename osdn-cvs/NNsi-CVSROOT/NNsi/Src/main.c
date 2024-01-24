/*============================================================================*
 *
 *  $Id: main.c,v 1.65 2004/11/25 13:42:33 mrsa Exp $
 *
 *  FILE:  
 *     main.c
 *
 *  Description: 
 *     main source code for Palm application.
 *     (This file includes in "skeleton.c".)
 *
 *===========================================================================*/
#define MAIN_C

#include "machine.c"

// external functions
extern Err     OpenForm_NNshMessage (FormType  *frm);
extern void   OpenForm_ThreadMain  (FormType  *frm);
extern void   OpenForm_WriteMessage(FormType  *frm);
extern void   OpenForm_SelectBBS   (FormType  *frm);
extern void   OpenForm_newThread   (FormType  *frm);
extern Err     OpenForm_NNshSetting (FormType  *frm);
extern Err     OpenForm_NNsiSetting2(FormType  *frm);
extern Err     OpenForm_NNsiSetting3(FormType  *frm);
extern Err     OpenForm_NNsiSetting4(FormType  *frm);
extern Err     OpenForm_NNsiSetting5(FormType  *frm);
extern Err     OpenForm_NNsiSetting6(FormType  *frm);
extern Err     OpenForm_NNsiSetting7(FormType  *frm);
extern Err     OpenForm_NNsiSetting8(FormType  *frm);
extern Err     OpenForm_NNsiSetting9(FormType  *frm);
extern Err     OpenForm_NNsiSettingA(FormType  *frm);
extern Err     OpenForm_FavorTabSet (FormType  *frm);
extern Err     OpenForm_NNsiDummy   (FormType  *frm);
extern Err     OpenForm_GetLogList  (FormType  *frm);

extern Boolean Handler_ThreadMain   (EventType *event);
extern Boolean Handler_NNshMessage  (EventType *event);
extern Boolean Handler_WriteMessage (EventType *event);
extern Boolean Handler_SelectBBS    (EventType *event);
extern Boolean Handler_NNshSetting  (EventType *event);
extern Boolean Handler_FavorTabSet  (EventType *event);
extern Boolean Handler_newThread    (EventType *event);
extern Boolean Handler_NNsiDummy    (EventType *event);
extern Boolean Handler_GetLogList   (EventType *event);


/*=========================================================================*/
/*   Function :   MyApplicationDispatchEvent                               */
/*                                                特殊ディスパッチイベント */
/*=========================================================================*/
static Boolean MyApplicationDispatchEvent(EventType *event)
{
    UInt16 *command;

    //  "ハードキー制御する"　指定があり、かつキー入力イベントなら
    // フォームの処理を実施する
    if (((NNshGlobal->NNsiParam)->useHardKeyControl != 0)&&(event->eType == keyDownEvent))
    {
        if (event->data.keyDown.chr >= vchrHardKeyMin)
        {
            // ハードキーコードの確認
            switch (event->data.keyDown.chr)
            {
              case vchrHard1:
                // ハードキー１
                command = &((NNshGlobal->NNsiParam)->useKey.key1);
                break;

              case vchrHard2:
                // ハードキー２
                command = &((NNshGlobal->NNsiParam)->useKey.key2);
                break;

              case vchrHard3:
                // ハードキー３
                command = &((NNshGlobal->NNsiParam)->useKey.key3);
                break;

              case vchrHard4:
                // ハードキー４
                command = &((NNshGlobal->NNsiParam)->useKey.key4);
                break;

              case vchrCapture:
              case vchrVoiceRec:
              case vchrVZ90functionMenu:      
              case vchrGarminRecord:
              case vchrHard5:
                // CLIE NX キャプチャボタン、CLIE TGボイスレコボタン
                command = &((NNshGlobal->NNsiParam)->useKey.clieCapture);
                break;

              case chrLeftArrow:         // カーソルキー左(5way左)
              case vchrRockerLeft:
              case vchrJogLeft:
              case vchrTapWaveSpecificKeyRLeft:
                // ハードキー左
                command = &((NNshGlobal->NNsiParam)->useKey.left);
                break;

              case chrRightArrow:     // カーソルキー右(5way右)
              case vchrRockerRight:
              case vchrJogRight:
              case vchrTapWaveSpecificKeyRRight:
                // ハードキー右
                command = &((NNshGlobal->NNsiParam)->useKey.right);
                break;

              case vchrHWKeyboardPush:
                // CLIE ハードキー押下(何もしない)
                return (false);
                break;

              default:
                //  （知らない）ハードキー入力が行われたとき、キーコードを
                // 表示する
                NNsh_DebugMessage(ALTID_INFO, "Hard Key Event ", " chrID:",
                                  event->data.keyDown.chr);
                return (false);
                break;
            }
            // コマンドの判定
            switch (*command)
            {
              case HARDKEY_FEATURE_CONTROLNNSI:
                // NNsiで制御(ハードキーが押されたときの処理を実行)
                FrmDispatchEvent(event);
                return (true);
                break;

              case HARDKEY_FEATURE_TRANSJOGPUSH:
                // イベントのキーコードを無理やりJOG PUSH(LineFeed)に書き換える
                event->data.keyDown.chr = chrLineFeed;
                break;

              case HARDKEY_FEATURE_TRANSJOGBACK:
                // イベントのキーコードを無理やりJOG BACK(ESC)に書き換える
                event->data.keyDown.chr = chrEscape;
                break;

              case HARDKEY_FEATURE_DONOTHING:
              default:
                // 何もしない
                break;
            }
        }
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   MyApplicationHandleEvent                                 */
/*                                                                         */
/*=========================================================================*/
static Boolean MyApplicationHandleEvent(EventType *event)
{
    FormType  *frm;
    TableType *tableP;
    UInt16     rowNum, fieldFont, objFont;

    // SONYシルク制御を行う場合の画面更新
    if (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY)
    {
         // SILKマネージャの表示にあわせた画面の更新を行う
        frm = FrmGetActiveForm();
        if (frm != NULL)
        {
            FrmEraseForm(frm);
            NNsi_FrmDrawForm(frm, true);
        }
        NNshGlobal->updateHR = NNSH_NOT_EFFECTIVE;
    }

    // Notifyイベントの受信処理を通過した時
    if (NNshGlobal->notifyUpdate != 0)
    {
        frm = FrmGetActiveForm();
        if (frm != NULL)
        {
            switch (NNshGlobal->notifyUpdate)
            {
              case NNSH_VFS_MOUNTED:
                // VFSがmountされたことをユーザに通知する
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_MOUNTED, MSG_WARN_VFS_UPDATE, 0);
                break;

              case NNSH_VFS_UNMOUNTED:
                // VFSがunmountされたことをユーザに通知する
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_UNMOUNTED, " ", 0);
                break;

              case NNSH_VFS_UNMOUNTED_WRITE:
                // VFSがunmountされたことをユーザに通知する
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_UNMOUNTED, MSG_WARN_BACK_THREAD, 0);

                // WRITE画面から一覧画面に移動する
                SaveWritingMessage();
                FrmEraseForm(frm);
                FrmGotoForm (NNshGlobal->backFormId);
                break;

              case NNSH_VFS_UNMOUNTED_OTHER:
                // VFSがunmountされたことをユーザに通知する
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_UNMOUNTED, MSG_WARN_BACK_THREAD, 0);

                // 前に開いていた画面(一覧画面 or ダミー画面 or 参照画面)に移動する
                FrmEraseForm(frm);
                FrmGotoForm (ConvertFormID((NNshGlobal->NNsiParam)->lastFrmID));
                break;

              case NNSH_VFS_UNMOUNTED_MESSAGE:
                // VFSがunmountされたことをユーザに通知する
                NNsh_InformMessage(ALTID_WARN, MSG_VFS_UNMOUNTED, MSG_WARN_BACK_THREAD, 0);

                // 参照画面から一覧画面に移動する
                ReadMessage_Close(NNshGlobal->backFormId);
                break;

              case NNSH_DISP_RESIZED:
                // 画面のサイズが変更になった...
                HandEraResizeForm(frm);
                UpdateFieldRegion();
                NNsh_DebugMessage(ALTID_INFO, "[[<<DISPLAY RESIZED>>]]", "", 0);

                FrmEraseForm(frm);
                NNsi_FrmDrawForm(frm, true);
                break;

              default:
                break;
            }
        }
        NNshGlobal->notifyUpdate = 0;
    }

    // ここで、(フォームロード等の)イベント受信時の処理を記述する。
    switch (event->eType)
    {
      // フォームを開いたよevent
      case frmLoadEvent:
        // 使用フォントを設定する
        fieldFont = (NNshGlobal->NNsiParam)->currentFont;
        FntSetFont(fieldFont);
        objFont   = (NNshGlobal->NNsiParam)->currentFont;

        // フォームを初期化 
        frm = FrmInitForm(event->data.frmLoad.formID);
#ifdef USE_HANDERA
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaFormModify(frm, vgaFormModify160To240);
        }
#endif
#ifdef USE_PIN_DIA
        if (NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)
        {
            VgaFormModify_DIA(frm, vgaFormModifyNormalToWide);
        }
        else if (NNshGlobal->device == NNSH_DEVICE_GARMIN)
        {
            // 画面をオープンしたときは、再描画する(Garmin機)
            VgaFormModify_DIA(frm, vgaFormModifyNormalToWide);
            NNshGlobal->notifyUpdate = NNSH_DISP_RESIZED;
            EvtWakeup();
        }
#endif

#ifdef USE_MACRO
        if (NNshGlobal->tempStatus != NNSH_MACROCMD_EXECUTE)
        {
            NNshGlobal->tempStatus = 0;
        }
#else
        NNshGlobal->tempStatus = 0;
#endif

        // フォームを開いてイベントハンドラを設定する
        FrmSetActiveForm(frm);
        switch (event->data.frmLoad.formID)
        {
          // スレ一覧画面
          case FRMID_THREAD:
            // グラフィティエリアを最小化して画面をリサイズ、フォントを変更
            (void) NNshSilkMinimize(frm);

            // フォームを開いて、イベントハンドラを設定する
            OpenForm_ThreadMain(frm);
            FrmSetEventHandler(frm, Handler_ThreadMain);

            // OpenForm_ThreadMain()で画面を描画するため、すぐに抜ける
            return (true);
            break;

          // スレ参照画面
          case FRMID_MESSAGE:
            // グラフィティエリアを最小化して画面をリサイズ、フォントを変更
            (void) NNshSilkMinimize(frm);
            FldSetFont(FrmGetObjectPtr(frm, 
                            FrmGetObjectIndex(frm, FLDID_MESSAGE)), fieldFont);
#ifdef USE_GLUE
            CtlGlueSetFont(FrmGetObjectPtr(frm,
                       FrmGetObjectIndex(frm, SELTRID_THREAD_TITLE)), objFont);
#endif
            // フォームを開いて、イベントハンドラを設定する
            OpenForm_NNshMessage(frm);
            FrmSetEventHandler(frm, Handler_NNshMessage);

            // OpenForm_NNshMessage()で画面を描画するため、すぐに抜ける
            return (true);
            break;
 
          // スレ書きこみ画面
          case FRMID_WRITE_MESSAGE:
            // グラフィティエリアを最大化して画面をリサイズ、フォントを変更
            FldSetFont(FrmGetObjectPtr(frm,
                         FrmGetObjectIndex(frm, FLDID_WRITE_NAME)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                        FrmGetObjectIndex(frm, FLDID_WRITE_EMAIL)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                      FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE)), fieldFont);
            if ((NNshGlobal->NNsiParam)->notOpenSilkWrite == 0)
            {
                (void) NNshSilkMaximize(frm);
            }
            else
            {
                (void) NNshSilkMinimize(frm);
            }

            // フォームを開いて、イベントハンドラを設定する
            OpenForm_WriteMessage(frm);
            FrmSetEventHandler(frm, Handler_WriteMessage);
            break;

          // BBS管理画面
          case FRMID_MANAGEBBS:
            // グラフィティエリアを最小化して画面をリサイズする
            if (NNshSilkMinimize(frm) == errNone)
            {
                // CLIE UX以外のときは拡大、、、（すげーベタ、、、）
#ifdef USE_CLIE
                if (NNshGlobal->silkVer != 3)
#endif
                {
                    // 領域サイズを求める
                    tableP = FrmGetObjectPtr(frm, 
                                   FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
                    for (rowNum = 0;
                         rowNum < TblGetNumberOfRows(tableP); rowNum++)
                    {
                        TblSetRowHeight(tableP, rowNum, 15);
                    }
                }
            }
            // フォームを開いて、イベントハンドラを設定する
            OpenForm_SelectBBS(frm);
            FrmSetEventHandler(frm, Handler_SelectBBS);
            break;

          // NNsi設定画面
          case FRMID_CONFIG_NNSH:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNshSetting(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定-2画面
          case FRMID_NNSI_SETTING2:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting2(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定-3画面
          case FRMID_NNSI_SETTING3:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting3(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定-4画面
          case FRMID_NNSI_SETTING4:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting4(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定-5画面
          case FRMID_NNSI_SETTING5:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting5(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定-6画面
          case FRMID_NNSI_SETTING6:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting6(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定-7画面
          case FRMID_NNSI_SETTING7:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting7(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定-8画面
          case FRMID_NNSI_SETTING8:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting8(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定-9画面
          case FRMID_NNSI_SETTING9:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSetting9(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // NNsi設定(概略)画面
          case FRMID_NNSI_SETTING_ABSTRACT:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNsiSettingA(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;

          // お気に入り条件設定画面
          case FRMID_FAVORSET_DETAIL:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_FavorTabSet(frm);
            FrmSetEventHandler(frm, Handler_FavorTabSet);
            break;

          // NNsi起動(DUMMY)画面
          case FRMID_NNSI_DUMMY:
            // グラフィティエリアを最小化して画面をリサイズ、フォントを変更
            (void) NNshSilkMinimize(frm);
            OpenForm_NNsiDummy(frm);
            FrmSetEventHandler(frm, Handler_NNsiDummy);
            break;

          // スレ立て画面
          case FRMID_NEW_THREAD:
            // グラフィティエリアを最大化して画面をリサイズ、フォントを変更
            FldSetFont(FrmGetObjectPtr(frm,
                         FrmGetObjectIndex(frm, FLDID_WRITE_NAME)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                        FrmGetObjectIndex(frm, FLDID_WRITE_EMAIL)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                      FrmGetObjectIndex(frm, FLDID_NEWTHREAD_TITLE)), fieldFont);
            FldSetFont(FrmGetObjectPtr(frm, 
                      FrmGetObjectIndex(frm, FLDID_NEWTHREAD_MESSAGE)), fieldFont);
            if ((NNshGlobal->NNsiParam)->notOpenSilkWrite == 0)
            {
                (void) NNshSilkMaximize(frm);
            }
            else
            {
                (void) NNshSilkMinimize(frm);
            }

            // フォームを開いて、イベントハンドラを設定する
            OpenForm_newThread(frm);
            FrmSetEventHandler(frm, Handler_newThread);
            break;

#ifdef USE_LOGCHARGE
          // 参照ログ一覧画面
          case FRMID_LISTGETLOG:
            // グラフィティエリアを最小化して画面をリサイズ、フォントを変更
            (void) NNshSilkMinimize(frm);
            OpenForm_GetLogList(frm);
            FrmSetEventHandler(frm, Handler_GetLogList);

            // OpenForm_GetLogList()で画面を描画するため、すぐに抜ける
            return (true);
            break;
#endif // #ifdef USE_LOGCHARGE

          default:
            // 何の"フォーム開いたよイベント"か表示してみる。(デバッグ)
            NNsh_DebugMessage(ALTID_INFO, "frmLoadEvent()", " formID:",
                              event->data.frmLoad.formID);
            break;
        }
        // 画面を描画する
        NNsi_FrmDrawForm(frm, true);
        break;

#ifdef USE_PIN_DIA
      case winEnterEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();

            // 画面を描画されたとき、シルク最小化/最大化を有効にする
            if (event->data.winEnter.enterWindow == FrmGetWindowHandle(frm))
            {
                PINSetInputTriggerState(pinInputTriggerEnabled);
            }
        }
        break;
#endif  // #ifdef USE_PIN_DIA
#if 0
      // フォームを開いたよevent
      case frmOpenEvent:
        break;

      // フォームを閉じたよevent
      case frmCloseEvent:
        break;
#endif
      default:
        // サポートしていないイベントを受信した。
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   MyApplicationHandleNotifications                         */
/*                                Notification処理                         */
/*=========================================================================*/
static UInt32  MyApplicationHandleNotifications(SysNotifyParamType *notifyParamsP)
{ 
  // PilotMainが持てるCode Resourceの場合はこっちでNotificationを処理するのが一般的
  // 持てない場合はSysNotifyRegisterでfunction pointerを渡してやる

  switch(notifyParamsP->notifyType)
  {
    case sysNotifyVolumeUnmountedEvent:
        VFSUnmountNotify(notifyParamsP);
        break;

    case sysNotifyVolumeMountedEvent:
        VFSMountNotify(notifyParamsP);
        break;     
  }
  
  return 0;
}

/*=========================================================================*/
/*   Function :   myStartApplication                                       */
/*                                 スタート時の(プログラム個別)初期化処理  */
/*=========================================================================*/
static UInt16 MyStartApplication(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    Int16                   ret;
    UInt16                  size;
    NNshSoftwareVersionInfo verInfo;
#ifdef USE_ARMLET
    UInt32                  processorType;
#endif
    UInt32                  osVersion;
#ifdef USE_TSPATCH
    UInt32                  fontId, fontVer;
#endif    

    // ビーム受信登録をする...
    ExgRegisterData(SOFT_CREATOR_ID, exgRegExtensionID, NNSI_EXCHANGEINFO_SUFFIX); 

    // NNsi設定バージョンの確認
    //   (Version確認用Preferenceを確認し、変更がなければ復旧)
    size = sizeof(NNshSoftwareVersionInfo);
    MemSet(&verInfo, sizeof(NNshSoftwareVersionInfo), 0x00);
    ret  = PrefGetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, 
                                 &verInfo, &size, false);
    if (ret == noPreferenceFound)
    {
        //  preferenceが存在しなかった場合には、バージョン番号と
        // NNsi設定のサイズは正しいものとして処理する。
        ret              = SOFT_PREFVERSION;
        verInfo.prefSize = sizeof(NNshSavedPref);
    }

    // NNsiグローバル領域用格納領域の確保
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MEMALLOC_PTR(size);
    if (NNshGlobal == NULL)
    {
        // 領域確保に失敗！(NNsiは起動しない)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_NNSI_CANNOT_LAUNCH,
                          "(NNsiGbl)\nsize:", size);
        return (0);
    }        
    MemSet (NNshGlobal, size, 0x00);
    StrCopy(NNshGlobal->dummyBuffer, " ");

    // 開いた当初は、スレ一覧画面に戻るようにする。
    // ※ 注意 ※
    //    参照ログ一覧→参照画面で開いたあと、NNsiを再起動して戻ると、
    //   このままだと、通常の一覧画面に遷移してしまう。
    NNshGlobal->backFormId = FRMID_THREAD;

    // NNsi設定用格納領域の確保
    size = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    (NNshGlobal->NNsiParam) = (NNshSavedPref *) MEMALLOC_PTR(size);
    if ((NNshGlobal->NNsiParam) == NULL)
    {
        // 領域確保に失敗！(NNsiは起動しない)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_NNSI_CANNOT_LAUNCH,
                          "(NNsiSet)\nsize:", size);
        return (0);
    }
    MemSet((NNshGlobal->NNsiParam), size, 0x00);

    // OSバージョンの確認
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osVersion) == errNone)
    {
        NNshGlobal->palmOSVersion = osVersion;
    }

    // NNsi設定を復旧させる
    (void) NNshRestoreNNsiSetting(ret, verInfo.prefSize, (NNshGlobal->NNsiParam));

    // デバッグモードの情報を復旧させる
    (NNshGlobal->NNsiParam)->debugMessageON = verInfo.debugMode;

    // VFS機能がない設定だった場合には、一旦VFSパラメータをクリアする。
    if ((NNshGlobal->NNsiParam)->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        (NNshGlobal->NNsiParam)->useVFS = NNSH_VFS_DISABLE;
    }
    // VFSサポートの設定
    checkVFSdevice_NNsh();

    // VFS 上にバックアップされたDBがあれば、それをコピーするかどうか
    if (((NNshGlobal->NNsiParam)->useVFS & (NNSH_VFS_ENABLE | NNSH_VFS_DBIMPORT)) ==
        (NNSH_VFS_ENABLE | NNSH_VFS_DBIMPORT))
    {
        UInt16 backup;
        
        backup = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_NOTHING;

        // VFS から DB をコピーする
        RestoreDBtoVFS(false);
        if (NNshGlobal->restoreSetting)
        {
            // NNsi設定を復旧させる
            (void) NNshRestoreNNsiSetting(ret, verInfo.prefSize, (NNshGlobal->NNsiParam));
        }
        else
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = backup;
        }
    }

#ifdef USE_CLIE
    // Silk制御の設定を反映
    NNshGlobal->notUseSilk = (NNshGlobal->NNsiParam)->notUseSilk;

    // CLIEサポートの設定
    setCLIESupport_NNsh();
#endif  // #ifdef USE_CLIE

#ifdef USE_HANDERA
    // HandEraサポートの設定
    setHandEraSupport_NNsh();
#endif // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
    // pen input managerの設定
    setDIASupport_NNsh();
#endif    // #ifdef USE_PIN_DIA

#ifdef USE_HIGHDENSITY
    // 高解像度サポート/カラーサポートのチェック
    setDoubleDensitySupport_NNsh();
#else
    // 高解像度サポート設定を初期化
    NNshGlobal->os5HighDensity = (UInt32) kDensityLow;
#endif    // #ifdef USE_HIGHDENSITY

#ifdef USE_TSPATCH
    // TsPatch対応コード...(TsPatch適用中かどうかチェックする)
    if ((NNshGlobal->NNsiParam)->notUseTsPatch == 0)
    {
        ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
        if (ret == errNone)
        {    
            // TsPatch適用中...使用フォントを小さくする。
            switch ((NNshGlobal->NNsiParam)->currentFont)
            {
              case stdFont:
                FtrGet(SmallFontAppCreator, SMF_FTR_TINY_FONT, &fontId);
                break;

              case boldFont:
                FtrGet(SmallFontAppCreator, SMF_FTR_TINY_BOLD_FONT, &fontId);
                break;

              case largeFont:
                FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT, &fontId);
                break;

              case largeBoldFont:
                FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_BOLD_FONT, &fontId);
                break;

              default:
                fontId = (NNshGlobal->NNsiParam)->currentFont;
                break;
            }
            (NNshGlobal->NNsiParam)->currentFont = fontId;
        }
    }
    else
    {
        // TsPatch機能を使わない場合,,,
        switch ((NNshGlobal->NNsiParam)->currentFont)
        {
          case stdFont:
          case boldFont:
          case largeFont:
          case largeBoldFont:
            // そのまま使う
            break;

          default:
            // 標準フォントに変更する
            (NNshGlobal->NNsiParam)->currentFont = stdFont;
            break;
        }
    }
#endif  // #ifdef USE_TSPATCH


#ifdef USE_ARMLET
    // ARMlet使用可否のチェック
    FtrGet(sysFileCSystem, sysFtrNumProcessorID, &processorType);
    if (sysFtrNumProcessorIsARM(processorType))
    {
        // ARMlet使用可能デバイス。
        // (NNshGlobal->NNsiParam)->useARMlet = NNSH_ENABLE;  // ARMletは使用する設定に変更
    }
    else
#endif
    {
        // ARMletは使用しない設定にする
        (NNshGlobal->NNsiParam)->useARMlet = NNSH_DISABLE;
    }

    // WebBrowserのインストールチェック
    checkInstalledWebBrowser();

    // しおり設定されていた場合
    if ((NNshGlobal->NNsiParam)->useBookmark != 0)
    {
        // しおり設定記憶領域を確保する
        NNshGlobal->bookmarkFileName = MEMALLOC_PTR(MAX_THREADFILENAME);
        if (NNshGlobal->bookmarkFileName != NULL)
        {
            NNshGlobal->bookmarkNick = MEMALLOC_PTR(MAX_NICKNAME);
            if (NNshGlobal->bookmarkNick == NULL)
            {
                MEMFREE_PTR(NNshGlobal->bookmarkFileName);
            }
        }
        if ((NNshGlobal->NNsiParam)->lastFrmID == NNSH_FRMID_MESSAGE)
        {
            // しおりが設定されていた場合には、開く場所を指定
            if ((NNshGlobal->NNsiParam)->bookMsgNumber != 0)
            {
                (NNshGlobal->NNsiParam)->openMsgIndex = (NNshGlobal->NNsiParam)->bookMsgIndex;
            }
        }
    }

    // NG設定の文字列チェック
    if ((NNshGlobal->NNsiParam)->useRegularExpression != 0)
    {
        SeparateWordList((NNshGlobal->NNsiParam)->hideWord1, &(NNshGlobal->hide1));
        SeparateWordList((NNshGlobal->NNsiParam)->hideWord2, &(NNshGlobal->hide2));
    }

    // タイトルレコードの指定が異常の場合、レコード番号を初期化する
    if (((NNshGlobal->NNsiParam)->startTitleRec == (NNshGlobal->NNsiParam)->endTitleRec)&&
        ((NNshGlobal->NNsiParam)->startTitleRec == 0))
    {
        (NNshGlobal->NNsiParam)->startTitleRec = 0xffff;
    }

    // 起動時に起動ワードが設定されていた場合、ダミー画面を開く
    if (StrLen((NNshGlobal->NNsiParam)->launchPass) != 0)
    {
        return (FRMID_NNSI_DUMMY);
    }

    // VFSがOFFだった場合には、確認メッセージを表示する。
    if ((NNshGlobal->NNsiParam)->useVFS == NNSH_VFS_DISABLE)
    {
        NNsh_WarningMessage(ALTID_WARN, MSG_USE_VFS_WARNING1,
                            MSG_USE_VFS_WARNING2, 0);
    }

    // ZLIBのサポート可否確認を行う
    SetZLIBsupport_NNsh();
        
#ifdef USE_MACRO
    // 起動時1回だけ実行するマクロが存在した場合、、、
    if (DmFindDatabase(0, DBNAME_RUNONCEMACRO) != 0)
    {
        NNshGlobal->tempStatus = NNSH_MACROCMD_OPERATE;
        return (FRMID_THREAD);
    }

    // マクロの自動実行が設定されていた場合には、一覧画面を開く
    if ((NNshGlobal->NNsiParam)->autostartMacro != 0)
    {
        NNshGlobal->tempStatus = NNSH_MACROCMD_EXECUTE;
        return (FRMID_THREAD);
    }
#endif

    // 最初にオープンするフォームのIDを応答する。
    return (ConvertFormID((NNshGlobal->NNsiParam)->lastFrmID));
}

/*=========================================================================*/
/*   Function :   myStopApplication                                        */
/*                                 ストップ時の(プログラム個別)初期化処理  */
/*=========================================================================*/
static void MyStopApplication(void)
{
    UInt16                  savedParam, frmId;
    NNshSoftwareVersionInfo verInfo;

    frmId = FrmGetActiveFormID();
    switch (frmId)
    {
      case FRMID_WRITE_MESSAGE:
        //  書き込み画面でアプリを閉じようとした場合
        // (書き込み内容を保存するか確認する)
        SaveWritingMessage();
        break;
      case FRMID_MESSAGE:
        //  参照ログ一覧画面から開いたスレ参照中の場合には、
        // 次回起動時はスレ参照画面を開くようにする。(苦肉の策...)
        if (NNshGlobal->backFormId == FRMID_LISTGETLOG)
        {
            if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&
                 ((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
            {
                // しおり設定中なので、何もしない
            }
            else
            {
                (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_GETLOGLIST;
            }
        }
        break;

      default:
        break;
    }

    // VFS Notificationの解放
    resetVFSNotifications();

    // NNsi設定のVersion確認用Preferenceの保存(Debugモードはこちらで記憶)
    verInfo.NNsiVersion = SOFT_PREFVERSION;
    verInfo.prefSize    = sizeof(NNshSavedPref);
    verInfo.prefType    = NNSH_PREFERTYPE;
    verInfo.debugMode   = ((NNshGlobal->NNsiParam) != NULL) ? (NNshGlobal->NNsiParam)->debugMessageON : 0;
    PrefSetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, SOFT_PREFVERSION,
                          &verInfo, sizeof(NNshSoftwareVersionInfo), false);

    // NNsiのグローバル領域が確保されていないときは終了する
    if (NNshGlobal == NULL)
    {
        return;
    }

    // スレタイトル表示情報を削除(一覧画面)
    ClearMsgTitleInfo();

#ifdef USE_CLIE
    // CLIEサポートを開放する
    resetCLIESupport_NNsh();
#endif

    // NNsi設定の保存(NNsiの起動に成功した時のみ)
    if ((NNshGlobal->NNsiParam) != NULL)
    {
        // テンポラリファイルの削除(DEBUGモード時には残しておく)
        if ((NNshGlobal->NNsiParam)->debugMessageON == 0)
        {
            (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
            (void) DeleteFile_NNsh(FILE_RECVMSG, NNSH_VFS_DISABLE);
        }

        // NNsi終了時に回線を切断する
        if ((NNshGlobal->NNsiParam)->disconnectNNsiEnd != 0)
        {
            // プログラム終了時には、「回線切断」の確認メッセージは出さない。
            savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            NNshNet_LineHangup();
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
        }

        // Silk制御の設定を反映
#ifdef USE_CLIE
        if ((NNshGlobal->NNsiParam)->notUseSilk != NNshGlobal->notUseSilk)
        {
            (NNshGlobal->NNsiParam)->notUseSilk = NNshGlobal->notUseSilk;
        }
#endif  // #ifdef USE_CLIE
        NNshSaveNNsiSetting(NNSH_PREFERTYPE, sizeof(NNshSavedPref), (NNshGlobal->NNsiParam));

        // NNsi終了時にデータベースをVFSにバックアップする
        if (((NNshGlobal->NNsiParam)->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            BackupDBtoVFS(NNSH_DISABLE);
        }

        // 「しおり設定」設定中の場合
        if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&
            ((NNshGlobal->NNsiParam)->bookMsgNumber != 0)&&
            (NNshGlobal->bookmarkFileName != NULL))
        {
            // しおり設定(index番号)を記憶する
            (void) GetSubjectIndex(NNshGlobal->bookmarkNick,
                                    NNshGlobal->bookmarkFileName,
                                    &((NNshGlobal->NNsiParam)->bookMsgIndex));
        }
    }

    // NNsiグローバル領域（の中身）を開放する
    NNshClearGlobal(NNshGlobal);

    // NNsi設定の使用領域を開放する
#ifdef RELEASE_NNSISETTING
    //MEMFREE_PTR((NNshGlobal->NNsiParam));
    MemPtrFree((NNshGlobal->NNsiParam));
#endif

    // NNsiのグローバル領域を開放する
#ifdef RELEASE_NNSIGLOBAL
    // MEMFREE_PTR(NNshGlobal);
    MemPtrFree(NNshGlobal);
#endif

    return;
}

/*=========================================================================*/
/*   Function : MyApplicationReceiveData                                   */
/*                            Exchangeマネージャを使用してデータを受信する */
/*=========================================================================*/
UInt32 MyApplicationReceiveData(void *cmdPBP, UInt16 launchFlags)
{
    ExgSocketType      *exgSocket;
    NNshGetLogDatabase *logP;
    Err                 err;
    UInt8              *dbName, *token, entried;
    UInt8              *buff, *ptr, *data;
    UInt16              len;
    UInt32              size, ret;
    MemHandle           strH;
    DmOpenRef           dbRef;

    // アプリケーションが起動中かどうかを確認するフラグ
    entried    = 0;

    // 受信データサイズの確保
    exgSocket = (ExgSocketType *) cmdPBP;
    size = (BUFSIZE > exgSocket->length) ? BUFSIZE : exgSocket->length;

    buff = MEMALLOC_PTR(size + MARGIN);
    if (buff == NULL)
    {
        // エラー...
        return (false);
    }
    MemSet(buff, (size + MARGIN), 0x00);
    
    // DB名領域の確保
    dbName = MEMALLOC_PTR(BUFSIZE + MARGIN);
    if (dbName == NULL)
    {
        MEMFREE_PTR(buff);
        return (false);
    }
    MemSet(dbName, (BUFSIZE + MARGIN), 0x00);

    // トークン領域の確保
    token = MEMALLOC_PTR(BUFSIZE + MARGIN);
    if (token == NULL)
    {
        MEMFREE_PTR(dbName);
        MEMFREE_PTR(buff);
        return (false);
    }
    MemSet(dbName, (BUFSIZE + MARGIN), 0x00);

    // 参照ログ用ＤＢを確保する
    logP = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (logP == NULL)
    {
        MEMFREE_PTR(token);
        MEMFREE_PTR(dbName);
        MEMFREE_PTR(buff);
        return (false);
    }
    
    // 受信データの受け付け
    err = ExgAccept(exgSocket);
    if (err != errNone)
    {
        // 受信失敗...
        goto FUNC_END;
    }

    // データが末尾に到達するまで受信する
    ptr = buff;
    while (1)
    {
        ret = ExgReceive(exgSocket, ptr, size, &err);
        if ((ret == 0)||(err == exgErrUserCancel)||(err == exgErrNotSupported))
        {
            // 転送終了、もしくはエラー終了なので抜ける
            break;
        }

        // データの確認
        MemSet(token, sizeof(token), 0x00);
        strH = DmGetResource('tSTR', NNSI_STRINGTAG_START_NNSIEXT_THREADNUMBER);
        if (strH != 0)
        {
            StrCopy(token, MemHandleLock(strH));
            MemHandleUnlock(strH);
        }
        else
        {
            token[0] = '<';
            token[1] = 'N';
            token[2] = 'N';
            token[3] = 's';
            token[4] = 'i';
            token[5] = ':';
        }
        data = StrStr(ptr, token);
        if (data != NULL)
        {
            // スレ番号の抽出
            data = data + 28; // 28は、StrLen(NNSI_NNSIEXT_THREADNUMBER_START);
            ptr = data;
            while ((*ptr != '<')&&(*ptr != '\0'))
            {
                ptr++;
            }
            *ptr = '\0';

            // DB名を登録する
            MemSet(dbName, sizeof(dbName), 0x00);
            dbName[0] = 'G';
            dbName[1] = 'e';
            dbName[2] = 't';
            dbName[3] = 'R';
            dbName[4] = 'e';
            dbName[5] = 's';
            dbName[6] = 'e';
            dbName[7] = 'r';
            dbName[8] = 'v';
            dbName[9] = 'e';
            dbName[10] = '-';
            dbName[11] = 'N';
            dbName[12] = 'N';
            dbName[13] = 's';
            dbName[14] = 'i';

            // スレ番号を登録する...
            //  (OpenDatabase_NNsh(dbName,DBVERSION_GETRESERVE,&dbRef); と等価)
           {
                LocalID dbId;

                dbRef = 0;
                dbId  = DmFindDatabase(0, dbName);
                if (dbId == 0)
                {
                    // データベースを新規作成
                    (void) DmCreateDatabase(0, dbName, 
                                       SOFT_CREATOR_ID, SOFT_DBTYPE_ID, false);
                    dbId      = DmFindDatabase(0, dbName);
                    if (dbId != 0)
                    {
                        UInt16 dbVersion;
                        dbVersion = DBVERSION_GETRESERVE;
                        (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, 
                                                      &dbVersion, NULL,
                                                      NULL,NULL,NULL,NULL,
                                                      NULL,NULL,NULL);
                    }
                }

                if (dbId != 0)
                {
                    dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);
                }
            }
            if (dbRef != 0)
            {
                // ＤＢに登録する
                // (void) EntryRecord_NNsh(dbRef, BUFSIZE, data); と等価
                {
                        MemHandle  newRecH;
                        UInt16     index;

                        // レコードの新規追加
                        index   = 0;
                        newRecH = DmNewRecord(dbRef, &index, 
                                                    BUFSIZE + sizeof(UInt32));
                        if (newRecH != 0)
                        {
                            // レコードにデータ書き込み
                            DmWrite(MemHandleLock(newRecH), 0, data, BUFSIZE);
                            MemHandleUnlock(newRecH);

                            // レコードを解放（変更終了）
                            DmReleaseRecord(dbRef, index, true);
                        }
                }
                // CloseDatabase_NNsh(dbRef); と等価
                {
                    DmCloseDatabase(dbRef);
                    dbRef = 0;
                }
                entried = 1;
            }
        }
        else
        {
            // DB名を登録する
            MemSet(dbName, sizeof(dbName), 0x00);
            dbName[0] = 'L';
            dbName[1] = 'o';
            dbName[2] = 'g';
            dbName[3] = 'P';
            dbName[4] = 'e';
            dbName[5] = 'r';
            dbName[6] = 'm';
            dbName[7] = 'a';
            dbName[8] = 'n';
            dbName[9] = 'e';
            dbName[10] = 'n';
            dbName[11] = 't';
            dbName[12] = '-';
            dbName[13] = 'N';
            dbName[14] = 'N';
            dbName[15] = 's';
            dbName[16] = 'i';

            // データの先頭を見つける...
            MemSet(token, sizeof(token), 0x00);
            strH = DmGetResource('tSTR', NNSI_STRINGTAG_START_NNSIEXT_URL);
            if (strH != 0)
            {
                StrCopy(token, MemHandleLock(strH));
                MemHandleUnlock(strH);
            }
            else
            {
                token[0] = '<';
                token[1] = 'N';
                token[2] = 'N';
                token[3] = 's';
                token[4] = 'i';
                token[5] = ':';
            }

            // URLの確認
            data = StrStr(ptr, token);
            if (data != NULL)
            {
                // URLの抽出
                data = data + 25; // 25は、StrLen(NNSI_NNSIEXT_URL_START);

                MemSet(token, sizeof(token), 0x00);
                strH = DmGetResource('tSTR', NNSI_STRINGTAG_END);
                if (strH != 0)
                {
                    StrCopy(token, MemHandleLock(strH));
                    MemHandleUnlock(strH);

                    // データの末尾を探す
                    ptr = StrStr(data, token);
                    if (ptr != NULL)
                    {
                        *ptr = '\0';
                    }
                }

                // URLを参照ログ用DBに登録する...
                // OpenDatabase_NNsh(dbName, DBVERSION_PERMANENTLOG, &dbRef);
                {
                    LocalID dbId;

                    dbRef = 0;
                    dbId  = DmFindDatabase(0, dbName);
                    if (dbId == 0)
                    {
                        // データベースを新規作成
                        (void) DmCreateDatabase(0, dbName, 
                                       SOFT_CREATOR_ID, SOFT_DBTYPE_ID, false);
                        dbId      = DmFindDatabase(0, dbName);
                        if (dbId != 0)
                        {
                            UInt16 dbVersion;
                            dbVersion = DBVERSION_PERMANENTLOG;
                            (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, 
                                                      &dbVersion, NULL,
                                                      NULL,NULL,NULL,NULL,
                                                      NULL,NULL,NULL);
                        }
                    } 
                    if (dbId != 0)
                    {
                        dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);
                    }
                }
                if (dbRef != 0)
                {
                    // データ領域の初期化
                    MemSet(logP, (sizeof(NNshGetLogDatabase) + MARGIN), 0x00);

                    // アドレス(URL)を取り出す
                    ptr = logP->dataURL;
                    len = 0;
                    while ((*data != '"')&&(len < MAX_GETLOG_URL))
                    {
                        *ptr = *data;
                        ptr++;
                        data++;
                        len++;
                    }
                    data++;

                    // タイトルを抽出する
                    ptr = logP->dataTitle;
                    len = 0;
                    while ((*data != '\0')&&(*data != '"')&&(len < MAX_TITLE_LABEL))
                    {
                        *ptr = *data;
                        ptr++;
                        data++;
                        len++;
                    }

                    // 参照ログのＤＢに登録する
                    // EntryRecord_NNsh(dbRef,sizeof(NNshGetLogDatabase),logP);
                    {
                        MemHandle  newRecH;
                        UInt16     index;

                        // レコードの新規追加
                        index   = 0;
                        newRecH = DmNewRecord(dbRef, &index, 
                                 sizeof(NNshGetLogDatabase) + sizeof(UInt32));
                        if (newRecH != 0)
                        {
                            // レコードにデータ書き込み
                            DmWrite(MemHandleLock(newRecH), 0, logP,
                                                   sizeof(NNshGetLogDatabase));
                            MemHandleUnlock(newRecH);

                            // レコードを解放（変更終了）
                            DmReleaseRecord(dbRef, index, true);
                        }
                    }
                    // CloseDatabase_NNsh(dbRef);
                    {
                        DmCloseDatabase(dbRef);
                        dbRef = 0;
                    }
                    entried = 2;
                }
            }
        }
        ptr = buff;
    }
    // 回線切断
    err = ExgDisconnect(exgSocket, err);

FUNC_END:
    // URL受信時の情報ダイアログ
    if (entried == 2)
    {
        strH = DmGetResource('tSTR', MSG_ENTRIED_URL);
        if (strH != 0)
        {
            // URLを受信したときのダイアログを表示する
            FrmCustomAlert(ALTID_WARN, MemHandleLock(strH), "", "");
        }
        MemHandleUnlock(strH);
    }
    
    // スレ番号受信時の情報ダイアログ
    if (entried == 1)
    {
        strH = DmGetResource('tSTR', MSG_ENTRIED_THREADNUMBER);
        if (strH != 0)
        {
            // スレ番号を受信したときのダイアログを表示する
            FrmCustomAlert(ALTID_WARN, MemHandleLock(strH), "", "");
        }
        MemHandleUnlock(strH);
    }

    // 領域の開放
    MEMFREE_PTR(token);
    MEMFREE_PTR(dbName);
    MEMFREE_PTR(logP);
    MEMFREE_PTR(buff);
    return (false);
}

/*=========================================================================*/
/*   Function :   checkLaunchCode                                          */
/*                                                       起動コードの確認  */
/*=========================================================================*/
Boolean checkLaunchCode(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    switch (cmd)
    {
      default:
        // 起動コードのチェック...
        break;
    }

    // これ以外の起動コード...

    // とりあえずは、ビーム受信登録をする...
    ExgRegisterData(SOFT_CREATOR_ID, exgRegExtensionID, NNSI_EXCHANGEINFO_SUFFIX); 
    return (false);
}
