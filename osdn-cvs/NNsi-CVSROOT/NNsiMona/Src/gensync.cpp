/*****************************************************************************
 *
 * Generic Conduit CSynchronizer Src File
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
#include <CPResource.h>
#endif

#ifndef macintosh
#include <sys/stat.h>
#include <TCHAR.H>
#endif

#include "syncmgr.h"
#include "hhmgr.h"
#include "pcmgr.h"
#include "bckupmgr.h"
#include "archive.h"
#include "gensync.h"
#include "resource.h"

#define MAX_PROD_ID_TEXT	255


extern HANDLE hLangInstance;

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  Constructor
 *
 * Description: This method initializes the class object.
 *
 * Parameter(s):  rProps - a CSyncProperties object that specifies
 *                  synchronization data for this conduit.
 *                hInst - Language instance for strings
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
CSynchronizer::CSynchronizer(CSyncProperties& rProps, DWORD dwDatabaseFlags)
{

   TRACE0("CSynchronizer Constructor\n");

    m_dbHH = NULL;
    m_dbPC = NULL;
    m_dbBackup = NULL;
    memcpy(&m_rSyncProperties, &rProps, sizeof(m_rSyncProperties)); 
    m_remoteDB = NULL;
    m_TotRemoteDBs = rProps.m_nRemoteCount; // Fixed 3/25/99 KRM
    m_ConduitHandle =  (CONDHANDLE)0;
    m_dwDatabaseFlags = dwDatabaseFlags;
    TCHAR szAppName[256];
    
#ifndef macintosh
    LoadString((HINSTANCE)hLangInstance, IDS_CONDUIT_NAME, szAppName, sizeof(szAppName));
#else
    CPResource::GetConduitName(szAppName, sizeof(szAppName));
#endif   
	
    m_pLog = CreateLogging(szAppName);
    
    m_sCheckLevel = SYNC_CHECK_LEVEL_BASE;



    memset(&m_SystemInfo, 0, sizeof(m_SystemInfo));

    for (DWORD dwIndex = 0; dwIndex < MAX_CATEGORIES; dwIndex++){
        m_dbArchive[dwIndex] = NULL;
    }
}

/******************************************************************************
 *
 * Class:   CSynchronizer
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
CSynchronizer::~CSynchronizer()
{
    TRACE0("CSynchronizer Destructor\n");

    DeleteHHManager();
    DeletePCManager();
    DeleteBackupManager();

    if (m_SystemInfo.m_ProductIdText) {
        delete [] m_SystemInfo.m_ProductIdText;
	    m_SystemInfo.m_ProductIdText = NULL;
	    m_SystemInfo.m_AllocedLen = 0; 
    }
    if ((!m_rSyncProperties.m_RemoteDbList) && (m_remoteDB)) {
        delete m_remoteDB;
        m_remoteDB = NULL;
    }
    if (m_ConduitHandle) {
        UnregisterConduit();
    }
    if (m_pLog) {
        delete m_pLog;
        m_pLog = NULL;
    }
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  SetSyncMode()
 *
 * Description: This method sets the synchronization mode of the conduit.
 *
 * Parameter(s): syncType - an enum specifying which type synchronization.
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
void CSynchronizer::SetSyncMode(eSyncTypes  syncType)
{
    m_rSyncProperties.m_SyncType = syncType;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  GetSyncMode()
 *
 * Description: This method returns the current synchronization mode of this conduit.
 *
 * Parameter(s): None.
 *
 * Return Value(s): an enum specifying the current synchronization mode.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
eSyncTypes CSynchronizer::GetSyncMode(void)
{
    return m_rSyncProperties.m_SyncType;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  Perform()
 *
 * Description: This method is the entry point to the conduit synchronization. 
 *          It completes all task necessary to sync the databases.
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
long CSynchronizer::Perform(void)
{
    long retval = 0;
    long retval2 = 0;

    if (m_rSyncProperties.m_SyncType > eProfileInstall)
        return GEN_ERR_BAD_SYNC_TYPE;


    if (m_rSyncProperties.m_SyncType == eDoNothing) {
        return 0;
    }
    // Obtain System Information
    m_SystemInfo.m_ProductIdText = (BYTE*) new char [MAX_PROD_ID_TEXT];
    if (!m_SystemInfo.m_ProductIdText)
        return GEN_ERR_LOW_MEMORY;
    m_SystemInfo.m_AllocedLen = (BYTE) MAX_PROD_ID_TEXT; 
    retval = SyncReadSystemInfo(m_SystemInfo);
    if (retval)
        return retval;

    retval = RegisterConduit();
    if (retval)
        return retval;



    for (int iCount=0; iCount < m_TotRemoteDBs && !retval; iCount++) {
        retval = GetRemoteDBInfo(iCount);
        if (retval) {
            retval = 0;
            break;
        }

        switch (m_rSyncProperties.m_SyncType) {
            case eFast:
                retval = PerformFastSync();
                if ((retval) && (retval == GEN_ERR_CHANGE_SYNC_MODE)){
                    if (GetSyncMode() == eHHtoPC)
                        retval = CopyHHtoPC();
                    else if (GetSyncMode() == ePCtoHH)
                        retval = CopyPCtoHH();
                }
                break;
            case eSlow:
                retval = PerformSlowSync();  
                if ((retval) && (retval == GEN_ERR_CHANGE_SYNC_MODE)){
                    if (GetSyncMode() == eHHtoPC)
                        retval = CopyHHtoPC();
                    else if (GetSyncMode() == ePCtoHH)
                        retval = CopyPCtoHH();
                }
                break;
            case eHHtoPC:
            case eBackup: 
                retval = CopyHHtoPC();
                break;
            case eInstall:
            case ePCtoHH:
            case eProfileInstall:
                retval = CopyPCtoHH();
                break;
            case eDoNothing:
                break;
            default:
                retval = GEN_ERR_SYNC_TYPE_NOT_SUPPORTED;
                break;
        }

        DeleteHHManager();
        DeletePCManager();
        DeleteBackupManager();
        CloseArchives();
    }

    // Unregister the conduit
    retval2 = UnregisterConduit((BOOL)(retval != 0));

    if (!retval)
        return retval2;
    return retval;
}


/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  CopyHHtoPC()
 *
 * Description: This method duplicates the HH databaes on the PC.
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
long CSynchronizer::CopyHHtoPC(void)  
{
    long retval;
    long err = 0;
    DWORD dwActionList = 0;
    DWORD dwhhStatus = 0;
    CPalmRecord palmRec;
    CPalmRecord emptyRec;
    
    retval = CreateDbManagers();
    if (retval)
        return retval;

    retval = m_dbPC->SetDBInfo(m_remoteDB->m_Name,
                                   m_remoteDB->m_DbFlags,
                                   m_remoteDB->m_DbType,
                                   m_remoteDB->m_Creator,
                                   m_remoteDB->m_Version,
                                   m_remoteDB->m_ModNumber,
                                   m_remoteDB->m_CreateDate,
                                   m_remoteDB->m_ModDate,
                                   m_remoteDB->m_BackupDate);
    retval = m_dbHH->PreSync();
    if (retval)
        return retval;
    retval = m_dbPC->PreSync();
    if (retval)
        return retval;

    // replace PC app info with HH app Info
    retval = CopyAppInfoHHtoPC();
    if (retval)
        return retval;


    err = m_dbHH->FindFirstRecByIndex(palmRec);
    while (!err && !retval) {
        dwhhStatus = ATTR_HH_MULTIPLE * palmRec.GetStatus();
        dwActionList = FindHhToPcActions(dwhhStatus);
        retval = PerformActions(dwActionList, palmRec, emptyRec);
        if (!retval)
            err = m_dbHH->FindNext(palmRec);
    }
    if (err == GEN_ERR_NO_MORE_RECORDS)
        err = 0;

    retval = m_dbHH->PostSync();
    if (retval)
        return retval;
    retval = m_dbPC->PostSync();

    if (retval)
        return retval;

    // backup the PC DB
	if (m_dbBackup)
		retval = m_dbBackup->PostSync();

    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  CopyPCtoHH()
 *
 * Description: This method duplicates the PC database on the HH.
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
long CSynchronizer::CopyPCtoHH(void)  
{
    long retval;
    long err = 0;
    CPalmRecord palmRec;
    CPalmRecord emptyRec;
    DWORD dwpcStatus = 0;
    DWORD dwActionList = 0;
    
    retval = CreateDbManagers();
    if (retval)
        return retval;

    retval = m_dbHH->PreSync();
    if (retval)
        return retval;
    retval = m_dbPC->PreSync();
    if (retval)
        return retval;

    // replace HH app info with PC app Info
    retval = CopyAppInfoPCtoHH();
    if (retval)
        return retval;

    err = m_dbPC->FindFirstRecByIndex(palmRec);
    while (!err && !retval) {
        dwpcStatus = ATTR_PC_MULTIPLE * palmRec.GetStatus();
        dwActionList = FindPcToHhActions(dwpcStatus);
        retval = PerformActions(dwActionList, emptyRec, palmRec);
        if (!retval) {
            err = m_dbPC->FindNext(palmRec);
        }
    }
    if (err == GEN_ERR_NO_MORE_RECORDS)
        err = 0;

    retval = m_dbHH->PostSync();
    if (retval)
        return retval;
    retval = m_dbPC->PostSync();

    if (retval)
        return retval;

    // backup the PC DB
	if (m_dbBackup)
		retval = m_dbBackup->PostSync();

    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  PerformSlowSync()
 *
 * Description: This method synchronizes data between the PC and a HH device that
 *          did not synchronize with this PC the last time. To synchronize in this
 *          case, all records in both databases must be compared.
 *
 * Parameter(s): None
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
long CSynchronizer::PerformSlowSync(void)  
{
    long retval;
    long err = 0;
    CPalmRecord palmRec;
    
    retval = CreateDbManagers();
    if (retval)
        return retval;

    retval = SynchronizeAppInfo();
    if (retval)
        return retval;

    if (!retval) {
        retval = m_dbHH->PreSync();
        if (!retval)
            retval = m_dbPC->PreSync();
    }
    err = m_dbHH->FindFirstRecByIndex(palmRec);
    while (!err && !retval) {
        retval = SynchronizeRemoteRecord(palmRec);
        if (!retval)
            err = m_dbHH->FindNext(palmRec);
    }
    if (err == GEN_ERR_NO_MORE_RECORDS)
        err = 0;


    err = m_dbPC->FindFirstPendingRec(palmRec);
    while (!err && !retval) {
        retval = SynchronizeLocalRecord(palmRec);
        if (!retval)
            err = m_dbPC->FindNext(palmRec);
    }
    if (err == GEN_ERR_NO_MORE_RECORDS)
        err = 0;

    if (retval)
        return retval;

    retval = ConfirmSynchronization();

    if (retval)
        return retval;
    retval = m_dbHH->PostSync();
    if (retval)
        return retval;
    retval = m_dbPC->PostSync();

    if (retval)
        return retval;

    // backup the PC DB
	if (m_dbBackup)
		retval = m_dbBackup->PostSync();

    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  PerformFastSync()
 *
 * Description: This method synchronizes data between the PC and a HH device that
 *          synchronized with this PC the last time. To synchronize in this
 *          case, only modified records in each database must be synchronized.
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
long CSynchronizer::PerformFastSync(void)  
{
    long retval;
    long err = 0;
    CPalmRecord palmRec;
    
    retval = CreateDbManagers();
    if (retval)
        return retval;

    retval = SynchronizeAppInfo();
    if (retval)
        return retval;

    if (!retval) {
        retval = m_dbHH->PreSync();
        if (!retval)
            retval = m_dbPC->PreSync();
    }
	if (retval)
		return retval;
    err = m_dbHH->FindFirstModifiedRec(palmRec);
    while (!err && !retval) {
        retval = SynchronizeRemoteRecord(palmRec);
        if (!retval)
            err = m_dbHH->FindNext(palmRec);
    }
    if (err == GEN_ERR_NO_MORE_RECORDS)
        err = 0;

    err = m_dbPC->FindFirstModifiedRec(palmRec);
    while (!err && !retval) {
        retval = SynchronizeLocalRecord(palmRec);
        if (!retval)
            err = m_dbPC->FindNext(palmRec);
    }
    if (err == GEN_ERR_NO_MORE_RECORDS)
        err = 0;

    if (retval)
        return retval;


    retval = ConfirmSynchronization();

    if (retval)
        return retval;

    retval = m_dbHH->PostSync();
    if (retval)
        return retval;
    retval = m_dbPC->PostSync();

    if (retval)
        return retval;

    // backup the PC DB
	if (m_dbBackup)
		retval = m_dbBackup->PostSync();

    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  SynchronizeRemoteRecord()
 *
 * Description: This method finds the actions, needed to synchronize a HH record with
 *              the PC database, and performs them.
 *
 * Parameter(s): hhRec - a CPalmRecord object specifying the HH db record
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
long CSynchronizer::SynchronizeRemoteRecord(CPalmRecord &hhRec)
{
    long retval = 0;
    CPalmRecord pcRec;
    DWORD dwpcStatus;
    DWORD dwhhStatus;
    DWORD dwActionStatus = 0;
    DWORD dwActionList;

    // see if there is a matching PC rec;
    retval = m_dbPC->FindRecByID(hhRec.GetID(), pcRec, FALSE);
    if (retval) {
        // pc rec not found
        dwpcStatus = ATTR_PC_MULTIPLE * ATTR_NO_REC;
    } else {
        dwpcStatus = ATTR_PC_MULTIPLE * pcRec.GetStatus();
    }
    dwhhStatus = ATTR_HH_MULTIPLE * hhRec.GetStatus();

    if (m_rSyncProperties.m_SyncType == eSlow){

        if (!(dwhhStatus & HH_ATTR_DELETED)) {
            // if the hh record is delete, then there is no longer any data with the record
            // so there is nothing to compare

            if (dwpcStatus != PC_ATTR_NO_REC) {
                // then we need to compare the records
                //if (hhRec.Compare(pcRec) != eEqual){
                if (CompareRecords(hhRec, pcRec) != eEqual) { 
                    dwActionStatus |= ATTR_NOT_EQUAL;
                } else {
                    dwActionStatus |= ATTR_EQUAL;
                }
                if (!hhRec.IsUpdate()){ // if the hh is not modified
                    // if the HH record is none, check to see if it is in the backup db and
                    // if so, is it different.
					if (m_dbBackup) {
						CPalmRecord backupRec;
	                    retval = m_dbBackup->FindRecByID(hhRec.GetID(), backupRec);
						//if ((retval) || (hhRec.Compare(backupRec) != eEqual)){ 
						if ((retval) || (CompareRecords(hhRec, backupRec) != eEqual)) { 
							// if there is an error, then the record is not in the 
							// backup db. => implies new record
							// or if the hhRec is not the same as the backup db record.
							// => implies modified record
							hhRec.SetUpdate();
							dwhhStatus = ATTR_HH_MULTIPLE * hhRec.GetStatus();
						} 
					}
                }
            } else {
                // TODO: Not sure this is needed or not.
                // if there is no PC record in the current db, check the backup DB 
                // for the record
            }
        }
        dwActionList = FindHhSlowActions(dwhhStatus | dwpcStatus | dwActionStatus);
    } else {
        dwActionList = FindHhFastActions(dwhhStatus | dwpcStatus);
    }

    retval = PerformActions(dwActionList, hhRec, pcRec);
    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  SynchronizeLocalRecord
 *
 * Description: This method finds the actions, needed to synchronize a PC record with
 *              the HH database, and performs them.
 *
 * Parameter(s): pcRec - a CPalmRecord object specifying the PC db record
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
long CSynchronizer::SynchronizeLocalRecord(CPalmRecord &pcRec)
{
    long retval = 0;
	CPalmRecord hhRec;
	DWORD dwpcStatus;
	DWORD dwActionList;


    if (m_rSyncProperties.m_SyncType == eFast) {
		if (pcRec.GetID() != 0) {

			retval = m_dbHH->FindRecByID(pcRec.GetID(), hhRec, FALSE);

			if (retval) {
				if (retval != SYNCERR_NOT_FOUND)
					return retval;
				retval = 0;
				pcRec.SetNew(TRUE);
			}
		} else {
			// make sure the record is marked new
			pcRec.SetNew(TRUE);
		}
		dwpcStatus = ATTR_PC_MULTIPLE * pcRec.GetStatus();
        dwActionList = FindPcFastActions(dwpcStatus);
	} else { // if this is a slow sync, then the pc record does not exist on the hh.
		dwpcStatus = ATTR_PC_MULTIPLE * pcRec.GetStatus();
        dwActionList = FindPcSlowActions(dwpcStatus);
	}
    retval = PerformActions(dwActionList, hhRec, pcRec);
    return retval;
}



/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  ArchiveRecord()
 *
 * Description: This method opens the archive database associated with this record and
 *              adds the record.
 *
 * Parameter(s): palmRec - a CPalmRecord object specifying the record to be archived
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
long CSynchronizer::ArchiveRecord(CPalmRecord &palmRec)
{
    long retval = 0;

    // need to open the category db associated with this record and add this record to the
    // db
    // 1st Open the category archive DB file.
    // 1st ask the hh db category manager for the name of the file associated
    CPDbBaseMgr *pArchive = OpenArchive(palmRec.GetCategory(), retval);
    if (retval)
        return retval;
    ASSERT(pArchive);
	DWORD dwIndex = palmRec.GetIndex();
    DWORD dwAttribs = palmRec.GetAttribs();
    retval = pArchive->AddNewRecord(palmRec);
	palmRec.SetIndex(dwIndex);
    palmRec.SetAttribs(dwAttribs);
    

    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  CloseArchives()
 *
 * Description: This method is called at the end of a synchroniation of a database
 *              to save and close any open archive databases.
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
long CSynchronizer::CloseArchives(void)
{
    for (DWORD dwIndex = 0; dwIndex < MAX_CATEGORIES; dwIndex++){
        if (m_dbArchive[dwIndex]){
            m_dbArchive[dwIndex]->PostSync();
            delete m_dbArchive[dwIndex];
            m_dbArchive[dwIndex] = NULL;
        }
    }
    return 0;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  OpenArchive()
 *
 * Description: This method opens an archive, if it is not already open. 
 *
 * Parameter(s): dwCatIndex - a DWORD specifying the Index of the category associated with
 *                  record that will be added to the archive. If the database does not support
 *                  categories, this parameter will be ignore.
 *               dwError - a pointer to a long that will receive results of this method. 0 - no error
 *                  <0 - error code.
 *
 * Return Value(s): If this method is successful, it will return a CPDbBaseMgr pointer to an archive
 *                      database.
 *                  If this method is unsuccessful, it will return a NULL.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
CPDbBaseMgr *CSynchronizer::OpenArchive(DWORD dwCatIndex, long &dwError)
{
    dwError = 0;
    if (!(m_dwDatabaseFlags & GENERIC_FLAG_CATEGORY_SUPPORTED)) {
        // then the archive file is just the DB file with an .arc extension
        if (m_dbArchive[0]) // archive is already open
            return m_dbArchive[0];

        m_dbArchive[0] = CreateArchiveManager(m_rSyncProperties.m_LocalName); 
        if (!m_dbArchive[0]) {
            dwError = GEN_ERR_LOW_MEMORY;
            return NULL;
        }
        dwError = m_dbArchive[0]->Open();
        if (!dwError) {
            // check to see if this archive has been initialized
            // TODO - make this a better method
            DWORD dwRecords = 0;
            m_dbArchive[dwCatIndex]->GetRecordCount(dwRecords);
            if (!dwRecords) {
                dwError = m_dbArchive[dwCatIndex]->SetDBInfo(m_remoteDB->m_Name,
                                           m_remoteDB->m_DbFlags,
                                           m_remoteDB->m_DbType,
                                           m_remoteDB->m_Creator,
                                           m_remoteDB->m_Version,
                                           m_remoteDB->m_ModNumber,
                                           m_remoteDB->m_CreateDate,
                                           m_remoteDB->m_ModDate,
                                           m_remoteDB->m_BackupDate);
            }
        }
        return m_dbArchive[0];
    }
    // if categories are supported, we need to ask the category manager for the name
    // of the category file

    // make sure the cat index is valid
    if (dwCatIndex >= MAX_CATEGORIES) {
        dwError = CAT_ERR_INDEX_OUT_OF_RANGE;
        return NULL;
    }
    // see if we already have this file open
    if (m_dbArchive[dwCatIndex]) 
        return m_dbArchive[dwCatIndex];

    CPCategoryMgr *pCatMgr = m_dbHH->m_pCatMgr;
    if (!pCatMgr){
        // for some reason the hh cat manager is no longer valid, maybe a memory problem.
        dwError = GEN_ERR_LOW_MEMORY;
        return NULL;
    }

    // else we need to open the category file
    CPCategory *pTemp = pCatMgr->GetByIndex(dwCatIndex);
    if (!pTemp) {
        // if this fails then check if the cat index is 0 or not
        if (dwCatIndex == 0) { // unfiled failed, then something else must be wrong
            dwError = GEN_ERR_LOW_MEMORY;
            return NULL;
        } 
        dwCatIndex = 0;
        pTemp = pCatMgr->GetByIndex(dwCatIndex);
        if (!pTemp) {
            dwError = GEN_ERR_LOW_MEMORY;
            return NULL;
        } 
    }
    // we have a category, now we need to know its filename.
	TCHAR   szFileName[MAX_CAT_FILE_NAME];
    int iSize = sizeof(szFileName);
    dwError = pTemp->GetFileName(szFileName, &iSize);
    if (dwError)
        return NULL;

    m_dbArchive[dwCatIndex] = CreateArchiveManager(szFileName); 
        
    if (!m_dbArchive[dwCatIndex]) {
        dwError = GEN_ERR_LOW_MEMORY;
        return NULL;
    }

    dwError = m_dbArchive[dwCatIndex]->Open();
    if (!dwError) {
        // check to see if this archive has been initialized
        // TODO - make this a better method
        DWORD dwRecords = 0;
        m_dbArchive[dwCatIndex]->GetRecordCount(dwRecords);
        if (!dwRecords) {
            dwError = m_dbArchive[dwCatIndex]->SetDBInfo(m_remoteDB->m_Name,
                                       m_remoteDB->m_DbFlags,
                                       m_remoteDB->m_DbType,
                                       m_remoteDB->m_Creator,
                                       m_remoteDB->m_Version,
                                       m_remoteDB->m_ModNumber,
                                       m_remoteDB->m_CreateDate,
                                       m_remoteDB->m_ModDate,
                                       m_remoteDB->m_BackupDate);
        }
    }
    return m_dbArchive[dwCatIndex];
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:
 *
 * Description:
 *
 * Parameter(s):
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
long CSynchronizer::CreateDbManagers(void)
{
    long retval;

    m_bPcMoveList = FALSE;
    for (WORD wIndex = 0; wIndex < 32; wIndex++){
        m_pcMoveList[wIndex] = (DWORD)CAT_NO_INDEX;
    }

    retval = CreateHHManager();
    if ((retval) && (retval == GEN_ERR_NO_HH_DB)){
        // if there is no HH db then look at the current
        // sync mode and make an appropriate decision on what to do next
        if ((GetSyncMode() == eFast) ||
            (GetSyncMode() == eSlow)){ 
            retval = GEN_ERR_CHANGE_SYNC_MODE;  
            SetSyncMode(ePCtoHH);
        }
        return retval;
    }
    if (!retval){
        retval = CreatePCManager();
        if ((retval) && (retval == GEN_ERR_NO_PC_DB)){
            // if there is no PC db then look at the current
            // sync mode and make an appropriate decision on what to do next
            if ((GetSyncMode() == eFast) ||
                (GetSyncMode() == eSlow)){ 
                retval = GEN_ERR_CHANGE_SYNC_MODE;  
                SetSyncMode(eHHtoPC);
            }
        }
    }

    // create the backup table.
    if (!retval) {
        retval = CreateBackupManager(); // Fixed 3/25/99 KRM
    }
    
    return retval;
}
/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  DeleteHHManager
 *
 * Description: This method frees, if allocated, the HH database manager
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
void CSynchronizer::DeleteHHManager(void)
{
    if (m_dbHH)
        delete m_dbHH;
    m_dbHH = NULL;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  CreateHHManager()
 *
 * Description: This method creates the HH database manager. If it successfully creates
 *          the manager, it then opens the database.
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
long CSynchronizer::CreateHHManager(void)
{
    DeleteHHManager();

    m_dbHH = new CHHMgr(m_pLog, 
                        m_dwDatabaseFlags,
                        m_remoteDB->m_Name,
                        m_remoteDB->m_Creator,
                        m_remoteDB->m_DbType,
                        m_remoteDB->m_DbFlags,
                        0,
						m_remoteDB->m_CardNum,
                        m_rSyncProperties.m_SyncType);

    if (!m_dbHH)
        return GEN_ERR_LOW_MEMORY;
    return m_dbHH->Open();
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  DeletePCManager()
 *
 * Description: This method frees, if allocated, the PC database manager
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
void CSynchronizer::DeletePCManager(void)
{
    if (m_dbPC)
        delete m_dbPC;
    m_dbPC = NULL;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  CreatePCManager()
 *
 * Description: This method creates the PC database manager. If it successfully creates
 *          the manager, it then opens the database.
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
long CSynchronizer::CreatePCManager(void)
{
    DeletePCManager();


    
    m_dbPC = new CPcMgr(m_pLog, 
                        m_dwDatabaseFlags,
                        m_remoteDB->m_Name, 
                        m_rSyncProperties.m_LocalName, 
                  #ifdef macintosh
                        (void *)&(m_rSyncProperties.u.m_UserDirFSSpec),
                  #else
                        (void *)m_rSyncProperties.m_PathName,
                  #endif
                        m_rSyncProperties.m_SyncType);
    if (!m_dbPC)
        return GEN_ERR_LOW_MEMORY;
    return m_dbPC->Open();
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  DeleteBackupManager()
 *
 * Description: This method frees, if allocated, the backup PC database manager
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
void CSynchronizer::DeleteBackupManager(void)
{
    if (m_dbBackup)
        delete m_dbBackup;
    m_dbBackup = NULL;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  CreateBackupManager()
 *
 * Description: This method creates the backup database manager. If it successfully creates
 *          the manager, it then opens the database.
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
long CSynchronizer::CreateBackupManager(void)
{
    DeleteBackupManager();

    m_dbBackup = new CBackupMgr(m_pLog, 
                        m_dwDatabaseFlags,
                        m_remoteDB->m_Name, 
                        m_rSyncProperties.m_LocalName, 
                  #ifdef macintosh
                        (TCHAR *)&(m_rSyncProperties.u.m_UserDirFSSpec),
                  #else
                        m_rSyncProperties.m_PathName,
                  #endif
                        m_rSyncProperties.m_SyncType);
    if (!m_dbBackup)
        return GEN_ERR_LOW_MEMORY;
    return m_dbBackup->Open();
}

CPDbBaseMgr *CSynchronizer::CreateArchiveManager(TCHAR *pFilename) 
{
    return (CPDbBaseMgr *)new CArchiveDatabase(m_pLog,
                                m_dwDatabaseFlags,
                                m_remoteDB->m_Name, 
                                pFilename, 
      	             	#ifdef macintosh
                        		 (TCHAR *)&(m_rSyncProperties.u.m_UserDirFSSpec));
   		               #else     
                                m_rSyncProperties.m_PathName);
                        #endif
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  GetRemoteDBInfo()
 *
 * Description: Since multiple database information can be contained in the 
 *          CSyncProperties structure, it may be necessary for the conduit to
 *          synchronize several database. This method is used to access each of
 *          the databases in the structure.
 *
 * Parameter(s): iIndex - an index used to specify the database to reference.
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
long CSynchronizer::GetRemoteDBInfo(int iIndex)
{
    long retval = 0;
    if (m_rSyncProperties.m_RemoteDbList) {
        m_remoteDB = m_rSyncProperties.m_RemoteDbList[iIndex];
    } else {
        if (m_remoteDB)
            delete m_remoteDB;

        m_remoteDB = new CDbList;
        if (m_remoteDB) {
            m_remoteDB->m_Creator  = m_rSyncProperties.m_Creator; 
            m_remoteDB->m_DbFlags  = eRecord; // todo
            m_remoteDB->m_DbType   = m_rSyncProperties.m_DbType; 
            strcpy(m_remoteDB->m_Name, m_rSyncProperties.m_RemoteName[iIndex]);
            m_remoteDB->m_Version   = 0;
            m_remoteDB->m_CardNum   = 0;
            m_remoteDB->m_ModNumber = 0;
            m_remoteDB->m_CreateDate= 0;
            m_remoteDB->m_ModDate   = 0;
            m_remoteDB->m_BackupDate= 0;
        } else {
            retval = GEN_ERR_LOW_MEMORY;
        }
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  RegisterConduit()
 *
 * Description: This method is used to inform the sync manager that this conduit
 *      will now be communicating with the HH device.
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
long CSynchronizer::RegisterConduit(void)
{
    long retval = 0;
    // Register this conduit with SyncMgr.DLL for communication to HH
    retval = SyncRegisterConduit(m_ConduitHandle);
    if (!retval){
#ifndef macintosh
			// Notify the log that a sync is about to begin
  			m_pLog->SyncStart();  
#endif          
  	    
    }

    return(retval);

}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  UnregisterConduit()
 *
 * Description: This message is used to inform the sync manager that this conduit
 *          is done communicating with the HH device.
 *
 * Parameter(s): bError - a boolean specifying whether the conduit had an error (TRUE) 
 *                      that cause the database, accessed by this conduit, not to be
 *                      synchronized or not.
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
long CSynchronizer::UnregisterConduit(BOOL bError)
{
    long retval = 0;
    if (!m_ConduitHandle)
        return retval;
#ifndef macintosh
    m_pLog->SyncFinish(bError);
#endif    
    // Unregister the conduit
    retval = SyncUnRegisterConduit(m_ConduitHandle);
    m_ConduitHandle = 0;

    return(retval);
}




/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  SynchronizeSortInfo()
 *
 * Description: This function, if implemented, would synchronize the Sort Info 
 *      block of a Palm Organizer device.
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
long CSynchronizer::SynchronizeSortInfo(void)
{
    long                retval = 0;
    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  IsCommsError()
 *
 * Description: This method determines whether an error code is a general error
 *          or a communication with the HH device error.
 *
 * Parameter(s): lErr - a long specifying the error code to check.
 *
 * Return Value(s): TRUE - the error code specifies a communication error
 *                  FALSE - Non-communication error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
BOOL CSynchronizer::IsCommsError(long lErr)  
{
	BOOL bRetval = FALSE;

// TODO Add Trace code for Macintosh
#ifndef macintosh
	if ((lErr != -1) && (lErr & 0x00002000))
	{
		bRetval = TRUE;

		char buff[64];
		sprintf(buff, "::IsCommsError(0x%x)\n", lErr);
		TRACE(buff);

	}
#endif
	return(bRetval);
}



long CSynchronizer::CopyAppInfoHHtoPC(void)
{
    long retval = 0;

    if ((!m_dbHH->IsAppInfoSupported()) || 
        (!m_dbPC->IsAppInfoSupported()))
        return retval;
    // replace PC app info with HH app Info
    CDbGenInfo appInfo;

    appInfo.m_pBytes = (BYTE *) new char[MAX_RECORD_SIZE];
    if (!appInfo.m_pBytes)
        return GEN_ERR_LOW_MEMORY;
    appInfo.m_TotalBytes = MAX_RECORD_SIZE;

    retval = m_dbHH->GetAppInfo(appInfo);

    if (!retval) {
        retval = m_dbPC->SetAppInfo(appInfo);
    }
    delete [] appInfo.m_pBytes;

    return retval;
}

long CSynchronizer::CopyAppInfoPCtoHH(void)
{
    long retval = 0;

    if ((!m_dbHH->IsAppInfoSupported()) || 
        (!m_dbPC->IsAppInfoSupported()))
        return retval;

    // replace HH app info with PC app Info
    CDbGenInfo appInfo;

    appInfo.m_pBytes = (BYTE *) new char[MAX_RECORD_SIZE];
    if (!appInfo.m_pBytes)
        return GEN_ERR_LOW_MEMORY;
    appInfo.m_TotalBytes = MAX_RECORD_SIZE;

    retval = m_dbPC->GetAppInfo(appInfo);

    if (!retval) {
        retval = m_dbHH->SetAppInfo(appInfo);
    }
    delete [] appInfo.m_pBytes;
    return retval;
}


/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  CreateLogging()
 *
 * Description: This method is used to create a new instance of the logging class.
 *          This allows for subclassing the class and using the new class.
 *
 * Parameter(s): 
 *
 * Return Value(s): Null - if failure.
 *                  CPLogging * - a pointer to a new CPLogging object.
 *                  
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  11/21/2000    KRM         initial revision
 *
 *****************************************************************************/
CPLogging *CSynchronizer::CreateLogging(TCHAR *pAppName)
{
#ifndef macintosh
    return new CPLogging((HINSTANCE)hLangInstance, pAppName);
#else
    return new CPLogging(pAppName);
#endif    
}

