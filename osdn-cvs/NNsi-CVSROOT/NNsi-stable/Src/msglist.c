/*============================================================================*
 *  FILE: 
 *     msglist.c
 *
 *  Description: 
 *     Message(Thread) list drawings for NNsh. 
 *
 *===========================================================================*/
#define MSGLIST_C
#include "local.h"

/*-------------------------------------------------------------------------*/
/*   Function : CheckDispList                                              */
/*                                         画面に表示する/しないを判定する */
/*-------------------------------------------------------------------------*/
Boolean CheckDispList(UInt16 bbsIndex, NNshSubjectDatabase *subjP,
                                                        NNshBoardDatabase *bdP)
{
    Char *ptr, *match1, *match2;
    Char  fileName[MAX_THREADFILENAME + MARGIN];

    // 表示する/しないの条件判定(板情報によって場合分け)
    switch (bbsIndex)
    { 
      case NNSH_SELBBS_GETALL:
        // 取得済み全てを表示
        if ((subjP->state == NNSH_SUBJSTATUS_NOT_YET)||
            (((NNshGlobal->NNsiParam)->notListReadOnly != 0)&&
             ((subjP->boardNick[0]  == '!')&&(subjP->boardNick[1]  == 'G'))))
        {
            //  「未取得」かもしくは参照ログかつ「参照ログは表示しない」の
            // 場合には表示しない
            return (false);
        }
        break;

      case NNSH_SELBBS_FAVORITE:
        // お気に入り指定を表示
        if ((subjP->msgAttribute & NNSH_MSGATTR_FAVOR) < (NNshGlobal->NNsiParam)->displayFavorLevel)
        {
            // 「お気に入り」ではないので表示しない
            return (false);
        }
        break;

      case NNSH_SELBBS_NOTREAD:
        // 未読ありを表示
        if (((NNshGlobal->NNsiParam)->notListReadOnlyNew != 0)&&
             ((subjP->boardNick[0]  == '!')&&(subjP->boardNick[1]  == 'G')))
        {
            //　参照ログかつ「未読ありにはチェックなし」が入っていた場合には
            // 表示しない
            return (false);
        }        
        if ((subjP->state != NNSH_SUBJSTATUS_NEW)&&
            (subjP->state != NNSH_SUBJSTATUS_UPDATE)&&
            (subjP->state != NNSH_SUBJSTATUS_REMAIN))
        {
            // 未読があるスレではないので表示しない
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM1:
        // ユーザ設定１の条件確認
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom1)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM2:
        // ユーザ設定２の条件確認
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom2)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM3:
        // ユーザ設定３の条件確認
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom3)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM4:
        // ユーザ設定４の条件確認
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom4)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM5:
        // ユーザ設定５の条件確認
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom5)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_OFFLINE:
        // 参照ログ
#ifdef USE_STRSTR
        if (StrCompare(subjP->boardNick, OFFLINE_THREAD_NICK) != 0)
#else
        if ((subjP->boardNick[0]  != '!')||
            (subjP->boardNick[1]  != 'G'))
#endif
        {
            // スレの所属が違うので表示しない
            return (false);
        }
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_DIROFFLINE) != 0)
        {
            if (subjP->dirIndex != (NNshGlobal->NNsiParam)->readOnlySelection)
            {
                // 保管されているディレクトリが違ったので表示しない
                return (false);
            }
        }
        if ((NNshGlobal->NNsiParam)->getLogSiteListMode != 0)
        {
            // 一覧画面を参照ログ一覧サイト別表示モードとして使用する場合。。。
            MemSet(fileName, sizeof(fileName), 0x00);
            StrCopy(fileName, FILE_LOGCHARGE_PREFIX);
            NUMCATI(fileName, (NNshGlobal->NNsiParam)->selectedTitleItemForGetLog);
            StrCat (fileName, "-");
            if (StrStr(subjP->threadFileName, fileName) == NULL)
            {
                // 該当のログではなかった、、、終了する
                return (false);
            }
        }
        break;

      default:
        // 「お気に入り」でも「取得済み全て」でも「未読あり」でもない場合
        if (StrCompare(subjP->boardNick, bdP->boardNick) != 0)
        {
            // スレの所属が違うので表示しない
            return (false);
        }
        break;
    }

    if ((NNshGlobal->NNsiParam)->searchPickupMode != 0)
    {
        // 絞込み検索モード... 

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

        // タイトルモードの検索
        if (StrStr(subjP->threadTitle, ptr) == NULL)
        {
            if ((NNshGlobal->NNsiParam)->searchCaseless == 0)
            {
                MemHandleUnlock(NNshGlobal->searchTitleH);
                return (false);
            }

            // 大小文字変換して比較...
            match1 = MEMALLOC_PTR(MAX_THREADNAME + MARGIN);
            match2 = MEMALLOC_PTR(StrLen(ptr) + MARGIN);
            (void) StrToLower(match1, subjP->threadTitle);
            (void) StrToLower(match2, ptr);
            if (StrStr(match1, match2) == NULL)
            {
                MEMFREE_PTR(match2);
                MEMFREE_PTR(match1);
                MemHandleUnlock(NNshGlobal->searchTitleH);
                return (false);
            }
            MEMFREE_PTR(match2);
            MEMFREE_PTR(match1);
        }
        MemHandleUnlock(NNshGlobal->searchTitleH);
    }

    // 表示するスレと判定
    return (true);
}                               

