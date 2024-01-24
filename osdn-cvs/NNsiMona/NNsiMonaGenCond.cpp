/*****************************************************************************
 *
 * CNNsiMonaGenCond Generic Conduit Entry Points Source File
 *
 ****************************************************************************/
#define STRICT 1
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
#include <COMMCTRL.H>


#include <syncmgr.h>
#include "NNsiMonaGenCond.h"

#include <logstrng.h>
#include "NNsiMonaSync.h"


#include <PALM_CMN.H>
#include "resource.h"

HANDLE hLangInstance;
HANDLE hAppInstance;
extern HANDLE hLangInstance;
extern HANDLE hAppInstance;

long CALLBACK ConfigureDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
void LoadCfgDlgBitmaps(HWND hDlg);

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
//
//     Function:        DllMain()
//
//     Description:    main entry point to the NNsiMona component
//
//     Parameters:    hInstance - instance handle of the DLL
//                    dwReason  - why the entry point was called
//                    lpReserved - reserved
//
//     Returns:        1 if okay
//
/////////////////////////////////////////////////////////////////////////////
//     REVISION HISTORY:
//      03/25/98 KRM inital 
//
/////////////////////////////////////////////////////////////////////////////
#ifndef _68K_

static int iTerminationCount = 0;

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        TRACE0("CNNsiMonaGenCond Initializing!\n");
        
        if (!iTerminationCount ) {
            hAppInstance = hInstance;
			// use PalmLoadLanguage here to load different languages
            hLangInstance = hInstance;
		        }
        ++iTerminationCount;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        TRACE0("CNNsiMonaGenCond Terminating!\n");

        --iTerminationCount;
        if (!iTerminationCount ) {
			// use PalmFreeLanguage here to unload different languages
        }
    }
    return 1;   // ok
}
#endif


/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
//
//     Function:        OpenConduit()
//
//     Description:  Extern "C" entry point into this conduit which starts the
//                 process of synchronizing the local database table with
//                 a remote conterpart residing on the remote view device. 
//
//     Parameters:   Pointer to a callback function used to report progress.
//                    
//                
//
//     Returns:        
//
/////////////////////////////////////////////////////////////////////////////
//     REVISION HISTORY:
//      03/25/98 KRM inital 
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc long OpenConduit(PROGRESSFN pFn, CSyncProperties& rProps)
{
    long retval = -1;
    if (pFn)
    {
		HANDLE  hfile;
		DWORD readByte;
		NNsiMonaPreferences preference;
		memset(&preference, 0x00, sizeof(NNsiMonaPreferences));
		preference.waitTime = 1500;
		preference.syncType = eInstall;
		hfile = CreateFile(NNSIMONA_PREFERENCE_FILENAME, (GENERIC_READ|GENERIC_WRITE), (FILE_SHARE_READ|FILE_SHARE_WRITE), NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hfile != INVALID_HANDLE_VALUE)
		{
			// データを読み込む
			ReadFile(hfile, &preference, sizeof(NNsiMonaPreferences), &readByte, NULL);
			CloseHandle(hfile);
		}

		// HotSyncのクラスを生成する...
		CNNsiMonaSync* pGeneric;
		pGeneric = new CNNsiMonaSync(rProps, GENERIC_FLAG_CATEGORY_SUPPORTED | GENERIC_FLAG_APPINFO_SUPPORTED );
        if (pGeneric)
		{
			// パラメータを渡す...
			pGeneric->setPreferences(&preference);

			// HotSyncの実行...
			retval = pGeneric->Perform();
            delete pGeneric;
        }
    }
    return(retval);
}

/////////////////////////////////////////////////////////////////////////////
//
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
//                              
//
//       Returns:          -1 indicates erros
//
/////////////////////////////////////////////////////////////////////////////
//       REVISION HISTORY:
//      03/25/98 KRM inital 
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc long GetConduitName(char* pszName,WORD nLen)
{
    long retval = -1;

    if (::LoadString((HINSTANCE)hLangInstance, IDS_CONDUIT_NAME, pszName, nLen))
        retval = 0;

    return retval;
}

