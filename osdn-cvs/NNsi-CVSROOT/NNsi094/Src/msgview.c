/*============================================================================*
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
    (void) displayMessage(NNshGlobal->msgIndex);

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
    (void) displayMessage(NNshGlobal->msgIndex);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : parseMessage                                               */
/*                                                  (メッセージの整形処理) */
/*-------------------------------------------------------------------------*/
static void parseMessage(Char *buf, Char *source, UInt32 size,
                         UInt16 *nofJmp, UInt16 *jmpBuf)
{
    UInt32 dataStatus, jmp;
    Char *ptr, *dst, *num;

    dataStatus = MSGSTATUS_NAME;

    // 超遅い解釈ルーチンかも... (１文字づつパースする)
    dst  = buf;
    ptr  = source;
    while (ptr < (source + size))
    {
        if (*ptr == '&')
	{
            // "&gt;" を '>' に置換
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&
		(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                if ((*ptr <= '9')&&(*ptr >= '0'))
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
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&
		(*(ptr + 3) == ';'))
            {
                *dst++ = '<';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&quot;" を '"' に置換
            if ((*(ptr + 1) == 'q')&&(*(ptr + 2) == 'u')&&
		(*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&
		(*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" を '    ' に置換
            if ((*(ptr + 1) == 'n')&&(*(ptr + 2) == 'b')&&
		(*(ptr + 3) == 's')&&(*(ptr + 4) == 'p')&&
		(*(ptr + 5) == ';'))
            {
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&amp;" を '&' に置換
            if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
		(*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
            {
                // ギコのしっぽ変換対策... (ここから)
                if ((*(ptr + 5) == 'g')&&(*(ptr + 6) == 't')&&
                    (*(ptr + 7) == ';'))
                {
                    *dst++ = '>';
                    ptr = ptr + 8;    // StrLen(TAG_AMP)+ 'g' + 't' + ';';
                    if ((*ptr <= '9')&&(*ptr >= '0'))
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
                    *dst++ = ' ';
                    *dst++ = ' ';
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_EMAIL;
                    break;

                  case MSGSTATUS_EMAIL:
                    // e-mail欄の区切り
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_DATE;
                    *dst++ = '\n';
                    break;

                  case MSGSTATUS_DATE:
                    // 年月日・時間およびＩＤ欄の区切り
                    *dst++ = '\n';
                    *dst++ = '\n';
                    dataStatus = MSGSTATUS_NORMAL;
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
            if ((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&
		(*(ptr + 3) == '>'))
            {
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }
            // タグは読み飛ばす
#ifdef USE_STRSTR
            ptr = StrStr(ptr, ">");
#else
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
#endif
            ptr++;
            *dst++ = ' ';
            continue;
        }

        // 終端文字を無視するように変更する
        if (*ptr != '\0')
        {
            // 普通の一文字転写
            *dst++ = *ptr;
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

    frm = FrmGetActiveForm();

    // フォントIDの選択
    if (NNshParam->useSonyTinyFont != 0)
    {
        // SONYハイレゾフォントを変更する
        NNshParam->sonyHRFont = FontSelect(NNshParam->sonyHRFont);

        // フィールドの描画(ハイレゾ画面)
        NNsi_HRFldDrawField(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, FLDID_MESSAGE)));
        // スクロールバーの更新
        NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, SCLID_MESSAGE)));
    }
    else
    {
        // 通常フォントを変更する
        NNshParam->currentFont = FontSelect(NNshParam->currentFont);

        // フィールドの描画(通常画面)
        FldSetFont(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE)),
                                   NNshParam->currentFont);

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
static void ReadMessage_Close(UInt16 nextFormID)
{
    NNshMessageIndex *idxP;

    // メッセージインデックス
    idxP = NNshGlobal->msgIndex;
    if (idxP != NULL)
    {
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(NNshGlobal->msgIndex);
    }

    // ハイレゾ用データ初期値をクリアする
    NNshGlobal->totalLine   = 0;
    NNshGlobal->nofPage     = 0;
    NNshGlobal->currentPage = 0;
    NNshGlobal->prevHRLines = 0;

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
                              UInt32 offset, UInt32 size)
{
    FormType    *frm;
    MemHandle    oldTxtH, txtH;
    Char        *buf, *ptr;
    UInt32       msgSize;
    void        *fldP;
#ifdef USE_ARMLET
    UInt32       armArgs[7], nofJump, lp, jmpMsg[JUMPBUF];
    MemHandle    armH;
    void        *armP;
#endif
    NNshMessageIndex *idxP;
    idxP = NNshGlobal->msgIndex;

    // 表示バッファを作成する(MARGIN * 3の部分でスレタイトル部分をカバー)
    txtH = MemHandleNew(size + (MARGIN * 3));
    if (txtH == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_SUBJECT_LISTFAIL, " size:", size);
        return;
    }
    buf  = MemHandleLock(txtH);
    if (buf == NULL)
    {
        MemHandleFree(txtH);
        NNsh_InformMessage(ALTID_WARN, "MemHandleLock()", " ", 0);
        return;
    }

    // メッセージの整形処理
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        ptr = StrStr((topP + offset), NNSH_MACHITOKEN_MSGSTART);
        if (ptr == NULL)
        {
            ptr = topP + offset;
        }
        else
        {
            ptr = ptr + StrLen(NNSH_MACHITOKEN_MSGSTART);
        }
        // メッセージサイズの現物合わせ...
        msgSize = size - (ptr - (topP + offset)) - 2;
    }
    else
    {
        ptr     = (topP + offset);
        msgSize = size;
    }

    // 表示領域を初期化し、先頭にメッセージ番号を表示
    MemSet(buf, msgSize, 0x00);
    if (NNshParam->boldMessageNum != 0)
    {
        StrCopy(buf, "【");
        NUMCATI(buf, mesNum);
        StrCat (buf, "】");
    }
    else
    {
        StrCopy(buf, "[");
        NUMCATI(buf, mesNum);
        StrCat (buf, "] ");
    }

    // ジャンプ参照数を初期化
    idxP->nofJump = 0;

// ARMletを使用してメッセージの解析を行う。(でも、なんか無駄な処理が...)
#ifdef USE_ARMLET
    // ARMletの関数ポインタを取得する
    if (NNshParam->useARMlet != 0)
    {
        armH = DmGetResource('armc', RSCID_parseMessage_ARM);
        if (armH != 0)
        {
            armP = MemHandleLock(armH);
            if (armP != NULL)
            {
                // ARMletを使用してのインデックス解析処理
                MemSet(armArgs, sizeof(armArgs), 0x00);
                armArgs[0] = (UInt32) (buf + StrLen(buf));
                armArgs[1] = (UInt32) ptr;
                armArgs[2] = (UInt32) msgSize;
                armArgs[3] = (UInt32) &(nofJump);
                armArgs[4] = (UInt32) &(jmpMsg[0]);
                (void) PceNativeCall(armP, armArgs);
                MemHandleUnlock(armH);
                idxP->nofJump = (UInt16) nofJump;

                // こんな処理、こっちで(68k側)やってりゃスポイルされるなぁ...
                for (lp = 0; lp < idxP->nofJump; lp++)
                {
                    idxP->jumpMsg[lp] = (UInt16) jmpMsg[lp];
                }
            }
            else
            {
                // (標準の)インデックス解析処理
                parseMessage((buf + StrLen(buf)), ptr, msgSize,
                             &(idxP->nofJump), &(idxP->jumpMsg[0]));
            }
            DmReleaseResource(armH);
        }
        else
        {
            // (標準の)インデックス解析処理
            parseMessage((buf + StrLen(buf)), ptr, msgSize,
                         &(idxP->nofJump), &(idxP->jumpMsg[0]));
        }
    }
    else
#endif
    {
        // (標準の)インデックス解析処理
        parseMessage((buf + StrLen(buf)), ptr, msgSize,
                     &(idxP->nofJump), &(idxP->jumpMsg[0]));
    }
    (void) MemHandleUnlock(txtH);

    // フィールドの更新
    frm     = FrmGetActiveForm();
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
    oldTxtH = FldGetTextHandle(fldP);
    FldSetTextHandle(fldP, NULL);
    FldSetTextHandle(fldP, txtH);
    if (oldTxtH != 0)
    {
        (void) MemHandleFree(oldTxtH);
    }

    // フィールドの表示
    if (NNshParam->useSonyTinyFont != 0)
    {
        // フィールドの表示系変数をクリアする
        NNshGlobal->totalLine   = 0;
        NNshGlobal->nofPage     = 0;
        NNshGlobal->currentPage = 0;
        NNshGlobal->prevHRLines = 0;

        // フィールドの描画(ハイレゾ画面)
        NNsi_HRFldDrawField(fldP);

        // スクロールバーの更新
        NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, SCLID_MESSAGE)));
    }
    else
    {
        // フィールドの描画(通常画面)
        FldDrawField(fldP);

        // スクロールバーの表示更新
        NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);

        FrmDrawForm(frm);
    }

#if 0
    // ジャンプ参照数がいくらあるかを表示させる(仮のデバッグ表示)
    if (idxP->nofJump != 0)
    {
        if ((buf = MemPtrNew(BUFSIZE)) != NULL)
        {
            MemSet(buf, BUFSIZE, 0x00);
            StrCopy(buf, "nofJump:");
            NUMCATI(buf, idxP->nofJump);
            StrCat (buf, "\n----\n");
            for (msgSize = 0; msgSize < idxP->nofJump; msgSize++)
            {
                StrCat(buf, "  >>");
                NUMCATI(buf, idxP->jumpMsg[msgSize]);
            }
            StrCat (buf, "\n");
            NNsh_DebugMessage(ALTID_INFO, buf, "", 0);
            MemPtrFree(buf);
        }
    }
#endif

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : displayMessage                                             */
/*                                                   メッセージを表示する  */
/*-------------------------------------------------------------------------*/
static Err displayMessage(NNshMessageIndex *idxP)
{
    Err          ret = errNone;
    UInt16       msg;
    UInt32       readSize, offset, setMsg, backMsg;

    msg    = idxP->openMsg;
    offset = 0;

    //  指定されたメッセージ番号が妥当かチェックする。
    if ((msg < 1)||(msg > idxP->nofMsg))
    {
        return (~errNone);
    }

    // データは今読み込まれている領域にあるかチェックする。(現物合わせ...)
    if ((idxP->msgOffset[msg - 1]  < idxP->fileOffset)||
        (idxP->msgOffset[msg] - 1  > idxP->fileOffset + idxP->fileReadSize))
    {
        // 現在、読み込まれているバッファにはデータがない場合。。。
        if (idxP->msgOffset[msg] < NNshParam->bufferSize)
        {
            // ファイルの先頭から読み込む
            offset = 0;
	}
        else if (idxP->msgOffset[msg - 1] >
                                     (idxP->fileSize - NNshParam->bufferSize))
        {
            // ファイルの末尾を読み込む
            offset = idxP->fileSize - NNshParam->bufferSize;
	}
        else
        {
            // 戻るメッセージ数を決定する。
            backMsg = NNshParam->bufferSize / 1200;

            // (インデックスの先頭 - backMsg)から読み込む
            setMsg = ((msg - backMsg) < 1) ? 1: (msg - backMsg);
	    offset = idxP->msgOffset[setMsg] - 10;  // 10はマージン(意味なし)
            if ((offset + NNshParam->bufferSize) < idxP->msgOffset[msg + 1])
            {
                // backMsgメッセージ前が読み込み範囲外だった場合はその場所から
                offset = idxP->msgOffset[msg] - 10; // 10はマージン(意味なし)
            }
        }

        // 「待て」表示 (hideBusyForm()まで)
        Show_BusyForm(MSG_READ_MESSAGE_WAIT);

        // データをバッファに読み込む。
        ret = ReadFile_NNsh(&(idxP->fileRef), offset, NNshParam->bufferSize,
                            &(idxP->buffer[0]), &readSize);
        if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
        {
            idxP->fileOffset   = offset;
            idxP->fileReadSize = readSize;
        }
        Hide_BusyForm();
    }
    if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        // メッセージを表示する。
        offset = (idxP->msgOffset[msg - 1] - (idxP->fileOffset));

        displayMessageSub(idxP->bbsType, idxP->buffer, msg, offset,
                          idxP->msgOffset[msg] - idxP->msgOffset[msg - 1]);
    }

    // 読んだメッセージ番号(とお気に入り情報を)記憶する
    MarkMessageIndex(NNshParam->openMsgIndex, NNSH_MSGATTR_NOTSPECIFY);

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

    // メッセージのスクロールチェック
    if (chkScroll == true)
    {
        ret = NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0, winDown); 
    }
    if ((ret != true)&&(idxP->openMsg < idxP->nofMsg))
    {
         // メッセージ番号をひとつ増やし、画面表示。
         (idxP->openMsg)++;
         (void) displayMessage(NNshGlobal->msgIndex);
         ret = true;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveMessagePrev                                            */
/*                                             ひとつ前のメッセージを表示  */
/*-------------------------------------------------------------------------*/
static Boolean moveMessagePrev(Boolean chkScroll)
{
    Boolean ret = false;
    NNshMessageIndex *idxP;

    // indexを変数に入れる
    idxP = NNshGlobal->msgIndex;

    // メッセージのスクロールチェック
    if (chkScroll == true)
    {
        ret = NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0,  winUp);
    }
    if ((ret != true)&&(idxP->openMsg > 1))
    {
         // メッセージ番号をひとつ減らし、画面表示。
         (idxP->openMsg)--;
         (void) displayMessage(NNshGlobal->msgIndex);
         ret = true;
    }
    return (ret);
}
/*-------------------------------------------------------------------------*/
/*   Function : jumpMessage                                                */
/*                                                (メッセージジャンプ処理) */
/*-------------------------------------------------------------------------*/
static Boolean jumpMessage(FormType *frm, NNshMessageIndex *idxP)
{
    UInt16     tempMsg, num, btnId;
    FieldType *fldP;
    Char      *numP;
    MemHandle  txtH;

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
        idxP->jumpListTitles = 
                            MemPtrNew((idxP->nofJump + 2) *  NNSH_JUMPMSG_LEN);
        if (idxP != NULL)
        {
            // リストに表示するタイトルリストを作成する
            MemSet(idxP->jumpListTitles, 0x00,
                                    ((idxP->nofJump + 2) *  NNSH_JUMPMSG_LEN));
            numP = idxP->jumpListTitles;
            if (NNshParam->addReturnToList != 0)
            {
                // リストの先頭に「一覧へ戻る」を追加
                StrCopy(numP, NNSH_JUMPMSG_TO_LIST);
                numP = numP + StrLen(numP) + 1;
            }

            // リストに「１つ戻る」を追加
            StrCopy(numP, NNSH_JUMPMSG_BACK);
            numP = numP + StrLen(numP) + 1;

            for (num = 0; num < idxP->nofJump; num++)
            {
                StrCopy(numP, NNSH_JUMPMSG_HEAD);
                NUMCATI(numP, idxP->jumpMsg[num]);
                numP = numP + StrLen(numP) + 1;
            }
            num = idxP->nofJump + 1;
            if (NNshParam->addReturnToList != 0)
            {
                num++;
            }

            // 選択ウィンドウを表示する
            btnId = NNshWinSelectionWindow(idxP->jumpListTitles, num);

            if (btnId != BTNID_JUMPCANCEL)
            {
                // スレ一覧に戻るが選択されたとき
                if ((NNshParam->addReturnToList != 0)&&
                    (NNshGlobal->jumpSelection == NNSH_JUMPSEL_TO_LIST))
                {
                    // スレ選択画面に戻る
                    ReadMessage_Close(FRMID_THREAD);
                    return (false);
                }
                if (((NNshParam->addReturnToList != 0)&&
                    (NNshGlobal->jumpSelection == NNSH_JUMPSEL_BACK_USELIST))||
                    ((NNshParam->addReturnToList == 0)&&
                    (NNshGlobal->jumpSelection == NNSH_JUMPSEL_BACK)))
                {
                    // １つ前に飛んだメッセージに戻る
                    jumpPrevious();
                    return (false);
                }

                // リストからジャンプメッセージの場所を求める
                (NNshGlobal->jumpSelection)--;
                if (NNshParam->addReturnToList != 0)
                {
                    (NNshGlobal->jumpSelection)--;
                }
                tempMsg = idxP->jumpMsg[NNshGlobal->jumpSelection];
            }
            else
            {
                // リスト選択キャンセル(何もしない)
                tempMsg = idxP->openMsg;
            }
            MemPtrFree(idxP->jumpListTitles);
        }
        else
        {
            NNsh_DebugMessage(ALTID_ERROR, "Jump LIST Create Failure.", "", 0);
        }
    }

    // 数字フィールドをクリアする
    NNshWinSetFieldText(frm, FLDID_MESSAGE_NUMBER, "", 0); 

    // メッセージを移動してジャンプ、メッセージを表示する
    if (tempMsg != idxP->openMsg)
    {
        idxP->histIdx = 
                  (idxP->histIdx < (PREVBUF - 1)) ? (idxP->histIdx + 1) : 0;
        idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
        idxP->openMsg = tempMsg;

        // メッセージ表示
        (void) displayMessage(idxP);
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

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
      (void)
           NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0, winUp);
    }
    else
    {
      (void)
           NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0, winDown);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectKeyDownEvent                                         */
