import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;;

/**
 * 「dotE」のメイン画面 
 *  @author MRSa
 *
 */
public class DotEMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    //private int backColorForOpe = 0x00f0f0ff;
    
    private DotEDataStorage storageDb = null;
	private BusyConfirmationStorage confirmStorage = null;
    private DotESelectionMenu selectionStorage = null;
    private DotEFileAccessor fileAccessor = null;	
    private boolean isReady = false;
    private boolean redrawAll = true;
    private boolean changeColor = false;
    private boolean autoMode = false;

    private int      areaTopX = 0;
    private int      areaTopY = 0;
    private int      cursorX  = 0;
    private int      cursorY  = 0;
    private int      previousX = 0;
    private int      previousY = 0;
    private int      regionX   = 0;
    private int      regionY   = 0;
    
    private boolean isRegionSetting = false;
    private boolean copyRegionArea = false;
    private int      copyRegionX = -1;
    private int      copyRegionY = -1;
    private int      copyRegionWidth = -1;
    private int      copyRegionHeight = -1;
    
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public DotEMainScreen(int screenId, MidpSceneSelector object, DotEDataStorage storage, BusyConfirmationStorage confirm, DotESelectionMenu selectionMenu)
    {
        super(screenId, object);
        storageDb = storage;
        confirmStorage = confirm;
        selectionStorage = selectionMenu;
        fileAccessor = new DotEFileAccessor();
        fileAccessor.prepare(storage);
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     * 
     */
    public void start()
    {
    	if (isReady == false)
    	{
    		isReady = true;
    	}
    	redrawAll = true;

    	int sceneId = selectionStorage.getNextSceneId();
    	selectionStorage.resetSelectionItems();
    	switch (sceneId)
    	{
    	  case DotESceneDB.CONFIRMATION_LOAD:
    		if (confirmStorage.isAccepted() == true)
    		{
        	    // ファイルからロードする
    		    fileAccessor.loadToFile(storageDb.getBaseDirectory() + storageDb.getDataFileName());
    		}
            break;

    	  case DotESceneDB.CONFIRMATION_SAVE:
      		if (confirmStorage.isAccepted() == true)
      		{
        		// ファイルへ保存する
       		    fileAccessor.saveToFile(storageDb.getBaseDirectory() + storageDb.getDataFileName());
      		}
    		break;

    	  case DotESceneDB.COLORPICK_PIXEL:
        	if (confirmStorage.isAccepted() == true)
        	{
                // 色を拾う
        		storageDb.setPaintColor(storageDb.getData(cursorX, cursorY));
        	}
      		break;

    	  case DotESceneDB.CONFIRMATION_END:
    		if (confirmStorage.isAccepted() == true)
    		{
    			// アプリを終了させる
    			parent.stop();
    		}
    		break;

    	  case DotESceneDB.REGION_COPY:
    		if (confirmStorage.isAccepted() == true)
    		{
    			autoMode = false;
    			isRegionSetting = true;
    			copyRegionArea  = false;
    		}
    		break;

    	  case DotESceneDB.REGION_PASTE:
    		if (confirmStorage.isAccepted() == true)
    		{
    			if ((copyRegionX >= 0)&&(copyRegionY >= 0)&&(copyRegionArea == true))
    			{
    			    storageDb.pasteAreaData(cursorX, cursorY);
    			}
    		}
    	    break;

    	  case DotESceneDB.REGION_CLEAR:
    		if (confirmStorage.isAccepted() == true)
    		{
                // コピー領域をクリアする
    			clearCopyRegion();
    		}
            break;

    	  default:
    		//
            break;
    	}
        return;
    }

    /**
     * 画面を表示
     * 
     */
    public void paint(Graphics g)
    {
        if (g == null)
        {
            return;
        }        
        
        try
        {
            // 準備が整っていない場合には...何もしない
            if (isReady != true)
            {
            	return;
            }

            int fontHeight = screenFont.getHeight() + 1; 
            int margin = (fontHeight * 2);
            int areaY = storageDb.getSizeY();
            int areaX = storageDb.getSizeX();
            
            if (redrawAll == true)
            {
                // タイトルと画面全体のクリアは親（継承元）の機能を使う
                super.paint(g);
            	
            	///// ディレクトリ名およびファイル名を表示する /////
                g.setColor(foreColorForOpe);
                g.drawString(storageDb.getBaseDirectory() + storageDb.getDataFileName(), (screenTopX + 2), (screenTopY + (fontHeight * 1)), (Graphics.LEFT | Graphics.TOP));

                // 実寸データを書く
                areaTopX = screenTopX + 2;
                areaTopY = screenTopY + margin;
                g.setColor(0x00808080);
                g.drawRect(areaTopX, areaTopY, areaX + 1, areaY + 1);  // 枠線

                areaTopX++;
                areaTopY++;
                drawPointedData(g, areaTopX, areaTopY, areaX, areaY);

                // 枠の表示
                int width = screenWidth - 5;
                int height = screenHeight - (screenTopY + margin + 5 + fontHeight * 3);
                regionX = width / areaX;
                regionY = height / areaY;

                drawPaintCanvas(g, screenTopX + 1, (screenTopY  + margin + areaY + 4), (regionX * storageDb.getSizeX()), (regionY * storageDb.getSizeY()));  
                darawPaintEditData(g, screenTopX + 1, (screenTopY  + margin + areaY + 4), areaX, areaY);
            
                if (copyRegionArea == true)
                {
                	// コピペ領域の枠を描画する
            		drawPaintRegionArea(g, (screenTopX + 1) + copyRegionX * regionX, (screenTopY  + margin + areaY + 4) + copyRegionY * regionY, copyRegionWidth, copyRegionHeight);
                }
            }

            if ((redrawAll == true)||(changeColor == true))
            {
                // ペンの色を表示する
            	drawPenColor(g);
            }

            // オートモードのしるしを表示
            if (autoMode == true)
            {
                g.setColor(0x0000ff00);
            }
            else
            {
            	g.setColor(0x00ffffff);
            }
            g.fillRect(areaTopX + areaX + 5, areaTopY, 5, 5);
            
            // カーソル部分を描画する
            drawPaintCursor(g, screenTopX + 1, (screenTopY  + margin + areaY + 4));

            if (isRegionSetting == true)
            {
            	if ((copyRegionX >= 0)&&(copyRegionY >= 0))
            	{
            		// コピペ領域を設定中...
            		drawPaintRegionCursor(g, screenTopX + 1, (screenTopY  + margin + areaY + 4));
            	}
            }
        }
        catch (Exception ex)
        {
            //
        }
        redrawAll = false;
        changeColor = false;
        return;
    }

    /**
     *  ペン先の色を表示する
     * @param g
     */
    private void drawPenColor(Graphics g)
    {
    	int fontHeight = g.getFont().getHeight();
        int fontWidth = g.getFont().stringWidth("COLOR ");
        int colorTopX = screenWidth - fontWidth - 16;
        g.setColor(0);
    	g.drawString("COLOR", colorTopX, areaTopY, (Graphics.LEFT | Graphics.TOP));
        g.drawRect(colorTopX, areaTopY + fontHeight, 16, 16);
        int paintColor = storageDb.getPaintColor();
        g.setColor(paintColor);
    	g.fillRect(colorTopX + 1, areaTopY + fontHeight + 1, 15, 15);

    	g.setColor(0x00ffffff);
    	g.fillRect(colorTopX + 18, areaTopY + fontHeight + 1, 40, 16);
    	int redRange   = (paintColor & 0x00f00000) >> 20;
        int greenRange = (paintColor & 0x0000f000) >> 12;
    	int blueRange  = (paintColor & 0x000000f0) >>  4;
        g.setColor(0x00ff0000);
        g.fillRect(colorTopX + 18, areaTopY + fontHeight + 2, redRange * 2, 4);

        g.setColor(0x0000ff00);
        g.fillRect(colorTopX + 18, areaTopY + fontHeight + 7, greenRange * 2, 4);

        g.setColor(0x000000ff);
        g.fillRect(colorTopX + 18, areaTopY + fontHeight + 12, blueRange * 2, 4);

    }
    
    /**
     *  データを全部表示する
     * @param g
     * @param topX
     * @param topY
     * @param sizeX
     * @param sizeY
     */
    private void darawPaintEditData(Graphics g, int topX, int topY, int sizeX, int sizeY)
    {
    	for (int x = 0; x < sizeX; x++)
    	{
    		for (int y = 0; y < sizeY; y++)
    		{
    			g.setColor(storageDb.getData(x, y));
    			g.fillRect((topX + x * regionX) + 1, (topY + y * regionY) + 1, regionX - 1, regionY - 1);
    		}
    	}
    }

    /**
     *  実寸データを描画する
     * @param g
     * @param topX
     * @param topY
     * @param sizeX
     * @param sizeY
     */
    private void drawPointedData(Graphics g, int topX, int topY, int sizeX, int sizeY)
    {
    	for (int x = 0; x < sizeX; x++)
    	{
    		for (int y = 0; y < sizeY; y++)
    		{
    			int color = storageDb.getData(x, y);
    			g.setColor(color);
    			g.drawLine(topX + x, topY + y, topX + x, topY + y);
    		}
    	}
    }
    
    /**
     *   画面に枠線を引く
     * 
     * @param g
     * @param topX
     * @param topY
     * @param width
     * @param height
     */
    private void drawPaintCanvas(Graphics g, int topX, int topY, int width, int height)
    {
        // 縦線を表示
    	int check = 0;
        for (int x = 0; x < width; x = x + regionX)
        {
        	if ((check % 5) == 0)
        	{
                g.setColor(0x00303030);
                check = 0;
        	}
        	else
        	{
                g.setColor(0x00a0a0a0);        		
        	}
        	check++;
        	g.drawLine(topX + x, topY, topX + x, topY + height);
        }

        // 横線を表示
        check = 0;
        for (int y = 0; y < height; y = y + regionY)
        {
        	if ((check % 5) == 0)
        	{
                g.setColor(0x00303030);        		
        	}
        	else
        	{
                g.setColor(0x00a0a0a0);        		
        	}
        	check++;
        	g.drawLine(topX, topY + y, topX + width, topY + y);
        }

        // 最後に枠を表示する
        g.setColor(0);
        g.drawRect(topX, topY, width, height);
        
        return;
    }

    /**
     *   画面にカーソルを描く
     * 
     * @param g
     */
    private void drawPaintCursor(Graphics g, int topX, int topY)
    {
        // カーソルが前にある位置をペイントしなおす
        g.setColor(0x00ffffff);
        g.fillRect(topX + regionX * previousX + 1, topY + regionY * previousY + 1, regionX - 1, regionY - 1);

        g.setColor(storageDb.getData(previousX, previousY));
        g.drawLine(areaTopX + previousX, areaTopY + previousY, areaTopX + previousX, areaTopY + previousY);  // 実寸位置のもの
        g.fillRect(topX + regionX * previousX + 1, topY + regionY * previousY + 1, regionX - 1, regionY - 1);

    	// カーソル位置を描画する
        g.setColor(0x00ffffff);
        g.fillRect(topX + regionX * cursorX + 1, topY + regionY * cursorY + 1, regionX - 1, regionY - 1);

        int cursorColor = storageDb.getData(cursorX, cursorY);   
        g.setColor(cursorColor);
        g.drawLine(areaTopX + cursorX, areaTopY + cursorY, areaTopX + cursorX, areaTopY + cursorY);
        g.fillRect(topX + regionX * cursorX + 1, topY + regionY * cursorY + 1, regionX - 1, regionY - 1);
        cursorColor = ((~cursorColor) & 0x00ffffff);
        g.setColor(cursorColor);
        g.drawRect(topX + regionX * cursorX + 1, topY + regionY * cursorY + 1, (regionX - 2), (regionY - 2));
        return;
    }

    /**
     *   画面に領域指定の先頭カーソルを描く
     * 
     * @param g
     */
    private void drawPaintRegionCursor(Graphics g, int topX, int topY)
    {
        int cursorColor = storageDb.getData(copyRegionX, copyRegionY);   
        int leftX = topX + regionX * copyRegionX + 1;
        int leftY = topY + regionY * copyRegionY + 1;

        cursorColor = ((~cursorColor) & 0x00ffffff);
        g.setColor(cursorColor);
        g.drawLine(leftX, leftY, leftX + (regionX - 2), leftY + (regionY - 2));
        g.drawLine(leftX + (regionX - 2), leftY, leftX, leftY + (regionY - 2));
        return;
    }

    /**
     *   画面上にコピーした領域を示す
     * 
     * @param g
     */
    private void drawPaintRegionArea(Graphics g, int topX, int topY, int width, int height)
    {
    	g.setColor(0x00ffa000);
    	g.drawRect(topX, topY, width * regionX, height * regionY);
    	return;
    }

    /**
     *  メニューボタンが押されたとき
     * 
     */
    public void showMenu()
    {
    	selectionStorage.resetSelectionItems();
    	parent.changeScene(DotESceneDB.MAIN_MENU, null);
        return;
    }

    /**
     *  アプリケーションを終了させるか？
     * 
     *  @return  true : 終了させる、false : 終了させない
     */
    public boolean checkExit()
    {
		autoMode = (autoMode == false) ? true : false;
        return (false);
    }

    /**
     *  上キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputUp   (boolean isRepeat)
    {
    	if (cursorY == 0)
    	{
            return (false);
    	}
    	previousX = cursorX;
    	previousY = cursorY;
    	cursorY--;
    	
    	if (autoMode == true)
    	{
            // ---------- 色を塗る ----------
            storageDb.setData(cursorX, cursorY);    		
    	}
        return (true);
    }

    /**
     *  下キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputDown (boolean isRepeat)
    {
        if (cursorY >= (storageDb.getSizeY() - 1))
        {
        	return (false);
        }
    	previousX = cursorX;
    	previousY = cursorY;
        cursorY++;
    	if (autoMode == true)
    	{
            // ---------- 色を塗る ----------
            storageDb.setData(cursorX, cursorY);    		
    	}
        return (true);
    }

    /**
     *  左キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputLeft(boolean isRepeat)
    {
    	if (cursorX == 0)
    	{
            return (false);
    	}
    	previousX = cursorX;
    	previousY = cursorY;
    	cursorX--;
    	if (autoMode == true)
    	{
            // ---------- 色を塗る ----------
            storageDb.setData(cursorX, cursorY);    		
    	}
        return (true);
    }

    /**
     *  右キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputRight(boolean isRepeat)
    {
        if (cursorX >= (storageDb.getSizeX() - 1))
        {
        	return (false);
        }
    	previousX = cursorX;
    	previousY = cursorY;
        cursorX++;
    	if (autoMode == true)
    	{
            // ---------- 色を塗る ----------
            storageDb.setData(cursorX, cursorY);    		
    	}
        return (true);
    }

    /**
     * 中ボタンが押されたとき：色を塗る
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
    	if (isRepeat == true)
    	{
            // リピートの場合は何もしない。（チャタリング対策）
    		return (false);
    	}
    	
    	if (isRegionSetting == true)
    	{
    		// コピー領域設定中の場合...
    		if ((copyRegionX < 0)||(copyRegionY < 0))
    		{
                // 領域の先頭を記録する
    		    copyRegionX = cursorX;
    		    copyRegionY = cursorY;
    		}
    		else
    		{
    		    // 領域を決定する
    			setCopyRegion();
    			isRegionSetting = false;
    			copyRegionArea = true;
    		}
    		return (true);
    	}

    	// ---------- 色を塗る ----------
        storageDb.setData(cursorX, cursorY);
     	return (true);
    }

    /**
     *  コピー領域を設定する
     */
    public void setCopyRegion()
    {
		int changeX = -1;
		int changeY = -1;
		if (copyRegionX > cursorX)
		{
			changeX = copyRegionX;
			copyRegionX = cursorX;
		}
		else
		{
			changeX = cursorX;
		}
		if (copyRegionY > cursorY)
		{
			changeY = copyRegionY;
			copyRegionY = cursorY;
		}
		else
		{
			changeY = cursorY;
		}
        copyRegionWidth = changeX - copyRegionX + 1;
        copyRegionHeight = changeY - copyRegionY + 1;
		storageDb.copyAreaData(copyRegionX, copyRegionY, copyRegionWidth, copyRegionHeight);
		redrawAll = true;
		return;
    }

    /**
     *   コピー領域をクリアする
     */
    private void clearCopyRegion()
    {
		copyRegionX = -1;
		copyRegionWidth = -1;
		copyRegionY = -1;
		copyRegionHeight = -1;
		copyRegionArea = false;
		storageDb.clearCopyAreaData();
		redrawAll = true;
    }
    
    
    
    /**
     *  ＃キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputPound(boolean isRepeat)
    {
        // 色の変更...
    	storageDb.changeColor(DotEDataStorage.ASCEND);
    	changeColor = true;
        return (true);
    }

    /**
     *  ＊キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        // 色の変更...
    	storageDb.changeColor(DotEDataStorage.DESCEND);
    	changeColor = true;
        return (true);
    }

    /**
     *  数字キーが入力されたときの処理
     *  @param number 入力された数字
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     *
     */
    public boolean inputNumber(int number, boolean isRepeat)
    {
        return (false);
    }

    /**
     *  キー入力されたときの処理
     *  @param keyCode 入力したキーのコード
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
    	if (keyCode ==Canvas.KEY_NUM0)
    	{
	        storageDb.changeColor(DotEDataStorage.OTHER);
	        changeColor = true;
	        return (true);
    	}
        return (false);
    }

    /**
     * GAME Cボタンが押されたとき (色をクリアする)
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	if (isRegionSetting == true)
    	{
    		if ((copyRegionX < 0)||(copyRegionY < 0))
    		{
    			// 領域指定モードをクリアする
    		    return (true);	
    		}
    		// 領域設定をやり直す
    		copyRegionX = -1;
    		copyRegionY = -1;
            return (true);
    	}

        // ---------- 色を消す ----------
        storageDb.clearData(cursorX, cursorY);
    	return (true);
    }
    
    /**
     * GAME Dボタンが押されたとき
     *  (ペイント色を黒色に設定する or 指定領域をペーストする)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
    	// 領域指定モードのときは、ペーストする
    	if (copyRegionArea == true)
    	{
    		// 領域指定を行う
			if ((copyRegionX >= 0)&&(copyRegionY >= 0)&&(copyRegionArea == true))
			{
			    storageDb.pasteAreaData(cursorX, cursorY);
			}
			redrawAll = true;
			return (true);
    	}

    	// 黒色に設定する
    	storageDb.setPaintColor(0);
    	changeColor = true;
        return (true);
    }
}
