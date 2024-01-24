/*============================================================================*
 *  FILE: 
 *     dbMgr.c
 *
 *  Description: 
 *     データベースマネージャ
 *===========================================================================*/
#define DBMGR_C
#include "local.h"

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF                                                */
/*                                                (ソーティング用比較関数) */
/*                       ※ NNsiデータベースの構造体を使用して並べ替えする */
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_Subject(void *rec1, void *rec2, Int16 other,
                                 SortRecordInfoPtr       rec1SortInfo,
                                 SortRecordInfoPtr       rec2SortInfo, 
                                 MemHandle               appInfoH)
{
    NNshSubjectDatabase *p1, *p2;
    Int16                ret;
    UInt8                sts1, sts2;
    
    p1 = (NNshSubjectDatabase *)rec1;
    p2 = (NNshSubjectDatabase *)rec2;

    // お気に入りスレなら、優先度順に並べ替える
    sts1 = (p1->msgAttribute & NNSH_MSGATTR_FAVOR);
    sts2 = (p2->msgAttribute & NNSH_MSGATTR_FAVOR);
    if (sts1 != sts2)
    {
        return (sts1 - sts2);
    }

#ifdef USE_SORT_BBS_NORMAL_ORDER
    // 板並びを昇順(A→Z)にする場合には、このロジックを有効にする
    // (通常は逆順でソートする)
    if ((p1->boardNick[0] == '!' && p2->boardNick[0] != '!') ||
        (p1->boardNick[0] != '!' && p2->boardNick[0] == '!'))
    {
        NNshSubjectDatabase *p3;

        /* どちらか一方が !GikoShippo の場合は、ソート結果を反転させる */
        /* （必ず前に集まるから...）                                   */
        p3 = p1;
        p1 = p2;
        p2 = p3;
    }
    ret = StrCompare(p2->boardNick, p1->boardNick);
#else
    ret = StrCompare(p1->boardNick, p2->boardNick);
#endif
    if (ret != 0)
    {
        // ボード名でソートする
        return (ret);
    }

    // ボード名が同じときで、状態が異なるとき
    if (p1->state != p2->state)
    {
        switch (p1->state)
        {
          case NNSH_SUBJSTATUS_NEW:
          case NNSH_SUBJSTATUS_UPDATE:
          case NNSH_SUBJSTATUS_REMAIN:
          case NNSH_SUBJSTATUS_ALREADY: 
          case NNSH_SUBJSTATUS_UNKNOWN: 
            if (p2->state == NNSH_SUBJSTATUS_NOT_YET)
            {
                // 未読は後ろ
                return (1);
            }
            break;
   
          case NNSH_SUBJSTATUS_NOT_YET:
          default:
            if (p2->state != NNSH_SUBJSTATUS_NOT_YET)
            {
                // 未読は後ろ
                return (-1);
            }
            break;
        }
    }

    /*** スレ番号でソートする ***/
    return (StrCompare(p2->threadFileName, p1->threadFileName));
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF                                                */
/*                                                (ソーティング用比較関数) */
/*                        ※ DBからotherバイト目に「文字列で」keyが並んで  */
/*                                                         いるものとする。*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_Char(void *rec1, void *rec2, Int16 other,
                              SortRecordInfoPtr       rec1SortInfo,
                              SortRecordInfoPtr       rec2SortInfo, 
                              MemHandle               appInfoH)
{
    Char *str1, *str2;

    str1 = ((Char *) rec1) + other;
    str2 = ((Char *) rec2) + other;
    return (StrCompare(str1, str2));
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF_UInt32                                         */
/*                                                (ソーティング用比較関数) */
/*                        ※ DBからotherバイト目に「UInt32で」keyが並んで  */
/*                                                         いるものとする。*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_UInt32(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH)
{
    Char   *loc1,  *loc2;
    UInt32 *data1, *data2;

    loc1  = ((Char *) rec1) + other;
    loc2  = ((Char *) rec2) + other;

    data1 = (UInt32 *) loc1;
    data2 = (UInt32 *) loc2;

    return (*data1 - *data2);
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF_UInt16                                         */
/*                                                (ソーティング用比較関数) */
/*                    ※ keyが先頭よりotherバイト目から「UInt16で」並んで  */
/*                                                         いるものとする。*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_UInt16(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH)
{
    Char   *loc1,  *loc2;
    UInt16 *data1, *data2;

    loc1  = ((Char *) rec1) + other;
    loc2  = ((Char *) rec2) + other;

    data1 = (UInt16 *) loc1;
    data2 = (UInt16 *) loc2;

    return (*data1 - *data2);
}

/*-------------------------------------------------------------------------*/
/*   Function : searchRecordSub                                            */
/*                                                        (レコードを検索) */
/*-------------------------------------------------------------------------*/
static Err searchRecordSub(DmOpenRef dbRef, void *target, UInt16 keyType, 
                           UInt16   offset, UInt16 *idx,  MemHandle *hnd)
{
    UInt16     nofData, start, end, mid;
    Int16      chk;
    MemHandle  dataH;
    Char      *data;

    // 検索する文字列をキャストする
    data = (Char *)target;
    data = data + offset;

    // データ数がゼロだったら検索失敗
    nofData = DmNumRecords(dbRef);
    if (nofData == 0)
    {
        return (~errNone - 5);
    }

    // ２分検索で指定されたデータが登録されているかチェック
    mid   = 0;
    start = 0;
    end   = nofData;
    while (start <= end)
    {
        mid = start + (end - start) / 2;
        dataH = DmQueryRecord(dbRef, mid);
        if (dataH == 0)
        {
            // 見つからなかった！ (ここで抜ける模様)
            return (~errNone);
        }

        // 指定されたキーによって、チェック方法を変える
        data = (Char *) MemHandleLock(dataH);
        data = data + offset;        // offsetがなかった、、、
        switch (keyType)
        {
          case NNSH_KEYTYPE_UINT32:
            chk  = *((UInt32 *) data) - *((UInt32 *) target);
            break;

          case NNSH_KEYTYPE_UINT16:
            chk  = *((UInt16 *) data) - *((UInt16 *) target);
            break;

          case NNSH_KEYTYPE_CHAR:
          default:
            chk  = StrCompare(data, (Char *) target);
            break;
        }
        if (chk == 0)
        {
            // 見つかった！(データをまるごとコピー)
            MemHandleUnlock(dataH);
            *idx  = mid;
            *hnd  = dataH;
            return (errNone);
        }
        MemHandleUnlock(dataH);
        if (chk < 0)
        {
            start = mid + 1;
        }
        else
        {
            end   = mid - 1;
        }
    }
    return (~errNone - 4);
}

/*=========================================================================*/
/*   Function : DeleteDB_wCreator                                          */
/*                                    データベースの削除(クリエータID指定) */
/*=========================================================================*/
Err DeleteDB_wCreator(Char *dbName, UInt16 chkVer, UInt32 creator, UInt32 type)
{
    LocalID    dbId;
    UInt16     dbVersion;

    // クリエータIDが指定されていない場合には、デフォルトのcreatorIDを使用する
    if (creator == 0)
    {
        creator = SOFT_CREATOR_ID;
    }
    if (type == 0)
    {
        type    = SOFT_DBTYPE_ID;
    }

    // データベースが存在するか確認する。
    dbId      = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // データベースのバージョン番号を取得
        (void) DmDatabaseInfo(0, dbId, NULL, NULL, &dbVersion, NULL, 
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (dbVersion == chkVer)
        {
            // データベースのバージョンが一致したときのみ削除する
            (void) DmDeleteDatabase(0, dbId);
            return (errNone);
        }
    }
    // データベースの削除を実行しなかったときには、エラー応答する
    return (~errNone);
}

/*=========================================================================*/
/*   Function : OpenDB_wCreator                                            */
/*                                データベースのオープン(クリエータID指定) */
/*=========================================================================*/
Err OpenDB_wCreator(Char *dbName, UInt16 chkVer, UInt32 creator, UInt32 type,
                                             Boolean erase, DmOpenRef *dbRef)
{
    LocalID    dbId;
    UInt16     dbVersion;

    // クリエータIDが指定されていない場合には、デフォルトのcreatorIDを使用する
    if (creator == 0)
    {
        creator = SOFT_CREATOR_ID;
    }
    if (type == 0)
    {
        type    = SOFT_DBTYPE_ID;
    }

    // データベースが存在するか確認する。
    *dbRef    = 0;
    dbId      = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // データベースのバージョン番号を取得
        (void) DmDatabaseInfo(0, dbId, NULL, NULL, &dbVersion, NULL, 
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if ((dbVersion < chkVer)||(erase == true))
        {
            // データベースのバージョンが古いとき、または消去モードだった時は、
            // いったんデータベースを削除する
            (void) DmDeleteDatabase(0, dbId);
            dbId = 0;
        }
    }

    // データベースが存在しないとき
    if (dbId == 0)
    {
        // データベースを新規作成
        (void) DmCreateDatabase(0, dbName, creator, type, false);

        dbId      = DmFindDatabase(0, dbName);
        if (dbId != 0)
        {
            // DBにバージョン番号を設定
            dbVersion = chkVer;
            (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, &dbVersion, NULL,
                                     NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        }
    }

    // データベースのオープン
    *dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);

    // エラー応答の判定
    if (*dbRef == 0)
    {
        return (~errNone);
    }
    return (errNone);
}

/*==========================================================================*/
/*  OpenDatabase_NNsh : データベースアクセスの初期化                        */
/*                                                                          */
/*==========================================================================*/
void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef)
{
    LocalID dbId;
    UInt16  dbVersion;

    // データベースが存在するか確認する。
    *dbRef    = 0;
    dbVersion = 0;
    dbId      = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // データベースのバージョン番号を取得
        (void) DmDatabaseInfo(0, dbId, NULL, NULL, &dbVersion, NULL, 
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (dbVersion < chkVersion)
        {
            // データベースのバージョンが古いときは、データベースを削除する
            (void) DmDeleteDatabase(0, dbId);
            dbId = 0;
        }
    }

    // データベースが存在しないとき
    if (dbId == 0)
    {
        // データベースを新規作成
        (void) DmCreateDatabase(0, dbName, 
                                SOFT_CREATOR_ID, SOFT_DBTYPE_ID, false);

        dbId      = DmFindDatabase(0, dbName);
        if (dbId != 0)
        {
            // DBにバージョン番号を設定
            dbVersion = chkVersion;
            (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, &dbVersion, NULL,
                                     NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        }
    }

    // データベースのオープン
    *dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);

    return;
}

