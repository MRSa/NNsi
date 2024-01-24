/*============================================================================*
 *
 *  $Id: getlog.c,v 1.90 2005/12/24 12:26:25 mrsa Exp $
 *
 *  FILE: 
 *     getlog.c
 *
 *  Description: 
 *     参照ログのログ取得
 *===========================================================================*/
#define GETLOG_C
#include "local.h"

#ifdef USE_LOGCHARGE
/*--------------------------------------------------------------------------*/
/*  getLogFileList() : ログファイルから取得すべきURLの一覧を取得する        */
/*                             ※※ 要リファクタリング！！ (Phase 2) ※※   */
/*                                                                          */
/*           ※ URLの一覧は、DBNAME_TEMPURL という DBに格納する。           */
/*                                                                          */
/*                    fileName :                                            */
/*                    baseURL  :                                            */
/*                    tokenId  :                                            */
/*                    tempURL  :  (データを一時的に切り出すワークバッファ)  */
/*--------------------------------------------------------------------------*/
static Err getLogFileList(Char *fileName, Char *baseURL, UInt32 tokenId, NNshGetLogTempURLDatabase *tempURL)
{
    NNshFileRef                 fileRef;
    NNshLogTokenDatabase       *logToken;
    Char                       *buffer,  *ptr, *ptr2, *chkPtr;
    Char                       *tempPtr, *tempBuf;
    DmOpenRef                   dbRef;
    UInt32                      offset, readSize;
    UInt16                      count,  status, bufCnt;
    Err                         ret;

    // テンポラリのメモリを確保
    tempBuf = MEMALLOC_PTR(MAX_GETLOG_URL + MARGIN);
    if (tempBuf == NULL)
    {
        return (~errNone - 9);
    }
    MemSet(tempBuf, (MAX_GETLOG_URL + MARGIN), 0x00);

    // テンポラリデータベースを削除する
    DeleteDB_wCreator(DBNAME_TEMPURL, DBVERSION_TEMPURL, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);

    // テンポラリDBオープン(新規オープン)
    OpenDatabase_NNsh(DBNAME_TEMPURL, DBVERSION_TEMPURL, &dbRef);
    if (dbRef == 0)
    {
        // DBオープン失敗(ありえないはずだが...)
        MEMFREE_PTR(tempBuf);
        return (~errNone - 10);
    }
    count = 0;
    GetDBCount_NNsh(dbRef, &count);
    if (count != 0)
    {
        // ＤＢのレコードを(全件)削除 (でも動かないはず)
        for (; count != 0; count--)
        {
            DeleteRecordIDX_NNsh(dbRef, (count - 1));
        }
    }

    // ファイル読み込み領域を確保する
    buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (buffer) ", " size:", 
                          (NNshGlobal->NNsiParam)->bufferSize);
        CloseDatabase_NNsh(dbRef);
        MEMFREE_PTR(tempBuf);
        return (~errNone);
    }
    MemSet(buffer, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    // トークンデータの領域を確保する
    logToken = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (logToken == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (logToken) ", "", 0);
        MEMFREE_PTR(buffer);
        CloseDatabase_NNsh(dbRef);
        MEMFREE_PTR(tempBuf);
        return (~errNone);
    }
    MemSet(logToken, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);

    // トークンデータの取得
    if (GetLogToken(tokenId, logToken) != errNone)
    {
        // トークンデータの取得に失敗した場合...
        MemSet(logToken, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, " startToken : %s ", logToken->startToken);
    HostTraceOutputTL(appErrorClass, " endToken : %s ",   logToken->endToken);
#endif // #ifdef USE_REPORTER

    // ファイルのオープン
    ret = OpenFile_NNsh(fileName, NNSH_FILEMODE_READONLY, &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", fileName, ret);
        MEMFREE_PTR(logToken);
        MEMFREE_PTR(buffer);
        CloseDatabase_NNsh(dbRef);
        MEMFREE_PTR(tempBuf);
        return (ret);
    }

    // ファイルをデータブロックごとに取得する
    status = NNSH_MSGTOKENSTATE_NONE;
    offset = 0;
    while ((ret == errNone)&&(status != NNSH_MSGTOKENSTATE_END))
    {
        readSize = 0;
        MemSet(buffer, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

        ret  = ReadFile_NNsh(&fileRef, offset, (NNshGlobal->NNsiParam)->bufferSize, buffer, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            // ファイル読みこみエラー、終了する
            NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh() :", fileName, ret);
            break;
        }

        chkPtr = buffer;
        if ((status == NNSH_MSGTOKENSTATE_NONE)&&(logToken->startToken[0] != '\0'))
        {
            //  先頭の区切り文字列を探す
            ptr = StrStr(buffer, logToken->startToken);
            if (ptr == NULL)
            {
                // データ先頭の区切りが見つからなかった、次バッファを読み込む
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " CANNOT FIND DATA HEADER... ");
#endif // #ifdef USE_REPORTER
                // 次回読み込む先頭を特定する
                offset = offset + readSize - StrLen(logToken->startToken);
                continue;
            }
            chkPtr = ptr + StrLen(logToken->startToken);
        }
        status = NNSH_MSGTOKENSTATE_MESSAGE;

        // 読み込んだバッファ内に、末尾の区切り文字が入っているか調べる
        if (logToken->endToken[0] != '\0')
        {
            ptr = StrStr(buffer, logToken->endToken);
            if (ptr != NULL)
            {
                // 末尾データを発見！ データはそこまでとする
                // (末尾データのポインタ位置計算がうまくできないため、ちょっと
                //  変な計算方法（バッファの先頭からばーっとインクリメントする）
                //  にした...)
                readSize = 0;
                while (ptr > buffer)
                {
                    ptr--;
                    readSize++;
                }
                status = NNSH_MSGTOKENSTATE_END;
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " DETECT END SEPARATOR ");
#endif // #ifdef USE_REPORTER
            }
        }

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " FILE readSize : %lu, buffer : %p ", readSize, buffer);
#endif // #ifdef USE_REPORTER

        // ここから Anchorを探す... (chkPtrに検索開始の先頭、読み込みサイズでreadSize)
        while (chkPtr < (buffer + readSize))
        {
            MemSet (tempURL,  (sizeof(NNshGetLogTempURLDatabase)), 0x00);
            ptr = StrStr(chkPtr, "<link>");
            if (ptr != NULL)
            {
                // <link> を検出した...リンクをtempURLに切り出す
                ptr = StrStr(ptr, "http://");
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

                    // 切り出したURLを出力用領域にコピーする
                    MemMove(tempURL->dataURL, ptr, ((ptr2 - ptr) + 1));

                    // 切り出したURLが妥当かどうかチェックするロジックへ...
                    goto VALIDATE_URL;
                }
            }

            // 大文字アンカーと小文字アンカーのどちらかが含まれるかチェックする
            ptr = StrStr (chkPtr, "<a");
            ptr2 = StrStr(chkPtr, "<A");
            if ((ptr == NULL)&&(ptr2 == NULL))
            {
                // アンカーが見つからなかった、とりあえず、ちょっとだけ戻してみる。
                if (readSize > MARGIN)
                {
                    readSize = readSize - MARGIN;
                }
                // ファイルの次のブロックへ移動する
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " not found anchor : readSize :%d", readSize);
#endif // #ifdef USE_REPORTER
                break;
            }
            if ((ptr == NULL)||((ptr2 != NULL)&&(ptr > ptr2)))
            {
                // 大文字アンカーの方が小文字アンカーよりも前にあった場合には、ptrを
                // 大文字アンカーのものに入れ替える
                ptr = ptr2;
            }
            ptr = ptr + 2;            // "<a"分先にすすめる
            ptr2 = StrStr(ptr, ">");  // アンカーの末尾を取得できるか確認
            if (ptr2 == NULL)
            {
                // アンカーの末尾が確認できなかった、ファイルの次ブロックへ移動する
                if (ptr != buffer)
                {
                    readSize = (ptr - buffer);
                }
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " NOT FOUND TAIL OF ANCHOR readSize: %d ", readSize);
#endif // #ifdef USE_REPORTER
                break;
            }

            // URLは ptr ～ ptr2 の間に(確実に)ある。
            while ((ptr < ptr2)&&(*ptr != '\0'))
            {
                if (((*ptr == 'h')||(*ptr == 'H'))&&
                    ((*(ptr + 1) == 'r')||(*(ptr + 1) == 'R'))&&
                    ((*(ptr + 2) == 'e')||(*(ptr + 2) == 'E'))&&
                    ((*(ptr + 3) == 'f')||(*(ptr + 3) == 'F')))
                {
                    // ここでURLの先頭を決定する...
                    ptr = ptr + 4;
                    while ((*ptr != '"')&&(*ptr != '\'')&&
                            (*ptr != '.')&&(*ptr != '/')&&
                            (ptr < ptr2)&&(*ptr != '\0'))
                    {
                        // 文字(alnum)があった場合は、ループを抜ける
                        if (((*ptr >= '0')&&(*ptr <= '9'))||
                            ((*ptr >= 'A')&&(*ptr <= 'Z'))||
                            ((*ptr >= 'a')&&(*ptr <= 'z')))
                        {
                            break;
                        }
                        ptr++;
                    }
                    break;
                }
                ptr++;
            }
            if ((ptr >= ptr2)||(*ptr == '\0'))
            {
                // データ異常、次のアンカー解析へ
                chkPtr = ptr2 + 1;
                continue;
            }
            if ((*ptr == '"')||(*ptr == '\''))
            {
                // 先頭が " または ' だったばあいには１文字すすめる
                ptr++;
            }
            // tempURLへアンカー文字列切り出し
            bufCnt = 0;
            while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != ' ')&&
                    (*ptr != '>')&&(*ptr != 0x00)&&(ptr < ptr2))
            {
                if (bufCnt < MAX_GETLOG_URL)
                {
                    // URLがURL格納バッファに収まるときは文字をコピーする
                    tempURL->dataURL[bufCnt] = *ptr;
                    bufCnt++;
                }
                ptr++;
            }

