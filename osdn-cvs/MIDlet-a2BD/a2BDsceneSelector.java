import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
/**
 * シーンセレクタ
 * @author MRSa
 *
 */
public class a2BDsceneSelector
{
	private a2BDmain  parent     = null;
	private busyForm  busyDialog = null;
	private Alert     alert      = null;

	
	public final int SCENE_UNKNOWN        = 0;
	public final int SCENE_OPEN_DATAINPUT = 1;
	public final int SCENE_DATAINPUT      = 2;
	public final int SCENE_EXECUTE_DATA   = 3;
	public final int SCENE_FILE_SELECT    = 4;
	public final int SCENE_SHOW_BUSY      = 900;

	private int currentScene = SCENE_UNKNOWN;
	
	/**
	 * コンストラクタ
	 * @param parent
	 */
	a2BDsceneSelector(a2BDmain object, busyForm busy)
	{
		alert = new Alert("");
		parent = object;		
		busyDialog = busy;
	}

	/**
	 *  初期画面にシーンを切り替える
	 *
	 */
	public void start()
	{
		changeScene(SCENE_OPEN_DATAINPUT);
		return;
	}

	/**
	 *  画面表示切替スレッド実行
	 *
	 */
	private void changeScene(int scene)
	{
		currentScene = scene;
		Thread thread = new Thread()
		{
			public void run()
			{
				parent.changeScene(currentScene);
			}
		};
		thread.start();
		return;
	}


	/**
	 * データ入力フォームを抜けたときの処理
	 * @param execute
	 */
	public void EndDataInputForm(boolean isCancel)
	{
		if (isCancel == true)
		{
			// 終了する
			currentScene = SCENE_UNKNOWN;
			parent.quit_a2BD();
			return;
		}
		currentScene = SCENE_EXECUTE_DATA;
		parent.changeScene(currentScene);
		return;
	}
	
	/**
	 * ビジーメッセージの表示
	 * @param title
	 * @param message
	 */
	public void showBusyMessage(String title, String message, String message2, String message3)
	{
        // 
		busyDialog.SetBusyMessage(title, message, message2, message3, false);
		return;
	}
	
	/**
	 * ビジーメッセージの解除
	 * 
	 */
	public void hideBusyMessage()
	{
		return;
	}
	
	/**
	 * HTTP通信の終了報告
	 * @param scene
	 * @param responseCode
	 * @param length
	 */
	public void CompletedToGetHttpData(int scene, int responseCode, int length)
	{
		String reply;
		
		// 応答コードの判定
		if (responseCode >= 200)
		{
			reply = "サーバからの応答：OK";
		}
		else
		{
			reply = "サーバからの応答：??";
		}
		
        // 通信終了を報告する
		busyDialog.SetBusyMessage("HTTP通信終了", reply, " (応答コード :" + responseCode + ")", "受信データサイズ :" + length, true);	
	}

	/** 
	 *  HTTP通信クラスで初回データを受け取ったときの処理...
	 * 
	 * 
	 */
	public int getHttpDataReceivedFirst(int scene, int responseCode, byte[] buffer, int length)
	{
		// 今回はここで何もしない。
		return (0);
	}

	/**
	 * ファイル名を選択したときの処理
	 * 
	 */
	public void EndFileSelector(String selectedFile)
	{
		if (selectedFile == null)
		{
			// ただキャンセルした場合...

			// 画面の切り替えを実施
			changeScene(SCENE_DATAINPUT);
			return;
		}
		
		// データを反映させる
		parent.setData2(selectedFile);
		
		
		// 画面の切り替えを実施
		changeScene(SCENE_DATAINPUT);
		return;
	}

	/**
	 * ファイル名の選択表示をする
	 * 
	 * @param fileName
	 */
	public void SelectFile(String fileName)
	{
		// 画面の切り替えを実施
		changeScene(SCENE_FILE_SELECT);
		return;
	}

	/**
	 *  操作説明を表示する
	 *
	 */
	public void ShowHelp()
	{
		String    message = "■a2BDとは？\n";
		message = message + "a2BDは、URLを指定してファイルをダウンロードするMIDletです。\n";
		message = message + "本来、a2Bの板一覧ファイル(bbstable.html)を単体で更新するためのソフトウェアですが、";
		message = message + "Javaによるhttp通信およびファイル記録のテストにもお使いいただけます。\n\n";
		message = message + "■使用方法\n";
        message = message + "□準備\n";
        message = message + "a2BDは、実行する前に取得するURLと記録するファイル名の指定が必要です。\n";
		message = message + "URL欄にデータを取得するURL、File欄に記録するファイル名を指定してください。\n";
        message = message + "『選択』ボタンは、押すとローカルファイルを指定するディレクトリ表示モードとなります。\n";
        message = message + "(既存ファイルを更新するときにご使用ください。)\n";
        message = message + "なお、ファイル名の指定が / で終わっていた場合は、そのディレクトリにURLの末尾ファイル名でダウンロードします。\n";
        message = message + "\n□実行\n";
		message = message + "『実行』ボタンを押してください。\nファイルのダウンロードを開始し、その結果を表示します。\n";
		message = message + "\n□通信結果の表示\n";
		message = message + "サーバからの応答と受信データサイズを表示します。\n\n";
		message = message + "■終了方法\n";
		message = message + "通信結果を表示している状態で「終了」選択してください。\n";
		message = message + "また通信オプションの「通信後自動終了」にチェックを入れておくと、通信終了後、10秒程度経過すると終了します。\n\n";
        message = message + "■その他\n";
        message = message + "a2BDは、MIDP2.0およびJSR75(fileConnection API)を利用しています。この機能が実装されていない場合にはご利用になれません。ご了承ください。\n\n";
        message = message + "WX310SA/WX310Jをお使いの場合には、通信オプションの「WX310のgzip転送対策」にチェックを入れてから実行してください。(WX320Kは不要です。)\n\n";
        message = message + "702NK/NKIIなどの機種をお使いの場合には、通信オプションの「分割取得」にチェックを入れてから実行してください。\n\n";
		showDialog("操作説明", message, 0);
		return;
	}
	
	
	/**
	 * ダイアログの表示
	 * @param message
	 */
	public void showDialog(String title, String message, int timeout)
	{
//		AlertType alertType = AlertType.INFO;
		AlertType alertType = null;

		// ダイアログ表示準備...
		alert.setTitle(title);
		alert.setString(message);
		alert.setType(alertType);

		// タイムアウト値がゼロ以外だったらタイムアウト値を設定する
		if (timeout != 0)
		{
			alert.setTimeout(timeout);
		}
		else
		{
			alert.setTimeout(Alert.FOREVER);
		}

		// ダイアログを画面表示する
		Displayable currentForm = (Display.getDisplay(parent)).getCurrent();
		Display.getDisplay(parent).setCurrent(alert, currentForm);
		
	}
}
