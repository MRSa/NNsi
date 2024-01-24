import javax.microedition.lcdui.Font;
import javax.microedition.midlet.MIDlet;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;;

/**
 *  �f�[�^�X�g���[�W�N���X
 * 
 * @author MRSa
 */
public class DotEDataStorage implements IMidpDataStorage
{
    // �A�v���P�[�V�������ƃX�v���b�V���X�N���[��
    static final private String MyMidpName = "dotE:�h�b�g�G�y�C���^";
    static final private String MyMidpSplashImageName = "/res/splash.png";
    static final private int    MySize = 32;
    static final public  int    ASCEND = 1;    // ����
    static final public  int    DESCEND = -1;  // �~��
    static final public  int    OTHER = 0;     // ���̑�

    // �f�[�^�X�g���[�W���ƃo�[�W����
    private final String DATASTORAGE_NAME     = "dotE";
    private final int    DATASTORAGE_VERSION  = 101;

    /***** �f�[�^�x�[�X���R�[�h�ԍ�(��������) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DB�̃o�[�W�����i�[���R�[�h
    private final int    BASEDIR_RECORDID     = 2;    // preference DB�̃x�[�X�f�B���N�g���i�[���R�[�h
    private final int    DATAFILE_RECORDID    = 3;    // preference DB�̃f�[�^�t�@�C�����i�[���R�[�h
    private final int    PREFERENCE_RECORDID  = 4;    // preference DB�̃I�v�V�����f�[�^�i�[���R�[�h
    /***** �f�[�^�x�[�X���R�[�h�ԍ�(�����܂�) *****/

    /***** �ۊǃf�[�^(��������) *****/
    private String       baseDirectory   = "file:///";     // �x�[�X�f�B���N�g�������L�^����
    private String       dataFileName    = "default.bmp";  // �t�@�C�����̂��L�^����
    private int          paintColor     = 0x00000000;     // �L�^����F
    private int          clearColor     = 0x00ffffff;     // �N���A����F
    private int          preference     = 0;              // �I�v�V�����f�[�^
    private int          fontSize       = Font.SIZE_SMALL;
    /***** �ۊǃf�[�^(�����܂�) *****/

    /***** ���R�[�h�A�N�Z�T *****/
    private MidpRecordAccessor dbAccessor = null;

    /*** ���̑��f�[�^ ***/
    private String informationMessage = "";
    private int        sizeX = MySize;
    private int        sizeY = MySize;
    private int[][]    dotEPointData = null;

    private int        copyAreaX = 0;
    private int        copyAreaY = 0;
    private int[][]    copyPasteArea = null;

    /**
     *   �R���X�g���N�^
     * 
     * @param ���R�[�h�A�N�Z�T
     */
    public DotEDataStorage(MidpRecordAccessor accessor)
    {
        dbAccessor = accessor;
        dotEPointData = new int[sizeX][sizeY];
    }

    /**
     *  �A�v���P�[�V����������������
     * 
     */
    public String getApplicationName()
    {
        return (MyMidpName);
    }

    /**
     * �X�v���b�V���̃C���[�W������������
     * 
     */
    public String getSplashImageName()
    {
        return (MyMidpSplashImageName);
    }

    /**
     *  �N���X�̎��s����
     *
     */
    public void prepare(MIDlet object)
    {
        // �����J���[�f�[�^��ݒ肷��
    	clearAllPointData();
    }

    /**
     *  ��ʂ̏���
     *
     */
    public void prepareScreen(MidpSceneSelector selector)
    {
        //
    }

    /**
     *  �A�v���P�[�V���������I���^�C�}�̒l����������
     * 
     */
    public long getTimeoutCount()
    {
        return (-1);
    }

    /**
     *  �X�g���[�W�̈�Ƀf�[�^��ޔ�������
     *
     */
    public void backup()
    {
        // �o�b�N�A�b�v����f�[�^���Ȃ���΁A���������I������
        if (baseDirectory == null)
        {
            return;
        }
        
        // DB���I�[�v��������
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        
        // �o�[�W�����ԍ��̋L�^
        dbAccessor.startCacheIntArray(1);
        dbAccessor.cacheRecordIntArray(0, DATASTORAGE_VERSION);
        dbAccessor.writeRecordIntArray(VERSION_RECORDID);

        // �x�[�X�f�B���N�g���̋L�^
        dbAccessor.writeRecord(BASEDIR_RECORDID, baseDirectory.getBytes(), 0, (baseDirectory.getBytes()).length);

        // �f�[�^�t�@�C�����̋L�^
        dbAccessor.writeRecord(DATAFILE_RECORDID, dataFileName.getBytes(), 0, (dataFileName.getBytes()).length);

        // �ݒ�f�[�^�A�y�C���g�F�̋L�^
        dbAccessor.startCacheIntArray(3);
        dbAccessor.cacheRecordIntArray(0, paintColor);
        dbAccessor.cacheRecordIntArray(1, clearColor);
        dbAccessor.cacheRecordIntArray(2, preference);
        dbAccessor.writeRecordIntArray(PREFERENCE_RECORDID);

        // DB���N���[�Y����
        dbAccessor.closeDatabase();
        return;
    }

