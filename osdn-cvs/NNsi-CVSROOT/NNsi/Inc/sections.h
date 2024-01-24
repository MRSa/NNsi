/*============================================================================*
 *
 * $Id: sections.h,v 1.202 2011/08/22 16:01:04 mrsa Exp $
 *
 *  FILE: 
 *     sections.h
 *
 *  Description: 
 *     Multi-segment definitions.
 *
 *===========================================================================*/
#include <PalmOS.h>
#include <PalmCompatibility.h>

#define EDIT_SECTION   __attribute__ ((section ("fns1")))
#define EDIT_SECTION2  __attribute__ ((section ("fns2")))
#define EDIT_SECTION3  __attribute__ ((section ("fns3")))
#define EDIT_SECTION4  __attribute__ ((section ("fns4")))
#define EDIT_SECTION5  __attribute__ ((section ("fns5")))
#define EDIT_SECTION6  __attribute__ ((section ("fns6")))
#define EDIT_SECTION7  __attribute__ ((section ("fns7")))
#define EDIT_SECTION8  __attribute__ ((section ("fns8")))
#define EDIT_SECTION9  __attribute__ ((section ("fns9")))
#define EDIT_SECTIONA  __attribute__ ((section ("fnsA")))
#define EDIT_SECTIONB  __attribute__ ((section ("fnsB")))
#define EDIT_SECTIONC  __attribute__ ((section ("fnsC")))
#define EDIT_SECTIOND  __attribute__ ((section ("fnsD")))
#define EDIT_SECTIONE  __attribute__ ((section ("fnsE")))
#define EDIT_SECTIONF  __attribute__ ((section ("fnsF")))

/*****  関数を追加したら、ここに記入する  *****/
/*****/
//------------------------------------------------------------//
///// bbsmng.c /////
#ifdef BBSMNG_C
static UInt16 getBoardURL(Char *url, Char *nick, Char *top, Char *bottom) EDIT_SECTION5;
static void parseBoard(DmOpenRef dbRef, Char *top, UInt32 *readSize) EDIT_SECTION5;
static void updateBoardList(void) EDIT_SECTION5;
static Err searchBBSTitle(Char *title, UInt16 direction, UInt16 *recNum) EDIT_SECTION5;
static void entryBBSRecord(UInt16 recNum) EDIT_SECTION5;
static void closeForm_SelectBBS(void) EDIT_SECTION5;
static void setBBSListItem(void) EDIT_SECTION5;
static void setAllBBS_Selection(UInt16 value) EDIT_SECTION5;
static Boolean detail_SelectBBS(void) EDIT_SECTION5;
static Boolean ctlSelEvt_SelectBBS(EventType *event) EDIT_SECTION5;
static Int16 ctlRepEvt_SelectBBS(EventType *event) EDIT_SECTION5;
static Int16 keyDownEvt_SelectBBS(EventType *event) EDIT_SECTION5;
static void set_Selection_Info(UInt16 pageNum) EDIT_SECTION5;
static void display_BBS_Selection(UInt16 pageNum, Int16 selection) EDIT_SECTION5;
#endif // #ifdef BBSMNG_C
Boolean GetBBSList(Char *url) EDIT_SECTION5;
Boolean Handler_SelectBBS(EventType *event) EDIT_SECTION5;
void OpenForm_SelectBBS(FormType *frm) EDIT_SECTION5;
Err Get_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo) EDIT_SECTION5;
Err Update_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo) EDIT_SECTION5;
Err Get_BBS_Database(Char *boardNick, NNshBoardDatabase *bbs, UInt16 *index) EDIT_SECTION5;
Err Update_BBS_Database(UInt16 index, NNshBoardDatabase *bbs) EDIT_SECTION5;
Err Create_BBS_INDEX(Char **bdLst, UInt16 *cnt) EDIT_SECTION5;

//------------------------------------------------------------//

///// dbmgr.c /////
#ifdef DBMGR_C
static Int16 aplCompareF_Subject(void *rec1, void *rec2, Int16 other,
                                 SortRecordInfoPtr       rec1SortInfo,
                                 SortRecordInfoPtr       rec2SortInfo, 
                                 MemHandle               appInfoH) EDIT_SECTION2;
static Int16 aplCompareF_Char(void *rec1, void *rec2, Int16 other,
                              SortRecordInfoPtr       rec1SortInfo,
                              SortRecordInfoPtr       rec2SortInfo, 
                              MemHandle               appInfoH) EDIT_SECTION2;
static Int16 aplCompareF_UInt32(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH) EDIT_SECTION2;
static Int16 aplCompareF_UInt16(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH) EDIT_SECTION2;
static Err searchRecordSub(DmOpenRef dbRef, void *target, UInt16 keyType, 
                           UInt16   offset, UInt16 *idx,  MemHandle *hnd) EDIT_SECTION2;
#endif // #ifdef DBMGR_C
Err DeleteDB_wCreator(Char *dbName, UInt16 chkVer, UInt32 creator, UInt32 type) EDIT_SECTION2;
Err OpenDB_wCreator(Char *dbName, UInt16 chkVer, UInt32 creator, UInt32 type,
                                             Boolean erase, DmOpenRef *dbRef) EDIT_SECTION2;
void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef) EDIT_SECTION2;
void CloseDatabase_NNsh(DmOpenRef dbRef) EDIT_SECTION2;
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset) EDIT_SECTION2;
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset) EDIT_SECTION2;
Err EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData) EDIT_SECTION2;
Err IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index) EDIT_SECTION2;
Err IsExistRecordRR_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index) EDIT_SECTION2;
Err SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,
                      UInt16 offset, UInt16 size, void *matchedData,
                      UInt16 *index) EDIT_SECTION2;
Err DeleteRecord_NNsh(DmOpenRef dbRef, void  *target,
                      UInt16 keyType,  UInt16 offset) EDIT_SECTION2;
Err DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index) EDIT_SECTION2;
Err GetRecord_NNsh(DmOpenRef dbRef, UInt16 index,
                   UInt16 size, void *matchedData) EDIT_SECTION2;
Err GetRecordReadOnly_NNsh(DmOpenRef dbRef, UInt16 index,
                           MemHandle *dataH, void **record) EDIT_SECTION2;
Err ReleaseRecordReadOnly_NNsh(DmOpenRef dbRef, MemHandle dataH) EDIT_SECTION2;
Err UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, 
                      UInt16 size, void *updateData) EDIT_SECTION2;
Err GetRecordID_NNsh(DmOpenRef dbRef, UInt16 index, UInt32 *recId) EDIT_SECTION2;
Err GetRecordIndexFromRecordID_NNsh(DmOpenRef dbRef, UInt32 recId, UInt16 *index) EDIT_SECTION2;
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count) EDIT_SECTION2;
Err GetDBInfo_NNsh(Char *dbName, NNshDBInfo *info) EDIT_SECTION2;
Boolean GetExistRecordList(DmOpenRef dbRef, Char *boardNick, MemHandle *listH, UInt16 *threadNum) EDIT_SECTION2;

//------------------------------------------------------------//

///// filemgr.c /////
#ifdef FILEMGR_C
static void SetFullPathFileName_NNsh(Char *file, Char *fullPath, UInt16 size) EDIT_SECTION2;
static Err openFile_VFSFile(Char *fileName, UInt16 fileMode, NNshFileRef *ref) EDIT_SECTION2;
static Err openFile_FileStream(Char *name, UInt16 fileMode, NNshFileRef *ref) EDIT_SECTION2;
#endif // #ifdef FILEMGR_C
Err CheckFileSpace_NNsh(UInt16 fileMode, UInt32 size) EDIT_SECTION2;
Err CloseFile_NNsh(NNshFileRef *fileRef) EDIT_SECTION2;
Err OpenFile_NNsh(Char *fileName, UInt16 fileMode, NNshFileRef *fileRef) EDIT_SECTION2;
Err GetFileSize_NNsh(NNshFileRef *fileRef, UInt32 *fileSize) EDIT_SECTION2;
Err ReadFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size, 
                  void *ptr, UInt32 *readSize) EDIT_SECTION2;
