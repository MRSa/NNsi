/*============================================================================*
 *  FILE: 
 *     getloglist.c
 *
 *  Description: 
 *     参照ログ取得設定の一覧画面
 *===========================================================================*/
#define GETLOGLIST_C
#include "local.h"

#ifdef USE_LOGCHARGE

/*------------------------------------------------------------------------*/
/*   Function : selectionMenu_getLogList                                  */
/*                                     取得ログ一覧画面選択メニューの表示 */
/*------------------------------------------------------------------------*/
static UInt16 selectionMenu_getLogList(void)
{
    UInt16  btnId, feature, nofItems;
    UInt16 *jumpList;
    Char   *listLabel, *numP;

    // ジャンプ一覧を設定
    feature  = NNSH_GETLOG_FEATURE_NONE;
    nofItems = 0;
    listLabel = MEMALLOC_PTR(NNSH_ITEMLEN_GETLOGJUMP * NNSH_NOF_JUMPMSG_GETLOG + MARGIN);
    if (listLabel == NULL)
    {
        return (feature);
    }
    MemSet(listLabel, (NNSH_ITEMLEN_GETLOGJUMP * NNSH_NOF_JUMPMSG_GETLOG + MARGIN), 0x00);
    numP = listLabel;

    jumpList = MEMALLOC_PTR(NNSH_NOF_JUMPMSG_GETLOG + MARGIN);
    if (jumpList == NULL)
    {
        MEMFREE_PTR(listLabel);
        return (feature);
    }
    MemSet(jumpList, (NNSH_NOF_JUMPMSG_GETLOG + MARGIN), 0x00);


    // 選択メニュー一覧を追加する

    // リストに「一覧へ戻る」を追加
    StrCopy(numP, NNSH_JUMPMSG_GETLOGRETURN);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOGRETURN);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOGRETURN);
    nofItems++;

    // リストに「開く」を追加
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_OPEN);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_OPEN);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_OPEN);
    nofItems++;

    if (NNshGlobal->browserCreator != 0)
    {
        // リストに「ブラウザで開く」を追加
        StrCopy(numP, NNSH_JUMPMSG_GETLOGBROWSER);
        numP = numP + sizeof(NNSH_JUMPMSG_GETLOGBROWSER);
        jumpList[nofItems] = (NNSH_JUMPSEL_GETLOGBROWSER);
        nofItems++;
    }

    if ((CheckInstalledResource_NNsh('appl', 'asUR') != false)||
         (CheckInstalledResource_NNsh('DAcc', 'mmuA') != false))
    {
        // MemoURL/MemoURLDAがインストールされていた...
        // リストに「MemoURLに送る」を追加
        StrCopy(numP, NNSH_JUMPMSG_MEMOURL);
        numP = numP + sizeof(NNSH_JUMPMSG_MEMOURL);
        jumpList[nofItems] = (NNSH_JUMPSEL_MEMOURL);
        nofItems++;
    }

    // リストに「サイト情報」を追加
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_WWWINFO);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_WWWINFO);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_WWWINFO);
    nofItems++;

    // リストに「新着確認」を追加
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_CHECK);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_CHECK);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_CHECK);
    nofItems++;

    // リストに「メニュー」を追加
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_OPENMNU);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_OPENMNU);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_OPENMNU);
    nofItems++;

    // リストに「Level変更」を追加
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_LEVEL);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_LEVEL);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_LEVEL);
    nofItems++;

    // リストに「回旋切断」を追加
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_DISCONN);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_DISCONN);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_DISCONN);
    nofItems++;

    // リストに「既読記事削除」を追加
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_DELETE_READ);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_DELETE_READ);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_DELETE_READ);
    nofItems++;

    // NNsi終了
    if ((NNshGlobal->NNsiParam)->addMenuNNsiEnd != 0)
    {
        StrCopy(numP, NNSH_JUMPMSG_NNSIEND);
        numP = numP + sizeof(NNSH_JUMPMSG_NNSIEND);
        jumpList[nofItems] = NNSH_JUMPSEL_NNSIEND;
        nofItems++;
    }

#ifdef USE_BT_CONTROL
    // Bluetooth On/Off
    if ((NNshGlobal->NNsiParam)->addBtOnOff != 0)
    {
        // リストに「Bt On/Bt Off」を追加
        StrCopy(numP, NNSH_JUMPMSG_GETLOG_BT_ON);
        numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_BT_ON);
        jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_BT_ON);
        nofItems++;

        StrCopy(numP, NNSH_JUMPMSG_GETLOG_BT_OFF);
        numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_BT_OFF);
        jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_BT_OFF);
        nofItems++;
    }
#endif

    // 選択ウィンドウを表示する
    btnId = NNshWinSelectionWindow(FRMID_JUMPMSG, listLabel, nofItems, 0);
    if (btnId != BTNID_JUMPCANCEL)
    {
        switch (jumpList[NNshGlobal->jumpSelection])
        {
          case NNSH_JUMPSEL_GETLOGRETURN:
            feature = NNSH_GETLOG_FEATURE_CLOSE_GETLOG;
            break;
          case NNSH_JUMPSEL_GETLOG_OPEN:
            feature = NNSH_GETLOG_FEATURE_OPEN_NNSI;
            break;
          case NNSH_JUMPSEL_GETLOGBROWSER:
            feature = NNSH_GETLOG_FEATURE_OPENWEB;
            break;
          case NNSH_JUMPSEL_GETLOG_MEMOURL:
            feature = NNSH_GETLOG_FEATURE_OPEN_MEMOURL;
            break;
          case NNSH_JUMPSEL_GETLOG_CHECK:
            feature = NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL;
            break;
          case NNSH_JUMPSEL_GETLOG_OPENMNU:
            feature = NNSH_GETLOG_FEATURE_OPEN_GETLOGMENU;
            break;
          case NNSH_JUMPSEL_GETLOG_LEVEL:
            feature = NNSH_GETLOG_FEATURE_OPEN_GETLEVEL;
            break;
          case NNSH_JUMPSEL_GETLOG_DISCONN:
            feature = NNSH_GETLOG_FEATURE_FORCE_DISCONNECT;
            break;
          case NNSH_JUMPSEL_GETLOG_BT_ON:
            feature = NNSH_GETLOG_FEATURE_BT_ON;
            break;
          case NNSH_JUMPSEL_GETLOG_BT_OFF:
            feature = NNSH_GETLOG_FEATURE_BT_OFF;
            break;
          case NNSH_JUMPSEL_GETLOG_NNSIEND:
            feature = NNSH_GETLOG_FEATURE_NNSIEND;
            break;
          case NNSH_JUMPSEL_GETLOG_EDIT:
            feature = NNSH_GETLOG_FEATURE_EDITITEM;
            break;
          case NNSH_JUMPSEL_GETLOG_NEW:
            feature = NNSH_GETLOG_FEATURE_ENTRY_NEWSITE;
            break;
          case NNSH_JUMPSEL_GETLOG_SEPA:
            feature = NNSH_GETLOG_FEATURE_EDIT_SEPARATOR;
            break;
          case NNSH_JUMPSEL_MEMOURL:
            feature = NNSH_GETLOG_FEATURE_OPEN_MEMOURL;
            break;
          case NNSH_JUMPSEL_GETLOG_WWWINFO:
            feature = NNSH_GETLOG_FEATURE_SITE_INFORMATION;
            break;
          case NNSH_JUMPSEL_GETLOG_DELETE_READ:
            feature = NNSH_GETLOG_FEATURE_DELETELOG_READ;
          default:
            break;
        }
    }

    MEMFREE_PTR(jumpList);
    MEMFREE_PTR(listLabel);
    return (feature);
}

/*-------------------------------------------------------------------------*/
/*   Function : displayString                                              */
/*                                                          画面表示の更新 */
/*-------------------------------------------------------------------------*/
static void displayString(Char *lineData, RectangleType *dimF, UInt16 command)
{
    IndexedColorType color;
    UInt16           len, isReverse;
    
    isReverse = command & NNSH_GETLOGSTATE_MASK_SELECT;

#ifdef USE_COLOR
    // フォントカラーの設定(未読の色にする)
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        if ((command & NNSH_GETLOGSTATE_MASK_UPDATE) == NNSH_GETLOGSTATE_UPDATE)
        {
            color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorNew;
            WinSetForeColor(color);
            WinSetTextColor(color);
        }
        if ((command & NNSH_GETLOGSTATE_DATETIME) == NNSH_GETLOGSTATE_DATETIME)
        {
            color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorUnknown;
            WinSetForeColor(color);
            WinSetTextColor(color);
        }
    }
#endif

#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        // フォントのワードラップ(描画領域内に内容を抑えるため)
        if (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE)
        {
            len = HRFntWordWrap(NNshGlobal->hrRef, lineData, dimF->extent.x);
        }
        else
        {
            len = FntWordWrap(lineData, dimF->extent.x);
        }
        // CLIEハイレゾ描画(Truncateして表示)
        HRWinDrawTruncChars(NNshGlobal->hrRef, 
                            lineData, len,
                            dimF->topLeft.x, dimF->topLeft.y, dimF->extent.x);
        if (isReverse != 0)
        {
            // カーソル位置を反転させる
            HRWinDrawInvertedChars(NNshGlobal->hrRef, 
                                   lineData, len,
                                   dimF->topLeft.x, dimF->topLeft.y);
        }
    }
    else
