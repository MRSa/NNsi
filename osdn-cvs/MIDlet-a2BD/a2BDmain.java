import java.util.Date;

import javax.microedition.io.HttpConnection;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

/**
 * MIDlet���C���N���X
 * @author MRSa
 *
 */
public class a2BDmain extends MIDlet
{
	private a2BDobjectFactory      objectFactory      = null;    // �I�u�W�F�N�g�Ǘ��N���X
    public  long                  watchDogMarking     = -1;      //  �E�H�b�`�h�b�O�̃}�[�L���O

	private final int             HTTPCOMM_DEFAULT   = 0;	

    /**
	 *   �R���X�g���N�^
	 * 
	 */
	public a2BDmain()
	{
		super();
		// �I�u�W�F�N�g�𐶐����Ă���
		objectFactory = new a2BDobjectFactory(this);
	}

	/**
	 *   �A�v���P�[�V�����N�����ɌĂяo����鏈��
	 * 
	 */
	protected void startApp() throws MIDletStateChangeException
	{
		splashScreen startScreen = new splashScreen();

		// �X�v���b�V���X�N���[����\������...
		Display.getDisplay(this).setCurrent(startScreen);

		// �I�u�W�F�N�g�̏������s��
		objectFactory.prepare();		
		objectFactory.prepareScreen();

		// �A�v���P�[�V�����̃X�^�[�g�w��
		(objectFactory.sceneSelector).start();
		
		// ���Ƃ��܂�...
		startScreen.finish();
		startScreen = null;

	}

	/**
	 *   �A�v���P�[�V�����ꎞ��~���ɌĂяo����鏈��
	 *  
	 */
	protected void pauseApp()
	{
		// ���݂̂Ƃ���A�������Ȃ�...
		return;
	}

	/**
	 *   �A�v���P�[�V��������I�����ɌĂяo����鏈��
	 *   
	 */
	protected void destroyApp(boolean arg0) throws MIDletStateChangeException
	{
		if (arg0 == true)
		{
			// �f�[�^�̃o�b�N�A�b�v�ƃI�u�W�F�N�g�̍폜�����s����
			objectFactory.cleanup();

			// �I���w��
			notifyDestroyed();
		}
		return;
	}


	/** 
	 *   a2BD�I������...
	 *   
	 */
	public void quit_a2BD()
	{
		// a2B�I�����w�����ꂽ�ꍇ...
		try
		{
			// �A�v���I�����Ăяo��...
			destroyApp(true);
		}
		catch (Exception e)
		{
			// ��O�����Ƃ��Ă͉������Ȃ�
		}
		return;
	}

	/**
	 * �\����ʂ̐؂�ւ�
	 * 
	 * @param scene
	 */
	public void changeScene(int scene)
	{
		if (scene == (objectFactory.sceneSelector).SCENE_OPEN_DATAINPUT)
		{
			// ���̓t�H�[����(�V�K)�I�[�v������
			(objectFactory.inputForm).prepareScreen("�t�@�C���擾", "URL", "File", true);
			Display.getDisplay(this).setCurrent((objectFactory.inputForm));
		}
		else if (scene == (objectFactory.sceneSelector).SCENE_DATAINPUT)
		{
			// ���̓t�H�[�����I�[�v������
			Display.getDisplay(this).setCurrent((objectFactory.inputForm));
		}
		else if (scene == (objectFactory.sceneSelector).SCENE_EXECUTE_DATA)
		{
			// BUSY�_�C�A���O��\������
			Display.getDisplay(this).setCurrent((objectFactory.busyScreen));			

			// �����̎��s...
			Thread thread = new Thread()
			{
				public void run()
				{
					// HTTP�ʐM�̎��s
					String url = (objectFactory.preference).getData1();
					String fileName = (objectFactory.preference).getData2();
					startHttpCommunication(HTTPCOMM_DEFAULT, url, (objectFactory.preference).decideFileName(url, fileName), (objectFactory.preference).getWX310workaroundFlag(), (objectFactory.httpCommunicator).APPENDMODE_NEWFILE);
                }
			};
			thread.start();
		}
		else if (scene == (objectFactory.sceneSelector).SCENE_SHOW_BUSY)
		{
			// BUSY�_�C�A���O��\������
			Display.getDisplay(this).setCurrent((objectFactory.busyScreen));			
		}
		else if (scene == (objectFactory.sceneSelector).SCENE_FILE_SELECT)
		{
			// �t�@�C���I���_�C�A���O��\������
			Display.getDisplay(this).setCurrent((objectFactory.fileSelector));						
		}
		else // (scene == (objectFactory.sceneSelector).SCENE_UNKNOWN)
		{
			// �I��...
		}
		return;
	}
	

