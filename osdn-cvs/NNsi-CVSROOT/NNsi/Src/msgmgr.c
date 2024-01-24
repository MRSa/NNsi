/*============================================================================*
 *
 *  $Id: msgmgr.c,v 1.97 2012/01/10 16:27:41 mrsa Exp $
 *
 *  FILE: 
 *     msgmgr.c
 *
 *  Description: 
 *     Message(Thread) manager for NNsh. 
 *
 *===========================================================================*/
#define MSGMGR_C
#include "local.h"
extern UInt32 CreateThreadIndexSub_ARM();

/*=========================================================================*/
/*   Function : CreateThreadIndexSub                                       */
/*                                              (インデックス取得の実処理) */
/*=========================================================================*/
static Err createThreadIndexSub(Char   *buffer, UInt32 size, UInt32 offset,
                                UInt32 *nofMsg, UInt32 *msgOffset)
{
    Char *chkPtr, *ptr;
    
    chkPtr = buffer;
    while (chkPtr < buffer + size)
    {
        // メッセージの区切りを探す
        ptr = chkPtr;
        while ((*ptr != '\x0a')&&(*ptr != '\0'))
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            ptr = ptr + 1;
            if (*nofMsg < (NNshGlobal->NNsiParam)->msgNumLimit)
            {
                (*nofMsg)++;
                msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
                chkPtr = ptr;
            }
            else
            {
                return (~errNone - 1);
            }
        }
        else
        {
            // とりあえず次にすすむ
            chkPtr = ptr + 1;
        }
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : get_subject_database                                       */
/*                                                         スレ情報を取得  */
/*=========================================================================*/
Err Get_Subject_Database(UInt16 index, NNshSubjectDatabase *subjDB)
{
    Err       ret;
    DmOpenRef dbRef;

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }

    // スレデータを取得
    ret = GetRecord_NNsh(dbRef, index, sizeof(NNshSubjectDatabase), subjDB);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", " ", ret);
    }
    CloseDatabase_NNsh(dbRef);
    return (ret);
}

/*=========================================================================*/
/*   Function : MoveMessageToReadOnly                                      */
/*                                  (メッセージを参照ログスレに移動させる) */
/*=========================================================================*/
Err MoveMessageToReadOnly(UInt16 index)
{
    Err                 ret;
    NNshSubjectDatabase subjDb;

    ////////////////////  データベースの所属スレ名更新  ////////////////////

    // 領域の初期化
    MemSet(&subjDb, sizeof(subjDb), 0x00);

    // データベースからデータを取得
    ret = Get_Subject_Database(index, &subjDb);
    if (ret != errNone)
    {
        // ありえないはずだが？
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()"," ", ret);
        return (ret);
    }

    // スレ名を「読みこみ専用」に更新
    MemSet (subjDb.boardNick, MAX_NICKNAME, 0x00);
    StrCopy(subjDb.boardNick, OFFLINE_THREAD_NICK);

    // データを更新
    ret = update_subject_database(index, &subjDb);
    if (ret != errNone)
    {
        // ありえないはず...
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()"," ", ret);
        return (ret);
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : checkLogChargeThread                                       */
/*                                        (ログチャージしたスレかチェック) */
/*-------------------------------------------------------------------------*/
static void checkLogChargeThread(Char *fileName, NNshGetLogDatabase *dbData, UInt32 *tokenId) 
{
    DmOpenRef  dbRef;
    Char      *chkPtr;
    UInt16     recNum, depth;

    // ログチャージしたログかどうか確認する
    depth = 1;
    chkPtr = StrStr(fileName, FILE_LOGCHARGE_PREFIX);
    if (chkPtr != NULL)
    {
        // ログチャージしたログと確認、レコード番号を切り出す
        chkPtr = chkPtr +  (sizeof(FILE_LOGCHARGE_PREFIX) - 1);
        recNum = StrAToI(chkPtr);
        if (recNum == NNSH_MSGNUM_UNKNOWN)
        {
            // データがログチャージしたログではない
            // (自動でファイル名を付加したログ)
            MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);
            *tokenId = 0;
            return;
        }

        // 参照ログチャージ用DBの取得
        OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
        if (dbRef != 0)
        {
            // ログDBのオープン成功、切り出したレコード番号データを取得
            MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);

            // レコードの読み出し
            GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), dbData);

            // データベースを閉じる
            CloseDatabase_NNsh(dbRef);
        }

        // 取得深度を確認
        while (*chkPtr != '-')
        {
            chkPtr++;
        }
        chkPtr++;
        depth = StrAToI(chkPtr);
    }
    
    if (depth != 0)
    {
        // 取得深度が違う場合、データをクリアする
        *tokenId = dbData->subTokenId;   
        MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);
    }
    else
    {
        *tokenId = dbData->tokenId;   
    }
    return;
}


