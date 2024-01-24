import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;

/**
 *  メイン画面 
 *  @author MRSa
 *
 */
public class A2BUProcessScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;

    private int foreColorForMsg = 0x00000000;
    private int backColorForMsg = 0x00f0f0f0;

    private BusyConfirmationStorage confirmStorage = null;
    private A2BUSceneStorage  sceneStorage = null;

    private boolean isSkipBBS    = false;
    private boolean isAbortBBS   = false;

    private int numberOfBBSes     = 0;
    private int numberOfFavorites = 0;

    private boolean logUpdating  = false;
    private int currentBBSindex    = 0;
    private int currentThreadindex = 0;

    private String getBoard = "";
    private String getTitle = "";
    
    
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public A2BUProcessScreen(int screenId, MidpSceneSelector object, BusyConfirmationStorage confirm, A2BUSceneStorage sceneDb)
    {
        super(screenId, object);
        confirmStorage = confirm;
        sceneStorage = sceneDb;
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     * 
     */
    public void start()
    {
        numberOfBBSes     = sceneStorage.getNumberOfBBS();
        numberOfFavorites = sceneStorage.getNumberOfFavorites();

        String msg = "BBS:" + numberOfBBSes + " Favor:" + numberOfFavorites;
        sceneStorage.setInformationMessage(msg);

        // ログ更新の開始...
        if (logUpdating == false)
        {
            startA2BLogUpdate();
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
        
        ////////// 画面の表示... //////////
        try
        {
            int fontHeight = screenFont.getHeight() + 1;
            drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 3) - 5, screenWidth - 20, (fontHeight * 3));

            // 取得状況を表示する。
            drawProgressInformation(g, screenTopX, (screenTopY + fontHeight + 5), screenWidth, (screenHeight - (fontHeight * 6) - 10));
 
            // 情報メッセージを表示する
            String msg = sceneStorage.getInformationMessage();
            if (msg != null)
            {
                // 情報メッセージを表示する
                g.drawString(msg, (screenTopX + 10), screenTopY + screenHeight - (fontHeight * 4) - 5, (Graphics.LEFT | Graphics.TOP));
            }             
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }
    /**
     *   画面に取得状況を表示する
     * 
     * @param g
     * @param topX
     * @param topY
     * @param width
     * @param height
     */
    private void drawProgressInformation(Graphics g, int topX, int topY, int width, int height)
    {
        // 枠を表示する
        g.setColor(backColorForMsg);
        g.fillRect(topX, topY, width, height);
        g.setColor(foreColorForMsg);
        g.drawRect(topX, topY, width, height);

        int aY = topY + 2;
        g.drawString("更新中 : " + getTitle, topX + 5, aY, (Graphics.LEFT | Graphics.TOP));

        aY = aY + screenFont.getHeight();
        g.drawString("(" + (sceneStorage.getCurrentThreadCount() + 1) + "件目" + ", " + getBoard + ")", topX + 5, aY, (Graphics.LEFT | Graphics.TOP));
        
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
        g.drawString("■■■ログ更新中操作■■■", topX + 5, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": 板取得スキップ",   topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("×",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ログ更新中断",   topX + 70, aY, (Graphics.LEFT | Graphics.TOP));

    }

    /**
     *  メニューボタンが押されたとき：板取得を中止し、次の板へスキップする
     * 
     */
    public void showMenu()
    {
        isSkipBBS = true;
        String msg = ">>>>>> 板Skip処理中... ";
        sceneStorage.setInformationMessage(msg);
        return;
    }

    /**
     *  アプリケーションを終了させるか？
     * 
     *  @return  true : 終了させる、false : 終了させない
     */
    public boolean checkExit()
    {
        isAbortBBS = true;
        String msg = ">>>>>> 中断中... ";
        sceneStorage.setInformationMessage(msg);
       
        if (logUpdating == false)
        {
            // 新着確認中断
            confirmStorage.showMessage("ログ更新を中断します。よろしいですか？", BusyConfirmationStorage.CONFIRMATION_MODE, A2BUSceneDB.DEFAULT_SCREEN);
        }
        return (false);
    }

    /**
     * 中ボタンが押されたとき：何もしない
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        // メッセージを表示する?
        sceneStorage.setInformationMessage("(・∀・)ｲｲ!!");
        return (true);
    }

    /**
     * GAME Dボタンが押されたとき：何もしない
     * (アプリも終了させない)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        return (false);
    }

    
    private void startA2BLogUpdate()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                // 実行の前処理...
                logUpdating = true;
                currentBBSindex    = 0;
                currentThreadindex = -1;

                sceneStorage.resetUpdateThreadCount();
                sceneStorage.prepare();

                // 板更新処理実行！
                updateA2BLogMain();

                return;
            }
        };
        try
        {
            thread.start();
        }
        catch (Exception ex)
        {
            //
        }
    }
    
    
    /**
     *  a2Bのログ更新実行
     *  
     *  @param bbsIndex 板index番号
     *  @param threadIndex スレ番号
     *
     */
    private void updateA2BLogMain()
    {
        String msg = "";
        int nofThreadsInBBS = sceneStorage.getNumberOfThreads();
        if ((currentThreadindex > 0)&&(currentThreadindex >= nofThreadsInBBS)&&(nofThreadsInBBS >= 0))
        {
            // 板の切替が発生、a2b.idx の更新を依頼する
            sceneStorage.backupA2BIndex();

            currentBBSindex++;
            currentThreadindex = -1;
        }
        if (currentBBSindex >= numberOfBBSes)
        {
            // 
            msg = "ログ更新処理が終了しました。" + "(" + sceneStorage.getUpdateThreadCount() + "/" + sceneStorage.getCurrentThreadCount()  + ")";
            sceneStorage.setInformationMessage(msg);

            // 全部の板を更新した、、、終了する...
            finishLogUpdating();
            return;
        }
        if ((currentThreadindex < 0)||(nofThreadsInBBS < 0))
        {
            // 新規の板読み込み、a2b.idx の読み込みを依頼する
            sceneStorage.prepareA2BIndex(currentBBSindex);

            currentThreadindex = -1;
            nofThreadsInBBS = sceneStorage.getNumberOfThreads();

            // subject.txtも同時に更新する？
            if (sceneStorage.isGetSubjectTxt() == true)
            {
                String subjectUrl = sceneStorage.getCurrentBBSUrl() + "/subject.txt";
                String subjectTxt = sceneStorage.getCurrentBBSNick() + "/subject.txt";
                getBoard = sceneStorage.getCurrentBBSNick();

                // メッセージを表示する
                msg = "更新中:" + subjectTxt;
                sceneStorage.setInformationMessage(msg);
                getTitle = null;
                getTitle = "板一覧 (" + subjectTxt + ")";
                sceneStorage.redraw(false);
                
            	// subject.txtを更新する
            	sceneStorage.getSubjectTxt(subjectUrl, sceneStorage.getDefaultDirectory(), subjectTxt);
            }            
            
            // 次のログ更新へ...
            sceneStorage.finishTrigger(currentThreadindex);
            return;
        }
        
        // 取得するスレの情報を取得する
        A2BUa2BsubjectDataParser.subjectDataHolder data = sceneStorage.getThreadData(currentThreadindex);
        if ((data == null)||(data.numberOfMessage > 1000))
        {
            // データ取得失敗 or 1000超え...次のログ更新へ...
            sceneStorage.finishTrigger(currentThreadindex);
            return;
        }
        
        // 取得先URLと更新するログファイル名を生成する
        getTitle = null;
        getTitle = new String(data.getTitleName());
        getBoard = sceneStorage.getCurrentBBSNick();
        String fileName = data.threadNumber + ".dat";
        String url = sceneStorage.getCurrentBBSUrl() + "/dat/" + fileName;
        String directory = sceneStorage.getDefaultDirectory() + sceneStorage.getCurrentBBSNick() + "/";

        // ログ更新メッセージを生成する...
        sceneStorage.setInformationMessage(url);

        // ログ更新の実処理を呼び出す
        sceneStorage.updateLogFile(url, directory, fileName, data);

        // 次のログ更新へ...
        sceneStorage.finishTrigger(currentThreadindex);
    }
    
    /**
     * 表示データの更新処理
     * 
     * @param processingId 処理ID
     * 
     */
    public void updateData(int processingId)
    {
        // 情報メッセージのクリア
        String msg = "";
        sceneStorage.setInformationMessage(msg);

        if (isAbortBBS == true)
        {
            // 
            msg = "ログ更新処理を中断しました。";
            sceneStorage.setInformationMessage(msg);

            // 更新処理の中断指示あり。
            abortLogUpdating();

            // ログ更新処理の終了
            finishLogUpdating();

            return;
        }

        if (isSkipBBS == true)
        {
            // 板更新処理のスキップ指示あり。

            // a2b.idx の更新を依頼する
            sceneStorage.backupA2BIndex();
            
            // 次の板の先頭へ...
            currentBBSindex++;
            currentThreadindex = -2; // 次にインクリメントするから...

            isSkipBBS = false;
        }
        currentThreadindex++;

        Thread thread = new Thread()
        {
            public void run()
            {
                // 板更新処理実行！
                updateA2BLogMain();
                return;
            }
        };
        try
        {
            thread.start();
//            thread.join();
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }
    
    /**
     *  ログ更新の中断処理
     *
     */
    private void abortLogUpdating()
    {
        // 中断時、やるべきことがあれば、ここに処理を追加する
        return;
    }
    
    /**
     *  ログ更新処理の終了処理
     *  (必ず呼ぶ！)
     *
     */
    private void finishLogUpdating()
    {
        sceneStorage.redraw(false);

        // お気に入り情報を更新する
        sceneStorage.writeFavoriteDb();
        
        // ログ更新中情報を落とす
        currentBBSindex    = 0;
        currentThreadindex = -1;
        isAbortBBS  = false;
        logUpdating = false;
        
        // バイブ実行する？
        if (sceneStorage.isDoVibrate() == true)
        {
            parent.vibrate(500);
        }
        
        // アプリを自動終了させる？
        if (sceneStorage.isAutoDestroyApplication() == true)
        {
        	// ５秒ぐらい待ってから終了する。
            String msg = "a2BU終了中...";
            sceneStorage.setInformationMessage(msg);
            parent.sleep(2000);
            parent.stop();
        }
        
        // 画面切替処理...
        parent.changeScene(A2BUSceneDB.DEFAULT_SCREEN, "");
    }
}
