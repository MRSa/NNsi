/*****************************************************************************
 *
 * Generic Conduit Logging Header File
 *
 * Copyright (c) 1998-2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
#ifndef _CPLOGGING_H_
#define _CPLOGGING_H_
#include <hslog.h>
#include <CPalmrec.h>

#define LOG_PC_ERROR FALSE
#define LOG_HH_ERROR TRUE
class CPLogging
{
public:
    CPLogging(HANDLE hLangInstance, const TCHAR *pAppName);
    CPLogging(const TCHAR *pAppName);
    virtual ~CPLogging();

public:
    virtual void Error(long lError);
    //virtual void Error(long lError, const TCHAR *pText);
    //virtual void Error(long lError, long lItem);
    virtual void Error(long lError, long lItem1, long lItem2);
    virtual void SyncStart(void);
    virtual void SyncFinish(BOOL bError);
    virtual void RecordFailure(long lError, CPalmRecord&rec, BOOL bFull=FALSE);
    virtual void LogBanner(void);
    virtual void RecordAddFailure(long lError, CPalmRecord&rec, BOOL bHH=LOG_HH_ERROR);
    virtual void RecordReadFailure(long lError, CPalmRecord&rec, BOOL bHH=LOG_HH_ERROR);

    virtual void CreateDBFailure(long lError, const TCHAR *pDBStr, BOOL bHH=LOG_HH_ERROR);


    virtual void BadPurge(long lError, BOOL bHH=LOG_HH_ERROR);
    virtual void BadLocalSave(long lError);
    virtual void BadCloseDB(long lError, BOOL bHH=LOG_HH_ERROR);
    virtual void BadResetSyncFlags(long lError, BOOL bHH=LOG_HH_ERROR);
    virtual void DoubleModify(CPalmRecord&rec);
    virtual void ReverseDelete(CPalmRecord&rec);
    virtual void CategoryDeleted(const char *pCategory, BOOL bHH=LOG_HH_ERROR);
    virtual void BadChangeCategory(const char *pCategory, BOOL bHH=LOG_HH_ERROR);
    virtual void TooManyCats(const char *pCategory, BOOL bHH=LOG_HH_ERROR);

    virtual int FormatDBString(TCHAR *szBuf, 
                                    char  *pName, 
                                    DWORD  dwCreator,
                                    DWORD  dwType, 
                                    WORD   dbFlags,
                                    WORD   dbVersion);

    virtual void LogInfo(const char *pMessage);
    virtual void LogWarning(const char *pMessage);
    virtual int FormatRecordString(CPalmRecord &rec);

private:
    virtual int LoadStr(int iStrId);
    virtual int DetermineStringID(long lError, Activity &actError);
    virtual int DetermineRemoteStringID(long lError, Activity &actError);
    virtual TCHAR *HexString(long lError);


protected:
    BOOL m_bStartBanner;
    short m_sDebugLevel;
    HANDLE m_hLangInstance;
    TCHAR m_szRecBuf[256];
    int m_iErrorCnt;
    TCHAR m_szError[16];
public:
    TCHAR m_szStrBuf[512];
    TCHAR m_szAppName[128];
};
#endif
