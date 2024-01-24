import jp.sourceforge.nnsi.a2b.utilities.IMidpCyclicTimerReport;
import jp.sourceforge.nnsi.a2b.utilities.MidpCyclicTimer;
import jp.sourceforge.nnsi.a2b.frameworks.IDataProcessing;

/**
 *  メイン画面用操作クラス
 * 
 * @author MRSa
 *
 */
public class fwSampleMainStorage implements ISampleMainCanvasStorage, IMidpCyclicTimerReport
{
    private MidpCyclicTimer myTimer = null;
    private IDataProcessing reportDestination = null;
    private int timerStatus = ISampleMainCanvasStorage.TIMER_READY;
    private int setTime     = 180;  // 設定時間（デフォルトは3分）
    private int currentTime = 0;    // 現在の経過時間
    private int vibrateTime = 500;  // タイムアウト時に振動する時間
    /**
     *  コンストラクタではタイマークラス生成と、タイムアウト発生時の報告先を設定します
     *
     */
    public fwSampleMainStorage(IDataProcessing destination)
    {
        myTimer = new MidpCyclicTimer(this, 1000);        
        myTimer.start();
        reportDestination = destination;
    }

    /**
     *   タイマーがスタートしているかどうかの状態を応答する
     * 
     * @return タイマーの状態
     */
    public int getTimerStatus()
    {
        return (timerStatus);
    }
    
    /**
     *   実行/停止が指示されたときの処理<br>
     * 　<li>停止中 → タイマースタート</li>
     *   <li>開始中 → 一時停止</li>
     *   <li>一時停止中 → タイマー再開</li>
     *   <li>終了中 → 停止中</li>
     * 
     * @return <code>true</code>:指示受付 / <code>false</code>:指示できる状態ではない
     */
    public boolean triggered()
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_READY)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_STARTED;
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_PAUSED;
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_PAUSED)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_STARTED;
        }
        else // if (timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_READY;
        }
        return (true);
    }
    
    /**
     *   タイマーをリセットする<br>
     *   停止中、あるいは終了中に受け付け可能。
     * 
     * @return <code>true</code>:指示受付 / <code>false</code>:指示できる状態ではない
     */    
    public boolean reset()
    {
        boolean ret = false;
        if ((timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)||
            (timerStatus == ISampleMainCanvasStorage.TIMER_READY)||
            (timerStatus == ISampleMainCanvasStorage.TIMER_PAUSED))
        {
            currentTime = 0;
            timerStatus = ISampleMainCanvasStorage.TIMER_READY;
            ret = true;
        }
        return (ret);
    }

    /**
     *  タイムアウト設定秒数を応答する
     */
    public int getSetTimeSeconds()
    {
        return (setTime);
    }

    /**
     *   タイムアウトするまでの残り秒数を応答する
     * @return 残り秒数、負のときはタイムアウト発生中
     */
    public int getRestSeconds()
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)
        {
            return (-1);
        }
        return (setTime - currentTime);
    }

    /**
     *  タイムアウト受信時の処理
     *  @return タイムアウト監視を継続する
     */
    public boolean reportCyclicTimeout(int count)
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            currentTime++;
            if (currentTime >= setTime)
            {
                // タイムアウト検出！！
                timerStatus = ISampleMainCanvasStorage.TIMER_TIMEOUT;
                
                // タイムアウトを報告する！
                reportDestination.finishUpdateData(0, ISampleMainCanvasStorage.TIMER_TIMEOUT, vibrateTime);
            }
            else
            {
                // 画面を更新する
                reportDestination.updateData(0);
            }
        }
        return (true);
    }
}
