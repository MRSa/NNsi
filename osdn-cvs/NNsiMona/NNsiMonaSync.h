/*****************************************************************************
 *
 * Generic Conduit CNNsiMonaSync Header File
 *
 ****************************************************************************/
#ifndef __NNsiMona_SYNCHRONIZER_H_
#define __NNsiMona_SYNCHRONIZER_H_

#include <string>
#include <map>
#include <GenSync.H>
#include <VFSMgr.h>
#include <wininet.h>
#include "NNsiMonaPCMgr.h"
#include "NNsiMonaBackupMgr.h"
#include "NNsiMonaRecord.h"

#include "I2chBrowserParser.h"


// データベースの名称
#define DBNAME_BBSLIST        "BBS-NNsi"
#define DBNAME_SUBJECT        "Subject-NNsi"

// メッセージの属性
#define NNSH_MSGATTR_NOTSPECIFY    0xff   // 「お気に入り」指定は指定しない
#define NNSH_MSGATTR_NOTFAVOR      0x4f   // 「お気に入り」スレでない(MASK値)
#define NNSH_MSGATTR_FAVOR_L1      0x10   // 「お気に入り」レベル(低)
#define NNSH_MSGATTR_FAVOR_L2      0x20   // 「お気に入り」レベル(中低)
#define NNSH_MSGATTR_FAVOR_L3      0x30   // 「お気に入り」レベル(中)
#define NNSH_MSGATTR_FAVOR_L4      0x80   // 「お気に入り」レベル(中高)
#define NNSH_MSGATTR_FAVOR         0xb0   // 「お気に入り」スレ(高)
#define NNSH_MSGATTR_NOTERROR      0xbf   // 「エラー発生」スレでない(MASK値)
#define NNSH_MSGATTR_ERROR         0x40   // 「エラー発生」スレである
#define NNSH_MSGATTR_GETRESERVE    0x08   // 「取得予約」スレである
#define NNSH_MSGATTR_NOTRESERVE    0xf7   // 「取得予約」スレをクリアする(MASK)

// スレの取得状態
#define NNSH_SUBJSTATUS_NOT_YET      0
#define NNSH_SUBJSTATUS_NEW          1
#define NNSH_SUBJSTATUS_UPDATE       2
#define NNSH_SUBJSTATUS_REMAIN       3
#define NNSH_SUBJSTATUS_ALREADY      4
#define NNSH_SUBJSTATUS_DELETE       5
#define NNSH_SUBJSTATUS_UNKNOWN      6
#define NNSH_SUBJSTATUS_GETERROR     7
#define NNSH_SUBJSTATUS_OVER         8
#define NNSH_SUBJSTATUS_DELETE_OFFLINE  20

// スレのログ格納場所...
#define FILEMGR_STATE_DISABLED      0            // ログなし
#define FILEMGR_STATE_OPENED_VFS    1            // VFS(メモリカード)にログ記録
#define FILEMGR_STATE_OPENED_STREAM 2            // Stream(Palm本体)にログ記録

// BBSタイプ
#define NNSH_BBSTYPE_2ch                 0x00    // 2ch(通常モード)
#define NNSH_BBSTYPE_MACHIBBS            0x01    // まちBBS(特殊モード)
#define NNSH_BBSTYPE_SHITARABA           0x02    // したらばBBS
#define NNSH_BBSTYPE_SHITARABAJBBS_OLD   0x03    // したらば@JBBS
#define NNSH_BBSTYPE_OTHERBBS            0x04    // 2ch互換BBS
#define NNSH_BBSTYPE_PLUGINSUPPORT       0x05    // 2ch形式変換BBS
#define NNSH_BBSTYPE_HTML                0x06    // html形式データ
#define NNSH_BBSTYPE_SHITARABAJBBS_RAW   0x07    // したらば@JBBS(RAWMODE)
#define NNSH_BBSTYPE_PLAINTEXT           0x08    // 通常のテキスト形式
#define NNSH_BBSTYPE_2ch_EUC             0x09    // 2ch(EUC形式)
#define NNSH_BBSTYPE_OTHERBBS_2          0x0a    // 2ch互換形式(その2)
#define NNSH_BBSTYPE_ERROR               0xff    // ボードタイプエラー
#define NNSH_BBSTYPE_MASK                0x1f    // ボードタイプMASK
#define NNSH_BBSTYPE_CHARSETMASK         0xe0    // ボードの文字コード
#define NNSH_BBSTYPE_CHAR_SJIS           0x00    // 標準文字コード(SHIFT JIS)
#define NNSH_BBSTYPE_CHAR_EUC            0x20    // EUC漢字コード
#define NNSH_BBSTYPE_CHAR_JIS            0x40    // JIS漢字コード(iso-2022-jp)
#define NNSH_BBSTYPE_CHAR_SHIFT             5    // 右シフト

