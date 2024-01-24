/*============================================================================*
 *  FILE: 
 *     nnshmain.c
 *
 *  Description: 
 *     Main routines for NNsh. 
 *
 *===========================================================================*/
#define THRMNG_C
#include "local.h"

static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR);
static Err get_message_Info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR, 
                            NNshBoardDatabase   *bbsInfo,  UInt16 *selBBS);
static Boolean set_SearchTitleString     (MemHandle *bufH, UInt16 status, UInt16 *mode);
static Boolean search_NextTitle          (Int16 direction);
static Boolean search_NextBBS            (UInt16 start, UInt16 *bbsId);
static Boolean display_message           (void);
static UInt16  get_all_message           (void);
static Boolean update_message            (void);
static Boolean update_getall_message     (void);
//static Boolean delete_notreceived_message(Boolean update);
//static Boolean delete_message            (void);
static Boolean get_subject_txt           (void);
static Boolean popEvt_ThreadMain         (EventType *event);
static Boolean keyDownEvt_ThreadMain     (EventType *event);
static Boolean ctlSelEvt_ThreadMain      (EventType *event);
static Boolean ctlRepEvt_ThreadMain      (EventType *event);
static Boolean menuEvt_ThreadMain        (EventType *event);

static Boolean rctPointEvt_ThreadMain(EventType *event,
                                      FormType *frm, RectangleType *rect);

extern void    Initialize_NNshSetting(FormType *frm);
extern void    Effect_NNshSetting    (FormType *frm);
extern Boolean Handler_NNshSetting   (EventType *event);
extern Boolean Handler_JumpSelection(EventType *event);

/*-------------------------------------------------------------------------*/
/*   Function : change_handera_rotate                                      */
/*                                                         HandEra画面回転 */
/*-------------------------------------------------------------------------*/
static void change_handera_rotate(void)
{
    FormType            *frm;

#ifdef USE_HANDERA
    VgaScreenModeType   handEraScreenMode;     // スクリーンモード
    VgaRotateModeType   handEraRotateMode;     // 回転モード

    if(NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        // HandEraデバイスなら、画面を回転させる
        VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
        VgaSetScreenMode(screenMode1To1, VgaRotateSelect(handEraRotateMode));
        frm = FrmGetActiveForm();
        HandEraResizeForm(frm);
        FrmDrawForm(frm);
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, (NNshGlobal->NNsiParam)->selectedTitleItem,
                                                             NNSH_STEP_REDRAW);
        return;
    }
#endif
#ifdef USE_PIN_DIA
    if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNshGlobal->device == NNSH_DEVICE_GARMIN))
    {
        // DIAサポートデバイスなら、画面回転を実行
        VgaFormRotate_DIA();

        // 画面回転後の大きさにあわせ、フォームを調整し描画する
        frm = FrmGetActiveForm();
        if (HandEraResizeForm(frm) == true)
        {
            FrmDrawForm(frm);
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, (NNshGlobal->NNsiParam)->selectedTitleItem,
                                                             NNSH_STEP_REDRAW);
        }
        return;
    }
#endif    // #ifdef USE_PIN_DIA

    // "現在サポートしていません" 表示を行う
    NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : updateSelectionItem                                        */
/*                                                          選択itemの更新 */
/*-------------------------------------------------------------------------*/
static void updateSelectionItem(Int16 direction)
{
    FormType      *frm;
    RectangleType  dimF;
    Int16          item;
    UInt16         fontID, nlines, fontHeight;

    // 選択アイテムを移動させる
    item = (NNshGlobal->NNsiParam)->selectedTitleItem + direction;
    
    // 前ページが選択された場合...
    if (item < 0)
    {
        // 前ページがある場合には、１つ前のページを表示する
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_HALFWAY)||
             ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_LOWERLIMIT))
        {   
            // 前ページを表示する
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS,
                               NNSH_ITEM_LASTITEM, NNSH_STEP_PAGEDOWN);
        }
        return;
    }
         
    // 選択アイテムを変更(画面ページが切り替わらなかった場合には選択itemを更新)    
    if (item < NNshGlobal->nofTitleItems)
    {
        frm = FrmGetActiveForm();
        FrmGetObjectBounds(frm,
                            FrmGetObjectIndex(frm, GADID_MESSAGE_LIST), &dimF);

        fontID = ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ?
                           (NNshGlobal->NNsiParam)->currentFont : (NNshGlobal->NNsiParam)->sonyHRFontTitle;

        NNsi_UpdateRectangle(&dimF, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                                                &fontID, &fontHeight, &nlines);

        // スレ一覧の表示(描画実処理)
        NNsi_WinUpdateList(item, (NNshGlobal->NNsiParam)->selectedTitleItem,
                         NNshGlobal->msgTitleIndex, nlines, &dimF, fontHeight);
        (NNshGlobal->NNsiParam)->selectedTitleItem = item;

        return;
    }

    // 画面ページ切り替え(次ページがある場合には、次ページを表示する)
    if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_HALFWAY)||
        ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_UPPERLIMIT))
    {     
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_PAGEUP);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : change_list_font                                           */
/*                                                            フォント変更 */
/*-------------------------------------------------------------------------*/
static void change_list_font(void)
{
#ifdef USE_TSPATCH
    UInt32  ver, fontId;
    Err     ret;
#endif

    // フォントIDの選択
    if ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0)
    {
        // 通常描画モード用フォントの変更
        (NNshGlobal->NNsiParam)->currentFont = FontSelect((NNshGlobal->NNsiParam)->currentFont);

#ifdef USE_TSPATCH
        // TsPatch機能を使うとき...
        if ((NNshGlobal->NNsiParam)->notUseTsPatch == 0)
        {    
            // TsPatch適用中かどうかチェックする
            ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &ver);
            if (ret == errNone)
            {    
                // TsPatch適用中...フォントを小さくする。
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
#endif  // #ifdef USE_TSPATCH

    }
    else
    {
        // ハイレゾモード用フォントの変更
        (NNshGlobal->NNsiParam)->sonyHRFontTitle = FontSelect((NNshGlobal->NNsiParam)->sonyHRFontTitle);
    }

    // フォント変更後の表示は、リストを再作成して表示させる
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);

    NNshGlobal->updateHR = NNSH_UPDATE_DISPLAY;

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : change_graph_mode                                          */
/*                                        グラフィック描画モードの切り替え */
/*-------------------------------------------------------------------------*/
static void change_graph_mode(void)
{
    // ハイレゾモード/通常描画モードの切り替え(1:ハイレゾ, 0:通常描画)
    (NNshGlobal->NNsiParam)->useSonyTinyFontTitle = 
                                ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ? 1 : 0;

    // 描画モード変更後の表示は、リストを再作成して表示させる
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : Show_tab_info                                              */
/*                                                          タブ情報の表示 */
/*-------------------------------------------------------------------------*/
UInt16 Show_tab_info(Boolean disp)
{
    UInt16               tabNum, selBBS, count, loop;
    Char                 logMsg[MINIBUF], buffer[BUFSIZE * 2];
    ListType            *lstP;
    NNshBoardDatabase    bbsInfo;
    NNshSubjectDatabase *mesRead;
    MemHandle            mesH;
    DmOpenRef            dbRef;

    // 変数の初期化
    tabNum = 0;
    MemSet(buffer, sizeof(buffer), 0x00);

    // タブ名を作成
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);

    StrCopy(buffer, MSG_TAB_INFO);
    StrCat (buffer, LstGetSelectionText(lstP, selBBS));

    // "スレ検索中"を表示する
    Show_BusyForm(MSG_SEARCHING_TITLE);

    switch (selBBS)
    {
      case NNSH_SELBBS_NOTREAD:     // 未読あり
      case NNSH_SELBBS_GETALL:      // 取得済み全て
      case NNSH_SELBBS_FAVORITE:    // お気に入りスレ
        // BBS情報は取得しない(逐次取得)
        break;

      case NNSH_SELBBS_CUSTOM1:     // CUSTOM1
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom1.boardNick, &bbsInfo);
        break;

      case NNSH_SELBBS_CUSTOM2:     // CUSTOM2
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom2.boardNick, &bbsInfo);
        break;

      case NNSH_SELBBS_CUSTOM3:     // CUSTOM3
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom3.boardNick, &bbsInfo);
        break;

      case NNSH_SELBBS_CUSTOM4:     // CUSTOM4
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom4.boardNick, &bbsInfo);
        break;

      case NNSH_SELBBS_CUSTOM5:     // CUSTOM5
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom5.boardNick, &bbsInfo);
        break;

      default:
        // 特殊BBS以外のとき
        Get_BBS_Info(selBBS, &bbsInfo);
        break;
    }

    // 全スレ数を取得する
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &count);

    // 全件の確認をする
    for (loop = count; loop > 0; loop--)
    {
        // およその検索残り件数を表示する
        if ((loop % 10) == 0)
        {
            StrCopy(logMsg, MSG_SEARCHING_TITLE MSG_SEARCHING_LEFT);
            NUMCATI(logMsg, loop);
            SetMsg_BusyForm(logMsg);
        }

        // スレデータを取得する
        GetRecordReadOnly_NNsh(dbRef, (loop - 1), &mesH, (void **) &mesRead);

        // 条件に合致するかどうかチェック
        if (CheckDispList(selBBS, mesRead, &bbsInfo) == true)
        {
            tabNum++;
        }

        // レコードロックの解除
        ReleaseRecordReadOnly_NNsh(dbRef, mesH);
   }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);

    // 件数を表示する
    if (disp == true)
    {
        StrCat(buffer, MSG_TAB_COUNT);
        NUMCATI(buffer, tabNum);
        NNsh_ErrorMessage(ALTID_INFO, buffer, " ", 0);
    }
    return (tabNum);
}

/*-------------------------------------------------------------------------*/
/*   Function : setMultiPurposeSwitch                                      */
/*                                          多目的スイッチに初期データ設定 */
/*-------------------------------------------------------------------------*/
static void setMultiPurposeSwitch(FormType *frm, UInt16 itemID, UInt16 btnID)
{
    UInt16       data;
    ControlType *ctlP;

    // 多目的スイッチに設定された役目にあわせてデータを取得する
    switch (itemID & (NNSH_SWITCHUSAGE_FUNCMASK))
    {
      case NNSH_SWITCHUSAGE_GETPART:
        // 一括/分割
        data = (NNshGlobal->NNsiParam)->enablePartGet;
        break;

      case NNSH_SWITCHUSAGE_HANZEN:
        // 半角/全角
        data = (NNshGlobal->NNsiParam)->convertHanZen;
        break;

      case NNSH_SWITCHUSAGE_USEPROXY:
        // proxy使用
        data = (NNshGlobal->NNsiParam)->useProxy;
        break;

      case NNSH_SWITCHUSAGE_CONFIRM:
        // 確認を省略
        data = (NNshGlobal->NNsiParam)->confirmationDisable;
        break;

      case NNSH_SWITCHUSAGE_SUBJECT:
        // 一覧全取得
        data = (NNshGlobal->NNsiParam)->getAllThread;
        break;

      case NNSH_SWITCHUSAGE_TITLEDISP:
        // 描画モード
        data = (NNshGlobal->NNsiParam)->useSonyTinyFontTitle;
        break;

      case NNSH_SWITCHUSAGE_MSGNUMBER:
        // 一覧に番号
        data = (NNshGlobal->NNsiParam)->printNofMessage;
        break;

      case NNSH_SWITCHUSAGE_GETRESERVE:
        // スレ取得予約
        data = (NNshGlobal->NNsiParam)->getReserveFeature;
        break;

      case NNSH_SWITCHUSAGE_IMODEURL:
        // i-mode URL使用
        data = (NNshGlobal->NNsiParam)->useImodeURL;
        break;

      case NNSH_SWITCHUSAGE_BLOCKDISP:
        // ゾーン表示モード
        data = (NNshGlobal->NNsiParam)->blockDispMode;
        break;

      case NNSH_SWITCHUSAGE_MACHINEW:
        // まちBBSも新着確認を実施する
        data = (NNshGlobal->NNsiParam)->enableNewArrivalHtml;
        break;

      case NNSH_SWITCHUSAGE_USEPLUGIN:
        // 新着確認後にメールチェックを実施する
        data = (NNshGlobal->NNsiParam)->use_DAplugin;
        break;

      case NNSH_SWITCHUSAGE_LISTUPDATE:
        // 一覧更新時に同時にスレ差分取得
        data = (NNshGlobal->NNsiParam)->listAndUpdate;
        break;

      case NNSH_SWITCHUSAGE_NOREADONLY:
        // 取得済み全てには参照ログなし
        data = (UInt16) (NNshGlobal->NNsiParam)->notListReadOnly;
        break;
      
      case NNSH_SWITCHUSAGE_NORDOLY_NW:
        // 未読ありには参照ログなし
        data = (UInt16) (NNshGlobal->NNsiParam)->notListReadOnlyNew;
        break;

      case NNSH_SWITCHUSAGE_DISCONN_NA:
        // 新着確認終了後に回線を切断する
        data = (NNshGlobal->NNsiParam)->disconnArrivalEnd;
        break;
        
      case NNSH_SWITCHUSAGE_PREEXEC_NA:
        // 新着確認実行前にＤＡを実行する
        data = (UInt16) (NNshGlobal->NNsiParam)->preOnDAnewArrival;
        break;

      case NNSH_SWITCHUSAGE_PR_NOTREAD:
        // 一覧画面に表示する数値を未読数にする
        data = (UInt16) (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum;
        break;

      case NNSH_SWITCHUSAGE_GETLOGURL:
        // 一覧画面を参照ログ一覧再と別表示モードとして使用する場合。。。
        data = (UInt16) (NNshGlobal->NNsiParam)->getLogSiteListMode;
 #ifdef USE_LOGCHARGE
         if (data == 0)
         {
            // 通常モードへ切り替え
            FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));
            FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), POPTRID_BBSINFO));            
         }
         else
         {
            // 参照ログ一覧サイト別表示モードへ切り替え
            FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), POPTRID_BBSINFO));            
            FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));

            // サイト名を復旧する
            changeGetLogUrlSite(true);
         }
#endif  // #ifdef USE_LOGCHARGE
        break;

      case NNSH_SWITCHUSAGE_NOUSE:
      default:
        // 使用しない(チェックボックス自体画面表示しない)
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btnID));
        CtlSetValue  (ctlP, 0);
        CtlSetEnabled(ctlP, false);
        CtlSetUsable (ctlP, false);
        return;
        break;
    }
    // 設定値を画面に反映させる
    SetControlValue(frm, btnID, &data);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : updateMultiPurposeSwitch                                   */
/*                                                  多目的ボタンが押された */
/*-------------------------------------------------------------------------*/
static Boolean updateMultiPurposeSwitch(UInt16 paramID, UInt16 btnID)
{
    Boolean ret;
    UInt16  data;
    Char    *ptr;

    // データの初期化
    ret = false;
    data = 0;
    ptr  = "";

    // 設定されたデータを取得する
    UpdateParameter(FrmGetActiveForm(), btnID, &data);
    switch (paramID & (NNSH_SWITCHUSAGE_FUNCMASK))
    {
      case NNSH_SWITCHUSAGE_GETPART:
        // 一括/分割
        (NNshGlobal->NNsiParam)->enablePartGet = data;
        ptr = MSG_PARTGET_MODE;
        break;

      case NNSH_SWITCHUSAGE_HANZEN:
        // 半角/全角
        (NNshGlobal->NNsiParam)->convertHanZen = data;
        ptr = MSG_HANZEN_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_USEPROXY:
        // proxy使用
        (NNshGlobal->NNsiParam)->useProxy = data;
        ptr = MSG_USEPROXY_MODE;
        break;

      case NNSH_SWITCHUSAGE_CONFIRM:
        if (data == 0)
        {
            // 確認を省略
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_NOTHING;
        }
        else
        {
            // ダイアログを全て省略
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        }
        ptr = MSG_OMITCONFIRM_MODE;
        break;

      case NNSH_SWITCHUSAGE_SUBJECT:
        // 一覧全取得
        (NNshGlobal->NNsiParam)->getAllThread = data;
        ptr = MSG_GETALLSUBJECT_MODE;
        break;

      case NNSH_SWITCHUSAGE_TITLEDISP:
        // 描画モード
        (NNshGlobal->NNsiParam)->useSonyTinyFontTitle = data;
        ptr = MSG_GRAPHIC_DRAW_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_MSGNUMBER:
        // 一覧に番号
        (NNshGlobal->NNsiParam)->printNofMessage = data;
        ptr = MSG_SHOWMESSAGENUM_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_GETRESERVE:
        // スレ取得予約
        (NNshGlobal->NNsiParam)->getReserveFeature = data;
        ptr = MSG_GETRESERVE_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_IMODEURL:
        // i-mode URL使用モード
        (NNshGlobal->NNsiParam)->useImodeURL = data;
        ptr = MSG_IMODEURL_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_BLOCKDISP:
        // ゾーン表示モード
        (NNshGlobal->NNsiParam)->blockDispMode = data * NNSH_BLOCKDISP_NOFMSG;
        ptr = MSG_BLOCKDISP_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_MACHINEW:
        // まちBBSも新着確認する
        (NNshGlobal->NNsiParam)->enableNewArrivalHtml = data;
        ptr = MSG_MACHINEW_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_USEPLUGIN:
        // 新着確認時にメール到着確認も実施する
        (NNshGlobal->NNsiParam)->use_DAplugin = data;
        ptr = MSG_USE_DA_PLUGIN_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_LISTUPDATE:
        // 一覧取得時に同時にスレ差分取得を実施する
        (NNshGlobal->NNsiParam)->listAndUpdate = data;
        ptr = MSG_LISTGET_AND_UPDATE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_NOREADONLY:
        // 取得済み全てには参照ログなし
        (NNshGlobal->NNsiParam)->notListReadOnly = (UInt8) data;
        ptr = MSG_NOTREADONLY;
        ret = true;
        break;
      
      case NNSH_SWITCHUSAGE_NORDOLY_NW:
        // 未読ありには参照ログなし
        (NNshGlobal->NNsiParam)->notListReadOnlyNew = (UInt8) data;
        ptr = MSG_NOTREADONLY_NEW;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_DISCONN_NA:
        // 新着確認後に回線を切断する
         (NNshGlobal->NNsiParam)->disconnArrivalEnd = data;
         ptr = MSG_DISCONNECT_NEWARRIVAL;
         ret = true;
         break;

      case NNSH_SWITCHUSAGE_PREEXEC_NA:
        // 新着確認前にDAを実行する
         (NNshGlobal->NNsiParam)->preOnDAnewArrival = (UInt8) data;
         ptr = MSG_PREEXECDA_NEWARRIVAL;
         ret = true;
         break;

      case NNSH_SWITCHUSAGE_PR_NOTREAD:
        // 一覧画面に表示する数値を未読数にする
        if (data == 0)
        {
            // レス数を表示する
            (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = (UInt8) 0;
        }
        else
        {
            // 未読数を表示する
            (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = (UInt8) 1;
        }
        ptr = MSG_DISPLAY_NOTREAD;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_GETLOGURL:
        // 一覧画面を参照ログ一覧サイト別表示モードとして使用する場合。。。
        (NNshGlobal->NNsiParam)->getLogSiteListMode = (UInt8) data;
        NNshWinSetPopItems(FrmGetActiveForm(),POPTRID_BBSINFO, LSTID_BBSINFO, NNSH_SELBBS_OFFLINE);
        ptr = MSG_DISPLAY_GETLOGURL;
        (NNshGlobal->NNsiParam)->lastBBS = NNSH_SELBBS_OFFLINE;
        (NNshGlobal->NNsiParam)->selectedTitleItem = 0;
#ifdef USE_LOGCHARGE
        if (data == 0)
        {
            // 通常モードへ切り替え
            FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));
            FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), POPTRID_BBSINFO));            
        }
        else
        {
            // 参照ログ一覧サイト別表示モードへ切り替え
            FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), POPTRID_BBSINFO));            
            FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));            
        }         
#endif  // #ifdef USE_LOGCHARGE
        ret = true;
        break;
      
      case NNSH_SWITCHUSAGE_NOUSE:
      default:
        // 使用しない
        return (false);
        break;
    }
    if (data == 0)
    {
        // 設定を解除したことを通知する
        NNsh_InformMessage(ALTID_INFO, ptr, MSG_RESET_SUFFIX, 0);
    }
    else
    {
        // 設定したことを通知する
        NNsh_InformMessage(ALTID_INFO, ptr, MSG_SET_SUFFIX, 0);
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveSelectionTab                                           */
/*                                                      表示タブを変更する */
/*-------------------------------------------------------------------------*/
static void moveSelectionTab(UInt16 selBBS)
{
    ListType    *lstP;
    ControlType *ctlP;
    UInt16       savedParam;

    if ((NNshGlobal->NNsiParam)->autoDeleteNotYet != 0)
    {
        // 板移動時に未取得スレを削除する場合...
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        delete_notreceived_message(false);
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }

    // BBS表示リストを変更する
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    LstSetTopItem  (lstP, selBBS);
    LstSetSelection(lstP, selBBS);

    // BBS表示タブを変更する
    (NNshGlobal->NNsiParam)->lastBBS = selBBS;

    // 表示タブ(ラベル)を書き換える
    ctlP = FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO);
    CtlSetLabel(ctlP, LstGetSelectionText(lstP, selBBS));

    // リストを更新する
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);

    return; 
}

