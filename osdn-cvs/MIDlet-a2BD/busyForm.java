import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * busyForm
 * 
 * @author MRSa
 *
 */
public class busyForm  extends Canvas  implements CommandListener
{
	private a2BDmain  parent     = null;
	private boolean painting     = false;
	private String   busyTitle    = "";
	private String   busyMessage  = "";
	private String   busyMessage2 = "";
	private String   busyMessage3 = "";
	private Font     screenFont   = null;
	private int     foreColor    = 0;
	private int     backColor    = 0x00ffffff;
	
	private boolean enableExit  = false;
	private Command  exitCmd      = new Command("�I��", Command.EXIT, -1);

	/**
	 * �R���X�g���N�^
	 * @param arg0
	 */
	public busyForm(a2BDmain object, String arg0)
    {
		// �^�C�g��
		setTitle(arg0);

		// ��ʂ̃R�}���h��o�^����
		this.addCommand(exitCmd);
		this.setCommandListener(this);

		// �e�N���X���L������
        parent = object;    	
    }
	
	/**
	 *   �t�H���g�Ɖ�ʃT�C�Y����������
	 * 
	 * 
	 */
	public void prepareScreen()
	{
		screenFont = Font.getDefaultFont();
//		screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_SMALL);
		screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_MEDIUM);
	}

	/**
	 * �`��F��ݒ肷��
	 * @param color
	 */
	public void setForeColor(int color)
	{
		foreColor = color;
	}

	/**
	 * �w�i�F��ݒ肷��
	 * @param color
	 */
	public void setBackColor(int color)
	{
		backColor = color;
	}
	
	/**
	 * BUSY���b�Z�[�W��ݒ肷��
	 * @param title
	 * @param message
	 * @param message2
	 */
	public void SetBusyMessage(String title, String message, String message2, String message3, boolean isExit)
	{
		enableExit   = isExit;
		busyTitle    = title;
		busyMessage  = message;
		busyMessage2 = message2;
		busyMessage3 = message3;
		repaint();
		return;
	}
	
	public void HideBusyMessage()
	{
		busyTitle    = "";
		busyMessage  = "";
		busyMessage2 = "";
		repaint();
		return;		
	}
	
	/*
	 *   ��ʕ`��...
	 * 
	 * 
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
		g.setColor(backColor);
		g.fillRect(0, 0, getWidth(), getHeight());
		g.setColor(foreColor);
		g.setFont(screenFont);

		// �^�C�g���̐ݒ�
		if (busyTitle != null)
		{
			setTitle(busyTitle);
		}

		int line = 0;
		
		// ���b�Z�[�W�P�̔��f
        if (busyMessage != null)
        {
  		    g.setColor(foreColor);
            line = line + drawString(g, line, busyMessage);
        }
        
        // ���b�Z�[�W�Q�̔��f
        if (busyMessage2 != null)
        {
  		    g.setColor(foreColor);
            line = line + drawString(g, line, busyMessage2);
        }
        
        // ���b�Z�[�W�R�̔��f
        if (busyMessage3 != null)
        {
  		    g.setColor(foreColor);
            line = line + drawString(g, line, busyMessage3);
       }

        painting = false;
		return;
	}

	/**
	 * �������o�͂���
	 * @param g
	 * @param startLine
	 * @param msg
	 * @return
	 */
	private int drawString(Graphics g, int startLine, String msg)
	{
		// �\���\�s��ݒ肷��
		int line    = 0;
		int y       = (startLine * (screenFont.getHeight())) + 4;
		int x       = 0;
		int limitX  = getWidth() - 4;
		for (int msgIndex = 0; msgIndex < msg.length(); msgIndex++)
		{
			char drawChar = msg.charAt(msgIndex);
			if (drawChar != '\n')
			{
				g.drawChar(drawChar, x, y, (Graphics.LEFT | Graphics.TOP));
				x = x + screenFont.charWidth(drawChar);
			}
			else
			{
				// ���s�R�[�h�̂Ƃ��͕\�������ɉ��s����...
				x = x + limitX;
			}
			if (x >= limitX)
			{
				x = 0;
				y = y + screenFont.getHeight();
				line++;
			}
		}
        return (line + 1);
	}

	/**
	 * �R�}���h���s���̏���
	 * 
	 **/
	public void commandAction(Command c, Displayable d)
	{
        if (enableExit == false)
        {
        	 // �R�}���h��t������Ȃ��ꍇ�ɂ͏I������B
        	 return;
        }
        if (c == exitCmd)
        {
        	// �A�v���P�[�V�����̏I��...
        	parent.quit_a2BD();
            return;
        }
        return;
    }
}