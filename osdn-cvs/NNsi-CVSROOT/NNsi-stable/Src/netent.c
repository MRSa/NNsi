/*============================================================================*
 *  FILE: 
 *     netent.c
 *
 *  Description: 
 *     ネットワーク通信ほかのエントリポイント
 *
 *===========================================================================*/
#define NETENT_C
#include "local.h"

/*==========================================================================*/
/*  NNshHttp_comm() : HTTP通信                                              */
/*                                                                          */
/*==========================================================================*/
Err NNshHttp_comm(UInt16 type, Char *url, Char *cookie, Char *appendData,
                  UInt32 range, UInt32 endRange, Char *message)
{
    UInt16  lp, savedParam, port, sslMode;
    Char   *proxy;
    Err     ret;
    Boolean retry;

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "GET SITE:%s", url);
#endif // #ifdef USE_REPORTER

    retry   = false;
    sslMode = NNSH_SSLMODE_NOTSSL;
    proxy   = NULL;
    port    = 80;
    if ((NNshGlobal->NNsiParam)->useProxy != 0)
    {
        port  = (NNshGlobal->NNsiParam)->proxyPort;
        proxy = (NNshGlobal->NNsiParam)->proxyURL;
    }
#ifdef USE_SSL
    else
    {
        // httpsプロトコルの場合、、、
        if ((url[0] == 'h')&&(url[1] == 't')&&(url[2] == 't')&&(url[3] == 'p')&&
            (url[4] == 's')&&(url[5] == ':'))
        {
            sslMode = NNSH_SSLMODE_SSLV3;
            port    = 443;
            proxy   = NULL;
        }
    }
#endif

    // リトライカウンタをゼロクリア
    lp = 0;
    do
    {
        // http通信を実行
        ret = NNshHttp_commMain(type, url, cookie, appendData,
                                range, endRange, port, proxy, message, sslMode);
        lp++;

        // うまく接続できなかった場合は、一度回線を切る
        if (ret == NNSH_ERRORCODE_FAILURECONNECT)
        {
            if (lp > (NNshGlobal->NNsiParam)->nofRetry)
            {
                if (retry != true)
                {
                    //  リトライ回数を超えても接続できなかった場合には、
                    // 一度回線を切って再接続する
                    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
                    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
                    NNshNet_LineHangup();
                    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
                    ret = netErrTimeout;
                    lp  = 0;
                    retry = true;
                }
            }
            else
            {
                // リトライ回数到達までは、再接続を実施する
                ret = netErrTimeout;
            }
        }
    } while ((lp <= (NNshGlobal->NNsiParam)->nofRetry)&&(ret == netErrTimeout));
    return (ret);
}

/*==========================================================================*/
/*  NNshNet_LineHangup() : 回線切断                                         */
/*                                                                          */
/*==========================================================================*/
void NNshNet_LineHangup(void)
{
    UInt16         netRef;

    if (NNshGlobal->netRef == 0)
    {
        // ネットライブラリのオープン
        (void) SysLibFind("Net.lib", &(netRef));
    }
    else
    {
        netRef = NNshGlobal->netRef;
    }

    // 回線切断の確認
    if ((netRef != 0)&&
        (NNsh_ConfirmMessage(ALTID_CONFIRM,
                             MSG_MESSAGE_DISCONNECT, "", 0) == 0))
    {
        Show_BusyForm(MSG_DISCONNECT_WAIT);
        (void) NetLibClose(netRef, true);           // 回線クローズ
        (void) NetLibFinishCloseWait(netRef);       // 接続待ち強制切断
        NNshGlobal->netRef = 0;

        Hide_BusyForm(true);
    }
    return;
}

