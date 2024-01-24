import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import java.util.Hashtable;

/**
 * �L�����o�X�N���X�̕\���i���C���j����...
 * 
 * @author MRSa
 *
 */
public class kaniFileCanvas  extends Canvas implements CommandListener
{
	public static final int   SCENE_FILELIST         = 0;
    public static final int   SCENE_DIRLIST          = 1;
    public static final int   SCENE_FILEOPEMODE      = 2;
    public static final int   SCENE_CONFIRMATION     = 3;
    public static final int   SCENE_TEXTEDIT         = 4;
    public static final int   SCENE_TEXTEDIT_COMMAND = 5;
    public static final int   SCENE_PICTUREVIEW      = 6;

    private final String        SCENESTR_FILELIST          = "F";
	private final String        SCENESTR_DIRLIST           = "D";
	private final String        SCENESTR_FILEOPEMODE       = "O";
	private final String        SCENESTR_CONFIRMATION      = "C";
	private final String        SCENESTR_TEXTEDIT          = "T";
	private final String        SCENESTR_TEXTEDIT_COMMAND  = "E";
	private final String        SCENESTR_PICTUREVIEW       = "P";

	private final int           COMMAND_EXIT      = 0;
	private final int           COMMAND_EXECUTE   = 1;
	
	private Font                 screenFont        = null;
	private kaniFileKeyInput     keyIn             = null;

	private Hashtable            drawObjectMap     = null;
	private IkaniFileCanvasPaint currentDrawObject = null;
	private kaniFileSceneSelector  parent          = null;

	private boolean             working           = false;
	private boolean             painting          = false;
	private boolean             isBusyMode        = false;
	private String               busyMessage       = null;

	private Command opeCmd  = new Command("���s",  Command.ITEM, COMMAND_EXECUTE);
//	private Command exitCmd = new Command("�I��",  Command.EXIT, COMMAND_EXIT);

	/**
	 * �R���X�g���N�^
	 * @param parent
	 */
	kaniFileCanvas(kaniFileSceneSelector object)
	{
		// �^�C�g���̐ݒ�
		setTitle(null);

		// �t�H���g�̐ݒ�
		screenFont = Font.getDefaultFont();
		screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_SMALL);

		// �L�[���̓N���X�̐ݒ�
		keyIn = new kaniFileKeyInput();
	
		// ��ʕ`��I�u�W�F�N�g�̏���...
		drawObjectMap = new Hashtable();
		prepareScreenObjects(object);

		parent = object;
		
