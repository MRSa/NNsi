/*============================================================================*
 *  FILE: 
 *     nnshdmmy.c
 *
 *  Description: 
 *     NNsh dummy Display.
 *
 *===========================================================================*/
#define NNSHDMMY_C
#include "local.h"

extern UInt16 ConvertFormID(UInt16 lastFrmID);

/*=========================================================================*/
/*   Function :   OpenForm_NNsiDummy                                       */
/*                                             NNsi起動(DUMMY)画面オープン */
/*=========================================================================*/
Err OpenForm_NNsiDummy(FormType *frm)
{
    Err          ret;
    Char         *osVer, buffer[BIGBUF];
    UInt8        remainPer;
    UInt32       useSize, totalSize, freeSize;

    // 出力バッファをクリアする
    MemSet (buffer, sizeof(buffer), 0x00);

    // 現在開いている画面をダミー画面とする(前回開いていた画面をwork1に記録)
    if ((NNshGlobal->NNsiParam)->lastFrmID != NNSH_FRMID_DUMMY)
    {
        NNshGlobal->work1    = (NNshGlobal->NNsiParam)->lastFrmID;
    }
    else
    {
        NNshGlobal->work1    = NNSH_FRMID_THREAD;
    }
    (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_DUMMY;

    // OSのバージョンを出力する
    osVer = SysGetOSVersionString();
    StrCat (buffer, "PalmOS ");
    StrCat (buffer, osVer);
    MEMFREE_PTR(osVer);

    // 現在のバッテリ残量をバッファに出力する
    remainPer      = 0;
    SysBatteryInfo(false, NULL, NULL, NULL, NULL, NULL, &remainPer);
    StrCat (buffer, "\n---\nBattery :");
    NUMCATI(buffer, (UInt16) remainPer); 
    StrCat (buffer, "%");

    // 空きメモリ情報の取得
    freeSize = GetOSFreeMem(&totalSize, &useSize);
    {
        // VFSの容量(空き/最大)を出力する
        StrCat (buffer, "\nMemory(Free/Total)\n    ");
        NUMCATI(buffer, ((freeSize)/1024));
        StrCat (buffer, "kB/");
        NUMCATI(buffer, ((totalSize/1024))); 
        StrCat (buffer, "kB");
    }

    // VFS情報の取得
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFSの容量を確認する
        ret = VFSVolumeSize(NNshGlobal->vfsVol, &useSize, &totalSize);
        if (ret == errNone)
        {
            // VFSの容量(空き/最大)を出力する
            StrCat (buffer, "\nVFS(Free/Total)\n    ");
            NUMCATI(buffer, ((totalSize - useSize)/1024));
            StrCat (buffer, "kB/");
            NUMCATI(buffer, ((totalSize/1024))); 
            StrCat (buffer, "kB");
        }
    }
    
    // デバイス情報を表示する
    NNshWinSetFieldText(frm, FLDID_DUMMYMSG, true, buffer, StrLen(buffer));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   Hander_NNsiDummy                                         */
/*                                  NNsi起動(DUMMY)画面のイベントハンドラ  */
/*=========================================================================*/
Boolean Handler_NNsiDummy(EventType *event)
{
    Err        ret;
    UInt16     itemId, len, savedParam;
    Char       kwBuf[BUFSIZE + MARGIN], *buffer;
    EventType *dummyEvent;

    buffer = NULL;
    // ボタンチェック以外のイベントは即戻り
    switch (event->eType)
    { 
      case menuEvent:
        // メニュー選択
        itemId = event->data.menu.itemID;
        break;

      case ctlSelectEvent:
        // ボタンが押された(次へ進む)
        itemId = event->data.ctlSelect.controlID;
        break;

      case keyDownEvent:
        // キー入力があった(次へすすむ)
        itemId = KeyConvertFiveWayToJogChara(event);
        break;

      default: 
        return (false);
        break;
    }

    switch (itemId)
    {
      case MNUID_NNSI_WELCOME:
      case SELTRID_NNSI_DUMMY:
      case chrCapital_E:  // Eキーの入力
      case chrSmall_E:
        // 起動コード入力
        MemSet(kwBuf, sizeof(kwBuf), 0x00);
        // 起動時パスワード設定
        if (DataInputDialog(NNSH_INPUTWIN_ENTPASS, kwBuf, PASSSIZE,
                            NNSH_DIALOG_USE_PASS, NULL) != false)
        {
            len = StrLen((NNshGlobal->NNsiParam)->launchPass);
            if (StrNCompare((NNshGlobal->NNsiParam)->launchPass, kwBuf, len) == 0)
            {
                // 起動時パスワードの一致を確認した
                NNsh_InformMessage(ALTID_INFO, MSG_MATCH_LAUNCHWORD, "", 0);

                // 前回終了した画面を開く
                FrmGotoForm(ConvertFormID(NNshGlobal->work1));
                goto FUNC_END;
            }
        }
        FrmDrawForm(FrmGetActiveForm());
        break;

      case MNUID_IPINFO:      
        // インタフェース情報を表示する
        buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize);
        if (buffer != NULL)
        {    
            // ネットライブラリがオープンされてないときはオープンする。
            ret = NNshNet_open(&(NNshGlobal->netRef));
            if (ret == errNone)
            {
                // 自ノードIP情報を取得する
                MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
                NNshNet_GetNetIFInfo(NNshGlobal->netRef, buffer);

                NNshNet_close(NNshGlobal->netRef);

                // 結果を表示する
                NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DUMMYMSG,
                                    true, buffer, StrLen(buffer));
            }
            MEMFREE_PTR(buffer);
        }
        FrmDrawForm(FrmGetActiveForm());
        break;

      case MNUID_RESOLVE:
        // ホストの確認を実施する
        buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize);
        MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
        if (buffer != NULL)
        {    
            // チェックするホスト名を入力...
            MemSet(kwBuf, sizeof(kwBuf), 0x00);
            if (DataInputDialog(NNSH_INPUTWIN_RESOLVE, kwBuf, BUFSIZE,
                                NNSH_DIALOG_USE_RESOLVE, NULL) != false)
            {
                // BUSYウィンドウに確認中のホスト名を表示する
                StrCopy(buffer, "Checking: ");
                StrCat (buffer, kwBuf);

                // ネットライブラリがオープンされてないときはオープンする。
                ret = NNshNet_open(&(NNshGlobal->netRef));
                if (ret == errNone)
                {
                    // BUSYウィンドウの表示
                    Show_BusyForm(buffer);

                    // ホストを確認する
                    MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
                    NNshNet_CheckHostAlive(NNshGlobal->netRef, kwBuf, 3, buffer);            

                    Hide_BusyForm(false);

                    NNshNet_close(NNshGlobal->netRef);

                    // 名前解決結果＋αを表示する
                    NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DUMMYMSG,
                                        true, buffer, StrLen(buffer));
                }
            }
            MEMFREE_PTR(buffer);
        }
        FrmDrawForm(FrmGetActiveForm());
        break;

      case MNUID_NNSI_VERSION:
        // バージョン情報を表示
        ShowVersion_NNsh();
        break;

      case MNUID_EDIT_UNDO:
      case MNUID_EDIT_CUT:
      case MNUID_EDIT_COPY:
      case MNUID_EDIT_PASTE:
      case MNUID_SELECT_ALL:
        // 編集メニューを呼び出す
        return (NNsh_MenuEvt_Edit(event));
        break;

      case MNUID_NETPREF:
        // ネットワーク設定を開く
        OpenNetworkPreferences();
        break;

      case MNUID_DISCONNECT:
      case chrCapital_Q:  // Qキーの入力
      case chrSmall_Q:
        // 回線切断
        if (NNshGlobal->netRef != 0)
        {
            savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            NNshNet_LineHangup();
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
        }
        break;

      case BTNID_NNSI_DUMMY_OK:
      default:
        // 上記以外(NNsi終了イベントを生成する)
        dummyEvent = &(NNshGlobal->dummyEvent);
        MemSet(dummyEvent, sizeof(EventType), 0x00);
        dummyEvent->eType = appStopEvent;
        EvtAddEventToQueue(dummyEvent);
        return (false);
        break; 
    }

FUNC_END:
    return (true);
}
