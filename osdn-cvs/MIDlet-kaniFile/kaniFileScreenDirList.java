import java.util.Date;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

/**
 * ディレクトリ一覧画面の表示
 * @author MRSa
 *
 */
public class kaniFileScreenDirList implements IkaniFileCanvasPaint
{
	private final int iconSize        = 12;
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
	private int startOffset   = 0;
	private int numberOfDatas = 0;

	private String currentDirectory = null;
	private String infoLine1  = null;
	private String infoLine2  = null;
	private String infoLine3  = null;

	Image folderIcon  = null;    // フォルダを示すアイコン
	Image fileIcon    = null;    // ファイルを示すアイコン
	Image selFileIcon = null;    // 選択中ファイルを示すアイコン
	
	/**
	 * コンストラクタ
	 * @param object
	 */
	kaniFileScreenDirList(kaniFileSceneSelector object)
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

		// アイコンの読み出し...
		try
		{
			folderIcon  = Image.createImage("/res/folder.png");
			fileIcon    = Image.createImage("/res/file.png");
			selFileIcon = Image.createImage("/res/file_sel.png");
		}
		catch (Exception e)
		{
			//
		}		

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
     * モードを応答する...
     */
    public int getMode()
    {
    	return (screenMode);
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
		if (screenMode == kaniFileCanvas.SCENE_DIRLIST)
		{
		    return (((height - (screenFont.getHeight() * 2)) / screenFont.getHeight()));
		}
		return (((height - (screenFont.getHeight() * 4)) / screenFont.getHeight()));
	}

	/**
	 *  画面切り替えメソッド...
	 * 
	 */
	public void changeScene()
	{
		parent.moveDirectory("");
		return;
	}
	
	/**
	 * 表示データの更新処理...
	 * 
	 */
	public void updateData()
	{
        String dir = currentDirectory;
        int   nofData = numberOfDatas;
		currentDirectory = parent.getCurrentDirectory();
        numberOfDatas    = parent.getNumberOfFiles();
        try
        {
    		if ((dir.compareTo(currentDirectory) != 0)||(nofData != numberOfDatas))
    		{
                // ディレクトリまたはファイル数が変更されていた場合には、カーソルを先頭に移動する
                startOffset      = 0;
                currentIndex     = 0;
    		}
        }
        catch (Exception e)
        {
            startOffset      = 0;
            currentIndex     = 0;        	
        }
		return;
	}

	/**
	 *  カーソルを３つ上に移動する
	 * 
	 */
	public boolean inputBack()
	{
		currentIndex = currentIndex - 3;
		if (currentIndex < startOffset)
		{
			currentIndex = (startOffset + screenLine - 1);
			if (currentIndex >= numberOfDatas)
			{
				currentIndex = numberOfDatas - 1;
			}
		}
/*
		// 一つ上の階層に上がる
        parent.upDirectory();
*/
		return (true);
	}

	/**
	 *   カーソルを３つ下に移動する
	 * 
	 */
	public boolean inputForward()
	{
		currentIndex = currentIndex + 3;
		if ((currentIndex >= (startOffset + screenLine))||(currentIndex >= (numberOfDatas)))
		{
			currentIndex = startOffset;
		}
/*
        // ディレクトリだったら、その下階層へ移動する
		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		if (fileInfo.isDirectory() != true)
		{
			return (false);
		}
		String fileName = fileInfo.getFileName();
		if (fileName.equals(".."))
		{
			return (false);
		}
		parent.moveDirectory(fileName);
*/
		return (true);
	}

	/**
	 *  カーソルを1つ上に移動する
	 * 
	 */
	public boolean inputPreviousItem()
	{
		currentIndex--;
		if (currentIndex < startOffset)
		{
			currentIndex = (startOffset + screenLine - 1);
			if (currentIndex >= numberOfDatas)
			{
				currentIndex = numberOfDatas - 1;
			}
		}
		return (true);
	}

	/**
	 *   カーソルを1つ下に移動する
	 * 
	 */
	public boolean inputNextItem()
	{
		currentIndex++;
		if ((currentIndex >= (startOffset + screenLine))||(currentIndex >= (numberOfDatas)))
		{
			currentIndex = startOffset;
		}
		return (true);
	}

	/**
	 *  前ページの表示
	 * 
	 */
	public boolean inputPreviousPage()
	{
		if (startOffset > 0)
		{
			startOffset  = startOffset - screenLine + 1;
			if (startOffset < (screenLine - 1))
			{
				startOffset = 0;
			}
			currentIndex = startOffset;
			return (true);
		}		
		return (true);
	}

	/**
	 *  次ページの表示
	 * 
	 */
	public boolean inputNextPage()
	{
		if ((startOffset + screenLine) < numberOfDatas)
		{
			startOffset = startOffset + screenLine - 1;
			currentIndex = startOffset;
			return (true);
		}
		return (true);
	}

