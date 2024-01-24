package jp.sourceforge.nnsi.a2b.screens.informations;


/**
 * 情報表示画面が使用するデータクラスのインタフェース
 * 
 * @author MRSa
 *
 */
public interface IInformationScreenStorage
{
    /** 前の画面に戻る */
	static public final int SCENE_TO_PREVIOUS = -1;

	/** キャンセルボタンのボタンID */
    static public final int BUTTON_CANCELED = -1;

    /** OKボタンのボタンID */
    static public final int BUTTON_ACCEPTED = 0;

    /**
     *  画面表示すべきメッセージの取得
     *  
     *  @return 表示すべきメッセージ
     */
	public abstract String getInformationMessageToShow();


    /**
     *   選択されたボタン名の設定
     * 
     *   @param buttonId 選択されたボタンのID
     */
    public abstract void setButtonId(int buttonId);


    /**
     *  ボタンラベルを取得する
     *  
     *  @param buttonId ラベルを取得したいボタンID
     *  @return ボタンラベル
     */
    public abstract String getInformationButtonLabel(int buttonId);

    /**
     *  画面IDをキーにして表示するボタン数を取得する
     *  
     *  @param screenId 画面ID
     *  @return 表示するボタンの数
     */
    public abstract int getNumberOfButtons(int screenId);
	
    /**
	 *  次の画面切り替え先画面IDを取得する
	 *  
	 *  @param screenId (現在の)画面ID
	 *  @return 切り替える画面ID
	 */
	public abstract int nextSceneToChange(int screenId);
}
