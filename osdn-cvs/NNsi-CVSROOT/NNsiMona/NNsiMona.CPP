//////////////////////////////////////////////////////////////////////////////
////////////////////// NNsi用にGenCond.CPPを変更する  ////////////////////////
////////////////////// (Visual C++専用のコードに変更) ////////////////////////
/// ※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※ ///
/// ※   HotSyncを実施するクラスを CSynchronizer → NNsiCSynchronizerに ※ ///
/// ※ 修正したのがミソです。                                           ※ ///
/// ※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※ ///
//////////////////////////////////////////////////////////////////////////////
/* ****************************************************************************
 *
 *     File:    NNsiMona.CPP(GenCond.cpp)
 *
 *     Module:  NNsiMona.DLL(GenCond.DLL)
 *
 * Copyright (c) 2000-2003 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 *****************************************************************************/
#define STRICT 1
#define ASSERT(f)     ((void)0)
#define TRACE0(sz)
#define TRACE(sz)

#define HELP_FILE     "hotsync.hlp"

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <sys/stat.h>
#include <TCHAR.H>
#include <COMMCTRL.H>
#include <syncmgr.h>
#include "NNsiMona.h"  // ← gencond.h からNNsiMona.hへ変更
#include <logstrng.h>
#include <gensync.h>
#include <PALM_CMN.H>
#include <Pgenerr.h>
#include "resource.h"
#include "NNsiSync.H"

       HANDLE hLangInstance = NULL;
       HANDLE hAppInstance  = NULL;
//extern HANDLE hLangInstance;
//extern HANDLE hAppInstance;

