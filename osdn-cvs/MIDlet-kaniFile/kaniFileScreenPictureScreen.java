import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

/**
 * 画像表示用の画面
 * @author MRSa
 *
 */
public class kaniFileScreenPictureScreen implements IkaniFileCanvasPaint
{
	private final int backColor       = 0x00ffffff;
	private final int foreColor       = 0x00000000;
//	private final int cursorBackColor = 0x000000f8;
//	private final int cursorForeColor = 0x00ffffff;

	private kaniFileSceneSelector parent = null;
	private Image imageObject = null;
	private Font screenFont = null;   // 表示に使用するフォント
	private int screenMode   = -1;    // 表示モード
	private int screenTopX   = 0;     // 描画領域の左端(X)
	private int screenTopY   = 0;     // 描画領域の左端(Y)
	private int screenWidth  = 160;   // 画面幅
	private int screenHeight = 160;   // 画面高さ
	private int screenLine   = 0;     // データ表示可能ライン数
	private int offsetX      = 0;     // 表示開始位置（Ｘ座標）
	private int offsetY      = 0;     // 表示開始位置（Ｙ座標）

	private boolean busyMode = false;
	private boolean useThumbnail = true;
	private String infoLine   = null;

	/**
	 * コンストラクタ
	 * @param object
	 */
	kaniFileScreenPictureScreen(kaniFileSceneSelector object)
	{
		//
		parent = object;
	}

	/**
	 * 画面の準備処理...
	 */
	public void prepare(Font font, int mode)
	{
		//
		screenFont   = font;
		screenMode   = mode;
	}

	/**
     *  画面サイズを設定する...
	 * 
	 */
    public void setRegion(int topX, int topY, int width, int height)
    {
		screenTopX   = topX;
		screenTopY   = topY;
		screenWidth  = width;
		screenHeight = height;
		screenLine   = decideScreenLine(height);
    }
	
	/**
	 * 左上端X座標を応答する
	 * 
	 */
	public int getTopX()
	{
		return (screenTopX);
	}

    /**
     * モードを応答する...
     */
    public int getMode()
    {
    	return (screenMode);
    }

    /**
	 * 左上端Y座標を応答する
	 * 
	 */
	public int getTopY()
	{
		return (screenTopY);
	}

	/**
	 * 描画ウィンドウ幅を応答する
	 * 
	 */
	public int getWidth()
	{
		return (screenWidth);
	}

	/**
	 * 描画ウィンドウ高さを応答する
	 * 
	 */
	public int getHeight()
	{
		return (screenHeight);
	}
	
	/**
	 * 表示可能ライン数を求める
	 * @param font
	 * @param height
	 * @param mode
	 * @return
	 */
	private int decideScreenLine(int height)
	{
	    return (((height - (screenFont.getHeight() * 1)) / screenFont.getHeight()));
	}

	/**
	 *  画面切り替えメソッド...
	 * 
	 */
	public void changeScene()
	{
        String message = infoLine;
        int pos = infoLine.indexOf(":");
        if (pos >= 0)
        {
        	message = infoLine.substring(pos + 1);
        }
        infoLine = " 画像展開中... " + message;
		imageObject = null;
		System.gc();
		try
		{
			Image img = parent.getImage(useThumbnail);
			if (img == null)
			{
				// 画像読み込み失敗...
                infoLine = "<<< OUT OF MEMORY? >>>";
                System.gc();
				return;
			}
//	        String appendMessage = "(" + img.getWidth() + "," + img.getHeight() + ")";
			int width  = screenWidth;
			int height = screenHeight - (screenFont.getHeight() * 1);
			if (img.getHeight() < (height + offsetY))
			{
				offsetY = img.getHeight() - height - 1;
				if (offsetY < 0)
				{
					offsetY = 0;
				}
				if ((offsetY + height) > img.getHeight())
				{
                    height = img.getHeight() - offsetY;
				}
			}
			if (img.getWidth() < (width + offsetX))
			{
				offsetX = img.getWidth() - width - 1;
				if (offsetX < 0)
				{
					offsetX = 0;
				}
				if ((offsetX + width) > img.getWidth())
				{
                    width = img.getWidth() - offsetX;
				}
			}
            imageObject = Image.createImage(img, offsetX, offsetY, width, height, Sprite.TRANS_NONE);
            infoLine = img.getWidth() + "x" + img.getHeight() + "(" + offsetX + "," + offsetY + ")";
            if (useThumbnail == true)
            {
                infoLine = infoLine + "T";
            }
            else
            {
                infoLine = infoLine + "N";
            }
            img = null;
            infoLine = infoLine + ":" + message;
		}
		catch (OutOfMemoryError e)
		{
            imageObject = null;
            infoLine = " ERROR:: Out of Memory...";
		}
		catch (Exception e)
		{
            imageObject = null;
            infoLine = " ERROR >> " + e.getMessage();
		}
        System.gc();
		return;
	}
	
