package jp.sourceforge.nnsi.a2b.screens.informations;

/**
 *  ���\����ʗp�f�[�^�ێ��N���X�̕W�������ł��B
 * 
 * @author MRSa
 *
 */
public class InformationDefaultStorage implements IInformationScreenStorage
{
    /** �\�����ׂ����b�Z�[�W */
	private String showMessage = null;
	
    /**
     *   �R���X�g���N�^�ł́A���ɉ����s���܂���B
     *
     */
	public InformationDefaultStorage()
    {

    }

	/**
	 *  �\�����ׂ����b�Z�[�W��ݒ肷��
	 * @param message �\�����ׂ����b�Z�[�W
	 */
	public void setInformationMessageToShow(String message)
	{
	    showMessage = message;
	}

	/**
	 *  �\�����郁�b�Z�[�W���N���A����
	 *
	 */
	public void clearInformationMessageToShow()
	{
        showMessage = null;
        System.gc();
	}
	
    /**
     *  ��ʕ\�����ׂ����b�Z�[�W�̎擾
     *  
     *  @return �\�����ׂ����b�Z�[�W
     */
	public String getInformationMessageToShow()
	{
        return (showMessage);
	}

    /**
     *   �I�����ꂽ�{�^�����̐ݒ�
     * 
     *   @param buttonId �I�����ꂽ�{�^����ID
     */
    public void setButtonId(int buttonId)
    {
        // �������Ȃ�
    }

    /**
     *  �{�^�����x�����擾����
     *  
     *  @param buttonId ���x�����擾�������{�^��ID
     *  @return �{�^�����x��
     */
    public String getInformationButtonLabel(int buttonId)
    {
        return ("OK");
    }

    /**
     *  ���ID���L�[�ɂ��ĕ\������{�^�������擾����
     *  
     *  @param screenId ���ID
     *  @return �\������{�^���̐�
     */
    public int getNumberOfButtons(int screenId)
    {
        return (1);
    }
	
    /**
	 *  ���̉�ʐ؂�ւ�����ID���擾����
	 *  
	 *  @param screenId (���݂�)���ID
	 *  @return �؂�ւ�����ID
	 */
	public int nextSceneToChange(int screenId)
	{
        return (IInformationScreenStorage.SCENE_TO_PREVIOUS);
	}
}
