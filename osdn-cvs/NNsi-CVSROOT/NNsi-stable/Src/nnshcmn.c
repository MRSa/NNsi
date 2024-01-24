/*============================================================================*
 *  FILE: 
 *     nnshcmn.c
 *
 *  Description: 
 *     NNsi common functions.
 *
 *===========================================================================*/
#define NNSHCMN_C
#include "local.h"
#ifdef USE_ZLIB // by Nickle 
  #include "SysZLib.h"
  #include "armutls.h" // エンディアンスワップに使う
#endif

/*=========================================================================*/
/*   Function : ConvertHanZen                                              */
/*                                                  半角カナ→全角カナ変換 */
/*=========================================================================*/
Boolean ConvertHanZen(UInt8 *dst, UInt8 *ptr)
{
    // 半角カナ→全角カナ変換が必要な文字
    if ((*ptr >= 0xa1)&&(*ptr <= 0xdf))
    {
        // テーブル変換を行う
        StrCopy(dst, Han2ZenTable[((unsigned char)*ptr) - 0xa1]);
        return(true);
    }
    return (false);
}

/*=========================================================================*/
/*   Function : ConvertJIStoSJ                                             */
/*                            JIS漢字コードをSHIFT JIS漢字コードに変換する */
/*=========================================================================*/
Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr)
{
    UInt8 upper, lower;

    // 上位8ビット/下位8ビットを変数にコピーする
    upper = *ptr;
    lower = *(ptr + 1);

    // 奥村さんのC言語によるアルゴリズム辞典(p.110)のコードを使わせていただく
    if ((upper & 1) != 0)
    {
        if (lower < 0x60)
        {
            lower = lower + 0x1f;
        }
        else
        {
            lower = lower + 0x20;
        }
    }
    else
    {
        lower = lower + 0x7e;
    }   
    if (upper < 0x5f)
    {
        upper = (upper + 0xe1) >> 1;
    }
    else
    {
        upper = (((upper + 0x61) >> 1)|(0x80));
    }
    *dst = upper;
    dst++;
    *dst = lower;

    return (true);
}

/*=========================================================================*/
/*   Function : ConvertEUCtoSJ                                             */
/*                                  漢字コードをEUCからSHIFT JISに変換する */
/*=========================================================================*/
Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte)
{
    UInt8 temp[2];

    *copyByte = 0;
    if (*ptr == 0x8e)
    {
        // 半角カナコード
        ptr++;
        *dst = *ptr;
        *copyByte = 1;
        return (true); 
    }
    if ((*ptr >= ((0x80)|(0x21)))&&(*ptr <= ((0x80)|(0x7e))))
    {
        // EUC漢字コードと判定、JIS漢字コードに一度変換してからSHIFT JISに変換
        temp[0] = ((*ptr)&(0x7f));
        ptr++;
        temp[1] = ((*ptr)&(0x7f));
        (void) ConvertJIStoSJ(dst, temp);
        *copyByte = 2;
        return (true);
    }
    return (false);
}

/*=========================================================================*/
/*   Function : ConvertSJtoJIS                                             */
/*                                  漢字コードをSHIFT JISからJISに変換する */
/*                                          (これまた奥村さんのコードから) */
/*=========================================================================*/
Boolean ConvertSJtoJIS(UInt8 *dst, UInt8 *ptr)
{
    UInt8 upper, lower;

    // 上位8ビット/下位8ビットを変数にコピーする
    upper   = *ptr;
    lower   = *(ptr + 1);

    if (upper <= 0x9f)
    {
        if (lower < 0x9f)
        {
            upper = (upper << 1) - 0xe1;
        }
        else
        {
            upper = (upper << 1) - 0xe0;
        }
    }
    else
    {
        if (lower < 0x9f)
        {
            upper = ((upper - 0xb0) << 1) - 1;
        }
        else
        {
            upper = ((upper - 0xb0) << 1);
        }
    }
    if (lower < 0x7f)
    {
        lower = lower - 0x1f;
    }
    else
    {
        if (lower < 0x9f)
        {
            lower = lower - 0x20;
        }
        else
        {
            lower = lower - 0x7e;
        }
    }
    *dst = upper;
    dst++;
    *dst = lower;

    return (true);
}


/*=========================================================================*/
/*   Function : ConvertSJtoEUC                                             */
/*                                  漢字コードをSHIFT JISからEUCに変換する */
/*=========================================================================*/
Boolean ConvertSJtoEUC(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte, UInt16 *parseByte)
{
    // 半角アルファベットコードはそのまま表示する
    if (*ptr <= 0x80)
    {
        *dst = *ptr;
        *copyByte  = 1;
        *parseByte = 1;
        return (true);
    }

    // 半角カナコードはEUC漢字コードに変換する
    if ((*ptr >= 0xa1)&&(*ptr <= 0xdf))
    {
        *dst = 0x8e;
        dst++;
        *dst = *ptr;
        *copyByte  = 2;
        *parseByte = 1;
        return (true);
    }

    // JIS漢字コードへ変換
    ConvertSJtoJIS(dst, ptr);

    // EUC漢字コードに変換
    *dst       = (*dst)|(0x80);
    *(dst + 1) = (*(dst + 1))|(0x80);

    *copyByte  = 2;
    *parseByte = 2;

    return (true);
}

/*=========================================================================*/
/*   Function : SendToLocalMemopad                                         */
/*                    Exchangeマネージャを使用してメモ帳にデータを転送する */
/*                    (http://www.palmos.com/dev/support/docs/recipes/     */
/*                                    recipe_exg_mgr_send_local.html より) */
/*=========================================================================*/
Err SendToLocalMemopad(Char *title, Char *data)
{
    ExgSocketType exgSocket;
    UInt32        size, ret, creatorId;
    Err           err;
    Char          exgName[] = "?_local:NNsiMess.txt";

    // データの初期化
    err  = errNone;
    size = StrLen(data) + 1;

    // 構造体をクリアする
    MemSet(&exgSocket, sizeof(exgSocket), 0x00);

    // レス転送後、メモ帳を起動しないようにする
    exgSocket.noGoTo      = 1;

    // レス転送中表示を行わない
    exgSocket.noStatus    = true;

    // will comm. with memopad app
    exgSocket.description = title;
    exgSocket.name        = exgName;
    exgSocket.type        = ".TXT";

    creatorId = 0;
    ExgGetDefaultApplication(&creatorId,  exgRegExtensionID , "txt");
    if (creatorId == 'DTGP')
    {
        // Palm TXは、なぜか Docs To Go に渡してしまうので...
        // (無理やり memo (新版のメモ帳)へ渡すようにする。
        exgSocket.target = 'PMem';
    }

    // set to comm. with local machine (PIM) only
    exgSocket.localMode = 1;

    ret = exgErrUserCancel;
    err = ExgPut(&exgSocket);
    if (err == errNone)
    {
        ret = ExgSend(&exgSocket, data, size, &err);
        err = ExgDisconnect(&exgSocket, err);
    }
    if ((err == errNone)&&(ret != exgErrUserCancel))
    {
        // キャンセルされなかったときには、メモ帳に出力した旨表示する
        NNsh_InformMessage(ALTID_INFO, MSG_SEND_TO_MEMOPAD, " size:", size);
    }
    
    return (err);
}

