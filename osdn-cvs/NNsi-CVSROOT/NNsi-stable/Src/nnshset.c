/*============================================================================*
 *  FILE: 
 *     nnshset.c
 *
 *  Description: 
 *     NNsh parameter setting functions for NNsh.
 *
 *===========================================================================*/
#define NNSHSET_C
#define GLOBAL_REAL
#include "local.h"

extern Err setOfflineLogDir(Char *fileName);

/*=========================================================================*/
/*   Function :   SetControlValue                                          */
/*                                       パラメータから画面に反映する処理  */
/*=========================================================================*/
void SetControlValue(FormType *frm, UInt16 objID, UInt16 *value)
{
    ControlType *chkObj;

    chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objID));
    CtlSetValue(chkObj, (UInt16) *value);

    return;
}

/*=========================================================================*/
/*   Function :   UpdateParameter                                          */
/*                                       画面からパラメータに反映する処理  */
/*=========================================================================*/
void UpdateParameter(FormType *frm, UInt16 objID, UInt16 *value)
{
    ControlType *chkObj;

    chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objID));
    *value = CtlGetValue(chkObj);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting7                                       */
/*                              NNsi設定-7(デバイス関連)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting7(FormType *frm)
{
    ControlType *chkObj, *chkObjSub;
    UInt16       butID;

#ifdef USE_TSPATCH
    Err        ret;
    UInt32     fontVer;

    // TsPatch機能を使用しない
    ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
    if (ret == errNone)
    {    
        UpdateParameter(frm, CHKID_NOTUSE_TSPATCH, &(butID));
        if (butID != 0)
        {
            (NNshGlobal->NNsiParam)->notUseTsPatch = 1;
        }
        else
        {
            (NNshGlobal->NNsiParam)->notUseTsPatch = 0;
        }
    }
    else
#endif
    {
        (NNshGlobal->NNsiParam)->notUseTsPatch = 0;
    }


    // ARMletを使用する
    UpdateParameter(frm, CHKID_USE_ARMLET, &((NNshGlobal->NNsiParam)->useARMlet));
    if ((NNshGlobal->NNsiParam)->useARMlet != 0)
    {
        // ArmLetはPalmOS5専用の機能なため、本当に実施するか確認を表示する)
        butID = NNsh_WarningMessage(ALTID_CONFIRM, MSG_WARNING_OS5, "", 0);
        if (butID != 0)
        {
            (NNshGlobal->NNsiParam)->useARMlet = 0;
        }
    }

#ifdef USE_CLIE
    // SONYハイレゾ機能を使わない
    UpdateParameter(frm, CHKID_NOTUSE_SONYHIRES, &((NNshGlobal->NNsiParam)->disableSonyHR));
    if (NNshGlobal->notUseHR != (NNshGlobal->NNsiParam)->disableSonyHR)
    {
        // SONYハイレゾ機能の使用可否設定は、次回起動時より有効となる
        NNsh_WarningMessage(ALTID_WARN, MSG_HR_NEXT_LAUNCH, "", 0);
    }

    // Silk制御を行わない
    UpdateParameter(frm, CHKID_NOTUSE_SILK, &NNshGlobal->notUseSilk);
    if (NNshGlobal->notUseSilk != (NNshGlobal->NNsiParam)->notUseSilk)
    {
        // SILK制御は行わないときは、警告を表示する
        NNsh_WarningMessage(ALTID_WARN, MSG_SILK_NEXT_LAUNCH, "", 0);
    }
#endif

    // 使用VFS指定
    butID  = LstGetSelection(FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, LSTID_VFS_LOCATION)));
    (NNshGlobal->NNsiParam)->vfsUseCompactFlash = (UInt8) butID;

    // VFS 利用可能時に自動的にVFS ON
    butID = 0;
    UpdateParameter(frm, CHKID_VFSON_AUTOMATIC, &butID);
    (NNshGlobal->NNsiParam)->vfsOnAutomatic = (UInt8) butID;

    // 自動VFS ONでもDBチェックなし
    UpdateParameter(frm, CHKID_VFSON_NOTDBCHK, &((NNshGlobal->NNsiParam)->vfsOnNotDBCheck));

    // VFSの使用と回避フラグ、OFFLINEログ使用フラグの設定
    if ((NNshGlobal->NNsiParam)->useVFS != NNSH_NOTSUPPORT_VFS)
    {
        chkObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_USE_VFS));
        (NNshGlobal->NNsiParam)->useVFS = CtlGetValue(chkObj);

        chkObjSub = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_WORKAROUND;
        }

        chkObjSub = 
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_USEOFFLINE;
        }

        chkObjSub =
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_DBBACKUP;
        }

        chkObjSub =
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_DBIMPORT;
        }
        
        chkObjSub =
           FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));
        if (CtlGetValue(chkObjSub) != 0)
        {
            (NNshGlobal->NNsiParam)->useVFS = (NNshGlobal->NNsiParam)->useVFS | NNSH_VFS_DIROFFLINE;
        }

        if ((NNshGlobal->NNsiParam)->useVFS != 0)
        {
            //  VFS関連パラメータの設定がされていたとき、次回起動時より
            // 有効となることを表示する。
            NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_VFSWARN, "", 0);

            // デフォルトの設定値を更新する
            (NNshGlobal->NNsiParam)->vfsOnDefault
                                             = (NNshGlobal->NNsiParam)->useVFS;
        }

        // logdir.txtを読み、OFFLINEログベースディレクトリを取得
        (void) setOfflineLogDir(LOGDIR_FILE);
    }

    // DBを更新したしるしをつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting8                                       */
/*                                  NNsi設定-8(通信関連)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting8(FormType *frm)
{
    FieldType   *fldP;
    UInt32       bufSize;
    UInt16       timeout;
    Char        *logBuf, *numP;
    MemHandle    txtH;


    // GZIPを使用する設定...
    timeout = 0;
    UpdateParameter(frm, CHKID_USE_GZIP, &(timeout));
    (NNshGlobal->NNsiParam)->useGZIP = timeout;

    // 通信タイムアウト値の変更
    timeout = SysTicksPerSecond();
    timeout = (timeout == 0) ? 1 : timeout;  // ゼロ除算の回避(保険)
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_COMM_TIMEOUT));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);

        // 最大時間よりも大きい場合...
        if (bufSize >= MAX_TIMEOUT_VALUE)
        {
            // 無限大時間待つようにする
            (NNshGlobal->NNsiParam)->netTimeout = -1;
        }
        else 
        {
            if ((NNshGlobal->NNsiParam)->netTimeout != timeout * bufSize)
            {
                (NNshGlobal->NNsiParam)->netTimeout = timeout * bufSize;

                // TIMEOUTの更新情報を表示
                NNsh_DebugMessage(ALTID_INFO, MSG_TIMEOUT_UPDATED, 
                                  "", (NNshGlobal->NNsiParam)->netTimeout);
            }
        }
    }

    // 内部バッファサイズの変更
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_BUFFER_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);
        if ((bufSize < NNSH_WORKBUF_MIN)||(bufSize > NNSH_WORKBUF_MAX))
        {
            // データ値異常、入力範囲異常を表示して元に戻る
            logBuf = MEMALLOC_PTR(BUFSIZE);
            if (logBuf != NULL)
            {
                MemSet (logBuf, BUFSIZE, 0x00);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE1);
                NUMCATI(logBuf, bufSize);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE2);
                NUMCATI(logBuf, NNSH_WORKBUF_MIN);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE3);
                NUMCATI(logBuf, NNSH_WORKBUF_MAX);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE4);
                NUMCATI(logBuf, NNSH_WORKBUF_DEFAULT);
                NNsh_ErrorMessage(ALTID_ERROR, logBuf, "", 0);
                MEMFREE_PTR(logBuf);
            }
            return (~errNone);
        }
        else
        {
            if (bufSize == (NNshGlobal->NNsiParam)->bufferSize)
            {
                // バッファサイズは変更しなかった旨報告し、終了する
                NNsh_DebugMessage(ALTID_INFO, MSG_WARN_NOTMODIFIED,
                                  "buffer size:", (NNshGlobal->NNsiParam)->bufferSize);
            }
            else
            {
                // バッファサイズを変更して終了する
                (NNshGlobal->NNsiParam)->bufferSize = bufSize;
                NNsh_DebugMessage(ALTID_INFO, MSG_INFO_MODIFIED,
                                  "buffer size:", (NNshGlobal->NNsiParam)->bufferSize);
            }
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "0", 0);
    }

    // 分割ダウンロード指定を取得する
    UpdateParameter(frm, CHKID_DL_PART, &((NNshGlobal->NNsiParam)->enablePartGet));

    // BBS一覧を上書きする
    UpdateParameter(frm, CHKID_BBS_OVERWRITE, &((NNshGlobal->NNsiParam)->bbsOverwrite));

    // 分割ダウンロードのサイズを取得
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_DL_PARTSIZE));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);

        if (bufSize < NNSH_DOWNLOADSIZE_MIN)
        {
            // ダウンロードサイズは、バッファサイズ下限よりも小さい
            NNsh_ErrorMessage(ALTID_WARN, MSG_DOWNLOADSIZE_UNDER,
                              "", NNSH_DOWNLOADSIZE_MIN);
            return (~errNone);
        }
        if (bufSize > (NNshGlobal->NNsiParam)->bufferSize)
        {
            // ダウンロードサイズは、ワークバッファサイズよりも小さい
            NNsh_ErrorMessage(ALTID_WARN, MSG_DOWNLOADSIZE_OVER, "", 0);
            return (~errNone);
        }
        if (bufSize != (NNshGlobal->NNsiParam)->partGetSize)
        {
            (NNshGlobal->NNsiParam)->partGetSize = bufSize;

            // ダウンロードサイズの更新を表示
            NNsh_DebugMessage(ALTID_INFO, MSG_DOWNLOADSIZE_UPDATE, "",
                              (NNshGlobal->NNsiParam)->partGetSize);
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "1", 0);
    }

    // BBS一覧取得先の設定
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_URL_BBS));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // フィールドからURLを取得する
        numP = MemHandleLock(txtH);
        MemSet  ((NNshGlobal->NNsiParam)->bbs_URL, MAX_URL, 0x00);
        StrNCopy((NNshGlobal->NNsiParam)->bbs_URL, numP, (MAX_URL - 1));
        MemHandleUnlock(txtH);
    }

    // Proxy経由でのアクセス
    UpdateParameter(frm, CHKID_USE_PROXY, &((NNshGlobal->NNsiParam)->useProxy));

    // Proxy URL
    fldP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, FLDID_USE_PROXY));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP = MemHandleLock(txtH);
        MemSet((NNshGlobal->NNsiParam)->proxyURL, MAX_URL, 0x00);
        if (*numP != '\0')
        {
            StrNCopy((NNshGlobal->NNsiParam)->proxyURL, numP, (MAX_URL - 1));
        }
        MemHandleUnlock(txtH);
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "PROXY URL", 0);
    }

    // Proxy Port #
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_PROXY_PORT));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP    = MemHandleLock(txtH);
        bufSize = StrAToI(numP);
        MemHandleUnlock(txtH);

        (NNshGlobal->NNsiParam)->proxyPort = bufSize;
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "PROXY PORT", 0);
    }

    // 通信タイムアウト時のリトライ回数
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_TIMEOUT_RETRY));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP     = MemHandleLock(txtH);
        timeout  = StrAToI(numP);
        MemHandleUnlock(txtH);

        // リトライ回数が規定より大きければ、規定値で丸める
        timeout = (timeout > NNSH_LIMIT_RETRY)? NNSH_LIMIT_RETRY : timeout;
        (NNshGlobal->NNsiParam)->nofRetry = timeout;
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "RETRY COUNT", 0);
    }

    // ZLIBのサポート可否確認を行う
    SetZLIBsupport_NNsh();

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting5                                       */
/*                              NNsi設定-5(書き込み関連)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting5(FormType *frm)
{
    UInt16       item;
    UInt32       bufSize;
    FieldType   *fldP;
    Char        *nameP, *logBuf;
    MemHandle    txtH;

    // 書き込み時バッファサイズの取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_WRITEBUF_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // フィールドから書き込みバッファサイズを取得する
        nameP = MemHandleLock(txtH);
        if (nameP != NULL)
        {
            bufSize = StrAToI(nameP);
            MemHandleUnlock(txtH);
        }
        else
        {
            bufSize = NNSH_WRITEBUF_DEFAULT;
        }
        if ((bufSize > NNSH_WRITEBUF_MAX)||(bufSize < NNSH_WRITEBUF_MIN))
        {
            // データ値異常、入力範囲異常を表示して元に戻る
            logBuf = MEMALLOC_PTR(BUFSIZE);
            if (logBuf != NULL)
            {
                MemSet (logBuf, BUFSIZE, 0x00);
                StrCat (logBuf, MSG_WRITEBUF_ILLEGAL);
                NUMCATI(logBuf, bufSize);
                StrCat (logBuf, MSG_WRITEBUF_ILLEGAL2);
                NUMCATI(logBuf, NNSH_WRITEBUF_MIN);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE3);
                NUMCATI(logBuf, NNSH_WRITEBUF_MAX);
                StrCat (logBuf, MSG_ERROR_BUFFER_SIZE4);
                NUMCATI(logBuf, NNSH_WRITEBUF_DEFAULT);
                NNsh_ErrorMessage(ALTID_ERROR, logBuf, "", 0);
                MEMFREE_PTR(logBuf);
            }
            return (~errNone);
        }
        else
        {
            // サイズを確認する
            if (bufSize == (NNshGlobal->NNsiParam)->writeBufferSize)
            {
                // サイズは変更しなかった旨報告し、終了する
                NNsh_DebugMessage(ALTID_INFO, MSG_WARN_NOTMODIFIED,
                                  "writeBufferSize:", (NNshGlobal->NNsiParam)->writeBufferSize);
            }
            else
            {
                (NNshGlobal->NNsiParam)->writeBufferSize = bufSize;
                NNsh_DebugMessage(ALTID_INFO, MSG_INFO_MODIFIED,
                                  "writeBufferSize:", (NNshGlobal->NNsiParam)->writeBufferSize);
            }
        }
    }

    // 書き込み時PUSHボタンを無効
    UpdateParameter(frm, CHKID_WRITE_AUTOSAVE,
                                       &((NNshGlobal->NNsiParam)->writeMessageAutoSave));

    // 書き込み時PUSHボタンを無効
    item = 0;
    UpdateParameter(frm, CHKID_WRITEPUSH_DISABLE, &(item));
    (NNshGlobal->NNsiParam)->writeJogPushDisable = item;

    // sage書き込み
    UpdateParameter(frm, CHKID_WRITE_SAGE,  &((NNshGlobal->NNsiParam)->writeAlwaysSage));

    // コテハン機能を使用する設定
    UpdateParameter(frm, CHKID_FIXED_HANDLE, &((NNshGlobal->NNsiParam)->useFixedHandle));

    // ハンドル名の設定(ハンドル名が設定できなくても正常応答する)
    MemSet((NNshGlobal->NNsiParam)->handleName, BUFSIZE, 0x00);
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_HANDLENAME));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        nameP = MemHandleLock(txtH);
        MemSet((NNshGlobal->NNsiParam)->handleName, BUFSIZE, 0x00);
        if (*nameP != '\0')
        {
            StrNCopy((NNshGlobal->NNsiParam)->handleName, nameP, (BUFSIZE - 1));
        }
        MemHandleUnlock(txtH);
    }

    // 受信Cookieを使用して書き込み
    UpdateParameter(frm, CHKID_WRITE_USE_COOKIE, &((NNshGlobal->NNsiParam)->useCookieWrite));

    // 書きこみ時、書きこみフィールドに自動的にフォーカスを移動しない
    item = 0;
    UpdateParameter(frm, CHKID_NOT_AUTOFOCUSSET, &(item));
    (NNshGlobal->NNsiParam)->notAutoFocus = item;

    // 書きこみ時、シルクを拡大しない
    item = 0;
    UpdateParameter(frm, CHKID_NOTSILK_WRITE, &(item));
    (NNshGlobal->NNsiParam)->notOpenSilkWrite = item;

    // 書き込み時、アンダーラインをOFFにする
    UpdateParameter(frm, CHKID_UNDERLINE_WRITE, &((NNshGlobal->NNsiParam)->disableUnderlineWrite));

    // 書き込み時引用カキコにする
    UpdateParameter(frm, CHKID_INSERT_REPLYMSG, &item);
    (NNshGlobal->NNsiParam)->writingReplyMsg = item;    

    // 書き込み時参照スレ番号を挿入する
    UpdateParameter(frm, CHKID_INSERT_REPLYNUM, &((NNshGlobal->NNsiParam)->insertReplyNum));

    // 書き込みシーケンスPART2
    UpdateParameter(frm, CHKID_WRITE_SEQUENCE2, &((NNshGlobal->NNsiParam)->writeSequence2));

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting6                                       */
/*                                NNsi設定-6(ジョグ関連)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting6(FormType *frm)
{
    UInt16 item;

    // 参照時JumpListに「一覧へ」
    UpdateParameter(frm, CHKID_ADD_RTNTOLIST, &((NNshGlobal->NNsiParam)->addReturnToList));

    // 参照時JumpListに「ひとつ戻る」
    UpdateParameter(frm, CHKID_ADD_BACKMESSAGE, &((NNshGlobal->NNsiParam)->addMenuBackRtnMsg));

    // 参照時JumpListにメニュー
    UpdateParameter(frm, CHKID_ADD_OPENMENU, &((NNshGlobal->NNsiParam)->addMenuMsg));

    // 参照時JumpListに「差分取得」
    UpdateParameter(frm, CHKID_ADD_GETPART, &((NNshGlobal->NNsiParam)->addLineGetPart));

    // 参照時JumpListに「回線切断」
    UpdateParameter(frm, CHKID_ADD_LINEDISCONN, &((NNshGlobal->NNsiParam)->addLineDisconn));

    // 参照時JumpListに先頭へ
    UpdateParameter(frm, CHKID_ADD_JUMPTOP, &((NNshGlobal->NNsiParam)->addJumpTopMsg));

    // 参照時JumpListに末尾へ
    UpdateParameter(frm, CHKID_ADD_JUMPBOTTOM, &((NNshGlobal->NNsiParam)->addJumpBottomMsg));

    // 参照時JumpListに全選択+Webブラウザで開く
    UpdateParameter(frm, CHKID_ADD_OPENWEB, &((NNshGlobal->NNsiParam)->addMenuSelAndWeb));

    // 参照時JumpListに描画モード切替
    UpdateParameter(frm, CHKID_ADD_GRAPHVIEW, &((NNshGlobal->NNsiParam)->addMenuGraphView));

    // 参照時JumpListにお気に入り設定切り替え
    UpdateParameter(frm, CHKID_ADD_FAVORITE, &item);
    (NNshGlobal->NNsiParam)->addMenuFavorite = item;

    // 参照時JumpListにスレ番指定取得
    UpdateParameter(frm, CHKID_ADD_GETTHREADNUM, &item);
    (NNshGlobal->NNsiParam)->addMenuGetThreadNum = item;

    // 参照時JumpListにメモ帳出力
    UpdateParameter(frm, CHKID_ADD_OUTPUTMEMO, &item);
    (NNshGlobal->NNsiParam)->addMenuOutputMemo = item;

    // 一覧時Menuにメニュー表示
    UpdateParameter(frm, CHKID_ADD_MENUOPEN, &item);
    (NNshGlobal->NNsiParam)->addMenuTitle = item;

    // 一覧時Menuに参照COPY
    UpdateParameter(frm, CHKID_ADD_MENUCOPY, &(NNshGlobal->NNsiParam)->addMenuCopyMsg);

    // 一覧時MenuにMSG削除
    UpdateParameter(frm, CHKID_ADD_MENUDELETE, &(NNshGlobal->NNsiParam)->addMenuDeleteMsg);

    // 一覧時Menuに描画モード変更
    UpdateParameter(frm, CHKID_ADD_MENUGRAPHMODE, &(NNshGlobal->NNsiParam)->addMenuGraphTitle);

    // 一覧時Menuに多目的スイッチ１
    UpdateParameter(frm, CHKID_ADD_MENUMULTISW1, &(NNshGlobal->NNsiParam)->addMenuMultiSW1);

    // 一覧時Menuに多目的スイッチ２
    UpdateParameter(frm, CHKID_ADD_MENUMULTISW2, &(NNshGlobal->NNsiParam)->addMenuMultiSW2);

    // 一覧時MenuにNNsi終了
    UpdateParameter(frm, CHKID_ADD_NNSIEND, &(NNshGlobal->NNsiParam)->addMenuNNsiEnd);

    // 一覧時Menuにデバイス情報
    UpdateParameter(frm, CHKID_ADD_MENUDEVICEINFO, &(NNshGlobal->NNsiParam)->addMenuDeviceInfo);

    // 参照時Menuにデバイス情報
    UpdateParameter(frm, CHKID_ADD_DEVICEINFO, &item);
    (NNshGlobal->NNsiParam)->addDeviceInfo = item;

    // 参照時MenuにNG3設定
    UpdateParameter(frm, CHKID_ADD_NG3SET, &item);
    (NNshGlobal->NNsiParam)->addNgSetting3 = item;

    // 一覧時MenuにDir選択
    UpdateParameter(frm, CHKID_ADD_MENUDIRSELECT, &(NNshGlobal->NNsiParam)->addMenuDirSelect);

    // MenuにBt On/Off表示
    UpdateParameter(frm, CHKID_ADD_BT_ONOFF, &item);
    (NNshGlobal->NNsiParam)->addBtOnOff = item;

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effect_NNshSetting                                       */
/*                   NNsi設定画面(NNsi総合)からパラメータ値に反映する処理  */
/*-------------------------------------------------------------------------*/
static Err effect_NNshSetting(FormType *frm)
{
    ListType *lstP;
    UInt16   item;

    /***  NNsh設定を(グローバル変数に)反映させる  ***/

    // 板URL取得時、有効チェックなし
    UpdateParameter(frm, CHKID_NOTCHECK_BBSURL,&((NNshGlobal->NNsiParam)->notCheckBBSURL));

    // 板移動時に未取得スレ削除
    UpdateParameter(frm, CHKID_AUTODEL_NOT_YET,&((NNshGlobal->NNsiParam)->autoDeleteNotYet));

    // カナを半角→全角変換
    UpdateParameter(frm,CHKID_CONVERT_HANZEN, &((NNshGlobal->NNsiParam)->convertHanZen));

    // Offlineスレ検索時にスレ消去を実施しない
    UpdateParameter(frm, CHKID_NOT_DELOFFLINE, &item);
    (NNshGlobal->NNsiParam)->notDelOffline = item;

    // お気に入り表示最低スレレベルの設定をパラメータへ反映
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_FAVOR_LEVEL));
    item  = LstGetSelection(lstP);
    switch (item)
    {
      case 1:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR_L2;
        break;

      case 2:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR_L3;
        break;

      case 3:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR_L4;
        break;

      case 4:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR;
        break;

      case 0:
      default:
        (NNshGlobal->NNsiParam)->displayFavorLevel = NNSH_MSGATTR_FAVOR_L1;
        break;
    }

    // 確認メッセージを省略
    item = 0;
    UpdateParameter(frm, CHKID_OMIT_DIALOG, &item);
    if (item == 0)
    {
        // 確認メッセージ省略設定をクリアする
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_NOTHING;
    }
    if ((item != 0)&&((NNshGlobal->NNsiParam)->confirmationDisable == 0))
    {
        // 確認メッセージ省略設定を全て設定する
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
    }

    // 検索時に、大文字小文字を区別しない
    UpdateParameter(frm, CHKID_SEARCH_CASELESS, &((NNshGlobal->NNsiParam)->searchCaseless));

    // 起動時にOfflineスレを検索
    UpdateParameter(frm, CHKID_OFFCHK_LAUNCH,   &((NNshGlobal->NNsiParam)->offChkLaunch));

    // 参照ログへコピーしたとき、同時にスレ削除を実施
    UpdateParameter(frm, CHKID_COPYDEL_READONLY,
                    &((NNshGlobal->NNsiParam)->copyDelReadOnly));

    // i-mode用のURLを使用する
    UpdateParameter(frm, CHKID_USE_IMODEURL, &((NNshGlobal->NNsiParam)->useImodeURL));


    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting2                                       */