#ifdef USE_REPORTER
            if (bufCnt == MAX_GETLOG_URL)
            {
                HostTraceOutputTL(appErrorClass, " ...DATA OVERFLOW... %d(%s)", bufCnt, tempURL->dataURL);
            }
#endif // #ifdef USE_REPORTER

            /////  ここでタイトル文字列を切り出す  /////
            /////   tempPtr  : 切り出し元バッファ
            /////   tempPtr2 : 切り出し先バッファ
            // タイトル文字列の先頭を設定する
            tempPtr = ptr2 + 1;

            ////////////// タイトルを抜き出す... ///////////
            /// (タイトルの先頭はtempTitle, tempPtr2は次の先頭格納領域)
            bufCnt = 0;
            while (((*tempPtr != '<')||(*(tempPtr + 1) != '/'))&&
                    ((UInt8) *tempPtr >= (UInt8) ' ')&&(bufCnt < MAX_THREADNAME))
            {
                if (*tempPtr == '<')
                {
                    // アンカーは(タイトル文字に組み込まず)読み飛ばす...
                    while ((*tempPtr != '\0')&&(*tempPtr != '>'))
                    {
                        tempPtr++;
                    }
                    tempPtr++;
                }
                tempURL->titleName[bufCnt] = *tempPtr;
                tempPtr++;
                bufCnt++;
            }
#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " xx tempTitle: %s", tempTitle);
#endif // #ifdef USE_REPORTER
            
            // tempURLに切り出したURLがある、これが(取得するのに)妥当なURLかチェック
            // (記事の範囲外だったりしないか確認する)
            if (ptr >= buffer + readSize)
            {
                // 記事の範囲外だった、URL登録はしない
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " RANGE OVER : %s ", tempURL);
#endif // #ifdef USE_REPORTER
                // breakし、次のバッファ領域を読み込む...
                break;
            }

            // ラベルつきURLだった場合...
            ptr = StrStr(tempURL->dataURL, "#");
            if (ptr != NULL)
            {
                // #が付いている、妥当なURLではなかった
                // (次のアンカー解析へ)
                chkPtr = ptr2 + 1;
                continue;
            }

VALIDATE_URL:
            ptr = tempURL->dataURL;
            if (((*ptr == 'h')||(*ptr == 'H'))&&
                ((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
                ((*(ptr + 3) == 'p')||(*(ptr + 3) == 'P')))
            {
                //////////////////////////////////////////////////////////////
                // httpプロトコル(URL)付き、妥当なURL、URLリストに登録する
                // (最大リンク数以下の場合)
                //////////////////////////////////////////////////////////////
                EntryRecord_NNsh(dbRef, sizeof(NNshGetLogTempURLDatabase), (void *) tempURL);
#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " ENTRY URL:%s ", tempURL->dataURL);
#endif // #ifdef USE_REPORTER
                chkPtr = ptr2 + 1;
                continue;  // 次のアンカーを探しにいく
            }

            ///////////////////////////////////////////////////////////////////
            // ベースURLが別のところで指定されていた場合
            // （相対位置でアンカーが指定されていた場合...）
            // ベースURLをあらかじめコピーする
            ///////////////////////////////////////////////////////////////////
            MemSet (tempBuf, (MAX_GETLOG_URL + MARGIN), 0x00);
            MemMove(tempBuf, tempURL->dataURL, MAX_GETLOG_URL);
            StrCopy(tempURL->dataURL, baseURL);
            if (tempBuf[0] == '/')
            {
                // 絶対アドレスパス指定
                // (最初の '/' までを抽出する)
                ptr = StrStr(tempURL->dataURL, "//");
                if (ptr != NULL)
                {
                	ptr = ptr + 2;
                    ptr = StrStr(ptr, "/");
                    if (ptr == NULL)
                    {
                        // 最後に / を付加する
                        StrCat(tempURL->dataURL, "/");
                        ptr = tempURL->dataURL+ StrLen(tempURL->dataURL) - 1;
                    }
                }
                else
                {
					ptr = tempURL->dataURL + StrLen(tempURL->dataURL) - 1;
                }
            }
            else
            {
                // 末尾の'/'を探す
                ptr = tempURL->dataURL + StrLen(tempURL->dataURL) - 1;
                while ((ptr > tempURL->dataURL)&&(*ptr != '/'))
                {
                    ptr--;
                }
            }
            ptr++;

            // 先頭の文字列を確認しコピーする
            MemSet(ptr, (StrLen(tempBuf) + 1), 0x00);
            if ((tempBuf[0] == '.')&&(tempBuf[1] == '/'))
            {
                // 相対アドレスパス？
                StrCopy(ptr, &tempBuf[2]);
            }
            else if (tempBuf[0] == '/')
            {
                // 絶対アドレスパス？
                StrCopy(ptr, &tempBuf[1]);
            }
            else
            {
                // それ以外はそのままコピー
                StrCopy(ptr, tempBuf);
            }

            // URLを表示する
            NNsh_DebugMessage(ALTID_INFO, "tempURL > ", tempBuf, 0);

            // URLを表示する
            NNsh_DebugMessage(ALTID_INFO, "URL > ", tempURL->dataURL, 0);

            // DBに登録する
            EntryRecord_NNsh(dbRef, sizeof(NNshGetLogTempURLDatabase), (void *) tempURL);
#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " entry URL:%s ", tempURL->dataURL);
#endif // #ifdef USE_REPORTER

            chkPtr = ptr2 + 1;
        }             // while (chkPtr < buffer + readSize)
        offset = offset + readSize;   // 次回読み込む先頭を特定する
    }                 // while ((ret == errNone)&&(status != NNSH_MSGTOKENSTATE_END))
    ret = errNone;

    // あとしまつ
    CloseFile_NNsh(&fileRef);
    CloseDatabase_NNsh(dbRef);
    MEMFREE_PTR(logToken);
    MEMFREE_PTR(buffer);
    MEMFREE_PTR(tempBuf);

    return (ret);
}

