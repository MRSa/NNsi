import java.util.Vector;

import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.screens.informations.*;
import jp.sourceforge.nnsi.a2b.screens.selections.*;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.forms.*;
import jp.sourceforge.nnsi.a2b.utilities.*;

/**
 * �A�v���P�[�V�����̃f�[�^���i�[����N���X
 * 
 * @author MRSa
 *
 */
public class A2chSSceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{
    private final int  MaxDataInputLength   = 2048;

    /***** �����f�[�^(��������) *****/
    private TextField    inputDataField     = null;
    private String       find2chUrl         = null;
    private String       informationMessage = null;
    private int          itemSelectionIndex = -1;
    /***** �����f�[�^(�����܂�) *****/

    /***** ���̑��A�N�Z�X�N���X *****/
    private IDataProcessing    dataProcessing = null;
    private MidpFileHttpCommunication httpCommunicator = null;
    private MidpResourceFileUtils resUtils    = null;
    private MidpFileOperations fileOperation  = null;
    private MidpKanjiConverter kanjiConverter = null;
    private A2chSDataStorage storageDb = null;
    
    // 2ch�����̌��ʊi�[���X�g
    private Vector pickup2chList = null;
    
    /**
     *  �R���X�g���N�^
     *
     *  @param dataStorage �f�[�^�L�^�N���X
     *  @param resourceUtils ���\�[�X�A�N�Z�X�N���X
     *  @param fileUtils �t�@�C���A�N�Z�X�N���X
     */
    public A2chSSceneStorage(A2chSDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpFileOperations fileUtils, MidpKanjiConverter knjConv, MidpFileHttpCommunication httpComm, IDataProcessing dataProcess)
    {
        storageDb = dataStorage;
        fileOperation = fileUtils;
        kanjiConverter = knjConv;
        resUtils = resourceUtils;
		httpCommunicator = httpComm;
		dataProcessing = dataProcess;
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
    	find2chUrl = null;
    	return;
    }

    /**
     *  �f�[�^���͂��n�j���ꂽ�Ƃ�
     *  
     */
    public void dataEntry()
    {
        // �������EUC�ϊ�����URL�𐶐�����...
    	String ddd  = inputDataField.getString();
    	if (ddd.length() > 0)
    	{
            String data = kanjiConverter.UrlEncode(kanjiConverter.ParseToEuc(ddd));
            find2chUrl = "http://find.2ch.net/?BBS=ALL&TYPE=TITLE&STR=" + data + "&COUNT=25";
    	}
        inputDataField = null;
        pickup2chList = null;
        System.gc();
        return;
    }

    /**
     *  �f�[�^���͂��ꂽ����URL������ꍇ...
     * 
     * @return ��������URL
     */
    public String getUrlToSearch()
    {
        return (find2chUrl);
    }

    /**
     *  �����pURL���N���A����
     */
    public void resetUrlToSearch()
    {
    	find2chUrl = null;
    }

    /**
     *  �f�[�^���͂̂��߂̏���
     *  
     */
    public void prepare()
    {
        // �������Ȃ�        
    }

    /**
     *  ����ʂ̐؂�ւ�
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == A2chSSceneDB.WELCOME_SCREEN)
        {
            return (A2chSSceneDB.DIR_SCREEN);
        }
        else if (screenId == A2chSSceneDB.DIR_SCREEN)
        {
            return (A2chSSceneDB.DEFAULT_SCREEN);
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
        inputDataField = new TextField("����������", "", MaxDataInputLength, TextField.ANY);
        return (inputDataField);
    }

    /**
     *  �I�v�V�����̌����擾����
     *  
     */
    public int getNumberOfOptionGroup()
    {
        return (0);
    }