/*                                                 キーが押された時の処理  */
/*                                (ジョグアシストＯＦＦ時のジョグ処理追加) */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_KeyDown_DispMessage(EventType *event)
{
    Boolean           ret = false;
    FormType         *frm;
    FieldType        *fld;
    NNshMessageIndex *idxP;
    UInt16            keyCode;

    // indexを変数に入れる
    idxP = NNshGlobal->msgIndex;

    // インデックスがない場合には、すぐにreturnする
    if (NNshGlobal->msgIndex == NULL)
    {
        return (false);
    }

    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // 「下」(ジョグダイヤル下)を押した時の処理
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
        (void) moveMessageNext(true);
        ret = true;
        break;

      case chrCapital_N:  // Nキーの入力
      case chrSmall_N:
        // ひとつ後ろのメッセージに移動する 
        (void) moveMessageNext(false);
        ret = true;
        break;

      // 「上」(ジョグダイヤル上)を押した時の処理
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
        (void) moveMessagePrev(true);
        ret = true;
        break;

      case chrCapital_P:  // Pキーの入力
      case chrSmall_P:
        // ひとつ前のメッセージに移動する 
        (void) moveMessagePrev(false);
        ret = true;
        break;

      // ジョグダイヤル押し回し(上)の処理
      case vchrJogPushedUp:
        // 一時状態を(回転中に)設定する
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGUP;
#endif
        /** not break!!  **/

      // カーソルキー(上)押下時の処理
      case chrRecordSeparator:
      case chrCapital_R:  // Rキーの入力
      case chrSmall_R:
        ret = true;
        // 押し回しで１つづづ上げる？
        if (NNshParam->notCursor != 0)
        {
            // 次のメッセージを表示して終了
            (void) moveMessagePrev(false);
            break;
        }
        if (idxP->openMsg > 1)
        {
            // メッセージ番号を10減らし、画面表示。
            idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                                                 (idxP->histIdx + 1) : 0;
            idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
            idxP->openMsg = (idxP->openMsg > 11) ? (idxP->openMsg - 10) : 1;
            (void) displayMessage(NNshGlobal->msgIndex);
        }
        break;

      // ジョグダイヤル押し回し(下)の処理
      case vchrJogPushedDown:
        // 一時状態を(回転中に)設定する
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGDOWN;
#endif
        /** not break!!  **/

      // カーソルキー(下)押下時の処理
      case chrUnitSeparator:
      case chrCapital_F:  // Fキーの入力
      case chrSmall_F:
        ret = true;
        // 押し回しで１つづづ下げる？
        if (NNshParam->notCursor != 0)
        {
            // 次のメッセージを表示して終了
            (void) moveMessageNext(false);
            break;
        }

        if (idxP->openMsg < idxP->nofMsg)
        {
            // メッセージ番号を10増やし、画面表示。
            idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                                                 (idxP->histIdx + 1) : 0;
            idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
            idxP->openMsg    = (idxP->openMsg < idxP->nofMsg - 10) ?
                                          (idxP->openMsg + 10) : idxP->nofMsg;
            (void) displayMessage(NNshGlobal->msgIndex);
        }
        break;

      // JOG Push時の処理(より正確にはボタンを離したときだけど)
      case vchrJogRelease:
#ifdef USE_CLIE
        ret = true;
        if (NNshGlobal->tempStatus == NNSH_TEMPTYPE_CLEARSTATE)
        {
            // スレ差分取得
            (void) PartReceiveMessage_View();
        }
        // 一時状態をクリアする
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
#endif
        break;

      // Sony バックボタン/HandEra バックボタン/ESCキー/Xキーの処理
      case vchrJogBack:
      case chrEscape:
      case chrCapital_X:  // Xキーの入力
      case chrSmall_X:
        if (NNshParam->jogBackBtnAsGo != 0)
        {
            // Goボタンと等価な処理にする
            jumpMessage(FrmGetActiveForm(), NNshGlobal->msgIndex);
            return (true);
        }
        // スレ選択画面に戻る
        ReadMessage_Close(FRMID_THREAD);
        return(true);
        break;

      //  HandEra Jog-Push/Enterキーを押したときの処理
      case chrCarriageReturn:
      case chrLineFeed:
        // (差分取得)
        PartReceiveMessage_View();
        return (true);
        break;

      // 一つ前のジャンプ元レスに戻る(Backボタンと同じ)
      case chrCapital_Z:  // Zキーの入力
      case chrSmall_Z:
        jumpPrevious();
        return (true);
        break;

      // Lキー入力
      case chrCapital_L:
      case chrSmall_L:
        ret = true;
        // フォント変更
        changeFont();
        break;

      // Dキー入力
      case chrCapital_D:
      case chrSmall_D:
        if (NNshParam->useSonyTinyFont == 0)
        {
            // ハイレゾモードへ変更
            NNshParam->useSonyTinyFont = 1;

            // 画面を描画する
            (void) displayMessage(NNshGlobal->msgIndex);
        }
        else
        {
            // 通常モードへ変更
            NNshParam->useSonyTinyFont = 0;
            frm  = FrmGetActiveForm();

            // フィールド領域を使用できるように変更する
            fld = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, FLDID_MESSAGE));
            FldSetUsable(fld, true);

            // スクロールバーの更新
            NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);

            // フォームを消して表示しなおす
            FrmEraseForm(frm);
            FrmDrawForm(frm);
        }
        ret = true;
        break;

      // Qキー入力
      case chrCapital_Q:
      case chrSmall_Q:
        ret = true;
        // 回線切断
        NNshNet_LineHangup();
        break;

        // Bキー入力
      case chrCapital_B:
      case chrSmall_B:
        ret = true;
        // スレの末尾へ
        jumpEdge(true);
        break;

        // Tキー入力
      case chrCapital_T:
      case chrSmall_T:
        // スレの先頭へ
        ret = true;
        jumpEdge(false);
        break;

        // スペースキー・Gキーの入力(メッセージJUMP)
      case chrSpace:
      case chrCapital_G:
      case chrSmall_G:
        ret = true;
        jumpMessage(FrmGetActiveForm(), NNshGlobal->msgIndex);
        break;

      case chrCapital_W:  // Wキーの入力
      case chrSmall_W:
        // スレに書き込み(OFFLINEスレには書き込みをしない)
        if (StrCompare(idxP->boardNick, OFFLINE_THREAD_NICK) != 0)
        {
            ReadMessage_Close(FRMID_WRITE_MESSAGE);
            ret = true;
        }
        break;

      default:
        // キーコードを表示する
        NNsh_DebugMessage(ALTID_INFO, "Pushed a key  code:", "",
                          event->data.keyDown.chr);
        break;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlSelEvt_DispMessage                                    */
