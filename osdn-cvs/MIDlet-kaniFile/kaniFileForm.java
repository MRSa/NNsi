import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.ItemCommandListener;
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextField;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Spacer;

/**
 * 入力フォームなど
 * @author MRSa
 *
 */
public class kaniFileForm extends Form implements ItemCommandListener, CommandListener
{
	static public final int  SCENE_MAKEDIR        = 100;
	static public final int  SCENE_RENAMEINPUT    = 101;
	static public final int  SCENE_DUPLICATEINPUT = 102;
	static public final int  SCENE_EDITATTRIBUTE  = 103;
	static public final int  SCENE_TEXTEDITLINE   = 104;

	private final int         CMD_EXIT        = -1;
	private final int         CMD_DOEXECUTE   = 0;
	private final int         CMD_LINEEDIT    = 1;

	private final int         maxString       = 128;
	private final int         maxLineString   = 3072;
	
	private kaniFileSceneSelector parent   = null;

	private kaniFileDirList.fileInformation targetInfo = null;
	private String                          targetName = null;

	private StringItem  okButton     = null;
	private StringItem  cancelButton = null;
	private TextField   txtField     = null;
	private ChoiceGroup attributes   = null;
	
	private Command  exitCmd  = new Command("Cancel", Command.EXIT, CMD_EXIT);
	
	/**
	 * コンストラクタ
	 * @param selector
	 */
	public kaniFileForm(String arg0, kaniFileSceneSelector selector)
	{
		super(arg0);

		// 親クラスを設定する
		parent = selector;

		// キャンセルコマンド、ヘルプコマンドをメニューに入れる
		this.addCommand(exitCmd);
		this.setCommandListener(this);
	}

	/**
	 *  コマンドリスナ実行時の処理
	 *  
	 */
	public void commandAction(Command c, Item item) 
	{
		int command = c.getPriority();
		commandActionMain(command);
		return;
	}

	/**
	 *   コマンドボタン実行時の処理
	 *   
	 */
	public void commandAction(Command c, Displayable d)
	{
		int command = c.getPriority();
		commandActionMain(command);
		return;
	}

	/**
	 *   コマンドの実行メイン
	 * 
	 */
	private void commandActionMain(int command)
	{
		if (command == CMD_EXIT)
		{
			// 実行キャンセル
			parent.cancelForm();
			return;
		}
		if (command == CMD_DOEXECUTE)
		{
			String fileName = null;
			int attribute = 0;
			if (attributes != null)
			{
				// 書き込み禁止属性...
				if (attributes.isSelected(0) == true)
				{
				    attribute = attribute + 1;
				}

				// 隠しファイル属性
				if (attributes.isSelected(1) == true)
				{
					attribute = attribute + 2;
				}
				fileName = targetName;
			}
			else
			{
				fileName = txtField.getString();
			}
			
			// 実行指示...
			parent.executeInputForm(fileName, attribute);
		}
		if (command == CMD_LINEEDIT)
		{
			// 一行編集の完了
			parent.commitLineEdit(txtField.getString());
		}

		// 一応、情報のクリア...
		targetInfo = null;
		targetName = null;
		attributes = null;
		return;
	}

	/**
	 * 表示を切り替える...
	 * @param scene
	 */
	public void changeScene(int scene, String message)
	{
		if (scene == SCENE_MAKEDIR)
		{
			prepareScreen(scene, "Dir作成", null, message);
		}
		if (scene == SCENE_RENAMEINPUT)
		{
			prepareScreen(scene, "リネーム", targetName, message);
		}
		if (scene == SCENE_DUPLICATEINPUT)
		{
			prepareScreen(scene, "ファイルの複製", targetName, message);			
		}
		if (scene == SCENE_EDITATTRIBUTE)
		{
			prepareScreenAttribute("ファイル属性変更", targetName);
		}
		if (scene == SCENE_TEXTEDITLINE)
		{
			prepareScreenEditTextLine("行編集(" + message + "行目)");
		}
		return;
	}	

	/**
	 * 画面表示の準備(クラス生成など...)を行う
	 *
	 */
	public void prepare()
	{
		return;
	}