    /**
     *   �I�v�V�������擾����
     */
    public ChoiceGroup getOptionField(int index)
    {
        return (null);
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
     *  2ch�����̎��s�B�B�B
     * 
     * @param url
     */
    public void doFind2chThreads()
    {
    	httpCommunicator.SetUserAgent("a2chS/1.00");
        httpCommunicator.DoHttpMain(find2chUrl, storageDb.getBaseDirectory(), "temporary.html");
        find2chUrl = null;
        dataProcessing.finishUpdateData(0, 0, 300);
        return;
    }

    /**
     *   �e���|�����t�@�C���i�������ʃt�@�C���j�����݂��邩�H
     * 
     * @return
     */
    public boolean isExistSearchResultFile()
    {
        return (MidpFileOperations.IsExistFile(storageDb.getBaseDirectory() + "temporary.html"));
    }

    /**
     *  �������ʃt�@�C����ǂݍ���ŏ�������
     * 
     */
    public boolean prepareSearchResultFile()
    {
    	int nofThreadTitles = 0;
    	byte[] data = fileOperation.ReadFileWhole(storageDb.getBaseDirectory() + "temporary.html");
    	if (data == null)
    	{
            // �t�@�C���ǂݏo�����s...
    		return (false);
    	}

        // 2ch���X�g
        pickup2chList = null;
        System.gc();
        pickup2chList = new Vector();

        // �f�[�^��̓��C������...
    	nofThreadTitles = parseFind2ch(data);

        // ���Ƃ��܂�
        data = null;
        System.gc();
        if (nofThreadTitles == 0)
        {
        	return (false);
        }
    	return (true);
    }

    /**
     * �Q�����˂錟���̌��ʂ���͂��� (a2B�������Ă���)
     * 
     * @param dataString
     */
    private int parseFind2ch(byte[] dataString)
    {
        int firstIndex = 0;

        // �������J�n����擪��T��
        try
        {
            int endIndex   = dataString.length - 4;
            for (int i = 0; i < endIndex ; i++)
            {
                if ((dataString[i] == '<')&&(dataString[i + 1] == 'd')&&(dataString[i + 2] == 't')&&(dataString[i + 3] == '>'))
                {
                    firstIndex = i;
                    break;
                }
            }
        }
        catch (Exception e)
        {
            //
        }

        int index = firstIndex;
        while ((index >= 0)&&(index < dataString.length))
        {
            index = pickupFind2chData(dataString, index);
        }
        return (pickup2chList.size());
    }

    /**
     * �Q�����˂錟���ŏE�����AURL���Ƃ��Ă���
     * 
     * @param buffer
     * @param startIndex
     * @return
     */
    private int pickupFind2chData(byte[] buffer, int startIndex)
    {
        int threadNumIndexStart   = startIndex;
        int threadNumIndexEnd     = startIndex;
        int threadNickIndexEnd    = startIndex;
        int threadTitleIndexStart = startIndex;
        int threadTitleIndexEnd   = startIndex;
        int boardHostIndexStart   = startIndex;
        int boardHostIndexEnd     = startIndex;
        int endIndex = buffer.length;

        try
        {
            boolean anchor = false;
        	for (int index = startIndex; index < buffer.length; index++)
            {
        		if ((buffer[index + 0] == ':')&&(buffer[index + 1] == '/')&&(buffer[index + 2] == '/'))
                {
                	boardHostIndexStart = index + 3;
                	for (int numberIndex = boardHostIndexStart; numberIndex < buffer.length; numberIndex++)
                	{
                        if (buffer[numberIndex] == '/')
                        {
                            // �z�X�g���̖���
                        	boardHostIndexEnd = numberIndex;
                        	index = numberIndex;
                        	break;
                        }
                	}
                }
                else if ((buffer[index + 0] == 'r')&&(buffer[index + 1] == 'e')&&
                    (buffer[index + 2] == 'a')&&(buffer[index + 3] == 'd')&&
                    (buffer[index + 4] == '.')&&(buffer[index + 5] == 'c')&&
                    (buffer[index + 6] == 'g')&&(buffer[index + 7] == 'i')&&
                    (buffer[index + 8] == '/'))
                {
                    // "read.cgi/" ��������
                    threadNumIndexStart = index + 9;
                    boolean firstSlash = false;  // ���[�A���_���_�A�t���O�Ȃ�āA�A�A�B
                    for (int numberIndex =threadNumIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '/')
                         {
                            if (firstSlash == true)
                            {
                                threadNumIndexEnd = numberIndex;
                                index = numberIndex;
                                anchor = true;
                                break;
                            }
                            else
                            {
                                firstSlash = true;
                                threadNickIndexEnd = numberIndex;
                            }
                         }
                    }
               
                }

                //                else if ((buffer[index + 0] == '<')&&(buffer[index + 1] == 'b')&&(buffer[index + 2] == '>'))
                else if (anchor == true)
                {
                	while ((buffer[index] != '>')&&(index < buffer.length))
                	{
                		index++;
                	}
                	index++;
                    threadTitleIndexStart = index;
                    for (int titleIndex =threadTitleIndexStart; titleIndex < buffer.length; titleIndex++)
                    {
                        if ((buffer[titleIndex + 0] == '<')&&(buffer[titleIndex + 1] == '/')&&
                             (buffer[titleIndex + 2] == 'a')&&(buffer[titleIndex + 3] == '>'))
                        {
                            threadTitleIndexEnd = titleIndex; 
                            break;
                        }
                    }

                    // �����Ń^�C�g�����G���g������
                    if ((threadNumIndexStart < threadNumIndexEnd)&&
                        (threadTitleIndexStart < threadTitleIndexEnd)&&
                        (threadNumIndexStart < threadTitleIndexStart))
                    {
                        String title  = kanjiConverter.ParseFromEUC(buffer, threadTitleIndexStart, (threadTitleIndexEnd - threadTitleIndexStart));
                        String host   = new String(buffer, boardHostIndexStart, (boardHostIndexEnd - boardHostIndexStart));
                        String number = new String(buffer, (threadNickIndexEnd + 1), (threadNumIndexEnd - (threadNickIndexEnd + 1)));
                        String nick   = new String(buffer, threadNumIndexStart, (threadNickIndexEnd - threadNumIndexStart));
                        title = "[" + nick + "]"+ title;
                        pickup2chList.addElement(new a2chSThreadInformation(title, host, nick, number));
                    }
                    endIndex = threadTitleIndexEnd;
                    break;
                }
            }
        	while ((buffer[endIndex] != 0x0a)&&(endIndex < buffer.length))
        	{
        		endIndex++;
        	}
        }
        catch (Exception e)
        {
            endIndex = -1;
        }
        return (endIndex);        
    }

