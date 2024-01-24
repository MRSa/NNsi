import java.util.Date;
import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.forms.IDataInputStorage;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.IInformationScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.IFileSelectionStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpA2BhttpCommunication;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileHttpCommunication;

public class A2BUSceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{

    /***** �����f�[�^(��������) *****/
    private ChoiceGroup  preferences        = null;
    private TextField    gettingPauseTime   = null;
    private String       currentBbsUrl      = "";
    private String       currentBbsNick     = "";
    private String       informationMessage = null;
    private int          itemSelectionIndex = -1;
    private int          errorThreadCount = 0;
    private int          updateThreadCount = 0;
    private int          currentThreadCount = 0;
    private int          threadIndexNumber = 0;
    /***** �����f�[�^(�����܂�) *****/

    /***** ���̑��A�N�Z�X�N���X *****/
    private MidpCanvas                baseCanvas = null;
    private MidpResourceFileUtils resUtils    = null;
    private A2BUa2BbbsCommunicator bbsManager = null;
    private A2BUa2BFavoriteManager favoriteDb = null;
    private MidpFileDirList       fileDirList = null;
    private A2BUa2BsubjectDataParser subjectDb = null;
    
    private A2BUDataStorage storageDb = null;
    
    
    private MidpA2BUhttpCommunicator httpCommController = null;
    private MidpA2BhttpCommunication httpCommunicator = null;
    
    /**
     *  �R���X�g���N�^
     *
     *  @param dataStorage �f�[�^�L�^�N���X
     *  @param resourceUtils ���\�[�X�A�N�Z�X�N���X
     *  @param fileUtils �t�@�C���A�N�Z�X�N���X
     */
    public A2BUSceneStorage(A2BUDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpCanvas dataProcess, MidpFileDirList dirList)
    {
        storageDb        = dataStorage;
        resUtils         = resourceUtils;
        baseCanvas       = dataProcess;
        fileDirList      = dirList;
        
        subjectDb = new A2BUa2BsubjectDataParser(this);
        httpCommController = new MidpA2BUhttpCommunicator(true, this);
        httpCommunicator = new MidpA2BhttpCommunication(httpCommController, null);
    }

    /**------------------------------------------------------------**
     **
     **  ��������ۊǃf�[�^�̓ǂݏ���...
     **
     **
     **------------------------------------------------------------**/
    /**
     * �f�B���N�g���̉���
     * @return �f�B���N�g����
     */
    public String getDefaultDirectory()
    {
        return (storageDb.getBaseDirectory());
    }

    /**
     *  ��URL����͂���...
     * @param fileName ��͂���t�@�C����
     */
    public void prepareBoardUrl()
    {
        String dir = storageDb.getBaseDirectory();
        bbsManager = new A2BUa2BbbsCommunicator(dir);

        // �ꗗ����͂���...
        bbsManager.prepare();

        // ���O�f�B���N�g���ꗗ���擾����
        if (fileDirList.prepare(dir) == false)
        {
            return;
        }

        int limit = fileDirList.getCount();
        int nofBoard = 0;
        for (int index = 0; index < limit; index++)
        {
             MidpFileDirList.fileInformation fileInfo = fileDirList.getFileInfo(index);
             if (fileInfo.isDirectory() == true)
             {
                 String dirName = fileInfo.getFileName();
                 if (dirName.indexOf(".") != 0)
                 {
                     if (bbsManager.markUsingBbs(dirName) == true)
                     {
                         nofBoard++;
                 
                     }
                 }
             }
        }
        bbsManager.trimBbsList();
        
        System.gc();
        return;
    }

    /**
     *  �I������Ă���i�t�@�C���j������������
     *  
     */
    public  int getSelectedNameCount()
    {
        if (storageDb.getBaseDirectory() == null)
        {
            return (0);
        }
        return (1);
    }

    /**
     *  �f�B���N�g���̉���
     * @return �f�B���N�g����
     */
    public String getSelectedName(int index)
    {
        return (storageDb.getBaseDirectory());
    }
    
    /**
     *   �f�B���N�g���̉���
     * @param �f�B���N�g����
     */
    public void setSelectedName(String name)
    {
        storageDb.setBaseDirectory(name);
    }

    /**
     *  �I�𒆂̃f�B���N�g�������N���A����
     * 
     */
    public void clearSelectedName()
    {
        storageDb.setBaseDirectory(null);
        return;
    }
    
