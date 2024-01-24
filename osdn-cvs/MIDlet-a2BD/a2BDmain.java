import java.util.Date;

import javax.microedition.io.HttpConnection;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

/**
 * MIDletメインクラス
 * @author MRSa
 *
 */
public class a2BDmain extends MIDlet
{
	private a2BDobjectFactory      objectFactory      = null;    // オブジェクト管理クラス
    public  long                  watchDogMarking     = -1;      //  ウォッチドッグのマーキング

	private final int             HTTPCOMM_DEFAULT   = 0;	

    /**
	 *   コンストラクタ
	 * 
	 */
	public a2BDmain()
	{
		super();
		// オブジェクトを生成しておく
		objectFactory = new a2BDobjectFactory(this);
	}

	/**
	 *   アプリケーション起動時に呼び出される処理
	 * 
	 */
	protected void startApp() throws MIDletStateChangeException
	{
		splashScreen startScreen = new splashScreen();

		// スプラッシュスクリーンを表示する...
		Display.getDisplay(this).setCurrent(startScreen);

		// オブジェクトの準備を行う
		objectFactory.prepare();		
		objectFactory.prepareScreen();

		// アプリケーションのスタート指示
		(objectFactory.sceneSelector).start();
		
		// あとしまつ...
		startScreen.finish();
		startScreen = null;

	}

	/**
	 *   アプリケーション一時停止時に呼び出される処理
	 *  
	 */
	protected void pauseApp()
	{
		// 現在のところ、何もしない...
		return;
	}

	/**
	 *   アプリケーション正常終了時に呼び出される処理
	 *   
	 */
	protected void destroyApp(boolean arg0) throws MIDletStateChangeException
	{
		if (arg0 == true)
		{
			// データのバックアップとオブジェクトの削除を実行する
			objectFactory.cleanup();

			// 終了指示
			notifyDestroyed();
		}
		return;
	}


	/** 
	 *   a2BD終了処理...
	 *   
	 */
	public void quit_a2BD()
	{
		// a2B終了を指示された場合...
		try
		{
			// アプリ終了を呼び出す...
			destroyApp(true);
		}
		catch (Exception e)
		{
			// 例外処理としては何もしない
		}
		return;
	}

	/**
	 * 表示画面の切り替え
	 * 
	 * @param scene
	 */
	public void changeScene(int scene)
	{
		if (scene == (objectFactory.sceneSelector).SCENE_OPEN_DATAINPUT)
		{
			// 入力フォームを(新規)オープンする
			(objectFactory.inputForm).prepareScreen("ファイル取得", "URL", "File", true);
			Display.getDisplay(this).setCurrent((objectFactory.inputForm));
		}
		else if (scene == (objectFactory.sceneSelector).SCENE_DATAINPUT)
		{
			// 入力フォームをオープンする
			Display.getDisplay(this).setCurrent((objectFactory.inputForm));
		}
		else if (scene == (objectFactory.sceneSelector).SCENE_EXECUTE_DATA)
		{
			// BUSYダイアログを表示する
			Display.getDisplay(this).setCurrent((objectFactory.busyScreen));			

			// 処理の実行...
			Thread thread = new Thread()
			{
				public void run()
				{
					// HTTP通信の実行
					String url = (objectFactory.preference).getData1();
					String fileName = (objectFactory.preference).getData2();
					startHttpCommunication(HTTPCOMM_DEFAULT, url, (objectFactory.preference).decideFileName(url, fileName), (objectFactory.preference).getWX310workaroundFlag(), (objectFactory.httpCommunicator).APPENDMODE_NEWFILE);
                }
			};
			thread.start();
		}
		else if (scene == (objectFactory.sceneSelector).SCENE_SHOW_BUSY)
		{
			// BUSYダイアログを表示する
			Display.getDisplay(this).setCurrent((objectFactory.busyScreen));			
		}
		else if (scene == (objectFactory.sceneSelector).SCENE_FILE_SELECT)
		{
			// ファイル選択ダイアログを表示する
			Display.getDisplay(this).setCurrent((objectFactory.fileSelector));						
		}
		else // (scene == (objectFactory.sceneSelector).SCENE_UNKNOWN)
		{
			// 終了...
		}
		return;
	}
	

