/*****************************************************************************
 *
 * Generic Conduit CNNsiMonaPCMgr Src File
 *
 ****************************************************************************/
#define ASSERT(f)          ((void)0)
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
#include <sys/stat.h>
#include <TCHAR.H>
#include <syncmgr.h>

#include "NNsiMonaPCMgr.h"
#include "resource.h"

#ifndef NO_HSLOG
extern HANDLE hLangInstance;
#endif

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
 *
 * Method:  Constructor
 *
 * Description: This method initializes the class object.
 *
 * Parameter(s): szDbName - a pointer to a character buffer specifying the 
 *                          name of the database being synced.
 *               pFileName - a pointer to a character buffer specifying the 
 *                          filename of the database.
 *               pDirName - a pointer to a character buffer specifying the 
 *                          directory where the database is stored.
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

CNNsiMonaPCMgr::CNNsiMonaPCMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName, eSyncTypes syncType)  
: CPcMgr(pLogging, dwGenericFlags, szDbName, pFileName, pDirName, syncType)
{

    m_dwDBVersion = NNsiMona_OBJECT_VERSION;

    
    // Initialize the TableString to an empty string
	// The TableString is some state information written by Palm Desktop
	// software. It is not used by the handheld. Just initialize it
	// to an empty string and the Desktop will handle it. Normally, the
	// Desktop would create this string, but the conduit may end up creating
	// it, in which case it should be an empty string.
	m_csTableString = "";
	
	m_Schema.lResourceID = mpSCHEMAResource;
    m_Schema.lFieldsPerRow = mpFLDCount;
    m_Schema.lRecordIdPos  = 0;
    m_Schema.lRecordStatusPos = 1;
    m_Schema.lPlacementPos = 2;
    m_Schema.wFieldCounts = mpFLDCount;
    m_Schema.wFieldArray[0] = eInteger;
    m_Schema.wFieldArray[1] = eInteger;
    m_Schema.wFieldArray[2] = eInteger;
    m_Schema.wFieldArray[3] = eString;
    m_Schema.wFieldArray[4] = eBool;
    m_Schema.wFieldArray[5] = eInteger;

 
    m_pCatMgr = new CPCategoryMgr();


}


CNNsiMonaPCMgr::~CNNsiMonaPCMgr() 
{
    TRACE0("CNNsiMonaPCMgr Destructor\n");
}



/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
 *
 * Method:  RetrieveDB()
 *
 * Description: This method loads the database from file.
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
long CNNsiMonaPCMgr::RetrieveDB(void)
{
    //Empty();

    m_bNeedToSave = FALSE;
    if (!_tcslen(m_szDataFile))
        return GEN_ERR_INVALID_DB_NAME;

    if (m_hFile == INVALID_HANDLE_VALUE)
        return GEN_ERR_INVALID_DB;

    BOOL bDone = FALSE;
    long retval = 0;



    retval = CreateDBInfo();
    
    if (retval){
        CloseDB();
        return retval;
    }

    // read in file version
    DWORD dwDBVersion;
    retval = ReadSerializedDword( dwDBVersion);
    if (retval) {
        CloseDB();
        if (retval == GEN_ERR_STORAGE_EOF)
            return 0; // no error
        return GEN_ERR_READING_DBINFO;
    }

    if (dwDBVersion != m_dwDBVersion) {
        CloseDB();
        return GEN_ERR_DB_VERSION_MISMATCH;
    }


    // get file name
    retval = ReadSerializedString(m_csDbFile);
    if (retval) {
        CloseDB();
        return retval;
    }
    // get TableString   
	// The TableString defines some state information for the Palm Desktop.
	// Just read it in and leave it unchanged.
    retval = ReadSerializedString(m_csTableString);
    if (retval) {
        CloseDB();
        return retval;
    }

    // Read in category info
    retval = LoadCategories();
    if (retval) {
        CloseDB();
        return retval;
    }
    // Read in the Schema
    retval = LoadSchema();
    if (retval) {
        CloseDB();
        return retval;
    }

    // load the records
    retval = LoadRecords();
    if (retval) {
        CloseDB();
        return retval;
    }


    CloseDB();
    m_bNeedToSave = FALSE;
    return retval;
}

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
 *
 * Method:  StoreDB()
 *
 * Description: This method is called to store changes back into the 
 *          storage meduim.
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
long CNNsiMonaPCMgr::StoreDB(void)
{
    if ( !m_bNeedToSave) { // if no changes, don't waste time saving
        if ((!m_pCatMgr) || (!m_pCatMgr->IsChanged()))
            return 0; 
    }
    long retval = OpenDB();
    if (retval)
        return GEN_ERR_UNABLE_TO_SAVE;

    // Write the Version
    retval = WriteSerializedDword(m_dwDBVersion);
    if (retval) {
        CloseDB();
        return retval;
    }

    // Write file name
    retval = WriteSerializedString(m_szDataFile);
    if (retval) {
        CloseDB();
        return retval;
    }
    // Write TableString   
    retval = WriteSerializedString(m_csTableString);
    if (retval) {
        CloseDB();
        return retval;
    }

    // Write category info
    retval = StoreCategories();
    if (retval) {
        CloseDB();
        return retval;
    }
    // Write the Schema
    retval = StoreSchema();
    if (retval) {
        CloseDB();
        return retval;
    }
    // Write the records
    retval = StoreRecords();



    CloseDB();
    m_bNeedToSave = FALSE;
    return 0;
}



