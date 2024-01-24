import jp.sourceforge.nnsi.a2b.utilities.IMidpCyclicTimerReport;
import jp.sourceforge.nnsi.a2b.utilities.MidpCyclicTimer;
import jp.sourceforge.nnsi.a2b.frameworks.IDataProcessing;

/**
 *  ���C����ʗp����N���X
 * 
 * @author MRSa
 *
 */
public class fwSampleMainStorage implements ISampleMainCanvasStorage, IMidpCyclicTimerReport
{
    private MidpCyclicTimer myTimer = null;
    private IDataProcessing reportDestination = null;
    private int timerStatus = ISampleMainCanvasStorage.TIMER_READY;
    private int setTime     = 180;  // �ݒ莞�ԁi�f�t�H���g��3���j
    private int currentTime = 0;    // ���݂̌o�ߎ���
    private int vibrateTime = 500;  // �^�C���A�E�g���ɐU�����鎞��
    /**
     *  �R���X�g���N�^�ł̓^�C�}�[�N���X�����ƁA�^�C���A�E�g�������̕񍐐��ݒ肵�܂�
     *
     */
    public fwSampleMainStorage(IDataProcessing destination)
    {
        myTimer = new MidpCyclicTimer(this, 1000);        
        myTimer.start();
        reportDestination = destination;
    }

    /**
     *   �^�C�}�[���X�^�[�g���Ă��邩�ǂ����̏�Ԃ���������
     * 
     * @return �^�C�}�[�̏��
     */
    public int getTimerStatus()
    {
        return (timerStatus);
    }
    
    /**
     *   ���s/��~���w�����ꂽ�Ƃ��̏���<br>
     * �@<li>��~�� �� �^�C�}�[�X�^�[�g</li>
     *   <li>�J�n�� �� �ꎞ��~</li>
     *   <li>�ꎞ��~�� �� �^�C�}�[�ĊJ</li>
     *   <li>�I���� �� ��~��</li>
     * 
     * @return <code>true</code>:�w����t / <code>false</code>:�w���ł����Ԃł͂Ȃ�
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
     *   �^�C�}�[�����Z�b�g����<br>
     *   ��~���A���邢�͏I�����Ɏ󂯕t���\�B
     * 
     * @return <code>true</code>:�w����t / <code>false</code>:�w���ł����Ԃł͂Ȃ�
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
     *  �^�C���A�E�g�ݒ�b������������
     */
    public int getSetTimeSeconds()
    {
        return (setTime);
    }

    /**
     *   �^�C���A�E�g����܂ł̎c��b������������
     * @return �c��b���A���̂Ƃ��̓^�C���A�E�g������
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
     *  �^�C���A�E�g��M���̏���
     *  @return �^�C���A�E�g�Ď����p������
     */
    public boolean reportCyclicTimeout(int count)
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            currentTime++;
            if (currentTime >= setTime)
            {
                // �^�C���A�E�g���o�I�I
                timerStatus = ISampleMainCanvasStorage.TIMER_TIMEOUT;
                
                // �^�C���A�E�g��񍐂���I
                reportDestination.finishUpdateData(0, ISampleMainCanvasStorage.TIMER_TIMEOUT, vibrateTime);
            }
            else
            {
                // ��ʂ��X�V����
                reportDestination.updateData(0);
            }
        }
        return (true);
    }
}
