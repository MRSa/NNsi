/*****************************************************************************
 *
 * Generic Conduit CPcMgr Src File
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
#include <MoreFiles.h>
#include <FSPCompat.h>
#include <FileCopy.h>
#include <Script.h>

#include "StCurResChain.h"
extern SInt16 gNonLocalizedRsrcFileRefNum;
#endif

#ifndef macintosh
#include <sys/stat.h>
#include <TCHAR.H>
#endif


#include <syncmgr.h>
#include <pcmgr.h>
#include <Pgenerr.h>

extern HINSTANCE hLangInstance;

/******************************************************************************
 *
 * Class:   CPcMgr
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
#ifndef NO_HSLOG
CPcMgr::CPcMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, 
	TCHAR *pFileName, void *pDirInfo, eSyncTypes syncType)  
: CPDbBaseMgr(dwGenericFlags, syncType)
#else
CPcMgr::CPcMgr(DWORD dwGenericFlags, char *szDbName, 
	TCHAR *pFileName, void *pDirInfo, eSyncTypes syncType)  
: CPDbBaseMgr(dwGenericFlags, syncType)
#endif
{
#ifndef macintosh
    TRACE0("CPcMgr Constructor\n");

	TCHAR *pDirName = (TCHAR *)pDirInfo;
    m_hFile = INVALID_HANDLE_VALUE;
    if ((szDbName)  && (_tcslen(szDbName))){
        _tcscpy(m_szDbName, szDbName);
    } else {
        *m_szDbName = '\0';
    }
    if ((pDirName)   && (_tcslen(pDirName))){
        int iLen = _tcslen(pDirName);
        _tcscpy(m_szDataFile, pDirName);
        if (*(pDirName + iLen - 1) != '\\'){
            // append a slash
            _tcscat(m_szDataFile, "\\");
        }

        if ((pFileName)  && (_tcslen(pFileName))){
            _tcscat(m_szDataFile, pFileName);
        } else {
            _tcscat(m_szDataFile, "GenericDB.DAT");
        }
    } else {
        if ((pFileName)  && (_tcslen(pFileName))){
            _tcscpy(m_szDataFile, pFileName);
        } else {
            *m_szDataFile = '\0';
        }
    }
#else
	int errCode = noErr;
	char fileName[256];
	
    if ((pFileName)  && (_tcslen(pFileName)))
   		_tcscpy(fileName, pFileName);
    else
        _tcscpy(fileName, "GenericDB.DAT");

    m_hFile = INVALID_HANDLE_VALUE;
    if ((szDbName)  && (_tcslen(szDbName)))
        _tcscpy(m_szDbName, szDbName);
    else 
        *m_szDbName = '\0';

	FSSpec *fsSpec = (FSSpec *)pDirInfo;

	// Save volume/parent/name information
	m_VolumeID = fsSpec->vRefNum;
	m_ParentID = fsSpec->parID;
	m_ParName[1] = '\:';
	memcpy((void *)&(m_ParName[2]), (void *)&(fsSpec->name[1]), fsSpec->name[0]); 
	m_ParName[0] = fsSpec->name[0] + 1;

	_tcscpy((char *)&(m_FileName[1]), (const char *)fileName);
	m_FileName[0] = (unsigned char)_tcslen(fileName);
#endif    

    m_pDBInfo = NULL;
    m_pSortInfo = NULL;
    m_pAppInfo = NULL;
    m_bNeedToSave = FALSE;
    m_bAlreadyLoaded = FALSE;
    m_bStartSync    = TRUE;
#ifndef NO_HSLOG
	m_pLog = pLogging;
#endif
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  Constructor
 *
 * Description: This method initializes the class object.
 *
 * Parameter(s): 
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
CPcMgr::CPcMgr()  
: CPDbBaseMgr()
{
    TRACE0("CPcMgr Constructor\n");

    m_hFile = INVALID_HANDLE_VALUE;
    
    *m_szDbName = '\0';
 #ifndef macintosh
    *m_szDataFile = '\0';
 #else
	m_VolumeID = 0;
	m_ParentID = 0;
	*m_ParName = '\0';
	*m_FileName = '\0';
 #endif
    m_pDBInfo = NULL;
    m_pSortInfo = NULL;
    m_pAppInfo = NULL;
    m_bNeedToSave = FALSE;
    m_bAlreadyLoaded = FALSE;
    m_bStartSync    = TRUE;
#ifndef NO_HSLOG
	m_pLog = NULL;
#endif
}
/******************************************************************************
 *
 * Class:   CPcMgr
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
CPcMgr::~CPcMgr()
{
    TRACE0("CPcMgr Destructor\n");


    if (m_hFile != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }

    if (m_pDBInfo)
    {
        free( m_pDBInfo);
        m_pDBInfo = NULL;
    }
    if (m_pSortInfo) {
        free( m_pSortInfo);
        m_pSortInfo = NULL;
    }

    if (m_pAppInfo) 
    {
        free( m_pAppInfo);
        m_pAppInfo = NULL;
    }

}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  Empty()
 *
 * Description: This method closes the open database and cleans up allocated
 *              data objects.
 *
 * Parameter(s): None
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
void CPcMgr::Empty(void)
{
    Close();
    FreeRecords();

    if (m_pDBInfo){
        free( m_pDBInfo);
        m_pDBInfo = NULL;
    }
    if (m_pSortInfo) {
        free( m_pSortInfo);
        m_pSortInfo = NULL;
    }

    if (m_pAppInfo) {
        free( m_pAppInfo);
        m_pAppInfo = NULL;
    }
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::Open(void)
{
    long retval = 0;
    retval = OpenDB();

    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  OpenDB()
 *
 * Description: This protected method opens the database in different modes 
 *          depending on the sync mode and whether this is the before or after
 *          the synchronization of the data.
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
long CPcMgr::OpenDB(void)
{
    long retval = 0;
    switch (m_SyncType) {
        case eFast:
        case eSlow:
            if (m_bStartSync){
                retval = Open(eRead);
                if ((retval) && (retval == GEN_ERR_UNABLE_TO_OPEN_FILE)) {
                    // if we are in slow or fast mode and there is no PC dB, then this is
                    // not really an error, we will just have to create the file on the close.
                    return GEN_ERR_NO_PC_DB;
                }
            } else {
                retval = Open(eCreate);
				m_bNeedToSave = TRUE;
            }
            break;
        case eHHtoPC:
        case eBackup: 
            retval = Open(eCreate);
			m_bNeedToSave = TRUE;
            break;
        case eInstall:
        case eProfileInstall:
			retval = Open(eRead);
            break;
        case ePCtoHH:
            if (m_bStartSync){
				retval = Open(eRead);
			} else {
				retval = Open(eCreate); // This was done to allow new records (rec's with
										// a record id of 0) to have the handheld assigned
										// record id reflected back into the pc based database.
				m_bNeedToSave = TRUE;
			}
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
 * Class:   CPcMgr
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
long CPcMgr::Open(eOpenDbType mode)
{
    long retval = 0;

    Close();

#ifndef macintosh
    switch (mode) {
        case eRead:
            m_hFile = CreateFile(m_szDataFile,
                               GENERIC_READ,
                               0,
                               NULL, //&secAttributes,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
            break;
        case eWrite:
            m_hFile = CreateFile(m_szDataFile,
                               GENERIC_WRITE,
                               0,
                               NULL, //&secAttributes,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
            break;
        case eReadWrite:
            m_hFile = CreateFile(m_szDataFile,
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               NULL, //&secAttributes,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
            if (m_hFile != INVALID_HANDLE_VALUE)
                break;
        case eCreate:
            m_hFile = CreateFile(m_szDataFile,
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               NULL, 
                               CREATE_ALWAYS ,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
            break;
        default:
            break;
    }
    if (m_hFile == INVALID_HANDLE_VALUE) {
        retval = GEN_ERR_UNABLE_TO_OPEN_FILE;
    }
#else

	FSSpec fsSpec;
	Str255 fileName;
	
	// Copy the parent folder name
	memcpy(fileName, m_ParName, sizeof(Str63));
	
	// Append the separator
	fileName[m_ParName[0]+1] = ':';
	
	fileName[m_ParName[0]+2] = '\0';
	
	// Append the filename to the end
	_tcscat((char *)fileName, (const char *)&(m_FileName[1]));
	
	// Update the length of the string
	fileName[0] = (unsigned char)(m_FileName[0] + m_ParName[0] + 1);
	
	int errCode = FSMakeFSSpec(m_VolumeID, m_ParentID, (unsigned char *)fileName, &fsSpec);
	
    switch (mode)
    {
        case eRead:
        	if(errCode != noErr)
        		break;
			FSpOpenDF(&fsSpec, fsRdPerm, &m_hFile); 
            break;
        
        case eWrite:
        	if(errCode != noErr)
        		break;
			FSpOpenDF(&fsSpec, fsWrPerm, &m_hFile); 
            break;
        
        case eReadWrite:
        	if(errCode != noErr)
        		break;
			FSpOpenDF(&fsSpec, fsRdWrPerm, &m_hFile); 
			break;
			
        case eCreate:
        	fsSpec.parID = m_ParentID;
        	fsSpec.vRefNum = m_VolumeID;
        	memcpy(fsSpec.name, fileName, sizeof(Str63));

			errCode = FSMakeFSSpec(fsSpec.vRefNum, fsSpec.parID, fileName, &fsSpec);
			if(errCode == noErr)
			{
				// Ensure that the file does not exist
				FSpDeleteCompat(&fsSpec);
			}
			
			if(errCode == fnfErr || errCode == noErr)
			{
				StCurResChain saveRes(gNonLocalizedRsrcFileRefNum);
				char creatorType[255] = "";
				char fileType[255] = "";
				StringPtr pString;
				StringHandle hString = ::GetString(kConduitCreatorTypeString);
				if (hString) {
					pString = *hString;
					CopyPascalStringToC(pString, creatorType);
					::ReleaseResource((Handle) hString); 
				}
				
				hString = ::GetString(kConduitFileTypeString);
				if (hString) {
					pString = *hString;
					CopyPascalStringToC(pString, fileType);
					::ReleaseResource((Handle) hString);
				}

				if ((creatorType && fileType)  && strlen(creatorType) && strlen(fileType)) 
					errCode = FSpCreateCompat(&fsSpec, *(OSType*) creatorType, *(OSType*) fileType, smRoman);
				else
					errCode = FSpCreateCompat(&fsSpec, 'gcnd', 'DATA', smRoman);
				if(errCode == noErr)
					FSpOpenDF(&fsSpec, fsRdWrPerm, &m_hFile); 
			}
            break;
        
        default:
            break;
    }
    if (m_hFile == INVALID_HANDLE_VALUE) 
        retval = GEN_ERR_UNABLE_TO_OPEN_FILE;
#endif
    
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::Close(BOOL bDontUpdate)
{
    long retval = 0;

    retval = CloseDB(bDontUpdate);
    if (retval) {
        m_pLog->BadCloseDB(retval);
    }

    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  CloseDB()
 *
 * Description: This method closes the Database file handle if it is open
 *
 * Parameter(s): bDontUpdate - an ignored boolean.
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
long CPcMgr::CloseDB(BOOL bDontUpdate)
{
    long retval = 0;
    if (m_hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hFile);
        m_hFile = INVALID_HANDLE_VALUE;
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::GetRecordCount(DWORD &dwRecCount)
{
    long retval = 0;
    dwRecCount = m_dwRecordCount;
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::SetRecordCount(DWORD dwRecCount)
{
    long retval = GEN_ERR_NOT_SUPPORTED;
    return retval;
}


/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::FindRecByID(DWORD dwID, CPalmRecord &palmRec, BOOL bLogError)
{
    long retval = CPDbBaseMgr::FindRecByID(dwID, palmRec);
    if (!retval) {
        m_pRecordList[palmRec.GetIndex()]->SetPending(FALSE); // clear the pending flag
    }
    return retval;
}



/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::DeleteCategory(DWORD dwCategory, BOOL bMoveToUnfiled)
{
    long retval = 0;

    if ((dwCategory < 0) || (dwCategory >= MAX_CATEGORIES))
        return CAT_ERR_INDEX_OUT_OF_RANGE;

    if (!bMoveToUnfiled) {
        for (DWORD dwIndex = 0; dwIndex < m_dwMaxRecordCount; dwIndex++) {
            if ((m_pRecordList[dwIndex]) &&
                (m_pRecordList[dwIndex]->GetCategory() == dwCategory)) {
                delete m_pRecordList[dwIndex];
                m_pRecordList[dwIndex] = NULL;
                --m_dwRecordCount;
            }
        }
    } else {
        retval = ChangeCategory(dwCategory, 0);
    }
    m_bNeedToSave = TRUE;
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::ChangeCategory(DWORD dwOldCatIndex, DWORD dwNewCatIndex)
{
    long retval = 0;

    if ((dwOldCatIndex < 0) || (dwOldCatIndex >= MAX_CATEGORIES))
        return CAT_ERR_INDEX_OUT_OF_RANGE;

    if ((dwNewCatIndex < 0) || (dwNewCatIndex >= MAX_CATEGORIES))
        return CAT_ERR_INDEX_OUT_OF_RANGE;
    
    for (DWORD dwIndex = 0; dwIndex < m_dwMaxRecordCount; dwIndex++) {
        if ((m_pRecordList[dwIndex]) &&
            (m_pRecordList[dwIndex]->GetCategory() == dwOldCatIndex)) {
            m_pRecordList[dwIndex]->SetCategory(dwNewCatIndex);
            m_pRecordList[dwIndex]->SetUpdate();
        }
    }
    m_bNeedToSave = TRUE;
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::AddRec(CPalmRecord &palmRec)
{
    long retval;
    retval = CPDbBaseMgr::AddRec(palmRec);
    m_bNeedToSave = TRUE;
    return retval;
}

    // Write Functions
/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::AddRec(StorageRecordType *pRec)
{
    long retval = 0;
    CPalmRecord palmRec;

    retval = ConvertPCtoGeneric(pRec, palmRec);
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
 * Class:   CPcMgr
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
long CPcMgr::ConvertGenericToPc(CPalmRecord *pPalmRec, BYTE *pBinary, BOOL bClearAttributes)
{
    DWORD dwRecSize;
    StorageRecordType *pRecord;
    BYTE *pData;
    long retval = 0;

    pRecord = (StorageRecordType *)pBinary;

    dwRecSize = pRecord->dwStructSize - CM_STORAGE_BUFFER_OFFSET;
    pData = (BYTE *)((BYTE *)pRecord + CM_STORAGE_BUFFER_OFFSET);
    retval = pPalmRec->GetRawData(pData, &dwRecSize);
    if (retval) {
        return retval;
    }


    pRecord->wAttribs = 0;    
    if (!bClearAttributes) {
        if (pPalmRec->IsUpdate())
            pRecord->wAttribs |= ATTRIB_UPDATE;    
        if (pPalmRec->IsDeleted())
            pRecord->wAttribs |= ATTRIB_DELETE;    
        if (pPalmRec->IsArchived())
            pRecord->wAttribs |= ATTRIB_ARCHIVE;    
        if (pPalmRec->IsNew())
            pRecord->wAttribs |= ATTRIB_NEW;    

        if (pPalmRec->IsPrivate()) {
            // bug in not saving the private attribute to file
            pRecord->wAttribs |= ATTRIB_PRIVATE;    
        }
    }

    pRecord->dwRecId                = pPalmRec->GetID();    
    pRecord->dwCatIndex             = pPalmRec->GetCategory();    
    pRecord->dwRecSize              = dwRecSize;    
    pRecord->dwStructVersion        = CM_STORAGE_VERSION_2;    
    pRecord->dwRawDataOffset        = CM_STORAGE_BUFFER_OFFSET;
    return retval;

}
/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::ConvertPCtoGeneric(StorageRecordType *pRec, CPalmRecord &palmRec)
{
    long retval = 0;

    ASSERT(pRec);

    BYTE *pData = (BYTE*)((BYTE*)pRec + pRec->dwRawDataOffset);
    palmRec.SetID(pRec->dwRecId);
    palmRec.SetCategory(pRec->dwCatIndex);
    if ((pRec->dwRecSize > 0) && (pData)) {
        palmRec.SetRawData(pRec->dwRecSize, pData);
    }

    palmRec.SetIndex((DWORD)-1);

    palmRec.ResetAttribs();
    if (pRec->wAttribs & ATTRIB_UPDATE)
        palmRec.SetUpdate();

    if (pRec->wAttribs & ATTRIB_DELETE)
        palmRec.SetDeleted();

    if (pRec->wAttribs & ATTRIB_ARCHIVE)
        palmRec.SetArchived();

    if (pRec->wAttribs & ATTRIB_NEW)
        palmRec.SetNew();

    // bug in not retrieving the private attribute from file
    palmRec.SetPrivate((BOOL)(pRec->wAttribs & ATTRIB_PRIVATE));

    return retval;
}


/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::ModifyRec(CPalmRecord &palmRec)
{
    long retval = 0;

    DWORD dwIndex = CPDbBaseMgr::FindRecByID(palmRec.GetID());

    if (dwIndex == (DWORD)-1)
        return GEN_ERR_RECORD_NOT_FOUND;


    ASSERT(m_pRecordList[dwIndex] != NULL);

    m_bNeedToSave = TRUE;
    m_pRecordList[dwIndex]->SetPending(FALSE); // clear the pending flag
    retval = m_pRecordList[dwIndex]->Modify(palmRec);
    m_pRecordList[dwIndex]->ResetAttribs();
    return retval;

}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::DeleteRec(CPalmRecord &palmRec)
{
	long retval = 0;
	DWORD dwIndex;
	if (palmRec.GetID() != 0) {
		dwIndex = CPDbBaseMgr::FindRecByID(palmRec.GetID());
	} else {
		// If the record does not have an ID, then delete it by the index
		dwIndex = palmRec.GetIndex();
		
	}

    if (dwIndex == (DWORD)-1)
        return GEN_ERR_RECORD_NOT_FOUND;


    ASSERT(m_pRecordList[dwIndex] != NULL);
	if (m_pRecordList[dwIndex] == NULL) {
        return GEN_ERR_RECORD_NOT_FOUND;
	}
    m_bNeedToSave = TRUE;

    delete m_pRecordList[dwIndex];
    m_pRecordList[dwIndex] = NULL;
    --m_dwRecordCount;

    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::ModifyRecByIndex(DWORD dwIndex, CPalmRecord &palmRec)
{
    long retval = 0;
    if ((dwIndex == (DWORD)-1) || 
        (!m_pRecordList[dwIndex]))
        return GEN_ERR_RECORD_NOT_FOUND;

    retval = m_pRecordList[dwIndex]->Modify(palmRec);
    m_pRecordList[dwIndex]->ResetAttribs();
    m_bNeedToSave = TRUE;
    return retval;
}


   

    // Because New records will mess up fast syncs when writing new records to the Palm
    // organizer, I think we should differentiate between modified records and new records
    // during sync time.
/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  FindFirstNewRec()
 *
 * Description: This method attempts to find the first new(never been synced to the HH 
 *              device) record in the database.
 *
 * Parameter(s): palmRec - a CPalmRecord object to receive the new record data.
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
long CPcMgr::FindFirstNewRec(CPalmRecord& palmRec)
{
    long retval = GEN_ERR_NO_MORE_RECORDS;
    for (DWORD dwIndex = 0; dwIndex < m_dwMaxRecordCount; dwIndex++) {
        if ((m_pRecordList[dwIndex]) &&
            (m_pRecordList[dwIndex]->IsNew())) {
            palmRec = m_pRecordList[dwIndex]; // Fixed 3/25/99 KRM
            return 0;
        }
    }
    return retval;
}


/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::PurgeDeletedRecs(void)
{
    long retval = 0;
    for (DWORD dwIndex = 0; dwIndex < m_dwMaxRecordCount; dwIndex++) {
        if ((m_pRecordList[dwIndex]) &&
            (m_pRecordList[dwIndex]->IsDeleted())) {
            delete m_pRecordList[dwIndex];
            m_pRecordList[dwIndex] = NULL;
            --m_dwRecordCount;
            m_bNeedToSave = TRUE;
        }
    }
    return retval;
}





/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::PreSync(void)
{
    long retval = 0;
    if (m_bAlreadyLoaded)
        return 0;
    m_bAlreadyLoaded = TRUE;
    switch (m_SyncType) {
        case eFast:
        case eSlow:
            retval = RetrieveDB();
            if ((retval ) && (retval == GEN_ERR_INVALID_DB)){
                // if we are in slow or fast mode and there is no PC dB, then this is
                // not really an error, we will just have to create the file on the close.
                retval = GEN_ERR_NO_PC_DB;
            }
            break;
        case eHHtoPC:
        case eBackup:
            break;
        case ePCtoHH:
        case eInstall:
        case eProfileInstall:
            retval = RetrieveDB();
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
 * Class:   CPcMgr
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
long CPcMgr::PostSync(void)
{
    long retval = 0;
    m_bStartSync = FALSE;
	ResetSyncFlags(); // added 12/11/2000 because flags were not being clear in PcToHH cases.
    switch (m_SyncType) {
        case eSlow:
        case eFast:
            retval = StoreDB();
            break;
        case eHHtoPC:
        case eBackup:
        case ePCtoHH: // Fix for problems with duplicate records
            retval = StoreDB();
            break;
        case eInstall:
        case eProfileInstall:
            break;
        case eDoNothing:
            break;
        default:
            break;
    }
    if (retval) {
        m_pLog->BadLocalSave(retval);
    }
    return retval;
}


/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  MoveRecordCategories()
 *
 * Description: This method is called by the synchronization of the categories 
 *      procedure. If Categories have had their indexes moved, these changes
 *      will have to completed on the records on a record by record process.
 *
 * Parameter(s): moveList - a DWORD array of the max number of categories containing
 *                  the category indexes to change to.
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
long CPcMgr::MoveRecordCategories(DWORD *moveList)
{
    long retval = 0;
    DWORD dwCategory;
    if (!moveList)
        return GEN_ERR_INVALID_POINTER;

    m_bNeedToSave = TRUE;

    for (DWORD dwIndex = 0; dwIndex < m_dwMaxRecordCount; dwIndex++) {
        if (m_pRecordList[dwIndex]) {
            dwCategory = m_pRecordList[dwIndex]->GetCategory();
            
            if (moveList[dwCategory] != CAT_NO_INDEX) { // this record has changed categories
                m_pRecordList[dwIndex]->SetCategory(moveList[dwCategory]);
                m_pRecordList[dwIndex]->SetUpdate();
            }

        }
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::PreAppInfoSync(void)
{
    if (!IsAppInfoSupported())
        return 0;

    // need to load the app Info block from the HH and
    // extract the categories.
    long retval = 0;
    retval = PreSync();
    if (!retval)
        retval = ExtractCategories();
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::PostAppInfoSync(void)
{
    long        retval = 0;

    if (!IsAppInfoSupported())
        return 0;

    if (!AreCategoriesSupported())
        return 0;

    if ((!m_pCatMgr) || (!m_pCatMgr->IsChanged()))
        return retval;

    // need to fold changes back into the app info block and then
    // write the block to the PC.
    retval = CompactCategories();

    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::ExtractCategories(void)
{
    long retval = 0;

    if (!AreCategoriesSupported())
        return 0;

    if (m_pCatMgr) {
        delete m_pCatMgr;
        m_pCatMgr = NULL;
    }

    if ((!m_pAppInfo) || (m_pAppInfo->dwBytesRead == 0)){
        return GEN_ERR_NO_CATEGORIES;
    }

    m_pCatMgr = new CPCategoryMgr((BYTE *)((BYTE *)m_pAppInfo + m_pAppInfo->dwRawDataOffset), 
                                 m_pAppInfo->dwBytesRead);
    if (!m_pCatMgr)
        return GEN_ERR_LOW_MEMORY;
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::CompactCategories()
{
    long retval = 0;
    BYTE *pData;

    if (!AreCategoriesSupported())
        return 0;

    if ((!m_pCatMgr) || (!m_pCatMgr->IsChanged()))
        return 0;

    if (!m_pAppInfo ){
        pData = (BYTE *)malloc(CM_MIN_STORAGE_INFO_SIZE + MIN_CATEGORY_BLOCK_SIZE);
        if (!pData)
            return GEN_ERR_LOW_MEMORY;
        memset(pData, 0, CM_MIN_STORAGE_INFO_SIZE + MIN_CATEGORY_BLOCK_SIZE);
        m_pAppInfo                  = (STORAGE_INFO_PTR)pData;
        m_pAppInfo->dwTotalBytes     = MIN_CATEGORY_BLOCK_SIZE;
        m_pAppInfo->dwStructSize    = CM_MIN_STORAGE_INFO_SIZE + MIN_CATEGORY_BLOCK_SIZE;
        m_pAppInfo->wVersion        = CM_STORAGE_VERSION_2;
        m_pAppInfo->dwRawDataOffset = CM_INFO_BUFFER_OFFSET;

        _tcscpy(m_pAppInfo->fileName, m_szDbName);  // Name of remote database file 
        m_pAppInfo->dwBytesRead     = 0;             // Inbound byte count
        m_pAppInfo->dwReserved      = 0;             // Reserved - set to NULL    
    }
    DWORD dwRecSize;
    BYTE *pAppInfo;

    pAppInfo = (BYTE*)m_pAppInfo;
    pData = (BYTE *)(pAppInfo + CM_INFO_BUFFER_OFFSET);

    dwRecSize = m_pAppInfo->dwTotalBytes;

    retval = m_pCatMgr->Compact(pData, &dwRecSize);
    if (!retval) {

        if (!m_pAppInfo->dwBytesRead)
            m_pAppInfo->dwBytesRead     = dwRecSize; 
    }

    m_bNeedToSave = TRUE;
    return retval;
}



/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::FindNextRecByIndex(CPalmRecord &palmRec)
{
    long retval = GEN_ERR_NO_MORE_RECORDS;
    
    while (m_dwCurrentFindIndex < m_dwMaxRecordCount) {
        if (m_pRecordList[m_dwCurrentFindIndex]) {
            palmRec = *m_pRecordList[m_dwCurrentFindIndex];
            retval = 0;
            ++m_dwCurrentFindIndex;
            return retval;
        }
        ++m_dwCurrentFindIndex;
    }

    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::FindNextModifiedRec(CPalmRecord &palmRec)
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
 * Class:   CPcMgr
 *
 * Method:  FindNextPendingRec()
 *
 * Description: This method attempts to find the next pending record 
 *      in the database.
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
long CPcMgr::FindNextPendingRec(CPalmRecord &palmRec)
{
    long retval= GEN_ERR_NO_MORE_RECORDS;
    
    while (m_dwCurrentFindIndex < m_dwMaxRecordCount) {
        if ((m_pRecordList[m_dwCurrentFindIndex]) && 
            (m_pRecordList[m_dwCurrentFindIndex]->IsPending())){
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
 * Class:   CPcMgr
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
long CPcMgr::FindNextRecInCat(CPalmRecord &palmRec)
{
    long retval = GEN_ERR_NO_MORE_RECORDS;

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
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::FindNextModRecInCat(CPalmRecord &palmRec)
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
 * Class:   CPcMgr
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
long CPcMgr::RetrieveDB(void)
{
    //Empty();

    m_bNeedToSave = FALSE;
#ifndef macintosh    
    if (!_tcslen(m_szDataFile))
        return GEN_ERR_INVALID_DB_NAME;
#else
	if(!_tcslen((const char *)m_FileName))
		return(GEN_ERR_INVALID_DB_NAME);
#endif


    if (m_hFile == INVALID_HANDLE_VALUE)
        return GEN_ERR_INVALID_DB;

    // 1st. retrieve file header info
    // 2nd. retrieve data
    StorageRecordType tempRec;
    DWORD dwNumberOfBytesToRead;
    DWORD dwNumberOfBytesRead;
    BOOL bDone = FALSE;
    DWORD dwAllocate;
    long retval = 0;

    retval = CreateDBInfo();
    
    if (retval){
        CloseDB();
        return retval;
    }

    // get main db info
    retval = ReadInData((LPVOID)m_pDBInfo, CM_STORAGE_HEADER_SIZE);

    if (retval) {
        CloseDB();
        if (retval == GEN_ERR_STORAGE_EOF)
            return 0; // no error, no archive file has been created yet.
        return GEN_ERR_READING_DBINFO;
    } 

    if (m_pDBInfo->dwAppInfo > 0) {
        m_pAppInfo = (STORAGE_INFO_PTR)malloc(m_pDBInfo->dwAppInfo);
        if (!m_pAppInfo)
            retval =  GEN_ERR_LOW_MEMORY;
        if (!retval) {
            retval = ReadInData((LPVOID)m_pAppInfo, 
                                m_pDBInfo->dwAppInfo);
        }
        if (retval) {
            CloseDB();
            return GEN_ERR_READING_APPINFO;
        }
        //if (m_pAppInfo->dwBytesRead == 0 ) {
        //    m_pAppInfo->dwBytesRead = m_pAppInfo->dwTotalBytes;
        //}
    }

    if (m_pDBInfo->dwSortInfo > 0) {
        m_pSortInfo = (STORAGE_INFO_PTR)malloc(m_pDBInfo->dwSortInfo);
        if (!m_pAppInfo)
            retval =  GEN_ERR_LOW_MEMORY;
        if (!retval) {
            retval = ReadInData((LPVOID)m_pSortInfo, 
                                 m_pDBInfo->dwSortInfo);
        }
        if (retval) {
            CloseDB();
            return GEN_ERR_READING_SORTINFO;
        }
    }

    STORAGE_RECORD_PTR pRecord;
    pRecord = (STORAGE_RECORD_PTR)malloc(MAX_RECORD_SIZE);
    if (!pRecord) {
        CloseDB();
        return GEN_ERR_LOW_MEMORY;
    }
    
    while ((!bDone) && (m_pDBInfo->lDBRecCount > (long)m_dwRecordCount)){

        dwNumberOfBytesToRead = CM_MIN_STORAGE_SIZE;

        retval = ReadInData(&tempRec,    // pointer to data to write to file 
                              dwNumberOfBytesToRead);
        if (retval) {
            bDone = TRUE;
            retval = GEN_ERR_READING_RECORD;
        } else {
            // determine how much more data to read
            dwAllocate = tempRec.dwStructSize;

            ASSERT(dwAllocate < MAX_RECORD_SIZE);

            memset(pRecord, 0, MAX_RECORD_SIZE);
            memcpy(pRecord, &tempRec, CM_MIN_STORAGE_SIZE);
            dwNumberOfBytesRead = dwAllocate - CM_MIN_STORAGE_SIZE; 
            retval = ReadInData((BYTE *)pRecord + CM_STORAGE_BUFFER_OFFSET,    // pointer to data to write to file 
                                 &dwNumberOfBytesRead);
            if (retval) {
                bDone = TRUE;
            } else {
                pRecord->dwRecSize = dwNumberOfBytesRead;
                AddRec(pRecord);
            }
        }

    }

    free(pRecord);

    CloseDB();
    m_bNeedToSave = FALSE;
    return retval;
}
/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  ReadInData()
 *
 * Description: This method is used to load data in from files.
 *
 * Parameter(s): hFile - the HANDLE of the file to read.
 *               pBuffer - a pointer to a buffer to receive the read in data.
 *               dwNumberOfBytesToRead - a DWORD specifying how much data to 
 *                      attempt to read.
 *
 * Return Value(s): 0>= - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CPcMgr::ReadInData(LPVOID pBuffer, DWORD dwNumberOfBytesToRead)
{
#ifdef macintosh
    long lNumBytesRead = (long)dwNumberOfBytesToRead;
	short errCode = FSRead(m_hFile, 
                           (long *)&lNumBytesRead, 
                           pBuffer);
	if (errCode != noErr)
        return GEN_ERR_READING_DATA;
		
	return 0;
#else

    long retval = 0;
    BOOL bSuccess;
    DWORD dwNumberOfBytesRead;

    bSuccess = ReadFile(m_hFile,    // handle to file to write to 
                        (LPVOID)pBuffer,    // pointer to data to write to file 
                         dwNumberOfBytesToRead,    // number of bytes to write 
                         &dwNumberOfBytesRead,    // pointer to number of bytes written 
                         NULL);
    if (!bSuccess)
        return GEN_ERR_READING_DATA;

    if (dwNumberOfBytesToRead != dwNumberOfBytesRead){
        if (!dwNumberOfBytesRead) {
            return GEN_ERR_STORAGE_EOF;
        } 
        return GEN_ERR_READING_NOT_ENOUGH;
    }

    return 0;
#endif
}
long CPcMgr::ReadInData(LPVOID pBuffer, DWORD *pdwNumBytes)
{
#ifdef macintosh
    long lNumBytesRead = (long)*pdwNumBytes;
	short errCode = FSRead(m_hFile, 
                           (long *)&lNumBytesRead, 
                           pBuffer);
	if (errCode != noErr)
        return GEN_ERR_READING_DATA;
		
    *pdwNumBytes = (DWORD)lNumBytesRead;
	return 0;
#else

    long retval = 0;
    BOOL bSuccess;
    DWORD dwNumberOfBytesRead;

    bSuccess = ReadFile(m_hFile,    // handle to file to write to 
                        (LPVOID)pBuffer,    // pointer to data to write to file 
                         *pdwNumBytes,    // number of bytes to write 
                         &dwNumberOfBytesRead,    // pointer to number of bytes written 
                         NULL);
    if (!bSuccess)
        return GEN_ERR_READING_DATA;

    if (*pdwNumBytes != dwNumberOfBytesRead){
        if (!dwNumberOfBytesRead) {
            return GEN_ERR_STORAGE_EOF;
        } 
        return GEN_ERR_READING_NOT_ENOUGH;
    }

    *pdwNumBytes = dwNumberOfBytesRead;
    return 0;
#endif
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  CreateDBInfo()
 *
 * Description: This method creates an empty database Info block in memory.
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
long CPcMgr::CreateDBInfo(void)
{
    m_pDBInfo = (STORAGE_HEADER_PTR)malloc(CM_STORAGE_HEADER_SIZE);
    if (!m_pDBInfo){
        return GEN_ERR_LOW_MEMORY;
    }
    memset(m_pDBInfo, 0, CM_STORAGE_HEADER_SIZE);
    m_pDBInfo->dwStructSize = CM_STORAGE_HEADER_SIZE;
    return 0;
}
/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::StoreDB(void)
{
    if ( !m_bNeedToSave) { // if no changes, don't waste time saving
        if ((!m_pCatMgr) || (!m_pCatMgr->IsChanged()))
            return 0; 
    }


    long retval = OpenDB();
    if (retval)
        return GEN_ERR_UNABLE_TO_SAVE;

    // 1st. Store file header info
    // 2nd. store data
    if (!m_pDBInfo){
        CloseDB();
        return 0; // no error if nothing to store.
    }

    if (m_pDBInfo){
        m_pDBInfo->lDBRecCount = (long)m_dwRecordCount;
        if (m_pAppInfo)
            m_pDBInfo->dwAppInfo = m_pAppInfo->dwStructSize;
        else 
            m_pDBInfo->dwAppInfo = 0;

        if (m_pSortInfo)
            m_pDBInfo->dwSortInfo = m_pSortInfo->dwStructSize;
        else 
            m_pDBInfo->dwSortInfo = 0;

        retval = WriteOutData(m_pDBInfo,    
                             m_pDBInfo->dwStructSize);
        if (retval != 0){
            CloseDB();
            return GEN_ERR_UNABLE_TO_SAVE;
        }
    }

    if (m_pAppInfo) {

        retval = WriteOutData(m_pAppInfo,    
                                m_pAppInfo->dwStructSize);
        if (retval != 0){
            CloseDB();
            return GEN_ERR_UNABLE_TO_SAVE;
        }
    }

    STORAGE_RECORD_PTR pRecord;
    BYTE *pBinary;
    pBinary = (BYTE *)malloc(MAX_RECORD_SIZE);
    if (!pBinary) {
        CloseDB();
        return GEN_ERR_LOW_MEMORY;
    }

    pRecord = (STORAGE_RECORD_PTR)pBinary;
    for (DWORD dwIndex = 0; (dwIndex < m_dwMaxRecordCount) && (!retval); dwIndex++){
        if (!m_pRecordList[dwIndex]) // if there is no record, skip ahead
            continue;

        memset(pBinary, 0, MAX_RECORD_SIZE);
        pRecord->dwStructSize           = MAX_RECORD_SIZE;    

        retval = ConvertGenericToPc(m_pRecordList[dwIndex], pBinary);
        if (retval) {
            break;
        }

        // reset the record size so we dont write all the unused data space.
        pRecord->dwStructSize           = CM_STORAGE_BUFFER_OFFSET + pRecord->dwRecSize;    
        retval = WriteOutData(pBinary,    
                              pRecord->dwStructSize);
        if (retval){
            CloseDB();
            free(pBinary);
            return GEN_ERR_UNABLE_TO_SAVE;
        }
    }
    free(pBinary);


    CloseDB();
    m_bNeedToSave = FALSE;
    return 0;
}
/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::GetAppInfo(CDbGenInfo &rInfo)
{
    if (!m_pAppInfo)
        return GEN_ERR_NO_APP_INFO;

    _tcscpy(rInfo.m_FileName, m_pAppInfo->fileName);  // Name of remote database file 
    rInfo.m_BytesRead  = LOWORD(m_pAppInfo->dwBytesRead);             // Inbound byte count
    if (rInfo.m_TotalBytes < rInfo.m_BytesRead) // Byte length of 'pBytes'
        return GEN_ERR_BUFFER_TOO_SMALL;

    if (!rInfo.m_pBytes)
        return GEN_ERR_INVALID_POINTER; 

    rInfo.m_dwReserved = m_pAppInfo->dwReserved;

    BYTE *pData = (BYTE *)((BYTE *)m_pAppInfo + m_pAppInfo->dwRawDataOffset);
    memcpy(rInfo.m_pBytes, pData , rInfo.m_BytesRead);

    return 0;

}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::SetAppInfo(CDbGenInfo &rInfo)
{
    m_bNeedToSave = TRUE;

    if (m_pAppInfo)
        free( m_pAppInfo);

    DWORD dwSize = CM_MIN_STORAGE_INFO_SIZE;
    dwSize += rInfo.m_BytesRead;

    m_pAppInfo = (STORAGE_INFO_PTR)malloc(dwSize);
    if (!m_pAppInfo)
        return GEN_ERR_LOW_MEMORY;

    m_pAppInfo->dwStructSize        = dwSize;
    m_pAppInfo->wVersion            = CM_STORAGE_VERSION_2;
    m_pAppInfo->dwRawDataOffset     = CM_INFO_BUFFER_OFFSET;

    _tcscpy( m_pAppInfo->fileName, rInfo.m_FileName);  // Name of remote database file 
    m_pAppInfo->dwTotalBytes        = dwSize - CM_INFO_BUFFER_OFFSET;            // Byte length of 'pBytes'

    m_pAppInfo->dwBytesRead         = rInfo.m_BytesRead ;             // Inbound byte count
    m_pAppInfo->dwReserved          = rInfo.m_dwReserved;

    BYTE *pData = (BYTE *)((BYTE *)m_pAppInfo + m_pAppInfo->dwRawDataOffset);

    memcpy(pData, rInfo.m_pBytes, rInfo.m_BytesRead);
    return 0;
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  GetSortInfo()
 *
 * Description: This method returns SortInfo data in the provided structure.
 *
 * Parameter(s): rInfo - a CDbGenInfo object used to receive the SortInfo data.
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
long CPcMgr::GetSortInfo(CDbGenInfo &rInfo)
{
    if (!m_pSortInfo)
        return GEN_ERR_NO_SORT_INFO;

    _tcscpy(rInfo.m_FileName, m_pSortInfo->fileName);  // Name of remote database file 

    rInfo.m_BytesRead  = LOWORD(m_pSortInfo->dwBytesRead);             // Inbound byte count

    if (rInfo.m_TotalBytes < rInfo.m_BytesRead) // Byte length of 'pBytes'
        return GEN_ERR_BUFFER_TOO_SMALL;

    if (!rInfo.m_pBytes)
        return GEN_ERR_INVALID_POINTER; 

    rInfo.m_dwReserved = m_pSortInfo->dwReserved;

    BYTE *pData = (BYTE *)((BYTE *)m_pSortInfo + m_pSortInfo->dwRawDataOffset);
    memcpy(rInfo.m_pBytes, pData, rInfo.m_BytesRead);

    return 0;
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  SetSortInfo()
 *
 * Description: This method is used to change the SortInfo data of this database.
 *
 * Parameter(s): rInfo - a CDbGenInfo object used to specify the new SortInfo data.
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
long CPcMgr::SetSortInfo(CDbGenInfo &rInfo)
{
    m_bNeedToSave = TRUE;

    if (m_pSortInfo)
        free( m_pSortInfo);

    DWORD dwSize = CM_MIN_STORAGE_INFO_SIZE;
    dwSize += rInfo.m_BytesRead;

    m_pSortInfo= (STORAGE_INFO_PTR)malloc(dwSize);
    if (!m_pSortInfo)
        return GEN_ERR_LOW_MEMORY;

    m_pSortInfo->dwStructSize       = dwSize;            // Byte length of 'pBytes'
    m_pSortInfo->wVersion           = CM_STORAGE_VERSION_2;
    m_pSortInfo->dwRawDataOffset    = CM_INFO_BUFFER_OFFSET;

    _tcscpy( m_pSortInfo->fileName, rInfo.m_FileName);  // Name of remote database file 

    m_pSortInfo->dwTotalBytes       = dwSize - CM_MIN_STORAGE_INFO_SIZE;            // Byte length of 'pBytes'
    m_pSortInfo->dwBytesRead        = rInfo.m_BytesRead ;             // Inbound byte count
    m_pSortInfo->dwReserved         = rInfo.m_dwReserved;

    BYTE *pData = (BYTE*)((BYTE *)m_pSortInfo + m_pSortInfo->dwRawDataOffset);

    memcpy(pData, rInfo.m_pBytes, rInfo.m_BytesRead);
    return 0;
}


/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  SetDBInfo()
 *
 * Description: This method is used to info the database what its HH device 
 *      counterparts settings are.
 *
 * Parameter(s): lpName - a pointer to a character string specifying the name
 *                  of the HH databse.
 *               wDbFlags - a WORD specifying the flag settings of the HH Database.
 *               dwDbType - a DWORD specifying the type of HH database.
 *               dwCreateor - a DWORD specifying the CreatorID of the HH database.
 *               wVersion   - a WORD specifying the version of the HH database.
 *               dwModNumber - a DWORD specifying the HH database's modification number.
 *               lCreateDate - a long specifying when the HH database was created.
 *               lModDate - a long specifying when the HH database was last modified.
 *               lBackupDate - a long specifying when the HH database was last backed up.
 *               
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
long CPcMgr::SetDBInfo(LPCSTR    lpName,
                          WORD      wDbFlags,    
                          DWORD     dwDbType,
                          DWORD     dwCreator, 
                          WORD      wVersion,
                          DWORD     dwModNumber,
                          long      lCreateDate,
                          long      lModDate,
                          long      lBackupDate)
{
    
    if (m_pDBInfo) {
        free(m_pDBInfo);
    }

    m_pDBInfo = (STORAGE_HEADER_PTR)malloc(CM_STORAGE_HEADER_SIZE);
    if (!m_pDBInfo)
        return GEN_ERR_LOW_MEMORY;

    m_bNeedToSave = TRUE;

    _tcscpy((char*)m_pDBInfo->szName, lpName);
    m_pDBInfo->wDBFlags             = wDbFlags;
    m_pDBInfo->dwDBVersion             = wVersion;
    m_pDBInfo->lDBCreateDate         = lCreateDate;
    m_pDBInfo->lDBModDate             = lModDate;
    m_pDBInfo->lDBBackupDate         = lBackupDate;
    m_pDBInfo->dwDBModNumber        = dwModNumber;
    m_pDBInfo->dwDBType             = dwDbType;
    m_pDBInfo->dwDBCreator          = dwCreator;

    m_pDBInfo->wVersion             = CM_STORAGE_HEADER_VERSION_1;
    m_pDBInfo->dwStructSize         = CM_STORAGE_HEADER_SIZE;

   return 0;
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  IsExtraAppInfo()
 *
 * Description: This method is used to tell whether there is other data stored
 *          in the appInfo block other than category information.
 *
 * Parameter(s): None.
 *
 * Return Value(s): TRUE - there is more data.
 *                  FALSE - there isn't more data.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
BOOL CPcMgr::IsExtraAppInfo(void)
{
    if ((!m_pAppInfo) || (!m_pAppInfo->dwBytesRead))
        return FALSE;

    if ((!AreCategoriesSupported()) || (m_pAppInfo->dwBytesRead > MIN_CATEGORY_BLOCK_SIZE))
        return TRUE;

    return FALSE;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::AllocateAppInfo(DWORD dwRawDataSize, BOOL bClearData)
{
    BOOL bNew = FALSE;
    DWORD dwRequiredSize = dwRawDataSize + CM_MIN_STORAGE_INFO_SIZE;
    STORAGE_INFO_PTR pAppInfo;
    long retval = 0;


    if (m_pAppInfo) {
        if (m_pAppInfo->dwStructSize < dwRequiredSize){
            DWORD dwCopySize = CM_MIN_STORAGE_INFO_SIZE;
            if (AreCategoriesSupported()) {
                dwCopySize += MIN_CATEGORY_BLOCK_SIZE;
            }
            // we need to allocate a larger buffer and copy the old data into the new structure.
            pAppInfo = (STORAGE_INFO_PTR)malloc(dwRequiredSize);
            if (!pAppInfo)
                return GEN_ERR_LOW_MEMORY;
            memset(pAppInfo, 0, dwRequiredSize);
            
            memcpy(pAppInfo, m_pAppInfo, dwCopySize);
            free(m_pAppInfo);
            m_pAppInfo = pAppInfo;
            m_pAppInfo->dwStructSize = dwRequiredSize;
            m_pAppInfo->dwTotalBytes = dwRequiredSize - CM_MIN_STORAGE_INFO_SIZE;
            m_pAppInfo->wVersion = CM_STORAGE_VERSION_2;
        }
    } else {
        m_pAppInfo = (STORAGE_INFO_PTR)malloc(dwRequiredSize);
        if (!m_pAppInfo)
            return GEN_ERR_LOW_MEMORY;
        memset(m_pAppInfo, 0, dwRequiredSize);
        m_pAppInfo->dwStructSize = dwRequiredSize;
        m_pAppInfo->dwTotalBytes = dwRequiredSize - CM_MIN_STORAGE_INFO_SIZE;
        m_pAppInfo->wVersion = CM_STORAGE_VERSION_2;
    }
    return 0;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::SetExtraAppInfo(DWORD dwSize, BYTE *pData)
{
    long retval = 0;
    DWORD dwDataOffset = 0;
    

    // offset the extra data spot if categories are supported
    if (AreCategoriesSupported()) {
        dwDataOffset = MIN_CATEGORY_BLOCK_SIZE;
    }

    retval = AllocateAppInfo(dwSize + dwDataOffset);
    if (retval)
        return retval;

    DWORD dwOffset = m_pAppInfo->dwRawDataOffset + dwDataOffset;
    DWORD dwExtraArea = m_pAppInfo->dwTotalBytes - dwDataOffset;
    // clear the buffer area
    BYTE *pBinData = (BYTE *)((BYTE *)m_pAppInfo + dwOffset);
    memset(pBinData, 0, dwExtraArea);
    if (!dwSize) {
        // our work is already done
        m_pAppInfo->dwBytesRead = dwDataOffset;
        return retval;
    }

    if (!pData) {
        return GEN_ERR_INVALID_POINTER;
    }

    if (dwSize > dwExtraArea) {
        return GEN_ERR_DATA_TOO_LARGE;
    }

    m_pAppInfo->dwBytesRead = dwDataOffset + dwSize;
    memcpy(pBinData, pData, dwSize);
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
DWORD CPcMgr::GetExtraAppInfoSize(void)
{
    DWORD dwDataOffset = 0;
    // offset the extra data spot if categories are supported
    if (AreCategoriesSupported()) {
        dwDataOffset = MIN_CATEGORY_BLOCK_SIZE;
    }

    if (!m_pAppInfo)
        return 0;

    return (m_pAppInfo->dwBytesRead - dwDataOffset);
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
long CPcMgr::GetExtraAppInfo(BYTE *pBuf, DWORD *pdwBufSize)
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

    ASSERT(m_pAppInfo);
    BYTE* pTemp = (BYTE *)((BYTE *)m_pAppInfo + m_pAppInfo->dwRawDataOffset + dwDataOffset);

    memcpy(pBuf, pTemp, dwSize);

    return 0;
}

/******************************************************************************
 *
 * Class:   CPcMgr
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
BYTE* CPcMgr::GetExtraAppInfo(void)
{
    DWORD dwDataOffset = 0;
    // offset the extra data spot if categories are supported
    if (AreCategoriesSupported()) {
        dwDataOffset = MIN_CATEGORY_BLOCK_SIZE;
    }

    DWORD dwSize = GetExtraAppInfoSize();
    if (!dwSize)
        return NULL;

    ASSERT(m_pAppInfo);
    return (BYTE *)((BYTE *)m_pAppInfo + m_pAppInfo->dwRawDataOffset + dwDataOffset);
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  SetSavedNeeded()
 *
 * Description: This method is used to inform the database that it needs to 
 *          save changes to the storage medium.
 *
 * Parameter(s): bSet - a boolean value specifying whether to set(TRUE)
 *              this object as Modified or to clear(FALSE) the Modified flag.
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
void CPcMgr::SetSavedNeeded(BOOL bSet) 
{ 
    if (bSet) {
        m_bNeedToSave = TRUE;
    } else {
        m_bNeedToSave = FALSE;
    }
}


    // read in int length of string to follow.
long CPcMgr::ReadSerializedString(TCHAR *pStrBuf, int *piSize)
{
    BYTE bLen;
    int iLen = 0;
    long retval;

    *pStrBuf = '\0';

    retval = ReadInData((LPVOID)&bLen, 1);
    if (retval) {
        return retval;
    }

    if (bLen == 0xff) {
        // string size is longer then ff
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
        return GEN_ERR_BUFFER_TOO_SMALL;
    }

    // get file name string
    retval = ReadInData((LPVOID)pStrBuf, iLen);
    if (!retval) {
        *piSize = iLen;
        *(pStrBuf + iLen) = '\0';
    }

    return retval;
}

long CPcMgr::ReadSerializedString(CPString &cStr)
{
    BYTE bLen;
    int iLen = 0;
    long retval;

    cStr = "";

    retval = ReadInData((LPVOID)&bLen, 1);
    if (retval) {
        return retval;
    }

    if (bLen == 0xff) {
        // string size is longer then ff
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
    retval = ReadInData((LPVOID)cStr.GetBuffer(iLen), iLen);

    return retval;
}

long CPcMgr::ReadSerializedByte(BYTE &bValue)
{
    long retval;

    retval = ReadInData((LPVOID)&bValue, sizeof(bValue));
    return retval;
}

long CPcMgr::ReadSerializedWord(WORD &wValue)
{
    long retval;

    retval = ReadInData((LPVOID)&wValue, sizeof(wValue));
    return retval;
}

// serialized DWORD is in form of: original - 0xAB CD EF GH / serialized 0x GH EF CD AB
long CPcMgr::ReadSerializedDword(DWORD &dwToRead)
{
    long retval;

    retval = ReadInData((LPVOID)&dwToRead, sizeof(dwToRead));
    if (retval) {
        return retval;
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  ReadSerializedLong
 * Description: Reads a long from todo.dat file.
 *
 * Parameter(s): lToRead -- a long pointer to receive the value read from file.
 *
 *
 * Return Value(s): 0 - success
 *					<0 - error
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/24/00    ELP         initial revision
 *
 *****************************************************************************/
