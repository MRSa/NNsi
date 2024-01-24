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
static Err create_BBS_INDEX(Char **bdLst, UInt16 *cnt);

static Boolean set_SearchTitleString     (void);
static Boolean search_NextTitle          (Int16 direction);
static Boolean display_message           (void);
static Boolean get_all_message           (void);
static Boolean update_message            (void);
static Boolean info_message              (void);
static Boolean update_getall_message     (void);
static Boolean redraw_Thread_List        (UInt16 bbsIndex, UInt16 threadIndex);
static Boolean delete_notreceived_message(void);
static Boolean delete_message            (void);
static Boolean get_subject_txt           (void);
static Boolean popEvt_ThreadMain         (EventType *event);
static Boolean keyDownEvt_ThreadMain     (EventType *event);
static Boolean ctlSelEvt_ThreadMain      (EventType *event);
static Boolean menuEvt_ThreadMain        (EventType *event);

extern void    Initialize_NNshSetting(FormType *frm);
extern void    Effect_NNshSetting    (FormType *frm);
extern Boolean Handler_NNshSetting   (EventType *event);
extern Boolean Handler_JumpSelection(EventType *event);

/*=========================================================================*/
/*   Function :   Hander_EditAction                                        */
/*                                         編集操作専用のイベントハンドラ  */
/*=========================================================================*/
Boolean Handler_EditAction(EventType *event)
{
    // 現在のフォームを取得
    switch (event->eType)
    { 
      // メニュー選択
      case menuEvent:
        return (NNsh_MenuEvt_Edit(event));
        break;

      case ctlSelectEvent:
      default: 
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   open_menu                                                */
/*                                                 NNsi独自のメニュー表示  */
/*-------------------------------------------------------------------------*/
static Boolean open_menu(void)
{
    FormType         *frm;
    ControlType      *ctlP;
    EventType        *dummyEvent;
    Char             *ptr, listPtr[BUFSIZE];
    UInt16            btnId;

    // リストの作成
    MemSet(listPtr, BUFSIZE, 0x00);
    ptr = listPtr;

    // BBSを選択
    StrCopy(ptr, NNSH_JUMPMSG_OPENBBS);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENBBS);

    // メニューを開く
    StrCopy(ptr, NNSH_JUMPMSG_OPENMENU);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENMENU);

    // 新着確認
    StrCopy(ptr, NNSH_JUMPMSG_OPENNEW);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENNEW);

    // 一覧取得
    StrCopy(ptr, NNSH_JUMPMSG_OPENLIST);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENLIST);

    // スレ取得
    StrCopy(ptr, NNSH_JUMPMSG_OPENGET);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENGET);

    // スレ情報
    StrCopy(ptr, NNSH_JUMPMSG_OPENINFO);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENINFO);

    // 選択ウィンドウを表示する
    btnId = NNshWinSelectionWindow(listPtr, NNSH_JUMPSEL_NUMLIST);
    if (btnId != BTNID_JUMPCANCEL)
    {
        // 選択リスト番号を取得
        switch (NNshGlobal->jumpSelection)
        {
          case NNSH_JUMPSEL_OPENBBS:
            // BBS一覧を選択(ポップアップ)
            frm  = FrmGetActiveForm();
            ctlP = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,POPTRID_BBSINFO));
            CtlHitControl(ctlP);
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

          case NNSH_JUMPSEL_OPENNEW:
            // 新着確認
            (void) update_newarrival_message();
            break;

          case NNSH_JUMPSEL_OPENLIST:
            // 一覧更新
            (void) get_subject_txt();
            break;

          case NNSH_JUMPSEL_OPENGET:
            // メッセージ取得(差分取得/全部取得は自動判断)
            (void) update_getall_message();
            break;

          case NNSH_JUMPSEL_OPENINFO:
            // スレ情報表示
            (void) info_message();
            break;

          default:
            // 何もしない
            break;
        }
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : NNshMain_Close                                             */
/*                                                      一覧画面のクローズ */
/*-------------------------------------------------------------------------*/
static void NNshMain_Close(UInt16 nextFormID)
{
    // スレ一覧の領域は一旦開放する(画面を開いたときに必ず再構築するため)
    if (NNshGlobal->threadTitles != NULL)
    {
        MemHandleUnlock(NNshGlobal->msgTTLStringH);
        NNshGlobal->threadTitles = NULL;
    }
    MEMFREE_HANDLE(NNshGlobal->msgTTLStringH);

    // 別の画面を開く
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(nextFormID);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   restoreDatabaseToVFS                                     */
/*                                           NNsiのDBをVFSからリストアする */
/*-------------------------------------------------------------------------*/
static Err restoreDatabaseToVFS(void)
{
    Err      ret;
    FileRef  fileRef;
    Char     fileName[MAXLENGTH_FILENAME];
    UInt32   dummy;
    UInt16   butId;

    // VFSのサポート有無を確認する。
    dummy = 0;
    ret = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &dummy);
    if (ret != errNone)
    {
        // VFS非サポートの場合には、即終了。
        NNsh_DebugMessage(ALTID_INFO, "Not Support VFS :", "", ret);
        return (~errNone);
    }

    // VFSが使用できるとき、BackupされたDBがあるか(FILEをOPENして)確認
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(fileName, DATAFILE_PREFIX);
    StrCat (fileName, DBNAME_SUBJECT);
    StrCat (fileName, ".pdb");
    if (VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &fileRef) !=
        errNone)
    {
        // BACKUPされたファイルがないので終了する
        NNsh_DebugMessage(ALTID_INFO, "Backup DB files do not exist", "", 0);
        return (~errNone);
    }
    // File Open成功(== DBがある)、リストアするか確認する
    VFSFileClose(fileRef);
    butId = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_RESTORE, "", 0);
    if (butId != 0)
    {
        // Cancelが選択されたため、リストアしない。
        NNsh_DebugMessage(ALTID_INFO, "DB-Restore is canceled.", "", 0);
        return (~errNone);
    }

    // "リストア中"表示
    Show_BusyForm(MSG_DBRESTORE_BUSY);

    // DBリストアメイン
    (void) RestoreDatabaseFromVFS_NNsh(DBNAME_SETTING);
    (void) RestoreDatabaseFromVFS_NNsh(DBNAME_DIRINDEX);
    ret  = RestoreDatabaseFromVFS_NNsh(DBNAME_BBSLIST);
    ret  = (ret)|(RestoreDatabaseFromVFS_NNsh(DBNAME_SUBJECT));

    // "リストア中"表示の削除
    Hide_BusyForm();

    // DBのリストア結果表示
    if (ret == errNone)
    {
        // リストア成功
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_OK, "", 0);
    }
    else
    {
        // リストア失敗
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_NG, "", 0);
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   copy_to_readOnly                                         */
/*                                  ログファイルを参照専用ログにコピーする */
/*-------------------------------------------------------------------------*/
static Boolean copy_to_readOnly(void)
{
    Err                  ret;
    UInt16               selM, selBBS, butID;
    NNshSubjectDatabase  mesInfo;
    NNshBoardDatabase    bbsInfo;
    Char                *fileName, *orgName, *area;

    // 本機能は、VFS専用である旨表示する(VFS以外は処理を行わない)
    if ((NNshParam->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        NNsh_ErrorMessage(ALTID_WARN, MSG_ONLYVFS_FEATURE, "", 0);
        return (false);
    }

    // バッファの確保
    fileName = MemPtrNew(MAXLENGTH_FILENAME);
    if (fileName == NULL)
    {
        return (false);
    }
    orgName  = MemPtrNew(MAXLENGTH_FILENAME);
    if (orgName == NULL)
    {
        MemPtrFree(fileName);
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
        // Palm本体からVFSの参照専用ログへコピーする
        ret = CopyFile_NNsh(fileName, orgName, NNSH_VFS_DISABLE);
    }
    else
    {
        // VFSのログからVFSの参照専用ログへコピーする
        ret = CopyFile_NNsh(fileName, orgName, NNSH_VFS_ENABLE);
    }
    Hide_BusyForm();
    if (ret != errNone)
    {
        // ファイルコピーに失敗した。
        (void) DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);

        // コピーに失敗した旨表示する
        NNsh_ErrorMessage(ALTID_ERROR, MSG_FAILURE_COPY, fileName, ret);
        goto FUNC_END;
    }

    // コピーに成功、参照専用ログに表示するときにはscan必要を警告
    NNsh_InformMessage(ALTID_INFO, MSG_INFORM_COPYEND, "", 0);

FUNC_END:
    MemPtrFree(orgName);
    MemPtrFree(fileName);
    if (ret != errNone)
    {
        return (false);
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_MessageFromMesNum                                    */
/*                                            スレ番号指定のメッセージ取得 */
/*-------------------------------------------------------------------------*/
static Boolean get_MessageFromMesNum(void)
{
    Boolean              rep;
    Err                  ret;
    UInt16               btnId, index, dataIndex;
    UInt32               size;
    DmOpenRef            dbRef;
    Char                *url, *ptr, *nPtr;
    FormType            *prevFrm, *diagFrm;
    NNshBoardDatabase   bbsData;
    NNshSubjectDatabase subjDb;

    // ワークバッファの確保と初期化
    url     = MemPtrNew(BUFSIZE);
    if (url == NULL)
    {
        return (false);
    }
    index = 0;
    MemSet(&subjDb,  sizeof(NNshSubjectDatabase), 0x00);
    MemSet(&bbsData, sizeof(NNshBoardDatabase),   0x00);
    MemSet(url,     BUFSIZE,                      0x00);

    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if ((FtrGet(sysFtrCreator, sysFtrNumROMVersion, &size) == errNone)
        &&(size < 0x03503000))
    {
        FrmEraseForm(prevFrm);
    }
    diagFrm  = FrmInitForm(FRMID_GETMESSAGE_NUMBER);
    if (diagFrm == NULL)
    {
        rep = false;
        goto FUNC_END;
    }

    // シルク表示を最大化
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(diagFrm, vgaFormModify160To240);
        SilkMaximizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMaximize(diagFrm);
    }
    HandEraMoveDialog(diagFrm);
    FrmSetActiveForm(diagFrm);

    // カーソルを入力欄に移動する
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_BBS_NICK));

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_EditAction);

    // ダイアログのオープン、文字列の入力を待つ
    btnId = FrmDoDialog(diagFrm);

    // Windowから(入力された)文字列を取得する
    NNshWinGetFieldText(diagFrm, FLDID_BBS_NICK, url, BUFSIZE);

    // ダイアログを閉じる
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // シルク表示を最小化
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMinimize(prevFrm);
    }
    FrmDrawForm(prevFrm);

    // 押されたボタンのチェック
    if (btnId == BTNID_GET_CANCEL)
    {
        // Cancelボタンが押されたときには、何もせず終了する。
        rep = false;
        goto FUNC_END;
    }

    // 入力データから板Nickを切り出す
    ptr  = url;
    nPtr = subjDb.boardNick;
    while (*ptr != '/')
    { 
        *nPtr = *ptr;
        ptr++;
        nPtr++;
        if (*ptr == '\0')
        {
            // データ取得エラー、終了する
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_GETFAILURE, "", 0);
            rep = false;
            goto FUNC_END;
        }
    }
    *nPtr = '/';    // 板Nickの最後に '/'を付加しておく

    // スレ番号を切り出す
    ptr++;
    nPtr = subjDb.threadFileName;
    while ((*ptr != '/')&&(*ptr != '\0'))
    {
        *nPtr = *ptr;
	nPtr++;
        ptr++;
    }

    // BBS一覧読み込み中のメッセージを表示する
    Show_BusyForm(MSG_READ_BBSLIST_WAIT);

    // 板情報の取得
    ret = Get_BBS_Database(subjDb.boardNick, &bbsData, &index);
    if (ret != errNone)
    {
        // 板情報取得に失敗、エラー表示して終了
        NNsh_ErrorMessage(ALTID_ERROR, subjDb.boardNick,
                          MSG_WRONG_BOARDNICK, ret);
        Hide_BusyForm();
        rep = false;
        goto FUNC_END;
    }

    // メッセージの記入
    MemSet (url, BUFSIZE, 0x00);
    StrCopy(url, bbsData.boardName);
    StrCat (url, MSG_CONFIRM_GETTHREAD);
    StrCat (url, subjDb.threadFileName);
    StrCat (url, ")");

    // メッセージの削除
    Hide_BusyForm();

    // 本当にスレを取得するか確認を行う
    btnId = NNsh_ConfirmMessage(ALTID_CONFIRM, url, "", 0);
    if (btnId != 0)
    {
        // 取得をキャンセルする
        NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
        rep = false;
        goto FUNC_END;
    }

    // スレ情報取得中のメッセージを表示する
    Show_BusyForm(MSG_MESSAGE_INFO_WAIT);

    // メッセージ取得先の作成
    MemSet (url, BUFSIZE, 0x00);
    StrCopy(url, bbsData.boardURL);
    switch (bbsData.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // まちBBSからデータ取得(HTMLを読む)
        StrCat (url, "bbs/read.pl?BBS=");
        StrCat (url, bbsData.boardNick);
        // boardNickの末尾にある '/'を削除
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, subjDb.threadFileName);

        StrCat (subjDb.threadFileName, ".cgi");   // 拡張子をつける

        //////////////////////////////////////////////////////////////////////
        // 現在、まちＢＢＳにはこの機能は使用できない旨を表示して終了する。
        NNsh_InformMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI,"",0);
        rep = false;
        goto FUNC_END;
        //////////////////////////////////////////////////////////////////////
        break;

      case NNSH_BBSTYPE_2ch:
      default:
        // 2chからデータ取得(datを読む)
        StrCat (subjDb.threadFileName, ".dat");   // 拡張子をつける

        StrCat (url, "dat/");
        StrCat (url, subjDb.threadFileName);
        break;
    }

    // スレデータが取得済みか確認する。
    if (check_same_thread(bbsData.threadCnt, &subjDb, &subjDb, &dataIndex)
                                                                    == errNone)
    {
        Hide_BusyForm();      // メッセージの削除

        // 取得済みと判明、差分取得に切り替える
        ret = Get_PartMessage(url, bbsData.boardNick, &subjDb, 
                              convertListIndexToMsgIndex(dataIndex));

        // 取得終了の表示(DEBUGオプション有効時のみ)
        NNsh_InformMessage(ALTID_INFO, MSG_GET_PARTMESSAGE_END,
                           subjDb.threadTitle, ret);

        // 先頭を表示する
        rep = redraw_Thread_List(NNshParam->lastBBS, 0);
        goto FUNC_END;
    }

    ////////////  メッセージ全部取得の処理  ///////////

    /////////////////////////////////////////////////////////////////////////
    // データベースにスレデータを(仮)追加する ※スレタイトルは後で追加する※
    /////////////////////////////////////////////////////////////////////////
    StrCopy(subjDb.threadTitle, "(Unknown)");

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // メッセージの削除
        Hide_BusyForm();

        // エラー発生
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh()", "", 0);
        rep = false;
        goto FUNC_END;
    }
    EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &subjDb);
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    /** (VFS上で)ディレクトリを作成する **/
    (void) CreateDir_NNsh(bbsData.boardNick);
    
    // メッセージの削除
    Hide_BusyForm();

    // ログ取得および記録処理
    ret = Get_AllMessage(url, bbsData.boardNick, subjDb.threadFileName,
                         0, (UInt16) bbsData.bbsType);

    if (ret != errNone)
    {
        // スレファイル名を 変数urlに書き出し、(長さ０の)スレファイルを削除
        MemSet (url, BUFSIZE, 0x00);
        StrCopy(url, bbsData.boardNick);
        StrCat (url, subjDb.threadFileName);
        if ((NNshParam->useVFS & NNSH_VFS_ENABLE) == 0)
        {
            // Palm本体上のログを削除する
            ret = DeleteFile_NNsh(url, NNSH_VFS_DISABLE);
        }
        else
        {
            // VFS上のログを削除する
            ret = DeleteFile_NNsh(url, NNSH_VFS_ENABLE);
        }

        // エラー発生(仮追加したデータベースデータを削除する)
        (void) OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
        DeleteRecordIDX_NNsh(dbRef, 0);
        CloseDatabase_NNsh(dbRef);
        NNsh_DebugMessage(ALTID_ERROR, "Get_AllMessage()", "", ret);
        rep = false;
        goto FUNC_END;
    }

    //////////////////スレタイトル取得処理/////////////////////////

    // スレ情報取得中のメッセージを表示する
    Show_BusyForm(MSG_MESSAGE_INFO_WAIT);

    // スレ情報を取得する
    (void) Get_Subject_Database(0, &subjDb);

    // ファイルサイズを一時的に記憶
    size = subjDb.fileSize;

    // スレファイル名を 変数urlに書き出す
    MemSet (url, BUFSIZE, 0x00);
    StrCopy(url, bbsData.boardNick);
    StrCat (url, subjDb.threadFileName);

    // ファイルからスレ情報を取得する
    ret = SetThreadInformation(url, (NNSH_FILEMODE_READONLY), &subjDb);
    Hide_BusyForm();  // メッセージの削除
    if (ret != errNone)
    {
        // エラー発生
        NNsh_DebugMessage(ALTID_ERROR, "SetThreadInformation()\nFileName:",
                          subjDb.threadFileName, ret);
        // リターンせずに処理を継続する
    }

    // ファイルサイズを書き戻し
    subjDb.fileSize = size;

    // レコードの登録
    ret = update_subject_database(0, &subjDb);
    if (ret != errNone)
    {
        // エラー発生
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()", "", ret);
        rep = false;
        goto FUNC_END;
    }

    // 取得終了の表示
    NNsh_InformMessage(ALTID_INFO,MSG_GET_MESSAGE_END,subjDb.threadTitle,ret);

    // 板情報がVISIBLEではない場合、VISIBLEにするか確認する
    btnId = 1;
    if (bbsData.state != NNSH_BBSSTATE_VISIBLE)
    {
        btnId = NNsh_ConfirmMessage(ALTID_CONFIRM, bbsData.boardName,
                                    MSG_CONFIRM_SETVISIBLE, 0);
        if (btnId == 0)
        {
            // BBSをVISIBLEに変更する（CONFIRMATIONで OKを選択した場合）
            bbsData.state = NNSH_BBSSTATE_VISIBLE;
        }
    }

    // 取得スレ数をカウントアップする
    dataIndex = bbsData.threadCnt;
    bbsData.threadCnt = NNSH_BBSLIST_AGAIN;

    // BBS情報の更新
    Update_BBS_Database(index, &bbsData);

    // 使用BBS一覧が更新されたとき
    if (btnId == 0)
    {
        // BBS一覧を初期化
        MEMFREE_PTR(NNshGlobal->bbsTitles);

        // BBS一覧の更新
        (void) create_BBS_INDEX(&(NNshGlobal->bbsTitles), &index);

        // BBS一覧のリスト状態を反映する。
        NNshWinSetListItems(prevFrm, LSTID_BBSINFO,
                            NNshGlobal->bbsTitles, index,
                            NNshParam->lastBBS, &(NNshGlobal->bbsTitleH));
    }

    // スレ一覧&BBS一覧を再表示する
    rep = redraw_Thread_List(NNshParam->lastBBS, (dataIndex + 1));

