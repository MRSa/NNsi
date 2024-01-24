/*****************************************************************************
 *
 * Generic Conduit - CNNsiMonaSync Src File
 *
 ****************************************************************************/
#define ASSERT(f)          ((void)0)
#define TRACE0(sz)
#define TRACE(sz)
#include <crtdbg.h>
//#include <atlbase.h>
//#include <atlhttp.h>
#include <windows.h>
#include <wininet.h>

#include <string.h>
#include <stdio.h>
#include <time.h>
#ifdef METROWERKS_WIN
#include <wmem.h>
#else
#include <memory.h>
#endif
#include <sys/stat.h>
#include <TCHAR.H>

#include <syncmgr.h>
#include "NNsiMonaSync.h"
#include "NNsiMonaArchive.h"
#include "NNsiMonaBrowserParser.h"

extern HANDLE hLangInstance;

#define MAX_PROD_ID_TEXT	255

/**
  コンストラクタ
**/
CNNsiMonaSync::CNNsiMonaSync(CSyncProperties& rProps, DWORD dwDatabaseFlags)
:  mDbPtr(0), mVfsFileRef(0), CSynchronizer(rProps, dwDatabaseFlags)
{
	mBBSmap.clear();
	memset(&mNNsiMonaPreferences, 0x00, sizeof(NNsiMonaPreferences));
	mNNsiMonaPreferences.waitTime = 800;
	mThreadNick.clear();
}

/**  
   デストラクタ

 **/
CNNsiMonaSync::~CNNsiMonaSync()
{
	if (mVfsFileRef != 0)
	{
		VFSFileClose(mVfsFileRef);
	}
	mThreadNick.clear();
}

/******************************************************************************
 *
 * Class:   CNNsiMonaSync
 *
 * Method:  SynchronizeAppInfo()
 *
 * Description: This method is used by slow/fast sync to synchronize the
 *              information contained in the AppInfo section on the
 *              Palm organizer.
 *
 * Parameter(s): None.
 *  
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CNNsiMonaSync::SynchronizeAppInfo(void)
{
	long                retval = 0;
    retval = m_dbHH->PreAppInfoSync();
    if (retval) {
        if (retval == GEN_ERR_CATEGORIES_NOT_SUPPORTED)
            return 0;
        return retval;
    }

    retval = m_dbPC->PreAppInfoSync();
    if (retval) {
        if (retval == GEN_ERR_CATEGORIES_NOT_SUPPORTED)
            return 0;
        return retval;
    }
    retval = SynchronizeCategories();
    if (retval)
        return retval;



    // store the changes back into the app info area on the HH
    retval = m_dbHH->PostAppInfoSync();
    if (retval) {
        return retval;
    }

    // store the changes back into the app info area on the PC
    retval = m_dbPC->PostAppInfoSync();
    if (retval) {
        return retval;
    }
    return retval;
}

long CNNsiMonaSync::CopyAppInfoHHtoPC( void )
{
	MessageBox(NULL, "CNNsiMonaSync::CopyAppInfoHHtoPC(void)", "Information", MB_OK);

	
	long retval = 0;

    retval = m_dbHH->PreAppInfoSync();
    if (retval) {
        return retval;
    }
    ASSERT(m_dbHH->GetCatMgr());
    ASSERT(m_dbPC->GetCatMgr());

    CPCategoryMgr *m_pHHCatMgr = m_dbHH->GetCatMgr();
    CPCategoryMgr *m_pPCCatMgr = m_dbPC->GetCatMgr();
    CPCategory *hhCat;

    // copy the categories
    hhCat = m_pHHCatMgr->FindFirst();
    while ((!retval) && (hhCat)) {
        retval = m_pPCCatMgr->Add(*hhCat);
        hhCat = m_pHHCatMgr->FindNext();
    }

	if (!retval) { //fix for categories not getting written to the Pc.
		retval = m_dbPC->PostAppInfoSync();
	}

    return retval;
}

long CNNsiMonaSync::CopyAppInfoPCtoHH( void )
{
	MessageBox(NULL, "CNNsiMonaSync::CopyAppInfoPCtoHH(void)", "Information", MB_OK);

	long retval = 0;

    retval = m_dbPC->PreAppInfoSync();
    if (retval) {
        return retval;
    }
    retval = m_dbHH->PreAppInfoSync();
    if (retval) {
        return retval;
    }
    ASSERT(m_dbHH->GetCatMgr());
    ASSERT(m_dbPC->GetCatMgr());

    CPCategoryMgr *m_pHHCatMgr = m_dbHH->GetCatMgr();
    CPCategoryMgr *m_pPCCatMgr = m_dbPC->GetCatMgr();
    CPCategory *pcCat;

    m_pHHCatMgr->Empty();

    // copy the categories
    pcCat = m_pPCCatMgr->FindFirst();
    while ((!retval) && (pcCat)) {
        retval = m_pHHCatMgr->Add(*pcCat);
        pcCat = m_pPCCatMgr->FindNext();
    }

	if (!retval) { //fix for categories not getting written to the hh.
		retval = m_dbHH->PostAppInfoSync();
	}

    return retval;

}


/******************************************************************************
 *
 * Class:   CNNsiMonaSync
 *
 * Method:  CreatePCManager()
 *
 * Description: This method creates the PC database manager. If it successfully creates
 *          the manager, it then opens the database.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CNNsiMonaSync::CreatePCManager(void)
{
    DeletePCManager();

    m_dbPC = new CNNsiMonaPCMgr(m_pLog, 
                        m_dwDatabaseFlags,
                        m_remoteDB->m_Name, 
                        m_rSyncProperties.m_LocalName, 
                        m_rSyncProperties.m_PathName,
                        m_rSyncProperties.m_SyncType);
    if (!m_dbPC)
        return GEN_ERR_LOW_MEMORY;
    return m_dbPC->Open();
}

/******************************************************************************
 *
 * Class:   CNNsiMonaSync
 *
 * Method:  CreateBackupManager()
 *
 * Description: This method creates the backup database manager. If it successfully creates
 *          the manager, it then opens the database.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CNNsiMonaSync::CreateBackupManager(void)
{
    DeleteBackupManager();

    m_dbBackup = new CNNsiMonaBackupMgr(m_pLog, 
                                m_dwDatabaseFlags,
                                m_remoteDB->m_Name, 
                                m_rSyncProperties.m_LocalName, 
                                m_rSyncProperties.m_PathName,
                                m_rSyncProperties.m_SyncType);
    if (!m_dbBackup)
        return GEN_ERR_LOW_MEMORY;
    return m_dbBackup->Open();
}

CPDbBaseMgr *CNNsiMonaSync::CreateArchiveManager(TCHAR *pFilename) 
{
    return (CPDbBaseMgr *)new CNNsiMonaArchive(m_pLog,
                                m_dwDatabaseFlags,
                                m_remoteDB->m_Name, 
                                pFilename, 
                                m_rSyncProperties.m_PathName);
}

/******************************************************************************
    HotSyncの実行!!
	   （ここをカスタマイズするならGeneric Conduitを使っている意味が
         実はあんまりなかったりもする、、、。）

 ******************************************************************************/
