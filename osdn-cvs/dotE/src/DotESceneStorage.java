import javax.microedition.lcdui.TextField;
import javax.microedition.lcdui.ChoiceGroup;
import jp.sourceforge.nnsi.a2b.forms.IDataInputStorage;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.IInformationScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.IFileSelectionStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;

public class DotESceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{

    /***** �����f�[�^(��������) *****/
    private TextField    dataFileNameField   = null;
    private String       informationMessage = null;
    private int          itemSelectionIndex = -1;
    /***** �����f�[�^(�����܂�) *****/

    /***** ���̑��A�N�Z�X�N���X *****/
    private MidpCanvas                baseCanvas = null;
    private MidpResourceFileUtils resUtils       = null;
    
    private DotEDataStorage storageDb = null;
    
    /**
     *  �R���X�g���N�^
     *
     *  @param dataStorage �f�[�^�L�^�N���X
     *  @param resourceUtils ���\�[�X�A�N�Z�X�N���X
     *  @param fileUtils �t�@�C���A�N�Z�X�N���X
     */
    public DotESceneStorage(DotEDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpCanvas dataProcess, MidpFileDirList dirList)
    {
        storageDb        = dataStorage;
        resUtils         = resourceUtils;
        baseCanvas       = dataProcess;
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
        dataFileNameField = null;
        resetSelectionItems();
        System.gc();
        return;
    }

    /**
     *  �f�[�^���͂��n�j���ꂽ�Ƃ�
     *  
     */
    public void dataEntry()
    {
        storageDb.setDataFileName(dataFileNameField.getString());
        dataFileNameField = null;
        resetSelectionItems();
        System.gc();
        return;
    }

    /**
     *  �f�[�^���͂̂��߂̏���
     *  
     */
    public void prepare()
    {

    }

    /**
     *  ����ʂ̐؂�ւ�
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == DotESceneDB.FILENAME_FORM)
        {
        	return (DotESceneDB.DEFAULT_SCREEN);
        }
        if (screenId == DotESceneDB.DIR_SCREEN)
        {
        	return (DotESceneDB.DEFAULT_SCREEN);
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
        return (1);
    }

    /**
     *  �f�[�^���̓t�B�[���h���擾����
     *  
     */
    public TextField getDataInputField(int index)
    {
    	dataFileNameField = null;
    	String fileName = "" + storageDb.getDataFileName();
    	dataFileNameField = new TextField("�f�[�^�t�@�C����", fileName, 64, TextField.ANY);
        return (dataFileNameField);
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

    // �I�����ꂽ�{�^��ID����������
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            resetSelectionItems();
        }
        return (false);
    }

    // �A�C�e���I�����N���A����
    public void resetSelectionItems()
    {
        itemSelectionIndex = -1;
        return;
    }

    // ���j���[�{�^���������ꂽ
    public boolean triggeredMenu()
    {    
        // ��ʂ�؂�ւ���...
        return (true);
    }
         
    /**
     *  �I���g���K...
     * @param threadIndex
     */
    public void finishTrigger(int threadIndex)
    {
    	//
    }
    
    /**
     *  ��ʍĕ`��w��
     * @param force
     */
    public void redraw(boolean force)
    {
        // ��ʕ\�����X�V
        baseCanvas.redraw(force);
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
