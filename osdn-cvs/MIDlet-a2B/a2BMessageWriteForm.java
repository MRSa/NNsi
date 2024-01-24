import java.io.InputStream;
import java.io.OutputStream;
import java.util.Date;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Spacer;
import javax.microedition.lcdui.ItemCommandListener;
import javax.microedition.lcdui.TextField;
import javax.microedition.lcdui.TextBox;
import javax.microedition.lcdui.StringItem;
//import java.util.Enumeration;
//import javax.microedition.lcdui.AlertType;
//import a2BNgWordManager.a2BngWord;

/**
 *  a2BMessageWriteForm
 *
 */
public class a2BMessageWriteForm  extends Form implements ItemCommandListener, CommandListener
{
    private a2BMain                 parent      = null;
    private a2BsjConverter          sjConverter = null;
    private a2Butf8Conveter         utf8Converter = null;
    private a2BConfirmationListener listener    = null;
    private Alert                   alert       = null;

    private final int         maxTitle        = 64;
    private final int         maxEmail        = 64;
    private final int         maxName         = 64;
    private final int         maxUrl          = 256;
    private final int         maxMessage      = 6400;
    private final int         MARGIN          = 10;
    private final long        TIME_OFFSET     = 50;

    private final int         CMD_EXIT        = -1;
    private final int         CMD_DOEXECUTE   = 0;
//    private final int         CMD_OYSTERLOGIN = 1;
    private final int         CMD_APPLYLIST   = 2;
    private final int         CMD_DONE        = 3;
    private final int         CMD_SETPROXY    = 4;
    private final int         CMD_WRITEFILE   = 5;
    private final int         CMD_LOADFILE    = 6;
    private final int         CMD_OPERATION   = 7;
    private final int         CMD_RESERVE     = 8;
    private final int         CMD_GETLOG      = 9;
    private final int         CMD_DETAIL      = 10;
    private final int         CMD_RESET_FONT  = 11;
    private final int         CMD_HTTP_COMM   = 12;
    private final int         CMD_DETAIL_VIEW = 13;
    private final int         CMD_DOWRITE     = 14;

    private final int         SCENE_WRITE     = 0;
    private final int         SCENE_INFORM    = 1;
//    private final int         SCENE_MEMOFORM  = 2;
//    private final int         SCENE_THREOPE   = 3;

    private int                messageIndex    = 0;
    private boolean           sendTitle       = false;
    private boolean           isFileLoaded    = false;
    private boolean           isGetNumber     = false;
    private boolean           isWriting       = false;

    private TextField          writeName       = null;
    private TextField          writeEmail      = null;
    private TextField          writeTitle      = null;
    private TextField          writeMessage    = null;
    private TextBox            msgBox          = null;
    private TextField          writeCount      = null;
    private TextField          aaWidth         = null;
    private TextField          keyRepeatMs     = null;
    private TextField          oysterName      = null;
    private TextField          oysterPass      = null;
    private TextField          beUserName      = null;
    private TextField          bePassword      = null;
    private TextField          divideSize      = null;
    private TextField          fontIdNumber    = null;

    private StringItem         writeButton     = null;
    private StringItem         cancelButton    = null;
    private StringItem         saveButton      = null;
    private StringItem         loadButton      = null;
    private StringItem         detailButton    = null;

    private ChoiceGroup        choiceGroup     = null;
//    private ChoiceGroup        choiceComm      = null;
    private ChoiceGroup        choiceOptFavor  = null;
    private ChoiceGroup        choiceListFunc  = null;
    private ChoiceGroup        choiceViewFunc  = null;
    private ChoiceGroup        useOysterInfo   = null;

    private String             targetTitle     = null;
    private String             receivedTitle   = null;
    private String             currentCookie   = "";
    private String             extendString    = "";
//    private String             temporaryMessage = null;

    private int      scene           = 0;
    private int      pickupHour      = 0;
    private boolean  returnToMessage = false;

    private Command  execCmd    = new Command("実行",     Command.ITEM, CMD_DOEXECUTE);
    private Command  exitCmd    = new Command("Cancel",   Command.EXIT, CMD_EXIT);
    private Command  opeCmd     = new Command("操作",     Command.SCREEN, CMD_OPERATION);
    private Command  reserveCmd = new Command("取得予約", Command.SCREEN, CMD_RESERVE);
    private Command  fontCmd    = new Command("font初期化", Command.SCREEN, CMD_RESET_FONT);
    private Command  getCmd     = new Command("スレ取得", Command.SCREEN, CMD_GETLOG);
    private Command  okCmd      = new Command("Done",     Command.ITEM, CMD_DOEXECUTE);
    private Command  writeCmd   = new Command("書込",     Command.EXIT, CMD_DOWRITE);
    private Command  doneCmd    = new Command("OK",       Command.ITEM, CMD_DONE);
    private Command  httpCmd    = new Command("http通信", Command.ITEM, CMD_HTTP_COMM);

