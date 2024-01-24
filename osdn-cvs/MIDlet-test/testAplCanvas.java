import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

public class testAplCanvas  extends Canvas implements CommandListener
{
	private testAplSceneSelector parent         = null;
	private testAplFileDirList   fileListObject = null;
	private boolean             painting       = false;

	private Font screenFont = null;
	private int  keyValue   = 0;
	private int  keyNumber  = 0;

	/**
	 *  �R���X�g���N�^
	 * 
	 * @param object
	 */
	testAplCanvas(testAplSceneSelector object)
	{
		parent = object;
		// �^�C�g���̐ݒ�
		setTitle(null);

		// �t�H���g�̐ݒ�
		screenFont = Font.getDefaultFont();
		screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_SMALL);
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

		// ��ʂ̓h��Ԃ�
		g.setColor(0x00f0f0f0);
		g.fillRect(0, 0, getWidth(), getHeight());
		g.setColor(0x00000000);
		g.setFont(screenFont);

		if (keyValue != 0)
		{			
	        String str = "keyCode : " + String.valueOf(keyValue);
            g.drawString(str, 10, 10, (Graphics.LEFT | Graphics.TOP));
		}
		if (keyNumber != 0)
		{
	        String str = "gameAction : " + String.valueOf(keyNumber);
            g.drawString(str, 10, 40, (Graphics.LEFT | Graphics.TOP));
		}
		painting = false;
        return;
	}
	/**
	 *   �L�[����
	 */
	public void keyPressed(int keyCode)
	{
		if ((keyCode == 0)||(painting == true))
		{
			// �L�[���͂��Ȃ��A�܂��͕`�撆�̏ꍇ�ɂ͐܂�Ԃ�
			keyValue  = 0;
			keyNumber = 0;
			return;
		}
		keyValue =  keyCode;
		keyNumber = getGameAction(keyCode);
		
		repaint();
		return;
	}

	/**
	 *   �R�}���h�{�^�����s���̏���
	 *   
	 */
	public void commandAction(Command c, Displayable d)
	{
		int command = c.getPriority();
		commandActionMain(command);
		return;
	}

	/**
	 *   �R�}���h�̎��s���C��
	 * 
	 */
	private void commandActionMain(int command)
	{
		prepareFileList();
        return;
	}
	

	/**
	 *  �t�@�C�����X�g�I�u�W�F�N�g�̏���...
	 *
	 */
	private void prepareFileList()
	{
		//
		fileListObject = parent.getDirList();
        return;
	}
}