/*==========================================================================*/
/*  closeDatabase_NNsh() : データベースをクローズする                       */
/*                                                                          */
/*==========================================================================*/
void CloseDatabase_NNsh(DmOpenRef dbRef)
{
    // DBがオープンできていたらCLOSEする。
    if (dbRef != 0)
    {
        (void) DmCloseDatabase(dbRef);
        dbRef = 0;
    }
    return;
}

/*=========================================================================*/
/*   Function : QsortRecord_NNsh                                           */
/*                                                (レコードのソーティング) */
/*=========================================================================*/
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset)
{

    // クイックソートを仕掛ける
    switch (keyType)
    {

      case NNSH_KEYTYPE_UINT32:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_UInt32, offset);
        break;

      case NNSH_KEYTYPE_UINT16:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_UInt16, offset);
        break;

      case NNSH_KEYTYPE_SUBJECT:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_Subject, offset);
        break;

      case NNSH_KEYTYPE_CHAR:
      default:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_Char, offset);
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function : IsortRecord_NNsh                                           */
/*                                                (レコードのソーティング) */
/*=========================================================================*/
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset)
{

    // 挿入ソートを仕掛ける
    switch (keyType)
    {
      case NNSH_KEYTYPE_UINT32:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_UInt32, offset);
        break;

      case NNSH_KEYTYPE_UINT16:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_UInt16, offset);
        break;

      case NNSH_KEYTYPE_SUBJECT:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_Subject,offset);
        break;

      case NNSH_KEYTYPE_CHAR:
      default:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_Char, offset);
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function : EntryRecord_NNsh                                           */
/*                                               (ＤＢの先頭にデータ登録)  */
/*=========================================================================*/
Err EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData)
{
    MemHandle  newRecH;
    UInt16     index;
    void      *dbP;

    // レコードの新規追加
    index   = 0;
    newRecH = DmNewRecord(dbRef, &index, size + sizeof(UInt32));
    if (newRecH == 0)
    {
        // レコード追加エラー
        return (DmGetLastErr());
    }

    // レコードにデータ書き込み
    dbP = MemHandleLock(newRecH);
    DmWrite(dbP, 0, recordData, size);
    MemHandleUnlock(newRecH);

    // レコードを解放（変更終了）
    DmReleaseRecord(dbRef, index, true);

    return (errNone);
}