/*=========================================================================*/
/*   Function : SendToExternalDevice                                       */
/*                            Exchangeマネージャを使用してデータを転送する */
/*                                (SendToLocalMemopad()とほとんどおんなじ) */
/*=========================================================================*/
Err SendToExternalDevice(Char *title, Char *data, UInt32 creator)
{
    ExgSocketType exgSocket;
    UInt32        size, ret;
    Err           err;

    // データの初期化
    err  = errNone;
    size = StrLen(data) + 1;

    // 構造体をクリアする
    MemSet(&exgSocket, sizeof(exgSocket), 0x00);

    // レス転送後、メモ帳を起動しないようにする
    exgSocket.noGoTo      = 1;

    // レス転送中表示を行う
    exgSocket.noStatus    = false;

    // will comm. with memopad app
    exgSocket.description = title;
    exgSocket.name        = "NNsiData." NNSI_EXCHANGEINFO_SUFFIX;
    exgSocket.target      = creator;

    // set to comm. with local machine (PIM) only
    // exgSocket.localMode = 1;

    ret = exgErrUserCancel;
    err = ExgPut(&exgSocket);
    if (err == errNone)
    {
        ret = ExgSend(&exgSocket, data, size, &err);
        err = ExgDisconnect(&exgSocket, err);
    }
    if ((err == errNone)&&(ret != exgErrUserCancel))
    {
        // キャンセルされなかったときには、メモ帳に出力した旨表示する
        NNsh_InformMessage(ALTID_INFO, MSG_SEND_TO_NNSIDATA, " size:", size);
    }
    return (err);
}

/*=========================================================================*/
/*   Function : StrCopyEUC                                                 */
/*                                     EUC漢字コードに変換して文字列コピー */
/*=========================================================================*/
void StrCopyEUC(Char *dst, Char *src)
{
    UInt16 copyByte, parseByte;

    while (*src != '\0')
    {
        (void) ConvertSJtoEUC(dst, src, &copyByte, &parseByte);
        dst = dst + copyByte;
        src = src + parseByte;
    }
    *dst = *src;
    return;
}


/*=========================================================================*/
/*   Function : StrCopySJ                                                  */
/*                          EUC漢字コードをSHIFT JISに変換して文字列コピー */
/*=========================================================================*/
void StrCopySJ(Char *dst, Char *src)
{
    UInt16 copyByte;

    while (*src != '\0')
    {
        if (ConvertEUCtoSJ(dst, src, &copyByte) == true)
        {
            dst = dst + copyByte;
            src = src + 2;
        }
        else
        {
            *dst = *src;
            dst++;
            src++;
        }
    }
    *dst = *src;
    return;
}

/*=========================================================================*/
/*   Function : StrCopyJIStoSJ                                             */
/*                          JIS漢字コードをSHIFT JISに変換して文字列コピー */
/*=========================================================================*/
void StrCopyJIStoSJ(Char *dst, Char *src)
{
    Boolean kanji;
    
    kanji = false; 
    while (*src != '\0')
    {
        if ((*src == '\x1b')&&(*(src + 1) == '\x24')&&(*(src + 2) == '\x42'))
        {
            // 漢字モードに切り替え
            kanji = true;
            src = src + 3;
            continue;
        }
        if ((*src == '\x1b')&&(*(src + 1) == '\x28')&&(*(src + 2) == '\x42'))
        {
            // ANKモードに切り替え
            kanji = false;
            src = src + 3;
            continue;
        }

        // データの１文字コピー
        if (kanji == true)
        {
            ConvertJIStoSJ(dst, src);
            dst = dst + 2;
            src = src + 2;
        }
        else
        {
            // 通常モード...
            *dst = *src;
            dst++;
            src++;
        }
    }
    *dst = *src;
    return;
}

/*=========================================================================*/
/*   Function : StrNCopyHanZen                                             */
/*              文字列のコピー(半角カナ→全角カナ変換および&quot;等の考慮) */
/*=========================================================================*/
void StrNCopyHanZen(Char *dest, Char *src, UInt16 length)
{
    Char *ptr, *dst;
 
    ptr = src;
    dst = dest;
    while ((dst < dest + length)&&(*ptr != '\0'))
    {
        if (*ptr == '&')
        {
            // "&gt;" を '>' に置換
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&
                (*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
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
                *dst++ = '&';
                ptr = ptr + 5;    // StrLen(TAG_AMP);
                continue;
            }
            // これはありえないはずだが...一応。
            *dst++ = '&';
            ptr++;
            continue;
        }

        // 半角カナ→全角カナ変換が有効なとき
        if ((NNshGlobal->NNsiParam)->convertHanZen != 0)
        {
            // 微妙に不具合を呼びそうな処理(バッファ不足チェック)...
            if (((UInt8) *ptr >= 0x81)&&((UInt8) *ptr <= 0x9f))
            {
                // 2バイト文字と判定
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }

            // 微妙に不具合を呼びそうな処理(バッファ不足チェック)...
            if (((UInt8) *ptr >= 0xe0)&&((UInt8) *ptr <= 0xef))
            {
                // 2バイト文字と判定
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }
            if (ConvertHanZen((UInt8 *) dst, (UInt8 *) ptr) == true)
            {
                // 半角カナ→全角変換
                dst = dst + 2;
                ptr++;
                continue;
            }
        }
        *dst = *ptr;
        ptr++;
        dst++;
    }
    *dst = '\0';
    return;
}

