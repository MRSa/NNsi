import java.util.Vector;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * �R�}���h���s��ʂ̕\��...
 * @author MRSa
 *
 */
public class kaniFileScreenConfirmation implements IkaniFileCanvasPaint
{
	private final int backColor       = 0x00ffffff;
	private final int foreColor       = 0x00000000;
	private final int cursorBackColor = 0x000000f8;
	private final int cursorForeColor = 0x00ffffff;

	private kaniFileSceneSelector parent = null;
	private Font screenFont = null;   // �\���Ɏg�p����t�H���g
	private int screenMode   = -1;    // �\�����[�h
	private int screenTopX   = 0;     // �`��̈�̍��[(X)
	private int screenTopY   = 0;     // �`��̈�̍��[(Y)
	private int screenWidth  = 160;   // ��ʕ�
	private int screenHeight = 160;   // ��ʍ���
	private int screenLine   = 0;     // �f�[�^�\���\���C����

	private boolean busyMode = false;
	private int currentIndex  = 1;
	private String infoLine   = null;
	private Vector commandList = null;

	/**
	 * �R���X�g���N�^
	 * @param object
	 */
	kaniFileScreenConfirmation(kaniFileSceneSelector object)
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
		commandList  = new Vector(2);
	
		commandList.addElement("���s");
		commandList.addElement("���~");
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
		parent.memoryCurrentDirectory();
		return;
	}
	
	/**
	 * �\���f�[�^�̍X�V����...
	 * 
	 */
	public void updateData()
	{
		currentIndex     = 0;
		return;
	}

	/**
	 * 
	 * 
	 */
	public boolean inputBack()
	{
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputForward()
	{
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputPreviousItem()
	{
		currentIndex--;
		if (currentIndex < 0)
		{
			currentIndex = commandList.size() - 1;
		}
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNextItem()
	{
		currentIndex++;
		if (currentIndex >= commandList.size())
		{
			currentIndex = 0;
		}
		return (true);
	}

	/**
	 *  �O�y�[�W�̕\��
	 * 
	 */
	public boolean inputPreviousPage()
	{
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNextPage()
	{
		return (false);
	}

	/**
	 *  �R�}���h��I��
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
        boolean ret = false;
		switch (currentIndex)
		{
          case 0:
        	//
            ret = parent.confirmation();
        	break;

          case 1:
        	// ���s�L�����Z��
      		parent.cancelCommandInput();
        	break;
 		}
		return (ret);
	}

	/**
	 *  �I���L�����Z���B�B�B�R�}���h���[�h�𔲂���
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		parent.cancelCommandInput();
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputOther(boolean isRepeat)
	{
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputTop(boolean isRepeat)
	{
		return (true);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputBottom(boolean isRepeat)
	{
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
	 * 
	 */
	public boolean inputDummy()
	{
		return (false);
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

			if (busyMode != true)
			{
				// ���C���p�l���̕`��...
		        int infoTop = screenHeight;
				drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (infoTop - screenFont.getHeight()));
			}

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
			g.setColor(0);
			g.drawString(infoLine, screenTopX, screenTopY, (Graphics.LEFT | Graphics.TOP));
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
		busyMode = false;
		return;
	}
	
	
	/**
	 * ���̈�̏����X�V����
	 * 
	 */
	public void updateInformation(String message, int mode)
	{
		infoLine = message;
		if (mode < 0)
		{
			busyMode = true;
		}
		else
		{
			busyMode = false;
		}
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
		int fontHeight = screenFont.getHeight();
		
		g.setColor(backColor);
		g.fillRect(topX, topY, width, height);

		for (int index = 0; index < screenLine; index++)
		{
			g.setColor(foreColor);
			if (index >= commandList.size())
			{
				// �\�����ׂ��f�[�^�ʂ𒴂���...
				break;
			}
			String drawLine = (String) commandList.elementAt(index);
			if (index == currentIndex)
			{
				g.setColor(cursorBackColor);
				g.fillRect(topX, (topY + (fontHeight * index)), width, fontHeight);
				g.setColor(cursorForeColor);
			}
			g.drawString(drawLine, topX, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));			
		}
		return;
	}
}
