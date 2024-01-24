package jp.sourceforge.nnsi.a2b.frameworks.samples;

import javax.microedition.lcdui.Displayable;

import jp.sourceforge.nnsi.a2b.frameworks.IMidpForm;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;


/**
 *  �V�[���f�[�^�x�[�X(�̃T���v������)
 * 
 * @author MRSa
 *
 */
public class MidpDefaultSceneDB implements IMidpSceneDB
{
    protected MidpCanvas baseScreen = null;
    protected IMidpScreenCanvas screenCanvas = null;
    
    /**
     *  �R���X�g���N�^�ł͉������삵�܂���
     *
     */
    public MidpDefaultSceneDB()
    {
        
    }
    
    /**
     *  �N���X�̏���
     *  
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
        return (IMidpSceneDB.NOSCREEN);
    }

    /**
     *  �X�N���[���\����ʃN���X��ݒ肷��
     *  @param screen �X�N���[���\����ʃN���X
     */
    public void setCanvas(IMidpScreenCanvas screen)
    {
        screenCanvas = screen;
        return;
    }

    /**
     *  �f�[�^���͗p�N���X��ݒ肷��
     *  @param form �f�[�^���̓t�H�[��
     */
    public void setForm(IMidpForm form)
    {
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
     *  ��ʂ̐؂�ւ����s����(�����ŉ�ʂ�؂�ւ��܂�)<br>
     *  (baseMidpCanvas.changeActiveCanvas() ���Ă񂾂�A�Ƃ����s���܂��B)<br>
     *  ��ʂ�؂�ւ���ꍇ�ɂ́A�V�[���Z���N�^�� getScreen()���Ăяo���A
     *  Displayable�N���X(��ʕ\���N���X)���擾���܂��B
     *  
     *  @param next �؂�ւ��������ID
     *  @param current ���݂̉��ID
     *  @param title �V������ʂ̃^�C�g��
     *  @return <code>true</code>:Display.setCurrent()�����s���܂��B<br>
     *   <code>false</code>:Display.setCurrent()�����s���܂���B
     *   (MidpScreenCanvas()�N���X��ؑւ�Ƃ��A�Ƃ��͂�������������Ă��������B)
     */
    public boolean changeScene(int next, int current, String title)
    {
        // �I����ʂ֐؂�ւ���
        baseScreen.changeActiveCanvas(screenCanvas);            
        return (true);
    }

    /**
     *  ��ʐ؂�ւ��������̏���
     *  @param current �ؑ֌�̉��ID
     *  @param previous �֑ؑO�̉��ID
     *  
     */
    public void sceneChanged(int current, int previous)
    {
        return;
    }

    /**
     *  ��ʂɊւ�镶����擾
     *  @param scene ���ID
     *  @return ��ʂɂ�����镶����
     */
    public String  getInfoMessage(int scene)
    {
        return ("DefaultSceneDB");
    }

    /**
     *  �\���N���X�̎擾
     *  @param scene ���ID
     *  @return ��ʕ\���N���X
     */
    public Displayable getScreen(int scene)
    {
        return (baseScreen);
    }
}