/*=========================================================================*/
/*   Function : StrCatURLencode                                            */
/*                                           文字列をURLエンコードして連結 */
/*=========================================================================*/
Char *StrCatURLencode(Char *dst, Char *src)
{
    Char   *ptr, logBuf[MINIBUF];
    UInt32  num;

    // コピーする場所を設定する
    ptr = &dst[StrLen(dst)];
    while (*src != '\0')
    {
        // スペースは＋に変換
        if (*src == ' ')
        {
            *ptr = '+';
            ptr++;
            src++;
            continue;
        }

        // 改行は、CR + LFに変換
        if (*src == '\n')
        {
            StrCopy(ptr, "%0D%0A");
            ptr = ptr + 6; // 6 == StrLen("%0D%0A");
            src++;
            continue;
        }

        // そのまま出力(その１)
        if ((*src == '.')||(*src == '_')||(*src == '-')||(*src == '*'))
        {
            *ptr = *src;
            ptr++;
            src++;
            continue;
        }

        // そのまま出力(その２)
        if (((*src >= '0')&&(*src <= '9'))||
            ((*src >= 'A')&&(*src <= 'Z'))||
            ((*src >= 'a')&&(*src <= 'z')))
        {
            *ptr = *src;
            ptr++;
            src++;
            continue;
        }

        // 上の条件のどれにも当てはまらない場合には、コンバートする。
        *ptr = '%';
        ptr++;
        
        num = (UInt32) *src;
        MemSet(logBuf, sizeof(logBuf), 0x00);
        StrIToH(logBuf, num);

        // 数値の下２桁が欲しいので...
        *ptr       = logBuf[6];
        *(ptr + 1) = logBuf[7];
        ptr = ptr + 2; // 2 == StrLen(data);
        src++;
    }
    return (dst);
}

/*=========================================================================*/
/*   Function : ShowVersion_NNsh                                           */
/*                                                    バージョン情報の表示 */
/*=========================================================================*/
void ShowVersion_NNsh(void)
{
    Char         buffer[BUFSIZE * 2];

    MemSet (buffer, sizeof(buffer), 0x00);
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
    StrCat (buffer, ")");

    FrmCustomAlert(ALTID_INFO, buffer, "", "");

    return;
}

/************************************************************************
 Sample Code Disclaimer Copyright ) 2001 Palm, Inc. or its subsidiaries. 
 All rights reserved.

 You may incorporate this sample code (the "Code") into your applications
 for Palm OSｮ platform products and may use the Code to develop
 such applications without restriction.  The Code is provided to you on
 an "AS IS" basis and the responsibility for its operation is 100% yours.
 PALM, INC. AND ITS SUBSIDIARIES (COLLECTIVELY, "PALM") DISCLAIM
 ALL WARRANTIES, TERMS AND CONDITIONS WITH RESPECT TO THE CODE, EXPRESS,
 IMPLIED, STATUTORY OR OTHERWISE, INCLUDING WARRANTIES, TERMS OR
 CONDITIONS OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 NONINFRINGEMENT AND SATISFACTORY QUALITY.  You are not permitted to
 redistribute the Code on a stand-alone basis and you may only
 redistribute the Code in object code form as incorporated into your
 applications.  TO THE FULL EXTENT ALLOWED BY LAW, PALM ALSO EXCLUDES ANY
 LIABILITY, WHETHER BASED IN CONTRACT OR TORT (INCLUDING NEGLIGENCE), FOR
 INCIDENTAL, CONSEQUENTIAL, INDIRECT, SPECIAL OR PUNITIVE DAMAGES OF ANY
 KIND, OR FOR LOSS OF REVENUE OR PROFITS, LOSS OF BUSINESS, LOSS OF
 INFORMATION OR DATA, OR OTHER FINANCIAL LOSS ARISING OUT OF OR IN
 CONNECTION WITH THE USE OR PERFORMANCE OF THE CODE.  The Code is subject
 to Restricted Rights for U.S. government users and export regulations.

 SAMPLE NAME: GetOSFreeMem function
 
 FILE:        GetOSFreeMem.c

 DESCRIPTION: Determine the amount of free memory and the amount of
              total memory in the current device.  Results are expressed
              in KB. The returned value is the amount of free memory in all
              heaps other than the dynamic heap.  This is very ACCURATE!
 
 REVISION HISTORY:   Name    Date         Description
                     ----    ----         -----------
                     mt      09/19/00     initial version  
                     mak     01/11/01     comments/dynamic memory conversion 
 ***********************************************************************/
// PalmSourceのKnowledge Base(Answer ID #291)を一部改変
//       (応答サイズをバイト数で応答するように変更する)
UInt32 GetOSFreeMem(UInt32 *totalMemoryP, UInt32 *dynamicMemoryP)
{
    Int16  i,         nCards;
    UInt16 cardNo,    heapID;
    UInt32 heapFree,  max;
    UInt32 freeMemory    = 0;
    UInt32 totalMemory   = 0;
    UInt32 dynamicMemory = 0;

    // Iterate through each card to support devices with multiple cards.
    nCards = MemNumCards();
    for (cardNo = 0; cardNo < nCards; cardNo++)
    {
        // Iterate through the RAM heaps on a card (excludes ROM).
        for (i = 0; i < MemNumRAMHeaps(cardNo); i++)
        {
            // Obtain the ID of the heap.
            heapID = MemHeapID(cardNo, i);

            if (MemHeapDynamic(heapID) != 0)
            {
                // If the heap is dynamic, increment the dynamic memory total.
                dynamicMemory = dynamicMemory + MemHeapSize(heapID);
            }
            else
            {
                // The heap is nondynamic.
                // Calculate the total memory and free memory of the heap.
                totalMemory = totalMemory + MemHeapSize(heapID);
                MemHeapFreeBytes(heapID, &heapFree, &max);
                freeMemory = freeMemory + heapFree;
            }
        }
    }

    if (totalMemoryP != NULL)
    {
        *totalMemoryP = totalMemory;
    }
    if (dynamicMemoryP != NULL)
    {
        *dynamicMemoryP = dynamicMemory;
    }
    return (freeMemory);
}

