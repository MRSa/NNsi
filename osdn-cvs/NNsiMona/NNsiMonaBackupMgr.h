/*****************************************************************************
 *
 * CNNsiMonaBackupMgr Generic Conduit Backup Database Mgr Header File
 *
 ****************************************************************************/

#ifndef _NNsiMona_BACKUP_DB_MGR_H_
#define _NNsiMona_BACKUP_DB_MGR_H_

#include "NNsiMonaPCMgr.h"


class CNNsiMonaBackupMgr : public CNNsiMonaPCMgr
{
public:
    CNNsiMonaBackupMgr(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName = NULL, eSyncTypes syncType = eDoNothing);
    virtual ~CNNsiMonaBackupMgr();

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

