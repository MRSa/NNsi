/*****************************************************************************
 *
 * Generic Conduit Backup Database Mgr Header File
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
#ifndef _BACKUP_DB_MGR_H_
#define _BACKUP_DB_MGR_H_
#include "pcmgr.h"


class CBackupMgr : public CPcMgr
{
public:
    CBackupMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName = NULL, eSyncTypes syncType = eDoNothing);
    virtual ~CBackupMgr();

public:   // Public Variables

protected:  // protected Variables
    TCHAR m_szRealDb[256];

public:     // Public Functions
    virtual long PreSync(void);
    virtual long PostSync(void);
    virtual long Close(BOOL bDontUpdate=FALSE);

protected:    // protected Functions
    virtual long OpenDB(void);


};

#endif