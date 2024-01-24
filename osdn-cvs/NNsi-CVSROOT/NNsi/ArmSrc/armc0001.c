/*============================================================================*
 *
 *  $Id: armc0001.c,v 1.4 2003/12/19 02:21:46 mrsa Exp $
 *
 *  FILE: 
 *     index.c
 *
 *  Description: 
 *     Calc. Message Index Location.
 *
 *===========================================================================*/
#define INDEX_C
#include "PceNativeCall.h"
#include "armutls.h"

///////////////////////////////////////////////////////////////////////
// リソースタイプとIDを登録(うまくいかないようだ...)
#include "Standalone.h"
STANDALONE_CODE_RESOURCE_TYPE_ID('armc',0x0001);
///////////////////////////////////////////////////////////////////////

// とりあえずのプロトタイプ宣言
unsigned long CreateThreadIndexSub_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP);

/*=========================================================================*/
/*   Function : CreateThreadIndexSub_ARM                                   */
/*                                              (インデックス取得の実処理) */
/*=========================================================================*/
unsigned long CreateThreadIndexSub_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP)
{
    char           *buffer, *chkPtr, *ptr;
    unsigned long   size, msgLimit, offset, *nofMsg, *msgOffset, msg, *args;

    // 引数を取得(エンディアン変換を行う)
    /*****************************************************************/
    args      = (unsigned long *) userData68KP;
    msgOffset = (unsigned long *) ReadUnaligned32(&args[0]);
    nofMsg    = (unsigned long *) ReadUnaligned32(&args[1]);
    offset    =                     ReadUnaligned32(&args[2]);
    msgLimit  =                     ReadUnaligned32(&args[3]);
    size      =                     ReadUnaligned32(&args[4]);
    buffer    = (char *)           ReadUnaligned32(&args[5]);
    /*****************************************************************/

    // 現在のメッセージ数を取得する
    msg    = ReadUnaligned32(nofMsg);
    chkPtr = buffer;
    do {
        // メッセージの区切りを探す
        ptr = chkPtr;
        while ((*ptr != '\x0a')&&(*ptr != '\0'))
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            ptr = ptr + 1;
            if (msg < msgLimit)
            {
                msg++;
                // 構造体にデータ書き込み
                WriteUnaligned32((msgOffset + msg),
                     ((unsigned long) ptr - (unsigned long) buffer + offset));
                chkPtr = ptr;
            }
            else
            {
                // オーバフロー時にも値を格納するよう変更する
                WriteUnaligned32(nofMsg, msg);
                return (0xffff);
            }
        }
        else
        {
            // 区切りがない、とりあえず次にすすむ
            ptr = ptr + 1;
        }
        chkPtr = ptr;
    } while (chkPtr < (buffer + size));

    // メッセージ数を書き込み
    WriteUnaligned32(nofMsg, msg);
    return (0x0000);
}
