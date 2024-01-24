import java.util.Date;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

/**
 * �f�B���N�g���ꗗ��ʂ̕\��
 * @author MRSa
 *
 */
public class kaniFileScreenDirList implements IkaniFileCanvasPaint
{
	private final int iconSize        = 12;
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
	private int startOffset   = 0;
	private int numberOfDatas = 0;

	private String currentDirectory = null;
	private String infoLine1  = null;
	private String infoLine2  = null;
	private String infoLine3  = null;

	Image folderIcon  = null;    // �t�H���_�������A�C�R��
	Image fileIcon    = null;    // �t�@�C���������A�C�R��
	Image selFileIcon = null;    // �I�𒆃t�@�C���������A�C�R��
	
	/**
	 * �R���X�g���N�^
	 * @param object
	 */
	kaniFileScreenDirList(kaniFileSceneSelector object)
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

		// �A�C�R���̓ǂݏo��...
		try
		{
			folderIcon  = Image.createImage("/res/folder.png");
			fileIcon    = Image.createImage("/res/file.png");
			selFileIcon = Image.createImage("/res/file_sel.png");
		}
		catch (Exception e)
		{
			//
		}		

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
     * ���[�h����������...
     */
    public int getMode()
    {
    	return (screenMode);
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
		if (screenMode == kaniFileCanvas.SCENE_DIRLIST)
		{
		    return (((height - (screenFont.getHeight() * 2)) / screenFont.getHeight()));
		}
		return (((height - (screenFont.getHeight() * 4)) / screenFont.getHeight()));
	}

	/**
	 *  ��ʐ؂�ւ����\�b�h...
	 * 
	 */
	public void changeScene()
	{
		parent.moveDirectory("");
		return;
	}
	
	/**
	 * �\���f�[�^�̍X�V����...
	 * 
	 */
	public void updateData()
	{
        String dir = currentDirectory;
        int   nofData = numberOfDatas;
		currentDirectory = parent.getCurrentDirectory();
        numberOfDatas    = parent.getNumberOfFiles();
        try
        {
    		if ((dir.compareTo(currentDirectory) != 0)||(nofData != numberOfDatas))
    		{
                // �f�B���N�g���܂��̓t�@�C�������ύX����Ă����ꍇ�ɂ́A�J�[�\����擪�Ɉړ�����
                startOffset      = 0;
                currentIndex     = 0;
    		}
        }
        catch (Exception e)
        {
            startOffset      = 0;
            currentIndex     = 0;        	
        }
		return;
	}

	/**
	 *  �J�[�\�����R��Ɉړ�����
	 * 
	 */
	public boolean inputBack()
	{
		currentIndex = currentIndex - 3;
		if (currentIndex < startOffset)
		{
			currentIndex = (startOffset + screenLine - 1);
			if (currentIndex >= numberOfDatas)
			{
				currentIndex = numberOfDatas - 1;
			}
		}
/*
		// ���̊K�w�ɏオ��
        parent.upDirectory();
*/
		return (true);
	}

	/**
	 *   �J�[�\�����R���Ɉړ�����
	 * 
	 */
	public boolean inputForward()
	{
		currentIndex = currentIndex + 3;
		if ((currentIndex >= (startOffset + screenLine))||(currentIndex >= (numberOfDatas)))
		{
			currentIndex = startOffset;
		}
/*
        // �f�B���N�g����������A���̉��K�w�ֈړ�����
		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		if (fileInfo.isDirectory() != true)
		{
			return (false);
		}
		String fileName = fileInfo.getFileName();
		if (fileName.equals(".."))
		{
			return (false);
		}
		parent.moveDirectory(fileName);
*/
		return (true);
	}

	/**
	 *  �J�[�\����1��Ɉړ�����
	 * 
	 */
	public boolean inputPreviousItem()
	{
		currentIndex--;
		if (currentIndex < startOffset)
		{
			currentIndex = (startOffset + screenLine - 1);
			if (currentIndex >= numberOfDatas)
			{
				currentIndex = numberOfDatas - 1;
			}
		}
		return (true);
	}

	/**
	 *   �J�[�\����1���Ɉړ�����
	 * 
	 */
	public boolean inputNextItem()
	{
		currentIndex++;
		if ((currentIndex >= (startOffset + screenLine))||(currentIndex >= (numberOfDatas)))
		{
			currentIndex = startOffset;
		}
		return (true);
	}

	/**
	 *  �O�y�[�W�̕\��
	 * 
	 */
	public boolean inputPreviousPage()
	{
		if (startOffset > 0)
		{
			startOffset  = startOffset - screenLine + 1;
			if (startOffset < (screenLine - 1))
			{
				startOffset = 0;
			}
			currentIndex = startOffset;
			return (true);
		}		
		return (true);
	}

	/**
	 *  ���y�[�W�̕\��
	 * 
	 */
	public boolean inputNextPage()
	{
		if ((startOffset + screenLine) < numberOfDatas)
		{
			startOffset = startOffset + screenLine - 1;
			currentIndex = startOffset;
			return (true);
		}
		return (true);
	}

	/**
	 *  �I�����ꂽ�ꍇ... (�������͑S�I��)
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
        // ���������ꂽ�Ƃ�...
		if (isRepeat == true)
		{
	        // �S�I�������{����
			parent.selectAllFiles(true);
			return (true);
		}
		
		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		String fileName = fileInfo.getFileName();
		if (fileInfo.isDirectory() == true)
		{
			// �f�B���N�g����I�������ꍇ�ɂ́A�f�B���N�g���ړ����s��
			if (fileName.equals(".."))
			{
				parent.upDirectory();
			}
			else
			{
				parent.moveDirectory(fileName);
			}
			return (true);
		}

		// �t�@�C�����I�����ꂽ�Ƃ��̏���...
		fileInfo.toggleSelected();
		parent.selectedFile(fileName, currentIndex, fileInfo.isSelected());
		return (true);
	}

	/**
	 *  �I���L�����Z���B�B�B
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		if (getMode() == kaniFileCanvas.SCENE_DIRLIST)
		{
			// �f�B���N�g���I�����[�h�������ꍇ...�I�����L�����Z������
			parent.cancelCommandDirInput();
			return (false);
		}

		// ���������ꂽ�Ƃ�
		if (isRepeat == true)
		{
	        // �S�Ă̑I������������
			parent.selectAllFiles(false);
			return (true);
		}

		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		String fileName = fileInfo.getFileName();
		if (fileInfo.isDirectory() == true)
		{
			return (true);
		}

		// �t�@�C�����I���L�����Z�����ꂽ�Ƃ��̏���...
		fileInfo.setSelected(false);
		parent.selectedFile(fileName, currentIndex, fileInfo.isSelected());
		return (true);
	}

	/**
	 *  GAME D�������ꂽ�Ƃ��̏���
	 * 
	 */
	public boolean inputOther(boolean isRepeat)
	{
		
		// �t�@�C����I������B�B�B
		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		if (fileInfo.isDirectory() == true)
		{
			// �f�B���N�g����I�������ꍇ�ɂ́A�������Ȃ�
			return (true);
		}
		String fileName = (fileInfo.getFileName()).toLowerCase();
		parent.selectedFile(fileName, currentIndex, true);

		if (isRepeat == true)
        {
            // �L�[���s�[�g���ɂ́A�u�u���E�U�ŊJ���v��I���������Ƃɂ���
            parent.selectedOpenWebBrowser();
            return (true);
        }

        if ((fileName.indexOf(".htm") > 0)||(fileName.indexOf(".html") > 0)
             ||(fileName.indexOf(".txt") > 0)||(fileName.indexOf(".log") > 0)
             ||(fileName.indexOf(".idx") > 0)||(fileName.indexOf(".csv") > 0)
             ||(fileName.indexOf(".dat") > 0)||(fileName.indexOf(".jad") > 0)
        	 ||(fileName.indexOf(".ini") > 0)||(fileName.indexOf(".xml") > 0))
        {
            // �e�L�X�g�ҏW���[�h�ŊJ��
        	parent.selectedEdit();
        	return (true);
        }
        // �摜�\�����[�h�ŊJ��...
        parent.selectedPictureView();
		return (true);
	}

	/**
	 *  �擪�ɃJ�[�\�����ړ�������
	 * 
	 */
	public boolean inputTop(boolean isRepeat)
	{
//		parent.updateFileDetail(currentIndex);
		startOffset = 0;
		currentIndex = startOffset;
		return (true);
	}

	/**
	 *  �����ɃJ�[�\�����ړ�������
	 * 
	 */
	public boolean inputBottom(boolean isRepeat)
	{
//      parent.updateFileDetail(currentIndex);
		startOffset = numberOfDatas - (screenLine);
		if (startOffset < 0)
		{
            startOffset = 0;
		}
		currentIndex = numberOfDatas - 1;
		return (true);
	}

	/**
	 * 
	 */
	public boolean inputDummy()
	{
		if (getMode() == kaniFileCanvas.SCENE_DIRLIST)
		{
			// �f�B���N�g���I�����[�h�������ꍇ...
			parent.selectedCopyDirectory();
			return (false);
		}
		
		kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(currentIndex);
		String fileName = fileInfo.getFileName();
		if (parent.selectedFile(fileName, currentIndex) == true)
		{
			fileInfo.setSelected(true);
		}
		return (false);
	}

	/**
	 * 
	 * 
	 */
	public boolean inputNumber(int number)
	{
		if (number == 0)
		{
            // �����L�[�[���ł́A�t�@�C������\������
			parent.updateFileDetail(currentIndex);
			return (false);
		}
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
	        int infoHeight = 0;
			if (screenMode != kaniFileCanvas.SCENE_DIRLIST)
			{
				infoHeight = screenHeight - screenFont.getHeight() * 3;
				drawMainPanel(g, screenTopX, screenTopY + screenFont.getHeight(), screenWidth, (infoHeight - (screenFont.getHeight())));

                // ���̈�̕`��
			    g.setColor(0x00d8ffd8);
			    g.fillRect(screenTopX, infoHeight, screenWidth, screenFont.getHeight() * 3);
			    drawInformationArea(g, screenTopX, screenTopY + infoHeight);
			}
			else
			{
				infoHeight = screenHeight - screenFont.getHeight() * 1;
				drawMainPanel(g, screenTopX, screenTopY + screenFont.getHeight(), screenWidth, (infoHeight - (screenFont.getHeight())));

				// ���̈�̕`��
			    g.setColor(0x00d8ffd8);
			    g.fillRect(screenTopX, screenTopY + infoHeight, screenWidth, screenFont.getHeight());
			    drawSingleInformationArea(g, screenTopX, screenTopY + infoHeight);
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
		if (currentDirectory != null)
		{
			g.setColor(0);
            int width = screenFont.stringWidth(currentDirectory);
            if (width > screenWidth)
            {
            	g.drawString("...", screenTopX, screenTopY, (Graphics.LEFT | Graphics.TOP));            	
				int offset = 0;
				int length = currentDirectory.length();
				while (screenFont.substringWidth(currentDirectory, offset, (length - offset)) > (screenWidth - 8))
				{
				    offset++;
				}
				g.drawSubstring(currentDirectory, offset, (length - offset), 8, screenTopY, (Graphics.LEFT | Graphics.TOP));
            }
            else
            {
            	g.drawString(currentDirectory, screenTopX, screenTopY, (Graphics.LEFT | Graphics.TOP));
            }
		}
		return;
	}

	/**
	 * �^�C�g���̈�ɕ`�悷��
	 * @param g
	 */
	private void drawInformationArea(Graphics g, int topX, int topY)
	{
		g.setColor(0);
		if (infoLine1 != null)
		{
			g.drawString(infoLine1, topX, topY, (Graphics.LEFT | Graphics.TOP));
		}
		if (infoLine2 != null)
		{
			g.drawString(infoLine2, topX, topY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));
		}
		if (infoLine3 != null)
		{
			g.drawString(infoLine3, topX, topY + (screenFont.getHeight() * 2), (Graphics.LEFT | Graphics.TOP));
		}
		return;
	}

	/**
	 * �^�C�g���̈�ɕ`�悷��
	 * @param g
	 */
	private void drawSingleInformationArea(Graphics g, int topX, int topY)
	{
		g.setColor(0);
		if (infoLine3 != null)
		{
			g.drawString(infoLine3, topX, topY, (Graphics.LEFT | Graphics.TOP));
		}
		return;
	}

	/**
	 * ���̈�ɏ���ݒ肷��
	 * 
	 */
	public void setInformation(String message)
	{
		infoLine3 = message;
		return;
	}
	
	/**
	 * ���̈�̏����X�V����
	 * 
	 */
	public void updateInformation(String message, int mode)
	{
		infoLine3 = message;
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

		for (int loop = 0; loop < screenLine; loop++)
		{
			int index = loop + startOffset;
			g.setColor(foreColor);
			if (index >= numberOfDatas)
			{
				// �\�����ׂ��f�[�^�ʂ𒴂���...
				break;
			}
			long fileSize     = -1;
			long lastModified = -1;
			kaniFileDirList.fileInformation fileInfo = parent.getFileInfo(index);
			Image icon = decideIcon(fileInfo);
			if (icon != null)
			{
				g.drawImage(icon, topX, (topY + (fontHeight * loop)), (Graphics.LEFT | Graphics.TOP));
			}
			fileSize = fileInfo.getFileSize();
			lastModified = fileInfo.getDateTime();
			String drawLine = fileInfo.getFileName();
			if (index == currentIndex)
			{
				g.setColor(cursorBackColor);
				g.fillRect(topX + iconSize, (topY + (fontHeight * loop)), width - iconSize, fontHeight);
				g.setColor(cursorForeColor);

				infoLine1 = "";
				if (lastModified > 0)
				{
					Date dateTime = new Date(lastModified);
					infoLine1 = dateTime.toString();
				}
				infoLine2 = "";
				if (fileSize >= 0)
				{
					infoLine2 = fileSize + " bytes";
				}

				if (fileInfo.isWritable() == false)
				{
					infoLine2 = infoLine2 + " [READ ONLY]";
				}

				if (fileInfo.isHidden() == true)
				{
					infoLine2 = infoLine2 + " [HIDDEN]";
				}
			}
			g.drawString(drawLine, topX + iconSize + 1, (topY + (fontHeight * loop)), (Graphics.LEFT | Graphics.TOP));			
		}
		if (startOffset > 0)
		{
			g.setColor(0x0030b030);
			g.drawString("��", topX + width - iconSize, (topY + 1), (Graphics.LEFT | Graphics.TOP));			
		}
		if ((startOffset + screenLine) < numberOfDatas)
		{
			g.setColor(0x0030b030);
			g.drawString("��", topX + width - iconSize, (topY + height - fontHeight) -1, (Graphics.LEFT | Graphics.TOP));			
		}

		return;
	}	
	
	/**
	 * �\������A�C�R������肷��
	 * @param info
	 * @return
	 */
	private Image decideIcon(kaniFileDirList.fileInformation info)
	{
		if (info.isDirectory() == true)
		{
			return (folderIcon);
		}
		if (info.isSelected() == true)
		{
			return (selFileIcon);
		}
		return (fileIcon);
	}
}
