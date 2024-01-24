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

#include "machine.c"

// external functions
extern Err     OpenForm_NNshMessage (FormType  *frm);
extern void    OpenForm_ThreadMain  (FormType  *frm);
extern void    OpenForm_WriteMessage(FormType  *frm);
extern Err     OpenForm_SelectBBS   (FormType  *frm);
extern Err     OpenForm_NNshSetting (FormType  *frm);
extern Err     OpenForm_NNsiSetting2(FormType  *frm);
extern Err     OpenForm_NNsiSetting3(FormType  *frm);
extern Err     OpenForm_NNsiSetting4(FormType  *frm);

extern Boolean Handler_ThreadMain   (EventType *event);
extern Boolean Handler_NNshMessage  (EventType *event);
extern Boolean Handler_WriteMessage (EventType *event);
extern Boolean Handler_SelectBBS    (EventType *event);
extern Boolean Handler_NNshSetting  (EventType *event);

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
#ifdef USE_CLIE
    if ((NNshGlobal->silkRef != 0)&&
        (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY))
    {
         // SILKマネージャの表示にあわせた画面の更新を行う
        frm = FrmGetActiveForm();
        FrmEraseForm(frm);
        NNsi_FrmDrawForm(frm);
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
          // スレ一覧画面
          case FRMID_THREAD:
            // グラフィティエリアを最小化して画面をリサイズ、フォントを変更
            (void) NNshSilkMinimize(frm);
#ifdef USE_GLUE
            LstGlueSetFont(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, 
                                           LSTID_SELECTION_THREAD)), objFont);
#endif
            // フォームを開いて、イベントハンドラを設定する
            OpenForm_ThreadMain(frm);
            FrmSetEventHandler(frm, Handler_ThreadMain);
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
            return (false);
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
            (void) NNshSilkMaximize(frm);

            // フォームを開いて、イベントハンドラを設定する
            OpenForm_WriteMessage(frm);
            FrmSetEventHandler(frm, Handler_WriteMessage);
            break;

          // BBS管理画面
          case FRMID_MANAGEBBS:
            // グラフィティエリアを最小化して画面をリサイズする
            if (NNshSilkMinimize(frm) == errNone)
            {
                // 領域サイズを求める
                tableP = FrmGetObjectPtr(frm, 
                                 FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
                for (rowNum = 0; rowNum < TblGetNumberOfRows(tableP); rowNum++)
                {
                    TblSetRowHeight(tableP, rowNum, 15);
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

          default:
            // 何の"フォーム開いたよイベント"か表示してみる。(デバッグ)
            NNsh_DebugMessage(ALTID_INFO, "frmLoadEvent()", " formID:",
                              event->data.frmLoad.formID);
            break;
        }
        // 画面を描画する
        NNsi_FrmDrawForm(frm);
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
/*   Function :   myStartApplication                                       */
/*                                 スタート時の(プログラム個別)初期化処理  */
/*=========================================================================*/
static UInt16 MyStartApplication(void)
{
    Int16                   ret;
    UInt16                  size;
    NNshSoftwareVersionInfo verInfo;
#ifdef USE_ARMLET
    UInt32                  processorType;
#endif

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
        NNsh_ErrorMessage(ALTID_ERROR, MSG_NNSI_CANNOT_LAUNCH,
                          "(NNsiSet)\nsize:", size);
        return (0);
    }
    MemSet(NNshParam, size, 0x00);

    // NNsiグローバル領域用格納領域の確保
    size       = sizeof(NNshWorkingInfo) + MARGIN;
    NNshGlobal = (NNshWorkingInfo *) MemPtrNew(size);
    if (NNshGlobal == NULL)
    {
        // 領域確保に失敗！(NNsiは起動しない)
        NNsh_ErrorMessage(ALTID_ERROR, MSG_NNSI_CANNOT_LAUNCH,
                          "(NNsiGbl)\nsize:", size);
        return (0);
    }        
    MemSet (NNshGlobal, size, 0x00);
    StrCopy(NNshGlobal->dummyBuffer, " ");

    // NNsi設定を復旧させる
    (void) NNshRestoreNNsiSetting(ret, verInfo.prefSize, NNshParam);

    // デバッグモードの情報を復旧させる
    NNshParam->debugMessageON = verInfo.debugMode;

    // VFS機能がない設定だった場合には、一旦VFSパラメータをクリアする。
    if (NNshParam->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        NNshParam->useVFS = NNSH_VFS_DISABLE;
    }
    // VFSサポートの設定
    checkVFSdevice_NNsh();

#ifdef USE_CLIE
    // Silk制御の設定を反映
    NNshGlobal->notUseSilk = NNshParam->notUseSilk;

    // CLIEサポートの設定
    setCLIESupport_NNsh();
#endif  // #ifdef USE_CLIE

#ifdef USE_HANDERA
    // HandEraサポートの設定
    setHandEraSupport_NNsh();
#endif // #ifdef USE_HANDERA

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

    // WebBrowserのインストールチェック
    checkInstalledWebBrowser();

    // VFSがOFFだった場合には、確認メッセージを表示する。
    if (NNshParam->useVFS == NNSH_VFS_DISABLE)
    {
        NNsh_InformMessage(ALTID_INFO, MSG_USE_VFS_WARNING1,
                           MSG_USE_VFS_WARNING2, 0);
    }

    // しおり設定されていた場合、開く場所を指定
    if ((NNshParam->useBookmark != 0)&&(NNshParam->lastFrmID == FRMID_MESSAGE))
    {
        NNshParam->openMsgIndex = NNshParam->bookMsgIndex;
    }

    // 最初にオープンするフォームのIDを応答する。
    return (NNshParam->lastFrmID);
}

/*=========================================================================*/
/*   Function :   myStopApplication                                        */
/*                                 ストップ時の(プログラム個別)初期化処理  */
/*=========================================================================*/
static void MyStopApplication(void)
{
    UInt16                  savedParam;
    NNshSoftwareVersionInfo verInfo;

    // テンポラリファイルの削除(DEBUGモード時には残しておく)
    if (NNshParam->debugMessageON == 0)
    {
        (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
        (void) DeleteFile_NNsh(FILE_RECVMSG, NNSH_VFS_DISABLE);
    }

    // NNsi終了時に回線を切断する
    if (NNshParam->disconnectNNsiEnd != 0)
    {
        // プログラム終了時には、「回線切断」の確認メッセージは出さない。
        savedParam = NNshParam->confirmationDisable;
        NNshParam->confirmationDisable = 1;
        NNshNet_LineHangup();
        NNshParam->confirmationDisable = savedParam;
    }

#ifdef USE_CLIE
    // CLIEサポートを開放する
    resetCLIESupport_NNsh();
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
        // Silk制御の設定を反映
#ifdef USE_CLIE
        NNshParam->notUseSilk = NNshGlobal->notUseSilk;
#else
        NNshParam->notUseSilk = 0;
#endif
        NNshSaveNNsiSetting(NNSH_PREFERTYPE, sizeof(NNshSavedPref), NNshParam);

        // NNsi終了時にデータベースをVFSにバックアップする
        if ((NNshParam->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            BackupDBtoVFS(NNSH_DISABLE);
        }
    }

    // NNsiのグローバル領域を開放する
    if (NNshGlobal != NULL)
    {
        NNshClearGlobal(NNshGlobal);
        MEMFREE_PTR(NNshGlobal);
        NNshGlobal = NULL;
    }

    // 使用領域の開放
    MEMFREE_PTR(NNshParam);
    NNshParam = NULL;

    return;
}
