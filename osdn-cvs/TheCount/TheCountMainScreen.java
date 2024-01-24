import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;


/**
 * �uTheCount�v�̃��C����� 
 *  @author MRSa
 *
 */
public class TheCountMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
	private TheCountDataStorage  dataStorage = null;

    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public TheCountMainScreen(int screenId, MidpSceneSelector object, TheCountDataStorage itemStorage)
    {
        super(screenId, object);
        dataStorage = itemStorage;
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     * 
     */
    public void start()
    {
        // �L�^����t�@�C�������擾����       
        String directory = dataStorage.getBaseDirectory();
        if (directory == null)
        {
            parent.changeScene(TheCountSceneDB.WELCOME_SCREEN, "�悤�����I");
        }
        System.gc();
        return;
    }

    /**
     * ��ʂ�`�悷��
     * 
     */
    public void paint(Graphics g)
    {
    	if (g == null)
    	{
            return;
    	}    	
    	
        // �^�C�g���Ɖ�ʑS�̂̃N���A�͐e�i�p�����j�̋@�\���g��
    	super.paint(g);
    	
        ////////// ��ʂ̕`�� //////////
    	try
    	{
    		// ������@��\������
        	int fontHeight = screenFont.getHeight() + 1; 
        	drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 6) - 5, screenWidth - 20, (fontHeight * 6));

        	// ��񃁃b�Z�[�W��\������
            String msg = dataStorage.getInformationMessage();
            if ((msg != null)&&(msg.length() > 0))
            {
                // ��񃁃b�Z�[�W��\������
            	g.drawString(msg, (screenTopX + 4), (screenTopY + (fontHeight * 1)), (Graphics.LEFT | Graphics.TOP));
            	g.drawString(msg, (screenTopX + 5), (screenTopY + (fontHeight * 1) + 1), (Graphics.LEFT | Graphics.TOP));
            }
        	
            ///// ��������\������ /////
            g.setColor(foreColorForOpe);
            g.drawString("������ :" + dataStorage.getTotalValueCount(), (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));

            ///// �f�[�^�l��\������  /////
            for (int index = 0; index < 6; index++)
            {
                drawItemData(g, (screenTopX + 5), (screenTopY + (fontHeight * (3 + index))), foreColorForOpe, index);
            }
    	}
    	catch (Exception ex)
    	{
    		//
    	}
        return;
    }

    /**
     *   �A�C�e����\������
     * @param g
     * @param topX
     * @param topY
     * @param itemIndex
     */
    private void drawItemData(Graphics g, int topX, int topY, int foreColor, int itemIndex)
    {
    	int value = dataStorage.getItemValue(itemIndex);
    	String percent = dataStorage.getValuePercent(itemIndex) + "%";
    	String msg = ": " + value;
    	g.setColor(foreColor);
        g.drawString("[" + (itemIndex + 1) + "] " + dataStorage.getItemName(itemIndex), topX + 5, topY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(msg, topX + 120, topY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(percent, topX + 180, topY, (Graphics.LEFT | Graphics.TOP));
    	return;
    }
    
    /**
     *   ��ʂɑ���K�C�h��\������
     * 
     * @param g
     * @param topX
     * @param topY
     * @param width
     * @param height
     */
    private void drawOperationGuide(Graphics g, int topX, int topY, int width, int height)
    {
    	// �g��\������
        g.setColor(backColorForOpe);
        g.fillRect(topX, topY, width, height);

        g.setColor(foreColorForOpe);
        g.drawRect(topX, topY, width, height);

        // �������\������
        int aY = topY + 2;
        g.drawString("������������@������", topX + 5, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight() + 2;
        g.drawString("1 - 6",            topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �J�E���g�A�b�v", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("0",                topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ���Z�b�g",       topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Enter",            topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �O���t�\��",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �A�C�e�����ݒ�", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("�~",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �A�v���I��",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));

    }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��F������������͉�ʂ֑J�ڂ���
     * 
     */
    public void showMenu()
    {
    	parent.changeScene(TheCountSceneDB.INPUT_SCREEN, "�A�C�e�����ݒ�");
        return;
    }

    /**
     *  �A�v���P�[�V�������I�������邩�H
     * 
     *  @return  true : �I��������Afalse : �I�������Ȃ�
     */
    public boolean checkExit()
    {
        return (true);
    }

    /**
     *  �����L�[�����͂��ꂽ�Ƃ��̏���
     *  @param number ���͂��ꂽ����
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     *
     */
    public boolean inputNumber(int number, boolean isRepeat)
    {
    	if (isRepeat == true)
    	{
    		// ���s�[�g���o���ɂ͉������Ȃ��B
    		return (false);
    	}
    	boolean ret = true;
    	switch (number)
    	{
    	  case 1:
    		// �J�E���^�C���N�������g�I
      		dataStorage.addItemValue(0, 1);
      		break;
    	  case 2:
      		// �J�E���^�C���N�������g�I
        	dataStorage.addItemValue(1, 1);
      		break;
    	  case 3:
      		// �J�E���^�C���N�������g�I
        	dataStorage.addItemValue(2, 1);
      		break;
    	  case 4:
      		// �J�E���^�C���N�������g�I
        	dataStorage.addItemValue(3, 1);
      		break;
    	  case 5:
      		// �J�E���^�C���N�������g�I
        	dataStorage.addItemValue(4, 1);
      		break;
    	  case 6:
      		// �J�E���^�C���N�������g�I
        	dataStorage.addItemValue(5, 1);
      		break;
    	  
    	  case 0:
    		// �J�E���^���Z�b�g�I
    		dataStorage.setItemValue(0, 0);
    		dataStorage.setItemValue(1, 0);
    		dataStorage.setItemValue(2, 0);
    		dataStorage.setItemValue(3, 0);
    		dataStorage.setItemValue(4, 0);
    		dataStorage.setItemValue(5, 0);
      		break;

    	  default:
    		ret = false;
    		break;
    	}
    	return (ret);
    }

    /**
     * ���̑��̃{�^���������ꂽ�Ƃ��F
     * 
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
    	return (false);
    }
    
    /**
     * ���{�^���������ꂽ�Ƃ��F �O���t�\��...
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
    	parent.changeScene(TheCountSceneDB.SUMMARY_SCREEN, "");
    	return (true);
    }

    /**
     * GAME C�{�^���������ꂽ�Ƃ�
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	dataStorage.setInformationMessage("");
    	return (true);
    }

    /**
     * GAME D�{�^���������ꂽ�Ƃ�
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
    	return (false);
    }
}