	/**
	 * HTTP�ʐM�̊J�n
	 * @param url
	 * @param fileName
	 */
	private void startHttpCommunication(int scene, String url, String fileName, boolean isWorkAroundWX310, int mode)
	{
		if ((objectFactory.httpCommunicator).isCommunicating() == true)
		{
			// �ʐM���̂Ƃ��͉������Ȃ��B
			return;
		}

        // HTTP�ʐM�̎��{
		if ((objectFactory.preference).getDivideGetHttp() == false)
		{
            // �ʏ��HTTP�ʐM
			(objectFactory.httpCommunicator).getURLUsingHttp(fileName, url, null, -1, -1, mode, (objectFactory.preference).getUserAgent(), isWorkAroundWX310);
		}
		else
		{
			// HTTP�����擾�̎��{...
			int rc = HttpConnection.HTTP_PARTIAL;
			int startRange = 0;
			(objectFactory.httpCommunicator).getURLUsingHttp(fileName, url, null, 0, -1, (objectFactory.httpCommunicator).APPENDMODE_NEWFILE, (objectFactory.preference).getUserAgent(), isWorkAroundWX310);
			while (rc == HttpConnection.HTTP_PARTIAL)
			{
				startRange = startRange + (objectFactory.httpCommunicator).getDivideGetSize() + 1;
				rc = (objectFactory.httpCommunicator).getURLUsingHttp(fileName, url, null, startRange, -1, (objectFactory.httpCommunicator).APPENDMODE_APPENDFILE, (objectFactory.preference).getUserAgent(), isWorkAroundWX310); 
			}
		}
		if ((objectFactory.preference).getAutoFinish() == true)
		{
            // �����I���̂��߂̃^�C�}��ݒ肷��
			keepWatchDog(0);
			startWatchDog();
		}
        return;
	}

	/**
	 * �f�[�^2��ݒ肷��
	 * @param data
	 */
	public void setData2(String data)
	{
		(objectFactory.preference).setData2(data);
		(objectFactory.inputForm).setData2(data);

		return;
	}
	
    /*------------------------------------------------------------------------------------------------------------*/
    /**
     *   �E�I�b�`�h�b�O�X���b�h
     * 
     */
    public void startWatchDog()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                Date  date      = new Date();
                long time      = date.getTime();
                long dummy     = time;
                long sleepTime = (1 * 1000 * 60) / 10;  // 6 �` 12 �b��Ɏ����I��������
                while (dummy != 0)
                {
                    if (watchDogMarking != -1)
                    {
                        // �E�H�b�`�h�b�O��~���ł͂Ȃ������Ƃ��A�A�A
                        time = date.getTime();
                        watchDogMarking = time;
                    }
                    try
                    {
                        Thread.sleep(sleepTime); // �҂�...
                    }
                    catch (Exception e)
                    {
                        // �������Ȃ�...
                    }
                    if (time == watchDogMarking)
                    {
                        // �^�C���A�E�g���o�A�A�A�A�v���P�[�V�������I������
                        objectFactory.cleanup();
                        notifyDestroyed();
                        return;
                    }
                }
            }
        };
        thread.start();
        return;
    }

    /**
     *  �E�I�b�`�h�b�O���N���A����B�B
     *
     */
    public void keepWatchDog(int number)
    {
        watchDogMarking = number;
        return;
    }
    /*------------------------------------------------------------------------------------------------------------*/

    /**
	 * �X�v���b�V���X�N���[��(�N�����)�̕\���N���X
	 * 
	 * @author MRSa
	 *
	 */
	public class splashScreen extends Canvas
	{
		Image image = null;  // �C���[�W�t�@�C���̏I��
		
		/**
		 * �R���X�g���N�^(�������Ȃ�)
		 *
		 */
		public splashScreen()
		{
			image = null;
		}

		/**
		 *  �N����ʂ�\�����������
		 *  
		 */
		public void paint(Graphics g)
		{
			// �^�C�g���̐ݒ�
			String screenTitle = "a2B downloader";
			
			// ��ʂ̓h��Ԃ�
			g.setColor(255, 255, 255);
			int width = 240;
			int height = 280;
			try
			{
				width = getWidth();
				height = getHeight();
			}
			catch (Exception ex)
			{
				//
			}
			g.fillRect(0, 0, width, height);
			
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
				int x = ((width  - image.getWidth())  / 2);
				if ((x + image.getWidth()) > width)
				{
					x = 0;
				}

				int y = ((height - image.getHeight()) / 2);
				if ((y + image.getHeight()) > height)
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
