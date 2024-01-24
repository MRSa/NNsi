package jp.sourceforge.nnsi.a2b.screens.selections;


/**
 * 選択画面用の操作クラス<br>
 * ISelectionScreenStorageクラスからデータをもらい、SelectionScreen
 * (画面)に表示する情報へ加工します。
 * @see ISelectionScreenStorage
 * @see SelectionScreen
 * 
 * @author MRSa
 */
public class SelectionScreenOperator
{
    static public final int BACKCOLOR_EVEN = 0x00d8d8ff;
    static public final int BACKCOLOR_ODD  = 0x00f8f8f8;
	static public final int WIDTH_MARGIN = 3;
    static public final int HEIGHT_MARGIN = 0;
	private ISelectionScreenStorage storage = null;

    /**
     * コンストラクタ
     * 
     * @param object 選択画面用記憶クラス
     */
    public SelectionScreenOperator(ISelectionScreenStorage object)
    {
        storage = object;
    }

    /**
     *  クラスの実行準備（なにもしない）
     */
    public void prepare()
    {
        //
    }

    /**
     * 表示するアイテム数を取得する
     * 
     * @return 表示するアイテム数
     */
    public int getSelectionItemCount()
    {
        return (storage.numberOfSelections());
    }
    
    /**
     *  メッセージデータの取得
     *  
     *  @param itemNumber アイテム番号(0スタート)
     *  @return 表示すべきメッセージ
     */
    public String getSelectionItem(int itemNumber)
    {
        String item = storage.getSelectionItem(itemNumber);
        if (item == null)
        {
        	return ("");
        }
    	return (item);
    }

    /**
     *  アイテムを複数選択することが可能か
     *  
     *  @return 複数選択可能(true) / 複数選択不可能(false)
     */
    public boolean isSelectedItemMulti()
    {
        return (storage.isSelectedItemMulti());
    }

    /**
     *  アイテムを選択したとき
     *  
     *  @param  itemId 選択したアイテムのID
     */
    public void selectedItem(int itemId)
    {
        storage.setSelectedItem(itemId);
        System.gc();
        return;
    }

    /**
     *   選択しているアイテムの数を応答する
     * 
     * @return 選択しているアイテムの数
     */
    public int getNumberOfSelectedItems()
    {
        return (storage.getNumberOfSelectedItems());
    }

    /**
     *   選択したアイテムのIDを応答する
     * 
     * @param itemIndex 選択したアイテムのインデックス番号
     * @return 選択したアイテムのID
     */
    public int getSelectedItem(int itemIndex)
    {
    	return (storage.getSelectedItem(itemIndex));
    }    
    
    /**
     *  コマンドボタンの入力
     *  
     *  @param  buttonId 押されたボタン
     *  @return 登録OK
     */
    public boolean selectedCommandButton(int buttonId)
    {
        boolean ret = storage.setSelectedCommandButtonId(buttonId);
        System.gc();
        return (ret);
    }

    /**
     *  メニューボタンが押されたことを通知する
     * 
     * @return 処理をうけつけた(true) / 処理しなかった(false)
     */
    public boolean triggeredMenu()
    {
    	return (storage.triggeredMenu());
    }

    /**
     * 選択肢用のガイド文字列を取得する
     * 
     * @return 選択用ガイド文字列
     */
    public String getGuideMessage()
    {
    	return (storage.getSelectionGuidance());
    }

    /**
     *   タイトルを表示するかどうか
     * 
     * @return 表示する(true) / 表示しない(false)
     */
    public boolean isShowTitle()
    {
        return (storage.isShowTitle());
    }

    /**
     *  start時、アイテムを初期化するか？
     * @return 初期化する(true) / 初期化しない(false)
     */
    public boolean isItemInitialize()
    {
    	return (storage.isItemInitialize());
    }
    
    /**
     * 画面上に用意するボタン数を取得する
     * 
     * @return 用意するボタン数
     */
    public int getNumberOfButtons()
    {
    	return (storage.getNumberOfSelectionButtons());
    }
    
    /**
     * 画面上に用意するボタンラベルを取得する
     * 
     * @param buttonId 取得するボタンのID
     * @return ボタンラベル
     * 
     */
    public String getButtonLabel(int buttonId)
    {
    	return (storage.getSelectionButtonLabel(buttonId));
    }

    /**
	 * 選択肢の背景色を取得する
	 * 
	 * @param line 行番号
	 * @return 背景色
	 * 
	 */
	public int getSelectionBackColor(int line)
	{
        if ((storage.getSelectionBackColorMode() == true)&&((line % 2) == 0))
        {
            return (BACKCOLOR_EVEN);
        }
        return (BACKCOLOR_ODD);
	}

	/**
     *   次の画面をどこに切り替えるか決める
     * 
     * @param sceneId 現在の画面ID
     * @return 次の画面ID
     */
    public int nextSceneToChange(int sceneId)
    {
        return (storage.nextSceneToChange(sceneId));
    }
}
