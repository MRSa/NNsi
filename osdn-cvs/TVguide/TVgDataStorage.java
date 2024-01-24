import javax.microedition.lcdui.Font;
import javax.microedition.midlet.MIDlet;

import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationDefaultStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpDefaultA2BhttpCommunicator;;

/**
 * �f�[�^�A�N�Z�X�N���X�ł��B<br>
 * ���̃N���X�ŃA�v���N�����ɕۊǃf�[�^��ǂݏo������A
 * �A�v���I�����Ƀf�[�^��ۊǂ����肷��悤�ɂ��܂��B
 * 
 * @author MRSa
 *
 */
public class TVgDataStorage implements IMidpDataStorage
{
    protected String     applicationName  = "TV-PGM";
    protected String     splashScreenFile = "/res/splash.png";
    protected MidpCanvas baseCanvas       = null;

    // �f�[�^�X�g���[�W���ƃo�[�W����
    private final String DATASTORAGE_NAME     = "TVguide";
    private final int    DATASTORAGE_VERSION  = 101;

    /***** �f�[�^�x�[�X���R�[�h�ԍ�(��������) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DB�̃o�[�W�����i�[���R�[�h
    private final int    BASEDIR_RECORDID     = 2;    // preference DB�̃x�[�X�f�B���N�g���i�[���R�[�h
    private final int    PREFER_RECORDID      = 3;    // preference DB�̃f�[�^���R�[�h
    private final int    AREANAME_RECORDID    = 4;    // preference DB�̒n�於��
    /***** �f�[�^�x�[�X���R�[�h�ԍ�(�����܂�) *****/

    private MidpRecordAccessor dbAccessor = null;
    private MidpDefaultA2BhttpCommunicator httpComm = null;
    private String baseDirectory = null;
    private String toGetUrl      = "http://tv.so-net.ne.jp/guest/pc/chart.action?&span=24&head=";
    private String areaString    = "����";
    private int areaId           = 13; // ����
    private int stationId        = 0;
    private int stationType      = 0;  // �n��g

    InformationDefaultStorage infoStorage = null;
    MidpResourceFileUtils     resUtils    = null;
    
    /**
     * �x�[�X�L�����o�X�N���X���L�����܂�
     * @param name �A�v���P�[�V�����̖��O
     * @param canvas �x�[�X�L�����o�X�N���X
     */
    public TVgDataStorage(MidpRecordAccessor accessor)
    {
        dbAccessor = accessor;
    }

    /**
     *  �I�u�W�F�N�g�N���X��������
     * 
     * @param canvas �L�����o�X�N���X
     */
    public void setObjects(MidpCanvas canvas)
    {
        baseCanvas = canvas;
        return;
    }
    
    public void setHttpCommunicator(MidpDefaultA2BhttpCommunicator comm)
    {
        httpComm = comm;
    }
    
    /**
     *  �A�v���P�[�V���������擾�擾����
     *  preturn �A�v���P�[�V������
     */
    public String getApplicationName()
    {
        return (applicationName);
    }

    /**
     *  �X�v���b�V���X�N���[���i�C���[�W���j���擾����
     *  @return �X�v���b�V���X�N���[���t�@�C����
     */
    public String getSplashImageName()
    {
        return (splashScreenFile);
    }

    /**
     *  ����(MIDlet)
     *  @param object MIDlet�N���X
     */
    public void prepare(MIDlet object)
    {
        return;
    }

    /**
     *  ���\�[�X���[�e�B���e�B���L������
     * 
     * @param utils
     */
    public void setResourceUtils(MidpResourceFileUtils utils)
    {
        resUtils = utils;
    }
    
    /**
     *  �����i��ʁj
     *  @param selector �V�[���Z���N�^
     *  
     */
    public void prepareScreen(MidpSceneSelector selector)
    {
        return;
    }

    /**
     *  �A�v�������I���^�C���A�E�g�̃J�E���g���擾
     *  @return �A�v�������I���̃^�C���A�E�g�l(�P�ʁFms)�A���̎��ɂ͖����B
     */
    public long getTimeoutCount()
    {
        return (-1);
    }