	/**
	 * 画面表示の準備を行う
	 * (画面表示自体の利用方法)
	 *
	 */
	public void prepareScreen()
	{
		return;
	}

	/**
	 * ターゲット名を入れる
	 * @param name
	 */
	public void setTargetName(String name)
	{
		targetName = name;
		return;
	}	

	/**
	 * ファイル情報を入れる
	 * @param info
	 */
	public void setFileInfo(kaniFileDirList.fileInformation info)
	{
		targetInfo = info;
		targetName = info.getFileName();
		return;
	}
	
    /**
     * 画面表示の準備を行う
     * @param scene 画面(フォーム)の利用方法
     * @param title タイトル
     */
	public void prepareScreen(int scene, String title, String original, String data)
    {
    	// 画面をクリアする
		deleteAll();

		// タイトルの表示
		setTitle(title);

		// テキスト欄の表示
		txtField = new TextField("Name", original, maxString, TextField.ANY);
		append(txtField);
		
		// 実行ボタン
		okButton  = new StringItem("", "実行", Item.BUTTON);
		okButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
		okButton.setDefaultCommand(new Command("Execute", Command.SCREEN, CMD_DOEXECUTE));
		okButton.setItemCommandListener(this);
		append(okButton);

		// キャンセルボタン
		cancelButton  = new StringItem("", "中止", Item.BUTTON);
		cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
		cancelButton.setDefaultCommand(new Command("Cancel", Command.SCREEN, CMD_EXIT));
		cancelButton.setItemCommandListener(this);
		append(cancelButton);
		return;
    }

	/**
     * 画面表示の準備を行う
     * @param scene 画面(フォーム)の利用方法
     * @param title タイトル
     */
	public void prepareScreenAttribute(String title, String fileName)
    {
    	// 画面をクリアする
		deleteAll();

		// タイトルの表示
		setTitle(title);

		// ファイル名の設定
		StringItem msg = new StringItem("ファイル名 : ", targetInfo.getFileName());
		append(msg);
		
		// スペーサの設定
		Spacer sp = new Spacer(20, 20);
		append(sp);
		
		// ファイル属性の設定
		attributes = new ChoiceGroup("ファイル属性", Choice.MULTIPLE);
		attributes.append("書き込み禁止", null);
		attributes.append("隠しファイル", null);
        append(attributes);

        // 書き込み可否設定のチェック...
        if (targetInfo.isWritable() == false)
        {
        	attributes.setSelectedIndex(0, true);
        }

        // 隠し属性のチェック...
        if (targetInfo.isHidden() == true)
        {
        	attributes.setSelectedIndex(1, true);
        }
		
		// 実行ボタン
		okButton  = new StringItem("", "実行", Item.BUTTON);
		okButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
		okButton.setDefaultCommand(new Command("Execute", Command.SCREEN, CMD_DOEXECUTE));
		okButton.setItemCommandListener(this);
		append(okButton);

		// キャンセルボタン
		cancelButton  = new StringItem("", "中止", Item.BUTTON);
		cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
		cancelButton.setDefaultCommand(new Command("Cancel", Command.SCREEN, CMD_EXIT));
		cancelButton.setItemCommandListener(this);
		append(cancelButton);
		return;
    }

	/**
	 * 一行編集の画面選択
	 * 
	 * @param title
	 */
    private void prepareScreenEditTextLine(String title)
    {
    	deleteAll();

    	// タイトルの表示
		setTitle(title);

		// テキスト欄の表示
		txtField = new TextField("", targetName, maxLineString, TextField.ANY);
		append(txtField);

		// 実行ボタン
		okButton  = new StringItem("", "確定", Item.BUTTON);
		okButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
		okButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_LINEEDIT));
		okButton.setItemCommandListener(this);
		append(okButton);

		// キャンセルボタン
		cancelButton  = new StringItem("", "中止", Item.BUTTON);
		cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
		cancelButton.setDefaultCommand(new Command("Cancel", Command.SCREEN, CMD_EXIT));
		cancelButton.setItemCommandListener(this);
		append(cancelButton);
		return;

    }
	
}
