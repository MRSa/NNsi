package jp.sourceforge.nnsi.a2b.screens.selections;
import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 * �A�C�e���I��p�̉��<br>
 * {@link jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas}�̍����Ƃ���
 * �쐬���Ă��܂��B
 *
 * @see jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas
 *
 * @author MRSa
 *
 */
public class SelectionScreen extends MidpScreenCanvas
{
    private final int SELECTION_JUMPCOUNT    = 3;
	private final int AREAHEIGHT_MARGIN      = 3;
	private final int buttonLineForeColor    = 0x00202020;
	private final int buttonBorderColor      = 0x00000000;
	private final int selectBackColor        = 0x000000ff;
	private final int selectForeColor        = 0x00ffffff;
	private final int unselectForeColor      = 0x00000000;
    private final int upDownPageForeColor    = 0x00008f00;
	//	private final int unselectBackColor      = 0x00ffffff;

    private SelectionScreenOperator operation = null;  // ����N���X...
    private int buttonSelection = 0;
    private int selectedButtonId = 0;
    private int topItem = 0;
    private int maxLine = 0;
    private int nofButton = 0;

    /**
     * �R���X�g���N�^�ł́A��ʂ�ID��K�v�ȃ��[�e�B���e�B��ݒ肵�܂�
     * @param screenId ���ID
     * @param object �V�[���Z���N�^
     * @param operator �I����ʗp���샆�[�e�B���e�B
     */
    public SelectionScreen(int screenId, MidpSceneSelector object, SelectionScreenOperator operator)
    {
        super(screenId, object);
        operation = operator;
    }

    /**
     *  �f�[�^�\���̊J�n
     * 
     */
    public void start()
    {
        // �A�C�e����������
        if (operation.isItemInitialize() == true)
        {
            selectedButtonId = 0;
    	    topItem = 0;
            buttonSelection = 0;
        }
    }

    /**
     * �f�[�^�\���̏I��
     * 
     */
    public void stop()
    {
        // 
    }

    /**
     * ��ʕ`��̎��{
     * 
     * @param g �O���t�B�b�N
     * 
     */
    public void paint(Graphics g)
    {
        try
        {        	
        	// �^�C�g���̈�̕`��
            int titleAreaHeight = screenFont.getHeight();
            if (operation.isShowTitle() == true)
            {
                drawTitleArea(g);
            }
            else
            {
            	titleAreaHeight = 0;
            }

            // �{�^�������X�V����
            nofButton = operation.getNumberOfButtons();

            if (nofButton != 0)
            {
                // ���C���p�l���̕`��
                int buttonAreaY = screenFont.getHeight() + screenFont.getHeight() / 8 + AREAHEIGHT_MARGIN * 2;
                drawMainPanel(g, screenTopX, screenTopY + titleAreaHeight, screenWidth, screenHeight - titleAreaHeight, buttonAreaY);

                // �{�^���̈�̕`��
                drawButtonArea(g, screenTopX, screenTopY + screenHeight - buttonAreaY, screenWidth);
            }
            else
            {
                // ���C���p�l���̕`��
                drawMainPanel(g, screenTopX, screenTopY + titleAreaHeight, screenWidth, screenHeight - titleAreaHeight, 0);
            }
        }
        catch (Exception e)
        {
            // �Ȃ����k���|���o��̂�...
        }
        return;
    }