/*==========================================================================*/
/*  GetReadOnlyLogData_NNsh() : 参照ログのログ取得                          */
/*                                                                          */
/*==========================================================================*/
Err GetReadOnlyLogData_NNsh(void)
{
    Char                      *fileName, *titlePtr, dateTime[MAX_GETLOG_DATETIME];
    Boolean                    penState;
    UInt16                     dbCount, loop, x, y, number, urlCnt;
    UInt32                     recordId;
    DmOpenRef                  dbRef, tempDbRef;
    NNshGetLogDatabase        *dbData;
    NNshGetLogTempURLDatabase *tempDbURL;

    // テンポラリデータベースをオープンする
    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef != 0)
    {
        // 件数を取得してクローズする
        GetDBCount_NNsh(dbRef, &dbCount);
        CloseDatabase_NNsh(dbRef);
        
        if (dbCount > NNSH_GETLOGDBMAX_SUBDATA)
        {
            // 件数パージ処理を実施する
            Show_BusyForm(MSG_EXECUTE_PURGE);
            PurgeSubFileDatabase(NNSH_GETLOGDB_FORCE_DELETE,
                                 recordId, 0,
                                 (dbCount - NNSH_GETLOGDBMAX_SUBDATA));
            Hide_BusyForm(false);
        }

        // 使用データをクリアする
        dbRef   = 0;
        dbCount = 0;
    }

    // テンポラリエリアの確保
    tempDbURL = MEMALLOC_PTR(sizeof(NNshGetLogTempURLDatabase) + MARGIN); 
    if (tempDbURL == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (tempDbURL) ", "", 0);
        return (~errNone);
    }
    MemSet(tempDbURL, (sizeof(NNshGetLogTempURLDatabase) + MARGIN), 0x00);

    // ファイル名確保バッファの取得
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (fileName) ", "", 0);
        MEMFREE_PTR(tempDbURL);
        return (~errNone);
    }
    MemSet(fileName, (MAXLENGTH_FILENAME + MARGIN), 0x00);

    // DB格納領域を確保
    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR (logDatabase) ", "", 0);
        MEMFREE_PTR(fileName);
        MEMFREE_PTR(tempDbURL);
        return (~errNone);
    }
    MemSet(dbData, (sizeof(NNshGetLogDatabase) + MARGIN), 0x00);

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        NNsh_DebugMessage(ALTID_ERROR, "DB open(permanentlog) ", "", 0);
        MEMFREE_PTR(dbData);
        MEMFREE_PTR(fileName);
        MEMFREE_PTR(tempDbURL);
        return (~errNone);
    }

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &dbCount);

    // パーマネント参照ログからデータを読み出す
    for (loop = 0; loop < dbCount; loop++)
    {
        // レコードの読み出し
        GetRecord_NNsh(dbRef, loop, sizeof(NNshGetLogDatabase), dbData);

        // DBのレベルが、取得レベルと同じだった場合、、、
        // (レベルゼロの時は全てを取得する)
        if (((NNshGlobal->NNsiParam)->getROLogLevel == 0)||
            (dbData->usable == (NNshGlobal->NNsiParam)->getROLogLevel))
        {
            // ログデータの抽出
            MemSet(tempDbURL->dataURL, (MAX_GETLOG_URL), 0x00);
            StrNCopy(tempDbURL->dataURL, dbData->dataURL, (MAX_GETLOG_URL));
            MemSet(fileName, MAXLENGTH_FILENAME, 0x00);
            MemSet(dateTime, MAX_GETLOG_DATETIME, 0x00);
            (void) ReadOnlyLogDataPickup(false, dbData, tempDbURL->dataURL, fileName, loop, 0, 0, 0, dateTime);
            if (dbData->getDepth != 0)
            {
                // １階層下のログデータも取得する場合...
                if (GetRecordID_NNsh(dbRef, loop, &(recordId)) != errNone)
                {
                    recordId = 0;
                }

                // BUSYウィンドウを表示する
                Show_BusyForm(MSG_EXECUTE_PURGE);

                if (dbData->getSubData > NNSH_GETLOGDBMAX_SITEDATA)
                {
                    // パージ処理を実行する...
                    PurgeSubFileDatabase(NNSH_GETLOGDB_CHECKPARENT,
                                         recordId, 0, 
                                         (dbData->getSubData - NNSH_GETLOGDBMAX_SITEDATA));
                    dbData->getSubData = NNSH_GETLOGDBMAX_SITEDATA;
                }

                // 時刻パージ処理を実施する
                if (dbData->getAfterHours != 255)
                {
/**
                    x = PurgeSubFileDatabase(NNSH_GETLOGDB_CHECK_PARENT_DATE,
                                             recordId, dbData->getAfterHours,
                                             (NNSH_GETLOGDBMAX_SUBDATA));
                    if (dbData->getSubData >= x)
                    {
                        dbData->getSubData = dbData->getSubData - x;
                    }
                    else
                    {
                        dbData->getSubData = 0;
                    }
**/
                }

                // BUSYウィンドウを表示する
                SetMsg_BusyForm(MSG_CHECK_URLLIST);

                // ログファイルからURL一覧をDBに展開する
                (void) getLogFileList(fileName, dbData->dataURL, dbData->tokenId, tempDbURL);

                Hide_BusyForm(false);

                // テンポラリDBオープン
                urlCnt    = 0;
                tempDbRef = 0;
                OpenDatabase_NNsh(DBNAME_TEMPURL, DBVERSION_TEMPURL, &tempDbRef);
                if (tempDbRef == 0)
                {
                    // ＤＢファイルのオープンに失敗した、次に...
                    continue;
                }

                // レコード数を取得する
                GetDBCount_NNsh(tempDbRef, &urlCnt);

                for (number = urlCnt; number != 0; number--)
                {
                    // x に取得するレコード番号を設定する
                    // (stack消費節約で変数を流用する...)
                    x = number - 1;

                    MemSet(tempDbURL, sizeof(NNshGetLogTempURLDatabase) + MARGIN, 0x00);
                    GetRecord_NNsh(tempDbRef, (x), sizeof(NNshGetLogTempURLDatabase), tempDbURL);

                    // ログを取得する(領域URLは下の関数で破壊されるため)
                    MemSet(fileName, MAXLENGTH_FILENAME, 0x00);
#ifdef USE_REPORTER
                    HostTraceOutputTL(appErrorClass, "SUB get URL(%d):%s ", (x), tempDbURL->dataURL);
#endif // #ifdef USE_REPORTER

                    // デバッグでURLを表示する
                    NNsh_DebugMessage(ALTID_INFO, "SUB URL : ", tempDbURL->dataURL, (x));

                    // スレタイトルヘッダを上位のやつを利用するか？
                    if (StrStr(dbData->dataTitlePrefix, "%t") != NULL)
                    {
                        // URL確認のときに抽出したタイトルをくっつける
                        titlePtr = tempDbURL->titleName;
                    }
                    else
                    {
                        titlePtr = NULL;
                    }
                    if (ReadOnlyLogDataPickup(false, dbData, tempDbURL->dataURL,
                                              fileName, loop, recordId, 1, (x), 
                                              titlePtr) == errNone)
                    {
                        // サブデータのカウントを更新する
                        (dbData->getSubData)++;
                    }

                    // ペンの状態を拾う(画面タップされているか確認する)
                    EvtGetPen(&x, &y, &penState);
                    if (penState == true)
                    {
                        // ペンがダウンされていたら、下位層取得を中止するか確認
                        if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                              MSG_CONFIRM_ABORT_SUBLAYER, 
                                              MSG_NOTCHECK_RECORDS, 
                                              urlCnt - number) == 0)
                        {
                            // OKが押された、breakする
                            break;
                        }
                    }
                }

                // データベースを閉じる
                CloseDatabase_NNsh(tempDbRef);
           }
            // 取得時刻が変わっていた場合には、更新する
            if (dateTime[0] != '\0')
            {
                if (StrCompare(dbData->getDateTime, dateTime) != 0)
                {
                    MemMove(dbData->getDateTime, dateTime, MAX_GETLOG_DATETIME);
                    dbData->lastUpdate = TimGetSeconds();     // 更新時刻を格納
                    dbData->state      = NNSH_SUBJSTATUS_NEW; // 更新マークをつける
#ifdef USE_REPORTER
                    HostTraceOutputTL(appErrorClass, "Update DateTime:%s(%lu)", dbData->getDateTime, dbData->lastUpdate);
#endif // #ifdef USE_REPORTER
                    UpdateRecord_NNsh(dbRef, loop, sizeof(NNshGetLogDatabase), dbData);
                }
#if 0
                else
                {
                    // 取得時刻が変わっていない場合には、更新マークを消す
                    dbData->state      = NNSH_SUBJSTATUS_NOT_YET;
                    UpdateRecord_NNsh(dbRef, loop, sizeof(NNshGetLogDatabase), dbData);
                }
#endif
            }
        }

        // ペンの状態を拾う(画面タップされているか確認する)
        EvtGetPen(&x, &y, &penState);
        if (penState == true)
        {
            // ペンがダウンされていたら、中止するか確認を行う
            if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                  MSG_CONFIRM_ABORT_GETLOG, 
                                  MSG_NOTCHECK_RECORDS, dbCount - loop) == 0)
            {
                // OKが押された、breakする
                break;
            }
        }
    }

    // データベースを閉じる
    CloseDatabase_NNsh(dbRef);
 
    // テンポラリデータベースを削除する
    DeleteDB_wCreator(DBNAME_TEMPURL, DBVERSION_TEMPURL, 0, 0);

    // DBレコード領域を開放する
    MEMFREE_PTR(dbData);
    MEMFREE_PTR(fileName);
    MEMFREE_PTR(tempDbURL);

    return (errNone); 
}