#endif
    {
        // 通常モード描画(Truncateして表示)
        len = FntWordWrap(lineData, dimF->extent.x);
        WinDrawTruncChars(lineData, len, dimF->topLeft.x,
                          dimF->topLeft.y, dimF->extent.x);
        if (isReverse != 0)
        {
            // カーソル位置を反転させる
            WinDrawInvertedChars(lineData, len,
                                 dimF->topLeft.x, dimF->topLeft.y);
        }
    }

#ifdef USE_COLOR
    // 既読の色に戻す
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        if (((command & NNSH_GETLOGSTATE_MASK_UPDATE) == NNSH_GETLOGSTATE_UPDATE)||
            ((command & NNSH_GETLOGSTATE_DATETIME) == NNSH_GETLOGSTATE_DATETIME))
        {
            color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorAlready;
            WinSetForeColor(color);
            WinSetTextColor(color);
        }
    }
#endif
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : createList_getLogList                                      */
/*                                                  画面表示用の文字列取得 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
static void createList_getLogList(UInt16 getLevel, NNshDispList *displayList)
{
    Err                  ret;
    DmOpenRef            dbRef;
    UInt16               lp, maxRec;
    NNshGetLogDatabase  *dbData;
    MemHandle            memH;

    // カウントはゼロ
    displayList->dataCount = 0;

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return;
    }

    // レコード件数取得
    maxRec = 0;
    GetDBCount_NNsh(dbRef, &maxRec);

    if (maxRec > MAX_GETLOG_RECORDS)
    {
        // レコードの最大件数を超えていた場合には最大件数まで表示...
        NNsh_DebugMessage(ALTID_ERROR, "OVERFLOW LogParmanent-NNsi", " cnt:", maxRec);
        maxRec = MAX_GETLOG_RECORDS;
    }
    for (lp = 0; lp < maxRec; lp++)
    {
        // レコードは、逆方向から読み出すようにする
        ret = GetRecordReadOnly_NNsh(dbRef, (maxRec - (lp + 1)), &memH, (void **) &dbData);
        if (ret != errNone)
        {
            // ログデータ取得中にエラー発生、抜ける
            NNsh_DebugMessage(ALTID_ERROR, "GetRecordReadOnly_NNsh()", " ret:", ret);
            break;
        }
        if ((getLevel == 0)||((getLevel - 1) == dbData->usable))
        {
            // 表示すべきレコードを検出
            if (displayList->dataCount < MAX_GETLOG_RECORDS)
            {
                displayList->title[displayList->dataCount] =
                         MEMALLOC_PTR(MAX_TITLE_LABEL + MAX_GETLOG_URL + (MARGIN * 2));
                if (displayList->title[displayList->dataCount] == NULL)
                {
                    // 領域の確保に失敗...
                    break;
                }
                // データ(タイトルと更新時刻)を格納する
                MemSet(displayList->title[displayList->dataCount],
                       (MAX_TITLE_LABEL + MAX_GETLOG_URL + (MARGIN * 2)), 0x00);
                StrCopy(displayList->title[displayList->dataCount], dbData->dataTitle);
                StrCat (displayList->title[displayList->dataCount], " (");
                StrNCopy(&displayList->title[displayList->dataCount][StrLen(displayList->title[displayList->dataCount])], dbData->dataURL, MAX_TITLE_LABEL);
                if (StrLen(dbData->dataURL) > MAX_TITLE_LABEL)
                {
                    displayList->title[displayList->dataCount][StrLen(displayList->title[displayList->dataCount])] = chrEllipsis;
                }
                StrCat (displayList->title[displayList->dataCount], ") ");
                displayList->updateTime[displayList->dataCount] = dbData->lastUpdate;
                displayList->recNum[displayList->dataCount] = (maxRec - (lp + 1));
                if (dbData->state != NNSH_SUBJSTATUS_NOT_YET)
                {
                    // 取得していた場合...最上位ビットを立てておく...
                    displayList->recNum[displayList->dataCount] = displayList->recNum[displayList->dataCount] | NNSH_GETLOG_NEWARRIVAL_LOG;
                }
                (displayList->dataCount)++;
            }
        }
        // DBレコードを開放
        ReleaseRecordReadOnly_NNsh(dbRef, memH);     
    }

    // データベースを閉じる
    CloseDatabase_NNsh(dbRef);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : RefreshDisplay_getLogList                                  */
/*                                                          画面表示の更新 */
/*-------------------------------------------------------------------------*/
void RefreshDisplay_getLogList(FormType *frm, UInt16 command, UInt16 parameter)
{
    RectangleType        dimF, dateF;
    Char                 lineData[BUFSIZE];
    Int16                pics;
    UInt16               fontID, nlines, fontHeight, update;
    UInt16               startLine, lineStatus, lp;
    UInt32               currentTime;
    DateTimeType         dateBuf;
    NNshDispList        *displayList;
    
    // ワーク領域を変数に反映させる
    displayList = NNshGlobal->dispList;

    // 現在時刻を取得
    currentTime = TimGetSeconds();

    // 状態の初期化
    lineStatus = NNSH_DISP_ALL;
    startLine  = (NNshGlobal->NNsiParam)->getLogListPage;

    // 画面を再描画する
    switch (command)
    {
      case NNSH_STEP_REDRAW:
        // FrmEraseForm(frm);
        FrmDrawForm(frm);
        break;

      case NNSH_STEP_NEWOPEN:
        FrmEraseForm(frm);
        FrmDrawForm(frm);
        
        // ポップアップトリガのラベルを(設定ログレベルに合わせ)更新する
        if ((NNshGlobal->NNsiParam)->getROLogLevel == 0)
        {
             update = 0;
        }
        else
        {
             update = (NNshGlobal->NNsiParam)->getROLogLevel + 1;
        }
        NNshWinSetPopItems(frm, POPTRID_GETLOGLEVEL, LSTID_GETLOGLEVEL, update);
        break;
    }

    // 画面の大きさと表示可能行数を取得する
    FrmGetObjectBounds(frm,
                   FrmGetObjectIndex(frm, GADID_MESSAGE_LIST), &dimF);
    fontID = ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ?
                           (NNshGlobal->NNsiParam)->currentFont : (NNshGlobal->NNsiParam)->sonyHRFontTitle;
    NNsi_UpdateRectangle(&dimF, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                                                &fontID, &fontHeight, &nlines);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, " ::disp:: nlines:%d, (%d+%d, %d+%d)",
                      nlines, dimF.topLeft.x, dimF.extent.x , dimF.topLeft.y , dimF.extent.y);
