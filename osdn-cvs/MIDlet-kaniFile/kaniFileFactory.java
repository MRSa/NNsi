/**
 * �I�u�W�F�N�g�̊Ǘ��N���X
 * 
 * @author MRSa
 *
 */
public class kaniFileFactory
{
	// �I�u�W�F�N�g�Q...
	private kaniFileZipOperations   zipOperation  = null;
	private kaniFileDirList         directoryInfo = null;
	private kaniFileOperations      fileOperation = null;
	private kaniFileTextFile        textFileInfo  = null;
	private kaniFileUtils           utils         = null;
	private kaniFileDatas           dataStorage   = null;
	private kaniFileHttpCommunication httpComm    = null;
	private kaniFileForm            inputForm     = null;
	private kaniFileCanvas          canvasForm    = null;
	
	/**
	 * �R���X�g���N�^
	 * @param object �e�N���X
	 */
	public kaniFileFactory()
	{
		// �������Ȃ�...
	}

	/**
	 * �I�u�W�F�N�g�̐����Ə���
	 * 
	 * @return  �I�u�W�F�N�g��������/�������s
	 */
	public boolean prepare(kaniFileSceneSelector parent)
	{
        // �I�u�W�F�N�g�𐶐�����
		zipOperation   = new kaniFileZipOperations(parent);
		directoryInfo  = new kaniFileDirList();
		dataStorage    = new kaniFileDatas();
		textFileInfo   = new kaniFileTextFile();
		utils          = new kaniFileUtils();
		fileOperation  = new kaniFileOperations(parent);
		httpComm       = new kaniFileHttpCommunication();
		inputForm      = new kaniFileForm("", parent);
		canvasForm     = new kaniFileCanvas(parent);

		// �I�u�W�F�N�g�̏��������s����
		dataStorage.prepare();
		directoryInfo.prepare(dataStorage.getDirectory());
		httpComm.prepare();
		inputForm.prepare();
		canvasForm.prepare();
		return (true);
	}

	/**
	 * �L�����o�X�N���X���擾����
	 * @return
	 */
	public kaniFileCanvas getCanvas()
	{
		return (canvasForm);
	}
	
	/**
	 * �t�H�[���N���X���擾����
	 * @return
	 */
	public kaniFileForm getForm()
	{
		return (inputForm);
	}

	/**
	 *  Zip����N���X���擾����
	 * @return
	 */
	public kaniFileZipOperations getZipOperation()
	{
        return (zipOperation);
	}
	
	/**
	 * �t�@�C������N���X���擾����
	 * @return
	 */
	public kaniFileOperations getFileOperation()
	{
		return (fileOperation);
	}

	/**
	 * �f�[�^�i�[�N���X���擾����
	 * @return �f�[�^�i�[�N���X
	 */
	public kaniFileDatas getDataStorage()
	{
		return (dataStorage);
	}

	/**
	 * �e�L�X�g�f�[�^�i�[�N���X���擾����
	 * @return �e�L�X�g�f�[�^�i�[�N���X
	 */
	public kaniFileTextFile getTextDataStorage()
	{
		return (textFileInfo);
	}

	/**
	 * ���[�e�B���e�B�N���X���擾����
	 * @return ���[�e�B���e�B�N���X
	 */
	public kaniFileUtils getUtils()
	{
		return (utils);
	}

	/**
	 * HTTP�ʐM�N���X���擾����
	 * @return HTTP�ʐM�N���X
	 */
	public kaniFileHttpCommunication getHttpCommunicator()
	{
		return (httpComm);
	}
	
	/**
	 * �f�B���N�g�����N���X���擾����
	 * @return �f�B���N�g�����N���X
	 */
	public kaniFileDirList getDirectoryInfo()
	{
		return (directoryInfo);
	}

	/**
	 * ��ʕ\���̏���
	 *
	 */
	public void prepareScreen()
	{
		canvasForm.prepareScreen();
		inputForm.prepareScreen();
		return;
	}

	/**
	 * �Ǘ��I�u�W�F�N�g�̍폜
	 *
	 */
	public void cleanup()
	{
		// �f�[�^���o�b�N�A�b�v����
		dataStorage.store();

        // �I�u�W�F�N�g���N���A����
		utils         = null;
		textFileInfo  = null;
		fileOperation = null;
		inputForm     = null;
		canvasForm    = null;
		dataStorage   = null;
		httpComm      = null;
		return;
	}
}