/*=========================================================================*/
/*   Function : IsExistRecord_NNsh                                         */
/*                                                (レコードの存在チェック) */
/*=========================================================================*/
Err IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index)
{
    MemHandle  dataH;

    *index  = 0;
    dataH   = 0;
    return (searchRecordSub(dbRef, target, keyType, offset, index, &dataH));
}

/*=========================================================================*/
/*   Function : IsExistRecordRR_NNsh                                       */
/*                     (レコードの存在チェック - 先頭から末尾まで１こづつ) */
/*=========================================================================*/
Err IsExistRecordRR_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index)
{
    UInt16     nofData, loop;
    MemHandle  dataH;
    Int16      chk;
    Char      *data;

    // データ数がゼロだったら検索失敗
    nofData = DmNumRecords(dbRef);
    if (nofData == 0)
    {
        return (~errNone - 5);
    }

    for (loop = 0; loop < nofData; loop++)
    {
        dataH = DmQueryRecord(dbRef, loop);
        if (dataH == 0)
        {
            // 見つからなかった！ (ここで抜ける模様)
            return (~errNone);
        }
        // 指定されたキーによって、チェック方法を変える
        data = (Char *) MemHandleLock(dataH);
        data = data + offset;  // オフセットを加える
        switch (keyType)
        {
          case NNSH_KEYTYPE_UINT32:
            chk  = *((UInt32 *) data) - *((UInt32 *) target);
            break;

          case NNSH_KEYTYPE_UINT16:
            chk  = *((UInt16 *) data) - *((UInt16 *) target);
            break;

          case NNSH_KEYTYPE_CHAR:
          default:
            chk  = StrCompare(data, (Char *) target);
            break;
        }
        MemHandleUnlock(dataH);
        if (chk == 0)
        {
            // 見つかった！
            *index  = loop;
            return (errNone);
        }     
    }
    *index = 0;
    return (~errNone - 4);
}

