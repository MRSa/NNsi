/*****************************************************************************
 *
 * Generic Conduit Archive Database Mgr Header File
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
#ifndef _ARCHIVE_DB_MGR_H_
#define _ARCHIVE_DB_MGR_H_
#include "pcmgr.h"


class CArchiveDatabase : public CPcMgr
{
public:
    CArchiveDatabase(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName = NULL);
    virtual ~CArchiveDatabase();

public:   // Public Variables

protected:  // protected Variables

public:     // Public Functions
    virtual long PostSync(void);
    virtual long PreSync(void);

protected:    // protected Functions
    virtual long OpenDB(void);

};

#endif