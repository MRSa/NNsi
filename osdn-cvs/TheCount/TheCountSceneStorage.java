import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.screens.informations.*;
import jp.sourceforge.nnsi.a2b.screens.selections.*;
import jp.sourceforge.nnsi.a2b.forms.*;
import jp.sourceforge.nnsi.a2b.utilities.*;

/**
 * �A�v���P�[�V�����̃f�[�^���i�[����N���X
 * 
 * @author MRSa
 *
 */
public class TheCountSceneStorage implements IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage, ISelectionScreenStorage
{
    private final int  MaxDataInputLength   = 48;

    /***** �����f�[�^(��������) *****/
    private int          itemSelectionIndex = -1;
    private String       informationMessage  = null;
    private TextField    inputItem1NameField = null;
    private TextField    inputItem2NameField = null;
    private TextField    inputItem3NameField = null;
    private TextField    inputItem4NameField = null;
    private TextField    inputItem5NameField = null;
    private TextField    inputItem6NameField = null;
    /***** �����f�[�^(�����܂�) *****/

    /***** ���̑��A�N�Z�X�N���X *****/
    private MidpResourceFileUtils resUtils    = null;
    private TheCountDataStorage storageDb     = null;
    
    /**
     *  �R���X�g���N�^
     *
     *  @param dataStorage �f�[�^�L�^�N���X
     *  @param resourceUtils ���\�[�X�A�N�Z�X�N���X
     *  @param fileUtils �t�@�C���A�N�Z�X�N���X
     */
    public TheCountSceneStorage(TheCountDataStorage dataStorage, MidpResourceFileUtils resourceUtils)
    {
        resUtils = resourceUtils;
        storageDb = dataStorage;
    }

    /**------------------------------------------------------------**
     **
     **  ��������ۊǃf�[�^�̓ǂݏ���...
     **
     **
     **------------------------------------------------------------**/
    /**
     * �f�B���N�g���̉���
     * @return �f�B���N�g����
     */
    public String getDefaultDirectory()
    {
        return (storageDb.getBaseDirectory());
    }

    /**
     *  �I������Ă���i�t�@�C���j������������
     *  
     */
    public  int getSelectedNameCount()
    {
        if (storageDb.getBaseDirectory() == null)
        {
        	return (0);
        }
        return (1);
    }

    /**
     *  �f�B���N�g���̉���
     * @return �f�B���N�g����
     */
    public String getSelectedName(int index)
    {
        return (storageDb.getBaseDirectory());
    }
    
    /**
     *   �f�B���N�g���̉���
     * @param �f�B���N�g����
     */
    public void setSelectedName(String name)
    {
        storageDb.setBaseDirectory(name);
    }

    /**
     *  �I�𒆂̃f�B���N�g�������N���A����
     * 
     */
    public void clearSelectedName()
    {
    	storageDb.setBaseDirectory(null);
    	return;
    }
    
    /**
     *  �ݒ肵���f�B���N�g�������m�肷��
     *  (�������Ȃ�)
     * 
     */
    public void confirmSelectedName()
    {
    	return;
    }
    
    /**------------------------------------------------------------**
     **
     **  ������������f�[�^�̓ǂݏ���...
     **
     **
     **------------------------------------------------------------**/
    /**
     * �\�����郁�b�Z�[�W����������
     * 
     * @return
     */
    public String getInformationMessageToShow()
    {
    	String message = "";
    	try
    	{
    	    message = resUtils.getResourceText("/res/welcome.txt");
    	}
    	catch (Exception ex)
    	{
    		//
    	}
        return (message);
    }

    /**
     *   �{�^��������������
     *   
     */
    public int getNumberOfButtons(int screenId)
    {
        return (1);
    }

    /**
     * ���b�Z�[�W���󂯕t����ꂽ���ǂ����ݒ肷��
     * @param buttonId �����ꂽ�{�^����ID
     */
    public void setButtonId(int buttonId)
    {
        // �{�^���������ꂽ�I�I�i��ʑJ�ځI�j
        
    }

    /**
     *  ���\����ʂ̃{�^�����x��������
     * 
     */
    public String getInformationButtonLabel(int buttonId)
    {
        return("Dir�I��");
    }

    /**
     *  �f�[�^���͂��L�����Z�����ꂽ�Ƃ�
     *  
     */
    public void cancelEntry()
    {
    	inputItem1NameField = null;
    	inputItem2NameField = null;
    	inputItem3NameField = null;
    	inputItem4NameField = null;
    	inputItem5NameField = null;
    	inputItem6NameField = null;
    	System.gc();
    	return;
    }

    /**
     *  �f�[�^���͂��n�j���ꂽ�Ƃ�
     *  
     */
    public void dataEntry()
    {
        // 
    	String ddd  = inputItem1NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(0, ddd);
    	}
    	
