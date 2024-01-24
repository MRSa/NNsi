package jp.sourceforge.nnsi.a2b.utilities;

/**
 *  �����^�C�}�[�N���X<br>
 *  �w�肵�������Ń^�C���A�E�g��񍐂��܂�
 * 
 * @author MRSa
 */
public class MidpCyclicTimer
{
	private IMidpCyclicTimerReport reportDestination = null;  // �񍐐�
    private int                    timeoutCount    = -1;      // �^�C���A�E�g�񍐉�
	private long                   sleepInterval   = 0;       // �Ď��Ԋu(ms)
	private boolean               timerIsStarted  = false;

    /**
     * �V�[���Z���N�^���L�����܂�
     * @param object  �^�C���A�E�g�񍐐�
     * @param duration �^�C���A�E�g�Ԋu(�P��:ms)
     */
    public MidpCyclicTimer(IMidpCyclicTimerReport object, long duration)
    {
    	reportDestination  = object;
    	sleepInterval = duration;
    }

    /**
     *  �^�C���A�E�g�ʒm�̊J�n
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
            		// �^�C�}�[�J�n��...�I������
            		return;
            	}
                if (sleepInterval <= 0)
                {
                    // �C���^�[�o���w�肪�Ȃ������A�Ď��͍s��Ȃ�
                	reportDestination.reportCyclicTimeout(timeoutCount);
                    return;
                }
                timerIsStarted = true;
                boolean timerExtended = true;
                while (timerExtended == true)
                {
                    try
                    {
                        // �҂�...
                        Thread.sleep(sleepInterval);
                    }
                    catch (Exception e)
                    {
                        // �����ł͉������Ȃ�
                    }

                    // �^�C���A�E�g��
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