/*==========================================================================*/
/*  WebBrowserCommand() : WebBrowser呼び出し関数                            */
/*                                              ※ 動いてくれるかな？ ※    */
/*==========================================================================*/
Err WebBrowserCommand(UInt32 creator, UInt16 subLaunch, UInt16 flags,
                      UInt16 cmd, Char *parameterP, UInt16 size, UInt32 *resultP)
{
    Char             *newParamP;
    UInt32            result;
    UInt16            cardNo;
    LocalID           dbID;
    DmSearchStateType searchState;
    Err               ret;

    if (resultP != NULL)
    {
        *resultP = errNone;
    }
    // WebBrowserがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, creator, 
                                         true, &cardNo, &dbID);
    if (ret != errNone)
    {
        // WebBrowser(NetFront)がインストールされていなかった
        NNsh_InformMessage(ALTID_ERROR, MSG_CANNOTFIND_BROWSER, "", ret);
        return (ret);
    }
    
    // childでforkするか、終了させてforkするかsubLaunchで判定
    // 終了させてforkしないと動かないので
    if (subLaunch == 0)
    {
        // 引数がない場合...
        if (size == 0)
        {
            // アプリケーションを起動する
            ret = SysUIAppSwitch(cardNo, dbID, cmd, NULL);
            return (ret);
        }

        // (NNsiを)一旦終了してWebBrowserを起動する場合
        if (parameterP != NULL)
        {

            // 引数（ＵＲＬ）領域を確保する
            newParamP = MEMALLOC_PTR(size + MARGIN);
            if (newParamP == NULL)
            {
                // 十分なメモリが確保できなかった、エラー応答する
                return (memErrNotEnoughSpace);
            }
            MemSet  (newParamP, (size + MARGIN), 0x00);
            MemMove (newParamP, parameterP, size);

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "PARAMETER:%s", newParamP);
#endif // #ifdef USE_REPORTER

            // アプリが終了しても領域は残るようにする
            MemPtrSetOwner(newParamP, 0);

            // アプリケーションを起動する
            ret = SysUIAppSwitch(cardNo, dbID, cmd, newParamP);
        }
        else
        {
            // アプリケーションを起動する
            ret = SysUIAppSwitch(cardNo, dbID, cmd, NULL);
            return (ret);
        }
    }
    else
    { 
        // なんかうまく動かない...
        if (parameterP != NULL)
        {
            // 引数（ＵＲＬ）領域を確保する
            newParamP = MEMALLOC_PTR(size + MARGIN);
            if (newParamP == NULL)
            {
                // 十分なメモリが確保できなかった、エラー応答する
                return (memErrNotEnoughSpace);
            }
            MemSet  (newParamP, (size + MARGIN), 0x00);
            MemMove (newParamP, parameterP, size);

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "PARAMETER:%s", newParamP);
#endif // #ifdef USE_REPORTER

            // アプリケーションをforkする(SubLaunchの場合)
            ret = SysAppLaunch(cardNo, dbID, flags, cmd, newParamP, &result);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_ERROR, "SysAppLaunch()", "", ret);
                // 処理は継続する //
            }
            if (resultP != NULL)
            {
                *resultP = result;
            }
            MEMFREE_PTR(newParamP);
        }
        else
        {
            // アプリケーションをforkする(SubLaunchの場合)
            ret = SysAppLaunch(cardNo, dbID, flags, cmd, NULL, &result);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_ERROR, "SysAppLaunch()", "", ret);
                // 処理は継続する //
            }
            if (resultP != NULL)
            {
                *resultP = result;
            }
        }
    }
    return (ret);
}

/*==========================================================================*/
/*  OpenNetworkPreferences() : ネットワーク設定画面呼び出し関数             */
/*                                                 ※ 動いてくれるかな？ ※ */
/*==========================================================================*/
void OpenNetworkPreferences(void)
{
    Err                  ret;
    UInt16               cardNo;
    LocalID              dbID;
    DmSearchStateType    state;

    // 変数の初期化
    cardNo = 0;
    MemSet(&state, sizeof(state), 0x00);

    // ネットワーク設定画面のリソースをとってくる。
    ret = DmGetNextDatabaseByTypeCreator(true, &state, sysFileTPanel,
                                         sysFileCNetworkPanel,
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // リソースがあったときには、設定画面を開く
        SysUIAppSwitch(cardNo, dbID, sysAppLaunchCmdPanelCalledFromApp, NULL);
    }
    return;
}

#ifdef USE_BT_CONTROL
/*==========================================================================*/
/*  OpenBluetoothPreferences() : Blutooth設定画面呼び出し関数               */
/*                                                 ※ 動いてくれるかな？ ※ */
/*==========================================================================*/
void OpenBluetoothPreferences(Boolean val)
#ifdef USE_BTTOGGLE_SDK
{
    UInt16            cardNo, code; 
    LocalID           dbID; 
    DmSearchStateType state;
    Err               err;
    UInt32            result;

    // BtToggleがインストールされているか調べる
    err = DmGetNextDatabaseByTypeCreator(true, &state, 0, BtToggle_CRID, 
                                                         true, &cardNo, &dbID);
    if (err == errNone)
    {
        // BtToggleがインストールされていた、、、
        if (val == true)
        {
            code = BtToggleLaunchCmdBtOn;
        }
        else
        {
            code = BtToggleLaunchCmdBtOff;
        }
        SysAppLaunch(cardNo, dbID, 0, code, NULL, &result);
   }
   return;
}
#else
{
    // 何もしない
    return;
}
#endif  // #ifdef USE_BTTOGGLE_SDK
#endif  // #ifdef USE_BT_CONTROL

