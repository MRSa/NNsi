import javax.microedition.midlet.*;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.*;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.Random;
import java.util.Vector;
import java.util.Date;

/**
 * a2B�̃��C���N���X�B(�A�v���P�[�V�����S�̂̃N���X�Ԃ̒������Ƃ��Ă����p����B)
 * 
 * @author MRSa
 *
 */
public class a2BMain extends MIDlet
{
    // HTTP(GET)�ʐM�V�[��
    public final int  SCENE_PARTGET_MESSAGE      = 1;   // �X�������擾�����{�����ꍇ...
    public final int  SCENE_WHOLEGET_MESSAGE     = 2;   // �X���S���擾�����{�����ꍇ...
    public final int  SCENE_BBSTABLE_MESSAGE     = 3;   // �ꗗ�S���擾�����{�����ꍇ...
    public final int  SCENE_GETSUBJECTLIST       = 4;   // �X���ꗗ�S���擾�����{�����ꍇ...
    public final int  SCENE_GETALL_MESSAGE       = 5;   // �X���V���m�F�����{�����ꍇ...
    public final int  SCENE_PREVIEW_MESSAGE      = 6;   // �v���r���[�w���̏ꍇ...
    public final int  SCENE_GETRESERVE_MESSAGE   = 7;   // �X���V���m�F(�擾�\��X��)�����{�����ꍇ...
    public final int  SCENE_PREVIEW_MESSAGE_EUC  = 8;   // �v���r���[�w��(EUC)�̏ꍇ...
    public final int  SCENE_PREVIEW_MESSAGE_SJIS = 9;   // �v���r���[�w��(SJIS)�̏ꍇ...
    public final int  SCENE_GET_OSUSUME_2CH      = 10;  // �������߂Q�����˂��\������ꍇ...
    public final int  SCENE_GET_FIND_2CH         = 11;  // 2�����˂錟���ŃX���^�C�g�����擾����ꍇ...
    public final int  SCENE_GET_RELATE_KEYWORD   = 12;  // �֘A�L�[���[�h����...
    
    // HTTP�ʐM�V�[��
    public final int  SCENE_MESSAGE_WRITE    = 101; // ���X�������݂����{�����ꍇ...
    public final int  SCENE_TITLE_SEARCH     = 201; // �X���^�C�������[�h
    public final int  SCENE_MESSAGE_SEARCH   = 202; // �X�����������[�h
    public final int  SCENE_NGWORD_INPUT     = 221; // NG���[�h�o�^���[�h
    public final int  SCENE_MEMOFILE_INPUT   = 222; // �����t�@�C�������̓��[�h
    public final int  SCENE_THREADNUM_INPUT  = 250; // �X���b�h�ԍ����ړ��̓��[�h
    public final int  SCENE_PROXY_SETTING    = 300; // a2B�ݒ�(Proxy�ݒ�)���[�h
    public final int  SCENE_RENAMEFILE_INPUT = 310; // �t�@�C�����ύX���̓��[�h
    public final int  SCENE_COPYFILE_INPUT   = 320; // �R�s�[�t�@�C�������̓��[�h
    public final int  SCENE_GETHTTP_INPUT    = 330; // http��MURL���̓��[�h
    
    private final int FILE_BUFFER_SIZE  = 5120;  // �o�b�t�@�T�C�Y
    private final int MARGIN            = 10;    // �o�b�t�@�̃}�[�W��
    static public final int TEMP_READ_SIZE    = 32768; // �t�@�C���ǂݏo���T�C�Y
    static public final int TEMP_PREVIEW_SIZE = 16384; // �v���r���[�f�[�^�T�C�Y

    // a2B�ғ����
    private final byte a2B_EXECUTE_UNDEFINED     = (byte) 0;  // �N���O
    private final byte a2B_EXECUTE_PREPARE       = (byte) 1;  // ������ (recordStore�ǂݏo����)
    private final byte a2B_EXECUTE_DIRSETTING    = (byte) 2;  // �f�[�^�i�[�f�B���N�g���ݒ蒆
    private final byte a2B_EXECUTE_STARTED       = (byte) 3;  // a2B�N������
    private final byte a2B_EXECUTE_SHOWLIST      = (byte) 4;  // �X���ꗗ�\����
    private final byte a2B_EXECUTE_VIEW_READONLY = (byte) 5;  // �t�@�C���[����X���Q�ƒ� (�Q�ƃ��[�h)
    private final byte a2B_EXECUTE_VIEW_MANAGED  = (byte) 6;  // (�ʏ��)�X���Q�ƒ� (�Ǘ����[�h)    

    // PROXY�g�p�ݒ�
    public final int PROXY_NOTUSE           = 0;   // ���p���Ȃ�
    public final int PROXY_USE_ALL          = 1;   // ���p����
    public final int PROXY_USE_ONLYGET      = 2;   // GET���̂ݗ��p����
    public final int PROXY_USE_ONLYRANGE    = 3;   // �����擾�̂ݗ��p����
    public final int PROXY_WORKAROUND_WX310 = 4;   // WX310�����̉������s��

    public final int SCENE_LISTFORM         = 0;
    public final int SCENE_VIEWFORM         = 1;

    public final int FEATURE_CHANGE_FONT    = 0;   // �t�H���g�ύX
    public final int FEATURE_NUMBER_MOVE    = 2;   // �����L�[�ړ����[�h
    public final int FEATURE_PICKUP_NOTREAD = 1;   // ���ǃX�����o
    
    public static final int a2B_HTTPMODE_NOUSECOOKIE = -1;
    public static final int a2B_HTTPMODE_PICKUPCOOKIE = 0;
    public static final int a2B_HTTPMODE_USEPREVIOUSCOOKIE = 1;

    // �_�C�A���O�\�����x��
    public final byte SHOW_DEBUG     = (byte) 100;   // �f�o�b�O���[�h
    public final byte SHOW_EXCEPTION = (byte)  70;   // ��O�������[�h
    public final byte SHOW_DEFAULT   = (byte)  50;   // �_�C�A���O�\���͈� (���̒l���傫���ꍇ�ɂ̓_�C�A���O��\��)
    public final byte SHOW_INFO      = (byte)  30;   // ��񃂁[�h
    public final byte SHOW_WARN      = (byte)  20;   // �x�����[�h
    public final byte SHOW_CONFIRM   = (byte)  10;   // �m�F���[�h
    public final byte SHOW_ERROR     = (byte)   1;   // �G���[���[�h

    // �J�e�S���I�����[�h
    public final byte SHOW_CATEGORYLIST           = (byte) 10;
    public final byte SHOW_BBSLIST                = (byte) 20;
    public final byte SHOW_SUBJECTCOMMAND         = (byte) 30;
    public final byte SHOW_SUBJECTCOMMANDFAVORITE = (byte) 40;
    public final byte SHOW_SUBJECTCOMMANDSECOND   = (byte) 44;
    public final byte SHOW_LOGINMENU              = (byte) 46;
    public final byte SHOW_VIEWCOMMANDSECOND      = (byte) 49;
    public final byte CONFIRM_DELETELOGDAT        = (byte) 50;
    public final byte CONFIRM_DELETELOGFILE       = (byte) 60;
    public final byte CONFIRM_URL_FOR_BROWSER     = (byte) 120;
    public final byte CONFIRM_GET_THREADNUMBER    = (byte) 140;
    public final byte CONFIRM_SET_THREADNUMBER    = (byte) 145;
    public final byte CONFIRM_SELECT_OSUSUME2CH   = (byte) 160;
    public final byte CONFIRM_SELECT_FIND2CH      = (byte) 165;
    public final byte CONFIRM_SELECT_RELATE_KEYWORD  = (byte) 166;

    // �A�N�V����
    public final int  ACTION_NOTHING_CANCEL       = -1;
    public final int  ACTION_NOTHING              = 0;
    public final int  ACTION_THREAD_OPERATION     = 1;
    public final int  ACTION_THREAD_RESERVE       = 2;
    public final int  ACTION_THREAD_GETLOG        = 3;
    
    // �N���X���ϐ��̒�`
    private a2BobjectFactory  objectFactory      = null;    // �I�u�W�F�N�g�Ǘ��N���X
    private Displayable       currentForm        = null;    // ���ݕ\�����t�H�[��
    private Displayable       previousForm       = null;    // �O��\�����̃t�H�[��

    private boolean          isReserveSequence  = false;
    private boolean          isGetNewArrival    = false;   // �V���m�F���{��...
    private boolean          isShowDialog       = false;   // �_�C�A���O�\����...    
    private byte             a2BexecutingStatus  = a2B_EXECUTE_UNDEFINED;  // a2B�ғ����    
    private byte[]          scratchBuffer        = null;    // �ꎞ�I�Ƀf�[�^���i�[����o�b�t�@(byte[]�^)
    private String           parseTargetString    = null;    //  �ꎞ�I�Ƀf�[�^���i�[����o�b�t�@(String�^)
    private String           be2chCookie          = null;    //  �ꎞ�I��Cookie�����i�[����o�b�t�@
    private String           previousHttpCookie   = null;    //  �ꎞ�I��Cookie�����i�[����o�b�t�@
    public  long            watchDogMarking       = -1;       //  �E�H�b�`�h�b�O�̃}�[�L���O

    /**
     *   �R���X�g���N�^...
     * 
     */
    public a2BMain()
    {
        // �I�u�W�F�N�g�𐶐����Ă���
        objectFactory = new a2BobjectFactory(this);
    }

    /**
     *   �A�v���P�[�V�����N�����ɌĂяo����鏈��
     * 
     */
    protected void startApp() throws MIDletStateChangeException
    {
        // �ċN�����ꂽ�Ƃ��ɂ́A�������Ȃ�
        if (a2BexecutingStatus != a2B_EXECUTE_UNDEFINED)
        {
            return;
        }

        // a2B�ғ���Ԃ��������֕ύX����
        a2BexecutingStatus = a2B_EXECUTE_PREPARE;

        // �X�v���b�V���X�N���[����\������...
        a2BSplashScreen startScreen = new a2BSplashScreen();
        Display.getDisplay(this).setCurrent(startScreen);

        // �������s!
        if (objectFactory.prepare() != true)
        {
            // a2B�ғ���Ԃ� STARTED�ɕύX����
            a2BexecutingStatus = a2B_EXECUTE_STARTED;
            
            // �ꗗ���̍\�z
            (objectFactory.bbsCommunicator).prepareBbsDatabase();
        }
        else
        {
            // a2B�ғ���Ԃ� DIRSETTING�ɕύX����
            a2BexecutingStatus = a2B_EXECUTE_DIRSETTING;

            // �f�B���N�g���I�����[�h
            openFileSelector();
        }

        // �E�H�b�`�h�b�O�^�C�}�̋N��
        startWatchDog();

        // �N����ʂ̂��Ƃ��܂�...
        startScreen.finish();
        startScreen = null;
        return;
    }

    /**
     *   �A�v���P�[�V�����ꎞ��~���ɌĂяo����鏈��
     *  
     */
    protected void pauseApp()
    {
        // ���݂̂Ƃ���A�������Ȃ�...
        return;
    }

    /**
     *   �A�v���P�[�V��������I�����ɌĂяo����鏈��
     *   
     */
    protected void destroyApp(boolean arg0) throws MIDletStateChangeException
    {
        if (arg0 == true)
        {
            // �f�[�^�̃o�b�N�A�b�v�ƃI�u�W�F�N�g�̍폜�����s����
            objectFactory.cleanup();
        }

        // �I���w��
        notifyDestroyed();
        return;
    }
    
    /*------------------------------------------------------------------------------------------------------------*/

