import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.SocketConnection;
import javax.microedition.io.file.FileConnection;

/**
 * a2BHttpCommunicator : HTTP�ʐM�N���X
 * 
 * @author MRSa
 *
 */
public class a2BHttpCommunicator
{
    public  final int     APPENDMODE_STRING       = 2;      // String�^�Ƀf�[�^���i�[����
    public  final int     APPENDMODE_APPENDFILE   = 1;      // �t�@�C���ɒǋL
    public  final int     APPENDMODE_NEWFILE      = 0;      // �t�@�C����V���ɍ쐬���ċL�^����

    private final int     TEMP_BUFFER_SIZE        = 20480;  // ��M�f�[�^�o�b�t�@�T�C�Y�A������ 32768 �ɂ��Ă���
//    private final int     DIVIDE_GET_SIZE         = 50000;  // �X�������擾���̂P�񕪎擾�T�C�Y
//    private final int     DIVIDE_GET_SIZE         = 30500;  // �X�������擾���̂P�񕪎擾�T�C�Y
    private final int     MARGIN                  = 10;     // �m�ۂ���o�b�t�@�̃}�[�W�� �i�ی�...�{���͕s�v�j
    
    private a2BMain        parent                 = null;   // �e�N���X (a2BMain�N���X)
    private FileConnection dataFileConnection     = null;
    private int            dataFileSize           = -1;

    private String         urlToGet               = null;
    private String         referenceUrl           = null;
    private String         getMessageString       = null;

    private boolean       communicating           = false;
    private boolean       divideCommunication     = false;

    private long           SLEEP_SECONDS          = 3000;
    private int            appendMode             = APPENDMODE_NEWFILE;
    private int            responseCode           = -1;
    private int            bytesRead              = -1;
    private int            startRange             = -1;
    private int            endRange               = -1;
    private int            httpScene              = -1;

    /**
     *   �R���X�g���N�^
     * 
     * @param object
     */
    public a2BHttpCommunicator(a2BMain object)
    {
        parent = object;
    }

    /**
     *   �ʐM�����ǂ������擾����
     * 
     * @return
     */
    public boolean isCommunicating()
    {
        return (communicating);
    }

    /**
     * �����擾�T�C�Y���擾����
     * 
     * @return
     */
    private int getDivideGetSize()
    {
        return (parent.getDivideGetSize());
    }

