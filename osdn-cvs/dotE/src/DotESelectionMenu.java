import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;

public class DotESelectionMenu implements ISelectionScreenStorage
{
	private int currentSelectedItem = -1;
	private int nextSceneId = SCENE_TO_PREVIOUS;
	private BusyConfirmationStorage confirmStorage = null;

    public DotESelectionMenu(BusyConfirmationStorage confirm)
    {
        confirmStorage = confirm;
    }
	
    /**
     *  �I�����̐����擾����
     * 
     * @return �I�����̐�
     */ 
    public int numberOfSelections()
    {
    	return (9);
    }

    /**
     *  �I�����̕�������擾����
     *  
     *  @param itemNumber �I����
     *  @return �I�����̕�����
     */
    public String getSelectionItem(int itemNumber)
    {
        String itemString = "";
    	switch (itemNumber)
    	{
	      case 8:
			itemString = "�A�v���I��";
            break;

	      case 7:
			itemString = "�̈�N���A";
			break;

	      case 6:
			itemString = "�̈�\�t";
			break;

	      case 5:
		    itemString = "�̈�w��";
		    break;

	      case 4:
	        itemString = "File���ύX";
	        break;

	      case 3:
	        itemString = "Dir�I��";
	        break;

	      case 2:
	        itemString = "Load";
	        break;

	      case 1:
		    itemString = "Save";
            break;

	      case 0:
	      default:
		    itemString = "�F�̑I��";
            break;
/*
  	      case 0:
          default:
        	itemString = "�J���[�I��";
            break;
*/
    	}
    	return (itemString);
    	
    }
    


    /**
     *  �A�C�e���I���𕡐������ɉ\���ǂ�������������
     * 
     *  @return �����I���\(true) / �P����(false)
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
        currentSelectedItem = -1;
        nextSceneId = SCENE_TO_PREVIOUS;
    }
    
    /**
     *  �I�𒆂̃A�C�e��������������
     * 
     *  @return �I�𒆂̃A�C�e����
     */
    public int getNumberOfSelectedItems()
    {
        if (currentSelectedItem == -1)
        {
        	return (0);
        }
        return (1);
    }

    /**
     *  �I�𒆂̂̃A�C�e��ID����������
     *  
     *  @param itemNumber �I�𒆂̃A�C�e���ԍ�(�[���X�^�[�g)
     *  @return �A�C�e��ID
     */
    public int getSelectedItem(int itemNumber)
    {
    	return (currentSelectedItem);
    }


    /**
     *   �I�������A�C�e����ݒ肷��
     *   
     *   @param itemId �I�������A�C�e��ID
     */
    public void setSelectedItem(int itemId)
    {
    	currentSelectedItem = itemId;
    }

    /**
     *  �I�����ꂽ�{�^��ID��ݒ肷��
     * 
     *  @param buttonId �{�^��ID
     *  @return �I������(true) / �I�����Ȃ� (false)
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        return (true);
    }

    /**
     *  �I��p�̃K�C�h��������擾����<br>
     *  �i����������������ꍇ�A��ʉ����ɃK�C�h�������\������)
     *  
     *  @return �K�C�h������
     *  
     */
	public String getSelectionGuidance()
	{
		return (null);
	}

	/**
     *  �^�C�g����\�����邩�ǂ����H
     *  
     *  @return �^�C�g���\������(true) / �\�����Ȃ�(false)
     */
	public boolean isShowTitle()
	{
		return (false);
	}

	/**
	 *  start���ɁA�A�C�e�������������邩�ǂ���<br>
	 *  (�J�[�\���ʒu��ێ����Ă��������Ƃ���false�Ƃ���
	 *  
	 *  @return ����������(true) / ���������Ȃ�(false)
	 */
	public boolean isItemInitialize()
	{
		return (true);
	}

	/**
	 *  �{�^���̐����擾����
	 * 
	 *  @return �\������{�^���̐�
	 */
	public int getNumberOfSelectionButtons()
	{
		return (0);
	}

