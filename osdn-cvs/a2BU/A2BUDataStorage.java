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
public class A2BUDataStorage implements IMidpDataStorage
{
    // �A�v���P�[�V�������ƃX�v���b�V���X�N���[��
    static final private String MyMidpName = "a2BU:a2B Updater";
    static final private String MyMidpSplashImageName = "/res/splash.png";

    // �f�[�^�X�g���[�W���ƃo�[�W����
    private final String DATASTORAGE_NAME     = "a2BU";
    private final int    DATASTORAGE_VERSION  = 101;

    /***** �f�[�^�x�[�X���R�[�h�ԍ�(��������) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DB�̃o�[�W�����i�[���R�[�h
    private final int    BASEDIR_RECORDID     = 2;    // preference DB�̃x�[�X�f�B���N�g���i�[���R�[�h
    private final int    PREFERENCE_RECORDID  = 3;    // prefernece DB�̃I�v�V�����f�[�^�i�[���R�[�h
    /***** �f�[�^�x�[�X���R�[�h�ԍ�(�����܂�) *****/

    /***** �ۊǃf�[�^(��������) *****/
    private String       baseDirectory = null;    // �x�[�X�f�B���N�g�������L�^����
    private int          fontSize     = Font.SIZE_SMALL;
    private int          preference   = 0;        // �I�v�V�����f�[�^
    private int          waitMilliSecond = 2000;  // ��~���鎞��
    /***** �ۊǃf�[�^(�����܂�) *****/

    /***** ���R�[�h�A�N�Z�T *****/
    private MidpRecordAccessor dbAccessor = null;

    /*** ���̑��f�[�^ ***/
    private String informationMessage = "";
    
    
    /**
     *   �R���X�g���N�^
     * 
     * @param ���R�[�h�A�N�Z�T
     */
    public A2BUDataStorage(MidpRecordAccessor accessor)
    {
        dbAccessor = accessor;
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

        // ���C�ɓ���ݒ�f�[�^�A����҂����Ԃ̋L�^
        dbAccessor.startCacheIntArray(2);
        dbAccessor.cacheRecordIntArray(0, preference);
        dbAccessor.cacheRecordIntArray(1, waitMilliSecond);
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
            if (dbAccessor.startReadRecordIntArray(PREFERENCE_RECORDID) > 0)
            {
                preference = dbAccessor.readRecordIntArray(0);
                waitMilliSecond = dbAccessor.readRecordIntArray(1);
                if (waitMilliSecond <= 0)
                {
                	waitMilliSecond = 2000;
                }
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
     *  ����҂����Ԃ�ݒ肷��
     * @param milliSeconds   ����҂�����
     */
    public void setWaitTime(int milliSeconds)
    {
        waitMilliSecond = milliSeconds;
    }

    /**
     *  ����҂����Ԃ��擾����
     *
     */
    public int getWaitTime()
    {
        return (waitMilliSecond);
    }

    /**
     *  ��񃁃b�Z�[�W�̐ݒ�
     * @param message ��񃁃b�Z�[�W
     */
    public void setInformationMessage(String message)
    {
        informationMessage = message;
    }
}
