import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;

/**
 *  スレ一覧選択画面用ストレージ
 * 
 * @author MRSa
 *
 */
public class A2chSListStorage implements ISelectionScreenStorage
{
    private A2chSSceneStorage sceneStorageDb = null;
	private BusyConfirmationStorage busyStorageDb = null;
	
	private A2chSSceneStorage.a2chSThreadInformation selectedThread = null;
	private boolean isShowBusy = true;

	/**
     *  コンストラクタ
     * 
     *
     */
	public A2chSListStorage(A2chSSceneStorage sceneStorage, BusyConfirmationStorage busyStorage)
    {
        sceneStorageDb = sceneStorage;
		busyStorageDb = busyStorage;
    }
	
    /**	
	 *  選択肢の数を取得する
	 *  
	 */
    public int numberOfSelections()
    {
        return (sceneStorageDb.getNumberOfThreadTitles());
    }

    /**
     *  選択肢の文字列を取得する
     *  
     */
    public String getSelectionItem(int itemNumber)
    {
    	return ((sceneStorageDb.getThreadInformation(itemNumber)).getTitle());
    }

    /**
     *  アイテム選択を複数同時に可能かどうかを応答する
     *  
     */
    public boolean isSelectedItemMulti()
    {
    	return (false);
    }

    /**
     *  選択したアイテムをクリアする
     *
     */
    public void resetSelectionItems()
    {
    	selectedThread = null;
    	return;
    }

    /**
     *  選択中のアイテム数を応答する
     *  
     */
    public int getNumberOfSelectedItems()
    {
        return (0);
    }

    /**
     *  選択中ののアイテムIDを応答する
     *  
     */
    public int getSelectedItem(int itemNumber)
    {
    	return (ISelectionScreenStorage.BUTTON_CANCELED);
    }

    /**
     *  選択したアイテムを設定する
     *  
     */
    public void setSelectedItem(int itemId)
    {
        selectedThread = sceneStorageDb.getThreadInformation(itemId);
    	return;
    }

    /**
     *  選択されたボタンIDを設定する
     *  
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            // キャンセルされた...終了する
        	return (true);
        }    	
    	if (selectedThread == null)
        {
        	return (false);
        }
        boolean ret = false;

        sceneStorageDb.setInformationMessage("取得中 : " + selectedThread.getTitle());
        Thread thread = new Thread()
        {
        	public void run()
        	{
                // ログを取得する
        		sceneStorageDb.getDatFile(selectedThread);
        		sceneStorageDb.setInformationMessage(null);
        	}
        };
        try
        {
            // 取得が終了するまで待つ...
        	thread.start();
//            thread.join();
            ret = true;
        }
        catch (Exception ex)
        {
        	ret = false;
        }        
    	return (ret);
    }

    /**
     *  選択用のガイド文字列を取得する
     *  
     */
	public String getSelectionGuidance()
	{
		return ("スレ選択でdat取得(通信)");
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
		if (isShowBusy == false)
		{
			return (true);
		}
		// BUSYウィンドウを表示して戻ってきたときには、アイテムを初期化しない
        isShowBusy = false;
        return (false);
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
		return ("取得");
	}
	
    /**
	 *  背景を互い違いの色で表示するか？
	 *  
	 */
	public boolean getSelectionBackColorMode()
	{
		return (true);
	}

	/**
     *  メニューボタンが押されたタイミングを通知する
     *  
     */
	public boolean triggeredMenu()
	{
        if (selectedThread == null)
        {
        	return (false);
        }
        String threadTitle =  selectedThread.getTitle();
        String threadUrl   =  selectedThread.getUrl();
        String title = "タイトル:\r\n  " + threadTitle + "\r\n\r\nURL: \r\n  " + threadUrl  + "\r\n\r\n";

		// ビジーウィンドウの表示...(スレタイの全部表示用...)
		busyStorageDb.showMessage(title, BusyConfirmationStorage.INFORMATION_MODE, A2chSSceneDB.LIST_SCREEN);

        isShowBusy = true;
		return (true);
    }
	
    /**
     *  次画面切り替え先を応答する
     * 
     */
	public int nextSceneToChange(int screenId)
	{
		if (isShowBusy == true)
		{
			int ret = A2chSSceneDB.BUSY_SCREEN;
			return (ret);
		}
        return (A2chSSceneDB.DEFAULT_SCREEN);
    }

    /**
     *  検索結果ファイルを読みこんで一覧表示の準備を行う
     * 
     * @return
     */
	public boolean readSearchResultFile()
	{
		String message = null;
		boolean result = false;

		// ビジーウィンドウの表示...
		busyStorageDb.outputInformationMessage("検索結果チェック中...", true, true);
		
        // 既に検索結果は未解析、あるいは検索結果ファイルが存在するか？
        if (sceneStorageDb.getNumberOfThreadTitles() == 0)
		{
/*
        	if (sceneStorageDb.getUrlToSearch() != null)
        	{
                // 通信しませう...
        		sceneStorageDb.doFind2chThreads();
        	}
*/
        	if (sceneStorageDb.isExistSearchResultFile() == true)
        	{
        	    // ファイルを読み込んでデータを展開する
        	    result = sceneStorageDb.prepareSearchResultFile();
        	    if (result == false)
        	    {
        	    	message = "(2ch検索ヒットなし)";
        	    }
        	}
        	else
        	{
        		message = "(2ch検索未実行)";
        	}
        }
        else
        {
            // 既に検索済み...再構築は行わない
        	result = true;
        }

        // 情報ウィンドウにメッセージを入れる
        if (message != null)
        {
        	sceneStorageDb.setInformationMessage(message);
        }
        selectedThread = null;
        
        // ビジーウィンドウのクローズ
		busyStorageDb.clearInformationMessage();
		busyStorageDb.finishInformationMessage();
		return (result);
	}
}
