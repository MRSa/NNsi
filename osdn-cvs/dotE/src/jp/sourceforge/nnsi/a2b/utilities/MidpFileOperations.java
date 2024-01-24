package jp.sourceforge.nnsi.a2b.utilities;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 * JSR75(FileConnection API)�̃t�@�C�����샆�[�e�B���e�B<br><br>
 * <ul>
 *   <li>�f�B���N�g���̍쐬</li>
 *   <li>�t�@�C���̑��ݗL��</li>
 *   <li>�t�@�C���̈ꊇ�ǂݏo��</li>
 *   <li>�t�@�C���̍폜</li>
 *   <li>�t�@�C���̃��l�[��</li>
 *   <li>�t�@�C���̑����ύX</li>
 *   <li>�t�@�C���̃R�s�[</li>
 *   <li>�t�@�C���ւ̃f�[�^�o��</li>
 *  </ul>  
 *  ���\�ł��B
 * 
 * @author MRSa
 *
 */
public class MidpFileOperations
{
    private final int COPY_BUFFER_SIZE = 32768;
    private final int BUFFER_MARGIN    = 4;

    /**
     *   �R���X�g���N�^�ł͉����������s���܂���
     */
    public MidpFileOperations()
    {
        // �Ƃ肠�����A�������Ȃ�...
    }

    /**
     * �f�B���N�g���̍쐬
     * @param directoryName �f�B���N�g���� (full path)
     * @return �f�B���N�g���쐬����(true)/���s(false)
     */
    public boolean MakeDirectory(String directoryName)
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

    /**
     *  �t�@�C���T�C�Y���擾����
     * 
     * @param targetFileName �m�F����t�@�C����
     * @return �t�@�C���T�C�Y
     */
    static public long GetFileSize(String targetFileName)
    {
        long           ret = -1;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            ret = dataFileConnection.fileSize();
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
            ret = -1;
        }
        return (ret);
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
     *  �t�@�C�����ꊇ���ēǂݏo��
     * 
     * @param targetFileName �ǂݏo���t�@�C����
     * @return �ǂݏo�����f�[�^
     */
    public byte[] ReadFileWhole(String targetFileName)
    {
        byte[]    readData = null;
        int       readSize = -1;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            int size = (int) dataFileConnection.fileSize();
            readData = new byte[size];
            readSize = (dataFileConnection.openInputStream()).read(readData);
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            try
            {
                dataFileConnection.close();
            }
            catch (Exception e2)
            {
                //
            }
            readSize = -1;
        }
        dataFileConnection = null;
        if (readSize <= 0)
        {
        	readData = null;
        	return (null);
        }
        return (readData);
    }

    /**
     *  �t�@�C���̕����ǂݏo��
     * 
     * @param targetFileName �ǂݏo���t�@�C����
     * @param startOffset �ǂݏo���擪�̃T�C�Y
     * @param getSize �ǂݏo���f�[�^�T�C�Y
     * @return �ǂݏo�����f�[�^
     */
    public byte[] ReadFilePart(String targetFileName, int startOffset, int getSize)
    {
    	int       size     = -1;
        byte[]    readData = null;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            InputStream in = dataFileConnection.openInputStream();
            size   = (int) dataFileConnection.fileSize();
            readData = new byte[getSize];
			while (startOffset > 0)
			{
				int readSize = 0;
				if (startOffset > getSize)
				{
					readSize = in.read(readData, 0, getSize);
				}
				else
				{
					readSize = in.read(readData, 0, startOffset);
				}
				startOffset = startOffset - readSize;
			}
			size = in.read(readData, 0, getSize);
			in.close();
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            try
            {
                dataFileConnection.close();
            }
            catch (Exception e2)
            {
                //
            }
        }
        dataFileConnection = null;
        if (size <= 0)
        {
        	readData = null;
        	System.gc();
        	return (null);
        }
        return (readData);
    }

    /**
     * �t�@�C���̍폜
     * @param targetFileName �폜����t�@�C�� (full path)
     * @return �폜����(true)/�폜���s(false)
     */
    public boolean DeleteFile(String targetFileName)
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
     * @return ���l�[������(true)/���l�[�����s(false)
     */
    public boolean RenameFile(String srcFileName, String renameFile)
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
     * @return �����ύX����(true)/�����ύX���s(false)
     */
    public boolean ChangeAttribute(String fileName, boolean writable, boolean hidden)
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
     * @return �R�s�[����(true)/�R�s�[���s(false)
     */
    public boolean CopyFile(String destFileName, String srcFileName)
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
	 *  �e�L�X�g�t�@�C���Ƀf�[�^���o�͂���
	 * 
	 * @param outputFileName  �o�̓t�@�C����
	 * @param separator       �^�C�g���̑O�ɏo�͂��镶����
	 * @param title           �^�C�g��
	 * @param message         �o�̓f�[�^
	 * @param isAppendMode    �㏑�����[�h(false)/�ǋL���[�h(true)
	 * @return �f�[�^�o�͐���(true)/�f�[�^�o�͎��s(false)
	 */
	public boolean outputDataTextFile(String outputFileName, String separator, String title, String message, boolean isAppendMode)
	{
        boolean ret = true;
		FileConnection connection = null;
		OutputStream  out = null;
        try 
		{
			// �t�@�C���̏������݃I�[�v��
			connection = (FileConnection) Connector.open(outputFileName, Connector.READ_WRITE);
			if (connection.exists() != true)
			{
				// �t�@�C���쐬
				connection.create();
			}
			else
			{
				if (isAppendMode == false)
				{
					// �t�@�C�������݂����ꍇ�ɂ́A��x�����č�蒼��
					connection.delete();
					connection.create();
				}
			}
			
			// �t�@�C���̖����܂œǂݔ�΂��B�B�B
            long fileSize = connection.fileSize();
            out = connection.openOutputStream(fileSize);

			// �t�@�C���ɏo�͂���
			String data;
            if (separator != null)
			{
                out.write(separator.getBytes());
			}
			if (title != null)
			{
                data = title  + "\r\n";
			    out.write(data.getBytes());
			}
			if (message != null)
			{
                data = message  + "\r\n";
			    out.write(data.getBytes());
			}
			out.close();
			connection.close();
		}
		catch (Exception e)
		{
			// ��O�����I�I�I
			try
			{
				if (out != null)
				{
					out.close();
				}
			}
			catch (Exception e2)
			{
				//			
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
				//
			}
			ret = false;
		}
        // ��n��
		out = null;
		connection = null;
		System.gc();
		return (ret);
	}
    /**
	 *  �t�@�C���Ƀf�[�^���o�͂���
	 * 
	 * @param outputFileName  �o�̓t�@�C����
	 * @param data            �o�̓f�[�^
	 * @param isAppendMode    �㏑�����[�h(false)/�ǋL���[�h(true)
	 * @return �f�[�^�o�͐���(true)/�f�[�^�o�͎��s(false)
	 */
	public boolean outputRawDataToFile(String outputFileName, byte[] data, boolean isAppendMode)
	{
        boolean ret = true;
		FileConnection connection = null;
		OutputStream  out = null;
        try 
		{
			// �t�@�C���̏������݃I�[�v��
			connection = (FileConnection) Connector.open(outputFileName, Connector.READ_WRITE);
			if (connection.exists() != true)
			{
				// �t�@�C���쐬
				connection.create();
			}
			else
			{
				if (isAppendMode == false)
				{
					// �t�@�C�������݂����ꍇ�ɂ́A��x�����č�蒼��
					connection.delete();
					connection.create();
				}
			}
			
			// �t�@�C���̖����܂œǂݔ�΂��B�B�B
            long fileSize = connection.fileSize();
            out = connection.openOutputStream(fileSize);

			// �t�@�C���ɏo�͂���
            out.write(data);

            // �o�͌�̌㏈��...
            out.close();
			connection.close();
		}
		catch (Exception e)
		{
			// ��O�����I�I�I
			try
			{
				if (out != null)
				{
					out.close();
				}
			}
			catch (Exception e2)
			{
				//			
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
				//
			}
			ret = false;
		}
        // ��n��
		out = null;
		connection = null;
		System.gc();
		return (ret);
	}
}
