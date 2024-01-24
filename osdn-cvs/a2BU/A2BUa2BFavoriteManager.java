import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Vector;
import java.util.Date;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

//import a2BsubjectDataParser.subjectDataHolder;

/**
 *  ���C�ɓ���Ǘ��N���X
 * @author MRSa
 *
 */
public class A2BUa2BFavoriteManager
{
    // �X����Ԃ̒�`�B
    public  final byte STATUS_NOTYET   = 0;   // ���擾
    public  final byte STATUS_NEW      = 1;   // �V�K�擾
    public  final byte STATUS_UPDATE   = 2;   // �X�V�擾
    public  final byte STATUS_REMAIN   = 3;   // ���ǂ���
    public  final byte STATUS_ALREADY  = 4;   // ����
    public  final byte STATUS_RESERVE  = 5;   // �擾�\��
    public  final byte STATUS_UNKNOWN  = 6;   // �X����ԕs��
    public  final byte STATUS_GETERROR = 7;
    public  final byte STATUS_OVER     = 8;
    public  final byte STATUS_DEL_OFFL = 20;

    // �I�u�W�F�N�g
    private final String    favoriteFile  = "favorite.idx";
    private final int       bufferMargin  = 16; 
    
    private String           baseDirectory = null;
    private Vector           favoriteList  = null;

    /**
     * �R���X�g���N�^
     */
    public A2BUa2BFavoriteManager(String directory)
    {
        baseDirectory = directory;
        favoriteList = new Vector();
    }
    
    /**
     *  ����...
     */
    public void prepare()
    {
        // �Ƃ肠�����A�_�~�[���\�b�h
    }
    
    /**    
     *  �X�������C�ɓ���ɓo�^�ς݂��`�F�b�N����
     *  (�o�^�ς�index�ԍ�����������)
     *
     */
    public int exist(String nick, long threadNumber)
    {
        int limit = favoriteList.size();
        for (int loop = 0; loop < limit; loop++)
        {
            a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(loop);
            if (data != null)
            {
                if (data.threadNumber == threadNumber)
                {
                    String boardNick = data.getBoardNick();
                    if (boardNick.startsWith(nick) == true)
                    {
                        // ���C�ɓ���ɓo�^�ς�
                        return (loop);
                    }
                }
            }            
        }
        return (-1);
    }
    
