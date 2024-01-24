import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.rms.RecordStore;
import javax.microedition.lcdui.Font;

public class a2BobjectFactory 
{
    private a2BMain                 parent          = null;  // �e�I�u�W�F�N�g

    public a2BPreferenceDatabase   preference       = null;  // �ݒ���ێ��N���X

    public a2BMessageListForm      listForm         = null;  // �ꗗ���
    public a2BMessageViewForm      viewForm         = null;  // �Q�Ɖ��
    public a2BMessageWriteForm     writeForm        = null;  // �������݉��
    public a2BfileSelector         fileSelector     = null;  // �t�@�C���Z���N�^���
    public a2BselectionForm        selectForm       = null;  // �f�[�^�I��p���
    public Alert                   dialogForm       = null;  // �x�����b�Z�[�W�_�C�A���O

    public a2BbbsCommunicator      bbsCommunicator  = null;  // �ꗗ�̉�́E�f�[�^�ʐM�N���X
    public a2BHttpCommunicator     httpCommunicator = null;  // HTTP�f�[�^�ʐM�N���X
    public a2BsubjectDataParser    subjectParser    = null;  // �X���ꗗ�̉�̓N���X
    public a2BFavoriteManager      favoriteManager  = null;  // ���C�ɓ���X���Ǘ��N���X
    public a2BloginHolder          loginUtility     = null;  // ���O�C�����[�e�B���e�B
    
    private a2BdialogListener      dialogListener   = null;  // �_�C�A���O�\�����̏���
    private a2Butf8Conveter        utf8Converter    = null;  // SHIFT JIS => UTF8�ϊ��N���X
    private a2BsjConverter         sjConverter      = null;  // UTF8 => SHIFT JIS�ϊ��N���X
    private a2BNgWordManager       ngWordManager    = null;  // ���[�J�����ځ[��L�[���[�h�Ǘ��N���X

    private final byte[] a2B_PREF_VERSION = { (byte) 4, (byte) 0 };  // preference DB�̃o�[�W����...

    private  boolean     baseDirSettingMode = false;  // �x�[�X�f�B���N�g���ݒ胂�[�h...

    private final int    a2B_VERSION_RECORDID     = 1; // preference DB�̃o�[�W�����i�[���R�[�h
    private final int    a2B_BASEDIR_RECORDID     = 2; // preference DB�̃x�[�X�f�B���N�g���i�[���R�[�h
    private final int    a2B_CURRENTBBS_RECORDID  = 3; // preference DB�̌���BBS�J�e�S���ݒ�
    private final int    a2B_OYSTERNAME_RECORDID  = 4;
    private final int    a2B_OYSTERPASS_RECORDID  = 5;
    private final int    a2B_WRITE_NAME_RECORDID  = 6;
    private final int    a2B_FONT_SIZE_RECORDID   = 7;
    private final int    a2B_PROXYURL_RECORDID    = 8;
    private final int    a2B_CURRENTLOAD_RECORDID = 9;
    private final int    a2B_BENAME_RECORDID      = 10;
    private final int    a2B_BEPASS_RECORDID      = 11;
    private final int    a2B_USERAGENT_RECORDID   = 12;
    private final int    a2B_FILESAVEDIR_RECORDID = 13;
    private final int    a2B_MEMOFILE_RECORDID    = 14;
    private final int    a2B_WRITEKEY_RECORDID    = 15;
    private final int    a2B_WRITECOOKIE_RECORDID = 16;
    
    // �R���X�g���N�^
    public a2BobjectFactory(a2BMain object)
    {
        parent = object;
    }

