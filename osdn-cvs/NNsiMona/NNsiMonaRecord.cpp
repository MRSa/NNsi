/*****************************************************************************
 *
 * Generic Conduit - DateBook CDatebookSync Src File
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
#include "NNsiMonaRecord.h"


//***********************************************************
CNNsiMonaRecord::CNNsiMonaRecord(void)
{
    Reset();
}


CNNsiMonaRecord::~CNNsiMonaRecord()
{

}


void CNNsiMonaRecord::Reset(void)
{
    m_dwRecordID = 0;
    m_dwStatus = 0;
    m_dwPosition = 0;
    m_dwPrivate = 0;
    m_dwCategoryID = 0;
    m_csMemo.Empty();
}



/**************************************************************************/


