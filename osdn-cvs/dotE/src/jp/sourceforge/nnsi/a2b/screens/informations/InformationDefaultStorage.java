package jp.sourceforge.nnsi.a2b.screens.informations;

/**
 *  情報表示画面用データ保持クラスの標準実装です。
 * 
 * @author MRSa
 *
 */
public class InformationDefaultStorage implements IInformationScreenStorage
{
    /** 表示すべきメッセージ */
	private String showMessage = null;
	
    /**
     *   コンストラクタでは、特に何も行いません。
     *
     */
	public InformationDefaultStorage()
    {

    }

	/**
	 *  表示すべきメッセージを設定する
	 * @param message 表示すべきメッセージ
	 */
	public void setInformationMessageToShow(String message)
	{
	    showMessage = message;
	}

	/**
	 *  表示するメッセージをクリアする
	 *
	 */
	public void clearInformationMessageToShow()
	{
        showMessage = null;
        System.gc();
	}
	
    /**
     *  画面表示すべきメッセージの取得
     *  
     *  @return 表示すべきメッセージ
     */
	public String getInformationMessageToShow()
	{
        return (showMessage);
	}

    /**
     *   選択されたボタン名の設定
     * 
     *   @param buttonId 選択されたボタンのID
     */
    public void setButtonId(int buttonId)
    {
        // 何もしない
    }

    /**
     *  ボタンラベルを取得する
     *  
     *  @param buttonId ラベルを取得したいボタンID
     *  @return ボタンラベル
     */
    public String getInformationButtonLabel(int buttonId)
    {
        return ("OK");
    }

    /**
     *  画面IDをキーにして表示するボタン数を取得する
     *  
     *  @param screenId 画面ID
     *  @return 表示するボタンの数
     */
    public int getNumberOfButtons(int screenId)
    {
        return (1);
    }
	
    /**
	 *  次の画面切り替え先画面IDを取得する
	 *  
	 *  @param screenId (現在の)画面ID
	 *  @return 切り替える画面ID
	 */
	public int nextSceneToChange(int screenId)
	{
        return (IInformationScreenStorage.SCENE_TO_PREVIOUS);
	}
}