/*=========================================================================*/
/*   Function : SearchRecord_NNsh                                          */
/*                                                        (レコードを検索) */
/*=========================================================================*/
Err SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,
                      UInt16 offset, UInt16 size, void *matchedData,
                      UInt16 *index)
{
    Err        ret;
    MemHandle  dataH;
#ifdef COPY_SELF
    Char      *sc,  *ds;
    UInt32    *src, *dest;
    UInt16     start;
#else
    UInt32    *src;
#endif

    *index = 0;
    dataH  = 0;

    ret    = searchRecordSub(dbRef, target, keyType, offset, index, &dataH);
    if (ret != errNone)
    {
        // データ見つからず
        return (ret);
    }
    if (dataH == 0)
    {
        // メモリハンドラ異常
        return ((~errNone) - 2);
    }

    src = (UInt32 *) MemHandleLock(dataH);
    if (src == NULL)
    {
        // メモリポインタ異常
        return ((~errNone) - 3);
    }

    // データをまるごとコピーして応答
#ifdef COPY_SELF
    MemSet(matchedData, size, 0x00);
    dest   = (UInt32 *) matchedData;
    for (start = 0; start < (size - sizeof(UInt32));
         start = start + sizeof(UInt32))
    {
        *dest++ = *src++;
    }
    for (sc = (Char *) src, ds = (Char *) dest;start < size;
         start = start + sizeof(Char))
    {
         *ds++ = *sc++;
    }
#else
    MemMove(matchedData, src, size);
#endif

    MemHandleUnlock(dataH);
    return (errNone);
}

/*=========================================================================*/
/*   Function : DeleteRecord_NNsh                                          */
/*                                                    (該当レコードを削除) */
/*=========================================================================*/
Err DeleteRecord_NNsh(DmOpenRef dbRef, void  *target,
                      UInt16 keyType,  UInt16 offset)
{
    Err       ret;
    UInt16    index;
    MemHandle dataH;

    index = 0;
    dataH = 0;
    ret = searchRecordSub(dbRef, target, keyType, offset, &index, &dataH);
    if (ret != errNone)
    {
        return (errNone + 1);
    }

    return (DmRemoveRecord(dbRef, index));
}

/*=========================================================================*/
/*   Function : DeleteRecordIDX_NNsh                                       */
/*                                                    (該当レコードを削除) */
/*=========================================================================*/
Err DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index)
{
    // データ数をオーバしているか確認
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone);
    }
    return (DmRemoveRecord(dbRef, index));
}