	/**
	 *  選択された場合... (長押しは全選択)
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
        // 長押しされたとき...
		if (isRepeat == true)
		{
	        // 全選択を実施する
			parent.selectAllFiles(true);
			return (true);
		}
		
		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		String fileName = fileInfo.getFileName();
		if (fileInfo.isDirectory() == true)
		{
			// ディレクトリを選択した場合には、ディレクトリ移動を行う
			if (fileName.equals(".."))
			{
				parent.upDirectory();
			}
			else
			{
				parent.moveDirectory(fileName);
			}
			return (true);
		}

		// ファイルが選択されたときの処理...
		fileInfo.toggleSelected();
		parent.selectedFile(fileName, currentIndex, fileInfo.isSelected());
		return (true);
	}

	/**
	 *  選択キャンセル。。。
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		if (getMode() == kaniFileCanvas.SCENE_DIRLIST)
		{
			// ディレクトリ選択モードだった場合...選択をキャンセルする
			parent.cancelCommandDirInput();
			return (false);
		}

		// 長押しされたとき
		if (isRepeat == true)
		{
	        // 全ての選択を解除する
			parent.selectAllFiles(false);
			return (true);
		}

		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		String fileName = fileInfo.getFileName();
		if (fileInfo.isDirectory() == true)
		{
			return (true);
		}

		// ファイルが選択キャンセルされたときの処理...
		fileInfo.setSelected(false);
		parent.selectedFile(fileName, currentIndex, fileInfo.isSelected());
		return (true);
	}

	/**
	 *  GAME Dが押されたときの処理
	 * 
	 */
	public boolean inputOther(boolean isRepeat)
	{
		
		// ファイルを選択する。。。
		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		if (fileInfo.isDirectory() == true)
		{
			// ディレクトリを選択した場合には、何もしない
			return (true);
		}
		String fileName = (fileInfo.getFileName()).toLowerCase();
		parent.selectedFile(fileName, currentIndex, true);

		if (isRepeat == true)
        {
            // キーリピート時には、「ブラウザで開く」を選択したことにする
            parent.selectedOpenWebBrowser();
            return (true);
        }

        if ((fileName.indexOf(".htm") > 0)||(fileName.indexOf(".html") > 0)
             ||(fileName.indexOf(".txt") > 0)||(fileName.indexOf(".log") > 0)
             ||(fileName.indexOf(".idx") > 0)||(fileName.indexOf(".csv") > 0)
             ||(fileName.indexOf(".dat") > 0)||(fileName.indexOf(".jad") > 0)
        	 ||(fileName.indexOf(".ini") > 0)||(fileName.indexOf(".xml") > 0))
        {
            // テキスト編集モードで開く
        	parent.selectedEdit();
        	return (true);
        }
        // 画像表示モードで開く...
        parent.selectedPictureView();
		return (true);
	}

	/**
	 *  先頭にカーソルを移動させる
	 * 
	 */
	public boolean inputTop(boolean isRepeat)
	{
//		parent.updateFileDetail(currentIndex);
		startOffset = 0;
		currentIndex = startOffset;
		return (true);
	}

	/**
	 *  末尾にカーソルを移動させる
	 * 
	 */
	public boolean inputBottom(boolean isRepeat)
	{
//      parent.updateFileDetail(currentIndex);
		startOffset = numberOfDatas - (screenLine);
		if (startOffset < 0)
		{
            startOffset = 0;
		}
		currentIndex = numberOfDatas - 1;
		return (true);
	}