/*                                                      ボタン押下時の処理 */
/*-------------------------------------------------------------------------*/
Boolean ctlSelEvt_DispMessage(EventType *event)
{
    UInt16       num;
    FormType    *frm;
    ControlType *fvP;
    NNshMessageIndex *idxP;

    // indexを変数に入れる
    idxP = NNshGlobal->msgIndex;

    // インデックスがない場合には、すぐにreturnする
    if (idxP == NULL)
    {
        return (false);
    }

    // 現在アクティブなフォームを取得する
    frm = FrmGetActiveForm();

    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_TOP:
        // スレの先頭へ
        jumpEdge(false);
        break;

        // 前ボタン 
      case BTNID_PREV_MESSAGE:
        moveMessagePrev(false);
        break;

        // 次ボタン 
      case BTNID_NEXT_MESSAGE:
        moveMessageNext(false);
        break;

        // 末尾ボタン(スレの末尾へ)
      case BTNID_BOTTOM:
        jumpEdge(true);
        break;

        // 戻るボタン 
      case BTNID_BACK_MESSAGE:
        jumpPrevious();
        break;

        // GOボタン
      case BTNID_JUMP:
        jumpMessage(frm, NNshGlobal->msgIndex);
        break;

        // 書込ボタン
      case BTNID_WRITE_MESSAGE:
        // スレに書き込み(OFFLINEスレには書き込みをしない)
        if (StrCompare(idxP->boardNick, OFFLINE_THREAD_NICK) != 0)
        {
            ReadMessage_Close(FRMID_WRITE_MESSAGE);
        }
        break;

        // しおりボタン
      case BTNID_BOOKMARK:
        if (NNshParam->useBookmark != 0)
        {
            NNshParam->lastFrmID     = FRMID_MESSAGE;
            NNshParam->bookMsgIndex  = NNshParam->openMsgIndex;
            NNshParam->bookMsgNumber = idxP->openMsg;
            NNsh_InformMessage(ALTID_INFO, MSG_SET_BOOKMARK, "", 0);
        }
        break;

      case SELTRID_THREAD_TITLE:  
        // タイトルボタン(一覧画面に戻る)
        ReadMessage_Close(FRMID_THREAD);
        break;

      case CHKID_FAVOR_THREAD:
        // お気に入りの設定情報を取得し、記憶
        fvP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_FAVOR_THREAD));
        num = CtlGetValue(fvP);
        MarkMessageIndex(NNshParam->openMsgIndex, num);
        if (num == 0)
        {
            // "お気に入り設定"を解除した旨表示する
            NNsh_InformMessage(ALTID_INFO, MSG_CHANGE_NOTFAVORITE, "", 0);
        }
        else
        {
            // "お気に入り設定"した旨表示する
            NNsh_InformMessage(ALTID_INFO, MSG_CHANGE_FAVORITE, "", 0);
        }
        break;

      default:
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
    Char       logMsg[BUFSIZE * 2];
    FormType  *frm;
    FieldType *fld;
    Char      *urlP, *ptr, *dest;
    MemHandle  memH;
    UInt16     len;