    /**
     *  �ݒ肵���f�B���N�g�������m�肷��
     *  (�������Ȃ�)
     * 
     */
    public void confirmSelectedName()
    {
        return;
    }
    
    
    /**
     *  ���C�ɓ������ǂݍ���
     * 
     * @param directory
     */
    public int prepareFavoriteDb(String directory)
    {
        favoriteDb = new A2BUa2BFavoriteManager(directory);
        favoriteDb.prepare();
        favoriteDb.restore();
        return (favoriteDb.getNumberOfFavorites());
    }

    /**
     *  ���C�ɓ����񐔂���������
     * @return
     */
    public int getNumberOfFavorites()
    {
        return (favoriteDb.getNumberOfFavorites());
    }
    
    /**
     *  ���C�ɓ������f���o��
     *
     */    
    public void writeFavoriteDb()
    {
        favoriteDb.backup();
    }
    
    /**
     *   ���C�ɓ���ɓo�^����Ă��邩�H
     * @param nick
     * @param threadNumber
     * @return
     */
    public int isExistFavorite(String nick, long threadNumber)
    {
        int retCode = -1;
        try
        {
            if (favoriteDb != null)
            {
                retCode = favoriteDb.exist(nick, threadNumber);
            }
        }
        catch (Exception e)
        {
            retCode = -1;
        }
        return (retCode);
    }

    /**
     *  ���C�ɓ�������擾����
     * @param index ���C�ɓ�����̃C���f�b�N�X�ԍ�
     * @return ���C�ɓ�����
     */
    public A2BUa2BFavoriteManager.a2BFavoriteThread getFavoriteThreadData(int index)
    {
        A2BUa2BFavoriteManager.a2BFavoriteThread threadData = null;
        try
        {
            if (favoriteDb != null)
            {
                threadData = favoriteDb.getThreadData(index);
            }
        }
        catch (Exception e)
        {
            threadData = null;
        }
        return (threadData);        
    }

    /**------------------------------------------------------------**
     **
     **  ������������f�[�^�̓ǂݏ���...
     **
     **
     **------------------------------------------------------------**/
    /**
     * �\�����郁�b�Z�[�W����������
     * 
     * @return
     */
    public String getInformationMessageToShow()
    {
        String message = "";
        try
        {
            message = resUtils.getResourceText("/res/welcome.txt");
        }
        catch (Exception ex)
        {
            //
        }
        return (message);
    }

    /**
     *   �{�^��������������
     *   
     */
    public int getNumberOfButtons(int screenId)
    {
        return (1);
    }

    /**
     * ���b�Z�[�W���󂯕t����ꂽ���ǂ����ݒ肷��
     * @param buttonId �����ꂽ�{�^����ID
     */
    public void setButtonId(int buttonId)
    {
        // �{�^���������ꂽ�I�I�i��ʑJ�ځI�j
        
    }

    /**
     *  ���\����ʂ̃{�^�����x��������
     * 
     */
    public String getInformationButtonLabel(int buttonId)
    {
        return("Dir�I��");
    }

    /**
     *  �f�[�^���͂��L�����Z�����ꂽ�Ƃ�
     *  
     */
    public void cancelEntry()
    {
        preferences      = null;
        gettingPauseTime = null;
        System.gc();
        return;
    }

    /**
     *  �f�[�^���͂��n�j���ꂽ�Ƃ�
     *  
     */
    public void dataEntry()
    {
        int preferenceData = 0;
        if (preferences.isSelected(1) == true)
        {
            preferenceData = preferenceData + 1;
        }
        if (preferences.isSelected(2) == true)
        {
            preferenceData = preferenceData + 2;
        }
        if (preferences.isSelected(0) == true)
        {
            preferenceData = preferenceData + 4;
        }
        if (preferences.isSelected(3) == true)
        {
            preferenceData = preferenceData + 8;
        }
        storageDb.setPreference(preferenceData);        
        storageDb.setWaitTime(Integer.valueOf(gettingPauseTime.getString()).intValue());
        
        preferences    = null;
        gettingPauseTime = null;
        System.gc();
        return;
    }

    /**
     *  �I�����A�o�C�u�𓮍삳����H
     * @return
     */
    public boolean isDoVibrate()
    {
        boolean ret = false;
        int setData = storageDb.getPreference();
        if ((setData & 1) != 0)
        {
            ret = true;
        }
        return (ret);
    }