/*                                   NNsi設定2(一覧画面)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting2(FormType *frm)
{
    UInt16     item;
    ListType  *lstP;

    // スレ一覧(subject.txt)全取得
    UpdateParameter(frm, CHKID_SUBJECT_ALL, &((NNshGlobal->NNsiParam)->getAllThread));

    // スレ一覧画面でtiny/smallフォントを使用する
    UpdateParameter(frm,CHKID_CLIE_USE_TITLE,
                                     &((NNshGlobal->NNsiParam)->useSonyTinyFontTitle));

    // 重複確認
    UpdateParameter(frm, CHKID_CHECK_DUPLICATE,
                    &((NNshGlobal->NNsiParam)->checkDuplicateThread));

    // 多目的スイッチ設定１の設定をパラメータへ反映
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_PRINT_HEADER));
    item  = LstGetSelection(lstP);
    
    switch (item)
    {
      case 0:
        // スレの先頭には何も表示しない
        (NNshGlobal->NNsiParam)->printNofMessage        = 0;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 0;
        break;

      case 2:
        // スレの先頭に未読数を表示する
        (NNshGlobal->NNsiParam)->printNofMessage        = 1;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 1;
        break;

      case 3:
        // スレの先頭にレベル-レス数を表示する
        (NNshGlobal->NNsiParam)->printNofMessage        = 1;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 2;
        break;

      case 4:
        // スレの先頭にレベル-未読数を表示する
        (NNshGlobal->NNsiParam)->printNofMessage        = 1;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 3;
        break;

      case 1:
      default:
        // スレの先頭にレス数を表示する
        (NNshGlobal->NNsiParam)->printNofMessage        = 1;
        (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = 0;
        break;
    }

    // 取得済み全ては参照ログに表示しない
    item = 0;
    UpdateParameter(frm, CHKID_NOT_READONLY_NEW, &(item));
    (NNshGlobal->NNsiParam)->notListReadOnlyNew = item;

    // 取得済み全てには参照ログに表示しない
    item = 0;
    UpdateParameter(frm, CHKID_NOT_READONLY, &(item));
    (NNshGlobal->NNsiParam)->notListReadOnly = item;

    // 取得後にスレ参照
    UpdateParameter(frm, CHKID_OPEN_AUTO,   &((NNshGlobal->NNsiParam)->openAutomatic));

#ifdef USE_COLOR
    // 一覧表示時にカラー
    UpdateParameter(frm, CHKID_USE_COLORMODE, &((NNshGlobal->NNsiParam)->useColor));
#endif

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting3                                       */
/*                                NNsi設定-3(一覧ボタン)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting3(FormType *frm)
{
    Char      *txtP;
    FieldType *fldP;
    ListType  *lstP;
    UInt16     item;
    MemHandle  txtH;

    // 多目的スイッチ設定１の設定をパラメータへ反映
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_TITLE_FUNCSW1));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1 = item | NNSH_SWITCHUSAGE_TITLE;

    // 多目的スイッチ設定２の設定をパラメータへ反映
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_TITLE_FUNCSW2));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2 = item | NNSH_SWITCHUSAGE_TITLE;

    if ((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1 ==
        (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2)
    {
        // 多目的スイッチ１と２の設定値が同じだった場合スイッチ２を使用しない
        (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2 = NNSH_SWITCHUSAGE_TITLE;
    }

    // 多目的ボタン１の設定
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN1));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn1Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN1));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn1Feature = item | MULTIBTN_FEATURE;

    // 多目的ボタン２の設定
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN2));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn2Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN2));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn2Feature = item | MULTIBTN_FEATURE;

    // 多目的ボタン３の設定
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN3));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn3Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN3));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn3Feature = item | MULTIBTN_FEATURE;

    // 多目的ボタン４の設定
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN4));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn4Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN4));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn4Feature = item | MULTIBTN_FEATURE;

    // 多目的ボタン５の設定
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN5));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn5Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN5));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn5Feature = item | MULTIBTN_FEATURE;

    // 多目的ボタン６の設定
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MULTIBTN6));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        StrNCopy ((NNshGlobal->NNsiParam)->multiBtn6Caption, txtP, LIMIT_CAPTION);
        MemHandleUnlock(txtH);
    }
    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIBTN6));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->multiBtn6Feature = item | MULTIBTN_FEATURE;

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting9                                       */
/*                             NNsi設定-9(新着/回線関連)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting9(FormType *frm)
{
    ListType  *lstP;
    UInt16     item;

    // NNsi終了時に回線を切断する
    UpdateParameter(frm, CHKID_END_OFFLINE, &((NNshGlobal->NNsiParam)->disconnectNNsiEnd));

    // 「新着確認」終了時に回線を切断する
    item = 0;
    UpdateParameter(frm, CHKID_DISCONN_ARRIVAL, &(item));
    (NNshGlobal->NNsiParam)->disconnArrivalEnd = item;

    // 「新着確認」前にDAを起動する
    item = 0;
    UpdateParameter(frm, CHKID_PREPARE_DA, &(item));
    (NNshGlobal->NNsiParam)->preOnDAnewArrival = item;

    // 「新着確認」終了後に未読一覧を表示する
    item = 0;
    UpdateParameter(frm, CHKID_ARRIVAL_NOTREAD, &(item));
    (NNshGlobal->NNsiParam)->autoOpenNotRead = item;

    // 「新着確認」終了後にビープ音
    item = 0;
    UpdateParameter(frm, CHKID_DONE_BEEP, &(item));
    (NNshGlobal->NNsiParam)->autoBeep = item;

    // 「新着確認」終了後にマクロ実行
    item = 0;
    UpdateParameter(frm, CHKID_EXECUTE_MACRO, &(item));
    (NNshGlobal->NNsiParam)->startMacroArrivalEnd = item;

    //  NNsi起動時にマクロを実行
    item = 0;
    UpdateParameter(frm, CHKID_AUTOSTARTMACRO, &(item));
    (NNshGlobal->NNsiParam)->autostartMacro = item;
    if ((NNshGlobal->NNsiParam)->autostartMacro != 0)
    {
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_MACROENABLE, "", 0);
    }

    // 回線未接続時は取得予約
    UpdateParameter(frm,CHKID_GETRESERVEFEATURE,&(NNshGlobal->NNsiParam)->getReserveFeature);

    // 回線接続後画面を再描画
    UpdateParameter(frm,CHKID_REDRAW_CONNECT,&((NNshGlobal->NNsiParam)->redrawAfterConnect));

    // まちBBS/したらば@JBBSも新着確認
    item = 0;
    UpdateParameter(frm, CHKID_HTMLBBS_ENABLE, &(item));
    (NNshGlobal->NNsiParam)->enableNewArrivalHtml = item;
    if ((NNshGlobal->NNsiParam)->enableNewArrivalHtml != 0)
    {
        //  まちBBS/したらば@JBBSで新着確認する場合は、差分取得ではなく
        // 再取得を実施することを警告する
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNMACHINEW, "", 0);
    }

    // BBS一覧のリスト状態を反映させる
    lstP  = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,LSTID_CHECK_NEWARRIVAL));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->newArrivalNotRead = item;

#ifdef USE_LOGCHARGE
    // 参照ログ取得レベルをパラメータに反映させる
    lstP  = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, LSTID_CHECK_READONLY));
    item  = LstGetSelection(lstP);
    (NNshGlobal->NNsiParam)->getROLogLevel = item;
#endif

    // 新着時エラーになれば再取得
    UpdateParameter(frm, CHKID_ALLUPDATE_ABORT, &((NNshGlobal->NNsiParam)->autoUpdateGetError));

    // WebBrowserを開くとき最新50
    UpdateParameter(frm, CHKID_WEBBROWSE_LAST50, &item);
    if (item != 0)
    {
        (NNshGlobal->NNsiParam)->browseMesNum = 50;
    }
    else
    {
        (NNshGlobal->NNsiParam)->browseMesNum = 0;
    }

    // 一覧取得時に新着確認
    UpdateParameter(frm, CHKID_UPDATEMSG_LIST, &((NNshGlobal->NNsiParam)->listAndUpdate));
    if ((NNshGlobal->NNsiParam)->listAndUpdate != 0)
    {
        // 一覧取得時に新着確認する場合は「一覧を上書き」に自動でチェックを入れる
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNLISTUPDATE, "", 0);
        (NNshGlobal->NNsiParam)->checkDuplicateThread = 1;
    }

    // 新着確認時にメールチェック
    UpdateParameter(frm, CHKID_cMDA_EXECUTE, &((NNshGlobal->NNsiParam)->use_DAplugin));
    if ((NNshGlobal->NNsiParam)->use_DAplugin != 0)
    {
        // 新着確認時にメールチェックする場合の注意事項を表示する
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNcMDA, MSG_NNSISET_WARNcMDA2, 0);
    }

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSetting4                                       */
/*                                  NNsi設定-4(参照画面)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSetting4(FormType *frm)
{
    FieldType *fldP;
    MemHandle txtH;
    Char      *numP;
    UInt16     resNum, item;

    // レス数の最大サイズ
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_RESNUM_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        // フィールドからレス数の最大サイズを取得する
        numP = MemHandleLock(txtH);
        if (numP != NULL)
        {
            resNum = StrAToI(numP);
            MemHandleUnlock(txtH);
        }
        else
        {
            resNum = NNSH_MESSAGE_MAXNUMBER;
        }

        // レス数の最大サイズ指定が異常だった場合には、初期値に戻す
        if ((resNum < NNSH_MESSAGE_MAXNUMBER)||(resNum > NNSH_MESSAGE_MAXLIMIT))
        {
            resNum = NNSH_MESSAGE_MAXNUMBER;
        }
        (NNshGlobal->NNsiParam)->msgNumLimit = resNum;
    }

    // RAW表示モード
    UpdateParameter(frm, CHKID_USE_RAWDISPLAY, &resNum);
    (NNshGlobal->NNsiParam)->rawDisplayMode = resNum;

    // Bookmark(しおり設定)
    UpdateParameter(frm, CHKID_USE_BOOKMARK,&((NNshGlobal->NNsiParam)->useBookmark));

    // 名前欄を簡略化する
    item = 0;
    UpdateParameter(frm, CHKID_INFO_ONELINE, &item);
    (NNshGlobal->NNsiParam)->useNameOneLine = item;
    if ((NNshGlobal->NNsiParam)->useNameOneLine != 0)
    {
//        // 名前欄を簡略化するときは、グラフィック描画モードになってないと有効でない
//        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNcMDA, MSG_NNSISET_WARNcMDA2, 0);
    }

    // スレ番号を【】にする
    item = 0;
    UpdateParameter(frm, CHKID_BOLD_MSGNUM, &item);
    (NNshGlobal->NNsiParam)->boldMessageNum = item;

    // スレ参照画面でtiny/smallフォントを使用する
    UpdateParameter(frm,CHKID_CLIE_USE_TINYFONT,
                                      &((NNshGlobal->NNsiParam)->useSonyTinyFont));

    // 参照時アンダーラインOFF
    UpdateParameter(frm, CHKID_MSG_LINEOFF, &((NNshGlobal->NNsiParam)->disableUnderline));

    // PUSH ONダイヤルで±１
    UpdateParameter(frm, CHKID_NOT_CURSOR, &((NNshGlobal->NNsiParam)->notCursor));

    // スレ参照画面でカーソル表示しない
    item = 0;
    UpdateParameter(frm,CHKID_NOT_FOCUSSET_NUM, &item);
    (NNshGlobal->NNsiParam)->notFocusNumField = item;

    // スレ参照画面でGoメニューのアンカーをソートさせる
    item = 0;
    UpdateParameter(frm,CHKID_GO_SORT, &item);
    (NNshGlobal->NNsiParam)->sortGoMenuAnchor = item;

    // 参照時前に移動で前レス末尾
    UpdateParameter(frm, CHKID_DISPLAY_BOTTOMMES, &((NNshGlobal->NNsiParam)->dispBottom));

    // 参照時レスを連続表示
    UpdateParameter(frm,CHKID_BLOCK_DISP_MODE, &((NNshGlobal->NNsiParam)->blockDispMode));
    if ((NNshGlobal->NNsiParam)->blockDispMode != 0)
    {
        // レスを連続表示するときは、前に移動で前レス末尾もＯＮにする
        (NNshGlobal->NNsiParam)->blockDispMode = NNSH_BLOCKDISP_NOFMSG;
        (NNshGlobal->NNsiParam)->dispBottom    = 1;
    }
/**
    // NG確認
    (NNshGlobal->NNsiParam)->hideMessage = 0;    
    UpdateParameter(frm, CHKID_HIDE_MESSAGE, &resNum);
    if (resNum != 0)
    {
        // NG確認-3を実施する
        (NNshGlobal->NNsiParam)->hideMessage = NNSH_USE_NGWORD1AND2;
    }
**/
    // NG確認(NG-3)
    (NNshGlobal->NNsiParam)->hideMessage = 0;    
    UpdateParameter(frm, CHKID_HIDE3_MESSAGE, &resNum);
    if (resNum != 0)
    {
        // NG確認-3を実施する
        (NNshGlobal->NNsiParam)->hideMessage = (NNshGlobal->NNsiParam)->hideMessage | NNSH_USE_NGWORD3;
    }

    // NGワード設定で","で複数のNGワードを使用する
    (NNshGlobal->NNsiParam)->useRegularExpression = 1;
#if 0
    // NGワード設定で複数のNGワードを使用する
    UpdateParameter(frm, CHKID_USE_REGEXPRESS,
                                        &((NNshGlobal->NNsiParam)->useRegularExpression));
    if ((NNshGlobal->NNsiParam)->useRegularExpression != 0)
    {
        //  正規表現を使用する場合には、パフォーマンスが保証できないことを
        // 警告する
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNREGEXP, "", 0);
    }
#endif

    // NG-3設定をスレ毎に切替える設定
    resNum = 0;
    UpdateParameter(frm, CHKID_USE_NG_EACHTHREAD, &resNum);
    if (resNum != 0)
    {
        (NNshGlobal->NNsiParam)->hideMessage =
                 (((NNshGlobal->NNsiParam)->hideMessage)|(NNSH_USE_NGWORD_EACHTHREAD));
    }
    else
    {
        (NNshGlobal->NNsiParam)->hideMessage =
                 (((NNshGlobal->NNsiParam)->hideMessage)&(NNSH_USE_NGWORD_CLEAREACHTHREAD));
    }

    // しおりを使う場合の設定
    if ((NNshGlobal->NNsiParam)->useBookmark == 0)
    {
        // しおりを使わない場合、しおり設定のクリア
        (NNshGlobal->NNsiParam)->lastFrmID     = NNSH_FRMID_THREAD;
        (NNshGlobal->NNsiParam)->bookMsgNumber = 0;
        (NNshGlobal->NNsiParam)->bookMsgIndex  = 0;
        MEMFREE_PTR(NNshGlobal->bookmarkFileName);
        MEMFREE_PTR(NNshGlobal->bookmarkNick);
    }
    else
    {
        if (NNshGlobal->bookmarkFileName == NULL)
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
        }
    }

    // NG設定1および2の文字列チェック
    SeparateWordList((NNshGlobal->NNsiParam)->hideWord1, &(NNshGlobal->hide1));
    SeparateWordList((NNshGlobal->NNsiParam)->hideWord2, &(NNshGlobal->hide2));

    // スレレベルのセレクタトリガに割り当てる
    (NNshGlobal->NNsiParam)->viewMultiBtnFeature = 
          LstGetSelection(FrmGetObjectPtr(frm,
                                     FrmGetObjectIndex(frm, LSTID_LVLSELTRIG)));

    // スレタイトルのセレクタトリガに割り当てる
    (NNshGlobal->NNsiParam)->viewTitleSelFeature = 
          LstGetSelection(FrmGetObjectPtr(frm,
                                     FrmGetObjectIndex(frm, LSTID_TTLSELTRIG)));

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNNsiSettingA                                       */
/*                                        NNsi設定(概略)の情報を反映させる */
/*-------------------------------------------------------------------------*/
static Err effectNNsiSettingA(FormType *frm)
{
    UInt16   item;

    // DEBUGモードの設定
    UpdateParameter(frm, CHKID_DEBUG_DIALOG,&((NNshGlobal->NNsiParam)->debugMessageON));

    // スレ一覧(subject.txt)全取得
    UpdateParameter(frm, CHKID_SUBJECT_ALL, &((NNshGlobal->NNsiParam)->getAllThread));

#ifdef USE_COLOR
    // 一覧表示時にカラー
    UpdateParameter(frm, CHKID_USE_TITLE_COLOR, &((NNshGlobal->NNsiParam)->useColor));
#endif

    // 使用VFS指定
    item  = LstGetSelection(FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, LSTID_VFS_LOCATION)));
    (NNshGlobal->NNsiParam)->vfsUseCompactFlash = (UInt8) item;

    // VFS 利用可能時に自動的にVFS ON
    item = 0;
    UpdateParameter(frm, CHKID_VFSON_AUTOMATIC, &item);
    if (item != 0)
    {
        // 自動VFS ONをNNsi設定(概略)で設定した場合には、DBチェックをOFFにする
        if (((NNshGlobal->NNsiParam)->vfsOnAutomatic == 0)&&((NNshGlobal->NNsiParam)->vfsOnNotDBCheck == 0))
        {
            (NNshGlobal->NNsiParam)->vfsOnNotDBCheck = 1;
        }
    }
    (NNshGlobal->NNsiParam)->vfsOnAutomatic = (UInt8) item;

    // 参照時レスを連続表示
    UpdateParameter(frm,CHKID_BLOCK_DISP_MODE, &((NNshGlobal->NNsiParam)->blockDispMode));
    if ((NNshGlobal->NNsiParam)->blockDispMode != 0)
    {
        // レスを連続表示するときは、前に移動で前レス末尾もＯＮにする
        (NNshGlobal->NNsiParam)->blockDispMode = NNSH_BLOCKDISP_NOFMSG;
        (NNshGlobal->NNsiParam)->dispBottom    = 1;
    }

    // 回線未接続時は取得予約
    UpdateParameter(frm,CHKID_GETRESERVEFEATURE,&(NNshGlobal->NNsiParam)->getReserveFeature);

    // まちBBS/したらば@JBBSも新着確認
    item = 0;
    UpdateParameter(frm, CHKID_HTMLBBS_ENABLE, &(item));
    (NNshGlobal->NNsiParam)->enableNewArrivalHtml = item;
    if ((NNshGlobal->NNsiParam)->enableNewArrivalHtml != 0)
    {
        //  まちBBS/したらば@JBBSで新着確認する場合は、差分取得ではなく
        // 再取得を実施することを警告する
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_WARNMACHINEW, "", 0);

    }

    // 確認メッセージを省略
    item = 0;
    UpdateParameter(frm, CHKID_OMIT_DIALOG, &item);
    if (item == 0)
    {
        // 確認メッセージ省略設定をクリアする
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_NOTHING;
    }
    if ((item != 0)&&((NNshGlobal->NNsiParam)->confirmationDisable == 0))
    {
        // 確認メッセージ省略設定を全て設定する
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
    }

    // DBが更新された印をつける
    NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

    return (errNone);
}


