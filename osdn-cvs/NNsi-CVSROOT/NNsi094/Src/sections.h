/*============================================================================*
 *  FILE: 
 *     sections.h
 *
 *  Description: 
 *     Multi-segment definitions.
 *
 *===========================================================================*/
#include <PalmOS.h>
#include <PalmCompatibility.h>

#define EDIT_SECTION   __attribute__ ((section ("editfns")))
#define EDIT_SECTION2  __attribute__ ((section ("editfns2")))
#define EDIT_SECTION3  __attribute__ ((section ("editfns3")))
#define EDIT_SECTION4  __attribute__ ((section ("editfns4")))

Err NNshHttp_commMain(UInt16 type, Char *url, Char *cookie, Char *appendData,
                      UInt32 range, UInt32 endRange, UInt16 port, Char *proxy) EDIT_SECTION;
Boolean ctlSelEvt_DispMessage(EventType *event) EDIT_SECTION;
Boolean menuEvt_DispMessage(EventType *event) EDIT_SECTION;
Boolean Handler_NNshMessage(EventType *event) EDIT_SECTION;
Err OpenForm_NNshMessage(FormType *frm) EDIT_SECTION;
Err GetSubDirectoryName(UInt16 index, Char *dirName) EDIT_SECTION;

Err Get_Subject_Database(UInt16 index, NNshSubjectDatabase *subjDB) EDIT_SECTION;
Err MoveMessageToReadOnly(UInt16 index) EDIT_SECTION;
Err CreateMessageThreadIndex(NNshMessageIndex *index, UInt32 *nofMsg,
                             UInt32 *msgOffset, UInt16 bbsType) EDIT_SECTION;
Err update_subject_database(UInt16 index, NNshSubjectDatabase *subjDB) EDIT_SECTION;
Err Get_AllMessage(Char *url, Char *boardNick, Char *file, UInt16 idxMES,
                   UInt16 bbsType) EDIT_SECTION;
Err Get_PartMessage(Char *url, Char *boardNick, NNshSubjectDatabase *mesDB, 
                    UInt16 idxMES) EDIT_SECTION;
Err MoveSubject_BBS(Char *dest, Char *src) EDIT_SECTION;
void MarkMessageIndex(UInt16 index, UInt16 favor) EDIT_SECTION;
     
Err OpenFile_NNsh(Char *fileName, UInt16 fileMode, NNshFileRef *fileRef) EDIT_SECTION;
Err GetFileSize_NNsh(NNshFileRef *fileRef, UInt32 *fileSize) EDIT_SECTION;
Err ReadFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size, 
                  void *ptr, UInt32 *readSize) EDIT_SECTION;
Err CloseFile_NNsh(NNshFileRef *fileRef) EDIT_SECTION;

void NNsi_FrmDrawForm(FormType *frm) EDIT_SECTION;
void NNshWinViewUpdateScrollBar(UInt16 fldID, UInt16 sclID) EDIT_SECTION;
Boolean NNshWinViewPageScroll(UInt16 fldID, UInt16 sclID, 
                              UInt16 lines, WinDirectionType direction) EDIT_SECTION;

void NNshNet_LineHangup(void) EDIT_SECTION;

Err Get_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo) EDIT_SECTION;
Err check_same_thread(UInt16 selMES, NNshSubjectDatabase *data, 
                      NNshSubjectDatabase *matchedData, UInt16 *idx) EDIT_SECTION;

Err Get_BBS_Database_NNsh(Char *nick, NNshBoardDatabase *bbs, UInt16 *idx) EDIT_SECTION;
Err Get_BBS_Database(Char *boardNick, NNshBoardDatabase *bbs, UInt16 *index) EDIT_SECTION;
UInt16 convertListIndexToMsgIndex(UInt16 listIdx) EDIT_SECTION;
Err GetPartMessage_NNsh(Char *url, Char *boardNick,
                        NNshSubjectDatabase *mesDB, UInt16 idxMES) EDIT_SECTION;
