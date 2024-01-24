import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import java.util.Vector;

/**
 * 選択肢の表示・選択 (BUSYメッセージの表示も兼用)
 * 
 * @author MRSa
 *
 */
//public class a2BselectionForm  extends Form implements ItemCommandListener, CommandListener
//public class a2BselectionForm  extends List implements CommandListener
public class a2BselectionForm extends Canvas implements CommandListener, Ia2BMidpTimerTimeout
{
	private final int blinkDuration   = 500;     // 500msごとに点滅
    private final int backColorOdd    = 0x00f8f8f8;
    private final int backColorEven   = 0x00b0c8c8;
    private final int foreColor       = 0x00000000;
    private final int cursorBackColor = 0x000000f8;
    private final int cursorForeColor = 0x00ffffff;
    private final int defaultTitleForeColor = 0x00f8f8f8;
    private final int defaultTitleBackColor = 0;

    static private final int  COLOR_A = 0x000000f0;
    static private final int  COLOR_B = 0x00c0c000;
    static private final int  COLOR_C = 0x00101010;
    static private final int  COLOR_D = 0x00f0f0f0;
    
    static public final int SELECTION_FORM    = 0;
    static public final int BUSY_FORM         = 1;
    static public final int CONFIRMATION_FORM = 2;
    static public final int INFORMATION_FORM  = 3;
    static public final int BUSY_FORM_UPDATE  = 4;
    
    private a2BMain parent             = null;
    private a2BMidpTimer watchDogTimer = null;

    private int     currentScene      = -1;
    private int     screenKind        = SELECTION_FORM;
    private int     numSelection      = -1;

    private boolean painting         = false;
    private boolean redraw           = true;
    private boolean isBlinking       = false;

    private Font      screenFont      = null;
    private int       screenHeight    = 0;
    private int       screenLines     = 1;

    private int       currentPage     = 0;
    private int       nofPage         = 0;
    private int       selectedLine    = 0;    
    private int       timerTemp       = 0;
    
    private Vector    selectionData   = null;
    private String    titleString     = null;
    private String    busyMessage     = null;
    
    private int      titleBackColor = defaultTitleBackColor;
    private int      titleForeColor = defaultTitleForeColor;
    
    private Command cancelCmd  = new Command("Cancel",          Command.EXIT,   -1);
    private Command selectCmd  = new Command("選択",            Command.SCREEN,  0);
    private Command confirmCmd = new Command("OK",              Command.OK,      1);
    private Command getMoveCmd = new Command("移動+取得(開く)", Command.SCREEN,  2);
    private Command outputCmd  = new Command("Memo出力",        Command.SCREEN,  3);
    private Command getHttpCmd = new Command("File取得",        Command.SCREEN,  4);
    private Command getPicsCmd = new Command("プレビュー",      Command.SCREEN,  5);

    
    /**
     * コンストラクタ
     * 
     * @param object
     */
    public a2BselectionForm(a2BMain object)
    {
        // タイトルを消す...
        setTitle(null);

        // フルスクリーンにする...
        setFullScreenMode(true);

        // コンストラクタ
        parent = object;

        watchDogTimer = new a2BMidpTimer(this);

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
        try
        {
            screenFont = Font.getFont(font.getFace(), font.getStyle(), parent.getListFontSize());
        }
        catch (Exception ex)
        {
        	screenFont = font;
        }

        screenHeight = getHeight();
        screenLines  = (screenHeight / (screenFont.getHeight() + 1)) - 1;

//        repaint();
        return;
    }

