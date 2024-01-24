import java.util.Date;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.screens.storage.*;
import jp.sourceforge.nnsi.a2b.framework.interfaces.*;
import jp.sourceforge.nnsi.a2b.utilities.*;

/**
 * アプリケーションのデータを格納するクラス
 * 
 * @author MRSa
 *
 */
public class HiMemoSceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{
    private final int  MaxDataInputLength   = 2048;

    /***** 揮発データ(ここから) *****/
    private TextField    inputDataField1     = null;
    private TextField    inputDataField2     = null;
    
    private int          itemSelectionIndex = -1;
    /***** 揮発データ(ここまで) *****/

    /***** その他アクセスクラス *****/
    private MidpFileOperations fileOperation = null;
    private MidpResourceFileUtils resUtils   = null;
    private HiMemoDataStorage storageDb = null;
    
    /**
     *  コンストラクタ
     *
     *  @param dataStorage データ記録クラス
     *  @param resourceUtils リソースアクセスクラス
     *  @param fileUtils ファイルアクセスクラス
     */
    public HiMemoSceneStorage(HiMemoDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpFileOperations fileUtils)
    {
        storageDb = dataStorage;
        fileOperation = fileUtils;
        resUtils = resourceUtils;
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
        return (resUtils.getResourceText("/res/welcome.txt"));
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
        return;
    }

    /**
     * メモファイル名を応答する
     * 
     * @return
     */
    public String getMemoFileName()
    {
        if (storageDb.getBaseDirectory() == null)
        {
            return (null);
        }
        String fileName = storageDb.getBaseDirectory() + "hiMEMO.txt";
        return (MidpResourceFileUtils.convertInternalFileLocation(fileName));
    }
    
    /**
     *  データ入力がＯＫされたとき
     *  
     */
    public void dataEntry()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                String fileName = storageDb.getBaseDirectory() + "hiMEMO.txt";
                Date date = new Date();
                String outputString = date.toString() + "," + inputDataField1.getString() + "," + inputDataField2.getString();
                fileOperation.outputDataTextFile(fileName, null, null, outputString, true);
                inputDataField1 = null;
                inputDataField2 = null;
            }
        };
        try
        {
            thread.start();
//            thread.join();
        }
        catch (Exception ex)
        {
            String fileName = storageDb.getBaseDirectory() + "hiMEMO.txt";
            String outputString = ex.getMessage() + " \r\n" + ex.toString();
            fileOperation.outputDataTextFile(fileName, null, null, outputString, true);            
        }
        thread = null;
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
        if (screenId == HiMemoSceneDB.WELCOME_SCREEN)
        {
            return (HiMemoSceneDB.DIR_SCREEN);
        }
        else if (screenId == HiMemoSceneDB.DIR_SCREEN)
        {
            return (HiMemoSceneDB.DEFAULT_SCREEN);
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
        return (2);
    }

    /**
     *  データ入力フィールドを取得する
     *  
     */
    public TextField getDataInputField(int index)
    {
        if (index == 0)
        {
            inputDataField1 = new TextField("データ1", "", MaxDataInputLength, TextField.ANY);
            return (inputDataField1);
        }
        inputDataField2 = new TextField("データ2", "", MaxDataInputLength, TextField.ANY);
        return (inputDataField2);
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
        return (true);
    }
}
