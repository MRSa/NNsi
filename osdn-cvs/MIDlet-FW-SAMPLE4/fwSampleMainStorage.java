import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;

import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.forms.IDataInputStorage;
import jp.sourceforge.nnsi.a2b.utilities.IMidpCyclicTimerReport;
import jp.sourceforge.nnsi.a2b.utilities.MidpCyclicTimer;
import jp.sourceforge.nnsi.a2b.frameworks.IDataProcessing;

/**
 *  ���C����ʗp����N���X
 * 
 * @author MRSa
 *
 */
public class fwSampleMainStorage implements ISampleMainCanvasStorage, IMidpCyclicTimerReport, ISelectionScreenStorage, IDataInputStorage
{
    private MidpCyclicTimer myTimer = null;
    private IDataProcessing reportDestination = null;
    private TextField       inputField = null;
    private int timerStatus = ISampleMainCanvasStorage.TIMER_READY;
    private int setTime     = 180;  // �ݒ莞�ԁi�f�t�H���g��3���j
    private int currentTime = 0;    // ���݂̌o�ߎ���
    private int vibrateTime = 500;  // �^�C���A�E�g���ɐU�����鎞��
    private int selectedItemId = -1;
    
    /**
     *  �R���X�g���N�^�ł̓^�C�}�[�N���X�����ƁA�^�C���A�E�g�������̕񍐐��ݒ肵�܂�
     *
     */
    public fwSampleMainStorage(IDataProcessing destination)
    {
        myTimer = new MidpCyclicTimer(this, 1000);        
        myTimer.start();

        reportDestination = destination;
    }

    /**
     *   �^�C�}�[���X�^�[�g���Ă��邩�ǂ����̏�Ԃ���������
     * 
     * @return �^�C�}�[�̏��
     */
    public int getTimerStatus()
    {
        return (timerStatus);
    }
    
