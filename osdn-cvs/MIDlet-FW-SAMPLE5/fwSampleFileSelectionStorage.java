import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.selections.*;
/**
 *  ファイル/ディレクトリ選択用ストレージクラス
 * 
 * @author MRSa
 *
 */
public class fwSampleFileSelectionStorage implements ISelectionScreenStorage, IFileSelectionStorage
{
    private String selectedItemName = null;
    private fwSampleStorage storageDb = null;
    private MidpSceneSelector selector = null;
    
    public fwSampleFileSelectionStorage(fwSampleStorage storage, MidpSceneSelector object)
    {
        storageDb = storage;
        selector = object;
    }

    /**
     *  選択肢の数を取得する
     * 
     * @return 選択肢の数
     */
    public int numberOfSelections()
    {
        return (-1);
    }

    /**
     *  選択肢の文字列を取得する
     *  
     *  @param itemNumber 選択肢
     *  @return 選択肢の文字列
     */
    public String getSelectionItem(int itemNumber)
    {
        return ("");
    }

    /**
     *  アイテム選択を複数同時に可能かどうかを応答する
     * 
     *  @return 同時選択可能(true) / １つだけ(false)
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
        //
    }
    
    /**
     *  選択中のアイテム数を応答する
     * 
     *  @return 選択中のアイテム数
     */
    public int getNumberOfSelectedItems()
    {
        if (selectedItemName == null)
        {
            return (0);
        }
        return (1);
    }

    /**
     *  選択中ののアイテムIDを応答する
     *  
     *  @param itemNumber 選択中のアイテム番号(ゼロスタート)
     *  @return アイテムID
     */
    public int getSelectedItem(int itemNumber)
    {
        return (itemNumber);
    }

    /**
     *   選択したアイテムを設定する
     *   
     *   @param itemId 選択したアイテムID
     */
    public void setSelectedItem(int itemId)
    {

    }

    /**
     *  選択されたボタンIDを設定する
     * 
     *  @param buttonId ボタンID
     *  @return 終了する(true) / 終了しない (false)
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        return (true);
    }

    /**
     *  選択用のガイド文字列を取得する<br>
     *  （文字列を応答した場合、画面下部にガイド文字列を表示する)
     *  
     *  @return ガイド文字列
     *  
     */
    public String getSelectionGuidance()
    {
        return ("");
    }

    /**
     *  タイトルを表示するかどうか？
     *  
     *  @return タイトル表示する(true) / 表示しない(false)
     */
    public boolean isShowTitle()
    {
        return (true);
    }

    /**
     *  start時に、アイテムを初期化するかどうか<br>
     *  (カーソル位置を保持しておきたいときにfalseとする
     *  
     *  @return 初期化する(true) / 初期化しない(false)
     */
    public boolean isItemInitialize()
    {
        return (true);
    }

    /**
     *  ボタンの数を取得する
     * 
     *  @return 表示するボタンの数
     */
    public int getNumberOfSelectionButtons()
    {
        return (1);
    }

    /**
     *  ボタンのラベルを取得する
     *  
     *  @param buttonId ボタンのID
     *  @return 表示するボタンのラベル
     */
    public String getSelectionButtonLabel(int buttonId)
    {
        return ("決定");
    }
    
    /**
     *  背景を互い違いの色で表示するかを取得する
     *
     *  @return 互い違いの色にする(true) / 背景は１色(false)
     */
    public boolean getSelectionBackColorMode()
    {
        return (true);
    }
    

    /**
     *  メニューボタンが押されたタイミングを通知する
     * 
     *  @return 処理を継続するかどうか
     */
    public boolean triggeredMenu()
    {
        confirmSelectedName();
        return (true);
    }
    
    /**
     *  次の画面切り替え先画面IDを取得する
     *  
     *  @param screenId (現在の)画面ID
     *  @return 切り替える画面ID
     */
    public int nextSceneToChange(int screenId)
    {
        return (fwSampleFactory.DEFAULT_SCREEN);
    }

    /**
     *  初期ディレクトリを応答する
     *  
     *  @return 初期ディレクトリ
     */
    public String getDefaultDirectory()
    {
        return (null);
    }

    /**
     *  現在選択されているファイルおよびディレクトリの数を応答する
     *  
     *  @return 現在選択されているファイル(ディレクトリ)数
     */
    public int getSelectedNameCount()
    {
        return (getNumberOfSelectedItems());
    }
    
    /**
     *  選択（ファイル）名を応答する
     * 
     *  @param index 選択番号(0スタート)
     *  @return 選択番号のファイル名
     */
    public String getSelectedName(int index)
    {
        return (selectedItemName);
    }

    /**
     *  選択（ファイル）名を設定する
     * 
     * @param name 選択したファイルの名前
     */
    public void setSelectedName(String name)
    {
        selectedItemName = name;
    }

    /**
     *  選択（ファイル）名をクリアする
     *  
     */
    public void clearSelectedName()
    {
        selectedItemName = null;
    }
    
    /**
     *  選択（ファイル）名を確定する
     *  
     */
    public void confirmSelectedName()
    {
        if (selectedItemName.endsWith("/") == false)
        {
            // ファイル名を設定する
            storageDb.setSoundFileName(selectedItemName);
            selector.changeScene(fwSampleFactory.DEFAULT_SCREEN, "");
        }
        return;
    }
}