long CPcMgr::ReadSerializedLong(long &lToRead)
{
    return ReadInData((LPVOID)&lToRead, sizeof(lToRead));
}


/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  ReadSerializedInteger
 * Description: Reads a integer from todo.dat file.
 *
 * Parameter(s): iToRead -- int pointer to receive the value read from file.
 *
 *
 * Return Value(s): 0 - success
 *					<0 - error
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/24/00    ELP         initial revision
 *
 *****************************************************************************/
long CPcMgr::ReadSerializedInteger(int &iToRead)
{
    return ReadInData((LPVOID)&iToRead, sizeof(iToRead));
}

DWORD CPcMgr::FlipSerialDword(DWORD &dwToFlip)
{
    // serialized Flipping is in the form of:
    // from 0x12345678 to 0x78563412
    DWORD dwFlipped = 0;
    DWORD dwMultiplier  = 0x01000000;

    for (int iMult = 1; iMult < 4; iMult++) {
        dwFlipped += (dwToFlip % 0x0000100) * dwMultiplier;
        dwToFlip = dwToFlip / 0x0000100;
        dwMultiplier = dwMultiplier / 0x0000100;
    }
    dwFlipped += dwToFlip;
    dwToFlip = dwFlipped;
    return dwFlipped;
}

WORD CPcMgr::FlipSerialWord(WORD &wToFlip)
{
    // serialized Flipping is in the form of:
    // from 0x1234 to 0x3412
    WORD wFlipped;
    wFlipped = (WORD)(((wToFlip % 0x100) * 0x100) + (wToFlip / 0x100));
    wToFlip = wFlipped;
    return wFlipped;
}

