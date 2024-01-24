package jp.sourceforge.nnsi.a2b.frameworks;
import java.util.Date;

/**
 * �^�C�}�[�N���X<br>
 * �^�C���A�E�g�����o���A�V�[���Z���N�^�ɕ񍐂��܂�
 * 
 * @author MRSa
 */
public class MidpTimer
{
    /** �^�C�}�[��~��� */
    static public final int TIMER_STOP   = -1; // �^�C�}�[�̏I��
 
    /** �^�C�}�[�̈ꎞ��~��� */
    static public final int TIMER_PAUSE  = -2; // �^�C�}�[�̈ꎞ��~

    /** �^�C�}�[�����}�[�N */
    static public final int TIMER_EXTEND = 0;  // �^�C�}�[�̉���

    private MidpSceneSelector parent          = null;  // �e�N���X
    private long              sleepInterval   = 0;     // �Ď��Ԋu(ms)
    private long              watchDogMarking = TIMER_STOP;  // �Ď��p

    /**
     * �V�[���Z���N�^���L�����܂�
     * @param object  �^�C���A�E�g�񍐐�(�V�[���Z���N�^)
     */
    public MidpTimer(MidpSceneSelector object)
    {
        parent  = object;
    }

    /**
     *   �^�C���A�E�g�Ď��̊J�n�ƒʒm
     *
     *   @param interval �Ď��Ԋu(�P�ʁFms)
     */
    public void startWatchDog(long interval)
    {
        // sleepInterval = interval * 1000 * 60;  // �P�ʁF���̏ꍇ...
        sleepInterval = interval;  // �P�ʁFms�̏ꍇ...
        Thread thread = new Thread()
        {
            public void run()
            {
                Date date = new Date();
                if (sleepInterval == 0)
                {
                    // �C���^�[�o���w�肪�Ȃ������A�Ď��͍s��Ȃ�
                    parent.detectTimeout(false);
                    return;
                }

                // �u�~�܂�v�ƌ�����܂ŉ��Â���...
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
                        // �҂�...
                        Thread.sleep(sleepInterval);
                    }
                    catch (Exception e)
                    {
                        // �����ł͉������Ȃ�
                    }
                    if (time == watchDogMarking)
                    {
                        // �҂��Ă���ԕω����Ȃ������A�A�A
                        // �^�C���A�E�g�̌��o�A�E�H�b�`�h�b�N���~������
                        parent.detectTimeout(true);
                        watchDogMarking = TIMER_STOP;
                        return;
                    }
                }
                // �^�C���A�E�g�����o�����ɏI������...
                parent.detectTimeout(false);
            }
        };
        thread.start();
        return;
    }

    /**
     *  �^�C�}�[�𐧌䂷��B
     *  �i��������s�����Ƃ��ɌĂяo���Ă��������j
     *
     *  @param number �^�C�}�[�X�V�̒l
     */
    public void extendTimer(int number)
    {
        watchDogMarking = number;
        return;
    }
}