/*-------------------------------------------------------------------------*/
/*   Function :   Handler_LogGetURL                                        */
/*                                      参照ログの取得設定イベントハンドラ */
/*                 使用するグローバルな変数                                */
/*                                      NNshGlobal->work1  : 全レコード数  */
/*                                      NNshGlobal->work2  : 表示レコード  */
/*                        NNshGlobal->jumpSelection : 編集中レコードの属性 */
/*                                                        (新規>1, 編集>0) */
/*-------------------------------------------------------------------------*/
Boolean Handler_LogGetURL(EventType *event)
{
    UInt16              keyCode;
    NNshGetLogDatabase  dbData;
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
            effectLogGetURL(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->jumpSelection = 0;
            break;

          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrTapWaveSpecificKeyRUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
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
            effectLogGetURL(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->jumpSelection = 0;
            break;

          case chrRightArrow:     // カーソルキー右(5way右)
          case chrLeftArrow:         // カーソルキー左(5way左)
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
        // 参照ログチャージ用DBの取得
        OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
        if (dbRef == 0)
        {
            // ログDBのオープン失敗、終了する
            return (false);
        }

        // データを読み出す
        MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
        GetRecord_NNsh(dbRef, NNshGlobal->work2, sizeof(NNshGetLogDatabase),
                       &dbData);

        // 読み出したデータを画面に反映させる
        displayLogDataInfo(frm, &dbData);

        // DBをクローズする
        CloseDatabase_NNsh(dbRef);
        return (true);
        break;

      case ctlSelectEvent:
        frm = FrmGetActiveForm();
        switch (event->data.ctlSelect.controlID)
        {
          case BTNID_GETLOG_PREV:
            // 前レコードに移動
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // データの先頭、更新しない
                return (true);
            }
            // データを更新
            effectLogGetURL(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->jumpSelection = 0;
            break;

          case BTNID_GETLOG_NEXT:
            // 次レコードに移動
            if (NNshGlobal->work2 == 0)
            {
                // データの末尾、更新しない
                return (true);
            }

            // データを更新
            effectLogGetURL(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->jumpSelection = 0;
            break;

          case BTNID_GETLOG_NEW:
            // 新規取得先作成(データを末尾に移動する)
            // データを更新
            effectLogGetURL(frm);

            // 空データを画面に反映させる
            MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
            dbData.usable = 1;
            displayLogDataInfo(frm, &dbData);
            NNshGlobal->jumpSelection = 1;
            NNshGlobal->work2 = 0;
            return (true);
            break;

          case BTNID_GETLOG_DELETE:
            // レコード削除
            if ((NNshGlobal->work1 != 0)&&(NNshGlobal->jumpSelection == 0))
            {
                // データを本当に削除してよいか確認する
                if (NNsh_ConfirmMessage(ALTID_CONFIRM, "",
                                        MSG_CONFIRM_DELETE_DATA, 0) == 0)
                {
                    // 参照ログチャージ用DBの取得
                    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
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
        // 参照ログチャージ用DBの取得
        OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
        if (dbRef == 0)
        {
            // ログDBのオープン失敗、終了する
            return (false);
        }

        // データを読み出す
        MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
        GetRecord_NNsh(dbRef, NNshGlobal->work2, sizeof(NNshGetLogDatabase),
                       &dbData);

        // 読み出したデータを画面に反映させる
        displayLogDataInfo(frm, &dbData);

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
/*   Function :   effectLogGetURL                                          */
/*                                              表示データのレコードを反映 */
/*                 使用するグローバルな変数                                */
/*                                      NNshGlobal->work1  : 全レコード数  */
/*                                      NNshGlobal->work2  : 表示レコード  */
/*                        NNshGlobal->jumpSelection : 編集中レコードの属性 */
/*                                                        (新規>1, 編集>0) */
/*-------------------------------------------------------------------------*/
static Boolean effectLogGetURL(FormType *frm)
{
    DmOpenRef           dbRef;
    UInt16              data;
    Char               *ptr;
    NNshGetLogDatabase  dbData;
    FieldType          *fldP;
    MemHandle           txtH;
    
    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return (false);
    }
    
    MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
    
    // ログURLの取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_URLSET));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        MemSet(dbData.dataURL, MAX_GETLOG_URL, 0x00);
        if (*ptr != '\0')
        {
            StrNCopy(dbData.dataURL, ptr, MAX_GETLOG_URL);
            MemHandleUnlock(txtH);
        }
        else
        {
            // URLが指定されなかった、終了する
            MemHandleUnlock(txtH);
            CloseDatabase_NNsh(dbRef);
            return (false);
        }
    }

    // 巡回レベルの設定
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_GETLOG_USE)));
    dbData.usable = (UInt8) data;

    // RSSモードかどうか
    data = 0;
    UpdateParameter(frm, CHKID_GETLOG_RSSDATA, &(data));
    dbData.rssData = (UInt8) data;

    // ログ取得しないかどうか(ログ取得しない場合には１階層下も取得しない)
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_GETLOGMODE)));
    dbData.wwwcMode = (UInt8) data;

    // １階層下も取得する
    UpdateParameter(frm, CHKID_GETLOG_DEPTH, &(data));
    if ((dbData.wwwcMode != 1)&&(data != 0))
    {
        dbData.getDepth = 1;
    }
    else
    {
        dbData.getDepth = 0;
    }

    // ログヘッダの取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_HEADER));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        MemSet(dbData.dataTitlePrefix, MAX_DATAPREFIX, 0x00);
        if (*ptr != '\0')
        {
            StrNCopy(dbData.dataTitlePrefix, ptr, MAX_DATAPREFIX);
        }
        MemHandleUnlock(txtH);
    }

    // 取得先名称の取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_TITLE));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        MemSet(dbData.dataTitle, MAX_TITLE_LABEL, 0x00);
        if (*ptr != '\0')
        {
            StrNCopy(dbData.dataTitle, ptr, MAX_TITLE_LABEL);
        }
        MemHandleUnlock(txtH);
    }

    // n時間後再取得時間
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_REGET_HOUR));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            data = (UInt16) StrAToI(ptr);
            if (data > MAX_GETLOG_REGETHOUR)
            {
                data = MAX_GETLOG_REGETHOUR;
            }
            dbData.getAfterHours = data;
        }
        MemHandleUnlock(txtH);
    }

    // 区切り文字列のIDの取得
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_NORMTOKEN)));
    if (NNshGlobal->work4 != NULL)
    {
        dbData.tokenId = NNshGlobal->work4[data];
    }

    // サブ区切り文字列のID取得
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_SUBTOKEN)));
    if (NNshGlobal->work4 != NULL)
    {
        dbData.subTokenId = NNshGlobal->work4[data];
    }

    // 漢字コードの取得
    data  = LstGetSelection(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, LSTID_KANJICODE)));
    dbData.kanjiCode = (UInt8) data;

    if (NNshGlobal->jumpSelection == 0)
    {
        // レコードを更新する
        UpdateRecord_NNsh(dbRef, NNshGlobal->work2,
                          sizeof(NNshGetLogDatabase), &dbData);
    }
    else
    {
        // レコードを登録する
        EntryRecord_NNsh(dbRef, sizeof(NNshGetLogDatabase), &dbData);
        (NNshGlobal->work1)++;
    }

    // データベースを閉じる
    CloseDatabase_NNsh(dbRef);

    // 参照ログデータベースを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_LOGCHARGE));

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   displayLogDataInfo                                       */
/*                                                      参照ログ取得の設定 */
/*                 使用するグローバルな変数                                */
/*                                      NNshGlobal->work1  : 全レコード数  */
/*                                      NNshGlobal->work2  : 表示レコード  */
/*                        NNshGlobal->jumpSelection : 編集中レコードの属性 */
/*                                                        (新規>1, 編集>0) */
/*-------------------------------------------------------------------------*/
static Boolean displayLogDataInfo(FormType *frm, NNshGetLogDatabase *dbData)
{
    UInt16 data;
    Char   numBuf[TINYBUF];

    // チェックボックスの反映(１階層下も取得)
    data = dbData->getDepth;

    SetControlValue(frm, CHKID_GETLOG_DEPTH, &data);

    // RSSデータかどうか
    data = dbData->rssData;
    SetControlValue(frm, CHKID_GETLOG_RSSDATA, &data);

    // ログ取得しないかどうか
    data = dbData->wwwcMode;
    NNshWinSetPopItems(frm, POPTRID_GETLOGMODE, LSTID_GETLOGMODE, data);

    // タイトルプレフィックス
    NNshWinSetFieldText(frm, FLDID_GETLOG_HEADER, false,
                        dbData->dataTitlePrefix, MAX_DATAPREFIX);

    // 取得URL
    NNshWinSetFieldText(frm, FLDID_GETLOG_URLSET, false, 
                        dbData->dataURL, MAX_GETLOG_URL);

    // n時間後再取得
    MemSet (numBuf, sizeof(numBuf), 0x00);
    StrIToA(numBuf, dbData->getAfterHours);
    NNshWinSetFieldText(frm,FLDID_REGET_HOUR, false, numBuf, TINYBUF);

    // 取得先の名称
    NNshWinSetFieldText(frm, FLDID_GETLOG_TITLE, true, 
                        dbData->dataTitle, MAX_TITLE_LABEL);

    // 巡回レベル設定
    data = dbData->usable;
    NNshWinSetPopItems(frm, POPTRID_GETLOG_USE, LSTID_GETLOG_USE, data);

    // 漢字コード設定
    data = dbData->kanjiCode;
    NNshWinSetPopItems(frm, POPTRID_KANJICODE, LSTID_KANJICODE, data);

    // ポップアップトリガの設定

    // (区切りパターン1)
    data = ConvertTokenListNum(dbData->tokenId);
    NNshWinSetPopItems(frm, POPTRID_NORMTOKEN, LSTID_NORMTOKEN, data);

    // (区切りパターン2)
    data = ConvertTokenListNum(dbData->subTokenId);
    NNshWinSetPopItems(frm, POPTRID_SUBTOKEN,  LSTID_SUBTOKEN, data);

   return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetLogGetURL                                             */
/*                                                      参照ログ取得の設定 */
/*                 使用するグローバルな変数                                */
/*                                      NNshGlobal->work1  : 全レコード数  */
/*                                      NNshGlobal->work2  : 表示レコード  */
/*                        NNshGlobal->jumpSelection : 編集中レコードの属性 */
/*                                                        (新規>1, 編集>0) */
/*-------------------------------------------------------------------------*/
Boolean SetLogGetURL(UInt16 recNum)
{
    Boolean              ret = false;
    FormType            *frm, *prevFrm;
    DmOpenRef            dbRef;
    NNshGetLogDatabase  *dbData;
    UInt16               labelCnt;
    UInt32              *idList;
    NNshWordList         separatorChoice;

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return (false);
    }

    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        return (false);
    }
    MemSet(dbData, (sizeof(NNshGetLogDatabase) + MARGIN), 0x00);

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &(NNshGlobal->work1));

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " SetLogGetURL(%d) : recNum:%d", 
                          recNum, NNshGlobal->work1);