    /**
     *  ���C�ɓ���o�^�ς݃X���̏ꍇ�A�폜����
     *
     */
    public void removeFavorite(String nick, long threadNumber)
    {
        int limit = favoriteList.size();
        for (int loop = 0; loop < limit; loop++)
        {
            a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(loop);
            if (data != null)
            {
                if (data.threadNumber == threadNumber)
                {
                    String boardNick = data.getBoardNick();
                    if (boardNick.startsWith(nick) == true)
                    {
                        // ���C�ɓ��肩���������
                        favoriteList.removeElementAt(loop);
                        data = null;
                        return;
                    }
                }
            }
        }
        return;
    }
/*
    // ���C�ɓ���o�^�ς݃X���̏ꍇ�A�X�V����
    public void updateFavorite(String nick, long threadNumber, String title, int maxMessage, int currentMessage, byte status)
    {
        int limit = favoriteList.size();
        for (int loop = 0; loop < limit; loop++)
        {
            a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(loop);
            if (data != null)
            {
                if (data.threadNumber == threadNumber)
                {
                    String boardNick = data.getBoardNick();
                    if (boardNick.startsWith(nick) == true)
                    {
                        // �X�����C�ɓ����Ԃ��X�V����
                        if (maxMessage >= 0)
                        {
                            data.numberOfMessage = maxMessage;
                        }                        
                        if (currentMessage >= 0)
                        {
                            data.currentMessage  = currentMessage;
                        }
                        if (status >= 0)
                        {
                            data.currentStatus   = status;
                        }
                        isUpdated = true;
                        return;
                    }
                }
            }            
        }
        return;
    }
*/
    /**
     *   ���C�ɓ������W�J����
     * 
     */
    public void restore()
    {
        FileConnection connection    = null;
        InputStream    in            = null;

        byte[]         buffer        = null;
        int            dataReadSize  = 0;
        
        // �t�@�C�����𐶐�����
        String fileName = baseDirectory + favoriteFile;
        try
        {
            connection   = (FileConnection) Connector.open(fileName, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  �f�[�^���ŏ����� size���ǂݏo��...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + bufferMargin];
            dataReadSize = in.read(buffer, 0, size);

            in.close();
            connection.close();
            in = null;
            connection = null;
        }
        catch (Exception e)
        {
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

        // �f�[�^�̓ǂݏo�����C������...
        for (int offset = 0; offset < dataReadSize; offset++)
        {
            int length = 0;
            if ((buffer[offset] == 10)||(buffer[offset] == 0x0a))
            {
                try
                {
                    // 1�s�P�ʂŃf�[�^�𒊏o����...
                    length = pickupData(buffer, (offset + 1), dataReadSize);
                    offset = offset + length;                    
                }
                catch (Exception e)
                {
                    // ��O����...
                    offset = offset + dataReadSize;
                }
            }
        }
        buffer = null;
        System.gc();
        return;
    }

    /**
     *   ���C�ɓ������ۊǂ���
     * 
     */
    public void backup()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                // �f�[�^�̃o�b�N�A�b�v�����s����
                backupMain();
            }
        };
        try
        {
            // �o�b�N�A�b�v
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            // �������Ȃ�...
        }
        System.gc();
        return;
    }
        
    /**
     *   ���C�ɓ������ۊǂ���
     * 
     */
    private void backupMain()
    {
        FileConnection connection = null;
        OutputStream  out = null;
        
        // �t�@�C�����𐶐�����
        String fileName = baseDirectory + favoriteFile;
        try 
        {
            // �t�@�C���̏�������
            connection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
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
            out        = connection.openOutputStream();

            // �C���f�b�N�X�f�[�^�̃R�����g���o�͂���
            String comment = ";; fileName, max, current, status, level, dateTime, 0, 0, url, nick, (Title)\n";
            out.write(comment.getBytes());

            Enumeration cnt = favoriteList.elements();
            while (cnt.hasMoreElements())
            {
                a2BFavoriteThread data = (a2BFavoriteThread) cnt.nextElement();
                if (data != null)
                {
                    String outputData = data.threadNumber + ".dat," + data.numberOfMessage + ",";
                    outputData = outputData + data.currentMessage + "," + data.currentStatus + ",";
                    outputData = outputData + data.threadLevel + "," + data.lastModified + ",0,0,";
                    outputData = outputData + data.getThreadUrl() + "," + data.getBoardNick() + ",";
                    out.write(outputData.getBytes());
                    out.write(data.getThreadTitle());
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
                //
            }
        }
        out = null;
        connection = null;
        System.gc();
        return;        
    }

    /**
     *  �L�[���[�h�̏�����
     *
     */
    public void clean()
    {
        favoriteList = null;
        System.gc();
    }

    /**
     *  ���C�ɓ���̓o�^����Ԃ�
     *
     */
    public int getNumberOfFavorites()
    {
        return (favoriteList.size());
    }

    /**
     *  ���C�ɓ���̃X�������擾����
     *
     */
    public a2BFavoriteThread getThreadData(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (null);
        }
        return ((a2BFavoriteThread) favoriteList.elementAt(index));
    }
    
    /**
     *  ���C�ɓ���̃X����Ԃ��X�V����
     * @param index
     * @param status
     * @param number
     * @param maxNumber
     */
    public void setStatus(int index, byte status, int number, int maxNumber)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return;
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return;
        }
        if (status >= 0)
        {
            data.currentStatus = status;
        }
        Date curTime = new Date();
        data.lastModified = curTime.getTime();
        
        if (number > 0)
        {
            data.currentMessage = number;
        }
        if (maxNumber > 0)
        {
            data.numberOfMessage = maxNumber;
        }
        return;
    }

    /**
     *  ���C�ɓ���Ƀf�[�^�o�^����
     *
     */
