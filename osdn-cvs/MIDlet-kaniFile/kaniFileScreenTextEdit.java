import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * テキストデータ編集表示クラス
 * 
 * @author MRSa
 *
 */
public class kaniFileScreenTextEdit implements IkaniFileCanvasPaint
{
	private final int backColor       = 0x00ffffff;
	private final int foreColor       = 0x00000000;
	private final int cursorBackColor = 0x000000f8;
	private final int cursorForeColor = 0x00ffffff;
	
	private final int SKIP_LENGTH     = 50;
//	private final int SKIP_LENGTH     = 18;

	private kaniFileSceneSelector parent   = null;
	private kaniFileTextFile      textData = null;
	private Font screenFont = null;   // 表示に使用するフォント
	private int screenMode   = -1;    // 表示モード
	private int screenTopX   = 0;     // 描画領域の左端(X)
	private int screenTopY   = 0;     // 描画領域の左端(Y)
	private int screenWidth  = 160;   // 画面幅
	private int screenHeight = 160;   // 画面高さ
	private int screenLine   = 0;     // データ表示可能ライン数

	private int screenStartWidth = 0; // 表示先頭ライン
	
	private boolean isDrawAll = false;
	
	private String infoLine   = null;

	/**
	 * コンストラクタ
	 * @param object
	 */
	kaniFileScreenTextEdit(kaniFileSceneSelector object)
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
		textData = parent.getTextFileData();
		isDrawAll = true;
		return;
	}
	
	/**
	 * 表示データの更新処理...
	 * 
	 */
	public void updateData()
	{
		textData.setCurrentTopLine(0);
		textData.setCurrentSelectedLine(0);
		isDrawAll = true;
		return;
	}

	/**
	 *  左移動...
	 * 
	 */
	public boolean inputPreviousPage()
	{
		screenStartWidth = screenStartWidth - (screenWidth - SKIP_LENGTH);
		if (screenStartWidth < 0)
		{
			screenStartWidth = 0;
		}
		isDrawAll = true;
		return (true);
	}

	/**
	 * 右移動...
	 * 
	 */
	public boolean inputNextPage()
	{
		screenStartWidth = screenStartWidth + (screenWidth - SKIP_LENGTH);
		isDrawAll = true;
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputPreviousItem()
	{
        int selLine = textData.getCurrentSelectedLine();
		int topLine = textData.getCurrentTopLine();
		
		if (selLine == 0)
		{
			return (false);
		}
		selLine--;
		isDrawAll = false;
		if (selLine < topLine)
		{
			topLine = topLine - (screenLine - 1);
			if (topLine < 0)
			{
				topLine = 0;
			}
			textData.setCurrentTopLine(topLine);
			isDrawAll = true;
		}
		textData.setCurrentSelectedLine(selLine);
		
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNextItem()
	{
        int selLine = textData.getCurrentSelectedLine();
		int topLine = textData.getCurrentTopLine();
        int btmLine = textData.getTotalLine();
        
        if (selLine == (btmLine - 1))
        {
        	return (false);
        }
		selLine++;
		isDrawAll = false;
		if (selLine > (topLine + (screenLine - 1)))
		{
			topLine = topLine + (screenLine - 1);
			textData.setCurrentTopLine(topLine);
			isDrawAll = true;
		}
		textData.setCurrentSelectedLine(selLine);
		return (true);
	}

	/**
	 *  前ページの表示
	 * 
	 */
	public boolean inputBack()
	{
        int selLine = textData.getCurrentSelectedLine();
		int topLine = textData.getCurrentTopLine();
		
		if (selLine == 0)
		{
			return (false);
		}
		topLine = topLine - screenLine - 1;
		if (topLine < 0)
		{
			topLine = 0;
		}
		textData.setCurrentTopLine(topLine);
		textData.setCurrentSelectedLine(topLine);

		isDrawAll = true;
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputForward()
	{
		int topLine = textData.getCurrentTopLine();
        int btmLine = textData.getTotalLine();
        
		topLine = topLine + screenLine - 1;
		if (topLine >= (btmLine - 1))
		{
			topLine = btmLine - screenLine - 2;
		}
		textData.setCurrentTopLine(topLine);
		textData.setCurrentSelectedLine(topLine);

		isDrawAll = true;
		return (true);
	}

	/**
	 *  コマンドを選択
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
        boolean ret = false;
        
        // 編集モードへ...        
        parent.selectedTextEditLine();
        return (ret);
	}

	/**
	 *  選択キャンセル。。。コマンドモードを抜ける
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		parent.exitTextEdit();
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputOther(boolean isRepeat)
	{
		if (isRepeat == true)
		{
            // キーリピート時には、「ブラウザで開く」を選択したことにする
            parent.selectedOpenWebBrowser();
            return (true);
		}
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputTop(boolean isRepeat)
	{
		textData.setCurrentTopLine(0);
		textData.setCurrentSelectedLine(0);
		isDrawAll = true;
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputBottom(boolean isRepeat)
	{
        int btmLine = textData.getTotalLine();
		textData.setCurrentTopLine(btmLine - screenLine);
		textData.setCurrentSelectedLine(btmLine - 1);
		isDrawAll = true;
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
	 *  編集したファイルの保存
	 * 
	 */
	public boolean inputDummy()
	{
	    // 編集コマンドモードに切り替える
		parent.selectedEditTextCommand();
		return (true);
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

			// メインパネルの描画...
		    int infoTop = screenHeight;
			drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (infoTop - screenFont.getHeight()));
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
			String appendData = "[" + (textData.getCurrentSelectedLine() + 1) + ":" + textData.getTotalLine() + "] ";
			if (textData.isModified() == true)
			{
				appendData = "*" + appendData;
			}
			else
			{
				appendData = " " + appendData;
			}
			g.setColor(0);
			g.drawString(appendData + infoLine, screenTopX, screenTopY, (Graphics.LEFT | Graphics.TOP));
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
		return;
	}
	
	/**
	 * 情報領域の情報を更新する
	 * 
	 */
	public void updateInformation(String message, int mode)
	{
		infoLine = message;
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
		int fontHeight   = screenFont.getHeight();
		int dataLine     = textData.getTotalLine();
		int currentIndex = textData.getCurrentSelectedLine();
        int offset       = textData.getCurrentTopLine();

        g.setColor(backColor);
        if (isDrawAll == true)
        {
            g.fillRect(topX, topY, width, height);
        }

		for (int index = 0; index <  + screenLine; index++)
		{
			g.setColor(foreColor);
			if (index >= dataLine)
			{
				// 表示すべきデータ量を超えた...
				break;
			}
			
			if ((isDrawAll == false)&&((index < currentIndex - offset - 1)||(index > currentIndex - offset + 1)))
			{
				continue;
			}

			String drawLine = new String(textData.getLine(index + offset));
			if (index == currentIndex - offset)
			{
				g.setColor(cursorBackColor);
				g.fillRect(topX, (topY + (fontHeight * index)), width, fontHeight);
				g.setColor(cursorForeColor);
			}
			else
			{
				g.setColor(backColor);
				g.fillRect(topX, (topY + (fontHeight * index)), width, fontHeight);
				g.setColor(foreColor);				
			}
			if (screenStartWidth > 0)
			{
		        int contWidth = screenFont.charWidth('$');
				int len       = drawLine.length();
				int offsetW   = 1;
				g.drawString("$", topX, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));
				while ((offsetW < len)&&(screenFont.substringWidth(drawLine, 0, offsetW) < screenStartWidth))
				{
				    offsetW = offsetW + 4;
				}
				if (len > offsetW)
				{
                    g.drawSubstring(drawLine, offsetW, (len - offsetW), topX + contWidth + 1, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));
				}
			}
			else
			{
				g.drawString(drawLine, topX, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));
			}
		}
		isDrawAll = false;
		return;
	}
}
