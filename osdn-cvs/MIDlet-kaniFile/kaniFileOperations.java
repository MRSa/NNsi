import java.util.Vector;
import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Image;

/**
 * �t�@�C��������܂Ƃ߂��N���X...
 * 
 * @author MRSa
 *
 */
public class kaniFileOperations
{
	private final int COPY_BUFFER_SIZE = 32768;
	private final int BUFFER_MARGIN    = 4;
    private kaniFileSceneSelector parent = null;
	
	/**
     * �R���X�g���N�^...
     */
	public kaniFileOperations(kaniFileSceneSelector selector)
	{
		// �Ƃ肠�����A�������Ȃ�...
		parent = selector;
	}

	/**
     *  �t�@�C���̑��݂��m�F����
     * 
     * @param targetFileName �m�F����t�@�C����
     * @return <code>true</code>:���݂��� / <code>false</code>:���݂��Ȃ�
     */
    static public boolean IsExistFile(String targetFileName)
    {
        boolean       ret = false;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            ret = dataFileConnection.exists();
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            try
            {
                if (dataFileConnection != null)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception e2)
            {
                //
            }
            ret = false;
        }
        return (ret);
    }

	/**
     * �f�B���N�g���̍쐬
     * @param directoryName �f�B���N�g���� (full path)
     */
	static public boolean MakeDirectory(String directoryName)
	{
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
           int index = 9;
           while (true)
           {
               String tempDirectory = new String(directoryName);
               int pos = tempDirectory.indexOf("/", index);
               if ((pos < 0)||(pos >= tempDirectory.length()))
               {
            	   // �����܂ł��ǂ���...������
            	   break;
               }
               String testDirectory = tempDirectory.substring(0, pos + 1);
		       index = pos + 1;
               dataFileConnection = (FileConnection) Connector.open(testDirectory, Connector.READ_WRITE);
			   if (dataFileConnection.exists() != true)
			   {
                    // �t�@�C�������݂��Ȃ������Ƃ��A�f�B���N�g�����쐬����
			        dataFileConnection.mkdir();
			   }
			   dataFileConnection.close();
			   dataFileConnection = null;
			}

            // �Ō�ɖ{����Ή�...
			dataFileConnection = (FileConnection) Connector.open(directoryName, Connector.READ_WRITE);
			if (dataFileConnection.exists() != true)
			{
				// �t�@�C�������݂��Ȃ������Ƃ��A�f�B���N�g�����쐬����
			    dataFileConnection.mkdir();
			    ret = true;
			}
			dataFileConnection.close();
		}
		catch (Exception e)
		{
			try
			{
				if (dataFileConnection != null)
				{
					dataFileConnection.close();
				}
				dataFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (false);
		}
		return (ret);
	}

    /**
     * �f�B���N�g���̍쐬
     * @param directoryName �f�B���N�g���� (full path)
     */