/*-------------------------------------------------------------------------*/
/*   Function :   clearFeatureList                                         */
/*                                            リスト文字列領域をクリアする */
/*-------------------------------------------------------------------------*/
static void clearFeatureList(void)
{
    if (NNshGlobal->featureList != NULL)
    {
        // ラベルの領域ロックをはずす。
        while (NNshGlobal->featureLockCnt != 0)
        {
            MemHandleUnlock((NNshGlobal->featureList)->wordmemH);
            (NNshGlobal->featureLockCnt)--;
        }

        // 機能設定リストが定義されていた場合は領域を解放する
        ReleaseWordList(NNshGlobal->featureList);
        MEMFREE_PTR(NNshGlobal->featureList);
        NNshGlobal->featureList = NULL;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   closeForm_NNsiSettings                                   */
/*                                        NNsi設定系画面のフォームを閉じる */
/*-------------------------------------------------------------------------*/
static void closeForm_NNsiSettings(UInt16 nextFormID)
{
    // リスト領域をクリア
    clearFeatureList();

    // 画面遷移する
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm (nextFormID);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   createFeatureListStrings                                 */
/*                              ストリングリソースから一覧リストを生成する */
/*-------------------------------------------------------------------------*/
static Err createFeatureListStrings(NNshWordList **lst, UInt16 strRscId)
{
    Err        ret;
    MemHandle  memH;
    Char      *ptr;

    // 機能一覧リストが作成済みなら即応答
    if (*lst != NULL)
    {
        return (errNone);
    }

    // 機能一覧リストの管理領域を確保する
    *lst = MEMALLOC_PTR(sizeof(NNshWordList));
    if (*lst == NULL)
    {
        // 領域確保に失敗
        return (errNone);
    }
    MemSet(*lst, sizeof(NNshWordList), 0x00);

    // 一覧機能リストの取得（ストリングリソースを取得）
    memH = DmGetResource('tSTR', strRscId);
    if (memH == 0)
    {
        return (~errNone);
    }
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        return (~errNone);
    }

    // ストリングリソースをリスト形式に分割する
    ret = SeparateWordList(ptr, *lst);
    if (ret != errNone)
    {
        // 文字列の分割に失敗した、管理領域を開放する。
        MEMFREE_PTR(*lst);
    }
    MemHandleUnlock(memH);
    DmReleaseResource(memH);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   keyDownEvt_NNshSetting                                   */
/*                                                        キー入力時の処理 */
/*-------------------------------------------------------------------------*/
static Boolean keyDownEvt_NNshSetting(EventType *event)
{
    FormType *frm;
    UInt16    formID;
    Int16     move;

    // 現在のフォームIDを取得
    formID = FrmGetActiveFormID();
    move   = 0;

    // 5way navigator用キーコードコンバート後、キーコードを判定する
    switch (KeyConvertFiveWayToJogChara(event))
    {
      // ハードキー下を押した時の処理
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case chrDownArrow:
        move = 1;
        break;

      // ハードキー上を押した時の処理
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case chrUpArrow:
        move = -1;
        break;

      // バックボタン/ESCキー/BSキーを押したときの処理
      // case chrBackspace:
      case vchrJogBack:
      case chrEscape:
      case vchrThumbWheelBack:
      case vchrGarminEscape:
        if (formID != FRMID_NNSI_SETTING_ABSTRACT)
        {
            // (変更を保存せず)NNsi設定画面へ遷移する
            closeForm_NNsiSettings(FRMID_NNSI_SETTING_ABSTRACT);
        }
        return (false);
        break;

      case chrCapital_D:    // Dキーの入力
      case chrSmall_D:
      case chrDigitZero:    // ゼロキーの入力
        if (formID != FRMID_NNSI_SETTING_ABSTRACT)
        {
            // NNsi設定(概要以外)は何もせず終了する
            return (false);
        }
        break;

      default: 
        return (false);
        break;
    }

    // 現在開いているフォームＩＤから、次に開くフォームを決定する
    frm = FrmGetActiveForm();
    switch (formID)
    {
      case FRMID_CONFIG_NNSH:
        effect_NNshSetting(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING2 : FRMID_NNSI_SETTING9;
        break;
      case FRMID_NNSI_SETTING2:
        effectNNsiSetting2(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING3 : FRMID_CONFIG_NNSH;
        break;
      case FRMID_NNSI_SETTING3:
        effectNNsiSetting3(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING4 : FRMID_NNSI_SETTING2;
        break;
      case FRMID_NNSI_SETTING4:
        effectNNsiSetting4(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING5 : FRMID_NNSI_SETTING3;
        break;
      case FRMID_NNSI_SETTING5:
        effectNNsiSetting5(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING6 : FRMID_NNSI_SETTING4;
        break;
      case FRMID_NNSI_SETTING6:
        effectNNsiSetting6(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING7 : FRMID_NNSI_SETTING5;
        break;
      case FRMID_NNSI_SETTING7:
        effectNNsiSetting7(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING8 : FRMID_NNSI_SETTING6;
        break;

      case FRMID_NNSI_SETTING8:
        effectNNsiSetting8(frm);
        formID = (move > 0) ? FRMID_NNSI_SETTING9 : FRMID_NNSI_SETTING7;
        break;

      case FRMID_NNSI_SETTING9:
        effectNNsiSetting9(frm);
        formID = (move > 0) ? FRMID_CONFIG_NNSH   : FRMID_NNSI_SETTING8;
        break;

      case FRMID_NNSI_SETTING_ABSTRACT:
        effectNNsiSettingA(frm);
        formID = FRMID_CONFIG_NNSH;
        break;

      default:
        return (false);
        break;
    }

    // 画面遷移する
    closeForm_NNsiSettings(formID);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetTitleHardKey                                          */
/*                                                        ハードキーの設定 */
/*-------------------------------------------------------------------------*/
static Boolean SetTitleHardKey(Char *title, UInt16 data, UInt16 mask, NNshHardkeyControl *info)
{
    Boolean    ret = false;
    FormType  *frm, *prevFrm;
    ListType  *lstP;
    UInt16     items[MINIBUF], *dataP, item;
    Char      *ptr;

    NNshListItem popList[] = {
        { POPTRID_LIST_MULTIUP,   LSTID_MULTIUP   },
        { POPTRID_LIST_MULTIDOWN, LSTID_MULTIDOWN },
        { POPTRID_LIST_HKEY1,     LSTID_HKEY1     },
        { POPTRID_LIST_HKEY2,     LSTID_HKEY2     },
        { POPTRID_LIST_HKEY3,     LSTID_HKEY3     },
        { POPTRID_LIST_HKEY4,     LSTID_HKEY4     },
        { POPTRID_LIST_JOGPUSH,   LSTID_JOGPUSH   },
        { POPTRID_LIST_JOGBACK,   LSTID_JOGBACK   },
        { POPTRID_LIST_CAPTURE,   LSTID_CAPTURE   },
        { POPTRID_LIST_MULTILEFT, LSTID_MULTILEFT   },
        { POPTRID_LIST_MULTIRIGHT, LSTID_MULTIRIGHT },
        { 0, 0 }
    };                 

    // 現在のフォームを取得
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // ダイアログフォームをアクティブにする
    frm = FrmInitForm(FRMID_HARDKEY_SETTING);
    FrmSetActiveForm(frm);

    // ウィンドウタイトルの設定
    FrmCopyTitle(frm, title);

    // 現在のキー機能設定をテーブルに格納
    MemSet(items, sizeof(items), 0x00);
    dataP = items;
    *dataP = (info->up)&(mask);            // ハードキー上の設定
    dataP++;
    *dataP = (info->down)&(mask);          // ハードキー下の設定
    dataP++;
    *dataP = (info->key1)&(mask);          // ハードキー１の設定
    dataP++;
    *dataP = (info->key2)&(mask);          // ハードキー２の設定
    dataP++;
    *dataP = (info->key3)&(mask);          // ハードキー３の設定
    dataP++;
    *dataP = (info->key4)&(mask);          // ハードキー４の設定
    dataP++;
    *dataP = (info->jogPush)&(mask);       // JOG PUSHの設定
    dataP++;
    *dataP = (info->jogBack)&(mask);       // JOG BACKの設定
    dataP++;
    *dataP = (info->clieCapture)&(mask);   // CLIE NXキャプチャボタンの設定
    dataP++;
    *dataP = (info->left)&(mask);          // ハードボタン左の設定
    dataP++;
    *dataP = (info->right)&(mask);         // ハードボタン右の設定
    dataP++;

    // 機能一覧リストをロックする
    ptr = MemHandleLock((NNshGlobal->featureList)->wordmemH);
    NNshWinSetPopItemsWithListArray(frm, popList, ptr,
                                    (NNshGlobal->featureList)->nofWord, items);

    // ダイアログの表示
    if (FrmDoDialog(frm) == BTNID_HARDKEY_OK)
    {
        // ハードキー上の設定
        lstP     = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIUP));
        item     = LstGetSelection(lstP);
        info->up = item | data;

        // ハードキー下の設定
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIDOWN));
        item       = LstGetSelection(lstP);
        info->down = item | data;

        // ハードキー１の設定
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_HKEY1));
        item       = LstGetSelection(lstP);
        info->key1 = item | data;

        // ハードキー２の設定
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_HKEY2));
        item       = LstGetSelection(lstP);
        info->key2 = item | data;

        // ハードキー３の設定
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_HKEY3));
        item       = LstGetSelection(lstP);
        info->key3 = item | data;

        // ハードキー４の設定
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_HKEY4));
        item       = LstGetSelection(lstP);
        info->key4 = item | data;

        // ジョグダイヤルPushボタン
        lstP          = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_JOGPUSH));
        item          = LstGetSelection(lstP);
        info->jogPush = item | data;

        // ジョグダイヤルBackボタン
        lstP          = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_JOGBACK));
        item          = LstGetSelection(lstP);
        info->jogBack = item | data;

        // NX Captureボタン
        lstP              = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_CAPTURE));
        item              = LstGetSelection(lstP);
        info->clieCapture = item | data;

        // ハードキー左の設定
        lstP     = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTILEFT));
        item     = LstGetSelection(lstP);
        info->left = item | data;

        // ハードキー右の設定
        lstP       = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_MULTIRIGHT));
        item       = LstGetSelection(lstP);
        info->right = item | data;

        // DBを更新したしるしをつける
        NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);

        ret = true;
    }

    // 機能一覧リストのロックを解除する
    MemHandleUnlock((NNshGlobal->featureList)->wordmemH);

    // ダイアログを閉じる
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(frm);
    FrmDrawForm(prevFrm);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Handler_NGwordInput                                      */
/*                                  NGワード設定の取得設定イベントハンドラ */
/*                 使用するグローバルな変数                                */
/*                                      NNshGlobal->work1  : 全レコード数  */
/*                                      NNshGlobal->work2  : 表示レコード  */
/*                        NNshGlobal->jumpSelection : 編集中レコードの属性 */
/*                                                        (新規>1, 編集>0) */
/*-------------------------------------------------------------------------*/
Boolean Handler_NGwordInput(EventType *event)
{
    UInt16              keyCode;
    NNshNGwordDatabase  dbData;
    DmOpenRef           dbRef;
    FormType           *frm;

    // 現在のフォームを取得
    switch (event->eType)
    { 
      // メニュー選択
      case menuEvent:
        // 編集イベントを実行
        return (NNsh_MenuEvt_Edit(event));
        break;

      case keyDownEvent:
        frm = FrmGetActiveForm();

        // キーコードによって処理を分岐させる
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrTapWaveSpecificKeyRDown:
          case vchrThumbWheelDown:
          case vchrRockerDown:
          case chrRightArrow:     // カーソルキー右(5way右)
          case vchrRockerRight:
          case vchrJogRight:
          case vchrTapWaveSpecificKeyRRight:
          case chrUnitSeparator:  // カーソルキー(下)押下時の処理
            // 次レコードに移動
            if (NNshGlobal->work2 == 0)
            {
                // データの末尾、更新しない
                return (true);
            }
            // データを更新
            effectNGwordInput(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->jumpSelection = 0;
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrTapWaveSpecificKeyRUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
          case chrLeftArrow:         // カーソルキー左(5way左)
          case vchrRockerLeft:
          case vchrJogLeft:
          case vchrTapWaveSpecificKeyRLeft:
          case chrRecordSeparator:   // カーソルキー(上)押下時の処理
            // 前レコードに移動
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // データの先頭、更新しない
                return (true);
            }
            // データを更新
            effectNGwordInput(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->jumpSelection = 0;
            break;

          case vchrRockerCenter:
          case vchrHardRockerCenter:
          case vchrThumbWheelPush:
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
          case chrEscape:
          default:
            return (false);
            break;
        }
        // NGワード格納用DBの取得
        OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
        if (dbRef == 0)
        {
            // NGワード用DBのオープン失敗、終了する
            return (false);
        }

        // データを読み出す
        MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
        GetRecord_NNsh(dbRef, NNshGlobal->work2, sizeof(NNshNGwordDatabase),
                       &dbData);

        // 読み出したデータを画面に反映させる
        displayNGwordInfo(frm, &dbData);

        // DBをクローズする
        CloseDatabase_NNsh(dbRef);
        return (true);
        break;

      case ctlSelectEvent:
        frm = FrmGetActiveForm();
        switch (event->data.ctlSelect.controlID)
        {
          case BTNID_NGWORD_PREV:
            // 前レコードに移動
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // データの先頭、更新しない
                return (true);
            }
            // データを更新
            effectNGwordInput(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->jumpSelection = 0;
            break;

          case BTNID_NGWORD_NEXT:
            // 次レコードに移動
            if (NNshGlobal->work2 == 0)
            {
                // データの末尾、更新しない
                return (true);
            }

            // データを更新
            effectNGwordInput(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->jumpSelection = 0;
            break;

          case BTNID_NGWORD_NEW:
            // 新規取得先作成(データを末尾に移動する)
            // データを更新
            effectNGwordInput(frm);

            // 空データを画面に反映させる
            MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
            displayNGwordInfo(frm, &dbData);
            NNshGlobal->jumpSelection = 1;
            NNshGlobal->work2 = 0;
            return (true);
            break;

          case BTNID_NGWORD_DELETE:
            // レコード削除
            if ((NNshGlobal->work1 != 0)&&(NNshGlobal->jumpSelection == 0))
            {
                // データを本当に削除してよいか確認する
                if (NNsh_ConfirmMessage(ALTID_CONFIRM, "",
                                        MSG_CONFIRM_DELETE_DATA, 0) == 0)
                {
                    // NGワード格納用DBの取得
                    OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
                    if (dbRef == 0)
                    {
                        // ログDBのオープン失敗、終了する
                        return (false);
                    }
                    // レコードを削除
                    DeleteRecordIDX_NNsh(dbRef, NNshGlobal->work2);
                    (NNshGlobal->work1)--;

                    if (NNshGlobal->work1 == NNshGlobal->work2)
                    {
                        // 末尾レコードを削除した場合...
                        (NNshGlobal->work2)--;
                    }

                    // DBをクローズする
                    CloseDatabase_NNsh(dbRef);
                }
            }            
            break;

          default:
            // 何もしない
            return (false);
            break;
        }
        // NGワード格納用DBの取得
        OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
        if (dbRef == 0)
        {
            // ログDBのオープン失敗、終了する
            return (false);
        }

        // データを読み出す
        MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
        GetRecord_NNsh(dbRef, NNshGlobal->work2, sizeof(NNshNGwordDatabase),
                       &dbData);

        // 読み出したデータを画面に反映させる
        displayNGwordInfo(frm, &dbData);

        // DBをクローズする
        CloseDatabase_NNsh(dbRef);
        return (true);
        break;

      default: 
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effectNGwordInput                                        */
/*                                              表示データのレコードを反映 */
/*                 使用するグローバルな変数                                */
/*                                      NNshGlobal->work1  : 全レコード数  */
/*                                      NNshGlobal->work2  : 表示レコード  */
/*                        NNshGlobal->jumpSelection : 編集中レコードの属性 */
/*                                                        (新規>1, 編集>0) */
/*-------------------------------------------------------------------------*/
static Boolean effectNGwordInput(FormType *frm)
{
    Boolean             ret;
    UInt16              item;
    Char               *ptr;
    NNshNGwordDatabase  dbData;
    FieldType          *fldP;
    MemHandle           txtH;
    
    MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
    
    // NGワードの取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_NGWORD_INPUT));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        MemSet(dbData.ngWord, NGWORD_LEN, 0x00);
        if (*ptr != '\0')
        {
            StrNCopy(dbData.ngWord, ptr, NGWORD_LEN - 1);
            MemHandleUnlock(txtH);
        }
        else
        {
            // NGワードが指定されなかった、終了する
            MemHandleUnlock(txtH);
            return (false);
        }
    }

    // リスト番号(NGをチェックする範囲)の取得と反映
    dbData.checkArea =
       (UInt8) LstGetSelection(FrmGetObjectPtr(frm, 
                                   FrmGetObjectIndex(frm, LSTID_NGWORD_AREA)));
    // 透明あぼーん設定の取得と反映
    item = 0;
    UpdateParameter(frm, CHKID_REPLACEWORD, &item);
    dbData.matchedAction = item;

    // データベースへのデータ登録
    ret = EntryNGword3(NNshGlobal->jumpSelection, NNshGlobal->work2, &dbData);
    if ((ret == true)&&(NNshGlobal->jumpSelection != 0))
    {
        (NNshGlobal->work1)++;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   displayNGwordInfo                                        */
/*                                                          NGワードの表示 */
/*                 使用するグローバルな変数                                */
/*                                      NNshGlobal->work1  : 全レコード数  */
/*                                      NNshGlobal->work2  : 表示レコード  */
/*                        NNshGlobal->jumpSelection : 編集中レコードの属性 */
/*                                                        (新規>1, 編集>0) */
/*-------------------------------------------------------------------------*/
static Boolean displayNGwordInfo(FormType *frm, NNshNGwordDatabase *dbData)
{
    UInt16 item;
    Char   data[MINIBUF];
    
    // 現在表示しているレコード番号の表示 (現在件数 / 全件数)
    MemSet (data, sizeof(data), 0x00);
    if (NNshGlobal->work1 != 0)
    {
        NUMCATI(data, NNshGlobal->work1 - NNshGlobal->work2);
    }
    else
    {
        StrCopy(data, "1");
    }
    StrCat (data, "/");
    NUMCATI(data, NNshGlobal->work1);
    NNshWinSetFieldText(frm, FLDID_NGWORD_INFO, false, data, MINIBUF);

    // ポップアップトリガ(NGワードの有効範囲)の値を設定
    item = dbData->checkArea;
    NNshWinSetPopItems(frm, POPTRID_NGWORD_AREA, LSTID_NGWORD_AREA, item);

    item = dbData->matchedAction;
    SetControlValue(frm, CHKID_REPLACEWORD, &item);

    // NGキーワードの表示
    NNshWinSetFieldText(frm,FLDID_NGWORD_INPUT,true,dbData->ngWord,NGWORD_LEN);

   return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : EntryNGword3                                               */
/*                                                NG設定-3データを登録する */
/*-------------------------------------------------------------------------*/
Boolean EntryNGword3(UInt16 entryMode, UInt16 recNum, NNshNGwordDatabase *ngData)
{
    DmOpenRef           dbRef;

    // NGワード格納用DBの取得
    OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
    if (dbRef == 0)
    {
        // 格納用DBのオープン失敗、終了する
        return (false);
    }

    // 状態により、更新/登録を決定する
    if (entryMode == 0)
    {
        // レコードを更新する
        UpdateRecord_NNsh(dbRef, recNum, sizeof(NNshNGwordDatabase), ngData);
    }
    else
    {
        // レコードを登録する
        EntryRecord_NNsh(dbRef, sizeof(NNshNGwordDatabase), ngData);
    }

    // データベースを閉じる
    CloseDatabase_NNsh(dbRef);

    return (true);    
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetNGword3                                               */
/*                                                          NG設定-3メイン */
/*-------------------------------------------------------------------------*/
Boolean SetNGword3(void)
{
    Boolean              ret = false;
    FormType            *frm, *prevFrm;
    DmOpenRef            dbRef;
    NNshNGwordDatabase   dbData;

    // NGワード用DBの取得
    OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
    if (dbRef == 0)
    {
        // NGワード用DBのオープン失敗、終了する
        return (false);
    }

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &(NNshGlobal->work1));

    // データの初期化
    NNshGlobal->work2 = 0;
    MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);

    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // ダイアログフォームをアクティブにする
    frm = FrmInitForm(FRMID_NGWORD_INPUT);
    FrmSetActiveForm(frm);
   
    // レコードが１件もなかった場合
    if (NNshGlobal->work1 == 0)
    {
        NNshGlobal->jumpSelection = 1;
    }
    else
    {
        // 最後のレコードデータを画面に表示する
        NNshGlobal->work2 = NNshGlobal->work1 - 1;
        GetRecord_NNsh(dbRef, NNshGlobal->work2,
                       sizeof(NNshNGwordDatabase), &dbData);
        NNshGlobal->jumpSelection = 0;
    }
    displayNGwordInfo(frm, &dbData);

    // データベースを閉じる
    CloseDatabase_NNsh(dbRef);

    // イベントハンドラの設定
    FrmSetEventHandler(frm, Handler_NGwordInput);

    // 参照ログ取得の設定ダイアログを表示する
    if (FrmDoDialog(frm) == BTNID_NGWORD_OK)
    {
        // 表示データのレコードを反映させる
        ret = effectNGwordInput(frm);
    }

    // ダイアログを閉じる
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(frm);
    FrmDrawForm(prevFrm);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   selectOmitDialog                                         */
/*                                          確認ダイアログの省略(詳細設定) */
/*-------------------------------------------------------------------------*/
static Boolean selectOmitDialog(void)
{
    Boolean   ret = false;
    FormType *frm, *prevFrm;
    UInt16    status;

    // 現在のチェックボックス設定と、NNsi設定変数の確認を行う
    UpdateParameter(FrmGetActiveForm(), CHKID_OMIT_DIALOG, &status);
    if ((status != 0)&&((NNshGlobal->NNsiParam)->confirmationDisable == 0))
    {
        // 確認メッセージ省略設定を全て設定する
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
    }


    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // ダイアログフォームをアクティブにする
    frm = FrmInitForm(FRMID_OMITDIALOG_DETAIL);
    FrmSetActiveForm(frm);

    // 確認ダイアログを省略する
    if (((NNshGlobal->NNsiParam)->confirmationDisable & (NNSH_OMITDIALOG_CONFIRM)) != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_OMITDIALOG_CONFIRM, &status);
    }

    // 警告ダイアログを表示しない
    if (((NNshGlobal->NNsiParam)->confirmationDisable & (NNSH_OMITDIALOG_WARNING)) != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_OMITDIALOG_WARNING, &status);
    }

    // 情報ダイアログを表示しない
    if (((NNshGlobal->NNsiParam)->confirmationDisable & (NNSH_OMITDIALOG_INFORMATION)) != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_OMITDIALOG_INFORMATION, &status);
    }

    // 詳細ダイアログを表示する
    if (FrmDoDialog(frm) == BTNID_OMITDIALOG_OK)
    {
        (NNshGlobal->NNsiParam)->confirmationDisable = 0;

        // 確認メッセージを省略する
        status = 0;
        UpdateParameter(frm, CHKID_OMITDIALOG_CONFIRM, &status);
        if (status != 0)
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = 
                     (NNshGlobal->NNsiParam)->confirmationDisable | NNSH_OMITDIALOG_CONFIRM;
        }

        // 警告メッセージを表示しない
        status = 0;
        UpdateParameter(frm, CHKID_OMITDIALOG_WARNING, &status);
        if (status != 0)
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = 
                     (NNshGlobal->NNsiParam)->confirmationDisable | NNSH_OMITDIALOG_WARNING;
        }

        // 情報メッセージを表示しない
        status = 0;
        UpdateParameter(frm, CHKID_OMITDIALOG_INFORMATION, &status);
        if (status != 0)
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = 
                 (NNshGlobal->NNsiParam)->confirmationDisable | NNSH_OMITDIALOG_INFORMATION;
        }
        ret = true;
    }

    // ダイアログを閉じる
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(frm);
    FrmDrawForm(prevFrm);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetRoundTripDelayTime                                    */
