/*============================================================================*
 *  FILE: 
 *     main.c
 *
 *  Description: 
 *     main source code for Palm application.
 *     (This file includes in "skeleton.c".)
 *
 *===========================================================================*/
#define MAIN_C

/********** Include Files **********/
#include "local.h"

// external functions
extern Err     OpenForm_MainForm    (FormType           *frm);
extern Err     OpenForm_NNshSetting (FormType           *frm);
extern Boolean Handler_MainForm     (EventType          *event);
extern Boolean Handler_NNshSetting  (EventType          *event);
extern Err     VFSUnmountNotify     (SysNotifyParamType *notifyParamsP);
extern Err     VFSMountNotify       (SysNotifyParamType *notifyParamsP);
extern void   SetDoubleDensitySupport_NNsh(void);
extern void   ResetVFSNotifications(void);

/*=========================================================================*/
/*   Function :   MyApplicationDispatchEvent                               */
/*                                                特殊ディスパッチイベント */
/*=========================================================================*/
Boolean MyApplicationDispatchEvent(EventType *event)
{
    NNshSavedPref *NNsiParam;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    //  "ハードキー制御する"　指定があり、かつキー入力イベントなら
    // フォームの処理を実施する
    if ((NNsiParam->useHardKeyControl != 0)&&(event->eType == keyDownEvent))
    {
        if (event->data.keyDown.chr >= vchrHardKeyMin)
        {
            // ハードキーコードの確認
            switch (event->data.keyDown.chr)
            {
              case vchrHard1:
                // ハードキー１
                event->data.keyDown.chr = NNsiParam->useKey.key1;
                break;

              case vchrHard2:
                // ハードキー２
                event->data.keyDown.chr = NNsiParam->useKey.key2;
                break;

              case vchrHard3:
                // ハードキー３
                event->data.keyDown.chr = NNsiParam->useKey.key3;
                break;

              case vchrHard4:
                // ハードキー４
                event->data.keyDown.chr = NNsiParam->useKey.key4;
                break;

              default:
                //  （知らない）ハードキー入力が行われたとき、キーコードを
                // 表示する
                NNsh_DebugMessage(ALTID_INFO, "Hard Key Event ", " chrID:",
                                  event->data.keyDown.chr);
                break;
            }
        }
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   MyFrmLoadEvent                                           */
/*                                               フォームオープン時の処理  */
/*-------------------------------------------------------------------------*/
static Boolean MyFrmLoadEvent(EventType *event)
{
    FormType  *frm;
    UInt16     fieldFont, objFont;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // 使用フォントを設定する
    fieldFont = NNsiParam->currentFont;
    FntSetFont(fieldFont);
    objFont   = NNsiParam->currentFont;

    // フォームを初期化 
    frm = FrmInitForm(event->data.frmLoad.formID);
#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(frm, vgaFormModify160To240);
    }
#endif
#ifdef USE_PIN_DIA
        if (NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)
        {
            VgaFormModify_DIA(frm, vgaFormModifyNormalToWide);
        }
        else if (NNsiGlobal->device == NNSH_DEVICE_GARMIN)
        {
            // 画面をオープンしたときは、再描画する(Garmin機)
            VgaFormModify_DIA(frm, vgaFormModifyNormalToWide);
            NNsiGlobal->notifyUpdate = NNSH_DISP_RESIZED;
            EvtWakeup();
        }
#endif
#ifdef USE_CLIE
    NNsiGlobal->tempStatus = 0;
#endif

    // フォームを開いてイベントハンドラを設定する
    FrmSetActiveForm(frm);

    // 画面を描画する
#ifdef USE_DEBUG_ROM
    NNsi_FrmDrawForm(frm, true);
#endif

    switch (event->data.frmLoad.formID)
    {
      // メインフォーム
      case FRMID_MAIN:
        // グラフィティエリアをどうするか、、、
        if (NNsiParam->silkMax == 0)
        {
            // グラフィティエリアを最小化
            (void) NNshSilkMinimize(frm);
        }
        else
        {
            // グラフィティエリアを最大化
            (void) NNshSilkMaximize(frm);
        }
        FldSetFont(FrmGetObjectPtr(frm,
                             FrmGetObjectIndex(frm, FLDID_SCREEN)), fieldFont);

        // フォームを開いて、イベントハンドラを設定する
        OpenForm_MainForm(frm);
        FrmSetEventHandler(frm, Handler_MainForm);
        break;

      default:
        // 何の"フォーム開いたよイベント"か表示してみる。(デバッグ)
        NNsh_DebugMessage(ALTID_INFO, "frmLoadEvent()", " formID:",
                          event->data.frmLoad.formID);
        break;
    }

    // 画面を描画する
    NNsi_FrmDrawForm(frm, true);
    return (false);
}


/*=========================================================================*/
/*   Function :   MyApplicationHandleEvent                                 */
/*                                                                         */
/*=========================================================================*/
Boolean MyApplicationHandleEvent(EventType *event)
{
    FormType  *frm;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // SONYシルク制御を行う場合の画面更新
#ifdef USE_CLIE
    if ((NNsiGlobal->silkRef != 0)&&
        (NNsiGlobal->updateHR == NNSH_UPDATE_DISPLAY))
    {
         // SILKマネージャの表示にあわせた画面の更新を行う
        frm = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm, true);
        NNsiGlobal->updateHR = NNSH_NOT_EFFECTIVE;
    }
#endif

    // Notifyイベントの受信処理を通過した時
    if (NNsiGlobal->notifyUpdate != 0)
    {
        frm = FrmGetActiveForm();
        if (frm != NULL)
        {
            switch (NNsiGlobal->notifyUpdate)
            {
              case NNSH_VFS_MOUNTED:
                // VFSがmountされたことをユーザに通知する
                NNsh_InformMessage(ALTID_WARN, "Mounted VFS", "", 0);
                break;

              case NNSH_VFS_UNMOUNTED:
                // VFSがunmountされたことをユーザに通知する
                NNsh_InformMessage(ALTID_WARN, "Unmounted VFS", "", 0);
                break;

              case NNSH_DISP_RESIZED:
                // 画面のサイズが変更になった...
                HandEraResizeForm(frm);
                NNsh_DebugMessage(ALTID_INFO, "[[<<DISPLAY RESIZED>>]]", "", 0);

                FrmEraseForm(frm);
                NNsi_FrmDrawForm(frm, true);
                break;

              default:
                break;
            }
        }
        NNsiGlobal->notifyUpdate = 0;
    }


    // ここで、(フォームロード等の)イベント受信時の処理を記述する。
    switch (event->eType)
    {
      // フォームを開いたよevent
      case frmLoadEvent:
        (void) MyFrmLoadEvent(event);
        break;

#ifdef USE_PIN_DIA
      case winEnterEvent:
        if ((NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNsiGlobal->device == NNSH_DEVICE_GARMIN))
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
UInt32  MyApplicationHandleNotifications(SysNotifyParamType *notifyParamsP)
{ 
  // PilotMainが持てるCode Resourceの場合はこっちでNotificationを
  // 処理するのが一般的、持てない場合はSysNotifyRegisterでfunction pointerを
  // 渡してやる

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
UInt16 MyStartApplication(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
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
    NNshSavedPref          *nnsiParam;     // NNsi設定
    NNshWorkingInfo        *nnsiGlobal;    // NNsiグローバル設定

    // NNsi設定バージョンの確認
    //   (Version確認用Preferenceを確認し、変更がなければ復旧)
    size = sizeof(NNshSoftwareVersionInfo);
    MemSet(&verInfo, sizeof(NNshSoftwareVersionInfo), 0x00);
    ret  = PrefGetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, 
                                 &verInfo, &size, false);

    // NNsi設定用格納領域の確保
    size = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    nnsiParam = (NNshSavedPref *) MemPtrNew(size);
    if (nnsiParam == NULL)
    {
        // 領域確保に失敗！(NNsiは起動しない)
        NNsh_ErrorMessage(ALTID_ERROR,MSG_CANNOT_LAUNCH,"(Prm)\nsize:",size);
        return (0);
    }        
    MemSet(nnsiParam, size, 0x00);

    // pointerをfeatureにセット
    FtrSet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32)nnsiParam);

    // NNsiグローバル領域用格納領域の確保
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    nnsiGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (nnsiGlobal == NULL)
    {
        // 領域確保に失敗！(NNsiは起動しない)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_CANNOT_LAUNCH,"(Gbl)\nsize:", size);
        return (0);
    }        
    MemSet(nnsiGlobal, size, 0x00);

    // pointerをfeatureにセット
    FtrSet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32)nnsiGlobal);

    // OSバージョンの確認
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osVersion) == errNone)
    {
        nnsiGlobal->palmOSVersion = osVersion;
    }

    // NNsi設定を復旧させる
    (void) RestoreSetting_NNsh(ret, verInfo.prefSize, nnsiParam);

    // デバッグモードの情報を復旧させる
    nnsiParam->debugMessageON = verInfo.debugMode;


    // VFS機能がない設定だった場合には、一旦VFSパラメータをクリアする。
    if (nnsiParam->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        nnsiParam->useVFS = NNSH_VFS_DISABLE;
    }
    // VFSサポートの設定
    CheckVFSdevice_NNsh();

