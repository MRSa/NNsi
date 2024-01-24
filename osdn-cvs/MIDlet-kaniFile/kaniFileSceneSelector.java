import java.util.Vector;
import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;
import javax.microedition.lcdui.Image;

/**
 * �V�[���Z���N�^
 * 
 * @author MRSa
 *
 */
public class kaniFileSceneSelector
{
    private final int  SCENE_DEFAULT  = kaniFileCanvas.SCENE_FILELIST;
    
    private final int SCENE_NOTSPECIFY          = -1;
    private final int SCENE_DELETE              = 200;
    private final int SCENE_COPYFILE            = 201;
    private final int SCENE_SAVETEXTFILE        = 202;
    private final int SCENE_OUTPUT_FILELIST     = 203;
    private final int SCENE_EXTRACT_ZIP         = 204;
    private final int SCENE_EXTRACT_ZIP_SAMEDIR = 205;

	private MIDlet          parent  = null;
	private kaniFileFactory factory = null;

	private boolean zipSame = false;
	private int    currentScene     = SCENE_DEFAULT;
	private String  showDirectory   = null;
	private String  memoryDirectory = null;
	private String  lastName        = null;
	private String  changeName      = null;
	private String  infoMessage     = null;
	private int    lastIndex          = -1;
	private int    selectedFileCount  = 0;
	private int    memoryFileCount    = 0;
	private int    confirmationMode   = SCENE_NOTSPECIFY;
	private int    currentIndexNumber = -1;
	private int    fileOperationInfo  = -1;
    private Vector targetFileList     = null;
	
	/**
	 * �R���X�g���N�^
	 * @param parent
	 */
	kaniFileSceneSelector(MIDlet object)
	{
		parent  = object;
		factory = new kaniFileFactory();
	}

	/**
	 * �N������
	 * 
	 * @return
	 */
	public boolean initialize()
	{
		// �������̏���...
		factory.prepare(this);
		
		// ��ʕ\���̏���...
		factory.prepareScreen();

		// �O��̃f�B���N�g�����v���o��
		showDirectory = (factory.getDataStorage()).getDirectory();

		System.gc();

		return (true);
	}
	
	/**
	 *  ������ʂɃV�[����؂�ւ���
	 *
	 */
	public void start()
	{
		changeScene(SCENE_DEFAULT, "");
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
		return;
	}

	/**
	 *  ������ʂɃV�[����؂�ւ���
	 *
	 */
	public void stop()
	{
		// �f�B���N�g�����L�^����
		(factory.getDataStorage()).setDirectory(showDirectory);

		factory.cleanup();
		return;
	}

	/**
	 *   �A�v���P�[�V�������I��������
	 *
	 */
	public void finishApp()
	{

		try
		{
			// �f�[�^�̃o�b�N�A�b�v�ƃI�u�W�F�N�g�̍폜�����s����
			stop();

			// �K�x�R���̎��{
	        System.gc();

			// �A�v���P�[�V�����̏I��...
			parent.notifyDestroyed();
		}
		catch (Exception ex)
		{
			//
		}
		return;
	}
	
	/**
	 *    URL��Web�u���E�U�ŊJ��
	 *     (�A�v���I����Ƀu���E�U���N������)
	 * 
	 *    @param url �u���E�U�Őڑ�����URL
	 * 
	 */
	private void switchToWebBrowser(String url)
	{
		try
		{
			// URL�̃I�[�v���w�����s��
			parent.platformRequest(url);

			// �f�[�^�̃o�b�N�A�b�v�ƃI�u�W�F�N�g�̍폜�����s����
			stop();

			// �K�x�R���̎��{
            System.gc();

			// �A�v���P�[�V�����̏I��...
			parent.notifyDestroyed();
		}
		catch (Exception e)
		{
			/////// �u���E�U�N���͖��T�|�[�g�������A�������Ȃ�  /////
		}
		return;
	}

	/**
	 *  ��ʕ\���ؑփX���b�h���s
	 *
	 */
	private void changeScene(int scene, String message)
	{
		currentScene = scene;
		infoMessage  = message;
		Thread thread = new Thread()
		{
			public void run()
			{
				//
                if (isShowCanvasForm(currentScene) == true)
                {
                	// ��ʃV�[����ύX����...
                	(factory.getCanvas()).changeScene(currentScene, infoMessage);
                	
            		// �L�����o�X�N���X�ɐ؂�ւ���...
            		Display.getDisplay(parent).setCurrent(factory.getCanvas());
                }
                else
                {
                	// ��ʃV�[����ύX����...
                	(factory.getForm()).changeScene(currentScene, infoMessage);

                	// ���̓t�H�[���ɐ؂�ւ���...
            		Display.getDisplay(parent).setCurrent(factory.getForm());                	
                }
			}
		};
		thread.start();
		return;
	}
	