Err DeleteFile_NNsh(Char *fileName, UInt16 location) EDIT_SECTION2;
Err CreateFile_NNsh(Char *fileName) EDIT_SECTION2;
Err RenameFile_NNsh(Char *oldFileName, Char *newFileName) EDIT_SECTION2;
Err CreateDir_NNsh(Char *dirName) EDIT_SECTION2;
Err CopyFile_NNsh(Char *destFile, Char *sourceFile, UInt16 location) EDIT_SECTION2;
Err WriteFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size,
                                                void *ptr, UInt32 *writeSize) EDIT_SECTION2;
Err AppendFile_NNsh(NNshFileRef *fileRef, UInt32 size, 
                    void *ptr, UInt32 *writeSize) EDIT_SECTION2;
Err AppendFileAsURLEncode_NNsh(NNshFileRef *fileRef, UInt32  size,
                               void        *ptr,     UInt32 *writeSize) EDIT_SECTION2;
Err BackupDatabaseToVFS_NNsh(Char *dbName) EDIT_SECTION2;
Err RestoreDatabaseFromVFS_NNsh(Char *dbName) EDIT_SECTION2;

//------------------------------------------------------------//

///// getlog.c /////
#ifdef USE_LOGCHARGE
#ifdef GETLOG_C
//static Err getLogFileList(Char   *fileName,  Char   *baseURL,
//                          UInt32  tokenId,    UInt16 *urlCnt,
//                          Char  **listArray,  Char  **urlList,
//                          Char  **titleArray, Char  **titleList) EDIT_SECTION8;
static Err getLogFileList(Char *fileName, Char *baseURL, UInt32 tokenId, NNshGetLogTempURLDatabase *tempURL) EDIT_SECTION8;
static Boolean effectLogGetURL(FormType *frm) EDIT_SECTION8;
static Boolean displayLogDataInfo(FormType *frm, NNshGetLogDatabase *dbData) EDIT_SECTION8;
static Boolean effectLogToken(FormType *frm) EDIT_SECTION8;
static Boolean displayLogToken(FormType *frm, UInt16 recNum) EDIT_SECTION8;
#endif // #ifdef GETLOG_C
Err GetReadOnlyLogData_NNsh(void) EDIT_SECTION8;
Boolean Handler_LogGetURL(EventType *event) EDIT_SECTION8;
Boolean SetLogGetURL(UInt16 recNum) EDIT_SECTION8;
Boolean Handler_setLogToken(EventType *event) EDIT_SECTION8;
Boolean SetLogCharge_LogToken(UInt16 recNum) EDIT_SECTION8;
Err GetLogToken(UInt32 key, NNshLogTokenDatabase *record) EDIT_SECTION8;
UInt16 ConvertTokenListNum(UInt32 key) EDIT_SECTION8;
UInt16 GetLogTokenLabelList(NNshWordList *wordList, UInt32 **idList) EDIT_SECTION8;
UInt16 SelectLogToken(UInt32 *selection) EDIT_SECTION8;
Boolean GetImageFile(Char *url) EDIT_SECTION8;
Err     EntryLogSubFileDatabase(NNshGetLogSubFileDatabase *subDb) EDIT_SECTION8;
UInt16 GetSubFileDatabase(UInt16 command, UInt32 recId, 
                          Char *dataURL, NNshGetLogSubFileDatabase *subDb) EDIT_SECTION8;
Err     UpdateSubFileDatabase(UInt16 index, NNshGetLogSubFileDatabase *subDb) EDIT_SECTION8;
UInt16 PurgeSubFileDatabase(UInt16 command, UInt32 recId, UInt8 argument, UInt16 purgeCount) EDIT_SECTION8;
#endif // #ifdef USE_LOGCHARGE

//------------------------------------------------------------//

///// getloglist.c /////
#ifdef USE_LOGCHARGE
#ifdef GETLOGLIST_C
static UInt16 selectionMenu_getLogList(void) EDIT_SECTION8;
static void displayString(Char *lineData, RectangleType *dimF, UInt16 command) EDIT_SECTION8;
static void createList_getLogList(UInt16 getLevel, NNshDispList *displayList) EDIT_SECTION8;
static UInt16 openNNsiThread_getLogList(UInt16 recNum) EDIT_SECTION8;
static Err GetLogChargeDB(UInt16 recNum, NNshGetLogDatabase *dbData) EDIT_SECTION8;
static void showSiteInfo_getLogList(UInt16 recNum) EDIT_SECTION8;
static void beamSiteURL_getLogList(UInt16 recNum) EDIT_SECTION8;
static void openWebBrowser_getLogList(UInt16 recNum) EDIT_SECTION8;
static void openURLwebDA_getLogList(UInt16 recNum) EDIT_SECTION8;
static void openMemoURL_getLogList(UInt16 recNum) EDIT_SECTION8;
static UInt16 checkGadget_getLogList(EventType *event, FormType *frm, RectangleType *rect) EDIT_SECTION8;
static UInt16 selectOpenThread_getLogList(NNshGetLogDatabase *dbData, UInt16 recNum, Char *fileName) EDIT_SECTION8;
static Boolean isDeleteRecord(Boolean isReadLog, UInt8 state) EDIT_SECTION8;
static UInt16 deleteAllThread_getLogList(UInt16 recNum, Boolean isReadLog)  EDIT_SECTION8;
#endif  // #ifdef GETLOGLIST_C
void RefreshDisplay_getLogList(FormType *frm, UInt16 command, UInt16 parameter) EDIT_SECTION8;
Boolean Handler_GetLogList(EventType *event) EDIT_SECTION8;
Boolean Execute_GetLogList(UInt16 feature) EDIT_SECTION8;
void ClearList_getLogList(NNshDispList *dispList) EDIT_SECTION8;
Err CloseForm_GetLogList(UInt16 nextFormId) EDIT_SECTION8;
Err OpenForm_GetLogList(FormType  *frm) EDIT_SECTION8;
#endif  // #ifdef USE_LOGCHARGE


///// modifylog.c /////
#ifdef USE_LOGCHARGE
#ifdef MODIFYLOG_C
static void launch_viewJPEG(UInt16 fileLoc, Char *fileName) EDIT_SECTION8;
#endif  // #ifdef MODIFYLOG_C
Err ReadOnlyLogDataPickup(Boolean isImage, NNshGetLogDatabase *dbData,
                          Char *url, Char *fileName, UInt16 recNum, UInt32 recId,
                          UInt16 depth, UInt16 number, Char *dateTime) EDIT_SECTION8;
Err TrimGetDataLog(Char *fileName, Char *url, UInt32 tokenId, Char *realBuf, UInt16 bufSize) EDIT_SECTION8;
#endif  // #ifdef USE_LOGCHARGE

//------------------------------------------------------------//

///// httpmgr.c /////
#ifdef HTTPMGR_C
static void NNshHttp_divideHostName(Char *url, Char *host, Char **loc, UInt16 *portNo) EDIT_SECTION3;
static Err NNshHttp_createPostMsg(UInt16 type, Char *url, Char *host,
                                   Char *extendUserAgent, Char *cookie, 
                                   Char *appendData, UInt16 *portNo,
                                   Char *buffer, UInt32 bufSize, Char *proxy) EDIT_SECTION3;
static void NNshHttp_createGetMsg(UInt16 type, Char *url, Char *host,
                                   Char *extendUserAgent, UInt16 *portNo,
                                   UInt32 getRange, UInt32 endRange, 
                                   Char *buffer, Char *proxy, Char *appendData) EDIT_SECTION3;