/*-------------------------------------------------------------------------*/
/*   Function : ExecuteAction                                              */
/*                                          機能を実行する(多目的ボタン用) */
/*-------------------------------------------------------------------------*/
Boolean ExecuteAction(UInt16 funcID)
{
    Boolean      ret = false;
    UInt16       item, dummy;
    EventType   *dummyEvent;
    Char        *data;

    // 機能コマンド番号をjumpSelectionに格納
    // (あんまり良くない。)
    NNshGlobal->jumpSelection = funcID;
    switch (funcID)
    {
      case MULTIBTN_FEATURE_INFO:
         // スレ情報
        if (NNshGlobal->nofTitleItems != 0)
        {
            item = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            (void) DisplayMessageInformations(item, &dummy);

            NNsi_RedrawThreadList();
        }
        break;

      case MULTIBTN_FEATURE_OPEN:
        // スレ参照
        display_message();
        break;

      case MULTIBTN_FEATURE_GET:
        // メッセージ取得(差分取得/全取得は自動切換え)
        update_getall_message();
        break;

      case MULTIBTN_FEATURE_PARTGET:
        // メッセージ差分取得
        update_message();
        break;

      case MULTIBTN_FEATURE_ALLGET:
        // メッセージ全部取得(再取得)
        get_all_message();
        break;

      case MULTIBTN_FEATURE_DELETE:
        // スレ削除
        delete_message();
        break;

      case MULTIBTN_FEATURE_SORT:
        // スレの整列を実行する
        Show_BusyForm(MSG_SORTING_WAIT);
        SortSubjectList();
        Hide_BusyForm(false);

        // スレタイトルの先頭へ移動させる
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        break;

      case MULTIBTN_FEATURE_MODTITLE:
        // スレタイトル変更
        modify_title();
        break;

      case MULTIBTN_FEATURE_OPENWEB:
        // 選択したスレをブラウザで開く
        if (NNshGlobal->browserCreator != 0)
        {
            // WebBrowserで開く
            launch_WebBrowser(NNSH_DISABLE);
        }
        else
        {
            // "現在サポートしていません" 表示を行う
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;

      case MULTIBTN_FEATURE_MESNUM:
        // スレ番指定のメッセージ取得
        Get_MessageFromMesNum(&ret, &dummy, false);
        if (ret == true)
        {
            // BBS一覧を初期化しておく
            MEMFREE_PTR(NNshGlobal->bbsTitles);

            // 使用BBS一覧の更新
            (void) Create_BBS_INDEX(&(NNshGlobal->bbsTitles), &item);
            if (item < (NNshGlobal->NNsiParam)->lastBBS)
            {
                (NNshGlobal->NNsiParam)->lastBBS = 0;
            }
        }
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        break;

      case MULTIBTN_FEATURE_SEARCH:
        // タイトル検索(文字列の設定)
        if (NNshGlobal->nofTitleItems != 0)
        {
            item = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem) + 1;
        }
        else
        {
            item = 0;
        }
        if (set_SearchTitleString(&(NNshGlobal->searchTitleH), item, NULL) != true)
        {
            return (true);
        }
        if ((NNshGlobal->NNsiParam)->searchPickupMode != 0)
        {
            // 絞込み検索モードのときはスレタイトルの先頭へ移動して終了する...
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_TO_TOP);
            return (true);
        }
        // not break;  (続けてスレタイ検索を実行する)

      case MULTIBTN_FEATURE_NEXT:
        // スレタイ検索処理(後方向)
        (void) search_NextTitle(NNSH_SEARCH_FORWARD);
        break;

      case MULTIBTN_FEATURE_PREV:
        // スレタイ検索処理(前方向)
        (void) search_NextTitle(NNSH_SEARCH_BACKWARD);
        break;

      case MULTIBTN_FEATURE_TOP:
        // スレタイトルの先頭へ移動
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_TO_TOP);
        break;

      case MULTIBTN_FEATURE_BOTTOM:
        // スレタイトルの末尾へ移動(ダミーで異常データを送り込む)
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, NNSH_ITEM_LASTITEM,
                                                          NNSH_STEP_TO_BOTTOM);
        break;

      case MULTIBTN_FEATURE_GETBBS:
        // 板情報の更新(取得)
        if (GetBBSList((NNshGlobal->NNsiParam)->bbs_URL) == false)
        {
            return (true);
        }
        /** not break; (続けて使用板選択も行う) **/

      case MULTIBTN_FEATURE_USEBBS:
        // 使用板選択画面を開く
        NNshMain_Close(FRMID_MANAGEBBS);
        break;

      case MULTIBTN_FEATURE_COPYGIKO:
        // 参照ログにコピーする
        copy_to_readOnly();
        break;

      case MULTIBTN_FEATURE_DELNOTGET:
        // 未取得スレ削除
        delete_notreceived_message(true);
        break;

      case MULTIBTN_FEATURE_DBCOPY:
        // DBをVFSにバックアップ
        if ((((NNshGlobal->NNsiParam)->useVFS) & (NNSH_VFS_ENABLE)) == NNSH_VFS_ENABLE)
        {
            // DBをVFSにバックアップする
            BackupDBtoVFS(NNSH_ENABLE);
        }
        else
        {
             // "現在サポートしていません" 表示を行う
             NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;

      case MULTIBTN_FEATURE_GRAPHMODE:
        // 描画モード変更
        change_graph_mode();
        break;

      case MULTIBTN_FEATURE_ROTATE:
        // HandEra画面回転
        change_handera_rotate();
        break;

      case MULTIBTN_FEATURE_FONT:
        // フォント変更
        change_list_font();
        break;

      case MULTIBTN_FEATURE_NETWORK:
        // ネットワーク設定を開く
        OpenNetworkPreferences();
        break;

      case MULTIBTN_FEATURE_SELMENU:
        // 選択メニュー表示
        open_menu();
        break;

      case MULTIBTN_FEATURE_DISCONN:
        // 回線切断
        NNshNet_LineHangup();
        break;

      case MULTIBTN_FEATURE_GETLIST:
        // スレ一覧取得(SUBJECT.TXT取得)
        get_subject_txt();
        break;

      case MULTIBTN_FEATURE_NEWARRIVAL:
        // 新着メッセージ確認
        update_newarrival_message(NNSH_NEWARRIVAL_NORMAL);
        break;

      case MULTIBTN_FEATURE_NNSHSET:
        // NNsi設定(概略)画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING_ABSTRACT);
        break;

      case MULTIBTN_FEATURE_NNSHSET1:
        // NNsi設定-1画面を開く
        NNshMain_Close(FRMID_CONFIG_NNSH);
        break;

      case MULTIBTN_FEATURE_NNSHSET2:
        // NNsi設定-2画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING2);
        break;

      case MULTIBTN_FEATURE_NNSHSET3:
        // NNsi設定-3画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING3);
        break;

      case MULTIBTN_FEATURE_NNSHSET4:
        // NNsi設定-4画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING4);
        break;

      case MULTIBTN_FEATURE_NNSHSET5:
        // NNsi設定-5画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING5);
        break;

      case MULTIBTN_FEATURE_NNSHSET6:
        // NNsi設定-6画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING6);
        break;

      case MULTIBTN_FEATURE_NNSHSET7:
        // NNsi設定-7画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING7);
        break;

      case MULTIBTN_FEATURE_NNSHSET8:
        // NNsi設定-8画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING8);
        break;

      case MULTIBTN_FEATURE_NNSHSET9:
        // NNsi設定-9画面を開く
        NNshMain_Close(FRMID_NNSI_SETTING9);
        break;

      case MULTIBTN_FEATURE_VERSION:
        // バージョン情報表示
        ShowVersion_NNsh();
        break;

      case MULTIBTN_FEATURE_SELECTNEXT:
        // 1つ下のアイテムを選択する
        updateSelectionItem(NNSH_STEP_ITEMDOWN);
        break;

      case MULTIBTN_FEATURE_SELECTPREV:
        // 1つ上のアイテムを選択する
        updateSelectionItem(NNSH_STEP_ITEMUP);
        break;

      case MULTIBTN_FEATURE_PREVPAGE:
        // 前ページの表示
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_HALFWAY)||
            ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_LOWERLIMIT))
        {
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_PAGEDOWN);
        }
        break;

      case MULTIBTN_FEATURE_NEXTPAGE:
        // 次ページの表示
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_HALFWAY)||
            ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_UPPERLIMIT))
        {
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_PAGEUP);
        }
        break;

      case MULTIBTN_FEATURE_NNSIEND:
        // NNsi終了
        dummyEvent = &(NNshGlobal->dummyEvent);
        MemSet(dummyEvent, sizeof(EventType), 0x00);
        dummyEvent->eType = appStopEvent;
        EvtAddEventToQueue(dummyEvent);
        break;

      case MULTIBTN_FEATURE_USER1TAB:
      case MULTIBTN_FEATURE_USER2TAB:
      case MULTIBTN_FEATURE_USER3TAB:
      case MULTIBTN_FEATURE_USER4TAB:
      case MULTIBTN_FEATURE_USER5TAB:
        // ユーザタブ設定画面を開く
        NNshMain_Close(FRMID_FAVORSET_DETAIL);
        break;

      case MULTIBTN_FEATURE_DEVICEINFO:
        // デバイスの情報表示
        ShowDeviceInfo_NNsh();
        break;
        
      case MULTIBTN_FEATURE_GOTODUMMY:
        // ダミー画面へ遷移
        NNshMain_Close(FRMID_NNSI_DUMMY);
        break;

      case MULTIBTN_FEATURE_NEWTHREAD:
        // スレ立て画面へ遷移
        newThread_message();
        break;

      case MULTIBTN_FEATURE_DIRSELECT:
        // ディレクトリ選択
        if ((NNshGlobal->NNsiParam)->lastBBS == NNSH_SELBBS_OFFLINE)
        {
            // 板タブが「参照ログ」の時だけ、格納ディレクトリ選択へ
            readonly_dir_selection();
        }
        break;
      case MULTIBTN_FEATURE_TABINFO:
        // タブ情報
        Show_tab_info(true);
        break;

      case MULTIBTN_FEATURE_MOVELOGLOC:
        // ログ管理場所切り替え
        move_log_location();
        break;

      case MULTIBTN_FEATURE_MOVELOGDIR:
        // ログ格納ディレクトリの変更
        move_log_directory();
        break;

      case MULTIBTN_FEATURE_TO_NOTREAD:
        // '未読あり'へ移動
        moveSelectionTab(NNSH_SELBBS_NOTREAD);
        break;

      case MULTIBTN_FEATURE_TO_GETALL:
        // '取得済み全て'へ移動
        moveSelectionTab(NNSH_SELBBS_GETALL);
        break;

      case MULTIBTN_FEATURE_LOGCHARGE:
        // 参照ログの取得
#ifdef USE_LOGCHARGE
        GetReadOnlyLogData_NNsh();
        
        // 画面表示を更新する
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                           (NNshGlobal->NNsiParam)->selectedTitleItem,
                           NNSH_STEP_REDRAW);
#endif
        break;

      case MULTIBTN_FEATURE_OPENURL:
#ifdef USE_LOGCHARGE
        // URLを開く
        openURL();
#endif
        break;

      case MULTIBTN_FEATURE_SETNGWORD1:
      case MULTIBTN_FEATURE_SETNGWORD2:
      case MULTIBTN_FEATURE_SETNGWORD3:
        // NGワード設定
        SetNGword3();
        NNsi_RedrawThreadList();
        break;

      case MULTIBTN_FEATURE_SETLOGCHRG:
#ifdef USE_LOGCHARGE
        // 参照ログ取得設定
        SetLogGetURL(NNSH_ITEM_BOTTOMITEM);
        NNsi_RedrawThreadList();
#endif
        break;

      case MULTIBTN_FEATURE_BT_ON:
        // Bluetooth ON
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(true);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MULTIBTN_FEATURE_BT_OFF:
        // Bluetooth OFF
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(false);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MULTIBTN_FEATURE_TO_OFFLINE:
        // '取得済み全て'へ移動
        moveSelectionTab(NNSH_SELBBS_OFFLINE);
        break;

      case MULTIBTN_FEATURE_TO_FAVOR:
        // 'お気に入り'へ移動
        moveSelectionTab(NNSH_SELBBS_FAVORITE);
        break;

      case MULTIBTN_FEATURE_TO_USER1:
        // 'ユーザ1'へ移動
        moveSelectionTab(NNSH_SELBBS_CUSTOM1);
        break;

      case MULTIBTN_FEATURE_TO_USER2:
        // 'ユーザ2'へ移動
        moveSelectionTab(NNSH_SELBBS_CUSTOM2);
        break;

      case MULTIBTN_FEATURE_TO_USER3:
        // 'ユーザ3'へ移動
        moveSelectionTab(NNSH_SELBBS_CUSTOM3);
        break;

      case MULTIBTN_FEATURE_TO_USER4:
        // 'ユーザ4'へ移動
        moveSelectionTab(NNSH_SELBBS_CUSTOM4);
        break;

      case MULTIBTN_FEATURE_TO_USER5:
        // 'ユーザ5'へ移動
        moveSelectionTab(NNSH_SELBBS_CUSTOM5);
        break;

      case MULTIBTN_FEATURE_MACRO_SET:
#ifdef USE_MACROEDIT
        // マクロ設定
        NNsi_MacroDataEdit(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT);
        //NNsi_RedrawThreadList();
#else
        // "現在サポートしていません" 表示を行う
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;
      
      case MULTIBTN_FEATURE_MACRO_EXEC:
#ifdef USE_MACRO
        // マクロ実行
        NNsi_ExecuteMacroMain(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);
        NNsi_RedrawThreadList();
#else
        // "現在サポートしていません" 表示を行う
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;

      case MULTIBTN_FEATURE_MACRO_VIEW:
#ifdef USE_MACRO
        // MACRO表示
        NNsi_MacroDataView(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT);
        //NNsi_RedrawThreadList();
#else
        // "現在サポートしていません" 表示を行う
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;

      case MULTIBTN_FEATURE_MACRO_LOG:
#ifdef USE_MACRO
        // MACROログ表示
        NNsi_MacroExecLogView(DBNAME_MACRORESULT, DBVERSION_MACRO);
        //NNsi_RedrawThreadList();
#else
        // "現在サポートしていません" 表示を行う
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;
      case MULTIBTN_FEATURE_SELECT_BBS:
#ifdef USE_LOGCHARGE
        if ((NNshGlobal->NNsiParam)->getLogSiteListMode != 0)
        {
            // サイトを選択する
            changeGetLogUrlSite(false);

            // スレ一覧の表示を更新
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_TO_TOP);
        }
        else
#endif // #ifdef USE_LOGCHARGE
        {
            // 板を選択
            CtlHitControl(FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO));
        }
        break;

      case MULTIBTN_FEATURE_OYSTERLOGIN:
#ifdef USE_SSL
        // '●'ログイン
        ProceedOysterLogin();
#else
        // "現在サポートしていません" 表示を行う
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;
        
      case MULTIBTN_FEATURE_USAGE:
        // 操作ヘルプの表示
        // (クリップボードに、nnDAに渡すデータについての指示を格納する)
        data = MEMALLOC_PTR(sizeof(nnDA_NNSIEXT_VIEWSTART
                                    nnDA_NNSIEXT_INFONAME
                                    nnDA_NNSIEXT_HELPLIST
                                    nnDA_NNSIEXT_ENDINFONAME
                                    nnDA_NNSIEXT_ENDVIEW) + MARGIN);
        if (data != NULL)
        {
            // nnDAを起動する
            MemSet(data, 
                   sizeof(nnDA_NNSIEXT_VIEWSTART
                           nnDA_NNSIEXT_INFONAME
                           nnDA_NNSIEXT_HELPLIST
                           nnDA_NNSIEXT_ENDINFONAME
                           nnDA_NNSIEXT_ENDVIEW) + MARGIN,
                   0x00);
            StrCopy(data, 
                    nnDA_NNSIEXT_VIEWSTART
                      nnDA_NNSIEXT_INFONAME
                      nnDA_NNSIEXT_HELPLIST
                      nnDA_NNSIEXT_ENDINFONAME
                      nnDA_NNSIEXT_ENDVIEW);            
            ClipboardAddItem(clipboardText, data, StrLen(data) + 1);
            MEMFREE_PTR(data);
            (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        }
        break;

#ifdef USE_LOGCHARGE
      case MULTIBTN_FEATURE_SETTOKEN:
        // 区切り文字列設定(先頭レコードを表示する)
        SetLogCharge_LogToken(0);
        break;

      case MULTIBTN_FEATURE_OPENGETLOGLIST:
        // 取得参照ログ一覧画面を開く
        NNshMain_Close(FRMID_LISTGETLOG);
        break;
#endif  // #ifdef USE_LOGCHARGE

      case MULTIBTN_FEATURE_SEARCHBBS:
        // 板を検索する
        if (set_SearchTitleString(&(NNshGlobal->searchBBSH), 0, NULL) != true)
        {
            return (true);
        }
        if (search_NextBBS(NNSH_NOF_SPECIAL_BBS, &item) == false)
        {
            // 見つからなかった...
            return (true);
        }
        // 見つかった、表示している板を変更する
        moveSelectionTab(item);
        break;

      case MULTIBTN_FEATURE_NEXTSEARCHBBS:
        // 板を次検索する
        if (search_NextBBS(((NNshGlobal->NNsiParam)->lastBBS + 1), &item) == false)
        {
            // 見つからなかった...
            return (true);
        }
        // 見つかった、表示している板を変更する
        moveSelectionTab(item);
        break;

      case MULTIBTN_FEATURE_BEAMURL:
        // URLをBeamする
        beam_URL();
        break;

      case MULTIBTN_FEATURE_UPDATERESNUM:
        // レス数の計算(表示)を実施する
        UpdateThreadResponseNumber(convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem));

        // 画面表示を更新する
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                           (NNshGlobal->NNsiParam)->selectedTitleItem,
                           NNSH_STEP_REDRAW);
        break;

      case MULTIBTN_FEATURE_OPENWEBDA:
        // webDAで開く
        launch_WebBrowser(NNSH_ENABLE);
        break;
        
      case MULTIBTN_FEATURE_SELECT_MACRO_EXEC:
        // マクロ選択実行...
#ifdef USE_MACRO
        // マクロ実行
        if (NNsi_ExecuteMacroSelection() == true)
        {
            // 画面再表示...
            NNsi_RedrawThreadList();
        }
#else
        // "現在サポートしていません" 表示を行う
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;

      case MULTIBTN_FEATURE_BE_LOGIN:
        // Be@2chログイン処理
        ProceedBe2chLogin();
        break;

      case MULTIBTN_FEATURE_LIST_GETLOG:
        // スレ番号指定予約リストを表示する
        ShowReserveGetLogList();
        break;

      default:
        // その他(何もしない)
        NNsh_DebugMessage(ALTID_WARN, "Unknown FuncID", "", funcID);
        break;
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   select_readonly_directory                                */
/*                                 　　  「参照ログ」のディレクトリを選択  */
/*-------------------------------------------------------------------------*/
static UInt16 select_readonly_directory(UInt16 orgDir)
{
    Char      *listPtr, area[(MAX_DIRNAME + 1) * MAX_DEPTH + MARGIN], *ptr;
    UInt16     itemCnt, btnId, size;
    DmOpenRef  dirRef;

    // ディレクトリカウント数を取得する
    OpenDatabase_NNsh(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, &dirRef);
    GetDBCount_NNsh(dirRef, &itemCnt);
    CloseDatabase_NNsh(dirRef);

    // リスト文字列確保領域の設定
    size    = (itemCnt + 2) * (MAX_LISTLENGTH + MARGIN) + MARGIN;
    listPtr = MEMALLOC_PTR(size);
    if (listPtr == NULL)
    {
        return (orgDir);
    }
    MemSet(listPtr, size, 0x00);
    
    // "スレ一覧作成中" の表示
    Show_BusyForm(MSG_MAKE_DIRECTORY_WAIT);

    // サブディレクトリ名称を(全部)取得する
    for (ptr = listPtr, size = 0; size <= itemCnt; size++)
    {        
        MemSet (area, sizeof(area), 0x00);
        area[0] = chrEllipsis;
        area[1] = '/';
        GetSubDirectoryName(size, &area[2]);
        btnId = StrLen(area);
        if (btnId > MAX_LISTLENGTH)
        {
            // 格納領域からあふれ出ていた場合...
            *ptr = chrEllipsis;
            ptr++;
            StrCopy(ptr, &area[btnId - MAX_LISTLENGTH]);
        }
        else
        {
            StrCopy(ptr, area);
        }
        ptr = ptr + StrLen(ptr) + 1;
    }

    // ディレクトリ作成中を消去
    Hide_BusyForm(false);

    // （ディレクトリ）選択ウィンドウを表示する
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, listPtr, 
                                                        (itemCnt + 1), orgDir);
    if (btnId == BTNID_JUMPCANCEL)
    {
        // キャンセルされた場合、、、指定されたディレクトリ番号を応答
        NNshGlobal->jumpSelection = orgDir;
    }

    // 確保した領域をクリアする
    MEMFREE_PTR(listPtr);
    
    // 選択された(ディレクトリ)番号を応答する
    return (NNshGlobal->jumpSelection);
}