/*                                                      巡回待ち時間の設定 */
/*-------------------------------------------------------------------------*/
Boolean SetRoundTripDelayTime(void)
{
    Boolean ret;
    Char    buffer[MINIBUF];
    
    MemSet(buffer, sizeof(buffer), 0x00);

    // 巡回待ち時間の初期値を反映させる
    NUMCATI(buffer, (NNshGlobal->NNsiParam)->roundTripDelay);
    
    // 巡回待ち時間の入力...
    ret = DataInputDialog(MSG_TIMEMS_INPUT, buffer, (sizeof(buffer) - 1),
                          NNSH_DIALOG_USE_OTHER, NULL);
    if (ret == true)
    {
        // 設定時間をNNsi設定に反映させる。。。
        (NNshGlobal->NNsiParam)->roundTripDelay = StrAToI(buffer);
    }
    return (ret);
}


/*=========================================================================*/
/*   Function :   Hander_NNshSetting                                       */
/*                                     NNsi設定系の(総合)イベントハンドラ  */
/*=========================================================================*/
Boolean Handler_NNshSetting(EventType *event)
{
    FormType    *frm;
    ControlType *chkObj, *chkObjSub1, *chkObjSub2, *chkObjSub3, *chkObjSub4;
    ControlType *chkObjSub5;
#ifdef USE_COLOR
    IndexedColorType color;
#endif
#ifdef USE_XML_OUTPUT
    Char        *locP;
    UInt16       bakVFS, btnId;
    EventType   *dummyEvent;
#endif
    UInt16       formID;
    Err          ret;
    void        *effectFunc;

    // ボタンチェック以外のイベントは即戻り
    switch (event->eType)
    { 
      case menuEvent:
        // メニュー選択
        switch (event->data.menu.itemID)
        {
          case MNUID_SHOW_HELP:
            // 操作ヘルプの表示
            // (クリップボードに、nnDAに渡すデータについての指示を格納する)
            ClipboardAddItem(clipboardText, 
                             nnDA_NNSIEXT_VIEWSTART
                             nnDA_NNSIEXT_INFONAME
                             nnDA_NNSIEXT_HELPNNSISET
                             nnDA_NNSIEXT_ENDINFONAME
                             nnDA_NNSIEXT_ENDVIEW,
                             sizeof(nnDA_NNSIEXT_VIEWSTART
                                     nnDA_NNSIEXT_INFONAME
                                     nnDA_NNSIEXT_HELPNNSISET
                                     nnDA_NNSIEXT_ENDINFONAME
                                     nnDA_NNSIEXT_ENDVIEW));
            break;

          case MNUID_SHOW_RECVFILE:
            // 受信データファイルの表示
            // (クリップボードに、nnDAに渡すデータについての指示を格納する)
            ClipboardAddItem(clipboardText, 
                             nnDA_NNSIEXT_VIEWSTART
                             nnDA_NNSIEXT_INFONAME
                             nnDA_NNSIEXT_FILERECVFILE
                             nnDA_NNSIEXT_ENDINFONAME
                             nnDA_NNSIEXT_ENDVIEW,
                             sizeof(nnDA_NNSIEXT_VIEWSTART
                                     nnDA_NNSIEXT_INFONAME
                                     nnDA_NNSIEXT_FILERECVFILE
                                     nnDA_NNSIEXT_ENDINFONAME
                                     nnDA_NNSIEXT_ENDVIEW));
            break;

          case MNUID_SHOW_SENDFILE:
            // 送信データファイルの表示
            // (クリップボードに、nnDAに渡すデータについての指示を格納する)
            ClipboardAddItem(clipboardText, 
                             nnDA_NNSIEXT_VIEWSTART
                             nnDA_NNSIEXT_INFONAME
                             nnDA_NNSIEXT_FILESENDFILE
                             nnDA_NNSIEXT_ENDINFONAME
                             nnDA_NNSIEXT_ENDVIEW,
                             sizeof(nnDA_NNSIEXT_VIEWSTART
                                     nnDA_NNSIEXT_INFONAME
                                     nnDA_NNSIEXT_FILESENDFILE
                                     nnDA_NNSIEXT_ENDINFONAME
                                     nnDA_NNSIEXT_ENDVIEW));
            break;

          default:
            // 編集メニューを実行する
            return (NNsh_MenuEvt_Edit(event));
            break;
        }
        // nnDAを起動する
        (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        break;

      case ctlSelectEvent:
        // ボタンが押された(次へ進む)
        break;

      case keyDownEvent:
        // キー入力があった
        return (keyDownEvt_NNshSetting(event));
        break;

#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        // ディスプレイサイズ変更イベント
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

      default: 
        return (false);
        break;
    }

    effectFunc = NULL;
    formID     = FRMID_NNSI_SETTING_ABSTRACT;
    frm        = FrmGetActiveForm();

    // 次に開くフォームIDの設定
    switch (event->data.ctlSelect.controlID)
    {
      // "OK"ボタンがおされたとき(NNsi設定画面に戻る)
      case BTNID_CONFIG_NNSH_OK:
      case BTNID_NNSI_SET_OK:
      case BTNID_NNSISET3_OK:
      case BTNID_NNSISET4_OK:
      case BTNID_NNSISET5_OK:
      case BTNID_NNSISET6_OK:
      case BTNID_NNSISET7_OK:
      case BTNID_NNSISET8_OK:
      case BTNID_NNSISET9_OK:
        formID     = FRMID_NNSI_SETTING_ABSTRACT;
        break;

      // NNsi設定画面で、"OK"/"Cancel"ボタンがおされたとき(一覧画面に戻る)
      case BTNID_NNSISETA_OK:
      case BTNID_NNSISETA_CANCEL:
        formID     = NNshGlobal->backFormId;
        break;

      // "1"ボタンが押されたとき
      case NNSHSET2_1:
      case NNSHSET3_1:
      case NNSHSET4_1:
      case NNSHSET5_1:
      case NNSHSET6_1:
      case NNSHSET7_1:
      case NNSHSET8_1:
      case NNSHSET9_1:
      case SELTRID_NNSISET_NNSI:
        formID     = FRMID_CONFIG_NNSH;
        break;

      // "2"ボタンが押されたとき
      case NNSHSET_2:
      case NNSHSET3_2:
      case NNSHSET4_2:
      case NNSHSET5_2:
      case NNSHSET6_2:
      case NNSHSET7_2:
      case NNSHSET8_2:
      case NNSHSET9_2:
      case SELTRID_NNSISET_THREAD:
        formID     = FRMID_NNSI_SETTING2; 
        break;

      // "3"ボタンが押されたとき
      case NNSHSET_3:
      case NNSHSET2_3:
      case NNSHSET4_3:
      case NNSHSET5_3:
      case NNSHSET6_3:
      case NNSHSET7_3:
      case NNSHSET8_3:
      case NNSHSET9_3:
      case SELTRID_NNSISET_BUTTON:
        formID     = FRMID_NNSI_SETTING3; 
        break;

      // "4"ボタンが押されたとき
      case NNSHSET_4:
      case NNSHSET2_4:
      case NNSHSET3_4:
      case NNSHSET5_4:
      case NNSHSET6_4:
      case NNSHSET7_4:
      case NNSHSET8_4:
      case NNSHSET9_4:
      case SELTRID_NNSISET_MESSAGE:
        formID     = FRMID_NNSI_SETTING4; 
        break;

      // "5"ボタンが押されたとき
      case NNSHSET_5:
      case NNSHSET2_5:
      case NNSHSET3_5:
      case NNSHSET4_5:
      case NNSHSET6_5:
      case NNSHSET7_5:
      case NNSHSET8_5:
      case NNSHSET9_5:
      case SELTRID_NNSISET_WRITE:
        formID     = FRMID_NNSI_SETTING5;
        break;

      // "6"ボタンが押されたとき
      case NNSHSET_6:
      case NNSHSET2_6:
      case NNSHSET3_6:
      case NNSHSET4_6:
      case NNSHSET5_6:
      case NNSHSET7_6:
      case NNSHSET8_6:
      case NNSHSET9_6:
      case SELTRID_NNSISET_JOG:
        formID     = FRMID_NNSI_SETTING6;
        break;

      // "7"ボタンが押されたとき
      case NNSHSET_7:
      case NNSHSET2_7:
      case NNSHSET3_7:
      case NNSHSET4_7:
      case NNSHSET5_7:
      case NNSHSET6_7:
      case NNSHSET8_7:
      case NNSHSET9_7:
      case SELTRID_NNSISET_DEVICE:
        formID     = FRMID_NNSI_SETTING7;
        break;

      // "8"ボタンが押されたとき
      case NNSHSET_8:
      case NNSHSET2_8:
      case NNSHSET3_8:
      case NNSHSET4_8:
      case NNSHSET5_8:
      case NNSHSET6_8:
      case NNSHSET7_8:
      case NNSHSET9_8:
      case SELTRID_NNSISET_LINE:
        formID     = FRMID_NNSI_SETTING8;
        break;

      // "9"ボタンが押されたとき
      case NNSHSET_9:
      case NNSHSET2_9:
      case NNSHSET3_9:
      case NNSHSET4_9:
      case NNSHSET5_9:
      case NNSHSET6_9:
      case NNSHSET7_9:
      case NNSHSET8_9:
      case SELTRID_NNSISET_COMM:
        formID     = FRMID_NNSI_SETTING9;
        break;

#ifdef USE_COLOR
      case  SELTRID_COLOR_UNKNOWN:
        color = (NNshGlobal->NNsiParam)->colorUnknown;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorUnknown = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_BACKGROUND:
        color = (NNshGlobal->NNsiParam)->colorBackGround;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorBackGround = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_OVER:
        color = (NNshGlobal->NNsiParam)->colorOver;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorOver = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_NEW:
        color = (NNshGlobal->NNsiParam)->colorNew;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorNew = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_UPDATE:
        color = (NNshGlobal->NNsiParam)->colorUpdate;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorUpdate = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_REMAIN:
        color = (NNshGlobal->NNsiParam)->colorRemain;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorRemain = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_ALREADY:
        color = (NNshGlobal->NNsiParam)->colorAlready;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorAlready = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_NOTYET:
        color = (NNshGlobal->NNsiParam)->colorNotYet;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorNotYet = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_ERROR:
        color = (NNshGlobal->NNsiParam)->colorError;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorError = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case  SELTRID_COLOR_UPDOWN:
        color = (NNshGlobal->NNsiParam)->colorButton;

        // 色を選択する
        if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
        {
            (NNshGlobal->NNsiParam)->colorButton = color;
        }
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case SELTRID_VIEWCOLOR_DETAIL:
        // 参照画面の色設定ダイアログを表示
        setViewColorDetail();
        return (false);
        break;
#endif

      // 確認ダイアログを省略
      case SELTRID_OMIT_DIALOG:
        // 詳細設定
        if (selectOmitDialog() == true)
        {
            if ((NNshGlobal->NNsiParam)->confirmationDisable != 0)
            {
                formID = 1;
            }
            else
            {
                formID = 0;
            }
            SetControlValue(FrmGetActiveForm(), CHKID_OMIT_DIALOG, &formID);
            FrmDrawForm(FrmGetActiveForm());
        }
        return (false);
        break;
/**
      case SELTRID_HIDE1_WORDSET:
        // NGワード１設定
        DataInputDialog(NNSH_INPUTWIN_NGWORD1,
                        (NNshGlobal->NNsiParam)->hideWord1, HIDEBUFSIZE,
                        NNSH_DIALOG_USE_SETNGWORD, NULL);
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case SELTRID_HIDE2_WORDSET:
        // NGワード２設定
        DataInputDialog(NNSH_INPUTWIN_NGWORD2,
                        (NNshGlobal->NNsiParam)->hideWord2, HIDEBUFSIZE,
                        NNSH_DIALOG_USE_SETNGWORD, NULL);
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;
**/
      case SELTRID_HIDE3_WORDSET:
        // NGワード３設定
        SetNGword3();
        return (false);
        break;

      case SELTRID_GETLOG_SEPARATOR:
        // 参照ログ区切り設定
#ifdef USE_LOGCHARGE
        SetLogCharge_LogToken(0);
        return (false);
#endif // #ifdef USE_LOGCHARGE
        break;

      case SELTRID_GETLOG_DETAIL:
#ifdef USE_LOGCHARGE
        // 参照ログ取得設定
        SetLogGetURL(NNSH_ITEM_BOTTOMITEM);
        return (false);
#endif
        break;

      case SELTRID_SET_LAUNCH_WORD:
        // 起動時パスワード設定
        DataInputDialog(NNSH_INPUTWIN_SETPASS, (NNshGlobal->NNsiParam)->launchPass, PASSSIZE,
                        NNSH_DIALOG_USE_PASS, NULL);
        FrmDrawForm(FrmGetActiveForm());
        return (false);
        break;

      case SELTRID_NNSISET_IMPORT:
#ifdef USE_XML_OUTPUT
        // NNsi設定を外部のXMLファイル(NNsiSet.xml)から読み出す処理
        if ((NNshGlobal->NNsiParam)->useVFS == NNSH_NOTSUPPORT_VFS)
        {
            // VFS未サポート機種の場合、本当に読み出すか設定を行う
            if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_IMPORT_XML, "", 0) == 0)
            {
                // 取り込みの実行実処理
                ret = Input_NNsiSet_XML(FILE_XML_NNSISET);
                if (ret == errNone)
                {
                    // 取り込み正常終了
                    NNsh_InformMessage(ALTID_INFO, MSG_INPUT_XML_DONE, "", 0);

                    // 取り込みが成功した場合、NNsi終了イベントを積む
                    dummyEvent = &(NNshGlobal->dummyEvent);
                    MemSet(dummyEvent, sizeof(EventType), 0x00);
                    dummyEvent->eType = appStopEvent;
                    EvtAddEventToQueue(dummyEvent);
                }
                else
                {
                    // データ読み出し失敗
                    NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", ret);
                }
            }
            // 読み込み終了、画面を開きなおす
            FrmDrawForm(FrmGetActiveForm());
            break;
        }

        // 以降はVFSサポート機種の場合、、、
        bakVFS = (NNshGlobal->NNsiParam)->useVFS;
        btnId = NNsh_ConfirmMessage(ALTID_CONFIRM_DATALOC, MSG_IMPORT_XML_LOC, "", 0);
        switch (btnId)
        {
          case 0:
            // VFSより読み出す(VFSボタンが押されたとき)
            if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
            {
                // NNsi設定が、VFSから読み出す設定になっていなかった場合
                // (無理やりVFS設定に更新する)
                (NNshGlobal->NNsiParam)->useVFS = ((NNSH_VFS_ENABLE)|
                                     (NNSH_VFS_WORKAROUND)|
                                     (NNSH_VFS_USEOFFLINE)|
                                     (NNSH_VFS_DIROFFLINE)|
                                     (NNSH_VFS_DBBACKUP));
                
            }
            locP = " (VFS)";
            break;

          case 1:
            // 無理やりPalm内読み出し設定に更新する(Palmボタンが押されたとき)
            (NNshGlobal->NNsiParam)->useVFS = 0;
            locP = " (Palm)";
            break;

          default:
            // データ読み出し中止、関数を抜ける
            NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
            return (false);
            break;
        }
        // btnIdに取り込み処理前のVFS設定を記憶する
        btnId = (NNshGlobal->NNsiParam)->useVFS;

        // 取り込みの実行実処理
        ret = Input_NNsiSet_XML(FILE_XML_NNSISET);
        if (ret == errNone)
        {
            // 取り込み正常終了
            NNsh_InformMessage(ALTID_INFO, MSG_INPUT_XML_DONE, locP, 0);

            // 取り込みが成功した場合、NNsi終了イベントを積む
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType = appStopEvent;
            EvtAddEventToQueue(dummyEvent);
        }
        else
        {
            // データ読み出し失敗
            NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, locP, 0);
        }

        // VFS設定を保管していた値に戻す
        // (ただし、XML形式のデータ読み込みで値が更新されなかった場合のみ)
        if (btnId == (NNshGlobal->NNsiParam)->useVFS)
        {
            (NNshGlobal->NNsiParam)->useVFS = bakVFS;
        }
        FrmDrawForm(FrmGetActiveForm());
#endif
        // 画面を開きなおす
        break;

      case SELTRID_NNSISET_EXPORT:
#ifdef USE_XML_OUTPUT
        // XMLファイル形式のNNsi設定ファイルを出力する
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // 出力先はVFS
            locP = "VFS";
        }
        else
        {
            // 出力先はPalm内
            locP = "Palm";
        }
        // 本当に出力するか確認を行う。
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_EXPORT_XML, locP, 0) == 0)
        {
            // 出力の実行実処理
            ret = Output_NNsiSet_XML(FILE_XML_NNSISET);
            if (ret == errNone)
            {
                // 出力終了通知
                NNsh_InformMessage(ALTID_INFO, MSG_OUTPUT_XML_DONE, "", 0);
            }
            else
            {
                // エラー発生通知
                NNsh_InformMessage(ALTID_ERROR, MSG_OUTPUT_XML_ERROR, " Code:", ret);
            }
        }
        FrmDrawForm(FrmGetActiveForm());
