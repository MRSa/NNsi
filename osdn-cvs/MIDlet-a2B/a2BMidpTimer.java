/**
 * �^�C�}�[�N���X
 * 
 * @author MRSa
 */
public class a2BMidpTimer
{
    private Ia2BMidpTimerTimeout parent         = null;  // �e�N���X

    /**
     * �R���X�g���N�^
     * @param parent  �^�C���A�E�g�񍐐�
     */
    public a2BMidpTimer(Ia2BMidpTimerTimeout object)
    {
    	parent  = object;
    }

    /**
     *   �^�C���A�E�g�Ď��̊J�n
     *
     *   @param interval �Ď��Ԋu(�P�ʁFms)
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
            // �������Ȃ�...
        }
        return;
    }

    /**
     *   �^�C�}�N���X
     * @author MRSa
     *
     */
    public class a2BTimerMain extends Thread
    {
    	public long interval = 0;
    	
    	/**
    	 *  �R���X�g���N�^
    	 *
    	 */
    	public a2BTimerMain(long duration)
    	{
    		interval = duration;
    	}
    	
    	/**
    	 *   ���s...
    	 */
        public void run()
        {
            try
            {
                if (interval > 0)
                {
                    // �҂�...
                    Thread.sleep(interval);
                }
            }
            catch (Exception e)
            {
                // �����ł͉������Ȃ�
            }

            // �^�C���A�E�g�񍐏���
            parent.detectTimeout(Ia2BMidpTimerTimeout.TIMER_STOP);
            return;
        }
    }
}
