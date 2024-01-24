/*****************************************************************************
 *
 * Generic Conduit Export Functions Header File (�� NNsiMona�p�Ɉꕔ�ύX)
 *
 * Copyright (c) 2000-2003 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/

#ifndef  __GENERIC_MAIN_CONDUIT__
#define  __GENERIC_MAIN_CONDUIT__
#include <condapi.h>

//  �R���W�b�g�̃o�[�W�����L�q
#define GENERIC_CONDUIT_VERSION 0x00000102    // NNsiMona(�ύX)

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