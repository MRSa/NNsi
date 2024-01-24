package jp.sourceforge.nnsi.a2b.utilities;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.SocketConnection;
import javax.microedition.io.file.FileConnection;

import jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay;

/**
 * HTTP�ʐM���[�e�B���e�B<br>
 * a2B��http�ʐM���W���[����a2B framework�Ƀ|�[�g�������̂ł��B
 * MidpFileHttpCommunication�Ɣ�r���Aproxy�o�R�̒ʐM�Ahttp�ʐM�̕����擾�A
 * �����W�w��擾���\�ł��B<br>
 * ���̊g�����s�����߂ɁAIMidpA2BhttpCommunicator�C���^�t�F�[�X�����������N���X��
 * �K�v�ƂȂ��Ă��܂��B
 * 
 * @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
 * @see jp.sourceforge.nnsi.a2b.utilities.MidpFileHttpCommunication
 * @see IMidpA2BhttpCommunicator
 * @author MRSa
 *
 */
public class MidpA2BhttpCommunication
{
    private final int     TEMP_BUFFER_SIZE        = 20480;  // ��M�f�[�^�o�b�t�@�T�C�Y�A������ 32768 �ɂ��Ă���
    private final int     MARGIN                  = 10;     // �m�ۂ���o�b�t�@�̃}�[�W�� �i�ی�...�{���͕s�v�j
    
    private IMidpA2BhttpCommunicator parent        = null;   // �ʐM�p�C���^�t�F�[�X�N���X
    private ILogMessageDisplay       displayForm   = null;   // ���O�o�͒��̃��b�Z�[�W�\��

    private FileConnection dataFileConnection      = null;
    private int            dataFileSize           = -1;

    private String         urlToGet               = null;
    private String         referenceUrl           = null;
    private String         getMessageString       = null;
    private boolean       divideCommunication    = false;
    private boolean       isRecieved             = false;

    private int            appendMode             = IMidpA2BhttpCommunicator.APPENDMODE_NEWFILE;
    private int            responseCode           = -1;
    private int            bytesRead              = -1;
    private int            startRange             = -1;
    private int            endRange               = -1;

    /**
     *   �R���X�g���N�^�ł́A�ʐM�p�C���^�t�F�[�X�A���O�o�͗p�C���^�t�F�[�X���L�����܂��B<br>
     * ���O�o�͗p�C���^�t�F�[�X�́Anull�ł����܂��܂���inull���w�肵���ꍇ�ɂ́A�ʐM���̐i���\���A
     * �G���[�\�����ł��܂���B
     * 
     * @param object �ʐM�p�C���^�t�F�[�X
     * @param screen ���O�o�͗p�C���^�t�F�[�X
     * @see IMidpA2BhttpCommunicator
     */
    public MidpA2BhttpCommunication(IMidpA2BhttpCommunicator object, ILogMessageDisplay screen)
    {
        parent = object;
        displayForm = screen;
    }