#endif // #ifdef USE_REPORTER

    // ラベル情報を取得する
    labelCnt = GetLogTokenLabelList(&separatorChoice, &idList);
    NNshGlobal->work4 = idList;

    // データの初期化
    //  work1 ： レコード件数
    //  work2 ： 表示中レコード
    //  jumpSelection ： 新規レコードフラグ(１なら新規)
    //  work4 ： 区切りデータリストのIDリストポインタ
    if ((recNum != NNSH_ITEM_LASTITEM)&&(recNum < NNshGlobal->work1))
    {
        // あるレコードをポイントして開く場合
        NNshGlobal->work2 = recNum;
        NNshGlobal->jumpSelection = 0;
    }
    else
    {
        // 新規レコード追加の場合...
        NNshGlobal->work2 = 0;
        NNshGlobal->jumpSelection = 1;        
    }

    // ダイアログ表示準備
    prevFrm = FrmGetActiveForm();
    PrepareDialog_NNsh(FRMID_GETLOG_INPUT, &frm, true);
    if (frm == NULL)
    {
        // データベースを閉じる
        CloseDatabase_NNsh(dbRef);
        ReleaseWordList(&separatorChoice);
        MEMFREE_PTR(idList);
        MEMFREE_PTR(dbData);
        return (false);
    }

    // ポップアップトリガの設定
    if (labelCnt != 0)
    {
        NNshWinSetPopItemsWithList(frm, POPTRID_NORMTOKEN, LSTID_NORMTOKEN, &separatorChoice, 0);
        NNshWinSetPopItemsWithList(frm, POPTRID_SUBTOKEN,  LSTID_SUBTOKEN,  &separatorChoice, 0);
    }

    // レコードが１件もなかった場合、もしくは新規レコード追加の場合
    if ((NNshGlobal->work1 == 0)||(NNshGlobal->jumpSelection == 1))
    {
        // 空のレコードデータを画面に表示する
        dbData->usable = (NNshGlobal->NNsiParam)->getROLogLevel;
    }
    else
    {
        // 最後のレコードデータを画面に表示する場合...
        if (NNshGlobal->work2 == NNSH_ITEM_BOTTOMITEM)
        {
            NNshGlobal->work2 = NNshGlobal->work1 - 1;
        }
        GetRecord_NNsh(dbRef, NNshGlobal->work2,
                       sizeof(NNshGetLogDatabase), dbData);
        NNshGlobal->jumpSelection = 0;
    }

    displayLogDataInfo(frm, dbData);

    // データベースを閉じる
    CloseDatabase_NNsh(dbRef);

    // イベントハンドラの設定
    FrmSetEventHandler(frm, Handler_LogGetURL);

    // 参照ログ取得の設定ダイアログを表示する
    if (FrmDoDialog(frm) == BTNID_GETLOG_OK)
    {
        // 表示データのレコードを反映させる
        ret = effectLogGetURL(frm);
    }

    // ダイアログ表示の後片付け
    PrologueDialog_NNsh(prevFrm, frm, true);

    // ラベル領域を開放する
    ReleaseWordList(&separatorChoice);
    MEMFREE_PTR(idList);
    MEMFREE_PTR(dbData);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :  effectLogToken                                            */