    /**
     *   コンストラクタ
     *   
     */
    public a2BMessageWriteForm(String arg0, a2BMain object, a2BsjConverter converterSJ, a2Butf8Conveter converterUTF8)
    {
        super(arg0);

        // 親クラス
        parent = object;

        // UTF8 => Shift JIS変換ルーチン
        sjConverter = converterSJ;
        
        // Shift JIS => UTF8変換ルーチン
        utf8Converter = converterUTF8;
        
        // コマンドリスナ
        listener = new a2BConfirmationListener(this);

        // アラートダイアログ
        alert = new Alert("");
        alert.setType(null);
        alert.setTimeout(Alert.FOREVER);
        alert.setCommandListener(listener);

        // キャンセルコマンドをメニューに入れる
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
     *  書き込み画面の終了処理...
     *
     */
    private void finishWriteForm(int afterAction)
    {
        // 書き込み画面の終了...
    	fontIdNumber = null;
        divideSize = null;
        aaWidth    = null;
        keyRepeatMs = null;
        writeCount = null;
        oysterName = null;
        oysterPass = null;
        beUserName = null;
        bePassword = null;
        writeTitle = null;
        writeEmail = null;
        writeMessage = null;
        writeButton = null;
        useOysterInfo = null;
        saveButton  = null;
        loadButton  = null;
        detailButton = null;
        cancelButton = null;
        if (isFileLoaded == true)
        {
            saveTemporary();
        }
        isFileLoaded = false;
        sendTitle    = false;
        parent.returnToPreviousForm(afterAction);
        System.gc();
        return;
    }
    
    /**
     *   コマンド実行メイン
     * 
     */
    private void commandActionMain(int command)
    {
        if (command == CMD_EXIT)
        {
            // 書き込み画面の終了... (キャンセル終了)
            this.removeCommand(opeCmd);
            this.removeCommand(reserveCmd);
            this.removeCommand(getCmd);
            this.removeCommand(fontCmd);
            this.removeCommand(httpCmd);
            isFileLoaded = false;   // テンポラリファイルは消去しない
            if (isGetNumber == true)
            {
                parent.setFavorThreadLevel((Integer.valueOf(writeName.getString())).intValue());
            }
            isGetNumber  = false;
            finishWriteForm(parent.ACTION_NOTHING_CANCEL);
            return;
        }
        if (command == CMD_DOEXECUTE)
        {
            // 書き込み前の確認実施...
            if (isWriting == true)
            {
                // 書き込み中の場合には、再度書き込みさせない
                return;
            }
            parent.setWriteName(writeName.getString());
            if (sendTitle == false)
            {
                confirmation(SCENE_WRITE, "書き込み確認", targetTitle, "に書き込みます。よろしいですか？");
            }
            else
            {
                confirmation(SCENE_WRITE, "スレ立て確認", targetTitle, "にスレを立てます。よろしいですか？");
            }
            return;
        }
        if (command == CMD_APPLYLIST)
        {
            boolean ignoreCase   = false;
            boolean useGoogleSearch = false;
            boolean use2chSearch = false;
            if (useOysterInfo != null)
            {
                // 大文字/小文字を区別する検索か?
                ignoreCase = useOysterInfo.isSelected(0);

                // googleで検索するか?
                useGoogleSearch = useOysterInfo.isSelected(1);

                if (useOysterInfo.size() > 2)
                {
                    // 2ちゃんねる検索を利用するか？
                    use2chSearch = useOysterInfo.isSelected(2);
                }
            }

            // スレ立て後抽出時間
            if (writeCount != null)
            {
                pickupHour = (Integer.valueOf(writeCount.getString())).intValue();
            }
            
            // 文字列の設定
            parent.setInputData(scene, writeTitle.getString(), a2BsubjectDataParser.STATUS_NOTSPECIFY, ignoreCase, use2chSearch, useGoogleSearch, pickupHour);

            // 書き込み画面の終了...
            finishWriteForm(parent.ACTION_NOTHING);
            return;
        }
        if (command == CMD_DONE)
        {
            // 書き込み画面の終了...
            finishWriteForm(parent.ACTION_NOTHING);
            return;
        }
        if (command == CMD_SETPROXY)
        {
            // proxyデータの設定
            parent.setProxyUrl(scene, writeTitle.getString(), choiceGroup.getSelectedIndex());

            // ●ユーザ名＆パスワードの設定
            parent.setOysterName(oysterName.getString());
            parent.setOysterPassword(oysterPass.getString());

            // Be@2chのユーザ名＆パスワードの設定
            if (beUserName != null)
            {
                parent.setBeName(beUserName.getString());
            }
            if (bePassword != null)
            {
                parent.setBePassword(bePassword.getString());
            }
            
            parent.setFunctionKeyFeature(parent.SCENE_LISTFORM, choiceListFunc.getSelectedIndex());
            parent.setFunctionKeyFeature(parent.SCENE_VIEWFORM, choiceViewFunc.getSelectedIndex());

            // 起動時に開く画面をお気に入り一覧にする設定
            parent.setLaunchAsFavoriteList(choiceOptFavor.isSelected(0));

            // ブラウザ起動後のアクションを設定する
            parent.setLaunchMode(choiceOptFavor.isSelected(1));

            // 次スレ取得時にお気に入りへ登録するフラグを登録する
            parent.setAutoEntryFavorite(choiceOptFavor.isSelected(2));

            // ファイルダウンロード時、保存ディレクトリをルートディレクトリにする
            parent.setSaveAtRootDirectory(choiceOptFavor.isSelected(3));

            // スレ分割取得モードを設定する
            parent.setDivideGetHttp(choiceOptFavor.isSelected(4));

            // スレタイを一覧下部に表示する
            parent.setShowTitleAnotherArea(choiceOptFavor.isSelected(5));

            // subject.txtを保管する...
            parent.setBackupSubjectTxt(choiceOptFavor.isSelected(6));

            // お気に情報保管回数の削減...
            parent.setIgnoreFavoriteBackup(choiceOptFavor.isSelected(7));

            // ブラウザで開くときgoogleプロキシを使用する
            parent.setUseGoogleProxy(choiceOptFavor.isSelected(8));

            // ファイル取得時、WX220JのUAを使用する
            parent.setWX220JUserAgent(choiceOptFavor.isSelected(9));

            // ファイル取得時、キャッシュファイルがあればそれを利用する            
            parent.setUseCachePictureFile(choiceOptFavor.isSelected(10));

            // カーソルの動きをページ送りに
            parent.setCursorSingleMovingMode(choiceOptFavor.isSelected(11));
            
            // 自前の漢字変換を利用する
            parent.setForceKanjiConvertMode(choiceOptFavor.isSelected(12));
            
            // 無操作終了時間の設定
            parent.setWatchDogTime((Integer.valueOf(writeName.getString())).intValue());

            // レスまとめ読み時のレス数
            int resCount = (Integer.valueOf(writeCount.getString())).intValue();
            if ((resCount < 1)||(resCount > 9))
            {
                resCount = 3;
            }
            parent.setResBatchCount(resCount);

            // AAモード横幅移動量
            parent.setAAwidthMargin((Integer.valueOf(aaWidth.getString())).intValue());

            // キーリピート検出時間(ms)
            parent.setKeyRepeatTime((Integer.valueOf(keyRepeatMs.getString())).intValue());

            // ユーザーエージェント
            parent.setUserAgent(writeEmail.getString());
            
            // 分割取得サイズ
            parent.setDivideGetSize((Integer.valueOf(divideSize.getString())).intValue());
            
            // フォントサイズの更新
            int fontId = Integer.valueOf(fontIdNumber.getString()).intValue();
            if (fontId >= 0)
            {
                parent.updateFontData(false, fontId);
            }
            
            // フォントのコマンドを削除
            this.removeCommand(fontCmd);

            // 書き込み画面の終了...
            finishWriteForm(parent.ACTION_NOTHING);
            return;
        }
        if (command == CMD_WRITEFILE)
        {
            // ファイルにデータを仮保存する
            saveTemporary();
            return;
        }
        if (command == CMD_LOADFILE)
        {
            // ファイルからデータを読み出す
            loadTemporary();
            return;
        }
        if (command == CMD_DETAIL)
        {
            // 全画面ボタンが押された...
            String data = "";
            if (writeMessage != null)
            {
                data = writeMessage.getString();
            }
            else if (writeTitle != null)
            {
                data = writeTitle.getString();
            }
            if (maxMessage < data.length())
            {
                data = data.substring(0, (maxMessage - 2));
            }
            msgBox = new TextBox("全画面", data, maxMessage, TextField.ANY);
            scene = SCENE_INFORM;
            msgBox.addCommand(writeCmd);
            msgBox.addCommand(doneCmd);
//            msgBox.addCommand(exitCmd);
            msgBox.setCommandListener(listener);
            parent.callScreen(msgBox);
            return;
        }
        if (command == CMD_DETAIL_VIEW)
        {
            // 全画面ボタンが押された...
            String data = "";
            if (writeTitle != null)
            {
                data = writeTitle.getString();
            }
            else if (writeMessage != null)
            {
                data = writeMessage.getString();
            }
            if (maxMessage < data.length())
            {
                data = data.substring(0, (maxMessage - 2));
            }
            msgBox = new TextBox("全画面", data, maxMessage, TextField.ANY);
            scene = SCENE_INFORM;
            msgBox.addCommand(exitCmd);
            msgBox.setCommandListener(listener);
            parent.callScreen(msgBox);
            return;
        }
        if (command == CMD_OPERATION)
        {
            // スレ操作
            this.removeCommand(opeCmd);
            this.removeCommand(httpCmd);
            isFileLoaded = false;   // テンポラリファイルは消去しない
            finishWriteForm(parent.ACTION_THREAD_OPERATION);
            return;
        }
        if (command == CMD_RESERVE)
        {
            // スレ取得予約
            this.removeCommand(reserveCmd);
            this.removeCommand(getCmd);
            isFileLoaded = false;   // テンポラリファイルは消去しない
            finishWriteForm(parent.ACTION_THREAD_RESERVE);
            return;
        }
        if (command == CMD_GETLOG)
        {
            // スレ取得
            this.removeCommand(reserveCmd);
            this.removeCommand(getCmd);
            isFileLoaded = false;   // テンポラリファイルは消去しない
            finishWriteForm(parent.ACTION_THREAD_GETLOG);
            return;
        }
        if (command == CMD_RESET_FONT)
        {
            // フォント情報のリセット
            parent.updateFontData(true, 0);
            this.removeCommand(fontCmd);
            return;
        }
        if (command == CMD_HTTP_COMM)
        {
            // HTTP通信...
            httpCommunication();

            // 通信のメニューを削除する..
            this.removeCommand(httpCmd);
            return;
        }
        return;
    }

    /**
     *   書き込み画面の準備を行う
     *   
     */
    public void prepareWriteMessageForm(boolean useTitle, String formTitle, int index, int number)
    {
        deleteAll();

        // 書き込み用Cookieの読み出し
        currentCookie = parent.getWriteCookie();
        
        sendTitle    = useTitle;
        messageIndex = index;

        // 書き込みタイトルの表示
        targetTitle = formTitle;
        setTitle(formTitle);
        
        // タイトル欄の表示
        if (useTitle == true)
        {
            writeTitle = new TextField("タイトル", "", maxTitle, TextField.ANY);
            append(writeTitle);
        }
        else
        {
            writeTitle = null;
        }

        // 名前欄の表示
        String nameData = parent.getWriteName();
        writeName = new TextField("【名前】", nameData, maxName, TextField.ANY);
//        writeName.setInitialInputMode("IS_KANJI");

        // E-mail欄の表示
        writeEmail = new TextField("【E-mail】", "sage", maxEmail, TextField.ANY);

        // 本文欄の表示
        String msg = "";
        if (number > 0)
        {
            // レス参照番号をデフォルトで入れるようにする
            msg = ">>" + number;
        }
        writeMessage = new TextField("【本文】", msg, maxMessage, TextField.ANY);
//      writeMessage.setConstraints(TextField.ANY);
//      writeMessage.setInitialInputMode("IS_KANJI");
//      writeMessage.setInitialInputMode("UCB_HIRAGANA");
        writeMessage.setLayout(Item.LAYOUT_2|Item.LAYOUT_VEXPAND|Item.LAYOUT_NEWLINE_AFTER);
//        writeMessage.setPreferredSize(-1, -1);

        // 全画面ボタン
        detailButton = new StringItem("", "全画面", Item.BUTTON);
        detailButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        detailButton.setDefaultCommand(new Command("DETAIL", Command.SCREEN, CMD_DETAIL));
        detailButton.setItemCommandListener(this);
    
        // 書き込みボタン
        writeButton  = new StringItem("", "書き込む", Item.BUTTON);
        writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        writeButton.setDefaultCommand(new Command("WRITE", Command.SCREEN, CMD_DOEXECUTE));
        writeButton.setItemCommandListener(this);

/**
        // キャンセルボタン (誤操作防止のため削除する
        cancelButton = new StringItem("", "キャンセル", Item.BUTTON);
        cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        cancelButton.setDefaultCommand(new Command("CANCEL", Command.EXIT, CMD_EXIT));
        cancelButton.setItemCommandListener(this);
**/

        // スペーサー
        Spacer sp = new Spacer(10, 10);

        // ファイル仮保存ボタン
        saveButton  = new StringItem("", "(仮保存)", Item.BUTTON);
        saveButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        saveButton.setDefaultCommand(new Command("SAVE", Command.SCREEN, CMD_WRITEFILE));
        saveButton.setItemCommandListener(this);

        // ファイル読み込みボタン
        loadButton  = new StringItem("", "(読込)", Item.BUTTON);
        loadButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        loadButton.setDefaultCommand(new Command("READ", Command.SCREEN, CMD_LOADFILE));
        loadButton.setItemCommandListener(this);

        // Formを構築する...
        append(writeName);
        append(writeEmail);
        append(writeMessage);

        // 詳細ボタン
        append(detailButton);
        
        String sessionId = parent.getOysterSessionId(0);
        String be2chCookie = parent.getBe2chCookie(0);

        // 選択グループ
        useOysterInfo    = new ChoiceGroup("通信オプション", Choice.MULTIPLE);
        useOysterInfo.append("Cookie消去後、書込", null);
        useOysterInfo.setSelectedIndex(0, false);
        if ((sessionId != null)||(be2chCookie != null))
        {
            if (sessionId != null)
            {
                useOysterInfo.append("2chログイン情報の利用", null);
                useOysterInfo.setSelectedIndex(1, true);
            }
            else
            {
                useOysterInfo.append("-----", null);
                useOysterInfo.setSelectedIndex(1, false);
            }
            if (be2chCookie != null)
            {
                useOysterInfo.append("Be@2chログイン情報の利用", null);
                useOysterInfo.setSelectedIndex(2, true);
            }
            else
            {
                useOysterInfo.append("-----", null);
                useOysterInfo.setSelectedIndex(2, false);
            }
        }
        append(useOysterInfo);

        append(writeButton);
//        append(cancelButton);
        append(sp);
        append(saveButton);
        append(loadButton);

        return;
    }
    
    /**
     *   書き込み結果を表示する
     * 
     */
    private void completedWriteMessage(boolean returnToForm, String result)
    {

        // BUSYダイアログを隠す
        parent.hideBusyMessage();
        parent.updateBusyMessage("", "", false);

        // 通信結果の表示
        information("書き込み結果", "", result);

        // 書き込み画面の終了設定...
        if (returnToForm == true)
        {
            returnToMessage = true;
        }
        else
        {
            returnToMessage = true;
        }

        // 書き込み用Cookieを記憶する
        parent.setWriteCookie(currentCookie);
        return;
    }

    /**
     * ファイルへデータを出力する
     * 
     * @param fileName 出力ファイル名
     * 
     */
    private void saveToFile(String writeFileName)
    {
        FileConnection connection = null;
        OutputStream  out = null;
        
        // ファイル名を生成する
        String fileName = parent.getBaseDirectory() + writeFileName;
        try 
        {
            // ファイルの書き込み
            connection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
            if (connection.exists() == true)
            {
                // ファイルが存在した場合には、一度消して作り直す
                connection.delete();
            }
            if ((writeTitle == null)&&(writeMessage == null))
            {
                // 書き込むデータがない場合...ファイルを消去して終了する
                connection.close();
                return;
            }
            connection.create();
            out = connection.openOutputStream();

            // フィールドにあるデータを出力する
            if (writeTitle == null)
            {
                String crlf = "\r\n";
                out.write((writeName.getString()).getBytes());
                out.write(crlf.getBytes());
                out.write((writeEmail.getString()).getBytes());
                out.write(crlf.getBytes());
                out.write((writeMessage.getString()).getBytes());
            }
            else
            {
                out.write((writeTitle.getString()).getBytes());
            }
            out.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(writeFile)", " Exception  ee:" + e.getMessage());
            try
            {
                if (out != null)
                {
                    out.close();
                }
            }
            catch (Exception e2)
            {
            
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
            
            }
        }
        return;        
    }

    /**
     *  ファイルからのデータ読み出しメイン処理
     * 
     * 
     */
    private void loadFromFile(String loadFileName)
    {
        FileConnection connection    = null;
        InputStream    in            = null;
        byte[]         buffer        = null;
        
        // ファイル名を生成する
        String fileName = parent.getBaseDirectory() + loadFileName;
        try
        {
            connection   = (FileConnection) Connector.open(fileName, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  データを最初から size分読み出す...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size];
            in.read(buffer, 0, size);
            in.close();
            connection.close();

            // 読み込んだデータをセットする
            if (buffer.length != 0)
            {
                try
                {
                    String msgString = new String(buffer);
                    int pos1 = msgString.indexOf("\r\n", 0);
                    int pos2 = -1;
                    if (pos1 >= 0)
                    {
                        pos2 = msgString.indexOf("\r\n", pos1 + 1);
                    }
                    
                    if (pos1 >= 0)
                    {
                        if (pos2 >= 0)
                        {
                            writeEmail.setString  (msgString.substring(pos1 + 2, pos2));
                            writeMessage.setString(msgString.substring(pos2 + 2));
                        }
                        else
                        {
                            writeMessage.setString(msgString.substring((pos1 + 2)));
                        }
                        writeName.setString(msgString.substring(0, pos1));
                    }
                     else
                    {
                        writeMessage.setString(new String(buffer));
                    }
                 }
                catch (Exception e)
                {
                    writeMessage.setString(new String(buffer));                    
                }
            }
            buffer  = null;
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(loadFile)", fileName + " EXCEption  e:" + e.getMessage());
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                //            
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            buffer = null;
        }
        System.gc();
        return;
    }
    
    /**
     *   ファイルからの読み出しを実施する
     * 
     */
    private void loadTemporary()
    {
        isFileLoaded = true;  // ファイルが読み込まれたよフラグをたてる
        Thread thread = new Thread()
        {
            public void run()
            {
                loadFromFile("temp.txt");
            }
        };
        thread.start();
        return;
    }    

    /**
     *   ファイルへの仮保存を実施する
     *
     */
    private void saveTemporary()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                saveToFile("temp.txt");
            }
        };
        thread.start();        
        return;
    }    

    /**
     *   HTTP通信でデータ取得を実行
     *
     */
    private void httpCommunication()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                String url = pickupUrl(writeTitle.getString());
                if (url != null)
                {
                    // URLの抽出に成功したときだけhttp通信を実行する...
                    String fileName = "index.html";
                    int pos = url.lastIndexOf('/');
                    if (url.length() > (pos + 1))
                    {
                        fileName = url.substring(pos + 1);
                        pos = fileName.indexOf("?");
                        if (pos >= 0)
                        {
                            fileName = fileName.substring(pos + 1) + ".html";
                        }
                        else
                        {
                            pos = fileName.indexOf("*");
                            if (pos >= 0)
                            {
                                fileName = fileName.substring(pos + 1) + ".html";                                
                            }
                        }
                    }
                    String directory = parent.getBaseDirectory();
                    if (parent.getSaveAtRootDirectory() != true)
                    {
                        directory = parent.getFileSaveDirectory();
                        parent.makeDirectory(directory);
                        directory = directory + "/";
                    }
                    else
                    {
                        // bbstable.html を上書きするような処理は禁止する。。。
                        if (fileName.indexOf("bbstable.html") >= 0)
                        {
                            System.gc();
                            parent.vibrate();
                            addCommand(httpCmd); // http通信ボタンの追加...
                            return;
                        }
                    }
                    fileName = directory + fileName;
                    parent.doHttpMain(url, fileName, null, false,  a2BMain.a2B_HTTPMODE_NOUSECOOKIE);
                    System.gc();
                    parent.vibrate();
                    addCommand(httpCmd); // http通信ボタンの追加...
                }
            }
        };
        thread.start();        
        return;        
    }
    
    /**
     * フォーム内にあるURLを抽出する
     * 
     * @param data
     * @return
     */
    private String pickupUrl(String data)
    {
        String url = "";
        int pos = data.indexOf("://");
        if (pos < 0)
        {
            return (null);
        }
        pos = pos + 3;
        int cnt = pos;
        for (; cnt < data.length(); cnt++)
        {
            char ch = data.charAt(cnt);
            if ((ch <= ' ')||(ch == '"')||(ch == '>')||(ch == '\''))
            {
                break;
            }
        }
        url = data.substring(pos, cnt);
        return ("http://" + url);
    }
    
    /**
     *   書き込みの実処理...
     * 
     */
    private void    doWrite()
    {
        // URLとファイル名を取得する
        String      url = parent.getBoardURL(messageIndex);
        String fileName = parent.getThreadFileName(messageIndex);
        String message  = "";
//        String confirmMessage = "&suka=pontan";
//        String confirmMessage = "&tepo=don";
        String confirmMessage = "&kuno=ichi";
        receivedTitle = "";

        // 取得先および取得ファイル名を生成する
        int pos = url.lastIndexOf('/', url.length() - 2);
        int logpos = fileName.lastIndexOf('.');

        // 送信先URL
        String sendUrl = url.substring(0, pos) + "/test/bbs.cgi?guid=ON";

        Date dateTime = new Date();
        long currentTime = (dateTime.getTime() / 1000) - 180;
        
        // 書き込みログにメッセージを書き出す
        parent.outputTextFile("writelog.txt", dateTime.toString() + "\r\n" + targetTitle + "\r\n", writeMessage.getString(), true);

        // 送信データの準備
        String titleBuffer = "";
        if (sendTitle != false)
        {
            titleBuffer = sjConverter.parseToSJAndUrlEncode(writeTitle.getString());
        }
        String emailBuffer = sjConverter.parseToSJAndUrlEncode(writeEmail.getString());
        String nameBuffer  = sjConverter.parseToSJAndUrlEncode(writeName.getString());
        String msgBuffer   = sjConverter.parseToSJAndUrlEncode(writeMessage.getString());

        // セッションID と Be2chログインCookieを取得する
        String sessionId = null;
        String be2chLoginId = null;
        if (useOysterInfo != null)
        {
            // Cookieを消去後に書き込みを行う
        	if (useOysterInfo.isSelected(0) == true)
            {
            	currentCookie = "";
            }
            try
            {
                // ログイン中の場合...
                String orgSessionId = parent.getOysterSessionId(1);
                if ((useOysterInfo.isSelected(1) == true)&&(orgSessionId != null))
                {
                    // nullじゃなかったらURLエンコードを実施する
                    sessionId = a2BsjConverter.urlEncode(orgSessionId.getBytes());
                }
                
                String orgBe2chId = parent.getBe2chCookie(2);
                if ((useOysterInfo.isSelected(2) == true)&&(orgBe2chId != null))
                {
                    be2chLoginId = orgBe2chId;
                    // nullじゃなかったらURLエンコードを実施する
                    // sessionId = sjConverter.urlEncode(orgSessionId.getBytes());
                }
            }
            catch (Exception ex)
            {
            	// 格好悪いが、、、 IndexOutOfBoundsException対策。
            	// （ちゃんと判定しとけよー自分）
            }
        }

        String referer = "";
        if (sendTitle != false)
        {
            // 送信用メッセージ(スレ立て)を作成する...
            referer = url + "test/bbs.cgi/";
            message = "subject=" + titleBuffer;
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + confirmMessage + "&time=" + currentTime + "&submit=%91S%90%D3%94C%82%F0%95%89%82%A4%82%B1%82%C6%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";
        }
        else
        {
            // 送信用メッセージ(レス)を作成する...
            referer = url.substring(0, pos) + "/test/read.cgi/" + url.substring((pos + 1), (url.length())) + fileName.substring(0, logpos) + "/";
            message = "key=" + fileName.substring(0, logpos);
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + confirmMessage + "&time=" + currentTime + "&submit=%8F%E3%8B%4C%91%53%82%C4%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";
        }
        if (sessionId != null)
        {
            message = message + "&sid=" + sessionId;
        }
        receivedTitle = message;

/**
        // 書き込みログにメッセージを書き出す
        parent.outputTextFile("writelog.txt", "DEBUG", message, true);
**/

        extendString  = "";
        receivedTitle = "";

        // cookie を拾うために空打ち
        postHttpMain(sendUrl, referer, getCookie(be2chLoginId, ""), message);

/**/
        if ((sessionId != null)&&(sendTitle != false))
        {
            // ●ログイン中のスレ立ては、２度書く必要はないようだ...
            // ここで書き込み画面を終了させる
            completedWriteMessage(returnToMessage, receivedTitle);
            return;
        }
/**/
        //  ここで応答コードをチェックする
        if (checkIsFinishServerResponseMessage(receivedTitle) == true)
        {
        	// 書き込まれている、と判定した場合...
        	
            // 書き込みログに送信するCookieを書き出す
            parent.outputTextFile("writelog.txt", "(COOKIE)", getCookie("",""), true);

            // ここで書き込み画面を終了させる
            completedWriteMessage(returnToMessage, receivedTitle);
            return;        	
        }
/**/
        
        // extendString (書き込み用キーワード) を拾うために再度打ち込み
//        postHttpMain(sendUrl, referer, getCookie(be2chLoginId, ""), message);

        
        // 時刻を追加する...
        if (extendString.length() == 0)
        {
            currentTime   = currentTime - 180;
            extendString = "&time=" + currentTime;
        }

        // データを抽出し送信メッセージを再構築...
        if (sendTitle != false)
        {
            // スレ立て用
            message = "subject=" + titleBuffer;
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + extendString + confirmMessage;
            message = message + "&submit=%91S%90%D3%94C%82%F0%95%89%82%A4%82%B1%82%C6%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";

        }
        else
        {
            // スレにレス用
            message = "key="  + fileName.substring(0, logpos);
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + extendString + confirmMessage + "&submit=%8F%E3%8B%4C%91%53%82%C4%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";
        }
        if (sessionId != null)
        {
            message = message + "&sid=" + sessionId;
        }
/**
        // 書き込みログにメッセージを書き出す
        parent.outputTextFile("writelog.txt", "DEBUG-2", message, true);

        // 書き込みログにメッセージを書き出す
        parent.outputTextFile("writelog.txt", "REFERER", referer, true);

        // 書き込みログに送信するCookieを書き出す
        parent.outputTextFile("writelog.txt", "(COOKIE)", getCookie("",""), true);
**/
        System.gc();  // ガベコレ...

        // 書き込み結果を拾うバッファをクリア...
        receivedTitle = "";

        // Cookieを入れて再送...
        postHttpMain(sendUrl, referer, getCookie(be2chLoginId, "; NAME=NNsi(MIDP); Mail="), message);

/***** 
        // 時刻を追加する...
        if (extendString.length() == 0)
        {
            currentTime   = currentTime - 180;
            extendString = "&time=" + currentTime;
        }

        // データを抽出し送信メッセージを再構築...
        if (sendTitle != false)
        {
            // スレ立て用
            message = "subject=" + titleBuffer;
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + extendString + confirmMessage;
            message = message + "&submit=%91S%90%D3%94C%82%F0%95%89%82%A4%82%B1%82%C6%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";

        }
        else
        {
            // スレにレス用
            message = "key="  + fileName.substring(0, logpos);
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + extendString + confirmMessage + "&submit=%8F%E3%8B%4C%91%53%82%C4%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";
        }
        if (sessionId != null)
        {
            message = message + "&sid=" + sessionId;
        }
*****/
        System.gc();

/****
        // 書き込みログにメッセージを書き出す
        parent.outputTextFile("writelog.txt", "DEBUG-3", message, true);

        // 書き込みログにメッセージを書き出す
        parent.outputTextFile("writelog.txt", "REFERER", referer, true);
****/
        // 書き込みログに送信するCookieを書き出す
        parent.outputTextFile("writelog.txt", "(COOKIE)", getCookie("",""), true);

//        // もう一度再送...
//        postHttpMain(sendUrl, referer, getCookie(be2chLoginId, "; NAME=NNsi(MIDP); Mail="), message);
        
        // 書き込み画面の終了
        completedWriteMessage(returnToMessage, receivedTitle);
        return;
    }
    
    /**
     *   現在のCookieを取得する (付加情報をくっつけて応答)
     * 
     * @param be2chLoginId
     * @param appendData
     * @return 現在設定されているCookie
     */
    private String getCookie(String be2chLoginId, String appendData)
    {
    	String returnCookie = "" + currentCookie;

        if (be2chLoginId != null)
        {
        	returnCookie = be2chLoginId + "; " + returnCookie;
        }
        return (returnCookie + appendData);        
    }

    /**
     *   サーバからもらったCookieデータを設定する
     * 
     * @param value
     */
    private void setCookie(String value)
    {
        // もらったcookieのキーワード部分を抽出する
    	int cookieKeywordPos = value.indexOf("=");
    	int cookieEndPos = value.indexOf(";");
    	if ((cookieEndPos > 0)&&(cookieKeywordPos > cookieEndPos))
    	{
    		cookieKeywordPos = cookieEndPos;
    	}
        String keyword = "";
    	if (cookieKeywordPos > 0)
    	{
            keyword = value.substring(0, cookieKeywordPos);
    	}

    	// 今のCookie情報に 記憶するkeywordが含まれているかチェックする
    	int isIncludeCookie = currentCookie.indexOf(keyword);
    	if (isIncludeCookie < 0)
    	{
            // キーワードが含まれていない、その場合は、そのままがっつり記憶する
    		int pos = value.indexOf(";");
            if (currentCookie.length() != 0)
            {
                currentCookie = currentCookie + "; ";
            }
            currentCookie = currentCookie + value.substring(0, pos);
            return;
    	}
        String cookieToSet = currentCookie;
    	int cookieLength = cookieToSet.length();
		int pos = value.indexOf(";");
    	int nextCookiePos = currentCookie.indexOf(";", isIncludeCookie);
    	if (nextCookiePos <= 0)
    	{
            // 末尾にセミコロンがなかったら、Cookieの末尾を設定する
    		nextCookiePos = cookieLength;
    	}
    	
    	// 空白飛ばし (遅いだろうけど...)
    	int nextIndex = nextCookiePos + 1;
    	while (cookieToSet.charAt(nextIndex) == ' ')
    	{
            nextIndex++;
    	}
    	// 受信したCookieデータで値を差し替える
        currentCookie = cookieToSet.substring(0, isIncludeCookie)  + cookieToSet.substring(nextIndex, cookieLength) + "; " + value.substring(0, pos);

        return;
    }
    
    /**
     *   サーバからの応答メッセージをチェックする
     * 
     * @param checkString  確認する文字列
     * @return falseなら再投入
     */
    private boolean checkIsFinishServerResponseMessage(String checkString)
    {
    	int pos = checkString.indexOf("書きこみました");
    	if (pos >= 0)
    	{
    		// 書き込み成功のメッセージ受信だった。
    		return (true);
    	}
        return (false);
    }    
    
    /**
     * UTF8の文字列をEUCへ変換し、さらにURLエンコードする
     * @param utf8String
     * @return
     */
    public String utf8ToEUCandURLEncode(String utf8String)
    {
        int length    = utf8String.length() * 3;
        byte buffer[] = new byte[length];
        byte srcBuffer[] = sjConverter.parseToSJ(utf8String);
        utf8Converter.StrCopySJtoEUC(buffer, 0, srcBuffer, 0, srcBuffer.length);
        String convertStr = a2BsjConverter.urlEncode(buffer);
        buffer = null;
        srcBuffer = null;
        return (convertStr);
    }
    
    /**
     * EUCをUTF8に変換する
     * 
     * @param eucString
     * @return
     */
    public String eucToUtf8(byte[] eucString, int startIndex, int length)
    {
        return (utf8Converter.parsefromEUC(eucString, startIndex, length));
    }
    
    /**
     * Shift JISをUTF8に変換する
     * 
     * @param sjString
     * @return
     */
    public String sjToUtf8(byte[] sjString, int startIndex, int length)
    {
        return (utf8Converter.parsefromSJ(sjString, startIndex, length));
    }

    /**
     * 確認処理を実行する
     * @param newScene
     * @param title
     * @param message1
     * @param message2
     */    
    public void confirmation(int newScene, String title, String message1, String message2)
    {
        String message = message1 + message2;

        scene = newScene;
        alert.setTitle(title);
        alert.setString(message);
        alert.addCommand(execCmd);
        alert.addCommand(exitCmd);
        parent.callScreen(alert);

        return;
    }

    /**
     *  メッセージを表示する
     * 
     * 
     */
    public void information(String title, String message1, String message2)
    {
        String message = message1 + message2;

        scene = SCENE_INFORM;
        alert.setTitle(title);
        alert.setString(message);
        alert.addCommand(okCmd);
        parent.callScreen(alert);

        return;
    }

    /**
     *  キャンセル実行
     * 
     * 
     */
    public void cancelConfirmation()
    {
        // とりあえずコマンド設定をクリア
        if (scene != SCENE_INFORM)
        {
            alert.removeCommand(execCmd);
            alert.removeCommand(exitCmd);
        }
        else
        {
            alert.removeCommand(okCmd);
        }

        // 書き込み画面に戻す
        parent.returnScreen();

        return;
    }
    
    /**
     *  スレ操作...
     * 
     * 
     */
    public void doOperation()
    {
        parent.returnScreen();
        return;
    }
    
    /**
     *  詳細で入力したメッセージを反映させる
     * 
     *
     */
    public void setWriteDetailMessage(int command)
    {
        if (command != CMD_EXIT)
        {
            writeMessage.setString(msgBox.getString());
        }              

        parent.returnScreen();

        msgBox = null;
        System.gc();

        // 書き込み前の確認実施...
        if ((isWriting == true)||(command != CMD_DOWRITE))
        {
            // 書き込み中の場合には、再度書き込みさせない
            return;
        }

        // 全画面から直接書き込み処理へ...
        parent.setWriteName(writeName.getString());
        if (sendTitle == false)
        {
            confirmation(SCENE_WRITE, "書き込み確認", targetTitle, "に書き込みます。よろしいですか？");
        }
        else
        {
            confirmation(SCENE_WRITE, "スレ立て確認", targetTitle, "にスレを立てます。よろしいですか？");
        }
        return;
    }
    
    /**
     *   確認終了(実行)
     * 
     */
    public void confirm()
    {
        // とりあえずコマンド設定をクリア
        if (scene != SCENE_INFORM)
        {
            alert.removeCommand(execCmd);
            alert.removeCommand(exitCmd);
        }
        else
        {
            alert.removeCommand(okCmd);
        }

        // 書き込み画面に戻る
        parent.returnScreen();
        
        // 書き込み処理の実行...
        if (scene == SCENE_WRITE)
        {
            Thread thread = new Thread()
            {
                public void run()
                {
                    isWriting = true;
                    parent.updateBusyMessage("", "", false);
                    parent.showBusyMessage("書き込み中", targetTitle, a2BselectionForm.BUSY_FORM);
                    doWrite();
                    isWriting = false;
                }
            };
            try
            {
                // 書き込み終了まで待つ...
                thread.start();
//                thread.join();
            }
            catch (Exception e)
            {
                // 何もしない...
            }
            return;
        }
        if (scene == SCENE_INFORM)
        {
            if (returnToMessage == true)
            {
                // 書き込み結果がOKでファイルを読み込んでいた場合には、データをクリアする
                if (isFileLoaded == true)
                {
                    writeTitle = null;
                    writeMessage = null;
                    saveTemporary();
                }
                isFileLoaded = false;

                // スレ（またはスレ一覧）をリロードする。
                parent.returnToPreviousForm(parent.ACTION_NOTHING);
                if (sendTitle == true)
                {
                    parent.reloadCurrentMessageList();
                }
                else
                {
                    parent.reloadCurrentMessage();
                }
                sendTitle       = false;
                returnToMessage = false;
            }
            return;
        }
        return;
    }

    /**
     *   HTTP接続を使ってファイルに出力する （メイン処理）
     *   
     */
    private int postHttpMain(String urlToPost, String referenceUrl, String cookie, String sendMessage)
    {
        HttpConnection c  = null;
        InputStream    is = null;
        OutputStream   os = null;
        int           rc = -1;
        String         key = "";
        String         value = "";

        String url = urlToPost;
        if (parent.PROXY_USE_ALL == parent.getUsedProxyScene())
        {
               // Proxy経由でアクセスする
            url = parent.getProxyUrl() + "/" + url;
        }

        // 書き込み実行中メッセージを表示する
        parent.updateBusyMessage("", "", false);
        String message = "URL\n  " + urlToPost;
        parent.showBusyMessage("http通信準備中", message, a2BselectionForm.BUSY_FORM);        
        try 
        {
            c = (HttpConnection) Connector.open(url);
            {
                c.setRequestMethod(HttpConnection.POST);
                if (referenceUrl != null)
                {
                    c.setRequestProperty("referer", referenceUrl);
                }
                if (cookie != null)
                {
                    c.setRequestProperty("cookie", cookie);
                }
                c.setRequestProperty("accept", "text/html, image/jpeg, */*");
                c.setRequestProperty("content-language", " ja, en");
                String userAgent = parent.getUserAgent(false);
                c.setRequestProperty("user-agent", userAgent);
                c.setRequestProperty("x-2ch-ua", "a2B/1.00");
                c.setRequestProperty("content-type", "application/x-www-form-url-encoded");
            }

            // Getting the output stream may flush the headers
            os = c.openOutputStream();
            os.write(sendMessage.getBytes());

            message = "通信中\n" + url;
            parent.updateBusyMessage("http通信中", message, false);

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            rc = c.getResponseCode();
            if (rc != HttpConnection.HTTP_OK)
            {
                // データの通信失敗...
                parent.hideBusyMessage();
                parent.updateBusyMessage("", "", false);

                // close streams
                os.close();
                c.close();
                return (rc);
            }
            message = "受信データ解析中...";
            parent.updateBusyMessage("http通信", message, false);

            //////////  Cookie の抽出を実施する  //////////
            int len   = (int)c.getLength();
            int index = 0;
            try
            {
                while (index >= 0)
                {
                    value = c.getHeaderField(index);
                    key   = c.getHeaderFieldKey(index);
                    if (key.compareTo("set-cookie") == 0)
                    {
                        setCookie(value);
                    }
                    else if (key.compareTo("Set-Cookie") == 0)
                    {
                        setCookie(value);
                    }
                    value = "";
                    key   = "";
                    index++;
                }
            }
            catch (Exception e)
            {
                // エラー情報を設定...
                receivedTitle = "C:" + e.getMessage();
            }

            //////////  ボディから情報を引き抜く... /////////
            is = c.openInputStream();

            // Get the length and process the data
            int receivedSize = 0;
            if (len > 0)
            {
                int parsed         = 0;
                int position       = 0;
                int bottom         = 0;

                byte[] data        = new byte[(maxMessage * 2) + MARGIN];
                try
                {
                	int actual = 0;
                    do
                	{
                    	receivedSize = receivedSize + actual;
                        actual = is.read(data, receivedSize, ((maxMessage * 2) - receivedSize));
                	} while ((actual > 0)&&(receivedSize < (maxMessage * 2)));
                }
                catch (Exception e)
                {
                	// 例外発生 (読み込みエラーだろうから、ここでは何もしない)
                }
/*
                byte[] data        = new byte[len + MARGIN];
                actual              = is.read(data, 0, len);
*/
                String readData     = utf8Converter.parsefromSJ(data, 0, receivedSize);
                data                = null;
                String matchPattern = "";
//                receivedTitle       = extendString + "///" + sendMessage + "///" + readData;
                receivedTitle       = readData + "\n(bytes: " + receivedSize + "[length: " + len + "])";
//                receivedTitle       = readData;
/**
                // 書き込みログに受信データを書き出す
                parent.outputTextFile("writelog.txt", "REPLY(DEBUG)", readData, true);
**/

                // タイトル部分引き抜き...
                matchPattern  = "<title>";
                position      = readData.indexOf(matchPattern);
                position      = position + matchPattern.length();
                bottom        = readData.indexOf("</title>", position);
                if (position < bottom)
                {
                    receivedTitle = readData.substring(position, bottom);
                }
                matchPattern  = "<b>";
                position      = readData.indexOf(matchPattern);
                if (position >= 0)
                {
                    position      = position + matchPattern.length();
                    bottom        = readData.indexOf("</b>", position);
                    if (position < bottom)
                    {
                        receivedTitle = receivedTitle + "\n   " + readData.substring(position, bottom);
                    }
                    returnToMessage = false;
                }
                else
                {
                    returnToMessage = true;
                }

                while (parsed < receivedSize)
                {
                    //  データアイテムを抽出する...
                    matchPattern = "<input type=hidden name=";
                    position     = readData.indexOf(matchPattern, parsed);
                    if (position < 0)
                    {
                        break;
                    }
                    position     = position + matchPattern.length();
                    bottom       = readData.indexOf(" ", position);

                    key   = readData.substring(position, bottom);
                    if (key.charAt(0) == '"')
                    {
                        if ((key.compareTo("\"subject\"") == 0)||(key.compareTo("\"time\"") == 0))
                        {
                            key = key.substring(1, (key.length() - 1));
                            extendString = extendString + "&" + key + "=";
                        }
                        else
                        {
                            key = key.substring(1, (key.length() - 1));
                            extendString = extendString + "&" + key + "=";
                        }
                    }
                    else
                    {
                        if ((key.compareTo("subject") == 0)||(key.compareTo("time") == 0))
                        {
                            extendString = extendString + "&" + key + "=";
                        }
                        else
                        {
                            key = "";
                        }
                    }
                    
                    matchPattern = "value=";
                    position   = readData.indexOf(matchPattern, bottom);
                    position     = position + matchPattern.length();
                    bottom       = readData.indexOf(">", position);
                    value   = readData.substring(position, bottom);
                    if ((key.length() != 0)&&(value.length() > 0))
                    {
                        if (value.charAt(0) == '"')
                        {
                            if (value.length() > 2)
                            {
                                if (key.compareTo("time") == 0)
                                {
                                    long timeData = Long.parseLong(value.substring(1, (value.length() - 2)));
                                    extendString = extendString + (timeData - TIME_OFFSET);
                                }
                                else
                                {
                                    extendString = extendString + value.substring(1, (value.length() - 1));                                    
                                }
                            }
                        }
                        else
                        {
                            if (key.compareTo("time") == 0)
                            {
                                long timeData = Long.parseLong(value);
                                extendString = extendString + (timeData - TIME_OFFSET);
                            }
                            else
                            {
                                extendString = extendString + value;                                
                            }
                        }
                    }
                    parsed = bottom;                    
                }
                //  データをクリアする
                readData = null;
            }
            else
            {
                receivedTitle = "??? (" + getCookie("","") + ")\n" + referenceUrl;
            }
            
            // close streams
            is.close();
            os.close();
            c.close();
        }
        catch (Exception e)
        {
            receivedTitle = receivedTitle + " I:" + e.getMessage();
            try
            {
                if (os != null)
                {
                    os.close();
                }
            }
            catch (Exception e2)
            {
                    
            }
            try
            {
                if (is != null)
                {
                    is.close();
                }
            }
            catch (Exception e2)
            {
                    
            }
            try
            {
                if (c != null)
                {
                    c.close();
                }
            }
            catch (Exception e2)
            {
                
            }
        }
//        receivedTitle = referenceUrl + " ### " + getCookie("", "") + " ### " + receivedTitle;
        parent.hideBusyMessage();
        parent.updateBusyMessage("", "", false);
        return (rc);
    }
    

    /**
     *   書き込み画面をデータ入力フォームとして利用する準備を行う
     *   
     */
    public void prepareWriteFormUsingDataInputMode(int dataScene, String title, String itemName, String itemData, int maxLength, boolean ignoreCase)
    {
        deleteAll();

        // タイトルサイズを決定する
        int length = 0;
        if (maxLength >= 0)
        {
            length = maxLength;
        }
        else
        {
            length = maxUrl;
        }
        
        // シーンを記憶する
        scene = dataScene;
        
        // タイトルの表示
        setTitle(title);

        // タイトル
        writeTitle = new TextField(itemName, itemData, length, TextField.ANY);
        append(writeTitle);

        // OKボタン
        writeButton  = new StringItem("", "OK", Item.BUTTON);
        writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        writeButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_APPLYLIST));
        writeButton.setItemCommandListener(this);
        append(writeButton);

        // スペーサー
        append(new Spacer(10, 10));

        // 選択グループ
        if (ignoreCase == true)
        {
            useOysterInfo = new ChoiceGroup("検索オプション", Choice.MULTIPLE);
            useOysterInfo.append("大文字と小文字の区別を無視", null);
            useOysterInfo.append("Google検索(Webブラウザで)", null);
            if (scene == parent.SCENE_TITLE_SEARCH)
            {
                useOysterInfo.append("2ちゃんねる検索で検索", null);
            }
            useOysterInfo.setSelectedIndex(0, true);
            append(useOysterInfo);
            if (scene == parent.SCENE_TITLE_SEARCH)
            {
                String pickString = "" + pickupHour;
                writeCount = new TextField("スレ立て経過時間(0:無効)", pickString, 3, TextField.DECIMAL);
                append(writeCount);
            }
        }
        // スペーサー
        append(new Spacer(0, 15));
        
