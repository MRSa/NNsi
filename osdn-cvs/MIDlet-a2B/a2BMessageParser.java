import java.io.InputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;


public class a2BMessageParser
{
    private a2Butf8Conveter      stringConverter = null;

    private boolean convertHanzen   = true;               // ���p�J�i���S�p�J�i�ϊ������{����

    private byte[]            parsedBuffer = null;         // ��͍ς݃��X�f�[�^�i�[�̈�
    private int[]             bufferIndex = null;          // ���X�̃C���f�b�N�X(���X�̐擪�A�h���X)
    private int               numberOfIndex = 0;           // ��͂������X�̐�

    private String            threadBaseTitle = null;      // �X���^�C�̃x�[�X�^�C�g��
    private byte[]            threadMessageTitle = null;  // �X���^�C�̉�͌�^�C�g��
    private int               threadMessageLength = 0;     // �X���^�C�̉�͌�^�C�g������

    private String            readFileName = null;         // �X���t�@�C����
    private long              dataFileSize  = 0;           // �X���t�@�C���̃t�@�C���T�C�Y

    private byte[]            buff = null;                // �X���\���p�f�[�^�o�b�t�@
    private int               currentFileOffset = -1;      // ���ݓǂݍ���ł���X���\���p�f�[�^�o�b�t�@�̐擪�A�h���X
    private int               currentReadSize   = 0;       // ���ݓǂݍ���ł���X���\���p�f�[�^�o�b�t�@�̃f�[�^�T�C�Y
    
    private String             nameField  = "";
    private String             emailField = "";
    //private String             dateTimeField = null;
    //private String             messageField = null;

    // �}�W�b�N�i���o�[
    private int               messageLimit = 1005;            // �X�����b�Z�[�W��͐��̍ő�
    private int               readBufferSize = (10240 * 2);   // �f�[�^�o�b�t�@�̍ő�̈�
    
    private int               separatorSize  = (1024 * 3);    // �e�L�X�g�f�[�^���[�h�̃f�[�^��؂茟���J�n
    private int               separatorLimit = (1024 * 4);    // �e�L�X�g�f�[�^���[�h�̃f�[�^��؂萧��

/**
    // ���p�J�i/�S�p�J�i�ϊ��e�[�u��
    private final short[] Han2ZenTable = {
      // "�B", "�u", "�v", "�A", "�E", "��",
      (short) 0x8142, (short) 0x8175, (short) 0x8176, 
      (short) 0x8141, (short) 0x8145, (short) 0x8392,
      
      // "�@", "�B", "�D", "�F", "�H", "��",
      (short) 0x8340, (short) 0x8342, (short) 0x8344, 
      (short) 0x8346, (short) 0x8348, (short) 0x8383,

      // "��", "��", "�b", "�[", "�A", "�C",
      (short) 0x8385, (short) 0x8387, (short) 0x8362, 
      (short) 0x815b, (short) 0x8341, (short) 0x8343,

      // "�E", "�G", "�I", "�J", "�L", "�N",
      (short) 0x8345, (short) 0x8347, (short) 0x8349, 
      (short) 0x834a, (short) 0x834c, (short) 0x834e,

      // "�P", "�R", "�T", "�V", "�X", "�Z",
      (short) 0x8350, (short) 0x8352, (short) 0x8354, 
      (short) 0x8356, (short) 0x8358, (short) 0x835a,

      // "�\\", "�^", "�`", "�c", "�e", "�g",
      (short) 0x835c, (short) 0x835e, (short) 0x8360, 
      (short) 0x8363, (short) 0x8365, (short) 0x8367,

      // "�i", "�j", "�k", "�l", "�m", "�n",
      (short) 0x8369, (short) 0x836a, (short) 0x836b, 
      (short) 0x836c, (short) 0x836d, (short) 0x836e,

      // "�q", "�t", "�w", "�z", "�}", "�~",
      (short) 0x8371, (short) 0x8374, (short) 0x8377, 
      (short) 0x837a, (short) 0x837d, (short) 0x837e,

      // "��", "��", "��", "��", "��", "��",
      (short) 0x8380, (short) 0x8381, (short) 0x8382, 
      (short) 0x8384, (short) 0x8386, (short) 0x8388,

      // "��", "��", "��", "��", "��", "��",
      (short) 0x8389, (short) 0x838a, (short) 0x838b, 
      (short) 0x838c, (short) 0x838d, (short) 0x838f,

      // "��", "�J", "�K", "",   "",   ""
      (short) 0x8393, (short) 0x814a, (short) 0x814b, 
      (short) 0x0000, (short) 0x0000, (short) 0x0000
    };
**/
    private final int  PARSING_NORMAL      = 0;
    private final int  PARSING_RAW         = 1;
//    private final int  PARSING_HTML        = 2;
    private int        messageParsingMode   = PARSING_NORMAL;
    
    // �R���X�g���N�^
    public a2BMessageParser(a2Butf8Conveter utf8Converter) 
    {
        // ������ϊ��N���X���L������
        stringConverter = utf8Converter;
    }