    /**
     * �{�^���̈��`�悷��
     * 
     * @param g       �O���t�B�b�N�X�I�u�W�F�N�g
     * @param topX    ������W(X��)
     * @param topY    ������W(Y��)
     * @param width   �E�B���h�E��
     * @param height  �E�B���h�E����
     */
    private void drawButtonArea(Graphics g, int topX, int topY, int width)
    {
        // �{�^���ʒu�̐ݒ�...
        int btnWidth   = screenFont.stringWidth(" Cancel "); 
        int btnHeight   = screenFont.getHeight() + screenFont.getHeight() / 8;
        
        // ���C���̕\��
    	g.setColor(buttonLineForeColor);
        g.drawLine((topX + SelectionScreenOperator.WIDTH_MARGIN), (topY + 1), (topX + width - (SelectionScreenOperator.WIDTH_MARGIN)), (topY + 1));

        int btnX = topX + width - (btnWidth + SelectionScreenOperator.WIDTH_MARGIN);
        for (int id = nofButton; id != 0; id--)
        {
            String label = operation.getButtonLabel(id - 1);
            if (isSelectedButton(id - 1) == true)
            {
                // �{�^���I�𒆏��
                g.setColor(selectBackColor);
                g.fillRoundRect(btnX, (topY + AREAHEIGHT_MARGIN), btnWidth, btnHeight, 4, 4);
                g.setColor(buttonBorderColor);
                g.drawRoundRect(btnX, (topY + AREAHEIGHT_MARGIN), btnWidth, btnHeight , 4, 4);
                g.setColor(selectForeColor);
                g.drawString(label, btnX + (btnWidth / 2), topY + 5, (Graphics.HCENTER | Graphics.TOP));        	
            }
            else
            {
                // �{�^����I�����
                g.setColor(buttonBorderColor);
                g.drawRoundRect(btnX, (topY + AREAHEIGHT_MARGIN), btnWidth, btnHeight , 4, 4);
                g.drawString(label, btnX + (btnWidth / 2), topY + 5, (Graphics.HCENTER | Graphics.TOP));        	
            }
            btnX = btnX - (btnWidth + SelectionScreenOperator.WIDTH_MARGIN);
            if (btnX < topX)
            {
            	// �{�^���\���G���A���s�������ꍇ�A�I������
                return;
            }
        }
        // �󂫃G���A������΃K�C�h���b�Z�[�W��\������
        String guide = operation.getGuideMessage();
        if (guide.length() == 0)
        {
            // �K�C�h���b�Z�[�W���Ȃ��ꍇ�A�I�����̃A�C�e������\������
        	guide = (buttonSelection + 1) + "/" + operation.getSelectionItemCount();
        }
        int area = btnX + btnWidth - (topX + SelectionScreenOperator.WIDTH_MARGIN);
        if (area > screenFont.stringWidth(guide))
        {
            g.setColor(buttonBorderColor);
            g.drawString(guide, topX + SelectionScreenOperator.WIDTH_MARGIN, topY + AREAHEIGHT_MARGIN + 2, (Graphics.LEFT | Graphics.TOP));
        }
        return;
    }

    /**
     * ���C���p�l���̈�ɕ`�悷��
     * 
     * @param g       �O���t�B�b�N�X�I�u�W�F�N�g
     * @param topX    ������W(X��)
     * @param topY    ������W(Y��)
     * @param width   �E�B���h�E��
     * @param height  �E�B���h�E����(�{�^���̈捂�����܂�)
     * @param buttonAreaHeight �{�^���̈捂��
     */
    private void drawMainPanel(Graphics g, int topX, int topY, int width, int height, int buttonAreaHeight)
    {
        // �w�i����������
        clearMainPanel(g, topX, topY, width, height);
        
        // �t�H���g�T�C�Y
        int fontHeight = screenFont.getHeight();

        // �I�����̃��C������ݒ肷��
        maxLine = (height - buttonAreaHeight) / (fontHeight + 1) + 1;
        int maxCount = operation.getSelectionItemCount();
        g.setColor(0);
        for (int lines = 0; lines < maxLine; lines++)
        {
            String itemString = operation.getSelectionItem(lines + topItem);
            if (itemString.length() != 0)
            {
                if ((lines + topItem) == buttonSelection)
                {
            	    g.setColor(selectBackColor);
            	    g.fillRect(topX, (topY + (fontHeight * lines)), (width), (fontHeight + 1));
                    g.setColor(selectForeColor);
            	    g.drawString(itemString, (topX + SelectionScreenOperator.WIDTH_MARGIN), (topY + (fontHeight + SelectionScreenOperator.HEIGHT_MARGIN) * lines), (Graphics.LEFT | Graphics.TOP));
                }
                else
                {
                	g.setColor(operation.getSelectionBackColor(lines));
            	    g.fillRect(topX, (topY + (fontHeight * lines)), (width), (fontHeight + 1));
                    g.setColor(unselectForeColor);
            	    g.drawString(itemString, (topX + SelectionScreenOperator.WIDTH_MARGIN), (topY + (fontHeight + SelectionScreenOperator.HEIGHT_MARGIN) * lines), (Graphics.LEFT | Graphics.TOP));
                }
            }
            if ((lines + topItem) >= (maxCount - 1))
            {
            	break;
            }
        }

        if (topItem != 0)
        {
            // �I�������܂���ɂ���ꍇ...
            g.setColor(upDownPageForeColor);
            g.drawString("��", (topX + width - screenFont.stringWidth("�� ")), (topY + SelectionScreenOperator.HEIGHT_MARGIN), (Graphics.LEFT | Graphics.TOP));
        }
        
        if ((topItem + maxLine) < maxCount)
        {
            // �I�������܂����ɂ���ꍇ...
            g.setColor(upDownPageForeColor);
            g.drawString("��", (topX + width - screenFont.stringWidth("�� ")), (topY + (fontHeight + SelectionScreenOperator.HEIGHT_MARGIN) * (maxLine - 1)), (Graphics.LEFT | Graphics.TOP));
        }
        return;
    }

