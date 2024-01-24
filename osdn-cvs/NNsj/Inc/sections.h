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
#define EDIT_SECTION1   __attribute__ ((section ("editfns1")))


void SetMsg_BusyForm    (Char *msg) EDIT_SECTION1;
void Show_BusyForm      (Char *msg) EDIT_SECTION1;
void Hide_BusyForm      (Boolean a) EDIT_SECTION1;

#ifdef DBMGR_C
static Int16 aplCompareF_Char(void *rec1, void *rec2, Int16 other,
                              SortRecordInfoPtr       rec1SortInfo,
                              SortRecordInfoPtr       rec2SortInfo, 
                              MemHandle               appInfoH) EDIT_SECTION;
static Int16 aplCompareF_UInt32(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH) EDIT_SECTION;
static Int16 aplCompareF_UInt16(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH) EDIT_SECTION;
static Err searchRecordSub(DmOpenRef dbRef, void *target, UInt16 keyType, 
                           UInt16   offset, UInt16 *idx,  MemHandle *hnd) EDIT_SECTION;
#endif
Err OpenDB_wCreator(Char *dbName, UInt16 chkVer, UInt32 creator, UInt32 type, Boolean erase, DmOpenRef *dbRef) EDIT_SECTION;
void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef) EDIT_SECTION;
void CloseDatabase_NNsh(DmOpenRef dbRef) EDIT_SECTION;
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset) EDIT_SECTION;
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset) EDIT_SECTION;
Err EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData) EDIT_SECTION;
Err IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index) EDIT_SECTION;
Err SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,
                      UInt16 offset, UInt16 size, void *matchedData,
                      UInt16 *index) EDIT_SECTION;
Err DeleteRecord_NNsh(DmOpenRef dbRef, void  *target,
                      UInt16 keyType,  UInt16 offset) EDIT_SECTION;
Err DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index) EDIT_SECTION;
Err GetRecord_NNsh(DmOpenRef dbRef, UInt16 index,
                   UInt16 size, void *matchedData) EDIT_SECTION;
Err UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, 
                      UInt16 size, void *updateData) EDIT_SECTION;
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count) EDIT_SECTION;

Err CloseFile_NNsh(NNshFileRef *fileRef) EDIT_SECTION;
Err OpenFile_NNsh(Char *fileName, UInt16 fileMode, NNshFileRef *fileRef) EDIT_SECTION;
Err GetFileSize_NNsh(NNshFileRef *fileRef, UInt32 *fileSize) EDIT_SECTION;
Err ReadFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size, 
                  void *ptr, UInt32 *readSize) EDIT_SECTION;
#ifdef FILEMGR_C
static void SetFullPathFileName_NNsh(Char *file, Char *fullPath, UInt16 size) EDIT_SECTION;
static Err openFile_VFSFile(Char *fileName, UInt16 fileMode, NNshFileRef *ref) EDIT_SECTION;
static Err openFile_FileStream(Char *name, UInt16 fileMode, NNshFileRef *ref) EDIT_SECTION;
#endif

Err DeleteFile_NNsh(Char *fileName, UInt16 location) EDIT_SECTION;
Err CreateFile_NNsh(Char *fileName) EDIT_SECTION;
Err RenameFile_NNsh(Char *oldFileName, Char *newFileName) EDIT_SECTION;
Err CreateDir_NNsh(Char *dirName) EDIT_SECTION;
Err CopyFile_NNsh(Char *destFile, Char *sourceFile, UInt16 location) EDIT_SECTION;
Err WriteFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size,
		   void *ptr, UInt32 *writeSize) EDIT_SECTION;
Err AppendFile_NNsh(NNshFileRef *fileRef, UInt32 size, 
                    void *ptr, UInt32 *writeSize) EDIT_SECTION;
Err AppendFileAsURLEncode_NNsh(NNshFileRef *fileRef, UInt32  size,
                               void        *ptr,     UInt32 *writeSize) EDIT_SECTION;
