/**
 *  選択画面用の画面パッケージです。
 * 
 */
package jp.sourceforge.nnsi.a2b.screens.selections;


/**
 * 情報表示画面で使用するデータのインタフェース
 * 
 * @author MRSa
 *
 */
public interface ISelectionScreenStorage
{
    /** キャンセルボタンの画面ID */
	static public final int BUTTON_CANCELED = -1;

	/** 前の画面に戻る */
	static public final int SCENE_TO_PREVIOUS = -1;

	
    /**
     *  選択肢の数を取得する
     * 
     * @return 選択肢の数
     */ 
    public abstract int numberOfSelections();

    /**
     *  選択肢の文字列を取得する
     *  
     *  @param itemNumber 選択肢
     *  @return 選択肢の文字列
     */
    public abstract String getSelectionItem(int itemNumber);


    /**
     *  アイテム選択を複数同時に可能かどうかを応答する
     * 
     *  @return 同時選択可能(true) / １つだけ(false)
     */
    public abstract boolean isSelectedItemMulti();

    /**
     *  選択したアイテムをクリアする
     *  
     */
    public abstract void resetSelectionItems();
    
    /**
     *  選択中のアイテム数を応答する
     * 
     *  @return 選択中のアイテム数
     */
    public abstract int getNumberOfSelectedItems();

    /**
     *  選択中ののアイテムIDを応答する
     *  
     *  @param itemNumber 選択中のアイテム番号(ゼロスタート)
     *  @return アイテムID
     */
    public abstract int getSelectedItem(int itemNumber);


    /**
     *   選択したアイテムを設定する
     *   
     *   @param itemId 選択したアイテムID
     */
    public abstract void setSelectedItem(int itemId);

    /**
     *  選択されたボタンIDを設定する
     * 
     *  @param buttonId ボタンID
     *  @return 終了する(true) / 終了しない (false)
     */
    public abstract boolean setSelectedCommandButtonId(int buttonId);

    /**
     *  選択用のガイド文字列を取得する<br>
     *  （文字列を応答した場合、画面下部にガイド文字列を表示する)
     *  
     *  @return ガイド文字列
     *  
     */
	public abstract String getSelectionGuidance();

	/**
     *  タイトルを表示するかどうか？
     *  
     *  @return タイトル表示する(true) / 表示しない(false)
     */
	public abstract boolean isShowTitle();

	/**
	 *  start時に、アイテムを初期化するかどうか<br>
	 *  (カーソル位置を保持しておきたいときにfalseとする
	 *  
	 *  @return 初期化する(true) / 初期化しない(false)
	 */
	public abstract boolean isItemInitialize();

	/**
	 *  ボタンの数を取得する
	 * 
	 *  @return 表示するボタンの数
	 */
	public abstract int getNumberOfSelectionButtons();

	/**
     *  ボタンのラベルを取得する
     *  
     *  @param buttonId ボタンのID
     *  @return 表示するボタンのラベル
     */
	public abstract String getSelectionButtonLabel(int buttonId);
	
	/**
     *  背景を互い違いの色で表示するかを取得する
     *
     *  @return 互い違いの色にする(true) / 背景は１色(false)
     */
	public abstract boolean getSelectionBackColorMode();

	/**
     *  メニューボタンが押されたタイミングを通知する
     * 
     *  @return 処理を継続するかどうか
     */
	public abstract boolean triggeredMenu();
	
    /**
	 *  次の画面切り替え先画面IDを取得する
	 *  
	 *  @param screenId (現在の)画面ID
	 *  @return 切り替える画面ID
	 */
	public abstract int nextSceneToChange(int screenId);
}
