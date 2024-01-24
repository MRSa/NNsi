/*============================================================================*
 *  FILE: 
 *     script.c
 *
 *  Description: 
 *     マクロスクリプトの読み込み/書き出し処理
 *
 *===========================================================================*/
#define SCRIPT_C
#include "local.h"

/*=========================================================================*/
/*   Function : readMacroDBData                                            */
/*                                                  マクロデータの読み出し */
/*=========================================================================*/
Err ReadMacroDBData(Char *dbName, UInt16 ver, UInt32 creator, UInt32 type,
                           UInt16 *cnt, UInt16 size, Char *dataPtr)
{
    Err              ret;
    UInt16           lp;
    NNshMacroRecord *lineData;
    DmOpenRef        dbRefR;

    *cnt = 0;
    if (dataPtr == NULL)
    {
        // バッファ格納領域がない、オープン失敗
        return (~errNone);
    }

    // データベースを開く(スクリプトファイル)
    OpenDB_wCreator(dbName, ver, creator, type, false, &dbRefR);
    if (dbRefR == 0)
    {
        // エラー発生、終了する
        return (~errNone);
    }

    // スクリプトのレコード数を確認する
    GetDBCount_NNsh(dbRefR, cnt);

    // 格納領域を越えたときは、バッファサイズまでにする
    if (*cnt > size)
    {
        *cnt = size;
    }

    // データの初期化
    lineData = (NNshMacroRecord *) dataPtr;
 
    // ログデータのバッファへの読み込み
    for (lp = 0; lp < *cnt; lp++)
    {
        // レコードを読み出す
        ret = GetRecord_NNsh(dbRefR, lp, sizeof(NNshMacroRecord), &lineData[lp]);
        if (ret != errNone)
        {
            // レコード読み出し失敗、データをクリアして次レコードへ
            MemSet(&lineData[lp], sizeof(NNshMacroRecord), 0x00);
        }
    }

    // あとしまつ
    CloseDatabase_NNsh(dbRefR);
    return (errNone);
}

/*=========================================================================*/
/*   Function : WriteMacroDBData                                           */
/*                                                  マクロデータの書き出し */
/*=========================================================================*/
Err WriteMacroDBData(Char *dbName, UInt16 ver, UInt32 creator, UInt32 type, 
                                                   UInt16 cnt, Char *dataPtr)
{
    UInt16           lp;
    DmOpenRef        dbRefW;
    NNshMacroRecord *lineData;

    // データベースを新規に開く
    OpenDB_wCreator(dbName, ver, creator, type, true, &dbRefW);
    if (dbRefW == 0)
    {
        // エラー発生、何もせず終了する
        return (~errNone);
    }

    // データの初期化
    lineData = (NNshMacroRecord *) dataPtr;
 
    // ログデータのバッファへの読み込み
    for (lp = 0; lp < cnt; lp++)
    {
        // レコードを書き出す
        lineData[lp].seqNum = lp + 1;
        (void) EntryRecord_NNsh(dbRefW, sizeof(NNshMacroRecord), &lineData[lp]);
    }

    // レコードの並べ替えを実施
    QsortRecord_NNsh(dbRefW, NNSH_KEYTYPE_UINT16, 0);

    // あとしまつ
    CloseDatabase_NNsh(dbRefW);
    return (errNone);
}

/*=========================================================================*/
/*   Function : AllocateMacroDBData                                        */
/*                                                      マクロデータの確保 */
/*=========================================================================*/
Char *AllocateMacroDBData(UInt16 len)
{
    Char *dataPtr;

    // データの初期化
    dataPtr = MemPtrNew_NNsh(sizeof(NNshMacroRecord) * len + MARGIN);
    if (dataPtr == NULL)
    {
        // メモリ確保エラー発生、終了する
        return (NULL);
    }
    MemSet(dataPtr, (sizeof(NNshMacroRecord) * len + MARGIN), 0x00);

    return (dataPtr);
}

/*=========================================================================*/
/*   Function : ReleaseMacroDBData                                         */
/*                                              確保したマクロデータの開放 */
/*=========================================================================*/
void ReleaseMacroDBData(Char *dataPtr)
{
    if (dataPtr != NULL)
    {
        MemPtrFree(dataPtr);
    }
    return;
}
