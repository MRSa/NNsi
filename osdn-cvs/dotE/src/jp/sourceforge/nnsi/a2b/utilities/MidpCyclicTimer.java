package jp.sourceforge.nnsi.a2b.utilities;

/**
 *  周期タイマークラス<br>
 *  指定した周期でタイムアウトを報告します
 * 
 * @author MRSa
 */
public class MidpCyclicTimer
{
	private IMidpCyclicTimerReport reportDestination = null;  // 報告先
    private int                    timeoutCount    = -1;      // タイムアウト報告回数
	private long                   sleepInterval   = 0;       // 監視間隔(ms)
	private boolean               timerIsStarted  = false;

    /**
     * シーンセレクタを記憶します
     * @param object  タイムアウト報告先
     * @param duration タイムアウト間隔(単位:ms)
     */
    public MidpCyclicTimer(IMidpCyclicTimerReport object, long duration)
    {
    	reportDestination  = object;
    	sleepInterval = duration;
    }

    /**
     *  タイムアウト通知の開始
     *
     */
    public void start()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
            	if (timerIsStarted == true)
            	{
            		// タイマー開始中...終了する
            		return;
            	}
                if (sleepInterval <= 0)
                {
                    // インターバル指定がなかった、監視は行わない
                	reportDestination.reportCyclicTimeout(timeoutCount);
                    return;
                }
                timerIsStarted = true;
                boolean timerExtended = true;
                while (timerExtended == true)
                {
                    try
                    {
                        // 待つ...
                        Thread.sleep(sleepInterval);
                    }
                    catch (Exception e)
                    {
                        // ここでは何もしない
                    }

                    // タイムアウト報告
                    timeoutCount++;
                    timerExtended = reportDestination.reportCyclicTimeout(timeoutCount);
                }
                timerIsStarted = false;
                timeoutCount = -1;
            }
        };
        thread.start();
        return;
    }
}