    /**
     *   http GET�ʐM�̎��s�����A�ʐM����/���s�̏ڍׂ͕ʃ��\�b�h�Œʒm���܂��i�֐����Ăяo���܂��j<br>
     *   �Ȃ��A���̃N���X�𗘗p���ē����ɕ����̒ʐM�͂ł��܂���B
     * 
     *   @param fileName   �擾�����f�[�^���L�^����t�@�C�����i�x�[�X�f�B���N�g������̐�΃p�X�j
     *   @param url        �擾����URL
     *   @param reference  ���M����Q��URL(���t�@��URL�Anull�̏ꍇ�ɂ͎w��ȗ�)
     *   @param appendMsg  �擾���ɉ�ʕ\�����郁�b�Z�[�W(�ǉ�������)
     *   @param offset     ���M����I�t�Z�b�g(�擾����擪�o�C�g�̎w��A���̏ꍇ�ɂ͎w��ȗ�)
     *   @param range      ���M���郌���W (�擾�f�[�^�T�C�Y�̎w��A���̏ꍇ�ɂ͎w��ȗ�)
     *   @param mode       �t�@�C���ɋL�^����Ƃ��ǋL���邩�A�㏑�����邩 (true�̎��ɂ͒ǋL���[�h)
     *   @return           <code>true</code>:�ʐM���� / <code>false</code>:�ʐM���s
     * 
     */
    public boolean GetHttpCommunication(String fileName, String url, String reference, String appendMsg, int offset, int range, boolean mode)
    {
    	isRecieved = false;
        int appendMode = IMidpA2BhttpCommunicator.APPENDMODE_NEWFILE;
        if (mode == true)
        {
            appendMode = IMidpA2BhttpCommunicator.APPENDMODE_APPENDFILE;
        }
        
        String getFileName = null;
        if (fileName != null)
        {
            getFileName = fileName;
        }
        else
        {
            // �����񃂁[�h
            appendMode = IMidpA2BhttpCommunicator.APPENDMODE_STRING;
            offset = range;    
        }

        // ���O�̃r�W�[�\�����s��
        startLogMessage();
        updateLogMessage("http�ʐM������ :" + url);

        // �ʐM�̎��s�O�Ƀt�@�C�����J����
        boolean ret = openFileConnection(getFileName, appendMode);
        if (ret != true)
        {
            return (false);
        }

        // HTTP�ʐM������...
        if (parent.GetDivideGetHttp() == false)
        {
            // �X���ʏ�擾���[�h
            setParameter(url, reference, appendMsg, offset, appendMode, true);
            getURLUsingHttp();
        }
        else
        {
            // �X�������擾���[�h
            int startRange = offset;
            setParameter(url, reference, appendMsg, startRange, appendMode, true);
            ret = getURLUsingHttp();
            while (ret == true)
            {
                startRange = endRange + 1;
                setParameter(url, reference, appendMsg, startRange, IMidpA2BhttpCommunicator.APPENDMODE_APPENDFILE, false);
                ret = getURLUsingHttp();
            }
        }

        // �t�@�C�����N���[�Y������
        closeFileConnection();
        
        // ���O�̃r�W�[�\�����I��������
        finishLogMessage();
        return (true);
    }

    /**
     *  �Ō�Ɏ擾���������R�[�h���擾����
     * @return �����R�[�h
     */
    public int getLastResponseCode()
    {
    	return (responseCode);
    }

    /**
     *  ��M�����f�[�^���L�^�������ǂ���
     * @return <code>true</code> ��M�����f�[�^���L�^����   <code>false</code> �L�^���Ă��Ȃ�
     */
    public boolean isReceived()
    {
    	return (isRecieved);
    }
    
    /**
     *   ���O���b�Z�[�W�̏o�͂��J�n����
     */
    private void startLogMessage()
    {
        if (displayForm != null)
        {
            displayForm.outputInformationMessage("", true, false);
        }
    }

    /**
     *  ���O���b�Z�[�W���o�͂���
     * @param message �o�͂��郁�b�Z�[�W
     */
    private void updateLogMessage(String message)
    {
        if (displayForm != null)
        {
            displayForm.outputInformationMessage(message, false, false);
        }
    }

    /**
     *  ���O���b�Z�[�W�̏o�͂��I������
     */
    private void finishLogMessage()
    {
        if (displayForm != null)
        {
            displayForm.finishInformationMessage();
        }
    }

    /**
     *  ���[�U�[�G�[�W�F���g������������
     * @return ���[�U�[�G�[�W�F���g��
     */
    private String getUserAgent()
    {
        String ua = parent.GetUserAgent();
        if (ua == null)
        {
            ua = "Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3";
        }
        return (ua);
    }

    /**
     *  �t�@�C���̃I�[�v������
     * 
     * @param dataFileName �I�[�v������t�@�C����
     * @param mode �I�[�v�����[�h
     * 
     * @return <code>true</code>:�t�@�C���I�[�v��OK / <code>false</code>:�t�@�C���I�[�v��NG
     */
    private boolean openFileConnection(String dataFileName, int mode)
    {
        dataFileConnection = null;
        if (mode == IMidpA2BhttpCommunicator.APPENDMODE_STRING)
        {
            // �f�[�^�X�g�����O�i�[���[�h...
            return (true);
        }

        try
        {
            // �t�@�C�����I�[�v������
            dataFileConnection = (FileConnection) Connector.open(dataFileName, Connector.READ_WRITE);
            if (dataFileConnection.exists() != true)
            {
                // �t�@�C���쐬
                dataFileConnection.create();
            }
            if (mode == IMidpA2BhttpCommunicator.APPENDMODE_NEWFILE)
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
            updateLogMessage("Exception :" + msg);
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
            return (false);
        }
        return (true);
    }