#endif
        return (false);
        break;

      case SELTRID_USE_HARDKEY_VIEW:
        // リスト文字列のクリア
        clearFeatureList();
      
        // 一覧画面機能リストの設定
        createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREVIEWMSG_STRINGS);

        // 一覧画面ハードキー詳細設定
        (void) SetTitleHardKey(NNSH_TITLE_HARDKEYVIEW_TITLE, MULTIVIEWBTN_FEATURE, 
                                MULTIVIEWBTN_FEATURE_MASK, &((NNshGlobal->NNsiParam)->viewFtr));

        //  一部ハードキーの制御は、NNsi総合設定のハードキー制御設定が必要な
        // ことを警告する。
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_SETTING_HARDKEY, "", 0);
        return (true);
        break;

      case SELTRID_USE_HARDKEY_DETAIL:
        // リスト文字列のクリア
        clearFeatureList();
      
        // 一覧画面機能リストの設定
        createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREMSG_STRINGS);

        // 一覧画面ハードキー詳細設定
        (void) SetTitleHardKey(NNSH_TITLE_HARDKEYSET_TITLE, MULTIBTN_FEATURE, 
                                MULTIBTN_FEATURE_MASK, &((NNshGlobal->NNsiParam)->ttlFtr));

        //  一部ハードキーの制御は、NNsi総合設定のハードキー制御設定が必要な
        // ことを警告する。
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_SETTING_HARDKEY, "", 0);
        return (true);
        break;

      case SELTRID_HARDKEY_CONTROL:
        // リスト文字列のクリア
        clearFeatureList();

        // ハードキー制御設定
        createFeatureListStrings(&(NNshGlobal->featureList), HARDKEY_CONTROL_STRINGS);

        // 全データを(表示だけ) "NNsiで制御" にしておく。
        (NNshGlobal->NNsiParam)->useKey.up      = 1;
        (NNshGlobal->NNsiParam)->useKey.down    = 1;
        (NNshGlobal->NNsiParam)->useKey.jogPush = 1;
        (NNshGlobal->NNsiParam)->useKey.jogBack = 1;

        (void) SetTitleHardKey(NNSH_HARDKEYCONTROL_TITLE, HARDKEY_FEATURE, 
                               HARDKEY_FEATURE_MASK, &((NNshGlobal->NNsiParam)->useKey));
        if (((NNshGlobal->NNsiParam)->useKey.up      != 1)||
            ((NNshGlobal->NNsiParam)->useKey.down    != 1)||
            ((NNshGlobal->NNsiParam)->useKey.jogPush != 1)||
            ((NNshGlobal->NNsiParam)->useKey.jogBack != 1))
        {
            // 設定不能な項目がＯＮになっていたのでそれらはクリアする
            NNsh_WarningMessage(ALTID_WARN, MSG_NOTSET_KEYITEM, "", 0);
        }

        // 全データをクリアする
        (NNshGlobal->NNsiParam)->useKey.up      = 0;
        (NNshGlobal->NNsiParam)->useKey.down    = 0;
        (NNshGlobal->NNsiParam)->useKey.jogPush = 0;
        (NNshGlobal->NNsiParam)->useKey.jogBack = 0;

        // 全体的にハードキーの制御をするのかしないのか判断する
        if (((NNshGlobal->NNsiParam)->useKey.key1 != 0)||((NNshGlobal->NNsiParam)->useKey.key2 != 0)||
            ((NNshGlobal->NNsiParam)->useKey.key3 != 0)||((NNshGlobal->NNsiParam)->useKey.key4 != 0)||
            ((NNshGlobal->NNsiParam)->useKey.clieCapture != 0)||
            ((NNshGlobal->NNsiParam)->useKey.left != 0)||((NNshGlobal->NNsiParam)->useKey.right != 0))
        {
            // ハードキー制御を実施する設定に変更
            (NNshGlobal->NNsiParam)->useHardKeyControl = 1;
        }
        else
        {
            // ハードキー制御を実施しない設定に変更
            (NNshGlobal->NNsiParam)->useHardKeyControl = 0;
        }
        // DBが更新された印をつける
        NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
        return (true);
        break;

      // 詳細表示モードに切り替えた場合
      case NNSHSET_SHOW_DETAIL:
        formID = FrmGetActiveFormID();
        // NNsi設定(概要)だったら、NNsiの詳細設定関連のセレクタトリガを表示する
        if (formID == FRMID_NNSI_SETTING_ABSTRACT)
        {
            FrmShowObject(frm,  FrmGetObjectIndex(frm, LBL_NNSISET_DETAIL));

            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_NNSI));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_THREAD));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_BUTTON));

            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_MESSAGE));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_WRITE));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_JOG));

            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_DEVICE));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_LINE));
            FrmShowObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_COMM));
        }
        FrmDrawForm(frm);
        return (true);
        break;

      case SELTRID_ROUNDTRIP_DELAYTIME:
        // 巡回待ち時間の設定...
        if (SetRoundTripDelayTime() == true)
        {
            // DBが更新された印をつける
            NNshGlobal->updateDatabaseInfo 
               = (NNshGlobal->updateDatabaseInfo)|(NNSH_DB_UPDATE_SETTING);
        }

        // 画面再描画...
        FrmDrawForm(FrmGetActiveForm());
        return (true);
        break;

      // フォーム遷移しないとき
      default:
        // なにもしない
        break;
    }

    // 反映させる関数を設定
    switch (event->data.ctlSelect.controlID)
    {
      // NNsi設定の確定
      case BTNID_CONFIG_NNSH_OK:
      case NNSHSET_2:
      case NNSHSET_3:
      case NNSHSET_4:
      case NNSHSET_5:
      case NNSHSET_6:
      case NNSHSET_7:
      case NNSHSET_8:
      case NNSHSET_9:
        effectFunc = &effect_NNshSetting;
        break;

      // NNsi設定-2の確定
      case BTNID_NNSI_SET_OK:
      case NNSHSET2_1:
      case NNSHSET2_3:
      case NNSHSET2_4:
      case NNSHSET2_5:
      case NNSHSET2_6:
      case NNSHSET2_7:
      case NNSHSET2_8:
      case NNSHSET2_9:
        effectFunc = &effectNNsiSetting2;
        break;

      // NNsi設定-3の確定
      case BTNID_NNSISET3_OK:
      case NNSHSET3_1:
      case NNSHSET3_2:
      case NNSHSET3_4:
      case NNSHSET3_5:
      case NNSHSET3_6:
      case NNSHSET3_7:
      case NNSHSET3_8:
      case NNSHSET3_9:
        effectFunc = &effectNNsiSetting3;
        break;

      // NNsi設定-4の確定
      case BTNID_NNSISET4_OK:
      case NNSHSET4_1:
      case NNSHSET4_2:
      case NNSHSET4_3:
      case NNSHSET4_5:
      case NNSHSET4_6:
      case NNSHSET4_7:
      case NNSHSET4_8:
      case NNSHSET4_9:
        effectFunc = &effectNNsiSetting4;
        break;

      // NNsi設定-5の確定
      case BTNID_NNSISET5_OK:
      case NNSHSET5_1:
      case NNSHSET5_2:
      case NNSHSET5_3:
      case NNSHSET5_4:
      case NNSHSET5_6:
      case NNSHSET5_7:
      case NNSHSET5_8:
      case NNSHSET5_9:
        effectFunc = &effectNNsiSetting5;
        break;

      // NNsi設定-6の確定
      case BTNID_NNSISET6_OK:
      case NNSHSET6_1:
      case NNSHSET6_2:
      case NNSHSET6_3:
      case NNSHSET6_4:
      case NNSHSET6_5:
      case NNSHSET6_7:
      case NNSHSET6_8:
      case NNSHSET6_9:
        effectFunc = &effectNNsiSetting6;
        break;

      // NNsi設定-7の確定
      case BTNID_NNSISET7_OK:
      case NNSHSET7_1:
      case NNSHSET7_2:
      case NNSHSET7_3:
      case NNSHSET7_4:
      case NNSHSET7_5:
      case NNSHSET7_6:
      case NNSHSET7_8:
      case NNSHSET7_9:
        effectFunc = &effectNNsiSetting7;
        break;

      // NNsi設定-8の確定
      case BTNID_NNSISET8_OK:
      case NNSHSET8_1:
      case NNSHSET8_2:
      case NNSHSET8_3:
      case NNSHSET8_4:
      case NNSHSET8_5:
      case NNSHSET8_6:
      case NNSHSET8_7:
      case NNSHSET8_9:
        effectFunc = &effectNNsiSetting8;
        break;

      // NNsi設定-9の確定
      case BTNID_NNSISET9_OK:
      case NNSHSET9_1:
      case NNSHSET9_2:
      case NNSHSET9_3:
      case NNSHSET9_4:
      case NNSHSET9_5:
      case NNSHSET9_6:
      case NNSHSET9_7:
      case NNSHSET9_8:
        effectFunc = &effectNNsiSetting9;
        break;

      // NNsi設定(概略)の確定
      case BTNID_NNSISETA_OK:
      case SELTRID_NNSISET_COMM:
      case SELTRID_NNSISET_LINE:
      case SELTRID_NNSISET_DEVICE:
      case SELTRID_NNSISET_JOG:
      case SELTRID_NNSISET_WRITE:
      case SELTRID_NNSISET_MESSAGE:
      case SELTRID_NNSISET_BUTTON:
      case SELTRID_NNSISET_THREAD:
      case SELTRID_NNSISET_NNSI:
        effectFunc = &effectNNsiSettingA;
        break;

      // "Cancel"ボタンがおされたとき
      case BTNID_CONFIG_NNSH_CANCEL:
      case BTNID_NNSI_SET_CANCEL:
      case BTNID_NNSISET3_CANCEL:
      case BTNID_NNSISET4_CANCEL:
      case BTNID_NNSISET5_CANCEL:
      case BTNID_NNSISET6_CANCEL:
      case BTNID_NNSISET7_CANCEL:
      case BTNID_NNSISET8_CANCEL:
      case BTNID_NNSISET9_CANCEL:
      case BTNID_NNSISETA_CANCEL:
        effectFunc = NULL;
        break;

      case CHKID_USE_VFS:
        // VFS切り替え設定を反映させる
        if ((NNshGlobal->NNsiParam)->useVFS != NNSH_NOTSUPPORT_VFS)
        {
            // VFS の ON/OFFが切り替わったら、画面上のVFSアイテムを更新する。
            chkObj   = FrmGetObjectPtr(frm,
                                       FrmGetObjectIndex(frm, CHKID_USE_VFS));
            chkObjSub1 = FrmGetObjectPtr(frm, 
                                 FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
            chkObjSub2 = FrmGetObjectPtr(frm, 
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
            chkObjSub3 = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
            chkObjSub4 = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));
            chkObjSub5 = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));
            if (CtlGetValue(chkObj) == 0)
            {
                CtlSetValue  (chkObjSub1, 0);
                CtlSetEnabled(chkObjSub1, false);
                CtlSetUsable (chkObjSub1, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));

                CtlSetValue  (chkObjSub2, 0);
                CtlSetEnabled(chkObjSub2, false);
                CtlSetUsable (chkObjSub2, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));

                CtlSetValue  (chkObjSub3, 0);
                CtlSetEnabled(chkObjSub3, false);
                CtlSetUsable (chkObjSub3, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));

                CtlSetValue  (chkObjSub4, 0);
                CtlSetEnabled(chkObjSub4, false);
                CtlSetUsable (chkObjSub4, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));
                
                CtlSetValue  (chkObjSub5, 0);
                CtlSetEnabled(chkObjSub5, false);
                CtlSetUsable (chkObjSub5, false);
                FrmHideObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));

                FrmEraseForm (frm);
            }
            else
            {
                CtlSetEnabled(chkObjSub1, true);
                CtlSetUsable (chkObjSub1, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));

                CtlSetEnabled(chkObjSub2, true);
                CtlSetUsable (chkObjSub2, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));

                CtlSetEnabled(chkObjSub3, true);
                CtlSetUsable (chkObjSub3, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));

                CtlSetEnabled(chkObjSub4, true);
                CtlSetUsable (chkObjSub4, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));

                CtlSetEnabled(chkObjSub5, true);
                CtlSetUsable (chkObjSub5, true);
                FrmShowObject(frm,
                              FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));
            }
            FrmDrawForm(frm);
        }
        return (false);
        break;

      case CHKID_USE_BBS_NEWURL:
        // 標準URLチェックボックスが押された時、BBS一覧取得先を標準URLに変更
        NNshWinSetFieldText(frm, FLDID_URL_BBS, true, URL_BBSTABLE, MAX_URL);
        NNsh_InformMessage(ALTID_INFO, MSG_CHANGED_URL_NEW, "", 0);
        chkObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_BBS_NEWURL));
        CtlSetValue(chkObj, 0);
        return (false);
        break;

      case CHKID_USE_BBS_OLDURL:
        // 旧URLチェックボックスが押されたとき、BBS一覧取得先を旧URLに変更する
        NNshWinSetFieldText(frm,FLDID_URL_BBS,true,URL_BBSTABLE_OLD, MAX_URL);
        NNsh_InformMessage(ALTID_INFO, MSG_CHANGED_URL_OLD, "", 0);
        chkObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_BBS_OLDURL));
        CtlSetValue(chkObj, 0);
        return (false);
        break;

      default:
        // 上記以外(何もしない)
        return (false);
        break;
    }

    // 設定されたデータを反映させる処理
    if (effectFunc != NULL)
    {
        ret =  ((Err (*)())effectFunc)(frm);
        if (ret != errNone)
        {
            // 設定エラー(画面はそのまま)
            return (false);
        }
    }

    // 画面遷移する
    closeForm_NNsiSettings(formID);
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting7                                    */
/*                                NNsi設定-7(デバイス関連)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting7(FormType  *frm)
{
    ControlType *chkObj, *chkObjSub1, *chkObjSub2;
    ControlType *chkObjSub3, *chkObjSub4, *chkObjSub5;
    UInt16       dum;
#ifdef USE_TSPATCH
    UInt32       fontVer;
    Err          ret;
#endif

#ifdef USE_ARMLET
    UInt32       processorType;

    // PalmOS5用機能を使用する
    FtrGet(sysFileCSystem, sysFtrNumProcessorID, &processorType);
    if (sysFtrNumProcessorIsARM(processorType))
    {
        // ARMletの使用
        SetControlValue(frm, CHKID_USE_ARMLET, &((NNshGlobal->NNsiParam)->useARMlet));
    }
    else
#endif
    {
        // ARMletの使用について、設定項目自体を画面に表示しない
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_USE_ARMLET));
        CtlSetUsable(chkObjSub1, false);

        (NNshGlobal->NNsiParam)->useARMlet           = 0;
    }

#ifdef USE_CLIE
    // Silk制御を行わないの表示可否チェック
    if (NNshGlobal->silkVer != 0)
    {
        if (NNshGlobal->notUseSilk != 0)
        {
            dum = 1;
            SetControlValue(frm, CHKID_NOTUSE_SILK, &dum);
        }
    }
    else
#endif
    {
        // (設定項目自体を画面に表示しない)
        chkObjSub1 = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_NOTUSE_SILK));
        CtlSetUsable(chkObjSub1, false);
    }

    // SONYハイレゾ制御を行わない
#ifdef USE_CLIE
    if ((NNshGlobal->notUseHR != 0)||(NNshGlobal->hrRef != 0))
    {
        if ((NNshGlobal->NNsiParam)->disableSonyHR != 0)
        {
            dum = 1;
        }
        else
        {
            dum = 0;
        }
        SetControlValue(frm, CHKID_NOTUSE_SONYHIRES, &dum);
    }
    else
#endif
    {
        // (設定項目自体を画面に表示しない)
        chkObjSub1 = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, 
                                                  CHKID_NOTUSE_SONYHIRES));
        CtlSetUsable(chkObjSub1, false);
    }

    // TsPatch機能を行わない
#ifdef USE_TSPATCH
    ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
    if (ret == errNone)
    {    
        if ((NNshGlobal->NNsiParam)->notUseTsPatch != 0)
        {
            dum = 1;
        }
        else
        {
            dum = 0;
        }
        SetControlValue(frm, CHKID_NOTUSE_TSPATCH, &dum);
    }
    else
#endif
    {
        // (設定項目自体を画面に表示しない)
        chkObjSub1 = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, 
                                                        CHKID_NOTUSE_TSPATCH));
        CtlSetUsable(chkObjSub1, false);
    }

    // 使用VFSの指定
    dum = (UInt16) (NNshGlobal->NNsiParam)->vfsUseCompactFlash;
    NNshWinSetPopItems(frm, POPTRID_VFS_LOCATION, LSTID_VFS_LOCATION, dum);

    // VFS 利用可能時に自動的にVFS ON
    dum = (UInt16) (NNshGlobal->NNsiParam)->vfsOnAutomatic;
    SetControlValue(frm, CHKID_VFSON_AUTOMATIC, &dum);

    // 自動的にVFS ONでもDBチェックしない
    SetControlValue(frm, CHKID_VFSON_NOTDBCHK,&((NNshGlobal->NNsiParam)->vfsOnNotDBCheck));

    // 「VFSの使用」は１つのパラメータで複数の意味合いを共有しているため
    chkObj     = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_USE_VFS));
    chkObjSub1 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFS_WORKAROUND));
    chkObjSub2 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_OFFLINE));
    chkObjSub3 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_DBBACKUP));
    chkObjSub4 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_DBIMPORT));
    chkObjSub5 = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_VFSUSE_DIRMODE));
    switch ((NNshGlobal->NNsiParam)->useVFS)
    {
      // VFS非サポート機の場合、「VFSの使用」設定は表示しない。
      case NNSH_NOTSUPPORT_VFS:
        CtlSetUsable(chkObj,     false);
        CtlSetUsable(chkObjSub1, false);
        CtlSetUsable(chkObjSub2, false);
        CtlSetUsable(chkObjSub3, false);
        CtlSetUsable(chkObjSub4, false);
        CtlSetUsable(chkObjSub5, false);
        break;

      // VFSのチェック
      case NNSH_VFS_DISABLE:
        CtlSetValue  (chkObj,     NNSH_VFS_DISABLE);
        CtlSetValue  (chkObjSub1, 0);
        CtlSetUsable (chkObjSub1, false);

        CtlSetValue  (chkObjSub2, 0);
        CtlSetUsable (chkObjSub2, false);

        CtlSetValue  (chkObjSub3, 0);
        CtlSetUsable (chkObjSub3, false);

        CtlSetValue  (chkObjSub4, 0);
        CtlSetUsable (chkObjSub4, false);

        CtlSetValue  (chkObjSub5, 0);
        CtlSetUsable (chkObjSub5, false);
        break;

      case NNSH_VFS_ENABLE:
      default:
        CtlSetValue  (chkObj,     NNSH_VFS_ENABLE);
        CtlSetUsable (chkObjSub1, true);
        CtlSetValue  (chkObjSub1, (NNSH_VFS_WORKAROUND & (NNshGlobal->NNsiParam)->useVFS));

        CtlSetUsable (chkObjSub2, true);
        CtlSetValue  (chkObjSub2, (NNSH_VFS_USEOFFLINE & (NNshGlobal->NNsiParam)->useVFS));

        CtlSetUsable (chkObjSub3, true);
        CtlSetValue  (chkObjSub3, (NNSH_VFS_DBBACKUP & (NNshGlobal->NNsiParam)->useVFS));

        CtlSetUsable (chkObjSub4, true);
        CtlSetValue  (chkObjSub4, (NNSH_VFS_DBIMPORT & (NNshGlobal->NNsiParam)->useVFS));

        CtlSetUsable (chkObjSub5, true);
        CtlSetValue  (chkObjSub5, (NNSH_VFS_DIROFFLINE & (NNshGlobal->NNsiParam)->useVFS));
        break;
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting8                                    */
/*                                    NNsi設定-8(通信関連)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting8(FormType  *frm)
{
    UInt16       timeout;
    Char         numBuf[TINYBUF];

    // GZIPを使用する設定
    timeout = (NNshGlobal->NNsiParam)->useGZIP;
    SetControlValue(frm, CHKID_USE_GZIP, &(timeout));

    // Proxy経由でのアクセス
    SetControlValue(frm, CHKID_USE_PROXY, &((NNshGlobal->NNsiParam)->useProxy));

    // Proxy URL
    if ((NNshGlobal->NNsiParam)->proxyURL[0] != '\0')
    { 
        NNshWinSetFieldText(frm, FLDID_USE_PROXY, false,
                            (NNshGlobal->NNsiParam)->proxyURL, MAX_URL);
    }

    // Proxy Port #
    if ((NNshGlobal->NNsiParam)->proxyPort != 0)
    {
        MemSet (numBuf, sizeof(numBuf), 0x00);
        StrIToA(numBuf, (NNshGlobal->NNsiParam)->proxyPort);
        NNshWinSetFieldText(frm,FLDID_PROXY_PORT, false, numBuf, TINYBUF);
    }

    // フィールドに現在のタイムアウト値を反映させる
    MemSet(numBuf, sizeof(numBuf), 0x00);
    if ((NNshGlobal->NNsiParam)->netTimeout == -1)
    {
        // タイムアウト無限大ならば、最大値に設定する
        StrIToA(numBuf, MAX_TIMEOUT_VALUE);
    }
    else
    {
        // パラメータには Tickも含んでいるので除算を実施して求める
        timeout = SysTicksPerSecond();
        timeout = (timeout == 0) ? 1 : timeout;  // ゼロ除算の回避(保険)
        StrIToA(numBuf, ((NNshGlobal->NNsiParam)->netTimeout / timeout));
    }
    NNshWinSetFieldText(frm,FLDID_COMM_TIMEOUT, false, numBuf, TINYBUF);

    // フィールドに現在のバッファサイズを反映させる
    MemSet(numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, (NNshGlobal->NNsiParam)->bufferSize);
    NNshWinSetFieldText(frm,FLDID_BUFFER_SIZE,false,numBuf, TINYBUF);

    // フィールドに現在の分割取得サイズを反映させる
    MemSet(numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, (NNshGlobal->NNsiParam)->partGetSize);
    NNshWinSetFieldText(frm,FLDID_DL_PARTSIZE,false,numBuf, TINYBUF);


    // フィールドに通信タイムアウト時リトライ回数を反映させる
    MemSet (numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, (NNshGlobal->NNsiParam)->nofRetry);
    NNshWinSetFieldText(frm,FLDID_TIMEOUT_RETRY,false,numBuf,sizeof(numBuf));

    // 分割ダウンロード指定を画面に反映する
    SetControlValue(frm, CHKID_DL_PART, &((NNshGlobal->NNsiParam)->enablePartGet));

    // BBS一覧を上書きする
    SetControlValue(frm, CHKID_BBS_OVERWRITE, &((NNshGlobal->NNsiParam)->bbsOverwrite));

    // BBS一覧取得先URLの設定
    NNshWinSetFieldText(frm, FLDID_URL_BBS, true, (NNshGlobal->NNsiParam)->bbs_URL, MAX_URL);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting5                                    */
