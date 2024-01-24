import javax.microedition.lcdui.Font;
import javax.microedition.midlet.MIDlet;
import jp.sourceforge.nnsi.a2b.framework.core.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;;

/**
 *  �f�[�^�X�g���[�W�N���X
 * 
 * @author MRSa
 */
public class HiMemoDataStorage implements IMidpDataStorage
{
    // �A�v���P�[�V�������ƃX�v���b�V���X�N���[��
    static final private String MyMidpName = "�ǋL����";
    static final private String MyMidpSplashImageName = "/res/splash.png";

    // �f�[�^�X�g���[�W���ƃo�[�W����
    private final String DATASTORAGE_NAME     = "hiMEMO";
    private final int    DATASTORAGE_VERSION  = 100;

    /***** �f�[�^�x�[�X���R�[�h�ԍ�(��������) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DB�̃o�[�W�����i�[���R�[�h
    private final int    BASEDIR_RECORDID     = 2;    // preference DB�̃x�[�X�f�B���N�g���i�[���R�[�h
    /***** �f�[�^�x�[�X���R�[�h�ԍ�(�����܂�) *****/

    /***** �ۊǃf�[�^(��������) *****/
    private String       baseDirectory = null; // �x�[�X�f�B���N�g�������L�^����
    private int          fontSize     = Font.SIZE_MEDIUM;
    /***** �ۊǃf�[�^(�����܂�) *****/

    /***** ���R�[�h�A�N�Z�T *****/
    private MidpRecordAccessor dbAccessor = null;

    /**
     *   �R���X�g���N�^
     * 
     * @param ���R�[�h�A�N�Z�T
     */
    public HiMemoDataStorage(MidpRecordAccessor accessor)
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
}
