/*****************************************************************************
 *
 * Generic Conduit Synchronizer Class Header File
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/

#ifndef _PALM_SYNCHRONIZER_H_
#define _PALM_SYNCHRONIZER_H_

#include <clog.h>
#include <CPCatgry.h>
#include <Cpalmrec.h>
#include <Pgenerr.h>
#include <CPDbBMgr.h>

#define ATTR_PC_MULTIPLE    0x00000001
#define ATTR_HH_MULTIPLE    0x00010000
// implies

#define HH_ATTR_DELETED        0x00010000
#define HH_ATTR_ARCHIVED       0x00020000
#define HH_ATTR_MODIFIED       0x00040000
#define HH_ATTR_NEW            0x00080000
#define HH_ATTR_NONE           0x00200000
#define HH_ATTR_NO_REC         0x00400000
// and
#define PC_ATTR_DELETED        0x00000001
#define PC_ATTR_ARCHIVED       0x00000002
#define PC_ATTR_MODIFIED       0x00000004
#define PC_ATTR_NEW            0x00000008
#define PC_ATTR_NONE           0x00000020
#define PC_ATTR_NO_REC         0x00000040

#define ATTR_NOT_EQUAL         0x10000000
#define ATTR_EQUAL             0x20000000

// action types
#define ACTION_NONE                                 0x00000000
#define ACTION_DELETE_PC                            0x00000001
#define ACTION_ARCHIVE_PC                           0x00000002
#define ACTION_DELETE_HH                            0x00000004
#define ACTION_ARCHIVE_HH_ON_PC                     0x00000008
#define ACTION_ADD_PC_TO_HH                         0x00000010
#define ACTION_ADD_HH_TO_PC                         0x00000020
#define ACTION_WRITE_HH_TO_PC                       0x00000040
#define ACTION_WRITE_PC_TO_HH                       0x00000080
#define ACTION_FAST_DOUBLE_MODIFY                   0x00000100
#define ACTION_FAST_HH_ARCHIVE_DOUBLE_MODIFY        0x00000200
#define ACTION_FAST_PC_ARCHIVE_DOUBLE_MODIFY        0x00000400
#define ACTION_FAST_DOUBLE_ARCHIVE_DOUBLE_MODIFY    0x00000800
#define ACTION_SLOW_DOUBLE_MODIFY                   0x00002000      // change PC to new record and write to HH, write existing HH to PC
#define ACTION_DELETE_HH_IGNORE_ERROR               0x00010000      // if we are deleting the record whether it exists or not, then ignore the error.
#define ACTION_SYNC_ERROR                           0x10000000

#define LOG_MOD_DELETE                              0x01000000


#define SYNC_CHECK_LEVEL_BASE   0
#define SYNC_CHECK_LEVEL_1      1


class CSynchronizer
{
public:
    CSynchronizer(CSyncProperties& rProps, DWORD dwDatabaseFlags = GENERIC_FLAG_CATEGORY_SUPPORTED | GENERIC_FLAG_APPINFO_SUPPORTED);
    virtual ~CSynchronizer();


public:   // Public Variables
protected:  // protected Variables
    CPDbBaseMgr  *m_dbHH;
    CPDbBaseMgr  *m_dbPC;
    CPDbBaseMgr  *m_dbBackup;
    CPDbBaseMgr  *m_dbArchive[MAX_CATEGORIES];

    CSyncProperties m_rSyncProperties;
    CDbList* m_remoteDB;// Pointer remote DB info passed in sync properties
    short                   m_TotRemoteDBs;
    CSystemInfo               m_SystemInfo;      // Filled with Pilot system info
    CONDHANDLE m_ConduitHandle;
    DWORD           m_dwDatabaseFlags; // nee BOOL    m_bSupportCategories;
    CPLogging *m_pLog;
    DWORD m_dwYieldTime;

    DWORD m_pcMoveList[32];
    BOOL m_bPcMoveList;
    short m_sCheckLevel; 


public:     // Public Functions
    void SetSyncMode(eSyncTypes syncType);
    eSyncTypes GetSyncMode(void);
    virtual long Perform(void);
    

    
protected:    // protected Functions
    virtual long CopyHHtoPC(void);  
    virtual long CopyPCtoHH(void);  
    virtual long PerformSlowSync(void);  
    virtual long PerformFastSync(void);  

    virtual long DoFastDoubleArchiveDoubleModified( CPalmRecord &hhRec, CPalmRecord &pcRec);
    virtual long DoFastPCArchiveDoubleModified( CPalmRecord &hhRec, CPalmRecord &pcRec);
    virtual long DoFastHHArchiveDoubleModified( CPalmRecord &hhRec, CPalmRecord &pcRec);
    virtual long DoSlowDoubleModified(CPalmRecord &hhRec, CPalmRecord &pcRec);
    virtual long DoFastDoubleModified(CPalmRecord &hhRec, CPalmRecord &pcRec); 

    virtual long SynchronizeCategories(void);
    virtual long SynchronizeAppInfo(void);
    virtual long SynchronizeSortInfo(void);
    virtual long SynchronizeRemoteRecord(CPalmRecord &hhRec);
    virtual long SynchronizeLocalRecord(CPalmRecord &pcRec);

    virtual long ArchiveRecord(CPalmRecord &palmRec);

    virtual void DeletePCManager(void);
    virtual void DeleteHHManager(void);
    virtual long CreateHHManager(void);
    virtual long CreatePCManager(void);
    virtual long CreateDbManagers(void);

    virtual void DeleteBackupManager(void);
    virtual long CreateBackupManager(void);
    virtual CPDbBaseMgr *CreateArchiveManager(TCHAR *pFilename);
    
    
    virtual long GetRemoteDBInfo(int iIndex);
    virtual long RegisterConduit(void);
    virtual long UnregisterConduit(BOOL bError = FALSE);
    virtual long SetupCategorySyncCases(void);
    virtual long RemoveDeletedCategories( CPCategoryMgr *pRemoveFrom, 
                                             CPCategoryMgr *pExistList, 
                                             BOOL bHH);

    virtual long SynchronizePCCategories(void);
    virtual long SynchronizeHHCategories(void);
    virtual long MoveLocalCat(CPCategory *pcCat, CPCategory *hhCat);
	virtual long MoveLocalCat(DWORD dwOldIndex, DWORD dwNewIndex);


    virtual long PerformActions(DWORD dwActionList, CPalmRecord &hhRec, CPalmRecord &pcRec);
    virtual DWORD FindHhSlowActions(DWORD dwStatus);
    virtual DWORD FindHhFastActions(DWORD dwStatus);
    virtual DWORD FindPcFastActions(DWORD dwStatus);
    virtual DWORD FindPcSlowActions(DWORD dwStatus);
    virtual DWORD FindPcToHhActions(DWORD dwStatus);
    virtual DWORD FindHhToPcActions(DWORD dwStatus);

    BOOL IsCommsError(long lErr) ;
    virtual BOOL IsInDb(DWORD *pdwIdArray, DWORD dwArraySize, DWORD dwIdToFind);
    virtual long ConfirmSynchronization(void);
    virtual CPDbBaseMgr *OpenArchive(DWORD dwCatIndex, long &dwError);
    virtual long CloseArchives(void);

    virtual long CopyAppInfoHHtoPC( void );
    virtual long CopyAppInfoPCtoHH( void );

    virtual  eRecCompare CompareRecords(CPalmRecord &rec1, CPalmRecord &rec2);
    virtual CPLogging *CreateLogging(TCHAR *pAppName);

};

#endif //_PALM_DBMGR_H_
