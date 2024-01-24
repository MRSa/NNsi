package jp.sourceforge.nnsi.a2b.utilities;
import java.util.Enumeration;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;

/**
 * �f�B���N�g���ꗗ���(�f�B���N�g�����t�@�C���̈ꗗ���)��񋟂��郆�[�e�B���e�B<br>
 * �t�@�C���E�f�B���N�g���̈ꗗ��񂾂��łȂ��A�t�@�C���̑����ύX���\�ł��B
 * @author MRSa
 *
 */
public class MidpFileDirList
{
    private String    currentDirectory = "file:///";
    private Vector    scanDirList      = null;
    private long     availableSize    = 0;
    private boolean dirOnlyMode     = false;

    /**
     *   �R���X�g���N�^�ł͉����������s���܂���<br>
     *   �ipremare() �ɂė��p�������s���܂��B�j
     */
    public MidpFileDirList()
    {
        // �������Ȃ�...
    }

    /**
     *  ���[�e�B���e�B�N���X�̎g�p�������s���܂�
     * 
     * @param directory �������J�n����f�B���N�g�� (null�̏ꍇ��root����)
     * @return true(root�f�B���N�g���̌�������) / false(�������s)
     */
    public boolean prepare(String directory)
    {
        return (scanDirectory(directory));
    }

    /**
     * �f�B���N�g���������[�h��ݒ肵�܂��B<br>
     * �f�B���N�g���ꗗ�Ń��X�g�������̂��A�f�B���N�g�������̈ꗗ���A
     * �f�B���N�g���ƃt�@�C���̈ꗗ�ɂ��邩���߂邱�Ƃ��ł��܂��B<br>
     * �ꗗ�\�����f�B���N�g�������ŗǂ��ꍇ�ɂ́AisDirectoryMode��
     * true�����Ă��������B
     * 
     * @param isDirectoryMode true:�f�B���N�g������/ false:�t�@�C�����܂�
     */
    public void setScanDirectoryMode(boolean isDirectoryMode)
    {
        dirOnlyMode = isDirectoryMode;
        return;
    }

    /**
     * �f�B���N�g�����ЂƂ��Ɉړ����܂��B<br>
     * �w�肷��Ƃ��ɂ́A�f�B���N�g���������� "/" �𕶎���̖�����
     * �i�[���Ă��������B("root/" �Ƃ����������Ŏw�肵�Ă��������B) <br>
     * 
     * @param path ���̃f�B���N�g����
     * @return true(���X�g�X�V����) / false(���X�g�X�V���s)
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
     * �f�B���N�g�����ЂƂ�Ɉړ����܂�<br>
     * root�f�B���N�g���ɂ����ꍇ�ɂ͈ړ����s(false)���������܂��B
     * 
     * @return true(�ړ�����) / false(�ړ����s)
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
     * �t�@�C�����N���X���擾���܂��B
     * @param index �C���f�b�N�X�ԍ�(0�X�^�[�g)
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
     * �f�B���N�g�����ɂ���t�@�C�������������܂�
     * @return �f�B���N�g�����ɂ���t�@�C����
     */
    public int getCount()
    {
        return (scanDirList.size());
    }

    /**
     * ���݂̃f�B���N�g�������������܂�
     * @return ���݂̃f�B���N�g����(full path)
     */
    public String getDirectoryName()
    {
        return (currentDirectory);
    }

    /**
     * �f�B���N�g���̃X�L�������s���܂�
     * @param path �X�L��������f�B���N�g�� (null�̏ꍇ�ɂ�root������s���܂�)
     * @return true(�X�L��������)/false(���s)
     */
    public boolean scanDirectory(String path)
    {
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
        System.gc();
        return (true);
    }

    /**
     * �t�@�C�����N���X��ǉ�����
     * @param path
     * @param name
     * @param data
     */
    private void addItem(String path, String name, String data)
    {
        fileInformation info = new fileInformation(name, this);
//        info.updateDetailInfo();
         scanDirList.addElement(info);
        return;
    }

    /**
     * �t�@�C�������̕ύX���s���܂�<br>
     * �t�@�C�����N���X�Ɋi�[����Ă���A���̏����X�V���܂�
     * <ul>
     *   <li>�ŏI�X�V����</li>
     *   <li>�B���t�@�C������</li>
     *   <li>�������ݑ���</li>
     *   <li>�t�@�C���T�C�Y</li>
     *  </ul>
     * @param info �t�@�C�����i�[�N���X
     * @return �f�[�^�擾����(true) / �擾���s(false)
     */
    public boolean updateDetailInfo(fileInformation info)
    {
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
        return (true);
    }

    /**
     * �t�@�C�������ׂđI�� / �I���������܂�
     * @param isSelected �I��(true) / �I������(false)
     * @return �I�������t�@�C����
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
     * �f�B�X�N�̋󂫃T�C�Y���������܂�
     * @return �f�B�X�N�̋󂫗̈�T�C�Y
     */
    public long getAvailableSize()
    {
        return (availableSize);
    }
    
