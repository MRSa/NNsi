import java.util.Date;
import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.forms.IDataInputStorage;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.IInformationScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.IFileSelectionStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpA2BhttpCommunication;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileHttpCommunication;

public class A2BUSceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{

    /***** 揮発データ(ここから) *****/
    private ChoiceGroup  preferences        = null;
    private TextField    gettingPauseTime   = null;
    private String       currentBbsUrl      = "";
    private String       currentBbsNick     = "";
    private String       informationMessage = null;
    private int          itemSelectionIndex = -1;
    private int          errorThreadCount = 0;
    private int          updateThreadCount = 0;
    private int          currentThreadCount = 0;
    private int          threadIndexNumber = 0;
    /***** 揮発データ(ここまで) *****/

    /***** その他アクセスクラス *****/
    private MidpCanvas                baseCanvas = null;
    private MidpResourceFileUtils resUtils    = null;
    private A2BUa2BbbsCommunicator bbsManager = null;
    private A2BUa2BFavoriteManager favoriteDb = null;
    private MidpFileDirList       fileDirList = null;
    private A2BUa2BsubjectDataParser subjectDb = null;
    
    private A2BUDataStorage storageDb = null;
    
    
    private MidpA2BUhttpCommunicator httpCommController = null;
    private MidpA2BhttpCommunication httpCommunicator = null;
    
    /**
     *  コンストラクタ
     *
     *  @param dataStorage データ記録クラス
     *  @param resourceUtils リソースアクセスクラス
     *  @param fileUtils ファイルアクセスクラス
     */
    public A2BUSceneStorage(A2BUDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpCanvas dataProcess, MidpFileDirList dirList)
    {
        storageDb        = dataStorage;
        resUtils         = resourceUtils;
        baseCanvas       = dataProcess;
        fileDirList      = dirList;
        
        subjectDb = new A2BUa2BsubjectDataParser(this);
        httpCommController = new MidpA2BUhttpCommunicator(true, this);
        httpCommunicator = new MidpA2BhttpCommunication(httpCommController, null);
    }

    /**------------------------------------------------------------**
     **
     **  ここから保管データの読み書き...
     **
     **
     **------------------------------------------------------------**/
    /**
     * ディレクトリの応答
     * @return ディレクトリ名
     */
    public String getDefaultDirectory()
    {
        return (storageDb.getBaseDirectory());
    }

    /**
     *  板URLを解析する...
     * @param fileName 解析するファイル名
     */
    public void prepareBoardUrl()
    {
        String dir = storageDb.getBaseDirectory();
        bbsManager = new A2BUa2BbbsCommunicator(dir);

        // 板一覧を解析する...
        bbsManager.prepare();

        // ログディレクトリ一覧を取得する
        if (fileDirList.prepare(dir) == false)
        {
            return;
        }

        int limit = fileDirList.getCount();
        int nofBoard = 0;
        for (int index = 0; index < limit; index++)
        {
             MidpFileDirList.fileInformation fileInfo = fileDirList.getFileInfo(index);
             if (fileInfo.isDirectory() == true)
             {
                 String dirName = fileInfo.getFileName();
                 if (dirName.indexOf(".") != 0)
                 {
                     if (bbsManager.markUsingBbs(dirName) == true)
                     {
                         nofBoard++;
                 
                     }
                 }
             }
        }
        bbsManager.trimBbsList();
        
        System.gc();
        return;
    }

    /**
     *  選択されている（ファイル）数を応答する
     *  
     */
    public  int getSelectedNameCount()
    {
        if (storageDb.getBaseDirectory() == null)
        {
            return (0);
        }
        return (1);
    }

    /**
     *  ディレクトリの応答
     * @return ディレクトリ名
     */
    public String getSelectedName(int index)
    {
        return (storageDb.getBaseDirectory());
    }
    
    /**
     *   ディレクトリの応答
     * @param ディレクトリ名
     */
    public void setSelectedName(String name)
    {
        storageDb.setBaseDirectory(name);
    }

