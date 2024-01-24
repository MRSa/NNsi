import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.frameworks.*;

/**
 * �V�[���ؑփN���X
 * @author MRSa
 *
 */
public class fwSampleSceneDB  implements IMidpSceneDB
{
    protected MidpCanvas baseScreen = null;
    protected IMidpForm         inputForm = null; 
    protected IMidpScreenCanvas screenCanvas = null;
    protected IMidpScreenCanvas selectionCanvas = null;

    private boolean isShowForm = false;
    
    /**
     *  �R���X�g���N�^�ł͉������삵�܂���
     */
    public fwSampleSceneDB()
    {
        
    }

    /**
     *  �N���X�̏�������
     */
    public void prepare()
    {
        return;
    }

    /**
     *  �X�N���[���I�u�W�F�N�g�̐ݒ�
     *  @param baseCanvas �x�[�X�L�����o�X�N���X
     */
    public void setScreenObjects(MidpCanvas baseCanvas)
    {
        baseScreen = baseCanvas;
        return;
    }

    /**
     *  �f�t�H���g�̉�ʂ���������
     *  @return �f�t�H���g�̉��ID
     */
    public int getDefaultScene()
    {
        return (fwSampleFactory.DEFAULT_SCREEN);
    }

    /**
     *  �X�N���[���\����ʃN���X��ݒ肷��
     *  @param screen �X�N���[���\����ʃN���X
     */
    public void setCanvas(IMidpScreenCanvas screen)
    {
        if (screen.getCanvasId() == fwSampleFactory.DEFAULT_SCREEN)
        {
            screenCanvas = screen;
        }
        else
        {
            selectionCanvas = screen;
        }
        return;
    }

    /**
     *  �f�[�^���͗p�N���X��ݒ肷��
     *  @param form �f�[�^���̓t�H�[��
     */
    public void setForm(IMidpForm form)
    {
    	inputForm = form;
        return;
    }

    /**
     *  ��ʐ؂�ւ��̉۔��f
     *  @param next �؂�ւ��������ID
     *  @param current ���݂̉��ID
     *  @return true:�ؑ�OK, false:�ؑ�NG
     */
    public boolean isAvailableChangeScene(int next, int current)
    {
        return (true);
    }

    /**
     *  ��ʂ̐؂�ւ����s����
     *  @param next �؂�ւ��������ID
     *  @param current ���݂̉��ID
     *  @param title �V������ʂ̃^�C�g��
     *  @return true:�ؑ�OK, false:�ؑ�NG
     */
    public boolean changeScene(int next, int current, String title)
    {
    	isShowForm = false;
    	if (next == fwSampleFactory.DATAINPUT_SCREEN)
    	{
    		isShowForm = true;
    	}
    	else if (next == fwSampleFactory.SELECTION_SCREEN)
        {
            // �I����ʂ֐؂�ւ���
            baseScreen.changeActiveCanvas(selectionCanvas);            
        }
        else
        {
            // ���C����ʂ֐؂�ւ���
            baseScreen.changeActiveCanvas(screenCanvas);
        }
        return (true);
    }

    /**
     *  ��ʐ؂�ւ��������̏���
     *  @param current �ؑ֌�̉��ID
     *  @param previous �֑ؑO�̉��ID
     */
    public void sceneChanged(int current, int previous)
    {
        if (isShowForm == true)
        {
            inputForm.start();
        }
        return;
    }

    /**
     *  ��ʂɊւ�镶����擾
     *  @param scene ���ID
     *  @return ��ʂɂ�����镶����
     */
    public String  getInfoMessage(int scene)
    {
        return ("");
    }

    /**
     *  �\���N���X�̎擾
     *  @param scene ���ID
     *  @return ��ʕ\���N���X
     */
    public Displayable getScreen(int scene)
    {
    	if (scene == fwSampleFactory.DATAINPUT_SCREEN)
    	{
    		return ((Displayable) inputForm);
        }
        return (baseScreen);
    }
}