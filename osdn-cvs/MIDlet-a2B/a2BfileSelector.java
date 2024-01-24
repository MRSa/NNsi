import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.List;
import java.util.Vector;

/**
 * �t�@�C���I���N���X (�ȈՃt�@�C���N���X)
 * 
 * @author MRSa
 *
 */
public class a2BfileSelector extends List implements CommandListener
{
    private a2BMain   parent = null;
    private String    path   = null;
    private String    nextPath = "file:///";
    private String    targetFilePath   = null;
    private String    targetFileName   = null;
    private boolean  isRefreshScreen  = false;
    private boolean  isUpdateTitle    = true;

    private final int COMMAND_EXIT     = -1;
    private final int COMMAND_SELECT   = 0;
    private final int COMMAND_FILEINFO = 1;
    private final int COMMAND_DELETE   = 2;
    private final int COMMAND_RENAME   = 3;
    private final int COMMAND_COPYFILE = 4;
    private final int COMMAND_RESCAN   = 5;
    private final int COMMAND_GETHTTP  = 6;

    private final int COPY_BUFFER_SIZE = 16384;
    private final int FILE_BUFFER_SIZE = 5120;
    
    private Command   exitCmd    = new Command("�ꗗ��",           Command.EXIT,    COMMAND_EXIT);
    private Command   selectCmd  = new Command("�I��",             Command.SCREEN,  COMMAND_SELECT);
    private Command   deleteCmd  = new Command("�폜",             Command.ITEM,    COMMAND_DELETE);
    private Command   renameCmd  = new Command("����",             Command.ITEM,    COMMAND_RENAME);
    private Command   infoCmd    = new Command("�v���r���[(dat)",  Command.ITEM,    COMMAND_FILEINFO);
    private Command   copyCmd    = new Command("�R�s�[",           Command.ITEM,    COMMAND_COPYFILE);
    private Command   rescanCmd  = new Command("�\���X�V",         Command.ITEM,    COMMAND_RESCAN);
    private Command   getHttpCmd = new Command("http��M",         Command.ITEM,     COMMAND_GETHTTP);
    private Vector    itemVector = null;

    /*
     *   �R���X�g���N�^
     * 
     */
    public a2BfileSelector(a2BMain object)
    {
        super("", List.IMPLICIT);
        parent = object;

        openDirectory(true);
    }

    /*
     *   ���̃f�B���N�g���p�X��ݒ肷��
     *
     */
    public void setnextPath(String path)
    {
        nextPath = path;
        return;
    }
    
    /*
     *   �f�B���N�g�����J��
     * 
     */    
    public void openDirectory(boolean isRoot)
    {
        Enumeration dirEnum = null;
        if (isRoot == true)
        {
            dirEnum = FileSystemRegistry.listRoots();
        }
        else
        {
            FileConnection fc = null;
            try
            {
                fc = (FileConnection) Connector.open(nextPath, Connector.READ);
                dirEnum = fc.list();
                fc.exists();
            }
            catch (Exception e)
            {
                try
                {
                    if (fc!=null)
                    {
                        fc.exists();
                    }
                }
                catch (Exception e2)
                {
                    
                }
                return;
            }
        }

        // �^�C�g���̕\��
        if (isUpdateTitle == true)
        {
            setTitle("wait...�ꗗ�X�V��");
        }

        // ��ʏ����N���A����
        deleteAll();
        itemVector = null;
        itemVector = new Vector();
        this.addCommand(exitCmd);
//        this.addCommand(selectCmd);
        this.addCommand(infoCmd);
        this.addCommand(renameCmd);
        this.addCommand(deleteCmd);
        this.addCommand(copyCmd);
        this.addCommand(getHttpCmd);
        this.addCommand(rescanCmd);
        this.setSelectCommand(selectCmd);
        this.setCommandListener(this);

        // �p�X�����X�V����
        path = nextPath;

/**
        // �ЂƂ�̊K�w�ɂ�����̂�ǉ�����i�擪�j
        if (!nextPath.equals("file:///"))
        {
            addItem("..", null);
        }
**/

        // �f�B���N�g�����ɂ���t�@�C���ꗗ��(���ׂ�)�\������
        while (dirEnum.hasMoreElements())
        {
            String name = (String)dirEnum.nextElement();
            if (name.toLowerCase().endsWith(".dat"))
            {
                // �t�@�C����dat�t�@�C���̎��ɂ̓^�C�g������͂���
                parent.prepareScratchBuffer((nextPath + name), FILE_BUFFER_SIZE);
                String title = parent.pickupThreadTitle();
                addItem(name, title);                
            }
            else
            {
                // �t�@�C���������̂܂ܕ\������
                addItem(name, null);
            }
        }

        // �ЂƂ�̊K�w�ɂ�����̂�ǉ�����i�����j
        if (!nextPath.equals("file:///"))
        {
            addItem("..", null);
        }

        // �^�C�g����\������...
        if (isUpdateTitle == true)
        {
            setTitle(nextPath);
        }
        return;
    }
    