FUNC_END:
    // 関数の終了、
    MEMFREE_PTR(url);
    return (rep);
}

/*-------------------------------------------------------------------------*/
/*   Function :   set_SearchTitleString                                    */
/*                                                スレタイ検索文字列の設定 */
/*-------------------------------------------------------------------------*/
static Boolean set_SearchTitleString(void)
{
    Boolean   ret = false;
    FormType *prevFrm, *diagFrm;
    Char     *ptr = NULL;
    UInt16    btnId;
    UInt32    romVersion;

    // 検索文字列領域の取得
    if (NNshGlobal->searchTitleH == 0)
    {
        NNshGlobal->searchTitleH = MemHandleNew(BUFSIZE);
        if (NNshGlobal->searchTitleH == 0)
        {
            return (false);
        }
        ptr = MemHandleLock(NNshGlobal->searchTitleH);
        if (ptr != NULL)
        {
            MemSet(ptr, BUFSIZE, 0x00);
        }
    }
    else
    {
        ptr = MemHandleLock(NNshGlobal->searchTitleH);
    }
    if (ptr == NULL)
    {
        // 領域取得失敗
        MemHandleFree(NNshGlobal->searchTitleH);
        NNshGlobal->searchTitleH = 0;
        return (false);
    }

    // PalmOS v3.5以下なら、前のフォームを消去する。
    prevFrm = FrmGetActiveForm();

    if ((FtrGet(sysFtrCreator,sysFtrNumROMVersion,&romVersion) == errNone)
        &&(romVersion < 0x03503000))
    {
        FrmEraseForm(prevFrm);
    }
    diagFrm  = FrmInitForm(FRMID_SEARCHTITLE);
    if (diagFrm == NULL)
    {
        MemHandleUnlock(NNshGlobal->searchTitleH);
        return (false);
    }

    // シルク表示を最大化
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(diagFrm, vgaFormModify160To240);
        SilkMaximizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMaximize(diagFrm);
    }
    HandEraMoveDialog(diagFrm);
    FrmSetActiveForm(diagFrm);

    // 現在の検索設定パラメータをウィンドウに反映させる
    SetControlValue(diagFrm, CHKID_CASELESS,
                                  &(NNshParam->searchCaseless));

    // 現在の検索文字列設定をウィンドウに反映させる
    NNshWinSetFieldText(diagFrm, FLDID_SEARCHTITLE, ptr, StrLen(ptr));
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_SEARCHTITLE));

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_EditAction);

    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_SEARCHTITLE_OK)
    {
        // OKボタンが押されたときには、検索文字列を取得する
        NNshWinGetFieldText(diagFrm, FLDID_SEARCHTITLE, ptr, BUFSIZE);

        // 現在の検索設定パラメータをパラメータに反映させる
        UpdateParameter(diagFrm, CHKID_CASELESS,
                             &(NNshParam->searchCaseless));
        ret = true;
        NNsh_DebugMessage(ALTID_INFO,"String:", ptr, 0);
    }
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // シルク表示を最小化
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMinimize(prevFrm);
    }
    FrmDrawForm(prevFrm);
    MemHandleUnlock(NNshGlobal->searchTitleH);
    return (ret);
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
    NNshSubjectDatabase  mesInfo;
    NNshBoardDatabase    bbsInfo;

    // check if title string is already set or not.
    if (NNshGlobal->searchTitleH == 0)
    {
        return (false);
    }

    // 検索文字列を取得
    ptr = MemHandleLock(NNshGlobal->searchTitleH);
    if (ptr == NULL)
    {
        return (false);
    }

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    // スレ検索の実施
    MemSet (msg, sizeof(msg), 0x00);
    StrCopy(msg, MSG_SEARCHING_TITLE);
    StrCat (msg, ptr);
    Show_BusyForm(msg);

    if (direction == NNSH_SEARCH_FORWARD)
    {
        // フォワード検索指示
        end  = NNshGlobal->threadCount;
        step = 1;
    }
    else
    {
        // バックワード検索指示
        end  =  0;
        step = -1;
    }

    // 小文字に変換
    MemSet(match1, sizeof(match1), 0x00);
    if (NNshParam->searchCaseless != 0)
    {
        (void) StrToLower(match1, ptr);
    }
    else
    {
        (void) StrCopy(match1, ptr);
    }

    // 検索の実処理
    for (loop = selMES + step; loop != end; loop = loop + step)
    {
        index = convertListIndexToMsgIndex(loop);
        ret = Get_Subject_Database(index, &mesInfo);
        if (ret != errNone)
        {
            // レコードの取得に失敗
            continue;
        }

        //  指定された文字列とマッチするか確認
        MemSet(match2, sizeof(match2), 0x00);
        if (NNshParam->searchCaseless != 0)
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
            Hide_BusyForm();

            // スレ一覧の表示箇所を変更
            (void) redraw_Thread_List(selBBS, loop);
            MemHandleUnlock(NNshGlobal->searchTitleH);
            return (true);
        }
    }
    Hide_BusyForm();

