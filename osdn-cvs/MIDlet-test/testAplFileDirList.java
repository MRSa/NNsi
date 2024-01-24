import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;

/**
 * �f�B���N�g�����(�f�B���N�g�����ꗗ���)�񋟃N���X
 * @author MRSa
 *
 */
public class testAplFileDirList
{
	private String    currentDirectory = "file:///";
	private Vector    scanDirList      = null;
	private long     availableSize    = 0;
	private boolean dirOnlyMode     = false;

	/**
	 * �R���X�g���N�^
	 *
	 */
	testAplFileDirList()
	{
        // �������Ȃ�...
	}

	/**
	 * �f�B���N�g�����̏���...
	 * @return true(root�f�B���N�g���̌�������)/false(�������s)
	 */
	public boolean prepare(String directory)
	{
		return (scanDirectory(directory));
	}

	/**
	 * �f�B���N�g���������[�h�̐ݒ�
	 * 
	 * @param isDirectoryMode true:�f�B���N�g������/ false:�t�@�C�����܂�
	 */
	public void setScanDirectoryMode(boolean isDirectoryMode)
	{
		dirOnlyMode = isDirectoryMode;
		return;
	}

    /**
     * �f�B���N�g�����ЂƂ��Ɉړ�����
     * @param path (���̃f�B���N�g����)
     * @return true(����)/false(���s)
     */
    public boolean downDirectory(String path)
    {
		if ((path.endsWith("/")) == false)
		{
			// �w�肳�ꂽ path�̓f�B���N�g���ł͂Ȃ��A�I������
			return (false);
		}
		if ((path.equals("..")) == true)
		{
			// �ЂƂ�̃f�B���N�g���Ɉړ����邱�Ƃ��w�肳�ꂽ�A�A
			return (upDirectory());
		}
    	return (scanDirectory(currentDirectory + path));
    }
    
    /**
     * �f�B���N�g�����ЂƂ�Ɉړ�����
     * @return true(����)/false(���s)
     */
    public boolean upDirectory()
    {
    	// ����root�f�B���N�g���ɂ����A�A�I������
        String nextPath = null;
        if (currentDirectory == null)
        {
        	return (false);
        }
		int pos = currentDirectory.lastIndexOf('/', currentDirectory.length() - 2);
		if (pos >= 0)
		{
            nextPath = currentDirectory.substring(0, pos + 1);
		}
		else
		{
			return (false);
		}
        return (scanDirectory(nextPath));
    }

	/**
     * �t�@�C�������擾����...
     * @param index
     * @return �t�@�C�����N���X
     */
    public fileInformation getFileInfo(int index)
    {
    	if ((index < 0)||(index >= scanDirList.size()))
    	{
    		// �C���f�b�N�X�͈̔͊O...
    		return (null);
    	}
    	return ((fileInformation) scanDirList.elementAt(index));
    }

    /**
     * �f�B���N�g�����ɂ���t�@�C��������������
     * @return
     */
    public int getCount()
    {
    	return (scanDirList.size());
    }

	/**
	 * �f�B���N�g��������������
	 * @return
	 */
	public String getDirectoryName()
	{
		return (currentDirectory);
	}
	
	/**
	 * �f�B���N�g���̃X�L����
	 * @param path �X�L��������f�B���N�g�� (null�̏ꍇ�ɂ�root����)
	 * @return true(�X�L��������)/false(���s)
	 */
    public boolean scanDirectory(String path)
	{
/*****
    	boolean        isRoot  = false;
		Enumeration     dirEnum = null;
		FileConnection  fc      = null;
		try
		{
			if ((path == null)||(path.equals("file:///")))
			{
				// �f�B���N�g���̃��[�g����ꗗ���擾����
				dirEnum = FileSystemRegistry.listRoots();
				isRoot  = true;
				path    = "file:///";
			}
			else
			{
				// �w�肳�ꂽ�p�X����ꗗ���擾����
				fc = (FileConnection) Connector.open(path, Connector.READ);
				dirEnum = fc.list("*", true);
				availableSize = fc.availableSize();  // ���łɃt�@�C���T�C�Y���擾����...
				fc.close();
			}
		}
		catch (Exception e)
		{
            try
            {
                if (fc != null)
				{
					fc.close();
				}
            }
			catch (Exception e2)
			{
				//
            }
			return (false);
		}

		// �f�B���N�g�����ɂ���t�@�C���ꗗ��ǉ�����
		scanDirList = null;
		scanDirList = new Vector();
		if (isRoot != true)
		{
			addItem(null, "..", null);
		}
		currentDirectory = path;
		while (dirEnum.hasMoreElements())
		{
			String name = (String)dirEnum.nextElement();
    		if (dirOnlyMode == true)
    		{
    			if (name.endsWith("/"))
    			{
    				addItem(path, name, null);    				
    			}
    		}
    		else
    		{
    			addItem(path, name, null);
    		}
		}
*****/
		System.gc();
		return (true);
	}

    /**
     * �t�@�C�����N���X��ǉ�����
     * @param name
     * @param data
     */
    private void addItem(String path, String name, String data)
    {
    	fileInformation info = new fileInformation(name, this);
//    	info.updateDetailInfo();
     	scanDirList.addElement(info);
		return;
    }

