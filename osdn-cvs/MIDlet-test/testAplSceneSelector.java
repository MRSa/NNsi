import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;

public class testAplSceneSelector
{
    private final int  SCENE_DEFAULT  = -1;

	private MIDlet          parent  = null;
	private testAplFactory  factory = null;

	private int    currentScene     = SCENE_DEFAULT;   // 現在の画面
    private String  currentMessage  = null;  

    /**
	 * コンストラクタ
	 * @param parent
	 */
	testAplSceneSelector(MIDlet object)
	{
		parent  = object;
		factory = new testAplFactory();
	}

	/**
	 * 起動準備
	 * 
	 * @return
	 */
	public boolean initialize()
	{
		// 内部情報の準備...
		factory.prepare(this);
		
		// 画面表示の準備...
		factory.prepareScreen();

		System.gc();

		return (true);
	}
	
	/**
	 *  初期画面にシーンを切り替える
	 *
	 */
	public void start()
	{
		changeScene(SCENE_DEFAULT, "");

		return;
	}

	/**
	 *  初期画面にシーンを切り替える
	 *
	 */
	public void stop()
	{
		factory.cleanup();
		return;
	}

	/**
	 *  画面表示切替スレッド実行
	 *
	 */
	private void changeScene(int scene, String message)
	{
		currentScene   = scene;
		currentMessage = message;
		Thread thread = new Thread()
		{
			public void run()
			{
				// 画面表示を切り替える
				factory.changeScene(currentScene, currentMessage);
				
            	// 入力フォームに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());                	
			}
		};
		thread.start();
		return;
	}

	/**
	 * ファイルリスト取得用オブジェクトの取得。。
	 * 
	 * @return
	 */
    public testAplFileDirList getDirList()
    {
    	return (factory.getFileList());
    }
}
