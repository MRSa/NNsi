package jp.sourceforge.nnsi.a2b.frameworks;
import javax.microedition.lcdui.Displayable;

/**
 * ��ʁi�Ƒ���́j�N���X�̃f�[�^�x�[�X<br>
 * (�C���^�t�F�[�X�N���X) 
 *
 * @author MRSa
 */
public interface IMidpSceneDB
{
    /** �X�N���[���Ȃ� */
    static public  final int NOSCREEN         = -1; // �X�N���[���Ȃ�

    /** ���\����� */
    static public  final int INFORM_SCREEN    =  0; // ���\�����

    /** �I�����\����� */
    static public  final int SELECTION_SCREEN =  1; // �I�����\�����

    /**
     *  �N���X�̏���
     */
    public abstract void prepare();

    /**
     *  �X�N���[���I�u�W�F�N�g�̐ݒ�
     * 
     *  @param baseCanvas �x�[�X�L�����o�X
     */
    public abstract void setScreenObjects(MidpCanvas baseCanvas);

    /**
     *  �f�t�H���g�̉�ʂ���������
     * 
     *  @return �f�t�H���g�̉��ID
     */
    public abstract int getDefaultScene();

    /**
     *  �X�N���[���\����ʃN���X��ݒ肷��
     *  @param screen �X�N���[���\����ʃN���X
     */
    public abstract void setCanvas(IMidpScreenCanvas screen);

    /**
     *  �f�[�^���͗p�N���X��ݒ肷��
     * 
     *  @param form �f�[�^���͗p�N���X
     */
    public abstract void setForm(IMidpForm form);
    
    /**
     *  ��ʐ؂�ւ��̉۔��f
     *  
     *  @param next �؂�ւ�����ID
     *  @param current ���݂̉��ID
     *  @return �؂�ւ���(<code>true</code>) / �؂�ւ��Ȃ�(<code>false</code>)
     */
    public abstract boolean isAvailableChangeScene(int next, int current);

    /**
     *  ��ʂ̐؂�ւ����s�O�̏���
     * 
     *  @param next �؂�ւ�����ID
     *  @param current ���݂̉��ID
     *  @param title �^�C�g��
     *  @return ���s����(<code>true</code>) / ���s���s(<code>false</code>)
     */
    public abstract boolean changeScene(int next, int current, String title);

    /**
     * ��ʐ؂�ւ�������̏���
     * @param current �ؑ֌�̉��ID
     * @param previous �֑ؑO�̉��ID
     */
    public abstract void    sceneChanged(int current, int previous);

    /**
     *  ��ʂɊւ�镶����̎擾
     * 
     *  @param scene ���ID
     *  @return ��ʂɂ�����镶����
     */
    public abstract String  getInfoMessage(int scene);

    /**
     *  �\���N���X�̎擾
     * 
     *  @param scene ���ID
     *  @return ��ʕ\���N���X
     */
    public abstract Displayable getScreen(int scene);
}