#ifdef USE_HANDERA
    VgaScreenModeType   handEraScreenMode;     // スクリーンモード
    VgaRotateModeType   handEraRotateMode;     // 回転モード
#endif
    NNshMessageIndex *idxP;
    UInt16            frmId;

    // indexを変数に入れる
    idxP = NNshGlobal->msgIndex;

    switch (event->data.menu.itemID)
    {
      case MNUID_THREAD_SELECTION:
        // スレ選択画面に戻る
        ReadMessage_Close(FRMID_THREAD);
        break;

      case MNUID_THREAD_INFO:
        // スレ情報を表示する。 
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, "TITLE: ");
        StrCat (logMsg, idxP->threadTitle);
        StrCat (logMsg, "\n\nfileSize: ");
        NUMCATI(logMsg, idxP->fileSize);
        StrCat (logMsg, "\ncurrent/Max\n");
        NUMCATI(logMsg, idxP->openMsg);
        StrCat (logMsg, "/");
        NUMCATI(logMsg, idxP->nofMsg);
        StrCat (logMsg, "\nfOffset:");
        NUMCATI(logMsg, idxP->fileOffset);
        StrCat (logMsg, " fRSize:");
        NUMCATI(logMsg, idxP->fileReadSize);
        NNsh_ErrorMessage(ALTID_INFO, "", logMsg, 0);
        break;

      case MNUID_TOP_MESSAGE:
        // スレの先頭にジャンプ
        jumpEdge(false);
        break;

      case MNUID_BOTTOM_MESSAGE:
        // スレの末尾にジャンプ
        jumpEdge(true);
        break;

      case MNUID_WRITE_MESSAGE:
        // スレに書き込み(OFFLINEスレには書き込みをしない)
        if (StrCompare(idxP->boardNick, OFFLINE_THREAD_NICK) != 0)
        {
            ReadMessage_Close(FRMID_WRITE_MESSAGE);
        }
        break;

      case MNUID_GET_PART:
        // スレ差分取得
        (void) PartReceiveMessage_View();
        break;

      case MNUID_NET_DISCONN:
        // 回線切断
        NNshNet_LineHangup();
        break;

      case MNUID_FONT_MSGVIEW:
        // フォント変更
        changeFont();
        break;

