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
    UInt32        size;
    Err           err;

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
    exgSocket.name        = "NNsiMess.txt";

    // set to comm. with local machine (PIM) only
    exgSocket.localMode = 1;

    err = ExgPut(&exgSocket);
    if (err == errNone)
    {
        ExgSend(&exgSocket, data, size, &err);
        ExgDisconnect(&exgSocket, err);
    }

    NNsh_InformMessage(ALTID_INFO, MSG_SEND_TO_MEMOPAD, "", err);
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
/*   Function : StrCopySJtoJIS                                             */
/*                                     JIS漢字コードに変換して文字列コピー */
/*=========================================================================*/
void StrCopySJtoJIS(UInt8 *dst, UInt8 *src)
{
    UInt16 mode;

    mode = 0;  // ANKモード
    while (*src != '\0')
    {
        // 半角アルファベットコードはそのまま表示する
        if (*src <= 0x80)
        {
            if (mode == 1)
            {
                *dst = 0x1b;
                dst++;
                *dst = 0x28;
                dst++;
                *dst = 0x42;
                dst++;
            }
            mode = 0;
 
            *dst = *src;
            dst++;
            src++;
        }
        else
        {
            if (mode == 0)
            {
                *dst = 0x1b;
                dst++;
                *dst = 0x24;
                dst++;
                *dst = 0x42;
                dst++;
            }
            mode = 1;  // KNJモード

            (void) ConvertSJtoJIS(dst, src);
            dst = dst + 2;
            src = src + 2;
        }
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
/*   Function : convertLF                                                  */
/*                                                LF(改行コード)を変換する */
/*=========================================================================*/
static void convertLF(UInt16 newLine, UInt8 *dst, UInt16 *size)
{
    // LFコードを変換する
    switch (newLine)
    {
      case NNSH_LINECODE_CR:
        // 改行コードは、CR
        *dst = 0x0d;
        *size = 1;
        break;

      case NNSH_LINECODE_CRLF:
        // 改行コードは、CR + LF
        *dst = 0x0d;
        *(dst + 1) = 0x0a;
        *size = 2;
        break;

      case NNSH_LINECODE_LF:
      default:
        // 改行コードは、LF
        *dst = 0x0a;
        *size = 1;
        break;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : checkNewLine                                               */
/*                                          改行コードかどうかチェックする */
/*-------------------------------------------------------------------------*/
static Boolean checkNewLine(UInt16 newLine, UInt8 *src, UInt16 *size)
{
    *size = 0;
    switch (newLine)
    {
      case NNSH_LINECODE_CR:
        // 改行コードは、CR
        if (*src == 0x0d)
        {
            *size = 1;
            return (true);
        }
        break;

      case NNSH_LINECODE_CRLF:
        // 改行コードは、CR + LF
        if ((*src == 0x0d)&&(*(src + 1) == 0x0a))
        {
            *size = 2;
            return (true);
        }
        break;

      case NNSH_LINECODE_LF:
      default:
        // 改行コードは、LF
        if (*src == 0x0a)
        {
            *size = 1;
            return (true);
        }
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function : StrNCopyJIStoSJ                                            */
/*                      文字列のコピー(JISからSHIFT JISコードへの変換実施) */
/*=========================================================================*/
void StrNCopyJIStoSJ(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine)
{
    UInt16 cnt, codes;
    Boolean kanji;
    
    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt <size))
    {
        // 改行コード
        if (checkNewLine(newLine, src, &codes) == true)
        {
            // 改行コード検出、LFに変換する
            *dst = 0x0a;
            dst++;
            cnt++;
            src = src + codes;
            continue;
        }

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
            cnt = cnt + 2;
            src = src + 2;
        }
        else
        {
            // 通常モード...
            *dst = *src;
            dst++;
            cnt++;
            src++;
        }
    }
    *dst = '\0';
    return;
}

/*=========================================================================*/
/*   Function : StrNCopySJtoSJLF                                           */
/*                                    文字列のコピー(改行コードの変換実施) */
/*=========================================================================*/
void StrNCopySJtoSJLF(Char *dst, Char *src, UInt32 size, UInt16 newLine)
{
    UInt16 cnt, codes;
    
    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // 改行コード
        if (checkNewLine(newLine, src, &codes) == true)
        {
            // 改行コード検出、LFに変換する
            *dst = 0x0a;
            dst++;
            cnt++;
            src = src + codes;
            continue;
        }

        // 1バイトコピー
        *dst = *src;
        dst++;
        cnt++;
        src++;
    }
    *dst = '\0';

    return;
}

/*=========================================================================*/
/*   Function : StrNCopyEUCtoSJ                                            */
/*                      文字列のコピー(EUCからSHIFT JISコードへの変換実施) */
/*=========================================================================*/
void StrNCopyEUCtoSJ(Char *dst, Char *src, UInt32 size, UInt16 newLine)
{
    UInt16 copyByte, cnt, codes;

    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        if (checkNewLine(newLine, src, &codes) == true)
        {
            // 改行コード検出
            *dst = 0x0a;
            dst++;
            cnt++;
            src = src + codes;
            continue;
        }

        if (ConvertEUCtoSJ(dst, src, &copyByte) == true)
        {
            dst = dst + copyByte;
            cnt = cnt + copyByte;
            src = src + 2;
        }
        else
        {
            *dst = *src;
            dst++;
            cnt++;
            src++;
        }
    }
    *dst = '\0';
    return;
}

/*=========================================================================*/
/*   Function : StrNCopySJtoJIS                                            */
/*                      文字列のコピー(SHIFT JISからJISコードへの変換実施) */
/*=========================================================================*/
void StrNCopySJtoJIS(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine, Boolean isFtr)
{
    Char   buffer[4];
    UInt16 mode, cnt, codes;

    mode = 0;  // ANKモード
    cnt  = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // 改行コード(LF)をコンバートする
        // 改行コード(LF)をコンバートする
        if (*src == 0x0a)
        {
            if (isFtr == true)
            {
                convertLF(newLine, buffer, &codes);
                DmWrite(dst, cnt, buffer, codes);
            }
            else
            {
                convertLF(newLine, (dst + cnt), &codes);
            }
            cnt = cnt + codes;
            src++;
            continue;
        }

        // 半角アルファベットコードはそのまま表示する
        if (*src <= 0x80)
        {
            if (mode == 1)
            {
                if (isFtr == true)
                {
                    buffer[0] = 0x1b;
                    buffer[1] = 0x28;
                    buffer[2] = 0x42;
                    DmWrite(dst, cnt, buffer, 3);
                    cnt = cnt + 3;
                }
                else
                {
                    *(dst + cnt) = 0x1b;
                    cnt++;
                    *(dst + cnt) = 0x28;
                    cnt++;
                    *(dst + cnt) = 0x42;
                    cnt++;
                }
            }
            mode = 0;
 
            if (isFtr == true)
            {
                DmWrite(dst, cnt, src, 1);
            }
            else
            {
                *(dst + cnt) = *src;
            }
            cnt++;
            src++;
        }
        else
        {
            if (mode == 0)
            {
                if (isFtr == true)
                {
                    buffer[0] = 0x1b;
                    buffer[1] = 0x24;
                    buffer[2] = 0x42;
                    DmWrite(dst, cnt, buffer, 3);
                    cnt = cnt + 3;
                }
                else
                {
                    *(dst + cnt) = 0x1b;
                    cnt++;
                    *(dst + cnt) = 0x24;
                    cnt++;
                    *(dst + cnt) = 0x42;
                    cnt++;
                }
            }
            mode = 1;  // KNJモード

            if (isFtr == true)
            {
                (void) ConvertSJtoJIS(buffer, src);
                DmWrite(dst, cnt, buffer, 2);
            }
            else
            {
                (void) ConvertSJtoJIS((dst + cnt), src);            
            }
            cnt = cnt + 2;
            src = src + 2;
        }
    }
    if (isFtr == true)
    {
        buffer[0] = '\0';
        DmWrite(dst, cnt, buffer, 1);
    }
    else
    {
        *(dst + cnt) = '\0';
    }
    return;
}