	/**
	 * 表示データの更新処理...
	 * 
	 */
	public void updateData()
	{
		return;
	}

	/**
	 * 
	 * 
	 */
	public boolean inputBack()
	{
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputForward()
	{
		return (false);
	}

	/**
	 *  上入力
	 * 
	 */
	public boolean inputPreviousItem()
	{
        offsetY = offsetY - 120;
        if (offsetY < 0)
        {
        	offsetY = 0;
        }
        changeScene();
		return (true);
	}

	/**
	 *  下入力
	 * 
	 */
	public boolean inputNextItem()
	{
        offsetY = offsetY + 120;
        changeScene();
		return (true);
	}

	/**
	 *  左入力
	 * 
	 */
	public boolean inputPreviousPage()
	{
        offsetX = offsetX - 120;
        if (offsetX < 0)
        {
        	offsetX = 0;
        }
        changeScene();
		return (true);
	}

	/**
	 *  右入力
	 * 
	 */
	public boolean inputNextPage()
	{
        offsetX = offsetX + 120;
        changeScene();
		return (true);
	}

	/**
	 *  コマンドを選択。。。画面を抜ける
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
		exitScreen();
		return (false);
	}

	/**
	 *  選択キャンセル。。。画面を抜ける
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		exitScreen();
		return (false);
	}

	/**
	 *  GAME Dボタンが押されたとき。。。画面を抜ける
	 * 
	 */
	public boolean inputOther(boolean isRepeat)
	{
		exitScreen();
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputTop(boolean isRepeat)
	{
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputBottom(boolean isRepeat)
	{
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNumber(int number)
	{
        if (number == 0)
        {
        	if (useThumbnail == true)
        	{
        		useThumbnail = false;
        	}
        	else
        	{
        		useThumbnail = true;
        	}

            // 画像ファイル再読み込み...
			Thread thread = new Thread()
			{
				public void run()
				{
                    changeScene();
				}
			};
			thread.run();
        }
		return (true);
	}

	/**
	 *  実行ボタンが押されたとき。。。画面を抜ける
	 */
	public boolean inputDummy()
	{
		exitScreen();
		return (false);
	}
	
	/**
	 * 画面を表示
	 * 
	 */
	public void paint(Graphics g)
	{
		try
		{
			// タイトル領域の描画
			g.setColor(0x00d8ffd8);
			g.fillRect(screenTopX, screenTopY, screenWidth, screenFont.getHeight());
			drawTitleArea(g);

			if (busyMode != true)
			{
				// メインパネルの描画...
		        int infoTop = screenHeight;
				drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (infoTop - screenFont.getHeight()));
			}
		}
		catch (Exception e)
		{
			// なぜかヌルポが出るので...
		}

		return;
	}

	/**
	 * 画面を抜ける...
	 *
	 */
	private void exitScreen()
	{
		imageObject = null;
		parent.cancelPictureView();
		offsetX = 0;
		offsetY = 0;
		System.gc();
		return;
	}

	/**
	 * タイトル領域に描画する
	 * @param g
	 */
	private void drawTitleArea(Graphics g)
	{
		if (infoLine != null)
		{
			g.setColor(0);
			g.drawString(infoLine, screenTopX + 1, screenTopY, (Graphics.LEFT | Graphics.TOP));
		}
		return;
	}

	/**
	 * 情報領域に情報を設定する
	 * 
	 */
	public void setInformation(String message)
	{
		infoLine = message;
		busyMode = false;
		return;
	}	
	
	/**
	 * 情報領域の情報を更新する
	 * 
	 */
	public void updateInformation(String message, int mode)
	{
		infoLine = message;
		if (mode < 0)
		{
			busyMode = true;
		}
		else
		{
			busyMode = false;
		}
		return;		
	}
	/**
	 * メインパネル領域に描画する
	 * 
	 * @param g
	 * @param top
	 * @param height
	 */
	private void drawMainPanel(Graphics g, int topX, int topY, int width, int height)
	{
        // フォントサイズ
		int fontHeight = screenFont.getHeight();

		// 枠を表示する
		g.setColor(backColor);
		g.fillRect(topX, topY, width, height);

		// イメージの表示座標を決める
		int x = topX;
		if (imageObject.getWidth() < width)
		{
			x = topX + ((width  - imageObject.getWidth())  / 2);
		}

		int y = topY;
		if (imageObject.getHeight() < height)
		{
			y = y + ((height - imageObject.getHeight()) / 2);
		}

		// イメージを描画する
        g.setColor(foreColor);
		if (imageObject != null)
		{
		    g.drawImage(imageObject, x, y, (Graphics.LEFT | Graphics.TOP));
		}
		else
		{
            // イメージがないので ????? を表示する
            g.drawString("?????", (topX + screenFont.stringWidth("□")), (topY + fontHeight * screenLine / 2), (Graphics.LEFT | Graphics.TOP));			
		}
		return;
	}
}