long CNNsiMonaSync::Perform(void)
{
    long retval = 0;
    long retval2 = 0;

	// 同期タイプのチェック...
    if (m_rSyncProperties.m_SyncType > eProfileInstall)
	{
        // 同期タイプの値が異常、、、終了する
		return (GEN_ERR_BAD_SYNC_TYPE);
	}

    if (m_rSyncProperties.m_SyncType == eDoNothing)
	{
        // 同期タイプが「何もしない」のときは終了する
		return (0);
    }

	// 母艦のインデックスデータを解析できるように準備する
	I2chBrowserParser *browserParser = new NNsiMonaBrowserParser;
	browserParser->setBrowserLocation(mNNsiMonaPreferences.browserFileLocation);
	browserParser->setBrowserConfigLocation(mNNsiMonaPreferences.browserConfigLocation);
	browserParser->prepare();
#if 0
	switch (mNNsiMonaPreferences.syncType)
	{
      case eDoNothing:
        // 何もしない
      case eProfileInstall:
        // NNsiのお気に入りを更新する
      case eInstall:
        // NNsiのログを更新する
        break;
	  case eSlow:
        // ログの更新とデータの同期
      case eFast:
        // データの同期
      case ePCtoHH:
        // PCのデータをPalmに反映
      case eHHtoPC:
        // PalmのデータをPCに反映
      default:
		break;
	}
#endif

    // システム情報の取得 (SyncReadSystemInfo())
	//  (PalmOSのバージョンとプロダクト情報を取得する)
    m_SystemInfo.m_ProductIdText = (BYTE*) new char [MAX_PROD_ID_TEXT];
    if (m_SystemInfo.m_ProductIdText == NULL)
	{
        // 領域の確保失敗...
		delete browserParser;
		return (GEN_ERR_LOW_MEMORY);
	}
    m_SystemInfo.m_AllocedLen = (BYTE) MAX_PROD_ID_TEXT; 
	retval = SyncReadSystemInfo(m_SystemInfo);
    if (retval != 0)
	{
		// システム情報の取得に失敗した、終了する
		delete browserParser;
		return (retval);
	}

	// SyncRegisterConduit() の呼び出し...
	//    (RegisterConduit()は基底クラスに実装済み)
	retval = RegisterConduit();
    if (retval != 0)
	{
		// 同期開始エラー...
		delete browserParser;
		return (retval);
	}
	LogAddEntry("", slSyncStarted, false);

    //////////////////////  同期処理ここから  //////////////////////

	// BBS一覧のアップロード (mapにデータを格納する)
	uploadBBSdatabase();

	// Palm本体のログのみ新着確認する場合...
	//if ((mNNsiMonaPreferences.syncType == eInstall)||(mNNsiMonaPreferences.syncType == eProfileInstall))

	// スレ一覧のデータ更新（新着確認）：引数は巡回待ち時間(ms)
    //     TODO: それぞれの処理によってクラス化する必要アリ！！ (リファクタリング箇所！）
	switch (mNNsiMonaPreferences.syncType)
	{
      case eHHtoPC:
		// Palmのログを母艦の２ちゃんブラウザにコピーする
		updateAndCopyHHtoPC(browserParser);
        break;

	  case ePCtoHH:
	  case eSlow:
      case eFast:
		// ログを母艦の２ちゃんブラウザからPalmにコピーする
		updateAndCopyPCtoHH(browserParser);
        break;

	  case eInstall:
      case eProfileInstall:
	  default:
        // Palmのログを更新 or Palmのお気に入りログを更新
		updateThreadInformation();
		break;
	}

	//////////////////////  同期処理ここまで  //////////////////////
	delete browserParser;

	// SyncUnRegisterConduit()の呼び出し...
	//    (UnregisterConduit()は基底クラスに実装済み)
	LogAddEntry("", slSyncFinished, false);
    retval2 = UnregisterConduit((BOOL)(retval != 0));
    if (retval == 0)
	{
		// 同期終了エラー...
		return (retval2);
	}
    return retval;
}

/*
 *    BBS一覧のアップロード...
 *
 */
void CNNsiMonaSync::uploadBBSdatabase(void)
{
	long ret;
	BYTE rHandle;
	BYTE openMode;
	openMode = eDbRead;

	// データベースのオープン
	ret = SyncOpenDB(DBNAME_BBSLIST, 0, rHandle, openMode);
	if (ret != 0)
	{
		return;
	}

	// BBS一覧のレコード数を取得する
	WORD recordCount = 0;
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// レコード件数の取得に失敗。。。終了する
		SyncCloseDB(rHandle);
		return;
	}

	// レコードをすべて読み出す
	for (WORD loop = 0; loop < recordCount; loop++)
	{
		// データ１件読み出し用の情報を作成
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = loop;

		BYTE recordBuffer[sizeof(NNshBoardDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// データ１件読み出し...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// データ(レコード)の読み出しに成功...
			NNshBoardDatabase *dbPtr;
			dbPtr = reinterpret_cast<NNshBoardDatabase *>(recordBuffer);

			// BBSのURLをmapに入れる...
			mBBSmap.insert(pair<string,string>(dbPtr->boardNick, dbPtr->boardURL));
        }
	}

	// データベースのクローズ
	SyncCloseDB(rHandle);

	return;
}

/*
 *    スレ一覧のデータ更新（新着確認）...
 *
 */
