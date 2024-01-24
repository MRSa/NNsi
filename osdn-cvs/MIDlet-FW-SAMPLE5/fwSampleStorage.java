import javax.microedition.midlet.MIDlet;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;

/**
 *  �f�[�^�X�g���[�W�N���X
 * 
 * @author MRSa
 */
public class fwSampleStorage implements IMidpDataStorage
{
    // �A�v���P�[�V�������ƃX�v���b�V���X�N���[��
    static final private String MyMidpName = "�L�b�`���^�C�}�[";
    static final private String MyMidpSplashImageName = "/res/splash.png";

    // �f�[�^�X�g���[�W���ƃo�[�W����
    private final String DATASTORAGE_NAME     = "fwSample";

    /***** �ۊǃf�[�^(��������) *****/
    private int         settingTime  = 180;  // �^�C�}�ݒ�l���L�^����
    private String       soundFile    = "/res/sound.mid"; // �T�E���h�t�@�C�������L�^����
    /***** �ۊǃf�[�^(�����܂�) *****/

    /***** �f�[�^�x�[�X���R�[�h�ԍ�(��������) *****/
    private final int    SETTIME_RECORDID     = 1;    // �^�C�}�ݒ�l�̃��R�[�h�ԍ�
    private final int    SNDFILE_RECORDID     = 2;    // �T�E���h�t�@�C�����̃��R�[�h�ԍ�
    /***** �f�[�^�x�[�X���R�[�h�ԍ�(�����܂�) *****/
    
    
    /***** ���R�[�h�A�N�Z�T *****/
    private MidpRecordAccessor dbAccessor = null;

    /**
     *   �R���X�g���N�^
     * 
     * @param ���R�[�h�A�N�Z�T
     */
    public fwSampleStorage(MidpRecordAccessor accessor)
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
        // DB���I�[�v��������
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }

        // �^�C�}�ݒ�b���̋L�^
        dbAccessor.startCacheIntArray(1);
        dbAccessor.cacheRecordIntArray(0, settingTime);
        dbAccessor.writeRecordIntArray(SETTIME_RECORDID);

        // �T�E���h�t�@�C�����̋L�^
        dbAccessor.writeRecord(SNDFILE_RECORDID, soundFile.getBytes(), 0, (soundFile.getBytes()).length);

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
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        if (dbAccessor.startReadRecordIntArray(SETTIME_RECORDID) > 0)
        {
            settingTime = dbAccessor.readRecordIntArray(0);
        }
        byte[] sndFile = dbAccessor.readRecord(SNDFILE_RECORDID);
        if (sndFile != null)
        {
            soundFile = new String(sndFile);
            if (MidpFileOperations.IsExistFile(soundFile) == false)
            {
                // �t�@�C�������݂��Ȃ��ꍇ�ɂ́A�T�E���h�t�@�C���ݒ����������
                soundFile = null;
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
     *  �T�E���h�t�@�C�������擾����
     * 
     * @return �T�E���h�t�@�C����
     */
    public String getSoundFileName()
    {
        return (soundFile);
    }

    /**
     *  �^�C�}�ݒ�l���擾����
     * 
     * @return �^�C�}�ݒ�l
     */
    public int getTimerSetTime()
    {
        return (settingTime);
    }

    /**
     *  �T�E���h�t�@�C������ݒ肷��
     * 
     * @param fileName �T�E���h�t�@�C����
     */
    public void setSoundFileName(String fileName)
    {
        soundFile = fileName;
    }

    /**
     *  �^�C�}�ݒ�l��ݒ肷��
     * 
     * @param time
     */
    public void setTimerSetTime(int time)
    {
        settingTime = time;
    }
}