/*                                    ログの区切り設定をデータベースに反映 */
/*-------------------------------------------------------------------------*/
static Boolean effectLogToken(FormType *frm)
{
    NNshLogTokenDatabase *dbData;
    DmOpenRef             dbRef;
    Char                 *ptr;
    FieldType            *fldP;
    MemHandle             txtH;
    UInt16                cnt;
    
    dbData = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (dbData == NULL)
    {
        // 領域確保失敗、終了する
        return (false);
    }

    // データの初期化
    MemSet(dbData, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        MEMFREE_PTR(dbData);
        return (false);
    }

    // データが新規登録か確認する
    if ((NNshGlobal->work3 == 0)&&(cnt != 0))
    {
        // レコードＩＤを取得、レコードIDに格納する
        GetRecordID_NNsh(dbRef, (NNshGlobal->work2), &(dbData->id));
    }

    // ログヘッダの取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_HEADER));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            StrNCopy(dbData->tokenPatternName, ptr, MAX_DATAPREFIX);
        }
        MemHandleUnlock(txtH);
    }

    // 開始文字列の取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_START));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            StrNCopy(dbData->startToken, ptr, MAX_DATATOKEN);
        }
        MemHandleUnlock(txtH);
    }

    // 終了文字列の取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_END));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            StrNCopy(dbData->endToken, ptr, MAX_DATATOKEN);
        }
        MemHandleUnlock(txtH);
    }
    
    // トークン文字列の取得
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_GETLOG_INDEX));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        ptr = MemHandleLock(txtH);
        if (*ptr != '\0')
        {
            StrNCopy(dbData->dataToken, ptr, MAX_DATATOKEN);
        }
        MemHandleUnlock(txtH);
    }

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &(cnt));

    // データが新規登録か確認する
    if ((NNshGlobal->work3 != 0)||(cnt == 0))
    {
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " CREATE NEW RECORD : %d ", cnt);
#endif // #ifdef USE_REPORTER

        // レコードを登録する
        EntryRecord_NNsh(dbRef, sizeof(NNshLogTokenDatabase), dbData);

        // レコードＩＤを取得、レコードIDに格納する
        GetRecordID_NNsh(dbRef, 0, &(dbData->id));

        NNshGlobal->work2 = 0;
        (NNshGlobal->work1)++;
    }

    // レコードを更新する
    UpdateRecord_NNsh(dbRef, NNshGlobal->work2,
                      sizeof(NNshLogTokenDatabase), dbData);

    // DBをクローズする
    CloseDatabase_NNsh(dbRef);

    // 参照ログトークンデータベースを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_LOGTOKEN));

    MEMFREE_PTR(dbData);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :  displayLogToken                                           */
/*                                                  ログの区切り設定を反映 */
/*-------------------------------------------------------------------------*/
static Boolean displayLogToken(FormType *frm, UInt16 recNum)
{
    NNshLogTokenDatabase *dbData;
    DmOpenRef            dbRef;


    dbData = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (dbData == NULL)
    {
        // 領域確保失敗、終了する
        return (false);
    }

    // データの初期化
    MemSet(dbData, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);
    
    // recNum が 0xffff の時は、空文字列を表示する(DBからデータは取得しない)
    if (recNum != NNSH_DATABASE_BLANK)
    {
        // 参照ログチャージ用DBの取得
        OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
        if (dbRef == 0)
        {
            // ログDBのオープン失敗、終了する
            MEMFREE_PTR(dbData);
            return (false);
        }

        // データを読み出す
        GetRecord_NNsh(dbRef, recNum, sizeof(NNshLogTokenDatabase), dbData);

        // DBをクローズする
        CloseDatabase_NNsh(dbRef);
    }

    // 取得したデータを画面に反映させる

    // 区切りパターン名
    NNshWinSetFieldText(frm, FLDID_GETLOG_HEADER, false,
                        dbData->tokenPatternName, MAX_DATAPREFIX);

    // 開始文字列
    NNshWinSetFieldText(frm, FLDID_GETLOG_START, false, 
                        dbData->startToken, MAX_DATATOKEN);

    // 終了文字列
    NNshWinSetFieldText(frm, FLDID_GETLOG_END, false, 
                        dbData->endToken, MAX_DATATOKEN);

    // 区切り文字列
    NNshWinSetFieldText(frm, FLDID_GETLOG_INDEX, true, 
                        dbData->dataToken, MAX_DATATOKEN);

    MEMFREE_PTR(dbData);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Handler_setLogToken                                      */
/*                              参照ログの区切り設定画面用イベントハンドラ */
/*-------------------------------------------------------------------------*/
Boolean Handler_setLogToken(EventType *event)
{
    UInt16              keyCode;
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
          case vchrPageUp:
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrTapWaveSpecificKeyRUp:
          case vchrThumbWheelUp:
          case vchrRockerUp:
          case vchrRockerLeft:
          case vchrJogLeft:
          case vchrTapWaveSpecificKeyRLeft:
          case chrRecordSeparator:   // カーソルキー(上)押下時の処理
            // 次レコードに移動
            if (NNshGlobal->work2 == 0)
            {
                // データの末尾、更新しない
                return (true);
            }
            // データを更新
            effectLogToken(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->work3 = 0;
            break;

          case vchrPageDown:
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrTapWaveSpecificKeyRDown:
          case vchrThumbWheelDown:
          case vchrRockerDown:
          case vchrRockerRight:
          case vchrJogRight:
          case vchrTapWaveSpecificKeyRRight:
          case chrUnitSeparator:  // カーソルキー(下)押下時の処理
            // 前レコードに移動
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // データの先頭、更新しない
                return (true);
            }
            // データを更新
            effectLogToken(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->work3 = 0;
            break;

          case chrLeftArrow:         // カーソルキー左(5way左)
          case chrRightArrow:     // カーソルキー右(5way右)
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

        // 読み出したデータを画面に反映させる
        displayLogToken(frm, NNshGlobal->work2);
        return (true);
        break;

      case ctlSelectEvent:
        frm = FrmGetActiveForm();
        switch (event->data.ctlSelect.controlID)
        {
          case BTNID_GETLOG_NEXT:
            // 前レコードに移動(だけど次ボタンを押したとき...)
            if (((NNshGlobal->work2 + 1) == NNshGlobal->work1)||
                 (NNshGlobal->work1 == 0))
            {
                // データの先頭、更新しない
                return (true);
            }
            // データを更新
            effectLogToken(frm);

            (NNshGlobal->work2)++;
            NNshGlobal->work3 = 0;
            break;

          case BTNID_GETLOG_PREV:
            // 次レコードに移動(だけど前ボタンを押したとき...)
            if (NNshGlobal->work2 == 0)
            {
                // データの末尾、更新しない
                return (true);
            }

            // データを更新
            effectLogToken(frm);

            (NNshGlobal->work2)--;
            NNshGlobal->work3 = 0;
            break;

          case BTNID_GETLOG_NEW:
            // 新規取得先作成(データを末尾に移動する)
            // データを更新
            effectLogToken(frm);

            // 空データを画面に反映させる
            NNshGlobal->work3 = 1;
            NNshGlobal->work2 = 0;
            displayLogToken(frm, NNSH_DATABASE_BLANK);
            return (true);
            break;

          case BTNID_GETLOG_DELETE:
            // レコード削除
            if ((NNshGlobal->work1 != 0)&&(NNshGlobal->work3 == 0))
            {
                // データを本当に削除してよいか確認する
                if (NNsh_ConfirmMessage(ALTID_CONFIRM, "",
                                        MSG_CONFIRM_DELETE_DATA, 0) == 0)
                {
                    // 参照ログチャージ用DBの取得
                    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
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

        // 読み出したデータを画面に反映させる
        displayLogToken(frm, NNshGlobal->work2);

        return (true);
        break;

      default: 
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   SetLogCharge_LogToken                                    */
/*                                                    区切りパターンの登録 */
/*=========================================================================*/
Boolean SetLogCharge_LogToken(UInt16 recNum)
{
    Boolean       ret = false;
    FormType     *prevFrm, *diagFrm;
    UInt16        btnId;
    DmOpenRef     dbRef;

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // 参照ログ区切りDBの取得
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "LOGTOKEN DB Open Failure.");
#endif // #ifdef USE_REPORTER
        return (false);
    }

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &(NNshGlobal->work1));

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "TOKEN DB count : %d", NNshGlobal->work1);
#endif // #ifdef USE_REPORTER

    // データベースを閉じる
    CloseDatabase_NNsh(dbRef);

    // 表示系初期データの反映（ワーク領域の使用方法）
    //   work1：現在のレコード件数
    //   work2：表示中のレコード番号
    //   work3：新規レコード追加（１なら追加）
    NNshGlobal->work2 = recNum;
    NNshGlobal->work3 = (NNshGlobal->work1 == 0) ? 1 : 0;

    // ダイアログ表示準備
    PrepareDialog_NNsh(FRMID_GETLOG_TOKENSET, &diagFrm, true);
    if (diagFrm == NULL)
    {
        return (false);
    }

    // 初期データの反映
    displayLogToken(diagFrm, NNshGlobal->work2);

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_setLogToken);

    // ウィンドウの表示
    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_GETLOG_OK)
    {
         // 表示中データをレコードに反映させる
         effectLogToken(diagFrm);
         ret = true;
    }

    // ダイアログ表示の後片付け
    PrologueDialog_NNsh(prevFrm, diagFrm, true);

    // データベースのソートを行っておく
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_UINT32, 0);
    CloseDatabase_NNsh(dbRef);

    return (ret);
}