    /**
     *   画面描画の実施...
     * 
     * 
     */
    public void paint(Graphics g)
    {
        if (painting == true)
        {
            // 描画中、何もしない
            return;
        }
        painting = true;
        try
        {
            // 画面の塗りつぶし
            if (redraw == true)
            {
                g.setColor(0x00f8f8f8);
                g.fillRect(0, 0, getWidth(), getHeight());
            }
            g.setFont(screenFont);

            // タイトルの表示...
            if (titleString != null)
            {
                g.setColor(titleBackColor);
                g.fillRect(0, 0, getWidth(), screenFont.getHeight() + 1);
                g.setColor(titleForeColor);
                g.drawRect(0, 0, getWidth() - 1, screenFont.getHeight() + 1);
                g.drawString(titleString, 2, 0, (Graphics.LEFT | Graphics.TOP));
            }

            // BUSYメッセージの表示...
            if (busyMessage != null)
            {
                showBusyMessage(g, busyMessage);
            }

            if ((screenKind != SELECTION_FORM)&&(screenKind == INFORMATION_FORM))
            {
                painting = false;
                return;
            }
            else if (screenKind == SELECTION_FORM)
            {
                // 選択肢の表示...
                if (selectionData != null)
                {
                    if (redraw == true)
                    {
                        drawSelectionItems(g, selectionData);
                    }
                    else
                    {
                        drawSelectionItemsPart(g, selectionData);                
                    }
                    redraw = true;
                }
            }

            // 前ページがある場合、マークをつける
            int charWidth = screenFont.stringWidth("▽") + 1;
            if (currentPage != 0)
            {
                g.setColor(0x00ff008f);
                g.drawString("△", getWidth() - charWidth, screenFont.getHeight() + 1, (Graphics.LEFT | Graphics.TOP));
            }

            // 次ページがある場合、マークをつける
            if (currentPage != (nofPage - 1))
            {
                g.setColor(0x00ff008f);
                g.drawString("▽", getWidth() - charWidth, getHeight() - screenFont.getHeight() - 1, (Graphics.LEFT | Graphics.TOP));
            }
        }
        catch (Exception e)
        {
            // 何もしない...
        }
        
        painting = false;
        return;
    }

    /**
     *  選択中のアイテムを表示する
     * @param g
     * @param data
     */
    private void drawCurrentSelectedItem(Graphics g, String data, int topX, int topY)
    {
        int drawRange = getWidth() - 1;
        int dataRange = screenFont.stringWidth(data);
        if (dataRange <= drawRange)
        {
            g.drawString(data, topX, topY, (Graphics.LEFT | Graphics.TOP));
        }
        else
        {
                int offset = 0;
                dataRange = data.length();
                while (screenFont.substringWidth(data, offset, (dataRange - offset)) > drawRange)
                {
                    offset++;
                }
                g.drawSubstring(data, offset, (dataRange - offset), topX, topY, (Graphics.LEFT | Graphics.TOP)); 
/*
            g.drawString(data, topX, topY, (Graphics.LEFT | Graphics.TOP));
*/
        }
    }

    /**
     * 選択肢を表示する...
     * 
     * @param g
     * @param items
     */
    private void drawSelectionItemsPart(Graphics g, Vector items)
    {
        int topLine = (screenLines - 1) * currentPage;
        for (int index = 0; index < screenLines; index++)
        {
            int lineToShow = index + topLine;
            if (lineToShow >= items.size())
            {
                break;
            }
            if ((lineToShow >= (selectedLine - 1))&&(lineToShow <= (selectedLine + 1)))
            {
                String item = (String) items.elementAt(lineToShow);
                int    y   = (index + 1) * (screenFont.getHeight() + 1) + 1;
                if (lineToShow == selectedLine)
                {
                    g.setColor(cursorBackColor);
                    g.fillRect(0, y, getWidth(), (screenFont.getHeight()));
                    g.setColor(cursorForeColor);
                    drawCurrentSelectedItem(g, item, 1, y);
                }
                else
                {
                    if (items.size() > 10)
                    {
                        if ((lineToShow % 2) == 0)
                        {
                            g.setColor(backColorEven);
                        }
                        else
                        {
                            g.setColor(backColorOdd);
                        }
                    }
                    else
                    {
                        g.setColor(backColorOdd);                        
                    }
                    g.fillRect(0, y, getWidth(), (screenFont.getHeight()));
                    g.setColor(foreColor);
                    g.drawString(item, 1, y, (Graphics.LEFT | Graphics.TOP));        
                }
            }
        }
        return;
    }
    
