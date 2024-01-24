import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.IFileSelectionStorage;
/**
 *  �I����ʂŕ\������f�[�^��ێ�����N���X�ł��B
 * @author MRSa
 *
 */
public class TVgSelectionStorage implements ISelectionScreenStorage, IFileSelectionStorage
{
    TVgDataStorage       storageDb  = null;
    TVgProgramDataParser dataParser = null;
    boolean isConfirmation = false;

    String selectedFileName = null;
    
    int scene = -1;
    int selectedItemId = -1;

    /**
     *  �R���X�g���N�^�ł́A���������������Ȃ��܂���
     *
     */
    public TVgSelectionStorage(TVgProgramDataParser parser, TVgDataStorage storage)
    {
        dataParser = parser;
        storageDb  = storage;
    }

    /**
     *  �I������\������V�[����ݒ肷��
     * 
     * @param mode �\���V�[��
     */
    public void setSceneMode(int mode)
    {
        scene = mode;
    }    
    
    /**
     *  �I�����̐����擾����
     * 
     * @return �I�����̐�
     */ 
    public int numberOfSelections()
    {
        int items = 0;
        switch (scene)
        {
          case TVgSceneDb.SCENE_MENUSELECTION: // ���j���[�I��
            items = 3;
            break;

          case TVgSceneDb.SCENE_STATIONSELECTION: // �e���r�ǑI��
            items = dataParser.getNumberOfTvStations();
            break;

          case TVgSceneDb.SCENE_TVTYPESELECTION: // �d�g��ʑI��
            items = 3;
            break;

          case TVgSceneDb.SCENE_LOCALESELECTION: // �n��I��
            items = 54;
            break;

          default:
            break;
        }
        return (items);
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
        switch (scene)
        {
          case TVgSceneDb.SCENE_MENUSELECTION: // ���j���[�I��
            itemString = getSelectionMenu(itemNumber);
            break;

          case TVgSceneDb.SCENE_STATIONSELECTION: // �e���r�ǑI��
            itemString = dataParser.getTvStationName(itemNumber);
            break;

          case TVgSceneDb.SCENE_TVTYPESELECTION: // �d�g��ʑI��
            itemString = getStationTypeMenu(itemNumber);
            break;

          case TVgSceneDb.SCENE_LOCALESELECTION: // �n��I��
            itemString = getLocalMenu(itemNumber);
            break;

          default:
            break;
        }
        return (itemString);
    }

    /**
     *  ���j���[�̑I�����i������j����������
     * @param itemNumber �A�C�e���ԍ�
     * @return �A�C�e����
     */
    public String getSelectionMenu(int itemNumber)
    {
        String name = "";
        if (itemNumber == 0)
        {
            name = "�n��̑I��";
        }
        else if (itemNumber == 1)
        {
            name = "�ǎ�ʑI��";
        }
        else if (itemNumber == 2)
        {
            name = "�f�B���N�g���w��";
        }
        return (name);
    }
    
    /**
     *  �e���r�ǎ�ʂ̑I�����i������j����������
     * @param itemNumber �A�C�e���ԍ�
     * @return �A�C�e����
     */
    public String getStationTypeMenu(int itemNumber)
    {
        String name = "";
        if (itemNumber == 0)
        {
            name = "�n��g";
        }
        else if (itemNumber == 1)
        {
            name = "UHF/BS";
        }
        else if (itemNumber == 2)
        {
            name = "BS�f�W�^��";
        }
        return (name);
    }