    /**
     *  選択中のディレクトリ名をクリアする
     * 
     */
    public void clearSelectedName()
    {
        storageDb.setBaseDirectory(null);
        return;
    }
    
    /**
     *  設定したディレクトリ名を確定する
     *  (何もしない)
     * 
     */
    public void confirmSelectedName()
    {
        return;
    }
    
    
    /**
     *  お気に入り情報を読み込む
     * 
     * @param directory
     */
    public int prepareFavoriteDb(String directory)
    {
        favoriteDb = new A2BUa2BFavoriteManager(directory);
        favoriteDb.prepare();
        favoriteDb.restore();
        return (favoriteDb.getNumberOfFavorites());
    }

    /**
     *  お気に入り情報数を応答する
     * @return
     */
    public int getNumberOfFavorites()
    {
        return (favoriteDb.getNumberOfFavorites());
    }
    
    /**
     *  お気に入り情報を吐き出す
     *
     */    
    public void writeFavoriteDb()
    {
        favoriteDb.backup();
    }
    
    /**
     *   お気に入りに登録されているか？
     * @param nick
     * @param threadNumber
     * @return
     */
    public int isExistFavorite(String nick, long threadNumber)
    {
        int retCode = -1;
        try
        {
            if (favoriteDb != null)
            {
                retCode = favoriteDb.exist(nick, threadNumber);
            }
        }
        catch (Exception e)
        {
            retCode = -1;
        }
        return (retCode);
    }

    /**
     *  お気に入り情報を取得する
     * @param index お気に入り情報のインデックス番号
     * @return お気に入り情報
     */
    public A2BUa2BFavoriteManager.a2BFavoriteThread getFavoriteThreadData(int index)
    {
        A2BUa2BFavoriteManager.a2BFavoriteThread threadData = null;
        try
        {
            if (favoriteDb != null)
            {
                threadData = favoriteDb.getThreadData(index);
            }
        }
        catch (Exception e)
        {
            threadData = null;
        }
        return (threadData);        
    }

    /**------------------------------------------------------------**
     **
     **  ここから揮発データの読み書き...
     **
     **
     **------------------------------------------------------------**/
    /**
     * 表示するメッセージを応答する
     * 
     * @return
     */
    public String getInformationMessageToShow()
    {
        String message = "";
        try
        {
            message = resUtils.getResourceText("/res/welcome.txt");
        }
        catch (Exception ex)
        {
            //
        }
        return (message);
    }

    /**
     *   ボタン数を応答する
     *   
     */
    public int getNumberOfButtons(int screenId)
    {
        return (1);
    }

    /**
     * メッセージが受け付けられたかどうか設定する
     * @param buttonId 押されたボタンのID
     */
    public void setButtonId(int buttonId)
    {
        // ボタンが押された！！（画面遷移！）
        
    }

    /**
     *  情報表示画面のボタンラベルを応答
     * 
     */
    public String getInformationButtonLabel(int buttonId)
    {
        return("Dir選択");
    }

    /**
     *  データ入力がキャンセルされたとき
     *  
     */
    public void cancelEntry()
    {
        preferences      = null;
        gettingPauseTime = null;
        System.gc();
        return;
    }

    /**
     *  データ入力がＯＫされたとき
     *  
     */
    public void dataEntry()
    {
        int preferenceData = 0;
        if (preferences.isSelected(1) == true)
        {
            preferenceData = preferenceData + 1;
        }
        if (preferences.isSelected(2) == true)
        {
            preferenceData = preferenceData + 2;
        }
        if (preferences.isSelected(0) == true)
        {
            preferenceData = preferenceData + 4;
        }
        if (preferences.isSelected(3) == true)
        {
            preferenceData = preferenceData + 8;
        }
        storageDb.setPreference(preferenceData);        
        storageDb.setWaitTime(Integer.valueOf(gettingPauseTime.getString()).intValue());
        
        preferences    = null;
        gettingPauseTime = null;
        System.gc();
        return;
    }

    /**
     *  終了時、バイブを動作させる？
     * @return
     */
    public boolean isDoVibrate()
    {
        boolean ret = false;
        int setData = storageDb.getPreference();
        if ((setData & 1) != 0)
        {
            ret = true;
        }
        return (ret);
    }

