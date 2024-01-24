import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;
import java.util.Date;

public class a2BsubjectDataParser
{
    // �����R�[�h�w��...
    static final int          PARSE_JIS       = 4;
    static final int          PARSE_UTF8      = 3;
    static final int          PARSE_EUC       = 2;
    static final int          PARSE_SJIS      = 1;    
    static final int          PARSE_2chMSG    = 0;
    static final int          PARSE_TOGGLE    = -1;

    // �֘A�I�u�W�F�N�g
    private a2BMain            parent          = null;
    private a2BbbsCommunicator bbsCommunicator = null;
    private a2Butf8Conveter    utf8Converter   = null;
    private a2BFavoriteManager favorManager    = null;

    // �X���Ǘ��p�f�[�^
    private Hashtable          subjectMap            = null;
    private Vector             subjectVector         = null;
    private int               subjectListCount      = 0;

    private byte[]            subjectTxt            = null;
    private int               subjectTxtSize        = 0;

    private int[]             subjectTxtOffsetArray = null;
    private int               subjectTitles         = 0;

    private boolean          useOnlyDatabase       = false;
    private boolean          dataExtracted         = false;
    private int               favoriteLevel         = -1;

    // �X����Ԃ̒�`�B
    static public  final byte       STATUS_NOTYETOLD  = 0;   // ���擾
    static public  final byte       STATUS_NEW        = 1;   // �V�K�擾
    static public  final byte       STATUS_UPDATE     = 2;   // �X�V�擾
    static public  final byte       STATUS_REMAIN     = 3;   // ���ǂ���
    static public  final byte       STATUS_ALREADY    = 4;   // ����
    static public  final byte       STATUS_RESERVE    = 5;   // �擾�\��
    static public  final byte       STATUS_UNKNOWN    = 6;   // �X����ԕs��
    static public  final byte       STATUS_GETERROR   = 7;
    static public  final byte       STATUS_OVER       = 8;   // 1000���X��
    static public  final byte       STATUS_NOTYET     = 10;  // ���擾
    static public  final byte       STATUS_DEL_OFFL   = 20;
    static public  final byte       STATUS_NOTSPECIFY = 127; // �X�e�[�^�X����`

    // �o�b�t�@�̃}�[�W���̈�...
    private final int        readBufferSize    = 10240;  // ���ǂݗp�̃o�b�t�@�T�C�Y
    private final int        bufferMargin      = 8;
//    private final int        subjectManageSize = 3000;  // �X���Ǘ��\�̍ő�T�C�Y
    private final int        subjectManageSize = 1200;  // �X���Ǘ��\�̍ő�T�C�Y
    private final int        indexReadSize     = 120;   // �C���f�b�N�X�ǂݏo���T�C�Y

    // ���[�N�̈�...
    private String            gettingLogDataFile = null;
    private String            gettingLogDataURL  = null;
    private String            deleteLogDataFile  = null;
    private String            boardNick          = null;

    /*
     *   �R���X�g���N�^
     *   
     */
    public a2BsubjectDataParser(a2BMain object, a2BbbsCommunicator communicator, a2Butf8Conveter converter, a2BFavoriteManager favor)
    {
        parent                = object;
        bbsCommunicator       = communicator;
        favorManager          = favor;

        utf8Converter         = converter;
        subjectMap            = new Hashtable();
        subjectVector         = new Vector();
        subjectTxtOffsetArray = new int[subjectManageSize + bufferMargin];        
    }

    /**
     * �X���ꗗ���X�V����������
     * 
     * @param boardIndex
     */
    public void prepareSubjectListUpdate(int boardIndex)
    {
//      // �X���ꗗ�������������A�č\�z����ꍇ... (�����Ď擾)
//        // �X������ a2B.idx�֏o�͂���
//        outputSubjectIndex(boardIndex);
//
//        // �X���C���f�b�N�X�̉�͂����{����
//        parseSubjectIndex(boardIndex);

        // �X���ꗗ���擾����
        getSubjectTxt(boardIndex);
        return;
    }
    
    /*
     *   �X���ꗗ����������
     * 
     */
    public void prepareSubjectList(int boardIndex)
    {
        // �X���C���f�b�N�X�̉�͂����{����
        parseSubjectIndex(boardIndex);

        // �擾URL�ƍX�V�t�@�C�����̊m��
        decideFileNameAndUrl(boardIndex, "subject.txt", false);

        // �X���ꗗ�̉�͂����{����
        parseSubjectTxt();

        // �^�C�g���ꗗ�\�z�����̒ʒm
        parent.readySubjectListDisplay();
        return;
    }

    /*
     *   �X���ꗗ�̉�͏���...
     * 
     * 
     */
    public void formatSubjectList()
    {
        // �X���ꗗ�̉�͂����{����
        parseSubjectTxt();

        // �^�C�g���ꗗ�\�z�����̒ʒm
        parent.readySubjectListDisplay();
        return;
    }

    /**
     *  �X�������o�́E�N���A���� (�؂�ւ��E�I�����ɌĂяo��)
     * 
     */
    public void leaveSubjectList(int boardIndex)
    {
        // �X������ a2B.idx�֏o�͂���
        outputSubjectIndex(boardIndex);
        boardNick = null;
        return;
    }

    /**
     * �X���Ǘ��N���X���擾����
     * 
     * 
     */
    private subjectDataHolder getSubjectDataHolder(int index)
    {
        // �擾�ςݐݒ�X���ꗗ�̏ꍇ...
        if (useOnlyDatabase == true)
        {
            subjectDataHolder dataholder = null;
            try
            {
                dataholder = (subjectDataHolder) subjectVector.elementAt(index);
//              subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
//                String key = data.threadNumber + ".dat";
//              dataholder = (subjectDataHolder) subjectMap.get(key);
            }
            catch (Exception e)
            {
                return (null);
            }
            return (dataholder);
        }        

        // �ʏ�ꗗ�̏ꍇ�B�B�B
        subjectDataHolder dataHolder = null;
        try
        {
            String fileName = getThreadFileName(index);
            dataHolder = (subjectDataHolder) subjectMap.get(fileName);
        }
        catch (Exception e)
        {
            return (null);
        }
        return (dataHolder);
    }
    
    /*
     *   �X���^�C�g���ꗗ�p�̕\���i�X����ԃT�}���j����������
     * 
     * 
     */
    public String getSubjectSummary(int index)
    {
        String headerString = "";
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        try
        {
            if (dataHolder != null)
            {
                if (dataHolder.entryFavorite > 0)
                {
                    headerString = "}";
                }
                headerString = headerString + getStatusSymbol(dataHolder.currentStatus);
                if ((dataHolder.currentStatus == STATUS_NEW)||(dataHolder.currentStatus == STATUS_UPDATE))
                {
                    return (headerString + "[New] " + getSubjectName(index));
                }
                else if (dataHolder.currentStatus == STATUS_RESERVE)
                {
                    return (headerString + "[---] " + getSubjectName(index));
                }
                else
                {
                    return (headerString + "[" + dataHolder.numberOfMessage + "] " + getSubjectName(index));                    
                }
            }
            int  resNum = getNofMessage(index);
            byte status = getStatus(index);
            if (isFavorite(index) == true)
            {
                headerString = "}";
            }
            headerString = headerString + getStatusSymbol(status);
            if ((status == STATUS_NEW)||(status == STATUS_UPDATE))
            {
                return (headerString + "[NEW] " + getSubjectName(index));
            }
            else if (status == STATUS_RESERVE)
            {
                return (headerString + "[---] " + getSubjectName(index));
            }
            else if (resNum == 0)
            {
                return ("");
            }
            else
            {
                return (headerString + "[" + resNum + "] " + getSubjectName(index));
            }
        }
        catch (Exception e)
        {
            // �������Ȃ�
        }
        return ("");
    }

    /*
     *   �X���ꗗ�̕ێ�������������
     *
     */
    public int getNumberOfSubjects()
    {
        if (useOnlyDatabase == true)
        {
            return (subjectMap.size());
        }
        return (subjectTitles);
    }
    
