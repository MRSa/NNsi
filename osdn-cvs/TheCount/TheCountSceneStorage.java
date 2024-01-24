import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.screens.informations.*;
import jp.sourceforge.nnsi.a2b.screens.selections.*;
import jp.sourceforge.nnsi.a2b.forms.*;
import jp.sourceforge.nnsi.a2b.utilities.*;

/**
 * アプリケーションのデータを格納するクラス
 * 
 * @author MRSa
 *
 */
public class TheCountSceneStorage implements IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage, ISelectionScreenStorage
{
    private final int  MaxDataInputLength   = 48;

    /***** 揮発データ(ここから) *****/
    private int          itemSelectionIndex = -1;
    private String       informationMessage  = null;
    private TextField    inputItem1NameField = null;
    private TextField    inputItem2NameField = null;
    private TextField    inputItem3NameField = null;
    private TextField    inputItem4NameField = null;
    private TextField    inputItem5NameField = null;
    private TextField    inputItem6NameField = null;
    /***** 揮発データ(ここまで) *****/

    /***** その他アクセスクラス *****/
    private MidpResourceFileUtils resUtils    = null;
    private TheCountDataStorage storageDb     = null;
    
    /**
     *  コンストラクタ
     *
     *  @param dataStorage データ記録クラス
     *  @param resourceUtils リソースアクセスクラス
     *  @param fileUtils ファイルアクセスクラス
     */
    public TheCountSceneStorage(TheCountDataStorage dataStorage, MidpResourceFileUtils resourceUtils)
    {
        resUtils = resourceUtils;
        storageDb = dataStorage;
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
    	inputItem1NameField = null;
    	inputItem2NameField = null;
    	inputItem3NameField = null;
    	inputItem4NameField = null;
    	inputItem5NameField = null;
    	inputItem6NameField = null;
    	System.gc();
    	return;
    }

    /**
     *  データ入力がＯＫされたとき
     *  
     */
    public void dataEntry()
    {
        // 
    	String ddd  = inputItem1NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(0, ddd);
    	}
    	
        // 
    	ddd  = inputItem2NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(1, ddd);
    	}
    	
        // 
    	ddd  = inputItem3NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(2, ddd);
    	}
    	
        // 
    	ddd  = inputItem4NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(3, ddd);
    	}
    	
        // 
    	ddd  = inputItem5NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(4, ddd);
    	}
    	
        // 
    	ddd  = inputItem6NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(5, ddd);
    	}
    	inputItem1NameField = null;
    	inputItem2NameField = null;
    	inputItem3NameField = null;
    	inputItem4NameField = null;
    	inputItem5NameField = null;
    	inputItem6NameField = null;
    	System.gc();
    	return;
    }

    /**
     *  データ入力のための準備
     *  
     */
    public void prepare()
    {
        // 何もしない        
    }

    /**
     *  次画面の切り替え
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == TheCountSceneDB.WELCOME_SCREEN)
        {
            return (TheCountSceneDB.DIR_SCREEN);
        }
        else if (screenId == TheCountSceneDB.DIR_SCREEN)
        {
            return (TheCountSceneDB.DEFAULT_SCREEN);
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
        return (6);
    }

    /**
     *  データ入力フィールドを取得する
     *  
     */
    public TextField getDataInputField(int index)
    {
    	String itemName = storageDb.getItemName(index);
    	if (index == 0)
    	{
    		inputItem1NameField = new TextField("項目1", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem1NameField);
    	}
    	if (index == 1)
    	{
    		inputItem2NameField = new TextField("項目2", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem2NameField);
    	}
    	if (index == 2)
    	{
    		inputItem3NameField = new TextField("項目3", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem3NameField);
    	}
    	if (index == 3)
    	{
    		inputItem4NameField = new TextField("項目4", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem4NameField);
    	}
    	if (index == 4)
    	{
    		inputItem5NameField = new TextField("項目5", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem5NameField);
    	}
    	if (index == 5)
    	{
    		inputItem6NameField = new TextField("項目6", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem6NameField);
    	}
    	return (null);
    }

    /**
     *  オプションの個数を取得する
     *  
     */
    public int getNumberOfOptionGroup()
    {
        return (0);
    }

    /**
     *   オプションを取得する
     */
    public ChoiceGroup getOptionField(int index)
    {
        return (null);
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

    /**
     *   選択されたボタンIDを応答する
     *   
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            resetSelectionItems();
        }
        return (false);
    }

    /**
     * アイテム選択をクリアする
     * 
     */
    public void resetSelectionItems()
    {
        itemSelectionIndex = -1;
        return;
    }

    /**
     * メニューボタンが押された
     * 
     */
    public boolean triggeredMenu()
    {    
        // 画面を切り替える...
        return (true);
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
}
