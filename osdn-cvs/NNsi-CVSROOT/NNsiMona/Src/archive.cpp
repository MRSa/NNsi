/*****************************************************************************
 *
 * Generic Conduit CArchiveDatabase Src File
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
#include <archive.h>
#include <Pgenerr.h>

/******************************************************************************
 *
 * Class:   CArchiveDatabase
 *
 * Method:  Constructor
 *
 * Description: This method initializes class data. This class is used to
 *              archive records from categories.
 *
 * Parameter(s): szDbName   - a pointer to a character buffer specifying the name
 *                          of the database associated with this archive.
 *               pFileName  - a pointer to a character buffer specifying the name
 *                          of the database to save to.
 *               pDirName   - a pointer to a character buffer specifying the name
 *                          of the directory where the database is stored
 *
 * Return Value(s): None
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
CArchiveDatabase::CArchiveDatabase(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName)
: CPcMgr(pLogging, dwGenericFlags, szDbName, pFileName, pDirName, eFast)
{
    TRACE0("CArchiveDatabase Constructor\n");

    // change the name of the file to the name of the Archive file.
    // 
#ifndef macintosh
    // change the name of the file to the name of the Archive file.
    // 
    char *pDot = _tcsrchr(m_szDataFile, '.');
    if (!pDot){
        _tcscat(m_szDataFile, ".ARC");
    } else {
        char *pSlash = _tcsrchr(m_szDataFile, '\\');
        if ((pSlash) && (pSlash > pDot)) // just tack on the extension
            _tcscat(m_szDataFile, ".ARC");
        else { // set the dot to null and cat on the extension.
            *pDot = '\0';
            _tcscat(m_szDataFile, ".ARC");
        }
    }
#else
	int len = m_FileName[0];

	char szFileName[256];

	BlockMoveData(m_FileName + 1, szFileName, len);
	szFileName[len] = '\0';			// null terminate to make it a C string.	
	
    char *pDot = _tcsrchr(szFileName, '.');
    if (!pDot){
        _tcscat(szFileName, ".ARC");
    } else {
        char *pSeparator = _tcsrchr(szFileName, ':');
        if ((pSeparator) && (pSeparator > pDot)) // just tack on the extension
            _tcscat(szFileName, ".ARC");
        else { // set the dot to null and cat on the extension.
            *pDot = '\0';
            _tcscat(szFileName, ".ARC");
        }
    }
	_tcscpy((char *)&(m_FileName[1]), (const char *)szFileName);
	m_FileName[0] = (unsigned char) _tcslen(szFileName);
				
#endif    
}

/******************************************************************************
 *
 * Class:   CArchiveDatabase
 *
 * Method:  Destructor
 *
 * Description: This method frees any allocated objects
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
CArchiveDatabase::~CArchiveDatabase()
{
    TRACE0("CArchiveDatabase Destructor\n");
}

/******************************************************************************
 *
 * Class:   CArchiveDatabase
 *
 * Method:  PostSync()
 *
 * Description: This method does closing type functions. It stores the database 
 *              and closes it.
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
long CArchiveDatabase::PostSync(void)
{
    long retval;
    m_bStartSync = FALSE;
    retval = StoreDB();
    if (retval)
        return retval;
    retval = CloseDB();
    return retval;
}


/******************************************************************************
 *
 * Class:   CArchiveDatabase
 *
 * Method:  OpenDB()
 *
 * Description: This protected method opens the database and loads it if available.
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
long CArchiveDatabase::OpenDB(void)
{
    long retval = 0;
	// always open the archive in ReadWrite Mode
    if (m_bStartSync){
        retval = Open(eRead);
        
        if ((retval) && (retval == GEN_ERR_UNABLE_TO_OPEN_FILE)){
            // this is not a problem if the file does not exist.
            return 0;
        }
        if (!retval){
            retval = PreSync();
        }
    } else {
        retval = Open(eCreate);
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CArchiveDatabase
 *
 * Method:  PreSync()
 *
 * Description: This method does initialization actions with the archive DB. If a database
 *              is access in memory, this method will load the database into memory.
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
long CArchiveDatabase::PreSync(void)
{
    long retval = 0;
    if (m_bAlreadyLoaded)
        return 0;
    m_bAlreadyLoaded = TRUE;
    retval = RetrieveDB();
    return retval;
}