    // ���b�Z�[�W�̃N���A (���̃^�C�~���O�ŋ����I�ɃK�x�R�������s...)
    public void leaveMessage()
    {
        // �m�ۂ����̈���J������...
        buff = null;
        readFileName = null;
        parsedBuffer = null;
        bufferIndex = null;
        threadBaseTitle = null;
        threadMessageTitle = null;

        // �K�x�R���̎��{
        System.gc();
    }

    // �t�@�C�����̐ݒ菈��
    public void setFileName(String fileName)
    {
        // �t�@�C������ݒ肷��
        readFileName = fileName;        
    }

    // �t�@�C�������擾����
    public String getFileName()
    {
        return (readFileName);
    }

    // �t�@�C���T�C�Y����������
    public long getFileSize()
    {
        return (dataFileSize);
    }
    
    // ���p�J�i -> �S�p�J�i�̕ϊ������{���邩�ǂ���
    public void setConvertHanZen(boolean setting)
    {
        convertHanzen = setting;
        return;
    }

    // �e�L�X�g�p�[�X���[�h���ǂ����̐ݒ�
    public void setMessageParsingMode(int parseMode)
    {
        messageParsingMode   = parseMode;
        return;
    }
    
    // ���b�Z�[�W��͂̏���
    public boolean prepareMessage()
    {
        // �t�@�C�������ݒ肳��Ă��Ȃ���΃G���[
        if (readFileName.length() == 0)
        {
            return (false);
        }
        FileConnection     dataFileConnection = null;

        // �ǂݍ��ݗp�f�[�^�o�b�t�@�̊m��
        buff = new byte[readBufferSize + 60];

        // �X���C���f�b�N�X�i�[�̈�̊m��
        bufferIndex = new int[(messageLimit + 1)];
        for (int loop = 0; loop <= messageLimit; loop++)
        {
            bufferIndex[loop] = 0;
        }
        numberOfIndex = 0;

        // �t�@�C���̃I�[�v���A�t�@�C���T�C�Y�̓���...
        dataFileSize = 0;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(readFileName, Connector.READ);
            dataFileSize = dataFileConnection.fileSize();
        }
        catch (Exception e)
        {
            // �t�@�C���T�C�Y�̓��莸�s...
            return (false);
        }

        // ���O�������Email����������
        nameField = "";        
        emailField = "";

        // �������ƃK�x�R�����{...
        System.gc();

        // �t�@�C�����ɂ���X���f�[�^�̃��R�[�h�C���f�b�N�X���쐬����
        InputStream  in = null;
        try
        {
            boolean firstTime = true;
            int offset = 0;
            int readSize = 1;
            in = dataFileConnection.openInputStream();
            while (readSize > 0)
            {
                readSize = in.read(buff, 0, readBufferSize);
                if (readSize > 0)
                {
                    if (messageParsingMode != PARSING_NORMAL)
                    {
                        // HTML/TEXT�p�̃X���C���f�b�N�X�����B�B�B
                        createThreadIndexSub_TextParsing(offset, buff, readSize);                        
                    }
                    else
                    {
                        // �ʏ�̃X���C���f�b�N�X�����B�B�B
                        createThreadIndexSub(offset, buff, readSize);
                    }
                    offset = offset + readSize;

                    // ���񂾂����ꍇ�ɂ́A�X���^�C�g���𒊏o����
                    if (firstTime == true)
                    {
                        pickupThreadTitle(buff, readSize);
                        firstTime = false;
                    }
                }
            }
            in.close();
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

            // �t�@�C�����I�[�v�������ςȂ���������N���[�Y����
            try
            {
                if (dataFileConnection.isOpen() == true)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception e3)
            {
                // ��O�����������Ƃ��͉������Ȃ�
            }
            // �Ȃ񂾂���͂Ɏ��s...
            return (false);
        }

        // �t�@�C���f�[�^�͂܂��ǂݍ��܂�Ă��Ȃ���Ԃɂ���
        currentFileOffset = -1;
        currentReadSize = 0;

        // �t�@�C�����I�[�v�������ςȂ���������N���[�Y����
        try
        {
            if (dataFileConnection.isOpen() == true)
            {
                dataFileConnection.close();
            }
            dataFileConnection = null;
        }
        catch (Exception e)
        {
            // ��O�����������Ƃ��͉������Ȃ�
        }

