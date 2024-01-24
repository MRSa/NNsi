import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.images.PictureScreen;

public class PreviewScreen extends PictureScreen
{
    private final int backColor = 0x00ffffff;
    private final int foreColor = 0x00000000;

    private PreviewScreenOperator previewOperator = null;

	/**
     *   コンストラクタ
     * @param screenId
     * @param object
     * @param operator
     */
	public PreviewScreen(int screenId, MidpSceneSelector object, PreviewScreenOperator operator)
    {
    	super(screenId, object, operator);
    	previewOperator = operator;
    }

    /**
     * 画面描画処理
     * 
     * @param g グラフィックスクラス
     * 
     */
    public void paint(Graphics g)
    {
        try
        {
        	// 表示タイトルデータを設定する
        	setTitle("Preview");
        	
            // タイトル領域の描画
            drawTitleArea(g);

            // メインパネルの描画...
            drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (screenHeight - screenFont.getHeight()));

        }
        catch (Exception e)
        {
            // なぜかヌルポが出るので...
        }
        return;
    }

    /**
     * メインパネル領域に描画する
     * 
     * @param g    グラフィックキャンバス
     * @param topX 左上座標X
     * @param topY 左上座標Y
     * @param width 描画領域幅
     * @param height 描画領域高さ
     */
    private void drawMainPanel(Graphics g, int topX, int topY, int width, int height)
    {
    	Image imageObject = null;
    	try
    	{
            // フォントサイズ
            int fontHeight = screenFont.getHeight();

            // イメージデータを取得する
    		Image img = previewOperator.getImage(false);

    		// イメージデータのサイズを取得する
    		int imgHeight = img.getHeight();
    		int imgWidth = img.getWidth();
    		
            // イメージが大きい場合、イメージの中央部分だけを抽出する
    		int srcX = imgWidth - width;
    		if (srcX > 0)
    		{
    			srcX = srcX / 2;
    			imgWidth = width;
    		}
    		else
    		{
    			srcX = 0;
    		}
    		
    		int srcY = imgHeight - (height + fontHeight + 2);
    		if (srcY > 0)
    		{
    			srcY = srcY / 2;
    			imgHeight = height;
    		}
    		else
    		{
    			srcY = 0;
    		}
    		imageObject = Image.createImage(img, srcX, srcY, imgWidth, (imgHeight - fontHeight), Sprite.TRANS_NONE);

            // 枠を表示する
            g.setColor(backColor);
            g.fillRect(topX, topY, width, height);

            // イメージを描画する
            g.setColor(foreColor);
            if (imageObject != null)
            {
                g.drawImage(imageObject, topX, topY, (Graphics.LEFT | Graphics.TOP));
                g.drawString(previewOperator.getMessage(), topX + 3, topY + (height - fontHeight), (Graphics.LEFT | Graphics.TOP));
            }
            else
            {
                // イメージがないので ????? を表示する
                g.drawString("?????", (topX + screenFont.stringWidth("□")), (topY + fontHeight * screenLine / 2), (Graphics.LEFT | Graphics.TOP));
            }
    	}
    	catch (Exception ex)
    	{
            // イメージがないので ????? を表示する
            g.drawString("?????", (topX + screenFont.stringWidth("□")), (topY + screenFont.getHeight() * 2), (Graphics.LEFT | Graphics.TOP));
    	}
        return;
    }

	/**
     *  アプリケーションを終了させるか？
     * 
     *  @return  true : 終了させる、false : 終了させない
     */
    public boolean checkExit()
    {
        return (false);
    }

    /**
     *  Enterキー操作処理 : 画像なしで位置情報を記録する
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputFire(boolean isRepeat)
    {
    	try
    	{
    	    previewOperator.setSaveAction(true);
    	}
    	catch (Exception ex)
    	{
    		//
    	}
    	parent.changeScene(LocHuntSceneDB.DEFAULT_SCREEN, "");
        return (true);
    }

    /**
     *  Game C操作処理 : 表示のクリア
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	try
    	{
    	    previewOperator.setSaveAction(false);
    	}
    	catch (Exception ex)
    	{
    		//
    	}
    	parent.changeScene(LocHuntSceneDB.DEFAULT_SCREEN, "");
        return (true);
    }
}
