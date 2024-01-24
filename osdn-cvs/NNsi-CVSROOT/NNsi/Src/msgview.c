/*============================================================================*
 *
 *  $Id: msgview.c,v 1.209 2009/03/07 11:30:36 mrsa Exp $
 *
 *  FILE: 
 *     msgview.c
 *
 *  Description: 
 *     Message(Thread) Viewer for NNsh. 
 *
 *===========================================================================*/
#define MSGVIEW_C
#include "local.h"
#include "Clipboard.h"

static Err PartReceiveMessage_View(void);
static void NNsh_Search_Message(UInt16 type, Char *str, Boolean flag);
static Err NNsh_Get_MessageToBuf(NNshMessageIndex *idxP, UInt16 msg, UInt16 type, Boolean flag);


/*-------------------------------------------------------------------------*/
/*   Function : pickupAnchor                                               */
/*                                    メッセージ内のアンカー検出および選択 */
/*-------------------------------------------------------------------------*/
static Boolean pickupAnchor(NNshMessageIndex *idxP, Char *buffer, UInt16 size)
{
    Boolean ret;
    UInt16  itemCnt, bufCnt, getState, kanji, btnId;
    UInt32  dataSize;
    Char   *data, *chkPtr, *labelP, *addrAddr[MAX_URL_LIST + 1];
    Char   *ptr, *ptr2, *ptr3;
    Char    tempURL[MAX_URL * 2 + MARGIN], tempLabel[MAX_NAMELABEL], *addrLabel;

    // アンカー検出するデータバッファのサイズ（幅）を確定する
    if ((idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
    {
        dataSize = (idxP->msgOffset)[idxP->nofMsg];
    }
    else
    {
        dataSize = (idxP->msgOffset)[idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode];
    }
    dataSize = dataSize - (idxP->msgOffset)[idxP->openMsg - 1];

    if (idxP->fileOffset > (idxP->msgOffset)[idxP->openMsg - 1])
    {
         // データバッファにデータが読み込まれていない(ありえないはず...)
         // エラーにする
#ifdef USE_REPORTER
         HostTraceOutputTL(appErrorClass, " NOT FOUND DATA: %lu , %lu, %d ",
                           idxP->fileOffset, 
                           (idxP->msgOffset)[idxP->openMsg - 1], idxP->openMsg);
#endif // #ifdef USE_REPORTER
         return (false);
    }

    // データラベル領域の確保
    addrLabel = MEMALLOC_PTR(((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN));
    if (addrLabel == NULL)
    {
        // データ領域確保失敗、エラーにする
        return (false);
    }
    MemSet(addrLabel, ((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN), 0x00);
    labelP = addrLabel;

    // "URL確認中"のBUSYフォームを出力する
    Show_BusyForm(MSG_CHECK_URLLIST);
    
    // 変数dataに、アンカーを検索する先頭のポインタを設定する
    data = idxP->buffer + (idxP->msgOffset)[idxP->openMsg - 1] - idxP->fileOffset - MARGIN;
    itemCnt = 0;
    chkPtr = data;

    // 文字が表示されるまでデータを読み飛ばす
    while  ((chkPtr < data + dataSize)&&(*chkPtr < ' '))
    {
        chkPtr++;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, " chkPtr: %lu , data:%lu, size:%lu ", chkPtr, data, dataSize);
#endif // #ifdef USE_REPORTER

    // アンカー切り出しメイン処理
    while (chkPtr < data + dataSize)
    {
        // itemラベルを抽出する
        ptr3 = StrStr(chkPtr, "<item");
        if ((ptr3 != NULL)&&(ptr3 < (data + dataSize)))
        {
            // アンカーを切り出す
            ptr = StrStr(chkPtr, "<link>");
            if (ptr != NULL)
            {
                // <link> を検出した...リンクを切り出す
                ptr  = StrStr(ptr, "http://");
                ptr2 = StrStr(ptr, "https://");
                if ((ptr == NULL)||((ptr2 != NULL)&&(ptr > ptr2)))
                {
                    // https:// を選択
                    ptr = ptr2;
                }
                if (ptr != NULL)
                {
                    // URLの末尾を特定する...
                    ptr2 = StrStr(ptr, "<");
                    while (ptr < ptr2)
                    {
                        ptr2--;
                        if (*ptr2 > ' ')
                        {
                            break;
                        }
                    }

                    MemSet(tempURL, sizeof(tempURL), 0x00);
                    if (sizeof(tempURL) >  ((ptr2 - ptr) + 1))
                    {
                        MemMove(tempURL, ptr, ((ptr2 - ptr) + 1));
                    }
                    else
                    {
                        MemMove(tempURL, ptr, (sizeof(tempURL) - 1));
                    }

                    // URL切り出し成功、アンカーラベルを構築する
                    MemSet(tempLabel, MAX_NAMELABEL, 0x00);
                    ptr = StrStr(ptr3, "<title>");
                    if (ptr == NULL)
                    {
                        // タイトルが見つからなかったとき、、、URLをラベルに
                        if (StrLen(tempURL) < MAX_NAMELABEL)
                        {
                            StrCopy(tempLabel, tempURL);
                        }
                        else
                        {
                            StrNCopy(tempLabel,
                                     &tempURL[StrLen(tempURL) - (35 - 1)],
                                     MAX_NAMELABEL);
                        }
                    }
                    else
                    {
                        // タイトルを発見、アンカーラベルにする。
                        // (RDF形式なので、UTF8だろう。。きめうち)
                        ptr = ptr + StrLen("<title>");
                        StrNCopyUTF8toSJ(tempLabel, ptr, (MAX_NAMELABEL - MARGIN));
                    }

                    // アンカー格納ロジックへ...
                    kanji = NNSH_BBSTYPE_CHAR_SJIS;  // 仮...

#ifdef USE_REPORTER
                    HostTraceOutputTL(appErrorClass, "anchor[%s], label[%s]", tempURL, tempLabel);
#endif // #ifdef USE_REPORTER
             
                    // HTTPアンカーが最大数に到達していない場合、データを入れる
                    if (itemCnt < MAX_URL_LIST)
                    {
                        // URLアンカーを格納する
                        addrAddr[itemCnt] = MEMALLOC_PTR(StrLen(tempURL) + MARGIN);
                        if (addrAddr[itemCnt] == NULL)
                        {
                            // 領域確保失敗、ループを抜ける
                            break;
                        }
                        MemSet(addrAddr[itemCnt], (StrLen(tempURL) + MARGIN), 0x00);
                        StrCopy(addrAddr[itemCnt], tempURL);

                        StrCopy(labelP, tempLabel);

                        // 文字列の末尾に省略記号をつける
                        labelP[StrLen(labelP)] = chrEllipsis;

                        // ポインタを次の領域に移動する
                        labelP = labelP + StrLen(labelP) + 1;

                        // アイテム数を増やす
                        itemCnt++;
                    }
                    chkPtr = ptr2;
                    continue;
                }
            }
        }

        // バッファ内にあるアンカーを探す（大文字と小文字のアンカー両方）
        ptr = StrStr(chkPtr, "<a");
        ptr2 = StrStr(chkPtr, "<A");
        if ((ptr == NULL)&&(ptr2 == NULL))
        {
            // もうアンカーがない、ループを抜ける
#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " anchor nothing... ");
#endif // #ifdef USE_REPORTER
            break;
        }

        // 大文字アンカーの方が前にある、大文字アンカーを有効にする
        if ((ptr == NULL)||((ptr2 != NULL)&&(ptr > ptr2)))
        {
            ptr = ptr2;
        }
        while (ptr < data + dataSize)
        {
            // 'href' を探す
            if (((*ptr == 'h')||(*ptr == 'H'))&&
                ((*(ptr + 1) == 'r')||(*(ptr + 1) == 'R'))&&
                ((*(ptr + 2) == 'e')||(*(ptr + 2) == 'E'))&&
                ((*(ptr + 3) == 'f')||(*(ptr + 3) == 'F')))
            {
#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " find HREF anchor... ");
#endif // #ifdef USE_REPORTER
                // hrefアトリビュート発見
                ptr = ptr + 4; // hrefを読み飛ばし
                while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != 0x00))
                {
                    ptr++;
                }
                if (*ptr == 0x00)
                {
                    // アンカーの途中でバッファが切れた。。。ループを抜ける
                    break;
                }
                ptr++;

                // URL切り出しバッファの初期化
                bufCnt = 0;
                MemSet(tempURL, sizeof(tempURL), 0x00);

                if ((StrNCaselessCompare(ptr, "http", StrLen("http")) != 0)||
                    ((*(ptr + 4) != 's')&&(*(ptr + 4) != ':'))||
                    (*(ptr + 6) != '/'))
                {
#ifdef USE_REPORTER
                     HostTraceOutputTL(appErrorClass, " not HTTP ");
#endif // #ifdef USE_REPORTER

                    // httpなURLではない、あらかじめベースURLを付加する
                    StrNCopy(tempURL, idxP->baseURL, (sizeof(tempURL) - 1));
                    bufCnt = StrLen(tempURL);

                    // URLは、ホストからの絶対アドレス指定
                    if (*ptr == '/')
                    {
                        // ホスト名だけを切り取る
                        bufCnt = StrLen("http://");
                        while ((tempURL[bufCnt] != '/')&&(tempURL[bufCnt] != '\0'))
                        {
                            bufCnt++;
                        }
                        tempURL[bufCnt] = '\0';                                                
                    }
                    else if (*ptr == '.')
                    {
                        // URLは、今の位置から相対アドレス指定だった場合、、、
                        ptr++;
                        ptr++;
                    }
                }

                // URLデータの切り出し
                while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != 0x00)&&(bufCnt < (sizeof(tempURL) - 2)))
                {
                    tempURL[bufCnt] = *ptr;
                    ptr++;
                    bufCnt++;
                }
                // アンカータグの末尾まで読み飛ばす
                while ((*ptr != '>')&&(*ptr != 0x00))
                {
                    ptr++;
                }
                ptr++;

#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " pick up anchor label ");
#endif // #ifdef USE_REPORTER

                // アンカーラベルの切り出し
                kanji = idxP->bbsType & NNSH_BBSTYPE_CHARSETMASK;
                getState = HTTP_ANCHORSTATE_NORMAL;
                bufCnt = 0;
                MemSet(tempLabel, MAX_NAMELABEL, 0x00);
                while ((*ptr != 0x00)&&(ptr < data + dataSize)&&
                        (bufCnt < (MAX_NAMELABEL - 2)))
                {
                    if ((kanji == NNSH_BBSTYPE_CHAR_JIS)&&(*ptr == 0x1b))
                    {
                        // エスケープコード発見 KNJ/ANK切り替え
                        if ((*(ptr + 1) == 0x24)&&(*(ptr + 2) == 0x42))
                        {
                            // 漢字コードに切り替え
                            getState = HTTP_ANCHORSTATE_KANJI;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            ptr++;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            ptr++;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            // ptr++ は、ifブロックの次にあるところで実施する
                        }
                        else if ((*(ptr + 1) == 0x28)&&(*(ptr + 2) == 0x42))
                        {
                            // ANKモードに切り替え
                            getState = HTTP_ANCHORSTATE_NORMAL;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            ptr++;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            ptr++;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            // ptr++ は、ifブロックの次にあるところで実施する
                        }
                    }
                    else if ((*ptr == 0x0a)||(*ptr == 0x0d))
                    {
                        // 改行コードは読み飛ばす
                    }
                    else if ((getState == HTTP_ANCHORSTATE_NORMAL)&&(*ptr == '<'))
                    {
                        getState = HTTP_ANCHORSTATE_ANCHOR;
                        if ((*(ptr + 1) == 'A')||(*(ptr + 1) == 'a'))
                        {
                            // 次のアンカーが見つかった、抜ける
                            ptr--;
                            break;
                        }
                    }
                    else if ((getState == HTTP_ANCHORSTATE_ANCHOR)&&(*ptr == '>'))
                    {
                        getState = HTTP_ANCHORSTATE_NORMAL;
                    }
                    else if ((getState == HTTP_ANCHORSTATE_NORMAL)||
                              (getState == HTTP_ANCHORSTATE_KANJI))
                    {
                        tempLabel[bufCnt] = *ptr;
                        bufCnt++;
                    }
                    ptr++;                
                }
              
                // HTTPアンカーが最大数に到達していない場合、データを入れる
                if (itemCnt < MAX_URL_LIST)
                {
                    // URLアンカーを格納する
                    addrAddr[itemCnt] = MEMALLOC_PTR(StrLen(tempURL) + MARGIN);
                    if (addrAddr[itemCnt] == NULL)
                    {
                        // 領域確保失敗、ループを抜ける
                        break;
                    }
                    MemSet(addrAddr[itemCnt], (StrLen(tempURL) + MARGIN), 0x00);
                    StrCopy(addrAddr[itemCnt], tempURL);

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " URL : %s (label:%s)", tempURL, tempLabel);
#endif // #ifdef USE_REPORTER
                    // データラベルを(漢字コード変換も同時に実施して)格納
                    switch (kanji)
                    {
                      case NNSH_BBSTYPE_CHAR_JIS:
                        StrCopyJIStoSJ(labelP, tempLabel);
                        break;

                      case NNSH_BBSTYPE_CHAR_EUC:
                        StrCopySJ(labelP, tempLabel);
                        break;

                      case NNSH_BBSTYPE_CHAR_SJIS:
                      default:
                        StrCopy(labelP, tempLabel);
                        break;
                    }
                    // 文字列の末尾に省略記号をつける
                    labelP[StrLen(labelP)] = chrEllipsis;

                    // ポインタを次の領域に移動する
                    labelP = labelP + StrLen(labelP) + 1;

                    // アイテム数を増やす
                    itemCnt++;
                }
            }
            ptr++;
        }
        chkPtr = ptr;
    }  // while (chkPtr < data + dataSize)
    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // １つもアンカーがない、終了する
        MEMFREE_PTR(addrLabel);
        return (false);
    }
    ret = false;

    // 選択ウィンドウを表示し、URLの選択を促す
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, addrLabel, itemCnt, 0);

    // OKボタンが押されたとき、、、
    if (btnId != BTNID_JUMPCANCEL)
    {
        // データをコピーする
        StrNCopy(buffer, addrAddr[NNshGlobal->jumpSelection], size);
        ret = true;

        // 一応、開くページ(URL)を表示できるようにしておく
        NNsh_DebugMessage(ALTID_INFO, buffer, MSG_WEBLAUNCH_CONFIRM, 0);
    }

    // 確保した領域を開放する
    for (bufCnt = 0; bufCnt < itemCnt; bufCnt++)
    {
        MEMFREE_PTR(addrAddr[bufCnt]);
    }
    MEMFREE_PTR(addrLabel);

    return (ret); 
}

