/**
 * オブジェクト管理クラス
 * 
 * @author MRSa
 *
 */
public class a2BDobjectFactory
{
	private a2BDmain           parent = null;  // 親オブジェクト
    public  a2BDHttpCommunicator httpCommunicator = null; // HTTP通信セレクタ
    public  a2BDfileSelector     fileSelector = null; // ファイルセレクタ
	public  a2BDsceneSelector  sceneSelector = null; // 画面セレクタ
    public  a2BDdataInput      inputForm = null;     // データ入力フォーム
    public  busyForm           busyScreen = null;    // 実行中表示画面
    public  a2BDPreference     preference = null;    // 設定項目

	/**
	 * コンストラクタ
	 * @param object 親クラス
	 */
	public a2BDobjectFactory(a2BDmain object)
	{
		parent = object;
	}
	
	/**
	 * 管理オブジェクトの生成
	 * 
	 * @return  オブジェクト生成成功/生成失敗
	 */
	public boolean prepare()
	{
		preference       = new a2BDPreference();
        preference.restore();

        busyScreen       = new busyForm(parent, "");
		sceneSelector    = new a2BDsceneSelector(parent, busyScreen);
		inputForm        = new a2BDdataInput("", sceneSelector, preference);
		httpCommunicator = new a2BDHttpCommunicator(sceneSelector);
		fileSelector     = new a2BDfileSelector(sceneSelector);
		return (true);
	}

	/**
	 * 画面表示の準備
	 *
	 */
	public void prepareScreen()
	{
		busyScreen.prepareScreen();
		return;
	}
	/**
	 * 管理オブジェクトの削除
	 *
	 */
	public void cleanup()
	{
        // 設定データのクリーンアップ
		preference.backup();

		// オブジェクトのお掃除...
		parent = null;
		httpCommunicator = null;
		sceneSelector = null;
		inputForm = null;
		busyScreen = null;
		preference = null;
		System.gc();
		return;
	}
}
