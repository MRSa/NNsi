import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import com.jcraft.jzlib.ZOutputStream;

/**
 *   ZIP�t�@�C���̈������s��...
 *   �Q�l�F http://www.pkware.com/documents/casestudies/APPNOTE.TXT
 *   ���p�F jzlib : http://www.jcraft.com/jzlib/  (1.0.7 ����荞��)
 * @author MRSa
 *
 */
public class kaniFileZipOperations
{
	static final private int MAX_WBITS = 15; // 32K LZ77 window
    private kaniFileSceneSelector sceneSelector = null;
	private long   targetFileSize = 0;
	private long   currentReadSize = 0;
	private int   compressedFileDataSize = -1;    // ���ɓW�J����f�[�^�̈��k�f�[�^�T�C�Y
	private String extractTargetFileName = null;  // ���ɓW�J����f�[�^�̓W�J�t�@�C����

	/**
     * �R���X�g���N�^...
     */
	public kaniFileZipOperations(kaniFileSceneSelector parent)
	{
		sceneSelector = parent;
	}

	/**
	 *  ����̐i���󋵂���������
	 * @return ���݂̃t�@�C���W�J�� (%)
	 */
	public String getCurrentStatus()
	{
		if (targetFileSize == 0)
		{
			return ("0/0");
		}
        return (currentReadSize + "/" + targetFileSize);
/*
        double allSize = targetFileSize;
		double current = currentReadSize;
		int answer = (int) ((current / allSize * 100));
		return (answer);
*/
	}
	
	/**
     * ZIP�t�@�C����W�J����
     * @param targetDirectory  �W�J��f�B���N�g��
     * @param targetFileName   �W�J�t�@�C����
     * @param targetFileName   ����f�B���N�g���ɓW�J���邩�H
     * @return
     */
	public boolean extractZipFile(String targetDirectory, String fileName, boolean isSameDirectory)
	{
        boolean       ret = false;
        FileConnection dataFileConnection = null;
        InputStream    istream = null;
        try
        {
        	// �W�J�^�[�Q�b�g�f�B���N�g�����쐬����
        	String extractDirectory = targetDirectory + fileName + ".unzip/";
        	if (isSameDirectory == false)
        	{
			    kaniFileOperations.MakeDirectory(extractDirectory);
        	}
        	else
        	{
        		extractDirectory = targetDirectory;
        	}

			// ���k�t�@�C�����I�[�v������
			String targetFileName = targetDirectory + fileName;
			dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() != true)
			{
				dataFileConnection.close();
				dataFileConnection = null;

				// ���Ƃ��܂�
				currentReadSize = 0;
				targetFileSize = 0;
				System.gc();
                return (false);
			}

			// �I�[�v������t�@�C���̃T�C�Y���擾����
			targetFileSize = dataFileConnection.fileSize();
			currentReadSize = 0;

			// ���������A�{���������s����
			istream = dataFileConnection.openInputStream();
		    
		    try
		    {
			    // ZIP�t�@�C�����Ɋi�[����Ă��鈳�k�t�@�C�������ׂēW�J����B
		    	ret = true;
                while (ret)
		    	{
                    ret = extractZipMain(istream, extractDirectory, isSameDirectory);
		    	}
		    }
		    catch (Exception ex)
		    {
                // �������Ȃ�
		    }
		    istream.close();
		    istream = null;

		    dataFileConnection.close();
			dataFileConnection = null;
		}
		catch (Exception e)
		{
            // ��O�����I�I�I
//          e.printStackTrace();
			try
			{
				if (istream != null)
				{
					istream.close();
				}
				istream = null;
			}
			catch (Exception e2)
			{
				//�������Ȃ�...
			}
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
		
		// ���Ƃ��܂�
		currentReadSize = 0;
		targetFileSize = 0;
		System.gc();
		return (ret);
	}
	