/*-------------------------------------------------------------------------*/
/*   Function : createThreadIndexSub_HTML                                  */
/*                                            (HTML用インデックス生成処理) */
/*-------------------------------------------------------------------------*/
static Err createThreadIndexSub_HTML(Char   *buffer,
                                      UInt32 *bufSize, UInt32 offset,
                                      UInt32 *nofMsg,  UInt32 *msgOffset, 
                                      Char *fileName,  UInt16 kanjiCode, 
                                      UInt16 *status,  UInt32 tokenId)
{
    Char *chkPtr, *ptr, *start, *end, *token, dummy, *orgPtr;
    NNshLogTokenDatabase *rec;
    UInt32 size;

    size = *bufSize;   
    NNsh_DebugMessage(ALTID_INFO, "createThreadIndexSub_HTML", "", 0);

    // データの末尾を見つける
    if (*status == NNSH_MSGTOKENSTATE_END)
    {
        // 区切りデータの末尾まで呼び出していた、メッセージ解析処理をしない
        return (errNone);
    }

    rec = MEMALLOC_PTR(sizeof(NNshLogTokenDatabase) + MARGIN);
    if (rec == NULL)
    {
        // データが指定されていないとき、データをダミーに置き換える
        dummy  = 0;
        start = &dummy;
        end   = &dummy;
        token = &dummy;
    }
    else
    {
        MemSet(rec, (sizeof(NNshLogTokenDatabase) + MARGIN), 0x00);
#ifdef USE_LOGCHARGE
        if (GetLogToken(tokenId, rec) != errNone)
        {
            // データが指定されていないとき、データをダミーに置き換える
            dummy  = 0;
            start = &dummy;
            end   = &dummy;
            token = &dummy;
            MEMFREE_PTR(rec);
            rec = NULL;
        }
        else
        {
            start = rec->startToken;
            end   = rec->endToken;
            token = rec->dataToken;
        }
#else   // #ifdef USE_LOGCHARGE
        {
            // データが指定されていないとき、データをダミーに置き換える
            dummy  = 0;
            start = &dummy;
            end   = &dummy;
            token = &dummy;
            MEMFREE_PTR(rec);
            rec = NULL;
        }
#endif   // #ifdef USE_LOGCHARGE
    }

    // データの先頭を見つける
    chkPtr = buffer;
    if (*status == NNSH_MSGTOKENSTATE_NONE)
    {
        //  先頭の区切り文字列を探す
        if (*start != 0)
        {
            ptr = StrStr(buffer, start);
            if (ptr == NULL)
            {
                // データ先頭の区切りが見つからなかった、解析終了
                MEMFREE_PTR(rec);
                *bufSize = *bufSize - StrLen(start); // 区切り文字分戻す...
                return (errNone);
            }
            ptr = ptr + StrLen(start);
            msgOffset[0] = ((UInt32) (ptr - buffer)) + offset;
            chkPtr = ptr;
            *status = NNSH_MSGTOKENSTATE_MESSAGE;
        }
        else
        {
            *status = NNSH_MSGTOKENSTATE_MESSAGE;
        }
    }
    if (*status == NNSH_MSGTOKENSTATE_MESSAGE)
    {
        // 末尾の区切り文字が入っているか探す
        if (*end != 0)
        {
            ptr = StrStr(buffer, end);
            if (ptr != NULL)
            {
                // 末尾データを発見！ データはそこまでとする
                *status = NNSH_MSGTOKENSTATE_END;
                size = (ptr - buffer) + StrLen(end);
            }
        }
    }

    // データの区切り文字列を探す
    if (*token != 0)
    {
        while (chkPtr < buffer + size)
        {
            // メッセージの区切りを探す
            ptr = StrStr(chkPtr, token);
            if (ptr == NULL)
            {
                // 区切りがないから、解析終了
                *bufSize = *bufSize - StrLen(token); // 区切り文字分戻す...
                break;
            }
            ptr = ptr + StrLen(token);
            if (*nofMsg < (NNshGlobal->NNsiParam)->msgNumLimit)
            {
                (*nofMsg)++;
                msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
                chkPtr = ptr;
            }
            else
            {
                // メッセージの格納可能な最大数を超えた(オーバフロー表示)
                NNsh_WarningMessage(ALTID_WARN, "Index Number overflow", "", 0);
                MEMFREE_PTR(rec);
                return (~errNone - 1);
            }
        }
    }
    else
    {
        // 仮のパース処理(BIGBUF * 2(1280bytes)で、インデックスを作成する)
        // ただし、トークン'>'で区切る。
        ptr = chkPtr;
        while(chkPtr < buffer + size)
        {
            ptr = ptr + BIGBUF * 2;
            if (ptr > buffer + size)
            {
                ptr = buffer + size;
            }
            orgPtr = ptr;
            if (kanjiCode == NNSH_KANJICODE_JIS)
            {
                // JIS漢字コードのとき、、、
                while ((*ptr != '\0')&&(ptr > buffer))
                {
                    // JIS漢字コードでは、タグのコードが混じるため、改行コードで切る
                    if (*ptr == 0x0a)
                    {
                        ptr++;
                        break;
                    }
                    // エスケープ文字でも区切る
                    if (*ptr == 0x1b)
                    {
                        break;
                    }
                    ptr--;
                }
            }
            else
            {
                // 漢字コードがJIS以外
                while ((*ptr != '\0')&&(ptr > chkPtr))
                {
                    // タグまたは改行コードで切る 
                    if ((*ptr == '>')||(*ptr == '\x0a'))
                    {
                        ptr++;
                        break;
                    }
                    ptr--;
                }
                if (ptr == chkPtr)
                {
                    // タグが含まれていなかった...
                    ptr = orgPtr;
                }
            }

            if (*nofMsg < (NNshGlobal->NNsiParam)->msgNumLimit)
            {
                (*nofMsg)++;
                msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
                chkPtr = ptr;
            }
            else
            {
                // メッセージの格納可能な最大数を超えた(オーバフロー表示)
                NNsh_WarningMessage(ALTID_WARN, "Index Number overflow", "", 0);
                MEMFREE_PTR(rec);
                return (~errNone - 1);
            }
        }
    }
    MEMFREE_PTR(rec);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : createThreadIndexSub_MachiBBS                              */
/*                            (インデックス取得の実処理:まちBBS用チューン) */
/*-------------------------------------------------------------------------*/
static Err createThreadIndexSub_MachiBBS(Char  *buffer,    UInt32  size,
                                         UInt32  offset,    UInt32 *nofMsg, 
                                         UInt32 *msgOffset, UInt16 endTokenType, 
                                         Char   *start,     Char   *end)
{
    Char   *chkPtr, *ptr, *stPtr;

    chkPtr = buffer;

    // 先頭から読み出したとき、メッセージの先頭を特定する
    if (*nofMsg == 0)
    {
        ptr = StrStr(chkPtr, start);
        if (ptr == NULL)
        {
            // 区切りが見つからなかった、解析終了(エラー応答)
            NNsh_DebugMessage(ALTID_ERROR, "Not Found Message Top :", start, 0);
            return (~errNone - 2);
        }
        ptr = ptr + StrLen(start);
        msgOffset[0] = ((UInt32) (ptr - buffer)) + offset;
        chkPtr = ptr;
    }
    while (chkPtr < buffer + size)
    {
        // メッセージの区切りを探す
        ptr = StrStr(chkPtr, end);
        if ((ptr == NULL)||(ptr > (buffer + size)))
        {
            // 区切りがないから、解析終了

            // でも、コメント開始の文字列が(区切りの後ろに)あった場合には、そこを最終のレス位置とする
            stPtr = StrStr(chkPtr, NNSH_MACHITOKEN_BEGINCOMMENT);
            if ((stPtr != NULL)&&(stPtr <= (buffer + size)))
            {
                (*nofMsg)++;
                msgOffset[(*nofMsg)] = (UInt32) stPtr - (UInt32) buffer + offset;
            }
            break;
        }

        // 区切りトークンタイプの指定を確認
        if (endTokenType != NNSH_DISABLE)
        {
            // 区切りトークンタイプの指定があった場合...

            // 区切りトークンの前にあるデータを確認する
            if ((endTokenType & NNSH_TOKEN_BEFORE_NUMBER) == NNSH_TOKEN_BEFORE_NUMBER)
            {
                // 区切りトークンの前データは数字でなければならない
                if ((*(ptr - 1) < '0')||(*(ptr - 1) > '9'))
                {
                    // 区切りトークンではなかった、次を解析
                    chkPtr = ptr + StrLen(end);
                    continue;
                }
            }
            if ((endTokenType & NNSH_TOKEN_BEFORE_LETTER) == NNSH_TOKEN_BEFORE_LETTER)
            {
                // 区切りトークンの前データは文字でなければならない
                if (*(ptr - 1) <= ' ')
                {
                    // 区切りトークンではなかった、次を解析
                    chkPtr = ptr + StrLen(end);
                    continue;
                }
            }

            // 区切りトークンの次にポインタを進める
            ptr = ptr + StrLen(end);

            // 区切りトークンの後にあるデータを確認する
            if ((endTokenType & NNSH_TOKEN_AFTER_NUMBER) == NNSH_TOKEN_AFTER_NUMBER)
            {
                // 区切りトークンの後データは数字でなければならない
                if ((*(ptr + 1) < '0')||(*(ptr + 1) > '9'))
                {
                    // 区切りトークンではなかった、次を解析
                    chkPtr = ptr;
                    continue;
                }
            }
            if ((endTokenType & NNSH_TOKEN_AFTER_LETTER) == NNSH_TOKEN_AFTER_LETTER)
            {
                // 区切りトークンの後データは文字でなければならない
                if (*(ptr + 1) <= ' ')
                {
                    // 区切りトークンではなかった、次を解析
                    chkPtr = ptr;
                    continue;
                }
            }
        }
        else
        {      
            // 区切りデータタイプの指定がなかった場合には、
            // 無条件に区切りと判断し、区切りトークンの次にポインタを進める
            ptr = ptr + StrLen(end);

            // 区切り文字の次に開始文字列がない場合にはまだ１つのレスとして判定してみる。
            stPtr = StrStr((ptr - 1), start);
            if (stPtr != (ptr - 1))
            {
                // 区切りトークンではなかった、次を解析
                chkPtr = ptr;
                continue;
            }
        }

        if (*nofMsg < (NNshGlobal->NNsiParam)->msgNumLimit)
        {
            (*nofMsg)++;
            msgOffset[(*nofMsg)] = (UInt32) ptr - (UInt32) buffer + offset;
            chkPtr = ptr;
        }
        else
        {
            // メッセージの格納可能な最大数を超えた(オーバフロー表示)
            NNsh_WarningMessage(ALTID_WARN, "Index Number overflow", "", 0);
            return (~errNone - 1);
        }
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : createMessageThreadIndex                                   */
/*                                (メッセージスレッドのインデックス作成)   */
/*=========================================================================*/
Err CreateMessageThreadIndex(NNshMessageIndex *index, UInt32 *nofMsg,
                             UInt32 *msgOffset, UInt16 bbsType)
{
    Err          ret  = errNone;
    UInt32       fileSize, readSize, chkPoint;
    Char        *bufPtr, *ptr;
    NNshGetLogDatabase *dbData;
#ifdef USE_ARMLET
    UInt32       armArgs[7];
    MemHandle    armH;
    void        *armP;

    // ARMletの関数ポインタを取得する
    armH = 0;
    armP = NULL;
    if ((NNshGlobal->NNsiParam)->useARMlet != 0)
    {
        armH = DmGetResource('armc', RSCID_createThreadIndexSub_ARM);
        if (armH != 0)
        {
            armP = MemHandleLock(armH);
        }
    }
#endif

    // インデックス作成中表示
    Show_BusyForm(MSG_CREATE_MESSAGE_IDX);

    dbData = NULL;
    if ((bbsType & NNSH_BBSTYPE_MASK)  == NNSH_BBSTYPE_HTML)
    {
        // htmlタイプデータのときは、DBから情報を取得しておく
        dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
        if (dbData != NULL)
        {
            MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);

            // ログチャージのスレかどうかチェックする
            checkLogChargeThread(index->threadFile, dbData, &fileSize);
            if (NNshGlobal->tokenId == 0)
            {
                // 初期のインデックス解析データをパラメータに反映させる
                NNshGlobal->tokenId = fileSize;
            }

            // ベースになるURLの切り出しを実行
            if (dbData->dataURL[0] != 0x00)
            {
                StrNCopy(index->baseURL, dbData->dataURL, MAX_URL);
                for (ptr = index->baseURL + StrLen(index->baseURL) - 1;
                     ((*ptr != '/')&&(ptr > index->baseURL)); ptr--)
                {
                    // 何もしない
                }
                ptr++;
                *ptr = '\0';
            }
        }
    }

    // 一時的に読み込む領域の初期化
    bufPtr   = index->buffer;
    *nofMsg  = 0;
    fileSize = 0;
    while (fileSize < index->fileSize)
    {
        MemSet(bufPtr, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
        readSize = 0;
        ret     = ReadFile_NNsh(&(index->fileRef), fileSize,
                                 (NNshGlobal->NNsiParam)->bufferSize, bufPtr, &readSize);
        switch (ret)
        {
            case vfsErrFileEOF:
            case fileErrEOF:
            case errNone:
              // read OK, create Index!
              break;

            default:
              // read failure!
              goto FUNC_END;
              break;
        }
        switch (bbsType & NNSH_BBSTYPE_MASK)
        {
          case NNSH_BBSTYPE_MACHIBBS:
            //  まちBBS/したらば＠ＪＢＢＳはトークンが長いため、トークンの
            // 途中でバッファが尽きたときの対策を入れた(つもり)。
            if (ret == errNone)
            {
                chkPoint = readSize - sizeof(NNSH_MACHITOKEN_END) + 2;
            }
            else
            {
                chkPoint = readSize;
            }
            //  まちBBSのインデックス解析処理(html解析)
            ret = createThreadIndexSub_MachiBBS(index->buffer, readSize,
                                                fileSize, nofMsg, msgOffset,
                                                NNSH_DISABLE,
                                                NNSH_MACHITOKEN_START,
                                                NNSH_MACHITOKEN_END);
            readSize = chkPoint;
            if (readSize < 10)
            {
                // 最終バイトが拾えないので、無理やり...
                readSize = 10;
            }
            break;

          case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
            //  まちBBS/したらば＠ＪＢＢＳはトークンが長いため、トークンの
            // 途中でバッファが尽きたときの対策を入れた(つもり)。
            if (ret == errNone)
            {
                chkPoint = readSize - sizeof(NNSH_JBBSTOKEN_END) + 2;
            }
            else
            {
                chkPoint = readSize;
            }
            //  まちBBSのインデックス解析処理(html解析)
            ret = createThreadIndexSub_MachiBBS(index->buffer, readSize,
                                                fileSize, nofMsg, msgOffset,
                                                NNSH_DISABLE,
                                                NNSH_JBBSTOKEN_START,
                                                NNSH_JBBSTOKEN_END);
            readSize = chkPoint;
            if (readSize < 10)
            {
                // 最終バイトが拾えないので、無理やり...
                readSize = 10;
            }
            break;

          case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
            //  まちBBS/したらば＠ＪＢＢＳはトークンが長いため、トークンの
            // 途中でバッファが尽きたときの対策を入れた(つもり)。
            if (ret == errNone)
            {
                chkPoint = readSize - sizeof(NNSH_JBBSTOKEN_RAW_END_RES) + 2;
            }
            else
            {
                chkPoint = readSize;
            }
            //  まちBBSのインデックス解析処理(html解析)
            ret = createThreadIndexSub_MachiBBS(index->buffer, readSize,
                                                fileSize, nofMsg, msgOffset,
                                                (NNSH_TOKEN_BEFORE_LETTER),
                                                NNSH_JBBSTOKEN_RAW_START_RES,
                                                NNSH_JBBSTOKEN_RAW_END_RES);
            readSize = chkPoint;
            if (readSize < 10)
            {
                // 最終バイトが拾えないので、無理やり...
                readSize = 10;
            }
            break;

          case NNSH_BBSTYPE_HTML:
            //  chkPointがゼロのときには、区切り文字列内でバッファが尽きた
            // ときの対策を実施する (区切り文字列長が可変のため...)
            if (ret == errNone)
            {
                chkPoint = 0;
            }
            else
            {
                chkPoint = readSize;
            }
            // HTMLタイプのインデックス処理解析
            ret = createThreadIndexSub_HTML(index->buffer, &readSize, 
                                            fileSize, nofMsg, msgOffset,
                                            index->threadFile, 
                                            ((bbsType & NNSH_BBSTYPE_CHARSETMASK) >> NNSH_BBSTYPE_CHAR_SHIFT),
                                            &(index->msgStatus), NNshGlobal->tokenId);
           if (chkPoint != 0)
           {
               readSize = chkPoint;
           }
           break;

#if 0
          case NNSH_BBSTYPE_2ch:
          case NNSH_BBSTYPE_2ch_EUC:
          case NNSH_BBSTYPE_SHITARABA:
          case NNSH_BBSTYPE_OTHERBBS:
          case NNSH_BBSTYPE_OTHERBBS_2:
#endif
          default:
            //  2ちゃんねる/したらばのインデックス解析処理(dat直読み)
#ifdef USE_ARMLET
            if (armP != NULL)
            {
                // ARMletを使用してのインデックス解析処理
                MemSet(armArgs, sizeof(armArgs), 0x00);
                armArgs[0] = (UInt32) msgOffset;
                armArgs[1] = (UInt32) nofMsg;
                armArgs[2] = (UInt32) fileSize;
                armArgs[3] = (UInt32) (NNshGlobal->NNsiParam)->msgNumLimit;
                armArgs[4] = (UInt32) readSize;
                armArgs[5] = (UInt32) index->buffer;
                ret        = (Err) PceNativeCall(armP, armArgs);
            }
            else
#endif
            {
                // ARMletを使用しない場合のインデックス解析処理
                ret = createThreadIndexSub(index->buffer, readSize,
                                           fileSize, nofMsg, msgOffset);
            }
            break;
        }
        if (ret != errNone)
        {
            // インデックスオーバフロー発生(ありえないはず...)、途中でやめる
            // メッセージの格納可能な最大数を超えた(オーバフロー表示)
            NNsh_WarningMessage(ALTID_WARN, "Index Number overflow", "", 0);

            // とりあえず読めるようにする(errNoneに置換)
            ret = errNone;
            break;
        }

        // 次回読み込み位置を fileSizeに設定する。
        fileSize = fileSize + readSize;        
    }

FUNC_END:
#ifdef USE_ARMLET
    // ARMletの関数を解放
    if (armP != NULL)
    {
        MemHandleUnlock(armH);
    }
    if (armH != 0)
    {
        DmReleaseResource(armH);
    }
#endif
    MEMFREE_PTR(dbData);
    Hide_BusyForm(true);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : getLastMessageNumShitaraba                                 */
/*                                 スレに格納されているレスの最大数を取得  */
/*-------------------------------------------------------------------------*/
Err getLastMessageNumShitaraba(UInt16 *resNum)
{
    Err           ret;
    UInt32        fileSize, offset, readSize;
    Char         *realBuf, *ptr, *bottom;
    NNshFileRef   fileRef;

    // 初期化
    *resNum = 1;

    // "メッセージ変換中" 表示を行う
    Show_BusyForm(MSG_MESSAGE_CHECK_LAST);

    // 一時バッファ領域を確保
    realBuf = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        // 一時領域の確保に失敗した
        Hide_BusyForm(false);
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        return (~errNone);
    }
    MemSet(realBuf, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    // 読み出しファイルのオープン
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        Hide_BusyForm(false);
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_RECVMSG, ret);
        return (ret);
    }

    // 受信メッセージサイズを取得
    GetFileSize_NNsh(&fileRef, &fileSize);

    // ファイルの読み出し位置を設定する
    if (fileSize <= (NNshGlobal->NNsiParam)->bufferSize)
    {
        offset = 0;
    }
    else
    {
        offset = fileSize - (NNshGlobal->NNsiParam)->bufferSize;
    }

    // ファイルからデータを読み出す(ファイルの末尾部分)
    ret = ReadFile_NNsh(&fileRef, offset, (NNshGlobal->NNsiParam)->bufferSize, realBuf, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :",  FILE_RECVMSG, ret);
        goto END_FUNC;
    }

    //////////////////////////////////////////// レス番号の検索処理
    ptr = realBuf + readSize;
    while ((ptr - 1) > realBuf)
    {
        // レス番号が記入されている個所を探す(バッファ末尾にある/\x0a[0-9]+<>/)
        if (((*ptr >= '0')&&(*ptr <= '9'))&&(*(ptr - 1) == '\x0a'))
        {
            // 数値データを切り出す
            bottom = ptr;
            while ((*bottom >= '0')&&(*bottom <= '9'))
            {
                bottom++;
            }
            if ((*bottom == '<')&&(*(bottom + 1) == '>'))
            {
                *bottom = '\0';
                *resNum = StrAToI(ptr);
                goto END_FUNC;
            }
        }
        ptr--;         
    }
    ////////////////////////////////////////////

END_FUNC:    
    MEMFREE_PTR(realBuf);
    Hide_BusyForm(false);

    NNsh_DebugMessage(ALTID_INFO, "RES NUM :",  "", *resNum);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : update_subject_database                                    */
/*                                                         スレ情報を更新  */
/*-------------------------------------------------------------------------*/
Err update_subject_database(UInt16 index, NNshSubjectDatabase *subjDB)
{
    Err       ret;
    DmOpenRef dbRef;

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }

    // スレデータを更新
    ret = UpdateRecord_NNsh(dbRef,index,sizeof(NNshSubjectDatabase),subjDB);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", " ", ret);
    }
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_entryDatabase                                    */
/*                                                メッセージ情報をDBに登録 */
/*-------------------------------------------------------------------------*/
static void message_entryDatabase(UInt16 idxMES,UInt32 fileSize,UInt16 bbsType)
{
    Err                 ret;
    NNshSubjectDatabase subjDb;

    // 領域の初期化
    MemSet(&subjDb, sizeof(subjDb), 0x00);

    if (idxMES != NNSH_NOTENTRY_THREAD)
    {
        // データベースからデータを取得
        ret = Get_Subject_Database(idxMES, &subjDb);
        if (ret != errNone)
        {
            // ありえないはずだが？
            NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()"," ", ret);
            return;
        }
    }

    ///// DBに書きこむデータを作成する /////

    // 状態値ほか
    if (fileSize != 0)
    {
        subjDb.state        = NNSH_SUBJSTATUS_NEW;
        subjDb.currentLoc   = 1;
        subjDb.fileSize     = fileSize;
        subjDb.bbsType      = (UInt8) bbsType;
        subjDb.msgAttribute = (subjDb.msgAttribute & NNSH_MSGATTR_NOTERROR);

        // スレの記録先を登録
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // スレ記録先はVFS
            subjDb.msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // スレ記録先はPalm本体
            subjDb.msgState = FILEMGR_STATE_OPENED_STREAM;
        }

        // したらば＠JBBS(rawmode)のときは、末尾レス番号を取得する
        if (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)
        {
            // 末尾のレス番号を取得する
            (void) getLastMessageNumShitaraba(&(subjDb.maxLoc));
        }
    }
    else
    {
        subjDb.state      = NNSH_SUBJSTATUS_NOT_YET;
        subjDb.currentLoc = 0;
        subjDb.fileSize   = 0;
        subjDb.bbsType    = (UInt8) bbsType;
    }

    // データを更新
    ret = update_subject_database(idxMES, &subjDb);
    if (ret != errNone)
    {
        // ありえないはず...
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()","",ret);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_entryMessage                                     */
/*                                                  メッセージを変換、登録 */
/*-------------------------------------------------------------------------*/
static Err message_entryMessage(Char *targetFile, UInt32 *dataSize, UInt16 bbsType)
{
    Err           ret;
    Boolean       deleteLogFile;
    UInt32        fileSize, offset, readSize, writeSize, chkSize;
    Char         *buffer, *ptr, *realBuf, *bakPtr;
    NNshFileRef  fileRefR, fileRefW;

    deleteLogFile = false;
    fileSize  = 0;
    offset    = 0;

    // "メッセージ変換中" 表示を行う
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    // データサイズを初期化
    *dataSize = 0;

    // 読み出しファイルのオープン
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        Hide_BusyForm(false);
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_RECVMSG, ret);
        return (ret);
    }

    // 一時バッファ領域を確保
    realBuf = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        // 一時領域の確保に失敗した
        Hide_BusyForm(false);
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        CloseFile_NNsh(&fileRefR);
        return (~errNone);
    }
    buffer = realBuf;
    MemSet(buffer, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    // 受信メッセージサイズを取得
    GetFileSize_NNsh(&fileRefR, &fileSize);

    // 空き容量の確認(VFSのみ)
    if (CheckFileSpace_NNsh(NNSH_DISABLE, fileSize) != errNone)
    {
        // 空き容量不足、終了する。
        ret = ~errNone - 10;
        goto END_FUNC;
    }

    // 初回データの読み込み
    ret = ReadFile_NNsh(&fileRefR, offset,
                        (NNshGlobal->NNsiParam)->bufferSize, buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_DebugMessage(ALTID_ERROR, "ReadFile_NNsh :",  FILE_RECVMSG, ret);
        goto END_FUNC;
    }
    ret = errNone;

    // 分割ダウンロード時は、ファイルの最後を細工する(ちょっと時間かかるけど)
    if ((NNshGlobal->NNsiParam)->enablePartGet != 0)
    {
        while ((readSize != 0)&&
               (*((Char *) (buffer + readSize - 1)) != '\x0a'))
        {
            readSize--;
        }
        // 強制的に受信ファイルサイズをメッセージの最後に調整する。
        fileSize = readSize;
    }

    // HTTPヘッダの終了部を見つける
    ptr = StrStr(buffer, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // ボディデータなし
        NNsh_DebugMessage(ALTID_ERROR, "Wrong data(not found separator,",
                          "[\xd\xa\xd\xa])", 0);
        ret = ~errNone + 3;
        goto END_FUNC;
    }

    // HTTPの応答コードをチェックする(HTTP/1.[01] 20[06]を受入れる)
    //   StrLen("HTTP/1.1 200") ==> 12 に置換(高速化のため)
#ifdef USE_STRNCOMPARE
    if ((StrNCompare(buffer,"HTTP/1.1 200",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.1 206",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.0 206",12) != 0)&&
        (StrNCompare(buffer,"HTTP/1.0 200",12) != 0))
#else
    if ((buffer[0] != 'H')||(buffer[1] != 'T')||(buffer[2] != 'T')||
        (buffer[3] != 'P')||(buffer[4] != '/')||(buffer[5] != '1')||
        (buffer[6] != '.')||((buffer[7] != '1')&&(buffer[7] != '0'))||
        (buffer[8] != ' ')||(buffer[9] != '2')||(buffer[10] != '0')||
        ((buffer[11] != '0')&&(buffer[11] != '6')))
#endif
    {
#ifdef USE_STRSTR
        ptr = StrStr(buffer, "\x0a");
#else
        ptr = buffer;
        while ((*ptr != '\x0a')&&(*ptr != '\0'))
        {
            ptr++;
        }
#endif
        *ptr = '\0';
        if (*(ptr - 1) == '\x0d')
        {
            *(ptr - 1) = '\0';
        }
#ifdef USE_ZLIB
        // サーバレスポンス異常
        if ((NNshGlobal->useGzip != 0)&&(NNshGlobal->loginDateTime != 0)&&
            (NNshGlobal->getRetry == 0))
        {
            NNsh_InformMessage(ALTID_WARN, MSG_RETRY_OYSTERLOG, buffer, 0);
        }
        else
#endif // #ifdef USE_ZLIB
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_RECV_NOTACCEPTED, buffer, 0);
        }
        ret = (NNSH_ERRORCODE_NOTACCEPTEDMSG);
        goto END_FUNC;
    }
    ptr = ptr + 4;
    while ((*ptr == '\x0a')||(*ptr == '\x0d'))  // 過去ログ用に補正...
    {
        ptr++;
    }
    *dataSize = fileSize - (ptr - buffer);

    switch (bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
#if 0
        //  まちBBSのときには、メッセージの先頭までデータの
        // 読み飛ばしを行う
        bakPtr = ptr;
        ptr = StrStr(ptr, NNSH_MACHITOKEN_START);
        if (ptr == NULL)
        {
            ptr = bakPtr;
        }
#endif
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
#if 0
        bakPtr = ptr;
        ptr = StrStr(ptr, NNSH_JBBSTOKEN_START);
        if (ptr == NULL)
        {
            ptr = bakPtr;
        }
#endif
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        bakPtr = ptr;
        ptr = StrStr(ptr, NNSH_JBBSTOKEN_RAW_START);
        if (ptr == NULL)
        {
            ptr = bakPtr;
        }
        else
        {
            // ptr = ptr + StrLen(NNSH_JBBSTOKEN_RAW_START);
            // データの先頭を<>に置き換える。
            *ptr       = '<';
            *(ptr + 1) = '>';
        }

        // [MEMO]
        // 受信したファイルのサイズを減らす？(末尾に7バイト何かがついている模様...)
        //
        break;

#if 0
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

    // 奇数データ書き込みのチェック(むりやり偶数に調整する)
    if (((UInt32) ptr & 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
    {
        ptr--;
        *ptr = ' ';
    }

    chkSize = readSize - (ptr - buffer);
    if (((chkSize % 2) == 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
    {
        if (ptr[chkSize - 1] == '\x0a')
        {
            ptr[chkSize]     = ptr[chkSize - 1];
            ptr[chkSize - 1] = ' ';
            NNsh_DebugMessage(ALTID_WARN, "APPEND SPACE(pre)", "", chkSize);
        }
        else
        {
            ptr[chkSize]     = ' ';
            NNsh_DebugMessage(ALTID_WARN, "APPEND SPACE(post)", "", chkSize);
        }
        chkSize++;
    }

    // 既にファイルが存在した場合は一旦削除してからオープンする。
    (void) DeleteFile_NNsh(targetFile, NNSH_VFS_ENABLE);
    ret = OpenFile_NNsh(targetFile,
                        NNSH_FILEMODE_APPEND, &fileRefW);
    if (ret != errNone)
    {
        // 書き込みファイルのオープンに失敗した
        NNsh_InformMessage(ALTID_ERROR,
                           MSG_WRITEFILE_OPENFAIL, targetFile, ret);
        deleteLogFile = false;
        *dataSize = 0;
        goto END_FUNC;
    }

    // データの書き込み(追記)
    ret = AppendFile_NNsh(&fileRefW, chkSize, ptr ,&writeSize);
    if (ret != errNone)
    {
        CloseFile_NNsh(&fileRefW);
        NNsh_InformMessage(ALTID_ERROR, "WriteFile_NNsh(1st)", "", ret);
        deleteLogFile = true;
        *dataSize = 0;
        ret = ~errNone + 11;
        goto END_FUNC;
    }
    offset = readSize;

    // データが最後になるまで追記する
    while (offset < *dataSize)
    {
        MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize + MARGIN, 0x00);
        readSize = 0;
        ret = ReadFile_NNsh(&fileRefR, offset, (NNshGlobal->NNsiParam)->bufferSize,
                            buffer, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh()", "", ret);
            deleteLogFile = true;
            *dataSize = 0;
            break;
        }
        ret = errNone;
        chkSize = 0;
        if (((readSize % 2) == 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
        {
            if (ptr[readSize - 1] == '\x0a')
            {
                buffer[readSize]     = buffer[readSize - 1];
                buffer[readSize - 1] = ' ';
                NNsh_DebugMessage(ALTID_WARN, "APPEND spase(pre)", "",
                                  readSize);
            }
            else
            {
                buffer[readSize] = ' ';
                NNsh_DebugMessage(ALTID_WARN, "APPEND spase(post)",
                                  "", readSize);
            }
            chkSize++;
        }
        ret = AppendFile_NNsh(&fileRefW, readSize+chkSize, buffer, &writeSize);
        if (ret != errNone)
        {
            NNsh_InformMessage(ALTID_ERROR, "AppendFile_NNsh()", "", ret);
            deleteLogFile = true;
            *dataSize = 0;
            break;
        }
        offset = offset + readSize;
    }
    CloseFile_NNsh(&fileRefW);

END_FUNC:
    CloseFile_NNsh(&fileRefR);
    MEMFREE_PTR(realBuf);
    if (deleteLogFile != false)
    {
        // 中途半端なログファイルが残っているときには、ファイルを削除する。
        (void) DeleteFile_NNsh(targetFile, NNSH_VFS_ENABLE);
    }
    Hide_BusyForm(false);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshMSG_appendMessage                                    */
/*                                            メッセージを変換、登録(追記) */
/*-------------------------------------------------------------------------*/
static Err message_appendMessage(Char *targetFile, NNshSubjectDatabase *mesDB)
{
    Err          ret;
    UInt32       fileSize, offset, readSize, writeSize, parseSize,chkSize;
    Char        *buffer, *ptr, *realBuf, tmpBuf[TINYBUF + MARGIN], *tmpPtr;
    NNshFileRef  fileRefR, fileRefW;

    // データ登録中表示
    Show_BusyForm(MSG_MESSAGE_ENTRY);

    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRefR);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :",FILE_RECVMSG, ret);
    }

    // 一時バッファ領域を確保
    realBuf = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (realBuf == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR()", " size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        ret = errNone - 10;
        goto END_FUNC;
    }
    buffer = realBuf;

    // 受信メッセージのサイズを取得(ヘッダ込み)
    GetFileSize_NNsh(&fileRefR, &fileSize);

    // 空き容量の確認(VFSのみ)
    if (CheckFileSpace_NNsh(NNSH_DISABLE, fileSize) != errNone)
    {
        // 空き容量不足、終了する。
        ret = ~errNone;
        goto END_FUNC;
    }

    parseSize = 0;
    readSize  = 0;
    offset    = 0;

    // 初回データの読み込み
    MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize + MARGIN, 0x00);
    ret = ReadFile_NNsh(&fileRefR, offset, (NNshGlobal->NNsiParam)->bufferSize,
                        buffer, &readSize);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh()", "", ret);

        goto END_FUNC;
    }

    // 分割ダウンロード時は、ファイルの最後を細工する(ちょっと時間かかるけど)
    if ((NNshGlobal->NNsiParam)->enablePartGet != 0)
    {
        while ((readSize != 0)&&
               (*((Char *) (buffer + readSize - 1)) != '\x0a'))
        {
            readSize--;
        }
        // 強制的に受信ファイルサイズをメッセージの最後に調整する。
        fileSize = readSize;
    }

    // HTTPボディ部を見つける
    ptr = StrStr(buffer, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // データ異常
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(" NNSH_HTTP_HEADER_ENDTOKEN ")", 0);
        goto END_FUNC;
    }
    parseSize = ptr - buffer;
    ptr       = ptr + 4;
    parseSize = parseSize + 4;

    // したらば@JBBS(rawmode)以外は、応答チェックをする
    if (mesDB->bbsType != NNSH_BBSTYPE_SHITARABAJBBS_RAW)
    {
        // HTTPの応答コードチェック (判定を数字部分のみにする)
        // StrLen("HTTP/1.1 200") ==> 12 に置換(高速化のため)
        // if ((StrNCompare(buffer,"HTTP/1.1 200 OK", 12) == 0)||
        //    (StrNCompare(buffer,"HTTP/1.0 200 OK", 12) == 0))
        if ((buffer[9] == '2')&&(buffer[10] == '0')&&(buffer[11] == '0'))
        {
            // なんか知らんけど、全メッセージ飛んできた(かな？)。
            CloseFile_NNsh(&fileRefR);
            MEMFREE_PTR(realBuf);
            Hide_BusyForm(false);

            // メッセージすべてが飛んできたことをDEBUG表示
            NNsh_DebugMessage(ALTID_INFO, "Received ALL MESSAGE!! ", targetFile, 0);

            // すべてのデータを受信する処理に切り替える
            ret = errNone;
            ret = message_entryMessage(targetFile, &fileSize, NNSH_BBSTYPE_2ch);

            mesDB->state      = NNSH_SUBJSTATUS_NEW;
            mesDB->fileSize   = fileSize;

            // スレの記録先を登録
            if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
            {    
                // スレ記録先はVFS
                mesDB->msgState = FILEMGR_STATE_OPENED_VFS;
            }
            else
            {    
                // スレ記録先はPalm本体
                mesDB->msgState = FILEMGR_STATE_OPENED_STREAM;
            }
            return (ret);
        }

        // 差分取得したデータで、ボディが飛んでこなかった場合...
        if ((parseSize + 1 == readSize)&&(*ptr == '\x0a'))
        {
            // スレ更新なし
            CloseFile_NNsh(&fileRefR);
            MEMFREE_PTR(realBuf);
            Hide_BusyForm(false);
            return (NNSH_MSGSTATE_NOTMODIFIED);
        }
        if (*ptr != '\x0a')
        {
            // データエラー？(エラーの詳細は未チェック)
            CloseFile_NNsh(&fileRefR);
            MEMFREE_PTR(realBuf);
            Hide_BusyForm(false);

            return (NNSH_MSGSTATE_ERROR);
        }
        // 更新チェック用の部分(1バイト)をすすめる。
        ptr++;
        parseSize++;
    }
    else
    {
        // したらば@JBBS(rawmode)の差分取得
        MemSet (tmpBuf, sizeof(tmpBuf), 0x00);
        NUMCATI(tmpBuf, (mesDB->maxLoc + 1));
        StrCat (tmpBuf, "<>");
        tmpPtr = StrStr(ptr, tmpBuf);
        if (tmpPtr == NULL)
        {
            // スレ更新なし
            CloseFile_NNsh(&fileRefR);
            MEMFREE_PTR(realBuf);
            Hide_BusyForm(false);
            return (NNSH_MSGSTATE_NOTMODIFIED);
        }

        // データを調整
#if 0
        *(tmpPtr - 3) = '<';
        *(tmpPtr - 2) = '>';
        *(tmpPtr - 1) = '\x0a';
        *(tmpPtr - 3) = '<';
        *(tmpPtr - 2) = '>';
        *(tmpPtr - 1) = '\x0a';
#endif
        parseSize = parseSize + (tmpPtr - 3) - ptr;
    }

    // ヘッダ部分を削ったファイルサイズに変更する
    fileSize = fileSize - (ptr - buffer);

    if (((UInt32) ptr & 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
    {
        ptr--;
        *ptr = ' ';
    }
     // データの書き込み(追記処理)
    chkSize  = readSize - (ptr - buffer);
    if (((chkSize % 2) == 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
    {
        //  最後の文字を記載しないのではなく、最後の文字の前に空白を挿入する
        // ことで、書き込みバイト数を偶数にする。(v0.80r003) これでいい？
        if (ptr[chkSize - 1] == '\x0a')
        {
            ptr[chkSize]     = ptr[chkSize - 1];
            ptr[chkSize - 1] = ' ';
            NNsh_DebugMessage(ALTID_WARN, "append spc(pre)", "", chkSize);
        }
        else
        {
            buffer[chkSize] = ' ';
            NNsh_DebugMessage(ALTID_WARN, "append spc(post)", "", chkSize);
        }
        chkSize++;
    }

    // 書き込みファイルオープン
    ret = OpenFile_NNsh(targetFile, NNSH_FILEMODE_APPEND, &fileRefW);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "ERR>OpenFile_NNsh", targetFile, ret);
        ret = errNone;
        goto END_FUNC;
    }

    ret = AppendFile_NNsh(&fileRefW, chkSize, ptr, &writeSize);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "WRITEFile_NNsh(1st)", "", ret);
        ret = errNone;
        goto END_FUNC;
    }
    offset = readSize;

    while (offset < fileSize)
    {
        MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize + MARGIN, 0x00);
        readSize = 0;
        ret = ReadFile_NNsh(&fileRefR, offset, (NNshGlobal->NNsiParam)->bufferSize,
                            buffer, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            NNsh_InformMessage(ALTID_ERROR, "ERR>ReadFile_NNsh", "", ret);
            break;
        }
        chkSize = 0;
        if (((readSize % 2) == 1)&&((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND))
        {
            if (ptr[readSize - 1] == '\x0a')
            {
                buffer[readSize]     = buffer[readSize - 1];
                buffer[readSize - 1] = ' ';
                NNsh_DebugMessage(ALTID_WARN, "Append space(pre)", "",
                                  readSize);
            }
            else
            {
                buffer[readSize] = ' ';
                NNsh_DebugMessage(ALTID_WARN, "Append space(post)", "",
                                  readSize);
            }
            chkSize++;
        }
        ret = AppendFile_NNsh(&fileRefW, readSize+chkSize, buffer, &writeSize);
        if (ret != errNone)
        {
            NNsh_InformMessage(ALTID_ERROR, "ERR>WriteFile_NNsh()", "", ret);
            break;
        }
        offset = offset + readSize;
    }
    mesDB->fileSize = mesDB->fileSize + fileSize;
    if (fileSize != 0)
    {
        // update成功
        mesDB->state = NNSH_SUBJSTATUS_UPDATE;

        // スレの記録先を登録
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
        {
            // スレ記録先はVFS
            mesDB->msgState = FILEMGR_STATE_OPENED_VFS;
        }
        else
        {    
            // スレ記録先はPalm本体
            mesDB->msgState = FILEMGR_STATE_OPENED_STREAM;
        }
    }
    CloseFile_NNsh(&fileRefW);
    ret = errNone;

END_FUNC:    
    CloseFile_NNsh(&fileRefR);
    MEMFREE_PTR(realBuf);
    Hide_BusyForm(false);

    return (ret);
}

/*=========================================================================*/
/*   Function :   Get_AllMessage                                           */
/*                                                        メッセージ全取得 */
/*=========================================================================*/
Err Get_AllMessage(Char *url, Char *boardNick, Char *file, UInt16 idxMES,
                   UInt16 bbsType)
{
    Err                  ret;
    UInt32               fileSize, range;
    Char                 fileName[MAXLENGTH_FILENAME];

    // メッセージを格納するファイル名を作成
    MemSet (fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, boardNick);
    StrCat (fileName, file);

    // 分割ダウンロード指定時にはレンジ設定を行う
    if ((NNshGlobal->NNsiParam)->enablePartGet != 0)
    {
        range = (UInt32) (NNshGlobal->NNsiParam)->partGetSize - NNSH_HTTPGET_HEADERMAX;
    }
    else
    {
        range = HTTP_RANGE_OMIT;
    }

    // メッセージを受信
    ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                        HTTP_GETSUBJECT_START, range, NULL);
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
        return (ret);
    }

    // 受信したデータを変換、登録する。
    ret = message_entryMessage (fileName, &fileSize, bbsType);
    if (ret == errNone)
    {
        // 受信エラーでなかったときには、取得情報をたデータベースに登録
        message_entryDatabase(idxMES, fileSize, bbsType);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   Get_PartMessage                                          */
/*                                                      メッセージ差分取得 */
/*=========================================================================*/
Err Get_PartMessage(Char *url, Char *boardNick, NNshSubjectDatabase *mesDB, 
                    UInt16 idxMES, Char *message)
{
    Err    ret, ret1;
    Char   fileName[MAXLENGTH_FILENAME];
    UInt32 range;

    // メッセージを格納するファイル名を作成
    MemSet (fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, boardNick);
    StrCat (fileName, mesDB->threadFileName);

    // 分割ダウンロード指定時にはレンジ設定を行う
    if ((NNshGlobal->NNsiParam)->enablePartGet != 0)
    {
        range = (UInt32) (NNshGlobal->NNsiParam)->partGetSize - 
                         NNSH_HTTPGET_HEADERMAX + (mesDB->fileSize - 1);
    }
    else
    {
        range = HTTP_RANGE_OMIT;
    }

    // httpで、レスを差分取得
    if (mesDB->bbsType != NNSH_BBSTYPE_SHITARABAJBBS_RAW)
    {
        // メッセージを送受信(したらば@JBBS以外のとき)
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                            (mesDB->fileSize - 1), range, message);
    }
    else
    {
        // したらば@JBBSのとき... rawmode なので、URLに取得レス番号を追加
        NUMCATI(url, (mesDB->maxLoc + 1));
        StrCat (url, "-");
        NNsh_DebugMessage(ALTID_INFO, " SHITARABA URL:", url, 0);

        // メッセージを送受信
        ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                            HTTP_RANGE_OMIT, range, message);
    }

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
        return (ret);
    }

    // 受信したデータを変換、登録する。
    ret1 = message_appendMessage(fileName, mesDB);
    switch (ret1)
    {
      case errNone:
        // データ変換成功、次へすすむ。
        // ただし、したらば＠JBBS(rawmode)のときは、末尾レス番号を取得する
        if (mesDB->bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)
        {
            // 末尾のレス番号を取得する
            (void) getLastMessageNumShitaraba(&(mesDB->maxLoc));
        }
        break;

      case NNSH_MSGSTATE_NOTMODIFIED:
        // データ未更新
        NNsh_InformMessage(ALTID_INFO, 
                           MSG_RECV_NOTMODIFY, mesDB->threadTitle, 0);
        goto FUNC_END;
        break;

      case NNSH_MSGSTATE_ERROR:
        // データ受信失敗
        NNsh_InformMessage(ALTID_ERROR, 
                          MSG_RECV_PARTMSG_ERROR, mesDB->threadTitle, ret);
        // mesDB->state        = NNSH_SUBJSTATUS_UNKNOWN;

        // スレにエラー情報を設定する(＆取得予約スレ情報をクリアする)
        mesDB->msgAttribute = (mesDB->msgAttribute | NNSH_MSGATTR_ERROR);
        mesDB->msgAttribute = (mesDB->msgAttribute & NNSH_MSGATTR_NOTRESERVE);
        (void) update_subject_database(idxMES, mesDB);
        return (NNSH_ERRORCODE_PARTGET);
        break;

      default:
        break;
    }