/*=========================================================================*/
/*   Function : GetRecord_NNsh                                             */
/*                                          (レコードをインデックスで獲得) */
/*=========================================================================*/
Err GetRecord_NNsh(DmOpenRef dbRef, UInt16 index,
                   UInt16 size, void *matchedData)
{
    MemHandle  dataH;
#ifdef COPY_SELF
    UInt16     start;
    UInt32    *src, *dest;
    Char      *sc,  *ds;
#else
    UInt32    *src;
#endif

    // データ数をオーバしているか確認
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone - 6);
    }

    dataH = DmQueryRecord(dbRef, index);
    if (dataH == 0)
    {
        // 見つからなかった！
        return (~errNone);
    }
    src = (UInt32 *) MemHandleLock(dataH);
    if (src == NULL)
    {
        // メモリポインタ異常
        return ((~errNone) - 3);
    }

    // データをまるごとコピーして応答
#ifdef COPY_SELF
    MemSet(matchedData, size, 0x00);
    dest   = (UInt32 *) matchedData;
    for (start = 0; start < (size - sizeof(UInt32));
         start = start + sizeof(UInt32))
    {
        *dest++ = *src++;
    }
    for (sc = (Char *) src, ds = (Char *) dest;start < size;
         start = start + sizeof(Char))
    {
         *ds++ = *sc++;
    }
#else
    MemMove(matchedData, src, size);
#endif
    MemHandleUnlock(dataH);
    return (errNone);
}

