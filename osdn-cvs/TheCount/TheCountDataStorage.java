import javax.microedition.lcdui.Font;
import javax.microedition.midlet.MIDlet;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;;

/**
 *  �f�[�^�X�g���[�W�N���X
 * 
 * @author MRSa
 */
public class TheCountDataStorage implements IMidpDataStorage
{
    // �A�v���P�[�V�������ƃX�v���b�V���X�N���[��
    static final private String MyMidpName = "TheCount";
    static final private String MyMidpSplashImageName = "/res/splash.png";

    // �f�[�^�X�g���[�W���ƃo�[�W����
    private final String DATASTORAGE_NAME     = "TheCount";
    private final int    DATASTORAGE_VERSION  = 100;

    /***** �f�[�^�x�[�X���R�[�h�ԍ�(��������) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DB�̃o�[�W�����i�[���R�[�h
    private final int    BASEDIR_RECORDID     = 2;    // preference DB�̃x�[�X�f�B���N�g���i�[���R�[�h
    private final int    REPORTFILE_RECORDID  = 3;
    private final int    ITEMVALUE_RECORDID   = 4;
    private final int    ITEM1NAME_RECORDID   = 5;
    private final int    ITEM2NAME_RECORDID   = 6;
    private final int    ITEM3NAME_RECORDID   = 7;
    private final int    ITEM4NAME_RECORDID   = 8;
    private final int    ITEM5NAME_RECORDID   = 9;
    private final int    ITEM6NAME_RECORDID   = 10;
    /***** �f�[�^�x�[�X���R�[�h�ԍ�(�����܂�) *****/

    /***** �ۊǃf�[�^(��������) *****/
    private final int   NUMBER_OF_ITEMS = 6;
    private String       baseDirectory  = null; // �x�[�X�f�B���N�g�������L�^����
    private String       reportFileName = "";    // �o�͂��郌�|�[�g�t�@�C�������L�^����
    private String[]     itemName       = null;
    private int[]        itemValue      = null;
    private int          fontSize     = Font.SIZE_SMALL;
    /***** �ۊǃf�[�^(�����܂�) *****/

    /***** ���R�[�h�A�N�Z�T *****/
    private MidpRecordAccessor dbAccessor = null;
	private MidpFileOperations fileUtils  = null;

    private String  informationMessage = "";    // ��񃁃b�Z�[�W

    /**
     *   �R���X�g���N�^
     * 
     * @param ���R�[�h�A�N�Z�T
     */
    public TheCountDataStorage(MidpRecordAccessor accessor, MidpFileOperations fileOperation)
    {
        dbAccessor = accessor;
        fileUtils  = fileOperation;
        
        itemName  = new String[NUMBER_OF_ITEMS];
        itemValue = new int[NUMBER_OF_ITEMS];
        for (int index = 0; index < NUMBER_OF_ITEMS; index++)
        {
        	itemName[index] = "";
        	itemValue[index] = 0;
        }
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
        //
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

        // ���|�[�g�t�@�C�����̋L�^
        dbAccessor.writeRecord(REPORTFILE_RECORDID, reportFileName.getBytes(), 0, (reportFileName.getBytes()).length);

        try
        {
            // �A�C�e���l�̋L�^
            dbAccessor.startCacheIntArray(NUMBER_OF_ITEMS);
            for (int index = 0; index < NUMBER_OF_ITEMS; index++)
            {
                dbAccessor.cacheRecordIntArray(index, itemValue[index]);
            }
            dbAccessor.writeRecordIntArray(ITEMVALUE_RECORDID);

            // �A�C�e�����̋L�^
            dbAccessor.writeRecord(ITEM1NAME_RECORDID, itemName[0].getBytes(), 0, (itemName[0].getBytes()).length);
            dbAccessor.writeRecord(ITEM2NAME_RECORDID, itemName[1].getBytes(), 0, (itemName[1].getBytes()).length);
            dbAccessor.writeRecord(ITEM3NAME_RECORDID, itemName[2].getBytes(), 0, (itemName[2].getBytes()).length);
            dbAccessor.writeRecord(ITEM4NAME_RECORDID, itemName[3].getBytes(), 0, (itemName[3].getBytes()).length);
            dbAccessor.writeRecord(ITEM5NAME_RECORDID, itemName[4].getBytes(), 0, (itemName[4].getBytes()).length);
            dbAccessor.writeRecord(ITEM6NAME_RECORDID, itemName[5].getBytes(), 0, (itemName[5].getBytes()).length);
        }
        catch (Exception e)
        {
        	//
        }

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
            // �o�[�W�����ԍ�����v�A���R�[�h��ǂݍ���
            byte[] dir = dbAccessor.readRecord(BASEDIR_RECORDID);
            if (dir != null)
            {
                baseDirectory = new String(dir);
            }

            // ���|�[�g�t�@�C������ǂݍ���
            byte[] fileName = dbAccessor.readRecord(REPORTFILE_RECORDID);
            if (fileName != null)
            {
            	reportFileName = new String(fileName);
            }

            try
            {
                // �A�C�e���l��W�J
            	int maxIndex = dbAccessor.startReadRecordIntArray(ITEMVALUE_RECORDID);
                if (maxIndex >= 0)
                {
                	if (maxIndex >= NUMBER_OF_ITEMS)
                	{
                		maxIndex = NUMBER_OF_ITEMS;
                	}
                    for (int index = 0; index < maxIndex; index++)
                    {
                    	itemValue[index] = dbAccessor.readRecordIntArray(index);
                    }
                }

                // �A�C�e������W�J (1)
                byte[] itemRecordName = dbAccessor.readRecord(ITEM1NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[0] = new String(itemRecordName);
                }
                itemRecordName = null;

                // �A�C�e������W�J (2)
                itemRecordName = dbAccessor.readRecord(ITEM2NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[1] = new String(itemRecordName);
                }
                itemRecordName = null;

                // �A�C�e������W�J (3)
                itemRecordName = dbAccessor.readRecord(ITEM3NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[2] = new String(itemRecordName);
                }
                itemRecordName = null;

                // �A�C�e������W�J (4)
                itemRecordName = dbAccessor.readRecord(ITEM4NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[3] = new String(itemRecordName);
                }
                itemRecordName = null;

                // �A�C�e������W�J (5)
                itemRecordName = dbAccessor.readRecord(ITEM5NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[4] = new String(itemRecordName);
                }
                itemRecordName = null;

                // �A�C�e������W�J (6)
                itemRecordName = dbAccessor.readRecord(ITEM6NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[5] = new String(itemRecordName);
                }
                itemRecordName = null;
            }
            catch (Exception e)
            {
            	//
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
     *   �A�C�e���̖��O����������
     * @param index  �A�C�e���C���f�b�N�X(0�n�܂�)
     * @return �A�C�e���̖��O(���ݒ�̏ꍇ�̓A�C�e���ԍ�(1�͂��܂�)������)
     */
    public String getItemName(int index)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
                if (itemName[index].length() > 0)
                {
                	return (itemName[index]);
                }
                return ("" + (index + 1));
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return (null);
    }

    /**
     *   �A�C�e���̌��ݒl����������
     * @param  index  �A�C�e���C���f�b�N�X(0�n�܂�)
     * @return �ݒ肷��A�C�e���̌��ݒl (index�ُ�̏ꍇ�� -1)
     */
    public int getItemValue(int index)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
    			return (itemValue[index]);
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return (-1);
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
     *   �A�C�e���̖��O��ݒ肷��
     * @param index  �A�C�e���C���f�b�N�X(0�n�܂�)
     * @param value  �ݒ肷��A�C�e���̖��O
     */
    public void setItemName(int index, String value)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
    			itemName[index] = value;
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return;
    }

