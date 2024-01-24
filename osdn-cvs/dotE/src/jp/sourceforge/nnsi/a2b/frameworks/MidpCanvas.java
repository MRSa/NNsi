package jp.sourceforge.nnsi.a2b.frameworks;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * 表示（メイン）処理クラス<br>
 * 画面描画を行う親玉です。
 * 
 * @author MRSa
 *
 */
public class MidpCanvas  extends Canvas implements CommandListener, IDataProcessing, ILogMessageDisplay
{
    private final int CMD_EXIT       = 0;           // Exitコマンド
    private final int CMD_MENU       = 1;           // Menuコマンド

    private final int COLOR_BLACK    = 0;           // 黒色
    private final int COLOR_WHITE    = 0x00ffffff;  // 白色
    private final int BACKCOLOR_BUSY = 0x00ffffc0;  // BUSYエリアの背景色
    private final int FORECOLOR_BUSY = COLOR_BLACK; // BUSYエリアの文字色
    
    private Font      useFont     = null;    // 表示フォント
    private String    busyMessage = null;    // ビジーメッセージ
    private boolean  isBusyMode  = false;  // ビジー状態(操作禁止中)
    private boolean  painting    = false;  // 描画実行中
    private boolean  working     = false;  // データ更新中
    private boolean  isFirstTime = true;   // 初回描画時か？
    private int       myWidth     = 240;
    private int       myHeight    = 320;

    private Command   menuCmd = new Command("Menu", Command.ITEM, CMD_MENU);
    private Command   exitCmd = new Command(" × ", Command.EXIT, CMD_EXIT);

    private MidpSceneSelector parent       = null;
    private IMidpScreenCanvas activeCanvas = null;

    /**
     * 画面表示の準備を行います
     * @param object シーンセレクタ
     * @param fontSize 表示するフォントのサイズ
     */
    public MidpCanvas(MidpSceneSelector object, int fontSize)
    {
        // 親を記憶する
        parent = object;

        // タイトルの設定
        setTitle(null);

        // フォントの設定
        useFont = Font.getDefaultFont();
        useFont = Font.getFont(useFont.getFace(), useFont.getStyle(),fontSize);

        // コマンドの追加
        this.addCommand(menuCmd);
        this.addCommand(exitCmd);
        this.setCommandListener(this);    
    }

    /**
     *  描画クラスの準備...
     *
     */
    public void prepare(int width, int height)
    {
        myWidth = width;
        myHeight = height;
        return;
    }

    /**
     *   画面の準備...
     */
    public void prepareScreen()
    {
        //
        return;
    }

    /**
     * 画面の描画幅を応答する
     * @return 画面の描画幅
     * 
     */
    public int getWidth()
    {
        return (myWidth);
    }

    /**
     * 画面の描画高さを応答する
     * @return 画面の描画高さ
     */
    public int getHeight()
    {
        return (myHeight);
    }

    /**
     * 使用するフォントを応答する
     * @return 使用するフォント
     */
    public Font getFont()
    {
        return (useFont);
    }

    /**
     * 画面再描画指示
     * @param forceUpdate 画面再描画が済むまで停止する
     */
    public void redraw(boolean forceUpdate)
    {
        repaint();
        if (forceUpdate == true)
        {
            serviceRepaints();
        }
        return;
    }