FUNC_END:
    // 検索に失敗した
    NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, ptr, 0);
    MemHandleUnlock(NNshGlobal->searchTitleH);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   create_BBS_INDEX                                         */
/*                                                      BBS一覧の作成処理  */
/*-------------------------------------------------------------------------*/
static Err create_BBS_INDEX(Char **bdLst, UInt16 *cnt)
{
    Err                   ret = errNone;
    DmOpenRef             bbsRef;
    UInt16                nofBBS, tempSize, lp, useCnt, *idxP;
    Char                 *ptr;
    NNshBoardDatabase     tmpBd;
    NNshBBSRelation      *relPtr;

    // 与えられた *bdLst が NULLでなければ、(BBS一覧構築済みと考え)何もしない
    if (*bdLst != NULL)
    {
        *cnt = NNshGlobal->useBBS;
        NNsh_DebugMessage(ALTID_INFO,"Already created BBS-name-LIST","",*cnt);
        return (errNone);
    }

    // "BBS一覧作成中"の表示
    Show_BusyForm(MSG_READ_BBS_WAIT);

    // BBSデータベースのオープン
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
    GetDBCount_NNsh(bbsRef, &nofBBS);
    if (nofBBS == 0)
    {
        // "作成中"表示の削除
        Hide_BusyForm();
        CloseDatabase_NNsh(bbsRef);

        // VFSからＤＢをリストア(もし可能なら)
        ret = restoreDatabaseToVFS();
        if (ret != errNone)
        {
            // BBSデータベースが存在しなかった場合、警告を表示する
            NNsh_ErrorMessage(ALTID_WARN,
                              MSG_BOARDDB_NONE1, MSG_BOARDDB_NONE2, 0);
        }
        return (~errNone);
    }

    // 使用中BBS数をカウントする
    useCnt = 0;
    for (lp = 0; lp < nofBBS; lp++)
    {
        (void) GetRecord_NNsh(bbsRef, lp, sizeof(tmpBd), &tmpBd);
        if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            useCnt++;
        } 
    }
    if (useCnt == 0)
    {
        // "作成中"表示の削除
        Hide_BusyForm();
        CloseDatabase_NNsh(bbsRef);

        // 使用するデータベースが設定されていなかった場合、警告を表示する
        NNsh_ErrorMessage(ALTID_WARN,
                          MSG_USEBOARD_WARN1, MSG_USEBOARD_WARN2, 0);
        return (~errNone);
    }

    // BBSタイトルインデックス用の領域を確保する
    if (NNshGlobal->boardIdxH != 0)
    {
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;
    }
    tempSize      = sizeof(NNshBBSRelation) +
                       sizeof(UInt16) * (useCnt + NNSH_NOF_SPECIAL_BBS);
    NNshGlobal->boardIdxH = MemHandleNew(tempSize);
    if (tempSize == 0)
    {
       // "作成中"表示の削除
       Hide_BusyForm();

       // 領域確保エラー
       ret = ~errNone;
       NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                          " size:", tempSize);

       // データベースのクローズ
       CloseDatabase_NNsh(bbsRef);
       return (~errNone);
    }
    relPtr = MemHandleLock(NNshGlobal->boardIdxH);
    idxP   = &(relPtr->bbsIndex);

    // BBSタイトル一覧用の領域を確保する
    tempSize = sizeof(Char) *
                     (useCnt + NNSH_NOF_SPECIAL_BBS) * (LIMIT_BOARDNAME + 1);
    *bdLst   = (Char *) MemPtrNew(tempSize);
    if (*bdLst == NULL)
    {
        MemHandleUnlock(NNshGlobal->boardIdxH);
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;

        // "作成中"表示の削除
        Hide_BusyForm();

        // 領域確保エラー
        ret = ~errNone;
        NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                           " size:", tempSize);

        // データベースのクローズ
        CloseDatabase_NNsh(bbsRef);
        return (~errNone);
    }
    MemSet(*bdLst, tempSize, 0x00);
    ptr = *bdLst;

    // BBS一覧の先頭に「お気に入り」「取得済み全て」「未読あり」を追加する
    StrCopy(ptr, FAVORITE_THREAD_NAME);
    ptr   = ptr + sizeof(FAVORITE_THREAD_NAME); // Terminatorも含めて考えてる
    *cnt  = 1;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, GETALL_THREAD_NAME);
    ptr   = ptr + sizeof(GETALL_THREAD_NAME); // Terminatorも含めて考えてる
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, NOTREAD_THREAD_NAME);
    ptr   = ptr + sizeof(NOTREAD_THREAD_NAME); // Terminatorも含めて考えてる
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    // (使用中の)BBSタイトル一覧とインデックスを作成する
    for (lp = 0; lp < nofBBS; lp++)
    {
        MemSet(&tmpBd, sizeof(tmpBd), 0x00);
        (void) GetRecord_NNsh(bbsRef, lp, sizeof(tmpBd), &tmpBd);
        if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            // 板名称をコピーする
            StrNCopy(ptr, tmpBd.boardName, LIMIT_BOARDNAME);
            ptr = ptr + StrLen(ptr) + 1;   // 1 は'\0'の意味、(リストで使用)
            (*cnt)++;
            *idxP = lp;
            idxP++;
	}
    }
    NNshGlobal->useBBS = *cnt;
    MemHandleUnlock(NNshGlobal->boardIdxH);

    // "作成中"表示の削除
    Hide_BusyForm();

    // データベースのクローズ
    CloseDatabase_NNsh(bbsRef);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_subject_info                                         */
