import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import java.io.InputStream;
import java.util.Date;
import java.util.Vector;

/**
 * スレ参照画面の表示クラス
 * 
 * @author MRSa
 *
 */
public class a2BMessageViewForm extends Canvas implements CommandListener
{
    private a2BMain              parent          = null;
    private a2Butf8Conveter      stringConverter = null;
    private a2BMessageParser     messageParser   = null;
    private a2BsubjectDataParser subjectParser   = null;
    private a2BNgWordManager     ngWordManager   = null;
    private String               parsedString    = null;
    private Vector               msgLines        = null;
    private Vector               commandList     = null;
    private a2BmonaFontWidth     monaFontWidth   = null;

    private int                 zoneScreenCount = 1;   // レスまとめ読み個数

    // コマンド群...
    private Command rtnCmd    = new Command("一覧へ",         Command.EXIT, 0);
    private Command quitCmd   = new Command("終了",           Command.SCREEN, 13);
    private Command outputCmd = new Command("レス出力",       Command.SCREEN, 12);
    private Command outFilCmd = new Command("出力File名変更", Command.SCREEN, 11);
    private Command ngEntCmd  = new Command("NG登録",         Command.SCREEN, 10);
    private Command ngCmd     = new Command("NG切替",         Command.SCREEN, 9);
    private Command copyCmd   = new Command("レス編集",       Command.SCREEN, 8);
    private Command partCmd   = new Command("新着確認",       Command.SCREEN, 3);
    private Command writeCmd  = new Command("書き込み",       Command.SCREEN, 2);
    private Command modeCmd   = new Command("モードMenu",     Command.SCREEN, 4);
//    private Command chgCmd    = new Command("Font切替",       Command.SCREEN, 7);
//    private Command scrCmd    = new Command("画面サイズ",     Command.SCREEN, 6);
//    private Command infoCmd   = new Command("スレ情報",       Command.SCREEN ,5);
//    private Command backCmd   = new Command("戻る",           Command.SCREEN, 3);

    private long       repeatingStartDate   = 0;
    private int        repeatingKeyCode     = 0;
    private int        currentMessageIndex  = 0;
    private int        messageNumber        = 1;
    private int[]      messageJumpList      = null;
    private int[]      pickupMessageList    = null;
    private int        jumpListIndex        = 0;    
    private int        currentPage          = 0;
    private int        screenLines          = 0;
    private int        msgPage              = 0;
    private int        buildNofPickupList   = 0;
    private int        currentPickupList    = -1;
    private Font       screenFont           = null;
    private int        informationColor     = 0;
    private int        triangleColor        = 0;
    private int        crossColor           = 0;
    private char       informationChar      = 0;
    private int        commandJumpNumber    = 0;
    private int        showXAxisforAAmode   = 0;
    private Font       fontKeepAASize        = null;
    private int        keepAAscreenLines    = 0;
    private int        messageBackColor     = 0x00f0f0f0;
    private int        messageForeColor     = 0x00000000;
    private int        searchDirection      = 0;
    private int        ngWordCheckDirection = 0;
    private int        screenHeight         = 0;
    private int        screenWidth          = 0;
    private int        AA_WIDTH_MARGIN      = 0;
    private int        currentGwtPageNum    = -1;
    private Image      wallPaperImage       = null;
    private Image      viewHelpImage        = null;
    private Image      jpegPreviewImage     = null;
    private String     searchMessageString  = null;
    private String     reservedThreadNumber = null;
    private String     previewURL           = null;
    private String     displayFileName      = null;
    private boolean   searchIgnoreCase     = false;
//    private boolean   isFullScreen         = false;
    private boolean   isCommandMode        = false;
    private boolean   painting             = false;
    private boolean   buildingMessage      = false;
    private boolean   checkNgWord          = false;
//    private boolean   doAbone              = false;
    private boolean   isHelpShowMode       = false;
    private boolean   isNumberMovingMode   = false;
    private boolean   isAsciiArtShowMode   = false;
    private boolean   isSearchingMode      = false;
    private boolean   isSearchExecution    = false;
    private boolean   isFirstTime          = true;
    private boolean   drawInformation      = false;
    private boolean   isMax                = false;
    private boolean   previewJpeg          = false;
    private boolean   previewText          = false;
    private boolean   useCacheDirectory    = false;

    private final int nofJumpList          = 24;
    private final int nofPickupMessageList = 16;
    private final int  LIMIT_WIDTH         = 999;

    private final int  PARSING_NORMAL      = 0;
    private final int  PARSING_RAW         = 1;
    private final int  PARSING_HTML        = 2;
//    private final int  PARSING_A2B_MEMO    = 3;
    private int        messageParsingMode   = PARSING_NORMAL;
    private String      busyMessage         = null;  // メッセージ表示...
    private String      busyInformation      = null;
    private long       keyRepeatDelayMs    = 400;    // キーリピート開始までの時間(ms)

    public boolean    lockOperation         = false;  // 画面操作の禁止フラグ...

    /**
     * コンストラクタ
     * 
     * @param arg0
     * @param object
     * @param subject
     * @param utf8Converter
     * @param ngManager
     */
    public a2BMessageViewForm(String arg0, a2BMain object, a2BsubjectDataParser subject, a2Butf8Conveter utf8Converter, a2BNgWordManager ngManager) 
    {
        // タイトル
        setTitle(arg0);

        parent           = object;
        subjectParser    = subject;
        stringConverter  = utf8Converter;
        ngWordManager    = ngManager;
        messageParser    = new a2BMessageParser(stringConverter);
        messageParser.setConvertHanZen(utf8Converter.isConvertHanZen());
        fontKeepAASize   = Font.getDefaultFont();
        messageJumpList   = new int[nofJumpList];
        jumpListIndex     = 0;

        pickupMessageList  = new int[nofPickupMessageList];
        buildNofPickupList = 0;
        currentPickupList  = -1;

        // コマンドを追加する
        this.addCommand(writeCmd);
        this.addCommand(copyCmd);
        this.addCommand(modeCmd);
        this.addCommand(partCmd);
        this.addCommand(ngEntCmd);
        this.addCommand(ngCmd);
        this.addCommand(outFilCmd);
        this.addCommand(outputCmd);
        this.addCommand(quitCmd);
        this.addCommand(rtnCmd);
        this.setCommandListener(this);
    }

    /*
     *   フォントと画面サイズを設定する
     * 
     * 
     */
    public void prepareScreenSize()
    {
        // モナーフォント幅応答クラスを用意する
        monaFontWidth = new a2BmonaFontWidth();
        
        // フォントと画面サイズ、フォント色を取得し、設定する
        int viewFontSize = parent.getViewFontSize();
        screenFont       = Font.getDefaultFont();
        try
        {
            screenFont       = Font.getFont(screenFont.getFace(), screenFont.getStyle(), viewFontSize);
        }
        catch (Exception ex)
        {
            screenFont       = Font.getDefaultFont();
        }
        messageForeColor = parent.getViewForeColor();
        messageBackColor = parent.getViewBackColor();

        // フルスクリーンモードを設定する
        setFullScreenMode(parent.getViewScreenMode());

        // NGワード検出設定を反映させる
        checkNgWord = parent.getNgWordDetectMode();
        
        // レスまとめ読みモードの値を取得する        
        zoneScreenCount = parent.getResBatchMode();
        
        AA_WIDTH_MARGIN = parent.getAAwidthMargin();
        return;
    }
    
    /**
     * AAモードのマージンを設定する
     * 
     * @param margin
     */
    public void setAAmodeWidthMargin(int margin)
    {
        AA_WIDTH_MARGIN = margin;
        return;
    }
    
    
    /**
     *  画面スクリーンサイズを設定する。。。
     *
     */
    public void setupScreenSize()
    {
        // タイトルの設定...
        setTitle(" ");
        
         // 表示可能行を設定する
        setLineWidthHeight(getWidth(), getHeight());

        return;
    }

    /**
     *  画面描画のメイン
     * 
     * 
     */
    public void paint(Graphics g)
    {
        // 画面描画中のときには折り返す
        if (painting == true)
        {
            return;
        }

        // メッセージ構築中の時には折り返す
        if (buildingMessage == true)
        {
            return;
        }
        
        // 表示データがない場合には折り返す
        if ((msgLines == null)||(msgLines.size() == 0))
        {
            // 実行中インフォメーションを表示する...
            if (busyInformation != null)
            {
                g.setColor(0x00d8ffd8);
                g.fillRect(0, screenHeight + 1, getWidth(), getHeight() - (screenHeight + 1));
                g.setFont(screenFont);
                g.setColor(0);
                g.drawString(busyInformation, 0, (screenHeight + 1), (Graphics.LEFT | Graphics.TOP));

/**
                // 真ん中に表示すると目立つので...
                int y = screenHeight / 2;
//                g.setColor(0x00ffdddd);
                g.setColor(0x00ffffd2);
                g.fillRect(10, (y - 3), (screenWidth - 20), (screenFont.getHeight() + 6));
                g.setColor(0);
                g.drawRect(10, (y - 3), (screenWidth - 20), (screenFont.getHeight() + 6));
                g.drawString(busyInformation, 20, y, (Graphics.LEFT | Graphics.TOP));
**/
            }
            return;
        }

        // WX310用ワークアラウンド...
        if (isFirstTime == true)
        {
            isFirstTime = false;

            // 表示可能行を設定する
            setLineWidthHeight(getWidth(), getHeight());        
        }
        
        // コマンドモード (レスジャンプ番号入力モード)か？
        if (isCommandMode == true)
        {
            showCommandWindow(g);
            return;
        }
        
        // 描画開始
        painting = true;

        // 画面の塗りつぶし(画面クリア)
        g.setColor(messageBackColor);
        g.fillRect(0, 0, getWidth(), getHeight());
        
        // 壁紙があった場合...
        if (wallPaperImage != null)
        {
            // 画面原点からイメージを描画する
            g.drawImage(wallPaperImage, 0, 0, (Graphics.LEFT | Graphics.TOP));            
        }

        // 描画フォントを設定する
        g.setColor(messageForeColor);
        g.setFont(screenFont);

        try
        {
            // レスを表示する
            drawMessageMain(g);

            // 画面にマークをつける
            drawMarkScreen(g);
        }
        catch (Exception e)
        {
            // 何もしない...
        }

        // BUSYメッセージの表示
        {
            g.setColor(0x00d8ffd8);
            g.fillRect(0, screenHeight + 1, getWidth(), getHeight() - (screenHeight + 1));
            if (busyMessage != null)
            {
                g.setColor(0);
                g.drawString(busyMessage, 0, (screenHeight + 1), (Graphics.LEFT | Graphics.TOP));
            }
        }
        painting = false;

        return;
    }
    
    /**
     * レスを描画する(メイン処理)
     * 
     * @param g
     */
    private void drawMessageMain(Graphics g)
    {
        int   line   = 0;
        int   start  = 0;
        String msg   = null;

        // topPointerが負数だった場合には末尾から、正数だったら先頭から
        if (currentPage < 0)
        {
            currentPage = msgPage;
        }

        // 先頭ページを表示する
        if (currentPage == 0)
        {
            if (messageParsingMode == PARSING_NORMAL)
            {
                if (zoneScreenCount == 1)
                {
                    // レス番号を表示 (ここはマージンをとらず０から...)
                    g.setColor(messageForeColor);
                    String dispData = "[" + messageNumber + "]  ";
                    g.drawString(dispData, 1, 0, (Graphics.LEFT | Graphics.TOP));
                    int x = screenFont.stringWidth(dispData) + 1;

                    // 名前欄を表示
                    g.setColor(0x00008000);
                    dispData = messageParser.getNameField();
                    g.drawString(dispData, x, 0, (Graphics.LEFT | Graphics.TOP));
                    x = x + screenFont.stringWidth(dispData + "           ");
                    
                    // email欄を表示 (1dot幅マージンあり)
                    g.setColor(0x00fa2020);
                    dispData = messageParser.getEmailField();
                    g.drawString(dispData, 1, (screenFont.getHeight() + 1), (Graphics.LEFT | Graphics.TOP));
                }
                else
                {
                    // レスの先頭行を表示 (1dot幅マージンあり)
                    g.setColor(messageForeColor);
                    g.drawString((String) msgLines.elementAt(0), 1, 0, (Graphics.LEFT | Graphics.TOP));
                }
                line++;
            }
        }
        else
        {
            start = currentPage * (screenLines - 1);
        }
        
        // 文字のフォントを設定する
        g.setColor(messageForeColor);
        if (isAsciiArtShowMode == true)
        {
            // AAモードの表示... (AAモードはマージンをとらずに...)
            for (; ((line < screenLines)&&((line + start) < msgLines.size())); line++)
            {
                int y       = (line * (screenFont.getHeight())) + 4;
                int x       = 0;
                int limitX  = screenWidth + showXAxisforAAmode - 4;
                msg = (String) msgLines.elementAt(line + start);
                for (int msgIndex = 0; msgIndex < msg.length(); msgIndex++)
                {
                    char drawChar = msg.charAt(msgIndex);
                    if (x >= showXAxisforAAmode)
                    {
                        g.drawChar(drawChar, (x - showXAxisforAAmode), y, (Graphics.LEFT | Graphics.TOP));
                    }
                    x = x + monaFontWidth.getWidth(drawChar);
                    if (x > limitX)
                    {
                        // 表示幅を超えた...
                        break;
                    }
                }
            }
        }
        else
        {            
            // データを行単位で表示する (1dot幅マージンあり)
            for (; ((line < screenLines)&&((line + start) < msgLines.size())); line++)
            {
                msg = (String) msgLines.elementAt(line + start);
                g.drawString(msg, 1, (line * (screenFont.getHeight() + 1)), (Graphics.LEFT | Graphics.TOP));
            }
        }
        if (isMax == true)
        {
            if (line < screenLines)
            {
                // エンドマークの表示
                line--;
                g.setColor(0x00703070);
                g.drawString("--- E N D ---", 0, (line * (screenFont.getHeight() + 1)), (Graphics.LEFT | Graphics.TOP));
                g.setColor(0x00000000);
            }
            else if ((line + start) >= msgLines.size())
            {
                // エンドマークの表示
                g.setColor(0x00703070);
                g.drawString("--- E N D ---", 0, (line * (screenFont.getHeight() + 1)), (Graphics.LEFT | Graphics.TOP));
                g.setColor(0x00000000);
            }
        }
        return;
    }
    