FUNC_END:
    // スレのエラー情報をクリアする
    mesDB->msgAttribute = ((mesDB->msgAttribute)&
                           (NNSH_MSGATTR_NOTERROR)&(NNSH_MSGATTR_NOTRESERVE));

    // スレメッセージデータベースの内容を更新する。
    ret = update_subject_database(idxMES, mesDB);
    if (ret != errNone)
    {
        // ありえないはず...
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()"," ", ret);
    }
    return (ret1);
}

/*=========================================================================*/
/*   Function : MoveSubject_BBS                                            */
/*                                              スレの所属する板を変更する */
/*=========================================================================*/
Err MoveSubject_BBS(Char *dest, Char *src)
{
    Err                 ret;
    UInt16              index, cnt;
    DmOpenRef           dbRef;
    NNshSubjectDatabase subjDB;

    // スレ名管理DBオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return (~errNone);
    }
    GetDBCount_NNsh(dbRef, &cnt);
    for (index = 0; index < cnt; cnt++)
    {
        // スレデータを取得
        ret = GetRecord_NNsh(dbRef, index,
                             sizeof(NNshSubjectDatabase), &subjDB);
        if (ret != errNone)
        {
            // エラー発生(デバッグ表示)
            NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", " ", ret);
            continue;
        }

        // 指定したボードニックネームかチェックする
        if (StrCompare(subjDB.boardNick, src) == 0)
        {
            // スレを移動させる
            MemSet (subjDB.boardNick, MAX_NICKNAME, 0x00);
            StrCopy(subjDB.boardNick, dest);
            UpdateRecord_NNsh(dbRef, index,
                              sizeof(NNshSubjectDatabase), &subjDB);
        }
    }
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));


    return (errNone);
}

