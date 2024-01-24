package jp.sourceforge.nnsi.a2b.screens.informations;

import jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;

/**
 *   �r�W�[�\���i���쒆�̏��\���j���s������A���s���J�n���Ă悢���ǂ�����
 *   �m�F����Ƃ����p�\�ȉ�ʂ̃f�[�^�ێ������ł��B
 *   (InformationScreen ����� InformationScreenOperator �ƂƂ��ɗ��p���܂��B)
 *   <br>
 *   ���̃N���X�ɕ\����������������Z�b�g����ʂ�؂�ւ��Ă�邱�ƂŁA�r�W�[
 *   �\����m�F�_�C�A���O�̕\�����������邱�Ƃ��ł��܂��B
 *   <br>
 *   �r�W�[�\�����s�����߂� ILogMessageDisplay�C���^�t�F�[�X���������Ă��܂��B
 *   �ڍׂ́A
 *   {@link jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay}
 *   ���Q�l�ɂ����p���������B
 *   <br>
 *   �Ȃ��AInformationScreenOperator �����̃N���X�𗘗p���邽�߂�
 *   {@link jp.sourceforge.nnsi.a2b.screens.informations.IInformationScreenStorage}
 *   ���������Ă��܂��B
 *   <br>
 *   <em>(���[�U�́A IInformationScreenStorage�̃C���^�t�F�[�X��
 *   ���p����K�v�͂���܂���B)</em>
 *   
 * @author MRSa
 *
 */
public class BusyConfirmationStorage implements IInformationScreenStorage, ILogMessageDisplay
{
    /** �r�W�[���[�h (�E�B���h�E�{�^������0�̏ꍇ) */
	static public final int BUSY_MODE = 0;

	/** ���\�����[�h (�E�B���h�E�{�^�������uOK�v�{�^��1�̏ꍇ) */
    static public final int INFORMATION_MODE = 1;   // 
    
    /** �m�F���[�h (�E�B���h�E�{�^������2��(�uOK�v�ƁuCancel�v)�̏ꍇ) */
    static public final int CONFIRMATION_MODE = 2; 

    private IMidpScreenCanvas view    = null; 
    private boolean isButtonAccepted = true;
    private int   confirmationMode    = INFORMATION_MODE;
    private int   returnScreen        = -1;
	private String dialogMessage       = null;
	
	/**
	 *  �R���X�g���N�^�ł̏����͂���܂���B<br>
	 *  <em>(�K�v�ȏ����������́Aprepare()�Ŏ��s���܂��B)</em>
	 */
    public BusyConfirmationStorage()
    {
        // �������Ȃ�
     }

    /**
     *  <b>�N���X�̎g�p�������s��</b>
     *  
     * �@@param infoScreen �y�A�ŗ��p����InformationScreen�N���X
     *   @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreen
     * 
     */
    public void prepare(IMidpScreenCanvas infoScreen)
    {
    	view = infoScreen;
    }
    
    /**
     *  <b>���b�Z�[�W����ʕ\�����鏀��������</b>
     *  ��ʑJ�ڂ͔��������Ȃ��B
     * 
     * @param message �\�����郁�b�Z�[�W
     * @param mode    ��ʃ��[�h BUSY_MODE / INFORMATION_MODE / CONFIRMATION_MODE
     * @param returnScene ��ʂ�����Ƃ��ɊJ�����̉��ID
     */
    public void prepareMessage(String message, int mode, int returnScene)
    {
        dialogMessage = message;
        confirmationMode = mode;
        returnScreen = returnScene;
    }

    /**
     *  <b>���b�Z�[�W����ʕ\������</b>
     * 
     * @param message �\�����郁�b�Z�[�W
     * @param mode    ��ʃ��[�h BUSY_MODE / INFORMATION_MODE / CONFIRMATION_MODE
     * @param returnScene ��ʂ�����Ƃ��ɊJ�����̉��ID
     */
    public void showMessage(String message, int mode, int returnScene)
    {
        dialogMessage = message;
        confirmationMode = mode;
        returnScreen = returnScene;
        view.inputFire(false);    // �_�~�[�̉�ʑJ�ڂ𔭐�������
    }

