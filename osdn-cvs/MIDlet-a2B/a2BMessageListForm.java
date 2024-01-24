import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import java.util.Date;
import java.util.Vector;

/**
 * スレ一覧を表示・操作するクラス
 * 
 * @author MRSa
 *
 */
public class a2BMessageListForm extends Canvas implements CommandListener
{
    private a2BMain               parent          = null;
    private a2BbbsCommunicator    bbsCommunicator = null;
    private a2BsubjectDataParser  subjectParser   = null;
    private a2BFavoriteManager    favorManager    = null;

    private Command modeCmd   = new Command("モード",        Command.EXIT,    -1);
    private Command catCmd    = new Command("カテゴリ選択",  Command.SCREEN,  50);
    private Command selCmd    = new Command("板選択",        Command.SCREEN,  50);
    private Command getCmd    = new Command("一覧取得 (x)",  Command.SCREEN,  50);
    private Command getAllCmd = new Command("新着確認/中止", Command.SCREEN,  50);
    private Command srchCmd   = new Command("スレ検索",      Command.SCREEN,  50);
    private Command opeCmd    = new Command("スレ操作",      Command.SCREEN,  50);
    private Command deleteCmd = new Command("スレ削除 (x)",  Command.SCREEN,  50);
    private Command proxyCmd  = new Command("a2B設定",       Command.SCREEN,  50);
    private Command quitCmd   = new Command("終了",          Command.SCREEN,  50);

    private final byte SHOW_SUBJECTLIST    = 0;
    private final int  MARGIN              = 6;
    private final int  TO_PREVIOUS         = -1;
    private final int  TO_NEXT             = 1;
    private final int  TO_REDRAW           = 0;
    private final int  FORCE_REDRAW        = 2;

    static final int  GETLOGLIST_START_RECURSIVE = 0;
    static final int  GETLOGLIST_CONTINUE_RECURSIVE = 1;
    static final int  GETLOGLIST_START_THREAD = 2;

    static final int  OPTION_NOTSPECIFY     = -1;   // 指定なし
    static final int  OPTION_NOTIGNORE_CASE = 0;    // 大文字/小文字を区別する
    static final int  OPTION_IGNORE_CASE    = 1;    // 大文字/小文字を区別しない
    static final int  OPTION_ONLYDATABASE   = 2;    // スレ情報からひっぱってくる
    static final int  OPTION_RESETMODE      = 3;    // 情報切り替えモードをリセット

    public final int GETMODE_GETPART       = 0;
    public final int GETMODE_GETALL        = 1;
    public final int GETMODE_PREVIEW       = 2;

    public final int PREVIEW_LENGTH        = 5120;    // プレビューモードで取得するデータ数
    
    private String    gettingLogDataFile      = null;
    private String    gettingLogDataURL       = null;
    private String    gettingLogDataTitle     = null;
    private String    searchTitleString       = null;
    private String    temporaryTitleString    = null;
    private String    informationString       = null;
    private Vector    pickup2chList           = null;
    private boolean  searchIgnoreCase        = false;
    private int      searchTitlePickupHour   = -1;

    private Font      screenFont              = null;
    private int       screenHeight            = 0;
    private int       screenWidth             = 0;

    private int      screenLines             = 0;
    private int      displayIndexes[]        = null;

    private int      currentSelectedLine     = 0;
    private int      previousSelectedLine    = 0;
    private int      nofSubjectIndex         = 0;
    private int      listDrawMode            = TO_REDRAW;
    private int      listCount               = -1;
    private int      lastSubjectIndex        = 0;

    private int      currentSubjIndex        = 0;    // 現在操作中のスレID
    private int      currentBbsIndex         = 0;    // 現在表示中のスレ板名
    private int      currentFavoriteLevel    = -1;    // スレお気に入りレベル
    
    private int      showInformationMode     = -1;
    
    private byte     currentScene             = SHOW_SUBJECTLIST;

    private final byte NO_SEARCHING_MODE     = a2BsubjectDataParser.STATUS_NOTSPECIFY; //
    private final byte SEARCHING_MODE        = a2BsubjectDataParser.STATUS_NOTYET;     //
    private final byte SEARCHING_ONLYDB      = a2BsubjectDataParser.STATUS_OVER;       //
    
    private byte     searchTitleMessageStatus = NO_SEARCHING_MODE;
    private boolean isHelpShowMode           = false;
    private boolean isNumberMovingMode       = false;
    private boolean isOpenImmediate          = true;
    private boolean isSubjectTxtUpdate       = false;
    private boolean isAbortNewArrival        = false;
    private boolean isFirstTime              = true;

    private int      newArrialCheckIndex      = 0;        // 現在新着確認を実行しているインデックス番号
    private int      busyAreaSize             = 1;

    private Image     helpImage               = null;  // ヘルプ用イメージファイル
    private String    busyMessage             = null;  // メッセージ表示...

    /**
     * コンストラクタ
     * @param arg0         フォームタイトル
     * @param object       親クラス
     * @param communicator 板一覧情報保持クラス
     * @param subjParser   スレ一覧情報保持クラス
     * @param favor        お気に入りスレ情報保持クラス
     */
    public a2BMessageListForm(String arg0, a2BMain object, a2BbbsCommunicator communicator, a2BsubjectDataParser subjParser, a2BFavoriteManager favor)
    {
        // タイトル
        setTitle(arg0);

        // 親クラス
        parent = object;

        // 板一覧用クラス
        bbsCommunicator = communicator;
        
        // スレ一覧用解析クラス
        subjectParser = subjParser;
        
        // お気に入り一覧クラス
        favorManager = favor;

        // コマンドの登録
        this.addCommand(modeCmd);
        this.addCommand(catCmd);
        this.addCommand(selCmd);
        this.addCommand(getCmd);
        this.addCommand(getAllCmd);
        this.addCommand(srchCmd);
        this.addCommand(opeCmd);
        this.addCommand(deleteCmd);
        this.addCommand(proxyCmd);
        this.addCommand(quitCmd);
        this.setCommandListener(this);

    }

    
    
    /**
     *    フォントと画面サイズを設定する
     * 
     */
    public void prepareScreenSize()
    {
        // フォントと画面サイズを取得する
        Font font = Font.getDefaultFont();
        font      = Font.getFont(font.getFace(), font.getStyle(), Font.SIZE_SMALL);
        int line = (screenHeight / (font.getHeight() + 1));

        // 表示中のスレID格納領域を初期化する
        displayIndexes = new int[line + MARGIN];
        for (int loop = 0; loop < (line + MARGIN); loop++)
        {
            displayIndexes[loop] = -1;
        }
        
        // 表示可能行を設定...
        try
        {
            screenFont = Font.getFont(font.getFace(), font.getStyle(), parent.getListFontSize());
        }
        catch (Exception ex)
        {
            screenFont = Font.getFont(font.getFace(), font.getStyle(), Font.SIZE_SMALL);
        }
        setLineWidthHeight(getWidth(), getHeight());
        repaint();

        return;
    }
    
    /**
     *    フォントと画面サイズを設定する
     * 
     */
/**
    public void setupScreenSize()
    {
        // フォントサイズを正規なものに戻し、も一回表示可能行を設定しなおす...
        try
        {
            screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), parent.getListFontSize());
        }
        catch (Exception ex)
        {
        }
        setLineWidthHeight(getWidth(), getHeight());

        // 表示リストを更新する
        updateTitleList(FORCE_REDRAW);

        repaint();
        return;
    }
**/

    /**
     * 表示可能領域を設定する
     * @param w
     * @param h
     */
    private void setLineWidthHeight(int w, int h)
    {
        // スレタイ表示行を確保する
        if (parent.getShowTitleAnotherArea() == true)
        {
            busyAreaSize = 2;
        }
        else
        {
            busyAreaSize = 1;
        }
        screenWidth  = w;
        screenHeight = h - screenFont.getHeight() * busyAreaSize;
        screenLines  = (screenHeight / (screenFont.getHeight() + 1));
        return;
    }

    /**
     *   画面描画の実施...
     * 
     * 
     */
    public void paint(Graphics g)
    {
        // オブジェクトが構築されていない...再描画せずに終了する
        if ((bbsCommunicator == null)||(subjectParser == null)||(parent == null))
        {
            return;
        }

        // WX310用ワークアラウンド...
        if (isFirstTime == true)
        {
            isFirstTime = false;

            // 表示可能行を設定する
            setLineWidthHeight(getWidth(), getHeight());        
        }

        // 表示モードを確認、一覧表示を切り替える
        if (currentFavoriteLevel < 0)
        {
            // 通常表示モードの画面描画
            showList(g);
        }
        else
        {
            // お気に入り表示モードの画面描画
            showListFavorite(g);
        }
        
        // ヘルプ表示モードか？
        if ((isHelpShowMode == true)&&(helpImage != null))
        {
            // ヘルプ表示を行う
            showHelpImage(g);
        }

        // 数字キー入力モード..
        if (isNumberMovingMode == true)
        {
            g.setColor(0x00ff00ff);
            g.fillRect(0, 0, 2, 12);
            g.setColor(0);
        }

        // インフォメーション表示モード
        if ((showInformationMode >= 0)&&(informationString != null))
        {
            int y = screenHeight / 2;
            if (showInformationMode == 0)
            {
                g.setColor(0x00ffffcc);
            }
            else
            {
                g.setColor(0x00ffcccc);
            }
            g.fillRect(10, (y - 3), (screenWidth - 20), (screenFont.getHeight() + 6));
            g.setColor(0);
            g.drawRect(10, (y - 3), (screenWidth - 20), (screenFont.getHeight() + 6));
            g.drawString(informationString, 20, y, (Graphics.LEFT | Graphics.TOP));
        }

        // BUSYメッセージの表示
        {
            int heightTop = screenLines * (screenFont.getHeight() + 1);
//            int heightTop = screenLines * (screenFont.getHeight() + 2);
            g.setColor(0x00d8ffd8);
            g.fillRect(0, heightTop, getWidth(), getHeight() - heightTop);
//            g.fillRect(0, screenHeight + 1, getWidth(), getHeight() - (screenHeight + 1));
            if (busyMessage != null)
            {
                if (busyAreaSize == 1)
                {
                    // １行表示モードの場合...
                    g.setColor(0x00);    // 黒
                    g.drawString(busyMessage, 0, (screenHeight + 1), (Graphics.LEFT | Graphics.TOP));
                }
                else
                {
                    // 複数行表示モードの場合...
                    drawBusyAreaMultiple(g, heightTop);
                }
            }
        }

        return;
    }

    /**
     * ヘルプの表示モードメイン
     * 
     * @param g
     */
    private void showHelpImage(Graphics g)
    {
        // イメージの表示座標を決める
        int x = ((screenWidth  - helpImage.getWidth())  / 2);
        if ((x + helpImage.getWidth()) > screenWidth)
        {
            x = 0;
        }

        int y = ((screenHeight - helpImage.getHeight()) / 2);
        if ((y + helpImage.getHeight()) > screenHeight)
        {
            y = 0;
        }
        
        // ヘルプイメージを描画
        g.setColor(0);
        g.drawImage(helpImage, x, y, (Graphics.LEFT | Graphics.TOP));

        return;
    }