/*=========================================================================*/
/*   Function : StrNCopySJtoEUC                                            */
/*                      文字列のコピー(SHIFT JISからEUCコードへの変換実施) */
/*=========================================================================*/
void StrNCopySJtoEUC(Char *dst, Char *src, UInt32 size, UInt16 newLine, Boolean isFtr)
{
    Char   buffer[4];
    UInt16 copyByte, parseByte, cnt, codes;

    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // 改行コード(LF)をコンバートする
        if (*src == 0x0a)
        {
            if (isFtr == true)
            {
                convertLF(newLine, buffer, &codes);
                DmWrite(dst, cnt, buffer, codes);
            }
            else
            {
                convertLF(newLine, (dst + cnt), &codes);
            }
            cnt = cnt + codes;
            src++;
            continue;
        }
        if (isFtr == true)
        {
            (void) ConvertSJtoEUC(buffer, src, &copyByte, &parseByte);
            DmWrite(dst, cnt, buffer, copyByte);
        }
        else
        {
            (void) ConvertSJtoEUC((dst + cnt), src, &copyByte, &parseByte);
        }
        cnt = cnt + copyByte;
        src = src + parseByte;
    }
    if (isFtr == true)
    {
        buffer[0] = '\0';
        DmWrite(dst, cnt, buffer, 1);
    }
    else
    {
        *(dst + cnt) = '\0';
    }
    return;
}