    /**
     * 選択肢を表示する...
     * 
     * @param g
     * @param items
     */
    private void drawSelectionItems(Graphics g, Vector items)
    {
        int topLine = (screenLines - 1) * currentPage;
        for (int index = 0; index < screenLines; index++)
        {
            int lineToShow = index + topLine;
            if (lineToShow >= items.size())
            {
                break;
            }
            String item = (String) items.elementAt(lineToShow);
            int    y   = (index + 1) * (screenFont.getHeight() + 1) + 1;
            if (lineToShow == selectedLine)
            {
                g.setColor(cursorBackColor);
                g.fillRect(0, y, getWidth(), (screenFont.getHeight()));
                g.setColor(cursorForeColor);
                drawCurrentSelectedItem(g, item, 1, y);
            }
            else
            {
                if (items.size() > 10)
                {
                    if ((lineToShow % 2) == 0)
                    {
                        g.setColor(backColorEven);
                    }
                    else
                    {
                        g.setColor(backColorOdd);
                    }
                }
                else
                {
                    g.setColor(backColorOdd);                        
                }
                g.fillRect(0, y, getWidth(), (screenFont.getHeight()));
                g.setColor(foreColor);
                g.drawString(item, 1, y, (Graphics.LEFT | Graphics.TOP));        
            }
        }
        return;
    }
    
    /**
     * ビジーメッセージを表示する
     * 
     * @param g
     */
    private void showBusyMessage(Graphics g, String msg)
    {
        // 表示可能行を設定する
        int y       = (screenFont.getHeight()) + 4;
        int x       = 0;
        int limitX  = getWidth() - 4;
        g.setColor(0);
        for (int msgIndex = 0; msgIndex < msg.length(); msgIndex++)
        {
            char drawChar = msg.charAt(msgIndex);
            if (drawChar != '\n')
            {
                g.drawChar(drawChar, x, y, (Graphics.LEFT | Graphics.TOP));
                x = x + screenFont.charWidth(drawChar);
            }
            else
            {
                // 改行コードのときは表示せずに改行する...
                x = x + limitX;
            }
            if (x >= limitX)
            {
                x = 0;
                y = y + screenFont.getHeight() + 1;
            }
        }
        return;
    }    

    /*
     *   コマンドボタン実行時の処理
     *   
     */
    public void commandAction(Command c, Displayable d)
    {
        if (screenKind == BUSY_FORM)
        {
            // 選択不可の場合は、キャンセルが押されても何もしない。
            return;
        }

        if (c == cancelCmd)
        {
            // キャンセル指示を受けた...
            inputClear();
        }
        else if (c == selectCmd)
        {
            // アイテムが選択された
            inputFire(0);
            return;
        }
        else if (c == confirmCmd)
        {
            // OKを押した...
            releaseConfirmation(Canvas.FIRE);
            return;
        }
        else if (c == getMoveCmd)
        {
            // 板移動＋取得を選択した...
            inputFire(1);
        }
        else if (c == outputCmd)
        {
            // memo出力を選択した...
            inputFire(2);
        }
        else if (c == getHttpCmd)
        {
            // http通信を選択した...
            inputFire(3);
        }
        else if (c == getPicsCmd)
        {
            // 画像表示を選択した...
            inputFire(4);
        }
        return;
    }

