package jp.sourceforge.nnsi.a2b.utilities;
import java.io.InputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Image;
import javax.microedition.media.Manager;
import javax.microedition.media.Player;
import javax.microedition.media.control.VolumeControl;

/**
 * ���f�B�A����֘A���[�e�B���e�B<br>
 * <ul>
 *   <li>�摜�̕\��(JPEG�̃T���l�[���\��)</li>
 *   <li>�摜�̕\��</li>
 *   <li>�T�E���h�̍Đ��E��~</li>
 * </ul>
 * ���\�ł��B<br><br>
 * �������A<em>���ۂɓ��삷�邩�ǂ����́A�[���̃T�|�[�g��c�胁�����e�ʂɈˑ����܂��B</em>
 * 
 * @author MRSa
 */
public class MidpFileMediaUtils
{
    private final int TEMP_READ_SIZE = 32768;
    private Player      musicPlayer = null;

    /**
     * �R���X�g���N�^�ł͉������������܂���
     *
     */
    public MidpFileMediaUtils()
    {
        // �Ȃɂ����Ȃ�...
    }

    /**
     *  �C���[�W�t�@�C����ǂݏo���A�������ɓW�J���܂�<BR>
     *  ���̂Ƃ��AJPEG�̃T���l�[����񂪂���ꍇ�ɂ́A���̏��𗘗p����<br>
     *  �摜�f�[�^���������ɓW�J�ł��邩�����܂��B<BR>
     *  <br>
     *  �T���l�[���摜���g�p���邱�ƂŁA�ʏ�J���Ȃ��傫���̉摜�t�@�C����
     *  ���e���Q�Ƃł���A��������܂���B
     *  
     * @param fileName �t�@�C����
     * @return         �C���[�W�f�[�^
     */
    public Image LoadJpegThumbnailImageFromFile(String fileName)
    {
        Image          image      = null;
        FileConnection connection = null;
        InputStream    in         = null;
        byte[]         buffer     = null;
        try
        {
            ///// ��x���ǂ݁A�A�A�T���l�[���摜�����邩�T��
            // �t�@�C���̃I�[�v��
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);

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
            return (LoadImageFromFile(fileName));
        }
        return (image);
    }

    /**
     *  �C���[�W�t�@�C����ǂݏo���A�������ɓW�J���ĉ������܂�
     * 
     * @param fileName �t�@�C����
     * @return         �C���[�W�f�[�^
     */
    public Image LoadImageFromFile(String fileName)
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
     *  �T�E���h��炵�܂��B(�����炷�����ł��B)
     * 
     * @param name �t�@�C����
     * @param playType �v���C�`��(�e�L�X�g�f�[�^)
     */
    public void PlaySound(String name, String playType)
    {
        if (name.indexOf("file://") >= 0)
        {
            // �t�@�C������ǂݏo��
        	PlaySoundFromFile(name, playType);
        }
        else
        {
            // ���\�[�X����ǂݏo��
        	PlaySoundFromResource(name, playType);
        }
        return;
    }
    
    /**
     * �T�E���h�t�@�C����ǂݏo���A�T�E���h��炵�܂��B(�����炷�����ł��B)
     * 
     * @param fileName �t�@�C����
     * @param playType �v���C�`��(�e�L�X�g�f�[�^)
     */
    public void PlaySoundFromFile(String fileName, String playType)
    {
        FileConnection connection = null;
        try
        {
            // �t�@�C���̃I�[�v��
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);
            InputStream in = connection.openInputStream();

            // �T�E���h�𗬂�...
            musicPlayer = null;
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
     * ���\�[�X����T�E���h�t�@�C����ǂݏo���A�T�E���h��炵�܂��B(�����炷�����ł��B)
     * 
     * @param name ���\�[�X(�t�@�C��)��
     * @param playType �v���C�`��(�e�L�X�g�f�[�^)
     */
    public void PlaySoundFromResource(String name, String playType)
    {
        try
        {
            // �t�@�C���̃I�[�v��
		    InputStream in = (this.getClass()).getResourceAsStream(name);

            // �T�E���h�𗬂�...
            musicPlayer = null;
            musicPlayer = Manager.createPlayer(in, playType);
            musicPlayer.start();
            in.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
        }
        System.gc();
        return;
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
        return;
    }

    /**
     *  �{�����[����؂�ւ��� (���Ύw��)
     * @param upDown �ύX����{�����[���T�C�Y�i���Ύw��A���݂̃{�����[�������������ʂ��w�肷��j
     */
    public void SetVolume(int upDown)
    {
    	if (musicPlayer == null)
    	{
    		return;
    	}
    	VolumeControl cnt = (VolumeControl) musicPlayer.getControl("VolumeControl");
    	int level = cnt.getLevel() + upDown;
    	if (level < 0)
    	{
    		level = 0;
    	}
    	else if (level > 100)
    	{
    		level = 100;
    	}
    	cnt.setLevel(level);
    }

    /**
     *  �~���[�g��Ԃ̐ؑ�
     * @param releaseMuting true�Ȃ疳�����炷
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
