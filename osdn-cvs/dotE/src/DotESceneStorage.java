import javax.microedition.lcdui.TextField;
import javax.microedition.lcdui.ChoiceGroup;
import jp.sourceforge.nnsi.a2b.forms.IDataInputStorage;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.IInformationScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.IFileSelectionStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;

public class DotESceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{

    /***** 揮発データ(ここから) *****/
    private TextField    dataFileNameField   = null;
    private String       informationMessage = null;
    private int          itemSelectionIndex = -1;
    /***** 揮発データ(ここまで) *****/

    /***** その他アクセスクラス *****/
    private MidpCanvas                baseCanvas = null;
    private MidpResourceFileUtils resUtils       = null;
    
    private DotEDataStorage storageDb = null;
    
    /**
     *  コンストラクタ
     *
     *  @param dataStorage データ記録クラス
     *  @param resourceUtils リソースアクセスクラス
     *  @param fileUtils ファイルアクセスクラス
     */
    public DotESceneStorage(DotEDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpCanvas dataProcess, MidpFileDirList dirList)
    {
        storageDb        = dataStorage;
        resUtils         = resourceUtils;
        baseCanvas       = dataProcess;
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
        dataFileNameField = null;
        resetSelectionItems();
        System.gc();
        return;
    }

    /**
     *  データ入力がＯＫされたとき
     *  
     */
    public void dataEntry()
    {
        storageDb.setDataFileName(dataFileNameField.getString());
        dataFileNameField = null;
        resetSelectionItems();
        System.gc();
        return;
    }

    /**
     *  データ入力のための準備
     *  
     */
    public void prepare()
    {

    }

    /**
     *  次画面の切り替え
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == DotESceneDB.FILENAME_FORM)
        {
        	return (DotESceneDB.DEFAULT_SCREEN);
        }
        if (screenId == DotESceneDB.DIR_SCREEN)
        {
        	return (DotESceneDB.DEFAULT_SCREEN);
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
    	dataFileNameField = null;
    	String fileName = "" + storageDb.getDataFileName();
    	dataFileNameField = new TextField("データファイル名", fileName, 64, TextField.ANY);
        return (dataFileNameField);
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
     *  終了トリガ...
     * @param threadIndex
     */
    public void finishTrigger(int threadIndex)
    {
    	//
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
