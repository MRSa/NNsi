import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 *  �T���v�����
 * 
 * @author MRSa
 */
public class fwSampleMainCanvas extends MidpScreenCanvas
{
    ISampleMainCanvasStorage storage = null;
    
    /**
     *  �R���X�g���N�^
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
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     */
    public void start()
    {
        // �^�C�g���������ݒ肷��
        titleString = "�L�b�`���^�C�}�[";
        return;
    }

    /**
     *  ��ʂ�`�悷��
     *  @param g �O���t�B�b�N�N���X
     */
    public void paint(Graphics g)
    {
        // �O���t�B�b�N��Ԃ̊m�F
        if (g == null)
        {
            // �O���t�B�b�N���Ȃ���΁A���������I������
            return;
        }

        // �^�C�g���Ɖ�ʑS�̂̃N���A�͐e�i�p�����j�̋@�\���g��
        super.paint(g);
      
        // �L�b�`���^�C�}�[�̏�Ԃɍ��킹�A�`����e��U�蕪����
        int timerStatus = storage.getTimerStatus();
        if (timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)
        {
            // �^�C���A�E�g���̕`��
            drawTimeout(g);    
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_PAUSED)
        {
            // �ꎞ��~���̃^�C�~���O�ł̕`��
            drawPausing(g);
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            // �^�C�}�[�������Ă���Ƃ��̕`��
            drawWatching(g);
        }
        else // if (timerStatus == ISampleMainCanvasStorage.TIMER_READY)
        {
            // �X�^�[�g�҂��̂Ƃ��̕`��
            drawReady(g);
        }
        return;
    }

    /**
     *  �u�^�C���A�E�g�������v��Ԃ̕`��
     * 
     * @param g
     */
    private void drawTimeout(Graphics g)
    {
        String message = "���Ԃł��I";
        
        // ������̕`�� �i�P�s�ڂ̓^�C�g���s�̂��߁A���̎��̍s�ɕ������\������j
        g.setColor(0x00000000);  // ���F
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));
    }

    /**
     *  �u�^�C�}�[���쒆�v��Ԃ̕`��
     * 
     * @param g
     */
    private void drawWatching(Graphics g)
    {
        int restMinutes = storage.getRestSeconds() / 60;
        int restSeconds = storage.getRestSeconds() % 60;
        
        String message = "����" + restMinutes + " �� " + restSeconds + " �b";

        // ������̕`�� �i�P�s�ڂ̓^�C�g���s�̂��߁A���̎��̍s�ɕ������\������j
        g.setColor(0x00000000);  // ���F
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));
    }
    
    /**
     *  �u�ꎞ��~���v��Ԃ̕`��
     * 
     * @param g
     */
    private void drawPausing(Graphics g)
    {
        int restMinutes = storage.getRestSeconds() / 60;
        int restSeconds = storage.getRestSeconds() % 60;
        
        String message = "�Z���^�[�ōĊJ" + "(�c�� "+ restMinutes + " �� " + restSeconds + "�b)";
        
        // ������̕`�� �i�P�s�ڂ̓^�C�g���s�̂��߁A���̎��̍s�ɕ������\������j
        g.setColor(0x00000000);  // ���F
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));        
    }
    
    /**
     *  �u�ҋ@���v��Ԃ̕`��
     * 
     * @param g
     */
    private void drawReady(Graphics g)
    {
        int restMinutes = storage.getRestSeconds() / 60;
        int restSeconds = storage.getRestSeconds() % 60;
        
        String message = "�Z���^�[�ŊJ�n" + "(" + restMinutes + " �� " + restSeconds + "�b)";

        // ������̕`�� �i�P�s�ڂ̓^�C�g���s�̂��߁A���̎��̍s�ɕ������\������j
        g.setColor(0x00000000);  // ���F
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));

    }
    
    /**
     *  ���j���[�{�^���������ꂽ�Ƃ�
     */
    public void showMenu()
    {
        int status = storage.getTimerStatus();
        if ((status == ISampleMainCanvasStorage.TIMER_PAUSED)||
            (status == ISampleMainCanvasStorage.TIMER_TIMEOUT)||
            (status == ISampleMainCanvasStorage.TIMER_READY))
        {
            // �^�C�}�[���~�܂��Ă����Ԃł́A���j���[�\�����w������
            parent.changeScene(fwSampleFactory.SELECTION_SCREEN, "");
        }
        return;
    }

    /**
     *  X �{�^�����������Ƃ��F�A�v���P�[�V�������I��������<br>
     *  �J�E���^���~�܂��Ă���Ƃ��̂ݏI���\�Ƃ���
     *  @return  <code>true</code>:�I�������� / <code>false</code>:�I�������Ȃ�
     */
    public boolean checkExit()
    {
        int status = storage.getTimerStatus();
        if ((status == ISampleMainCanvasStorage.TIMER_PAUSED)||
            (status == ISampleMainCanvasStorage.TIMER_TIMEOUT)||
            (status == ISampleMainCanvasStorage.TIMER_READY))
        {
            // ���̏�Ԃ̂Ƃ��ɂ͏I��������
            return (true);
        }
        return (false);
    }

    /**
     *  �N���A�{�^���������ꂽ�Ƃ�
     */
    public boolean inputGameC(boolean isRepeat)
    {
        storage.reset();
        return (true);
    }

    /**
     *  ���{�^���������ꂽ�Ƃ�
     */
    public boolean inputFire(boolean isRepeat)
    {
        storage.triggered();
        return (true);
    }
}
