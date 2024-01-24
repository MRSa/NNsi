import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import java.util.Date;

/**
 * 「locHunt」のメイン画面 
 *  @author MRSa
 *
 */
public class LocHuntMainScreen extends MidpScreenCanvas
{
    private CameraDeviceController cameraDevice = null;
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
    private int foreColorForMessage = 0x00ff0000;
    private LocHuntSceneStorage storageDb = null;
    private String currentFileName = "";

    private int  dataLength = -1;
    private boolean performSaving = false;
    
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public LocHuntMainScreen(int screenId, MidpSceneSelector object, LocHuntSceneStorage storage, CameraDeviceController camera)
    {
        super(screenId, object);        
        storageDb = storage;
        cameraDevice = camera;
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
            parent.changeScene(LocHuntSceneDB.WELCOME_SCREEN, "ようこそ！");
        }
        storageDb.setStartLocation(false);

        // 画像を保存する？
        if (storageDb.getSaveAction() == true)
        {
            // データを保存する
        	storageDb.setSaveAction(false);
        	executeSaveAction();
        }
        return;
    }
    /**
     *   データの保存を実行する
     *
     */
    private void executeSaveAction()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                try
                {
                    Date currentDate = new Date();
                    currentFileName = currentDate.getTime() + "";
                    if (dataLength == 0)
                    {
                        // 取得した画像データを保存する
                        storageDb.outputImageData(currentFileName, null);
                    }
                    else if (dataLength > 0)
                    {
                        // 取得した画像データを保存する
                        storageDb.outputImageData(currentFileName, cameraDevice.getInputStream());                    	
                    }
                }
                catch (Exception ex)
                {
                	currentFileName = "";
                	foreColorForMessage = 0x007f7f00;
                    storageDb.setInformationMessage("保存失敗" + ex.getMessage());
                }
            }
        };
        try
        {
        	performSaving = true;
        	foreColorForMessage = 0x00ff0000;
        	storageDb.setInformationMessage("保存中...お待ちください。");
        	thread.start();
        	thread.join();
        	performSaving = false;
        	foreColorForMessage = 0x000000ff;
            storageDb.setInformationMessage("出力終了");
            System.gc();
        }
        catch (Exception ex)
        {
            
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
            drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 7) - 5, screenWidth - 20, (fontHeight * 7));

            String msg = storageDb.getInformationMessage();
            if (msg != null)
            {
                // 情報メッセージを表示する
            	g.setColor(foreColorForMessage);
                g.drawString(msg, (screenTopX + 10), (screenTopY + (fontHeight * 6)), (Graphics.LEFT | Graphics.TOP));
                g.drawString(msg, (screenTopX + 11), (screenTopY + (fontHeight * 6) + 1), (Graphics.LEFT | Graphics.TOP));
            }
            
            ///// ファイル格納ディレクトリを表示する /////
            g.setColor(foreColorForOpe);
            g.drawString("ファイル格納ディレクトリ :", (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));
            g.drawString(storageDb.getDefaultDirectory(), (screenTopX + 15), (screenTopY + (fontHeight * 3)), (Graphics.LEFT | Graphics.TOP));

            ///// データ長さを表示する  /////
            if (currentFileName.length() != 0)
            {
            	msg = currentFileName;
            	if (dataLength > 0)
            	{
                    msg = msg + ".jpg" + " (" + dataLength + " bytes)";
            	}
            	else
            	{
            		msg = msg + ".html";
            	}
            }
            g.drawString(msg, (screenTopX + 20), (screenTopY + (fontHeight * 7)), (Graphics.LEFT | Graphics.TOP));
            storageDb.setStartLocation(false);        
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
        g.drawString(": カメラ起動",     topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("#",                topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": 位置のみ記録",   topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Game D or *",      topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ブラウザで開く", topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": 撮影メモ設定",   topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("CLR",              topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": 表示クリア",     topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("×",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": アプリ終了",     topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
    }

    /**
     *  メニューボタンが押されたとき：パラメータ設定画面へ遷移する
     * 
     */
    public void showMenu()
    {
        storageDb.setStartLocation(false);
    	parent.changeScene(LocHuntSceneDB.INPUT_SCREEN, "パラメータ設定");
        return;
    }

    /**
     *  アプリケーションを終了させるか？
     * 
     *  @return  true : 終了させる、false : 終了させない
     */
    public boolean checkExit()
    {
    	if (performSaving == true)
    	{
    		// 画像保存中のときは、何もしない
    		return (false);
    	}
        return (true);
    }

    /**
     * 中ボタンが押されたとき：カメラを起動してイメージデータを保存する
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        // 位置の記憶
    	storageDb.setStartLocation(false);

    	if (performSaving == true)
    	{
    		// 画像保存中のときは、何もしない
    		return (false);
    	}
    	
        // カメラを起動し、画像をキャプチャする
    	foreColorForMessage = 0x00000000;
        storageDb.setInformationMessage("画像撮影...");
        boolean ret = cameraDevice.doCapture();
        storageDb.setInformationMessage("");
        if (ret != true)
        {
        	currentFileName = "";
        	foreColorForMessage = 0x007f7f00;
            storageDb.setInformationMessage("(未撮影)");
            return (false);
        }

        // プレビュー画面を表示する
        dataLength = cameraDevice.getDataLength();
    	parent.changeScene(LocHuntSceneDB.PREVIEW_SCREEN, "");

    	return (true);
    }

    /**
     *  キー入力されたときの処理
     *  @param keyCode 入力したキーのコード
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
        storageDb.setStartLocation(false);
    	if (performSaving == true)
    	{
    		// 画像保存中のときは、何もしない
    		return (false);
    	}
    	if (keyCode ==Canvas.KEY_NUM0)
    	{
    		// ゼロキーが押されたとき...ブラウザで開く...
    		parent.platformRequest(storageDb.getGoogleMapStaticMapUrl(240, 240, true), true);
	        return (true);
    	}
    	if (isRepeat == true)
    	{
    		return (false);
    	}
    	return (storageDb.setLocation(keyCode));
    }
    	
    /**
     *  Game C操作処理 : 表示のクリア
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	if (performSaving == true)
    	{
    		// 画像保存中のときは、何もしない
    		return (false);
    	}
    	currentFileName = "";
    	foreColorForMessage = 0x00000000;
    	storageDb.setInformationMessage("");
        storageDb.setStartLocation(true);
        storageDb.clearLocation();
        return (true);
    }

    /**
     * GAME Dボタンが押されたとき：ブラウザで開く
     * (アプリも終了させる)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
    	if (performSaving == true)
    	{
    		// 画像保存中のときは、何もしない
    		return (false);
    	}

    	// ファイル名を作り上げる
        String fileName = storageDb.getDefaultDirectory() + currentFileName + ".html";

        // ターゲットのファイル名を設定する
        String targetFileName;
        
        // 端末チェック...
        String platForm = System.getProperty("microedition.platform");
        if (platForm.indexOf("WX320K") >= 0)
        {
            // WX320Kの時には、ディレクトリを補正する。。。
            targetFileName = "file://localhost/D:/" + fileName.substring(8);
        }
        else if ((platForm.indexOf("WS023T") >= 0)||(platForm.indexOf("WS018KE") >= 0)||(platForm.indexOf("WS009KE") >= 0))
        {
            // WS023T or WS018KEの時には、ディレクトリを補正する。。。
            targetFileName = "file:///affm/nand0/PS/data/" + fileName.substring(19); // "file:///DataFolder/" を置換
        }
        else if (platForm.indexOf("WX34") >= 0)
        {
            // WX340K or WX341Kの時には、ディレクトリを補正する。。。
        	if (fileName.indexOf("file:///data/") >= 0)
        	{
        		// データフォルダの場合...
        		targetFileName = "file:///D:/" + fileName.substring(8); //   "file:///" を置換
        	}
        	else
        	{
        	    // SDカードの場合...
        		targetFileName = "file:///F:/" + fileName.substring(12); //  "file:///SD:/" を置換
        	}
        }
        else
        {
            targetFileName = fileName;
        }

        // 指定されたファイルをブラウザで開く
        parent.platformRequest(targetFileName, true);
        return (true);
    }

    /**
     *  ＃キー操作処理 : 画像なしで位置情報を記録する
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputPound(boolean isRepeat)
    {
        storageDb.setStartLocation(false);
    	if (performSaving == true)
    	{
    		// 画像保存中のときは、何もしない
    		return (false);
    	}

    	// 位置データだけを保存する
    	dataLength = 0;
    	storageDb.setSaveAction(false);
    	executeSaveAction();

        return (true);
    }
    
    /**
     *  ＊キー操作処理 : ブラウザで開く
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        return (inputGameD(isRepeat));
    }
    
}