/*-------------------------------------------------------------------------*/
/*   Function : pickupAnchorURL                                            */
/*                                         メッセージ内のURL検出および選択 */
/*-------------------------------------------------------------------------*/
static Boolean pickupAnchorURL(NNshMessageIndex *idxP, Char *buffer, UInt16 size)
{
    Boolean ret, isHttps;
    UInt16  itemCnt, bufCnt, btnId;
    UInt32  dataSize;
    Char   *data, *chkPtr, *ptr, *labelP, *addrAddr[MAX_URL_LIST + MARGIN];
    Char   *tempURL, tempLabel[MAX_NAMELABEL], *addrLabel;

    // URL一時保管領域を確保
    tempURL = MEMALLOC_PTR(MAX_URL * 3 + MARGIN);
    if (tempURL == NULL)
    {
        return (false);
    }
    MemSet(tempURL, (MAX_URL * 3 + MARGIN), 0x00);

    // アンカー検出するデータバッファのサイズ（幅）を確定する
    if ((idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
    {
        dataSize = (idxP->msgOffset)[idxP->nofMsg];
    }
    else
    {
        dataSize = (idxP->msgOffset)[idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode];
    }
    if (idxP->openMsg != 0)
    {
        dataSize = dataSize - (idxP->msgOffset)[idxP->openMsg - 1];

        if (idxP->fileOffset > (idxP->msgOffset)[idxP->openMsg - 1])
        {
             // データバッファにデータが読み込まれていない(ありえないはず...)
             // エラーにする
             NNsh_DebugMessage(ALTID_INFO, "illegal range", " ", 0);
             MEMFREE_PTR(tempURL);
             return (false);
        }
    }

    // データラベル領域の確保
    addrLabel = MEMALLOC_PTR(((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN));
    if (addrLabel == NULL)
    {
        // データ領域確保失敗、エラーにする
        NNsh_DebugMessage(ALTID_INFO, "alloc fail", " ", 0);
        MEMFREE_PTR(tempURL);
        return (false);
    }
    MemSet(addrLabel, ((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN), 0x00);
    labelP = addrLabel;

    // "URL確認中"のBUSYフォームを出力する
    Show_BusyForm(MSG_CHECK_URLLIST);

    // 変数dataに、アンカーを検索する先頭のポインタを設定する
    data = idxP->buffer - idxP->fileOffset - MARGIN;
    if (idxP->openMsg != 0)
    {
        data = data + (idxP->msgOffset)[idxP->openMsg - 1];
    }
    itemCnt = 0;
    chkPtr = data;

    // 文字が表示されるまでデータを読み飛ばす
    while  ((chkPtr < data + dataSize)&&(*chkPtr < ' '))
    {
        chkPtr++;
    }

    // アンカー切り出しメイン処理
    while (chkPtr < data + dataSize)
    {
        // バッファ内にあるURLを探す
        isHttps = false;
        ptr = StrStr(chkPtr, "tp://");
        if (ptr == NULL)
        {
            ptr = StrStr(chkPtr, "tps://");
            isHttps = true;
        }
        if (ptr == NULL)
        {
            // もうアンカーがない、ループを抜ける
            NNsh_DebugMessage(ALTID_INFO, "link:", " ", itemCnt);
            break;
        }
        // 先に進める...
        ptr = ptr + sizeof("tp://") - 1;
        if (isHttps == true)
        {
            // https の's'分だけ進める
            ptr++;
        }

        // 検出したURLが範囲外ならループを抜ける
        if (ptr >= data + dataSize)
        {
            NNsh_DebugMessage(ALTID_INFO, "LINK:", " ", itemCnt);
            break;
        }

        // 検出したURLを抽出
        NNsh_DebugMessage(ALTID_INFO, "http(s)://", ptr, isHttps);

        MemSet (tempURL, (MAX_URL * 3 + MARGIN), 0x00);
        if (isHttps == false)
        {
            StrCopy(tempURL, "http://");
            bufCnt = 7; // 7 は、StrLen(tempURL);
        }
        else
        {
            StrCopy(tempURL, "https://");
            bufCnt = 8; // 8 は、StrLen(tempURL);
        }

        // URLデータの切り出し
        while ((*ptr > ' ')&&(*ptr != '>')&&(*ptr != '<')&&
                (*ptr != '"')&&(*ptr != '\''))
        {
            tempURL[bufCnt] = *ptr;
            ptr++;
            bufCnt++;
            if (bufCnt > (MAX_URL * 3))
            {
                break;
            }
        }

        // URLラベルのコピー
        MemSet(tempLabel, MAX_NAMELABEL, 0x00);
        if (bufCnt < (MAX_NAMELABEL - 2))
        {
            StrCopy(tempLabel, tempURL);
        }
        else
        {
            tempLabel[0] = chrEllipsis;
            StrNCopy(&tempLabel[1], 
                     &tempURL[bufCnt - (MAX_NAMELABEL - 1)],
                     (MAX_NAMELABEL - 2));
        }
                
        // HTTPアンカーが最大数に到達していない場合、データを入れる
        if (itemCnt < MAX_URL_LIST)
        {
            // URLアンカーを格納する
            addrAddr[itemCnt] = MEMALLOC_PTR(StrLen(tempURL) + MARGIN);
            if (addrAddr[itemCnt] == NULL)
            {
                // 領域確保失敗、ループを抜ける
                break;
            }
            MemSet(addrAddr[itemCnt], (StrLen(tempURL) + MARGIN), 0x00);
            StrCopy(addrAddr[itemCnt], tempURL);

            // データラベルを(漢字コード変換も同時に実施して)格納
            StrNCopy(labelP, tempLabel, (MAX_NAMELABEL - 1));

            // ポインタを次の領域に移動する
            labelP = labelP + StrLen(labelP) + 1;

            // アイテム数を増やす
            itemCnt++;
        }
        ptr++;
        chkPtr = ptr;
    }  // while (chkPtr < data + dataSize)
    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // １つもアンカーがない、終了する
        MEMFREE_PTR(tempURL);
        MEMFREE_PTR(addrLabel);
        return (false);
    }
    ret = false;

    // 選択ウィンドウを表示し、URLの選択を促す
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, addrLabel, itemCnt, 0);

    // OKボタンが押されたとき、、、
    if (btnId != BTNID_JUMPCANCEL)
    {
        // データをコピーする
        MemSet  (buffer, size, 0x00);
        StrNCopy(buffer, addrAddr[NNshGlobal->jumpSelection], (size - 1));
        ret = true;

        // 一応、開くページ(URL)を表示できるようにしておく
        NNsh_DebugMessage(ALTID_INFO, buffer, MSG_WEBLAUNCH_CONFIRM, 0);
    }

    // 確保した領域を開放する
    for (bufCnt = 0; bufCnt < itemCnt; bufCnt++)
    {
        MEMFREE_PTR(addrAddr[bufCnt]);
    }
    MEMFREE_PTR(addrLabel);
    MEMFREE_PTR(tempURL);
    return (ret); 
}

/*-------------------------------------------------------------------------*/
/*   Function : pickupNGwordCandidate                                      */
/*                                              NGワードの"候補"を抽出する */
/*-------------------------------------------------------------------------*/
static Boolean pickupNGwordCandidate(NNshMessageIndex *idxP, Char *buffer, UInt16 size)
{
    Boolean ret, isName;
    UInt16  itemCnt, bufCnt, btnId;
    UInt32  dataSize;
    Char   *data, *chkPtr, *ptr, *ptr2, *labelP, *addrAddr[MAX_URL_LIST + MARGIN];
    Char   *tempURL, tempLabel[MAX_NAMELABEL], *addrLabel;

    // URL一時保管領域を確保
    tempURL = MEMALLOC_PTR(MAX_URL * 2 + MARGIN);
    if (tempURL == NULL)
    {
        return (false);
    }
    MemSet(tempURL, (MAX_URL * 2 + MARGIN), 0x00);

    // アンカー検出するデータバッファのサイズ（幅）を確定する
    if ((idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
    {
        dataSize = (idxP->msgOffset)[idxP->nofMsg];
    }
    else
    {
        dataSize = (idxP->msgOffset)[idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode];
    }
    if (idxP->openMsg != 0)
    {
        dataSize = dataSize - (idxP->msgOffset)[idxP->openMsg - 1];

        if (idxP->fileOffset > (idxP->msgOffset)[idxP->openMsg - 1])
        {
             // データバッファにデータが読み込まれていない(ありえないはず...)
             // エラーにする
             NNsh_DebugMessage(ALTID_INFO, "illegal range", " ", 0);
             MEMFREE_PTR(tempURL);
             return (false);
        }
    }

    // データラベル領域の確保
    addrLabel = MEMALLOC_PTR(((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN));
    if (addrLabel == NULL)
    {
        // データ領域確保失敗、エラーにする
        NNsh_DebugMessage(ALTID_INFO, "alloc fail", " ", 0);
        MEMFREE_PTR(tempURL);
        return (false);
    }
    MemSet(addrLabel, ((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN), 0x00);
    labelP = addrLabel;

    // "キーワード抽出中"のBUSYフォームを出力する
    Show_BusyForm(MSG_PICKUP_KEYWORD);

    // 変数dataに、アンカーを検索する先頭のポインタを設定する
    data = idxP->buffer - idxP->fileOffset - MARGIN;
    if (idxP->openMsg != 0)
    {
        data = data + (idxP->msgOffset)[idxP->openMsg - 1];
    }
    itemCnt = 0;
    chkPtr = data;

    // 文字が表示されるまでデータを読み飛ばす
    while  ((chkPtr < data + dataSize)&&(*chkPtr < ' '))
    {
        chkPtr++;
    }
    ptr = chkPtr;  // 先頭は名前欄として抽出する...

    // キーワード切り出しメイン処理
    while (chkPtr < data + dataSize)
    {
        // バッファ内にあるキーワードを探す
        isName = true;
        ptr = StrStr(chkPtr, "\x0a");
        ptr2 = StrStr(chkPtr, "ID:");
        if ((ptr != NULL)&&(ptr2 != NULL)&&(ptr2 < ptr))
        {
            isName = false;
            ptr = ptr2;
        }
        if (ptr == NULL)
        {
            if (ptr2 == NULL)
            {
                // もうキーワードがない、ループを抜ける
                NNsh_DebugMessage(ALTID_INFO, "WD:", " ", itemCnt);
                break;
            }
            else
            {
                isName = false;
                ptr = ptr2;
            }
        }
        // 名前欄の検出だった場合、ちょっと先に進める...
        if (isName == true)
        {
            ptr = ptr + 1;
        }

        // 検出したキーワードが範囲外ならループを抜ける
        if (ptr >= data + dataSize)
        {
            NNsh_DebugMessage(ALTID_INFO, "WD:", " ", itemCnt);
            break;
        }

        bufCnt = 0;
        MemSet (tempURL, (MAX_URL * 2 + MARGIN), 0x00);

        // キーワードの切り出し
        while ((*ptr != ' ')&&(*ptr != '>')&&(*ptr != '<')&&
                (*ptr != '"')&&(*ptr != '\''))
        {
            tempURL[bufCnt] = *ptr;
            ptr++;
            bufCnt++;
        }

        // キーワードのコピー
        MemSet(tempLabel, MAX_NAMELABEL, 0x00);
        StrNCopy(tempLabel, tempURL, (MAX_NAMELABEL - 1));

        // 検出したキーワードを抽出
        NNsh_DebugMessage(ALTID_INFO, "WD:", tempURL, isName);
                
        // キーワードが最大数に到達していない場合、データを入れる
        if (itemCnt < MAX_URL_LIST)
        {
            // キーワードを格納する
            addrAddr[itemCnt] = MEMALLOC_PTR(StrLen(tempURL) + MARGIN);
            if (addrAddr[itemCnt] == NULL)
            {
                // 領域確保失敗、ループを抜ける
                break;
            }
            MemSet(addrAddr[itemCnt], (StrLen(tempURL) + MARGIN), 0x00);
            StrCopy(addrAddr[itemCnt], tempURL);

            // データラベルを(漢字コード変換も同時に実施して)格納
            StrNCopy(labelP, tempLabel, (MAX_NAMELABEL - 1));

            // ポインタを次の領域に移動する
            labelP = labelP + StrLen(labelP) + 1;

            // アイテム数を増やす
            itemCnt++;
        }
        ptr++;
        chkPtr = ptr;
        ptr = NULL;
    }  // while (chkPtr < data + dataSize)
    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // １つもアンカーがない、終了する
        MEMFREE_PTR(tempURL);
        MEMFREE_PTR(addrLabel);
        return (false);
    }
    ret = false;

    // 選択ウィンドウを表示し、URLの選択を促す
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, addrLabel, itemCnt, 0);

    // OKボタンが押されたとき、、、
    if (btnId != BTNID_JUMPCANCEL)
    {
        // データをコピーする
        MemSet  (buffer, size, 0x00);
        StrNCopy(buffer, addrAddr[NNshGlobal->jumpSelection], (size - 1));
        ret = true;

        // 一応、抽出したキーワードを表示できるようにしておく
        NNsh_DebugMessage(ALTID_INFO, "selected : ", buffer, 0);
    }

    // 確保した領域を開放する
    for (bufCnt = 0; bufCnt < itemCnt; bufCnt++)
    {
        MEMFREE_PTR(addrAddr[bufCnt]);
    }
    MEMFREE_PTR(addrLabel);
    MEMFREE_PTR(tempURL);
    return (ret); 
}

/*-------------------------------------------------------------------------*/
/*   Function : pickupBeProfile                                            */
/*                              メッセージ内のBeプロフィール検出および選択 */
/*-------------------------------------------------------------------------*/
static Boolean pickupBeProfile(NNshMessageIndex *idxP, Char *buffer, UInt16 size)
{
    UInt16  itemCnt, bufCnt, btnId;
    UInt32  dataSize;
    Char   *data, *chkPtr, *ptr, *labelP, *addrAddr[MAX_URL_LIST + MARGIN];
    Char   *tempURL, tempLabel[MAX_NAMELABEL], *addrLabel;

    // URL一時保管領域を確保
    tempURL = MEMALLOC_PTR(MAX_URL * 2 + MARGIN);
    if (tempURL == NULL)
    {
        return (false);
    }
    MemSet(tempURL, (MAX_URL * 2 + MARGIN), 0x00);

    // アンカー検出するデータバッファのサイズ（幅）を確定する
    if ((idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
    {
        dataSize = (idxP->msgOffset)[idxP->nofMsg];
    }
    else
    {
        dataSize = (idxP->msgOffset)[idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode];
    }
    if (idxP->openMsg != 0)
    {
        dataSize = dataSize - (idxP->msgOffset)[idxP->openMsg - 1];

        if (idxP->fileOffset > (idxP->msgOffset)[idxP->openMsg - 1])
        {
             // データバッファにデータが読み込まれていない(ありえないはず...)
             // エラーにする
             NNsh_DebugMessage(ALTID_INFO, "illegal range", " ", 0);
             MEMFREE_PTR(tempURL);
             return (false);
        }
    }

    // データラベル領域の確保
    addrLabel = MEMALLOC_PTR(((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN));
    if (addrLabel == NULL)
    {
        // データ領域確保失敗、エラーにする
        NNsh_DebugMessage(ALTID_INFO, "alloc fail", " ", 0);
        MEMFREE_PTR(tempURL);
        return (false);
    }
    MemSet(addrLabel, ((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN), 0x00);
    labelP = addrLabel;

    // "Be ID抽出中..."のBUSYフォームを出力する
    Show_BusyForm(MSG_CHECK_BEID);

    // 変数dataに、アンカーを検索する先頭のポインタを設定する
    data = idxP->buffer - idxP->fileOffset - MARGIN;
    if (idxP->openMsg != 0)
    {
        data = data + (idxP->msgOffset)[idxP->openMsg - 1];
    }
    itemCnt = 0;
    chkPtr = data;

    // 文字が表示されるまでデータを読み飛ばす
    while  ((chkPtr < data + dataSize)&&(*chkPtr < ' '))
    {
        chkPtr++;
    }

    // アンカー切り出しメイン処理
    while (chkPtr < data + dataSize)
    {
        // バッファ内にあるBe IDを探す
        ptr = StrStr(chkPtr, "BE:");
        if (ptr == NULL)
        {
            // もうアンカーがない、ループを抜ける
            NNsh_DebugMessage(ALTID_INFO, "link:", " ", itemCnt);
            break;
        }
        // 先に進める...
        ptr = ptr + sizeof("BE:") - 1;

        // 検出したBE IDが範囲外ならループを抜ける
        if (ptr >= data + dataSize)
        {
            NNsh_DebugMessage(ALTID_INFO, "LINK:", " ", itemCnt);
            break;
        }

        // 検出したBE IDをデバッグ表示
        NNsh_DebugMessage(ALTID_INFO, "BE:", ptr, 0);

        MemSet (tempURL, (MAX_URL * 2 + MARGIN), 0x00);
        StrCopy(tempURL, "BE:");
        bufCnt = 3; // 3 は、StrLen("BE:");

        // BE iDの切り出し
        while ((*ptr > ' ')&&(*ptr != '>')&&(*ptr != '<')&&
                (*ptr != '"')&&(*ptr != '\'')&&(*ptr != '\x0d')&&(*ptr != '\x0d'))
        {
            tempURL[bufCnt] = *ptr;
            ptr++;
            bufCnt++;
        }

        // BE IDのコピー
        MemSet(tempLabel, MAX_NAMELABEL, 0x00);
        StrNCopy(tempLabel, tempURL, (MAX_NAMELABEL - 1));
                
        // BE IDが最大数に到達していない場合、データを入れる
        if (itemCnt < MAX_URL_LIST)
        {
            // BE IDを(選択リストに)格納する
            StrNCopy(labelP, tempLabel, (MAX_NAMELABEL - 1));
            addrAddr[itemCnt] = labelP;

            // ポインタを次の領域に移動する
            labelP = labelP + StrLen(labelP) + 1;

            // アイテム数を増やす
            itemCnt++;
        }
        ptr++;
        chkPtr = ptr;
    }  // while (chkPtr < data + dataSize)

    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // １つもアンカーがない、終了する
        MEMFREE_PTR(tempURL);
        MEMFREE_PTR(addrLabel);
        return (false);
    }
    if (itemCnt == 1)
    {
        // BE IDはバッファ内にひとつしかなかった...
        // (自動的に選択させ、応答する)
        NNshGlobal->jumpSelection = 0;
        goto CREATE_PROFILE_URL;
    }

    // 選択ウィンドウを表示し、URLの選択を促す
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, addrLabel, itemCnt, 0);

    // Cancelボタンが押されたとき、、、
    if (btnId == BTNID_JUMPCANCEL)
    {
        // 確保した領域を開放する
        MEMFREE_PTR(addrLabel);
        MEMFREE_PTR(tempURL);
        return (false);
    }

CREATE_PROFILE_URL:
    // Be プロフィール参照用のURLを作成する
    MemSet (buffer, size, 0x00);
    StrCopy(buffer, URL_BE2ch_PROFILE);
    StrCat (buffer, URL_PREFIX_BE2ch_PROFILE);

    // BeのID（レスに記述されたもの）をコピーする
    chkPtr = addrAddr[NNshGlobal->jumpSelection];
    chkPtr = chkPtr + sizeof("BE:") - 1; 
    ptr    = buffer + StrLen(buffer);
    while ((*chkPtr > ' ')&&(*chkPtr != '-')&&(*chkPtr != '#')&&(*chkPtr != ':'))
    {
        *ptr = *chkPtr;
        ptr++;
        chkPtr++;   
    }
    *ptr = '\0';  // NULLターミネート

    // 確保した領域を開放する
    MEMFREE_PTR(addrLabel);
    MEMFREE_PTR(tempURL);
    return (true); 
}

/*-------------------------------------------------------------------------*/
/*   Function : replaceHideWord                                            */
/*                                                   「透明あぼーん」実施  */
/*-------------------------------------------------------------------------*/
static void replaceHideWord(Char *buffer, Char *checkWord)
{
    UInt16 matchedWord;

    // NGワードだけを * に置換する
    matchedWord = StrLen(checkWord);
    buffer = StrStr(buffer, checkWord);    
    while (buffer != NULL)
    {
        MemSet(buffer, matchedWord, '*');
        buffer = buffer + matchedWord;
        buffer = StrStr(buffer, checkWord);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : checkHideMessage                                           */
/*                                                  (簡易)NGキーワード検閲 */
/*-------------------------------------------------------------------------*/
static void checkHideMessage(Char *buffer)
{
    UInt16  loop, maxCnt, matchedWord;
    Char   *ptr, **data, *checkWord, *area1, *area2;

    NNshNGwordDatabase *dbData;
    DmOpenRef           dbRef;
    MemHandle           dbH;
    
    // NGチェック１＆２を実行する場合、、、
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD1AND2) == NNSH_USE_NGWORD1AND2)
    {
        // NGキーワードに引っかかるかチェック
        if ((NNshGlobal->NNsiParam)->hideWord1[0] != '\0')
        {
            if (((NNshGlobal->NNsiParam)->useRegularExpression != 0)&&
                (NNshGlobal->hide1.wordmemH != 0))
            {
                // NGキーワードを複数指定した場合...
                data = MemHandleLock(NNshGlobal->hide1.wordmemH);
                for (loop = 0; loop < NNshGlobal->hide1.nofWord; loop++)
                {
                    if (StrStr(buffer, data[loop]) != NULL)
                    {
                        // キーワード発見！
                        MemHandleUnlock(NNshGlobal->hide1.wordmemH);
                        goto REPLACE_MESSAGE;
                    }
                }
                MemHandleUnlock(NNshGlobal->hide1.wordmemH);
            }
            else
            {
                // NGキーワードを複数指定しない場合
                if (StrStr(buffer, (NNshGlobal->NNsiParam)->hideWord1) != NULL)
                {
                    goto REPLACE_MESSAGE;
                }
            }
        }
        if ((NNshGlobal->NNsiParam)->hideWord2[0] != '\0')
        {
            if (((NNshGlobal->NNsiParam)->useRegularExpression != 0)&&
                (NNshGlobal->hide2.wordmemH != 0))
            {
                // NGキーワードを複数指定した場合...
                data = MemHandleLock(NNshGlobal->hide2.wordmemH);
                for (loop = 0; loop < NNshGlobal->hide2.nofWord; loop++)
                {
                    if (StrStr(buffer, data[loop]) != NULL)
                    {
                        // キーワード発見！
                        MemHandleUnlock(NNshGlobal->hide2.wordmemH);
                        goto REPLACE_MESSAGE;
                    }
                }
                MemHandleUnlock(NNshGlobal->hide2.wordmemH);
            }
            else
            {
                // NGキーワードを複数指定しない場合
                if (StrStr(buffer, (NNshGlobal->NNsiParam)->hideWord2) != NULL)
                {
                    goto REPLACE_MESSAGE;
                }
            }
        }
    }

    // NG確認-3を実施する場合、、、DBから呼び出して全レコードの文字列をチェック
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
    {
        // NGワード格納用DBの取得
        OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
        if (dbRef == 0)
        {
            // ログDBのオープン失敗、終了する
            return;
        }

        // レコード件数取得
        GetDBCount_NNsh(dbRef, &maxCnt);

        // (全件数分)データを読み出して、NGキーワードチェック
        for (loop = 0; loop < maxCnt; loop++)
        {
            GetRecordReadOnly_NNsh(dbRef, loop, &dbH, (void **) &dbData);

            // NGキーワードが一致するかチェック
            checkWord = StrStr(buffer, dbData->ngWord);
            if (checkWord != NULL)
            {
                // キーワードが「全体」チェックだった場合、、、
                if (dbData->checkArea == NNSH_NGCHECK_ALLAREA)
                {
                    // NGキーワードが "置換"指定されているか？
                    if (dbData->matchedAction == 0)
                    {
                        // NGキーワードが一致した、表示しない
                        ReleaseRecordReadOnly_NNsh(dbRef, dbH);
                        CloseDatabase_NNsh(dbRef);
                        goto REPLACE_MESSAGE;
                    }
                    // 文字列だけを表示しない
                    replaceHideWord(buffer, dbData->ngWord);
                    goto NEXT_RECORD;
                }

                area1 = StrStr(buffer, "\n");
                if (area1 != NULL)
                {
                    area2 = StrStr(area1, "\n");
                }
                else
                {
                    area2 = NULL;
                }

                // NGキーワードのチェック枠が指定されていた場合...
                matchedWord = 0;
                switch (dbData->checkArea)
                {
                  case NNSH_NGCHECK_NAME_MAIL:
                    // Name欄とMail欄に含まれているかチェック
                    if (checkWord < area1)
                    {
                        matchedWord = 1;
                    }
                    break;

                  case NNSH_NGCHECK_NOTMESSAGE:
                    // メッセージ欄以外に含まれているかチェック
                    if (checkWord < area2)
                    {
                        matchedWord = 1;
                    }
                    break;

                  case NNSH_NGCHECK_MESSAGE:
                    // メッセージ欄に含まれているかチェック
                    if (area2 != NULL)
                    {
                        if (StrStr(area2, dbData->ngWord) != NULL)
                        {
                            // 本文にNGワードが存在した
                            matchedWord = 1;
                        }
                    }
                    break;

                  default:
                    // デフォはとりあえずマッチ
                    matchedWord = 1;
                    break;
                }
                if (matchedWord != 0)
                {
                    // NGキーワードが一致した、表示しない
                    if (dbData->matchedAction == 0)
                    {
                        ReleaseRecordReadOnly_NNsh(dbRef, dbH);
                        CloseDatabase_NNsh(dbRef);
                        goto REPLACE_MESSAGE;
                    }
                    // 文字列だけを表示しない
                    replaceHideWord(buffer, dbData->ngWord);
                }
            }
NEXT_RECORD:
            ReleaseRecordReadOnly_NNsh(dbRef, dbH);
        }
        // 見つからなかった、DBをクローズする
        CloseDatabase_NNsh(dbRef);
    }
    return;

REPLACE_MESSAGE:
    ptr = StrStr(buffer, " ");
    if (ptr == NULL)
    {
        ptr = buffer;
    }
    else
    {
        ptr++;
    }
    // メッセージを置き換える
    StrCopy(ptr, "* * *\n* * *\n");
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : setFavoriteLabel                                           */
/*                                            お気に入り設定のラベルを設定 */
/*-------------------------------------------------------------------------*/
static void setFavoriteLabel(FormType *frm, UInt16 attribute)
{
    ControlType *selP;

    // お気に入り設定のセレクタトリガオブジェクトを取得
    selP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SELTRID_FAVOR_THREAD));
    if (selP == NULL)
    {
        // オブジェクトの取得失敗
        return;
    }

    switch (attribute & NNSH_MSGATTR_FAVOR)
    {
      case NNSH_MSGATTR_FAVOR_L1:
        // お気に入りレベル(低)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR_L1);
        break;

      case NNSH_MSGATTR_FAVOR_L2:
        // お気に入りレベル(中低)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR_L2);
        break;

      case NNSH_MSGATTR_FAVOR_L3:
        // お気に入りレベル(中)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR_L3);
        break;

      case NNSH_MSGATTR_FAVOR_L4:
        // お気に入りレベル(中高)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR_L4);
        break;

      case NNSH_MSGATTR_FAVOR:
        // お気に入りレベル(高)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR);
        break;

      default:
        // お気に入りではない
        CtlSetLabel(selP, NNSH_ATTRLABEL_NOTFAVOR);
        break;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : outputMemoPad                                              */
/*                                                    メモ帳へファイル出力 */
/*-------------------------------------------------------------------------*/
static void outputMemoPad(Boolean useClipboard)
{
    UInt16            len, lp, limit;
    MemHandle         txtH;
    FormType         *frm;
    FieldType        *fldP;
    Char             *buf, *ptr, *txtP;
    NNshMessageIndex *idxP;

    // メッセージインデックス
    idxP = NNshGlobal->msgIndex;

    // コピー用バッファ領域を確保する
    buf = MEMALLOC_PTR(NNSH_LIMIT_MEMOPAD);
    if (buf == NULL)
    {
        // メモ帳出力をキャンセルする
        NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                           " size:", NNSH_LIMIT_MEMOPAD);
        return;
    }

    // メモの先頭にスレタイトルを出力する
    StrCopy(buf, idxP->threadTitle);
    StrCat (buf, "\n---\n");

    // メモ帳データを書き込む領域をタイトルの後ろに設定する
    ptr = buf + StrLen(buf);
    len = NNSH_LIMIT_MEMOPAD - StrLen(buf);

    // クリップボードにある文字列をメモ帳に出力する？
    if (useClipboard == true)
    {
        txtH = ClipboardGetItem(clipboardText, &lp);
        if ((lp != 0)&&(txtH != 0))
        {
            txtP = MemHandleLock(txtH);
            if (txtP == NULL)
            {
                // ポインタが取得できない場合には、終了する
                goto FUNC_END;
            }
            MemHandleUnlock(txtH);
            
            // 出力文字列長の調整
            if (lp > len)
            {
                lp = len - 1;
            }

            // クリップボードの内容を出力用バッファへコピーする
            StrNCopy(ptr, txtP, lp);
            ptr[lp] = 0;

            // スレデータをメモ帳へ吐き出す
            SendToLocalMemopad(idxP->threadTitle, buf);
       }
        goto FUNC_END;
    }

    // フィールドデータをバッファにコピーする
    frm  = FrmGetActiveForm();
    fldP = (FieldType *) FrmGetObjectPtr(frm, 
                                         FrmGetObjectIndex(frm,FLDID_MESSAGE));
    txtH = FldGetTextHandle(fldP);
    if (txtH == 0)
    {
        // Fieldが確保できなかった場合には、終了する
        goto FUNC_END;
    }
    limit = FldGetTextAllocatedSize(fldP);

    txtP = MemHandleLock(txtH);
    if (txtP == NULL)
    {
        // ポインタが取得できない場合には、終了する
        goto FUNC_END;
    }

    // 文字をメモ帳送信用バッファに転送する
    for (lp = 0; lp < len; lp++)
    {
        if (lp >= limit)
        {
            // フィールドサイズの最大値に到達した、終了する
            *ptr = 0;
            break;
        }
        if (*txtP == '\0')
        {
            // 文字列の末尾、breakする
            *ptr = *txtP;
            break;
        }
        if (*txtP == NNSH_CHARCOLOR_ESCAPE)
        {
            // カラー用制御文字は読み飛ばす
            txtP++;
        }
        else
        {
            // 通常の１文字コピー
            *ptr = *txtP;
            ptr++;
        }
        txtP++;
    }
    MemHandleUnlock(txtH);

    // スレデータをメモ帳へ吐き出す
    SendToLocalMemopad(idxP->threadTitle, buf);

FUNC_END:
    // バッファ領域を開放する
    MEMFREE_PTR(buf);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : outputToFile                                               */
/*                                        レスを指定したファイルへ出力する */
/*-------------------------------------------------------------------------*/
static void outputToFile(Char *fileName, UInt16 fileMode, Boolean isQuote, UInt16 startQuoteLine, Char *appendLine)
{
    UInt32            writeSize;
    UInt16            len, lp, lineCount;
    MemHandle         txtH;
    FormType         *frm;
    FieldType        *fldP;
    Char             *buf, *ptr, *txtP, *startPtr;
    Err               ret;
    NNshFileRef       fileRef;

    // コピー用バッファ領域を確保する
    buf = MEMALLOC_PTR(NNSH_LIMIT_MEMOPAD + MARGIN);
    if (buf == NULL)
    {
        // ファイル出力をキャンセルする
        NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                           " size:", (NNSH_LIMIT_MEMOPAD + MARGIN));
        return;
    }
    MemSet(buf, (NNSH_LIMIT_MEMOPAD + MARGIN), 0x00);
    ptr = buf;
    len = NNSH_LIMIT_MEMOPAD - MARGIN;

    // フィールドデータをバッファにコピーする
    frm  = FrmGetActiveForm();
    fldP = (FieldType *) FrmGetObjectPtr(frm, 
                                         FrmGetObjectIndex(frm,FLDID_MESSAGE));
    txtH = FldGetTextHandle(fldP);
    if (txtH == 0)
    {
        // Fieldが確保できなかった場合には、終了する
        goto FUNC_END;
    }
    txtP = MemHandleLock(txtH);
    if (txtP == NULL)
    {
        // ポインタが取得できない場合には、終了する
        goto FUNC_END;
    }

    // ファイル出力用のデータを作成する
    startPtr = buf;
    lineCount = 0;
    lp = 0;
    if (isQuote == true)
    {
        *ptr = '>';
        ptr++;
        lp++;
        *ptr = ' ';
        ptr++;
        lp++;
    }    
    while (lp < len)
    {
        if (*txtP == '\0')
        {
            // 文字列の末尾、breakする
            *ptr = '\n';
            ptr++;
            *ptr = *txtP;
            break;
        }
        if (*txtP == NNSH_CHARCOLOR_ESCAPE)
        {
            // カラー用制御文字は読み飛ばす
            txtP++;
        }
        else
        {
            // 通常の１文字コピー
            *ptr = *txtP;
            ptr++;
            lp++;
        }
        if ((*txtP == 0x0a)&&(isQuote == true))
        {
            // 引用符をつける設定だった場合...
            lineCount++;
            if (lineCount == startQuoteLine)
            {
                // 先頭の場所を進める
                StrCopy(ptr, appendLine);
                startPtr = ptr;
                ptr = ptr + StrLen(appendLine);
            }
            *ptr = '>';
            ptr++;
            lp++;
            *ptr = ' ';
            ptr++;
            lp++;
        }
        txtP++;
    }
    MemHandleUnlock(txtH);

    // 取得したバッファをファイルに書き込む
    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
        goto FUNC_END;
    }
    ret = AppendFile_NNsh(&fileRef, StrLen(startPtr), startPtr, &writeSize);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", ret);
    }
    ret = CloseFile_NNsh (&fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "CloseFile() ", " ret:", ret);
    }
    else
    {
        NNsh_DebugMessage(ALTID_INFO, "Save Message", " bytes:", writeSize);
    }

FUNC_END:
    // バッファ領域を開放する
    MEMFREE_PTR(buf);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openWebBrowser                                             */
/*                                                       Webブラウザで開く */
/*-------------------------------------------------------------------------*/
static void openWebBrowser(UInt16 confirmation, Char *buffer, UInt16 size)
{
    MemHandle  memH;
    UInt16     len, savedParam;
    Char      *urlP, *ptr, *dest;

    // クリップボードにあるデータを取得する
    MemSet(buffer, size, 0x00);
    memH = ClipboardGetItem(clipboardText, &len);
    if ((len != 0)&&(memH != 0))
    {
        urlP = MemHandleLock(memH);
        if (urlP != NULL)
        {
            ptr = StrStr(urlP, "tp://");
            if (ptr != NULL)
            {
                // URLを取得("http://"だけど)
                ptr = ptr + StrLen("tp://");
                StrCopy(buffer, "http://");
                dest = &buffer[StrLen(buffer)];

                // クリップボードからURLの文字列(ASCII)だけを抽出する
                while ((dest < buffer + size)&&
                        (*ptr > ' ')&&(*ptr < 0x7f)&&
                        (*ptr != '<')&&(*ptr != '>')&&(*ptr != '`')&&
                        (*ptr != '{')&&(*ptr != '}')&&(*ptr != '^')&&
                        (*ptr != '|')&&(*ptr != '"')&&                        
                        (ptr  < urlP + len))
                {
                    *dest = *ptr;
                    dest++;
                    ptr++;
                }
            }
            MemHandleUnlock(memH);                
        }
    }
    if (buffer[0] != 0x00)
    {
        // NetFrontを指定されたURLで起動するか確認する
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = confirmation;
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, buffer,
                                MSG_WEBLAUNCH_CONFIRM, 0) == 0)
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

            // NetFrontを起動する(NNsi終了後に起動)
            (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                     0, 0, 
                                     NNshGlobal->browserLaunchCode,
                                     buffer, StrLen(buffer), NULL);
        }
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : jumpPrevious                                               */
/*                                                １つ前のメッセージへ戻る */
/*-------------------------------------------------------------------------*/
static void jumpPrevious(void)
{
    NNshMessageIndex *idxP;

    // メッセージインデックス
    idxP = NNshGlobal->msgIndex;
 
    // 1回でもジャンプしていれば戻る
    if (idxP->prevMsg[idxP->histIdx] != 0)
    {
        idxP->openMsg = idxP->prevMsg[idxP->histIdx];
        idxP->histIdx = (idxP->histIdx != 0) ?
                                       (idxP->histIdx - 1) : (PREVBUF - 1);
    }
    (void) displayMessage(NNshGlobal->msgIndex, 0);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : jumpEdge                                                   */
/*                                             メッセージの移動(先頭/末尾) */
/*-------------------------------------------------------------------------*/
static void jumpEdge(Boolean loc)
{
    NNshMessageIndex *idxP;

    // メッセージインデックス
    idxP = NNshGlobal->msgIndex;

    idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ? (idxP->histIdx + 1) : 0;
    idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
    if (loc == false)
    {
        // スレの先頭へ
        idxP->openMsg    = 1;
    }
    else
    {
        // スレの末尾へ
        idxP->openMsg    = idxP->nofMsg;
    }
    (void) displayMessage(NNshGlobal->msgIndex, 0);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : parseMessage                                               */
/*                                                  (メッセージの整形処理) */
/*-------------------------------------------------------------------------*/
static void parseMessage(Char *buf, Char *source, UInt32 size,
                         UInt16 *nofJmp, UInt16 *jmpBuf, UInt16 kanjiCode,
                         Boolean setJumpFlag)
{
    Boolean kanjiMode;
    UInt32 dataStatus, jmp;
    UInt16 len, fontFlag, parseLen; 
    Char *ptr, *dst, *num, *tblP;

#ifdef USE_COLOR
    if (((NNshGlobal->NNsiParam)->useColor != 0)&&
        ((NNshGlobal->NNsiParam)->useColorMessageToken != 0)&&
        ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0))
    {
        fontFlag = NNSH_ENABLE;
    }
    else
#endif  //#ifdef USE_COLOR
    {
        fontFlag = NNSH_DISABLE;
    }

    dataStatus = MSGSTATUS_NAME;
    kanjiMode = false;

    // 超遅い解釈ルーチンかも... (１文字づつパースする)
    dst  = buf;
    ptr  = source;
    parseLen = 0;

    // ヘッダ色にする
    if (fontFlag != NNSH_DISABLE)
    {
        *dst++ = NNSH_CHARCOLOR_ESCAPE;
        *dst++ = NNSH_CHARCOLOR_HEADER;
    }
    while (ptr < (source + size))
    {
        // JIS漢字コードの変換ロジック
        if (kanjiCode == NNSH_KANJICODE_JIS)
        {
            // JIS→SHIFT JISのコード変換
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x24')&&(*(ptr + 2) == '\x42'))
            {
                // 漢字モードに切り替え
                kanjiMode = true;
                ptr = ptr + 3;
                continue;
            }
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x28')&&(*(ptr + 2) == '\x42'))
            {
                // ANKモードに切り替え
                kanjiMode = false;
                ptr = ptr + 3;
                continue;
            }
            if (kanjiMode == true)
            {
                // JIS > SJIS 漢字変換
                ConvertJIStoSJ(dst, ptr);
                ptr = ptr + 2;
                dst = dst + 2;
                continue;
            }
        }
        if (*ptr == '&')
        {
            // "&gt;" を '>' に置換
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                if ((setJumpFlag == true) && (*ptr <= '9')&&(*ptr >= '0'))
                {
                    // スレ番を取得
                    num = ptr;
                    jmp = 0;
                    while ((*num <= '9')&&(*num >= '0'))
                    {
                        jmp = jmp * 10 + (*num - '0');
                        num++;
                    }
                    if ((*nofJmp < JUMPBUF)&&(jmp != 0))
                    {
                        // スレ番をバッファに追加
                        jmpBuf[*nofJmp] = jmp;
                        (*nofJmp)++;
                    }
                }
                continue;
            }
            // "&lt;" を '<' に置換
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '<';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&quot;" を '"' に置換
            if ((*(ptr + 1) == 'q')&&(*(ptr + 2) == 'u')&&
                (*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&(*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" を '    ' に置換
            if ((*(ptr + 1) == 'n')&&(*(ptr + 2) == 'b')&&
                (*(ptr + 3) == 's')&&(*(ptr + 4) == 'p')&&(*(ptr + 5) == ';'))
            {
                *dst++ = ' ';
/*
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
*/
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&amp;" を '&' に置換
            if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
            {
                // ギコのしっぽ変換対策... (ここから)
                if ((*(ptr + 5) == 'g')&&(*(ptr + 6) == 't')&&(*(ptr + 7) == ';'))
                {
                    *dst++ = '>';
                    ptr = ptr + 8;    // StrLen(TAG_AMP)+ 'g' + 't' + ';';
                    if ((setJumpFlag == true) && (*ptr <= '9')&&(*ptr >= '0'))
                    {
                        // スレ番を取得
                        num = ptr;
                        jmp = 0;
                        while ((jmp < NNSH_MESSAGE_LIMIT)&&
                               (*num <= '9')&&(*num >= '0'))
                        {
                            jmp = jmp * 10 + (*num - '0');
                            num++;
                        }
                        if ((*nofJmp < JUMPBUF)&&(jmp != 0))
                        {
                            // スレ番をバッファに追加
                            jmpBuf[*nofJmp] = jmp;
                            (*nofJmp)++;
                        }
                    }
                    continue;
                }
                // ギコのしっぽ変換対策... (ここまで)

                *dst++ = '&';
                ptr = ptr + 5;    // StrLen(TAG_AMP);
                continue;
            }

            if (*(ptr + 1) == 'a')
            {
/*
                // "&amp;" を '&' に置換
                if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                    (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
                {
                    *dst++ = '&';
                    ptr = ptr + 5;    // StrLen(TAG_AMP);
                    continue;
                }
*/
                // "&acute;" を '´' に置換
                if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'c')&&
                    (*(ptr + 3) == 'u')&&(*(ptr + 4) == 't')&&
                    (*(ptr + 5) == 'e')&&(*(ptr + 6) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0x4C;
                    ptr = ptr + 7;    // StrLen(´);
                    continue;
                }
                // "&alpha;" を 'α' に置換
                if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'l')&&
                    (*(ptr + 3) == 'p')&&(*(ptr + 4) == 'h')&&
                    (*(ptr + 5) == 'a')&&(*(ptr + 6) == ';'))
                {
                    *dst++ = 0x83;
                    *dst++ = 0xBF;
                    ptr = ptr + 7;    // StrLen(TAG_ALPHA);
                    continue;
                }
                // 変換できなかった箇所。
                *dst++ = '&';
                ptr++;
                *dst++ = 'a';
                ptr++;
                continue;
            }
            // "&beta;" を 'β' に置換
            if ((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'e')&&
                (*(ptr + 3) == 't')&&(*(ptr + 4) == 'a')&&
                (*(ptr + 5) == ';'))
            {
                *dst++ = 0x83;
                *dst++ = 0xC0;
                ptr = ptr + 6;    // StrLen(TAG_BETA);
                continue;
            }
            // "&clubs;" を '※' に置換 (FONTないので...)
            if ((*(ptr + 1) == 'c')&&(*(ptr + 2) == 'l')&&
                (*(ptr + 3) == 'u')&&(*(ptr + 4) == 'b')&&
                (*(ptr + 5) == 's')&&(*(ptr + 6) == ';'))
            {
                *dst++ = 0x81;
                *dst++ = 0xA6;
                ptr = ptr + 7;    // StrLen(TAG_CLUBS);
                continue;
            }
            
            // "&d"
            if (*(ptr + 1) == 'd')
            {
                // "&diams;" を '◆' に置換
                if ((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'i')&&
                    (*(ptr + 3) == 'a')&&(*(ptr + 4) == 'm')&&
                    (*(ptr + 5) == 's')&&(*(ptr + 6) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0x9F;
                    ptr = ptr + 7;    // StrLen(TAG_DIAMS);
                    continue;
                }
                // "&darr;" を '↓' に置換
                if ((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xAB;
                    ptr = ptr + 6;    // StrLen(TAG_DIAMS);
                    continue;
                }
                // "&dArr;" を '↓' に置換
                if ((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xAB;
                    ptr = ptr + 6;    // StrLen(TAG_DIAMS);
                    continue;
                }

                // 変換できなかった箇所。
                *dst++ = '&';
                ptr++;
                *dst++ = 'd';
                ptr++;
                continue;
            }            
            
            if (*(ptr + 1) == 'h')
            {
                // "&hellip;" を '･･･' に置換
                if ((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'e')&&
                    (*(ptr + 3) == 'l')&&(*(ptr + 4) == 'l')&&
                    (*(ptr + 5) == 'i')&&(*(ptr + 6) == 'p')&&
                    (*(ptr + 7) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0x63;
                    ptr = ptr + 8;    // StrLen(TAG_...)
                    continue;
                }
                if ((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xCC;
                    ptr = ptr + 6;    // StrLen(TAG_...)
                    continue;
                }
                if ((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xCC;
                    ptr = ptr + 6;    // StrLen(TAG_...)
                    continue;
                }
                if ((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'e')&&
                    (*(ptr + 3) == 'a')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == 't')&&(*(ptr + 6) == 's')&&
                    (*(ptr + 7) == ';'))
                {
                    // 本当はハートだが☆にしてみた。
                    *dst++ = 0x81;
                    *dst++ = 0x99;
                    ptr = ptr + 8;    // StrLen(TAG_...)
                    continue;
                }

                // 変換できなかった箇所。
                *dst++ = '&';
                ptr++;
                *dst++ = 'h';
                ptr++;
                continue;
            }
            // "&r"
            if (*(ptr + 1) == 'r')
            {
                // "&rarr;" を '→' に置換
                if ((*(ptr + 1) == 'r')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xA8;
                    ptr = ptr + 6;    // StrLen(→);
                    continue;
                }
                 // "&rdquo;" を '”' に置換
                if ((*(ptr + 1) == 'r')&&(*(ptr + 2) == 'd')&&
                    (*(ptr + 3) == 'q')&&(*(ptr + 4) == 'u')&&
                    (*(ptr + 5) == 'o')&&(*(ptr + 6) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0x68;
                    ptr = ptr + 7;    // StrLen(´);
                    continue;
                }
                // "&rArr;" を '⇒' に置換
                if ((*(ptr + 1) == 'r')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xCB;
                    ptr = ptr + 6;    // StrLen(→);
                    continue;
                }
                // 変換できなかった箇所。
                *dst++ = '&';
                ptr++;
                *dst++ = 'd';
                ptr++;
                continue;
            }
            
            if ((*(ptr + 1) == 's')&&(*(ptr + 2) == 'p')&&
                 (*(ptr + 3) == 'a')&&(*(ptr + 4) == 'd')&&
                 (*(ptr + 5) == 'e')&&(*(ptr + 6) == 's')&&
                 (*(ptr + 7) == ';'))
            {
                // 本当はスペードだが※にしてみた。
                *dst++ = 0x81;
                *dst++ = 0xA6;
                ptr = ptr + 8;    // StrLen(TAG_...)
                continue;
            }

             // "&omega;" を 'ω' に置換
            if ((*(ptr + 1) == 'o')&&(*(ptr + 2) == 'm')&&
                (*(ptr + 3) == 'e')&&(*(ptr + 4) == 'g')&&
                (*(ptr + 5) == 'a')&&(*(ptr + 6) == ';'))
            {
                *dst++ = 0x83;
                *dst++ = 0xD6;
                ptr = ptr + 7;    // StrLen(´);
                continue;
            }

            // "&plusmn;" を ± へ変換
            if ((*(ptr + 1) == 'p')&&(*(ptr + 2) == 'l')&&
                 (*(ptr + 3) == 'u')&&(*(ptr + 4) == 's')&&
                 (*(ptr + 5) == 'm')&&(*(ptr + 6) == 'n')&&
                 (*(ptr + 7) == ';'))
            {
                // "±"
                *dst++ = 0x81;
                *dst++ = 0x7D;
                ptr = ptr + 8;    // StrLen(TAG_...)
                continue;
            }

            // "&l"
            if (*(ptr + 1) == 'l')
            {
                // "&larr;" を '←' に置換
                if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xA9;
                    ptr = ptr + 6;    // StrLen(TAG_DIAMS);
                    continue;
                }
                // "&lArr;" を '←' に置換
                if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xA9;
                    ptr = ptr + 6;    // StrLen(TAG_DIAMS);
                    continue;
                }

                // 変換できなかった箇所。
                *dst++ = '&';
                ptr++;
                *dst++ = 'l';
                ptr++;
                continue;
            }            

            // "&u"
            if (*(ptr + 1) == 'u')
            {
                // "&uarr;" を '↑' に置換
                if ((*(ptr + 1) == 'u')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xAA;
                    ptr = ptr + 7;    // StrLen(TAG_DIAMS);
                    continue;
                }
                // "&uArr;" を '↑' に置換
                if ((*(ptr + 1) == 'u')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xAA;
                    ptr = ptr + 7;    // StrLen(TAG_DIAMS);
                    continue;
                }

                // 変換できなかった箇所。
                *dst++ = '&';
                ptr++;
                *dst++ = 'u';
                ptr++;
                continue;
            }            

            // "&yen;" を '￥' に置換
            if ((*(ptr + 1) == 'y')&&(*(ptr + 2) == 'e')&&
                (*(ptr + 3) == 'n')&&(*(ptr + 4) == ';'))
            {
                *dst++ = 0x81;
                *dst++ = 0x8F;
                ptr = ptr + 5;    // StrLen(´);
                continue;
            }
             // "&Pi;" を 'Π' に置換
            if ((*(ptr + 1) == 'P')&&(*(ptr + 2) == 'i')&&
                (*(ptr + 3) == ';'))
            {
                *dst++ = 0x83;
                *dst++ = 0xAE;
                ptr = ptr + 4;    // StrLen(Π);
                continue;
            }

            // これはありえないはずだが...一応。
            *dst++ = '&';
            ptr++;
            continue;
        }
        if (*ptr == '<')
        {
            //  "<>" は、セパレータ(状態によって変わる)
            if (*(ptr + 1) == '>')
            {
                ptr = ptr + 2;   // StrLen(DATA_SEPARATOR);
                switch (dataStatus)
                {
                  case MSGSTATUS_NAME:
                    // 名前欄の区切り
                    if (((NNshGlobal->NNsiParam)->useNameOneLine != 0)&&(parseLen >= NNSH_MAX_OMIT_LENGTH))
                    {
                        *dst++ = chrEllipsis;
                        *dst++ = ' ';
                        *dst++ = ' ';
                    }
                    else
                    {
                        *dst++ = ' ';
                        *dst++ = ' ';
                        *dst++ = ' ';
                    }
                    dataStatus = MSGSTATUS_EMAIL;
                    parseLen = 0;

                    // ヘッダ色にする
                    if (fontFlag != NNSH_DISABLE)
                    {
                        *dst++ = NNSH_CHARCOLOR_ESCAPE;
                        *dst++ = NNSH_CHARCOLOR_BOLDHEADER;
                    }
                    break;

                  case MSGSTATUS_EMAIL:
                    // e-mail欄の区切り
                    if ((NNshGlobal->NNsiParam)->useNameOneLine != 0)
                    {
                        if (parseLen >= NNSH_MAX_OMIT_LENGTH)
                        {
                            *dst++ = chrEllipsis;
                        }
                    }
                    else
                    {
                        *dst++ = '\n';
                    }
                    dataStatus = MSGSTATUS_DATE;
                    parseLen = 0;

                    // ヘッダ色にする
                    if (fontFlag != NNSH_DISABLE)
                    {
                        *dst++ = NNSH_CHARCOLOR_ESCAPE;
                        *dst++ = NNSH_CHARCOLOR_INFORM;
                    }
                    break;

                  case MSGSTATUS_DATE:
                    // 年月日・時間およびＩＤ欄の区切り
                    if ((NNshGlobal->NNsiParam)->useNameOneLine != 0)
                    {
                        *dst++ = '\n';
                    }
                    else
                    {
                        *dst++ = '\n';
                        *dst++ = '\n';
                    }
                    dataStatus = MSGSTATUS_NORMAL;
                    if (*ptr == ' ')
                    {
                        // スペースは読み飛ばす
                        ptr++;
                    }
                    // 通常色にする
                    if (fontFlag != NNSH_DISABLE)
                    {
                        *dst++ = NNSH_CHARCOLOR_ESCAPE;
                        *dst++ = NNSH_CHARCOLOR_NORMAL;
                    }
                    break;

                  case MSGSTATUS_NORMAL:
                  default:
                    // メッセージの終端
                    *dst++ = ' ';
                    *dst++ = '\0';
                    return;
                    break;
                }
                continue;
            }
            //  "<br>" は、改行に置換
            if (((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                // 行末と行頭のスペースを削ってみる場合
                if ((ptr > source)&&(*(ptr - 1) == ' '))
                {
                    dst--;
                }
                if (*(ptr + 4) == ' ')
                {
                    *dst++ = '\n';
                    ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                    continue;
                }
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<p*>" は、改行2つに置換
            if ((*(ptr + 1) == 'p')||(*(ptr + 1) == 'P'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 2;
                while (*ptr != '>')
                {
                    ptr++;
                    if (ptr >= (source + size))
                    {
                        // 確保した領域を越えた...
                        break;
                    }
                }
                ptr++;
                continue;
            }

            // <li>タグを改行コードと:に置換する
            if (((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'i')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'L')&&(*(ptr + 2) == 'I')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '･';
                ptr = ptr + 4;
                continue;
            }   

            //  "<hr>" は、改行 === 改行 に置換
            if (((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'H')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dt>"は、改行に置換
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "</tr>" および "</td>" は、改行に置換
            if (((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'r')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'R')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'd')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'D')&&(*(ptr + 4) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dd>" は、改行と空白４つに置換
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'd')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'D')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<h?" は、改行 + 改行 に置換
            if ((*(ptr + 1) == 'h')&&(*(ptr + 1) == 'H'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                // 強調色にする
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_HEADER;
                }
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "</h?>" は、改行 + 改行 に置換
            if (((*(ptr + 2) == 'h')||(*(ptr + 2) == 'H'))&&(*(ptr + 1) == '/'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                // 通常色にする
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            // <a* はアンカー色へ
            if (((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&(*(ptr + 2) == ' '))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_ANCHOR;
                }
                goto SKIP_TAG;
            }

            // </a* は通常色へ
            if (((*(ptr + 2) == 'a')||(*(ptr + 2) == 'A'))&&(*(ptr + 1) == '/'))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                goto SKIP_TAG;
            }

            // <EM* は強調色へ
            if (((*(ptr + 1) == 'e')||(*(ptr + 1) == 'E'))&&
                ((*(ptr + 2) == 'm')||(*(ptr + 2) == 'M')))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_BOLDDATA;
                }
                goto SKIP_TAG;
            }

            // </em* は通常色へ
            if (((*(ptr + 2) == 'e')||(*(ptr + 2) == 'E'))&&
                ((*(ptr + 3) == 'm')||(*(ptr + 3) == 'M'))&&
                (*(ptr + 1) == '/'))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                goto SKIP_TAG;
            }

            // <tr>, <td> はスペース１つに変換
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R')||
                 (*(ptr + 2) == 'd')||(*(ptr + 2) == 'D')))
            {
                *dst++ = ' ';
                goto SKIP_TAG;
            }

            //  "<SCRIPT>～</SCRIPT>" は表示しない(読み飛ばす)
            if ((*(ptr + 1) == 'S')&&(*(ptr + 2) == 'C')&&
                (*(ptr + 3) == 'R')&&(*(ptr + 4) == 'I')&&
                (*(ptr + 5) == 'P')&&(*(ptr + 6) == 'T'))
            {
                ptr = ptr + 7;
                tblP = ptr; 
                while (*ptr != '\0')
                {
                    if ((*(ptr + 0) == '<')&&(*(ptr + 1) == '/')&&
                        (*(ptr + 2) == 'S')&&(*(ptr + 3) == 'C')&&
                        (*(ptr + 4) == 'R')&&(*(ptr + 5) == 'I')&&
                        (*(ptr + 6) == 'P')&&(*(ptr + 7) == 'T'))
                    {
                        // スクリプトタグの末尾を発見、タグ読み飛ばしモードへ
                        goto SKIP_TAG;
                    }
                    ptr++;
                }
                // </SCRIPT>の末尾が見つからなかった、、、タグ位置を戻す..
                ptr = tblP;                
                goto SKIP_TAG;
            }

            //  "<script>～</script>" は表示しない(読み飛ばす)
            if ((*(ptr + 1) == 's')&&(*(ptr + 2) == 'c')&&
                (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'i')&&
                (*(ptr + 5) == 'p')&&(*(ptr + 6) == 't'))
            {
                ptr = ptr + 7;
                tblP = ptr; 
                while (*ptr != '\0')
                {
                    if ((*(ptr + 0) == '<')&&(*(ptr + 1) == '/')&&
                        (*(ptr + 2) == 's')&&(*(ptr + 3) == 'c')&&
                        (*(ptr + 4) == 'r')&&(*(ptr + 5) == 'i')&&
                        (*(ptr + 6) == 'p')&&(*(ptr + 7) == 't'))
                    {
                        // スクリプトタグの末尾を発見、タグ読み飛ばしモードへ
                        goto SKIP_TAG;
                    }
                    ptr++;
                }
                // </script>の末尾が見つからなかった、、、タグ位置を戻す..
                ptr = tblP;
                goto SKIP_TAG;
            }

            // <BE: は、BE IDのため読み飛ばさない。(画面に表示する)
            if ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'E')&&
                (*(ptr + 3) == ':'))
            {
                *dst++ = ' ';
                ptr++;
                while ((*ptr != '>')&&(*ptr != '\0'))
                {
                    *dst++ = *ptr;
                    ptr++;                    
                }
                ptr++;
                continue;
            }

SKIP_TAG:
            // その他のタグは読み飛ばす
#ifdef USE_STRSTR
            ptr = StrStr(ptr, ">");
#else
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
#endif
            ptr++;
            // *dst++ = ' ';   // タグは完全無視とする。
            continue;
        }
        // 漢字コードがEUCだった場合...
        if (kanjiCode == NNSH_KANJICODE_EUC)
        {
            if (ConvertEUCtoSJ((UInt8 *) dst, (UInt8 *) ptr, &len) == true)
            {
                // EUC > SHIFT JIS変換を実行した
                dst = dst + len;
                ptr = ptr + 2;
                continue;
            }
            if (*ptr != '\0')
            {
                // 普通の一文字転写
                *dst++ = *ptr;
            }
            ptr++;
            continue;
        }
        if ((NNshGlobal->NNsiParam)->convertHanZen != 0)
        {
            if (((UInt8) *ptr >= 0x81)&&((UInt8) *ptr <= 0x9f))
            {
                // 2バイト文字と判定
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }
            if (((UInt8) *ptr >= 0xe0)&&((UInt8) *ptr <= 0xef))
            {
                // 2バイト文字と判定
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }
            // 半角カナ→全角変換
            if (ConvertHanZen((UInt8 *) dst, (UInt8 *) ptr) == true)
            {
                dst = dst + 2;
                ptr++;
                continue;
            }
        }

        // スペースが連続していた場合、１つに減らす
        if ((*ptr == ' ')&&(*(ptr + 1) == ' '))
        {
            ptr++;
            while ((*ptr == ' ')&&(ptr < (source + size)))
            {
                ptr++;
            }
            ptr--;
        }

        // NULL および 0x0a, 0x0d, 0x09(タブ) は無視する
        if ((*ptr != '\0')&&(*ptr != 0x0a)&&(*ptr != 0x0d)&&(*ptr != 0x09))
        {
            switch (dataStatus)
            {
              case MSGSTATUS_NAME:
                if (((NNshGlobal->NNsiParam)->useNameOneLine == 0)||
                    (((NNshGlobal->NNsiParam)->useNameOneLine != 0)&&(parseLen <= NNSH_MAX_OMIT_LENGTH)))
                {
                    // 一文字転写
                    *dst++ = *ptr;
                }
                parseLen++;
                break;

              case MSGSTATUS_EMAIL:
                if (((NNshGlobal->NNsiParam)->useNameOneLine == 0)||
                    (((NNshGlobal->NNsiParam)->useNameOneLine != 0)&&(parseLen <= NNSH_MAX_OMIT_LENGTH)))
                {
                    // 一文字転写
                    *dst++ = *ptr;
                }
                parseLen++;
                break;

              case MSGSTATUS_DATE:
                if ((NNshGlobal->NNsiParam)->useNameOneLine != 0)
                {
                    // 転写しない...
                }
                else
                {
                    // 一文字転写
                    *dst++ = *ptr;
                }
                break;

              case MSGSTATUS_NORMAL:
              default:
                // 普通の一文字転写
                *dst++ = *ptr;
                break;
            }
        }
        ptr++;
    }
    *dst++ = '\0';
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : changeFont                                                 */
/*                                                          フォントの変更 */
/*-------------------------------------------------------------------------*/
static void changeFont(void)
{
    FormType  *frm;

#ifdef USE_TSPATCH
    UInt32 fontId, ver;
    Err    ret;
#endif

    frm = FrmGetActiveForm();

    // フォントIDの選択
    if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
    {
        // SONYハイレゾフォントを変更する
        (NNshGlobal->NNsiParam)->sonyHRFont = FontSelect((NNshGlobal->NNsiParam)->sonyHRFont);

        // フィールドの描画(ハイレゾ画面)
        NNsi_HRFldDrawField(FrmGetObjectPtr(frm, 
                              FrmGetObjectIndex(frm, FLDID_MESSAGE)), 0);
        // スクロールバーの更新
        NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, SCLID_MESSAGE)));
    }
    else
    {
        // 通常フォントを変更する
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

        // フィールドの描画(通常画面)
        FldSetFont(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE)),
                                   (NNshGlobal->NNsiParam)->currentFont);

        // スクロールバーを更新して、通常の画面描画を行う
        NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);
        FrmDrawForm(frm);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : ReadMessage_Close                                          */
/*                                                      参照画面のクローズ */
/*-------------------------------------------------------------------------*/
void ReadMessage_Close(UInt16 nextFormID)
{
    NNshMessageIndex *idxP;

    // メッセージインデックス
    idxP = NNshGlobal->msgIndex;
    if (idxP != NULL)
    {
        // 「しおり設定」されていた場合、
        if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
        {
            if (NNshGlobal->bookmarkNick != NULL)
            {
                // しおり設定されている板名を記憶する
                StrCopy(NNshGlobal->bookmarkNick,
                        idxP->boardNick);
            }
            if (NNshGlobal->bookmarkFileName != NULL)
            {
                // しおり設定されているスレＩＤを記憶する
                StrCopy(NNshGlobal->bookmarkFileName,
                        idxP->threadFile);
            }
        }

        MEMFREE_PTR((NNshGlobal->msgIndex)->msgOffset);

        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(NNshGlobal->msgIndex);
    }

    // ハイレゾ用データ初期値をクリアする
    NNshGlobal->totalLine   = 0;
    NNshGlobal->nofPage     = 0;
    NNshGlobal->currentPage = 0;
    NNshGlobal->prevHRLines = 0;

#ifdef USE_COLOR
    // フォームをオープンしたときに更新した色を元に戻す
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // 色(前景色、文字色、背景色)を元に戻す
        UIColorSetTableEntry(UIObjectForeground, &NNshGlobal->fgColor);
        UIColorSetTableEntry(UIFieldText,        &NNshGlobal->txColor);
        UIColorSetTableEntry(UIFieldBackground,  &NNshGlobal->bgColor);
        UIColorSetTableEntry(UIFormFill,         &NNshGlobal->bgFormColor);
        UIColorSetTableEntry(UIDialogFill,       &NNshGlobal->bgDialogColor);
        UIColorSetTableEntry(UIAlertFill,        &NNshGlobal->bgAlertColor);
        UIColorSetTableEntry(UIObjectFill,       &NNshGlobal->bgObjectColor);

        // 前景色と背景色を設定する
        WinSetForeColor(UIColorGetTableEntryIndex(UIFieldText));
        WinSetTextColor(UIColorGetTableEntryIndex(UIFieldText));
        WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));
    }
#endif

    if (nextFormID != FRMID_MESSAGE)
    {
        NNshGlobal->tokenId = 0;  // 参照画面の区切りトークンをクリア
    }

    // スレ選択画面を開く
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(nextFormID);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : displayMessageSub                                          */
/*                                                   メッセージ表示メイン  */
/*-------------------------------------------------------------------------*/
static void displayMessageSub(UInt16 bbsType, Char *topP, UInt32 mesNum,
                              UInt32 offset,  UInt32 size, Char *buf,
                              Boolean setJumpFlag)
{
    Char        *ptr, *nameP, *htmlP;
    UInt32       msgSize;
    UInt16       kanjiCode, resNum;
#ifdef USE_ARMLET
    UInt32       armArgs[9], nofJump, lp, jmpMsg[JUMPBUF], hanzen;
    MemHandle    armH;
    void        *armP;
#endif
    NNshMessageIndex *idxP;
    idxP = NNshGlobal->msgIndex;

    // 漢字コードの設定とメッセージの整形処理
    kanjiCode = NNSH_KANJICODE_SHIFTJIS;
    switch (bbsType & NNSH_BBSTYPE_MASK)
    {
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        kanjiCode = NNSH_KANJICODE_EUC;
        ptr = StrStr((topP + offset), NNSH_JBBSTOKEN_MSGSTART);
        if (ptr == NULL)
        {
            ptr = topP + offset;
        }
        else
        {
            ptr = ptr + StrLen(NNSH_JBBSTOKEN_MSGSTART);
        }
        // メッセージサイズの現物合わせ... (最後の２、ってなんだっけ？)
        // msgSize = size - (ptr - (topP + offset)) - 2;
        msgSize = size - (ptr - (topP + offset));
        break;

      case NNSH_BBSTYPE_MACHIBBS:
        if (mesNum != 1)
        {
            // 先頭にある、"<dt>"部分を削る... レス番号について、(ずれる場合があるけど)表示するように変更する。
            // （レス番号部分を表示しないようにするには、NNSH_MACHITOKEN_MSGSTART2 → NNSH_MACHITOKEN_MSGSTART にする）
            ptr = StrStr((topP + offset), NNSH_MACHITOKEN_MSGSTART2);
            if (ptr == NULL)
            {
                ptr = topP + offset;
            }
            else
            {
                ptr = ptr + StrLen(NNSH_MACHITOKEN_MSGSTART2);
            }
        }
        else
        {
            // 先頭メッセージの場合...
            ptr = topP + offset;
        }
        // メッセージサイズの現物合わせ...
        msgSize = size - (ptr - (topP + offset)) - 2;
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        kanjiCode = NNSH_KANJICODE_EUC;
        ptr     = (topP + offset);

        // RAWモードのデータ構造調整
        nameP  = StrStr((ptr + 1), NNSH_JBBSTOKEN_RAW_START_RES);
        if (nameP != NULL)
        {
            ptr = nameP + sizeof(NNSH_JBBSTOKEN_RAW_START_RES) - 1;
        }
        msgSize = size;
        break;

      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_SHITARABA:
        kanjiCode = NNSH_KANJICODE_EUC;
        ptr     = (topP + offset);
        msgSize = size;
        break;

      case NNSH_BBSTYPE_HTML:
        // 漢字コードを拾う
        kanjiCode = (NNSH_BBSTYPE_CHARSETMASK & bbsType) >> NNSH_BBSTYPE_CHAR_SHIFT;
        // not break;
#if 0
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif

      default:
        ptr     = (topP + offset);
        msgSize = size;
        break;
    }

    // 先頭にメッセージ番号を表示
    if ((NNshGlobal->NNsiParam)->boldMessageNum != 0)
    {
        StrCopy(buf, "【");
        NUMCATI(buf, mesNum);
        StrCat (buf, "】 ");
    }
    else
    {
        StrCopy(buf, "[");
        NUMCATI(buf, mesNum);
        StrCat (buf, "] ");
    }

    // HTMLタイプのデータだったら、一行目にはデータを表示しないように小細工
    switch ((bbsType & NNSH_BBSTYPE_MASK))
    {
      case NNSH_BBSTYPE_HTML:
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        // ヘッダ色を元に戻すための細工...
        StrCat(buf, "\n");
        htmlP = buf + StrLen(buf);
        break;

      case NNSH_BBSTYPE_MACHIBBS:
        // ヘッダ色を元に戻すための細工...
        StrCat(buf, "\n");
        htmlP = buf + StrLen(buf);
        break;

      default:
        htmlP = NULL;
        break;
    }

    // 名前欄の半角数字をレス番として追加する
    // (ここに入れるのは、、、とりあえず許して...)
    nameP = ptr;
    while ((*nameP == ' ')||(*nameP == '\x0d'))
    { 
        nameP++;
    }
    resNum = 0;
    if ((*nameP >= '1')&&(*nameP <= '9'))
    {
        resNum = *nameP - '0';
        nameP++;
        while ((*nameP >= '0')&&(*nameP <= '9'))
        {
            resNum = resNum * 10 + *nameP - '0';
            nameP++;
        }

        if (setJumpFlag == true)
        {
            // 名前欄の数字を格納
            idxP->jumpMsg[idxP->nofJump] = resNum;
            (idxP->nofJump)++;
        }
    }
    
    // RAW-dispモードのチェック
    if ((NNshGlobal->NNsiParam)->rawDisplayMode != 0)
    {
        // RAW-displayモード、文字列は単純コピーとする
        StrNCopy((buf + StrLen(buf)), ptr, msgSize);
        return;
    }

#ifdef USE_LOGCHARGE
    // UTF8変換DBがインストールされていてかつ、漢字コードがUTF8だったとき...
    if ((kanjiCode == NNSH_KANJICODE_UTF8)&&(DmFindDatabase(0, "UnicodeToJis0208") != 0))
    {
        // RDF/UTF8の解析処理...
        Show_BusyForm(MSG_MESSAGE_ENTRY);  // "メッセージを変換中..." 表示
        ParseMessage_UTF8((buf + StrLen(buf)), ptr, msgSize,
                         &(idxP->nofJump), &(idxP->jumpMsg[0]),
                         kanjiCode, setJumpFlag);
        Hide_BusyForm(false);
        return;
    }
#endif  // #ifdef USE_LOGCHARGE

// ARMletを使用してメッセージの解析を行う。(でも、なんか無駄な処理が...)
#ifdef USE_ARMLET
    // ARMletの関数ポインタを取得する
    if (((NNshGlobal->NNsiParam)->useARMlet != 0)&&(kanjiCode == NNSH_KANJICODE_SHIFTJIS))
    {
        armH = DmGetResource('armc', RSCID_parseMessage_ARM);
        if (armH != 0)
        {
            armP = MemHandleLock(armH);
            if (armP != NULL)
            {
                // ARMletを使用してのインデックス解析処理
#ifdef USE_COLOR
                if (((NNshGlobal->NNsiParam)->useColor != 0)&&
                    ((NNshGlobal->NNsiParam)->useColorMessageToken != 0)&&
                    ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0))
                {
                    // 
                    hanzen = ((0x10000000)|((NNshGlobal->NNsiParam)->convertHanZen));
                }
                else
#endif  //#ifdef USE_COLOR
                {
                    hanzen = (NNshGlobal->NNsiParam)->convertHanZen;
                }
                MemSet(armArgs, sizeof(armArgs), 0x00);
                armArgs[0] = (UInt32) (buf + StrLen(buf));
                armArgs[1] = (UInt32) ptr;
                armArgs[2] = (UInt32) msgSize;
                armArgs[3] = (UInt32) &(nofJump);
                armArgs[4] = (UInt32) &(jmpMsg[0]);
                armArgs[5] = (UInt32) hanzen;
                armArgs[6] = (UInt32) &Han2ZenTable[0];
                armArgs[7] = (UInt32) setJumpFlag;
                armArgs[8] = (UInt32) (NNshGlobal->NNsiParam)->useNameOneLine;  // parseMode;
                (void) PceNativeCall(armP, armArgs);
                MemHandleUnlock(armH);

                if (setJumpFlag == true)
                {
                    // こんな処理、こっち(68k側)でやってりゃスポイルされるなぁ...
                    for (lp = 0; ((lp < nofJump)&&(idxP->nofJump < JUMPBUF)); lp++)
                    {
                        idxP->jumpMsg[idxP->nofJump] = (UInt16) jmpMsg[lp];
                        (idxP->nofJump)++;
                    }
                }
            }
            else
            {
                // (標準の)インデックス解析処理
                parseMessage((buf + StrLen(buf)), ptr, msgSize,
                             &(idxP->nofJump), &(idxP->jumpMsg[0]),kanjiCode,
                             setJumpFlag);
            }
            DmReleaseResource(armH);
        }
        else
        {
            // (標準の)インデックス解析処理
            parseMessage((buf + StrLen(buf)), ptr, msgSize,
                         &(idxP->nofJump), &(idxP->jumpMsg[0]), kanjiCode,
                         setJumpFlag);
        }
    }
    else
#endif
    {
        // (標準の)インデックス解析処理
        parseMessage((buf + StrLen(buf)), ptr, msgSize,
                     &(idxP->nofJump), &(idxP->jumpMsg[0]), kanjiCode,
                     setJumpFlag);
    }

    // Goメニューのアンカーを整列する場合...
    if ((NNshGlobal->NNsiParam)->sortGoMenuAnchor != 0)
    {
        sortJumpMsgOrder(&(idxP->nofJump), &(idxP->jumpMsg[0]));
    }

    // HTMLモードの場合、色を無理やりNORMAL色に戻す(強引...)
    if ((htmlP != NULL)&&(*htmlP == NNSH_CHARCOLOR_ESCAPE))
    {
        *(htmlP + 1) = NNSH_CHARCOLOR_NORMAL;
    }
    return;
}


/*-------------------------------------------------------------------------*/
/*   Function : sortJumpMsgOrder                                           */
/*                                                 レス番号を並べ替えする  */
/*-------------------------------------------------------------------------*/
static void sortJumpMsgOrder(UInt16 *nofJump, UInt16 *msgList)
{
    Boolean isMatching;
    UInt16 msgNumList[JUMPBUF + MARGIN];
    UInt16 lp, lp2, lp3, msgCnt;

    // レス番号の重複を削る...
    msgCnt = 0;
    for (lp = 0; lp < *nofJump; lp++)
    {
        isMatching = false;
        for (lp2 = 0; lp2 < msgCnt; lp2++)
        {
            if (msgList[lp] == msgNumList[lp2])
            {
                isMatching = true;
                break;
            }
        }
        if (isMatching == false)
        {
            // アイテムを登録する
            if ((NNshGlobal->NNsiParam)->sortGoMenuAnchor < 2)
            {
                // insertion sortをしかける...
                for (lp3 = 0; lp3 < msgCnt; lp3++)
                {
                    if (msgNumList[lp3] > msgList[lp])
                    {
                        MemMove(&msgNumList[lp3 + 1], &msgNumList[lp3], (sizeof(UInt16) * (msgCnt - lp3)));
                        msgNumList[lp3] = msgList[lp];
                        break;
                    }
                }
                if (lp3 >= msgCnt)
                {
                    msgNumList[msgCnt] = msgList[lp];
                }
            }
            else
            {
                // 出現順に並べる。。。
                msgNumList[msgCnt] = msgList[lp];
            }
            msgCnt++;
        }
    }
    
    // 書き戻す...
    for (lp = 0; lp < msgCnt; lp++)
    {
        msgList[lp] = msgNumList[lp];
    }
    *nofJump = msgCnt;
    
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : displayMessage                                             */
/*                                                   メッセージを表示する  */
/*-------------------------------------------------------------------------*/
static Err displayMessage(NNshMessageIndex *idxP, UInt16 isBottom)
{
    FormType    *frm;
    FieldType   *fldP;
    MemHandle    txtH, oldTxtH;
    Err          ret = errNone;
    UInt16       msg, allocSize, lp;
#ifdef USE_COLOR
    UInt16       len;
#endif
    UInt32       offset;
    Char        *buf;

    // 変数の初期化
    msg    = idxP->openMsg;
    offset = 0;

    //  指定されたメッセージ番号が妥当かチェックする。
    if ((msg < 1)||(msg > idxP->nofMsg))
    {
        return (~errNone);
    }
    idxP->blockStartMsg = msg;

    // バッファからメッセージを取得する
    ret = NNsh_Get_MessageToBuf(idxP, msg, GET_MESSAGE_TYPE_HALF, true);
    if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        // 表示バッファを確保する
        if ((msg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
        {
            allocSize = (idxP->msgOffset)[idxP->nofMsg];
        }
        else
        {
            allocSize = (idxP->msgOffset)[msg + (NNshGlobal->NNsiParam)->blockDispMode];
        }
        allocSize = allocSize - (idxP->msgOffset)[msg - 1] + TINYBUF;
        allocSize = allocSize + allocSize;

        txtH = MemHandleNew(allocSize);
        if (txtH == 0)
        {
            // 領域の確保に失敗した場合、エラー表示する
            NNsh_InformMessage(ALTID_WARN, MSG_SUBJECT_LISTFAIL,
                               " size:", allocSize);
            return (~errNone);
        }
        buf  = MemHandleLock(txtH);
        if (buf == NULL)
        {
            MemHandleFree(txtH);
            NNsh_InformMessage(ALTID_WARN, "MemHandleLock()", " ", 0);
            return (~errNone);
        }
        MemSet(buf, allocSize, 0x00);

        // ジャンプ番号バッファを初期化
        idxP->nofJump = 0;
        idxP->dispMsg = (NNshGlobal->NNsiParam)->blockDispMode;
        lp = 1;
        do {
            // メッセージの整形処理
            offset = ((idxP->msgOffset)[msg - 1] - (idxP->fileOffset));
            displayMessageSub(idxP->bbsType, idxP->buffer, msg, offset,
                              (idxP->msgOffset)[msg] - (idxP->msgOffset)[msg - 1],
                              buf, true);
            msg++;
            // RAW-dispモードでなく、キーワードチェックが指示されていたときは、
            if (((NNshGlobal->NNsiParam)->rawDisplayMode == 0)&&((NNshGlobal->NNsiParam)->hideMessage != 0))
            {
                // (簡単)NGキーワードチェック処理
                checkHideMessage(buf);
            }

            if (((NNshGlobal->NNsiParam)->blockDispMode == 0)||(msg > idxP->nofMsg))
            {
                // 連続表示でない、もしくはスレの末尾まで表示した(抜ける)
                idxP->dispMsg = lp;
                break;
            }

            // Zone表示時のセパレータ設定
#ifdef USE_COLOR
            // セパレータの色を設定する
            if (((NNshGlobal->NNsiParam)->useColor != 0)&&
                ((NNshGlobal->NNsiParam)->useColorMessageToken != 0)&&
                ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0))
            {
                len = StrLen(buf);
                buf[len]     = NNSH_CHARCOLOR_ESCAPE;
                buf[len + 1] = NNSH_CHARCOLOR_FOOTER;
                buf[len + 2] = '\0';
                StrCat(buf, (NNshGlobal->NNsiParam)->messageSeparator);

                len = StrLen(buf);
                buf[len]     = NNSH_CHARCOLOR_ESCAPE;
                buf[len + 1] = NNSH_CHARCOLOR_NORMAL;
                buf[len + 2] = '\0';
            }
            else
#endif  //#ifdef USE_COLOR
            {
                // 次データへのセパレータを表示する
                StrCat(buf, (NNshGlobal->NNsiParam)->messageSeparator);
            }
            buf = buf + StrLen(buf);

            // バッファ内にデータが読み込まれているか確認する
            if (((idxP->msgOffset)[msg - 1]  < idxP->fileOffset)||
                ((idxP->msgOffset)[msg] - 1  > idxP->fileOffset + idxP->fileReadSize))
            {
                // 次のデータが現在読み込まれているバッファに存在しない場合は抜ける
                idxP->dispMsg = lp;
                break;
            }            
            lp++;
        } while (lp <= (NNshGlobal->NNsiParam)->blockDispMode);

       (void) MemHandleUnlock(txtH);

        //////////// フィールドの表示 ////////////
        frm     = FrmGetActiveForm();
        fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        oldTxtH = FldGetTextHandle(fldP);
        FldSetTextHandle(fldP, NULL);
        FldSetTextHandle(fldP, txtH);
        if (oldTxtH != 0)
        {
            (void) MemHandleFree(oldTxtH);
        }


        // 描画モードの切り替え
        if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
        {
            // フィールドの表示系変数をクリアする
            NNshGlobal->totalLine   = 0;
            NNshGlobal->nofPage     = 0;
            NNshGlobal->currentPage = 0;
            NNshGlobal->prevHRLines = 0;

            // フィールドの描画(ハイレゾ画面)
            NNsi_HRFldDrawField(fldP, isBottom);

            // スクロールバーの更新
            NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, SCLID_MESSAGE)));
        }
        else
        {
            // フィールドの描画(通常画面)
            FldDrawField(fldP);

            // スクロールバーの表示を更新する
            NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);

            FrmDrawForm(frm);
        }
    }

    if ((NNshGlobal->NNsiParam)->blockDispMode != 0)
    {
        // ゾーン表示時、レス末尾を表示しているか？
        if ((idxP->openMsg + idxP->dispMsg) >= idxP->nofMsg)
        {
            FormType      *frm;
            ScrollBarType *barP;
            UInt16         val, max, min, page;

            // ゾーン表示で、次ページがない場合は、末尾まで読んだことにする
            frm  = FrmGetActiveForm();
            barP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SCLID_MESSAGE));
            SclGetScrollBar(barP, &val, &min, &max, &page);
            if ((max == min)&&(min == val)) // ← スクロールバーが表示されないパターンはこのはず...
            {
                // 末尾まで読んだことにする。
                idxP->openMsg =  idxP->nofMsg;
            }
        }
    }

    // 読んだメッセージ番号(とお気に入り情報を)記憶する
    MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                     NNSH_MSGATTR_NOTSPECIFY, 
                     (NNshGlobal->NNsiParam)->hideMessage);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveMessageNext                                            */