#endif // #ifdef USE_REPORTER

    // セレクション部分が画面表示範囲を超えていた場合...
    if (parameter >= nlines)
    {
        parameter = nlines - 1;
    }
    NNshGlobal->work1 = displayList->dataCount;

    // コマンドから表示位置の先頭と表示状態を決定する
    switch (command)
    {
      case NNSH_STEP_TO_TOP:
        // 先頭ページを表示する
        startLine  = 0;
        lineStatus = NNSH_DISP_UPPERLIMIT;
        break;

      case NNSH_STEP_PAGEUP:
        // １ページ上を表示する
        if ((NNshGlobal->NNsiParam)->getLogListPage <= nlines)
        {
            startLine  = 0;
            lineStatus = NNSH_DISP_UPPERLIMIT;
        }
        else
        {
            startLine  = (NNshGlobal->NNsiParam)->getLogListPage - nlines;
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_PAGEDOWN:
        // １ページ下を表示する
        if (((NNshGlobal->NNsiParam)->getLogListPage + nlines) >= displayList->dataCount)
        {
            startLine  = (NNshGlobal->NNsiParam)->getLogListPage;
            lineStatus = NNSH_DISP_LOWERLIMIT;
        }
        else
        {
            startLine  = (NNshGlobal->NNsiParam)->getLogListPage + nlines;
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_TO_BOTTOM:
        // 末尾ページを表示する
        startLine  = ((displayList->dataCount) < nlines) ? 
                                     0 : (displayList->dataCount) - nlines;
        lineStatus = NNSH_DISP_LOWERLIMIT;
        break;

      default:
        // その他（再描画？）
        startLine  = (NNshGlobal->NNsiParam)->getLogListPage;
        if (startLine != 0)
        {
            lineStatus = NNSH_DISP_HALFWAY;
        }
        else
        {
            lineStatus = NNSH_DISP_UPPERLIMIT;
        }
        break;
    }

    // ページ状態を反映
    if (lineStatus != NNSH_DISP_UPPERLIMIT)
    {
        if (startLine + nlines >= displayList->dataCount)
        {
            lineStatus = NNSH_DISP_LOWERLIMIT;
        }
        else
        {
            lineStatus = NNSH_DISP_HALFWAY;
        }
    }
    if (displayList->dataCount <= nlines)
    {
        lineStatus = NNSH_DISP_ALL;
    }

    // 表示領域をクリアする
    NNsi_EraseRectangle(&dimF);

    // 上下ボタンの表示設定
    NNsh_UpDownButtonControl(frm, lineStatus, BTNID_LISTPREV, BTNID_LISTNEXT);

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

    // 文字列の描画（行単位で描画する）
    parameter = parameter + startLine;

    // 選択アイテムが下限を越えているかどうかのチェック
    if (parameter >= displayList->dataCount)
    {
        parameter = displayList->dataCount - 1;
    }
    for (lp = 0; lp < displayList->dataCount; lp++)
    {
        if (lp < startLine)
        {
            // 表示データではない、次のデータを探す
            continue;
        }
        if (lp >= startLine + nlines)
        {
            // 表示可能領域数を超えた場合にはループを抜ける
            break;
        }

        // 選択列かどうかをチェック
        update = 0;
        if (parameter == lp)
        {
            update = update | NNSH_GETLOGSTATE_SELECTED;
        }

        // １行出力
        MemSet(lineData, sizeof(lineData), 0x00);
        if (displayList->updateTime[lp] != 0)
        {
            // 更新時刻と現在時刻が20分以内だった場合...
            if (((currentTime - displayList->updateTime[lp]) < (60 * 20))&&
                ((displayList->recNum[lp] & 0x8000) != 0))
            {
                // 更新マークをつける
                StrCopy(lineData, "* ");
                update = update | NNSH_GETLOGSTATE_UPDATE;
            }
            else
            {
                StrCopy(lineData, "   ");
            }
            StrCat(lineData, displayList->title[lp]);
            displayString(lineData, &dimF, update);

            // 最新チェック時刻を表示する
            MemSet(lineData, sizeof(lineData), 0x00);
            MemSet(&dateBuf, sizeof(dateBuf), 0x00);
            TimSecondsToDateTime(displayList->updateTime[lp], &dateBuf);

            DateToAscii((UInt8)  dateBuf.month,
                        (UInt8)  dateBuf.day,
                        (UInt16) dateBuf.year, 
                        dfYMDWithSlashes,
                        lineData);
            StrCat(lineData, ",");
            TimeToAscii((UInt8) dateBuf.hour,
                        (UInt8) dateBuf.minute,
                        tfColon24h,
                        &lineData[StrLen(lineData)]);
            MemMove(&dateF, &dimF, sizeof(RectangleType));

#ifdef USE_CLIE
            if (NNshGlobal->hrRef != 0)
            {
                // フォントのワードラップ(描画領域内に内容を抑えるため)
                if (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE)
                {
                    pics = HRFntCharsWidth(NNshGlobal->hrRef, lineData, 15);
                }
                else
                {
                    pics = FntCharsWidth(lineData, 15);
                }
            }
            else
#endif
            {
                pics = FntCharsWidth(lineData, 15);
            }

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "()()UPDATE: %s  pics:%d", lineData, pics);
#endif // #ifdef USE_REPORTER
            // ここらあたり、えらい無駄な処理が多いな。。。リファクタリング必要!!
            dateF.topLeft.x = dimF.extent.x - pics - MARGIN;
            dateF.extent.x  = pics + MARGIN;
            dateF.extent.y  = fontHeight;

            // Coordモードを切り替えているため、NNsi_EraseRectangle()が使えない...
#ifdef USE_CLIE 
            if (NNshGlobal->hrRef != 0)
            {
                // CLIEハイレゾモード...
                HRWinEraseRectangle(NNshGlobal->hrRef, &dateF, 0);
            }
            else
#endif
            {
                // 通常描画モード...
                WinEraseRectangle(&dateF, 0);
            }
            dateF.topLeft.x = dimF.extent.x - pics;
            dateF.extent.x  = pics;
            update = ((update & NNSH_GETLOGSTATE_SELECTED) | NNSH_GETLOGSTATE_DATETIME);
            displayString(lineData, &dateF, update);
        }
        else
        {
            // 更新時刻なし、タイトルだけ表示する
            StrCopy(lineData, "   ");
            StrCat(lineData, displayList->title[lp]);

            displayString(lineData, &dimF, update);
        }

        // 次の行に描画位置を移動させる
        dimF.topLeft.y = dimF.topLeft.y + fontHeight;

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

    // 先頭行をwork1に、表示可能行数をwork2に記憶する
    (NNshGlobal->NNsiParam)->getLogListPage = startLine;
    NNshGlobal->work2 = nlines;

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openNNsiThread_getLogList                                  */
/*                                                              NNsiで開く */
/*-------------------------------------------------------------------------*/
static UInt16 openNNsiThread_getLogList(UInt16 recNum)
{
    Err                  ret;
    UInt16               index;
    Char                 fileName[MAX_THREADFILENAME + MARGIN];
    DmOpenRef            dbRef;
    NNshGetLogDatabase  *dbData;
#ifdef CHECK_GETLOGTHREAD_INFORMATION_STRICTLY
    NNshSubjectDatabase *subjData;
    MemHandle            memH;
#endif

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "OPEN THREAD(recNum:%d)", recNum);
#endif // #ifdef USE_REPORTER


    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        return (NNSH_DATABASE_BLANK);
    }
    MemSet(dbData, (sizeof(NNshGetLogDatabase) + MARGIN), 0x00);

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        index = NNSH_DATABASE_BLANK;
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "OPEN:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

    ret = GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), dbData);
    if (ret != errNone)
    {
        // エラー発生,　終了する。
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        CloseDatabase_NNsh(dbRef);
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "GET:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

    // データ取得成功、更新したデータを落とす
    dbData->state = NNSH_SUBJSTATUS_NOT_YET;
    UpdateRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), dbData);

    CloseDatabase_NNsh(dbRef);

    // 1階層下も取得する設定だった場合。。。
    if (dbData->getDepth != 0)
    {
        // 開く記事の一覧からどの記事を開くか選択する
        MemSet(fileName, sizeof(fileName), 0x00);
        index = selectOpenThread_getLogList(dbData, recNum, fileName);
        switch (index)
        {
          case NNSH_DATABASE_BLANK:
          case NNSH_DATABASE_UNKNOWN:
            // 設定せずに抜ける...
            break;

          default:
            // OPENする記事のレコード番号を設定する
            (NNshGlobal->NNsiParam)->openMsgIndex = index;
            break;
        }
        // 関数を抜ける...
        goto FUNC_END;
    }

    // スレデータベースの検索条件設定
    // ファイル名を指定する(デフォルトの名称...)
    MemSet(fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, FILE_LOGCHARGE_PREFIX);
    NUMCATI(fileName, recNum);
    StrCat (fileName, "-0-0");
    StrCat (fileName, FILE_LOGCHARGE_SUFFIX);    

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "FILE:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

    // スレデータベースのオープン
    dbRef = 0;
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        index = NNSH_DATABASE_BLANK;
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "FILE:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

    // 取得済みログが存在するかどうかのチェック
    // (スレデータのＤＢは、ファイル名でのソートかけてないので動かない、、、)
    if (IsExistRecordRR_NNsh(dbRef, fileName, NNSH_KEYTYPE_CHAR, 0, &index) != errNone)
    {
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "NOT FOUND:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

        // レコード未発見、終了する
        index = NNSH_DATABASE_BLANK;
        CloseDatabase_NNsh(dbRef);
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "FIND:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

#ifdef CHECK_GETLOGTHREAD_INFORMATION_STRICTLY
    // レコード情報を取得する
    ret = GetRecordReadOnly_NNsh(dbRef, index, &memH, (void**) &subjData);
    if (ret != errNone)
    {
        index = NNSH_DATABASE_BLANK;
        CloseDatabase_NNsh(dbRef);
        goto FUNC_END;
    }

    // スレ状態の確認
    switch (subjData->state)
    {
      case NNSH_SUBJSTATUS_NOT_YET:
      case NNSH_SUBJSTATUS_DELETE:
        // 未取得の場合には、オープンしない
        index = NNSH_DATABASE_BLANK;
        break;

      default:
        // 取得済みログの場合には、オープンする設定を行う
        (NNshGlobal->NNsiParam)->openMsgIndex = index;
        (NNshGlobal->msgIndex)->openMsg = 0;
        break;
    }
    // 確保したレコードを解除する
    ReleaseRecordReadOnly_NNsh(dbRef, memH);
#else
    // 考えてみれば、参照ログは取得済みしかSubj-DBに格納しない...
    (NNshGlobal->NNsiParam)->openMsgIndex = index;
#endif
    CloseDatabase_NNsh(dbRef);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "GOTO VIEW FORM fileName:%s", fileName);
#endif // #ifdef USE_REPORTER

FUNC_END:
    MEMFREE_PTR(dbData);
    return (index);
}

