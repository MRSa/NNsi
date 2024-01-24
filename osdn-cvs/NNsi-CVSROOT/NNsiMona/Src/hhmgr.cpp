/*****************************************************************************
 *
 * Generic Conduit CHHMgr Src File
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
#ifndef macintosh
#define ASSERT(f)          ((void)0)
#endif

#define TRACE0(sz)
#define TRACE(sz)
#include <windows.h>
#include <string.h>
#include <stdio.h>
#ifdef METROWERKS_WIN
#include <wmem.h>
#else
#include <memory.h>
#endif

#ifdef macintosh
#include <GenCndMacSupport.h>
#include <string.h>
#include <stdlib.h>
#endif

#ifndef macintosh
#include <sys/stat.h>
#include <TCHAR.H>
#endif

#include <syncmgr.h>
#include <hhmgr.h>
#include <Pgenerr.h>

#ifndef dmHdrAttrResDB
#define	dmHdrAttrResDB				0x0001	// Resource database
#endif



/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  Constructor
 *
 * Description: This method initializes the class object.
 *
 * Parameter(s): syncType - an enum value specifying the sync type to be performed.
 *
 * Return Value(s): None.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
CHHMgr::CHHMgr(CPLogging *pLogging, DWORD dwGenericFlags, eSyncTypes syncType) 
: CPDbBaseMgr(dwGenericFlags, syncType)
{
    TRACE0("CHHMgr Constructor\n");

    m_hhDB = 0;
    
    m_rInfo.m_pBytes = NULL;
    m_rInfo.m_TotalBytes = 0;
    m_pLog = pLogging;

    m_szName[0] = '\0';
    m_dwCreator = 0;
    m_dwType    = 0;
    m_wFlags    = 0;
    m_wVersion  = 0;
    m_bRecordDB = TRUE; // default as a Record DB vs a Resource DB

    memset(&m_appInfo, 0, sizeof(CDbGenInfo));
    memset(&m_sortInfo, 0, sizeof(CDbGenInfo));

    m_bNeedToSaveAppInfo = FALSE;
}


/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  Constructor
 *
 * Description: This method initializes the class object.
 *
 * Parameter(s): pName - a pointer to a character buffer specifying the 
 *                          name of the database being synced.
 *               dwCreator - a DWORD specifying the Creator ID for the database.
 *               dwType - a DWORD specifying the type of Database on the HH.
 *               dbFlags - a WORD specifying the flags of the HH database.
 *               dbVersion - a WORD specifying the Version of the HH Database.
 *               syncType - an enum value specifying the sync type to be performed.
 *
 * Return Value(s): None.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
CHHMgr::CHHMgr(CPLogging *pLogging, 
               DWORD dwGenericFlags, 
               char *pName, 
               DWORD dwCreator, 
               DWORD dwType, 
               WORD dbFlags, 
               WORD dbVersion, 
			   int  iCardNum,
               eSyncTypes syncType)
: CPDbBaseMgr(dwGenericFlags, syncType)
{
    TRACE0("CHHMgr Constructor\n");

    m_hhDB = 0;
    
    m_rInfo.m_pBytes = NULL;
    m_rInfo.m_TotalBytes = 0;

    m_szName[0] = '\0';
    SetName( pName);
    m_dwCreator = dwCreator;
    m_dwType    = dwType;
    m_wFlags    = dbFlags;
    m_wVersion  = dbVersion;
	m_CardNum = iCardNum;

    /*
        // resource or record DB? 
    */
    m_bRecordDB = !(dbFlags & dmHdrAttrResDB); // default as a Record DB vs a Resource DB

    memset(&m_appInfo, 0, sizeof(CDbGenInfo));
    memset(&m_sortInfo, 0, sizeof(CDbGenInfo));
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method: Destructor
 *
 * Description: This method frees any allocated objects
 *
 * Parameter(s): None.
 *
 * Return Value(s): None.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