///////////////////////////////////////////////////////////////////////////
// CBaseDTLinkConverter 
///////////////////////////////////////////////////////////////////////////
//
//    Function:        StripCRs()      ** Protected **     
//
//    Description:    Remove all the Carriage returns (if they exist) for the
//                    passed in source byte stream.    
//
//    Parameters:        TCHAR* pDest - New string with CRs removed
//                    TCHAR* pSrc - String to be striped
//                    int len - length of pSrc
//
//    Returns:        int    - destination length
//
///////////////////////////////////////////////////////////////////////////
//    REVISION HISTORY:
//    10/20/95 mja     
///////////////////////////////////////////////////////////////////////////
int CPcMgr::StripCRs(TCHAR* pDest, const TCHAR* pSrc, int len)
{
    int  retval = 0;
    int  off    = 0;
    TCHAR* pCurr; 
    TCHAR* pStart = pDest;

    // See if any cr's are present in the first place.
    pCurr = _tcspbrk(pSrc, STR_CRETURN);
    if (pCurr)
    {
        while (off < len && *pSrc)
        {
            if (*pSrc == CH_CRETURN)
                pSrc++;

            *pDest = *pSrc;

            off++;
            pDest++ ; pSrc++;
        }
        *pDest = 0;

        retval = _tcslen(pStart) + 1;
    }
    else
    {
        _tcsncpy(pDest, pSrc, len);
        *(pDest + len) = '\0';
        retval = len + 1;
    }    

    return(retval);
}

