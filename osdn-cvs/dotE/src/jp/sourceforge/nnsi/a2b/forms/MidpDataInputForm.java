/**
 *  a2B Framework用のデータ入力画面パッケージです。
 */
package jp.sourceforge.nnsi.a2b.forms;
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

import jp.sourceforge.nnsi.a2b.frameworks.IMidpForm;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;

/**
 * データ入力フォーム
 * @author MRSa
 *
 */
public class MidpDataInputForm extends Form implements IMidpForm, ItemCommandListener, CommandListener
{
    private final int    CMD_EXIT = -1; // 入力キャンセル
    private final int    CMD_OK   = 0;  // 入力確定

    private MidpSceneSelector parent = null;
    private IDataInputStorage storageDb = null;

    private Command  okCmd    = new Command("OK", Command.ITEM, CMD_OK);
    private Command  exitCmd  = new Command("Cancel", Command.EXIT, CMD_EXIT);

    private StringItem okButton     = null;
    private StringItem cancelButton = null;

    private boolean   showOkButton = true;
    private boolean   showCancelButton = true;
    
    private int    formId = -1;
    private String  titleString = null;
    
    /**
     * コンストラクタでは必要なクラスを設定します
     * @param arg0 タイトル初期値
     * @param id   フォームID
     * @param selector シーンセレクタ
     * @param storage 画面入力データストレージ
     */
    public MidpDataInputForm(String arg0, int id, MidpSceneSelector selector, IDataInputStorage storage)
    {
        // 親クラスを設定する
        super(arg0);
        
        // 変数を設定する
        parent = selector;
        formId = id;
        storageDb = storage;

        // キャンセルコマンド、OKコマンドをメニューに入れる
        this.addCommand(exitCmd);
        this.addCommand(okCmd);
        this.setCommandListener(this);
    }

    /**
     *  コマンドリスナ実行時の処理
     *  @param c コマンド
     *  @param item アイテム
     */
    public void commandAction(Command c, Item item) 
    {
        int command = c.getPriority();
        commandActionMain(command);
        return;
    }

    /**
     *   画面タイトルを設定する
     *   @param title 画面タイトル
     */
    public void setTitleString(String title)
    {
        titleString = title;
    }
    
    /**
     *  画面フォームIDを取得する
     *  @return 画面ID
     */
    public int getFormId()
    {
        return (formId);
    }

    /**
     *   コマンドボタン実行時の処理
     *   @param c コマンド
     *   @param d 画面クラス
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
            // 入力キャンセル指示を受け付けた
            storageDb.cancelEntry();
        }
        else if (command == CMD_OK)
        {
            // データ格納指示を受け付けた
            storageDb.dataEntry();
        }
        else
        {
            // 上記のコマンドではない場合...
            titleString = "[" + command + "]";
            return;
        }

        // 画面終了処理
        finishForm();
        return;
    }

    /**
     *  画面表示の準備を行う
     *
     */
    public void prepare()
    {
        // 特に何もしない
        return;
    }

    /**
     *  画面表示の開始
     *
     */
    public void start()
    {
        // ストレージクラスに準備を依頼
        storageDb.prepare();
        
        // 画面を全部クリアする
        deleteAll();

        // タイトルを設定する
        setTitle(titleString);
        
        // 入力フィールド
        int nofField = storageDb.getNumberOfDataInputField();
        for (int index = 0; index < nofField; index++)
        {
            TextField inputField = storageDb.getDataInputField(index);
            if (inputField != null)
            {
                append(inputField);

                // スペーサー
                append(new Spacer(0, 10));
            }
        }

        // OKボタン
        if (showOkButton == true)
        {
            okButton  = new StringItem("", "OK", Item.BUTTON);
            okButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
            okButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_OK));
            okButton.setItemCommandListener(this);
            append(okButton);
        }
        else
        {
            okButton = null;
        }

        // スペースを空ける...
        append(new Spacer(0, 10));

        // 画面へオプションを設定する
        int nofOption = storageDb.getNumberOfOptionGroup();
        for (int index = 0; index < nofOption; index++)
        {
            ChoiceGroup choice = storageDb.getOptionField(index);
            if (choice != null)
            {
                append(new Spacer(0, 10));
                append(choice);
            }
        }

        // スペースを空ける...
        append(new Spacer(0, 20));

        // Cancelボタン
        if (showCancelButton == true)
        {
            cancelButton  = new StringItem("", "Cancel", Item.BUTTON);
            cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
            cancelButton.setDefaultCommand(new Command("Cancel", Command.SCREEN, CMD_EXIT));
            cancelButton.setItemCommandListener(this);
            append(cancelButton);
        }
        else
        {
            cancelButton = null;
        }
        return;
    }

    /**
     *  画面表示のクリーンアップ
     * 
     */
    protected void finishForm()
    {
        // オブジェクトのあとしまつ
        okButton = null;
        cancelButton = null;
        storageDb.finish();

        int nextScene = storageDb.nextSceneToChange(formId);
        if (nextScene == IDataInputStorage.SCENE_TO_PREVIOUS)
        {
            // 前画面へ戻る
            parent.previousScene();
        }
        else
        {
            // 他画面へ切り替える
            parent.changeScene(nextScene, storageDb.nextSceneTitleToChange(formId));
        }
        System.gc();
        return;
    }

    /**
     *   OKボタンを用意するか？
     * 
     *   @param isShow 表示する(true) / 表示しない(false)
     */
    public void setOkButton(boolean isShow)
    {
        showOkButton = isShow;
        return;
    }

    /**
     *   Cancelボタンを用意するか？
     * 
     *   @param isShow 表示する(true) / 表示しない(false)
     */
    public void setCancelButton(boolean isShow)
    {
        showCancelButton = isShow;
        return;
    }
}