    /**
     *  �I�����A�����I�ɃA�v�����I��������H
     * @return
     */
    public boolean isAutoDestroyApplication()
    {
        boolean ret = false;
        int setData = storageDb.getPreference();
        if ((setData & 2) != 0)
        {
            ret = true;
        }
        return (ret);
    }

    /**
     *  subject.txt���X�V����H
     * @return
     */
    public boolean isGetSubjectTxt()
    {
        boolean ret = false;
        int setData = storageDb.getPreference();
        if ((setData & 4) != 0)
        {
            ret = true;
        }
        return (ret);
    }

    /**
     *  WX310�n��Gzip�]���΍�����{����H
     * @return
     */
    public boolean isWorkaroundGzip()
    {
        boolean ret = false;
        int setData = storageDb.getPreference();
        if ((setData & 8) != 0)
        {
            ret = true;
        }
        return (ret);    	
    }
    
    /**
     *  �f�[�^���͂̂��߂̏���
     *  
     */
    public void prepare()
    {
        httpCommController.SetUserAgent("Monazilla/1.00  a2BU/1.00");
    }

    /**
     *  ����ʂ̐؂�ւ�
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == A2BUSceneDB.WELCOME_SCREEN)
        {
            return (A2BUSceneDB.DIR_SCREEN);
        }
        else if (screenId == A2BUSceneDB.DIR_SCREEN)
        {
            return (A2BUSceneDB.DEFAULT_SCREEN);
        }
        return (IDataInputStorage.SCENE_TO_PREVIOUS);
    }

    /**
     *  ����ʂ̐؂�ւ�
     * 
     */
    public String nextSceneTitleToChange(int screenId)
    {
        return ("");
    }

    /**
     *  �f�[�^���͏I��
     *  
     */
    public void finish()
    {
        // �������Ȃ�
    }

    /**
     *  �f�[�^���̓t�B�[���h�̌����擾����
     *  
     */
    public int getNumberOfDataInputField()
    {
        return (1);
    }

    /**
     *  �f�[�^���̓t�B�[���h���擾����
     *  
     */
    public TextField getDataInputField(int index)
    {
    	gettingPauseTime = null;
    	String waitTime = "" + storageDb.getWaitTime();
    	gettingPauseTime = new TextField("����҂�����(ms)", waitTime, 6, TextField.DECIMAL);
        return (gettingPauseTime);
    }

    /**
     *  �I�v�V�����̌����擾����
     *  
     */
    public int getNumberOfOptionGroup()
    {
        return (1);
    }

    /**
     *   �I�v�V�������擾����
     */
    public ChoiceGroup getOptionField(int index)
    {
        preferences    = null;
        preferences    = new ChoiceGroup("�I�v�V����", Choice.MULTIPLE);
        preferences.append("�X���ꗗ(subject.txt)���X�V����", null);
        preferences.append("�X�V�I�����A�o�C�u���s", null);
        preferences.append("�X�V�I�����A�A�v�����I��", null);
        preferences.append("WX310�n��gzip�]���΍�", null);

        // �f�t�H���g�l�𔽉f������
        int setData = storageDb.getPreference();
        if ((setData & 1) != 0)
        {
            preferences.setSelectedIndex(1, true);
        }
        if ((setData & 2) != 0)
        {
            preferences.setSelectedIndex(2, true);
        }
        if ((setData & 4) != 0)
        {
            preferences.setSelectedIndex(0, true);
        }
        if ((setData & 8) != 0)
        {
            preferences.setSelectedIndex(3, true);
        }
        return (preferences);
    }

    /**
     *  �I�����̐����擾����
     *  
     */
    public int numberOfSelections()
    {
        return (0);
    }

    /**
     *  �I�����̕�������擾����
     *  
     */
    public String getSelectionItem(int itemNumber)
    {
        return ("");
    }
    
    /**
     *  �I��p�̃K�C�h��������擾����
     *  
     */
    public String getSelectionGuidance()
    {
        return ("Menu��Dir�m��");
    }

    /**
     *  �^�C�g����\�����邩�ǂ����H
     *  
     */
    public boolean isShowTitle()
    {
        return (true);
    }

