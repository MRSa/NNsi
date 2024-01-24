/*****************************************************************************
 *
 * Generic Conduit CBackupMgr Src File
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
#endif

#ifndef macintosh
#include <sys/stat.h>
#include <TCHAR.H>
#endif


#include <syncmgr.h>
#include <bckupmgr.h>
#include <Pgenerr.h>
#include <PcMgr.h>

/******************************************************************************
 *
 * Class:   CBackupMgr
 *
 * Method:  Constructor
 *
 * Description: This method initial class data. This class is used to
 *              backup the main database. This backup copy is used during
 *              slow syncs to determine which records have been modified 
 *              on the Palm organizer.
 *
 * Parameter(s): szDbName   - a pointer to a character buffer specifying the name
 *                          of the database associated with this archive.
 *               pFileName  - a pointer to a character buffer specifying the name
 *                          of the database to save to.
 *               pDirName   - a pointer to a character buffer specifying the name
 *                          of the directory where the database is stored.
 *               syncType   - a enum value specifying what type of synchronization
 *                          is to be performed.
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
CBackupMgr::CBackupMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName, eSyncTypes syncType)
: CPcMgr(pLogging, dwGenericFlags, szDbName, pFileName, pDirName, syncType)
{
#ifndef macintosh
    TRACE0("CBackupMgr Constructor\n");

    // change the name of the file to the name of the backup file.
    // 
    _tcscpy(m_szRealDb, m_szDataFile);
    char *pDot = _tcsrchr(m_szDataFile, '.');
    if (!pDot){
        _tcscat(m_szDataFile, ".BAK");
    } else {
        char *pSlash = _tcsrchr(m_szDataFile, '\\');
        if ((pSlash) && (pSlash > pDot)) // just tack on the extension
            _tcscat(m_szDataFile, ".BAK");
        else { // set the dot to null and cat on the extension.
            *pDot = '\0';
            _tcscat(m_szDataFile, ".BAK");
        }
    }
#else
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

	// copy the path to the RealDb	
    _tcscpy(m_szRealDb, (char*) fileName);	

	
	// Convert fileName to the backup filename
	int len = m_FileName[0];
    char szFileName[256];
	BlockMoveData(&m_FileName[1], szFileName, len);    

	szFileName[len] = '\0';
		
    char *pDot = _tcsrchr(szFileName, '.');
    if (!pDot){
        _tcscat(szFileName, ".BAK");
    } else {
        char *pSeparator = _tcsrchr(szFileName, ':');
        if ((pSeparator) && (pSeparator > pDot)) // just tack on the extension
            _tcscat(szFileName, ".BAK");
        else { // set the dot to null and cat on the extension.
            *pDot = '\0';
            _tcscat(szFileName, ".BAK");
        }
    }
    
	_tcscpy((char *)&(m_FileName[1]), (const char *)szFileName);
	m_FileName[0] = (unsigned char) _tcslen(szFileName);
	
#endif
}

/******************************************************************************
 *
 * Class:   CBackupMgr
 *
 * Method:  Destructor
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
CBackupMgr::~CBackupMgr()
{
    TRACE0("CBackupMgr Destructor\n");
}

/******************************************************************************
 *
 * Class:   CBackupMgr
 *
 * Method:  Close()
 *
 * Description: This method closes the backup database. Since the PostSync()
 *              method just copies the normal database, there is no reason
 *              to update the backup database here.
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
long CBackupMgr::Close(BOOL bDontUpdate)
{
    long retval = CloseDB(TRUE);
    return retval;
}

/******************************************************************************
 *
 * Class:   CBackupMgr
 *
 * Method:  PostSync()
 *
 * Description: This method does closing type functions on the backup 
 *              Database. In this case, it closes the current open database
 *              and then File copies the real database on top of the 
 *              old backup file.
 *
 * Parameter(s):    None.
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
long CBackupMgr::PostSync(void)
{
    long retval = CloseDB(TRUE);
    if (retval)
        return retval;

    // copy the PC DB to the bak file.
#ifndef macintosh    
    BOOL bSuccess = DuplicateDatabase(m_szRealDb, m_szDataFile, FALSE);
#else     
	 BOOL bSuccess = DuplicateDatabase(m_szRealDb, (TCHAR *) m_FileName, FALSE);
#endif
    if (bSuccess)
        return 0;
    
    return GEN_ERR_UNABLE_TO_SAVE_BACKUP;
}


/******************************************************************************
 *
 * Class:   CBackupMgr
 *
 * Method:  OpenDB()
 *
 * Description: This protected method is used to retrieve the database from
 *              its storage medium. For backup database, this retrieval is
 *              only performed on the start of a slow sync.
 *              In all over cases, the database is not retrieved.
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
long CBackupMgr::OpenDB(void)
{
    long retval = 0;
    // only open and read the backup file in a slow sync.
    if ((m_SyncType == eSlow) && (m_bStartSync)) {
        retval = Open(eRead);
        if ((retval) && (retval == GEN_ERR_UNABLE_TO_OPEN_FILE)) {
            // if there is no backup file, don't worry about it
            return 0;
        }
        if (!retval){ // Fixed 3/25/99 KRM
            retval = PreSync();
        }
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CBackupMgr
 *
 * Method:  PreSync()
 *
 * Description: This method does initialization actions with the Backup DB. If a database
 *              is access in memory, this method will load the database into memory. For backup 
 *              database, this retrieval is only performed on the 
 *              start of a slow sync.
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
long CBackupMgr::PreSync(void)
{
    long retval = 0;
    if (m_bAlreadyLoaded)
        return 0;
    m_bAlreadyLoaded = TRUE;
    // only open and read the backup file in a slow sync.
    if (m_SyncType == eSlow) {
        retval = RetrieveDB();
        if ((retval ) && (retval == GEN_ERR_INVALID_DB)){
        // if there is no backup file, don't worry about it
            retval = 0;
        }
    }
    return retval;
}
