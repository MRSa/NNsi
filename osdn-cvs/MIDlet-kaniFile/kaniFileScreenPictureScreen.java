import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

/**
 * �摜�\���p�̉��
 * @author MRSa
 *
 */
public class kaniFileScreenPictureScreen implements IkaniFileCanvasPaint
{
	private final int backColor       = 0x00ffffff;
	private final int foreColor       = 0x00000000;
//	private final int cursorBackColor = 0x000000f8;
//	private final int cursorForeColor = 0x00ffffff;

	private kaniFileSceneSelector parent = null;
	private Image imageObject = null;
	private Font screenFont = null;   // �\���Ɏg�p����t�H���g
	private int screenMode   = -1;    // �\�����[�h
	private int screenTopX   = 0;     // �`��̈�̍��[(X)
	private int screenTopY   = 0;     // �`��̈�̍��[(Y)
	private int screenWidth  = 160;   // ��ʕ�
	private int screenHeight = 160;   // ��ʍ���
	private int screenLine   = 0;     // �f�[�^�\���\���C����
	private int offsetX      = 0;     // �\���J�n�ʒu�i�w���W�j
	private int offsetY      = 0;     // �\���J�n�ʒu�i�x���W�j

	private boolean busyMode = false;
	private boolean useThumbnail = true;
	private String infoLine   = null;

	/**
	 * �R���X�g���N�^
	 * @param object
	 */
	kaniFileScreenPictureScreen(kaniFileSceneSelector object)
	{
		//
		parent = object;
	}

	/**
	 * ��ʂ̏�������...
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
        String message = infoLine;
        int pos = infoLine.indexOf(":");
        if (pos >= 0)
        {
        	message = infoLine.substring(pos + 1);
        }
        infoLine = " �摜�W�J��... " + message;
		imageObject = null;
		System.gc();
		try
		{
			Image img = parent.getImage(useThumbnail);
			if (img == null)
			{
				// �摜�ǂݍ��ݎ��s...
                infoLine = "<<< OUT OF MEMORY? >>>";
                System.gc();
				return;
			}
//	        String appendMessage = "(" + img.getWidth() + "," + img.getHeight() + ")";
			int width  = screenWidth;
			int height = screenHeight - (screenFont.getHeight() * 1);
			if (img.getHeight() < (height + offsetY))
			{
				offsetY = img.getHeight() - height - 1;
				if (offsetY < 0)
				{
					offsetY = 0;
				}
				if ((offsetY + height) > img.getHeight())
				{
                    height = img.getHeight() - offsetY;
				}
			}
			if (img.getWidth() < (width + offsetX))
			{
				offsetX = img.getWidth() - width - 1;
				if (offsetX < 0)
				{
					offsetX = 0;
				}
				if ((offsetX + width) > img.getWidth())
				{
                    width = img.getWidth() - offsetX;
				}
			}
            imageObject = Image.createImage(img, offsetX, offsetY, width, height, Sprite.TRANS_NONE);
            infoLine = img.getWidth() + "x" + img.getHeight() + "(" + offsetX + "," + offsetY + ")";
            if (useThumbnail == true)
            {
                infoLine = infoLine + "T";
            }
            else
            {
                infoLine = infoLine + "N";
            }
            img = null;
            infoLine = infoLine + ":" + message;
		}
		catch (OutOfMemoryError e)
		{
            imageObject = null;
            infoLine = " ERROR:: Out of Memory...";
		}
		catch (Exception e)
		{
            imageObject = null;
            infoLine = " ERROR >> " + e.getMessage();
		}
        System.gc();
		return;
	}
	
	/**
	 * �\���f�[�^�̍X�V����...
	 * 
	 */
	public void updateData()
	{
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
	 *  �����
	 * 
	 */
	public boolean inputPreviousItem()
	{
        offsetY = offsetY - 120;
        if (offsetY < 0)
        {
        	offsetY = 0;
        }
        changeScene();
		return (true);
	}

	/**
	 *  ������
	 * 
	 */
	public boolean inputNextItem()
	{
        offsetY = offsetY + 120;
        changeScene();
		return (true);
	}

	/**
	 *  ������
	 * 
	 */
	public boolean inputPreviousPage()
	{
        offsetX = offsetX - 120;
        if (offsetX < 0)
        {
        	offsetX = 0;
        }
        changeScene();
		return (true);
	}

	/**
	 *  �E����
	 * 
	 */
	public boolean inputNextPage()
	{
        offsetX = offsetX + 120;
        changeScene();
		return (true);
	}

	/**
	 *  �R�}���h��I���B�B�B��ʂ𔲂���
	 * 
	 */
	public boolean inputSelected(boolean isRepeat)
	{
		exitScreen();
		return (false);
	}

	/**
	 *  �I���L�����Z���B�B�B��ʂ𔲂���
	 * 
	 */
	public boolean inputCancel(boolean isRepeat)
	{
		exitScreen();
		return (false);
	}

	/**
	 *  GAME D�{�^���������ꂽ�Ƃ��B�B�B��ʂ𔲂���
	 * 
	 */
	public boolean inputOther(boolean isRepeat)
	{
		exitScreen();
		return (false);
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
        if (number == 0)
        {
        	if (useThumbnail == true)
        	{
        		useThumbnail = false;
        	}
        	else
        	{
        		useThumbnail = true;
        	}

            // �摜�t�@�C���ēǂݍ���...
			Thread thread = new Thread()
			{
				public void run()
				{
                    changeScene();
				}
			};
			thread.run();
        }
		return (true);
	}

	/**
	 *  ���s�{�^���������ꂽ�Ƃ��B�B�B��ʂ𔲂���
	 */
	public boolean inputDummy()
	{
		exitScreen();
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
	 * ��ʂ𔲂���...
	 *
	 */
	private void exitScreen()
	{
		imageObject = null;
		parent.cancelPictureView();
		offsetX = 0;
		offsetY = 0;
		System.gc();
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
			g.drawString(infoLine, screenTopX + 1, screenTopY, (Graphics.LEFT | Graphics.TOP));
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
        // �t�H���g�T�C�Y
		int fontHeight = screenFont.getHeight();

		// �g��\������
		g.setColor(backColor);
		g.fillRect(topX, topY, width, height);

		// �C���[�W�̕\�����W�����߂�
		int x = topX;
		if (imageObject.getWidth() < width)
		{
			x = topX + ((width  - imageObject.getWidth())  / 2);
		}

		int y = topY;
		if (imageObject.getHeight() < height)
		{
			y = y + ((height - imageObject.getHeight()) / 2);
		}

		// �C���[�W��`�悷��
        g.setColor(foreColor);
		if (imageObject != null)
		{
		    g.drawImage(imageObject, x, y, (Graphics.LEFT | Graphics.TOP));
		}
		else
		{
            // �C���[�W���Ȃ��̂� ????? ��\������
            g.drawString("?????", (topX + screenFont.stringWidth("��")), (topY + fontHeight * screenLine / 2), (Graphics.LEFT | Graphics.TOP));			
		}
		return;
	}
}
