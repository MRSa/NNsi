///////////////////////////////////////////////////////
//// NNsiSync.H  : NNsi用の同期クラス(ヘッダ)
///////////////////////////////////////////////////////
#ifndef  __NNSISYNC_H__
#define  __NNSISYNC_H__

// CSynchronizerクラスを継承して作成する
class NNsiCSynchronizer : public CSynchronizer
{
  public:
    // コンストラクタ(CSynchronizerと同じ)
	NNsiCSynchronizer(HANDLE langInstance, CSyncProperties& rProps, DWORD dwDatabaseFlags = 0);

    // HotSyncの処理開始
    virtual long Perform(void);

  protected:
	// 使用言語
	HANDLE m_hLangInstance;

	// リモートDBのレコードサイズ
	long m_remoteDBrecSize;

  private:
	// デバッグログ出力
	void NNsiDebugLog(char *message, char *msg2, long value);

	// ログ情報出力
	void NNsiOutputInfoLog(UINT strId, char *msg, long value);

	// 同期するDB情報を準備
	long PrepareRemoteDBInfo(char *dbName, long version, long recSize);


	// 同期前処理
	long PrologueAllSynchronize(void);
	long PrologueSynchronize(char *dbName);

	// 同期後処理
	long EpilogueAllSynchronize(void);
	long EpilogueSynchronize(char *dbName);

	// 同期処理
	long ExecuteSynchronize(char *dbName, long version, long recSize);

	// HHマネージャをNNsiMona固有のものを定義
	virtual long CreateHHManager(void);

	// デバッグ用に仮追加(AppInfoは同期させないようにする。)
	long SynchronizeAppInfo(void);
	long CopyAppInfoHHtoPC(void);
	long CopyAppInfoPCtoHH(void);
};


#endif