/*                                NNsi設定-5(書き込み画面)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting5(FormType  *frm)
{
    UInt16       item;
    ControlType  *objP;
    Char         numBuf[TINYBUF];

    // 書き込み画面終了時レスを自動で保存
    SetControlValue(frm, CHKID_WRITE_AUTOSAVE,
                                       &((NNshGlobal->NNsiParam)->writeMessageAutoSave));

    // 書き込み時PUSHボタンを無効
    item = (NNshGlobal->NNsiParam)->writeJogPushDisable;
    SetControlValue(frm, CHKID_WRITEPUSH_DISABLE, &(item));

    // 受信Cookieを使用して書き込み
    SetControlValue(frm, CHKID_WRITE_USE_COOKIE, &((NNshGlobal->NNsiParam)->useCookieWrite));

    // 書きこみ時、書きこみフィールドに自動的にフォーカスを移動しない
    item = (NNshGlobal->NNsiParam)->notAutoFocus;
    SetControlValue(frm, CHKID_NOT_AUTOFOCUSSET, &item);

#ifdef USE_CLIE
    // Silk制御を行わないの表示可否チェック
    if (NNshGlobal->silkVer != 0)
    {
        // 書きこみ時、シルクを拡大しない
        item = (NNshGlobal->NNsiParam)->notOpenSilkWrite;
        SetControlValue(frm, CHKID_NOTSILK_WRITE, &item);
    }
    else
#endif
    {
        // (設定項目自体を画面に表示しない)
        objP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_NOTSILK_WRITE));
        CtlSetUsable(objP, false);
    }

    // 書き込み時、アンダーラインをOFFにする
    SetControlValue(frm, CHKID_UNDERLINE_WRITE,
                    &((NNshGlobal->NNsiParam)->disableUnderlineWrite));

    // 書き込み時参照スレ番号を挿入する
    SetControlValue(frm, CHKID_INSERT_REPLYNUM, &((NNshGlobal->NNsiParam)->insertReplyNum));

    // 書き込み時引用カキコにする
    item = (NNshGlobal->NNsiParam)->writingReplyMsg;
    SetControlValue(frm, CHKID_INSERT_REPLYMSG, &item);

    // 書き込みシーケンスPART2
    SetControlValue(frm, CHKID_WRITE_SEQUENCE2, &((NNshGlobal->NNsiParam)->writeSequence2));

    // sage書き込み
    SetControlValue(frm, CHKID_WRITE_SAGE,  &((NNshGlobal->NNsiParam)->writeAlwaysSage));

    // コテハン機能を使用する設定を画面に反映する
    SetControlValue(frm, CHKID_FIXED_HANDLE, &((NNshGlobal->NNsiParam)->useFixedHandle));

    // ハンドル名の設定
    if ((NNshGlobal->NNsiParam)->handleName[0] != '\0')
    { 
      NNshWinSetFieldText(frm, FLDID_HANDLENAME, false,
                            (NNshGlobal->NNsiParam)->handleName, BUFSIZE);
    }

    // フィールドに書き込みバッファサイズ
    MemSet (numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, (NNshGlobal->NNsiParam)->writeBufferSize);
    NNshWinSetFieldText(frm,FLDID_WRITEBUF_SIZE, true, numBuf, TINYBUF);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting6                                    */
/*                                  NNsi設定-6(ジョグ関連)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting6(FormType  *frm)
{
    UInt16 item;

    // 参照時JumpListに「一覧へ」
    SetControlValue(frm, CHKID_ADD_RTNTOLIST, &((NNshGlobal->NNsiParam)->addReturnToList));

    // 参照時JumpListに「ひとつ戻る」
    SetControlValue(frm,CHKID_ADD_BACKMESSAGE,&((NNshGlobal->NNsiParam)->addMenuBackRtnMsg));

    // 参照時JumpListに「メニュー」
    SetControlValue(frm, CHKID_ADD_OPENMENU, &((NNshGlobal->NNsiParam)->addMenuMsg));

    // 参照時JumpListに「差分取得」
    SetControlValue(frm, CHKID_ADD_GETPART, &((NNshGlobal->NNsiParam)->addLineGetPart));

    // 参照時JumpListに「回線切断」
    SetControlValue(frm, CHKID_ADD_LINEDISCONN, &((NNshGlobal->NNsiParam)->addLineDisconn));

    // 参照時JumpListに先頭へ
    SetControlValue(frm, CHKID_ADD_JUMPTOP, &((NNshGlobal->NNsiParam)->addJumpTopMsg));

    // 参照時JumpListに末尾へ
    SetControlValue(frm, CHKID_ADD_JUMPBOTTOM, &((NNshGlobal->NNsiParam)->addJumpBottomMsg));

    // 参照時JumpListに全選択+Webブラウザで開く
    SetControlValue(frm, CHKID_ADD_OPENWEB, &((NNshGlobal->NNsiParam)->addMenuSelAndWeb));

    // 参照時JumpListに描画モード切替
    SetControlValue(frm, CHKID_ADD_GRAPHVIEW, &((NNshGlobal->NNsiParam)->addMenuGraphView));

    // 参照時JumpListにお気に入り設定切り替え
    item = (NNshGlobal->NNsiParam)->addMenuFavorite;
    SetControlValue(frm, CHKID_ADD_FAVORITE, &item);

    // 参照時JumpListにスレ番号指定取得
    item = (NNshGlobal->NNsiParam)->addMenuGetThreadNum;
    SetControlValue(frm,CHKID_ADD_GETTHREADNUM,&item);

    // 参照時JumpListにメモ帳出力
    item = (NNshGlobal->NNsiParam)->addMenuOutputMemo;
    SetControlValue(frm,CHKID_ADD_OUTPUTMEMO,&item);

    // 一覧時Menuにメニューを追加
    item = (NNshGlobal->NNsiParam)->addMenuTitle;
    SetControlValue(frm, CHKID_ADD_MENUOPEN, &item);

    // 一覧時Menuに参照COPY
    SetControlValue(frm, CHKID_ADD_MENUCOPY, &(NNshGlobal->NNsiParam)->addMenuCopyMsg);

    // 一覧時MenuにMSG削除
    SetControlValue(frm, CHKID_ADD_MENUDELETE, &(NNshGlobal->NNsiParam)->addMenuDeleteMsg);

    // 一覧時Menuに描画モード変更
    SetControlValue(frm,CHKID_ADD_MENUGRAPHMODE,&(NNshGlobal->NNsiParam)->addMenuGraphTitle);

    // 一覧時Menuに多目的スイッチ１
    SetControlValue(frm, CHKID_ADD_MENUMULTISW1, &(NNshGlobal->NNsiParam)->addMenuMultiSW1);

    // 一覧時Menuに多目的スイッチ２
    SetControlValue(frm, CHKID_ADD_MENUMULTISW2, &(NNshGlobal->NNsiParam)->addMenuMultiSW2);

    // 一覧時MenuにNNsi終了
    SetControlValue(frm, CHKID_ADD_NNSIEND, &(NNshGlobal->NNsiParam)->addMenuNNsiEnd);

    // 一覧時Menuにデバイス情報
    SetControlValue(frm, CHKID_ADD_MENUDEVICEINFO, &(NNshGlobal->NNsiParam)->addMenuDeviceInfo);

    // 参照時Menuにデバイス情報
    item = (NNshGlobal->NNsiParam)->addDeviceInfo;
    SetControlValue(frm, CHKID_ADD_DEVICEINFO, &item);

    // 参照時MenuにNG3設定
    item = (NNshGlobal->NNsiParam)->addNgSetting3;
    SetControlValue(frm, CHKID_ADD_NG3SET, &item);

    // 一覧時MenuにDir選択
    SetControlValue(frm, CHKID_ADD_MENUDIRSELECT, &(NNshGlobal->NNsiParam)->addMenuDirSelect);

    // MenuにBt On/Off表示
    item = (NNshGlobal->NNsiParam)->addBtOnOff;
    SetControlValue(frm, CHKID_ADD_BT_ONOFF, &item);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNshSetting                                     */
/*                                      NNsi設定(NNsi総合)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNshSetting(FormType *frm)
{
    ControlType *ctlObj;
    ListType    *lstP;
    UInt16       item;
    Char        *ptr;

    // 参照ログへコピーしたとき、同時にスレ削除を実施
    SetControlValue(frm, CHKID_COPYDEL_READONLY,&((NNshGlobal->NNsiParam)->copyDelReadOnly));


    // 板URL取得時、有効チェックなし
    SetControlValue(frm, CHKID_NOTCHECK_BBSURL,&((NNshGlobal->NNsiParam)->notCheckBBSURL));

    // 板移動時に未取得スレ削除
    SetControlValue(frm, CHKID_AUTODEL_NOT_YET,&((NNshGlobal->NNsiParam)->autoDeleteNotYet));

    // カナを半角→全角変換
    SetControlValue(frm,CHKID_CONVERT_HANZEN, &((NNshGlobal->NNsiParam)->convertHanZen));

    // Offlineスレ検索時にスレ消去を実施しない
    item = (NNshGlobal->NNsiParam)->notDelOffline;
    SetControlValue(frm, CHKID_NOT_DELOFFLINE, &item);

    // お気に入りを表示する最低スレレベルのラベル設定(とリストアイテム設定)
    switch ((NNshGlobal->NNsiParam)->displayFavorLevel)
    {
      case NNSH_MSGATTR_FAVOR_L2:
        item = 1;
        ptr  = NNSH_ATTRLABEL_FAVOR_L2;
        break;

      case NNSH_MSGATTR_FAVOR_L3:
        item = 2;
        ptr  = NNSH_ATTRLABEL_FAVOR_L3;
        break;

      case NNSH_MSGATTR_FAVOR_L4:
        item = 3;
        ptr  = NNSH_ATTRLABEL_FAVOR_L4;
        break;

      case NNSH_MSGATTR_FAVOR:
        item = 4;
        ptr  = NNSH_ATTRLABEL_FAVOR;
        break;

      case NNSH_MSGATTR_FAVOR_L1:
      default:
        item = 0;
        ptr  = NNSH_ATTRLABEL_FAVOR_L1;
        break;
    }

    ctlObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, POPTRID_FAVOR_LEVEL));
    CtlSetLabel(ctlObj, ptr);

    lstP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_FAVOR_LEVEL));
    LstSetSelection(lstP, item);
    LstSetTopItem  (lstP, item);


    /***  NNsh設定を(グローバル変数から)反映させる  ***/

    // 確認メッセージを省略
    SetControlValue(frm, CHKID_OMIT_DIALOG, &((NNshGlobal->NNsiParam)->confirmationDisable));

    // 検索時に、大文字小文字を区別しない
    SetControlValue(frm, CHKID_SEARCH_CASELESS, &((NNshGlobal->NNsiParam)->searchCaseless));

    // 起動時にOfflineスレを検索
    SetControlValue(frm, CHKID_OFFCHK_LAUNCH,   &((NNshGlobal->NNsiParam)->offChkLaunch));

    // i-mode用のURLを使用する
    SetControlValue(frm, CHKID_USE_IMODEURL, &((NNshGlobal->NNsiParam)->useImodeURL));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting2                                    */
/*                                    NNsi設定-2(一覧画面)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting2(FormType  *frm)
{
    UInt16       item;
    ControlType *ctlObj;

    // スレ一覧(subject.txt)全取得
    SetControlValue(frm, CHKID_SUBJECT_ALL, &((NNshGlobal->NNsiParam)->getAllThread));

    // スレ一覧画面でtiny/smallフォントを使用する
    SetControlValue(frm,CHKID_CLIE_USE_TITLE,
                                     &((NNshGlobal->NNsiParam)->useSonyTinyFontTitle));

    // スレタイトルを重複させない
    SetControlValue(frm, CHKID_CHECK_DUPLICATE,&((NNshGlobal->NNsiParam)->checkDuplicateThread));

    
    // ”スレ先頭の表示”設定
    if ((NNshGlobal->NNsiParam)->printNofMessage == 0)
    {
        // スレ先頭にレス数表示しない
        item = 0;
    }
    else
    {
        switch ((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum)
        {
          case 1:
            item = 2;  // 未読数表示
            break;
            
          case 2:
            item = 3;  // レベル + レス数
            break;

          case 3:
            item = 4;  // レベル + 未読数
            break;
            
          default:
            item = 1;  // レス数
            break;
        }
    }
    
    // 表示モードを反映させる
    NNshWinSetPopItems(frm, POPTRID_PRINT_HEADER, LSTID_PRINT_HEADER, item);

    // 取得済み全てには参照ログを表示しない
    item = (NNshGlobal->NNsiParam)->notListReadOnly;
    SetControlValue(frm, CHKID_NOT_READONLY, &(item));

    // 未読ありには参照ログを表示しない
    item = (NNshGlobal->NNsiParam)->notListReadOnlyNew;
    SetControlValue(frm, CHKID_NOT_READONLY_NEW, &(item));

    // スレ取得後にレス参照
    SetControlValue(frm, CHKID_OPEN_AUTO,   &((NNshGlobal->NNsiParam)->openAutomatic));


#ifdef USE_COLOR
    // 一覧表示時にカラー(OS3.5以上で設定項目を有効にする)
    if (NNshGlobal->palmOSVersion >= 0x03503000)
    {
        // カラーの使用設定
        SetControlValue(frm, CHKID_USE_COLORMODE, &((NNshGlobal->NNsiParam)->useColor));

        // 本当は、セレクタトリガのキャプションに色をつけたいが...
        // （よくわからんので省略）
    }
    else
#endif
    {
        (NNshGlobal->NNsiParam)->useColor = 0;

        // (設定項目自体を画面に表示しない)
        FrmCopyLabel(frm, LBL_SEL_COLOR, "  ");

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, CHKID_USE_COLORMODE));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm, SELTRID_COLOR_UNKNOWN));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                            FrmGetObjectIndex(frm, SELTRID_COLOR_BACKGROUND));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_OVER));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_NEW));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_UPDATE));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_REMAIN));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, SELTRID_COLOR_ALREADY));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_NOTYET));
        CtlSetUsable(ctlObj, false);

        ctlObj = FrmGetObjectPtr(frm,
                                 FrmGetObjectIndex(frm, SELTRID_COLOR_ERROR));
        CtlSetUsable(ctlObj, false);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting3                                    */
/*                                  NNsi設定-3(一覧ボタン)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting3(FormType  *frm)
{
    UInt16       selItem;

    // 一覧画面機能リストの設定
    createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREMSG_STRINGS);

    // 現在のフォームへのポインタを取得
    frm = FrmGetActiveForm();

    // 多目的スイッチ１(一覧画面)のラベル設定(とリストアイテム設定)
    selItem = 
             (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1 & (NNSH_SWITCHUSAGE_FUNCMASK);
    NNshWinSetPopItems(frm, POPTRID_LIST_FUNCSW1, LSTID_TITLE_FUNCSW1, selItem);

    // 多目的スイッチ２(一覧画面)のラベル設定(とリストアイテム設定)
    selItem = 
             (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2 & (NNSH_SWITCHUSAGE_FUNCMASK);
    NNshWinSetPopItems(frm, POPTRID_LIST_FUNCSW2,LSTID_TITLE_FUNCSW2,selItem);

    // 多目的ボタン１の設定を画面に反映させる
    selItem = ((NNshGlobal->NNsiParam)->multiBtn1Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN1, LSTID_MULTIBTN1,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN1, false,
                        (NNshGlobal->NNsiParam)->multiBtn1Caption, MAX_CAPTION);

    // 多目的ボタン２の設定を画面に反映させる
    selItem = ((NNshGlobal->NNsiParam)->multiBtn2Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN2, LSTID_MULTIBTN2,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN2, false,
                        (NNshGlobal->NNsiParam)->multiBtn2Caption, MAX_CAPTION);

    // 多目的ボタン３の設定を画面に反映させる
    selItem = ((NNshGlobal->NNsiParam)->multiBtn3Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN3, LSTID_MULTIBTN3,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN3, false,
                        (NNshGlobal->NNsiParam)->multiBtn3Caption, MAX_CAPTION);

    // 多目的ボタン４の設定を画面に反映させる
    selItem = ((NNshGlobal->NNsiParam)->multiBtn4Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN4, LSTID_MULTIBTN4,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN4, false,
                        (NNshGlobal->NNsiParam)->multiBtn4Caption, MAX_CAPTION);

    // 多目的ボタン５の設定を画面に反映させる
    selItem = ((NNshGlobal->NNsiParam)->multiBtn5Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN5, LSTID_MULTIBTN5,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN5, false,
                        (NNshGlobal->NNsiParam)->multiBtn5Caption, MAX_CAPTION);

    // 多目的ボタン６の設定を画面に反映させる
    selItem = ((NNshGlobal->NNsiParam)->multiBtn6Feature)&(MULTIBTN_FEATURE_MASK);
    NNshWinSetPopItemsWithList(frm, POPTRID_LIST_MULTIBTN6, LSTID_MULTIBTN6,
                               NNshGlobal->featureList, selItem);
    (NNshGlobal->featureLockCnt)++;

    NNshWinSetFieldText(frm, FLDID_MULTIBTN6, true,
                        (NNshGlobal->NNsiParam)->multiBtn6Caption, MAX_CAPTION);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting9                                    */