void CNNsiMonaSync::updateThreadInformation(void)
{
	long ret;
	BYTE rHandle;
	BYTE openMode;
	openMode = eDbRead | eDbWrite;

	// データベースのオープン
	ret = SyncOpenDB(DBNAME_SUBJECT, 0, rHandle, openMode);
	if (ret != 0)
	{
		return;
	}

	// スレ一覧のレコード数を取得する
	WORD recordCount = 0;
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// レコード件数の取得に失敗。。。終了する
		SyncCloseDB(rHandle);
		return;
	}

	// レコードをすべて読み出す
	for (WORD loop = recordCount; loop != 0; loop--)
	{
		// データ１件読み出し用の情報を作成
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = (loop - 1);

		BYTE recordBuffer[sizeof(NNshSubjectDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// データ１件読み出し...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// データ(レコード)の読み出しに成功...
			mDbPtr = reinterpret_cast<NNshSubjectDatabase *>(recordBuffer);

			// 読み込んだスレの情報をログ出力する
			if (checkGetThreadStatus(mDbPtr) == TRUE)
			{
				// 新着確認するスレだった場合...
                ret = getThreadLog(mDbPtr);
				if (ret == NNSH_NORMAL_END)
				{
					// データを書く...
					SyncWriteRec(recordInfo);

					///// 取得のためにdelayを入れる... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
				if (ret == NNSH_RECEIVE_RESPONSE_NG)
				{
					///// 取得のためにdelayを入れる... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
			}
		}
		mDbPtr = 0;
	}

	// データベースのクローズ
    SyncCloseDB(rHandle);
}

/*
      ログを取得(新着確認)する...

 */
long CNNsiMonaSync::getThreadLog(NNshSubjectDatabase *dbPtr)
{

	// 板のURLを取得する
	map<string,string>::iterator ite;
	ite = mBBSmap.find(dbPtr->boardNick);
	if (ite == mBBSmap.end())
	{
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "    BBS does not find... [%s]", dbPtr->boardNick);
		LogAddEntry(buffer, slWarning, false);
		return (NNSH_NOTFOUND_BBS);
	}

	// 板タイプが２ちゃんねる/まちBBS以外は新着確認しない... (とりあえず現状は...)
	if ((dbPtr->bbsType != NNSH_BBSTYPE_2ch)&&(dbPtr->bbsType != NNSH_BBSTYPE_MACHIBBS))
	{
		return (NNSH_BBSTYPE_NG);
	}

	// ログがVFSに格納されていない場合には、ログ取得しない...
    //  (取得予約スレの場合には、次に進む...)
	if ((dbPtr->msgState != FILEMGR_STATE_OPENED_VFS)&&
		((dbPtr->msgAttribute & NNSH_MSGATTR_GETRESERVE) == 0))
	{
		//MessageBox(NULL, "NNSH_LOGGINGPLACE_WRONG", "Information", MB_OK);
		return (NNSH_LOGGINGPLACE_WRONG);
	}

	// 同期タイプが「お気に入りだけ新着確認」だった場合...
	if (mNNsiMonaPreferences.syncType == eProfileInstall)
	{
		// お気に入りスレだけ新着確認を実施する...
		if ((dbPtr->msgAttribute & ((NNSH_MSGATTR_FAVOR)|(NNSH_MSGATTR_FAVOR_L4)|(NNSH_MSGATTR_FAVOR_L3)|(NNSH_MSGATTR_FAVOR_L2)|(NNSH_MSGATTR_FAVOR_L1))) == 0)
		{
			return (NNSH_LOGGINGTYPE_NOTMATCHED);
		}
	}

    if (checkVFSVolumes(dbPtr->boardNick, dbPtr->threadFileName) == FALSE)
	{
		// VFSのデータがうまくみつからなかった
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "    Valid VFS Volume is nothing... [%s]", dbPtr->threadTitle);
		LogAddEntry(buffer, slText, false);
		return (NNSH_LOGGINGPLACE_WRONG_VFS);
	}

	// 取得するURLを生成する... (今は２ちゃんねるのみ...)
	char url[256], *ptr;
	DWORD getBytes = 0;
    memset(url, 0x00, sizeof(url));
	switch (dbPtr->bbsType)
	{
      case NNSH_BBSTYPE_MACHIBBS:
        // まちBBSの取得URLを生成する
		strcpy(url, ite->second.c_str());
		strcat(url, "bbs/read.pl?BBS=");
        strcat(url, dbPtr->boardNick);
        // boardNickの末尾にある '/'を削除
        if (url[strlen(url) - 1] == '/')
        {
            url[strlen(url) - 1] = '\0';
        } 
        strcat(url, "&KEY=");
        strcat(url, dbPtr->threadFileName);
        ptr = &url[strlen(url) - 1];
        // threadFileNameの拡張子を削除する
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '\0';

		// まちBBSは差分取得しない...
		getBytes = 0;
        break;

	  case NNSH_BBSTYPE_2ch:
	  default:
        // 2ちゃんねるの取得URLを生成する
		sprintf(url, "%sdat/%s", ite->second.c_str(), dbPtr->threadFileName);

	    // 取得するスレの先頭バイトを確認する
	    if (dbPtr->fileSize != 0)
	    {
		    getBytes = SyncHHToHostDWord(dbPtr->fileSize) - 1;
	    }
		break;
	}

    // HTTP(差分取得)を実施する
	long returnValue = httpCommunication(url, getBytes);
	if (returnValue == NNSH_RECEIVE_RESPONSE_NG)
	{
		// 新着確認するスレだった場合、(とりあえず)ログ出力する...
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "        %s (%s)", dbPtr->threadTitle, url);
		LogAddEntry(buffer, slText, false);
	}

	return (returnValue);
}

/*
    http通信を実施 (WinInetを使用する)
 */
long CNNsiMonaSync::httpCommunication(char *url, int range)
{
	HINTERNET hSession = 0;
	hSession = InternetOpen("Monazilla/1.00  (NNsiMona 1.0)", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hSession == 0)
	{
        // internetのオープンに失敗...終了する
		LogAddEntry("   InternetOpen()", slWarning, false);
		return (NNSH_RECIEVE_ERROR);
	}

	// http要求ヘッダの追加
	char httpHeader[4096 + 1];
	memset(httpHeader, 0x00, sizeof(httpHeader));
	if (range == 0)
	{
		strcpy(httpHeader, "ACCEPT: text/html, */*\r\nACCEPT-ENCODING: identity\r\nConnection: close\r\nACCEPT-LANGUAGE: ja, en\r\nPragma: no-cache\r\n\r\n");
	}
	else
	{
		sprintf(httpHeader, "RANGE: bytes=%d-\r\nACCEPT: text/html, */*\r\nACCEPT-ENCODING: identity\r\nConnection: close\r\nACCEPT-LANGUAGE: ja, en\r\nPragma: no-cache\r\n\r\n", range);
	}

	// URLからデータを取得する...
	HINTERNET hService = 0;
	hService = InternetOpenUrl(hSession, url, httpHeader, strlen(httpHeader), INTERNET_FLAG_NO_AUTO_REDIRECT, 0);
	if (hService == 0)
	{
		// 接続に失敗、、、終了する
		InternetCloseHandle(hSession);
		LogAddEntry("   InternetOpenUrl()", slWarning, false);
		return (NNSH_RECIEVE_ERROR);
	}

	// HTTP応答ヘッダを確認する...
	DWORD headerBufferLength;
	memset(httpHeader, 0x00, sizeof(httpHeader));
	headerBufferLength = sizeof(httpHeader) - 1;
	if (HttpQueryInfo(hService, HTTP_QUERY_RAW_HEADERS_CRLF, httpHeader, &headerBufferLength, NULL) != TRUE)
	{
		// 応答ヘッダが取得できない、、、ありえないはずだが...終了する
		InternetCloseHandle(hService);
		InternetCloseHandle(hSession);
		LogAddEntry("   HttpQueryInfo()", slWarning, false);
		return (NNSH_RECIEVE_ERROR);
	}

	// 受信データのデータバイト数を取得する...
	char *ptr;
	ptr = strstr(httpHeader, "Content-Length");
	if (ptr == NULL)
	{
		ptr = strstr(httpHeader, "CONTENT-LENGTH");
	}
	long contentLength = getContentLength(ptr);

	// 応答コードのチェック
	long returnValue;
	if ((httpHeader[9] == '2')&&(httpHeader[10] == '0')&&(httpHeader[11] == '0'))
    {
	    // 全メッセージを受信した...
		returnValue = receivedWholeMessage(hService, contentLength);
	}
    else if ((httpHeader[9] == '2')&&(httpHeader[10] == '0')&&(httpHeader[11] == '6'))
    {

		// 差分取得に成功した...
		returnValue = receivedPartMessage(hService, contentLength);
	}
	else
	{
        // 応答コード異常...
		char message[80];
		sprintf(message, "    WRONG HTTP RESPONSE(HTTP/1.1 %c%c%c)", 
			    httpHeader[9], httpHeader[10], httpHeader[11]);
		LogAddEntry(message, slWarning, false);
	    returnValue = NNSH_RECEIVE_RESPONSE_NG;
	}

	// HTTPのセッションをクローズする...
	InternetCloseHandle(hService);
	InternetCloseHandle(hSession);

	return (returnValue);
}