long CPcMgr::AddCRs(TCHAR* pSrc, TCHAR* pDest, int len)
{
    long retval = GEN_ERR_INVALID_POINTER;
    int off=0;

    TCHAR* pCurr;

    if (pDest) {

        // Only add Cr's if newlines are present..
        pCurr = _tcspbrk(pSrc, STR_NEWLINE);
        if(pCurr) 
        {
            while (off < len && *pSrc) {
                if (*pSrc == CH_NEWLINE){
                    *pDest = CH_CRETURN;
                    pDest++;
                }
                *pDest = *pSrc;

                pDest++ ; pSrc++;
                off++;
            }
            *pDest = 0;
        } else {
            _tcsncpy(pDest, pSrc, len);
            pDest[len] = 0;
        }
        retval = 0;
    }
    return(retval);
}
long CPcMgr::AddCRs(TCHAR* pSrc, TCHAR* pDest, int len, int iBufSize)
{
    long retval = GEN_ERR_INVALID_POINTER;
    int off=0;

    TCHAR* pCurr;
    int iNewSize = 0;
    int iLimitSize = iBufSize - 1;

    if (pDest) {

        // Only add Cr's if newlines are present..
        pCurr = _tcspbrk(pSrc, STR_NEWLINE);
        if(pCurr) 
        {
            while ((off < len) && (*pSrc) && (iNewSize < iLimitSize)) {
                if (*pSrc == CH_NEWLINE){
                    *pDest = CH_CRETURN;
                    pDest++;
                    iNewSize++;
                }
                *pDest = *pSrc;

                pDest++ ; pSrc++;
                off++;
                iNewSize++;
            }
            *pDest = '\0';
        } else {
            _tcsncpy(pDest, pSrc, len);
            pDest[len] = 0;
            iNewSize = len;
        }
        retval = 0;
    }
    return(iNewSize);
}

