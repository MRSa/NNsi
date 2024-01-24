import java.util.Date;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.screens.storage.*;
import jp.sourceforge.nnsi.a2b.framework.interfaces.*;
import jp.sourceforge.nnsi.a2b.utilities.*;

/**
 * �A�v���P�[�V�����̃f�[�^���i�[����N���X
 * 
 * @author MRSa
 *
 */
public class HiMemoSceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{
    private final int  MaxDataInputLength   = 2048;

    /***** �����f�[�^(��������) *****/
    private TextField    inputDataField1     = null;
    private TextField    inputDataField2     = null;
    
    private int          itemSelectionIndex = -1;
    /***** �����f�[�^(�����܂�) *****/

    /***** ���̑��A�N�Z�X�N���X *****/
    private MidpFileOperations fileOperation = null;
    private MidpResourceFileUtils resUtils   = null;
    private HiMemoDataStorage storageDb = null;
    
    /**
     *  �R���X�g���N�^
     *
     *  @param dataStorage �f�[�^�L�^�N���X
     *  @param resourceUtils ���\�[�X�A�N�Z�X�N���X
     *  @param fileUtils �t�@�C���A�N�Z�X�N���X
     */
    public HiMemoSceneStorage(HiMemoDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpFileOperations fileUtils)
    {
        storageDb = dataStorage;
        fileOperation = fileUtils;
        resUtils = resourceUtils;
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
        return (resUtils.getResourceText("/res/welcome.txt"));
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
        return;
    }

    /**
     * �����t�@�C��������������
     * 
     * @return
     */
    public String getMemoFileName()
    {
        if (storageDb.getBaseDirectory() == null)
        {
            return (null);
        }
        String fileName = storageDb.getBaseDirectory() + "hiMEMO.txt";
        return (MidpResourceFileUtils.convertInternalFileLocation(fileName));
    }
    
    /**
     *  �f�[�^���͂��n�j���ꂽ�Ƃ�
     *  
     */
    public void dataEntry()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                String fileName = storageDb.getBaseDirectory() + "hiMEMO.txt";
                Date date = new Date();
                String outputString = date.toString() + "," + inputDataField1.getString() + "," + inputDataField2.getString();
                fileOperation.outputDataTextFile(fileName, null, null, outputString, true);
                inputDataField1 = null;
                inputDataField2 = null;
            }
        };
        try
        {
            thread.start();
//            thread.join();
        }
        catch (Exception ex)
        {
            String fileName = storageDb.getBaseDirectory() + "hiMEMO.txt";
            String outputString = ex.getMessage() + " \r\n" + ex.toString();
            fileOperation.outputDataTextFile(fileName, null, null, outputString, true);            
        }
        thread = null;
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
        if (screenId == HiMemoSceneDB.WELCOME_SCREEN)
        {
            return (HiMemoSceneDB.DIR_SCREEN);
        }
        else if (screenId == HiMemoSceneDB.DIR_SCREEN)
        {
            return (HiMemoSceneDB.DEFAULT_SCREEN);
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
        return (2);
    }

    /**
     *  �f�[�^���̓t�B�[���h���擾����
     *  
     */
    public TextField getDataInputField(int index)
    {
        if (index == 0)
        {
            inputDataField1 = new TextField("�f�[�^1", "", MaxDataInputLength, TextField.ANY);
            return (inputDataField1);
        }
        inputDataField2 = new TextField("�f�[�^2", "", MaxDataInputLength, TextField.ANY);
        return (inputDataField2);
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
        return (true);
    }
}