/*-------------------------------------------------------------------------*/
/*   Function :   readonly_dir_selection                                   */
/*                                　 「参照ログ」の格納ディレクトリ選択  */
/*-------------------------------------------------------------------------*/
static Boolean readonly_dir_selection(void)
{
    UInt16 newDir;

    // ディレクトリの階層表示を実施しない場合...
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_DIROFFLINE) == 0)
    {
        return (false);
    }

    // ディレクトリを選択する
    newDir = select_readonly_directory((NNshGlobal->NNsiParam)->readOnlySelection);

    if (newDir != (NNshGlobal->NNsiParam)->readOnlySelection)
    {
        (NNshGlobal->NNsiParam)->readOnlySelection = newDir;

        // 選択されたディレクトリが更新された場合には、スレタイ一覧を更新する
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
    }
    else
    {
        // スレ一覧画面を再表示する
        NNsi_RedrawThreadList();
    }
    
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   open_menu                                                */
/*                                                 NNsi独自のメニュー表示  */
/*-------------------------------------------------------------------------*/
static Boolean open_menu(void)
{
    ControlType *ctlP;
    EventType   *dummyEvent;
    Char      *ptr, listPtr[NNSH_ITEMLEN_JUMPLIST*(NNSH_JUMPSEL_NUMLIST + 1)];
    UInt16    btnId, savedParam, jumpList[NNSH_JUMPSEL_NUMLIST + 1], itemCnt;

    // リストの作成
    MemSet(listPtr, sizeof(listPtr), 0x00);
    MemSet(jumpList, sizeof(jumpList), 0x00);
    ptr = listPtr;
    itemCnt = 0;

    // BBSを選択
#ifdef USE_LOGCHARGE
    if ((NNshGlobal->NNsiParam)->getLogSiteListMode != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_OPENBBS_GETLOG);
        ptr = ptr + sizeof(NNSH_JUMPMSG_OPENBBS_GETLOG);
        jumpList[itemCnt] = NNSH_JUMPSEL_OPENBBS;
        itemCnt++;
    }
    else
#endif  // #ifdef USE_LOGCHARGE
    {
        StrCopy(ptr, NNSH_JUMPMSG_OPENBBS);
        ptr = ptr + sizeof(NNSH_JUMPMSG_OPENBBS);
        jumpList[itemCnt] = NNSH_JUMPSEL_OPENBBS;
        itemCnt++;
    }

    // メニューを開く
    if ((NNshGlobal->NNsiParam)->addMenuTitle != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_OPENMENU);
        ptr = ptr + sizeof(NNSH_JUMPMSG_OPENMENU);
        jumpList[itemCnt] = NNSH_JUMPSEL_OPENMENU;
        itemCnt++;
    }

    // 回線切断
    StrCopy(ptr, NNSH_JUMPMSG_DISCONNECT);
    ptr = ptr + sizeof(NNSH_JUMPMSG_DISCONNECT);
    jumpList[itemCnt] = NNSH_JUMPSEL_DISCONNECT;
    itemCnt++;

    // 新着確認
    StrCopy(ptr, NNSH_JUMPMSG_OPENNEW);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENNEW);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENNEW;
    itemCnt++;

    // 一覧取得
    StrCopy(ptr, NNSH_JUMPMSG_OPENLIST);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENLIST);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENLIST;
    itemCnt++;

    // スレ取得
    StrCopy(ptr, NNSH_JUMPMSG_OPENGET);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENGET);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENGET;
    itemCnt++;

    // スレ参照
    StrCopy(ptr, NNSH_JUMPMSG_OPENMES);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENMES);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENMES;
    itemCnt++;

    // 参照ログへコピー
    if ((NNshGlobal->NNsiParam)->addMenuCopyMsg != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_COPYMSG);
        ptr = ptr + sizeof(NNSH_JUMPMSG_COPYMSG);
        jumpList[itemCnt] = NNSH_JUMPSEL_COPYMSG;
        itemCnt++;
    }

    // スレ情報
    StrCopy(ptr, NNSH_JUMPMSG_OPENINFO);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENINFO);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENINFO;
    itemCnt++;

    // スレ削除
    if ((NNshGlobal->NNsiParam)->addMenuDeleteMsg != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_DELMSG);
        ptr = ptr + sizeof(NNSH_JUMPMSG_DELMSG);
        jumpList[itemCnt] = NNSH_JUMPSEL_DELMSG;
        itemCnt++;
    }

    // 参照ログ一覧画面へ
    StrCopy(ptr, NNSH_JUMPMSG_OPENGETLOGLIST);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENGETLOGLIST);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENGETLOGLIST;
    itemCnt++;

    // 描画モード変更
    if ((NNshGlobal->NNsiParam)->addMenuGraphTitle != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_GRAPHMODE);
        ptr = ptr + sizeof(NNSH_JUMPMSG_GRAPHMODE);
        jumpList[itemCnt] = NNSH_JUMPSEL_GRAPHMODE;
        itemCnt++;
    }

    // 多目的スイッチ１
    if ((NNshGlobal->NNsiParam)->addMenuMultiSW1 != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_MULTISW1);
        ptr = ptr + sizeof(NNSH_JUMPMSG_MULTISW1);
        jumpList[itemCnt] = NNSH_JUMPSEL_MULTISW1;
        itemCnt++;
    }

    // 多目的スイッチ２
    if ((NNshGlobal->NNsiParam)->addMenuMultiSW2 != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_MULTISW2);
        ptr = ptr + sizeof(NNSH_JUMPMSG_MULTISW2);
        jumpList[itemCnt] = NNSH_JUMPSEL_MULTISW2;
        itemCnt++;
    }

    // デバイス情報
    if ((NNshGlobal->NNsiParam)->addMenuDeviceInfo != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_SHOWDEVINFO);
        ptr = ptr + sizeof(NNSH_JUMPMSG_SHOWDEVINFO);
        jumpList[itemCnt] = NNSH_JUMPSEL_SHOWDEVINFO;
        itemCnt++;
    }

    // Dir選択とDir移動
    if ((NNshGlobal->NNsiParam)->addMenuDirSelect != 0)
    {
        if ((NNshGlobal->NNsiParam)->lastBBS == NNSH_SELBBS_OFFLINE)
        {
            // 参照ログのときのみ表示する
            StrCopy(ptr, NNSH_JUMPMSG_DIRSELECTION);
            ptr = ptr + sizeof(NNSH_JUMPMSG_DIRSELECTION);
            jumpList[itemCnt] = NNSH_JUMPSEL_DIRSELECTION;
            itemCnt++;

            StrCopy(ptr, NNSH_JUMPMSG_CHANGELOGDIR);
            ptr = ptr + sizeof(NNSH_JUMPMSG_CHANGELOGDIR);
            jumpList[itemCnt] = NNSH_JUMPSEL_CHANGELOGDIR;
            itemCnt++;
        }
    }

    // NNsi終了
    if ((NNshGlobal->NNsiParam)->addMenuNNsiEnd != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_NNSIEND);
        ptr = ptr + sizeof(NNSH_JUMPMSG_NNSIEND);
        jumpList[itemCnt] = NNSH_JUMPSEL_NNSIEND;
        itemCnt++;
    }

    // ログ位置移動
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        // VFSを使用する設定のときのみ表示する
        StrCopy(ptr, NNSH_JUMPMSG_CHANGELOGLOC);
        ptr = ptr + sizeof(NNSH_JUMPMSG_CHANGELOGLOC);
        jumpList[itemCnt] = NNSH_JUMPSEL_CHANGELOGLOC;
        itemCnt++;
    }

#ifdef USE_BT_CONTROL
    // Bluetooth On/Off
    if ((NNshGlobal->NNsiParam)->addBtOnOff != 0)
    {
            // リストに「Bt On/Bt Off」を追加
            StrCopy(ptr, NNSH_JUMPMSG_BT_ON);
            ptr = ptr + sizeof(NNSH_JUMPMSG_BT_ON);
            jumpList[itemCnt] = NNSH_JUMPSEL_BT_ON;
            itemCnt++;

            StrCopy(ptr, NNSH_JUMPMSG_BT_OFF);
            ptr = ptr + sizeof(NNSH_JUMPMSG_BT_OFF);
            jumpList[itemCnt] = NNSH_JUMPSEL_BT_OFF;
            itemCnt++;
    }
#endif

    // 選択ウィンドウを表示する
    btnId = NNshWinSelectionWindow(FRMID_JUMPMSG, listPtr, itemCnt, 0);
    if (btnId != BTNID_JUMPCANCEL)
    {
        // 選択リスト番号→メニュー番号の変換
        btnId = jumpList[NNshGlobal->jumpSelection];
        switch (btnId)
        {
          case NNSH_JUMPSEL_OPENBBS:
            // BBS一覧を選択(ポップアップ)
            (void) ExecuteAction(MULTIBTN_FEATURE_SELECT_BBS);
            break;

          case NNSH_JUMPSEL_OPENMENU:
            // メニューを開く
            // (ダミーで左上部分のpenDownEventを生成)
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType    = penDownEvent;
            dummyEvent->penDown  = true;
            dummyEvent->tapCount = 1;
            dummyEvent->screenX  = 5;
            dummyEvent->screenY  = 5;
            EvtAddEventToQueue(dummyEvent);
            break;

          case NNSH_JUMPSEL_DISCONNECT:
            // 回線切断
            savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            NNshNet_LineHangup();
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
            break;

          case NNSH_JUMPSEL_OPENNEW:
            // 新着確認
            (void) ExecuteAction(MULTIBTN_FEATURE_NEWARRIVAL);
            break;

          case NNSH_JUMPSEL_OPENLIST:
            // 一覧更新
            (void) ExecuteAction(MULTIBTN_FEATURE_GETLIST);
            break;

          case NNSH_JUMPSEL_OPENGET:
            // メッセージ取得(差分取得/全部取得は自動判断)
            (void) ExecuteAction(MULTIBTN_FEATURE_GET);
            break;

          case NNSH_JUMPSEL_OPENMES:
            // スレ参照
            (void) ExecuteAction(MULTIBTN_FEATURE_OPEN);
            break;

          case NNSH_JUMPSEL_OPENINFO:
            // スレ情報表示
            (void) ExecuteAction(MULTIBTN_FEATURE_INFO);
            break;

          case NNSH_JUMPSEL_COPYMSG:
            // 参照ログへコピー
            (void) ExecuteAction(MULTIBTN_FEATURE_COPYGIKO);
            break;

          case NNSH_JUMPSEL_DELMSG:
            // スレ削除
            (void) ExecuteAction(MULTIBTN_FEATURE_DELETE);
            break;

          case NNSH_JUMPSEL_GRAPHMODE:
            // 描画モード変更
            (void) ExecuteAction(MULTIBTN_FEATURE_GRAPHMODE);
            return (true);
            break;

          case NNSH_JUMPSEL_MULTISW1:
            // 多目的スイッチ１を押したことにする
            ctlP = FRM_GET_ACTIVE_OBJECT_PTR(CHKID_SELECT_GETMODE);
            btnId = CtlGetValue(ctlP);
            btnId = (btnId == 0) ? 1 : 0;
            CtlSetValue(ctlP, btnId);
            CtlHitControl(ctlP);
            break;

          case NNSH_JUMPSEL_MULTISW2:
            // 多目的スイッチ２を押したことにする
            ctlP = FRM_GET_ACTIVE_OBJECT_PTR(CHKID_SELECT_MULTIPURPOSE);
            btnId = CtlGetValue(ctlP);
            btnId = (btnId == 0) ? 1 : 0;
            CtlSetValue(ctlP, btnId);
            CtlHitControl(ctlP);
            break;

          case NNSH_JUMPSEL_SHOWDEVINFO:
            // デバイス情報表示
            (void) ExecuteAction(MULTIBTN_FEATURE_DEVICEINFO);
            break;

          case NNSH_JUMPSEL_DIRSELECTION:
            // ディレクトリ選択
            (void) ExecuteAction(MULTIBTN_FEATURE_DIRSELECT);
            return (true);
            break;

          case NNSH_JUMPSEL_NNSIEND:
            // NNsi終了
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType    = appStopEvent;
            EvtAddEventToQueue(dummyEvent);
            break;

          case NNSH_JUMPSEL_CHANGELOGDIR:
            // ログ格納ディレクトリの変更
            (void) ExecuteAction(MULTIBTN_FEATURE_MOVELOGDIR);
            return (true);
            break;

          case NNSH_JUMPSEL_CHANGELOGLOC:
            // ログ管理場所の変更
            (void) ExecuteAction(MULTIBTN_FEATURE_MOVELOGLOC);
            return (true);
            break;
            
#ifdef USE_BT_CONTROL
          case NNSH_JUMPSEL_BT_ON:
            // Bluetooth On
            (void) ExecuteAction(MULTIBTN_FEATURE_BT_ON);
            break;

          case NNSH_JUMPSEL_BT_OFF:
            // Bluetooth Off
            (void) ExecuteAction(MULTIBTN_FEATURE_BT_OFF);
            break;
#endif
          case NNSH_JUMPSEL_OPENGETLOGLIST:
            // 参照ログ一覧画面を開く
            (void) ExecuteAction(MULTIBTN_FEATURE_OPENGETLOGLIST);
            break;

          default:
            // 何もしない
            break;
        }
    }
    // スレ一覧画面を再表示する
    NNsi_RedrawThreadList();
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : NNshMain_Close                                             */
/*                                                      一覧画面のクローズ */
/*-------------------------------------------------------------------------*/
static void NNshMain_Close(UInt16 nextFormID)
{
#ifdef ERASE_NOTRECEIVED_THREAD_CLOSEFORM
    UInt16 savedParam;

    if ((NNshGlobal->NNsiParam)->autoDeleteNotYet != 0)
    {
        // 板移動時に未取得スレを削除する
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        delete_notreceived_message(false);
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }
#endif

    // スレタイトル表示情報を削除
    ClearMsgTitleInfo();

    // 別の画面を開く
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(nextFormID);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   copy_to_readOnly                                         */
/*                                      ログファイルを参照ログにコピーする */
/*-------------------------------------------------------------------------*/
static Boolean copy_to_readOnly(void)
{
    Err                  ret;
    UInt16               selM, selBBS, butID;
    NNshSubjectDatabase  mesInfo;
    NNshBoardDatabase    bbsInfo;
    Char                *fileName, *orgName, *area;

    // 本機能は、VFS専用である旨表示する(VFS以外は処理を行わない)
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        NNsh_ErrorMessage(ALTID_WARN, MSG_ONLYVFS_FEATURE, "", 0);
        return (false);
    }

    // バッファの確保
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME);
    if (fileName == NULL)
    {
        return (false);
    }
    orgName  = MEMALLOC_PTR(MAXLENGTH_FILENAME);
    if (orgName == NULL)
    {
        MEMFREE_PTR(fileName);
        return (false);
    }

    // スレ情報を取得する
    ret = get_message_Info(&mesInfo, &selM, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        //  スレ未取得のため、コピーは行わない
        NNsh_InformMessage(ALTID_WARN, MSG_NOTGET_MESSAGE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // コピー先ディレクトリを作成する
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);

    // ログベースディレクトリを付加する
    area = NULL;
    if (NNshGlobal->logPrefixH != 0)
    {
        area = MemHandleLock(NNshGlobal->logPrefixH);
    }
    if (area == NULL)
    {
        StrCopy(fileName, LOGDATAFILE_PREFIX);
    }
    else
    {
        StrCopy(fileName, area);
        MemHandleUnlock(NNshGlobal->logPrefixH);
    }
    StrCat (fileName, mesInfo.boardNick);
    fileName[StrLen(fileName) - 1] = '\0';
    (void) CreateDir_NNsh(fileName);

    // コピー先ファイル名を作成する
    StrCat (fileName, "/");
    StrCat (fileName, mesInfo.threadFileName);

    // コピー元ファイル名を作成する
    MemSet (orgName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(orgName, mesInfo.boardNick);
    StrCat (orgName, mesInfo.threadFileName);

    // ファイルを本当にコピーするかを確認する
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_COPY,
                                mesInfo.threadTitle, 0);
    if (butID != 0)
    {
        // Cancelを選択(何もしない)
        ret = ~errNone;
        goto FUNC_END;
    }

    // コピーの実施
    NNsh_DebugMessage(ALTID_INFO, "Dest :",   fileName, 0);
    NNsh_DebugMessage(ALTID_INFO, "Source :", orgName, 0);
    Show_BusyForm(MSG_COPY_IN_PROGRESS);

    if (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        // Palm本体からVFSの参照ログへコピーする
        ret = CopyFile_NNsh(fileName, orgName, NNSH_COPY_PALM_TO_VFS);
    }
    else
    {
        // VFSのログからVFSの参照ログへコピーする
        ret = CopyFile_NNsh(fileName, orgName, NNSH_COPY_VFS_TO_VFS);
    }
    Hide_BusyForm(false);
    if (ret != errNone)
    {
        // ファイルコピーに失敗した。
        (void) DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);

        // コピーに失敗した旨表示する
        NNsh_ErrorMessage(ALTID_ERROR, MSG_FAILURE_COPY, fileName, ret);
        goto FUNC_END;
    }

    // コピー成功を通知する
    NNsh_InformMessage(ALTID_INFO, MSG_INFORM_COPYEND, "", 0);

FUNC_END:
    MEMFREE_PTR(orgName);
    MEMFREE_PTR(fileName);
    if (ret != errNone)
    {
        return (false);
    }

    if ((NNshGlobal->NNsiParam)->copyDelReadOnly != 0)
    {
        // コピーが成功したとき、コピー元ファイルを削除する
        return (delete_message());
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   move_log_directory                                       */
/*                                ログファイルの格納ディレクトリを変更する */
/*-------------------------------------------------------------------------*/
static Boolean move_log_directory(void)
{
    Err                  ret;
    UInt16               selM, dirIndex;
    Char                 source[BUFSIZE + MARGIN], dest[BUFSIZE + MARGIN];
    Char                *area;
    NNshSubjectDatabase  mesInfo;

    ret = ~errNone;

    // 本機能は、VFS専用である旨表示する(VFS以外は処理を行わない)
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        NNsh_ErrorMessage(ALTID_WARN, MSG_ONLYVFS_FEATURE, "", 0);
        goto FUNC_END;
    }

    // 本機能は、VFSに参照ログを置く場合にしか使えない旨表示する
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_OFFLOG_VFS_FEATURE, "", 0);
        goto FUNC_END;
    }

    // スレ情報を取得する
    ret = get_subject_info(&mesInfo, &selM);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_info()", "", ret);
        goto FUNC_END;
    }

    // 指定されたスレが参照ログか確認する
#ifdef USE_STRSTR
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) != 0)
#else
    if ((mesInfo.boardNick[0]  != '!')||
        (mesInfo.boardNick[1]  != 'G')||
        (mesInfo.boardNick[2]  != 'i')||
        (mesInfo.boardNick[3]  != 'k'))