/*=========================================================================*/
/*   Function :   GetLogToken                                              */
/*                                                    区切りパターンの取得 */
/*=========================================================================*/
Err GetLogToken(UInt32 key, NNshLogTokenDatabase *record)
{
   Err       ret;
   DmOpenRef dbRef;
   UInt16    index;

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return (~errNone);
    }

    // idをkeyにして検索する
    ret = SearchRecord_NNsh(dbRef, &key, NNSH_KEYTYPE_UINT32, 0,
                            sizeof(NNshLogTokenDatabase), record, &index);

    // DBをクローズする
    CloseDatabase_NNsh(dbRef);

    return (ret);
}

/*=========================================================================*/
/*   Function :  ConvertTokenListNum                                       */
/*                                      区切りパターンIDをリスト番号に変換 */
/*=========================================================================*/
UInt16 ConvertTokenListNum(UInt32 key)
{
   Err       ret;
   DmOpenRef dbRef;
   UInt16    index;
   NNshLogTokenDatabase record;

    // key無指定の場合には、0を応答する
    if (key == 0)
    {
        return (0);
    }

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return (0);
    }

    // idをkeyにして検索する
    index = 0;
    ret = SearchRecord_NNsh(dbRef, &key, NNSH_KEYTYPE_UINT32, 0,
                            sizeof(NNshLogTokenDatabase), &record, &index);
    if (ret != errNone)
    {
        index = 0;
    }

    // DBをクローズする
    CloseDatabase_NNsh(dbRef);

    return (index);
}

/*=========================================================================*/
/*   Function :   GetLogTokenLabelList                                     */
/*                                                区切りパターンの一覧取得 */
/*=========================================================================*/
UInt16 GetLogTokenLabelList(NNshWordList *wordList, UInt32 **idList)
{
    Err                   ret;
    Char                 *nameP, *strBuf;
    UInt32               *idP;
    DmOpenRef             dbRef;
    UInt16                index, count, recCount;
    MemHandle             dataH;
    NNshLogTokenDatabase *data;

    // 変数の初期化
    MemSet(wordList, sizeof(NNshWordList), 0x00);
    *idList = NULL;

    // 参照ログチャージ用DBの取得
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    if (dbRef == 0)
    {
        // ログDBのオープン失敗、終了する
        return (0);
    }

    // レコード件数取得
    GetDBCount_NNsh(dbRef, &count);
    if (count == 0)
    {
        return (0);
    }

    // ID格納領域を確保
    *idList = MEMALLOC_PTR(sizeof(UInt32) * count + MARGIN);
    if (*idList == NULL)
    {
        return (0);
    }
    MemSet(*idList, (sizeof(UInt32) * count + MARGIN), 0x00);
    idP = *idList;

    // ラベル格納領域を確保
    strBuf = MEMALLOC_PTR((sizeof(Char) * (MAX_STRLEN)) * count + MARGIN);
    if (strBuf == NULL)
    {
        MEMFREE_PTR(*idList);
        return (0);
    }
    MemSet(strBuf, ((sizeof(Char) * (MAX_STRLEN)) * count + MARGIN), 0x00);
    nameP = strBuf;

    recCount = 0;
    for (index = 0; index < count; index++)
    {
        // レコードデータの取得
        ret = GetRecordReadOnly_NNsh(dbRef, index, &dataH, (void **) &data);
        if (ret == errNone)
        {
            if (recCount != 0)
            {
                StrCat(nameP, ",");
            }

            // 読み出したデータを確保した領域に反映させる
            *idP = data->id;
            idP++;
            if (data->tokenPatternName[0] != '\0')
            {
                // 文字列を取得
                StrCat(nameP, data->tokenPatternName);
            }
            else
            {
                StrCat(nameP, "???");
            }

            // レコードデータの表示
            nameP = nameP + StrLen(nameP);

            recCount++;
        }
        // レコードを解放し、次へすすむ
        ReleaseRecordReadOnly_NNsh(dbRef, dataH);
    }

    // ラベル一覧をリストに表示する構造へ変換...
    SeparateWordList(strBuf, wordList);
    MEMFREE_PTR(strBuf);

    // DBをクローズする
    CloseDatabase_NNsh(dbRef);

    return (recCount);
}

/*=========================================================================*/
/*   Function :   SelectLogToken                                           */
/*                                                    区切りパターンの選択 */
/*=========================================================================*/
UInt16 SelectLogToken(UInt32 *selection)
{
    Char         *ptr;
    FormType     *prevFrm, *diagFrm;
    UInt32       *idList;
    UInt16        btnId, nofItems;
    MemHandle     listH;
    ListType     *lstP;
    NNshWordList  wordList;
    
    // 初期化
    listH     = 0;
    *selection = 0;
    btnId     = BTNID_JUMPCANCEL;
    prevFrm   = FrmGetActiveForm();
    MemSet(&wordList, sizeof(NNshWordList), 0x00);

    // ログトークンをとりだす
    nofItems = GetLogTokenLabelList(&wordList, &idList);

    // アイテム数の確認(ゼロだったら、何もせず戻る)
    if (nofItems == 0)
    {
        // 区切りパターンなし
        return (BTNID_JUMPCANCEL);
    }

    // ダイアログ表示準備
    PrepareDialog_NNsh(FRMID_DIRSELECTION, &diagFrm, false);
    if (diagFrm == NULL)
    {
        return (BTNID_JUMPCANCEL);
    }

    // リストの設定
    lstP  = FrmGetObjectPtr(diagFrm,
                                FrmGetObjectIndex(diagFrm, LSTID_JUMPLIST));

    ptr = MemHandleLock(wordList.wordmemH);
    LstSetListChoices(lstP, (Char **) ptr, wordList.nofWord);
    LstSetSelection  (lstP, 0);
    LstSetTopItem    (lstP, 0);

    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_JumpSelection);

    // ジャンプ先をクリア
    NNshGlobal->jumpSelection = 0;

    // ダイアログのオープン、文字列の入力を待つ
    btnId = FrmDoDialog(diagFrm);

    // ダイアログ表示の後片付け
    PrologueDialog_NNsh(prevFrm, diagFrm, false);

    // ハンドルアンロック
    MemHandleUnlock(wordList.wordmemH);

    if (btnId == BTNID_JUMPEXEC)
    {
        // 選択された
        *selection = idList[NNshGlobal->jumpSelection];        
    }

    // あとしまつ
    MEMFREE_PTR(idList);
    ReleaseWordList(&wordList);

    return (btnId);
}