    /*
     *   ビジーメッセージを表示する
     * 
     * 
     */
    public void showBusyMessage(String title, String message, int usage)
    {
        // 一度コマンドを解除する...
        this.removeCommand(selectCmd);
        this.removeCommand(cancelCmd);
        this.removeCommand(getMoveCmd);
        this.removeCommand(outputCmd);
        this.removeCommand(getHttpCmd);
        this.removeCommand(getPicsCmd);
        this.setCommandListener(null);

        currentPage  = 0;
        nofPage      = 1;
        titleString  = title;
        busyMessage  = message;
        screenKind   = usage;
        switch (screenKind)
        {
          case CONFIRMATION_FORM:
            this.addCommand(confirmCmd);
            this.setCommandListener(this);
            break;

          case BUSY_FORM_UPDATE:
        	// 情報更新時には、タイマ設定を更新しない
            break;

          default:
            // ウォッチドッグタイマを設定する
            try
            {
                if (watchDogTimer != null)
                {
                    watchDogTimer.startWatchDog(blinkDuration);
                    isBlinking = true;
                }
            }
            catch (Exception ex)
            {
                // 何もしない...
            }
            /**/
            break;
        }
        repaint();
        return;
    }    

    /*
     *   ビジーメッセージを消す
     *   
     */
    public void hideBusyMessage()
    {
        titleString      = null;
        busyMessage      = null;
        screenKind       = SELECTION_FORM;
        repaint();
        timerTemp = -1;
        isBlinking = false;
        parent.endSelectForm(-1, -1, -1);    

        // タイトルの色を設定
        titleBackColor = defaultTitleBackColor;
        titleForeColor = defaultTitleForeColor;

        System.gc();
        return;
    }

    /*
     *   選択アイテムの表示を行う
     * 
     * 
     */
    public void showSelection(String title, Vector itemList, int topItem, int scene)
    {
        prepareScreenSize();

        // タイトルの色を設定
        titleBackColor = defaultTitleBackColor;
        titleForeColor = defaultTitleForeColor;

        // 選択アイテムの表示を行う
        titleString      = title;
        busyMessage      = null;
        currentScene     = scene;
        screenKind       = SELECTION_FORM;
        
        if (itemList.size() <= 10)
        {
            selectionData = null;
            selectionData = new Vector(itemList.size());
            for (int loop = 0; loop < itemList.size(); loop++)
            {
                String item = ((loop + 1) % 10) + " : " + (String) itemList.elementAt(loop);
                selectionData.addElement(item);
            }
            numSelection = itemList.size();
        }
        else
        {
            selectionData   = null;
            selectionData   = itemList;
            numSelection    = -1;
        }

        // コマンドを追加する
        if ((scene == parent.CONFIRM_SELECT_OSUSUME2CH)||
            (scene == parent.CONFIRM_GET_THREADNUMBER)||
            (scene == parent.CONFIRM_SELECT_FIND2CH))
        {
            this.addCommand(selectCmd);
            this.addCommand(getMoveCmd);
            this.addCommand(cancelCmd);
            this.setCommandListener(this);
        }
        else if (scene == parent.CONFIRM_URL_FOR_BROWSER)
        {
            this.addCommand(selectCmd);
            this.addCommand(outputCmd);
            this.addCommand(getHttpCmd);
            this.addCommand(getPicsCmd);
            this.addCommand(cancelCmd);
            this.setCommandListener(this);            
        }
        else
        {
            this.addCommand(selectCmd);
            this.addCommand(cancelCmd);
            this.setCommandListener(this);
        }

        // 先頭行（選択行）を設定する
        nofPage     = (itemList.size() / screenLines) + 1;
        if (itemList.size() == screenLines)
        {
            nofPage = 1;
        }
        currentPage = topItem / (screenLines - 1);
        selectedLine = topItem;
        repaint();
        return;
    }
    
