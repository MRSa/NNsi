/*============================================================================*
 *
 *  $Id: bbsmng.c,v 1.53 2006/05/08 15:18:43 mrsa Exp $
 *
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
static void    display_BBS_Selection(UInt16     pageNum, Int16 selection);

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

    // まちBBSのボードも大丈夫にしてみる。。。(新URL)
    chk = StrStr(top, "machi.to");
    if ((chk != NULL)&&(chk < bottom))
    {
        // まちBBSのURLは、"http://(ホスト名)/" だけにする。
        StrNCopy(url,  ptr,  (ptr2 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_MACHIBBS);
    }

    // まちBBSのボードも大丈夫にしてみる。。。(旧URL)
    chk = StrStr(top, "machibbs.com");
    if ((chk != NULL)&&(chk < bottom))
    {
        // まちBBSのURLは、"http://(ホスト名)/" だけにする。
        StrNCopy(url,  ptr,  (ptr2 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_MACHIBBS);
    }

    if ((NNshGlobal->NNsiParam)->notCheckBBSURL != 0)
    {
        // 板URLの有効チェックをしない場合(とりあえず)
        StrNCopy(url,  ptr,  (ptr3 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_2ch);
#if 0
        {
            // まちBBSのURLは、"http://(ホスト名)/" だけにする。
            StrNCopy(url,  ptr,  (ptr2 - ptr));
            StrNCopy(nick, ptr2, (ptr3 - ptr2));
            return (NNSH_BBSTYPE_MACHIBBS);
        }
#endif
    }

    return (~errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   parseBoard                                               */
/*                                  受信データの構造解釈および板一覧の更新 */
/*-------------------------------------------------------------------------*/
static void parseBoard(DmOpenRef dbRef, Char *top, UInt32 *readSize)
{
    Err               err;
    Char             *data, *ptr, *chkToken;
    UInt32            parseSize;
    UInt16            state, index, ret;
    NNshBoardDatabase bbsData, bbsData2;

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
            if ((data - ptr) < (MAX_BOARDNAME - sizeof(NNSH_MACHINAME_SUFFIX)))
            {
                StrNCopy(bbsData.boardName, ptr, (data - ptr));
            }
            else
            {
                StrNCopy(bbsData.boardName, 
                         ptr, 
                         (MAX_BOARDNAME - sizeof(NNSH_MACHINAME_SUFFIX)));
            }
            switch (ret)
            {
              case NNSH_BBSTYPE_SHITARABA:
                // したらばだった場合
                StrCat(bbsData.boardName, NNSH_SHITARABA_SUFFIX);
                break;

              case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
              case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
                // したらば@JBBSだった場合
                StrCat(bbsData.boardName, NNSH_SHITARABAJBBS_SUFFIX);
                break;

              case NNSH_BBSTYPE_MACHIBBS:
                // まちBBSだった場合
                StrCat(bbsData.boardName, NNSH_MACHINAME_SUFFIX);
                break;

              case NNSH_BBSTYPE_2ch:
              case NNSH_BBSTYPE_2ch_EUC:
              case NNSH_BBSTYPE_OTHERBBS:
              case NNSH_BBSTYPE_OTHERBBS_2:
                // 2chだった場合
                break;

              default:
                // BBSとは何もしない
                goto NEXT_BBS;
                break;
            }

            // BBSデータをDBに登録する
            bbsData.state     = (UInt8) NNSH_BBSSTATE_INVISIBLE;
            bbsData.bbsType   = (UInt8) ret;

            if ((NNshGlobal->NNsiParam)->bbsOverwrite == 0)
            {
                // ボードデータをＤＢに登録する。
                (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase),
                                        &bbsData);
            }
            else
            {
                // ボードデータがすでに登録済みかチェックする
                err = SearchRecord_NNsh(dbRef, bbsData.boardNick,
                                        NNSH_KEYTYPE_CHAR, 0,
                                        sizeof(NNshBoardDatabase),
                                        &bbsData2, &index);
                if (err != errNone)
                {
                    // データ見つからず、新規登録する
                    (void) EntryRecord_NNsh(dbRef, 
                                            sizeof(NNshBoardDatabase),
                                            &bbsData);
                }
                else
                {
                    // 既存データ発見、既存データを上書きする
                    bbsData.state   = bbsData2.state;
                    bbsData.bbsType = bbsData2.bbsType;
                    (void) UpdateRecord_NNsh(dbRef, index,
                                             sizeof(NNshBoardDatabase),
                                             &bbsData);
                }
            }
            ret = NNSH_BBSTYPE_ERROR;