#endif
    {
        //  参照ログではないため、移動は行わない
        NNsh_InformMessage(ALTID_WARN, MSG_NOTMOVE_LOGMESSAGE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // ログ移動先の指定
    dirIndex = select_readonly_directory(mesInfo.dirIndex);
    if (dirIndex == mesInfo.dirIndex)
    {
        // ログの移動先が指定されなかった、終了する
        ret = ~errNone;
        goto FUNC_END;
    }

    // 参照ログ格納ディレクトリのベースを付加する
    if ((NNshGlobal->logPrefixH == 0)||
         ((area = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
    {
        // デフォルトのディレクトリ名を使用
        StrCopy(source, LOGDATAFILE_PREFIX);
        StrCopy(dest,   LOGDATAFILE_PREFIX);
    }
    else
    {
        // カスタマイズされたディレクトリ名を使用する
        StrCopy(source, area);
        StrCopy(dest,   area);
        MemHandleUnlock(NNshGlobal->logPrefixH);
    }

    // さらにサブディレクトリを検索して付加する
    GetSubDirectoryName(mesInfo.dirIndex, &source[StrLen(source)]);
    GetSubDirectoryName(dirIndex, &dest[StrLen(dest)]);

    // ファイル名を付加する
    StrCat(source, mesInfo.threadFileName);
    StrCat(dest,   mesInfo.threadFileName);

    // ファイルを本当にコピーするかを確認する
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_MOVELOGDIR,
                            mesInfo.threadTitle, 0) != 0)
    {
        // Cancelを選択(何もしない)
        ret = ~errNone;
        goto FUNC_END;
    }

    // ログディレクトリ変更の実施
    Show_BusyForm(MSG_MOVEDIR_IN_PROGRESS);

    // ログ管理実行実処理
    ret = CopyFile_NNsh(dest, source, NNSH_COPY_VFS_TO_VFS);
    if (ret != errNone)
    {
        // ファイルのコピーに失敗
        // コピーしようとしたファイル(中途半端なファイル)を削除する
        DeleteFile_NNsh(dest, NNSH_VFS_ENABLE);
    }
    else
    {
        // コピー元ログファイルを削除する
        DeleteFile_NNsh(source, NNSH_VFS_ENABLE);

        // DBに記録されている格納場所を更新
        mesInfo.dirIndex = dirIndex;
    }

    Hide_BusyForm(false);
    if (ret != errNone)
    {
        // コピーに失敗した旨表示する
        NNsh_ErrorMessage(ALTID_ERROR, MSG_FAILURE_COPY, mesInfo.threadFileName, ret);
        goto FUNC_END;
    }

    // コピー成功を通知する
    NNsh_InformMessage(ALTID_INFO, MSG_INFORM_CHGLOGDIR_END, dest, 0);

    // データを更新(選択番号からDB Index番号へ変換して更新)
    dirIndex = convertListIndexToMsgIndex(selM);
    ret = update_subject_database(dirIndex, &mesInfo);
    if (ret != errNone)
    {
        // ありえないはず...
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()"," ", ret);
    }

FUNC_END:
    // ログのコピーに成功した場合は、リストを再作成して表示させる
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   move_log_location                                        */
/*                                        ログファイルの管理場所を変更する */
/*-------------------------------------------------------------------------*/
static Boolean move_log_location(void)
{
    Err                  ret;
    UInt16               selM;
    NNshSubjectDatabase  mesInfo;

    // 本機能は、VFS専用である旨表示する(VFS以外は処理を行わない)
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        NNsh_ErrorMessage(ALTID_WARN, MSG_ONLYVFS_FEATURE, "", 0);
        return (false);
    }

    // スレ情報を取得する
    ret = get_subject_info(&mesInfo, &selM);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_info()", "", ret);
        goto FUNC_END;
    }

    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        //  スレ未取得のため、コピーは行わない
        NNsh_InformMessage(ALTID_WARN, MSG_NOTGET_MESSAGE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // ファイルを本当にコピーするかを確認する
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_MANAGELOGLOC,
                                mesInfo.threadTitle, 0) != 0)
    {
        // Cancelを選択(何もしない)
        ret = ~errNone;
        goto FUNC_END;
    }

    // 管理場所変更の実施
    Show_BusyForm(MSG_CHANGE_IN_PROGRESS);

    // ログ管理実行実処理
    ret = MoveLogMessage((NNshGlobal->NNsiParam)->openMsgIndex);

    Hide_BusyForm(false);
    if (ret != errNone)
    {
        // コピーに失敗した旨表示する
        NNsh_ErrorMessage(ALTID_ERROR, MSG_FAILURE_COPY, mesInfo.threadFileName, ret);
        goto FUNC_END;
    }

    // コピー成功を通知する
    NNsh_InformMessage(ALTID_INFO, MSG_INFORM_CHANGELOCEND, mesInfo.threadTitle, 0);

FUNC_END:
    if (ret != errNone)
    {
        return (false);
    }

    // ログのコピーに成功した場合は、リストを再作成して表示させる
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   set_SearchTitleString                                    */
/*                                                スレタイ検索文字列の設定 */
/*-------------------------------------------------------------------------*/
static Boolean set_SearchTitleString(MemHandle *bufH, UInt16 status, UInt16 *mode)
{
    Boolean             ret = false;
    Char               *ptr = NULL;
    UInt16              usage = NNSH_DIALOG_USE_SEARCH;
    NNshSubjectDatabase mesInfo;

    // 検索文字列領域の取得
    if (*bufH == 0)
    {
        *bufH = MemHandleNew(BUFSIZE);
        if (*bufH == 0)
        {
            return (false);
        }
        ptr = MemHandleLock(*bufH);
        if (ptr != NULL)
        {
            MemSet(ptr, BUFSIZE, 0x00);
        }
    }
    else
    {
        ptr = MemHandleLock(*bufH);
    }
    if (ptr == NULL)
    {
        // 領域取得失敗
        MemHandleFree(*bufH);
        *bufH = 0;
        return (false);
    }

    // 現在のカーソルがあるタイトル名を反映させる
    MemSet(&mesInfo, sizeof(NNshSubjectDatabase), 0x00);
    if (status != 0)
    {
        // スレタイ検索モードのとき...
        usage = NNSH_DIALOG_USE_SEARCH_TITLE;
        if (StrLen(ptr) == 0)
        {
            // メッセージ情報をデータベースから取得
            if (Get_Subject_Database((status - 1), &mesInfo) == errNone)
            {
                // タイトルを検索用文字列バッファをコピー
                StrNCopy(ptr, mesInfo.threadTitle, BUFSIZE);        
            }
        }
    }

    // 文字列入力ウィンドウを開き、データを獲得
    ret = DataInputDialog(NNSH_INPUTWIN_SEARCH, ptr, BUFSIZE, usage, mode);

    MemHandleUnlock(*bufH);

    // スレ一覧画面を再表示する
    NNsi_RedrawThreadList();
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   isValidRecordForSearchTitle                              */
/*                  スレタイ検索処理を実施してもよいスレかどうかのチェック */
/*-------------------------------------------------------------------------*/
static Boolean isValidRecordForSearchTitle(UInt16  selBBS, Char *bbsName,
                                           NNshSubjectDatabase  *mesInfo)
{
    // 板リストからチェック
    switch (selBBS)
    {
      case NNSH_SELBBS_FAVORITE:
        if ((mesInfo->msgAttribute & NNSH_MSGATTR_FAVOR) < (NNshGlobal->NNsiParam)->displayFavorLevel)
        {
            // 「お気に入り」ではない
            return (false);
        }
        break;

      case NNSH_SELBBS_GETALL:
        if (mesInfo->state == NNSH_SUBJSTATUS_NOT_YET)
        {
            // 取得済み全てのときは未取得のスレではない場合
            return (false);
        }
        break;

      case NNSH_SELBBS_NOTREAD:
        if ((mesInfo->state != NNSH_SUBJSTATUS_NEW)&&
            (mesInfo->state != NNSH_SUBJSTATUS_UPDATE)&&
            (mesInfo->state != NNSH_SUBJSTATUS_REMAIN))
        {
            // 未読があるスレではない
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM1:
        // 検索確認を実施するかどうかの確認(ユーザ設定１)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom1)));
        break;

      case NNSH_SELBBS_CUSTOM2:
        // 検索確認を実施するかどうかの確認(ユーザ設定２)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom2)));
        break;

      case NNSH_SELBBS_CUSTOM3:
        // 検索確認を実施するかどうかの確認(ユーザ設定３)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom3)));
        break;

      case NNSH_SELBBS_CUSTOM4:
        // 検索確認を実施するかどうかの確認(ユーザ設定４)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom4)));
        break;

      case NNSH_SELBBS_CUSTOM5:
        // 検索確認を実施するかどうかの確認(ユーザ設定５)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom5)));
        break;

      default:
        if (StrCompare(bbsName, mesInfo->boardNick) != 0)
        {
            // スレの所属が一致しない
            return (false);
        }
        break;
    }
    // 検索を実行するスレだった
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   search_NextBBS                                           */
/*                                                              板検索処理 */
/*-------------------------------------------------------------------------*/
static Boolean search_NextBBS(UInt16 start, UInt16 *bbsId)
{
    ListType        *lstP;
    Char            *ptr;
    Char             msg[BUFSIZE];
    Char             match1[BUFSIZE], match2[MAX_BOARDNAME + MARGIN];
    UInt16           selBBS, maxBBS;

    // 検索文字列が確保済みか確認する
    if (NNshGlobal->searchBBSH == 0)
    {
        return (false);
    }

    // 検索文字列が設定済みの場合には、その文字列を取得する
    ptr = MemHandleLock(NNshGlobal->searchBBSH);
    if (ptr == NULL)
    {
        return (false);
    }
    if (*ptr == '\0')
    {
        MemHandleUnlock(NNshGlobal->searchTitleH);
        return (false);
    }

    // スレ検索の実施(検索中表示)
    MemSet (msg, sizeof(msg), 0x00);
    StrCopy(msg, MSG_SEARCHING_TITLE);
    StrCat (msg, ptr);
    Show_BusyForm(msg);

    // 検索...
    if (start < NNSH_NOF_SPECIAL_BBS)
    {
        // 検索は通常板のみ。
        start = NNSH_NOF_SPECIAL_BBS;
    }

    // 板名を大小文字変換（入力文字列）
    if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
    {
        (void) StrToLower(match1, ptr);
    }
    else
    {
        (void) StrCopy(match1, ptr);
    }

    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    maxBBS  = LstGetNumberOfItems(lstP);

    // 板名を（リストの一覧から）探す
    for (selBBS = start; selBBS < maxBBS; selBBS++)
    {
        // 板名を大小文字変換（出力文字列）
        MemSet (match2, sizeof(match2), 0x00);
        if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
        {
            (void) StrToLower(match2, LstGetSelectionText(lstP, selBBS));
        }
        else
        {
            (void) StrCopy(match2, LstGetSelectionText(lstP, selBBS));
        }

        if (StrStr(match2, match1) != NULL)
        {
            // 文字列が見つかった！
            Hide_BusyForm(false);
            *bbsId = selBBS;
            MemHandleUnlock(NNshGlobal->searchBBSH);
            return (true);
        }
    }

    // 検索中表示を消去する
    Hide_BusyForm(false);

    // 検索に失敗した(板が見つからなかった)
    NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, ptr, 0);
    MemHandleUnlock(NNshGlobal->searchBBSH);

    // スレ一覧画面を再表示する
    NNsi_RedrawThreadList();

    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   search_NextTitle                                         */
/*                                                        スレタイ検索処理 */
/*-------------------------------------------------------------------------*/
static Boolean search_NextTitle(Int16 direction)
{
    Err                  ret;
    Char                 msg[BUFSIZE], match1[BUFSIZE], match2[MAX_THREADNAME];
    Char                *ptr;
    UInt16               selMES, selBBS, index, loop, end;
    Int16                step;
    DmOpenRef            dbRef;
    NNshSubjectDatabase  mesInfo;
    NNshBoardDatabase    bbsInfo;

    // スレタイ一覧がない場合は、すぐに応答
    if (NNshGlobal->nofTitleItems == 0)
    {
        // レコード情報が存在しない
        NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, MSG_NOTFOUND_OMIT, 0);
        return (false);
    }

    // 検索文字列が確保済みか確認する
    if (NNshGlobal->searchTitleH == 0)
    {
        return (false);
    }

    // 検索文字列が設定済みの場合には、その文字列を取得する
    ptr = MemHandleLock(NNshGlobal->searchTitleH);
    if (ptr == NULL)
    {
        return (false);
    }
    if (*ptr == '\0')
    {
        MemHandleUnlock(NNshGlobal->searchTitleH);
        return (false);
    }

    // 現在(今カーソルがある位置)のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    // ユーザ設定のときには、板名を別途取得する
    switch (selBBS)
    {
       case NNSH_SELBBS_CUSTOM1:
        // ユーザ設定１
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom1.boardNick, &bbsInfo);
        break;

       case NNSH_SELBBS_CUSTOM2:
        // ユーザ設定２
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom2.boardNick, &bbsInfo);
        break;

       case NNSH_SELBBS_CUSTOM3:
        // ユーザ設定３
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom3.boardNick, &bbsInfo);
        break;

       case NNSH_SELBBS_CUSTOM4:
        // ユーザ設定４
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom4.boardNick, &bbsInfo);
        break;

       case NNSH_SELBBS_CUSTOM5:
        // ユーザ設定５
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom5.boardNick, &bbsInfo);
        break;

       default:
         // ユーザ設定以外
         ret = errNone;
         break;        
    }
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    // スレ検索の実施(検索中表示)
    MemSet (msg, sizeof(msg), 0x00);
    StrCopy(msg, MSG_SEARCHING_TITLE);
    StrCat (msg, ptr);
    Show_BusyForm(msg);

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (direction == NNSH_SEARCH_FORWARD)
    {
        // フォワード検索指示
        GetDBCount_NNsh(dbRef, &end);
        end  = 0;
        step = -1;
    }
    else
    {
        // バックワード検索指示
        GetDBCount_NNsh(dbRef, &end);
        end--;
        step = 1;
    }

    // 大文字小文字を区別しない場合には、小文字に変換
    MemSet(match1, sizeof(match1), 0x00);
    if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
    {
        (void) StrToLower(match1, ptr);
    }
    else
    {
        (void) StrCopy(match1, ptr);
    }

    // 検索開始レコード番号を取得
    index = convertListIndexToMsgIndex(selMES);
    if (ret != errNone)
    {
        // レコードの取得に失敗、検索失敗を表示する
        goto FUNC_END;
    }
    // 検索開始レコード番号の"次"から検索を実施するよう調整する
    index = index + step;

    // 検索の実処理
    for (loop = index; loop != end; loop = loop + step)
    {
        // GetRecord_NNsh()内でゼロクリアしているため不要
        // MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
        GetRecord_NNsh(dbRef, loop, sizeof(NNshSubjectDatabase), &mesInfo);

        // 文字列比較する条件に合致したスレかどうかのチェック
        if (isValidRecordForSearchTitle(selBBS, bbsInfo.boardNick, &mesInfo) ==
            false)
        {
            // 比較する必要のないスレだった、次にすすむ
            continue;
        }

        //  指定された文字列とマッチするか確認
        MemSet(match2, sizeof(match2), 0x00);
        if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
        {
            // スレタイを小文字に変換
            (void) StrToLower(match2, mesInfo.threadTitle);
        }
        else
        {
            (void) StrCopy(match2, mesInfo.threadTitle);
        }

        if (StrStr(match2, match1) != NULL)
        {
            // 文字列が見つかった！
            CloseDatabase_NNsh(dbRef);
            Hide_BusyForm(false);

            // スレ一覧の表示箇所を変更
            (NNshGlobal->NNsiParam)->startTitleRec = loop;
            (NNshGlobal->NNsiParam)->endTitleRec   = loop;
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_HALFWAY;
            Update_Thread_List(selBBS, 0, NNSH_STEP_REDRAW);
            MemHandleUnlock(NNshGlobal->searchTitleH);
            return (true);
        }
    }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);

FUNC_END:
    // 検索に失敗した
    NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, ptr, 0);
    MemHandleUnlock(NNshGlobal->searchTitleH);

    // スレリストの更新(SELMES注意)
    Update_Thread_List(selBBS, (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);

    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_subject_info                                         */
/*                                                   メッセージ情報の取得  */
/*-------------------------------------------------------------------------*/
static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR)
{
    Err ret;

    if (NNshGlobal->nofTitleItems == 0)
    {
        // レコード情報が存在しない
        return (~errNone);
    }

    // メッセージ情報の初期化
    *selTHR                    = (NNshGlobal->NNsiParam)->selectedTitleItem;
    (NNshGlobal->NNsiParam)->openThreadIndex = *selTHR;

    // リスト番号からスレindexに変換する
    (NNshGlobal->NNsiParam)->openMsgIndex    = convertListIndexToMsgIndex(*selTHR);

    // スレ情報を取得する
    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, mesInfo);
    if (ret != errNone)
    {
        // レコードの取得に失敗
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        MemSet(mesInfo, sizeof(NNshSubjectDatabase), 0x00);
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_message_Info                                         */
/*                                                   メッセージ情報の取得  */
/*-------------------------------------------------------------------------*/
static Err get_message_Info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR, 
                            NNshBoardDatabase   *bbsInfo,  UInt16 *selBBS)
{
    Err        ret;
    ListType  *lstP;
    UInt16     dummy;

    *selBBS = 0;

    // スレ情報を取得する
    ret = get_subject_info(mesInfo, selTHR);
    if (ret != errNone)
    {
        return (~errNone);
    }

    // 選択されているBBSのリスト番号を取得する
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    *selBBS = LstGetSelection(lstP);

    // 選択されたBBSのリスト番号で、BBSのモードを判断する
    if (*selBBS >= NNSH_SELBBS_OFFLINE)
    {
        // 特殊BBS以外のときは、BBS情報を取得する
        ret = Get_BBS_Info(*selBBS, bbsInfo);
    }
    else
    {
        // 特殊板は、boardNickを使ってBBS情報を取得
        ret = Get_BBS_Database(mesInfo->boardNick, bbsInfo, &dummy);
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   display_message                                          */
/*                                                        メッセージの表示 */
/*-------------------------------------------------------------------------*/
static Boolean display_message(void)
{
    Err                  ret;
    UInt16               selM, butID, selBBS;
    NNshSubjectDatabase  subjInfo;
    NNshBoardDatabase    bbsInfo;

    // スレ情報を取得する
    ret = get_message_Info(&subjInfo, &selM, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // スレ未取得のとき、未取得の旨を警告し、スレを取得するか確認する。
    if (subjInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_NOT_YET_SUBJECT, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // Cancelを選択(何もしない)
            return (true);
        }
        // スレを取得する
        if (get_all_message() != errNone)
        {
            // スレ一覧画面を再表示する
            NNsi_RedrawThreadList();
            return (false);
        }
        // スレデータ取得できたときは、画面を表示に遷移する。
    }

    // BBSを(最後に)参照したところに変更する(お気に入り対応)
    (NNshGlobal->NNsiParam)->lastBBS = selBBS;

    // 参照画面をオープンする
    NNshMain_Close(FRMID_MESSAGE);

    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_all_message                                          */
/*                                                     メッセージ全部取得  */
/*-------------------------------------------------------------------------*/
static UInt16 get_all_message(void)
{
    Boolean             err = false;
    Err                 ret;
    Char                *url;
    UInt16              butID, selMES, selBBS, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // URLの取得領域を確保する
    url = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (url == NULL)
    {
        ret = ~errNone;
        goto FUNC_END;
    }
    MemSet(url, (BIGBUF + MARGIN), 0x00);

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        ret = ~errNone;
        goto FUNC_END;
    }

    // OFFLINEスレが選択された場合、更新不可を表示
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // 取得停止中ＢＢＳのスレが選択された場合、更新不可を表示
    if (((bbsInfo.state) & (NNSH_BBSSTATE_SUSPEND)) == NNSH_BBSSTATE_SUSPEND)
    {
        NNsh_InformMessage(ALTID_WARN, bbsInfo.boardName,
                           MSG_WARN_SUSPENDBBS, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // 同じスレがスレリストの上位にあるかチェックする
    if (Check_same_thread(selMES, &mesInfo, NULL, &butID) == errNone)
    {
        //  同じスレがあった場合、DBの不整合を防止するため、警告を表示して
        // データの取得を行わない。
        NNsh_InformMessage(ALTID_WARN, MSG_THREADLIST_ALREADY,
                           mesInfo.threadTitle, 0);
        NNsi_RedrawThreadList();
        ret = ~errNone;
        goto FUNC_END;
    }

    if (mesInfo.state != NNSH_SUBJSTATUS_NOT_YET)
    {
        // 既に取得済み、メッセージを最初から取得するが、本当にいいかの確認
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_GET_ALREADY, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // キャンセルする
            NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
            NNsi_RedrawThreadList();
            ret = ~errNone;
            goto FUNC_END;
        }
    }
    ////////////  メッセージ全部取得の処理  ///////////

    //  取得予約機能が有効でかつネットに接続されていない場合、
    // 取得予約するか確認する
    if (((NNshGlobal->NNsiParam)->getReserveFeature != 0)&&(NNshGlobal->netRef == 0))
    {
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                NNSH_MSGINFO_RESERVE, "", 0) == 0)
        {
            // リスト番号からスレindexに変換する
            index = convertListIndexToMsgIndex(selMES);

            // 新着予約時に取得予約を設定して終了する
            mesInfo.msgAttribute
                     = mesInfo.msgAttribute | NNSH_MSGATTR_GETRESERVE;
            (void) update_subject_database(index, &mesInfo);

            // スレリストの更新(SELMES注意)
            err = Update_Thread_List(selBBS,
                             (NNshGlobal->NNsiParam)->selectedTitleItem,
                             NNSH_STEP_REDRAW);
            ret = (errNone + 1);
            goto FUNC_END;
        }
    }

    /** (VFS上で)ディレクトリを作成する **/
    (void) CreateDir_NNsh(mesInfo.boardNick);

    // メッセージ取得先の作成
    (void) CreateThreadURL(NNSH_DISABLE, url, (BIGBUF), &bbsInfo, &mesInfo);
    
    // リスト番号からスレindexに変換する
    index = convertListIndexToMsgIndex(selMES);


    // ログ取得および記録処理
#ifdef USE_ZLIB
    NNshGlobal->getRetry = 0;
GETALL_MESSAGE:
#endif // #ifdef USE_ZLIB
    ret = Get_AllMessage(url, bbsInfo.boardNick, mesInfo.threadFileName,
                         index, (UInt16) bbsInfo.bbsType);
    if ((ret == errNone)&&((bbsInfo.state&(NNSH_BBSSTATE_LEVELMASK)) != 0))         
    {
        // 受信が正常終了し、標準スレレベルが設定されていた場合
        if (Get_Subject_Database(index, &mesInfo) == errNone)
        {
            // BBS情報に設定された標準スレレベルを反映させる
            switch ((bbsInfo.state)&(NNSH_BBSSTATE_LEVELMASK))
            {
              case NNSH_BBSSTATE_FAVOR_L1:
                // Lレベルに設定
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L1));
                break;

              case NNSH_BBSSTATE_FAVOR_L2:
                // 1レベルに設定
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L2));
                break;

              case NNSH_BBSSTATE_FAVOR_L3:
                // 2レベルに設定
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L3));
                break;

              case NNSH_BBSSTATE_FAVOR_L4:
                // 3レベルに設定
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L4));
                break;

              case NNSH_BBSSTATE_FAVOR:
                // Hレベルに設定
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR));
                break;

              default:
                // 何もしない
                break;
            }
            // スレ情報DBを更新する
            (void) update_subject_database(index, &mesInfo);
        }
    }
