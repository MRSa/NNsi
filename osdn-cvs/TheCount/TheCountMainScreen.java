import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;


/**
 * 「TheCount」のメイン画面 
 *  @author MRSa
 *
 */
public class TheCountMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
	private TheCountDataStorage  dataStorage = null;

    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public TheCountMainScreen(int screenId, MidpSceneSelector object, TheCountDataStorage itemStorage)
    {
        super(screenId, object);
        dataStorage = itemStorage;
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     * 
     */
    public void start()
    {
        // 記録するファイル名を取得する       
        String directory = dataStorage.getBaseDirectory();
        if (directory == null)
        {
            parent.changeScene(TheCountSceneDB.WELCOME_SCREEN, "ようこそ！");
        }
        System.gc();
        return;
    }

    /**
     * 画面を描画する
     * 
     */
    public void paint(Graphics g)
    {
    	if (g == null)
    	{
            return;
    	}    	
    	
        // タイトルと画面全体のクリアは親（継承元）の機能を使う
    	super.paint(g);
    	
        ////////// 画面の描画 //////////
    	try
    	{
    		// 操作方法を表示する
        	int fontHeight = screenFont.getHeight() + 1; 
        	drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 6) - 5, screenWidth - 20, (fontHeight * 6));

        	// 情報メッセージを表示する
            String msg = dataStorage.getInformationMessage();
            if ((msg != null)&&(msg.length() > 0))
            {
                // 情報メッセージを表示する
            	g.drawString(msg, (screenTopX + 4), (screenTopY + (fontHeight * 1)), (Graphics.LEFT | Graphics.TOP));
            	g.drawString(msg, (screenTopX + 5), (screenTopY + (fontHeight * 1) + 1), (Graphics.LEFT | Graphics.TOP));
            }
        	
            ///// 総件数を表示する /////
            g.setColor(foreColorForOpe);
            g.drawString("総件数 :" + dataStorage.getTotalValueCount(), (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));

            ///// データ値を表示する  /////
            for (int index = 0; index < 6; index++)
            {
                drawItemData(g, (screenTopX + 5), (screenTopY + (fontHeight * (3 + index))), foreColorForOpe, index);
            }
    	}
    	catch (Exception ex)
    	{
    		//
    	}
        return;
    }

    /**
     *   アイテムを表示する
     * @param g
     * @param topX
     * @param topY
     * @param itemIndex
     */
    private void drawItemData(Graphics g, int topX, int topY, int foreColor, int itemIndex)
    {
    	int value = dataStorage.getItemValue(itemIndex);
    	String percent = dataStorage.getValuePercent(itemIndex) + "%";
    	String msg = ": " + value;
    	g.setColor(foreColor);
        g.drawString("[" + (itemIndex + 1) + "] " + dataStorage.getItemName(itemIndex), topX + 5, topY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(msg, topX + 120, topY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(percent, topX + 180, topY, (Graphics.LEFT | Graphics.TOP));
    	return;
    }
    
    /**
     *   画面に操作ガイドを表示する
     * 
     * @param g
     * @param topX
     * @param topY
     * @param width
     * @param height
     */
    private void drawOperationGuide(Graphics g, int topX, int topY, int width, int height)
    {
    	// 枠を表示する
        g.setColor(backColorForOpe);
        g.fillRect(topX, topY, width, height);

        g.setColor(foreColorForOpe);
        g.drawRect(topX, topY, width, height);

        // 文字列を表示する
        int aY = topY + 2;
        g.drawString("■■■操作方法■■■", topX + 5, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight() + 2;
        g.drawString("1 - 6",            topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": カウントアップ", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("0",                topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": リセット",       topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Enter",            topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": グラフ表示",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": アイテム名設定", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("×",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": アプリ終了",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));

    }

    /**
     *  メニューボタンが押されたとき：検索文字列入力画面へ遷移する
     * 
     */
    public void showMenu()
    {
    	parent.changeScene(TheCountSceneDB.INPUT_SCREEN, "アイテム名設定");
        return;
    }

    /**
     *  アプリケーションを終了させるか？
     * 
     *  @return  true : 終了させる、false : 終了させない
     */
    public boolean checkExit()
    {
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
    	if (isRepeat == true)
    	{
    		// リピート検出時には何もしない。
    		return (false);
    	}
    	boolean ret = true;
    	switch (number)
    	{
    	  case 1:
    		// カウンタインクリメント！
      		dataStorage.addItemValue(0, 1);
      		break;
    	  case 2:
      		// カウンタインクリメント！
        	dataStorage.addItemValue(1, 1);
      		break;
    	  case 3:
      		// カウンタインクリメント！
        	dataStorage.addItemValue(2, 1);
      		break;
    	  case 4:
      		// カウンタインクリメント！
        	dataStorage.addItemValue(3, 1);
      		break;
    	  case 5:
      		// カウンタインクリメント！
        	dataStorage.addItemValue(4, 1);
      		break;
    	  case 6:
      		// カウンタインクリメント！
        	dataStorage.addItemValue(5, 1);
      		break;
    	  
    	  case 0:
    		// カウンタリセット！
    		dataStorage.setItemValue(0, 0);
    		dataStorage.setItemValue(1, 0);
    		dataStorage.setItemValue(2, 0);
    		dataStorage.setItemValue(3, 0);
    		dataStorage.setItemValue(4, 0);
    		dataStorage.setItemValue(5, 0);
      		break;

    	  default:
    		ret = false;
    		break;
    	}
    	return (ret);
    }

    /**
     * その他のボタンが押されたとき：
     * 
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
    	return (false);
    }
    
    /**
     * 中ボタンが押されたとき： グラフ表示...
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
    	parent.changeScene(TheCountSceneDB.SUMMARY_SCREEN, "");
    	return (true);
    }

    /**
     * GAME Cボタンが押されたとき
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	dataStorage.setInformationMessage("");
    	return (true);
    }

    /**
     * GAME Dボタンが押されたとき
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
    	return (false);
    }
}
