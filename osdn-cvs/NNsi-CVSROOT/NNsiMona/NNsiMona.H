/*****************************************************************************
 *
 * Generic Conduit Export Functions Header File (を NNsiMona用に一部変更)
 *
 * Copyright (c) 2000-2003 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/

#ifndef  __GENERIC_MAIN_CONDUIT__
#define  __GENERIC_MAIN_CONDUIT__
#include <condapi.h>

//  コンジットのバージョン記述
#define GENERIC_CONDUIT_VERSION 0x00000102    // NNsiMona(変更)

#define ExportFunc __declspec(dllexport)

extern "C" 
{
    typedef    long  (*PROGRESSFN) (char*);

    ExportFunc long  OpenConduit(PROGRESSFN, CSyncProperties&);
    ExportFunc long  GetConduitName(char*,WORD);
    ExportFunc DWORD GetConduitVersion();
    ExportFunc long  ConfigureConduit(CSyncPreference& pref);
    ExportFunc long  GetConduitInfo(ConduitInfoEnum infoType, void *pInArgs, void *pOut, DWORD *dwOutSize);
}

#endif
