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
    private String message = "";  // �\�����郁�b�Z�[�W��ݒ肷��

    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public fwSampleMainCanvas(int screenId, MidpSceneSelector object)
    {
        super(screenId, object); 
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     */
    public void start()
    {
        // �^�C�g���������ݒ肷��
        titleString = "�T���v���A�v��!!";

        // �\�����b�Z�[�W��ݒ肷��
        message = "Hello World!";
        return;
    }

    /**
     *  ��ʂ�\��
     *  @param g �O���t�B�b�N�N���X
     */
    public void paint(Graphics g)
    {
        // �O���t�B�b�N��Ԃ̊m�F
        if (g == null)
        {
            return;
        }

        // �^�C�g���Ɖ�ʑS�̂̃N���A�͐e�i�p�����j�̋@�\���g��
        super.paint(g);
        
        g.setColor(0x00000000);  // ���F

        // ������̕`�� �i�P�s�ڂ̓^�C�g���s�̂��߁A���̎��̍s�ɕ������\������j
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));

        return;
    }
}
