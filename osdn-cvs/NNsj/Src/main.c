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

/*=========================================================================*/
/*   Function :  MyGetFormID                                               */
/*                                        オープンするフォームIDを応答する */
/*=========================================================================*/
UInt16 GetFormID_NNsh(UInt16 frmID)
{
    UInt16 formID;

    switch (frmID)
    {
#if 0
      case NNSH_FRMID_CONFIG_NNSH:
        formID = FRMID_CONFIG_NNSH;
        break;
#endif
      case NNSH_FRMID_MAIN:
      default:
        formID = FRMID_MAIN;
        break;
    }
    return (formID);
}

/*=========================================================================*/
/*   Function :   MyApplicationDispatchEvent                               */
/*                                                特殊ディスパッチイベント */
/*=========================================================================*/
Boolean MyApplicationDispatchEvent(EventType *event)
{
    //  "ハードキー制御する"　指定があり、かつキー入力イベントなら
    // フォームの処理を実施する
    if ((NNshParam->useHardKeyControl != 0)&&(event->eType == keyDownEvent))
    {
        if (event->data.keyDown.chr >= vchrHardKeyMin)
        {
            // ハードキーコードの確認
            switch (event->data.keyDown.chr)
            {
              case vchrHard1:
                // ハードキー１
                event->data.keyDown.chr = NNshParam->useKey.key1;
                break;

              case vchrHard2:
                // ハードキー２
                event->data.keyDown.chr = NNshParam->useKey.key2;
                break;

              case vchrHard3:
                // ハードキー３
                event->data.keyDown.chr = NNshParam->useKey.key3;
                break;

              case vchrHard4:
                // ハードキー４
                event->data.keyDown.chr = NNshParam->useKey.key4;
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

/*=========================================================================*/
/*   Function :   MyApplicationHandleEvent                                 */
/*                                                                         */
/*=========================================================================*/
Boolean MyApplicationHandleEvent(EventType *event)
{
    FormType  *frm;
    UInt16     fieldFont, objFont;

    // SONYシルク制御を行う場合の画面更新
#ifdef USE_CLIE
    if ((NNshGlobal->silkRef != 0)&&
        (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY))
    {
         // SILKマネージャの表示にあわせた画面の更新を行う
        frm = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm, true);
        NNshGlobal->updateHR = NNSH_NOT_EFFECTIVE;
    }
#endif

    // ここで、(フォームロード等の)イベント受信時の処理を記述する。
    switch (event->eType)
    {
      // フォームを開いたよevent
      case frmLoadEvent:
        // 使用フォントを設定する
        fieldFont = NNshParam->currentFont;
        FntSetFont(fieldFont);
        objFont   = NNshParam->currentFont;

        // フォームを初期化 
        frm = FrmInitForm(event->data.frmLoad.formID);
#ifdef USE_HANDERA
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaFormModify(frm, vgaFormModify160To240);
        }
#endif
#ifdef USE_CLIE
        NNshGlobal->tempStatus = 0;
#endif
        // フォームを開いてイベントハンドラを設定する
        FrmSetActiveForm(frm);
        switch (event->data.frmLoad.formID)
        {
          // メインフォーム
          case FRMID_MAIN:
            // グラフィティエリアを最小化して画面をリサイズ、フォントを変更
            (void) NNshSilkMinimize(frm);

            // フォームを開いて、イベントハンドラを設定する
            OpenForm_MainForm(frm);
            FrmSetEventHandler(frm, Handler_MainForm);
            break;
#if 0
          // NNsi設定画面
          case FRMID_CONFIG_NNSH:
            // グラフィティ領域を最大化、フォームを開いてイベントハンドラを設定
            (void) NNshSilkMaximize(frm);
            OpenForm_NNshSetting(frm);
            FrmSetEventHandler(frm, Handler_NNshSetting);
            break;
#endif

          default:
            // 何の"フォーム開いたよイベント"か表示してみる。(デバッグ)
            NNsh_DebugMessage(ALTID_INFO, "frmLoadEvent()", " formID:",
                              event->data.frmLoad.formID);
            break;
        }
        // 画面を描画する
        //　NNsi_FrmDrawForm(frm, true);
        break;
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
UInt16 MyStartApplication(void)
{
    Int16                   ret;
    UInt16                  size;
    NNshSoftwareVersionInfo verInfo;
#ifdef USE_ARMLET
    UInt32                  processorType;
#endif
    UInt32                  osVersion;

    // NNsi設定バージョンの確認
    //   (Version確認用Preferenceを確認し、変更がなければ復旧)
    size = sizeof(NNshSoftwareVersionInfo);
    MemSet(&verInfo, sizeof(NNshSoftwareVersionInfo), 0x00);
    ret  = PrefGetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, 
                                 &verInfo, &size, false);

    // NNsi設定用格納領域の確保
    size = sizeof(NNshSavedPref) + DBSIZE_SETTING;
    NNshParam = (NNshSavedPref *) MemPtrNew(size);
    if (NNshParam == NULL)
    {
        // 領域確保に失敗！(NNsiは起動しない)
        NNsh_ErrorMessage(ALTID_ERROR,MSG_CANNOT_LAUNCH,"(Prm)\nsize:",size);
        return (0);
    }        
    MemSet(NNshParam, size, 0x00);

    // NNsiグローバル領域用格納領域の確保
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // 領域確保に失敗！(NNsiは起動しない)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_CANNOT_LAUNCH,"(Gbl)\nsize:", size);
        return (0);
    }        
    MemSet(NNshGlobal, size, 0x00);

    // OSバージョンの確認
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &osVersion) == errNone)
    {
        NNshGlobal->palmOSVersion = osVersion;
    }

    // NNsi設定を復旧させる
    (void) RestoreSetting_NNsh(ret, verInfo.prefSize, NNshParam);

    // デバッグモードの情報を復旧させる
    NNshParam->debugMessageON = verInfo.debugMode;

    // VFS機能がない設定だった場合には、一旦VFSパラメータをクリアする。
    if (NNshParam->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        NNshParam->useVFS = NNSH_VFS_DISABLE;
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

#ifdef USE_HIGHDENSITY
    // 高解像度サポート/カラーサポートのチェック
    SetDoubleDensitySupport_NNsh();
#else
    // 高解像度サポート設定を初期化
    NNshGlobal->os5HighDensity = (UInt32) kDensityLow;
#endif    // #ifdef USE_HIGHDENSITY

#ifdef USE_TSPATCH
    // TsPatch対応コード...(TsPatch適用中かどうかチェックする)
    if (NNshParam->notUseTsPatch == 0)
    {
        ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
        if (ret == errNone)
        {    
            // TsPatch適用中...使用フォントを小さくする。
            switch (NNshParam->currentFont)
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
                fontId = NNshParam->currentFont;
                break;
            }
            NNshParam->currentFont = fontId;
        }
    }
    else
    {
        // TsPatch機能を使わない場合,,,
        switch (NNshParam->currentFont)
        {
          case stdFont:
          case boldFont:
          case largeFont:
          case largeBoldFont:
            // そのまま使う
            break;

          default:
            // 標準フォントに変更する
            NNshParam->currentFont = stdFont;
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
        // NNshParam->useARMlet = NNSH_ENABLE;  // ARMletは使用する設定に変更
    }
    else
#endif
    {
        // ARMletは使用しない設定にする
        NNshParam->useARMlet = NNSH_DISABLE;
    }

#if 0
    // VFSがOFFだった場合には、確認メッセージを表示する。
    if (NNshParam->useVFS == NNSH_VFS_DISABLE)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_USE_VFS_WARNING, "", 0);
    }