/*                                           ひとつ後ろのメッセージを表示  */
/*-------------------------------------------------------------------------*/
static Boolean moveMessageNext(Boolean chkScroll)
{

    Boolean           ret = false;
    NNshMessageIndex *idxP;

    // indexを変数に入れる
    idxP = NNshGlobal->msgIndex;

    if (idxP->openMsg != idxP->blockStartMsg)
    {
        idxP->openMsg = idxP->blockStartMsg;

        // 読んだメッセージ番号(とお気に入り情報を)記憶する
        MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                         NNSH_MSGATTR_NOTSPECIFY,
                         (NNshGlobal->NNsiParam)->hideMessage);
    }

    // メッセージのスクロールチェック
    if (chkScroll == true)
    {
        ret = NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0, winDown); 
    }


    if ((NNshGlobal->NNsiParam)->blockDispMode == 0)
    {
        if ((ret != true)&&(idxP->openMsg < idxP->nofMsg))
        {
             // 表示メッセージ番号を更新し、画面表示。
             (idxP->openMsg)++;
             (void) displayMessage(NNshGlobal->msgIndex, 0);
             ret = true;
        }
    }
    else
    {
        // レス連続表示モードのとき
        if (ret != true)  // 次ブロックを表示する...
        {
            // 表示メッセージ番号を更新
            if ((idxP->openMsg + idxP->dispMsg) <= idxP->nofMsg)
            {
                idxP->openMsg =  idxP->openMsg + idxP->dispMsg;

                // 画面表示。
                (void) displayMessage(NNshGlobal->msgIndex, 0);

                return (true);
            }
            if (idxP->openMsg < idxP->nofMsg)
            {
                // 末尾まで読んだことにする。
                idxP->openMsg =  idxP->nofMsg;
 
                // 読んだメッセージ番号(とお気に入り情報を)記憶する
                MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                                 NNSH_MSGATTR_NOTSPECIFY,
                                 (NNshGlobal->NNsiParam)->hideMessage);
                ret = true;
            }
        }
        else
        {
            // スクロールしたとき、末尾まで読んでいるか？
            if ((idxP->openMsg + idxP->dispMsg) > idxP->nofMsg)
            {
                FormType      *frm;
                ScrollBarType *barP;
                UInt16         val, max, min, page;

                // ゾーン表示で、次ページがない場合は、末尾まで読んだことにする
                frm  = FrmGetActiveForm();
                barP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SCLID_MESSAGE));
                SclGetScrollBar(barP, &val, &min, &max, &page);
                if (val >= max) // ← スクロールバーの末尾に到達しているパターンはこのはず...
                {
                    // (スクロールバーが表示される)末尾まで読んだことにする。
                    idxP->openMsg =  idxP->nofMsg;

                    // 読んだメッセージ番号(とお気に入り情報を)記憶する
                    MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                                     NNSH_MSGATTR_NOTSPECIFY,
                                     (NNshGlobal->NNsiParam)->hideMessage);
                    ret = true;
                }
            }
        }
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveMessagePrev                                            */
/*                                             ひとつ前のメッセージを表示  */
/*-------------------------------------------------------------------------*/
static Boolean moveMessagePrev(Boolean chkScroll, UInt16 dispLoc)
{
    Int16             resNum;
    Boolean           ret = false;
    NNshMessageIndex *idxP;

    // indexを変数に入れる
    idxP = NNshGlobal->msgIndex;

    if (idxP->openMsg != idxP->blockStartMsg)
    {
        idxP->openMsg = idxP->blockStartMsg;

        // 読んだメッセージ番号(とお気に入り情報を)記憶する
        MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                         NNSH_MSGATTR_NOTSPECIFY,
                         (NNshGlobal->NNsiParam)->hideMessage);
    }

    // メッセージのスクロールチェック
    if (chkScroll == true)
    {
        ret = NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0,  winUp);
    }

    // あとでリファクタ必要？
    if ((NNshGlobal->NNsiParam)->blockDispMode == 0)
    {
        if ((ret != true)&&(idxP->openMsg > 1))
        {
            // 表示メッセージ番号を更新し、画面表示。
            (idxP->openMsg)--;
            (void) displayMessage(NNshGlobal->msgIndex, dispLoc);
            ret = true;
        }
    }
    else
    {
        // レス連続表示モードのとき
        if (ret != true)
        {
            // 表示メッセージ番号を更新
            resNum = idxP->openMsg - (NNshGlobal->NNsiParam)->blockDispMode;
            if (resNum >= 1)
            {
                idxP->openMsg = idxP->openMsg - (NNshGlobal->NNsiParam)->blockDispMode;

                // 画面表示。
                (void) displayMessage(NNshGlobal->msgIndex,
                                      (NNshGlobal->NNsiParam)->dispBottom);
            }
            else
            {
                idxP->openMsg = 1;

                // 画面表示。
                (void) displayMessage(NNshGlobal->msgIndex, 0);
            }
            ret = true;
        }
    }
    return (ret);
}
/*-------------------------------------------------------------------------*/
/*   Function : jumpMessage                                                */
/*                                                (メッセージジャンプ処理) */
/*-------------------------------------------------------------------------*/
static Boolean jumpMessage(FormType *frm, NNshMessageIndex *idxP)
{
    UInt16     tempMsg, num, btnId, size, nofItems, savedParam;
    UInt16     jumpList[NNSH_NOF_JUMPMSG_EXT + JUMPBUF + 2];
    UInt16     msgAttribute;
    EventType *dummyEvent;
    FieldType *fldP;
    Char      *numP, buffer[BUFSIZE * 2];
    MemHandle  txtH;

    // とりあえず、、、レスpop up 用
    NNshGlobal->work3 = 0;

    // 入力された数値を取得する
    tempMsg = idxP->openMsg;
    fldP    = FrmGetObjectPtr(frm, 
                              FrmGetObjectIndex(frm, FLDID_MESSAGE_NUMBER));
    txtH = FldGetTextHandle(fldP);
    if (txtH != NULL)
    {
        numP = MemHandleLock(txtH);
        num = StrAToI(numP);

        // 数値が妥当なものかチェック
        if ((num > 0)&&(num <= idxP->nofMsg))
        {
            tempMsg = num;
        }
        MemHandleUnlock(txtH);
    }

    // 正しい数字がフィールドに入力されていなかった場合
    if (tempMsg == idxP->openMsg)
    {
        //  Goボタンが押されたとき、数字が入力されていなかった場合、
        //  解釈したスレ番号をリストオブジェクトとして表示する。
        size = (idxP->nofJump + NNSH_NOF_JUMPMSG_EXT + 1) * NNSH_JUMPMSG_LEN;
        idxP->jumpListTitles = MEMALLOC_PTR(size);
        if (idxP != NULL)
        {
            // リストに表示するタイトルリストを作成する
            nofItems = 0; 
            MemSet(jumpList, sizeof(jumpList), 0x00);
            MemSet(idxP->jumpListTitles, 0x00, size);
            numP = idxP->jumpListTitles;

            if ((NNshGlobal->NNsiParam)->addReturnToList != 0)
            {
                // リストに「一覧へ」を追加
                StrCopy(numP, NNSH_JUMPMSG_TO_LIST);
                numP = numP + sizeof(NNSH_JUMPMSG_TO_LIST);
                jumpList[nofItems] = (NNSH_JUMPSEL_TO_LIST);
                nofItems++;
            }

            // ジャンプ番号を設定する
            for (num = 0; num < idxP->nofJump; num++)
            {
                // ジャンプ番号が範囲外の大きな値のときは表示しない
                if (idxP->jumpMsg[num] <= idxP->nofMsg)
                {
                    StrCopy(numP, NNSH_JUMPMSG_HEAD);
                    NUMCATI(numP, idxP->jumpMsg[num]);
                    numP = numP + StrLen(numP) + 1;
                    jumpList[nofItems] = idxP->jumpMsg[num];
                    nofItems++;
                }
            }

            if ((NNshGlobal->NNsiParam)->addMenuBackRtnMsg != 0)
            {
                // リストに「１つ戻る」を追加
                StrCopy(numP, NNSH_JUMPMSG_BACK);
                numP = numP + sizeof(NNSH_JUMPMSG_BACK);
                jumpList[nofItems] = (NNSH_JUMPSEL_BACK);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addJumpTopMsg != 0)
            {
                // リストに「先頭へ」を追加
                StrCopy(numP, NNSH_JUMPMSG_TOP);
                numP = numP + sizeof(NNSH_JUMPMSG_TOP);
                jumpList[nofItems] = (NNSH_JUMPSEL_TOP);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addJumpBottomMsg != 0)
            {
                // リストに「末尾へ」を追加
                StrCopy(numP, NNSH_JUMPMSG_BOTTOM);
                numP = numP + sizeof(NNSH_JUMPMSG_BOTTOM);
                jumpList[nofItems] = (NNSH_JUMPSEL_BOTTOM);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addMenuMsg != 0)
            {
                // リストに「メニュー」を追加
                StrCopy(numP, NNSH_JUMPMSG_OPEN_MENU);
                numP = numP + sizeof(NNSH_JUMPMSG_OPEN_MENU);
                jumpList[nofItems] = (NNSH_JUMPSEL_OPEN_MENU);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addLineGetPart != 0)
            {
                // リストに「差分取得」を追加
                StrCopy(numP, NNSH_JUMPMSG_UPDATE);
                numP = numP + sizeof(NNSH_JUMPMSG_UPDATE);
                jumpList[nofItems] = (NNSH_JUMPSEL_UPDATE);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addLineDisconn != 0)
            {
                // リストに「回線切断」を追加
                StrCopy(numP, NNSH_JUMPMSG_HANGUP);
                numP = numP + sizeof(NNSH_JUMPMSG_HANGUP);
                jumpList[nofItems] = (NNSH_JUMPSEL_HANGUP);
                nofItems++;
            }

            if (NNshGlobal->browserCreator != 0)
            {
                if ((NNshGlobal->NNsiParam)->addMenuSelAndWeb != 0)
                {
                    // リストに「選択+Web」を追加
                    StrCopy(numP, NNSH_JUMPMSG_OPENWEB);
                    numP = numP + sizeof(NNSH_JUMPMSG_OPENWEB);
                    jumpList[nofItems] = (NNSH_JUMPSEL_OPENWEB);
                    nofItems++;
                }
            }

            if (NNshGlobal->browserCreator != 0)
            {
#ifdef USE_OPENURL_ONLYHTML
                if ((idxP->bbsType & NNSH_BBSTYPE_MASK) == NNSH_BBSTYPE_HTML)
#endif
                {
                    // リストに「リンクを開く」を追加
                    StrCopy(numP, NNSH_JUMPMSG_OPENLINK);
                    numP = numP + sizeof(NNSH_JUMPMSG_OPENLINK);
                    jumpList[nofItems] = (NNSH_JUMPSEL_OPENLINK);
                    nofItems++;
                }
            }

            if ((NNshGlobal->NNsiParam)->addMenuGraphView != 0)
            {
                // リストに「描画モード変更」を追加
                StrCopy(numP, NNSH_JUMPMSG_CHANGEVIEW);
                numP = numP + sizeof(NNSH_JUMPMSG_CHANGEVIEW);
                jumpList[nofItems] = (NNSH_JUMPSEL_CHANGEVIEW);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addMenuFavorite != 0)
            {
                // リストに「お気に入り設定変更」を追加
                StrCopy(numP, NNSH_JUMPMSG_FAVORITE);
                numP = numP + sizeof(NNSH_JUMPMSG_FAVORITE);
                jumpList[nofItems] = (NNSH_JUMPSEL_FAVORITE);
                nofItems++;
            }
            if ((NNshGlobal->NNsiParam)->addMenuOutputMemo != 0)
            {
                // リストに「メモ帳出力」を追加
                StrCopy(numP, NNSH_JUMPMSG_OUTPUTMEMO);
                numP = numP + sizeof(NNSH_JUMPMSG_OUTPUTMEMO);
                jumpList[nofItems] = (NNSH_JUMPSEL_OUTPUTMEMO);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addDeviceInfo != 0)
            {
                // リストに「デバイス情報」を追加
                StrCopy(numP, NNSH_JUMPMSG_DEVICEINFO);
                numP = numP + sizeof(NNSH_JUMPMSG_DEVICEINFO);
                jumpList[nofItems] = (NNSH_JUMPSEL_DEVICEINFO);
                nofItems++;
            }

            // A2DAがインストールされているか確認する
            if (CheckInstalledResource_NNsh('DAcc', 'a2DA') != false)
            {
                // リストに「AA表示」を追加
                StrCopy(numP, NNSH_JUMPMSG_EXECA2DA);
                numP = numP + sizeof(NNSH_JUMPMSG_EXECA2DA);
                jumpList[nofItems] = (NNSH_JUMPSEL_EXECA2DA);
                nofItems++;
            }

#ifdef USE_BT_CONTROL
            // Bluetooth On/Off
            if ((NNshGlobal->NNsiParam)->addBtOnOff != 0)
            {
                    // リストに「Bt On/Bt Off」を追加
                    StrCopy(numP, NNSH_JUMPMSG_BTOOTH_ON);
                    numP = numP + sizeof(NNSH_JUMPMSG_BTOOTH_ON);
                    jumpList[nofItems] = (NNSH_JUMPSEL_BTOOTH_ON);
                    nofItems++;

                    StrCopy(numP, NNSH_JUMPMSG_BTOOTH_OFF);
                    numP = numP + sizeof(NNSH_JUMPMSG_BTOOTH_OFF);
                    jumpList[nofItems] = (NNSH_JUMPSEL_BTOOTH_OFF);
                    nofItems++;
            }
#endif

            if ((NNshGlobal->NNsiParam)->useBookmark != 0)
            {
                // リストに「しおり設定」を追加
                StrCopy(numP, NNSH_JUMPMSG_BOOKMARK);
                numP = numP + sizeof(NNSH_JUMPMSG_BOOKMARK);
                jumpList[nofItems] = (NNSH_JUMPSEL_BOOKMARK);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addMenuGetThreadNum != 0)
            {
                // リストに「スレ番指定取得」を追加
                StrCopy(numP, NNSH_JUMPMSG_GET_THREADNUM);
                numP = numP + sizeof(NNSH_JUMPMSG_GET_THREADNUM);
                jumpList[nofItems] = (NNSH_JUMPSEL_GET_THREADNUM);
                nofItems++;
            }

            if ((CheckInstalledResource_NNsh('appl', 'asUR') != false)||
                (CheckInstalledResource_NNsh('DAcc', 'mmuA') != false))
            {
                // MemoURL/MemoURLDAがインストールされていた...
                // リストに「MemoURLへ出力」を追加
                StrCopy(numP, NNSH_JUMPMSG_MEMOURL);
                numP = numP + sizeof(NNSH_JUMPMSG_MEMOURL);
                jumpList[nofItems] = (NNSH_JUMPSEL_MEMOURL);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addNgSetting3 != 0)
            {
                // リストに「NG切替」を追加
                StrCopy(numP, NNSH_JUMPMSG_TOGGLE_NGWORD);
                numP = numP + sizeof(NNSH_JUMPMSG_TOGGLE_NGWORD);
                jumpList[nofItems] = (NNSH_JUMPSEL_TOGGLE_NGWORD);
                nofItems++;

                // リストに「NG登録」を追加
                StrCopy(numP, NNSH_JUMPMSG_ENTRY_NGWORD);
                numP = numP + sizeof(NNSH_JUMPMSG_ENTRY_NGWORD);
                jumpList[nofItems] = (NNSH_JUMPSEL_ENTRY_NGWORD);
                nofItems++;
            }

            if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
            {
                // webDAがインストールされていた...
                // リストに「webDA(link)」を追加
                StrCopy(numP, NNSH_JUMPMSG_OPENLINK_WEBDA);
                numP = numP + sizeof(NNSH_JUMPMSG_OPENLINK_WEBDA);
                jumpList[nofItems] = (NNSH_JUMPSEL_OPENLINK_WEBDA);
                nofItems++;
            }

            if ((idxP->bbsType & NNSH_BBSTYPE_MASK) == NNSH_BBSTYPE_HTML)
            {
                // HTML形式のファイル参照中の場合には、、、

                // リストに「RAWモード切替」を追加
                StrCopy(numP, NNSH_JUMPMSG_TOGGLERAWMODE);
                numP = numP + sizeof(NNSH_JUMPMSG_TOGGLERAWMODE);
                jumpList[nofItems] = (NNSH_JUMPSEL_TOGGLERAWMODE);
                nofItems++;

                // リストに「区切り変更」を追加
                StrCopy(numP, NNSH_JUMPMSG_SELECTSEPA);
                numP = numP + sizeof(NNSH_JUMPMSG_SELECTSEPA);
                jumpList[nofItems] = (NNSH_JUMPSEL_SELECTSEPA);
                nofItems++;
 
                 // リストに「区切り編集」を追加
                StrCopy(numP, NNSH_JUMPMSG_EDITSEPA);
                numP = numP + sizeof(NNSH_JUMPMSG_EDITSEPA);
                jumpList[nofItems] = (NNSH_JUMPSEL_EDITSEPA);
                nofItems++;

                 // リストに「削除後一覧へ」を追加
                StrCopy(numP, NNSH_JUMPMSG_BACK_AND_DEL);
                numP = numP + sizeof(NNSH_JUMPMSG_BACK_AND_DEL);
                jumpList[nofItems] = (NNSH_JUMPSEL_BACK_AND_DEL);
                nofItems++;
            }

            // 選択ウィンドウを表示する
            btnId = NNshWinSelectionWindow(FRMID_JUMPMSG, idxP->jumpListTitles, nofItems, 0);
            MEMFREE_PTR(idxP->jumpListTitles);
            if (btnId != BTNID_JUMPCANCEL)
            {
                // 選択リスト番号からコマンドを取得
                switch (jumpList[NNshGlobal->jumpSelection])
                {
                  case NNSH_JUMPSEL_BACK:
                    // １つ前に飛んだメッセージに戻る
                    jumpPrevious();
                    return (false);
                    break;

                  case NNSH_JUMPSEL_TO_LIST:
                    // スレ選択画面に戻る
                    ReadMessage_Close(NNshGlobal->backFormId);
                    return (false);
                    break;

                  case NNSH_JUMPSEL_OPEN_MENU:
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

                    // メッセージ(仮)再表示
                    (void) displayMessage(idxP, 0);
                    return (false);
                    break;

                  case NNSH_JUMPSEL_UPDATE:
                    // 差分取得
                    (void) PartReceiveMessage_View();
                    return (false);
                    break;

                  case NNSH_JUMPSEL_HANGUP:
                    // 回線切断
                    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
                    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
                    NNshNet_LineHangup();
                    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

                    // メッセージ表示
                    (void) displayMessage(idxP, 0);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_OPENWEB:
                    // 全選択＋Webブラウザで開く
                    frm  = FrmGetActiveForm();
                    fldP = FrmGetObjectPtr(frm, 
                                       FrmGetObjectIndex(frm, FLDID_MESSAGE));
                    savedParam = FldGetTextLength(fldP);

                    // 選択文字列の上限を設定する
                    savedParam = (savedParam > NNSH_LIMIT_COPYBUFFER) ?
                                           NNSH_LIMIT_COPYBUFFER : savedParam;
                    FldSetSelection(fldP, 0, savedParam);
                    FldCopy(fldP);
                    if (NNshGlobal->browserCreator != 0)
                    {
                        // WebBrowserで開く(このとき、確認MSGなし)
                        openWebBrowser(NNSH_OMITDIALOG_ALLOMIT, buffer, sizeof(buffer));
                    }
                    // メッセージ表示
                    (void) displayMessage(idxP, 0);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_TOP:
                    // 先頭へ
                    tempMsg = 1;
                    break;

                  case NNSH_JUMPSEL_BOTTOM:
                    // 末尾へ
                    tempMsg = idxP->nofMsg;
                    break;

                  case NNSH_JUMPSEL_CHANGEVIEW:
                    // 描画モード変更
                    if ((NNshGlobal->NNsiParam)->useSonyTinyFont == 0)
                    {
                        // ハイレゾモードへ変更
                        (NNshGlobal->NNsiParam)->useSonyTinyFont = 1;
                    }
                    else
                    {
                        // 通常モードへ変更
                        (NNshGlobal->NNsiParam)->useSonyTinyFont = 0;
                    }

                    // 画面を一度閉じてもう一度開く
                    ReadMessage_Close(FRMID_MESSAGE);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_FAVORITE:
                    // スレ情報を表示する
                    DisplayMessageInformations((NNshGlobal->NNsiParam)->openMsgIndex, 
                                               &msgAttribute);

                    // お気に入り情報ラベルを更新
                    setFavoriteLabel(FrmGetActiveForm(), msgAttribute);

                    // 表示を更新する(OS3.1デバイスでは真っ白になるため)
                    FrmDrawForm(FrmGetActiveForm());
                    (void) displayMessage(idxP, 0);
                    break;

                  case NNSH_JUMPSEL_DEVICEINFO:
                    // デバイス情報を表示する
                    ShowDeviceInfo_NNsh();

                    // 表示を更新する(OS3.1デバイスでは真っ白になるため)
                    FrmDrawForm(FrmGetActiveForm());
                    (void) displayMessage(idxP, 0);
                    break;

                  case NNSH_JUMPSEL_OUTPUTMEMO:
                    // メモ帳へ出力する
                    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
                    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
                    (void) outputMemoPad(false);
                    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

                    // メッセージ表示
                    (void) displayMessage(idxP, 0);
                    break;

                  case NNSH_JUMPSEL_BOOKMARK:
                    // しおり設定
                    if ((NNshGlobal->NNsiParam)->useBookmark != 0)
                    {
                        (NNshGlobal->NNsiParam)->lastFrmID     = NNSH_FRMID_MESSAGE;
                        (NNshGlobal->NNsiParam)->bookMsgIndex  = (NNshGlobal->NNsiParam)->openMsgIndex;
                        (NNshGlobal->NNsiParam)->bookMsgNumber = idxP->openMsg;
                        NNsh_InformMessage(ALTID_INFO, MSG_SET_BOOKMARK, "",0);
                    }

                    // メッセージ表示
                    (void) displayMessage(idxP, 0);
                    break;
                    
                  case NNSH_JUMPSEL_OPENLINK:
                    // リンクを開く
                    executeViewAction(MULTIVIEWBTN_FEATURE_OPENURL);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_EXECA2DA:
                    // a2DA起動(AA参照)
                    executeViewAction(MULTIVIEWBTN_FEATURE_EXECA2DA);

                    // メッセージ表示
                    (void) displayMessage(idxP, 0);

                    return (false);
                    break;

#ifdef USE_BT_CONTROL
                  case NNSH_JUMPSEL_BTOOTH_ON:
                    // Bluetooth On
                    executeViewAction(MULTIVIEWBTN_FEATURE_BT_ON);
                    break;

                  case NNSH_JUMPSEL_BTOOTH_OFF:
                    // Bluetooth Off
                    executeViewAction(MULTIVIEWBTN_FEATURE_BT_OFF);
                    break;
#endif
                  case NNSH_JUMPSEL_MEMOURL:
                    // MemoURLへ出力
                    if (CheckInstalledResource_NNsh('DAcc', 'mmuA') == false)
                    {
                         // MemoURLDAがないので、MemoURLへ渡す
                        executeViewAction(MULTIVIEWBTN_FEATURE_MEMOURL);
                    }
                    else
                    {
                         // MemoURLへMemoURLDA経由で渡す
                        executeViewAction(MULTIVIEWBTN_FEATURE_MEMOURLDA);
                    }
                    break;

                  case NNSH_JUMPSEL_TOGGLERAWMODE:
                    // RAWモード/通常モード切替
                    executeViewAction(MULTIVIEWBTN_FEATURE_CHANGERAW);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_SELECTSEPA:
                    // 区切りモード切替
                    executeViewAction(MULTIVIEWBTN_FEATURE_CHGSEPA);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_EDITSEPA:
                    // 区切り文字の編集
                    executeViewAction(MULTIVIEWBTN_FEATURE_EDITSEPA);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_OPENLINK_WEBDA:
                    // webDAでリンクを開く
                    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
                    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
                    executeViewAction(MULTIVIEWBTN_FEATURE_OPENWEBDA);
                    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
                    return (true);
                    break;

                  case NNSH_JUMPSEL_TOGGLE_NGWORD:
                    // NG-3切り替え
                    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
                    {
                        // NG3 ON → OFF
                        (NNshGlobal->NNsiParam)->hideMessage = ((NNshGlobal->NNsiParam)->hideMessage & (NNSH_RESETMASK_NGWORD3));
                        NNsh_InformMessage(ALTID_INFO, MSG_RESET_NGWORD3, "",0);
                    }
                    else
                    {
                        // NG3 OFF → ON
                        (NNshGlobal->NNsiParam)->hideMessage = ((NNshGlobal->NNsiParam)->hideMessage | (NNSH_USE_NGWORD3));
                        NNsh_InformMessage(ALTID_INFO, MSG_SET_NGWORD3, "",0);
                    }
                    (void) displayMessage(idxP, 0);
                    break;

                  case NNSH_JUMPSEL_BACK_AND_DEL:
                    // 削除後一覧へ戻る...
                    // (画面を閉じて一覧画面に戻る準備...)
                    ReadMessage_Close(NNshGlobal->backFormId);

                    // スレ削除実処理...
                    DeleteThreadMessage((NNshGlobal->NNsiParam)->openMsgIndex);
                    return (false);
                    break;

                  case NNSH_JUMPSEL_GET_THREADNUM:
                    // スレ番号指定取得を実施する...
                    executeViewAction(MULTIVIEWBTN_FEATURE_GETMESNUM);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_ENTRY_NGWORD:
                    // NG登録
                    executeViewAction(MULTIVIEWBTN_FEATURE_PICKUP_NG);
                    break;

                  default:
                    // 指定番号のメッセージへジャンプ or レス pop up
                    if (((NNshGlobal->NNsiParam)->showResPopup != 0)&&
                        (CheckInstalledResource_NNsh('DAcc', 'nnDA') != false))
                    {
                        // nnDAをインストールしていた場合...レスポップアップ
                        NNshGlobal->work3 = jumpList[NNshGlobal->jumpSelection];
                        return (true);
                    }
                    tempMsg = jumpList[NNshGlobal->jumpSelection];
                    break;
                }
            }
        }
        else
        {
            NNsh_DebugMessage(ALTID_ERROR, "Jump LIST Create Failure.", "", 0);
        }
    }

    // 数字フィールドをクリアする
    NNshWinSetFieldText(frm, FLDID_MESSAGE_NUMBER, true, "", 0); 

    // メッセージを移動してジャンプ、メッセージを表示する
    if (tempMsg != idxP->openMsg)
    {
        idxP->histIdx = 
                  (idxP->histIdx < (PREVBUF - 1)) ? (idxP->histIdx + 1) : 0;
        idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
        idxP->openMsg = tempMsg;

        // メッセージ表示
        (void) displayMessage(idxP, 0);

        return (true);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectScrollEvent                                          */
/*                                       スクロールバー更新イベントの処理  */
/*-------------------------------------------------------------------------*/
static Boolean sclRepEvt_DispMessage(EventType *event)
{
    Int16   lines;

    if (event->data.sclExit.newValue == event->data.sclExit.value)
    {
        // 何もしない
        return (true);
    }

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
        // スクロール行数を指定してスクロール(戻り方向)
        NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, -lines, winUp);
    }
    else
    {
        // スクロール行数を指定してスクロール(進み方向)
        NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, lines, winDown);
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectKeyDownEvent                                         */
/*                                                 キーが押された時の処理  */
/*                                (ジョグアシストＯＦＦ時のジョグ処理追加) */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_KeyDown_DispMessage(EventType *event)
{
    // インデックスがない場合には、すぐにreturnする
    if (NNshGlobal->msgIndex == NULL)
    {
        return (false);
    }

    // キーコード変換(281さん、感謝！)
    switch (KeyConvertFiveWayToJogChara(event))
    {
      // 「下」を押した時の処理
      case vchrPageDown:
        (void) executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.down)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // ジョグダイヤル下を押した時の処理
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case chrDownArrow:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // 次検索
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHNEXT);
        }
        else
        {
            // 次ページ
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PAGEDOWN);
        }
        break;

      case chrCapital_N:  // Nキーの入力
      case chrSmall_N:
        // ひとつ後ろのメッセージに移動する 
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_NEXT);
        break;

      // 「上」を押した時の処理
      case vchrPageUp:
        (void) executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.up)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // ジョグダイヤル上を押した時の処理
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case chrUpArrow:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // 前検索
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHPREV);
        }
        else
        {
            // 前ボタン 
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PAGEUP);
        }
        break;

      case chrCapital_P:  // Pキーの入力
      case chrSmall_P:
        // ひとつ前のメッセージに移動する
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_PREVIOUS); 
        break;

      // ジョグダイヤル押し回し(上)の処理
      case vchrJogPushedUp:
        // 一時状態を(回転中に)設定する
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGUP;
#endif
        // 押し回しで１つづづ上げる？
        if ((NNshGlobal->NNsiParam)->notCursor != 0)
        {
             // 次のメッセージを表示して終了
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PREVIOUS);
        }
        else
        {
            // 10個前のレス表示
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PAGEUP10);
        }
        break;

      case chrCapital_R:  // Rキーの入力
      case chrSmall_R:
        break;

      case chrCapital_F:  // Fキーの入力
      case chrSmall_F:
        // 検索表示
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCH);
        break;

      // カーソルキー(左)押下時の処理
      case chrLeftArrow:
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.left)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // カーソルキー(右)押下時の処理
      case chrRightArrow:
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.right)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // ジョグダイヤル押し回し(下)の処理
      case vchrJogPushedDown:
        // 一時状態を(回転中に)設定する
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGDOWN;
#endif
        // 押し回しで１つづづ下げる？
        if ((NNshGlobal->NNsiParam)->notCursor != 0)
        {
            // 次のメッセージを表示して終了
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_NEXT);
        }
        else
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PAGEDOWN10);
        }
        break;