/*=========================================================================*/
/*   Function : ShowDeviceInfo_NNsh                                        */
/*                                                  デバイス個別情報の表示 */
/*=========================================================================*/
void ShowDeviceInfo_NNsh(void)
{
    Err          ret;
    Char         *osVer, buffer[BUFSIZE * 2];
    UInt8        remainPer;
    UInt32       currentSec, useSize, totalSize, freeSize;
    DateTimeType dateBuf;

    MemSet (buffer, sizeof(buffer), 0x00);

    // 現在時刻とバッテリ情報を取得する
    MemSet(&dateBuf, sizeof(dateBuf), 0x00);
    remainPer      = 0;
    currentSec     = TimGetSeconds ();
    SysBatteryInfo(false, NULL, NULL, NULL, NULL, NULL, &remainPer);
    TimSecondsToDateTime(currentSec, &dateBuf);    

    // 現在の日時をバッファに出力する
    StrCat (buffer, MSG_DEVICEINFO_DATE);
    DateToAscii((UInt8)  dateBuf.month,
                (UInt8)  dateBuf.day,
                (UInt16) dateBuf.year, 
                dfYMDLongWithDot,
                &buffer[StrLen(buffer)]);
    TimeToAscii((UInt8) dateBuf.hour,
                (UInt8) dateBuf.minute,
                tfColon24h,
                &buffer[StrLen(buffer)]);
    StrCat (buffer, "\n");

    // OSのバージョンを出力する
    osVer = SysGetOSVersionString();
    StrCat (buffer, "PalmOS ");
    StrCat (buffer, osVer);
    StrCat (buffer, "\n");
    MEMFREE_PTR(osVer);

    // 現在のバッテリ残量をバッファに出力する
    StrCat (buffer, MSG_DEVICEINFO_BATTERY);
    NUMCATI(buffer, (UInt16) remainPer); 
    StrCat (buffer, "%");

    // 空きメモリ情報の取得
    freeSize = GetOSFreeMem(&totalSize, &useSize);
    {
        // VFSの容量(空き/最大)を出力する
        StrCat (buffer, MSG_DEVICEINFO_MEM);
        NUMCATI(buffer, ((freeSize)/1024));
        StrCat (buffer, "kB/");
        NUMCATI(buffer, ((totalSize/1024))); 
        StrCat (buffer, "kB");
    }

    // VFS情報の取得
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFSの容量を確認する
        ret = VFSVolumeSize(NNshGlobal->vfsVol, &useSize, &totalSize);
        if (ret == errNone)
        {
            // VFSの容量(空き/最大)を出力する
            StrCat (buffer, MSG_DEVICEINFO_VFS);
            NUMCATI(buffer, ((totalSize - useSize)/1024));
            StrCat (buffer, "kB/");
            NUMCATI(buffer, ((totalSize/1024))); 
            StrCat (buffer, "kB");
        }
    }
    FrmCustomAlert(ALTID_INFO, buffer, "", "");

    return;
}

/*=========================================================================*/
/*   Function : SeparateWordList                                           */
/*                                  文字列をセパレータで分割してコピーする */
/*=========================================================================*/
Err SeparateWordList(Char *string, NNshWordList *wordList)
{
    Char   *ptr;
    UInt16  len;

    // キーワード数をクリアする
    wordList->nofWord = 0;

    if (StrLen(string) == 0)
    {
        //  分割する文字列が指定されていなかった場合には終了する。
        return (errNone);
    }

    // 領域を確保し、NG文字列をコピー
    MEMFREE_PTR(wordList->wordString);
    len = StrLen(string) + MARGIN;
    wordList->wordString = MEMALLOC_PTR(len);
    if (wordList->wordString == NULL)
    {
        return (~errNone);
    }
    MemSet (wordList->wordString, len, 0x00);
    StrCopy(wordList->wordString, string);

    // セパレータ(',')単位で文字列の先頭ポインタをチェックする
    ptr = wordList->wordString;
    (wordList->nofWord)++;
    ptr++;
    while (*ptr != '\0')
    {
        if (*ptr == NNSH_NGSET_SEPARATOR)
        {
            // 文字列のセパレータ発見！(文字列を区切る)
            *ptr = '\0';
            // セパレータが連続していた場合は読みとばす...
            do
            {
                ptr++;
            } while ((*ptr != '\0')&&(*ptr == NNSH_NGSET_SEPARATOR));
            if (*ptr != '\0')
            {
                (wordList->nofWord)++;
            }
        }
        else
        {
            ptr++;
        }
    }

    // 文字列リストのメモリハンドルを作成する
    MEMFREE_HANDLE(wordList->wordmemH);
    wordList->wordmemH = 
        SysFormPointerArrayToStrings(wordList->wordString, wordList->nofWord);

    return (errNone);
}

/*=========================================================================*/
/*   Function : ReleaseWordList                                            */
/*                                            リスト文字列の領域を解放する */
/*=========================================================================*/
void ReleaseWordList(NNshWordList *wordList)
{
    MEMFREE_PTR   (wordList->wordString);
    MEMFREE_HANDLE(wordList->wordmemH);
    wordList->nofWord = 0;    

    return;
}

/*=========================================================================*/
/*   Function : LaunchResource_NNsh                                        */
/*                                   リソース関数(例：DAアプリ)を起動する  */
/*=========================================================================*/
Boolean LaunchResource_NNsh(UInt32    type,    UInt32  creator,
                            DmResType resType, DmResID resID)
{
    UInt8               *funcPtr;
    Boolean              result;
    Err                  ret;
    UInt16               cardNo;
    LocalID              dbID;
    DmOpenRef            dbRef;
    MemHandle            memH;
    DmSearchStateType    state;
    FormActiveStateType  formState;

    // 変数の初期化
    cardNo = 0;
    dbID   = 0;
    result = false;
    MemSet(&state,     sizeof(state),     0x00);
    MemSet(&formState, sizeof(formState), 0x00);

    // 指定されたリソースがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &state,  type, creator, 
                                         true, &cardNo, &dbID);
    if (ret != errNone)
    {
        // インストールされていなかった、終了する。
        return (false);
    }

    // フォームの現在の状態を記憶する
    FrmSaveActiveState(&formState);
    
    // DBを拾う
    dbRef = DmOpenDatabase(cardNo, dbID, dmModeReadOnly);
    if (dbRef == 0)
    {
        // DB取得失敗
        goto FUNC_END;
    }

    // リソースを取得
    memH = DmGet1Resource(resType, resID);
    if (memH == 0)
    {
        // メモリハンドル取得失敗
        goto CLOSE_DB;
    }
    funcPtr = MemHandleLock(memH);
    if (funcPtr == NULL)
    {
        // アドレス取得失敗
        goto CLOSE_DB;
    }

    ///////////////////////////////////////////
    // 取得したリソースを関数として呼び出す
    ///////////////////////////////////////////
    ((void (*)(void))funcPtr)();

    // "実行したよ"フラグをたてる
    result = true;

    // リソースを解放         
    MemHandleUnlock(memH);
    DmReleaseResource(memH);

CLOSE_DB:
    (void) DmCloseDatabase(dbRef);

FUNC_END:
    // 保存していたフォームの状態を戻す
    FrmRestoreActiveState(&formState);
    return (result);
}

/*=========================================================================*/
/*   Function : CheckInstalledResource_NNsh                                */
/*                         リソース関数(例：DAアプリ)があるかチェックする  */
/*=========================================================================*/
Boolean CheckInstalledResource_NNsh(UInt32 type, UInt32 creator)
{
    Err                  ret;
    UInt16               cardNo;
    LocalID              dbID;
    DmSearchStateType    state;

    // 変数の初期化
    cardNo = 0;
    dbID   = 0;
    MemSet(&state, sizeof(state), 0x00);

    // 指定されたリソースがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &state,  type, creator, 
                                         true, &cardNo, &dbID);
    if (ret != errNone)
    {
        // インストールされていなかった、終了する。
        return (false);
    }
    return (true);
}