/////////////////////////////////////////////////////////////////////////////
//
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
//       REVISION HISTORY:
//      03/25/98 KRM inital 
/////////////////////////////////////////////////////////////////////////////
ExportFunc DWORD GetConduitVersion()
{
    return GENERIC_CONDUIT_VERSION;
}

/////////////////////////////////////////////////////////////////////////////
//
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
//       REVISION HISTORY:
//      03/25/98 KRM inital 
/////////////////////////////////////////////////////////////////////////////
ExportFunc long ConfigureConduit(CSyncPreference& pref)
{

    long nRtn = -1;
    CfgConduitInfoType cfg;
    cfg.dwVersion = CFGCONDUITINFO_VERSION_1;
    cfg.dwSize  = sizeof(CfgConduitInfoType);
    cfg.dwCreatorId = 0;
    cfg.dwUserId = 0;
    memset(cfg.szUser , 0, sizeof(cfg.szUser));
    memset(cfg.m_PathName, 0, sizeof(cfg.m_PathName));
    cfg.syncPermanent = pref.m_SyncType;
    cfg.syncTemporary = pref.m_SyncType;
    cfg.syncNew = pref.m_SyncType;
    cfg.syncPref = eTemporaryPreference; 

	//cfg.m_2chBrowserKind = 0;
	//memset(cfg.m_2chBrowserInstalledDirectory, 0, sizeof(cfg.m_2chBrowserInstalledDirectory));
	//strcpy(cfg.m_2chBrowserInstalledDirectory, "C:\\APL");
	//memset(cfg.m_2chBrowserStoredLogDirectory, 0, sizeof(cfg.m_2chBrowserStoredLogDirectory));
	//strcpy(cfg.m_2chBrowserStoredLogDirectory, "C:\\APL");

    int iResult;
    iResult = DialogBoxParam((HINSTANCE)hLangInstance, 
              MAKEINTRESOURCE(IDD_CONDUIT_ACTION), 
              GetForegroundWindow(), 
              (DLGPROC)ConfigureDlgProc,
              (LPARAM)&cfg);
    if (iResult == 0) {
        pref.m_SyncType = cfg.syncNew;
        pref.m_SyncPref = cfg.syncPref;
        nRtn = 0;
    }
    return nRtn;
}

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
//
//	 Method:		GetConduitInfo
//
//	 Description:	This function provides a way for a Conduit to provide info
//                  to the caller. 
//                  In this version of the call, MFC Version, Conduit Name, and
//                  Default sync action are the types of information this call
//                  will return.
//
//	 Parameters:	ConduitInfoEnum infoType - enum specifying what info is being
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
//	 Returns:		0		- Success.
//					!0      - error code.
//
/////////////////////////////////////////////////////////////////////////////
//	 REVISION HISTORY:
//		01/23/98    KRM Created
//
/////////////////////////////////////////////////////////////////////////////
ExportFunc long GetConduitInfo(ConduitInfoEnum infoType, void *pInArgs, void *pOut, DWORD *pdwOutSize)
{
    if (!pOut)
        return CONDERR_INVALID_PTR;
    if (!pdwOutSize)
        return CONDERR_INVALID_OUTSIZE_PTR;

    switch (infoType) {
        case eConduitName:

            // This code is for example. This conduit does not use this code
            
            if (!pInArgs)
                return CONDERR_INVALID_INARGS_PTR;
            ConduitRequestInfoType *pInfo;
            pInfo = (ConduitRequestInfoType *)pInArgs;
            if ((pInfo->dwVersion != CONDUITREQUESTINFO_VERSION_1) ||
                (pInfo->dwSize != SZ_CONDUITREQUESTINFO))
                return CONDERR_INVALID_INARGS_STRUCT;
            
	        if (!::LoadString((HINSTANCE)hLangInstance, IDS_CONDUIT_NAME, (TCHAR*)pOut, *pdwOutSize))
                return CONDERR_CONDUIT_RESOURCE_FAILURE;
            break;
        case eDefaultAction:
            if (*pdwOutSize != sizeof(eSyncTypes))
                return CONDERR_INVALID_BUFFER_SIZE;
            (*(eSyncTypes*)pOut) = eFast;
            break;
        case eMfcVersion:
            if (*pdwOutSize != sizeof(DWORD))
                return CONDERR_INVALID_BUFFER_SIZE;
            (*(DWORD*)pOut) = MFC_NOT_USED;
            break;
        default:
            return CONDERR_UNSUPPORTED_CONDUITINFO_ENUM;
    }
    return 0;
}