#ifdef USE_CLIE
    // CLIEサポートの設定
    SetCLIESupport_NNsh();
#endif  // #ifdef USE_CLIE

#ifdef USE_HANDERA
    // HandEraサポートの設定
    SetHandEraSupport_NNsh();
#endif // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
    // pen input managerの設定
    SetDIASupport_NNsh();
#endif    // #ifdef USE_PIN_DIA

#ifdef USE_HIGHDENSITY
    // 高解像度サポート/カラーサポートのチェック
    SetDoubleDensitySupport_NNsh();
#else
    // 高解像度サポート設定を初期化
    nnsiGlobal->os5HighDensity = (UInt32) kDensityLow;
#endif    // #ifdef USE_HIGHDENSITY

#ifdef USE_TSPATCH
    // TsPatch対応コード...(TsPatch適用中かどうかチェックする)
    if (nnsiParam->notUseTsPatch == 0)
    {
        ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
        if (ret == errNone)
        {    
            // TsPatch適用中...使用フォントを小さくする。
            switch (nnsiParam->currentFont)
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
                fontId = nnsiParam->currentFont;
                break;
            }
            nnsiParam->currentFont = fontId;
        }
    }
    else
    {
        // TsPatch機能を使わない場合,,,
        switch (nnsiParam->currentFont)
        {
          case stdFont:
          case boldFont:
          case largeFont:
          case largeBoldFont:
            // そのまま使う
            break;

          default:
            // 標準フォントに変更する
            nnsiParam->currentFont = stdFont;
            break;
        }
    }
