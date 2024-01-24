import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * �e�L�X�g�f�[�^�ҏW�\���N���X
 * 
 * @author MRSa
 *
 */
public class kaniFileScreenTextEdit implements IkaniFileCanvasPaint
{
	private final int backColor       = 0x00ffffff;
	private final int foreColor       = 0x00000000;
	private final int cursorBackColor = 0x000000f8;
	private final int cursorForeColor = 0x00ffffff;
	
	private final int SKIP_LENGTH     = 50;
//	private final int SKIP_LENGTH     = 18;

	private kaniFileSceneSelector parent   = null;
	private kaniFileTextFile      textData = null;
	private Font screenFont = null;   // �\���Ɏg�p����t�H���g
	private int screenMode   = -1;    // �\�����[�h
	private int screenTopX   = 0;     // �`��̈�̍��[(X)
	private int screenTopY   = 0;     // �`��̈�̍��[(Y)
	private int screenWidth  = 160;   // ��ʕ�
	private int screenHeight = 160;   // ��ʍ���
	private int screenLine   = 0;     // �f�[�^�\���\���C����

	private int screenStartWidth = 0; // �\���擪���C��
	
	private boolean isDrawAll = false;
	
	private String infoLine   = null;

	/**
	 * �R���X�g���N�^
	 * @param object
	 */
	kaniFileScreenTextEdit(kaniFileSceneSelector object)
	{
		//
		parent = object;
	}

	/**
	 * ��������...
	 */
	public void prepare(Font font, int mode)
	{
		//
		screenFont   = font;
		screenMode   = mode;
	}

	/**
     *  ��ʃT�C�Y��ݒ肷��...
	 * 
	 */
    public void setRegion(int topX, int topY, int width, int height)
    {
		screenTopX   = topX;
		screenTopY   = topY;
		screenWidth  = width;
		screenHeight = height;
		screenLine   = decideScreenLine(height);
    }

	/**
	 * ����[X���W����������
	 * 
	 */
	public int getTopX()
	{
		return (screenTopX);
	}

    /**
     * ���[�h����������...
     */
    public int getMode()
    {
    	return (screenMode);
    }

    /**
	 * ����[Y���W����������
	 * 
	 */
	public int getTopY()
	{
		return (screenTopY);
	}

	/**
	 * �`��E�B���h�E������������
	 * 
	 */
	public int getWidth()
	{
		return (screenWidth);
	}

	/**
	 * �`��E�B���h�E��������������
	 * 
	 */
	public int getHeight()
	{
		return (screenHeight);
	}
	
	/**
	 * �\���\���C���������߂�
	 * @param font
	 * @param height
	 * @param mode
	 * @return
	 */
	private int decideScreenLine(int height)
	{
	    return (((height - (screenFont.getHeight() * 1)) / screenFont.getHeight()));
	}

	/**
	 *  ��ʐ؂�ւ����\�b�h...
	 * 
	 */
	public void changeScene()
	{
		textData = parent.getTextFileData();
		isDrawAll = true;
		return;
	}
	
	/**
	 * �\���f�[�^�̍X�V����...
	 * 
	 */
	public void updateData()
	{
		textData.setCurrentTopLine(0);
		textData.setCurrentSelectedLine(0);
		isDrawAll = true;
		return;
	}

	/**
	 *  ���ړ�...
	 * 
	 */
	public boolean inputPreviousPage()
	{
		screenStartWidth = screenStartWidth - (screenWidth - SKIP_LENGTH);
		if (screenStartWidth < 0)
		{
			screenStartWidth = 0;
		}
		isDrawAll = true;
		return (true);
	}

	/**
	 * �E�ړ�...
	 * 
	 */
	public boolean inputNextPage()
	{
		screenStartWidth = screenStartWidth + (screenWidth - SKIP_LENGTH);
		isDrawAll = true;
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputPreviousItem()
	{
        int selLine = textData.getCurrentSelectedLine();
		int topLine = textData.getCurrentTopLine();
		
		if (selLine == 0)
		{
			return (false);
		}
		selLine--;
		isDrawAll = false;
		if (selLine < topLine)
		{
			topLine = topLine - (screenLine - 1);
			if (topLine < 0)
			{
				topLine = 0;
			}
			textData.setCurrentTopLine(topLine);
			isDrawAll = true;
		}
		textData.setCurrentSelectedLine(selLine);
		
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNextItem()
	{
        int selLine = textData.getCurrentSelectedLine();
		int topLine = textData.getCurrentTopLine();
        int btmLine = textData.getTotalLine();
        
        if (selLine == (btmLine - 1))
        {
        	return (false);
        }
		selLine++;
		isDrawAll = false;
		if (selLine > (topLine + (screenLine - 1)))
		{
			topLine = topLine + (screenLine - 1);
			textData.setCurrentTopLine(topLine);
			isDrawAll = true;
		}
		textData.setCurrentSelectedLine(selLine);
		return (true);
	}

	/**
	 *  �O�y�[�W�̕\��
	 * 
	 */
	public boolean inputBack()
	{
        int selLine = textData.getCurrentSelectedLine();
		int topLine = textData.getCurrentTopLine();
		
		if (selLine == 0)
		{
			return (false);
		}
		topLine = topLine - screenLine - 1;
		if (topLine < 0)
		{
			topLine = 0;
		}
		textData.setCurrentTopLine(topLine);
		textData.setCurrentSelectedLine(topLine);

		isDrawAll = true;
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputForward()
	{
		int topLine = textData.getCurrentTopLine();
        int btmLine = textData.getTotalLine();
        
		topLine = topLine + screenLine - 1;
		if (topLine >= (btmLine - 1))
		{
			topLine = btmLine - screenLine - 2;
		}
		textData.setCurrentTopLine(topLine);
		textData.setCurrentSelectedLine(topLine);

		isDrawAll = true;
		return (true);
	}

	/**
	 *  �R�}���h��I��
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
        boolean ret = false;
        
        // �ҏW���[�h��...        
        parent.selectedTextEditLine();
        return (ret);
	}

	/**
	 *  �I���L�����Z���B�B�B�R�}���h���[�h�𔲂���
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		parent.exitTextEdit();
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputOther(boolean isRepeat)
	{
		if (isRepeat == true)
		{
            // �L�[���s�[�g���ɂ́A�u�u���E�U�ŊJ���v��I���������Ƃɂ���
            parent.selectedOpenWebBrowser();
            return (true);
		}
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputTop(boolean isRepeat)
	{
		textData.setCurrentTopLine(0);
		textData.setCurrentSelectedLine(0);
		isDrawAll = true;
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputBottom(boolean isRepeat)
	{
        int btmLine = textData.getTotalLine();
		textData.setCurrentTopLine(btmLine - screenLine);
		textData.setCurrentSelectedLine(btmLine - 1);
		isDrawAll = true;
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNumber(int number)
	{
		return (true);
	}

	/**
	 *  �ҏW�����t�@�C���̕ۑ�
	 * 
	 */
	public boolean inputDummy()
	{
	    // �ҏW�R�}���h���[�h�ɐ؂�ւ���
		parent.selectedEditTextCommand();
		return (true);
	}
	
	/**
	 * ��ʂ�\��
	 * 
	 */
	public void paint(Graphics g)
	{
		try
		{
			// �^�C�g���̈�̕`��
			g.setColor(0x00d8ffd8);
            g.fillRect(screenTopX, screenTopY, screenWidth, screenFont.getHeight());
			drawTitleArea(g);

			// ���C���p�l���̕`��...
		    int infoTop = screenHeight;
			drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (infoTop - screenFont.getHeight()));
		}
		catch (Exception e)
		{
			// �Ȃ����k���|���o��̂�...
		}
		return;
	}

	/**
	 * �^�C�g���̈�ɕ`�悷��
	 * @param g
	 */
	private void drawTitleArea(Graphics g)
	{
		if (infoLine != null)
		{
			String appendData = "[" + (textData.getCurrentSelectedLine() + 1) + ":" + textData.getTotalLine() + "] ";
			if (textData.isModified() == true)
			{
				appendData = "*" + appendData;
			}
			else
			{
				appendData = " " + appendData;
			}
			g.setColor(0);
			g.drawString(appendData + infoLine, screenTopX, screenTopY, (Graphics.LEFT | Graphics.TOP));
		}
		return;
	}

	/**
	 * ���̈�ɏ���ݒ肷��
	 * 
	 */
	public void setInformation(String message)
	{
		infoLine = message;
		return;
	}
	
	/**
	 * ���̈�̏����X�V����
	 * 
	 */
	public void updateInformation(String message, int mode)
	{
		infoLine = message;
		return;		
	}
	/**
	 * ���C���p�l���̈�ɕ`�悷��
	 * 
	 * @param g
	 * @param top
	 * @param height
	 */
	private void drawMainPanel(Graphics g, int topX, int topY, int width, int height)
	{
		int fontHeight   = screenFont.getHeight();
		int dataLine     = textData.getTotalLine();
		int currentIndex = textData.getCurrentSelectedLine();
        int offset       = textData.getCurrentTopLine();

        g.setColor(backColor);
        if (isDrawAll == true)
        {
            g.fillRect(topX, topY, width, height);
        }

		for (int index = 0; index <  + screenLine; index++)
		{
			g.setColor(foreColor);
			if (index >= dataLine)
			{
				// �\�����ׂ��f�[�^�ʂ𒴂���...
				break;
			}
			
			if ((isDrawAll == false)&&((index < currentIndex - offset - 1)||(index > currentIndex - offset + 1)))
			{
				continue;
			}

			String drawLine = new String(textData.getLine(index + offset));
			if (index == currentIndex - offset)
			{
				g.setColor(cursorBackColor);
				g.fillRect(topX, (topY + (fontHeight * index)), width, fontHeight);
				g.setColor(cursorForeColor);
			}
			else
			{
				g.setColor(backColor);
				g.fillRect(topX, (topY + (fontHeight * index)), width, fontHeight);
				g.setColor(foreColor);				
			}
			if (screenStartWidth > 0)
			{
		        int contWidth = screenFont.charWidth('$');
				int len       = drawLine.length();
				int offsetW   = 1;
				g.drawString("$", topX, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));
				while ((offsetW < len)&&(screenFont.substringWidth(drawLine, 0, offsetW) < screenStartWidth))
				{
				    offsetW = offsetW + 4;
				}
				if (len > offsetW)
				{
                    g.drawSubstring(drawLine, offsetW, (len - offsetW), topX + contWidth + 1, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));
				}
			}
			else
			{
				g.drawString(drawLine, topX, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));
			}
		}
		isDrawAll = false;
		return;
	}
}
