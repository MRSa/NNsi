package jp.sourceforge.nnsi.a2b.screens.informations;

import jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;

/**
 *   ビジー表示（動作中の情報表示）を行ったり、実行を開始してよいかどうかを
 *   確認するとき利用可能な画面のデータ保持部分です。
 *   (InformationScreen および InformationScreenOperator とともに利用します。)
 *   <br>
 *   このクラスに表示したい文字列をセットし画面を切り替えてやることで、ビジー
 *   表示や確認ダイアログの表示を実現することができます。
 *   <br>
 *   ビジー表示を行うために ILogMessageDisplayインタフェースを実装しています。
 *   詳細は、
 *   {@link jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay}
 *   を参考にご利用ください。
 *   <br>
 *   なお、InformationScreenOperator がこのクラスを利用するために
 *   {@link jp.sourceforge.nnsi.a2b.screens.informations.IInformationScreenStorage}
 *   を実装しています。
 *   <br>
 *   <em>(ユーザは、 IInformationScreenStorageのインタフェースを
 *   利用する必要はありません。)</em>
 *   
 * @author MRSa
 *
 */
public class BusyConfirmationStorage implements IInformationScreenStorage, ILogMessageDisplay
{
    /** ビジーモード (ウィンドウボタン数が0個の場合) */
	static public final int BUSY_MODE = 0;

	/** 情報表示モード (ウィンドウボタン数が「OK」ボタン1個の場合) */
    static public final int INFORMATION_MODE = 1;   // 
    
    /** 確認モード (ウィンドウボタン数が2個(「OK」と「Cancel」)の場合) */
    static public final int CONFIRMATION_MODE = 2; 

    private IMidpScreenCanvas view    = null; 
    private boolean isButtonAccepted = true;
    private int   confirmationMode    = INFORMATION_MODE;
    private int   returnScreen        = -1;
	private String dialogMessage       = null;
	
	/**
	 *  コンストラクタでの処理はありません。<br>
	 *  <em>(必要な初期化処理は、prepare()で実行します。)</em>
	 */
    public BusyConfirmationStorage()
    {
        // 何もしない
     }

    /**
     *  <b>クラスの使用準備を行う</b>
     *  
     * 　@param infoScreen ペアで利用するInformationScreenクラス
     *   @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreen
     * 
     */
    public void prepare(IMidpScreenCanvas infoScreen)
    {
    	view = infoScreen;
    }
    
    /**
     *  <b>メッセージを画面表示する準備をする</b>
     *  画面遷移は発生させない。
     * 
     * @param message 表示するメッセージ
     * @param mode    画面モード BUSY_MODE / INFORMATION_MODE / CONFIRMATION_MODE
     * @param returnScene 画面を閉じたときに開く次の画面ID
     */
    public void prepareMessage(String message, int mode, int returnScene)
    {
        dialogMessage = message;
        confirmationMode = mode;
        returnScreen = returnScene;
    }

    /**
     *  <b>メッセージを画面表示する</b>
     * 
     * @param message 表示するメッセージ
     * @param mode    画面モード BUSY_MODE / INFORMATION_MODE / CONFIRMATION_MODE
     * @param returnScene 画面を閉じたときに開く次の画面ID
     */
    public void showMessage(String message, int mode, int returnScene)
    {
        dialogMessage = message;
        confirmationMode = mode;
        returnScreen = returnScene;
        view.inputFire(false);    // ダミーの画面遷移を発生させる
    }

    /**
     *  <b>OKボタンが押されたかどうかを確認する</b>
     * 
     * @return true:OKボタンが押された
     */
    public boolean isAccepted()
    {
    	return (isButtonAccepted);
    }

    /**
	 *  表示すべきメッセージを取得
	 *  <em>(InformationScreenOperatorが使用します。)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
	 *  
	 *  @return 表示すべきメッセージ
	 */
	public String getInformationMessageToShow()
	{
		return (dialogMessage);
	}

    /** 
     *  選択されたボタンを設定する
	 *  <em>(InformationScreenOperatorが使用します。)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
     *  
     *  @param buttonId 選択されたボタン
     */
    public void setButtonId(int buttonId)
    {
    	if (buttonId == BUTTON_ACCEPTED)
    	{
    		isButtonAccepted = true;
    	}
    	else
    	{
    		isButtonAccepted = false;
    	}
    	return;
    }

    /**
	 *  ボタンラベルを取得
	 *  <em>(InformationScreenOperatorが使用します。)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
	 *  
	 *  @param  buttonId ボタンのID
	 *  @return ボタンラベル
	 */
    public String getInformationButtonLabel(int buttonId)
    {
        if (buttonId == BUTTON_ACCEPTED)
        {
        	return ("OK");
        }
        return ("Cancel");
    }

    /**
     *  表示するボタン数を取得する
	 *  <em>(InformationScreenOperatorが使用します。)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
	 *  
     *  @return 表示するボタン数
     */
    public int getNumberOfButtons(int screenId)
    {
        return (confirmationMode);
    }

    /**
	 *  次の画面切り替え先の指定
	 *  <em>(InformationScreenOperatorが使用します。)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
	 *   
	 *  @param screenId 現在の画面ID
	 *  @return 次に遷移する画面ID
	 */
	public int nextSceneToChange(int screenId)
	{
        int nextScene = IInformationScreenStorage.SCENE_TO_PREVIOUS;
		if (confirmationMode == BUSY_MODE)
		{
			// 画面ロック中(画面遷移させない)
			return (screenId);
		}
		if (returnScreen != IInformationScreenStorage.SCENE_TO_PREVIOUS)
		{
			nextScene = returnScreen;
		    returnScreen = IInformationScreenStorage.SCENE_TO_PREVIOUS;	
		}
        return (nextScene);
	}

    /**
     *  ログの出力の開始
	 *  <em>(ログ出力するユーティリティクラスが使用します。)</em>
     * 
     * @param message 表示するメッセージ
     * @param forceUpdate 表示を強制的に更新するか？
     * @param lockOperation データ表示中、操作を禁止させるか？
     * @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
     */
    public void outputInformationMessage(String message, boolean forceUpdate, boolean lockOperation)
    {
        dialogMessage = message;

        // 操作を禁止するかどうか
        if (lockOperation == true)
        {
        	confirmationMode = BUSY_MODE;
        }
        else
        {
        	confirmationMode = INFORMATION_MODE;
        }
        if (forceUpdate == true)
        {
        	// ダミーの画面遷移を発生させて強制的に更新
        	view.inputFire(false);
        }
        return;
    }

	/**
	 *  ログ出力メッセージのクリア
	 *  <em>(ログ出力するユーティリティクラスが使用します。)</em>
 	 *  
     *   @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
	 */
    public void clearInformationMessage()
    {
    	dialogMessage = null;
    	isButtonAccepted = true;
    }

    /**
     *  ログ出力の終了(操作禁止も解除)
 	 *  <em>(ログ出力するユーティリティクラスが使用します。)</em>
     *  
     *   @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
     */
    public void finishInformationMessage()
    {
        dialogMessage = null;
    	isButtonAccepted = true;
    	confirmationMode = INFORMATION_MODE;
    	view.inputFire(false);
    }
}
