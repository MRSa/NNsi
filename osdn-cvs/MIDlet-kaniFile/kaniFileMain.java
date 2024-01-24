import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

/**
 * kaniFileMain : 簡易ファイル管理ツール
 * @author MRSa
 *
 */
public class kaniFileMain extends MIDlet
{
	private kaniFileSceneSelector sceneSelector = null;
	
	/**
	 *  コンストラクタ
	 *
	 */
	public kaniFileMain()
	{
		super();

		// オブジェクトを生成しておく
		sceneSelector = new kaniFileSceneSelector(this);
	}

	/**
	 *  アプリケーションの起動処理
	 * 
	 */
	protected void startApp() throws MIDletStateChangeException
	{
		splashScreen startScreen = new splashScreen();

		// スプラッシュスクリーンを表示する...
		Display.getDisplay(this).setCurrent(startScreen);

		// シーンセレクタに初期化を指示する
		boolean ret = sceneSelector.initialize();
		if (ret == false)
		{
			// 起動失敗...終了する
			destroyApp(true);
			return;
		}

		// シーンセレクタに処理の開始を指示する
		sceneSelector.start();
		
		// あとしまつ(スプラッシュスクリーンを消す)...
		startScreen.finish();
		startScreen = null;

		return;
	}

	/**
	 *  アプリケーションの一時停止処理
	 * 
	 */
	protected void pauseApp()
	{
		// 現在のところ、何もしない...
		return;
	}

	/**
	 *  アプリケーションの終了処理
	 * 
	 */
	protected void destroyApp(boolean arg0) throws MIDletStateChangeException
	{
		if (arg0 == true)
		{
			// データのバックアップとオブジェクトの削除を実行する
			sceneSelector.stop();

			// 終了指示
			notifyDestroyed();
		}
		return;
	}

	/**
	 * スプラッシュスクリーン(起動画面)の表示クラス
	 * 
	 * @author MRSa
	 *
	 */
	private class splashScreen extends Canvas
	{
		Image image = null;  // イメージファイルの終了
		
		/**
		 * コンストラクタ(何もしない)
		 *
		 */
		public splashScreen()
		{
			// イメージファイルを読み出す
			try
			{
				image = Image.createImage("/res/splash.png");
			}
			catch (Exception e)
			{
				image = null;
			}
		}

		/**
		 *  起動画面を表示する実処理
		 *  
		 */
		public void paint(Graphics g)
		{
			// タイトルの設定
			String screenTitle = "kaniFile";
			
			// 画面の塗りつぶし
			g.setColor(255, 255, 255);
			g.fillRect(0, 0, getWidth(), getHeight());
			
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
				int x = ((getWidth()  - image.getWidth())  / 2);
				if ((x + image.getWidth()) > getWidth())
				{
					x = 0;
				}

				int y = ((getHeight() - image.getHeight()) / 2);
				if ((y + image.getHeight()) > getHeight())
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