/*
        // OKボタン
        writeButton  = new StringItem("", "OK", Item.BUTTON);
        writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        writeButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_APPLYLIST));
        writeButton.setItemCommandListener(this);
        append(writeButton);
*/

        // スペースを空ける...
        append(new Spacer(0, 20));

        // キャンセルボタン
        cancelButton = new StringItem("", "キャンセル", Item.BUTTON);
        cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        cancelButton.setDefaultCommand(new Command("CANCEL", Command.EXIT, CMD_EXIT));
        cancelButton.setItemCommandListener(this);
        append(cancelButton);

        return;
    }

    /**
     *   書き込み画面をテキスト編集用フォームとして利用する準備を行う
     *   
     */
    public void prepareWriteFormUsingTextEditMode(String title, String message)
    {
        deleteAll();

        // タイトルの表示
        setTitle(title);
        
        // データの表示を行う
        writeTitle = new TextField("", message, a2BMain.TEMP_PREVIEW_SIZE, TextField.ANY);
        writeTitle.setLayout(Item.LAYOUT_2 | Item.LAYOUT_VEXPAND);
        append(writeTitle);

        // スペーサー
        Spacer sp = new Spacer(10, 10);
        append(sp);
        sp = null;

        // ファイル仮保存ボタン
        saveButton  = new StringItem("", "(仮保存)", Item.BUTTON);
        saveButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        saveButton.setDefaultCommand(new Command("SAVE", Command.SCREEN, CMD_WRITEFILE));
        saveButton.setItemCommandListener(this);
        append(saveButton);

        // キャンセルボタン
        cancelButton = new StringItem("", "戻る", Item.BUTTON);
        cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        cancelButton.setDefaultCommand(new Command("CANCEL", Command.EXIT, CMD_EXIT));
        cancelButton.setItemCommandListener(this);
        append(cancelButton);

        // 全画面ボタン
        detailButton = new StringItem("", "全画面", Item.BUTTON);
        detailButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        detailButton.setDefaultCommand(new Command("DETAIL", Command.SCREEN, CMD_DETAIL_VIEW));
        detailButton.setItemCommandListener(this);
        append(detailButton);

        // スペースを空ける...
        append(new Spacer(0, 20));

        // HTTP通信ボタン
        this.addCommand(httpCmd);

        return;
    }

    /**
     *  メニューにスレ操作メニューを追加する..
     * 
     *
     */
    public void prepareThreadOperationMenu(String addInfo, int threadLevel)
    {
        // 操作コマンドを追加する
        this.addCommand(opeCmd);

        // その他情報
        StringItem item = new StringItem("", addInfo, Item.PLAIN);
        append(item);
        
        if (threadLevel >= 0)
        {
            // スレレベルの設定
            String level = threadLevel + "";
            writeName = new TextField("お気に入り番号(0, 1-9)", level, 1, TextField.DECIMAL);
            append(writeName);
            isGetNumber = true;
        }
        else
        {
            isGetNumber = false;
        }
        return;
    }

    /**
     *  メニューにスレ操作メニューを追加する..
     * 
     *
     */
    public void preparePreviewMenu()
    {
        this.addCommand(reserveCmd);
        this.addCommand(getCmd);
        return;
    }
    
    /**
     *   書き込み画面をa2B設定(Proxy設定)フォームとして利用する準備を行う
     *   
     */
    public void prepareWriteFormUsingProxySettingMode(int dataScene, String title, String proxyUrl, int useMode, boolean webLaunch)
    {
        deleteAll();

        // タイトルサイズを決定する
        int length = maxUrl;

        // シーンを記憶する
        scene = dataScene;
        
        // タイトルの表示
        setTitle(title);

        // proxy URL
        writeTitle = new TextField("Proxy(Host名：ポート番号)", proxyUrl, length, TextField.ANY);
        append(writeTitle);
        
        // スペースを入れる
        append(new Spacer(10, 5));

        // ●ログイン名
        oysterName = new TextField("2chログイン名(●)", parent.getOysterName(), length, TextField.EMAILADDR);
        append(oysterName);
        
        // ●パスワード
        oysterPass = new TextField("2chパスワード(●)", parent.getOysterPassword(), length, TextField.PASSWORD);
        append(oysterPass);

/**********/
        // スペースを入れる
        append(new Spacer(10, 5));

        // Be@2chログイン名
        beUserName = new TextField("BE@2chログイン名", parent.getBeName(), length, TextField.EMAILADDR);
        append(beUserName);
        
        // Be@2chパスワード
        bePassword = new TextField("BE@2chパスワード", parent.getBePassword(), length, TextField.PASSWORD);
        append(bePassword);
/**********/

        // スペースを入れる
        append(new Spacer(10, 5));

        // Proxy使用の選択肢
        choiceGroup = new ChoiceGroup("Proxy利用設定", Choice.EXCLUSIVE);
        choiceGroup.append("利用しない", null);
        choiceGroup.append("利用する(現状GETのみ)", null);
        choiceGroup.append("GET時のみ利用する", null);
        choiceGroup.append("差分取得のみ利用する", null);
        choiceGroup.append("WX310のgzip転送対策", null);
        choiceGroup.setSelectedIndex(useMode, true);
        append(choiceGroup);

        // スペースを入れる
        append(new Spacer(10, 10));

/**
//        choiceComm = new ChoiceGroup("新着確認方法(要再起動)", Choice.POPUP);
        choiceComm = new ChoiceGroup("新着確認方法(要再起動)", Choice.EXCLUSIVE);
        choiceComm.append("FG通信", null);
        choiceComm.append("BG通信", null);
        int index = 0;
        if (commMode == true)
        {
            index = 1;
        }
        choiceComm.setSelectedIndex(index, true);
        append(choiceComm);

        // スペースを入れる
        append(new Spacer(10, 10));
**/        
//        choiceListFunc = new ChoiceGroup("一覧画面０キー ", Choice.POPUP);
        choiceListFunc = new ChoiceGroup("一覧画面０キー機能", Choice.EXCLUSIVE);
        choiceListFunc.append("フォントサイズ変更", null);
        choiceListFunc.append("数字キー移動モード", null);
        choiceListFunc.append("ヘルプ表示モード", null);
        choiceListFunc.append("スレ情報表示", null);
        choiceListFunc.append("未読スレ抽出", null);
        choiceListFunc.append("スレ操作", null);
        choiceListFunc.append("モードメニュー", null);
        choiceListFunc.append("ログ参照モード", null);
        choiceListFunc.append("クリア", null);
        choiceListFunc.append("関連キーワード", null);
        choiceListFunc.append("スレ削除", null);
        choiceListFunc.append("ログイン(●/BE)", null);
        choiceListFunc.setSelectedIndex(parent.getFunctionKeyFeature(parent.SCENE_LISTFORM), true);
        append(choiceListFunc);

        // スペースを入れる
        append(new Spacer(7, 7));

//        choiceViewFunc = new ChoiceGroup("参照画面０キー ", Choice.POPUP);
        choiceViewFunc = new ChoiceGroup("参照画面０キー機能", Choice.EXCLUSIVE);
        choiceViewFunc.append("フォントサイズ変更", null);
        choiceViewFunc.append("数字キー移動モード", null);
        choiceViewFunc.append("ヘルプ表示モード", null);
        choiceViewFunc.append("スレ情報表示", null);
        choiceViewFunc.append("NGワード検出モード", null);        
        choiceViewFunc.append("レス出力", null);
        choiceViewFunc.append("モードメニュー", null);
        choiceViewFunc.append("レスまとめ読みモード", null);
        choiceViewFunc.append("クリア", null);
        choiceViewFunc.append("おすすめ２ちゃんねる", null);
        choiceViewFunc.setSelectedIndex(parent.getFunctionKeyFeature(parent.SCENE_VIEWFORM), true);
        append(choiceViewFunc);

        // スペースを入れる
        append(new Spacer(10, 10));

        // 選択グループ
        choiceOptFavor    = new ChoiceGroup("オプション", Choice.MULTIPLE);
        choiceOptFavor.append("起動時にお気に入り一覧を表示", null);
        choiceOptFavor.append("ブラウザ起動後もa2Bを継続", null);
        choiceOptFavor.append("次スレ取得時にお気に入り登録", null);
        choiceOptFavor.append("ダウンロードを直ディレクトリに", null);
        choiceOptFavor.append("スレ分割取得モード", null);
        choiceOptFavor.append("スレタイを一覧下部に表示", null);
        choiceOptFavor.append("subject.txt保管", null);
        choiceOptFavor.append("お気に情報保管回数削減", null);
        choiceOptFavor.append("ブラウザ起動時Google変換利用", null);
        choiceOptFavor.append("プレビュー時小さい画像に", null);
        choiceOptFavor.append("取得済みファイルがあれば利用", null);
        choiceOptFavor.append("カーソルの動きをページ送り", null);
        choiceOptFavor.append("自前のUTF8漢字変換(要再起動)", null);
        append(choiceOptFavor);

        // 起動時にお気に入り一覧を表示がtrueの場合には、画面にチェックを反映させる
        if (parent.getLaunchAsFavoriteList() == true)
        {
            choiceOptFavor.setSelectedIndex(0, true);
        }

        // Webブラウザ起動後もa2Bを継続するかどうかを確認し、画面にチェックを反映させる
        if (parent.getLaunchMode() == true)
        {
            choiceOptFavor.setSelectedIndex(1, true);
        }

        // 次スレ取得時にお気に入りへ自動登録するかどうかを確認し、画面にチェックを反映させる
        if (parent.getAutoEntryFavorite() == true)
        {
            choiceOptFavor.setSelectedIndex(2, true);            
        }
        
        // ファイルダウンロードは、ルートディレクトリに記録する
        if (parent.getSaveAtRootDirectory() == true)
        {
            choiceOptFavor.setSelectedIndex(3, true);
        }

        // スレ分割取得モードを設定する
        if (parent.getDivideGetHttp() == true)
        {
            choiceOptFavor.setSelectedIndex(4, true);
        }

        // スレタイを一覧下部に表示
        if (parent.getShowTitleAnotherArea() == true)
        {
            choiceOptFavor.setSelectedIndex(5, true);
        }

        // subject.txtを保管する...
        if (parent.getBackupSubjectTxt() == true)
        {
            choiceOptFavor.setSelectedIndex(6, true);
        }

        // お気に入りデータ保管タイミングを減らす...
        if (parent.getIgnoreFavoriteBackup() == true)
        {
            choiceOptFavor.setSelectedIndex(7, true);
        }

        // ブラウザ起動でgoogleプロキシを使用する
        if (parent.getUseGoogleProxy() == true)
        {
            choiceOptFavor.setSelectedIndex(8, true);
        }

        // 画像取得時、googleプロキシを使用する
        if (parent.getWX220JUserAgent() == true)
        {
            choiceOptFavor.setSelectedIndex(9, true);
        }

        // 画像取得時、取得済みファイルを利用する
        if (parent.getUseCachePictureFile() == true)
        {
            choiceOptFavor.setSelectedIndex(10, true);
        }

        // カーソルの動きをページ送りに
        if (parent.getCursorSingleMovingMode() == true)
        {
            choiceOptFavor.setSelectedIndex(11, true);
        }

        // 自前のUTF8漢字変換(要再起動)
        if (parent.getForceKanjiConvertMode() == true)
        {
            choiceOptFavor.setSelectedIndex(12, true);
        }

        // 無操作終了時間 
        String    time = "" + parent.getWatchDogTime();
        writeName = new TextField("無操作終了時間[分](要再起動)",  time, 3, TextField.DECIMAL);
        append(writeName);

        // レスまとめ読みモード
        String    cnt = "" + parent.getResBatchCount();
        writeCount = new TextField("レスまとめ読み時のレス数",  cnt, 1, TextField.DECIMAL);
        append(writeCount);

        // AAマージン 
        String    margin = "" + parent.getAAwidthMargin();
        aaWidth = new TextField("AA表示横移動量",  margin, 3, TextField.DECIMAL);
        append(aaWidth);

        // キーリピート検出時間(ms) 
        margin = "" + parent.getKeyRepeatTime();
        keyRepeatMs = new TextField("キーリピート検出間隔(ms)",  margin, 4, TextField.DECIMAL);
        append(keyRepeatMs);

        // ユーザーエージェント
        writeEmail = new TextField("User-Agent", parent.getUserAgent(false), 256, TextField.ANY);
        append(writeEmail);

        // 分割取得サイズ
        String divSize = "" + parent.getDivideGetSize();
        divideSize = new TextField("分割取得サイズ", divSize, 6, TextField.DECIMAL);
        append(divideSize);
        
        // フォントサイズの指定
        fontIdNumber = new TextField("Font(>=0で更新)", "-1", 6, TextField.DECIMAL);
        append(fontIdNumber);

        // スペースを入れる
        append(new Spacer(10, 15));

        // OKボタン
        writeButton  = new StringItem("", "設定", Item.BUTTON);
        writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        writeButton.setDefaultCommand(new Command("設定", Command.SCREEN, CMD_SETPROXY));
        writeButton.setItemCommandListener(this);
        append(writeButton);
/**
        // キャンセルボタン
        cancelButton = new StringItem("", "キャンセル", Item.BUTTON);
        cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        cancelButton.setDefaultCommand(new Command("CANCEL", Command.EXIT, CMD_EXIT));
        cancelButton.setItemCommandListener(this);
        append(cancelButton);
**/
        this.addCommand(fontCmd);
        
        return;
    }
    
    /**
     *  確認ダイアログのコマンド選択時処理
     *
     */
    public class a2BConfirmationListener implements CommandListener
    {
        private a2BMessageWriteForm parent = null;
        
        /**
         *   コンストラクタ
         * 
         */
        public a2BConfirmationListener(a2BMessageWriteForm object)
        {
            parent = object;
        }

        /**
         *  コマンドボタン実行時の処理
         *  
         */
        public void commandAction(Command c, Displayable d)
        {
            int num = c.getPriority();
            if (num == CMD_DOEXECUTE)
            {
                // 確認された場合...
                parent.confirm();
                return;
            }
            if (num == CMD_OPERATION)
            {
                return;
            }
            if ((num == CMD_DONE)||(num == CMD_EXIT)||(num == CMD_DOWRITE))
            {
                // 書き込んだ値を反映させる
                parent.setWriteDetailMessage(num);
                return;
            }

            // キャンセルされた場合...
            parent.cancelConfirmation();
            return;
        }
    }
}
//--------------------------------------------------------------------------
//  a2BMessageWriteForm  --- レス書き込みクラス
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