    /**
     * 画面の描画を実施する
     * @param g グラフィックキャンバス
     */
    public void paint(Graphics g)
    {
        // 画面描画中のときには折り返す
        if ((painting == true)||(g == null))
        {
            return;
        }
        painting = true;  // 描画開始

        if (isFirstTime == true)
        {
        	// 縦・横サイズを設定しなおす。
        	isFirstTime = false;
        }
        
        // BUSYメッセージが残っている？
        if ((isBusyMode == false)&&(busyMessage != null))
        {
            hideBusyMessage(g);
        }
        
        /////////////////////////////////////
        try
        {
            if (activeCanvas != null)
            {
                // フォントを設定
                g.setFont(useFont);

                // ウィンドウが画面全体ではないときは、ウィンドウ枠を表示する
                if (activeCanvas.getTopY() > 0)
                {
                    g.setColor(COLOR_BLACK);
                    g.drawRect(activeCanvas.getTopX() - 1, activeCanvas.getTopY() - 1, activeCanvas.getWidth() + 1, activeCanvas.getHeight() + 1);
                }
                // 画面描画を実施...
                activeCanvas.paint(g);
            }
        }
        catch (Exception ex)
        {
            // エラー情報を画面表示 (灰色で...)
            g.setColor(0x007f7f7f);
            g.drawString(ex.getMessage(), 0, (useFont.getHeight() * 2), (Graphics.LEFT | Graphics.TOP));
        }
        //////////////////////////////////////

        // BUSYメッセージを表示する？
        if ((isBusyMode == true)&&(busyMessage != null))
        {
            showBusyMessage(g);
        }
        painting = false;  // 描画終了
        return;
    }

    /**
     * ビジーメッセージの表示(画面最下行に文字列を表示する)
     * 
     */
    private void showBusyMessage(Graphics g)
    {
        int height = useFont.getHeight() + 2;

        // 画面の塗りつぶし
        g.setColor(BACKCOLOR_BUSY);
        g.fillRect(0, (getHeight() - height - 1), getWidth(), (getHeight() - 1));

        // メッセージの表示
        g.setColor(FORECOLOR_BUSY);
        g.setFont(useFont);
        g.drawString(busyMessage, 5, (getHeight() - height), (Graphics.LEFT | Graphics.TOP));
        return;        
    }

    /**
     *  ビジーメッセージの表示削除
     * 
     */
    private void hideBusyMessage(Graphics g)
    {
        int height = useFont.getHeight() + 2;
        
        // 画面の塗りつぶし
        g.setColor(COLOR_WHITE);
        g.fillRect(0, (getHeight() - height - 1), getWidth(), (getHeight() - 1));
        busyMessage = null;  // 表示メッセージの削除
        return;        
    }

    /**
     *  ビジーメッセージの設定
     *
     *  @param message     ビジーメッセージ
     *  @param forceUpdate 画面再描画
     *  @param lockOperation 画面操作の禁止
     */    
    public void outputInformationMessage(String message, boolean forceUpdate, boolean lockOperation)
    {
        working = lockOperation;
        isBusyMode  = true;
        busyMessage = message;
        if (forceUpdate == true)
        {
             repaint();
             serviceRepaints();
        }
        return;
    }

    /**
     *  ビジーメッセージの表示終了
     *
     */    
    public void finishInformationMessage()
    {
        working = false;
        isBusyMode  = false;
        return;
    }

    /**
     *  ビジーメッセージのクリア
     *
     */    
    public void clearInformationMessage()
    {
        working = false;
        isBusyMode  = false;
        repaint();
        return;
    }

    /**
     * 画面表示を切り替える
     * @param newCanvas 新しい画面
     */
    public void changeActiveCanvas(IMidpScreenCanvas newCanvas)
    {
        try
        {
            if (activeCanvas != null)
            {
                activeCanvas.stop();
            }
            activeCanvas = newCanvas;
            if (activeCanvas != null)
            {
                activeCanvas.start();
           }
        }
        catch (Exception ex)
        {
            //
            // setBusyMessage() : エラーの情報
            //
        }
        repaint();
        return;
    }

    /**
     *  データの更新処理
     *  @param processingId 処理ID
     *
     */
    public void updateData(int processingId)
    {
        if (working == true)
        {
            return;
        }
        if (activeCanvas != null)
        {
            working = true;
            activeCanvas.updateData(processingId);
            working = false;
        }
        repaint();
        return;
    }

    /**
     *  情報領域の情報を更新する
     * 
     *  @param message     表示するメッセージ
     *  @param mode        表示モード(キャンバス用)
     *  @param forceUpdate 画面再描画を実施を今実施
     */
    public void updateInformation(String message, int mode, boolean forceUpdate)
    {
        if (activeCanvas != null)
        {
            activeCanvas.updateInformation(message, mode);
            repaint();
            if (forceUpdate == true)
            {
                serviceRepaints();
            }
        }
        return;
    }