#ifdef USE_CLIE
      // JOG Push時の処理(より正確にはボタンを離したときだけど)
      case vchrJogRelease:
        if (NNshGlobal->tempStatus == NNSH_TEMPTYPE_CLEARSTATE)
        {
            // JOG PUSHボタンと等価な動作
            executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.jogPush)&(MULTIVIEWBTN_FEATURE_MASK));
        }
        else
        {
            // 一時状態をクリアする
            NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
            return (true);
        }
        break;
#endif
      //  HandEra Jog-Push/Enterキーを押したときの処理
      case chrCarriageReturn:
      case chrLineFeed:
      case vchrRockerCenter:
      case vchrHardRockerCenter:
      case vchrThumbWheelPush:
        // JOG Pushボタンの動作
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.jogPush)&(MULTIVIEWBTN_FEATURE_MASK));
        return (true);
        break;

      case vchrHard1:
      case vchrTapWaveSpecificKeyULeft:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // ハードキー１を押したときの設定
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.key1)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      case vchrHard2:
      case vchrTapWaveSpecificKeyURight:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // ハードキー２を押したときの設定
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.key2)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      case vchrHard3:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // ハードキー３を押したときの設定
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.key3)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      case vchrHard4:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // ハードキー制御しない場合は終了
            return (false);
        }
        // ハードキー４を押したときの設定
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.key4)&(MULTIVIEWBTN_FEATURE_MASK));
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
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.clieCapture)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // Sony バックボタン/HandEra バックボタン/ESCキー/Xキーの処理
      // case chrBackspace:
      case vchrJogBack:
      case chrEscape:
      case vchrThumbWheelBack:
      case vchrGarminEscape:
      case chrCapital_X:  // Xキーの入力
      case chrSmall_X:
        // JOG Backボタンの動作
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.jogBack)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // 一つ前のジャンプ元レスに戻る(Backボタンと同じ)
      case chrCapital_Z:  // Zキーの入力
      case chrSmall_Z:
        executeViewAction(MULTIVIEWBTN_FEATURE_RETURN);
        break;

      // Lキー入力
      case chrCapital_L:
      case chrSmall_L:
        // フォント変更
        executeViewAction(MULTIVIEWBTN_FEATURE_CHANGEFONT);
        break;

      case chrCapital_V:  // Vキーの入力
      case chrSmall_V:
        // a2DA起動
        executeViewAction(MULTIVIEWBTN_FEATURE_EXECA2DA);
        break;

      case chrCapital_H:  // Hキーの入力
      case chrSmall_H:
        // リンクを開く
        executeViewAction(MULTIVIEWBTN_FEATURE_OPENURL);
        break;

      case chrCapital_U:  // Uキーの入力
      case chrSmall_U:
        // 選択＋Web
        executeViewAction(MULTIVIEWBTN_FEATURE_SELANDWEB);
        break;

      case chrCapital_O:  // Oキーの入力
      case chrSmall_O:
        // メモ帳に出力
        executeViewAction(MULTIVIEWBTN_FEATURE_OUTPUTMEMO);
        break;

      case chrCapital_J:  // Jキーの入力
      case chrSmall_J:
        // スレ番指定のメッセージ取得
        executeViewAction(MULTIVIEWBTN_FEATURE_GETMESNUM);
        break;

      // Dキー入力
      case chrCapital_D:
      case chrSmall_D:
        executeViewAction(MULTIVIEWBTN_FEATURE_DRAWMODE);
        break;

      // Qキー入力
      case chrCapital_Q:
      case chrSmall_Q:
        // 回線切断
        executeViewAction(MULTIVIEWBTN_FEATURE_DISCONNECT);
        break;

      // Cキー入力
      case chrCapital_C:
      case chrSmall_C:
        // デバイスの情報表示
        executeViewAction(MULTIVIEWBTN_FEATURE_DEVICEINFO);
        break;

      // Bキー入力
      case chrCapital_B:
      case chrSmall_B:
        // スレの末尾へ
        executeViewAction(MULTIVIEWBTN_FEATURE_GOTOBOTTOM);
        break;

        // Tキー入力
      case chrCapital_T:
      case chrSmall_T:
        // スレの先頭へ
        executeViewAction(MULTIVIEWBTN_FEATURE_GOTOTOP);
        break;

        // スペースキー・Gキーの入力(メッセージJUMP)
      case chrSpace:
      case chrSJISIdeographicSpace:
      case chrCapital_G:
      case chrSmall_G:
        executeViewAction(MULTIVIEWBTN_FEATURE_JUMP);
        break;

      case chrCapital_W:  // Wキーの入力
      case chrSmall_W:
        // スレ書き込み(OFFLINEスレでない時のみ書き込み可)
        if ((NNshGlobal->NNsiParam)->writingReplyMsg == 0)
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_WRITE);
        }
        else
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_REPLYMSG);
        }
        break;

      // 数値入力する可能性があるため、、、
      case chrCapital_Y:
      case chrSmall_Y:
        // キーY (Bt ON)
        executeViewAction(MULTIVIEWBTN_FEATURE_BT_ON);
        break;

      case chrCapital_I:
      case chrSmall_I:
        // キーI (Bt Off)
        executeViewAction(MULTIVIEWBTN_FEATURE_BT_OFF);
        break;

      case chrCapital_A:
      case chrSmall_A:
        // キーA (ネットワーク設定)
        executeViewAction(MULTIVIEWBTN_FEATURE_SETNETWORK);
        break;

      case chrCapital_M:
      case chrSmall_M:
        // キーM (一覧へ戻る)
        executeViewAction(MULTIVIEWBTN_FEATURE_BACKTOLIST);
        break;

      case chrComma:
        // コンマキー(レベルのセレクタトリガを押したことにする)
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        SELTRID_FAVOR_THREAD)));
        break;

      case chrFullStop:
        // ピリオドキー（タイトルのセレクタトリガを押したことにする)
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        SELTRID_THREAD_TITLE)));
        break;

      case chrHyphenMinus:
        // －キー（差分取得）
        executeViewAction(MULTIVIEWBTN_FEATURE_GETPART);
        break;
      case chrCommercialAt:
        // @キー(ヘルプ表示)
        executeViewAction(MULTIVIEWBTN_FEATURE_USAGE);
        break;

      case chrSemicolon:
        // セミコロンキー(webDA起動)
        executeViewAction(MULTIVIEWBTN_FEATURE_OPENWEBDA);
        break;

      case chrColon:
        // コロンキー(Image取得)
        executeViewAction(MULTIVIEWBTN_FEATURE_GETIMAGE);
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
                // リサイズ後の描画
                UpdateFieldRegion();
                NNsi_FrmDrawForm(FrmGetActiveForm(), true);
            }
        }
        break;
