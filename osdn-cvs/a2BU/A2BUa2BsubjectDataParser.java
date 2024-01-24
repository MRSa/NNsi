import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import java.util.Vector;
import java.util.Enumeration;

public class A2BUa2BsubjectDataParser
{

    // �X���Ǘ��p�f�[�^
    private Vector    subjectVector    = null;
    private String    currentBoardNick = null;

    // �X����Ԃ̒�`�B
    static public final byte STATUS_NOTYETOLD  = 0;   // ���擾
    static public final byte STATUS_NEW        = 1;   // �V�K�擾
    static public final byte STATUS_UPDATE     = 2;   // �X�V�擾
    static public final byte STATUS_REMAIN     = 3;   // ���ǂ���
    static public final byte STATUS_ALREADY    = 4;   // ����
    static public final byte STATUS_RESERVE    = 5;   // �擾�\��
    static public final byte STATUS_UNKNOWN    = 6;   // �X����ԕs��
    static public final byte STATUS_GETERROR   = 7;
    static public final byte STATUS_OVER       = 8;   // 1000���X��
    static public final byte STATUS_NOTYET     = 10;  // ���擾
    static public final byte STATUS_DEL_OFFL   = 20;
    static public final byte STATUS_NOTSPECIFY = 127; // �X�e�[�^�X����`

    // �N���X...
    private A2BUSceneStorage sceneStorage = null;

    /*
     *   �R���X�g���N�^
     *   
     */
    public A2BUa2BsubjectDataParser(A2BUSceneStorage storageDb)
    {
        sceneStorage = storageDb;
    }

