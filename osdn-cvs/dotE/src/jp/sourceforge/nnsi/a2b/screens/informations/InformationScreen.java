/**
 *  ���\����ʗp�̃p�b�P�[�W�ł��B
 */
package jp.sourceforge.nnsi.a2b.screens.informations;
import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 * ���b�Z�[�W�\���p�̉��<br>
 * ��ʂɃ��b�Z�[�W��\�������ʂł��B
 * �E���ɃX�N���[���o�[��\�����A�X�N���[�����ĕ\�����邱�Ƃ��ł��܂��B
 * ��ʉ����ɕ����̃{�^�������Ă�������܂��B<br>
 * (��ʂ����Ƃ��ɑI�����ꂽ�{�^����ʒm���܂��B<br>
 * ���̂��߁A�m�F�_�C�A���O�Ƃ��Ă����p�\�ł��B
 *
 * @author MRSa
 *
 */
public class InformationScreen extends MidpScreenCanvas
{
    private final int AREAWIDTH_MARGIN       = 5;
	private final int AREAHEIGHT_MARGIN      = 3;
	private final int selectBackColor        = 0x000000ff;
	private final int selectForeColor        = 0x00ffffff;
	private final int unselectBackColor      = 0x00ffffff;
	private final int unselectForeColor      = 0x00000000;
	private final int buttonBorderColor      = 0x00000000;
	
    private InformationScreenOperator operation = null;  // ����N���X...

    private int nofButton = 0;
    private int selectedButtonId = 0;  // �f�t�H���g��OK
    private int topItem = 0;
    private int maxLine = 0;

    /**
     * �R���X�g���N�^�ł́A�K�v�ȃI�u�W�F�N�g���L�����܂�
     * @param screenId ���ID
     * @param object �V�[���Z���N�^
     * @param operator �f�[�^����N���X
     */
    public InformationScreen(int screenId, MidpSceneSelector object, InformationScreenOperator operator)
    {
        super(screenId, object);
        operation = operator;
    }

    /**
     *  �f�[�^�\�����J�n���܂�
     * 
     */
    public void start()
    {
        // �\�����郁�b�Z�[�W�����H����
    	int btnMargin = screenFont.getHeight() + (screenFont.getHeight() / 8);
    	operation.formatMessage(screenWidth, screenHeight - btnMargin, screenFont, true);
    }

    /**
     * �f�[�^�\����ʂ��I�����܂�
     * 
     */
    public void stop()
    {
        //
    }
    /**
     * ��ʕ`�揈���������Ȃ��܂��B
     * @param g �O���t�B�b�N�X
     * 
     */
    public void paint(Graphics g)
    {
        try
        {
            // �^�C�g���̈�̕`��
            drawTitleArea(g);

            // �{�^�������X�V����
            nofButton = operation.getNumberOfButtons(canvasId);
            
            int topY = screenFont.getHeight() + screenTopY;
            int height = screenHeight - screenFont.getHeight();
            if (nofButton != 0)
            {
                // ���C���p�l���̕`��
                int buttonAreaY = screenFont.getHeight() + screenFont.getHeight() / 8 + AREAHEIGHT_MARGIN * 2;
                drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, height, buttonAreaY);

                // �X�N���[���}�[�J�[�̕\��
                drawScrollMarker(g, screenTopX, topY, screenWidth, height - buttonAreaY);
                
                // �{�^���̈�̕`��
                drawButtonArea(g, screenTopX, screenTopY + screenHeight - buttonAreaY, screenWidth);
            }
            else
            {
                // ���C���p�l���̕`��
                drawMainPanel(g, screenTopX, topY, screenWidth - AREAWIDTH_MARGIN, height, 0);

                // �X�N���[���}�[�J�[�̕\��
                drawScrollMarker(g, screenTopX, topY, screenWidth, height);
            }
        }
        catch (Exception e)
        {
            // �Ȃ����k���|���o��̂�...
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
     * @param height  �E�B���h�E����
     */
    private void drawMainPanel(Graphics g, int topX, int topY, int width, int height, int buttonAreaHeight)
    {
        // �w�i����������
//        clearMainPanel(g, topX, topY, width + AREAWIDTH_MARGIN, height);
        clearMainPanel(g, topX, topY, width, height);

        // �t�H���g�T�C�Y
        int fontHeight = screenFont.getHeight();

        // �I�����̃��C������ݒ肷��
        maxLine = (height - buttonAreaHeight) / (fontHeight + 1) + 1;

        // �s���Ƃɕ\������...
        int maxCount  = operation.getMessageLine();
        g.setColor(unselectForeColor);
        for (int lines = 0; lines < maxLine; lines++)
        {
            String lineString = operation.getMessage(lines + topItem);
            if (lineString != null)
            {
                g.drawString(lineString, (topX + InformationScreenOperator.WIDTH_MARGIN), (topY + (fontHeight + InformationScreenOperator.HEIGHT_MARGIN) * lines), (Graphics.LEFT | Graphics.TOP));
            }
            if ((lines + topItem) >= (maxCount - 1))
            {
            	break;
            }
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

        int btnX = topX + width - (btnWidth + InformationScreenOperator.WIDTH_MARGIN);
        for (int id = nofButton; id != 0; id--)
        {
            // �E���̃{�^�����珇���`�悷��
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
            btnX = btnX - (btnWidth + InformationScreenOperator.WIDTH_MARGIN);
            if (btnX < btnWidth)
            {
                return;
            }
        }
        return;
    }
 
    /**
     * �X�N���[���}�[�J�[��`�悷��
     * 
     * @param g       �O���t�B�b�N�X�I�u�W�F�N�g
     * @param topX    ������W(X��)
     * @param topY    ������W(Y��)
     * @param width   �E�B���h�E��
     * @param height  �E�B���h�E����
     */
    private void drawScrollMarker(Graphics g, int topX, int topY, int width, int height)
    {
        int line = operation.getMessageLine();
    	if (maxLine >= line)
        {
            // �f�[�^�͈��ʂɂ����܂�ʁA�A�`�悹���I������
            return;
        }

        // ���C����`�悷��
        int w = topX + width - (AREAWIDTH_MARGIN - 2);
        g.setColor(unselectBackColor);
        g.fillRect(w, topY, (AREAWIDTH_MARGIN - 2), height);

        // �X�N���[���}�[�N��\������
        g.setColor(buttonBorderColor);
        g.drawLine(w, topY, w, topY + height);
        int par = 0;
        if (topItem != 0)
        {
        	par = (line * height) / topItem;
        }
        int start = par - 4;
        if (start < topY)
        {
        	start = topY;
        }
        else if (start >= topY + height)
        {
            start = (topY + height - 6);
        }
        g.fillRect(w, start, (AREAWIDTH_MARGIN - 2), 6);
        
        return;
    }

    /**
     * �{�^�����I������Ă��邩�ǂ����`�F�b�N����
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
     *  ���j���L�[�������ꂽ�Ƃ��̏���<br>
     *  �I�𒆂̃{�^�����E�̂��̂ɐ؂�ւ��܂��B
     */
    public void showMenu()
    {
		selectedButtonId++;
    	if (selectedButtonId == nofButton)
    	{
    	    selectedButtonId = 0;
    	}
        return;
    }

    /**
     *  ���L�[�������ꂽ�Ƃ��̏���<br>
     *  �I�𒆂̃{�^�������̂��̂ɐ؂�ւ��܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
    public boolean inputLeft(boolean isRepeat)
    {
		selectedButtonId--;
    	if (selectedButtonId < 0)
    	{
    	    selectedButtonId = nofButton - 1;
    	}
        return (true);
    }

    /**
     *  �E�L�[�������ꂽ�Ƃ��̏���<br>
     *  �I�𒆂̃{�^�����E�̂��̂ɐ؂�ւ��܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
    public boolean inputRight(boolean isRepeat)
    {
		selectedButtonId++;
    	if (selectedButtonId == nofButton)
    	{
    	    selectedButtonId = 0;
    	}
        return (true);
    }

    /**
     *  ��L�[�������ꂽ�Ƃ��̏���<br>
     *  �O�y�[�W��\�����܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputUp (boolean isRepeat)
    {
        topItem = topItem - (maxLine - 1);
        if (topItem < 0)
        {
            topItem = 0;
        }
        return (true);
    }

    /**
     *  ���L�[�������ꂽ�Ƃ��̏���<br>
     *  ���y�[�W��\�����܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputDown(boolean isRepeat)
	{
        topItem = topItem + (maxLine - 1);
        if (topItem >= operation.getMessageLine())
        {
            // ���y�[�W��\�����Ȃ� �i���ɖ߂��j
        	topItem = topItem - (maxLine - 1);
//        	topItem = operation.getMessageLine() - 1;
        }
        return (true);
    }

	/**
     *  �Z���^�[�L�[�������ꂽ�Ƃ��̏���<br>
     *  �{�^�������������ƂƂ��āA��ʂ�؂�ւ��܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputFire (boolean isRepeat)
    {
        operation.finishScreen(selectedButtonId);
        finishCanvas();
        return (true);
    }

	/**
     *  GAME C�L�[(�N���A�L�[)�������ꂽ�Ƃ��̏���<br>
     *  �L�����Z���{�^�������������ƂƂ��āA��ʂ�؂�ւ��܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
	public boolean inputGameC(boolean isRepeat)
    {
        operation.finishScreen(IInformationScreenStorage.BUTTON_CANCELED);
        finishCanvas();
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