#endif

      case chrCapital_S:
      case chrSmall_S:
      case chrCapital_E:
      case chrSmall_E:
      default:
        // キーコードを表示する
        // 注：数字キーでの制御はしてはいけない（レス番号で入力するため）
        NNsh_DebugMessage(ALTID_INFO, "Pushed a key  code:", "",
                          event->data.keyDown.chr);
        return (false);
        break;
    }
    return (true);
}

/*=========================================================================*/
/*   Function : OpenForm_NNshMessage                                       */
/*                              (スレッド新規読み出し：メッセージ表示以外) */
/*=========================================================================*/
Err OpenForm_NNshMessage(FormType *frm)
{
    Err                  ret;
    UInt16               bufferSize, fileMode;
    Char                 title   [NNSH_MSGTITLE_BUFSIZE];
    Char                 fileName[MAXLENGTH_FILENAME], *area;
    NNshSubjectDatabase  subjDB;
    ControlType         *selP;
    FieldAttrType        attr;
    FieldType           *fldP;
    NNshMessageIndex    *idxP;
#ifdef USE_TRUNCATE_TITLE
    UInt32               offset;
    UInt16               len;
    Coord                width;
    RectangleType        r;
#endif
#ifdef USE_COLOR
    IndexedColorType savedIndex;
    RGBColorType     drawColor;

    // 参照画面のオープン
    NNsh_DebugMessage(ALTID_INFO, "OpenForm_NNshMessage()", "", 0);

    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
#if USE_DEBUG_ROM
        FrmDrawForm(FrmGetActiveForm());
#endif

        // 現在の色(前景色と文字色)を保存する
        savedIndex = UIColorGetTableEntryIndex(UIObjectForeground);
        WinIndexToRGB(savedIndex, &(NNshGlobal->fgColor));
        savedIndex = UIColorGetTableEntryIndex(UIFieldText);
        WinIndexToRGB(savedIndex, &(NNshGlobal->txColor));

        // 現在の色(背景色：フィールド、フォーム、ダイアログ等)を保存する
        savedIndex = UIColorGetTableEntryIndex(UIFieldBackground);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgColor));
        savedIndex = UIColorGetTableEntryIndex(UIFormFill);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgFormColor));

        savedIndex = UIColorGetTableEntryIndex(UIDialogFill);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgDialogColor));

        savedIndex = UIColorGetTableEntryIndex(UIAlertFill);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgAlertColor));

        savedIndex = UIColorGetTableEntryIndex(UIObjectFill);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgObjectColor));

        // 文字の色を変換する
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorViewFG,    &drawColor);
        UIColorSetTableEntry(UIObjectForeground, &drawColor);
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorViewFG,    &drawColor);
        UIColorSetTableEntry(UIFieldText,        &drawColor);

        // 背景の色を指定された色に設定する
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorViewBG,    &drawColor);
        UIColorSetTableEntry(UIFieldBackground,  &drawColor);
        UIColorSetTableEntry(UIFormFill,         &drawColor);
        UIColorSetTableEntry(UIDialogFill,       &drawColor);
        UIColorSetTableEntry(UIAlertFill,        &drawColor);
        UIColorSetTableEntry(UIObjectFill,       &drawColor);

        // 前景色と背景色を設定する
        WinSetForeColor(UIColorGetTableEntryIndex(UIFieldText));
        WinSetTextColor(UIColorGetTableEntryIndex(UIFieldText));
        WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));
    }
