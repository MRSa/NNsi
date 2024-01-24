package jp.sourceforge.nnsi.a2b.frameworks;
import java.util.Date;

/**
 * タイマークラス<br>
 * タイムアウトを検出し、シーンセレクタに報告します
 * 
 * @author MRSa
 */
public class MidpTimer
{
    /** タイマー停止状態 */
    static public final int TIMER_STOP   = -1; // タイマーの終了
 
    /** タイマーの一時停止状態 */
    static public final int TIMER_PAUSE  = -2; // タイマーの一時停止

    /** タイマー延長マーク */
    static public final int TIMER_EXTEND = 0;  // タイマーの延長

    private MidpSceneSelector parent          = null;  // 親クラス
    private long              sleepInterval   = 0;     // 監視間隔(ms)
    private long              watchDogMarking = TIMER_STOP;  // 監視用

    /**
     * シーンセレクタを記憶します
     * @param object  タイムアウト報告先(シーンセレクタ)
     */
    public MidpTimer(MidpSceneSelector object)
    {
        parent  = object;
    }

    /**
     *   タイムアウト監視の開始と通知
     *
     *   @param interval 監視間隔(単位：ms)
     */
    public void startWatchDog(long interval)
    {
        // sleepInterval = interval * 1000 * 60;  // 単位：分の場合...
        sleepInterval = interval;  // 単位：msの場合...
        Thread thread = new Thread()
        {
            public void run()
            {
                Date date = new Date();
                if (sleepInterval == 0)
                {
                    // インターバル指定がなかった、監視は行わない
                    parent.detectTimeout(false);
                    return;
                }

                // 「止まれ」と言われるまで回りつづける...
                watchDogMarking = TIMER_EXTEND;
                while (watchDogMarking != TIMER_STOP)
                {
                    long time       = date.getTime();
                    if (watchDogMarking != TIMER_PAUSE)
                    {
                        watchDogMarking = time;
                    }
                    try
                    {
                        // 待つ...
                        Thread.sleep(sleepInterval);
                    }
                    catch (Exception e)
                    {
                        // ここでは何もしない
                    }
                    if (time == watchDogMarking)
                    {
                        // 待っている間変化がなかった、、、
                        // タイムアウトの検出、ウォッチドックを停止させる
                        parent.detectTimeout(true);
                        watchDogMarking = TIMER_STOP;
                        return;
                    }
                }
                // タイムアウトを検出せずに終了する...
                parent.detectTimeout(false);
            }
        };
        thread.start();
        return;
    }

    /**
     *  タイマーを制御する。
     *  （操作を実行したときに呼び出してください）
     *
     *  @param number タイマー更新の値
     */
    public void extendTimer(int number)
    {
        watchDogMarking = number;
        return;
    }
}