    /**
     *  �t�@�C�����N���[�Y����
     *  
     */
    private void closeFileConnection()
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
        getMessageString    = null;
        
        // ������Ԃ�
        parent.CompletedToGetHttpData(responseCode, bytesRead);

        System.gc();
        return;
    }

    /**
     *   HTTP�ڑ����g���ăt�@�C���ɏo�͂���
     *   
     *       @param url
     *       @param reference
     *       @param appendString
     *       @param range
     *       @param scene
     *       @param mode
     */
    private void setParameter(String url, String reference, String appendString, int range, int mode, boolean isDivideMode)
    {
        // �p�����[�^��ݒ肷��
        getMessageString    = appendString;
        urlToGet            = null;
        urlToGet            = url;
        referenceUrl        = null;
        referenceUrl        = reference;
        startRange          = range;
        appendMode          = mode;
        responseCode        = -1;
        bytesRead           = -1;
        divideCommunication = isDivideMode;

        if (parent.GetDivideGetHttp() == true)
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
     *  �� WX310�p�̃��[�N�A���E���h
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
        String cookie = parent.GetCookie();
        if (cookie != null)
        {
        	urlToGet = urlToGet + "Cookie: " + cookie + "\r\n";
        }
        urlToGet = urlToGet + "User-Agent: " + getUserAgent() + "\r\n";
        urlToGet = urlToGet + "Accept-Encoding: identity\r\n";
        urlToGet = urlToGet + "Content-Language: ja, en\r\n";
        urlToGet = urlToGet + "Connection: Close\r\n";
        urlToGet = urlToGet + "Pragma: no-cache\r\n";

        if (appendMode != IMidpA2BhttpCommunicator.APPENDMODE_STRING)
        {
            // �����W�w�肪����Ă��邩�ǂ����`�F�b�N����
            if ((startRange > 0)||((startRange == 0)&&(divideCommunication == true)))
            {
                // Range�w�肪�������ꍇ... (��Βl�w��)
                urlToGet = urlToGet + "Range: bytes=" + startRange + "-";
                if (parent.GetDivideGetHttp() == true)
                {
                    endRange = (startRange + parent.GetDivideGetSize());
                    urlToGet = urlToGet + endRange;
                }
                urlToGet = urlToGet + "\r\n";
            }
            else if ((dataFileSize > 0)&&(startRange < 0)&&(dataFileSize > startRange))
            {
                // Range�w�肪�������ꍇ... (���Βl�w��)
                urlToGet = urlToGet + "Range: bytes=" + (startRange + dataFileSize) + "-";
                if (parent.GetDivideGetHttp() == true)
                {
                    endRange = (startRange + dataFileSize + parent.GetDivideGetSize());
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
        if (proxyUsage == IMidpA2BhttpCommunicator.PROXY_WORKAROUND_WX310)
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
            if (appendMode != IMidpA2BhttpCommunicator.APPENDMODE_STRING)
            {
                // Range�w�肪�������ꍇ...
                if ((startRange > 0)||((startRange == 0)&&(divideCommunication == true)))
                {
                    String range = "bytes=" + startRange + "-";
                    if (parent.GetDivideGetHttp() == true)
                    {
                        endRange = (startRange + parent.GetDivideGetSize());
                        range = range +  endRange;
                    }
                    connector.setRequestProperty("Range", range);
                }
                else if ((startRange < 0)&&(dataFileSize > startRange))
                {
                    String range = "bytes=" + (startRange + dataFileSize) + "-";
                    if (parent.GetDivideGetHttp() == true)
                    {
                        endRange = (startRange + dataFileSize + parent.GetDivideGetSize());
                        range = range + endRange;
                    }
                    connector.setRequestProperty("Range", range);                    
                }
            }
            connector.setRequestProperty("Connection", "Close");
            connector.setRequestProperty("Accept-Encoding", "identity");
            connector.setRequestProperty("Accept", "text/html, image/jpeg, */*");
            connector.setRequestProperty("Content-Language", " ja, en");
            String cookie = parent.GetCookie();
            if (cookie != null)
            {
            	connector.setRequestProperty("Cookie", cookie);
            }
            connector.setRequestProperty("User-Agent",getUserAgent());
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
            updateLogMessage("�L�^�� size�F " + bytesRead + appendMessage);
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
                    int offset = parent.GetHttpDataReceivedFirst(responseCode, divideCommunication, data, contentsOffset, actual);
                    if (offset >= 0)
                    {
                        if (outputDataStream != null)
                        {
                            outputDataStream.write(data, offset, (actual - offset));
                        }
                        else
                        {
                        	parent.ReceivedData(data, offset, (actual - offset));
                        }
                        bytesRead = bytesRead + (actual - offset);
                        isRecieved = true;
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
                    else
                    {
                    	parent.ReceivedData(data, 0, actual);
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
            if (proxyUsage == IMidpA2BhttpCommunicator.PROXY_WORKAROUND_WX310)
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
            updateLogMessage("http�ʐM�� " + getMessageString + "\n" + urlToGet);

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            responseCode = c.getResponseCode();
            if ((responseCode != HttpConnection.HTTP_OK)&&(responseCode != HttpConnection.HTTP_PARTIAL))
            {
                // close streams
                c.close();
                
                // �����R�[�h��416(Requested Range Not Satisfiable) �ȊO�������ꍇ�ɂ́A�G���[�\���B
                if (responseCode != 416)
                {
                    updateLogMessage("RESP NG (Code : " + responseCode + ") " + getMessageString + "\n" + urlToGet);
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
            updateLogMessage("http�ʐM�I�� "+ getMessageString + "\n" + urlToGet);

            // stream open
            InputStream is = c.openInputStream();
            if (appendMode == IMidpA2BhttpCommunicator.APPENDMODE_APPENDFILE)
            {
                // �t�@�C���ǋL���[�h
                os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
            }
            else if (appendMode == IMidpA2BhttpCommunicator.APPENDMODE_NEWFILE)
            {
                // �t�@�C�����㏑�����郂�[�h
                os = dataFileConnection.openOutputStream();                
            }
            else  // if (appendMode == APPENDMODE_STRING)
            {
                // �o�̓t�@�C�����I�[�v�����Ȃ�...
            }

            // Get the ContentType
            //String type = c.getType();

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
            updateLogMessage("ERR>" + msg + "\nURL: " + urlToGet + "\n");
            try
            {
                if (os != null)
                {
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
            ret = false;
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
        String nameOperator = "socket://" + parent.GetProxyUrl();
        try 
        {               
            // BUSY���b�Z�[�W��ݒ肷��
            updateLogMessage("proxy�v�����M������");

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
            updateLogMessage("http�v�����M��\n " + urlToGet);

            // �f�[�^�𑗏o����
            outputStream.write(urlToGet.getBytes());

            updateLogMessage("http������M��...");

            // ���́i��M�j�p�X�g���[�����I�[�v������
            InputStream is = sockConn.openInputStream();

            if (appendMode == IMidpA2BhttpCommunicator.APPENDMODE_APPENDFILE)
            {
                // �t�@�C���ǋL���[�h
                os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
            }
            else if (appendMode == IMidpA2BhttpCommunicator.APPENDMODE_NEWFILE)
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
            updateLogMessage("Exception : " + msg + "\n" + nameOperator);
            try
            {
                if (os != null)
                {
                    os.flush();
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
     * @return <code>true</code>:�ʐM���� / <code>false</code>:�ʐM���s
     */
    private boolean getURLUsingHttp()
    {
        boolean result = false;

        // �t�@�C�����o�͂���R�l�N�^���I�[�v������Ă��Ȃ���ΏI������
        if ((dataFileConnection == null)&&(appendMode != IMidpA2BhttpCommunicator.APPENDMODE_STRING))
        {
            // �ʐM�G���[��\������
            updateLogMessage("CONNECTION(ERROR) Connection : null");
            
            // �ʐM���s�� (CLUB AIR-EDGE , �m�F�����Ƃ���No��I�������ꍇ...)
            return (result);
        }

        // �r�W�[�\�����X�V����
        updateLogMessage("HTTP�ʐM������ " + urlToGet);

        // Proxy�̎g�p�ݒ���擾����
        int proxyUsage = parent.GetProxyMode();

        // PROXY���g�p���邩�ǂ����̔�����s���B
        if (proxyUsage == IMidpA2BhttpCommunicator.PROXY_DO_USE_PROXY)
        {
            // Proxy�o�R�̒ʐM���s��
            result = executeProxyHttpCommunication(proxyUsage);
        }
        else
        {
            // �ʏ��HTTP�ʐM���s��
            result = executeHttpCommunication(proxyUsage);
        }
        return (result);
    }
}