#endif

    // スレ情報をまず取得
    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // subjectデータベースからデータ取得に失敗
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        goto FUNC_END;
    }

    // 一時バッファ領域がすでに確保されていたら解放し、再度とりなおす。
    if (NNshGlobal->msgIndex != NULL)
    {
        MEMFREE_PTR((NNshGlobal->msgIndex)->msgOffset);
        idxP = NNshGlobal->msgIndex;
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(NNshGlobal->msgIndex);
    }
    bufferSize = sizeof(NNshMessageIndex) + (NNshGlobal->NNsiParam)->bufferSize + MARGIN * 2;

    // memory leakが心配なので一応確保前にfree testをしておく
    MEMFREE_PTR(NNshGlobal->msgIndex);
    NNshGlobal->msgIndex = MEMALLOC_PTR(bufferSize);

    if (NNshGlobal->msgIndex == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR, MSG_ERR_MEMPTR_NEW_BUFF, " size:",
                           bufferSize);
        ret = ~errNone;
        goto FUNC_END;
    }
    MemSet(NNshGlobal->msgIndex, bufferSize, 0x00);

    // メッセージオフセット領域を確保する
    bufferSize = sizeof(UInt32) * ((NNshGlobal->NNsiParam)->msgNumLimit + MARGIN);
    (NNshGlobal->msgIndex)->msgOffset = MEMALLOC_PTR(bufferSize);
    if ((NNshGlobal->msgIndex)->msgOffset == NULL)
    {
        // メッセージオフセット領域の確保失敗
        MEMFREE_PTR(NNshGlobal->msgIndex);
        NNsh_InformMessage(ALTID_ERROR, MSG_ERR_MEMPTR_NEW_BUFF, " size:",
                           bufferSize);
        ret = ~errNone;
        goto FUNC_END;
    }
    MemSet((NNshGlobal->msgIndex)->msgOffset, bufferSize, 0x00);

    // BBSタイプをワーク領域にコピー
    idxP = NNshGlobal->msgIndex;
    idxP->bbsType = (UInt16) subjDB.bbsType;

    // タイトルとボードニックネーム、ボードＩＤをワーク領域にコピー
    StrNCopyHanZen(idxP->threadTitle, subjDB.threadTitle, MAX_THREADNAME - 1);
    StrNCopy(idxP->boardNick,   subjDB.boardNick,   MAX_NICKNAME - 1);
    StrNCopy(idxP->threadFile,  subjDB.threadFileName, MAX_THREADFILENAME - 1);

    // 読み込むファイルの初期化
    MemSet (fileName, sizeof(fileName), 0x00);

    // OFFLINEスレのときは、boardNickを付加しないようにする
    if (StrCompare(subjDB.boardNick, OFFLINE_THREAD_NICK) != 0)
    {
        StrCopy(fileName, subjDB.boardNick);
    }
    else
    {
        // OFFLINEスレがVFSに格納されている場合には、ディレクトリを付加する。
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
        {
            // ログベースディレクトリを付加する
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

            // さらにサブディレクトリを検索して付加する
            GetSubDirectoryName(subjDB.dirIndex, &fileName[StrLen(fileName)]);
        }
    }
    StrCat (fileName, subjDB.threadFileName);

    // ファイルのありかで、開くモードを切り替える
    if (subjDB.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        fileMode = ((NNSH_FILEMODE_TEMPFILE)|(NNSH_FILEMODE_READONLY));
    }
    else
    {
        fileMode = (NNSH_FILEMODE_READONLY);
    }
    ret = OpenFile_NNsh(fileName,fileMode,&(idxP->fileRef));
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "Message File Open :", fileName, ret);

        // リファレンスのコピーでFreeは駄目!  
        // メモリの領域開放は、たぶんこれが正解のはず、、、
        MEMFREE_PTR(NNshGlobal->msgIndex);
        goto FUNC_END;
    }
    GetFileSize_NNsh(&(idxP->fileRef), &(idxP->fileSize));

    //  メッセージスレッドのインデックスを作成する
    ret = CreateMessageThreadIndex(idxP, &(idxP->nofMsg), idxP->msgOffset,
                                   (UInt16) subjDB.bbsType);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "ERR>index creation failure", "", ret);
        goto FUNC_END;
    }

    // アンダーラインの表示設定
    MemSet(&attr, sizeof(FieldAttrType), 0x00);
    fldP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
    FldGetAttributes(fldP, &attr);
    if ((NNshGlobal->NNsiParam)->disableUnderline != 0)
    {
        attr.underlined = noUnderline;
    }
    else
    {
        attr.underlined = grayUnderline;
    }
    FldSetAttributes(fldP, &attr);

    // フォームタイトルの更新(メッセージ数を格納)
    StrCopy(title, "1:");
    NUMCATI(title, idxP->nofMsg);
    FrmCopyTitle(frm, title);

    // スレタイトル表示領域にスレ名称をコピーする
    StrCopy(idxP->showTitle, idxP->threadTitle);

    // HandEra対策、、、スレタイTruncateはコンパイルスイッチ指定時のみ
#ifdef USE_TRUNCATE_TITLE
#ifdef USE_GLUE
    len   = StrLen(idxP->threadTitle);
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, SELTRID_THREAD_TITLE), &r);
    width = r.extent.x - 4;
    if (FntCharsWidth(idxP->threadTitle, len) >= width)
    {
        // スレタイトルが表示幅に表示できない場合、末尾に省略記号をつける
        width  = width - FntCharWidth(chrEllipsis);
        offset = FntWidthToOffset(idxP->showTitle, len, width, NULL, NULL);
        offset = TxtGlueGetTruncationOffset(idxP->showTitle, offset);
        idxP->showTitle[offset] = chrEllipsis;
        idxP->showTitle[offset + 1] = '\0';
    }
#endif
#endif
    // スレタイトルの表示
    selP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SELTRID_THREAD_TITLE));
    if (selP != NULL)
    {
        CtlSetLabel(selP, idxP->showTitle);
    }

    // 読み出し箇所の確認(異常なら先頭から)
    if ((subjDB.currentLoc < 1)||(subjDB.currentLoc > idxP->nofMsg))
    {
        idxP->openMsg = 1;
    }
    else
    {
        idxP->openMsg = subjDB.currentLoc;
    }
    idxP->prevMsg[0] = idxP->openMsg;
    idxP->histIdx    = 0;

    // とりあえずファイルの先頭からデータを読み込んでおく
    switch (ret)
    {
        case errNone:
        case vfsErrFileEOF:
        case fileErrEOF:
          // read OK, create Index!
          ret = errNone;
          break;

        default:
          goto FUNC_END;
          break;
    }

    // しおりが設定されている場合は、その値を戻す。
    if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
    {
        if ((NNshGlobal->NNsiParam)->bookMsgIndex == (NNshGlobal->NNsiParam)->openMsgIndex)
        {
            (NNshGlobal->msgIndex)->openMsg = (NNshGlobal->NNsiParam)->bookMsgNumber;
        }
    }

    // 「お気に入り」設定情報をセレクタトリガのラベルに設定する
    setFavoriteLabel(frm, subjDB.msgAttribute);

    // 「スレ参照画面でフォーカス移動しない」にチェックがなかった場合
    if ((NNshGlobal->NNsiParam)->notFocusNumField == 0)
    {
        // フォーカスをメッセージ番号フィールドに設定
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE_NUMBER));
    }

    // 検索モードチェックボックスの設定
    CtlSetValue(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_SEARCH)), 
                (NNshGlobal->NNsiParam)->searchMode);

    // NGワード設定をスレ毎に設定する設定
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD_EACHTHREAD) == NNSH_USE_NGWORD_EACHTHREAD)
    {
#if 0
        if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
        {
            // NG設定-3の設定をONにする。。。
            subjDB.msgAttribute  = (subjDB.msgAttribute | NNSH_MSGATTR_NGWORDSET);
        }
        else
        {
            // NG設定-3の設定をクリアする。。。
            subjDB.msgAttribute  = (subjDB.msgAttribute & NNSH_MSGATTR_CLRNGWORD);
        }
#endif

        // スレに格納されたNG設定を読み出す...
        if (((subjDB.msgAttribute)&(NNSH_MSGATTR_NGWORDSET)) == 0)
        {
            // NG3 ON → OFF
            (NNshGlobal->NNsiParam)->hideMessage = ((NNshGlobal->NNsiParam)->hideMessage & (NNSH_RESETMASK_NGWORD3));
        }
        else
        {
            // NG3 OFF → ON
            (NNshGlobal->NNsiParam)->hideMessage = ((NNshGlobal->NNsiParam)->hideMessage | (NNSH_USE_NGWORD3));
        }
    }
    
    // 画面表示
    (void) displayMessage(NNshGlobal->msgIndex, 0);
    (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_MESSAGE;
    return (errNone);

FUNC_END:
    // エラー終了する
    ReadMessage_Close(NNshGlobal->backFormId);
    return (ret);
}

/*=========================================================================*/
/*   Function : GetSubDirectoryName                                        */
/*                                                  サブディレクトリの取得 */
/*=========================================================================*/
Err GetSubDirectoryName(UInt16 index, Char *dirName)
{
    DmOpenRef              dirRef;
    UInt16                 size, nextIndex;
    Int16                  depth, saveDepth;
    Err                    ret;
    Char                  *buffer;
    NNshDirectoryDatabase  dirDb;

    if (index == 0)
    {
        // 指定INDEXが異常
        NNsh_DebugMessage(ALTID_INFO,
                          "There is not any sub-directory.", "", index);
        return (~errNone - 2);
    }

    OpenDatabase_NNsh(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, &dirRef);

    // サブディレクトリトップを取得する
    MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
    ret = SearchRecord_NNsh(dirRef, &index, NNSH_KEYTYPE_UINT16, 0,
                            sizeof(NNshDirectoryDatabase), &dirDb, &saveDepth);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_INFO, "SearchRecord_NNsh() ret:", "",   ret);
        NNsh_DebugMessage(ALTID_INFO, " index:", "", index);

        GetDBCount_NNsh(dirRef, &size);
        for (nextIndex = 0; nextIndex < size; nextIndex++)
        {
            MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
            ret = GetRecord_NNsh(dirRef, nextIndex,
                                 sizeof(NNshDirectoryDatabase), &dirDb);
            NNsh_DebugMessage(ALTID_INFO, "GetRecord_NNsh() ret:", "", ret);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " INDEX:", nextIndex);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " dirIdx:", dirDb.dirIndex);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " prevdx:", dirDb.previousIndex);
        }

        // DBレコード確保エラー
        CloseDatabase_NNsh(dirRef);
        return (ret);
    }
    NNsh_DebugMessage(ALTID_INFO, "depth:", dirDb.dirName, dirDb.depth);

    // 一時的に領域を確保する
    size = (dirDb.depth + 1) * sizeof(Char) * MAX_DIRNAME + MARGIN;
    buffer = MEMALLOC_PTR(size);
    if (buffer == NULL)
    {
        // 領域確保エラー
        CloseDatabase_NNsh(dirRef);
        return (~errNone);
    }
    MemSet(buffer, size, 0x00);

    // サブディレクトリをdirDBから検索
    saveDepth = dirDb.depth - 1;
    for (depth = saveDepth; depth >= 0; depth--)
    {
        NNsh_DebugMessage(ALTID_INFO, "copy ", dirDb.dirName, depth);

        // ディレクトリ名をコピーする
        StrNCopy(&buffer[depth * sizeof(Char) * MAX_DIRNAME],
                 dirDb.dirName, (MAX_DIRNAME - 1));
        if (dirDb.previousIndex == 0)
        {
            // 検索終了
            break;
        }

        // 次ディレクトリを検索
        nextIndex = dirDb.previousIndex;
        MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
        ret = SearchRecord_NNsh(dirRef, &(nextIndex), NNSH_KEYTYPE_UINT16, 0,
                                sizeof(NNshDirectoryDatabase), &dirDb, &index);
        if (ret != errNone)
        {
            // 検索失敗、次にすすむ
            NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh()", "", ret);
            break;
        }
    }

    // ディレクトリ名をコピー
    for (depth = 0; depth <= saveDepth; depth++)
    {
        StrCat(dirName, &buffer[depth * sizeof(Char) * MAX_DIRNAME]);
    }
    NNsh_DebugMessage(ALTID_INFO, "SubDirectory:", dirName, depth);

    // あとしまつ
    MEMFREE_PTR(buffer);
    CloseDatabase_NNsh(dirRef);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : PartReceiveMessage_View                                    */
/*                                                            スレ差分取得 */
/*-------------------------------------------------------------------------*/
static Err PartReceiveMessage_View(void)
{
    Err                  ret;
    Char                 url[BUFSIZE];
    UInt16               index;
    NNshSubjectDatabase  subjDB;
    NNshBoardDatabase    bbsData;
    NNshMessageIndex    *idxP;

    // メッセージインデックス
    idxP = NNshGlobal->msgIndex;

    // メッセージ情報をデータベースから取得
    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()", "", ret);
        return (ret);
    }

    // OFFLINEスレが選択された場合、更新不可を表示
    if (StrCompare(subjDB.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           idxP->threadTitle, 0);
        return (~errNone);
    }
    
    ret = Get_BBS_Database(subjDB.boardNick, &bbsData, &index);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          subjDB.boardNick, ret);
        return (ret);
    }

    switch (subjDB.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        // まちBBSに対して差分取得を選択したが、現在サポートなし(とりあえず?)
        NNsh_ErrorMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI, "", 0);
        return (~errNone);
        break;
#if 0
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_SHITARABA:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // なにもしない
        break;
    }

    //  取得予約機能が有効でかつネットに接続されていない場合、
    // 取得予約するか確認する
    if (((NNshGlobal->NNsiParam)->getReserveFeature != 0)&&(NNshGlobal->netRef == 0))
    {
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                NNSH_MSGINFO_RESERVE, "", 0) == 0)
        {
            // 新着予約時に取得予約を設定して終了する
            subjDB.msgAttribute
                     = subjDB.msgAttribute | NNSH_MSGATTR_GETRESERVE;

            (void) update_subject_database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
            return (errNone);
        }
    }

    // 現在参照中のファイルを一旦クローズしておく
    CloseFile_NNsh(&(idxP->fileRef));

    // メッセージ取得先の作成
    (void) CreateThreadURL(NNSH_DISABLE, url, sizeof(url), &bbsData, &subjDB);

    // スレ差分取得を行う
    (void) Get_PartMessage(url, bbsData.boardNick, &subjDB, 
                           (NNshGlobal->NNsiParam)->openMsgIndex, NULL);

    // 表示しなおし
    ReadMessage_Close(FRMID_MESSAGE);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : NNsh_Search_Message                                        */
/*                                                            スレ内検索   */
/*-------------------------------------------------------------------------*/
static void NNsh_Search_Message(UInt16 type, Char *str, Boolean flag)
{
    NNshMessageIndex *idxP;
    UInt16            start, end;
    Int16             step, len, msg;
    Int16             cmpLen;
    UInt32            offset;
    Char             *ptr;
    Char              backup;
    FormType         *frm;
    FieldType        *fld;
    Char             *targetString = NULL;
    UInt8             caselessExchange = 0;

    // 文字列がなければ終了
    if (StrLen(str) == 0)
    {
        return;
    }

    // メッセージインデックス
    idxP = NNshGlobal->msgIndex;
 
    // start, end は 1 origin で考える
    switch(type)
    {
      case NNSH_SEARCH_METHOD_FROM_TOP:  //先頭から検索
        start = 1;
        end   = idxP->nofMsg + 1;
        step  = 1;
        break;

      case NNSH_SEARCH_METHOD_NEXT:    //次検索
        start = idxP->openMsg + 1;
        end   = idxP->nofMsg + 1;
        step  = 1;
        break;

      case NNSH_SEARCH_METHOD_PREV: //前検索
        start = idxP->openMsg - 1;
        end   = 0;
        step  = -1;
        break;

      case NNSH_SEARCH_METHOD_FROM_BOTTOM: //末尾から検索
        start = idxP->nofMsg;
        end   = 0;
        step  = -1;
        break;

      default:
        return;
        break;
    }
    cmpLen = StrLen(str);

    // 「検索中」表示 (hideBusyForm()まで)
    Show_BusyForm(MSG_FINDING_WAIT);

    // 検索する文字列を大小文字変換
    targetString = NULL;
    caselessExchange = 0;
    if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
    {
        targetString = MEMALLOC_PTR(cmpLen + MARGIN);
        if (targetString != NULL)
        {
            (void) StrToLower(targetString, str);
            caselessExchange = 1;
        }
        else
        {
            // メモリ確保失敗
            NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " SIZE:", cmpLen);
        }
    }

    // 文字列が設定されていない場合...
    if (targetString == NULL)
    {
        targetString = str;
    }

    for (msg = start; msg != end; msg += step)
    {
        Char *buf = NULL;
        NNsh_Get_MessageToBuf(idxP, msg, 
                         (step > 0) ? GET_MESSAGE_TYPE_TOP : GET_MESSAGE_TYPE_BOTTOM, false);

        // バッファ内の offsetを調べる
        offset = (idxP->msgOffset)[msg - 1] - idxP->fileOffset;
        ptr = idxP->buffer + offset;
        len = (idxP->msgOffset)[msg] - (idxP->msgOffset)[msg - 1];

        //メッセージの末尾が NULL terminate されていないため、
        //次のメッセージの先頭の１文字壊すのでバックアップ
        backup = *(ptr + len);
        *(ptr + len) = NULL;

        // 変換用バッファを用意する
        buf = MEMALLOC_PTR((len * 2) + MARGIN); // 2倍確保しておけば問題ないと思う
        if (buf == NULL)
        {
            // メモリ確保失敗
            NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                                   " size  ::", (len * 2) + MARGIN);
            Hide_BusyForm(true);

            //バックアップを元に戻す 
            *(ptr + len) = backup;
            goto SEARCH_END;
        }
        MemSet(buf, ((len * 2) + MARGIN), 0x00);
            
        // 表示用に変換させる
        displayMessageSub(idxP->bbsType, idxP->buffer, idxP->openMsg,
                          offset, len, buf, false);

        if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
        {
            // 大文字/小文字を気にしないモードのときは、、、
#ifdef USE_GLUE
            // 検索対象を小文字に変換...
            TxtGlueLowerStr(buf, StrLen(buf));
#else
            // たぶん、これで動くはず。。。
            (void) StrToLower(buf, buf);
#endif
        }

        // 文字列の照合を行う
        if (StrStr(buf, targetString) != 0)
        {
            // 検索(文字列の照合)成功
            Hide_BusyForm(true);

            // バックアップを元に戻す 
            *(ptr + len) = backup;

            if (idxP->openMsg != msg)
            {
                if (flag == true)
                {
                    idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                            (idxP->histIdx + 1) : 0;
                    idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
                    idxP->openMsg = (idxP->openMsg < idxP->nofMsg - 10) ?
                            (idxP->openMsg + 10) : idxP->nofMsg;
                }          
                idxP->openMsg = msg;
                (void) displayMessage(idxP, 0);
            }
            MEMFREE_PTR(buf);
            goto SEARCH_END;
        }
        MEMFREE_PTR(buf);

        //バックアップを元に戻す 
        *(ptr + len) = backup;
    }
    Hide_BusyForm(true);

    NNsh_ErrorMessage(ALTID_INFO, NNSH_MSGINFO_NOTFOUND, "", 0);

    // 表示を更新する(OS3.1デバイスでは真っ白になるため)
    frm = FrmGetActiveForm();
    FrmDrawForm(frm);
    if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
    {
        // ハイレゾフォント設定中の時には、フィールドの再描画を行う
        fld = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm,FLDID_MESSAGE));
        NNsi_HRFldDrawField(fld, 0);
    }