    /**
     *   start���A�A�C�e�������������邩�H
     * 
     */
    public boolean isItemInitialize()
    {
        return (true);
    }

    /**
     *  �{�^���̐����擾����
     *  
     */
    public int getNumberOfSelectionButtons()
    {
        return (1);
    }

    /**
     *  �{�^���̃��x�����擾����
     *  
     */
    public String getSelectionButtonLabel(int buttonId)
    {
        return ("DIR�I��");
    }

    /**
     *  �w�i���݂��Ⴂ�̐F�ŕ\�����邩�H
     *  
     */
    public boolean getSelectionBackColorMode()
    {
        return (false);
    }

    
    public void setSelectedItem(int itemId)
    {
        itemSelectionIndex = itemId;
    }

    public int getSelectedItem(int itemNumber)
    {
        if (itemNumber != 0)
        {
            return (-1);
        }
        return (itemSelectionIndex);
    }
    
    public boolean isSelectedItemMulti()
    {
        return (false);
    }

    public int getNumberOfSelectedItems()
    {
        if (itemSelectionIndex < 0)
        {
            return (0);
        }
        return (1);
    }

    // �I�����ꂽ�{�^��ID����������
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            resetSelectionItems();
        }
        return (false);
    }

    // �A�C�e���I�����N���A����
    public void resetSelectionItems()
    {
        itemSelectionIndex = -1;
        return;
    }

    // ���j���[�{�^���������ꂽ
    public boolean triggeredMenu()
    {    
        // ��ʂ�؂�ւ���...
        return (true);
    }
 
    /**
     *  �̃X���擾���(a2b.idx)���t�@�C���ɏ����o��
     *
     */
    public void backupA2BIndex()
    {
        subjectDb.outputA2BIndex();
    }

    /**
     *  �̃X���擾���(a2b.idx)���������ɓW�J����
     * @param bbsIndex
     */
    public void prepareA2BIndex(int bbsIndex)
    {
        currentBbsUrl  = bbsManager.getBoardUrl(bbsIndex);
        currentBbsNick = bbsManager.convertBoardNick(currentBbsUrl);
        subjectDb.prepareA2BIndex(currentBbsNick);
    }

    /**
     *  ���݂�BBS��URL���擾����
     * @return
     */
    public String getCurrentBBSUrl()
    {
        return (currentBbsUrl);
    }

    /**
     *  ���݂�BBS�̃j�b�N�l�[�����擾����
     * @return
     */
    public String getCurrentBBSNick()
    {
        return (currentBbsNick);
    }

    /**
     *  �Ɋ܂܂��X��������������
     * @return
     */
    public int getNumberOfThreads()
    {
        if (subjectDb == null)
        {
            return (-1);
        }
        return (subjectDb.getNumberOfThread());
    }

    /**
     *  �X���^�C�g�����擾����
     * @param threadIndex
     * @return
     */
    public A2BUa2BsubjectDataParser.subjectDataHolder getThreadData(int threadIndex)
    {
        A2BUa2BsubjectDataParser.subjectDataHolder data = subjectDb.getSubjectData(threadIndex);
        return (data);
    }

    /**
     *  �J�E���^�����Z�b�g
     *
     */
    public void resetUpdateThreadCount()
    {
        updateThreadCount = 0;
        currentThreadCount = 0;
        errorThreadCount = 0;
    }

    /**
     *  �J�E���^�̒l���擾
     * @return
     */
    public int getUpdateThreadCount()
    {
        return (updateThreadCount);
    }

    /**
     *  ���ݍX�V���̃X����������
     * @return
     */
    public int getCurrentThreadCount()
    {
        return (currentThreadCount);
    }

    /**
     *  �擾�G���[�����������X��������������
     * @return
     */
    public int getErrorThreadCount()
    {
    	return (errorThreadCount);
    }    
    
    /**
     *  subject.txt���擾����
     * @param url �擾��URL
     * @param baseDirectory �L�^����f�B���N�g��
     * @param fileName �L�^����t�@�C����
     */
    public void getSubjectTxt(String url, String directory, String fileName)
    {
    	try
    	{
            MidpFileHttpCommunication communicator = new MidpFileHttpCommunication();
    	    communicator.prepare(null);
    	    communicator.SetUserAgent("Monazilla/1.00  a2BU/1.00");
    	    communicator.DoHttpMain(url, directory, fileName);
    	    communicator = null;
    	}
    	catch (Exception ex)
    	{
    		//
    	}
        System.gc();    	
    	return;
    }
    
    /**
     *  ���X���ֈړ�...
     * @param threadIndex
     */
    public void finishTrigger(int threadIndex)
    {
        threadIndexNumber = threadIndex;
        Thread thread = new Thread()
        {
            public void run()
            {
            	int waitTime = storageDb.getWaitTime();
            	if (waitTime > 0)
            	{
            		// ��������{����܂ŁA�����~�܂�B
            		try
            		{
            	        sleep(waitTime);
            		}
            		catch (Exception ex)
            		{
            			// �~�܂�...
            		}
            	}
        
                baseCanvas.redraw(false);
                baseCanvas.updateData(threadIndexNumber);
                System.gc();
            }
        };
        try
        {
            thread.start();
        }
        catch (Exception ex)
        {
            //
        }
    }
    
    /**
     *  ��ʍĕ`��w��
     * @param force
     */
    public void redraw(boolean force)
    {
        // ��ʕ\�����X�V
        baseCanvas.redraw(force);
    }

    /**
     *  ���O�X�V�����{�I
     * @param url
     * @param fileName
     */
    public void updateLogFile(String url, String directory, String fileName, A2BUa2BsubjectDataParser.subjectDataHolder data)
    {
        boolean err = true;
        boolean ret = true;
        int responseCode = 200;
        String updateFileName = directory + fileName;
    	
    	// ��ʕ\�����X�V
        baseCanvas.redraw(false);

        long fileSize = MidpFileOperations.GetFileSize(updateFileName);
        if (fileSize <= 0)
        {
        	// �܂����O���擾���Ă��Ȃ�����...
        	// (subject.txt�̎擾���W�b�N�𗬗p����)
        	getSubjectTxt(url, directory, fileName);
        }
        else
        {
            fileSize = fileSize - 1;
            
            /// ���O�X�V���Ăяo���I
            err = httpCommunicator.GetHttpCommunication(updateFileName, url, null, null, (int) fileSize, -1, true);
            ret = httpCommunicator.isReceived();
            responseCode = httpCommunicator.getLastResponseCode();
        }

        // �X�V����Ă����H
        if ((ret == true)||((ret == false)&&(err == false))||(responseCode == 302))
        {
            long lastModified = data.lastModified;

            // �X����Ԃ��X�V...
            data.currentStatus = A2BUa2BsubjectDataParser.STATUS_UPDATE;
            if ((err == false)||(responseCode == 302))
            {
                // ��M�G���[����...
                data.currentStatus = A2BUa2BsubjectDataParser.STATUS_GETERROR;
            }
            
            try
            {
                // �X�V�������Ƃ��Ă���
                Date curTime = new Date();
                lastModified = curTime.getTime();
            }
            catch (Exception e)
            {
                // ��O�����A�������Ȃ�...
            }

            if (data.favoriteIndex >= 0)
            {
                // ���C�ɓ���ɓo�^����Ă�X���������I
                // �i�����ł��C�ɓ���̏����X�V����j
                A2BUa2BFavoriteManager.a2BFavoriteThread favorData = favoriteDb.getThreadData(data.favoriteIndex);
                favorData.currentStatus = data.currentStatus;
                favorData.lastModified  = lastModified;
                String bbsUrl = getCurrentBBSUrl() + "/";
                favorData.setThreadUrl(bbsUrl);
            }
            data.lastModified = lastModified;
            if (data.currentStatus == A2BUa2BsubjectDataParser.STATUS_GETERROR)
            {
            	errorThreadCount++;
            }
            else
            {
                updateThreadCount++;
            }
        }
        currentThreadCount++;
        System.gc();
        return;
    }
    
    /**
     *  ��񃁃b�Z�[�W��ݒ肷��
     * 
     * @param message
     */
    public void setInformationMessage(String message)
    {
        informationMessage = message;
    }

    /**
     *  ��񃁃b�Z�[�W����������
     * 
     * @return
     */
    public String getInformationMessage()
    {
        return (informationMessage);
    }

    /**
     *  �̐����������� 
     * @return
     */
    public int getNumberOfBBS()
    {
        if (bbsManager == null)
        {
            return (-1);
        }
        return (bbsManager.getNumberOfBoardName());
    }
}
