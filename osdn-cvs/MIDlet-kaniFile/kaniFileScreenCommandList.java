import java.util.Vector;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * コマンド選択画面の表示...
 * @author MRSa
 *
 */
public class kaniFileScreenCommandList implements IkaniFileCanvasPaint
{
//	static public final int selectionList    = 13;
	static public final int selectionList    = 7;
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

	private int currentIndex  = 0;
	private String infoLine   = null;
	private Vector commandList = null;

	/**
	 * コンストラクタ
	 * @param object
	 */
	kaniFileScreenCommandList(kaniFileSceneSelector object)
	{
		//
		parent = object;
	}

	/**
	 * 準備処理...
	 */
	public void prepare(Font font, int mode)
	{
		screenFont   = font;
		screenMode   = mode;
		
		if (mode == kaniFileCanvas.SCENE_FILEOPEMODE)
		{
			prepareCommandListFileOpeMode();
		}
		else
		{
			prepareCommandListTextEdit();
		}
		return;
	}

	/**
	 *  通常用のコマンド一覧の準備...
	 */
	private void prepareCommandListFileOpeMode()
	{
		commandList  = new Vector(selectionList);
		commandList.addElement("ブラウザで開く");
		commandList.addElement("テキスト編集");
		commandList.addElement("画像表示");
		commandList.addElement("ファイル一覧出力");
		commandList.addElement("Dir作成");
		commandList.addElement("リネーム");
		commandList.addElement("属性変更");
		commandList.addElement("Snd再生/停止");
		commandList.addElement("ZIP展開");
		commandList.addElement("ZIP展開(同Dir.)");
		commandList.addElement("複製");
		commandList.addElement("コピー");
		commandList.addElement("削除");
		commandList.addElement("終了");
		return;
	}

	/**
	 *  テキスト編集用のコマンド一覧の準備...
	 *
	 */
	private void prepareCommandListTextEdit()
	{
		commandList  = new Vector(selectionList);
		commandList.addElement("保存");
		commandList.addElement("次行挿入");
		commandList.addElement("1行挿入");
		commandList.addElement("1行貼り付け");
		commandList.addElement("1行削除");
		commandList.addElement("ブラウザで開く");
		commandList.addElement("ブラウザで開く(Google経由)");
		commandList.addElement("URLを取得");
/**
		commandList.addElement("ジャンプ");
		commandList.addElement("検索");
		commandList.addElement("前検索");
		commandList.addElement("次検索");
**/
		return;
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
		currentIndex = 0;
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
		currentIndex = currentIndex - screenLine;
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
	public boolean inputNextPage()
	{
		currentIndex = currentIndex + screenLine;
		if (currentIndex >= commandList.size())
		{
			currentIndex = 0;
		}
	    return (true);
	}

	/**
	 *  コマンドを選択
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
		boolean ret = false;
		if (screenMode == kaniFileCanvas.SCENE_FILEOPEMODE)
		{
			ret = inputSelectedFileOpeMode(isRepeat);
		}
		else
		{
			ret = inputSelectedTextEdit(isRepeat);
		}
		return (ret);
	}
	
	/**
	 *  コマンドを選択 (ファイル操作モード)
	 * 
	 */
	private boolean inputSelectedFileOpeMode(boolean isRepeat)
	{
        boolean ret = false;
		switch (currentIndex)
		{
          case 0:
          	ret = parent.selectedOpenWebBrowser();
          	break;

          case 1:
            ret = parent.selectedEdit();
        	break;
	
          case 2:
          	ret = parent.selectedPictureView();
          	break;

          case 3:
        	ret = parent.selectedOutputFileList();
        	break;

          case 4:
        	ret = parent.selectedMkDir();
        	break;

          case 5:
        	ret = parent.selectedRename();
        	break;

          case 6:
          	ret = parent.selectedEditAttribute();
          	break;

          case 7:
            ret = parent.selectedPlayStopSound();
            break;

          case 8:
            ret = parent.selectedExtractZip(false);
            break;

          case 9:
            ret = parent.selectedExtractZip(true);
            break;

          case 10:
            ret = parent.selectedDuplicate();
            break;

          case 11:
          	ret = parent.selectedCopy();
          	break;

          case 12:
            ret = parent.selectedDelete();
            break;

          case 13:
            parent.finishApp();
            ret = true;
            break;

          default:
        	break;
		}
		return (ret);
	}

	/**
	 *  コマンドを選択
	 * 
	 */
	private boolean inputSelectedTextEdit(boolean isRepeat)
	{
		switch (currentIndex)
		{
          case 0:
        	// 保存
        	parent.saveTextEditFile();
           	break;

          case 1:
        	// 次行挿入
          	parent.insertLine(true);
          	break;
  	
          case 2:
        	// 1行挿入
            parent.insertLine(false);
            break;

          case 3:
        	// 1行貼り付け
            parent.pasteLine();
          	break;

          case 4:
        	// 1行削除
        	parent.deleteLine();
        	break;

          case 5:
        	// ブラウザで開く
        	parent.openUrl(false);
        	break;

          case 6:
          	// ブラウザで開く(Google Proxy利用)
          	parent.openUrl(true);
          	break;

          case 7:
        	// URLを取得する
        	parent.getFileFromUrl();
        	break;

          default:
        	break;
		}

		// コマンドモードを抜ける
		parent.cancelCommandInputTextEdit();
		return (false);
	}

	/**
	 *  選択キャンセル。。。コマンドモードを抜ける
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		if (screenMode == kaniFileCanvas.SCENE_FILEOPEMODE)
		{
			parent.cancelCommandInput();
		}
		else
		{
			parent.cancelCommandInputTextEdit();
		}
		currentIndex  = 0;
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
		int fontHeight = screenFont.getHeight();
		int startTopX  = topX;
		int maxItem    = commandList.size();
		
		g.setColor(backColor);
		g.fillRect(topX, topY, width, height);

		int index = 0;
		for (int item = 0; item < maxItem; item++)
		{
            if (index == screenLine)
            {
                // 2行目にコマンドを表示する
            	startTopX = (width + topX) / 2;
            	index = 0;
            }

            String drawLine = (String) commandList.elementAt(item);
			if (item == currentIndex)
			{
				g.setColor(cursorBackColor);
				g.fillRect(startTopX, (topY + (fontHeight * index)), ((width + topX) / 2), fontHeight);
				g.setColor(cursorForeColor);
			}
			else
			{
				g.setColor(backColor);
				g.fillRect(startTopX, (topY + (fontHeight * index)), ((width + topX) / 2), fontHeight);
				g.setColor(foreColor);
			}
			g.drawString(drawLine, startTopX + 1, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));
			index++;
		}
		g.setColor(foreColor);
		return;
	}
}