    /**
     * �t�@�C�����N���X��ǉ�����
     * @param name
     * @param data
     */
    public boolean updateDetailInfo(fileInformation info)
    {
/*****/
    	FileConnection  fc       = null;
    	try
    	{
    		String path = currentDirectory + info.getFileName();
			fc = (FileConnection) Connector.open(path, Connector.READ);
			info.setDateTime(fc.lastModified());
			info.setHidden(fc.isHidden());
			info.setWritable(fc.canWrite());
    		if (fc.isDirectory() != true)
			{
    			info.setFileSize(fc.fileSize());
			}
			fc.close();
		}
		catch (Exception e)
		{
            try
            {
                if (fc != null)
				{
					fc.close();
				}
            }
			catch (Exception e2)
			{
				//
            }
			return (false);
     	}
/*****/
		return (true);
    }

    /**
     * �t�@�C�������ׂđI������
     * @return
     */
    public int selectAllFiles(boolean isSelected)
    {
    	int count = 0;
		Enumeration cnt = scanDirList.elements();
		while (cnt.hasMoreElements())
		{
			fileInformation data = (fileInformation) cnt.nextElement();
			if (data.isDirectory() != true)
			{
				data.setSelected(isSelected);
				count++;
			}
		}
    	return (count);
    }
    
    /**
     * �f�B�X�N�̋󂫃T�C�Y����������
     * @return
     */
    public long getAvailableSize()
    {
    	return (availableSize);
    }
    
    /**
     * �t�@�C�����N���X
     * @author MRSa
     */
    public class fileInformation
    {
    	private testAplFileDirList parent = null;
    	private String   fileName      = null;
    	private long    dateTime      = -1;
    	private long    fileSize      = -1;
    	private boolean hiddenFile    = false;
    	private boolean writableFile  = true;
    	private boolean selected      = false;
    	private boolean isDetail      = false;

    	/**
    	 * �R���X�g���N�^...
    	 * @param name
    	 */
    	fileInformation(String name, testAplFileDirList object)
    	{
    		fileName    = name;
    		parent      = object;
    	}

    	/**
    	 * �t�@�C����(�f�B���N�g����)����������
    	 * @return
    	 */
    	public String getFileName()
    	{
    		return (fileName);
    	}

    	/**
    	 * �I�����[�h��؂�ւ���
    	 * @param sel
    	 */
    	public void setSelected(boolean sel)
    	{
    		selected = sel;
    		return;
    	}

    	/**
    	 * �I�����[�h��؂�ւ���
    	 * @param sel
    	 */
    	public void toggleSelected()
    	{
    		if (selected == true)
    		{
    			selected = false;
    		}
    		else
    		{
    			selected = true;
    		}
    		return;
    	}

    	/**
    	 * �\���f�[�^�̍X�V���s��
    	 * @return
    	 */
    	public boolean updateDetailInfo()
    	{
    		isDetail = parent.updateDetailInfo(this);
    		return (isDetail);
    	}

    	/**
    	 * �t�@�C���T�C�Y��ݒ肷��
    	 * @param size
    	 */
    	public void setFileSize(long size)
    	{
    		fileSize = size;
    		return;
    	}

    	/**
    	 * �t�@�C���T�C�Y���擾����
    	 * @return
    	 */
    	public long getFileSize()
    	{
    		if (isDetail == false)
    		{
    			return (-1);
    		}    			
    		return (fileSize);
    	}

    	public void setDateTime(long time)
    	{
    		dateTime = time;
    		return;
    	}
 
    	/**
    	 * �t�@�C���쐬��������������
    	 * 
    	 * @return
    	 */
    	public long getDateTime()
    	{
    		if (isDetail == false)
    		{
    			return (-1);
    		}    			
    		return (dateTime);
    	}
    	
    	/**
    	 *  �B���t�@�C�����ǂ����̐ݒ���s��
    	 */
    	public void setHidden(boolean hidden)
    	{
    		hiddenFile = hidden;
    		return;
    	}
  	
    	/**
    	 * �B���t�@�C�����ǂ�����������
    	 * 
    	 * @return
    	 */
    	public boolean isHidden()
    	{
    		return (hiddenFile);
    	}

    	/**
    	 * �������݉ۂ�ݒ肷��
    	 * @param writable
    	 */
    	public void setWritable(boolean writable)
    	{
    		writableFile = writable;
    		return;
    	}

    	/**
    	 * �������݉ۂ���������
    	 * @return
    	 */
    	public boolean isWritable()
    	{
    		return (writableFile);
    	}
    	
    	
    	/**
    	 * �f�B���N�g�����ǂ�������������
    	 * @return  true: �f�B���N�g���Afalse:�ʏ�t�@�C��
    	 */
    	public boolean isDirectory()
    	{
    		if (fileName.endsWith("/"))
    		{
    			return (true);
    		}
    		if (fileName.equals(".."))
    		{
    			return (true);
    		}
    		return (false);
    	}

    	/**
    	 * �t�@�C�����I�𒆂��ǂ���
    	 * @return
    	 */
    	public boolean isSelected()
    	{
    		return (selected);
    	}
    }
}