    /**
     * 処理の終了を通知する
     * 
     * @param processingId 処理ID
     * @param result 処理結果
     * @param vibrateTime バイブレーションする時間(単位:ms)
     */
    public void finishUpdateData(int processingId, int result, int vibrateTime)
    {
        if (activeCanvas != null)
        {
            activeCanvas.finishUpdateData(processingId, result);
        }
        if (vibrateTime > 0)
        {
            parent.vibrate(vibrateTime);
        }
        repaint();
        return;
    }

    /**
     * コマンド実行時の処理
     *
     * @param c コマンド
     * @param d 表示
     */
    public void commandAction(Command c, Displayable d) 
    {
        int command = c.getPriority();
        commandActionMain(command);
        return;
    }

    /**
     * コマンド実行時の処理。。。
     *
     * @param command コマンド
     */
    private void commandActionMain(int command)
    {
        if (command == CMD_MENU)
        {
            // メニューの表示
            if (activeCanvas != null)
            {
                activeCanvas.showMenu();
                repaint();
            }
            return;
        }
        else if (command == CMD_EXIT)
        {
            if (activeCanvas == null)
            {
                // アプリケーションを終了させる...
                parent.stop();
                return;
            }
            // 終了するかどうかチェックする
            if (activeCanvas.checkExit() == true)
            {
                parent.stop();
            }
            return;
        }
        return;
    }

    /**
     *   キー入力があったときの処理
     *   @param keyCode キー入力コード
     */
    public void keyPressed(int keyCode)
    {
        // タイマーの更新
        parent.extendTimer(MidpTimer.TIMER_EXTEND);

        boolean repaint = false;
        if ((keyCode == 0)||(painting == true)||(working == true))
        {
            // キー入力がない、または描画中の場合には折り返す
            return;
        }

        // キー入力の確認
        if (activeCanvas != null)
        {
            repaint = checkKeyInput(keyCode, false);
        }

        // 画面再描画するか？
        if (repaint == true)
        {
            repaint();
        }
        return;
    }

    /**
     *   キーリピートしているとき
     *   @param keyCode キーコード
     */
    public void keyRepeated(int keyCode)
    {
        // タイマーの更新
        parent.extendTimer(MidpTimer.TIMER_EXTEND);

        boolean repaint = false;
        if ((keyCode == 0)||(painting == true)||(working == true))
        {
            // キー入力がない、または描画中の場合には折り返す
            return;
        }

        // キー入力の確認
        if (activeCanvas != null)
        {
            repaint = checkKeyInput(keyCode, true);
        }

        // 画面を再描画する？
        if (repaint == true)
        {
            repaint();
        }
        return;
    }

    /**
     *   画面サイズ変更時に呼ばれる処理(何もしない...)
     *   
     *   @param w 画面幅
     *   @param h 画面高さ
     */
    public void sizeChanged(int w, int h)
    {
        parent.extendTimer(MidpTimer.TIMER_EXTEND);
        repaint();
        return;
    }
    
    /**
     *  押した座標を処理
     *  
     *  @param x タップしたX座標
     *  @param y タップしたY座標
     */
    public void pointerPressed(int x, int y)
    {
        parent.extendTimer(MidpTimer.TIMER_EXTEND);
        boolean repaint = false;
        if (activeCanvas != null)
        {
            int topX   = activeCanvas.getTopX();
            int topY   = activeCanvas.getTopY();
            int width  = activeCanvas.getWidth();
            int height = activeCanvas.getHeight();

            if (((x >= topX)&&(x <= topX + width))&&
                ((y >= topY)&&(y <= topY + height)))
            {
                repaint = activeCanvas.pointerPressed((x - topX), (y - topY));
            }
        }

        // 画面を再描画する？
        if (repaint == true)
        {
            repaint();
        }
        return;
    }

