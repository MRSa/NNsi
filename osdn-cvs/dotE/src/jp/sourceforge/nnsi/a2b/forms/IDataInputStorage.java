package jp.sourceforge.nnsi.a2b.forms;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;

/**
 * �f�[�^���͗p�̃X�g���[�W<br>
 * (�C���^�t�F�[�X�N���X�j
 * 
 * @author MRSa
 *
 */
public interface IDataInputStorage
{
    /** �O�̉�ʂɖ߂� */
    static public final int SCENE_TO_PREVIOUS = -1;

    /**
     *  �f�[�^���͂��L�����Z�����ꂽ�Ƃ��̏���
     */
    public abstract void cancelEntry();

    /**
     *  �f�[�^���͂�OK���ꂽ�Ƃ��̏���
     */
    public abstract void dataEntry();

    /**
     *  �f�[�^���͂̂��߂̏�������
     */
    public abstract void prepare();

    /**
     *  �f�[�^���͏I������
     */
    public abstract void finish();

    /**
     *  �\������f�[�^���̓t�B�[���h�̌����擾����
     *  
     *  @return �\������f�[�^���̓t�B�[���h�̌�
     */
    public abstract int getNumberOfDataInputField();

    /**
     *  �f�[�^���̓t�B�[���h���擾����
     *  @param index �f�[�^�C���f�b�N�X�ԍ��i�O�X�^�[�g�j
     *  @return �e�L�X�g���̓t�B�[���h
     */
    public abstract TextField getDataInputField(int index);

    /**
     *  �\������I�v�V����(�I����͍���)�̌����擾����
     *  @return �\������I�v�V�����̌� 
     */
    public abstract int getNumberOfOptionGroup();
    
    /**
     *  �\������I�v�V����(�I����͍���)���擾����
     *  @param index �I�v�V�����C���f�b�N�X�ԍ��i�O�X�^�[�g�j
     *  @return �I�v�V����(�I����͍���)
     */
    public abstract ChoiceGroup getOptionField(int index);

    /**
     *  ��ʐ؂�ւ�����擾����
     *  @param screenId ���ID
     *  @return �؂�ւ�����ID
     */
    public abstract int nextSceneToChange(int screenId);

    /**
     *  ��ʐؑ֐�̃^�C�g�����擾����
     *  @param screenId ���ID
     *  @return ��ʃ^�C�g��
     */
    public abstract String nextSceneTitleToChange(int screenId);
}
