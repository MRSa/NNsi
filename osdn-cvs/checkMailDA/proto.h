/*============================================================================*
 *  $Id: proto.h,v 1.5 2004/05/09 05:34:35 mrsa Exp $
 *
 *  FILE: 
 *     proto.h
 *
 *  Description: 
 *     Prototype definitions for NNsh.
 *
 *===========================================================================*/

/************************   MACROs   **************************************/
#define NUMCATI(buf,num)   StrIToA(&(buf)[StrLen((buf))], (num))
#define NUMCATH(buf,num)   StrIToH(&(buf)[StrLen((buf))], (num))

// 領域のクリア(ポインタ編)
#define MEMFREE_PTR(ptr)        \
    if (ptr != NULL)            \
    {                           \
        MemPtrFree(ptr);        \
        ptr = NULL;             \
    }

// 領域のクリア(ハンドル編)
#define MEMFREE_HANDLE(handle)  \
    if (handle != 0)            \
    {                           \
        MemHandleFree(handle);  \
        handle = 0;             \
    }


/******************  FUNCTION PROTOTYPES  *********************************/
/////  設定記録・復旧関数群(machine.c)
void RestoreSetting_NNsh(UInt16 size, NNshSavedPref *prm);
void SaveSetting_NNsh   (UInt16 size, NNshSavedPref *param);

/////  データベース操作関数群(dbmgr.c)
void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef);
void CloseDatabase_NNsh(DmOpenRef dbRef);
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset);
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset);
Err  EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData);
Err  IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType, UInt16 offset, UInt16 *index);
Err  SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,UInt16 offset, UInt16 size, void *matchedData, UInt16 *index);
Err  DeleteRecord_NNsh(DmOpenRef dbRef, void  *target, UInt16 keyType,  UInt16 offset);
Err  DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index);
Err  GetRecord_NNsh(DmOpenRef dbRef, UInt16 index, UInt16 size, void *matchedData);
Err  GetRecordReadOnly_NNsh(DmOpenRef dbRef, UInt16 index, MemHandle *dataH, void **record);
Err  ReleaseRecordReadOnly_NNsh(DmOpenRef dbRef, MemHandle dataH);
Err  UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, UInt16 size, void *updateData);
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count);
Err  GetDBInfo_NNsh(Char *dbName, NNshDBInfo *info);
                      
/////  ネットワーク操作系関数群 (netmgr.c)
Err NNshNet_open (UInt16 *netRef);
Err NNshNet_close(UInt16 netRef);
Err NNshNet_write(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, Int32 timeout);
Err NNshNet_read (UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, UInt16 *readSize, Int32 timeout);
Err NNshNet_disconnect(UInt16 netRef, NetSocketRef socketRef, Int32 timeout);
Err NNshNet_connect(NetSocketRef *socketRef, UInt16 netRef, Char *hostName, UInt16 port, Int32 timeout);
void NNshNet_LineHangup(void);

/////   その他のユーティリティ関数群(nnshcmn.c)
Err    SendToLocalMemopad(Char *title, Char *data);
void   ShowVersion_NNsh(void);
UInt16 KeyConvertFiveWayToJogChara(EventType *event);

///// イベントハンドラ
Boolean Handler_EditAction(EventType *event);
Boolean Handler_MainForm  (EventType *event);

/////  BUSYウィンドウ
void SetMsg_BusyForm(Char *msg);
void Show_BusyForm  (Char *msg, NNshWorkingInfo *NNshGlobal);
void Hide_BusyForm  (NNshWorkingInfo *NNshGlobal);

///// 画面関連
void NNshWinSetPopItems (FormType *frm, UInt16 popId, UInt16 lstId, UInt16 item);
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Char *msg, UInt32 size);
void NNshWinGetFieldText(FormType *frm, UInt16 fldID, Char *area, UInt32 len);
Boolean InputDialog(Char *title, Char *area, UInt16 size);

///// メールチェック本処理
void checkMailMessage_POP3(NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal, Char *buff, UInt8 *msgBuf);
UInt16 checkMail_POP3(UInt16 type, NNshSavedPref *NNshParam, NNshWorkingInfo *NNshGlobal, Char *buff, UInt8 *msgBuf);

//// MIME(Base64)の文字列変換
UInt16 StrCopyMIME64ToSJ(UInt8 *dst, UInt8 *src);
void ConvertJIStoSJ   (UInt8 *dst, UInt8 *src);
void StrCopyJIStoSJ   (UInt8 *dst, UInt8 *src);
void ConvertFromBASE64(UInt8 *dst, UInt8 *src, UInt16 size);


///// MD5
void CalcMD5(Char *buffer, Char *string, NNshWorkingInfo *NNshGlobal);
             
////////////////////////////////////////////////////////////////////////////
void MD5_memcpy   (UInt8   *output, UInt8  *input, UInt16 len);
void MD5sub_Encode(UInt8   *output, UInt32 *input, UInt16 len);
void MD5sub_Decode(UInt32  *output, UInt8  *input, UInt16 len);
void MD5Transform (UInt32   state[4],   UInt8    block[64]);
void MD5Final     (UInt8 digest[16], MD5_CTX *context, UInt8 *PADDING);
void MD5Update    (MD5_CTX *context, UInt8 *input, UInt16 inputLen);
void MD5Init      (MD5_CTX *context);