	/**
	 *   ZIP�t�@�C���̓W�J���C������ (1�t�@�C�����̓W�J����)
	 * @param in
	 * @param targetDirectory
	 * @param compressedSize
	 * @param outputSize
	 * @return
	 */
	private boolean extractZipMain(InputStream in, String targetDirectory, boolean isSame)
	{
		FileConnection dstFileConnection = null;
		OutputStream  os = null;
        int targetSize = openZipFile(in, isSame);
		if (targetSize < 0)
		{
			return (false);
		}
        sceneSelector.updateProgressForZipExtract(); // �f�[�^�̍X�V

		String targetFileName = extractTargetFileName;
		int compressedSize = compressedFileDataSize;
		if (compressedSize == 0)
		{
			if (isSame == false)
			{
			    // �W�J����t�@�C���ł͂Ȃ�����(��̓I�ɂ̓f�B���N�g��������)...�f�B���N�g�����@���ďI������
			    kaniFileOperations.MakeDirectory(targetDirectory + targetFileName);
			}
			return (true);
		}

		
		// �W�J����\��̃f�B���N�g�������邩�ǂ����`�F�b�N����
		String extractDirectory = targetFileName;
		if (isSame == false)
		{
			int pos = extractDirectory.lastIndexOf('/');
			if (pos > 0)
			{
				String makeDir = extractDirectory.substring(0, pos);
				if (kaniFileOperations.IsExistFile(targetDirectory + makeDir) == false)
				{
					// �f�B���N�g�������݂��Ȃ��ꍇ�ɂ́A�Y������f�B���N�g�����@��
					kaniFileOperations.MakeDirectory(targetDirectory + makeDir);
				}
			}
		}
		
		try
        {
            //  �W�J�t�@�C�����J��
			dstFileConnection = (FileConnection) Connector.open(targetDirectory + targetFileName, Connector.READ_WRITE);
    		if (dstFileConnection.exists() == true)
    		{
    			// �t�@�C�������݂����A�A�A�폜���č�蒼��
    			dstFileConnection.delete();
    		}    		

    		dstFileConnection.create();
    		os = dstFileConnection.openOutputStream();
    		if (os != null)
    		{
    			if (targetSize != compressedSize)
    			{
                    // �f�[�^�͈��k����Ă����̂�
    				// �W�J�t�@�C���ɂ�����o�͂���...
        			ZOutputStream zOut = new ZOutputStream(os, MAX_WBITS, true, false);
        			try
        			{
                        for (int i = 0, loop = 0; i < compressedSize; i++)
                        {
                        	int data = in.read();
                        	currentReadSize++;
                        	loop++;
                        	zOut.write(data);
                        	if (loop > 1000)
                        	{
                        		 // �f�[�^�W�J�̐i����
                                sceneSelector.updateProgressForZipExtract();
                                loop = 0;
                        	}
                        }
        			}
        			catch (Exception e)
        			{
        				// �W�J�̗�O�����A�������Ȃ�...
        			}
    		        zOut.finish();
    		        zOut.end();
    		        zOut.close();    				
    			}
    			else
    			{
        	        // �f�[�^���񈳏k�Ŋi�[����Ă���I
    				try
    				{
        				for (int i = 0, loop = 0; i < compressedSize; i++)
        				{
                        	int data = in.read();
                        	currentReadSize++;
                        	loop++;
                        	os.write(data);
                        	if (loop > 1000)
                        	{
                        		 // �f�[�^�W�J�̐i����
                                sceneSelector.updateProgressForZipExtract();
                                loop = 0;
                        	}   					
        				}
    				}
        			catch (Exception e)
        			{
        				// ��O�����A�ł��������Ȃ�...
        			}
    			}
    		}
    		os.close();
    		dstFileConnection.close();
    		dstFileConnection = null;
        }
        catch (Exception ex)
        {
        	// ��O�����I
        	if (os != null)
        	{
        		try
        		{
                	os.close();
        		}
        		catch (Exception e)
        		{
        			// 
        		}
        	}
        	return (true);
        }
		return (true);
	}