/*=========================================================================*/
/*   Function : MarkMessageIndex                                           */
/*                                           メッセージ読み出し状況を設定  */
/*=========================================================================*/
void MarkMessageIndex(UInt16 index, UInt16 favor, UInt16 ngWordCheck)
{
    Err                  ret;
    NNshSubjectDatabase  subjDB;

    ret = Get_Subject_Database(index, &subjDB);
    if (ret == errNone)
    {
        if ((NNshGlobal->msgIndex) != NULL)
        {
            subjDB.currentLoc = (NNshGlobal->msgIndex)->openMsg;
            subjDB.maxLoc     = (NNshGlobal->msgIndex)->nofMsg;    
            if ((NNshGlobal->msgIndex)->nofMsg == 
                                             (NNshGlobal->msgIndex)->openMsg)
            {
                subjDB.state = NNSH_SUBJSTATUS_ALREADY;
            }
            else
            {
                subjDB.state = NNSH_SUBJSTATUS_REMAIN;
            }

            // MSGの場所が不明だった場合には、記録先を記入する。
            if ((subjDB.msgState == 0)&&
                (subjDB.maxLoc != 0)&&(subjDB.currentLoc != 0))
            {
                if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
                {
                    // スレ記録先はVFS
                    subjDB.msgState = FILEMGR_STATE_OPENED_VFS;
                }
                else
                {    
                    // スレ記録先はPalm本体
                    subjDB.msgState = FILEMGR_STATE_OPENED_STREAM;
                }
            }
        }
        switch (favor)
        {
          case NNSH_MSGATTR_NOTSPECIFY:
            // お気に入り設定は変更しない
            break;

          case 0:
            // お気に入り設定をクリアする
            subjDB.msgAttribute 
                               = (subjDB.msgAttribute & NNSH_MSGATTR_NOTFAVOR);
            break;

          default:
            // 指定されたお気に入り設定を実行する
            subjDB.msgAttribute 
                  = ((subjDB.msgAttribute & NNSH_MSGATTR_NOTFAVOR)| favor);
            break;
        }

        // スレ毎にNG設定を使う/使わないを指定する場合...
        if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD_EACHTHREAD) == NNSH_USE_NGWORD_EACHTHREAD)
        {
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
        }
        (void) update_subject_database(index, &subjDB);
    }
    return;
}

/*=========================================================================*/
/*   Function :   GetThreadDataFromMesNum                                  */
/*                                            スレ番号指定のメッセージ取得 */
/*=========================================================================*/
Boolean GetThreadDataFromMesNum(Boolean *updateBBS, Char *buf, UInt16 bufLen, UInt16 *recNum)
{
    Boolean              rep = false;
    Err                  ret;
    UInt16               btnId, index, dataIndex;
    UInt32               size;
    DmOpenRef            dbRef;
    Char                *ptr, *nPtr;
    NNshBoardDatabase    bbsData;
    NNshSubjectDatabase  subjDb;

    // 領域の初期化
    index = 0;
    MemSet(&subjDb,  sizeof(NNshSubjectDatabase), 0x00);
    MemSet(&bbsData, sizeof(NNshBoardDatabase),   0x00);
    *updateBBS = false;
    *recNum = 0;

    // 入力データから板Nickを切り出す
    ptr  = buf;
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
            return (false);
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
        NNsh_InformMessage(ALTID_ERROR, subjDb.boardNick,
                          MSG_WRONG_BOARDNICK, ret);
        Hide_BusyForm(false);
        return (false);
    }

    // メッセージの記入
    MemSet (buf, bufLen, 0x00);
    StrCopy(buf, bbsData.boardName);
    StrCat (buf, MSG_CONFIRM_GETTHREAD);
    StrCat (buf, subjDb.threadFileName);
    StrCat (buf, ")");

    // メッセージの削除
    Hide_BusyForm(false);

    // メッセージ取得先(URL)の作成
    switch (bbsData.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // まちBBSからデータ取得(HTMLを読む)
        StrCat (subjDb.threadFileName, ".cgi");

        //////////////////////////////////////////////////////////////////////
        // 現在、まちＢＢＳにはこの機能は使用できない旨を表示して終了する。
        NNsh_InformMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI,"",0);
        return (false);
        //////////////////////////////////////////////////////////////////////
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // したらば＠JBBSからデータ取得(HTMLを読む)
        StrCat (subjDb.threadFileName, ".cgi");   // 拡張子をつける

        //////////////////////////////////////////////////////////////////////
        // 現在、この機能は使用できない旨を表示して終了する。
        NNsh_InformMessage(ALTID_WARN, MSG_NOTSUPPORT_BBS,"",0);
        return (false);
        //////////////////////////////////////////////////////////////////////
        break;