/*                                                   メッセージ情報の取得  */
/*-------------------------------------------------------------------------*/
static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR)
{
    Err        ret;
    UInt16     index;
    FormType  *frm;
    ListType  *lstP;

    // メッセージ情報の初期化
    *selTHR = 0;

    if (NNshGlobal->threadIdxH == 0)
    {
        // スレインデックスの取得ができない
        NNsh_DebugMessage(ALTID_ERROR, 
                          "the thread-index is not allocated.", "", 0);
        return (~errNone);
    }

    // 選択されているスレのリスト番号を取得する
    frm  = FrmGetActiveForm();
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
    *selTHR = LstGetSelection(lstP);
    NNshParam->openThreadIndex = *selTHR;

    // リスト番号からスレindexに変換する
    index = convertListIndexToMsgIndex(*selTHR);
    NNshParam->openMsgIndex    = index;

    // スレ情報を取得する
    ret = Get_Subject_Database(index, mesInfo);
    if (ret != errNone)
    {
        // レコードの取得に失敗
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()", "", ret);
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
    FormType  *frm;
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
    frm     = FrmGetActiveForm();
    lstP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO));
    *selBBS = LstGetSelection(lstP);

    // 選択されたBBSのリスト番号で、BBSのモードを判断する
    if ((*selBBS == NNSH_SELBBS_FAVORITE)||
        (*selBBS == NNSH_SELBBS_GETALL)||
        (*selBBS == NNSH_SELBBS_NOTREAD))
    {
        // お気に入り/スレ全取得/未読ありModeは、boardNickを使ってBBS情報を取得
        ret = Get_BBS_Database(mesInfo->boardNick, bbsInfo, &dummy);
    }
    else
    {
        // BBSのリスト番号より、BBS情報を取得する
        ret = Get_BBS_Info(*selBBS, bbsInfo);
    }

    // スレがひとつも存在しないボードかどうか確認
    if (bbsInfo->threadCnt == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_THREAD, "", 0);
        return (~errNone - 10);
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
        if (get_all_message() == true)
        {
            return (false);
        }
        // スレデータ取得できたときは、画面を表示に遷移する。
    }

    // BBSを(最後に)参照したところに変更する(お気に入り対応)
    NNshParam->lastBBS = selBBS;

    // 参照画面をオープンする
    NNshMain_Close(FRMID_MESSAGE);

    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_all_message                                          */
/*                                                     メッセージ全部取得  */
/*-------------------------------------------------------------------------*/
static Boolean get_all_message(void)
{
    Boolean             err;
    Err                 ret;
    Char                url[BUFSIZE], *ptr;
    UInt16              butID, selMES, selBBS, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (true);
    }

    // OFFLINEスレが選択された場合、更新不可を表示
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    // 同じスレがスレリストの上位にあるかチェックする
    if (check_same_thread(selMES, &mesInfo, NULL, &butID) == errNone)
    {
        //  同じスレがあった場合、DBの不整合を防止するため、警告を表示して
        // データの取得を行わない。
        NNsh_InformMessage(ALTID_WARN, MSG_THREADLIST_ALREADY,
                           mesInfo.threadTitle, 0);
        return (true);
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
            return (true);
        }
    }
    ////////////  メッセージ全部取得の処理  ///////////

    /** (VFS上で)ディレクトリを作成する **/
    (void) CreateDir_NNsh(mesInfo.boardNick);

    // メッセージ取得先の作成
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsInfo.boardURL);
    switch (bbsInfo.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // まちBBSからデータ取得(HTMLを読む)
        StrCat (url, "bbs/read.pl?BBS=");
        StrCat (url, mesInfo.boardNick);
        // boardNickの末尾にある '/'を削除
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo.threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileNameの拡張子を削除する
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '\0';
        NNsh_DebugMessage(ALTID_INFO, "MACHI BBS URL:", url, 0);
        break;

      case NNSH_BBSTYPE_2ch:
      default:
        // 2chからデータ取得(datを読む)
        StrCat (url, "dat/");
        StrCat (url, mesInfo.threadFileName);
        break;
    }

    // リスト番号からスレindexに変換する
    index = convertListIndexToMsgIndex(selMES);

    // ログ取得および記録処理
    ret = Get_AllMessage(url, bbsInfo.boardNick, mesInfo.threadFileName,
                         index, (UInt16) bbsInfo.bbsType);

    // 取得終了の表示(DEBUGオプション有効時のみ)
    NNsh_DebugMessage(ALTID_INFO, MSG_GET_MESSAGE_END,
                      mesInfo.threadTitle, ret);

    // スレリストの更新
    err = redraw_Thread_List(selBBS, selMES);

    // NNsi設定で、「取得後スレを表示」にチェックが入っていた場合は、画面表示
    if ((ret == errNone)&&(NNshParam->openAutomatic != 0))
    {
        // スレを表示する。
        display_message();
        err = true;
    }
    return (err);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_message                                           */
/*                                                     メッセージ差分取得  */
/*-------------------------------------------------------------------------*/
static Boolean update_message(void)
{
    Boolean             err;
    Err                 ret;
    Char                url[BUFSIZE], *ptr;
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

    //  Palm本体にログがあるが、VFSを使用する設定になっていた場合には、
    // 更新できない旨表示する
    if ((((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)&&
        (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM))||
        (((NNshParam->useVFS & NNSH_VFS_ENABLE) == 0)&&
         (mesInfo.msgState == FILEMGR_STATE_OPENED_VFS)))
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE_LOG,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    if (bbsInfo.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        //  まちBBSに対して差分取得を選択したが、現在サポートなし
        // 差分取得するかわりに全取得するか確認を行う
        butID = NNsh_ConfirmMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI,
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // Cancelを選択(何もしない)
            return (true);
        }
        // スレを(全部)取得する
        return (get_all_message());
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
        return (get_all_message());
    }

    ////////////  メッセージ差分取得の処理  ///////////

    /** (VFS上の)ディレクトリを作成する **/
    (void) CreateDir_NNsh(mesInfo.boardNick);

    // メッセージ取得先の作成
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsInfo.boardURL);
    switch (bbsInfo.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // まちBBSからデータ取得(HTMLを読む)
        StrCat (url, "bbs/read.pl?BBS=");
        StrCat (url, mesInfo.boardNick);
        // boardNickの末尾にある '/'を削除
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        }
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo.threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileNameの拡張子を削除する
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '\0';
        NNsh_DebugMessage(ALTID_INFO, "MACHI BBS URL:", url, 0);
        break;

      case NNSH_BBSTYPE_2ch:
      default:
        // 2chからデータ取得(datを読む)
        StrCat (url, "dat/");
        StrCat (url, mesInfo.threadFileName);
        break;
    }

    // リスト番号からスレindexに変換する
    index = convertListIndexToMsgIndex(selMES);

    // ログの取得および記録(追記)処理
    ret = Get_PartMessage(url, bbsInfo.boardNick, &mesInfo, index);

    // 取得終了の表示(DEBUGオプション有効時のみ)
    NNsh_DebugMessage(ALTID_INFO, MSG_GET_PARTMESSAGE_END,
                      mesInfo.threadTitle, ret);

    // スレリストの更新
    err = redraw_Thread_List(selBBS, selMES);

    // NNsi設定で、「取得後スレを表示」にチェックが入っていた場合は、画面表示
    if ((ret == errNone)&&(NNshParam->openAutomatic != 0))
    {
        // スレを表示する。
        display_message();
        err = true;
    }

    return (err);
}