	/**
	 *  ZIP�t�@�C�����J��
	 * @param is
	 * @return
	 */
	private int openZipFile(InputStream is, boolean isSame)
	{
		int extractSize = -1;
		try
		{
            byte[] buffer = new byte[500];
            while (true)
            {
                int size = is.read(buffer, 0, 2);
                currentReadSize = currentReadSize + 2;
                if (size <= 0)
                {
                	// �ǂݍ��ݎ��s...�I������
        			extractSize = -1;
        			compressedFileDataSize = -1;
                    extractTargetFileName = null;
                    return (extractSize);
                }
            	// �f�[�^(Local File Header)�̐擪���m�F... "PK.." ��ZIP�`�����ǂ���...
            	if ((buffer[0] != (byte) 0x50)||(buffer[1] != (byte) 0x4b))
                {
            		// �f�[�^�̐擪�ł͂Ȃ������̂�...�J��Ԃ�
            		continue;
                }
                size = is.read(buffer, 0, 2);
                currentReadSize = currentReadSize + 2;
                if (size <= 0)
                {
                	// �ǂݍ��ݎ��s...�I������
        			extractSize = -1;
        			compressedFileDataSize = -1;
                    extractTargetFileName = null;
                    return (extractSize);
                }
                if ((buffer[0] != (byte) 0x03)||(buffer[1] != (byte) 0x04))
                {
                	// �f�[�^�u���b�N�ł͂Ȃ������̂�...
                	continue;
                }

                // �Ƃ肠�����A�����炠����̃f�[�^��ǂݔ�΂�...
                size = is.read(buffer, 0, 2); // version needed to extract
                size = is.read(buffer, 0, 2); // general purpose bit flag (0x0000)
                size = is.read(buffer, 0, 2); // compression method (0x0008)
                size = is.read(buffer, 0, 2); // last mod file time
                size = is.read(buffer, 0, 2); // last mod file date
                size = is.read(buffer, 0, 4); // crc-32
                currentReadSize = currentReadSize + 14;
                
                // compressed size                
                compressedFileDataSize = 0;
                size = is.read(buffer, 0, 4); // compressed size
                currentReadSize = currentReadSize + 4;
                compressedFileDataSize = compressedFileDataSize + ((buffer[0] & 0xff) << 0);
                compressedFileDataSize = compressedFileDataSize + ((buffer[1] & 0xff) << 8);
                compressedFileDataSize = compressedFileDataSize + ((buffer[2] & 0xff) << 16);
                compressedFileDataSize = compressedFileDataSize + ((buffer[3] & 0xff) << 24);

                // uncompressed size
                extractSize = 0;
                size = is.read(buffer, 0, 4); // uncomressed size
                currentReadSize = currentReadSize + 4;
                extractSize = extractSize + ((buffer[0] & 0xff) << 0);
                extractSize = extractSize + ((buffer[1] & 0xff) << 8);
                extractSize = extractSize + ((buffer[2] & 0xff) << 16);
                extractSize = extractSize + ((buffer[3] & 0xff) << 24);

                // File name length
                int fileNameLength = 0;
                size = is.read(buffer, 0, 2); // file name length
                currentReadSize = currentReadSize + 2;
                fileNameLength = buffer[0];
                fileNameLength = fileNameLength + (int) buffer[1] * 0x100;

                // Extra field length
                int extraFieldLength = 0;
                size = is.read(buffer, 0, 2); // extra field length
                currentReadSize = currentReadSize + 2;
                extraFieldLength = buffer[0];
                extraFieldLength = extraFieldLength + (int) buffer[1] * 0x100;

                // �t�@�C����(���f�[�^)��ǂݍ���
                size = is.read(buffer, 0, fileNameLength);
                currentReadSize = currentReadSize + size;
                extractTargetFileName = null;
                extractTargetFileName = new String(buffer, 0, size);
                if (isSame == true)
                {
                	// �f�B���N�g���W�J�����Ȃ��ꍇ...
            		int pos = extractTargetFileName.lastIndexOf('/');
            		if (pos > 0)
            		{
            			String fileName = extractTargetFileName.substring(pos + 1);
            			extractTargetFileName = fileName;
            		}
                }

                if (extraFieldLength > 0)
                {
                	// extra field �����݂���ꍇ�ɂ́A���̕�����ǂݔ�΂�
                	size = is.read(buffer, 0, extraFieldLength);
                    currentReadSize = currentReadSize + size;
                }
                
                break; // ���܂������΁A���Ŕ�����
            }
            buffer = null;
         }
		catch (Exception ex)
		{
			extractSize = -1;
			compressedFileDataSize = -1;
            extractTargetFileName = null;
		}
		return (extractSize);
	}

}