#define NNSH_RECEIVE_RESPONSE_NG          -10
#define NNSH_RECIEVE_ERROR                 -1
#define NNSH_BBSTYPE_NG                    -2
#define NNSH_NOTFOUND_BBS                  -3
#define NNSH_MEMORY_ALLOCATION_FAILURE     -4
#define NNSH_RECEIVE_DATA_WRONG            -5
#define NNSH_LOGGINGPLACE_WRONG            -6
#define NNSH_LOGGINGPLACE_WRONG_VFS        -7
#define NNSH_LOGGINGTYPE_NOTMATCHED        -8
#define NNSH_NORMAL_END                     0
#define NNSH_NOTMODIFIED                    1

#define NNSIMONA_FILENAME_BUFFERLEN       512
#define NNSIMONA_PREFERENCE_FILENAME      "NNsiMona.dat"


/////////////////////////////////////////////////////////////////////////////////
//
//  "NNsiMona.dat" に記録するデータ...
//
/////////////////////////////////////////////////////////////////////////////////
typedef struct NNsiMonaPrefTag
{
	char browserFileLocation[NNSIMONA_FILENAME_BUFFERLEN];
	char browserConfigLocation[NNSIMONA_FILENAME_BUFFERLEN];
	int  browserType;
	int  waitTime;
	eSyncTypes syncType;
} NNsiMonaPreferences;
/////////////////////////////////////////////////////////////////////////////////

using namespace std;

class CNNsiMonaSync : public CSynchronizer
{
public:
    CNNsiMonaSync(CSyncProperties& rProps, DWORD dwDatabaseFlags);
    virtual ~CNNsiMonaSync();

public:
	virtual long Perform(void);
	virtual void setPreferences(NNsiMonaPreferences *dataSource);

protected:
    virtual CPDbBaseMgr *CreateArchiveManager(TCHAR *pFilename);
	virtual long CreatePCManager(void);
    virtual long CreateBackupManager(void);
	virtual long CopyAppInfoPCtoHH( void );
    virtual long CopyAppInfoHHtoPC( void );

    virtual long SynchronizeAppInfo(void);

private:
	void uploadBBSdatabase(void);
	void updateThreadInformation(void);
	void updateAndCopyHHtoPC(I2chBrowserParser *apParser);
	void updateAndCopyPCtoHH(I2chBrowserParser *apParser);

	long getThreadLog(NNshSubjectDatabase *dbPtr);

	long httpCommunication(char *url, int range);

	long receivedWholeMessage(HINTERNET hService, long contentLength);
	long receivedPartMessage (HINTERNET hService, long contentLength);
	long getContentLength(char *str);
	BOOL checkVFSVolumes(char *boardNick, char *datFileName);
	BOOL checkGetThreadStatus(NNshSubjectDatabase *dbPtr);

	int  copyLogDatFile(std::string &aBasePath, std::string &aBoardNick, char *apSeparator, threadIndexRecord *apRecord, void *apBuffer, long aSize, long *linePtr, long resSize);

private:
	std::map<std::string,std::string>   mBBSmap;
	NNshSubjectDatabase                *mDbPtr;
	FileRef                             mVfsFileRef;
	NNsiMonaPreferences                 mNNsiMonaPreferences;

	std::map<std::string,std::string>   mThreadNick;

};

#endif