#endif  // #ifdef USE_TSPATCH

    // CLIE smallフォントを使用する
    if (nnsiParam->sonyHRFont != 0)
    {
        // 現物あわせ、、、
        nnsiParam->currentFont = 52;
    }

#ifdef USE_ARMLET
    // ARMlet使用可否のチェック
    FtrGet(sysFileCSystem, sysFtrNumProcessorID, &processorType);
    if (sysFtrNumProcessorIsARM(processorType))
    {
        // ARMlet使用可能デバイス。
        // nnsiParam->useARMlet = NNSH_ENABLE;  // ARMletは使用する設定に変更
    }
    else
#endif
    {
        // ARMletは使用しない設定にする
        nnsiParam->useARMlet = NNSH_DISABLE;
    }

    // VFSがOFFだった場合には、確認メッセージを表示する。
    if (nnsiParam->useVFS == NNSH_VFS_DISABLE)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_USE_VFS_WARNING, "", 0);
    }

#ifdef USE_UTF8
    // UTF8のデータテーブルを設定する
    OpenUtf8Database(&(nnsiGlobal->jis2unicodeDB), &(nnsiGlobal->unicode2jisDB));
#endif

    // 起動コードの確認
    switch (cmd)
    {
      case kaniEditCmdOpenFileStream:
        // ファイルストリームを開く
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_STREAM;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, DATAFILE_PREFIX, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFS:
        // VFSファイルを開く
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSdos:
        // VFSファイルを開く(DOS)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_DOS;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSmac:
        // VFSファイルを開く(MAC)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_MAC;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSjis:
        // VFSファイルを開く(JIS)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_JIS;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSeuc:
        // VFSファイルを開く(EUC)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_EUC;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSutf8:
        // VFSファイルを開く(UTF8)
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_UTF8;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenVFSreadOnly:
        // VFSファイルを読み込み専用で開く
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_VFS_READONLY;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      case kaniEditCmdOpenFileStreamReadOnly:
        // ファイルストリームを読み込み専用で開く
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_OPENED_STREAM_READONLY;
        StrNCopy(nnsiParam->fileName, cmdPBP, MAXLENGTH_FILENAME);
        StrNCopy(nnsiGlobal->dirName, cmdPBP, MAXLENGTH_FILENAME);
        break;

      default:
        // 何もしない
        nnsiGlobal->autoOpenFile = FILEMGR_STATE_DISABLED;
        StrNCopy(nnsiGlobal->dirName, DATAFILE_PREFIX, MAXLENGTH_FILENAME);
        break;
    }

    // デフォルトディレクトリの設定
    size = StrLen(nnsiGlobal->dirName) - 1;
    while (size > 0)
    {
        if (nnsiGlobal->dirName[size] == '/')
        {
            nnsiGlobal->dirName[size + 1] = '\0';
            break;
        }
        size--;
    }

    // デフォルトディレクトリ未指定だった場合
    if (nnsiGlobal->dirName[0] != '/')
    {
        StrCopy(nnsiGlobal->dirName, DATAFILE_PREFIX);
    }

    // 最初にオープンするフォームのIDを応答する。
    return (FRMID_MAIN);
}