    /**
     * 数字入力を検出する...
     * 
     * @param keyCode
     * @return
     */
    private boolean keyPressedNumber(int keyCode)
    {
        if (numSelection <= 0)
        {
            // 数字で入力するモードでない...
            return (false);
        }

        int code = -1;
        switch (keyCode)
        {
          case Canvas.KEY_NUM1:
            code = 0;
            break;

          case Canvas.KEY_NUM2:
            code = 1;
            break;

          case Canvas.KEY_NUM3:
            code = 2;
            break;

          case Canvas.KEY_NUM4:
            code = 3;
            break;

          case Canvas.KEY_NUM5:
            code = 4;
            break;

          case Canvas.KEY_NUM6:
            code = 5;
            break;

          case Canvas.KEY_NUM7:
            code = 6;
            break;

          case Canvas.KEY_NUM8:
            code = 7;
            break;

          case Canvas.KEY_NUM9:
            code = 8;
            break;

          case Canvas.KEY_NUM0:
            code = 9;
            break;

          default:
            // 数字キー以外の入力だった...
            return (false);
            //break;
        }
        if ((code >= 0)&&(code < numSelection))
        {
            // アイテムが選択された！！
            selectedLine = code;

            // 有効な数値入力だった場合は、Enterを入力し終了する
            serviceRepaints();
            inputFire(0);
        }
        repaint();
        return (true);
    }