SEARCH_END:
    if (caselessExchange != 0)
    {
        // 大文字/小文字の変換を実施したとき...
        MEMFREE_PTR(targetString);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : NNsh_Get_MessageToBuf                                      */
/*                                                      メッセージの読込   */
/*-------------------------------------------------------------------------*/
static Err NNsh_Get_MessageToBuf(NNshMessageIndex *idxP, UInt16 msg, UInt16 type, Boolean flag)
{
    Err ret = errNone;
    UInt32       readSize, offset, setMsg, backMsg, size;
    Int32 i;

    offset = 0;
    // データは今読み込まれている領域にあるかチェックする。(現物合わせ...)
    if (((idxP->msgOffset)[msg - 1]  < idxP->fileOffset)||
        ((idxP->msgOffset)[msg] - 1  > idxP->fileOffset + idxP->fileReadSize))
    {
        // 現在、読み込まれているバッファにはデータがない場合。。。
        if ((idxP->msgOffset)[msg] < (NNshGlobal->NNsiParam)->bufferSize)
        {
            // ファイルの先頭から読み込む
            offset = 0;
        }
        else if ((idxP->msgOffset)[msg - 1] >
                                     (idxP->fileSize - (NNshGlobal->NNsiParam)->bufferSize))
        {
            // ファイルの末尾を読み込む
            offset = idxP->fileSize - (NNshGlobal->NNsiParam)->bufferSize;
        }
        else
        {
            switch (type) 
            {
              case GET_MESSAGE_TYPE_HALF:
                // 戻るメッセージ数を決定する。
                backMsg = (NNshGlobal->NNsiParam)->bufferSize / 1200;

                // (インデックスの先頭 - backMsg)から読み込む
                setMsg = (msg <= backMsg) ? 1 : (msg - backMsg);
                if ((idxP->msgOffset)[setMsg] > 10)
                {
                    offset = (idxP->msgOffset)[setMsg] - 10;  // 10はマージン(意味なし)
                }
                else
                {
                    offset = 0;
                }
                if ((offset + (NNshGlobal->NNsiParam)->bufferSize) < (idxP->msgOffset)[msg + 1])
                {
                    // backMsgメッセージ前が読み込み範囲外だった場合はその場所から
                    if ((idxP->msgOffset)[msg - 1] > 10)
                    {
                        offset = (idxP->msgOffset)[msg - 1] - 10; // 10はマージン(意味なし)
                    }
                    else
                    {
                        offset = 0;
                    }
                }
                break;
                
              case GET_MESSAGE_TYPE_TOP:
                offset = (idxP->msgOffset)[msg - 1];
                break;

              case GET_MESSAGE_TYPE_BOTTOM:
                size = 0;
                i = msg - 1;
                while(i > 0)
                {
                    if (size + (idxP->msgOffset)[i + 1] - (idxP->msgOffset)[i] >
                                                         (NNshGlobal->NNsiParam)->bufferSize)
                    {
                        break;
                    }
                    size = size + (idxP->msgOffset)[i + 1] - (idxP->msgOffset)[i];
                }
                offset = (idxP->msgOffset)[msg - 1] - size;
                break;
            }
        }
        
        if (flag == true)
        {
            // 「待て」表示 (hideBusyForm()まで)
            Show_BusyForm(MSG_READ_MESSAGE_WAIT);
        }

        // データをバッファに読み込む。
        ret = ReadFile_NNsh(&(idxP->fileRef), offset, (NNshGlobal->NNsiParam)->bufferSize,
                            &(idxP->buffer[0]), &readSize);
        if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
        {
            idxP->fileOffset   = offset;
            idxP->fileReadSize = readSize;
        }

        if (flag == true) 
        {
            Hide_BusyForm(true);
        }
    }
    return(ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : UpdateFieldRegion                                          */
/*                                            フィールドタップ位置の設定   */
/*-------------------------------------------------------------------------*/
void UpdateFieldRegion(void)
{
    FormPtr frm;
    RectangleType r;

    frm = FrmGetActiveForm();
    if(FrmGetFormId(frm) != FRMID_MESSAGE)
      return;

    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE), &r);
    
    NNshGlobal->fieldPos[0].startX = r.topLeft.x;
    NNshGlobal->fieldPos[0].startY = r.topLeft.y;
    NNshGlobal->fieldPos[0].endX   = r.topLeft.x + r.extent.x;
    NNshGlobal->fieldPos[0].endY   = r.topLeft.y + r.extent.y / 2;

    NNshGlobal->fieldPos[1].startX = r.topLeft.x;
    NNshGlobal->fieldPos[1].startY = r.topLeft.y + r.extent.y / 2;
    NNshGlobal->fieldPos[1].endX   = r.topLeft.x + r.extent.x;
    NNshGlobal->fieldPos[1].endY   = r.topLeft.y + r.extent.y;
}

/*-------------------------------------------------------------------------*/
/*   Function : getViewDataString                                          */
/*                              表示中スレデータから指定されたデータを取得 */
/*-------------------------------------------------------------------------*/
static void getViewDataString(Char *buffer, Char *headerStr, UInt16 bufSize)
{
    FormType  *frm;
    FieldType *fldP;
    MemHandle txtH;
    Char      *str, *ptr;
    UInt16     copySize;

    // 抽出文字列の初期化
    MemSet(buffer, bufSize, 0x00);
    
    // フィールドから文字列を取得
    frm     = FrmGetActiveForm();
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
    txtH    = FldGetTextHandle(fldP);
    if (txtH == 0)
    {
         // メモリハンドル取得失敗
         return;
    }
    str = MemHandleLock(txtH);
    if (str == NULL)
    {
        // メモリハンドルロック失敗
        return;
    }

    ptr = StrStr(str, headerStr);
    if (ptr != NULL)
    {
        // 文字列をバッファサイズ分抽出(スペース, 改行...を区切りにする)
        copySize = 0;
        while ((*ptr > ' ')&&(copySize < (bufSize - 1)))
        {
             // 文字列をコピー
             buffer[copySize] = *ptr;
             ptr++;
             copySize++;
        } 
    }
    MemHandleUnlock(txtH);
}

/*-------------------------------------------------------------------------*/
/*   Function : executeViewAction                                          */
/*                                            機能を実行する(ハードキー用) */
/*-------------------------------------------------------------------------*/
static Boolean executeViewAction(UInt16 funcID)
{
    Char                 logMsg[BUFSIZE * 2], *txtP;
    Boolean              clip, err, ret = false;
    UInt16               item, frmId, start, end;
    FormType            *frm;
    FieldType           *fld;
    ControlType         *ctlP;
    NNshSubjectDatabase  subjDB;
    NNshMessageIndex    *idxP;
    NNshFileRef          fileRef;
    MemHandle            txtH;
    UInt32               size;
#ifdef USE_HANDERA
    VgaScreenModeType   handEraScreenMode;     // スクリーンモード
    VgaRotateModeType   handEraRotateMode;     // 回転モード
#endif

    // indexを変数に入れる
    idxP = NNshGlobal->msgIndex;

    // インデックスがない場合には、すぐにreturnする
    if (NNshGlobal->msgIndex == NULL)
    {
        return (false);
    }

    // 機能コマンド番号をjumpSelectionに格納(あんまり良くない...。)
    NNshGlobal->jumpSelection = funcID;
    switch (funcID)
    {
      case MULTIVIEWBTN_FEATURE_INFO:
        // "スレ情報"
        item = 0;
        DisplayMessageInformations((NNshGlobal->NNsiParam)->openMsgIndex, &item);

        // お気に入り情報ラベルを更新
        setFavoriteLabel(FrmGetActiveForm(), item);

        // 表示を更新する(OS3.1デバイスでは真っ白になるため)
        FrmDrawForm(FrmGetActiveForm());
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_BACKTOLIST:
        // "一覧画面へ"
        ReadMessage_Close(NNshGlobal->backFormId);
        break;


      case MULTIVIEWBTN_FEATURE_REPLYMSG:
        // 引用＆書き込み  (not BREAK!!)
        StrCopy(logMsg, ">>");
        NUMCATI(logMsg, idxP->openMsg);
        StrCat(logMsg, "\n");
        outputToFile(FILE_WRITEMSG, (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE), true, 3, logMsg);
        // break;

      case MULTIVIEWBTN_FEATURE_WRITE:
        // "書き込み"
        if (StrCompare(idxP->boardNick, OFFLINE_THREAD_NICK) != 0)
        {
            // OFFLINEスレを参照中でない場合には、書き込み画面へ遷移
            ReadMessage_Close(FRMID_WRITE_MESSAGE);
            break;
        }

        // 参照ログの場合には､URL選択表示を行う。
        if (NNshGlobal->browserCreator == 0)
        {
            // Web Browser未インストール時には、"現在サポートしていません" 表示を行う
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "(WRITE)", 0);
            break;
        }

        // アンカー一覧をピックアップする
        MemSet(logMsg, sizeof(logMsg), 0x00);
        ret = pickupAnchor(NNshGlobal->msgIndex, logMsg, sizeof(logMsg));
        if (ret == true)
        {
            // 指定されたURLを開くNetFrontをNNsi終了後に起動する
            (void) WebBrowserCommand(NNshGlobal->browserCreator, 0, 0, 
                                     NNshGlobal->browserLaunchCode,
                                     logMsg, StrLen(logMsg), NULL);
        }
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_GETPART:
        // "差分取得"
        (void) PartReceiveMessage_View();
        break;

      case MULTIVIEWBTN_FEATURE_GETMESNUM:
        // "スレ番指定取得"
        clip  = false;

        //////// 文字列が選択状態だったら、選択文字列をクリップボードにコピー
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId != noFocus)
        {        
            if (FrmGetObjectType(frm, frmId) == frmFieldObj)
            {
                fld = FrmGetObjectPtr(frm, frmId);
                FldGetSelection(fld, &start, &end);
                if (start != end)
                {
                    // 選択文字列をコピーする
                    FldCopy(FrmGetObjectPtr(frm, frmId));
                    if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
                    {
                        // ハイレゾフォント設定中の時にはフィールド再描画を行う
                        NNsi_HRFldDrawField(FrmGetObjectPtr(frm, frmId), 0);
                    }
                    clip = true;
                }
            }
        }

        // クリップボードの文字列を使わない場合には、フィールドからデータを取得
        if (clip != true)
        {
            getViewDataString(logMsg, "/read.cgi/", sizeof(logMsg));
            if (logMsg[0] == '/')
            {
                // '/read.cgi/xxxx/yyyyyyyy' の xxxx/yyyyyyyy 部分を抽出する
                txtP = logMsg + sizeof("/read.cgi/") - 1;
                while (*txtP != '\0')
                {
                    if (*txtP == '/')
                    {
                        //
                        if (clip == true)
                        {
                            *txtP = '\0';
                            break;
                        }
                        clip = true;
                    }
                    txtP++;
                }

                // 検出した文字列をクリップボードにコピーする
                txtP = logMsg + sizeof("/read.cgi/") - 1;
                ClipboardAddItem(clipboardText, txtP, (StrLen(txtP) + 1));
            }
        }

        // スレ番指定取得を実施する
        ret = Get_MessageFromMesNum(&err, &item, clip);
        if (err == true)
        {
            // 正常終了、そのときはBBS一覧を初期化しておく
            MEMFREE_PTR(NNshGlobal->bbsTitles);
        }
        // スレ番指定取得を実行したとき
        if (ret == true)
        {
            // スレ番指定で今開いているレコードの番号が変わったか確認する。
            MemSet(&subjDB, sizeof(subjDB), 0x00);
            (void) Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
            if (StrCompare(subjDB.threadTitle, idxP->threadTitle) != 0)
            {
                // スレレコード番号が(スレ番指定取得によって)変わった！
                // (レコード番号を１つ増やす)
                ((NNshGlobal->NNsiParam)->openMsgIndex)++;
                ((NNshGlobal->NNsiParam)->startTitleRec)++;
                ((NNshGlobal->NNsiParam)->endTitleRec)++;
            }

            // 取得予約モードでないとき、スレ取得後、スレを開くかどうか確認する
            if ((NNshGlobal->NNsiParam)->getReserveFeature == 0)
            {
                if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                        MSG_OPEN_CONFIRM_MESNUM, "", 0) == 0)
                {
                    // "OK"を選択した場合、そのスレを開く
                    // (参照画面を閉じて開きなおす) 
                    (NNshGlobal->NNsiParam)->openMsgIndex      = item;
                    (NNshGlobal->NNsiParam)->startTitleRec     = item;
                    (NNshGlobal->NNsiParam)->endTitleRec       = item;
                    (NNshGlobal->NNsiParam)->selectedTitleItem = 0;
                    ReadMessage_Close(FRMID_MESSAGE);

                    // (一覧画面のBBS一覧は「取得済み全て」にする)
//                    (NNshGlobal->NNsiParam)->lastBBS = NNSH_SELBBS_GETALL;
                    return (true);
                }
                else
                {
                    // スレ番指定取得したレスを開かない場合は、再描画
                    (void) displayMessage(NNshGlobal->msgIndex, 0);
                }
            }
        }
        else
        {
            // OS3.1デバイスの時にキャンセルされると、画面の再描画が必要
            frm = FrmGetActiveForm();
            FrmDrawForm(frm);
            if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
            {
                // ハイレゾフォント設定中の時には、フィールドの再描画を行う
                fld = FrmGetObjectPtr(frm,
                                         FrmGetObjectIndex(frm,FLDID_MESSAGE));
                NNsi_HRFldDrawField(fld, 0);
            }
        }
        return (false);
        break;

      case MULTIVIEWBTN_FEATURE_GOTOTOP:
        // "先頭MSGへ"
        jumpEdge(false);
        break;

      case MULTIVIEWBTN_FEATURE_PREVIOUS:
        // "前MSGへ"
        (void) moveMessagePrev(false, 0);
        break;

      case MULTIVIEWBTN_FEATURE_PAGEUP:
        // "前スクロール"
        (void) moveMessagePrev(true, (NNshGlobal->NNsiParam)->dispBottom);
        break;

      case MULTIVIEWBTN_FEATURE_PAGEDOWN:
        // "後スクロール"
        (void) moveMessageNext(true);
        break;

      case MULTIVIEWBTN_FEATURE_NEXT:
        // "次MSGへ"
        (void) moveMessageNext(false);
        break;

      case MULTIVIEWBTN_FEATURE_GOTOBOTTOM:
        // "末尾MSGへ"
        jumpEdge(true);
        break;

      case MULTIVIEWBTN_FEATURE_JUMP:
        // "Go"
        jumpMessage(FrmGetActiveForm(), NNshGlobal->msgIndex);
        if (NNshGlobal->work3 != 0)
        {
            // レスpop up、stackが深すぎて動かない?ためここに移動
            popUP_Message(NNshGlobal->work3);
            NNshGlobal->work3 = 0;
        }
        break;

      case MULTIVIEWBTN_FEATURE_RETURN:
        // "１つ戻る"
        jumpPrevious();
        break;

      case MULTIVIEWBTN_FEATURE_COPY:
        // "文字列コピー"
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId == noFocus)
        {        
           // フォーカスが取得できない、終了する
            break;
        }
        if (FrmGetObjectType(frm, frmId) == frmFieldObj)
        {
            FldCopy(FrmGetObjectPtr(frm, frmId));
            if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
            {
                // ハイレゾフォント設定中の時には、フィールドの再描画を行う
                NNsi_HRFldDrawField(FrmGetObjectPtr(frm, frmId), 0);
            }
        }
        break;

      case MULTIVIEWBTN_FEATURE_SELECTALL:
        // "文字列すべて選択"
        frm = FrmGetActiveForm();
        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        FldSetSelection(fld, 0, FldGetTextLength(fld));
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
        {
            // ハイレゾフォント設定中の時には、フィールドの再描画を行う
            NNsi_HRFldDrawField(fld, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_SELANDWEB:
        // 文字列すべて選択後、Web Browserで開く
        frm = FrmGetActiveForm();
        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        FldSetSelection(fld, 0, FldGetTextLength(fld));
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        // not break; //

      case MULTIVIEWBTN_FEATURE_OPENWEB:
        // "文字列をWeb"
        if (NNshGlobal->browserCreator == 0)
        {
            // "現在サポートしていません" 表示を行う
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
            break;
        }

        // 選択文字列を使って、NetFrontで開く
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId == noFocus)
        {        
           // フォーカスが取得できない、終了する
            break;
        }
        if (FrmGetObjectType(frm, frmId) == frmFieldObj)
        {
            // 選択している文字列を一旦クリップボードにコピー
            FldCopy(FrmGetObjectPtr(frm, frmId));

            // Webブラウザで開く
            openWebBrowser((NNshGlobal->NNsiParam)->confirmationDisable, logMsg, sizeof(logMsg));
        }
        break;

      case MULTIVIEWBTN_FEATURE_OUTPUTMEMO:
        // "メモ帳出力"
        clip  = false;

        //////// 文字列が選択状態だったら、選択文字列をクリップボードにコピー
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId != noFocus)
        {        
            if (FrmGetObjectType(frm, frmId) == frmFieldObj)
            {
                fld = FrmGetObjectPtr(frm, frmId);
                FldGetSelection(fld, &start, &end);
                if (start != end)
                {
                    // 選択文字列をコピーする
                    FldCopy(FrmGetObjectPtr(frm, frmId));
                    if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
                    {
                        // ハイレゾフォント設定中の時にはフィールド再描画を行う
                        NNsi_HRFldDrawField(FrmGetObjectPtr(frm, frmId), 0);
                    }
                    clip = true;
                }
            }
        }
        // メモ帳に出力する
        outputMemoPad(clip);
        break;

      case MULTIVIEWBTN_FEATURE_SEARCH:
        // "検索"
        frm  = FrmGetActiveForm();
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_SEARCH));
        if ((NNshGlobal->NNsiParam)->searchMode == 1)
        {
            // 検索モードを抜ける
            (NNshGlobal->NNsiParam)->searchMode = 0;
            CtlSetValue(ctlP, 0);
            break;
        }
        // 検索ウィンドウに表示する文字列を表示中スレから抽出する場合...
        if ((NNshGlobal->NNsiParam)->enableSearchStrHeader != 0)
        {
            getViewDataString((NNshGlobal->NNsiParam)->searchStrMessage,
                              (NNshGlobal->NNsiParam)->viewSearchStrHeader,
                              (sizeof((NNshGlobal->NNsiParam)->searchStrMessage)));
        }
        if (DataInputDialog(MSG_DIALOG_SEARCH_MESSAGE,
                            (NNshGlobal->NNsiParam)->searchStrMessage, 
                            sizeof((NNshGlobal->NNsiParam)->searchStrMessage), 
                            NNSH_DIALOG_USE_SEARCH_MESSAGE, 
                            (void *)&item) == true)
        {
            CtlSetValue(ctlP, 1);
            (NNshGlobal->NNsiParam)->searchMode = 1;
            NNsh_Search_Message(item, (NNshGlobal->NNsiParam)->searchStrMessage, true);
        }
        else
        {
            // 検索モードを抜ける
            (NNshGlobal->NNsiParam)->searchMode = 0;
            CtlSetValue(ctlP, 0);

            // OS3.1用では画面の再描画が必要
            FrmDrawForm(frm);
            if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
            {
                // ハイレゾフォント設定中の時には、フィールドの再描画を行う
                fld = FrmGetObjectPtr(frm,
                                      FrmGetObjectIndex(frm, FLDID_MESSAGE));
                NNsi_HRFldDrawField(fld, 0);
            }
        }
        break;

      case MULTIVIEWBTN_FEATURE_SEARCHNEXT:
        // "次検索"
        NNsh_Search_Message(NNSH_SEARCH_METHOD_NEXT, (NNshGlobal->NNsiParam)->searchStrMessage, false);
        break;

      case MULTIVIEWBTN_FEATURE_SEARCHPREV:
        // "前検索"
        NNsh_Search_Message(NNSH_SEARCH_METHOD_PREV, (NNshGlobal->NNsiParam)->searchStrMessage, false);
        break;

      case MULTIVIEWBTN_FEATURE_SEARCHTOP:
        // "先頭から検索"
        NNsh_Search_Message(NNSH_SEARCH_METHOD_FROM_TOP, (NNshGlobal->NNsiParam)->searchStrMessage, false);
        break;

      case MULTIVIEWBTN_FEATURE_SEARCHBTM:
        // "末尾から検索"
        NNsh_Search_Message(NNSH_SEARCH_METHOD_FROM_BOTTOM, (NNshGlobal->NNsiParam)->searchStrMessage, false);
        break;

      case MULTIVIEWBTN_FEATURE_CHANGEFONT:
        // "フォント変更"
        changeFont();
        break;

      case MULTIVIEWBTN_FEATURE_DRAWMODE:
        // "描画モード変更"
        if ((NNshGlobal->NNsiParam)->useSonyTinyFont == 0)
        {
            // ハイレゾモードへ変更
            (NNshGlobal->NNsiParam)->useSonyTinyFont = 1;
        }
        else
        {
            // 通常モードへ変更
            (NNshGlobal->NNsiParam)->useSonyTinyFont = 0;
        }
        ReadMessage_Close(FRMID_MESSAGE);
        return (true);
        break;

      case MULTIVIEWBTN_FEATURE_ROTATE:
        // "画面の回転(HE)"
#ifdef USE_HANDERA
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
            VgaSetScreenMode(screenMode1To1, 
                             VgaRotateSelect(handEraRotateMode));

            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
            return (false);
            break;
        }
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            // 画面回転を実行
            VgaFormRotate_DIA();

            frm = FrmGetActiveForm();
            // 画面回転後の大きさにあわせ、フォームを調整し描画する
            if (HandEraResizeForm(frm) == true)
            {
                FrmEraseForm(frm);
                NNsi_FrmDrawForm(frm, true);
            }
            return (false);
            break;
        }