    /*
     *   �X���ꗗ�^�C�g�����擾����
     *   
     */
    public String getSubjectName(int index)
    {
        if (useOnlyDatabase == true)
        {
            subjectDataHolder dataHolder = getSubjectDataHolder(index);
            if (dataHolder == null)
            {
                return ("");
            }
            return (utf8Converter.parsefromSJ(dataHolder.getTitleName()));
        }
        
        if ((subjectTxtOffsetArray == null)||(subjectTxt == null))
        {
            return ("");
        }
        
        if ((index >= subjectTitles)||(index < 0))
        {
            return (" ");
        }
        int offset = subjectTxtOffsetArray[index];
        while (subjectTxt[offset] != '>')
        {
            offset++;
        }
        offset++;
        int length = subjectTxtOffsetArray[index + 1] - offset;

        while ((length > 0)&&(subjectTxt[offset + length] != '('))
        {
            length--;
        }
        if (subjectTxt[offset + (length - 1)] == ' ')
        {
            length--;
        }
        if (length <= 0)
        {
            return ("  ");
        }
        return (utf8Converter.parsefromSJ(subjectTxt, offset, length));
    }

    /*
     *  ���ݕ\�����Ă��郌�X�����擾����
     *
     */
    public int getCurrentMessage(int index)
    {
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder != null)
        {
            return (dataHolder.currentMessage);
        }
        return (1);
    }

    /*
     *  �X���̑S���X�����擾����
     *
     */
    public int getNumberOfMessages(int index)
    {
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder != null)
        {
            return (dataHolder.numberOfMessage);
        }
        return (1);
    }

    /*
     *   ���ݕ\�����X���ƍő僌�X����ݒ�
     *   
     */
    public void setCurrentMessage(int index, int number, int maxNumber)
    {
        //
        byte status = STATUS_ALREADY;
        if (number < maxNumber)
        {
            status = STATUS_REMAIN;
        }
        if (number > 1000)
        {
            status = STATUS_OVER;
        }
        if (number <= 0)
        {
            status = STATUS_RESERVE;
        }
        if (maxNumber < 0)
        {
            status = STATUS_NEW;
        }
        
        // ���C�ɓ���f�[�^�\�����[�h���ǂ����m�F����
        if (parent.getFavoriteShowMode() == true)
        {
            // ���C�ɓ���f�[�^�������ꍇ�A������̏����X�V
            favorManager.setStatus(index, status, number, maxNumber);
            return;
        }

        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        try
        {
            if (dataHolder != null)
            {
                dataHolder.currentMessage  = number;
                dataHolder.numberOfMessage = maxNumber;
                dataHolder.currentStatus   = status;
                Date curTime = new Date();
                dataHolder.lastModified = curTime.getTime();
                if (dataHolder.entryFavorite != 0)
                {
                    // ���C�ɓ���f�[�^�������ꍇ�ɂ́A��������X�V
                    int favoriteIndex = favorManager.exist(boardNick, dataHolder.threadNumber);
                    if (favoriteIndex >= 0)
                    {
                        favorManager.setStatus(favoriteIndex, status, number, maxNumber);
                    }
                    
                }
                return;
            }
        }
        catch (Exception e)
        {
            // �������Ȃ�
        }

        // �V�K�o�^
        entryThreadDataHolder(index, status, number, maxNumber);
        return;
    }

    /*
     *   ���ݕ\�����X���ƍő僌�X����ݒ�
     *   
     */
    public void setCurrentMessage(long threadNumber, int number, int maxNumber)
    {
        //
        byte status = STATUS_ALREADY;
        if (number < maxNumber)
        {
            status = STATUS_REMAIN;
        }
        if (number > 1000)
        {
            status = STATUS_OVER;
        }
        if (number <= 0)
        {
            status = STATUS_RESERVE;
        }
        if (maxNumber < 0)
        {
            status = STATUS_NEW;
        }
        subjectDataHolder dataHolder = null;
        try
        {
            String fileName = threadNumber + ".dat";
            dataHolder = (subjectDataHolder) subjectMap.get(fileName);
            if (dataHolder != null)
            {
                dataHolder.currentMessage  = number;
                dataHolder.numberOfMessage = maxNumber;
                dataHolder.currentStatus   = status;
                Date curTime = new Date();
                dataHolder.lastModified = curTime.getTime();

                return;
            }
        }
        catch (Exception e)
        {
            // �������Ȃ�
        }
        return;
    }

    /*
     *  �f�[�^�x�[�X�̂݃A�N�Z�X���郂�[�h���ǂ����̏�Ԃ�ݒ肷��
     * 
     */
    public void setOnlyDatabaseAccess(boolean isDatabase)
    {
        useOnlyDatabase = isDatabase;
        parent.setOnlyDatabaseAccess(isDatabase);
        return;
    }    
    
    /*
     *  �f�[�^�x�[�X�̂݃A�N�Z�X���郂�[�h���ǂ����̏�Ԃ��擾����
     * 
     */
    public boolean getOnlyDatabaseAccess()
    {
        return (useOnlyDatabase);
    }

    /*
     *   ���ݕ\�����X��Ԃ��擾
     *   
     */
    public byte getStatus(int index)
    {
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder == null)
        {
            return (STATUS_NOTYET);
        }
        return (dataHolder.currentStatus);
    }

    /*
     *   ���ݕ\�����X�����C�ɓ���ɓo�^����Ă��邩�`�F�b�N����
     *   
     */
    public boolean isFavorite(int index)
    {
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder == null)
        {
            return (false);
        }
        if (dataHolder.entryFavorite != 0)
        {
            return (true);
        }
        return (false);
    }
    
    /*
     *   ���ݕ\�����X��Ԃ�ݒ�
     *  
     */
    public void setStatus(int index, byte status)
    {
        if ((subjectTitles <= index)||(index < 0))
        {
            // �w�背�X�ԍ��I�[�o�t���[...
            return;
        }
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        try
        {
            if (dataHolder != null)
            {
                Date curTime = new Date();
                dataHolder.lastModified = curTime.getTime();
                dataHolder.currentStatus = status;
                if (dataHolder.entryFavorite != 0)
                {
                    // ���C�ɓ���ɓo�^�ς݂̃X���A���C�ɓ���f�[�^����Ԃ��X�V����
                    int favorIndex = favorManager.exist(boardNick, dataHolder.threadNumber);
                    if (favorIndex >= 0)
                    {
                        // ���C�ɓ���̃f�[�^��Ԃ� a2B.idx�ɓ���������
                        favorManager.setStatus(favorIndex, dataHolder.currentStatus, dataHolder.currentMessage, dataHolder.numberOfMessage);
                    }
                }
                return;
            }
        }
        catch (Exception e)
        {
            // �������Ȃ�
        }
        entryThreadDataHolder(index, status, 1, -1);
        return;
    }

    /*
     *   �X���t�@�C�������擾����
     * 
     * 
     */
    public String getThreadFileName(int index)
    {
        long threadNumber = getThreadNumber(index);
        if (threadNumber == 0)
        {
            return ("");
        }
         return (threadNumber + ".dat");
    }

    /**
     *  �t�@�C�����폜����
     * 
     * @param datFileName
     */
    private void deleteFile(String datFileName)
    {
        // �t�@�C�������쐬����
        deleteLogDataFile = parent.getBaseDirectory() + datFileName;

        // �X���ꗗ�̎擾
        Thread thread = new Thread()
        {
            public void run()
            {
                FileConnection    connection = null;
                try
                {
                    // ���O�t�@�C���̍폜�����s�I�I
                    connection = (FileConnection) Connector.open(deleteLogDataFile, Connector.READ_WRITE);
                    if (connection.exists() == true)
                    {
                        // �t�@�C�������݂����ꍇ�ɂ́A�t�@�C�����폜����
                        connection.delete();
                    }
                    connection.close();            
                }
                catch (Exception e)
                {
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
                deleteLogDataFile = null;
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            //
        }
    }
    
    /*
     *  ���O�t�@�C�����폜����
     * 
     */
    public void deleteLogDataFile(int index, String datFileName)
    {
        if (index < 0)
        {
            deleteFile(datFileName);
            return;
        }
        
        subjectDataHolder dataHolder = null;
        try
        {
            String fileName = getThreadFileName(index);
            dataHolder = (subjectDataHolder) subjectMap.remove(fileName);
            if (dataHolder != null)
            {
                // Hashtable/Vector����폜����...
                subjectVector.removeElement(dataHolder);
                dataHolder = null;

                // �t�@�C�����폜����
                deleteFile(datFileName);
            }
        }
        catch (Exception e)
        {
            //
        }

        // ���O�t�@�C�����Ȃ��������߁A�폜���Ȃ�...
        return;    
    }
    
    /*
     *   �X���f�[�^�N���X��o�^����
     * 
     */
    private void entryThreadDataHolder(int index, byte status, int currentMessage, int maxMessage)
    {
         if ((subjectTitles <= index)||(index < 0))
        {
            // �w�背�X�ԍ��I�[�o�t���[...
            return;
        }

        subjectDataHolder dataHolder   = null;

        // �X���^�C�g�����烌�X�����擾����
        long    threadNumber = getThreadNumber(index);
        int     resNumber    = getNofMessage(index);
        String   fileName     = threadNumber + ".dat";
        int     length       = getSubjectDataTitleLength(index);
        int     top          = subjectTxtOffsetArray[index];
        int     end          = subjectTxtOffsetArray[index+ 1] - 1;
        int     offset       = top;
        while ((offset < end)&&(subjectTxt[offset] != '>'))
        {
            offset++;
        }
        offset++;

        //
        if (currentMessage < 0)
        {
            currentMessage = 1;
        }

        //
        if (maxMessage < 0)
        {
            maxMessage = resNumber;
        }
        
        //  �f�[�^���e�[�u���Ɋi�[����
        dataHolder                   = null;
        dataHolder                   = new subjectDataHolder(subjectTxt, offset, length);
        dataHolder.threadNumber      = threadNumber;
        dataHolder.numberOfMessage   = maxMessage;
        dataHolder.currentMessage    = currentMessage;
        dataHolder.currentStatus     = status;
        subjectMap.put(fileName, dataHolder);
        subjectVector.addElement(dataHolder);
        dataExtracted = true;
        return;
    }

    /**
     * �X���^�C�g���̒����𒊏o����
     * 
     * @param index
     * @return
     */
    private int getSubjectDataTitleLength(int index)
    {
        if ((subjectTitles <= index)||(index < 0))
        {
            // �w�背�X�ԍ��I�[�o�t���[...
            return (0);
        }

        int     offset        = subjectTxtOffsetArray[index];
        int     endOffset     = subjectTxtOffsetArray[index+ 1] - 1;
        while ((offset < endOffset)&&(subjectTxt[offset] != '>'))
        {
            offset++;
        }
        offset++;
        
        while ((offset < endOffset)&&(subjectTxt[endOffset] != '('))
        {
            endOffset--;
        }
        endOffset--;

        return (endOffset - offset);
    }
    
    /**
     *   ���ݕ\�����̃X���ԍ����擾����
     *   
     */
    public long getThreadNumber(int index)
    {
        if (useOnlyDatabase == true)
        {
            if (index < 0)
            {
                return (0);
            }
            subjectDataHolder dataHolder = getSubjectDataHolder(index);
            if (dataHolder == null)
            {
                return (0);
            }
            return (dataHolder.threadNumber);
        }        

        if ((subjectTitles <= index)||(index < 0))
        {
            // �w�背�X�ԍ��I�[�o�t���[...
            return (0);
        }
    
        // �X�����擾�̏ꍇ�ɂ́A�ǉ�����
        long number = 0;
        int offset = subjectTxtOffsetArray[index];
        while ((subjectTxt[offset] >= '0')&&(subjectTxt[offset] <= '9'))
        {
            number = number * 10 + (subjectTxt[offset] - '0');
            offset++;
        }
        return (number);
    }

    /**
     *  �X���̃��X�����擾����
     * 
     * 
     */
    private int getNofMessage(int index)
    {
        // �C���f�b�N�X�ԍ��ُ�`�F�b�N
        if ((subjectTitles <= index)||(index < 0))
        {
            // �w�背�X�ԍ��I�[�o�t���[...
            return (0);
        }
        if (useOnlyDatabase == true)
        {
            subjectDataHolder dataHolder = getSubjectDataHolder(index);
            if (dataHolder == null)
            {
                return (0);
            }
            return (dataHolder.numberOfMessage);
        }            

        // �ʂ�ۉ��...
        if ((subjectTxtOffsetArray == null)||(subjectTxt == null))
        {
            return (0);
        }
        
        // �X���^�C�g�����烌�X�����擾����
        int offset    = subjectTxtOffsetArray[index];
        int endOffset = subjectTxtOffsetArray[index+ 1] - 1;
        while ((offset < endOffset)&&(subjectTxt[endOffset] != '('))
        {
            endOffset--;
        }
        endOffset++;
        int number = 0;
        while ((subjectTxt[endOffset] >= '0')&&(subjectTxt[endOffset] <= '9'))
        {
            number = number * 10 + (subjectTxt[endOffset] - '0');
            endOffset++;
        }
        return (number);
    }

    /**
     *   �t�@�C������URL�����߂�
     * 
     * 
     */
    private boolean decideFileNameAndUrl(int boardIndex, String fileName, boolean checkDirectory)
    {
        boolean ret = checkDirectory;
        
        // ���݂̃J�e�S���Ɣ���\������
        gettingLogDataURL = bbsCommunicator.getBoardURL(boardIndex);

        // �j�b�N�l�[�������肷��
        boardNick = bbsCommunicator.getBoardNickName(gettingLogDataURL);

        // �f�B���N�g���`�F�b�N���[�h���m�F
        if (checkDirectory == true)
        {
            // �f�B���N�g�����Ȃ���΍쐬����
            ret = bbsCommunicator.checkBoardNickName(boardNick);
        }

        // �t�@�C�����̐���
        gettingLogDataFile = boardNick + fileName;

        // URL���̊m��...
        gettingLogDataURL = gettingLogDataURL + fileName;

        return (ret);
    }
    
    /**
     *   �X���ꗗ�̎擾����
     *
     */
    private void getSubjectTxt(int boardIndex)
    {
        // �擾URL�ƍX�V�t�@�C�����̊m��
        decideFileNameAndUrl(boardIndex, "subject.txt", true);

        // �X���ꗗ�̎擾�����{
        Thread thread = new Thread()
        {
            public void run()
            {
                // �o�b�N�A�b�v
                if (parent.getBackupSubjectTxt() == true)
                {
                    // �t�@�C�����o�b�N�A�b�v���Ă���ʐM���s��...
                    parent.doCopyFile(parent.getBaseDirectory() + gettingLogDataFile + ".bak", parent.getBaseDirectory() + gettingLogDataFile);
                }
                parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, "", 0, 0, parent.SCENE_GETSUBJECTLIST, false);
            }
        };
        thread.start();            

        return;
    }

    /**
     *    �X���ꗗ�f�[�^���t�@�C������o�b�t�@�ɓǂݏo���A�C���f�b�N�X�𒣂�
     *
     *
     */
    private boolean parseSubjectTxt()
    {
        FileConnection connection = null;
        InputStream    in         = null;
        byte[] subjectTxtTmp     = null;

        if (gettingLogDataFile == null)
        {
            // �t�@�C���f�[�^�擾����ł͂Ȃ��A�A��͎��s...
            return (false);
        }

        // subject.txt���t�@�C������ǂݏo��...
        try 
        {
            // �t�@�C���I�[�v������...
            connection = (FileConnection) Connector.open( parent.getBaseDirectory() + gettingLogDataFile, Connector.READ);

            // �f�[�^�ǂݏo������...
            int dataFileSize   = (int) connection.fileSize();
            if ((dataFileSize == 0)&&(parent.getBackupSubjectTxt() == true))
            {
                // �ʐM���s�H�H �t�@�C���T�C�Y���[��������...
                connection.close();
                
                // �o�b�N�A�b�v�����t�@�C���������߂�...
                parent.doCopyFile(parent.getBaseDirectory() + gettingLogDataFile, parent.getBaseDirectory() + gettingLogDataFile + ".bak");

                // �t�@�C���I�[�v������...
                connection = (FileConnection) Connector.open( parent.getBaseDirectory() + gettingLogDataFile, Connector.READ);

                // �f�[�^�ǂݏo������...
                dataFileSize   = (int) connection.fileSize();
            }
            subjectTxt          = null;
            subjectTxtSize      = 0;
            subjectTxtTmp       = new byte[dataFileSize + bufferMargin];
            
            // �t�@�C���̓ǂݏo�����C��...
            in = connection.openInputStream();
            subjectTxtSize = in.read(subjectTxtTmp, 0, dataFileSize);
            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", gettingLogDataFile + " Exception  ee:" + e.getMessage());
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // �������Ȃ�
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
                // �������Ȃ�
            }
            gettingLogDataFile = null;        
            return (false);
        }
        
        // subject.txt����͂���...
//        int index  = 0;
//        subjectTxtOffsetArray[index] = 0;
//        index++;
//        for (int offset = 0; ((offset < subjectTxtSize)&&(index <= subjectManageSize)); offset++)
//        {
//            // ���s�R�[�h������ꏊ��������...
//            if (subjectTxt[offset] == 10)
//            {
//                subjectTxtOffsetArray[index] = offset + 1;
//                index++;
//            }
//        }
//        subjectTxtOffsetArray[index] = subjectTxtSize; // �\�h��...
//        subjectTitles = (index - 1);

        int index = 1;
        int subjectTxtSizeTmp = subjectTxtSize; 
        // �e�L�X�g�����߂���ǂ�ł���
        for (int offset = 0; offset < subjectTxtSizeTmp; offset++)
        {
            // ���s�R�[�h������ꏊ��������...
            if (subjectTxtTmp[offset] == 10)
            {
                offset++;
                subjectTxtOffsetArray[index] = offset;
                index++;
                if (index == subjectManageSize)
                {
                    break;
                }
            }
        }

        // �I�u�W�F�N�g�ϐ��̎Q�Ɛ��ύX
        subjectTxt = subjectTxtTmp;

        subjectTxtOffsetArray[index - 1] = subjectTxtSize; // �\�h��...
        subjectTitles = (index - 1);

        // �t�@�C�����͗p�ς݂Ȃ̂ŃN���A����
        gettingLogDataFile = null;

        System.gc();
        return (true);
    }

    /**
     *  �C���f�b�N�X��񐔂��m�F����
     * 
     */
    public int currentSubjectIndexSize()
    {
        return (subjectMap.size());
    }

    /**
     *   �C���f�b�N�X�����t�@�C���o�͂��A�C���f�b�N�X�f�[�^���N���A����
     *   
     */
    private void outputSubjectIndex(int boardIndex)
    {
        if (dataExtracted != true)
        {
            // �f�[�^���Ȃ��Ƃ��ɂ́A�t�@�C���o�͂��Ȃ�
            return;
        }
        FileConnection connection = null;
        OutputStream  out = null;
        
        // �擾�您��ю擾�t�@�C�����𐶐�����
        String url = bbsCommunicator.getBoardURL(boardIndex);
        int pos = url.lastIndexOf('/', url.length() - 2);
        // �j�b�N�l�[�������肷��
        boardNick = url.substring(pos + 1);
        String indexFile = parent.getBaseDirectory() + boardNick + "a2b.idx";
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

            Enumeration cnt = subjectMap.elements();
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
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", indexFile + " Exception  ee:" + e.getMessage() + " index:" + boardIndex);
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

        // �X���ꗗ�C���f�b�N�X���폜����
        subjectMap.clear();
        subjectVector.removeAllElements();
        dataExtracted = false;

        subjectTxt     = null;
        subjectTxtSize = 0;
        subjectTitles  = 0;
//        boardNick      = null;
        System.gc();
        return;
    }

    /**
     *   �X���ꗗ���C���f�b�N�X�t�@�C������W�J����...
     *   
     */
    private void parseSubjectIndex(int boardIndex)
    {
        // �X���ꗗ�C���f�b�N�X�t�@�C�����\�z�ς݂̏ꍇ�ɂ́A���̂܂ܖ߂�...
        dataExtracted = true;
        if (subjectMap.size() != 0)
        {
            return;
        }

        FileConnection connection    = null;
        InputStream    in            = null;

        byte[]         buffer        = null;
        int            dataReadSize  = 0;
        
        // �擾�您��ю擾�t�@�C�����𐶐�����
        String url = bbsCommunicator.getBoardURL(boardIndex);
        int pos = url.lastIndexOf('/', url.length() - 2);
        boardNick = url.substring(pos + 1);
        String indexFile = parent.getBaseDirectory() + boardNick + "a2b.idx";
        try
        {
            connection   = (FileConnection) Connector.open(indexFile, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  �f�[�^���ŏ����� size���ǂݏo��...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + bufferMargin];
            dataReadSize = in.read(buffer, 0, size);

            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", indexFile + " EXCEption  e:" + e.getMessage());
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

        // �t�@�C�������X�g�����O������
        String fileName = new String(buffer, nameOffset, nameLength);
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

        int favorIndex = favorManager.exist(nick, threadNumber);
        if (favorIndex >= 0)
        {
            // ���C�ɓ���o�^�ς݃f�[�^
            dataHolder.entryFavorite = 1;
            
            // ���C�ɓ���ɋL�^����Ă���X����Ԃ��擾����
            int sts    = favorManager.getStatus(favorIndex);
            int nofMsg = favorManager.getNumberOfMessages(favorIndex);
            int curMsg = favorManager.getCurrentMessage(favorIndex);
            long lastModifiedFavor = favorManager.getLastModified(favorIndex);

            // ���C�ɓ���f�[�^���X�V����Ă������H�H
            if (lastModifiedFavor > lastModified)
            {
                // �X����ԁA���X�ő吔�A���݂̓ǂ񂾈ʒu��(���C�ɓ���̂��̂�)���f������
                dataHolder.currentStatus = (byte) sts;
                dataHolder.numberOfMessage   = nofMsg;
                dataHolder.currentMessage    = curMsg;
                dataHolder.lastModified      = lastModifiedFavor;
            }
            else
            {
                // �ʏ��Ԃ�ʏ�ꗗ �� ���C�ɓ���֔��f������
                favorManager.setStatus(favorIndex, dataHolder.currentStatus, dataHolder.currentMessage, dataHolder.numberOfMessage);
            }

            if (nofMsg > dataHolder.numberOfMessage)
            {
                // a2B.idx�����C�ɓ���̏�Ԃɓ���������
                dataHolder.numberOfMessage = nofMsg;
                dataHolder.currentMessage = curMsg;
            }
            else if (nofMsg < dataHolder.numberOfMessage)
            {
                // ���C�ɓ���̃f�[�^��Ԃ� a2B.idx�ɓ���������
                favorManager.setStatus(favorIndex, dataHolder.currentStatus, dataHolder.currentMessage, dataHolder.numberOfMessage);
            }
        }
        subjectMap.put(fileName, dataHolder);
        subjectVector.addElement(dataHolder);

        return (length);
    }
    
    /**
     *   ���C�ɓ���̓o�^����������
     * 
     */
    public void removeFavorite(String nick, long threadNumber)
    {
        subjectDataHolder dataHolder = null;
        try
        {
            String fileName = threadNumber + ".dat";
            dataHolder = (subjectDataHolder) subjectMap.get(fileName);
            if (dataHolder == null)
            {
                return;
            }
            dataHolder.entryFavorite = 0;
        }
        catch (Exception e)
        {
            // �������Ȃ�
            return;
        }
        return;
    }
    
    /**
     *   ���C�ɓ���̓o�^/�o�^���������{����
     * 
     */
    public void setResetFavorite(int index, String url, String nick, boolean mode)
    {
        setResetFavorite(getSubjectDataHolder(index), url, nick, mode);
        return;
    }

    /**
     *   ���C�ɓ���̓o�^/�o�^���������{����
     * 
     * @param dataHolder
     * @param url
     * @param nick
     * @param mode
     */
    private boolean setResetFavorite(subjectDataHolder dataHolder, String url, String nick, boolean mode)
    {
        if (dataHolder == null)
        {
            return (false);
        }
        long threadNumber = dataHolder.threadNumber;
        if (favorManager.exist(nick, threadNumber) < 0)
        {
            // ���C�ɓ���o�^���s��
            if (mode == true)
            {
                favorManager.entryFavorite(dataHolder.getTitleName(), url, nick, threadNumber, dataHolder.numberOfMessage, dataHolder.currentMessage, dataHolder.currentStatus);
                dataHolder.entryFavorite = 1;
            }
        }
        else
        {
            // ���C�ɓ���o�^����������
            if (mode == false)
            {
                favorManager.removeFavorite(nick, threadNumber);
                dataHolder.entryFavorite = 0;
            }
        }
        return (true);
    }

    /**
     *   �V���m�F�̏��������{����
     * 
     * 
     */
    public void prepareGetLogList(int favorLevel)
    {
        favoriteLevel = favorLevel;
        if (favoriteLevel < 0)
        {
            subjectListCount = subjectMap.size();
        }
        else
        {
            subjectListCount = favorManager.getNumberOfFavorites();
        }

        // �V���m�F���{����������񍐂���
        parent.readyGetLogList();
        return;    
    }

    /**
     * 
     * 
     * 
     */
    public long getThreadNumberGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return (-1);
        }
        if (favoriteLevel >= 0)
        {
            return (favorManager.getThreadNumber(index));
        }
        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.threadNumber);
    }

    /**
     * 
     * 
     * 
     */
    public int getThreadResNumberGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return (-1);
        }
        if (favoriteLevel >= 0)
        {
            int resNumber = favorManager.getNumberOfMessages(index);
            return (resNumber);
        }

        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.numberOfMessage);
    }
    
    
    /**
     * 
     * 
     * 
     */
    public byte getThreadStatusGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return (STATUS_NOTYET);
        }
        if (favoriteLevel >= 0)
        {
            byte status = (byte) favorManager.getStatus(index);
            return (status);
        }

        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        if (data == null)
        {
            return (STATUS_NOTYET);
        }
        return (data.currentStatus);
    }

    /**
     * �X���^�C�g�����擾����
     * 
     * 
     */
    public String getSubjectNameGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return ("");
        }
        if (favoriteLevel >= 0)
        {
            return (favorManager.getSubjectName(index));
        }

        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        if (data == null)
        {
            return ("");
        }
        return (utf8Converter.parsefromSJ(data.getTitleName()));
    }

    /**
     *   URL���擾����
     * 
     */
    public String getFavoriteUrlGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return (null);
        }
        if (favoriteLevel < 0)
        {
            return (null);
        }
        if (favorManager.getNumberOfMessages(index) > 1000)
        {
            return (null);
        }
        int level = favorManager.getThreadLevel(index);
        if ((favoriteLevel == 0)||(favoriteLevel == level))
        {
            return (favorManager.getUrl(index));
        }
        return (null);
    }
    
    /**
     * 
     * 
     */
    public void setGetLogStatus(int index, byte status)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return ;
        }
        if (favoriteLevel >= 0)
        {
            // ���C�ɓ���̏�ԍX�V...
            favorManager.setStatus(index, status, -1, -1);
            if ((favorManager.getBoardNick(index)).equals(boardNick) == true)
            {
                // ������(�ʏ�)�f�[�^����Ԃ��X�V����
                String key = favorManager.getThreadNumber(index) + ".dat";
                try
                {
                    subjectDataHolder dataHolder = (subjectDataHolder) subjectMap.get(key);
                    if (dataHolder != null)
                    {
                        dataHolder.currentStatus = status;
                        return;
                    }
                }
                catch (Exception e)
                {
                    // �������Ȃ�
                }
            }
            return;
        }

        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        String key = data.threadNumber + ".dat";
        try
        {
            subjectDataHolder dataHolder = (subjectDataHolder) subjectMap.get(key);
            if (dataHolder != null)
            {
                dataHolder.currentStatus = status;
            
                if (dataHolder.entryFavorite != 0)
                {
                    // ���C�ɓ���f�[�^�������ꍇ�ɂ́A��������X�V
                    int favoriteIndex = favorManager.exist(boardNick, dataHolder.threadNumber);
                    if (favoriteIndex >= 0)
                    {
                        favorManager.setStatus(favoriteIndex, status, -1, -1);
                    }    
                }
                return;
            }
        }
        catch (Exception e)
        {
            // �������Ȃ�
        }
        return;
    }

    /**
     * 
     * 
     * 
     */
    public void clearGetLogList()
    {
        if (favoriteLevel >= 0)
        {
            // ���C�ɓ������ۑ�����
            favorManager.backup();
        }
        favoriteLevel    = -1;
        subjectListCount =  0;
        System.gc();
        return;
    }
    /**
     *  �f�[�^�v���r���[�����{����B
     * 
     */
    public void previewMessage(byte[] targetData, boolean useForm, int parseMode)
    {
        byte[] data = targetData;
        
        if (parseMode == a2BsubjectDataParser.PARSE_EUC)
        {
            data = new byte[targetData.length * 2];
            utf8Converter.StrCopyEUCtoSJ(data, 0, targetData, 0, targetData.length);
        }
        else
        {
            data = targetData;
        }

        // ���ȒP�ȃ��b�Z�[�W�̉��...
        byte[] parsedData = new byte[data.length];
        int tokenCount = 0;
        int srcIndex = 0;
        int dstIndex = 0;
        while (srcIndex < data.length)
        {
            // 1���X�̍Ō�����o�����ꍇ�ɂ͔�����...
            if ((parseMode == PARSE_2chMSG)&&(data[srcIndex] == (byte) 0x0a))
            {
                break;
            }
            else if (data[srcIndex] == (byte) '<')
            {
                // �^�O�͓ǂݔ�΂�...
                srcIndex++;
                if (data[srcIndex] == (byte) '>')
                {
                    tokenCount++;
                    if (tokenCount >= 3)
                    {
                        // 3�ڈȍ~�� <> �́A���s�ɒu������
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                    }
                    else
                    {
                        // 3�܂ł� <> �́A�X�y�[�X�P�ɒu������
                        parsedData[dstIndex] = (byte) ' ';                        
                    }
                    dstIndex++;
                    srcIndex++;
                }
                else if (((data[srcIndex] == (byte) 'b')||(data[srcIndex] == (byte) 'B'))&&
                    ((data[srcIndex + 1] == (byte) 'r')||(data[srcIndex + 1] == (byte) 'R')))
                {
                    parsedData[dstIndex] = (byte) '\r';
                    dstIndex++;
                    parsedData[dstIndex] = (byte) '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 2;
                    while ((srcIndex < data.length)&&(data[srcIndex] != (byte) '>'))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                }
                else if (((data[srcIndex] == (byte) 'b')||(data[srcIndex] == (byte) 'B'))&&
                        ((data[srcIndex + 1] == (byte) 'o')||(data[srcIndex + 1] == (byte) 'O'))&&
                        ((data[srcIndex + 2] == (byte) 'd')||(data[srcIndex + 2] == (byte) 'D'))&&
                        ((data[srcIndex + 3] == (byte) 'y')||(data[srcIndex + 3] == (byte) 'Y'))&&
                        (data[srcIndex + 4] == (byte) '>'))
                {
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 5;
                }
                else if (((data[srcIndex] == (byte) 'h')||(data[srcIndex] == (byte) 'H'))&&
                        ((data[srcIndex + 1] == (byte) 'e')||(data[srcIndex + 1] == (byte) 'E'))&&
                        ((data[srcIndex + 2] == (byte) 'a')||(data[srcIndex + 2] == (byte) 'A'))&&
                        ((data[srcIndex + 3] == (byte) 'd')||(data[srcIndex + 3] == (byte) 'D'))&&
                        (data[srcIndex + 4] == (byte) '>'))
                {
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 5;
                }
                else if (((data[srcIndex] == (byte) '/'))&&
                        ((data[srcIndex + 1] == (byte) 'b')||(data[srcIndex + 1] == (byte) 'B'))&&
                        ((data[srcIndex + 2] == (byte) 'o')||(data[srcIndex + 2] == (byte) 'O'))&&
                        ((data[srcIndex + 3] == (byte) 'd')||(data[srcIndex + 3] == (byte) 'D'))&&
                        ((data[srcIndex + 4] == (byte) 'y')||(data[srcIndex + 4] == (byte) 'Y'))&&
                        (data[srcIndex + 5] == (byte) '>'))
                {
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 6;
                        break;
                }
                else if (((data[srcIndex] == (byte) '/'))&&
                        ((data[srcIndex + 1] == (byte) 'h')||(data[srcIndex + 1] == (byte) 'H'))&&
                        ((data[srcIndex + 2] == (byte) 't')||(data[srcIndex + 2] == (byte) 'T'))&&
                        ((data[srcIndex + 3] == (byte) 'm')||(data[srcIndex + 3] == (byte) 'M'))&&
                        ((data[srcIndex + 4] == (byte) 'l')||(data[srcIndex + 4] == (byte) 'L'))&&
                        (data[srcIndex + 5] == (byte) '>'))
                {
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 6;
                        break;
                }
                else
                {
                    if (((data[srcIndex + 0] == (byte) 'h')||(data[srcIndex + 0] == (byte) 'H'))||
                         ((data[srcIndex + 0] == (byte) 'p')||(data[srcIndex + 0] == (byte) 'P'))||
                         ((data[srcIndex + 0] == (byte) 'd')||(data[srcIndex + 0] == (byte) 'D'))||
                         ((data[srcIndex + 0] == (byte) 'a')||(data[srcIndex + 0] == (byte) 'A'))||
                         ((data[srcIndex + 0] == (byte) 'l')||(data[srcIndex + 0] == (byte) 'L')))
                    {
                        // ���s�R�[�h������
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 1;
                    }
//                    parsedData[dstIndex] = (byte) ' ';
//                    dstIndex++;
                    while ((srcIndex < data.length)&&(data[srcIndex] != (byte) '>'))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                }
            }
            else if (data[srcIndex] == (byte) '&')
            {
                srcIndex++;
                if (((data[srcIndex + 0] == (byte) 'n')||(data[srcIndex + 0] == (byte) 'N'))&&
                        ((data[srcIndex + 1] == (byte) 'b')||(data[srcIndex + 1] == (byte) 'B'))&&
                        ((data[srcIndex + 2] == (byte) 's')||(data[srcIndex + 2] == (byte) 'S'))&&
                        ((data[srcIndex + 3] == (byte) 'p')||(data[srcIndex + 3] == (byte) 'P'))&&
                        (data[srcIndex + 4] == (byte) ';'))
                {
                    parsedData[dstIndex] = (byte) ' ';
                    dstIndex++;
                    srcIndex = srcIndex + 5;
                }
                else if (((data[srcIndex + 0] == (byte) 'a')||(data[srcIndex + 0] == (byte) 'A'))&&
                        ((data[srcIndex + 1] == (byte) 'm')||(data[srcIndex + 1] == (byte) 'M'))&&
                        ((data[srcIndex + 2] == (byte) 'p')||(data[srcIndex + 2] == (byte) 'P'))&&
                        (data[srcIndex + 3] == (byte) ';'))
                {
                    parsedData[dstIndex] = (byte) '&';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                }
                else if (((data[srcIndex + 0] == (byte) 'l')||(data[srcIndex + 0] == (byte) 'L'))&&
                        ((data[srcIndex + 1] == (byte) 't')||(data[srcIndex + 1] == (byte) 'T'))&&
                        (data[srcIndex + 2] == (byte) ';'))
                {
                    parsedData[dstIndex] = (byte) '<';
                    dstIndex++;
                    srcIndex = srcIndex + 3;
                }
                else if (((data[srcIndex + 0] == (byte) 'g')||(data[srcIndex + 0] == (byte) 'G'))&&
                        ((data[srcIndex + 1] == (byte) 't')||(data[srcIndex + 1] == (byte) 'T'))&&
                        (data[srcIndex + 2] == (byte) ';'))
                {
                    parsedData[dstIndex] = (byte) '>';
                    dstIndex++;
                    srcIndex = srcIndex + 3;
                }
                else
                {
                    // �ʏ�̈ꕶ���]��...
                    parsedData[dstIndex] = data[srcIndex];
                    dstIndex++;
                    srcIndex++;
                }
            }
            else
            {
                // �ʏ�̈ꕶ���]��...
                parsedData[dstIndex] = data[srcIndex];
                dstIndex++;
                srcIndex++;
            }
        }
        // �v���r���[�f�[�^�̕\��
        String previewData = "";
        if (parseMode == PARSE_2chMSG)
        {
            previewData = utf8Converter.parsefromSJ(parsedData, 0, dstIndex);
            int pos = previewData.lastIndexOf('\n');
            previewData = previewData.substring(pos + 1) + "\r\n=====\r\n" + previewData.substring(0, pos);
            parsedData  = null;
            if (useForm == false)
            {
                parent.showDialog(parent.SHOW_INFO, 0, "�v���r���[", previewData);
            }
            else
            {
                parent.OpenWriteFormUsingPreviewMode("�v���r���[", previewData);
            }
            previewData = null;
        }
        else if (parseMode == PARSE_UTF8)
        {
            try
            {
                previewData = new String(parsedData, 0, dstIndex, "UTF-8");
            }
            catch (Exception e)
            {
                previewData = new String(parsedData);
            }
            parsedData  = null;
            if (useForm == false)
            {
                parent.showDialog(parent.SHOW_INFO, 0, "�v���r���[", previewData);
            }
            else
            {
                parent.OpenWriteFormUsingPreviewMode("�v���r���[", previewData);
            }
            previewData = null;
        }
        else //  if (parseMode == PARSE_SJIS)
        {
            previewData = utf8Converter.parsefromSJ(parsedData, 0, dstIndex);
            parsedData  = null;
            parent.OpenWriteFormUsingTextEditMode("�f�[�^", previewData);
            previewData = null;
        }
        System.gc();
        return;
    }

    /**
     *  �X���^�C�g���𒊏o���ĉ�������
     * 
     */
    public String pickupThreadTitle(byte[] data)
    {
        // ���ȒP�ȃ��b�Z�[�W�̉��...
        byte[] parsedData = new byte[data.length];
        int tokenCount = 0;
        int srcIndex = 0;
        int dstIndex = 0;
        while (srcIndex < data.length)
        {
            // 1���X�̍Ō�����o�����ꍇ�ɂ͔�����...
            if (data[srcIndex] == (byte) 0x0a)
            {
                break;
            }
            else if (data[srcIndex] == (byte) '<')
            {
                // �^�O�͓ǂݔ�΂�...
                srcIndex++;
                if (data[srcIndex] == (byte) '>')
                {
                    tokenCount++;
                    if (tokenCount >= 3)
                    {
                        // 3�ڈȍ~�� <> �́A���s�ɒu������
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                    }
                    else
                    {
                        // 3�܂ł� <> �́A�X�y�[�X�P�ɒu������
                        parsedData[dstIndex] = (byte) ' ';                        
                    }
                    dstIndex++;
                    srcIndex++;
                }
                else if (((data[srcIndex] == (byte) 'b')||(data[srcIndex] == (byte) 'B'))&&
                    ((data[srcIndex + 1] == (byte) 'r')||(data[srcIndex + 1] == (byte) 'R'))&&
                    (data[srcIndex + 2] == (byte) '>'))
                {
                    parsedData[dstIndex] = (byte) '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 3;
                }
                else
                {
                    parsedData[dstIndex] = (byte) ' ';
                    dstIndex++;
                    while ((srcIndex < data.length)&&(data[srcIndex] != (byte) '>'))
                    {
                        srcIndex++;
                    }
                }
            }
            else
            {
                // �ʏ�̈ꕶ���]��...
                parsedData[dstIndex] = data[srcIndex];
                dstIndex++;
                srcIndex++;
            }
        }
        
        // �X���^�C�g���𒊏o����
        String previewData = utf8Converter.parsefromSJ(parsedData, 0, dstIndex);
        int pos = previewData.lastIndexOf('\n');
        if (pos < 0)
        {
            // �^�C�g���̒��o�Ɏ��s...
            return ("");
        }
        return (previewData.substring(pos + 1));
    }

    /**
     *  ��ԃV���{�����擾����
     * 
     */
    private String getStatusSymbol(byte status)
    {
        String objName;
        switch (status)
        {
          case 0:  // subjectParser.STATUS_NOTYET:
            objName = " ";
            break;

          case 1:  // subjectParser.STATUS_NEW:
            objName = "*";
            break;

          case 2:  // subjectParser.STATUS_UPDATE:
            objName = "+";
            break;

          case 3:  // subjectParser.STATUS_REMAIN:
            objName = "-";
            break;

          case 4:  // subjectParser.STATUS_ALREADY:
            objName = ":";
            break;

          case 5:  // subjectParser.STATUS_RESERVE:
            objName = "!";
            break;

          case 6:  // subjectParser.STATUS_UNKNOWN:
            objName = "?";
            break;

          case 7:  // subjectParser.STATUS_GETERROR:
            objName = "#";
            break;

          case 8:  // subjectParser.STATUS_OVER:
            objName = " ";
            break;

          default:
            objName = " ";
            break;
        }
        return (objName);
    }
    
    /**
     *   �X���ԍ��w���͏����̎��{
     *      �� �����ɂ́A(nick)/(�X���ԍ�) �Ƃ�����񂪑����Ă���
     * 
     *      - boardNick �� (nick) ����v�����ꍇ�A�A�A���̂܂܎擾�\��
     *      - (nick)��������ꍇ�A (nick)/a2b.idx ����͂��A�ΏۃX�����Ȃ���Ύ擾�\���ǉ�����
     *      - (nick)��������ꍇ�A
     * 
     * @param threadNumber
     */
    public boolean parseMessageThreadNumber(String threadNumber)
    {
        int pos = threadNumber.indexOf("/");
        if (pos <= 0)
        {
            // '/'�̌��o���s�A�A�A�I������
            return (false);
        }
        String threadNick = threadNumber.substring(0, (pos + 1));
        long number       = Long.parseLong(threadNumber.substring(pos + 1));
        if (boardNick.equals(threadNick) == true)
        {
            // ����v�����A�A���̂܂܎擾�\������{����
            String datFileName = number + ".dat";
            subjectDataHolder dataHolder = null;
            try
            {
                dataHolder = (subjectDataHolder) subjectMap.get(datFileName);
                if (dataHolder != null)
                {
                    // ���Ɏ擾�ς݃X���������A�A�A�擾�\�񂷂�
                    if (dataHolder.currentStatus >= STATUS_UNKNOWN)
                    {
                        dataHolder.currentStatus = STATUS_RESERVE;
                    }
                    if (parent.getAutoEntryFavorite() == true)
                    {
                        // ���C�ɓ���Ɏ����ǉ�����ꍇ...
                        String url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
                        setResetFavorite(dataHolder, url, threadNick, true);
                    }
                    return (true);
                }

                // �o�^����Ă��Ȃ��A�A�A�f�[�^���e�[�u���ɉ��i�[����
                for (int index = 0; index < subjectTitles; index++)
                {
                    long numnum = getThreadNumber(index);
                    if (numnum == number)
                    {
                        // subject.txt�Ɋi�[����Ă���...�^�C�g����؂����Ċi�[����
                        entryThreadDataHolder(index, STATUS_RESERVE, 1, 0);
                        if (parent.getAutoEntryFavorite() == true)
                        {
                            // ���C�ɓ���Ɏ����ǉ�����ꍇ...
                            String url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
                            setResetFavorite(index, url, threadNick, true);
                        }
                        parent.detectMessageThreadNumber(index);
                        return (true);
                    }
                }

                // subject.txt�ɂȂ�...�Ƃ肠�����Aa2b.idx�ɂ͊i�[����
                String threadTitleInterim =  "(a2B_INTERIM)" + threadNumber;
                dataHolder                   = null;
                dataHolder                   = new subjectDataHolder(threadTitleInterim.getBytes(), 0, threadTitleInterim.getBytes().length);
                dataHolder.threadNumber      = number;
                dataHolder.numberOfMessage   = 0;
                dataHolder.currentMessage    = 1;
                if (parent.getAutoEntryFavorite() == true)
                {
                    // ���C�ɓ���Ɏ����ǉ�����ꍇ...
                    String url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
                    setResetFavorite(dataHolder, url, threadNick, true);
                }
                subjectMap.put(datFileName, dataHolder);
                subjectVector.addElement(dataHolder);
                dataExtracted = true;

                // ���̂Ƃ��ɂ́A�\�����[�h���f�[�^�x�[�X�A�N�Z�X���[�h�ɋ����I�ɐ؂�ւ���
                useOnlyDatabase = true;
                parent.detectMessageThreadNumber(subjectVector.size() - 1);
            }
            catch (Exception e)
            {
                // �������Ȃ�
                return (false);
            }
            dataHolder.currentStatus     = STATUS_RESERVE;
            return (true);
        }

        if (parent.getAutoEntryFavorite() == true)
        {
            // ���C�ɓ���Ɏ����ǉ�����ꍇ...
            if (favorManager.exist(threadNick, number) < 0)
            {
                String url = bbsCommunicator.getBoardURL(threadNick);
                String tempTitle = "(a2B_INTERIM)" + threadNumber;
                favorManager.entryFavorite(tempTitle.getBytes(), url, threadNick, number, 0, 0, STATUS_RESERVE);
            }
        }
        // ���݃I�[�v�����Ă��Ȃ�������... (a2b.idx���I�[�v�����Ēǉ�����)
        return (reserveThread(threadNick, number));
    }

    /**
     *   �X�����擾�\�񂷂�...
     * 
     */
    private boolean reserveThread(String nick, long threadNumber)
    {
        String indexFile = parent.getBaseDirectory() + nick;
        FileConnection connection         = null;
        
        try
        {
            // �f�B���N�g�����I�[�v���ł��邩�H
            connection = (FileConnection) Connector.open(indexFile);
            if (connection.exists() == true)
            {
                if (connection.isDirectory() != true)
                {
                    // �f�B���N�g���ł͂Ȃ�����...�I������
                    connection.close();
                    connection = null;
                    return (false);
                }
            }
            else
            {
                // �f�B���N�g�����쐬����
                connection.mkdir();
                connection.close();
                connection = null;
                
                // a2b.idx�𖖔��ɒǉ�����
                return (appendA2Bindex(nick, threadNumber));
            }
            connection.close();
            connection = null;
        }
        catch (Exception e)
        {
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
            return (false);
        }
        
        InputStream    in                = null;
        byte[]         buffer            = new byte[indexReadSize + bufferMargin];
        int            dataReadSize      = 0;
        long           checkThreadNumber = 0;
        boolean       isChecking         = false;

        indexFile = indexFile + "a2b.idx";
        try
        {
            connection   = (FileConnection) Connector.open(indexFile, Connector.READ);

            //////  �f�[�^���ŏ����� size���ǂݏo��...  /////
            in           = connection.openInputStream();
            while (dataReadSize >= 0)
            {
                dataReadSize = in.read(buffer, 0, indexReadSize);

                // a2b.idx�̉��...
                for (int index = 0; index < dataReadSize; index++)
                {
                    if (buffer[index] != 10)
                    {
                        if (isChecking == true)
                        {
                            if ((buffer[index] >= '0')&&(buffer[index] <= '9'))
                            {
                                checkThreadNumber = checkThreadNumber * 10 + (buffer[index] - '0');
                            }
                            else
                            {
                                if (threadNumber == checkThreadNumber)
                                {
                                    // �X���͓o�^�ς݂������A�A�A���̏ꍇ�ɂ͉������Ȃ�...
                                    in.close();
                                    connection.close();
                                    buffer = null;                                    
                                    System.gc();
                                    return (true);
                                }
                                isChecking = false;
                            }
                        }
                    }
                    else
                    {
                        // �X���ԍ��̉�̓��[�h�ɐ؂�ւ���
                        isChecking = true;
                    }
                }
            }
            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", indexFile + " ExCEption  e:" + e.getMessage());
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
            dataReadSize = 0;
            System.gc();
            return (false);
        }
        buffer = null;        
        System.gc();

        // a2b.idx�ɋL�^����Ă��Ȃ��A�����ɋL�^����B�B
        return (appendA2Bindex(nick, threadNumber));
    }

    /**
     *  �\��X���𖖔��ɒǉ�����
     * 
     * @param nick
     * @param threadNumber
     * @return
     */
    private boolean appendA2Bindex(String nick, long threadNumber)
    {
        String         indexFile   = parent.getBaseDirectory() + nick + "a2b.idx";
        FileConnection connection  = null;
        OutputStream   out         = null;
        try 
        {
            // �t�@�C���̏�������
            connection = (FileConnection) Connector.open(indexFile, Connector.READ_WRITE);
            if (connection.exists() != true)
            {
                // �t�@�C���쐬
                connection.create();
            }
            
            // �t�@�C���̖������珑�����݂��s��...
            long size  =  connection.fileSize();
            out         = connection.openOutputStream(size + bufferMargin);
            if (size == 0)
            {
                // �t�@�C���T�C�Y���[���̏ꍇ�A�C���f�b�N�X�f�[�^�̃R�����g���o�͂���
                String comment = "; fileName, max, current, status, 0, 0, (Title)\n";
                out.write(comment.getBytes());
            }
            String outputData = ";\r\n" + threadNumber + ".dat,0,1," + STATUS_RESERVE + ",0,0," + "(a2B_INTERIM)" + threadNumber + "\r\n";
            out.write(outputData.getBytes());
            out.close();
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", indexFile + " Exception  ee:" + e.getMessage());
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
            return (false);
        }
        return (true);
    }

    /**
     *  �^�C�g�����𒣂�ւ���...
     * 
     * @param index
     * @param title
     */
    public void updateThreadTitle(int index, byte[] title, int offset, int length)
    {
        // �^�C�g�����𒣂�ւ���
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder == null)
        {
            return;
        }
        dataHolder.changeTitleName(title, offset, length);
        return;
    }

    /**
     *  �e�L�X�g�t�@�C���Ƀf�[�^�o�͂���
     * 
     * @param outputFileName  �o�̓t�@�C����
     * @param title           �^�C�g��
     * @param message         �o�̓f�[�^
     * @param isAppendMode    �㏑�����[�h(true)/�ǋL���[�h(false)
     * @return
     */
    public boolean outputDataTextFile(String outputFileName, String title, String message, boolean isAppendMode)
    {
        FileConnection connection = null;
        OutputStream  out = null;
        String memoFile = outputFileName;
        try
        {
            // �w��t�@�C��������΃t�@�C�����ǂ����H
            if (memoFile.indexOf("file:///") < 0)
            {
                // �����t�@�C��������΃p�X�w��ł͂Ȃ��ꍇ�A�A�A
                memoFile = parent.getBaseDirectory() + memoFile;
            }

            // �t�@�C���̏�������
            connection = (FileConnection) Connector.open(memoFile, Connector.READ_WRITE);
            if (connection.exists() != true)
            {
                // �t�@�C���쐬
                connection.create();
            }
            else
            {
                if (isAppendMode == false)
                {
                    // �t�@�C�������݂����ꍇ�ɂ́A��x�����č�蒼��
                    connection.delete();
                    connection.create();
                }
            }
            long fileSize = connection.fileSize();
            out = connection.openOutputStream(fileSize);

            // �t�@�C���ɏo�͂���
            String data = "- - - - - -\r\n";
            out.write(data.getBytes());
            data = title  + "\r\n";
            out.write(data.getBytes());
            data = message  + "\r\n";
            out.write(data.getBytes());
            
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
            return (false);
        }
        return (true);        
    }

    /**
     *  ���q�ɂȂ��Ă���dat�t�@�C�������炤
     *  (subject.txt�ɑ��݂��A�����݃X���f�[�^�N���X�ŊǗ�����Ă��Ȃ� dat�t�@�C����T��)
     * 
     * @return
     */
    public int collectDatFile()
    {
        int            nofFile   = 0;
        FileConnection connection = null;
        Enumeration    dirEnum    = null;
        String         dirName    = parent.getBaseDirectory() + boardNick;
        byte[]        readBuffer = new byte[readBufferSize + bufferMargin];
        try
        {
            // �w�肳�ꂽ�p�X����ꗗ���擾����
            connection = (FileConnection) Connector.open(dirName, Connector.READ);
            dirEnum = connection.list("*", true);
            while (dirEnum.hasMoreElements())
            {
                String name  = (String)dirEnum.nextElement();
                String title = name;
                if ((name.endsWith(".dat") == true)&&((name.indexOf(".bak") < 0)))
                {
                    // dat�t�@�C��������!!
                    subjectDataHolder dataHolder = (subjectDataHolder) subjectMap.get(name);
                    if (dataHolder == null)
                    {
                        // ���q��dat�t�@�C�������I                        
                        ////// �X���ԍ����o����... //////
                        int  index = 0;
                        long threadNumber = 0;
                        char ch = name.charAt(index);
                        while (ch != '.')
                        {
                            if ((ch >= '0')&&(ch <= '9'))
                            {
                                threadNumber = threadNumber * 10 + (ch - '0');
                            }
                            else
                            {
                                break;
                            }
                            index++;
                            ch = name.charAt(index);
                        }

                        long           fileSize = 0;
                        FileConnection  fc = null;
                        try
                        {
                            // �t�@�C�����I�[�v�����Ă݂āA�T�C�Y���m�F����...
                            fc = (FileConnection) Connector.open(parent.getBaseDirectory() + boardNick + name, Connector.READ_WRITE);
                            fileSize = fc.fileSize();

                            if (fileSize != 0)
                            {
                                // �X���^�C��ǂݏo���Ă݂�...
                                InputStream in = fc.openInputStream();
                                   in.read(readBuffer, 0, readBufferSize);
                                   title = pickupThreadTitle(readBuffer);
                                   in.close();
                            }
                            else
                            {
                                // �t�@�C���T�C�Y���[���̏ꍇ...�t�@�C�����폜���A�X���o�^�͂��Ȃ�
                                fc.delete();
                            }
                            fc.close();
                        }
                        catch (Exception ee)
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
                            fileSize = 0;
                        }

                        // �Ǘ��f�[�^�Ƃ��Ċi�[����B�B�B
                        if (fileSize != 0)
                        {
                            dataHolder                   = new subjectDataHolder(title.getBytes(), 0, (title.getBytes()).length);
                            dataHolder.threadNumber      = threadNumber;
                            dataHolder.numberOfMessage   = 1;
                            dataHolder.currentMessage    = 1;
                            dataHolder.currentStatus     = STATUS_UNKNOWN;
                            subjectMap.put(name, dataHolder);
                            subjectVector.addElement(dataHolder);
                            nofFile++;
                        }
                    }
                }
            }
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", dirName + " Exception eee:" + e.getMessage());
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
            nofFile = -1;
        }
        readBuffer = null;
        dataExtracted = true;
        System.gc();
        return (nofFile);
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
        public byte    entryFavorite   = 0;       // ���C�ɓ���o�^�ς݂��ǂ���
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