#ifdef USE_ZLIB
    else if (ret == NNSH_ERRORCODE_NOTACCEPTEDMSG)
    {
        if ((NNshGlobal->useGzip != 0)&&(NNshGlobal->loginDateTime != 0)&&
            (NNshGlobal->getRetry == 0))
        {
            //  ●ログイン中で、サーバからの応答メッセージが
            // 異常だったときは、過去ログ倉庫をあさる
            MemSet(url, (BIGBUF + MARGIN), 0x00);

           // メッセージ取得先の作成
           (void) CreateThreadURL(NNSH_ENABLE, url, (BIGBUF), &bbsInfo, &mesInfo);

            NNshGlobal->getRetry = 1;
            goto GETALL_MESSAGE;  // すごい無理矢理...再取得
        }
    }
    NNshGlobal->getRetry = 0;
#endif // #ifdef USE_ZLIB

    // 取得終了の表示(DEBUGオプション有効時のみ)
    NNsh_DebugMessage(ALTID_INFO, MSG_GET_MESSAGE_END,
                      mesInfo.threadTitle, ret);

    // スレリストの更新(SELMES注意)
    err = Update_Thread_List(selBBS,
                             (NNshGlobal->NNsiParam)->selectedTitleItem,
                             NNSH_STEP_REDRAW);

    // NNsi設定で、「取得後スレを表示」にチェックが入っていた場合は、画面表示
    if ((ret == errNone)&&((NNshGlobal->NNsiParam)->openAutomatic != 0))
    {
        // スレを表示する。
        display_message();
        ret = ~errNone;
    }
    if (err == false)
    {
        ret = ~errNone;
    }

FUNC_END:
    // 取得エラー時には、エラー応答する
    MEMFREE_PTR(url);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_message                                           */
/*                                                     メッセージ差分取得  */
/*-------------------------------------------------------------------------*/
static Boolean update_message(void)
{
    Boolean             err;
    Err                 ret;
    Char                url[BUFSIZE];
    UInt16              butID, selBBS, selMES, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // OFFLINEスレが選択された場合、更新不可を表示
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    // 取得停止中ＢＢＳのスレが選択された場合、更新不可を表示
    if (((bbsInfo.state) & (NNSH_BBSSTATE_SUSPEND)) == NNSH_BBSSTATE_SUSPEND)
    {
        NNsh_InformMessage(ALTID_WARN, bbsInfo.boardName,
                           MSG_WARN_SUSPENDBBS, 0);
        return (false);
    }

    //  Palm本体にログがあるが、VFSを使用する設定になっていた場合には、
    // 更新できない旨表示する
    if (((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)&&
        (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM))||
        ((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)&&
         (mesInfo.msgState == FILEMGR_STATE_OPENED_VFS)))
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE_LOG,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    //  VFSにログがあるが、VFSを使用する設定になっていない場合には、
    // 更新できない旨表示する
    if (((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)&&
        (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM))||
        ((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)&&
         (mesInfo.msgState == FILEMGR_STATE_OPENED_VFS)))
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE_LOG,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        // 差分取得を選択したが、まだ取得していない。(全取得するか確認する)
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_NOT_YET_SUBJECT, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // Cancelを選択(何もしない)
            return (true);
        }
        // スレを(全部)取得する
        if (get_all_message() != errNone)
        {
            return (false);
        }
        return (true);
    }

    ////////////  メッセージ差分取得の処理  ///////////

    //  取得予約機能が有効でかつネットに接続されていない場合、
    // 取得予約するか確認する
    if (((NNshGlobal->NNsiParam)->getReserveFeature != 0)&&(NNshGlobal->netRef == 0))
    {
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                NNSH_MSGINFO_RESERVE, "", 0) == 0)
        {
            // リスト番号からスレindexに変換する
            index = convertListIndexToMsgIndex(selMES);

            // 新着予約時に取得予約を設定して終了する
            mesInfo.msgAttribute
                     = mesInfo.msgAttribute | NNSH_MSGATTR_GETRESERVE;
            (void) update_subject_database(index, &mesInfo);

            // スレリストの更新(SELMES注意)
            err = Update_Thread_List(selBBS,
                             (NNshGlobal->NNsiParam)->selectedTitleItem,
                             NNSH_STEP_REDRAW);
            return (true);
        }
    }

    /** (VFS上の)ディレクトリを作成する **/
    (void) CreateDir_NNsh(mesInfo.boardNick);

    // メッセージ取得先の作成
    (void) CreateThreadURL(NNSH_DISABLE, url, sizeof(url), &bbsInfo, &mesInfo);

    // リスト番号からスレindexに変換する
    index = convertListIndexToMsgIndex(selMES);

    // ログの取得および記録(追記)処理
    ret = Get_PartMessage(url, bbsInfo.boardNick, &mesInfo, index, NULL);

    // 取得終了の表示(DEBUGオプション有効時のみ)
    NNsh_DebugMessage(ALTID_INFO, MSG_GET_PARTMESSAGE_END,
                      mesInfo.threadTitle, ret);

    // スレリストの更新(SELMES)
    err = Update_Thread_List(selBBS, 
                             (NNshGlobal->NNsiParam)->selectedTitleItem,
                             NNSH_STEP_REDRAW);

    // NNsi設定で、「取得後スレを表示」にチェックが入っていた場合は、画面表示
    if ((ret == errNone)&&((NNshGlobal->NNsiParam)->openAutomatic != 0))
    {
        // スレを表示する。
        display_message();
        err = true;
    }

    return (err);
}

/*-------------------------------------------------------------------------*/
/*   Function :   modify_title                                             */
/*                                                      スレタイトルの変更 */
/*-------------------------------------------------------------------------*/
static Boolean modify_title(void)
{
    Err                 ret;
    Char                newName[MAX_THREADNAME], logBuf[BUFSIZE];
    UInt16              selBBS, selTHR, butID, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // 初期値として現在のタイトルをコピーする
    MemSet(newName, sizeof(newName), 0x00);
    StrNCopy(newName, mesInfo.threadTitle, MAX_THREADNAME);

    // 文字列入力ウィンドウを開き、データを獲得
    ret = DataInputDialog(NNSH_INPUTWIN_MODIFYTITLE, newName, sizeof(newName),
                          NNSH_DIALOG_USE_MODIFYTITLE, NULL);
    if (ret == true)
    {
        MemSet(logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, MSG_DIALOG_MODIFYTITLE);
        StrCat (logBuf, mesInfo.threadTitle);
        StrCat (logBuf, MSG_DIALOG_MODIFYTITLE2);

        // 本当にスレタイトルを変更するか確認する
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, logBuf, newName, 0);
        if (butID == 0)
        {
            // タイトルを更新する
            MemSet  (mesInfo.threadTitle, MAX_THREADNAME, 0x00);
            StrNCopy(mesInfo.threadTitle, newName, MAX_THREADNAME);
            index = convertListIndexToMsgIndex(selTHR);
            ret =  update_subject_database(index, &mesInfo);
            if (ret != errNone)
            {
                NNsh_InformMessage(ALTID_WARN, "Update failure", " ret:", ret);
            }
            else
            {
                Update_Thread_List(selBBS, 
                                   (NNshGlobal->NNsiParam)->selectedTitleItem, 
                                   NNSH_STEP_REDRAW);
            }
            return (true);
        }
    }
    NNsi_RedrawThreadList();
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   launch_WebBrowser                                        */
/*                                                  WebBrowserでスレを開く */
/*-------------------------------------------------------------------------*/
static Boolean launch_WebBrowser(UInt16 webDA)
{
    Err                 ret;
    Char                logBuf[BIGBUF];
    UInt16              selBBS, selTHR;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // スレメッセージのURLを構築する
    MemSet (logBuf, sizeof(logBuf), 0x00);
    CreateThreadBrowserURL(NNSH_DISABLE, logBuf, &bbsInfo, &mesInfo);

    // WebBrowserで開くとき、最新５０だけ開くようにする
    if ((NNshGlobal->NNsiParam)->browseMesNum != 0)
    {
        switch (bbsInfo.bbsType)
        {
          case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
          case NNSH_BBSTYPE_MACHIBBS:
            StrCat(logBuf, "&LAST=");
            break;

          case NNSH_BBSTYPE_SHITARABA:
            StrCat(logBuf, "&ls=");
            break;

#if 0
          case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
          case NNSH_BBSTYPE_2ch_EUC:
          case NNSH_BBSTYPE_2ch:
          case NNSH_BBSTYPE_OTHERBBS:
          case NNSH_BBSTYPE_OTHERBBS_2:
#endif
          default:
            if ((NNshGlobal->NNsiParam)->useImodeURL != 0)
            {
                StrCat(logBuf, "/n");
                goto OPEN_WEB;
            }
            else
            {
                StrCat(logBuf, "/l");
            }
            break;
        }
        NUMCATI(logBuf, (NNshGlobal->NNsiParam)->browseMesNum);
    }
    else
    {
        // i-mode用のURLだった場合、URLを調整する
        if ((NNshGlobal->NNsiParam)->useImodeURL != 0)
        {
            switch (bbsInfo.bbsType)
            {
              case NNSH_BBSTYPE_2ch_EUC:
              case NNSH_BBSTYPE_2ch:
                StrCat(logBuf, "/i");
                break;
#if 0
              case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
              case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
              case NNSH_BBSTYPE_MACHIBBS:
              case NNSH_BBSTYPE_SHITARABA:
              case NNSH_BBSTYPE_OTHERBBS:
              case NNSH_BBSTYPE_OTHERBBS_2:
#endif
              default:
                break;
            }
        }
    }
OPEN_WEB:

    // 表示するスレURLをデバッグ表示
    NNsh_DebugMessage(ALTID_INFO, "OPEN URL :", logBuf, 0);

    // webDAで開く場合...
    if (webDA == NNSH_ENABLE)
    {
        if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
        {
            // クリップボードにURLをコピーしwebDAを起動する
            ClipboardAddItem(clipboardText, logBuf, StrLen(logBuf));
            (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
        }
        return (false);
    }

    // NetFrontを起動する(NNsi終了後に起動)
    (void) WebBrowserCommand(NNshGlobal->browserCreator,
                             0, 0, NNshGlobal->browserLaunchCode, 
                             logBuf, StrLen(logBuf), NULL);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   beam_URL                                                 */
/*                                                          スレ番号をBeam */
/*-------------------------------------------------------------------------*/
static Boolean beam_URL(void)
{
    Err                 ret;
    Char                logBuf[BUFSIZE + BUFSIZE + MARGIN];
    Char                titleBuf[MAX_THREADNAME + MARGIN], *ptr;
    UInt16              selBBS, selTHR;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    if ((mesInfo.bbsType != NNSH_BBSTYPE_2ch)||
        ((mesInfo.boardNick[0]  == '!')&&
        (mesInfo.boardNick[1]  == 'G')&&
        (mesInfo.boardNick[2]  == 'i')))
    {
        // 参照ログ、または２ちゃんタイプ以外なら、Beamしない
        //   ("現在サポートしていません" 表示を行う)
        NNsh_InformMessage(ALTID_ERROR, MSG_NOTSUPPORT_BBS, "", 0);
        return (false);
    }

    // "スレ番号"を生成する
    MemSet(logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, NNSI_NNSIEXT_THREADNUMBER_START);
    StrCat (logBuf, mesInfo.boardNick);
    StrCat (logBuf, mesInfo.threadFileName);
    ptr = logBuf;
    while ((*ptr != '\0')&&(*ptr != '.'))
    {
        ptr++;
    }
    *ptr = '\0';  // スレファイル名のdat部分を削除する
    StrCat (logBuf, NNSI_NNSIEXT_THREADNUMBER_END);

    // 送信データのタイトルを生成する
    MemSet(titleBuf, sizeof(titleBuf), 0x00);
    StrCopy(titleBuf, "  ");
    StrCat (titleBuf, mesInfo.threadTitle);

    // データをNNsiにBeamする
    SendToExternalDevice(titleBuf, logBuf, SOFT_CREATOR_ID);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkIfNewArrivalIsValid                                 */
/*                                        新着メッセージ取得可否のチェック */
/*-------------------------------------------------------------------------*/
static Err checkIfNewArrivalIsValid(UInt16  selBBS, Char *bbsName,
                                    NNshSubjectDatabase  *mesInfo)
{
    if ((mesInfo->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
    {
        // 取得予約していたスレ、メッセージ取得する
        // ...ついでにディレクトリを掘る(あんまり良くない...)
        (void) CreateDir_NNsh(bbsName);
        return (errNone);
    }

    if (mesInfo->state == NNSH_SUBJSTATUS_NOT_YET)
    {
        // メッセージ未取得なら、取得を実施しない
        return (~errNone);
    }

    if (((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)&&
        (mesInfo->msgState == FILEMGR_STATE_OPENED_STREAM))||
        ((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)&&
         (mesInfo->msgState == FILEMGR_STATE_OPENED_VFS)))
    {
        //  Palm本体にログがあるが、VFSを使用する設定になっていた場合には、
        // 取得を実施できない
        return (~errNone);
    }

    switch (mesInfo->bbsType)
    {
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
      case NNSH_BBSTYPE_SHITARABA:
        // チェックＯＫ
        if (mesInfo->maxLoc >= NNSH_MESSAGE_LIMIT)
        {
            // 取得(最大)メッセージ数に到達しているなら、取得を実施しない
            return (~errNone);
        }
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // チェックＯＫ
        if (mesInfo->maxLoc >= ((NNshGlobal->NNsiParam)->msgNumLimit - 1))
        {
            // 取得(最大)メッセージ数に到達しているなら、取得を実施しない
            return (~errNone);
        }
        break;

      case NNSH_BBSTYPE_MACHIBBS:
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        if ((NNshGlobal->NNsiParam)->enableNewArrivalHtml != 0)
        {
            // まちBBS/したらば@JBBSも新着確認する場合には、チェックOK
            break;
        }
        return (~errNone);
        break;

      default:
        // 板タイプが "２ちゃんねる" または "したらば" 以外は、実施しない
        return (~errNone);
        break;
    }

#ifdef USE_STRSTR
    if (StrCompare(mesInfo->boardNick, OFFLINE_THREAD_NICK) == 0)
#else
    if ((mesInfo->boardNick[0]  == '!')&&
        (mesInfo->boardNick[1]  == 'G')&&
        (mesInfo->boardNick[2]  == 'i'))
#endif
    {
        // 参照ログなら、取得を実施しない
        return (~errNone);
    }

    // 板別のチェック
    switch (selBBS)
    {
      case NNSH_SELBBS_FAVORITE:
        if ((mesInfo->msgAttribute & NNSH_MSGATTR_FAVOR) < (NNshGlobal->NNsiParam)->displayFavorLevel)
        {
            // 「お気に入り」ではない
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_GETALL:
        // 取得済み全てのときは全て
        break;

      case NNSH_SELBBS_NOTREAD:
        if ((mesInfo->state != NNSH_SUBJSTATUS_NEW)&&
            (mesInfo->state != NNSH_SUBJSTATUS_UPDATE)&&
            (mesInfo->state != NNSH_SUBJSTATUS_REMAIN))
        {
            // 未読があるスレではない
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM1:
        // ユーザ設定１の条件チェック
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom1)) != true)
        {
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM2:
        // ユーザ設定２の条件チェック
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom2)) != true)
        {
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM3:
        // ユーザ設定３の条件チェック
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom3)) != true)
        {
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM4:
        // ユーザ設定４の条件チェック
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom4)) != true)
        {
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM5:
        // ユーザ設定５の条件チェック
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom5)) != true)
        {
            return (~errNone);
        }
        break;

      default:
        if (StrCompare(bbsName, mesInfo->boardNick) != 0)
        {
            // スレの所属が違う
            return (~errNone);
        }
        break;
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_newarrival_message                                */
/*                                                  新着メッセージ取得処理 */
/*-------------------------------------------------------------------------*/
static Boolean update_newarrival_message(UInt16 command)
{
    Err                  ret = ~errNone;
    Boolean              penState, bbsUpdate;
    Int32                waitTime;
    UInt16               selBBS, loop, savedParam, count, x, y;
    UInt16               getBBS, dummy, keepBBS;
    Char                *url, logMsg[MINIBUF], dispMsg[MINIBUF];
    ListType            *lstP;
    ControlType         *ctlP;
    MemHandle            mesH;
    DmOpenRef            dbRef;
    NNshBoardDatabase   *bbsInfo;
    NNshSubjectDatabase *mesInfo, *mesRead;

    // 新着確認前にＤＡを実施する設定だった場合...
    if ((NNshGlobal->NNsiParam)->preOnDAnewArrival != 0)
    {
        // DAを起動する
        LaunchDA_NNsh(((NNshGlobal->NNsiParam)->prepareDAforNewArrival));
    }

    // ワーク領域の確保(3個所)
    url = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (url == NULL)
    {
        return (false);
    }
    MemSet(url, (BIGBUF + MARGIN), 0x00);

    // スレ番指定取得で取得を保留したものについて、"新着確認"する
    OpenDatabase_NNsh(DBNAME_GETRESERVE, DBVERSION_GETRESERVE, &dbRef);
    if (dbRef != 0)
    {
        // 取得保留スレのスレ番指定取得実施中は、確認メッセージを表示させない
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

        // 件数取得
        GetDBCount_NNsh(dbRef, &count);

        // 全件、スレ番指定取得を実施する
        for (loop = 0; loop < count; loop++)
        {
            // データ取得
            MemSet(url, (BIGBUF + MARGIN), 0x00);
            GetRecord_NNsh(dbRef, loop, BUFSIZE, url);

            // スレ番指定取得を実施する
            (void) GetThreadDataFromMesNum(&bbsUpdate, url, BIGBUF, &x);

            // 巡回Delayが指定されていた場合、巡回待ち時間を入れる...
            if ((NNshGlobal->NNsiParam)->roundTripDelay != 0)
            {
                waitTime = (NNshGlobal->NNsiParam)->roundTripDelay;
                waitTime = waitTime * (SysTicksPerSecond() / 1000);
                if (waitTime != 0)
                {
                    SysTaskDelay(waitTime);
                }
            }
        }

        // スレ番指定取得の取得保留ＤＢの全レコードを削除する
        while (count != 0)
        {
            (void) DeleteRecordIDX_NNsh(dbRef, (count - 1));
            count--;
        }
        CloseDatabase_NNsh(dbRef);

        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }
    if (command == NNSH_NEWARRIVAL_ONLYMESNUM)
    {
        // スレ番指定取得の取得保留のみ新着確認するモードだった、終了する
        MEMFREE_PTR(url);
        goto KICK_DA;
    }


    bbsInfo = MEMALLOC_PTR(sizeof(NNshBoardDatabase));
    if (bbsInfo == NULL)
    {
        MEMFREE_PTR(url);
        return (false);
    }
    mesInfo = MEMALLOC_PTR(sizeof(NNshSubjectDatabase));
    if (bbsInfo == NULL)
    {
        MEMFREE_PTR(url);
        MEMFREE_PTR(bbsInfo);
        return (false);
    }

    // 確保したワーク領域を初期化する
    MemSet(url,     (BIGBUF + MARGIN),            0x00);
    MemSet(bbsInfo, sizeof(NNshBoardDatabase),   0x00);
    MemSet(mesInfo, sizeof(NNshSubjectDatabase), 0x00);

    // 選択されているBBSのリスト番号を取得する
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);
    keepBBS = selBBS;
    switch (selBBS)
    {
      case NNSH_SELBBS_NOTREAD:
        // 未読にいるとき、新着確認するタブは、パラメータから取得する
        selBBS = (NNshGlobal->NNsiParam)->newArrivalNotRead;
        dummy  = 1;
        break;

      case NNSH_SELBBS_CUSTOM1:
        selBBS  = (NNshGlobal->NNsiParam)->custom1.newArrival;
        dummy   = 1;
        break;

      case NNSH_SELBBS_CUSTOM2:
        selBBS = (NNshGlobal->NNsiParam)->custom2.newArrival;
        dummy  = 1;
        break;

      case NNSH_SELBBS_CUSTOM3:
        selBBS = (NNshGlobal->NNsiParam)->custom3.newArrival;
        dummy  = 1;
        break;

      case NNSH_SELBBS_CUSTOM4:
        selBBS = (NNshGlobal->NNsiParam)->custom4.newArrival;
        dummy  = 1;
        break;

      case NNSH_SELBBS_CUSTOM5:
        selBBS = (NNshGlobal->NNsiParam)->custom5.newArrival;
        dummy  = 1;
        break;

      default:
        dummy   = 0;
        break;
    }
    if (dummy != 0)
    {
        // BBS表示タブを(無理やり)変更する
        LstSetSelection(lstP, selBBS);

        // BBS表示タブを変更する
        (NNshGlobal->NNsiParam)->lastBBS = selBBS;

        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO);
        CtlSetLabel(ctlP, LstGetSelectionText(lstP, selBBS));
    }

    // BBS情報の取得先タブを選択
    switch (selBBS)
    {
      case NNSH_SELBBS_CUSTOM1:
        // ユーザ設定１の板情報を取得
        getBBS = (NNshGlobal->NNsiParam)->custom1.boardNick;
        break;

      case NNSH_SELBBS_CUSTOM2:
        // ユーザ設定２の板情報を取得
        getBBS = (NNshGlobal->NNsiParam)->custom2.boardNick;
        break;

      case NNSH_SELBBS_CUSTOM3:
        // ユーザ設定３の板情報を取得
        getBBS = (NNshGlobal->NNsiParam)->custom3.boardNick;
        break;

      case NNSH_SELBBS_CUSTOM4:
        // ユーザ設定４の板情報を取得
        getBBS = (NNshGlobal->NNsiParam)->custom4.boardNick;
        break;

      case NNSH_SELBBS_CUSTOM5:
        // ユーザ設定５の板情報を取得
        getBBS = (NNshGlobal->NNsiParam)->custom5.boardNick;
        break;

      case NNSH_SELBBS_OFFLINE:
        // OFFLINEスレが選択されていた場合は、参照ログ取得機能を実施する
        ExecuteAction(MULTIBTN_FEATURE_LOGCHARGE);
        goto FUNC_END;
        break;

      case NNSH_SELBBS_GETALL:
        // 「取得済み全て」が選択されていたときでかつ、
        // 「取得済み全てには参照ログなし」にチェックが入っていないときは、
        // 参照ログ取得機能を実行する
        if ((NNshGlobal->NNsiParam)->notListReadOnly == 0)
        {
            ExecuteAction(MULTIBTN_FEATURE_LOGCHARGE);
        }
        getBBS = selBBS;
        break;

      case NNSH_SELBBS_FAVORITE:
      case NNSH_SELBBS_NOTREAD:
      default:
        getBBS = selBBS;
        break;
    }

    // 通常板の場合には、板情報を取得する(特殊板の場合は逐次取得する)
    if (getBBS > NNSH_SELBBS_OFFLINE)
    {
        ret = Get_BBS_Info(getBBS, bbsInfo);
        if (ret != errNone)
        {
            // BBS情報に取得失敗、終了する
            NNsh_DebugMessage(ALTID_ERROR, MSG_CANNOT_UPDATE, "(BBS data)", 0);
            goto FUNC_END;
        }
    }

    /////////////////  メッセージ差分取得の処理  ////////////////

    // "スレ検索中"を表示する
    Show_BusyForm(MSG_SEARCHING_TITLE);

    // 新着メッセージ確認処理中は、確認メッセージを「表示しない」設定にする。
    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

    // 全スレ数を取得する
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &count);

    // BBS取得更新フラグをＯＦＦにする
    bbsUpdate = false;

    // 新着確認をする
    for (loop = count; loop > 0; loop--)
    {
        // およその検索残り件数を表示する
        if ((loop % 10) == 0)
        {
            StrCopy(logMsg, MSG_SEARCHING_TITLE MSG_SEARCHING_LEFT);
            NUMCATI(logMsg, loop);
            SetMsg_BusyForm(logMsg);
        }

        // スレデータを取得する
        GetRecordReadOnly_NNsh(dbRef, (loop - 1), &mesH, (void **) &mesRead);

        // 取得予約スレ、タブがお気に入り/取得済み全て/未読かどうかのチェック
        if (((mesRead->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)||
            (getBBS < NNSH_SELBBS_OFFLINE))
        {
            //  チェックに該当した場合には、BBS情報を取得する。ただし、取得済み
            // BBS情報のboardNickと今回取得しようとするBBS情報のboardNickが一致
            // した場合には、BBS情報はすでに取得済みとしてその情報を利用する
            if (StrCompare(mesRead->boardNick, bbsInfo->boardNick) != 0)
            {
                ret = Get_BBS_Database(mesRead->boardNick, bbsInfo, &dummy);
                if (ret != errNone)
                {
                    // BBS情報に取得失敗、中断チェックへすすむ
                    ReleaseRecordReadOnly_NNsh(dbRef, mesH);
                    goto NEXT_STEP;
                }
                bbsUpdate = true;  // BBS情報を更新した、とフラグを立てる
            }
        }

        // 新着メッセージを取得するスレかどうかチェックする
        if (checkIfNewArrivalIsValid(selBBS,bbsInfo->boardNick,mesRead) != 
                                                                       errNone)
        {
            // 新着確認を実施するスレではなかった、中断チェックへすすむ
            ReleaseRecordReadOnly_NNsh(dbRef, mesH);
            goto NEXT_STEP;
        }

        // 取得停止中ＢＢＳのスレが選択された場合、更新不可(次へ進む)
        if (((bbsInfo->state) & (NNSH_BBSSTATE_SUSPEND)) == NNSH_BBSSTATE_SUSPEND)
        {
            // 中断チェックへすすむ
            ReleaseRecordReadOnly_NNsh(dbRef, mesH);
            goto NEXT_STEP;
        }

        // データベースを一度閉じる
        ReleaseRecordReadOnly_NNsh(dbRef, mesH);
        CloseDatabase_NNsh(dbRef);
        Get_Subject_Database((loop - 1), mesInfo);

        // メッセージ取得先URLの作成
        (void) CreateThreadURL(NNSH_DISABLE, url, BIGBUF, bbsInfo, mesInfo);

        // 表示するメッセージの追加
        MemSet (dispMsg, MINIBUF, 0x00);
        StrCopy(dispMsg, " (");
        NUMCATI(dispMsg, (count - loop) + 1);
        StrCat (dispMsg, "/");
        NUMCATI(dispMsg, count);
        StrCat (dispMsg, ")");

        // 一時的にBUSYウィンドウを消去する
        Hide_BusyForm(false);

        // ログの取得および記録(追記)処理
        ret = Get_PartMessage(url, bbsInfo->boardNick, mesInfo,
                              (loop - 1), dispMsg);

        // 新着確認で差分取得が失敗した場合
        if ((ret == NNSH_ERRORCODE_PARTGET)&&
            ((NNshGlobal->NNsiParam)->autoUpdateGetError != 0))
        {
            // メッセージを読んだ場所
            x = mesInfo->maxLoc;
            y = mesInfo->currentLoc;

            // 最初から全て取得しなおす
#ifdef USE_ZLIB
            NNshGlobal->getRetry = 0;
GET_ALLMESSAGE:
#endif // #ifdef USE_ZLIB
            ret = Get_AllMessage(url, bbsInfo->boardNick,
                                 mesInfo->threadFileName,
                                 (loop - 1), (UInt16) bbsInfo->bbsType);
            if (ret == errNone)         
            {
                // 受信が正常終了のとき、読み出した場所を以前の場所に復旧
                if (Get_Subject_Database((loop - 1), mesInfo) == errNone)
                {
                    mesInfo->maxLoc     = x;
                    mesInfo->currentLoc = y;
                    (void) update_subject_database((loop - 1), mesInfo);
                }
            }
#ifdef USE_ZLIB
            else if ((NNshGlobal->useGzip != 0)&&
                      (NNshGlobal->loginDateTime != 0)&&
                      (ret == NNSH_ERRORCODE_NOTACCEPTEDMSG)&&
                      (NNshGlobal->getRetry == 0))
            {
                //  ●ログイン中で、サーバからの応答メッセージが
                // 異常だったときは、過去ログ倉庫をあさる
                MemSet(url, (BIGBUF + MARGIN), 0x00);

                // メッセージ取得先URLの作成
                (void) CreateThreadURL(NNSH_ENABLE, url, BIGBUF, bbsInfo, mesInfo);
                
                NNshGlobal->getRetry = 1;
                goto GET_ALLMESSAGE;  // すごい無理矢理...
            }
            NNshGlobal->getRetry = 0;
#endif // #ifdef USE_ZLIB
        }

        // BUSYウィンドウを復活させる
        Show_BusyForm(MSG_SEARCHING_TITLE);

        // データベースを再度開く
        OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);

        //  ログ取得処理が中止された or 致命的エラー発生時には
        // 新着メッセージの確認を中止する。
        if (ret == ~errNone)
        {
            // 確認メッセージ省略のパラメータを元に戻す
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

            // 処理を中止する旨、画面表示する
            NNsh_InformMessage(ALTID_INFO, 
                               MSG_ABORT_MESSAGE, 
                               MSG_NOTCHECK_RECORDS, loop);
            break;
        }

        //  BBS情報を(取得予約スレのために)更新していた場合
        if (bbsUpdate == true)
        {
            if (getBBS > NNSH_SELBBS_OFFLINE)
            {
                // BBS情報を再取得して元のBBS情報に戻す
                // (特殊板はループ中毎回取得するので更新しない)
                (void) Get_BBS_Info(getBBS, bbsInfo);
                bbsUpdate = false;
            }
        }

        // 巡回Delayが指定されていた場合、待ち時間を入れる...
        if ((NNshGlobal->NNsiParam)->roundTripDelay != 0)
        {
            // 待機中を表示する...
            SetMsg_BusyForm(MSG_INFO_NEWTHREADWAIT);

            waitTime = SysTicksPerSecond();
            waitTime = (NNshGlobal->NNsiParam)->roundTripDelay * waitTime;
            waitTime = waitTime / 1000;
            if (waitTime != 0)
            {
                SysTaskDelay(waitTime);
            }

            // 検索中に復帰...
            SetMsg_BusyForm(MSG_SEARCHING_TITLE);
        }

NEXT_STEP:
        // ペンの状態を拾う(画面タップされているか確認する)
        EvtGetPen(&x, &y, &penState);
        if (penState == true)
        {
            // ペンがダウンされていたら、中止するか確認を行う
            if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                  MSG_CONFIRM_ABORT_GET, 
                                  MSG_NOTCHECK_RECORDS, loop) == 0)
            {
                // OKが押された、breakする
                break;
            }
        }
    }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);

    // 確認メッセージ省略のパラメータを元に戻す
    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

    // 「新着確認」後に未読一覧を表示する
    if ((NNshGlobal->NNsiParam)->autoOpenNotRead != 0)
    {
        // 選択されているBBSのリスト番号を変更する
        keepBBS = NNSH_SELBBS_NOTREAD;
    }

    // BBS表示タブを変更する
    (NNshGlobal->NNsiParam)->lastBBS = keepBBS;
    NNshWinSetPopItems(FrmGetActiveForm(),POPTRID_BBSINFO, LSTID_BBSINFO, keepBBS);

