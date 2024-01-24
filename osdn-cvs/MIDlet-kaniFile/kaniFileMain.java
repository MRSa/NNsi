import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

/**
 * kaniFileMain : �ȈՃt�@�C���Ǘ��c�[��
 * @author MRSa
 *
 */
public class kaniFileMain extends MIDlet
{
	private kaniFileSceneSelector sceneSelector = null;
	
	/**
	 *  �R���X�g���N�^
	 *
	 */
	public kaniFileMain()
	{
		super();

		// �I�u�W�F�N�g�𐶐����Ă���
		sceneSelector = new kaniFileSceneSelector(this);
	}

	/**
	 *  �A�v���P�[�V�����̋N������
	 * 
	 */
	protected void startApp() throws MIDletStateChangeException
	{
		splashScreen startScreen = new splashScreen();

		// �X�v���b�V���X�N���[����\������...
		Display.getDisplay(this).setCurrent(startScreen);

		// �V�[���Z���N�^�ɏ��������w������
		boolean ret = sceneSelector.initialize();
		if (ret == false)
		{
			// �N�����s...�I������
			destroyApp(true);
			return;
		}

		// �V�[���Z���N�^�ɏ����̊J�n���w������
		sceneSelector.start();
		
		// ���Ƃ��܂�(�X�v���b�V���X�N���[��������)...
		startScreen.finish();
		startScreen = null;

		return;
	}

	/**
	 *  �A�v���P�[�V�����̈ꎞ��~����
	 * 
	 */
	protected void pauseApp()
	{
		// ���݂̂Ƃ���A�������Ȃ�...
		return;
	}

	/**
	 *  �A�v���P�[�V�����̏I������
	 * 
	 */
	protected void destroyApp(boolean arg0) throws MIDletStateChangeException
	{
		if (arg0 == true)
		{
			// �f�[�^�̃o�b�N�A�b�v�ƃI�u�W�F�N�g�̍폜�����s����
			sceneSelector.stop();

			// �I���w��
			notifyDestroyed();
		}
		return;
	}

	/**
	 * �X�v���b�V���X�N���[��(�N�����)�̕\���N���X
	 * 
	 * @author MRSa
	 *
	 */
	private class splashScreen extends Canvas
	{
		Image image = null;  // �C���[�W�t�@�C���̏I��
		
		/**
		 * �R���X�g���N�^(�������Ȃ�)
		 *
		 */
		public splashScreen()
		{
			// �C���[�W�t�@�C����ǂݏo��
			try
			{
				image = Image.createImage("/res/splash.png");
			}
			catch (Exception e)
			{
				image = null;
			}
		}

		/**
		 *  �N����ʂ�\�����������
		 *  
		 */
		public void paint(Graphics g)
		{
			// �^�C�g���̐ݒ�
			String screenTitle = "kaniFile";
			
			// ��ʂ̓h��Ԃ�
			g.setColor(255, 255, 255);
			g.fillRect(0, 0, getWidth(), getHeight());
			
			if (image == null)
			{
				// �^�C�g���̕\�� (����)
				g.setColor(32, 32, 32);
				Font font = Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_BOLD, Font.SIZE_LARGE);
				g.setFont(font);
				g.drawString(screenTitle, 0, (getHeight() / 2), (Graphics.LEFT | Graphics.TOP));
				return;
			}
			else
			{
				// �C���[�W�̕\�����W�����߂�
				int x = ((getWidth()  - image.getWidth())  / 2);
				if ((x + image.getWidth()) > getWidth())
				{
					x = 0;
				}

				int y = ((getHeight() - image.getHeight()) / 2);
				if ((y + image.getHeight()) > getHeight())
				{
					y = 0;
				}
				
				// �X�v���b�V���X�N���[���̃C���[�W��`��
				g.setColor(0);
				g.drawImage(image, x, y, (Graphics.LEFT | Graphics.TOP));
			}
			return;
		}

		/**
		 *   �N����ʂ̌�n��
		 *
		 */
		public void finish()
		{
			// �K�x�R�����{...
			image = null;
			System.gc();
			
			return;
		}
	}
}