    /**
     *  �X�g���[�W�̈悩��f�[�^�𕜋�������
     *
     */
    public void restore()
    {
        int versionNumber = 0;
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        if (dbAccessor.startReadRecordIntArray(VERSION_RECORDID) > 0)
        {
            versionNumber = dbAccessor.readRecordIntArray(0);
        }
        if (versionNumber == DATASTORAGE_VERSION)
        {
            // �o�[�W������v�A���R�[�h��ǂݍ���
            byte[] dir = dbAccessor.readRecord(BASEDIR_RECORDID);
            if (dir != null)
            {
                baseDirectory = new String(dir);
            }
            byte[] fileName = dbAccessor.readRecord(DATAFILE_RECORDID);
            if (fileName != null)
            {
            	dataFileName = new String(fileName);
            }
            if (dbAccessor.startReadRecordIntArray(PREFERENCE_RECORDID) > 0)
            {
            	paintColor = dbAccessor.readRecordIntArray(0);
                clearColor = dbAccessor.readRecordIntArray(1);
                preference = dbAccessor.readRecordIntArray(2);
            }
        }
        dbAccessor.closeDatabase();
        return;
    }

    /**
     *  �I�u�W�F�N�g���N���A����
     *
     */
    public void cleanup()
    {
        System.gc();
        return;
    }

    /**
     *  �t�H���g�T�C�Y���擾����
     * 
     * @return �t�H���g�T�C�Y
     */
    public int getFontSize()
    {
        return (fontSize);
    }

    /**
     *  �x�[�X�f�B���N�g�������擾����
     * 
     * @return �x�[�X�f�B���N�g����
     */
    public String getBaseDirectory()
    {
        return (baseDirectory);
    }

    /**
     *  �f�[�^�t�@�C�������擾����
     * 
     * @return �f�[�^�t�@�C����
     */
    public String getDataFileName()
    {
        return (dataFileName);
    }

    /**
     *   ��񃁃b�Z�[�W����������
     * @return ��񃁃b�Z�[�W
     */
    public String getInformationMessage()
    {
        return (informationMessage);
    }

    /**
     *  �t�H���g�T�C�Y���L�^(�ݒ�)����
     * 
     * @param size �t�H���g�T�C�Y
     */
    public void setFontSize(int size)
    {
        fontSize = size;
    }

    /**
     *   �x�[�X�f�B���N�g������ݒ肷��
     * 
     * @param directory �x�[�X�f�B���N�g����
     */
    public void setBaseDirectory(String directory)
    {
        baseDirectory = directory;
    }

    /**
     *   �t�@�C������ݒ肷��
     * 
     * @param fileName �ݒ肷��t�@�C����
     */
    public void setDataFileName(String fileName)
    {
    	dataFileName = fileName;
    }

    /**
     *  ���C�ɓ���ݒ�f�[�^��ݒ肷��
     * @param preferenceData
     */
    public void setPreference(int preferenceData)
    {
        preference = preferenceData;
    }

    /**
     *  ���C�ɓ���ݒ�f�[�^���擾����
     *
     */
    public int getPreference()
    {
        return (preference);
    }

    /**
     *  �h��F��ݒ肷��
     * @param color  �F
     */
    public void setPaintColor(int color)
    {
    	paintColor = color;
    }

    /**
     *  �h��F����������
     * @return �F
     */
    public int getPaintColor()
    {
        return (paintColor);
    }
    

    /**
     *  ��񃁃b�Z�[�W�̐ݒ�
     * @param message ��񃁃b�Z�[�W
     */
    public void setInformationMessage(String message)
    {
        informationMessage = message;
    }
    
    /**
     *  �����T�C�Y����������
     * @return �����T�C�Y
     */
    public int getSizeX()
    {
        return (sizeX);
    }

    /**
     *  �c���T�C�Y����������
     * @return
     */
    public int getSizeY()
    {
    	return (sizeY);
    }