/*
	public boolean makeDirectory(String directoryName)
	{
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(directoryName, Connector.READ_WRITE);
			if (dataFileConnection.exists() != true)
			{
				// �t�@�C�������݂��Ȃ������Ƃ��A�f�B���N�g�����쐬����
			    dataFileConnection.mkdir();
			    ret = true;
			}
			dataFileConnection.close();
		}
		catch (Exception e)
		{
			try
			{
				if (dataFileConnection != null)
				{
					dataFileConnection.close();
				}
				dataFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (false);
		}
		return (ret);
	}
*/

	/**
	 * �t�@�C���̍폜
	 * @param targetFileName �폜����t�@�C�� (full path)
	 */
	public boolean deleteFile(String targetFileName)
	{
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() == true)
			{
				// �t�@�C�����������Ƃ��ɂ́A�폜�����s����
				dataFileConnection.delete();
				ret = true;
			}
			dataFileConnection.close();
		}
		catch (Exception e)
		{
			// ��O�����I�I�I
			try
			{
				if (dataFileConnection != null)
				{
					dataFileConnection.close();
				}
				dataFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (false);
		}
		return (ret);
	}

    /**
     * �t�@�C���̃��l�[��
     * @param srcFileName  ���O��ύX����t�@�C�� (full path)
     * @param renameFile   �ύX��̃t�@�C����     (�t�@�C�����̂�)
     */
	public boolean renameFile(String srcFileName, String renameFile)
	{
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(srcFileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() == true)
			{
				// �t�@�C�������݂����Ƃ��A���l�[������
			    dataFileConnection.rename(renameFile);
			    ret = true;
			}
			dataFileConnection.close();
		}
		catch (Exception e)
		{
			try
			{
				if (dataFileConnection != null)
				{
					dataFileConnection.close();
				}
				dataFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (false);
		}
		return (ret);
	}

	/**
	 * �t�@�C���̑����ύX�B�B�B
	 * 
	 * @param fileName ������ύX����t�@�C����
	 * @param writable �������ݑ���
	 * @param hidden   �B���t�@�C������
	 * @return
	 */
    public boolean changeAttribute(String fileName, boolean writable, boolean hidden)
    {
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() == true)
			{
				// �t�@�C�������݂����Ƃ��A������ύX����
			    dataFileConnection.setWritable(writable);
			    dataFileConnection.setHidden(hidden);
			    ret = true;
			}
			dataFileConnection.close();
		}
		catch (Exception e)
		{
			try
			{
				if (dataFileConnection != null)
				{
					dataFileConnection.close();
				}
				dataFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (false);
		}
		return (ret);
    }
	
    /**
     * �t�@�C���̃R�s�[
     * 
     * @param destFileName �R�s�[��t�@�C�� (full path)
     * @param srcFileName  �R�s�[���t�@�C�� (full path)
     */
	public boolean copyFile(String destFileName, String srcFileName)
	{
		FileConnection srcFileConnection = null;
		FileConnection dstFileConnection = null;
		
		boolean     ret = false;
		InputStream   is = null;
		OutputStream  os = null;

		if (destFileName == srcFileName)
		{
			// �t�@�C�����������������ꍇ�ɂ̓R�s�[�����s���Ȃ�
			return (false);
		}
		
		try
		{
			srcFileConnection = (FileConnection) Connector.open(srcFileName, Connector.READ_WRITE);
			if (srcFileConnection.exists() != true)
			{
				// �t�@�C�������݂��Ȃ������A�A�A�I������
				srcFileConnection.close();
				return (false);
			}
			is = srcFileConnection.openInputStream();

			long dataFileSize = srcFileConnection.fileSize();
			byte[] buffer = new byte[COPY_BUFFER_SIZE + BUFFER_MARGIN];

			dstFileConnection = (FileConnection) Connector.open(destFileName, Connector.READ_WRITE);
			if (dstFileConnection.exists() == true)
			{
				// �t�@�C�������݂����A�A�A�폜���č�蒼��
				dstFileConnection.delete();
			}
			dstFileConnection.create();

			os = dstFileConnection.openOutputStream();
			if ((is != null)&&(os != null))
			{
				while (dataFileSize > 0)
				{
			        int size = is.read(buffer, 0, COPY_BUFFER_SIZE);
			        if (size <= 0)
			        {
			        	break;
			        }
			        os.write(buffer, 0, size);
				}
			}
			os.close();
			is.close();
			dstFileConnection.close();
			srcFileConnection.close();

			buffer = null;
			is = null;
			os = null;
			srcFileConnection = null;
			dstFileConnection = null;
			ret = true;
			System.gc();
		}
		catch (Exception e)
		{
			// ��O�����I�I�I
			try
			{
				if (is != null)
				{
					is.close();
				}
				is = null;
				
				if (os != null)
				{
					os.close();
				}
				os = null;
				
				if (srcFileConnection != null)
				{
					srcFileConnection.close();
				}
				srcFileConnection = null;

				if (dstFileConnection != null)
				{
					dstFileConnection.close();
				}
				dstFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (false);
		}
		return (ret);
	}

	/**
	 *  �t�@�C���ꗗ��html�t�@�C���ɏo�͂���
	 * 
	 * @param fileName
	 * @param fileListString
	 * @return
	 */
	public boolean outputFileListAsHtml(String fileName, Vector fileListString)
	{
		boolean                     ret = false;
		OutputStream                  os = null;
		FileConnection dstFileConnection = null;

		try
		{
            dstFileConnection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
			if (dstFileConnection.exists() == true)
			{
				// �t�@�C�������݂����A�A�A�폜���č�蒼��
				dstFileConnection.delete();
			}
			dstFileConnection.create();

			os = dstFileConnection.openOutputStream();
			if (os != null)
			{
				// HTML�t�@�C���̏o��...
				os.write("<html>\r\n<head>\r\n<title>".getBytes());
				os.write(fileName.getBytes());
				os.write("</title>\r\n</head>\r\n<body>\r\n".getBytes());

				int count = fileListString.size();
				for (int loop = 0; loop < count; loop++)
				{
			        String targetFileName = (String) fileListString.elementAt(loop);
			        String item = "<a href=\"" + targetFileName + "\">" + targetFileName + "</a><br>\r\n";
			        os.write(item.getBytes());
				}
	            // �ukaniFiler�֖߂�v�^�O�𖄂ߍ��ށB�i�Ή��@��̂݁j
				String platForm = System.getProperty("microedition.platform");
				if ((platForm.indexOf("WS023T") >= 0)||
                     (platForm.indexOf("WS018KE") >= 0)||
                     (platForm.indexOf("WS009KE") >= 0)||
                     (platForm.indexOf("WX0") >= 0)||
                     (platForm.indexOf("WX33") >= 0)||
                     (platForm.indexOf("WX34") >= 0)||
                     (platForm.indexOf("WX35") >= 0))
		        {
		            String data = "<hr><object id=\"app\" declare=\"declare\" classid=\"x-oma-application:java-ams\">";
		            data = data + "\r\n" + "  <param name=\"AMS-Filename\" value=\"http://nnsi.sourceforge.jp/archives/midp/kaniFiler/kaniFile.jad\"/>";
		            data = data + "\r\n" + "  <param name=\"MIDlet-Name\" value=\""    + parent.getAppProperty("MIDlet-Name") + "\"/>";
		            data = data + "\r\n" + "  <param name=\"MIDlet-Version\" value=\"" + parent.getAppProperty("MIDlet-Version") + "\"/>";
		            data = data + "\r\n" + "  <param name=\"MIDlet-Vendor\" value=\""  + parent.getAppProperty("MIDlet-Vendor") + "\"/>";
		            data = data + "\r\n" + "  <param name=\"AMS-Startup\" value=\"download-confirm\"/>";
		            data = data + "\r\n" + "</object><a href=\"#app\">(" + parent.getAppProperty("MIDlet-Name") + "�֖߂�)</a>\r\n";
	                os.write(data.getBytes());
		        }
                os.write("</body>\r\n</html>\r\n".getBytes());
			}
			os.close();
			dstFileConnection.close();
			os = null;
			dstFileConnection = null;
			ret = true;
			System.gc();
		}
		catch (Exception e)
		{
			// ��O�����I�I�I
			try
			{				
				if (os != null)
				{
					os.close();
				}
				os = null;
				if (dstFileConnection != null)
				{
					dstFileConnection.close();
				}
				dstFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (false);
		}
		return (ret);
	}
	
	
	/**
	 *  �C���[�W�t�@�C��(png�`��)��ǂݏo���A�������ɓW�J����
	 * 
	 * @param fileName �t�@�C���� (full path)
	 * @return         �C���[�W�f�[�^
	 */
	public Image loadImageFromFile(String fileName)
	{
		Image          image      = null;
        FileConnection connection = null;
        InputStream    in         = null;
        byte[]        buffer     = null;
        try
		{
			// �t�@�C���̃I�[�v��
			connection  = (FileConnection) Connector.open(fileName, Connector.READ);
			int size   = (int) connection.fileSize();

			// �f�[�^���ŏ����� size���ǂݏo��...
			in      = connection.openInputStream();
			buffer  = new byte[size + BUFFER_MARGIN];
			int dataReadSize = in.read(buffer, 0, size);

			// �t�@�C���̃N���[�Y
			in.close();
			connection.close();

			// �ǎ��f�[�^�Ƃ��ăC���[�W�ɕϊ�...
            image = Image.createImage(buffer, 0, dataReadSize);
            
            // �o�b�t�@���N���A����
            buffer = null;
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
		return (image);
	}
	
}
