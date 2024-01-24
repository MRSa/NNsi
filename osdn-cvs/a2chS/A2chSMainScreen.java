import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;


/**
 * 「追記メモ」のメイン画面 
 *  @author MRSa
 *
 */
public class A2chSMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
	private A2chSSceneStorage storageDb = null;
	private A2chSListStorage listStorageDb = null;
	private boolean lockOperation = false;

    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public A2chSMainScreen(int screenId, MidpSceneSelector object, A2chSSceneStorage storage, A2chSListStorage listStorage)
    {
        super(screenId, object);        
        storageDb = storage;
        listStorageDb = listStorage;
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     * 
     */
    public void start()
    {
        // 記録するファイル名を取得する       
        String directory = storageDb.getDefaultDirectory();
        if (directory == null)
        {
        	storageDb.resetSelectionItems();
            parent.changeScene(A2chSSceneDB.WELCOME_SCREEN, "ようこそ！");
        }

        // ２ちゃん検索を実施する        
        if (storageDb.getUrlToSearch() != null)
        {
        	startFind2ch();
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
    	
        ////////// 操作方法の記入 //////////
    	try
    	{
        	int fontHeight = screenFont.getHeight() + 1; 
        	drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 5) - 5, screenWidth - 20, (fontHeight * 5));

            String msg = storageDb.getInformationMessage();
            if (msg != null)
            {
                // 情報メッセージを表示する
            	g.drawString(msg, (screenTopX + 10), (screenTopY + (fontHeight * 6)), (Graphics.LEFT | Graphics.TOP));
            }
        	
            ///// a2Bディレクトリを表示する /////
            g.setColor(foreColorForOpe);
            g.drawString("a2B格納ディレクトリ :", (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));
            g.drawString(storageDb.getDefaultDirectory(), (screenTopX + 15), (screenTopY + (fontHeight * 3)), (Graphics.LEFT | Graphics.TOP));
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
        g.drawString(": スレ選択",       topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Game D",           topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ブラウザで開く", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": 検索文字列入力", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("×",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": アプリ終了",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));

    }

    /**
     *  2ちゃんねる検索実行実処理
     *  <br>本来はコントローラの役目。。。
     *
     */
    private void startFind2ch()
    {
    	lockOperation = true;    // 構築終了まで操作を禁止
    	storageDb.setInformationMessage("2ch検索実行中...");
    	Thread thread = new Thread()
        {
            public void run()
            {
                // 検索（URLからデータ取得）実行
                storageDb.doFind2chThreads();
                lockOperation = false;
                storageDb.setInformationMessage(null);
                inputFire(false);  // 構築終了のトリガ
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
        return;
    }

    /**
     *  メニューボタンが押されたとき：検索文字列入力画面へ遷移する
     * 
     */
    public void showMenu()
    {
        if (lockOperation == true)
        {
            return;
        }
    	parent.changeScene(A2chSSceneDB.INPUT_SCREEN, "検索文字列入力");
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
    	if (lockOperation == true)
    	{
            // 既に中ボタンが押されていたときには何もしない
    		return (true);
    	}
    	lockOperation = true;
    	Thread thread = new Thread()
        {
            public void run()
            {
                // ファイルが存在するかどうか？
            	if (listStorageDb.readSearchResultFile() == true)
                {
                    // ファイルが存在する場合には、画面を検索結果一覧へ切り替える
                    parent.changeScene(A2chSSceneDB.LIST_SCREEN, "検索結果一覧");
                }
            	lockOperation = false;
            }
        };
        thread.start();
        return (true);
    }

    /**
     * GAME Dボタンが押されたとき：2ch検索結果ファイルをブラウザで開く
     * (アプリも終了させる)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        if (lockOperation == true)
        {
            return (true);
        }
        String fileName = storageDb.getDefaultDirectory() + "temporary.html";
        parent.platformRequest(fileName, true);
        return (true);
    }
}
