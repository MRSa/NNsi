import java.io.InputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Image;
import javax.microedition.media.Player;
import javax.microedition.media.Manager;
import javax.microedition.media.control.VolumeControl;

/**
 * ���[�e�B���e�B�N���X�Q...
 * @author MRSa
 */
public class kaniFileUtils
{
    private final int TEMP_READ_SIZE = 32768;
    private Player     musicPlayer = null;

    /**
	 * �R���X�g���N�^...
	 *
	 */
    kaniFileUtils()
    {
    	// �Ȃɂ����Ȃ�...
    }

	/**
	 * �����~�܂�
	 * 
	 * @param time (ms)
	 */
	static public void sleep(long time)
	{
		try
		{
			Thread.sleep(time);
		}
		catch (Exception e)
		{
		    // �������Ȃ�...
		}
        return;
	}


	/**
	 *  �C���[�W�t�@�C��(png�`��)��ǂݏo���A�������ɓW�J����
	 * 
	 * @param fileName �t�@�C����
	 * @return         �C���[�W�f�[�^
	 */
	public Image loadJpegThumbnailImageFromFile(String fileName)
	{
		Image          image      = null;
        FileConnection connection = null;
        InputStream    in         = null;
        byte[]        buffer     = null;
        try
		{
            ///// ��x���ǂ݁A�A�A�T���l�[���摜�����邩�T��
        	// �t�@�C���̃I�[�v��
			connection  = (FileConnection) Connector.open(fileName, Connector.READ);
//			int size   = (int) connection.fileSize();

			// �f�[�^���ŏ����� size���ǂݏo��...
			in      = connection.openInputStream();
			int startThumbnailOffset = 0;
			int endThumbnailOffset = 0;
            int dataReadSize = 0;
            int currentReadSize = 0;
            byte[] tempBuffer = new byte[TEMP_READ_SIZE + 8];
            tempBuffer[0] = 0x00;
            while (currentReadSize >= 0)
            {
                int index = 0;
            	currentReadSize = in.read(tempBuffer, 1, TEMP_READ_SIZE);
                if (dataReadSize == 0)
                {
                    // �擪�}�[�J���΂�
                	index = index + 3;
                }
                while (index < currentReadSize)
                {
                	if ((tempBuffer[index] == (byte) 0xff)&&(tempBuffer[index + 1] == (byte) 0xd8))
                	{
                        startThumbnailOffset = index + dataReadSize;
                	}
                	if ((tempBuffer[index] == (byte) 0xff)&&(tempBuffer[index + 1] == (byte) 0xd9))
                	{
                        endThumbnailOffset = index + dataReadSize;
                        if (startThumbnailOffset != 0)
                        {
                            currentReadSize = -1;
                            break;
                        }
                	}
                	index++;
                }
                if ((currentReadSize >= 0)&&(index == currentReadSize))
                {
                    tempBuffer[0] = tempBuffer[currentReadSize - 1];
                }
                dataReadSize = dataReadSize + currentReadSize;
            }
            // �t�@�C���̃N���[�Y
			in.close();
			connection.close();
			in = null;
			connection = null;
			tempBuffer = null;

			///// ��x���ǂ݁A�A�A�T���l�[���摜�����邩�T��
        	// �t�@�C���̃I�[�v��
			connection  = (FileConnection) Connector.open(fileName, Connector.READ);

			// �f�[�^���ŏ����� size���ǂݏo��...
			in      = connection.openInputStream();
            int bufferSize = endThumbnailOffset - startThumbnailOffset + 10;
            if (bufferSize < 0)
            {
            	bufferSize = (int) connection.fileSize();
            }
            if (startThumbnailOffset < 0)
            {
            	startThumbnailOffset = 0;
            }
			buffer = new byte[bufferSize + 4];

			// �t�@�C���̐擪�܂œǂݏo��...
			if (startThumbnailOffset != 0)
			{
				//in.skip(startThumbnailOffset);

				// in.skip(startThumbnailOffset)�̏��������܂�ɂ��x�������̂ŁA�A�A�J���ǂݏ����B				
				int startOffset = startThumbnailOffset - 1;
				while (startOffset > 0)
				{
					int readSize = 0;
					if (startOffset > bufferSize)
					{
						readSize = in.read(buffer, 0, bufferSize);
					}
					else
					{
						readSize = in.read(buffer, 0, startOffset);
					}
					startOffset = startOffset - readSize;
				}
			}
			
			// �f�[�^�̓ǂݏo������...
			currentReadSize = in.read(buffer, 0, bufferSize);
			
            // �t�@�C���̃N���[�Y
			in.close();
			connection.close();

			// �ǎ��f�[�^�Ƃ��ăC���[�W�ɕϊ�...
            image = Image.createImage(buffer, 0, bufferSize);
            
            // �o�b�t�@���N���A����
            buffer = null;

		}
        catch (OutOfMemoryError e)
        {
			// �������I�[�o�t���[��O�����I�I�I
			try
			{
				if (in != null)
				{
					in.close();
				}
			}
			catch (Exception e2)
			{
				// (��O�������ɂ͉������Ȃ�)
			}
			try
			{
				if (connection != null)
				{
					connection.close();
				}
			}
			catch (Exception e2)
			{
				// (��O�������ɂ͉������Ȃ�)
			}
			// �ǂݍ��ݎ��s�A�f�[�^��j������
			buffer  = null;
			image   = null;
        }
		catch (Exception e)
		{
			// ��O�����I�I�I
			try
			{
				if (in != null)
				{
					in.close();
				}
			}
			catch (Exception e2)
			{
				// (��O�������ɂ͉������Ȃ�)
			}
			try
			{
				if (connection != null)
				{
					connection.close();
				}
			}
			catch (Exception e2)
			{
				// (��O�������ɂ͉������Ȃ�)
			}
			// �ǂݍ��ݎ��s�A�f�[�^��j������
			buffer  = null;
			image   = null;
		}
		System.gc();
		if (image == null)
		{
            // �T���l�[���̎擾�Ɏ��s�����ꍇ�ɂ́B�B�B
            return (loadImageFromFile(fileName));
		}
		return (image);
	}

