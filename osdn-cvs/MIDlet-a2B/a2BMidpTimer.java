/**
 * タイマークラス
 * 
 * @author MRSa
 */
public class a2BMidpTimer
{
    private Ia2BMidpTimerTimeout parent         = null;  // 親クラス

    /**
     * コンストラクタ
     * @param parent  タイムアウト報告先
     */
    public a2BMidpTimer(Ia2BMidpTimerTimeout object)
    {
    	parent  = object;
    }

    /**
     *   タイムアウト監視の開始
     *
     *   @param interval 監視間隔(単位：ms)
     */
    public void startWatchDog(long interval)
    {
        try
        {
        	a2BTimerMain timerThread = new a2BTimerMain(interval);    	
        	timerThread.start();
        }
        catch (Exception e)
        {
            // 何もしない...
        }
        return;
    }

    /**
     *   タイマクラス
     * @author MRSa
     *
     */
    public class a2BTimerMain extends Thread
    {
    	public long interval = 0;
    	
    	/**
    	 *  コンストラクタ
    	 *
    	 */
    	public a2BTimerMain(long duration)
    	{
    		interval = duration;
    	}
    	
    	/**
    	 *   実行...
    	 */
        public void run()
        {
            try
            {
                if (interval > 0)
                {
                    // 待つ...
                    Thread.sleep(interval);
                }
            }
            catch (Exception e)
            {
                // ここでは何もしない
            }

            // タイムアウト報告処理
            parent.detectTimeout(Ia2BMidpTimerTimeout.TIMER_STOP);
            return;
        }
    }
}