/****************************************************************************************
 * Store the database
 ********************************************************************************/
long CPcMgr::WriteOutData(LPVOID pBuffer, 
                          DWORD dwNumberOfBytesToWrite)
{
#ifdef macintosh
	long bytesToWrite = (long)dwNumberOfBytesToWrite;
	
	
	short errCode = FSWrite(m_hFile, &bytesToWrite, pBuffer);
    if((errCode != noErr) || (bytesToWrite != (long)dwNumberOfBytesToWrite)){
        return GEN_ERR_UNABLE_TO_SAVE;
    }
	
	return 0;

#else
    long retval = 0;
    BOOL bSuccess;
    DWORD dwNumberOfBytesWritten;

    bSuccess = WriteFile(m_hFile,    // handle to file to write to 
                         (BYTE *)pBuffer,    // pointer to data to write to file 
                         dwNumberOfBytesToWrite,    // number of bytes to write 
                         &dwNumberOfBytesWritten,    // pointer to number of bytes written 
                         NULL);
    if ((!bSuccess) || (dwNumberOfBytesToWrite != dwNumberOfBytesWritten)){
        return GEN_ERR_UNABLE_TO_SAVE;
    }
    return retval;
#endif
}

long CPcMgr::WriteSerializedDword(DWORD dwToWrite)
{
    return WriteOutData((LPVOID) &dwToWrite, sizeof(DWORD));
}