    /**
     *  <b>OK�{�^���������ꂽ���ǂ������m�F����</b>
     * 
     * @return true:OK�{�^���������ꂽ
     */
    public boolean isAccepted()
    {
    	return (isButtonAccepted);
    }

    /**
	 *  �\�����ׂ����b�Z�[�W���擾
	 *  <em>(InformationScreenOperator���g�p���܂��B)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
	 *  
	 *  @return �\�����ׂ����b�Z�[�W
	 */
	public String getInformationMessageToShow()
	{
		return (dialogMessage);
	}

    /** 
     *  �I�����ꂽ�{�^����ݒ肷��
	 *  <em>(InformationScreenOperator���g�p���܂��B)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
     *  
     *  @param buttonId �I�����ꂽ�{�^��
     */
    public void setButtonId(int buttonId)
    {
    	if (buttonId == BUTTON_ACCEPTED)
    	{
    		isButtonAccepted = true;
    	}
    	else
    	{
    		isButtonAccepted = false;
    	}
    	return;
    }

    /**
	 *  �{�^�����x�����擾
	 *  <em>(InformationScreenOperator���g�p���܂��B)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
	 *  
	 *  @param  buttonId �{�^����ID
	 *  @return �{�^�����x��
	 */
    public String getInformationButtonLabel(int buttonId)
    {
        if (buttonId == BUTTON_ACCEPTED)
        {
        	return ("OK");
        }
        return ("Cancel");
    }

    /**
     *  �\������{�^�������擾����
	 *  <em>(InformationScreenOperator���g�p���܂��B)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
	 *  
     *  @return �\������{�^����
     */
    public int getNumberOfButtons(int screenId)
    {
        return (confirmationMode);
    }

    /**
	 *  ���̉�ʐ؂�ւ���̎w��
	 *  <em>(InformationScreenOperator���g�p���܂��B)</em>
	 *  @see jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator
	 *   
	 *  @param screenId ���݂̉��ID
	 *  @return ���ɑJ�ڂ�����ID
	 */
	public int nextSceneToChange(int screenId)
	{
        int nextScene = IInformationScreenStorage.SCENE_TO_PREVIOUS;
		if (confirmationMode == BUSY_MODE)
		{
			// ��ʃ��b�N��(��ʑJ�ڂ����Ȃ�)
			return (screenId);
		}
		if (returnScreen != IInformationScreenStorage.SCENE_TO_PREVIOUS)
		{
			nextScene = returnScreen;
		    returnScreen = IInformationScreenStorage.SCENE_TO_PREVIOUS;	
		}
        return (nextScene);
	}

    /**
     *  ���O�̏o�͂̊J�n
	 *  <em>(���O�o�͂��郆�[�e�B���e�B�N���X���g�p���܂��B)</em>
     * 
     * @param message �\�����郁�b�Z�[�W
     * @param forceUpdate �\���������I�ɍX�V���邩�H
     * @param lockOperation �f�[�^�\�����A������֎~�����邩�H
     * @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
     */
    public void outputInformationMessage(String message, boolean forceUpdate, boolean lockOperation)
    {
        dialogMessage = message;

        // ������֎~���邩�ǂ���
        if (lockOperation == true)
        {
        	confirmationMode = BUSY_MODE;
        }
        else
        {
        	confirmationMode = INFORMATION_MODE;
        }
        if (forceUpdate == true)
        {
        	// �_�~�[�̉�ʑJ�ڂ𔭐������ċ����I�ɍX�V
        	view.inputFire(false);
        }
        return;
    }

	/**
	 *  ���O�o�̓��b�Z�[�W�̃N���A
	 *  <em>(���O�o�͂��郆�[�e�B���e�B�N���X���g�p���܂��B)</em>
 	 *  
     *   @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
	 */
    public void clearInformationMessage()
    {
    	dialogMessage = null;
    	isButtonAccepted = true;
    }

    /**
     *  ���O�o�͂̏I��(����֎~������)
 	 *  <em>(���O�o�͂��郆�[�e�B���e�B�N���X���g�p���܂��B)</em>
     *  
     *   @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
     */
    public void finishInformationMessage()
    {
        dialogMessage = null;
    	isButtonAccepted = true;
    	confirmationMode = INFORMATION_MODE;
    	view.inputFire(false);
    }
}