#endif  // #ifdef HTTPMGR_C
Err NNshHttp_commMain(UInt16 type, Char *url, Char *cookie, Char *appendData,
                      UInt32 range, UInt32 endRange, UInt16 port, Char *proxy,
                      Char   *message, UInt16 sslMode) EDIT_SECTION3;

//------------------------------------------------------------//

///// inoutXML.c /////
#ifdef USE_XML_OUTPUT
#ifdef INOUTXML_C
    // static関数なし
#endif //  #ifdef INOUTXML_C
Err Output_NNsiSet_XML(Char *fileName) EDIT_SECTION9;
Err Input_NNsiSet_XML(Char *fileName)  EDIT_SECTION9;
#endif // #ifdef USE_XML_OUTPUT

//------------------------------------------------------------//

///// machine.c /////
#ifdef MACHINE_C
static void checkVFSdevice_NNsh(void) EDIT_SECTIONA;
// ※※ SECTION指定しない { ※※
static Err VFSUnmountNotify(SysNotifyParamType *notifyParamsP);
static Err VFSMountNotify(SysNotifyParamType *notifyParamsP);
// ※※ } SECTION指定しない ※※
static void resetVFSNotifications(void) EDIT_SECTIONA;

#ifdef USE_CLIE
static void setCLIESupport_NNsh(void) EDIT_SECTIONA;
static void resetCLIESupport_NNsh(void) EDIT_SECTIONA;
#endif  // #ifdef USE_CLIE
#ifdef USE_HANDERA
static void setHandEraSupport_NNsh(void) EDIT_SECTIONA;
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
static void setDIASupport_NNsh(void) EDIT_SECTIONA;
#endif // #ifdef USE_PIN_DIA
static void HandEraMoveObject(FormPtr frmP, UInt16 objIndex,
                              Coord x_diff, Coord y_diff) EDIT_SECTIONA;
static void HandEraResizeObject(FormPtr frmP, UInt16 objIndex,
                                Coord x_diff, Coord y_diff) EDIT_SECTIONA;
static void NNshSaveNNsiSetting(Int16 version, UInt16 size,
                                NNshSavedPref *param) EDIT_SECTIONA;
static void NNshClearGlobal(NNshWorkingInfo *workBuf) EDIT_SECTIONA;
static void checkInstalledWebBrowser(void) EDIT_SECTIONA;
#ifdef USE_HIGHDENSITY
static void setDoubleDensitySupport_NNsh(void) EDIT_SECTIONA;
#endif  // #ifdef USE_HIGHDENSITY
#endif #ifdef MACHINE_C
#ifdef USE_PIN_DIA
#ifdef USE_PIN_GARMIN
Err DisplayResizeNotify(SysNotifyParamType *notifyP) EDIT_SECTIONA;
#endif  // #ifdef USE_PIN_GARMIN
void resetDIASupport_NNsh(void) EDIT_SECTIONA;
Err VgaFormModify_DIA(FormType *frmP, VgaFormModifyType_DIA type) EDIT_SECTIONA;
void VgaFormRotate_DIA(void) EDIT_SECTIONA;
#endif  // #ifdef USE_PIN_DIA
#ifdef USE_CLIE
Err CLIEdisplayNotify(SysNotifyParamType *notifyP) EDIT_SECTIONA;
#endif // #ifdef USE_CLIE
Boolean HandEraResizeForm(FormPtr frmP) EDIT_SECTIONA;
void HandEraMoveDialog(FormPtr frmP) EDIT_SECTIONA;
void NNshRestoreNNsiSetting(Int16 version, UInt16 size, NNshSavedPref *prm) EDIT_SECTIONA;
Err NNshSilkMinimize(FormType *frm) EDIT_SECTIONA;
Err NNshSilkMaximize(FormType *frm) EDIT_SECTIONA;
void BackupDBtoVFS(UInt16 backupAllDB) EDIT_SECTIONA;
Err RestoreDBtoVFS(Boolean forceRead) EDIT_SECTIONA;
void SetZLIBsupport_NNsh(void) EDIT_SECTIONA;
UInt16 KeyConvertFiveWayToJogChara(EventType *event) EDIT_SECTIONA;
UInt16 ConvertFormID(UInt16 lastFrmID) EDIT_SECTIONA;

//------------------------------------------------------------//

///// macro.c /////
#ifdef USE_MACRO
#ifdef MACRO_C
static void outputMacroLog(DmOpenRef dbRefW, UInt8 level, UInt16 code, Char *data) EDIT_SECTIONC;
static UInt16 *macroSelectGR(NNshMacroRecord *lineData, UInt16 *gr) EDIT_SECTIONC;
static void macroDataOperation(NNshMacroRecord *lineData, UInt16 *gr, Boolean setter) EDIT_SECTIONC;
static UInt16 getMacroValue(NNshMacroRecord *lineData, UInt16 *gr) EDIT_SECTIONC;
static Boolean checkMacroCommandAvailable(UInt16 command, UInt16 halt) EDIT_SECTIONC;
static UInt16 setSourceDataDisplay(Char *buffer, NNshMacroRecord *lineData) EDIT_SECTIONC;
static void setExecFuncDataDisplay(Char *buffer, UInt16 command) EDIT_SECTIONC;
static UInt16 setFileListItems(Char *fnameP) EDIT_SECTIONC;
#endif // #ifdef MACRO_C
Boolean NNsi_ExecuteMacroSelection(void) EDIT_SECTIONC;
void NNsi_ExecuteMacroMain(Char *macroDBName, UInt16 macroDBVersion, UInt32 creator, UInt32 type) EDIT_SECTIONC;
void NNsi_MacroDataView(Char *macroDBName, UInt16 macroDBVersion) EDIT_SECTIONC;
void NNsi_MacroExecLogView(Char *macroDBName, UInt16 macroDBVersion) EDIT_SECTIONC;
#endif // #ifdef USE_MACRO

//------------------------------------------------------------//

///// macroedit.c /////
#ifdef USE_MACROEDIT
#ifdef MACROEDIT_C
   // static関数なし
#endif // #ifdef MACROEDIT_C
void NNsi_MacroDataEdit(Char *macroDBName, UInt16 macroDBVersion) EDIT_SECTIONC;
#endif // #ifdef USE_MACROEDIT

//------------------------------------------------------------//

///// main.c /////
//  ※※※ SECTION指定しない { ※※※
#ifdef MAIN_C
static Boolean MyApplicationDispatchEvent(EventType *event);
static Boolean MyApplicationHandleEvent  (EventType *event);
static UInt32  MyApplicationHandleNotifications(SysNotifyParamType *paramsP);
static UInt16  MyStartApplication(UInt16 cmd, void *cmdPBP, UInt16 launchFlgs);
static void    MyStopApplication(void);
#endif // #ifdef MAIN_C
UInt32  MyApplicationReceiveData   (void *cmdPBP, UInt16 launchFlags);
Boolean checkLaunchCode(UInt16 cmd, void *cmdPBP, UInt16 launchFlags);
//  ※※※ } SECTION指定しない ※※※

//------------------------------------------------------------//

///// msglist.c /////
#ifdef MSGLIST_C
static void copyThreadTitleString(Char  *dest, Char *src, UInt16 length,
                                  UInt16 state, UInt16 msgNumber) EDIT_SECTION7;
static Err AllocMsgTitleInfo(UInt16 nofItems) EDIT_SECTION7;
static Err create_TitleList(UInt16 bbsIndex, UInt16 areaCount,
                             Char  *buffer,   Int16  command) EDIT_SECTION7;

#endif // #ifdef MSGLIST_C
Boolean CheckDispList(UInt16 bbsIndex, NNshSubjectDatabase *subjP,
                                                        NNshBoardDatabase *bdP) EDIT_SECTION7;
