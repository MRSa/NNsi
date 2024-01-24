///////////////////////////////////////////////////////
//// NNsiSync.CPP  : NNsi用の同期クラス
///////////////////////////////////////////////////////
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
#include "NNsiMona.h"
#include <logstrng.h>
#include <gensync.h>
#include <PALM_CMN.H>
#include <Pgenerr.h>
#include "resource.h"
#include "hhmgr.h"
#include "NNsiSync.H"
#include "NNsiHHMgr.h"

// NNsi本体の定義(V1.05 Revision001より)
#include "symbols.h"

// 元のソースより引っ張ってきた（でも格好悪いから、、、あとで変更する。）
#define MAX_PROD_ID_TEXT    255

/******************************************************************************
 * Class:   NNsiCSynchronizer (コンストラクタ)
 *
 *****************************************************************************/
NNsiCSynchronizer::NNsiCSynchronizer(HANDLE langInstance, CSyncProperties& rProps, DWORD dwDatabaseFlags) : m_hLangInstance(langInstance), CSynchronizer(rProps, dwDatabaseFlags = 0)
{
   TRACE0("NNsiCSynchronizer Constructor\n");
}

/******************************************************************************
 *  Method:   NNsiCSynchronizer::Perform() (HotSync処理の実行)
 *
 *****************************************************************************/
long NNsiCSynchronizer::Perform()
{
    long retval  = 0;

    // HotSyncスタートのログを吐く
    NNsiDebugLog("HotSync started...", "", 0);

    if (m_rSyncProperties.m_SyncType > eProfileInstall)
    {
        // ログを吐く
        NNsiDebugLog("GEN_ERR_BAD_SYNC_TYPE", "", GEN_ERR_BAD_SYNC_TYPE);

        // SyncTypeの設定値がおかしい
        return (GEN_ERR_BAD_SYNC_TYPE);
    }

	// 同期しない、に設定されているかを確認
    if (m_rSyncProperties.m_SyncType == eDoNothing) 
    {
        // 「何もしない」ことをログに出力する
		NNsiOutputInfoLog(IDS_DO_NOTHING, "", 0);
        return (0);
    }

    // Obtain System Information
    m_SystemInfo.m_ProductIdText = (BYTE*) new char [MAX_PROD_ID_TEXT];
    if (m_SystemInfo.m_ProductIdText == NULL)
    {
        // ログを吐く
        NNsiDebugLog("GEN_ERR_LOW_MEMORY", "", GEN_ERR_LOW_MEMORY);

        // メモリ確保の失敗
        return (GEN_ERR_LOW_MEMORY);
    }
    m_SystemInfo.m_AllocedLen = (BYTE) MAX_PROD_ID_TEXT; 

    retval = SyncReadSystemInfo(m_SystemInfo);
    if (retval != 0)
    {
        // ログを吐く
        NNsiDebugLog("fail SyncReadSystemInfo()", "", retval);

        // システム情報の読み出し失敗
        return (retval);
    }

	// 同期処理開始前の処理(NNsiSyncExt.cpp)
	retval = PrologueAllSynchronize();
	if (retval != 0)
	{
		NNsiDebugLog("fail PrologueAllSynchronize()", "", retval);
		return (retval);
	}

	// Register the conduit
    retval = RegisterConduit();
    if (retval != 0)
    {
        // ログを吐く
        NNsiDebugLog("fail RegisterConduit()", "", retval);

        // 同期処理開始の失敗
        return (retval);
    }

    //////////////////////////////////////////////////
	/////////    同期処理の実行(NNsi用に変更)    /////
    //////////////////////////////////////////////////
	long retval2 = 0;

	// BBS一覧ＤＢの同期処理実施
	retval2      = ExecuteSynchronize(DBNAME_BBSLIST, DBVERSION_BBSLIST, DBSIZE_BBSLIST);
	if (retval2 != 0)
	{
		// BBS一覧ＤＢの同期処理失敗
		NNsiDebugLog("fail. ExecuteSynchronize(DBNAME_BBSLIST)", "", retval2);
	}
	retval  = retval2;

	// スレ一覧ＤＢの同期処理実施
	retval2 = ExecuteSynchronize(DBNAME_SUBJECT, DBVERSION_SUBJECT, DBSIZE_SUBJECT);
    if (retval2 != 0)
	{
		// スレ一覧ＤＢの同期処理失敗
		NNsiDebugLog("fail. ExecuteSynchronize(DBNAME_SUBJECT)", "", retval2);
	}
	retval  = retval | retval2;

    // NNsi設定ＤＢの同期処理実施
	retval2 = ExecuteSynchronize(DBNAME_SETTING, DBVERSION_SETTING, DBSIZE_SETTING);
	if (retval2 != 0)
	{
		// NNsi設定ＤＢの同期処理失敗
		NNsiDebugLog("fail. ExecuteSynchronize(DBNAME_SETTING)", "", retval2);
	}
	retval  = retval | retval2;

	// 参照専用ログ用ディレクトリ情報ＤＢの同期処理実施
	retval2 = ExecuteSynchronize(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, DBSIZE_DIRINDEX);
	if (retval2 != 0)
	{
		// 参照専用ログ、ディレクトリ情報ＤＢの同期処理失敗
		NNsiDebugLog("fail. ExecuteSynchronize(DBNAME_DIRINDEX)", "", retval2);
	}
	retval = retval | retval2;

    // Unregister the conduit
    retval2 = UnregisterConduit((BOOL)(retval != 0));

	// 同期処理開始後の処理(NNsiSyncExt.cpp)
	long retval3 = 0;
	retval3      = EpilogueAllSynchronize();
	if (retval3 != 0)
	{
		NNsiDebugLog("fail EpilogueAllSynchronize()", "", retval3);
		return (retval3);
	}

	// 応答コードの判定
    if (retval != 0)
    {
        // どこかでHotSyncが失敗していた場合
        return (retval2);
    }
    return (retval);
}