FUNC_END:
    // 関数の終了
    MEMFREE_PTR(url);
    MEMFREE_PTR(bbsInfo);
    MEMFREE_PTR(mesInfo);

KICK_DA:
    if ((ret != ~errNone)&&((NNshGlobal->NNsiParam)->use_DAplugin != 0))
    {
        // DAのプラグイン(cMDA)を起動する
        (void) LaunchResource_NNsh('DAcc','cMDA','code',1000);
    }

    if ((ret != ~errNone)&&((NNshGlobal->NNsiParam)->startMacroArrivalEnd != 0))
    {
#ifdef USE_MACRO
        // 新着確認後にマクロを実行する
        NNsi_ExecuteMacroMain(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);
#endif
    }

    if ((NNshGlobal->NNsiParam)->disconnArrivalEnd != 0)
    {
        // 「新着確認」終了時に回線切断
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        NNshNet_LineHangup();
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }

    // リスト情報を更新する
    NNsi_FrmDrawForm(FrmGetActiveForm(), true);
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);

    // 「新着確認」後にビープ音を鳴らしてみる
    if ((NNshGlobal->NNsiParam)->autoBeep != 0)
    {
        NNsh_BeepSound(sndAlarm);
    }

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_getall_message                                    */
/*                                     メッセージ取得処理("更新" ボタン用) */
/*-------------------------------------------------------------------------*/
static Boolean update_getall_message(void)
{
    Err                 ret;
    UInt16              selMES, selBBS;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // メッセージ状態を取得する
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // メッセージ状態によって、アクションを変更する
    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        // 未取得なら全部取得
        (void) get_all_message();
        goto RETR;
    }

    // 取得済みなら差分取得
    (void) update_message();
RETR:
    // スレ一覧画面を再表示する
    NNsi_RedrawThreadList();
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   delete_notreceived_message                               */
/*                                                    未受信スレ名削除処理 */
/*-------------------------------------------------------------------------*/
Boolean delete_notreceived_message(Boolean update)
{
    Err                   ret;
    UInt16                selMES, selBBS, cnt;
    NNshSubjectDatabase   mesInfo;

    // BBS番号を取得する
    selBBS = (NNshGlobal->NNsiParam)->lastBBS;

    //  「未読あり」「取得済み全て」タブ表示中に未取得一覧削除された場合は、
    // 全未取得スレ(タイトル)を削除する
    if ((selBBS == NNSH_SELBBS_GETALL)||(selBBS == NNSH_SELBBS_NOTREAD))
    {
        (void) DeleteSubjectList(" ", NNSH_SUBJSTATUS_UNKNOWN, &cnt);
        // リスト情報を更新する(selBBS)
        return (Update_Thread_List(selBBS, 0, NNSH_STEP_UPDATE));
        // return (true);
    }

    // 特殊板から未取得一覧は削除できない。
    if (selBBS <  NNSH_SELBBS_OFFLINE)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOTDELETE_FAVOR,"", 0);
        return (false);    
    }

    // スレ情報の取得
    ret = get_subject_info(&mesInfo, &selMES);
    if (ret != errNone)
    {
        return (false);
    }

    // 未取得のスレを削除する
    (void) DeleteSubjectList(mesInfo.boardNick, NNSH_SUBJSTATUS_NOT_YET, &cnt);

    // スレ数が現在の位置より小さい場合には、末尾のスレにカーソルをあわせる
    if (selMES >= cnt)
    {
        selMES = cnt - 1;    
    }

    if (update == false)
    {
        // 画面表示を更新しない場合
        return (true);
    }

    // リスト情報を更新する(selBBS)
    return (Update_Thread_List(selBBS, 0, NNSH_STEP_UPDATE));
}


/*-------------------------------------------------------------------------*/
/*   Function :   newThread_message                                        */
/*                                                           スレ立て処理  */
/*-------------------------------------------------------------------------*/
static Boolean newThread_message(void)
{
#ifdef USE_NEWTHREAD_FEATURE
    Err                ret;
    UInt16             selBBS, btnID;
    NNshBoardDatabase  bbsData;

    // スレ立てする板名を取得する
    selBBS = (NNshGlobal->NNsiParam)->lastBBS;

    // 特殊板から未取得一覧は削除できない
    if (selBBS <=  NNSH_SELBBS_OFFLINE)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_NOTAVAIL_BBS, "", 0);
        return (false);    
    }

    // BBS情報を取得する
    MemSet(&bbsData, sizeof(bbsData), 0x00);
    ret = Get_BBS_Info(selBBS, &bbsData);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, MSG_NOTAVAIL_BBS, "(Get_BBS_Info())", ret);
        return (false);
    }

    // BBSタイプが２ちゃんねる以外のときは、スレ立てしない
    if (bbsData.bbsType != NNSH_BBSTYPE_2ch)
    {
        //   「現在無効なフィーチャー」を表示する
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "(BBS TYPE)", 0);
        return (false);
    }

    // スレを本当に立てるか確認する
    btnID = NNsh_ErrorMessage(ALTID_CONFIRM, bbsData.boardName, MSG_NEWTHREAD_CONFIRM, 0);
    if (btnID != 0)
    {
        // スレ立てキャンセル
        return (false);
    }

    // スレ立て画面を開く
    NNshMain_Close(FRMID_NEW_THREAD);
    
#else
    // スレ立て処理を実行しない場合...
    //   「現在無効なフィーチャー」を表示する
    NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif     
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   delete_message                                           */
/*                                                           スレ削除処理  */
/*-------------------------------------------------------------------------*/
Boolean delete_message(void)
{
    Err                 ret;
    UInt16              selMES, selBBS, index;
    NNshSubjectDatabase mesInfo;
    ListType           *lstP;

    // 選択されているBBSのリスト番号を取得する
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);

    // メッセージ情報を取得する
    ret = get_subject_info(&mesInfo, &selMES);
    if (ret != errNone)
    {
        return (false);
    }

    // スレを本当に削除するか確認する
    index = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_DELETE_MSGCONFIRM, 
                                mesInfo.threadTitle, 0);
    if (index != 0)
    {
        // 削除キャンセル
        return (false);
    }

    // リスト番号からスレindexに変換する
    index = convertListIndexToMsgIndex(selMES);

    // スレ削除メイン処理
    DeleteThreadMessage(index);

    // 一覧画面を再描画する
    return (Update_Thread_List(selBBS, selMES, NNSH_STEP_REDRAW));
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_subject_txt                                          */
/*                                      スレ名一覧(subject.txt)を取得する  */
/*-------------------------------------------------------------------------*/
static Boolean get_subject_txt(void)
{
    Err          ret;
    Boolean      penState;
    UInt16       selBBS, keepBBS, loop, x, y;
    ListType    *lstP;
    ControlType *ctlP;

    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);

    // ユーザ設定タブのときは、指定されたタブの一覧を取得する
    switch (selBBS)
    {
      case NNSH_SELBBS_CUSTOM1:
        selBBS  = (NNshGlobal->NNsiParam)->custom1.getList;
        keepBBS = NNSH_SELBBS_CUSTOM1;
        break;

      case NNSH_SELBBS_CUSTOM2:
        selBBS  = (NNshGlobal->NNsiParam)->custom2.getList;
        keepBBS = NNSH_SELBBS_CUSTOM2;
        break;
 
      case NNSH_SELBBS_CUSTOM3:
        selBBS  = (NNshGlobal->NNsiParam)->custom3.getList;
        keepBBS = NNSH_SELBBS_CUSTOM3;
        break;
        
      case NNSH_SELBBS_CUSTOM4:
        selBBS  = (NNshGlobal->NNsiParam)->custom4.getList;
        keepBBS = NNSH_SELBBS_CUSTOM4;
        break;

      case NNSH_SELBBS_CUSTOM5:
        selBBS  = (NNshGlobal->NNsiParam)->custom5.getList;
        keepBBS = NNSH_SELBBS_CUSTOM5;
        break;

      default:
        keepBBS = 0;
        break;
    }

    if (keepBBS != 0)
    {
        // BBS表示タブを(無理やり)変更する
        LstSetSelection(lstP, selBBS);

        // BBS表示タブを変更する
        (NNshGlobal->NNsiParam)->lastBBS = selBBS;
        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO);
        CtlSetLabel(ctlP, LstGetSelectionText(lstP, selBBS));
    }

    switch (selBBS)
    {
      case NNSH_SELBBS_NOTREAD:
      case NNSH_SELBBS_FAVORITE:
      case NNSH_SELBBS_CUSTOM1:
      case NNSH_SELBBS_CUSTOM2:
      case NNSH_SELBBS_CUSTOM3:
      case NNSH_SELBBS_CUSTOM4:
      case NNSH_SELBBS_CUSTOM5:
        // "ユーザ設定タブ"、"お気に入り"、"未読あり"表示中には、受け付けない。
        ret = ~errNone;
        break;
      case NNSH_SELBBS_GETALL:
        // "取得済み全て"の時には、全スレの一覧を取得する
        for (loop = (NNshGlobal->useBBS - 1); loop > NNSH_SELBBS_OFFLINE; loop--)
        {
            // スレ一覧を取得する
            (void) NNsh_GetSubjectList(loop);

            // ペンの状態を拾う(画面タップされているか確認する)
            EvtGetPen(&x, &y, &penState);
            if (penState == true)
            {
                // ペンがダウンされていたら、中止するか確認を行う
                if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                      MSG_CONFIRM_ABORT_LIST, 
                                      MSG_NOTCHECK_RECORDS,
                                      (loop - NNSH_SELBBS_OFFLINE)) == 0)
                {
                    // OKが押された、breakする
                    break;
                }
            }
        }
        ret = errNone;
        break;

      case NNSH_SELBBS_OFFLINE:
        // オフラインスレの検索を行う
        create_offline_database();
        ret = errNone;
        break;

      default:
        // スレ一覧を取得する
        ret = NNsh_GetSubjectList(selBBS);
        break;
    }
    if (keepBBS != 0)
    {
        // BBS表示タブを(無理やり)元に戻す
        LstSetSelection(lstP, keepBBS);

        // BBS表示タブを変更する
        (NNshGlobal->NNsiParam)->lastBBS = keepBBS;
        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO);
        CtlSetLabel(ctlP, LstGetSelectionText(lstP, keepBBS));
    }

    // スレ一覧取得時に新着確認も同時に実施する場合
    if ((NNshGlobal->NNsiParam)->listAndUpdate != 0)
    {
        // 取得保留中のスレ番指定取得を実施する
        (void) update_newarrival_message(NNSH_NEWARRIVAL_ONLYMESNUM);
    }

    // スレ一覧を表示しなおす
    if (ret == errNone)
    {
        FrmDrawForm(FrmGetActiveForm());
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        return (true);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   popEvt_ThreadMain                                        */
/*                                          リスト選択イベント処理ルーチン */
/*-------------------------------------------------------------------------*/
static Boolean popEvt_ThreadMain(EventType *event)
{
    UInt16 savedParam;
    switch (event->data.popSelect.listID)
    {
      case LSTID_BBSINFO:
        // 絞込み検索モードをクリアする...
        (NNshGlobal->NNsiParam)->searchPickupMode = 0;

        if ((NNshGlobal->NNsiParam)->autoDeleteNotYet != 0)
        {
            // 板移動時に未取得スレを削除する
            savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            delete_notreceived_message(false);
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
        }

        // LSTID_BBSINFOのpopSelectEventだったなら、スレ情報を更新する
        (NNshGlobal->NNsiParam)->lastBBS = event->data.popSelect.selection;
        (void) Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   rctPointEvt_ThreadMain                                   */
/*                                              スレをタップしたときの処理 */
/*-------------------------------------------------------------------------*/
static Boolean rctPointEvt_ThreadMain(EventType *event,
                                      FormType *frm, RectangleType *rect)
{
    Coord  locY;
    UInt16 fontID, nlines, fontHeight, selItem;

    // 選択したアイテムを特定する
    if ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0)
    {
        fontID = (NNshGlobal->NNsiParam)->currentFont;
    }
    else
    {
        fontID = (NNshGlobal->NNsiParam)->sonyHRFontTitle;
    }
    NNsi_UpdateRectangle(rect, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                         &fontID, &fontHeight, &nlines);

#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        locY = (event->screenY * 2) - rect->topLeft.y;
    }
    else
#endif
    {
#ifdef USE_HIGHDENSITY
        // 解像度にあわせて描画領域の高さを取得
        switch (NNshGlobal->os5HighDensity)
        {
          case kDensityDouble:
            locY = (event->screenY * 2) - rect->topLeft.y;
            break;

          case kDensityTriple:
            locY = (event->screenY * 3) - rect->topLeft.y;
            break;

          case kDensityQuadruple:
            locY = (event->screenY * 4) - rect->topLeft.y;
            break;

          case kDensityOneAndAHalf:
            locY = (event->screenY * 3 / 2) - rect->topLeft.y;
            break;

          case kDensityLow:
          default:
            locY = event->screenY - rect->topLeft.y;
            break;
        }
#else
        locY = event->screenY - rect->topLeft.y;
#endif // #ifdef USE_HIGHDENSITY
    }
    selItem = locY / fontHeight;

    // 範囲外の選択だった場合、何もしない
    if (selItem >= NNshGlobal->nofTitleItems)
    {
        return (true);
    }

    if (selItem == (NNshGlobal->NNsiParam)->selectedTitleItem)
    {
        // ダブルタップを検出、スレを開こうとする
        return (display_message());
    }

    // 選択アイテムを移動させる
    NNsi_WinUpdateList(selItem,
                       (NNshGlobal->NNsiParam)->selectedTitleItem, 
                       NNshGlobal->msgTitleIndex,
                       nlines, rect, fontHeight);
    (NNshGlobal->NNsiParam)->selectedTitleItem = selItem;
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   keyDownEvt_ThreadMain                                    */
/*                                                        キー入力時の処理 */
/*-------------------------------------------------------------------------*/
static Boolean keyDownEvt_ThreadMain(EventType *event)
{
    Boolean      ret = false;
    UInt16       keyCode;
    ControlType *ctlP;


    // 5way navigator用キーコードコンバート
    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      case vchrHard1:
      case vchrTapWaveSpecificKeyULeft:
        // ハードキーを制御する場合は、、、
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // ハードキー１を押したときの設定
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.key1)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

      case vchrHard2:
      case vchrTapWaveSpecificKeyURight:
        // ハードキーを制御する場合は、、、
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // ハードキー２を押したときの設定
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.key2)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

      case vchrHard3:
        // ハードキーを制御する場合は、、、
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // ハードキー３を押したときの設定
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.key3)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

      case vchrHard4:
        // ハードキーを制御する場合は、、、
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // ハードキー４を押したときの設定
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.key4)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