long CPcMgr::WriteSerializedWord(WORD wToWrite)
{
    return WriteOutData((LPVOID) &wToWrite, sizeof(WORD));
}

long CPcMgr::WriteSerializedByte(BYTE sToWrite)
{
    return WriteOutData((LPVOID) &sToWrite, sizeof(BYTE));
}

long CPcMgr::WriteSerializedString(TCHAR *pString)
{
    long retval = 0;
    int iLen;
    BYTE bLen;

    if (!pString) {
        iLen = 0;
    } else {
        iLen = _tcslen(pString);
    }

    if (iLen >= 0xff) {
        WORD wLen = (WORD)iLen;
        bLen = 0xff;
        retval = WriteSerializedByte(bLen);
        if (retval) 
            return retval;
        retval = WriteSerializedWord(wLen);
    } else {
        bLen = (BYTE)iLen;
        retval = WriteSerializedByte(bLen);
    }
    if (retval) 
        return retval;

    if (!iLen)
        return 0;

    return WriteOutData((LPVOID) pString, iLen);
}

long CPcMgr::WriteSerializedBoolField(DWORD dwToWrite)
{
    long retval = 0;
    DWORD dwType = eBool;

    retval = WriteSerializedDword(dwType);
    if (retval) 
        return retval;

    return WriteSerializedDword(dwToWrite);
}