/*
    全部取得を実行...
 */
long CNNsiMonaSync::receivedWholeMessage(HINTERNET hService, long contentLength)
{
	if ((mVfsFileRef == 0)||(mDbPtr == NULL))
	{
		// VFSの書き込み準備ができていない...終了する
		return (NNSH_RECIEVE_ERROR);
	}

	// ファイルを先頭から書き込むようにする...
	VFSFileSeek(mVfsFileRef, fsOriginBeginning, 0);

	// ファイルサイズの記憶...
	DWORD originalFileSize = SyncHHToHostDWord(mDbPtr->fileSize);

	// 受信データを格納するバッファを用意する...
    char receiveBuffer[8192 + 8];
	DWORD totalReadBytes = 0;
	while (1)
	{
		// 取得バッファをクリアする
		memset(receiveBuffer, 0, sizeof(receiveBuffer));
	
		// 受信データを読み出す、、、
		DWORD dwBytesRead = 8192;
	    BOOL ret = InternetReadFile(hService, receiveBuffer, 8192, &dwBytesRead);
		if ((ret == FALSE)||(dwBytesRead == 0))
		{
			// データ取得終了...ループを抜ける
			break;
        }
		totalReadBytes = totalReadBytes + dwBytesRead;

		// VFSファイルにログをappendする
		UINT32 outputDataSize = 0;
		if ((dwBytesRead % 2) == 0)
		{
			// 末尾をNULLターミネート
			receiveBuffer[dwBytesRead]     = '\0';
		
			// 補正なしVFSデータ出力
			VFSFileWrite(mVfsFileRef, dwBytesRead, receiveBuffer, &outputDataSize);
		}
		else
		{
			// CLIE用補正データを作成...
			receiveBuffer[dwBytesRead]     = ' ';
			receiveBuffer[dwBytesRead + 1] = '\0';

			// 補正ありVFSデータ(出力データを偶数バイトに補正する...)
			VFSFileWrite(mVfsFileRef, (dwBytesRead + 1), receiveBuffer, &outputDataSize);
		}
	}

#if 0
	// 出力ファイルサイズをログ出力する
	char buffer[400];
	memset(buffer, 0x00, sizeof(buffer));
	sprintf(buffer, "   Wrote %d bytes. (%s)", totalReadBytes, mDbPtr->threadTitle);
	LogAddEntry(buffer, slText, false);
#endif

	// ファイルのクローズ...
	VFSFileClose(mVfsFileRef);
	mVfsFileRef = 0;

	// ファイルサイズの加算...
	mDbPtr->fileSize = SyncHostToHHDWord(totalReadBytes);

	// 受信データを「新規取得」に変更する...
	if (originalFileSize != totalReadBytes)
	{
		// 新規取得にするのは、ファイルサイズが異なっているときのみ...
		mDbPtr->state = NNSH_SUBJSTATUS_NEW;
	}
	mDbPtr->msgState = FILEMGR_STATE_OPENED_VFS;
	mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&((NNSH_MSGATTR_NOTERROR)&(NNSH_MSGATTR_NOTRESERVE)));

	return (NNSH_NORMAL_END);
}

/*
    差分取得を実行...
 */
long CNNsiMonaSync::receivedPartMessage(HINTERNET hService, long contentLength)
{
	// 受信データを読み出す、、、
	char *receiveBuffer;

	// 受信データを格納するバッファを用意する...
	receiveBuffer = static_cast<char *>(malloc(contentLength + 100 + 1));
	if (receiveBuffer == NULL)
	{
		return (NNSH_MEMORY_ALLOCATION_FAILURE);
	}
	memset(receiveBuffer, 0x00, (contentLength + 100 + 1));

	// 受信データをバッファに格納する...
	DWORD dwBytesRead = contentLength + 100;
    InternetReadFile(hService, receiveBuffer, (contentLength + 100), &dwBytesRead);

	// スレ情報の変更があるかどうかチェックする
	if ((dwBytesRead == 1)&&(receiveBuffer[0] == '\x0a'))
	{
		// スレ変更なし（新着レスなし）
		free(receiveBuffer);

		// VFSファイルをオープンしている場合にはクローズ
		if (mVfsFileRef != 0)
		{
			VFSFileClose(mVfsFileRef);
			mVfsFileRef = 0;
		}

		// エラーフラグと取得保留フラグを落とす...
		mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&((NNSH_MSGATTR_NOTERROR)&(NNSH_MSGATTR_NOTRESERVE)));
		return (NNSH_NORMAL_END);
	}
	receiveBuffer[dwBytesRead] = '\0';

	// 受信データの異常をチェックする...
	if (receiveBuffer[0] != '\x0a')
	{
		// 受信データが異常、、、終了する
		return (NNSH_RECEIVE_DATA_WRONG);
	}
	receiveBuffer[0] = ' ';  // CLIE補正用に準備...

	if ((mVfsFileRef != 0)&&(mDbPtr != NULL))
	{
		// VFSファイルにログをappendする
		UINT32 outputDataSize = 0;
		VFSFileSeek(mVfsFileRef, fsOriginEnd, 0);
		if ((dwBytesRead % 2) == 0)
		{
			// 補正付きVFSデータ出力
			VFSFileWrite(mVfsFileRef, dwBytesRead, receiveBuffer, &outputDataSize);
		}
		else
		{
			// 補正なしVFSデータ出力 (1バイト目には '\x0a'、前レスの区切りが入っている)
			VFSFileWrite(mVfsFileRef, (dwBytesRead - 1), &receiveBuffer[1], &outputDataSize);
		}

		// ファイルのクローズ...
		VFSFileClose(mVfsFileRef);
		mVfsFileRef = 0;

		// ファイルサイズの加算...
		DWORD fileSize = SyncHHToHostDWord(mDbPtr->fileSize);
		fileSize = fileSize + contentLength - 1;
		mDbPtr->fileSize = SyncHostToHHDWord(fileSize);

		// 受信データを「更新済み」に変更する...
		mDbPtr->state = NNSH_SUBJSTATUS_UPDATE;
		mDbPtr->msgState = FILEMGR_STATE_OPENED_VFS;
	    mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&((NNSH_MSGATTR_NOTERROR)&(NNSH_MSGATTR_NOTRESERVE)));
	}

	// 確保したバッファをクリアする...
	free(receiveBuffer);

	return (NNSH_NORMAL_END);
}