/*
    public void entryFavorite(byte[] buffer, String url, String nick, long threadNumber, int maxNumber, int currentNumber, byte currentStatus)
    {
        // �f�[�^��Vector�̈�Ɋi�[����
        a2BFavoriteThread dataHolder    = new a2BFavoriteThread(buffer, url, nick, threadNumber);
        dataHolder.numberOfMessage   = maxNumber;
        dataHolder.currentMessage    = currentNumber;
        dataHolder.currentStatus     = currentStatus;
        favoriteList.insertElementAt(dataHolder, 0);
        
        return;
    }
*/

    /**
     *   FAVORITE.CSV ���� Vector�փf�[�^��W�J����...
     *   (���Ȃ�x�^�x�^�x�^...)
     * 
     */
    private int pickupData(byte[] buffer, int offset, int limit)
    {
        int length = 0;

        // �f�[�^���Ȃ��Ƃ�...
        if (offset + 12 > limit)
        {
            length = limit - offset + 1;
            return (length);
        }
        
        // �R�����g�s�������A�A�A�P�s�ǂݔ�΂�
        if (buffer[offset] == 59)
        {
            while (((offset + length) < limit)&&(buffer[offset + length] != 10))
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

        // �X�����x���𒊏o
        int threadLevel = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            threadLevel = threadLevel * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // ���X�V���Ԃ𒊏o
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

        //�@�_�~�[�̈�R
        int dummy3 = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            dummy3 = dummy3 * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // �X��URL�𒊏o����
        int urlOffset = offset + length;
        int urlLength = 0;
        while ((buffer[urlOffset + urlLength] != 0)&&(buffer[urlOffset + urlLength] != 10)&&(buffer[urlOffset + urlLength] != 44))
        {
            urlLength++;
        }
        String url = new String(buffer, urlOffset, urlLength);

        // �{�[�h�j�b�N�l�[���𒊏o����
        int nickOffset = urlOffset + urlLength + 1;
        int nickLength = 0;
        while ((buffer[nickOffset + nickLength] != 0)&&(buffer[nickOffset + nickLength] != 10)&&(buffer[nickOffset + nickLength] != 44))
        {
            nickLength++;
        }
        String nick = new String(buffer, nickOffset, nickLength);

        // �^�C�g�����𒊏o����
        offset     = nickOffset + nickLength + 1;
        nameOffset = offset;
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

        // �f�[�^��Vector�̈�Ɋi�[����
        a2BFavoriteThread dataHolder    = new a2BFavoriteThread(buffer, nameOffset, nameLength, url, nick, threadNumber);
        dataHolder.numberOfMessage   = maxNumber;
        dataHolder.currentMessage    = currentNumber;
        dataHolder.currentStatus     = (byte) currentStatus;
        dataHolder.threadLevel       = threadLevel;
        dataHolder.lastModified      = lastModified;
        favoriteList.addElement(dataHolder);
        
        // �ꉞ�A�ϐ��N���A
        url  = null;
        nick = null;

        return (length);
    }

    /**
     * ���C�ɓ���X��URL�̍X�V
     * 
     * @return
     */
    public boolean updateURLData(A2BUa2BbbsCommunicator bbsAccessor)
    {
        if (bbsAccessor == null)
        {
            return (false);
        }

        // ���C�ɓ���X���̑S���AURL���X�V���� 
        // (�������A�A���炢���Ԃ������邾�낤�Ȃ�...)
        Enumeration cnt = favoriteList.elements();
        while (cnt.hasMoreElements())
        {
            a2BFavoriteThread data = (a2BFavoriteThread) cnt.nextElement();
            if (data != null)
            {
                data.boardUrl = bbsAccessor.getBoardURL(data.getBoardNick());
            }
        }
        backupMain();  // �X�V���ʂ��t�@�C���o�͂���
        return (true);
    }
    
    /**
     *  ���C�ɓ���X���̕ێ��N���X
     * 
     * @author MRSa
     *
     */
    public class a2BFavoriteThread
    {
        private final int MARGIN      = 2;      // ���܂�̈�

        public long   threadNumber     = 0;     // �X���Ǘ��ԍ�
        public long   lastModified     = 0;     // �X�V����
        public int    numberOfMessage  = 0;     // ���o�ς݃��X��
        public int    currentMessage   = 1;     // �Q�ƒ����X�ԍ�
        public int    threadLevel      = 0;     // �X�����x��
//        public int    updated          = 0;     // �X�V�t���O
        public byte   currentStatus    = 0;     // �X���Q�Ə��
        
        private String  boardNick      = null;  // �X���j�b�N�l�[��
        private String  boardUrl       = null;  // �X��URL
        private byte[] threadTitle     = null;  // �X���^�C�g��
        
        static public final int LEVEL_OTHER = -1;
        static public final int LEVEL_NONE  = 0;   // �X�����x���Ȃ�
        static public final int LEVEL_1     = 1;   // �X�����x���P
        static public final int LEVEL_2     = 2;   // �X�����x���Q
        static public final int LEVEL_3     = 3;   // �X�����x���R
        static public final int LEVEL_4     = 4;   // �X�����x���S
        static public final int LEVEL_5     = 5;   // �X�����x���T
        static public final int LEVEL_6     = 6;   // �X�����x���U
        static public final int LEVEL_7     = 7;   // �X�����x���V
        static public final int LEVEL_8     = 8;   // �X�����x���W
        static public final int LEVEL_9     = 9;   // �X�����x���X

        /**
         * �R���X�g���N�^
         *
         */
        public a2BFavoriteThread(byte[] title, String url, String nick, long number)
        {
            int len     = title.length;
            threadTitle  = new byte[len + MARGIN];
            boardUrl     = new String(url);
            boardNick    = new String(nick);
            System.arraycopy(title, 0, threadTitle, 0, len);

            threadNumber     = number;
            threadTitle[len] = 0;
            threadLevel      = LEVEL_1;
        }

        /**
         * �R���X�g���N�^
         *
         */
        public a2BFavoriteThread(byte[] title, int offset, int len, String url, String nick, long number)
        {
            threadTitle = new byte[len + MARGIN];
            boardUrl    = new String(url);
            boardNick   = new String(nick);
            System.arraycopy(title, offset, threadTitle, 0, len);

            threadNumber     = number;
            threadTitle[len] = 0;
            threadLevel      = LEVEL_1;
        }

        /**
         * �X���^�C�g������������
         *
         */
        public byte[] getThreadTitle()
        {
            return (threadTitle);
        }

        /**
         * �X����URL����������
         *
         */
        public String getThreadUrl()
        {
            return (boardUrl);
        }

        /**
         * �X����URL��ݒ肷��
         *
         */
        public void setThreadUrl(String url)
        {
            boardUrl = url;
        }

        /**
         * �X�����Ǘ�����Ă���j�b�N�l�[�����擾����
         *
         */
        public String getBoardNick()
        {
            return (boardNick);
        }

        /**
         * �^�C�g������ύX����
         *
         */
        public void changeTitleName(byte[] name, int offset, int len)
        {
            threadTitle = null;
            threadTitle = new byte[len + MARGIN];
            System.arraycopy(name, offset, threadTitle, 0, len);
            threadTitle[len] = 0;
            System.gc();
        }
    }
}
//--------------------------------------------------------------------------
//  a2BFavoriteManager  --- ���C�ɓ���̃f�[�^�Ǘ�
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