    /**
     *   �f�[�^���擾����
     * @param x  X���W
     * @param y  Y���W
     * @return   ���W�ɂ�����J���[���
     */
    public int getData(int x, int y)
    {
        if ((x >= sizeX)||(y >= sizeY))
        {
        	return (0x00000000);
        }
    	return (dotEPointData[x][y]);
    }

    /**
     *   �J���[�f�[�^��ݒ肷��
     * @param x x���W
     * @param y y���W
     */
    public void setData(int x, int y)
    {
        if ((x >= sizeX)||(y >= sizeY))
        {
        	// �T�C�Y�I�[�o�[�A�������Ȃ�
        	return;
        }
    	dotEPointData[x][y] = paintColor;
    }
    
    /**
     *   �J���[�f�[�^��ݒ肷��
     * @param x x���W
     * @param y y���W
     * @param data �J���[�f�[�^
     */
    public void setData(int x, int y, int data)
    {
        if ((x >= sizeX)||(y >= sizeY))
        {
        	// �T�C�Y�I�[�o�[�A�������Ȃ�
        	return;
        }
    	dotEPointData[x][y] = data;
    }

    /**
     *  �J���[�f�[�^���N���A����
     * @param x x���W
     * @param y y���W
     */
    public void clearData(int x, int y)
    {
        if ((x >= sizeX)||(y >= sizeY))
        {
        	// �T�C�Y�I�[�o�[�A�������Ȃ�
        	return;
        }
    	dotEPointData[x][y] = clearColor;
    }

    /**
     *  �J���[�f�[�^�����ׂăN���A����
     */
    public void clearAllPointData()
    {
    	for (int x = 0; x < sizeX; x++)
    	{
    		for (int y = 0; y < sizeY; y++)
    		{
    			dotEPointData[x][y] = clearColor;
    		}
    	}
    	return;
    }
    
    /**
     *  �F��ύX����
     * @param changeOrder
     */
    public void changeColor(int changeOrder)
    {
        //  paintColor
    	int red = (paintColor & 0x00ff0000) >> 16;
    	int green = (paintColor & 0x0000ff00) >> 8;
    	int blue = (paintColor & 0x000000ff);
    	
    	switch (changeOrder)
    	{
    	  case ASCEND:
    		// blue
      		blue = (blue + 0x20) & 0x000000f0;
      		if (blue != 0)
      		{
      			blue = blue + 0x0f;
      		}
      		break;

    	  case DESCEND:
    		// red
      		red = (red + 0x20) & 0x000000f0;
      		if (red != 0)
      		{
      			red = red + 0x0f;
      		}
    		break;

    	  case OTHER:
    	  default:
    		// green
    		green = (green + 0x20) & 0x000000f0;
    		if (green != 0)
      		{
      			green = green + 0x0f;
      		}
      		break;
    	}
    	paintColor = blue + (green << 8) + (red << 16);
    	return;
    }

    /**
     *   �G���A���N���A����
     */
    public void clearCopyAreaData()
    {
    	copyPasteArea = null;
    	copyAreaX = 0;
    	copyAreaY = 0;
    	System.gc();
    }
    
    /**
     *   �̈���R�s�[����
     * @param topX    ����(X��)
     * @param topY    �E��(Y��)
     * @param width   �R�s�[��
     * @param height  �R�s�[����
     */
    public void copyAreaData(int topX, int topY, int width, int height)
    {
        int areaX = 0;
        int areaY = 0;
    	
    	clearCopyAreaData();

    	copyAreaX = width;
    	copyAreaY = height;
        copyPasteArea = new int[copyAreaX][copyAreaY];
    	for (int x = topX; x < (topX + width); x++)
    	{
    		for (int y = topY; y < (topY + height); y++)
    		{
                copyPasteArea[areaX][areaY] = dotEPointData[x][y];
                areaY++;
    		}
    		areaX++;
    		areaY = 0;
    	}
    	return;
    }

    /**
     *  �̈���y�[�X�g����
     * @param startX  ������W(X��)
     * @param startY  ������W(Y��)
     */
    public void pasteAreaData(int startX, int startY)
    {
    	int areaX = 0;
    	int areaY = 0;
    	for (int x = startX; x < sizeX; x++)
    	{
    		for (int y = startY; y < sizeY; y++)
    		{
    			try
    			{
        			dotEPointData[x][y] = copyPasteArea[areaX][areaY];
                    areaY++;
                    if (areaY >= copyAreaY)
                    {
                    	break;
                    }    				
    			}
    			catch (Exception ex)
    			{
    				return;
    			}
    		}
    		areaX++;
    		if (areaX >= copyAreaX)
    		{
    			return;
    		}
    		areaY = 0;
    	}
    	return;
    }
}