    /**
     *  画面にマークを入れる
     *  
     * @param g
     */
    private void drawMarkScreen(Graphics g)
    {
        int markColor = 0;        
        int lineColor = 0;

        // NGワード検出を実施しているかどうか...
        if (checkNgWord == true)
        {
            lineColor = 0x00eb0aeb; // 紫色...
        }
        else
        {
            lineColor = 0;          // 黒色...
        }

        // レスのどのへん(x軸、AA表示モード)を表示しているのか表示する
        if (isAsciiArtShowMode == true)
        {
            int width = screenWidth;
            width = (width * width) / (LIMIT_WIDTH + width);

            int startWidth = ((showXAxisforAAmode) / (screenWidth - AA_WIDTH_MARGIN)) * width;

            int y = (screenHeight - 3);
            g.setColor(255, 255, 255);
            g.fillRect(0, (screenHeight - 3), screenWidth,  3);
            g.setColor(lineColor);
            g.drawLine(0, y, screenWidth, y);
            g.setColor(0x00ff007f);
            g.setColor(markColor);
            g.fillRect(startWidth, y, width, 4);
         }

        // 検索モードかどうか？
        if (isSearchingMode == true)
        {
            if (isSearchExecution == true)
            {
                // 検索をまさに実行中のとき...
                markColor = 0x000000ff;
                
                // "検索中" を表示する
                showSearchingWindow(g);
            }
            else
            {
                // 検索モードのときはマークの色を赤色に
                markColor = 0x00ff0000;
            }
        }
        
        // レスの表示位置(y軸)を表示する
        int line = (screenWidth - 3);
        g.setColor(255, 255, 255);
        g.fillRect(line, 0, line + 3, screenHeight);
        g.setColor(lineColor);
        g.drawLine(line, 0, line, screenHeight);

        int start = messageParser.getNumberOfMessages();
        if (start != 0)
        {
            int par =  messageNumber * screenHeight / start;
//            float par = ((float) messageNumber / (float) start) * ((float) screenHeight);
            start = (int) par - 2;
            if (start < 0)
            {
                start = 0;
            }
            g.setColor(markColor);
            g.fillRect(line, start, 3, 4);
        }
        
        // インフォメーション情報があった場合...
        if (drawInformation == true)
        {
            if (informationColor != 0)
            {
                // 右上に●マークをつける
                g.setColor(informationColor);
                g.fillArc((screenWidth - 15),1, 10,10, 0, 360);
                informationColor = 0;
                g.setColor(0);
            }
            if (triangleColor != 0)
            {
                // 右上に▲マークをつける
                g.setColor(triangleColor);
                g.fillTriangle((screenWidth - 10),1, (screenWidth - 15), 10, (screenWidth - 5), 10);
                triangleColor = 0;
                g.setColor(0);            
            }
            if (crossColor != 0)
            {
                // 右上に×マークをつける
                g.setColor(crossColor);
                g.drawLine((screenWidth - 12), 1, (screenWidth -  8), 10);
                g.drawLine((screenWidth - 8),  1, (screenWidth - 12), 10);
                crossColor = 0;
                g.setColor(0);            
            }
            if (informationChar != 0)
            {
            	// 右上に文字を表示する
                g.setColor(0x00f000f0);
                g.drawChar(informationChar, (screenWidth - 12), 1, (Graphics.LEFT | Graphics.TOP));
                informationChar = 0;
                g.setColor(0);            
            }
            drawInformation = false;
            lockOperation   = false;
        }

        // JPEG表示モード/ヘルプ表示モードのチェック...
        if ((previewJpeg == true)&&(jpegPreviewImage != null))
        {
            // プレビューイメージを表示する
            showImage(g, jpegPreviewImage);
        }
        else if ((isHelpShowMode == true)&&(viewHelpImage != null))
        {
            // ヘルプ表示を行う
            showImage(g, viewHelpImage);            
        }

        // 数字キー入力モード..
        if (isNumberMovingMode == true)
        {
            g.setColor(0x00ff00ff);
            g.fillRect(0, 0, 2, 12);
            g.setColor(0);
        }

        return;
    }    
    
    /**
     * ヘルプの表示モードメイン
     * 
     * @param g
     */
    private void showImage(Graphics g, Image viewImage)
    {
        // イメージの表示座標を決める
        int x = ((screenWidth  - viewImage.getWidth())  / 2);
        if ((x + viewImage.getWidth()) > screenWidth)
        {
            x = 0;
        }

        int y = ((screenHeight - viewImage.getHeight()) / 2);
        if ((y + viewImage.getHeight()) > screenHeight)
        {
            y = 0;
        }
        
        // ヘルプイメージを描画
        g.setColor(0);
        g.drawImage(viewImage, x, y, (Graphics.LEFT | Graphics.TOP));

        return;
    }

    /**
     *   プレビューウィンドウを消去する
     * @param keyCode
     */
    private void clearPreviewWindow(int keyCode)
    {
        boolean doDelete = false;

        // JPEGファイル・テキストデータのプレビューテキストを消す
        if (keyCode == Canvas.KEY_NUM0)
        {
            // 0キー
            doDelete = true;
            currentGwtPageNum = -1;
        }

        // ファイルを消す指示があったか？
        if ((doDelete == true)&&(displayFileName != null))
        {
            // ファイルを消す...
            parent.doDeleteFile(displayFileName);
        }
        displayFileName = null;

        busyMessage = null;
        jpegPreviewImage = null;
        previewJpeg = false;
        previewText = false;
        System.gc();
        repaint();
        
        return;
    }