    /**
     *  �X���^�C������������
     * 
     * @return
     */
    public int getNumberOfThreadTitles()
    {
    	if (pickup2chList == null)
    	{
    		return (0);
    	}
        return (pickup2chList.size());
    }

    /**
     *   �X��������������
     * 
     * @param itemNumber
     * @return
     */
    public a2chSThreadInformation getThreadInformation(int itemNumber)
    {
    	try
    	{
            // �͈̓I�[�o�[�`�F�b�N
        	if ((itemNumber < 0)||(itemNumber >= pickup2chList.size()))
        	{
        		return (null);
        	}
        	a2chSThreadInformation threadInfo = (a2chSThreadInformation) pickup2chList.elementAt(itemNumber);
        	return (threadInfo);
    	}
    	catch (Exception e)
    	{
    		//
    	}
    	return (null);
    }

    /**
     *  �X������http�ʐM�Ŏ擾����
     * 
     * @param threadInfo
     * @return ����(true) / ���s(false)
     */
    public boolean getDatFile(a2chSThreadInformation threadInfo)
    {
        setInformationMessage("datFile�擾��...");

        // �f�B���N�g���𒲐�����
    	String directory = storageDb.getBaseDirectory() + threadInfo.getBoardNick();
        if (directory == null)
        {
        	return (false);
        }
        // �f�B���N�g���𐶐�����
        fileOperation.MakeDirectory(directory);
        
        // �t�@�C����
        String fileName = threadInfo.getDatFileName();

        // dat�t�@�C�������݂��Ă��邩�ǂ����`�F�b�N
        boolean fileIsExist = MidpFileOperations.IsExistFile(directory + fileName);

        // ���[�U�[�G�[�W�F���g�𒲐�����
        httpCommunicator.SetUserAgent("Monazilla/1.00  (MIDP2.0)");

        // �ʐM���s����dat�t�@�C�����擾����
        httpCommunicator.DoHttpMain(threadInfo.getUrl(), directory, fileName);

        // ���Ƀt�@�C�������������H
        if (fileIsExist == false)
        {
            // �t�@�C�����Ȃ����a2b.idx�ɒǉ�����
        	// (�{����index�t�@�C���ɋL�^�����邩�Ȃ����`�F�b�N�K�v)
            appendToA2Bindex(directory, fileName);
        }
        setInformationMessage("�擾�I�� : " + threadInfo.getTitle());
//        dataProcessing.updateInformation("�擾�I�� :" + threadInfo.getTitle(), 0, false);
        dataProcessing.finishUpdateData(0, 0, 300);
        return (true);
    }