		// �R�}���h�̒ǉ�
		this.addCommand(opeCmd);
//		this.addCommand(exitCmd);
		this.setCommandListener(this);	
	}

	/**
	 * ��ʕ`��I�u�W�F�N�g�̐���...
	 * 
	 * @param object
	 */
	private void prepareScreenObjects(kaniFileSceneSelector object)
	{
		/// �t�@�C��/�f�B���N�g���\�����[�h�̏���...
		IkaniFileCanvasPaint screenObject = new kaniFileScreenDirList(object);
		screenObject.prepare(screenFont, SCENE_FILELIST);
		screenObject.setRegion(0, 0, getWidth(), getHeight());
		drawObjectMap.put(SCENESTR_FILELIST, screenObject);	

		// �f�B���N�g���\�����[�h�̏���...
		screenObject = new kaniFileScreenDirList(object);
		screenObject.prepare(screenFont, SCENE_DIRLIST);
		screenObject.setRegion(15, 15, getWidth() - 30, getHeight() - 30);
		drawObjectMap.put(SCENESTR_DIRLIST, screenObject);	

		// �R�}���h�I�����[�h�̏���...
		screenObject = new kaniFileScreenCommandList(object);
		screenObject.prepare(screenFont, SCENE_FILEOPEMODE);
		screenObject.setRegion(2, getHeight() - (screenFont.getHeight() * (kaniFileScreenCommandList.selectionList + 1)) - 2, getWidth() - 4, (screenFont.getHeight() * (kaniFileScreenCommandList.selectionList + 1)));
		drawObjectMap.put(SCENESTR_FILEOPEMODE, screenObject);

		// �R�}���h�I�����[�h(�e�L�X�g�ҏW��)�̏���...
		screenObject = new kaniFileScreenCommandList(object);
		screenObject.prepare(screenFont, SCENE_TEXTEDIT_COMMAND);
		screenObject.setRegion(2, getHeight() - (screenFont.getHeight() * (kaniFileScreenCommandList.selectionList + 1)) - 2, getWidth() - 4, (screenFont.getHeight() * (kaniFileScreenCommandList.selectionList + 1)));
		drawObjectMap.put(SCENESTR_TEXTEDIT_COMMAND, screenObject);

		// �m�F��ʂ̏���...
		screenObject = new kaniFileScreenConfirmation(object);
		screenObject.prepare(screenFont, SCENE_CONFIRMATION);
		screenObject.setRegion(2, (getHeight() / 2) - (screenFont.getHeight() * 2), getWidth() - 4, (screenFont.getHeight() * 3));
		drawObjectMap.put(SCENESTR_CONFIRMATION, screenObject);

		// �e�L�X�g�ҏW��ʂ̏���...
		screenObject = new kaniFileScreenTextEdit(object);
		screenObject.prepare(screenFont, SCENE_TEXTEDIT);
		screenObject.setRegion(0, 0, getWidth(), getHeight());
		drawObjectMap.put(SCENESTR_TEXTEDIT, screenObject);

		// �O���t�B�b�N�\����ʂ̏���...
		screenObject = new kaniFileScreenPictureScreen(object);
		screenObject.prepare(screenFont, SCENE_PICTUREVIEW);
		screenObject.setRegion(10, 10, getWidth() - 20, getHeight() - 20);
		drawObjectMap.put(SCENESTR_PICTUREVIEW, screenObject);

		return;
	}

	/**
	 *  �`��N���X�̏���...
	 *
	 */
	public void prepare()
	{
		return;
	}

	/**
	 *   ��ʂ̏���...
	 */
	public void prepareScreen()
	{
		//
		return;
	}

	/**
	 * ��ʍĕ`��w��...
	 *
	 */
	public void redraw()
	{
		repaint();
	}
	
	/**
	 * ��ʂ̕`������{����
	 * @param g �O���t�B�b�N�L�����o�X
	 */
	public void paint(Graphics g)
	{
		// ��ʕ`�撆�̂Ƃ��ɂ͐܂�Ԃ�
		if (painting == true)
		{
			return;
		}
		painting = true;
		
		if ((isBusyMode == false)&&(busyMessage != null))
		{
			// BUSY ���[�h����������Ƃ�������...
			hideBusyWindow(g);			
		}
		
        /////////////////////////////////////

		// �`��G���W�����w�肳��Ă��邩�H
		if (currentDrawObject != null)
		{
			g.setFont(screenFont);
/**
			// ��ʑS�̂̃N���A...�́A�`��G���W����...
			g.setColor(0x00ffffff);
			g.fillRect(currentDrawObject.getTopX(), currentDrawObject.getTopY(), currentDrawObject.getWidth(), currentDrawObject.getHeight());
**/

			// �E�B���h�E����ʑS�̂ł͂Ȃ��Ƃ��́A�g��\������
			if (currentDrawObject.getTopY() > 0)
			{
				g.setColor(0);
				g.drawRect(currentDrawObject.getTopX() - 1, currentDrawObject.getTopY() - 1, currentDrawObject.getWidth() + 1, currentDrawObject.getHeight() + 1);
			}
			
			// ��ʂ̕`������{...
			currentDrawObject.paint(g);
		}

        //////////////////////////////////////
		
		if ((isBusyMode == true)&&(busyMessage != null))
		{
			// BUSY ���[�h������...BUSY���b�Z�[�W��\������
			showBusyWindow(g);			
		}

		// �I������...
		painting = false;
		return;
	}

	/**
	 * �������̕\��...
	 * 
	 */
	private void showBusyWindow(Graphics g)
	{
		int height = screenFont.getHeight() + 4;

		// ��ʂ̓h��Ԃ�
		g.setColor(0x00ffffc0);
		g.fillRect(0, (getHeight() - height - 2), getWidth(), (getHeight() - 2));
		g.setColor(0, 0, 0);
		g.setFont(screenFont);
		
		// �R�}���h�E�B���h�E�^�C�g���̕\��
		g.drawString(busyMessage, 5, (getHeight() - height), (Graphics.LEFT | Graphics.TOP));
		return;		
	}

	/**
	 * �������̕\���폜...
	 * 
	 */
	private void hideBusyWindow(Graphics g)
	{
		int height = screenFont.getHeight() + 4;
		
		// ��ʂ̓h��Ԃ�
		g.setColor(0x00ffffff);
		g.fillRect(0, (getHeight() - height - 2), getWidth(), (getHeight() - 2));

		busyMessage = null;

		return;		
	}
	
	/**
	 * �\����؂�ւ���...
	 * @param scene
	 */
	public void changeScene(int scene, String message)
	{
		try
		{
			String sceneString = convertScene(scene);
			IkaniFileCanvasPaint object = (IkaniFileCanvasPaint) drawObjectMap.get(sceneString);
			if (object != null)
			{
				currentDrawObject = object;
				currentDrawObject.setInformation(message);
				currentDrawObject.changeScene();
				keyIn.setScreenCanvas(currentDrawObject);
				repaint();
			}
		}
		catch (Exception e)
		{
			// �������Ȃ�...
		}
		return;
	}

	/**
	 * ���b�Z�[�W��\������..
	 * @param message
	 */
	public void setInformation(String message)
	{
		if (currentDrawObject != null)
		{
			currentDrawObject.setInformation(message);
		}
		return;
	}

	/**
	 * ���̈�̏����X�V����
	 * 
	 */
	public void updateInformation(String message, int mode)
	{
		if (currentDrawObject != null)
		{
			currentDrawObject.updateInformation(message, mode);
			repaint();
		}
		return;
	}
	
	public void startUpdate(String message)
	{
		isBusyMode  = true;
		busyMessage = message;
		repaint();
		return;
	}
	
	/**
	 *  ��ʍX�V...
	 *
	 */
	public void updateData()
	{
		if (working == true)
		{
			return;
		}
		if (currentDrawObject != null)
		{
			working = true;
			currentDrawObject.updateData();
			working = false;
		}
		isBusyMode = false;
		repaint();
		return;
	}

	/**
	 * �R�}���h���s���̏���
	 */
	public void commandAction(Command c, Displayable d) 
	{
		int command = c.getPriority();
		
		commandActionMain(command);
		return;
	}

	/**
	 * �\���V�[������肷��B�B�B
	 * @param scene
	 * @return
	 */
	private String convertScene(int scene)
	{
		if (scene == SCENE_DIRLIST)
		{
			return (SCENESTR_DIRLIST);
		}
		if (scene == SCENE_FILELIST)
		{
			return (SCENESTR_FILELIST);
		}
		if (scene == SCENE_FILEOPEMODE)
		{
			return (SCENESTR_FILEOPEMODE);
		}
		if (scene == SCENE_CONFIRMATION)
		{
			return (SCENESTR_CONFIRMATION);
		}
		if (scene == SCENE_TEXTEDIT)
		{
			return (SCENESTR_TEXTEDIT);
		}
		if (scene == SCENE_TEXTEDIT_COMMAND)
		{
			return (SCENESTR_TEXTEDIT_COMMAND);
		}
		if (scene == SCENE_PICTUREVIEW)
		{
			return (SCENESTR_PICTUREVIEW);
		}
		return (null);
	}
	
	/**
	 *   �L�[����
	 */
	public void keyPressed(int keyCode)
	{
		boolean repaint = false;
		if ((keyCode == 0)||(painting == true)||(working == true))
		{
			// �L�[���͂��Ȃ��A�܂��͕`�撆�̏ꍇ�ɂ͐܂�Ԃ�
			return;
		}

		// �����L�[�̓��͂��`�F�b�N����B�B�B
		int number = keyIn.numberInputMode(keyCode);
		if (number == -1)
		{
			// �R�}���h�����s�����ꍇ...
			repaint();
			return;
		}
		if (number >= 0)
		{
            // ���������͂��ꂽ��...
			repaint();
			return;
		}

		// �J�[�\���L�[�̓��́B�B�B
		switch (getGameAction(keyCode))
		{
          case Canvas.LEFT:
        	repaint = keyIn.inputLeft(false);
			break;
			  
          case Canvas.RIGHT:
        	repaint = keyIn.inputRight(false);
			break;

		  case Canvas.DOWN:
			repaint = keyIn.inputDown(false);
	        break;

		  case Canvas.UP:
			repaint = keyIn.inputUp(false);
            break;

		  case Canvas.FIRE:
			repaint = keyIn.inputFire(false);
			break;

		  case Canvas.GAME_A:
			// GAME-A�L�[
			repaint = keyIn.inputGameA(false);
			break;

		  case Canvas.GAME_B:
			// GAME-B�L�[
			repaint = keyIn.inputGameB(false);
			break;

		  case Canvas.GAME_C:
			// GAME-C�L�[
			repaint = keyIn.inputGameC(false);
			break;

		  case Canvas.GAME_D:
			// GAME-D�L�[
			repaint = keyIn.inputGameD(false);
			break;

          default:
        	repaint = false;
            break;
		}
		if (repaint == true)
		{
			repaint();
		}
		return;
	}

	/*
	 *   �L�[���s�[�g���Ă���Ƃ�...
	 * 
	 * 
	 */
	public void keyRepeated(int keyCode)
	{
		boolean repaint = false;
		if ((keyCode == 0)||(painting == true)||(working == true))
		{
			// �L�[���͂��Ȃ��A�܂��͕`�撆�̏ꍇ�ɂ͐܂�Ԃ�
			return;
		}

		// �J�[�\���L�[�̓��́B�B�B
		switch (getGameAction(keyCode))
		{
          case Canvas.LEFT:
        	repaint = keyIn.inputLeft(true);
			break;
			  
          case Canvas.RIGHT:
        	repaint = keyIn.inputRight(true);
			break;

		  case Canvas.DOWN:
			repaint = keyIn.inputDown(true);
	        break;

		  case Canvas.UP:
			repaint = keyIn.inputUp(true);
            break;

		  case Canvas.FIRE:
			repaint = keyIn.inputFire(true);
			break;

		  case Canvas.GAME_A:
			// GAME-A�L�[
			repaint = keyIn.inputGameA(true);
			break;

		  case Canvas.GAME_B:
			// GAME-B�L�[
			repaint = keyIn.inputGameB(true);
			break;

		  case Canvas.GAME_C:
			// GAME-C�L�[
			repaint = keyIn.inputGameC(true);
			break;

		  case Canvas.GAME_D:
			// GAME-D�L�[
			repaint = keyIn.inputGameD(true);
			break;

          default:
        	repaint = false;
            break;
		}
		if (repaint == true)
		{
			repaint();
		}
        return;
	}

	/**
	 *  �����I������
	 * 
	 */
	private void selectOperation()
	{
		if (currentDrawObject == null)
		{
			return;
		}
		
		if (currentDrawObject.getMode() == SCENE_FILEOPEMODE)
		{
            // �R�}���h�\�����[�h�̂Ƃ��ɂ́A��ʂ�؂�ւ��Ȃ�...
			return;
		}
		
		if (currentDrawObject.getMode() == SCENE_DIRLIST)
		{
			// �f�B���N�g���I�����[�h�Ńf�B���N�g�����I�����ꂽ�ꍇ...
			keyIn.inputDummy();
			return;
		}
		if (currentDrawObject.getMode() == SCENE_TEXTEDIT)
		{
			// �e�L�X�g�ҏW���[�h�Łu���s�v�{�^���������ꂽ�Ƃ�...
			keyIn.inputDummy();
			repaint();
			return;
		}
		
		if (currentDrawObject.getMode() == SCENE_PICTUREVIEW)
		{
            // �摜�\�����[�h�Łu���s�v�{�^���������ꂽ�Ƃ�...
			// (��ʂ�؂�ւ��Ȃ�)
			keyIn.inputDummy();
			return;
		}

		// �_�~�[���͂�����B�B�B(���I���Ȃ�A�J�[�\���ʒu�̃A�C�e����I������)
		keyIn.inputDummy();
		
		// ��ʂ�؂�ւ���...
		changeScene(SCENE_FILEOPEMODE, "<<<<< ����I�� >>>>>");
        repaint();
		return;
	}
	
	/**
	 * �R�}���h���s���̏����B�B�B
	 * @param command
	 */
	private void commandActionMain(int command)
	{
		if (command == COMMAND_EXECUTE)
		{
			// �����I������...
			selectOperation();
			return;
		}
		else if (command == COMMAND_EXIT)
		{
			// �A�v���P�[�V�������I��������
			parent.finishApp();
		}
		return;
	}
}