    // �I�u�W�F�N�g�̏���
    public boolean prepare()
    {
        // �_�C�A���O�̏���
        dialogForm = new Alert("");
        dialogListener = new a2BdialogListener(parent);
        dialogForm.setCommandListener(dialogListener);

        // �f�[�^�x�[�X�̏���
        prepareDatabase();

        // ��ʂ̕\������
        prepareScreen();        

        // �p�����[�^�̓ǂݏo�� (�X���b�h���N�����Ď��s)
//        boolean ret = restoreParameters();
        Thread thread = new Thread()
        {
            public void run()
            {
                baseDirSettingMode = restoreParameters();
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            //
        }
        thread = null;
        System.gc();

        // ��ʃT�C�Y�̐ݒ�(�\����������2)
        prepareScreenSize();
        
        // �����ϊ��������I�Ɏ��O�̂��̂��g���ꍇ...
        if (preference.forceSelfKanjiConvert == true)
        {
            utf8Converter.forceSelfKanjiConvertor();
        }
        
        return (baseDirSettingMode);
    }

    /*  
     *  �I�u�W�F�N�g���폜����
     *
     */
    public void cleanup()
    {
        backupParameters();

        favoriteManager  = null;
        ngWordManager    = null;
        utf8Converter    = null;
        sjConverter      = null;
        httpCommunicator = null;
        bbsCommunicator  = null;
        subjectParser    = null;
        loginUtility     = null;

        viewForm         = null;
        listForm         = null;
        writeForm        = null;
        fileSelector     = null;
        selectForm       = null;
        dialogForm       = null;
        dialogListener   = null;
        
        preference       = null;
    }

    /**
     * �@���C�ɓ�����o�b�N�A�b�v����
     *
     */
    public void backupFavorite()
    {
         favoriteManager.backup();
    }

    /**
     *   �f�[�^�x�[�X����������
     * 
     */
    private void prepareDatabase()
    {
        // �ݒ���ێ��N���X�𐶐�����
        //preference = new a2BPreferenceDatabase(parent);

        // ������R���o�[�^(to UTF8)�𐶐�����
        utf8Converter = new a2Butf8Conveter();

        // ������R���o�[�^(to Shift JIS)�𐶐�����
        sjConverter   = new a2BsjConverter();

        // NG���[�h�Ǘ��N���X�𐶐�����
        ngWordManager = new a2BNgWordManager(parent, utf8Converter, sjConverter);

        // HTTP GET�f�[�^�ʐM�N���X�𐶐�����
        httpCommunicator = new a2BHttpCommunicator(parent);

        // ���O�C�����[�e�B���e�B�N���X�𐶐�����
        loginUtility     = new a2BloginHolder(parent, utf8Converter);
        
        // �ꗗ��́E�f�[�^�R�~���j�P�[�^�𐶐�����
        bbsCommunicator = new a2BbbsCommunicator(parent, utf8Converter);

        // ���C�ɓ���X���Ǘ��N���X�𐶐�����
        favoriteManager = new a2BFavoriteManager(parent, utf8Converter);        

        // �X���ꗗ��̓N���X�𐶐�����
        subjectParser = new a2BsubjectDataParser(parent, bbsCommunicator, utf8Converter, favoriteManager);
    }

    /*
     *   �g�p�����ʂ���������
     * 
     */
    private void prepareScreen()
    {
        // �Q�Ɖ�ʂ̏���
        viewForm = new a2BMessageViewForm("a2B", parent, subjectParser, utf8Converter, ngWordManager);
        viewForm.setCommandListener(viewForm);

        // �ꗗ��ʂ̏���
        listForm = new a2BMessageListForm("a2B", parent, bbsCommunicator, subjectParser, favoriteManager);
        listForm.setCommandListener(listForm);

        // �������݉�ʂ̏���
        writeForm = new a2BMessageWriteForm("Write", parent, sjConverter, utf8Converter);
        writeForm.setCommandListener(writeForm);

        // �t�@�C���Z���N�^�̏���
        fileSelector = new a2BfileSelector(parent);

        // �A�C�e���I���t�H�[���̏���
        selectForm = new a2BselectionForm(parent);
        
        return;
    }
    
    /*
     *   ��ʃT�C�Y�̐ݒ�
     * 
     * 
     */
    private void prepareScreenSize()
    {
        // �ǎ��̓ǂݏo��
        viewForm.loadWallPaperImage();
        
        // �w���v�C���[�W�̓ǂݏo��
        listForm.loadHelpImage();

        listForm.prepareScreenSize();

        viewForm.prepareScreenSize();
        
        selectForm.prepareScreenSize();
        
        return;
    }
    
    /**
     * a2B�ݒ�������Z�b�g���邩�ǂ���
     * 
     * @param isReset
     * @return  true : ���Z�b�g����Afalse : ���Z�b�g���Ȃ�
     * 
     */
    private boolean checkResetPreference(boolean isReset)
    {
        if (isReset == true)
        {
            // �{���Aa2B�ݒ�������Z�b�g����ꍇ...
            String baseDir = null;
            try
            {
                // jad�t�@�C���Ńf�B���N�g���̏ꏊ���w�肳��Ă��邩�H
                baseDir = parent.getAppProperty("a2B-Directory");

                //  �� WX310�ł̎w��� :  a2B-Directory: file:///SD:/$USER_DATA/a2B/
                //  �� 702NK�ł̎w��� :  a2B-Directory: file:///E:/WORK/a2B/
                //  �� W-ZERO3�̐ݒ�� :  a2B-Directory: file:///miniSD/WORK/a2B/
                //  �� Android�̐ݒ�� :  a2B-Directory: file:///sdcard/a2B/
            }
            catch (Exception e)
            {
                baseDir = null;
            }
            if (baseDir != null)
            {
                // jad�t�@�C���ɏ�����Ă���x�[�X�f�B���N�g����ݒ���Ƃ��ď����o��
                preference.setBaseDirectory(baseDir);
                
                // �x�[�X�f�B���N�g�����t�@�C���Z���N�^�̃f�t�H���g�l�Ƃ���
                fileSelector.setnextPath(preference.getBaseDirectory());                    

                // jad�t�@�C���Ƀx�[�X�f�B���N�g����񂪐ݒ肳��Ă����ꍇ�A���Z�b�g���Ȃ�
                return (false);
            }
        }
        return (isReset);
    }
    
    /*
     *   �p�����[�^����f�[�^��ǂݍ��ޏ���
     *   
     */
    private boolean restoreParameters()
    {
        if (preference == null)
        {
            preference = new a2BPreferenceDatabase(parent);
        }

        boolean     isReset            = false;
        byte[]       versionInfo       = null;
        byte[]       baseDirectory     = null;
        byte[]       boardInfo         = null;
        byte[]       fontInfo          = null;
        RecordStore  prefDB             = null;
        try
        {
            // ���R�[�h�X�g�A���I�[�v������
            prefDB = RecordStore.openRecordStore("a2B-Settings", true);
            if (prefDB.getNumRecords() == 0)
            {
                // ���R�[�h���o�^����Ă��Ȃ������ꍇ�́A�����f�[�^��o�^����
                isReset = true;
            }
            else
            {
                // �f�[�^�x�[�X�̃o�[�W�����m�F...
                versionInfo = prefDB.getRecord(a2B_VERSION_RECORDID);
                if (versionInfo.length == a2B_PREF_VERSION.length)
                {
                    for (int loop = 0; ((loop < versionInfo.length)&&(isReset == false)); loop++)
                    {
                        if (versionInfo[loop] != a2B_PREF_VERSION[loop])
                        {
                            // �o�[�W�������Ⴂ�A�A�A���Z�b�g����
                            isReset = true;
                            loop = versionInfo.length;
                        }
                    }
                }
                else
                {
                    // �o�[�W�������Ⴂ�ADB�����Z�b�g����
                    isReset = true;
                }

                // �x�[�X�f�B���N�g����ǂݏo��
                baseDirectory = prefDB.getRecord(a2B_BASEDIR_RECORDID);
                if (baseDirectory.length == 0)
                {
                    // �x�[�X�f�B���N�g����񂪌�����Ȃ�����...
                    parent.showDialog(parent.SHOW_INFO, 600000, "���", "�f�[�^(bbstable.html)�̊i�[�ꏊ���L�^����Ă��܂���B�w�肵�Ă��������B");    
                    baseDirSettingMode  = true;
                }
                else
                {
                    // �x�[�X�f�B���N�g����ݒ���ɓǂݏo��
                    preference.setBaseDirectory(new String(baseDirectory));

                    // �x�[�X�f�B���N�g�����t�@�C���Z���N�^�̃f�t�H���g�l�Ƃ���
                    fileSelector.setnextPath(preference.getBaseDirectory());                    
                }

                // �I�𒆔��A���X�ԍ���ǂݏo��
                boardInfo = prefDB.getRecord(a2B_CURRENTBBS_RECORDID);
                if (boardInfo.length == 72)
                {
                    
                    preference.currentBbsCategoryIndex = deSerializeData(boardInfo,  0);
                    preference.currentBbsIndex         = deSerializeData(boardInfo,  4);
                    preference.selectedSubjectIndex    = deSerializeData(boardInfo,  8);
                    preference.selectedMessageNumber   = deSerializeData(boardInfo, 12);
                    preference.ngWordDetectMode        = deSerializeData(boardInfo, 16);
                    preference.favoriteShowMode        = deSerializeData(boardInfo, 20);
                    preference.usedProxyScene          = deSerializeData(boardInfo, 24);
                    preference.openCurrentScreen       = deSerializeData(boardInfo, 28);
                    preference.currentMessageIndex     = deSerializeData(boardInfo, 32);
                    preference.currentScreenNumber     = deSerializeData(boardInfo, 36);
                    preference.vibrateDuration         = deSerializeData(boardInfo, 40);
                    int optionParameters              = deSerializeData(boardInfo, 44);
                    preference.favoriteShowLevel       = deSerializeData(boardInfo, 48);
                    preference.viewKanjiMode           = deSerializeData(boardInfo, 52);
                    preference.resBatchMode            = deSerializeData(boardInfo, 56);
                    preference.keyRepeatDelayTime      = deSerializeData(boardInfo, 60);
                    int optionDatas                   =  deSerializeData(boardInfo, 64);
                    preference.divideCommunicationSize = deSerializeData(boardInfo, 68);

                    if ((optionParameters & 1) != 0)
                    {
                        preference.viewScreenMode = true;
                    }
                    else
                    {
                        preference.viewScreenMode = false;                        
                    }

                    if ((optionParameters & 2) != 0)
                    {
                        preference.afterLaunchWebAction = true;
                    }
                    else
                    {
                        preference.afterLaunchWebAction = false;                        
                    }
                    if ((optionParameters & 4) != 0)
                    {
                        preference.autoEntryFavorite = true;
                    }
                    else
                    {
                        preference.autoEntryFavorite = false;                        
                    }
                    if ((optionParameters & 8) != 0)
                    {
                        preference.saveAtRootDirectory = true;
                    }
                    else
                    {
                        preference.saveAtRootDirectory = false;
                    }                
                    if ((optionParameters & 16) != 0)
                    {
                        preference.divideToGetHttp = true;
                    }
                    else
                    {
                        preference.divideToGetHttp = false;
                    }                
                    if ((optionParameters & 32) != 0)
                    {
                        preference.onlyDatabaseAccess = true;
                    }
                    else
                    {
                        preference.onlyDatabaseAccess = false;
                    }                
                    if ((optionParameters & 64) != 0)
                    {
                        preference.isShowTitleBusyArea = true;
                    }
                    else
                    {
                        preference.isShowTitleBusyArea = false;
                    }
                    if ((optionParameters & 128) != 0)
                    {
                        preference.backupSubjectTxt = true;
                    }
                    else
                    {
                        preference.backupSubjectTxt = false;
                    }
                    if ((optionParameters & 256) != 0)
                    {
                        preference.useGoogleProxy = true;
                    }
                    else
                    {
                        preference.useGoogleProxy = false;
                    }
                    if ((optionParameters & 512) != 0)
                    {
                        preference.useCachePicFile = true;
                    }
                    else
                    {
                        preference.useCachePicFile = false;
                    }
                    if ((optionParameters & 1024) != 0)
                    {
                        preference.ignoreFavoriteBackup = true;
                    }
                    else
                    {
                        preference.ignoreFavoriteBackup = false;
                    }
                    if ((optionParameters & 2048) != 0)
                    {
                        preference.cursorSingleMovingMode = true;
                    }
                    else
                    {
                        preference.cursorSingleMovingMode = false;
                    }
                    if ((optionParameters & 4096) != 0)
                    {
                        preference.useWX220JUserAgent = true;
                    }
                    else
                    {
                        preference.useWX220JUserAgent = false;
                    }
                    if ((optionParameters & 8192) != 0)
                    {
                        preference.reserve1 = true;
                    }
                    else
                    {
                        preference.reserve1 = false;
                    }
                    if ((optionParameters & 16384) != 0)
                    {
                        preference.forceSelfKanjiConvert = true;
                    }
                    else
                    {
                        preference.forceSelfKanjiConvert = false;
                    }
                    if ((optionParameters & 32768) != 0)
                    {
                        preference.usePicToConvert = true;
                    }
                    else
                    {
                        preference.usePicToConvert = false;
                    }
                    preference.aaWidth = optionDatas % 512;
                }

                byte[] byteData = null;
                byteData = prefDB.getRecord(a2B_OYSTERNAME_RECORDID);
                if (byteData != null)
                {
                    preference.oysterLoginName = new String(byteData);
                    if ((preference.oysterLoginName).compareTo(" ") == 0)
                    {
                        preference.oysterLoginName = "";
                    }
                }
                else
                {
                    preference.oysterLoginName = new String("name");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_OYSTERPASS_RECORDID);
                if (byteData != null)
                {
                    preference.oysterLoginPassword = new String(byteData);
                    if ((preference.oysterLoginPassword).compareTo(" ") == 0)
                    {
                        preference.oysterLoginPassword = "";
                    }
                }
                else
                {
                    preference.oysterLoginPassword = new String("pass");
                }
                byteData = null;
                
                byteData = prefDB.getRecord(a2B_WRITE_NAME_RECORDID);
                if (byteData != null)
                {
                    preference.writeName = new String(byteData);
                    if ((preference.writeName).compareTo(" ") == 0)
                    {
                        preference.writeName = "";
                    }
                }
                else
                {
                    preference.writeName = new String("7743@a2B");
                }
                byteData = null;

                // �ݒ肵�Ă���t�H���g�T�C�Y��ǂݏo��
                fontInfo = prefDB.getRecord(a2B_FONT_SIZE_RECORDID);
                if (fontInfo.length == 32)
                {
                    preference.listFormFontSize    = deSerializeData(fontInfo,  0);
                    preference.viewFormFontSize    = deSerializeData(fontInfo,  4);
                    preference.viewFormBackColor   = deSerializeData(fontInfo,  8);
                    preference.viewFormForeColor   = deSerializeData(fontInfo,  12);
                    preference.watchDogMinutes     = deSerializeData(fontInfo,  16);
                    preference.listFormFunctionKey = deSerializeData(fontInfo,  20);
                    preference.viewFormFunctionKey = deSerializeData(fontInfo,  24);
                    preference.resetSettings       = deSerializeData(fontInfo,  28);
                }

                // a2B�ݒ���N���A����...
                if (preference.resetSettings < 0)
                {
                    preference.resetSettings = 0;
                    isReset = true;
                }
                
                // proxy URL���̓ǂݏo��
                byteData = null;
                byteData = prefDB.getRecord(a2B_PROXYURL_RECORDID);
                if (byteData != null)
                {
                    preference.proxyUrl = new String(byteData);
                    if ((preference.proxyUrl).compareTo(" ") == 0)
                    {
                        preference.proxyUrl = null;
                    }
                }
                else
                {
                    preference.proxyUrl = null;
                }
                byteData = null;
                
                // �X���ǂݏo�����t�@�C�����̓ǂݏo��
                byteData = null;
                byteData = prefDB.getRecord(a2B_CURRENTLOAD_RECORDID);
                if (byteData != null)
                {
                    preference.openedFileName = new String(byteData);
                    if ((preference.openedFileName).compareTo(" ") == 0)
                    {
                        preference.openedFileName = null;
                    }
                }
                else
                {
                    preference.openedFileName = null;
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_BENAME_RECORDID);
                if (byteData != null)
                {
                    preference.beLoginName = new String(byteData);
                    if ((preference.beLoginName).compareTo(" ") == 0)
                    {
                        preference.beLoginName = "";
                    }
                }
                else
                {
                    preference.beLoginName = new String("name");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_BEPASS_RECORDID);
                if (byteData != null)
                {
                    preference.beLoginPassword = new String(byteData);
                    if ((preference.beLoginPassword).compareTo(" ") == 0)
                    {
                        preference.beLoginPassword = "";
                    }
                }
                else
                {
                    preference.beLoginPassword = new String("pass");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_USERAGENT_RECORDID);
                if (byteData != null)
                {
                    preference.changedUserAgent = new String(byteData);
                    if ((preference.changedUserAgent).compareTo(" ") == 0)
                    {
                        preference.changedUserAgent = "Monazilla/1.00  NNsi/1.10(a2B)";
                    }
                }
                else
                {
                    preference.changedUserAgent = new String("Monazilla/1.00  NNsi/1.10(a2B)");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_FILESAVEDIR_RECORDID);
                if (byteData != null)
                {
                    preference.fileSaveDirectory = new String(byteData);
                    if ((preference.fileSaveDirectory).compareTo(" ") == 0)
                    {
                        preference.fileSaveDirectory = "a2B_fetch";
                    }
                }
                else
                {
                    preference.fileSaveDirectory = new String("a2B_fetch");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_MEMOFILE_RECORDID);
                if (byteData != null)
                {
                    preference.memoFileName = new String(byteData);
                    if ((preference.memoFileName).compareTo(" ") == 0)
                    {
                        preference.memoFileName = "memo.txt";
                    }
                }
                else
                {
                    preference.memoFileName = new String("memo.txt");
                }

                // �������ݗp�L�[���[�h�̓ǂݍ���
                byteData = null;
                byteData = prefDB.getRecord(a2B_WRITEKEY_RECORDID);
                if (byteData != null)
                {
                    preference.writeKeyString = new String(byteData);
                    if ((preference.writeKeyString).compareTo(" ") == 0)
                    {
                        preference.writeKeyString = null;
                    }
                }
                else
                {
                    preference.writeKeyString = null;
                }

                // �������ݗpCookie�̓ǂݍ���
                byteData = null;
                byteData = prefDB.getRecord(a2B_WRITECOOKIE_RECORDID);
                if (byteData != null)
                {
                    preference.writeCookie = new String(byteData);
                    if ((preference.writeCookie).compareTo(" ") == 0)
                    {
                        preference.writeCookie = null;
                    }
                }
                else
                {
                    preference.writeCookie = null;
                }
                byteData = null;
            }

            // ���R�[�h�����
            prefDB.closeRecordStore();
            
        }
        catch (Exception e)
        {
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXCEPTION(restoreRecord)  e:" + e.toString());

            // ���R�[�h�X�g�A�����ĂȂ���Ε���
            try
            {
                if (prefDB != null)
                {
                    prefDB.closeRecordStore();
                }
            }
            catch (Exception e2)
            {
                // �������Ȃ�...
            }
            isReset = true;
        }

        // a2B�ݒ�������Z�b�g���邩�H
        if (checkResetPreference(isReset) == true)
        {
            // a2B�ݒ�������Z�b�g����
            preference.reset();
            
            // �\�����[�`�����Ăяo��...
            Thread thread = new Thread()
            {
                public void run()
                {    
                    // a2B�ݒ�̊i�[�ꏊ���w�肷��悤�ɕύX����
                    parent.showDialog(parent.SHOW_INFO, 3000, "���", " a2B�ݒ���N���A���܂����B\n\n �f�[�^(bbstable.html)�̊i�[�ꏊ���w�肵�Ă��������B");    
//                    parent.showDialog(parent.SHOW_INFO, 600000, "���", " a2B�ݒ���N���A���܂����B\n\n �f�[�^(bbstable.html)�̊i�[�ꏊ���w�肵�Ă��������B");    
//                    parent.showDialog(parent.SHOW_INFO, 0, "���", " a2B�ݒ���N���A���܂����B\n\n �f�[�^(bbstable.html)�̊i�[�ꏊ���w�肵�Ă��������B");    
                }
            };
            thread.start();
            baseDirSettingMode  = true;
        }
        else
        {
            // ���[�J�����ځ[��L�[���[�h��ǂݍ���...
            ngWordManager.restore();
            
            // ���C�ɓ���X������ǂݍ���...
            favoriteManager.restore();

            System.gc();

            // ���[�J�����ځ[��L�[���[�h���󏑂�����...
            ngWordManager.backup();    
        }
        return (baseDirSettingMode);
    }

    /*
     *   �p�����[�^���o�b�N�A�b�v���鏈��
     *   
     */
    private void backupParameters()
    {
        // �I�����ɂ̓C���f�b�N�X��ۑ�����
        //if (subjectParser.currentSubjectIndexSize() != 0)
        if (baseDirSettingMode != true)
        {            
            subjectParser.leaveSubjectList(preference.currentBbsIndex);
        }

        // ���[�J�����ځ[��L�[���[�h��ۑ�����... (WX310SA�ł̓t�@�C���������邽�߁A�����ŕۑ��ł��Ȃ�)
        //ngWordManager.backup();
        
        // ���C�ɓ���X������ۑ�����... (WX310SA�ł̓t�@�C���������邽�߁A�����ŕۑ��ł��Ȃ�)
        //favoriteManager.backup();
        
        // �_�~�[�t�@�C���o�͏���
        //parent.dummyBackup();

        RecordStore  prefDB = null;
        try
        {
            // ���R�[�h�X�g�A���I�[�v������ (���A�v������Q�ƁE�������݋���)
            prefDB = RecordStore.openRecordStore("a2B-Settings", true, RecordStore.AUTHMODE_ANY, true);

            // 1���R�[�h��...�o�[�W�����ԍ�
            int nofRecord = prefDB.getNumRecords();
            if (nofRecord >= a2B_VERSION_RECORDID)
            {
                // �o�[�W�������̏�������(�X�V)...
                prefDB.setRecord(a2B_VERSION_RECORDID, a2B_PREF_VERSION, 0, a2B_PREF_VERSION.length);
            }
            else
            {
                // �o�[�W�������̏�������(�ǉ�)...
                prefDB.addRecord(a2B_PREF_VERSION, 0, a2B_PREF_VERSION.length);                    
            }

            // 2���R�[�h��...�x�[�X�f�B���N�g��
            int size = ((preference.getBaseDirectory()).getBytes()).length;
            if (nofRecord >= a2B_BASEDIR_RECORDID)
            {
                // �x�[�X�f�B���N�g�����̏�������(�X�V)...
                prefDB.setRecord(a2B_BASEDIR_RECORDID, (preference.getBaseDirectory()).getBytes(), 0, size);
            }
            else
            {
                // �x�[�X�f�B���N�g�����̏�������(�ǉ�)...
                prefDB.addRecord((preference.getBaseDirectory()).getBytes(), 0, size);                    
            }

            // �I�v�V�����̃p�����[�^
            int optionParameters = 0;
            if (preference.usePicToConvert == true)
            {
                optionParameters = optionParameters + 32768;                
            }
            if (preference.forceSelfKanjiConvert == true)
            {
                optionParameters = optionParameters + 16384;
            }
            if (preference.reserve1 == true)
            {
                optionParameters = optionParameters + 8192;
            }
            if (preference.useWX220JUserAgent == true)
            {
                optionParameters = optionParameters + 4096;
            }
            if (preference.cursorSingleMovingMode == true)
            {
                optionParameters = optionParameters + 2048;
            }
            if (preference.ignoreFavoriteBackup == true)
            {
                optionParameters = optionParameters + 1024;
            }
            if (preference.useCachePicFile == true)
            {
                optionParameters = optionParameters + 512;
            }
            if (preference.useGoogleProxy == true)
            {
                optionParameters = optionParameters + 256;
            }
            if (preference.backupSubjectTxt == true)
            {
                optionParameters = optionParameters + 128;
            }
            if (preference.isShowTitleBusyArea == true)
            {
                optionParameters = optionParameters + 64;
            }
            if (preference.onlyDatabaseAccess == true)
            {
                optionParameters = optionParameters + 32;
            }
            if (preference.divideToGetHttp == true)
            {
                // 
                optionParameters = optionParameters + 16;
            }
            if (preference.saveAtRootDirectory == true)
            {
                // 
                optionParameters = optionParameters + 8;
            }
            if (preference.autoEntryFavorite == true)
            {
                // ���X���擾���Ɏ����ł��C�ɓ���ɓo�^����
                optionParameters = optionParameters + 4;
            }
            if (preference.afterLaunchWebAction == true)
            {
                // WEB�u���E�U�N����A�I�����Ȃ�
                optionParameters = optionParameters + 2;
            }

            if (preference.viewScreenMode == true)
            {
                // �t���X�N���[�����[�h
                optionParameters = optionParameters + 1;
            }

            int optionDatas = preference.aaWidth;
             
            // 3���R�[�h��...���A�I�𒆃��X�ԍ�...
            size = 72;
            byte[] bbsInfo = new byte[size];
            serializeData(bbsInfo,  0, preference.currentBbsCategoryIndex);
            serializeData(bbsInfo,  4, preference.currentBbsIndex);
            serializeData(bbsInfo,  8, preference.selectedSubjectIndex);
            serializeData(bbsInfo, 12, preference.selectedMessageNumber);
            serializeData(bbsInfo, 16, preference.ngWordDetectMode);
            serializeData(bbsInfo, 20, preference.favoriteShowMode);
            serializeData(bbsInfo, 24, preference.usedProxyScene);
            serializeData(bbsInfo, 28, preference.openCurrentScreen);
            serializeData(bbsInfo, 32, preference.currentMessageIndex);
            serializeData(bbsInfo, 36, preference.currentScreenNumber);
            serializeData(bbsInfo, 40, preference.vibrateDuration);
            serializeData(bbsInfo, 44, optionParameters);
            serializeData(bbsInfo, 48, preference.favoriteShowLevel);
            serializeData(bbsInfo, 52, preference.viewKanjiMode);
            serializeData(bbsInfo, 56, preference.resBatchMode);
            serializeData(bbsInfo, 60, preference.keyRepeatDelayTime);
            serializeData(bbsInfo, 64, optionDatas);
            serializeData(bbsInfo, 68, preference.divideCommunicationSize);

            if (nofRecord >= a2B_CURRENTBBS_RECORDID)
            {
                // �I�𒆔��A���X�ԍ��̏�������(�X�V)...
                prefDB.setRecord(a2B_CURRENTBBS_RECORDID, bbsInfo, 0, size);
            }
            else
            {
                // �I�𒆔��A���X�ԍ��̏�������(�ǉ�)...
                prefDB.addRecord(bbsInfo, 0, size);                    
            }
            bbsInfo = null;

            // 4���R�[�h��...���[�U��
            if (preference.oysterLoginName == null)
            {
                preference.oysterLoginName = " ";
            }
            size = (preference.oysterLoginName).getBytes().length;
            if (size == 0)
            {
                preference.oysterLoginName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_OYSTERNAME_RECORDID)
            {
                // �x�[�X�f�B���N�g�����̏�������(�X�V)...
                prefDB.setRecord(a2B_OYSTERNAME_RECORDID, (preference.oysterLoginName).getBytes(), 0, size);
            }
            else
            {
                // �x�[�X�f�B���N�g�����̏�������(�ǉ�)...
                prefDB.addRecord((preference.oysterLoginName).getBytes(), 0, size);
            }

            // 5���R�[�h��...�p�X���[�h
            if (preference.oysterLoginPassword == null)
            {
                preference.oysterLoginPassword = " ";
            }
            size = (preference.oysterLoginPassword).getBytes().length;
            if (size == 0)
            {
                preference.oysterLoginPassword = " ";
                size = 1;
            }
            if (nofRecord >= a2B_OYSTERPASS_RECORDID)
            {
                // �x�[�X�f�B���N�g�����̏�������(�X�V)...
                prefDB.setRecord(a2B_OYSTERPASS_RECORDID, (preference.oysterLoginPassword).getBytes(), 0, size);
            }
            else
            {
                // �x�[�X�f�B���N�g�����̏�������(�ǉ�)...
                prefDB.addRecord((preference.oysterLoginPassword).getBytes(), 0, size);                    
            }

            // 6���R�[�h��...�������ݎ��̖��O
            if (preference.writeName == null)
            {
                preference.writeName = " ";
            }
            size = (preference.writeName).getBytes().length;
            if (size == 0)
            {
                preference.writeName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_WRITE_NAME_RECORDID)
            {
                // �x�[�X�f�B���N�g�����̏�������(�X�V)...
                prefDB.setRecord(a2B_WRITE_NAME_RECORDID, (preference.writeName).getBytes(), 0, size);
            }
            else
            {
                // �x�[�X�f�B���N�g�����̏�������(�ǉ�)...
                prefDB.addRecord((preference.writeName).getBytes(), 0, size);                    
            }

            // 7���R�[�h��...�t�H���g���
            size = 32;
            byte[] fontInfo = new byte[size];
            serializeData(fontInfo,  0, preference.listFormFontSize);
            serializeData(fontInfo,  4, preference.viewFormFontSize);
            serializeData(fontInfo,  8, preference.viewFormBackColor);
            serializeData(fontInfo, 12, preference.viewFormForeColor);
            serializeData(fontInfo, 16, preference.watchDogMinutes);
            serializeData(fontInfo, 20, preference.listFormFunctionKey);
            serializeData(fontInfo, 24, preference.viewFormFunctionKey);
            serializeData(fontInfo, 28, preference.resetSettings);
            if (nofRecord >= a2B_FONT_SIZE_RECORDID)
            {
                // �I�𒆔��A�t�H���g���̏�������(�X�V)...
                prefDB.setRecord(a2B_FONT_SIZE_RECORDID, fontInfo, 0, size);
            }
            else
            {
                // �I�𒆔��A�t�H���g���̏�������(�ǉ�)...
                prefDB.addRecord(fontInfo, 0, size);                    
            }
            fontInfo = null;
            
            // 8���R�[�h��...proxy URL
            if (preference.proxyUrl == null)
            {
                preference.proxyUrl = " ";
            }
            size = (preference.proxyUrl).getBytes().length;
            if (size == 0)
            {
                preference.proxyUrl = " ";
                size = 1;
            }
            if (nofRecord >= a2B_PROXYURL_RECORDID)
            {
                // proxyUrl���̏�������(�X�V)...
                prefDB.setRecord(a2B_PROXYURL_RECORDID, (preference.proxyUrl).getBytes(), 0, size);
            }
            else
            {
                // proxyUrl���̏�������(�ǉ�)...
                prefDB.addRecord((preference.proxyUrl).getBytes(), 0, size);
            }

            // 9���R�[�h��...�Q�ƒ��X���̃t�@�C����
            if (preference.openedFileName == null)
            {
                preference.openedFileName = " ";
            }
            size = (preference.openedFileName).getBytes().length;
            if (size == 0)
            {
                preference.openedFileName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_CURRENTLOAD_RECORDID)
            {
                // �Q�ƒ��X���̃t�@�C�����̏�������(�X�V)...
                prefDB.setRecord(a2B_CURRENTLOAD_RECORDID, (preference.openedFileName).getBytes(), 0, size);
            }
            else
            {
                // �Q�ƒ��X���̃t�@�C�����̏�������(�ǉ�)...
                prefDB.addRecord((preference.openedFileName).getBytes(), 0, size);
            }

            // 10���R�[�h��...���[�U��(Be@2ch)
            if (preference.beLoginName == null)
            {
                preference.beLoginName = " ";
            }
            size = (preference.beLoginName).getBytes().length;
            if (size == 0)
            {
                preference.beLoginName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_BENAME_RECORDID)
            {
                // �a�d���[�U���̏�������(�X�V)...
                prefDB.setRecord(a2B_BENAME_RECORDID, (preference.beLoginName).getBytes(), 0, size);
            }
            else
            {
                // �a�d���[�U���̏�������(�ǉ�)...
                prefDB.addRecord((preference.beLoginName).getBytes(), 0, size);
            }

            // 11���R�[�h��...�p�X���[�h
            if (preference.beLoginPassword == null)
            {
                preference.beLoginPassword = " ";
            }
            size = (preference.beLoginPassword).getBytes().length;
            if (size == 0)
            {
                preference.beLoginPassword = " ";
                size = 1;
            }
            if (nofRecord >= a2B_BEPASS_RECORDID)
            {
                // �p�X���[�h�̂̏�������(�X�V)...
                prefDB.setRecord(a2B_BEPASS_RECORDID, (preference.beLoginPassword).getBytes(), 0, size);
            }
            else
            {
                // �p�X���[�h�̏�������(�ǉ�)...
                prefDB.addRecord((preference.beLoginPassword).getBytes(), 0, size);                    
            }

            // 12���R�[�h��...���[�U�G�[�W�F���g���            
            if (preference.changedUserAgent == null)
            {
                preference.changedUserAgent = " ";
            }
            size = (preference.changedUserAgent).getBytes().length;
            if (size == 0)
            {
                preference.changedUserAgent = " ";
                size = 1;
            }
            if (nofRecord >= a2B_USERAGENT_RECORDID)
            {
                // ���[�U�G�[�W�F���g���̏�������(�X�V)...
                prefDB.setRecord(a2B_USERAGENT_RECORDID, (preference.changedUserAgent).getBytes(), 0, size);
            }
            else
            {
                // ���[�U�G�[�W�F���g���̏�������(�ǉ�)...
                prefDB.addRecord((preference.changedUserAgent).getBytes(), 0, size);                    
            }

            // 13���R�[�h��...�擾�t�@�C���L�^�f�B���N�g��
            if (preference.fileSaveDirectory == null)
            {
                preference.fileSaveDirectory = " ";
            }
            size = (preference.fileSaveDirectory).getBytes().length;
            if (size == 0)
            {
                preference.fileSaveDirectory = " ";
                size = 1;
            }
            if (nofRecord >= a2B_FILESAVEDIR_RECORDID)
            {
                // �擾�t�@�C���L�^�f�B���N�g���̏�������(�X�V)...
                prefDB.setRecord(a2B_FILESAVEDIR_RECORDID, (preference.fileSaveDirectory).getBytes(), 0, size);
            }
            else
            {
                // �擾�t�@�C���L�^�f�B���N�g���̏�������(�ǉ�)...
                prefDB.addRecord((preference.fileSaveDirectory).getBytes(), 0, size);                    
            }

            // 14���R�[�h��...�����t�@�C����
            if (preference.memoFileName == null)
            {
                preference.memoFileName = " ";
            }
            size = (preference.memoFileName).getBytes().length;
            if (size == 0)
            {
                preference.memoFileName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_MEMOFILE_RECORDID)
            {
                // �����t�@�C�����̏�������(�X�V)...
                prefDB.setRecord(a2B_MEMOFILE_RECORDID, (preference.memoFileName).getBytes(), 0, size);
            }
            else
            {
                // �����t�@�C�����̏�������(�ǉ�)...
                prefDB.addRecord((preference.memoFileName).getBytes(), 0, size);                    
            }

            // 15���R�[�h��...�������ݗp�L�[���[�h
            if (preference.writeKeyString == null)
            {
                preference.writeKeyString = " ";
            }
            size = (preference.writeKeyString).getBytes().length;
            if (size == 0)
            {
                preference.writeKeyString = " ";
                size = 1;
            }
            if (nofRecord >= a2B_WRITEKEY_RECORDID)
            {
                // �������ݗp�L�[���[�h�̏�������(�X�V)...
                prefDB.setRecord(a2B_WRITEKEY_RECORDID, (preference.writeKeyString).getBytes(), 0, size);
            }
            else
            {
                // �������ݗp�L�[���[�h�̏�������(�ǉ�)...
                prefDB.addRecord((preference.writeKeyString).getBytes(), 0, size);                    
            }

            // 16���R�[�h��...�������ݗpCookie
            if (preference.writeCookie == null)
            {
                preference.writeCookie = " ";
            }
            size = (preference.writeCookie).getBytes().length;
            if (size == 0)
            {
                preference.writeCookie = " ";
                size = 1;
            }
            if (nofRecord >= a2B_WRITECOOKIE_RECORDID)
            {
                // �������ݗpCookie�̏�������(�X�V)...
                prefDB.setRecord(a2B_WRITECOOKIE_RECORDID, (preference.writeCookie).getBytes(), 0, size);
            }
            else
            {
                // �������ݗpCookie�̏�������(�ǉ�)...
                prefDB.addRecord((preference.writeCookie).getBytes(), 0, size);                    
            }

            // ���R�[�h�����
            prefDB.closeRecordStore();
        }
        catch (Exception e)
        {
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXCEPTION(backupRecord)  e:" + e.getMessage());

            // ���R�[�h�X�g�A�����ĂȂ���Ε���
            try
            {
                if (prefDB != null)
                {
                    prefDB.closeRecordStore();
                }
            }
            catch (Exception e2)
            {
                // �������Ȃ�...
            }
        }
        preference = null;
        return;
    }

    /*
     *  �f�[�^�� int�^����o�C�g��֕ϊ�����
     * 
     * 
     */
    private void serializeData(byte[] buffer, int offset, int convId)
    {
        buffer[offset + 0] = (byte)((convId >> 24)&(0xff));
        buffer[offset + 1] = (byte)((convId >> 16)&(0xff));
        buffer[offset + 2] = (byte)((convId >>  8)&(0xff));
        buffer[offset + 3] = (byte)((convId      )&(0xff));
        return;
    }
    
    /*
     *  �o�C�g��f�[�^��int�^�֕ϊ�����
     * 
     * 
     */
    private int deSerializeData(byte[] buffer, int offset)
    {
        return ((((buffer[offset + 0]&(0xff))<< 24) + ((buffer[offset + 1]&(0xff))<< 16) +
                 ((buffer[offset + 2]&(0xff))<<  8) + ((buffer[offset + 3]&(0xff)))));
    }    
    
    /*
     *   Dialog�����p�̃R�}���h���X�i�N���X
     *   
     */
    public class a2BdialogListener implements CommandListener
    {
        a2BMain parent;
        
        // �R���X�g���N�^
        public a2BdialogListener(a2BMain object)
        {
            parent = object;
        }

        // �R�}���h��M...
        public void commandAction(Command c, Displayable d)
        {
            // �_�C�A���O���B��...
            parent.hideDialog();
            return;
        }
    }
    
    /*
     *   a2B�ݒ�ێ��N���X...
     *   
     */
    public class a2BPreferenceDatabase
    {
//        ////// �萔�̒�` //////
        private final int SLEEP_WATCHDOG_TIME   = 5;  // 5���҂�... (�Œ�10�����x)

        private a2BMain parent = null;

        private String   baseDirectory          = null;
        private byte    confirmationMode       = 0;

        // �O��̃J�e�S���C���f�b�N�X�ƔC���f�b�N�X���L������
        public int    previousBbsCategoryIndex = -1;
//        public int    previousBbsIndex         = -1;

        // ���݂�BBS�J�e�S���ƃC���f�b�N�X
        public int     currentBbsCategoryIndex = 0;
        public int     currentBbsIndex         = 0;
        public int     currentMessageIndex     = 0;
        public int     currentScreenNumber     = 0;
        
        // ����̉�ʃ��[�h...
        public int   openCurrentScreen         = 0;
                
        // ���݂̑I�𒆃X���ƃ��X�ԍ�
        public int     selectedSubjectIndex    = 0;
        public int     selectedMessageNumber   = 0;

        public int     ngWordDetectMode        = 0;
        public int     favoriteShowMode        = 0;
        public int     favoriteShowLevel       = 0;
        
        public int     listFormFunctionKey     = 4;
        public int     viewFormFunctionKey     = 7;

        public int     keyRepeatDelayTime      = 400;        

        public int     viewKanjiMode              = 0;        

        public int     vibrateDuration         = 300;
        public int     divideCommunicationSize = 49100;
//        public boolean backgroundComm         = true;
        public boolean afterLaunchWebAction   = false;
        public boolean autoEntryFavorite      = false;
        public boolean saveAtRootDirectory    = false;
        public boolean divideToGetHttp        = false;
        public boolean onlyDatabaseAccess     = false;
        public boolean viewScreenMode         = false;
        public boolean isShowTitleBusyArea    = false;
        public boolean backupSubjectTxt       = false;
        public boolean ignoreFavoriteBackup   = false;
        public boolean useGoogleProxy         = false;
        public boolean useWX220JUserAgent     = false;
        public boolean useCachePicFile        = false;
        public boolean cursorSingleMovingMode = false;
        public boolean forceSelfKanjiConvert  = false;
        public boolean usePicToConvert        = false;
        public boolean reserve1               = false;

        public int     listFormFontSize        = Font.SIZE_SMALL;
        public int     viewFormFontSize        = Font.SIZE_SMALL;
        
        public int     viewFormBackColor       = 0x00ffffff;
        public int     viewFormForeColor       = 0x00000000;

        public int     resBatchMode            = 1 + (256 * 3);
        
        public int     watchDogMinutes         = SLEEP_WATCHDOG_TIME;
        public long    oysterLoginDateTime     = 0;
        public String   oysterLoginName         = null;
        public String   oysterLoginPassword     = null;
        public String   oysterLoginSessionId    = null;
        public String   oysterLoginUserAgent    = null;
        public String   beLoginName             = null;
        public String   beLoginPassword         = null;
        public String   beCookie                = null;
        public long    beLoginDateTime         = 0;
        public String   writeName               = "7743@a2B";
        public String   writeKeyString          = null;
        public String   writeCookie             = null;
        
        public String   proxyUrl                = null;
        public String   changedUserAgent        = "Monazilla/1.00  NNsi/1.10(a2B)";
        public String   fileSaveDirectory       = "a2B_fetch";
        public String   memoFileName            = "memo.txt";
        public int     usedProxyScene          = 0;
        public int     resetSettings           = 0;
        public int     aaWidth                 = 99;
        public String   openedFileName          = null;

        // �R���X�g���N�^
        public a2BPreferenceDatabase(a2BMain object)
        {
            // �v���b�g�t�H�[�����̂𒲂ׁAWX310�������ꍇ�ɂ́Agzip�]���΍��ݒ肷��
            String platForm = System.getProperty("microedition.platform");
            if (platForm.indexOf("WX310") >= 0)
            {
                usedProxyScene = 4;
            }

            parent = object;
            confirmationMode  = parent.SHOW_DEFAULT;
        }

        // �ݒ������������
        public void reset()
        {
            baseDirectory      = null;
            vibrateDuration    = 300;
            confirmationMode   = parent.SHOW_DEFAULT;

            // �K�x�R�����{
            System.gc();
            return;
        }

        /**
         *  �t�H���g�����X�V����
         *
         */
        public void updateFontData(boolean isReset, int fontSize)
        {
        	if (isReset == true)
        	{
                listFormFontSize   = Font.SIZE_SMALL;
                viewFormFontSize   = Font.SIZE_SMALL;
                viewFormBackColor  = 0x00ffffff;
                viewFormForeColor  = 0x00000000;
        	}
        	else
        	{
                listFormFontSize   = fontSize;
                viewFormFontSize   = fontSize;        		
        	}
            return;
        }        
        
        // �f�[�^�i�[�x�[�X�f�B���N�g����ݒ肷��
        public void setBaseDirectory(String directory)
        {
            baseDirectory = directory;

            // �L���f�B���N�g����\������B
            parent.showDialog(parent.SHOW_DEBUG, 0, "Debug", "�f�[�^�f�B���N�g���́A '" + baseDirectory + "' �ł��B");
        }

        // �f�[�^�i�[�x�[�X�f�B���N�g����Ԃ�
        public String getBaseDirectory()
        {
            return (baseDirectory);
        }

        // �������[�h�̐ݒ�
        public void setConfirmationMode(byte mode)
        {
            confirmationMode = mode;
        }

        // �������[�h�ݒ��Ԃ���������
        public byte getConfirmationMode()
        {
            return (confirmationMode);
        }    
    }
}
//--------------------------------------------------------------------------
//  a2BobjectFactory  --- �I�u�W�F�N�g�Ǘ��N���X
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
