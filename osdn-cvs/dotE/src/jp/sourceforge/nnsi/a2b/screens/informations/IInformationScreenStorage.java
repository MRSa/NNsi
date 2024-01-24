package jp.sourceforge.nnsi.a2b.screens.informations;


/**
 * ���\����ʂ��g�p����f�[�^�N���X�̃C���^�t�F�[�X
 * 
 * @author MRSa
 *
 */
public interface IInformationScreenStorage
{
    /** �O�̉�ʂɖ߂� */
	static public final int SCENE_TO_PREVIOUS = -1;

	/** �L�����Z���{�^���̃{�^��ID */
    static public final int BUTTON_CANCELED = -1;

    /** OK�{�^���̃{�^��ID */
    static public final int BUTTON_ACCEPTED = 0;

    /**
     *  ��ʕ\�����ׂ����b�Z�[�W�̎擾
     *  
     *  @return �\�����ׂ����b�Z�[�W
     */
	public abstract String getInformationMessageToShow();


    /**
     *   �I�����ꂽ�{�^�����̐ݒ�
     * 
     *   @param buttonId �I�����ꂽ�{�^����ID
     */
    public abstract void setButtonId(int buttonId);


    /**
     *  �{�^�����x�����擾����
     *  
     *  @param buttonId ���x�����擾�������{�^��ID
     *  @return �{�^�����x��
     */
    public abstract String getInformationButtonLabel(int buttonId);

    /**
     *  ���ID���L�[�ɂ��ĕ\������{�^�������擾����
     *  
     *  @param screenId ���ID
     *  @return �\������{�^���̐�
     */
    public abstract int getNumberOfButtons(int screenId);
	
    /**
	 *  ���̉�ʐ؂�ւ�����ID���擾����
	 *  
	 *  @param screenId (���݂�)���ID
	 *  @return �؂�ւ�����ID
	 */
	public abstract int nextSceneToChange(int screenId);
}