    /**
     *  a2b.idx�Ɏ擾�����t�@�C�����L�^����Ă��Ȃ���ΒǋL����
     * 
     * @param directory �Ǘ��f�B���N�g��
     * @param datFile dat�t�@�C��
     */
    private void appendToA2Bindex(String directory, String datFile)
    {
    	String targetFileName = directory + "a2b.idx";

        // �^�C�g���s���K�v���H
    	String title = null;
    	if (MidpFileOperations.IsExistFile(targetFileName) == false)
    	{
    		// �t�@�C�����Ȃ�...a2b.idx��V�K�ɐ�������ꍇ�ɂ̓w�b�_�𐶐�����
			title = "; FILEname, max, current, status, lastModified, 0, (Title)";
    	}

    	// �f�[�^�s�̐���
    	String dataToWrite = datFile + ",1,1,1,0,0,(a2B_INTERIM)" + datFile + "\r\n; [End]";
        
        // �C���f�b�N�X�t�@�C���ւ̒ǋL�����s�I
    	fileOperation.outputDataTextFile(targetFileName, null, title, dataToWrite, true);
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
     *  �X�����i�[�N���X
     *  <br>2ch�����̌��ʂ�ێ�����
     *  <i>�X���^�C</i>
     *  <i>�z�X�g��</i>
     *  <i>�j�b�N�l�[��</i>
     *  <i>dat�t�@�C����</i>
     * @author MRSa
     *
     */
    public class a2chSThreadInformation
    {
        private String title  = null;
        private String host   = null;
        private String nick   = null;
        private String datNum = null;    	
    	
    	/**
    	 *   �R���X�g���N�^
    	 * @param titleString  �X���^�C�g��
    	 * @param hostString   �z�X�g��
    	 * @param nickString   �j�b�N�l�[��
    	 * @param datNumString dat�ԍ�
    	 */
    	public a2chSThreadInformation(String titleString, String hostString, String nickString, String datNumString)
    	{
            title  = titleString;
            host   = hostString;
            nick   = nickString;
            datNum = datNumString;
    	}
    	
    	/**
    	 *  �X���^�C�g������������
    	 * @return �X���^�C�g��������(UTF8�`���ɕϊ��ς�)
    	 */
    	public String getTitle()
    	{
    		return (title);
    	}

    	/**
    	 *  dat�t�@�C����URL����������
    	 * @return dat�t�@�C����URL 'http://(�z�X�g)/(nick)/dat/(dat�ԍ�).dat'
    	 */
    	public String getUrl()
    	{
    	    return ("http://" + host + "/" + nick + "/dat/" + datNum + ".dat");
    	}

    	/**
    	 *  ��nick������������(�����ɃX���b�V������I)
    	 * @return ��nick '(���O)'
    	 */
    	public String getBoardNick()
    	{
    		return (nick + "/");
    	}

        /**
         *  dat�t�@�C�������擾����
         * @return dat�t�@�C���� (xxxxxxxxxx.dat)
         */
    	public String getDatFileName()
    	{
    		return (datNum + ".dat");
    	}
    }
}