/*                                   NNsi設定-9(新着/回線)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting9(FormType  *frm)
{
    Err          ret;
    UInt16       lp;
    ControlType *chkObj;

    // NNsi終了時に回線を切断する
    lp = (NNshGlobal->NNsiParam)->disconnectNNsiEnd;
    SetControlValue(frm, CHKID_END_OFFLINE, &(lp));

    // 「新着確認」終了時に回線を切断する
    lp = (NNshGlobal->NNsiParam)->disconnArrivalEnd;
    SetControlValue(frm,CHKID_DISCONN_ARRIVAL,&(lp));

    // 「新着確認」終了後に未読一覧を表示する
    lp = (NNshGlobal->NNsiParam)->autoOpenNotRead;
    SetControlValue(frm, CHKID_ARRIVAL_NOTREAD, &(lp));

    // 「新着確認」前にDAを起動する
    lp = (NNshGlobal->NNsiParam)->preOnDAnewArrival;
    SetControlValue(frm, CHKID_PREPARE_DA, &(lp));

    //  NNsi起動時にマクロを実行
#ifdef USE_MACRO
    lp = (NNshGlobal->NNsiParam)->autostartMacro;
    SetControlValue(frm, CHKID_AUTOSTARTMACRO, &(lp));
#else
    // マクロ機能が有効でない場合にはアイテムを消す
    CtlSetUsable(FrmGetObjectPtr(frm, 
                         FrmGetObjectIndex(frm, CHKID_AUTOSTARTMACRO)), false);
#endif

    // 「新着確認」終了後にビープ音を鳴らす
    lp = (NNshGlobal->NNsiParam)->autoBeep;
    SetControlValue(frm, CHKID_DONE_BEEP, &(lp));

    // 「新着確認」終了後にマクロ実行
#ifdef USE_MACRO
    lp = (NNshGlobal->NNsiParam)->startMacroArrivalEnd;
    SetControlValue(frm, CHKID_EXECUTE_MACRO, &(lp));
#else
    // マクロ機能が有効でない場合にはアイテムを消す
    CtlSetUsable(FrmGetObjectPtr(frm, 
                         FrmGetObjectIndex(frm, CHKID_EXECUTE_MACRO)), false);
#endif

    // 回線未接続時は取得予約
    SetControlValue(frm,CHKID_GETRESERVEFEATURE,&(NNshGlobal->NNsiParam)->getReserveFeature);

    // 回線接続後画面を再描画
    SetControlValue(frm,CHKID_REDRAW_CONNECT,&((NNshGlobal->NNsiParam)->redrawAfterConnect));

    // まちBBS/したらば@JBBSも新着確認
    lp = (NNshGlobal->NNsiParam)->enableNewArrivalHtml;
    SetControlValue(frm, CHKID_HTMLBBS_ENABLE, &(lp));

    // 新着時エラーになれば再取得
    SetControlValue(frm, CHKID_ALLUPDATE_ABORT,
                                             &((NNshGlobal->NNsiParam)->autoUpdateGetError));

    // 一覧取得時に新着確認
    SetControlValue(frm, CHKID_UPDATEMSG_LIST, &((NNshGlobal->NNsiParam)->listAndUpdate));

    // 新着確認時にメールチェック
    if (CheckInstalledResource_NNsh('DAcc', 'cMDA') == false)
    {
        // cMDAがインストールされていない場合には、設定項目を消去する
        CtlSetUsable(FrmGetObjectPtr(frm, 
                         FrmGetObjectIndex(frm, CHKID_cMDA_EXECUTE)), false);
    }
    else
    {
        // 設定情報を画面に反映させる
        SetControlValue(frm, CHKID_cMDA_EXECUTE, &((NNshGlobal->NNsiParam)->use_DAplugin));
    }

    // BBS一覧の作成
    ret = Create_BBS_INDEX(&(NNshGlobal->bbsTitles), &lp);
    if (ret == errNone)
    {
        // BBS一覧のリスト状態を反映する。
        NNshWinSetListItems(frm, LSTID_CHECK_NEWARRIVAL,
                            NNshGlobal->bbsTitles, lp,
                            (NNshGlobal->NNsiParam)->newArrivalNotRead,
                            &(NNshGlobal->bbsTitleH), NULL);

        // ポップアップトリガのラベルを(リスト状態に合わせ)更新する
        NNshWinSetPopItems(frm, POPTRID_NEWARRIVAL, LSTID_CHECK_NEWARRIVAL,
                           (NNshGlobal->NNsiParam)->newArrivalNotRead);
    }


    // WebBrowserを開くとき最新50
    if (NNshGlobal->browserCreator != 0)
    {
        if ((NNshGlobal->NNsiParam)->browseMesNum != 0)
        {
            lp = 1;
            SetControlValue(frm, CHKID_WEBBROWSE_LAST50, &lp);
        }
    }
    else
    {
        // (設定項目自体を画面に表示しない)
        chkObj = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, CHKID_WEBBROWSE_LAST50));
        CtlSetUsable(chkObj, false);
    }

#ifdef USE_LOGCHARGE
    // 参照ログ取得レベルの設定(ポップアップトリガのラベルを更新する)
    lp = (NNshGlobal->NNsiParam)->getROLogLevel;
    NNshWinSetPopItems(frm, POPTRID_CHECK_READONLY, LSTID_CHECK_READONLY, lp);

#else
    // ログチャージ機能を使用しない場合は、項目を表示しない
    chkObj = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, SELTRID_GETLOG_DETAIL));
    CtlSetUsable(chkObj, false);

    chkObj = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, SELTRID_GETLOG_SEPARATOR));
    CtlSetUsable(chkObj, false);

    chkObj = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, POPTRID_CHECK_READONLY));
    CtlSetUsable(chkObj, false);

#endif
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSetting4                                    */
/*                                    NNsi設定-4(参照画面)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSetting4(FormType  *frm)
{
    Char         numBuf[TINYBUF];
    UInt16       item;

    // レス数
    if ((NNshGlobal->NNsiParam)->msgNumLimit != 0)
    {
        MemSet (numBuf, sizeof(numBuf), 0x00);
        StrIToA(numBuf, (NNshGlobal->NNsiParam)->msgNumLimit);
        NNshWinSetFieldText(frm, FLDID_RESNUM_SIZE, false, numBuf, TINYBUF);
    }

    // RAW表示モード
    item = (NNshGlobal->NNsiParam)->rawDisplayMode;
    SetControlValue(frm, CHKID_USE_RAWDISPLAY, &item);

    // しおり設定機能を使用する
    SetControlValue(frm, CHKID_USE_BOOKMARK,&((NNshGlobal->NNsiParam)->useBookmark));

    // 名前欄を簡略化する
    item = (NNshGlobal->NNsiParam)->useNameOneLine;
    SetControlValue(frm, CHKID_INFO_ONELINE, &item);

    // スレ番号を【】にする
    item = (NNshGlobal->NNsiParam)->boldMessageNum;
    SetControlValue(frm, CHKID_BOLD_MSGNUM, &item);

    // 参照時グラフィック描画モードを使用する
    SetControlValue(frm,CHKID_CLIE_USE_TINYFONT,&((NNshGlobal->NNsiParam)->useSonyTinyFont));

    // 参照時アンダーラインOFF
    SetControlValue(frm, CHKID_MSG_LINEOFF, &((NNshGlobal->NNsiParam)->disableUnderline));

    // PUSH ONダイヤルで±１
    SetControlValue(frm, CHKID_NOT_CURSOR, &((NNshGlobal->NNsiParam)->notCursor));

    // スレ参照画面でカーソル表示しない
    item = (NNshGlobal->NNsiParam)->notFocusNumField;
    SetControlValue(frm, CHKID_NOT_FOCUSSET_NUM, &item);

    // スレ参照画面でGoメニューアンカーを整列させる
    item = (NNshGlobal->NNsiParam)->sortGoMenuAnchor;
    SetControlValue(frm, CHKID_GO_SORT, &item);

    // 参照時レスをゾーンで表示
    SetControlValue(frm,CHKID_BLOCK_DISP_MODE, &((NNshGlobal->NNsiParam)->blockDispMode));

    // 参照時前に移動で前レス末尾
    SetControlValue(frm, CHKID_DISPLAY_BOTTOMMES, &((NNshGlobal->NNsiParam)->dispBottom));

/**
    // NG確認(NG1, 2)
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD1AND2) == NNSH_USE_NGWORD1AND2)
    {
        item = 1;
    }
    else
    {
        item = 0;
    }
    SetControlValue(frm, CHKID_HIDE_MESSAGE, &item);
**/

    // NG確認(NG3)
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
    {
        item = 1;
    }
    else
    {
        item = 0;
    }
    SetControlValue(frm, CHKID_HIDE3_MESSAGE, &item);

    // NGワード設定をスレ毎に設定する設定
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD_EACHTHREAD) == NNSH_USE_NGWORD_EACHTHREAD)
    {
        item = 1;
    }
    else
    {
        item = 0;
    }
    SetControlValue(frm, CHKID_USE_NG_EACHTHREAD, &item);


#if 0
    // NGワード設定で正規表現を使用する
    SetControlValue(frm, CHKID_USE_REGEXPRESS,
                                        &((NNshGlobal->NNsiParam)->useRegularExpression));
#endif

#ifdef USE_COLOR
    // 一覧表示時にカラー(OS3.5以上で設定項目を有効にする)
    if (NNshGlobal->palmOSVersion >= 0x03503000)
    {
        // カラーの使用設定
        // SetControlValue(frm, CHKID_USE_COLORMODE, &((NNshGlobal->NNsiParam)->useColor));

        // 本当は、セレクタトリガのキャプションに色をつけたいが...
        // （よくわからんので省略）
    }
    else
#endif
    {
        (NNshGlobal->NNsiParam)->useColor = 0;
    }

    // リスト文字列のクリア
    clearFeatureList();
      
    // 参照画面機能リストの設定
    createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREVIEWMSG_STRINGS);

    // 機能一覧リストをロックする(レベルのセレクタトリガ)
    NNshWinSetPopItemsWithList(frm, POPTRID_LVLSELTRIG, LSTID_LVLSELTRIG,
                               NNshGlobal->featureList, 
                               (NNshGlobal->NNsiParam)->viewMultiBtnFeature);

    // 機能一覧リストをロックする(レベルのセレクタトリガ)
    NNshWinSetPopItemsWithList(frm, POPTRID_TTLSELTRIG, LSTID_TTLSELTRIG,
                               NNshGlobal->featureList, 
                               (NNshGlobal->NNsiParam)->viewTitleSelFeature);
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNsiSettingA                                    */
/*                                          NNsi設定(概略)のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNsiSettingA(FormType  *frm)
{
    ControlType *ctlObj;
    UInt16       item;

#ifdef USE_COLOR
    // 一覧表示時にカラー(OS3.5以上で設定項目を有効にする)
    if (NNshGlobal->palmOSVersion >= 0x03503000)
    {
        // カラーの使用設定
        SetControlValue(frm, CHKID_USE_TITLE_COLOR, &((NNshGlobal->NNsiParam)->useColor));
    }
    else
#endif
    {
        // カラーの設定は表示しない
        (NNshGlobal->NNsiParam)->useColor = 0;
        ctlObj = FrmGetObjectPtr(frm,
                                FrmGetObjectIndex(frm, CHKID_USE_TITLE_COLOR));
        CtlSetUsable(ctlObj, false);
    }

#ifdef USE_XML_OUTPUT
#else
    // XML形式を出力しない場合は、NNsi設定の Import/Export指定は表示させない。
    FrmHideObject(frm, FrmGetObjectIndex(frm, LBL_NNSISET_XML));

    ctlObj = FrmGetObjectPtr(frm,
                             FrmGetObjectIndex(frm, SELTRID_NNSISET_IMPORT));
    CtlSetUsable(ctlObj, false);

    ctlObj = FrmGetObjectPtr(frm,
                             FrmGetObjectIndex(frm, SELTRID_NNSISET_EXPORT));
    CtlSetUsable(ctlObj, false);
#endif

    // DEBUG
    SetControlValue(frm, CHKID_DEBUG_DIALOG,&((NNshGlobal->NNsiParam)->debugMessageON));

    // スレ一覧(subject.txt)全取得
    SetControlValue(frm, CHKID_SUBJECT_ALL, &((NNshGlobal->NNsiParam)->getAllThread));

    // VFS 利用可能時に自動的にVFS ON
    item = (UInt16) (NNshGlobal->NNsiParam)->vfsOnAutomatic;
    SetControlValue(frm, CHKID_VFSON_AUTOMATIC, &item);

    // 参照時レスをゾーンで表示
    SetControlValue(frm,CHKID_BLOCK_DISP_MODE, &((NNshGlobal->NNsiParam)->blockDispMode));

    // 回線未接続時は取得予約
    SetControlValue(frm,CHKID_GETRESERVEFEATURE,&(NNshGlobal->NNsiParam)->getReserveFeature);

    // まちBBS/したらば@JBBSも新着確認
    item = (NNshGlobal->NNsiParam)->enableNewArrivalHtml;
    SetControlValue(frm, CHKID_HTMLBBS_ENABLE, &(item));

    // 確認メッセージを省略
    SetControlValue(frm, CHKID_OMIT_DIALOG, &((NNshGlobal->NNsiParam)->confirmationDisable));

    // 使用VFSの指定
    item = (UInt16) (NNshGlobal->NNsiParam)->vfsUseCompactFlash;
    NNshWinSetPopItems(frm, POPTRID_VFS_LOCATION, LSTID_VFS_LOCATION, item);

#ifdef USE_USERTAB_DETAIL
         // 何もしない...
#else
    if ((NNshGlobal->NNsiParam)->debugMessageON == 0)
    {
        // NNsiの詳細設定関連のセレクタトリガを表示しない
        FrmHideObject(frm,  FrmGetObjectIndex(frm, LBL_NNSISET_DETAIL));

        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_NNSI));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_THREAD));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_BUTTON));

        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_MESSAGE));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_WRITE));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_JOG));

        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_DEVICE));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_LINE));
        FrmHideObject(frm,  FrmGetObjectIndex(frm, SELTRID_NNSISET_COMM));
    }
#endif  // #ifdef USE_USERTAB_DETAIL

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   closeForm_FavorTabSet                                    */
/*                                    お気に入り条件設定のフォームを閉じる */
/*-------------------------------------------------------------------------*/
static void closeForm_FavorTabSet(UInt16 nextForm)
{
    if (NNshGlobal->listHandle != 0)
    {
        MemHandleUnlock(NNshGlobal->listHandle);
        MemHandleFree  (NNshGlobal->listHandle);
        NNshGlobal->listHandle = 0;
    }
    FrmGotoForm(nextForm);
    return;
}