    /*
     *   キー入力する
     * 
     * 
     */
    public void keyPressed(int keyCode)
    {
        parent.keepWatchDog(0);

        // メッセージ構築中にはキー操作を受け付けない
        if ((buildingMessage == true)||(lockOperation == true))
        {
            return;
        }
//        drawInformation = false;

        if ((previewJpeg == true)||(previewText == true))
        {
            clearPreviewWindow(keyCode);
            return;
        }

        boolean repaint   = true;
        repeatingKeyCode   = 0;
        repeatingStartDate = 0;
        if (keyCode == 0)
        {
            // キー入力がなければ折り返す
            return;
        }
        if (isCommandMode == true)
        {
            // 数字入力モードのとき...
            if (numberInputMode(keyCode) == true)
            {
                repaint();
                return;
            }
        }
        else if (isNumberMovingMode == true)
        {
            // 数字でカーソル移動モードのとき...
            if (numberMovingMode(keyCode) == true)
            {
                repaint();
                return;
            }
        }
        else
        {
            // 通常の数字入力モードチェック
            if (numberKeyInMode(keyCode) == true)
            {
                repaint();
                return;
            }
        }

        // カーソルキーの入力。。。
        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            repaint = inputLeft();
            break;
              
          case Canvas.RIGHT:
            repaint = inputRight();
            break;

          case Canvas.DOWN:
            repaint = inputDown();
            break;

          case Canvas.UP:
            repaint = inputUp();
            break;

          case Canvas.FIRE:
            repaint = inputFire();
            break;

          case Canvas.GAME_A:
            // GAME-Aキー : 数字キーでスレ移動モードにする
            repaint = inputGameA();
            break;

          case Canvas.GAME_B:
            // GAME-Bキー、ヘルプモードを解除する
            repaint = inputGameB();
            break;

          case Canvas.GAME_C:
            // クリアキー：前回ジャンプしたところに戻る
            repaint = inputGameC();
            break;

          case Canvas.GAME_D:
            // ブラウザで開く
            repaint = inputGameD();
            break;

          default:
            repaint = false;
            break;
        }
        if (repaint == true)
        {
            repaint();
        }
        return;
    }
    
    /**
     *  数字が入力するモード
     * 
     * @param keyCode
     * @return
     */
    private boolean numberKeyInMode(int keyCode)
    {
        boolean repaint = true;  // 再描画

        // 通常モード時のキー入力処理...
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            // 機能の実行。。。
            executeFunctionFeature();
            break;

          case Canvas.KEY_NUM1:
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return (true);
            }
            // お気に入りに登録する
            setFavoriteThread();
            break;

          case Canvas.KEY_NUM2:
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return (true);
            }
            // 書き込み画面を表示可能なスレだった場合、書き込み画面に切り替える
            if (parent.canWriteMessage() == true)
            {
                subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
                String title = stringConverter.parsefromSJ(messageParser.getThreadTitle());
                parent.OpenWriteForm(false, title, currentMessageIndex, messageNumber);
            }
            break;

          case Canvas.KEY_NUM3:
            //  参照画面から抜ける
            endShowMessage();
            break;

          case Canvas.KEY_NUM4:
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return (true);
            }
            // スレ情報を表示する
            showMessageInformation();
            break;

          case Canvas.KEY_NUM5:
            // スレ内検索
              setSearchKeyword();
            break;

          case Canvas.KEY_NUM6:
            // AA表示モード...
            if (isAsciiArtShowMode == true)
            {
                // AAモードOFFに切り替える
                isAsciiArtShowMode = false;
                screenFont = fontKeepAASize;
                screenLines = keepAAscreenLines;
            }
            else
            {
                // AAモードONに切り替える
                isAsciiArtShowMode = true;
                fontKeepAASize = screenFont;
                keepAAscreenLines = screenLines;
                screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_SMALL);
            }

            // 画面サイズのセットアップ...
            setLineWidthHeight(getWidth(), getHeight());

            // レスの先頭へ移動
            currentPage = 0;

            // データを左はしから表示するように
            showXAxisforAAmode = 0;

            showMessageRelative(0);
            break;

          case Canvas.KEY_NUM7:
            // レス内URLをブラウザでOPENする
            subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
            repaint = launchWebBrowser();
            break;

          case Canvas.KEY_NUM8:
            // 「スレ番号指定取得」を行う...
            pickupThreadNumber();
            break;

          case Canvas.KEY_NUM9:
            // 差分取得を実施...
            subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
            parent.reloadCurrentMessage();
            break;
            
          case 46:
          case Canvas.KEY_POUND:
            // レスの末尾へ移動する
            jumpBottom();
            break;

          case 44:
          case Canvas.KEY_STAR:
            // レスの先頭へ移動する
            jumpTop();
            break;

          case -5:
            // 押す
            inputFire();
            break;

          case -8:
            // Nokiaのクリアキー (現物合わせ)
            // 前回ジャンプしたところに戻る
            repaint = inputGameC();
            break;

          case -50:
            // NokiaのFuncキー... (ヘルプを表示する)
            repaint = inputGameB();
            break;

          case 119:
          case  87:
            // ブラウザで開く (W)
            repaint = inputGameD();
            break;

          case 8:
            // BSキー
              if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // 実は、fireキーの入力と判断、、、ここでは何もしない
                inputFire();
                return (repaint);
            }
            //not break!!
          case 27:
          case 113:
          case 81:
           // ESCキー、Qキー
           inputGameC();
           break;

          default:
            repaint = false;
            break;
        }
        return (repaint);
    }

    /**
     *  数字で移動モード
     * 
     * @param keyCode
     * @return
     */
    private boolean numberMovingMode(int keyCode)
    {
        boolean repaint = true;  // 再描画
        switch (keyCode)
        {
          case Canvas.KEY_NUM2:
            // 前ページを表示する
            inputUp();
            break;

          case Canvas.KEY_NUM4:
            // 前メッセージ表示
            inputLeft();
            break;


          case Canvas.KEY_NUM6:
            // 次メッセージ表示
            inputRight();
            break;

          case Canvas.KEY_NUM8:
            // 次ページを表示する
            inputDown();
            break;

          case Canvas.KEY_NUM9:
            // 差分取得を実施...
            subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
            parent.reloadCurrentMessage();
            break;

          case Canvas.KEY_NUM3:
            //  参照画面から抜ける
            endShowMessage();
            break;

          case Canvas.KEY_NUM1:
            // お気に入りに登録する
            setFavoriteThread();
            break;

          case 44:
          case Canvas.KEY_STAR:
            // レスの先頭へ移動する
            jumpTop();
            break;

          case 46:
          case Canvas.KEY_POUND:
            // レスの末尾へ移動する
            jumpBottom();
            break;

          case Canvas.KEY_NUM7:
            // レス内URLをブラウザでOPENする
            subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
            repaint = launchWebBrowser();
            break;

          case Canvas.KEY_NUM0:
              // 機能の実行。。。
            executeFunctionFeature();
            break;

          case -5:
            // 押す
            inputFire();
            break;

          case 119:
          case  87:
            // ブラウザで開く (W)
            inputGameD();
            break;

          case 8:
            // BSキー...
            if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // 実は、fireキーの入力と判断、、、ここでは何もしない
                inputFire();
                return (repaint);
            }
            //not break!!
          case 27:
          case 113:
          case 81:
           // ESCキー、Qキー
           inputGameC();
           break;

          default:
            repaint = false;
            break;
        }
        return (repaint);
    }

    /**
     *  数字入力モード
     * 
     * @param keyCode
     * @return
     */
    private boolean numberInputMode(int keyCode)
    {
        boolean repaint = true;  // 再描画
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 0;
            }
            break;

          case Canvas.KEY_NUM1:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 1;
            }
            break;

          case Canvas.KEY_NUM2:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 2;
            }
            break;

          case Canvas.KEY_NUM3:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 3;
            }
            break;

          case Canvas.KEY_NUM4:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 4;
            }
            break;

          case Canvas.KEY_NUM5:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 5;
            }
            break;

          case Canvas.KEY_NUM6:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 6;
            }
            break;

          case Canvas.KEY_NUM7:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 7;
            }
            break;

          case Canvas.KEY_NUM8:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 8;
            }
            break;

          case Canvas.KEY_NUM9:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 9;
            }
            break;

          case 46:
          case Canvas.KEY_POUND:
            // 末尾レス番号
            if (commandJumpNumber == messageParser.getNumberOfMessages())
            {
                commandJumpNumber = 0;
            }
            else
            {
                commandJumpNumber = messageParser.getNumberOfMessages();
            }
            break;

          case 44:
          case Canvas.KEY_STAR:
            // 先頭レス番号
            if (commandJumpNumber == 1)
            {
                commandJumpNumber = 0;
            }
            else
            {
                commandJumpNumber = 1;
            }
            break;

          case -50:
            // Nokiaのえんぴつキー... (現物合わせ...)
            commandJumpNumber = 0;
            break;

          case -5:
            // 押す
            inputFire();
            break;

          case  8:
              // BSキー...
            if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // 実は、fireキーの入力と判断、、、ここでは何もしない
                inputFire();
                return (repaint);
            }
           //not break!!
          case -8:
          case 27:
          case 113:
          case 81:
           // Nokiaのクリアキー... (現物合わせ...) ESCキー、Qキー
           inputGameC();
           break;

          case 119:
          case  87:
            // ブラウザで開く (W)
            inputGameD();
            break;

          default:
            // 上記以外のキーだった場合...
            repaint = false;
            break;
        }
        return (repaint);
    }
    
    /**
     * 上移動
     * @return
     */
    private boolean inputUp()
    {    
        boolean ret = true;
        if (isCommandMode != true)
        {
            // 前ページを表示する
            currentPage--;
            if (currentPage < 0)
            {

                // データを左はしから表示するように
                showXAxisforAAmode = 0;

                // 前メッセージ表示
                showMessageRelative(-1 * zoneScreenCount);
                ret = false;
            }
        }
        else
        {
            // ひとつ前の解析...
            if (buildNofPickupList != 0)
            {
                currentPickupList--;
                if (currentPickupList < 0)
                {
                    commandJumpNumber = 0;
                    currentPickupList = buildNofPickupList;
                }
                else
                {
                    commandJumpNumber = pickupMessageList[currentPickupList];
                }
            }
        }
        return (ret);
    }
    
    /**
     * 下移動
     * @return
     */
    private boolean inputDown()
    {
        boolean ret = true;
        if (isCommandMode != true)
        {
            // 次ページを表示する
            currentPage++;
            if (currentPage > msgPage)
            {
                currentPage = 0;

                // データを左はしから表示するように
                showXAxisforAAmode = 0;

                // 最終レスではなければ、次レスを表示する
                if (messageNumber != messageParser.getNumberOfMessages())
                {
                    showMessageRelative(zoneScreenCount);
                }
                else
                {
                    currentPage = msgPage;
                }
                ret = false;
            }
        }
        else
        {
            // ひとつ次の解析...
            if (buildNofPickupList != 0)
            {
                currentPickupList++;
                if (currentPickupList >= buildNofPickupList)
                {
                    commandJumpNumber = 0;
                    currentPickupList = -1;
                }
                else
                {
                    commandJumpNumber = pickupMessageList[currentPickupList];
                }
            }
        }
        return (ret);
    }

    /**
     * 左移動
     * @return
     */
    private boolean inputLeft()
    {
        if (checkNgWord == true)
        {
            ngWordCheckDirection = -1;
        }

        if (isSearchingMode == true)
        {
            // スレ内検索モードのとき...前方向へ検索する
            executeSearchKeyword(-1);
            return (false);
        }        
        if (isAsciiArtShowMode == true)
        {
            showXAxisforAAmode = showXAxisforAAmode - AA_WIDTH_MARGIN;
            if (showXAxisforAAmode < 0)
            {
                showXAxisforAAmode = 0;
            }
            showMessageRelative(0);
            return (false);
        }
        currentPage = 0;
        showMessageRelative(-1 * zoneScreenCount);
        return (false);
    }

    /**
     * 右移動
     * @return
     */
    private boolean inputRight()
    {
        if (checkNgWord == true)
        {
            ngWordCheckDirection = 1;
        }
        if (isSearchingMode == true)
        {
            // スレ内検索モードのとき...次方向へ検索する
            executeSearchKeyword(1);
            return (false);
        }
        if (isAsciiArtShowMode == true)
        {
            showXAxisforAAmode = showXAxisforAAmode + AA_WIDTH_MARGIN;
            if (showXAxisforAAmode > LIMIT_WIDTH)
            {
                showXAxisforAAmode = LIMIT_WIDTH;
            }
            showMessageRelative(0);
            return (false);
        }
        currentPage = 0;
        showMessageRelative(zoneScreenCount);
        return (false);
    }

    /**
     * Fireボタンを押したときの処理
     * @return
     */
    private boolean inputFire()
    {
        // スレ選択メニューを表示する
        busyMessage = "";
        if (isCommandMode == true)
        {
            // コマンドモードをOFFにする
            isCommandMode = false;
            if (commandJumpNumber < 0)
            {
/**
                if (parent.isGetNewArrival() == true)
                {
                    // 新着確認中の場合には、表示クリア
                    isCommandMode = true;
                    commandJumpNumber = 0;
                    return (true);
                }
**/
                //  参照画面から抜ける
                endShowMessage();
                return (true);
            }
            else if (commandJumpNumber != 0)
            {
                if (jumpListIndex < nofJumpList)
                {
                    messageJumpList[jumpListIndex] = messageNumber;
                    jumpListIndex++;
                }
                currentPage = 0;

                // データを左はしから表示するように
                showXAxisforAAmode = 0;

                showMessage(commandJumpNumber);
            }
            commandJumpNumber  = 0;
            buildNofPickupList = 0;
            currentPickupList  = -1;
        }
        else
        {
            // コマンドモードに切り替える
            isCommandMode = true;
            pickUpJumpList();
            if (buildNofPickupList > 0)
            {
                currentPickupList = 0;
                commandJumpNumber = pickupMessageList[currentPickupList];
            }
        }        
        return (true);
    }
    
    /**
     * GAME Aの入力
     * 
     * @return
     */
   private boolean inputGameA()
    {
        return (inputUp());
/*
        parent.toggleNumberInputMode(isNumberMovingMode);
        repaint();
        return (true);
*/
    }

    /**
     * GAME Bの入力
     * 
     * @return
     */
    private boolean inputGameB()
    {
        return (inputDown());
/*
        changeHelpShowMode();
        return (true);        
*/
    }

    /**
     * GAME Cの入力
     * 
     * @return
     */
    private boolean inputGameC()
    {
        busyMessage = "";
        if (isCommandMode != true)
        {
            if (jumpListIndex != 0)
            {
                jumpListIndex--;
                currentPage = 0;

                // データを左はしから表示するように
                showXAxisforAAmode = 0;

                int num = messageJumpList[jumpListIndex];
                showMessage(num);
            }
        }
        else
        {
            // コマンドモードで、数字が入力されていない場合には、コマンドモードを解除する
            if (commandJumpNumber == 0)
            {
                isCommandMode = false;
                commandJumpNumber  = 0;
                buildNofPickupList = 0;
                currentPickupList  = -1;
                return (true);
            }

            // クリアキー
            commandJumpNumber = commandJumpNumber / 10;                
        }

        // ヘルプ表示中の場合は、解除する
        if (isHelpShowMode == true)
        {
            changeHelpShowMode();
        }
        
        // ジャンプバッファがゼロの場合には、検索モードを解除する
        if (jumpListIndex == 0)
        {
            isSearchingMode = false;
            searchMessageString = null;
        }
        return (true);
    }

    /**
     * GAME Dの入力
     * 
     * @return
     */
    private boolean inputGameD()
    {
        subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
        parent.launchBrowserCurrentMessage();
        return (true);
    }

    /*
     *   キーリピートしているとき...
     * 
     * 
     */
    public void keyRepeated(int keyCode)
    {
        parent.keepWatchDog(0);

        // メッセージ構築中にはキー操作を受け付けない
        if ((buildingMessage == true)||(lockOperation == true))
        {
            return;
        }
//        drawInformation = false;

        boolean repaint = true;
        if (keyCode == 0)
        {
            return;
        }

        // 現在時刻を取得する
        Date dateTime = new Date();
        long currentTime = dateTime.getTime();
        dateTime = null;

        // キーリピートは、2回に１回、それもWAIT_TIME ms反応するようにする。。。
        if (keyCode != repeatingKeyCode)
        {
            repeatingKeyCode = keyCode;
            repeatingStartDate = currentTime;
            return;
        }
        
        // リピートイベントがkeyRepeatDelayMs ms経っていなかった場合にはもう一度...
        if (currentTime < (repeatingStartDate + keyRepeatDelayMs))
        {
            return;
        }
        repeatingKeyCode   = 0;
        repeatingStartDate = 0;

        if (isNumberMovingMode == true)
        {
            repaint = true;
            switch (keyCode)
            {
              case Canvas.KEY_NUM2:
                // 前ページを表示する
                inputUp();
                break;

              case Canvas.KEY_NUM4:
                // 前メッセージ表示
                inputLeft();
                break;

              case Canvas.KEY_NUM6:
                // 次メッセージ表示
                inputRight();
                break;

              case Canvas.KEY_NUM8:
                // 次ページを表示する
                inputDown();
                break;

              default:
                repaint = false;
                break;
            }
            if (repaint == true)
            {
                repaint();
                return;
            }            
        }

        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            // 前メッセージ表示
            repaint = inputLeft();
            break;
              
          case Canvas.RIGHT:
            // 次メッセージ表示
            repaint = inputRight();
            break;

          case Canvas.DOWN:
            // 次ページを表示する
            repaint = inputDown();
            break;
          case Canvas.UP:
            // 前ページを表示する
            repaint = inputUp();
            break;

          case Canvas.GAME_A:
            // Game Aボタン処理
            repaint = inputGameA();
            break;

          case Canvas.GAME_B:
            // Game Bボタン処理
            repaint = inputGameB();
            break;

          case Canvas.FIRE:
          case Canvas.GAME_C:
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
     *  レスの末尾へ移動する
     *
     */
    private void jumpBottom()
    {
        // ジャンプ元番号を記憶する
        if (jumpListIndex < nofJumpList)
        {
            messageJumpList[jumpListIndex] = messageNumber;
            jumpListIndex++;
        }

        // レスの末尾へ移動
        currentPage = 0;

        // データを左はしから表示するように
        showXAxisforAAmode = 0;

        showMessage(messageParser.getNumberOfMessages());
        
        return;
    }
    
    
    /**
     *  レスの先頭へ移動する
     *
     */
    private void jumpTop()
    {
        // ジャンプ元番号を記憶する
        if (jumpListIndex < nofJumpList)
        {
            messageJumpList[jumpListIndex] = messageNumber;
            jumpListIndex++;
        }

        // レスの先頭へ移動
        currentPage = 0;

        // データを左はしから表示するように
        showXAxisforAAmode = 0;

        showMessage(1);
        return;
    }
    
    /**
     *  押した座標を処理する...
     * 
     * 
     */
    public void pointerPressed(int x, int y)
    {
        parent.keepWatchDog(0);

        if (lockOperation == true)
        {
            // 操作禁止中...何もしない。
            return;
        }
//        drawInformation = false;

        int zoneWidth = (screenFont.charWidth('□') * 3);
        if (x > (getWidth() - zoneWidth))
        {
            // 画面の右上端、右下端をタップした場合...
            int index =  y / (screenFont.getHeight() + 1);
            if (index <= 2)
            {
                // レスの先頭へ移動する
                jumpTop();
                return;
            }
            else if (index >= (screenLines - 2))
            {
                // レスの末尾へ移動する
                jumpBottom();
                 return;
            }
        }
        if (x < zoneWidth)
        {
            // 画面の左上端、左下端をタップした場合...
            int index =  y / (screenFont.getHeight() + 1);
            if (index <= 2)
            {
                // 左上端、リンクを開く (レス内URLをブラウザでOPENする)
                subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
                launchWebBrowser();
                repaint();
                return;
            }
            else if (index >= (screenLines - 2))
            {
                // 左下端、機能の実行。。。
                executeFunctionFeature();
                 return;
            }            
        }

        if (y < (screenFont.getHeight() * 2))
        {
            // 一番上の行をタップされたときは、一覧へ戻る
            endShowMessage();
            return;
        }        
        
        if (y <= (getHeight() / 2))
        {
            // １ページ戻り
            inputUp();
        }
        else
        {
            // １ページ送り
            inputDown();
        }
        repaint();
        return;
    }

    /*
     *  離した座標を処理する...
     * 
     * 
     */
    public void pointerReleased(int x, int y)
    {
        return;
    }
    

    /*--*/
    // コマンドボタン実行時の処理
    public void commandAction(Command c, Displayable d) 
    {
        parent.keepWatchDog(0);

        if ((c != quitCmd)&&(lockOperation == true))
        {
            // 操作禁止中...何もしない。
            return;
        }
//        drawInformation = false;

        // コマンドボタン実行処理
        if (c == rtnCmd)
        {
            //  参照画面から抜ける
            endShowMessage();
        }
        else if (c == modeCmd)
        {
            // メニュー２
            showMenuSecond();
        }
/*
        else if (c == backCmd)
        {
            // 前回ジャンプしたところに戻る
            if (jumpListIndex != 0)
            {
                jumpListIndex--;
                currentPage = 0;

                // データを左はしから表示するように
                showXAxisforAAmode = 0;

                int num = messageJumpList[jumpListIndex];
                showMessage(num);
            }
        }
*/
        else if (c == ngEntCmd)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // NG登録フォームを開く
            openNgEntryForm();
        }
        else if (c == ngCmd)
        {
            // NG検査する/しないの切り替え
            parent.toggleNgWordDetectMode();
            checkNgWord = parent.getNgWordDetectMode();

            // レスの再表示
            showMessage(messageNumber);
        }
        else if (c == partCmd)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // 新着確認を実施する...
            parent.startGetLogList();
        }
        else if (c == writeCmd)
        {
            if (parent.isGetNewArrival() == true)
            {
                // 新着確認中は禁止する
                return;
            }
            // 書き込み画面を表示可能なスレだった場合、書き込み画面に切り替える
            if (parent.canWriteMessage() == true)
            {
                subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
                String title = stringConverter.parsefromSJ(messageParser.getThreadTitle());
//                parent.OpenWriteForm(false, title, cureentMessageIndex, messageNumber);
                parent.OpenWriteForm(false, title, currentMessageIndex, 0);
                return;
            }
        }
        else if (c == outFilCmd)
        {
            // レス出力ファイル名の指定
            changeOutputFileName();
        }
        else if (c == copyCmd)
        {
            // レスを編集モードにする
            messageEditText(true);
        }
        else if (c == outputCmd)
        {
            // レスをメモファイルへ出力する
            messageEditText(false);
        }
        else if (c == quitCmd)
        {
            // a2Bを終了させる
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
                // 何もしない...
            }
            return;
        }
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

        Vector commandList = new Vector(9);
        commandList.addElement("一覧画面に戻る");
        commandList.addElement("数字キー移動モード");
        commandList.addElement("ヘルプ表示モード");
        commandList.addElement("画面モード");
        commandList.addElement("Fontサイズ切替");
        commandList.addElement("BE profile参照");
        commandList.addElement("レスまとめ読みモード");
        commandList.addElement("おすすめ２ちゃんねる");
        commandList.addElement("SJIS/EUC/UTF8/JIS切替");

        parent.openSelectForm(title, commandList, 0, parent.SHOW_VIEWCOMMANDSECOND);
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
    public void executeMenuSecond(byte scene, int index)
    {
        if (index == 0)
        {
            //  参照画面から抜ける
            endShowMessage();
        }
        if (index == 1)
        {
            // 数字キー移動モード切替
            parent.toggleNumberInputMode(isNumberMovingMode);
        }
        if (index == 2)
        {
            // ヘルプ表示モード切替
            changeHelpShowMode();
        }
        if (index == 3)
        {
            // スクリーンサイズの変更
            boolean isFullScreen = parent.getViewScreenMode();
            if (isFullScreen == true)
            {
                isFullScreen = false;
            }
            else
            {
                isFullScreen = true;
            }
            changeScreenMode(isFullScreen);
        }
        if (index == 4)
        {
            // フォントサイズ変更
            changeFontSize(false);
        }
        if (index == 5)
        {
            // BE profile参照
            viewBeProfile();
        }
        if (index == 6)
        {
            // レスまとめ読みモード
            if (zoneScreenCount == 1)
            {
                zoneScreenCount = parent.getResBatchCount();
            }
            else
            {
                zoneScreenCount = 1;
            }
            parent.setResBatchMode(zoneScreenCount);
            showMessage(messageNumber);
        }
        if (index == 7)
        {
            // おすすめ２ちゃんねるの表示
            viewOsusume2ch();
        }
/**/
        if (index == 8)
        {
            // 表示モードの切り替え(SJIS/EUC/UTF8)
            parent.toggleViewKanjiMode(a2BsubjectDataParser.PARSE_TOGGLE);
            showMessage(messageNumber);

            // モードを右上に表示する
            int mode = parent.getViewKanjiMode();
            if (mode == a2BsubjectDataParser.PARSE_UTF8)
            {
            	informationChar = 'U';
                drawInformation = true;
            }
            else if (mode == a2BsubjectDataParser.PARSE_JIS)
            {
                informationChar = 'J';
                drawInformation = true;
            }
            else if (mode == a2BsubjectDataParser.PARSE_EUC)
            {
                informationChar = 'E';	
                drawInformation = true;
            }
            else // if (mode == a2BsubjectDataParser.PARSE_SJIS)
            {
            	informationChar = 'S';
                drawInformation = true;
            }
        }
/**/
        System.gc();
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
        int feature = parent.getFunctionKeyFeature(parent.SCENE_VIEWFORM);
        if (feature == 0)
        {
            // フォントサイズの変更
            changeFontSize(false);
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
            // スレ情報を表示する
            if (parent.isGetNewArrival() != true)
            {
                // 新着確認中は禁止する
                showMessageInformation();
            }
        }
        else if (feature == 4)
        {
            // NG検査する/しないの切り替え
            parent.toggleNgWordDetectMode();
            checkNgWord = parent.getNgWordDetectMode();

            // レスの再表示
            showMessage(messageNumber);
        }
        else if (feature == 5)
        {
            // レスをメモファイルへ出力する
            messageEditText(false);            
        }
        else if (feature == 6)
        {
            // モードメニューの表示
            showMenuSecond();
        }
        else if (feature == 7)
        {
            // レスまとめ読みモード
            if (zoneScreenCount == 1)
            {
                zoneScreenCount = parent.getResBatchCount();
            }
            else
            {
                zoneScreenCount = 1;
            }
            parent.setResBatchMode(zoneScreenCount);
            showMessage(messageNumber);            
        }
        else if (feature == 8)
        {
            // クリア
            inputGameC();
        }
        else if (feature == 9)
        {
            // おすすめ２ちゃんねる表示
            viewOsusume2ch();
        }
        return;
    }

    // スクリーンサイズの変更
    private void changeScreenMode(boolean isFullScreen)
    {
        currentPage = 0;
        setFullScreenMode(isFullScreen);
        parent.setViewScreenMode(isFullScreen);

        // 表示可能行を再設定し、レスを再表示...
        sizeChanged(getWidth(), getHeight());

        return;
    }
    
    // 画面サイズ変更時に呼ばれる処理...
    public void sizeChanged(int w, int h)
    {
        setLineWidthHeight(w, h);

        // レスの再表示
        showMessage(messageNumber);

        return;
    }

    /**
     * 表示可能領域を設定する
     * @param w
     * @param h
     */
    private void setLineWidthHeight(int w, int h)
    {
        // 表示可能行を再設定する (1行分、BUSYウィンドウ表示として確保...)
        screenWidth  = w;
        screenHeight = h - screenFont.getHeight() * 1;
        screenLines  = (screenHeight / (screenFont.getHeight() + 1));

        return;
    }
    
    /**
     *  フォント色を変更する
     *
     */
    private void changeFontColor()
    {
        if ((messageBackColor == 0x00000000)||(messageBackColor == 0x008080f0)||(messageBackColor == 0x00d080f0)||(messageBackColor == 0x00004101))
        {
            if (messageBackColor == 0x00000000)
            {
                // ブルーグレー、背景は白
                parent.setViewFontColor(0x008080f0, 0x00f0f0f0);                
            }
            else if (messageBackColor == 0x008080f0)
            {
                // 緑、背景は白
                parent.setViewFontColor(0x00004101, 0x00f0f0f0);                
            }
            else if (messageBackColor == 0x00004101)
            {
                // 白っぽいブルーグレー、背景は黒
                parent.setViewFontColor(0x00d080f0, 0x00080808);                
            }
            else
            {
                // 文字はくろ、背景は白
                parent.setViewFontColor(0x00000000, 0x00f0f0f0);                
            }
        }
        else
        {
            // 色を反転させる..
            parent.setViewFontColor(messageBackColor, messageForeColor);
        }

        // 色を取り出す...
        messageForeColor = parent.getViewForeColor();
        messageBackColor = parent.getViewBackColor();
        return;
    }
    
    // フォントサイズの変更
    public void changeFontSize(boolean isReset)
    {
        if (isAsciiArtShowMode == true)
        {
            // AA表示モードの時には、フォントサイズを変更できないようにする。
            return;            
        }
        
        // フォントサイズの変更 (小(色反転)→中→大→小(色反転)→中→... と切り替える)
        int size;
        if (isReset == false)
        {
            size = screenFont.getSize();
            switch (size)
            {
              case Font.SIZE_MEDIUM:
                size = Font.SIZE_LARGE;
                break;

              case Font.SIZE_SMALL:
                size = Font.SIZE_MEDIUM;
                break;

              case Font.SIZE_LARGE:
              default:
                size = Font.SIZE_SMALL;
                screenFont = Font.getDefaultFont();
                changeFontColor();
                break;
            }
        }
        else
        {
            // フォントサイズをリセットする
            size = parent.getViewFontSize();

            // 色をリセットする
            messageForeColor = parent.getViewForeColor();
            messageBackColor = parent.getViewBackColor();
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
        parent.setViewFontSize(size);

        // 表示可能行を再設定する
        setLineWidthHeight(getWidth(), getHeight());

        // レスの再表示
        currentPage = 0;

        // データを左はしから表示するように
        showXAxisforAAmode = 0;

        showMessage(messageNumber);

        return;
    }    

    // 参照画面の終了
    private void endShowMessage()
    {
        // 情報更新中...
        msgLines = null;
        isCommandMode = false;
        busyInformation = "情報書き込み中...";
        repaint();
        serviceRepaints();

        // 終了ルーチンを呼び出す...
        Thread thread = new Thread()
        {
            public void run()
            {
                finishShowMessage();
            }
        };

        // メッセージ更新中のしるしを立てる
//        buildingMessage = true;    

        thread.start();

        return;
    }

    /**
     *  参照画面を抜ける処理...
     * 
     */
    private void finishShowMessage()
    {
        // メッセージ更新中のしるしを立てる
//        buildingMessage = true;

        // スレタイトルデータの更新を行う...
        parent.updateThreadTitle(messageParser.getThreadTitle(), 0, messageParser.getThreadTitleLength());

        // 末尾の値を取得する
        int bottom = messageParser.getNumberOfMessages();
        
        // データのクリア...
        messageParser.leaveMessage();
        
        // 親に対し、画面を抜けたことを通知する
        parent.EndViewForm(messageNumber, bottom);

        // 画面が戻ったとき、ページを先頭にする
        currentPage = 0;

        // データを左はしから表示するように
        showXAxisforAAmode = 0;

        // 使用する漢字コードをShift JISに戻す
        parent.toggleViewKanjiMode(a2BsubjectDataParser.PARSE_SJIS);
        
        // スレ内レスジャンプリストをクリアする
        jumpListIndex     = 0;
        
        // 検索モードと画面更新モードをクリアする...
        painting          = false;
        isSearchExecution = false;
        buildingMessage   = false;

        return;
    }

    // NGワード登録フォームを開く...
    private void openNgEntryForm()
    {
        // 表示ルーチンを呼び出す...
        Thread thread = new Thread()
        {
            public void run()
            {
                // デフォルトキーワードの抽出
                String defaultKeyword = "";

                int startPos = parsedString.indexOf("ID:");
                if (startPos >= 0)
                {
                    int endPos = parsedString.indexOf("\r", startPos);
                    if (endPos >= 0)
                    {
                        defaultKeyword = parsedString.substring(startPos, endPos);
                    }
                }        

                // NGワード登録用画面を開く
                parent.OpenWriteFormUsingWordInputMode(parent.SCENE_NGWORD_INPUT, "NGワード登録", "NGワード", defaultKeyword, false);
            }
        };
        thread.start();

        return;
    }

    
    /**
     * 検索中の表示...
     * 
     */
    private void showSearchingWindow(Graphics g)
    {
        int width, height;
        
        width  = screenWidth / 2 - 10;
        height = screenFont.getHeight() + 4;
        
        // 画面の塗りつぶし
        g.setColor(0x00ffffc0);
        g.fillRect(10, (screenHeight - height - 2), width * 2, (screenHeight - 2));
        g.setColor(0, 0, 0);
        g.setFont(screenFont);
        
        // コマンドウィンドウタイトルの表示
        g.drawString("検索中: " + searchMessageString, 15, (screenHeight - height), (Graphics.LEFT | Graphics.TOP));

        return;        
    }
    
    // コマンドウィンドウの表示...
    private void showCommandWindow(Graphics g)
    {
        int width, height;
        
        width  = screenWidth / 2 - 20;
        height = screenFont.getHeight() + 6;
        
        // 画面の塗りつぶし
        g.setColor(210,210, 210);
        g.fillRect(20, (screenHeight - height - 2), width * 2, height);
        g.setColor(0, 0, 0);
        g.setFont(screenFont);
        
        // コマンドウィンドウタイトルの表示
        if (commandJumpNumber < 0)
        {
            // レス番号なしの表示
            g.drawString("移動先 : (一覧へ戻る) ", 20 + 6, (screenHeight - height), (Graphics.LEFT | Graphics.TOP));            
        }
        else if (commandJumpNumber == 0)
        {
            // レス番号なしの表示
            g.drawString("移動先レス番号： ", 20 + 6, (screenHeight - height), (Graphics.LEFT | Graphics.TOP));
        }
        else
        {
            // レス番号付きの表示 
            g.drawString("移動先レス番号： " + commandJumpNumber, 20 + 6, (screenHeight - height), (Graphics.LEFT | Graphics.TOP));
        }
        return;        
    }

    /**
     * レスを取得し加工、表示の準備を行う
     *
     * @param getCount 連続して取得するレスの数
     * @return
     */
    private String prepareMessage()
    {
        String parsedMessage = null;
        boolean ngLoop = true;
        while (ngLoop == true)
        {
            // レスデータを取得する
        	int kanjiMode = parent.getViewKanjiMode();
            byte[] resData = messageParser.getMessage(messageNumber, kanjiMode);
            if (kanjiMode == a2BsubjectDataParser.PARSE_UTF8)
            {
            	// UTF8の漢字モードのときには、何もしない
            	try
            	{
                    parsedMessage = new String(resData, 0, resData.length, "UTF-8");
            	}
            	catch (Exception e)
            	{
                    parsedMessage = new String(resData, 0, resData.length);
            	}
            }
            else if (kanjiMode == a2BsubjectDataParser.PARSE_JIS)
            {
            	// JISの場合には、JIS => UTF8変換を実施する
                parsedMessage = stringConverter.parsefromJIS(resData, 0, resData.length);            	
            }
            else
            {
            	// UTF8/JIS以外の場合には、SJ => UTF8変換を実施する
                parsedMessage = stringConverter.parsefromSJ(resData, 0, resData.length);
            }

            boolean doAbone = false;
            if (checkNgWord == true)
            {
                // NGチェックモードのとき、NGキーワードが含まれているかチェックする
                doAbone = checkNgKeyword(parsedMessage);
                if (doAbone == true)
                {
                    if ((zoneScreenCount == 1)&&(ngWordCheckDirection != 0))
                    {
                        // 次の(NGでない)レスを探す
                        messageNumber = messageNumber + ngWordCheckDirection;
                        if ((messageNumber > messageParser.getNumberOfMessages())||(messageNumber < 1))
                        {
                            // レス範囲を超えた、ループを抜ける...
                            messageNumber = messageNumber - ngWordCheckDirection;
                            parsedMessage = null;
                            parsedMessage = "* * *\r\n\r\n\r\n";
                            messageParser.doAbone();
                            messageNumber = messageNumber - ngWordCheckDirection;
                            ngLoop = false;
                        }
                    }
                    else
                    {
                        // あぼーん実施中、レス番号だけを表示
                        parsedMessage = null;
                        parsedMessage = "* * *\r\n\r\n\r\n";
                        messageParser.doAbone();
                        ngLoop = false;
                    }
                }
                else
                {
                    // ループを抜ける...
                    break;
                }
            }
            else
            {
                // ループを抜ける...
                break;
            }
        }
        ngWordCheckDirection = 0;
        return ("[" + messageNumber + "] " + parsedMessage);
    }    

    /*
     *  レスデータを行単位に加工する。
     * 
     */
    private void chopData(String parsedMessage)
    {
        // テキストデータ表示モードの場合、１行追加する
        if (messageParsingMode != PARSING_NORMAL)
        {
            msgLines.addElement("");
        }

        // 表示可能キャラクタ数を設定する
        int width  = screenWidth - 5;
        int drawRange = width / screenFont.charWidth('X');
        int index  = 0;
        int top    = 0;
        String  msg = "";

        while (top < parsedMessage.length())
        {
            index    = parsedMessage.indexOf('\n', top);
            int pos = index;
            if ((index >= 0)&&(parsedMessage.charAt(index - 1) == '\r'))
            {
                pos--;
            }
            if (index < 0)
            {
                index = parsedMessage.length();
                pos   = index;
            }
            if (index == top)
            {
                top++;
                msgLines.addElement("");
            }
            else
            {
                // 先頭行を表示しない (代替手段で表示するため)
                if ((top == 0)&&(messageParsingMode == PARSING_NORMAL)&&(zoneScreenCount == 1))
                {
                    msgLines.addElement("");
                    top = index;
                    continue;
                }
                if (isAsciiArtShowMode == true)
                {
                    // 表示するデータの先頭を見つける...
                    if (pos <= 0)
                    {
                        // 最終行の場合...
                        msg = parsedMessage.substring(top);
                        msgLines.addElement(msg);
                        index = parsedMessage.length();
                    }
                    else
                    {
                        // １行切り出し... (オーバーラップしない場合...)
                        msg = parsedMessage.substring(top, pos);
                        msgLines.addElement(msg);
                    }
                }
                else
                {
                    /* 1行に収まらない行を複数の行に分割する処理 */
                    int ch    = pos - top;  // ch   : キャラクタ数
                    while (ch > 0)
                    {
                        int loop = (drawRange < ch) ? drawRange : ch;
                        if (screenFont.substringWidth(parsedMessage, top, ch) > width)
                        {
                            // とりあえず、 'X'が画面いっぱい並んでいたときよりもキャラクタ数が多いかどうか比較する
                            if (screenFont.substringWidth(parsedMessage, top, loop) >= width)
                            {
                                // いきすぎた、徐々に減らす...
                                do
                                {
                                    loop--;
                                }
                                while (screenFont.substringWidth(parsedMessage, top, loop) >= width);
                            }
                            else
                            {
                                // まだ足りない...
                                do
                                {
                                    loop++;
                                }
                                while (screenFont.substringWidth(parsedMessage, top, loop) < width);
                                loop--; // 超えた分減らす必要あり!
                            }
                        }
                        else
                        {
                            // 一行に収まる場合...
                            loop = ch;
                        }
                        msg = parsedMessage.substring(top, top + loop);
                        msgLines.addElement(msg);
                        top = top + loop;
                        ch = ch - loop;
                    }
                }
                top = index + 1;
            }
        }        
        msgLines.addElement("");
    }    
    
    /**
     *  レス表示処理...
     *  
     */
    private void showMessageMain()
    {
        // デッドロック回避...
        if (buildingMessage == true)
        {
            return;
        }
        buildingMessage = true;
        isMax = false;

        int currentMessage = messageNumber;
        int maxMsg = messageParser.getNumberOfMessages();
        
        // データ行データ
        msgLines = new Vector();

        try
        {
            // 解析したメッセージを行単位に区切る
            parsedString = prepareMessage();
            currentMessage = messageNumber;
            messageNumber++;
            for (int loop = 1; loop < zoneScreenCount; loop++)
            {
                if (messageNumber > maxMsg)
                {
                    // 解析数を超えた...breakする
                    break;
                }
                parsedString = parsedString + "\r\n=====\r\n" + prepareMessage();
                messageNumber++;
            }
            if (messageNumber > maxMsg)
            {
                parsedString = parsedString + "\r\n\r\n";  // ENDマーカ出力用領域を確保する
                isMax = true;
            }
            else
            {
                if (zoneScreenCount != 1)
                {
                    parsedString = parsedString + "\r\n=====\r\n";                    
                }
            }
            chopData(parsedString);
        }
        catch (Exception e)
        {
            // エラー発生時の情報...
            msgLines.addElement("");
        }

        // 末尾を表示しているかどうか
        if (isMax == true)
        {
            // 未読ポイントをレス末尾に設定する
            messageNumber = maxMsg;
        }
        else
        {
            // 未読ポイントをゾーン表示の先頭に戻す
            messageNumber = currentMessage;
        }
        
        // ページ数を求め、msgPageに格納する
        msgPage  = msgLines.size() / (screenLines - 1);
        if (msgPage != 0)
        {
            // １ページに収まりきらない場合には...ちょっと補正する
            if ((msgLines.size() % (screenLines - 1)) <= 1)
            {
                msgPage--;
            }
        }
        buildingMessage    = false;
        repaint();

        // 読み出したレスの場所(先頭)を記憶する
        subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, maxMsg);
        return;
    }

    // 相対値でレス番号を指定してレス表示
    public void showMessageRelative(int relativeNumber)
    {
        showMessage(messageNumber + relativeNumber);
    }
    
    /*
     *  レスの表示処理
     *  
     */
    public void showMessage(int number)
    {        
        // レスデータ構築中のときには、メッセージの更新は実施しない...
        if (buildingMessage == true)
        {
            return;
        }

        // 画面描画中のときには、メッセージの更新は実施しない...
        if (painting == true)
        {
            return;
        }
        
        // 表示レス番号を決定する
        int nofMsg = messageParser.getNumberOfMessages();
        if (number > nofMsg)
        {
            // 末尾に設定し、何もしない
            currentPage = 0;

            // データを左はしから表示するように
            showXAxisforAAmode = 0;

            messageNumber = nofMsg;
        }
        else if (number < 1)
        {
            // 先頭に設定し、何もしない
            currentPage = 0;

            // データを左はしから表示するように
            showXAxisforAAmode = 0;

            messageNumber = 1;
        }
        else
        {
            // 指定したレス番号に設定
            messageNumber = number;
        }

        // メッセージ番号を記録する
        parent.setMessageNumber(messageNumber);
        
        // 表示ルーチンを呼び出す...
        Thread thread = new Thread()
        {
            public void run()
            {
                showMessageMain();
            }
        };
        thread.start();        
    }

    /**
     * 現在表示中のレス管理番号を更新する
     * @param index
     */
    public void updateCurrentMessageIndex(int index)
    {
        currentMessageIndex = index;
    }
    
    /**
     * レスを表示する
     * 
     * @param fileName
     * @param bbsIndex
     * @param number
     */
    public void showMessage(String fileName, int bbsIndex, int number)
    {
        currentMessageIndex = bbsIndex;

        if (fileName == null)
        {
            // ファイル名が指定されていない...終了する
            return;
        }
        setTitle("スレ解析中..."); // ダミーのタイトル表示...
        msgLines = null;
        busyInformation = "スレ解析中...";
        String title = null;
        messageParsingMode = PARSING_NORMAL;
        if (bbsIndex < 0)
        {
            // BBS indexが負(== ログ参照モード)でオープンされたとき...
            title = fileName;
            if ((fileName.toLowerCase().endsWith(".txt"))||(fileName.toLowerCase().endsWith(".a2b")))
            {
                // テキスト解析モード(RAWモード)とする
                messageParsingMode = PARSING_RAW;
            }
            if ((fileName.toLowerCase().endsWith(".htm"))||(fileName.toLowerCase().endsWith(".html"))||
                (fileName.toLowerCase().endsWith(".csv"))||
                (fileName.toLowerCase().endsWith(".idx"))||(fileName.toLowerCase().endsWith(".cnf")))
            {
                // HTML解析モードとする
                messageParsingMode = PARSING_HTML;
            }
        }
        else
        {
            // BBS indexが正(== 通常参照モード)でオープンされたとき...
            title = parent.getSubjectName(bbsIndex);
        }

        // メッセージ解析モードを設定する
        messageParser.setMessageParsingMode(messageParsingMode);
        
        // 解析するファイル名を設定する
        messageParser.setFileName(fileName);

        // スレタイトルのベースを設定する
        messageParser.setThreadBaseTitle(title);
        
        // 表示レス番号を設定する
        messageNumber = number;

        // 表示ルーチンを呼び出す...
        Thread thread = new Thread()
        {
            public void run()
            {
                // メッセージ構築中とする
                buildingMessage = true;

                // データ表示準備を実行する
                messageParser.prepareMessage();

                // スレタイトルを決める...
                setTitle(messageParser.decideThreadTitle());

                // メッセージ構築中を解除する
                buildingMessage = false;

                // スレを表示する
                showMessageMain();
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
        thread = null;
        title = null;
        System.gc();
        return;
    }

    /**
     *  スレ内検索の実行
     *
     */
    private void setSearchKeyword()
    {
        String defaultKeyword = "";

        int startPos = parsedString.indexOf("ID:");
        if (startPos >= 0)
        {
            int endPos = parsedString.indexOf("\r", startPos);
            if (endPos >= 0)
            {
                defaultKeyword = parsedString.substring(startPos, endPos);
            }
        }        
        parent.OpenWriteFormUsingWordInputMode(parent.SCENE_MESSAGE_SEARCH, "スレ内検索", "キーワード", defaultKeyword, true);
        return;
    }

    // スレ情報の表示...
    private void showMessageInformation()
    {    
        String  data = "";
        data = stringConverter.parsefromSJ(messageParser.getThreadTitle());
        data = data + "\n" + messageParser.getFileName();
        data = data + "\n" + (messageParser.getFileSize() / 1000) + "kB (" + messageParser.getFileSize() + ")\n";
        data = data + parent.getHeapInfo();
        parent.showDialog(parent.SHOW_INFO, 0, "スレ情報", data);
        return;
    }

    /**
     *  メモファイルへURL一覧を出力する...
     * 
     *
     */
    private void outputUrlToMemoTxt()
    {
        String  data = "";
        String  urlList = "";
        for (int index = 0; index < commandList.size(); index++)
        {
            String urlToOpen = (String) commandList.elementAt(index);
            urlList = urlList + urlToOpen + "\r\n";
        }
        data = stringConverter.parsefromSJ(messageParser.getThreadTitle());
        parent.outputMemoTextFile(data, urlList);

        // ガベコレ実施...
        commandList = null;
        System.gc();
        return;
    }

    /**
     *  レスのコピー
     *  
     */
    private void messageEditText(boolean isEditMode)
    {
        // レス編集モードで表示する
        if (isEditMode == true)
        {
            String  data = "";
            data = stringConverter.parsefromSJ(messageParser.getThreadTitle());
            parent.OpenWriteFormUsingTextEditMode(data, parsedString);
            return;
        }

        // レスのファイル出力を実施する
        Thread thread = new Thread()
        {
            public void run()
            {
                buildingMessage = true;
                String  data = "";
                data = stringConverter.parsefromSJ(messageParser.getThreadTitle());
                parent.outputMemoTextFile(data, parsedString);
                buildingMessage = false;
            }
        };
        try
        {
            // レスのファイル出力が終わるまで待つ...
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            //
        }
        return;
    }

    /**
     * レス内のジャンプリストを抽出する
     * 
     */
    private void pickUpJumpList()
    {
        // 文字列解析データがなければ終了する
        if (parsedString == null)
        {
            return;
        }

        // レス番号を解析する
        int index      = 0;
        int limit      = parsedString.length();
        int maxMesNum  = messageParser.getNumberOfMessages();
        buildNofPickupList = 0;
        
        // まず、名前欄に数字が記述されているか確認する
        try
        {
            int nameNum = Integer.parseInt(messageParser.getNameField());
            if ((nameNum > 0)&&(nameNum <= maxMesNum))
            {
                // 数字が記載されていた！ 追加する
                pickupMessageList[buildNofPickupList] = nameNum;
                buildNofPickupList++;
            }
        }
        catch (Exception e)
        {
            // 名前欄は数字ではなかった、何もしない
        }

        // レス内に数字があるかチェックする
        while (index < limit)
        {
            int startNumber = parsedString.indexOf(">", index);
            if (startNumber < index)
            {
                // 解析終了...抜ける
                break;
            }
            startNumber++;
            int pickupNumber = 0;
            char checkChar = parsedString.charAt(startNumber);
            while (checkChar == '>')
            {
                startNumber++;
                checkChar = parsedString.charAt(startNumber);
            }
            while ((checkChar >= '0')&&(checkChar <= '9'))
            {
                int addCount = checkChar - '0';
                pickupNumber = pickupNumber * 10 + addCount;
                startNumber++;
                checkChar = parsedString.charAt(startNumber);
            }
            if ((pickupNumber > 0)&&(buildNofPickupList < nofPickupMessageList)&&(pickupNumber <= maxMesNum))
            {
                // 抽出した数値を入れる
                pickupMessageList[buildNofPickupList] = pickupNumber;
                buildNofPickupList++;
            }
            index = startNumber;
        }

        // 末尾に「一覧へ戻る」を追加する
        pickupMessageList[buildNofPickupList] = -1;            
        buildNofPickupList++;

        return;        
    }
    
    // NGキーワードを登録する
    public void setNGword(String dataString)
    {
        ngWordManager.entryNgWord(dataString, ngWordManager.CHECKING_ALL);
        backupNGword();
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

    // NGキーワードをバックアップする
    public void backupNGword()
    {
        // 表示ルーチンを呼び出す...
        Thread thread = new Thread()
        {
            public void run()
            {
                // NGワードをバックアップする
                ngWordManager.backup();
            }
        };
        thread.start();
        return;
    }
    
    // NGキーワードが含まれているかチェックする
    private boolean checkNgKeyword(String checkString)
    {
        int count = ngWordManager.numberOfNgWords();
        for (int index = 0; index < count; index++)
        {
            String word   = ngWordManager.getNgWord(index);
//            int checkType = ngWordManager.getNgCheckingType(index);
            if (word != null)
            {
                //parent.showDialog(parent.SHOW_INFO, 0, "<< NGワードチェック >> " , "word:" + word + "[" + index + "/" + count + "]" + "\n\n" + parsedString);
                // レス全体にNGワードが含まれているか
                if (checkString.indexOf(word, 0) >= 0)
                {
                    // NGワードが含まれていることを検出した！！
                    return (true);
                }
            }
        }
        return (false);
    }

    
    /**
     *  URLを抽出する
     * @param data
     * @return
     */
    private void pickupUrl(String data)
    {
        // WebBrowserに渡すURLをレスから抽出し、選択肢とする...
        int index    = 0;
        int limit    = data.length();
        while (index < limit)
        {
            int startUrl = data.indexOf("tp://", index);
            if (startUrl < index)
            {
                // 解析終了...抜ける
                break;
            }
            // 抽出URLの末尾を解析する
            int endUrl = startUrl;
            char checkChar = data.charAt(endUrl);
            // てきとーに半角文字のみ抽出する... ('"' よりも大きく、'~' 以下の文字コードを持つ文字列)
            while ((checkChar > ' ')&&(checkChar != '"')&&(checkChar != '>')&&((checkChar <= 0x007e)))
            //    while ((checkChar > ' ')&&(checkChar != '>'))
            {
                endUrl++;
                checkChar = data.charAt(endUrl);
            }
            String url = new String("ht" + data.substring(startUrl, endUrl));
            commandList.addElement(url);
            url   = null;
            index = endUrl;
        }
        return;
    }
    
    /**
     *  URLを抽出する ("www." からはじまる文字列を抽出する)
     * @param data
     * @return
     */
    private void pickupUrlBulkString(String data)
    {
        // WebBrowserに渡すURLをレスから抽出し、選択肢とする...
        int index    = 0;
        int limit    = data.length();
        while (index < limit)
        {
            int startUrl = data.indexOf("www.", index);
            if (startUrl < index)
            {
                // 解析終了...抜ける
                break;
            }
            int pickUrl = data.indexOf("tp://www.", index);
            if ((pickUrl + 5) == startUrl)
            {
                // 検出対象外としてinexを先に進める
                index = startUrl + 1;
                continue;
            }
            // 抽出URLの末尾を解析する
            int endUrl = startUrl;
            char checkChar = data.charAt(endUrl);
            // てきとーに半角文字のみ抽出する... ('"' よりも大きく、'~' 以下の文字コードを持つ文字列)
            while ((checkChar > ' ')&&(checkChar != '"')&&(checkChar != '>')&&((checkChar <= 0x007e)))
            {
                endUrl++;
                checkChar = data.charAt(endUrl);
            }
            String url = new String("http://" + data.substring(startUrl, endUrl));
            int chkPos = url.indexOf(".");
            if (chkPos > 0)
            {
                int chkPos2 = url.indexOf(".", (chkPos + 2));
                if (chkPos2 >= 0)
                {
                    // . が 2つ以上あったときだけ、URLとして認識する。
                    commandList.addElement(url);                    
                }
            }
            url   = null;
            index = endUrl;
        }
        return;
    }
    
    /**
     *   Webブラウザの起動準備
     *
     */
    private boolean launchWebBrowser()
    {
        // 文字列解析データがなければ終了する
        if (parsedString == null)
        {
            return (true);
        }

        // 抽出したURLを格納する文字列を初期化
        commandList = null;
        commandList = new Vector();

        pickupUrl(parsedString);
        pickupUrlBulkString(parsedString);
        if (commandList.size() != 0)
        {
            // レス内にURLが１件以上あった場合には、選択肢を開く
            parent.openSelectForm("ブラウザで開く", commandList, 0, parent.CONFIRM_URL_FOR_BROWSER);
        }
        else
        {
            // レス内にURLが１件もなかった、、、何もせずに終了する
            commandList = null;
            System.gc();
            return (true);
        }
        return (true);
    }

    /**
     *  スレ番号指定取得機能の実行準備
     *  
     */
    private void pickupThreadNumber()
    {
        // 文字列解析データがなければ終了する
        if (parsedString == null)
        {
            return;
        }

        // レスから、'/read.cgi/xxxx/yyyyyyyy' の xxxx/yyyyyyyy 部分を抽出する
        commandList = null;
        commandList = new Vector();
        int index     = 0;
        int limit     = parsedString.length();
        int nofThread = 0;
        while (index < limit)
        {
            int startUrl = parsedString.indexOf("/read.cgi/", index);
            if (startUrl < index)
            {
                // 解析終了...抜ける
                break;
            }
            // '/read.cgi/' を読み飛ばす
            startUrl = startUrl + 10;
            
            // スレ番号の末尾を解析する
            boolean firstSeparator = false;
            int endUrl = startUrl;
            char checkChar = parsedString.charAt(endUrl);
            while ((checkChar > ' ')&&(checkChar != '>')&&(checkChar != 0))
            {
                if (checkChar == '/')
                {
                    if (firstSeparator == true)
                    {
                        break;
                    }
                    firstSeparator = true;
                }
                endUrl++;
                checkChar = parsedString.charAt(endUrl);
            }
            String url = new String(parsedString.substring(startUrl, endUrl));
            commandList.addElement(url);
            nofThread++;
            url   = null;
            index = endUrl;
        }
        if (nofThread != 0)
        {
            // レス内にスレ番号が１件以上あった場合には、選択肢を開く
            parent.openSelectForm("スレ番号指定取得", commandList, 0, parent.CONFIRM_GET_THREADNUMBER);
        }
        else
        {
            // レス内にURLが１件もなかった、、、何もせずに終了する
            commandList = null;
            System.gc();
        }
        return;
    }

    /**
     *  レス出力ファイル名の変更。。。
     * 
     *
     */
    private void changeOutputFileName()
    {
        // 表示ルーチンを呼び出す...
        Thread thread = new Thread()
        {
            public void run()
            {
                // 現在のファイル名を取得する
                String fileName = parent.getOutputMemoFileName();
            
                // ファイル名変更用画面を開く
                parent.OpenWriteFormUsingWordInputMode(parent.SCENE_MEMOFILE_INPUT, "メモファイル名変更", "ファイル名", fileName, false);
            }
        };
        thread.start();
        return;
    }
    
    /**
     *  コマンドの実行...
     * 
     * 
     */
    public void executeCommandAction(byte scene, int index, int appendInfo)
    {
        if (scene == parent.CONFIRM_URL_FOR_BROWSER)
        {
            if (appendInfo == 2)
            {
                // メモ帳へURLを出力する
                outputUrlToMemoTxt();

                informationColor = 0x0000f0f0;
                drawInformation = true;
                repaint();
                return;
            }

            // 選択肢をコピーして、ブラウザでオープンさせる
            String urlToOpen = (String) commandList.elementAt(index);
            commandList = null;
            if (urlToOpen.indexOf("http://") < 0)
            {
                // HTTPではなかった。。。
                commandList = null;
                System.gc();
            }

            if ((appendInfo == 3)||(appendInfo == 4))
            {
                // リンク先を取得する...
                if (appendInfo == 4)
                {
                    currentGwtPageNum = -1;
                    useCacheDirectory = true;
                    previewURL = decideUrlForPreview(urlToOpen);
                }
                else
                {
                    useCacheDirectory = false;
                    previewURL = urlToOpen;
                }
                Thread thread = new Thread()
                {
                    public void run()
                    {
                        // HTTP通信でファイルを取得する
                        getFileUsingHttp(previewURL, useCacheDirectory);
                    }
                };
                try
                {
                    thread.start();
                }
                catch (Exception ex)
                {
                    setBusyMessage("EX:" + ex.getMessage(), false);
                    repaint();
                }
                System.gc();
                return;
            }

            // ブラウザで開く...
            parent.openWebBrowser(urlToOpen);
            System.gc();

            informationColor = 0x0000f0f0;
            drawInformation = true;
            repaint();
            return;
        }
        else if (scene == parent.CONFIRM_GET_THREADNUMBER)
        {
            // スレ番号指定取得処理...
            reservedThreadNumber = (String) commandList.elementAt(index);


            
            // 画面を抜ける...
            if (appendInfo == 1)
            {
                finishShowMessage();
            }
            else
            {
                // 末尾の値を取得する
                int bottom = messageParser.getNumberOfMessages();
                
                // 現在の未読位置を記録する
                parent.setCurrentMessage(messageNumber, bottom);
            }
            
            // 入力されたスレ番号の解析を依頼する
            parent.parseMessageThreadNumber(reservedThreadNumber, appendInfo);
            reservedThreadNumber = null;
        }
        else if (scene == parent.CONFIRM_SELECT_OSUSUME2CH)
        {
            // おすすめ２ちゃんねるの表示...
            String buffer = (String) commandList.elementAt(index);
            int pos = buffer.indexOf("<>/");
            if (pos >= 0)
            {
                // スレ番号部分を切り出す..
                pos = pos + 3;
                reservedThreadNumber = (String) buffer.substring(pos);

                // 画面を抜ける...
                if (appendInfo == 1)
                {
                    finishShowMessage();
                }
                // 入力されたスレ番号の解析を依頼する
                parent.parseMessageThreadNumber(reservedThreadNumber, appendInfo);
                reservedThreadNumber = null;
            }
        }
        commandList = null;
        System.gc();
        return;
    }
    

    /**
     *   現在参照中のスレをお気に入りスレとして登録する
     *
     *
     */
    private void setFavoriteThread()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.setFavoriteThread(currentMessageIndex);
                informationColor = 0x000000f0;
                drawInformation = true;
                repaint();
            }
        };
        thread.start();
        return;
    }
    
    /**
     * 検索モードの実施
     * 
     * @param begin
     */
    private void executeSearchKeyword(int direction)
    {
        // 検索実行中のときには何もしない...
        if (isSearchExecution == true)
        {
            return;
        }

        // 検索方向を設定して、検索する
        searchDirection = direction;
        isCommandMode = false;
        Thread thread = new Thread()
        {
            public void run()
            {
                executeSearchKeywordMain();
            }
        };
        thread.start();
        
        return;
    }

    /**
     * 検索モードの実施(メイン処理)
     * 
     * @param begin
     */
    private void executeSearchKeywordMain()
    {
        int bottom;  // 検索終了の場所を指定する...
        isSearchExecution = true;

        // とりあえず、画面更新..
        repaint();
        
        if (searchDirection > 0)
        {
            // 正方向の検索
            bottom = messageParser.getNumberOfMessages() + 1;
        }
        else
        {
            // 負方向の検索
            bottom = 0;
        }

        // 画面描画終了まで待つ...
        while ((painting == true)||(buildingMessage == true))
        {
            parent.sleepTime(1);
        }
        System.gc();
        
        // 検索を実行する...
        for (int resNum = (messageNumber + searchDirection); resNum != bottom; resNum = resNum + searchDirection)
        {
            if (existKeyword(resNum) == true)
            {
                while ((painting == true)||(buildingMessage == true))
                {
                    parent.sleepTime(1);
                }
                System.gc();
                currentPage = 0;
                showMessage(resNum);
                System.gc();
                isSearchExecution = false;
                return;
            }
        }
        // 検索文字列が見つからなかった...
        String  data = "文字列が見つかりません\n文字列：" + searchMessageString;
        parent.showDialog(parent.SHOW_INFO, 0, "検索終了", data);
        showMessage(messageNumber);
        System.gc();
        isSearchExecution = false;
        return;
    }

    /**
     *  BE profile参照
     * 
     *
     */
    private void viewBeProfile()
    {
        // BEのIDがレス内にあるかどうかチェックする
        int pos = parsedString.indexOf("BE:");
        int endPos = parsedString.indexOf("-", pos + 3);
        if ((pos >= 0)&&(endPos >= 0))
        {
            String id = parsedString.substring((pos + 3), (endPos));
            previewURL = "http://be.2ch.net/test/p.php?i=" + id;
            
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(null, previewURL, null, "Be Profile", 0, 0, parent.SCENE_PREVIEW_MESSAGE_EUC, false);
                }
            };
            thread.start();
        }
        
        return;
    }

    /**
     *  おすすめ２ちゃんねるの表示
     */
    private void viewOsusume2ch()
    {
        String url = parent.getCurrentMessageBoardUrl() + "i/" + parent.getThreadFileName(currentMessageIndex);
        previewURL = url.substring(0, (url.length() - 3)) + "html";
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.GetHttpCommunication(null, previewURL, null, "おすすめ2ch", 0, 0, parent.SCENE_GET_OSUSUME_2CH, false);
                parent.unlockCommunicationScreen();   // BUSYスクリーンのロックをはずす
            }
        };
        // BUSYスクリーンのロックをつける
        parent.lockCommunicationScreen();
        parent.showBusyMessage("", "おすすめ2chチェック中...", -1);
        thread.start();
    }

    /**
     *   おすすめ２ちゃんねるのデータパース
     *
     */
    public boolean parseOsusume2ch(byte[] getString)
    {
        // BUSYスクリーンのロックをはずす
        parent.lockCommunicationScreen();

        // 文字列解析データがなければ終了する
        if (getString == null)
        {
            return (true);
        }
        String dataBuffer = new String(getString);
        
        // WebBrowserに渡すURLをレスから抽出し、選択肢とする...
        commandList = null;
        commandList = new Vector();
        int index    = 0;
        int limit    = dataBuffer.length();
        int urlCount = 0;
        while (index < limit)
        {
            int startUrl = dataBuffer.indexOf("read.cgi/", index);
            if (startUrl < 0)
            {
                break;
            }
            startUrl = startUrl + 9;
            if (startUrl < index)
            {
                // 解析終了...抜ける
                break;
            }
            // 抽出URLの末尾を解析する
            boolean firstSlash = false;
            int endUrl = startUrl;
            char checkChar = dataBuffer.charAt(endUrl);
            while (checkChar != '"')
            {
                endUrl++;
                checkChar = dataBuffer.charAt(endUrl);
                if (checkChar == '/')
                {
                    if (firstSlash == true)
                    {
                        break;
                    }
                    firstSlash = true;
                }
            }
            int titleStartPos = dataBuffer.indexOf(">", endUrl);
            int titleEndPos   = dataBuffer.indexOf("<", endUrl);
            String title = dataBuffer.substring((titleStartPos + 1), titleEndPos);
            String url = new String(title + " <>/" + dataBuffer.substring(startUrl, endUrl));
            commandList.addElement(url);
            urlCount++;
            url   = null;
            index = endUrl;
        }
        if (urlCount != 0)
        {
            // レス内にURLが１件以上あった場合には、選択肢を開く
            parent.openSelectForm("おすすめ２ちゃんねる", commandList, 0, parent.CONFIRM_SELECT_OSUSUME2CH);
        }
        else
        {
            // レス内にURLが１件もなかった、、、何もせずに終了する
            commandList = null;
            busyMessage ="おすすめ2ch:みつかりません。";
            drawInformation = true;
            repaint();
            System.gc();
            return (true);
        }
        return (true);
    }
    
    /**
     * 文字列が含まれているかどうか確認する
     * 
     * @param messageNumber
     * @return
     */
    private boolean existKeyword(int messageNumber)
    {
        // レスデータを取得する
        String message = stringConverter.parsefromSJ(messageParser.getMessage(messageNumber, parent.getViewKanjiMode()));
        if (searchIgnoreCase != true)
        {
            // 大文字/小文字を区別する場合...
            int pos = message.indexOf(searchMessageString);
            message = null;
            if (pos < 0)
            {
                return (false);
            }
            return (true);
        }

        // 大文字/小文字を区別しない場合...
        String target     = message.toLowerCase();
        String matchData  = searchMessageString.toLowerCase();
        if (target.indexOf(matchData) >= 0)
        {
            return (true);
        }
        return (false);
    }

    /**
     *   URLからファイル名を取得する
     * 
     * @param url URL
     * @return    ファイル名
     */
    private String decideFileNameUsingUrl(String url)
    {
        String fileName = "index.html";
        boolean modifyFileName = false;
        int pos = url.lastIndexOf('/');
        if (url.length() > (pos + 1))
        {
            // ファイル名の調整を行う...
            fileName = url.substring(pos + 1);
            pos = fileName.lastIndexOf('?');
            if (pos >= 0)
            {
                fileName = fileName.substring(pos + 1);
                modifyFileName = true;
            }
            pos = fileName.lastIndexOf('*');
            if (pos >= 0)
            {
                fileName = fileName.substring(pos + 1);
                modifyFileName = true;
            }
            pos = fileName.lastIndexOf('%');
            if (pos >= 0)
            {
                fileName = fileName.substring(pos + 1);
                modifyFileName = true;
            }
            if (modifyFileName == true)
            {
                // ファイル名の補正をした場合には、末尾にhtmlをつける...
                fileName = fileName + ".html";
            }
        }
        return (fileName);
    }
    
    /**
     *  プレビューで開くURLを調整する
     * 
     * @param urlToOpen
     * @return
     */
    private String decideUrlForPreview(String urlToOpen)
    {
    	String url = urlToOpen;
        if (urlToOpen.indexOf("imepita.jp") >=  0)
        {
        	// イメぴたの場合は、そのまま応答する
            return (url);
        }
        if (urlToOpen.indexOf("pita.st") >=  0)
        {
        	// ＠ぴたの場合は、そのまま応答する
            return (url);
        }
        if (((urlToOpen.indexOf(".jpg") > 0)||(urlToOpen.indexOf(".JPG") > 0))||
             ((urlToOpen.indexOf(".gif") > 0)||(urlToOpen.indexOf(".GIF") > 0))||
             ((urlToOpen.indexOf(".png") > 0)||(urlToOpen.indexOf(".PNG") > 0)))
        {
            // 画像ファイルの場合には、 tinysrc 経由でイメージファイルを取得することにする
        	//  (http://i.tinysrc.mobi/横幅/縦幅/オリジナルのURL)
            return ("http://i.tinysrc.mobi/240/320/" + url);
        }
        if (urlToOpen.indexOf("http://") == 0)
        {
            url = "http://www.google.co.jp/gwt/n?u=http%3A%2F%2F" + urlToOpen.substring(7);
        }
        return (url);
    }
    
    /**
     * HTTP通信でファイルを取得する
     * 
     * @param urlToGet  取得するURL
     */
    private void getFileUsingHttp(String url, boolean useCacheDir)
    {
        // URLの抽出に成功したときだけhttp通信を実行する...
        setBusyMessage("通信中：" + url, false);

        // URLからファイル名を抽出する
        String fileName = decideFileNameUsingUrl(url);
        
        // bbstable.html を上書きするような処理は禁止する。。。
        if ((parent.getSaveAtRootDirectory() == true)&&(fileName.indexOf("bbstable.html") >= 0))
        {
            setBusyMessage(null, false);
            parent.vibrate();
            System.gc();
            return;
        }

        // gwtのページ補正...
        if (currentGwtPageNum > 0)
        {
            url = url + "&_gwt_pg=" + currentGwtPageNum;
        }

        String directory = parent.getBaseDirectory();
        if (useCacheDir == true)
        {
            // キャッシュディレクトリにファイルを保管する
            directory = directory + "a2B_Cache";
            parent.makeDirectory(directory);
            directory = directory + "/";
            
            if ((url.indexOf("imepita.jp") > 0)||(url.indexOf("pita.st") > 0))
            {
                // いめピタ あるいは ＠ぴた の場合には、拡張子(jpeg)を追加する
                fileName = fileName + ".jpg";
            }
            if (((fileName.indexOf(".jpg") > 0)||(fileName.indexOf(".JPG") > 0))||
                 ((fileName.indexOf(".gif") > 0)||(fileName.indexOf(".GIF") > 0))||
                 ((fileName.indexOf(".png") > 0)||(fileName.indexOf(".PNG") > 0)))  
            {
                //  取得するファイルの拡張子が .jpg または .png または gif の場合、
                // または imepita.jp の場合、
                // Google Mobile Proxy経由で縮小した画像を取得して表示する
                getCachePicturesUsingHttp(directory, fileName, url);
                showPreviewPictures(directory, fileName);
            }
            else
            {
                // ファイルをキャッシュし、プレビューする
                getCacheFileUsingHttp(directory, fileName, url);
            }
            finishGetFile();
            return;
        }
 
        if (parent.getSaveAtRootDirectory() != true)
        {
            directory = parent.getFileSaveDirectory();
            parent.makeDirectory(directory);
            directory = directory + "/";
        }

        int rc = parent.doHttpMain(url, (directory + fileName), null, parent.getUseCachePictureFile(),  a2BMain.a2B_HTTPMODE_NOUSECOOKIE);
        if (rc < 0)
        {
            // 通信に失敗した場合...でも、WX310のときには、これが返ってくる...
            informationColor = 0x0000f0f0;
            crossColor       = 0x00010101;
            drawInformation = true;
            repaint();
            System.gc();
        }
        setBusyMessage("http通信終了 : " + fileName, false);
        if (((fileName.indexOf(".jpg") > 0)||(fileName.indexOf(".JPG") > 0))||
            ((fileName.indexOf(".gif") > 0)||(fileName.indexOf(".GIF") > 0))||
            ((fileName.indexOf(".png") > 0)||(fileName.indexOf(".PNG") > 0)))
        {
            // 拡張子 .jpg または .png または gif の場合には、サムネール部分を表示する...
            showPreviewPictures(directory, fileName);
        }
        finishGetFile();
        return;
    }

    /**
     *  画像ファイルのプレビューを実施する
     * 
     * @param directory ディレクトリ名
     * @param fileName  ファイル名
     */
    private void showPreviewPictures(String directory, String fileName)
    {
        // 拡張子 .jpg または .png または gif の場合には、サムネール部分を表示する...
        jpegPreviewImage = parent.loadJpegThumbnailImageFromFile(directory + fileName);
        if (jpegPreviewImage == null)
        {
            // JPEGファイル表示失敗...
            setBusyMessage("表示失敗(0:削除) : " + fileName, false);
        }
        else
        {
            // JPEGファイルの表示を行う...
            setBusyMessage("0:キャッシュ削除 (" + fileName + ")", false);
        }
        previewJpeg = true;
        displayFileName = directory + fileName;
        return;        
    }
    
    /**
     *  ファイル取得の後処理...
     *
     */
    private void finishGetFile()
    {
        parent.vibrate();
        informationColor = 0x0000f0f0;
        drawInformation = true;
        repaint();
        System.gc();
        return;
    }

    /**
     *  キャッシュファイルの取得 (画像表示用)
     * 
     * @param directory
     * @param fileName
     * @param url
     */
    private void getCachePicturesUsingHttp(String directory, String fileName, String url)
    {
        if (parent.getUseCachePictureFile() == true)
        {
            if (a2BfileSelector.IsExistFile(directory + fileName) == true)
            {
                // 既にファイルが存在した場合、何もしない (キャッシュファイルを利用する)
                return;
            }
        }

        String referer = null;
        int rc = parent.doHttpMain(url, (directory + fileName), null, false,  a2BMain.a2B_HTTPMODE_PICKUPCOOKIE);
        String getFileUrl = pickupPreviewPictureFileUrl(directory + fileName);   
        if (url.indexOf("imepita.jp") >= 0)
        {
        	// 無理やり いめピタ対応...
            getFileUrl = "http://imepita.jp/image/" + url.substring(18);
            referer = url;
        }
        if (getFileUrl == null)
        {
            // File名の抽出に失敗した場合... もう一回試す。
            rc = parent.doHttpMain(url, (directory + fileName), referer, false,  a2BMain.a2B_HTTPMODE_USEPREVIOUSCOOKIE);
            getFileUrl = pickupPreviewPictureFileUrl(directory + fileName);
            if (getFileUrl == null)
            {
                setBusyMessage("FNF:" + fileName, false);
                return;
            }
        }

        rc = parent.doHttpMain(getFileUrl, (directory + fileName), referer, false,  a2BMain.a2B_HTTPMODE_USEPREVIOUSCOOKIE);
        if (rc < 0)
        {
            // 通信に失敗した場合... (でも、WX310のときには、これが返ってくる...)
            informationColor = 0x0000f0f0;
            crossColor       = 0x00010101;
            drawInformation = true;
            repaint();
        }
        setBusyMessage("http通信終了 : " + fileName, false);
        parent.clearHttpCacheCookie();
        System.gc();
        return;
    }
    
    
    /**
     *  プレビューする画像ファイルのURLを取得する
     * @param fileName チェックするファイル名
     * @return 抽出した画像ファイルのURL
     */
    private String pickupPreviewPictureFileUrl(String fileName)
    {
    	String returnFileUrl = null;
        FileConnection connection    = null;
        InputStream    in            = null;
        byte[]         buffer        = null;
        String          checkBuffer  = null;
        int            dataReadSize  = 0;

        try
        {
            connection   = (FileConnection) Connector.open(fileName, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  データを最初から size分読み出す...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + 4];
            dataReadSize = in.read(buffer, 0, size);
            checkBuffer = new String(buffer, 0, dataReadSize);
            in.close();
            connection.close();

            // 読み出しに使った変数をクリアする
            in = null;
            connection = null;
            buffer = null;

            int pos0 = checkBuffer.indexOf("<img");
            int pos1 = -1;
            int pos2 = -1;
            if (pos0 >= 0)
            {
                pos1 = checkBuffer.indexOf("src=\"", pos0);
            }
            if (pos1 >= 0)
            {
                pos2 = checkBuffer.indexOf("\"", (pos1 + 6));
            }
            if ((pos2 >= 0)&&(pos1 >= 0)&&(pos1 < pos2))
            {
            	if (checkBuffer.indexOf("pita.st") > 0)
            	{
            		// ＠ぴたの場合...
            		returnFileUrl = "http://i.tinysrc.mobi/240/320/" + checkBuffer.substring((pos1 + 5), (pos2));
            	}
            	else
            	{
            		// それ以外の場合
                    returnFileUrl = "http://www.google.co.jp" + checkBuffer.substring((pos1 + 5), (pos2));
            	}
            }
        }
        catch (Exception ex)
        {
            // exception...
            in = null;
            connection = null;
            buffer = null;
            returnFileUrl = null;
        }
        checkBuffer = null;
        return (returnFileUrl);
    }

    /**
     *  キャッシュファイルの取得 (テキストプレビュー用)
     * 
     * @param directory
     * @param fileName
     * @param url
     */
    private void getCacheFileUsingHttp(String directory, String fileName, String url)
    {
        // HTTP通信でデータを取得する
        int rc = parent.doHttpMain(url, (directory + fileName), null, parent.getUseCachePictureFile(),  a2BMain.a2B_HTTPMODE_NOUSECOOKIE);
        if (rc < 0)
        {
            // 通信に失敗した場合... (でも、WX310のときには、これが返ってくる...)
            informationColor = 0x0000f0f0;
            crossColor       = 0x00010101;
            drawInformation = true;
            repaint();
            System.gc();
        }
        setBusyMessage("http通信終了 : " + fileName, false);

        // キャッシュの場合、ファイルのプレビューを仕掛けてみる..
        previewText = true;
        displayFileName = directory + fileName;
        int mode = a2BsubjectDataParser.PARSE_UTF8;
        if (parent.getWX220JUserAgent() != true)
        {
            mode = a2BsubjectDataParser.PARSE_SJIS;
        }
        parent.doPreviewFile(displayFileName, true, mode);
        setBusyMessage("0:File削除 (" + fileName + ")", false);
        
        return;
    }

    /**
     *  キーリピート時間の設定(単位：ms)
     * @param ms キーリピート時間
     */
    public void setKeyRepeatDelay(int ms)
    {
    	keyRepeatDelayMs = (long) ms;
    }

    /**
     *  キーリピート時間の取得(単位:ms)
     * @return キーリピート時間
     */
    public int getKeyRepeatDelay()
    {
    	return ((int) keyRepeatDelayMs);
    }

    /**
     * 検索文字列入力が完了した場合...
     * 
     * @param keyWord
     */
    public void setSearchString(String keyWord, byte searchStatus, boolean ignoreCase)
    {
        searchIgnoreCase = ignoreCase;
        currentPage = 0;
        if ((keyWord == null)||(keyWord.compareTo("") == 0))
        {
            // 検索モードを解除する
            searchMessageString = null;
            isSearchingMode = false;
            showMessage(messageNumber);
            return;
        }

        // 検索文字列を追加する
        searchMessageString = keyWord;
        isSearchingMode = true;
        isCommandMode   = false;

        // 戻り先場所に追加
        if (jumpListIndex < nofJumpList)
        {
            messageJumpList[jumpListIndex] = messageNumber;
            jumpListIndex++;
        }
        else
        {
            messageJumpList[nofJumpList - 1] = messageNumber;            
        }
        showMessage(messageNumber);
        executeSearchKeyword(1);        
        return;
    }

    /**
     *   スレ番号指定取得の登録が完了したことを通知する
     *
     */
    public void completedParseMessageThreadNumber(boolean result)
    {
        triangleColor = 0x00f000f0;
        if (result == false)
        {
            triangleColor = 0;
            crossColor = 0x00f000f0;
//          busyMessage ="番号指定取得失敗";
        }
        else
        {
//          busyMessage ="番号指定取得成功";
        }
        drawInformation = true;
        repaint();
        return;
    }

    /**
     *   レスのファイル出力が完了したことを通知する
     *
     */
    public void completedOutputMessage(boolean result)
    {
        triangleColor = 0x00f0f018;
        if (result == false)
        {
            triangleColor = 0;
            crossColor = 0x00f000f0;
            busyMessage ="出力失敗...";
        }
        else
        {
            busyMessage ="出力終了";
        }
        drawInformation = true;
        repaint();
        return;
    }

    /**
     *   ヘルプイメージを表示する
     *
     */
    private void changeHelpShowMode()
    {
        if (viewHelpImage == null)
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
     *  壁紙・操作ヘルプのイメージを読み込む
     * 
     * 
     */
    public void loadWallPaperImage()
    {
        wallPaperImage = parent.loadImageFromFile("a2B_WallPaper.png");
        viewHelpImage  = parent.loadImageFromFile("a2B_ViewHelp.png");
        return;
    }
    
}
//--------------------------------------------------------------------------
//  a2BMessageViewForm  --- スレメッセージ表示フォーム (レスの表示フォーム)
//
//
//--------------------------------------------------------------------------
// MRSa (mrsa@sourceforge.jp)