NEXT_BBS:
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
    bufSize = (NNshGlobal->NNsiParam)->bufferSize + MARGIN;
    bufPtr     = NULL;
    while (bufPtr == NULL)
    {
        bufPtr  = (Char *) MEMALLOC_PTR(bufSize);
        bufSize = bufSize - 12;
    }
    MemSet(bufPtr, bufSize, 0x00);

    // 使用中BBS数を数えて、現在の設定を退避する。
    Show_BusyForm(MSG_BBSBACKUP_INFO);
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    GetDBCount_NNsh(dbRef, &cnt);

    useBBSCnt   = 0;
    saveBBSInfo = NULL;

    if ((NNshGlobal->NNsiParam)->bbsOverwrite == 0)
    { 
        // 使用中に設定されたBBS数を数える
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
        saveBBSInfo = 
                MEMALLOC_PTR((sizeof(Char) * (MAX_NICKNAME + 1)) * (useBBSCnt + 1));
        if (saveBBSInfo == NULL)
        {
            // ありゃりゃ、メモリ不足(BBS使用情報は記録しない)
            useBBSCnt = 0;
        }
        else
        {
            // 領域初期化
            MemSet(saveBBSInfo,
                   (sizeof(Char) * MAX_NICKNAME + 1) * (useBBSCnt + 1),0x00);
        }

        // ＢＢＳ一覧を全部削除する。
        SetMsg_BusyForm(MSG_BBSDELETE_INFO);
        ptr = saveBBSInfo;
        lp  = 0;
        for (index = cnt; index != 0; index--)
        {
            GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
            if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE)== NNSH_BBSSTATE_VISIBLE)
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
    }

    // レコード件数を確認する
    GetDBCount_NNsh(dbRef, &cnt);
    if (cnt == 0)
    {
        // レコードが１件もなかった場合、オフライン用BBS名を(先頭に)登録しておく
        MemSet (&tmpBd, sizeof(NNshBoardDatabase), 0x00);
        StrCopy(tmpBd.boardNick, OFFLINE_THREAD_NICK);
        StrCopy(tmpBd.boardURL,  OFFLINE_THREAD_URL);
        StrCopy(tmpBd.boardName, OFFLINE_THREAD_NAME);
        tmpBd.state     = NNSH_BBSSTATE_VISIBLE;
        (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase), &tmpBd);
    }

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
        if (readSize == 0)
        {
            // パース失敗...
            NNsh_DebugMessage(ALTID_INFO, "readSize == 0", " ret:", ret);
            break;
        }
    }
    MEMFREE_PTR(bufPtr);
    CloseFile_NNsh(&fileRef);
    SetMsg_BusyForm(MSG_BBSSORT_INFO);

    // 登録したBBS名を整列する(Quick Sortを使用)
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_CHAR, 0);

    // ダブったBBS名(boardNick)を削除する
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

    //    BBS一覧を上書きしない場合には、一時的に記憶しておいた
    //  BBS使用設定を反映させる(結構時間かかるかも？)
    if ((NNshGlobal->NNsiParam)->bbsOverwrite == 0)
    {
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
    }

    // あとしまつ
    MEMFREE_PTR(saveBBSInfo);
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(true);
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
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
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
        (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase), &tmpBd);

        // BBS一覧画面も初期化する
        if (NNshGlobal->boardIdxH != 0)
        {
            MemHandleFree(NNshGlobal->boardIdxH);
            NNshGlobal->boardIdxH = 0;
        }
        MEMFREE_PTR((NNshGlobal->bbsTitles));
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
/*   Function :   searchBBSTitle                                           */
/*                                                 BBSタイトル検索実行処理 */
/*-------------------------------------------------------------------------*/
static Err searchBBSTitle(Char *title, UInt16 direction, UInt16 *recNum)
{
    Err                ret;
    Int16              step;
    UInt16             end, loop;
    Char               string[BUFSIZE];
    Char               match1[MAX_BOARDNAME], match2[MAX_BOARDNAME];
    DmOpenRef          dbRef;
    MemHandle          bdH;
    NNshBoardDatabase *tmpBd;

    // "検索中"の表示
    MemSet (string, 0x00, sizeof(string));
    StrCopy(string, MSG_SEARCHING_TITLE);
    StrCat (string, title);
    Show_BusyForm(string);

    // 大文字小文字を区別しない場合には、小文字に変換
    MemSet(match1, sizeof(match1), 0x00);
    if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
    {
        (void) StrToLower(match1, title);
    }
    else
    {
        (void) StrCopy(match1, title);
    }

    // BBS情報をデータベースから取得
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);

    switch (direction)
    {
      case NNSH_SEARCH_ALL:
        // 先頭から全検索
        *recNum = 0;
        GetDBCount_NNsh(dbRef, &end);
        end--;
        step = 1;
        break;

      case NNSH_SEARCH_FORWARD:
        // フォワード検索指示
        GetDBCount_NNsh(dbRef, &end);
        end--;
        step = 1;
        break;

      case NNSH_SEARCH_BACKWARD: 
      default:
        // バックワード検索指示
        end  = 0;
        step = -1;
        break;
    }
    loop = *recNum;

    // 検索の実処理
    do {
        // GetRecord_NNsh()内でゼロクリアしているため不要
        // MemSet(&tmpBd, sizeof(NNshBoardDatabase), 0x00);
        GetRecordReadOnly_NNsh(dbRef,(loop + step), &bdH, (void **)  &tmpBd);

        //  指定された文字列とマッチするか確認
        MemSet(match2, sizeof(match2), 0x00);
        if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
        {
            // スレタイを小文字に変換
            (void) StrToLower(match2, tmpBd->boardName);
        }
        else
        {
            (void) StrCopy(match2, tmpBd->boardName);
        }
        ReleaseRecordReadOnly_NNsh(dbRef, bdH);

        loop = loop + step;
        if (StrStr(match2, match1) != NULL)
        {
            // 見つかった！
            ret = errNone;
            goto FUNC_END;
        }
    } while (loop != end);

    // 検索に失敗、見つからなかった
    ret = ~errNone;