/*-------------------------------------------------------------------------*/
/*   Function : copyThreadTitleString                                      */
/*                                スレタイトル文字列をコピー(データ変換付) */
/*-------------------------------------------------------------------------*/
static void copyThreadTitleString(Char  *dest, Char *src, UInt16 length,
                                  UInt16 state, UInt16 msgNumber)
{
    UInt16  msgAttribute;
    UInt16 msgState, size;
    Char   *dst;

    // エラー状態が発生したとき、メッセージの先頭に '#'を表示する
    dst = dest;
    if (((state >> 8)&(NNSH_MSGATTR_ERROR)) == NNSH_MSGATTR_ERROR)
    {
        *dst = '#';
        dst++;
    }

    // 取得予約中のとき、メッセージの先頭に '!'を表示する
    if (((state >> 8)&(NNSH_MSGATTR_GETRESERVE)) == NNSH_MSGATTR_GETRESERVE)
    {
        *dst = '!';
        dst++;
    }

    // メッセージの状態を記号で反映
    msgState = (state & 0xff);
    switch (msgState)
    {
      case NNSH_SUBJSTATUS_NEW:
        /** 新規取得(再取得) **/
        *dst = '*';
        break;

      case NNSH_SUBJSTATUS_UPDATE:
        /** メッセージ更新 **/
        *dst = '+';
        break;

      case NNSH_SUBJSTATUS_REMAIN:
        /** 未読あり **/
        *dst = '-';
        break;

      case NNSH_SUBJSTATUS_ALREADY:
        /** メッセージ全部読んだ **/
        *dst = ':';
        break;
#ifdef USE_MARK_SUBJSTATUS_OVER
      case NNSH_SUBJSTATUS_OVER:
        /** レス数の最大を超えた **/
        *dst = '~';
#endif
      case NNSH_SUBJSTATUS_NOT_YET:
      default:                
        /** メッセージ未取得 **/
        *dst = ' ';
        break;
    }
    dst++;

    // メッセージ数のコピー部分
    if ((NNshGlobal->NNsiParam)->printNofMessage != 0)
    {
        switch (msgState)
        {
          case NNSH_SUBJSTATUS_UNKNOWN:
            // レス数未解析なことを表示する
            StrCat(dst, "[???]");
            break;
          case NNSH_SUBJSTATUS_NEW:
            // 新たに取得したことを表示する
            StrCat(dst, "[NEW]");
            break;
          case NNSH_SUBJSTATUS_UPDATE:
            // 更新があったことを表示する
            StrCat(dst, "[New]");
            break;

          default:
            // 取得メッセージ数を表示する
            if (((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum == 2)||((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum == 3))
            {
                // スレレベルを表示に追加する
                msgAttribute = ((state >> 8)&(NNSH_MSGATTR_FAVOR));
                if (msgAttribute == NNSH_MSGATTR_FAVOR_L1)
                {
                    StrCat(dst, "[L.");
                }                
                else if (msgAttribute == NNSH_MSGATTR_FAVOR_L2)
                {
                    StrCat(dst, "[1.");
                }                
                else if (msgAttribute == NNSH_MSGATTR_FAVOR_L3)
                {
                    StrCat(dst, "[2.");
                }                
                else if (msgAttribute == NNSH_MSGATTR_FAVOR_L4)
                {
                    StrCat(dst, "[3.");
                }                
                else if (msgAttribute == NNSH_MSGATTR_FAVOR)
                {
                    StrCat(dst, "[H.");
                }                
                else
                {
                    StrCat(dst, "[");
                }                
            }
            else
            {
                StrCat(dst, "[");
            }
            (void) NUMCATI(dst, msgNumber);
            StrCat(dst, "]");
            break;
        }
    }
    size = StrLen(dest);
    dst  = dest + size;

    // スレタイトルのコピー
    StrNCopyHanZen(dst, src, (length - size - 1));
    return;
}

/*=========================================================================*/
/*   Function : ClearMsgTitleInfo                                          */
/*                                           スレタイ一覧の関連領域を開放  */
/*=========================================================================*/
void ClearMsgTitleInfo(void)
{
    MEMFREE_PTR(NNshGlobal->msgListIndex);
    MEMFREE_PTR(NNshGlobal->msgListStrings);
    MEMFREE_PTR(NNshGlobal->msgTitleIndex);
    MEMFREE_PTR(NNshGlobal->msgTitleStatus);
    MEMFREE_PTR(NNshGlobal->msgNumbers);

    NNshGlobal->nofTitleItems = 0;

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : AllocMsgTitleInfo                                          */
/*                                           スレタイ一覧の関連領域を確保  */
/*-------------------------------------------------------------------------*/
static Err AllocMsgTitleInfo(UInt16 nofItems)
{
    UInt16 allocSize;

    // スレ一覧→スレレコード番号変換テーブル領域の初期化
    allocSize = sizeof(UInt16) * (nofItems + 2);
    NNshGlobal->msgListIndex = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgListIndex == NULL)
    {
        return (~errNone + 10);
    }
    MemSet(NNshGlobal->msgListIndex, allocSize, 0x00);

    // スレタイトルの先頭位置格納領域の初期化
    allocSize = sizeof(Char *) * (nofItems + 2);
    NNshGlobal->msgTitleIndex = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgTitleIndex == NULL)
    {
        MEMFREE_PTR(NNshGlobal->msgListIndex);
        return (~errNone + 11);    
    }
    MemSet(NNshGlobal->msgTitleIndex, allocSize, 0x00);

    // スレタイトル格納領域の初期化
    allocSize = (LIMIT_TITLENAME_DEFAULT + 10 + MARGIN) * (nofItems + 1);
    NNshGlobal->msgListStrings = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgListStrings == NULL)
    {
        MEMFREE_PTR(NNshGlobal->msgListIndex);
        MEMFREE_PTR(NNshGlobal->msgTitleIndex);
        return (~errNone + 12);
    }
    MemSet(NNshGlobal->msgListStrings, allocSize, 0x00);

    // スレタイトル状態格納領域の初期化
    allocSize = sizeof(UInt16) * (nofItems + 2);
    NNshGlobal->msgTitleStatus = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgTitleStatus == NULL)
    {
        MEMFREE_PTR(NNshGlobal->msgListIndex);
        MEMFREE_PTR(NNshGlobal->msgTitleIndex);
        MEMFREE_PTR(NNshGlobal->msgListStrings);
        return (~errNone + 13);
    }
    MemSet(NNshGlobal->msgTitleStatus, allocSize, 0x00);

    // スレメッセージ数格納領域の初期化
    allocSize = sizeof(UInt16) * (nofItems + 2);
    NNshGlobal->msgNumbers = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgNumbers == NULL)
    {
        MEMFREE_PTR(NNshGlobal->msgListIndex);
        MEMFREE_PTR(NNshGlobal->msgTitleIndex);
        MEMFREE_PTR(NNshGlobal->msgListStrings);
        MEMFREE_PTR(NNshGlobal->msgTitleStatus);
        return (~errNone + 14);
    }
    MemSet(NNshGlobal->msgNumbers, allocSize, 0x00);

    NNshGlobal->nofTitleItems = 0;

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   function :   create_TitleList                                         */
/*                                            メッセージタイトル一覧を作成 */
/*-------------------------------------------------------------------------*/
static Err create_TitleList(UInt16 bbsIndex, UInt16 areaCount,
                             Char  *buffer,   Int16  command)
{
    Err                  ret = ~errNone;
    Char                *ptr;
    Int16                stepCnt;
    UInt16               dataIndexMax, loop, startRec, endRec, tempIndex;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subjDb;
    NNshBoardDatabase    bbsData;
    MemHandle            msgH;

#ifdef DEBUG
    Char debugMsg[129];
#endif

    // 合致タイトル数の初期化
    NNshGlobal->nofTitleItems  = 0;
    endRec                     = 0;
    ptr                        = buffer;

    switch (bbsIndex)
    {
      case NNSH_SELBBS_NOTREAD:     // 未読あり
      case NNSH_SELBBS_GETALL:      // 取得済み全て
      case NNSH_SELBBS_FAVORITE:    // お気に入りスレ
        // BBS情報は取得しない(逐次取得)
        break;

      case NNSH_SELBBS_CUSTOM1:     // CUSTOM1
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom1.boardNick, &bbsData) != errNone)
        {
            // 「一覧なし」設定部分へジャンプ
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_CUSTOM2:     // CUSTOM2
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom2.boardNick, &bbsData) != errNone)
        {
            // 「一覧なし」設定部分へジャンプ
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_CUSTOM3:     // CUSTOM3
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom3.boardNick, &bbsData) != errNone)
        {
            // 「一覧なし」設定部分へジャンプ
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_CUSTOM4:     // CUSTOM4
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom4.boardNick, &bbsData) != errNone)
        {
            // 「一覧なし」設定部分へジャンプ
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_CUSTOM5:     // CUSTOM5
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom5.boardNick, &bbsData) != errNone)
        {
            // 「一覧なし」設定部分へジャンプ
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_OFFLINE:
        // 参照ログ
        if (Get_BBS_Info(bbsIndex, &bbsData) != errNone)
        {
            // 「一覧なし」設定部分へジャンプ
            goto RECORD_NOTHING;
        }
        break;

      default:
        // 特殊BBS以外のとき
        if (Get_BBS_Info(bbsIndex, &bbsData) != errNone)
        {
            // 「一覧なし」設定部分へジャンプ
            goto RECORD_NOTHING;
        }
        break;
    }
    ret = errNone;

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &dataIndexMax);

    // １件もデータが登録されていなかった場合
    if (dataIndexMax == 0)
    {
        // 「一覧なし」設定部分へジャンプ
        goto RECORD_NOTHING;
    }

    //////////////////// 現在の表示状態を確認する ///////////////////////
    if (command == NNSH_STEP_UPDATE)
    {
        // スレ一覧を更新する指示だった場合には、状態を"上限"に変更する
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_UPPERLIMIT;
    }

    switch ((NNshGlobal->NNsiParam)->titleDispState)
    {
      case NNSH_DISP_UPPERLIMIT:
      case NNSH_DISP_ALL:
        // 先頭から検索する
        (NNshGlobal->NNsiParam)->startTitleRec = (dataIndexMax - 1);
        break;

      case NNSH_DISP_LOWERLIMIT:
        // 末尾から検索する
        (NNshGlobal->NNsiParam)->endTitleRec = 0;
        break;

      default:
        // 何もしない
        break;
    }

    //////////////////// 表示要求を確認する ///////////////////////
    switch (command)
    {
      case NNSH_STEP_PAGEUP:
        // 1ページ"下"へ移動させる        (レコード番号の大→小検索）
        stepCnt  = -1;
        startRec = (NNshGlobal->NNsiParam)->endTitleRec;
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_HALFWAY;
        break;

      case NNSH_STEP_PAGEDOWN:
        // 1ページ"上"へ移動させる       （レコード番号の小→大検索）
        stepCnt  = 1;
        startRec = (NNshGlobal->NNsiParam)->startTitleRec;
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_HALFWAY;
        break;

      case NNSH_STEP_TO_BOTTOM:
        // 一覧の末尾へ移動させる        （レコード番号の０から検索）
        stepCnt  = 1;
        startRec = 0;
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_LOWERLIMIT;
        break;

      case NNSH_STEP_REDRAW:
        // 現在表示中のページを再描画する（前回の表示レコード番号の先頭から）
        stepCnt  = -1;
        startRec = (NNshGlobal->NNsiParam)->startTitleRec;
        break;

      // case NNSH_STEP_UPDATE:
      // case NNSH_STEP_TO_TOP:
      default:
        // 検索条件の先頭から再表示する  （レコード番号の最大から検索）
        stepCnt  = -1;
        startRec = (dataIndexMax - 1);
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_UPPERLIMIT;
        break;
    }

    //////////////// 表示するスレのレコード番号を特定 /////////////////////////
    for (loop = startRec + 1;
         ((loop > 0)&&(loop <= dataIndexMax));
         loop = loop + stepCnt)
    {
        // 表示レコードを取得する
        GetRecordReadOnly_NNsh(dbRef, (loop - 1), &msgH, (void **) &subjDb);

        // 表示する/しないの条件判定
        if (CheckDispList(bbsIndex, subjDb, &bbsData) == false)
        {
            // 表示しないスレと判定
            ReleaseRecordReadOnly_NNsh(dbRef, msgH);
            continue;
        }
        ReleaseRecordReadOnly_NNsh(dbRef, msgH);

        if (NNshGlobal->nofTitleItems >= areaCount)
        {
            // リスト表示数の表示予定数 + 1が終了したので、ここで抜ける
            endRec = NNshGlobal->msgListIndex[(NNshGlobal->nofTitleItems - 1)];
            break;                                          // ループを抜ける
        }

        // 表示するレコード番号を記憶しておく
        NNshGlobal->msgListIndex[NNshGlobal->nofTitleItems] = (loop - 1);

        // レコード番号を記録する
        (NNshGlobal->nofTitleItems)++;

    }
    
    //////////  ループの終了した条件を確認する
    if (loop == 0)                   // ： 先頭まで検索
    {
        // レコードの末尾から先頭まで検索したかどうかチェックする
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_UPPERLIMIT)||
             ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_ALL))
        {
            // 全て表示
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_ALL;
        }
        else
        {
            // 下限
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_LOWERLIMIT;
        }
        endRec = loop;
    }
    else if (loop > dataIndexMax)    // 末尾まで検索
    {
        // レコードの先頭から末尾まで検索したかどうかチェックする
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_LOWERLIMIT)||
            ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_ALL))
        {
            // 全て表示
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_ALL;
        }
        else
        {
            // 上限
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_UPPERLIMIT;
        }
        endRec = (dataIndexMax - 1);
    }
    else   //   0 < loop <= dataIndexMax の時...
    {
        if (startRec == 0)
        {
            // 下限
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_LOWERLIMIT;
        }
        else if (startRec == (dataIndexMax - 1))
        {
            // 上限
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_UPPERLIMIT;
        }
        else
        {
            // 途中まで表示
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_HALFWAY;
        }
    }

    //////// 備忘録 ///////////////////////////////////////////////////////////
    //   現状、未読が減った場合、NNshGlobal->nofTitleItemsが0でここに到達    //
    // することがある。(未読ありの最終ページを参照し終わったとき、スレを削除 //
    // してそのページに表示するスレがなくなった場合)                         //
    //  「なし」と表示されるが、実際にタブを選択して更新するとスレは存在する //
    // ので、対処が必要。(この関数を呼び出す側で対処した)                    //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////// スレ一覧データの並べ替え ////////////////////////////
    if (stepCnt > 0)
    {
        // レコード番号の入れ替え(降順に整列させる)
        for (loop = 0; loop < (NNshGlobal->nofTitleItems / 2) ; loop++)
        {
            tempIndex = NNshGlobal->msgListIndex[loop];

            NNshGlobal->msgListIndex[loop] =
               NNshGlobal->msgListIndex[NNshGlobal->nofTitleItems - (loop + 1)];

            NNshGlobal->msgListIndex[NNshGlobal->nofTitleItems - (loop + 1)] =
                                                                      tempIndex;
        }
    }

    //////////////////// 開始/終了レコード番号の格納 //////////////////////
    if (startRec < endRec)
    {
        // 開始/終了のレコード番号を記録する
        (NNshGlobal->NNsiParam)->startTitleRec = endRec;
        (NNshGlobal->NNsiParam)->endTitleRec   = startRec;
    }
    else
    {
        // 開始/終了のレコード番号を記録する
        (NNshGlobal->NNsiParam)->startTitleRec = startRec;
        (NNshGlobal->NNsiParam)->endTitleRec   = endRec;     
    }