long CALLBACK ConfigureDlgProc (HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void          LoadCfgDlgBitmaps(HWND hDlg);

static int iTerminationCount = 0;

/////////////////////////////////////////////////////////////////////////////
//
//     Function:        DllMain()
//
//     Description:    main entry point to the date book component
//
//     Parameters:    hInstance - instance handle of the DLL
//                    dwReason  - why the entry point was called
//                    lpReserved - reserved
//
//     Returns:        1 if okay
//
/////////////////////////////////////////////////////////////////////////////
extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{

    switch (dwReason)
    {
      case DLL_PROCESS_ATTACH:
        // NNsiMona.DLLの使用開始
        TRACE0("NNsiMona.DLL Initializing!\n");
        if (iTerminationCount == 0)
        {
            DWORD dwVersion;
            hAppInstance  = hInstance;
            hLangInstance = (HANDLE) PalmLoadLanguage("GenCLANG.DLL", hInstance, &dwVersion);
        }
        iTerminationCount = iTerminationCount + 1;
        break;

      case DLL_PROCESS_DETACH:
        // NNsiMona.DLLの終了
        TRACE0("NNsiMona.DLL Terminating!\n");

        iTerminationCount = iTerminationCount - 1;

        if (iTerminationCount == 0)
        {
            if (PalmFreeLanguage((HINSTANCE)hLangInstance, (HINSTANCE)hAppInstance))
            {
                hLangInstance = hAppInstance;
            }
        }
        break;

      default:
        // 何もしない
        break;
    }

    // OK応答(らしい)
    return (1);
}

/////////////////////////////////////////////////////////////////////////////
//
//       Function:              GetConduitName()
//
//       Description:  Extern "C" entry point into this conduit which returns
//                                 the name to be used when display messages regarding
//                                 this conduit.
//
//       Parameters:   pszName - buffer in which to place the name
//                                 nLen - maximum number of bytes of buffer
//
//       Returns:          -1 indicates errors
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc long GetConduitName(char* pszName, WORD nLen)
{
    long retval = -1;

    if (::LoadString((HINSTANCE)hLangInstance, IDS_CONDUIT_NAME, pszName, nLen))
    {
        // コンジット名の読み出しに成功
        retval = 0;
    }
    return (retval);
}

/////////////////////////////////////////////////////////////////////////////
//
//       Function:     GetConduitVersion()
//
//       Description:  Extern "C" entry point into this conduit which returns
//                     the conduits version
//
//       Parameters:   none
//
//       Returns:      DWORD indicating major and minor version number
//                        HIWORD - reserved
//                        HIBYTE(LOWORD) - major number
//                        LOBYTE(LOWORD) - minor number
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc DWORD GetConduitVersion()
{
    // バージョン番号を応答する(シンボルは、NNsiMona.H に定義されている)
    return (GENERIC_CONDUIT_VERSION);
}

/////////////////////////////////////////////////////////////////////////////
//
//       Function:     ConfigureConduit
//
//       Description:  Extern "C" entry point into this conduit which returns
//                     should display the UI necessary to configure this 
//                       conduit.
//
//       Parameters:   none
//
//       Returns:      0 - success, !0 - failure
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc long ConfigureConduit(CSyncPreference& pref)
{
    long                nRtn = -1;
    CfgConduitInfoType  cfg;

    cfg.dwVersion     = CFGCONDUITINFO_VERSION_1;
    cfg.dwSize        = sizeof(CfgConduitInfoType);
    cfg.dwCreatorId   = 0;
    cfg.dwUserId      = 0;

    memset(cfg.szUser ,    0x00, sizeof(cfg.szUser));
    memset(cfg.m_PathName, 0x00, sizeof(cfg.m_PathName));
    
    cfg.syncPermanent = pref.m_SyncType;
    cfg.syncTemporary = pref.m_SyncType;
    cfg.syncNew       = pref.m_SyncType;
    cfg.syncPref      = eTemporaryPreference; 

    int iResult;
    iResult = DialogBoxParam((HINSTANCE) hLangInstance, 
                             MAKEINTRESOURCE(IDD_CONDUIT_ACTION), 
                             GetForegroundWindow(), 
                             (DLGPROC)   ConfigureDlgProc,
                             (LPARAM)    &cfg);
    if (iResult == 0) 
    {
        pref.m_SyncType = cfg.syncNew;
        pref.m_SyncPref = cfg.syncPref;
        nRtn            = 0;
    }
    return (nRtn);
}

/////////////////////////////////////////////////////////////////////////////
//
//     Method:        GetConduitInfo
//
//     Description:    This function provides a way for a Conduit to provide info
//                  to the caller. 
//                  In this version of the call, MFC Version, Conduit Name, and
//                  Default sync action are the types of information this call
//                  will return.
//
//     Parameters:    ConduitInfoEnum infoType - enum specifying what info is being
//                          requested.
//                  void *pInArgs - This parameter may be null, except for the Conduit
//                          name enum, this value will be a ConduitRequestInfoType structure.
//                  This following to parameters vary depending upon the info being requested. 
//                  For enum eConduitName
//                  void *pOut - will be a pointer to a character buffer
//                  DWORD *pdwOutSize - will be a pointer to a DWORD specifying the size of the character buffer.
//
//                  For enum eMfcVersion
//                  void *pOut - will be a pointer to a DWORD
//                  DWORD *pdwOutSize - will be a pointer to a DWORD specifying the size of pOut.
//
//                  For enum eDefaultAction
//                  void *pOut - will be a pointer to a eSyncType variable
//                  DWORD *pdwOutSize - will be a pointer to a DWORD specifying the size of pOut.
//
//     Returns:        0        - Success.
//                    !0      - error code.
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc long GetConduitInfo(ConduitInfoEnum infoType, void *pInArgs, void *pOut, DWORD *pdwOutSize)
{
    if (pOut == NULL)
    {
        return (CONDERR_INVALID_PTR);
    }
    if (pdwOutSize == NULL)
    {
        return (CONDERR_INVALID_OUTSIZE_PTR);
    }

    switch (infoType) 
    {
      case eConduitName:
        if (pInArgs == NULL)
        {
            return (CONDERR_INVALID_INARGS_PTR);
        }
        ConduitRequestInfoType *pInfo;
        pInfo = (ConduitRequestInfoType *) pInArgs;

        if ((pInfo->dwVersion != CONDUITREQUESTINFO_VERSION_1) ||
            (pInfo->dwSize    != SZ_CONDUITREQUESTINFO))
        {
            return (CONDERR_INVALID_INARGS_STRUCT);
        }

        if (!::LoadString((HINSTANCE) hLangInstance, IDS_CONDUIT_NAME, (TCHAR*)pOut, *pdwOutSize))
        {
            return (CONDERR_CONDUIT_RESOURCE_FAILURE);
        }
        break;

      case eDefaultAction:
        if (*pdwOutSize != sizeof(eSyncTypes))
        {
            *pdwOutSize = sizeof(eSyncTypes);
            return (CONDERR_INVALID_BUFFER_SIZE);
        }
        (*(eSyncTypes*)pOut) = eFast;
        break;

      case eMfcVersion:
        if (*pdwOutSize != sizeof(DWORD))
        {
            return (CONDERR_INVALID_BUFFER_SIZE);
        }
        (*(DWORD*)pOut) = MFC_NOT_USED;
        break;

      default:
        return (CONDERR_UNSUPPORTED_CONDUITINFO_ENUM);
        break;
    }
    return (0);
}

/////////////////////////////////////////////////////////////////////////////
//
//     Method:        CfgConduit
//
//     Description:    This is the second instance of ConfigureConduit. This function
//                  is provided with more info from the caller.
//                  This version of the function only supports the eConfig1 enum.
//
//     Parameters:    ConduitCfgEnum cfgType - enum specifying the version of the 
//                      CfgConduit Call.
//                  void *pArgs - In the eConfig1 call, this is a pointer to aCfgConduitInfoType 
//                      structure. This structure provides extra info to the conduit.
//                  DWORD *pdwArgsSize - This is a pointer to a DWORD specifying the size of pArgs.
//
//     Returns:        0        - Success.
//                    !0      - error code.
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc long CfgConduit(ConduitCfgEnum cfgType, void *pArgs, DWORD *pdwArgsSize)
{
    long                    nRtn = -1;
    TCHAR                   szName[256];
    DWORD                   dwNamesize;
    ConduitRequestInfoType  infoStruct;
    CfgConduitInfoType     *pCfgInfo;

    dwNamesize = sizeof(szName);

    if (pArgs == NULL)
    {
        return (CONDERR_INVALID_INARGS_PTR);
    }
    if (pdwArgsSize == NULL)
    {
        return (CONDERR_INVALID_ARGSSIZE_PTR);
    }
    if (*pdwArgsSize != SZ_CFGCONDUITINFO)
    { 
        *pdwArgsSize = SZ_CFGCONDUITINFO;
        return (CONDERR_INVALID_ARGSSIZE);
    }
    if (cfgType != eConfig1)
    {
        return (CONDERR_UNSUPPORTED_CFGCONDUIT_ENUM);
    }
    pCfgInfo = (CfgConduitInfoType *)pArgs;
    if (pCfgInfo->dwVersion != CFGCONDUITINFO_VERSION_1)
    {
        return (CONDERR_UNSUPPORTED_STRUCT_VERSION);
    }
    infoStruct.dwVersion   = CONDUITREQUESTINFO_VERSION_1;
    infoStruct.dwSize      = SZ_CONDUITREQUESTINFO;
    infoStruct.dwCreatorId = pCfgInfo->dwCreatorId;
    infoStruct.dwUserId    = pCfgInfo->dwUserId;
    _tcscpy(infoStruct.szUser, pCfgInfo->szUser);

    nRtn = GetConduitInfo(eConduitName, (void *)&infoStruct, (void *)szName, &dwNamesize);
    if (nRtn != 0)
    {
        return (nRtn);
    }
    int iResult;
    iResult = DialogBoxParam((HINSTANCE)hLangInstance, 
                             MAKEINTRESOURCE(IDD_CONDUIT_CFG_DETAILED), 
                             GetForegroundWindow(), 
                             (DLGPROC)ConfigureDlgProc,
                             (LPARAM)pCfgInfo);
    return (0);
}

/////////////////////////////////////////////////////////////////////////////
//     Method:        ConfigureDlgProc
//
/////////////////////////////////////////////////////////////////////////////
long CALLBACK ConfigureDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static CfgConduitInfoType *pCfgInfo;
    TCHAR szPath[256];

    switch (Message)
    {
      case WM_INITDIALOG:
        if (lParam != 0)
        {
            TCHAR szBuffer[256];
            TCHAR szBuf2  [256];
            LoadString((HINSTANCE)hLangInstance, IDS_SYNC_ACTION_TEXT, szBuffer, sizeof(szBuffer));
            SetDlgItemText(hWnd, IDC_ACTIONGROUPBOXTEXT, szBuffer);

            LoadString((HINSTANCE)hLangInstance, IDS_CONDUIT_NAME, szBuffer, sizeof(szBuffer));
            SetWindowText(hWnd, szBuffer);
     
            // Load the bitmaps properly
            LoadCfgDlgBitmaps(hWnd);

            pCfgInfo = (CfgConduitInfoType *)lParam;
            switch (pCfgInfo->syncTemporary)
            {
              case eFast:
              case eSlow:
                CheckRadioButton(hWnd, IDC_RADIO_SYNC, IDC_RADIO_DONOTHING, IDC_RADIO_SYNC);
                LoadString((HINSTANCE)hLangInstance, IDS_SYNC_FILES, szBuffer, sizeof(szBuffer));
                break;

              case ePCtoHH:
                CheckRadioButton(hWnd, IDC_RADIO_SYNC, IDC_RADIO_DONOTHING, IDC_RADIO_PCTOHH);
                LoadString((HINSTANCE)hLangInstance, IDS_PCTOHH, szBuffer, sizeof(szBuffer));
                break;

              case eHHtoPC:
                CheckRadioButton(hWnd, IDC_RADIO_SYNC, IDC_RADIO_DONOTHING, IDC_RADIO_HHTOPC);
                LoadString((HINSTANCE)hLangInstance, IDS_HHTOPC, szBuffer, sizeof(szBuffer));
                break;

              case eDoNothing:
              default:
                CheckRadioButton(hWnd, IDC_RADIO_SYNC, IDC_RADIO_DONOTHING, IDC_RADIO_DONOTHING);
                LoadString((HINSTANCE)hLangInstance, IDS_DO_NOTHING, szBuffer, sizeof(szBuffer));
                break;
           }
           // did we get called from the old config call or the new cfg call?
           // (旧バージョンの設定を呼び出したか、それとも新しい設定を呼び出したかチェック)
           if (pCfgInfo->dwCreatorId != 0)
           {
               SetDlgItemText(hWnd, IDC_STATIC_TEMPORARY, szBuffer);

               switch (pCfgInfo->syncPermanent)
               {
                 case eFast:
                 case eSlow:
                   LoadString((HINSTANCE)hLangInstance, IDS_SYNC_FILES, szBuffer, sizeof(szBuffer));
                   break;

                 case ePCtoHH:
                   LoadString((HINSTANCE)hLangInstance, IDS_PCTOHH, szBuffer, sizeof(szBuffer));
                   break;

                 case eHHtoPC:
                   LoadString((HINSTANCE)hLangInstance, IDS_HHTOPC, szBuffer, sizeof(szBuffer));
                   break;

                 case eDoNothing:
                 default:
                   LoadString((HINSTANCE)hLangInstance, IDS_DO_NOTHING, szBuffer, sizeof(szBuffer));
                   break;
               }
               SetDlgItemText(hWnd, IDC_STATIC_PERMANENT, szBuffer);

               LoadString((HINSTANCE)hLangInstance, IDS_CURRENT_SETTINGS_GROUP, szBuffer, sizeof(szBuffer));
               wsprintf(szBuf2, szBuffer, pCfgInfo->szUser);
               SetDlgItemText(hWnd, IDC_CURRENT_SETTINGS_GROUP, szBuf2);
            }
        }
        break;

      case WM_COMMAND:
        switch (wParam)
        {
          case IDC_RADIO_SYNC:
          case IDC_RADIO_PCTOHH:
          case IDC_RADIO_HHTOPC:
          case IDC_RADIO_DONOTHING:
            CheckRadioButton(hWnd, IDC_RADIO_SYNC, IDC_RADIO_DONOTHING, wParam);
            break;

          case IDCANCEL:
            EndDialog(hWnd, 1);
            return (TRUE);
            break;

          case IDOK:
            if (IsDlgButtonChecked(hWnd, IDC_RADIO_SYNC))
            {
                pCfgInfo->syncNew = eFast;
            }
            else if (IsDlgButtonChecked(hWnd, IDC_RADIO_PCTOHH))
            {
                pCfgInfo->syncNew = ePCtoHH;
            }
            else if (IsDlgButtonChecked(hWnd, IDC_RADIO_HHTOPC))
            {
                pCfgInfo->syncNew = eHHtoPC;
            }
            else
            {
                pCfgInfo->syncNew = eDoNothing;
            }
            if (IsDlgButtonChecked(hWnd, IDC_MAKEDEFAULT))
            {
                pCfgInfo->syncPref = ePermanentPreference;
            }
            else
            {
                pCfgInfo->syncPref = eTemporaryPreference;
            }
            EndDialog(hWnd, 0);
            return (TRUE);
            break;

          case IDHELP:
            if (GetModuleFileName((HINSTANCE)hAppInstance, szPath, sizeof(szPath)))
            {
                // Look for the end of the path
                int        fileOffset;
                fileOffset = _tcslen(szPath);
                for (; fileOffset > 0; fileOffset--)
                {
                    if (szPath[fileOffset] == '\\')
                    {
                        break;
                    }
                }
                if (fileOffset > 0)
                {
                    szPath[fileOffset + 1] = 0;
                    _tcscat(szPath, HELP_FILE);
                    WinHelp(hWnd, szPath, HELP_CONTEXT, IDD_CONDUIT_ACTION + 0x20000 /*base resource*/);
                }
            }
            return (TRUE);
            break;

          default:
            break;
        }
        break;

      case WM_HELP:
        if (GetModuleFileName((HINSTANCE)hAppInstance, szPath, sizeof(szPath)))
        {
            // Look for the end of the path
            int        fileOffset;
            fileOffset = _tcslen(szPath);
            for (; fileOffset > 0; fileOffset--)
            {
               if (szPath[fileOffset] == '\\')
               {
                   break;
               }
            }
            if (fileOffset > 0)
            {
                szPath[fileOffset + 1] = 0;
                _tcscat(szPath, HELP_FILE);
                WinHelp(hWnd, szPath, HELP_CONTEXT, IDD_CONDUIT_ACTION + 0x20000 /*base resource*/);
            }
        }
        return (TRUE);
        break;

      case WM_SYSCOLORCHANGE:
        LoadCfgDlgBitmaps(hWnd);
        break;

      default:
        break;
    }
    return (FALSE);
}