Err BackupDatabaseToVFS_NNsh(Char *dbName) EDIT_SECTION;
Err RestoreDatabaseFromVFS_NNsh(Char *dbName) EDIT_SECTION;

void SetControlValue(FormType *frm, UInt16 objID, UInt16 *value) EDIT_SECTION;
void UpdateParameter(FormType *frm, UInt16 objID, UInt16 *value) EDIT_SECTION;

void NNshWinSetPopItems(FormType *frm, UInt16 popId, UInt16 lstId, UInt16 item) EDIT_SECTION;


Err ReadMacroDBData(Char *dbName, UInt16 ver, UInt32 creator, UInt32 type, UInt16 *cnt,
                                                 UInt16 size, Char *dataPtr) EDIT_SECTION1;

Err WriteMacroDBData(Char *dbName, UInt16 ver, UInt32 creator, UInt32 type,
                                                   UInt16 cnt, Char *dataPtr) EDIT_SECTION1;
Char *AllocateMacroDBData(UInt16 len)   EDIT_SECTION1;
void ReleaseMacroDBData(Char *dataPtr) EDIT_SECTION1;
void PrintMacroLine(Char *buf, UInt16 size, UInt16 num, NNshMacroRecord *data) EDIT_SECTION;
MemPtr MemPtrNew_NNsh(UInt32 size) EDIT_SECTION;

Boolean NNsh_RctPointEvt(EventType *event, RectangleType *dimF) EDIT_SECTION;
Boolean NNsh_MainCtlEvt(EventType *event) EDIT_SECTION;
Boolean NNsh_MainCtlRepEvt(EventType *event) EDIT_SECTION;
Boolean NNsh_MainMenuEvt(EventType *event) EDIT_SECTION;
Boolean Handler_MainForm(EventType *event) EDIT_SECTION;
void NNsi_putLine(FormType *frm, UInt16 gadId, UInt16 upBtnID, UInt16 downBtnID, UInt16 command)
 EDIT_SECTION1;
void NNsh_UpDownButtonControl(FormType *frm, UInt16 btnState, UInt16 upBtnID, UInt16 downBtnID) EDIT_SECTION1;

Err SendToLocalMemopad(Char *title, Char *data) EDIT_SECTION1;
void outputMemoPad(void) EDIT_SECTION1;

#ifdef USE_HIGHDENSITY
void SetDoubleDensitySupport_NNsh(void) EDIT_SECTION;
#endif
void NNsi_ConvertLocation(Coord *screenX, Coord *screenY) EDIT_SECTION1;
void NNsi_UpdateRectangle(RectangleType *dimF, UInt16 fontType, UInt16 *fontID,
                         UInt16 *fontHeight, UInt16 *nlines) EDIT_SECTION1;
void NNsi_EraseRectangle(RectangleType *dimF) EDIT_SECTION;

void effectOpCodeData(FormType *frm, NNshMacroRecord *dataRec) EDIT_SECTION1;
Boolean Handler_opCodeAction(EventType *event) EDIT_SECTION1;
Boolean OpCodeInputDialog(UInt16 lineNum, NNshMacroRecord *dataRec) EDIT_SECTION1;
void prepareOpCodeDialog(FormType *frm, NNshMacroRecord *dataRec, UInt16 opCode) EDIT_SECTION1;

void NNshWinSetPopItemsWithList(FormType *frm, UInt16 popId, UInt16 lstId, 
                                NNshWordList *wordList, UInt16 item) EDIT_SECTION1;
Boolean  extractMacroData(void) EDIT_SECTION1;

Boolean NNsh_FileOperation(UInt16 command) EDIT_SECTION;
void PrologueDialog_NNsh(FormType *prevFrm, FormType *diagFrm) EDIT_SECTION;
void PrepareDialog_NNsh(UInt16 formID, FormType **diagFrm, Boolean silk) EDIT_SECTION;