/*-------------------------------------------------------------------------*/
/*   Function :   info_message                                             */
/*                                                    メッセージの情報表示 */
/*-------------------------------------------------------------------------*/
Boolean info_message(void)
{
    Err                 ret;
    Char                logBuf[BIGBUF], *ptr;
    UInt16              selBBS, selTHR;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // スレメッセージ情報を構築する
    MemSet (logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, mesInfo.threadTitle);
    StrCat (logBuf, "\n");
    if (bbsInfo.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // まちＢＢＳのＵＲＬを出力する
        StrCat (logBuf, bbsInfo.boardURL);
        StrCat (logBuf, "bbs/read.pl?BBS=");
        StrCat (logBuf, mesInfo.boardNick);
        // boardNickの末尾にある '/'を削除
        if (logBuf[StrLen(logBuf) - 1] == '/')
        {
            logBuf[StrLen(logBuf) - 1] = '\0';
        } 
        StrCat (logBuf, "&KEY=");
        StrCat (logBuf, mesInfo.threadFileName);
    }
    else
    {
        // 2chのＵＲＬを出力する
        StrCat (logBuf, bbsInfo.boardURL);

        // 2chのURLには、ボード名も含まれているので、その部分を削る。
        ptr = logBuf + StrLen(logBuf) - 2;
        while (*ptr != '/')
        {
            ptr--;
        }
        *ptr = '\0';
        StrCat (logBuf, "/test/read.cgi/");
        StrCat (logBuf, bbsInfo.boardNick);
        StrCat (logBuf, mesInfo.threadFileName);
    }
    // 末尾の".dat" もしくは ".cgi " は削除
    ptr = logBuf + StrLen(logBuf) - 1;
    while (*ptr != '.')
    {
        ptr--;
    }
    *ptr = '\0';

    StrCat (logBuf, "\n\n[");

    // メッセージの記録箇所を出力する
    switch (mesInfo.msgState)
    {
      case FILEMGR_STATE_OPENED_VFS:
        StrCat(logBuf, "VFS");
        break;
      case FILEMGR_STATE_OPENED_STREAM:
        StrCat(logBuf, "Palm");
        break;
      default:
        StrCat(logBuf, "???");
        break;
    }
    if ((mesInfo.msgAttribute & NNSH_MSGATTR_FAVOR) == NNSH_MSGATTR_FAVOR)
    {
        StrCat(logBuf, "<<CHK>>");
    }
    StrCat (logBuf, "] size:");
    NUMCATI(logBuf, mesInfo.fileSize);
    StrCat (logBuf, "(st:");
    NUMCATI(logBuf, mesInfo.state);
    StrCat (logBuf, ", msg#:");
    NUMCATI(logBuf, mesInfo.currentLoc);
    StrCat (logBuf, ")");

    // スレメッセージ情報を表示する
    NNsh_ErrorMessage(ALTID_INFO, logBuf, "", 0);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   launch_WebBrowser                                        */
/*                                                  WebBrowserでスレを開く */
/*-------------------------------------------------------------------------*/
Boolean launch_WebBrowser(void)
{
    Err                 ret;
    Char                logBuf[BIGBUF], *ptr;
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
    if (bbsInfo.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // まちＢＢＳのＵＲＬを出力する
        StrCat (logBuf, bbsInfo.boardURL);
        StrCat (logBuf, "bbs/read.pl?BBS=");
        StrCat (logBuf, mesInfo.boardNick);
        // boardNickの末尾にある '/'を削除
        if (logBuf[StrLen(logBuf) - 1] == '/')
        {
            logBuf[StrLen(logBuf) - 1] = '\0';
        } 
        StrCat (logBuf, "&KEY=");
        StrCat (logBuf, mesInfo.threadFileName);

    }
    else
    {
        // 2chのＵＲＬを出力する
        StrCat (logBuf, bbsInfo.boardURL);

        // 2chのURLには、ボード名も含まれているので、その部分を削る。
        ptr = logBuf + StrLen(logBuf) - 2;
        while (*ptr != '/')
        {
            ptr--;
        }
        *ptr = '\0';
        StrCat (logBuf, "/test/read.cgi/");
        StrCat (logBuf, bbsInfo.boardNick);
        StrCat (logBuf, mesInfo.threadFileName);
    }
    // 末尾の".dat" もしくは ".cgi " は削除
    ptr = logBuf + StrLen(logBuf) - 1;
    while (*ptr != '.')
    {
        ptr--;
    }
    *ptr = '\0';

    // WebBrowserで開くとき、最新５０だけ開くようにする
    if (NNshParam->browseMesNum != 0)
    {
        if (mesInfo.bbsType == NNSH_BBSTYPE_MACHIBBS)
        {
            StrCat(logBuf, "&LAST=");
        }
        else
        {
            StrCat(logBuf, "/l");
        }
        NUMCATI(logBuf, NNshParam->browseMesNum);
    }

    // 表示するスレURLをデバッグ表示
    NNsh_DebugMessage(ALTID_INFO, "OPEN URL :", logBuf, 0);

    // NetFrontを起動する(NNsi終了後に起動)
    (void) WebBrowserCommand(NNshGlobal->browserCreator, false, 0, 
                             NNshGlobal->browserLaunchCode, logBuf, NULL);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_newarrival_message                                */
/*                                                  新着メッセージ取得処理 */
/*-------------------------------------------------------------------------*/
static Boolean update_newarrival_message(void)
{
    Err                  ret;
    Boolean              penState;
    UInt16               selMES, selBBS, index, loop, savedParam, count, idx;
    UInt16               x, y;
    Char                *url, *bbsLbl;
    FormType            *frm;
    ListType            *lstP;
    ControlType         *ctlP;
    NNshBoardDatabase   *bbsInfo;
    NNshSubjectDatabase *mesInfo;

    // ワーク領域の確保(3個所)
    url = MemPtrNew(BUFSIZE);
    if (url == NULL)
    {
        return (false);
    }
    bbsInfo = MemPtrNew(sizeof(NNshBoardDatabase));
    if (bbsInfo == NULL)
    {
        MemPtrFree(url);
        return (false);
    }
    mesInfo = MemPtrNew(sizeof(NNshSubjectDatabase));
    if (bbsInfo == NULL)
    {
        MemPtrFree(url);
        MemPtrFree(bbsInfo);
        return (false);
    }

    // 確保したワーク領域を初期化する
    MemSet(url,     BUFSIZE,                     0x00);
    MemSet(bbsInfo, sizeof(NNshBoardDatabase),   0x00);
    MemSet(mesInfo, sizeof(NNshSubjectDatabase), 0x00);

    // 現在のメッセージ情報を取得する
    ret = get_message_Info(mesInfo, &selMES, bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    // OFFLINEスレが選択されていた場合は、更新不可を表示
    if (selBBS == NNSH_SELBBS_OFFLINE)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo->threadTitle, 0);
        goto FUNC_END;
    }

    ////////////  メッセージ差分取得の処理  ///////////

    // 新着メッセージ確認処理中は、確認メッセージを「表示しない」設定にする。
    savedParam = NNshParam->confirmationDisable;
    NNshParam->confirmationDisable = 1;

    // "スレ検索中"を表示する
    Show_BusyForm(MSG_SEARCHING_TITLE);

    // 現在の一覧スレ数を取得する
    Get_Thread_Count(selBBS, &count);
    for (loop = 0; loop < count; loop++)
    {
        index = convertListIndexToMsgIndex(loop);
        ret = Get_Subject_Database(index, mesInfo);
        if (ret != errNone)
        {
            // レコードの取得に失敗
            goto NEXT_STEP;
        }

        //   １度でもメッセージ取得済みのスレでかつ、メッセージ数の上限に
        // 到達していないスレで、まちＢＢＳでなかったら、差分取得を実施する
        if (mesInfo->state != NNSH_SUBJSTATUS_NOT_YET)
        {
            if ((mesInfo->maxLoc < NNSH_MESSAGE_LIMIT)&&
                (mesInfo->bbsType != NNSH_BBSTYPE_MACHIBBS))
            {
                //  お気に入り/取得済み全て/未読ありだったら、
                // ＢＢＳ情報を取得する
                if ((selBBS == NNSH_SELBBS_FAVORITE)||
                    (selBBS == NNSH_SELBBS_GETALL)||
                    (selBBS == NNSH_SELBBS_NOTREAD))
                {
                    idx = 0;
                    ret = Get_BBS_Database(mesInfo->boardNick, bbsInfo, &idx);
                    if (ret != errNone)
                    {
                        // レコードの取得に失敗
                        NNsh_DebugMessage(ALTID_WARN, "BBSINFO Get Fail Nick:",
                                          mesInfo->boardNick, 0);
                        goto NEXT_STEP;
                    }
                    if (idx == 0)
                    {
                        // OFFLINEスレなので、新着確認は行わない。
                        goto NEXT_STEP;
                    }
                }

                // メッセージ取得先の作成
                MemSet (url, BUFSIZE, 0x00);
                StrCopy(url, bbsInfo->boardURL);
                StrCat (url, "dat/");
                StrCat (url, mesInfo->threadFileName);
 
                // 一時的にBUSYウィンドウを消去する
                Hide_BusyForm();

                // ログの取得および記録(追記)処理
                ret = Get_PartMessage(url, bbsInfo->boardNick, mesInfo, index);

                // BUSYウィンドウを復活させる
                Show_BusyForm(MSG_SEARCHING_TITLE);

                //  ログ取得処理が中止された or 致命的エラー発生時には
                // 新着メッセージの確認を中止する。
                if (ret == ~errNone)
                {
                    // 処理を中止する旨、画面表示する
                    NNsh_InformMessage(ALTID_INFO, MSG_ABORT_MESSAGE, "", 0);
                    break;
                }
            }
        }
NEXT_STEP:
        // ペンの状態を拾う(画面タップされているか確認する)
        EvtGetPen(&x, &y, &penState);
        if (penState == true)
        {
            // ペンがダウンされていたら、中止するか確認を行う
            if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                  MSG_CONFIRM_ABORT_GET, "", 0) == 0)
            {
                // OKが押された、breakする
                break;
            }
        }
    }
    Hide_BusyForm();

    // 確認メッセージ省略のパラメータを元に戻す
    NNshParam->confirmationDisable = savedParam;

    // 「新着確認」後に未読一覧を表示する
    if (NNshParam->autoOpenNotRead != 0)
    {
        // 選択されているBBSのリスト番号を変更する
        selBBS = NNSH_SELBBS_NOTREAD;

        frm    = FrmGetActiveForm();
        lstP   = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO));
        LstSetSelection(lstP, selBBS);

        // BBS表示タブを変更する
        NNshParam->lastBBS = selBBS;
        bbsLbl = NNshGlobal->bbsTitles;
        for (x = 0; x < NNshParam->lastBBS; x++)
        {
            bbsLbl = bbsLbl + StrLen(bbsLbl) + 1;
        }
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, POPTRID_BBSINFO));
        CtlSetLabel(ctlP, bbsLbl);
    }

    // リスト情報を更新する
    ret = redraw_Thread_List(selBBS, 0);