CHHMgr::~CHHMgr()
{
    TRACE0("CHHMgr Destructor\n");

    FreeRawRecord();
    if (m_appInfo.m_pBytes) {
        delete [] m_appInfo.m_pBytes;
        m_appInfo.m_pBytes = NULL;
    }

    if (m_sortInfo.m_pBytes) {
        delete [] m_sortInfo.m_pBytes;
        m_sortInfo.m_pBytes = NULL;
    }
    Close(TRUE);

}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  Open()
 *
 * Description: This method opens the database.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::Open(void)
{
    long retval = 0;
    switch (m_SyncType) {
        case eFast:
        case eSlow:
            retval = Open(eReadWrite);
            break;
        case eHHtoPC:
        case eBackup: 
            retval = Open(eReadWrite);
            break;
        case eInstall:
        case ePCtoHH:
        case eProfileInstall:
            retval = Open(eCreate);
            break;
        case eDoNothing:
            break;
        default:
            retval = GEN_ERR_SYNC_TYPE_NOT_SUPPORTED;
            break;
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  Open()
 *
 * Description: This method opens the actual database file.
 *
 * Parameter(s): mode - an enum value specifying how to open the database file.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::Open(eOpenDbType mode)
{
    long retval = 0;
    //BYTE m_TargetCardNo = CARD_NO;
    switch (mode) {
        case eRead:
            retval = SyncOpenDB(m_szName, 
                                m_CardNum, 
                                m_hhDB,
                                eDbRead);
            if ((retval) && (retval == SYNCERR_NOT_FOUND))
                retval = GEN_ERR_NO_HH_DB;


            break;
        case eWrite:
            retval = SyncOpenDB(m_szName, 
                                m_CardNum, 
                                m_hhDB,
                                eDbWrite);
            if ((retval) && (retval == SYNCERR_NOT_FOUND))
                retval = GEN_ERR_NO_HH_DB;
            break;
        case eReadWrite:
            retval = SyncOpenDB(m_szName, 
                                m_CardNum, 
                                m_hhDB,
                                eDbRead | eDbWrite);
            if ((retval) && (retval == SYNCERR_NOT_FOUND))
                retval = GEN_ERR_NO_HH_DB;
            break;
        case eCreate:
            {
                CDbCreateDB  createInfo;

                // Delete any existing DB with the same name
                retval = SyncDeleteDB(m_szName, m_CardNum);
                // TODO: Check this value
    
                memset(&createInfo, 0, sizeof(CDbCreateDB));

                createInfo.m_Creator     = m_dwCreator; 
                createInfo.m_Type        = m_dwType; 
                createInfo.m_Flags       = (eDbFlags) m_wFlags;                //eRecord; 
                createInfo.m_CardNo     = m_CardNum;  
                _tcscpy(createInfo.m_Name, m_szName);
                createInfo.m_Version    = m_wVersion;
    
                if ((retval = SyncCreateDB(createInfo)) == SYNCERR_NONE){
                    m_hhDB = createInfo.m_FileHandle;
                } else {
                    TCHAR szDB[256];
                    m_pLog->FormatDBString(szDB, m_szName, m_dwCreator, m_dwType,m_wFlags,m_wVersion);
                    m_pLog->CreateDBFailure(retval, szDB);
                }
            }
            break;
        default:
            break;
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  Close()
 *
 * Description: This method closes the database.
 *
 * Parameter(s): bDontUpdate - a boolean specifying whether the database changes
 *                  should be saved or not.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::Close(BOOL bDontUpdate)
{
    long retval = 0;
    long retval2 = 0;

    if (m_hhDB == NULL) 
        return GEN_ERR_DB_NOT_OPEN;

    if (!bDontUpdate) {
        retval = SyncResetSyncFlags(m_hhDB);
        if (retval)
            m_pLog->BadResetSyncFlags(retval);
    }
    retval2 = SyncCloseDB(m_hhDB);
    if (retval2)
        m_pLog->BadCloseDB(retval2);
    if (!retval)
        retval = retval2;
    m_hhDB = NULL;
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  GetRecordCount()
 *
 * Description: This method retrieves the current record count of the database.
 *
 * Parameter(s): dwRecCount - a DWORD that receives the record count.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::GetRecordCount(DWORD &dwRecCount)
{
    long retval;
    WORD wRecCount;

    retval = SyncGetDBRecordCount(m_hhDB, wRecCount);
    if (!retval) {
        m_dwRecordCount = wRecCount;
        dwRecCount = m_dwRecordCount;
    }

    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SetRecordCount()
 *
 * Description: This method could be used to allocate record space in the database.
 *          It is not implemented in this database.
 *
 * Parameter(s): dwRecCount - a DWORD specifying the new database record count.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::SetRecordCount(DWORD dwRecCount)
{
    return GEN_ERR_NOT_SUPPORTED;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  FindRecByID()
 *
 * Description: This method searches through the database for a record with 
 *          the matching ID.
 *
 * Parameter(s): dwID - a dword specifying the record ID to search for.
 *               palmRec - a CPalmRecord to receive the found record's data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::FindRecByID(DWORD dwID, CPalmRecord &palmRec, BOOL bLogError)
{
    long retval;

    retval = AllocateRawRecord();
    if (retval) 
        return retval;

    m_rInfo.m_RecId = dwID;

    retval = SyncReadRecordById(m_rInfo);
    if (!retval) {
        retval = palmRec.ConvertToPC(m_rInfo);
    } else {
        palmRec.Initialize();
        palmRec.SetID(dwID);
        if (bLogError) {
            m_pLog->RecordReadFailure(retval, palmRec);
        }
    }
    return retval;
}


/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  DeleteCategory()
 *
 * Description: This method deletes all records in a category or moves all the 
 *              records in the category to unfiled depending on the value of
 *              'bMoveToUnfiled'.
 *
 * Parameter(s): dwCategory - a DWORD specifying the index of the category to delete.
 *               bMoveToUnfiled - a boolean. If TRUE, all records in the category are
 *                          moved to unfiled. If False, all records in the category 
 *                          are deleted.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::DeleteCategory(DWORD dwCategory, BOOL bMoveToUnfiled)
{
    long retval;

    if ((dwCategory < 0) || (dwCategory >= MAX_CATEGORIES))
        return CAT_ERR_INDEX_OUT_OF_RANGE;

    BYTE sCategory = LOBYTE(LOWORD(dwCategory));

    if (!bMoveToUnfiled)
        retval = SyncPurgeAllRecsInCategory(m_hhDB, sCategory);
    else 
        retval = SyncChangeCategory(m_hhDB, sCategory, 0);
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  ChangeCategory()
 *
 * Description: This method moves all records in one category to another category.
 *
 * Parameter(s): dwOldCatIndex - a dword specifying the category to move from.
 *               dwNewCatIndex - a dword speciyfing the category to move to.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::ChangeCategory(DWORD dwOldCatIndex, DWORD dwNewCatIndex)
{
    long retval;

    if ((dwOldCatIndex < 0) || (dwOldCatIndex >= MAX_CATEGORIES))
        return CAT_ERR_INDEX_OUT_OF_RANGE;

    BYTE sCategory = LOBYTE(LOWORD(dwOldCatIndex));

    if ((dwNewCatIndex < 0) || (dwNewCatIndex >= MAX_CATEGORIES))
        return CAT_ERR_INDEX_OUT_OF_RANGE;

    BYTE sNewCategory = LOBYTE(LOWORD(dwNewCatIndex));

    retval = SyncChangeCategory(m_hhDB, sCategory, sNewCategory);
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  AddRec()
 *
 * Description: This method adds records to the database.
 *
 * Parameter(s): palmRec - a CPalmRecord object specifying the record to add.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::AddRec(CPalmRecord &palmRec)
{
    long retval;

    palmRec.ResetAttribs();

    retval = AllocateRawRecord();
    if (retval) 
        return retval;

    retval = palmRec.ConvertToHH(m_rInfo);
    if (!retval) {
        m_rInfo.m_FileHandle = m_hhDB;
        if (m_bRecordDB)
            retval = SyncWriteRec(m_rInfo);
        else 
            retval = SyncWriteResourceRec(m_rInfo);
        if (!retval) {
            palmRec.SetIndex(m_rInfo.m_RecIndex);
            palmRec.SetID(m_rInfo.m_RecId);
        }
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  ModifyRecByIndex()
 *
 * Description: This method modifies the record at the specified index to be a 
 *          clone of the passed in record.
 *
 * Parameter(s):  dwIndex - the index of the record to modify.
 *                palmRec - a CPalmRecord object with the new data
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::ModifyRecByIndex(DWORD dwIndex, CPalmRecord &palmRec)
{
    long retval = 0;
    if (dwIndex == (DWORD)-1) 
        return GEN_ERR_RECORD_NOT_FOUND;
    palmRec.SetIndex(dwIndex);

    retval = ModifyRec(palmRec);
    if (retval)
        return retval;

    if (!m_pRecordList[dwIndex]) {
        m_pRecordList[dwIndex] = new CPalmRecord(palmRec);
        if (dwIndex >= m_dwMaxRecordCount)
            m_dwMaxRecordCount = dwIndex + 1;
    } else {
        retval = m_pRecordList[dwIndex]->Modify(palmRec);
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  ModifyRec()
 *
 * Description: This method changes a record in the database to be a clone of the
 *              record passed in.
 *
 * Parameter(s): palmRec - a CPalmRecord object specifying the record to change and
 *                      the changes to be made.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::ModifyRec(CPalmRecord &palmRec)
{
    long retval;
    retval = AllocateRawRecord();
    if (retval) 
        return retval;

    retval = palmRec.ConvertToHH(m_rInfo);
    if (!retval) {
        m_rInfo.m_FileHandle = m_hhDB;
        if (m_bRecordDB)
            retval = SyncWriteRec(m_rInfo);
        else 
            retval = SyncWriteResourceRec(m_rInfo);
        if (!retval) {
            palmRec.SetIndex(m_rInfo.m_RecIndex);
            palmRec.SetID(m_rInfo.m_RecId);
        }
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  DeleteRec()
 *
 * Description: This method deletes a record, by ID, from this database.
 *
 * Parameter(s): palmRec - a CPalmRecord object of the record to delete
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::DeleteRec(CPalmRecord &palmRec)
{
    long retval;
    retval = AllocateRawRecord();
    if (retval) 
        return retval;

    retval = palmRec.ConvertToHH(m_rInfo);
    if (!retval) {
        m_rInfo.m_FileHandle = m_hhDB;
        if (m_bRecordDB)
            retval = SyncDeleteRec(m_rInfo);
        else 
            retval = SyncDeleteResourceRec(m_rInfo);
    }
    return retval;
}


/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  PurgeDeletedRecs()
 *
 * Description: This method removes all deleted/archive record data from 
 *          the database.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::PurgeDeletedRecs(void)
{
    long retval = 0;
    retval = SyncPurgeDeletedRecs(m_hhDB);
    if (retval) {
        m_pLog->BadPurge(retval);
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  Sort()
 *
 * Description: This method, if implemented, would resort the database depending
 *          upon the sort type.
 *
 * Parameter(s): sortMethod - an enum specifying how to sort the database
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::Sort(eSortMethods sortMethod)
{
    long retval = GEN_ERR_NOT_SUPPORTED;
    // this could only sort the database when in cache. It would not resort the 
    // hh database on the hh.
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  GetAppInfo()
 *
 * Description: This method returns AppInfo data in the provided structure.
 *
 * Parameter(s): rInfo - a CDbGenInfo object used to receive the App Info data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::GetAppInfo(CDbGenInfo &rInfo)
{
    long retval = 0;
    BYTE *pBuffer;

    pBuffer = rInfo.m_pBytes; 
    WORD wTotalBytes = rInfo.m_TotalBytes;
    memset(&rInfo, 0, sizeof(CDbGenInfo));
    if ((wTotalBytes > 0) && (pBuffer)) {
        memset(pBuffer, 0, wTotalBytes);
        rInfo.m_pBytes       = pBuffer; 
        rInfo.m_TotalBytes   = wTotalBytes;
    }

    retval = GetInfoBlock(rInfo);

    if (retval) {
        if (rInfo.m_pBytes) {
            delete [] rInfo.m_pBytes;
            rInfo.m_pBytes = NULL;
            rInfo.m_TotalBytes   = 0;
        }
    }


    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SetAppInfo()
 *
 * Description: This method is used to change the AppInfo data of this database.
 *
 * Parameter(s): rInfo - a CDbGenInfo object used to specify the new AppInfo data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::SetAppInfo(CDbGenInfo &rInfo)
{
    long retval = 0;

    retval = SyncWriteDBAppInfoBlock(m_hhDB, rInfo);

    if (rInfo.m_pBytes) {
        delete [] rInfo.m_pBytes;
        rInfo.m_pBytes = NULL;
    } 
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  ExtractCategories()
 *
 * Description: This method retrieves the category data from storage meduim.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::ExtractCategories(void)
{
    long retval = 0;

    if (!AreCategoriesSupported())
        return GEN_ERR_CATEGORIES_NOT_SUPPORTED;

    if (m_pCatMgr) {
        delete m_pCatMgr;
        m_pCatMgr = NULL;
    }

    if (m_appInfo.m_BytesRead == 0){
        retval = GetAppInfo(m_appInfo);
        if (retval)
            return retval;
    }

    if ((m_appInfo.m_BytesRead > 0) && (m_appInfo.m_pBytes)){
        m_pCatMgr = new CPCategoryMgr(m_appInfo.m_pBytes, m_appInfo.m_BytesRead);
        if (!m_pCatMgr)
            return GEN_ERR_LOW_MEMORY;
    } else {
        retval = GEN_ERR_NO_CATEGORIES;
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  CompactCategories()
 *
 * Description: This method stores the category information in a compress structure.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::CompactCategories()
{
    long retval = 0;

    if ((!m_pCatMgr) || (!m_pCatMgr->IsChanged()))
        return 0;

    retval = AllocateDBInfo(m_appInfo);
    if (retval)
        return retval;

    DWORD dwRecSize;

    dwRecSize = m_appInfo.m_TotalBytes;

    retval = m_pCatMgr->Compact(m_appInfo.m_pBytes, &dwRecSize);
    if (!retval) {
        // more than just the categories may be stored in the app info structure
        // This code only replaces the category area.
        if (!m_appInfo.m_BytesRead)
            m_appInfo.m_BytesRead = LOWORD(dwRecSize);
    }

    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  FindNextRecByIndex()
 *
 * Description: This method attempts to find the next indexed record in 
 *          the database.
 *
 * Parameter(s): palmRec - a CPalmRecord to receive the next found record's data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::FindNextRecByIndex(CPalmRecord &palmRec)
{
    long retval = GEN_ERR_NO_MORE_RECORDS;
    
    if (m_SyncType == eFast) {
        if (m_dwCurrentFindIndex >= m_dwMaxRecordCount)
            return retval;
        if (m_pRecordList[m_dwCurrentFindIndex] != NULL) {
            palmRec = *m_pRecordList[m_dwCurrentFindIndex];
            retval = 0;
            ++m_dwCurrentFindIndex;
        } else { 
            // we need to read the record from the HH

            retval = AllocateRawRecord();
            if (retval) 
                return retval;
            m_rInfo.m_RecIndex = LOWORD(m_dwCurrentFindIndex);

            retval = SyncReadRecordByIndex(m_rInfo);
            if (!retval) {
                retval = palmRec.ConvertToPC(m_rInfo);
                m_dwCurrentFindIndex = palmRec.GetIndex() + 1;
                AddToCache(palmRec);
            } else {
                palmRec.Initialize();
                palmRec.SetIndex(m_dwCurrentFindIndex);
                m_pLog->RecordReadFailure(retval, palmRec);
            }
        }


    } else {
        // in a slow sync all records will be in the cache
        if (m_dwCurrentFindIndex >= m_dwRecordCount)
            return retval;
        if (m_pRecordList[m_dwCurrentFindIndex]) {
            palmRec = *m_pRecordList[m_dwCurrentFindIndex];
            retval = 0;
        }
        ++m_dwCurrentFindIndex;

    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  FindNextModifiedRec()
 *
 * Description: This method attempts to find the next modified record in 
 *          the database.
 *
 * Parameter(s): palmRec - a CPalmRecord to receive the next found record's data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::FindNextModifiedRec(CPalmRecord &palmRec)
{
    long retval= GEN_ERR_NO_MORE_RECORDS;
    
    while (m_dwCurrentFindIndex < m_dwMaxRecordCount) {
        if ((m_pRecordList[m_dwCurrentFindIndex]) && 
            (m_pRecordList[m_dwCurrentFindIndex]->IsModified())){
            palmRec = *m_pRecordList[m_dwCurrentFindIndex];
            ++m_dwCurrentFindIndex;
            return 0;
        }
        ++m_dwCurrentFindIndex;
    }
    return retval;
}


/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  FindNextRecInCat()
 *
 * Description: This method attempts to find the next record in a 
 *      specific category in the database.
 *
 * Parameter(s): palmRec - a CPalmRecord to receive the next found record's data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::FindNextRecInCat(CPalmRecord &palmRec)
{
    long retval = GEN_ERR_NO_MORE_RECORDS;

    if (m_SyncType == eFast) {
        // read from the HH
    
        retval = AllocateRawRecord();
        if (retval) 
            return retval;

        m_rInfo.m_RecIndex = LOWORD(m_dwCurrentFindIndex);

        retval = SyncReadNextRecInCategory(m_rInfo);
        if (!retval) {
            retval = palmRec.ConvertToPC(m_rInfo);
            m_dwCurrentFindIndex = palmRec.GetIndex();
            AddToCache(palmRec);
        } else {
            palmRec.Initialize();
            palmRec.SetIndex(m_dwCurrentFindIndex);
            m_pLog->RecordReadFailure(retval, palmRec);
        }
    } else {
        // hh records are already cached in memory
        while (m_dwCurrentFindIndex < m_dwMaxRecordCount) {
            if ((m_pRecordList[m_dwCurrentFindIndex]) && 
                (m_pRecordList[m_dwCurrentFindIndex]->GetCategory() == m_dwCurrentFindCat)){
                palmRec = *m_pRecordList[m_dwCurrentFindIndex];
                ++m_dwCurrentFindIndex;
                return 0;
            }
            ++m_dwCurrentFindIndex;
        }
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  FindNextModRecInCat()
 *
 * Description: This method attempts to find the next modified record in a 
 *      specific category in the database.
 *
 * Parameter(s): palmRec - a CPalmRecord to receive the next found record's data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::FindNextModRecInCat(CPalmRecord &palmRec)
{
    long retval = GEN_ERR_NO_MORE_RECORDS;

    // in slow or fast sync, all modified records will be in the cache

    while (m_dwCurrentFindIndex < m_dwMaxRecordCount) {
        if ((m_pRecordList[m_dwCurrentFindIndex]) && 
            (m_pRecordList[m_dwCurrentFindIndex]->GetCategory() == m_dwCurrentFindCat) &&
            (m_pRecordList[m_dwCurrentFindIndex]->IsModified())){
            palmRec = *m_pRecordList[m_dwCurrentFindIndex];
            ++m_dwCurrentFindIndex;
            return 0;
        }
        ++m_dwCurrentFindIndex;
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  AllocateRawRecord()
 *
 * Description: This method allocates the memory for the Internal CRawRecordInfo
 *          structure. This structure is used for retrieving records from the HH
 *          device.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::AllocateRawRecord(void)
{
    if (m_rInfo.m_pBytes != NULL) {
        BYTE *pBytes;

        pBytes = m_rInfo.m_pBytes;
        memset(&m_rInfo, 0, sizeof(CRawRecordInfo));
        memset(pBytes, 0, MAX_RECORD_SIZE);

        m_rInfo.m_pBytes        = pBytes;
        m_rInfo.m_TotalBytes    = MAX_RECORD_SIZE;
        m_rInfo.m_FileHandle    = m_hhDB;
        return 0;
    }
    
    memset(&m_rInfo, 0, sizeof(CRawRecordInfo));
    m_rInfo.m_pBytes        = (BYTE *) new char[MAX_RECORD_SIZE];
    if (!m_rInfo.m_pBytes)
        return GEN_ERR_LOW_MEMORY;

    memset(m_rInfo.m_pBytes, 0, MAX_RECORD_SIZE);
    m_rInfo.m_TotalBytes    = MAX_RECORD_SIZE;
    m_rInfo.m_FileHandle    = m_hhDB;
    return 0;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  FreeRawRecord()
 *
 * Description: This method frees the memory associated with the internal
 *          CRawRecordInfo structure.
 *
 * Parameter(s): None.
 *
 * Return Value(s): None.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
void CHHMgr::FreeRawRecord(void)
{
    if (m_rInfo.m_pBytes != NULL) {
        delete [] m_rInfo.m_pBytes;
    }
    m_rInfo.m_pBytes = NULL;
    m_rInfo.m_TotalBytes = 0;

}


    // HH DB Properties
/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  GetName()
 *
 * Description: This method is used to retrieve the name of the database currently
 *          associated with this class.
 *
 * Parameter(s): pName - a pointer to a character array used to receive the name.
 *               pwNameSize - a pointer to a WORD specifying the size, in bytes, of
 *                  the pName array. On the return, this WORD will contain the length
 *                  of the copied in string
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::GetName( char *pName, WORD *pwNameSize)
{
    if (!pName)
        return GEN_ERR_INVALID_POINTER;
    if (!pwNameSize)
        return GEN_ERR_INVALID_POINTER;

    WORD wLen = (WORD)_tcslen(m_szName);
    if (!wLen){
        *pName = '\0';
        return 0;
    }
    if (wLen >= *pwNameSize) {
        *pwNameSize = (WORD)(wLen + 1);
        return GEN_ERR_BUFFER_TOO_SMALL;
    }
    _tcscpy(pName, m_szName);
    return 0;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SetName()
 *
 * Description: This method sets the name of the database associated with this
 *              Class.
 *
 * Parameter(s): pName - a pointer to a character array specifying the new name..
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::SetName( char *pName)
{
    if (m_hhDB)
        return GEN_ERR_DB_ALREADY_OPEN;

    WORD wLen;
    if (!pName){
        *m_szName = '\0';
        return 0;
    } 
    wLen = (WORD)_tcslen(pName);
    if (!wLen) {
        *m_szName = '\0';
        return 0;
    }
    if (wLen >= sizeof(m_szName))
        return GEN_ERR_DATA_TOO_LARGE;

    _tcscpy(m_szName, pName);
    return 0;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SetCreatorID()
 *
 * Description: This method set the Creator ID of the database accessed by this
 *          instance of this class.
 *
 * Parameter(s): dwCreator - a DWORD specifying the new Creator ID.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::SetCreatorID(DWORD dwCreator)
{
    if (m_hhDB)
        return GEN_ERR_DB_ALREADY_OPEN;
    m_dwCreator = dwCreator;
    return 0;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SetType()
 *
 * Description: This method sets the Type of this database.
 *
 * Parameter(s): dwType - a DWORD specifying the database's Type.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::SetType(DWORD dwType)
{
    if (m_hhDB)
        return GEN_ERR_DB_ALREADY_OPEN;
    m_dwType = dwType;
    return 0;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SetFlags()
 *
 * Description: This method sets the flags associated with this Database.
 *
 * Parameter(s): wFlags - a WORD specifying the new flags for this database.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::SetFlags(WORD wFlags)
{
    if (m_hhDB)
        return GEN_ERR_DB_ALREADY_OPEN;
    m_wFlags = wFlags;
    return 0;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SetVersion()
 *
 * Description: This method sets the version of the database accessed by this class.
 *
 * Parameter(s): wVersion - a WORD specifying the Version of the database on the HH.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::SetVersion(WORD wVersion)
{
    if (m_hhDB)
        return GEN_ERR_DB_ALREADY_OPEN;
    m_wVersion = wVersion;
    return 0;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  GetInfoBlock()
 *
 * Description: This method attempts to retrieve AppInfo/SortInfo data from the
 *      HH device.
 *
 * Parameter(s): rInfo - a CDbGenInfo object used to receive the Info data.
 *               bSortInfo - a boolean specifying whether to retrieve AppInfo(FALSE)
 *                      or SortInfo(TRUE) from the HH. The default for this value is
 *                      FALSE.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::GetInfoBlock(CDbGenInfo &info, BOOL bSortInfo)
{
    long        retval;


    // Allocate storage for app/sort info blocks
    retval = AllocateDBInfo(info);
    if (retval)
        return retval;

    _tcscpy((char*)info.m_FileName, m_szName);
    memset(info.m_pBytes, 0, info.m_TotalBytes);


    if (!bSortInfo) {
        // Read the AppInfo block
        retval = SyncReadDBAppInfoBlock(m_hhDB, info);
    } else {
        // Read the SortInfo block
        retval = SyncReadDBSortInfoBlock(m_hhDB, info);
    }
    if (retval) {// if error then clean up
        delete [] info.m_pBytes;
        info.m_pBytes = NULL;
        info.m_TotalBytes = 0;
    }

    return retval;
}


/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  PreAppInfoSync()
 *
 * Description: This method is called prior to the synchronization of the AppInfo
 *          block. It allows the database to load the AppInfo data from the storage
 *          medium.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::PreAppInfoSync(void)
{
    if (!IsAppInfoSupported())
        return 0;
    // need to load the app Info block from the HH and
    // extract the categories.
    return ExtractCategories();
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  PostAppInfoSync()
 *
 * Description: This method is called after the synchronization of the appInfo
 *          block. Its purpose is to store the AppInfo block back in the storage
 *          meduim.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::PostAppInfoSync(void)
{
    long        retval = 0;

    if (!IsAppInfoSupported())
        return 0;

    if (!m_bNeedToSaveAppInfo) {
        if (!AreCategoriesSupported())
            return 0;
        if ((!m_pCatMgr) || (!m_pCatMgr->IsChanged()))
            return retval;
    }
    if (AreCategoriesSupported()) {
        // need to fold changes back into the app info block and then
        // write the block to the HH.
        retval = CompactCategories();
    }

    retval = SetAppInfo(m_appInfo);
    return retval;
}


/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  PreSync()
 *
 * Description: This method is called prior to the synchronization of the records
 *          of the database. Depending upon the sync method, this method may or may 
 *          not load the database into memory.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::PreSync(void)
{
    long        retval = 0;

    switch (m_SyncType) {
        case eFast:
            retval = LoadModifiedRecords();
            break;
        case eHHtoPC:
        case eBackup:
        case eSlow:
            retval = LoadAllRecords();
            break;
        case ePCtoHH:
        case eInstall:
        case eProfileInstall:
            break;
        case eDoNothing:
            break;
        default:
            break;
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  PostSync()
 *
 * Description: This method is called after the synchonization of the records. 
 *          Depending on the synchronization type, this method may or may not
 *          store the record back in the file.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::PostSync(void)
{
    long        retval = 0;
    switch (m_SyncType) {
        case eFast:
        case eSlow:
            retval = Close();
            break;
        case eHHtoPC:
			retval = Close();
			break;
        case eBackup:
            retval = Close(TRUE);
            break;
        case ePCtoHH:
        case eInstall:
        case eProfileInstall:
            retval = Close();
            break;
        case eDoNothing:
            break;
        default:
            break;
    }
    return retval;
}


/***********************************************************************
 *
 * Internal Functions
 *
 **********************************************************************/
/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  HHFindRecByID()
 *
 * Description: This method searches through the database for a record with 
 *          the matching ID.
 *
 * Parameter(s): dwID - a dword specifying the record ID to search for.
 *               palmRec - a CPalmRecord to receive the found record's data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::HHFindRecByID(DWORD dwID, CPalmRecord &palmRec)
{
    long retval;

    retval = AllocateRawRecord();
    if (retval) 
        return retval;

    m_rInfo.m_RecId = dwID;

    retval = SyncReadRecordById(m_rInfo);
    if (!retval) {
        retval = palmRec.ConvertToPC(m_rInfo);
    } else {
        palmRec.Initialize();
        palmRec.SetID(dwID);
        m_pLog->RecordReadFailure(retval, palmRec);
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  LoadModifiedRecords()
 *
 * Description: This method retrieves all the modified records from the HH database and
 *          adds the records to the cache.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::LoadModifiedRecords(void)
{
    long retval;
    DWORD dwRecCount;

    retval = GetRecordCount(dwRecCount);
    if (retval)
        return retval;
    
    // allocate an array twice the size. 
    // Just a pointer array.

    retval = ReAllocateRecordList(dwRecCount * 2);
    if (retval)
        return retval;

    CPalmRecord *pPalmRec;

    retval = AllocateRawRecord();

    WORD wRecIndex = 0;
    while (!retval) {
        memset(m_rInfo.m_pBytes, 0, m_rInfo.m_TotalBytes);
        m_rInfo.m_RecIndex = wRecIndex;
        retval = SyncReadNextModifiedRec(m_rInfo);
        if (!retval) {
            wRecIndex = m_rInfo.m_RecIndex;
            pPalmRec = new CPalmRecord(m_rInfo);
            if (pPalmRec) {
                m_pRecordList[pPalmRec->GetIndex()] = pPalmRec;
                if (pPalmRec->GetIndex() >= m_dwMaxRecordCount)
                    m_dwMaxRecordCount = pPalmRec->GetIndex() + 1;
            } else {
                retval = GEN_ERR_LOW_MEMORY;
            }
        } else if (retval != SYNCERR_FILE_NOT_FOUND) { 
            CPalmRecord palmRec;
            palmRec.SetIndex(wRecIndex);
            m_pLog->RecordReadFailure(retval, palmRec);
        }
    }

    if (retval == SYNCERR_FILE_NOT_FOUND) // if there are no more records
        retval = 0;

    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  LoadAllRecords()
 *
 * Description: This method retrieves all the records from the HH database and
 *          adds the records to the cache.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::LoadAllRecords(void)
{
    long retval;
    DWORD dwRecCount, dwIndex;

    retval = GetRecordCount(dwRecCount);
    if (retval)
        return retval;
    
    // allocate an array twice the size. 
    // Just a pointer array.
    retval = ReAllocateRecordList(dwRecCount * 2);
    if (retval)
        return retval;

    CPalmRecord *pPalmRec;


    retval = AllocateRawRecord();
    if (retval) 
        return retval;

    for (dwIndex = 0; (dwIndex < m_dwRecordCount) && (!retval); dwIndex++){
        memset(m_rInfo.m_pBytes, 0, m_rInfo.m_TotalBytes);
        m_rInfo.m_RecIndex = LOWORD(dwIndex);

        retval = SyncReadRecordByIndex(m_rInfo);
        if (!retval) {
            pPalmRec = new CPalmRecord(m_rInfo);
            if (pPalmRec) {
                m_pRecordList[dwIndex] = pPalmRec;
            } else {
                retval = GEN_ERR_LOW_MEMORY;
            }
        } else if (retval != SYNCERR_FILE_NOT_FOUND) {
            CPalmRecord palmRec; 
            palmRec.Initialize();
            palmRec.SetIndex(dwIndex);
            m_pLog->RecordReadFailure(retval, palmRec);
        }
    }
    m_dwMaxRecordCount = dwIndex + 1;

    if (retval == SYNCERR_FILE_NOT_FOUND) // if there are no more records
        retval = 0;

    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  AddToCache()
 *
 * Description: This method adds a record to the cache in the same index position
 *          the record is on the HH device
 *
 * Parameter(s): palmRec - a CPalmRecord to add.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::AddToCache(CPalmRecord &palmRec)
{
    if (palmRec.GetIndex() >= m_dwArraySize) {
        // return error for now
        return GEN_ERR_INVALID_INDEX;
    }
    if (!m_pRecordList)
        return GEN_ERR_CACHE_NOT_INITIALIZED;

    if (m_pRecordList[palmRec.GetIndex()] != NULL) {
        delete m_pRecordList[palmRec.GetIndex()];
        m_pRecordList[palmRec.GetIndex()] = NULL;
    }
    m_pRecordList[palmRec.GetIndex()] = new CPalmRecord(palmRec);
    if (palmRec.GetIndex() >= m_dwMaxRecordCount)
        m_dwMaxRecordCount = palmRec.GetIndex() + 1;
    return 0;
        
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  IsExtraAppInfo()
 *
 * Description: This method is used to tell whether there is other data stored
 *          in the appInfo block other than category information.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
BOOL CHHMgr::IsExtraAppInfo(void)
{
    if (!m_appInfo.m_BytesRead)
        return FALSE;

    if ((!AreCategoriesSupported()) || (m_appInfo.m_BytesRead > MIN_CATEGORY_BLOCK_SIZE))
        return TRUE;

    return FALSE;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  AllocateDBInfo()
 *
 * Description: This method allocates the extra data associated with CDbGenInfo 
 *              structures. 
 *
 * Parameter(s): rInfo - the CDbGenInfo object to allocated data for.
 *               bClearData - a boolean specifying whether to zero out the allocated
 *                      buffer(TRUE) or not (FALSE).
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::AllocateDBInfo(CDbGenInfo &info, BOOL bClearData)
{
    BOOL bNew = FALSE;

    if (!info.m_pBytes) {
        info.m_pBytes = (BYTE *) new char[MAX_RECORD_SIZE];
        if (!info.m_pBytes)
            return GEN_ERR_LOW_MEMORY;
        info.m_TotalBytes = MAX_RECORD_SIZE;
        bNew = TRUE;
    } 

    if ((bClearData) || (bNew)) {
        memset(info.m_pBytes, 0, info.m_TotalBytes);
    }
    return 0;
}
/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SetExtraAppInfo()
 *
 * Description: This method is used to store non-category information in the 
 *          AppInfo block.
 *
 * Parameter(s): dwSize - a DWORD specifying the size of data pointed to by pData.
 *               pData - a pointer to a byte buffer containing the new data.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::SetExtraAppInfo(DWORD dwSize, BYTE *pData)
{
    long retval = 0;
    DWORD dwDataOffset = 0;
    
    retval = AllocateDBInfo(m_appInfo);
    if (retval)
        return retval;


    // offset the extra data spot if categories are supported
    if (AreCategoriesSupported()) {
        dwDataOffset = MIN_CATEGORY_BLOCK_SIZE;
    }

    // clear the buffer area
    memset(m_appInfo.m_pBytes + dwDataOffset, 0, m_appInfo.m_TotalBytes - dwDataOffset);
    if (!dwSize) {
        // our work is already done
        m_appInfo.m_BytesRead = LOWORD(dwDataOffset);
        return retval;
    }

    if (!pData) {
        return GEN_ERR_INVALID_POINTER;
    }

    if (dwSize > m_appInfo.m_TotalBytes) {
        return GEN_ERR_DATA_TOO_LARGE;
    }

    m_appInfo.m_BytesRead = (WORD)(LOWORD(dwDataOffset) + LOWORD(dwSize));
    memcpy(m_appInfo.m_pBytes + dwDataOffset, pData, dwSize);
    m_bNeedToSaveAppInfo = TRUE;
    return retval;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  GetExtraAppInfoSize()
 *
 * Description: This method returns the size of the non-category information
 *          stored in the AppInfo block.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
DWORD CHHMgr::GetExtraAppInfoSize(void)
{
    DWORD dwDataOffset = 0;
    // offset the extra data spot if categories are supported
    if (AreCategoriesSupported()) {
        dwDataOffset = MIN_CATEGORY_BLOCK_SIZE;
    }

    return (m_appInfo.m_BytesRead - dwDataOffset);
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  GetExtraAppInfo()
 *
 * Description: This method returns the non-category information stored in AppInfo
 *      block.
 *
 * Parameter(s): pBuf - a pointer to a byte buffer used to receive the requested data.
 *               pdwBufSize - a pointer to a DWORD specifying the size of pBuf.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::GetExtraAppInfo(BYTE *pBuf, DWORD *pdwBufSize)
{
    DWORD dwDataOffset = 0;
    // offset the extra data spot if categories are supported
    if (AreCategoriesSupported()) {
        dwDataOffset = MIN_CATEGORY_BLOCK_SIZE;
    }

    DWORD dwSize = GetExtraAppInfoSize();
    if (!dwSize){
        pdwBufSize = 0;
        return 0;
    }

    if (dwSize > *pdwBufSize) {
        *pdwBufSize = dwSize;
        return GEN_ERR_BUFFER_TOO_SMALL;
    }

    *pdwBufSize = dwSize;
    if (!pBuf) {
        return GEN_ERR_INVALID_POINTER;
    }

    ASSERT(m_appInfo.m_pBytes);
    ASSERT(m_appInfo.m_TotalBytes);
    memcpy(pBuf, (BYTE*)(m_appInfo.m_pBytes + dwDataOffset), dwSize);

    return 0;
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  GetExtraAppInfo()
 *
 * Description: This method returns a pointer to the non-category information
 *          stored in the AppInfo block.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
BYTE* CHHMgr::GetExtraAppInfo(void)
{
    DWORD dwDataOffset = 0;
    // offset the extra data spot if categories are supported
    if (AreCategoriesSupported()) {
        dwDataOffset = MIN_CATEGORY_BLOCK_SIZE;
    }

    DWORD dwSize = GetExtraAppInfoSize();
    if (!dwSize)
        return NULL;

    ASSERT(m_appInfo.m_pBytes);
    ASSERT(m_appInfo.m_TotalBytes);

    return (BYTE*)(m_appInfo.m_pBytes + dwDataOffset);
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  GetRecIdMap()
 *
 * Description: This method retrieves all the record IDs from the database and 
 *          stores the data in the supplied buffer.
 *          This method is used during post synchronization to confirm the databases
 *          have been synced. It may also be used to resort one database to match the
 *          order of the other.
 *
 * Parameter(s): pdwIdArray - a pointer to an array of DWORDs used to receive the 
 *                      database's record IDs.
 *               pdwSizeInBytes - a pointer to a dword specifying the size in Bytes
 *                      of the pdwIdArray buffer.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::GetRecIdMap(DWORD *pdwIdArray, DWORD *pdwSizeInBytes)
{
    DWORD dwRecCount;
    long retval = 0;

    GetRecordCount(dwRecCount);
    if (*pdwSizeInBytes < dwRecCount){
        *pdwSizeInBytes = dwRecCount;
        return GEN_ERR_BUFFER_TOO_SMALL;
    }

    
    if (!pdwIdArray)
        return GEN_ERR_INVALID_POINTER;

    CPositionInfo    posInfo;

    memset(&posInfo, 0, sizeof(posInfo));

    posInfo.m_TotalBytes = LOWORD(*pdwSizeInBytes);
    posInfo.m_pBytes     = (BYTE*) pdwIdArray;

    posInfo.m_FileHandle = m_hhDB;
    posInfo.m_MaxEntries = (WORD)LOWORD(dwRecCount);  

    // Read in the remote array of record Id's (motorola format)
    retval = SyncReadPositionXMap(posInfo);
    if (retval) {
        return retval;
    }
    return ConvertPositionMap(posInfo);
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  ConvertPositionMap()
 *
 * Description: This method converts a record ID list from the HH to the PC format.
 *
 * Parameter(s): rInfo - a CPositionInfo object containing the Records IDs to convert.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CHHMgr::ConvertPositionMap(CPositionInfo &rInfo)
{
#ifndef macintosh // no need to switch the byte ordering of the record ids on the mac.
    WORD  wOff;
    DWORD dwRecId;
    DWORD* pRecs;

    if (rInfo.m_pBytes && rInfo.m_NumReadIn > 0) 
    {
        pRecs = (DWORD*) rInfo.m_pBytes;

        for (wOff=0; wOff < rInfo.m_NumReadIn; wOff++)
        {
            dwRecId = pRecs[wOff]; 

            pRecs[wOff] = SwapDWordToIntel(dwRecId);
        }
    }
#endif
    return(0);
}

/******************************************************************************
 *
 * Class:   CHHMgr
 *
 * Method:  SwapDWordToIntel()
 *
 * Description: This method converts DWORD from motorola format to Intel format.
 *              Converts a 4 byte Motorola DWORD to an intel 4 byte DWORD.
 *              Grinds through and makes the least significant byte of 
 *              the passed in subject the most significant byte of the
 *              returned answer.
 *                    Motorola order:    Least to Most
 *                    Intel order: Most to Least
 *
 * Parameter(s): dwSubj - the DWORD to convert.
 *
 * Return Value(s): the converted DWORD.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
unsigned long CHHMgr::SwapDWordToIntel(DWORD dwSubj)
{
    DWORD  dwAnswer = 0; 
    BYTE   oneByte; 
    int    maxCars = sizeof(DWORD) - 1;    // 3

    for (int i=0; i <= maxCars; i++)
    {
        oneByte = (BYTE)((dwSubj >> ((maxCars - i) * 8)) & 0xFF); 
        dwAnswer |= oneByte << (i * 8);
    }
    return(dwAnswer);
}