/**
    Content-Length に記述された数値を取得する
 **/
long CNNsiMonaSync::getContentLength(char *str)
{
	if (str == NULL)
	{
		return (0);
	}

	long length = 0;
	while ((*str < '0')||(*str > '9'))
	{
		if (*str < ' ')
		{
			break;
		}
		str++;
	}
	while ((*str >= '0')&&(*str <= '9'))
	{
		length = length * 10 + (*str - '0');
		str++;
	}
	return (length);
}

/***
   VFSのボリュームをチェックする...
   (オープンしてそのハンドルをmVfsFileRef に入れて応答する
 ***/
BOOL CNNsiMonaSync::checkVFSVolumes(char *boardNick, char *datFileName)
{
	// ファイルがオープンしている状態なら、一度ファイルを閉じる
	if (mVfsFileRef != 0)
	{
		VFSFileClose(mVfsFileRef);
	}
	mVfsFileRef = 0;

	WORD  nofVolume = 0;
	WORD *volRefNumList;
	long ret = VFSVolumeEnumerate(&nofVolume, NULL);
	if ((ret != 0)||(nofVolume == 0))
	{
		// VFSがない、、、終了する
		return (FALSE);
	}

	// VFSボリュームの格納領域を確保する
	volRefNumList = new WORD[nofVolume];
	if (volRefNumList == NULL)
	{
		// 領域確保に失敗...
		return (FALSE);
	}
	ret = VFSVolumeEnumerate(&nofVolume, volRefNumList);
	if ((ret != 0)||(nofVolume == 0))
	{
		// VFSがない、、、終了する (ただありえないはず。。。)
		return (FALSE);
	}

	// ファイル名を生成する...
	char openFileName[256];
	memset(openFileName, 0x00, sizeof(openFileName));
	sprintf(openFileName, "/Palm/Programs/NNsh/%s%s", boardNick, datFileName);
	// VFSのボリュームを確認する
	BOOL retValue = FALSE;
	for (int loop = 0; loop < nofVolume; loop++)
	{
		VolumeInfoType  volInfo;
		memset(&volInfo, 0x00, sizeof(volInfo));
		ret = VFSVolumeInfo(volRefNumList[loop], &volInfo);
		if (ret != 0)
		{
			// VFSボリューム情報がとれなかった、、、次のボリュームへ...
			continue;
		}
		if (volInfo.fsType != fsFilesystemType_VFAT)
		{
			// VFSボリュームがVFATではなかった、、、次のボリュームへ...
			continue;
		}

		// 目的のファイルがオープンできるか実験する...
		FileRef fileRef = 0;
		ret = VFSFileOpen(volRefNumList[loop], openFileName, vfsModeReadWrite, &fileRef);
		if (ret == 0)
		{
			// datファイルのOPENに成功した場合...
			mVfsFileRef = fileRef;
			retValue = TRUE;

			// 取得予約フラグを落とす...
			mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&(NNSH_MSGATTR_NOTRESERVE));

			break;
		}
		//if (ret != 0)
		{
			// 目的のファイルがオープンできなかった場合、、、
			if ((mDbPtr->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
			{
				// 取得予約スレの場合には、ファイルを作成する
				(void) VFSFileOpen(volRefNumList[loop], openFileName, (vfsModeCreate), &fileRef);
				VFSFileClose(fileRef);
				ret = VFSFileOpen(volRefNumList[loop], openFileName, vfsModeReadWrite, &fileRef);
				if (ret != 0)
				{
					continue;
				}
				// datファイルのOPENに成功した場合...
				mVfsFileRef = fileRef;
				retValue = TRUE;

				// 取得予約フラグを落とす...
			    mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&(NNSH_MSGATTR_NOTRESERVE));

				break;
			}
			else
			{
				continue;
			}
		}
	}
	delete [] volRefNumList;
	return (retValue);
}

/*
	NNsiMonaの設定情報を取得する

 */
void CNNsiMonaSync::setPreferences(NNsiMonaPreferences *dataSource)
{
	memcpy(&mNNsiMonaPreferences, dataSource, sizeof(NNsiMonaPreferences));
}

/*
    スレ取得状態を確認し、取得するスレだった場合にはtrueを返す

 */