    /**
     *  �g�p����t�H���g�T�C�Y����������
     * 
     * @return �g�p����t�H���g�T�C�Y
     */
    public int getFontSize()
    {
        return (Font.SIZE_SMALL);
//        return (Font.SIZE_MEDIUM);
    }

    /**
     *   �n��ID���擾����
     * 
     * @return �n��ID
     */
    public int getAreaId()
    {
        return (areaId);
    }
    
    /**
     *   �n�於�̂��擾����
     * @return �n�於��
     */
    public String getAreaName()
    {
        return (areaString);
    }

    /**
     *  �n��ID�Ɩ��̂�ݒ肷��
     * 
     * @param id �n��ID
     * @param areaName �n�於��
     */
    public void setAreaId(int id, String areaName)
    {
        areaId = id;
        areaString = areaName;
        String cookie = "areaId=" + id + "; span=24";
        httpComm.SetCookie(cookie);
    }

    /**
     *  �擾��URL���擾����
     * @return �擾��URL
     */
    public String getUrlToGet()
    {
        return (toGetUrl);
    }

    /**
     *  �擾��URL��ݒ肷��
     * @param url �擾��URL
     */
    public void setUrlToGet(String url)
    {
        toGetUrl = url;
    }

    /**
     *  ���X�g���[�W�N���X���L������
     *  
     * @param storage ���X�g���[�W�N���X
     */
    public void setInformationStorage(InformationDefaultStorage storage)
    {
        infoStorage = storage;
        try
        {
            infoStorage.setInformationMessageToShow(resUtils.getResourceText("/res/welcome.txt"));
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }

    public void setTvStation(int stationNumber)
    {
        stationId = stationNumber;
    }
    
    public int getTvStation()
    {
        return (stationId);
    }

    public void setTvType(int stationNumber)
    {
        stationType = stationNumber;
    }

    public int getTvType()
    {
        return (stationType);
    }

    public String getTvTypeString()
    {
        String name = "";
        if (stationType == 0)
        {
            name = "�n��g";
        }
        else if (stationType == 1)
        {
            name = "UHF/BS";
        }
        else if (stationType == 2)
        {
            name = "BS�f�W�^��";
        }
        return (name);
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
     *   �x�[�X�f�B���N�g������ݒ肷��
     * 
     * @param directory �x�[�X�f�B���N�g����
     */
    public void setBaseDirectory(String directory)
    {
        baseDirectory = directory;
    }

    /**
     *  �f�[�^���o�b�N�A�b�v����
     *  
     */
    public void backup()
    {
        int dummy = 0;

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

        // �f�[�^�̋L�^
        dbAccessor.startCacheIntArray(4);
        dbAccessor.cacheRecordIntArray(0, areaId);
        dbAccessor.cacheRecordIntArray(1, stationId);
        dbAccessor.cacheRecordIntArray(2, stationType);
        dbAccessor.cacheRecordIntArray(3, dummy);
        dbAccessor.writeRecordIntArray(PREFER_RECORDID);

        // �n�於�̂̋L�^
        dbAccessor.writeRecord(AREANAME_RECORDID, areaString.getBytes(), 0, (areaString.getBytes()).length);

        // DB���N���[�Y����
        dbAccessor.closeDatabase();
        return;

    }

    /**
     *  �f�[�^�����X�g�A����
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
            
            int recId = dbAccessor.startReadRecordIntArray(PREFER_RECORDID);
            if (recId > 1)
            {
                areaId = dbAccessor.readRecordIntArray(0);
            }
            if (recId > 2)
            {
                stationId = dbAccessor.readRecordIntArray(1);
            }
            if (recId > 3)
            {
                stationType = dbAccessor.readRecordIntArray(2);
            }
            byte[] areaName = dbAccessor.readRecord(AREANAME_RECORDID);
            if (areaName != null)
            {
                areaString = new String(areaName);
            }
            dir = null;
            areaName = null;
        }
        dbAccessor.closeDatabase();
        return;
    }

    /**
     *  �f�[�^���N���[���A�b�v����
     *  
     */
    public void cleanup()
    {
        return;
    }
}