#ifdef DEBUG
    ///////////////////////////////////////// DEBUG MESSAGE { /////////////////
    MemSet (debugMsg, sizeof(debugMsg), 0x00);
    StrCopy(debugMsg, "startRec:");
    NUMCATI(debugMsg, (NNshGlobal->NNsiParam)->startTitleRec);
    StrCat (debugMsg, " endRec:");
    NUMCATI(debugMsg, (NNshGlobal->NNsiParam)->endTitleRec);
    NNsh_DebugMessage(ALTID_INFO, debugMsg, " (NNshGlobal->NNsiParam)->titleDispState:", 
                      (NNshGlobal->NNsiParam)->titleDispState);
    ///////////////////////////////////////// } DEBUG MESSAGE /////////////////
#endif  // #ifdef DEBUG


    ////////////////////////  スレ一覧データを格納する  //////////////////////
    for (loop = 0; loop < NNshGlobal->nofTitleItems; loop++)
    {
        // 表示レコードを取得
        GetRecordReadOnly_NNsh(dbRef, NNshGlobal->msgListIndex[loop],
                                                    &msgH, (void **) &subjDb);

        // メッセージ数が規定よりオーバしていたら、その色にしてしまう
        if ((subjDb->maxLoc > NNSH_MESSAGE_LIMIT)&&
            (subjDb->state == NNSH_SUBJSTATUS_ALREADY))
        {
            NNshGlobal->msgTitleStatus[loop] = 
                        ((NNSH_SUBJSTATUS_OVER)|((subjDb->msgAttribute << 8)));
        }
        else
        {
            NNshGlobal->msgTitleStatus[loop] = 
                               ((subjDb->state)|((subjDb->msgAttribute << 8)));
        }

        // スレ格納数を記録する
        if (((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum == 0)||((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum == 2))
        {
            NNshGlobal->msgNumbers[loop] = subjDb->maxLoc;
        }
        else
        {
            // 未読数を格納する
            NNshGlobal->msgNumbers[loop] = subjDb->maxLoc - subjDb->currentLoc;
        }

        // スレタイトルを保存する
        copyThreadTitleString(ptr, subjDb->threadTitle,
                              (LIMIT_TITLENAME_DEFAULT + 10),
                              NNshGlobal->msgTitleStatus[loop],
                              NNshGlobal->msgNumbers[loop]);

        // スレタイトルの先頭ポインタを保存し、次の格納領域へ移動させる
        NNshGlobal->msgTitleIndex [loop] = ptr;
        ptr = ptr + StrLen(ptr) + 1;

        // レコード領域を開放する
        ReleaseRecordReadOnly_NNsh(dbRef, msgH);
    }

RECORD_NOTHING:
    // データベースをクローズする
    CloseDatabase_NNsh(dbRef);


    //////////////////// 該当するスレが１件もなかった場合 /////////////////////
    if (NNshGlobal->nofTitleItems == 0)
    {
        // 「一覧なし」を設定する
        StrCopy(buffer, MSG_SUBJECT_DEFAULT);
        (NNshGlobal->NNsiParam)->titleDispState     = NNSH_DISP_NOTHING;
        NNshGlobal->msgListIndex  [0] = 0;
        NNshGlobal->msgTitleIndex [0] = buffer;
        NNshGlobal->msgNumbers    [0] = 0;
        NNshGlobal->msgTitleStatus[0] = 0;
        ret = (NNSH_ERRORCODE_THREADNOTHING);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   Update_Thread_List                                       */
/*                                                      スレ一覧表示の更新 */
/*=========================================================================*/
Boolean Update_Thread_List(UInt16 bbsIdx, UInt16 selItem, UInt16 step)
{
    Err               ret;
    Char             *ptr;
    FormType         *frm;
    RectangleType     dimF;
    UInt16            fontID, nlines, fontHeight, threadStatus;

    // アクティブなフォームのポインタ取得
    frm = FrmGetActiveForm();
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, GADID_MESSAGE_LIST), &dimF);

    // 描画領域とフォントの情報、表示可能行数を更新
    fontID = ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ?
                           (NNshGlobal->NNsiParam)->currentFont : (NNshGlobal->NNsiParam)->sonyHRFontTitle;

    // 画面の大きさを取得(設定)
    NNsi_UpdateRectangle(&dimF, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                         &fontID, &fontHeight, &nlines);


    // "スレ一覧作成中" の表示
    Show_BusyForm(MSG_READ_THREAD_WAIT);

    // スレタイ関連領域を開放する
    ClearMsgTitleInfo();

    // スレタイ関連メモリ領域を確保する
    if (AllocMsgTitleInfo(nlines) != errNone)
    {
        // スレタイトル関連メモリ領域の確保に失敗
        return (false);
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    /////       スレタイトルの文字列作成
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    ptr = NNshGlobal->msgListStrings;
    threadStatus = (NNshGlobal->NNsiParam)->titleDispState;
    ret = create_TitleList(bbsIdx, nlines, ptr, step);
    if (ret == NNSH_ERRORCODE_THREADNOTHING)
    {
        // スレに表示するものがない、という反応、、、
        if ((step == NNSH_STEP_REDRAW)&&(threadStatus != NNSH_DISP_ALL)&&
            (threadStatus != NNSH_DISP_UPPERLIMIT))
        {
            //  再描画指示で、スレ状態が「全部表示」と「上限」でなかった場合
            // スレタイトル文字列作成関数をもう一度呼び出す
            MemSet(ptr, StrLen(ptr), 0x00);
            selItem = NNSH_ITEM_LASTITEM;
            step    = NNSH_STEP_TO_BOTTOM;
            ret     = create_TitleList(bbsIdx, nlines, ptr, step);
        }
    }

    // 選択スレを指定したアイテム番号に設定する
    // (表示可能行数を超えていたら、一番下のアイテム番号に変更する)
    (NNshGlobal->NNsiParam)->selectedTitleItem = (selItem >= NNshGlobal->nofTitleItems) ?
                                     (NNshGlobal->nofTitleItems - 1): selItem;

    // "スレ一覧作成中"の表示削除
    Hide_BusyForm(false);

#ifdef USE_PIN_DIA
    if (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY)
    {
        // CLIEでシルクのサイズが変更されたときには一旦フォームを消去
        FrmEraseForm(frm);
    }
#endif
    FrmDrawForm(frm);

    // スレ一覧の表示(描画実処理)
    NNsi_WinDrawList((NNshGlobal->NNsiParam)->selectedTitleItem, NNshGlobal->msgTitleIndex,
                     nlines, &dimF, fontHeight);

    return (true);
}

#ifdef USE_COLOR
/*=========================================================================*/
/*   Function :   NNsi_SetTitleColor                                       */
/*                                                    スレタイトルの色設定 */
/*=========================================================================*/
void NNsi_SetTitleColor(UInt16 status)
{
    IndexedColorType color;

    if (((status >> 8)&(NNSH_MSGATTR_ERROR)) == NNSH_MSGATTR_ERROR)
    {
        // エラースレ    
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorError;
        goto SET_COLOR;
    }

    // メッセージの状態で色を設定
    switch (status & 0xff)
    {
      case NNSH_SUBJSTATUS_NEW:
        /** 新規取得(再取得) **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorNew;
        break;

      case NNSH_SUBJSTATUS_UPDATE:
        /** メッセージ更新 **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorUpdate;
        break;

      case NNSH_SUBJSTATUS_REMAIN:
        /** 未読あり **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorRemain;
        break;

      case NNSH_SUBJSTATUS_ALREADY:
        /** メッセージ全部読んだ **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorAlready;
        break;

      case NNSH_SUBJSTATUS_NOT_YET:
        /** メッセージ未取得 **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorNotYet;
        break;

      case NNSH_SUBJSTATUS_OVER:
        /** 1000超 **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorOver;
        break;

      default:                
        /** その他(ありえない？) **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorUnknown;
        break;
    }

SET_COLOR:
    // 色を設定する
    WinSetForeColor(color);
    WinSetTextColor(color);
    return;
}
#endif

/*=========================================================================*/
/*   Function :   NNsi_WinDrawList                                         */
/*                                                    スレ一覧の表示メイン */
/*=========================================================================*/
void NNsi_WinDrawList(UInt16 pointIndex, Char *titleIndex[], UInt16 listItems,
                      RectangleType *dimF, UInt16 height)
{
    UInt16        lp, len;
#ifdef USE_COLOR
    IndexedColorType savedIndex;
    RGBColorType     savedColor, drawColor;
#endif

    // スレ一覧に表示する文字列がなかった場合には、何もせず応答する
    if (NNshGlobal->msgTitleIndex == NULL)
    {
        return;
    }

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        ///////////////////////////////////////////////////////////////
        //  この設定により、一覧リストの領域だけ背景色が変わる...
        // (なんかちょっと格好悪いなぁ。。。)
        ///////////////////////////////////////////////////////////////

        // 背景色を設定する
        WinSetBackColor((IndexedColorType) (NNshGlobal->NNsiParam)->colorBackGround);
    }
#endif

    // 表示領域をクリアする
    NNsi_EraseRectangle(dimF);

#ifdef USE_HIGHDENSITY
    // 高解像度の設定
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFontTitle);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

    // スレタイトルの描画
    for (lp = 0; lp < listItems; lp++)
    {
        if (NNshGlobal->msgTitleIndex[lp] == '\0')
        {
            // タイトルインデックスがない、終了する
            break;
        }

#ifdef USE_COLOR
        // フォントカラーの設定
        if ((NNshGlobal->NNsiParam)->useColor != 0)
        {
            // ちょっと失敗、引数は反則だ...許してください...(^^;
            NNsi_SetTitleColor(NNshGlobal->msgTitleStatus[lp]);
        }
#endif

#ifdef USE_CLIE
        if (NNshGlobal->hrRef != 0)
        {
            // CLIEハイレゾ描画
            HRWinDrawChars(NNshGlobal->hrRef, 
                           titleIndex[lp], StrLen(titleIndex[lp]),
                           dimF->topLeft.x, dimF->topLeft.y);
            if (pointIndex == lp)
            {
                // カーソル位置を反転させる
                HRWinDrawInvertedChars(NNshGlobal->hrRef, 
                                       titleIndex[lp], StrLen(titleIndex[lp]),
                                       dimF->topLeft.x, dimF->topLeft.y);
            }
        }
        else
#endif
        {
            // 通常モード描画
            len = FntWordWrap(titleIndex[lp], dimF->extent.x);
            WinDrawChars(titleIndex[lp],len,dimF->topLeft.x,dimF->topLeft.y);
            if (pointIndex == lp)
            {
                // カーソル位置を反転させる
                WinDrawInvertedChars(titleIndex[lp], len,
                                     dimF->topLeft.x, dimF->topLeft.y);
            }
        }

        // 次の行に描画位置を移動させる
        dimF->topLeft.y = dimF->topLeft.y + height;
    }

#ifdef USE_HIGHDENSITY
    // 高解像度の設定を解除
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),
                      NNshGlobal->os5HighDensity);
        WinSetCoordinateSystem(kCoordinatesStandard);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // 現在の色を保存する
        savedIndex = UIColorGetTableEntryIndex(UIObjectForeground);
        WinIndexToRGB(savedIndex, &savedColor);

        // ボタンの色を変換する
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorButton,    &drawColor);
        UIColorSetTableEntry(UIObjectForeground, &drawColor);
    }