	/**
	 * �L�����o�X��\�����邩�A�t�H�[����\�����邩...
	 * @return
	 */
	private boolean isShowCanvasForm(int scene)
	{
        // �\���V�[���ɍ��킹�āA�L�����o�X�t�H�[���ɂ���(true)���A���̓t�H�[���ɂ���(false)�����肷��
		if (scene == kaniFileForm.SCENE_RENAMEINPUT)
		{
			return (false);
		}	
		if (scene == kaniFileForm.SCENE_MAKEDIR)
		{
			return (false);
		}
		if (scene == kaniFileForm.SCENE_DUPLICATEINPUT)
		{
			return (false);
		}
		if (scene == kaniFileForm.SCENE_EDITATTRIBUTE)
		{
			return (false);
		}
		if (scene == kaniFileForm.SCENE_TEXTEDITLINE)
		{
			return (false);
		}
		return (true);
	}

	/**
	 * ���݂̃f�B���N�g��������������
	 * @return
	 */
    public String getCurrentDirectory()
    {
    	return ((factory.getDirectoryInfo()).getDirectoryName());
    }
    
    /**
     * �f�B���N�g���̈ړ�...
     * @param pathName
     */
    public void moveDirectory(String pathName)
    {
		if (showDirectory == null)
		{
			showDirectory = "file:///";
		}
    	showDirectory = showDirectory + pathName;
    	selectedFileCount = 0;
        Thread thread = new Thread()
		{
			public void run()
			{
				(factory.getCanvas()).startUpdate("Checking " + showDirectory);
				boolean ret = (factory.getDirectoryInfo()).scanDirectory(showDirectory);
				if (ret == false)
				{
					// �f�B���N�g���̌����Ɏ��s����...
					showDirectory = "file:///";
                    ret = (factory.getDirectoryInfo()).scanDirectory(showDirectory);
				}
				if (ret != false)
				{
					(factory.getCanvas()).updateData();
				}
			}
		};
		thread.start();
        return;    	
    }

    /**
     * �f�B���N�g���̈ړ�...
     * @param pathName
     */
    public void upDirectory()
    {
        Thread thread = new Thread()
		{
			public void run()
			{
				(factory.getCanvas()).startUpdate("Checking ..");
				(factory.getDirectoryInfo()).upDirectory();
            	(factory.getCanvas()).updateData();
            	showDirectory = (factory.getDirectoryInfo()).getDirectoryName();
            	selectedFileCount = 0;
			}
		};
		thread.start();
        return;    	
    }

    /**
     * ���݂̃f�B���N�g���ɂ���t�@�C�������擾����
     * @return
     */
    public int getNumberOfFiles()
    {
    	return ((factory.getDirectoryInfo()).getCount());
    }

    /**
     * �t�@�C�������擾����
     * @param index
     * @return
     */
    public kaniFileDirList.fileInformation getFileInfo(int index)
    {
    	kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(index);
    	return (info);
    }
    
    /**
     * �t�@�C�����I�����ꂽ/�I���������ꂽ�Ƃ�...
     * @param fileName
     * @param isSelected
     */
    public void selectedFile(String fileName, int index, boolean isSelected)
    {
        if (isSelected == true)
        {
        	selectedFileCount++;
        	lastName  = fileName;
        	lastIndex = index;
        }
        else
        {
        	selectedFileCount--;
        }    	
    	return;
    }

    /**
     * ���I���������ꍇ�A�t�@�C����I������
     * @param fileName
     * @param index
     * @return
     */
    public boolean selectedFile(String fileName, int index)
    {
    	if (selectedFileCount == 0)
    	{
    		selectedFileCount++;
    		lastName  = fileName;
    		lastIndex = index;
    		return (true);
    	}
    	return (false);
    }

    /**
     * �S�I��/�S�I������
     *
     */
    public void selectAllFiles(boolean isSelected)
    {
    	selectedFileCount = (factory.getDirectoryInfo()).selectAllFiles(isSelected);
    	if (isSelected == false)
    	{
    		selectedFileCount = 0;
    	}
    	return;
    }
    
    /**
     * ���݂̃f�B���N�g�����L������...
     *
     */
    public void memoryCurrentDirectory()
    {
    	memoryDirectory = showDirectory;
    	return;
    }