/* --------------------------------------------------------------------------
 *   AMsoftさんのUTF8変換テーブルを利用して実現する
 *       (http://amsoft.minidns.net/palm/gfmsg_chcode.html)
 * --------------------------------------------------------------------------*/

/*=========================================================================*/
/*   Function : StrNCopyUTF8toSJ                                           */
/*                     文字列のコピー(UTF8からSHIFT JISコードへの変換実施) */
/*=========================================================================*/
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size)
{
    UInt8     buffer[2];
    UInt16    cnt, codes, ucode;
    Boolean   kanji;
    DmOpenRef dbRef;   
    LocalID   dbId;

    // 変換テーブルがあるか確認する
    dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        // なかった場合には、変換せずそのままコピーする
        StrNCopy(dst, src, size);
        return;
    }
    dbRef = DmOpenDatabase(0 , dbId, dmModeReadOnly);

    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        if (*src < 0x80)
        {
            // 半角モード...
            *dst = *src;
            dst++;
            cnt++;
            src++;
            continue;
        }

        ucode = ((*src & 0x0f) << 12);
        ucode = ucode | ((*(src + 1) & 0x3f) << 6);
        ucode = ucode | ((*(src + 2) & 0x3f));

        buffer[0] = ((ucode & 0xff00) >> 8);
        buffer[1] = ((ucode & 0x00ff));

        // 漢字コードの変換
        codes = Unicode11ToJisx0208(dbRef, buffer[0], buffer[1]);
        buffer[0] = ((codes & 0xff00) >> 8);
        buffer[1] = (codes & 0x00ff);
        ConvertJIStoSJ(dst, buffer);

        dst = dst + 2;
        src = src + 3;
        cnt = cnt + 2;
    }
    *dst = '\0';

    DmCloseDatabase(dbRef);
    return;
}

/* --------------------------------------------------------------------------
 *  Unicode11ToJisx0208()
 *                                                       (Unicode > JIS0208)
 * --------------------------------------------------------------------------*/
static UInt16 Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L)
{
    MemHandle            recH;
    UInt16               num, col, code = 0x0000;
    UnicodeToJis0208Rec *rec;

    // データベースのレコードがなかったとき...
    if (dbRef == 0)
    {
        // エラー...
        return (0x2121);
    }
    num  = utf8H;

    // 現物あわせ､､､
    if (utf8L < 0x80)
    {
        num = (utf8H * 2);
        col = utf8L;
    }
    else
    {
        num = (utf8H * 2) + 1;
        col = utf8L - 0x80;
    }
    
    recH = DmQueryRecord(dbRef, num);
    if (recH == 0)
    {
        // エラー...
        return (0x2122);
    }
    rec = (UnicodeToJis0208Rec *) MemHandleLock(recH);
    if (rec->size > 0)
    {
        // データ有り
        code  = rec->table[col];
    }
    else
    {
        // データなし
        code = 0x2121;
    }
    MemHandleUnlock(recH);
    return (code);
}