#endif

    // 上限/下限のボタンを表示する
    // (上下ボタンを表示/非表示にする)
    NNsh_UpDownButtonControl(FrmGetActiveForm(), (NNshGlobal->NNsiParam)->titleDispState, BTNID_LISTPREV, BTNID_LISTNEXT);

    if ((NNshGlobal->NNsiParam)->searchPickupMode != 0)
    {
        // 絞込み検索モードのとき...
        FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), BTNID_SEARCHLIST));        
    }
    else
    {
        // 絞込み検索モードではないとき...
        FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), BTNID_SEARCHLIST));
    }

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // ボタンの色を元に戻す
        UIColorSetTableEntry(UIObjectForeground, &savedColor);

        // デフォルトの背景色を設定する
        WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));

        // 文字について、デフォルトの標準色を設定する
        WinSetForeColor(UIColorGetTableEntryIndex(UIObjectForeground));
        WinSetTextColor(UIColorGetTableEntryIndex(UIFieldText));
    }   
#endif

    return;
}


/*=========================================================================*/
/*   Function :   NNsi_WinUpdateList                                       */
/*                                            リストデータの反転情報を更新 */
/*=========================================================================*/
void NNsi_WinUpdateList(UInt16 pointIndex, UInt16 prevIndex,
                        Char *titleIndex[], UInt16 listItems,
                        RectangleType *dimF, UInt16 height)
{
    UInt16         lp, len;
#ifdef USE_COLOR
    IndexedColorType savedIndex;
    RGBColorType     savedColor, drawColor;
#endif

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        ///////////////////////////////////////////////////////////////
        //  この設定により、一覧リストの領域だけ背景色が変わる...
        // (なんかちょっと格好悪いなぁ。。。)
        ///////////////////////////////////////////////////////////////

        // 背景色を設定する
        WinSetBackColor((IndexedColorType) (NNshGlobal->NNsiParam)->colorBackGround);
    }