Err Count_Thread_BBS(Char *boardNick, UInt16 *count) EDIT_SECTION;
Err DeleteSubjectList(Char *boardNick, UInt16 delState, UInt16 *thrCnt) EDIT_SECTION;
UInt16 convertListIndexToMsgIndex(UInt16 listIdx) EDIT_SECTION;
Err NNsh_GetSubjectList(UInt16 index) EDIT_SECTION;
Err Create_SubjectList(UInt16 index, UInt16 threadCnt, 
                       Char *buffer, UInt32 bufSize, UInt16 *cnt) EDIT_SECTION;
Err Get_Thread_Count(UInt16 index, UInt16 *count) EDIT_SECTION;
Err SetThreadInformation_NNsh(Char *fileName, UInt16 fileMode,
                                                 NNshSubjectDatabase *subDB) EDIT_SECTION;
Err Update_BBS_Database(UInt16 index, NNshBoardDatabase *bbs) EDIT_SECTION;
Err Update_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo) EDIT_SECTION;
Err create_offline_database(void) EDIT_SECTION;

Err setOfflineLogDir(Char *fileName) EDIT_SECTION;

#ifdef THRMNG_C
static Boolean copy_to_readOnly(void) EDIT_SECTION2;
static Boolean get_MessageFromMesNum(void) EDIT_SECTION2;
static Boolean set_SearchTitleString(void) EDIT_SECTION2;
static Boolean search_NextTitle(Int16 direction) EDIT_SECTION2;
static Err create_BBS_INDEX(Char **bdLst, UInt16 *cnt) EDIT_SECTION2;
static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR) EDIT_SECTION2;
static Err get_message_Info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR, 
                            NNshBoardDatabase   *bbsInfo,  UInt16 *selBBS) EDIT_SECTION2;
static Boolean display_message(void) EDIT_SECTION2;
static Boolean get_all_message(void) EDIT_SECTION2;
static Boolean update_message(void) EDIT_SECTION2;
static Boolean update_newarrival_message(void) EDIT_SECTION2;
static Boolean update_getall_message(void) EDIT_SECTION2;
static Boolean redraw_Thread_List(UInt16 bbsIndex, UInt16 threadIndex) EDIT_SECTION2;
static Boolean get_subject_txt(void) EDIT_SECTION2;
static Boolean popEvt_ThreadMain(EventType *event) EDIT_SECTION2;
static Boolean keyDownEvt_ThreadMain(EventType *event) EDIT_SECTION2;
static Boolean ctlSelEvt_ThreadMain(EventType *event) EDIT_SECTION2;
static Boolean lstSelEvt_ThreadMain(EventType *event) EDIT_SECTION2;
static Boolean menuEvt_ThreadMain(EventType *event) EDIT_SECTION2;
#endif

#ifdef OFFLINE_C
static Char *getThreadTitle(Char *msgP, UInt32 *size) EDIT_SECTION2;
static void setVFSOfflineThreadInfo(DmOpenRef dbRef, DmOpenRef dirDBRef,
                                    UInt16 depth, UInt16 index, Char *dirName,
                                    NNshSubjectDatabase *subDb, UInt16 *cnt) EDIT_SECTION2;
#endif
#ifdef BBSMNG_C
static UInt16 getBoardURL(Char *url, Char *nick, Char *top, Char *bottom) EDIT_SECTION2;
static void parseBoard(DmOpenRef dbRef, Char *top, UInt32 *readSize) EDIT_SECTION2;
static void updateBoardList(void) EDIT_SECTION2;
static void closeForm_SelectBBS(void) EDIT_SECTION2;
#endif

#ifdef DBMGR_C
static Int16 aplCompareF_Char(void *rec1, void *rec2, Int16 other,
                              SortRecordInfoPtr       rec1SortInfo,
                              SortRecordInfoPtr       rec2SortInfo, 
                              MemHandle               appInfoH) EDIT_SECTION2;
