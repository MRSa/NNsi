import java.util.Vector;
import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;

/**
 *  グラフほかの表示画面！
 * @author MRSa
 *
 */
public class TheCountSummaryScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
//    private int backColorForOpe = 0x00f0f0ff;
	private TheCountDataStorage  dataStorage = null;

	private boolean isSorted = true;
	private boolean isOutput = false;
	
	private int item1Color = 0x00c00000;
	private int item2Color = 0x0000c000;
	private int item3Color = 0x000000c0;
	private int item4Color = 0x00c000c0;
	private int item5Color = 0x00c0c000;
	private int item6Color = 0x0000c0c0;
	
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public TheCountSummaryScreen(int screenId, MidpSceneSelector object, TheCountDataStorage itemStorage)
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
        	int fontHeight = screenFont.getHeight() + 1; 

        	///// 総件数を表示する /////
            g.setColor(foreColorForOpe);
            g.drawString("総件数 :" + dataStorage.getTotalValueCount(), (screenTopX + 5), (screenTopY + (fontHeight * 1)), (Graphics.LEFT | Graphics.TOP));

            // メッセージを出す
            String outData = "* : データ出力, CLR : 戻る";
            int fontWides = screenFont.stringWidth(outData);
            g.drawString(outData, screenTopX + screenWidth - fontWides - 5, (screenTopY + (fontHeight * 1)), (Graphics.LEFT | Graphics.TOP));

            // グラフを表示する
            int size = (screenHeight - (fontHeight * 7) - 2) - (screenTopY + (fontHeight * 2) + 5);
            drawGraph(g, ((screenWidth - size) / 2), (screenTopY + (fontHeight * 2)), size);

            // データ値の枠を表示する
        	g.setColor(0);
        	g.drawRect(screenTopX + 3, (screenHeight - (fontHeight * 7) - 4), screenWidth - screenTopX - 6, fontHeight * 7 + 4);

        	// 出力先ディレクトリを表示する
        	g.drawString(dataStorage.getBaseDirectory(), screenTopX + 5, (screenHeight - (fontHeight * 7) - 1), (Graphics.LEFT | Graphics.TOP));

            ///// データ値を表示する  /////
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 6) - 1), fontHeight, item1Color, 0);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 5) - 1), fontHeight, item2Color, 1);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 4) - 1), fontHeight, item3Color, 2);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 3) - 1), fontHeight, item4Color, 3);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 2) - 1), fontHeight, item5Color, 4);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 1) - 1), fontHeight, item6Color, 5);

    	}
    	catch (Exception ex)
    	{
    		//
    	}
        return;
    }

    /**
     *   グラフを表示する
     * @param g
     * @param topX
     * @param topY
     * @param wides
     */
    private void drawGraph(Graphics g, int topX, int topY, int wides)
    {
        int total = dataStorage.getTotalValueCount();

        int index = getItemIndex(0);
    	int value = dataStorage.getItemValue(index);
        double beginData = 0.0 + 90.0;
        double data = ((double) value / (double) total) * ((double) 360.0);
        g.setColor(item1Color);
    	g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(1);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
    	g.setColor(item2Color);
        g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(2);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
        g.setColor(item3Color);
    	g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(3);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
        g.setColor(item4Color);
        g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(4);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
        g.setColor(item5Color);
    	g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(5);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
    	g.setColor(item6Color);
        g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);
    }
    
    /**
     *   アイテムを表示する
     * @param g
     * @param topX
     * @param topY
     * @param itemIndex
     */
    private void drawItemData(Graphics g, int topX, int topY, int height, int foreColor, int index)
    {
    	int itemIndex = getItemIndex(index);
    	int value = dataStorage.getItemValue(itemIndex);
    	String percent = ": " + dataStorage.getValuePercent(itemIndex) + "%";
     	String valueMsg = "(" + value + ")";

   	    g.setColor(foreColor);
    	g.fillRect(topX + 10, topY, (height - 2), (height - 2));
        g.drawString(dataStorage.getItemName(itemIndex), topX + 10 + height, topY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(percent, topX + 110, topY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(valueMsg, topX + 170, topY, (Graphics.LEFT | Graphics.TOP));
    	return;
    }

    /**
     *   データを取得する
     * @param index
     * @return
     */
    private int getItemIndex(int index)
    {
        // 非ソートモードは、そのまま応答する
    	if (isSorted == false)
    	{
    		return (index);
    	}

    	int itemIndex = -1;
        try
        {
        	int value = 0;
        	Vector valueVector  = new Vector();
        	Vector itemVector   = new Vector();
        	for (int lp = 0; lp < 6; lp++)
        	{
                boolean isMatched = false;
        		value = dataStorage.getItemValue(lp);
        	    for (int vecIndex = 0; vecIndex < valueVector.size(); vecIndex++)
        	    {
        	    	int checkValue = ((Integer) valueVector.elementAt(vecIndex)).intValue();
        	    	if (checkValue < value)
        	    	{
        	    		valueVector.insertElementAt(new Integer(value), vecIndex);
        	    		itemVector.insertElementAt(new Integer(lp), vecIndex);
        	    		isMatched = true;
        	    		break;
        	    	}
        	    }
        	    if (isMatched == false)
        	    {
        	    	valueVector.addElement(new Integer(value));
        	    	itemVector.addElement(new Integer(lp));
        	    }
        	}
        	itemIndex = ((Integer) itemVector.elementAt(index)).intValue();
        }
        catch (Exception ex)
        {
        	itemIndex = -1;
        }
        return (itemIndex);
    }

    /**
     *  メニューボタンが押されたとき：アイテムデータの表示順番を変更する
     * 
     */
    public void showMenu()
    {
    	isSorted = (isSorted == true) ? false : true;
        System.gc();
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
     * その他のボタンが押されたとき：
     * 
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
    	return (false);
    }
    
    /**
     * 中ボタンが押されたとき： 何もしない
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
    	return (false);
    }

    /**
     * GAME Cボタンが押されたとき...メイン画面に戻る
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	if (isOutput == true)
    	{
    		return (false);
    	}
    	parent.changeScene(TheCountSceneDB.DEFAULT_SCREEN, "");
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

    /**
     *  ＊キー操作処理  : データファイル出力
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        Thread thread = new Thread()
        {
            public void run()
            {
            	dataStorage.outputDataToFile();
            }
        };
        try
        {
        	isOutput = true;
        	thread.start();
        	isOutput = false;
        }
        catch (Exception ex)
        {
        	//
        }
        return (true);
    }
}
