import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;

/**
 *  �X���ꗗ�I����ʗp�X�g���[�W
 * 
 * @author MRSa
 *
 */
public class A2chSListStorage implements ISelectionScreenStorage
{
    private A2chSSceneStorage sceneStorageDb = null;
	private BusyConfirmationStorage busyStorageDb = null;
	
	private A2chSSceneStorage.a2chSThreadInformation selectedThread = null;
	private boolean isShowBusy = true;

	/**
     *  �R���X�g���N�^
     * 
     *
     */
	public A2chSListStorage(A2chSSceneStorage sceneStorage, BusyConfirmationStorage busyStorage)
    {
        sceneStorageDb = sceneStorage;
		busyStorageDb = busyStorage;
    }
	
    /**	
	 *  �I�����̐����擾����
	 *  
	 */
    public int numberOfSelections()
    {
        return (sceneStorageDb.getNumberOfThreadTitles());
    }

    /**
     *  �I�����̕�������擾����
     *  
     */
    public String getSelectionItem(int itemNumber)
    {
    	return ((sceneStorageDb.getThreadInformation(itemNumber)).getTitle());
    }

    /**
     *  �A�C�e���I���𕡐������ɉ\���ǂ�������������
     *  
     */
    public boolean isSelectedItemMulti()
    {
    	return (false);
    }

    /**
     *  �I�������A�C�e�����N���A����
     *
     */
    public void resetSelectionItems()
    {
    	selectedThread = null;
    	return;
    }

    /**
     *  �I�𒆂̃A�C�e��������������
     *  
     */
    public int getNumberOfSelectedItems()
    {
        return (0);
    }

    /**
     *  �I�𒆂̂̃A�C�e��ID����������
     *  
     */
    public int getSelectedItem(int itemNumber)
    {
    	return (ISelectionScreenStorage.BUTTON_CANCELED);
    }

    /**
     *  �I�������A�C�e����ݒ肷��
     *  
     */
    public void setSelectedItem(int itemId)
    {
        selectedThread = sceneStorageDb.getThreadInformation(itemId);
    	return;
    }

    /**
     *  �I�����ꂽ�{�^��ID��ݒ肷��
     *  
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            // �L�����Z�����ꂽ...�I������
        	return (true);
        }    	
    	if (selectedThread == null)
        {
        	return (false);
        }
        boolean ret = false;

        sceneStorageDb.setInformationMessage("�擾�� : " + selectedThread.getTitle());
        Thread thread = new Thread()
        {
        	public void run()
        	{
                // ���O���擾����
        		sceneStorageDb.getDatFile(selectedThread);
        		sceneStorageDb.setInformationMessage(null);
        	}
        };
        try
        {
            // �擾���I������܂ő҂�...
        	thread.start();
//            thread.join();
            ret = true;
        }
        catch (Exception ex)
        {
        	ret = false;
        }        
    	return (ret);
    }

    /**
     *  �I��p�̃K�C�h��������擾����
     *  
     */
	public String getSelectionGuidance()
	{
		return ("�X���I����dat�擾(�ʐM)");
	}

	/**
     *  �^�C�g����\�����邩�ǂ����H
     *  
     */
	public boolean isShowTitle()
	{
		return (true);
	}

	/**
	 *   start���A�A�C�e�������������邩�H
	 * 
	 */
	public boolean isItemInitialize()
	{
		if (isShowBusy == false)
		{
			return (true);
		}
		// BUSY�E�B���h�E��\�����Ė߂��Ă����Ƃ��ɂ́A�A�C�e�������������Ȃ�
        isShowBusy = false;
        return (false);
	}

	/**
	 *  �{�^���̐����擾����
	 *  
	 */
	public int getNumberOfSelectionButtons()
	{
        return (1);
	}

	/**
     *  �{�^���̃��x�����擾����
     *  
     */
	public String getSelectionButtonLabel(int buttonId)
	{
		return ("�擾");
	}
	
    /**
	 *  �w�i���݂��Ⴂ�̐F�ŕ\�����邩�H
	 *  
	 */
	public boolean getSelectionBackColorMode()
	{
		return (true);
	}

	/**
     *  ���j���[�{�^���������ꂽ�^�C�~���O��ʒm����
     *  
     */
	public boolean triggeredMenu()
	{
        if (selectedThread == null)
        {
        	return (false);
        }
        String threadTitle =  selectedThread.getTitle();
        String threadUrl   =  selectedThread.getUrl();
        String title = "�^�C�g��:\r\n  " + threadTitle + "\r\n\r\nURL: \r\n  " + threadUrl  + "\r\n\r\n";

		// �r�W�[�E�B���h�E�̕\��...(�X���^�C�̑S���\���p...)
		busyStorageDb.showMessage(title, BusyConfirmationStorage.INFORMATION_MODE, A2chSSceneDB.LIST_SCREEN);

        isShowBusy = true;
		return (true);
    }
	
    /**
     *  ����ʐ؂�ւ������������
     * 
     */
	public int nextSceneToChange(int screenId)
	{
		if (isShowBusy == true)
		{
			int ret = A2chSSceneDB.BUSY_SCREEN;
			return (ret);
		}
        return (A2chSSceneDB.DEFAULT_SCREEN);
    }

    /**
     *  �������ʃt�@�C����ǂ݂���ňꗗ�\���̏������s��
     * 
     * @return
     */
	public boolean readSearchResultFile()
	{
		String message = null;
		boolean result = false;

		// �r�W�[�E�B���h�E�̕\��...
		busyStorageDb.outputInformationMessage("�������ʃ`�F�b�N��...", true, true);
		
        // ���Ɍ������ʂ͖���́A���邢�͌������ʃt�@�C�������݂��邩�H
        if (sceneStorageDb.getNumberOfThreadTitles() == 0)
		{
/*
        	if (sceneStorageDb.getUrlToSearch() != null)
        	{
                // �ʐM���܂���...
        		sceneStorageDb.doFind2chThreads();
        	}
*/
        	if (sceneStorageDb.isExistSearchResultFile() == true)
        	{
        	    // �t�@�C����ǂݍ���Ńf�[�^��W�J����
        	    result = sceneStorageDb.prepareSearchResultFile();
        	    if (result == false)
        	    {
        	    	message = "(2ch�����q�b�g�Ȃ�)";
        	    }
        	}
        	else
        	{
        		message = "(2ch���������s)";
        	}
        }
        else
        {
            // ���Ɍ����ς�...�č\�z�͍s��Ȃ�
        	result = true;
        }

        // ���E�B���h�E�Ƀ��b�Z�[�W������
        if (message != null)
        {
        	sceneStorageDb.setInformationMessage(message);
        }
        selectedThread = null;
        
        // �r�W�[�E�B���h�E�̃N���[�Y
		busyStorageDb.clearInformationMessage();
		busyStorageDb.finishInformationMessage();
		return (result);
	}
}