/******************************************************************************
 *  Method:   NNsiCSynchronizer::NNsiDebugLog() (デバッグ用ログ出力)
 *
 *****************************************************************************/
void NNsiCSynchronizer::NNsiDebugLog(char *message, char *msg2, long value)
{
#ifdef _DEBUG
    char outputData[512];

    if (value != 0)
    {
        sprintf(outputData, "    %s %s 0x%x(%d)", message, msg2, value, value);
    }
    else
    {
        sprintf(outputData, "    %s %s", message, msg2);
    }

    // ログを吐く
    m_pLog->LogInfo(outputData);
#endif
    return;
}

/******************************************************************************
 *  Method:   NNsiCSynchronizer::NNsiOutputInfo() (ログ情報出力)
 *
 *****************************************************************************/
void NNsiCSynchronizer::NNsiOutputInfoLog(UINT strId, char *msg, long value)
{
    TCHAR szBuffer[330], buffer[300], hexBuffer[20];

	// リソースからテキストデータを取得する
    LoadString((HINSTANCE)m_hLangInstance, strId, buffer, sizeof(buffer));

	if (value != 0)
	{
		// 数値を16進数に変換
		_itoa(value, hexBuffer, 16);

		// ログ出力文字列を作成
		wsprintf(szBuffer, "    %s %s 0x%s(%u)", buffer, msg, hexBuffer, value);
	}
	else
	{
		// ログ出力文字列を作成
		wsprintf(szBuffer, "    %s %s", buffer, msg);
	}

    // ログを吐く
    m_pLog->LogInfo(szBuffer);
    return;
}

/*****************************************************************************
 * Method:  PrepareRemoteDBInfo() PalmデバイスのＤＢ情報を準備
 *
 *****************************************************************************/
long NNsiCSynchronizer::PrepareRemoteDBInfo(char *dbName, long version, long recSize)
{
    long retval = 0;

    if (m_remoteDB)
	{
		delete m_remoteDB;
	}
    m_remoteDB = new CDbList;
    if (m_remoteDB)
	{
        m_remoteDB->m_Creator   = m_rSyncProperties.m_Creator; 
        m_remoteDB->m_DbFlags   = eRecord; // todo
        m_remoteDB->m_DbType    = m_rSyncProperties.m_DbType; 
        strcpy(m_remoteDB->m_Name, dbName);
        m_remoteDB->m_Version   = 0;
        m_remoteDB->m_CardNum   = 0;
        m_remoteDB->m_ModNumber = 0;
        m_remoteDB->m_CreateDate= 0;
        m_remoteDB->m_ModDate   = 0;
        m_remoteDB->m_BackupDate= 0;
    }
	else
	{
		// メモリ不足エラー
        return (GEN_ERR_LOW_MEMORY);
    }

	// レコードサイズを格納する
    m_remoteDBrecSize = recSize + DBSIZE_MARGIN;
    return (0);
}

/******************************************************************************
 *  Method:   NNsiCSynchronizer::Perform() (HotSync処理の実行処理)
 *
 *****************************************************************************/