long CPcMgr::WriteSerializedIntegerField(DWORD dwToWrite)
{
    long retval = 0;
    DWORD dwType = eInteger;

    retval = WriteSerializedDword(dwType);
    if (retval) 
        return retval;

    return WriteSerializedDword(dwToWrite);
}

long CPcMgr::WriteSerializedStringField(TCHAR *pString, DWORD dwCaseSensitive)
{
    long retval = 0;
    DWORD dwType = eString;
    

    retval = WriteSerializedDword(dwType);
    if (retval)
        return retval;

    retval = WriteSerializedDword(dwCaseSensitive);
    if (retval)
        return retval;

    return WriteSerializedString(pString);
}

long CPcMgr::WriteSerializedDateField(DWORD dwToWrite)
{
    long retval = 0;
    DWORD dwType = eDate;

    retval = WriteSerializedDword(dwType);
    if (retval) 
        return retval;

    return WriteSerializedDword(dwToWrite);
}


/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method: WriteSerializedLong
 * Description: Writes a long to todo.dat file
 *
 * Parameter(s): lToWrite -- the value to write to file
 *
 * Return Value(s): 0 - success
 *					<0 - error
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/24/00    ELP         initial revision
 *
 *****************************************************************************/
long CPcMgr::WriteSerializedLong(long lToWrite)
{
    return WriteOutData((LPVOID) &lToWrite, sizeof(long));
}


