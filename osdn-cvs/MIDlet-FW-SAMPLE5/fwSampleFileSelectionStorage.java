import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.selections.*;
/**
 *  �t�@�C��/�f�B���N�g���I��p�X�g���[�W�N���X
 * 
 * @author MRSa
 *
 */
public class fwSampleFileSelectionStorage implements ISelectionScreenStorage, IFileSelectionStorage
{
    private String selectedItemName = null;
    private fwSampleStorage storageDb = null;
    private MidpSceneSelector selector = null;
    
    public fwSampleFileSelectionStorage(fwSampleStorage storage, MidpSceneSelector object)
    {
        storageDb = storage;
        selector = object;
    }

    /**
     *  �I�����̐����擾����
     * 
     * @return �I�����̐�
     */
    public int numberOfSelections()
    {
        return (-1);
    }

    /**
     *  �I�����̕�������擾����
     *  
     *  @param itemNumber �I����
     *  @return �I�����̕�����
     */
    public String getSelectionItem(int itemNumber)
    {
        return ("");
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
        //
    }
    
    /**
     *  �I�𒆂̃A�C�e��������������
     * 
     *  @return �I�𒆂̃A�C�e����
     */
    public int getNumberOfSelectedItems()
    {
        if (selectedItemName == null)
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
        return (itemNumber);
    }

    /**
     *   �I�������A�C�e����ݒ肷��
     *   
     *   @param itemId �I�������A�C�e��ID
     */
    public void setSelectedItem(int itemId)
    {

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
        return ("");
    }

    /**
     *  �^�C�g����\�����邩�ǂ����H
     *  
     *  @return �^�C�g���\������(true) / �\�����Ȃ�(false)
     */
    public boolean isShowTitle()
    {
        return (true);
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
        return (1);
    }

    /**
     *  �{�^���̃��x�����擾����
     *  
     *  @param buttonId �{�^����ID
     *  @return �\������{�^���̃��x��
     */
    public String getSelectionButtonLabel(int buttonId)
    {
        return ("����");
    }
    
    /**
     *  �w�i���݂��Ⴂ�̐F�ŕ\�����邩���擾����
     *
     *  @return �݂��Ⴂ�̐F�ɂ���(true) / �w�i�͂P�F(false)
     */
    public boolean getSelectionBackColorMode()
    {
        return (true);
    }
    

    /**
     *  ���j���[�{�^���������ꂽ�^�C�~���O��ʒm����
     * 
     *  @return �������p�����邩�ǂ���
     */
    public boolean triggeredMenu()
    {
        confirmSelectedName();
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
        return (fwSampleFactory.DEFAULT_SCREEN);
    }

    /**
     *  �����f�B���N�g������������
     *  
     *  @return �����f�B���N�g��
     */
    public String getDefaultDirectory()
    {
        return (null);
    }

    /**
     *  ���ݑI������Ă���t�@�C������уf�B���N�g���̐�����������
     *  
     *  @return ���ݑI������Ă���t�@�C��(�f�B���N�g��)��
     */
    public int getSelectedNameCount()
    {
        return (getNumberOfSelectedItems());
    }
    
    /**
     *  �I���i�t�@�C���j������������
     * 
     *  @param index �I��ԍ�(0�X�^�[�g)
     *  @return �I��ԍ��̃t�@�C����
     */
    public String getSelectedName(int index)
    {
        return (selectedItemName);
    }

    /**
     *  �I���i�t�@�C���j����ݒ肷��
     * 
     * @param name �I�������t�@�C���̖��O
     */
    public void setSelectedName(String name)
    {
        selectedItemName = name;
    }

    /**
     *  �I���i�t�@�C���j�����N���A����
     *  
     */
    public void clearSelectedName()
    {
        selectedItemName = null;
    }
    
    /**
     *  �I���i�t�@�C���j�����m�肷��
     *  
     */
    public void confirmSelectedName()
    {
        if (selectedItemName.endsWith("/") == false)
        {
            // �t�@�C������ݒ肷��
            storageDb.setSoundFileName(selectedItemName);
            selector.changeScene(fwSampleFactory.DEFAULT_SCREEN, "");
        }
        return;
    }
}