#ifdef USE_HANDERA
      case MNUID_HANDERA_ROTATE_MSGVIEW:
        // HandEra 画面回転 //
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
            VgaSetScreenMode(screenMode1To1, 
                             VgaRotateSelect(handEraRotateMode));

            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
	}
        else
        {
            // "現在サポートしていません" 表示を行う
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;
#endif // #ifdef USE_HANDERA

      case MNUID_CLIE_DRAW_MODE: 
        if (NNshParam->useSonyTinyFont == 0)
        {
            // ハイレゾモードへ変更
            NNshParam->useSonyTinyFont = 1;

            // 画面を描画する
            (void) displayMessage(NNshGlobal->msgIndex);
        }
        else
        {
            // 通常モードへ変更
            NNshParam->useSonyTinyFont = 0;
            frm  = FrmGetActiveForm();

            // フィールド領域を使用できるように変更する
            fld = FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, FLDID_MESSAGE));
            FldSetUsable(fld, true);

            // スクロールバーの更新
            NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);

            // フォームを消して表示しなおす
            FrmEraseForm(frm);
            FrmDrawForm(frm);
        }
        break;

      case MNUID_OS5_LAUNCH_WEB:
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
            // 選択している文字列を一旦クリップボードにコピーし、取得
            MemSet (logMsg, sizeof(logMsg), 0x00);
            FldCopy(FrmGetObjectPtr(frm, frmId));
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
                        StrCopy(logMsg, "http://");
                        dest = &logMsg[StrLen(logMsg)];

                        // クリップボードからURLの文字列(ASCII)だけを抽出する
                        while ((*ptr > ' ')&&(*ptr < 0x7f))
                        {
                            *dest = *ptr;
                            dest++;
                            ptr++;
                        }
                    }
                    MemHandleUnlock(memH);                
                }
            }
            if (logMsg[0] != 0x00)
            {
                // NetFrontを指定されたURLで起動するか確認する
                if (NNsh_ConfirmMessage(ALTID_CONFIRM, logMsg,
                                        MSG_WEBLAUNCH_CONFIRM, 0) == 0)
                {
                    // NetFrontを起動する(NNsi終了後に起動)
                    (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                             false, 0, 
                                             NNshGlobal->browserLaunchCode,
                                             logMsg, NULL);
                }
            }
        }
        break;

      case MNUID_EDIT_COPY:
        // コピー
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
            if (NNshParam->useSonyTinyFont != 0)
            {
                // ハイレゾフォント設定中の時には、フィールドの再描画を行う
                NNsi_HRFldDrawField(FrmGetObjectPtr(frm, frmId));
            }
        }
        break;

      case MNUID_SELECT_ALL:
        // 文字列の全選択
        frm = FrmGetActiveForm();
        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        FldSetSelection(fld, 0, FldGetTextLength(fld));
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        if (NNshParam->useSonyTinyFont != 0)
        {
            // ハイレゾフォント設定中の時には、フィールドの再描画を行う
            NNsi_HRFldDrawField(fld);
        }
        break;

      default:
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
#ifdef USE_HANDERA
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
            FrmDrawForm(frm);
        }
        break;