/*=========================================================================*/
/*   Function : StrNCopySJtoSJ                                             */
/*                                    文字列のコピー(改行コードの変換実施) */
/*=========================================================================*/
void StrNCopySJtoSJ(Char *dst, Char *src, UInt32 size, UInt16 newLine, Boolean isFtr)
{
    Char buffer[2];
    UInt16 cnt, codes;
    
    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // 改行コード(LF)をコンバートする
        if (*src == 0x0a)
        {
            if (isFtr == true)
            {
                convertLF(newLine, buffer, &codes);
                DmWrite(dst, cnt, buffer, codes);
            }
            else
            {
                convertLF(newLine, (dst + cnt), &codes);
            }
            cnt = cnt + codes;
            src++;
            continue;
        }

        // 1バイトコピー
        if (isFtr == true)
        {
            DmWrite(dst, cnt, src, 1);
        }
        else
        {
            *(dst + cnt) = *src;
        }
        cnt++;
        src++;
    }
    if (isFtr == true)
    {
        buffer[0] = '\0';
        DmWrite(dst, cnt, buffer, 1);
    }
    else
    {
        *(dst + cnt) = '\0';
    }
    return;
}
/*=========================================================================*/
/*   Function : StrNCopySJtoUTF8                                           */
/*                      文字列のコピー(SHIFT JISからJISコードへの変換実施) */
/*=========================================================================*/
void StrNCopySJtoUTF8(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine, Boolean isFtr)
{
    UInt8  buffer[4];
    UInt16 cnt, codes;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    cnt  = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // 改行コード(LF)をコンバートする
        if (*src == 0x0a)
        {
            if (isFtr == true)
            {
                convertLF(newLine, buffer, &codes);
                DmWrite(dst, cnt, buffer, codes);
            }
            else
            {
                convertLF(newLine, (dst + cnt), &codes);
            }
            cnt = cnt + codes;
            src++;
            continue;
        }

        // 半角アルファベットコードはそのまま表示する
        if (*src <= 0x80)
        { 
            if (isFtr == true)
            {
                DmWrite(dst, cnt, src, 1);
            }
            else
            {
                *(dst + cnt) = *src;
            }
            cnt++;
            src++;
            continue;
        }

        // 漢字コードはSJ -> JIS変換
        (void) ConvertSJtoJIS(buffer, src);

        // その後、UTF8変換...
        codes = Jisx0208ToUnicode11(NNsiGlobal->jis2unicodeDB, buffer[0], buffer[1]);

        // UCS-4 > UTF-8 3byte
        buffer[0] = (((codes >> 12) & 0x000F) | 0xE0);
        buffer[1] = (((codes >>  6) & 0x003F) | 0x80);
        buffer[2] = (((codes)       & 0x003F) | 0x80);
        if (isFtr == true)
        {
            DmWrite(dst, cnt, buffer, 3);
        }
        else
        {
            *(dst + cnt)     = buffer[0];
            *(dst + cnt + 1) = buffer[1];
            *(dst + cnt + 2) = buffer[2];
        }
        cnt = cnt + 3;
        src = src + 2;
    }
    if (isFtr == true)
    {
        buffer[0] = '\0';
        DmWrite(dst, cnt, buffer, 1);
    }
    else
    {
        *(dst + cnt) = '\0';
    }
    return;
}