    /**
     * �{�^�����I������Ă��邩�ǂ����`�F�b�N���܂�
     * 
     * @param buttonId �I������Ă��邩�ǂ����`�F�b�N����{�^��ID
     * @return �I������Ă���(true) / �I������Ă��Ȃ�(false)
     */
    public boolean isSelectedButton(int buttonId)
    {
        if (selectedButtonId == buttonId)
        {
            return (true);
        }
    	return (false);
    }
    
    /**
     *  ��L�[�������ꂽ�Ƃ��̏���<br>
     *  �O�̃A�C�e����I�����܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
    public boolean inputUp   (boolean isRepeat)
    {
        if (buttonSelection > 0)
        {
            buttonSelection--;            
        }
        if (topItem > buttonSelection)
        {
            topItem = topItem - (maxLine - 1);
            if (topItem < 0)
            {
            	topItem = 0;
            }
        }
        return (true);
    }

    /**
     *  ���L�[�������ꂽ�Ƃ��̏���<br>
     *  ���̃A�C�e����I�����܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputDown (boolean isRepeat)
    {
        if (buttonSelection < (operation.getSelectionItemCount() - 1))
        {
            buttonSelection++;
        }
        if ((topItem + maxLine) <= buttonSelection)
        {
            topItem = topItem + (maxLine - 1);
            if (topItem >= operation.getSelectionItemCount())
            {
            	topItem = operation.getSelectionItemCount() - 1;
            }
        }
        return (true);
    }

    /**
     *  ���L�[�������ꂽ�Ƃ��̏���<br>
     *  �O�y�[�W��\�����܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputLeft (boolean isRepeat)
    {
        if (buttonSelection > 0)
        {
            buttonSelection = buttonSelection - (maxLine - 1);
            if (buttonSelection < 0)
            {
            	buttonSelection = 0;
            }
        }
        if (topItem > buttonSelection)
        {
            topItem = topItem - (maxLine - 1);
            if (topItem < 0)
            {
            	topItem = 0;
            }
        }
        return (true);
    }

    /**
     *  �E�L�[�������ꂽ�Ƃ��̏���<br>
     *  ���y�[�W��\�����܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputRight(boolean isRepeat)
    {
        if (buttonSelection < (operation.getSelectionItemCount() - 1))
        {
            buttonSelection = buttonSelection + (maxLine - 1);
            if (buttonSelection > (operation.getSelectionItemCount() - 1))
            {
            	buttonSelection = operation.getSelectionItemCount() - 1;
            }
        }
        if ((topItem + maxLine) <= buttonSelection)
        {
            topItem = topItem + (maxLine - 1);
            if (topItem >= operation.getSelectionItemCount())
            {
            	topItem = operation.getSelectionItemCount() - 1;
            }
        }
        return (true);
    }

	/**
     *  ���j���[�\�����w�����ꂽ�Ƃ��̏���<br>
     *  �i�I�𒆂̃{�^����ύX���܂��j
     */
    public void showMenu()
    {
        if (nofButton == 1)
        {
            // �I�������A�C�e����ʒm����
        	operation.selectedItem(buttonSelection);

            // �{�^���������ꂽ���Ƃ�ʒm����
        	if (operation.triggeredMenu() == true)
        	{
        		finishCanvas();
        	}
            return;
        }
        if (nofButton == 0)
        {
        	// �{�^�������݂��Ȃ��Ƃ��́A�����N���[�Y����̂�
        	if (operation.triggeredMenu() == true)
        	{
        		finishCanvas();
        	}
        }

        // �I�𒆂̃{�^����؂�ւ���
        selectedButtonId++;
    	if (selectedButtonId == nofButton)
    	{
    	    selectedButtonId = 0;
    	}
        return;
    }