    /**
     *   キーの入力を解析し報告する
     */
    private boolean checkKeyInput(int keyCode, boolean isRepeat)
    {
        boolean isProcessed = false;
        
        // 数字キーの検出...
        isProcessed = checkNumberInput(keyCode, isRepeat);
        if (isProcessed == true)
        {
            return (true);
        }

        // カーソルキーの検出...
        isProcessed = checkCursorKeyInput(keyCode, isRepeat);
        if (isProcessed == true)
        {
            return (true);
        }

        // キー入力が認識できなかった...画面に聞く。
        return (activeCanvas.inputOther(keyCode, isRepeat));
    }

    /**
     * 数字ボタンが入力されたかどうかチェックする
     * @param keyCode キーコード
     * @param isRepeat キーリピート入力(true) / １ショット入力(false)
     * @return 表示する(true) / 表示しない(false)
     */
    private boolean checkNumberInput(int keyCode, boolean isRepeat)
    {
        boolean ret = true;
        int number = -1;
        
        // 数字キーの入力検出
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            number = 0;
            break;

          case Canvas.KEY_NUM1:
            number = 1;
            break;

          case Canvas.KEY_NUM2:
            number = 2;
            break;

          case Canvas.KEY_NUM3:
            number = 3;
            break;

          case Canvas.KEY_NUM4:
            number = 4;
            break;

          case Canvas.KEY_NUM5:
            number = 5;
            break;

          case Canvas.KEY_NUM6:
            number = 6;
            break;

          case Canvas.KEY_NUM7:
            number = 7;
            break;

          case Canvas.KEY_NUM8:
            number = 8;
            break;

          case Canvas.KEY_NUM9:
            number = 9;
            break;

          case 46:
          case Canvas.KEY_POUND:
            // # キー、, キーの入力
            ret = activeCanvas.inputPound(isRepeat);
            break;

          case 44:
          case Canvas.KEY_STAR:
            // * キー、. キーの入力
            ret = activeCanvas.inputStar(isRepeat);
            break;

          case 90:
          case 122:
            // Zキー
            ret = activeCanvas.inputGameA(isRepeat);
            break;

          case 88:
          case 120:
            // Xキー
            ret = activeCanvas.inputGameB(isRepeat);
            break;

          case 27:
          case 113:
          case 81:
          case -8:
            // ESCキー、Qキー、Nokiaのクリアキー (現物合わせ)
            ret = activeCanvas.inputGameC(isRepeat);
            break;

          case 119:
          case  87:
          case -50:
            // Wキー、NokiaのFuncキー... (現物合わせ)
            ret = activeCanvas.inputGameD(isRepeat);
            break;

          case 67:
          case 99:
          case -5:
            // Sキー、Enterキー (現物合わせ)
            ret = activeCanvas.inputFire(isRepeat);
            break;

          default:
            ret = false;
            break;
        }

        // 数字キー入力を通知する
        if (number >= 0)
        {
            ret = activeCanvas.inputNumber(number, isRepeat);
        }
        return (ret);
    }

    /**
     * カーソルキーが入力されたかどうかチェックする
     * @param isRepeat
     * @return 表示する(true) / 表示しない(false)
     */
    private boolean checkCursorKeyInput(int keyCode, boolean isRepeat)
    {
        boolean ret = false;
        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            ret = activeCanvas.inputLeft(isRepeat);
            break;
              
          case Canvas.RIGHT:
            ret = activeCanvas.inputRight(isRepeat);
            break;

          case Canvas.DOWN:
            ret = activeCanvas.inputDown(isRepeat);
            break;

          case Canvas.UP:
            ret = activeCanvas.inputUp(isRepeat);
            break;

          case Canvas.FIRE:
            ret = activeCanvas.inputFire(isRepeat);
            break;

          case Canvas.GAME_A:
            ret = activeCanvas.inputGameA(isRepeat);
            break;

          case Canvas.GAME_B:
            ret = activeCanvas.inputGameB(isRepeat);
            break;

          case Canvas.GAME_C:
            ret = activeCanvas.inputGameC(isRepeat);
            break;

          case Canvas.GAME_D:
            ret = activeCanvas.inputGameD(isRepeat);
            break;

          default:
            // キー入力を検出できなかった...
            ret = false;
            break;
        }
        return (ret);
    }
}