/*=========================================================================*/
/*   Function :   myStopApplication                                        */
/*                                 ストップ時の(プログラム個別)初期化処理  */
/*=========================================================================*/
void MyStopApplication(void)
{
    UInt16                  savedParam;
    Char                   *txtP;
    FieldType              *fldP;
    FormType               *prevFrm;
    MemHandle               txtH;
    NNshSoftwareVersionInfo verInfo;

    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    savedParam = NNsiParam->confirmationDisable;
    NNsiParam->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

    // メインフォームが開いているとき
    if (FrmGetActiveFormID() == FRMID_MAIN)
    {
        // データ保存ダイアログを開く (パラメータ設定時)
        if (NNsiParam->useEndSaveDialog != 0)
        {
            (void) NNsh_FileData(BTNID_DATA_SAVE);
        }

        // 前回のファイルを復帰する設定の場合、、、
        if (NNsiParam->autoRestore != 0)
        {
            prevFrm = FrmGetActiveForm();
            fldP = FrmGetObjectPtr(prevFrm, FrmGetObjectIndex(prevFrm, FLDID_SCREEN));
            txtH = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                txtP = MemHandleLock(txtH);
                if ((txtP != NULL)&&(*txtP != '\0'))
                {
                    // データの保管
                    (void) DataSaveToFile(prevFrm, txtP, FILE_TEMPFILE, 0, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_LF);
                }
                MemHandleUnlock(txtH);
            }
        }
    }

    // UTF8のデータテーブルをクローズする
#ifdef USE_UTF8
    CloseUtf8Database(NNsiGlobal->jis2unicodeDB, NNsiGlobal->unicode2jisDB);
#endif

    NNsiParam->confirmationDisable = savedParam;

    // NNsi設定のVersion確認用Preferenceの保存(Debugモードはこちらで記憶)
    verInfo.NNsiVersion = SOFT_PREFVERSION;
    verInfo.prefSize    = sizeof(NNshSavedPref);
    verInfo.prefType    = NNSH_PREFERTYPE;
    verInfo.debugMode   = (NNsiParam != NULL) ? NNsiParam->debugMessageON : 0;
    PrefSetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, SOFT_PREFVERSION,
                          &verInfo, sizeof(NNshSoftwareVersionInfo), false);

    // NNsi設定の保存(NNsiの起動に成功した時のみ)
    if (NNsiParam != NULL)
    {
        SaveSetting_NNsh(NNSH_PREFERTYPE, sizeof(NNshSavedPref), NNsiParam);
        MemPtrFree(NNsiParam);
        NNsiParam = NULL;
    }

#ifdef USE_CLIE
    // CLIEサポートを開放する
    ResetCLIESupport_NNsh();
#endif

#ifdef USE_PIN_DIA
    // pen input managerの設定解除
    ResetDIASupport_NNsh();
#endif    // #ifdef USE_PIN_DIA

    // VFSのNotifyを終了させる
    ResetVFSNotifications();

    // NNsiのグローバル領域を開放する
    if (NNsiGlobal != NULL)
    {
        MemPtrFree(NNsiGlobal);
        NNsiGlobal = NULL;
    }

    // featureを解放する
    FtrUnregister(SOFT_CREATOR_ID, FTRID_GLOBALPTR);
    FtrUnregister(SOFT_CREATOR_ID, FTRID_PARAMPTR);

    return;
}

/*=========================================================================*/
/*   Function :   checkLaunchCode                                          */
/*                                                       起動コードの確認  */
/*=========================================================================*/
Boolean checkLaunchCode(UInt16 cmd)
{

    switch (cmd)
    {
      case kaniEditCmdOpenFileStreamReadOnly:
      case kaniEditCmdOpenFileStream:
      case kaniEditCmdOpenVFSreadOnly:
      case kaniEditCmdOpenVFS:
      case kaniEditCmdOpenVFSdos:
      case kaniEditCmdOpenVFSmac:
      case kaniEditCmdOpenVFSjis:
      case kaniEditCmdOpenVFSeuc:
      case kaniEditCmdOpenVFSutf8:
        // KaniEdit用起動コード...
        return (true);
        break;

      default:
        break;
    }

    // これ以外の起動コード...
    return (false);
}