#if 0
      case vchrCapture:
      case vchrVoiceRec:
      case vchrVZ90functionMenu:      
      case vchrGarminRecord:
#endif
      case vchrHard5:
        // CLIEキャプチャーボタンを制御する場合は、、、
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // CLIEキャプチャーボタンを押したときの設定
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.clieCapture)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

      // ハードキー左を押した時の処理
      case chrLeftArrow:         // カーソルキー左(5way左)
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.left)&(MULTIBTN_FEATURE_MASK)));
        break;

      // ハードキー右を押した時の処理
      case chrRightArrow:         // カーソルキー右(5way右)
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.right)&(MULTIBTN_FEATURE_MASK)));
        break;

      // ハードキー下を押した時の処理
      case vchrPageDown:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.down)&(MULTIBTN_FEATURE_MASK)));
        break;

      // ハードキー上を押した時の処理
      case vchrPageUp:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.up)&(MULTIBTN_FEATURE_MASK)));
        break;

      // 「下」(ジョグダイヤル下)を押した時の処理
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrNextField:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case chrDownArrow:
        ret = ExecuteAction(MULTIBTN_FEATURE_SELECTNEXT);
        break;

      // 「上」(ジョグダイヤル上)を押した時の処理
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrPrevField:
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case chrUpArrow:
        ret = ExecuteAction(MULTIBTN_FEATURE_SELECTPREV);
        break;

      case chrCapital_U:  // Uキーの入力
      case chrSmall_U:
        // スレの整列を実行
        (void) ExecuteAction(MULTIBTN_FEATURE_SORT);
        break;

      case chrCapital_T:  // Tキーの入力
      case chrSmall_T:
        // リストの先頭から表示する
        (void) ExecuteAction(MULTIBTN_FEATURE_TOP);
        break;

      case chrCapital_B:    // Bキーの入力
      case chrSmall_B:
        // スレタイトルの末尾へ移動
        (void) ExecuteAction(MULTIBTN_FEATURE_BOTTOM);
        break;

      case chrCapital_K:    // Kキーの入力
      case chrSmall_K:
        // スレの削除
        (void) ExecuteAction(MULTIBTN_FEATURE_DELETE);
        break;

      case chrCapital_F:    // Fキーの入力
      case chrSmall_F:
        // スレタイトル検索
        (void) ExecuteAction(MULTIBTN_FEATURE_SEARCH);
        break;

      case chrCapital_E:    // Eキーの入力
      case chrSmall_E:
        // スレの未取得一覧削除
        (void) ExecuteAction(MULTIBTN_FEATURE_DELNOTGET);
        break;

      case chrCapital_D:    // Dキーの入力
      case chrSmall_D:
        // 描画モード変更
        (void) ExecuteAction(MULTIBTN_FEATURE_GRAPHMODE);
        break;

      // ジョグダイヤル押し回し(上)の処理(1ページ上)
      case vchrJogPushedUp:
        // 一時状態を(回転中に)設定する
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGUP;
#endif
        /** not break!!  **/
      case chrCapital_P:         // Pキーの入力
      case chrSmall_P:           // pキーの入力
        // 前ページの表示 
        (void) ExecuteAction(MULTIBTN_FEATURE_PREVPAGE);
        break;

      // ジョグダイヤル押し回し(下)の処理(1ページ下)
      case vchrJogPushedDown:
        // 一時状態を(回転中に)設定する
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGDOWN;
#endif
        /** not break!!  **/

      case chrCapital_N:      // Nキーの入力
      case chrSmall_N:        // nキーの入力
        // 次ページの表示 
        (void) ExecuteAction(MULTIBTN_FEATURE_NEXTPAGE);
        break;

      // JOG Push選択時の処理(リスト選択)
      case vchrJogRelease:
#ifdef USE_CLIE
        if (NNshGlobal->tempStatus != NNSH_TEMPTYPE_CLEARSTATE)
        {
            // 一時状態をクリアする(押しまわししたとき)
            NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
            break;
        }
#endif
        /*****  not break!! (リスト選択を実施する)  *****/

      // HandEra JOG Push選択/Enterキー入力時の処理(リスト選択)
      case chrCarriageReturn:
      case chrLineFeed:
      case vchrRockerCenter:
      case vchrHardRockerCenter:
      case vchrThumbWheelPush:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.jogPush)&(MULTIBTN_FEATURE_MASK)));
        break;

      // バックボタン/ESCキー/BSキーの処理
      case vchrJogBack:
      case chrBackspace:
      case chrEscape:
      case vchrThumbWheelBack:
      case vchrGarminEscape:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.jogBack)&(MULTIBTN_FEATURE_MASK)));
        break;

      // スペースキー入力
      case chrSpace:
      case chrSJISIdeographicSpace:
        // 新着MSG確認
        return (ExecuteAction(MULTIBTN_FEATURE_NEWARRIVAL));
        break;

      case chrCapital_L:
      case chrSmall_L:
        // 一覧取得
        return (ExecuteAction(MULTIBTN_FEATURE_GETLIST));
        break;

      case chrCapital_A:
      case chrSmall_A:
        // A (ネットワーク設定を開く)
        return (ExecuteAction(MULTIBTN_FEATURE_NETWORK));
        break;

      case chrCapital_J:
      case chrSmall_J:
        // スレ番指定のメッセージ取得
        return (ExecuteAction(MULTIBTN_FEATURE_MESNUM));
        break;

      // @キー入力
      case chrCommercialAt:
        // BBS一覧取得
        return (ExecuteAction(MULTIBTN_FEATURE_GETBBS));
        break;

     case chrHyphenMinus:
        // ハイフンキー(BBS一覧を選択(ポップアップ))
        return (ExecuteAction(MULTIBTN_FEATURE_SELECT_BBS));
        break;

      // Qキー入力
      case chrCapital_Q:
      case chrSmall_Q:
        // 回線切断
        return (ExecuteAction(MULTIBTN_FEATURE_DISCONN));
        break;

      // Cキー入力
      case chrCapital_C:
      case chrSmall_C:
        // デバイス情報
        return (ExecuteAction(MULTIBTN_FEATURE_DEVICEINFO));
        break;

      // Oキー入力
      case chrCapital_O:
      case chrSmall_O:
        //URL取得
        return (ExecuteAction(MULTIBTN_FEATURE_OPENURL));
        break;

      // Gキー入力
      case chrCapital_G:
      case chrSmall_G:
        // ディレクトリ選択(chdir)
        return (ExecuteAction(MULTIBTN_FEATURE_DIRSELECT));
        break;

      case chrDigitNine:
        // ９キー（多目的スイッチ１の設定更新）
        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(CHKID_SELECT_GETMODE);
        keyCode = CtlGetValue(ctlP);
        keyCode = (keyCode == 0) ? 1 : 0;
        CtlSetValue(ctlP, keyCode);
        CtlHitControl(ctlP);
        break;

      case chrDigitZero:
        // ０キー（多目的スイッチ２の設定更新）
        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(CHKID_SELECT_MULTIPURPOSE);
        keyCode = CtlGetValue(ctlP);
        keyCode = (keyCode == 0) ? 1 : 0;
        CtlSetValue(ctlP, keyCode);
        CtlHitControl(ctlP);
        break;

      case chrDigitOne:
        // 多目的ボタン１
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn1Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitTwo:
        // 多目的ボタン２
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn2Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitThree:
        // 多目的ボタン３
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn3Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitFour:
        // 多目的ボタン４
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn4Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitFive:
        // 多目的ボタン５
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn5Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitSix:
        // 多目的ボタン６
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn6Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrComma:
        // カンマキー（'未読あり'へ移動）
        return (ExecuteAction(MULTIBTN_FEATURE_TO_NOTREAD));
        break;

      case chrFullStop:
        // ピリオドキー（'取得済み全て'へ移動）
        return (ExecuteAction(MULTIBTN_FEATURE_TO_GETALL));
        break;

      case chrColon:
        // コロンキー（'参照ログ'へ移動）
        return (ExecuteAction(MULTIBTN_FEATURE_TO_OFFLINE));
        break;

      case chrSemicolon:
        // セミコロンキー（'お気に入り'へ移動）
        return (ExecuteAction(MULTIBTN_FEATURE_TO_FAVOR));
        break;

      case chrDigitSeven:
        // キー7 (Bt ON)
        return (ExecuteAction(MULTIBTN_FEATURE_BT_ON));
        break;

      case chrDigitEight:
        // キー8 (Bt Off)
        return (ExecuteAction(MULTIBTN_FEATURE_BT_OFF));
        break;

      case chrExclamationMark:
        // NNsi設定-1
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET1));
        break;

      case chrQuotationMark:
        // NNsi設定-2
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET2));
        break;

      case chrNumberSign:
        // NNsi設定-3
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET3));
        break;

      case chrDollarSign:
        // NNsi設定-4
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET4));
        break;

      case chrPercentSign:
        // NNsi設定-5
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET5));
        break;

      case chrAmpersand:
        // NNsi設定-6
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET6));
        break;

      case chrApostrophe:
        // NNsi設定-7
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET7));
        break;

      case chrLeftParenthesis:
        // NNsi設定-8
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET8));
        break;

      case chrRightParenthesis:
        // NNsi設定-9
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET9));
        break;

      case chrQuestionMark:
        // NNsi設定画面を開く
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET));
        break;

      case chrEqualsSign:
        // ユーザ設定１を開く
        return (ExecuteAction(MULTIBTN_FEATURE_USER1TAB));
        break;

      case chrCapital_S:
      case chrSmall_S:
        // マクロ実行
        return (ExecuteAction(MULTIBTN_FEATURE_MACRO_EXEC));
        break;

      case chrCapital_I:
      case chrSmall_I:
        // スレ情報
        return (ExecuteAction(MULTIBTN_FEATURE_INFO));
        break;

      case chrCapital_X:
      case chrSmall_X:
        // 選択メニュー
        return (ExecuteAction(MULTIBTN_FEATURE_SELMENU));
        break;

      case chrCapital_W:
      case chrSmall_W:
        // マクロ結果表示
        return (ExecuteAction(MULTIBTN_FEATURE_MACRO_LOG));
        break;

      case chrCapital_V:
      case chrSmall_V:
        // 参照ログ取得設定一覧を開く
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENGETLOGLIST);
        break;

      case chrCapital_Y:
      case chrSmall_Y:
        // 指定スレをwebDAで開く
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENWEBDA);
        break;

#ifdef USE_PIN_DIA
      case vchrTT3Short:
      case vchrTT3Long:
        // 画面サイズ変更の指示
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            if (HandEraResizeForm(FrmGetActiveForm()) == true)
            {
                FrmDrawForm(FrmGetActiveForm());
                Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                                   (NNshGlobal->NNsiParam)->selectedTitleItem,
                                   NNSH_STEP_REDRAW);
            }
        }
        break;
#endif

      case chrCapital_R:
      case chrSmall_R:
      case chrCapital_H:
      case chrSmall_H:
      default:
        // その他
        NNsh_DebugMessage(ALTID_INFO, "Enters", "", keyCode);
        return (false);
        break;
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlSelEvt_ThreadMain                                     */
/*                                                      ボタン押下時の処理 */
/*-------------------------------------------------------------------------*/
static Boolean ctlSelEvt_ThreadMain(EventType *event)
{
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_BUTTON6:
        // 多目的ボタン６
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn6Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON1:
        // 多目的ボタン１
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn1Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON2:
        // 多目的ボタン２
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn2Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON3:
        // 多目的ボタン３
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn3Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON5:
        // 多目的ボタン５
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn5Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON4:
        // 多目的ボタン４
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn4Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case CHKID_SELECT_GETMODE:
        // 多目的スイッチ１の設定更新
        if (updateMultiPurposeSwitch((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1,
                                     CHKID_SELECT_GETMODE) == true)
        {
            // 画面更新が必要なパラメータが更新された場合
            // スレ一覧の表示を更新
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                               (NNshGlobal->NNsiParam)->selectedTitleItem,
                               NNSH_STEP_REDRAW);
        }
        break;

      case CHKID_SELECT_MULTIPURPOSE:
        // 多目的スイッチ２の設定更新
        if (updateMultiPurposeSwitch((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2,
                                     CHKID_SELECT_MULTIPURPOSE) == true)
        {
            // 画面更新が必要なパラメータが更新された場合
            // スレ一覧の表示を更新
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                               (NNshGlobal->NNsiParam)->selectedTitleItem,
                               NNSH_STEP_REDRAW);
        }
        break;
        
      case SELTRID_GETLOG_URL_SELECT:
#ifdef USE_LOGCHARGE
        // 表示タイトルの更新
        changeGetLogUrlSite(false);

        // スレ一覧の表示を更新
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_TO_TOP);
#endif  // #ifdef USE_LOGCHARGE
        break;
  
      default:
        // その他
        return (false);
        break;
    }

    // スレ一覧画面を再表示する
    NNsi_RedrawThreadList();
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlRepEvt_ThreadMain                                     */
/*                                              リピートボタン押下時の処理 */
/*-------------------------------------------------------------------------*/
static Boolean ctlRepEvt_ThreadMain(EventType *event)
{
    switch (event->data.ctlRepeat.controlID)
    {
      case BTNID_LISTPREV:
        // １ページ上へ移動
        return (ExecuteAction(MULTIBTN_FEATURE_PREVPAGE));
        break;

      case BTNID_LISTNEXT:
        // １ページ下へ移動
        return (ExecuteAction(MULTIBTN_FEATURE_NEXTPAGE));
        break;

      case BTNID_SEARCHLIST:
        // 検索モードマーカ
        return (false);
        break;

      default:
        // その他
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Handler_ThreadMain                                       */
/*                                          スレ管理画面用イベントハンドラ */
/*=========================================================================*/
Boolean Handler_ThreadMain(EventType *event)
{
    Boolean        ret;
    FormType      *frm;
    RectangleType  dimF;

#ifdef USE_MACRO
    LocalID        dbId;
    if (NNshGlobal->tempStatus == NNSH_MACROCMD_OPERATE)
    {
        if ((NNshGlobal->NNsiParam)->debugMessageON == 0)
        {
            // 起動時にマクロを1度実行する場合、、、
            // (ただし、デバッグモード時には実行しない...)
            NNsi_ExecuteMacroMain(DBNAME_RUNONCEMACRO, DBVERSION_MACROSCRIPT, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);
            NNsi_RedrawThreadList();

            // 自動実行のデータベースを削除する
            dbId = DmFindDatabase(0, DBNAME_RUNONCEMACRO);
            DmDeleteDatabase(0, dbId);

            NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
            return (false);
        }
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
    }

    if (((NNshGlobal->NNsiParam)->autostartMacro != 0)&&
        (NNshGlobal->tempStatus == NNSH_MACROCMD_EXECUTE))
    {
        // マクロの自動実行が指示されていた場合には、マクロを実行する
        ExecuteAction(MULTIBTN_FEATURE_MACRO_EXEC);
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
        return (false);
    }
#endif

    ret = false;
    switch (event->eType)
    {
      case keyDownEvent:
        (void) keyDownEvt_ThreadMain(event);
        return (true);
        break;

      case fldEnterEvent:
        break;

      case ctlSelectEvent:
        return (ctlSelEvt_ThreadMain(event));
        break;

      case ctlRepeatEvent:
        return (ctlRepEvt_ThreadMain(event));
        break;

      case frmOpenEvent:
        break;

      case popSelectEvent:
        return (popEvt_ThreadMain(event));
        break;

      case lstSelectEvent:
        break;

      case lstEnterEvent:
        break;

      case lstExitEvent:
        break;

      case menuEvent:
        (void) menuEvt_ThreadMain(event);
        return (true);
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent :
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                               (NNshGlobal->NNsiParam)->selectedTitleItem,
                               NNSH_STEP_REDRAW);
        }
        break;
#endif  // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                FrmDrawForm(frm);        
                Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                                   (NNshGlobal->NNsiParam)->selectedTitleItem,
                                   NNSH_STEP_REDRAW);
            }
        }
        break;
