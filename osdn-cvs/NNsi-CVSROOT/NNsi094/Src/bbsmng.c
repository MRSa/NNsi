/*============================================================================*
 *  FILE: 
 *     bbsmng.c
 *
 *  Description: 
 *     BBS list manage form functions for NNsh.
 *
 *===========================================================================*/
#define BBSMNG_C
#include "local.h"

// BBS取得関数群
static UInt16  getBoardURL    (Char *url, Char *nick, Char *top, Char *bottom);
static void    parseBoard     (DmOpenRef dbRef, Char *top, UInt32 *readSize);
static void    updateBoardList(void);

// 使用BBS選択関数群
static void    closeForm_SelectBBS  (void);
static void    setBBSListItem       (void);
static void    setAllBBS_Selection  (UInt16     value);
static Boolean ctlSelEvt_SelectBBS  (EventType *event);
static Int16   ctlRepEvt_SelectBBS  (EventType *event);
static Int16   keyDownEvt_SelectBBS (EventType *event);
static void    set_Selection_Info   (UInt16     pageNum);
static void    display_BBS_Selection(UInt16     pageNum);

///////////////////////////////////////////////////////////////////////////////
//////////   BBS名取得用関数群
///////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------------*/
/*   Function :   getBoardURL                                              */
/*                                                                         */
/*                                              板名称かどうかの判断と取得 */
/*-------------------------------------------------------------------------*/
static UInt16 getBoardURL(Char *url, Char *nick, Char *top, Char *bottom)
{
    Char   *ptr, *ptr2, *ptr3, *chk;
    
    ptr  = StrStr(top, "http://");
    ptr2 = ptr + 7; // 7 == StrLen("http://");

#ifdef USE_STRSTR
    ptr2 = StrStr(ptr2, "/");
#else
    while ((*ptr2 != '/')&&(*ptr2 != '\0'))
    {
        ptr2++;
    }
#endif
    ptr2 = ptr2 + 1; // 1 == StrLen("/");
#ifdef USE_STRSTR
    ptr3 = StrStr(ptr2, "/");
#else
    ptr3 = ptr2;
    while ((*ptr3 != '/')&&(*ptr3 != '\0'))
    {
        ptr3++;
    }
#endif
    ptr3 = ptr3 + 1; // 1 == StrLen("/");
#ifdef USE_STRSTR
    if ((ptr3 == NULL)||(bottom < ptr3))
#else
    if ((bottom < ptr3)||(ptr3 == '\0'))
#endif
    {
        // "http://ほにゃらら/ほへほへ/" 形式ではないので、板ではないと判断
        return (NNSH_BBSTYPE_ERROR);
    }

    // 2ちゃんねるのボードか、BBSPINKのボード以外は板とはみなさない
    chk = StrStr(top, "2ch.net");
    if ((chk != NULL)&&(chk < bottom))
    {
#if 0   // あまりにも解析に時間がかかりすぎるのではずす
        chk = StrStr(top, "www.2ch.net");
        if ((chk != NULL)&&(chk < bottom))
        {
            // ただし、"www.2ch.net" だと、板とはみなさないようにする
            return (NNSH_BBSTYPE_ERROR);
        }
#endif
        StrNCopy(url,  ptr,  (ptr3 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_2ch);
    }
    chk = StrStr(top, "bbspink.com");
    if ((chk != NULL)&&(chk < bottom))
    {
        StrNCopy(url,  ptr,  (ptr3 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_2ch);
    }

    // まちBBSのボードも大丈夫にしてみる。。。
    chk = StrStr(top, "machibbs.com");
    if ((chk != NULL)&&(chk < bottom))
    {
        // まちBBSのURLは、"http://(ホスト名)/" だけにする。
        StrNCopy(url,  ptr,  (ptr2 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_MACHIBBS);
    }

    return (~errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   parseBoard                                               */
/*                                  受信データの構造解釈および板一覧の更新 */
/*-------------------------------------------------------------------------*/
static void parseBoard(DmOpenRef dbRef, Char *top, UInt32 *readSize)
{
    Char             *data, *ptr, *chkToken;
    UInt32            parseSize;
    UInt16            state, index, ret;
    NNshBoardDatabase bbsData;

    // データの初期化
    index     = 0;
    ret       = NNSH_BBSTYPE_ERROR;
    state     = HTTP_ANCHORSTATE_NORMAL;
    chkToken  = "<A";
    MemSet(&bbsData, sizeof(NNshBoardDatabase), 0x00);

    parseSize = 0;
    ptr       = top;
    while (parseSize < *readSize)
    {
        data = StrStr(ptr, chkToken);
        if (data == NULL)
        {
            // トークンが見つからなかった(途中までパースした)
            if (state != HTTP_ANCHORSTATE_NORMAL)
            {
                // 前回パースした先頭を応答する
                *readSize = parseSize;
            }
            return;
        }
        // トークン発見！ 現在のステートにより、処理を分岐する
        switch (state)
        {
          case HTTP_ANCHORSTATE_ANCHOR:
            // URLをコピー
            ret      = getBoardURL(bbsData.boardURL, bbsData.boardNick, 
                                   ptr, data);
            chkToken = "</A>";
            ptr      = data + 1;  // 1 == StrLen(">");
            state    = HTTP_ANCHORSTATE_LABEL;
            break;

          case HTTP_ANCHORSTATE_LABEL:
            StrNCopy(bbsData.boardName, ptr, (data - ptr));
            switch (ret)
            {
              case NNSH_BBSTYPE_MACHIBBS:
                // まち BBSの板だった場合
                StrCat(bbsData.boardName, NNSH_MACHINAME_SUFFIX);
                /** not break! **/
              case NNSH_BBSTYPE_2ch:
                // 2chの板 or まち BBSの板だった場合
                bbsData.threadCnt = NNSH_BBSLIST_AGAIN;
                bbsData.state     = (UInt8) NNSH_BBSSTATE_INVISIBLE;
                bbsData.bbsType   = (UInt8) ret;
                {
                    // ボードデータをＤＢに登録する。
                    (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase),
                                            &bbsData);
                }
                ret = NNSH_BBSTYPE_ERROR;
                break;

              default:
                // BBSとは何もしない
                break;
            }

            // 次の板情報を探す
            state     = HTTP_ANCHORSTATE_NORMAL;
            chkToken  = "<A";
            MemSet(&bbsData, sizeof(NNshBoardDatabase), 0x00);
            ptr       = data + 4; // 4 == StrLen("</A>");
            parseSize = ptr - top;
            break;

          case HTTP_ANCHORSTATE_NORMAL:
          default:
            state     = HTTP_ANCHORSTATE_ANCHOR;
            chkToken  = ">";
            ptr       = data + 2; // 2 == StrLen("<A");
            break;
        }
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   updateBoardList                                          */
/*                                                        ボード一覧の更新 */
/*-------------------------------------------------------------------------*/
static void updateBoardList(void)
{
    Err                   ret;
    Char                 *ptr, boardNick[MAX_NICKNAME], *saveBBSInfo, *bufPtr;
    UInt16                bufSize, cnt, index, useBBSCnt, lp;
    UInt32                fileSize, offset, readSize;
    DmOpenRef             dbRef;
    NNshFileRef           fileRef;
    NNshBoardDatabase     tmpBd;

    // 一時領域の確保
    bufSize = NNshParam->bufferSize + MARGIN;
    bufPtr     = NULL;
    while (bufPtr == NULL)
    {
        bufPtr  = (Char *) MemPtrNew(bufSize);
        bufSize = bufSize - 12;
    }
    MemSet(bufPtr, bufSize, 0x00);

    // 使用中BBS数を数えて、現在の設定を退避する。
    Show_BusyForm(MSG_BBSBACKUP_INFO);
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    GetDBCount_NNsh(dbRef, &cnt);

    // 使用中に設定されたBBS数を数える
    useBBSCnt = 0;
    for (index = cnt; index != 0; index--)
    {
        MemSet(&tmpBd, sizeof(tmpBd), 0x00);
        GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
        if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            useBBSCnt++;
        }
    }

    // 設定されているBBS名を記憶する
    saveBBSInfo = MemPtrNew((sizeof(Char) * MAX_NICKNAME) * (useBBSCnt + 1));
    if (saveBBSInfo == NULL)
    {
        // ありゃりゃ、メモリ不足(BBS使用情報は記録しない)
        useBBSCnt = 0;
    }
    else
    {
        // 領域初期化
        MemSet(saveBBSInfo,
               (sizeof(Char) * MAX_NICKNAME) * (useBBSCnt + 1),0x00);
    }

    // ＢＢＳ一覧を全部削除する。
    SetMsg_BusyForm(MSG_BBSDELETE_INFO);
    lp  = 0;
    ptr = saveBBSInfo;
    for (index = cnt; index != 0; index--)
    {
        GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
        if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            // BBS使用情報があれば、その情報を覚えておく
            if (lp < useBBSCnt)
            {
                StrCopy(ptr, tmpBd.boardNick);
                lp++;
                ptr = ptr + MAX_NICKNAME;
            }
        }
        DeleteRecordIDX_NNsh(dbRef, (index - 1));
    }
    useBBSCnt = lp;

    // まず、オフライン用BBS名を(先頭に)登録しておく
    MemSet (&tmpBd, sizeof(NNshBoardDatabase), 0x00);
    StrCopy(tmpBd.boardNick, OFFLINE_THREAD_NICK);
    StrCopy(tmpBd.boardURL,  OFFLINE_THREAD_URL);
    StrCopy(tmpBd.boardName, OFFLINE_THREAD_NAME);
    tmpBd.state     = NNSH_BBSSTATE_VISIBLE;
    tmpBd.threadCnt = NNSH_BBSLIST_AGAIN;
    (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase), &tmpBd);

    // ファイルから受信データを読み出しながらボード一覧を更新する
    SetMsg_BusyForm(MSG_BBSPARSE_INFO);
    OpenFile_NNsh(FILE_RECVMSG, 
                  (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                  &fileRef);
    GetFileSize_NNsh(&fileRef, &fileSize);

    // (デバッグメッセージ表示)
    NNsh_DebugMessage(ALTID_INFO, FILE_RECVMSG, " fileSize:", fileSize);
    
    // 受信データをすべて読み出し、その中からBBS名の部分を抽出、登録
    offset = 0;
    while (offset < fileSize)
    {
        readSize = 0;
        ret = ReadFile_NNsh(&fileRef, offset, bufSize, bufPtr, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            NNsh_DebugMessage(ALTID_ERROR, 
                              "ReadFile_NNsh() ", FILE_RECVMSG, ret);
            break;
        }

        // BBS名の抽出およびDBに登録するメイン処理
        parseBoard(dbRef, bufPtr, &readSize);
        offset = offset + readSize;
    }
    if (bufPtr != NULL)
    {
        MemPtrFree(bufPtr);
    }
    CloseFile_NNsh(&fileRef);
    SetMsg_BusyForm(MSG_BBSSORT_INFO);

    // 登録したBBS名を整列する(Quick Sortを使用)
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_CHAR, 0);

    // ダブったBBS名を削除する
    //  (BBS名は整列しているため、ダブったBBS名は連続しているはず)
    SetMsg_BusyForm(MSG_BBSDUPCHK_INFO);
    GetDBCount_NNsh(dbRef, &cnt);
    if (cnt != 0)
    {
        MemSet(&tmpBd, sizeof(tmpBd), 0x00);
        GetRecord_NNsh(dbRef, (cnt - 1), sizeof(tmpBd), &tmpBd);
        MemSet (boardNick, sizeof(boardNick), 0x00);
        StrCopy(boardNick, tmpBd.boardNick);
        for (index = (cnt - 1) ; index != 0; index--)
        {
            MemSet(&tmpBd, sizeof(tmpBd), 0x00);
            GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
            if (StrCompare(boardNick, tmpBd.boardNick) == 0)
            {
                // INDEXの大きいほうを削除する
                DeleteRecordIDX_NNsh(dbRef, index);
            }
            else
            {
                MemSet (boardNick, sizeof(boardNick), 0x00);
                StrCopy(boardNick, tmpBd.boardNick);
            }
        }
    }

    //  一時的に記憶しておいたBBS使用設定を反映させる(結構時間かかるかも？)
    SetMsg_BusyForm(MSG_BBSRESTORE_INFO);
    GetDBCount_NNsh(dbRef, &cnt);
    for (index = cnt; index != 0; index--)
    {
        GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
        for (ptr = saveBBSInfo, lp = 0;
             lp < useBBSCnt;
             lp++, ptr = ptr + MAX_NICKNAME)
        {
	    if (StrCompare(tmpBd.boardNick, ptr) == 0)
            {
                tmpBd.state = NNSH_BBSSTATE_VISIBLE;
                break;
            }
        }
        // BBS状態を(必ず)更新する
        UpdateRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
    }

    // あとしまつ
    if (saveBBSInfo != NULL)
    {
        MemPtrFree(saveBBSInfo);
    }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm();
    return;
}

/*=========================================================================*/
/*   Function :   GetBBSList                                               */
/*                                            Internetより板一覧を取得する */
/*=========================================================================*/
Boolean GetBBSList(Char *url)
{
    Err                ret;
    UInt16             buttonID, cnt;
    DmOpenRef          dbRef;
    NNshBoardDatabase  tmpBd;

    // ＢＢＳ取得開始を確認する
    buttonID = NNsh_ConfirmMessage(ALTID_CONFIRM, url, MSG_BBSURL_INFO, 0);
    if (buttonID != 0)
    {
        // キャンセルボタンが押されていた場合、BBS一覧を取得せずに終了する
        //   ただし、このタイミングでBBS一覧が０件だった場合には、OFFLINE
        // 用のスレだけ追加する。
        goto SET_DUMMY_BBS;
    }

    // ホストからボード一覧データを取得
    ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    if (ret != errNone)
    {
        // 通信エラー発生
        if (ret == netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "", ret);
        }
        else
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "", ret);
        }
        // エラー終了処理
        goto SET_DUMMY_BBS;
    }

    // 受信したボード一覧データを変換し、現在記録されているものと置き換える。
    updateBoardList();
    return (true);

SET_DUMMY_BBS:
    // BBS一覧が０件だった場合には、OFFLINE用のスレだけ追加する処理
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    GetDBCount_NNsh(dbRef, &cnt);
    if (cnt == 0)
    {
        // オフラインログ用BBS名を登録
        MemSet (&tmpBd, sizeof(NNshBoardDatabase), 0x00);
        StrCopy(tmpBd.boardNick, OFFLINE_THREAD_NICK);
        StrCopy(tmpBd.boardURL,  OFFLINE_THREAD_URL);
        StrCopy(tmpBd.boardName, OFFLINE_THREAD_NAME);
        tmpBd.bbsType   = NNSH_BBSTYPE_2ch;
        tmpBd.state     = NNSH_BBSSTATE_VISIBLE;
        tmpBd.threadCnt = NNSH_BBSLIST_AGAIN;
        (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase), &tmpBd);

        // BBS一覧画面も初期化する
        if ((NNshGlobal->bbsTitles) != NULL)
        {
            MemPtrFree((NNshGlobal->bbsTitles));
            (NNshGlobal->bbsTitles) = NULL;
        }
        CloseDatabase_NNsh(dbRef);
        return (true);
    }
    CloseDatabase_NNsh(dbRef);
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
//////////   使用BBS選択関連
///////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------------*/
/*   Function :   CloseForm_SelectBBS                                      */
/*                                        画面表示で使用した一時領域の開放 */
/*-------------------------------------------------------------------------*/
static void closeForm_SelectBBS(void)
{
    // 使ったデータのあとしまつ
    MemPtrFree((NNshGlobal->tempInfo));
    (NNshGlobal->tempInfo) = NULL;

    // BBS一覧画面も初期化する
    if ((NNshGlobal->bbsTitles) != NULL)
    {
        MemPtrFree((NNshGlobal->bbsTitles));
        (NNshGlobal->bbsTitles) = NULL;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   setBBSListItem                                           */
/*                                   選択したBBS名を使用する設定に反映する */
/*-------------------------------------------------------------------------*/
static void setBBSListItem(void)
{
    Err                   ret;
    UInt16                index, cnt, useCnt;
    DmOpenRef             dbRef;
    NNshBoardDatabase     bbsData;

    // ＢＢＳ一覧を開き、現在の登録BBS数を取得する
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    GetDBCount_NNsh(dbRef, &cnt);

    // BBS情報を更新する
    useCnt = 0;
    for (index = 0; index < cnt; index++)
    {
        // BBS情報を取得する
        ret =  GetRecord_NNsh(dbRef, index, sizeof(bbsData), &bbsData);
        if (ret != errNone)
        {
            // ここでエラーはありえないはずなんだが...
            NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh()", "", ret);
            continue;
        }
        if (((NNshGlobal->tempInfo)->checkBoxState[index] == 
                                          NNSH_BBSSTATE_VISIBLE)||(index == 0))
        {
            bbsData.state = NNSH_BBSSTATE_VISIBLE;
            useCnt++;
        } 
        else
        {
            bbsData.state = NNSH_BBSSTATE_INVISIBLE;
        } 
        UpdateRecord_NNsh(dbRef, index, sizeof(bbsData), &bbsData);
    }

    // データベースのクローズ
    CloseDatabase_NNsh(dbRef);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   setAllBBS_Selection                                      */
/*                                            ボード一覧選択情報を一括更新 */
/*-------------------------------------------------------------------------*/
static void setAllBBS_Selection(UInt16 value)
{
    FormType  *frm;
    TableType *tableP;
    UInt16     row, nofRow;

    // すべて選択 もしくは 解除する
    for (row = 0; row < (NNshGlobal->tempInfo)->nofRecords; row++)
    {
        (NNshGlobal->tempInfo)->checkBoxState[row] = value;
    }

    // 画面上のオブジェクトに設定を反映させる
    frm    = FrmGetActiveForm();
    tableP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
    nofRow = TblGetNumberOfRows(tableP);
    for (row = 0; row < nofRow; row++)
    {
        TblSetItemInt(tableP, row, 1, value);
    }

    //テーブルの表示状態を更新する
    TblEraseTable(tableP);
    TblDrawTable (tableP);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : ctlSelEvt_SelectBBS                                        */
/*                                                ボタンが押された時の処理 */
/*-------------------------------------------------------------------------*/
static Boolean ctlSelEvt_SelectBBS(EventType *event)
{
    Boolean ret = true;
    Err     err;

    switch (event->data.ctlSelect.controlID)
    {
      // 「すべて選択」ボタン
      case BTNID_SELECT_ALLBBS:
        setAllBBS_Selection(NNSH_BBSSTATE_VISIBLE);
        break;

      // 「すべて解除」ボタン
      case BTNID_DESELECT_ALLBBS:
        setAllBBS_Selection(NNSH_BBSSTATE_INVISIBLE);
        break;

      // 「設定」ボタン
      case BTNID_SELECT_BBS:
        // 現在の設定を「使用状態格納ワーク領域」に反映させる
        set_Selection_Info((NNshGlobal->tempInfo)->currentPage);
        // 使用BBS設定をDBに反映する
        Show_BusyForm(MSG_BBSLISTSETTING_INFO);
        setBBSListItem();

        // 使用BBS設定DBをVFSにバックアップする
        if ((NNshParam->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            err = BackupDatabaseToVFS_NNsh(DBNAME_BBSLIST);
            if (err != errNone)
            {
                // エラー表示
                NNsh_DebugMessage(ALTID_ERROR, "BackupDatabaseToVFS()",
                                  DBNAME_BBSLIST, err);
            }
        }
        Hide_BusyForm();

        // フォームで使用した一時領域の開放
        closeForm_SelectBBS();

        // スレ一覧画面に戻る
        FrmGotoForm(FRMID_THREAD);
        break;

      // それ以外のボタン(ありえない)
      default:
        ret = false;
        break;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : ctlRepEvt_SelectBBS                                        */
/*                                        リピートボタンが押された時の処理 */
/*-------------------------------------------------------------------------*/
static Int16 ctlRepEvt_SelectBBS(EventType *event)
{
    Int16 command = 0;

    switch (event->data.ctlRepeat.controlID)
    {
      case RPTID_MANAGEBBS_UP:
        // 上リピートボタン : 前のページを表示
        command = -1;
        break;

      case RPTID_MANAGEBBS_DOWN:
        // 下リピートボタン : 次のページを表示
        command = 1;
        break;

      default:
        // それ以外のボタン
        break;
    }
    return (command);
}

/*-------------------------------------------------------------------------*/
/*   Function : keyDownEvt_SelectBBS                                       */
/*                                                  キー入力された時の処理 */
/*-------------------------------------------------------------------------*/
static Int16 keyDownEvt_SelectBBS(EventType *event)
{
    Int16  command = 0;
    UInt16 keyCode;

    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
        // Bキー入力
      case chrCapital_B:
      case chrSmall_B:
        // 末尾へ
        command = (NNshGlobal->tempInfo)->maxPage
                                  - ((NNshGlobal->tempInfo)->currentPage + 1);
        break;

        // Tキー入力
      case chrCapital_T:
      case chrSmall_T:
        // 先頭へ
        command = (NNshGlobal->tempInfo)->currentPage * (-1);
        break;


      // 「上」を押した時の処理(ジョグダイヤル上) : 前のページを表示
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case chrCapital_P:  // Pキーの入力
      case chrSmall_P:
        command = -1;
        break;

      // 「下」を押した時の処理(ジョグダイヤル上) : 次のページを表示
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case chrCapital_N:  // Nキーの入力
      case chrSmall_N:
        command = 1;
        break;

      // ジョグダイヤル押し回し(下)の処理
      case vchrJogPushedDown:
      // カーソルキー(下)押下時の処理
      case chrUnitSeparator:
        if (((NNshGlobal->tempInfo)->maxPage - 
                                     (NNshGlobal->tempInfo)->currentPage) < 5)
        {
            command = (NNshGlobal->tempInfo)->maxPage
                                  - ((NNshGlobal->tempInfo)->currentPage + 1);
        }
        else
        {
            command = 5;
        }
        break;

        // ジョグダイヤル押し回し(上)の処理
      case vchrJogPushedUp:
      // カーソルキー(上)押下時の処理
      case chrRecordSeparator:
        if ((NNshGlobal->tempInfo)->currentPage < 5)
        {
            command = (NNshGlobal->tempInfo)->currentPage * (-1);
        }
        else
        {
            command = -5;
        }
        break;

      // Qキー入力
      case chrCapital_Q:
      case chrSmall_Q:
        // 回線切断
        NNshNet_LineHangup();
        break;

      // 上下以外の処理(何もしない)
      case vchrJogRelease:
      case vchrJogBack:
      default:
        break;
    }
    return (command);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Set_BBS_Selection                                        */
/*                                        表示中のボード一覧選択情報を取得 */
/*-------------------------------------------------------------------------*/
static void set_Selection_Info(UInt16 pageNum)
{
    FormType  *frm;
    TableType *tableP;
    UInt16     nofRow, lp, startItem;

    // フォームの情報を取得する
    frm    = FrmGetActiveForm();
    tableP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));

    // 取得する先頭のINDEX番号を決定する
    nofRow    = (NNshGlobal->tempInfo)->nofRow;
    startItem = pageNum * (nofRow - 1);

    // 表示されているデータを「使用状態格納ワーク領域」に反映させる
    for (lp = 0; lp < nofRow; lp++)
    {
        // データの最大数を超えたかどうかのチェック
        if ((startItem + lp) >= (NNshGlobal->tempInfo)->nofRecords)
        {
            return;
        }
        (NNshGlobal->tempInfo)->checkBoxState[startItem + lp]
                                               = TblGetItemInt(tableP, lp, 1);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   display_BBS_Selection                                    */
/*                                  ボード一覧選択情報を(ページ単位で)表示 */
/*-------------------------------------------------------------------------*/
static void display_BBS_Selection(UInt16 pageNum)
{
    FormType         *frm;
    TableType        *tableP;
    Char             *namePtr;
    UInt16           *chkPtr, nofRow, lp, startItem;
    DmOpenRef         dbRef;
    NNshBoardDatabase tmpBd;

    // フォームの情報を取得する
    frm    = FrmGetActiveForm();
    tableP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
    nofRow = TblGetNumberOfRows(tableP);

    // 現在表示中のページ数を(画面右上に)表示する
    MemSet ((NNshGlobal->tempInfo)->PageLabel, BBSSEL_PAGEINFO, 0x00);
    NUMCATI((NNshGlobal->tempInfo)->PageLabel, (pageNum + 1));
    StrCat ((NNshGlobal->tempInfo)->PageLabel, " / ");
    NUMCATI((NNshGlobal->tempInfo)->PageLabel, 
            (NNshGlobal->tempInfo)->maxPage);
    NNshWinSetFieldText(frm, FLDID_PAGENUM, (NNshGlobal->tempInfo)->PageLabel,
			StrLen((NNshGlobal->tempInfo)->PageLabel));

    // 取得する先頭のINDEX番号を決定する
    startItem = pageNum * (nofRow - 1);

    // 画面に表示する板名ラベルを作成しながら、画面を設定する
    MemSet((NNshGlobal->tempInfo)->BBSName_Temp, 
           (NNshGlobal->tempInfo)->BBSName_size, 0x00);
    namePtr = (NNshGlobal->tempInfo)->BBSName_Temp;
    chkPtr  = (NNshGlobal->tempInfo)->checkBoxState;
    for (lp = 0; lp < nofRow; lp++)
    {
        // データの最大数を超えたかどうかのチェック
        if ((startItem + lp) < (NNshGlobal->tempInfo)->nofRecords)
        {
            // 板名ラベルをnamePtrにコピーする(板名 + boardNickを表示する)
            MemSet(&tmpBd, sizeof(tmpBd), 0x00);
            OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
            GetRecord_NNsh(dbRef, (lp + startItem), sizeof(tmpBd), &tmpBd);
            CloseDatabase_NNsh(dbRef);
            StrCopy(namePtr, tmpBd.boardName);
            StrCat (namePtr, "(");
            StrCat (namePtr, tmpBd.boardNick);
            namePtr[StrLen(namePtr) - 1] = ')';

            // 列の作成
            TblSetRowData(tableP, lp, (UInt32) (lp + 1));

            // データセット (板名称の設定)
            TblSetItemStyle(tableP, lp, 0, labelTableItem);
            TblSetItemPtr  (tableP, lp, 0, namePtr);

            // データセット (チェックボックス)
            TblSetItemStyle(tableP, lp, 1, checkboxTableItem);
            TblSetItemInt (tableP, lp, 1, (UInt16) chkPtr[lp + startItem]);

            // 板名ラベル格納領域を次へすすめる
            namePtr = namePtr + StrLen(namePtr) + 1;

            TblSetRowUsable(tableP, lp, true);
        }
        else
        {
            TblSetRowUsable(tableP, lp, false);
        }
    }
    
    // テーブルのデータを表示できるようにする
    TblSetColumnUsable(tableP, 0, true);
    TblSetColumnUsable(tableP, 1, true);

    // 画面の再表示
    TblEraseTable(tableP);
    TblDrawTable (tableP);
    return;
}

/*=========================================================================*/
/*   Function : Handler_SelectBBS                                          */
/*                                           ＢＢＳ管理のイベントハンドラ  */
/*=========================================================================*/
Boolean Handler_SelectBBS(EventType *event)
{
    Int16  addPage = 0;

    switch (event->eType)
    {
      case ctlSelectEvent:
        // ボタンが押されたときの処理
        ctlSelEvt_SelectBBS(event);
        break;

      case ctlRepeatEvent:
        // リピートボタンが押されたときの処理
        addPage = ctlRepEvt_SelectBBS(event);
	break;

      case keyDownEvent:
        // キー入力されたときの処理
        addPage = keyDownEvt_SelectBBS(event);
	break;

      case menuEvent:
        // 回線切断
        if (event->data.menu.itemID == MNUID_NET_DISCONN)
        {
            NNshNet_LineHangup();
        }
        return (false);
	break;

      default:
        // 上記以外のイベント
	break;
    }

    // ページの移動処理
    if ((addPage != 0)&&
        (((NNshGlobal->tempInfo)->currentPage + addPage) >= 0)&&
        (((NNshGlobal->tempInfo)->currentPage +addPage) <
                                              (NNshGlobal->tempInfo)->maxPage))
    {
        // 現在の設定を「使用状態格納ワーク領域」に反映させる
        set_Selection_Info((NNshGlobal->tempInfo)->currentPage);

        // 表示するページ番号を更新する
        (NNshGlobal->tempInfo)->currentPage =
                                  (NNshGlobal->tempInfo)->currentPage +addPage;

        //////////  スレ選択画面を更新する  /////////
        display_BBS_Selection((NNshGlobal->tempInfo)->currentPage);
        return (true);
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_SelectBBS                                       */
/*                                       使用するBBS名を選択する画面を開く */
/*=========================================================================*/
void OpenForm_SelectBBS(FormType *frm)
{
    Err                    ret;
    DmOpenRef              dbBBSRef;
    UInt16                 bbsCnt,lp, nofRow;
    UInt32                 bufSize;
    NNshBoardDatabase      tmpBd;
    TableType             *tableP;

    // 使用BBS選択画面用バッファ領域の確保
    if ((NNshGlobal->tempInfo) != NULL)
    {
        MemPtrFree((NNshGlobal->tempInfo));
    }
    (NNshGlobal->tempInfo) = MemPtrNew(sizeof(NNshBBS_Temp_Info));
    if ((NNshGlobal->tempInfo) == NULL)
    {
        // 領域確保エラー、元の一覧画面に戻る
        FrmGotoForm(FRMID_THREAD);
        return;
    }
    MemSet((NNshGlobal->tempInfo), sizeof(NNshBBS_Temp_Info), 0x00);

    // 列数を取得する
    tableP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
    nofRow = TblGetNumberOfRows(tableP);

    // BUSYウィンドウを表示する
    Show_BusyForm(MSG_READ_BBSLIST_WAIT);

    // 登録されている板数を取得
    OpenDatabase_NNsh (DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbBBSRef);
    GetDBCount_NNsh   (dbBBSRef, &bbsCnt);

    // 使用状態格納ワーク領域の確保(確保済みだった場合には解放してから登録)
    if ((NNshGlobal->tempInfo)->checkBoxState != NULL)
    {
        MemPtrFree((NNshGlobal->tempInfo)->checkBoxState);
        (NNshGlobal->tempInfo)->checkBoxState = NULL;
    }
    bufSize = (bbsCnt + 1) * sizeof(UInt16) + 8;
    (NNshGlobal->tempInfo)->checkBoxState = (UInt16 *) MemPtrNew(bufSize);
    if ((NNshGlobal->tempInfo)->checkBoxState == NULL)
    {
        //  バッファ領域確保エラー
        NNsh_InformMessage(ALTID_ERROR,MSG_MEMORY_ALLOC_FAIL,"size:",bufSize);
        goto ERROR_END;
    }
    MemSet((NNshGlobal->tempInfo)->checkBoxState, bufSize, 0x00);

    // BBSの使用中状態を一時領域に反映させる
    for (lp = 0; lp < bbsCnt; lp++)
    {
        ret = GetRecord_NNsh(dbBBSRef, lp, sizeof(tmpBd), &tmpBd);
        if (ret == errNone)
        {
            (NNshGlobal->tempInfo)->checkBoxState[lp] = tmpBd.state;
        }
    }

    // 板名表示領域の確保(確保済みだった場合には解放してから再度確保する)
    if ((NNshGlobal->tempInfo)->BBSName_Temp != NULL)
    {
        MemPtrFree((NNshGlobal->tempInfo)->BBSName_Temp);
        (NNshGlobal->tempInfo)->BBSName_Temp = NULL;
    }
    bufSize = (nofRow + 1) * 
                    (MAX_BOARDNAME + MAX_NICKNAME + 3 + 1) * sizeof(Char) + 8;
    (NNshGlobal->tempInfo)->BBSName_Temp = (Char *) MemPtrNew(bufSize);
    if ((NNshGlobal->tempInfo)->BBSName_Temp == NULL)
    {
        //  バッファ領域確保エラー
        NNsh_InformMessage(ALTID_ERROR,MSG_MEMORY_ALLOC_FAIL,"Size:",bufSize);

        // チェック状態の領域を開放
        MemPtrFree((NNshGlobal->tempInfo)->checkBoxState);
        (NNshGlobal->tempInfo)->checkBoxState = NULL;

        goto ERROR_END;
    }
    (NNshGlobal->tempInfo)->BBSName_size = bufSize;
    MemSet((NNshGlobal->tempInfo)->BBSName_Temp, bufSize, 0x00);

    // BBS選択画面最大ページ数を求める
    (NNshGlobal->tempInfo)->maxPage     = (bbsCnt / (nofRow - 1));
    if ((bbsCnt % (nofRow - 1)) > 1)
    {
        //  スクロールを考慮すると、1ページに実質(nofRow - 1)個のBBS名が
        // 表示できるが、最初のページだけは nofRow個表示できるため、その
        // 部分を考慮してあまりページを求める。
        (NNshGlobal->tempInfo)->maxPage++;
    }
    (NNshGlobal->tempInfo)->currentPage = 0;
    (NNshGlobal->tempInfo)->nofRecords  = bbsCnt;
    (NNshGlobal->tempInfo)->nofRow      = nofRow;

    // データベースのクローズ
    CloseDatabase_NNsh(dbBBSRef);

    // BUSYウィンドウを消去する
    Hide_BusyForm();

    //////////  スレ選択画面を表示する  /////////
    display_BBS_Selection((NNshGlobal->tempInfo)->currentPage);

    // BBSタイトル情報はクリアする
    if ((NNshGlobal->bbsTitles) != NULL)
    {
        MemPtrFree((NNshGlobal->bbsTitles));
        (NNshGlobal->bbsTitles) = NULL;
    }
    return;

ERROR_END:
    // データベースのクローズ
    CloseDatabase_NNsh(dbBBSRef);

    // BUSYウィンドウを消去する
    Hide_BusyForm();

    // データをクリアする
    closeForm_SelectBBS();

    // スレ一覧画面に戻る
    FrmGotoForm(FRMID_THREAD);
    return;
}

/*=========================================================================*/
/*   Function :   Get_BBS_Info                                             */
/*                                     リスト選択番号よりBBS情報を取得する */
/*=========================================================================*/
Err Get_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo)
{
    Err              ret;
    DmOpenRef        bbsRef;
    UInt16          *bbsIdx, index;
    NNshBBSRelation *relPtr;

#if 0    // この関数を呼び出す前にこのエラーは判定済み、、、のはず。

    // BBS一覧が「お気に入り」「取得済みスレ全て」「未読あり」の時はエラー
    if ((selBBS == NNSH_SELBBS_FAVORITE)||
        (selBBS == NNSH_SELBBS_GETALL)||
        (selBBS == NNSH_SELBBS_NOTREAD))
    {    
        return (~errNone - 3);
    }

#endif

    // BBSのインデックスを取得する
    if (NNshGlobal->boardIdxH == 0)
    {
        // インデックス取得失敗
        return (~errNone);
    }
    relPtr = MemHandleLock(NNshGlobal->boardIdxH);
    if (relPtr == NULL)
    {
        // インデックス取得失敗
        return (~errNone);
    }
    bbsIdx = &(relPtr->bbsIndex);
    index  = bbsIdx[selBBS];
    MemHandleUnlock(NNshGlobal->boardIdxH);

    // データベースをオープンし、BBS情報を取得する
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
    ret = GetRecord_NNsh(bbsRef, index, sizeof(NNshBoardDatabase), bbsInfo);
    CloseDatabase_NNsh(bbsRef);
    if (ret != errNone)
    {
        // レコードの取得に失敗
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Info()", "", ret);
        MemSet(bbsInfo, sizeof(NNshBoardDatabase),   0x00);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   Update_BBS_Info                                          */
/*                                     リスト選択番号からBBS情報を更新する */
/*=========================================================================*/
Err Update_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo)
{
    UInt16          *bbsIdx, index;
    NNshBBSRelation *relPtr;
    
    // BBS一覧が「お気に入り」「取得済みスレ全て」「未読あり」の時はエラー
    if ((selBBS == NNSH_SELBBS_FAVORITE)||
        (selBBS == NNSH_SELBBS_GETALL)||
        (selBBS == NNSH_SELBBS_NOTREAD))
    {    
        return (~errNone - 3);
    }

    // BBSのインデックスを取得する
    if (NNshGlobal->boardIdxH == 0)
    {
        // インデックス取得失敗
        return (~errNone);
    }
    relPtr = MemHandleLock(NNshGlobal->boardIdxH);
    if (relPtr == NULL)
    {
        // インデックス取得失敗
        return (~errNone);
    }
    bbsIdx = &(relPtr->bbsIndex);
    index  = bbsIdx[selBBS];
    MemHandleUnlock(NNshGlobal->boardIdxH);

    // BBSのデータベースにデータを登録する
    return (Update_BBS_Database(index, bbsInfo));
}

/*=========================================================================*/
/*   Function : Get_BBS_Database                                           */
/*                                      (boardNickからBBS情報を取得する)   */
/*=========================================================================*/
Err Get_BBS_Database(Char *boardNick, NNshBoardDatabase *bbs, UInt16 *index)
{
    Err       ret;
    DmOpenRef dbRef;

    *index = 0;

    // BBS情報をデータベースから取得
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);

    ret = SearchRecord_NNsh(dbRef, boardNick, NNSH_KEYTYPE_CHAR, 0,
                            sizeof(NNshBoardDatabase), bbs, index);

    CloseDatabase_NNsh(dbRef);

    return (ret);
}

/*=========================================================================*/
/*   Function : Update_BBS_Database                                        */
/*                                          (indexからBBS情報を記録する)   */
/*=========================================================================*/
Err Update_BBS_Database(UInt16 index, NNshBoardDatabase *bbs)
{
    Err       ret;
    DmOpenRef bbsRef;

    // データベースをオープンし、BBS情報を更新する
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
    ret = UpdateRecord_NNsh(bbsRef, index, sizeof(NNshBoardDatabase), bbs);
    CloseDatabase_NNsh(bbsRef);

    return (ret);
}
