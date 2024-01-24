import java.io.InputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 *  BBS�f�[�^�Ǘ��N���X...
 * 
 * @author MRSa
 *
 */
public class a2BbbsCommunicator
{
    // �I�u�W�F�N�g
    private a2BMain         parent        = null;
    private a2Butf8Conveter utf8Converter = null;

    // �e��}�W�b�N�i���o�[
    private final int   BBS_MAX_CATEGORY   = 60;   // �Ǘ��\�ȔJ�e�S����
    private final int   BBS_MAX_ARRAY      = 96;   // �J�e�S���Ɋ܂܂��̍ő吔
    private final int   MARGIN             = 4;    // �f�[�^�o�b�t�@

    private final String FILE_BBSTABLE      = "bbstable.html";                      // �ꗗ�̃t�@�C����

    // ���A�J�e�S�����̊Ǘ�...
    private int          bbsCategoryCount   = 0;
    private int          bbsCount           = 0;

    private bbsCategoryHolder[]  bbsCategoryList     = null;
    private bbsDataHolder[]      bbsInformationArray = null;
    private int                  bbsInformationData  = 0;

    /**
     *   �R���X�g���N�^
     *
     */
    public a2BbbsCommunicator(a2BMain object, a2Butf8Conveter converter)
    {
        // �e���L�^����...
        parent = object;
        utf8Converter = converter;

        // �J�e�S���e�[�u���̏���...
        bbsCategoryList = new bbsCategoryHolder[BBS_MAX_CATEGORY];

        // �e�[�u���̏���...
        bbsInformationArray = new bbsDataHolder[BBS_MAX_ARRAY];
    }

    /**
     *   �ꗗ�f�[�^�x�[�X���쐬(����)����
     *   
     */
    public void prepareBbsDatabase()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                // �ꗗ�f�[�^�x�[�X�̍\�z
                updateBbsDatabase();
            }
        };
        thread.start();
        return;
    }

    /**
     * ��nick�𒊏o����
     * @param boardUrl
     * @return
     */
    public String getBoardNickName(String boardUrl)
    {

        // �擾�您��ю擾�t�@�C�����𐶐�����
//        int pos = gettingLogDataURL.lastIndexOf('/', gettingLogDataURL.length() - 2);

        // �擾�您��ю擾�t�@�C�����𐶐�����
        int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);

        // �j�b�N�l�[�������肷��
        return (boardUrl.substring(pos + 1));
    }    
    
    /**
     *   (��Nick�f�B���N�g�������݂��Ȃ����)�f�B���N�g�����쐬����
     *  
     */
    public boolean checkBoardNickName(String nickName)
    {
        boolean ret = true;
        FileConnection fc = null;
        String directory = parent.getBaseDirectory() + nickName;
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
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXCEPTION(checkNick)  e:" + e.getMessage());
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
     *   �J�e�S�����̂��擾����
     *   
     */
    public String getCategory(int index)
    {
        if ((index < 0)&&(index >= bbsCategoryCount))
        {
            index = 0;
        }
        return (utf8Converter.parsefromSJ(bbsCategoryList[index].categoryName, 0, bbsCategoryList[index].categoryName.length));
    }

    /**
     *   �J�e�S���Ɋ܂܂��̐擪index�ԍ����擾����
     *   
     */
    public int getCategoryIndex(int index)
    {
        if ((index < 0)&&(index >= bbsCategoryCount))
        {
            index = 0;
        }
        return (bbsCategoryList[index].categoryIndex);
    }

    /**
     *   �o�^�ςݔ�����������
     *   
     */
    public int getNumberOfBoardName()
    {
        return (bbsCount);
    }

    /**
     *  �o�^�ς݃J�e�S��������������
     *  
     */
    public int getNumberOfCategory()
    {
        return (bbsCategoryCount);
    }

    /**
     *   ���݂̔��̂��擾����
     *   
     */
    public String getBoardName(int index)
    {
        int categoryIndex          = parent.getCurrentCategoryIndex();
        int categoryIndexOffset    = bbsCategoryList[categoryIndex].categoryIndex;
        int categoryIndexMaxOffset = bbsCategoryList[categoryIndex + 1].categoryIndex;
        if ((categoryIndexOffset > index)||(categoryIndexMaxOffset <= index))
        {
            return (".");
        }
        if ((bbsInformationArray == null)||(bbsInformationArray[index - categoryIndexOffset] == null))
        {
            return (" ");
        }

        return (utf8Converter.parsefromSJ(bbsInformationArray[index - categoryIndexOffset].boardName, 0, bbsInformationArray[index - categoryIndexOffset].nameLength));
    }

    /**
     *  ���݂̔�URL���擾����
     * 
     */
    public String getBoardURL(int index)
    {
        int categoryIndex = parent.getCurrentCategoryIndex();
        int categoryIndexOffset = bbsCategoryList[categoryIndex].categoryIndex;
        int categoryIndexMaxOffset = bbsCategoryList[categoryIndex + 1].categoryIndex;
        if ((categoryIndexOffset > index)||(categoryIndexMaxOffset <= index))
        {
            return ("Unknown");
        }
        return (new String(bbsInformationArray[(index - categoryIndexOffset)].boardUrl, 0, bbsInformationArray[(index - categoryIndexOffset)].urlLength));
    }
    
    /**
     * ��Nick����AURL���擾����...
     * 
     * @param boardNick
     * @return
     */
    public String getBoardURL(String boardNick)
    {
        int currentBbsCategoryIndex = parent.getCurrentCategoryIndex();
        String currentBbsNick       = getBoardNickName(getBoardURL(parent.getCurrentBbsIndex()));
        String url                  = "";

        // �J�e�S����擪����S�ăi����B�B�B
        for (int categoryLoop = 0; categoryLoop < bbsCategoryCount; categoryLoop++)
        {
            //
            int bbsIndex = checkBoardNick(categoryLoop, boardNick);
            if (bbsIndex >= 0)
            {
                refreshBoardInformationCache(categoryLoop, false, boardNick);
                int categoryIndexOffset    = bbsCategoryList[categoryLoop].categoryIndex;
                url = new String(bbsInformationArray[(bbsIndex - categoryIndexOffset)].boardUrl, 0, bbsInformationArray[(bbsIndex - categoryIndexOffset)].urlLength);
                break;
            }
        }
        refreshBoardInformationCache(currentBbsCategoryIndex, false, currentBbsNick);
        return (url);
    }

    /**
     * �J�e�S���̐؂�ւ�...
     * 
     * @param checkOnly
     */
    private void clearBoardInformation(boolean checkOnly)
    {
        if (checkOnly == true)
        {
            // �`�F�b�N�����̂Ƃ��ɂ͉������Ȃ�
            return;
        }
        for (int loop = 0; loop < bbsInformationData; loop++)
        {
            bbsInformationArray[loop] = null;
        }
        bbsInformationData = 0;
        
        return;
    }

    /**
     * �����i�[����
     * 
     * @param checkOnly
     * @param buffer
     * @param startOffset
     * @param endSize
     * @param bbsIndex
     * @return
     */
    private int enterBoardInformationArray(boolean checkOnly, String boardNick, byte[] buffer, int startOffset, int endSize, int bbsIndex)
    {
        // URL���o!
        int loop  = startOffset;
        int index = 0;
        while ((index < endSize)&&(buffer[startOffset + index] != '>')&&(buffer[startOffset + index] != 34)&&(buffer[startOffset + index] != 39))
        {
            index++;
        }
        // URL����荞�ށI
        if (index >= endSize)
        {
            // �z��O�̃f�[�^...
            return (-1);
        }

        // ��nick���`�F�b�N���郂�[�h�������ꍇ...
        if (checkOnly == true)
        {
            String url = new String(buffer, startOffset, index);
            String nick = getBoardNickName(url);
            if (nick.equals(boardNick) == true)
            {
                // ��nick�̈�v�����o!!
                url  = null;
                nick = null;
                return (0);
            }
            url  = null;
            nick = null;
            return (-100);
        }

        bbsInformationArray[bbsInformationData] = new bbsDataHolder();
        
        // URL�̒��o (�R�s�[)
        bbsInformationArray[bbsInformationData].setBoardUrl(buffer, startOffset, index);
        loop = loop + index;

        // �A���J�[����΂�...
        while ((loop < (startOffset + endSize))&&(buffer[loop] != '>'))
        {
            loop++;
        }
        loop++;

        // �{�[�h���̂����o����
        index = 0;
        while ((index < endSize)&&(buffer[loop + index] != '<'))
        {
            index++;
        }
        bbsInformationArray[bbsInformationData].boardName = null;
        bbsInformationArray[bbsInformationData].setBoardName(buffer, loop, index);
        loop = loop + index;

        bbsInformationArray[bbsInformationData].boardIndex = bbsIndex;

        if (bbsInformationData < (BBS_MAX_ARRAY - 1))
        {
            bbsInformationData++;
        }
        return (loop);
    }

    /**
     *   ���t�@�C��(bbstabl.html)�̈ꕔ�i�J�e�S�����j�����������ɃL���b�V������
     *
     *   
     */
    public int refreshBoardInformationCache(int categoryIndex, boolean checkOnly, String boardNick)
    {
        FileConnection  bbsFileConnection  = null;
        int          size      = 0;
        InputStream  in        = null;
        byte[]       buffer   = null;
//        int categoryIndex = parent.getCurrentCategoryIndex();
        int dataSize = bbsCategoryList[categoryIndex + 1].fileOffset - bbsCategoryList[categoryIndex].fileOffset;
        if (dataSize < 0)
        {
            // �f�[�^�T�C�Y���ُ�...�I������
            return (-100);
        }
        int bbsIndex = bbsCategoryList[categoryIndex].categoryIndex;
    
        try
        {
            bbsFileConnection = (FileConnection) Connector.open((parent.getBaseDirectory() + FILE_BBSTABLE), Connector.READ);

            size = (int) bbsFileConnection.fileSize();
            if (size < dataSize)
            {
                bbsFileConnection.close();
                return (-101);
            }

            // ����������(���m�Ɍ����ƁA�ꕔ�͎c���Ă���...)����...
            clearBoardInformation(checkOnly);
            
            //////  �f�[�^���ŏ����� dataSize���ǂݏo��...  /////
            in = bbsFileConnection.openInputStream();

            // �f�[�^�o�b�t�@��p�ӂ���
            buffer = new byte[dataSize + MARGIN];
            
            // �t�@�C���̐擪�܂œǂݏo��...
            int startOffset = bbsCategoryList[categoryIndex].fileOffset;
            while (startOffset > 0)
            {
                int readSize = 0;
                if (startOffset > dataSize)
                {
                    readSize = in.read(buffer, 0, dataSize);
                }
                else
                {
                    readSize = in.read(buffer, 0, startOffset);
                }
                startOffset = startOffset - readSize;
            }
            size = in.read(buffer, 0, dataSize);
            in.close();

            bbsFileConnection.close();

            for (int loop = 0; loop < size; loop++)
            {
                if ((buffer[loop] == 'h')&&(buffer[loop + 1] == 't')&&(buffer[loop + 2] == 't')&&(buffer[loop + 3] == 'p'))
                {
                    // �����i�[����...
                    int parsed = enterBoardInformationArray(checkOnly, boardNick, buffer, loop, (size - loop), bbsIndex);
                    if (parsed < 0)
                    {
                        loop++;
                        bbsIndex++;
                        continue;
                    }
                    if (parsed == 0)
                    {
                        // �A�C�e�����o...
                        buffer = null;
                        System.gc();
                        return (bbsIndex);
                    }
                    loop = parsed;
                    bbsIndex++;
                }
            }
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
            try
            {
                if (bbsFileConnection != null)
                {
                    bbsFileConnection.close();
                }
            }
            catch (Exception e2)
            {
            
            }
            return (-102);
        }

        // �K�x�R�����{�B
        System.gc();
        return (-1);
    }

    /**
     *  ��nick�w��ŏ�������ֈړ�����
     * 
     * @param boardUrl
     * @return
     */
    public boolean moveToBoard(String boardNick)
    {
        // �J�e�S����擪����S�ăi����B�B�B
        for (int categoryLoop = 0; categoryLoop < bbsCategoryCount; categoryLoop++)
        {
            //
            int bbsIndex = checkBoardNick(categoryLoop, boardNick);
            if (bbsIndex >= 0)
            {
                // ��ς��I �����������I�I (�J�e�S���ƁA�C���f�b�N�X��ύX����)
                parent.setCurrentBbsCategoryIndex(categoryLoop);
                parent.setCurrentBbsIndex(bbsIndex);
                return (true);
            }
        }
        return (false);
    }
    
    /**
     * ��url���w�肵���J�e�S���Ɋ܂܂�邩��������...
     * @param index
     * @param boardNick
     * @return
     */
    private int checkBoardNick(int index, String boardNick)
    {
        return (refreshBoardInformationCache(index, true, boardNick));
    }

    /**
     *   �J�e�S�����ꗗ�f�[�^�x�[�X�́i�������j�\�z����
     *   
     */
    private void parseBbsDatabase(byte[] buffer, int bufferSize, int offset)
    {
        int entryBbsIndex = 0;
        int loop;

        // �J�e�S���A�{�[�hURL�A�{�[�h���𒊏o����B
        bbsCategoryCount = 0;
        for (loop = 0; loop < bufferSize; loop++)
        {
            // �ꗗDB�쐬�J�n...
            if ((buffer[loop] == (byte) 0x81)&&(buffer[loop + 1] == 121)&&(buffer[loop + 2] == 60)&&
                (buffer[loop + 3] == 66)&&(buffer[loop + 4] == 62))
            {
                // �ꗗ�J�e�S�������I
                loop = loop + 5;
                int index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != 60))
                {
                    index++;
                }

                // �J�e�S����o�^����
                bbsCategoryList[bbsCategoryCount] = new bbsCategoryHolder(entryBbsIndex, (loop + index + 1));
                bbsCategoryList[bbsCategoryCount].setText(buffer, loop, index);
                bbsCategoryCount++;
                if (bbsCategoryCount >= BBS_MAX_CATEGORY)
                {
                    return;
                }
                loop = loop + index;
                continue;
            }
            if ((buffer[loop] == 'h')&&(buffer[loop + 1] == 't')&&(buffer[loop + 2] == 't')&&(buffer[loop + 3] == 'p'))
            {
                // URL���o!
                int index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != '>')&&(buffer[loop + index] != 34)&&(buffer[loop + index] != 39))
                {
                    index++;
                }
                // URL������荞�ށI
                loop = loop + index;
                if (buffer[loop] != '>')
                {
                    // �z��O�̃f�[�^...
                    continue;
                }
                loop++;

                // �{�[�h���̂����o����
                index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != '<'))
                {
                    index++;
                }
                loop = loop + index;

                // ����o�^����B
                if (bbsCategoryCount > 0)
                {
                    bbsCount++;
                    entryBbsIndex++;
                }
                continue;
            }
        }
        return;
    }

    /**
     *   �ꗗ�f�[�^�x�[�X���쐬����...
     *   
     */
    private void updateBbsDatabase()
    {
        FileConnection  bbsFileConnection  = null;
        InputStream     in                 = null;

        String          fileName           = parent.getBaseDirectory() + FILE_BBSTABLE;    
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
//            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXCEPTION  e:" + e.getMessage());
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
            buffer = null;
            return;
        }
        buffer = null;

        // BBS�ꗗ�f�[�^�x�[�X�̏���������
        parent.completedPrepareBbsDatabase();
        return;
    }
    
    /**
     *     �J�e�S������ێ�����N���X
     *
     */
    private class bbsCategoryHolder
    {
        public byte[] categoryName  = null;  // �J�e�S������(�������g�p�ʍ팸�̂���byte[]�ŕۊ�)
        public int    categoryIndex = 0;      // �J�e�S���C���f�b�N�X�ԍ�
        public int    fileOffset    = 0;      // bbstable.html���Ńf�[�^������ʒu (�I�t�Z�b�g(�o�C�g��))

        // �R���X�g���N�^
        public bbsCategoryHolder(int indexNumber, int offset)
        {
            categoryIndex = indexNumber;
            fileOffset    = offset;
        }

        // �J�e�S�����̂̓o�^
        public void setText(byte[] buffer, int offset, int length)
        {
            categoryName = null;
            categoryName = new byte[length + 2];
            System.arraycopy(buffer, offset, categoryName, 0, length);
            categoryName[length] = 0;
            return;
        }
    }

    /**
     *     ����ێ�����N���X
     *
     */
    private class bbsDataHolder
    {
        // ����(�������g�p�ʍ팸�̂���byte[]�ŕۊ�)
        public byte[] boardName = null;
        public int    nameLength = 0;

        // ��URL(�������g�p�ʍ팸�̂���byte[]�ŕۊ�)
        public byte[] boardUrl  = null;
        public int   urlLength = 0;
        
        // �C���f�b�N�X�ԍ�
        public int   boardIndex = 0;

/*
         // �����F �����̊g���C���[�W...
        public final byte BBSTYPE_2ch = 1;     // �^�C�v��2�����˂�
        private byte bbsType = BBSTYPE_2ch;    // ���̔^�C�v
*/

        // �R���X�g���N�^
        public bbsDataHolder()
        {
            // �������Ȃ�...
        }

        /**
         *  ���̂̋L�^ (�ǂݏo���͒��ڍs��)
         * 
         */
        public void setBoardName(byte[] buffer, int offset, int length)
        {
            boardName = null;
            boardName = new byte[length + 2];
            System.arraycopy(buffer, offset, boardName, 0, length);
            boardName[length] = 0;
            nameLength = length;
            return;
        }        

        /**
         *  ��URL�̋L�^ (�ǂݏo���͒��ڍs��)
         * 
         */
        public void setBoardUrl(byte[] buffer, int offset, int length)
        {
            boardUrl = null;
            boardUrl = new byte[length + 2];
            System.arraycopy(buffer, offset, boardUrl, 0, length);
            boardUrl[length] = 0;
            urlLength = length;
            return;
        }        
    }
}

//
//  <<< ���Y�^ >>>
//  ��DB�̍\�� (144bytes �~ ��) �F �t�@�C���ɂ��邩��...
//     - �J�e�S��ID      �F int
//     - ��Nick             : byte[] (16bytes)
//     - ��URL              : byte[] (80bytes)
//     - ����             : byte[] (40bytes)
//     - BBS�^�C�v          : byte
//     - �X�����x��         : byte
//     - �X�����           : byte
//     - �\��               : byte
//
//  �J�e�S��DB�̍\�� (52bytes �~ �J�e�S����)
//     - �J�e�S��ID       : int
//     - �J�e�S������     : byte[] (40bytes)
//     - �e�J�e�S��ID     : int
//     - ��DB�̗L��         : byte
//     - �\��               : byte
//     - �\��               : byte
//     - �\��               : bytes
//
//
//--------------------------------------------------------------------------
//  a2BbbsDatabase  --- BBS�R�~���j�P�[�^...
//
//        - �Ƃ̒ʐM
//        - �ꗗ�̊Ǘ�
//--------------------------------------------------------------------------
//   MRSa (mrsa@sourceforge.jp)