#if 0
      case NNSH_BBSTYPE_SHITARABA:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // 2chからデータ取得(datを読む)
        StrCat (subjDb.threadFileName, ".dat");   // 拡張子をつける
        break;
    }

    // 本当にスレを取得するか確認を行う
    btnId = NNsh_ConfirmMessage(ALTID_CONFIRM, buf, "", 0);
    if (btnId != 0)
    {
        // 取得をキャンセルする
        NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
        return (false);
    }

    // データ取得先URLを生成する
    CreateThreadURL(NNSH_DISABLE, buf, bufLen, &bbsData, &subjDb);

    // スレデータが取得済みか確認する。
    if (Check_same_thread(NNSH_NOTENTRY_THREAD, &subjDb, &subjDb, &dataIndex)
                                                                    == errNone)
    {
        // 取得済みと判明、差分取得に切り替える
        ret = Get_PartMessage(buf,bbsData.boardNick,&subjDb,dataIndex,NULL);

        // 取得終了の表示(DEBUGオプション有効時のみ)
        NNsh_InformMessage(ALTID_INFO, MSG_GET_PARTMESSAGE_END,
                           subjDb.threadTitle, ret);

        // 取得したスレのレコード番号を格納する
        *recNum = dataIndex;
        rep = (ret == errNone) ? true : false;
        return (rep);
    }

    ////////////  メッセージ全部取得の処理  ///////////

    // スレ情報取得中のメッセージを表示する
    Show_BusyForm(MSG_MESSAGE_INFO_WAIT);

    /////////////////////////////////////////////////////////////////////////
    // データベースにスレデータを(仮)追加する ※スレタイトルは後で追加する※
    /////////////////////////////////////////////////////////////////////////
    StrCopy(subjDb.threadTitle, "(Unknown)");

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // メッセージの削除
        Hide_BusyForm(false);

        // エラー発生
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh()", "", 0);
        return (false);
    }
    EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &subjDb);
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    /** (VFS上で)ディレクトリを作成する **/
    (void) CreateDir_NNsh(bbsData.boardNick);
    
    // メッセージの削除
    Hide_BusyForm(false);

    // ログ取得および記録処理
#ifdef USE_ZLIB
    NNshGlobal->getRetry = 0;
GET_ALLMESSAGE:
    ret = Get_AllMessage(buf, bbsData.boardNick, subjDb.threadFileName,
                         0, (UInt16) bbsData.bbsType);

    if ((NNshGlobal->useGzip != 0)&&(NNshGlobal->loginDateTime != 0)&&
         (ret == NNSH_ERRORCODE_NOTACCEPTEDMSG)&&
         (NNshGlobal->getRetry == 0))
    {
        //  ●ログイン中で、サーバからの応答メッセージが
        // 異常だったときは、過去ログ倉庫をあさる
        MemSet(buf, bufLen, 0x00);

        // メッセージ取得先URLの作成
        (void) CreateThreadURL(NNSH_ENABLE, buf, bufLen, &bbsData, &subjDb);
        NNshGlobal->getRetry = 1;
        goto GET_ALLMESSAGE;
    }
    else if (ret != errNone)
#else // #ifdef USE_ZLIB
    ret = Get_AllMessage(buf, bbsData.boardNick, subjDb.threadFileName,
                         0, (UInt16) bbsData.bbsType);
    if (ret != errNone)
#endif // #ifdef USE_ZLIB
    {
        // スレファイル名を 変数bufに書き出し、(長さ０の)スレファイルを削除
        MemSet (buf, bufLen, 0x00);
        StrCopy(buf, bbsData.boardNick);
        StrCat (buf, subjDb.threadFileName);
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
        {
            // Palm本体上のログを削除する
            ret = DeleteFile_NNsh(buf, NNSH_VFS_DISABLE);
        }
        else
        {
            // VFS上のログを削除する
            ret = DeleteFile_NNsh(buf, NNSH_VFS_ENABLE);
        }

        // エラー発生(仮追加したデータベースデータを削除する)
        (void) OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
        DeleteRecordIDX_NNsh(dbRef, 0);
        CloseDatabase_NNsh(dbRef);
        NNsh_DebugMessage(ALTID_ERROR, "Get_AllMessage()", "", ret);
        return (false);
    }
#ifdef USE_ZLIB
    NNshGlobal->getRetry = 0;
#endif // #ifdef USE_ZLIB

    //////////////////スレタイトル取得処理/////////////////////////

    // スレ情報取得中のメッセージを表示する
    Show_BusyForm(MSG_MESSAGE_INFO_WAIT);

    // スレ情報を取得する
    (void) Get_Subject_Database(0, &subjDb);

    // ファイルサイズを一時的に記憶
    size = subjDb.fileSize;

    // スレファイル名を 変数bufに書き出す
    MemSet (buf, bufLen, 0x00);
    StrCopy(buf, bbsData.boardNick);
    StrCat (buf, subjDb.threadFileName);

    // BBS情報に設定された標準スレレベルを反映させる
    switch ((bbsData.state)&(NNSH_BBSSTATE_LEVELMASK))
    {
      case NNSH_BBSSTATE_FAVOR_L1:
        // Lレベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L1));
        break;

      case NNSH_BBSSTATE_FAVOR_L2:
        // 1レベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L2));
        break;

      case NNSH_BBSSTATE_FAVOR_L3:
        // 2レベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L3));
        break;

      case NNSH_BBSSTATE_FAVOR_L4:
        // 3レベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L4));
        break;

      case NNSH_BBSSTATE_FAVOR:
        // Hレベルに設定
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR));
        break;

      default:
        // 何もしない
        break;
    }

    // ファイルからスレ情報を取得する
    ret = SetThreadInformation(buf, (NNSH_FILEMODE_READONLY), NULL, &subjDb);
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

    // メッセージの削除
    Hide_BusyForm(false);

    if (ret != errNone)
    {
        // エラー発生
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()", "", ret);
        return (false);
    }

    // 取得終了の表示
    NNsh_InformMessage(ALTID_INFO,MSG_GET_MESSAGE_END,subjDb.threadTitle,ret);

#ifdef UPDATE_BBS_AUTOMATICALLY
    // BBSを使用する設定になっていなかった場合...
    if ((bbsData.state & NNSH_BBSSTATE_VISIBLE) != NNSH_BBSSTATE_VISIBLE)
    {
        // BBSが使用する設定にして更新する
        bbsData.state = ((NNSH_BBSSTATE_LEVELMASK)&(bbsData.state))|(NNSH_BBSSTATE_VISIBLE);

        // BBS情報の更新
        Update_BBS_Database(index, &bbsData);

        *updateBBS = true;

        // 使用BBS一覧が追加されたことを通知
        NNsh_InformMessage(ALTID_WARN,bbsData.boardName,MSG_WARN_BBSUPDATE,0);

        // BBSタイトルインデックス用の領域を一旦解放する
        if (NNshGlobal->boardIdxH != 0)
        {
            MemHandleUnlock(NNshGlobal->boardIdxH);
            MemHandleFree(NNshGlobal->boardIdxH);
            NNshGlobal->boardIdxH = 0;
        }
        MEMFREE_PTR((NNshGlobal->bbsTitles));
        NNshGlobal->bbsTitles = 0; // 念のため...
    }
#endif  // #ifdef UPDATE_BBS_AUTOMATICALLY
    return (true);
}


/*=========================================================================*/
/*   Function :   Hander_MessageInfo                                       */
/*                                 メッセージ情報操作時のイベントハンドラ  */
/*=========================================================================*/
Boolean Handler_MessageInfo(EventType *event)
{
    FormType *frm;
    UInt16    itemId, status;

    // イベントを切り替える
    itemId = 0;
    switch (event->eType)
    { 
      case menuEvent:
        break;

      case keyDownEvent:
        // キー入力があった
        itemId = KeyConvertFiveWayToJogChara(event);
        break;

      case ctlSelectEvent:
        break;

      default: 
        break;
    }

    status = 1;
    frm = FrmGetActiveForm();
    switch (itemId)
    {
      case chrDigitOne:
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitTwo:
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitThree:
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitFour:
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitFive:
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        break;

      case chrDigitSix:
        SetControlValue(frm, PBTNID_FAVOR_L5, &status);
        status = 0;
        SetControlValue(frm, PBTNID_FAVOR_L0, &status);
        SetControlValue(frm, PBTNID_FAVOR_L1, &status);
        SetControlValue(frm, PBTNID_FAVOR_L2, &status);
        SetControlValue(frm, PBTNID_FAVOR_L3, &status);
        SetControlValue(frm, PBTNID_FAVOR_L4, &status);
        break;

      case chrCapital_W:
      case chrSmall_W:
        // ブラウザで開く
        CtlHitControl(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_OPEN_BROWSER)));
        break;

      case chrCapital_V:
      case chrSmall_V:
        // webDAで開く
        CtlHitControl(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_OPEN_WEBDA)));
        break;

      default:
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function : DisplayMessageInformations                                 */
/*                                                     スレ情報を表示する  */
/*=========================================================================*/
Err DisplayMessageInformations(UInt16 msgIndex, UInt16 *msgAttribute)
{
    Err                 ret;
    Char                logBuf[BIGBUF], *urlTop;
    UInt16              index, btnId, status;
    FormType           *prevFrm, *diagFrm;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // メッセージ情報をデータベースから取得
    ret = Get_Subject_Database(msgIndex, &mesInfo);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()", "", ret);
        return (ret);
    }

    // BBS情報をデータベースから取得
    ret = Get_BBS_Database(mesInfo.boardNick, &bbsInfo, &index);
    if (ret != errNone)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          mesInfo.boardNick, ret);
        return (ret);
    }
    // index番号をwork1に格納する
    NNshGlobal->work1 = index;

    /////////////////// スレメッセージ情報を構築する  ///////////////////////

    // 現在の画面ポインタを取得
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5以下なら、前のフォームを消去する。
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // スレ情報画面を開く
    diagFrm  = FrmInitForm(FRMID_MESSAGEINFO);
    if (diagFrm != NULL)
    {
        // フォームをアクティブにする
        FrmSetActiveForm(diagFrm);

        // PalmOS5.3DIA向けにSILK制御を宣言する
        if (NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)
        {
            FrmSetDIAPolicyAttr(diagFrm, frmDIAPolicyCustom);
            PINSetInputTriggerState(pinInputTriggerEnabled);

            // えらいベタ、、、ウィンドウサイズの最大値最小値を設定
            WinSetConstraintsSize(FrmGetWindowHandle(diagFrm),
                                  160, 225, 225, 160, 225, 225);
            // シルク制御を有効にする
            FrmSetDIAPolicyAttr(diagFrm, frmDIAPolicyCustom);

            // シルク操作を有効にする
            // PINSetInputTriggerState(pinInputTriggerEnabled);

            // シルク状態を更新する
            // PINSetInputAreaState(current);
            PINSetInputAreaState(pinInputAreaUser);
        }

        // イベントハンドラの設定
        FrmSetEventHandler(diagFrm, Handler_MessageInfo);

        // メッセージ情報の生成
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCat (logBuf, NNSH_MSGINFO_MSGNUM);
        NUMCATI(logBuf, mesInfo.currentLoc);
        StrCat (logBuf, "/");
        NUMCATI(logBuf, mesInfo.maxLoc);

        // メッセージ情報の表示
        NNshWinSetFieldText(diagFrm,FLDID_MSGSTATUS,false,logBuf,StrLen(logBuf));

        // ファイル情報の作成
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, NNSH_MSGINFO_FILELOC);

        // メッセージの記録箇所を出力する
        switch (mesInfo.msgState)
        {
          case FILEMGR_STATE_OPENED_VFS:
            StrCat(logBuf, "VFS (");
            break;
          case FILEMGR_STATE_OPENED_STREAM:
            StrCat(logBuf, "Palm (");
            break;
          default:
            StrCat(logBuf, "??? (");
            break;
        }
        NUMCATI(logBuf, mesInfo.fileSize);
        StrCat (logBuf, " bytes)");

        // ファイル情報の表示
        NNshWinSetFieldText(diagFrm,FLDID_FILESTATUS,false,logBuf,StrLen(logBuf));

        // その他情報の作成
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, NNSH_MSGINFO_STATUS);

        // 取得停止中ＢＢＳかどうかの表示
        if (((bbsInfo.state) & (NNSH_BBSSTATE_SUSPEND)) == NNSH_BBSSTATE_SUSPEND)
        {
            StrCat(logBuf, NNSH_MSGINFO_SUSPEND);
        }

        // NG設定-3の設定可否
        if ((mesInfo.msgAttribute & NNSH_MSGATTR_NGWORDSET) == NNSH_MSGATTR_NGWORDSET)
        {
            StrCat(logBuf, NNSH_MSGINFO_NG_3); 
        }

        // スレ状態の表示
        switch(mesInfo.state)
        {
          case NNSH_SUBJSTATUS_NOT_YET:
            // 未取得
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_NOT_YET);
            break;
          case NNSH_SUBJSTATUS_NEW:
            // 新規取得
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_NEW);
            break;
          case NNSH_SUBJSTATUS_UPDATE:
            // 更新取得
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_UPDATE);
            break;
          case NNSH_SUBJSTATUS_REMAIN:
            // 未読あり
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_REMAIN);
            break;
          case NNSH_SUBJSTATUS_ALREADY:
            // 既読
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_ALREADY);
            break;
          case NNSH_SUBJSTATUS_UNKNOWN:
          default:
            // 状態不明
            StrCat(logBuf, NNSH_SUBJSTATUSMSG_UNKNOWN);
            break;
        }
        if ((mesInfo.msgAttribute & NNSH_MSGATTR_ERROR) != 0)
        {
            // 受信エラー発生を表示
            StrCat(logBuf, NNSH_MSGINFO_RECVERR);
        }
        if ((mesInfo.msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
        {
            // 取得予約を表示
            StrCat(logBuf, NNSH_MSGINFO_RECVRESERVE);
        }

        // その他情報(メッセージ取得状態)の表示
        NNshWinSetFieldText(diagFrm, FLDID_ETCSTATUS,
                            false, logBuf, StrLen(logBuf));

        // スレタイトルの設定
        // (このタイトル欄に記入したURLを再利用させるため、表示順番を変更する)
        MemSet (logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, mesInfo.threadTitle);
        StrCat (logBuf, "\n");

        // スレURLの先頭ポインタを記憶する
        urlTop = &logBuf[StrLen(logBuf)];

        // スレのURL生成
        CreateThreadBrowserURL(NNSH_DISABLE, urlTop, &bbsInfo, &mesInfo);

        // スレURLの設定
        NNshWinSetFieldText(diagFrm,FLDID_MSGURL,true,logBuf,StrLen(logBuf));

        // 現在のスレお気に入りレベルを設定
        status = 1;
        switch(mesInfo.msgAttribute & NNSH_MSGATTR_FAVOR)
        {
          case NNSH_MSGATTR_FAVOR_L1:
            // お気に入りレベル(低)
            SetControlValue(diagFrm, PBTNID_FAVOR_L1, &status);
            break;

          case NNSH_MSGATTR_FAVOR_L2:
            // お気に入りレベル(中低)
            SetControlValue(diagFrm, PBTNID_FAVOR_L2, &status);
            break;

          case NNSH_MSGATTR_FAVOR_L3:
            // お気に入りレベル(中)
            SetControlValue(diagFrm, PBTNID_FAVOR_L3, &status);
            break;

          case NNSH_MSGATTR_FAVOR_L4:
            // お気に入りレベル(中高)
            SetControlValue(diagFrm, PBTNID_FAVOR_L4, &status);
            break;

          case NNSH_MSGATTR_FAVOR:
            // お気に入りレベル(高)
            SetControlValue(diagFrm, PBTNID_FAVOR_L5, &status);
            break;

          default:
            // お気に入りではない
            SetControlValue(diagFrm, PBTNID_FAVOR_L0, &status);
            break;
        }

        // ダイアログをオープンする
        btnId = FrmDoDialog(diagFrm);

        // スレレベルの設定結果確認
        status = 0;
        UpdateParameter(diagFrm, PBTNID_FAVOR_L0, &status);
        if (status != 0)
        {
            // お気に入り設定されていない
            *msgAttribute = 0;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L1, &status);
        if (status != 0)
        {
            // お気に入り設定(低)
            *msgAttribute = NNSH_MSGATTR_FAVOR_L1;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L2, &status);
        if (status != 0)
        {
            // お気に入り設定(中低)
            *msgAttribute = NNSH_MSGATTR_FAVOR_L2;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L3, &status);
        if (status != 0)
        {
            // お気に入り設定(中)
            *msgAttribute = NNSH_MSGATTR_FAVOR_L3;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L4, &status);
        if (status != 0)
        {
            // お気に入り設定(中高)
            *msgAttribute = NNSH_MSGATTR_FAVOR_L4;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L5, &status);
        if (status != 0)
        {
            // お気に入り設定(高)
            *msgAttribute = NNSH_MSGATTR_FAVOR;
            goto DELETE_DIALOG;
        }

DELETE_DIALOG:
        // ダイアログを閉じる
        FrmSetActiveForm(prevFrm);
        FrmDeleteForm(diagFrm);

        if ((btnId == BTNID_OPEN_BROWSER)||(btnId == BTNID_OPEN_WEBDA))
        {
            // 「ブラウザで開く」ボタンが押されたとき

            // お気に入り設定について(データベースを)更新する
            MarkMessageIndex(msgIndex, *msgAttribute, (NNshGlobal->NNsiParam)->hideMessage);

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
                  case NNSH_BBSTYPE_2ch:
                  case NNSH_BBSTYPE_2ch_EUC:
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
            // webDAで開く指示だった場合...
            if (btnId == BTNID_OPEN_WEBDA)
            {
                if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
                {
                    // クリップボードにURLをコピーしwebDAを起動する
                    ClipboardAddItem(clipboardText, urlTop, StrLen(urlTop));
                    (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
                }
                return (errNone);
            }

            // Webブラウザがインストールされているか確認
            if (NNshGlobal->browserCreator == 0)
            {
                // installされていないときは"サポートしていません" 表示を行う
                NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
                return (errNone);
            }

            // Webブラウザで開くか確認する
            if (NNsh_ConfirmMessage(ALTID_CONFIRM, urlTop,
                                    MSG_WEBLAUNCH_CONFIRM, 0) == 0)
            {
                // Web Browserを起動する(NNsi終了後に起動)
                (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                         0, 0, 
                                         NNshGlobal->browserLaunchCode,
                                         urlTop, StrLen(urlTop), NULL);
            }
            return (errNone);
        }        
    }

    // お気に入り設定について(データベースを)更新する
    NNshGlobal->updateHR = NNSH_UPDATE_DISPLAY;
    MarkMessageIndex(msgIndex, *msgAttribute, (NNshGlobal->NNsiParam)->hideMessage);

    return (errNone);
}

/*=========================================================================*/
/*   Function :   Get_MessageFromMesNum                                    */
/*                                            スレ番号指定のメッセージ取得 */
/*=========================================================================*/
Boolean Get_MessageFromMesNum(Boolean *updateBBS, UInt16 *recNum, Boolean useClipboard)
{
    DmOpenRef dbRef;
    Boolean   ret;
    Char     *url, *clipP;
    MemHandle memH;
    UInt16    len;
    
    // ワークバッファの確保と初期化
    url     = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (url == NULL)
    {
        return (false);
    }
    MemSet(url, (BIGBUF + MARGIN), 0x00);

    // クリップボードのデータを使用する場合
    if (useClipboard == true)
    {
        // クリップボードのデータを取得する
        memH = ClipboardGetItem(clipboardText, &len);
        if ((len != 0)&&(memH != 0))
        {
            len = (len > (BUFSIZE - 1)) ? (BUFSIZE - 1) : len;
            clipP = MemHandleLock(memH);
            if (clipP != NULL)
            {
                // クリップボードのデータをデフォルトデータとしてコピー
                StrNCopy(url, clipP, len);
            }
            MemHandleUnlock(memH);                
        }
    }

    // 入力文字列の取得
    ret = DataInputDialog(NNSH_INPUTWIN_GETMESNUM, url, BUFSIZE,
                          NNSH_DIALOG_USE_GETMESNUM, NULL);
    if (ret != false)
    {
        //  取得予約機能が有効でかつネットに接続されていない場合、
        // 取得予約するか確認する
        if (((NNshGlobal->NNsiParam)->getReserveFeature != 0)&&(NNshGlobal->netRef == 0))
        {
            if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                    NNSH_MSGINFO_RESERVE, "", 0) == 0)
            {
                // 取得予約が指示された
                OpenDatabase_NNsh(DBNAME_GETRESERVE,
                                  DBVERSION_GETRESERVE, &dbRef);
                if (dbRef == 0)
                {
                    // DBオープンエラー
                    NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh():",
                                      DBNAME_GETRESERVE, 0);
                    return (false);
                }
                // ＤＢに登録し、終了する
                (void) EntryRecord_NNsh(dbRef, BUFSIZE, url);
                CloseDatabase_NNsh(dbRef);
                return (true);
            }
        }

        // スレ番号指定のメッセージ取得を実行する
        ret = GetThreadDataFromMesNum(updateBBS, url, BIGBUF, recNum);
    }

    // 確保した領域を解放
    MEMFREE_PTR(url);
    return (ret);
}