    /**
     *   �A�C�e���̒l��ݒ肷��
     * @param index  �A�C�e���C���f�b�N�X(0�n�܂�)
     * @param value  �ݒ肷��A�C�e���̒l
     */
    public void setItemValue(int index, int value)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
    			itemValue[index] = value;
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return;
    }

    /**
     *   �A�C�e���̒l�ɉ��Z����
     * @param index  �A�C�e���C���f�b�N�X(0�n�܂�)
     * @param value  ���Z����l
     */
    public void addItemValue(int index, int value)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
    			itemValue[index] = itemValue[index] + value;
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return;
    }

    /**
     *  �J�E���g�̑������擾����
     * @return
     */
    public int getTotalValueCount()
    {
        int count = 0;
        for (int index = 0; index < NUMBER_OF_ITEMS; index++)
        {
        	count = count + itemValue[index];
        }
        return (count);
    }
    
    /**
     *  �l�̊���(�p�[�Z���g) �𕶎���ŉ�������
     * @param index
     * @return
     */
    public String getValuePercent(int index)
    {
    	int value = getItemValue(index);
    	int totalCount = getTotalValueCount();
    	if (value < 0)
    	{
    		return ("????");
    	}
        if (totalCount == 0)
        {
        	return ("0.00");
        }
        String percent = ((double) value / (double) totalCount) * 100.0 + "";
        int dot = percent.indexOf(".");
        if ((dot > 0)&&((dot + 3) < percent.length()))
        {
            percent = percent.substring(0, dot + 3);
        }
        return (percent);
    }

    /**
     *  ��񃁃b�Z�[�W���擾����
     * @return
     */
    public String getInformationMessage()
    {
        return (informationMessage);
    }
    
    /**
     *   ��񃁃b�Z�[�W��ݒ肷��
     * @param message
     */
    public void setInformationMessage(String message)
    {
    	informationMessage = message;
    }    
    
    /**
     *   CSV�`���̃f�[�^�t�@�C�����o�͂���
     *
     */
    public void outputDataToFile()
    {
        String fileName = getBaseDirectory() + "TheCount.csv";        
        String valueData = "; itemName, value" + "\r\n";
        for (int index = 0; index < NUMBER_OF_ITEMS; index++)
        {
            valueData = valueData + "\"" + getItemName(index) + "\"" + ", " + getItemValue(index) + "\r\n";
        }
        fileUtils.outputDataTextFile(fileName, null, null, valueData, false);
        return;
    }
}