        // 
    	ddd  = inputItem2NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(1, ddd);
    	}
    	
        // 
    	ddd  = inputItem3NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(2, ddd);
    	}
    	
        // 
    	ddd  = inputItem4NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(3, ddd);
    	}
    	
        // 
    	ddd  = inputItem5NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(4, ddd);
    	}
    	
        // 
    	ddd  = inputItem6NameField.getString();
    	if (ddd.length() > 0)
    	{
    		storageDb.setItemName(5, ddd);
    	}
    	inputItem1NameField = null;
    	inputItem2NameField = null;
    	inputItem3NameField = null;
    	inputItem4NameField = null;
    	inputItem5NameField = null;
    	inputItem6NameField = null;
    	System.gc();
    	return;
    }

    /**
     *  �f�[�^���͂̂��߂̏���
     *  
     */
    public void prepare()
    {
        // �������Ȃ�        
    }

    /**
     *  ����ʂ̐؂�ւ�
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == TheCountSceneDB.WELCOME_SCREEN)
        {
            return (TheCountSceneDB.DIR_SCREEN);
        }
        else if (screenId == TheCountSceneDB.DIR_SCREEN)
        {
            return (TheCountSceneDB.DEFAULT_SCREEN);
        }
        return (IDataInputStorage.SCENE_TO_PREVIOUS);
    }

    /**
     *  ����ʂ̐؂�ւ�
     * 
     */
    public String nextSceneTitleToChange(int screenId)
    {
        return ("");
    }

    /**
     *  �f�[�^���͏I��
     *  
     */
    public void finish()
    {
        // �������Ȃ�
    }

    /**
     *  �f�[�^���̓t�B�[���h�̌����擾����
     *  
     */
    public int getNumberOfDataInputField()
    {
        return (6);
    }

    /**
     *  �f�[�^���̓t�B�[���h���擾����
     *  
     */
    public TextField getDataInputField(int index)
    {
    	String itemName = storageDb.getItemName(index);
    	if (index == 0)
    	{
    		inputItem1NameField = new TextField("����1", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem1NameField);
    	}
    	if (index == 1)
    	{
    		inputItem2NameField = new TextField("����2", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem2NameField);
    	}
    	if (index == 2)
    	{
    		inputItem3NameField = new TextField("����3", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem3NameField);
    	}
    	if (index == 3)
    	{
    		inputItem4NameField = new TextField("����4", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem4NameField);
    	}
    	if (index == 4)
    	{
    		inputItem5NameField = new TextField("����5", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem5NameField);
    	}
    	if (index == 5)
    	{
    		inputItem6NameField = new TextField("����6", itemName, MaxDataInputLength, TextField.ANY);
    		return (inputItem6NameField);
    	}
    	return (null);
    }

    /**
     *  �I�v�V�����̌����擾����
     *  
     */
    public int getNumberOfOptionGroup()
    {
        return (0);
    }

    /**
     *   �I�v�V�������擾����
     */
    public ChoiceGroup getOptionField(int index)
    {
        return (null);
    }

    /**
     *  �I�����̐����擾����
     *  
     */
    public int numberOfSelections()
    {
        return (0);
    }

    /**
     *  �I�����̕�������擾����
     *  
     */
    public String getSelectionItem(int itemNumber)
    {
        return ("");
    }
    
    /**
     *  �I��p�̃K�C�h��������擾����
     *  
     */
    public String getSelectionGuidance()
    {
        return ("Menu��Dir�m��");
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
		return (true);
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
        return ("DIR�I��");
    }

    /**
     *  �w�i���݂��Ⴂ�̐F�ŕ\�����邩�H
     *  
     */
    public boolean getSelectionBackColorMode()
    {
        return (false);
    }

    
    public void setSelectedItem(int itemId)
    {
        itemSelectionIndex = itemId;
    }

    public int getSelectedItem(int itemNumber)
    {
        if (itemNumber != 0)
        {
            return (-1);
        }
        return (itemSelectionIndex);
    }
    
    public boolean isSelectedItemMulti()
    {
        return (false);
    }

    public int getNumberOfSelectedItems()
    {
        if (itemSelectionIndex < 0)
        {
            return (0);
        }
        return (1);
    }

    /**
     *   �I�����ꂽ�{�^��ID����������
     *   
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            resetSelectionItems();
        }
        return (false);
    }

    /**
     * �A�C�e���I�����N���A����
     * 
     */
    public void resetSelectionItems()
    {
        itemSelectionIndex = -1;
        return;
    }

    /**
     * ���j���[�{�^���������ꂽ
     * 
     */
    public boolean triggeredMenu()
    {    
        // ��ʂ�؂�ւ���...
        return (true);
    }

    /**
     *  ��񃁃b�Z�[�W��ݒ肷��
     * 
     * @param message
     */
    public void setInformationMessage(String message)
    {
    	informationMessage = message;
    }

    /**
     *  ��񃁃b�Z�[�W����������
     * 
     * @return
     */
    public String getInformationMessage()
    {
    	return (informationMessage);
    }
}