    /**
     * �f�[�^���͂��L�����Z������...
     *
     */
    public void cancelForm()
    {
    	if (currentScene == kaniFileForm.SCENE_TEXTEDITLINE)
    	{
			// ��ʃV�[�����e�L�X�g�ҏW���[�h�ɕύX����...
    		changeTextEditScreen();
    		return;
    	}

    	showDirectory = memoryDirectory;
		currentScene = SCENE_DEFAULT;
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
    	changeScene(SCENE_DEFAULT, "");
		return;
    }

    /**
     *  �e�L�X�g�ҏW���[�h�ɖ߂�...
     *
     */
    public void cancelCommandInputTextEdit()
    {
		// ��ʃV�[�����e�L�X�g�ҏW���[�h�ɕύX����...
		changeTextEditScreen();
    	return;
    }

    /**
     * �\����ʂ��L�����Z������...
     *
     */
    public void cancelPictureView()
    {
    	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
		currentScene = SCENE_DEFAULT;
//		(factory.getDirectoryInfo()).setScanDirectoryMode(false);		
		return;
    }

    /**
     * �R�}���h���͂��L�����Z������...
     *
     */
    public void cancelCommandInput()
    {
    	showDirectory = memoryDirectory;
    	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
		currentScene = SCENE_DEFAULT;
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);		
		return;
    }

    /**
     * �f�B���N�g���I����͂��L�����Z������...
     *
     */
    public void cancelCommandDirInput()
    {
    	showDirectory = changeName;
    	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
		currentScene = SCENE_DEFAULT;
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);		
		return;
    }

    /**
     * �摜�\�����[�h���w�肵���Ƃ��A�A
     * @return
     */
    public boolean selectedPictureView()
    {
    	changeScene(kaniFileCanvas.SCENE_PICTUREVIEW, lastName);
    	return (false);
    }
    
    /**
     * �f�B���N�g���쐬�w�����󂯂�...
     * @return
     */
    public boolean selectedMkDir()
    {
    	changeScene(kaniFileForm.SCENE_MAKEDIR, "");
    	return (false);
    }

    /**
     * �t�@�C���̏o��..
     * 
     * 
     * @return
     */
    public boolean selectedOutputFileList()
    {
    	if (pickupCopyFile(showDirectory, true) <= 0)
    	{
    		return (false);
    	}
    	String message = "file�ꗗ��filelist.html��(�㏑��)�o�͂��܂��B";
    	confirmationMode = SCENE_OUTPUT_FILELIST;
    	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);
    	return (false);
    }
    
    /**
     * �t�@�C�������擾����
     * @param index
     * @return
     */
    public void updateFileDetail(int index)
    {
    	currentIndexNumber = index;
        Thread thread = new Thread()
		{
			public void run()
			{
		    	kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(currentIndexNumber);
		    	info.updateDetailInfo();

		    	// �`��X�V�w��...
	    		(factory.getCanvas()).updateInformation("", 0);
		    	currentIndexNumber = -1;
			}
		};
		thread.start();
		return;
    }

    private boolean checkSingleTargetFile()
    {
    	if (selectedFileCount != 1)
    	{
    		return (false);
    	}
		if (lastName.equals(".."))
		{
			return (false);
		}
        return (true);    	
    }
    
    /**
     * ���l�[���w����������...
     * @return
     */
    public boolean selectedRename()
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			int pos = lastName.lastIndexOf('/');
			lastName = lastName.substring(0, pos);
		}
    	(factory.getForm()).setTargetName(lastName);
    	changeScene(kaniFileForm.SCENE_RENAMEINPUT, "");
    	return (false);
    }

    /**
     * �t�@�C���R�s�[���I�����ꂽ�ꍇ...
     * @return
     */
    public boolean selectedCopy()
    {
    	if (pickupCopyFile(showDirectory, false) <= 0)
    	{
    		return (false);
    	}
    	String message = "Copy��Dir��'���s'�������ĉ�����.";
    	changeName = showDirectory;
		(factory.getDirectoryInfo()).setScanDirectoryMode(true);
    	changeScene(kaniFileCanvas.SCENE_DIRLIST, message);
    	return (false);
    }

    /**
     * �R�s�[����t�@�C�������擾����
     * @param path      �擾�f�B���N�g��
     * @param isAllFile �S�Ẵt�@�C��
     * @return
     */
    private int pickupCopyFile(String path, boolean isAllFile)
    {
    	memoryFileCount = 0;
    	targetFileList = null;
    	targetFileList = new Vector();
    	int count = (factory.getDirectoryInfo()).getCount();
    	for (int loop = 0; loop < count; loop++)
    	{
    		kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(loop);
            if (info.isDirectory() == false)
            {    		
    		    if ((isAllFile == true)||(info.isSelected() == true))
    		    {
	        	    // �R�s�[�t�@�C���Ƃ��ēo�^����
    			    String fileName = info.getFileName();
    			    targetFileList.addElement(fileName);
    			    memoryFileCount++;
    		    }
    		}
    	}
    	return (memoryFileCount);
    }

    /**
     * Web�u���E�U�ŊJ���A���w�������ꍇ...
     * 
     * @return
     */
    public boolean selectedOpenWebBrowser()
    {
    	if (checkSingleTargetFile() == false)
    	{
            // �t�@�C���������I������Ă����Ƃ��́A�������Ȃ�
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			return (false);
		}

        // �`�F�b�N...
		String platForm = System.getProperty("microedition.platform");
        if ((platForm.indexOf("WX34") >= 0)||(platForm.indexOf("WX01K") >= 0))
        {
            // WX340K or WX341K�̎��ɂ́A�f�B���N�g����␳����B�B�B
			String fileName = showDirectory + lastName;
        	if (fileName.indexOf("file:///data/") >= 0)
        	{
        		// �f�[�^�t�H���_�̏ꍇ...
        		switchToWebBrowser("file:///D:/" + fileName.substring(8)); //   "file:///" ��u��
        	}
        	else
        	{
        	    // SD�J�[�h�̏ꍇ...
        		switchToWebBrowser("file:///F:/" + fileName.substring(12)); //  "file:///SD:/" ��u��
        	}
        }
		else if ((platForm.indexOf("WS023T") >= 0)||(platForm.indexOf("WS018KE") >= 0)||(platForm.indexOf("WS009KE") >= 0))
        {
            // WS023T or WS018KE�̎��ɂ́A�f�B���N�g����␳����B�B�B
			String fileName = showDirectory + lastName;
            switchToWebBrowser("file:///affm/nand0/PS/data/" + fileName.substring(19)); // "file:///DataFolder/" ��u��
        }
        else if ((platForm.indexOf("WX320K") >= 0)||(platForm.indexOf("WX33") >= 0)||(platForm.indexOf("WX35") >= 0))
        {
            // WX320K�̎��ɂ́A�f�B���N�g����␳����B�B�B
			String fileName = showDirectory + lastName;
            switchToWebBrowser("file://localhost/D:/" + fileName.substring(8));
        }
		else
		{
            // �u���E�U�ŊJ��
            switchToWebBrowser(showDirectory + lastName);
        }
    	return (false);
    }
    
    /**
     * �t�@�C���R�s�[���w�����ꂽ�ꍇ...
     * 
     * @return
     */
    public boolean selectedCopyDirectory()
    {
    	String message = memoryFileCount + "�t�@�C���̃R�s�[�����s���܂��B";
    	confirmationMode = SCENE_COPYFILE;
    	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);
    	return (false);
    }

    /**
     * �t�@�C��(Dir)�폜���w�����ꂽ�ꍇ...
     * 
     * @return
     */
    public boolean selectedDelete()
    {
    	String message = selectedFileCount + "�A�C�e���̍폜�����s���܂��B";
    	confirmationMode = SCENE_DELETE;
    	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);
    	return (false);
    }


    /**
     *  �t�@�C����ZIP�𓀂��w�����ꂽ�ꍇ...
     * @return
     */
    public boolean selectedExtractZip(boolean isSameDirectory)
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			return (false);
		}
    	(factory.getForm()).setTargetName(lastName);
    	String message = "ZIP�W�J";
    	if (isSameDirectory == true)
    	{
    		message = message + "(����Dir)";
    	}
    	message = message + ":" + lastName;
    	if (isSameDirectory == true)
    	{
    		confirmationMode = SCENE_EXTRACT_ZIP_SAMEDIR;
    	}
    	else
    	{
    	    confirmationMode = SCENE_EXTRACT_ZIP;
    	}
    	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);    	
    	return (false);
    }
    
    /**
     * �t�@�C���̕����w��...
     * @return
     */
    public boolean selectedDuplicate()
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			return (false);
		}
    	(factory.getForm()).setTargetName(lastName);
    	changeScene(kaniFileForm.SCENE_DUPLICATEINPUT, "");
    	return (false);
    }

    /**
     * �I�������t�@�C����ҏW����
     * 
     *
     */
    public boolean selectedEdit()
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			return (false);
		}
		executeEditText();
    	return (false);
    }
    
    /**
     * �e�L�X�g�ҏW���ɃR�}���h���[�h�ɐ؂�ւ���...
     * 
     * @return
     */
    public boolean selectedEditTextCommand()
    {
		// ��ʃV�[����ύX����...
    	(factory.getCanvas()).changeScene(kaniFileCanvas.SCENE_TEXTEDIT_COMMAND, lastName);
    	
		// �L�����o�X�N���X�ɐ؂�ւ���...
		Display.getDisplay(parent).setCurrent(factory.getCanvas());

		(factory.getCanvas()).updateInformation("<<< ����I�� >>>", -1);
    	
		return (true);
    }
    
    /**
     * �I�������t�@�C���̑�����ҏW����
     * 
     *
     */
    public boolean selectedEditAttribute()
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
    	
        Thread thread = new Thread()
		{
			public void run()
			{
                // �I�������t�@�C���̃t�@�C�������X�V���āA�����ҏW��ʂ��J��...
				kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(lastIndex);
		    	info.updateDetailInfo();
		    	(factory.getForm()).setFileInfo(info);
		    	changeScene(kaniFileForm.SCENE_EDITATTRIBUTE, "");
			}
		};
		thread.start();
    	
    	return (false);
    }

    /**
     *  �T�E���h�̍Đ��E��~ (�g���K)
     * 
     */
    public boolean selectedPlayStopSound()
    {
		Thread thread = new Thread()
		{
			public void run()
			{
				// ����炷
				playSound();
//		    	vibrate(400);
		    	System.gc();

		    	// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());
			}
		};
		thread.start();
		return (false);
    }

    /**
     * ���͌��ʂ̔��f...
     * @param fileName
     */
    public void executeInputForm(String fileName, int informations)
    {
    	if (currentScene == kaniFileForm.SCENE_MAKEDIR)
    	{
    		// �f�B���N�g���쐬
    		executeMakeDirectory(fileName);
    		return;
    	}
    	if (currentScene == kaniFileForm.SCENE_RENAMEINPUT)
    	{
    		// �t�@�C�����ύX
    		executeRenameFile(fileName);
    		return;
    	}
    	if (currentScene == kaniFileForm.SCENE_DUPLICATEINPUT)
    	{
    		// �t�@�C������
    		executeDuplicateFile(fileName);
    		return;
    	}
    	if (currentScene == kaniFileForm.SCENE_EDITATTRIBUTE)
    	{
    		// �t�@�C���̑����ύX
    		executeEditAttribute(informations);
    		return;
    	}
    	return;
    }

    /**
     * �f�B���N�g���̍쐬���C��
     * @param fileName
     */
    private void executeMakeDirectory(String fileName)
    {
		lastName = fileName;
        Thread thread = new Thread()
		{
			public void run()
			{
				// �f�B���N�g�����쐬����
//				(factory.getFileOperation()).makeDirectory(showDirectory + lastName);
				kaniFileOperations.MakeDirectory(showDirectory + lastName);

				// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName = null;
			}
		};
		thread.start();
		return;
    }

    /**
     * �t�@�C�����̕ύX���C��
     * @param fileName
     */
    private void executeRenameFile(String fileName)
    {
		changeName = fileName;
        Thread thread = new Thread()
		{
			public void run()
			{
				// �t�@�C�������l�[������
				(factory.getFileOperation()).renameFile(showDirectory + lastName, changeName);

				// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
			}
		};
		thread.start();
		return;
    }

    /**
     * �t�@�C���̕������C��
     * @param fileName
     */
    private void executeDuplicateFile(String fileName)
    {
		changeName = fileName;
        Thread thread = new Thread()
		{
			public void run()
			{
				// �t�@�C���𕡐�����...
				(factory.getFileOperation()).copyFile(showDirectory + changeName, showDirectory + lastName);

				// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
			}
		};
		thread.start();
		return;
    }

    /**
     * �t�@�C���̕������C��
     * @param fileName
     */
    private void executeEditAttribute(int attribute)
    {
    	fileOperationInfo = attribute;
        Thread thread = new Thread()
		{
			public void run()
			{
				boolean hidden   = false;
				boolean writable = true;  // ��ʂ̃t���O��writable�t���O�́A�t�ɂȂ��Ă��邱�Ƃɒ���

				// �������ݐ�p�t�@�C���ݒ���Z�b�g (�� ��ʂ̃t���O�� writable�t���O�͋t�ɂȂ��Ă��邱�Ƃɒ���)
				if ((fileOperationInfo == 1)||(fileOperationInfo == 3))
				{
					writable = false; // writable �� false �F �������݋֎~�ݒ�
				}

				// �B���t�@�C���ݒ���Z�b�g
				if ((fileOperationInfo == 2)||(fileOperationInfo == 3))
				{
					hidden = true;  // hidden �� true �F �B���t�@�C������
				}

				// �t�@�C���𕡐�����...
				(factory.getFileOperation()).changeAttribute((showDirectory + lastName), writable, hidden);

				// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName          = null;
        		changeName        = null;
        		fileOperationInfo = -1;
        		lastIndex         = -1;
			}
		};
		thread.start();
		return;
    }
    
    /**
     * �폜�̎��s
     *
     */
    private void executeDelete()
    {
        Thread thread = new Thread()
		{
			public void run()
			{
	        	int count = (factory.getDirectoryInfo()).getCount();
	        	for (int loop = 0; loop < count; loop++)
	        	{
	        		kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(loop);
	        		if (info.isSelected() == true)
	        		{
	    	        	// �t�@�C�����폜����
	        			String fileName = showDirectory + info.getFileName();
	    				(factory.getFileOperation()).deleteFile(fileName);
	        		}
	        	}

				// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
			}
		};
		thread.start();    	
    }

    
    /**
     *  ZIP�t�@�C���W�J�̎��s
     * 
     *
     */
    private void executeExtractZip(boolean isSame)
    {
    	zipSame = isSame;
        Thread thread = new Thread()
		{
			public void run()
			{
				// ZIP�t�@�C����W�J���郍�W�b�N���Ăяo��
				(factory.getZipOperation()).extractZipFile(showDirectory, lastName, zipSame);

				// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());
			}
		};
		try
		{
			thread.start();
//			thread.join();
		}
		catch (Exception ex)
		{
			//
		}
    }

    /**
     *  ZIP�t�@�C���W�J�󋵂���ʕ`�悷��
     *
     */
    public void updateProgressForZipExtract()
    {
    	String data = (factory.getZipOperation()).getCurrentStatus();
    	(factory.getCanvas()).updateInformation("ZIP�W�J��... " + data, -1);
    	(factory.getCanvas()).redraw();
    	return;
    }
    
    /**
     *  �t�@�C���ꗗ�̏o��...
     * 
     *
     */
    private void executeOutputFileList()
    {
        Thread thread = new Thread()
		{
			public void run()
			{
				try
				{
			        (factory.getFileOperation()).outputFileListAsHtml((showDirectory + "fileList.html"), targetFileList);
				}
				catch (Exception e)
				{
					// �������Ȃ�...
				}

				// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
				targetFileList = null;
				System.gc();
			}
		};
		thread.start();
		return;
    }
    
    /**
     * �R�s�[�̎��s
     *
     */
    private void executeCopy()
    {
        Thread thread = new Thread()
		{
			public void run()
			{
				try
				{
					int count = targetFileList.size();
					for (int loop = 0; loop < count; loop++)
					{
				        String fileName = (String) targetFileList.elementAt(loop);
				        (factory.getFileOperation()).copyFile((showDirectory + fileName), (changeName + fileName));
					}
				}
				catch (Exception e)
				{
					// �������Ȃ�...
				}

				// ��ʃV�[����ύX����...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// �L�����o�X�N���X�ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
				targetFileList = null;
				System.gc();
			}
		};
		thread.start();
		return;
    }

    /**
     *  �e�L�X�g�ҏW�t�@�C���̕ҏW���s
     * 
     */
    private void executeEditText()
    {
		Thread thread = new Thread()
		{
			public void run()
			{
                // �I�������t�@�C�����������ɓW�J����
				if ((factory.getTextDataStorage()).openFile(showDirectory + lastName) > 0)
				{
					// ��ʃV�[����ύX����...
		    		changeTextEditScreen();
				}			
			}
		};
		thread.start();
		
		return;
    }

    /**
     * �e�L�X�g�f�[�^�t�@�C���̕\���ʒu�擾
     * 
     * @return
     */
    public kaniFileTextFile getTextFileData()
    {
    	return ((factory.getTextDataStorage()));
    }
    
    /**
     * �R�}���h�̎��s�w��...
     * 
     * @return
     */
    public boolean confirmation()
    {
    	if (confirmationMode == SCENE_DELETE)
    	{
    		// �t�@�C���폜�̎��s...
    		(factory.getCanvas()).updateInformation("�폜��...", -1);
    		executeDelete();
    		return (true);
    	}
    	if (confirmationMode == SCENE_EXTRACT_ZIP)
    	{
    		// ZIP�t�@�C���W�J�̎��s...
    		(factory.getCanvas()).updateInformation("ZIP�W�J��...", -1);
    		executeExtractZip(false);
    		return (true);    		
    	}
    	if (confirmationMode == SCENE_EXTRACT_ZIP_SAMEDIR)
    	{
    		// ZIP�t�@�C���W�J�̎��s...
    		(factory.getCanvas()).updateInformation("ZIP�W�J(��Dir)...", -1);
    		executeExtractZip(true);
    		return (true);
    	}
    	if (confirmationMode == SCENE_COPYFILE)
    	{
    		// �t�@�C���R�s�[�̎��s...
    		(factory.getCanvas()).updateInformation("�R�s�[��...", -1);
    		executeCopy();
    		return (true);    		
    	}
    	if (confirmationMode == SCENE_OUTPUT_FILELIST)
    	{
    		// �t�@�C���ꗗ�̏o��...
    		(factory.getCanvas()).updateInformation("�t�@�C���ꗗ�o�͒�...", -1);
    		executeOutputFileList();
    		return (true);    		
    	}
    	if (confirmationMode == SCENE_SAVETEXTFILE)
    	{
    		// �e�L�X�g�t�@�C���̕ۑ����s...
    		(factory.getCanvas()).updateInformation("�ۑ���...", -1);
    		saveTextEditFile();

    		// ��ʃV�[����ύX����...
        	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
    		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
        	
    		// �L�����o�X�N���X�ɐ؂�ւ���...
    		Display.getDisplay(parent).setCurrent(factory.getCanvas());

    		return (true);
    	}
    	return (false);
    }

    /**
     *  �e�L�X�g�ҏW���[�h�𔲂���
     *
     */
    public void exitTextEdit()
    {
    	if ((factory.getTextDataStorage()).isModified() == true)
    	{
            // �t�@�C���ҏW���ɂ́A�A�����ɔ������A�m�F���b�Z�[�W��\������
//        	String message = "    ���s�F�ۑ��I��    ���~�F�j���I��";
        	String message = "�ۑ������s���܂����H";
        	confirmationMode = SCENE_SAVETEXTFILE;
        	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);
    		return;
    	}

		// ��ʃV�[����ύX����...
    	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
    	
		// �L�����o�X�N���X�ɐ؂�ւ���...
		Display.getDisplay(parent).setCurrent(factory.getCanvas());

		return;
    }
    
    /**
     * �P�s�̃e�L�X�g�ҏW
     * 
     *
     */
    public void selectedTextEditLine()
    {
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	String lineData = new String((factory.getTextDataStorage()).getLine(line));
    	(factory.getForm()).setTargetName(lineData);
    	changeScene(kaniFileForm.SCENE_TEXTEDITLINE, (line + 1) + "");
    	return;
    }
    
    /**
     * �f�[�^�̕ҏW�m�� (�f�[�^�̓���ւ�)
     * 
     * @param lineData
     */
    public void commitLineEdit(String lineData)
    {
        // �P�s�f�[�^�̓���ւ�
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	(factory.getTextDataStorage()).replaceLine(lineData.getBytes(), line);
    	
    	// �e�L�X�g�ҏW���[�h�ɐ؂�ւ���
    	changeTextEditScreen();
    	
    	return;
    }

    /**
     * ��ʃV�[�����e�L�X�g�ҏW���[�h�ɕύX����
     * 
     *
     */
    private void changeTextEditScreen()
    {
		// ��ʃV�[�����e�L�X�g�ҏW���[�h�ɕύX����...
    	(factory.getCanvas()).changeScene(kaniFileCanvas.SCENE_TEXTEDIT, lastName);
    	
		// �L�����o�X�N���X�ɐ؂�ւ���...
		Display.getDisplay(parent).setCurrent(factory.getCanvas());    	

		return;
    }
    
    /**
     *  �ҏW�����e�L�X�g�t�@�C����ۑ�����B
     *
     */
    public void saveTextEditFile()
    {
		Thread thread = new Thread()
		{
			public void run()
			{
				// �t�@�C���f�[�^���o�͂���
		    	(factory.getTextDataStorage()).outputFile(null);

		    	// ��ʍĕ`��
		    	(factory.getCanvas()).redraw();
			}
		};
		thread.start();
		return;
    }
    
    /**
     * ��s�}������
     * 
     * @param isNext
     */
    public void insertLine(boolean isNext)
    {
        // �P�s�}��
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	if (isNext == true)
    	{
    		line++;
    	}
    	(factory.getTextDataStorage()).insertLine(line);
    }
    
    /**
     * ��s�\��t����
     *
     */
    public void pasteLine()
    {
        // �P�s�\��t��
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	(factory.getTextDataStorage()).pasteLine(line);
    }

    /**
     * 1�s�폜����
     *
     */
    public void deleteLine()
    {
        // �P�s�폜
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	(factory.getTextDataStorage()).deleteLine(line);
    	
    }
    
    /**
     * URL���u���E�U�ŊJ��...
     *
     */
    public void openUrl(boolean useGoogleProxy)
    {
        // URL�𒊏o����
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	String url = (factory.getTextDataStorage()).pickupUrl(line);
    	if (url != null)
    	{
    		if (useGoogleProxy == true)
    		{
    			// Google�̃v���L�V�o�R��URL���u���E�U�ŊJ���ݒ�̏ꍇ
                String openUrl = url;
            	if (url.indexOf("www.google.co.jp/m/") < 0)
            	{
            		// google �̃v���L�V���g�p����ݒ�łȂ��ꍇ...
                    openUrl = "http://www.google.co.jp/gwt/n?u=http%3A%2F%2F" + url.substring(7);
            	}
            	// Web�u���E�U�ŊJ��...
    		    switchToWebBrowser(openUrl);
    		}
    		else
    		{
                // Web�u���E�U�ŊJ��...
    		    switchToWebBrowser(url);
    		}
    	}
    	System.gc();
    }

    /**
     * URL���擾���A�t�@�C���ɋL�^����(�I����A�u���u������)
     *
     */
    public void getFileFromUrl()
    {
		Thread thread = new Thread()
		{
			public void run()
			{
			    // URL�𒊏o����
		    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
		    	String url = (factory.getTextDataStorage()).pickupUrl(line);
		    	if (url != null)
		    	{
		            // HTTP�ʐM�̎��s...
			        (factory.getHttpCommunicator()).doHttpMain(null, url, getCurrentDirectory());
		    	}
		    	vibrate(350);
		    	System.gc();
			}
		};
		thread.start();
    }

    /**
     *  �g���q���`�F�b�N���A�T�E���h���Đ�����
     *  (�T�E���h�����Ă����ꍇ�ɂ͉����~�߂Ă���Đ�����)
     *
     */
    public void playSound()
    {
        // �T�E���h���~�߂�(���Ă����ꍇ�ɂ�...)
        if ((factory.getUtils()).isPlayingSound() == true)
        {
            (factory.getUtils()).StopSound();
            //return;
        }

        boolean isPlay = false;
        String playType = "audio/midi";
        if ((lastName.indexOf(".mld") > 0)||(lastName.indexOf(".mid") > 0)||
             (lastName.indexOf(".MLD") > 0)||(lastName.indexOf(".MID") > 0))
        {
            playType = "audio/midi";
            isPlay = true;
        }

        if ((lastName.indexOf(".wav") > 0)||(lastName.indexOf(".WAV") > 0))
        {
            playType = "audio/x-wav";
           isPlay = true;
        }
        if (isPlay == true)
        {
        	// �T�E���h��炷!
            (factory.getUtils()).playSoundFromFile((showDirectory + lastName), playType);
        }
        return;
    }
    
    /**
     * �C���[�W��W�J����
     * 
     * @return
     */
    public Image getImage(boolean usingThumbnail)
    {        
        boolean isJpeg = false;
        if ((lastName.indexOf(".jpg") > 0)||(lastName.indexOf(".JPG") > 0))
        {
            isJpeg = true;
            if (usingThumbnail == true)
            {
                return ((factory.getUtils()).loadJpegThumbnailImageFromFile(showDirectory + lastName));
            }
        }

    	Image img = (factory.getUtils()).loadImageFromFile(showDirectory + lastName);
        if ((img == null)&&(isJpeg == true)&&
    	    ((lastName.indexOf(".jpg") > 0)||(lastName.indexOf(".JPG") > 0)))
        {
            // �g���q .jpg �̏ꍇ�ɂ́A�T���l�[��������\������...
        	System.gc();
            return ((factory.getUtils()).loadJpegThumbnailImageFromFile(showDirectory + lastName));
        }
        return (img);
    }    

	/**
	 *  �u���u��������...
	 * 
	 *
	 */
	public void vibrate(int milli)
	{
        Display.getDisplay(parent).vibrate(milli);
		return;
	}

	/**
     *  jad�t�@�C���Ŏw�肳��Ă���l���擾����
     * 
     * @param key jad�t�@�C���ɏ�����Ă���L�[���[�h
     * @return �v���p�e�B�̒l
     */	
	public String getAppProperty(String key)
	{
    	// jad�t�@�C���Ńf�B���N�g���̏ꏊ���w�肳��Ă��邩�H
    	return (parent.getAppProperty(key));
	}
    
}