    /**
     * �\������A�C�e����ǉ�����
     * 
     * @param string   �t�@�C����
     * @param title    �^�C�g��(�t�@�C�����X���̏ꍇ)
     */
    private void addItem(String string, String title) 
    {
        if (title != null)
        {
            append(title + " (" + string + ")", null);            
        }
        else
        {
            append(string, null);
        }
        itemVector.addElement(string);
        return;
    }

    /**
     * �t�@�C�����𒊏o����
     * 
     * @param data
     * @return
     */
    private String pickupFileName()
    {
        int index = getSelectedIndex();
        if (index < 0)
        {
            return ("");
        }
        String data = (String) itemVector.elementAt(index);
        return (data);
    }

    /**
     *  �A�C�e�����폜�����Ƃ��̏���
     *
     */
    private void deleteItem()
    {
        // �t�@�C�����ƃp�X��ݒ肷��
        if (setFileNameAndPath() == false)
        {
            return;
        }
        
        // �^�C�g���̕\��...
        String title = "�폜�F " + targetFilePath + targetFileName;

        Vector commandList = new Vector(2);
        String doDelete  = "�폜���s";
        String cancelCmd = "�L�����Z��";
        commandList.addElement(cancelCmd);
        commandList.addElement(doDelete);

        // �t�@�C���폜�����s���邩�m�F����
        parent.openSelectForm(title, commandList, 0, parent.CONFIRM_DELETELOGFILE);
        commandList = null;

        return;
    }

    /**
     * �t�@�C�����ƃp�X��ݒ肷��
     * 
     * @return
     */
    private boolean setFileNameAndPath()
    {
        String name = pickupFileName();
        if (name.equals(".."))
        {
            // �������Ȃ�...
            return (false);
        }

        if (name.endsWith("/"))
        {
            // �f�B���N�g���͉����ł��Ȃ�
            return (false);
        }

        targetFilePath = path;
        targetFileName = name;
        return (true);        
    }
    
    /**
     *   �A�C�e�����l�[���w�����̏���
     * 
     */
    private void renameItem()
    {
        // �t�@�C�����ƃt�@�C���p�X��ݒ肷��
        if (setFileNameAndPath() == false)
        {
            return;
        }

        // �t�@�C�����̓���
        parent.OpenWriteFormUsingWordInputMode(parent.SCENE_RENAMEFILE_INPUT, "����", "�V�t�@�C����", targetFileName, false);
        return;
    }

    /**
     *   http�Ńt�@�C������M���鏈��
     * 
     */
    private void getHttpFile()
    {
        // �t�@�C���́A������f�B���N�g���Ɏ�M����...
        targetFilePath = path;
        targetFileName = "";

        Thread thread = new Thread()
        {
            public void run()
            {
                // URL�̓���
                parent.OpenWriteFormUsingWordInputMode(parent.SCENE_GETHTTP_INPUT, "http��M", "�擾��URL", "http://", false);
            }
        };
        thread.start();

        return;
    }

