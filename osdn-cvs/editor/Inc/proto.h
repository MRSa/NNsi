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
    {                            \
        MemPtrFree(ptr);         \
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
void CheckVFSdevice_NNsh(void);
void RestoreSetting_NNsh(Int16 version, UInt16 size, NNshSavedPref *prm);
void SaveSetting_NNsh   (Int16 version, UInt16 size, NNshSavedPref *param);

Err  NNshSilkMinimize(FormType *frm);
Err  NNshSilkMaximize(FormType *frm);

#ifdef USE_CLIE
Err  CLIEdisplayNotify    (SysNotifyParamType *notifyP);
void SetCLIESupport_NNsh  (void);
void ResetCLIESupport_NNsh(void);
#endif // #ifdef USE_CLIE

#ifdef USE_HANDERA
void SetHandEraSupport_NNsh(void);
#endif // #ifdef USE_HANDERA

void NNsi_FrmDrawForm(FormType *frm, Boolean redraw);

#ifdef USE_CLIE
void NNsi_HRSclUpdate   (ScrollBarType *barP);
void NNsi_HRFldDrawField(FieldType *fldP, UInt16 isBottom);
#endif

void NNshWinSetListItems(FormType *frm, UInt16 lstID, Char *list,
                         UInt16 cnt, UInt16 selItem, 
                         MemHandle *memH, Char **listP);
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Boolean redraw, 
                         Char *msg, UInt32 size);
void NNshWinGetFieldText(FormType *frm, UInt16 fldID, Char *area, UInt32 len);

void SetMsg_BusyForm    (Char *msg);
void Show_BusyForm      (Char *msg);
void Hide_BusyForm      (Boolean redraw);

void    NNshWinViewUpdateScrollBar(UInt16 fldID, UInt16 sclID);
Boolean NNshWinViewPageScroll(UInt16 fldID, UInt16 sclID, 
                              UInt16 lines, WinDirectionType direction);

Boolean NNsh_MenuEvt_Edit(EventType *event);


void    ShowVersion_NNsh(void);

Boolean  HandEraResizeForm(FormPtr frmP);
void    HandEraMoveDialog(FormPtr frmP);

void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef);
void CloseDatabase_NNsh(DmOpenRef dbRef);
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset);
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset);
Err  EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData);
Err IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index);
Err SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,
                      UInt16 offset, UInt16 size, void *matchedData,
                      UInt16 *index);
Err DeleteRecord_NNsh(DmOpenRef dbRef, void  *target,
                      UInt16 keyType,  UInt16 offset);
Err DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index);
Err GetRecord_NNsh(DmOpenRef dbRef, UInt16 index,
                   UInt16 size, void *matchedData);
Err UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, 
                      UInt16 size, void *updateData);
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count);

Err CloseFile_NNsh(NNshFileRef *fileRef);
Err OpenFile_NNsh(Char *fileName, UInt16 fileMode, NNshFileRef *fileRef);
Err GetFileSize_NNsh(NNshFileRef *fileRef, UInt32 *fileSize);
Err ReadFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size, 
                  void *ptr, UInt32 *readSize);
Err DeleteFile_NNsh(Char *fileName, UInt16 location);
Err CreateFile_NNsh(Char *fileName);
Err RenameFile_NNsh(Char *oldFileName, Char *newFileName);
Err CreateDir_NNsh(Char *dirName);
Err CopyFile_NNsh(Char *destFile, Char *sourceFile, UInt16 location);
Err WriteFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size,
		   void *ptr, UInt32 *writeSize);
Err AppendFile_NNsh(NNshFileRef *fileRef, UInt32 size, 
                    void *ptr, UInt32 *writeSize);
Err AppendFileAsURLEncode_NNsh (NNshFileRef *fileRef, UInt32  size,
                                void        *ptr,     UInt32 *writeSize);
Err BackupDatabaseToVFS_NNsh   (Char *dbName);
Err RestoreDatabaseFromVFS_NNsh(Char *dbName);

void SetControlValue(FormType *frm, UInt16 objID, UInt16 *value);
void UpdateParameter(FormType *frm, UInt16 objID, UInt16 *value);

/*** MESSAGE DIALOGUE MANAGE FUNCTIONS ***/
UInt16  NNsh_DialogMessage(UInt16 level, UInt16 altID, Char *mes1, Char *mes2, UInt32 num);

#define NNsh_ErrorMessage(altId,mes1,mes2,num)   NNsh_DialogMessage(NNSH_LEVEL_ERROR,   altId, mes1, mes2, num)
#define NNsh_WarningMessage(altId,mes1,mes2,num) NNsh_DialogMessage(NNSH_LEVEL_WARN,    altId, mes1, mes2, num)
#define NNsh_InformMessage(altId,mes1,mes2,num)  NNsh_DialogMessage(NNSH_LEVEL_INFO,    altId, mes1, mes2, num)
#define NNsh_ConfirmMessage(altId,mes1,mes2,num) NNsh_DialogMessage(NNSH_LEVEL_CONFIRM, altId, mes1, mes2, num)
#define NNsh_DebugMessage(altId,mes1,mes2,num)   NNsh_DialogMessage(NNSH_LEVEL_DEBUG,   altId, mes1, mes2, num)

UInt16 KeyConvertFiveWayToJogChara(EventType *event);