    /**
     *  �n��I���̑I�����i������j����������
     * @param itemMenu �A�C�e���ԍ�
     * @return �A�C�e����
     */
    public String getLocalMenu(int itemMenu)
    {
        String itemString = "";
        switch (itemMenu)
        {
          case 0:
              itemString = "�k�C���i�D�y�j";
              break;
          case 1:
              itemString = "�k�C���i���فj";
              break;
          case 2:
              itemString = "�k�C���i����j";
              break;
          case 3:
              itemString = "�k�C���i�эL�j";
              break;
          case 4:
              itemString = "�k�C���i���H�j";
              break;
          case 5:
              itemString = "�k�C���i�k���j";
              break;
          case 6:
              itemString = "�k�C���i�����j";
              break;
          case 7:
              itemString = "�X";
              break;
          case 8:
              itemString = "���";
              break;
          case 9:
              itemString = "�{��";
              break;
          case 10:
              itemString = "�H�c";
              break;
          case 11:
              itemString = "�R�`";
              break;
          case 12:
              itemString = "����";
              break;
          case 13:
              itemString = "���";
              break;
          case 14:
              itemString = "�Ȗ�";
              break;
          case 15:
              itemString = "�Q�n";
              break;
          case 16:
              itemString = "���";
              break;
          case 17:
              itemString = "��t";
              break;
          case 18:
              itemString = "����";
              break;
          case 19:
              itemString = "�_�ސ�";
              break;
          case 20:
              itemString = "�V��";
              break;
          case 21:
              itemString = "�R��";
              break;
          case 22:
              itemString = "����";
              break;
          case 23:
              itemString = "�x�R";
              break;
          case 24:
              itemString = "�ΐ�";
              break;
          case 25:
              itemString = "����";
              break;
          case 26:
              itemString = "��";
              break;
          case 27:
              itemString = "�É�";
              break;
          case 28:
              itemString = "���m";
              break;
          case 29:
              itemString = "�O�d";
              break;
          case 30:
              itemString = "����";
              break;
          case 31:
              itemString = "���s";
              break;
          case 32:
              itemString = "���";
              break;
          case 33:
              itemString = "����";
              break;
          case 34:
              itemString = "�ޗ�";
              break;
          case 35:
              itemString = "�a�̎R";
              break;
          case 36:
              itemString = "����";
              break;
          case 37:
              itemString = "����";
              break;
          case 38:
              itemString = "���R";
              break;
          case 39:
              itemString = "�L��";
              break;
          case 40:
              itemString = "�R��";
              break;
          case 41:
              itemString = "����";
              break;
          case 42:
              itemString = "����";
              break;
          case 43:
              itemString = "���Q";
              break;
          case 44:
              itemString = "���m";
              break;
          case 45:
              itemString = "�����i�����j";
              break;
          case 46:
              itemString = "�����i�k��B�j";
              break;
          case 47:
              itemString = "����";
              break;
          case 48:
              itemString = "����";
              break;
          case 49:
              itemString = "�F�{";
              break;
          case 50:
              itemString = "�啪";
              break;
          case 51:
              itemString = "�{��";
              break;
          case 52:
              itemString = "������";
              break;
          case 53:
              itemString = "����";
              break;
          default:
              itemString = "";
              break;
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
        selectedItemId = -1;
    }
    
    /**
     *  �I�𒆂̃A�C�e��������������
     * 
     *  @return �I�𒆂̃A�C�e����
     */
    public int getNumberOfSelectedItems()
    {
        if (selectedItemId == -1)
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
     *  �I�𒆂̂̃A�C�e��ID����������
     *  
     *  @param itemNumber �I�𒆂̃A�C�e���ԍ�(�[���X�^�[�g)
     *  @return �A�C�e��ID
     */
    private int getSelectedItemLocal(int itemNumber)
    {
      int item = -1;
      switch (itemNumber)
      {
      case 0:
        item = 1;
        break;
      case 1:
        item = 52;
        break;
      case 2:
        item = 48;
        break;
      case 3:
        item = 50;
        break;
      case 4:
        item = 51;
        break;
      case 5:
        item = 49;
        break;
      case 6:
        item = 53;
        break;
      case 7:
        item = 2;
        break;
      case 8:
        item = 3;
        break;
      case 9:
        item = 4;
        break;
      case 10:
        item = 5;
        break;
      case 11:
        item = 6;
        break;
      case 12:
        item = 7;
        break;
      case 13:
        item = 8;
        break;
      case 14:
        item = 9;
        break;
      case 15:
        item = 10;
        break;
      case 16:
        item = 11;
        break;
      case 17:
        item = 12;
        break;
      case 18:
        item = 13;
        break;
      case 19:
        item = 14;
        break;
      case 20:
        item = 15;
        break;
      case 21:
        item = 16;
        break;
      case 22:
        item = 17;
        break;
      case 23:
        item = 18;
        break;
      case 24:
        item = 19;
        break;
      case 25:
        item = 20;
        break;
      case 26:
        item = 21;
        break;
      case 27:
        item = 22;
        break;
      case 28:
        item = 23;
        break;
      case 29:
        item = 24;
        break;
      case 30:
        item = 25;
        break;
      case 31:
        item = 26;
        break;
      case 32:
        item = 27;
        break;
      case 33:
        item = 28;
        break;
      case 34:
        item = 29;
        break;
      case 35:
        item = 30;
        break;
      case 36:
        item = 31;
        break;
      case 37:
        item = 32;
        break;
      case 38:
        item = 33;
        break;
      case 39:
        item = 34;
        break;
      case 40:
        item = 35;
        break;
      case 41:
        item = 36;
        break;
      case 42:
        item = 37;
        break;
      case 43:
        item = 38;
        break;
      case 44:
        item = 39;
        break;
      case 45:
        item = 40;
        break;
      case 46:
        item = 54;
        break;
      case 47:
        item = 41;
        break;
      case 48:
        item = 42;
        break;
      case 49:
        item = 43;
        break;
      case 50:
        item = 44;
        break;
      case 51:
        item = 45;
        break;
      case 52:
        item = 46;
        break;
      case 53:
        item = 47;
        break;
      default:
        break;
      }
      return (item);
    }

    /**
     *   �I�������A�C�e����ݒ肷��
     *   
     *   @param itemId �I�������A�C�e��ID
     */
    public void setSelectedItem(int itemId)
    {
        selectedItemId = itemId;
    }

    /**
     *  �I�����m�肳�ꂽ�Ƃ��̏���
     *  
     * @param itemNumber �A�C�e���ԍ�
     */
    private void decideSelectedItem(int itemNumber)
    {
        switch (scene)
        {
          case TVgSceneDb.SCENE_MENUSELECTION: // ���j���[�I��
            // �����ł͉������Ȃ�
            break;

          case TVgSceneDb.SCENE_STATIONSELECTION: // �e���r�ǑI��
            storageDb.setTvStation(itemNumber);
            break;

          case TVgSceneDb.SCENE_TVTYPESELECTION: // �d�g��ʑI��
            storageDb.setTvType(itemNumber);
            break;

          case TVgSceneDb.SCENE_LOCALESELECTION: // �n��I��
            storageDb.setAreaId(getSelectedItemLocal(itemNumber), getLocalMenu(itemNumber));
            break;

          default:
            break;
        }
        
        return;
    }

    /**
     *  �I�����ꂽ�{�^��ID��ݒ肷��
     * 
     *  @param buttonId �{�^��ID
     *  @return �I������(true) / �I�����Ȃ� (false)
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId != ISelectionScreenStorage.BUTTON_CANCELED)
        {
            // �A�C�e�����I�����ꂽ
            decideSelectedItem(selectedItemId);
            isConfirmation = true;
        }
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
        if(scene == TVgSceneDb.SCENE_MENUSELECTION)
        {
            // ���j���[�I�����ɂ́A�^�C�g����\�����Ȃ�
            return (false);
        }
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
        if (scene == TVgSceneDb.SCENE_MENUSELECTION)
        {
            return (0);
        }
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
        return ("OK");
    }

    /**
     *  �w�i���݂��Ⴂ�̐F�ŕ\�����邩���擾����
     *
     *  @return �݂��Ⴂ�̐F�ɂ���(true) / �w�i�͂P�F(false)
     */
    public boolean getSelectionBackColorMode()
    {
        if(scene == TVgSceneDb.SCENE_MENUSELECTION)
        {
            // ���j���[�I�����ɂ́A�^�C�g����\�����Ȃ�
            return (false);
        }
        return (true);
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
        int nextId = ISelectionScreenStorage.SCENE_TO_PREVIOUS;
        switch (scene)
        {
          case TVgSceneDb.SCENE_MENUSELECTION: // ���j���[�I��
          case TVgSceneDb.SCREEN_MENUSELECT:
            nextId = decideNextScreen(selectedItemId);
            break;

          case TVgSceneDb.SCENE_STATIONSELECTION: // �e���r�ǑI��
            nextId = TVgSceneDb.SCENE_PROGRAMLIST;
            break;

          case TVgSceneDb.SCENE_TVTYPESELECTION: // �d�g��ʑI��
            nextId = TVgSceneDb.SCENE_DATELIST;
            break;

          case TVgSceneDb.SCENE_LOCALESELECTION: // �n��I��
            nextId = TVgSceneDb.SCENE_DATELIST;
            break;
            
          case TVgSceneDb.SCENE_DIRSELECTION:    // �f�B���N�g���I��
          case TVgSceneDb.SCREEN_DIRSELECT:
            nextId = TVgSceneDb.SCENE_DATELIST;
            break;

          case TVgSceneDb.SCENE_WELCOME:         // �悤�������
            nextId = TVgSceneDb.SCENE_DIRSELECTION;
            break;

          default:
            break;
        }
        isConfirmation = false;
        return (nextId);
    }
    
    /**
     *  ���j���[��ʂ��甲�����Ƃ��A�ǂ̉�ʂɑJ�ڂ��邩���߂�
     * 
     * @param itemNumber �I�������ԍ�
     * @return �����ID
     */
    private int decideNextScreen(int itemNumber)
    {
        int nextId = ISelectionScreenStorage.SCENE_TO_PREVIOUS;
        if(isConfirmation == false)
        {
            // �O��ʂɖ߂�
            return (nextId);
        }
        if (itemNumber == 0)
        {
            // �n��I��
            nextId = TVgSceneDb.SCENE_LOCALESELECTION;
        }
        else if (itemNumber == 1)
        {
            // �ǎ�ʑI��
            nextId = TVgSceneDb.SCENE_TVTYPESELECTION;
        }
        else if (itemNumber == 2)
        {
            // �L�^�f�B���N�g���w��
            nextId = TVgSceneDb.SCENE_DIRSELECTION;
        }
        return (nextId);
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
        if (selectedFileName == null)
        {
            return (0);
        }
        return (1);
    }

    /**
     *  �I���i�t�@�C���j������������
     * 
     *  @param index �I��ԍ�(0�X�^�[�g)
     *  @return �I��ԍ��̃t�@�C����
     */
    public String getSelectedName(int index)
    {
        if (selectedFileName == null)
        {
            return ("");
        }
        return (selectedFileName);
    }

    /**
     *  �I���i�t�@�C���j����ݒ肷��
     * 
     * @param name �I�������t�@�C���̖��O
     */
    public void setSelectedName(String name)
    {
        selectedFileName = name;
    }

    /**
     *  �I���i�t�@�C���j�����N���A����
     *  
     */
    public void clearSelectedName()
    {
        selectedFileName = null;
    }
    
    /**
     *  �I���i�t�@�C���j�����m�肷��
     *  
     */
    public void confirmSelectedName()
    {
        if (selectedFileName != null)
        {
            storageDb.setBaseDirectory(selectedFileName);
        }
        selectedFileName = null;
    }
}