void ClearMsgTitleInfo(void) EDIT_SECTION7;
Boolean Update_Thread_List(UInt16 bbsIdx, UInt16 selItem, UInt16 step) EDIT_SECTION7;
#ifdef USE_COLOR
void NNsi_SetTitleColor(UInt16 status) EDIT_SECTION7;
#endif
void NNsi_WinDrawList(UInt16 pointIndex, Char *titleIndex[], UInt16 listItems,
                      RectangleType *dimF, UInt16 height) EDIT_SECTION7;
void NNsi_WinUpdateList(UInt16 pointIndex, UInt16 prevIndex,
                        Char *titleIndex[], UInt16 listItems,
                        RectangleType *dimF, UInt16 height) EDIT_SECTION7;

//------------------------------------------------------------//

///// msgmgr.c /////
#ifdef MSGMGR_C
static Err createThreadIndexSub(Char   *buffer, UInt32 size, UInt32 offset,
                                UInt32 *nofMsg, UInt32 *msgOffset) EDIT_SECTION6;
static void checkLogChargeThread(Char *fileName, NNshGetLogDatabase *dbData, UInt32 *tokenId) EDIT_SECTION6;
static Err createThreadIndexSub_HTML(Char   *buffer,
                                      UInt32 *bufSize, UInt32  offset,
                                      UInt32 *nofMsg,  UInt32 *msgOffset, 
                                      Char *fileName,  UInt16 kanjiCode, 
                                      UInt16 *status,  UInt32 tokenId) EDIT_SECTION6;
static Err createThreadIndexSub_MachiBBS(Char  *buffer,    UInt32  size,
                                         UInt32  offset,    UInt32 *nofMsg, 
                                         UInt32 *msgOffset, UInt16 endTokenType, 
                                         Char   *start,     Char   *end) EDIT_SECTION6;
static void message_entryDatabase(UInt16 idxMES,UInt32 fileSize,UInt16 bbsType) EDIT_SECTION6;
static Err message_entryMessage(Char *targetFile, UInt32 *dataSize, UInt16 bbsType) EDIT_SECTION6;
static Err message_appendMessage(Char *targetFile, NNshSubjectDatabase *mesDB) EDIT_SECTION6;
#endif // #ifdef MSGMGR_C
Err Get_Subject_Database(UInt16 index, NNshSubjectDatabase *subjDB) EDIT_SECTION6;
Err MoveMessageToReadOnly(UInt16 index) EDIT_SECTION6;
Err CreateMessageThreadIndex(NNshMessageIndex *index, UInt32 *nofMsg,
                             UInt32 *msgOffset, UInt16 bbsType) EDIT_SECTION6;
Err getLastMessageNumShitaraba(UInt16 *resNum) EDIT_SECTION6;
Err update_subject_database(UInt16 index, NNshSubjectDatabase *subjDB) EDIT_SECTION6;
Err Get_AllMessage(Char *url, Char *boardNick, Char *file, UInt16 idxMES,
                   UInt16 bbsType) EDIT_SECTION6;
Err Get_PartMessage(Char *url, Char *boardNick, NNshSubjectDatabase *mesDB, 
                    UInt16 idxMES, Char *message) EDIT_SECTION6;
Err MoveSubject_BBS(Char *dest, Char *src) EDIT_SECTION6;
void MarkMessageIndex(UInt16 index, UInt16 favor, UInt16 ngWordCheck) EDIT_SECTION6;
Boolean GetThreadDataFromMesNum(Boolean *updateBBS, Char *buf, UInt16 bufLen, UInt16 *recNum) EDIT_SECTION6;
Boolean Handler_MessageInfo(EventType *event) EDIT_SECTION6;
Err DisplayMessageInformations(UInt16 msgIndex, UInt16 *msgAttribute) EDIT_SECTION6;
Boolean Get_MessageFromMesNum(Boolean *updateBBS, UInt16 *recNum, Boolean useClipboard) EDIT_SECTION6;
Err CreateThreadURL(UInt16 urlType, Char *url, UInt16 bufSize, 
                    NNshBoardDatabase *bbsInfo, NNshSubjectDatabase *mesInfo) EDIT_SECTION6;
void CreateThreadBrowserURL(UInt16 urlType, Char *url,
                            NNshBoardDatabase    *bbsInfo,
                            NNshSubjectDatabase  *mesInfo) EDIT_SECTION6;
Err MoveLogMessage(UInt16 index) EDIT_SECTION6;
#ifdef USE_SSL
Err ProceedOysterLogin(void) EDIT_SECTION6;
#endif  // #ifdef USE_SSL
Err ProceedBe2chLogin(void)  EDIT_SECTION6;
void ShowReserveGetLogList(void) EDIT_SECTION6;
UInt16  UpdateThreadResponseNumber(UInt16 dbIndex) EDIT_SECTION6;
Boolean DeleteThreadMessage(UInt16 msgIndex) EDIT_SECTION6;
Boolean DeleteThreadMessage_RecordID(UInt32 recId) EDIT_SECTION6;


//------------------------------------------------------------//

///// msgview.c /////
#ifdef MSGVIEW_C
static Boolean pickupAnchor   (NNshMessageIndex *idxP, Char *buffer, UInt16 size) EDIT_SECTIONB;
static Boolean pickupAnchorURL(NNshMessageIndex *idxP, Char *buffer, UInt16 size) EDIT_SECTIONB;
static Boolean pickupNGwordCandidate(NNshMessageIndex *idxP, Char *buffer, UInt16 size) EDIT_SECTIONB;
static void entryPickupedNGword(Char *ngWord) EDIT_SECTIONB;
static Boolean pickupBeProfile(NNshMessageIndex *idxP, Char *buffer, UInt16 size) EDIT_SECTIONB;
static void replaceHideWord(Char *buffer, Char *checkWord) EDIT_SECTIONB;
static void checkHideMessage(Char *buffer) EDIT_SECTIONB;
static void setFavoriteLabel(FormType *frm, UInt16 attribute) EDIT_SECTIONB;
static void outputMemoPad(Boolean useClipboard) EDIT_SECTIONB;
static void outputToFile(Char *fileName, UInt16 fileMode, Boolean isQuote, UInt16 startQuoteLine, Char *appendLine) EDIT_SECTIONB;
static void openWebBrowser(UInt16 confirmation, Char *buffer, UInt16 size) EDIT_SECTIONB;
static void jumpPrevious(void) EDIT_SECTIONB;
static void jumpEdge(Boolean loc) EDIT_SECTIONB;
static void parseMessage(Char *buf, Char *source, UInt32 size,
                         UInt16 *nofJmp, UInt16 *jmpBuf, UInt16 kanjiCode,
                         Boolean setJumpFlag) EDIT_SECTIONF;
static void changeFont(void) EDIT_SECTIONB;
static void displayMessageSub(UInt16 bbsType, Char *topP, UInt32 mesNum,
                              UInt32 offset,  UInt32 size, Char *buf,
                              Boolean setJumpFlag) EDIT_SECTIONB;
static void sortJumpMsgOrder(UInt16 *nofJump, UInt16 *msgList) EDIT_SECTIONB;
static Err displayMessage(NNshMessageIndex *idxP, UInt16 isBottom) EDIT_SECTIONB;
static Boolean moveMessageNext(Boolean chkScroll) EDIT_SECTIONB;
static Boolean moveMessagePrev(Boolean chkScroll, UInt16 dispLoc) EDIT_SECTIONB;
static Boolean jumpMessage(FormType *frm, NNshMessageIndex *idxP) EDIT_SECTIONB;
static Boolean sclRepEvt_DispMessage(EventType *event) EDIT_SECTIONB;
static Boolean selEvt_KeyDown_DispMessage(EventType *event) EDIT_SECTIONB;
static Err PartReceiveMessage_View(void) EDIT_SECTIONB;
static void NNsh_Search_Message(UInt16 type, Char *str, Boolean flag) EDIT_SECTIONB;
static Err NNsh_Get_MessageToBuf(NNshMessageIndex *idxP, UInt16 msg, UInt16 type, Boolean flag) EDIT_SECTIONB;
static void getViewDataString(Char *buffer, Char *headerStr, UInt16 bufSize) EDIT_SECTIONB;
static Boolean executeViewAction(UInt16 funcID) EDIT_SECTIONB;
static void popUP_Message(UInt16 resNum) EDIT_SECTIONB;
static void updateThreadLevel(void) EDIT_SECTIONB;