	/**
	 * HTTP通信の開始
	 * @param url
	 * @param fileName
	 */
	private void startHttpCommunication(int scene, String url, String fileName, boolean isWorkAroundWX310, int mode)
	{
		if ((objectFactory.httpCommunicator).isCommunicating() == true)
		{
			// 通信中のときは何もしない。
			return;
		}

        // HTTP通信の実施
		if ((objectFactory.preference).getDivideGetHttp() == false)
		{
            // 通常のHTTP通信
			(objectFactory.httpCommunicator).getURLUsingHttp(fileName, url, null, -1, -1, mode, (objectFactory.preference).getUserAgent(), isWorkAroundWX310);
		}
		else
		{
			// HTTP分割取得の実施...
			int rc = HttpConnection.HTTP_PARTIAL;
			int startRange = 0;
			(objectFactory.httpCommunicator).getURLUsingHttp(fileName, url, null, 0, -1, (objectFactory.httpCommunicator).APPENDMODE_NEWFILE, (objectFactory.preference).getUserAgent(), isWorkAroundWX310);
			while (rc == HttpConnection.HTTP_PARTIAL)
			{
				startRange = startRange + (objectFactory.httpCommunicator).getDivideGetSize() + 1;
				rc = (objectFactory.httpCommunicator).getURLUsingHttp(fileName, url, null, startRange, -1, (objectFactory.httpCommunicator).APPENDMODE_APPENDFILE, (objectFactory.preference).getUserAgent(), isWorkAroundWX310); 
			}
		}
		if ((objectFactory.preference).getAutoFinish() == true)
		{
            // 自動終了のためのタイマを設定する
			keepWatchDog(0);
			startWatchDog();
		}
        return;
	}

	/**
	 * データ2を設定する
	 * @param data
	 */
	public void setData2(String data)
	{
		(objectFactory.preference).setData2(data);
		(objectFactory.inputForm).setData2(data);

		return;
	}
	
    /*------------------------------------------------------------------------------------------------------------*/
    /**
     *   ウオッチドッグスレッド
     * 
     */
    public void startWatchDog()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                Date  date      = new Date();
                long time      = date.getTime();
                long dummy     = time;
                long sleepTime = (1 * 1000 * 60) / 10;  // 6 ～ 12 秒後に自動終了させる
                while (dummy != 0)
                {
                    if (watchDogMarking != -1)
                    {
                        // ウォッチドッグ停止中ではなかったとき、、、
                        time = date.getTime();
                        watchDogMarking = time;
                    }
                    try
                    {
                        Thread.sleep(sleepTime); // 待つ...
                    }
                    catch (Exception e)
                    {
                        // 何もしない...
                    }
                    if (time == watchDogMarking)
                    {
                        // タイムアウト検出、、、アプリケーションを終了する
                        objectFactory.cleanup();
                        notifyDestroyed();
                        return;
                    }
                }
            }
        };
        thread.start();
        return;
    }

    /**
     *  ウオッチドッグをクリアする。。
     *
     */
    public void keepWatchDog(int number)
    {
        watchDogMarking = number;
        return;
    }
    /*------------------------------------------------------------------------------------------------------------*/

    /**
	 * スプラッシュスクリーン(起動画面)の表示クラス
	 * 
	 * @author MRSa
	 *
	 */
	public class splashScreen extends Canvas
	{
		Image image = null;  // イメージファイルの終了
		
		/**
		 * コンストラクタ(何もしない)
		 *
		 */
		public splashScreen()
		{
			image = null;
		}

		/**
		 *  起動画面を表示する実処理
		 *  
		 */
		public void paint(Graphics g)
		{
			// タイトルの設定
			String screenTitle = "a2B downloader";
			
			// 画面の塗りつぶし
			g.setColor(255, 255, 255);
			int width = 240;
			int height = 280;
			try
			{
				width = getWidth();
				height = getHeight();
			}
			catch (Exception ex)
			{
				//
			}
			g.fillRect(0, 0, width, height);
			
			if (image == null)
			{
				// タイトルの表示 (文字)
				g.setColor(32, 32, 32);
				Font font = Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_BOLD, Font.SIZE_LARGE);
				g.setFont(font);
				g.drawString(screenTitle, 0, (getHeight() / 2), (Graphics.LEFT | Graphics.TOP));
				return;
			}
			else
			{
				// イメージの表示座標を決める
				int x = ((width  - image.getWidth())  / 2);
				if ((x + image.getWidth()) > width)
				{
					x = 0;
				}

				int y = ((height - image.getHeight()) / 2);
				if ((y + image.getHeight()) > height)
				{
					y = 0;
				}
				
				// スプラッシュスクリーンのイメージを描画
				g.setColor(0);
				g.drawImage(image, x, y, (Graphics.LEFT | Graphics.TOP));
			}
			return;
		}

		/**
		 *   起動画面の後始末
		 *
		 */
		public void finish()
		{
			// ガベコレ実施...
			image = null;
			System.gc();
			
			return;
		}
	}
}
