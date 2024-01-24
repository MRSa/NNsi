import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;

/**
 *   Bitmap�t�@�C���̓ǂݏo���E�������݃��W�b�N
 *   
 *   
 * 
 * @author MRSa
 *
 */
public class DotEFileAccessor
{
	private MidpFileOperations fileAccessor = null;
    private DotEDataStorage  dataStorage = null;

    private String            dataFileName = null;	
    
    // �b��d�l... 32x32�� 24bitClor BITMAP�̃w�b�_�B(�ʓ|�������Ȃ���...)
    private final byte[] bmp32x32Header = { 
  		  0x42, 0x4D, 0x36, 0x0C, 0x00, 
  		  0x00, 0x00, 0x00, 0x00, 0x00, 
  		  0x36, 0x00, 0x00, 0x00, 0x28, 
  		  0x00, 0x00, 0x00, 0x20, 0x00, 
  		  0x00, 0x00, 0x20, 0x00, 0x00, 
  		  0x00, 0x01, 0x00, 0x18, 0x00, 
  		  0x00, 0x00, 0x00, 0x00, 0x00, 
  		  0x0C, 0x00, 0x00, 0x00, 0x00, 
  		  0x00, 0x00, 0x00, 0x00, 0x00, 
  		  0x00, 0x00, 0x00, 0x00, 0x00, 
  		  0x00, 0x00, 0x00, 0x00};

    /**
     *  �R���X�g���N�^
     *  
     */
    public DotEFileAccessor()
    {
    	//
    	fileAccessor = new MidpFileOperations();
    }

    /**
     *  �N���X�̏���
     */
    public boolean prepare(DotEDataStorage dataSt)
    {
    	dataStorage = dataSt;
    	if ((fileAccessor == null)||(dataStorage == null))
    	{
    		return (false);
    	}
    	return (true);
    }
    
    /**
     *  �f�[�^����������
     *  
     *  @param targetFileName �o�̓t�@�C����
     */
    public void saveToFile(String targetFileName)
    {
    	dataFileName = targetFileName;
        Thread thread = new Thread()
        {
            public void run()
            {
                //
            	outputBitmapData();
            }
        };
        
        try
        {
            thread.start();  // ��ʂ̐؂�ւ�����...
            thread.join();   // �I������܂ő҂�...
        }
        catch (Exception ex)
        {
            // �������Ȃ�...
        }

        // ���Ƃ��܂�
        thread = null;
        System.gc();
        return;
    }

    /**
     *   �t�@�C���ɕۑ������r�b�g�}�b�v�f�[�^��ǂݏo��
     *   
     *   @param targetFileName  �ǂݏo���t�@�C����
     */
    public void loadToFile(String targetFileName)
    {
    	dataFileName = targetFileName;
        Thread thread = new Thread()
        {
            public void run()
            {
                //
            	readBitmapData();
            }
        };
        
        try
        {
            thread.start();  // ��ʂ̐؂�ւ�����...
            thread.join();   // �I������܂ő҂�...
        }
        catch (Exception ex)
        {
            // �������Ȃ�...
        }

        // ���Ƃ��܂�
        thread = null;
        System.gc();
        return;
    }

    
    /**
     *  �r�b�g�}�b�v�f�[�^���o�͂���
     * @return �o�͐����E���s
     */
    public boolean outputBitmapData()
    {
    	byte[] buffer = new byte[3126];
    	System.arraycopy(bmp32x32Header, 0, buffer, 0, bmp32x32Header.length);
        int areaIndex = bmp32x32Header.length;
        
        // �ʓ|������...32x32�ɂ��Ă��܂��B(�f�[�^���i�[����)
        for (int y = 31; y >=0 ; y--)
        {
            for (int x = 0; x < 32; x++)
            {
            	int data = dataStorage.getData(x, y);
            	byte red =   (byte) (((data & 0x00ff0000) >> 16) & 0x000000ff);
            	byte green = (byte) (((data & 0x0000ff00) >> 8)  & 0x000000ff);
            	byte blue =  (byte) (data & 0x000000ff);
            	buffer[areaIndex] = blue;
            	areaIndex++;
            	buffer[areaIndex] = green;
            	areaIndex++;
            	buffer[areaIndex] = red;
            	areaIndex++;
            }
        }

        // �t�@�C���ɓf���o��
        return (fileAccessor.outputRawDataToFile(dataFileName, buffer, false));
    }

    /**
     *  �ۑ����Ă���f�[�^��ǂݏo��
     * @return  �ǂݏo�������E���s
     */
    public boolean readBitmapData()
    {
    	try
    	{
    	    byte[] buffer = fileAccessor.ReadFileWhole(dataFileName);
    	    int index = bmp32x32Header.length;

    	    // �ʓ|������...32x32�ɂ��Ă��܂��B(�f�[�^���i�[����)
            for (int y = 31; y >=0 ; y--)
            {
                for (int x = 0; x < 32; x++)
                {
                    byte blue = buffer[index];
                    index++;
                    byte green = buffer[index];
                    index++;
                    byte red = buffer[index];
                    index++;

                    int data = (((red & 0xff)<< 16) | ((green & 0xff) << 8) | (blue & 0xff));
                    dataStorage.setData(x, y, data);
                }
            }
    	}
    	catch (Exception ex)
    	{
    		// �Ȃɂ����Ȃ�
    		return (false);
    	}
    	return (false);
    }
}