#endif  // #ifdef MSGVIEW_C
void ReadMessage_Close(UInt16 nextFormID) EDIT_SECTIONB;
Err OpenForm_NNshMessage(FormType *frm) EDIT_SECTIONB;
Err GetSubDirectoryName(UInt16 index, Char *dirName) EDIT_SECTIONB;
void UpdateFieldRegion(void) EDIT_SECTIONB;
Boolean ctlSelEvt_DispMessage(EventType *event) EDIT_SECTIONB;
Boolean menuEvt_DispMessage(EventType *event) EDIT_SECTIONB;
Boolean Handler_NNshMessage(EventType *event) EDIT_SECTIONB;

//------------------------------------------------------------//

///// msgwrite.c /////
#ifdef MSGWRITE_C
static void setBBSNameAndTime(UInt16 bbsType, Char *boardNick,
                              Char *buffer, UInt16 bufSize) EDIT_SECTION5;
static void setFormParameters(Char *nick, Char *buffer,   UInt16 bufSize,
                              Char *tempArea, UInt16 areaSize, Char *defSubmit) EDIT_SECTION5;
static void returnToMessageView(void) EDIT_SECTION5;
static Boolean menuEvt_WriteMessage(EventType *event) EDIT_SECTION5;
static void previewWritingForm_NNsh(void) EDIT_SECTION5;
static void checkCheckBoxUpdate(UInt16 chkId) EDIT_SECTION5;
static Boolean selEvt_Control_WriteMessage(EventType *event) EDIT_SECTION5;
static Boolean selEvt_KeyDown_WriteMessage(EventType *event) EDIT_SECTION5;
static Boolean sclRepEvt_WriteMessage(EventType *event) EDIT_SECTION5;
static Boolean fldChgEvt_WriteMessage(EventType *event) EDIT_SECTION5;
static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command, UInt16 bbsType, Err *ret) EDIT_SECTION5;
static Char *pickupCookie(Err *ret, Char *start, UInt32 bufSize) EDIT_SECTION5;
static void appendWriteCookie(Char *buffer, UInt32 bufSize, Char *data, UInt32 dataSize) EDIT_SECTION5;
static Err threadWrite_Message(void) EDIT_SECTION5;
static Err create_outputMessageFile(FormType *diagFrm, Char *buffer,
                                    UInt32 bufSize, Char *boardNick, 
                                    Char *key, UInt16 bbsType) EDIT_SECTION5;
static void outputTextField(FormType *frm, UInt16 fldID, Char *buf, UInt16 size, UInt16 bbsType, NNshFileRef *fileRef) EDIT_SECTION5;
static void openWriteConfigulation(void) EDIT_SECTION5;
#ifdef USE_NEWTHREAD_FEATURE
static Boolean sclRepEvt_newThread(EventType *event) EDIT_SECTION5;
static Err execute_newThread(FormType *frm) EDIT_SECTION5;
static Boolean selEvt_Control_newThread(EventType *event) EDIT_SECTION5;
static Boolean selEvt_KeyDown_newThread(EventType *event) EDIT_SECTION5;
#endif // #ifdef USE_NEWTHREAD_FEATURE
#endif // #ifdef MSGWRITE_C
Boolean Handler_WriteConfigulation(EventType *event) EDIT_SECTION5;
void SaveWritingMessage(void) EDIT_SECTION5;
Boolean Handler_WriteMessage(EventType *event) EDIT_SECTION5;
void OpenForm_WriteMessage(FormType *frm) EDIT_SECTION5;
void OpenForm_newThread(FormType *frm) EDIT_SECTION5;
Boolean Handler_newThread(EventType *event) EDIT_SECTION5;

//------------------------------------------------------------//

///// netent.c /////
#ifdef NETENT_C
    // static関数なし
#endif // #ifdef NETENT_C
Err NNshHttp_comm(UInt16 type, Char *url, Char *cookie, Char *appendData,
                  UInt32 range, UInt32 endRange, Char *message) EDIT_SECTION3;
void NNshNet_LineHangup(void) EDIT_SECTION3;
Err WebBrowserCommand(UInt32 creator, UInt16 subLaunch, UInt16 flags,
                      UInt16 cmd, Char *parameterP, UInt16 size, UInt32 *resultP) EDIT_SECTION3;
void OpenNetworkPreferences(void) EDIT_SECTION3;
#ifdef USE_BT_CONTROL
void OpenBluetoothPreferences(Boolean val) EDIT_SECTION3;
#endif // #ifdef USE_BT_CONTROL

//------------------------------------------------------------//

///// netmng.c /////
#ifdef NETMNG_C
    // static関数なし
#endif  // #ifdef NETMNG_C
Err NNshNet_open(UInt16 *netRef) EDIT_SECTION2;
Err NNshNet_close(UInt16 netRef) EDIT_SECTION2;
Err NNshNet_write(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data) EDIT_SECTION2;
Err NNshNet_read(UInt16 netRef, NetSocketRef socketRef, UInt16 size, void *data, UInt16 *readSize) EDIT_SECTION2;
Err NNshNet_disconnect(UInt16 netRef, NetSocketRef socketRef) EDIT_SECTION2;
Err NNshNet_connect(NetSocketRef *socketRef, UInt16 netRef, Char *hostName, UInt16 port) EDIT_SECTION2;
void NNshNet_CheckHostAlive(UInt16 netRef, Char *hostName,
                                               UInt16 count, Char *resultMsg) EDIT_SECTION2;
void NNshNet_GetNetIFInfo(UInt16 netRef, Char *buffer) EDIT_SECTION2;
void NNshNet_TraceBitSet(UInt16 netRef, UInt32 traceBits) EDIT_SECTION2;
#ifdef USE_SSL
Err NNshSSL_open(UInt16 sslMode, UInt16 *netRef,
                       UInt16 *sslRef, SslLib **template, SslContext **context) EDIT_SECTION2;
Err NNshSSL_close(UInt16 sslMode, UInt16 netRef,
                         UInt16 sslRef, SslLib *template, SslContext *context) EDIT_SECTION2;
Err NNshSSL_connect(UInt16 sslMode, UInt16 sslRef, SslContext *context,
                    NetSocketRef *socketRef, UInt16 netRef,
                    Char *hostName, UInt16 port) EDIT_SECTION2;
Err NNshSSL_disconnect(UInt16 sslMode, UInt16 sslRef, SslContext *context,
                       NetSocketRef socketRef, UInt16 netRef) EDIT_SECTION2;
Err NNshSSL_write(UInt16 sslMode, UInt16  netRef, NetSocketRef socketRef,
                  UInt16 sslRef, SslContext *context, UInt16 size, void *data) EDIT_SECTION2;
Err NNshSSL_read(UInt16 sslMode, UInt16  netRef, NetSocketRef socketRef,
                 UInt16 sslRef, SslContext *context,
                 UInt16 size, void *data, UInt16 *readSize) EDIT_SECTION2;
Err NNshSSL_flush(UInt16 sslMode, UInt16  sslRef, SslContext *context) EDIT_SECTION2;
Int32 NNshSSL_callbackFunc(SslCallback *callbackStruct, Int32 command,
                           Int32 flavor, void *info);
