/*============================================================================*
 *
 *  $Id: armc0002.c,v 1.26 2008/10/26 15:02:42 mrsa Exp $
 *
 *  FILE: 
 *     parse.c
 *
 *  Description: 
 *     メッセージの整形(ARMlet)
 *
 *===========================================================================*/
#define PARSE_C
#include "PceNativeCall.h"
#include "armutls.h"
#include "symbols.h"
#include <PalmTypes.h>

///////////////////////////////////////////////////////////////////////
// リソースタイプとIDを登録(うまくいかないようだ...)
#include "Standalone.h"
STANDALONE_CODE_RESOURCE_TYPE_ID('armc',0x0002);
///////////////////////////////////////////////////////////////////////

// とりあえずのプロトタイプ宣言
void ParseMessage_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP);

/*=========================================================================*/
/*   Function : ParseMessage_ARM                                           */
/*                                                  (メッセージパース処理) */
/*=========================================================================*/
void ParseMessage_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP)
{
    char          *buf, *source, *ptr, *dst, *num, *tblP, **zenTblP;
    unsigned long  nofJmp, *jmpBuf, *numjmp; 
    unsigned long  size, dataStatus, *args, jmp, hanzen, fontFlag;
    unsigned long  parseMode, parseLen;
    Boolean setJumpFlag;
    
    // 引数を取得(エンディアン変換を行う)
    /*****************************************************************/
    args    = userData68KP;
    buf     = (char *)  ReadUnaligned32(&args[0]);
    source  = (char *)  ReadUnaligned32(&args[1]);
    size    =           ReadUnaligned32(&args[2]);
    numjmp  = (unsigned long *) ReadUnaligned32(&args[3]);
    jmpBuf  = (unsigned long *) ReadUnaligned32(&args[4]);
    hanzen  =           ReadUnaligned32(&args[5]);
    zenTblP = (char **) ReadUnaligned32(&args[6]);
    setJumpFlag = (Boolean)ReadUnaligned32(&args[7]);
    parseMode   = (unsigned long) ReadUnaligned32(&args[8]);
    /*****************************************************************/

    nofJmp     = 0;
    dataStatus = MSGSTATUS_NAME;

    // 超遅い解釈ルーチンかも... (１文字づつパースする)
    dst  = buf;
    ptr  = source;
    parseLen = 0;

    fontFlag = (hanzen & 0xf0000000) >> 28;  // カラー表示設定
    hanzen   = hanzen & 0x0fffffff;          // 半角カナ→全角カナ変換

    // ヘッダ色にする
    if (fontFlag != NNSH_DISABLE)
    {
        *dst++ = NNSH_CHARCOLOR_ESCAPE;
        *dst++ = NNSH_CHARCOLOR_HEADER;
    }
    while (ptr < source + size)
    {
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
                    while ((jmp < NNSH_MESSAGE_LIMIT)&&
                           (*num <= '9')&&(*num >= '0'))
                    {
                        jmp = jmp * 10 + (*num - '0');
                        num++;
                    }
                    if ((nofJmp < JUMPBUF)&&(jmp != 0))
                    {
                        // スレ番をバッファに追加
                        WriteUnaligned32(&jmpBuf[nofJmp], jmp);
                        (nofJmp)++;
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
                (*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&
                (*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" を '    ' スペース１個に置換
            if ((*(ptr + 1) == 'n')&&(*(ptr + 2) == 'b')&&
                (*(ptr + 3) == 's')&&(*(ptr + 4) == 'p')&&
                (*(ptr + 5) == ';'))
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

            if (*(ptr + 1) == 'a')
            {
                // "&amp;" を '&' に置換
                if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                    (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
                {
                    *dst++ = '&';
                    ptr = ptr + 5;    // StrLen(TAG_AMP);
                    continue;
                }
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
                    if ((parseMode != 0)&&(parseLen >= NNSH_MAX_OMIT_LENGTH))
                    {
                        *dst++ = 0x18;  // chrEllipsis
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
                    if (parseMode != 0)
                    {
                        if (parseLen >= NNSH_MAX_OMIT_LENGTH)
                        {
                            *dst++ = 0x18;  // chrEllipsis
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
                    if (parseMode != 0)
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
                    WriteUnaligned32(numjmp, nofJmp);
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

            //  "<dt>" は、改行に置換
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
                *dst++ = '\n';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }
            
            //  "<h*>" は、改行 + 改行 に置換
            if ((*(ptr + 1) == 'h')||(*(ptr + 1) == 'H'))
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
                // continueはなし。。(タグは読み飛ばすため)
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
                // continueはなし。。(タグは読み飛ばすため)
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
                // continueはなし。。(タグは読み飛ばすため)
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
                // continueはなし。。(タグは読み飛ばすため)
            }

            // <tr>, <td> はスペース１つに変換
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R')||
                 (*(ptr + 2) == 'd')||(*(ptr + 2) == 'D')))
            {
                *dst++ = ' ';
                goto SKIP_TAG;
                // continueはなし。。(タグは読み飛ばすため)
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
                // continueはなし。。 (タグは読み飛ばすため)
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
                // </SCRIPT>の末尾が見つからなかった、、、タグ位置を戻す..
                ptr = tblP;                
                // continueはなし。。 (タグは読み飛ばすため)
                goto SKIP_TAG;
            }

            // <BE: は、BE IDのため読み飛ばさない。(画面表示する)
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
            // タグは読み飛ばす
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
            ptr++;
            // *dst++ = ' ';   // タグは完全無視とする
            continue;
        }

        // 半角カナ→全角カナ変換する場合の処理
        if (hanzen != 0)
        {
            if ((*ptr >= 0x81)&&(*ptr <= 0x9f))
            {
                // 2バイト文字と判定
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }
            if ((*ptr >= 0xe0)&&(*ptr <= 0xef))
            {
                // 2バイト文字と判定
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }
            if ((*ptr >= 0xa1)&&(*ptr <= 0xdf))
            {
                // カタカナ文字と判定、テーブルを使って半角カナ→全角変換する
                tblP = (char *) 
                            ByteSwap32(zenTblP[((unsigned char)*ptr) - 0xa1]);
                *dst++ = *tblP;
                tblP++;
                *dst++ = *tblP;
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

        // NULL および 0x0a, 0x0d は無視する
        if ((*ptr != '\0')&&(*ptr != 0x0a)&&(*ptr != 0x0d)&&(*ptr != 0x09))
        {
            switch (dataStatus)
            {
              case MSGSTATUS_NAME:
                if ((parseMode == 0)||
                    ((parseMode != 0)&&(parseLen < NNSH_MAX_OMIT_LENGTH)))
                {
                    // 一文字転写
                    *dst++ = *ptr;
                }
                parseLen++;
                break;

              case MSGSTATUS_EMAIL:
                if ((parseMode == 0)||
                    ((parseMode != 0)&&(parseLen < NNSH_MAX_OMIT_LENGTH)))
                {
                    // 一文字転写
                    *dst++ = *ptr;
                }
                parseLen++;
                break;

              case MSGSTATUS_DATE:
                if (parseMode != 0)
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

    WriteUnaligned32(numjmp, nofJmp);
    return;
}