/*=========================================================================*/
/*   Function : CreateThreadURL                                            */
/*                                               データ取得先のURLを生成   */
/*=========================================================================*/
Err CreateThreadURL(UInt16 urlType, Char *url, UInt16 bufSize, 
                    NNshBoardDatabase *bbsInfo, NNshSubjectDatabase *mesInfo)
{
    Char   *ptr;
    UInt16  size;

    MemSet (url, bufSize, 0x00);
    StrCopy(url, bbsInfo->boardURL);

    switch (bbsInfo->bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // JBBS/まちBBSからデータ取得(HTMLを読む)
        StrCat (url, URL_PREFIX_MACHIBBS);
        StrCat (url, mesInfo->boardNick);
        // boardNickの末尾にある '/'を削除
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo->threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileNameの拡張子を削除する
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '\0';
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        // したらば@JBBS 新read.cgi対応
        size = 0;
        ptr = &url[StrLen(url) - 1];
        ptr--;
        size++;
        while ((ptr > url)&&(*ptr != '/'))
        {
            ptr--;
            size++;
        }

        // "/bbs/read.cgi" をカテゴリの前に挿入
        MemMove(ptr + (sizeof("/bbs/read.cgi") - 1), ptr, (size + 1));

        StrCopy(ptr, "/bbs/read.cgi");
        url[StrLen(url)] = '/';

        StrCat(url, mesInfo->boardNick);
        StrCat(url, mesInfo->threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileNameの拡張子を削除する
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '/';
        ptr++;
        *ptr = '\0';

        NNsh_DebugMessage(ALTID_INFO, "URL:", url, 0);
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // したらば@JBBS(rawmode.cgi)対応
        size = 0;
        ptr = &url[StrLen(url) - 1];
        ptr--;
        size++;
        while ((ptr > url)&&(*ptr != '/'))
        {
            ptr--;
            size++;
        }

        // "/bbs/rawmode.cgi" をカテゴリの前に挿入
        MemMove(ptr + (sizeof("/bbs/rawmode.cgi") - 1), ptr, (size + 1));

        StrCopy(ptr, "/bbs/rawmode.cgi");
        url[StrLen(url)] = '/';

        StrCat(url, mesInfo->boardNick);
        StrCat(url, mesInfo->threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileNameの拡張子を削除する
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '/';
        ptr++;
        *ptr = '\0';

        NNsh_DebugMessage(ALTID_INFO, "URL:", url, 0);
        break;

      case NNSH_BBSTYPE_SHITARABA:
        // したらばからデータ取得(datを読む)
        StrCat (url, "bbs/");
        StrCat (url, mesInfo->boardNick);
        StrCat (url, "dat/");
        StrCat (url, mesInfo->threadFileName);
        break;

#if 0
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // ２ちゃんねるからデータを取得(datを読む)
#ifdef USE_ZLIB
        // gzip通信使用中で、かつ●ログイン中かどうかをチェックする
        if ((urlType != NNSH_DISABLE)&&
            (NNshGlobal->useGzip != 0)&&
            (NNshGlobal->loginDateTime != 0))
        {
            // ●ログイン中なので●セッションIDをつけて取得 by Nickle
            char serverName[64];
            char threadNum[16];

            // サーバ名を取得する
            // ex. http://pc5.2ch.net/hard/ --> http://pc5.2ch.net/
            {
                Char* end;

                StrNCopy(serverName, bbsInfo->boardURL, 64);
                serverName[63] = '\0';

                end = StrChr(serverName + 7, '/'); // +7 : http://を飛ばす
                if (end == NULL)
                {
                    NNsh_DebugMessage(ALTID_ERROR, "Can't determine server name", "", 0);
                    break;
                }
                *(end + 1) = '\0';
            }

            // スレ番号取得(threadFileNameから.datを削除)
            {
                Char* end;

                StrNCopy(threadNum, mesInfo->threadFileName, 16);
                threadNum[15] = '\0';

                end = StrChr(threadNum, '.');
                if (end == NULL)
                {
                    NNsh_DebugMessage(ALTID_ERROR, "Can't determine thread number", "", 0);
                    break;
                }
                *end = '\0';
            }
            // ●セッションIDつきURLを生成
            // raw=.0& と、リクエストヘッダに ACCEPT-ENCODING: gzip をつけたらうまくいったっぽい。
            // やはりgzip対応が必要なようだ
            // URLエンコードしてないとgzipつけてもこけたので、URLエンコードも必須のようだ
            // StrPrintF(url, "%stest/offlaw.cgi/%s/%s/?raw=.0&sid=", serverName, mesInfo->boardNick, threadNum);
            StrPrintF(url, "%stest/offlaw.cgi/%s%s/?raw=.0&sid=", serverName, mesInfo->boardNick, threadNum);
            StrCatURLencode(url, NNshGlobal->connectedSessionId);

            NNsh_DebugMessage(ALTID_INFO, "URL:", url, 0);
        }
        else
#endif
        {
            // ログインしていなかった、●なしで取得(Original)
            StrCat (url, "dat/");
            StrCat (url, mesInfo->threadFileName);
        }
        break;
    }
    return (errNone);
}


/*=========================================================================*/
/*   Function : CreateThreadBrowserURL                                     */
/*                                 データ取得先のURL(WebBrowser用)を生成   */
/*=========================================================================*/
void CreateThreadBrowserURL(UInt16 urlType, Char *url,
                            NNshBoardDatabase    *bbsInfo,
                            NNshSubjectDatabase  *mesInfo)
{
    Char *ptr;

    StrCat(url, bbsInfo->boardURL);
    switch (bbsInfo->bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // まちＢＢＳのＵＲＬを出力する
        if ((NNshGlobal->NNsiParam)->useImodeURL == 0)
        {
            StrCat (url, URL_PREFIX_MACHIBBS);
        }
        else
        {
            StrCat (url, URL_PREFIX_MACHIBBS_IMODE);
        }
        StrCat (url, mesInfo->boardNick);
        // boardNickの末尾にある '/'を削除
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo->threadFileName);
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // したらば＠ＪＢＢＳのＵＲＬを出力する
        if ((NNshGlobal->NNsiParam)->useImodeURL == 0)
        {
            StrCat (url, URL_PREFIX_SHITARABAJBBS);
        }
        else
        {
            StrCat (url, URL_PREFIX_SHITARABAJBBS_IMODE);
        }
        StrCat (url, mesInfo->boardNick);
        // boardNickの末尾にある '/'を削除
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo->threadFileName);
        NNsh_DebugMessage(ALTID_INFO, "URL:", url, 0);
        break;

      case NNSH_BBSTYPE_SHITARABA:
        // したらばのＵＲＬを出力する
        if ((NNshGlobal->NNsiParam)->useImodeURL == 0)
        {
            StrCat (url, URL_PREFIX_SHITARABA);
        }
        else
        {
            StrCat (url, URL_PREFIX_SHITARABA_IMODE);
        }
        StrCat (url, mesInfo->boardNick);
        // boardNickの末尾にある '/'を削除
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&key=");
        StrCat (url, mesInfo->threadFileName);
        break;

#if 0
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default: 
        // 2chのURLには、ボード名も含まれているので、その部分を削る。
        ptr = url + StrLen(url) - 2;
        while (*ptr != '/')
        {
            ptr--;
        }
        *ptr = '\0';
        StrCat (url, "/");
        if ((NNshGlobal->NNsiParam)->useImodeURL == 0)
        {
            StrCat (url, URL_PREFIX);
        }
        else
        {
            StrCopy(url, URL_PREFIX_MOBILE);
            //StrCat (url, URL_PREFIX_IMODE);
        }
        StrCat (url, bbsInfo->boardNick);
        StrCat (url, mesInfo->threadFileName);
        break;
    }

    // 末尾の".dat" もしくは ".cgi " は削除
    ptr = url + StrLen(url) - 1;
    while (*ptr != '.')
    {
        ptr--;
    }
    *ptr = '\0';

    return;
}

