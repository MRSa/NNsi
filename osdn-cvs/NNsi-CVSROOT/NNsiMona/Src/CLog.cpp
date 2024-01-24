/*****************************************************************************
 *
 * Generic Conduit Logging Src File
 *
 * Copyright (c) 1998-2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
#ifndef macintosh
#define ASSERT(f)          ((void)0)
#endif

#define TRACE0(sz)
#include <windows.h>
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
#include <CPResource.h>
#endif

#ifndef macintosh
#include <sys/stat.h>
#include <TCHAR.H>
#endif


#include <syncmgr.h>
#include <logstrng.h>
#include "resource.h"
#include <Clog.h>
#include <Pgenerr.h>

#define CHECK_LEVEL_0 0


/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method:  Constructor
 *
 * Description: This method initializes the class object.
 *
 * Parameter(s):    hLangInstance - a HINSTANCE specifying where to load the resources
 *                      from.
 *                  pAppName - a character array specifying the name of the conduit.
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
CPLogging::CPLogging(HANDLE hLangInstance, const TCHAR *pAppName)
{
    TRACE0("CPLogging Constructor\n");

    ASSERT(hLangInstance);
    ASSERT(pAppName);
    ASSERT(_tcslen(pAppName) < sizeof(m_szAppName));
    m_hLangInstance = hLangInstance;
    _tcscpy(m_szAppName, pAppName);

    m_iErrorCnt = 0;
}


/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method:  Constructor
 *
 * Description: This method initializes the class object.
 *
 * Parameter(s):    pAppName - a character array specifying the name of the conduit.
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
CPLogging::CPLogging(const TCHAR *pAppName)
{
    TRACE0("CPLogging Constructor\n");

    ASSERT(pAppName);
    ASSERT(_tcslen(pAppName) < sizeof(m_szAppName));
    _tcscpy(m_szAppName, pAppName);

    m_iErrorCnt = 0;
}
/******************************************************************************
 *
 * Class:   CPLogging
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
CPLogging::~CPLogging()
{
    TRACE0("CPLogging Destructor\n");

}


/*
enum Activity { slText = -1,
                slDoubleModify,
                slDoubleModifyArchive,
                slReverseDelete,
                slTooManyCategories,
                slCategoryDeleted,
                slDateChanged,
                slCustomLabel,
                slChangeCatFailed,
                slRemoteReadFailed,
                slRemoteAddFailed,
                slRemotePurgeFailed,
                slRemoteChangeFailed,
                slRemoteDeleteFailed,
                slLocalAddFailed,
                slRecCountMismatch,
                slXMapFailed,
                slArchiveFailed,
                slLocalSaveFailed,
                slResetFlagsFailed,
                slSyncStarted,
                slSyncFinished,
                slSyncAborted,
                slWarning,
                slDoubleModifySubsc };
*/
/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method:  CreateDBFailure
 *
 * Description: This method adds an error to the log specifying that one of the
 *          databases (HH or PC) was unable to be created.
 *
 * Parameter(s):    lError - the error code that occurred.
 *                  pDbStr - a character buffer created by the caller providing
 *                      details about the database.
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::CreateDBFailure(long lError, const TCHAR *pDBStr, BOOL bHH)
{
    LogBanner();
    Activity actError = slText;
    TCHAR szBuf[512];
    if (!bHH) 
    {
        LoadStr(IDS_PC_DB_CREATE_FAILURE);
         wsprintf(szBuf, m_szStrBuf, pDBStr, lError);
     } 
    else 
    {
        LoadStr(IDS_HH_DB_CREATE_FAILURE);
        wsprintf(szBuf, m_szStrBuf, pDBStr, lError);
    }
    LogAddEntry((const char*)szBuf, actError, FALSE);             
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method:  RecordAddFailure
 *
 * Description: This method adds an error to the log specifying that one of the
 *          databases (HH or PC) was unable to add a new record.
 *
 * Parameter(s):    lError - the error code that occurred.
 *                  rec - a CPalmRecord object specifying the record to describe
 *                      in the error.
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::RecordAddFailure(long lError, CPalmRecord &rec, BOOL bHH)
{
    LogBanner();
    Activity actError = slRemoteAddFailed;
   
    FormatRecordString(rec);

    TCHAR szBuf[512];
    if (!bHH) {
        LoadStr(IDS_PC_ADD_RECORD_ERROR);
        actError = slLocalAddFailed;
        wsprintf(szBuf, m_szStrBuf, m_szRecBuf, lError);
    } 
    else 
    {
        LoadStr(IDS_HH_ADD_RECORD_ERROR);
        wsprintf(szBuf, m_szStrBuf, m_szRecBuf, lError);
    }
    LogAddEntry((const char*)szBuf, actError, FALSE);             
}


/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method:  RecordReadFailure
 *
 * Description: This method adds an error to the log specifying that one of the
 *          databases (HH or PC) was unable to read a record from the database medium.
 *
 * Parameter(s):    lError - the error code that occurred.
 *                  rec - a CPalmRecord object specifying the record to describe
 *                      in the error.
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::RecordReadFailure(long lError, CPalmRecord &rec, BOOL bHH)
{
    LogBanner();
    Activity actError = slRemoteReadFailed;

    FormatRecordString(rec);

    TCHAR szBuf[512];
    if (!bHH) {
        actError = slText;
        LoadStr(IDS_PC_READ_REC_ERROR);

        wsprintf(szBuf, m_szStrBuf, m_szRecBuf, lError);
    } 
    else 
    {
        LoadStr(IDS_HH_READ_REC_ERROR);
        wsprintf(szBuf, m_szStrBuf, m_szRecBuf, lError);
    }
    LogAddEntry((const char*)szBuf, actError, FALSE);             
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method:  DetermineRemoteStringID
 *
 * Description: This method looks at the error code from the HH and attempts
 *          to determine the correct string to load and activity to associated
 *          with the error.
 *
 * Parameter(s):    lError - the error code that occurred.
 *                  actError - an activity enum used to receive the activity 
 *                      determined.
 *
 * Return Value(s): An int specifying the string to load..
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
int CPLogging::DetermineRemoteStringID(long lError, Activity &actError)
{
    switch (lError) {
        case SYNCERR_UNKNOWN:
        case SYNCERR_MORE:
        case SYNCERR_NOT_FOUND:
        case SYNCERR_FILE_NOT_OPEN:
        case SYNCERR_FILE_OPEN:
        case SYNCERR_RECORD_BUSY:
        case SYNCERR_RECORD_DELETED:
        case SYNCERR_READ_ONLY:
        case SYNCERR_COMM_NOT_INIT:
        case SYNCERR_FILE_ALREADY_EXIST:
        case SYNCERR_FILE_ALREADY_OPEN:
        case SYNCERR_NO_FILES_OPEN:
        case SYNCERR_BAD_OPERATION:
        case SYNCERR_REMOTE_BAD_ARG:
        case SYNCERR_BAD_ARG_WRAPPER:
        case SYNCERR_ARG_MISSING:
        case SYNCERR_LOCAL_BUFF_TOO_SMALL:
        case SYNCERR_REMOTE_MEM:
        case SYNCERR_REMOTE_NO_SPACE:
        case SYNCERR_REMOTE_SYS:
        case SYNCERR_LOCAL_MEM:
        case SYNCERR_BAD_ARG:
        case SYNCERR_LIMIT_EXCEEDED:
        case SYNCERR_UNKNOWN_REQUEST:
        case SYNCERR_TOO_MANY_OPEN_FILES:
        case SYNCERR_REMOTE_CANCEL_SYNC:
        case SYNCERR_LOST_CONNECTION:
        case SYNCERR_LOCAL_CANCEL_SYNC:
        default:
            actError = slText;
            return IDS_UNKNOWN_FATAL_ERROR;
            break;
    }

    return 0; 
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method:  DetermineStringID
 *
 * Description: This method looks at the error code and attempts
 *          to determine the correct string to load and activity to associated
 *          with the error.
 *
 * Parameter(s):    lError - the error code that occurred.
 *                  actError - an activity enum used to receive the activity 
 *                      determined.
 *
 * Return Value(s): An int specifying the string to load..
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
int CPLogging::DetermineStringID(long lError, Activity &actError)
{
    switch (lError) {
        case GEN_ERR_BUFFER_TOO_SMALL:
        case GEN_ERR_LOW_MEMORY:
        case GEN_ERR_INVALID_BUFFER: 
        case GEN_ERR_DATA_TOO_LARGE:
        case GEN_ERR_INVALID_INDEX:
        case GEN_ERR_INVALID_ID:
        case GEN_ERR_INVALID_VALUE:
        case GEN_ERR_VALUE_NOT_FOUND:
        case GEN_ERR_INVALID_POINTER:
        case GEN_ERR_INDEX_IN_USE:
        case GEN_ERR_FIND_TYPE_OUT_OF_RANGE:
        case GEN_ERR_NOT_SUPPORTED:
        case GEN_ERR_DB_ALREADY_OPEN:
        case GEN_ERR_NO_CATEGORIES:
        case GEN_ERR_NO_MORE_RECORDS:
        case GEN_ERR_CATEGORIES_NOT_SUPPORTED:
        case GEN_ERR_CACHE_NOT_INITIALIZED:
        case GEN_ERR_SYNC_TYPE_NOT_SUPPORTED:
        case GEN_ERR_BAD_SYNC_TYPE:
        case GEN_ERR_DB_NOT_OPEN:
        case GEN_ERR_UNABLE_TO_OPEN_FILE:
        case GEN_ERR_RECORD_NOT_FOUND:
        case GEN_ERR_STORAGE_EOF:
        case GEN_ERR_READING_DATA:
        case GEN_ERR_READING_NOT_ENOUGH:
        case GEN_ERR_INVALID_DB:
        case GEN_ERR_INVALID_DB_NAME:
        case GEN_ERR_READING_DBINFO:
        case GEN_ERR_READING_APPINFO:
        case GEN_ERR_READING_SORTINFO:
        case GEN_ERR_READING_RECORD:
        case GEN_ERR_UNABLE_TO_SAVE:
        case GEN_ERR_NO_APP_INFO:
        case GEN_ERR_NO_SORT_INFO:
        default:
            actError = slText;
            return IDS_UNKNOWN_FATAL_ERROR;
            break;
    }
    return 0; 
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: LogBanner
 *
 * Description: This method is used to declare that the conduit encountered 
 *      an error and posts the name of the conduit before the error. The error
 *      banner is only display once.
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
void CPLogging::LogBanner(void)
{
#ifndef macintosh
    if (LogTestCounters() == 0) 
    { // if no errors yet
        _tcscpy(m_szStrBuf, "-- ");
        _tcscat(m_szStrBuf, m_szAppName);
        LogAddEntry((const char*)m_szStrBuf, slText, FALSE);             
    }
#endif
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: RecordFailure
 *
 * Description: This method adds an error to the log specifying that one of the
 *          databases (HH or PC) encountered a record level error.
 *
 * Parameter(s):    lError - the error code that occurred.
 *                  rec - a CPalmRecord object specifying the record to describe
 *                      in the error.
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
void CPLogging::RecordFailure(long lError, CPalmRecord &rec, BOOL bFull)
{
    LogBanner();
    TCHAR szBuffer[512];
    Activity actError;

    int iStrID = DetermineStringID(lError, actError);
    if (LoadStr(iStrID) < 0)
        return;

    FormatRecordString(rec);

    if (iStrID != IDS_UNKNOWN_FATAL_ERROR) {

        wsprintf(szBuffer, m_szStrBuf, m_szRecBuf);
        LogAddEntry((const char*)m_szStrBuf, actError,FALSE);

        if (bFull)
         {
            LoadStr(IDS_DEVICE_FULL);
            LogAddEntry((const char*)m_szStrBuf,slText,FALSE);
        }
    } 
    else
    {
        wsprintf(szBuffer, m_szStrBuf, lError);
        LogAddEntry((const char*)szBuffer, actError,FALSE);
    }

}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: Error()
 *
 * Description: This method adds an error to the log specifying that an error
 *              has occurred.
 *
 * Parameter(s): lError - the error code that occurred.
 *               lItem1 - a long value
 *               lItem2 - a long value
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
void CPLogging::Error(long lError, 
                      long lItem1, 
                      long lItem2)
{
    LogBanner();
    TCHAR szBuffer[512];
    Activity actError;

    int iStrID = DetermineStringID(lError, actError);
    if (LoadStr(iStrID) < 0)
        return;

    if (iStrID != IDS_UNKNOWN_FATAL_ERROR) 
    {
        wsprintf(szBuffer, m_szStrBuf, lItem1, lItem2);
        LogAddEntry((const char*)szBuffer, actError,FALSE);             
    } else {

        wsprintf(szBuffer, m_szStrBuf, lError);
        LogAddEntry((const char*)szBuffer, actError,FALSE);
    }
}


/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: Error()
 *
 * Description: This method adds an error to the log specifying that an error
 *              has occurred.
 *
 * Parameter(s): lError - the error code that occurred.
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
void CPLogging::Error(long lError)
{
    LogBanner();
    TCHAR szBuffer[512];

    Activity actError;

    int iStrID = DetermineStringID(lError, actError);
    if (LoadStr(iStrID) < 0)
        return;

    if (iStrID != IDS_UNKNOWN_FATAL_ERROR) 
    {
            LogAddEntry((const char*)m_szStrBuf, actError,FALSE);             
    } 
    else
    {
        wsprintf(szBuffer, m_szStrBuf, lError);
        LogAddEntry((const char*)szBuffer, actError,FALSE);
    }
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: SyncStart()
 *
 * Description: This method informs the log that this conduit is going to start
 *          a sync.
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
void CPLogging::SyncStart(void)
{
#ifndef macintosh // on Mac this results in an extra blank log entry
    // clear log
    LogAddEntry("", slSyncStarted, FALSE);
    LogAddEntry("", slSyncFinished, FALSE);
    LogAddEntry("", slSyncStarted, FALSE);
#endif             
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: SyncFinish()
 *
 * Description: This method informs the log that this conduit is done with its
 *          synchronization.
 *
 * Parameter(s): bError - a boolean specifying whether an error has occurred or
 *                      not.
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
void CPLogging::SyncFinish(BOOL bError)
{
#ifndef macintosh // on Mac this results in an extra blank log entry
    if (!bError) // Send Finished Syncing message to the log
        LogAddEntry(m_szAppName, slSyncFinished, FALSE);
    else         // Print Aborted message to the log
        LogAddEntry(m_szAppName, slSyncAborted, FALSE);
#endif
}


/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: LoadStr
 *
 * Description: This method loads the specified string from the resource locatioin.
 *
 * Parameter(s): iStrId - an integer specifying the string to load.
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
int CPLogging::LoadStr(int iStrId)
{
#ifndef macintosh
    return (LoadString((HINSTANCE)m_hLangInstance, iStrId, m_szStrBuf, sizeof(m_szStrBuf)));
#else
    return (CPResource::LoadString(iStrId, m_szStrBuf, sizeof(m_szStrBuf)));
#endif
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: HexString()
 *
 * Description: This method converts a hex number into a string "0xXXXXXXXXX" type
 *              string.
 *
 * Parameter(s): lError - the error code to convert.
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
TCHAR *CPLogging::HexString(long lError)
{
    _itoa( lError, m_szError, 16 );
    return (TCHAR *)m_szError;
}
/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: BadPurge()
 *
 * Description: This method adds an error to the log specifying that one of the
 *          databases (HH or PC) was unable purge the deleted records.
 *
 * Parameter(s): lError - the error code that occurred.
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::BadPurge(long lError, BOOL bHH)
{
    Activity actError = slRemotePurgeFailed;
    TCHAR szBuffer[512];

    if (bHH == LOG_HH_ERROR) {
        LoadStr(IDS_BAD_PURGE_REMOTE);
    } else {
        actError = slWarning;
        LoadStr(IDS_BAD_PURGE_LOCAL);
    }
    wsprintf(szBuffer, m_szStrBuf, HexString(lError)); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: BadLocalSave()
 *
 * Description: This method adds an error to the log specifying that the
 *          PC database was unable save the database changes.
 *
 * Parameter(s): lError - the error code that occurred.
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
void CPLogging::BadLocalSave(long lError)
{
    Activity actError = slLocalSaveFailed;
    TCHAR szBuffer[512];

    LoadStr(IDS_BAD_LOCAL_SAVE);
    wsprintf(szBuffer, m_szStrBuf, HexString(lError)); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: BadResetSyncFlags
 *
 * Description: This method adds an error to the log specifying that one of the
 *          databases (HH or PC) was unable reset the synchronization flags.
 *
 * Parameter(s):    lError - the error code that occurred.
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::BadResetSyncFlags(long lError, BOOL bHH)
{
    Activity actError = slResetFlagsFailed;
    TCHAR szBuffer[512];

    if (bHH == LOG_HH_ERROR) {
        LoadStr(IDS_BAD_RESET_FLAGS_REMOTE);
    } else {
        actError = slWarning;
        LoadStr(IDS_BAD_RESET_FLAGS_LOCAL);
    }
    wsprintf(szBuffer, m_szStrBuf, HexString(lError)); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: BadCloseDB
 *
 * Description: This method adds an error to the log specifying that one of the
 *          databases (HH or PC) was unable to close the database.
 *
 * Parameter(s):    lError - the error code that occurred.
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::BadCloseDB(long lError, BOOL bHH)
{
    Activity actError = slWarning;
    TCHAR szBuffer[512];

    if (bHH == LOG_HH_ERROR) {
        LoadStr(IDS_BAD_CLOSE_DB_REMOTE);
    } else {
        actError = slLocalSaveFailed;
        LoadStr(IDS_BAD_CLOSE_DB_LOCAL);
    }
    wsprintf(szBuffer, m_szStrBuf, HexString(lError)); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: DoubleModify
 *
 * Description: This method adds a log entry specifying a record was modified
 *          in both databases and the modifications were different.
 *
 * Parameter(s): rec - a CPalmRecord object specifying the record to describe
 *                      in the error.
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
void CPLogging::DoubleModify(CPalmRecord &rec)
{
    Activity actError = slDoubleModify;
    TCHAR szBuffer[512];

    LoadStr(IDS_DOUBLE_MODIFIED);

    FormatRecordString(rec);

    wsprintf(szBuffer, m_szStrBuf, m_szRecBuf); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: ReverseDelete
 *
 * Description: This method adds a log entry specifying that a record was undeleted
 *          due to changes.
 *
 * Parameter(s): rec - a CPalmRecord object specifying the record to describe
 *                      in the error.
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
void CPLogging::ReverseDelete(CPalmRecord &rec)
{
    Activity actError = slReverseDelete;
    TCHAR szBuffer[512];

    LoadStr(IDS_REVERSE_DELETE);
    FormatRecordString(rec);

    wsprintf(szBuffer, m_szStrBuf, m_szRecBuf); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: TooManyCats
 *
 * Description: This method adds a log entry specifying that a category is being
 *           deleted because there are too many categories.
 *
 * Parameter(s):    pCategory - a pointer to a charater buffer containing the name
 *                      of the category being deleted..
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::TooManyCats(const char *pCategory, BOOL bHH)
{
    Activity actError = slTooManyCategories;
    TCHAR szBuffer[512];
    

    if (bHH == LOG_HH_ERROR) {
        LoadStr(IDS_REMOTE_TOOMANY_CATS);
    } else {
        LoadStr(IDS_LOCAL_TOOMANY_CATS);
    }
    wsprintf(szBuffer, m_szStrBuf, pCategory); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: BadChangeCategory()
 *
 * Description: This method adds a log entry specifying that one of the databases 
 *          were unable to change a category.
 *
 * Parameter(s): pCategory - a pointer to a charater buffer containing the name
 *                      of the category being changed.
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::BadChangeCategory(const char *pCategory, BOOL bHH)
{
    Activity actError = slChangeCatFailed;
    TCHAR szBuffer[512];
    

    if (bHH == LOG_HH_ERROR) {
        LoadStr(IDS_REMOTE_BAD_CHANGE_CAT);
    } else {
        LoadStr(IDS_LOCAL_BAD_CHANGE_CAT);
    }
    wsprintf(szBuffer, m_szStrBuf, pCategory); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: CategoryDeleted()
 *
 * Description: This method adds a log entry specifying that a category is being
 *           deleted.
 *
 * Parameter(s):    pCategory - a pointer to a charater buffer containing the name
 *                      of the category being deleted.
 *                  bHH - a boolean specifying whether this is a HH or PC error.
 *                      The default value is TRUE.
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
void CPLogging::CategoryDeleted(const char *pCategory, BOOL bHH)
{
    Activity actError = slCategoryDeleted;
    TCHAR szBuffer[512];
    

    if (bHH == LOG_HH_ERROR) {
        LoadStr(IDS_REMOTE_CAT_DELETED);
    } else {
        LoadStr(IDS_LOCAL_CAT_DELETED);
    }
    wsprintf(szBuffer, m_szStrBuf, pCategory); 
    LogAddEntry(szBuffer, actError, FALSE);
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method: FormatDBString
 *
 * Description: This method formats a database description variables into a
 *          formated string.
 *
 * Parameter(s): 
 *
 * Return Value(s): the length of the formated string
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
int CPLogging::FormatDBString(TCHAR *szBuf, 
                                    char  *pName, 
                                    DWORD  dwCreator,
                                    DWORD  dwType, 
                                    WORD   dbFlags,
                                    WORD   dbVersion) 
{
    int iValue;
    char szCreator[16];
    _itoa( dwCreator, szCreator, 16 );
    
    if (m_sDebugLevel != CHECK_LEVEL_0) 
    {
        LoadStr(IDS_DATABASE_DESCRIPTION);
        iValue = wsprintf(szBuf, 
                          m_szStrBuf, 
                          pName,
                          szCreator,
                          dwType,
                          dbFlags,
                          dbVersion);
    } 
    else
    {
        LoadStr(IDS_DATABASE_SHORT_DESC);
        iValue = wsprintf(szBuf, 
                          m_szStrBuf, 
                          pName,
                          szCreator);
    }
    return iValue;
}
void CPLogging::LogInfo(const char *pMessage)
{
    LogBanner();
#ifndef NO_HSLOG
    LogAddEntry(pMessage, slText, FALSE);
#endif
}

void CPLogging::LogWarning(const char *pMessage)
{
    LogBanner();
#ifndef NO_HSLOG
    LogAddEntry(pMessage, slWarning, FALSE);
#endif
}

/******************************************************************************
 *
 * Class:   CPLogging
 *
 * Method:  FormatRecordString()
 *
 * Description: This method is used to format a record's data into a log-able
 *          string. This is used to inform the user of problems during
 *          synchronization.
 *
 * Parameter(s): rec - CPalmRecord to use to create a record description.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  11/21/2000    KRM         initial revision
 *
 *****************************************************************************/
int CPLogging::FormatRecordString(CPalmRecord &rec)
{
    int iSize = sizeof(m_szRecBuf);
    return rec.FormatRecordString(m_szRecBuf, &iSize);
}
