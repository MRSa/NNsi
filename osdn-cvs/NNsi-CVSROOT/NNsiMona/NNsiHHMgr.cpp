///////////////////////////////////////////////////////
//// NNsiCHHMgr.CPP  : NNsi�p�̃n���h�w���h�����N���X
///////////////////////////////////////////////////////
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <sys/stat.h>
#include <TCHAR.H>
#include <syncmgr.h>
#include <hhmgr.h>
#include <Pgenerr.h>
#include "NNsiHHMgr.h"

// �R���X�g���N�^�P
NNsiCHHMgr::NNsiCHHMgr(CPLogging *pLogging, DWORD dwGenericFlags, eSyncTypes syncType):CHHMgr(pLogging, dwGenericFlags, syncType)
{

}

// �R���X�g���N�^�Q
NNsiCHHMgr::NNsiCHHMgr(CPLogging *pLogging, DWORD dwGenericFlags, char  *pName, DWORD  dwCreator, DWORD  dwType, WORD   dbFlags, WORD   dbVersion, int  iCardNum, eSyncTypes syncType ):CHHMgr(pLogging, dwGenericFlags, pName, dwCreator, dwType, dbFlags, dbVersion, iCardNum, syncType)
{

}