#endif  // #ifdef USE_SSL

//------------------------------------------------------------//

///// nnshcmn.c /////
#ifdef NNSHCMN_C
static UInt16 Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L) EDIT_SECTION4;
#ifdef USE_ZLIB
static UInt32 Get2chStatusCodeSize(Char* buf, UInt32 bufSize) EDIT_SECTION4;
static Err InflateZLib(NNshFileRef* fileRef, Char* inputBuf, UInt16 inputSize, Boolean firstBlock) EDIT_SECTION4;
#endif // #ifdef USE_ZLIB
#endif // #ifdef NNSHCMN_C
Boolean ConvertHanZen(UInt8 *dst, UInt8 *ptr) EDIT_SECTION4;
Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr) EDIT_SECTION4;
Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte) EDIT_SECTION4;
Boolean ConvertSJtoJIS(UInt8 *dst, UInt8 *ptr) EDIT_SECTION4;
Boolean ConvertSJtoEUC(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte, UInt16 *parseByte) EDIT_SECTION4;
Err SendToLocalMemopad(Char *title, Char *data) EDIT_SECTION4;
Err SendToExternalDevice(Char *title, Char *data, UInt32 creator) EDIT_SECTION4;
void StrCopyEUC(Char *dst, Char *src) EDIT_SECTIONC;
void StrCopySJ(Char *dst, Char *src) EDIT_SECTION4;
void StrCopyJIStoSJ(Char *dst, Char *src) EDIT_SECTION4;
void StrNCopyHanZen(Char *dest, Char *src, UInt16 length) EDIT_SECTION4;
Char *StrCatURLencode(Char *dst, Char *src) EDIT_SECTION4;
void ShowVersion_NNsh(void) EDIT_SECTION4;
UInt32 GetOSFreeMem(UInt32 *totalMemoryP, UInt32 *dynamicMemoryP) EDIT_SECTION4;
void ShowDeviceInfo_NNsh(void) EDIT_SECTION4;
Err SeparateWordList(Char *string, NNshWordList *wordList) EDIT_SECTION4;
void ReleaseWordList(NNshWordList *wordList) EDIT_SECTION4;
Boolean LaunchResource_NNsh(UInt32    type,    UInt32  creator,
                            DmResType resType, DmResID resID) EDIT_SECTION4;
Boolean CheckInstalledResource_NNsh(UInt32 type, UInt32 creator) EDIT_SECTION4;
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size) EDIT_SECTION4;
#ifdef USE_LOGCHARGE
void ParseMessage_UTF8(Char *buf, Char *source, UInt32 size,
                       UInt16 *nofJmp, UInt16 *jmpBuf, UInt16 kanjiCode,
                       Boolean setJumpFlag) EDIT_SECTION4;
#endif // #ifdef USE_LOGCHARGE
#ifdef USE_ZLIB
Boolean OpenZLib(void) EDIT_SECTION4;
void CloseZLib(void) EDIT_SECTION4;
Err ParseGZip(NNshFileRef* fileRef, Char* inputBuf, UInt16 inputSize, Boolean firstBlock) EDIT_SECTION4;
#endif  // #ifdef USE_ZLIB
void NNsh_BeepSound(SndSysBeepType soundType) EDIT_SECTION4;

//------------------------------------------------------------//

///// nnshdmmy.c /////
#ifdef NNSHDMMY_C
#endif  // #ifdef NNSHDMMY_C
Err OpenForm_NNsiDummy(FormType *frm) EDIT_SECTION4;
Boolean Handler_NNsiDummy(EventType *event) EDIT_SECTION4;

//------------------------------------------------------------//

///// nnshmain.c /////
#ifdef THRMNG_C
static void change_handera_rotate(void) EDIT_SECTIOND;
static void updateSelectionItem(Int16 direction) EDIT_SECTIOND;
static void change_list_font(void) EDIT_SECTIOND;
static void change_graph_mode(void) EDIT_SECTIOND;
static void setMultiPurposeSwitch(FormType *frm, UInt16 itemID, UInt16 btnID) EDIT_SECTIOND;
static Boolean updateMultiPurposeSwitch(UInt16 paramID, UInt16 btnID) EDIT_SECTIOND;
static void moveSelectionTab(UInt16 selBBS) EDIT_SECTIOND;
static UInt16 select_readonly_directory(UInt16 orgDir) EDIT_SECTIOND;
static Boolean readonly_dir_selection(void) EDIT_SECTIOND;
static Boolean open_menu(void) EDIT_SECTIOND;
static void NNshMain_Close(UInt16 nextFormID) EDIT_SECTIOND;
static Boolean copy_to_readOnly(void) EDIT_SECTIOND;
static Boolean move_log_directory(void) EDIT_SECTIOND;
static Boolean move_log_location(void) EDIT_SECTIOND;
static Boolean set_SearchTitleString(MemHandle *bufH, UInt16 status, UInt16 *mode) EDIT_SECTIOND;
static Boolean isValidRecordForSearchTitle(UInt16  selBBS, Char *bbsName,
                                           NNshSubjectDatabase  *mesInfo) EDIT_SECTIOND;
static Boolean search_NextBBS(UInt16 start, UInt16 *bbsId) EDIT_SECTIOND;
static Boolean search_NextTitle(Int16 direction) EDIT_SECTIOND;
static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR) EDIT_SECTIOND;
static Err get_message_Info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR, 
                            NNshBoardDatabase   *bbsInfo,  UInt16 *selBBS) EDIT_SECTIOND;
static Boolean display_message(void) EDIT_SECTIOND;
static UInt16 get_all_message(void) EDIT_SECTIOND;
static Boolean update_message(void) EDIT_SECTIOND;
static Boolean modify_title(void) EDIT_SECTIOND;
static Boolean launch_WebBrowser(UInt16 webDA) EDIT_SECTIOND;
static Boolean beam_URL(void) EDIT_SECTIOND;
static Err checkIfNewArrivalIsValid(UInt16  selBBS, Char *bbsName,
                                    NNshSubjectDatabase  *mesInfo) EDIT_SECTIOND;
static Boolean update_newarrival_message(UInt16 command) EDIT_SECTIOND;
static Boolean update_getall_message(void) EDIT_SECTIOND;
static Boolean newThread_message(void) EDIT_SECTIOND;
static Boolean get_subject_txt(void) EDIT_SECTIOND;
static Boolean popEvt_ThreadMain(EventType *event) EDIT_SECTIOND;
static Boolean rctPointEvt_ThreadMain(EventType *event,
                                      FormType *frm, RectangleType *rect) EDIT_SECTIOND;
static Boolean keyDownEvt_ThreadMain(EventType *event) EDIT_SECTIOND;
static Boolean ctlSelEvt_ThreadMain(EventType *event) EDIT_SECTIOND;
static Boolean ctlRepEvt_ThreadMain(EventType *event) EDIT_SECTIOND;
static Boolean menuEvt_ThreadMain(EventType *event) EDIT_SECTIOND;
#ifdef USE_LOGCHARGE
static void openURL(void) EDIT_SECTIOND;
static void changeGetLogUrlSite(Boolean defaultSettingMode) EDIT_SECTIOND;
#endif // #ifdef USE_LOGCHARGE
#endif  // #ifdef THRMNG_C
UInt16 Show_tab_info(Boolean disp) EDIT_SECTIOND;
Boolean ExecuteAction(UInt16 funcID) EDIT_SECTIOND;
Boolean delete_notreceived_message(Boolean update) EDIT_SECTIOND;
Boolean delete_message(void) EDIT_SECTIOND;
Boolean Handler_ThreadMain(EventType *event) EDIT_SECTIOND;
void OpenForm_ThreadMain(FormType *frm) EDIT_SECTIOND;
void NNsi_RedrawThreadList(void) EDIT_SECTIOND;

//------------------------------------------------------------//

