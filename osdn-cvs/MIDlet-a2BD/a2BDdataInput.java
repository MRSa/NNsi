import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.ItemCommandListener;
import javax.microedition.lcdui.Spacer;
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextField;
import javax.microedition.lcdui.Choice;

public class a2BDdataInput   extends Form implements ItemCommandListener, CommandListener
{

	a2BDsceneSelector parent = null;

	private final int         MAX_DATAINPUT   = 1024;

	private final int         CMD_EXIT        = -1;
	private final int         CMD_SHOWHELP    = 100;
	private final int         CMD_DOEXECUTE   = 0;
	private final int         CMD_DOSELECION  = 1;

    private TextField          inputUserAgent  = null;
	private TextField          inputData1      = null;
	private TextField          inputData2      = null;
	private StringItem         writeButton     = null;
	private StringItem         selectButton    = null;
	private ChoiceGroup        choiceGroup     = null;

	private a2BDPreference     dataStorage     = null;
	
	private Command            helpCmd         = new Command("Help", Command.ITEM, CMD_SHOWHELP);
	private Command            exitCmd         = new Command("Cancel", Command.EXIT, CMD_EXIT);
	
	/**
	 * コンストラクタ
	 * @param selector
	 */
	public a2BDdataInput(String arg0, a2BDsceneSelector selector, a2BDPreference preference)
	{
		super(arg0);

		// 親クラスを設定する
		parent = selector;

        // データ保持クラスを設定する
		dataStorage = preference;

		// キャンセルコマンド、ヘルプコマンドをメニューに入れる
		this.addCommand(exitCmd);
		this.addCommand(helpCmd);
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

	/*
	 *   コマンドボタン実行時の処理
	 *   
	 */
	public void commandAction(Command c, Displayable d)
	{
		int command = c.getPriority();
		commandActionMain(command);
		return;
	}

	/*
	 *   コマンド実行メイン
	 * 
	 * 
	 */
	private void commandActionMain(int command)
	{
		if (command == CMD_EXIT)
		{
			// 実行キャンセル
			parent.EndDataInputForm(true);
			return;
		}
		if (command == CMD_DOEXECUTE)
		{
			// 入力データの確定...
			dataStorage.setData1(inputData1.getString());
            dataStorage.setData2(inputData2.getString());
			dataStorage.setUserAgent(inputUserAgent.getString());
			dataStorage.setWX310workaroundFlag(choiceGroup.isSelected(0));
            dataStorage.setDivideGetHttp(choiceGroup.isSelected(1));
            dataStorage.setAutoFinish(choiceGroup.isSelected(2));

			// 実行指示
			parent.EndDataInputForm(false);
		}
		if (command == CMD_DOSELECION)
		{
			parent.SelectFile(inputData2.getString());
		}
		if (command == CMD_SHOWHELP)
		{
			// ヘルプを表示する
			parent.ShowHelp();
		}
	}

	/**
	 * 入力データ２を設定する
	 * @return
	 */
	public void setData2(String data)
	{
		inputData2.setString(data);
	}

	/**
	 *   画面の準備
	 *
	 */
	public void prepareScreen(String title, String message, String message2, boolean select)
	{
		deleteAll();

		// タイトルの表示
		setTitle(title);

		// データの表示を行う
		inputData1 = new TextField(message, dataStorage.getData1(), MAX_DATAINPUT, TextField.ANY);
		append(inputData1);

		// スペーサー
		append(new Spacer(10, 10));

		if (message2 != null)
		{
			// データの表示を行う
			inputData2 = new TextField(message2, dataStorage.getData2(), MAX_DATAINPUT, TextField.ANY);
			append(inputData2);
		}
		
		if (select == true)
		{
			// 選択ボタンの追加
			selectButton = new StringItem("", "選択", Item.BUTTON);	
			selectButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
			selectButton.setDefaultCommand(new Command("選択", Command.SCREEN, CMD_DOSELECION));
			selectButton.setItemCommandListener(this);
			append(selectButton);

		}

		// スペーサー
		append(new Spacer(10, 30));

		// ユーザーエージェント
		inputUserAgent = new TextField("User-Agent", dataStorage.getUserAgent(), MAX_DATAINPUT, TextField.ANY);
		append(inputUserAgent);
		
		// スペーサー
		append(new Spacer(10, 30));

		// 選択グループ
		choiceGroup	= new ChoiceGroup("通信オプション", Choice.MULTIPLE);
		choiceGroup.append("WX310のgzip転送対策", null);
		choiceGroup.append("分割取得", null);
		choiceGroup.append("通信後自動終了", null);

        // 初期フラグの設定
		if (dataStorage.getWX310workaroundFlag() == true)
        {
			choiceGroup.setSelectedIndex(0, true);
        }
        if (dataStorage.getDivideGetHttp() == true)
        {
        	choiceGroup.setSelectedIndex(1, true);
        }
        if (dataStorage.getAutoFinish() == true)
        {
        	choiceGroup.setSelectedIndex(2, true);
        }
		append(choiceGroup);

		// スペーサー
		append(new Spacer(10, 20));
	
		// 実行ボタン
		writeButton  = new StringItem("", "実行", Item.BUTTON);
		writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
		writeButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_DOEXECUTE));
		writeButton.setItemCommandListener(this);
		append(writeButton);

		return;
	}
}