/*-------------------------------------------------------------------------*/
/*   Function : GetLogChargeDB                                             */
/*                                              ログデータベースデータの取得 */
/*-------------------------------------------------------------------------*/
static Err GetLogChargeDB(UInt16 recNum, NNshGetLogDatabase *dbData)
{
    Err                 ret;
    DmOpenRef           dbRef;

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return (~errNone);
    }
    MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);
    ret = GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), dbData);
    CloseDatabase_NNsh(dbRef);
    if (ret != errNone)
    {
        // エラー発生,　終了する。
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        /** not return; **/
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :  showSiteInfo_getLogList                                   */
/*                                                         サイト情報の表示 */
/*-------------------------------------------------------------------------*/
static void showSiteInfo_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    Char                logBuf[BIGBUF];

    // 参照ログチャージ用DBの取得
    ret = GetLogChargeDB(recNum, &dbData);
    if (ret != errNone)
    {
        // エラー発生,　終了する。
        NNsh_DebugMessage(ALTID_ERROR, "GetLogChargeDB() ", "ret:", ret);
        return;
    }
    MemSet(logBuf, sizeof(logBuf), 0x00);
    if (dbData.wwwcMode == 1)
    {
        StrCat(logBuf, "[WWWC]");
    }
    if (dbData.wwwcMode == 2)
    {
        StrCat(logBuf, "[MDL]");
    }
    if (dbData.rssData != 0)
    {
        StrCat(logBuf, "[RSS]");
    }
    if (dbData.getDepth != 0)
    {
        StrCat(logBuf, "[link]");
    }
    if (dbData.getDateTime[0] != '\0')
    {
        StrCat (logBuf, "\n[");
        StrCat (logBuf, dbData.getDateTime);
        StrCat (logBuf, "]\n---\n");
    }
    StrCat (logBuf, dbData.dataTitle);
    StrCat (logBuf, "\n");
    StrCat (logBuf, dbData.dataURL);
    StrCat (logBuf, "\n\n");
    StrCat (logBuf, dbData.dataFileName);
    StrCat (logBuf, "\n");

    //画面表示    
    NNsh_ErrorMessage(ALTID_INFO, logBuf, "", 0);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : beamSiteURL_getLogList                                     */
/*                                                       URL情報をBEAMする */
/*-------------------------------------------------------------------------*/
static void beamSiteURL_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    Char                logBuf[BIGBUF];

    // 参照ログチャージ用DBの取得
    ret = GetLogChargeDB(recNum, &dbData);
    if (ret != errNone)
    {
        // エラー発生,　終了する。
        NNsh_DebugMessage(ALTID_ERROR, "GetLogChargeDB() ", "ret:", ret);
        return;
    }

    // BEAM送信データの生成...
    MemSet(logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, NNSI_NNSIEXT_URL_START);
    StrCat (logBuf, dbData.dataURL);
    StrCat (logBuf, "\"");
    StrCat (logBuf, dbData.dataTitle);
    StrCat (logBuf, "\"");
    StrCat (logBuf, NNSI_NNSIEXT_URL_END);

    // データの送信
    SendToExternalDevice(dbData.dataTitle, logBuf, SOFT_CREATOR_ID);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openWebBrowser_getLogList                                  */
/*                                                       Webブラウザで開く */
/*-------------------------------------------------------------------------*/
static void openWebBrowser_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    DmOpenRef           dbRef;

    // 参照ログチャージ用DBの取得

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return;
    }
    MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
    ret = GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), &dbData);
    if (ret != errNone)
    {
        // エラー発生,　終了する。
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        CloseDatabase_NNsh(dbRef);
        return;
    }

    // データ取得成功、更新したデータを落とす
    dbData.state = NNSH_SUBJSTATUS_NOT_YET;
    UpdateRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), &dbData);
    CloseDatabase_NNsh(dbRef);

    // NetFrontを起動する(NNsi終了後に起動)
    if (NNshGlobal->browserCreator != 0)
    {
        (void) WebBrowserCommand(NNshGlobal->browserCreator, 0, 0, 
                                 NNshGlobal->browserLaunchCode,
                                 dbData.dataURL, MAX_GETLOG_URL, NULL);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openURLwebDA_getLogList                                    */
/*                                                             webDAで開く */
/*-------------------------------------------------------------------------*/
static void openURLwebDA_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    DmOpenRef           dbRef;

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return;
    }
    MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
    ret = GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), &dbData);
    if (ret != errNone)
    {
        // エラー発生,　終了する。
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        CloseDatabase_NNsh(dbRef);
        return;
    }

    // データ取得成功、更新したデータを落とす
    dbData.state = NNSH_SUBJSTATUS_NOT_YET;
    UpdateRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), &dbData);
    CloseDatabase_NNsh(dbRef);

    // クリップボードに選択しているURLをコピーする
    ClipboardAddItem(clipboardText, dbData.dataURL, StrLen(dbData.dataURL));

    // webDAを起動する
    if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
    {
        // webDAがインストールされていた場合...
        (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openMemoURL_getLogList                                     */
/*                                                MemoURL(MemoURLDA)で開く */
/*-------------------------------------------------------------------------*/
static void openMemoURL_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    Char                logMsg[MAX_GETLOG_URL + MAX_TITLE_LABEL + (MARGIN * 2)];
    UInt16              len;

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "OPEN MEMO URL(recNum:%d)", recNum);
#endif // #ifdef USE_REPORTER

    // 参照ログチャージ用DBの取得
    ret = GetLogChargeDB(recNum, &dbData);
    if (ret != errNone)
    {
        // エラー発生,　終了する。
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        return;
    }

    if (CheckInstalledResource_NNsh('DAcc', 'mmuA') != false)
    {
        // MemoURLDAがインストールされていた場合...
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, dbData.dataTitle);
        len = StrLen(logMsg);
        StrCopy(&logMsg[len + 1], dbData.dataURL);
        len  = len + 1 + StrLen(dbData.dataURL);

        // クリップボードに選択したURLをコピーする
        ClipboardAddItem(clipboardText, logMsg, len);

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " COPIED URL :%s(len:%d)", logMsg, len);
#endif // #ifdef USE_REPORTER

        NNsh_DebugMessage(ALTID_INFO, "SEND to MemoURLDA URL:", dbData.dataURL, 0);

        (void) LaunchResource_NNsh('DAcc','mmuA','code',1000);
        return;
    }
    NNsh_DebugMessage(ALTID_INFO, "SEND to MemoURL URL:", dbData.dataURL, 0);

    // URLをMemoURLへ送る
    if (CheckInstalledResource_NNsh('appl', 'asUR') == false)
    {
        // MemoURLがインストールされていなかった、何もせず抜ける
        return;
    }

    MemSet(logMsg, sizeof(logMsg), 0x00);
    logMsg[0] = '1';
    StrCopy(&logMsg[2], dbData.dataTitle); // スレタイをタイトルに...
    len = StrLen(&logMsg[2]) + 2;
    StrCopy(&logMsg[len + 1], dbData.dataURL);
    len = len + 1 + StrLen(dbData.dataURL);

    // 指定されたURLをMemoURLに送る
    (void) WebBrowserCommand('asUR', 0, 0, 0x8001, logMsg, len, NULL);
    return;
}


/*-------------------------------------------------------------------------*/
/*   Function : checkGadget_getLogList                                     */
/*                                                タップした領域のチェック */
/*-------------------------------------------------------------------------*/
static UInt16 checkGadget_getLogList(EventType *event, FormType *frm, RectangleType *rect)
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

    if ((selItem + (NNshGlobal->NNsiParam)->getLogListPage) >= NNshGlobal->work1)
    {
        // 範囲外の選択だった場合、何もしない
        return (0);
    }

    if (selItem == (NNshGlobal->NNsiParam)->getLogListSelection)
    {
        // ダブルタップを検出、Webブラウザで開こうとする
        return (vchrJogRelease);
    }

    // 選択アイテムを移動させる
    (NNshGlobal->NNsiParam)->getLogListSelection = selItem;
    return (chrCapital_R);
}

/*=========================================================================*/
/*   Function :   Handler_GetLogList                                       */
/*                                参照ログの取得一覧画面のイベントハンドラ */
/*=========================================================================*/
Boolean Handler_GetLogList(EventType *event)
{
    FormType     *frm;
    RectangleType dimF;
    UInt16        itemId, feature;

    // ボタンチェック以外のイベントは即戻り
    itemId = 0;
    switch (event->eType)
    {
      case popSelectEvent:
        // ポップアップトリガー変更
        itemId = event->data.popSelect.listID;
        break;

      case menuEvent:
        // メニュー選択
        itemId = event->data.menu.itemID;
        break;

      case ctlSelectEvent:
        // ボタンが押された(次へ進む)
        itemId = event->data.ctlSelect.controlID;
        break;

      case ctlRepeatEvent:
        // リピートボタンが押された(次へ進む)
        itemId = event->data.ctlRepeat.controlID;
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent:
        // 画面の回転
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
        }
        return (true);
        break;
#endif  // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        // 画面のサイズ変更
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
            }
        }
        return (true);
        break;