///// nnshset.c /////
#ifdef NNSHSET_C
static Err effectNNsiSetting7(FormType *frm) EDIT_SECTIONE;
static Err effectNNsiSetting8(FormType *frm) EDIT_SECTIONE;
static Err effectNNsiSetting5(FormType *frm) EDIT_SECTIONE;
static Err effectNNsiSetting6(FormType *frm) EDIT_SECTIONE;
static Err effect_NNshSetting(FormType *frm) EDIT_SECTIONE;
static Err effectNNsiSetting2(FormType *frm) EDIT_SECTIONE;
static Err effectNNsiSetting3(FormType *frm) EDIT_SECTIONE;
static Err effectNNsiSetting9(FormType *frm) EDIT_SECTIONE;
static Err effectNNsiSetting4(FormType *frm) EDIT_SECTIONE;
static Err effectNNsiSettingA(FormType *frm) EDIT_SECTIONE;
static void clearFeatureList(void) EDIT_SECTIONE;
static void closeForm_NNsiSettings(UInt16 nextFormID) EDIT_SECTIONE;
static Err createFeatureListStrings(NNshWordList **lst, UInt16 strRscId) EDIT_SECTIONE;
static Boolean keyDownEvt_NNshSetting(EventType *event) EDIT_SECTIONE;
static Boolean SetTitleHardKey(Char *title, UInt16 data, UInt16 mask, NNshHardkeyControl *info) EDIT_SECTIONE;
static Boolean effectNGwordInput(FormType *frm) EDIT_SECTIONE;
static Boolean displayNGwordInfo(FormType *frm, NNshNGwordDatabase *dbData) EDIT_SECTIONE;
static Boolean selectOmitDialog(void) EDIT_SECTIONE;
static void closeForm_FavorTabSet(UInt16 nextForm) EDIT_SECTIONE;
static Err effect_FavorTabSet(FormType *frm, NNshCustomTab *custom) EDIT_SECTIONE;
#ifdef USE_COLOR
static Boolean setViewColorDetail(void) EDIT_SECTIONE;
#endif
#endif  // #ifdef NNSHSET_C
void SetControlValue(FormType *frm, UInt16 objID, UInt16 *value) EDIT_SECTIONE;
void UpdateParameter(FormType *frm, UInt16 objID, UInt16 *value) EDIT_SECTIONE;
Boolean Handler_NGwordInput(EventType *event) EDIT_SECTIONE;
Boolean EntryNGword3(UInt16 entryMode, UInt16 recNum, NNshNGwordDatabase *ngData) EDIT_SECTIONE;
Boolean SetNGword3(void) EDIT_SECTIONE;
Boolean SetRoundTripDelayTime(void) EDIT_SECTIONE;
Boolean Handler_NNshSetting(EventType *event) EDIT_SECTIONE;
Err OpenForm_NNsiSetting7(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_NNsiSetting8(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_NNsiSetting5(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_NNsiSetting6(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_NNshSetting(FormType *frm) EDIT_SECTIONE;
Err OpenForm_NNsiSetting2(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_NNsiSetting3(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_NNsiSetting9(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_NNsiSetting4(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_NNsiSettingA(FormType  *frm) EDIT_SECTIONE;
Err OpenForm_FavorTabSet(FormType *frm) EDIT_SECTIONE;
Boolean Handler_FavorTabSet(EventType *event) EDIT_SECTIONE;
#ifdef USE_COLOR
Boolean Handler_setViewColorDetail(EventType *event) EDIT_SECTIONE;
#endif // #ifdef USE_COLOR
#ifdef USE_LOGCHARGE
void GetLog_SetHardKeyFeature(void) EDIT_SECTIONE;
#endif //#ifdef USE_LOGCHARGE

//------------------------------------------------------------//

///// offline.c /////
#ifdef OFFLINE_C
static Err checkOfflineEntryAvailable(DmOpenRef dbRef, UInt16 *recNum,
                                      UInt16 nofItems, UInt16 *recList,
                                      Char *threadFileName, UInt16 msgState,
                                      UInt16 offset) EDIT_SECTION7;
static Err getOfflineThreadRecords(UInt16 *nofItems, UInt16 **offThread) EDIT_SECTION7;
static Char *getThreadTitle(Char *msgP, UInt32 *size) EDIT_SECTION7;
static void setVFSOfflineThreadInfo(DmOpenRef dbRef, DmOpenRef dirDBRef,
                                    UInt16 nofItems, UInt16 *recList,
                                    UInt16 depth, UInt16 index, Char *dirName,
                                    NNshSubjectDatabase *subDb, UInt16 *cnt) EDIT_SECTION7;
#endif  // #ifdef OFFLINE_C
Err create_offline_database(void) EDIT_SECTION7;
Err SetThreadInformation(Char *fileName, UInt16 fileMode, UInt16 *kanjiCode,
                                                 NNshSubjectDatabase *subDB) EDIT_SECTION7;
Err setOfflineLogDir(Char *fileName) EDIT_SECTION7;

//------------------------------------------------------------//

///// skeleton.c /////
//  ※※※ SECTION指定しない { ※※※
UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags);
//  } ※※※ SECTION指定しない ※※※

//------------------------------------------------------------//

///// sslv2.c /////
#ifdef USE_SSL
#ifdef USE_SSL_V2
#ifdef SSLV2_C
static void client_hello_NNshSSLv2(NNshSSLv2Ref *sslRef) EDIT_SECTION3;
static Err get_server_hello_NNshSSLv2(NNshSSLv2Ref *sslRef) EDIT_SECTION3;
static Err client_master_key_NNshSSLv2(NNshSSLv2Ref *sslRef) EDIT_SECTION3;
static Err create_encryption_key_NNshSSLv2(NNshSSLv2Ref *sslRef) EDIT_SECTION3;
static Err client_finished_NNshSSLv2(NNshSSLv2Ref *sslRef) EDIT_SECTION3;
static Err get_server_verify_NNshSSLv2(NNshSSLv2Ref *sslRef) EDIT_SECTION3;
static Err get_server_finished_NNshSSLv2(NNshSSLv2Ref *sslRef) EDIT_SECTION3;
static void client_encode_NNshSSLv2(NNshSSLv2Ref *sslRef, UInt16 dataLen, Char *data) EDIT_SECTION3;
static void client_decode_NNshSSLv2(NNshSSLv2Ref *sslRef, UInt16 size, UInt8 *from, UInt8 *to) EDIT_SECTION3;
#endif // #ifdef SSLV2_C
Err NNshSSLv2_Open(NNshSSLv2Ref **sslRef) EDIT_SECTION3;
Err NNshSSLv2_Close(NNshSSLv2Ref **sslRef) EDIT_SECTION3;
Err NNshSSLv2_Connect(NNshSSLv2Ref *sslRef, NetSocketRef socketRef, UInt16 netRef) EDIT_SECTION3;
Err NNshSSLv2_Disconnect(NNshSSLv2Ref *sslRef) EDIT_SECTION3;
Err NNshSSLv2_Read(NNshSSLv2Ref *sslRef, UInt16 size, void *data, UInt16 *readSize) EDIT_SECTION3;
Err NNshSSLv2_Write(NNshSSLv2Ref *sslRef, UInt16 size, void *data) EDIT_SECTION3;
#endif // #ifdef USE_SSL_V2
#endif // #ifdef USE_SSL

//------------------------------------------------------------//

///// sslv2md5.c /////
#ifdef USE_SSL
#ifdef USE_SSL_V2
#ifdef SSLV2MD5_C
static void MD5Transform (UInt32 state[4], UInt8 block[64]) EDIT_SECTION3;
static void MD5sub_Encode(UInt8 *output, UInt32 *input, UInt16 len) EDIT_SECTION3;
static void MD5sub_Decode(UInt32 *output, UInt8 *input, UInt16 len) EDIT_SECTION3;
static void MD5_memcpy(UInt8 *output, UInt8 *input, UInt16 len) EDIT_SECTION3;
#endif // #ifdef SSLV2MD5_C
void CalcMD5(Char *string, UInt32 stringLen, Char *buffer, UInt32 *bufLen, NNshSSLv2Ref *sslRef) EDIT_SECTION3;
void MD5Init (MD5_CTX *context) EDIT_SECTION3;
void MD5Update(MD5_CTX *context, UInt8 *input, UInt16 inputLen) EDIT_SECTION3;
void MD5Final(UInt8 digest[16], MD5_CTX *context, UInt8 *PADDING) EDIT_SECTION3;
#endif // #ifdef USE_SSL_V2
#endif // #ifdef USE_SSL

//------------------------------------------------------------//

///// sslv2rc4.c /////
#ifdef USE_SSL
#ifdef USE_SSL_V2
#ifdef SSLV2RC4_C
    // static関数なし
#endif // #ifdef SSLV2RC4_C
void RC4_SetKey(RC4_KEY *key, UInt16 len, UInt8 *data) EDIT_SECTION3;
void RC4_Calculate(RC4_KEY *key, UInt32 len, Char *indata, Char *outdata) EDIT_SECTION3;
#endif // #ifdef USE_SSL_V2
#endif // #ifdef USE_SSL

//------------------------------------------------------------//

///// sslv2rsa.c /////
#ifdef USE_SSL
#ifdef USE_SSL_V2
#ifdef SSLV2RSA_C
    // static関数なし
#endif // #ifdef SSLV2RSA_C
UInt16 NNshPublicEncryptRsa(UInt16 flen, UInt8 *from, UInt8 *to, NNshSSLv2Ref *sslRef) EDIT_SECTION3;
#endif // #ifdef USE_SSL_V2
#endif // #ifdef USE_SSL

//------------------------------------------------------------//

///// subjmgr.c /////
#ifdef SUBJMGR_C
static void entryNotExistSubject(DmOpenRef dbRef, NNshSubjectDatabase *subjDb) EDIT_SECTION6;
static Err parse_SubjectList(Char *buffer, UInt32 readSize, UInt32 *parseSize,
                              UInt16 bbsType, Char *boardNick, UInt16 *cnt) EDIT_SECTION6;
#endif // #ifdef SUBJMGR_C
Err DeleteSubjectList(Char *boardNick, UInt16 delState, UInt16 *thrCnt) EDIT_SECTION6;
void SortSubjectList(void) EDIT_SECTION6;
UInt16 convertListIndexToMsgIndex(UInt16 listIdx) EDIT_SECTION6;
Err NNsh_GetSubjectList(UInt16 index) EDIT_SECTION6;
Err Check_same_thread(UInt16 selMES, NNshSubjectDatabase *data, 
                      NNshSubjectDatabase *matchedData, UInt16 *idx) EDIT_SECTION6;
Err GetSubjectIndex(Char *boardNick, Char *threadFileName, UInt16 *index) EDIT_SECTION6;
Boolean CheckIfCustomTabIsValid(Char *bbsName, NNshSubjectDatabase *subjP,
                                NNshCustomTab *customTab) EDIT_SECTION6;

//------------------------------------------------------------//

///// windows.c /////
#ifdef WINDOWS_C
static Boolean NNsi_HRFieldPageScroll(UInt16 fldID, UInt16 sclID,
                                      UInt16 lines, WinDirectionType direction) EDIT_SECTION;
#endif // #ifdef WINDOWS_C
void NNsi_EraseRectangle(RectangleType *dimF) EDIT_SECTION;
void NNsi_UpdateRectangle(RectangleType *dimF, UInt16 fontType, UInt16 *fontID,
                         UInt16 *fontHeight, UInt16 *nlines) EDIT_SECTION;
void NNsi_FrmDrawForm(FormType *frm, Boolean redraw) EDIT_SECTION;
void NNsi_HRSclUpdate(ScrollBarType *barP) EDIT_SECTION;
#ifdef USE_COLOR
Boolean NNsiWinDrawColorLine(Char *str, UInt16 lineLen, Coord x, Coord y) EDIT_SECTION;
#endif // #ifdef USE_COLOR
void NNsi_HRFldDrawField(FieldType *fldP, UInt16 isBottom) EDIT_SECTION;
void NNshWinClearList(FormType *frm, UInt16 lstID) EDIT_SECTION;
void NNshWinSetListItems(FormType *frm, UInt16 lstID, Char *list,
                         UInt16 cnt, UInt16 selItem, 
                         MemHandle *memH, Char **listP) EDIT_SECTION;
void NNshWinUpdateListItems(FormType *frm, UInt16 lstID, Char *ptr,
                            UInt16 cnt, UInt16 selItem) EDIT_SECTION;
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Boolean redraw, 
                         Char *msg, UInt32 size) EDIT_SECTION;
void NNshWinSetPopItemsWithList(FormType *frm, UInt16 popId, UInt16 lstId, 
                                NNshWordList *wordList, UInt16 item) EDIT_SECTION;
void NNshWinSetPopItemsWithListArray(FormType *frm, NNshListItem *target,
                                     Char *listArray, UInt16 num, UInt16 *items) EDIT_SECTION;
void NNshWinSetPopItems(FormType *frm, UInt16 popId, UInt16 lstId, UInt16 item) EDIT_SECTION;
void NNshWinGetFieldText(FormType *frm, UInt16 fldID, Char *area, UInt32 len) EDIT_SECTION;
void SetMsg_BusyForm(Char *msg) EDIT_SECTIONC;
void Show_BusyForm(Char *msg)   EDIT_SECTION9;
void Hide_BusyForm(Boolean redraw)EDIT_SECTIONB;
UInt16 NNsh_DialogMessage(UInt16 level, UInt16 altID, Char *mes1, Char *mes2, UInt32 num) EDIT_SECTION;
void NNshWinViewUpdateScrollBar(UInt16 fldID, UInt16 sclID) EDIT_SECTION;
Boolean NNshWinViewPageScroll(UInt16 fldID, UInt16 sclID, 
                              UInt16 lines, WinDirectionType direction) EDIT_SECTION;
Boolean NNsh_MenuEvt_Edit(EventType *event) EDIT_SECTION;
Boolean Handler_DialogAction(EventType *event) EDIT_SECTION;
Boolean Handler_JumpSelection(EventType *event) EDIT_SECTION;
UInt16 NNshWinSelectionWindow(UInt16 frmId, Char *listItemP, UInt16 nofItems, UInt16 selection) EDIT_SECTION;
Boolean DataInputDialog(Char *title, Char *area, UInt16 size, UInt16 usage, void *valueP) EDIT_SECTION;
Boolean Handler_IDpwAction(EventType *event) EDIT_SECTION;
Boolean DataInputPassword(Char *title, Char *id, UInt16 idSize, Char *pw, UInt16 pwSize, Char *info) EDIT_SECTION;
Boolean Handler_ViewLineAction(EventType *event) EDIT_SECTION;
Boolean DataViewForm(Char *title, UInt16 nofLine, Char **dataLine) EDIT_SECTION;
void NNsi_putLine(FormType *frm, UInt16 command) EDIT_SECTION;
void NNsh_UpDownButtonControl(FormType *frm, UInt16 btnState, UInt16 upBtnID, UInt16 downBtnID) EDIT_SECTION;
void PrepareDialog_NNsh(UInt16 formID, FormType **diagFrm, Boolean silk) EDIT_SECTION;
void PrologueDialog_NNsh(FormType *prevFrm, FormType *diagFrm, Boolean silk) EDIT_SECTION;
void OutputHexStringDebug(UInt16 level, Char *msg, Char *data, UInt16 len) EDIT_SECTION8;