    /**
     *  �t�@�C���̃I�[�v������
     * 
     * @param dataFileName
     * @param mode
     * 
     * @return
     */
    public boolean openFileConnection(String dataFileName, int mode)
    {
        // �ʐM���ɐݒ肷��
        communicating = true;

        dataFileConnection = null;
        if (mode == APPENDMODE_STRING)
        {
            // �f�[�^�X�g�����O�i�[���[�h...
            return (true);
        }

        try
        {
            dataFileConnection = (FileConnection) Connector.open(dataFileName, Connector.READ_WRITE);
            if (dataFileConnection.exists() != true)
            {
                // �t�@�C���쐬
                dataFileConnection.create();
            }
            if (mode == APPENDMODE_NEWFILE)
            {
                // �V�K�擾���[�h�̂Ƃ��́A�폜���Ă���t�@�C�����쐬����
                dataFileConnection.delete();
                dataFileConnection.create();
            }
            dataFileSize = (int) dataFileConnection.fileSize();
        }        
        catch (Exception e)
        {
            // ��O�����I�I�I
            String msg = e.toString() + " : " + e.getMessage();
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + msg);
            try
            {
                if (dataFileConnection != null)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
                dataFileSize = -1;
            }
            catch (Exception e2)
            {
                // 
            }
            communicating = false;
            return (false);
        }
        return (true);
    }

    /**
     *  �t�@�C�����N���[�Y����
     *  
     */
    public void closeFileConnection()
    {
        try
        {
            if (dataFileConnection != null)
            {
                dataFileConnection.close();
            }
        }
        catch (Exception e)
        {
            // ��O�������ɂ͉������Ȃ�
        }
        dataFileConnection  = null;
        dataFileSize        = -1;
        startRange          = -1;
        endRange            = -1;
        communicating       = false;
        divideCommunication = false;
        getMessageString    = null;
        
        // ������Ԃ�
        parent.CompletedToGetHttpData(httpScene, responseCode, bytesRead);

        System.gc();
        return;
    }
    
    /**
     *   HTTP�ڑ����g���ăt�@�C���ɏo�͂���
     *   
     *       @param url�F�擾��URL
     *       @param reference
     *       @param appendString
     *       @param range
     *       @param scene
     *       @param mode
     *       @param isDivide
     */
    public void setParameter(String url, String reference, String appendString, int range, int scene, int mode, boolean isDivide)
    {
        // �p�����[�^��ݒ肷��
        getMessageString    = appendString;
        urlToGet            = null;
        urlToGet            = url;
        referenceUrl        = null;
        referenceUrl        = reference;
        startRange          = range;
        httpScene           = scene;
        appendMode          = mode;
        responseCode        = -1;
        bytesRead           = -1;
        divideCommunication = isDivide;        

        if (parent.getDivideGetHttp() == true)
        {
            try
            {
                dataFileSize = (int) dataFileConnection.fileSize();
            }
            catch (Exception e)
            {
                // �Ƃ肠�����A�傫�Ȓl�ɂ���...
                dataFileSize = 2147483600;
            }
        }
        return;
    }

    /**
     *  �擾URL����AHTTP�v���w�b�_�𐶐��i�ϊ��j����
     * 
     */
    private void updateToGetUrlForHttpHeader()
    {
        int start = urlToGet.indexOf("://");
        start = start + 3; // '://' ��3������...
        int end   = urlToGet.indexOf("/", start);
        String host = urlToGet.substring(start, end);
        urlToGet = urlToGet + " HTTP/1.0\r\n";
        urlToGet = urlToGet + "Host: " + host + "\r\n";
        urlToGet = urlToGet + "User-Agent: " + parent.getUserAgent(false) + "\r\n";
        urlToGet = urlToGet + "Accept-Encoding: identity\r\n";
        urlToGet = urlToGet + "Content-Language: ja, en\r\n";
        urlToGet = urlToGet + "Connection: Close\r\n";
        urlToGet = urlToGet + "Pragma: no-cache\r\n";

        if (appendMode == APPENDMODE_STRING)
        {
            if (httpScene == parent.SCENE_PREVIEW_MESSAGE)
            {
                // ���Xpreview���[�h�̂Ƃ�...
                urlToGet = urlToGet + "Range: bytes=0-" + startRange + "\r\n";
            }
        }
        else
        {
            // �����W�w�肪����Ă��邩�ǂ����`�F�b�N����
            if ((startRange > 0)||((startRange == 0)&&(divideCommunication == true)))
            {
                // Range�w�肪�������ꍇ... (��Βl�w��)
                urlToGet = urlToGet + "Range: bytes=" + startRange + "-";
                if (parent.getDivideGetHttp() == true)
                {
                    endRange = (startRange + getDivideGetSize());
                    urlToGet = urlToGet + endRange;
                }
                urlToGet = urlToGet + "\r\n";
            }
            else if ((dataFileSize > 0)&&(startRange < 0)&&(dataFileSize > startRange))
            {
                // Range�w�肪�������ꍇ... (���Βl�w��)
                urlToGet = urlToGet + "Range: bytes=" + (startRange + dataFileSize) + "-";
                if (parent.getDivideGetHttp() == true)
                {
                    endRange = (startRange + dataFileSize + getDivideGetSize());
                    urlToGet = urlToGet + endRange;
                }
                urlToGet = urlToGet + "\r\n";
            }
        }
        urlToGet = urlToGet + "\r\n";        
        return;
    }

    /**
     *   HTTP�̗v���w�b�_��ݒ肷�� 
     * 
     * @param connector
     * @param proxyUsage
     */
    private void setHttpConnectionParameter(HttpConnection connector, int proxyUsage)
    {
        if (proxyUsage == parent.PROXY_WORKAROUND_WX310)
        {
            // PROXY�ɂ��āAWX310�p�̒ʐM�ݒ肾�����ꍇ�ɂ́A�����Őݒ肵�Ȃ�
            return;
        }

        try
        {
            // HTTP�̒ʐM�p�w�b�_�𐶐�����...
            connector.setRequestMethod(HttpConnection.GET);
            if (referenceUrl != null)
            {
                connector.setRequestProperty("Referer", referenceUrl);
            }
            if (appendMode == APPENDMODE_STRING)
            {
                if (httpScene == parent.SCENE_PREVIEW_MESSAGE)
                {
                    // ���Xpreview���[�h�̂Ƃ�...
                    String range = "bytes=" + "0-" + startRange;
                    connector.setRequestProperty("Range", range);                
                }
            }
            else
            {
                // Range�w�肪�������ꍇ...
                if ((startRange > 0)||((startRange == 0)&&(divideCommunication == true)))
                {
                    String range = "bytes=" + startRange + "-";
                    if (parent.getDivideGetHttp() == true)
                    {
                        endRange = (startRange + getDivideGetSize());
                        range = range +  endRange;
                    }
                    connector.setRequestProperty("Range", range);
                }
                else if ((startRange < 0)&&(dataFileSize > startRange))
                {
                    String range = "bytes=" + (startRange + dataFileSize) + "-";
                    if (parent.getDivideGetHttp() == true)
                    {
                        endRange = (startRange + dataFileSize + getDivideGetSize());
                        range = range + endRange;
                    }
                    connector.setRequestProperty("Range", range);                    
                }
            }
            connector.setRequestProperty("Connection", "Close");
            connector.setRequestProperty("Accept-Encoding", "identity");
            connector.setRequestProperty("Accept", "text/html, image/jpeg, */*");
            connector.setRequestProperty("Content-Language", " ja, en");
            connector.setRequestProperty("User-Agent",parent.getUserAgent(false));
        }
        catch (Exception e)
        {
            // �p�����[�^���ݒ�ł��Ȃ������ꍇ�ɂ́A���̂܂܏I������B
        }
        return;
    }

    /**
     * �w�b�_�̍Ō����������
     * 
     * @param dataBuffer
     * @param length
     * 
     * @return
     */
    private int  lookupHttpBody(byte[] dataBuffer, int length)
    {
        //
        // TODO: �{���́A�����ŉ����R�[�h���m�F����K�v����I�I
        //      (HTTP_OK ���邢�� HTTP_PARTIAL �̏ꍇ�̂݉�������)
        //

        String buffer = new String(dataBuffer);
        int index = buffer.indexOf("\r\n\r\n");
        buffer = null;
        if (index <= 0)
        {
            return (0);
        }
        return (index + 4);
    }
    
    /**
     * ��M�f�[�^���R�s�[����
     * 
     * @param inputDataStream    ���̓X�g���[��
     * @param outputDataStream   �o�̓X�g���[��
     * @param contentLength  �f�[�^�T�C�Y
     * 
     * @return
     */
    private boolean copyReceivedData(InputStream inputDataStream, OutputStream outputDataStream, boolean hasHeader, String appendMessage) throws IOException
    {
        // Get the length and process the data
        boolean first = true;
        boolean ret   = true;
        byte[]  data   = new byte[TEMP_BUFFER_SIZE + MARGIN];
        int  contentsOffset = 0;
        bytesRead = 0;
        
        int  actual = MARGIN;
        while (actual > 0)
        {
            actual = inputDataStream.read(data, 0, TEMP_BUFFER_SIZE);
            parent.updateBusyMessage("�L�^��", "size�F " + bytesRead + appendMessage, false);
            if (actual > 0)
            {
                // ����Ɏ�M�����f�[�^�̏���...
                if (first == true)
                {
                    first = false;

                    // ��M�f�[�^�Ƀw�b�_���������Ă���ꍇ...
                    if (hasHeader == true)
                    {
                        // �R���e���c�{�f�B�̐擪��������
                        contentsOffset = lookupHttpBody(data, actual);
                    }

                    // �f�[�^�̐��������`�F�b�N����
                    int offset = parent.getHttpDataReceivedFirst(httpScene, responseCode, divideCommunication, data, contentsOffset, actual);
                    if (offset >= 0)
                    {
                        if (outputDataStream != null)
                        {
                            outputDataStream.write(data, offset, (actual - offset));
                        }
                        bytesRead = bytesRead + (actual - offset);
                    }
                    else
                    {
                        // �ʐM�I��
                        actual    = offset;
                        if (offset != -1)
                        {
                            // �ʐM�G���[�����������ꍇ...
                            bytesRead = offset;
                        }
                        ret = false;
                    }
                }
                else
                {
                    // 2��ڈȍ~��M�����f�[�^...�����f�[�^�������Ă�������...
                    if (outputDataStream != null)
                    {
                        outputDataStream.write(data, 0, actual);
                    }
                    bytesRead = bytesRead + actual;
                }
            }
        }
        data = null;
        return (ret);
    }
    
    /**
     * HTTP�ʐM�̎��s������
     * 
     * @param proxyUsage �v���L�V���p���[�h
     * 
     * @return
     */
    private boolean executeHttpCommunication(int proxyUsage)
    {
        boolean      ret = true;
        OutputStream  os = null;
        HttpConnection c = null;
        try 
        {
            bytesRead = -1;
            if (proxyUsage == parent.PROXY_WORKAROUND_WX310)
            {
                // WX310SA(�V�t�@�[��)/WX310J�����̉����...
                // URL�������ւ���...
                updateToGetUrlForHttpHeader();
                urlToGet = urlToGet + "WX310";
            }

            // HTTP�̐ڑ��I�[�v��
            c = (HttpConnection) Connector.open(urlToGet);
 
            // HTTP�̃p�����[�^��ݒ肷��
            setHttpConnectionParameter(c, proxyUsage);

            // BUSY���b�Z�[�W��ݒ肷��
            parent.updateBusyMessage("http�ʐM��", "�ʐM�� " + getMessageString + "\n" + urlToGet, false);

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            responseCode = c.getResponseCode();
            if ((responseCode != HttpConnection.HTTP_OK)&&(responseCode != HttpConnection.HTTP_PARTIAL))
            {
                // close streams
                c.close();
                
                // �����R�[�h��416(Requested Range Not Satisfiable) �������ꍇ�ɂ́A�G���[�\���B
                if (responseCode != 416)
                {
                    parent.updateBusyMessage("HTTP RESPONSE NG", "Code : " + responseCode + " " + getMessageString + "\n" + urlToGet, false);
                    parent.sleepTime(SLEEP_SECONDS);
                    bytesRead     = -1;
                }
                else
                {
                    // �����R�[�h��416�̏ꍇ...
                    bytesRead = endRange;
                }
                c = null;
                System.gc();
                return (false);
            }
            parent.updateBusyMessage("http�ʐM�I��", "�ʐM�I�� "+ getMessageString + "\n" + urlToGet, false);

            // stream open
            InputStream is = c.openInputStream();
            if (appendMode == APPENDMODE_APPENDFILE)
            {
                // �t�@�C���ǋL���[�h
                os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
            }
            else if (appendMode == APPENDMODE_NEWFILE)
            {
                // �t�@�C�����㏑�����郂�[�h
                os = dataFileConnection.openOutputStream();                
            }
            else  // if (appendMode == APPENDMODE_STRING)
            {
                // �o�̓t�@�C�����I�[�v�����Ȃ�...
            }

            // Get the ContentType
//          String type = c.getType();

            // ��M�f�[�^���R�s�[���� (��M�f�[�^��HTTP�w�b�_�͕t������Ă��Ȃ�)
            ret = copyReceivedData(is, os, false, "\n (Len: " + c.getLength() + ")");

            // �X�g���[�������
            if (os != null)
            {
                os.flush();
                os.close();
                os = null;
            }
            is.close();
            c.close();
            is = null;
            c  = null;
        }
        catch (Exception e)
        {
            String msg = e.toString() + " : " + e.getMessage();
            parent.updateBusyMessage("Exception(http)", "ERR>" + msg + "\nURL: " + urlToGet + "\n", false);
            parent.sleepTime(SLEEP_SECONDS);
            try
            {
                if (os != null)
                {
                    //Thread.sleep(SLEEP_MILLISECONDS);
                    os.flush();
                    os.close();
                    os = null;
                }
            }
            catch (Exception e2)
            {
                // 
                os = null;
            }
            c = null;
            System.gc();
            return (false);
        }
        System.gc();
        return (ret);
    }

    /**
     * �v���L�V�o�R�ł̃A�N�Z�X�{����...
     * 
     * @param proxyUsage
     * 
     * @return
     */
    private boolean executeProxyHttpCommunication(int proxyUsage)
    {
        boolean ret = true;
        bytesRead = -1;
        OutputStream os = null;
        String nameOperator = "socket://" + parent.getProxyUrl();
        try 
        {               
            // BUSY���b�Z�[�W��ݒ肷��
            parent.updateBusyMessage("proxyHttp�ʐM", "�v�����M������", false);

            // �\�P�b�g���I�[�v������
            SocketConnection sockConn = (SocketConnection) Connector.open(nameOperator, Connector.READ_WRITE);

            // �I�v�V�������΂�΂�ݒ肷��
            sockConn.setSocketOption(SocketConnection.KEEPALIVE, 1);
            sockConn.setSocketOption(SocketConnection.DELAY,     0);
            sockConn.setSocketOption(SocketConnection.LINGER,    0);
            
            // �o�́i���M�j�p�X�g���[�����I�[�v������
            OutputStream outputStream = sockConn.openOutputStream();

            // URL�������ւ���...
            updateToGetUrlForHttpHeader();
            urlToGet = "GET " + urlToGet;

            // BUSY���b�Z�[�W��ݒ肷��
            parent.updateBusyMessage("http���M��", "�v�����M��\n " + urlToGet, false);

            // �f�[�^�𑗏o����
            outputStream.write(urlToGet.getBytes());

            parent.updateBusyMessage("http��M��", "������M��...", false);

            // ���́i��M�j�p�X�g���[�����I�[�v������
            InputStream is = sockConn.openInputStream();

            if (appendMode == APPENDMODE_APPENDFILE)
            {
                // �t�@�C���ǋL���[�h
                os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
            }
            else if (appendMode == APPENDMODE_NEWFILE)
            {
                // �t�@�C�����㏑�����郂�[�h
                os = dataFileConnection.openOutputStream();                
            }
            else  // if (appendMode == APPENDMODE_STRING)
            {
                // �o�̓t�@�C�����I�[�v�����Ȃ�...
                os = null;
            }

            // ��M�f�[�^���R�s�[���� (��M�f�[�^��HTTP�w�b�_�͕t������Ă���I�I)
            ret = copyReceivedData(is, os, true, "\n");

            // �X�g���[���Q�����
            outputStream.close();
            if (os != null)
            {
                os.flush();
                os.close();
                os = null;
            }
            is.close();
            is = null;
            sockConn.close();
            sockConn = null;
        }
        catch (Exception e)
        {
            String msg = e.toString() + " : " + e.getMessage();
            parent.updateBusyMessage("Exception(proxyHttp)", urlToGet + "\nReason : " + msg + "\n" + nameOperator, false);
            parent.sleepTime(SLEEP_SECONDS);
            try
            {
                if (os != null)
                {
                    os.flush();
                    //Thread.sleep(SLEEP_MILLISECONDS);
                    os.close();
                }
            }
            catch (Exception e2)
            {
                //                
            }
            os = null;
            return (false);
        }
        System.gc();
        return (ret);
    }

    /**
     *   HTTP�ڑ����g���ăt�@�C���ɏo�͂��� �i���C�������j
     *   
     * @return
     */
    public boolean getURLUsingHttp()
    {
        boolean result = false;

        // �t�@�C�����o�͂���R�l�N�^���I�[�v������Ă��Ȃ���ΏI������
        if ((dataFileConnection == null)&&(appendMode != APPENDMODE_STRING))
        {
            // �ʐM�G���[��\������
            parent.updateBusyMessage("CONNECTION(ERROR)", "Connection : null", false);
            parent.sleepTime(SLEEP_SECONDS);
            
            // �ʐM���s�� (CLUB AIR-EDGE , �m�F�����Ƃ���No��I�������ꍇ...)
            return (result);
        }

        // �r�W�[�\�����X�V����
        parent.updateBusyMessage("HTTP�ʐM������", urlToGet, false);        

        // Proxy�̎g�p�ݒ���擾����
        int proxyUsage = parent.getUsedProxyScene();
        boolean useProxy = false;

        // PROXY���g�p���邩�ǂ����̔�����s���B
        if ((proxyUsage == parent.PROXY_USE_ALL)||(proxyUsage == parent.PROXY_USE_ONLYGET))
        {
            // proxy�o�R�̒ʐM�����{����
            useProxy = true;
        }
        else if (proxyUsage == parent.PROXY_USE_ONLYRANGE)
        {
            if (startRange != 0)
            {
                // Range�w�肪����Ă����ꍇ�ɂ́AProxy�o�R�̒ʐM�����s����
                useProxy = true;
            }
        }

        // HTTP�ŒʐM���邩�AProxy�o�R�ŒʐM���邩�A�����������...
        if (useProxy == false)
        {
            // �ʏ��HTTP�ʐM���s��
            result = executeHttpCommunication(proxyUsage);
        }
        else
        {
            // Proxy�o�R�̒ʐM���s��
            result = executeProxyHttpCommunication(proxyUsage);
        }
        return (result);
    }

    /**
     * �Ō�Ɏw�肵�����������W���擾����
     * 
     * @return
     */
    public int getLastEndRange()
    {
        return (endRange);
    }
}
//--------------------------------------------------------------------------
//  a2BHttpCommunicator  --- HTTP�ʐM�N���X
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
