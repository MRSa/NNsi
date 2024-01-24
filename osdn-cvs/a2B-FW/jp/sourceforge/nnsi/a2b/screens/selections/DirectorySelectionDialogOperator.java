package jp.sourceforge.nnsi.a2b.screens.selections;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;

/**
 *  ファイル/ディレクトリ一覧画面用操作制御クラス<br>
 * 
 * @author MRSa
 *
 */
public class DirectorySelectionDialogOperator extends SelectionScreenOperator
{
	private MidpFileDirList fileDirList = null;
    private IFileSelectionStorage fileStorage = null;
    private DirectorySelectionDialog  screenClass = null;

    private String scanPath = null;     // 次検索パス（一時的に利用する）
    private String selectedDir = null;  // 選択されたディレクトリ
    private boolean isDirectoryMode = true;  // 

    /**
     * クラスの初期化を行います<br>
     * 利用前に必ずprepare()を呼び出してください。
     * @param storageDb 画面操作用記録クラス
     * @param fileStorageDb ファイル・ディレクトリの動作記録クラス
     * @param object ファイル/ディレクトリ操作クラス
     */
    public DirectorySelectionDialogOperator(ISelectionScreenStorage storageDb, IFileSelectionStorage fileStorageDb, MidpFileDirList object, boolean directoryMode)
    {
        super(storageDb);
        fileDirList = object;
        fileStorage = fileStorageDb;
        isDirectoryMode = directoryMode;
    }
    
    /**
     *  クラスの使用準備を行います
     *  @param screen 表示画面
     */
    public void prepare(DirectorySelectionDialog screen)
    {
    	screenClass = screen;
    	fileDirList.setScanDirectoryMode(isDirectoryMode);
    	scanDirectory(fileStorage.getDefaultDirectory());
        return;
    }

    /**
     *  ディレクトリをスキャンする
     *  @param path 検索するディレクトリ
     *
     */
    private void scanDirectory(String path)
    {
		scanPath = path;
        Thread thread = new Thread()
        {
            public void run()
            {
                if (scanPath == null)
                {
                	// ディレクトリの検索
                	fileDirList.scanDirectory(null);
                }
                else if (scanPath.equals(".."))
                {
            	    // ひとつ上のディレクトリ検索
            	    fileDirList.upDirectory();
                }
                else  // if (scanPath != null)
                {
            	    // ひとつ上のディレクトリ検索
            	    fileDirList.downDirectory(scanPath);
                }
                

                // タイトルを更新する
            	screenClass.setTitle(fileDirList.getDirectoryName());
            	
                // 画面を再描画する
                screenClass.refreshScreen();
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
        	//
        }
	    return;
    }

    /**
     *  タイトルを表示するかどうか？
     *  (現状は、必ず表示する)
     *  @return true
     */
	public boolean isShowTitle()
	{
		return (true);
    }

    /**
     * 表示するアイテム数を取得する
     * 
     * @return 表示するアイテム数
     */
    public int getSelectionItemCount()
    {
        return (fileDirList.getCount());
    }

    /**
     *  表示するメッセージデータを取得する
     *  
     *  @param itemNumber アイテム番号(0スタート)
     *  @return ファイル名
     */
    public String getSelectionItem(int itemNumber)
    {
    	MidpFileDirList.fileInformation info = fileDirList.getFileInfo(itemNumber);
        return (info.getFileName());
    }

    /**
     *  アイテムを選択したとき
     *  
     *  @param  itemId 選択したアイテムのID
     */
    public void selectedItem(int itemId)
    {
        selectedDir = null;
    	if ((itemId >= 0)&&(itemId < fileDirList.getCount()))
    	{
    		selectedDir = getSelectionItem(itemId);
            String directory = fileDirList.getDirectoryName();
            if (isDirectoryMode == false)
            {
                fileStorage.setSelectedName(directory + selectedDir);
            }
        }
    	return;
    }

    /**
     *  コマンドボタンの入力
     *  
     *  @param  buttonId 押されたボタン
     *  @return 登録OK
     */
    public boolean selectedCommandButton(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            // キャンセルされた、何もしない
            return (true);
        }
        fileStorage.confirmSelectedName();
        scanDirectory(selectedDir);
        System.gc();
        return (false);
    }

    /**
     *  メニューボタンが押されたときの処理
     *  (ディレクトリの確定)
     * 
     * @return true (画面遷移を実行)
     */
    public boolean triggeredMenu()
    {
        // ディレクトリ名を記録する
        String directory = fileDirList.getDirectoryName();
        fileStorage.setSelectedName(directory);
        fileStorage.confirmSelectedName();
        return (true);
    }
}