#endif

      case menuOpenEvent:
      case fldEnterEvent:
      case frmOpenEvent:
      case popSelectEvent:
      case lstSelectEvent:
      case lstEnterEvent:
      case lstExitEvent:
      case sclExitEvent:
      default:
       break;
    }
    return (ret);
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

    // しおり機能を使う・使わないでしおり設定ボタンを表示するか切り替える
    if (NNshParam->useBookmark != 0)
    {
        FrmShowObject(frm, FrmGetObjectIndex(frm, BTNID_BOOKMARK));
    }
    else
    {
        FrmHideObject(frm, FrmGetObjectIndex(frm, BTNID_BOOKMARK));
    }

    // スレ情報をまず取得
    ret = Get_Subject_Database(NNshParam->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // subjectデータベースからデータ取得に失敗
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        goto FUNC_END;
    }

    // 一時バッファ領域がすでに確保されていたら解放し、再度とりなおす。
    if (NNshGlobal->msgIndex != NULL)
    {
        idxP = NNshGlobal->msgIndex;
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(NNshGlobal->msgIndex);
    }
    bufferSize = sizeof(NNshMessageIndex) + NNshParam->bufferSize + MARGIN * 2;
    NNshGlobal->msgIndex = MemPtrNew(bufferSize);
    if (NNshGlobal->msgIndex == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR, "ERR>MemPtrNew(MsgIndex)",
                       " size:", bufferSize);
        ret = ~errNone;
        goto FUNC_END;
    }
    NNsh_DebugMessage(ALTID_INFO, "MemPtrNew(MsgIndex)"," size:",bufferSize);
    MemSet(NNshGlobal->msgIndex, bufferSize, 0x00);

    // BBSタイプをワーク領域にコピー
    idxP = NNshGlobal->msgIndex;
    idxP->bbsType = (UInt16) subjDB.bbsType;

    // タイトルとボードニックネームをワーク領域にコピー
    StrNCopy(idxP->threadTitle, subjDB.threadTitle, MAX_THREADNAME - 1);
    StrNCopy(idxP->boardNick,   subjDB.boardNick,   MAX_NICKNAME - 1);

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
        if ((NNshParam->useVFS & NNSH_VFS_USEOFFLINE) != 0)
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
        MemPtrFree(idxP);
        idxP = NULL;
        NNsh_DebugMessage(ALTID_ERROR, "Message File Open :", fileName, ret);
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
    if (NNshParam->disableUnderline != 0)
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

    // スレタイトルの表示
    selP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SELTRID_THREAD_TITLE));
    if (selP != NULL)
    {
        CtlSetLabel(selP, idxP->threadTitle);
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
    if ((NNshParam->useBookmark != 0)&&(NNshParam->bookMsgNumber != 0))
    {
        if (NNshParam->bookMsgIndex == NNshParam->openMsgIndex)
        {
            (NNshGlobal->msgIndex)->openMsg = NNshParam->bookMsgNumber;
        }
    }

    // 「お気に入り」設定なら、チェックボックスにチェックを反映
    if ((subjDB.msgAttribute & NNSH_MSGATTR_FAVOR) == NNSH_MSGATTR_FAVOR)
    {
        selP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm, CHKID_FAVOR_THREAD));
        CtlSetValue(selP, (UInt16) 1);
    }

    // フォーカスをメッセージ番号フィールドに設定
    FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE_NUMBER));

    // 画面表示
    (void) displayMessage(NNshGlobal->msgIndex);
    NNshParam->lastFrmID = FRMID_MESSAGE;
    return (errNone);

FUNC_END:
    // エラー終了する
    ReadMessage_Close(FRMID_THREAD);
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
        NNsh_DebugMessage(ALTID_INFO, "SearchRecord_NNsh() index:", "", index);

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
    buffer = MemPtrNew(size);
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
    MemPtrFree(buffer);
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
    ret = Get_Subject_Database(NNshParam->openMsgIndex, &subjDB);
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

    if (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // まちBBSに対して差分取得を選択したが、現在サポートなし(とりあえず?)
        NNsh_ErrorMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI, "", 0);
        return (~errNone);
    }

    // 現在参照中のファイルを一旦クローズしておく
    CloseFile_NNsh(&(idxP->fileRef));

    // スレ差分取得を行う
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsData.boardURL);
    StrCat (url, "dat/");
    StrCat (url, subjDB.threadFileName);

    (void) Get_PartMessage(url, bbsData.boardNick, &subjDB, 
                           NNshParam->openMsgIndex);

    // 表示しなおし
    return (OpenForm_NNshMessage(FrmGetActiveForm()));
}