BOOL CNNsiMonaSync::checkGetThreadStatus(NNshSubjectDatabase *dbPtr)
{
	// 参照専用ログだった場合には、何もしない
	if (dbPtr->boardNick[0] == '!')
	{
        return (FALSE);
	}

	// 取得予約スレだった場合には更新する
	if ((dbPtr->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
	{
        return (TRUE);
	}


	WORD mesNum = 0;
    switch (dbPtr->state)
	{
      case NNSH_SUBJSTATUS_NEW:
	  case NNSH_SUBJSTATUS_UPDATE:
	  case NNSH_SUBJSTATUS_REMAIN:
      case NNSH_SUBJSTATUS_ALREADY:
		// 取得する状態...

		// レス数の最大を検出する...  
		mesNum = SyncHHToHostWord(dbPtr->maxLoc);
		if (mesNum > 1000)
		{
			// スレの最大を超えた、取得しない。
			char buffer[400];
            memset(buffer, 0x00, sizeof(buffer));
			sprintf(buffer, "   Res: %d  [%s%s]", mesNum, dbPtr->threadTitle, dbPtr->boardNick, dbPtr->threadFileName);
            LogAddEntry(buffer, slText, false);
			return (FALSE);
		}
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   MATCH %s  [%s%s]", dbPtr->threadTitle, dbPtr->boardNick, dbPtr->threadFileName);
		LogAddEntry(buffer, slText, false);
#endif
		return (TRUE);
		break;

	  default:
        // 取得しない状態...
		break;
	}
	return (FALSE);
}


/*
 *    新着確認を実施し、かつPalmのスレ情報を母艦の２ちゃんブラウザにコピーする
 *     → コピーする内容：ログファイル、未読ポイント、取得ログファイルサイズ...
 */
void CNNsiMonaSync::updateAndCopyHHtoPC(I2chBrowserParser *apParser)
{
	long ret;
	BYTE rHandle;
	BYTE openMode;
	openMode = eDbRead | eDbWrite;

	// データベースのオープン
	ret = SyncOpenDB(DBNAME_SUBJECT, 0, rHandle, openMode);
	if (ret != 0)
	{
		return;
	}

	// スレ一覧のレコード数を取得する
	WORD recordCount = 0;
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// レコード件数の取得に失敗。。。終了する
		SyncCloseDB(rHandle);
		return;
	}

	char logDataSeparator[32];
	memset(logDataSeparator, 0x00, sizeof(logDataSeparator));
	apParser->getSeparatorData(logDataSeparator);

	// レコードをすべて読み出す
	for (WORD loop = recordCount; loop != 0; loop--)
	{
		// データ１件読み出し用の情報を作成
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = (loop - 1);

		BYTE recordBuffer[sizeof(NNshSubjectDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// データ１件読み出し...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// データ(レコード)の読み出しに成功...
			mDbPtr = reinterpret_cast<NNshSubjectDatabase *>(recordBuffer);

			// 読み込んだスレの情報をログ出力する
			if (checkGetThreadStatus(mDbPtr) == TRUE)
			{
				// 新着確認するスレだった場合...
                ret = getThreadLog(mDbPtr);
				if (ret == NNSH_NORMAL_END)
				{
					// データを書く...
					SyncWriteRec(recordInfo);

					///// 取得のためにdelayを入れる... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
				if (ret == NNSH_RECEIVE_RESPONSE_NG)
				{
					///// 取得のためにdelayを入れる... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
			}
			else
			{
				// 新着確認しないスレだった場合...
				//  (ログがVFSにあれば、そのログをPCのログに上書きする)
			}

			// インデックスレコードを取得する...
			std::string boardNick = mDbPtr->boardNick;
			std::string fileName  = mDbPtr->threadFileName;
			threadIndexRecord *indexRecord;

			long  resNumArray[65536];
			memset(resNumArray, 0x00, sizeof(resNumArray));

			// 現在時刻を取得する
			time_t unixTime = 0;
			time(&unixTime);
			//localtime(&unixTime);
			DWORD dataFileSize = SyncHHToHostDWord(mDbPtr->fileSize);
			DWORD currentUnixTime = unixTime;
			currentUnixTime = currentUnixTime + (9 * 60 * 60);  // 9時間時間を進める...

			if ((mDbPtr->bbsType == NNSH_BBSTYPE_2ch)&&(mDbPtr->msgState == FILEMGR_STATE_OPENED_VFS))
			{
				// ここでログを更新する...
				if (checkVFSVolumes(mDbPtr->boardNick, mDbPtr->threadFileName) == TRUE)
				{
					// ログをPCにコピーする...
					UINT32 fileSize, readSize;
					if (VFSFileSize(mVfsFileRef, &fileSize) == 0)
					{
						char *buf = new char[fileSize + 100];
						if (buf != NULL)
						{
							memset(buf, 0x00, (fileSize + 100));
							if (VFSFileRead(mVfsFileRef, fileSize, buf, &readSize) == 0)
							{
								// ベースパスの設定...
								std::string basePath;
								apParser->getBasePath(basePath);
								int resNum = 0;

								// ログが取得済みだった場合...
								if (apParser->getIndexRecord(boardNick, fileName, indexRecord) == true)
								{
									// ここでログファイルをコピーする...
									resNum = copyLogDatFile(basePath, boardNick, logDataSeparator, indexRecord, buf, readSize, resNumArray, 65536);

									// レス番号等を更新する...
									DWORD locNum = SyncHHToHostWord(mDbPtr->currentLoc);
									DWORD maxNum = resNum;
									DWORD newNum = maxNum - locNum;
									if (maxNum < locNum)
									{
										newNum = 0;
									}
									DWORD newResNum = SyncHHToHostWord(mDbPtr->maxLoc) + 1;
									if (maxNum < newResNum)
									{
										newResNum = maxNum;
									}

									// 未読位置を設定...
									DWORD curResNum = 0;
									indexRecord->getCurrentResNumber(curResNum);
									if (curResNum < locNum)
									{
										indexRecord->setCurrentResNumber(locNum);
									}
									indexRecord->setAllResCount(maxNum);  // 全レス数 (サーバ)
									indexRecord->setResCount(maxNum);     // 全レス数 (ローカル)
									indexRecord->setFileSize(dataFileSize); // ファイルサイズ
									indexRecord->setNewReceivedRes(newResNum);  // ここから新規取得
									indexRecord->setNewResCount(newNum);        // 新着数
									indexRecord->setRoundDate(currentUnixTime);  // 取得時刻を記入する
									//indexRecord->setScrollTop(resNumArray[locNum]);    // "ここまで読んだ"番号
									indexRecord->setReadResNumber(locNum);          // "ここまで読んだ"番号
									//indexRecord->setCurrentNumber(0);           // 現在の板でのインデックス位置
									indexRecord->setCurrentResNumber(locNum);
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   %s (resNum:%d [%d])", mDbPtr->threadTitle, resNumArray[locNum], locNum);
		LogAddEntry(buffer, slText, false);
#endif
								}
								else
								{
									// スレタイトルを追加してログファイルをコピーする
									threadIndexRecord *indexRecord = new threadIndexRecord;

									indexRecord->getFileName(fileName);
									std::string threadTitle = mDbPtr->threadTitle;
									indexRecord->setThreadTitle(threadTitle);
									apParser->entryIndexRecord(boardNick, indexRecord);
									resNum = copyLogDatFile(basePath, boardNick, logDataSeparator, indexRecord, buf, readSize, resNumArray, 65536);

									// レス番号等を更新する...
									DWORD locNum = SyncHHToHostWord(mDbPtr->currentLoc);
									DWORD maxNum = resNum;
									DWORD newNum = maxNum - locNum;
									if (maxNum < locNum)
									{
										newNum = 0;
									}
									DWORD newResNum = SyncHHToHostWord(mDbPtr->maxLoc) + 1;
									if (maxNum < newResNum)
									{
										newResNum = maxNum;
									}
									indexRecord->setCurrentResNumber(locNum);
									indexRecord->setAllResCount(maxNum);  // 全レス数 (サーバ)
									indexRecord->setResCount(maxNum);     // 全レス数 (ローカル)
									indexRecord->setFileSize(dataFileSize); // ファイルサイズ
									indexRecord->setNewReceivedRes(newResNum);  // ここから新規取得
									indexRecord->setNewResCount(newNum);        // 新着数
									indexRecord->setRoundDate(currentUnixTime); // 取得時刻を記入する
									//indexRecord->setScrollTop(resNumArray[locNum]);    // スクロールの先頭
									indexRecord->setReadResNumber(locNum);    // "ここまで読んだ"番号
									//indexRecord->setCurrentNumber(0);         // 現在の板でのインデックス番号
									delete indexRecord;
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   %s (resNum:%d [%d])", mDbPtr->threadTitle, resNumArray[locNum], locNum);
		LogAddEntry(buffer, slText, false);
#endif
								}
								delete [] buf;
							}
						}
						VFSFileClose(mVfsFileRef);
					}
				}

			}
		}
		mDbPtr = 0;
	}
	apParser->updateIndexRecords();

	// データベースのクローズ
    SyncCloseDB(rHandle);
}


/*
 *    新着確認を実施し、かつPalmのスレ情報を母艦の２ちゃんブラウザにコピーする
 *     → コピーする内容：ログファイル、未読ポイント、取得ログファイルサイズ...
 */
void CNNsiMonaSync::updateAndCopyPCtoHH(I2chBrowserParser *apParser)
{
	long ret;
	BYTE rHandle;
	BYTE openMode;
	openMode = eDbRead | eDbWrite;

	// データベースのオープン
	ret = SyncOpenDB(DBNAME_SUBJECT, 0, rHandle, openMode);
	if (ret != 0)
	{
		return;
	}

	// スレ一覧のレコード数を取得する
	WORD recordCount = 0;
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// レコード件数の取得に失敗。。。終了する
		SyncCloseDB(rHandle);
		return;
	}

	// スレ情報を読み出す前に初期化する...
	mThreadNick.clear();

	// レコードをすべて読み出す
	for (WORD loop = recordCount; loop != 0; loop--)
	{
		// データ１件読み出し用の情報を作成
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = (loop - 1);

		BYTE recordBuffer[sizeof(NNshSubjectDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// データ１件読み出し...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// データ(レコード)の読み出しに成功...
			mDbPtr = reinterpret_cast<NNshSubjectDatabase *>(recordBuffer);

			// 2chタイプのスレで取得済みのものだけ、、、
			if ((mDbPtr->bbsType == NNSH_BBSTYPE_2ch)&&(mDbPtr->msgState == FILEMGR_STATE_OPENED_VFS))
			{
				// スレ情報を格納する
				std::string boardNick  = mDbPtr->boardNick;
				std::string datLogFile = mDbPtr->threadFileName;
				mThreadNick.insert(pair<std::string,std::string>(datLogFile, boardNick));
			}
		}
		mDbPtr = 0;
	}

	/////////// レコードが追加されているかどうかチェック  //////////
	std::list<ThreadInfo> appendRecordList;
	appendRecordList.clear();
	ret = apParser->getThreadRecordToAppend(mThreadNick, appendRecordList);
	if (ret != 0)
	{
		NNshSubjectDatabase subjDB;
		std::list<ThreadInfo>::iterator it;
		for (it = appendRecordList.begin(); it != appendRecordList.end(); it++)
		{
			memset(&subjDB, 0x00, sizeof(NNshSubjectDatabase));

			// データベースにレコードを追加する
			// apParser->getAppendThreadList(mThreadNick, (ret + 4), subjDBList);
		}
	}
	///////////////////////////////////

	char logDataSeparator[32];
	memset(logDataSeparator, 0x00, sizeof(logDataSeparator));
	apParser->getSeparatorData(logDataSeparator);

	// 改めてレコード件数を取得する...
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// レコード件数の取得に失敗。。。終了する
		SyncCloseDB(rHandle);
		return;
	}

	// レコードをすべて読み出す
	for (WORD loop = recordCount; loop != 0; loop--)
	{
		// データ１件読み出し用の情報を作成
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = (loop - 1);

		BYTE recordBuffer[sizeof(NNshSubjectDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// データ１件読み出し...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// データ(レコード)の読み出しに成功...
			mDbPtr = reinterpret_cast<NNshSubjectDatabase *>(recordBuffer);

			// 読み込んだスレの情報をログ出力する
			if (checkGetThreadStatus(mDbPtr) == TRUE)
			{
				// 新着確認するスレだった場合...
                ret = getThreadLog(mDbPtr);
				if (ret == NNSH_NORMAL_END)
				{
					// データを書く...
					SyncWriteRec(recordInfo);

					///// 取得のためにdelayを入れる... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
				if (ret == NNSH_RECEIVE_RESPONSE_NG)
				{
					///// 取得のためにdelayを入れる... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
			}
			else
			{
				// 新着確認しないスレだった場合...
				//  (ログがVFSにあれば、そのログをPCのログに上書きする)
			}

			// インデックスレコードを取得する...
			std::string boardNick = mDbPtr->boardNick;
			std::string fileName  = mDbPtr->threadFileName;
			threadIndexRecord *indexRecord;

			long  resNumArray[65536];
			memset(resNumArray, 0x00, sizeof(resNumArray));

			// 現在時刻を取得する
			time_t unixTime = 0;
			time(&unixTime);
			//localtime(&unixTime);
			DWORD dataFileSize = SyncHHToHostDWord(mDbPtr->fileSize);
			DWORD currentUnixTime = unixTime;
			currentUnixTime = currentUnixTime + (9 * 60 * 60);  // 9時間時間を進める...

			if ((mDbPtr->bbsType == NNSH_BBSTYPE_2ch)&&(mDbPtr->msgState == FILEMGR_STATE_OPENED_VFS))
			{
				// ここでログを更新する...
				if (checkVFSVolumes(mDbPtr->boardNick, mDbPtr->threadFileName) == TRUE)
				{
					// ログをPCにコピーする...
					UINT32 fileSize, readSize;
					if (VFSFileSize(mVfsFileRef, &fileSize) == 0)
					{
						char *buf = new char[fileSize + 100];
						if (buf != NULL)
						{
							memset(buf, 0x00, (fileSize + 100));
							if (VFSFileRead(mVfsFileRef, fileSize, buf, &readSize) == 0)
							{
								// ベースパスの設定...
								std::string basePath;
								apParser->getBasePath(basePath);
								int resNum = 0;

								// ログが取得済みだった場合...
								if (apParser->getIndexRecord(boardNick, fileName, indexRecord) == true)
								{
									// ここでログファイルをコピーする...
									resNum = copyLogDatFile(basePath, boardNick, logDataSeparator, indexRecord, buf, readSize, resNumArray, 65536);

									// レス番号等を更新する...
									DWORD locNum = SyncHHToHostWord(mDbPtr->currentLoc);
									DWORD maxNum = resNum;
									DWORD newNum = maxNum - locNum;
									if (maxNum < locNum)
									{
										newNum = 0;
									}
									DWORD newResNum = SyncHHToHostWord(mDbPtr->maxLoc) + 1;
									if (maxNum < newResNum)
									{
										newResNum = maxNum;
									}

									// 未読位置を設定...
									DWORD curResNum = 0;
									indexRecord->getCurrentResNumber(curResNum);
									if (curResNum < locNum)
									{
										indexRecord->setCurrentResNumber(locNum);
									}
									indexRecord->setAllResCount(maxNum);  // 全レス数 (サーバ)
									indexRecord->setResCount(maxNum);     // 全レス数 (ローカル)
									indexRecord->setFileSize(dataFileSize); // ファイルサイズ
									indexRecord->setNewReceivedRes(newResNum);  // ここから新規取得
									indexRecord->setNewResCount(newNum);        // 新着数
									indexRecord->setRoundDate(currentUnixTime);  // 取得時刻を記入する
									//indexRecord->setScrollTop(resNumArray[locNum]);    // "ここまで読んだ"番号
									indexRecord->setReadResNumber(locNum);          // "ここまで読んだ"番号
									//indexRecord->setCurrentNumber(0);           // 現在の板でのインデックス位置
									indexRecord->setCurrentResNumber(locNum);
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   %s (resNum:%d [%d])", mDbPtr->threadTitle, resNumArray[locNum], locNum);
		LogAddEntry(buffer, slText, false);
#endif
								}
								else
								{
									// スレタイトルを追加してログファイルをコピーする
									threadIndexRecord *indexRecord = new threadIndexRecord;

									indexRecord->getFileName(fileName);
									std::string threadTitle = mDbPtr->threadTitle;
									indexRecord->setThreadTitle(threadTitle);
									apParser->entryIndexRecord(boardNick, indexRecord);
									resNum = copyLogDatFile(basePath, boardNick, logDataSeparator, indexRecord, buf, readSize, resNumArray, 65536);

									// レス番号等を更新する...
									DWORD locNum = SyncHHToHostWord(mDbPtr->currentLoc);
									DWORD maxNum = resNum;
									DWORD newNum = maxNum - locNum;
									if (maxNum < locNum)
									{
										newNum = 0;
									}
									DWORD newResNum = SyncHHToHostWord(mDbPtr->maxLoc) + 1;
									if (maxNum < newResNum)
									{
										newResNum = maxNum;
									}
									indexRecord->setCurrentResNumber(locNum);
									indexRecord->setAllResCount(maxNum);  // 全レス数 (サーバ)
									indexRecord->setResCount(maxNum);     // 全レス数 (ローカル)
									indexRecord->setFileSize(dataFileSize); // ファイルサイズ
									indexRecord->setNewReceivedRes(newResNum);  // ここから新規取得
									indexRecord->setNewResCount(newNum);        // 新着数
									indexRecord->setRoundDate(currentUnixTime); // 取得時刻を記入する
									//indexRecord->setScrollTop(resNumArray[locNum]);    // スクロールの先頭
									indexRecord->setReadResNumber(locNum);    // "ここまで読んだ"番号
									//indexRecord->setCurrentNumber(0);         // 現在の板でのインデックス番号
									delete indexRecord;
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   %s (resNum:%d [%d])", mDbPtr->threadTitle, resNumArray[locNum], locNum);
		LogAddEntry(buffer, slText, false);
#endif
								}
								delete [] buf;
							}
						}
						VFSFileClose(mVfsFileRef);
					}
				}

			}
		}
		mDbPtr = 0;
	}
	apParser->updateIndexRecords();

	// データベースのクローズ
    SyncCloseDB(rHandle);
}

/*
 *   ログファイルをコピーする...
 *
 */
int CNNsiMonaSync::copyLogDatFile(std::string &aBasePath, std::string &aBoardNick, char *apSeparator, threadIndexRecord *apRecord, void *apBuffer, long aSize, long *linePtr, long resSize)
{
	int resNum = 0;
	char *ptr, *buf, *base;

	// boardNick...
	char boardNick[96];
	sprintf(boardNick, "%s", aBoardNick.c_str());
	boardNick[strlen(boardNick) - 1] = '\\';  // 末尾の / を \ に置換する

	// ファイル名...
	std::string fileName;
	apRecord->getFileName(fileName);
	if (strlen(fileName.c_str()) < 1)
	{
		// ファイル名が取得できない場合にはレス数ゼロで応答する...
		return (0);
	}

    // full path名をつくる
	char fullPath[1024];
	sprintf(fullPath, "%s%s%s", aBasePath.c_str(), boardNick, fileName.c_str());

	// ファイルをオープンする (書き込み用)
	HANDLE hFile = CreateFile(fullPath,
		                      GENERIC_WRITE, 
		                      FILE_SHARE_WRITE,
							  NULL, 
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL, 
							  NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// ファイルオープンに失敗。。。終了する
		return (0);
	}
	SetEndOfFile(hFile);

	DWORD  lines = 0;
	base = static_cast<char *>(apBuffer);
	buf  = base;
	ptr  = buf;
	while (ptr < (base + aSize))
	{
        if (*ptr == '\x0a')
		{
			resNum++;
			if (resNum < resSize)
			{
                linePtr[resNum] = lines;
                linePtr[resNum + 1] = lines;
			}

			DWORD writeSize = 0;
			DWORD length    = 0;

			// レス（本文）を出力する
			writeSize = static_cast<DWORD>(ptr - buf);
			length    = writeSize;
			(void) WriteFile(hFile, buf, length, &writeSize, NULL);

			// セパレータを出力する
			writeSize = static_cast<DWORD>(strlen(apSeparator));
			length    = writeSize;
			(void) WriteFile(hFile, apSeparator, length, &writeSize, NULL);

            // 先頭を設定する...
			buf = ptr + 1;
			lines++;
		}
		if ((*ptr == '<')&&(*(ptr + 1) == '>'))
		{
			lines++;
		}
		if ((*ptr == '<')&&
			((*(ptr + 1) == 'b')||(*(ptr + 1) == 'B'))&&
			((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R'))&&
			((*(ptr + 3) == '>')))
		{
			lines++;
		}
		ptr++;
	}
	// ファイルをクローズする
	CloseHandle(hFile);
	return (resNum);
}
