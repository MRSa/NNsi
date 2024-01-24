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

void SetMsg_BusyForm    (Char *msg) EDIT_SECTION;
void Show_BusyForm      (Char *msg) EDIT_SECTION;
void Hide_BusyForm      (Boolean a) EDIT_SECTION;

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

void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef) EDIT_SECTION;
void CloseDatabase_NNsh(DmOpenRef dbRef) EDIT_SECTION;
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset) EDIT_SECTION;
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset) EDIT_SECTION;
Err EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData) EDIT_SECTION;
Err IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index) EDIT_SECTION;
Err IsExistRecordRR_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
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
Err GetRecordID_NNsh(DmOpenRef dbRef, UInt16 index, UInt32 *recId) EDIT_SECTION;
Err GetRecordIndexFromRecordID_NNsh(DmOpenRef dbRef, UInt32 recId, UInt16 *index) EDIT_SECTION;

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

Boolean NNsh_FileData(UInt16 command) EDIT_SECTION;
Boolean DataSaveToFile(FormType *frm, Char *data, Char *fileName, UInt16 loc, UInt16 kanji, UInt16 newLine) EDIT_SECTION;

MemPtr MemPtrNew_NNsh(UInt32 size)  EDIT_SECTION;
Boolean checkFieldEnterAvailable(UInt16 fldId, EventType *event) EDIT_SECTION;
#ifdef USE_PIN_DIA
void SetDIASupport_NNsh(void) EDIT_SECTION;
void ResetDIASupport_NNsh(void) EDIT_SECTION;
Err  VgaFormModify_DIA(FormType *frmP, VgaFormModifyType_DIA type) EDIT_SECTION;
void VgaFormRotate_DIA(void) EDIT_SECTION;
#endif

UInt16 Jisx0208ToUnicode11(DmOpenRef dbRef, UInt8 jisH, UInt8 jisL)  EDIT_SECTION;
UInt16 Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L) EDIT_SECTION;
Err    OpenUtf8Database(DmOpenRef *jis2unicodeTbl, DmOpenRef *unicode2jisTbl) EDIT_SECTION;
void  CloseUtf8Database(DmOpenRef tbl1, DmOpenRef tbl2) EDIT_SECTION;
Boolean isInstalledUTF8table(void) EDIT_SECTION;
void StrNCopySJtoUTF8(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine, Boolean isFtr) EDIT_SECTION;
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine) EDIT_SECTION;