/*=========================================================================*/
/*   Function : GetRecordReadOnly_NNsh                                     */
/*                                          (レコードをインデックスで獲得) */
/*=========================================================================*/
Err GetRecordReadOnly_NNsh(DmOpenRef dbRef, UInt16 index,
                           MemHandle *dataH, void **record)
{
    // データ数をオーバしているか確認
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone - 6);
    }

    *dataH = DmQueryRecord(dbRef, index);
    if (*dataH == 0)
    {
        // 見つからなかった！
        return (~errNone);
    }

    // 領域をロックする
    *record = MemHandleLock(*dataH);
    if (record == NULL)
    {
        // メモリポインタ異常
        return ((~errNone) - 3);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : ReleaseRecordReadOnly_NNsh                                 */
/*                                                (レコード読み出しを終了) */
/*=========================================================================*/
Err ReleaseRecordReadOnly_NNsh(DmOpenRef dbRef, MemHandle dataH)
{
    if (dataH != 0)
    {
        MemHandleUnlock(dataH);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : UpdateRecord_NNsh                                          */
/*                                          (レコードをインデックスで更新) */
/*=========================================================================*/
Err UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, 
                      UInt16 size, void *updateData)
{
    MemHandle recH;
    void     *dbP;

    recH = DmGetRecord(dbRef, index);
    if (recH == 0)
    {
        // レコード参照エラー
        return (DmGetLastErr());
    }

    // レコードにデータ書き込み
    dbP = MemHandleLock(recH);
    DmWrite(dbP, 0, updateData, size);
    MemHandleUnlock(recH);

    // レコードを解放（変更終了）
    DmReleaseRecord(dbRef, index, true);

    return (errNone);
}

/*=========================================================================*/
/*   Function : GetRecordID_NNsh                                           */
/*                                      (レコードのユニークなIDを取得する) */
/*=========================================================================*/
Err GetRecordID_NNsh(DmOpenRef dbRef, UInt16 index, UInt32 *recId)
{
    return (DmRecordInfo(dbRef, index, NULL, recId, NULL));
}

/*=========================================================================*/
/*   Function : GetRecordIndexFromRecordID_NNsh                            */
/*                  (レコードのユニークなIDからインデックス番号を取得する) */
/*=========================================================================*/
Err GetRecordIndexFromRecordID_NNsh(DmOpenRef dbRef, UInt32 recId, UInt16 *index)
{
    return (DmFindRecordByID(dbRef, recId, index));
}

/*==========================================================================*/
/*  GetDBCount_NNsh : データベースに登録されているデータ件数を応答する      */
/*                                                                          */
/*==========================================================================*/
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count)
{
    *count = DmNumRecords(dbRef);
    return;
}

/*==========================================================================*/
/*  GetDBInfo_NNsh : データベースの情報の取得                               */
/*                                                                          */
/*==========================================================================*/
Err GetDBInfo_NNsh(Char *dbName, NNshDBInfo *info)
{
    LocalID dbId;
    
    dbId = DmFindDatabase(0, dbName);

    // データベースが存在しないとき
    if (dbId == 0)
        return(~errNone);

    return(DmDatabaseInfo(0, dbId,
                          info->nameP, info->attributesP,
                          info->versionP, info->crDateP,
                          info->modDateP, info->bckUpDateP,
                          info->modNumP, info->appInfoIDP,
                          info->sortInfoIDP, info->typeP,
                          info->creatorP));
}

/*==========================================================================*/
/*  GetExistRecordList : レスを取得しているスレッドのDBのレコード情報の取得 */
/*  (一覧で重複させないための情報取得)                                      */
/*==========================================================================*/
Boolean GetExistRecordList(DmOpenRef dbRef, Char *boardNick, MemHandle *listH, UInt16 *threadNum)
{
    UInt16 nofRecords, i;
    MemHandle            recH;
    NNshSubjectDatabase *chkDb;
    Char *buf;
    UInt32 areaSize, size;
    
    size       = 0;
    areaSize   = 0;
    *threadNum = 0;
    
    // 登録済みレコード数を獲得
    GetDBCount_NNsh(dbRef, &nofRecords);

    // boardNick のスレが格納されている DB の先頭を探す（レス情報領域の確保）
    for (i = 0; i < nofRecords; i++)
    {
        (void)GetRecordReadOnly_NNsh(dbRef, i, &recH, (void **)&chkDb);
        if (StrCompare(chkDb->boardNick, boardNick) == 0)
        {
            areaSize = MAX_THREADFILENAME + 2 + 6; // 6はレス数格納用領域(4 + 2) 
            *listH   = MemHandleNew(areaSize);
            if(*listH == 0)
            {
                // 領域確保失敗
                return (false);
            }

            // スレのファイル名とレス数を記録する
            buf      = MemHandleLock(*listH);
            MemSet (buf, areaSize, 0x00);
            StrCopy(buf, chkDb->threadFileName);
            switch (chkDb->state)
            {
              case NNSH_SUBJSTATUS_UPDATE:
              case NNSH_SUBJSTATUS_REMAIN:
              case NNSH_SUBJSTATUS_ALREADY:
              case NNSH_SUBJSTATUS_NEW:
                // レス取得済みの場合は、DBの値を反映させる
                NUMCATI(buf, chkDb->maxLoc);
                break;

              default:
                // 未取得の場合にはゼロを格納する
                NUMCATI(buf, 0);
                break;
            }

            // スレのファイル名のセパレータとして '\x0a' を使用する
            size = StrLen(buf);
            buf[size] = '\x0a';

            MemHandleUnlock(*listH);
            (*threadNum)++;
            
            ReleaseRecordReadOnly_NNsh(dbRef, recH);
            break;
        }
        ReleaseRecordReadOnly_NNsh(dbRef, recH);
    }

    // DB に入っていなければ false を返す。
    if (*threadNum == 0)
    {
        return (false);
    }
    
    // boardNick のスレが格納されていれば、リストに追加する
    for (i += 1; i < nofRecords; i++)
    {
         (void)GetRecordReadOnly_NNsh(dbRef, i, &recH, (void **)&chkDb);
        
        if (!StrCompare(chkDb->boardNick, boardNick))
        {
            // pos はセパレータの位置をさしているため、
            // pos + 1 + len + 1 -> pos + len + 2 の領域を確保する
            areaSize = size + MAX_THREADFILENAME + 2 + 6; // 6はレス数格納用領域(4 + 2) 
            if (MemHandleResize(*listH, areaSize) != errNone)
            {
                // 領域拡大失敗...
                MemHandleFree(*listH);
                ReleaseRecordReadOnly_NNsh(dbRef, recH);
                return(false);
            }
            // スレのファイル名を保存            
            buf = MemHandleLock(*listH);
            MemSet (&buf[size + 1], (areaSize - (size + 1)), 0x00);
            StrCat (buf, chkDb->threadFileName);
            NUMCATI(buf, chkDb->maxLoc);

            // スレのファイル名のセパレータとして '\x0a' を使用する
            size = StrLen(buf);
            buf[size] = '\x0a';

            MemHandleUnlock(*listH);
            (*threadNum)++;
        }
        ReleaseRecordReadOnly_NNsh(dbRef, recH);
    }
    return(true);
}