FUNC_END:
    // 関数の終了
    MemPtrFree(url);
    MemPtrFree(bbsInfo);
    MemPtrFree(mesInfo);

    // エラーが発生したときは、回線を切らずに終了する
    if (ret != errNone)
    {
        return (false);
    }
    if (NNshParam->disconnArrivalEnd != 0)
    {
        // 「新着確認」終了時に回線切断
        savedParam = NNshParam->confirmationDisable;
        NNshParam->confirmationDisable = 1;
        NNshNet_LineHangup();
        NNshParam->confirmationDisable = savedParam;
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
        return (get_all_message());
    }

    // 取得済みなら差分取得
    return (update_message());
}

/*-------------------------------------------------------------------------*/
/*   Function :   redraw_Thread_list                                       */
/*                                                      スレ一覧表示の更新 */
/*-------------------------------------------------------------------------*/
static Boolean redraw_Thread_List(UInt16 bbsIndex, UInt16 threadIndex)
{
    Err               ret;
    FormType         *frm;
    UInt32            dataSize, margin;
    UInt16            cnt, threadCnt;

    // スレリストのタップ情報を初期化
    NNshGlobal->tappedPrev = -1;
    frm = FrmGetActiveForm();

    // 確保済み領域を開放する。
    if (NNshGlobal->threadTitles != NULL)
    {
        MemHandleUnlock(NNshGlobal->msgTTLStringH);
        NNshGlobal->threadTitles = NULL;
    }
    MEMFREE_HANDLE(NNshGlobal->msgTTLStringH);

    // 一回リストを消去する
    NNshWinSetListItems(frm, LSTID_SELECTION_THREAD, NNshGlobal->dummyBuffer,
                        1, 0, &(NNshGlobal->threadTitleH));
    NNshWinClearList(frm, LSTID_SELECTION_THREAD);

    // "スレ一覧作成中" の表示
    Show_BusyForm(MSG_READ_THREAD_WAIT);

    // 該当するスレッド数を求める。
    ret = Get_Thread_Count(bbsIndex, &threadCnt);
    if (ret == errNone)
    {
        //   スレ数から、リスト領域の確保サイズを求める(8は、タイトルの前に
        // 表示するMSG数部分のバイト数)
        dataSize = (NNshParam->titleListSize + 8) * (threadCnt + 1);
        margin   = MARGIN;
        if (dataSize > (NNSH_WORKBUF_MAX - margin))
        {
            dataSize = NNSH_WORKBUF_MAX - margin;
	}
        NNshGlobal->msgTTLStringH = MemHandleNew(dataSize + margin);
        if (NNshGlobal->msgTTLStringH != 0)
        {
            NNshGlobal->threadTitles =
                                      MemHandleLock(NNshGlobal->msgTTLStringH);
            if (NNshGlobal->threadTitles != NULL)
            {
                // スレ一覧を作成する
                MemSet(NNshGlobal->threadTitles, dataSize + margin, 0x00);
                (void) Create_SubjectList(bbsIndex, threadCnt,
                                     NNshGlobal->threadTitles, dataSize, &cnt);
                // 次のステップへ
                goto NEXT_STEP;
            }
        }
        // リスト領域の確保に失敗した(その旨を表示する)
        NNsh_DebugMessage(ALTID_WARN, MSG_SUBJECT_LISTFAIL, " SIZE:",dataSize);
    }

NEXT_STEP:
    // スレ名の取得に失敗した場合には、ダミーのリストを作成する
    if (cnt == 0)
    {
        // リスト領域が確保済みだった場合には、一旦開放する。
        if (NNshGlobal->threadTitles != NULL)
        {
            MemHandleUnlock(NNshGlobal->msgTTLStringH);
            NNshGlobal->threadTitles = NULL;
        }
        MEMFREE_HANDLE(NNshGlobal->msgTTLStringH);
        NNshGlobal->msgTTLStringH = MemHandleNew(MAX_THREADNAME + MARGIN);
        if (NNshGlobal->msgTTLStringH != 0)
        {
            NNshGlobal->threadTitles =
                                      MemHandleLock(NNshGlobal->msgTTLStringH);
            if (NNshGlobal->threadTitles != NULL)
            {
                MemSet (NNshGlobal->threadTitles,
                        (MAX_THREADNAME + MARGIN), 0x00);
                StrCopy(NNshGlobal->threadTitles, MSG_SUBJECT_DEFAULT);
                cnt = 1;
            }
        }

        // インデックスの初期化
        if (NNshGlobal->threadIdxH != 0)
        {
            MemHandleFree(NNshGlobal->threadIdxH);
            NNshGlobal->threadIdxH = 0;
        }
    }

    // "スレ一覧作成中"の表示削除
    Hide_BusyForm();

    // スレ一覧を表示する
    if ((frm != NULL)&&(NNshGlobal->threadTitles != NULL))
    {
        NNshWinSetListItems(frm, LSTID_SELECTION_THREAD,
                            NNshGlobal->threadTitles, cnt, threadIndex,
                            &(NNshGlobal->threadTitleH));
        NNshGlobal->threadCount = cnt;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   delete_notreceived_message                               */
/*                                                    未受信スレ名削除処理 */
/*-------------------------------------------------------------------------*/
Boolean delete_notreceived_message(void)
{
    Err                   ret;
    UInt16                selMES, selBBS, cnt;
    FormType             *frm;
    ListType             *lstP;
    NNshSubjectDatabase   mesInfo;
    NNshBoardDatabase     bbsInfo;

    // 選択されているBBSのリスト番号を取得する
    frm    = FrmGetActiveForm();
    lstP   = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO));
    selBBS = LstGetSelection(lstP);

    // 「お気に入り」一覧は、削除できない。
    if ((selBBS == NNSH_SELBBS_FAVORITE)||
        (selBBS == NNSH_SELBBS_GETALL)||
        (selBBS == NNSH_SELBBS_NOTREAD))
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

    // 現在保持するスレ数をBBS-DBに反映させる
    Get_BBS_Info   (selBBS, &bbsInfo);
    bbsInfo.threadCnt = cnt;
    Update_BBS_Info(selBBS, &bbsInfo);

    // スレ数が現在の位置より小さい場合には、末尾のスレにカーソルをあわせる
    if (selMES >= cnt)
    {
        selMES = cnt - 1;    
    }

    // リスト情報を更新する
    return (redraw_Thread_List(selBBS, selMES));
}

