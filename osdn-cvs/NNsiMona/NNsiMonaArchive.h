/*****************************************************************************
 *
 * NNsiMonaArchive Generic Conduit Archive Database Mgr Header File
 *
 ****************************************************************************/
#ifndef _NNsiMona_ARCHIVE_DB_MGR_H_
#define _NNsiMona_ARCHIVE_DB_MGR_H_

#include "NNsiMonaPCMgr.h"


class CNNsiMonaArchive : public CNNsiMonaPCMgr
{
public:
    CNNsiMonaArchive(CPLogging *pLogging, DWORD dwGenericFlags, char *szDbName, TCHAR *pFileName, TCHAR *pDirName = NULL);
    virtual ~CNNsiMonaArchive();
public:   // Public Variables

protected:  // protected Variables

public:     // Public Functions
    virtual long PostSync(void);
    virtual long PreSync(void);

protected:    // protected Functions
    virtual long OpenDB(void);
};

#endif

