/**
 *  a2B Frameworkの画面表示用パッケージです。
 * 
 */
package jp.sourceforge.nnsi.a2b.screens;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;

/**
 * 画面キャンバス(ベースクラス)<br>
 * <em>直接このクラスを使うことはないとは思いますが、このクラスを継承することで、
 * 実装な必要なクラスを省略することができます。</em>
 *
 * @author MRSa
 *
 */
public class MidpScreenCanvas implements IMidpScreenCanvas
{
    protected MidpSceneSelector parent  = null;

    protected final int baseBackColor      = 0x00ffffff;
    protected final int baseForeColor      = 0x00000000;
    protected final int baseTitleForeColor = 0x00f0f0f0;

    protected int   baseTitleBackColor = 0x000000ff;
    protected int   titleForeColor     = baseTitleForeColor;
    protected int   subTitleForeColor  = baseTitleForeColor;

    protected int   canvasId           = -1;

    protected Font   screenFont        = null;  // 表示に使用するフォント
    protected String titleString       = null;  // 表示タイトル
    protected String titleSubString    = null; // 表示タイトル(サブ)

    protected int   screenTopX   = 0;     // 描画領域の左端(X)
    protected int   screenTopY   = 0;     // 描画領域の左端(Y)
    protected int   screenWidth  = 160;   // 画面幅
    protected int   screenHeight = 160;   // 画面高さ
    protected int   screenLine   = 0;     // データ表示可能ライン数

    /**
     * コンストラクタでは、シーンセレクタをもらいます。
     * 
     * @param screenId 画面ID
     * @param object シーンセレクタ
     */
    public MidpScreenCanvas(int screenId, MidpSceneSelector object)
    {
        canvasId  = screenId;
        parent    = object;
    }

    /**
     * 画面の表示準備
     * @param font 表示するフォント
     */
    public void prepare(Font font)
    {
        screenFont   = font;
    }

    /**
     *  画面サイズを設定する
     *  
     *  @param topX  左上X座標
     *  @param topY  左上Y座標
     *  @param width 画面幅(ピクセル数)
     *  @param height 画面高さ(ピクセル数)
     */
    public void setRegion(int topX, int topY, int width, int height)
    {
        screenTopX   = topX;
        screenTopY   = topY;
        screenWidth  = width;
        screenHeight = height;
        screenLine   = decideScreenLine(height);
    }

    /**
     *  自分の画面IDを応答する
     *  @return 画面ID
     */    
    public int  getCanvasId()
    {
        return (canvasId);
    }

    /**
     * 左上端X座標を応答する
     * 
     * @return 画面左上X座標
     */
    public int getTopX()
    {
        return (screenTopX);
    }

    /**
     * 左上端Y座標を応答する
     * @return 画面左上Y座標
     */
    public int getTopY()
    {
        return (screenTopY);
    }

    /**
     * 描画ウィンドウ幅を応答する
     * @return 画面描画ウィンドウ幅
     */
    public int getWidth()
    {
        return (screenWidth);
    }

    /**
     * 描画ウィンドウ高さを応答する
     * @return 画面描画ウィンドウ高さ
     */
    public int getHeight()
    {
        return (screenHeight);
    }
    
    /**
     * 表示可能ライン数を求める
     * @param height
     * @return 表示可能ライン数
     */
    private int decideScreenLine(int height)
    {
        return (((height - (screenFont.getHeight() * 1)) / screenFont.getHeight()));
    }