/*=========================================================================*/
/*   Function : MoveLogMessage                                             */
/*                                (ログメッセージをPalm内/VFSへ移動させる) */
/*=========================================================================*/
Err MoveLogMessage(UInt16 index)
{
    Err                 ret;
    UInt16              copyMode, delMode;
    Char                logFileName[MAX_NICKNAME + MAX_THREADFILENAME + MARGIN];
    NNshSubjectDatabase subjDb;

    // VFSパラメータがONのときのみ実施する
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        // VFSのモードがおかしいので実施しない
        return (NNSH_ERRORCODE_WRONGVFSMODE);
    }

    // 領域の初期化
    MemSet(&subjDb, sizeof(subjDb), 0x00);

    // データベースからデータを取得
    ret = Get_Subject_Database(index, &subjDb);
    if (ret != errNone)
    {
        // ありえないはずだが？
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()"," ", ret);
        return (ret);
    }

    switch (subjDb.msgState)
    {
      case FILEMGR_STATE_OPENED_STREAM:
        // Palm内からVFSへログデータ移動
        copyMode = NNSH_COPY_PALM_TO_VFS;
        delMode  = NNSH_VFS_DISABLE;
        subjDb.msgState = FILEMGR_STATE_OPENED_VFS;

        // (VFS上で)ディレクトリを作成する
        (void) CreateDir_NNsh(subjDb.boardNick);
        break;

      case FILEMGR_STATE_OPENED_VFS:
        // VFSからPalm内へログデータ移動
        copyMode = NNSH_COPY_VFS_TO_PALM;
        delMode  = NNSH_VFS_ENABLE;
        subjDb.msgState = FILEMGR_STATE_OPENED_STREAM;
        break;

      default:
        // ありえないはず、、、何もしないで終了する
        // ありえないはずだが？
        NNsh_DebugMessage(ALTID_ERROR, "wrong msgState :", subjDb.threadFileName, subjDb.msgState);
        return (~errNone);
        break;
    }

    // ログデータのファイル名を作成し、コピーを実施
    MemSet (logFileName, sizeof(logFileName), 0x00);
    StrCopy(logFileName, subjDb.boardNick);
    StrCat (logFileName, subjDb.threadFileName);
    ret = CopyFile_NNsh(logFileName, logFileName, copyMode);
    if (ret != errNone)
    {
        // ファイルのコピーに失敗
        // コピーしようとしたファイル(中途半端なファイル)を削除する
        delMode = (delMode == NNSH_VFS_DISABLE) ?
                                            NNSH_VFS_ENABLE : NNSH_VFS_DISABLE;
        DeleteFile_NNsh(logFileName, delMode);
        return (NNSH_ERRORCODE_COPYFAIL);
    }

    // コピー元ログファイルを削除する
    DeleteFile_NNsh(logFileName, delMode);

    // データを更新
    ret = update_subject_database(index, &subjDb);
    if (ret != errNone)
    {
        // ありえないはず...
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()"," ", ret);
        return (ret);
    }
    return (errNone);
}

#ifdef USE_SSL
/*=========================================================================*/
/*   Function : ProceedOysterLogin                                         */
/*                                                        ●のログイン処理 */
/*=========================================================================*/
Err ProceedOysterLogin(void)
{
    Err          err;
    Boolean      ret;
    UInt32       dummy;
    Int32        timeData, sample;
    DateTimeType dateBuf;
    NNshFileRef  fileRef;
    Char         *buffer, *buf, *ptr, *sId;

    buffer = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (buffer == NULL)
    {
        // ログインを実施しない
        return (errNone);
    }
    MemSet (buffer, (BIGBUF + MARGIN),  0x00);
    if (NNshGlobal->loginDateTime != 0)
    {
        // ログイン時刻を表示
        StrCopy(buffer, MSG_LOGINDATETIME);

        MemSet(&dateBuf, sizeof(dateBuf), 0x00);
        TimSecondsToDateTime(NNshGlobal->loginDateTime, &dateBuf);

        DateToAscii((UInt8)  dateBuf.month,
                    (UInt8)  dateBuf.day,
                    (UInt16) dateBuf.year, 
                    dfYMDLongWithDot,
                    &buffer[StrLen(buffer)]);

        TimeToAscii((UInt8) dateBuf.hour,
                    (UInt8) dateBuf.minute,
                     tfColon24h,
                    &buffer[StrLen(buffer)]);
    }
    else
    {
        // ログインしていないことを表示
        StrCopy(buffer, MSG_NOTLOGIN);
    }
    dummy    = TimGetSeconds() - NNshGlobal->loginDateTime;
    timeData = (Int32) dummy;
    sample   = ((TIME_24HOUR_SECOND) - (Int32) 10);
    if (timeData > sample)
    {
        // 再ログインが必要なことを表示する
        StrCat(buffer, MSG_LOGIN_INFO);
    }

    ret = DataInputPassword("2ch LOGIN", (NNshGlobal->NNsiParam)->oysterUserId,
                            sizeof((NNshGlobal->NNsiParam)->oysterUserId) - 1,
                            (NNshGlobal->NNsiParam)->oysterPassword,
                            sizeof((NNshGlobal->NNsiParam)->oysterPassword) - 1,
                            buffer);
    // Cancelボタンが押されたとき
    if (ret == false)
    {
        // ログインを実施しない
        MEMFREE_PTR(buffer);
        return (errNone);
    }

    // ログイン処理を実行するか確認
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_LOGIN_CONFIRM,
                            (NNshGlobal->NNsiParam)->oysterUserId, 0) != 0) 
    {
        // ログインを実施しない
        MEMFREE_PTR(buffer);
        return (errNone);
    }

    MemSet(buffer, (BIGBUF + MARGIN),  0x00);
    StrCopy(buffer, "ID=");
    StrCat(buffer, (NNshGlobal->NNsiParam)->oysterUserId);
    StrCat(buffer, "&PW=");
    StrCat(buffer, (NNshGlobal->NNsiParam)->oysterPassword);
    ptr = buffer;

    // 出力用ファイルの準備
    (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
    if (OpenFile_NNsh(FILE_SENDMSG, 
                      (NNSH_FILEMODE_APPEND|NNSH_FILEMODE_TEMPFILE),
                      &fileRef) != errNone)
    {
        // ファイルopen失敗
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    // データを書き出してファイルクローズ
    (void) AppendFile_NNsh(&fileRef, StrLen(ptr), ptr, &dummy);
    CloseFile_NNsh(&fileRef);

    // データの送信
#ifdef USE_OLD_2ch_LOGINSERVER
    err = NNshHttp_comm(HTTP_SENDTYPE_POST_2chLOGIN,
                        "https://tiger2.he.net/~tora3n2c/futen.cgi",
                        NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
#else
    err = NNshHttp_comm(HTTP_SENDTYPE_POST_2chLOGIN,
                        "https://2chv.tora3.net/futen.cgi",
                        NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
#endif
    if (err != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (err != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "[LOGIN]", err);
            MEMFREE_PTR(buffer);
            return (err);
        }

        // 通信タイムアウト発生
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[LOGIN]", err);
        MEMFREE_PTR(buffer);
        return (err);
    }

#if 0
    // 書き込んだあと、読み出し処理に移るのがはやすぎるのかな... 1秒のwaitを入れる...
    SysTaskDelay(1 * SysTicksPerSecond());
#endif

    // 受信データを解析する
    err = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "HTTP Reply Message ", "", err);
        MEMFREE_PTR(buffer);
        return (err);
    }
    buf = MEMALLOC_PTR(NNSH_WORKBUF_MIN);
    if (buf == NULL)
    {
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    (void) ReadFile_NNsh(&fileRef, 0, NNSH_WORKBUF_MIN, buf, &dummy);
    CloseFile_NNsh(&fileRef);

    // 応答コンテンツの先頭を探す
    ptr = StrStr(buf, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // データ異常
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(" NNSH_HTTP_HEADER_ENDTOKEN ")", 0);
        MEMFREE_PTR(buf);
        MEMFREE_PTR(buffer);
        return (~errNone - 10);
    }

    // 応答されたUAとSessionID（と認証OKとなった時刻）を記憶する
    sId = StrStr(ptr, "SESSION-ID=");
    if (sId == NULL)
    {
        // サーバがエラーだった、終了する
        // (ログインに失敗したことを通知する)
        NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_FAILURE, "(abnormal)", 0);
        goto FUNC_END;
    }
    // if (sId != NULL)
    {
        // データの末尾を探す
        sId = sId + StrLen("SESSION-ID=");

        // エラー判定...
        if ((*(sId + 0) == 'E')&&(*(sId + 1) == 'R')&&(*(sId + 2) == 'R')&&
            (*(sId + 3) == 'O')&&(*(sId + 4) == 'R'))
        {
            // サーバがエラーだった、終了する
            // (ログインに失敗したことを通知する)
            NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_FAILURE, "", 0);
            goto FUNC_END;
        }

        ptr = sId;
        while (*ptr > ' ')
        {
            ptr++;
        }
        *ptr = '\0';
        
        dummy = StrLen(sId);
        ptr = MEMALLOC_PTR(dummy + MARGIN);
        if (ptr != NULL)
        {
            // セッションＩＤをコピーする
            MemSet(ptr, (dummy + MARGIN), 0x00);
            StrCopy(ptr, sId);
            NNshGlobal->connectedSessionId = ptr;
        }
        ptr = sId;
        while ((*ptr != ':')&&(*ptr != '\0'))
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            *ptr = '\0';
            dummy = StrLen(sId);
            ptr = MEMALLOC_PTR(dummy + MARGIN);
            if (ptr != NULL)
            {
                // ユーザーエージェント名をコピーする
                MemSet(ptr, (dummy + MARGIN), 0x00);
                StrCopy(ptr, sId);
                NNshGlobal->customizedUserAgent = ptr;
            }
        }            
    }
    // ログイン時刻を記録しておく
    NNshGlobal->loginDateTime = TimGetSeconds();

    // ログインに成功した事を表示する
    NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_SUCCESS, (NNshGlobal->NNsiParam)->oysterUserId, 0);

FUNC_END:
    MEMFREE_PTR(buf);
    MEMFREE_PTR(buffer);
    return (errNone);
}
#endif

/*=========================================================================*/
/*   Function : ProceedBe2chLogin                                          */
/*                                                    Be@2chのログイン処理 */
/*=========================================================================*/
Err ProceedBe2chLogin(void)
{
    Err          err;
    Boolean      ret;
    UInt32       dummy;
    Int32        timeData, sample;
    DateTimeType dateBuf;
    NNshFileRef  fileRef;
    Char         *buffer, *buf, *ptr, *sId, *uId, *btm;

    buffer = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (buffer == NULL)
    {
        // ログインを実施しない
        return (errNone);
    }
    MemSet (buffer, (BIGBUF + MARGIN),  0x00);
    if (NNshGlobal->be2chLoginDateTime != 0)
    {
        // ログイン時刻を表示
        StrCopy(buffer, MSG_LOGINDATETIME);

        MemSet(&dateBuf, sizeof(dateBuf), 0x00);
        TimSecondsToDateTime(NNshGlobal->be2chLoginDateTime, &dateBuf);

        DateToAscii((UInt8)  dateBuf.month,
                    (UInt8)  dateBuf.day,
                    (UInt16) dateBuf.year, 
                    dfYMDLongWithDot,
                    &buffer[StrLen(buffer)]);

        TimeToAscii((UInt8) dateBuf.hour,
                    (UInt8) dateBuf.minute,
                     tfColon24h,
                    &buffer[StrLen(buffer)]);
    }
    else
    {
        // ログインしていないことを表示
        StrCopy(buffer, MSG_NOTLOGIN);
    }
    dummy    = TimGetSeconds() - NNshGlobal->loginDateTime;
    timeData = (Int32) dummy;
    sample   = ((TIME_24HOUR_SECOND) - (Int32) 10);

    // ユーザIDとパスワードの入力
    ret = DataInputPassword("Be@2ch LOGIN", (NNshGlobal->NNsiParam)->be2chId,
                            sizeof((NNshGlobal->NNsiParam)->be2chId) - 1,
                            (NNshGlobal->NNsiParam)->be2chPw,
                            sizeof((NNshGlobal->NNsiParam)->be2chPw) - 1,
                            buffer);
    // Cancelボタンが押されたとき
    if (ret == false)
    {
        // ログインを実施しない
        MEMFREE_PTR(buffer);
        return (errNone);
    }

    // ログイン処理を実行するか確認
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_LOGIN_CONFIRM,
                            (NNshGlobal->NNsiParam)->be2chId, 0) != 0) 
    {
        // ログインを実施しない
        MEMFREE_PTR(buffer);
        return (errNone);
    }

    // 出力用ファイルの準備
    (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
    if (OpenFile_NNsh(FILE_SENDMSG, 
                      (NNSH_FILEMODE_APPEND|NNSH_FILEMODE_TEMPFILE),
                      &fileRef) != errNone)
    {
        // ファイルopen失敗
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    // ユーザID＆パスワードデータを書き出してファイルクローズ
    MemSet(buffer, (BIGBUF + MARGIN),  0x00);
    StrCopy(buffer, "m=");
    (void) AppendFile_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    StrCopy(buffer, (NNshGlobal->NNsiParam)->be2chId);
    (void) AppendFileAsURLEncode_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    StrCopy(buffer, "&p=");
    (void) AppendFile_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    StrCopy(buffer, (NNshGlobal->NNsiParam)->be2chPw);
    (void) AppendFileAsURLEncode_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    StrCopy(buffer, "&submit=%C5%D0%CF%BF");
    (void) AppendFile_NNsh(&fileRef, StrLen(buffer), buffer, &dummy);

    CloseFile_NNsh(&fileRef);


    // LOGINフォームの受信
    err = NNshHttp_comm(HTTP_SENDTYPE_GET,
                        "http://be.2ch.net/test/login.php",
                        NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    if (err != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (err != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "[GET FORM]", err);
            MEMFREE_PTR(buffer);
            return (err);
        }

        // 通信タイムアウト発生
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[GET FORM]", err);
        MEMFREE_PTR(buffer);
        return (err);
    }

    // LOGINデータの送信
    err = NNshHttp_comm(HTTP_SENDTYPE_POST_Be2chLOGIN,
                        "http://be.2ch.net/test/login.php",
                        NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    if (err != errNone)
    {
        // (タイムアウト以外の)通信エラー発生
        if (err != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "[LOGIN]", err);
            MEMFREE_PTR(buffer);
            return (err);
        }

        // 通信タイムアウト発生
        NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[LOGIN]", err);
        MEMFREE_PTR(buffer);
        return (err);
    }

#if 0
    // 書き込んだあと、読み出し処理に移るのがはやすぎるのかな... 1秒のwaitを入れる...
    SysTaskDelay(1 * SysTicksPerSecond());
#endif

    // 受信データを解析する
    err = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "HTTP Reply Message ", "", err);
        MEMFREE_PTR(buffer);
        return (err);
    }
    buf = MEMALLOC_PTR(NNSH_WORKBUF_MIN);
    if (buf == NULL)
    {
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    (void) ReadFile_NNsh(&fileRef, 0, NNSH_WORKBUF_MIN, buf, &dummy);
    CloseFile_NNsh(&fileRef);

    // 応答コンテンツの先頭を探す
    ptr = StrStr(buf, NNSH_HTTP_HEADER_ENDTOKEN);
    if (ptr == NULL)
    {
        // データ異常
        NNsh_InformMessage(ALTID_ERROR, "ERR>Wrong data(not found separator,",
                           "(" NNSH_HTTP_HEADER_ENDTOKEN ")", 0);
        MEMFREE_PTR(buf);
        MEMFREE_PTR(buffer);
        return (~errNone - 10);
    }

    // 応答されたCookieデータを記憶する
    sId = StrStr(ptr, "document.cookie");
    if (sId == NULL)
    {
        // サーバがエラーだった、終了する
        // (ログインに失敗したことを通知する)
        NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_FAILURE, "(abnormal/cookie1)", 0);
        goto FUNC_END;
    }
    // if (sId != NULL)
    {
        // 実データの先頭まで移動する
        sId = sId + StrLen("document.cookie");
        while ((*sId != '\'')&&(*sId != '\0'))
        {
            sId++;
        }       
        if (*sId == '\0')
        {
            // 実データがない...   
            NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_FAILURE, "(abnormal/cookie2)", 0);
            goto FUNC_END;
        }
        sId++;

        // データの末尾を探す
        ptr = sId;
        while ((*ptr != ';')&&(*ptr != '\x0a'))
        {
            ptr++;
        }
        ptr++;
        *ptr = ' ';
        ptr++;
        *ptr = '\0';
        btm  = ptr;

        // 応答されたCookieデータを記憶する(part2)
        ptr++;
        uId = StrStr(ptr, "document.cookie");
        if (uId != NULL)
        {
            // 実データの先頭まで移動する
            uId = uId + StrLen("document.cookie");
            while ((*uId != '\'')&&(*uId != '\0'))
            {
                uId++;
            }
            // 実データがなかった...
            if (*uId == '\0')
            {
                goto ENTRY_COOKIE;   
            }

            // データの末尾を探す
            uId++;
            ptr = uId;
            while ((*ptr != ';')&&(*ptr != '\x0a')&&(*ptr != '\0'))
            {
                if (*ptr == '\'')
                {
                    ptr++;
                    while ((*ptr != '\'')&&(*ptr != '\x0a')&&(*ptr != '\0'))
                    {
                        ptr++;    
                    }
                    if ((*ptr == '\x0a')||(*ptr == '\0'))
                    {
                        break;
                    }
                    ptr++;
                }
                *btm = *ptr;
                btm++;
                ptr++;
            }
            *btm = '\0';
        }