////////////////////////////////////===============================///////////

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
//
//	 Method:		CfgConduit
//
//	 Description:	This is the second instance of ConfigureConduit. This function
//                  is provided with more info from the caller.
//                  This version of the function only supports the eConfig1 enum.
//
//	 Parameters:	ConduitCfgEnum cfgType - enum specifying the version of the 
//                      CfgConduit Call.
//                  void *pArgs - In the eConfig1 call, this is a pointer to aCfgConduitInfoType 
//                      structure. This structure provides extra info to the conduit.
//                  DWORD *pdwArgsSize - This is a pointer to a DWORD specifying the size of pArgs.
//
//	 Returns:		0		- Success.
//					!0      - error code.
//
/////////////////////////////////////////////////////////////////////////////
//	 REVISION HISTORY:
//		01/23/98    KRM Created
//
/////////////////////////////////////////////////////////////////////////////

ExportFunc long CfgConduit( ConduitCfgEnum cfgType, void *pArgs, DWORD *pdwArgsSize)
{
	long nRtn = -1;
	TCHAR szName[256];
    DWORD dwNamesize;
    ConduitRequestInfoType infoStruct;
    CfgConduitInfoType *pCfgInfo;


    dwNamesize = sizeof(szName);


    if (!pArgs)
        return CONDERR_INVALID_INARGS_PTR;
    if (!pdwArgsSize)
        return CONDERR_INVALID_ARGSSIZE_PTR;
    if (*pdwArgsSize != SZ_CFGCONDUITINFO) 
        return CONDERR_INVALID_ARGSSIZE;

    if (cfgType != eConfig1)
            return CONDERR_UNSUPPORTED_CFGCONDUIT_ENUM;

    pCfgInfo = (CfgConduitInfoType *)pArgs;
    if (pCfgInfo->dwVersion != CFGCONDUITINFO_VERSION_1)
        return CONDERR_UNSUPPORTED_STRUCT_VERSION;



    infoStruct.dwVersion = CONDUITREQUESTINFO_VERSION_1;
    infoStruct.dwSize = SZ_CONDUITREQUESTINFO;
    infoStruct.dwCreatorId = pCfgInfo->dwCreatorId;
    infoStruct.dwUserId = pCfgInfo->dwUserId;
    strcpy(infoStruct.szUser, pCfgInfo->szUser);
    nRtn = GetConduitInfo(eConduitName, (void *)&infoStruct, (void *)szName, &dwNamesize);
    if (nRtn)
        return nRtn;

    int iResult;
    iResult = DialogBoxParam((HINSTANCE)hLangInstance, 
              MAKEINTRESOURCE(IDD_CONDUIT_CFG_DETAILED), 
              GetForegroundWindow(), 
              (DLGPROC)ConfigureDlgProc,
              (LPARAM)pCfgInfo);

    return 0;
}