    /**
     * 文字列を（改行コードを解析して）出力する
     * @param g 画面スクリーン
     * @param startLine 表示するラインの先頭行
     * @param offsetX Xのオフセット
     * @param nextOffsetX 改行後のオフセット
     * @param msg 表示する文字列
     * @return 使用したライン数
     */
    protected int drawString(Graphics g, int startLine, int offsetX, int nextOffsetX, String msg)
    {
        // 表示可能行を設定する
        g.setColor(0);
        int line    = 0;
        int y       = screenTopY + (startLine * (screenFont.getHeight() + 1));
        int x       = screenTopX + offsetX;
        int limitX  = screenWidth - screenFont.stringWidth("   ");
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
                x = screenTopX + offsetX + nextOffsetX;
                y = y + screenFont.getHeight() + 1;
                line++;
            }
            if (y >= screenTopY + screenHeight)
            {
                // 描画領域があふれた...終了する
                break;
            }
        }
        return (line + 1);
    }

    /**
     *  画面表示の開始
     * 
     */
    public void start()
    {
        // TODO : 実装すべし！
    }

    /**
     * 画面表示の終了
     * 
     */
    public void stop()
    {
        // TODO : 実装すべし！
        return;
    }

    /**
     * 表示データの更新処理
     * 
     * @param processingId 処理ID
     * 
     */
    public void updateData(int processingId)
    {
        // TODO : 実装すべし！
        return;
    }
    
    /**
     * 画面を表示
     * @param g グラフィック
     * 
     */
    public void paint(Graphics g)
    {
        try
        {
            // タイトル領域の描画
            drawTitleArea(g);

            // メインパネルの描画...
            clearMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (screenHeight - screenFont.getHeight()));
        }
        catch (Exception e)
        {
            // なぜかヌルポが出るので...
        }
        return;
    }

    /**
     * タイトル領域に描画する
     * @param g
     */
    protected void drawTitleArea(Graphics g)
    {
        // タイトル部分の背景を表示する
        g.setColor(baseTitleBackColor);
        g.fillRect(screenTopX, screenTopY, screenWidth, screenFont.getHeight());
        if (titleString != null)
        {
            g.setColor(titleForeColor);
            g.drawString(titleString, screenTopX + 1, screenTopY, (Graphics.LEFT | Graphics.TOP));
        }
        if (titleSubString != null)
        {
            g.setColor(subTitleForeColor);
            int width = (screenTopX + 1) + screenFont.stringWidth(titleString + " ");
            g.drawString(titleSubString, width, screenTopY, (Graphics.LEFT | Graphics.TOP));        	
        }
        return;
    }

    /**
     * タイトルを設定する
     * @param title タイトル
     */
    public void setTitle(String title)
    {
        titleString = title;
        return;
    }    

    /**
     * 情報領域に情報を設定する
     * @param message 表示するメッセージ
     */
    public void setInformation(String message)
    {
        // TODO : 実装すべし！
        return;
    }    
    
    /**
     * 情報領域の情報を更新する
     * @param message 表示するメッセージ
     * @param mode 表示モード
     * 
     */
    public void updateInformation(String message, int mode)
    {
        // TODO : 実装すべし！
        return;        
    }

    /**
     *  処理完了の通知
     *  @param processingId 処理ID
     *  @param result 処理結果
     */
    public void finishUpdateData(int processingId, int result)
    {
        // TODO : 実装すべし！
        return;
    }
    
    /**
     * メインパネル領域をクリアする
     * 
     * @param g 描画領域
     * @param topX 左上X座標
     * @param topY 左上Y座標
     * @param width 幅
     * @param height 高さ
     */
    protected void clearMainPanel(Graphics g, int topX, int topY, int width, int height)
    {
        // 枠を表示する
        g.setColor(baseBackColor);
        g.fillRect(topX, topY, width, height);
        return;
    }

    /**
     * ヒープ情報(空きメモリと全体メモリ容量)の文字列を応答する
     * @return ヒープ情報(文字列)
     */
    protected String getDebugMemoryInfo()
    {
        Runtime run = Runtime.getRuntime();
        return ("Heap : " + run.freeMemory() + " / " + run.totalMemory());
    }

    /**
     *  画面を終了させる
     * 
     */
    protected void finishCanvas()
    {
        // 前画面へ戻る
        parent.previousScene();
        
        return;
    }
    
    /**
     *  キー入力されたときの処理
     *  @param keyCode 入力したキーのコード
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  数字キーが入力されたときの処理
     *  @param number 入力された数字
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     *
     */
    public boolean inputNumber(int number, boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  メニュー表示が指示されたときの処理
     *
     */
    public void showMenu()
    {
        // TODO : 実装すべし！
        return;
    }

    /**
     *  アプリケーションを終了するかどうか決定する<br>
     *  (ソフトキー"X"が押されたときの処理)
     * 
     *  @return  true : アプリケーションを終了させる、false : アプリケーションを終了させない
     */
    public boolean checkExit()
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  画面タッチされたときの処理
     *  @param x x座標
     *  @param y y座標
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean pointerPressed(int x, int y)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  上キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputUp   (boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  下キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputDown (boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  左キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputLeft (boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  右キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputRight(boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  センターキー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputFire (boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (true);
    }

    /**
     *  Game Aキー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputGameA(boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  Game Bキー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputGameB(boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  Game Cキー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputGameC(boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  Game Dキー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputGameD(boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  ＃キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputPound(boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }

    /**
     *  ＊キー操作処理
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        // TODO : 実装すべし！
        return (false);
    }
}