#endif

      case keyDownEvent:
        // キー入力があった
        itemId = KeyConvertFiveWayToJogChara(event);
        break;

      case penDownEvent:
        // 画面タップを検知
        frm = FrmGetActiveForm();
        FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_TAPPED),&dimF);
        if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
        {
            // Gadget内をタップした(== スレを選択した)
            itemId = checkGadget_getLogList(event, frm, &dimF);
            break;
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
                itemId = BTNID_LISTNEXT;
                break;
            }

            // 前ページボタンが押されたかチェック
            FrmGetObjectBounds(frm, 
                               FrmGetObjectIndex(frm, BTNID_LISTPREV),&dimF);
            if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
            {
                // 前ページボタンが押されたことにする
                itemId = BTNID_LISTPREV;
                break;
            }

            // スレを選択したことにする
            FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_LIST),&dimF);
            itemId = checkGadget_getLogList(event, frm, &dimF);
            break;
        }
        return (false); 
        break;

      default: 
        return (false);
        break;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "::GetLog:: itemId:%d[work1:%d work2:%d work3:%d]",
                      itemId, (NNshGlobal->NNsiParam)->getLogListPage, NNshGlobal->work2, (NNshGlobal->NNsiParam)->getLogListSelection);
#endif // #ifdef USE_REPORTER

    // コマンドを実行する
    switch (itemId)
    {
      case chrCapital_U:
      case chrSmall_U:
      case MNUID_SITE_INFO:
        // サイト情報の表示
        feature = NNSH_GETLOG_FEATURE_SITE_INFORMATION;
        break;

      case chrCapital_C:
      case chrSmall_C:
      case MNUID_CONFIG_GETLOG:
        // 参照ログ一覧設定画面を開く
        feature = NNSH_GETLOG_FEATURE_CONFIG_GETLOG;
        break;

      case BTNID_BUTTON1:
      case chrDigitOne:
        // NNsiで開く
        feature = NNSH_GETLOG_FEATURE_OPEN_NNSI;
        break;

      case chrCapital_Z:  // Zキーの入力
      case chrSmall_Z:
      case MNUID_BACK:
      case SELTRID_BACK:
        // 一覧画面を開く
        feature = NNSH_GETLOG_FEATURE_CLOSE_GETLOG;
        break;

      case chrDigitFour:
      case BTNID_BUTTON4:
      case MNUID_ENTRY:
        // サイトの登録
        feature = NNSH_GETLOG_FEATURE_ENTRY_NEWSITE;
        break;
         
      case chrDigitFive:
      case BTNID_BUTTON5:
      case MNUID_SETSEPA:
        // 区切りの設定画面を開く
        feature = NNSH_GETLOG_FEATURE_EDIT_SEPARATOR;
        break;

      case chrDigitSix:
      case BTNID_BUTTON6:
      case chrSpace:
      case chrSJISIdeographicSpace:
      case MNUID_GETLOGOPE:
        // 新着確認実施
        feature = NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL;
        break;

      case MNUID_SITE_DETAIL:
      case chrDigitThree:
      case chrCapital_E:  // Eキーの入力
      case chrSmall_E:
      case BTNID_BUTTON3:
        // データ編集
        feature = NNSH_GETLOG_FEATURE_EDITITEM;
        break;

      case vchrJogBack:
      case chrBackspace:
      case chrEscape:
      case vchrThumbWheelBack:
      case vchrGarminEscape:
        // Jog Back
        feature = (NNshGlobal->NNsiParam)->getLogFeature.jogBack;
        break;

      case vchrJogRelease:
      case chrCarriageReturn:
      case chrLineFeed:
      case vchrRockerCenter:
      case vchrThumbWheelPush:
        // Jog Push
        feature = (NNshGlobal->NNsiParam)->getLogFeature.jogPush;
        break;

      case chrDigitTwo:
      case chrCapital_W:  // Wキーの入力
      case chrSmall_W:
      case BTNID_BUTTON2:
        // Webブラウザで開く
        feature = NNSH_GETLOG_FEATURE_OPENWEB;
        break;

      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrPrevField:
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case chrUpArrow:
        // up
        feature = (NNshGlobal->NNsiParam)->getLogFeature.up;
        break;

      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrNextField:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case chrDownArrow:
        // down
        feature = (NNshGlobal->NNsiParam)->getLogFeature.down;
        break;

      case chrLeftArrow:         // カーソルキー左(5way左)
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
        feature = (NNshGlobal->NNsiParam)->getLogFeature.left;
        break;

      case chrCapital_P:  // Pキーの入力
      case chrSmall_P:
      case BTNID_LISTPREV:
        // 前ページ
        feature = NNSH_GETLOG_FEATURE_PREVPAGE;
        break;

      case chrRightArrow:         // カーソルキー右(5way右)
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
        feature = (NNshGlobal->NNsiParam)->getLogFeature.right;
        break;

      case chrCapital_N:  // Nキーの入力
      case chrSmall_N:
      case BTNID_LISTNEXT:
        // 次ページ
        feature = NNSH_GETLOG_FEATURE_NEXTPAGE;
        break;

      case chrCapital_T:       // Tキーの入力
      case chrSmall_T:
        // 先頭から表示
        feature = NNSH_GETLOG_FEATURE_TOP;
        break;

      case chrCapital_B:  // Bキーの入力
      case chrSmall_B:
        // 末尾から表示
        feature = NNSH_GETLOG_FEATURE_BOTTOM;
        break;

      // Qキー入力
      case chrCapital_Q:
      case chrSmall_Q:
      case MNUID_NET_DISCONN:
        // 回線切断
        feature = NNSH_GETLOG_FEATURE_DISCONNECT;
        break;

      case chrDigitSeven:
        // キー7 (Bt ON)
        feature = NNSH_GETLOG_FEATURE_BT_ON;
        break;

      case chrDigitEight:
        // キー8 (Bt Off)
        feature = NNSH_GETLOG_FEATURE_BT_OFF;
        break;

      case LSTID_GETLOGLEVEL:
        // ポップアップトリガー更新
        feature = NNSH_GETLOG_FEATURE_CREATELIST;
        break;

      // Lキー入力
      case chrCapital_L:
      case chrSmall_L:
        // 巡回レベルを選択
        feature = NNSH_GETLOG_FEATURE_OPEN_GETLEVEL;
        break;

      // ?キー入力
      case chrQuestionMark:
      case MNUID_NNSI_END:
        // NNsiを終了させる
        feature = NNSH_GETLOG_FEATURE_NNSIEND;
        break;

      // Vキー入力
      case chrCapital_V:
      case chrSmall_V:
      case MNUID_SHOW_VERSION:
        // バージョン情報表示
        feature = NNSH_GETLOG_FEATURE_SHOWVERSION;
        break;

      // Rキー入力
      case chrCapital_R:
      case chrSmall_R:
        // 画面の再描画
        feature = NNSH_GETLOG_FEATURE_REDRAW;
        break;

      // Aキー入力
      case chrCapital_A:
      case chrSmall_A:
      case MNUID_NET_CONFIG:
        // ネットワーク設定を開く
        feature = NNSH_GETLOG_FEATURE_NETCONFIG;
        break;

      // Iキー入力
      case chrCapital_I:
      case chrSmall_I:
      case MNUID_SHOW_DEVICEINFO:
        // デバイスの情報表示
        feature = NNSH_GETLOG_FEATURE_DEVICEINFO;
        break;

      // !キー入力
      case chrExclamationMark:
      case MNUID_GOTO_DUMMY:
        // ダミー画面へ遷移
        feature = NNSH_GETLOG_FEATURE_OPEN_DUMMY;
        break;

      case MNUID_SITELOG_DELETE:
      case chrCapital_D:
      case chrSmall_D:
        // ログ(一括)削除
        feature = NNSH_GETLOG_FEATURE_DELETELOG;
        break;

      case MNUID_SITELOG_DELETE_READ:
        // 既読ログ(一括)削除
        feature = NNSH_GETLOG_FEATURE_DELETELOG_READ;
        break;

      case chrDigitNine:
      case MNUID_CONFIG_NNSH:
        // NNsi設定を開く
        feature = NNSH_GETLOG_FEATURE_OPEN_NNSISET;
        break;

      case chrCapital_H:
      case chrSmall_H:
      case MNUID_SHOW_HELP:
        feature = NNSH_GETLOG_FEATURE_HELP;
        break;

      case chrCapital_X:
      case chrSmall_X:
        // 選択メニュー
        feature = NNSH_GETLOG_FEATURE_OPENMENU;
        break;

#if 0
      case vchrCapture:
      case vchrVoiceRec:
      case vchrVZ90functionMenu:      
      case vchrGarminRecord:
#endif
      case vchrHard5:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // CLIEキャプチャーボタンを押したときの設定
        feature = (NNshGlobal->NNsiParam)->getLogFeature.clieCapture;
        break;

      case vchrHard1:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // ハードキー１を押したときの設定
        feature = (NNshGlobal->NNsiParam)->getLogFeature.key1;
        break;

      case vchrHard2:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // ハードキー２を押したときの設定
        feature = (NNshGlobal->NNsiParam)->getLogFeature.key2;
        break;

      case vchrHard3:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // ハードキー３を押したときの設定
        feature = (NNshGlobal->NNsiParam)->getLogFeature.key3;
        break;

      case vchrHard4:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // ハードキー４を押したときの設定
        feature = (NNshGlobal->NNsiParam)->getLogFeature.key4;
        break;

      case chrCapital_S:
      case chrSmall_S:
      case MNUID_BEAM_URL:
        // ＵＲＬ送信
        feature = NNSH_GETLOG_FEATURE_BEAM_URL;
        break;

      case chrCapital_Y:
      case chrSmall_Y:
      case MNUID_OPEN_WEBDA:
        // webDAで開く
        feature = NNSH_GETLOG_FEATURE_OPENWEBDA;
        break;

#ifdef USE_PIN_DIA
      case vchrTT3Short:
      case vchrTT3Long:
        // 画面のサイズ変更
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            if (HandEraResizeForm(FrmGetActiveForm()) == true)
            {
                RefreshDisplay_getLogList(FrmGetActiveForm(), NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
            }
        }
        return (true);
        break;
#endif

      default:
        // 上記以外
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "<>UNKNOWN(%d)<>", itemId);
#endif // #ifdef USE_REPORTER
        return (false);
        break;
    }