    /*
     *  押した座標を処理する...
     * 
     * 
     */
    public void pointerPressed(int x, int y)
    {
        int currentSelectedLine = (screenLines - 1) * currentPage + (y / (screenFont.getHeight() + 1)) - 1;

        if (x > (getWidth() - (screenFont.charWidth('□') * 2)))
        {
            // 画面の右上端、右下端をタップした場合...
            int index =  y / (screenFont.getHeight() + 1);
            if (index <= 1)
            {
                // 前ページに移動する
                if (inputLeft() == true)
                {
                    repaint();
                }
                return;
            }
            else if (index >= (screenLines - 1))
            {
                // 次ページに移動する
                if (inputRight() == true)
                {
                    repaint();
                }
                 return;
            }
        }
        
        if (currentSelectedLine == selectedLine)
        {
            // アイテム選択、、、
            inputFire(0);
            return;
        }
        if (currentSelectedLine < selectionData.size())
        {
            selectedLine = currentSelectedLine;
            repaint();
        }
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

    /**
     * 画面サイズ変更時に呼ばれる処理...
     * 
     * 
     */
    public void sizeChanged(int w, int h)
    {
        prepareScreenSize();
        
        repaint();

        return;
    }

    /**
     *   キー入力
     */
    public void keyPressed(int keyCode)
    {
        boolean repaint = false;
        if ((keyCode == 0)||(painting == true))
        {
            // キー入力がない、または描画中の場合には折り返す
            return;
        }

        if (screenKind == BUSY_FORM)
        {
            // 選択不可の場合は、押されても何もしない。
            return;
        }
        parent.keepWatchDog(0);

        int kc = getGameAction(keyCode);
        if (releaseConfirmation(kc) == true)
        {
            // 確認フォームで何かキーが押された場合には終了する。
            return;
        }

        // 数値直接入力可能モードだった場合...
        if (keyPressedNumber(keyCode) == true)
        {
            return;
        }

        // *キー、 ','キーを押した場合には、先頭へ移動する
        if ((keyCode == Canvas.KEY_STAR)||(keyCode == 44))
        {
            if (currentScene == parent.CONFIRM_URL_FOR_BROWSER)
            {
                // 画像表示を行う。。。
                inputFire(4);
                return;
            }
            currentPage  = 0;
            selectedLine = 0;
            repaint();
            return;
        }

        // #キー、'.'キーを押した場合には、末尾へ移動する
        if ((keyCode == Canvas.KEY_POUND)||(keyCode == 46))
        {
            if (currentScene == parent.CONFIRM_URL_FOR_BROWSER)
            {
                // ファイル取得を行う。。。
                inputFire(3);
                return;
            }
            currentPage  = nofPage - 1;
            selectedLine = selectionData.size() - 1;
            repaint();
            return;
        }
        if (keyCode == -8)
        {
            // クリアキー for Nokia ... (現物合わせ...)
            inputClear();
            repaint();
            return;
        }

        if ((keyCode == 112)||(keyCode == 80))
        {
            // 前ページを表示する (p, Pキー)
            inputLeft();
            repaint();
            return;
        }

        if ((keyCode == 110)||(keyCode == 78))
        {
            // 次ページを表示する (n, Nキー)
            inputRight();
            repaint();
            return;
        }

        if ((keyCode == 27)||(keyCode == 113)||(keyCode == 81))
        {
            // クリアする...(ESC, Qキー)
            repaint = inputClear();
            repaint();
            return;
        }

        if ((keyCode == 8)&&(getGameAction(keyCode) != Canvas.FIRE))
        {
            // BSキー (クリアする)
            inputClear();
            repaint();
        }
        
        if ((keyCode == 119)||(keyCode == 87))
        {
            // Wキー
            inputGameD();
            repaint();
            return;
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
            repaint = inputDown(1);
            break;

          case Canvas.UP:
            repaint = inputUp(1);
            break;

          case Canvas.FIRE:
            repaint = inputFire(0);
            break;

          case Canvas.GAME_A:
            // GAME-Aキー
            repaint = inputUp(3);
            break;

          case Canvas.GAME_B:
            // GAME-Bキー
            repaint = inputDown(3);
            break;

          case Canvas.GAME_C:
            // GAME-Cキー
            repaint = inputClear();
            break;

          case Canvas.GAME_D:
            // GAME-Dキー
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
     *   キーリピートしているとき...
     * 
     * 
     */
    public void keyRepeated(int keyCode)
    {
        parent.keepWatchDog(0);

        boolean repaint = false;
        if ((keyCode == 0)||(painting == true))
        {
            // キー入力がない、または描画中の場合には折り返す
            return;
        }

        if (screenKind != SELECTION_FORM)
        {
            // 選択不可の場合は、押されても何もしない。
            return;
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
            repaint = inputDown(1);
            break;

          case Canvas.UP:
            repaint = inputUp(1);
            break;

          case Canvas.FIRE:
            repaint = false;
            break;

          case Canvas.GAME_A:
            // GAME-Aキー
            repaint = inputUp(3);
            break;

          case Canvas.GAME_B:
            // GAME-Bキー
            repaint = inputDown(3);
            break;

          case Canvas.GAME_C:
            // GAME-Cキー
            repaint = false;
            break;

          case Canvas.GAME_D:
            // GAME-Dキー
            repaint = false;
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

    private boolean inputLeft()
    {
        if (currentPage == 0)
        {
            currentPage  = nofPage - 1;
            selectedLine = selectionData.size() - 1;
            return (true);
        }
        currentPage--;
        selectedLine = selectedLine - (screenLines - 1);
        if (selectedLine < 0)
        {
            selectedLine = 0;
        }        
        return (true);
    }

    private boolean inputRight()
    {
        currentPage++;
        if (currentPage == nofPage)
        {
            currentPage  = 0;
            selectedLine = 0;
            return (true);
        }
        selectedLine = selectedLine + screenLines - 1;
        if (selectedLine >= selectionData.size())
        {
            selectedLine = selectionData.size() - 1;
        }
        return (true);
    }

    private boolean inputUp(int count)
    {
        if (parent.getCursorSingleMovingMode() == true)
        {
            selectedLine = selectedLine - count;
            if (selectedLine < currentPage * (screenLines - 1))
            {
                selectedLine = selectedLine + (screenLines - 1);
                inputLeft();
            }
            return (true);
        }

        selectedLine = selectedLine - count;
        if (selectedLine < currentPage * (screenLines - 1))
        {
            // ページの末尾にカーソルを移す
            selectedLine = (screenLines - 1) * (currentPage + 1);
            if (selectedLine >= selectionData.size())
            {
                selectedLine = selectionData.size() - 1;
            }
        }
        else
        {
            if (count == 1)
            {
                redraw = false;
            }
        }
        return (true);
    }

    private boolean inputDown(int count)
    {
        selectedLine = selectedLine + count;
        if (parent.getCursorSingleMovingMode() == true)
        {
            if (selectedLine >= selectionData.size())
            {
                selectedLine = 0;
                currentPage = 0;
            }
            else if (selectedLine > (currentPage + 1) * (screenLines - 1))
            {
                selectedLine = selectedLine - (screenLines - 1);
                   inputRight();
            }
            return (true);
        }

        if (selectedLine == selectionData.size())
        {
            // ページの先頭にカーソルを移す
            selectedLine = (screenLines - 1) * currentPage;
            return (true);
        }

        if (selectedLine > (currentPage + 1) * (screenLines - 1))
        {
            // ページの先頭にカーソルを移す
            selectedLine = (screenLines - 1) * currentPage;
        }
        else
        {
            if (count == 1)
            {
                redraw = false;
            }
        }
        return (true);
    }

    private boolean inputFire(int addInfo)
    {
        if (screenKind == BUSY_FORM)
        {
            // 選択不可の場合は、キャンセルが押されても何もしない。
            return (false);
        }
        if (releaseConfirmation(Canvas.FIRE) == true)
        {
            // Confirmの場合には終了する...
            return (false);
        }

        titleString   = null;
        selectionData = null;
        parent.endSelectForm(currentScene, selectedLine, addInfo);
        System.gc();
        return (false);
    }

    private boolean inputClear()
    {
        if (screenKind == BUSY_FORM)
        {
            // 選択不可の場合は、キャンセルが押されても何もしない。
            return (false);
        }
        if (releaseConfirmation(Canvas.GAME_C) == true)
        {
            // Confirmの場合には終了する...
            return (false);
        }

        titleString   = null;
        selectionData = null;
        parent.endSelectForm(currentScene, -1, -1);
        System.gc();
        return (false);
    }

    private boolean inputGameD()
    {
        if (screenKind == BUSY_FORM)
        {
            // 選択不可の場合は、キャンセルが押されても何もしない。
            return (false);
        }
        if (releaseConfirmation(Canvas.GAME_D) == true)
        {
            // Confirmの場合には終了する...
            return (false);
        }

        titleString   = null;
        selectionData = null;
        parent.endSelectForm(currentScene, -1, -1);
        System.gc();
        return (false);
    }
    
    private boolean releaseConfirmation(int actionCode)
    {
        // コマンドを解除する...
        if (screenKind == CONFIRMATION_FORM)
        {
            this.removeCommand(confirmCmd);
            this.setCommandListener(null);

            titleString   = null;
            selectionData = null;
            parent.returnToPreviousForm(parent.ACTION_NOTHING);
            System.gc();
            return (true);
        }
        return (false);
    }

    /**
     *  タイムアウトを検出する
     * 
     */
    public void detectTimeout(long isDetected)
	{
        // ウォッチドッグタイマを再設定する
        try
        {
            if ((watchDogTimer != null)&&(isBlinking == true))
            {
                watchDogTimer.startWatchDog(blinkDuration);
            }
        }
        catch (Exception ex)
        {
            // 何もしない...
        }
        /**/

        // タイトル色を反転させる
    	int swapColor = titleBackColor;
    	timerTemp++;
    	switch ((timerTemp % 4))
    	{
            case 0:
              swapColor = COLOR_A;
              break;

            case 1:
              swapColor = COLOR_B;
              break;

            case 2:
              swapColor = COLOR_C;
              break;

            case 3:
            default:
              swapColor = COLOR_D;
              break;
    	}
        titleBackColor = titleForeColor;
        titleForeColor = swapColor;
        repaint();
        serviceRepaints();
        System.gc();
        return;
	}
}
