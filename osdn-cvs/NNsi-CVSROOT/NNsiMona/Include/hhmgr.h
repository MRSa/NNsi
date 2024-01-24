/*****************************************************************************
 *
 * Generic Conduit HHMgr Header File
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/

#ifndef _PALM_HH_DBMGR_H_
#define _PALM_HH_DBMGR_H_

#include <CPDbBMgr.h>
#include <CLog.H>

class CHHMgr : public CPDbBaseMgr
{
public:
    CHHMgr(CPLogging *pLogging, DWORD dwGenericFlags, eSyncTypes syncType = eDoNothing);
    CHHMgr(CPLogging *pLogging, DWORD dwGenericFlags, char  *pName, DWORD  dwCreator, DWORD  dwType, WORD   dbFlags, WORD   dbVersion, int  iCardNum, eSyncTypes syncType );

    virtual ~CHHMgr();

public:   // Public Variables
    CPLogging *m_pLog;

protected:  // protected Variables
    BYTE m_hhDB;
    
    CRawRecordInfo  m_rInfo;

    char  m_szName[SYNC_DB_NAMELEN];
    DWORD m_dwCreator;
    DWORD m_dwType;
    WORD  m_wFlags;
    WORD  m_wVersion;
    int     m_CardNum;
    BOOL  m_bRecordDB;

    CDbGenInfo m_appInfo;
    CDbGenInfo m_sortInfo;
    BOOL m_bNeedToSaveAppInfo;

public:     // Public Functions

    virtual long Open(void);
    virtual long Close(BOOL bDontUpdate=FALSE);

    // HH DB Properties
    virtual long GetName( char *pName, WORD *pwNameSize);
    virtual DWORD GetCreatorID(void){ return m_dwCreator;}
    virtual DWORD GetType(void){ return m_dwType;}
    virtual DWORD GetFlags(void){ return m_wFlags;}
    virtual DWORD GetVersion(void){ return m_wVersion;}


    virtual long SetName( char *pName);
    virtual long SetCreatorID(DWORD dwCreator);
    virtual long SetType(DWORD dwType);
    virtual long SetFlags(WORD wFlags);
    virtual long SetVersion(WORD wVersion);

    virtual long GetRecordCount(DWORD &dwRecCount);
    virtual long SetRecordCount(DWORD dwRecCount);

    // record functions
    virtual long FindRecByID(DWORD dwID, CPalmRecord &palmRec, BOOL bLogError=TRUE);

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

    virtual long PurgeDeletedRecs(void);


    virtual long Sort(eSortMethods sortMethod);

    virtual long PreSync(void);
    virtual long PostSync(void);

    virtual long PreAppInfoSync(void);
    virtual long PostAppInfoSync(void);

    virtual long GetAppInfo(CDbGenInfo &rInfo);
    virtual long SetAppInfo(CDbGenInfo &rInfo);
    virtual BOOL IsExtraAppInfo(void);
    virtual long SetExtraAppInfo(DWORD dwSize, BYTE *pData);
    virtual BYTE* GetExtraAppInfo(void);
    virtual DWORD GetExtraAppInfoSize(void);
    virtual long GetExtraAppInfo(BYTE *pBuf, DWORD *pdwBufSize);

    virtual long LoadModifiedRecords(void);
    virtual long LoadAllRecords(void);

    virtual long GetRecIdMap(DWORD *pdwIdArray, DWORD *pdwSizeInBytes);
    virtual long SetRecIdMap(DWORD *pdwIdArray, DWORD dwIDCount){ return GEN_ERR_NOT_SUPPORTED;}

protected:    // protected Functions
    virtual long Open(eOpenDbType mode);

    virtual long AllocateRawRecord(void);
    virtual void FreeRawRecord(void);

    virtual long ExtractCategories(void);
    virtual long CompactCategories();

    virtual long GetInfoBlock(CDbGenInfo &info, BOOL bSortInfo=FALSE);


    virtual long FindNextRecByIndex(CPalmRecord &palmRec);
    virtual long FindNextModifiedRec(CPalmRecord &palmRec);

    // Category Record Access
    virtual long FindNextRecInCat(CPalmRecord &palmRec);
    virtual long FindNextModRecInCat(CPalmRecord &palmRec);


    // Internal Functions
    virtual long HHFindRecByID(DWORD dwID, CPalmRecord &palmRec);
    virtual long AddToCache(CPalmRecord &palmRec);

    virtual long AllocateDBInfo(CDbGenInfo &info, BOOL bClearData=FALSE);

    virtual long ConvertPositionMap(CPositionInfo &rInfo);
    virtual unsigned long SwapDWordToIntel(DWORD dwSubj);
};
#endif