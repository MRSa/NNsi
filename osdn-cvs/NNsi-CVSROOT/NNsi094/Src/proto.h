/*============================================================================*
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

/*--------------------  Generic Functions  --------------------*/

/*** WINDOW MANAGE FUNCTIONS ***/
void NNsi_FrmDrawForm   (FormType *frm);

void NNsi_HRSclUpdate   (ScrollBarType *barP);
void NNsi_HRFldDrawField(FieldType *fldP);

void NNshWinClearList(FormType *frm, UInt16 lstID);
void NNshWinSetListItems(FormType *frm, UInt16 lstID, Char *list, UInt16 cnt, UInt16 topItem, MemHandle *memH);
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Char *msg, UInt32 size);
void NNshWinGetFieldText(FormType *frm, UInt16 fldID, Char *area, UInt32 len);
void NNshWinViewUpdateScrollBar(UInt16 fldID, UInt16 sclID);
Boolean NNshWinViewPageScroll(UInt16 fldID, UInt16 sclID, 
                              UInt16 lines, WinDirectionType direction);
/*** BUSY WINDOW MANAGE FUNCTIONS ***/
void SetMsg_BusyForm(Char *msg);
void Show_BusyForm  (Char *msg);
void Hide_BusyForm  (void);

/*** Common Menus ***/
Boolean NNsh_MenuEvt_Edit(EventType *event);

/*** JUMP SELECTION WINDOW ***/
UInt16 NNshWinSelectionWindow(Char *listItemP, UInt16 nofItems);

/*** MESSAGE DIALOGUE MANAGE FUNCTIONS ***/
void    NNsh_DebugMessage  (UInt16 altID, Char *mes1, Char *mes2, UInt32 num);
UInt16  NNsh_ErrorMessage  (UInt16 altID, Char *mes1, Char *mes2, UInt32 num);
UInt16  NNsh_ConfirmMessage(UInt16 altID, Char *mes1, Char *mes2, UInt32 num);
#define NNsh_InformMessage  NNsh_ConfirmMessage

/*** DATABASE ACCESSING FUNCTIONS ***/
void OpenDatabase_NNsh (Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef);
void CloseDatabase_NNsh(DmOpenRef dbRef);
void QsortRecord_NNsh  (DmOpenRef dbRef, UInt16 keyType, UInt16 offset);
void IsortRecord_NNsh  (DmOpenRef dbRef, UInt16 keyType, UInt16 offset);
void GetDBCount_NNsh   (DmOpenRef dbRef, UInt16 *count);
Err  EntryRecord_NNsh  (DmOpenRef dbRef, UInt16 size,  void *recordData);
Err  GetRecord_NNsh    (DmOpenRef dbRef, UInt16 index, UInt16 size, void *matchedData);
Err  UpdateRecord_NNsh (DmOpenRef dbRef, UInt16 index, UInt16 size, void *updateData);
Err  IsExistRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType, UInt16 offset, UInt16 *index);
Err  SearchRecord_NNsh (DmOpenRef dbRef, void *target, UInt16 keyType, UInt16 offset, UInt16 size, void *matchedData, UInt16 *index);
Err  DeleteRecord_NNsh (DmOpenRef dbRef, void *target, UInt16 keyType, UInt16 offset);
Err  DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index);

/*** FILE(FileStream/VFS) ACCESSING FUNCTIONS ***/
Err DeleteFile_NNsh (Char *fileName, UInt16 location);
Err CreateFile_NNsh (Char *fileName);
Err RenameFile_NNsh (Char *oldFileName, Char *newFileName);
Err CreateDir_NNsh  (Char *dirName);
Err CloseFile_NNsh  (NNshFileRef *fileRef);
Err CopyFile_NNsh   (Char *destFile, Char *sourceFile, UInt16 location);
Err OpenFile_NNsh   (Char *fileName, UInt16 fileMode, NNshFileRef *fileRef);
Err GetFileSize_NNsh(NNshFileRef *fileRef, UInt32 *fileSize);
Err ReadFile_NNsh   (NNshFileRef *fileRef, UInt32 offset, UInt32 size, void *ptr, UInt32 *readSize);
Err WriteFile_NNsh  (NNshFileRef *fileRef, UInt32 offset, UInt32 size, void *ptr, UInt32 *writeSize);
Err AppendFile_NNsh (NNshFileRef *fileRef, UInt32 size, void *ptr, UInt32 *writeSize);
Err AppendFileAsURLEncode_NNsh(NNshFileRef *fileRef, UInt32 size, void *ptr, UInt32 *writeSize);
Err BackupDatabaseToVFS_NNsh   (Char *dbName);
Err RestoreDatabaseFromVFS_NNsh(Char *dbName);

