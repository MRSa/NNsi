import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;;

/**
 * 「a2BU」のメイン画面 
 *  @author MRSa
 *
 */
public class A2BUMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
    
    private A2BUDataStorage storageDb = null;
    private BusyConfirmationStorage confirmStorage = null;
    private A2BUSceneStorage  sceneStorage = null;

    private boolean isReady = false;
    
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public A2BUMainScreen(int screenId, MidpSceneSelector object, A2BUDataStorage storage, BusyConfirmationStorage confirm, A2BUSceneStorage sceneDb)
    {
        super(screenId, object);
        storageDb = storage;
        sceneStorage = sceneDb;
        confirmStorage = confirm;
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     * 
     */
    public void start()
    {
        // a2Bのログ記録ディレクトリを取得する       
        String directory = storageDb.getBaseDirectory();
        if (directory == null)
        {
            parent.changeScene(A2BUSceneDB.WELCOME_SCREEN, "ようこそ！");
        }
        else
        {
            // 解析する...
            if (isReady == false)
            {
                isReady = true;
                sceneStorage.prepareBoardUrl();
                sceneStorage.prepareFavoriteDb(directory);
            }
            int bbsCnt = sceneStorage.getNumberOfBBS();
            int favorCnt = sceneStorage.getNumberOfFavorites();
            int upCnt   = sceneStorage.getUpdateThreadCount();
            int curCnt  = sceneStorage.getCurrentThreadCount();
            int errCnt  = sceneStorage.getErrorThreadCount();
            String msg = "板：" + bbsCnt + " お気に：" + favorCnt;
            if (curCnt > 0)
            {
                msg = msg + "  更新:" + upCnt + "/" + curCnt + "(Err:" + errCnt + ")";
            }
            storageDb.setInformationMessage(msg);
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
        
        // タイトルと画面全体のクリアは親（継承元）の機能を使う
        super.paint(g);
        
         try
        {
            int fontHeight = screenFont.getHeight() + 1; 

            // 準備が整っているかどうか？
            if (isReady == true)
            {
                ////////// 操作方法の表示 //////////
                drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 5) - 5, screenWidth - 20, (fontHeight * 5));
            }
            else
            {
/*
                // 準備中を表示する
                String infoMsg = "a2Bのログ状態を解析中。";  // (" + procCount + "%)";
                g.setColor(foreColorForOpe);
                g.drawString(infoMsg,        (screenTopX + 5), (screenTopY + screenHeight - (fontHeight * 2) - 4), (Graphics.LEFT | Graphics.TOP));
                g.drawString("（しばらくお待ちください。）",   (screenTopX + 5), (screenTopY + screenHeight - (fontHeight * 1) - 2), (Graphics.LEFT | Graphics.TOP));
*/
            }

            ////////// 情報の表示 //////////
            String msg = storageDb.getInformationMessage();
            if (msg != null)
            {
                
            	g.drawString(msg, (screenTopX + 10), (screenTopY + (fontHeight * 6)), (Graphics.LEFT | Graphics.TOP));
            }
            
            ///// a2Bディレクトリを表示する /////
            g.setColor(foreColorForOpe);
            g.drawString("a2Bログ格納Dir :", (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));
            g.drawString(storageDb.getBaseDirectory(), (screenTopX + 15), (screenTopY + (fontHeight * 3)), (Graphics.LEFT | Graphics.TOP));
        }
        catch (Exception ex)
        {
            //
        }
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
        g.drawString("Enter",            topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ログ更新開始",   topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Game D",           topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": a2BログDir指定", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": 設定項目変更",   topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
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
        if (isReady == false)
        {
            return;
        }
        
//        parent.changeScene(A2BUSceneDB.MAIN_MENU, "メインメニュー");
        parent.changeScene(A2BUSceneDB.EDITPREF_FORM, "a2BU設定");
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
     * 中ボタンが押されたとき：スレ一覧画面へ遷移する
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        if (isReady == false)
        {
            return (true);
        }
        
        // ログ更新
        confirmStorage.showMessage("ログ更新を開始します。よろしいですか？", BusyConfirmationStorage.CONFIRMATION_MODE, A2BUSceneDB.PROCESS_SCREEN);
        return (true);
    }

    public boolean inputGameC(boolean isRepeat)
    {
        return (false);
    }
    
    /**
     * GAME Dボタンが押されたとき：ベースディレクトリの指定画面へ遷移
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        if (isReady == false)
        {
            return (true);
        }
        parent.changeScene(A2BUSceneDB.DIR_SCREEN, "a2BログDir選択");
        return (true);
    }
}