/*=========================================================================*/
/*   Function : StrNCopyUTF8toSJ                                           */
/*                     文字列のコピー(UTF8からSHIFT JISコードへの変換実施) */
/*=========================================================================*/
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine)
{
    UInt8  buffer[2];
    UInt16 cnt, codes, ucode;
    Boolean kanji;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);
    
    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt <size))
    {
        // 改行コード
        if (checkNewLine(newLine, src, &codes) == true)
        {
            // 改行コード検出、LFに変換する
            *dst = 0x0a;
            dst++;
            cnt++;
            src = src + codes;
            continue;
        }
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
        codes = Unicode11ToJisx0208(NNsiGlobal->unicode2jisDB, buffer[0], buffer[1]);
        buffer[0] = ((codes & 0xff00) >> 8);
        buffer[1] = (codes & 0x00ff);
        ConvertJIStoSJ(dst, buffer);

        dst = dst + 2;
        src = src + 3;
        cnt = cnt + 2;
    }
    *dst = '\0';
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
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

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
    MemPtrFree(osVer);

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
    if ((NNsiParam->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFSの容量を確認する
        ret = VFSVolumeSize(NNsiGlobal->vfsVol, &useSize, &totalSize);
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
    wordList->wordString = MemPtrNew_NNsh(len);
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
/*   Function : ShowVersion_NNsh                                           */
/*                                                    バージョン情報の表示 */
/*=========================================================================*/
void ShowVersion_NNsh(void)
{
    Char         buffer[BUFSIZE * 2];

    MemSet (buffer, sizeof(buffer), 0x00);
    StrCopy(buffer, SOFT_NAME);
    StrCat (buffer, "\n");
    StrCat (buffer, SOFT_VERSION);
    StrCat (buffer, SOFT_REVISION);
    StrCat (buffer, "\n(");
    StrCat (buffer, SOFT_DATE);
#ifdef OFFLINE_DEBUG
    StrCat (buffer, "[OFFLINE DEBUG]");
#endif
    StrCat (buffer, ")\n    ");
    StrCat (buffer, SOFT_INFO);
    FrmCustomAlert(ALTID_INFO, buffer, "", "");

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

/*=========================================================================*/
/*   Function : MemPtrNew_NNsh                                             */
/*                                                         メモリ確保関数  */
/*=========================================================================*/
MemPtr MemPtrNew_NNsh(UInt32 size)
{
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    if (NNsiGlobal->palmOSVersion > 0x04000000)
    {
        // over 64kBの領域を確保する(OS4.x以上)
        return (MemGluePtrNew(size));
    }
    // 通常のメモリ確保
    return (MemPtrNew(size));
}
