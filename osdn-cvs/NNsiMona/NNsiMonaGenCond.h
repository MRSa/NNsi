/*****************************************************************************
 *
 * Generic Conduit Export Functions Header File
 *
 ****************************************************************************/


#ifndef  __GENERIC_MAIN_CONDUIT__
#define  __GENERIC_MAIN_CONDUIT__
#include <condapi.h>

#define GENERIC_CONDUIT_VERSION 0x00000102

#ifndef ExportFunc
#ifdef _68K_
#define ExportFunc
#else
#define ExportFunc __declspec( dllexport )
#endif
#endif

//typedef struct NNsiMonaCfgConduitInfoType {
//    DWORD dwVersion;
//    DWORD dwSize;
//    DWORD dwCreatorId;
//    DWORD dwUserId;
//    TCHAR szUser[64];
//    char  m_PathName[BIG_PATH];     
//    eSyncTypes syncPermanent;
//    eSyncTypes syncTemporary;
//    eSyncTypes syncNew;
//    eSyncPref  syncPref;
//	DWORD m_2chBrowserKind;
//	TCHAR m_2chBrowserInstalledDirectory[BIG_PATH];
//	TCHAR m_2chBrowserStoredLogDirectory[BIG_PATH];
//} NNSIMONACFGCONDUITINFO;

extern "C" {

typedef  long (*PROGRESSFN) (char*);
ExportFunc long OpenConduit(PROGRESSFN, CSyncProperties&);
ExportFunc long GetConduitName(char*,WORD);
ExportFunc DWORD GetConduitVersion();
ExportFunc long ConfigureConduit(CSyncPreference& pref);
ExportFunc long GetConduitInfo(ConduitInfoEnum infoType, void *pInArgs, void *pOut, DWORD *dwOutSize);

#ifdef macintosh
	pascal OSErr __initialize(const CFragInitBlock *theInitBlock);
	pascal void __terminate(void);
	pascal OSErr ConduitInit(const CFragInitBlock *theInitBlock);
	pascal void ConduitExit(void);
#endif
}


#endif