    /**
     *  終了時、自動的にアプリを終了させる？
     * @return
     */
    public boolean isAutoDestroyApplication()
    {
        boolean ret = false;
        int setData = storageDb.getPreference();
        if ((setData & 2) != 0)
        {
            ret = true;
        }
        return (ret);
    }

    /**
     *  subject.txtも更新する？
     * @return
     */
    public boolean isGetSubjectTxt()
    {
        boolean ret = false;
        int setData = storageDb.getPreference();
        if ((setData & 4) != 0)
        {
            ret = true;
        }
        return (ret);
    }

    /**
     *  WX310系のGzip転送対策を実施する？
     * @return
     */
    public boolean isWorkaroundGzip()
    {
        boolean ret = false;
        int setData = storageDb.getPreference();
        if ((setData & 8) != 0)
        {
            ret = true;
        }
        return (ret);    	
    }
    
    /**
     *  データ入力のための準備
     *  
     */
    public void prepare()
    {
        httpCommController.SetUserAgent("Monazilla/1.00  a2BU/1.00");
    }

    /**
     *  次画面の切り替え
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == A2BUSceneDB.WELCOME_SCREEN)
        {
            return (A2BUSceneDB.DIR_SCREEN);
        }
        else if (screenId == A2BUSceneDB.DIR_SCREEN)
        {
            return (A2BUSceneDB.DEFAULT_SCREEN);
        }
        return (IDataInputStorage.SCENE_TO_PREVIOUS);
    }

    /**
     *  次画面の切り替え
     * 
     */
    public String nextSceneTitleToChange(int screenId)
    {
        return ("");
    }

    /**
     *  データ入力終了
     *  
     */
    public void finish()
    {
        // 何もしない
    }

    /**
     *  データ入力フィールドの個数を取得する
     *  
     */
    public int getNumberOfDataInputField()
    {
        return (1);
    }

    /**
     *  データ入力フィールドを取得する
     *  
     */
    public TextField getDataInputField(int index)
    {
    	gettingPauseTime = null;
    	String waitTime = "" + storageDb.getWaitTime();
    	gettingPauseTime = new TextField("巡回待ち時間(ms)", waitTime, 6, TextField.DECIMAL);
        return (gettingPauseTime);
    }

    /**
     *  オプションの個数を取得する
     *  
     */
    public int getNumberOfOptionGroup()
    {
        return (1);
    }

    /**
     *   オプションを取得する
     */
    public ChoiceGroup getOptionField(int index)
    {
        preferences    = null;
        preferences    = new ChoiceGroup("オプション", Choice.MULTIPLE);
        preferences.append("スレ一覧(subject.txt)も更新する", null);
        preferences.append("更新終了時、バイブ実行", null);
        preferences.append("更新終了時、アプリを終了", null);
        preferences.append("WX310系のgzip転送対策", null);

        // デフォルト値を反映させる
        int setData = storageDb.getPreference();
        if ((setData & 1) != 0)
        {
            preferences.setSelectedIndex(1, true);
        }
        if ((setData & 2) != 0)
        {
            preferences.setSelectedIndex(2, true);
        }
        if ((setData & 4) != 0)
        {
            preferences.setSelectedIndex(0, true);
        }
        if ((setData & 8) != 0)
        {
            preferences.setSelectedIndex(3, true);
        }
        return (preferences);
    }

    /**
     *  選択肢の数を取得する
     *  
     */
    public int numberOfSelections()
    {
        return (0);
    }

    /**
     *  選択肢の文字列を取得する
     *  
     */
    public String getSelectionItem(int itemNumber)
    {
        return ("");
    }
    
    /**
     *  選択用のガイド文字列を取得する
     *  
     */
    public String getSelectionGuidance()
    {
        return ("MenuでDir確定");
    }

    /**
     *  タイトルを表示するかどうか？
     *  
     */
    public boolean isShowTitle()
    {
        return (true);
    }