	/**
     *  �Z���^�[�L�[�������ꂽ�Ƃ��̏���<br>
     *  �A�C�e����I�����܂��B�����A�C�e���I�����[�h�̏ꍇ�ɂ́A�A�C�e���̑I���݂̂��s���܂��B<br>
     *  �i���̏ꍇ�A�{�^���������ŃA�C�e�����m�肵�܂��B�j
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputFire (boolean isRepeat)
    {
        // �I�������A�C�e�����L�^����
        operation.selectedItem(buttonSelection);
        if ((isRepeat == true)||(operation.isSelectedItemMulti() != true))
        {
            // �{�^���������A���邢�̓V���O���A�C�e���I�����[�h�Ȃ�I��
        	if (operation.selectedCommandButton(selectedButtonId) == true)
        	{
                finishCanvas();
        	}
        }
        return (true);
    }

	/**
     *  GAME D�L�[�������ꂽ�Ƃ��̏���<br>
     *  �I�𒼌�ɉ�ʂ�؂�ւ��܂��B<br>
     *  �i�قƂ�ǃZ���^�[�L�[�̑���Ɠ����ł����A�����I�����[�h�ł��A�{�^����������
     *  �A�C�e�����m�肳���܂��j
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputGameD(boolean isRepeat)
    {
        // �I�������A�C�e�����L�^���ďI������
        operation.selectedItem(buttonSelection);
        if (operation.selectedCommandButton(selectedButtonId) == true)
        {
            finishCanvas();
        }
        return (true);
    }

	/**
     *  GAME C�L�[(�N���A�L�[)�������ꂽ�Ƃ��̏���<br>
     *  �I�����L�����Z�����A��ʂ�؂�ւ��܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputGameC(boolean isRepeat)
    {
        // �����ꂽ�{�^�����L�^���ďI������
		if (operation.selectedCommandButton(ISelectionScreenStorage.BUTTON_CANCELED) == true)
		{
            finishCanvas();
		}
        return (true);
    }

	/**
     *  GAME A�L�[�������ꂽ�Ƃ��̏���<br>
     *  ������x�i�R�j��΂��ď�փA�C�e�����ړ������܂�
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputGameA(boolean isRepeat)
    {
        if (buttonSelection > 0)
        {
            buttonSelection = buttonSelection - SELECTION_JUMPCOUNT;
            if (buttonSelection < 0)
            {
            	buttonSelection = 0;
            }
        }
        if (topItem > buttonSelection)
        {
            topItem = topItem - (maxLine - 1);
            if (topItem < 0)
            {
            	topItem = 0;
            }
        }
        return (true);
    }

    /**
     *  GAME B�L�[�������ꂽ�Ƃ��̏���<br>
     *  ������x�i�R�j��΂��ĉ��փA�C�e�����ړ������܂�
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputGameB(boolean isRepeat)
    {
        if (buttonSelection < (operation.getSelectionItemCount() - 1))
        {
            buttonSelection = buttonSelection + SELECTION_JUMPCOUNT;
            if (buttonSelection > (operation.getSelectionItemCount() - 1))
            {
            	buttonSelection = operation.getSelectionItemCount() - 1;
            }
        }
        if ((topItem + maxLine) <= buttonSelection)
        {
            topItem = topItem + (maxLine - 1);
            if (topItem >= operation.getSelectionItemCount())
            {
            	topItem = operation.getSelectionItemCount() - 1;
            }
        }
        return (true);
    }

    /**
     *  ���L�[�������ꂽ�Ƃ��̏���<br>
     *  �i�A�C�e���̑I���𖖔��̃A�C�e���ֈړ������܂��j
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputStar(boolean isRepeat)
    {
		topItem = 0;
		buttonSelection = 0;
        return (true);
    }

	/**
     *  ���L�[�������ꂽ�Ƃ��̏���<br>
     *  �i�A�C�e���̑I���𖖔��̃A�C�e���ֈړ������܂��j
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputPound(boolean isRepeat)
    {
		int nofPage = ((operation.getSelectionItemCount() - 1) / (maxLine - 1));
        topItem = nofPage * (maxLine - 1);
        if (topItem < 0)
        {
            topItem = 0;
        }
        buttonSelection = operation.getSelectionItemCount() - 1;
        return (true);
    }

	/**
	 *   ���̉�ʂ֐؂�ւ���
	 *   �i�ǂ̉�ʂɐ؂�ւ��邩�͑��샆�[�e�B���e�B�ɖ₢���킹�Č��߂�j
	 * 
	 */
	protected void finishCanvas()
	{
		int nextScene = operation.nextSceneToChange(canvasId);
		if (nextScene < 0)
		{
		    parent.previousScene();
		}
		else
		{
			parent.changeScene(nextScene, null);
		}
        return;
	}
}