    /**
     *   �A�C�e���R�s�[�w�����̏���
     * 
     */
    private void copyItem()
    {
        // �t�@�C�����ƃt�@�C���p�X��ݒ肷��
        if (setFileNameAndPath() == false)
        {
            return;
        }

        // �t�@�C�����̓���
        parent.OpenWriteFormUsingWordInputMode(parent.SCENE_COPYFILE_INPUT, "�R�s�[", "�R�s�[��t�@�C����", targetFileName, false);
        return;
    }

    /**
     * �R�}���h�̎��s���C��
     * 
     * @param scene  �������
     * @param index  �I���A�C�e��
     */
    public void executeCommandAction(byte scene, int index)
    {
        if (scene == parent.CONFIRM_DELETELOGFILE)
        {
            if (index == 1)
            {
                // �t�@�C���̍폜�����{����
                deleteFile(targetFilePath + targetFileName, true);
            }
            return;
        }
        return;
    }
    
    /**
     *    �A�C�e�� �I�����̏���
     *   
     */
    private void selection()
    {
        String name = pickupFileName();
        if (name.equals(".."))
        {
            int pos = path.lastIndexOf('/', path.length() - 2);
            nextPath = path.substring(0, pos + 1);
        }
        else
        {
            nextPath = path + name;
        }
    
        if (nextPath.endsWith("/"))
        {
            refreshScreen();
        }

        if ((name.toLowerCase().endsWith(".txt"))||
            (name.toLowerCase().endsWith(".bak"))||
            (name.toLowerCase().endsWith(".dat"))||
            (name.toLowerCase().endsWith(".csv"))||
            (name.toLowerCase().endsWith(".idx"))||
            (name.toLowerCase().endsWith(".htm"))||
            (name.toLowerCase().endsWith(".html"))||
            (name.toLowerCase().endsWith(".cnf")))
        {
            // �e�ɑ΂��A�I�������e�L�X�g�t�@�C������ʒm����
            parent.EndFileSelector(nextPath);
        }
        return;
    }