    /**
     *   start時、アイテムを初期化するか？
     * 
     */
    public boolean isItemInitialize()
    {
        return (true);
    }

    /**
     *  ボタンの数を取得する
     *  
     */
    public int getNumberOfSelectionButtons()
    {
        return (1);
    }

    /**
     *  ボタンのラベルを取得する
     *  
     */
    public String getSelectionButtonLabel(int buttonId)
    {
        return ("DIR選択");
    }

    /**
     *  背景を互い違いの色で表示するか？
     *  
     */
    public boolean getSelectionBackColorMode()
    {
        return (false);
    }

    
    public void setSelectedItem(int itemId)
    {
        itemSelectionIndex = itemId;
    }

    public int getSelectedItem(int itemNumber)
    {
        if (itemNumber != 0)
        {
            return (-1);
        }
        return (itemSelectionIndex);
    }
    
    public boolean isSelectedItemMulti()
    {
        return (false);
    }

    public int getNumberOfSelectedItems()
    {
        if (itemSelectionIndex < 0)
        {
            return (0);
        }
        return (1);
    }

    // 選択されたボタンIDを応答する
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            resetSelectionItems();
        }
        return (false);
    }

    // アイテム選択をクリアする
    public void resetSelectionItems()
    {
        itemSelectionIndex = -1;
        return;
    }

    // メニューボタンが押された
    public boolean triggeredMenu()
    {    
        // 画面を切り替える...
        return (true);
    }
 
    /**
     *  板のスレ取得情報(a2b.idx)をファイルに書き出す
     *
     */
    public void backupA2BIndex()
    {
        subjectDb.outputA2BIndex();
    }

    /**
     *  板のスレ取得情報(a2b.idx)をメモリに展開する
     * @param bbsIndex
     */
    public void prepareA2BIndex(int bbsIndex)
    {
        currentBbsUrl  = bbsManager.getBoardUrl(bbsIndex);
        currentBbsNick = bbsManager.convertBoardNick(currentBbsUrl);
        subjectDb.prepareA2BIndex(currentBbsNick);
    }

    /**
     *  現在のBBSのURLを取得する
     * @return
     */
    public String getCurrentBBSUrl()
    {
        return (currentBbsUrl);
    }

    /**
     *  現在のBBSのニックネームを取得する
     * @return
     */
    public String getCurrentBBSNick()
    {
        return (currentBbsNick);
    }

    /**
     *  板に含まれるスレ数を応答する
     * @return
     */
    public int getNumberOfThreads()
    {
        if (subjectDb == null)
        {
            return (-1);
        }
        return (subjectDb.getNumberOfThread());
    }

    /**
     *  スレタイトルを取得する
     * @param threadIndex
     * @return
     */
    public A2BUa2BsubjectDataParser.subjectDataHolder getThreadData(int threadIndex)
    {
        A2BUa2BsubjectDataParser.subjectDataHolder data = subjectDb.getSubjectData(threadIndex);
        return (data);
    }

    /**
     *  カウンタをリセット
     *
     */
    public void resetUpdateThreadCount()
    {
        updateThreadCount = 0;
        currentThreadCount = 0;
        errorThreadCount = 0;
    }

    /**
     *  カウンタの値を取得
     * @return
     */
    public int getUpdateThreadCount()
    {
        return (updateThreadCount);
    }

    /**
     *  現在更新中のスレ数を応答
     * @return
     */
    public int getCurrentThreadCount()
    {
        return (currentThreadCount);
    }

    /**
     *  取得エラーが発生したスレ数を応答する
     * @return
     */
    public int getErrorThreadCount()
    {
    	return (errorThreadCount);
    }    
    
    /**
     *  subject.txtを取得する
     * @param url 取得先URL
     * @param baseDirectory 記録するディレクトリ
     * @param fileName 記録するファイル名
     */
    public void getSubjectTxt(String url, String directory, String fileName)
    {
    	try
    	{
            MidpFileHttpCommunication communicator = new MidpFileHttpCommunication();
    	    communicator.prepare(null);
    	    communicator.SetUserAgent("Monazilla/1.00  a2BU/1.00");
    	    communicator.DoHttpMain(url, directory, fileName);
    	    communicator = null;
    	}
    	catch (Exception ex)
    	{
    		//
    	}
        System.gc();    	
    	return;
    }
    
    /**
     *  次スレへ移動...
     * @param threadIndex
     */
    public void finishTrigger(int threadIndex)
    {
        threadIndexNumber = threadIndex;
        Thread thread = new Thread()
        {
            public void run()
            {
            	int waitTime = storageDb.getWaitTime();
            	if (waitTime > 0)
            	{
            		// 巡回を実施するまで、少し止まる。
            		try
            		{
            	        sleep(waitTime);
            		}
            		catch (Exception ex)
            		{
            			// 止まる...
            		}
            	}
        
                baseCanvas.redraw(false);
                baseCanvas.updateData(threadIndexNumber);
                System.gc();
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
     *  画面再描画指示
     * @param force
     */
    public void redraw(boolean force)
    {
        // 画面表示を更新
        baseCanvas.redraw(force);
    }

    /**
     *  ログ更新を実施！
     * @param url
     * @param fileName
     */
    public void updateLogFile(String url, String directory, String fileName, A2BUa2BsubjectDataParser.subjectDataHolder data)
    {
        boolean err = true;
        boolean ret = true;
        int responseCode = 200;
        String updateFileName = directory + fileName;
    	
    	// 画面表示を更新
        baseCanvas.redraw(false);

        long fileSize = MidpFileOperations.GetFileSize(updateFileName);
        if (fileSize <= 0)
        {
        	// まだログを取得していなかった...
        	// (subject.txtの取得ロジックを流用する)
        	getSubjectTxt(url, directory, fileName);
        }
        else
        {
            fileSize = fileSize - 1;
            
            /// ログ更新を呼び出す！
            err = httpCommunicator.GetHttpCommunication(updateFileName, url, null, null, (int) fileSize, -1, true);
            ret = httpCommunicator.isReceived();
            responseCode = httpCommunicator.getLastResponseCode();
        }

        // 更新されていた？
        if ((ret == true)||((ret == false)&&(err == false))||(responseCode == 302))
        {
            long lastModified = data.lastModified;

            // スレ状態を更新...
            data.currentStatus = A2BUa2BsubjectDataParser.STATUS_UPDATE;
            if ((err == false)||(responseCode == 302))
            {
                // 受信エラー発生...
                data.currentStatus = A2BUa2BsubjectDataParser.STATUS_GETERROR;
            }
            
            try
            {
                // 更新時刻をとってくる
                Date curTime = new Date();
                lastModified = curTime.getTime();
            }
            catch (Exception e)
            {
                // 例外発生、何もしない...
            }

            if (data.favoriteIndex >= 0)
            {
                // お気に入りに登録されてるスレだった！
                // （ここでお気に入りの情報も更新する）
                A2BUa2BFavoriteManager.a2BFavoriteThread favorData = favoriteDb.getThreadData(data.favoriteIndex);
                favorData.currentStatus = data.currentStatus;
                favorData.lastModified  = lastModified;
                String bbsUrl = getCurrentBBSUrl() + "/";
                favorData.setThreadUrl(bbsUrl);
            }
            data.lastModified = lastModified;
            if (data.currentStatus == A2BUa2BsubjectDataParser.STATUS_GETERROR)
            {
            	errorThreadCount++;
            }
            else
            {
                updateThreadCount++;
            }
        }
        currentThreadCount++;
        System.gc();
        return;
    }
    
    /**
     *  情報メッセージを設定する
     * 
     * @param message
     */
    public void setInformationMessage(String message)
    {
        informationMessage = message;
    }

    /**
     *  情報メッセージを応答する
     * 
     * @return
     */
    public String getInformationMessage()
    {
        return (informationMessage);
    }

    /**
     *  板の数を応答する 
     * @return
     */
    public int getNumberOfBBS()
    {
        if (bbsManager == null)
        {
            return (-1);
        }
        return (bbsManager.getNumberOfBoardName());
    }
}
