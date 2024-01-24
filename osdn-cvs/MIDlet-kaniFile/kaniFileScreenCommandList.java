import java.util.Vector;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * �R�}���h�I����ʂ̕\��...
 * @author MRSa
 *
 */
public class kaniFileScreenCommandList implements IkaniFileCanvasPaint
{
//	static public final int selectionList    = 13;
	static public final int selectionList    = 7;
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

	private int currentIndex  = 0;
	private String infoLine   = null;
	private Vector commandList = null;

	/**
	 * �R���X�g���N�^
	 * @param object
	 */
	kaniFileScreenCommandList(kaniFileSceneSelector object)
	{
		//
		parent = object;
	}

	/**
	 * ��������...
	 */
	public void prepare(Font font, int mode)
	{
		screenFont   = font;
		screenMode   = mode;
		
		if (mode == kaniFileCanvas.SCENE_FILEOPEMODE)
		{
			prepareCommandListFileOpeMode();
		}
		else
		{
			prepareCommandListTextEdit();
		}
		return;
	}

	/**
	 *  �ʏ�p�̃R�}���h�ꗗ�̏���...
	 */
	private void prepareCommandListFileOpeMode()
	{
		commandList  = new Vector(selectionList);
		commandList.addElement("�u���E�U�ŊJ��");
		commandList.addElement("�e�L�X�g�ҏW");
		commandList.addElement("�摜�\��");
		commandList.addElement("�t�@�C���ꗗ�o��");
		commandList.addElement("Dir�쐬");
		commandList.addElement("���l�[��");
		commandList.addElement("�����ύX");
		commandList.addElement("Snd�Đ�/��~");
		commandList.addElement("ZIP�W�J");
		commandList.addElement("ZIP�W�J(��Dir.)");
		commandList.addElement("����");
		commandList.addElement("�R�s�[");
		commandList.addElement("�폜");
		commandList.addElement("�I��");
		return;
	}

	/**
	 *  �e�L�X�g�ҏW�p�̃R�}���h�ꗗ�̏���...
	 *
	 */
	private void prepareCommandListTextEdit()
	{
		commandList  = new Vector(selectionList);
		commandList.addElement("�ۑ�");
		commandList.addElement("���s�}��");
		commandList.addElement("1�s�}��");
		commandList.addElement("1�s�\��t��");
		commandList.addElement("1�s�폜");
		commandList.addElement("�u���E�U�ŊJ��");
		commandList.addElement("�u���E�U�ŊJ��(Google�o�R)");
		commandList.addElement("URL���擾");
/**
		commandList.addElement("�W�����v");
		commandList.addElement("����");
		commandList.addElement("�O����");
		commandList.addElement("������");
**/
		return;
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
		currentIndex = 0;
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
		currentIndex = currentIndex - screenLine;
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
	public boolean inputNextPage()
	{
		currentIndex = currentIndex + screenLine;
		if (currentIndex >= commandList.size())
		{
			currentIndex = 0;
		}
	    return (true);
	}

	/**
	 *  �R�}���h��I��
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
		boolean ret = false;
		if (screenMode == kaniFileCanvas.SCENE_FILEOPEMODE)
		{
			ret = inputSelectedFileOpeMode(isRepeat);
		}
		else
		{
			ret = inputSelectedTextEdit(isRepeat);
		}
		return (ret);
	}
	
	/**
	 *  �R�}���h��I�� (�t�@�C�����샂�[�h)
	 * 
	 */
	private boolean inputSelectedFileOpeMode(boolean isRepeat)
	{
        boolean ret = false;
		switch (currentIndex)
		{
          case 0:
          	ret = parent.selectedOpenWebBrowser();
          	break;

          case 1:
            ret = parent.selectedEdit();
        	break;
	
          case 2:
          	ret = parent.selectedPictureView();
          	break;

          case 3:
        	ret = parent.selectedOutputFileList();
        	break;

          case 4:
        	ret = parent.selectedMkDir();
        	break;

          case 5:
        	ret = parent.selectedRename();
        	break;

          case 6:
          	ret = parent.selectedEditAttribute();
          	break;

          case 7:
            ret = parent.selectedPlayStopSound();
            break;

          case 8:
            ret = parent.selectedExtractZip(false);
            break;

          case 9:
            ret = parent.selectedExtractZip(true);
            break;

          case 10:
            ret = parent.selectedDuplicate();
            break;

          case 11:
          	ret = parent.selectedCopy();
          	break;

          case 12:
            ret = parent.selectedDelete();
            break;

          case 13:
            parent.finishApp();
            ret = true;
            break;

          default:
        	break;
		}
		return (ret);
	}

	/**
	 *  �R�}���h��I��
	 * 
	 */
	private boolean inputSelectedTextEdit(boolean isRepeat)
	{
		switch (currentIndex)
		{
          case 0:
        	// �ۑ�
        	parent.saveTextEditFile();
           	break;

          case 1:
        	// ���s�}��
          	parent.insertLine(true);
          	break;
  	
          case 2:
        	// 1�s�}��
            parent.insertLine(false);
            break;

          case 3:
        	// 1�s�\��t��
            parent.pasteLine();
          	break;

          case 4:
        	// 1�s�폜
        	parent.deleteLine();
        	break;

          case 5:
        	// �u���E�U�ŊJ��
        	parent.openUrl(false);
        	break;

          case 6:
          	// �u���E�U�ŊJ��(Google Proxy���p)
          	parent.openUrl(true);
          	break;

          case 7:
        	// URL���擾����
        	parent.getFileFromUrl();
        	break;

          default:
        	break;
		}

		// �R�}���h���[�h�𔲂���
		parent.cancelCommandInputTextEdit();
		return (false);
	}

	/**
	 *  �I���L�����Z���B�B�B�R�}���h���[�h�𔲂���
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		if (screenMode == kaniFileCanvas.SCENE_FILEOPEMODE)
		{
			parent.cancelCommandInput();
		}
		else
		{
			parent.cancelCommandInputTextEdit();
		}
		currentIndex  = 0;
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
		int fontHeight = screenFont.getHeight();
		int startTopX  = topX;
		int maxItem    = commandList.size();
		
		g.setColor(backColor);
		g.fillRect(topX, topY, width, height);

		int index = 0;
		for (int item = 0; item < maxItem; item++)
		{
            if (index == screenLine)
            {
                // 2�s�ڂɃR�}���h��\������
            	startTopX = (width + topX) / 2;
            	index = 0;
            }

            String drawLine = (String) commandList.elementAt(item);
			if (item == currentIndex)
			{
				g.setColor(cursorBackColor);
				g.fillRect(startTopX, (topY + (fontHeight * index)), ((width + topX) / 2), fontHeight);
				g.setColor(cursorForeColor);
			}
			else
			{
				g.setColor(backColor);
				g.fillRect(startTopX, (topY + (fontHeight * index)), ((width + topX) / 2), fontHeight);
				g.setColor(foreColor);
			}
			g.drawString(drawLine, startTopX + 1, (topY + (fontHeight * index)), (Graphics.LEFT | Graphics.TOP));
			index++;
		}
		g.setColor(foreColor);
		return;
	}
}