long CNNsiMonaPCMgr::LoadCategories()
{
    long retval;
    int iLen;

    if (m_pCatMgr)
        delete m_pCatMgr;

    m_pCatMgr = new CPCategoryMgr();
    if (!m_pCatMgr)
        return GEN_ERR_LOW_MEMORY;

    // 1st. Cat Mgr info
    // Next add ID Long
    retval = ReadSerializedDword(m_pCatMgr->m_dwNextFreeID);
    if (retval)
        return retval;

    // Count of categories - 1 // also less unfiled.
    DWORD dwCatCount=0;
    retval = ReadSerializedDword(dwCatCount);
    if (retval)
        return retval;

    // add unfiled to category mgr
    CPCategory cat;
    cat.SetIndex(0);
    cat.SetID(0);
    cat.SetName("Unfiled");

    retval = m_pCatMgr->Add(cat);
    if (retval)
        return retval;

    // categories
    DWORD dwUtility;
    TCHAR szUtility[256];
    for (int iLoop = 0; iLoop < LOWORD(dwCatCount); iLoop++){
        // index - long
        retval = ReadSerializedDword(dwUtility);
        if (retval)
            return retval;

        cat.SetIndex(dwUtility);

        // id - long
        retval = ReadSerializedDword(dwUtility);
        if (retval)
            return retval;

        cat.SetID(dwUtility);

        // Dirty - long
        retval = ReadSerializedDword(dwUtility);
        if (retval)
            return retval;

    
        if (dwUtility)
            cat.SetDirty(TRUE);
        else
            cat.SetDirty(FALSE);

        // name - string
        iLen = sizeof(szUtility);
        retval = ReadSerializedString(szUtility, &iLen);
        if (retval) {
            return retval;
        }
        cat.SetName(szUtility);
        // file name - string
        iLen = sizeof(szUtility);
        retval = ReadSerializedString(szUtility, &iLen);
        if (retval) {
            return retval;
        }

        cat.SetFileName(szUtility);
        retval = m_pCatMgr->Add(cat);
        if (retval)
            return retval;
    }
    m_pCatMgr->SetChanged(FALSE);
    return retval;
    
}

