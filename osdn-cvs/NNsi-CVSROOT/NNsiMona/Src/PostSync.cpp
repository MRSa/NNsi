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
#endif

#ifndef macintosh
#include <sys/stat.h>
#include <TCHAR.H>
#endif


#include <syncmgr.h>
#include <CPDbBMgr.h>
#include <hhmgr.h>
#include <pcmgr.h>
#include <bckupmgr.h>
#include <Pgenerr.h>
#include <gensync.h>


/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  ConfirmSynchronization()
 *
 * Description: This method checks to make sure record count matches between
 *              the two databases, and if not, gets the record IDs from 
 *              both databases and sees which record IDs do not exist 
 *              in the other database.
 *              The process in the ID existence test area is:
 *              1. see if a HH record ID exist in the PC record list
 *                  Yes, it exists) Then replace the PC ID in the list with -1.
 *                  No, not exists) Add the record to the PC list.
 *              2. Iterate through the remaining(non -1 IDs) PC IDs and
 *                  add the records to the HH.
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
long CSynchronizer::ConfirmSynchronization(void)
{
    long retval = 0;
    DWORD dwHhRecCount;
    DWORD dwPcRecCount;

    retval = m_dbHH->PurgeDeletedRecs();
    if (retval) {
        m_pLog->BadPurge(retval);
        return retval;
    } 

    retval = m_dbPC->PurgeDeletedRecs();
    if (retval) {
        m_pLog->BadPurge(retval, FALSE);
        return retval;
    } 

    retval = m_dbHH->GetRecordCount(dwHhRecCount);
    if (retval) 
        return retval;
    retval = m_dbPC->GetRecordCount(dwPcRecCount);
    if (retval) 
        return retval;

    if ((dwPcRecCount == dwHhRecCount) && (m_sCheckLevel == SYNC_CHECK_LEVEL_BASE)){
        // then we believe the sync was successful.
        return 0;
    }

    // allocate the record ID arrays
    DWORD *pdwHhIds = new DWORD[dwHhRecCount + 1];
    if (!pdwHhIds)
        return GEN_ERR_LOW_MEMORY;

    DWORD *pdwPcIds = new DWORD[dwPcRecCount + 1];
    if (!pdwPcIds){
        delete [] pdwHhIds;
        return GEN_ERR_LOW_MEMORY;
    }

    CPalmRecord palmRec;
    
    retval = m_dbHH->GetRecIdMap(pdwHhIds, &dwHhRecCount);
    if (!retval) {
        retval = m_dbPC->GetRecIdMap(pdwPcIds, &dwPcRecCount);
        if (!retval) {
            // loop through all the HH rec IDs and see which ones do not exist in the PC
            // Db. If one does not exist, retrieve it and add it to the db.
            for (DWORD dwHhIndex = 0; (dwHhIndex < dwHhRecCount) && (!retval); dwHhIndex++){
                if (!IsInDb(pdwPcIds, dwPcRecCount, pdwHhIds[dwHhIndex])){
                    retval = m_dbHH->FindRecByID(pdwHhIds[dwHhIndex], palmRec);
                    if (!retval) {
                        retval = m_dbPC->AddNewRecord(palmRec);
                    }
                }
            }
            for (DWORD dwPcIndex = 0; (dwPcIndex < dwPcRecCount) && (!retval); dwPcIndex++){
                // loop through all the PC rec Ids and see which ones are still valid.
                // these records need to be added to the HH
                if (pdwPcIds[dwPcIndex] != (DWORD)-1){
                    retval = m_dbPC->FindRecByID(pdwPcIds[dwPcIndex], palmRec);
                    if (!retval) {
                        DWORD dwId = palmRec.GetID();
                        DWORD dwIndex = (DWORD)palmRec.GetIndex();

                        retval = m_dbHH->AddNewRecord(palmRec);
                        if ((!retval) && (dwId != palmRec.GetID())){
                            // change the id of the PC record
                            retval = m_dbPC->ModifyRecByIndex(dwIndex, palmRec);
                        }
                    }
                    
                }
            }
        }
    }

    delete [] pdwHhIds;
    delete [] pdwPcIds;

    return(retval);
}


/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  IsInDb()
 *
 * Description: This method checks to see if a record ID exists in a record
 *              ID array.
 *
 * Parameter(s): pdwIdArray - a pointer to a DWORD array containing record Ids.
 *               dwArraySize - a DWORD specifying the number of DWORDs in the record array.
 *               dwIdToFind - a DWORD specifying the record ID to find
 *
 * Return Value(s): TRUE - the record ID was found in the list.
 *                  FALSE - the record ID was not found in the list.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
BOOL CSynchronizer::IsInDb(DWORD *pdwIdArray, DWORD dwArraySize, DWORD dwIdToFind)
{
    for (DWORD dwIndex = 0; dwIndex < dwArraySize; dwIndex++){
        if (pdwIdArray[dwIndex] == dwIdToFind){
            // set the record ID to -1 so we can later look for non -1 records to find
            // which ones were on the other side.
            pdwIdArray[dwIndex] = (DWORD)-1;
            return TRUE;
        }
    }
    return FALSE;
}
