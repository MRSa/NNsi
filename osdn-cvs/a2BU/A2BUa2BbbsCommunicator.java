import java.io.InputStream;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 *  BBS�f�[�^�Ǘ��N���X...
 * 
 * @author MRSa
 *
 */
public class A2BUa2BbbsCommunicator
{
    private String    baseDirectory = null;
    private Hashtable bbsList       = null;
    private Hashtable usingBbsList  = null;
    private Vector    getBbsList    = null;

    private final String FILE_BBSTABLE = "bbstable.html";  // �ꗗ�̃t�@�C����
    private final int    MARGIN = 16;
    
    /**
     *   �R���X�g���N�^
     *
     */
    public A2BUa2BbbsCommunicator(String directory)
    {
        baseDirectory = directory;
        bbsList = new Hashtable();
        usingBbsList = new Hashtable();
    }

    /**
     *   �ꗗ�f�[�^�x�[�X���쐬(����)����
     *   
     */
    public void prepare()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                // �ꗗ�f�[�^�x�[�X�̍\�z
                updateBbsDatabase();
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
            // �������Ȃ�...
        }
        System.gc();
        return;
    }
    
    /**
     *   (��Nick�f�B���N�g�������݂��Ȃ����)�f�B���N�g�����쐬����
     *  
     */
    public boolean checkBoardNickName(String nickName)
    {
        boolean ret = true;
        FileConnection fc = null;
        String directory = baseDirectory + nickName;
        try 
        {
            fc = (FileConnection) Connector.open(directory, Connector.READ_WRITE);
            if (fc.exists() == false)
            {
                fc.mkdir();
                ret = false;
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
                // �������Ȃ�...
            }
            return (false);
        }
        return (ret);
    }

    /**
     *   �o�^�ςݔ�����������
     *   
     */
    public int getNumberOfBoardName()
    {
        return (getBbsList.size());
    }
    
    /**
     *  ��Index����AURL���擾����
     * @param boardIndex
     * @return
     */
    public String getBoardUrl(int boardIndex)
    {
        String boardUrl = null;
        try
        {
            bbsDataHolder holder = (bbsDataHolder) getBbsList.elementAt(boardIndex);
            if (holder != null)
            {
                boardUrl = holder.getBoardUrl();
            }
        }
        catch (Exception ex)
        {
            return ("");
        }
        return (boardUrl);
    }

    /**
     * ��Nick����AURL���擾����...
     * 
     * @param boardNick
     * @return
     */
    public String getBoardURL(String boardNick)
    {
        bbsDataHolder holder = (bbsDataHolder) usingBbsList.get(boardNick);
        if (holder == null)
        {
            return ("");
        }
        return (holder.getBoardUrl());
    }

    /**
     *  �g�p���Ă���Ƃ��ă}�[�N����
     * 
     * @param boardNick
     * @return
     */
    public boolean markUsingBbs(String boardNick)
    {
    	String key = boardNick.toLowerCase();
        bbsDataHolder holder = (bbsDataHolder) bbsList.get(key);
        if (holder == null)
        {
            return (false);
        }
        usingBbsList.put(boardNick, holder);
        return (true);
    }
    
    /**
     *  �ꗗ���g�p������̂����ɏk������
     * 
     * @return
     */
    public int trimBbsList()
    {
        bbsList = null;
        getBbsList = new Vector();
        for (Enumeration e = usingBbsList.elements() ; e.hasMoreElements(); )
        {
            getBbsList.addElement(e.nextElement());
        }
        return (getBbsList.size());
    }

    /**
     *   �J�e�S�����ꗗ�f�[�^�x�[�X�́i�������j�\�z����
     *   
     */
    private void parseBbsDatabase(byte[] buffer, int bufferSize, int offset)
    {
        int loop;

        // �J�e�S���A�{�[�hURL�A�{�[�h���𒊏o����B
        for (loop = 0; loop < bufferSize; loop++)
        {
            // �ꗗDB�쐬�J�n...
            if ((buffer[loop] == 'h')&&(buffer[loop + 1] == 't')&&(buffer[loop + 2] == 't')&&(buffer[loop + 3] == 'p'))
            {
                // URL���o!
                int index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != '>')&&(buffer[loop + index] != 34)&&(buffer[loop + index] != 39))
                {
                    index++;
                }
                // URL������荞�ށI
                String boardUrl = new String(buffer, loop, (index - 1));
                loop = loop + index;
                if (buffer[loop] != '>')
                {
                    // �z��O�̃f�[�^...
                    continue;
                }
                loop++;

                // ��Nick�𒊏o����
                String boardNick = convertBoardNick(boardUrl);
                if (boardNick != null)
                {
                    // �f�[�^��Vector�̈�Ɋi�[����
                    boardNick = boardNick + "/";
                    bbsDataHolder dataHolder    = new bbsDataHolder(boardNick, boardUrl);
                    bbsList.put(boardNick, dataHolder);
                }
                
                // �{�[�h���̂����o����
                index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != '<'))
                {
                    index++;
                }
                loop = loop + index;
            }
        }
        return;
    }

    /**
     *  ��Nick�𐶐�����
     * @param boardUrl
     * @return
     */
    public String convertBoardNick(String boardUrl)
    {
        // �擾�您��ю擾�t�@�C�����𐶐�����
        int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
        if (pos < 7)
        {
            // '/' �� "http://" �̂Ƃ���Ƀ}�b�`����悤��������A��Nick�ł͂Ȃ��A�Ɣ��f����
            return ("");
        }

        // �j�b�N�l�[�������肷��
        return (boardUrl.substring(pos + 1));
    }    
    
    /**
     *   �ꗗ�f�[�^�x�[�X���쐬����...
     *   
     */
    private void updateBbsDatabase()
    {
        FileConnection  bbsFileConnection  = null;
        InputStream     in                 = null;
        String          fileName           = baseDirectory + FILE_BBSTABLE;    
        byte[]         buffer             = null;

        try
        {
            bbsFileConnection = (FileConnection) Connector.open(fileName, Connector.READ);

            // �f�[�^�ǂݏo������...
            int dataFileSize = (int) bbsFileConnection.fileSize();
            int dataReadSize = 0;
            buffer = new byte[dataFileSize + MARGIN];

            // �t�@�C������f�[�^��ǂݏo��...
            in = bbsFileConnection.openInputStream();
            dataReadSize = in.read(buffer, 0, dataFileSize);            

            // ���\�[�X���N���[�Y����
            in.close();
            bbsFileConnection.close();

            // �f�[�^�̉�͂����{����
            parseBbsDatabase(buffer, dataReadSize, 0);
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
                if (bbsFileConnection != null)
                {
                    bbsFileConnection.close();
                }
            }
            catch (Exception e2)
            {
                //
            }    
        }
        in = null;
        bbsFileConnection = null;
        buffer = null;
        return;
    }

    /**
     *     ����ێ�����N���X
     *
     */
    public class bbsDataHolder
    {
        private String boardUrl  = null;
        private String boardNick = null;    
        /**
         *  �R���X�g���N�^
         * @param url
         */
        public bbsDataHolder(String nick, String url)
        {
            boardUrl = url;
            boardNick = nick;
        }

        /**
         * ��nick�𒊏o����
         * @return
         */
        public String getBoardNick()
        {
            return (boardNick);
        }

        /**
         *  ��URL���擾����
         * @return
         */        
        public String getBoardUrl()
        {
            return (boardUrl);
        }
    }
}
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
//
