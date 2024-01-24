package jp.sourceforge.nnsi.a2b.screens.selections;


/**
 * �I����ʗp�̑���N���X<br>
 * ISelectionScreenStorage�N���X����f�[�^�����炢�ASelectionScreen
 * (���)�ɕ\��������։��H���܂��B
 * @see ISelectionScreenStorage
 * @see SelectionScreen
 * 
 * @author MRSa
 */
public class SelectionScreenOperator
{
    static public final int BACKCOLOR_EVEN = 0x00d8d8ff;
    static public final int BACKCOLOR_ODD  = 0x00f8f8f8;
	static public final int WIDTH_MARGIN = 3;
    static public final int HEIGHT_MARGIN = 0;
	private ISelectionScreenStorage storage = null;

    /**
     * �R���X�g���N�^
     * 
     * @param object �I����ʗp�L���N���X
     */
    public SelectionScreenOperator(ISelectionScreenStorage object)
    {
        storage = object;
    }

    /**
     *  �N���X�̎��s�����i�Ȃɂ����Ȃ��j
     */
    public void prepare()
    {
        //
    }

    /**
     * �\������A�C�e�������擾����
     * 
     * @return �\������A�C�e����
     */
    public int getSelectionItemCount()
    {
        return (storage.numberOfSelections());
    }
    
    /**
     *  ���b�Z�[�W�f�[�^�̎擾
     *  
     *  @param itemNumber �A�C�e���ԍ�(0�X�^�[�g)
     *  @return �\�����ׂ����b�Z�[�W
     */
    public String getSelectionItem(int itemNumber)
    {
        String item = storage.getSelectionItem(itemNumber);
        if (item == null)
        {
        	return ("");
        }
    	return (item);
    }

    /**
     *  �A�C�e���𕡐��I�����邱�Ƃ��\��
     *  
     *  @return �����I���\(true) / �����I��s�\(false)
     */
    public boolean isSelectedItemMulti()
    {
        return (storage.isSelectedItemMulti());
    }

    /**
     *  �A�C�e����I�������Ƃ�
     *  
     *  @param  itemId �I�������A�C�e����ID
     */
    public void selectedItem(int itemId)
    {
        storage.setSelectedItem(itemId);
        System.gc();
        return;
    }

    /**
     *   �I�����Ă���A�C�e���̐�����������
     * 
     * @return �I�����Ă���A�C�e���̐�
     */
    public int getNumberOfSelectedItems()
    {
        return (storage.getNumberOfSelectedItems());
    }

    /**
     *   �I�������A�C�e����ID����������
     * 
     * @param itemIndex �I�������A�C�e���̃C���f�b�N�X�ԍ�
     * @return �I�������A�C�e����ID
     */
    public int getSelectedItem(int itemIndex)
    {
    	return (storage.getSelectedItem(itemIndex));
    }    
    
    /**
     *  �R�}���h�{�^���̓���
     *  
     *  @param  buttonId �����ꂽ�{�^��
     *  @return �o�^OK
     */
    public boolean selectedCommandButton(int buttonId)
    {
        boolean ret = storage.setSelectedCommandButtonId(buttonId);
        System.gc();
        return (ret);
    }

    /**
     *  ���j���[�{�^���������ꂽ���Ƃ�ʒm����
     * 
     * @return ��������������(true) / �������Ȃ�����(false)
     */
    public boolean triggeredMenu()
    {
    	return (storage.triggeredMenu());
    }

    /**
     * �I�����p�̃K�C�h��������擾����
     * 
     * @return �I��p�K�C�h������
     */
    public String getGuideMessage()
    {
    	return (storage.getSelectionGuidance());
    }

    /**
     *   �^�C�g����\�����邩�ǂ���
     * 
     * @return �\������(true) / �\�����Ȃ�(false)
     */
    public boolean isShowTitle()
    {
        return (storage.isShowTitle());
    }

    /**
     *  start���A�A�C�e�������������邩�H
     * @return ����������(true) / ���������Ȃ�(false)
     */
    public boolean isItemInitialize()
    {
    	return (storage.isItemInitialize());
    }
    
    /**
     * ��ʏ�ɗp�ӂ���{�^�������擾����
     * 
     * @return �p�ӂ���{�^����
     */
    public int getNumberOfButtons()
    {
    	return (storage.getNumberOfSelectionButtons());
    }
    
    /**
     * ��ʏ�ɗp�ӂ���{�^�����x�����擾����
     * 
     * @param buttonId �擾����{�^����ID
     * @return �{�^�����x��
     * 
     */
    public String getButtonLabel(int buttonId)
    {
    	return (storage.getSelectionButtonLabel(buttonId));
    }

    /**
	 * �I�����̔w�i�F���擾����
	 * 
	 * @param line �s�ԍ�
	 * @return �w�i�F
	 * 
	 */
	public int getSelectionBackColor(int line)
	{
        if ((storage.getSelectionBackColorMode() == true)&&((line % 2) == 0))
        {
            return (BACKCOLOR_EVEN);
        }
        return (BACKCOLOR_ODD);
	}

	/**
     *   ���̉�ʂ��ǂ��ɐ؂�ւ��邩���߂�
     * 
     * @param sceneId ���݂̉��ID
     * @return ���̉��ID
     */
    public int nextSceneToChange(int sceneId)
    {
        return (storage.nextSceneToChange(sceneId));
    }
}