    /**
     *  スレ操作モードの動作コマンドを実行する
     * 
     * @param index
     */    
    private void executeSubjectOperation(int index)
    {
        // スレ状態を取得する
        int  status  = subjectParser.getStatus(currentSubjIndex);

        // スレ操作モード...
        if (index == 0)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // 再取得...(スレ全取得を実施)
            getLogDataFile(currentSubjIndex, GETMODE_GETALL);
            return;
        }
        else if (index == 1)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // プレビュー...
            getLogDataFile(currentSubjIndex, GETMODE_PREVIEW);
            return;
        }
        else if (index == 2)
        {
            // 取得予約...(次回新着確認時にスレ全取得を実施する)
            if (status == a2BsubjectDataParser.STATUS_NOTYET)
            {
                // スレ未取得の場合には取得状態の設定を入れる
                subjectParser.setCurrentMessage(currentSubjIndex, -1, 0);
                //subjectParser.setGetLogStatus(currentSubjIndex, subjectParser.STATUS_RESERVE);            
                return;
            }
            return;
        }
        if (index == 3)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            if ((status == a2BsubjectDataParser.STATUS_NOTYET)||(status == a2BsubjectDataParser.STATUS_RESERVE))
            {
                // スレ未取得(スレ全取得を実施)
                getLogDataFile(currentSubjIndex, GETMODE_GETALL);
                return;
            }

            // 差分取得...
            getLogDataFile(currentSubjIndex, GETMODE_GETPART);
            return;
        }
        else if (index == 4)
        {
            // スレ参照... (親に対し、画面を抜けてスレを表示することを通知する)
            if ((status == a2BsubjectDataParser.STATUS_NOTYET)||(status == a2BsubjectDataParser.STATUS_RESERVE))
            {
                // スレ未取得なので、何もしない...
                return;
            }
            parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, subjectParser.getCurrentMessage(currentSubjIndex));
            gettingLogDataFile = null;
            return;
        }
        else if (index == 5)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // お気に入り登録...
            Thread thread = new Thread()
            {
                public void run()
                {
                    // お気に入りの登録/登録解除を実施する
                    setResetFavoriteThread(currentSubjIndex, true);
                }
            };
            thread.start();
            return;
        }
        else if (index == 6)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // お気に入り登録解除...
            Thread thread = new Thread()
            {
                public void run()
                {
                    // お気に入りの登録/登録解除を実施する
                    setResetFavoriteThread(currentSubjIndex, false);
                }
            };
            thread.start();
            return;
        }
        else if (index == 7)
        {
            // ブラウザで開く...
            launchWebBrowserCurrentSelectedIndex();
        }
        else if (index == 8)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }

            // 取得先URLを生成し、スレタイトルと連結する
            String boardUrl = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
            int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
            String nick = boardUrl.substring(pos + 1);
            String url  = boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + subjectParser.getThreadNumber(currentSubjIndex) + "/";
            showRelatedKeyword(url);
        }
        return;
    }
    
    /**
     *  お気に入り一覧表示を行っているとき、スレ操作モードの動作コマンドを実行する
     * 
     * @param index
     */    
    private void executeSubjectOperationFavorite(int index)
    {
        // スレ状態を取得する
        byte  status  = favorManager.getStatus(currentSubjIndex);

        // スレ操作モード...
        if (index == 0)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // 再取得...(スレ全取得を実施)
            getLogDataFileFavorite(currentSubjIndex, GETMODE_GETALL);
            return;
        }
        else if (index == 1)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            if ((status == favorManager.STATUS_NOTYET)||(status == favorManager.STATUS_RESERVE))
            {
                // スレ未取得(スレ全取得を実施)
                getLogDataFileFavorite(currentSubjIndex, GETMODE_GETALL);
                return;
            }

            // 差分取得...
            getLogDataFileFavorite(currentSubjIndex, GETMODE_GETPART);
            return;
        }
        else if (index == 2)
        {
            // スレ参照... (親に対し、画面を抜けてスレを表示することを通知する)
            if ((status == favorManager.STATUS_NOTYET)||(status == favorManager.STATUS_RESERVE))
            {
                // スレ未取得なので、何もしない...
                return;
            }
            parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, favorManager.getCurrentMessage(currentSubjIndex));
            gettingLogDataFile = null;
            return;
        }
        else if (index == 3)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // お気に入り登録解除...
            Thread thread = new Thread()
            {
                public void run()
                {
                    // お気に入りの登録/登録解除を実施する
                    setResetFavoriteThread(currentSubjIndex, false);
                }
            };
            thread.start();
            return;
        }
        else if (index == 4)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // 表示している板一覧を切り替える
            Thread thread = new Thread()
            {
                public void run()
                {
                    // 板を切り替える準備を実行する...
                    subjectParser.leaveSubjectList(parent.getCurrentBbsIndex());

                    // 板URLを検索する
                    String boardNick = favorManager.getBoardNick(currentSubjIndex);
                    parent.moveToBoard(boardNick);

                    // BUSY表示を削除する
                    showInformationMode = -1;
                    informationString = null;
                    repaint();
                }
            };

            // BUSY表示する...
            showInformationMode = 0;
            informationString = "板一覧切り替え中...";
            repaint();
            
            // 板一覧切り替え処理の実行...
            thread.start();
            return;
        }
        else if (index == 5)
        {
            // ブラウザで開く アクション
            launchWebBrowserCurrentSelectedIndex();
            return;
        }
        else if (index == 6)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // 関連キーワード表示...
            String boardUrl = favorManager.getUrl(currentSubjIndex);
            int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
            String nick = boardUrl.substring(pos + 1);
            String url  = boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + favorManager.getThreadNumber(currentSubjIndex) + "/";
            showRelatedKeyword(url);
        }
        return;
    }

    
    /**
     * BUSY表示を行う...
     * 
     * @param message
     */
    public void setInformationMessage(int mode, String message)
    {
        // BUSY表示する...
        showInformationMode = mode;
        informationString   = message;
        repaint();
        return;
    }
    
    /**
     *  選択されたコマンドの実行
     * 
     * @param scene
     * @param index
     */
    public void executeCommandAction(byte scene, int index, int appendInfo)
    {
        // スレ一覧表示モード
        currentScene = SHOW_SUBJECTLIST;

        if (scene == parent.SHOW_SUBJECTCOMMAND)
        {
            // スレ操作モードの実行処理
            executeSubjectOperation(index);
        }
        else if (scene == parent.SHOW_SUBJECTCOMMANDFAVORITE)
        {
            // スレ操作モードの実行処理 (お気に入りモードのとき)
            executeSubjectOperationFavorite(index);
        }
        else if (scene == parent.CONFIRM_DELETELOGDAT)
        {
            // スレ削除モード
            if (index == 1)
            {
                // お気に入りの登録/登録解除を実施する
                setResetFavoriteThread(currentSubjIndex, false);

                // スレ削除の実行
                if (currentFavoriteLevel >= 0)
                {
                    // お気に入り...強制削除
                    subjectParser.deleteLogDataFile(-1, gettingLogDataFile);
                }
                else
                {
                    subjectParser.deleteLogDataFile(currentSubjIndex, gettingLogDataFile);
                }
                gettingLogDataFile = null;
                repaint();
/*
                // スレ削除の実行メイン...
                Thread thread = new Thread()
                {
                    public void run()
                    {
                        // お気に入りの登録/登録解除を実施する
                        setResetFavoriteThread(currentSubjIndex, false);

                        // スレ削除の実行
                        if (currentFavoriteLevel >= 0)
                        {
                            // お気に入り...強制削除
                            subjectParser.deleteLogDataFile(-1, gettingLogDataFile);
                        }
                        else
                        {
                            subjectParser.deleteLogDataFile(currentSubjIndex, gettingLogDataFile);
                        }
                        gettingLogDataFile = null;
                        repaint();
                    }
                };
                thread.start();                
*/
            }
        }
        else if (scene == parent.CONFIRM_SELECT_FIND2CH)
        {
            // 2ch検索...
            if (index >= 0)
            {
                // ２ちゃんねるスレッドの表示...
                String buffer = (String) pickup2chList.elementAt(index);
                int pos = buffer.indexOf("<>");
                if (pos >= 0)
                {
                    // スレ番号部分を切り出す..
                    pos = pos + 2;
                    String reservedThreadNumber = (String) buffer.substring(pos);

                    // 入力されたスレ番号の解析を依頼する
                    parent.parseMessageThreadNumber(reservedThreadNumber, appendInfo);
                }
            }
            pickup2chList = null;
            System.gc();
        }
        else if (scene == parent.CONFIRM_SELECT_RELATE_KEYWORD)
        {
            // 関連キーワードの選択...
            if (index >= 0)
            {
                // 関連キーワードを利用して、２ちゃんねる検索を実行する...
                String buffer = (String) pickup2chList.elementAt(index);
                int pos = buffer.indexOf("<>");
                if (pos >= 0)
                {
                    // URL部分を切り出す...
                    pos = pos + 2;
                    String str = (String) buffer.substring(pos);
                    gettingLogDataURL = "http://find.2ch.net/?BBS=ALL&TYPE=TITLE&ENCODING=SJIS&" + str + "&COUNT=25";
                    find2chThreadTitles();
                }
            }
            pickup2chList = null;
            System.gc();
        }
        return;
    }
    
    /**
     * アイテムが選択されたときの処理
     * 
     * @param autoOpen
     */
    public void selectedItem(boolean autoOpen)
    {
        int index = getSelectedIndex();
        if (index < 0)
        {
            // インデックス番号がおかしい、、、終了する
            return;
        }

        // 選択されたサブジェクトインデックスを記憶する
        parent.setSelectedSubjectIndex(index);
            
        if (currentScene != SHOW_SUBJECTLIST)
        {
            // スレ一覧表示をしていなかった、、終了する
            return;
        }
        // 表示モードを確認、一覧表示を切り替える
        if (currentFavoriteLevel < 0)
        {
            // スレ選択モードだった、スレ選択コマンドを表示する
            showActionChoiceGroup(index, autoOpen);
        }
        else
        {
            // お気に入りモードのとき、スレ選択モードだった、スレ選択コマンドを表示する
            showActionChoiceGroupFavorite(index, autoOpen);            
        }
        return;
    }

    /**
     *  お気に入り一覧:通常板一覧の切り替え
     * 
     *
     */
    private void toggleFavoriteList()
    {
        if (parent.isGetNewArrival() == true)
        {
            // 新着確認中は禁止する
            return;
        }
        // お気に入り一覧/通常板表示モードの切り替えを行う...
        parent.toggleFavoriteShowMode();
        currentFavoriteLevel = parent.getFavoriteShowLevel();

/**
        // 一覧を切り替えた場合には、スレタイ検索文字列をクリアする
        clearSearchTitleString();
**/

        // カーソルを先頭へ移動させる
        nofSubjectIndex     = 0;
        currentSelectedLine = 0;
        listCount           = 0;
        listDrawMode        = FORCE_REDRAW;

        // 表示リストを更新する
        updateTitleList(FORCE_REDRAW);

        return;
    }

    /**
     *  押した座標を処理する...
     * 
     * 
     */
    public void pointerPressed(int x, int y)
    {
        // 何かキーを押すと画面をクリアする
        boolean clearInfo = false;
        if (showInformationMode == 0)
        {
            showInformationMode = -1;
            informationString = null;
            repaint();
        }
        if (showInformationMode == 1)
        {
            // 数字入力を待つ...
            return;
        }

        int zoneWidth = (screenFont.charWidth('□') * 3);
        int index = y / (screenFont.getHeight() + 1);
        if (x > (getWidth() - zoneWidth))
        {
            // 画面の右上端、右下端をタップした場合...
            if (index == 0)
            {
                // 前ページに移動する
                listDrawMode = TO_PREVIOUS;
                repaint();
                return;
            }
            else if (index >= (listCount - 2))
            {
                // 次ページに移動する
                listDrawMode = TO_NEXT;
                repaint();
                return;
            }
        }
        if (x < zoneWidth)
        {
            // 画面の左上端、左下端をタップした場合...
            if (index == 0)
            {
                // 左上端、お気に入り一覧との切り替え...
                toggleFavoriteList();
                repaint();
                return;
            }
            else if (index >= (listCount - 2))
            {
                // 左下端、機能の実行
                executeFunctionFeature();
                repaint();
                return;
            }
        }
        
        if (currentSelectedLine == index)
        {
            // スレ選択、、、レスを読む...
            selectedItem(true);            
        }
        else
        {
            previousSelectedLine = currentSelectedLine;
            currentSelectedLine  = index;
        }
        
        if (clearInfo == true)
        {
            // 画面再描画
            listDrawMode        = FORCE_REDRAW;

            // 表示リストを更新し、再描画する
            updateTitleList(FORCE_REDRAW);
        }
        repaint();
        return;
    }

    /**
     *  離した座標を処理する...
     * 
     * 
     */
    public void pointerReleased(int x, int y)
    {
        return;
    }

    /**
     *  画面サイズ変更時に呼ばれる処理...
     * 
     */
    public void sizeChanged(int w, int h)
    {
        int topIndex  = -1;
        int bottomIndex = -1;

        if (displayIndexes != null)
        {
            topIndex = displayIndexes[0];
            if (listCount > 0)
            {
                bottomIndex = displayIndexes[listCount - 1];
            }
            if (topIndex > bottomIndex)
            {
                int tempIndex = topIndex;
                topIndex = bottomIndex;
                bottomIndex = tempIndex;
            }
        }

        displayIndexes = null;
        listCount      = -1;
        setLineWidthHeight(w, h);
        prepareScreenSize();
        displayIndexes[0] = topIndex;
        displayIndexes[screenLines - 1] = bottomIndex;
        listCount = screenLines;
        updateTitleList(FORCE_REDRAW);

        System.gc();
        repaint();

        return;
    }
    
    
    /**
     *   上を選択した場合...
     */
    private void inputUp(int count)
    {
        previousSelectedLine = currentSelectedLine;
        currentSelectedLine = currentSelectedLine - count;
        if (currentSelectedLine < 0)
        {
            if (parent.getCursorSingleMovingMode() == true)
            {
                if (displayIndexes[0] != 0)
                {
                    currentSelectedLine = listCount - 2;
                }
                if (currentSelectedLine < 0)
                {
                    currentSelectedLine = 0;
                }
                listDrawMode = TO_PREVIOUS;
            }
            else
            {
                currentSelectedLine = (listCount - 1);
            }
        }
        return;
    }
    
    /**
     *  下を選択した場合...
     *  
     * @param count
     */
    private void inputDown(int count)
    {
        previousSelectedLine = currentSelectedLine;
        currentSelectedLine = currentSelectedLine + count;
        if (currentSelectedLine > (listCount - 1))
        {
            if (parent.getCursorSingleMovingMode() == true)
            {
                listDrawMode = TO_NEXT;
                currentSelectedLine = 1;
            }
            else
            {
                currentSelectedLine = 0;                
            }
        }
        return;
    }
    

    private void eraseInformationMessage(int keyCode)
    {
        // 0キーを押した場合には、バイブレーションをOFFにする
        if (keyCode == Canvas.KEY_NUM0)
        {
            parent.setVibrateDuration(0);
        }

        // *キーを押した場合には、バイブレーションをON(300ms)にする
        if (keyCode == Canvas.KEY_STAR)
        {
            parent.setVibrateDuration(300);
        }

        // #キーを押した場合には、バイブレーションをON(1500ms)にする
        if (keyCode == Canvas.KEY_POUND)
        {
            parent.setVibrateDuration(1500);
        }

        // 何かキーを押すと画面をクリアする
        showInformationMode = -1;
        informationString = null;

        // readySubjectList();  // 新着確認終了時にスレ一覧を初期化する場合...
        
        // 画面再描画
        listDrawMode        = FORCE_REDRAW;

        // 表示リストを更新し、再描画する
        updateTitleList(FORCE_REDRAW);
        repaint();
        
        return;        
    }
   
    /**
     *  ゲームキーのアクション指定。。。
     * 
     * @param keyCode
     */
    private boolean gameKeyAction(int keyCode)
    {
        boolean repaint   = true;
        boolean processed = true;
        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            // 前ページに移動する
            listDrawMode = TO_PREVIOUS;
            break;
              
          case Canvas.RIGHT:
            // 次ページに移動する
            listDrawMode = TO_NEXT;
            break;

          case Canvas.DOWN:
            // カーソルをひとつ下に移動させる
            inputDown(1);
            break;

          case Canvas.UP:
            // カーソルをひとつ上に移動させる
            inputUp(1);
            break;

          case Canvas.FIRE:
            // スレ選択、、、レスを読む...
            selectedItem(true);
            break;

          case Canvas.GAME_A:
            // WX310SA : 文字ボタン
            // 3つ上を選択する
            inputUp(3);
            break;

          case Canvas.GAME_B:
            // WX310SA : Myキー
            // 3つ下を表示する
            inputDown(3);
            break;

          case Canvas.GAME_C:
            // WX310SA : クリアキー
            // 検索モード、ヘルプ表示モードを解除する
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_RESETMODE, -1);
            if (isHelpShowMode == true)
            {
                changeHelpShowMode();
                repaint = false;                
            }
            break;

          case Canvas.GAME_D:
            // WX310SA : サイドキー
            // ブラウザで開く
            launchWebBrowserCurrentSelectedIndex();
            break;

          default:
            repaint = false;
            processed = false;
            break;
        }
        if (repaint == true)
        {
            repaint();
        }            
        return (processed);
    }

    private boolean numberKeyMoveAction(int keyCode)
    {
        boolean repaint = true;
        boolean processed = true;
        
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            // 機能の実行
            executeFunctionFeature();
            break;

          case Canvas.KEY_NUM1:
            // お気に入り一覧：通常板一覧の切り替えを行う
            toggleFavoriteList();
            break;

          case Canvas.KEY_NUM2:
            // カーソルをひとつ上に移動させる
            inputUp(1);
            break;

          case Canvas.KEY_NUM3:
            if (parent.isGetNewArrival() == false)
            {
                // 板の選択... (ただし、新着確認中は動作しない)
                selectBoard();  
            }
            break;

          case 112:
          case 80:
               // p, Pキー
          case Canvas.KEY_NUM4:
            // 前ページに移動する
            listDrawMode = TO_PREVIOUS;
            break;

          case Canvas.KEY_NUM5:
            // スレ選択、、、レスを読む...
            selectedItem(true);
            break;

          case 110:
          case 78:
              // n,Nキー
          case Canvas.KEY_NUM6:
            // 次ページに移動する
            listDrawMode = TO_NEXT;
            break;

          case Canvas.KEY_NUM7:
            if (parent.isGetNewArrival() == false)
            {
                // スレ取得を実行する (新着確認中でない場合...)
                getLogDataFileImmediate();
            }
            break;

          case Canvas.KEY_NUM8:
            // カーソルをひとつ下に移動させる
            inputDown(1);
            break;

          case Canvas.KEY_NUM9:
            // 新着確認の実施...
            parent.startGetLogList();
            break;

          case 46:
          case Canvas.KEY_POUND:
            // . #マーク(末尾へ)
            if (nofSubjectIndex > 0)
            {
                // 無理やり末尾へ移動させる...
                listDrawMode      = TO_PREVIOUS;
                listCount         = 1;
                displayIndexes[0] = (nofSubjectIndex - 1);
            }
            break;

          case 44:
          case Canvas.KEY_STAR:
            // , *マーク (先頭へ)
            listCount = 0;
            listDrawMode = FORCE_REDRAW;
            break;

          case 111:
          case  79:
            // スレ操作モードを表示する
            selectedItem(false);
            break;

          case 119:
          case  87:
            // ブラウザで開く (W)
            launchWebBrowserCurrentSelectedIndex();
            break;

          case -5:
            // スレ選択、、、レスを読む...
            selectedItem(true);
            break;

          case -50:
            // ヘルプイメージを表示する (Nokia)
            changeHelpShowMode();
            break;

          case 8:
            // BSキー...
            if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // 実は、fireキーの入力と判断、、、ここでは何もしない
                return (false);
            }
            //not break!!
          case 27:
          case 113:
          case 81:
          case -8:
            // クリアキー for Nokia ... (現物合わせ...)
            // BSキー、ESCキー、Qキー
            // 検索モード、ヘルプ表示モードを解除する
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_RESETMODE, -1);
            if (isHelpShowMode == true)
            {
                changeHelpShowMode();
                repaint = false;
            }
            break;

          default:
            repaint = false;
            processed = false;
            break;
        }
        if (repaint == true)
        {
            repaint();
        }
        return (processed);
    }
    
    private boolean numberKeyNormalAction(int keyCode)
    {
        Thread thread = null;
        boolean repaint   = true;
        boolean processed = true;
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            // 機能の実行
            executeFunctionFeature();
            break;

          case Canvas.KEY_NUM1:
            // お気に入り一覧/通常板表示モードの切り替えを行う
            toggleFavoriteList();
            break;

          case Canvas.KEY_NUM2:
            if (parent.isGetNewArrival() == false)
            {
                // 板カテゴリの選択...(新着確認中は動作しない)
                selectCategory();            
            }
            break;

          case Canvas.KEY_NUM3:
            if (parent.isGetNewArrival() == false)
            {
                // 板の選択...(新着確認中は禁止する)
                selectBoard();            
            }
            break;

          case Canvas.KEY_NUM4:
            if (parent.isGetNewArrival() == false)
            {
                // スレ情報の表示（新着確認中は禁止する）
                showMessageInformation();
            }
            break;

          case Canvas.KEY_NUM5:
            if (parent.isGetNewArrival() == false)
            {
                // スレ検索 (新着確認中は禁止する)
                startSearchTitle();
            }
            break;

          case Canvas.KEY_NUM6:
            if (currentFavoriteLevel < 0)
            {
                // 検索モードの切り替えを行う
                // (お気に入り表示モードなら、このコマンドは受け付けない)
                setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_ONLYDATABASE, -1);
            }
            break;

          case Canvas.KEY_NUM7:
           if (parent.isGetNewArrival() == false)
            {
                // スレ取得を実行する(新着確認中は禁止する)
                getLogDataFileImmediate();
            }
            break;

          case Canvas.KEY_NUM8:
            if ((parent.isGetNewArrival() == false)&&(currentFavoriteLevel < 0))
            {
                // 新着確認中でなく、お気に入り表示モードでもないとき、スレ一覧を取得する。
                thread = new Thread()
                {
                    public void run()
                    {
                        // スレ一覧の取得を実施する
                        subjectParser.prepareSubjectListUpdate(parent.getCurrentBbsIndex());
                    }
                };
                setBusyMessage("subject.txt取得中...", true);
                thread.start();
                parent.setSelectedSubjectIndex(0);
            }
            break;

          case Canvas.KEY_NUM9:
            // 新着確認の実施...
            parent.startGetLogList();
            break;
                
          case 46:
          case Canvas.KEY_POUND:
            // #マーク(末尾へ)
            if (nofSubjectIndex > 0)
            {
                // 無理やり末尾へ移動させる...
                listDrawMode      = TO_PREVIOUS;
                listCount         = 1;
                displayIndexes[0] = (nofSubjectIndex - 1);
            }
            break;

          case 44:
          case Canvas.KEY_STAR:
            // *マーク (先頭へ)
            listCount = 0;
            listDrawMode = FORCE_REDRAW;
            break;

          case 111:
          case  79:
            // スレ操作モードを表示する
            selectedItem(false);
            break;

          case -50:
            // ヘルプイメージを表示する (Nokia)
            changeHelpShowMode();
            break;

          case 112:
          case 80:
            // p, Pキー
            // 前ページに移動する
            listDrawMode = TO_PREVIOUS;
            break;

          case 110:
          case 78:
            // n,Nキー
            // 次ページに移動する
            listDrawMode = TO_NEXT;
            break;

          case -5:
            // スレ選択、、、レスを読む...
            selectedItem(true);
            break;

          case 119:
          case  87:
            // ブラウザで開く (W)
            launchWebBrowserCurrentSelectedIndex();
            break;

          case 8:
            // BSキー
              if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // 実は、fireキーの入力と判断、、、ここでは何もしない
                return (false);
            }
            //not break!!
          case 27:
          case 113:
          case 81:
          case -8:
            // クリアキー for Nokia ... (現物合わせ...)
            // ESCキー、Qキー
            // 検索モード、ヘルプ表示モードを解除する
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_RESETMODE, -1);
            if (isHelpShowMode == true)
            {
                changeHelpShowMode();
                repaint = false;
            }
            break;

          default:
            repaint   = false;
            processed = false;
            break;
        }
        if (repaint == true)
        {
            repaint();
        }
        return (processed);        
    }
    
    
    
    /**
     *   キー入力の処理
     * 
     * 
     */
    public void keyPressed(int keyCode)
    {
        parent.keepWatchDog(0);
        
        if (showInformationMode == 0)
        {
            // 新着確認の直後にキー入力したとき...
            // (表示を消去する)
            eraseInformationMessage(keyCode);
            return;
        }
        if (showInformationMode == 1)
        {
            // お気に番号の入力待ち...
            showInformationMode = -1;
            informationString = null;
            if ((keyCode >= Canvas.KEY_NUM0)&&(keyCode <= Canvas.KEY_NUM9))
            {
                // お気に番号を更新する
                currentFavoriteLevel = keyCode - Canvas.KEY_NUM0;
            }
            // 表示リストを更新し、再描画する
            updateTitleList(FORCE_REDRAW);
            repaint();
            return;
        }
        
        if (keyCode == 0)
        {
            // キー入力がなかった...終了する
            return;
        }

        boolean processed = false;
        if (isNumberMovingMode == true)
        {
            processed = numberKeyMoveAction(keyCode);
        }
        else
        {
            processed = numberKeyNormalAction(keyCode);
        }
        if (processed == false)
        {
            gameKeyAction(keyCode);
        }        
        return;
    }

    /**
     *   キーリピートしているとき...
     * 
     * 
     */
    public void keyRepeated(int keyCode)
    {
        parent.keepWatchDog(0);

        boolean repaint = true;
        if (keyCode == 0)
        {
            return;
        }

        if (isNumberMovingMode == true)
        {
            repaint = false;
            switch (keyCode)
            {
              case Canvas.KEY_NUM2:
                // カーソルをひとつ上に移動させる
                inputUp(1);
                break;

              case Canvas.KEY_NUM4:
                // 前ページに移動する
                listDrawMode = TO_PREVIOUS;
                break;

              case Canvas.KEY_NUM6:
                // 次ページに移動する
                listDrawMode = TO_NEXT;
                break;

              case Canvas.KEY_NUM8:
                // カーソルをひとつ下に移動させる
                inputDown(1);
                break;

              default:
                repaint = true;
                break;
            }
            if (repaint != true)
            {
                repaint();
                return;
            }
        }

        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            // 前ページに移動する
            listDrawMode = TO_PREVIOUS;
            break;
              
          case Canvas.RIGHT:
            // 次ページに移動する
            listDrawMode = TO_NEXT;
            break;

          case Canvas.DOWN:
            // カーソルをひとつ下に移動させる
            inputDown(1);
            break;

          case Canvas.UP:
            // カーソルをひとつ上に移動させる
            inputUp(1);
            break;

          case Canvas.FIRE:
            // セレクトボタン
            break;

          case Canvas.GAME_A:
            // WX310SA : 文字ボタン
            // 3つ上を選択する
            inputUp(3);
            break;

          case Canvas.GAME_B:
            // WX310SA : Myキー
            // 3つ下を表示する
            inputDown(3);
            break;

          case Canvas.GAME_C:
            // WX310SA : クリアキー
            break;

          case Canvas.GAME_D:
            break;

          default:
            break;
        }        
        if (repaint == true)
        {
            repaint();
        }
        return;
    }
    
    
    /**
     *  スレ検索の実行...
     *
     */
    private void startSearchTitle()
    {
        int index = getSelectedIndex();
        if ((index >= 0)&&((temporaryTitleString == null)||(temporaryTitleString.length() == 0)))
        {
            // 現在選択中のスレタイトルを反映させる（表示モードを確認）
            if (currentFavoriteLevel < 0)
            {
                // スレ選択モードだった
                temporaryTitleString = trimTitleString(subjectParser.getSubjectName(index));
            }
            else
            {
                // お気に入りモードのとき
                temporaryTitleString = trimTitleString(favorManager.getSubjectName(index));            
            }
        }
        parent.OpenWriteFormUsingWordInputMode(parent.SCENE_TITLE_SEARCH, "スレタイトル検索", "タイトル", temporaryTitleString, true);
    }

    /**
     *  
     * @param data スレタイトル
     * @return （加工済み）タイトル文字列
     */
    private String trimTitleString(String data)
    {
        if (data == null)
        {
            return (null);
        }
       
        int pos1 = data.indexOf("【");
        int pos2 = data.indexOf("】");
        String cutData = data;
        String andData = "";
        if (pos1 >= 0)
        {
            andData = cutData.substring(0, pos1);
        }
        try
        {
            while ((pos1 >= 0)&&(pos2 >= 0)&&(pos2 > pos1))
            {
                cutData = cutData.substring(pos2 + 1);
                pos1 = cutData.indexOf("【");
                if (pos1 >= pos2)
                {
                    andData = andData + cutData.substring(0, pos1);
                }
                pos2 = cutData.indexOf("】");
            }
            if (cutData.length() != 0)
            {
                if (andData.length() != 0)
                {
                    andData = andData + " " + cutData;
                }
                else
                {
                    andData = cutData;
                }
            }
        }
        catch (Exception e)
        {
            //
        }
        if (andData.length() == 0)
        {
            andData = cutData;
        }
        return (andData);
    }

    /**
     *  検索文字列を指定したものに変更する
     *
     */
    public void setSearchTitleTemporary(String data)
    {
        temporaryTitleString = data;
        return;
    }

    /**
     * 
     *  機能キー（数字キーのゼロ）を押したときの処理...
     * 
     *
     */
    private void executeFunctionFeature()
    {
        int feature = parent.getFunctionKeyFeature(parent.SCENE_LISTFORM);
        if (feature == 0)
        {
            // フォント変更
            changeFont(false);            
        }
        else if (feature == 1)
        {
            // 数字キー移動モード切替
            parent.toggleNumberInputMode(isNumberMovingMode);
            repaint();    
        }
        else if (feature == 2)
        {
            // ヘルプ表示モード切替
            changeHelpShowMode();
        }
        else if (feature == 3)
        {
            // スレ情報表示(新着確認中は禁止)
            if (parent.isGetNewArrival() != true)
            {
                showMessageInformation();
            }
        }
        else if (feature == 4)
        {
            // スレ検索(未読スレ抽出)
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_REMAIN, OPTION_RESETMODE, -1);
        }
        else if (feature == 5)
        {
            // スレ操作メニュー(新着確認中は禁止)
            if (parent.isGetNewArrival() != true)
            {
                selectedItem(false);
            }
        }
        else if (feature == 6)
        {
            // モードメニュー
            showMenuSecond();
        }
        else if (feature == 7)
        {
            // ログ参照モード
            Thread thread = new Thread()
            {
                public void run()
                {
                    // 親に対し、画面を抜けたことを通知する
                    parent.EndListFormLogViewMode();
                }
            };
            thread.start();
        }
        else if (feature == 8)
        {
            // クリアキーと等価な処理...
            // 検索モード、ヘルプ表示モードを解除する
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_RESETMODE, -1);
            if (isHelpShowMode == true)
            {
                changeHelpShowMode();            
            }
        }
        else if (feature == 9)
        {
            // 関連キーワード抽出機能...
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }

            // 取得先URLを生成し、スレタイトルと連結する
            String url = "";
            int index = getSelectedIndex();
            if (currentFavoriteLevel >= 0)
            {
                // お気に入り一覧を表示中...
                String boardUrl = favorManager.getUrl(index);
                int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
                String nick = boardUrl.substring(pos + 1);
                url  = boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + favorManager.getThreadNumber(index) + "/";
            }
            else
            {
                // 通常の板一覧を表示中...
                String boardUrl = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
                int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
                String nick = boardUrl.substring(pos + 1);
                url  = boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + subjectParser.getThreadNumber(index) + "/";
            }
            // 関連キーワード表示...
            showRelatedKeyword(url);
        }
        else if (feature == 10)
        {
            // スレ削除
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            deleteMessage();
        }
        else if (feature == 11)
        {
            // ●/Be@2chログイン
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            showLoginMenu(false);
        }
        return;
    }

    /**
     *  コマンドボタン実行時の処理
     *  
     */
    public void commandAction(Command c, Displayable d)
    {
        // 新着確認中は、コマンドボタンの使用を禁止する
        if (parent.isGetNewArrival() == true)
        {
            // 新着確認中にメニューの新着確認ボタンが押されたら中止する。
            if (c == getAllCmd)
            {
                isAbortNewArrival = true;
            }
            return;
        }    

        // コマンドボタン実行処理
        if (c == modeCmd)
        {
            // メニュー２
            showMenuSecond();
            return;
        }
        if (c == srchCmd)
        {
            // スレ検索
            startSearchTitle();
            return;
        }
        else if (c == opeCmd)
        {
            // スレ操作
            selectedItem(false);
            return;
        }        
        else if (c == getAllCmd)
        {
            // 新着確認の実施...
            parent.startGetLogList();
            return;
        }
        else if (c == proxyCmd)
        {
            // a2B設定(Proxy設定)
            parent.OpenWriteFormUsingProxySetting();
            return;
        }
        else if (c == catCmd)
        {
            // 板カテゴリの選択...
            selectCategory();            
            return;
        }
        else if (c == selCmd)
        {
            // 板の選択...
            selectBoard();            
            return;
        }
        else if (c == quitCmd)
        {
            // a2B終了
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.quit_a2B();
                }
            };
            try
            {
                setBusyMessage("a2B終了中...", true);
                thread.start();
                thread.join();
            }
            catch (Exception ex)
            {
                // 何もしない
            }
            return;
        }
        else if (c == deleteCmd)
        {
            // スレ削除
            deleteMessage();
            return;
        }

        // お気に入り表示モードかどうかのチェック
        if (currentFavoriteLevel >= 0)
        {
            // お気に入り表示モードなら、これ以降のコマンドは受け付けない
            return;
        }

        //////////  以降は通常表示モードのみで有効 /////////
        if (c == getCmd)
        {
            // スレ一覧の取得
            Thread thread = new Thread()
            {
                public void run()
                {
                    // スレ一覧の取得を実施する
                    subjectParser.prepareSubjectListUpdate(parent.getCurrentBbsIndex());
                }
            };
            setBusyMessage("subject.txt取得中...", true);
            thread.start();
            parent.setSelectedSubjectIndex(0);
            return;    
        }
        return;
    }

    /**
     *   フォントサイズの変更
     * 
     */
    public void changeFont(boolean isReset)
    {
        int size;
        if (isReset == false)
        {
            
            // フォントサイズの変更 (小→中→大→小→... と切り替える)
            size   = screenFont.getSize();
            switch (size)
            {
              case Font.SIZE_LARGE:
                size = Font.SIZE_SMALL;
                break;

              case Font.SIZE_MEDIUM:
                size = Font.SIZE_LARGE;
                break;

              case Font.SIZE_SMALL:
                size = Font.SIZE_MEDIUM;
                break;

              default:
                size = Font.SIZE_SMALL;
                break;
            }
        }
        else
        {
            // フォントサイズをリセットする
            size = parent.getListFontSize();
        }
        try
        {
            screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), size);
        }
        catch (Exception ex)
        {
        	size = Font.SIZE_SMALL;
            screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), size);
        }
        parent.setListFontSize(size);

        // 表示可能行を再設定する
        setLineWidthHeight(getWidth(), getHeight());

        // 表示リストを更新する
        updateTitleList(FORCE_REDRAW);

        // 再表示
        repaint();

        return;
    }
        
    /**
     *  一覧表示を行う(お気に入り表示モード)
     *  
     */
    private void showListFavorite(Graphics g)
    {
        // 板一覧を表示中...
        currentScene = SHOW_SUBJECTLIST;
        
        // お気に入り一覧に含まれているスレ数を獲得
        int subjects = favorManager.getNumberOfFavorites();
        if (nofSubjectIndex != subjects)
        {
            currentSelectedLine = 0;
            nofSubjectIndex     = subjects;
            listDrawMode        = FORCE_REDRAW;
        }

        // 板タイトルを表示する
        if (currentFavoriteLevel == 0)
        {
            setTitle("【 お気に入り 】");
        }
        else
        {
            setTitle("【 お気に入り(" + currentFavoriteLevel + ") 】");            
        }
/**
        g.setColor(0);
        g.fillRect(0, 0, getWidth(), screenFont.getHeight() + 1);
        g.setFont(screenFont);
        g.setColor(0x00ffffff);
        g.drawString("【 お気に入り 】", 0, 0, (Graphics.LEFT | Graphics.TOP));
**/

        // お気に入りが登録されていないのか？
        if (nofSubjectIndex == 0)
        {
            // 画面の塗りつぶし
            g.setColor(0x00f8f8f8);
            g.fillRect(0, screenFont.getHeight() + 1, getWidth(), getHeight());
            g.setColor(0);
            g.setFont(screenFont);

            // スレ未取得の場合には、折り返す
            g.setColor(0x00777777);
            g.drawString("(登録なし)", 0, screenFont.getHeight() + 2, (Graphics.LEFT | Graphics.TOP));

            return;
        }

        // スレ一覧のページが変わる場合には、タイトル一覧を更新する
        if ((listDrawMode != TO_REDRAW)||(listCount <= 0))
        {
            updateTitleList(listDrawMode);
        }

        // フォントを設定する
        g.setFont(screenFont);

        // カーソル移動時の更新...
        if ((listDrawMode == TO_REDRAW)&&(previousSelectedLine != currentSelectedLine))
        {

            // カーソルが移動されたときは、その部分のみ再描画する。
            if (previousSelectedLine >= 0)
            {
                String title = favorManager.getSubjectSummary(displayIndexes[previousSelectedLine]);
                byte status = favorManager.getStatus(displayIndexes[previousSelectedLine]);
                drawTitleSummary(g, previousSelectedLine, title, status);
            }
            if (currentSelectedLine >= 0)
            {
                String title = favorManager.getSubjectSummary(displayIndexes[currentSelectedLine]);
                byte status = favorManager.getStatus(displayIndexes[currentSelectedLine]);
                drawTitleSummary(g, currentSelectedLine, title, status);
                previousSelectedLine = currentSelectedLine;
            }

            // 一覧表示中の場所を示すマークをつける
            markArea(g, lastSubjectIndex);
            return;
        }
                
        // 画面の塗りつぶし
        g.setColor(0x00f8f8f8);
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setColor(0);
        
        // スレタイトル一覧の表示...
        int drawLine         = 0;
        for (int loop = 0; loop < screenLines; loop++)
        {
            int index = displayIndexes[loop];
            if (index >= 0)
            {
                String title    = favorManager.getSubjectSummary(index);
                byte status    = favorManager.getStatus(index);
                if (drawTitleSummary(g, drawLine, title, status) == true)
                {
                    lastSubjectIndex = index;
                    drawLine++;
                }    
            }
        }
        
        // 一覧表示中の場所を示すマークをつける
        markArea(g, lastSubjectIndex);

        listDrawMode = TO_REDRAW;
        return;
    }

    /**
     *      一覧表示を行う(メイン処理)
     *   @param g
     */
    private void showList(Graphics g)
    {
        
        // 現在のカテゴリと板名を表示する
        int boardIndex = parent.getCurrentBbsIndex();
        if (boardIndex < 0)
        {
            return;
        }
        int categoryIndex = parent.getCurrentCategoryIndex();
        if (categoryIndex < 0)
        {
            return;
        }

        // 板一覧を表示中...
        currentScene = SHOW_SUBJECTLIST;
        
        // 板一覧に含まれているスレ数を獲得
        int subjects = subjectParser.getNumberOfSubjects();
        if (nofSubjectIndex != subjects)
        {
            currentSelectedLine = 0;
            nofSubjectIndex     = subjects;
            listDrawMode        = FORCE_REDRAW;
        }

        // 板タイトルを表示する
        String bbsTitle = "【 " + bbsCommunicator.getCategory(categoryIndex) + " ： ";
        bbsTitle = bbsTitle + bbsCommunicator.getBoardName(boardIndex) + " 】";
        setTitle(bbsTitle);
/**
        g.setColor(0);
        g.fillRect(0, 0, getWidth(), screenFont.getHeight() + 1);
        g.setFont(screenFont);
        g.setColor(0x00ffffff);
        g.drawString(bbsTitle, 0, 0, (Graphics.LEFT | Graphics.TOP));
**/

        // スレ一覧取得済みか？
        if (nofSubjectIndex == 0)
        {
            // 画面の塗りつぶし
            g.setColor(0x00f8f8f8);
            g.fillRect(0, 0, getWidth(), getHeight());
            g.setColor(0, 0, 0);
            g.setFont(screenFont);

            // スレ未取得の場合には、折り返す
            g.setColor(0x00777777);
            g.drawString("(未取得)", 0, 2, (Graphics.LEFT | Graphics.TOP));
            busyMessage = "";
            return;
        }

        // スレ一覧のページが変わる場合には、タイトル一覧を更新する
        if ((listDrawMode != TO_REDRAW)||(listCount <= 0))
        {
            updateTitleList(listDrawMode);
        }

        // フォントを設定する
        g.setFont(screenFont);

        // カーソル移動時の更新...
        if ((listDrawMode == TO_REDRAW)&&(previousSelectedLine != currentSelectedLine))
        {
            // カーソルが移動されたときは、その部分のみ再描画する。
            if (previousSelectedLine >= 0)
            {
                String title    = subjectParser.getSubjectSummary(displayIndexes[previousSelectedLine]);
                byte status    = subjectParser.getStatus(displayIndexes[previousSelectedLine]);    
                drawTitleSummary(g, previousSelectedLine, title, status);
            }
            if (currentSelectedLine >= 0)
            {
                String title    = subjectParser.getSubjectSummary(displayIndexes[currentSelectedLine]);
                byte status    = subjectParser.getStatus(displayIndexes[currentSelectedLine]);
                drawTitleSummary(g, currentSelectedLine, title, status);
                previousSelectedLine = currentSelectedLine;
            }

            // 一覧表示中の場所を示すマークをつける
            markArea(g, lastSubjectIndex);
            return;
        }
                
        // 画面の塗りつぶし
        g.setColor(0x00f8f8f8);
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setColor(0);
        
        // スレタイトル一覧の表示...
        int drawLine         = 0;
        for (int loop = 0; loop < screenLines; loop++)
        {
            int index = displayIndexes[loop];
            if (index >= 0)
            {
                String title  = subjectParser.getSubjectSummary(index);
                byte status  = subjectParser.getStatus(index);
                if (drawTitleSummary(g, drawLine, title, status) == true)
                {
                    lastSubjectIndex = displayIndexes[loop];
                    drawLine++;
                }    
            }
        }
        
        // 一覧表示中の場所を示すマークをつける
        markArea(g, lastSubjectIndex);

        listDrawMode = TO_REDRAW;
        return;
    }

    /**
     *  Webブラウザで開く
     *
     */
    public void launchWebBrowserCurrentSelectedIndex()
    {
        int   index;
        long  threadNumber;
        int   current;
        String url;
        if (currentFavoriteLevel >= 0)
        {
            index = currentSubjIndex;
            url =  favorManager.getUrl(index);    
            threadNumber = favorManager.getThreadNumber(index);
            current = favorManager.getCurrentMessage(index);
        }
        else
        {
            index = getSelectedIndex();
            int boardIndex = parent.getCurrentBbsIndex();
            url = bbsCommunicator.getBoardURL(boardIndex);
            threadNumber = subjectParser.getThreadNumber(index);
            current = subjectParser.getCurrentMessage(index);
        }
        int pos = url.indexOf("://");
        pos = pos + 4;
        pos = url.indexOf("/", pos);
        String nick = url.substring(pos);

        if (url.indexOf("2ch.net") < 0)
        {
            // 2ちゃんねる外のURLを読んでいた場合...
            url = url.substring(0, pos) + "/test/read.cgi" + nick + threadNumber + "/" + current + "n-";
        }
        else
        {
            // ２ちゃんなる内だった場合には、クラッシックメニューを利用する
            url = "http://c.2ch.net/test/-" + nick + threadNumber + "/" + current + "-";
            //url = "http://c-others.2ch.net/test/-" + nick + threadNumber + "/" + current + "-";
        }
        parent.updateBusyMessage("Open URL", "URL>>" + url, false);
        parent.openWebBrowser(url);

        return;
    }
    
    /**
     * ビジーメッセージの設定...
     * @param message
     */
    public void setBusyMessage(String message, boolean forceUpdate)
    {
        busyMessage = message;
        repaint();
        if (forceUpdate == true)
        {
            serviceRepaints();
        }
        return;
    }

    /**
     * ビジーメッセージ（複数行）を表示する
     * 
     * @param g
     * @param title
     */
    private void drawBusyAreaMultiple(Graphics g, int topHeight)
    {
        g.setColor(0x00800080);    // 紫
        int offset    = 0;
        int width     = screenWidth - screenFont.stringWidth("XX");
        int maxLength = busyMessage.length();
        int maxHeight = getHeight();
        while (topHeight < maxHeight)
        {
            int length  = 0;
            while ((length + offset < maxLength)&&(screenFont.substringWidth(busyMessage, offset, length) < width))
            {
                length++;
            }
            g.drawSubstring(busyMessage, offset, length, 1, topHeight, (Graphics.LEFT | Graphics.TOP));
            if (length >= maxLength)
            {
                break;
            }
            offset = offset + length;
            topHeight = topHeight + screenFont.getHeight();
        }
    }

    /**
     *   一覧表示中の場所を示すマークをつける
     * 
     * 
     */
    private void markArea(Graphics g, int index)
    {
        // 一覧表示中の場所を示すマークをつける
        int drawLine = (screenWidth - 3);
        int height   = screenLines * (screenFont.getHeight() + 1);

        g.setColor(0x00ffffff);
        g.fillRect(drawLine, 0, drawLine + 3, height);
        if (currentFavoriteLevel >= 0)
        {
            // お気に入り表示モードのときは、色を変える
            g.setColor(0x0000cddf);
        }
        else
        {
            g.setColor(0);
        }            
        g.drawLine(drawLine, 0, drawLine, height);
        int start = nofSubjectIndex;
        if (start != 0)
        {
            int par = (index * height) / start;
//            float par = ((float) index / (float) start) * ((float) height);
            start = (int) par - 2;
            if (start < 0)
            {
                start = 0;
            }
            if (searchTitlePickupHour > 0)
            {
                g.setColor(0x00ddccff);
            }
            else if (searchTitleMessageStatus == NO_SEARCHING_MODE)
            {
                g.setColor(0x00000000);
            }
            else if (searchTitleMessageStatus == SEARCHING_ONLYDB)
            {
                // 検索モードのときは、色を変える
                g.setColor(0x00ff00ff);
            }
            else
            {
                // 検索モードのときは、色を変える
                g.setColor(0x00ff0000);
            }
            g.fillRect(drawLine, start, 3, 8);
            g.setColor(0x00000000);
        }
        return;
    }

    /**
     *  スレタイトルを描画する
     * 
     * 
     */
    private boolean drawTitleSummary(Graphics g, int line, String title, byte status)
    {
        int foreColor  = getForeColor(status, line);
        int backColor  = getBackColor(status, line);
        int fontHeight = screenFont.getHeight();
        int stringTop  =  (line * (fontHeight+ 1));
        int width      = screenWidth - 3;
        if (title != null)
        {
            if (line == currentSelectedLine)
            {
                g.setColor(foreColor);
                g.fillRect(0, stringTop, width, fontHeight + 2);
                g.setColor(backColor);

                // スレタイ複数行表示モードの場合...
                if ((busyAreaSize > 1)&&(parent.isGetNewArrival() != true))
                {
                    // 下部エリアにスレタイを表示する。。。
                    int pos = title.indexOf("]");
                    if (pos >= 0)
                    {
                        busyMessage = title.substring(pos + 1);
                    }
                    else
                    {
                        busyMessage = title;
                    }
                }        

                int offset = 0;
                int length = title.length();
                while (screenFont.substringWidth(title, offset, (length - offset)) > width)
                {
                    offset++;
                }
                g.drawSubstring(title, offset, (length - offset), 0, stringTop, (Graphics.LEFT | Graphics.TOP));
            }
            else
            {
                g.setColor(backColor);
                g.fillRect(0, stringTop, width, fontHeight + 2);
                g.setColor(foreColor);
                g.drawString(title, 0, stringTop, (Graphics.LEFT | Graphics.TOP));
            }
            title = null;
            g.setColor(0);
            return (true);
        }
        return (false);
    }

    /**
     *  スレタイトル一覧を作成する...
     * 
     * 
     */
    private boolean updateTitleList(int command)
    {
        int start, update;

        if (listCount > 0)
        {
            // 先頭と末尾のインデックス番号を取得する
            int topIndex    = displayIndexes[0];
            int bottomIndex = displayIndexes[listCount - 1];
            if (topIndex > bottomIndex)
            {
                int tempIndex = topIndex;
                topIndex = bottomIndex;
                bottomIndex = tempIndex;
            }
            if (command == TO_PREVIOUS)
            {
                // 前ページのデータを作成する...
                start  = topIndex;
                update = -1;
                if (start == 0)
                {
                    // 既に先頭にいる、再構築しない
                    return (false);
                }
            }
            else if (command == TO_NEXT)
            {
                // 次ページのデータを作成する
                start  = bottomIndex;
                update = 1;
            }
            else   // if (command == FORCE_REDRAW)
            {
                // 再描画する
                start = topIndex;
                update = 1;
            }
        }
        else
        {
            // 先頭からデータを作成する
            start     = 0;
            update    = 1;
            listCount = 0;
        }

        listCount = 0;
        for (int loop = start; ((loop >= 0)&&(loop < nofSubjectIndex)&&(listCount < screenLines)); loop = loop + update)
        {
            // 表示すべきかどうかチェックする
            boolean check = false;
            if (currentFavoriteLevel < 0)
            {
                check = canShowTitle(loop);
            }
            else
            {
                check = canShowTitleFavorite(loop);
            }
            if (check == true)
            {
                displayIndexes[listCount] = loop;
                listCount++;
            }
        }
        if (command == TO_PREVIOUS)
        {
            // 逆順で検索した場合には、正しい順番に並べ替えを行う...
            for (int loop = 0; loop < listCount / 2; loop++)
            {
                int temp = displayIndexes[loop];
                displayIndexes[loop] = displayIndexes[(listCount - 1) -loop];
                displayIndexes[(listCount - 1) -loop] = temp;
            }
        }

        // スレタイトルのあまり部分には負の値を入れる
        for (int loop = listCount; loop < screenLines; loop++)
        {
            displayIndexes[loop] = -1;
        }
        return (true);
    }
    
    /**
     *   選択されたスレのインデックス番号を応答する
     * 
     * 
     */
    private int getSelectedIndex()
    {
        if ((currentSelectedLine < 0)||(currentSelectedLine > screenLines))
        {
            return (-1);
        }        
        return (displayIndexes[currentSelectedLine]);
    }
    

    /**
     *  タイトルを表示すべきかどうかのチェックを行う
     * 
     * 
     */
    private boolean canShowTitle(int index)
    {
        // スレ経過日数が設定している場合...
        if (searchTitlePickupHour > 0)
        {
            long dateTime = getProgressDate(subjectParser.getThreadNumber(index));
            if (dateTime <= searchTitlePickupHour)
            {
                return (true);
            }
            if (searchTitleString == null)
            {
                // 検索文字列が設定されていない場合には、抽出しない
                return (false);
            }
        }

        // 検索モードがOFFのときは終了する
        if ((searchTitleMessageStatus == NO_SEARCHING_MODE)&&(searchTitleString == null))
        {
            return (true);
        }

        // スレ状態の状態を検出する...
        byte status = subjectParser.getStatus(index);
        if (status > searchTitleMessageStatus)
        {
            return (false);
        }

        // 検索文字列が設定されてない場合にはOK
        if (searchTitleString == null)
        {
            return (true);
        }
        
        // タイトルを取得
        String title  = subjectParser.getSubjectName(index);
        if (title != null)
        {
            if (searchIgnoreCase != true)
            {
                // 検索文字列が含まれているか確認する
                if (title.indexOf(searchTitleString) >= 0)
                {
                    return (true);
                }
                return (false);
            }
    
            // 検索文字列が含まれているか確認する(大文字/小文字区別なし)
            String target    = title.toLowerCase();
            String matchData = searchTitleString.toLowerCase();
            if (target.indexOf(matchData) >= 0)
            {
                return (true);
            }            
        }
        return (false);
    }
    
    /**
     *  タイトルを表示すべきかどうかのチェックを行う(お気に入り表示モードのとき)
     * 
     * 
     */
    private boolean canShowTitleFavorite(int index)
    {

        // お気に入り表示レベルの確認
        int level = favorManager.getThreadLevel(index);
        if ((currentFavoriteLevel != 0)&&(currentFavoriteLevel != level))
        {
            return (false);
        }

        // スレ経過日数が設定している場合...
        if (searchTitlePickupHour > 0)
        {
            long dateTime = getProgressDate(subjectParser.getThreadNumber(index));
            if (dateTime <= searchTitlePickupHour)
            {
                return (true);
            }
            if (searchTitleString == null)
            {
                // 検索文字列が設定されていない場合には、抽出しない
                return (false);
            }
        }

        // 検索モードがOFFのときは終了する
        if ((searchTitleMessageStatus == NO_SEARCHING_MODE)&&(searchTitleString == null))
        {
            return (true);
        }

        // スレ状態の状態を検出する...
        byte status = favorManager.getStatus(index);
        if (status > searchTitleMessageStatus)
        {
            return (false);
        }

        // 検索文字列が設定されてない場合にはOK
        if (searchTitleString == null)
        {
            return (true);
        }
        
        // タイトルを取得
        String title  = favorManager.getSubjectName(index);
        if (title != null)
        {
            if (searchIgnoreCase != true)
            {
                // 検索文字列が含まれているか確認する
                if (title.indexOf(searchTitleString) >= 0)
                {
                    return (true);
                }
                return (false);
            }
    
            // 検索文字列が含まれているか確認する(大文字/小文字区別なし)
            String target    = title.toLowerCase();
            String matchData = searchTitleString.toLowerCase();
            if (target.indexOf(matchData) >= 0)
            {
                return (true);
            }            
        }        
        return (false);
    }

    /**
     * 板カテゴリ選択処理...
     * 
     */
    private void selectCategory()
    {
        setBusyMessage("板カテゴリ整列中...", true);

        // 板カテゴリ選択の実施
        Thread thread = new Thread()
        {
            public void run()
            {
                // ビジーフォームを表示する...
                setTitle("板カテゴリ整列中...");
                
                // 板一覧情報をクリアする
                subjectParser.leaveSubjectList(parent.getCurrentBbsIndex());

                // 今はカテゴリ選択モード...
                currentScene = parent.SHOW_CATEGORYLIST;

                int limit = bbsCommunicator.getNumberOfCategory();
                Vector categoryList = new Vector(limit);
                for (int loop = 0; loop < (limit - 2); loop++)
                {
                    // カテゴリ一覧を取得する (2個減らしている理由は、「他のサイト」と「まちBBS」を削っているため)
                    // (注意！ a2Bでは動かないカテゴリがある！！）
                    String categoryName = bbsCommunicator.getCategory(loop);
                    categoryList.addElement(categoryName);
                }

                // カテゴリ一覧を表示する
                int index = parent.getCurrentCategoryIndex();
                parent.openSelectForm("板カテゴリを選択してください", categoryList, index, currentScene);

                categoryList = null;
                return;
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
            // 何もしない...
        }
        return;
    }

    /**
     *  お気に入り登録/解除を行う
     *  
     */
    public void setResetFavoriteThread(int index, boolean mode)
    {
        // 登録先および取得ファイル名を生成する
        int boardIndex = parent.getCurrentBbsIndex();
        String url      = bbsCommunicator.getBoardURL(boardIndex);

        int pos = url.lastIndexOf('/', url.length() - 2);
        String nick = url.substring(pos + 1);

        if (currentFavoriteLevel < 0)
        {
            // お気に入り登録/登録解除を実施する
            subjectParser.setResetFavorite(index, url, nick, mode);
        }
        else
        {
            // お気に入りから削除する...
            String threadNick   = favorManager.getBoardNick(index);
            long  threadNumber = favorManager.getThreadNumber(index);
            
            // 今現在、展開中の板一覧スレだった場合...
            if (threadNick.equals(nick) == true)
            {
                // スレの内部情報からお気に入り登録解除を実施する
                subjectParser.removeFavorite(nick, threadNumber);
            }

            // お気に入り一覧から削除する
            favorManager.removeFavorite(threadNick, favorManager.getThreadNumber(index));

            // 表示リストを更新して再表示
            updateTitleList(FORCE_REDRAW);
            repaint();
        }
        return;
    }

    /**
     *  板選択モード
     *  
     */
    public void boardSelectionMode()
    {
        // 板の選択...
        selectBoard();
        return;
    }
    
    /**
     *  板選択処理...
     *  
     */
    private void selectBoard()
    {
        // 板選択の実施
        setBusyMessage("板情報解析中...", true);
        Thread thread = new Thread()
        {
            public void run()
            {
                // タイトルに表示...
                setTitle("板情報解析中...");

                // 板を切り替える
                subjectParser.leaveSubjectList(parent.getCurrentBbsIndex());

                // お気に入り情報を（とりあえず）バックアップする
                if (parent.getIgnoreFavoriteBackup() != true)
                {
                    favorManager.backup();
                }

                // 板一覧を更新する
                bbsCommunicator.refreshBoardInformationCache(parent.getCurrentCategoryIndex(), false, null);

                int   index    = parent.getCurrentCategoryIndex();

                // メッセージを表示する
                String msg =  "【" + bbsCommunicator.getCategory(parent.getCurrentCategoryIndex()) + "】" + " 板を選択してください。";
            
                // 今は板選択モード...
                currentScene = parent.SHOW_BBSLIST;

                // 板数を取得する
                int endIndex   = bbsCommunicator.getCategoryIndex((index + 1));
                int startIndex = bbsCommunicator.getCategoryIndex(index);
                int count      = endIndex - startIndex;
                
                // 選択肢を構築する
                Vector boardList = new Vector(count);
                for (int loop = 0; loop < count; loop++)
                {
                    String boardName    = bbsCommunicator.getBoardName(startIndex + loop);
                    boardList.addElement(boardName);
                }

                // 板一覧を表示する
                index = parent.getCurrentBbsIndex();
                if ((index >= startIndex)&&(index <= endIndex))
                {
                    index = index - startIndex;
                }
                else
                {
                    index = 0;
                }
                parent.openSelectForm(msg, boardList, index, currentScene);
                boardList = null;
                return;
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }

    /**
     * 選択したスレを削除する...
     * 
     */
    private void deleteMessage()
    {
        // アイテムが選択された
        int index = getSelectedIndex();
        if (index < 0)
        {
            return;
        }
        if (currentScene != SHOW_SUBJECTLIST)
        {
            return;
        }
        String doDelete  = "削除実行";
        String title     = "";
        if (currentFavoriteLevel >= 0)
        {
            // お気に入り一覧だった場合...
            doDelete = "datファイル強制削除";
            String fileName = favorManager.getThreadNumber(index) + ".dat";
            gettingLogDataFile = favorManager.getBoardNick(index) + fileName;

            // タイトルの表示...
            title = "強制削除： " + favorManager.getSubjectName(index);
        }
        else
        {
            // 通常の板一覧だった場合...
            int status = subjectParser.getStatus(index);
            if (status == a2BsubjectDataParser.STATUS_NOTYET)
            {
                // スレ未取得なら、何もしない
                return;
            }

            // ファイル名を特定する
            int boardIndex  = parent.getCurrentBbsIndex();
            String url      = bbsCommunicator.getBoardURL(boardIndex);
            String fileName = subjectParser.getThreadFileName(index);
            int pos = url.lastIndexOf('/', url.length() - 2);
            gettingLogDataFile = url.substring(pos + 1) + fileName;
            
            // タイトルの表示...
            title = "スレ削除： " + subjectParser.getSubjectName(index);
        }

        // 今はスレ削除確認モード
        currentScene = parent.CONFIRM_DELETELOGDAT;

        Vector commandList = new Vector(2);
        String cancelCmd = "キャンセル";
        commandList.addElement(cancelCmd);
        commandList.addElement(doDelete);

        parent.openSelectForm(title, commandList, 0, currentScene);
        commandList = null;
        
        currentSubjIndex = index;

        // ガベコレ実施...
        System.gc();
        return;
    }

    /**
     *  メニュー（その２）
     *  
     */
    private void showMenuSecond()
    {
        // タイトルの表示...
        String title = "モード選択";

        Vector commandList = new Vector(10);
        commandList.addElement("ログ参照モード");
        commandList.addElement("数字キー移動モード");
        commandList.addElement("ヘルプ表示モード");
        commandList.addElement("未読スレ抽出");
        commandList.addElement("フォントサイズ変更");
        commandList.addElement("2chログイン(●/BE@2ch)");
        if (currentFavoriteLevel >= 0)
        {
            // お気に入り一覧のとき、機能を追加
            commandList.addElement("所属の板一覧へ");
            commandList.addElement("お気に入り解除");
            commandList.addElement("お気に番号変更");
            commandList.addElement("スレURL更新");
        }
        else
        {
            // 通常の板一覧のとき、機能を追加
            commandList.addElement("プレビュー");
            commandList.addElement("お気に入り登録");
            commandList.addElement("スレ立て");
            commandList.addElement("dat探し");
        }
        currentScene = parent.SHOW_SUBJECTCOMMANDSECOND;
        parent.openSelectForm(title, commandList, 0, currentScene);
        commandList = null;

        // ガベコレ実施...
        System.gc();
        return;
    }

    /**
     *  メニューの実行
     * 
     * 
     * @param scene
     * @param index
     */
    public boolean executeMenuSecond(byte scene, int index)
    {
        if (index == 0)
        {
            // ログ参照モード
            Thread thread = new Thread()
            {
                public void run()
                {
                    // 親に対し、画面を抜けたことを通知する
                    parent.EndListFormLogViewMode();
                }
            };
            thread.start();
            return (true);
        }
        if (index == 1)
        {
            // 数字キー移動モード切替
            parent.toggleNumberInputMode(isNumberMovingMode);
            repaint();
        }
        if (index == 2)
        {
            // ヘルプ表示モード切替
            changeHelpShowMode();
        }
        if (index == 3)
        {
            // スレ検索(未読スレ抽出)
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_REMAIN, OPTION_RESETMODE, -1);
        }
        if (index == 4)
        {
            // フォントサイズ変更
            changeFont(false);
        }
        if (index == 5)
        {
            // ● / BE ログインの実行
            // ログインメニューを表示する
            showLoginMenu(true);
//            isSecondMenuScene = parent.SHOW_LOGINMODEMENU;
//            showLoginMenu(true);
            return (false);
        }

        // スレ番号の抽出
        int threadIndex = getSelectedIndex();
        if (threadIndex >= 0)
        {
            currentSubjIndex = threadIndex;
            if (index == 6)
            {
                if (currentFavoriteLevel >= 0)
                {
                    // 所属の板一覧へ切り替える （この番号(4)は、「スレ操作(お気に入り用)」の番号に依存）
                    executeSubjectOperationFavorite(4);
                }
                else
                {
                    // プレビュー（この番号(1)は、「スレ操作(通常板用)」の番号に依存）
                    executeSubjectOperation(1);
                }
            }
            if (index == 7)
            {
                if (currentFavoriteLevel >= 0)
                {
                    // お気に入り登録解除（この番号(3)は、「スレ操作(お気に入り用)」の番号に依存）
                    executeSubjectOperationFavorite(3);
                }
                else
                {
                    // お気に入り登録（この番号(5)は、「スレ操作(通常板用)」の番号に依存）
                    executeSubjectOperation(5);                
                }
            }
        }
        if (index == 8)
        {
            if (currentFavoriteLevel >= 0)
            {
                // お気に入り番号変更
                informationString   = "お気に番号？ (0,1-9)";
                showInformationMode = 1;
                repaint();
            }
            else
            {
                // スレ立て
                int boardIndex = parent.getCurrentBbsIndex();
                if (boardIndex >= 0)
                {
/***/
                    serviceRepaints();
                    parent.OpenWriteForm(true, "スレ立て : " + bbsCommunicator.getBoardName(boardIndex), -1, -1);
/***/
                }
            }
        }
        if (index == 9)
        {
            if (currentFavoriteLevel >= 0)
            {
                // (お気に入り) ： お気に入りスレのURLを調整する
                Thread thread = new Thread()
                {
                    public void run()
                    {
                        // スレ一覧の取得を実施する
                        boolean ret = favorManager.updateURLData(bbsCommunicator);
                        if (ret == false)
                        {
                            informationString = "※ スレURL更新失敗 ※";
                        }
                        else
                        {
                            informationString = " スレURLの更新完了";
                        }
                        showInformationMode = 0;
                        repaint();
                        parent.vibrate();
                    }
                };
                thread.start();
                showInformationMode = 0;
                informationString = "スレURLを更新中...";
                repaint();
/**
                try
                {
                    // 同期待ち...
                    thread.join();
                }
                catch (Exception e)
                {
                    // 何もしない...
                }
**/
            }
            else
            {
                // 通常板：スレの迷子datファイル探し
                Thread thread = new Thread()
                {
                    public void run()
                    {
                        // スレ一覧の取得を実施する
                        int nofFile = subjectParser.collectDatFile();
                        if (nofFile < 0)
                        {
                            informationString = "※ ファイル検索失敗 ※";
                        }
                        else
                        {
                            informationString = nofFile + " 件のdatファイル発見";
                        }
                        showInformationMode = 0;
                        repaint();
                        parent.vibrate();
                    }
                };
                thread.start();
                showInformationMode = 0;
                informationString = "スレファイル検索中...";
                repaint();
            }
        }
        System.gc();
        return (true);
    }

    /**
     *  ログインメニューの実行
     * 
     * 
     * @param scene
     * @param index
     */
    public void executeLoginMenu(byte scene, int index)
    {
        if (index == 0)
        {
            // ●ログインの実行
            parent.startOysterLogin();            
        }
        else if (index == 1)
        {
            // Be@2chログインの実行
            parent.startBe2chLogin();
        }
        System.gc();
        return;
    }

    /**
     * スレ一覧のアクション選択...
     * 
     */
    private void showActionChoiceGroup(int subjIndex, boolean autoOpen)
    {
        int  status     = subjectParser.getStatus(subjIndex);
        int boardIndex  = parent.getCurrentBbsIndex();
        String fileName = subjectParser.getThreadFileName(subjIndex);
        String url      = bbsCommunicator.getBoardURL(boardIndex);

        // 取得先および取得ファイル名を生成する
        int pos = url.lastIndexOf('/', url.length() - 2);
        gettingLogDataFile = url.substring(pos + 1) + fileName;

        if ((autoOpen == true)&&(status != a2BsubjectDataParser.STATUS_NOTYET)&&(status != a2BsubjectDataParser.STATUS_RESERVE))
        {
            // スレ参照... (親に対し、画面を抜けてスレを表示することを通知する)
            currentSubjIndex = subjIndex;
            parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, subjectParser.getCurrentMessage(currentSubjIndex));
            gettingLogDataFile = null;
            return;
        }    

        if (parent.isGetNewArrival() == true)
        {
            // 新着確認中はメニュー表示を禁止する
            return;
        }

        // タイトルの表示...
        String title = subjectParser.getSubjectName(subjIndex);

        Vector commandList = new Vector(9);
        commandList.addElement("再取得");
        commandList.addElement("プレビュー");
        commandList.addElement("取得予約");
        commandList.addElement("取得");
        commandList.addElement("表示");
        commandList.addElement("お気に入り登録");
        commandList.addElement("お気に入り解除");
        commandList.addElement("ブラウザで開く");
        commandList.addElement("関連キーワード");

        currentScene = parent.SHOW_SUBJECTCOMMAND;
        parent.openSelectForm(title, commandList, 4, currentScene);
        commandList = null;
        
        currentSubjIndex = subjIndex;

        // ガベコレ実施...
        System.gc();
        return;
    }
    
    /**
     * スレ一覧のアクション選択...
     * 
     */
    private void showActionChoiceGroupFavorite(int index, boolean autoOpen)
    {
        byte  status     = favorManager.getStatus(index);
        String fileName = favorManager.getThreadNumber(index) + ".dat";
        gettingLogDataFile = favorManager.getBoardNick(index) + fileName;
        gettingLogDataURL  = favorManager.getUrl(index) + "dat/" + fileName;

        currentScene = SHOW_SUBJECTLIST;
        currentSubjIndex = index;

        if ((autoOpen == true)&&(status != favorManager.STATUS_NOTYET)&&(status != favorManager.STATUS_RESERVE))
        {
            // スレ参照... (親に対し、画面を抜けてスレを表示することを通知する)
            parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, favorManager.getCurrentMessage(currentSubjIndex));
            gettingLogDataFile = null;
            return;
        }    

        if (parent.isGetNewArrival() == true)
        {
            // 新着確認中はメニュー表示を禁止する
            return;
        }

        // タイトルの表示...
        String title = favorManager.getSubjectName(index);

        Vector commandList = new Vector(7);
        commandList.addElement("再取得");
        commandList.addElement("取得");
        commandList.addElement("表示");
        commandList.addElement("お気に入り解除");
        commandList.addElement("所属の板一覧へ");
        commandList.addElement("ブラウザで開く");
        commandList.addElement("関連キーワード");

        currentScene = parent.SHOW_SUBJECTCOMMANDFAVORITE;
        parent.openSelectForm(title, commandList, 2, currentScene);
        commandList = null;

        // ガベコレ実施...
        System.gc();
        return;
    }
    
    /**
     *   現在選択中スレのタイトルを取得する
     * 
     */
    public String getSubjectName()
    {
        String title = "";
        if (currentFavoriteLevel < 0)
        {
            title = subjectParser.getSubjectName(currentSubjIndex);
        }
        else
        {
            // お気に入り一覧表示モード
            title = favorManager.getSubjectName(currentSubjIndex);
        }
        return (title);
    }

    /**
     *  メニュー（その２）
     *  
     */
    private void showLoginMenu(boolean isMenu)
    {
        // タイトルの表示...
        String title = "2chログイン";

        Vector commandList = new Vector(2);
        commandList.addElement("●(Oyster)");
        commandList.addElement("BE@2ch");
        currentScene = parent.SHOW_LOGINMENU;            

        parent.openSelectForm(title, commandList, 0, currentScene);
        commandList = null;

        // ガベコレ実施...
        System.gc();
        return;
    }
        
    /**
     *  スレ情報の表示...
     *  
     */
    private void showMessageInformation()
    {
        int   subjIndex = getSelectedIndex();
        String title = "";
        String addInfo = "";
        byte  status = 0;

        long  threadNumber  = 0;
        int   messageNumber = 0;
        int   level         = -1;

        // 表示モードを確認表示データを切り替える
        if (currentFavoriteLevel < 0)
        {
            threadNumber = subjectParser.getThreadNumber(subjIndex);
            messageNumber = subjectParser.getNumberOfMessages(subjIndex);

            // スレタイトル
            title = subjectParser.getSubjectName(subjIndex);

            // 取得先URLを生成し、スレタイトルと連結する
            int boardIndex = parent.getCurrentBbsIndex();
            String boardUrl = bbsCommunicator.getBoardURL(boardIndex);
            int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
            String nick = boardUrl.substring(pos + 1);
            title = title + "\r\n" + boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + threadNumber + "/";

            // お気に入りに登録されているかチェックする
            if (subjectParser.isFavorite(subjIndex) == true)
            {
                addInfo ="<<< お気に入り >>>\r\n";
            }
            status = subjectParser.getStatus(subjIndex);
        }
        else
        {
            threadNumber  = favorManager.getThreadNumber(subjIndex);
            messageNumber = favorManager.getNumberOfMessages(subjIndex);

            // スレタイトル、URLの情報を生成する
            title = favorManager.getSubjectName(subjIndex);

            String boardUrl = favorManager.getUrl(subjIndex);
            int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
            String nick = boardUrl.substring(pos + 1);
            title = title + "\r\n" + boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + threadNumber + "/";
            level = favorManager.getThreadLevel(subjIndex);
            addInfo = "<<< お気に入り >>>\r\n";
            status = favorManager.getStatus(subjIndex);
        }

        // スレ状態を表示
        if (status == a2BsubjectDataParser.STATUS_NOTYET)
        {
            addInfo = addInfo + "(未取得)";
        }
        else if (status == a2BsubjectDataParser.STATUS_NEW)
        {
            addInfo = addInfo + "(新規取得)";            
        }
        else if (status == a2BsubjectDataParser.STATUS_UPDATE)
        {
            addInfo = addInfo + "(更新あり)";
        }
        else if (status == a2BsubjectDataParser.STATUS_REMAIN)
        {
            addInfo = addInfo + "(未読あり)";                
        }
        else if (status == a2BsubjectDataParser.STATUS_ALREADY)
        {
            addInfo = addInfo + "(既読)";            
        }
        else if (status == a2BsubjectDataParser.STATUS_RESERVE)
        {
            addInfo = addInfo + "(取得予約)";
        }
        else
        {
            addInfo = addInfo + "(???)";            
        }

        // スレの勢い(?)を表示する
        long dateNum = getProgressDate(threadNumber);
        if ((messageNumber > 0)&&(dateNum >= 0))
        {
            addInfo = addInfo + " [" + (dateNum / 24) + "日経過]";
        }
        
        // ダイアログでスレ情報を表示する
        title = title + parent.getHeapInfo();
//        parent.showDialog(parent.SHOW_INFO, 0, "スレ情報", title);
        parent.OpenWriteFormUsingThreadOperationMode("スレ情報", title, addInfo, level);
        return;
    }

    /**
     * 現在から経過日数を取得する
     * 
     * @param sinceDate
     * @return
     */
    private long getProgressDate(long sinceDate)
    {
        Date dateTime = new Date();
        long diffTime = (dateTime.getTime() / 1000) - sinceDate;
        long num = 60 * 60;
        num = diffTime / num;
        return (num);
    }

    /**
     * お気に入りスレレベルを更新する
     * @param level
     */
    public void updateFavoriteThreadLevel(int level)
    {
        if ((currentFavoriteLevel >= 0)&&(level >= 0))
        {
            int index = getSelectedIndex();
            if (index >= 0)
            {
                favorManager.setThreadLevel(index, level);
                if (parent.getIgnoreFavoriteBackup() != true)
                {
                	favorManager.doBackup();
                }
            }
        }
        return;
    }
    
    /**
     *   ヘルプイメージを表示する
     * 
     *
     */
    private void changeHelpShowMode()
    {
        if (helpImage == null)
        {
            return;
        }
        if (isHelpShowMode == true)
        {
            isHelpShowMode = false;
        }
        else
        {
            isHelpShowMode = true;
        }
        return;
    }
    
    /**
     * タイトル検索用情報を設定する
     * 
     * @param searchString
     * @param searchTitleStatus
     * @param option
     */
    public void setSearchTitleInformation(String searchString, byte searchTitleStatus, int option, int searchHour)
    {
        // スレタイ検出時間...
        searchTitlePickupHour = searchHour;
        
        // 既読スレ一覧モードに切り替える
        if ((option == OPTION_ONLYDATABASE)&&(subjectParser.getOnlyDatabaseAccess() == false))
        {
            subjectParser.setOnlyDatabaseAccess(true);
            searchTitleStatus = SEARCHING_ONLYDB;
        }
        else
        {
            subjectParser.setOnlyDatabaseAccess(false);
        }
        
        // 大文字/小文字の区別について反映させる
        if (option <= OPTION_NOTIGNORE_CASE)
        {
            searchIgnoreCase = false;
        }
        else
        {
            searchIgnoreCase = true;            
        }

        if ((searchString == null)||(searchString.compareTo("") == 0))
        {
            // タイトル検索を解除する
            searchTitleString = null;
        }
        else
        {
            // タイトル検索を設定する
            searchTitleString = searchString;
            searchTitleStatus = SEARCHING_MODE;
        }
        searchTitleMessageStatus = searchTitleStatus;

        // スレタイトル一覧を再描画する
        readySubjectList();
        return;
    }
    
    /**
     *  タイトル検索文字列をクリアする
     *
     */
    public void clearSearchTitleString()
    {
        // タイトル検索だけを解除する
        searchTitleString = null;
        return;
    }

    /** 
     *  ログデータの(直接)取得
     * 
     *
     */
    private void getLogDataFileImmediate()
    {
        currentSubjIndex = getSelectedIndex();
        if (currentSubjIndex < 0)
        {
            // インデックス番号がおかしい、、、終了する
            return;
        }

        // 選択されたサブジェクトインデックスを記憶する
        parent.setSelectedSubjectIndex(currentSubjIndex);

        if (currentScene != SHOW_SUBJECTLIST)
        {
            // スレ一覧表示をしていなかった、、終了する
            return;
        }

        // 表示モードを確認、状況に合わせてスレを取得する
        isOpenImmediate = false;
        if (currentFavoriteLevel < 0)
        {
            // スレ状態を取得する
            int  status  = subjectParser.getStatus(currentSubjIndex);

            if ((status == a2BsubjectDataParser.STATUS_NOTYET)||(status == a2BsubjectDataParser.STATUS_RESERVE))
            {
                // スレ未取得(スレ全取得を実施)
                getLogDataFile(currentSubjIndex, GETMODE_GETALL);
                return;
            }

            // 差分取得...
            getLogDataFile(currentSubjIndex, GETMODE_GETPART);
        }
        else
        {
            // お気に入りモードのとき、、、
            byte  status  = favorManager.getStatus(currentSubjIndex);
            if ((status == favorManager.STATUS_NOTYET)||(status == favorManager.STATUS_RESERVE))
            {
                // スレ未取得(スレ全取得を実施)
                getLogDataFileFavorite(currentSubjIndex, GETMODE_GETALL);
                return;
            }

            // 差分取得...
            getLogDataFileFavorite(currentSubjIndex, GETMODE_GETPART);
        }
        return;        
    }

    
    /**
     *  関連キーワード表示...
     * 
     *  @param  threadUrl  スレのURL (read.cgi経由のもの)
     */
    private void showRelatedKeyword(String theadUrl)
    {
        gettingLogDataURL = "http://p2.2ch.io/getf.cgi?" + theadUrl;
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.GetHttpCommunication(null, gettingLogDataURL, null, "関連キーワード", 0, 0, parent.SCENE_GET_RELATE_KEYWORD, false);
                parent.unlockCommunicationScreen();   // BUSYスクリーンのロックをはずす
            }
        };
        // BUSYスクリーンのロックをつける
        parent.lockCommunicationScreen();
        parent.showBusyMessage("", "関連キーワード抽出中...", -1);
        thread.start();
    }

    /**
     *  受信した関連キーワードの解析を行う
     *
     */
    public void parseRelateKeyword(byte[] dataString)
    {
        pickup2chList = null;
        pickup2chList = new Vector();

        int index = 0;
        while ((index >= 0)&&(index < dataString.length))
        {
            index = pickupRelatedKeyword(dataString, index);
        }

        if (pickup2chList.isEmpty() != true)
        {
            currentScene = parent.CONFIRM_SELECT_RELATE_KEYWORD;
            parent.openSelectForm("関連キーワード", pickup2chList, 0, currentScene);
        }
        else
        {
            // 関連キーワードの検出に失敗した場合...
            // 1件もひっかからなかった場合...
            showInformationMode = 0;
            informationString = "関連キーワード:見つかりません";
            repaint();
        }

        // ガベコレ実施...
        System.gc();
        return;
    }

    /**
     * 関連キーワード検索で拾った、2ch検索用URLをとってくる
     * 
     * @param buffer
     * @param startIndex
     * @return
     */
    private int pickupRelatedKeyword(byte[] buffer, int startIndex)
    {
        boolean       pickupStr  = false;
        int threadNumIndexStart   = startIndex;
        int threadNumIndexEnd     = startIndex;
        int threadTitleIndexStart = startIndex;
        int threadTitleIndexEnd   = startIndex;
        int endIndex = buffer.length;
        try
        {
            for (int index = startIndex; index < buffer.length; index++)
            {
/**
                if ((buffer[index + 0] == 'h')&&(buffer[index + 1] == 't')&&
                    (buffer[index + 2] == 't')&&(buffer[index + 3] == 'p')&&
                    (buffer[index + 4] == ':')&&(buffer[index + 5] == '/')&&
                    (buffer[index + 6] == '/')&&(buffer[index + 7] == 'f')&&
                    (buffer[index + 8] == 'i')&&(buffer[index + 9] == 'n')&&
                    (buffer[index + 10] == 'd')&&(buffer[index + 11] == '.'))
**/
                if ((buffer[index + 0] == 'S')&&(buffer[index + 1] == 'T')&&
                    (buffer[index + 2] == 'R')&&(buffer[index + 3] == '='))
                {
                    // "http://find." を見つける (２ちゃんねる検索用のURL)
                    threadNumIndexStart = index;
                    threadNumIndexEnd   = buffer.length;
                    for (int numberIndex =threadNumIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '"')
                         {
                            index = numberIndex - 1;
                            threadNumIndexEnd = numberIndex;
                            break;
                         }
                    }
                    pickupStr = true;
                }
/**
                else if ((buffer[index + 0] == '_')&&(buffer[index + 1] == 'b')&&
                          (buffer[index + 2] == 'l')&&(buffer[index + 3] == 'a')&&
                          (buffer[index + 4] == 'n')&&(buffer[index + 5] == 'k')&&
                          (buffer[index + 6] == '"')&&(buffer[index + 7] == '>'))
**/
                else if ((pickupStr == true)&&(buffer[index + 0] == '"')&&(buffer[index + 1] == '>'))
                 {
                    // スレタイトル。。。 （フォーマットが変わると手直しが必要だな...）
                    threadTitleIndexStart = index + 2;
                    for (int titleIndex =threadTitleIndexStart; titleIndex < buffer.length; titleIndex++)
                    {
                        if ((buffer[titleIndex + 0] == '<')&&(buffer[titleIndex + 1] == '/')&&
                            (buffer[titleIndex + 2] == 'a')&&(buffer[titleIndex + 3] == '>'))
                         {
                            threadTitleIndexEnd = titleIndex;
                            break;
                         }
                    }

                    // ここでタイトルとURLをエントリする
                    if ((threadNumIndexStart < threadNumIndexEnd)&&
                        (threadTitleIndexStart < threadTitleIndexEnd)&&
                        (threadNumIndexStart < threadTitleIndexStart))
                    {
                        String title  = parent.sjToUtf8(buffer, threadTitleIndexStart, (threadTitleIndexEnd - threadTitleIndexStart));
                        String number = new String(buffer, threadNumIndexStart, (threadNumIndexEnd - threadNumIndexStart));
                        title = title + "  <>" + number;
                        pickup2chList.addElement(title);
                    }
                    endIndex = threadTitleIndexEnd;
                    break;
                }
            }
        }
        catch (Exception e)
        {
            endIndex = -1;
        }
        return (endIndex);        
    }
    
    /**
     *   ログデータの取得
     * 
     */
    public void getLogDataFile(int subjIndex, int getMode)
    {
        // 取得先および取得ファイル名を生成する
        int boardIndex = parent.getCurrentBbsIndex();
        String url      = bbsCommunicator.getBoardURL(boardIndex);
        String fileName = subjectParser.getThreadFileName(subjIndex);
        int pos = url.lastIndexOf('/', url.length() - 2);
        gettingLogDataFile = url.substring(pos + 1) + fileName;
        gettingLogDataURL  = url + "dat/" + fileName;

        // スレタイの取得
        gettingLogDataTitle = "'" + subjectParser.getSubjectName(subjIndex) + "' ";
        
        currentScene = SHOW_SUBJECTLIST;
        currentSubjIndex = subjIndex;

        if (getMode == GETMODE_GETPART)
        {
            // 差分取得を実施する。。。
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, -1, -1, parent.SCENE_PARTGET_MESSAGE, true);
                }
            };
            thread.start();            
        }
        else if (getMode == GETMODE_GETALL)
        {
            // 全取得を実施する。。。
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, 0, 0, parent.SCENE_WHOLEGET_MESSAGE, false);
                }
            };
            thread.start();
        }
        else
        {
            // プレビューモード
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(null, gettingLogDataURL, null, gettingLogDataTitle, 0, PREVIEW_LENGTH, parent.SCENE_PREVIEW_MESSAGE, false);
                }
            };
            thread.start();
        }
    }

    /**
     *   ログデータの取得 (お気に入り)
     * 
     */
    public void getLogDataFileFavorite(int index, int getMode)
    {
        String fileName = favorManager.getThreadNumber(index) + ".dat";
        gettingLogDataFile = favorManager.getBoardNick(index) + fileName;
        gettingLogDataURL  = favorManager.getUrl(index) + "dat/" + fileName;

        // スレタイの取得
        gettingLogDataTitle = "'" + favorManager.getSubjectName(index) + "' ";
        
        currentScene = SHOW_SUBJECTLIST;
        currentSubjIndex = index;

        if (getMode == GETMODE_GETPART)
        {
            // 差分取得を実施する。。。
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, -1, -1, parent.SCENE_PARTGET_MESSAGE, true);
                }
            };
            thread.start();            
        }
        else if (getMode == GETMODE_GETALL)
        {
            // 全取得を実施する。。。
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, 0, 0, parent.SCENE_WHOLEGET_MESSAGE, false);
                }
            };
            thread.start();
        }
        else
        {
            // プレビューモード
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(null, gettingLogDataURL, null, gettingLogDataTitle, 0, PREVIEW_LENGTH, parent.SCENE_PREVIEW_MESSAGE, false);
                }
            };
            thread.start();
        }
    }

    /**
     * スレ取得完了した場合...
     * @param result
     */
    public void completedGetLogDataFile(boolean result, int length)
    {
        currentScene = SHOW_SUBJECTLIST;

        if (currentFavoriteLevel < 0)
        {
            if (result == true)
            {
                // 通常表示モード
                subjectParser.setStatus(currentSubjIndex, a2BsubjectDataParser.STATUS_NEW);
                subjectParser.setCurrentMessage(currentSubjIndex, 1, -1);
            }
            else
            {
                if (length > 0)
                {
                    subjectParser.setStatus(currentSubjIndex, a2BsubjectDataParser.STATUS_UPDATE);
                }
            }
            if (isOpenImmediate == true)
            {
                // 親に対し、画面を抜けてスレを表示することを通知する
                parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, subjectParser.getCurrentMessage(currentSubjIndex));
            }
        }
        else
        {
            if (result == true)
            {
                // お気に入り表示モードのステータスを更新する
                favorManager.setStatus(currentSubjIndex, favorManager.STATUS_NEW, 1, -1);
            }
            else
            {
                if (length > 0)
                {
                    favorManager.setStatus(currentSubjIndex, favorManager.STATUS_UPDATE, -1, -1);
                }
            }
            if (isOpenImmediate == true)
            {
                // 親に対し、画面を抜けてスレを表示することを通知する
                parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, favorManager.getCurrentMessage(currentSubjIndex));
            }
        }
        isOpenImmediate    = true;
        gettingLogDataFile = null;
        return;
    }

    /**
     *  ２ちゃんねる検索を実行する (最大20件)
     */
    private void find2chThreadTitles()
    {       
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.GetHttpCommunication("temporary.html", gettingLogDataURL, null, "2ch検索", 0, 0, parent.SCENE_GET_FIND_2CH, false);
                parent.unlockCommunicationScreen();   // BUSYスクリーンのロックをはずす
            }
        };
        // BUSYスクリーンのロックをつける
        try
        {
            parent.lockCommunicationScreen();
            parent.showBusyMessage("", "2ch検索中...", -1);
            thread.start();
        }
        catch (Exception ex)
        {
        	// 
        }
    }

    /**
     *  ２ちゃんねる検索を使って、スレタイトルを検索する (最大20件)
     */
    public void find2chThreadNames(String titleToSearch)
    {
        gettingLogDataURL = "http://find.2ch.net/?BBS=ALL&TYPE=TITLE&STR=" + titleToSearch + "&COUNT=25";
        find2chThreadTitles();
    }

    /**
     * ２ちゃんねる検索の結果を解析する
     * 
     * @param dataString
     */
    public void parseFind2ch(byte[] dataString)
    {
        byte [] dataBuffer = null;
        try
        {
            int firstIndex = 0;
            int endIndex   = dataString.length - 4;
            for (int i = 0; i < endIndex ; i++)
            {
                if ((dataString[i] == '<')&&(dataString[i + 1] == 'd')&&(dataString[i + 2] == 't')&&(dataString[i + 3] == '>'))
                {
                    firstIndex = i;
                    break;
                }
            }
            dataBuffer = new byte[dataString.length - firstIndex + 1];
            System.arraycopy(dataString, firstIndex, dataBuffer, 0, (dataString.length - firstIndex));
        }
        catch (Exception e)
        {
            dataBuffer = dataString;
        }

        pickup2chList = null;
        pickup2chList = new Vector();

        int index = 0;
        while ((index >= 0)&&(index < dataBuffer.length))
        {
            index = pickupFind2chData(dataBuffer, index);
        }

        if (pickup2chList.isEmpty() != true)
        {
            currentScene = parent.CONFIRM_SELECT_FIND2CH;
            parent.openSelectForm("2ch検索", pickup2chList, 0, currentScene);
        }
        else
        {
            // 1件もひっかからなかった場合...
            showInformationMode = 0;
            informationString = "2ch検索:見つかりません";
            repaint();
        }
        System.gc();
        return;
    }

    /**
     * ２ちゃんねる検索で拾った、URLをとってくる
     * 
     * @param buffer
     * @param startIndex
     * @return
     */
    private int pickupFind2chData(byte[] buffer, int startIndex)
    {
        int threadNumIndexStart   = startIndex;
        int threadNumIndexEnd     = startIndex;
//        int threadNickIndexEnd    = startIndex;
        int threadTitleIndexStart = startIndex;
        int threadTitleIndexEnd   = startIndex;
        int boardHostIndexStart   = startIndex;
//        int boardHostIndexEnd     = startIndex;
        int endIndex = buffer.length;

        try
        {
            boolean anchor = false;
            for (int index = startIndex; index < buffer.length; index++)
            {
                if ((buffer[index + 0] == ':')&&(buffer[index + 1] == '/')&&(buffer[index + 2] == '/'))
                {
                    boardHostIndexStart = index + 3;
                    for (int numberIndex = boardHostIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '/')
                        {
                            // ホスト名の末尾
//                            boardHostIndexEnd = numberIndex;
                            index = numberIndex;
                            break;
                        }
                    }
                }
                else if ((buffer[index + 0] == 'r')&&(buffer[index + 1] == 'e')&&
                    (buffer[index + 2] == 'a')&&(buffer[index + 3] == 'd')&&
                    (buffer[index + 4] == '.')&&(buffer[index + 5] == 'c')&&
                    (buffer[index + 6] == 'g')&&(buffer[index + 7] == 'i')&&
                    (buffer[index + 8] == '/'))
                {
                    // "read.cgi/" を見つける
                    threadNumIndexStart = index + 9;
                    boolean firstSlash = false;  // あー、ヤダヤダ、フラグなんて、、、。
                    for (int numberIndex =threadNumIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '/')
                         {
                            if (firstSlash == true)
                            {
                                threadNumIndexEnd = numberIndex;
                                index = numberIndex;
                                anchor = true;
                                break;
                            }
                            else
                            {
                                firstSlash = true;
  //                              threadNickIndexEnd = numberIndex;
                            }
                         }
                    }
               
                }

                //                else if ((buffer[index + 0] == '<')&&(buffer[index + 1] == 'b')&&(buffer[index + 2] == '>'))
                else if (anchor == true)
                {
                    while ((buffer[index] != '>')&&(index < buffer.length))
                    {
                        index++;
                    }
                    index++;
                    threadTitleIndexStart = index;
                    for (int titleIndex =threadTitleIndexStart; titleIndex < buffer.length; titleIndex++)
                    {
                        if ((buffer[titleIndex + 0] == '<')&&(buffer[titleIndex + 1] == '/')&&
                             (buffer[titleIndex + 2] == 'a')&&(buffer[titleIndex + 3] == '>'))
                        {
                            threadTitleIndexEnd = titleIndex; 
                            break;
                        }
                    }

                    // ここでタイトルをエントリする
                    if ((threadNumIndexStart < threadNumIndexEnd)&&
                        (threadTitleIndexStart < threadTitleIndexEnd)&&
                        (threadNumIndexStart < threadTitleIndexStart))
                    {
                        String title  = parent.eucToUtf8(buffer, threadTitleIndexStart, (threadTitleIndexEnd - threadTitleIndexStart));
                        String number = new String(buffer, threadNumIndexStart, (threadNumIndexEnd - threadNumIndexStart));
                        title = title + "<>" + number;
                        pickup2chList.addElement(title);
                    }
                    endIndex = threadTitleIndexEnd;
                    break;
                }
            }
            while ((buffer[endIndex] != 0x0a)&&(endIndex < buffer.length))
            {
                endIndex++;
            }
        }
        catch (Exception e)
        {
            endIndex = -1;
        }
        return (endIndex);        
    }
    /**
     * ２ちゃんねる検索で拾った、URLをとってくる
     * 
     * @param buffer
     * @param startIndex
     * @return
     */
