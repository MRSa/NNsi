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
                  UInt32 range, UInt32 endRange)
{
    Err ret;

    if (NNshParam->useProxy != 0)
    {
        // Proxy使用
        ret = NNshHttp_commMain(type, url, cookie, appendData,
                                range, endRange,
                                NNshParam->proxyPort, NNshParam->proxyURL);
    }
    else
    {
        // Proxy未使用
        ret = NNshHttp_commMain(type, url, cookie, appendData,
                                range, endRange, 80, NULL);
    }
    return (ret);
}

/*==========================================================================*/
/*  NNshNet_LineHangup() : 回線切断                                         */
/*                                                                          */
/*==========================================================================*/
void NNshNet_LineHangup(void)
{
    UInt16 netRef;

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
        Hide_BusyForm();
    }
    return;
}

/*==========================================================================*/
/*  WebBrowserCommand() : WebBrowser呼び出し関数                            */
/*                                              ※ 動いてくれるかな？ ※    */
/*==========================================================================*/
Err WebBrowserCommand(UInt32 creator, Boolean subLaunch, UInt16 flags,
                      UInt16 cmd, Char *parameterP, UInt32 *resultP)
{
    Char             *newParamP;
    UInt32            result;
    UInt16            cardNo, size;
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
    if (subLaunch != true)
    {
        // (NNsiを)一旦終了してWebBrowserを起動する場合
        if (parameterP != NULL)
        {
            // 引数（ＵＲＬ）領域を確保する
            size = StrLen(parameterP);
            newParamP = MemPtrNew(size + MARGIN);
            if (newParamP == NULL)
            {
                // 十分なメモリが確保できなかった、エラー応答する
                return (memErrNotEnoughSpace);
            }
            MemSet  (newParamP, (size + MARGIN), 0x00);
            StrNCopy(newParamP, parameterP, size);

            // アプリが終了しても領域は残るようにする
            MemPtrSetOwner(newParamP, 0);

            // アプリケーションを起動する
            ret = SysUIAppSwitch(cardNo, dbID, cmd, newParamP);
        }
    }
    else
    { 
        // アプリケーションをforkする(SubLaunchの場合)
        ret = SysAppLaunch(cardNo, dbID, flags, cmd, parameterP, &result);
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
    return (ret);
}