	/**
     *  �{�^���̃��x�����擾����
     *  
     *  @param buttonId �{�^����ID
     *  @return �\������{�^���̃��x��
     */
	public String getSelectionButtonLabel(int buttonId)
	{
		return (null);
	}
	
	/**
     *  �w�i���݂��Ⴂ�̐F�ŕ\�����邩���擾����
     *
     *  @return �݂��Ⴂ�̐F�ɂ���(true) / �w�i�͂P�F(false)
     */
	public boolean getSelectionBackColorMode()
	{
		return (false);
	}

	/**
     *  ���j���[�{�^���������ꂽ�^�C�~���O��ʒm����
     * 
     *  @return �������p�����邩�ǂ���
     */
	public boolean triggeredMenu()
	{
		return (true);
	}
	
    /**
	 *  ���̉�ʐ؂�ւ�����ID���擾����
	 *  
	 *  @param screenId (���݂�)���ID
	 *  @return �؂�ւ�����ID
	 */
	public int nextSceneToChange(int screenId)
	{
        int sceneId = DotESceneDB.DEFAULT_SCREEN;
        nextSceneId = SCENE_TO_PREVIOUS;
        // currentSelectedItem �̓��e�ŕ���
		switch (currentSelectedItem)
		{
           case 8:
        	 // �A�v���I��
  		     sceneId = DotESceneDB.CONFIRMATION;
	         confirmStorage.prepareMessage("�A�v�����I�����܂��B", BusyConfirmationStorage.CONFIRMATION_MODE, DotESceneDB.DEFAULT_SCREEN);
	         nextSceneId = DotESceneDB.CONFIRMATION_END;
	         break;

           case 7:
        	 // �R�s�[�̈�̃N���A
             sceneId = DotESceneDB.DEFAULT_SCREEN;
    	     nextSceneId = DotESceneDB.REGION_CLEAR;
             break;

           case 6:
        	 // �R�s�[�̈�̓\��t��
             sceneId = DotESceneDB.DEFAULT_SCREEN;
             nextSceneId = DotESceneDB.REGION_PASTE;
             break;

           case 5:
        	 // �R�s�[�̈�̎w��
             sceneId = DotESceneDB.DEFAULT_SCREEN;
    	     nextSceneId = DotESceneDB.REGION_COPY;
             break;

           case 4:
             sceneId = DotESceneDB.FILENAME_FORM;
	         break;

		   case 3:
		     sceneId = DotESceneDB.DIR_SCREEN;
		     break;

           case 2:
		     sceneId = DotESceneDB.CONFIRMATION;
	         confirmStorage.prepareMessage("�f�[�^��ǂݏo���܂��B", BusyConfirmationStorage.CONFIRMATION_MODE, DotESceneDB.DEFAULT_SCREEN);
	         nextSceneId = DotESceneDB.CONFIRMATION_LOAD;
	         break;

           case 1:
			 sceneId = DotESceneDB.CONFIRMATION;
	         confirmStorage.prepareMessage("�f�[�^��ۑ����܂��B", BusyConfirmationStorage.CONFIRMATION_MODE, DotESceneDB.DEFAULT_SCREEN);
	         nextSceneId = DotESceneDB.CONFIRMATION_SAVE;
		     break;

           case 0:
  			 sceneId = DotESceneDB.DEFAULT_SCREEN;
  	         nextSceneId = DotESceneDB.COLORPICK_PIXEL;
  		     break;
/*
	  	   case 0:
		     sceneId = DotESceneDB.COLORPICK_SCREEN;
		     break;
*/
	  	   default:
	         sceneId = DotESceneDB.DEFAULT_SCREEN;
	         break;
		 }
		 currentSelectedItem = -1;
         return (sceneId);
	}
	
	/**
	 *  ���̉��ID���擾����
	 * @return ���ID
	 */
	public int getNextSceneId()
	{
		return (nextSceneId);
	}	
}
