package jp.sourceforge.nnsi.a2b.screens.selections;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;

/**
 *  �t�@�C��/�f�B���N�g���ꗗ��ʗp���쐧��N���X<br>
 * 
 * @author MRSa
 *
 */
public class DirectorySelectionDialogOperator extends SelectionScreenOperator
{
	private MidpFileDirList fileDirList = null;
    private IFileSelectionStorage fileStorage = null;
    private DirectorySelectionDialog  screenClass = null;

    private String scanPath = null;     // �������p�X�i�ꎞ�I�ɗ��p����j
    private String selectedDir = null;  // �I�����ꂽ�f�B���N�g��
    private boolean isDirectoryMode = true;  // 

    /**
     * �N���X�̏��������s���܂�<br>
     * ���p�O�ɕK��prepare()���Ăяo���Ă��������B
     * @param storageDb ��ʑ���p�L�^�N���X
     * @param fileStorageDb �t�@�C���E�f�B���N�g���̓���L�^�N���X
     * @param object �t�@�C��/�f�B���N�g������N���X
     */
    public DirectorySelectionDialogOperator(ISelectionScreenStorage storageDb, IFileSelectionStorage fileStorageDb, MidpFileDirList object, boolean directoryMode)
    {
        super(storageDb);
        fileDirList = object;
        fileStorage = fileStorageDb;
        isDirectoryMode = directoryMode;
    }
    
    /**
     *  �N���X�̎g�p�������s���܂�
     *  @param screen �\�����
     */
    public void prepare(DirectorySelectionDialog screen)
    {
    	screenClass = screen;
    	fileDirList.setScanDirectoryMode(isDirectoryMode);
    	scanDirectory(fileStorage.getDefaultDirectory());
        return;
    }

    /**
     *  �f�B���N�g�����X�L��������
     *  @param path ��������f�B���N�g��
     *
     */
    private void scanDirectory(String path)
    {
		scanPath = path;
        Thread thread = new Thread()
        {
            public void run()
            {
                if (scanPath == null)
                {
                	// �f�B���N�g���̌���
                	fileDirList.scanDirectory(null);
                }
                else if (scanPath.equals(".."))
                {
            	    // �ЂƂ�̃f�B���N�g������
            	    fileDirList.upDirectory();
                }
                else  // if (scanPath != null)
                {
            	    // �ЂƂ�̃f�B���N�g������
            	    fileDirList.downDirectory(scanPath);
                }
                

                // �^�C�g�����X�V����
            	screenClass.setTitle(fileDirList.getDirectoryName());
            	
                // ��ʂ��ĕ`�悷��
                screenClass.refreshScreen();
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
        	//
        }
	    return;
    }

    /**
     *  �^�C�g����\�����邩�ǂ����H
     *  (����́A�K���\������)
     *  @return true
     */
	public boolean isShowTitle()
	{
		return (true);
    }

    /**
     * �\������A�C�e�������擾����
     * 
     * @return �\������A�C�e����
     */
    public int getSelectionItemCount()
    {
        return (fileDirList.getCount());
    }

    /**
     *  �\�����郁�b�Z�[�W�f�[�^���擾����
     *  
     *  @param itemNumber �A�C�e���ԍ�(0�X�^�[�g)
     *  @return �t�@�C����
     */
    public String getSelectionItem(int itemNumber)
    {
    	MidpFileDirList.fileInformation info = fileDirList.getFileInfo(itemNumber);
        return (info.getFileName());
    }

    /**
     *  �A�C�e����I�������Ƃ�
     *  
     *  @param  itemId �I�������A�C�e����ID
     */
    public void selectedItem(int itemId)
    {
        selectedDir = null;
    	if ((itemId >= 0)&&(itemId < fileDirList.getCount()))
    	{
    		selectedDir = getSelectionItem(itemId);
            String directory = fileDirList.getDirectoryName();
            if (isDirectoryMode == false)
            {
                fileStorage.setSelectedName(directory + selectedDir);
            }
        }
    	return;
    }

    /**
     *  �R�}���h�{�^���̓���
     *  
     *  @param  buttonId �����ꂽ�{�^��
     *  @return �o�^OK
     */
    public boolean selectedCommandButton(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            // �L�����Z�����ꂽ�A�������Ȃ�
            return (true);
        }
        fileStorage.confirmSelectedName();
        scanDirectory(selectedDir);
        System.gc();
        return (false);
    }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��̏���
     *  (�f�B���N�g���̊m��)
     * 
     * @return true (��ʑJ�ڂ����s)
     */
    public boolean triggeredMenu()
    {
        // �f�B���N�g�������L�^����
        String directory = fileDirList.getDirectoryName();
        fileStorage.setSelectedName(directory);
        fileStorage.confirmSelectedName();
        return (true);
    }
}