long CNNsiMonaPCMgr::LoadSchema(void)
{
    long retval;
    // Resource ID - long
    retval = ReadSerializedDword( (DWORD)m_Schema.lResourceID);
    if (retval) {
        return retval;
    }

    // FieldsPerRow - long
    retval = ReadSerializedDword( (DWORD)m_Schema.lFieldsPerRow);
    if (retval) {
        return retval;
    }

    // RecordIdPos - long
    retval = ReadSerializedDword( (DWORD)m_Schema.lRecordIdPos);
    if (retval) {
        return retval;
    }

    // RecordStatusPos - long
    retval = ReadSerializedDword( (DWORD)m_Schema.lRecordStatusPos);
    if (retval) {
        return retval;
    }

    // PlacementPos - long
    retval = ReadSerializedDword( (DWORD)m_Schema.lPlacementPos);
    if (retval) {
        return retval;
    }


    // Array Field Types 
    // count - WORD
    retval = ReadSerializedWord( m_Schema.wFieldCounts);
    if (retval) {
        return retval;
    }
    // count * sizeof(word)
    for (BYTE iLoop = 0; (iLoop < m_Schema.wFieldCounts) && (!retval); iLoop++){
        retval = ReadSerializedWord( m_Schema.wFieldArray[iLoop]);

    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
 *
 * Method:  ConvertGenericToPc()
 *
 * Description: This method converts records from the PC storage type to the 
 *          CPalmRecord record type.
 *
 * Parameter(s): palmRec - a CPalmRecord object to be converted.
 *               pRec - a pointer to a StorageRecordType record to receive the converted data.
 *               bClearAttributes - a boolean specifying whether the attributes should be kept(FALSE)
 *                      or cleared (TRUE). The default is TRUE.
 *               
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  04/07/98    KRM         initial revision
 *
 *****************************************************************************/
long CNNsiMonaPCMgr::ConvertGenericToPc(CPalmRecord &palmRec, CNNsiMonaRecord &rec, BOOL bClearAttributes)
{ 
   long                retval = 0;
    BYTE                *pBuff;
    int iLen;

    rec.Reset();

    if (palmRec.GetRawDataSize() == 0){
        // then this is a deleted record because it has no data
        return GEN_ERR_EMPTY_RECORD;
    }


    rec.m_dwRecordID = palmRec.GetID();

    if (bClearAttributes)
        rec.m_dwStatus = 0;
    else {
        // TODO
        /*
        if (rInfo.m_Attribs & ARCHIVE_BIT)
            retval = rec.SetArchiveBit(TRUE);
        else
            retval = rec.SetArchiveBit(FALSE); 

        retval = rec.SetStatus(fldStatusNONE);
        if (rInfo.m_Attribs & DELETE_BIT) // Delete flag
            retval = rec.SetStatus(fldStatusDELETE);
        else if (rInfo.m_Attribs & DIRTY_BIT) // Dirty flag
            retval = rec.SetStatus(fldStatusUPDATE);
            */
        //DWORD dwStatus;             //,            (WORD)eInteger);    
    }


    rec.m_dwPosition = mpDefaultPosition;
    rec.m_dwCategoryID = palmRec.GetCategory();
    rec.m_dwPrivate    = palmRec.IsPrivate();

    DWORD dwRawSize = palmRec.GetRawDataSize();
    if (!dwRawSize) {
        // invalid record
        return 0;
    }

    pBuff = palmRec.GetRawData();
	
    // Convert Memo
	// Add in any necessary CRs
	
	iLen = _tcslen((char *)pBuff);
	
	AddCRs((char *)pBuff, 
		rec.m_csMemo.GetBuffer(iLen * 2), 
		iLen);
	rec.m_csMemo.ReleaseBuffer();
	
	pBuff += iLen + 1;
	
    return retval;

}
long CNNsiMonaPCMgr::ConvertGenericToPc(CPalmRecord &palmRec, MEMORECORD &rec, BOOL bClearAttributes)
{ 
   long                retval = 0;
    BYTE                *pBuff;

    if (palmRec.GetRawDataSize() == 0){
        // then this is a deleted record because it has no data
        return GEN_ERR_EMPTY_RECORD;
    }

    rec.dwRecordID = palmRec.GetID();

    if (bClearAttributes)
        rec.dwStatus = 0;
    else {
        // TODO
        /*
        if (rInfo.Attribs & ARCHIVE_BIT)
            retval = rec.SetArchiveBit(TRUE);
        else
            retval = rec.SetArchiveBit(FALSE); 

        retval = rec.SetStatus(fldStatusNONE);
        if (rInfo.Attribs & DELETE_BIT) // Delete flag
            retval = rec.SetStatus(fldStatusDELETE);
        else if (rInfo.Attribs & DIRTY_BIT) // Dirty flag
            retval = rec.SetStatus(fldStatusUPDATE);
            */
        //DWORD dwStatus;             //,            (WORD)eInteger);    
    }


    rec.dwPosition = mpDefaultPosition;
    rec.dwCategoryID = palmRec.GetCategory();
    rec.dwPrivate    = palmRec.IsPrivate();

    // clear all values in the Memo Record
    rec.szMemo[0] = '\0';

    DWORD dwRawSize = palmRec.GetRawDataSize();
    if (!dwRawSize) {
        // invalid record
        return 0;
    }

    pBuff = palmRec.GetRawData();

    // Convert Memo
	// Add in any necessary CRs
	AddCRs((char *)pBuff, rec.szMemo, _tcslen((char *)pBuff));
	
	pBuff += _tcslen((char *)pBuff) + 1;
	
    return retval;
}

long CNNsiMonaPCMgr::StoreCategories(void)
{
    long retval;
    DWORD dwCatCount=1;
    DWORD dwNextFreeID = 128;

    if (m_pCatMgr) {
        dwCatCount = m_pCatMgr->GetCount();
        dwNextFreeID = m_pCatMgr->m_dwNextFreeID;
    } 

    // 1st. Cat Mgr info
    // Next add ID Long
    retval = WriteSerializedDword(dwNextFreeID);
    if (retval)
        return retval;

    // Count of categories - 1 // also less unfiled.
    if (dwCatCount > 1)
        retval = WriteSerializedDword(dwCatCount - 1);
    else { // only unfiled
        retval = WriteSerializedDword(0);
        return retval;
    }
    if (retval)
        return retval;

    // add unfiled to category mgr
    CPCategory *pCat;

    DWORD dwUtility;
    TCHAR szUtility[256];
    int iSize;
    // 
    for (DWORD dwIndex = 0; (dwIndex < MAX_CATEGORIES) && (!retval); dwIndex++){

        
        pCat = m_pCatMgr->GetByIndex(dwIndex);
        if (!pCat)
            continue;

        if (pCat->GetID() == 0) // "unfiled"
            continue;

        // index - long
        retval = WriteSerializedDword(dwIndex);
        if (retval)
            return retval;

        // id - long
        retval = WriteSerializedDword(pCat->GetID());
        if (retval)
            return retval;

        // Dirty - long
        if (TRUE) { // TODO: If you are using this class to read/write the database
                    // during times other than synchronization, you may want to store
                    // the dirty flags. 
                    // After a synchronization session, you want to remove all the
                    // modification flags.
            dwUtility = 0;
        } else { 
            dwUtility = pCat->IsDirty();
        }
        retval = WriteSerializedDword(dwUtility);
        if (retval)
            return retval;

        // name - string
        iSize = sizeof(szUtility);
        pCat->GetName(szUtility, &iSize);
        retval = WriteSerializedString(szUtility);
        if (retval) {
            return retval;
        }

        iSize = sizeof(szUtility);
        pCat->GetFileName(szUtility, &iSize);
        retval = WriteSerializedString(szUtility);
        if (retval)
            return retval;
    }
    return retval;
    
}

long CNNsiMonaPCMgr::StoreSchema(void)
{
    long retval;

    // Resource ID - long
    retval = WriteSerializedDword(m_Schema.lResourceID);
    if (retval) {
        return retval;
    }

    // FieldsPerRow - long
    retval = WriteSerializedDword(m_Schema.lFieldsPerRow);
    if (retval) {
        return retval;
    }

    // RecordIdPos - long
    retval = WriteSerializedDword(m_Schema.lRecordIdPos);
    if (retval) {
        return retval;
    }

    // RecordStatusPos - long
    retval = WriteSerializedDword(m_Schema.lRecordStatusPos);
    if (retval) {
        return retval;
    }

    // PlacementPos - long
    retval = WriteSerializedDword(m_Schema.lPlacementPos);
    if (retval) {
        return retval;
    }


    // Array Field Types 
    // count - WORD
    retval = WriteSerializedWord(m_Schema.wFieldCounts);
    if (retval) {
        return retval;
    }
    // count * sizeof(word)
    for (BYTE iLoop = 0; (iLoop < m_Schema.wFieldCounts) && (!retval); iLoop++){
        retval = WriteSerializedWord(m_Schema.wFieldArray[iLoop]);

    }
    return retval;
}

long CNNsiMonaPCMgr::StoreRecords(void)
{
    DWORD dwRecordCount = m_dwRecordCount;
    long retval;
	MEMORECORD Rec;

    DWORD dwFldCount;
    // Write in the number of fields
    dwFldCount = dwRecordCount * m_Schema.lFieldsPerRow;
    retval = WriteSerializedDword(dwFldCount);
    if (retval) {
        return retval;
    }


    // Field count is equal to record count * Fields per row.

    CPalmRecord palmRec;
    retval = FindFirstRecByIndex(palmRec);
    DWORD dwCount = 0;

    while (!retval) {
        retval = ConvertGenericToPc(palmRec, Rec);
        if (!retval) {
            retval = StoreRecord(Rec);
            if (retval)
                break;
        } else if (retval == GEN_ERR_EMPTY_RECORD) {
            retval = 0;
        } else {
            break;
        }



        --dwRecordCount;
        if (dwRecordCount == 0)
            break;
        if (++dwCount > m_dwYieldTime) {
#ifndef NO_HSLOG
            SyncYieldCycles(100);
#endif
            dwCount = 0;
        }
        retval = FindNext(palmRec);
    }
    return retval;
}




///=======================================

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
 *
 * Method:  ConvertPCtoGeneric()
 *
 * Description: This method converts records from the PC storage type to the 
 *          CPalmRecord record type.
 *
 * Parameter(s): pRec - a pointer to a StorageRecordType record to be converted.
 *               palmRec - a CPalmRecord object to receive the converted record.
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
long CNNsiMonaPCMgr::ConvertPCtoGeneric(CNNsiMonaRecord &rec, 
                                CPalmRecord &palmRec)
{
  long                retval = 0;
    char                *pBuff;
    int                 destLen;
    BYTE                szRawData[MAX_RECORD_SIZE];
    DWORD               dwRecSize = 0;


    palmRec.SetID(rec.m_dwRecordID);

    palmRec.SetCategory(rec.m_dwCategoryID);

    if (rec.m_dwPrivate)
        palmRec.SetPrivate(TRUE);
    else
        palmRec.SetPrivate(FALSE);

    palmRec.SetArchived((BOOL)(rec.m_dwStatus & fldStatusARCHIVE));
    palmRec.SetDeleted((BOOL)(rec.m_dwStatus & fldStatusDELETE));
    palmRec.SetUpdate((BOOL)(rec.m_dwStatus & fldStatusUPDATE));

    
    
    pBuff = (char*)szRawData;

    // Convert Memo
    int iLen;
    iLen = rec.m_csMemo.length();
    if (iLen > 0){
        // Strip the CR's (if present) places result directly into pBuff
        destLen = StripCRs(pBuff, rec.m_csMemo.c_str(), iLen);  
    
        pBuff += destLen;
        dwRecSize += destLen;
    }

    if (dwRecSize == 0){
        // then this is a deleted record because it has no data
        palmRec.SetDeleted();
    }

    retval = palmRec.SetRawData(dwRecSize, szRawData);

    return(retval);
}

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
 *
 * Method:  ConvertPCtoGeneric()
 *
 * Description: This method converts records from the PC storage type to the 
 *          CPalmRecord record type.
 *
 * Parameter(s): pRec - a pointer to a StorageRecordType record to be converted.
 *               palmRec - a CPalmRecord object to receive the converted record.
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
long CNNsiMonaPCMgr::ConvertPCtoGeneric(MEMORECORD &rec, 
                                CPalmRecord &palmRec)
{
    long                retval = 0;
    char                *pBuff;
    int                 destLen;
    BYTE                szRawData[MAX_RECORD_SIZE];
    DWORD               dwRecSize = 0;


    palmRec.SetID(rec.dwRecordID);

    palmRec.SetCategory(rec.dwCategoryID);

    if (rec.dwPrivate)
        palmRec.SetPrivate(TRUE);
    else
        palmRec.SetPrivate(FALSE);

    palmRec.SetArchived((BOOL)(rec.dwStatus & fldStatusARCHIVE));
    palmRec.SetDeleted((BOOL)(rec.dwStatus & fldStatusDELETE));
    palmRec.SetUpdate((BOOL)(rec.dwStatus & fldStatusUPDATE));

    
    
    pBuff = (char*)szRawData;

    // Convert Memo
    int iLen;
    iLen = _tcslen(rec.szMemo);
    if (iLen > 0){
        // Strip the CR's (if present) places result directly into pBuff
        destLen = StripCRs(pBuff, rec.szMemo, iLen);  
    
        pBuff += destLen;
        dwRecSize += destLen;
    }

    
    if (dwRecSize == 0){
        // then this is a deleted record because it has no data
        palmRec.SetDeleted();
    }

    retval = palmRec.SetRawData(dwRecSize, szRawData);

    return(retval);
}

long CNNsiMonaPCMgr::LoadRecords(void)
{
    DWORD dwRecordCount = 0;
    long retval;
    CNNsiMonaRecord Rec;
    DWORD dwFldCount;
    DWORD dwBadRecCount = 0;
    // read in the number of fields
    retval = ReadSerializedDword( dwFldCount);
    if (retval) {
        return retval;
    }


    // record count is equal to Field count / Fields per row.
    dwRecordCount = dwFldCount / m_Schema.lFieldsPerRow;

    DWORD dwCaseSensitive;

    DWORD dwCount = 0;
	
    for (DWORD dwLoop=0; (dwLoop < dwRecordCount) && (!retval); dwLoop++){
        for (WORD wFld=0; (wFld < m_Schema.lFieldsPerRow) && (!retval); wFld++) {
            if (retval) 
                break;
            
            switch (wFld) {
			case mpFLDRecordID:
				retval = ReadSerializedFieldValue(Rec.m_dwRecordID, wFld); 
				break;
			case mpFLDStatus:
				retval = ReadSerializedFieldValue(Rec.m_dwStatus, wFld); 
				break;
			case mpFLDPosition:
				retval = ReadSerializedFieldValue(Rec.m_dwPosition, wFld); 
				break;
			case mpFLDMemo:
				retval = ReadSerializedFieldValue(Rec.m_csMemo, 
					dwCaseSensitive, 
					wFld);
				break;                   
			case mpFLDPrivate:
				retval = ReadSerializedFieldValue(Rec.m_dwPrivate, wFld); 
				break;
			case mpFLDCategoryID:
				retval = ReadSerializedFieldValue(Rec.m_dwCategoryID, wFld); 
				break;
			default:
				retval = 0;
				break;
            }
        }
        if (!retval) { // add the record
            retval = AddCustomRec(Rec);
        }
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
 *
 * Method:  AddRec()
 *
 * Description: This protected function converts records from the PC format to
 *          the generic CPalmRecord format and then adds the records to the database
 *          in memory.
 *
 * Parameter(s): pRec - a pointer to a StorageRecordType record to be added.
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
long CNNsiMonaPCMgr::AddCustomRec(CNNsiMonaRecord &rec)
{
    long retval = 0;
    CPalmRecord palmRec;

    retval = ConvertPCtoGeneric(rec, palmRec);
    if (retval)
        return retval;

    if (m_SyncType == eSlow) { // then set the rec to pending
        // The pending flag is use to tell which records have been synced from the
        // hh side. If, after syncing all HH recs, there are some PC pending recs, they will
        // need to be synchronized.
        palmRec.SetPending();
    }

    retval = AddRec(palmRec);
    return retval;
}

long CNNsiMonaPCMgr::AddCustomRec(MEMORECORD &rec)
{
    long retval = 0;
    CPalmRecord palmRec;

    retval = ConvertPCtoGeneric(rec, palmRec);
    if (retval)
        return retval;

    if (m_SyncType == eSlow) { // then set the rec to pending
        // The pending flag is use to tell which records have been synced from the
        // hh side. If, after syncing all HH recs, there are some PC pending recs, they will
        // need to be synchronized.
        palmRec.SetPending();
    }

    retval = AddRec(palmRec);
    return retval;
}
/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
 *
 * Method:  AllocateAppInfo()
 *
 * Description: This method is used to allocate memory for the storage of AppInfo
 *          data.
 *
 * Parameter(s):  dwRawDataSize - a DWORD specifying how much memory to 
 *                      allocate for the raw data of the AppInfo block.
 *                bClearData - a BOOLean specifying whether to memset the appInfo
 *                      block after allocating it.
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
long CNNsiMonaPCMgr::AllocateAppInfo(DWORD dwRawDataSize, BOOL bClearData)
{
    return 0;
}


/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
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
BOOL CNNsiMonaPCMgr::IsExtraAppInfo(void)
{
	// TODO - return TRUE if you have extra data in the appInfo block
    return FALSE;
}



/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
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
long CNNsiMonaPCMgr::SetExtraAppInfo(DWORD dwSize, BYTE *pData)
{
    long retval = 0;
    return retval;
}

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
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
DWORD CNNsiMonaPCMgr::GetExtraAppInfoSize(void)
{
	// TODO - return the size of the extra AppInfo bock information 
	return 0;
}

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
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
long CNNsiMonaPCMgr::GetExtraAppInfo(BYTE *pBuf, DWORD *pdwBufSize)
{
	// TODO - return a buffer and its size that contains the non-category info in the AppInfo block
    return 0;
}

/******************************************************************************
 *
 * Class:   CNNsiMonaPCMgr
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
BYTE* CNNsiMonaPCMgr::GetExtraAppInfo(void)
{
        return NULL;

}

long CNNsiMonaPCMgr::ReadSerializedFieldValue(CPString &csValue, 
                                      DWORD &dwCaseSensitive, 
                                      WORD wIndex)
{
    BYTE bLen;
    int iLen = 0;
    long retval;

    DWORD dwFieldType;

    csValue.Empty();

    // load the field type
    retval = ReadSerializedDword( dwFieldType);

    if ((dwFieldType != m_Schema.wFieldArray[wIndex]) || 
        (dwFieldType != eString)) {
        return GEN_ERR_INVALID_DB;
    }

    retval = ReadSerializedDword( dwCaseSensitive);
    if (retval) {
        return retval;
    }

    retval = ReadInData( (LPVOID)&bLen, sizeof(bLen));
    if (retval) {
        return retval;
    }

    if (bLen == 0xff) {
        // string size is longer than ff
        // read in the WORD size
        WORD wLen;
        retval = ReadSerializedWord( wLen);
        if (retval)
            return retval;
        iLen = (int)wLen;
    } else {
        iLen = (int)bLen;
    }


    // get file name string
    if (iLen) {
        retval = ReadInData( (LPVOID)csValue.GetBuffer(iLen + 2), iLen);
        csValue.ReleaseBuffer();
    }

    return retval;
}


long CNNsiMonaPCMgr::ReadSerializedFieldValue(TCHAR *pStrBuf, 
                                      int *piSize, 
                                      DWORD &dwCaseSensitive, 
                                      WORD wIndex)
{
    BYTE bLen;
    int iLen = 0;
    long retval;

    DWORD dwFieldType;

    // load the field type
    retval = ReadSerializedDword( dwFieldType);

    if ((dwFieldType != m_Schema.wFieldArray[wIndex]) || 
        (dwFieldType != eString)) {
        return GEN_ERR_INVALID_DB;
    }

    *pStrBuf = '\0';
    retval = ReadSerializedDword( dwCaseSensitive);
    if (retval) {
        return retval;
    }

    retval = ReadInData( (LPVOID)&bLen, sizeof(bLen));
    if (retval) {
        return retval;
    }

    if (bLen == 0xff) {
        // string size is longer than ff
        // read in the WORD size
        WORD wLen;
        retval = ReadSerializedWord( wLen);
        if (retval)
            return retval;
        iLen = (int)wLen;
    } else {
        iLen = (int)bLen;
    }
    if (iLen > *piSize) {
        *piSize = iLen;
        // seek ahead so that we can read the rest of the file.
        SetFilePointer( m_hFile, iLen, NULL, FILE_CURRENT); 
        return GEN_ERR_BUFFER_TOO_SMALL;
    }

    // get file name string
    retval = ReadInData( (LPVOID)pStrBuf, iLen);
    if (!retval) {
        *piSize = iLen;
        *(pStrBuf + iLen) = '\0';
    }

    return retval;
}


long CNNsiMonaPCMgr::ReadSerializedFieldValue(DWORD &dwValue, 
                                      WORD wIndex)
{
    int iLen = 0;
    long retval;

    DWORD dwFieldType;

    // load the field type
    retval = ReadSerializedDword( dwFieldType);

    if ((dwFieldType != m_Schema.wFieldArray[wIndex]) || 
        ((dwFieldType != eInteger) && (dwFieldType != eBool))) {
        return GEN_ERR_INVALID_DB;
    }


    retval = ReadSerializedDword( dwValue);

    return retval;
}



long CNNsiMonaPCMgr::StoreRecord(CNNsiMonaRecord &Rec)
{
    long retval = 0;
	
    for (WORD wFld=0; (wFld < m_Schema.lFieldsPerRow) && (!retval); wFld++) {
        switch (wFld) {
		case mpFLDRecordID:
			retval = WriteSerializedIntegerField(Rec.m_dwRecordID); 
			break;
		case mpFLDStatus:
			retval = WriteSerializedIntegerField(Rec.m_dwStatus); 
			break;
		case mpFLDPosition:
			retval = WriteSerializedIntegerField(Rec.m_dwPosition); 
			break;
		case mpFLDMemo:
			retval = WriteSerializedStringField(Rec.m_csMemo);
			break;
		case mpFLDPrivate:
			retval = WriteSerializedBoolField(Rec.m_dwPrivate); 
			break;
		case mpFLDCategoryID:
			retval = WriteSerializedIntegerField(Rec.m_dwCategoryID); 
			break;
		default:
			retval = 0;
			break;
        }
        if (retval) { 
            return retval;
        }
    }
    return retval;
}

long CNNsiMonaPCMgr::StoreRecord(MEMORECORD &Rec)
{
  long retval = 0;
	
    for (WORD wFld=0; (wFld < m_Schema.lFieldsPerRow) && (!retval); wFld++) {
        switch (wFld) {
		case mpFLDRecordID:
			retval = WriteSerializedIntegerField(Rec.dwRecordID); 
			break;
		case mpFLDStatus:
			retval = WriteSerializedIntegerField(Rec.dwStatus); 
			break;
		case mpFLDPosition:
			retval = WriteSerializedIntegerField(Rec.dwPosition); 
			break;
		case mpFLDMemo:
			retval = WriteSerializedStringField(Rec.szMemo);
			break;
		case mpFLDPrivate:
			retval = WriteSerializedBoolField(Rec.dwPrivate); 
			break;
		case mpFLDCategoryID:
			retval = WriteSerializedIntegerField(Rec.dwCategoryID); 
			break;
		default:
			retval = 0;
			break;
        }

        if (retval) { 
            return retval;
        }
    }
    return retval;
}