#ifdef USE_LOGCHARGE
/*-------------------------------------------------------------------------*/
/*   Function : ParseMessage_UTF8                                          */
/*                                          (メッセージの整形処理/RDF形式) */
/*-------------------------------------------------------------------------*/
void ParseMessage_UTF8(Char *buf, Char *source, UInt32 size,
                       UInt16 *nofJmp, UInt16 *jmpBuf, UInt16 kanjiCode,
                       Boolean setJumpFlag)
{
    UInt16 fontFlag, ucode, codes;
    UInt8 *ptr, *dst;
    Char   convBuf[4];
    DmOpenRef dbRef;   
    LocalID   dbId;

    // 変換テーブルがあるか確認する
    dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        // なかった場合には、変換せずそのままコピーする
        StrNCopy(buf, source, size);
        return;
    }
    dbRef = DmOpenDatabase(0 , dbId, dmModeReadOnly);

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

    // 超遅い解釈ルーチンかも... (１文字づつパースする)
    dst  = buf;
    ptr  = source;
    MemSet(convBuf, sizeof(convBuf), 0x00);
    while (ptr < ((UInt8 *) source + size))
    {
        // UTF8に文字列を変換する...
        if (*ptr >= 0x80)
        {
            ucode = ((*ptr & 0x0f) << 12);
            ucode = ucode | ((*(ptr + 1) & 0x3f) << 6);
            ucode = ucode | ((*(ptr + 2) & 0x3f));

            convBuf[0] = ((ucode & 0xff00) >> 8);
            convBuf[1] = ((ucode & 0x00ff));

            // 漢字コードの変換(UTF8 -> SHIFT JIS)
            codes = Unicode11ToJisx0208(dbRef, convBuf[0], convBuf[1]);
            convBuf[0] = ((codes & 0xff00) >> 8);
            convBuf[1] = (codes & 0x00ff);
            ConvertJIStoSJ(dst, convBuf);
            ptr = ptr + 3;
            dst = dst + 2;
            continue;
        }

        if (*ptr == '&')
        {
            // "&gt;" を '>' に置換
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                ptr = ptr + 4;   // StrLen(TAG_GT);
                *dst++ = '>';
                continue;
            }
            // "&lt;" を '<' に置換
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                ptr = ptr + 4;   // StrLen(TAG_LT);

                ///// エンコーディングされているアンカー / 改行は読み飛ばす
                if (((*ptr == 'b')||(*ptr == 'B'))&&
                    ((*(ptr + 1) == 'r')||(*(ptr + 1) == 'R')))
                {
                    *dst++ = '\n';

                    // アンカーの末尾まで省略する
                    ptr = ptr + 2;
                    while ((ptr > (UInt8 *) source)&&((*ptr != '&')||
                            (*(ptr + 1) != 'g')||(*(ptr + 2) != 't')||(*(ptr + 3) != ';')))
                    {
                        ptr++;
                    }
                    ptr = ptr + 4;
                    continue;
                }
                if ((*ptr == 'a')||(*ptr == 'A'))
                {
                    // アンカーの末尾まで省略する
                    ptr++;
                    while ((ptr < ((UInt8 *) source + size))&&((*ptr != '&')||
                            (*(ptr + 1) != 'g')||(*(ptr + 2) != 't')||(*(ptr + 3) != ';')))
                    {
                        ptr++;
                    }
                    ptr = ptr + 4;
                    continue;
                }
                if ((*ptr == '/')&&((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A')))
                {
                    // アンカーの末尾まで省略する
                    ptr = ptr + 2;
                    while ((ptr < ((UInt8 *) source + size))&&((*ptr != '&')||
                            (*(ptr + 1) != 'g')||(*(ptr + 2) != 't')||(*(ptr + 3) != ';')))
                    {
                        ptr++;
                    }
                    ptr = ptr + 4;
                    continue;
                }
                *dst++ = '<';
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
            //  "<>" は、セパレータ(無視する)
            if (*(ptr + 1) == '>')
            {
                ptr = ptr + 2;   // StrLen(DATA_SEPARATOR);
                continue;
            }
            //  "<br>" は、改行に置換
            if (((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r'))||
                ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'R')))
            {
                // 行末と行頭のスペースを削ってみる場合
                if ((ptr > (UInt8 *) source)&&(*(ptr - 1) == ' '))
                {
                    dst--;
                }
                if (*(ptr + 4) == ' ')
                {
                    *dst++ = '\n';
                    ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                    goto SKIP_TAG;
                }
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "<p*>" は、改行2つに置換
            if ((*(ptr + 1) == 'p')||(*(ptr + 1) == 'P'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 2;
                goto SKIP_TAG;
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
                // 強調色にする
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_HEADER;
                }
                goto SKIP_TAG;
            }

            //  "</h?>" は、改行 + 改行 に置換
            if (((*(ptr + 2) == 'h')||(*(ptr + 2) == 'H'))&&(*(ptr + 1) == '/'))
            {
                *dst++ = '\n';
                // 通常色にする
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                goto SKIP_TAG;
            }

            // <item は改行
            if (((*(ptr + 1) == 'i')||(*(ptr + 1) == 'I'))&&
                 ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
                 ((*(ptr + 3) == 'e')||(*(ptr + 3) == 'E'))&&
                 ((*(ptr + 4) == 'm')||(*(ptr + 4) == 'M')))                 
            {
                *dst++ = '\n';
                ptr = ptr + 5;
                goto SKIP_TAG;
            }

            // <link は色付け
            if (((*(ptr + 1) == 'l')||(*(ptr + 1) == 'L'))&&
                 ((*(ptr + 2) == 'i')||(*(ptr + 2) == 'I'))&&
                 ((*(ptr + 3) == 'n')||(*(ptr + 3) == 'N'))&&
                 ((*(ptr + 4) == 'k')||(*(ptr + 4) == 'K')))                 
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_ANCHOR;
                }
                ptr = ptr + 5;
                goto SKIP_TAG;
            }


            // <IMAGE ～ </IMAGE> は表示しない
            if (((*(ptr + 1) == 'i')||(*(ptr + 1) == 'I'))&&
                 ((*(ptr + 2) == 'm')||(*(ptr + 2) == 'M'))&&
                 ((*(ptr + 3) == 'a')||(*(ptr + 3) == 'A'))&&
                 ((*(ptr + 4) == 'g')||(*(ptr + 4) == 'G'))&&                 
                 ((*(ptr + 5) == 'e')||(*(ptr + 5) == 'E')))
            {
                ptr = ptr + 6;
                while ((ptr < ((UInt8 *) source + size))&&
                        ((*ptr != '<')&&(*(ptr + 1) != '/'))&&
                        (((*(ptr + 2) == 'i')||(*(ptr + 2) == 'I'))&&
                        ((*(ptr + 3) == 'm')||(*(ptr + 3) == 'M'))&&
                        ((*(ptr + 4) == 'a')||(*(ptr + 4) == 'A'))&&
                        ((*(ptr + 5) == 'g')||(*(ptr + 5) == 'G'))&&                 
                        ((*(ptr + 6) == 'e')||(*(ptr + 6) == 'E'))))
                {
                    ptr++;
                }
                goto SKIP_TAG;
            }


            // <TITLE は色付け
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                 ((*(ptr + 2) == 'i')||(*(ptr + 2) == 'I'))&&
                 ((*(ptr + 3) == 't')||(*(ptr + 3) == 'T'))&&
                 ((*(ptr + 4) == 'l')||(*(ptr + 4) == 'L'))&&                 
                 ((*(ptr + 5) == 'e')||(*(ptr + 5) == 'E')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_HEADER;
                }
                ptr = ptr + 6;
                goto SKIP_TAG;
            }

            // </title は改行
            if (((*(ptr + 1) == '/')||(*(ptr + 1) == '/'))&&
                 ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
                 ((*(ptr + 3) == 'i')||(*(ptr + 3) == 'I'))&&
                 ((*(ptr + 4) == 't')||(*(ptr + 4) == 'T'))&&                 
                 ((*(ptr + 5) == 'l')||(*(ptr + 5) == 'L'))&&
                 ((*(ptr + 6) == 'e')||(*(ptr + 6) == 'E')))                 
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                ptr = ptr + 7;
                goto SKIP_TAG;
            }

            // </link は改行
            if (((*(ptr + 1) == '/')||(*(ptr + 1) == '/'))&&
                 ((*(ptr + 2) == 'l')||(*(ptr + 2) == 'L'))&&
                 ((*(ptr + 3) == 'i')||(*(ptr + 3) == 'I'))&&
                 ((*(ptr + 4) == 'n')||(*(ptr + 4) == 'N'))&&                 
                 ((*(ptr + 5) == 'k')||(*(ptr + 5) == 'K')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                ptr = ptr + 6;
                goto SKIP_TAG;
            }

            // </item は改行
            if (((*(ptr + 1) == '/')||(*(ptr + 1) == '/'))&&
                 ((*(ptr + 2) == 'i')||(*(ptr + 2) == 'I'))&&
                 ((*(ptr + 3) == 't')||(*(ptr + 3) == 'T'))&&
                 ((*(ptr + 4) == 'e')||(*(ptr + 4) == 'E'))&&                 
                 ((*(ptr + 5) == 'm')||(*(ptr + 5) == 'M')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 6;
                goto SKIP_TAG;
            }

            // </desc はSPACE
            if (((*(ptr + 1) == '/')||(*(ptr + 1) == '/'))&&
                 ((*(ptr + 2) == 'd')||(*(ptr + 2) == 'D'))&&
                 ((*(ptr + 3) == 'e')||(*(ptr + 3) == 'E'))&&
                 ((*(ptr + 4) == 's')||(*(ptr + 4) == 'S'))&&                 
                 ((*(ptr + 5) == 'c')||(*(ptr + 5) == 'C')))
            {
                *dst++ = ' ';
                ptr = ptr + 6;
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

        // スペースが連続していた場合、１つに減らす
        if ((*ptr == ' ')&&(*(ptr + 1) == ' '))
        {
            ptr++;
            while ((*ptr == ' ')&&(ptr < ((UInt8 *) source + size)))
            {
                ptr++;
            }
            ptr--;
        }

        // NULL および 0x0a, 0x0d, 0x09(タブ) は無視する
        if ((*ptr != '\0')&&(*ptr != 0x0a)&&(*ptr != 0x0d)&&(*ptr != 0x09))
        {
            // 普通の一文字転写
            *dst++ = *ptr;
        }
        ptr++;
    }
    *dst++ = '\0';

    DmCloseDatabase(dbRef);
    return;
}
#endif  // #ifdef USE_LOGCHARGE

#ifdef USE_ZLIB
/*=========================================================================*/
/*   Function :   OpenZLib                                                 */
/*                                            zlibの初期化を行う by Nickle */
/*=========================================================================*/
Boolean OpenZLib(void)
{
    z_stream *z;

    // zlibの初期化
    if(ZLSetup != errNone)
    {
        // 失敗。SysZLib.prcがない？
        NNsh_DebugMessage(ALTID_ERROR, "Can't init zlib","", 0);
        return (false);
    }
 
    // z_streamの領域を確保する
    NNshGlobal->streamZ = MEMALLOC_PTR((sizeof(z_stream) + MARGIN));
    if (NNshGlobal->streamZ == NULL)
    {
        // 領域確保に失敗。。。
        NNsh_DebugMessage(ALTID_ERROR, "Can't allocate z_stream","", 0);
        return (false);
    }
    z = NNshGlobal->streamZ;
    MemSet(z, (sizeof(z_stream) + MARGIN), 0x00);
 
    // バッファのクリア
    z->zalloc   = Z_NULL;
    z->zfree    = Z_NULL;
    z->opaque   = Z_NULL;
    z->next_in  = Z_NULL;
    z->avail_in = 0;

    // gzipの場合はこう初期化しないといけない inflateInit(&z) じゃだめ
    // thx to http://ghanyan.monazilla.org/gzip.html
    #define MAX_WBITS 15
    if(inflateInit2(NNshGlobal->streamZ, -MAX_WBITS) != Z_OK)
    {
        // 失敗。なんで？
        NNsh_DebugMessage(ALTID_ERROR, "Can't init zlib","(inflateInit2())", 0);
        return (false);
    }
 
    // zlib展開先バッファの確保
    NNshGlobal->inflateBuf = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (NNshGlobal->inflateBuf == NULL)
    {
        // 領域の確保失敗！
        NNsh_DebugMessage(ALTID_ERROR, "ZLIB:inflateBuf", "MEMALLOC_PTR(), size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        return (false);
    }
    MemSet(NNshGlobal->inflateBuf, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    return (true);
}
#endif  // #ifdef USE_ZLIB

#ifdef USE_ZLIB
/*=========================================================================*/
/*   Function :   CloseZLib                                                */
/*                                          zlibの終了処理を行う by Nickle */
/*=========================================================================*/
void CloseZLib(void)
{
    // 確保していた領域を開放する
    inflateEnd(NNshGlobal->streamZ);
    MEMFREE_PTR(NNshGlobal->streamZ);
    MEMFREE_PTR(NNshGlobal->inflateBuf);
    ZLTeardown;
}
#endif  // #ifdef USE_ZLIB

#ifdef USE_ZLIB
/*-------------------------------------------------------------------------*/
/*   Function :   GetStatusCodeSize                                        */
/*        渡されたバッファの先頭にofflaw.cgiの返す応答コードがあった場合、 */
/*            そのサイズを計算して返す (InflateZLibの内部で使用) by Nickle */
/*-------------------------------------------------------------------------*/
static UInt32 Get2chStatusCodeSize(Char* buf, UInt32 bufSize)
{
    Char   *ptr;
    UInt16  cnt;

    // [+OK] の場合は差分
    // [-INCR] (Incorrect)の場合はすべてのデータ
    // [-ERR (テキスト)]の場合はなんかエラー
    // 例：+OK 23094/512K
    //       -INCR 23094/512K
    //       -ERR そんな板ないです
    // thx to ギコナビ ItenDownload.pas
    if (((buf[0] != '+')||(buf[1] != 'O')||(buf[2] != 'K'))&&
        ((buf[0] != '-')||(buf[1] != 'I')||(buf[2] != 'N')||
         (buf[3] != 'C')||(buf[4] != 'R'))&&
        ((buf[0] != '-')||(buf[1] != 'E')||(buf[2] != 'R')||(buf[3] != 'R')))
    {
        // 応答コードが見つからなかった、終了する
        return (0);
    }

    // 改行コードを探す
    cnt = 0;
    ptr = buf;
    while ((*ptr != '\0')&&(*ptr != '\r')&&(*ptr != '\n'))
    {
        cnt++;
        ptr++;
    }
    if (*ptr == '\0')
    {
        // 改行コードが見つからなかった、終了する
        return (0);
    }
    // 文字数を応答する
    return (cnt);
}
#endif  // #ifdef USE_ZLIB

#ifdef USE_ZLIB
/*-------------------------------------------------------------------------*/
/*   Function :   InflateZLib                                              */
/*      gzip圧縮データを展開しつつ、ファイルに書き込む                     */
/*                                       (ParseGZipの内部で使用) by Nickle */
/*-------------------------------------------------------------------------*/
static Err InflateZLib(NNshFileRef* fileRef, Char* inputBuf, UInt16 inputSize, Boolean firstBlock)
{
    int      status;
    UInt32    writeSize, fileWriteSize;
    Char*     mergeBuf = NULL;
    Err       err;
    UInt32    skipBytes = 0; // offlaw.cgiの応答コードがあったら読み飛ばす。そのサイズ
    z_stream *z;

    z = NNshGlobal->streamZ;
    
    /* 出力ポインタ(next_out) */
    z->next_out  = NNshGlobal->inflateBuf;

    /* 出力バッファ残量(avail_out) */
    z->avail_out = (NNshGlobal->NNsiParam)->bufferSize;
    if (z->avail_in != 0)
    {
        // 前回のデータの残りがある
        // 前回の残りと今回のデータをマージする
        mergeBuf = MEMALLOC_PTR(z->avail_in + inputSize);
        if (mergeBuf == NULL)
        {
            return (~errNone);
        }         
        MemMove(mergeBuf, NNshGlobal->inflateBuf, z->avail_in);
        MemMove(mergeBuf + z->avail_in, inputBuf, inputSize);

        z->next_in  = mergeBuf;
        z->avail_in = z->avail_in + inputSize;
    }
    else
    {
        // 前回のデータの残りはない
        z->next_in  = inputBuf;
        z->avail_in = inputSize;
    }

    // gzip展開を実施する
    status = inflate(z, Z_NO_FLUSH);
    if ((status != Z_OK)&&(status != Z_STREAM_END))
    {
        NNsh_DebugMessage(ALTID_ERROR, "zlib decode error", z->msg, status);
        MEMFREE_PTR(mergeBuf);
        z->avail_in = 0; // 次回にゴミをinflateしないようにクリアしておく
        return (~errNone);
    }

    while ((z->avail_in != 0)&&(status != Z_STREAM_END))
    {
        // まだ入力データがあるのでファイルに書き出して続きを展開
        fileWriteSize = (NNshGlobal->NNsiParam)->bufferSize - z->avail_out;

        // contentの最初なら、offlaw.cgiの返すステータスコードがついてるかも
        if (firstBlock != false)
        {
            skipBytes = Get2chStatusCodeSize(NNshGlobal->inflateBuf, fileWriteSize);
        }
        else
        {
            skipBytes = 0;
        }

        // ファイルに追記する
        err = AppendFile_NNsh(fileRef, (fileWriteSize - skipBytes), (NNshGlobal->inflateBuf + skipBytes), &writeSize);
        if (err != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", err);
        }
        if (writeSize == 0)
        {
            NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " write size == 0", writeSize);
        }

        // 出力ポインタを戻す(入力ポインタは自動で進んでいる)
        z->next_out  = NNshGlobal->inflateBuf;
        z->avail_out = (NNshGlobal->NNsiParam)->bufferSize;

        // 続きを展開
        status = inflate(z, Z_NO_FLUSH);
        if ((status != Z_OK)&&(status != Z_STREAM_END))
        {
            NNsh_DebugMessage(ALTID_ERROR, "zlib decode error(2)", z->msg, status);
            z->avail_in = 0; // 次回にゴミをinflateしないように
            break;
        }
    }

    // 最後の一回の書き込み
    fileWriteSize = (NNshGlobal->NNsiParam)->bufferSize - z->avail_out;

    // contentの最初なら、offlaw.cgiの返すステータスコードがついてるかも
    if(firstBlock != false)
    {
        skipBytes = Get2chStatusCodeSize(NNshGlobal->inflateBuf, fileWriteSize);
    }
    else
    {
        skipBytes = 0;
    }

    // ファイルに追記する
    err = AppendFile_NNsh(fileRef, (fileWriteSize - skipBytes), (NNshGlobal->inflateBuf + skipBytes), &writeSize);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", err);
    }
    if (writeSize == 0)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " write size == 0", writeSize);
    }

    if(z->avail_in != 0)
    {
         // 入力バッファの末尾にデコードできてないデータが残ってる
         // なので、とりあえずそれをinflateBufに入れておいて次回使う
         MemMove(NNshGlobal->inflateBuf, z->next_in, z->avail_in);
    }
    MEMFREE_PTR(mergeBuf);

    return err;
}
#endif  // #ifdef USE_ZLIB

