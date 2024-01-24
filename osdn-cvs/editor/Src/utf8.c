/*============================================================================*
 *  FILE: 
 *     utf8.c
 *
 *  Description: 
 *     shift jis <-> UTF8 convertion data
 *
 *===========================================================================*/
#define UTF8_C
#include "local.h"

/* --------------------------------------------------------------------------
 *   AMsoftさんのUTF8変換テーブルを利用して実現する
 *       (http://amsoft.minidns.net/palm/gfmsg_chcode.html)
 * --------------------------------------------------------------------------*/

/*===========================================================================*/
/*   isInstalledUTF8table()                                                  */
/*                     UTF8変換テーブルがインストールされているか確認する    */
/*===========================================================================*/
Boolean isInstalledUTF8table(void)
{
    LocalID dbId;

    dbId      = DmFindDatabase(0, "Jis0208ToUnicode");
    if (dbId == 0)
    {
        return (false);
    }
    dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        return (false);
    }
    return (true);
}

/* --------------------------------------------------------------------------
 *   OpenUtf8Database()
 *                                                変換用テーブルDBのオープン
 * --------------------------------------------------------------------------*/
Err OpenUtf8Database(DmOpenRef *jis2unicodeTbl, DmOpenRef *unicode2jisTbl)
{
    LocalID dbId;

    *jis2unicodeTbl = 0;
    *unicode2jisTbl = 0;

    dbId      = DmFindDatabase(0, "Jis0208ToUnicode");
    if (dbId == 0)
    {
        return (~errNone);
    }
    *jis2unicodeTbl = DmOpenDatabase(0 , dbId, dmModeReadOnly);

    dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        DmCloseDatabase(*jis2unicodeTbl);
        *jis2unicodeTbl = 0;
        return (~errNone);
    }
    *unicode2jisTbl = DmOpenDatabase(0 , dbId, dmModeReadOnly);

    return (errNone);
}

/* --------------------------------------------------------------------------
 *   CloseUtf8Database()
 *                                                 変換テーブルDBのクローズ
 * --------------------------------------------------------------------------*/
void CloseUtf8Database(DmOpenRef tbl1, DmOpenRef tbl2)
{
    if (tbl1 != 0)
    {
        DmCloseDatabase(tbl1);
    }
    if (tbl2 != 0)
    {
        DmCloseDatabase(tbl2);
    }
    return;
}

/* --------------------------------------------------------------------------
 *  Unicode11ToJisx0208()
 *                                                       (JIS0208 > Unicode)
 * --------------------------------------------------------------------------*/
UInt16 Jisx0208ToUnicode11(DmOpenRef dbRef, UInt8 jisH, UInt8 jisL)
{
    UInt8                mod;
    MemHandle            recH;
    UInt16               num, code = 0x0000;
    //Jis0208ToUnicodeRec *rec;
    UnicodeToJis0208Rec  *rec;

    // データベースのレコードがなかったとき...
    if (dbRef == 0)
    {
        // エラー...
        return (0x03a7);
    }

    if ((0x21 <= jisH)&&(jisH <= 0x7e)&&(0x21 <= jisL)&&(jisL <= 0x7e))
    {
        // 現物あわせ、、、
        num  =  (jisH - 0x21);
        num  = num * 0x005e;
        num  = num + (jisL - 0x21);
        mod  = num % 128;
        num  = num / 128;

        recH = DmQueryRecord(dbRef, num);
        if (recH == 0)
        {
            // エラー...
            return (0x203b);
        }

        // rec  = (Jis0208ToUnicodeRec *) MemHandleLock(recH);
        rec  = (UnicodeToJis0208Rec *) MemHandleLock(recH);
        if (rec->size > mod)
        {
            code = rec->table[mod];
        }
        else
        {
            code = 0x203c;
        }
        MemHandleUnlock(recH);
    }
    return (code);
}

/* --------------------------------------------------------------------------
 *  Unicode11ToJisx0208()
 *                                                       (Unicode > JIS0208)
 * --------------------------------------------------------------------------*/
UInt16 Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L)
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