FUNC_END:
    *recNum = loop;
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   entryBBSRecord                                           */
/*                                                           BBSの個別登録 */
/*-------------------------------------------------------------------------*/
static void entryBBSRecord(UInt16 recNum)
{
    Err                err;
    Boolean            ret = false;
    FormType          *prevFrm, *diagFrm;
    UInt16             btnId, item;
    ListType          *lstP;
    NNshBoardDatabase  bbsData;
    Char               msg[BIGBUF], nickBak[MAX_NICKNAME + MARGIN];
    DmOpenRef          bbsRef;

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // ダイアログフォームをアクティブにする
    diagFrm  = FrmInitForm(FRMID_BOARDINPUT);
    if (diagFrm == NULL)
    {
        return;
    }
    FrmSetActiveForm(diagFrm);

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

    if (recNum != NNSH_NOTENTRY_BBS)
    {
        // レコードデータを取得
        MemSet(&bbsData, sizeof(bbsData), 0x00);
        OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
        err = 
           GetRecord_NNsh(bbsRef, recNum, sizeof(NNshBoardDatabase), &bbsData);
        CloseDatabase_NNsh(bbsRef);
        if (err == errNone)
        {
            // データが取得できたときは、その取得内容を画面に反映

            // BBSタイプ
            switch (bbsData.bbsType)
            {
              case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
                // したらば＠JBBS
                item = 7;
                break;

              case NNSH_BBSTYPE_2ch_EUC:
                // 2ch(EUC)
                item = 6;
                break;

              case NNSH_BBSTYPE_OTHERBBS_2:
                // 2ch互換(2)
                item = 5;
                break;

              case NNSH_BBSTYPE_OTHERBBS:
                // 2ch互換
                item = 4;
                break;
              
              case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
                // したらば＠JBBS
                item = 3;
                break;

              case NNSH_BBSTYPE_SHITARABA:
                // したらば
                item = 2;
                break;

              case NNSH_BBSTYPE_MACHIBBS:
                // まちBBS
                item = 1;
                break;

              case NNSH_BBSTYPE_2ch:
              default:
                // ２ちゃんねる
                item = 0;
                break;
            }
            NNshWinSetPopItems(diagFrm,POPTRID_BBSINPUT, LSTID_BBSINPUT, item);

            // BBS名
            NNshWinSetFieldText(diagFrm, FLDID_BBSINPUT_NAME, false,
                                bbsData.boardName, (MAX_BOARDNAME));
            // URL
            NNshWinSetFieldText(diagFrm, FLDID_BBSINPUT_URL, false,
                                bbsData.boardURL, (MAX_URL));
            // BBS Nick
            NNshWinSetFieldText(diagFrm, FLDID_BBSINPUT_NICK, true,
                                bbsData.boardNick, (MAX_NICKNAME));
            MemSet (nickBak, sizeof(nickBak), 0x00);
            StrCopy(nickBak, bbsData.boardNick);

            if (((bbsData.state) & NNSH_BBSSTATE_SUSPEND) == NNSH_BBSSTATE_SUSPEND)
            {
                // 取得停止中の状態設定
                item = 1;
                SetControlValue(diagFrm, CHKID_BBS_STOP, &item);
            }

            // 標準スレレベルの設定
            item = 1;
            switch((bbsData.state) & NNSH_BBSSTATE_LEVELMASK)
            {
              case NNSH_BBSSTATE_FAVOR_L1:
                // お気に入りレベル(低)
                SetControlValue(diagFrm, PBTNID_FAVOR_L1, &item);
                break;

              case NNSH_BBSSTATE_FAVOR_L2:
                // お気に入りレベル(中低)
                SetControlValue(diagFrm, PBTNID_FAVOR_L2, &item);
                break;

              case NNSH_BBSSTATE_FAVOR_L3:
                // お気に入りレベル(中)
                SetControlValue(diagFrm, PBTNID_FAVOR_L3, &item);
                break;

              case NNSH_BBSSTATE_FAVOR_L4:
                // お気に入りレベル(中高)
                SetControlValue(diagFrm, PBTNID_FAVOR_L4, &item);
                break;

              case NNSH_BBSSTATE_FAVOR:
                // お気に入りレベル(高)
                SetControlValue(diagFrm, PBTNID_FAVOR_L5, &item);
                break;

              default:
                // お気に入りではない
                SetControlValue(diagFrm, PBTNID_FAVOR_L0, &item);
                break;
            }
        }
    }

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_DialogAction);

    // 入力ウィンドウを開く
    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_DIALOG_OK)
    {
        // OKボタンが押されたとき、登録する情報を作成する
        MemSet(&bbsData, sizeof(bbsData), 0x00);

        // BBS名取得
        NNshWinGetFieldText(diagFrm, FLDID_BBSINPUT_NAME, bbsData.boardName,
                            (MAX_BOARDNAME - 1));
        // URL取得
        NNshWinGetFieldText(diagFrm, FLDID_BBSINPUT_URL, bbsData.boardURL,
                            (MAX_URL - 1));
        // Nick取得
        NNshWinGetFieldText(diagFrm, FLDID_BBSINPUT_NICK, bbsData.boardNick,
                            (MAX_NICKNAME - 1));

        // スレレベルおよびBBSを使用する設定を更新する
        item = 0;
        UpdateParameter(diagFrm, PBTNID_FAVOR_L0, &item);
        if (item != 0)
        {
            // お気に入り設定されていない
            bbsData.state = NNSH_BBSSTATE_VISIBLE;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L1, &item);
        if (item != 0)
        {
            // お気に入り設定(低)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR_L1);
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L2, &item);
        if (item != 0)
        {
            // お気に入り設定(中低)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR_L2);
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L3, &item);
        if (item != 0)
        {
            // お気に入り設定(中)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR_L3);
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L4, &item);
        if (item != 0)
        {
            // お気に入り設定(中高)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR_L4);
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L5, &item);
        if (item != 0)
        {
            // お気に入り設定(高)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR);
            goto DELETE_DIALOG;
        }

DELETE_DIALOG:
        // 取得停止中の状態
        UpdateParameter(diagFrm, CHKID_BBS_STOP, &item);
        if (item != 0)
        {
            // 取得停止中状態の設定
            bbsData.state = (bbsData.state)|(NNSH_BBSSTATE_SUSPEND);
        }

        // BBSタイプの設定
        lstP = FrmGetObjectPtr(diagFrm,
                               FrmGetObjectIndex(diagFrm, LSTID_BBSINPUT));
        item = LstGetSelection(lstP);
        switch (item)
        {
          case 7:
            // したらば@JBBSタイプ
            bbsData.bbsType = NNSH_BBSTYPE_SHITARABAJBBS_OLD;
            break;

          case 6:
            // 2ch(EUC)タイプ
            bbsData.bbsType = NNSH_BBSTYPE_2ch_EUC;
            break;

          case 5:
            // 2ch互換タイプ(Part2)
            bbsData.bbsType = NNSH_BBSTYPE_OTHERBBS_2;
            break;

          case 4:
            // 2ch互換タイプ
            bbsData.bbsType = NNSH_BBSTYPE_OTHERBBS;
            break;
            
          case 3:
            // したらば@JBBSタイプ
            bbsData.bbsType = NNSH_BBSTYPE_SHITARABAJBBS_RAW;
            break;

          case 2:
            // したらばタイプ
            bbsData.bbsType = NNSH_BBSTYPE_SHITARABA;
            break;

          case 1:
            // まちＢＢＳタイプ
            bbsData.bbsType = NNSH_BBSTYPE_MACHIBBS;
            break;

          case 0:
          default:
            // 2chタイプ
            bbsData.bbsType = NNSH_BBSTYPE_2ch;
            break;
        }
        MemSet (msg, sizeof(msg), 0x00);
        StrCopy(msg, bbsData.boardName);
        StrCat (msg, "(");
        StrCat (msg, bbsData.boardURL);
        StrCat (msg, ", Nick:");
        StrCat (msg, bbsData.boardNick);
        StrCat (msg, ") type:");
        NUMCATI(msg, (UInt16) bbsData.bbsType);
        btnId = NNsh_ConfirmMessage(ALTID_CONFIRM, 
                                    NNSH_CONFIRMMSG_ENTRYBBS, msg, 0);
        if (btnId == 0)
        {
            /////////////////////////////////////////////////////
            // BBSデータベースに登録/更新する
            ////////////////////////////////////////////////////
            if (recNum == NNSH_NOTENTRY_BBS)
            {
                OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);

                EntryRecord_NNsh(bbsRef,sizeof(NNshBoardDatabase),&bbsData);

                // 登録したBBS名を整列する(Quick Sortを使用)
                QsortRecord_NNsh(bbsRef, NNSH_KEYTYPE_CHAR, 0);

                CloseDatabase_NNsh(bbsRef);

                // 登録したことを通知、再起動を要求する
                NNsh_InformMessage(ALTID_WARN, 
                                   bbsData.boardName, NNSH_MSGINFO_DBENTRY, 0);
            }
            else
            {

                // BBS情報を更新する
                err = Update_BBS_Database(recNum, &bbsData);

                // ボードニックネームを変更したかどうか確認する
                if (StrCompare(nickBak, bbsData.boardNick) != 0)
                {
                    // ボードニックネームを変更した場合には、ソートする
                    OpenDatabase_NNsh(DBNAME_BBSLIST, 
                                      DBVERSION_BBSLIST, &bbsRef);
                    // 登録したBBS名を整列する(Quick Sortを使用)
                    QsortRecord_NNsh(bbsRef, NNSH_KEYTYPE_CHAR, 0);

                    CloseDatabase_NNsh(bbsRef);
                }

                // 更新したことを通知する
                NNsh_InformMessage(ALTID_WARN, bbsData.boardName, 
                                   NNSH_MSGINFO_DBUPDATE, err);
            }
        }
    }
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        FrmEraseForm(prevFrm);

        // シルク表示を最小化
        NNshSilkMinimize(prevFrm);

        FrmDrawForm(prevFrm);
    }
    else
