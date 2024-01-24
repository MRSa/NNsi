import javax.microedition.lcdui.Canvas;

public class testAplFactory
{
	private testAplCanvas      screen0 = null;
	private testAplFileDirList fileList = null;
	
    /**
     *  コンストラクタ
     *
     */
	public testAplFactory()
	{
		// 何もしない...
	}

	/**
	 * オブジェクトの生成と準備
	 * 
	 * @return  オブジェクト生成成功/生成失敗
	 */
	public boolean prepare(testAplSceneSelector parent)
	{
        screen0 = new testAplCanvas(parent);
        screen0.prepare();

        fileList = new testAplFileDirList();

        return (true);
	}

	/**
	 * 画面表示の準備
	 *
	 */
	public void prepareScreen()
	{
		//
		screen0.prepareScreen();
		return;
	}

	/**
	 * 管理オブジェクトの削除
	 *
	 */
	public void cleanup()
	{
        //
		return;
	}
	
	/**
	 *  画面の切り替え
	 * 
	 * @param scene
	 * @param message
	 */
	public void changeScene(int scene, String message)
	{
		// 
		return;
	}

	/**
	 * 表示する画面を応答する
	 * 
	 * @return
	 */
	public Canvas getCanvas()
	{
		return (screen0);
	}
	
	/**
	 * ファイルリストオブジェクトを応答する
	 * 
	 * @return
	 */
	public testAplFileDirList getFileList()
	{
		return (fileList);
	}	
}
