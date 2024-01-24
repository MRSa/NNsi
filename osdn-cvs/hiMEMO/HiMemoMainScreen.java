import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.framework.core.*;
import jp.sourceforge.nnsi.a2b.framework.base.*;

/**
 * 「追記メモ」のメイン画面 
 *  @author MRSa
 *
 */
public class HiMemoMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
	private HiMemoSceneStorage storageDb = null;
    
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public HiMemoMainScreen(int screenId, MidpSceneSelector object, HiMemoSceneStorage storage)
    {
        super(screenId, object);        
        storageDb = storage;
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     * 
     */
    public void start()
    {
        // 記録するファイル名を取得する       
        String fileName = storageDb.getMemoFileName();
        if (fileName == null)
        {
        	storageDb.resetSelectionItems();
            parent.changeScene(HiMemoSceneDB.WELCOME_SCREEN, "ようこそ！");
        }
        return;
    }

    /**
     * 画面を表示
     * 
     */
    public void paint(Graphics g)
    {
        // タイトルと画面全体のクリアは親（継承元）の機能を使う
    	super.paint(g);
    	
        ////////// 操作方法の記入 //////////
    	int fontHeight = screenFont.getHeight() + 1; 
    	drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 5) - 5, screenWidth - 20, (fontHeight * 5));

        ///// 現在時刻を表示する /////
/*
    	g.setColor(foreColorForOpe);
    	Date date = new Date();
        g.drawString("現在時刻 : ", screenTopX + 5, (screenTopY + (fontHeight * 5)), (Graphics.LEFT | Graphics.TOP));
        g.drawString(date.toString(), screenTopX + 15, (screenTopY + (fontHeight * 6)), (Graphics.LEFT | Graphics.TOP));
*/

        ///// 記録ファイル名を表示する /////
        g.setColor(foreColorForOpe);
        g.drawString("記録ファイル名 :", (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));
        g.drawString(storageDb.getMemoFileName(), (screenTopX + 15), (screenTopY + (fontHeight * 3)), (Graphics.LEFT | Graphics.TOP));

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
        g.drawString("Enter",            topX + 15, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": メモ入力",       topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Game D",           topX + 15, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ブラウザで開く", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 15, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": (機能なし)",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("×",               topX + 15, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": アプリ終了",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
   
    }
    
    
    /**
     * 中ボタンが押されたとき：データ入力画面へ遷移する
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        parent.changeScene(HiMemoSceneDB.INPUT_SCREEN, "データ入力");
        return (true);
    }

    /**
     * GAME Dボタンが押されたとき：メモデータをブラウザで開く
     * (アプリも終了させる)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        String fileName = storageDb.getMemoFileName();
        parent.platformRequest(fileName, true);
        return (true);
    }
}