	/**
	 *  �C���[�W�t�@�C��(png�`��)��ǂݏo���A�������ɓW�J����
	 * 
	 * @param fileName �t�@�C����
	 * @return         �C���[�W�f�[�^
	 */
	public Image loadImageFromFile(String fileName)
	{
		Image          image      = null;
        FileConnection connection = null;
        try
		{
			// �t�@�C���̃I�[�v��
			connection  = (FileConnection) Connector.open(fileName, Connector.READ);
			InputStream in = connection.openInputStream();
            image = Image.createImage(in);
			in.close();
			connection.close();
		}
        catch (OutOfMemoryError e)
        {
			// �������I�[�o�t���[��O�����I�I�I
			try
			{
				if (connection != null)
				{
					connection.close();
				}
			}
			catch (Exception e2)
			{
				// (��O�������ɂ͉������Ȃ�)
			}
			// �ǂݍ��ݎ��s�A�f�[�^��j������
        	image = null;
        }
		catch (Exception e)
		{
			// ��O�����I�I�I
			try
			{
				if (connection != null)
				{
					connection.close();
				}
			}
			catch (Exception e2)
			{
				// (��O�������ɂ͉������Ȃ�)
			}
			// �ǂݍ��ݎ��s�A�f�[�^��j������
			image   = null;
		}
		System.gc();
		return (image);
	}
	

	/**
	 * �T�E���h�̃v���C�o�b�N
	 * 
	 * @param fileName
	 */
	public void playSoundFromFile(String fileName, String playType)
	{
        FileConnection connection = null;
        try
		{
            // �T�E���h�����Ă���ꍇ�ɂ́A��x�~�߂�B
        	if (musicPlayer != null)
            {
            	StopSound();
            }
        	
        	// �t�@�C���̃I�[�v��
			connection  = (FileConnection) Connector.open(fileName, Connector.READ);
			InputStream in = connection.openInputStream();

			// �T�E���h�𗬂�...
			musicPlayer = Manager.createPlayer(in, playType);
			musicPlayer.start();
            in.close();
			connection.close();
		}
        catch (Exception e)
        {
			// ��O�����I�I�I
			try
			{
				if (connection != null)
				{
					connection.close();
				}
			}
			catch (Exception e2)
			{
				// (��O�������ɂ͉������Ȃ�)
			}
        }
		System.gc();
		return;
	}

    /**
     *  �T�E���h�Đ������ǂ����`�F�b�N
     * @return
     */
	public boolean isPlayingSound()
	{
		if (musicPlayer == null)
		{
			return (false);
		}
		return (true);
	}

	/**
     *  �T�E���h�Đ��̒�~���s��
     *
     */
    public void StopSound()
    {
        if (musicPlayer == null)
        {
        	return;
        }
        try
        {
        	musicPlayer.stop();
        	musicPlayer.close();
        }
        catch (Exception ex)
        {
        	// �������Ȃ�
        }
        musicPlayer = null;
        System.gc();
    }
    
    /**
     *  �{�����[����؂�ւ���
     * @param upDown
     */
    public void SetVolume(int upDown)
    {
    	if (musicPlayer == null)
    	{
    		return;
    	}
    	VolumeControl cnt = (VolumeControl) musicPlayer.getControl("VolumeControl");
    	int level = cnt.getLevel() + upDown;
    	cnt.setLevel(level);
    }

    /**
     *  �~���[�g��Ԃ̐ؑ�
     * @param 
     */
    public void ToggleMute(boolean releaseMuting)
    {
    	if (musicPlayer == null)
    	{
    		return;
    	}
    	VolumeControl cnt = (VolumeControl) musicPlayer.getControl("VolumeControl");
    	boolean isMuting = cnt.isMuted();
    	if ((isMuting == true)||(releaseMuting == true))
    	{
    		isMuting = false;
    	}
    	else
    	{
    		isMuting = true;
    	}
    	cnt.setMute(isMuting);
    	return;
    }
}