/*=========================================================================*/
/*   Function :   OpenForm_FavorTabSet                                     */
/*                                      お気に入り条件設定のフォームを開く */
/*=========================================================================*/
Err OpenForm_FavorTabSet(FormType *frm)
{
    Err            ret;
    Char          *ptr;
    UInt16         status, lp, bbsCond, thLevel, thrCond, allCond;
    NNshCustomTab *customTab;

    switch (NNshGlobal->jumpSelection)
    {
      case MULTIBTN_FEATURE_USER1TAB:
        lp        = PBTNID_FAVORSET_TAB1;
        customTab = &((NNshGlobal->NNsiParam)->custom1);
        break;

      case MULTIBTN_FEATURE_USER2TAB:
        lp        = PBTNID_FAVORSET_TAB2;
        customTab = &((NNshGlobal->NNsiParam)->custom2);
        break;

      case MULTIBTN_FEATURE_USER3TAB:
        lp        = PBTNID_FAVORSET_TAB3;
        customTab = &((NNshGlobal->NNsiParam)->custom3);
        break;

      case MULTIBTN_FEATURE_USER4TAB:
        lp        = PBTNID_FAVORSET_TAB4;
        customTab = &((NNshGlobal->NNsiParam)->custom4);
        break;

      case MULTIBTN_FEATURE_USER5TAB:
      default:
        lp        = PBTNID_FAVORSET_TAB5;
        customTab = &((NNshGlobal->NNsiParam)->custom5);
        break;
    }

    // ユーザタブ(プッシュボタン)設定
    status = 1;
    SetControlValue(frm, lp, &status);

    // データの抽出
    thLevel = customTab->threadLevel & NNSH_THREADLEVEL_MASK;
    thrCond = 
      (customTab->threadLevel & NNSH_THREADCOND_MASK) >> NNSH_THREADCOND_SHIFT;
    allCond = customTab->condition & NNSH_CONDITION_ALL;
    bbsCond = customTab->boardCondition;

    // 参照ログは除外
    status  = customTab->condition & NNSH_CONDITION_NOTREADONLY;
    if (status != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_NOT_READONLY, &status);
    }

    // ユーザタブ名をフィールドに反映させる
    NNshWinSetFieldText(frm, FLDID_FAVORSET_TABTITLE, false,
                        customTab->tabName, StrLen(customTab->tabName));

    // BBS一覧の作成
    ret = Create_BBS_INDEX(&(NNshGlobal->bbsTitles), &lp);
    if (ret == errNone)
    {
        // BBS一覧のリスト状態を反映する。(新着確認実施タブ)
        NNshWinSetListItems(frm, LSTID_FAVORSET_CHECKNEW,
                            NNshGlobal->bbsTitles, lp,
                            customTab->newArrival, 
                            &(NNshGlobal->listHandle),
                            &ptr);

        NNshWinSetPopItems(frm, POPTRID_FAVORSET_CHECKNEW,
                           LSTID_FAVORSET_CHECKNEW, customTab->newArrival);

        // BBS一覧のリスト状態を反映する。(一覧取得タブ)
        NNshWinUpdateListItems(frm, LSTID_FAVORSET_GETLIST, ptr, lp,
                               customTab->getList);

        NNshWinSetPopItems(frm, POPTRID_FAVORSET_GETLIST,
                           LSTID_FAVORSET_GETLIST, customTab->getList);


        // BBS一覧のリスト状態を反映する。(板設定タブ)
        NNshWinUpdateListItems(frm, LSTID_FAVORSET_BBSNICK, ptr, lp,
                               customTab->boardNick);

        NNshWinSetPopItems(frm, POPTRID_FAVORSET_BBSNICK,
                           LSTID_FAVORSET_BBSNICK, customTab->boardNick);
    }

    // 板設定の初期値を反映
    if (bbsCond != 0)
    {
        // 板設定のチェックを戻す
        SetControlValue(frm, CHKID_FAVORSET_BBSNICK, &(bbsCond));
        bbsCond--;
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_BBSNICK_SET,
                           LSTID_FAVORSET_BBSNICK_SET, bbsCond);
    }

    status = 1;
    if (thrCond != 0)
    {
        // スレレベルのチェックボックス反映
        SetControlValue(frm, CHKID_FAVORSET_LEVEL, &(status));

        // スレレベルの条件反映
        thrCond--;
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_LVL_SET, 
                           LSTID_FAVORSET_LVL_SET, thrCond);
    }

    // スレレベルの反映
    switch (thLevel)
    {
      case NNSH_MSGATTR_FAVOR_L1:
        SetControlValue(frm, PBTNID_FAVORSET_LVL1, &(status));
        break;

      case NNSH_MSGATTR_FAVOR_L2:
        SetControlValue(frm, PBTNID_FAVORSET_LVL2, &(status));
        break;

      case NNSH_MSGATTR_FAVOR_L3:
        SetControlValue(frm, PBTNID_FAVORSET_LVL3, &(status));
        break;

      case NNSH_MSGATTR_FAVOR_L4:
        SetControlValue(frm, PBTNID_FAVORSET_LVL4, &(status));
        break;

      case NNSH_MSGATTR_FAVOR:
        SetControlValue(frm, PBTNID_FAVORSET_LVL5, &(status));
        break;

      case NNSH_MSGATTR_NOTFAVOR:
      default:
        // 指定なし
        SetControlValue(frm, PBTNID_FAVORSET_LVL0, &(status));
        break;
    }

    // 条件のAND/OR設定
    NNshWinSetPopItems(frm, POPTRID_FAVORSET_CONDITIONSET,
                       LSTID_FAVORSET_CONDITIONSET, allCond);

    // ログ位置の指定
    status = (customTab->condition & NNSH_CONDITION_LOGLOC_ALL);
    if (status != 0)
    {

        if (status == NNSH_CONDITION_LOGLOC_PALM)
        {
            // Palm内
            allCond = 0;
        }
        else
        {
            // VFS内
            allCond = 1;
        }

        // ログ位置のチェックボックスを反映
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_LOGLOC, &(status));

        // リストを更新
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_LOGLOC_SET,
                           LSTID_FAVORSET_LOGLOC_SET, allCond);
    }

    // 取得エラーの設定
    status = (customTab->condition & NNSH_CONDITION_GETERR_ALL);
    if (status != 0)
    {

        if (status == NNSH_CONDITION_GETERR_NONE)
        {
            // エラーなし
            allCond = 0;
        }
        else
        {
            // エラー発生
            allCond = 1;
        }

        // ログ位置のチェックボックスを反映
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_GETERROR, &(status));

        // リストを更新
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_GETERROR_SET,
                           LSTID_FAVORSET_GETERROR_SET, allCond);
    }

    // 取得保留中の設定
    status = (customTab->condition & NNSH_CONDITION_GETRSV_ALL);
    if (status != 0)
    {

        if (status == NNSH_CONDITION_GETRSV_RSV)
        {
            // 取得保留中スレ
            allCond = 0;
        }
        else
        {
            // 非取得保留中スレ
            allCond = 1;
        }

        // 取得保留のチェックボックスを反映
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_GETRESERVE, &(status));

        // リストを更新
        NNshWinSetPopItems(frm, POPTRID_FAVORSET_GETRESERVE_SET,
                           LSTID_FAVORSET_GETRESERVE_SET, allCond);
    }

    // スレ状態のチェックボックス設定
    if ((customTab->threadStatus != 0)||(customTab->threadCreate))
    {
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_GETSTATUS, &(status));
    }
    // スレ状態のチェックボックス設定
    if ((customTab->stringSet & NNSH_STRING_SELECTION) != 0)
    {
        status = 1;
        SetControlValue(frm, CHKID_FAVORSET_TITLESET, &(status));
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   Hander_FavorTabSet                                       */
/*                                     お気に入り条件設定イベントハンドラ  */
/*=========================================================================*/
Boolean Handler_FavorTabSet(EventType *event)
{
    UInt16         btnId, status, keyId;
    MemHandle      txtH;
    Char          *ptr, numBuf[TINYBUF];
    FormType      *frm, *diagFrm;
    NNshCustomTab *custom;
    ListType      *lstP;
    FieldType     *fldP;

    // ボタンチェック以外のイベントは即戻り
    keyId = 0;
    switch (event->eType)
    { 
      case ctlSelectEvent:
        // ボタンが押された(次へ進む)
        keyId = event->data.ctlSelect.controlID;
        break;

      case keyDownEvent:
        // キー入力された(次へ進む)
        switch (KeyConvertFiveWayToJogChara(event))
        {
          // ハードキー下を押した時の処理
          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrTapWaveSpecificKeyRDown:
          case vchrThumbWheelDown:
          case vchrRockerDown:
          case chrRightArrow:
          case vchrRockerRight:
          case vchrJogRight:
          case vchrTapWaveSpecificKeyRRight:
            keyId = chrRightArrow;
            break;

          // ハードキー上を押した時の処理
          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrTapWaveSpecificKeyRUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
          case chrLeftArrow:
          case vchrRockerLeft:
          case vchrJogLeft:
          case vchrTapWaveSpecificKeyRLeft:
            keyId = chrLeftArrow;
            break;

          // バックボタン/ESCキー/BSキーを押したときの処理
          // case chrBackspace:
          case vchrJogBack:
          case chrEscape:
          case vchrThumbWheelBack:
          case vchrGarminEscape:
            keyId = BTNID_FAVORSET_CANCEL;
            break;

          // HandEra JOG Push選択/Enterキー入力時の処理(リスト選択)
          case vchrJogRelease:
          case chrCarriageReturn:
          case chrLineFeed:
          case vchrRockerCenter:
          case vchrHardRockerCenter:
          case vchrThumbWheelPush:
            keyId = BTNID_FAVORSET_OK;
            break;

#ifdef USE_PIN_DIA
          case winDisplayChangedEvent:
            // ディスプレイサイズ変更イベント
            if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
                (NNshGlobal->device == NNSH_DEVICE_GARMIN))
            {
                frm = FrmGetActiveForm();
                if (HandEraResizeForm(frm) == true)
                {
                    FrmDrawForm(frm);        
                }
            }
            return (false);
            break;
#endif

          default: 
            return (false);
            break;
        }
        break;

      case menuEvent:
        // メニューを開かれた
        if (event->data.menu.itemID == MNUID_SHOW_HELP)
        {
            // 操作ヘルプの表示
            // (クリップボードに、nnDAに渡すデータについての指示を格納する)
            ClipboardAddItem(clipboardText, 
                             nnDA_NNSIEXT_VIEWSTART
                             nnDA_NNSIEXT_INFONAME
                             nnDA_NNSIEXT_HELPFAVORTAB
                             nnDA_NNSIEXT_ENDINFONAME
                             nnDA_NNSIEXT_ENDVIEW,
                             sizeof(nnDA_NNSIEXT_VIEWSTART
                                     nnDA_NNSIEXT_INFONAME
                                     nnDA_NNSIEXT_HELPFAVORTAB
                                     nnDA_NNSIEXT_ENDINFONAME
                                     nnDA_NNSIEXT_ENDVIEW));
            // nnDAを起動する
            (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        }
        return (false);
        break;

      default: 
        return (false);
        break;
    }

    // 現在開いている画面の設定項目を確認する
    switch (NNshGlobal->jumpSelection)
    {
      case MULTIBTN_FEATURE_USER1TAB:
        custom = &((NNshGlobal->NNsiParam)->custom1);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB5;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB2;
        }
        break;

      case MULTIBTN_FEATURE_USER2TAB:
        custom = &((NNshGlobal->NNsiParam)->custom2);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB1;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB3;
        }
        break;

      case MULTIBTN_FEATURE_USER3TAB:
        custom = &((NNshGlobal->NNsiParam)->custom3);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB2;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB4;
        }
        break;

      case MULTIBTN_FEATURE_USER4TAB:
        custom = &((NNshGlobal->NNsiParam)->custom4);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB3;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB5;
        }
        break;

      case MULTIBTN_FEATURE_USER5TAB:
      default:
        custom = &((NNshGlobal->NNsiParam)->custom5);
        if (keyId == chrLeftArrow)
        {
            keyId = PBTNID_FAVORSET_TAB4;
        }
        else if (keyId == chrRightArrow)
        {
            keyId = PBTNID_FAVORSET_TAB1;
        }
        break;
    }

    frm = FrmGetActiveForm();
    switch (keyId)
    {
      // 取得状態の設定
      case SELTRID_FAVORSET_GETSTATUS:
        diagFrm  = FrmInitForm(FRMID_FAVORSET_THREADSTATE);
        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_NEW) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_NEWGET,     &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_UPDATE) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_PARTGET,    &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_REMAIN) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_NOTREAD,    &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_ALREADY) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_ALREADY,    &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_OVER) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_UPPERLIMIT, &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_NOT_YET) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_NOTYET,     &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_GETERROR) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_ERROR,      &status);

        status = ((custom->threadStatus >> NNSH_SUBJSTATUS_UNKNOWN) & 1);
        SetControlValue(diagFrm, CHKID_FAVORSET_ST_ETC,        &status);

        // スレ立て日 n 日以降のデータ反映
        status = custom->threadCreate;
        SetControlValue(diagFrm, CHKID_THREAD_DATE, &status);
        if (status != 0)
        {
            MemSet (numBuf, TINYBUF, 0x00);
            NUMCATI(numBuf, status);
            NNshWinSetFieldText(diagFrm, 
                                FLDID_THREAD_DATE, false, numBuf, TINYBUF);
        }

        FrmSetActiveForm(diagFrm);
        btnId = FrmDoDialog(diagFrm);
        if (btnId == BTNID_FAVORSET_ST_OK)
        {
            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_NEWGET,     &status);
            custom->threadStatus = (status << NNSH_SUBJSTATUS_NEW);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_PARTGET,    &status);
            custom->threadStatus = 
                       custom->threadStatus|(status << NNSH_SUBJSTATUS_UPDATE);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_NOTREAD,    &status);
            custom->threadStatus =
                       custom->threadStatus|(status << NNSH_SUBJSTATUS_REMAIN);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_ALREADY,    &status);
            custom->threadStatus = 
                      custom->threadStatus|(status << NNSH_SUBJSTATUS_ALREADY);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_UPPERLIMIT, &status);
            custom->threadStatus = 
                         custom->threadStatus|(status << NNSH_SUBJSTATUS_OVER);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_NOTYET,     &status);
            custom->threadStatus = 
                      custom->threadStatus|(status << NNSH_SUBJSTATUS_NOT_YET);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_ERROR,      &status);
            custom->threadStatus =
                     custom->threadStatus|(status << NNSH_SUBJSTATUS_GETERROR);

            UpdateParameter(diagFrm, CHKID_FAVORSET_ST_ETC,        &status);
            custom->threadStatus =
                      custom->threadStatus|(status << NNSH_SUBJSTATUS_UNKNOWN);

            // スレ立て日の取得
            custom->threadCreate = 0;
            fldP    = FrmGetObjectPtr(diagFrm, 
                                FrmGetObjectIndex(diagFrm, FLDID_THREAD_DATE));
            txtH = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                ptr    = MemHandleLock(txtH);
                if (StrLen(ptr) != 0)
                {
                    custom->threadCreate = StrAToI(ptr);
                }
                MemHandleUnlock(txtH);
            }
        }
        FrmSetActiveForm(frm);
        FrmDeleteForm(diagFrm);

        // 状態設定を更新
        status = ((custom->threadStatus != 0)||(custom->threadCreate)) ? 1 : 0;
        SetControlValue(frm, CHKID_FAVORSET_GETSTATUS, &(status));
        break;

      // タイトル文字列の詳細設定
      case SELTRID_FAVORSET_TITLESET:
        diagFrm  = FrmInitForm(FRMID_FAVORSET_THREADTITLE);
        FrmSetActiveForm(diagFrm);
        if (custom->string1[0] != '\0')
        {
            NNshWinSetFieldText(diagFrm, FLDID_FAVORSET_TTL_STR1, false,
                                custom->string1, sizeof(custom->string1));
        }
        if (custom->string2[0] != '\0')
        {
            NNshWinSetFieldText(diagFrm, FLDID_FAVORSET_TTL_STR2, false,
                                custom->string2, sizeof(custom->string2));
        }
        if (custom->string3[0] != '\0')
        {
            NNshWinSetFieldText(diagFrm, FLDID_FAVORSET_TTL_STR3, false, 
                                custom->string3, sizeof(custom->string2));
        }

        // 文字列の条件設定
        status = custom->stringSet & NNSH_STRING_SETMASK;
        NNshWinSetPopItems(diagFrm, 
                           POPTRID_FAVORSET_TTL_CONDSET,
                           LSTID_FAVORSET_TTL_CONDSET, status);

        btnId    = FrmDoDialog(diagFrm);
        if (btnId == BTNID_FAVORSET_TTL_OK)
        {
            // 文字列１の取得
            fldP    = FrmGetObjectPtr(diagFrm, 
                          FrmGetObjectIndex(diagFrm, FLDID_FAVORSET_TTL_STR1));
            txtH    = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                ptr = MemHandleLock(txtH);
                MemSet(custom->string1, MINIBUF, 0x00);
                StrNCopy(custom->string1, ptr, (MINIBUF - 1));
                MemHandleUnlock(txtH);
            }

            // 文字列２の取得
            fldP    = FrmGetObjectPtr(diagFrm,
                          FrmGetObjectIndex(diagFrm, FLDID_FAVORSET_TTL_STR2));
            txtH    = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                ptr = MemHandleLock(txtH);
                MemSet(custom->string2, MINIBUF, 0x00);
                StrNCopy(custom->string2, ptr, (MINIBUF - 1));
                MemHandleUnlock(txtH);
            }

            // 文字列３の取得
            fldP    =  FrmGetObjectPtr(diagFrm,
                          FrmGetObjectIndex(diagFrm, FLDID_FAVORSET_TTL_STR3));
            txtH    = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                ptr = MemHandleLock(txtH);
                MemSet(custom->string3, MINIBUF, 0x00);
                StrNCopy(custom->string3, ptr, (MINIBUF - 1));
                MemHandleUnlock(txtH);
            }
            lstP  = FrmGetObjectPtr(diagFrm,
                       FrmGetObjectIndex(diagFrm, LSTID_FAVORSET_TTL_CONDSET));
            custom->stringSet = custom->stringSet & NNSH_STRING_SELECTION;
            custom->stringSet = custom->stringSet | LstGetSelection(lstP);
        }
        FrmSetActiveForm(frm);
        FrmDeleteForm(diagFrm);
        break;

      // "Cancel"ボタンがおされたとき
      case BTNID_FAVORSET_CANCEL:
        // 値を破棄した後、一覧画面に画面遷移する
        FrmEraseForm(frm);
        closeForm_FavorTabSet(NNshGlobal->backFormId);
        break;

      // "OK"ボタンがおされたとき
      case BTNID_FAVORSET_OK:
        // 値を反映後、一覧画面に画面遷移する
        effect_FavorTabSet(frm, custom);
        FrmEraseForm(frm);
        closeForm_FavorTabSet(NNshGlobal->backFormId);
        break;

      case PBTNID_FAVORSET_TAB1:
        // 画面上の値を保存後、画面遷移
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER1TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      case PBTNID_FAVORSET_TAB2:
        // 画面上の値を保存後、画面遷移
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER2TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      case PBTNID_FAVORSET_TAB3:
        // 画面上の値を保存後、画面遷移
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER3TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      case PBTNID_FAVORSET_TAB4:
        // 画面上の値を保存後、画面遷移
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER4TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      case PBTNID_FAVORSET_TAB5:
        // 画面上の値を保存後、画面遷移
        effect_FavorTabSet(frm, custom);
        NNshGlobal->jumpSelection = MULTIBTN_FEATURE_USER5TAB;
        FrmEraseForm(frm);
        closeForm_FavorTabSet(FRMID_FAVORSET_DETAIL);
        break;

      default:
        // 上記以外（何もしない）
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   effect_FavorTabSet                                       */
/*                         お気に入り条件設定をパラメータ値に反映する処理  */
/*-------------------------------------------------------------------------*/
static Err effect_FavorTabSet(FormType *frm, NNshCustomTab *custom)
{
    UInt16     status;
    ListType  *lstP;
    FieldType *fldP;
    MemHandle  txtH;
    Char      *txtP;

    // タブタイトルの取得
    fldP    = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm, FLDID_FAVORSET_TABTITLE));
    txtH    = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        MemSet(custom->tabName, MAX_BOARDNAME, 0x00);
        StrNCopy(custom->tabName, txtP, (MAX_BOARDNAME - 1));
        MemHandleUnlock(txtH);

        // BBSタイトルインデックス用の領域を解放
        if (NNshGlobal->boardIdxH != 0)
        {
            MemHandleFree(NNshGlobal->boardIdxH);
            NNshGlobal->boardIdxH = 0;
        }
        MEMFREE_PTR((NNshGlobal->bbsTitles));
    }

    // 新着確認実施タブ
    lstP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, LSTID_FAVORSET_CHECKNEW));
    custom->newArrival = LstGetSelection(lstP);

    // 一覧取得実施タブ
    lstP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, LSTID_FAVORSET_GETLIST));
    custom->getList = LstGetSelection(lstP);

    /////////////////////////////////////////////////////////////////////////

    // 条件チェック
    lstP   = FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, LSTID_FAVORSET_CONDITIONSET));
    status = LstGetSelection(lstP);
    if (status != 0)
    {
        // どれか一致
        custom->condition = NNSH_CONDITION_OR;
    }
    else
    {
        // 全て一致
        custom->condition = NNSH_CONDITION_AND;
    }

    // 参照ログは除外
    UpdateParameter(frm, CHKID_FAVORSET_NOT_READONLY, &status);
    if (status != 0)
    {
        custom->condition = custom->condition | NNSH_CONDITION_NOTREADONLY;
    }

    // スレレベルのチェック
    UpdateParameter(frm, CHKID_FAVORSET_LEVEL, &(status));
    if (status != 0)
    {
        // スレレベルを反映させる
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm, LSTID_FAVORSET_LVL_SET));
        custom->threadLevel =
                          (LstGetSelection(lstP) + 1) << NNSH_THREADCOND_SHIFT;
        UpdateParameter(frm, PBTNID_FAVORSET_LVL1, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR_L1;
            goto SET_CONDITION;
        }
        UpdateParameter(frm, PBTNID_FAVORSET_LVL2, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR_L2;
            goto SET_CONDITION;
        }
        UpdateParameter(frm, PBTNID_FAVORSET_LVL3, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR_L3;
            goto SET_CONDITION;
        }
        UpdateParameter(frm, PBTNID_FAVORSET_LVL4, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR_L4;
            goto SET_CONDITION;
        }
        UpdateParameter(frm, PBTNID_FAVORSET_LVL5, &status);
        if (status != 0)
        {
            status = NNSH_MSGATTR_FAVOR;
            goto SET_CONDITION;
        }
#if 0
        UpdateParameter(frm, PBTNID_FAVORSET_LVL0, &status);
        if (status != 0)
        {
            status = 0;
            goto SET_CONDITION;
        }
        status = 0;
#endif

SET_CONDITION:
        custom->threadLevel = custom->threadLevel|(status);
    }
    else
    {
        custom->threadLevel = 0;
    }
     
    // 板設定のチェック
    UpdateParameter(frm, CHKID_FAVORSET_BBSNICK, &(status));
    if (status != 0)
    {
        // 板設定を反映させる
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm, LSTID_FAVORSET_BBSNICK));
        custom->boardNick = LstGetSelection(lstP);
        if (custom->boardNick < NNSH_NOF_SPECIAL_BBS)
        {
            // 板設定は、通常の板が指定されていること
            NNsh_WarningMessage(ALTID_WARN, MSG_WARN_NOTEFFECT_BBS, "", 0);

            // 板設定をクリア
            custom->boardCondition = 0;
            goto FUNC_END;
        }
        lstP = FrmGetObjectPtr(frm,
                           FrmGetObjectIndex(frm, LSTID_FAVORSET_BBSNICK_SET));
        custom->boardCondition = LstGetSelection(lstP) + 1;
    }
    else
    {
        custom->boardCondition = 0;
    }

    // ログ位置の指定
    UpdateParameter(frm, CHKID_FAVORSET_LOGLOC, &(status));
    if (status != 0)
    {
        // ログ位置データを反映させる
        lstP = FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, LSTID_FAVORSET_LOGLOC_SET));
        status = LstGetSelection(lstP);
        if (status == 0)
        {
            custom->condition = custom->condition | NNSH_CONDITION_LOGLOC_PALM;
        }
        else
        {
            custom->condition = custom->condition | NNSH_CONDITION_LOGLOC_VFS;
        }
    }

    // 取得エラーの指定
    UpdateParameter(frm, CHKID_FAVORSET_GETERROR, &(status));
    if (status != 0)
    {
        // 取得エラーデータを反映させる
        lstP = FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, LSTID_FAVORSET_GETERROR_SET));
        status = LstGetSelection(lstP);
        if (status == 0)
        {
            custom->condition = custom->condition|NNSH_CONDITION_GETERR_NONE;
        }
        else
        {
            custom->condition = custom->condition|NNSH_CONDITION_GETERR_ERROR;
        }
    }

    // 取得保留の指定
    UpdateParameter(frm, CHKID_FAVORSET_GETRESERVE, &(status));
    if (status != 0)
    {
        // 取得保留データを反映させる
        lstP = FrmGetObjectPtr(frm,
                        FrmGetObjectIndex(frm, LSTID_FAVORSET_GETRESERVE_SET));
        status = LstGetSelection(lstP);
        if (status == 0)
        {
            custom->condition = custom->condition | NNSH_CONDITION_GETRSV_RSV;
        }
        else
        {
            custom->condition = custom->condition | NNSH_CONDITION_GETRSV_NONE;
        }
    }

    // スレ状態の確認 
    UpdateParameter(frm, CHKID_FAVORSET_GETSTATUS, &(status));
    if (status == 0)
    {
        // スレ状態をクリアする
        custom->threadStatus = 0;    
    }

    // 文字列の設定
    UpdateParameter(frm, CHKID_FAVORSET_TITLESET, &(status));
    if (status != 0)
    {
        custom->stringSet = custom->stringSet | NNSH_STRING_SELECTION;
    }
    else
    {
        custom->stringSet = custom->stringSet & NNSH_STRING_SETMASK;
    }

FUNC_END:
    return (errNone);
}

#ifdef USE_COLOR
/*-------------------------------------------------------------------------*/
/*   Function :   Handler_setViewColorDetail                               */
/*                                      参照画面の色設定用イベントハンドラ */
/*-------------------------------------------------------------------------*/
Boolean Handler_setViewColorDetail(EventType *event)
{
    UInt8            *paramPtr;
    IndexedColorType  color;

    if (event->eType != ctlSelectEvent)
    {
        // 何もしない
        return (false);
    }

    switch (event->data.ctlSelect.controlID)
    {
      case SELTRID_NNSISET_COLOR_LETTER:
        // 文字の色
        paramPtr = &((NNshGlobal->NNsiParam)->colorViewFG);
        break;

      case SELTRID_NNSISET_COLOR_HEADER:
        // ヘッダの色
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgHeader);
        break;

      case SELTRID_NNSISET_COLOR_BOLDHEADER:
        // ヘッダ（強調）の色
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgHeaderBold);
        break;

      case SELTRID_NNSISET_COLOR_BACKGROUND:
        // 背景の色
        paramPtr = &((NNshGlobal->NNsiParam)->colorViewBG);
        break;

      case SELTRID_NNSISET_COLOR_FOOTER:
        // フッタの色
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgFooter);
        break;

      case SELTRID_NNSISET_COLOR_BOLDFOOTER:
        //　フッタ(強調)の色
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgFooterBold);
        break;

      case SELTRID_NNSISET_COLOR_MARKING:
        // マークの色
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgInform);
        break;

      case SELTRID_NNSISET_COLOR_BOLDMARKING:
        // マーク（強調）の色
        paramPtr = &((NNshGlobal->NNsiParam)->colorMsgInformBold);
        break;
        
      default:
        // 何もしない
        return (false);
        break;
    }
    // 色を選択する
    color = *paramPtr;
    if (UIPickColor(&color, NULL, UIPickColorStartPalette, NULL, NULL)
                                                                      == true)
    {
        // 設定した色を反映させる
        *paramPtr = color;
    }
    FrmDrawForm(FrmGetActiveForm());
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   selectOmitDialog                                         */
/*                                          確認ダイアログの省略(詳細設定) */
/*-------------------------------------------------------------------------*/
static Boolean setViewColorDetail(void)
{
    Boolean       ret = false;
    FormType     *prevFrm, *diagFrm;
    UInt16        btnId;
    FieldType    *fldP;
    MemHandle     txtH;
    Char         *nameP, separator[SEPARATOR_LEN];

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // ダイアログ表示準備
    PrepareDialog_NNsh(FRMID_NNSISET_VIEWCOLOR, &diagFrm, true);
    if (diagFrm == NULL)
    {
        return (false);
    }

    // 初期データの設定
    // ヘッダ/フッタの色分け表示を実施
    btnId = (NNshGlobal->NNsiParam)->useColorMessageToken;
    SetControlValue(diagFrm, CHKID_NNSISET_USEVIEWCOLOR, &btnId);

    // nnDA使用時、レスをpop up 表示する
    btnId = (NNshGlobal->NNsiParam)->showResPopup;
    SetControlValue(diagFrm, CHKID_NNSISET_USE_POPUP, &btnId);

    // 現在のメッセージ区切り文字列を取得
    MemSet (separator, sizeof(separator), 0x00);
    StrCopy(separator, &((NNshGlobal->NNsiParam)->messageSeparator[1]));
    
    // 末尾の文字を消す(改行コードが入っているから)
    nameP = separator;
    while ((*nameP != '\n')&&(*nameP != '\0'))
    {
        nameP++;
    }
    *nameP = '\0';

    // メッセージの区切り文字列の現在設定値をウィンドウに反映させる
    NNshWinSetFieldText(diagFrm, FLDID_ZONE_SEPARATOR, true, separator, sizeof(separator));
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_ZONE_SEPARATOR));

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_setViewColorDetail);

    // ウィンドウの表示
    btnId = FrmDoDialog(diagFrm);

    // データのヘッダ/フッタの色分け表示
    UpdateParameter(diagFrm, CHKID_NNSISET_USEVIEWCOLOR, &btnId);
    (NNshGlobal->NNsiParam)->useColorMessageToken = btnId;

    // データのヘッダ/フッタの強調表示
    UpdateParameter(diagFrm, CHKID_NNSISET_USE_POPUP, &btnId);
    (NNshGlobal->NNsiParam)->showResPopup = btnId;

    // 区切り文字列の取得
    fldP = FrmGetObjectPtr(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_ZONE_SEPARATOR));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        nameP = MemHandleLock(txtH);
        if (*nameP != '\0')
        {
            (NNshGlobal->NNsiParam)->messageSeparator[0] = '\n';
            StrNCopy(&((NNshGlobal->NNsiParam)->messageSeparator[1]),
                     nameP, (SEPARATOR_LEN - MARGIN));
            StrCat((NNshGlobal->NNsiParam)->messageSeparator, "\n");
        }
        MemHandleUnlock(txtH);        
    }

    // ダイアログ表示の後片付け
    PrologueDialog_NNsh(prevFrm, diagFrm, true);

    return (ret);
}
#endif  // #ifdef USE_COLOR

#ifdef USE_LOGCHARGE
/*=========================================================================*/
/*   Function :  GetLog_SetHardKeyFeature                                  */
/*                                    参照ログ一覧画面のハードキー機能設定 */
/*=========================================================================*/
void GetLog_SetHardKeyFeature(void)
{
    // リスト文字列のクリア
    clearFeatureList();
      
    // 参照ログ一覧画面機能リストの設定
    createFeatureListStrings(&(NNshGlobal->featureList), MULTIBTN_FEATUREGETLOG_STRINGS);

    // ハードキー詳細設定
    (void) SetTitleHardKey(NNSH_TITLE_GETLOGLIST_TITLE, GETLOGBTN_FEATURE, 
                            GETLOGBTN_FEATURE_MASK, &((NNshGlobal->NNsiParam)->getLogFeature));

    //  一部ハードキーの制御は、NNsi総合設定のハードキー制御設定が必要な
    // ことを警告する。
    NNsh_WarningMessage(ALTID_WARN, MSG_WARN_SETTING_HARDKEY, "", 0);

    // リスト文字列のクリア
    clearFeatureList();

    return;
}
#endif // #ifdef USE_LOGCHARGE