#endif
#ifdef MSGWRITE_C
static Boolean menuEvt_WriteMessage(EventType *event) EDIT_SECTION2;
static void checkCheckBoxUpdate(UInt16 chkId) EDIT_SECTION2;
static Boolean selEvt_Control_WriteMessage(EventType *event) EDIT_SECTION2;
static Boolean selEvt_KeyDown_WriteMessage(EventType *event) EDIT_SECTION2;
static Boolean sclRepEvt_WriteMessage(EventType *event) EDIT_SECTION2;
static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command) EDIT_SECTION2;
static Err threadWrite_Message(void) EDIT_SECTION2;
static Err create_outputMessageFile(FormType *diagFrm, Char *buffer,
                                    UInt32 bufSize, Char *boardNick, 
                                    Char *key, UInt16 bbsType) EDIT_SECTION2;
#endif
#ifdef MSGMGR_C
static Err createThreadIndexSub_MachiBBS(Char   *buffer, UInt32 size,
                           UInt32 offset, UInt32 *nofMsg, UInt32 *msgOffset) EDIT_SECTION2;
static void message_entryDatabase(UInt16 idxMES,UInt32 fileSize,UInt16 bbsType) EDIT_SECTION2;
static Err message_entryMessage(Char *targetFile, UInt32 *dataSize) EDIT_SECTION2;
static Err message_appendMessage(Char *targetFile, NNshSubjectDatabase *mesDB) EDIT_SECTION2;
#endif
#ifdef MSGVIEW_C
static void jumpPrevious(void) EDIT_SECTION3;
static void jumpEdge(Boolean loc) EDIT_SECTION3;
static void parseMessage(Char *buf, Char *source, UInt32 size, UInt16 *nofJmp, UInt16 *jmpBuf) EDIT_SECTION3;
static void changeFont(void) EDIT_SECTION3;
static void ReadMessage_Close(UInt16 nextFormID) EDIT_SECTION3;
static void displayMessageSub(UInt16 bbsType, Char *topP, UInt32 mesNum,
                              UInt32 offset, UInt32 size) EDIT_SECTION3;
static Err displayMessage(NNshMessageIndex *idxP) EDIT_SECTION3;
static Boolean moveMessageNext(Boolean chkScroll) EDIT_SECTION3;
static Boolean moveMessagePrev(Boolean chkScroll) EDIT_SECTION3;
static Boolean sclRepEvt_DispMessage(EventType *event) EDIT_SECTION3;
static Boolean selEvt_KeyDown_DispMessage(EventType *event) EDIT_SECTION3;
static Err PartReceiveMessage_View(void) EDIT_SECTION3;
#endif

// BUSY FORMs
void SetMsg_BusyForm(Char *msg) EDIT_SECTION4;
void Show_BusyForm  (Char *msg) EDIT_SECTION4;
void Hide_BusyForm  (void)      EDIT_SECTION4;

// DIALOGs
void   NNsh_DebugMessage  (UInt16 altID, Char *mes1, Char *mes2, UInt32 num) EDIT_SECTION4;
UInt16 NNsh_ErrorMessage  (UInt16 altID, Char *mes1, Char *mes2, UInt32 num) EDIT_SECTION4;
UInt16 NNsh_ConfirmMessage(UInt16 altID, Char *mes1, Char *mes2, UInt32 num) EDIT_SECTION4;
Boolean NNsh_MenuEvt_Edit(EventType *event) EDIT_SECTION4;
Boolean Handler_JumpSelection(EventType *event) EDIT_SECTION4;
UInt16 NNshWinSelectionWindow(Char *listItemP, UInt16 nofItems) EDIT_SECTION4;

// Backup/Restore Databases
Err BackupDatabaseToVFS_NNsh   (Char *dbName) EDIT_SECTION4;
Err RestoreDatabaseFromVFS_NNsh(Char *dbName) EDIT_SECTION4;
void BackupDBtoVFS(UInt16 backupAllDB) EDIT_SECTION4;

#ifdef MAIN_C
static void restoreDatabaseToVFS(void) EDIT_SECTION4;
#endif

// fork Web Browser
#ifdef USE_ARMLET
Err WebBrowserCommand(UInt32 creator, Boolean subLaunch, 
                      UInt16 flags, UInt16 cmd, Char *parameterP,
                      UInt32 *resultP) EDIT_SECTION4;
#endif
