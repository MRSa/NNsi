/*****************************************************************************
 *
 * Generic Conduit PC Database Mgr Header File
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
#ifndef _PC_DB_MGR_H_
#define _PC_DB_MGR_H_
#include <CPDbBMgr.h>
#include <CLog.H>
#include <CPString.H>

#define ATTRIB_UPDATE       0x0001
#define ATTRIB_DELETE       0x0002
#define ATTRIB_ARCHIVE      0x0004
#define ATTRIB_NEW          0x0008
#define ATTRIB_PRIVATE      0x0010


#define  STR_CRETURN  "\r" 
#define  STR_NEWLINE  "\n" 

#define  CH_CRETURN  0x0D 
#define  CH_NEWLINE  0x0A 

#ifndef fldStatusNONE
#define fldStatusNONE     0
#define fldStatusADD      0x01
#define fldStatusUPDATE   0x02
#define fldStatusDELETE   0x04
#define fldStatusPENDING  0x08
#define fldStatusARCHIVE  0x80 
#define fldStatusPRIVATE  0x40 
#endif
enum eFieldTypes { eNone=0,   eInteger=1, 
                   eFloat=2,  eDate=3, 
                   eAlpha=4,  eString=5, 
                   eBool=6,   eBitFlag=7,
                   eRepeateEvt=8 }; 


#ifdef macintosh
// moved CREATOR_TYPE and FILE_TYPE into STR resources to be customized. 
#include "GenericConduitResources.h"
#endif



typedef struct StorageRecordType {
    // structure info
    DWORD   dwStructSize;
    DWORD    dwStructVersion;

    DWORD   dwRawDataOffset;
    // non-standard members
    DWORD   dwInternalFlags;
    
    // header info // DWORDs for WORDs => future changes possible.
                    // less work for us than if we do the change now.
    DWORD       dwRecId;    
    DWORD       dwRecIndex;    
    WORD        wAttribs;    
    DWORD       dwCatIndex;        
    DWORD       dwConduitId;    
    DWORD       dwRecSize;    

} STORAGE_RECORD;

typedef  STORAGE_RECORD *STORAGE_RECORD_PTR;
#define CM_MIN_STORAGE_SIZE            sizeof(STORAGE_RECORD)
#define CM_STORAGE_BUFFER_OFFSET           sizeof(STORAGE_RECORD) 
#define CM_STORAGE_VERSION_1            0x00010001
#define CM_STORAGE_VERSION_2            0x1002

#ifndef MAX_DATA_SIZE
#define MAX_DATA_SIZE   MAX_RECORD_SIZE
#endif

typedef struct StorageInfoType {
    DWORD   dwStructSize;
    WORD    wVersion;
    DWORD   dwRawDataOffset;

    char    fileName[DB_NAMELEN];  // Name of remote database file 
    DWORD    dwTotalBytes;            // Byte length of 'pBytes'
    DWORD    dwBytesRead;             // Inbound byte count
    DWORD    dwReserved;             // Reserved - set to NULL    
} STORAGE_INFO;


typedef  STORAGE_INFO *STORAGE_INFO_PTR;
#define CM_MIN_STORAGE_INFO_SIZE            sizeof(STORAGE_INFO)
#define CM_INFO_BUFFER_OFFSET           sizeof(STORAGE_INFO) 

typedef struct StorageHeaderType {
    DWORD   dwStructSize;
    WORD    wVersion;

    char    szName[DB_NAMELEN];
    DWORD    dwDBVersion;
    DWORD   dwDBCreator;
    DWORD   dwDBType;
    WORD    wDBFlags;

    long    lDBModDate;
    DWORD   dwDBModNumber;
    long    lDBCreateDate;
    long    lDBBackupDate;
    long    lDBRecCount;
    long    dwAppInfo;
    long    dwSortInfo;
} STORAGE_HEADER;

typedef STORAGE_HEADER *STORAGE_HEADER_PTR;
#define CM_STORAGE_HEADER_SIZE sizeof(STORAGE_HEADER)
#define CM_STORAGE_HEADER_VERSION_1 0x0001


#ifdef _16K_NOTES	
	#define  TRANS_BUFFER_SIZE  64000
#else
	#define  TRANS_BUFFER_SIZE  8192
#endif

class CPcMgr : public CPDbBaseMgr
{
public:
#ifndef NO_HSLOG
    CPcMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName = NULL, void *pFileUInfo = NULL, eSyncTypes syncType = eDoNothing);
#else
    CPcMgr(DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName = NULL, void *pFileUInfo = NULL, eSyncTypes syncType = eDoNothing);
#endif
    CPcMgr();
    virtual ~CPcMgr();

public:   // Public Variables
    CPLogging *m_pLog;

protected:  // protected Variables
#ifndef macintosh
    HANDLE m_hFile;
#else
	short m_hFile;
#endif

#ifndef macintosh
    TCHAR m_szDataFile[256];
#else
	short	m_VolumeID;
	long 	m_ParentID;
	Str63	m_ParName;
	Str63 	m_FileName;
#endif


    StorageInfoType    *m_pSortInfo;
    StorageInfoType    *m_pAppInfo;

    STORAGE_HEADER_PTR  m_pDBInfo;
    char    m_szDbName[DB_NAMELEN];  // Name of remote database file 

    BOOL    m_bAlreadyLoaded;
    DWORD   m_dwDBVersion;
    DWORD   m_dwDBCreator;
    DWORD   m_dwDBType;
    WORD    m_wDBFlags;
    long    m_lDBModDate;
    long    m_dwDBModNumber;
    long    m_lDBCreateDate;
    long    m_lDBBackupDate;
    long    m_lDBRecCount;

    BOOL    m_bNeedToSave;
    BOOL    m_bStartSync;
public:     // Public Functions
    virtual long Open(void);
    virtual long Close(BOOL bDontUpdate=FALSE);

    virtual long GetRecordCount(DWORD &dwRecCount);
    virtual long SetRecordCount(DWORD dwRecCount);

    // record functions
    virtual long FindRecByID(DWORD dwID, CPalmRecord &palmRec, BOOL bLogError = TRUE);

    // Category Record Access
    virtual long DeleteCategory(DWORD dwCategory, BOOL bMoveToUnfiled = TRUE);
    virtual long ChangeCategory(DWORD dwOldCatIndex, DWORD dwNewCatIndex);

    // Write Functions
    virtual long AddRec(CPalmRecord &palmRec);
    virtual long ModifyRec(CPalmRecord &palmRec);
    virtual long DeleteRec(CPalmRecord &palmRec);
    virtual long ModifyRecByIndex(DWORD dwIndex, CPalmRecord &palmRec);
   

    // Because New records will mess up fast syncs when writing new records to the Palm
    // organizer, I think we should differentiate between modified records and new records
    // during sync time.
    virtual long FindFirstNewRec(CPalmRecord& palmRec);

    virtual long PurgeDeletedRecs(void);


    virtual long PreSync(void);
    virtual long PostSync(void);

    virtual long PreAppInfoSync(void);
    virtual long PostAppInfoSync(void);

    virtual long MoveRecordCategories(DWORD *moveList);

    virtual long SetDBInfo(LPCSTR    lpName,
                              WORD      wDbFlags,    
                              DWORD     dwDbType,
                              DWORD     dwCreator, 
                              WORD      wVersion,
                              DWORD     dwModNumber,
                              long      lCreateDate,
                              long      lModDate,
                              long      lBackupDate);
    virtual long ExtractCategories(void);
    virtual long CompactCategories();
    // Palm Organizer DB Info calls
    virtual long GetAppInfo(CDbGenInfo &rInfo);
    virtual long SetAppInfo(CDbGenInfo &rInfo);
    virtual long GetSortInfo(CDbGenInfo &rInfo);
    virtual long SetSortInfo(CDbGenInfo &rInfo);

    virtual BOOL IsExtraAppInfo(void);
    virtual long SetExtraAppInfo(DWORD dwSize, BYTE *pData);
    virtual BYTE* GetExtraAppInfo(void);
    virtual DWORD GetExtraAppInfoSize(void);
    virtual long GetExtraAppInfo(BYTE *pBuf, DWORD *pdwBufSize);

protected:    // protected Functions
    virtual BOOL IsSavedNeeded(void) { return m_bNeedToSave; }
    virtual void SetSavedNeeded(BOOL bSet = TRUE); 
    virtual long AllocateAppInfo(DWORD dwRawDataSize, BOOL bClearData=FALSE);



    virtual long FindNextRecByIndex(CPalmRecord &palmRec);
    virtual long FindNextModifiedRec(CPalmRecord &palmRec);
    virtual long FindNextPendingRec(CPalmRecord &palmRec);

    // Category Record Access
    virtual long FindNextRecInCat(CPalmRecord &palmRec);
    virtual long FindNextModRecInCat(CPalmRecord &palmRec);

    // extra calls
    virtual long CloseDB(BOOL bDontUpdate=FALSE);
    virtual long Open(eOpenDbType mode);
    virtual long OpenDB(void);
    virtual long StoreDB(void);
    virtual long AddRec(StorageRecordType *pRec);
    virtual long RetrieveDB(void);
    virtual long ReadInData(LPVOID pBuffer, DWORD dwNumberOfBytesToRead);
    virtual long ReadInData(LPVOID pBuffer, DWORD *dwNumberOfBytesToRead);
    virtual BOOL DuplicateDatabase(char* srcPath, char* destPath, BOOL bFailIfExists = FALSE);
    virtual long CreateDBInfo(void);
    virtual void Empty(void);

    virtual long ConvertPCtoGeneric(StorageRecordType *pRec, CPalmRecord &palmRec);
    virtual long ConvertGenericToPc(CPalmRecord *pPalmRec, BYTE *pBinary, BOOL bClearAttributes=TRUE);
protected: // calls for serializing data
    virtual long WriteSerializedDword(DWORD dwToWrite);
    virtual long WriteSerializedWord(WORD wToWrite);
    virtual long WriteSerializedByte(BYTE sToWrite);
    virtual long WriteSerializedString(TCHAR *pString);
    virtual long WriteSerializedStringField(TCHAR *pString, DWORD dwCaseSensitive=FALSE);
    virtual long WriteSerializedBoolField(DWORD dwToWrite);
    virtual long WriteSerializedIntegerField(DWORD dwToWrite);
    virtual long WriteSerializedDateField(DWORD dwToWrite);
	virtual long WriteSerializedLong(long lToWrite);
	virtual long WriteSerializedInteger(int iToWrite);

    virtual long AddCRs(TCHAR* pSrc, TCHAR* pDest, int len);
    virtual long AddCRs(TCHAR* pSrc, TCHAR* pDest, int len, int iBufSize);
    virtual int StripCRs(TCHAR* pDest, const TCHAR* pSrc, int len);
    virtual long ReadSerializedString(TCHAR *pStrBuf, int *piSize);
    virtual long ReadSerializedString(CPString &cStr);

    virtual long ReadSerializedByte(BYTE &bValue);
    virtual long ReadSerializedWord(WORD &wValue);
    virtual long ReadSerializedDword(DWORD &dwToRead);
	virtual long ReadSerializedLong(long &lToRead);
	virtual long ReadSerializedInteger(int &iToRead);
    virtual DWORD FlipSerialDword(DWORD &dwToFlip);
    virtual WORD FlipSerialWord(WORD &wToFlip);

    virtual long WriteOutData(LPVOID pBuffer, DWORD dwNumberOfBytesToWrite);
    virtual long ResetSyncFlags();

};

#endif