/*** NETWORK CONNECTION FUNCTIONS ***/
void NNshNet_LineHangup(void);
Err NNshNet_open (UInt16 *netRef);
Err NNshNet_close(UInt16 netRef);
Err NNshNet_connect(NetSocketRef *socketRef, UInt16 netRef, Char *hostName, UInt16 port);
Err NNshNet_disconnect(UInt16 netRef, NetSocketRef socketRef);
Err NNshNet_write(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data);
Err NNshNet_read (UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, UInt16 *readSize);

/*** HTTP COMMUNICATION FUNCTIONS ***/
Err NNshHttp_comm(UInt16 type, Char *url, Char *cookie, Char *appendData,
                  UInt32 range, UInt32 endRange);
Err NNshHttp_commMain(UInt16 type, Char *url, Char *cookie, Char *appendData,
                      UInt32 range, UInt32 endRange, UInt16 port, Char *proxy);
Err WebBrowserCommand(UInt32 creator, Boolean subLaunch, UInt16 flags,
                      UInt16 cmd, Char *parameterP, UInt32 *resultP);

/*--------------------  NNsh Related Functions  --------------------*/
Boolean GetBBSList(Char *url);
Err Get_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo);
Err Update_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo);
Err Update_BBS_Database(UInt16 index, NNshBoardDatabase *bbs);
Err Get_BBS_Database(Char *boardNick, NNshBoardDatabase *bbs, UInt16 *index);
Err Get_BBS_Database_NNsh(Char *nick, NNshBoardDatabase *bbs, UInt16 *index);

Err CreateMessageThreadIndex(NNshMessageIndex *index, UInt32 *nofMsg,
                             UInt32 *msgOffset, UInt16 bbsType);
Err Get_AllMessage (Char *url, Char *boardNick, Char *file, UInt16 idxMES,
                    UInt16 bbsType);
Err Get_PartMessage(Char *url, Char *boardNick, NNshSubjectDatabase *mesDB, 
                    UInt16 idxMES);
Err MoveSubject_BBS(Char *dest, Char *src);
Err Get_Subject_Database(UInt16 index, NNshSubjectDatabase *subjDB);
Err Get_Subject_Database_NNsh(UInt16 index, NNshSubjectDatabase *subjDB);
Err create_offline_database(void);
Err Count_Thread_BBS(Char *boardNick, UInt16 *count);
Err check_same_thread(UInt16 selMES, NNshSubjectDatabase *data,
                      NNshSubjectDatabase *matchedData, UInt16 *idx);
Err Create_SubjectList(UInt16 index, UInt16 threadCnt, 
                       Char *buffer, UInt32 bufSize, UInt16 *cnt);
Err Get_Thread_Count(UInt16 index, UInt16 *count);
Err DeleteSubjectList(Char *boardNick, UInt16 delState, UInt16 *thrCnt);
Err NNsh_GetSubjectList(UInt16 index);

void MarkMessageIndex(UInt16 index, UInt16 favor);

UInt16 convertListIndexToMsgIndex(UInt16 listIdx);
Err GetSubDirectoryName(UInt16 index, Char *dirName);

Err OpenForm_NNshMessage(FormType *frm);

Err SetThreadInformation(Char *fileName,
                         UInt16 fileMode, NNshSubjectDatabase *subDB);

void SetControlValue(FormType *frm, UInt16 objID, UInt16 *value);
void UpdateParameter(FormType *frm, UInt16 objID, UInt16 *value);
Err  update_subject_database(UInt16 index, NNshSubjectDatabase *subjDB);

Err  NNshSilkMinimize(FormType *frm);
Err  NNshSilkMaximize(FormType *frm);
void HandEraMoveDialog(FormPtr frmP);
void HandEraResizeForm(FormPtr frmP);
void NNshRestoreNNsiSetting(Int16 version, UInt16 size, NNshSavedPref *prm);
void BackupDBtoVFS(UInt16 backupAllDB);
UInt16 KeyConvertFiveWayToJogChara(EventType *event);
