/*============================================================================*
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

///////////////////////////////////////////////////////////////////////
// リソースタイプとIDを登録(うまくいかないようだ...)
#include "Standalone.h"
STANDALONE_CODE_RESOURCE_TYPE_ID('armc',0x0002);
///////////////////////////////////////////////////////////////////////

/*** Jump用バッファの最大サイズ ***/
#define JUMPBUF                     12
/*** メッセージ番号の最大値 ***/
#define NNSH_MESSAGE_LIMIT          1000

/**********  メッセージパース用シンボル(parse.c)  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

// とりあえずのプロトタイプ宣言
void ParseMessage_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP);
/*=========================================================================*/
/*   Function : CreateThreadIndexSub_ARM                                   */
/*                                              (インデックス取得の実処理) */
/*=========================================================================*/
void ParseMessage_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP)
{
    char          *buf, *source, *ptr, *dst, *num;
    unsigned long  nofJmp, *jmpBuf, *numjmp; 
    unsigned long  size, dataStatus, *args, jmp;

    // 引数を取得(エンディアン変換を行う)
    /*****************************************************************/
    args    = userData68KP;
    buf     = (char *) ReadUnaligned32(&args[0]);
    source  = (char *) ReadUnaligned32(&args[1]);
    size    =          ReadUnaligned32(&args[2]);
    numjmp  = (unsigned long *) ReadUnaligned32(&args[3]);
    jmpBuf  = (unsigned long *) ReadUnaligned32(&args[4]);
    /*****************************************************************/

    nofJmp     = 0;
    dataStatus = MSGSTATUS_NAME;

    // 超遅い解釈ルーチンかも... (１文字づつパースする)
    dst  = buf;
    ptr  = source;
    while (ptr < source + size)
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
                    WriteUnaligned32(numjmp, nofJmp);
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
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
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

    WriteUnaligned32(numjmp, nofJmp);
    return;
}