long NNsiCSynchronizer::ExecuteSynchronize(char *dbName, long version, long recSize)
{
	long retval = 0;
	char localName[BUFSIZE];

	// バックアップするＤＢ名を設定する
	memset(localName, 0x00, sizeof(localName));
	strcpy(localName, dbName);
	strcat(localName, ".dat");
	strcpy(m_rSyncProperties.m_LocalName, localName);
	NNsiDebugLog("Backup file name is ", m_rSyncProperties.m_LocalName, 0);

	// DB同期処理の前処理
	retval = PrologueSynchronize(dbName);
    if (retval != 0)
	{
        NNsiDebugLog("fail. PrologueSynchronize() ", dbName, retval);
		return (retval);
	}
	
	// 同期するＤＢのＤＢ情報を準備する
	retval = PrepareRemoteDBInfo(dbName, version, recSize);
	if (retval != 0)
	{
        NNsiDebugLog("fail. GetRemoteDBInfo() " , dbName, retval);
		return (retval);
	}

	// 同期実行フラグ状態によって処理を分岐
    switch (m_rSyncProperties.m_SyncType)
    {
      case eFast:
		// 高速同期
		retval = PerformFastSync();
        if ((retval) && (retval == GEN_ERR_CHANGE_SYNC_MODE))
        {
            if (GetSyncMode() == eHHtoPC)
            {
                retval = CopyHHtoPC();

				// ログを吐く
		        NNsiDebugLog("SyncType:eFast CopyHHtoPC() ", dbName, retval);
            }
            else if (GetSyncMode() == ePCtoHH)
            {
                retval = CopyPCtoHH();

				// ログを吐く
		        NNsiDebugLog("SyncType:eFast CopyPCtoHH() ", dbName, retval);
            }
        }
        // ログを吐く
        NNsiDebugLog("SyncType:eFast ", dbName, retval);
        break;

      case eSlow:
		// 低速同期
        retval = PerformSlowSync();  
        if ((retval) && (retval == GEN_ERR_CHANGE_SYNC_MODE))
        {
            if (GetSyncMode() == eHHtoPC)
            {
                retval = CopyHHtoPC();
            }
            else if (GetSyncMode() == ePCtoHH)
            {
                retval = CopyPCtoHH();
            }
        }
        // ログを吐く
        NNsiDebugLog("SyncType:eSlow ", dbName, retval);
        break;

      case eHHtoPC:
      case eBackup:
		// PalmのデータをＰＣに保管
        retval = CopyHHtoPC();
        // ログを吐く
        NNsiDebugLog("SyncType:eHHtoPC/eBackup ", dbName, retval);
        break;

      case eInstall:
      case ePCtoHH:
      case eProfileInstall:
		// PCのデータをPalmに保管
		retval = CopyPCtoHH();
        // ログを吐く
        NNsiDebugLog("SyncType:eInstall/ePCtoHH/eProfileInstall", dbName, retval);
        break;

      case eDoNothing:
      default:
        // 上記以外（"サポート外"のエラー)
        retval = GEN_ERR_SYNC_TYPE_NOT_SUPPORTED;

        // ログを吐く
        NNsiDebugLog("GEN_ERR_SYNC_TYPE_NOT_SUPPORTED", dbName, retval);
        break;
    }
    // ハンドラ系の削除
    DeleteHHManager();
    DeletePCManager();
    DeleteBackupManager();
    CloseArchives();

	// DB同期処理の後処理
	retval = EpilogueSynchronize(dbName);
    if (retval != 0)
	{
        NNsiDebugLog("fail. EpilogueSynchronize()", dbName, retval);
		return (retval);
	}
    return (retval);
}

// Palmと同期するマネージャの設定
long NNsiCSynchronizer::CreateHHManager(void)
{
    DeleteHHManager();

    m_dbHH = new NNsiCHHMgr(m_pLog, 
                            m_dwDatabaseFlags,
                            m_remoteDB->m_Name,
                            m_remoteDB->m_Creator,
                            m_remoteDB->m_DbType,
                            m_remoteDB->m_DbFlags,
                            0,
					    	m_remoteDB->m_CardNum,
                            m_rSyncProperties.m_SyncType);

    if (!m_dbHH)
	{
        NNsiDebugLog("fail. CreateHHManager()", m_remoteDB->m_Name, 0);
        return (GEN_ERR_LOW_MEMORY);
	}
    NNsiDebugLog("CreateHHManager()", m_remoteDB->m_Name, 0);
    return (m_dbHH->Open());
}

long NNsiCSynchronizer::SynchronizeAppInfo(void)
{
	// AppInfoは同期させない。。。
    return (0);
}

long NNsiCSynchronizer::CopyAppInfoHHtoPC(void)
{
	// AppInfoは同期させない。。。
    return (0);
}

long NNsiCSynchronizer::CopyAppInfoPCtoHH(void)
{
    // AppInfoは同期させない。。。
	return (0);
}