    /**
     * �t�@�C�������i�[����N���X�ł�
     * 
     * @author MRSa
     */
    public class fileInformation
    {
        private MidpFileDirList parent = null;
        private String   fileName      = null;
        private long    dateTime      = -1;
        private long    fileSize      = -1;
        private boolean hiddenFile    = false;
        private boolean writableFile  = true;
        private boolean selected      = false;
        private boolean isDetail      = false;

        /**
         * �R���X�g���N�^
         * @param name   �t�@�C����
         * @param object �e�N���X
         */
        private fileInformation(String name, MidpFileDirList object)
        {
            fileName    = name;
            parent      = object;
        }

        /**
         * �t�@�C����(�f�B���N�g����)���擾���܂�<br>
         * (�p�X�͊܂܂�Ă��܂���)
         * @return �t�@�C����
         */
        public String getFileName()
        {
            return (fileName);
        }

        /**
         * �A�C�e����I��/��I���̃}�[�N�����܂�<br>
         * (������Ԃ͔�I���ł��B)
         * 
         * @param sel �I��(true) / ��I��(false)
         */
        public void setSelected(boolean sel)
        {
            selected = sel;
            return;
        }

        /**
         * �I�����[�h��؂�ւ��܂�<br>
         * �I�𒆂ł���Δ�I���ɁA��I���ł���ΑI�𒆂ɐ؂�ւ��܂�
         *
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
         *   �t�@�C�������̍X�V�����s���܂��B
         * <ul>
         *   <li>�ŏI�X�V����</li>
         *   <li>�B���t�@�C������</li>
         *   <li>�������ݑ���</li>
         *   <li>�t�@�C���T�C�Y</li>
         *  </ul>
         * 
         * @return �f�[�^�̍X�V����(true) / �X�V���s(false)
         */
        public boolean updateDetailInfo()
        {
            isDetail = parent.updateDetailInfo(this);
            return (isDetail);
        }

        /**
         * �t�@�C���T�C�Y��ݒ肵�܂�<br><br>
         * �{���\�b�h�ł́A�t�@�C���V�X�e���ւ̔��f�͍s���܂���B<br>
         * �i�t�@�C���V�X�e���ւ̔��f�́AupdateDetailInfo() �ɂčs���܂��B�j
         * @param size �t�@�C���T�C�Y
         */
        public void setFileSize(long size)
        {
            fileSize = size;
            return;
        }

        /**
         * �t�@�C���T�C�Y���擾���܂�
         * @return �t�@�C���T�C�Y
         */
        public long getFileSize()
        {
            if (isDetail == false)
            {
                return (-1);
            }                
            return (fileSize);
        }

        /**
         * �����i�t�@�C���쐬�����j��ݒ肵�܂�<br><br>
         * �{���\�b�h�ł́A�t�@�C���V�X�e���ւ̔��f�͍s���܂���B<br>
         * �i�t�@�C���V�X�e���ւ̔��f�́AupdateDetailInfo() �ɂčs���܂��B�j
         * 
         * @param time ����
         */
        public void setDateTime(long time)
        {
            dateTime = time;
            return;
        }
 
        /**
         * �t�@�C���쐬�������擾���܂�
         * 
         * @return �t�@�C���쐬����
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
         *  �B���t�@�C�����ǂ����̐ݒ���s���܂�<br><br>
         * �{���\�b�h�ł́A�t�@�C���V�X�e���ւ̔��f�͍s���܂���B<br>
         * �i�t�@�C���V�X�e���ւ̔��f�́AupdateDetailInfo() �ɂčs���܂��B�j
         * 
         *  @param hidden �B���t�@�C��(true) / �ʏ�t�@�C�� (false)
         */
        public void setHidden(boolean hidden)
        {
            hiddenFile = hidden;
            return;
        }
      
        /**
         * �B���t�@�C�����ǂ����̐ݒ���擾���܂�
         * 
         *  @return �B���t�@�C��(true) / �ʏ�t�@�C�� (false)
         */
        public boolean isHidden()
        {
            return (hiddenFile);
        }

        /**
         * �������݉ۂ�ݒ肵�܂�<br><br>
         * 
         * �{���\�b�h�ł́A�t�@�C���V�X�e���ւ̔��f�͍s���܂���B<br>
         * �i�t�@�C���V�X�e���ւ̔��f�́AupdateDetailInfo() �ɂčs���܂��B�j 
         * @param writable �������݋���(true) / �������݋֎~(false)
         */
        public void setWritable(boolean writable)
        {
            writableFile = writable;
            return;
        }

        /**
         * �������݉ۏ�Ԃ��擾���܂�
         * @return �������݉� �������݋���(true) / �������݋֎~(false)
         */
        public boolean isWritable()
        {
            return (writableFile);
        }
        
        
        /**
         * �f�B���N�g�����ǂ������擾���܂�
         * 
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
         * �t�@�C�����I�𒆂��ǂ������擾���܂�
         * 
         * @return �t�@�C���I��(true)/��I�����(false)
         */
        public boolean isSelected()
        {
            return (selected);
        }
    }
}