    /**
     *   ���s/��~���w�����ꂽ�Ƃ��̏���<br>
     * �@<li>��~�� �� �^�C�}�[�X�^�[�g</li>
     *   <li>�J�n�� �� �ꎞ��~</li>
     *   <li>�ꎞ��~�� �� �^�C�}�[�ĊJ</li>
     *   <li>�I���� �� ��~��</li>
     * 
     * @return <code>true</code>:�w����t / <code>false</code>:�w���ł����Ԃł͂Ȃ�
     */
    public boolean triggered()
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_READY)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_STARTED;
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_PAUSED;
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_PAUSED)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_STARTED;
        }
        else // if (timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_READY;
        }
        return (true);
    }
    
    /**
     *   �^�C�}�[�����Z�b�g����<br>
     *   ��~���A���邢�͏I�����Ɏ󂯕t���\�B
     * 
     * @return <code>true</code>:�w����t / <code>false</code>:�w���ł����Ԃł͂Ȃ�
     */    
    public boolean reset()
    {
        boolean ret = false;
        if ((timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)||
            (timerStatus == ISampleMainCanvasStorage.TIMER_READY)||
            (timerStatus == ISampleMainCanvasStorage.TIMER_PAUSED))
        {
            currentTime = 0;
            timerStatus = ISampleMainCanvasStorage.TIMER_READY;
            ret = true;
        }
        return (ret);
    }

    /**
     *  �^�C���A�E�g�ݒ�b������������
     */
    public int getSetTimeSeconds()
    {
        return (setTime);
    }

    /**
     *   �^�C���A�E�g����܂ł̎c��b������������
     * @return �c��b���A���̂Ƃ��̓^�C���A�E�g������
     */
    public int getRestSeconds()
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)
        {
            return (-1);
        }
        return (setTime - currentTime);
    }

    /**
     *  �^�C���A�E�g��M���̏���
     *  @return �^�C���A�E�g�Ď����p������
     */
    public boolean reportCyclicTimeout(int count)
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            currentTime++;
            if (currentTime >= setTime)
            {
                // �^�C���A�E�g���o�I�I
                timerStatus = ISampleMainCanvasStorage.TIMER_TIMEOUT;
                
                // �^�C���A�E�g��񍐂���I
                reportDestination.finishUpdateData(0, ISampleMainCanvasStorage.TIMER_TIMEOUT, vibrateTime);
            }
            else
            {
                // ��ʂ��X�V����
                reportDestination.updateData(0);
            }
        }
        return (true);
    }


    /**
     *  �I�����̐����擾����
     * 
     * @return �I�����̐�
     */ 
    public  int numberOfSelections()
    {
        return (5);
    }

    /**
     *  �I�����̕�������擾����
     *  
     *  @param itemNumber �I����
     *  @return �I�����̕�����
     */
    public String getSelectionItem(int itemNumber)
    {
        String item = "";
        switch (itemNumber)
        {
          case 4:
            item = "�ݒ�b������";
            break;

          case 3:
            item = "7��(420�b) �ݒ�";
            break;

          case 2:
            item = "5��(300�b) �ݒ�";
            break;

          case 1:
            item = "3��(180�b) �ݒ�";
            break;

          case 0:
          default:
            item = "2��(120�b) �ݒ�";
            break;
        }
        return (item);
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
     *  �I�������A�C�e�����N���A����(�������Ȃ�) 
     */
    public void resetSelectionItems()
    {
        
    }

    /**
     *  �I�𒆂̃A�C�e��������������(0)
     * 
     *  @return �I�𒆂̃A�C�e����(0����������)
     */
    public int getNumberOfSelectedItems()
    {
        return (0);
    }

    /**
     *  �I�𒆂̂̃A�C�e��ID����������
     *  
     *  @param itemNumber �I�𒆂̃A�C�e���ԍ�(�[���X�^�[�g)
     *  @return �A�C�e��ID(���̂܂܉�������)
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
        selectedItemId = itemId;
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
            // �^�C�}�[�ݒ�l��ύX����
            switch (selectedItemId)
            {
              case 4:
                // �����ł͉������Ȃ�...
                break;

              case 3:
                setTime = 420;
                break;

              case 2:
                setTime = 300;
                break;

              case 1:
                setTime = 180;
                break;

              case 0:
              default:
                setTime = 120;
                break;
            }
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
        return (false);
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
        return ("");
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
    	if (screenId == fwSampleFactory.DATAINPUT_SCREEN)
    	{
    		return (fwSampleFactory.DEFAULT_SCREEN);
    	}
    	if (screenId == fwSampleFactory.SELECTION_SCREEN)
    	{
    		if (selectedItemId == 4)
    		{
    			return (fwSampleFactory.DATAINPUT_SCREEN);
    		}
    	}
        return (ISelectionScreenStorage.SCENE_TO_PREVIOUS);
    }

    /**
     *  �f�[�^���͂��L�����Z�����ꂽ�Ƃ��̏���
     */
	public void cancelEntry()
	{
	    inputField = null;
	}

    /**
     *  �f�[�^���͂�OK���ꂽ�Ƃ��̏���
     */
	public void dataEntry()
	{
        // �ݒ�b�����i�[����
        setTime = Integer.valueOf(inputField.getString()).intValue();
        inputField = null;	
	}

	/**
     *  �f�[�^���͂̂��߂̏�������
     */
	public void prepare()
	{
		
	}

    /**
	 *  �f�[�^���͏I������
	 */
	public void finish()
	{
	    System.gc();
	}

	/**
     *  �\������f�[�^���̓t�B�[���h�̌����擾����
     *  
     *  @return �\������f�[�^���̓t�B�[���h�̌�
     */
    public int getNumberOfDataInputField()
    {
    	return (1);
    }

    /**
     *  �f�[�^���̓t�B�[���h���擾����
     *  @param index �f�[�^�C���f�b�N�X�ԍ��i�O�X�^�[�g�j
     *  @return �e�L�X�g���̓t�B�[���h
     */
	public TextField getDataInputField(int index)
	{
		inputField = new TextField("�ݒ莞��(�P�ʁF�b)", "", 3, TextField.DECIMAL);
		return (inputField);
	}

	/**
     *  �\������I�v�V����(�I����͍���)�̌����擾����
     *  @return �\������I�v�V�����̌� 
     */
    public int getNumberOfOptionGroup()
    {
    	return (0);
    }
    
    /**
     *  �\������I�v�V����(�I����͍���)���擾����
     *  @param index �I�v�V�����C���f�b�N�X�ԍ��i�O�X�^�[�g�j
     *  @return �I�v�V����(�I����͍���)
     */
	public ChoiceGroup getOptionField(int index)
	{
		return (null);
	}

	/**
     *  ��ʐؑ֐�̃^�C�g�����擾����
     *  @param screenId ���ID
     *  @return ��ʃ^�C�g��
     */
	public String nextSceneTitleToChange(int screenId)
	{
		return ("");
	}

}