/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method: WriteSerializedInteger
 * Description: Writes an integer to todo.dat file
 *
 * Parameter(s): iToWrite -- the value to write to file
 *
 * Return Value(s): 0 - success
 *					<0 - error
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/24/00    ELP         initial revision
 *
 *****************************************************************************/
long CPcMgr::WriteSerializedInteger(int iToWrite)
{
    return WriteOutData((LPVOID) &iToWrite, sizeof(int));
}


#ifdef macintosh
static char *CStrToPStr( const char* inCStr, char * outPStr, UInt16 inDestSize )
{
	UInt32 c_tcslen = _tcslen( inCStr );
	UInt16 len = inDestSize;
	if( c_tcslen < len )
		len = c_tcslen;
	
	::BlockMoveData( inCStr, &outPStr[1], len );
	outPStr[0] = len;
	
	return( outPStr );
}
#endif
/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  DuplicateDatabase()
 *
 * Description: This method is to provide a Macintosh version of the Window's
 *				function CopyFile()
 *
 * Parameter(s): See MSW CopyFile() function
 *
 * Return Value(s): true - no error
 *                  false - error copying file
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  07/27/99    DLW         initial revision
 *
 *****************************************************************************/
BOOL CPcMgr::DuplicateDatabase(char* srcPath, char* destPath, BOOL bFailIfExists)
{
#ifdef macintosh

	int errCode = noErr;
	short bufSize = 255;
	FSSpec srcSpec, dstSpec;
	char *pStr = NULL;


	errCode = FSMakeFSSpec(m_VolumeID, m_ParentID, (unsigned char *)srcPath, &srcSpec);
	if (errCode != noErr)
		return errCode;

	errCode = FSMakeFSSpec(srcSpec.vRefNum, srcSpec.parID, (unsigned char *)destPath, &dstSpec); 
	if (errCode == noErr)
	{
		errCode = FSpDeleteCompat(&dstSpec);
	}

	

		
	/* Copy the file using MoreFiles FileCopy function */
	errCode = FileCopy(
		srcSpec.vRefNum, 		/* Source volume */
		srcSpec.parID, 			/* Source directory */
		srcSpec.name,			/* Source name */ 
		dstSpec.vRefNum, 		/* Destination volume */
		dstSpec.parID, 			/* Destination directory */
		nil, 					/* Destination pathname */
		dstSpec.name,			/* Destination filename (use same as original) */
		nil, 					/* Buffer used by FileCopy */
		0, 						/* Size of buffer */
		true);					/* 'Pre-flight' the directories for space, etc. */
	
	
				
	if(errCode == noErr)
		return(true);
		
	return(false);
#else
    return CopyFile(srcPath, destPath, bFailIfExists);
#endif
}

/******************************************************************************
 *
 * Class:   CPcMgr
 *
 * Method:  ResetSyncFlags()
 *
 * Description: This method is used to clear all modification flags in each
 *				record. If a record is changed by this, then the database is
 *				marked as "needs to be saved".
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  < 0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  12/11/2000 KRM			initial version
 *
 *****************************************************************************/
long CPcMgr::ResetSyncFlags()
{
    for (DWORD dwIndex = 0; dwIndex < m_dwMaxRecordCount; dwIndex++) {
        if (m_pRecordList[dwIndex]) {
            if (m_pRecordList[dwIndex]->IsModified()) {
                m_pRecordList[dwIndex]->ResetAttribs();
				SetSavedNeeded();
            }
        }
    }
	return 0;
}