/*-------------------------------------------------------------------------*/
/*   Function :   delete_message                                           */
/*                                                           スレ削除処理  */
/*-------------------------------------------------------------------------*/
Boolean delete_message(void)
{
    Err                 ret;
    UInt16              selMES, selBBS, butID, index;
    Char                fileName[MAXLENGTH_FILENAME], *area;
    DmOpenRef           dbRef;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // メッセージ情報を取得する
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // スレを本当に削除するか確認する
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_DELETE_MSGCONFIRM, 
                                mesInfo.threadTitle, 0);
    if (butID != 0)
    {
        // 削除キャンセル
        return (false);
    }

    // スレ情報DBのオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生
        return (~errNone);
    }

    // スレファイル(本体のログファイル)を削除
    MemSet (fileName, sizeof(fileName), 0x00);
    if (selBBS != NNSH_SELBBS_OFFLINE)
    {
        // OFFLINEログのときは、boardNickを付加しない
        StrCopy(fileName, mesInfo.boardNick);
    }
    else
    {
        // OFFLINEスレがVFSに格納されている場合には、ディレクトリを付加する。
        if ((NNshParam->useVFS & NNSH_VFS_USEOFFLINE) != 0)
        {
            // ログベースディレクトリを取得する
            if ((NNshGlobal->logPrefixH == 0)||
                ((area = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
            {
                StrCopy(fileName, LOGDATAFILE_PREFIX);
            }
            else
            {
                StrCopy(fileName, area);
                MemHandleUnlock(NNshGlobal->logPrefixH);
            }

            GetSubDirectoryName(mesInfo.dirIndex, &fileName[StrLen(fileName)]);
        }
    }
    StrCat (fileName, mesInfo.threadFileName);

    if (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        // Palm本体上のログを削除する
        ret = DeleteFile_NNsh(fileName, NNSH_VFS_DISABLE);
    }
    else
    {
        // VFS上のログを削除する
        ret = DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }
    if((ret != errNone)&&(ret != fileErrNotFound)&&(ret != vfsErrFileNotFound))
    {
        NNsh_InformMessage(ALTID_ERROR, "File Delete: ", fileName, ret);
    }

    // リスト番号からスレindexに変換する
    index = convertListIndexToMsgIndex(selMES);

    // スレインデックスファイルを削除してクローズ
    (void) DeleteRecordIDX_NNsh(dbRef, index);
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    // 現在保持するスレ数をBBS-DBに反映させる
    Get_BBS_Info   (selBBS, &bbsInfo);
    bbsInfo.threadCnt--;
    Update_BBS_Info(selBBS, &bbsInfo);

    // カーソルを１つ前のメッセージに移動させる。
    if (selMES != 0)
    {
        selMES--;    
    }

    return (redraw_Thread_List(selBBS, selMES));
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_subject_txt                                          */
/*                                      スレ名一覧(subject.txt)を取得する  */
/*-------------------------------------------------------------------------*/
static Boolean get_subject_txt(void)
{
    Err       ret;
    UInt16    selBBS;
    FormType *frm;
    ListType *lstP;

    frm  = FrmGetActiveForm();
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO)); 
    selBBS  = LstGetSelection(lstP);

    switch (selBBS)
    {
      case NNSH_SELBBS_FAVORITE:
      case NNSH_SELBBS_GETALL:
      case NNSH_SELBBS_NOTREAD:
        // "お気に入り"、"取得済み全て"、"未読あり"表示中には、受け付けない。
        return (false);
        break;

      case NNSH_SELBBS_OFFLINE:
        // オフラインスレの検索を行う
        create_offline_database();
        redraw_Thread_List(selBBS, 0);
        FrmDrawForm(frm);
        break;

      default:
        // スレ一覧を取得する
        ret = NNsh_GetSubjectList(selBBS);
        if (ret == errNone)
        {  
            // スレ一覧の取得に成功したときは、スレ一覧を再表示する
            redraw_Thread_List(selBBS, 0);
            FrmDrawForm(frm);
        }
        break;
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   popEvt_ThreadMain                                        */
/*                                          リスト選択イベント処理ルーチン */
/*-------------------------------------------------------------------------*/
static Boolean popEvt_ThreadMain(EventType *event)
{
    switch (event->data.popSelect.listID)
    {
      case LSTID_BBSINFO:
        // LSTID_BBSINFOのpopSelectEventだったなら、スレ情報を更新する
        NNshParam->lastBBS = event->data.popSelect.selection;
        return (redraw_Thread_List(NNshParam->lastBBS, 0));
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   keyDownEvt_ThreadMain                                    */
/*                                                        キー入力時の処理 */
/*-------------------------------------------------------------------------*/
static Boolean keyDownEvt_ThreadMain(EventType *event)
{
    Boolean   ret = false;
    UInt16    keyCode;
    Int16     curr, max, itms;
    FormType *frm;
    ListType *lstP;

    // 現在のリストオブジェクト情報を取得
    frm  = FrmGetActiveForm();
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
    curr = LstGetSelection(lstP);
    max  = LstGetNumberOfItems(lstP);
    itms = LstGetVisibleItems(lstP);

    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // 「下」(ジョグダイヤル下)を押した時の処理
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
        // リスト１つ下
        if (curr < (max - 1))
        {
            curr++;
            LstSetSelection(lstP, curr);
            ret = true;
	}
        break;

      // 「上」(ジョグダイヤル上)を押した時の処理
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
        // リスト１つ上
        if (curr > 0)
        {
            curr--;
            LstSetSelection(lstP, curr);
            ret = true;
        }
        break;

      case chrCapital_T:  // Tキーの入力
      case chrSmall_T:
        // リストの先頭へ移動
        curr = 0;
        LstSetSelection(lstP, curr);
        LstSetTopItem  (lstP, curr);
        ret = true;
        break;

      case chrCapital_B:    // Bキーの入力
      case chrSmall_B:
        curr = max - 1;
        LstSetSelection(lstP, curr);
        LstSetTopItem  (lstP, curr);
        ret = true;
        break;

      // ジョグダイヤル押し回し(上)の処理(1ページ上)
      case vchrJogPushedUp:
        // 一時状態を(回転中に)設定する
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGUP;
#endif
        /** not break!!  **/

      case chrRecordSeparator:   // カーソルキー(上)押下時の処理
      case chrCapital_P:         // Pキーの入力
      case chrSmall_P:           // pキーの入力
        curr = (curr < (itms - 1)) ? 0 : curr - (itms - 1);
        LstSetSelection(lstP, curr);
        LstSetTopItem  (lstP, curr);
        ret = true;
        break;

      // ジョグダイヤル押し回し(下)の処理(1ページ下)
      case vchrJogPushedDown:
        // 一時状態を(回転中に)設定する
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGDOWN;
#endif
        /** not break!!  **/

      case chrUnitSeparator:  // カーソルキー(下)押下時の処理
      case chrCapital_N:      // Nキーの入力
      case chrSmall_N:        // nキーの入力
        curr = ((max - 1) < (curr + itms)) ? (max - 1) : (curr + itms);
        LstSetSelection(lstP, curr);
        break;

      // JOG Push選択時の処理(リスト選択)
      case vchrJogRelease:
#ifdef USE_CLIE
        if (NNshGlobal->tempStatus == NNSH_TEMPTYPE_CLEARSTATE)
        {
            // スレ参照
            return (display_message());
        }
        // 一時状態をクリアする
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
#endif
        break;

      // バックボタン/ESCキー/BSキーの処理
      case vchrJogBack:
      case chrBackspace:
      case chrEscape:
        if (NNshParam->jogBackBtnAsMenu != 0)
        {
            // メニューを開く
            return (open_menu());
        }
        else
        {
            // スレ情報表示
            info_message();
        }
        break;

      // HandEra JOG Push選択/Enterキー入力時の処理(リスト選択)
      case chrCarriageReturn:
      case chrLineFeed:
        // スレ参照
        return (display_message());
        break;

      // スペースキー入力
      case chrSpace:
        // 新着MSG確認
        return (update_newarrival_message());

      // Qキー入力
      case chrCapital_Q:
      case chrSmall_Q:
        NNshNet_LineHangup();
        break;

      default:
        // その他
        break;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlSelEvt_ThreadMain                                     */
/*                                                      ボタン押下時の処理 */
/*-------------------------------------------------------------------------*/
static Boolean ctlSelEvt_ThreadMain(EventType *event)
{
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_MSGCHK:
        // 新着MSG確認
        return (update_newarrival_message());
        break;

      case BTNID_OPEN_THREAD:
        // スレ参照
        return (display_message());
        break;

      case BTNID_UPDATE_THREAD:
        // メッセージ取得(差分取得/全部取得は自動判断)
        return (update_getall_message());
        break;

      case BTNID_LIST_THREAD:
        // 新規スレ取得(SUBJECT.TXT取得)
        return (get_subject_txt());
        break;

      case BTNID_DELETE_THREAD:
        // スレ削除
        return (delete_message());
        break;

      case BTNID_DISCONNECT:
        // 回線切断
        NNshNet_LineHangup();
        break;

      case CHKID_SELECT_GETMODE:
        // スレ分割取得/全部取得の切り替え(パラメータに反映させる)
        UpdateParameter(FrmGetActiveForm(),
                        CHKID_SELECT_GETMODE, &(NNshParam->enablePartGet));
        if (NNshParam->enablePartGet == 0)
        {
            // スレ一括取得モードに切り替えたことを通知する
            NNsh_InformMessage(ALTID_INFO, MSG_CHANGE_ALLGET_MODE, "", 0);
        }
        else
        {
            // スレ分割取得モードに切り替えたことを通知する
            NNsh_InformMessage(ALTID_INFO, MSG_CHANGE_PARTGET_MODE, "", 0);
        }
        break;
  
      default:
        // その他
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   lstSelEvt_ThreadMain                                     */
/*                                          リスト選択イベント処理ルーチン */
/*-------------------------------------------------------------------------*/
static Boolean lstSelEvt_ThreadMain(EventType *event)
{
    if (event->data.lstSelect.listID == LSTID_SELECTION_THREAD)
    {
        if (event->data.lstSelect.selection == NNshGlobal->tappedPrev)
        {
            // ダブルタップを検出(その場合はスレ参照)
            return (display_message());
        }
        else
        {
            NNshGlobal->tappedPrev = event->data.lstSelect.selection;
        }
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Handler_ThreadMain                                       */
/*                                          スレ管理画面用イベントハンドラ */
/*=========================================================================*/
Boolean Handler_ThreadMain(EventType *event)
{
    Boolean   ret;
#ifdef USE_HANDERA
    FormType *frm;
#endif

    ret = false;
    switch (event->eType)
    {
      case keyDownEvent:
        return (keyDownEvt_ThreadMain(event));
        break;

      case fldEnterEvent:
        break;

      case ctlSelectEvent:
        return (ctlSelEvt_ThreadMain(event));
        break;

      case frmOpenEvent:
        break;

      case popSelectEvent:
        return (popEvt_ThreadMain(event));
        break;

      case lstSelectEvent:
        return (lstSelEvt_ThreadMain(event));
        break;

      case lstEnterEvent:
        break;

      case lstExitEvent:
        break;

      case menuEvent:
        return (menuEvt_ThreadMain(event));
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent :
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
        break;
#endif  // #ifdef USE_HANDERA

      case menuOpenEvent:
        break;

      case sclRepeatEvent:
        break;

      case sclExitEvent:
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
    Char        *bbsLbl;
    ControlType *ctlP;

    // 初回オープン時、オフラインスレの検索
    if ((NNshParam->offChkLaunch != 0)&&(NNshGlobal->bbsTitles == NULL))
    {
        create_offline_database();
    }

    // BBS一覧の作成(板一覧がなければ、BBS一覧を取得するよう促す)
    ret = create_BBS_INDEX(&(NNshGlobal->bbsTitles), &lp);
    if (ret != errNone)
    {
        // 作成失敗、フォーム情報は更新しない
        return;
    }

    // NNshParam->lastBBS(前回選択していたBBS名)が変なとき(一覧の先頭にする)
    if (lp < NNshParam->lastBBS)
    {
        NNshParam->lastBBS = 0;
    }

    // BBS一覧のリスト状態を反映する。
    NNshWinSetListItems(frm, LSTID_BBSINFO, NNshGlobal->bbsTitles, lp,
                        NNshParam->lastBBS, &(NNshGlobal->bbsTitleH));

    // ポップアップトリガのラベルを(リスト状態に合わせ)更新する
    bbsLbl = NNshGlobal->bbsTitles;
    for (lp = 0; lp < NNshParam->lastBBS; lp++)
    {
        bbsLbl = bbsLbl + StrLen(bbsLbl) + 1;
    }
    if (bbsLbl != NULL)
    {
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, POPTRID_BBSINFO));
        CtlSetLabel(ctlP, bbsLbl);
    }

    // スレ分割取得/全部取得の設定値を反映させる
    SetControlValue(frm, CHKID_SELECT_GETMODE, &(NNshParam->enablePartGet));
                        
    // スレ一覧の表示を更新
    redraw_Thread_List(NNshParam->lastBBS, NNshParam->openThreadIndex);

    // 開いたフォームIDを設定する(しおり設定中には、lastFrmIDは変更しない)
    if ((NNshParam->useBookmark != 0)&&(NNshParam->bookMsgNumber != 0))
    {
        // しおり設定中なので、何もしない
    }
    else
    {
        NNshParam->lastFrmID = FRMID_THREAD;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   menuEvt_ThreadMain                                       */
/*                                      スレ管理画面用メニュー処理ルーチン */
/*-------------------------------------------------------------------------*/
static Boolean menuEvt_ThreadMain(EventType *event)
{
    Boolean             err = false;
    Int16               max;
    FormType           *frm;
    ListType           *lstP;
    Char               *buffer;
#ifdef USE_GLUE
    UInt16              selM, selB;
#endif
#ifdef USE_HANDERA
    VgaScreenModeType   handEraScreenMode;     // スクリーンモード
    VgaRotateModeType   handEraRotateMode;     // 回転モード
#endif

    frm  = FrmGetActiveForm();
    switch (event->data.menu.itemID)
    {
      // バージョンの表示(ここだけ直接 FrmCustomAlert() を呼び出す)
      case MNUID_SHOW_VERSION:
        buffer = MemPtrNew(BUFSIZE);
        if (buffer == NULL)
        {
            return (false);
        }
        MemSet (buffer, BUFSIZE, 0x00);
        StrCopy(buffer, "NNsi\n");
        StrCat (buffer, SOFT_VERSION);
        StrCat (buffer, "\n");
        StrCat (buffer, SOFT_REVISION);
        StrCat (buffer, "\n(");
        StrCat (buffer, SOFT_DATE);
#ifdef SMALL
        StrCat (buffer, ", OS31NNsi.prc");
#endif
#ifdef STD
        StrCat (buffer, ", NNsiSTD.prc");
#endif
#ifdef OFFLINE_DEBUG
        StrCat (buffer, "[OFFLINE DEBUG]");
#endif
        FrmCustomAlert(ALTID_INFO, buffer, ")", "");
        MemPtrFree(buffer);
        buffer = NULL;
        err    = true;
        break;

        // NNsi設定画面を開く
      case MNUID_CONFIG_NNSH:
        NNshMain_Close(FRMID_CONFIG_NNSH);
        break;

        // NNsi設定-2画面を開く
      case MNUID_NNSI_SETTING2:
        NNshMain_Close(FRMID_NNSI_SETTING2);
        break;

        // NNsi設定-3画面を開く
      case MNUID_NNSI_SETTING3:
        NNshMain_Close(FRMID_NNSI_SETTING3);
        break;

        // NNsi設定-4画面を開く
      case MNUID_NNSI_SETTING4:
        NNshMain_Close(FRMID_NNSI_SETTING4);
        break;

        // 回線切断
      case MNUID_NET_DISCONN:
        NNshNet_LineHangup();
        break;

        // 新着メッセージ確認
      case MNUID_UPDATE_CHECK:
        err = update_newarrival_message();
        break;

        // 板情報更新(取得)
      case MNUID_UPDATE_BBS:
        err = GetBBSList(NNshParam->bbs_URL);
        if (err == false)
        {
            break;
        }
        /** not break; (続けて使用板選択も行う) **/

        // 使用板選択画面を開く
      case MNUID_SELECT_BBS:
        NNshMain_Close(FRMID_MANAGEBBS);
        break;

        // 新規スレ取得(SUBJECT.TXT取得)
      case MNUID_GET_NEWMESSAGE:
        err = get_subject_txt();
        break;

        // スレ情報
      case MNUID_INFO_MESSAGE:
        err = info_message();
        break;

        // 未取得スレ削除
      case MNUID_DEL_THREADLIST:
        err = delete_notreceived_message();
        break;

        // スレ削除
      case MNUID_DELETE_MESSAGE:
        err = delete_message();
        break;

        // メッセージ全部取得(再取得)
      case MNUID_GET_ALLMESSAGE:
        err = get_all_message();
        break;

        // メッセージ差分取得
      case MNUID_UPDATE_MESSAGE:
        err = update_message();
        break;

        // スレ参照
      case MNUID_OPEN_MESSAGE:
        err = display_message();
        break;

        // フォント変更
      case MNUID_FONT:
        NNshParam->currentFont = FontSelect(NNshParam->currentFont);
#ifdef USE_GLUE
        LstGlueSetFont(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,
                            LSTID_SELECTION_THREAD)), NNshParam->currentFont);

        // フォント変更後の表示は、リストを再作成して表示させる
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
        selM = LstGetSelection(lstP);
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO));
        selB = LstGetSelection(lstP);
        redraw_Thread_List(selB, selM);
#endif
        break;

#ifdef USE_HANDERA
        // HandEra 画面回転 //
      case MNUID_HANDERA_ROTATE:
        if(NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
            VgaSetScreenMode(screenMode1To1,
                             VgaRotateSelect(handEraRotateMode));
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
        else
        {
            // "現在サポートしていません" 表示を行う
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;
#endif  // #ifdef USE_HANDERA

      case MNUID_GET_MESSAGENUM:
        // スレ番指定のメッセージ取得
        err = get_MessageFromMesNum();
        break;

      case MNUID_SEARCH_TITLE:
        // タイトル検索(文字列の設定)
        err = set_SearchTitleString();
        if (err != true)
        {
            // 検索文字列が設定されなかった(終了する) 
            break;
        }
        // not break! (続けてスレタイ検索を実行!)
      case MNUID_SEARCH_NEXT:
        // スレタイ検索処理(後方向)
        err = search_NextTitle(NNSH_SEARCH_FORWARD);
        break;

      case MNUID_SEARCH_PREV:
        // スレタイ検索処理(前方向)
        err = search_NextTitle(NNSH_SEARCH_BACKWARD);
        break;

      case MNUID_MOVE_TOP:
        // スレタイトルの先頭へ移動
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
        LstSetSelection(lstP, 0);
        LstSetTopItem  (lstP, 0);
        break;

      case MNUID_MOVE_BOTTOM:
        // スレタイトルの先頭へ移動
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
        max  = LstGetNumberOfItems(lstP);
        LstSetSelection(lstP, max - 1);
        LstSetTopItem  (lstP, max - 1);
        break;

      case MNUID_COPY_READONLY:
        // スレを参照専用ログへコピーする
        err = copy_to_readOnly();
        if ((err == true)&&(NNshParam->copyDelReadOnly != 0))
        {
            // コピーが成功したとき、コピー元ファイルを削除する
            err = delete_message();
        }
        break;

      case MNUID_OS5_LAUNCH_WEB:
        if (NNshGlobal->browserCreator != 0)
        {
            // WebBrowserで開く
            launch_WebBrowser();
        }
        else
        {
            // "現在サポートしていません" 表示を行う
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;

      case MNUID_BACKUP_DATABASE:
        if (((NNshParam->useVFS) & (NNSH_VFS_ENABLE)) == NNSH_VFS_ENABLE)
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

      default:
        // その他(ありえない)
        break;
    }
    return (err);
}