    /**
     *  �폜�����̎�����
     * 
     * 
     */
    private void doDeleteMain(String targetFileName)
    {
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ_WRITE);
            if (dataFileConnection.exists() == true)
            {
                // �폜���s
                dataFileConnection.delete();
            }
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
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
        }
        return;
    }    

    /**
     * �t�@�C�����̃��l�[���������C��
     * 
     * @param destFileName
     * @param srcFileName
     */
    private void doRenameMain(String destFileName, String srcFileName)
    {
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(srcFileName, Connector.READ_WRITE);
            if (dataFileConnection.exists() == true)
            {
                // �t�@�C�����̕ύX���s�B�B�B
                dataFileConnection.rename(destFileName);
            }
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
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
        }
        return;
    }    
    
    /**
     * �t�@�C�����̃R�s�[�������C��
     * 
     * @param destFileName
     * @param srcFileName
     */
    public void doCopyFileMain(String destFileName, String srcFileName)
    {
        FileConnection srcFileConnection = null;
        FileConnection dstFileConnection = null;
        
        InputStream   is = null;
        OutputStream  os = null;

        if (destFileName == srcFileName)
        {
            // �t�@�C�����������������ꍇ�ɂ̓R�s�[�����s���Ȃ�
            return;
        }
        
        try
        {
            srcFileConnection = (FileConnection) Connector.open(srcFileName, Connector.READ_WRITE);
            if (srcFileConnection.exists() != true)
            {
                // �t�@�C�������݂��Ȃ������A�A�A�I������
                srcFileConnection.close();
                return;
            }
            is = srcFileConnection.openInputStream();

            long dataFileSize = srcFileConnection.fileSize();
            byte[] buffer = new byte[COPY_BUFFER_SIZE + 4];

            dstFileConnection = (FileConnection) Connector.open(destFileName, Connector.READ_WRITE);
            if (dstFileConnection.exists() == true)
            {
                // �t�@�C�������݂����A�A�A�폜���č�蒼��
                dstFileConnection.delete();
            }
            dstFileConnection.create();

            os = dstFileConnection.openOutputStream();
            if ((is != null)&&(os != null))
            {
                while (dataFileSize > 0)
                {
                    int size = is.read(buffer, 0, COPY_BUFFER_SIZE);
                    if (size <= 0)
                    {
                        break;
                    }
                    os.write(buffer, 0, size);
                }
            }
            os.close();
            is.close();
            dstFileConnection.close();
            srcFileConnection.close();

            is = null;
            os = null;
            srcFileConnection = null;
            dstFileConnection = null;
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
            try
            {
                if (is != null)
                {
                    is.close();
                }
                is = null;
                
                if (os != null)
                {
                    os.close();
                }
                os = null;
                
                if (srcFileConnection != null)
                {
                    srcFileConnection.close();
                }
                srcFileConnection = null;

                if (dstFileConnection != null)
                {
                    dstFileConnection.close();
                }
                dstFileConnection = null;
            }
            catch (Exception e2)
            {
                //
            }
        }
        return;
    }

    
    /**
     *  �R�}���h�I�����̏���
     * 
     */
    public void commandAction(Command c, Displayable d)
    {
        if (c == exitCmd)
        {
            // �ꗗ�֖߂鏈��
            parent.EndFileSelector(null);
            return;
        }
        else if (c == selectCmd)
        {
            // �A�C�e���I�����̏���
            selection();
            return;
        }
        else if (c == deleteCmd)
        {
            // �t�@�C���폜�w�����̏���
            deleteItem();
            return;
        }
        else if (c == renameCmd)
        {
            // �t�@�C�����l�[���w��
            renameItem();
            return;
        }
        else if (c == getHttpCmd)
        {
            // html�ʐM�Ńt�@�C������M
            getHttpFile();
            return;
        }
        else if (c == copyCmd)
        {
            // �t�@�C���R�s�[�w��
            copyItem();
            return;
        }
        else if (c == infoCmd)
        {
            // �v���r���[���������s����
            doPreviewDat();
            return;
        }
        else if (c == rescanCmd)
        {
            // �ĕ`��w��...
            refreshScreen();
            return;
        }
        return;
    }

    /**
     *  �v���r���[�����̎��s
     * 
     *
     */
    private void doPreviewDat()
    {
        // �v���r���[�w�����̏���
        Thread thread = new Thread()
        {
            public void run()
            {
                // �t�@�C�������擾����
                String name = pickupFileName();
                if (name.equals(".."))
                {
                    // �e�f�B���N�g����I�������Ƃ��̓v���r���[��\�����Ȃ�(�\���ł��Ȃ�)
                    return;
                }

                // �f�[�^�t�@�C������(�t���p�X��)�\������
                name = path + name;
                if (name.endsWith("/"))
                {
                    // �f�B���N�g����I�������Ƃ��̓v���r���[��\�����Ȃ�(�\���ł��Ȃ�)
                    return;
                }
                parent.doPreviewFile(name, false, a2BsubjectDataParser.PARSE_2chMSG);
            }
        };
        thread.start();
    }
    
    /**
     *  ���l�[������t�@�C�������o�^���ꂽ�Ƃ�
     * 
     * 
     * @param fileName
     */
    public void doRenameFileName(String fileName)
    {
        // �t�@�C������ݒ肷��
        targetFilePath = targetFilePath + targetFileName;  // ���t�@�C���� (full path)
        targetFileName = fileName;                         // �V�t�@�C���� (�t�@�C��������)

        // �t�@�C���̉��������{����
        Thread thread = new Thread()
        {
            public void run()
            {
                doRenameMain(targetFileName, targetFilePath);
                targetFilePath = null;
                targetFileName = null;
                refreshScreen();
            }
        };
        thread.start();
        return;
    }

    /**
     * HTTP�Ńt�@�C����M���w�����ꂽ�Ƃ�
     * 
     * @param url
     */
    public void doReceiveHttp(String url)
    {
        String fileName = "index.html";
        int pos = url.lastIndexOf('/');
        if (url.length() > (pos + 1))
        {
            fileName = url.substring(pos + 1);
        }
        
        // �t�@�C������ݒ肷��
        targetFilePath = targetFilePath + fileName;  // �������ރt�@�C����
        targetFileName = url;                        // �擾��URL

        // HTTP�ʐM�����s����B�B
        Thread thread = new Thread()
        {
            public void run()
            {
                setTitle("�ʐM��:" + targetFileName);
                isUpdateTitle = false;
                parent.doHttpMain(targetFileName, targetFilePath, null, false, a2BMain.a2B_HTTPMODE_NOUSECOOKIE);
                targetFilePath = null;
                targetFileName = null;
                isUpdateTitle = true;
                refreshScreen();
                System.gc();
                parent.vibrate();
            }
        };
        thread.start();
        return;
    }
        
    /**
     *  �R�s�[����t�@�C�������o�^���ꂽ�Ƃ�
     *  (�t�@�C���R�s�[�����s)
     * 
     * @param fileName �R�s�[��t�@�C����
     */
    public void doCopyFileName(String fileName)
    {
        copyFile(targetFilePath + fileName, targetFilePath + targetFileName, true);
        return;
    }

    /**
     *   ��ʕ\���X�V���̏���...
     * 
     */
    public void refreshScreen()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                openDirectory(false);
            }
        };
        try
        {
            setTitle("�\���X�V��...");
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            // �������Ȃ�...
        }
        return;
    }

    /**
     * �t�@�C���R�s�[�̎��s
     * 
     * @param dstFileName �R�s�[��t�@�C���� (full path)
     * @param srcFileName �R�s�[���t�@�C���� (full path)
     */
    public void copyFile(String dstFileName, String srcFileName, boolean refresh)
    {
        targetFilePath  = dstFileName;
        targetFileName  = srcFileName;
        isRefreshScreen = refresh;

        // �t�@�C���̃R�s�[�����{����
        Thread thread = new Thread()
        {
            public void run()
            {
                if (isRefreshScreen == true)
                {
                    setTitle("wait...�R�s�[��");
                }
                doCopyFileMain(targetFilePath, targetFileName);
                targetFilePath = null;
                targetFileName = null;
                if (isRefreshScreen == true)
                {
                    openDirectory(false);
                }
                isRefreshScreen = false;
            }
        };
        thread.start();        
    }

    /**
     * �t�@�C�����폜����
     * 
     * @param fileName �폜����t�@�C���� (full path)
     */    
    public void deleteFile(String fileName, boolean refresh)
    {
        targetFileName = fileName;
        isRefreshScreen = refresh;

        Thread thread = new Thread()
        {
            public void run()
            {
                if (isRefreshScreen == true)
                {
                    setTitle("wait...�폜��");
                }
                doDeleteMain(targetFileName);
                targetFilePath = null;
                targetFileName = null;
                if (isRefreshScreen == true)
                {
                    openDirectory(false);
                }
                isRefreshScreen = false;
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            // �������Ȃ��B�B�B
        }
        return;
    }

    /**
     *  �t�@�C���T�C�Y���擾����
     * 
     * @param targetFileName �m�F����t�@�C����
     * @return �t�@�C���T�C�Y
     */
    static public long GetFileSize(String targetFileName)
    {
        long           ret = -1;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            ret = dataFileConnection.fileSize();
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
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
            ret = -1;
        }
        return (ret);
    }

    /**
     *  �t�@�C���̑��݂��m�F����
     * 
     * @param targetFileName �m�F����t�@�C����
     * @return ���݂���(true) / ���݂��Ȃ� (false)
     */
    static public boolean IsExistFile(String targetFileName)
    {
        boolean       ret = false;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            ret = dataFileConnection.exists();
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
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
            ret = false;
        }
        return (ret);
    }
}
//--------------------------------------------------------------------------
//  a2BfileSelector  --- �t�@�C���I��
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