/**************************************************************
  HotSync設定ダイアログのコールバック関数
***************************************************************/
long CALLBACK ConfigureDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    long returnValue = FALSE;

	// カレントディレクトリを保存
	char currentDirectory[BIG_PATH];
	GetCurrentDirectory(sizeof(currentDirectory), currentDirectory);
	//MessageBox(hWnd, currentDirectory, "GetCurrentDirectory()", MB_OK);
	
	static CfgConduitInfoType *pCfgInfo;
    TCHAR szPath[BIG_PATH];

	char dirName[BIG_PATH];
	char fileName[BIG_PATH];
	memset(dirName,  0, sizeof(dirName));
	memset(fileName, 0, sizeof(fileName));

	// 2ちゃんねるブラウザが入っているディレクトリの選択...
	OPENFILENAME ofn;
	int retVal;

	switch (Message) {
        case WM_INITDIALOG:
			if (lParam != 0) {
                // ダイアログで設定しているディレクトリを記憶する
				HANDLE  hfile;
				DWORD readByte;
				NNsiMonaPreferences preference;
				memset(&preference, 0x00, sizeof(NNsiMonaPreferences));
				hfile = CreateFile(NNSIMONA_PREFERENCE_FILENAME, (GENERIC_READ|GENERIC_WRITE), (FILE_SHARE_READ|FILE_SHARE_WRITE), NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hfile != INVALID_HANDLE_VALUE)
				{
                    // データを読み込む
					ReadFile(hfile, &preference, sizeof(NNsiMonaPreferences), &readByte, NULL);
					CloseHandle(hfile);
				}
				memset(fileName, 0x00, sizeof(fileName));
				itoa(preference.waitTime, fileName, 10);
				SetDlgItemText(hWnd, IDC_EDIT_LOGWAITTIME, fileName);

				memset(fileName, 0x00, sizeof(fileName));
				strncpy(fileName, preference.browserFileLocation, sizeof(fileName));

				memset(dirName, 0x00, sizeof(dirName));
				strncpy(dirName, preference.browserConfigLocation, sizeof(dirName));

				TCHAR szBuffer[BIG_PATH];
                TCHAR szBuf2[BIG_PATH];
                LoadString((HINSTANCE)hLangInstance, IDS_SYNC_ACTION_TEXT, szBuffer, sizeof(szBuffer));
                SetDlgItemText(hWnd, IDC_ACTIONGROUPBOXTEXT, szBuffer);

                LoadString((HINSTANCE)hLangInstance, IDS_CONDUIT_NAME, szBuffer, sizeof(szBuffer));
                SetWindowText(hWnd, szBuffer);

				LoadString((HINSTANCE)hLangInstance, IDS_2chBROWSER_NAME, szBuffer, sizeof(szBuffer));
                SetDlgItemText(hWnd, IDC_2chBROWSER, szBuffer);
     
				// Load the bitmaps properly
				LoadCfgDlgBitmaps(hWnd);

                pCfgInfo = (CfgConduitInfoType *)lParam;
				pCfgInfo->syncTemporary = preference.syncType;
                switch (pCfgInfo->syncTemporary){
                    case eInstall:
                        CheckRadioButton( hWnd, IDC_RADIO_SYNC, IDC_RADIO_UPDATEFAVOR_2CH, IDC_RADIO_UPDATE_2CH);
                        LoadString((HINSTANCE)hLangInstance, IDS_SYNC_2CHTOHH, szBuffer, sizeof(szBuffer));
                        break;
                    case eSlow:
                        CheckRadioButton( hWnd, IDC_RADIO_SYNC, IDC_RADIO_UPDATEFAVOR_2CH, IDC_RADIO_UPDATEFAVOR_2CH);
                        LoadString((HINSTANCE)hLangInstance, IDS_SYNC_2CH, szBuffer, sizeof(szBuffer));
                        break;
                    case eProfileInstall:
                    case eFast:
                        CheckRadioButton( hWnd, IDC_RADIO_SYNC, IDC_RADIO_UPDATEFAVOR_2CH, IDC_RADIO_SYNC);
                        LoadString((HINSTANCE)hLangInstance, IDS_SYNC_FILES, szBuffer, sizeof(szBuffer));
                        break;
                    case ePCtoHH:
                        CheckRadioButton( hWnd, IDC_RADIO_SYNC, IDC_RADIO_UPDATEFAVOR_2CH, IDC_RADIO_PCTOHH);
                        LoadString((HINSTANCE)hLangInstance, IDS_PCTOHH, szBuffer, sizeof(szBuffer));
                        break;
                    case eHHtoPC:
                        CheckRadioButton( hWnd, IDC_RADIO_SYNC, IDC_RADIO_UPDATEFAVOR_2CH, IDC_RADIO_HHTOPC);
                        LoadString((HINSTANCE)hLangInstance, IDS_HHTOPC, szBuffer, sizeof(szBuffer));
                        break;
                    case eDoNothing:
                    default:
                        CheckRadioButton( hWnd, IDC_RADIO_SYNC, IDC_RADIO_UPDATEFAVOR_2CH, IDC_RADIO_DONOTHING);
                        LoadString((HINSTANCE)hLangInstance, IDS_DO_NOTHING, szBuffer, sizeof(szBuffer));
                        break;
                }
                // did we get called from the old config call or the new cfg call?
                if (pCfgInfo->dwCreatorId != 0) {

                    SetDlgItemText(hWnd, IDC_STATIC_TEMPORARY, szBuffer);

                    switch (pCfgInfo->syncPermanent){
                        case eInstall:
                            LoadString((HINSTANCE)hLangInstance, IDS_SYNC_2CHTOHH, szBuffer, sizeof(szBuffer));
                            break;
                        case eSlow:
                            LoadString((HINSTANCE)hLangInstance, IDS_SYNC_2CH, szBuffer, sizeof(szBuffer));
                            break;
                        case eProfileInstall:
						case eFast:
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

				// 設定しているディレクトリをダイアログに反映させる
				SetDlgItemText(hWnd, IDC_BROWSERDIRECTORY, fileName);

				// 設定しているディレクトリをダイアログに反映させる
				SetDlgItemText(hWnd, IDC_2chBROWSERDIRECTORY, dirName);

				// 警告メッセージ(現バージョンの制約事項)を表示する。
				char msgBuffer[BIG_PATH];
				LoadString((HINSTANCE)hLangInstance, IDS_WARN_ACTION_TEMPORARY, msgBuffer, sizeof(msgBuffer));
				MessageBox(hWnd, msgBuffer, "Warning!", (MB_ICONWARNING|MB_OK));
			}
            break;

		case WM_COMMAND:
            switch (wParam) {
				case IDC_RADIO_UPDATEFAVOR_2CH:
				case IDC_RADIO_UPDATE_2CH:
                case IDC_RADIO_SYNC:
                case IDC_RADIO_PCTOHH:
                case IDC_RADIO_HHTOPC:
                case IDC_RADIO_DONOTHING:
                    CheckRadioButton( hWnd, IDC_RADIO_SYNC, IDC_RADIO_UPDATEFAVOR_2CH, wParam);
                    break;

				case IDCANCEL:
                    EndDialog(hWnd, 1);
                    return TRUE;
					break;

				case IDOK:
                    if (IsDlgButtonChecked(hWnd, IDC_RADIO_SYNC)) {
                        pCfgInfo->syncNew = eFast;
                    } else if (IsDlgButtonChecked(hWnd, IDC_RADIO_PCTOHH)) {
                        pCfgInfo->syncNew = ePCtoHH;
                    } else if (IsDlgButtonChecked(hWnd, IDC_RADIO_HHTOPC)) {
                        pCfgInfo->syncNew = eHHtoPC;
					} else if (IsDlgButtonChecked(hWnd, IDC_RADIO_UPDATEFAVOR_2CH)) {
                        pCfgInfo->syncNew = eSlow;
					} else if (IsDlgButtonChecked(hWnd, IDC_RADIO_UPDATE_2CH)) {
                        pCfgInfo->syncNew = eInstall;
                    } else {
                        pCfgInfo->syncNew = eDoNothing;
                    }
                    if (IsDlgButtonChecked(hWnd, IDC_MAKEDEFAULT)){
                        pCfgInfo->syncPref = ePermanentPreference;
                    } else {
                        pCfgInfo->syncPref = eTemporaryPreference;
                    }

                    // カレントディレクトリを元のものに戻す
	                SetCurrentDirectory(currentDirectory);

					// ダイアログで設定しているディレクトリを記憶する
					GetDlgItemText(hWnd, IDC_EDIT_LOGWAITTIME, fileName, sizeof(fileName));
					int waitTime;
					waitTime = strtoul(fileName, NULL, 10);

					// ダイアログで設定しているディレクトリを記憶する
					GetDlgItemText(hWnd, IDC_BROWSERDIRECTORY, fileName, sizeof(fileName));
					GetDlgItemText(hWnd, IDC_2chBROWSERDIRECTORY, dirName, sizeof(dirName));
					HANDLE  hfile;
					DWORD writtenByte;
					NNsiMonaPreferences preference;
					memset(&preference, 0x00, sizeof(NNsiMonaPreferences));
					strcpy(preference.browserFileLocation, fileName);
					strcpy(preference.browserConfigLocation, dirName);
					preference.syncType = pCfgInfo->syncNew;
					preference.waitTime = waitTime;
					hfile = CreateFile(NNSIMONA_PREFERENCE_FILENAME, (GENERIC_READ|GENERIC_WRITE), (FILE_SHARE_READ|FILE_SHARE_WRITE), NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hfile != INVALID_HANDLE_VALUE)
					{
					    // データを記録する
						WriteFile(hfile, &preference, sizeof(NNsiMonaPreferences), &writtenByte, NULL);
						CloseHandle(hfile);
					}
					// 仮に「データを同期する」に変更する
					if ((pCfgInfo->syncNew == eSlow)||(pCfgInfo->syncNew == eInstall)||(pCfgInfo->syncNew == eProfileInstall))
					{
						pCfgInfo->syncNew = eFast;
					}
					EndDialog(hWnd, 0);
                    return TRUE;
					break;

				case IDHELP:
#ifdef _DEBUG
#define HELP_FILE "hotsyncd.hlp"
#else
#define HELP_FILE "hotsync.hlp"
#endif
					if (GetModuleFileName((HINSTANCE)hAppInstance, szPath, sizeof(szPath)))
					{
						// Look for the end of the path
						int		fileOffset;
						fileOffset = strlen(szPath);
						for (; fileOffset > 0; fileOffset--)
						{
							if (szPath[fileOffset] == '\\')
								break;
						}

						if (fileOffset > 0)
						{
							szPath[fileOffset + 1] = 0;
							strcat(szPath, HELP_FILE);
							WinHelp(hWnd, szPath, HELP_CONTEXT, IDD_CONDUIT_ACTION + 0x20000 /*base resource*/);
						}
					}
					return TRUE;
					break;

				case IDC_2chSELECTBROWSER:
					// 初期ディレクトリの反映...
					memset(dirName, 0, sizeof(dirName));
					memset(&ofn, 0, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					GetDlgItemText(hWnd, IDC_2chBROWSERDIRECTORY, dirName, sizeof(dirName));
					ofn.Flags = 0;
					//ofn.lpstrInitialDir = fileName;
					ofn.lpstrFile = dirName;
					ofn.nMaxFile = sizeof(dirName);

					// ファイル名選択ダイアログを表示する
					retVal = GetOpenFileName(&ofn);
					if (retVal != 0)
					{
                        /***
						// ディレクトリだけを抽出
						char *ptr = fileName + strlen(fileName);
						while ((ptr > fileName)&&(*ptr != '\\'))
						{
							ptr--;
						}
						*(ptr + 1) = '\0';
                        ***/

						// 抽出したディレクトリ名(ファイル名)を画面に反映させる
						SetDlgItemText(hWnd, IDC_2chBROWSERDIRECTORY, dirName);
					}
					else
					{
						//MessageBox(hWnd, "GetOpenFileName()", "Error", MB_OK);
					}
					break;

				case IDC_SELECTBROWSER:
					// 初期ディレクトリの反映...
					memset(fileName, 0, sizeof(fileName));
					memset(&ofn, 0, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					GetDlgItemText(hWnd, IDC_BROWSERDIRECTORY, fileName, sizeof(fileName));
					ofn.Flags = 0;
					//ofn.lpstrInitialDir = fileName;
					ofn.lpstrFile = fileName;
					ofn.nMaxFile = sizeof(fileName);

					// ファイル名選択ダイアログを表示する
					retVal = GetOpenFileName(&ofn);
					if (retVal != 0)
					{
                        /***
						// ディレクトリだけを抽出
						char *ptr = fileName + strlen(fileName);
						while ((ptr > fileName)&&(*ptr != '\\'))
						{
							ptr--;
						}
						*(ptr + 1) = '\0';
                        ***/

						// 抽出したディレクトリ名(ファイル名)を画面に反映させる
						SetDlgItemText(hWnd, IDC_BROWSERDIRECTORY, fileName);
					}
					else
					{
						//MessageBox(hWnd, "GetOpenFileName()", "Error", MB_OK);
					}
					break;

				default:
                    break;
            }
            break;

		case WM_HELP:
			if (GetModuleFileName((HINSTANCE)hAppInstance, szPath, sizeof(szPath)))
			{
				// Look for the end of the path
				int		fileOffset;
				fileOffset = strlen(szPath);
				for (; fileOffset > 0; fileOffset--)
				{
					if (szPath[fileOffset] == '\\')
						break;
				}

				if (fileOffset > 0)
				{
					szPath[fileOffset + 1] = 0;
					strcat(szPath, HELP_FILE);
					WinHelp(hWnd, szPath, HELP_CONTEXT, IDD_CONDUIT_ACTION + 0x20000 /*base resource*/);
				}
			}
			returnValue = TRUE;
			break;

		case WM_SYSCOLORCHANGE:
			LoadCfgDlgBitmaps(hWnd);
			break;

		default:
            break;
    }

	// カレントディレクトリを元のものに戻す
	SetCurrentDirectory(currentDirectory);
    return (returnValue);
}

/////////////////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////////
//
//	 Method:		LoadCfgDlgBitmaps
//
//	 Description:	This routines translates the bitmap gray background to the
//					button face color set in the control panel/window appearances
//
//	 Parameters:	HWND	- hDlg, handle of the dialog
//
//	 Returns:		none
//
/////////////////////////////////////////////////////////////////////////////
//	 REVISION HISTORY:
//		01/15/99    Carl Created
//
/////////////////////////////////////////////////////////////////////////////
void LoadCfgDlgBitmaps(HWND hDlg)
{
	COLORMAP		colorMap;
	HWND			hwndButton;
	HBITMAP			hBitmap, hOldBitmap;

	//
	// setup the bitmaps
	//
	colorMap.to = GetSysColor(COLOR_BTNFACE);
	colorMap.from = RGB(192,192,192);

	// Sync 
	hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_SYNC2CH, 0, &colorMap, 1);
	// associate the bitmap with the button.
	if ((hwndButton = GetDlgItem(hDlg, IDC_SYNC)) != NULL)
	{
		hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
		if (hOldBitmap != NULL)
			DeleteObject((HGDIOBJ)hOldBitmap);
	}

	// PC to HH 
	hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_PCTOHH, 0, &colorMap, 1);
	// associate the bitmap with the button.
	if ((hwndButton = GetDlgItem(hDlg, IDC_PCTOHH)) != NULL)
	{
		hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
		if (hOldBitmap != NULL)
			DeleteObject((HGDIOBJ)hOldBitmap);
	}

	// HH to PC 
	hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_HHTOPC, 0, &colorMap, 1);
	// associate the bitmap with the button.
	if ((hwndButton = GetDlgItem(hDlg, IDC_HHTOPC)) != NULL)
	{
		hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
		if (hOldBitmap != NULL)
			DeleteObject((HGDIOBJ)hOldBitmap);
	}

	// Do Nothing 
	hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_DONOTHING, 0, &colorMap, 1);
	// associate the bitmap with the button.
	if ((hwndButton = GetDlgItem(hDlg, IDC_DONOTHING)) != NULL)
	{
		hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
		if (hOldBitmap != NULL)
			DeleteObject((HGDIOBJ)hOldBitmap);
	}

	// Sync HH(favorite) from 2ch
	hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_2chTOHH_FAVOR, 0, &colorMap, 1);
	// associate the bitmap with the button.
	if ((hwndButton = GetDlgItem(hDlg, IDC_2CHTOHH_FAVOR)) != NULL)
	{
		hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
		if (hOldBitmap != NULL)
			DeleteObject((HGDIOBJ)hOldBitmap);
	}

	// Sync HH from 2ch
	hBitmap = CreateMappedBitmap((HINSTANCE)hLangInstance, IDB_2CHTOHH, 0, &colorMap, 1);
	// associate the bitmap with the button.
	if ((hwndButton = GetDlgItem(hDlg, IDC_2CHTOHH)) != NULL)
	{
		hOldBitmap = (HBITMAP)SendMessage(hwndButton, STM_SETIMAGE,(WPARAM)IMAGE_BITMAP, (LPARAM)(HANDLE)hBitmap);
		if (hOldBitmap != NULL)
			DeleteObject((HGDIOBJ)hOldBitmap);
	}

}