/////////////////////////////////////////////////////////////////////////////
//
//     Method:        LoadCfgDlgBitmaps
//
//     Description:    This routines translates the bitmap gray background to the
//                    button face color set in the control panel/window appearances
//
//     Parameters:    HWND    - hDlg, handle of the dialog
//
//     Returns:        none
//
/////////////////////////////////////////////////////////////////////////////
void LoadCfgDlgBitmaps(HWND hDlg)
{
    COLORMAP        colorMap;
    HWND            hwndButton;
    HBITMAP         hBitmap, hOldBitmap;

    //
    // setup the bitmaps
    //
    colorMap.to   = GetSysColor(COLOR_BTNFACE);
    colorMap.from = RGB(192,192,192);

    // Sync 
    hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_SYNC, 0, &colorMap, 1);

    // associate the bitmap with the button.
    if ((hwndButton = GetDlgItem(hDlg, IDC_SYNC)) != NULL)
    {
        hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
        if (hOldBitmap != NULL)
        {
            DeleteObject((HGDIOBJ)hOldBitmap);
        }
    }

    // PC to HH 
    hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_PCTOHH, 0, &colorMap, 1);

    // associate the bitmap with the button.
    if ((hwndButton = GetDlgItem(hDlg, IDC_PCTOHH)) != NULL)
    {
        hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
        if (hOldBitmap != NULL)
        {
            DeleteObject((HGDIOBJ)hOldBitmap);
    }
    }

    // HH to PC 
    hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_HHTOPC, 0, &colorMap, 1);
    // associate the bitmap with the button.
    if ((hwndButton = GetDlgItem(hDlg, IDC_HHTOPC)) != NULL)
    {
        hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
        if (hOldBitmap != NULL)
        {
            DeleteObject((HGDIOBJ)hOldBitmap);
        }
    }

    // Do Nothing 
    hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_DONOTHING, 0, &colorMap, 1);
    // associate the bitmap with the button.
    if ((hwndButton = GetDlgItem(hDlg, IDC_DONOTHING)) != NULL)
    {
        hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
        if (hOldBitmap != NULL)
        {
            DeleteObject((HGDIOBJ)hOldBitmap);
        }
    }
    return;
}


/////////////////////////////////////////////////////////////////////////////
//
//     Function:        OpenConduit()
//
//     Description:  Extern "C" entry point into this conduit which starts the
//                 process of synchronizing the local database table with
//                 a remote counterpart residing on the remote view device. 
//
//     Parameters:   Pointer to a callback function used to report progress.
//                    
//                
//
//     Returns:        
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc long OpenConduit(PROGRESSFN pFn, CSyncProperties& rProps)
{
    long retval = -1;

    if (pFn != NULL)
    {
        NNsiCSynchronizer* pGeneric;

        // NNsiの同期クラスを作成する
        pGeneric = new NNsiCSynchronizer(hLangInstance, rProps, GENERIC_FLAG_CATEGORY_SUPPORTED | GENERIC_FLAG_APPINFO_SUPPORTED);
        if (pGeneric != NULL)
        {
            // HotSync作業の実施したあと、同期クラスを削除し終了する
            retval = pGeneric->Perform();
            delete pGeneric;
        }
    }
    return (retval);
}
