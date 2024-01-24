/*****************************************************************************
 *
 * Generic Conduit CSynchronizer Src File
 *
 * Subfile - AppInfo/Category synchronization
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
#ifndef macintosh
#define ASSERT(f)          ((void)0)
#endif
#define TRACE0(sz)
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
#include <CPDbBMgr.h>
#include <hhmgr.h>
#include <pcmgr.h>
#include <Pgenerr.h>
#include <gensync.h>

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  SynchronizeAppInfo()
 *
 * Description: This method is used by slow/fast sync to synchronize the
 *              information contained in the AppInfo section on the
 *              Palm organizer.
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
long CSynchronizer::SynchronizeAppInfo(void)
{
    long                retval = 0;
    
    if ((!m_dbHH->IsAppInfoSupported()) || 
        (!m_dbPC->IsAppInfoSupported()))
        return retval;

    retval = m_dbHH->PreAppInfoSync();
    if (retval) {
        if (retval == GEN_ERR_CATEGORIES_NOT_SUPPORTED)
            return 0;
        return retval;
    }

    retval = m_dbPC->PreAppInfoSync();
    if (retval) {
        if (retval == GEN_ERR_CATEGORIES_NOT_SUPPORTED)
            return 0;
        return retval;
    }
    if ((m_dbHH->AreCategoriesSupported()) && 
        (m_dbPC->AreCategoriesSupported())) {
        retval = SynchronizeCategories();
        if (retval)
            return retval;
    }


    // need to do the rest of the synchronization of the app info block here.
    // look at the other part of the app info block for more data
    if (m_dbHH->IsExtraAppInfo()){

        if (m_dbPC->IsExtraAppInfo()){
            // TODO: need to synchronize this data
            DWORD dwPcSize = m_dbPC->GetExtraAppInfoSize();
            DWORD dwHhSize = m_dbHH->GetExtraAppInfoSize();
            if (dwPcSize != dwHhSize) {
                // For this example, the HH info will always override the PC info if they are different

                m_dbPC->SetExtraAppInfo(dwHhSize, m_dbHH->GetExtraAppInfo());
            } else {

                if (memcmp(m_dbHH->GetExtraAppInfo(), m_dbPC->GetExtraAppInfo(), dwHhSize)){
                // For this example, the HH info will always override the PC info if they are different
                    m_dbPC->SetExtraAppInfo(dwHhSize, m_dbHH->GetExtraAppInfo());
                }
            }

        } else { // no extra info on PC side, add HH info
            m_dbPC->SetExtraAppInfo(m_dbHH->GetExtraAppInfoSize(), m_dbHH->GetExtraAppInfo());
        }
    } else if (m_dbPC->IsExtraAppInfo()){ // no current info on HH side, add PC info
        m_dbHH->SetExtraAppInfo(m_dbPC->GetExtraAppInfoSize(), m_dbPC->GetExtraAppInfo());
    }

    // store the changes back into the app info area on the HH
    retval = m_dbHH->PostAppInfoSync();
    if (retval) {
        return retval;
    }

    // store the changes back into the app info area on the PC
    retval = m_dbPC->PostAppInfoSync();
    if (retval) {
        return retval;
    }

    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  SynchronizeCategories()
 *
 * Description: This method performs all the actions necessary to
 *          synchronize category information between the PC and the 
 *          Palm organizer.
 *
 * Steps to Synchronize Categories:
 *      1. Setup the sync cases. This entails looking at what type of a
 *          sync is being performed( ie slow/ fast).
 *      2. Removal of deleted categories from both the HH category list and
 *          and the PC category list.
 *          If a category exists on one database and is not dirty and 
 *          does not exist in the other database, then it has most likely
 *          been deleted from the other database so delete it from
 *          this database.
 *      3. General synchronization of categories.
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
long CSynchronizer::SynchronizeCategories(void)
{
    long retval = 0;

    if (!(m_dwDatabaseFlags & GENERIC_FLAG_CATEGORY_SUPPORTED))
        return retval;

    CPCategoryMgr *m_pHHCatMgr = m_dbHH->m_pCatMgr;
    CPCategoryMgr *m_pPCCatMgr = m_dbPC->m_pCatMgr;
    ASSERT(m_pHHCatMgr);
    ASSERT(m_pPCCatMgr);

    if (*m_pPCCatMgr == *m_pHHCatMgr)
        return 0;

    retval = SetupCategorySyncCases();    
    if (retval)
        return retval;
    retval = RemoveDeletedCategories(m_pPCCatMgr, 
                                     m_pHHCatMgr, 
                                     FALSE);
    if (retval)
        return retval;

    retval = RemoveDeletedCategories(m_pHHCatMgr, 
                                     m_pPCCatMgr, 
                                     TRUE);
    if (retval)
        return retval;

    retval = SynchronizeHHCategories();
    if (retval)
        return retval;

    retval = SynchronizePCCategories();
    if (retval)
        return retval;

    if (m_bPcMoveList) {
        // iterate through all pc records and change the categories if applicable.
        retval = m_dbPC->MoveRecordCategories(m_pcMoveList);
    }
    // add all the hh cats to the pc
    if (!retval) {
        CPCategory *hhCat;



        hhCat = m_pHHCatMgr->FindFirst();
        while ((!retval) && (hhCat)) {
            m_pPCCatMgr->Add(*hhCat);
            hhCat = m_pHHCatMgr->FindNext();
        }
    }


    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:
 *
 * Description:
 *      1. Setup the sync cases. This entails looking at what type of a
 *          sync is being performed( ie slow/ fast).
 *      2. Removal of deleted categories from both the HH category list and
 *          and the PC category list.
 *          If a category exists on one database and is not dirty and 
 *          does not exist in the other database, then it has most likely
 *          been deleted from the other database so delete it from
 *          this database.
 *      3. General synchronization of categories.
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
long CSynchronizer::SetupCategorySyncCases(void)    
{
    long retval = 0;
    // slow / fast
    // If the PC is a virgin, then set all the HH category flags so
    // those HH categories get added and not deleted
    if (m_rSyncProperties.m_FirstDevice == ePC) {
        m_dbHH->m_pCatMgr->SetAllModified();
    
    // Profile scenario.  The HH is has been profiled, and is treated
    // as a new device, but a slow sync was called for since the profile
    // acts as if the HH was synced with another PC.
    } else if ((m_rSyncProperties.m_FirstDevice == eHH) 
          && (m_rSyncProperties.m_SyncType == eSlow) ) {
        m_dbPC->m_pCatMgr->SetAllModified();
        m_dbHH->m_pCatMgr->SetAllModified();
    
    
    // If the HH is a virgin, then set all the PC category flags so
    // those PC categories get added and not deleted
    } else if (m_rSyncProperties.m_FirstDevice == eHH) {
        m_dbPC->m_pCatMgr->SetAllModified();
    
    // If this is a slow sync that means that the last sync was with
    // a different PC, so set the category dirty flags on the HH
    // because they were cleared in the last sync.
    } else if (m_rSyncProperties.m_SyncType == eSlow) {
        m_dbHH->m_pCatMgr->SetAllModified();
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  RemoveDeletedCategories()
 *
 * Description: If a category exists on one database and is not dirty and 
 *          does not exist in the other database, then it has most likely
 *          been deleted from the other database so delete it from
 *          this database.
 *
 * Parameter(s):    pRemoveFrom - A pointer to a category manager object
 *                      specifying the category list to remove deleted categories from.
 *                  pExistList - a pointer to a category manager object specifying
 *                      the category list to check for the existance of categories.
 *                  bHH - a boolean specifying whether pRemoveFrom is the HH DB or
 *                      the PC DB.
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
long CSynchronizer::RemoveDeletedCategories( CPCategoryMgr *pRemoveFrom, 
                                             CPCategoryMgr *pExistList, 
                                             BOOL bHH)
{
    CPCategory            *fromCat;
    CPCategory            *existCat;
    long                retval = 0;
    // First search through the pRemoveFrom categories and delete the categories that
    // were deleted from the pExistList and need to be deleted from the pRemoveFrom.
    // A category needs to be deleted when the category exists on one side
    // and not the other and the dirty flag is off.
    if (m_rSyncProperties.m_FirstDevice == eHH) {
        return retval;
    }

    DWORD dwIndex;
    fromCat = pRemoveFrom->FindFirst();
    while ((!retval) && (fromCat != NULL)) {
        if (!fromCat->IsDirty()) {

            existCat = pExistList->FindName(fromCat->GetName());
            if (!existCat) {    
                // HH catName does not exist on PC and the HH is not a new device
                existCat = pExistList->FindID(fromCat->GetID());
                if (!existCat) {
                    // The category has been deleted on one side, so it needs 
                    // to be deleted on the other side.  Also, change all the 
                    // records under that category to Unfiled.
                    if (bHH){
                        retval = m_dbHH->ChangeCategory(fromCat->GetIndex(), 
                                                        CATEGORY_UNFILED);
                    } else {
                        retval = m_dbPC->ChangeCategory( fromCat->GetIndex(), 
                                                         CATEGORY_UNFILED);
                    }
                    if (!retval){
                        dwIndex = fromCat->GetIndex();
						if(bHH)
							m_pLog->CategoryDeleted(fromCat->GetName(), !bHH);
                        retval = pRemoveFrom->DeleteByIndex(dwIndex);
                    }

                    /*
                    // Move records to unfiled - Report to Log
                    if (!retval) {
                        m_pLog->CategoryDeleted(fromCat->GetName(), bRemote);
                            // Data Subsc :  for subsc cats, sub info has already been removed
                        pRemoveFrom->DeleteByIndex(fromCat->GetIndex());
                    } else {
                        m_pLog->BadChangeCategory(fromCat->GetName(), bRemote);
                    }
                    */
                }  // else is handled by the local
            } // else is handled by the local
        } // who handles this?
        fromCat = pRemoveFrom->FindNext();
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  SynchronizeHHCategories()
 *
 * Description: This method iterates through the HH categories and
 *          synchronizes them against the PC.
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
long CSynchronizer::SynchronizeHHCategories(void) 
{
    CPCategory             *hhCat;
    CPCategory             *pcCat;
    CPCategory             *oldpcCat;
    long                    retval = 0;

    // Merge the PC categories into the HH categories
    CPCategoryMgr *m_pHHCatMgr = m_dbHH->m_pCatMgr;
    CPCategoryMgr *m_pPCCatMgr = m_dbPC->m_pCatMgr;



    hhCat = m_pHHCatMgr->FindFirst();
    while ((!retval) && (hhCat)) {

        pcCat = m_pPCCatMgr->FindName(hhCat->GetName() );
        if (pcCat) {// hh catName exists on pc
            if (pcCat->GetIndex() == hhCat->GetIndex()) {// catIndexes are same
                if (pcCat->GetID() == hhCat->GetID()) {// catId are same
                    // no work needed
                    m_pPCCatMgr->DeleteByIndex(pcCat->GetIndex());

                } else if( (oldpcCat = m_pPCCatMgr->FindID(hhCat->GetID())) != NULL) {
                    // see if there is an old pc cat with matching ID
                    // in case this was a rename
                    retval = m_dbPC->ChangeCategory(oldpcCat->GetIndex(), 
                                                    hhCat->GetIndex());

                    // Move records - Report to Log
                    if (! retval) {
						// 01/02/2001 KRM: The category really hasn't been deleted, 
						// it has been merged by the renaming of the category on the device.
						// Developer: If you want users to know you have done this merge,
						// log an appropriate error. Else, you do not need to display
						// any error message. Fix for bug 29253
                        //m_pLog->CategoryDeleted(oldpcCat->GetName(), LOG_PC_ERROR);

                        // Delete the old category
                        m_pPCCatMgr->DeleteByIndex(oldpcCat->GetIndex());

                    } else {
                        m_pLog->BadChangeCategory(oldpcCat->GetName(), LOG_PC_ERROR);
                    }
                    m_pPCCatMgr->DeleteByIndex(pcCat->GetIndex());
                } else { // use the HH's ID
                    m_pPCCatMgr->DeleteByIndex(pcCat->GetIndex());
                }
            } else // indexes are different
                if( (oldpcCat = m_pPCCatMgr->FindID(hhCat->GetID())) != NULL) {
                // Check for an id on the pc in case this was a rename

                // move oldpccat to hhcat and move pccat to hhcat
                retval = MoveLocalCat(pcCat, hhCat);
                retval = MoveLocalCat(oldpcCat, hhCat);

                // delete the old pc cat
				// 01/02/2001 KRM: The category really hasn't been deleted, 
				// it has been merged by the renaming of the category on the device.
				// Developer: If you want users to know you have done this merge,
				// log an appropriate error. Else, you do not need to display
				// any error message. Fix for bug 29253
                //m_pLog->CategoryDeleted(oldpcCat->GetName(), LOG_PC_ERROR);

                // Delete the old category
                m_pPCCatMgr->DeleteByIndex(oldpcCat->GetIndex());


                m_pPCCatMgr->DeleteByIndex(pcCat->GetIndex());
            } else {
                // indexes are different
                // move to hh index
                retval = MoveLocalCat(pcCat, hhCat);

                // make sure the PC cat id is the same as the hh.
                m_pPCCatMgr->DeleteByIndex(pcCat->GetIndex());

            }
        } else // name doesn't exist
        if ((pcCat = m_pPCCatMgr->FindID(hhCat->GetID())) != NULL) { 
            // hh catName does not exist on pc so we see if hh catID exists on pc
            
            // If hh category was renamed, then replace the PC
            // catName with the hh catName
            if (hhCat->IsDirty() || (!pcCat->IsDirty())) {
                // do nothing
            } else {
                hhCat->SetName(pcCat->GetName());
                m_pHHCatMgr->SetChanged();
            } 
            if (pcCat->GetIndex() == hhCat->GetIndex()) {
                // no work here
            } else { // If the indexes are different, change the PC catIndex
                retval = MoveLocalCat(pcCat, hhCat);
            }
            m_pPCCatMgr->DeleteByIndex(pcCat->GetIndex());

        } else { // new category on the hh or a strange, but possible rename case.
            // Else, the pc category may have already been sync-ed and
            // removed from the list of need to be synced pc categories.
            // Look in the HH cat list for another with the same
            // name.
            // Bug 27977
            DWORD dwCurrentIndex = hhCat->GetIndex();
            CPCategory *pCat;
            CPString cshhName = hhCat->GetName();
            BOOL bFound = FALSE;
            for (DWORD dwIndex = 0; (dwIndex < dwCurrentIndex) && (!bFound); dwIndex++) {
                pCat = m_pHHCatMgr->GetByIndex(dwIndex);
                if (pCat != NULL) {
                    if (cshhName == pCat->GetName()) {
                        m_dbHH->ChangeCategory(hhCat->GetIndex(), pCat->GetIndex());
                        // delete the old pc cat
                        m_pHHCatMgr->DeleteByIndex(hhCat->GetIndex());
                        bFound = TRUE;
                    }
                }
            }
            



            // or 
            // do nothing
        } 
        hhCat = m_pHHCatMgr->FindNext();
    }
    return (retval);
}                 

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  SynchronizePCCategories()
 *
 * Description: This method iterates through the remaining PC categories and
 *          Adds them to the HH.
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
long CSynchronizer::SynchronizePCCategories(void) 
{
    CPCategory             *pcCat;
    long                    retval = 0;
    DWORD dwIndex;
    DWORD dwOldIndex;

    // Merge the PC categories into the HH categories
    CPCategoryMgr *m_pHHCatMgr = m_dbHH->m_pCatMgr;
    CPCategoryMgr *m_pPCCatMgr = m_dbPC->m_pCatMgr;

    // since we deleted all the hh synced categories, only new PC categories exist.
    // 
    pcCat = m_pPCCatMgr->FindFirst();
    while ((!retval) && (pcCat)) {
         // new category on the Pc, see if the index slot is open
        // no category in the index slot on the pc
        dwIndex = m_pHHCatMgr->FindFirstEmpty();
       
        if (dwIndex == (DWORD)CAT_NO_INDEX) {
            m_pLog->TooManyCats(pcCat->GetName(), LOG_PC_ERROR);
            // Set Pc catIndex to unfiled (0)
            retval = m_dbPC->ChangeCategory(pcCat->GetIndex(), CATEGORY_UNFILED);
            if (!retval){
                retval = m_pPCCatMgr->DeleteByIndex(pcCat->GetIndex());

                // Move records to unfiled - Report to Log
                m_pLog->CategoryDeleted(pcCat->GetName(), LOG_PC_ERROR);
            }
		} else {
			dwOldIndex = pcCat->GetIndex();
			pcCat->SetIndex(dwIndex);
			retval = m_pHHCatMgr->Add(*pcCat);
			if (!retval) {
				retval = MoveLocalCat(dwOldIndex, dwIndex);
			}
			if (!retval) {
				retval = m_pPCCatMgr->DeleteByIndex(dwOldIndex);
			}
		}
        if (!retval) {
            pcCat = m_pPCCatMgr->FindNext();
        }

    }
    return (retval);
}                 

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  MoveLocalCat()
 *
 * Description: This method build a list of category index moves to be made against the
 *              record database
 *
 * Parameter(s): pcCat - a CPCategory object used to specify the index to move from.
 *               hhCat - a CPCategory object used to specify the index to move to.
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
long CSynchronizer::MoveLocalCat(CPCategory *pcCat, CPCategory *hhCat)
{

    m_pcMoveList[pcCat->GetIndex()] = hhCat->GetIndex();
    m_bPcMoveList = TRUE;

    return 0;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  MoveLocalCat()
 *
 * Description: This method build a list of category index moves to be made against the
 *              record database
 *
 * Parameter(s): dwOldIndex - a DWORD value used to specify the index to move from.
 *               dwNewIndex - a DWORD value used to specify the index to move to.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  01/09/00    KRM         initial revision
 *
 *****************************************************************************/
long CSynchronizer::MoveLocalCat(DWORD dwOldIndex, DWORD dwNewIndex)
{

    m_pcMoveList[dwOldIndex] = dwNewIndex;
    m_bPcMoveList = TRUE;

    return 0;
}
