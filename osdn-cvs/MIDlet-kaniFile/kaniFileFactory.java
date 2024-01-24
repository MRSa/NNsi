/**
 * オブジェクトの管理クラス
 * 
 * @author MRSa
 *
 */
public class kaniFileFactory
{
	// オブジェクト群...
	private kaniFileZipOperations   zipOperation  = null;
	private kaniFileDirList         directoryInfo = null;
	private kaniFileOperations      fileOperation = null;
	private kaniFileTextFile        textFileInfo  = null;
	private kaniFileUtils           utils         = null;
	private kaniFileDatas           dataStorage   = null;
	private kaniFileHttpCommunication httpComm    = null;
	private kaniFileForm            inputForm     = null;
	private kaniFileCanvas          canvasForm    = null;
	
	/**
	 * コンストラクタ
	 * @param object 親クラス
	 */
	public kaniFileFactory()
	{
		// 何もしない...
	}

	/**
	 * オブジェクトの生成と準備
	 * 
	 * @return  オブジェクト生成成功/生成失敗
	 */
	public boolean prepare(kaniFileSceneSelector parent)
	{
        // オブジェクトを生成する
		zipOperation   = new kaniFileZipOperations(parent);
		directoryInfo  = new kaniFileDirList();
		dataStorage    = new kaniFileDatas();
		textFileInfo   = new kaniFileTextFile();
		utils          = new kaniFileUtils();
		fileOperation  = new kaniFileOperations(parent);
		httpComm       = new kaniFileHttpCommunication();
		inputForm      = new kaniFileForm("", parent);
		canvasForm     = new kaniFileCanvas(parent);

		// オブジェクトの準備を実行する
		dataStorage.prepare();
		directoryInfo.prepare(dataStorage.getDirectory());
		httpComm.prepare();
		inputForm.prepare();
		canvasForm.prepare();
		return (true);
	}

	/**
	 * キャンバスクラスを取得する
	 * @return
	 */
	public kaniFileCanvas getCanvas()
	{
		return (canvasForm);
	}
	
	/**
	 * フォームクラスを取得する
	 * @return
	 */
	public kaniFileForm getForm()
	{
		return (inputForm);
	}

	/**
	 *  Zip操作クラスを取得する
	 * @return
	 */
	public kaniFileZipOperations getZipOperation()
	{
        return (zipOperation);
	}
	
	/**
	 * ファイル操作クラスを取得する
	 * @return
	 */
	public kaniFileOperations getFileOperation()
	{
		return (fileOperation);
	}

	/**
	 * データ格納クラスを取得する
	 * @return データ格納クラス
	 */
	public kaniFileDatas getDataStorage()
	{
		return (dataStorage);
	}

	/**
	 * テキストデータ格納クラスを取得する
	 * @return テキストデータ格納クラス
	 */
	public kaniFileTextFile getTextDataStorage()
	{
		return (textFileInfo);
	}

	/**
	 * ユーティリティクラスを取得する
	 * @return ユーティリティクラス
	 */
	public kaniFileUtils getUtils()
	{
		return (utils);
	}

	/**
	 * HTTP通信クラスを取得する
	 * @return HTTP通信クラス
	 */
	public kaniFileHttpCommunication getHttpCommunicator()
	{
		return (httpComm);
	}
	
	/**
	 * ディレクトリ情報クラスを取得する
	 * @return ディレクトリ情報クラス
	 */
	public kaniFileDirList getDirectoryInfo()
	{
		return (directoryInfo);
	}

	/**
	 * 画面表示の準備
	 *
	 */
	public void prepareScreen()
	{
		canvasForm.prepareScreen();
		inputForm.prepareScreen();
		return;
	}

	/**
	 * 管理オブジェクトの削除
	 *
	 */
	public void cleanup()
	{
		// データをバックアップする
		dataStorage.store();

        // オブジェクトをクリアする
		utils         = null;
		textFileInfo  = null;
		fileOperation = null;
		inputForm     = null;
		canvasForm    = null;
		dataStorage   = null;
		httpComm      = null;
		return;
	}
}