    /**
     *   �E�I�b�`�h�b�O�X���b�h
     * 
     */
    public void startWatchDog()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                Date  date      = new Date();
                long time      = date.getTime();
                long dummy     = time;
                long sleepTime = (objectFactory.preference).watchDogMinutes * 1000 * 60;
                if (sleepTime == 0)
                {
                    // �E�H�b�`�h�b�O�̊Ď��͍s��Ȃ�
                    return;
                }
                while (dummy != 0)
                {
                    if (watchDogMarking != -1)
                    {
                        // �E�H�b�`�h�b�O��~���ł͂Ȃ������Ƃ��A�A�A
                        time = date.getTime();
                        watchDogMarking = time;
                    }
                    try
                    {
                        Thread.sleep(sleepTime); // �҂�...
                    }
                    catch (Exception e)
                    {
                        // �������Ȃ�...
                    }
                    if (time == watchDogMarking)
                    {
                        // �^�C���A�E�g���o�A�A�A�A�v���P�[�V�������I������
                        objectFactory.backupFavorite();
                        objectFactory.cleanup();
                        notifyDestroyed();
                        return;
                    }
                }
            }
        };
        thread.start();
        return;
    }

    /**
     *  �E�I�b�`�h�b�O���N���A����B�B
     *
     */
    public void keepWatchDog(int number)
    {
        watchDogMarking = number;
        return;
    }
    
    /**
     *   �X���Q�Ɖ�ʂ𔲂��Ă������̏���...
     *   
     *       @param  threadNumber �X���ԍ�
     *       @param  maxNumber    �Ō�ɎQ�Ƃ������X�ԍ�
     *
     */
    public void EndViewForm(int threadNumber, int maxNumber)
    {
        keepWatchDog(-1);
        hideBusyMessage(); 

        // �ċN�����̃X���ǂݏo�����Ƃ��ċL�^���Ă������̂����ׂăN���A����
        if ((objectFactory.preference).openCurrentScreen > 0)
        {
            (objectFactory.preference).openCurrentScreen = 0;
        }
        (objectFactory.preference).currentMessageIndex = 0;
        (objectFactory.preference).currentScreenNumber = 0;
        (objectFactory.preference).openedFileName      = null;

        if (a2BexecutingStatus == a2B_EXECUTE_VIEW_MANAGED)
        {
            // �Ǘ��X����\�����ɎQ�Ɖ�ʂ��甲���Ă����ꍇ�́A���X�ԍ���DB�ɋL�^����
            (objectFactory.listForm).setCurrentMessage(threadNumber, maxNumber);
            openListForm(false);
        }
        else if (a2BexecutingStatus == a2B_EXECUTE_VIEW_READONLY)
        {
            // ���O�Q�ƃ��[�h�ŎQ�Ɖ�ʂ��甲���Ă����ꍇ�́A�f�B���N�g���Q�ƃ��[�h�ɂ���
            openFileSelector();
        }

        // �ꗗ���[�h�Q�ƒ��ɐݒ肷��
        a2BexecutingStatus = a2B_EXECUTE_SHOWLIST;
        return;
    }
    
    /** 
     *   a2B�I������...
     *   
     */
    public void quit_a2B()
    {
        keepWatchDog(-1);
        (objectFactory.listForm).setBusyMessage("a2B�I����...", false);
        (objectFactory.viewForm).setBusyMessage("a2B�I����...", false);

        // (�O�̂���)���C�ɓ���̃f�[�^���o�b�N�A�b�v����
        (objectFactory.favoriteManager).doBackup();

        // a2B�I�����w�����ꂽ�ꍇ...
        try
        {
            // �A�v���I�����Ăяo��...
            destroyApp(true);
        }
        catch (Exception e)
        {
            // ��O�����Ƃ��Ă͉������Ȃ�
        }
        return;
    }

    /**
     *  �X���ꗗ��ʂ����O�t�@�C���I�[�v�����[�h�Ŕ����Ă������̏���...
     * 
     *     @param  fileName   ���O�t�@�C����(*.dat)
     *     @param  msgIndex   �f�[�^�Ǘ��ԍ�
     *     @param  number     �I�[�v�����郌�X�ԍ�
     *  
     */
    public void EndListFormShowMode(String fileName, int msgIndex, int number)
    {
        keepWatchDog(-1);
        (objectFactory.listForm).setBusyMessage("�X����͒�...", true);

        // �X���Q�Ɖ�ʂ��J��...
        if ((objectFactory.preference).openCurrentScreen == 0)
        {
            (objectFactory.preference).openCurrentScreen = 1;
        }

        // �Ǘ��X���Q�ƒ���ԂɕύX����
        a2BexecutingStatus = a2B_EXECUTE_VIEW_MANAGED;
        openViewForm((objectFactory.preference).getBaseDirectory() + fileName, msgIndex, number);
        return;
    }

    /**
     *  �X���ꗗ��ʂ�dat�t�@�C���Q�ƃ��[�h�Ŕ����Ă������̏���...
     * 
     */
    public void EndListFormLogViewMode()
    {
        keepWatchDog(-1);
        (objectFactory.viewForm).setBusyMessage(null, false);

        // �t�@�C���I����ʂ��J��
        openFileSelector();
        return;
    }
    
    /**
     *   �������݃t�H�[���𔲂��Ă����Ƃ��̏���
     * 
     * 
     */
    public void returnToPreviousForm(int afterAction)
    {
        // �O��\�����Ă����t�H�[���ɐ؂�ւ���
        keepWatchDog(0);
        Display.getDisplay(this).setCurrent(previousForm);
        currentForm  = previousForm;
        previousForm = null;
        
        // �������Ȃ�...
        if (afterAction == ACTION_NOTHING_CANCEL)
        {
            // �������{�p����������Z�b�g����
            (objectFactory.listForm).setSearchTitleTemporary(null);
            return;
        }
        
        // �������Ȃ�...
        if (afterAction == ACTION_NOTHING)
        {
            return;
        }

        // �A�C�e���I�������{����B
        if (afterAction == ACTION_THREAD_OPERATION)
        {
            (objectFactory.listForm).selectedItem(false);        
            return;
        }

        // �X���擾�\������{����
        int  index  = 1;   // ������ ���̔ԍ��́A�u�X������v�R�}���h�̃C���f�b�N�X�ԍ��ƈ�v���Ă��Ȃ���΂Ȃ�Ȃ��B
        byte scene  = SHOW_SUBJECTCOMMAND;        
        if (afterAction == ACTION_THREAD_RESERVE)
        {
            if (getFavoriteShowMode() == false)
            {
                // �擾�\��́A�u���C�ɓ���v�ł͂ł��Ȃ����߁A�A�A�A�B
                index = 2;
                (objectFactory.listForm).executeCommandAction(scene, index, -1);
            }
            return;
        }

        // �X�����擾����
        if (afterAction == ACTION_THREAD_GETLOG)
        {
            if (getFavoriteShowMode() == true)
            {
                // ���C�ɓ���̂Ƃ��̎擾
                scene = SHOW_SUBJECTCOMMANDFAVORITE;
                index = 1;
            }
            else
            {
                // �ʏ�̎擾
                index = 3;
            }
            (objectFactory.listForm).executeCommandAction(scene, index, -1);
            return;
        }    
        return;
    }

    /**
     *   �������݂��\�ȃX�����ǂ�������������
     * 
     */    
    public boolean canWriteMessage()
    {
        if (a2BexecutingStatus == a2B_EXECUTE_VIEW_MANAGED)
        {
            return (true);
        }
        return (false);
    }
        
    /**
     *   �t�@�C���I���̏I��(�t�@�C����I�������ꍇ)
     *
     *      @param  fileName  �I�������t�@�C����
     */
    public void EndFileSelector(String fileName)
    {
        // �x�[�X�f�B���N�g���ݒ胂�[�h�̂Ƃ�...
        if (a2BexecutingStatus == a2B_EXECUTE_DIRSETTING)
        {
            // �f�B���N�g�����݂̂ɍ��...
            int pos = fileName.lastIndexOf('/', fileName.length() - 2);
            
            // �x�[�X�f�B���N�g�����t�@�C���ɐݒ肷��
            (objectFactory.preference).setBaseDirectory(fileName.substring(0, pos + 1));

            // �x�[�X�f�B���N�g�����t�@�C���Z���N�^�̃f�t�H���g�l�Ƃ���
            (objectFactory.fileSelector).setnextPath((objectFactory.preference).getBaseDirectory());

            // �L���f�B���N�g����\������B
            showDialog(SHOW_INFO, 0, "Information", "�f�[�^�f�B���N�g���� " + (objectFactory.preference).getBaseDirectory() + " �ɐݒ肵�܂����B\n\n ��xa2B���I�����܂��B");

            // ����(1.0s)�~�܂�...
            sleepTime(1000);

            // ��x�A�v���P�[�V�������I������
            try
            {
                destroyApp(true);
            }
            catch (Exception e)
            {
                // ��O�������ɂ͂Ȃɂ����Ȃ�
            }
            notifyDestroyed();

            return;
        }

        // �t�@�C���I�����[�h�ŁA�t�@�C�������w�肳��Ă��Ȃ��ꍇ...
        if (fileName == null)
        {
            // �ꗗ��ʂ��J��
            System.gc();

            // �ꗗ���[�h�Q�ƒ��ɐݒ肷��
            a2BexecutingStatus = a2B_EXECUTE_SHOWLIST;

            openListForm(true);
            return;
        }

        // �t�@�C���I�����[�h�̂Ƃ�...
        // (�w�肳�ꂽ�t�@�C����擪����I�[�v������)
        a2BexecutingStatus = a2B_EXECUTE_VIEW_READONLY;

        showDialog(SHOW_DEBUG, 0, "Debug", "�I�������t�@�C�����F" + fileName);
        openViewForm(fileName, -1, 1);
        return;
    }

    /**
     *    URL��Web�u���E�U�ŊJ��
     *     (a2B�I����Ƀu���E�U���N������)
     * 
     *    @param url �u���E�U�Őڑ�����URL
     * 
     */
    public void openWebBrowser(String url)
    {
        // (�O�̂���)���C�ɓ���̃f�[�^���o�b�N�A�b�v����
        (objectFactory.favoriteManager).doBackup();
        
        String openUrl = url;
        try
        {
            if ((getUseGoogleProxy() == true)&&(url.indexOf("http://") == 0))
            {
            	if (url.indexOf("www.google.co.jp/m/") < 0)
            	{
            		// google �̃v���L�V���g�p����ݒ�łȂ��ꍇ...
//                  openUrl = "http://www.google.co.jp/gwt/n?u=http%3A%2F%2F" + url.substring(7) + "&output=chtml";
                    openUrl = "http://www.google.co.jp/gwt/n?u=http%3A%2F%2F" + url.substring(7);
            	}
            }

            // URL�̃I�[�v���w�����s��
            this.platformRequest(openUrl);

            // URL�I�[�v����Ɍp�����邩�ǂ����m�F����
            if (getLaunchMode() != true)
            {
                // �p�����Ȃ��ꍇ�A�A�v���I�����Ăяo��...
                destroyApp(true);
            }
        }
        catch (Exception e)
        {
            /////// �u���E�U�N���͖��T�|�[�g�������A�������Ȃ�  /////
        }
        return;
    }

    /**
     *  �_�C�A���O��\������
     * 
     *      @param catagory  �_�C�A���O�\���̃J�e�S��
     *      @param timeout   �_�C�A���O�\������^�C���A�E�g�l (�P�ʁFms, 0�̂Ƃ��͖�����҂�)
     *      @param title     �\������_�C�A���O�^�C�g��
     *      @param message   �\������_�C�A���O�̃��b�Z�[�W
     *  
     *      @return  �����R�[�h (0�FOK, 0�ȊO�FNG)
     */
    public int showDialog(byte category, int timeout, String title, String message)
    {
        AlertType alertType = null;  // alertType �� null���ƁAWX310SA�ł͉�����Ȃ�

        // �J�e�S�����u�m�F�v�������ꍇ�ɂ�alertType��ύX����
        if (category == SHOW_CONFIRM)
        {
            alertType = AlertType.CONFIRMATION;
        }

        if ((objectFactory.preference).getConfirmationMode() < category)
        {
            // �ݒ肪�u�_�C�A���O��\�����Ȃ��v�ɂȂ��Ă����ꍇ�ɂ́A�\�������I������
            return (0);
        }

        if (timeout < 0)
        {
            // �\���^�C���A�E�g�l�����������ꍇ�ɂ́A�_�C�A���O��\�������ɏI������
            return (0);
        }

        if (timeout != 0)
        {
            // �^�C���A�E�g�l���[���ȊO��������^�C���A�E�g�l��ݒ肷��
            (objectFactory.dialogForm).setTimeout(timeout);
        }
        else
        {
            // �^�C���A�E�g�l���[���̂Ƃ��ɂ́A�����҂�(FOREVER)�ɂ���
            // (���̏ꍇ�ɂ́A�_�C�A���O���g���̂ł͂Ȃ��ABUSY�t�H�[�����g��)
            (objectFactory.selectForm).showBusyMessage(title, message, a2BselectionForm.CONFIRMATION_FORM);
            if (isShowDialog == true)
            {
                previousForm = currentForm;
                Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.selectForm);
            }
            else
            {
                previousForm = (Display.getDisplay(this)).getCurrent();
                Display.getDisplay(this).setCurrent(objectFactory.selectForm);
            }
            currentForm  = (Display.getDisplay(this)).getCurrent();
            return (0);
        }

        // �_�C�A���O�\������...
        (objectFactory.dialogForm).setTitle(title);
        (objectFactory.dialogForm).setString(message);
        (objectFactory.dialogForm).setType(alertType);

        // �_�C�A���O����ʕ\������
        if ((objectFactory.dialogForm) == (Display.getDisplay(this)).getCurrent())
        {
            return (0);
        }
        currentForm = (Display.getDisplay(this)).getCurrent();
        Display.getDisplay(this).setCurrent((objectFactory.dialogForm), currentForm);
        
        isShowDialog = true;
        return (0);
    }

    /**
     *   �_�C�A���O���B��
     * 
     */
    public void hideDialog()
    {
        isShowDialog = false;
        Display.getDisplay(this).setCurrent(currentForm);
        return;
    }

    /**
     *   �r�W�[���b�Z�[�W��\������
     * 
     * 
     */
    public void showBusyMessage(String title, String message, int usage)
    {
        if (isGetNewArrival == true)
        {
            keepWatchDog(0);
            (objectFactory.listForm).setBusyMessage(message, false);
            (objectFactory.viewForm).setBusyMessage(message, false);
            return;
        }        

        (objectFactory.selectForm).showBusyMessage(title, message, usage);
        if (isShowDialog == true)
        {
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.selectForm);
        }
        else
        {
            Display.getDisplay(this).setCurrent(objectFactory.selectForm);
        }
        return;
    }

    /**
     *   �r�W�[���b�Z�[�W���X�V����
     * 
     * 
     */
    public void updateBusyMessage(String title, String message, boolean forceUpdate)
    {
        keepWatchDog(0);
        (objectFactory.listForm).setBusyMessage(message, forceUpdate);
        (objectFactory.viewForm).setBusyMessage(message, forceUpdate);
        (objectFactory.selectForm).showBusyMessage(title, message, a2BselectionForm.BUSY_FORM_UPDATE);
        return;
    }

    /**
     *   �r�W�[���b�Z�[�W���B��
     * 
     */
    public void hideBusyMessage()
    {
        keepWatchDog(0);
        (objectFactory.listForm).setBusyMessage(null, false);
        (objectFactory.viewForm).setBusyMessage(null, false);
        (objectFactory.selectForm).hideBusyMessage();
        return;
    }

    /**
     *   �ꗗ�f�[�^�x�[�X�̍\�z�������������̏���
     *   
     */
    public void completedPrepareBbsDatabase()
    {
        // �ꗗ�̎擾���I���������Ƃ��f�o�b�O�\������
        showDialog(SHOW_DEBUG, 0, "Debug", "�ꗗ��͏I��");
        
        // �K�x�R�����{
        System.gc();

        // �X�N���[���T�C�Y��ݒ肷��
        (objectFactory.listForm).prepareScreenSize();

        // a2B�N�����ォ�ǂ���
        if (a2BexecutingStatus == a2B_EXECUTE_STARTED)
        {
            if ((objectFactory.preference).openCurrentScreen < 0)
            {
                // a2B��K�����C�ɓ���ꗗ�ŋN��������ꍇ...���C�ɓ���ݒ�ɐ؂�ւ���B
                (objectFactory.preference).favoriteShowMode = 1;
            }
        }

        // �ꗗ��ʂ�\������
        openListForm(true);
        return;
    }

    /**
     *   �X���ꗗ�̕\���������ł����Ƃ��ɌĂяo��...
     * 
     * 
     */
    public void readySubjectListDisplay()
    {
        // a2B�N�����ォ�ǂ���
        if (a2BexecutingStatus == a2B_EXECUTE_STARTED)
        {
            // ���ǃX���Q�ƃ��[�h���ǂ���
            (objectFactory.subjectParser).setOnlyDatabaseAccess((objectFactory.preference).onlyDatabaseAccess);
    
            // �O��a2B�I�����ɃX�����Q�Ƃ��Ă������ǂ����̃`�F�b�N
            if ((objectFactory.preference).openCurrentScreen > 0)
            {
                // a2B����I�[�v�����A�O��̎Q�ƃX���ƌ��݂̏�񂪐������Ă��邩�`�F�b�N����
                int currentNumber = checkFirstOpenScreen((objectFactory.preference).openedFileName, 
                                                          (objectFactory.preference).currentMessageIndex);
                if (currentNumber > 0)
                {
                    // �O��I�������Ƃ��̏��Ɛ������Ă����A�O��Q�Ƃ��Ă����X���̃��X��\������
                    (objectFactory.listForm).setCurrentMessageIndex((objectFactory.preference).currentMessageIndex);
                    a2BexecutingStatus = a2B_EXECUTE_VIEW_MANAGED;
                    openViewForm((objectFactory.preference).openedFileName, 
                                 (objectFactory.preference).currentMessageIndex,
                                 currentNumber);
                    return;
                }
            }
        }

        // �X���ꗗ�\�������������������Ƃ��ꗗ��ʂɒʒm����
        (objectFactory.listForm).readySubjectList();

        // �X���ꗗ�\������Ԃ֕ύX����
        a2BexecutingStatus = a2B_EXECUTE_SHOWLIST;

        // �X���ꗗ�\�����A�Ƃ����ݒ�ɂ���
        if ((objectFactory.preference).openCurrentScreen > 0)
        {
            (objectFactory.preference).openCurrentScreen = 0;
        }

        // �X���擾�\��V�[�P���X���ǂ���...
        if (isReserveSequence == true)
        {
            // "�擾�\��V�[�P���X"���������ꍇ�A�A�A�����̏��������s����
            afterReserveSequence();
            isReserveSequence = false;
            System.gc();
        }        
        return;
    }

    /**
     * ���X�Q�ƒ���a2B���I��������a2B���N�������Ƃ��A�O��I�������Ƃ̐��������`�F�b�N����
     * 
     * @param fileName     �O��a2B�I�����ɊJ���Ă����X���̃t�@�C����
     * @param messageIndex �O��a2B�I�����ɊJ���Ă����X���̊Ǘ��C���f�b�N�X�ԍ�
     * @return
     */
    private int checkFirstOpenScreen(String fileName, int messageIndex)
    {
        // �X���̃t�@�C�������Ǘ��C���f�b�N�X�ԍ��̃t�@�C�����ƈ�v���邩�H�H
        String datFileName = getThreadFileName(messageIndex);
        if (fileName.indexOf(datFileName) < 0)
        {
            // ��v���Ȃ��ꍇ�ɂ́A�ꗗ�\���Ƃ���
            return (-1);
        }
        
        // �O��\�����Ă������X�ԍ�����������
        return ((objectFactory.preference).currentScreenNumber);
    }
    
    /**
     *   http GET�ʐM�̎��s�����A�ʐM����/���s�͕ʃ��\�b�h�Ŕ��f����
     * 
     *   @param fileName   �擾�����f�[�^���L�^����t�@�C�����i�x�[�X�f�B���N�g������̑��΃p�X�j
     *   @param url        �擾����URL
     *   @param reference  ���M����Q��URL(���t�@��URL�Anull�̏ꍇ�ɂ͎w��ȗ�)
     *   @param appendMsg  �擾���ɉ�ʕ\�����郁�b�Z�[�W
     *   @param offset     ���M����I�t�Z�b�g(�擾����擪�o�C�g�̎w��A���̏ꍇ�ɂ͎w��ȗ�)
     *   @param range      ���M���郌���W (�擾�f�[�^�T�C�Y�̎w��A���̏ꍇ�ɂ͎w��ȗ�)
     *   @param scene      http�ʐM���g�p����󋵎w�� (�R�[���o�b�N���̎��ʎq�Ƃ��ė��p����)
     *   @param mode       �t�@�C���ɋL�^����Ƃ��ǋL���邩�A�㏑�����邩 (true�̎��ɂ͒ǋL���[�h)
     *   @return           �t�@�C���I�[�v������(true)�A�t�@�C���I�[�v�����s(false)
     * 
     */
    public boolean GetHttpCommunication(String fileName, String url, String reference, String appendMsg, int offset, int range, int scene, boolean mode)
    {
        // �ʐM���̏ꍇ�A�����ɏI������
        if ((objectFactory.httpCommunicator).isCommunicating() == true)
        {
            return (false);
        }

        int appendMode = (objectFactory.httpCommunicator).APPENDMODE_NEWFILE;
        if (mode == true)
        {
            appendMode = (objectFactory.httpCommunicator).APPENDMODE_APPENDFILE;
        }
        
        String getFileName = null;
        if (fileName != null)
        {
            getFileName = (objectFactory.preference).getBaseDirectory() + fileName;
        }
        else
        {
            // �����񃂁[�h
            appendMode = (objectFactory.httpCommunicator).APPENDMODE_STRING;
            offset = range;    
        }

        // �ʐM���s�w��
        boolean ret = (objectFactory.httpCommunicator).openFileConnection(getFileName, appendMode);
        if (ret != true)
        {
            return (false);
        }

        // �r�W�[�\�����s��
        updateBusyMessage("", "", false);
        showBusyMessage("http�ʐM������", url, a2BselectionForm.BUSY_FORM);
        
        // HTTP�ʐM������...
        if (getDivideGetHttp() == false)
        {
            // �X���ʏ�擾���[�h
            (objectFactory.httpCommunicator).setParameter(url, reference, appendMsg, offset, scene, appendMode, true);
            (objectFactory.httpCommunicator).getURLUsingHttp();
        }
        else
        {
            // �X�������擾���[�h
            int startRange = offset;
            (objectFactory.httpCommunicator).setParameter(url, reference, appendMsg, startRange, scene, appendMode, true);
            ret = (objectFactory.httpCommunicator).getURLUsingHttp();
            while (ret == true)
            {
                startRange = (objectFactory.httpCommunicator).getLastEndRange() + 1;
                (objectFactory.httpCommunicator).setParameter(url, reference, appendMsg, startRange, scene, (objectFactory.httpCommunicator).APPENDMODE_APPENDFILE, false);
                ret = (objectFactory.httpCommunicator).getURLUsingHttp();
            }
        }

        // BUSY�E�B���h�E�̕\������߂�
        hideBusyMessage();
        updateBusyMessage("", "", false);
        (objectFactory.httpCommunicator).closeFileConnection();
        return (true);
    }
    
    /**
     *    ��ʃI�[�v������(�������݉��)
     *   
     */
    public void OpenWriteForm(boolean useTitle, String title, int messageIndex, int number)
    {
        (objectFactory.writeForm).prepareWriteMessageForm(useTitle, title, messageIndex, number);
        openWriteForm();
        return;
    }
    
    /**
     *    ��ʃI�[�v������(�������݉�ʂ𕶎�����͉�ʂŗ��p����ꍇ...)
     *   
     */
    public void OpenWriteFormUsingWordInputMode(int mode, String title, String subject, String defaultData, boolean ignoreCase)
    {
        String data = "";
        if (defaultData != null)
        {
            data = defaultData;
        }
        (objectFactory.writeForm).prepareWriteFormUsingDataInputMode(mode, title, subject, data, -1, ignoreCase);
        openWriteForm();
        return;
    }

    /**
     *   ��ʃI�[�v������(�������݉�ʂ��f�[�^�ҏW���[�h�ŗ��p����ꍇ...)
     *   
     */
    public void OpenWriteFormUsingTextEditMode(String title, String message)
    {
        (objectFactory.writeForm).prepareWriteFormUsingTextEditMode(title, message);
        openWriteForm();
        return;
    }

    /**
     *   ��ʃI�[�v������(�������݉�ʂ��f�[�^�ҏW���[�h�ŗ��p����ꍇ...)
     *   
     */
    public void OpenWriteFormUsingThreadOperationMode(String title, String message, String addInfo, int level)
    {
        (objectFactory.writeForm).prepareWriteFormUsingTextEditMode(title, message);
        (objectFactory.writeForm).prepareThreadOperationMenu(addInfo, level);
        openWriteForm();
        return;
    }

    /**
     *   ��ʃI�[�v������(�������݉�ʂ��v���r���[�\�����[�h�ŗ��p����ꍇ...)
     *   
     */
    public void OpenWriteFormUsingPreviewMode(String title, String message)
    {
        (objectFactory.writeForm).prepareWriteFormUsingTextEditMode(title, message);
        (objectFactory.writeForm).preparePreviewMenu();
        openWriteForm();
        return;
    }
    
    /**
     * �X�����x����ݒ肷��
     * 
     * @param level
     */
    public void setFavorThreadLevel(int level)
    {
        (objectFactory.listForm).updateFavoriteThreadLevel(level);
        return;
    }

    /**
     *    ��ʃI�[�v������(�������݉�ʂ�a2B�ݒ�(Proxy�ݒ�)�ŗ��p����ꍇ...)
     *   
     */
    public void OpenWriteFormUsingProxySetting()
    {
        (objectFactory.writeForm).prepareWriteFormUsingProxySettingMode(SCENE_PROXY_SETTING, "a2B�ݒ�", getProxyUrl(), getUsedProxyScene(), getLaunchMode());
        openWriteForm();
        return;
    }
    
    /**
     * �������݉�ʂ̃I�[�v��(��ʐ؂�ւ�)����
     *
     */
    private void openWriteForm()
    {
        previousForm = currentForm;
        if (isShowDialog == true)
        {
//            previousForm = currentForm;
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.writeForm);
        }
        else
        {
//            previousForm = (Display.getDisplay(this)).getCurrent();
            Display.getDisplay(this).setCurrent(objectFactory.writeForm);
        }
        currentForm = objectFactory.writeForm;
        keepWatchDog(-1);
        return;
    }

    /**
     *   ��ʃI�[�v������(�Q�Ɖ��)
     * 
     *       @param  fileName �F ���O�t�@�C�����i�I�[�v������t�@�C�����j
     *       @param  msgIndex �F �f�[�^�Ǘ��ԍ�
     *       @param  number   �F �I�[�v�����郌�X�ԍ�
     */
    private void openViewForm(String fileName, int msgIndex, int number)
    {
        if (number <= 0)
        {
            number = 1;
        }
        (objectFactory.preference).openedFileName      = fileName;
        (objectFactory.preference).currentMessageIndex = msgIndex;
        (objectFactory.preference).currentScreenNumber = number;
        if (isShowDialog == true)
        {
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.viewForm);
        }
        else
        {
            Display.getDisplay(this).setCurrent(objectFactory.viewForm);
        }
        (objectFactory.viewForm).setupScreenSize();
        (objectFactory.viewForm).showMessage(fileName, msgIndex, number);
        (objectFactory.viewForm).setKeyRepeatDelay(getKeyRepeatTime());
        currentForm = objectFactory.viewForm;
        keepWatchDog(0);
        return;
    }
    
    /**
     *   ��ʃI�[�v������(�ꗗ��ʂ̃I�[�v��)
     * 
     */
    private void openListForm(boolean isUpdate)
    {
        showDialog(SHOW_DEBUG, 0, "Debug", "(open subject list form)");
        if (isShowDialog == true)
        {
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.listForm);
        }
        else
        {
            Display.getDisplay(this).setCurrent(objectFactory.listForm);
        }
        currentForm = objectFactory.listForm;
        if (isUpdate == true)
        {
            (objectFactory.listForm).prepareSubjectList((objectFactory.preference).currentBbsIndex, false);
        }
        ///  (objectFactory.listForm).setTitle("a2B");  // �ȗ����邩��...
        keepWatchDog(0);
        return;
    }

    /**
     *   ��ʃI�[�v������(�t�@�C���I�����)
     *   
     */
    private void openFileSelector()
    {
        showDialog(SHOW_DEBUG, 0, "Debug", "(open file directory)");

        (objectFactory.fileSelector).openDirectory(false);
        if (isShowDialog == true)
        {    
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.fileSelector);
        }
        else
        {
            Display.getDisplay(this).setCurrent(objectFactory.fileSelector);
        }
        currentForm = objectFactory.fileSelector;
        return;
    }

    /**
     *   �J�e�S��/�I�����[�h���L�����Z�������ꍇ�̌㏈��...
     * 
     */
    private void rewindBoardSelection()
    {
        // �J�e�S��/�I�����[�h�������ꍇ�ɃL�����Z���w�����󂯂��ꍇ...
        if ((objectFactory.preference).previousBbsCategoryIndex >= 0)
        {
            (objectFactory.preference).currentBbsCategoryIndex  = (objectFactory.preference).previousBbsCategoryIndex;
            (objectFactory.preference).previousBbsCategoryIndex = -1;
        }

        // �ꗗ���X�V����
        (objectFactory.bbsCommunicator).refreshBoardInformationCache(getCurrentCategoryIndex(), false, null);
        (objectFactory.listForm).prepareSubjectList((objectFactory.preference).currentBbsIndex, false);
        return;
    }

    /**
     *  �I�����ꂽ�A�C�e���ԍ����������� (�A�C�e���I����ʂ𔲂��Ă����Ƃ��̏���)
     * 
     */
    private boolean decideSelection(int scene, int selection, int appendInfo)
    {
        boolean ret = true;
        if (scene == SHOW_CATEGORYLIST)
        {
            // �J�e�S���I�����[�h...
            setCurrentBbsCategoryIndex(selection);
            (objectFactory.listForm).boardSelectionMode();
            return (false);
        }
        else if (scene == SHOW_BBSLIST)
        {
            // �I�t�Z�b�g�����߂�
            int index  = getCurrentCategoryIndex();
            int offset = objectFactory.bbsCommunicator.getCategoryIndex(index);
            
            // �I�����[�h...
            setCurrentBbsIndex(selection + offset);

            // �ʏ�\�����[�h�ɐ؂�ւ���
            (objectFactory.preference).favoriteShowMode = 0;            

            // ��؂�ւ����Ƃ��ɂ́A�X���^�C������������N���A����
            (objectFactory.listForm).clearSearchTitleString();

            // �ꗗ���\�z����
            (objectFactory.listForm).prepareSubjectList((selection + offset), false);
            return (true);
        }
        else if (scene == SHOW_SUBJECTCOMMAND)
        {
            // �I���R�}���h���s(�ꗗ���)...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == SHOW_SUBJECTCOMMANDFAVORITE)
        {
            // �I���R�}���h���s(�ꗗ���)...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == SHOW_SUBJECTCOMMANDSECOND)
        {
            // �ꗗ��ʂ̑�񃁃j���[...
            ret = objectFactory.listForm.executeMenuSecond((byte) scene, selection);
        }
        else if (scene == SHOW_VIEWCOMMANDSECOND)
        {
            // �Q�Ɖ�ʂ̑�񃁃j���[...
            objectFactory.viewForm.executeMenuSecond((byte) scene, selection);            
        }
        else if (scene == CONFIRM_DELETELOGDAT)
        {
            // �I���R�}���h���s(�ꗗ���)...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == CONFIRM_DELETELOGFILE)
        {
            objectFactory.fileSelector.executeCommandAction((byte) scene, selection);
        }
        else if (scene == CONFIRM_URL_FOR_BROWSER)
        {
            // "�u���E�U�ŊJ��"�w��...
            objectFactory.viewForm.executeCommandAction((byte) scene, selection, appendInfo);            
        }
        else if (scene == CONFIRM_GET_THREADNUMBER)
        {
            // �X���ԍ��w��擾�w��...
            objectFactory.viewForm.executeCommandAction((byte) scene, selection, appendInfo);            
        }
        else if (scene == CONFIRM_SET_THREADNUMBER)
        {
            // �X���ԍ��w��擾�w��...(�ڍ�)
            objectFactory.viewForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == CONFIRM_SELECT_OSUSUME2CH)
        {
            // �������߂Q�����˂�̕\���B�B�B
            objectFactory.viewForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == CONFIRM_SELECT_FIND2CH)
        {
            // 2�����˂錟���ŃX����I��...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);            
        }
        else if (scene == CONFIRM_SELECT_RELATE_KEYWORD)
        {
            // �֘A�L�[���[�h�����ŁA�L�[���[�h��I��...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);            
        }
        else if (scene == SHOW_LOGINMENU)
        {
            // �ꗗ��ʂ̃��O�C�����j���[...
            objectFactory.listForm.executeLoginMenu((byte) scene, selection);
        }
        return (ret);
    }

    /**
     *   ��ʂ��ꎞ�I�ɐ؂�ւ���
     * 
     */
    public void callScreen(Displayable a)
    {
        Display.getDisplay(this).setCurrent(a);    
        return;
    }
    
    /**
     *   ��ʂ�O�̉�ʂɖ߂�
     * 
     */
    public void returnScreen()
    {
        Display.getDisplay(this).setCurrent(currentForm);    
        return;
    }
    
    /**
     *  �t�H���g�ݒ�������Z�b�g
     *
     */
    public void updateFontData(boolean isReset, int fontSize)
    {
        (objectFactory.preference).updateFontData(isReset, fontSize);
        (objectFactory.listForm).changeFont(true);
        (objectFactory.viewForm).changeFontSize(true);
        return;
    }
    
    /**
     *  �X���ꗗ�������[�h����
     * 
     */
    public void reloadCurrentMessageList()
    {
        if (isGetNewArrival() == true)
        {
            // �V���m�F���͋֎~����
            return;
        }

        // �X���ꗗ�������[�h����
        (objectFactory.listForm).prepareSubjectList(((objectFactory.preference).currentBbsIndex), true);
        return;
    }

    /**
     *  �X���������[�h����
     * 
     */
    public void reloadCurrentMessage()
    {
        if (isGetNewArrival() == true)
        {
            // �V���m�F���͋֎~����
            return;
        }

        // �X���������[�h����
        (objectFactory.listForm).reloadCurrentMessage();
        return;
    }
    
    /**
     * ���݊J���Ă����URL����������
     * 
     * @return ���݊J���Ă���URL
     */
    public String getCurrentMessageBoardUrl()
    {
        return ((objectFactory.listForm).getCurrentMessageBoardUrl());
    }

    /**
     *  �Q�ƒ��̃��O�t�@�C�����u���E�U�ŊJ��
     * 
     * 
     */
    public void launchBrowserCurrentMessage()
    {    
        (objectFactory.listForm).launchWebBrowserCurrentSelectedIndex();
        return;
    }
    
    /**
     * �����t�@�C��������������
     * 
     * @return
     */
    public String getOutputMemoFileName()
    {
        return ((objectFactory.preference).memoFileName);
    }
    
    /**
     *  ���X���t�@�C���ɏo�͂���
     * 
     */
    public void outputMemoTextFile(String title, String message)
    {
        boolean result = (objectFactory.subjectParser).outputDataTextFile(getOutputMemoFileName(), title, message, true);
        (objectFactory.viewForm).completedOutputMessage(result);
    }

    /**
     * ��URL�֔ꗗ���X�V����B�B�B
     * 
     * @param boardNick
     * @return
     */
    public boolean moveToBoard(String boardNick)
    {
        boolean ret = (objectFactory.bbsCommunicator).moveToBoard(boardNick);
        System.gc();
        if (ret == true)
        {
            // �f�B���N�g�����@���Ă��Ȃ��ꍇ�ɂ͍쐬����
            (objectFactory.bbsCommunicator).checkBoardNickName(boardNick);
            
            // �ʏ�\�����[�h�ɐ؂�ւ��A�\������ꗗ��ύX����...
            (objectFactory.preference).favoriteShowMode = 0;            

            // ��؂�ւ����Ƃ��ɂ́A�X���^�C������������N���A����
            (objectFactory.listForm).clearSearchTitleString();

            // �ꗗ���\�z����
            (objectFactory.listForm).prepareSubjectList((objectFactory.preference).currentBbsIndex, false);
        }
        return (ret);
    }

    /**
     *  �f�[�^���t�@�C���ɏo�͂���
     * 
     */
    public boolean outputTextFile(String fileName, String title, String message, boolean appendMode)
    {
        return ((objectFactory.subjectParser).outputDataTextFile(fileName, title, message, appendMode));
    }
    
    /**
     *  �Q�ƒ��̃X���^�C�g�����̂��擾����
     * 
     */
    public String getSubjectName(int index)
    {
        return ((objectFactory.listForm).getSubjectName());
    }
    
    /**
     *   ��ʃI�[�v������(�f�[�^�I�����)
     * 
     *       @param  busyTitle �F �^�C�g��
     *       @param  scene     �F �\���V�[��
     */
    public void openSelectForm(String busyTitle, Vector itemList, int topItem, int scene)
    {
        (objectFactory.selectForm).showSelection(busyTitle, itemList, topItem, scene);
        if (isShowDialog == true)
        {
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.selectForm);
        }
        else
        {
            if (Display.getDisplay(this).getCurrent() != objectFactory.selectForm)
            {
                Display.getDisplay(this).setCurrent(objectFactory.selectForm);
            }
        }
        return;
    }

    /**
     *   ��ʃI�[�v������(�f�[�^�I����ʂ��甲����ꍇ...)
     * 
     */
    public void endSelectForm(int scene, int selection, int appendInfo)
    {
        boolean redraw = true;
        Thread thread = null;
        if (selection < 0)
        {
            // �J�e�S���܂��́A�I�����L�����Z�������Ƃ��̏���...
            if ((scene == SHOW_CATEGORYLIST)||(scene == SHOW_BBSLIST))
            {
                // �I���̎��{
                thread = new Thread()
                {
                    public void run()
                    {
                        // �̑I���L�����Z��...
                        rewindBoardSelection();            
                    }
                };
                thread.start();
            }
            else
            {
                // ���̑��̃L�����Z������...
                if ((scene == CONFIRM_SELECT_FIND2CH)||(scene == CONFIRM_SELECT_RELATE_KEYWORD))
                {
                    // 2ch��������ъ֘A�L�[���[�h�����͂̂Ƃ��́A�L�����Z�����Ă�"�I��"���Ăяo��
                    // (�\�z�������X�g���N���A����K�v�����邽��)
                    redraw = decideSelection(scene, selection, appendInfo);                    
                }
            }
        }
        else  // if (selection >= 0)
        {
            // �A�C�e����I�������Ƃ�...
            redraw = decideSelection(scene, selection, appendInfo);
        }

        if (redraw == true)
        {
            // �\����ʂ�؂�ւ��� (���̉�ʂɖ߂�)
            if (isShowDialog == true)
            {
                Display.getDisplay(this).setCurrent(objectFactory.dialogForm, currentForm);
            }
            else
            {
                Display.getDisplay(this).setCurrent(currentForm);
            }
        }
        System.gc();
        return;
    }
    
    /** 
     *  HTTP�ʐM�N���X�ŏ���f�[�^���󂯎�����Ƃ��̏���...
     * 
     * 
     */
    public int getHttpDataReceivedFirst(int scene, int responseCode, boolean isDivideContinue, byte[] buffer, int offset, int length)
    {
        if (isDivideContinue == false)
        {
            // �X�������擾���̂Q��ڈȍ~�̃��N�G�X�g�������ꍇ...
            return (offset);
        }
        
        // �X�������擾���s��...
        if ((scene == SCENE_PARTGET_MESSAGE)||(scene == SCENE_GETALL_MESSAGE))
        {
            if ((length == 1)&&(buffer[offset] == (byte) 10))
            {
                // �X���X�V�Ȃ�...
                return (-1);
            }
            if (buffer[offset] != (byte) 10)
            {
                // �f�[�^�擾�G���[...
                return (-10);
            }
            // �����擾�̎��s�I�I
            return (offset + 1);
        }
        if ((scene == SCENE_WHOLEGET_MESSAGE)||(scene == SCENE_GETRESERVE_MESSAGE))
        {
            // �X���S�̎擾�����{����
            return (offset);
        }
        if ((scene == SCENE_PREVIEW_MESSAGE)||(scene == SCENE_PREVIEW_MESSAGE_EUC)||
            (scene == SCENE_PREVIEW_MESSAGE_SJIS)||(scene == SCENE_GET_OSUSUME_2CH)||
            (scene == SCENE_GET_RELATE_KEYWORD))
        {
            // �X�N���b�`�o�b�t�@�Ƀf�[�^�R�s�[����
            copyToScratchBuffer(buffer, offset, length);
            return (offset);
        }
        return (offset);
    }

    /**
     *   �f�[�^���X�N���b�`�o�b�t�@�ɃR�s�[����
     * 
     * 
     */
    public void copyToScratchBuffer(byte[] buffer, int offset, int length)
    {
        // �X�N���b�`�o�b�t�@�Ƀf�[�^�R�s�[����
        scratchBuffer = new byte[length];
        System.arraycopy(buffer, offset, scratchBuffer, 0, length);
        return;
    }
    
    /**
     *   HTTP�ʐM���I�������Ƃ��̏���...
     * 
     * 
     */
    public void CompletedToGetHttpData(int scene, int responseCode, int length)
    {
        // �X�������擾
        if (scene == SCENE_PARTGET_MESSAGE)
        {
            // length == 0 �Ȃ�A�f�[�^�̒ǋL�Ȃ�...
            boolean result = false;
            (objectFactory.listForm).completedGetLogDataFile(result, length);
            return;
        }

        // �X���S���擾
        if (scene == SCENE_WHOLEGET_MESSAGE)
        {
            boolean result = true;
            (objectFactory.listForm).completedGetLogDataFile(result, length);
            return;
        }

        // �ꗗ�S���擾
        if (scene == SCENE_BBSTABLE_MESSAGE)
        {
            (objectFactory.bbsCommunicator).prepareBbsDatabase();
            return;
        }
        
        // �X���ꗗ�S���擾
        if (scene == SCENE_GETSUBJECTLIST)
        {
            // �X���ꗗ�̃f�[�^���H�w��
            (objectFactory.subjectParser).formatSubjectList();
            return;
        }
        
        // �V���m�F�����{����...
        if ((scene == SCENE_GETALL_MESSAGE)||(scene == SCENE_GETRESERVE_MESSAGE))
        {
            // �V���m�F���ʂ𔽉f������
            (objectFactory.listForm).completedGetLogList(length);

            // ���̐V���m�F���s��...
            (objectFactory.listForm).startGetLogList(a2BMessageListForm.GETLOGLIST_CONTINUE_RECURSIVE);
            return;
        }
        
        // �v���r���[�����{�����ꍇ...
        if (scene == SCENE_PREVIEW_MESSAGE)
        {
            previewMessage(true, a2BsubjectDataParser.PARSE_2chMSG);
        }
        else if (scene == SCENE_PREVIEW_MESSAGE_EUC)
        {
            previewMessage(true, a2BsubjectDataParser.PARSE_EUC);
        }
        else if (scene == SCENE_PREVIEW_MESSAGE_SJIS)
        {
            previewMessage(true, a2BsubjectDataParser.PARSE_SJIS);
        }
        else if (scene == SCENE_GET_OSUSUME_2CH)
        {
            // �������߂Q�����˂�̕\�����s��
            (objectFactory.viewForm).parseOsusume2ch(scratchBuffer);
            scratchBuffer = null;
        }
        else if (scene == SCENE_GET_FIND_2CH)
        {
            // 2�����˂錟���̕\�����s���B(����̓v���r���[�̂�...)
            loadScratchBufferFromFile("temporary.html");
            (objectFactory.listForm).parseFind2ch(scratchBuffer);
            scratchBuffer = null;
            System.gc();
        }
        else if (scene == SCENE_GET_RELATE_KEYWORD)
        {
            // ��M�����֘A�L�[���[�h�̉�͂��s��
            (objectFactory.listForm).parseRelateKeyword(scratchBuffer);
            scratchBuffer = null;
            System.gc();
        }
        return;
    }
    
    /**
     * �q�[�v������������...
     * @return �q�[�v���
     */
    public String getHeapInfo()
    {
/***/
        return ("");
/***
        String appendString = "a2B-Dir : " + (objectFactory.preference).getBaseDirectory() + "\n";
        Runtime run = Runtime.getRuntime();
        return ("\n\n(Heap : " + run.freeMemory() + " / " + run.totalMemory() + ")\n" + appendString);
***/
    }

    /**
     *  ���b�Z�[�W�̃v���r���[���s��
     * 
     * 
     */
    public void previewMessage(boolean useForm, int viewMode)
    {
        (objectFactory.subjectParser).previewMessage(scratchBuffer, useForm, viewMode);
        scratchBuffer = null;
        System.gc();
        return;
    }

    /**
     *  �X���^�C�g�����X�V����
     * 
     * @param title
     */
    public void updateThreadTitle(byte[] title, int offset, int length)
    {
        if ((a2BexecutingStatus == a2B_EXECUTE_VIEW_MANAGED)&&(title != null))
        {
            // �Ǘ��X����\�����̏ꍇ�ɂ́A�X���^�C�g�����X�V����
            (objectFactory.listForm).updateThreadTitle(title, offset, length);
        }
        return;
    }

    /*
     *  �X���^�C�g���𒊏o���ĉ�������
     * 
     */
    public String pickupThreadTitle()
    {
        String title = (objectFactory.subjectParser).pickupThreadTitle(scratchBuffer);
        scratchBuffer = null;
        return (title);        
    }

    /**
     *  �X��URL���擾����
     * 
     */
    public String getBoardURL(int index)
    {
        if (getFavoriteShowMode() == false)
        {
            return ((objectFactory.bbsCommunicator).getBoardURL(getCurrentBbsIndex()));
        }
        else
        {
            return ((objectFactory.favoriteManager).getUrl(index));
        }
    }

    /**
     *   �X���t�@�C�������擾����
     * 
     */
    public String getThreadFileName(int index)
    {
        if (getFavoriteShowMode() == false)
        {
            return ((objectFactory.subjectParser).getThreadFileName(index));
        }
        else
        {
            return ((objectFactory.favoriteManager).getThreadNumber(index) + ".dat");
        }
    }

    /**
     * ���ݓǂ񂾃��X�ʒu��o�^����
     * @param number     �ǂ񂾃��X�ʒu
     * @param maxNumber  ���X�̍ő吔
     */
    public void setCurrentMessage(int number, int maxNumber)
    {
        // ���ݓǂ񂾃��X�����L�^����
        (objectFactory.listForm).setCurrentMessage(number, maxNumber);
    }

    /**
     *   �X���ԍ��w��擾�̉��
     * 
     */
    public void parseMessageThreadNumber(String threadNumber, int addInfo)
    {
        if (parseTargetString != null)
        {
            return;
        }
        parseTargetString = threadNumber;
        if (addInfo == 0)
        {
            // �X���ԍ��擾�̃X���擾�\�񏈗�...
            Thread thread = new Thread()
            {
                public void run()
                {
                    // �X���ԍ��w��擾�̉�͎�����
                    (objectFactory.viewForm).lockOperation = true;
                    
                    boolean ret = false;
                    ret = (objectFactory.subjectParser).parseMessageThreadNumber(parseTargetString);
                    parseTargetString = null;
                    if ((getAutoEntryFavorite() == true)&&(ret == true)&&(getFavoriteShowMode() == true))
                    {
                        // ���C�Ɏ����o�^���[�h�ŁA���݂��C�ɓ��胂�[�h�ŁA�X���ԍ��w��\�񂵂��ꍇ...
                        // �i���C�ɓ���̃C���f�b�N�X���P�����̂ōX�V����...���̑΍�ł����߃|...�j
                    	int index = (objectFactory.listForm).getCurrentMessageIndex() + 1;
                        (objectFactory.listForm).setCurrentMessageIndex(index);
                        (objectFactory.preference).currentMessageIndex = index;
                        (objectFactory.viewForm).updateCurrentMessageIndex(index);
                    }
                    (objectFactory.viewForm).completedParseMessageThreadNumber(ret);
                    (objectFactory.viewForm).lockOperation = false;
                }
            };
            try
            {
                thread.start();
                thread.join();
            }
            catch (Exception ex)
            {
            	// �������Ȃ��B
            }
        }
        else if ((addInfo == 1)||(addInfo == 2))
        {            
            // �X���ԍ��擾����... (�ړ� + �X���擾(�\��)����)
            Thread thread = new Thread()
            {
                public void run()
                {
                    // �X���ԍ��w��擾�̉�͎�����
                    String targetString = parseTargetString;
                    int pos = targetString.indexOf("/");
                    if (pos > 0)
                    {
                        // BUSY�\��������
                        (objectFactory.listForm).setInformationMessage(0, "�ꗗ�؂�ւ���...");

                        // �X���ꗗ�����o�b�N�A�b�v����...
                        (objectFactory.subjectParser).leaveSubjectList(getCurrentBbsIndex());
                        
                        // ��؂�ւ���...
                        String threadNick = targetString.substring(0, (pos + 1));
                        isReserveSequence = true;
                        moveToBoard(threadNick);
                    }
                    System.gc();
                }
            };
            thread.start();
        }
        return;
    }

    /**
     *  �X���擾�\��V�[�P���X�̑���...
     * 
     */
    private void afterReserveSequence()
    {
        // �擾(�\��)�̎��������s...        
        (objectFactory.listForm).setInformationMessage(0, "�擾(�\��)���s");
        (objectFactory.subjectParser).parseMessageThreadNumber(parseTargetString);    
        parseTargetString = null;
        
        return;
    }
    
    /**
     * �X���擾�\�񒆂ɃX�����X���ꗗ�ɔ��������ꍇ...
     * 
     * @param index
     */
    public void detectMessageThreadNumber(int index)
    {
        //
        if (isReserveSequence == true)
        {
            // �X�����擾����...
            (objectFactory.listForm).getLogDataFile(index, (objectFactory.listForm).GETMODE_GETALL);
            (objectFactory.listForm).setInformationMessage(-1, null);
        }
        return;
    }

    /**
     *   �V���m�F�I�����̏���
     * 
     */
    public void endGetNewArrivalMessage()
    {
        isGetNewArrival = false;

        // �u���u��������...
        vibrate();

        // �I�����b�Z�[�W��\������
        (objectFactory.viewForm).setBusyMessage("�V���m�F�I��", false);

        return;
    }
    
    /**
     *  �u���u��������...
     * 
     *
     */
    public void vibrate()
    {
        int milli = (objectFactory.preference).vibrateDuration;
        Display.getDisplay(this).vibrate(milli);        
        return;
    }

    
    /**
     * ���X�܂Ƃߓǂ݃��[�h��ݒ肷��
     * 
     * 
     * @param count
     */
    public void setResBatchMode(int count)
    {        
        int batchCount = ((objectFactory.preference).resBatchMode) / 256;
        (objectFactory.preference).resBatchMode = batchCount * 256 + count;
        return;
    }

    /**
     * ���X�܂Ƃߓǂ݃��[�h�̒l����������
     * 
     * @return
     */
    public int getResBatchMode()
    {
        int batchMode = ((objectFactory.preference).resBatchMode) % 256;
        return (batchMode);
    }

    /**
     * ���X�܂Ƃߓǂ݃��[�h�̂܂Ƃߓǂݒl��ݒ肷��
     * 
     * 
     * @param count
     */
    public void setResBatchCount(int count)
    {
         int batchMode = ((objectFactory.preference).resBatchMode) % 256;
        (objectFactory.preference).resBatchMode = batchMode + (count * 256);
        return;
    }

    /**
     * ���X�܂Ƃߓǂ݃��[�h�̒l����������
     * 
     * @return
     */
    public int getResBatchCount()
    {
        int batchCount = ((objectFactory.preference).resBatchMode) / 256;
        return (batchCount);
    }
    
    /**
     * �u���u�������鎞�Ԃ�ݒ肷��...
     * @param milliSeconds
     */
    public void setVibrateDuration(int milliSeconds)
    {
        (objectFactory.preference).vibrateDuration = milliSeconds;
        if (milliSeconds != 0)
        {
            Display.getDisplay(this).vibrate(milliSeconds);            
        }
        return;
    }

    /**
     * �V���m�F���s�����ǂ�������������
     * @return true(�V���m�F��) / false(�V���m�F���Ă��Ȃ�)
     */
    public boolean isGetNewArrival()
    {
        return (isGetNewArrival);
    }

    /**
     * �V���m�F�̎��s���J�n����
     *
     */
    public void startGetLogList()
    {
        if (isGetNewArrival() == true)
        {
            // �V���m�F���͋֎~����
            return;
        }
        // �V���m�F
        Thread thread = new Thread()
        {
            public void run()
            {
                // �V���m�F�����s����
                isGetNewArrival = true;
                (objectFactory.listForm).getNewArrivalMessage();
            }
        };
        thread.start();
        return;
    }

    /**
     * �r�W�[��ʂ��o���Ȃ��悤�ɂ���
     *
     */
    public void lockCommunicationScreen()
    {
        isGetNewArrival = true;
    }

    /**
     * �r�W�[��ʂ��o���悤�ɂ���
     *
     */
    public void unlockCommunicationScreen()
    {
        isGetNewArrival = false;
    }
    
    /**
     *   �V���m�F�̏����������������Ƃ�񍐂���
     *
     * 
     */
    public void readyGetLogList()
    {
        isGetNewArrival = true;
        (objectFactory.listForm).startGetLogList(a2BMessageListForm.GETLOGLIST_START_RECURSIVE);
        return;
    }

    /**
     *   ���C�ɓ���ݒ肷��
     * 
     */
    public void setFavoriteThread(int index)
    {
        if (getFavoriteShowMode() == false)
        {
            (objectFactory.listForm).setResetFavoriteThread(index, true);
        }
        return;
    }
    
    /**
     *  �X���^�C�����������ݒ肷��
     * 
     */
    public void setInputData(int scene, String dataString, byte searchStatus, boolean ignoreCase, boolean use2chSearch, boolean useGoogleSearch, int searchHour)
    {
    	if (useGoogleSearch == true)
    	{
    		// Google�������g�p���Č�������ꍇ...
    		String convertStr = "http://www.google.co.jp/m/search?q=";
//            String convertStr = "http://www.google.co.jp/m/search?ie=shift_jis&q=" + a2BsjConverter.urlEncode(dataString.getBytes());
    		try
    		{
                convertStr =  convertStr + a2BsjConverter.urlEncode(dataString.getBytes("UTF-8"));
    		}
    		catch (Exception e)
    		{
                convertStr =  convertStr + a2BsjConverter.urlEncode(dataString.getBytes());    			
    		}
            openWebBrowser(convertStr);
    		dataString = "";
    		return;
    	}
        if (scene == SCENE_TITLE_SEARCH)
        {
            if (use2chSearch == true)
            {
                // �Q�����˂錟�����g�p���Č�������ꍇ...
                (objectFactory.listForm).find2chThreadNames((objectFactory.writeForm).utf8ToEUCandURLEncode(dataString));
                dataString = "";
            }
            int option = a2BMessageListForm.OPTION_NOTIGNORE_CASE;
            if (ignoreCase == true)
            {
                option = a2BMessageListForm.OPTION_IGNORE_CASE;
            }
            (objectFactory.listForm).setSearchTitleTemporary(dataString);
            (objectFactory.listForm).setSearchTitleInformation(dataString, searchStatus, option, searchHour);
        }
        else if (scene == SCENE_NGWORD_INPUT)
        {
            (objectFactory.viewForm).setNGword(dataString);
        }
        else if (scene == SCENE_MEMOFILE_INPUT)
        {
            (objectFactory.preference).memoFileName = dataString;
        }
        else if (scene == SCENE_THREADNUM_INPUT)
        {
            parseMessageThreadNumber(dataString, 0);            
        }
        else if (scene == SCENE_RENAMEFILE_INPUT)
        {
            (objectFactory.fileSelector).doRenameFileName(dataString);
        }
        else if (scene == SCENE_GETHTTP_INPUT)
        {
            (objectFactory.fileSelector).doReceiveHttp(dataString);            
        }
        else if (scene == SCENE_COPYFILE_INPUT)
        {
            (objectFactory.fileSelector).doCopyFileName(dataString);
        }
        else if (scene == SCENE_MESSAGE_SEARCH)
        {
            (objectFactory.viewForm).setSearchString(dataString, searchStatus, ignoreCase);            
        }
        return;
    }

    /**
     *  �擾URL����AHTTP�v���w�b�_�𐶐��i�ϊ��j���� (WX310�p)
     * 
     *
     */
    private String modifyUrlforWX310(String urlToGet, String cookie, String referer)
    {
        int start = urlToGet.indexOf("://");
        start = start + 3;
        int end   = urlToGet.indexOf("/", start);
        String host = urlToGet.substring(start, end);
        urlToGet = urlToGet + " HTTP/1.0\r\n";
        urlToGet = urlToGet + "Host: " + host + "\r\n";
        urlToGet = urlToGet + "User-Agent: " + getUserAgent(true) + "\r\n";
        urlToGet = urlToGet + "Accept-Encoding: identity\r\n";
        urlToGet = urlToGet + "Content-Language: ja, en\r\n";
        if (cookie != null)
        {
            urlToGet = urlToGet + "Cookie:" + cookie + "\r\n";            
        }
        if (referer != null)
        {
            urlToGet = urlToGet + "Referer:" + referer + "\r\n";            
        }
        urlToGet = urlToGet + "Connection: Close\r\n";
        urlToGet = urlToGet + "Pragma: no-cache\r\n";
        urlToGet = urlToGet + "\r\nWX310";        
        return (urlToGet);
    }

    /**
     *  �L�^���Ă���Cookie���N���A����
     *  (doHttpMain() �� Cookie�𗘗p����ꍇ)
     *
     */
    public void clearHttpCacheCookie()
    {
        previousHttpCookie = null;
    }
    
    /**
     * HTTP�ʐM�����s���ăt�@�C�����擾����
     * 
     * @param url
     * @param fileName
     * @param useCachedFile
     */
    public int doHttpMain(String url, String fileName, String referer, boolean useCachedFile, int usingMode)
    {
        int            rc  = -1;
        HttpConnection   c = null;
        InputStream     is = null;
        OutputStream    os = null;
        FileConnection dfc = null;
        byte[] data = new byte[FILE_BUFFER_SIZE + MARGIN];
        try
        {
            (objectFactory.viewForm).setBusyMessage("OPN:" + url, false);

            // �����o���t�@�C���̏���...
            dfc = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
            if (dfc.exists() == true)
            {
                if ((useCachedFile == true)&&(dfc.fileSize() > 0))
                {
                    // �t�@�C���擾�͎��{���Ȃ�...�擾�ς݃t�@�C�����t�@�C���𗘗p����
                    return (HttpConnection.HTTP_OK);
                }
                else
                {
                    try
                    {
                        // �t�@�C�������݂���ꍇ�ɂ͍폜����
                        dfc.delete();
                    }
                    catch (Exception ex)
                    {
                        (objectFactory.viewForm).setBusyMessage("DEL:" + ex.getMessage(), false);                        
                    }
                }
            }
            // �t�@�C���쐬
            dfc.create();

            // �v���b�g�t�H�[�����̂𒲂ׁAWX310�������ꍇ�ɂ́Agzip�]���΍���s��
            String urlToGet = url;
            String platForm = System.getProperty("microedition.platform");
            if (platForm.indexOf("WX310") >= 0)
            {
                // WX310�p��URL��ύX����...
                if (usingMode == a2B_HTTPMODE_USEPREVIOUSCOOKIE)
                {
                    urlToGet = modifyUrlforWX310(url, previousHttpCookie, referer);
                }
                else
                {
                    urlToGet = modifyUrlforWX310(url, null, referer);
                }

                // HTTP�ʐM�̏���
                c = (HttpConnection) Connector.open(urlToGet);
            }
            else
            {
                // HTTP�ʐM�̏���
                c = (HttpConnection) Connector.open(urlToGet);

                // �l�t�����U��...
                c.setRequestMethod(HttpConnection.GET);
                c.setRequestProperty("user-agent", getUserAgent(true));
                c.setRequestProperty("accept", "text/html, */*");
                c.setRequestProperty("content-language", " ja, en");
                c.setRequestProperty("connection", " close");
                c.setRequestProperty("range", " bytes=0-");
                
                if (referer != null)
                {
                    c.setRequestProperty("referer", referer);
                }
                if (usingMode == a2B_HTTPMODE_USEPREVIOUSCOOKIE)
                {
                    c.setRequestProperty("cookie", previousHttpCookie);
                }
            }

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            rc = c.getResponseCode();

            (objectFactory.viewForm).setBusyMessage("RCV:" + url, false);

            // Cookie �𒊏o����...
            if (usingMode == a2B_HTTPMODE_PICKUPCOOKIE)
            {
                int   index = 0;
                String key = "";
                String value = "";
                previousHttpCookie = "";
                try
                {
                    while (index >= 0)
                    {
                        value = c.getHeaderField(index);
                        key   = c.getHeaderFieldKey(index);
                        if (key.compareTo("set-cookie") == 0)
                        {
                            //
                            int pos = value.indexOf(";");
                            if (previousHttpCookie.length() != 0)
                            {
                                previousHttpCookie = previousHttpCookie + "; ";
                            }
                            previousHttpCookie = previousHttpCookie + value.substring(0, pos);
                        }
                        else if (key.compareTo("Set-Cookie") == 0)
                        {
                            //
                            int pos = value.indexOf(";");
                            if (previousHttpCookie.length() != 0)
                            {
                                previousHttpCookie = previousHttpCookie + "; ";
                            }
                            previousHttpCookie = previousHttpCookie + value.substring(0, pos);
                        }
                        value = "";
                        key   = "";
                        index++;
                    }
                }
                catch (Exception e)
                {
                    // �G���[����ݒ�...
                    (objectFactory.viewForm).setBusyMessage("a:" + e.toString(), false);
                }
            }

            // stream open
            is = c.openInputStream();
            os = dfc.openOutputStream();                

            (objectFactory.viewForm).setBusyMessage("WRT:" + url, false);

            int writeBytes = 0;
            int actual = 10;
            while (actual > 0)
            {
                actual = is.read(data, 0, FILE_BUFFER_SIZE);
                writeBytes = writeBytes + actual;
                (objectFactory.viewForm).setBusyMessage("RCV:" + writeBytes + "(" + actual + ")", false);
                if (actual > 0)
                {
                    os.write(data, 0, actual);
                }
                else
                {
                    (objectFactory.viewForm).setBusyMessage("Fin:" + writeBytes + "(" + actual + ")", false);                    
                }
            }
            os.flush();
            os.close();
            is.close();
            c.close();
            dfc.close();
            data = null;
            (objectFactory.viewForm).setBusyMessage("END:" + url, false);
        }
        catch (Exception e)
        {
            (objectFactory.viewForm).setBusyMessage("b:" + e.toString(), false);
            rc = rc * (-1000);
            try
            {
                if (os != null)
                {
                    os.flush();
                    os.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
                if (is != null)
                {
                    is.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
                if (c != null)
                {
                    c.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
                if (dfc != null)
                {
                    dfc.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
        }
        data = null;
        System.gc();
        return (rc);
    }    
    
    /*
     *   ���݂̔J�e�S���C���f�b�N�X��ݒ肷��
     *   
     *       @param  index �F �J�e�S���C���f�b�N�X�ԍ�
     */
    public void setCurrentBbsCategoryIndex    (int index)
    {
        if (index < 0)
        {
            index = 0;
        }
        (objectFactory.preference).previousBbsCategoryIndex = (objectFactory.preference).currentBbsCategoryIndex;
        (objectFactory.preference).currentBbsCategoryIndex  = index;
        return;
    }
    
    /*
     *   ���݂̔C���f�b�N�X��ݒ肷��
     *   
     *       @param  index �F �C���f�b�N�X�ԍ�
     */
    public void setCurrentBbsIndex(int index)
    {
        if (index < 0)
        {
            index = 0;
        }
        (objectFactory.preference).currentBbsIndex          = index;
        (objectFactory.preference).previousBbsCategoryIndex = -1;
        return;
    }

    /*
     *   NG���[�h���o���[�h��؂�ւ��� 
     * 
     */
    public void toggleNgWordDetectMode()
    {
        if ((objectFactory.preference).ngWordDetectMode == 0)
        {
            // NG���[�h���o���[�h�ɐ؂�ւ���
            (objectFactory.preference).ngWordDetectMode = 1;
        }
        else
        {
            // �ʏ�\�����[�h�ɐ؂�ւ���
            (objectFactory.preference).ngWordDetectMode = 0;            
        }
        return;
    }

    /*
     *   ���C�ɓ���ꗗ�\�����[�h��؂�ւ��� 
     * 
     */
    public void toggleFavoriteShowMode()
    {
        if ((objectFactory.preference).favoriteShowMode == 0)
        {
            // ���C�ɓ���ꗗ�\�����[�h�ɐ؂�ւ���
            (objectFactory.preference).favoriteShowMode = 1;
        }
        else
        {
            // �ʏ�\�����[�h�ɐ؂�ւ���
            (objectFactory.preference).favoriteShowMode = 0;            
        }
        return;
    }

    /**
     * ���C�ɓ���\�����x����ݒ肷��
     * 
     * @param level
     */
    public void setFavoriteShowLevel(int level)
    {
        (objectFactory.preference).favoriteShowLevel = level;
        return;
    }
    
    /*
     *   NG���[�h���o���[�h�̐ݒ����������
     * 
     */
    public boolean getNgWordDetectMode()
    {
        if ((objectFactory.preference).ngWordDetectMode == 0)
        {
            return (false);
        }
        return (true);
    }

    /**
     * �X�������擾���[�h�̐ݒ���擾����
     * 
     * @return
     */    
    public boolean getDivideGetHttp()
    {
        return ((objectFactory.preference).divideToGetHttp);

    }
    /**
     * �X�������擾���[�h��ݒ肷��
     * 
     * @return
     */    
    public void setDivideGetHttp(boolean isSet)
    {
        (objectFactory.preference).divideToGetHttp = isSet;
        return;
    }

    /**
     * �L�[���̓��[�h��ύX����
     * 
     * @param mode
     */
    public void toggleNumberInputMode(boolean mode)
    {
        boolean setMode = false;
        if (mode == true)
        {
            setMode = false;
        }
        else
        {
            setMode = true;
        }
        
        (objectFactory.listForm).setNumberInputMode(setMode);
        (objectFactory.viewForm).setNumberInputMode(setMode);
        
        return;
    }
    
    /**
     * �X�N���[�����[�h�̐ݒ���擾����
     * 
     * @return
     */
    public boolean getViewScreenMode()
    {
        return ((objectFactory.preference).viewScreenMode);
    }

    /**
     * �X�N���[�����[�h�̐ݒ��ύX����
     * 
     * @return
     */
    public void setViewScreenMode(boolean mode)
    {
        (objectFactory.preference).viewScreenMode = mode;
    }
    
    /**
     * Proxy URL��(preference��)�ݒ肷��
     * @param scene
     * @param proxyUrl
     * @param selection
     */
    public void setProxyUrl(int scene, String proxyUrl, int selection)
    {
        (objectFactory.preference).proxyUrl       = proxyUrl;
        (objectFactory.preference).usedProxyScene = selection;
        return;
    }

    /*
     *   proxy URL���擾����
     * 
     */
    public String getProxyUrl()
    {
        return ((objectFactory.preference).proxyUrl);
    }

    /*
     *   proxy���p�V�[�����擾����
     * 
     */
    public int getUsedProxyScene()
    {
        return ((objectFactory.preference).usedProxyScene);
    }
    
    /*
     *   ���C�ɓ���ꗗ�\�����[�h�̐ݒ����������
     * 
     */
    public boolean getFavoriteShowMode()
    {
        if ((objectFactory.preference).favoriteShowMode == 0)
        {
            return (false);
        }
        return (true);
    }

    /**
     * ���݂̂��C�ɓ���(�\��)���x�����擾����
     * 
     * @return
     */
    public int getFavoriteShowLevel()
    {
        if ((objectFactory.preference).favoriteShowMode == 0)
        {
            return (-1);
        }
        return ((objectFactory.preference).favoriteShowLevel);
    }

    /**
     *  pic.to�ϊ����s��
     * @return
     */
    public boolean getUsePicToConvert()
    {
        return ((objectFactory.preference).usePicToConvert);
    }
    
    /**
     *  pic.to�ϊ����s��
     * @return
     */
    public void setUsePicToConvert(boolean isSet)
    {
        (objectFactory.preference).usePicToConvert = isSet;
    }

    /**
     *  �����I�Ɏ��O�̊����ϊ����s��
     * @return
     */
    public boolean getForceKanjiConvertMode()
    {
        return ((objectFactory.preference).forceSelfKanjiConvert);
    }
    
    /**
     *  �����I�Ɏ��O�̊����ϊ����s��
     * @return
     */
    public void setForceKanjiConvertMode(boolean isSet)
    {
        (objectFactory.preference).forceSelfKanjiConvert = isSet;
    }

    /**
     *  �摜�擾���AGoogle�̃v���L�V���g�p���邩�ǂ�������������
     * @return �g�p���� <code>true</code> / �g�p���Ȃ� <code>false</code>
     */
    public boolean getWX220JUserAgent()
    {
        return ((objectFactory.preference).useWX220JUserAgent);
    }
    
    /**
     *  �摜�擾���AGoogle�̃v���L�V�𗘗p���邩�ǂ�����ݒ肷��
     * @param isUse google�̃v���L�V�𗘗p���邩�ǂ���
     */
    public void setWX220JUserAgent(boolean isUse)
    {
        (objectFactory.preference).useWX220JUserAgent = isUse;
    }

    /**
     *  �摜�擾���A�L���b�V���t�@�C��������΂���𗘗p����ݒ����������
     * @return �g�p���� <code>true</code> / �g�p���Ȃ� <code>false</code>
     */
    public boolean getUseCachePictureFile()
    {
        return ((objectFactory.preference).useCachePicFile);
    }
    
    /**
     *  �摜�擾���A�L���b�V���t�@�C��������΂���𗘗p����ݒ��ݒ肷��
     * @param isUse �L���b�V���t�@�C�����g�p���邩�ǂ���
     */
    public void setUseCachePictureFile(boolean isUse)
    {
        (objectFactory.preference).useCachePicFile = isUse;
    }

    /**
     *  �J�[�\���̓������y�[�W����ɂ��邩�ǂ���
     * @return �g�p���� <code>true</code> / �g�p���Ȃ� <code>false</code>
     */
    public boolean getCursorSingleMovingMode()
    {
        return ((objectFactory.preference).cursorSingleMovingMode);
    }
    
    /**
     *  �J�[�\���̓������y�[�W����ɂ��邩�ǂ����ύX����
     * @param isUse <code>true</code> �y�[�W����ɂ��� / <code>false</code> �y�[�W����ɂ��Ȃ�
     */
    public void setCursorSingleMovingMode(boolean isUse)
    {
        (objectFactory.preference).cursorSingleMovingMode = isUse;
    }
    
    /**
     *  Google�̃v���L�V���g�p���邩�ǂ�������������
     * @return �g�p���� <code>true</code> / �g�p���Ȃ� <code>false</code>
     */
    public boolean getUseGoogleProxy()
    {
        return ((objectFactory.preference).useGoogleProxy);
    }
    
    /**
     *  Google�̃v���L�V�𗘗p���邩�ǂ�����ݒ肷��
     * @param isUse google�̃v���L�V�𗘗p���邩�ǂ���
     */
    public void setUseGoogleProxy(boolean isUse)
    {
        (objectFactory.preference).useGoogleProxy = isUse;
    }
    
    /*
     *   ���݂̃X���C���f�b�N�X��ݒ肷��
     *
     *       @param  index �F �X���C���f�b�N�X�ԍ�
     */
    public void setSelectedSubjectIndex(int index)
    {
        if (index < 0)
        {
            index = 0;
        }
        (objectFactory.preference).selectedSubjectIndex = index;
        return;
    }

    /*
     *  ���݂̃��b�Z�[�W�ԍ���ݒ肷��
     * 
     *       @param  index �F ���ݕ\�����̃��b�Z�[�W�ԍ�
     */
    public void setSelectedMessageNumber(int index)
    {
        if (index < 0)
        {
            index = 0;
        }
        (objectFactory.preference).selectedMessageNumber = index;
        return;
    }

    /**
     *   ���b�Z�[�W�ԍ����L������
     * 
     * @param messageNumber
     */
    public void setMessageNumber(int messageNumber)
    {
        (objectFactory.preference).currentScreenNumber = messageNumber;
        return;
    }    
    
    /*
     *  �Q�Ɖ�ʂ̃t�H���g�T�C�Y���L������
     * 
     */
    public void setViewFontSize(int fontSize)
    {
        (objectFactory.preference).viewFormFontSize = fontSize;
        return;
    }

    /*
     *  �Q�Ɖ�ʂ̃t�H���g�T�C�Y���擾����
     * 
     */
    public int getViewFontSize()
    {
        return ((objectFactory.preference).viewFormFontSize);
    }

    /*
     *  �Q�Ɖ�ʂ̃t�H���g�F���L������
     * 
     */
    public void setViewFontColor(int foreColor, int backColor)
    {
        (objectFactory.preference).viewFormBackColor = backColor;
        (objectFactory.preference).viewFormForeColor = foreColor;
        return;
    }

    /*
     *  �Q�Ɖ�ʂ̔w�i�F���擾����
     * 
     */
    public int getViewBackColor()
    {
        return ((objectFactory.preference).viewFormBackColor);
    }

    /*
     *  �Q�Ɖ�ʂ̃t�H���g�F���擾����
     * 
     */
    public int getViewForeColor()
    {
        return ((objectFactory.preference).viewFormForeColor);
    }    
    
    /*
     *  �ꗗ��ʂ̃t�H���g�T�C�Y���L������
     * 
     */
    public void setListFontSize(int fontSize)
    {
        (objectFactory.preference).listFormFontSize = fontSize;
        return;
    }

    /**
     *  ���X���擾���Ɏ����I�ɂ��C�ɓ���ɓo�^���邩�ǂ������m�F����
     * 
     * 
     * @return
     */
    public boolean getAutoEntryFavorite()
    {
        return ((objectFactory.preference).autoEntryFavorite);
    }

    /**
     *  ���X���擾���Ɏ����I�ɂ��C�ɓ���ɓo�^���邩�ǂ�����ݒ肷��
     * 
     * 
     * @return
     */
    public void setAutoEntryFavorite(boolean isSet)
    {
        (objectFactory.preference).autoEntryFavorite = isSet;
        return;
    }

    /**
     *  �u�t�@�C���_�E�����[�h���̃f�B���N�g����a2B���[�g�f�B���N�g���ɂ���v
     * 
     * 
     * @return
     */
    public boolean getSaveAtRootDirectory()
    {
        return ((objectFactory.preference).saveAtRootDirectory);
    }

    /**
     *  �u�t�@�C���_�E�����[�h���̃f�B���N�g����a2B���[�g�f�B���N�g���ɂ���v
     * 
     * 
     * @return
     */
    public void setSaveAtRootDirectory(boolean isSet)
    {
        (objectFactory.preference).saveAtRootDirectory = isSet;
        return;
    }
    
    /**
     *  a2B���N��������u���C�ɓ���ꗗ�v����͂��߂邩�ǂ������m�F����
     * 
     * 
     * @return
     */
    public boolean getLaunchAsFavoriteList()
    {
        if ((objectFactory.preference).openCurrentScreen < 0)
        {
            return (true);
        }
        return (false);
    }
    
    /**
     *  a2B�N�����ɂ́A�K���u���C�ɓ���v�ꗗ����͂��߂�ݒ��؂�ւ���
     * 
     * 
     */
    public void setLaunchAsFavoriteList(boolean isSet)
    {
        int value = 0;
        if (isSet == true)
        {
            value = -1;
        }
        (objectFactory.preference).openCurrentScreen = value;
    }

    /**
     * �X���^�C�g����ʗ̈�ɕ\������
     * 
     * @return
     */
    public boolean getShowTitleAnotherArea()
    {
        return ((objectFactory.preference).isShowTitleBusyArea);
    }

    /**
     * �X���^�C�g����ʗ̈�ɕ\������
     * 
     * @return
     */
    public void setShowTitleAnotherArea(boolean setting)
    {
        (objectFactory.preference).isShowTitleBusyArea = setting;
        return;
    }

    /**
     * subject.txt���o�b�N�A�b�v���邩�ǂ���...
     * 
     * @return
     */
    public boolean getBackupSubjectTxt()
    {
        return ((objectFactory.preference).backupSubjectTxt);
    }

    /**
     * subjectTxt���o�b�N�A�b�v���邩�ǂ���...
     * 
     * @return
     */
    public void setBackupSubjectTxt(boolean setting)
    {
        (objectFactory.preference).backupSubjectTxt = setting;
        return;
    }

    /**
     * ���C�ɓ���o�b�N�A�b�v�ȗ����[�h���擾����
     * 
     * @return
     */
    public boolean getIgnoreFavoriteBackup()
    {
        return ((objectFactory.preference).ignoreFavoriteBackup);
    }

    /**
     * ���C�ɓ���o�b�N�A�b�v�ȗ����[�h��ݒ肷��
     * 
     * @return
     */
    public void setIgnoreFavoriteBackup(boolean setting)
    {
        (objectFactory.preference).ignoreFavoriteBackup = setting;
        return;
    }
    
    /**
     * Web�u���E�U�N�����[�h���擾����
     * 
     * @return
     */
    public boolean getLaunchMode()
    {
        return ((objectFactory.preference).afterLaunchWebAction);
    }

    /**
     * Web�u���E�U�N�����[�h��ݒ肷��
     * 
     * @return
     */
    public void setLaunchMode(boolean setting)
    {
        (objectFactory.preference).afterLaunchWebAction = setting;
        return;
    }
    
    /**
     * �t�@���N�V�����L�[(�����O�L�[)�̋@�\��ݒ肷��
     * 
     * @param scene
     * @param featureNumber
     * @return
     */
    public void setFunctionKeyFeature(int scene, int featureNumber)
    {
        if (scene == SCENE_LISTFORM)        
        {
            (objectFactory.preference).listFormFunctionKey = featureNumber;
        }
        // if (scene == SCENE_VIEWFORM)
        (objectFactory.preference).viewFormFunctionKey = featureNumber;
        
        return;
    }
    
    /**
     * �t�@���N�V�����L�[(�����O�L�[)�̋@�\����������
     * 
     * @param scene
     * @return
     */
    public int getFunctionKeyFeature(int scene)
    {
        if (scene == SCENE_LISTFORM)        
        {
            return ((objectFactory.preference).listFormFunctionKey);            
        }
        // if (scene == SCENE_VIEWFORM)
        return ((objectFactory.preference).viewFormFunctionKey);
    }    
    
    /*
     *  �ꗗ��ʂ̃t�H���g�T�C�Y���擾����
     * 
     */
    public int getListFontSize()
    {
        return ((objectFactory.preference).listFormFontSize);
    }

    /**
     * ������I�����Ԃ��擾����
     * @return
     */
    public int getWatchDogTime()
    {
        return ((objectFactory.preference).watchDogMinutes);
    }

    /**
     *  �X�������擾�T�C�Y���擾����
     * 
     * @return
     */
    public int getDivideGetSize()
    {
        return ((objectFactory.preference).divideCommunicationSize);
    }

    /**
     *  �X�������擾�T�C�Y��ݒ肷��
     */
    public void setDivideGetSize(int size)
    {
        (objectFactory.preference).divideCommunicationSize = size;
    }

    /**
     * ������I�����Ԃ�ݒ肷��(0:������A�P��:��)
     * @param time
     */
    public void setWatchDogTime(int time)
    {
        (objectFactory.preference).watchDogMinutes = time;
        return;
    }

    /**
     * EUC�����R�[�h��UTF8�ɕϊ�����
     * @param eucString
     * @param startIndex
     * @param length
     * @return
     */
    public String eucToUtf8(byte[] eucString, int startIndex, int length)
    {
        return ((objectFactory.writeForm).eucToUtf8(eucString, startIndex, length));
    }

    /**
     * Shift JIS�����R�[�h��UTF8�ɕϊ�����
     * @param sjString
     * @param startIndex
     * @param length
     * @return
     */
    public String sjToUtf8(byte[] sjString, int startIndex, int length)
    {
        return ((objectFactory.writeForm).sjToUtf8(sjString, startIndex, length));
    }
    
    /*
     *  �������݉�ʂ̖��O�����L�^����
     * 
     */
    public void setWriteName(String name)
    {
        (objectFactory.preference).writeName = name;
        return;
    }
    
    /*
     *  �������݉�ʂ̖��O�����擾����
     * 
     */
    public String getWriteName()
    {
        return ((objectFactory.preference).writeName);
    }

    /**
     * �f�[�^�x�[�X�A�N�Z�X���[�h���L������
     * @param isDatabase
     */
    public void setOnlyDatabaseAccess(boolean isDatabase)
    {
        (objectFactory.preference).onlyDatabaseAccess = isDatabase;
        return;
    }

    /*
     *   ���݂̃J�e�S���C���f�b�N�X���擾����
     *   
     */
    public int getCurrentCategoryIndex()
    {
        return ((objectFactory.preference).currentBbsCategoryIndex);
    }
    
    /**
     * AA���[�h�̃y�[�W�ړ����擾
     * 
     * @return
     */
    public int getAAwidthMargin()
    {
        return ((objectFactory.preference).aaWidth);
    }

    /**
     * AA���[�h�̃y�[�W�ړ����ݒ�
     * 
     * @return
     */
    public void setAAwidthMargin(int width)
    {
        int margin = width % 512;
        (objectFactory.preference).aaWidth = margin;
        (objectFactory.viewForm).setAAmodeWidthMargin(margin);
        return;
    }

    /*
     *   ���݂̔C���f�b�N�X���擾����
     *   
     */
    public int getCurrentBbsIndex()
    {
        return ((objectFactory.preference).currentBbsIndex);
    }

    /*
     *   ���݂̃X���C���f�b�N�X��ݒ肷��
     *   
     */
    public int getSelectedSubjectIndex()
    {
        return ((objectFactory.preference).selectedSubjectIndex);
    }

    /*
     *   ���݂̃��b�Z�[�W�ԍ���ݒ肷��
     *   
     */
    public int getSelectedMessageNumber()
    {
        return ((objectFactory.preference).selectedMessageNumber);
    }

    /*
     *   �f�[�^�Ǘ��p�x�[�X�f�B���N�g�����擾����
     * 
     * 
     */
    public String getBaseDirectory()
    {
        return ((objectFactory.preference).getBaseDirectory());
    }

    /*
     *   ���O�C�����̐ݒ�(Be@2ch)
     * 
     * 
     */
    public void setBeName(String name)
    {
        (objectFactory.preference).beLoginName = name;
        return;
    }
    
    /*
     *   ���O�C���p�p�X���[�h�̐ݒ� (Be@2ch)
     * 
     * 
     */
    public void setBePassword(String password)
    {
        (objectFactory.preference).beLoginPassword = password;
        return;
    }

    /*
     *   ���O�C���N�b�L�[�̕ۑ� (Be@2ch)
     * 
     * 
     */
    public void setBeCookie(String value)
    {
        Date  date      = new Date();
        (objectFactory.preference).beLoginDateTime = date.getTime();
        (objectFactory.preference).beCookie = value;
        
        date = null;
        return;
    }

    /**
     *  �L�[���s�[�g��F�����鎞�ԊԊu��ݒ�
     * @param ms
     */
    public void setKeyRepeatTime(int ms)
    {
        (objectFactory.viewForm).setKeyRepeatDelay(ms);
        (objectFactory.preference).keyRepeatDelayTime = ms;
    }

    /**
     *  �L�[���s�[�g��F�����鎞�ԊԊu���擾
     * @return
     */
    public int getKeyRepeatTime()
    {
    	return ((objectFactory.preference).keyRepeatDelayTime);
    }
    
    /*
     *   ���O�C���N�b�L�[�̎擾 (Be@2ch)
     * 
     * 
     */
    public String getBeCookie()
    {
        Date  date      = new Date();
        long time      = date.getTime() - (24 * 60 * 60 * 1000);
        
        if (((objectFactory.preference).beLoginDateTime == 0)||
            (time >= (objectFactory.preference).beLoginDateTime))
        {
            // ���O�C����24���Ԍo�߁A�܂��͖����O�C�����ɂ�cookie�Ƃ���null����������
            (objectFactory.preference).beLoginDateTime = 0;
            return (null);
        }
        return ((objectFactory.preference).beCookie);
    }

    /*
     *   ���O�C�����̎擾(Be@2ch)
     * 
     * 
     */
    public String getBeName()
    {
        return ((objectFactory.preference).beLoginName);
    }
    
    /**
     * BE�\�����[�h�̐ݒ���m�F����
     * 
     * @return
     */
    public int getViewKanjiMode()
    {
    	return ((objectFactory.preference).viewKanjiMode);
    }
    
    /**
     * �����\�����[�h�̐ݒ��ύX����
     * 
     * @return
     */
    public void toggleViewKanjiMode(int kanjiMode)
    {
        int mode = (objectFactory.preference).viewKanjiMode;
        if (kanjiMode == a2BsubjectDataParser.PARSE_TOGGLE)
        {
            mode = (mode % 4) + 1;
        }
        else
        {
        	mode = kanjiMode % 5;
        }
        (objectFactory.preference).viewKanjiMode = mode;
        return;
    }
    
    /*
     *   ���O�C���p�p�X���[�h�̎擾 (Be@2ch)
     * 
     * 
     */
    public String getBePassword()
    {
        return ((objectFactory.preference).beLoginPassword);
    }

    /**
     *  �t�@�C���ۑ��f�B���N�g������������
     * @return �t�@�C���ۑ��f�B���N�g��
     */
    public String getFileSaveDirectory()
    {
        if (((objectFactory.preference).fileSaveDirectory).indexOf("file:///") >= 0)
        {
            return ((objectFactory.preference).fileSaveDirectory);
        }
        return (getBaseDirectory() + (objectFactory.preference).fileSaveDirectory);
    }

    /**
     *  �t�@�C���ۑ��f�B���N�g����ݒ肷��
     * @param fileDirectory �t�@�C���ۑ��f�B���N�g��
     */
    public void setFileSaveDirectory(String fileDirectory)
    {
        (objectFactory.preference).fileSaveDirectory = fileDirectory;
        return;
    }
  
    /**
     *  �������ݗp�g���L�[���[�h�ݒ�
     */
    public void setWriteKeyString(String value)
    {
        (objectFactory.preference).writeKeyString = value;
        return;
    }

    /**
     *  �������ݗp�g���L�[���[�h�ݒ�
     */
    public String getWriteKeyString()
    {
    	return ((objectFactory.preference).writeKeyString);
    }

    /**
     *  �������ݗpCookie�ݒ�
     */
    public String getWriteCookie()
    {
    	return ((objectFactory.preference).writeCookie);
    }

    /**
     *  �������ݗpCookie�ݒ�
     */
    public void setWriteCookie(String value)
    {
        (objectFactory.preference).writeCookie = value;
        return;
    }

    /*
     *   ���O�C�����̐ݒ�
     * 
     * @param name ���O�C������
     * 
     */
    public void setOysterName(String name)
    {
        (objectFactory.preference).oysterLoginName = name;
        return;
    }

    /*
     *   ���O�C���p�X���[�h�̐ݒ�
     * 
     * 
     */
    public void setOysterPassword(String pass)
    {
        (objectFactory.preference).oysterLoginPassword = pass;
        return;
    }
    
    /*
     *   ���O�C�����̎擾
     * 
     * 
     */
    public String getOysterName()
    {
        return ((objectFactory.preference).oysterLoginName);
    }
    
    /*
     *   ���O�C���p�p�X���[�h�̎擾
     * 
     * 
     */
    public String getOysterPassword()
    {
        return ((objectFactory.preference).oysterLoginPassword);
    }
    
    /**
     *  �����O�C���̊J�n
     *
     */
    public void startOysterLogin()
    {
        // �����O�C�����s�J�n��\������
        (objectFactory.listForm).setInformationMessage(0, "2ch���O�C�����s��...");

        Thread thread = new Thread()
        {
            public void run()
            {
                // ���O�C�������s����
                String name = (objectFactory.preference).oysterLoginName;
                String pass = (objectFactory.preference).oysterLoginPassword;
                boolean result = (objectFactory.loginUtility).doOysterLogin(name, pass);
                finishOysterLogin(result);
                System.gc();
            }
        };
        thread.start();
    
    }

    /**
     *  Be@2ch���O�C���̊J�n
     *
     */
    public void startBe2chLogin()
    {
        // Be@2ch���O�C�����s�J�n��\������
        (objectFactory.listForm).setInformationMessage(0, "BE@2ch���O�C����...");

        Thread thread = new Thread()
        {
            public void run()
            {
                // ���O�C�������s����
                String name = getBeName();
                String pass = getBePassword();
                boolean result = (objectFactory.loginUtility).doBe2chLogin(name, pass);
                finishBe2chLogin(result);
                System.gc();
            }
        };
        thread.start();
    
    }

    /**
     * ���[�U�[�G�[�W�F���g��ݒ肷��
     * 
     * @param userAgent
     */
    public void setUserAgent(String userAgent)
    {
        if (userAgent.length() == 0)
        {
            (objectFactory.preference).changedUserAgent = "Monazilla/1.00  NNsi/1.10(a2B)";    
        }
        else
        {
            (objectFactory.preference).changedUserAgent = userAgent;            
        }
        return;
    }
    
    /**
     * ���[�U�[�G�[�W�F���g���擾����
     * 
     * @return
     */
    public String getUserAgent(boolean isNormalBrowser)
    {
        if (isNormalBrowser == true)
        {
/*
            if ((objectFactory.preference).useWX220JUserAgent == false)
            {
                return ("Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3");
            }
            return ("Mozilla/3.0(WILLCOM;JRC/WX220J/2;1.0/0100/C50) CNF/2.0");
*/
            if ((objectFactory.preference).usedProxyScene == PROXY_WORKAROUND_WX310)
            {
                if ((objectFactory.preference).useWX220JUserAgent == false)
                {
                    return ("Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3");
                }
                return ("Mozilla/3.0(WILLCOM;JRC/WX220J/2;1.0/0100/C50) CNF/2.0");
            }
            if ((((objectFactory.preference).changedUserAgent).indexOf("Monazilla")) >= 0)
            {
                // UA���`���[�j���O����Ă��Ȃ��ꍇ�A�l�t�����U������...
                if ((objectFactory.preference).useWX220JUserAgent == false)
                {
                    return ("Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3");
                }
                return ("Mozilla/3.0(WILLCOM;JRC/WX220J/2;1.0/0100/C50) CNF/2.0");
            }
        }
        String userAgent = (objectFactory.preference).changedUserAgent;
        if ((objectFactory.preference).oysterLoginDateTime != 0)
        {
            // �L���AUser Agent
            userAgent = (objectFactory.preference).oysterLoginUserAgent;
        }
        return (userAgent);
    }

    /**
     *   Be@2ch �̃N�b�L�[�����L�^����
     * 
     * @param setCookie
     */
    public void setBe2chCookie(String setCookie)
    {
        be2chCookie = setCookie;
    }

    /**
     *  Be@2ch�̃N�b�L�[������������
     * @param scene
     * @return
     */
    public String getBe2chCookie(int scene)
    {
        return (be2chCookie);
    }

    /**
     * �Z�b�V����ID���擾����
     * 
     * @param scene
     * @return
     */
    public String getOysterSessionId(int scene)
    {
        // ���ݎ������擾����
        String sessionId = null;
//      Date   curTime   = new Date();
//      long  diffTime  = curTime.getTime() - (objectFactory.preference).oysterLoginDateTime;
//
//        // ���O�C�����͗L�����ǂ����m�F����B
//        if ((diffTime > 0)&&(diffTime < (24 * 60 * 60 * 1000)))
        if ((objectFactory.preference).oysterLoginDateTime != 0)
        {
            // �L���A�m�F����
            sessionId = (objectFactory.preference).oysterLoginSessionId;
        }
        else
        {
            // expired...
            sessionId = null;
            (objectFactory.preference).oysterLoginDateTime = 0;
        }
        return (sessionId);
    }

    /**
     * �����O�C�������L������
     * 
     * @param allString
     * @param sessionId
     */
    public void setOysterSessionId(String userAgent, String sessionId)
    {
        // ���ݎ������擾����
        Date curTime = new Date();

        // �����O�C�����̏����L������...
        (objectFactory.preference).oysterLoginUserAgent    = userAgent;
        (objectFactory.preference).oysterLoginSessionId    = sessionId;
        (objectFactory.preference).oysterLoginDateTime     = curTime.getTime(); // �~���Z�J���h(24 * 60 * 60 * 1000)
    }
    
    /**
     *  �����O�C������
     *  
     * @return
     */
    public boolean finishOysterLogin(boolean result)
    {
        // ���O�C�����ʂ�\��...
        String message;
        String msg  = (objectFactory.loginUtility).getResultString();
        if (result == true)
        {
            message = "2ch���O�C������ " + msg;
        }
        else
        {
            message = "2ch���O�C�����s:" + msg;
        }

        // �����O�C�����s����
        (objectFactory.listForm).setInformationMessage(0, message);
        return (false);
    }

    /**
     *  Be���O�C������
     *  
     * @return
     */
    public boolean finishBe2chLogin(boolean result)
    {
        // ���O�C�����ʂ�\��...
        String message;
        String msg  = (objectFactory.loginUtility).getResultString();
        if (result == true)
        {
            message = "BE���O�C������ " + msg;
        }
        else
        {
            message = "BE���O�C�����s:" + msg;
        }

        // Be@2ch���O�C�����s����
        (objectFactory.listForm).setInformationMessage(0, message);
        return (false);
    }

    /**
     * �����~�܂�
     * 
     * @param time (ms)
     */
    public void sleepTime(long time)
    {
        try
        {
            Thread.sleep(time);
        }
        catch (Exception e)
        {
                
        }
        return;
    }
    
    /**
     *  �C���[�W�t�@�C��(png�`��)��ǂݏo���A�������ɓW�J����
     * 
     * @param fileName �t�@�C����
     * @return         �C���[�W�f�[�^
     */
    public Image loadJpegThumbnailImageFromFile(String fileName)
    {
        Image          image      = null;
        FileConnection connection = null;
        InputStream    in         = null;
        byte[]        buffer     = null;
        try
        {
            ///// ��x���ǂ݁A�A�A�T���l�[���摜�����邩�T��
            // �t�@�C���̃I�[�v��
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);
//            int size   = (int) connection.fileSize();

            // �f�[�^���ŏ����� size���ǂݏo��...
            in      = connection.openInputStream();
            int startThumbnailOffset = 0;
            int endThumbnailOffset = 0;
            int dataReadSize = 0;
            int currentReadSize = 0;
            byte[] tempBuffer = new byte[TEMP_READ_SIZE + 8];
            tempBuffer[0] = 0x00;
            while (currentReadSize >= 0)
            {
                int index = 0;
                currentReadSize = in.read(tempBuffer, 1, TEMP_READ_SIZE);
                if (dataReadSize == 0)
                {
                    // �擪�}�[�J���΂�
                    index = index + 3;
                }
                while (index < currentReadSize)
                {
                    if ((tempBuffer[index] == (byte) 0xff)&&(tempBuffer[index + 1] == (byte) 0xd8))
                    {
                        startThumbnailOffset = index + dataReadSize;
                    }
                    if ((tempBuffer[index] == (byte) 0xff)&&(tempBuffer[index + 1] == (byte) 0xd9))
                    {
                        endThumbnailOffset = index + dataReadSize;
                        if (startThumbnailOffset != 0)
                        {
                            currentReadSize = -1;
                            break;
                        }
                    }
                    index++;
                }
                if ((currentReadSize >= 0)&&(index == currentReadSize))
                {
                    tempBuffer[0] = tempBuffer[currentReadSize - 1];
                }
                dataReadSize = dataReadSize + currentReadSize;
            }
            // �t�@�C���̃N���[�Y
            in.close();
            connection.close();
            in = null;
            connection = null;
            tempBuffer = null;

            ///// ��x���ǂ݁A�A�A�T���l�[���摜�����邩�T��
            // �t�@�C���̃I�[�v��
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);

            // �f�[�^���ŏ����� size���ǂݏo��...
            in      = connection.openInputStream();
            int bufferSize = endThumbnailOffset - startThumbnailOffset + 10;
            if (bufferSize < 0)
            {
                bufferSize = (int) connection.fileSize();
            }
            if (startThumbnailOffset < 0)
            {
                startThumbnailOffset = 0;
            }
            buffer = new byte[bufferSize + 4];

            // �t�@�C���̐擪�܂œǂݏo��...
            if (startThumbnailOffset != 0)
            {
                //in.skip(startThumbnailOffset);

                // in.skip(startThumbnailOffset)�̏��������܂�ɂ��x�������̂ŁA�A�A�J���ǂݏ����B                
                int startOffset = startThumbnailOffset - 1;
                while (startOffset > 0)
                {
                    int readSize = 0;
                    if (startOffset > bufferSize)
                    {
                        readSize = in.read(buffer, 0, bufferSize);
                    }
                    else
                    {
                        readSize = in.read(buffer, 0, startOffset);
                    }
                    startOffset = startOffset - readSize;
                }
            }
            
            // �f�[�^�̓ǂݏo������...
            currentReadSize = in.read(buffer, 0, bufferSize);
            
            // �t�@�C���̃N���[�Y
            in.close();
            connection.close();

            // �ǎ��f�[�^�Ƃ��ăC���[�W�ɕϊ�...
            image = Image.createImage(buffer, 0, bufferSize);
            
            // �o�b�t�@���N���A����
            buffer = null;

        }
        catch (OutOfMemoryError e)
        {
            (objectFactory.viewForm).setBusyMessage("MemOver", false);

            // �������I�[�o�t���[��O�����I�I�I
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
        catch (Exception e)
        {
            // ��O�����I�I�I
            (objectFactory.viewForm).setBusyMessage("Err:" + e.getMessage(), false);
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

    /**
     * �f�[�^���X�N���b�`�o�b�t�@�ɓǂݍ���...
     * 
     * @param fileName �t�@�C����
     */
    private void loadScratchBufferFromFile(String fileName)
    {
        FileConnection connection = null;
        InputStream    in         = null;
        try
        {
            // �t�@�C���̃I�[�v��
            connection  = (FileConnection) Connector.open(getBaseDirectory() + fileName, Connector.READ);
            int size   = (int) connection.fileSize();

            // �f�[�^���ŏ����� size���ǂݏo��...
            in      = connection.openInputStream();
            scratchBuffer = new byte[size];
            in.read(scratchBuffer, 0, size);

            // �t�@�C���̃N���[�Y
            in.close();
            connection.close();
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
            scratchBuffer  = null;
        }
        System.gc();
    }

    /**
     *  �C���[�W�t�@�C��(png�`��)��ǂݏo���A�������ɓW�J����
     * 
     * @param fileName �t�@�C����
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
            connection  = (FileConnection) Connector.open(getBaseDirectory() + fileName, Connector.READ);
            int size   = (int) connection.fileSize();

            // �f�[�^���ŏ����� size���ǂݏo��...
            in      = connection.openInputStream();
            buffer  = new byte[size + 4];
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
    /**
     * �t�@�C�����̃R�s�[���������s����
     * 
     * @param destFileName
     * @param srcFileName
     */
    public void doCopyFile(String destFileName, String srcFileName)
    {
        (objectFactory.fileSelector).doCopyFileMain(destFileName, srcFileName);
    }

    /**
     *  �t�@�C���̍폜�������s��
     * @param fileName
     */
    public void doDeleteFile(String fileName)
    {
        (objectFactory.fileSelector).deleteFile(fileName, false);
    }
    
    /**
     *  �t�@�C���̓��e��\������...
     * @param fileName �t�@�C����
     * @param previewMode �v���r���[���[�h
     */
    public void doPreviewFile(String fileName, boolean useForm, int previewMode)
    {
        // �e�N���X�Ƀf�[�^���R�s�[�ł����ꍇ...
        if (prepareScratchBuffer(fileName, TEMP_PREVIEW_SIZE) == true)
        {
            // �f�[�^�v���r���[�����s
            previewMessage(useForm, previewMode);
        }
        return;
    }

    /** 
     *   �t�@�C����ǂ�Őe�̃X�N���b�`�o�b�t�@�̏������s��
     * 
     */
    public boolean prepareScratchBuffer(String dataFile, int limitSize)
    {
        FileConnection connection    = null;
        InputStream    in            = null;
        byte[]         buffer        = null;
        int            dataReadSize  = 0;

        try
        {
            connection   = (FileConnection) Connector.open(dataFile, Connector.READ);
            int size     = (int) connection.fileSize();

            // �t�@�C���T�C�Y���傫���ꍇ�A�T�C�Y������p�ӂ���
            if (size > limitSize)
            {
                size = limitSize;
            }

            //////  �f�[�^���ŏ����� size���ǂݏo��...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size];
            dataReadSize = in.read(buffer, 0, size);

            in.close();
            connection.close();
            
            // �t�@�C���f�[�^��e�̃X�N���b�`�o�b�t�@�փR�s�[����
            copyToScratchBuffer(buffer, 0, dataReadSize);
        }
        catch (Exception e)
        {
            try
            {
                if (in != null)
                {
                    in.close();
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
            buffer = null;
            return (false);
        }
        buffer = null;
        return (true);
    }

    /**
     * �f�B���N�g���̍쐬
     * @param directoryName �f�B���N�g���� (full path)
     */
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

    /**
     * �X�v���b�V���X�N���[��(�N�����)�̕\���N���X
     * 
     * @author MRSa
     *
     */
    public class a2BSplashScreen extends Canvas
    {
        Image image = null;  // �C���[�W�t�@�C���̏I��
        
        /**
         * �R���X�g���N�^(�X�v���b�V���X�N���[���ɕ\������C���[�W�����߂�)
         *
         */
        public a2BSplashScreen()
        {
            Random rand = new Random();

            int item = rand.nextInt(17);
//            int item = (rand.nextInt() % 17);

            // �C���[�W�t�@�C�������߂�
            String resource = "/res/a2B_splash.png";
            switch (item)
            {
              case 1:
                resource = "/res/a2B_splash1.png";
                break;

              case 2:
                resource = "/res/a2B_splash2.png";
                break;

              case 3:
                resource = "/res/a2B_splash3.png";
                break;

              case 4:
                resource = "/res/a2B_splash4.png";
                break;

              case 5:
                resource = "/res/a2B_splash5.png";
                break;

              case 6:
                resource = "/res/a2B_splash6.png";
                break;

              case 7:
                resource = "/res/a2B_splash7.png";
                break;

              case 8:
                resource = "/res/a2B_splash8.png";
                break;

              case 9:
                resource = "/res/a2B_splash9.png";
                break;

              case 10:
                resource = "/res/a2B_splashA.png";
                break;

              case 11:
                resource = "/res/a2B_splashB.png";
                break;

              case 12:
                resource = "/res/a2B_splashC.png";
                break;

              case 13:
                resource = "/res/a2B_splashD.png";
                break;

              case 14:
                resource = "/res/a2B_splashE.png";
                break;

              case 15:
                resource = "/res/a2B_splashF.png";
                break;

              case 16:
                resource = "/res/a2B_splash0.png";
                break;

              default:
                resource = "/res/a2B_splash.png";
                break;
            }
            rand = null;
            
            // �C���[�W�t�@�C����ǂݏo��
            try
            {
                image = Image.createImage(resource);
            }
            catch (Exception e)
            {
                image = null;
            }
        }

        /**
         *   �N����ʂ�\������
         *   
         */
        public void paint(Graphics g)
        {
            // ��ʂ̓h��Ԃ�
            g.setColor(255, 255, 255);
            g.fillRect(0, 0, getWidth(), getHeight());
            
            if (image == null)
            {
                String message = "a 2ch Browser (a2B)";

                // �^�C�g���̕\�� (����)
                g.setColor(32, 32, 32);
                Font font = Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_BOLD, Font.SIZE_LARGE);
                g.setFont(font);
                g.drawString(message, 0, (getHeight() / 2), (Graphics.LEFT | Graphics.TOP));
                return;
            }
            else
            {
                // �C���[�W�̕\�����W�����߂�
                int x = ((getWidth()  - image.getWidth())  / 2);
                if ((x + image.getWidth()) > getWidth())
                {
                    x = 0;
                }

                int y = ((getHeight() - image.getHeight()) / 2);
                if ((y + image.getHeight()) > getHeight())
                {
                    y = 0;
                }
                
                // �X�v���b�V���X�N���[���̃C���[�W��`��
                g.setColor(0);
                g.drawImage(image, x, y, (Graphics.LEFT | Graphics.TOP));
            }
            return;
        }

        /**
         *   �N����ʂ̌�n��
         *
         */
        public void finish()
        {
            // �K�x�R�����{...
            image = null;
            System.gc();
            
            return;
        }
    }
}
//
// --------------------------------------------------------------------------
//    a2BMain  --- a 2ch Browser ���C������
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
