import java.util.Vector;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * コマンド実行画面の表示...
 * @author MRSa
 *
 */
public class kaniFileScreenConfirmation implements IkaniFileCanvasPaint
{
	private final int backColor       = 0x00ffffff;
	private final int foreColor       = 0x00000000;
	private final int cursorBackColor = 0x000000f8;
	private final int cursorForeColor = 0x00ffffff;

	private kaniFileSceneSelector parent = null;
	private Font screenFont = null;   // 表示に使用するフォント
	private int screenMode   = -1;    // 表示モード
	private int screenTopX   = 0;     // 描画領域の左端(X)
	private int screenTopY   = 0;     // 描画領域の左端(Y)
	private int screenWidth  = 160;   // 画面幅
	private int screenHeight = 160;   // 画面高さ
	private int screenLine   = 0;     // データ表示可能ライン数

	private boolean busyMode = false;
	private int currentIndex  = 1;
	private String infoLine   = null;
	private Vector commandList = null;

	/**
	 * コンストラクタ
	 * @param object
	 */
	kaniFileScreenConfirmation(kaniFileSceneSelector object)
	{
		//
		parent = object;
	}

	/**
	 * 準備処理...
	 */
	public void prepare(Font font, int mode)
	{
		//
		screenFont   = font;
		screenMode   = mode;
		commandList  = new Vector(2);
	
		commandList.addElement("実行");
		commandList.addElement("中止");
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
		parent.memoryCurrentDirectory();
		return;
	}
	
	/**
	 * 表示データの更新処理...
	 * 
	 */
	public void updateData()
	{
		currentIndex     = 0;
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
	 * 
	 * 
	 */
	public boolean inputPreviousItem()
	{
		currentIndex--;
		if (currentIndex < 0)
		{
			currentIndex = commandList.size() - 1;
		}
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNextItem()
	{
		currentIndex++;
		if (currentIndex >= commandList.size())
		{
			currentIndex = 0;
		}
		return (true);
	}

	/**
	 *  前ページの表示
	 * 
	 */
	public boolean inputPreviousPage()
	{
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNextPage()
	{
		return (false);
	}

	/**
	 *  コマンドを選択
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
        boolean ret = false;
		switch (currentIndex)
		{
          case 0:
        	//
            ret = parent.confirmation();
        	break;

          case 1:
        	// 実行キャンセル
      		parent.cancelCommandInput();
        	break;
 		}
		return (ret);
	}

	/**
	 *  選択キャンセル。。。コマンドモードを抜ける
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		parent.cancelCommandInput();
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputOther(boolean isRepeat)
	{
		return (true);
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
		return (true);
	}

	/**
	 * 
	 */
	public boolean inputDummy()
	{
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
	 * タイトル領域に描画する
	 * @param g
	 */
	private void drawTitleArea(Graphics g)
	{
		if (infoLine != null)
		{
			g.setColor(0);
			g.drawString(infoLine, screenTopX, screenTopY, (Graphics.LEFT | Graphics.TOP));
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
		int fontHeight = screenFont.getHeight();
		
		g.setColor(backColor);
		g.fillRect(topX, topY, width, height);

		for (int index = 0; index < screenLine; index++)
		{
			g.setColor(foreColor);
			if (index >= commandList.size())
			{
				// 表示すべきデータ量を超えた...
				break;
			}
			String drawLine = (String) commandList.elementAt(index);
			if (index == currentIndex)
			{
				g.setColor(cursorBackColor);
				g.fillRect(topX, (topY + (fontHeight * index)), width, fontHeight);
				g.setColor(cursorForeColor);
			}
			g.drawString(drawLine, topX, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));			
		}
		return;
	}
}