#endif    // #ifdef USE_PIN_DIA

        // "現在サポートしていません" 表示を行う
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        break;

      case MULTIVIEWBTN_FEATURE_DISCONNECT:
        // "回線切断"
        NNshNet_LineHangup();
        break;

      case MULTIVIEWBTN_FEATURE_SETNETWORK:
        // "Network設定"
        OpenNetworkPreferences();
        break;

      case MULTIVIEWBTN_FEATURE_BOOKMARK:
        // "しおり設定"
        if ((NNshGlobal->NNsiParam)->useBookmark == 0)
        {
            // "現在サポートしていません" 表示を行う
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
            break;
        }

        // 現在の位置を記憶する
        (NNshGlobal->NNsiParam)->lastFrmID     = NNSH_FRMID_MESSAGE;
        (NNshGlobal->NNsiParam)->bookMsgIndex  = (NNshGlobal->NNsiParam)->openMsgIndex;
        (NNshGlobal->NNsiParam)->bookMsgNumber = idxP->openMsg;
        NNsh_InformMessage(ALTID_INFO, MSG_SET_BOOKMARK, "", 0);
        break;

      case MULTIVIEWBTN_FEATURE_DEVICEINFO:
        // "デバイス情報"
        ShowDeviceInfo_NNsh();
        break;

      case MULTIVIEWBTN_FEATURE_PAGEUP10:
        // 10レス前
        if (idxP->openMsg > 1)
        {
            // メッセージ番号を10減らし、画面表示。
            idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                                                 (idxP->histIdx + 1) : 0;
            idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
            idxP->openMsg = (idxP->openMsg > 11) ? (idxP->openMsg - 10) : 1;
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_PAGEDOWN10:
        // 10レス後ろ
        if (idxP->openMsg < idxP->nofMsg)
        {
            // メッセージ番号を10増やし、画面表示。
            idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                                                 (idxP->histIdx + 1) : 0;
            idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
            idxP->openMsg    = (idxP->openMsg < idxP->nofMsg - 10) ?
                                          (idxP->openMsg + 10) : idxP->nofMsg;
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_GOTODUMMY:
        // ダミー画面へ遷移させる
        ReadMessage_Close(FRMID_NNSI_DUMMY);
        break;

      case MULTIVIEWBTN_FEATURE_OPENURL:
        // WebBrowserでURLを選択する
        if (NNshGlobal->browserCreator != 0)
        {
            // アンカー一覧をピックアップする
            MemSet(logMsg, sizeof(logMsg), 0x00);
            if ((idxP->bbsType & NNSH_BBSTYPE_MASK) == NNSH_BBSTYPE_HTML)
            {
                // WebBrowserでURLを開く(HTMLのアンカーを抽出)
                ret = pickupAnchor(NNshGlobal->msgIndex, logMsg, sizeof(logMsg));
            }
            else
            {
                // WebBrowserでURLを開く(レス内のURLを抽出)
                ret = pickupAnchorURL(NNshGlobal->msgIndex, logMsg, sizeof(logMsg));
            }
            if (ret == true)
            {
                // 指定されたURLを開く
                // NetFrontを起動する(NNsi終了後に起動)
                (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                          0, 0,  NNshGlobal->browserLaunchCode,
                                          logMsg, StrLen(logMsg), NULL);
            }
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_EXECA2DA:
        // フィールドから、データを出力
        frm = FrmGetActiveForm();
        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        txtH    = FldGetTextHandle(fld);
        if (txtH == 0)
        {
            // フィールドのファイルハンドル取得失敗、、、終了する
            return (false);
        }
        txtP = MemHandleLock(txtH);

        // ファイルにデータを出力する
        ret = OpenFile_NNsh(FILE_SENDMSG,
                            (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                            &fileRef);
        if (ret == errNone)
        {
            (void) WriteFile_NNsh(&fileRef, 0, StrLen(txtP), txtP, &size);
            CloseFile_NNsh(&fileRef);
        }
        else
        {
            NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
        }
        MemHandleUnlock(txtH);

        // クリップボードに、a2DAに渡すデータについての指示を格納する。
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, A2DA_NNSIEXT_AADATASTART);
        StrCat (logMsg, A2DA_NNSIEXT_NNSICREATOR);
        StrCat (logMsg, A2DA_NNSIEXT_SRC_STREAM);
        StrCat (logMsg, A2DA_NNSIEXT_DATANAME);
        StrCat (logMsg, FILE_SENDMSG);
        StrCat (logMsg, A2DA_NNSIEXT_ENDDATANAME);
        StrCat (logMsg, A2DA_NNSIEXT_ENDDATA);
        ClipboardAddItem(clipboardText, logMsg, (StrLen(logMsg) + 1));

        // a2DAを起動する
        (void) LaunchResource_NNsh('DAcc','a2DA','code',1000);
        break;

      case MULTIVIEWBTN_FEATURE_BT_ON:
        // Bluetooth ON
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(true);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MULTIVIEWBTN_FEATURE_BT_OFF:
        // Bluetooth OFF
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(false);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MULTIVIEWBTN_FEATURE_USAGE:
        // 操作ヘルプの表示
        // (クリップボードに、nnDAに渡すデータについての指示を格納する)
        ClipboardAddItem(clipboardText, 
                         nnDA_NNSIEXT_VIEWSTART
                         nnDA_NNSIEXT_INFONAME
                         nnDA_NNSIEXT_HELPVIEW
                         nnDA_NNSIEXT_ENDINFONAME
                         nnDA_NNSIEXT_ENDVIEW,
                         sizeof(nnDA_NNSIEXT_VIEWSTART
                                 nnDA_NNSIEXT_INFONAME
                                 nnDA_NNSIEXT_HELPVIEW
                                 nnDA_NNSIEXT_ENDINFONAME
                                 nnDA_NNSIEXT_ENDVIEW));
        // nnDAを起動する
        (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        break;

      case MULTIVIEWBTN_FEATURE_MEMOURL:
        // URLをMemoURLへ送る
        if (CheckInstalledResource_NNsh('appl', 'asUR') == false)
        {
            // MemoURLがインストールされていなかった、何もせず抜ける
            return (false);
        }
        MemSet(logMsg, sizeof(logMsg), 0x00);
        logMsg[0] = '1';
        StrCopy(&logMsg[2], idxP->threadTitle); // スレタイをタイトルに...
        ret = pickupAnchorURL(NNshGlobal->msgIndex, 
                              &logMsg[StrLen(&logMsg[2]) + 3], 
                              (sizeof(logMsg) - (StrLen(&logMsg[2]) + 3) - 1));
        if (ret == true)
        {
            // 指定されたURLをMemoURLに送る
            (void) WebBrowserCommand('asUR', 0, 0, 0x8001, logMsg, sizeof(logMsg), &size);
        }
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_CHANGERAW:
        // RAWモードトグル切替
        if ((NNshGlobal->NNsiParam)->rawDisplayMode != 0)
        {
            // RAWモードOFF
            (NNshGlobal->NNsiParam)->rawDisplayMode = 0;
        }
        else
        {
            // RAWモードON
            (NNshGlobal->NNsiParam)->rawDisplayMode = 1;
        }
        // 表示しなおし
        // (void) displayMessage(NNshGlobal->msgIndex, 0);
        ReadMessage_Close(FRMID_MESSAGE);
        break;

#ifdef USE_LOGCHARGE
      case MULTIVIEWBTN_FEATURE_CHGSEPA:
        // 区切り文字変更
        size = 0;
        if (SelectLogToken(&size) == BTNID_JUMPEXEC)
        {
            // 区切りを変更(HTML形式のときのみ有効)
            NNsh_DebugMessage(ALTID_INFO, "Selected Token ID:", "", size);            
            NNshGlobal->tokenId = size;
        }
        // 表示しなおし
        ReadMessage_Close(FRMID_MESSAGE);
#endif  // #ifdef USE_LOGCHARGE
        break;

#ifdef USE_LOGCHARGE
      case MULTIVIEWBTN_FEATURE_EDITSEPA:
        // 区切り文字編集
        SetLogCharge_LogToken(0);
        // 参照画面を表示しなおし
        ReadMessage_Close(FRMID_MESSAGE);
        break;
#endif  // #ifdef USE_LOGCHARGE

      case MULTIVIEWBTN_FEATURE_OPENURL_HTTP:
        // WebBrowserでURLを選択する
        if (NNshGlobal->browserCreator != 0)
        {
            // アンカー一覧をピックアップする
            MemSet(logMsg, sizeof(logMsg), 0x00);

            // WebBrowserでURLを開く(レス内のURLを抽出)
            ret = pickupAnchorURL(NNshGlobal->msgIndex, logMsg, sizeof(logMsg));
            if (ret == true)
            {
                // 指定されたURLを開く
                // NetFrontを起動する(NNsi終了後に起動)
                (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                          0, 0,  NNshGlobal->browserLaunchCode,
                                          logMsg, StrLen(logMsg), NULL);
            }
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_MEMOURLDA:
        // URLをMemoURLDAへ送る
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, idxP->threadTitle); // スレタイをタイトルに...
        ret = pickupAnchorURL(NNshGlobal->msgIndex, 
                              &logMsg[StrLen(logMsg)], 
                              (sizeof(logMsg) - StrLen(logMsg) - 1));
        if (ret != true)
        {
            (void) displayMessage(NNshGlobal->msgIndex, 0);
            return (false);
        }

        // クリップボードに選択したURLをコピーする
        ClipboardAddItem(clipboardText, logMsg, StrLen(logMsg));

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " COPIED URL :%s", logMsg);
#endif // #ifdef USE_REPORTER

        if (CheckInstalledResource_NNsh('DAcc', 'mmuA') != false)
        {
            // MemoURLDAがインストールされていた場合...
            (void) LaunchResource_NNsh('DAcc','mmuA','code',1000);
        }
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_OPENWEBDA:
        // URLをwebDAで開く
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, idxP->threadTitle); // スレタイをタイトルに...
        ret = pickupAnchorURL(NNshGlobal->msgIndex, 
                              &logMsg[StrLen(logMsg)], 
                              (sizeof(logMsg) - StrLen(logMsg) - 1));
        if (ret != true)
        {
            (void) displayMessage(NNshGlobal->msgIndex, 0);
            return (false);
        }

#ifdef USE_LOGCHARGE
        start = StrLen(logMsg);
        if (((logMsg[start -1] == 'P')||(logMsg[start - 1] == 'p')||
             (logMsg[start -1] == 'G')||(logMsg[start - 1] == 'g')||
             (logMsg[start -1] == 'B')||(logMsg[start - 1] == 'b')||
             (logMsg[start -1] == 'C')||(logMsg[start - 1] == 'c'))&&
            ((logMsg[start -2] == 'P')||(logMsg[start - 2] == 'p')||
             (logMsg[start -2] == 'I')||(logMsg[start - 2] == 'i')||
             (logMsg[start -2] == 'D')||(logMsg[start - 2] == 'd')||
             (logMsg[start -2] == 'R')||(logMsg[start - 2] == 'r'))&&
            ((logMsg[start -3] == 'Z')||(logMsg[start - 3] == 'z')||
             (logMsg[start -3] == 'J')||(logMsg[start - 3] == 'j')||
             (logMsg[start -3] == 'P')||(logMsg[start - 3] == 'p'))&&
             (logMsg[start -4] == '.'))
        {
            // webDAではなく、ログ取得モードに切り替える
            //  (URLの末端が .jpg, .zip, .prc または .pdb のとき)
            GetImageFile(logMsg);

            // 画面を再描画する
            (void) displayMessage(NNshGlobal->msgIndex, 0);
            break;
        }
#endif	// #ifdef USE_LOGCHARGE

        // クリップボードに選択したURLをコピーする
        ClipboardAddItem(clipboardText, logMsg, StrLen(logMsg));

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " COPIED URL :%s", logMsg);
#endif // #ifdef USE_REPORTER

        if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
        {
            // webDAがインストールされていた場合...
            (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
        }
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

#ifdef USE_LOGCHARGE
      case MULTIVIEWBTN_FEATURE_GETIMAGE:
        // ファイルを /Palm/Imagesディレクトリに取得(or 参照専用ログに取得)
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, idxP->threadTitle); // スレタイをタイトルに...
        ret = pickupAnchorURL(NNshGlobal->msgIndex, 
                              &logMsg[StrLen(logMsg)], 
                              (sizeof(logMsg) - StrLen(logMsg) - 1));
        if (ret != true)
        {
            (void) displayMessage(NNshGlobal->msgIndex, 0);
            return (false);
        }

        // ファイルを取得する
        GetImageFile(logMsg);

        // 画面を再描画する
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;
#endif	// #ifdef USE_LOGCHARGE

      case MULTIVIEWBTN_FEATURE_DELETEBACK:
        // 削除後、一覧に戻る...
        // (一覧に戻す設定。。)
        // スレを本当に削除するか確認する
        item = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_DELETE_MSGCONFIRM, 
                                   idxP->threadTitle, 0);
        if (item != 0)
        {
            // 削除キャンセル
            return (false);
        }
        // 画面を閉じて一覧画面に戻る準備...
        ReadMessage_Close(NNshGlobal->backFormId);

        // スレ削除実処理...
        DeleteThreadMessage((NNshGlobal->NNsiParam)->openMsgIndex);
        return (false);
        break;

      case MULTIVIEWBTN_FEATURE_VIEWBEPROF:
        // Beプロフィール参照
        if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
        {
            // webDAがインストールされていた場合...
            // (Beプロフィールの抽出)
            ret = pickupBeProfile(NNshGlobal->msgIndex, 
                                  &logMsg[StrLen(logMsg)], 
                                  (sizeof(logMsg) - StrLen(logMsg) - 1));
            if (ret == true)
            {
                // クリップボードに選択したURLをコピーする
                ClipboardAddItem(clipboardText, logMsg, StrLen(logMsg));

                // webDAの起動
                (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
            }
            // レスの再表示
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        else
        {
            // webDAがインストールされていない場合。。。
            // ("webDAがない"表示を行う。)
            NNsh_InformMessage(ALTID_ERROR, MSG_NOTICE_WEBDA_INSTALL, "", 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_LVLUPDATE:
        // スレレベルの更新
        updateThreadLevel();
        break;

      case MULTIVIEWBTN_FEATURE_PICKUP_NG:
        // NGワードを抽出して登録
        MemSet (logMsg, sizeof(logMsg), 0x00);
        ret = pickupNGwordCandidate(NNshGlobal->msgIndex, logMsg, (NGWORD_LEN - 1));
        if (ret == true)
        {
            // NGキーワードを登録する
            entryPickupedNGword(logMsg);
        }

        // 画面を再描画する
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_ENTRY_NG:
        // NGワード設定
        ret = SetNGword3();

        if (ret == true)
        {
            // 参照画面を表示しなおし
            ReadMessage_Close(FRMID_MESSAGE);
        }
        else
        {
            // 画面を再描画する
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      default:
        // その他(何もしない)
        NNsh_DebugMessage(ALTID_WARN, "Unknown FuncID(view):", "", funcID);
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   entryPickupedNGword                                      */
/*                                                  抽出したNGワードの登録 */
/*-------------------------------------------------------------------------*/
static void entryPickupedNGword(Char *ngWord)
{
    NNshNGwordDatabase  dbData;
    MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
    StrNCopy(dbData.ngWord, ngWord, (NGWORD_LEN - 1));
    dbData.checkArea     = 0;  // レス全体でチェック
    dbData.matchedAction = 0;  // 透明あぼーん

    NNsh_DebugMessage(ALTID_WARN, "Entry NG WORD:", ngWord, 0);

    // NG-3データベースへのデータ登録
    //  (新規登録なので最初の引数は１、2番目の引数はN/A）
    (void) EntryNGword3(1, 0, &dbData);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlSelEvt_DispMessage                                    */
/*                                                      ボタン押下時の処理 */
/*-------------------------------------------------------------------------*/
Boolean ctlSelEvt_DispMessage(EventType *event)
{
    // インデックスがない場合には、すぐにreturnする
    if (NNshGlobal->msgIndex == NULL)
    {
        return (false);
    }

    // 押されたボタンによって条件分岐
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_TOP:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // 末尾から先頭に向かって検索
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHBTM);
        }
        else
        {
            // スレの先頭へ
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_GOTOTOP);
        }
        break;

      case BTNID_PREV_MESSAGE:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // 前検索
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHPREV);
        }
        else
        {
            // 前ボタン 
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PREVIOUS);
        }
        break;

      case BTNID_NEXT_MESSAGE:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // 次検索
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHNEXT);
        }
        else
        {
            // 次ボタン 
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_NEXT);
        }
        break;

      case BTNID_BOTTOM:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // 先頭から末尾に向かって検索
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHTOP);
        }
        else
        {
            // 末尾ボタン(スレの末尾へ)
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_GOTOBOTTOM);
        }
        break;

      case BTNID_BACK_MESSAGE:
        // 戻るボタン 
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_RETURN);
        break;

      case BTNID_JUMP:
        // GOボタン
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_JUMP);
        break;

      case BTNID_WRITE_MESSAGE:
        // 書込ボタン
        if ((NNshGlobal->NNsiParam)->writingReplyMsg == 0)
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_WRITE);
        }
        else
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_REPLYMSG);
        }
        break;

      case SELTRID_THREAD_TITLE:  
        // タイトルボタン(一覧画面に戻る)
        (void) executeViewAction((NNshGlobal->NNsiParam)->viewTitleSelFeature);
        break;

      case SELTRID_FAVOR_THREAD:
        // スレ情報を表示する
        (void) executeViewAction((NNshGlobal->NNsiParam)->viewMultiBtnFeature);
        break;

      case CHKID_SEARCH:
        // 検索モード ON/OFF
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCH);
        break;

      default:
        // 何もしない
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   MenuEvt_DispMessage                                      */
/*                                                    メニュー選択時の処理 */
/*-------------------------------------------------------------------------*/
Boolean menuEvt_DispMessage(EventType *event)
{
    switch (event->data.menu.itemID)
    {
      case MNUID_THREAD_SELECTION:
        // スレ選択画面に戻る
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_BACKTOLIST);
        break;

      case MNUID_THREAD_INFO:
        // スレ情報を表示
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_INFO);
        break;

      case MNUID_TOP_MESSAGE:
        // スレの先頭にジャンプ
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_GOTOTOP);
        break;

      case MNUID_BOTTOM_MESSAGE:
        // スレの末尾にジャンプ
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_GOTOBOTTOM);
        break;

      case MNUID_WRITE_MESSAGE:
        // スレ書き込み(OFFLINEスレでない時のみ書き込み可) ※ レス引用書き込みの設定と逆になる
        if ((NNshGlobal->NNsiParam)->writingReplyMsg == 0)
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_REPLYMSG);
        }
        else
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_WRITE);
        }
        break;

      case MNUID_GET_PART:
        // スレ差分取得
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_GETPART);
        break;

      case MNUID_GET_MESNUM:
        // スレ番指定のメッセージ取得
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_GETMESNUM);
        break;

      case MNUID_NET_DISCONN:
        // 回線切断
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_DISCONNECT);
        break;

      case MNUID_SET_BOOKMARK:
        // しおり設定
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_BOOKMARK);
        break;

      case MNUID_FONT_MSGVIEW:
        // フォント変更
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_CHANGEFONT);
        break;

      case MNUID_HANDERA_ROTATE_MSGVIEW:
        // HandEra 画面回転 //
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_ROTATE);
        break;

      case MNUID_CLIE_DRAW_MODE: 
        // 描画モード変更
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_DRAWMODE);
        return (true);
        break;

      case MNUID_OS5_LAUNCH_WEB:
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_OPENWEB);
        break;

      case MNUID_EDIT_COPY:
        // コピー
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_COPY);
        break;

      case MNUID_SELECT_ALL:
        // 文字列の全選択
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SELECTALL);
        break;

      case MNUID_NET_CONFIG:
        // ネットワーク設定を開く
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SETNETWORK);
        break;

      case MNUID_SEARCH:
        // 検索
        (NNshGlobal->NNsiParam)->searchMode = 0;
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCH);
        break;

      case MNUID_SEARCH_NEXT:
        // 次検索
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHNEXT);
        break;

      case MNUID_SEARCH_PREV:
        // 前検索
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHPREV);
        break;

      case MNUID_SEARCH_TOP_TO_BOTTOM:
        // 先頭から末尾検索
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHTOP);
        break;

      case MNUID_SEARCH_BOTTOM_TO_TOP:
        // 末尾から先頭検索
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHBTM);
        break;

      case MNUID_OUTPUT_MEMOPAD:
        // メッセージをメモ帳へ出力する
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_OUTPUTMEMO);
        break;

      case MNUID_SHOW_DEVICEINFO:
        // デバイス情報の表示
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_DEVICEINFO);
        break;

      case MNUID_START_A2DA:
        // a2DA起動(AA表示)
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_EXECA2DA);
        break;

      case MNUID_OPEN_URL:
        // URLを開く
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_OPENURL);
        break;
        
      case MNUID_SHOW_HELP:
        // 操作説明を表示
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_USAGE);
        break;

      case MNUID_THREAD_DELETE:
        // 削除後一覧画面に戻る
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_DELETEBACK);
        break;

      case MNUID_VIEW_BE_PROFILE:
        // Beプロフィール参照
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_VIEWBEPROF);
        break;

      case MNUID_ENTRY_NGWORD:
        // NGワード登録
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_ENTRY_NG);
        break;

      default:
        // 上記以外
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Hander_NNshMessage                                       */
/*                                                       イベントハンドラ  */
/*-------------------------------------------------------------------------*/
Boolean Handler_NNshMessage(EventType *event)
{
    Boolean   ret = false;
#if defined(USE_HANDERA) || defined(USE_PIN_DIA)
    FormType *frm;
#endif

    switch (event->eType)
    {
      case keyDownEvent:
        return (selEvt_KeyDown_DispMessage(event));
        break;

      case ctlSelectEvent:
        return (ctlSelEvt_DispMessage(event));
        break;

      case fldChangedEvent:
        NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);
        return (false);
        break;

      case menuEvent:
        return (menuEvt_DispMessage(event));
        break;

      case sclRepeatEvent:
        return (sclRepEvt_DispMessage(event));
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent:
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            UpdateFieldRegion();
            NNsi_FrmDrawForm(frm, true);
        }
        break;
#endif
#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                // リサイズ後の描画
                UpdateFieldRegion();
                NNsi_FrmDrawForm(frm, true);
            }
        }
        break;
#endif

      case penDownEvent:
        if((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
        {
            /* tap された領域をチェック *
             *     上部エリアをチェック */
            if (NNshGlobal->fieldPos[0].startX <= event->screenX &&
               NNshGlobal->fieldPos[0].endX > event->screenX &&
               NNshGlobal->fieldPos[0].startY <= event->screenY &&
               NNshGlobal->fieldPos[0].endY > event->screenY)
           {
               (void) moveMessagePrev(true, (NNshGlobal->NNsiParam)->dispBottom);
               return(true);
           }
           else
           {
               /* 下部エリアをチェック */
               if (NNshGlobal->fieldPos[1].startX <= event->screenX &&
                   NNshGlobal->fieldPos[1].endX > event->screenX &&
                   NNshGlobal->fieldPos[1].startY <= event->screenY &&
                   NNshGlobal->fieldPos[1].endY > event->screenY)
               {
                   (void) moveMessageNext(true);
                   return(true);
               }
           }
        }
        break;

      case frmOpenEvent:
        UpdateFieldRegion();
        break;
#if 0
      case menuOpenEvent:
      case fldEnterEvent:
      case popSelectEvent:
      case lstSelectEvent:
      case lstEnterEvent:
      case lstExitEvent:
      case sclExitEvent:
      case nilEvent:
#endif
      default:
        break;
    }
    return (ret);
}


/*=========================================================================*/
/*   Function : popUP_Message                                              */
/*                                (nnDAを使ったレスポップアップ表示の実行) */
/*=========================================================================*/
static void popUP_Message(UInt16 resNum)
{
    Err                  ret;
    Char                *cmdMsg, *txtP;
    UInt32               readSize;
    UInt16               size;
    MemHandle            memH;
    NNshFileRef          fileRef;

    if ((resNum < 1)&&((NNshGlobal->msgIndex)->nofMsg > resNum))
    {
        // 指定された数字が異常、終了する
        return;
    }

    // クリップボード用の領域を確保する
    cmdMsg = MEMALLOC_PTR(BUFSIZE * 2 + MARGIN);
    if (cmdMsg == NULL)
    {
        return;
    }
    MemSet(cmdMsg, (BUFSIZE * 2) + MARGIN, 0x00);

    // 領域を確保する
    size   = (((NNshGlobal->msgIndex)->msgOffset)[resNum] -
              ((NNshGlobal->msgIndex)->msgOffset)[resNum - 1]);
    if (size > ((NNshGlobal->NNsiParam)->bufferSize - (MARGIN * 3)))
    {
        size = (NNshGlobal->NNsiParam)->bufferSize - (MARGIN * 3);
    }
    memH = MemHandleNew(size + MARGIN * 3);
    if (memH == 0)
    {
        // 領域確保失敗、すぐ終了する
        MEMFREE_PTR(cmdMsg);
        return;
    }
    txtP = MemHandleLock(memH);
    if (txtP == NULL)
    {
        // 領域ロック失敗、終了する...
        MEMFREE_PTR(cmdMsg);
        MEMFREE_HANDLE(memH);
        return;
    }
    MemSet(txtP, (size + MARGIN * 3), 0x00);
#ifdef USE_REPORTER_VIEW
    HostTraceOutputTL(appErrorClass, "+++++ %lu :ALLOCATE BUFFER SIZE:%d ", memH, (size + MARGIN * 3));
#endif // #ifdef USE_REPORTER

    // レス番号を追加する...
    StrCopy(txtP, "[");
    NUMCATI(txtP, resNum);
    StrCat (txtP, "]  ");
    
    // データをバッファに読み込む
    ret = ReadFile_NNsh(&((NNshGlobal->msgIndex)->fileRef),
                        ((NNshGlobal->msgIndex)->msgOffset)[resNum - 1], 
                        size, &txtP[StrLen(txtP)], &readSize);
    if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        // ファイルにデータを出力する
        ret = OpenFile_NNsh(FILE_SENDMSG,
                            (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                            &fileRef);
        if (ret == errNone)
        {
            (void) WriteFile_NNsh(&fileRef, 0, StrLen(txtP), txtP, &readSize);
            CloseFile_NNsh(&fileRef);
        }
        else
        {
            // バッファの開放...
            MemHandleUnlock(memH);
            MEMFREE_HANDLE(memH);

            // ファイルオープンに失敗...
            NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
            return;
        }
#ifdef USE_REPORTER_VIEW
        HostTraceOutputTL(appErrorClass, "-----");
        HostTraceOutputTL(appErrorClass, "%s", txtP);
        HostTraceOutputTL(appErrorClass, "-----");
#endif // #ifdef USE_REPORTER

        // バッファの開放...
        MemHandleUnlock(memH);
        MEMFREE_HANDLE(memH);

        // 読み出し成功、ポップアップ表示を行う
        // (クリップボードに、nnDAに渡すデータについての指示を格納する)
        MemSet (cmdMsg, ((BUFSIZE * 2) + MARGIN), 0x00);
        StrCopy(cmdMsg, nnDA_NNSIEXT_VIEWSTART);
        StrCat (cmdMsg, nnDA_NNSIEXT_INFONAME);
        StrCat (cmdMsg, nnDA_NNSIEXT_FOLDHTMLFILE);
        // 漢字コードの決定
        switch ((NNshGlobal->msgIndex)->bbsType & NNSH_BBSTYPE_MASK)
        {
          case NNSH_BBSTYPE_SHITARABA:
          case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
          case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
          case NNSH_BBSTYPE_2ch_EUC:
            // 漢字コードはEUC
            StrCat (cmdMsg, "e:send.txt");
            break;

          default:
            // 標準は shift jis
            StrCat (cmdMsg, "s:send.txt");
            break;
        }
        StrCat (cmdMsg, nnDA_NNSIEXT_ENDINFONAME);
        StrCat (cmdMsg, nnDA_NNSIEXT_ENDVIEW);
        ClipboardAddItem(clipboardText, cmdMsg, StrLen(cmdMsg) + 1); 

#ifdef USE_REPORTER_VIEW
        HostTraceOutputTL(appErrorClass, "===== CMD %s ", cmdMsg);
#endif // #ifdef USE_REPORTER

        // nnDAを起動する
        (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
    }
    // 領域の開放
    MEMFREE_PTR(cmdMsg);

    return;
}

/*=========================================================================*/
/*   Function : updateThreadLevel                                          */
/*                                                        スレレベルの更新 */
/*=========================================================================*/
static void updateThreadLevel(void)
{
    Err                  ret;
    NNshSubjectDatabase  subjDb;

    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDb);
    if (ret != errNone)
    {
        // データが取得できなかったときには何もしない...
    }

    // BBS情報に設定された標準スレレベルを反映させる
    switch ((subjDb.msgAttribute)&(NNSH_MSGATTR_FAVOR))
    {
      case NNSH_MSGATTR_FAVOR_L1:
        // 1レベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L2));
        break;

      case NNSH_MSGATTR_FAVOR_L2:
        // 2レベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L3));
        break;

      case NNSH_MSGATTR_FAVOR_L3:
        // 3レベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L4));
        break;

      case NNSH_MSGATTR_FAVOR_L4:
        // Hレベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR));
        break;

      case NNSH_MSGATTR_FAVOR:
        // レベルをクリアする
        subjDb.msgAttribute = (subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR));
        break;

      default:
        // Lレベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L1));
        break;
    }

    // スレレベル情報を更新する
    update_subject_database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDb);

    // 「お気に入り」設定情報をセレクタトリガのラベルに設定する
    setFavoriteLabel(FrmGetActiveForm(), subjDb.msgAttribute);

    return;
}