        return (true);
    }

    // ���X���̍ő吔����������
    public int getNumberOfMessages()
    {
        return (numberOfIndex);        
    }

    // �X���^�C�̐ݒ菈��
    public String decideThreadTitle()
    {
        if (threadMessageTitle != null)
        {
//            threadBaseTitle = new String(threadMessageTitle);
            threadBaseTitle = stringConverter.parsefromSJ(threadMessageTitle);
        }
        return ("[1:" + numberOfIndex + "] " + threadBaseTitle);
    }

    // �X���^�C�g�����w�肳�ꂽ�ꍇ...
    public void setThreadBaseTitle(String title)
    {
        threadBaseTitle = title;
        return;
    }

    /**
     * �X���^�C�g������������
     * 
     * @return
     */
    public String getTitle()
    {
        return (threadBaseTitle);
    }

    // ���X�f�[�^���擾����
    public byte[] getMessage(int number, int kanjiMode)
    {
        parsedBuffer = null;
        System.gc();

        // �擾����悤�w�肳�ꂽ���X�ԍ����m�F���A�͈͊O�Ȃ�\�����Ȃ�
        if ((numberOfIndex < 1)||(number < 1)||(number > numberOfIndex))
        {
            parsedBuffer = new byte[4];
            parsedBuffer[0] = '-';
            parsedBuffer[1] = '-';
            parsedBuffer[2] = '-';
            parsedBuffer[3] = '\0';
            return (parsedBuffer);
        }

        int offset = 0;
        int parseSize = 0;
        if (number == 1)
        {
            offset = 0;
            parseSize = bufferIndex[0];
        }
        else
        {
            offset = bufferIndex[(number - 2)];
            parseSize = bufferIndex[(number - 1)] - bufferIndex[(number - 2)];
        }

        // ���X�f�[�^�����ݓǂݍ���ł���f�[�^�o�b�t�@���Ɋ܂܂�Ă��Ȃ�...
        if ((offset < currentFileOffset)||((offset + parseSize) > (currentFileOffset + currentReadSize)))
        {
            // �f�[�^�t�@�C����ǂݏo��...
            if (loadFromFile(offset, parseSize) != true)
            {
                parsedBuffer = new byte[4];
                parsedBuffer[0] = 'x';
                parsedBuffer[1] = 'x';
                parsedBuffer[2] = 'x';
                parsedBuffer[3] = '\0';
                return (parsedBuffer);
            }
        }

        // �t�@�C�����r���Ő؂�Ă����ꍇ...
        if ((offset + parseSize) > (currentFileOffset + currentReadSize))
        {
            parseSize = (currentFileOffset + currentReadSize) - offset;
        }
        
        // �I�t�Z�b�g�𒲐�����
        if (offset >= currentFileOffset)
        {
            offset = offset - currentFileOffset;
        }
        
        // �f�[�^�����H����(RAW���[�h)
        if (messageParsingMode == PARSING_RAW)
        {
            if (kanjiMode == a2BsubjectDataParser.PARSE_EUC)
            {
            	// EUC �������[�h (EUC����SJ�֕ϊ�����)
                parsedBuffer = new byte[(parseSize * 2) + 60];
                stringConverter.StrCopyEUCtoSJ(parsedBuffer, 0, buff, offset, parseSize);
                return (parsedBuffer);
            }
            parsedBuffer = new byte[parseSize + 2];
            System.arraycopy(buff, offset, parsedBuffer, 0, parseSize);
            return (parsedBuffer);
        }
        
        // �f�[�^�����H����(�ʏ�/HTML��̓��[�h)
        byte[] originBuffer = null;
        if (kanjiMode == a2BsubjectDataParser.PARSE_EUC)
        {
        	// EUC �������[�h (EUC����SJ�֕ϊ�����)
            originBuffer = new byte[(parseSize * 2) + 60];
            stringConverter.StrCopyEUCtoSJ(originBuffer, 0, buff, offset, parseSize);
            parsedBuffer = new byte[(parseSize * 2) + 60];
            parseMessage(parsedBuffer, originBuffer, 0, parseSize, number);
            originBuffer = null;
            return (parsedBuffer);
        }
        // Shift JIS�������[�h or UTF8�������[�h
        parsedBuffer = new byte[(parseSize * 2) + 60];
        parseMessage(parsedBuffer, buff, offset, parseSize, number);
        return (parsedBuffer);
    }    

    // ����̃��X���Ɋ܂܂��A���J�[�����擾����
    public int getNumberOfAnchors(int number)
    {
        return (0);        
    }
    
    // ���X�Ɋ܂܂��A���J�[�ԍ����擾����
    public int getAnchorNumber(int index)
    {
        return (0);
    }

    // ���ځ[����{
    public void doAbone()
    {
        nameField  = "* * *";
        emailField = "";
    }

    // ���O�����擾
    public String getNameField()
    {
        return (nameField);
    }

    // eMail�����擾
    public String getEmailField()
    {
        return (emailField);
    }

    /*
     *   �X���^�C�g�����擾����
     * 
     */    
    public byte[] getThreadTitle()
    {
        return (threadMessageTitle);
    }

    /*
     *   �X���^�C�g�������擾����
     * 
     */
    public int getThreadTitleLength()
    {
        return (threadMessageLength);
    }
    
    /*
     *   �X���^�C�g���𒊏o����
     * 
     */
    private void pickupThreadTitle(byte[] buff, int readSize)
    {
        // �C���f�b�N�X���Ȃ������ꍇ�ɂ͒��o���Ȃ�
        if (numberOfIndex < 1)
        {
            return;
        }
        
        for (int index = bufferIndex[0]; index > 0; index--)
        {
            if (buff[index] == '>')
            {
                index++;
                int length = (bufferIndex[0] - index);
                
                threadMessageTitle = new byte[length + 4];
                int loopCnt = 0;
                while (loopCnt < length)
                {
                    threadMessageTitle[loopCnt] = buff[index];
                    index++;
                    loopCnt++;
                    if (buff[index] == 0x0a)
                    {
                        // ���s�R�[�h�ŋ�؂�...
                        break;
                    }
                }
                threadMessageTitle[length] = 0;
                threadMessageLength = loopCnt;
                index = -1;
                break;
            }
        }
        return;
    }

    // �t�@�C������f�[�^��ǂݏo��...
    private boolean loadFromFile(int offset, int parseSize)
    {
        FileConnection  dataFileConnection = null;
        int start = 0;
        if (((offset + parseSize) < readBufferSize)||
             (offset < (readBufferSize / 2))||
             (dataFileSize < readBufferSize))
        {
            // �f�[�^�̐擪����ǂݏo��...
            start = 0;
        }
        else if (offset > (dataFileSize - readBufferSize))
        {
            // �f�[�^�̖���������ǂݏo��...
            start = (int) dataFileSize - readBufferSize;
        }
        else
        {
            // �f�[�^�̐^�񒆂�ǂݏo��...
            start = offset - (readBufferSize / 2);
        }

        // �t�@�C���̃I�[�v���A�t�@�C���T�C�Y�̓���...
        try
        {
            dataFileConnection = (FileConnection) Connector.open(readFileName, Connector.READ);
        }
        catch (Exception e)
        {
            // �t�@�C���I�[�v�����s...
            System.gc();
            return (false);
        }

        //////  �f�[�^��start���� readBufferSize���ǂݏo��...  /////
        InputStream  in = null;
        try
        {
            in = dataFileConnection.openInputStream();

            // �t�@�C���̐擪�܂œǂݏo��...
            if (start != 0)
            {
                //in.skip(start);

                // in.skip(start)�̏��������܂�ɂ��x�������̂ŁA�A�A�J���ǂݏ����B                
                int startOffset = start;
                while (startOffset > 0)
                {
                    int readSize = 0;
                    if (startOffset > readBufferSize)
                    {
                        readSize = in.read(buff, 0, readBufferSize);
                    }
                    else
                    {
                        readSize = in.read(buff, 0, startOffset);
                    }
                    startOffset = startOffset - readSize;
                }
            }
            currentReadSize = in.read(buff, 0, readBufferSize);
            in.close();
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

            // �Ȃ񂾂���͂Ɏ��s...
            currentReadSize = 0;

            // �t�@�C�����I�[�v�������ςȂ���������N���[�Y����
            try
            {
                if (dataFileConnection.isOpen() == true)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception ez)
            {
                // ��O�����������Ƃ��͉������Ȃ�
            }
            System.gc();
            return (false);
        }

        // �t�@�C���̐擪��ݒ肷��
        currentFileOffset = start;

        // �t�@�C�����I�[�v�������ςȂ���������N���[�Y����
        try
        {
            if (dataFileConnection.isOpen() == true)
            {
                dataFileConnection.close();
            }
            dataFileConnection = null;
        }
        catch (Exception e)
        {
            // ��O�����������Ƃ��͉������Ȃ�
        }
        System.gc();
        return (true);
    }
    
    /*=========================================================================*/
    /*   Function : ParseMessage�@�@                                           */
    /*                                       (���b�Z�[�W�p�[�X����...NNsi���) */
    /*=========================================================================*/
    private void parseMessage(byte[] dst, byte[] src, int offset, int size, int number)
    {
        int dataStatus = 0;
        int dstIndex  = 0;
        int srcIndex  = offset;
        int previousIndex = 0;

        // ���x�����߃��[�`������... (�P�����Âp�[�X����)
        while (srcIndex < (offset + size))
        {
            if (src[srcIndex] == '&')
            {
                // "&gt;" �� '>' �ɒu��
                if ((src[srcIndex + 1]== 'g')&&(src[srcIndex + 2] == 't')&&(src[srcIndex + 3] == ';'))
                {
                    dst[dstIndex] = '>';
                    dstIndex++;
                    srcIndex = srcIndex + 4;

                    // TODO �����Ƀ��X�Q�Ɣԍ���pick up ���W�b�N������
                    continue;
                }

                // "&lt;" �� '<' �ɒu��
                if ((src[srcIndex + 1]== 'l')&&(src[srcIndex + 2] == 't')&&(src[srcIndex + 3] == ';'))
                {
                    dst[dstIndex] = '<';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }

                // "&quot;" �� '"' �ɒu��
                if ((src[srcIndex + 1] == 'q')&&(src[srcIndex + 2] == 'u')&&(src[srcIndex + 3] == 'o')&&
                    (src[srcIndex + 4] == 't')&&(src[srcIndex + 5] == ';'))
                {
                    dst[dstIndex] = '"';
                    dstIndex++;
                    srcIndex = srcIndex + 6;
                    continue;
                }

                // "&nbsp;" �� '    ' �ɒu�� (AA�\�����[�h�̂��߂ɁA�X�y�[�X1�ɂ���悤�ύX)
                if ((src[srcIndex + 1] == 'n')&&(src[srcIndex + 2] == 'b')&&(src[srcIndex + 3] == 's')&&
                        (src[srcIndex + 4] == 'p')&&(src[srcIndex + 5] == ';'))
                {
                        dst[dstIndex] = ' ';
                        dstIndex++;
/*
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        dst[dstIndex] = ' ';
                        dstIndex++;
*/
                        srcIndex = srcIndex + 6;
                        continue;
                }

                // "&amp;" �� '&' �ɒu��
                if ((src[srcIndex + 1] == 'a')&&(src[srcIndex + 2] == 'm')&&(src[srcIndex + 3] == 'p')&&
                    (src[srcIndex + 4] == ';'))
                {
                    dst[dstIndex] = '&';
                    dstIndex++;
                    srcIndex = srcIndex + 5;
                    continue;
                }
                // "&alpha;" �� '��' �ɒu��
                if ((src[srcIndex + 1] == 'a')&&(src[srcIndex + 2] == 'l')&&
                    (src[srcIndex + 3] == 'p')&&(src[srcIndex + 4] == 'h')&&
                    (src[srcIndex + 5] == 'a')&&(src[srcIndex + 6] == ';'))
                {
                    dst[dstIndex] = (byte) 0x83;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xbf;
                    dstIndex++;
                    srcIndex = srcIndex + 7;    // StrLen(TAG_ALPHA);
                    continue;
                }
                 // "&beta;" �� '��' �ɒu��
                if ((src[srcIndex + 1] == 'b')&&(src[srcIndex + 2] == 'e')&&
                    (src[srcIndex + 3] == 't')&&(src[srcIndex + 4] == 'a')&&
                    (src[srcIndex + 5] == ';'))
                {
                    dst[dstIndex] = (byte) 0x83;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xc0;
                    dstIndex++;
                    srcIndex = srcIndex + 6;    // StrLen(TAG_BETA);
                    continue;
                }
                // "&hellip;" �� '���' �ɒu��
                if ((src[srcIndex + 1] == 'h')&&(src[srcIndex + 2] == 'e')&&
                    (src[srcIndex + 3] == 'l')&&(src[srcIndex + 4] == 'l')&&
                    (src[srcIndex + 5] == 'i')&&(src[srcIndex + 6] == 'p')&&
                    (src[srcIndex + 7] == ';'))
                {
                    dst[dstIndex] = (byte) 0x81;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0x63;
                    dstIndex++;
                    srcIndex = srcIndex + 8;    // StrLen(TAG_...)
                    continue;
                }
                // "&rarr;" �� '��' �ɒu��
                if ((src[srcIndex + 1] == 'r')&&(src[srcIndex + 2] == 'a')&&
                    (src[srcIndex + 3] == 'r')&&(src[srcIndex + 4] == 'r')&&
                    (src[srcIndex + 5] == ';'))
                {
                    dst[dstIndex] = (byte) 0x81;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xa8;
                    dstIndex++;
                    srcIndex = srcIndex + 6;    // StrLen(��);
                    continue;
                }
                // "&acute;" �� '�L' �ɒu��
                if ((src[srcIndex + 1] == 'a')&&(src[srcIndex + 2] == 'c')&&
                    (src[srcIndex + 3] == 'u')&&(src[srcIndex + 4] == 't')&&
                    (src[srcIndex + 5] == 'e')&&(src[srcIndex + 6] == ';'))
                {
                    dst[dstIndex] = (byte) 0x81;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0x4c;
                    dstIndex++;
                    srcIndex = srcIndex + 7;    // StrLen(�L);
                    continue;
                }
                 // "&omega;" �� '��' �ɒu��
                if ((src[srcIndex + 1] == 'o')&&(src[srcIndex + 2] == 'm')&&
                    (src[srcIndex + 3] == 'e')&&(src[srcIndex + 4] == 'g')&&
                    (src[srcIndex + 5] == 'a')&&(src[srcIndex + 6] == ';'))
                {
                    dst[dstIndex] = (byte) 0x83;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xd6;
                    dstIndex++;
                    srcIndex = srcIndex + 7;    // StrLen(�L);
                    continue;
                }
                 // "&rdquo;" �� '�h' �ɒu��
                if ((src[srcIndex + 1] == 'r')&&(src[srcIndex + 2] == 'd')&&
                    (src[srcIndex + 3] == 'q')&&(src[srcIndex + 4] == 'u')&&
                    (src[srcIndex + 5] == 'o')&&(src[srcIndex + 6] == ';'))
                {
                    dst[dstIndex] = (byte) 0x81;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0x68;
                    dstIndex++;
                    srcIndex = srcIndex + 7;    // StrLen(�L);
                    continue;
                }
                //  "&Pi;" �� '��' �ɒu��
                if ((src[srcIndex + 1] == 'P')&&(src[srcIndex + 2] == 'i')&&
                    (src[srcIndex + 3] == ';'))
                {
                    dst[dstIndex] = (byte) 0x83;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xae;
                    dstIndex++;
                    srcIndex = srcIndex + 4;    // StrLen(�L);
                    continue;
                }

                // ����͂��肦�Ȃ��͂�����...�ꉞ�B
                dst[dstIndex] = '&';
                dstIndex++;
                srcIndex++;
                continue;
            }
            if (src[srcIndex] == '<')
            {
                //  "<>" �́A�Z�p���[�^(��Ԃɂ���ĕς��)
                if (src[srcIndex + 1] == '>')
                {
                    srcIndex = srcIndex + 2;
                    if (messageParsingMode != PARSING_NORMAL)
                    {
                        // �X����̓��[�h�́A�X�y�[�X�P�ɕϊ�����...
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        continue;
                    }
                    switch (dataStatus)
                    {
                      case 0:
                        // ���O���̋�؂�
                        nameField = stringConverter.parsefromSJ(dst, previousIndex, (dstIndex - previousIndex));
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        dataStatus = 1;
                        previousIndex = dstIndex;
                        break;

                      case 1:
                        // e-mail���̋�؂�
                        emailField = stringConverter.parsefromSJ(dst, previousIndex, (dstIndex - previousIndex));
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                        dataStatus = 2;
                        previousIndex = dstIndex;
                        break;

                      case 2:
                        // �N�����E���Ԃ���тh�c���̋�؂�
                        //dateTimeField = new String(dst, previousIndex, (dstIndex - previousIndex));
                        //dst[dstIndex] = '\n';
                        //dstIndex++;
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                        dataStatus = 3;
                        previousIndex = dstIndex;

                        // �擪�ɂ���X�y�[�X...
                        if (src[srcIndex] == ' ')
                        {
                            srcIndex++;  // �� ���X�̐擪�ɓ����Ă���X�y�[�X����������
                        }
                        break;

                      case 3:
                      default:
                        // ���b�Z�[�W�̏I�[
                        //messageField = new String(dst, previousIndex, (dstIndex - previousIndex));
                        dst[dstIndex] = '\0';
                        dstIndex++;
                        //previousIndex = dstIndex;
                        return;
                        //break;
                    }
                    continue;
                }

                //  "<br>" �́A���s�ɒu��
                if (((src[srcIndex + 1] == 'b')||(src[srcIndex + 1] == 'B'))&&
                    ((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    // �s���ƍs���̃X�y�[�X������Ă݂�ꍇ
                    if ((srcIndex != 0)&&((srcIndex -1) == ' '))
                    {
                        dstIndex--;
                    }
                    if (src[srcIndex + 4] == ' ')
                    {
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 5;
                        continue;
                    }
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }

                //  "<p*>" �́A���s2�ɒu��
                if ((src[srcIndex + 1] == 'p')||(src[srcIndex + 1] == 'P'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 2;
                    while (src[srcIndex] != '>')
                    {
                        srcIndex++;
                        if (srcIndex >= (offset + size))
                        {
                            // �m�ۂ����̈���z����...
                            break;
                        }
                    }
                    srcIndex++;
                    continue;
                }

                // <li>�^�O�����s�R�[�h�� - �ɒu������
                if (((src[srcIndex + 1] == 'l')||(src[srcIndex + 1] == 'L'))&&
                    ((src[srcIndex + 2] == 'i')||(src[srcIndex + 2] == 'I'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '-';
                    dstIndex++;
                    dst[dstIndex] = ' ';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }
                   
                //  "<hr>" �́A���s === ���s �ɒu��
                if (((src[srcIndex + 1] == 'h')||(src[srcIndex + 1] == 'H'))&&
                    ((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '=';
                    dstIndex++;
                    dst[dstIndex] = '=';
                    dstIndex++;
                    dst[dstIndex] = '=';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }

                //  "<dt>" �́A���s�ɒu��
                if (((src[srcIndex + 1] == 'd')||(src[srcIndex + 1] == 'D'))&&
                    ((src[srcIndex + 2] == 't')||(src[srcIndex + 2] == 'T'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    if (dst[dstIndex - 1] != '\n')
                    {
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                    }
                    srcIndex = srcIndex + 4;
                    continue;
                }

                //  "</tr>" ����� "</td>" �́A���s�ɒu��
                if (((src[srcIndex + 2] == 't')||(src[srcIndex + 2] == 'T'))&&
                    ((src[srcIndex + 3] == 'r')||(src[srcIndex + 3] == 'R'))||
                    ((src[srcIndex + 3] == 'd')||(src[srcIndex + 3] == 'D'))&&
                    (src[srcIndex + 1] == '/')&&(src[srcIndex + 4] == '>'))
                {
                    if (dst[dstIndex - 1] != '\n')
                    {
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                    }
                    srcIndex = srcIndex + 5;
                    continue;
                }

                //  "<dd>" �́A���s�Ƌ�2�ɒu��
                if (((src[srcIndex + 1] == 'd')||(src[srcIndex + 1] == 'D'))&&
                    ((src[srcIndex + 2] == 'd')||(src[srcIndex + 2] == 'D'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = ' ';
                    dstIndex++;
                    dst[dstIndex] = ' ';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }
                
                //  "<h*>" �́A���s + ���s �ɒu��
                if ((src[srcIndex + 1] == 'h')||(src[srcIndex + 1] == 'H'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;

                    // TODO: �����F�̐ݒ�...

                    // �^�O�͓ǂݔ�΂�
                    srcIndex = srcIndex + 2;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                //  "</h*>" �́A���s + ���s �ɒu��
                if ((src[srcIndex + 1] == '/')&&((src[srcIndex + 2] == 'h')||(src[srcIndex + 2] == 'H')))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;

                    // TODO: �����F�̉���...

                    // �^�O�͓ǂݔ�΂�
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                //  "<a*>" �ŁA�A���J�[�F�̌��o
                if (((src[srcIndex + 1] == 'a')||(src[srcIndex + 1] == 'A'))&&(src[srcIndex + 2] == ' '))
                {
                    // TODO: �A���J�[�F�̐ݒ�...

                    // �^�O�͓ǂݔ�΂�
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                //  "</a*>" �ŁA�A���J�[�F�̉���
                if (((src[srcIndex + 2] == 'a')||(src[srcIndex + 2] == 'A'))&&(src[srcIndex + 1] == '/'))
                {
                    // TODO: �A���J�[�F�̉���

                    // �^�O�͓ǂݔ�΂�
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                     continue;
                }

                // <EM* �͋����F��
                if (((src[srcIndex + 1] == 'e')||(src[srcIndex + 1] == 'E'))&&
                    ((src[srcIndex + 2] == 'm')||(src[srcIndex + 2] == 'M')))
                {
                    // TODO: �����F�֐؂�ւ�

                    // �^�O�͓ǂݔ�΂�
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                // </em* �͒ʏ�F��
                if (((src[srcIndex + 2] == 'e')||(src[srcIndex + 2] == 'E'))&&
                    ((src[srcIndex + 3] == 'm')||(src[srcIndex + 3] == 'M'))&&
                    (src[srcIndex + 1] == '/'))
                {
                    // TODO: �����F�̉���

                    // �^�O�͓ǂݔ�΂�
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                // <tr>, <td> �̓X�y�[�X�P�ɕϊ�
                if (((src[srcIndex + 1] == 't')||(src[srcIndex + 1] == 'T'))&&
                    ((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))||
                    ((src[srcIndex + 2] == 'd')||(src[srcIndex + 2] == 'D')))
                {
                    dst[dstIndex] = ' ';
                    dstIndex++;

                    // �^�O�͓ǂݔ�΂�
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                // <BE: �́ABE ID�̂��ߓǂݔ�΂��Ȃ��B(��ʕ\������)
                if (((src[srcIndex + 1] == 'b')||(src[srcIndex + 1] == 'B'))&&
                    ((src[srcIndex + 2] == 'e')||(src[srcIndex + 2] == 'E'))&&
                    (src[srcIndex + 3] == ':'))
                {
                    dst[dstIndex] = ' ';
                    dstIndex++;
                    srcIndex++;

                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        dst[dstIndex] = src[srcIndex];
                        dstIndex++;
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                // ���̑��̃^�O�͓ǂݔ�΂�
                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                {
                    srcIndex++;
                }
                // �^�O�͊��S�����Ƃ���
                //dst[dstIndex] = ' ';
                //dstIndex++;
                srcIndex++;
                continue;
            }

            // ���p�J�i���S�p�J�i�ϊ����������{����ꍇ...
            if ((convertHanzen == true)&&(src[srcIndex] < 0))
            {
                int checkChar = 256 + src[srcIndex];
                if ((checkChar >= 0x81)&&(checkChar <= 0x9f))
                {
                    // 2�o�C�g�����Ɣ���
                    dst[dstIndex] = src[srcIndex];
                    dstIndex++;
                    srcIndex++;

                    dst[dstIndex] = src[srcIndex];
                    dstIndex++;
                    srcIndex++;
                    continue;
                }

                if ((checkChar >= 0xe0)&&(checkChar <= 0xef))
                {
                    // 2�o�C�g�����Ɣ���
                    dst[dstIndex] = src[srcIndex];
                    dstIndex++;
                    srcIndex++;

                    dst[dstIndex] = src[srcIndex];
                    dstIndex++;
                    srcIndex++;
                    continue;
                }
                // ���p�J�i �� �S�p�J�i�ϊ��𖳌�������B
/**
                if ((checkChar >= 0xa1)&&(checkChar <= 0xdf))
                {
                    // �J�^�J�i�����Ɣ���A�e�[�u�����g���Ĕ��p�J�i���S�p�ϊ�����
                    short code = Han2ZenTable[checkChar - 0xa1];
                    byte temp;

                    temp = (byte) ((code & 0xff00) >> 8);
                    dst[dstIndex] = temp;
                    dstIndex++;

                    temp = (byte) ((code & 0x00ff));
                    dst[dstIndex] = temp;
                    dstIndex++;
                                        
                    srcIndex++;
                    continue;
                }
**/
            }

            if ((messageParsingMode != PARSING_NORMAL)&&((src[srcIndex] == 0x0d)||(src[srcIndex] == 0x0a)))
            {
                // �e�L�X�g�\�����[�h�̂Ƃ��ɂ́A���s�R�[�h�𕁒ʂ̈ꕶ���]�ʂ���B
                srcIndex++;
                dst[dstIndex] = '\r';
                dstIndex++;
                dst[dstIndex] = '\n';
                dstIndex++;
                continue;
            }
            
            // �X�y�[�X���A�����Ă����ꍇ�A�P�Ɍ��炷
            if ((src[srcIndex] == ' ')&&(src[srcIndex + 1] == ' '))
            {
                srcIndex++;
                while ((src[srcIndex] == ' ')&&(srcIndex < (offset + size)))
                {
                    srcIndex++;
                }
                srcIndex--;
            }

            // NULL ����� 0x0a, 0x0d �͖�������
            if ((src[srcIndex] != '\0')&&(src[srcIndex] != 0x0a)&&(src[srcIndex] != 0x0d)&&(src[srcIndex] != 0x09))
            {
                // ���ʂ̈ꕶ���]��
                dst[dstIndex] = src[srcIndex];
                dstIndex++;
            }
            srcIndex++;
        }
        //  ���X�̏I�[...
        //messageField = new String(dst, previousIndex, (dstIndex - previousIndex));
        dst[dstIndex] = '\0';
        dstIndex++;
        return;
    }

    /*=========================================================================*/
    /*   Function : CreateThreadIndexSub                                       */
    /*                                                                         */
    /*                                   (�C���f�b�N�X�擾�̎�����...NNsi���) */
    /*=========================================================================*/
    private void createThreadIndexSub(int offset, byte[] buffer, int bufferLength)
    {

        // ���݂̃��b�Z�[�W�����擾����
        int index = 0;
        while (index < bufferLength)
        {
            // ���b�Z�[�W�̋�؂��T��
            while ((buffer[index] != '\n')&&(buffer[index] != '\0'))
            {
                index++;
            }
            if (buffer[index] != '\0')
            {
                index++;
                if (numberOfIndex < messageLimit)
                {
                    bufferIndex[numberOfIndex] = (offset + index);
                    numberOfIndex++;
                }
                else
                {
                    // �i�[�G���A�s��...����������������
                    return;
                }
            }
            else
            {
                // ��؂肪�Ȃ�(�f�[�^�̖����H ���Ă���H)�A�Ƃ肠�������ɂ�����
                index++;
            }
        }
        // �Ƃ肠�����A�����ɖ���������
        bufferIndex[numberOfIndex] = (offset + index);
        return;
    }

    /*=========================================================================*/
    /*   Function : CreateThreadIndexSub                                       */
    /*                                                                         */
    /*                                   (�C���f�b�N�X�擾�̎�����...NNsi���) */
    /*=========================================================================*/
    private void createThreadIndexSub_TextParsing(int offset, byte[] buffer, int bufferLength)
    {
        // ���݂̃��b�Z�[�W�����擾����
        int index = 0;
        while (index < bufferLength)
        {
            int before = index;
            index = index + separatorSize;

            // ���b�Z�[�W�̋�؂��T�� (�Ȃ����limit�Ő؂�)
            while ((index < (before + separatorLimit))&&
                    (index < bufferLength)&&
                    (buffer[index] != '\n')&&
                    (buffer[index] != '\r')&&
                    (buffer[index] != '\0'))
            {
                index++;
            }
            if ((index < bufferLength)&&(buffer[index] != '\0'))
            {
                index++;
                if (numberOfIndex < messageLimit)
                {
                    bufferIndex[numberOfIndex] = (offset + index);
                    numberOfIndex++;
                }
                else
                {
                    // �i�[�G���A�s��...����������������
                    return;
                }
            }
        }

        // �Ƃ肠�����A�����ɖ���������
        bufferIndex[numberOfIndex] = (offset + index);
        numberOfIndex++;
        return;
    }
}

//--------------------------------------------------------------------------
//   a2BMessageParser  --- �X�����b�Z�[�W�p�[�T 
//                                        (dat�t�@�C���̉�͂ƃf�[�^�ێ�)
//
//
//--------------------------------------------------------------------------
//   MRSa (mrsa@sourceforge.jp)