#endif
    {
        // シルク表示を最小化
        NNshSilkMinimize(prevFrm);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   CloseForm_SelectBBS                                      */
/*                                        画面表示で使用した一時領域の開放 */
/*-------------------------------------------------------------------------*/
static void closeForm_SelectBBS(void)
{
    // 使ったデータのあとしまつ
    MEMFREE_PTR((NNshGlobal->tempInfo)->checkBoxState);
    MEMFREE_PTR((NNshGlobal->tempInfo)->BBSName_Temp);
    MEMFREE_PTR((NNshGlobal->tempInfo));

    // BBS一覧画面も初期化する
    if (NNshGlobal->boardIdxH != 0)
    {
        // MemHandleUnlock(NNshGlobal->boardIdxH);  // 必要?
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;
    }
    MEMFREE_PTR((NNshGlobal->bbsTitles));
    NNshGlobal->bbsTitles = 0;  // 念のため...
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
            bbsData.state = ((NNSH_BBSSTATE_VISIBLEMASK)&(bbsData.state))|(NNSH_BBSSTATE_VISIBLE);
            useCnt++;
        } 
        else
        {
            bbsData.state = ((NNSH_BBSSTATE_VISIBLEMASK)&(bbsData.state))|(NNSH_BBSSTATE_INVISIBLE);
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
/*   Function : detail_SelectBBS                                           */
/*                                                       BBS詳細の表示処理 */
/*-------------------------------------------------------------------------*/
static Boolean detail_SelectBBS(void)
{
    FormType    *frm;
    TableType   *tableP;
    UInt16       row, col, nofRow, recNum;

    frm    = FrmGetActiveForm();
    tableP = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,TBLID_BBS_SELECTION));
                                 
    nofRow = TblGetNumberOfRows(tableP);
    if (TblGetSelection(tableP, &row, &col) == true)
    {
        // 取得するレコード番号を決定する
        recNum = ((NNshGlobal->tempInfo)->currentPage * 
                  ((NNshGlobal->tempInfo)->nofRow - 1)) + row;
        entryBBSRecord(recNum);

        //////////  スレ選択画面を更新する  /////////
        display_BBS_Selection((NNshGlobal->tempInfo)->currentPage,
                              NNSH_ITEM_LASTITEM);
        return (true);
    }
    // テーブルが選択されていなかった(警告表示)
    NNsh_WarningMessage(ALTID_WARN, NNSH_MSGINFO_NOTSELECTBBS, "", 0);

    return (false);
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

      // 「詳細」ボタン
      case BTNID_DETAIL_BBS:
        return (detail_SelectBBS());
        break;

      // 「設定」ボタン
      case BTNID_SELECT_BBS:
        // 現在の設定を「使用状態格納ワーク領域」に反映させる
        set_Selection_Info((NNshGlobal->tempInfo)->currentPage);
        // 使用BBS設定をDBに反映する
        Show_BusyForm(MSG_BBSLISTSETTING_INFO);
        setBBSListItem();

        // 使用BBS設定DBをVFSにバックアップする
        if (((NNshGlobal->NNsiParam)->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            err = BackupDatabaseToVFS_NNsh(DBNAME_BBSLIST);
            if (err != errNone)
            {
                // エラー表示
                NNsh_DebugMessage(ALTID_ERROR, "BackupDatabaseToVFS()",
                                  DBNAME_BBSLIST, err);
            }
        }
        Hide_BusyForm(true);

        // フォームで使用した一時領域の開放
        closeForm_SelectBBS();

        // スレ一覧画面に戻る
        FrmGotoForm(NNshGlobal->backFormId);
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

    //tungsten T 5way navigator (281さん、感謝！)
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
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case chrUpArrow:
      case chrCapital_P:  // Pキーの入力
      case chrSmall_P:
        command = -1;
        break;

      // 「下」を押した時の処理(ジョグダイヤル上) : 次のページを表示
      case vchrPageDown:
      case vchrJogDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case chrDownArrow:
      case chrCapital_N:  // Nキーの入力
      case chrSmall_N:
        command = 1;
        break;

      // ジョグダイヤル押し回し(下)の処理
      // カーソルキー右（5way 右）の処理
      // カーソルキー(下)押下時の処理
      case vchrJogPushedDown:
      case chrRightArrow:
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
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
      // カーソルキー（左）・5way左の処理 
      // カーソルキー(上)押下時の処理
      case vchrJogPushedUp:
      case chrLeftArrow:
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
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
static void display_BBS_Selection(UInt16 pageNum, Int16 selection)
{
    FormType          *frm;
    TableType         *tableP;
    Char              *namePtr;
    UInt16            *chkPtr, nofRow, lp, startItem;
    DmOpenRef          dbRef;
    MemHandle          bdH;
    NNshBoardDatabase *tmpBd;

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
    NNshWinSetFieldText(frm, FLDID_PAGENUM, true,
                        (NNshGlobal->tempInfo)->PageLabel,
                        StrLen((NNshGlobal->tempInfo)->PageLabel));

    // 取得する先頭のINDEX番号を決定する
    startItem = pageNum * (nofRow - 1);

    // 画面に表示する板名ラベルを作成しながら、画面を設定する
    MemSet((NNshGlobal->tempInfo)->BBSName_Temp, 
           (NNshGlobal->tempInfo)->BBSName_size, 0x00);
    namePtr = (NNshGlobal->tempInfo)->BBSName_Temp;
    chkPtr  = (NNshGlobal->tempInfo)->checkBoxState;
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    for (lp = 0; lp < nofRow; lp++)
    {
        // データの最大数を超えたかどうかのチェック
        if ((startItem + lp) < (NNshGlobal->tempInfo)->nofRecords)
        {
            // 板名ラベルをnamePtrにコピーする(板名 + boardNickを表示する)
            (void) GetRecordReadOnly_NNsh(dbRef, (lp + startItem), &bdH, 
                                          (void **) &tmpBd);
            StrCopy(namePtr, tmpBd->boardName);
            StrCat (namePtr, "(");
            StrCat (namePtr, tmpBd->boardNick);
            ReleaseRecordReadOnly_NNsh(dbRef, bdH);
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
    CloseDatabase_NNsh(dbRef);
    
    // テーブルのデータを表示できるようにする
    TblSetColumnUsable(tableP, 0, true);
    TblSetColumnUsable(tableP, 1, true);

    // 画面の再表示
    TblEraseTable(tableP);
    TblDrawTable (tableP);

    // 検索したテーブルを選択する
    if (selection < nofRow)
    {
        TblSelectItem(tableP, selection, 0);
    }
    return;
}

/*=========================================================================*/
/*   Function : Handler_SelectBBS                                          */
/*                                           ＢＢＳ管理のイベントハンドラ  */
/*=========================================================================*/
Boolean Handler_SelectBBS(EventType *event)
{
    Int16        addPage = 0;
    UInt16       nofRow, recNum  = 0;
    Char        *ttl;
#if defined(USE_HANDERA) || defined(USE_PIN_DIA)
    FormType    *frm;
#endif

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

#ifdef USE_HANDERA
      case displayExtentChangedEvent:
        // HandEra/DIA用画面サイズ変更イベント
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            FrmEraseForm(frm);
            HandEraResizeForm(frm);
            FrmDrawForm(frm);

            //////////  スレ選択画面を更新する  /////////
            display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                                  NNSH_ITEM_LASTITEM);
        } 
        break;
#endif  // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            // HandEra/DIA用画面サイズ変更イベント
            frm = FrmGetActiveForm();
            // FrmEraseForm(frm);
            if (HandEraResizeForm(frm) == true)
            {
                // 画面のリサイズが行われたときだけ更新
                FrmDrawForm(frm);

                //////////  スレ選択画面を更新する  /////////
                display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                                      NNSH_ITEM_LASTITEM);
            }
        }
        break;
#endif

      case menuEvent:
        switch (event->data.menu.itemID)
        {
          case MNUID_NET_DISCONN:
            // 回線切断
            NNshNet_LineHangup();
            break;

          case MNUID_SEARCH_TITLE:
            ttl = ((NNshGlobal->tempInfo)->searchName);
            MemSet(ttl, MAX_BOARDNAME, 0x00);
            // タイトル検索
            // (文字列入力ウィンドウを開き、データを獲得)
            if (DataInputDialog(NNSH_INPUTWIN_SEARCH, ttl, MAX_BOARDNAME,
                                NNSH_DIALOG_USE_SEARCH, NULL) != true)
            {
                break;
            }
            addPage = -1;
            recNum = 0;
            // not break;  続けて検索へ...

          case MNUID_SEARCH_NEXT:
            // 検索を実行する(実処理)
            if (addPage == 0)
            {
                // 表示しているページの一番下から検索
                recNum =  (((NNshGlobal->tempInfo)->currentPage + 1) * 
                           ((NNshGlobal->tempInfo)->nofRow - 1));
                if (recNum >= (NNshGlobal->tempInfo)->nofRecords)
                {
                    // レコード数がオーバフローした場合は先頭から
                    recNum = 0;
                }
            }
            ttl = ((NNshGlobal->tempInfo)->searchName);
            if (searchBBSTitle(ttl, NNSH_SEARCH_FORWARD, &recNum) == errNone)
            {
                // (タイトル発見)
                // 現在の設定を「使用状態格納ワーク領域」に反映させる
                set_Selection_Info((NNshGlobal->tempInfo)->currentPage);

                // 表示するページ番号を更新する
                (NNshGlobal->tempInfo)->currentPage =
                               recNum / (((NNshGlobal->tempInfo)->nofRow) - 1);
                nofRow =   recNum % (((NNshGlobal->tempInfo)->nofRow) - 1);

                //////////  スレ選択画面を更新する(表示ページの移動) /////
                display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                                      nofRow);
                return (true);
            }
            NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, ttl, 0);

            // 画面を再表示する
            display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                                  (NNshGlobal->tempInfo)->nofRow);
            break;

          case MNUID_ENTRY_BBS:
            // 現在の設定を「使用状態格納ワーク領域」に反映させる
            set_Selection_Info((NNshGlobal->tempInfo)->currentPage);

            // 使用BBS設定をDBに反映する
            Show_BusyForm(MSG_BBSLISTSETTING_INFO);
            setBBSListItem();
            Hide_BusyForm(true);

            // BBSの個別登録後、画面を閉じる
            entryBBSRecord(NNSH_NOTENTRY_BBS);
            closeForm_SelectBBS();
            FrmGotoForm(NNshGlobal->backFormId);
            break;

          case MNUID_UPDATE_BBS:
            // BBS情報詳細
            return (detail_SelectBBS());
            break;

          default:
            // 上記以外 
            break;
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
        display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                              NNSH_ITEM_LASTITEM);
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
        // 使用状態格納ワーク領域の確保(確保済みだった場合には解放してから登録)
        MEMFREE_PTR((NNshGlobal->tempInfo)->checkBoxState);
        MEMFREE_PTR((NNshGlobal->tempInfo));
    }
    (NNshGlobal->tempInfo) = MEMALLOC_PTR(sizeof(NNshBBS_Temp_Info));
    if ((NNshGlobal->tempInfo) == NULL)
    {
        // 領域確保エラー、元の一覧画面に戻る
        FrmGotoForm(NNshGlobal->backFormId);
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

    bufSize = (bbsCnt + 1) * sizeof(UInt16) + 8;
    (NNshGlobal->tempInfo)->checkBoxState = (UInt16 *) MEMALLOC_PTR(bufSize);
    if ((NNshGlobal->tempInfo)->checkBoxState == NULL)
    {
        //  バッファ領域確保エラー
        NNsh_DebugMessage(ALTID_ERROR,MSG_MEMORY_ALLOC_FAIL,"size:",bufSize);
        goto ERROR_END;
    }
    MemSet((NNshGlobal->tempInfo)->checkBoxState, bufSize, 0x00);

    // BBSの使用中状態を一時領域に反映させる
    for (lp = 0; lp < bbsCnt; lp++)
    {
        ret = GetRecord_NNsh(dbBBSRef, lp, sizeof(tmpBd), &tmpBd);
        if (ret == errNone)
        {
            (NNshGlobal->tempInfo)->checkBoxState[lp] = ((tmpBd.state)&(NNSH_BBSSTATE_VISIBLE));
        }
    }

    // 板名表示領域の確保(確保済みだった場合には解放してから再度確保する)
    MEMFREE_PTR((NNshGlobal->tempInfo)->BBSName_Temp);
    bufSize = (nofRow + 1) * 
                    (MAX_BOARDNAME + MAX_NICKNAME + 3 + 1) * sizeof(Char) + 8;
    (NNshGlobal->tempInfo)->BBSName_Temp = (Char *) MEMALLOC_PTR(bufSize);
    if ((NNshGlobal->tempInfo)->BBSName_Temp == NULL)
    {
        //  バッファ領域確保エラー
        NNsh_DebugMessage(ALTID_ERROR,MSG_MEMORY_ALLOC_FAIL,"Size:",bufSize);

        // チェック状態の領域を開放
        MEMFREE_PTR((NNshGlobal->tempInfo)->checkBoxState);
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
    Hide_BusyForm(true);

    //////////  スレ選択画面を表示する  /////////
    display_BBS_Selection((NNshGlobal->tempInfo)->currentPage,
                           NNSH_ITEM_LASTITEM);

    // BBSタイトル情報はクリアする
    if (NNshGlobal->boardIdxH != 0)
    {
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;
    }
    MEMFREE_PTR((NNshGlobal->bbsTitles));
    return;

ERROR_END:
    // データベースのクローズ
    CloseDatabase_NNsh(dbBBSRef);

    // BUSYウィンドウを消去する
    Hide_BusyForm(true);

    // データをクリアする
    closeForm_SelectBBS();

    // スレ一覧画面に戻る
    FrmGotoForm(NNshGlobal->backFormId);
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
        (selBBS == NNSH_SELBBS_NOTREAD)||
        (selBBS == NNSH_SELBBS_CUSTOM1)||
        (selBBS == NNSH_SELBBS_CUSTOM2)||
        (selBBS == NNSH_SELBBS_CUSTOM3)||
        (selBBS == NNSH_SELBBS_CUSTOM4)||
        (selBBS == NNSH_SELBBS_CUSTOM5))
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
    
    // BBS一覧が特殊ボードの時はエラー
    if (selBBS <= NNSH_SELBBS_CUSTOM2)
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

/*=========================================================================*/
/*   Function :   Create_BBS_INDEX                                         */
/*                                                      BBS一覧の作成処理  */
/*=========================================================================*/
Err Create_BBS_INDEX(Char **bdLst, UInt16 *cnt)
{
    Err                   ret = errNone;
    DmOpenRef             bbsRef;
    UInt16                nofBBS, tempSize, lp, useCnt, *idxP;
    Char                 *ptr;
    MemHandle             bdH;
    NNshBoardDatabase    *tmpBd;
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
        CloseDatabase_NNsh(bbsRef);

        // VFSからＤＢをリストア(もし可能なら)
        ret = RestoreDBtoVFS(true);
        if (ret != errNone)
        {
            // BBSデータベースが存在しなかった場合、警告を表示する
            NNsh_WarningMessage(ALTID_WARN,
                                MSG_BOARDDB_NONE1, MSG_BOARDDB_NONE2, 0);
        }
#ifdef ENTRY_INITIAL_BBSDATA_AUTOMATICALLY
        // 「参照ログ」をＢＢＳデータとして登録する
        OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
        tmpBd = MEMALLOC_PTR(sizeof(NNshBoardDatabase) + MARGIN);
        if (tmpBd == NULL)
        {
            // メモリ確保失敗...終了する
            return (~errNone);
        }
        MemSet(tmpBd, (sizeof(NNshBoardDatabase) + MARGIN), 0x00);
        StrCopy(tmpBd->boardNick, OFFLINE_THREAD_NICK);
        StrCopy(tmpBd->boardURL,  OFFLINE_THREAD_URL);
        StrCopy(tmpBd->boardName, OFFLINE_THREAD_NAME);
        tmpBd->state = NNSH_BBSSTATE_VISIBLE;
        (void) EntryRecord_NNsh(bbsRef, sizeof(NNshBoardDatabase), tmpBd);
        MEMFREE_PTR(tmpBd);
        CloseDatabase_NNsh(bbsRef);

        // 再度開きなおし
        OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
        GetDBCount_NNsh(bbsRef, &nofBBS);
        if (nofBBS == 0)
        {
            // これはありえないはずだが...
            return (~errNone);
        }
#else  // #ifdef ENTRY_INITIAL_BBSDATA_AUTOMATICALLY
        return (~errNone);
#endif  // #ifdef ENTRY_INITIAL_BBSDATA_AUTOMATICALLY
    }

    // 使用中BBS数をカウントする
    useCnt = 0;
    for (lp = 0; lp < nofBBS; lp++)
    {
        (void) GetRecordReadOnly_NNsh(bbsRef, lp, &bdH, (void **) &tmpBd);
        if ((tmpBd->state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            useCnt++;
        } 
        ReleaseRecordReadOnly_NNsh(bbsRef, bdH);
    }
    if (useCnt == 0)
    {
        // "作成中"表示の削除
        Hide_BusyForm(false);
        CloseDatabase_NNsh(bbsRef);

        // 使用するデータベースが設定されていなかった場合、警告を表示する
        NNsh_WarningMessage(ALTID_WARN,
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
       Hide_BusyForm(false);

       // 領域確保エラー
       ret = ~errNone;
       NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
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
    *bdLst   = (Char *) MEMALLOC_PTR(tempSize);
    if (*bdLst == NULL)
    {
        MemHandleUnlock(NNshGlobal->boardIdxH);
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;

        // "作成中"表示の削除
        Hide_BusyForm(false);

        // 領域確保エラー
        ret = ~errNone;
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                          " size:", tempSize);

        // データベースのクローズ
        CloseDatabase_NNsh(bbsRef);
        return (~errNone);
    }
    MemSet(*bdLst, tempSize, 0x00);
    ptr = *bdLst;
    *cnt  = 0;

    // BBS一覧の先頭に「未読あり」「取得済み全て」「お気に入り」を追加する
    StrCopy(ptr, NOTREAD_THREAD_NAME);
    ptr   = ptr + sizeof(NOTREAD_THREAD_NAME); // Terminatorも含めて考えてる
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, GETALL_THREAD_NAME);
    ptr   = ptr + sizeof(GETALL_THREAD_NAME); // Terminatorも含めて考えてる
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, FAVORITE_THREAD_NAME);
    ptr   = ptr + sizeof(FAVORITE_THREAD_NAME); // Terminatorも含めて考えてる
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, CUSTOM1_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom1.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, CUSTOM2_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom2.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, CUSTOM3_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom3.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, CUSTOM4_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom4.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    StrCopy(ptr, CUSTOM5_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom5.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // ダミーデータ(この値には価値なし)
    idxP++;

    // (使用中の)BBSタイトル一覧とインデックスを作成する
    for (lp = 0; lp < nofBBS; lp++)
    {
        (void) GetRecordReadOnly_NNsh(bbsRef, lp, &bdH, (void **) &tmpBd);
        if ((tmpBd->state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            // 板名称をコピーする
            StrNCopy(ptr, tmpBd->boardName, LIMIT_BOARDNAME);
            ptr = ptr + StrLen(ptr) + 1;   // 1 は'\0'の意味、(リストで使用)
            (*cnt)++;
            *idxP = lp;
            idxP++;
        }
        ReleaseRecordReadOnly_NNsh(bbsRef, bdH);
    }
    NNshGlobal->useBBS = *cnt;
    MemHandleUnlock(NNshGlobal->boardIdxH);

    // "作成中"表示の削除
    Hide_BusyForm(false);

    // データベースのクローズ
    CloseDatabase_NNsh(bbsRef);
    return (ret);
}