#ifdef USE_ZLIB
/*=========================================================================*/
/*   Function :   ParseGZip                                                */
/*                               gzip圧縮されたcontentを展開する by Nickle */
/*=========================================================================*/
Err ParseGZip(NNshFileRef* fileRef, Char* inputBuf, UInt16 inputSize, Boolean firstBlock)
{
    // see RFC 1952 - GZIP file format specification version 4.3
    // #pragma pack(1) だとなぜかignoreされるのでこう書いてみる
    struct gzipHeader
    {
        UInt8  id1   __attribute__((packed));
        UInt8  id2   __attribute__((packed));
        UInt8  cm    __attribute__((packed));
        UInt8  flg   __attribute__((packed));
        UInt32 mtime __attribute__((packed));
        UInt8  xfl   __attribute__((packed));
        UInt8  os    __attribute__((packed));
    };

    // flgのビットマスク
    // static UInt8 FTEXT       = (1 << 0); ここでは使わない
    static UInt8 FHCRC      = (1 << 1);
    static UInt8 FEXTRA     = (1 << 2);
    static UInt8 FNAME      = (1 << 3);
    static UInt8 FCOMMENT   = (1 << 4);

    struct gzipHeader* gzHeader;
  
    gzHeader = (struct gzipHeader*)inputBuf;
  
    if ((firstBlock != false)&&
        (gzHeader->id1 == 0x1f)&&
        (gzHeader->id2 == 0x8b)&&
        (((gzHeader->flg) >> 5) == 0))
    {
        // ヘッダの条件を満たすので、ヘッダ部を読み飛ばす
        int headerSize = sizeof(struct gzipHeader);

        if ((gzHeader->flg & FEXTRA) != 0)
        {
            headerSize = headerSize + 2 + ByteSwap16(*(unsigned int*)(inputBuf + headerSize));
        }

        if ((gzHeader->flg & FNAME) != 0)
        {
            headerSize = headerSize + StrLen(inputBuf + headerSize);
        }

        if ((gzHeader->flg & FCOMMENT) != 0)
        {
            headerSize = headerSize + StrLen(inputBuf + headerSize);
        }

        if ((gzHeader->flg & FHCRC) != 0)
        {
            headerSize = headerSize + 2;
        }

        // ヘッダ部を除いてzlibに渡す
        NNsh_DebugMessage(ALTID_INFO, "info", " gzip headerSize:", headerSize);
        return (InflateZLib(fileRef, (inputBuf + headerSize), (inputSize - headerSize), firstBlock));
    }

    // ヘッダではなくデータの途中なのでそのままzlibに渡す
    // (完全なチェックではないが…実用上問題ないと思う)
    return InflateZLib(fileRef, inputBuf, inputSize, firstBlock);
}
#endif  // #ifdef USE_ZLIB

/*=========================================================================*/
/*   Function :   NNsh_BeepSound                                           */
/*                                                              音を鳴らす */
/*=========================================================================*/
void NNsh_BeepSound(SndSysBeepType soundType)
{
#ifdef USE_WAVE_MELODY
    if ((soundType == sndAlarm)&&((NNshGlobal->NNsiParam)->melody != 0))
    {
        // Sound Stream Featureがサポートされているか確認する
        Err    ret;
        UInt32 flags;
        ret = FtrGet(sysFileCSoundMgr, sndFtrIDVersion, &version);
        if (ret == errNone)
        {
            SndPtr soundPtr; 

            // TO DO
            // WAVEのリソースをつかまえ、ポインタをsoundPtrに設定する..
            //

            Int32  volume = sndAlarmVolume;
            flags = sndFlagSync;
            ret = SndPlayResource(soundPtr, volume, flags);
            if (ret != errNone)
            {
                SndPlaySystemSound(soundType);
            }
        }
    }
    else
#endif  // #ifdef USE_WAVE_MELODY
    {
        if (soundType == sndClick)
        {
            SndPlaySystemSound(sndAlarm);
        }
        else
        {
            SndPlaySystemSound(soundType);
        }
    }
	return;
}