EXEC_FEATURE:
    Execute_GetLogList(feature);
    switch (feature)
    {
      case NNSH_GETLOG_FEATURE_CONFIG_GETLOG:
      case NNSH_GETLOG_FEATURE_EDIT_SEPARATOR:
        // 画面表示情報を再描画する
        // (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        Execute_GetLogList(NNSH_GETLOG_FEATURE_CREATELIST);
        break;

      case NNSH_GETLOG_FEATURE_EDITITEM:
      case NNSH_GETLOG_FEATURE_ENTRY_NEWSITE:
        // 画面表示情報をクリアする
        (NNshGlobal->NNsiParam)->getLogListPage = 0;
        NNshGlobal->work2 = 0;
        (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        Execute_GetLogList(NNSH_GETLOG_FEATURE_CREATELIST);
        break;

      case NNSH_GETLOG_FEATURE_CREATELIST:
      case NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL:
        // 
        return (false);
        break;

      case NNSH_GETLOG_FEATURE_OPENMENU:
        // 選択メニューを開いた場合には、再度機能を実行する。
        if (NNshGlobal->jumpSelection != NNSH_GETLOG_FEATURE_NONE)
        {
            // 
            feature = NNshGlobal->jumpSelection;
            goto EXEC_FEATURE;
        }
        break;

      default:
        // 何もしない
        break;
    }
    return (true);
}

