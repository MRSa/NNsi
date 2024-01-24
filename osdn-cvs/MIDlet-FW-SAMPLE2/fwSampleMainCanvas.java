import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 *  サンプル画面
 * 
 * @author MRSa
 */
public class fwSampleMainCanvas extends MidpScreenCanvas
{
    ISampleMainCanvasStorage storage = null;
    
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     *  @param data
     */
    public fwSampleMainCanvas(int screenId, MidpSceneSelector object, ISampleMainCanvasStorage data)
    {
        super(screenId, object); 
        storage = data;
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     */
    public void start()
    {
        // タイトル文字列を設定する
        titleString = "キッチンタイマー";
        return;
    }

    /**
     *  画面を描画する
     *  @param g グラフィッククラス
     */
    public void paint(Graphics g)
    {
        // グラフィック状態の確認
        if (g == null)
        {
            // グラフィックがなければ、何もせず終了する
            return;
        }

        // タイトルと画面全体のクリアは親（継承元）の機能を使う
        super.paint(g);
      
        // キッチンタイマーの状態に合わせ、描画内容を振り分ける
        int timerStatus = storage.getTimerStatus();
        if (timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)
        {
            // タイムアウト時の描画
            drawTimeout(g);    
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_PAUSED)
        {
            // 一時停止中のタイミングでの描画
            drawPausing(g);
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            // タイマーが動いているときの描画
            drawWatching(g);
        }
        else // if (timerStatus == ISampleMainCanvasStorage.TIMER_READY)
        {
            // スタート待ちのときの描画
            drawReady(g);
        }
        return;
    }

    /**
     *  「タイムアウト発生中」状態の描画
     * 
     * @param g
     */
    private void drawTimeout(Graphics g)
    {
        String message = "時間になりました！";
        
        // 文字列の描画 （１行目はタイトル行のため、その次の行に文字列を表示する）
        g.setColor(0x00000000);  // 黒色
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));
    }

    /**
     *  「タイマー動作中」状態の描画
     * 
     * @param g
     */
    private void drawWatching(Graphics g)
    {
        String message = "あと" + storage.getRestSeconds() + " 秒です。";
        
        // 文字列の描画 （１行目はタイトル行のため、その次の行に文字列を表示する）
        g.setColor(0x00000000);  // 黒色
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));
    }
    
    /**
     *  「一時停止中」状態の描画
     * 
     * @param g
     */
    private void drawPausing(Graphics g)
    {
        String message = "停止中、センターボタンで再開します。" + " (" + storage.getRestSeconds() + "秒)";
        
        // 文字列の描画 （１行目はタイトル行のため、その次の行に文字列を表示する）
        g.setColor(0x00000000);  // 黒色
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));        
    }
    
    /**
     *  「待機中」状態の描画
     * 
     * @param g
     */
    private void drawReady(Graphics g)
    {
        String message = "センターボタンで開始します。" + "(" + storage.getRestSeconds() + "秒)";

        // 文字列の描画 （１行目はタイトル行のため、その次の行に文字列を表示する）
        g.setColor(0x00000000);  // 黒色
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));        
    }
    
    /**
     *  メニューボタンが押されたとき
     */
    public void showMenu()
    {
        // 拡張予定
        return;
    }

    /**
     *  X ボタンを押したとき：アプリケーションを終了させる<br>
     *  カウンタが止まっているときのみ終了可能とする
     *  @return  <code>true</code>:終了させる / <code>false</code>:終了させない
     */
    public boolean checkExit()
    {
        int status = storage.getTimerStatus();
        if ((status == ISampleMainCanvasStorage.TIMER_PAUSED)||
            (status == ISampleMainCanvasStorage.TIMER_TIMEOUT)||
            (status == ISampleMainCanvasStorage.TIMER_READY))
        {
            // この状態のときには終了させる
            return (true);
        }
        return (false);
    }

    /**
     *  クリアボタンが押されたとき
     */
    public boolean inputGameC(boolean isRepeat)
    {
        storage.reset();
        return (true);
    }

    /**
     *  中ボタンが押されたとき
     */
    public boolean inputFire(boolean isRepeat)
    {
        storage.triggered();
        return (true);
    }
}