/*
    private int pickupFind2chData(byte[] buffer, int startIndex)
    {
        int threadNumIndexStart   = startIndex;
        int threadNumIndexEnd     = startIndex;
        int threadTitleIndexStart = startIndex;
        int threadTitleIndexEnd   = startIndex;
        int endIndex = buffer.length;
        try
        {
            for (int index = startIndex; index < buffer.length; index++)
            {
                if ((buffer[index + 0] == 'r')&&(buffer[index + 1] == 'e')&&
                    (buffer[index + 2] == 'a')&&(buffer[index + 3] == 'd')&&
                    (buffer[index + 4] == '.')&&(buffer[index + 5] == 'c')&&
                    (buffer[index + 6] == 'g')&&(buffer[index + 7] == 'i')&&
                    (buffer[index + 8] == '/'))
                {
                    // "read.cgi/" を見つける
                    threadNumIndexStart = index + 9;
                    boolean firstSlash = false;  // あー、ヤダヤダ、フラグなんて、、、。
                    for (int numberIndex =threadNumIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '/')
                         {
                            if (firstSlash == true)
                            {
                                threadNumIndexEnd = numberIndex;
                                index = numberIndex;
                                break;
                            }
                            else
                            {
                                firstSlash = true;
                            }
                         }
                    }
               
                }
                else if ((buffer[index + 0] == '<')&&(buffer[index + 1] == 'b')&&(buffer[index + 2] == '>'))
                 {
                    threadTitleIndexStart = index + 3;
                    for (int titleIndex =threadTitleIndexStart; titleIndex < buffer.length; titleIndex++)
                    {
                        if ((buffer[titleIndex + 0] == '<')&&(buffer[titleIndex + 1] == '/')&&
                            (buffer[titleIndex + 2] == 'b')&&(buffer[titleIndex + 3] == '>'))
                         {
                            if ((buffer[titleIndex + 4] == '<')&&(buffer[titleIndex + 5] == '/')&&
                                (buffer[titleIndex + 6] == 'a')&&(buffer[titleIndex + 7] == '>'))
                            {
                                threadTitleIndexEnd = titleIndex;                                
                            }
                            else
                            {
                                threadTitleIndexStart = buffer.length;
                                threadTitleIndexEnd = buffer.length;
                            }
                            break;
                         }
                    }
                    // ここでタイトルをエントリする
                    if ((threadNumIndexStart < threadNumIndexEnd)&&
                        (threadTitleIndexStart < threadTitleIndexEnd)&&
                        (threadNumIndexStart < threadTitleIndexStart))
                    {
                        String title  = parent.eucToUtf8(buffer, threadTitleIndexStart, (threadTitleIndexEnd - threadTitleIndexStart));
                        String number = new String(buffer, threadNumIndexStart, (threadNumIndexEnd - threadNumIndexStart));
                        title = title + "<>" + number;
                        pickup2chList.addElement(title);
                    }
                    endIndex = threadTitleIndexEnd;
                    break;
                }
            }
        }
        catch (Exception e)
        {
            endIndex = -1;
        }
        return (endIndex);        
    }
*/

    /**
     * スレ一覧の表示準備
     * @param index
     * @param forceUpdate
     */
    public void prepareSubjectList(int index, boolean forceUpdate)
    {
        // 指定された板indexを記録する
        currentBbsIndex = index;

        // お気に入り一覧かどうか
        currentFavoriteLevel = parent.getFavoriteShowLevel();            

        // 強制スレ一覧取得を実施するかどうか
        isSubjectTxtUpdate = forceUpdate;

        // スレ一覧の取得
        Thread thread = new Thread()
        {
            public void run()
            {
                setTitle(" ");
                
                // 板一覧を更新する
                bbsCommunicator.refreshBoardInformationCache(parent.getCurrentCategoryIndex(), false, null);

                // スレ一覧の準備...
                if (isSubjectTxtUpdate == true)
                {
                    subjectParser.prepareSubjectListUpdate(currentBbsIndex);
                }
                else
                {
                    subjectParser.prepareSubjectList(currentBbsIndex);
                }
                
                // 表示可能行を設定する
                setLineWidthHeight(getWidth(), getHeight());
                isSubjectTxtUpdate = false;
            }
        };
        setBusyMessage("subject.txt更新中...", true);
        thread.start();
    }

    /**
     *   新着確認の実行を開始
     * 
     */
    public void getNewArrivalMessage()
    {
        // 新着確認用の準備を実行する
        subjectParser.prepareGetLogList(currentFavoriteLevel);
        return;
    }
    
    /**
     *  新着確認結果を反映させる
     * 
     */
    public void completedGetLogList(int length)
    {
        if (length > 0)
        {
            // データ更新
            subjectParser.setGetLogStatus(newArrialCheckIndex, a2BsubjectDataParser.STATUS_UPDATE);
        }
        else if (length < 0)
        {
            // エラー発生
            subjectParser.setGetLogStatus(newArrialCheckIndex, a2BsubjectDataParser.STATUS_GETERROR);            
        }
        return;
    }

    /**
     * 新着確認を実施する...
     * 
      */
    public void startGetLogList(int mode)
    {
        // WX320Kチェック...
        String platForm = System.getProperty("microedition.platform");

        int actionMode = mode;
        if (actionMode == GETLOGLIST_START_RECURSIVE)
        {
            newArrialCheckIndex = 0;

            if (platForm.indexOf("WX320K") >= 0)
            {
                // WX320Kのときには、スレッド内で次スレッドを起こしたりしない
                actionMode = GETLOGLIST_START_THREAD;
            }
        }
        else // if (actionMode == GETLOGLIST_CONTINUE_RECURSIVE)
        {
            if (platForm.indexOf("WX320K") >= 0)
            {
                // WX320Kのときには、終了する。
                return;
            }
            newArrialCheckIndex++;
        }

        // 新着確認の実施メイン...
        while (newArrialCheckIndex >= 0)
        {
            Thread thread = new Thread()
            {
                public void run()
                {
                    // 次の新着確認を実行する...
                    getLogList(newArrialCheckIndex);
                }
            };
            try
            {
                thread.start();

                if (actionMode == GETLOGLIST_START_THREAD)
                {
                    // WX320Kのときには、150msのdelayを設ける.(I/O Exception対策)
                    thread.join();
                    Thread.sleep(150);
                    thread = null;
                    System.gc();
                }
                else
                {
                    break;
                }
            }
            catch (Exception e)
            {
                //
            }
            newArrialCheckIndex++;
            thread = null;
//          System.gc();
        }
        return;
    }

    /**
     * 新着確認を実施する
     * @param index
     */
    private void getLogList(int index)
    {
        long threadNumber = subjectParser.getThreadNumberGetLogList(index);
        if ((threadNumber < 0)||(isAbortNewArrival == true))
        {
            // 新着確認終了...
            currentScene = SHOW_SUBJECTLIST;
            subjectParser.clearGetLogList();
            
            //parent.showDialog(parent.SHOW_INFO, 0, "情報", "新着確認が終了しました。");
            parent.endGetNewArrivalMessage();
            
            showInformationMode = 0;

            // 終了メッセージの設定
            if (isAbortNewArrival == true)
            {
                informationString = "※ 新着確認中止 ※";
                isAbortNewArrival = false;
            }
            else
            {
                informationString = "新着確認終了";                
            }
            newArrialCheckIndex = -10;
            repaint();
            System.gc();
            return;
        }

        // 取得先および取得ファイル名を生成する
        String url      = null;        
        if (currentFavoriteLevel < 0)
        {
            // 通常取得モード
            int boardIndex = parent.getCurrentBbsIndex();        
            url = bbsCommunicator.getBoardURL(boardIndex);
        }
        else
        {
            // お気に入り表示モード
            url = subjectParser.getFavoriteUrlGetLogList(index);
        }
        if (url == null)
        {
            // 無理やり次の新着確認へ
            parent.CompletedToGetHttpData(parent.SCENE_GETALL_MESSAGE, 300, 0);
            return;
        }
        String fileName = threadNumber + ".dat";
        int pos = url.lastIndexOf('/', url.length() - 2);
        gettingLogDataFile = null;
        gettingLogDataFile = url.substring(pos + 1) + fileName;
        
        gettingLogDataURL  = null;
        gettingLogDataURL  = url + "dat/" + fileName;

        // 取得済みレスの番号を確認する
        int resNumber = subjectParser.getThreadResNumberGetLogList(index);
        if ((resNumber < 0)||(resNumber > 1000))
        {
            // 無理やり次の新着確認へ
            parent.CompletedToGetHttpData(parent.SCENE_GETALL_MESSAGE, 300, 0);
            return;
        }

        // スレタイトルを取得する
        gettingLogDataTitle = "'" + subjectParser.getSubjectNameGetLogList(index) +"' ";

        // http通信（全取得/差分取得)を実施する
        byte status = subjectParser.getThreadStatusGetLogList(index);
        if ((status == a2BsubjectDataParser.STATUS_NOTYET)||(status == a2BsubjectDataParser.STATUS_RESERVE))
        {
            // スレ全取得を実施する
            parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, 0, 0, parent.SCENE_GETRESERVE_MESSAGE, false);
        }
        else
        {
            // スレ差分取得を実施する
            parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, -1, -1, parent.SCENE_GETALL_MESSAGE, true);
        }

        // ガベコレ実施...
        System.gc();
        return;
    }

    /**
     *   スレ一覧の表示準備ができたとき...
     * 
     * 
     */
    public void readySubjectList()
    {
        // カーソルを先頭にもってくる
        currentSelectedLine  = 0;
        previousSelectedLine = 0;
        listCount            = 0;

        nofSubjectIndex     = 0;
        listDrawMode        = FORCE_REDRAW;

        // 表示リストを更新する
        updateTitleList(FORCE_REDRAW);
        
        // 画面を更新する
        setBusyMessage(null, false);

        // ガベコレ実施...
        System.gc();
        return;
    }

    /**
     *   文字色を応答する
     * 
     * 
     */
    public int getForeColor(byte status, int line)
    {
        int  color  = 0x00000000;
        switch (status)
        {
          case 0:  // subjectParser.STATUS_NOTYET:
            color   = 0x00777777;    // グレー
            break;

          case 1:  // subjectParser.STATUS_NEW:
//            color   = 0x0000c8c8;   // 水
            color   = 0x000080ff;   // 青紫
            break;

          case 2:  // subjectParser.STATUS_UPDATE:
            color   = 0x001414c8;   // 青
            break;

          case 3:  // subjectParser.STATUS_REMAIN:
//            color   = 0x0037a637;   // 緑
            color   = 0x00007000;   // 緑
            break;

          case 4:  // subjectParser.STATUS_ALREADY:
            color   = 0x00000000;   // 黒
            break;

          case 5:  // subjectParser.STATUS_RESERVE:
            color   = 0x00804000;   // 茶色
            break;

          case 6:  // subjectParser.STATUS_UNKNOWN:
            color   = 0x00808020; // 草色...
            break;

          case 7:  // subjectParser.STATUS_GETERROR:
            color = 0x00eb0000;   // 赤
            break;

          case 8:  // subjectParser.STATUS_OVER:
            color   = 0x00eb0aeb; // 紫
            break;

          default:
            color   = 0x00828282;   // グレー
            break;
        }
        return (color);
    }

    /**
     *   背景色を応答する
     * 
     * 
     */
    public int getBackColor(byte status, int line)
    {
        if ((line % 2) == 0)
        {
            return (0x00f8f8f8);
        }
        else
        {
            return (0x00d8d8ff);
        }
/*
        if (subjectTitles <= index)
        {
            return (0x00f8f8f8);
        }
        int  color  = 0;
        byte status = getStatus(index);
        switch (status)
        {
          case 0:  // subjectParser.STATUS_NOTYET:
            color = 0x00f8f8f8;
            break;

          case 1:  // subjectParser.STATUS_NEW:
            color = 0x00f8f8f8;
            break;

          case 2:  // subjectParser.STATUS_UPDATE:
            color = 0x00f8f8f8;
            break;

          case 3:  // subjectParser.STATUS_REMAIN:
            color = 0x00f8f8f8;
            break;

          case 4:  // subjectParser.STATUS_ALREADY:
            color = 0x00f8f8f8;
            break;

          case 5:  // subjectParser.STATUS_RESERVE:
            color = 0x00f8f8f8;
            break;

          case 6:  // subjectParser.STATUS_UNKNOWN:
            color = 0x00f8f8f8;
            break;

          case 7:  // subjectParser.STATUS_GETERROR:
            color = 0x00f8f8f8;
            break;

          case 8:  // subjectParser.STATUS_OVER:    
            color = 0x00f8f8f8;
            break;

          default:
            color = 0x00f8f8f8;
            break;
        }
        return (color);
*/
    }

    /**
     *   スレタイトルを更新する
     * 
     * @param title
     */
    public  void updateThreadTitle(byte[] title, int offset, int length)
    {
        if (currentFavoriteLevel < 0)
        {
            // お気に入りじゃあないときだけ更新する
            subjectParser.updateThreadTitle(currentSubjIndex, title, offset, length);
        }
        else
        {
            // お気に入りの場合...更新する
            favorManager.updateThreadTitle(currentSubjIndex, title, offset, length);
        }
        return;
    }
    
    /**
     *   現在表示レス数と最大レス数を設定
     *   
     */
    public void setCurrentMessage(int number, int maxNumber)
    {
        // お気に入り登録スレ、こちらの状態も更新する (けど、バックアップはしない...)
        String url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
        int   pos  = url.lastIndexOf('/', url.length() - 2);

        if (currentFavoriteLevel < 0)
        {
            // 管理スレを表示中に参照画面から抜けてきた場合は、レス番号をDBに記録する
            subjectParser.setCurrentMessage(currentSubjIndex, number, maxNumber);
            if (subjectParser.isFavorite(currentSubjIndex) == true)
            {
                long  threadNumber = subjectParser.getThreadNumber(currentSubjIndex);
                int favorIndex = favorManager.exist(url.substring(pos + 1), threadNumber);
                if (favorIndex >= 0)
                {
                    // 状態を決める。。。
                    byte status = favorManager.STATUS_ALREADY;
                    if (number < maxNumber)
                    {
                        status = favorManager.STATUS_REMAIN;
                    }
                    if (number > 1000)
                    {
                        status = favorManager.STATUS_OVER;
                    }
                    if (maxNumber < 0)
                    {
                        status = favorManager.STATUS_NEW;
                    }

                    // お気に入り表示モードのステータスを更新する
                    favorManager.setStatus(favorIndex, status, number, maxNumber);            
                    
                    // お気に入り情報をバックアップする
                    //favorManager.backup();
                }
            }            
        }
        else
        {
            // 状態を決める。。。
            byte status = favorManager.STATUS_ALREADY;
            if (number < maxNumber)
            {
                status = favorManager.STATUS_REMAIN;
            }
            if (number > 1000)
            {
                status = favorManager.STATUS_OVER;
            }
            if (maxNumber < 0)
            {
                status = favorManager.STATUS_NEW;
            }

            // お気に入り表示モードのステータスを更新する
            favorManager.setStatus(currentSubjIndex, status, number, maxNumber);            

            // お気に入り情報をバックアップする
            if (parent.getIgnoreFavoriteBackup() != true)
            {
                favorManager.backup();
            }
            
            // スレ一覧が構築済みだった場合...
            String nick = favorManager.getBoardNick(currentSubjIndex);
            if (nick.equals(url.substring(pos + 1)) == true)
            {
                // 構築済み板一覧のほうも更新する
                long  threadNumber = favorManager.getThreadNumber(currentSubjIndex);
                subjectParser.setCurrentMessage(threadNumber, number, maxNumber);
            }
        }
        return;
    }
    
    /**
     *   現在参照中のスレインデックスを応答する
     * 
     */
    public int getCurrentMessageIndex()
    {
        return (currentSubjIndex);
    }

    /**
     *   現在参照中のスレインデックスを記録する
     * 
     */
    public void setCurrentMessageIndex(int index)
    {
        currentSubjIndex = index;
    }
    
    /**
     * ナンバー移動モード
     * 
     * @param mode
     */
    public void setNumberInputMode(boolean mode)
    {
        isNumberMovingMode = mode;
        return;
    }
    
    /**
     *   現在参照中レスをリロードする
     * 
     */
    public void reloadCurrentMessage()
    {
        if (currentFavoriteLevel < 0)
        {
            // 通常板一覧表示モード
            getLogDataFile(currentSubjIndex, GETMODE_GETPART);
        }
        else
        {
            // お気に入り一覧表示モード
            getLogDataFileFavorite(currentSubjIndex, GETMODE_GETPART);
        }
        return;
    }
    
    /**
     *  現在参照中レスの板URLを返す
     * 
     */
    public String getCurrentMessageBoardUrl()
    {
        String url;
        if (currentFavoriteLevel < 0)
        {
            // 板URLを取得する (通常板)
            url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
        }
        else
        {
            // 板URLを取得する (お気に入り)
            url  = favorManager.getUrl(currentSubjIndex);
        }
        return (url);
    }

    /**
     *  ヘルプイメージを読み込む
     * 
     * 
     */
    public void loadHelpImage()
    {
        helpImage = parent.loadImageFromFile("a2B_ListHelp.png");
        return;
    }
}
//--------------------------------------------------------------------------
//  a2BmessageListForm  --- 一覧画面
//
//
//--------------------------------------------------------------------------
//   MRSa (mrsa@sourceforge.jp)