ENTRY_COOKIE:    
        dummy = StrLen(sId);
        ptr = MEMALLOC_PTR(dummy + MARGIN);
        if (ptr != NULL)
        {
            // Cookieをコピーする
            MemSet(ptr, (dummy + MARGIN), 0x00);
            StrCopy(ptr, sId);

            // すでにCookieが入っているときには、一度領域をクリアする
            if (NNshGlobal->be2chCookie != NULL)
            {
                MEMFREE_PTR(NNshGlobal->be2chCookie);
                NNshGlobal->be2chCookie = NULL;
            }
            NNshGlobal->be2chCookie = ptr;
        }
    }
    // ログイン時刻を記録しておく
    NNshGlobal->be2chLoginDateTime = TimGetSeconds();

    // ログインに成功した事を表示する
    NNsh_InformMessage(ALTID_INFO, MSG_LOGIN_SUCCESS, (NNshGlobal->NNsiParam)->be2chId, 0);
    NNsh_DebugMessage (ALTID_INFO, "Cookie : ", NNshGlobal->be2chCookie, 0);

FUNC_END:
    MEMFREE_PTR(buf);
    MEMFREE_PTR(buffer);
    return (errNone);
}

/*=========================================================================*/
/*   Function : ShowReserveGetLogList                                      */
/*                             スレ番号指定予約しているURLの一覧を表示する */
/*=========================================================================*/
void ShowReserveGetLogList(void)
{
    Char    **dataList;
    UInt16    nofList, loop;
    DmOpenRef dbRef;

    // 初期化...
    nofList = 0;
    dataList = NULL;

    // スレ番指定取得で取得を保留したものを取得する
    OpenDatabase_NNsh(DBNAME_GETRESERVE, DBVERSION_GETRESERVE, &dbRef);
    if (dbRef == 0)
    {
        // DBオープンエラー
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh():", 
                          DBNAME_GETRESERVE, 0);
        return;
    }

    // 件数取得
    GetDBCount_NNsh(dbRef, &nofList);
    if (nofList != 0)
    {
        dataList = MEMALLOC_PTR(sizeof(Char *) * nofList + MARGIN);
        if (dataList == NULL)
        {
            // ＤＢオープンエラー発生、終了する
            CloseDatabase_NNsh(dbRef);
            NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR():", 
                              DBNAME_GETRESERVE, nofList);
            return;
        }
        MemSet(dataList, (sizeof(Char *) * nofList + MARGIN), 0x00);

        // 全件、スレ番指定取得を実施する
        for (loop = 0; loop < nofList; loop++)
        {
            dataList[loop] = MEMALLOC_PTR(BUFSIZE + MARGIN);
            if (dataList[loop] == NULL)
            {
                // 領域確保エラー、確保したところまで...
                nofList = loop;
                break;
            }
            MemSet(dataList[loop], (BUFSIZE + MARGIN), 0x00);

            // データ取得
            GetRecord_NNsh(dbRef, loop, BUFSIZE, dataList[loop]);
        }
    }
    else
    {
        // 予約なし、と表示する
        nofList = 1;
        dataList = MEMALLOC_PTR(sizeof(Char *) + MARGIN);
        dataList[0] = MEMALLOC_PTR(BUFSIZE + MARGIN);
        MemSet(dataList[0], (BUFSIZE + MARGIN), 0x00);
        StrCopy(dataList[0], NNSH_GETLOGMSG_NONE);
    }
    CloseDatabase_NNsh(dbRef);

    // データの表示
    DataViewForm(NNSH_GETLOGMSG_VIEWTITLE, nofList, dataList);

    // 確保した領域を開放する
    for (loop = 0; loop < nofList; loop++)
    {
        if (dataList[loop] != NULL)
        {
            MEMFREE_PTR(dataList[loop]);
        }
    }
    MEMFREE_PTR(dataList);

    return;
}

/*=========================================================================*/
/*   Function : UpdateThreadResponseNumber                                 */
/*                                              スレに含まれるレス数を更新 */
/*=========================================================================*/
UInt16  UpdateThreadResponseNumber(UInt16 dbIndex)
{
    Err                  ret;
    NNshSubjectDatabase *subjDB;
    NNshMessageIndex    *msgIndex;
    UInt16               bufferSize, fileMode, nofMsg;
    Char                *fileName, *area;

    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        // 領域確保失敗...
        return (0);
    }

    subjDB = MEMALLOC_PTR(sizeof(NNshSubjectDatabase) + MARGIN);
    if (subjDB == NULL)
    {
        // 領域確保失敗...
        MEMFREE_PTR(fileName);
        return (0);
    }

    // スレ情報をまず取得
    ret = Get_Subject_Database(dbIndex, subjDB);
    if (ret != errNone)
    {
        // subjectデータベースからデータ取得に失敗
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        MEMFREE_PTR(fileName);
        MEMFREE_PTR(subjDB);
        return (0);
    }

    bufferSize = sizeof(NNshMessageIndex) + (NNshGlobal->NNsiParam)->bufferSize + MARGIN * 2;
    msgIndex   = MEMALLOC_PTR(bufferSize);
    if (msgIndex == NULL)
    {
        MEMFREE_PTR(subjDB);
        MEMFREE_PTR(fileName);
        return (0);
    }
    MemSet(msgIndex, bufferSize, 0x00);

    // メッセージオフセット領域を確保する
    bufferSize = sizeof(UInt32) * ((NNshGlobal->NNsiParam)->msgNumLimit + MARGIN);
    (msgIndex)->msgOffset = MEMALLOC_PTR(bufferSize);
    if ((msgIndex)->msgOffset == NULL)
    {
        // メッセージオフセット領域の確保失敗
        MEMFREE_PTR(msgIndex);
        MEMFREE_PTR(subjDB);
        MEMFREE_PTR(fileName);
        NNsh_InformMessage(ALTID_ERROR, MSG_ERR_MEMPTR_NEW_BUFF, " size:",
                           bufferSize);
        return (0);
    }
    MemSet((msgIndex)->msgOffset, bufferSize, 0x00);

    msgIndex->bbsType = (UInt16) subjDB->bbsType;

    // 読み込むファイルの初期化
    MemSet (fileName, sizeof(fileName), 0x00);

    // OFFLINEスレのときは、boardNickを付加しないようにする
    if (StrCompare(subjDB->boardNick, OFFLINE_THREAD_NICK) != 0)
    {
        StrCopy(fileName, subjDB->boardNick);
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
            GetSubDirectoryName(subjDB->dirIndex, &fileName[StrLen(fileName)]);
        }
    }
    StrCat (fileName, subjDB->threadFileName);

    // ファイルのありかで、開くモードを切り替える
    if (subjDB->msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        fileMode = ((NNSH_FILEMODE_TEMPFILE)|(NNSH_FILEMODE_READONLY));
    }
    else
    {
        fileMode = (NNSH_FILEMODE_READONLY);
    }
    nofMsg = 0;
    ret = OpenFile_NNsh(fileName,fileMode,&(msgIndex->fileRef));
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Message File Open :", fileName, ret);
        goto FUNC_END;
    }
    GetFileSize_NNsh(&(msgIndex->fileRef), &(msgIndex->fileSize));

    //  メッセージスレッドのインデックスを作成する
    ret = CreateMessageThreadIndex(msgIndex, &(msgIndex->nofMsg), 
                                   msgIndex->msgOffset,
                                   (UInt16) subjDB->bbsType);
    if (ret != errNone)
    {
        // スレの番号取得に失敗、更新しない
        msgIndex->nofMsg = 0;
        NNsh_DebugMessage(ALTID_ERROR, "ERR>index creation failure", "", ret);
    }
    nofMsg = msgIndex->nofMsg;
    CloseFile_NNsh(&(msgIndex->fileRef));

FUNC_END:
    // 終了処理
    MEMFREE_PTR((msgIndex)->msgOffset);
    MEMFREE_PTR(msgIndex);
    MEMFREE_PTR(fileName);

    // 受信レス数に更新があったかどうか確認する
    if (nofMsg != 0)
    {
        if (subjDB->maxLoc != nofMsg)
        {
            // レス数の更新を検知、最大レス数とレス状態を変更する
            subjDB->maxLoc = nofMsg;
            subjDB->state  = NNSH_SUBJSTATUS_REMAIN;
        }
        else
        {
            // レスを全部読んでないとき、「新着」マークがついていた
            // 時に、既読マークに変更する
            if ((subjDB->maxLoc > subjDB->currentLoc)&&
                (subjDB->state == NNSH_SUBJSTATUS_NEW))
            {
                // Newマークを消去する
                subjDB->state = NNSH_SUBJSTATUS_REMAIN;
            }
        }

        // スレＤＢを更新する
        update_subject_database(dbIndex, subjDB); 
    }

    MEMFREE_PTR(subjDB);
    return (nofMsg);
}

/*=========================================================================*/
/*   Function :   DeleteThreadMessage                                      */
/*                                                           スレ削除処理  */
/*=========================================================================*/
Boolean DeleteThreadMessage(UInt16 msgIndex)
{
    Err                 ret;
    Char                fileName[MAXLENGTH_FILENAME], *area;
    DmOpenRef           dbRef;
    NNshSubjectDatabase mesInfo;

    // スレ情報をまず取得
    ret = Get_Subject_Database(msgIndex, &mesInfo);
    if (ret != errNone)
    {
        // subjectデータベースからデータ取得に失敗
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        return (false);
    }

    // スレ情報DBのオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh(SUBJECT)", "", 0);
        return (false);
    }

    // スレファイル(本体のログファイル)を削除
    MemSet (fileName, sizeof(fileName), 0x00);
    // OFFLINEスレのときは、boardNickを付加しないようにする
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) != 0)
    {
        // OFFLINEログのときは、boardNickを付加しない
        // (OFFLINEログのとき、このロジックは通らない)
        StrCopy(fileName, mesInfo.boardNick);
    }
    else
    {
        // OFFLINEスレがVFSに格納されている場合には、ディレクトリを付加する。
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
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

    // スレインデックスファイルを削除してクローズ
    (void) DeleteRecordIDX_NNsh(dbRef, msgIndex);
    CloseDatabase_NNsh(dbRef);

    // スレ一覧DBを更新したことにする
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    return (true);
}

/*=========================================================================*/
/*   Function :   DeleteThreadMessage_RecordID                             */
/*                                                           スレ削除処理  */
/*=========================================================================*/
Boolean DeleteThreadMessage_RecordID(UInt32 recId)
{
    Boolean   ret;
    DmOpenRef dbRef;
    UInt16    msgIndex = 0;

    // スレ情報DBのオープン
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生
        return (false);
    }

    ret = false;
    if (GetRecordIndexFromRecordID_NNsh(dbRef, recId, &msgIndex) == errNone)
    {
        CloseDatabase_NNsh(dbRef);
        ret = DeleteThreadMessage(msgIndex);
    }
    else
    {
        CloseDatabase_NNsh(dbRef);
    }
    return (ret);
}