#endif

#ifdef USE_HIGHDENSITY
    // 高解像度の設定
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif

    // スレタイトルの描画
    for (lp = 0; lp < listItems; lp++)
    {
        if (NNshGlobal->msgTitleIndex[lp] == '\0')
        {
            // タイトルインデックスがない、終了する
            break;
        }

        if (lp == prevIndex)
        {
#ifdef USE_COLOR
            // フォントカラーの設定
            if ((NNshGlobal->NNsiParam)->useColor != 0)
            {
                // ちょっと失敗、引数は反則だ...許してください...(^^;
                NNsi_SetTitleColor(NNshGlobal->msgTitleStatus[lp]);
            }
#endif
#ifdef USE_CLIE
            if (NNshGlobal->hrRef != 0)
            {
                // カーソル位置を元に戻す(CLIEハイレゾ描画)
                HRWinDrawChars(NNshGlobal->hrRef, 
                               titleIndex[lp], StrLen(titleIndex[lp]),
                               dimF->topLeft.x, dimF->topLeft.y);
            }
            else
#endif
            {
                // カーソル位置を元に戻す(通常モード描画)
                len = FntWordWrap(titleIndex[lp], dimF->extent.x);
                WinDrawChars(titleIndex[lp], len,
                             dimF->topLeft.x, dimF->topLeft.y);
            }
        }

        if (lp == pointIndex)
        {
            // フォントカラーの設定
#ifdef USE_COLOR
            if ((NNshGlobal->NNsiParam)->useColor != 0)
            {
                // ちょっと失敗、引数は反則だ...許してください...(^^;
                NNsi_SetTitleColor(NNshGlobal->msgTitleStatus[lp]);
            }
#endif
#ifdef USE_CLIE
            if (NNshGlobal->hrRef != 0)
            {
                // カーソル位置を反転させる(CLIEハイレゾ描画)
                HRWinDrawInvertedChars(NNshGlobal->hrRef, 
                                       titleIndex[lp], StrLen(titleIndex[lp]),
                                       dimF->topLeft.x, dimF->topLeft.y);
            }
            else
#endif
            {
                // カーソル位置を反転させる(通常モード描画)
                len = FntWordWrap(titleIndex[lp], dimF->extent.x);
                WinDrawInvertedChars(titleIndex[lp], len,
                                     dimF->topLeft.x, dimF->topLeft.y);
            }
        }
 
        // 次の行に描画位置を移動させる
        dimF->topLeft.y = dimF->topLeft.y + height;
    }

#ifdef USE_HIGHDENSITY
    // 高解像度の設定解除
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),
                      NNshGlobal->os5HighDensity);
        WinSetCoordinateSystem(kCoordinatesStandard);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif // #ifdef USE_HIGHDENSITY

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // 現在の色を保存する
        savedIndex = UIColorGetTableEntryIndex(UIObjectForeground);
        WinIndexToRGB(savedIndex, &savedColor);

        // ボタンの色を変換する
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorButton,    &drawColor);
        UIColorSetTableEntry(UIObjectForeground, &drawColor);
    }
#endif

    // 上限/下限のボタンを表示する
    // (上下ボタンを表示/非表示にする)
    NNsh_UpDownButtonControl(FrmGetActiveForm(), (NNshGlobal->NNsiParam)->titleDispState, BTNID_LISTPREV, BTNID_LISTNEXT);

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // ボタンの色を元に戻す
        UIColorSetTableEntry(UIObjectForeground, &savedColor);

        // デフォルトの背景色を設定する
        WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));

        // 文字について、デフォルトの標準色を設定する
        WinSetForeColor(UIColorGetTableEntryIndex(UIObjectForeground));
        WinSetTextColor(UIColorGetTableEntryIndex(UIFieldText));
    }
#endif
    return;
}