/*=========================================================================*/
/*   Function :   GetImageFile                                             */
/*                                             イメージファイルを取得する  */
/*=========================================================================*/
Boolean GetImageFile(Char *url)
{
    Err                  ret;
    Char                *fileName, *ptr;
    NNshGetLogDatabase  *dbData;

    // ファイル取得の確認...
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, url, MSG_COMFIRM_GETFILE, 0) != 0)
    {
        // データの取得確認がキャンセルされた場合
        return (false);
    }

    // ファイル名格納領域を確保
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        return (false);
    }
    MemSet(fileName, MAXLENGTH_FILENAME + MARGIN, 0x00);

    // DBのダミー領域を確保
    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        MEMFREE_PTR(fileName);
        return (false);
    }
    MemSet(dbData, sizeof(NNshGetLogDatabase) + MARGIN, 0x00);

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

    // ＵＲＬデータの取得実処理...
    ret = ReadOnlyLogDataPickup(true, dbData, url, fileName, NNSH_MSGNUM_UNKNOWN, 0, 0, 0, NULL);

    // 確保領域を開放
    MEMFREE_PTR(dbData);
    MEMFREE_PTR(fileName);
    if (ret != errNone)
    {
        return (false);
    }
    return (true);
}

/*=========================================================================*/
/*   Function :   EntryLogSubFileDatabase                                  */
/*                           「１つ下も取得」の取得情報にデータを登録する  */
/*=========================================================================*/
Err EntryLogSubFileDatabase(NNshGetLogSubFileDatabase *subDb)
{
    Err       ret;
    DmOpenRef dbRef;

    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", DBNAME_TEMPORARYLOG, 0);
        return (~errNone);
    }

    // スレデータを更新
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshGetLogSubFileDatabase), subDb);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "EntryRecord_NNsh() ", DBNAME_TEMPORARYLOG, ret);
    }
    else
    {
        // サブログデータベースを更新したことにする
        NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_LOGSUBDB));
    }
    CloseDatabase_NNsh(dbRef);

    return (ret);
}

/*=========================================================================*/
/*   Function :   GetSubFileDatabase                                       */
/*                   「１つ下も取得」の取得情報が登録済みかどうか確認する  */
/*                             (あれば、subDbにデータをコピーして応答する) */
/*                         ※ 失敗時には、NNSH_DATABASE_BLANKを応答する    */
/*=========================================================================*/
UInt16 GetSubFileDatabase(UInt16 command, UInt32 recId, 
                             Char *dataURL, NNshGetLogSubFileDatabase *subDb)
{
    Err                        ret;
    UInt16                     dbCount, loop, len, matched;
    MemHandle                  dataH;
    DmOpenRef                  dbRef;
    NNshGetLogSubFileDatabase *subDbRec;

    len = StrLen(dataURL);
    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", DBNAME_TEMPORARYLOG, 0);
        return (NNSH_DATABASE_BLANK);
    }
    // レコード件数を取得
    GetDBCount_NNsh(dbRef, &dbCount);
    for (loop = dbCount; loop != 0; loop--)
    {
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &dataH, (void **)&subDbRec);
        if (ret == errNone)
        {
            matched = NNSH_DATABASE_BLANK;
            switch (command)
            {
              case NNSH_GETLOGDB_CHECKPARENT:
                // 親レコードが指定されたもの...
                if (recId == subDbRec->parentDbId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECKTOKEN:
                // 区切りのレコードが指定されたもの...
                if (recId == subDbRec->tokenId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECKURL:
                // URLが指定されたもの...
                matched = StrNCompare(subDbRec->dataURL, dataURL, len);
                break;
              
              default:
                // 上記以外...
                break;
            }
            if (matched == 0)
            {
                // レコードを見つけた。。
                if (subDb != NULL)
                {
                    // レコードのデータをコピーする
                    MemMove(subDb, subDbRec, sizeof(NNshGetLogSubFileDatabase));
                }
                // レコードのロックを開放する
                ReleaseRecordReadOnly_NNsh(dbRef, dataH);

                // DBをクローズする
                CloseDatabase_NNsh(dbRef);
                return (loop - 1);
            }
            // レコードのロックを開放する
            ReleaseRecordReadOnly_NNsh(dbRef, dataH);
        }
    }
    // DBをクローズして応答する
    CloseDatabase_NNsh(dbRef);
    return (NNSH_DATABASE_BLANK);
}

/*=========================================================================*/
/*   Function :   UpdateSubFileDatabase                                    */
/*                                   「１つ下も取得」の取得情報を更新する  */
/*=========================================================================*/
Err UpdateSubFileDatabase(UInt16 index, NNshGetLogSubFileDatabase *subDb)
{
    Err       ret;
    DmOpenRef dbRef;

    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", DBNAME_TEMPORARYLOG, 0);
        return (~errNone);
    }

    // スレデータを更新
    ret = UpdateRecord_NNsh(dbRef,index,sizeof(NNshGetLogSubFileDatabase), subDb);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", DBNAME_TEMPORARYLOG, ret);
    }
    CloseDatabase_NNsh(dbRef);

    // サブログデータベースを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_LOGSUBDB));

    return (ret);
}

/*=========================================================================*/
/*   Function :   PurgeSubFileDatabase                                     */
/*                                 「１つ下も取得」の取得情報をパージする  */
/*=========================================================================*/
UInt16 PurgeSubFileDatabase(UInt16 command, UInt32 recId, UInt8 argument, UInt16 purgeCount)
{
    Err                        ret;
    UInt32                     diffTime;
    UInt16                     dbCount, loop, matched, count;
    MemHandle                  dataH;
    DmOpenRef                  dbRef;
    NNshGetLogSubFileDatabase *subDbRec;

    OpenDatabase_NNsh(DBNAME_TEMPORARYLOG, DBVERSION_TEMPORARYLOG, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", DBNAME_TEMPORARYLOG, 0);
        return (0);
    }

    count = 0;
    // レコード件数を取得
    GetDBCount_NNsh(dbRef, &dbCount);
    for (loop = dbCount; ((loop != 0)&&(count < purgeCount)); loop--)
    {
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &dataH, (void **)&subDbRec);
        if (ret == errNone)
        {
            matched = NNSH_DATABASE_BLANK;
            switch (command)
            {
              case NNSH_GETLOGDB_CHECKPARENT:
                // 親レコード番号が一致する...
                if (recId == subDbRec->parentDbId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECKTOKEN:
                // トークン番号が一致する...
                if (recId == subDbRec->tokenId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECKDATE:
                // n秒より古いサブログは消去する...
                diffTime = TimGetSeconds() - subDbRec->lastUpdate;
                if (diffTime > recId)
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_CHECK_PARENT_DATE:
                // 親レコード番号が一致し、n秒より古いサブログは消去する...
                diffTime = TimGetSeconds() - subDbRec->lastUpdate;
                if ((diffTime > (((UInt32) argument) * 60 * 60))&&(recId == subDbRec->parentDbId))
                {
                    matched = 0;
                }
                break;

              case NNSH_GETLOGDB_FORCE_DELETE:
                // 必ず消す...
                matched = 0;
                break;

              case NNSH_GETLOGDB_CHECKURL:
              default:
                // パージしない...
                break;
            }
            // レコードのロックを開放する
            ReleaseRecordReadOnly_NNsh(dbRef, dataH);

            // purgeに該当するレコードだった場合...
            if (matched == 0)
            {
                // DBを削除する...
                DeleteRecordIDX_NNsh(dbRef, (loop - 1));
                
                // 実ログファイルを消去する...
                if (subDbRec->logDbId != 0)
                {
                    DeleteThreadMessage_RecordID(subDbRec->logDbId);
                }
                count++;
            }
        }
    }
    // DBをクローズして応答する (purge完了)
    CloseDatabase_NNsh(dbRef);
    return (count);
}
#endif  // #ifdef USE_LOGCHARGE