    /**
     *   a2b.idx ��ǂݍ��݁A�������֓W�J����
     *   (�ȑO�ɓǂ�ł����f�[�^�͔j������)
     */
    public void prepareA2BIndex(String boardNick)
    {
        // �ȑO�ɓǂݍ���ł��� a2b.idx �����Z�b�g����
        resetA2BIndex();

        subjectVector = new Vector();
        currentBoardNick = boardNick;

        FileConnection connection    = null;
        InputStream    in            = null;

        byte[]         buffer        = null;
        int            dataReadSize  = 0;
        
        // �ǂݍ��ރt�@�C�����𐶐�����
        String indexFile = sceneStorage.getDefaultDirectory() + currentBoardNick + "/" + "a2b.idx";
        try
        {
            connection   = (FileConnection) Connector.open(indexFile, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  �f�[�^���ŏ����� size���ǂݏo��...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + 8];
            dataReadSize = in.read(buffer, 0, size);

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
        }

        for (int offset = 0; offset < dataReadSize; offset++)
        {
            int length = 0;
            if (buffer[offset] == 10)
            {
                try
                {
                    // 1�s�P�ʂŃf�[�^�𒊏o����...
                    length = pickupData(boardNick, buffer, (offset + 1), dataReadSize);
                    offset = offset + length;
                }
                catch (Exception e)
                {
                    // ��O����...�f�[�^��ǂݔ�΂��悤�ɂ���
                    offset = offset + dataReadSize;
                }
            }
        }
        buffer = null;
        System.gc();
        return;
    }
    
    /**
     *   a2b.idx ���� hashTable�փf�[�^��W�J����...
     *   (���Ȃ�x�^�x�^...)
     * 
     */
    private int pickupData(String nick, byte[] buffer, int offset, int limit)
    {
        int length = 0;

        // �f�[�^���Ȃ��Ƃ�...
        if (offset + 13 > limit)
        {
            length = limit - offset + 1;
            return (length);
        }
        
        // �R�����g�s�������A�A�A�P�s�ǂݔ�΂�
        if (buffer[offset] == 59)
        {
            while (buffer[offset + length] != 10)
            {
                length++;
            }
            return (length);
        }

        int nameOffset = offset + length;
        int nameLength = 0;

        // �t�@�C�����𒊏o����
        long threadNumber = 0;
        while ((buffer[nameOffset + nameLength] >= '0')&&(buffer[nameOffset + nameLength] <= '9'))
        {
            threadNumber = threadNumber * 10 + (buffer[nameOffset + nameLength] - '0');
            nameLength++;
        }
        while (buffer[nameOffset + nameLength] != 44)
        {
            nameLength++;
        }

        length = (nameOffset + nameLength + 1) - offset;

        // �ő僌�X���𒊏o
        int maxNumber = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            maxNumber = maxNumber * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // ���݃��X���𒊏o
        int currentNumber = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            currentNumber = currentNumber * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;
        
        // ���݃X�e�[�^�X�𒊏o
        int currentStatus = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            currentStatus = currentStatus * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // �ŏI�X�V����
        long lastModified = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            lastModified = lastModified * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;
        
        //�@�_�~�[�̈�Q
        int dummy2 = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            dummy2 = dummy2 * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // �^�C�g�����𒊏o����
        nameOffset = offset + length;
        nameLength = 0;
        while ((buffer[nameOffset + nameLength] != 0)&&(buffer[nameOffset + nameLength] != 10))
        {
            nameLength++;
        }

        length = (nameOffset + nameLength) - offset;
        while (buffer[offset + length] != 10)
        {
            length++;
        }

        //  �f�[�^���e�[�u���Ɋi�[����
        subjectDataHolder dataHolder  = new subjectDataHolder(buffer, nameOffset, nameLength);
        dataHolder.threadNumber      = threadNumber;
        dataHolder.numberOfMessage   = maxNumber;
        dataHolder.currentMessage    = currentNumber;
        dataHolder.currentStatus     = (byte) currentStatus;
        dataHolder.lastModified      = lastModified;
        dataHolder.favoriteIndex     = -1;

        int favorIndex =  sceneStorage.isExistFavorite(nick, threadNumber);
        if (favorIndex >= 0)
        {
            // ���C�ɓ���o�^�ς݃f�[�^
            dataHolder.favoriteIndex = favorIndex;
            
            // ���C�ɓ���ɋL�^����Ă���X����Ԃ��擾����
            A2BUa2BFavoriteManager.a2BFavoriteThread data = sceneStorage.getFavoriteThreadData(favorIndex);
            try
            {
                if (data != null)
                {
                    // ���C�ɓ���f�[�^���X�V����Ă������H�H
                    if (data.lastModified > lastModified)
                    {
                        // �X����ԁA���X�ő吔�A���݂̓ǂ񂾈ʒu��(���C�ɓ���̂��̂�)���f������
                        dataHolder.currentStatus   = data.currentStatus;
                        dataHolder.numberOfMessage = data.numberOfMessage;
                        dataHolder.currentMessage  = data.currentMessage;
                        dataHolder.lastModified    = data.lastModified;
                    }
                    else
                    {
                        // �ʏ��Ԃ�ʏ�ꗗ �� ���C�ɓ���֔��f������
                        data.currentStatus   = dataHolder.currentStatus;
                        data.currentMessage  = dataHolder.currentMessage;
                        data.numberOfMessage = dataHolder.numberOfMessage;
                        data.lastModified    = dataHolder.lastModified;
                    }

                    if (data.numberOfMessage > dataHolder.numberOfMessage)
                    {
                        // a2B.idx�����C�ɓ���̏�Ԃɓ���������
                        dataHolder.numberOfMessage = data.numberOfMessage;
                        dataHolder.currentMessage  = data.currentMessage;
                    }
                    else if (data.numberOfMessage < dataHolder.numberOfMessage)
                    {
                        // ���C�ɓ���̃f�[�^��Ԃ� a2B.idx�ɓ���������
                        data.currentStatus   = dataHolder.currentStatus;
                        data.currentMessage  = dataHolder.currentMessage;
                        data.numberOfMessage = dataHolder.numberOfMessage;
                        data.lastModified    = dataHolder.lastModified;
                    }
                }
            }
            catch (Exception e)
            {
                //
            }
        }
//        subjectMap.put(fileName, dataHolder);
        subjectVector.addElement(dataHolder);

        return (length);
    }

    /**
     *  �ǂݏo�����X���̐�����������
     *  
     * @return
     */
    public int getNumberOfThread()
    {
        if (subjectVector == null)
        {
            return (-1);
        }
        return (subjectVector.size());
    }
    
    /**
     * �X���Ǘ��N���X���擾����
     * 
     * 
     */
    public subjectDataHolder getSubjectData(int index)
    {
        subjectDataHolder dataholder = null;
        try
        {
            dataholder = (subjectDataHolder) subjectVector.elementAt(index);
        }
        catch (Exception e)
        {
            return (null);
        }
        return (dataholder);
    }

    /**
     *   �C���f�b�N�X�����t�@�C���o�͂��A�C���f�b�N�X�f�[�^���N���A����
     *   
     */
    public void outputA2BIndex()
    {
        if ((subjectVector == null)||(currentBoardNick == null))
        {
            // �f�[�^���Ȃ��Ƃ��ɂ́A�t�@�C���o�͂��Ȃ�
            return;
        }
        FileConnection connection = null;
        OutputStream  out = null;
        
        // �o�̓t�@�C�����𐶐�����
        String indexFile = sceneStorage.getDefaultDirectory() + currentBoardNick + "/" + "a2b.idx";
        try 
        {
            // �t�@�C���̏�������
            connection = (FileConnection) Connector.open(indexFile, Connector.READ_WRITE);
            if (connection.exists() != true)
            {
                // �t�@�C���쐬
                connection.create();
            }
            else
            {
                // �t�@�C�������݂����ꍇ�ɂ́A��x�����č�蒼��
                connection.delete();
                connection.create();
            }
            out = connection.openOutputStream();

            // �C���f�b�N�X�f�[�^�̃R�����g���o�͂���
            String comment = "; fileName, max, current, status, lastModified, 0, (Title)\n";
            out.write(comment.getBytes());

            Enumeration cnt = subjectVector.elements();
            while (cnt.hasMoreElements())
            {
                subjectDataHolder data = (subjectDataHolder) cnt.nextElement();
                if (data.currentStatus != STATUS_NOTYET)
                {
                    String outputData = data.threadNumber + ".dat" + "," + data.numberOfMessage + ",";
                    outputData = outputData + data.currentMessage + "," + data.currentStatus + ",";
                    outputData = outputData + data.lastModified + "," + 0 + ",";
                    out.write(outputData.getBytes());
                    out.write(data.titleName);
                    outputData = null;
                    outputData = "\n";
                    out.write(outputData.getBytes());
                }
            }
            comment = ";[End]";
            out.write(comment.getBytes());
            out.close();
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            try
            {
                if (out != null)
                {
                    out.close();
                }
            }
            catch (Exception e2)
            {
            
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
            
            }
        }
        return;
    }

    /**
     *  �ǂݍ���ł��� a2b.idx �����Z�b�g(�N���A)����
     *
     */
    public void resetA2BIndex()
    {
        // �X���ꗗ�C���f�b�N�X���폜����
        if (subjectVector != null)
        {
            subjectVector.removeAllElements();
        }
        subjectVector    = null;
        currentBoardNick = null;
        
        System.gc();
    }
    
    /**
     *  �X���f�[�^�������N���X
     * 
     */
    public class subjectDataHolder
    {
        private final int MARGIN = 2;
        
        public long    threadNumber    = 0;       // �X���ԍ�
        public long    lastModified    = 0;       // ���X��ԍX�V����
        public int     numberOfMessage = 0;       // ���X��
        public int     currentMessage  = 1;       // ���ݕ\�����ӏ�(���X�ԍ�)
        public byte    currentStatus   = 0;       // ���݂̏��
        public int     favoriteIndex   = 0;       // ���C�ɓ���o�^�ς݂��ǂ���
        private byte[] titleName       = null;   // �X���^�C�g��
        public int     titleSize       = 0;       // �X���^�C�g���T�C�Y

        // �R���X�g���N�^
        public subjectDataHolder(byte[] name, int offset, int len)
        {
            titleName = new byte[len + MARGIN];
            System.arraycopy(name, offset, titleName, 0, len);
            titleName[len] = 0;
            titleSize = len;
        }

        // �^�C�g�������擾����
        public byte[] getTitleName()
        {
            return (titleName);
        }
        
        // �^�C�g������ύX����
        public void changeTitleName(byte[] name, int offset, int len)
        {
            titleName = null;
            titleName = new byte[len + MARGIN];
            System.arraycopy(name, offset, titleName, 0, len);
            titleName[len] = 0;
            titleSize = len;
            System.gc();
        }
    }
}

//--------------------------------------------------------------------------
//  a2BsubjectDataParser  --- �X���ꗗ�f�[�^��́��ێ��N���X
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
