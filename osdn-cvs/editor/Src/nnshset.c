/*============================================================================*
 *  FILE: 
 *     nnshset.c
 *
 *  Description: 
 *     NNsh parameter setting functions for NNsh.
 *
 *===========================================================================*/
#define NNSHSET_C
#include "local.h"

extern void NNshWinSetPopItems(FormType *frm, UInt16 popId, UInt16 lstId,
                                                                  UInt16 item);

/*=========================================================================*/
/*   Function :  StrAToI_NNsh                                              */
/*                                                  文字列を数値に変換する */
/*=========================================================================*/
static UInt32 StrAToI_NNsh(Char *num)
{
    UInt32 size = 0;
    while ((*num >= '0')&&(*num <= '9'))
    {
        size = size * 10 + (*num - '0');
        num++;
    }

    return (size);
}

/*=========================================================================*/
/*   Function :  StrAToI_NNsh                                              */
/*                                                  数値を文字列に変換する */
/*=========================================================================*/
void StrIToA_NNsh(Char *dst, UInt32 data)
{
    Char buf[40], *ptr;
    UInt32 temp;
    UInt16  num;
    
    MemSet(buf, sizeof(buf), 0x00);
    ptr  = buf;
    temp = data;

    // bufに一番下の桁から文字列に変換して代入する
    while (temp != 0)
    {
        num  = temp % 10;
        *ptr = '0' + num;
        temp = temp / 10;
        ptr++;
    }
    // dstにデータを格納する
    while (ptr != buf)
    {
        ptr--;
        *dst = *ptr;
        dst++;
    }
    return;
}


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
/*   Function :   effect_NNshSetting                                       */
/*                             NNsi設定画面からパラメータ値に反映する処理  */
/*-------------------------------------------------------------------------*/
static Err effect_NNshSetting(FormType *frm)
{
    FieldType   *fldP;
    UInt16       item;
    UInt32       bufSize;
    Char        *logBuf, *numP;
    MemHandle    txtH;
    NNshSavedPref *NNsiParam;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    /***  NNsh設定を(グローバル変数に)反映させる  ***/

    // フォーカスを自動設定しない
    UpdateParameter(frm, CHKID_NOTAUTOFOCUS, &(NNsiParam->notAutoFocus));

    // シルクを拡大する
    UpdateParameter(frm, CHKID_SILKMAX, &(NNsiParam->silkMax));

    // TsPatchを使用しない
    UpdateParameter(frm, CHKID_NOTUSE_TSPATCH, &(NNsiParam->notUseTsPatch));

    // CLIE smallフォントを使用する
    UpdateParameter(frm, CHKID_USE_CLIESMALL, &(NNsiParam->sonyHRFont));

    // 終了時保存ダイアログを表示
    UpdateParameter(frm, CHKID_ENDSAVE_DIALOG, &(NNsiParam->useEndSaveDialog));

    // 起動時前回の編集ファイルを復帰
    UpdateParameter(frm, CHKID_AUTORESTORE, &(NNsiParam->autoRestore));

    // 使用VFS指定
    item  = LstGetSelection(FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, LSTID_VFS_LOCATION)));
    NNsiParam->vfsUseCompactFlash = item;

    // DEBUG
    UpdateParameter(frm, CHKID_DEBUG_DIALOG,&(NNsiParam->debugMessageON));

    // 内部バッファサイズの変更
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_BUFFER_SIZE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        numP = MemHandleLock(txtH);
        bufSize = StrAToI_NNsh(numP);
        MemHandleUnlock(txtH);
        if ((NNsiParam->debugMessageON == 0)&&((bufSize < NNSH_WORKBUF_MIN)||(bufSize > NNSH_WORKBUF_MAX)))
        {
            // データ値異常、入力範囲異常を表示して元に戻る
            logBuf = MemPtrNew_NNsh(BUFSIZE);
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
                MemPtrFree(logBuf);
            }
            return (~errNone);
        }
        else
        {
            // バッファサイズを変更して終了する
            if (NNsiParam->bufferSize != bufSize)
            {
                NNsiParam->bufferSize = bufSize;
            }
        }
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "FldGetTextHandle()", "0", 0);
    }

    return (errNone);
}

/*=========================================================================*/
/*   Function :   Hander_NNshSetting                                       */
/*                                     NNsi設定系の(総合)イベントハンドラ  */
/*=========================================================================*/
Boolean Handler_NNshSetting(EventType *event)
{
    Err          ret;
    FormType    *frm;
    void        *effectFunc;

    // ボタンチェック以外のイベントは即戻り
    switch (event->eType)
    { 
      case ctlSelectEvent:
        // ボタンが押された(次へ進む)
        break;

      default: 
        return (false);
        break;
    }

    effectFunc = NULL;
    frm = FrmGetActiveForm();
    switch (event->data.ctlSelect.controlID)
    {
      // "OK"ボタンがおされたとき(NNsi設定)
      case BTNID_CONFIG_NNSH_OK:
        effectFunc = &effect_NNshSetting;
        break;

      // "Cancel"ボタンがおされたとき
      case BTNID_CONFIG_NNSH_CANCEL:
        effectFunc = NULL;
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
            return (true);
        }
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_NNshSetting                                     */
/*                                                NNsi設定のフォームを開く */
/*=========================================================================*/
Err OpenForm_NNshSetting(FormType *frm)
{
    Char      bufSize[MINIBUF];
#ifdef USE_TSPATCH
    Err       ret;
    UInt32    fontVer;
#endif
    NNshSavedPref *NNsiParam;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    /***  NNsh設定を(グローバル変数から)反映させる  ***/

    // フォーカスを自動設定しない
    SetControlValue(frm, CHKID_NOTAUTOFOCUS, &(NNsiParam->notAutoFocus));

    // シルクを拡大する
    SetControlValue(frm, CHKID_SILKMAX, &(NNsiParam->silkMax));

    // 終了時保存ダイアログを表示
    SetControlValue(frm, CHKID_ENDSAVE_DIALOG, &(NNsiParam->useEndSaveDialog));

    // 起動時前回の編集ファイルを復帰
    SetControlValue(frm, CHKID_AUTORESTORE, &(NNsiParam->autoRestore));

    // CLIE smallフォントを使用する
    SetControlValue(frm, CHKID_USE_CLIESMALL, &(NNsiParam->sonyHRFont));

    // TsPatchを使用しない
#ifdef USE_TSPATCH
    ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &fontVer);
    if (ret == errNone)
    {    
        SetControlValue(frm, CHKID_NOTUSE_TSPATCH, &(NNsiParam->notUseTsPatch));
    }
    else
#endif
    {
        // (設定項目自体を画面に表示しない)
        CtlSetUsable(FrmGetObjectPtr(frm, 
                          FrmGetObjectIndex(frm, CHKID_NOTUSE_TSPATCH)), false);
    }

    // 使用VFSの指定
    NNshWinSetPopItems(frm, POPTRID_VFS_LOCATION, LSTID_VFS_LOCATION,
                                                 NNsiParam->vfsUseCompactFlash);

    // DEBUG
    SetControlValue(frm, CHKID_DEBUG_DIALOG,&(NNsiParam->debugMessageON));

    // フィールドに現在のバッファサイズを反映させる
    MemSet(bufSize, sizeof(bufSize), 0x00);
    StrIToA_NNsh(bufSize, NNsiParam->bufferSize);
    NNshWinSetFieldText(frm, FLDID_BUFFER_SIZE, true, bufSize, StrLen(bufSize));

    return (errNone);
}