/*=========================================================================*/
/*   Function :  Execute_GetLogList                                        */
/*                            参照ログ画面で実行可能な機能の一覧(機能分岐) */
/*=========================================================================*/
Boolean Execute_GetLogList(UInt16 feature)
{
    FormType     *frm;
    EventType    *dummyEvent;
    UInt16        selItem, savedParam, savedParam2;
    ListType     *lstP;

    frm = FrmGetActiveForm();
    switch (feature)
    {
      case NNSH_GETLOG_FEATURE_CLOSE_GETLOG:
        // 一覧画面を開く
        CloseForm_GetLogList(FRMID_THREAD);
        return (false);
        break;

      case NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL:
        // 新着確認実施
        selItem = LstGetSelection(FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, LSTID_GETLOGLEVEL)));
        if (selItem > 1)
        {
            // リスト番号を巡回レベルに調整する
            selItem--;
        }
        savedParam = (NNshGlobal->NNsiParam)->getROLogLevel;

        // 巡回レベルをタブに合わせて変更する
        (NNshGlobal->NNsiParam)->getROLogLevel = selItem;

		// 新着確認時には、ダイアログを表示しないようにする
        savedParam2 = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

        // 新着確認前にＤＡを実施する設定だった場合...
        if ((NNshGlobal->NNsiParam)->preOnDAnewArrival != 0)
        {
            // DAを起動する
            LaunchDA_NNsh(((NNshGlobal->NNsiParam)->prepareDAforNewArrival));
        }

        // 参照ログの取得実行
        GetReadOnlyLogData_NNsh();

        // 巡回レベルを元に戻す
        (NNshGlobal->NNsiParam)->getROLogLevel = savedParam;

        // データを取り直してみる
        lstP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_GETLOGLEVEL));
        selItem = LstGetSelection(lstP);
        Show_BusyForm(MSG_FINDING_WAIT);
        ClearList_getLogList (NNshGlobal->dispList);
        createList_getLogList(selItem, NNshGlobal->dispList);
        Hide_BusyForm(false);

        // PalmOS v3.5以下なら、前のフォームを消去する。
        if (NNshGlobal->palmOSVersion < 0x03503000)
        {
            FrmEraseForm(FrmGetActiveForm());
            FrmDrawForm(FrmGetActiveForm());
        }

        // 画面の再描画を実施する
        (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        RefreshDisplay_getLogList(frm, NNSH_STEP_TO_TOP, (NNshGlobal->NNsiParam)->getLogListSelection);

        // NNsi設定-9のパラメータを流用して、新着確認後の処理を実施する...

#ifdef USE_MACRO
        if ((NNshGlobal->NNsiParam)->startMacroArrivalEnd != 0)
        {
            // 新着確認終了時にマクロを実行する
            NNsi_ExecuteMacroMain(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);
        }
#endif

        if ((NNshGlobal->NNsiParam)->use_DAplugin != 0)
        {
            // 新着確認終了時にメールチェックの確認を実施(cMDA起動)
            (void) LaunchResource_NNsh('DAcc','cMDA','code',1000);
        }

        if ((NNshGlobal->NNsiParam)->disconnArrivalEnd != 0)
        {
            // 「新着確認」終了時に回線切断
            NNshNet_LineHangup();
        }

        // 「新着確認」後にビープ音を鳴らしてみる
        if ((NNshGlobal->NNsiParam)->autoBeep != 0)
        {
            NNsh_BeepSound(sndAlarm);
        }

		// ダイアログ表示設定を元に戻す
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam2;
        break;

      case NNSH_GETLOG_FEATURE_EDITITEM:
        // サイトデータの編集
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " EDIT RECORD:%d",
                          ((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
#endif // #ifdef USE_REPORTER
        SetLogGetURL(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_PREVITEM:
        // ひとつ上選択
        if ((NNshGlobal->NNsiParam)->getLogListSelection > 0)
        {
            (NNshGlobal->NNsiParam)->getLogListSelection--;
            RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
        }
        else
        {
            if ((NNshGlobal->NNsiParam)->getLogListPage == 0)
            {
                // 前ページがない場合には終了
                return (true);
            }
            // 前ページ
            (NNshGlobal->NNsiParam)->getLogListSelection = (NNshGlobal->work2 - 1);
            RefreshDisplay_getLogList(frm, NNSH_STEP_PAGEUP, (NNshGlobal->NNsiParam)->getLogListSelection);
        }
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "<>Jog UP<>");
#endif // #ifdef USE_REPORTER
        break;

      case NNSH_GETLOG_FEATURE_NEXTITEM:
        // ひとつ下選択
        if (((NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage) >= (NNshGlobal->work1 - 1))
        {
            // 越えていた、カーソルを末尾にあわせる
            (NNshGlobal->NNsiParam)->getLogListSelection = NNshGlobal->work1 - (NNshGlobal->NNsiParam)->getLogListPage - 1;
            RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);

            NNsh_DebugMessage(ALTID_INFO, "<>ADJUST<>", " ", (NNshGlobal->NNsiParam)->getLogListSelection);
            NNsh_DebugMessage(ALTID_INFO, "<>ADJUST<>", " work1: ", NNshGlobal->work1);
            return (true);
        }
        if ((NNshGlobal->NNsiParam)->getLogListSelection < (NNshGlobal->work2 - 1))
        {
            (NNshGlobal->NNsiParam)->getLogListSelection++;
            RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
            NNsh_DebugMessage(ALTID_INFO, "<>REDRAW<>", " ", (NNshGlobal->NNsiParam)->getLogListSelection);
            NNsh_DebugMessage(ALTID_INFO, "<>REDRAW<>", " work2:", NNshGlobal->work2);
        }
        else
        {
            if (((NNshGlobal->NNsiParam)->getLogListSelection == (NNshGlobal->work2 - 1))&&
                ((NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage == NNshGlobal->work1))
            {
                // 次ページがない場合には終了
                return (true);
            }
            // 次ページ表示
            (NNshGlobal->NNsiParam)->getLogListSelection = 0;
            RefreshDisplay_getLogList(frm, NNSH_STEP_PAGEDOWN, (NNshGlobal->NNsiParam)->getLogListSelection);
        }
        NNsh_DebugMessage(ALTID_INFO, "<>Jog DOWN<>", " ", (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_PREVPAGE:
        // 前ページ
        RefreshDisplay_getLogList(frm, NNSH_STEP_PAGEUP, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_NEXTPAGE:
        // 後ページ
        RefreshDisplay_getLogList(frm, NNSH_STEP_PAGEDOWN, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_TOP:
        // 一覧の先頭へ
        (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        RefreshDisplay_getLogList(frm, NNSH_STEP_TO_TOP, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_BOTTOM:
        // 一覧の末尾へ
        (NNshGlobal->NNsiParam)->getLogListSelection = (NNshGlobal->work2 - 1);
        RefreshDisplay_getLogList(frm, NNSH_STEP_TO_BOTTOM, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_CREATELIST:
        // 一覧を再構築する
        lstP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_GETLOGLEVEL));
        selItem = LstGetSelection(lstP);
        Show_BusyForm(MSG_FINDING_WAIT);

        // 巡回レベルのパラメータ保存
        if (selItem == 0)
        {
            (NNshGlobal->NNsiParam)->getROLogLevel = 0;
        }
        else
        {
            (NNshGlobal->NNsiParam)->getROLogLevel = selItem - 1;
        }
        ClearList_getLogList (NNshGlobal->dispList);
        createList_getLogList(selItem, NNshGlobal->dispList);
        Hide_BusyForm(false);
        if (NNshGlobal->palmOSVersion < 0x03503000)
        {
            FrmEraseForm(FrmGetActiveForm());
            FrmDrawForm(FrmGetActiveForm());
        }
        (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        RefreshDisplay_getLogList(frm, NNSH_STEP_TO_TOP, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_EDIT_SEPARATOR:
        // 区切り設定画面を開く
        SetLogCharge_LogToken(0);
        break;

      case NNSH_GETLOG_FEATURE_ENTRY_NEWSITE:
        // 新規登録画面を開く
        SetLogGetURL(NNSH_ITEM_LASTITEM);
        break;

      case NNSH_GETLOG_FEATURE_OPEN_GETLEVEL:
        // 新着確認レベルを開く
        CtlHitControl(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, POPTRID_GETLOGLEVEL)));
        break;

      case NNSH_GETLOG_FEATURE_REDRAW:
        // 画面の再描画
        RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "<>REDRAW<>REDRAW<>");
#endif // #ifdef USE_REPORTER
        break;

      case NNSH_GETLOG_FEATURE_OPENWEB:
        // Webブラウザで開く
        openWebBrowser_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_BT_ON:
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(true);
#endif  // #ifdef USE_BT_CONTROL

      case NNSH_GETLOG_FEATURE_BT_OFF:
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(false);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case NNSH_GETLOG_FEATURE_DISCONNECT:
        // 回線切断
        NNshNet_LineHangup();
        break;

      case NNSH_GETLOG_FEATURE_NETCONFIG:
        // ネットワーク設定画面を開く
        OpenNetworkPreferences();
        break;

      case NNSH_GETLOG_FEATURE_DEVICEINFO:
        // デバイス情報を表示する
        ShowDeviceInfo_NNsh();
        break;

      case NNSH_GETLOG_FEATURE_OPEN_DUMMY:
        // ダミー画面を開く
        CloseForm_GetLogList(FRMID_NNSI_DUMMY);
        return (false);
        break;

      case NNSH_GETLOG_FEATURE_OPEN_NNSISET:
        // NNsi設定画面を開く
        CloseForm_GetLogList(FRMID_NNSI_SETTING_ABSTRACT);
        return (false);
        break;

      case NNSH_GETLOG_FEATURE_SHOWVERSION:
        // バージョン情報の表示
        ShowVersion_NNsh();
        break;

      case NNSH_GETLOG_FEATURE_HELP:
        // ヘルプ表示
        // (クリップボードに、nnDAに渡すデータについての指示を格納する)
        ClipboardAddItem(clipboardText, 
                         nnDA_NNSIEXT_VIEWSTART
                         nnDA_NNSIEXT_INFONAME
                         nnDA_NNSIEXT_HELPGETLOG
                         nnDA_NNSIEXT_ENDINFONAME
                         nnDA_NNSIEXT_ENDVIEW,
                         sizeof(nnDA_NNSIEXT_VIEWSTART
                                 nnDA_NNSIEXT_INFONAME
                                 nnDA_NNSIEXT_HELPGETLOG
                                 nnDA_NNSIEXT_ENDINFONAME
                                 nnDA_NNSIEXT_ENDVIEW));
        // nnDAを起動する
        (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        break;

     case NNSH_GETLOG_FEATURE_NNSIEND:
        // NNsiを終了させる
        dummyEvent = &(NNshGlobal->dummyEvent);
        MemSet(dummyEvent, sizeof(EventType), 0x00);
        dummyEvent->eType = appStopEvent;
        EvtAddEventToQueue(dummyEvent);
        break;

      case NNSH_GETLOG_FEATURE_OPENMENU:
        // 選択メニューを表示
        NNshGlobal->jumpSelection = selectionMenu_getLogList();
        break;

      case NNSH_GETLOG_FEATURE_FORCE_DISCONNECT:
        // 回線切断その２
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        NNshNet_LineHangup();
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
        break;

      case NNSH_GETLOG_FEATURE_OPEN_GETLOGMENU:
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

      case NNSH_GETLOG_FEATURE_OPEN_MEMOURL:
        // MemoURLで開く
        openMemoURL_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_CONFIG_GETLOG:
        // 参照ログ一覧画面のハードキー詳細設定を実施する
        GetLog_SetHardKeyFeature();
        break;

      case NNSH_GETLOG_FEATURE_OPEN_NNSI:
        // 取得ログをNNsiで開く
        selItem = openNNsiThread_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        if (selItem == NNSH_DATABASE_UNKNOWN)
        {
            // 取得ログ表示をキャンセルした...何もせず抜ける
            break;
        }
        else if (selItem != NNSH_DATABASE_BLANK)
        {
            // ログは取得されていた、、、参照画面を開く
            CloseForm_GetLogList(FRMID_MESSAGE);
            return (true);
        }
        // ログが取得できていない、Webブラウザで開くか確認する
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_OPENWEB, "", 0) == 0)
        {
            // Webブラウザで開く
            openWebBrowser_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        }
        break;

      case NNSH_GETLOG_FEATURE_SITE_INFORMATION:
        // サイト情報を表示する
        showSiteInfo_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_BEAM_URL:
        // サイト情報をBeamする
        beamSiteURL_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_OPENWEBDA:
        // サイトをwebDAで開く
        openURLwebDA_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_DELETELOG:
        // ログを一括削除
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_DELETELOGS, "", 0) == 0)
        {
            // 確認がとれたときにはログ削除
            selItem = deleteAllThread_getLogList((((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK), false);
            if (selItem != 0)
            {
                // 削除後、削除したログ件数を応答する
                NNsh_InformMessage(ALTID_INFO, "", MSG_DELETED_LOGFILES, selItem);
            }
            else
            {
                // 削除すべきログが見つからなかったとき...
                NNsh_InformMessage(ALTID_INFO, "", MSG_NOT_DELETED_LOGFILE, 0);                
            }
        }
        break;

      case NNSH_GETLOG_FEATURE_DELETELOG_READ:
        // 既読ログを一括削除
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_DELETELOGS_READ, "", 0) == 0)
        {
            // 確認がとれたときにはログ削除
            selItem = deleteAllThread_getLogList((((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK), true);
            if (selItem != 0)
            {
                // 削除後、削除したログ件数を応答する
                NNsh_InformMessage(ALTID_INFO, "", MSG_DELETED_LOGFILES, selItem);
            }
            else
            {
                // 削除すべきログが見つからなかったとき...
                NNsh_InformMessage(ALTID_INFO, "", MSG_NOT_DELETED_LOGFILE, 0);                
            }
        }
        break;

      default:
        // 知らない機能番号が指定された
        NNsh_DebugMessage(ALTID_INFO, "Unknown Feature Number :", "", feature);
        return (false);
        break;
    }
    // 必ずredrawしてみる...
    RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
    return (true);
}

/*=========================================================================*/
/*   Function :   clearList_getLogList                                     */
/*                              参照ログの取得一覧表示用データをクリアする */
/*=========================================================================*/
void ClearList_getLogList(NNshDispList *dispList)
{
    UInt16 lp;

    for (lp = 0; lp < dispList->dataCount; lp++)
    {
        MEMFREE_PTR(dispList->title[lp]);
    }
    dispList->dataCount = 0;
    return;
}


/*=========================================================================*/
/*   Function :   CloseForm_GetLogList                                     */
/*                                          参照ログの取得一覧画面を閉じる */
/*=========================================================================*/
Err CloseForm_GetLogList(UInt16 nextFormId)
{

    // あとしまつ...
    ClearList_getLogList(NNshGlobal->dispList);
    MEMFREE_PTR(NNshGlobal->dispList);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "CLOSE getloglist next: %d ", nextFormId);
#endif // #ifdef USE_REPORTER

    if (nextFormId != 0)
    {
        // 次の画面を開く...
        FrmGotoForm(nextFormId);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_GetLogList                                      */
/*                                            参照ログの取得一覧画面を開く */
/*=========================================================================*/
Err OpenForm_GetLogList(FormType  *frm)
{
    UInt16 update;

    // ワーク領域を確保する
    NNshGlobal->dispList = MEMALLOC_PTR(sizeof(NNshDispList) + MARGIN);
    if (NNshGlobal->dispList == NULL)
    {
        // ワーク領域の確保失敗...
        return (~errNone);
    }
    MemSet(NNshGlobal->dispList, (sizeof(NNshDispList) + MARGIN), 0x00);

    // 一覧を(設定ログレベルに合わせ)更新する
    if ((NNshGlobal->NNsiParam)->getROLogLevel == 0)
    {
        update = 0;
    }
    else
    {
         update = (NNshGlobal->NNsiParam)->getROLogLevel + 1;
    }

    // 画面に表示するデータを構築する
    Show_BusyForm(MSG_FINDING_WAIT);
    createList_getLogList(update, NNshGlobal->dispList);
    Hide_BusyForm(false);

    // work1 は、表示するデータのアイテム数、work2は、表示可能ライン数を格納する
    // (NNshGlobal->NNsiParam)->getLogListPage      = 0;
    // (NNshGlobal->NNsiParam)->getLogListSelection = 0;
    NNshGlobal->work1 = 0;
    NNshGlobal->work2 = 0;

    RefreshDisplay_getLogList(frm, NNSH_STEP_NEWOPEN, (NNshGlobal->NNsiParam)->getLogListSelection);

    // 開いたフォームIDを設定する(しおり設定中には、lastFrmIDは変更しない)
    if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
    {
        // しおり設定中なので、何もしない
    }
    else
    {
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_GETLOGLIST;
    }
    NNshGlobal->backFormId = FRMID_LISTGETLOG;
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectOpenThread_getLogList                                */
/*                                          一覧から読みたい記事を選択する */
/*-------------------------------------------------------------------------*/
static UInt16 selectOpenThread_getLogList(NNshGetLogDatabase *dbData, UInt16 recNum, Char *fileName)
{
    Err                  ret;
    UInt16               index, dbCount, loop, itemCnt;
    UInt16              *recArray;
    Char                *selLabel, *labelPtr;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subjData;
    MemHandle            memH;

    // 変数の初期化
    index    = NNSH_DATABASE_BLANK;
    recArray = NULL;
    selLabel = NULL;

    // ファイル名の先頭を用意する
    StrCopy(fileName, FILE_LOGCHARGE_PREFIX);
    NUMCATI(fileName, recNum);
    StrCat (fileName, "-");

    // レコード番号格納用のエリアを確保する
    loop = (sizeof(UInt16) * (MAX_URLLIST_CNT + MARGIN + 1));
    recArray = MEMALLOC_PTR(loop + MARGIN);
    if (recArray == NULL)
    {

        return (NNSH_DATABASE_UNKNOWN);
    }
    MemSet(recArray, (loop + MARGIN), 0x00);
    
    // 選択ラベル用のエリアを確保する
    loop = (sizeof(Char) * (MAX_NAMELABEL + MARGIN) * (MAX_URLLIST_CNT + MARGIN + 1));
    selLabel = MEMALLOC_PTR(loop + MARGIN);
    if (selLabel == NULL)
    {
        // 選択ラベル用の領域確保失敗...終了する
        MEMFREE_PTR(recArray);
        return (NNSH_DATABASE_UNKNOWN);
    }
    MemSet(selLabel, (loop + MARGIN), 0x00);

    // スレデータベースのオープン
    dbRef = 0;
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // スレDBのオープン失敗、終了する
        MEMFREE_PTR(selLabel);
        MEMFREE_PTR(recArray);
        return (NNSH_DATABASE_UNKNOWN);
    }

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &dbCount);

    // BUSYウィンドウを表示する(URL確認中)
    Show_BusyForm(MSG_CHECK_URLLIST);
        
    itemCnt  = 0;
    labelPtr = selLabel;

    // レコード番号の若いほうから順に表示する...
    for (loop = dbCount; loop != 0; loop--)
    {
        // レコード情報を取得する
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &memH, (void**) &subjData);
        if (ret == errNone)
        {
            if ((subjData->state != NNSH_SUBJSTATUS_NOT_YET)&&
                (subjData->state != NNSH_SUBJSTATUS_DELETE))
            {
                // スレデータ取得済みのとき...
                if (StrNCompare(subjData->threadFileName, fileName, StrLen(fileName)) == 0)
                {
                    // 該当スレを発見! (選択リストに記録する)
                    recArray[itemCnt] = (loop - 1);

                    if (subjData->state == NNSH_SUBJSTATUS_NEW)
                    {
                        // 新スレの場合には、*をつける
                        StrCopy(labelPtr, "*");
                    }
                    else if (subjData->state == NNSH_SUBJSTATUS_REMAIN)
                    {
                        // 途中まで読んだスレの場合には、-をつける
                        StrCopy(labelPtr, "-");
                    }
                    else
                    {
                        StrCopy(labelPtr, " ");
                    }
                    StrNCopy(&labelPtr[StrLen(labelPtr)], subjData->threadTitle, MAX_NAMELABEL);
                    labelPtr = labelPtr + StrLen(labelPtr) + 1;
                    itemCnt++;
                    if (itemCnt >= (MAX_URLLIST_CNT + MARGIN))
                    {
                        // 選択可能なリスト数を超えた...ループから抜ける
                        //  (確保したレコードを解除してから...)
                        ReleaseRecordReadOnly_NNsh(dbRef, memH);
                        break;
                    }
                }
            }
            // 確保したレコードを解除する
            ReleaseRecordReadOnly_NNsh(dbRef, memH);
        }
    }
    Hide_BusyForm(false);

    // DBをクローズする...
    CloseDatabase_NNsh(dbRef);

    // 抽出レコードが0件ではなかった場合...
    if (itemCnt != 0)
    {
        // 選択ウィンドウを表示し、読みたい記事の選択を促す
        loop = NNshWinSelectionWindow(FRMID_DIRSELECTION, selLabel, itemCnt, 0);

        // OKボタンが押されたとき、、、
        if (loop != BTNID_JUMPCANCEL)
        {
            // indexを選択した記事のレコード番号に設定する
            index = recArray[NNshGlobal->jumpSelection];
        }
        else
        {
            // ＤＢの選択は行われなかった...
            index = NNSH_DATABASE_UNKNOWN;
        }
    }

    // 確保した領域を開放して終了する
    MEMFREE_PTR(selLabel);
    MEMFREE_PTR(recArray);
    return (index);
}

/*-------------------------------------------------------------------------*/
/*   Function : isDeleteRecord　　　　　　                                 */
/*                                            ログを削除する対象か判断する */
/*-------------------------------------------------------------------------*/
static Boolean isDeleteRecord(Boolean isReadLog, UInt8 state)
{
    if ((state == NNSH_SUBJSTATUS_NOT_YET)||
        (state == NNSH_SUBJSTATUS_DELETE))
    {
        // スレ状態が「未取得」「削除」のときは対象外
        return (false);
    }
    if (isReadLog == true)
    {
        if (state == NNSH_SUBJSTATUS_ALREADY)
        {
            // 既読削除モードで、スレ状態が既読のときは対象
            return (true);
        }

        // 既読削除モードで、スレ状態が既読でなければ削除対象外
        return (false);
    }

    // スレ状態が上記以外は削除対象
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : deleteAllThread_getLogList                                 */
/*                                                      ログを一括削除する */
/*-------------------------------------------------------------------------*/
static UInt16 deleteAllThread_getLogList(UInt16 recNum, Boolean isReadLog)
{
    Err                  ret;
    UInt16               dbCount, loop, itemCnt;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subjData;
    MemHandle            memH;
    Char                 fileName[MAX_THREADFILENAME + MARGIN];

    // ファイル名の先頭を用意する
    MemSet (fileName, (MAX_THREADFILENAME + MARGIN), 0x00);
    StrCopy(fileName, FILE_LOGCHARGE_PREFIX);
    NUMCATI(fileName, recNum);
    StrCat (fileName, "-");

    // 削除件数をクリアする
    itemCnt = 0;

    // スレデータベースのオープン
    dbRef = 0;
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // スレDBのオープン失敗、終了する
        return (itemCnt);
    }

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &dbCount);

    // BUSYウィンドウを表示する(パージ処理中の表示)
    Show_BusyForm(MSG_EXECUTE_PURGE);

    // レコード番号の若いほうから表示する...
    for (loop = dbCount; loop != 0; loop--)
    {
        // レコード情報を取得する
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &memH, (void**) &subjData);
        if (ret == errNone)
        {
            // 削除対象のレコードかチェックする
            if (isDeleteRecord(isReadLog, subjData->state) == true)
            {
                // スレデータ取得済みのとき...
                if (StrNCompare(subjData->threadFileName, fileName, StrLen(fileName)) == 0)
                {
                    // 確保したレコードを解除する
                    ReleaseRecordReadOnly_NNsh(dbRef, memH);

                    // 一度DBをクローズする...
                    CloseDatabase_NNsh(dbRef);

                    // 該当スレを発見! (スレを削除する)
                    DeleteThreadMessage(loop - 1);
                    itemCnt++;
                    
                    // 再度DBをオープンする
                    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
                    if (dbRef == 0)
                    {
                        // なぜかオープン失敗...この場合にはループを抜ける
                        break;   
                    }
                }
                else
                {
                    // 確保したレコードを解除する
                    ReleaseRecordReadOnly_NNsh(dbRef, memH);            
                }
            }
            else
            {
                // 確保したレコードを解除する
                ReleaseRecordReadOnly_NNsh(dbRef, memH);
            }
        }
    }
    Hide_BusyForm(false);

    // DBをクローズする...
    CloseDatabase_NNsh(dbRef);

    // 削除したレコード数を応答する
    return (itemCnt);
}

#endif  // #ifdef USE_LOGCHARGE