	/**
	 * 
	 */
	public boolean inputDummy()
	{
		if (getMode() == kaniFileCanvas.SCENE_DIRLIST)
		{
			// ディレクトリ選択モードだった場合...
			parent.selectedCopyDirectory();
			return (false);
		}
		
		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		String fileName = fileInfo.getFileName();
		if (parent.selectedFile(fileName, currentIndex) == true)
		{
			fileInfo.setSelected(true);
		}
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNumber(int number)
	{
		if (number == 0)
		{
            // 数字キーゼロでは、ファイル情報を表示する
			parent.updateFileDetail(currentIndex);
			return (false);
		}
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
	        int infoHeight = 0;
			if (screenMode != kaniFileCanvas.SCENE_DIRLIST)
			{
				infoHeight = screenHeight - screenFont.getHeight() * 3;
				drawMainPanel(g, screenTopX, screenTopY + screenFont.getHeight(), screenWidth, (infoHeight - (screenFont.getHeight())));

                // 情報領域の描画
			    g.setColor(0x00d8ffd8);
			    g.fillRect(screenTopX, infoHeight, screenWidth, screenFont.getHeight() * 3);
			    drawInformationArea(g, screenTopX, screenTopY + infoHeight);
			}
			else
			{
				infoHeight = screenHeight - screenFont.getHeight() * 1;
				drawMainPanel(g, screenTopX, screenTopY + screenFont.getHeight(), screenWidth, (infoHeight - (screenFont.getHeight())));

				// 情報領域の描画
			    g.setColor(0x00d8ffd8);
			    g.fillRect(screenTopX, screenTopY + infoHeight, screenWidth, screenFont.getHeight());
			    drawSingleInformationArea(g, screenTopX, screenTopY + infoHeight);
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
		if (currentDirectory != null)
		{
			g.setColor(0);
            int width = screenFont.stringWidth(currentDirectory);
            if (width > screenWidth)
            {
            	g.drawString("...", screenTopX, screenTopY, (Graphics.LEFT | Graphics.TOP));            	
				int offset = 0;
				int length = currentDirectory.length();
				while (screenFont.substringWidth(currentDirectory, offset, (length - offset)) > (screenWidth - 8))
				{
				    offset++;
				}
				g.drawSubstring(currentDirectory, offset, (length - offset), 8, screenTopY, (Graphics.LEFT | Graphics.TOP));
            }
            else
            {
            	g.drawString(currentDirectory, screenTopX, screenTopY, (Graphics.LEFT | Graphics.TOP));
            }
		}
		return;
	}

	/**
	 * タイトル領域に描画する
	 * @param g
	 */
	private void drawInformationArea(Graphics g, int topX, int topY)
	{
		g.setColor(0);
		if (infoLine1 != null)
		{
			g.drawString(infoLine1, topX, topY, (Graphics.LEFT | Graphics.TOP));
		}
		if (infoLine2 != null)
		{
			g.drawString(infoLine2, topX, topY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));
		}
		if (infoLine3 != null)
		{
			g.drawString(infoLine3, topX, topY + (screenFont.getHeight() * 2), (Graphics.LEFT | Graphics.TOP));
		}
		return;
	}

	/**
	 * タイトル領域に描画する
	 * @param g
	 */
	private void drawSingleInformationArea(Graphics g, int topX, int topY)
	{
		g.setColor(0);
		if (infoLine3 != null)
		{
			g.drawString(infoLine3, topX, topY, (Graphics.LEFT | Graphics.TOP));
		}
		return;
	}

	/**
	 * 情報領域に情報を設定する
	 * 
	 */
	public void setInformation(String message)
	{
		infoLine3 = message;
		return;
	}
	
	/**
	 * 情報領域の情報を更新する
	 * 
	 */
	public void updateInformation(String message, int mode)
	{
		infoLine3 = message;
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

		for (int loop = 0; loop < screenLine; loop++)
		{
			int index = loop + startOffset;
			g.setColor(foreColor);
			if (index >= numberOfDatas)
			{
				// 表示すべきデータ量を超えた...
				break;
			}
			long fileSize     = -1;
			long lastModified = -1;
			kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(index);
			Image icon = decideIcon(fileInfo);
			if (icon != null)
			{
				g.drawImage(icon, topX, (topY + (fontHeight * loop)), (Graphics.LEFT | Graphics.TOP));
			}
			fileSize = fileInfo.getFileSize();
			lastModified = fileInfo.getDateTime();
			String drawLine = fileInfo.getFileName();
			if (index == currentIndex)
			{
				g.setColor(cursorBackColor);
				g.fillRect(topX + iconSize, (topY + (fontHeight * loop)), width - iconSize, fontHeight);
				g.setColor(cursorForeColor);

				infoLine1 = "";
				if (lastModified > 0)
				{
					Date dateTime = new Date(lastModified);
					infoLine1 = dateTime.toString();
				}
				infoLine2 = "";
				if (fileSize >= 0)
				{
					infoLine2 = fileSize + " bytes";
				}

				if (fileInfo.isWritable() == false)
				{
					infoLine2 = infoLine2 + " [READ ONLY]";
				}

				if (fileInfo.isHidden() == true)
				{
					infoLine2 = infoLine2 + " [HIDDEN]";
				}
			}
			g.drawString(drawLine, topX + iconSize + 1, (topY + (fontHeight * loop)), (Graphics.LEFT | Graphics.TOP));			
		}
		if (startOffset > 0)
		{
			g.setColor(0x0030b030);
			g.drawString("▲", topX + width - iconSize, (topY + 1), (Graphics.LEFT | Graphics.TOP));			
		}
		if ((startOffset + screenLine) < numberOfDatas)
		{
			g.setColor(0x0030b030);
			g.drawString("▼", topX + width - iconSize, (topY + height - fontHeight) -1, (Graphics.LEFT | Graphics.TOP));			
		}

		return;
	}	
	
	/**
	 * 表示するアイコンを特定する
	 * @param info
	 * @return
	 */
	private Image decideIcon(kaniFileDirList.fileInformation info)
	{
		if (info.isDirectory() == true)
		{
			return (folderIcon);
		}
		if (info.isSelected() == true)
		{
			return (selFileIcon);
		}
		return (fileIcon);
	}
}