#endif

    // 格納バッファ領域を取得する
    NNshGlobal->maxScriptLine = MAX_MACRO_STEP;
    NNshGlobal->scriptArea    = AllocateMacroDBData(NNshGlobal->maxScriptLine);
    if (NNshGlobal->scriptArea != NULL)
    {
        // DBからNNsi設定で記録されているスクリプトを呼び出す
        ret = ReadMacroDBData(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT,
                              DATA_CREATOR_ID, 0,
                              &(NNshGlobal->currentScriptLine), 
                              NNshGlobal->maxScriptLine,
                              NNshGlobal->scriptArea);
    }
    NNshGlobal->editFlag = false;
    NNshGlobal->editMode = NNSH_MODE_NORMAL;

    // 最初にオープンするフォームのIDを応答する。
    return (GetFormID_NNsh(NNshParam->lastFrmID));
}

/*=========================================================================*/
/*   Function :   myStopApplication                                        */
/*                                 ストップ時の(プログラム個別)初期化処理  */
/*=========================================================================*/
void MyStopApplication(void)
{
    NNshSoftwareVersionInfo verInfo;
    UInt16                  savedParam;
    
    savedParam = NNshParam->confirmationDisable;
    if (NNshGlobal->editFlag == true)
    {
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_OUTPUT_MACRO, "", 0) == 0)
        {
            // マクロ設定データを出力する
            NNshParam->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            extractMacroData();
        }
    }
    NNshParam->confirmationDisable = savedParam;

    // スクリプト領域の開放
    if (NNshGlobal->scriptArea != NULL)
    {
        ReleaseMacroDBData(NNshGlobal->scriptArea);
    }

#ifdef USE_CLIE
    // CLIEサポートを開放する
    ResetCLIESupport_NNsh();
#endif

    // NNsi設定のVersion確認用Preferenceの保存(Debugモードはこちらで記憶)
    verInfo.NNsiVersion = SOFT_PREFVERSION;
    verInfo.prefSize    = sizeof(NNshSavedPref);
    verInfo.prefType    = NNSH_PREFERTYPE;
    verInfo.debugMode   = (NNshParam != NULL) ? NNshParam->debugMessageON : 0;
    PrefSetAppPreferences(SOFT_CREATOR_ID, SOFT_PREFID, SOFT_PREFVERSION,
                          &verInfo, sizeof(NNshSoftwareVersionInfo), false);

    // NNsi設定の保存(NNsiの起動に成功した時のみ)
    if (NNshParam != NULL)
    {
        SaveSetting_NNsh(NNSH_PREFERTYPE, sizeof(NNshSavedPref), NNshParam);
        MemPtrFree(NNshParam);
        NNshParam = NULL;
    }

    // NNsiのグローバル領域を開放する
    MEMFREE_PTR(NNshGlobal);
    return;
}