#endif

      case menuOpenEvent:
        break;

      case sclRepeatEvent:
        break;

      case sclExitEvent:
        break;

      case penDownEvent:
        frm = FrmGetActiveForm();
        FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_TAPPED),&dimF);
        if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
        {
            // Gadget内をタップした(== スレを選択した)
            return (rctPointEvt_ThreadMain(event, frm, &dimF));
        }
        FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_LIST),&dimF);
        if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
        {
            // Gadget内をタップした(== スレの右端を押した)

            // 次ページボタンが押されたかチェック
            FrmGetObjectBounds(frm, 
                               FrmGetObjectIndex(frm, BTNID_LISTNEXT),&dimF);
            if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
            {
                // 次ページボタンが押されたことにする
                return (ExecuteAction(MULTIBTN_FEATURE_NEXTPAGE));
            }

            // 前ページボタンが押されたかチェック
            FrmGetObjectBounds(frm, 
                               FrmGetObjectIndex(frm, BTNID_LISTPREV),&dimF);
            if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
            {
                // 前ページボタンが押されたことにする
                return (ExecuteAction(MULTIBTN_FEATURE_PREVPAGE));
            }

            // スレを選択したことにする
            FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_LIST),&dimF);
            return (rctPointEvt_ThreadMain(event, frm, &dimF));
        }
        return (false);
        break;

      default:
        break;
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   OpenForm_ThreadMain                                      */
/*                                         スレ管理画面を開いたときの処理  */
/*=========================================================================*/
void OpenForm_ThreadMain(FormType *frm)
{
    Err          ret;
    UInt16       lp;
    ControlType *ctlP;

    // 初回オープン時、オフラインスレの検索
    if (((NNshGlobal->NNsiParam)->offChkLaunch != 0)&&(NNshGlobal->bbsTitles == NULL))
    {
        create_offline_database();
    }

    // 多目的スイッチ１の初期設定を画面に反映
    setMultiPurposeSwitch(frm, (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1,
                          CHKID_SELECT_GETMODE);

    // 多目的スイッチ２の初期設定を画面に反映
    setMultiPurposeSwitch(frm, (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2,
                          CHKID_SELECT_MULTIPURPOSE);

    // 多目的ボタン１のキャプション設定
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON1));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn1Caption);

    // 多目的ボタン２のキャプション設定
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON2));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn2Caption);

    // 多目的ボタン３のキャプション設定
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON3));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn3Caption);

    // 多目的ボタン４のキャプション設定
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON4));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn4Caption);

    // 多目的ボタン５のキャプション設定
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON5));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn5Caption);

    // 多目的ボタン６のキャプション設定
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON6));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn6Caption);

    // BBS一覧の作成(板一覧がなければ、BBS一覧を取得するよう促す)
    ret = Create_BBS_INDEX(&(NNshGlobal->bbsTitles), &lp);
    if (ret != errNone)
    {
        // 作成失敗、フォーム情報は更新しないが、一旦画面を描画する
        Update_Thread_List(0, 0, NNSH_STEP_REDRAW);

        // とりあえず、開く画面の設定を行う
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_THREAD;
        NNshGlobal->backFormId = FRMID_THREAD;
        return;
    }

    // (NNshGlobal->NNsiParam)->lastBBS(前回選択していたBBS名)が変なとき(一覧の先頭にする)
    if (lp <= (NNshGlobal->NNsiParam)->lastBBS)
    {
        (NNshGlobal->NNsiParam)->lastBBS = 0;
    }

    // BBS一覧のリスト状態を反映する。
    NNshWinSetListItems(frm, LSTID_BBSINFO, NNshGlobal->bbsTitles, lp,
                        (NNshGlobal->NNsiParam)->lastBBS, &(NNshGlobal->bbsTitleH), NULL);

    // ポップアップトリガのラベルを(リスト状態に合わせ)更新する
    NNshWinSetPopItems(frm, POPTRID_BBSINFO, LSTID_BBSINFO, (NNshGlobal->NNsiParam)->lastBBS);

    // スレ一覧の表示を更新
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem,
                       NNSH_STEP_REDRAW);

    // 開いたフォームIDを設定する(しおり設定中には、lastFrmIDは変更しない)
    if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
    {
        // しおり設定中なので、何もしない
    }
    else
    {
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_THREAD;
    }
    NNshGlobal->backFormId = FRMID_THREAD;
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   menuEvt_ThreadMain                                       */
/*                                      スレ管理画面用メニュー処理ルーチン */
/*-------------------------------------------------------------------------*/
static Boolean menuEvt_ThreadMain(EventType *event)
{
    Boolean err = false;

    switch (event->data.menu.itemID)
    {
      case MNUID_SHOW_DEVICEINFO:
        // デバイスの現在の状態表示
        (void) ExecuteAction(MULTIBTN_FEATURE_DEVICEINFO);
        break;

      case MNUID_SHOW_VERSION:
        // バージョンの表示
        (void) ExecuteAction(MULTIBTN_FEATURE_VERSION);
        break;

      case MNUID_CONFIG_NNSH:
        // NNsi設定画面を開く
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET));
        break;

      case MNUID_NET_DISCONN:
        // 回線切断
        return (ExecuteAction(MULTIBTN_FEATURE_DISCONN));
        break;

      case MNUID_UPDATE_CHECK:
        // 新着メッセージ確認
        return (ExecuteAction(MULTIBTN_FEATURE_NEWARRIVAL));
        break;

      case MNUID_UPDATE_BBS:
        // 板情報更新(取得)
        return (ExecuteAction(MULTIBTN_FEATURE_GETBBS));
        break;

      case MNUID_SELECT_BBS:
        // 使用板選択画面を開く
        return (ExecuteAction(MULTIBTN_FEATURE_USEBBS));
        break;

      case MNUID_GET_NEWMESSAGE:
        // スレ一覧取得(SUBJECT.TXT取得)
        return (ExecuteAction(MULTIBTN_FEATURE_GETLIST));
        break;

      case MNUID_INFO_MESSAGE:
        // スレ情報
        return (ExecuteAction(MULTIBTN_FEATURE_INFO));
        break;

      case MNUID_MODIFY_TITLE:
        // スレタイトル変更
        return (ExecuteAction(MULTIBTN_FEATURE_MODTITLE));
        break;

      case MNUID_DEL_THREADLIST:
        // 未取得スレ削除
        return (ExecuteAction(MULTIBTN_FEATURE_DELNOTGET));
        break;

      case MNUID_DELETE_MESSAGE:
        // スレ削除
        err = ExecuteAction(MULTIBTN_FEATURE_DELETE);
        break;

      case MNUID_GET_ALLMESSAGE:
        // メッセージ全部取得(再取得)
        return (ExecuteAction(MULTIBTN_FEATURE_ALLGET));
        break;

      case MNUID_UPDATE_MESSAGE:
        // メッセージ差分取得
        return (ExecuteAction(MULTIBTN_FEATURE_PARTGET));
        break;

      case MNUID_OPEN_MESSAGE:
        // スレ参照
        return (ExecuteAction(MULTIBTN_FEATURE_OPEN));
        break;

      case MNUID_FONT_MODE:
        // 描画モード変更
        return (ExecuteAction(MULTIBTN_FEATURE_GRAPHMODE));
        break;

      case MNUID_FONT:
        // フォント変更
        return (ExecuteAction(MULTIBTN_FEATURE_FONT));
        break;

      case MNUID_GET_MESSAGENUM:
        // スレ番指定のメッセージ取得
        return (ExecuteAction(MULTIBTN_FEATURE_MESNUM));
        break;

      case MNUID_SEARCH_TITLE:
        // タイトル検索(文字列の設定)
        err = ExecuteAction(MULTIBTN_FEATURE_SEARCH);
        break;

      case MNUID_SEARCH_NEXT:
        // スレタイ検索処理(後方向)
        err = ExecuteAction(MULTIBTN_FEATURE_NEXT);
        break;

      case MNUID_SEARCH_PREV:
        // スレタイ検索処理(前方向)
        err = ExecuteAction(MULTIBTN_FEATURE_PREV);
        break;

      case MNUID_MOVE_TOP:
        // スレタイトルの先頭へ移動
        return (ExecuteAction(MULTIBTN_FEATURE_TOP));
        break;

      case MNUID_MOVE_BOTTOM:
        // スレタイトルの末尾へ移動(ダミーで異常データを送り込む)
        return (ExecuteAction(MULTIBTN_FEATURE_BOTTOM));
        break;

      case MNUID_COPY_READONLY:
        // スレを参照ログへコピーする
        (void) ExecuteAction(MULTIBTN_FEATURE_COPYGIKO);
        break;

      case MNUID_OS5_LAUNCH_WEB:
        // 選択したスレをブラウザで開く
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENWEB);
        break;

      case MNUID_BACKUP_DATABASE:
        // DBをVFSにバックアップする
        (void) ExecuteAction(MULTIBTN_FEATURE_DBCOPY);
        break;

      case MNUID_NET_CONFIG:
        // ネットワーク設定を開く
        (void) ExecuteAction(MULTIBTN_FEATURE_NETWORK);
        break;

      case MNUID_SORT_SUBJECT:
        // スレの整列を実行する
        return (ExecuteAction(MULTIBTN_FEATURE_SORT));
        break;

      case MNUID_HANDERA_ROTATE:
        // HandEra 画面回転 //
        (void) ExecuteAction(MULTIBTN_FEATURE_ROTATE);
        break;

      case MNUID_CONFIG_USER1TAB:
        // ユーザタブ１設定 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER1TAB);
        break;

#ifdef USE_USERTAB_DETAIL
      case MNUID_CONFIG_USER2TAB:
        // ユーザタブ２設定 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER2TAB);
        break;

      case MNUID_CONFIG_USER3TAB:
        // ユーザタブ３設定 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER3TAB);
        break;

      case MNUID_CONFIG_USER4TAB:
        // ユーザタブ４設定 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER4TAB);
        break;

      case MNUID_CONFIG_USER5TAB:
        // ユーザタブ５設定 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER5TAB);
        break;
#endif

      case MNUID_NEW_THREAD:
        // スレ立て
        (void) ExecuteAction(MULTIBTN_FEATURE_NEWTHREAD);
        break;
        
      case MNUID_GOTO_DUMMY:
        // ダミー画面へ
        (void) ExecuteAction(MULTIBTN_FEATURE_GOTODUMMY);
        break;

      case MNUID_NNSI_END:
        // NNsi終了
        (void) ExecuteAction(MULTIBTN_FEATURE_NNSIEND);
        break;

      case MNUID_CHECK_NOFMESSAGE:
        // タブ情報(スレ数確認)
        (void) ExecuteAction(MULTIBTN_FEATURE_TABINFO);
        break;

      case MNUID_MOVE_LOGMNGLOC:
        // ログ管理場所変更
        (void) ExecuteAction(MULTIBTN_FEATURE_MOVELOGLOC);
        break;

      case MNUID_GET_URL:
        // URLを開く
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENURL);
        break;

#ifdef USE_MACROEDIT
      case MNUID_MACRO_SETTING:
        // マクロ設定
        (void) ExecuteAction(MULTIBTN_FEATURE_MACRO_SET);
        break;
#endif

      case MNUID_MACRO_EXECUTE:
        // マクロ実行
        (void) ExecuteAction(MULTIBTN_FEATURE_MACRO_EXEC);
        break;
      case MNUID_MACRO_EXECLOG:
        // マクロ実行ログ
        (void) ExecuteAction(MULTIBTN_FEATURE_MACRO_LOG);
        break;

      case MNUID_MACRO_DATAVIEW:
        // マクロ内容表示
        (void) ExecuteAction(MULTIBTN_FEATURE_MACRO_VIEW);
        break;

      case MNUID_OYSTERLOGIN:
        // ●ログイン
        (void) ExecuteAction(MULTIBTN_FEATURE_OYSTERLOGIN);
        break;

      case MNUID_SHOW_LISTHELP:
        // 操作説明
        (void) ExecuteAction(MULTIBTN_FEATURE_USAGE);
        break;

      case MNUID_SELECTIONMENU:
        // 選択メニュー
        (void) ExecuteAction(MULTIBTN_FEATURE_SELMENU);
        break;
      case MNUID_GETLOGLIST:
        // 参照ログ取得設定一覧
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENGETLOGLIST);
        break;

      case MNUID_SEARCH_BBS:
        // 板検索(文字列の設定)
        err = ExecuteAction(MULTIBTN_FEATURE_SEARCHBBS);
        break;

      case MNUID_SEARCH_NEXTBBS:
        // 板の次検索
        err = ExecuteAction(MULTIBTN_FEATURE_NEXTSEARCHBBS);
        break;

      case MNUID_BEAM_URL:
        // URLをBeamする
        err = ExecuteAction(MULTIBTN_FEATURE_BEAMURL);
        break;

      case MNUID_MACRO_SELECT_EXEC:
        // マクロ選択実行
        err = ExecuteAction(MULTIBTN_FEATURE_SELECT_MACRO_EXEC);
        break;

      case MNUID_BE2chLOGIN:
        // Be@2chログイン処理を実行する
        err = ExecuteAction(MULTIBTN_FEATURE_BE_LOGIN);
        break;

      case MNUID_LIST_GETLOGLIST:
        // スレ番号指定予約の予約一覧を表示する
        (void) ExecuteAction(MULTIBTN_FEATURE_LIST_GETLOG);
        break;

      default:
        // その他(ありえないはずだが...)
        break;
    }

    // スレ一覧画面を再表示する
    NNsi_RedrawThreadList();
    return (err);
}

/*=========================================================================*/
/*   Function : NNsi_RedrawThreadList                                      */
/*                                                    スレ一覧画面を再描画 */
/*=========================================================================*/
void NNsi_RedrawThreadList(void)
{
    FormType      *frm;
    RectangleType  dimF;
    UInt16         fontID, fontHeight, nlines;

    // スレ一覧画面以外では更新しない
    if (FrmGetActiveFormID() == FRMID_THREAD)
    {
        frm = FrmGetActiveForm();
        FrmDrawForm(frm);
        FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, GADID_MESSAGE_LIST), 
                           &dimF);
        fontID = ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ?
                           (NNshGlobal->NNsiParam)->currentFont : (NNshGlobal->NNsiParam)->sonyHRFontTitle;

        NNsi_UpdateRectangle(&dimF, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                             &fontID, &fontHeight, &nlines);

        NNsi_WinDrawList((NNshGlobal->NNsiParam)->selectedTitleItem, 
                         NNshGlobal->msgTitleIndex, nlines, &dimF, fontHeight);
#ifdef USE_PIN_DIA
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            NNshGlobal->updateHR = NNSH_UPDATE_DISPLAY;
        }
#endif
    }
    return;
}

#ifdef USE_LOGCHARGE
/*-------------------------------------------------------------------------*/
/*   Function :   openURL                                                  */
/*                                                              URLを開く  */
/*-------------------------------------------------------------------------*/
static void openURL(void)
{
    ListType            *lstP;
    UInt16               selBBS, isImage;
    Char                *url, *fileName, *ptr;
    NNshGetLogDatabase  *dbData;
    
    // 表示タブが参照ログかチェック
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);
#ifdef GETLOG_LIMIT_OFFLINE
    if (selBBS != NNSH_SELBBS_OFFLINE)
    {
        // この機能は、参照ログだけの機能なので何もせず終了する
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_ONLY_READONLYLOG, MSG_ABORT_MESSAGE, 0);
        return;
    }
#endif

    // 取得するURLの入力
    url = MEMALLOC_PTR(BUFSIZE + MARGIN);
    if (url == NULL)
    {
        return;
    }
    MemSet(url, BUFSIZE + MARGIN, 0x00);

    // ファイル名格納領域を確保
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        MEMFREE_PTR(url);
        return;
    }
    MemSet(fileName, MAXLENGTH_FILENAME + MARGIN, 0x00);

    // DBのダミー領域を確保
    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        MEMFREE_PTR(url);
        MEMFREE_PTR(fileName);
        return;
    }
    MemSet(dbData, sizeof(NNshGetLogDatabase) + MARGIN, 0x00);

    // URLを入力してもらう。
    isImage = 0;
    StrCopy(url, "http://");
    if (DataInputDialog(MSG_DIALOG_SET_URL, url, BUFSIZE, 
                            NNSH_DIALOG_USE_INPUTURL, &(isImage)) != true)
    {
        // URLの入力がキャンセルされた場合
        goto ABORT_END;
    }
#ifdef USE_SSL
    if ((url[0] != 'h')||(url[1] != 't')||(url[2] != 't')||(url[3] != 'p')||
        ((url[4] != ':')&&(url[4] != 's'))||((url[5] != '/')&&(url[5] != ':')))
    {
        // URLの指定がまちがっていた場合(http:// ではじまらなかった場合)
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_WRONGURL, url, 0);
        goto ABORT_END;
    }
#else
    if ((url[0] != 'h')||(url[1] != 't')||(url[2] != 't')||(url[3] != 'p')||
        (url[4] != ':')||(url[5] != '/'))
    {
        // URLの指定がまちがっていた場合(http:// ではじまらなかった場合)
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_WRONGURL_NOTSSL, url, 0);
        goto ABORT_END;
    }
#endif
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, url, MSG_GETURLLOG_CONFIRM, 0) != 0)
    {
        // データの取得確認がキャンセルされた場合
        goto ABORT_END;
    }

    // データを保存するスレファイル名の特定
    ptr = url + StrLen(url) - 1;
    while ((*ptr != '/')&&(*ptr != '?')&&(*ptr != '*'))
    {
        ptr--;
    }

    // データの最後にスラッシュがあった場合にはファイル名をコピーしない
    if (ptr < url + StrLen(url) - 1)
    {
        StrNCopy(fileName, (ptr + 1), (MAXLENGTH_FILENAME - 1));
    }

    if (isImage == 0)
    {
        // ＵＲＬデータの取得実処理...
        ReadOnlyLogDataPickup(false, dbData, url, fileName, NNSH_MSGNUM_UNKNOWN, 0, 0, 0, NULL);

        // 現在表示中の板は参照専用ログの場合
        // (スレ一覧に表示すべき項目が増える可能性がある場合...)
        if (selBBS == NNSH_SELBBS_OFFLINE)
        {
            // ログ一覧の先頭のTOPディレクトリを表示する
            (NNshGlobal->NNsiParam)->readOnlySelection = 0;

            // 選択されたディレクトリが更新された場合には、スレタイ一覧を更新する
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        }
        else
        {
            //  板一覧が参照専用ログではない場合には、スレ一覧の情報が更新
            // されないはずなので、領域を開放して終了する(ABORTじゃないけど...)
            goto ABORT_END;
        }
    }
    else
    {
        // URLからイメージデータを取得する処理
        ReadOnlyLogDataPickup(true, dbData, url, fileName, NNSH_MSGNUM_UNKNOWN, 0, 0, 0, NULL);

        // イメージデータ受信時には、スレ一覧の情報が更新されないので
        // 領域を開放して終了する(ABORTじゃないけど...)
        goto ABORT_END;
    }

    // 確保領域を開放
    MEMFREE_PTR(dbData);
    MEMFREE_PTR(fileName);
    MEMFREE_PTR(url);
    return;

ABORT_END:
    // 確保領域を開放
    MEMFREE_PTR(dbData);
    MEMFREE_PTR(fileName);
    MEMFREE_PTR(url);
    
    // 画面を再描画して終了する
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                              (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);
    return;
}
#endif
#ifdef USE_LOGCHARGE
/*-------------------------------------------------------------------------*/
/*   Function :   changeGetLogUrlSite                                      */
/*                                        参照ログモードのサイトを選択する */
/*-------------------------------------------------------------------------*/
static void changeGetLogUrlSite(Boolean defaultSettingMode)
{
    Err                   ret;
    NNshGetLogDatabase  *logDb;
    DmOpenRef            dbRef;
    MemHandle            memH;
    UInt16               size, itemCnt, btnId, dbCount, loop, recList[MAX_SELECTLOGRECORD + 1];
    ControlType         *selP;
    Char                *labelP, *buffer, *addrAddr[MAX_SELECTLOGRECORD + 1];

    // 各種変数の初期化
    dbCount = 0;
    itemCnt = 0;
    memH    = 0;
    size    = MAX_TITLE_LABEL * MAX_SELECTLOGRECORD;
    selP    = FrmGetObjectPtr(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));

    // データラベル領域の確保
    buffer = MEMALLOC_PTR(size + MARGIN);
    if (buffer == NULL)
    {
        // データ領域確保失敗、エラーにする
        return;
    }
    MemSet(buffer, (size + MARGIN), 0x00);
    labelP = buffer;

    //////////  サイト名の一覧とインデックスを生成する //////////
    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return;
    }

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &dbCount);

    // Busyダイアログを表示する
    Show_BusyForm(MSG_CHECK_URLLIST);

    // レコード番号の若いほうから順に表示する...
    for (loop = dbCount; ((loop != 0)&&(itemCnt < MAX_SELECTLOGRECORD)); loop--)
    {
        // レコード情報を取得する
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &memH, (void**) &logDb);
        if (ret == errNone)
        {
            // リストに表示すべきログか確認する
            if (((NNshGlobal->NNsiParam)->getROLogLevel == 0)||(logDb->usable == (NNshGlobal->NNsiParam)->getROLogLevel))
            {
                // 表示すべきログタイトルだった！
                StrCopy(labelP, logDb->dataTitle);
                addrAddr[itemCnt] = labelP;
                recList [itemCnt] = (loop - 1);
                labelP = labelP + StrLen(logDb->dataTitle) + 1;
                itemCnt++;
            }

            // 確保したレコードを解除する
            ReleaseRecordReadOnly_NNsh(dbRef, memH);
        }
    }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // １つもない、終了する
        CtlSetLabel(selP, "???");
        MEMFREE_PTR(buffer);
        return;
    }
    // 選択ウィンドウを表示し、URLの選択を促す
    if (defaultSettingMode != true)
    {
        btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, buffer, itemCnt, 0);    
    }
    else
    {
        //(NNshGlobal->NNsiParam)->selectedTitleItem = 0;

        // 前回の設定を自動的に画面へ反映させる場合...
        for (loop = 0; loop < itemCnt; loop++)
        {
            if (recList[loop] == ((NNshGlobal->NNsiParam)->selectedTitleItemForGetLog))
            {
                StrNCopy(NNshGlobal->getLogListUrlNameLabel, addrAddr[loop], MAX_TITLE_LABEL);
                CtlSetLabel(selP, NNshGlobal->getLogListUrlNameLabel);
                MEMFREE_PTR(buffer);
                return;
            }
        }
        // サイトが登録抹消されていた場合...
        (NNshGlobal->NNsiParam)->selectedTitleItemForGetLog = recList[0];
        StrNCopy(NNshGlobal->getLogListUrlNameLabel, addrAddr[0], MAX_TITLE_LABEL);
        CtlSetLabel(selP, NNshGlobal->getLogListUrlNameLabel);
        MEMFREE_PTR(buffer);
        return;
    }

    // OKボタンが押されたとき、、、
    if (btnId != BTNID_JUMPCANCEL)
    {
        //(NNshGlobal->NNsiParam)->selectedTitleItem = 0;

        // サイト名をセレクタトリガラベルに反映させる
        (NNshGlobal->NNsiParam)->selectedTitleItemForGetLog = recList[NNshGlobal->jumpSelection];
        StrNCopy(NNshGlobal->getLogListUrlNameLabel, addrAddr[NNshGlobal->jumpSelection], MAX_TITLE_LABEL);
        CtlSetLabel(selP, NNshGlobal->getLogListUrlNameLabel);
    }
    MEMFREE_PTR(buffer);
    return;
}
#endif // #ifdef USE_LOGCHARGE
