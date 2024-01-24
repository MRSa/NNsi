package jp.sourceforge.nnsi.a2b.utilities;
import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;

import jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay;

/**
 * HTTP�ʐM���[�e�B���e�B<br>
 * <ul>
 *   <li>HTTP�ʐM���s���A���̎�M���e���t�@�C���ɋL�^���邱�Ƃ��ł��܂��B</li>
 *   <li>�ʐM���郆�[�U�[�G�[�W�F���g��ύX���邱�Ƃ��ł��܂��B<br>
 * (�f�t�H���g�ł�WX310J(Netfront 3.3)�Ɠ����Ȃ��̂�ݒ肵�Ă��܂��B)</li>
 * </ul>
 * �Ȃ��A�ʐM�̐i���󋵂�prepare()�Ŏw�肵��ILogMessageDisplay��
 * �C���^�t�F�[�X���g���A�\�������邱�Ƃ��ł��܂��B
 * <br>
 * �ڍׂ́A
 * {@link jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay }
 * ���Q�l�ɂ��Ă��������B
 * 
 * 
 * @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
 * @author MRSa
 */
public class MidpFileHttpCommunication
{
    // 
    private final int MARGIN           = 16;
	private final int FILE_BUFFER_SIZE = 43008;  // �o�b�t�@�T�C�Y
    private ILogMessageDisplay displayForm = null;  // ���O�o�͒��̃��b�Z�[�W�\��

    private   String UserAgent = "Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3";  // ���[�U�G�[�W�F���g

    /**
	 *  �R���X�g���N�^�ł̏����͂���܂���B
     * 
     */
    public MidpFileHttpCommunication()
    {
        // �Ȃɂ����Ȃ�...
    }
    
    /**
     *  �i���󋵕\���N���X�̐ݒ�
     *  <br>
     *  �ʒʐM���A�ʐM�O�ɐݒ肵�Ă��������B(1��ʐM����Ɖ������܂��B)
     *  
     *  @param screen �f�[�^�o�̓N���X�̐ݒ�
     * 
     */
    public void prepare(ILogMessageDisplay screen)
    {
        displayForm = screen;
    }

    /**
     * ���[�U�[�G�[�W�F���g��ݒ肵�܂�<br>
     * �f�t�H���g�ł�WX310J(Netfront 3.3)�Ɠ����Ȃ��̂�ݒ肵�Ă��܂��B
     * 
     * @param userAgent ���[�U�G�[�W�F���g��
     */
    public void SetUserAgent(String userAgent)
    {
        if ((userAgent == null)||(userAgent.length() == 0))
        {
            UserAgent = "Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3";    
        }
        else
        {
            UserAgent = userAgent;
        }
        return;
    }
    
    /**
     * ���[�U�G�[�W�F���g���擾���܂�
     * 
     * @return  ���ݐݒ肵�Ă��郆�[�U�G�[�W�F���g
     */
    public String GetUserAgent()
    {
        return (UserAgent);
    }

    /**
     *  �w�肳�ꂽ�擾��URL����AHTTP�v���w�b�_�𐶐��i�ϊ��j���܂�(WX310�p�ł�)<br>
     *  WX310�ł́Ahttp�ʐM�ŕK��gzip�̎w�肪�t������Ă��܂��܂��̂ŁA<br>
     *  �����������邽�߂Ɏ擾���URL�𒲐����܂��B
     *  
     *  @param urlToGet �擾��URL
     *  @return HTTP�w�b�_���̎擾��URL
     *
     */
    private String modifyUrlforWX310(String urlToGet)
    {
        int start = urlToGet.indexOf("://");
        start = start + 3;
        int end   = urlToGet.indexOf("/", start);
        String host = urlToGet.substring(start, end);
        urlToGet = urlToGet + " HTTP/1.0\r\n";
        urlToGet = urlToGet + "Host: " + host + "\r\n";
        urlToGet = urlToGet + "User-Agent: " + GetUserAgent() + "\r\n";
        urlToGet = urlToGet + "Accept-Encoding: identity\r\n";
        urlToGet = urlToGet + "Content-Language: ja, en\r\n";
        urlToGet = urlToGet + "Connection: Close\r\n";
        urlToGet = urlToGet + "Pragma: no-cache\r\n";
        urlToGet = urlToGet + "\r\nWX310";        
        return (urlToGet);
    }

    /**
     * �i�����b�Z�[�W�̏o��
     * 
     * @param message �i�����b�Z�[�W
     */
    private void updateBusyMessage(String message)
    {
    	if (displayForm != null)
        {
            displayForm.outputInformationMessage(message, false, false);
        }
        return;
    }
    
    /**
     * HTTP�ʐM�����s���ăt�@�C�����擾���܂�
     * 
     * @param url           �擾����URL
     * @param baseDirectory �L�^����f�B���N�g����
     * @return �T�[�o�[�̉����R�[�h
     */
    public int DoHttpMain(String url, String baseDirectory, String targetFileName)
    {
        int            rc  = -1;
        int    receiveByte = 0;
        int         actual = 10;
        String     urlToGet = null;
        byte[]        data = null;
        HttpConnection   c = null;
        InputStream     is = null;
        OutputStream    os = null;
        FileConnection dfc = null;
        try
        {
            // ���b�Z�[�W
        	urlToGet = (baseDirectory + targetFileName);
            updateBusyMessage("���� : " + urlToGet);
            
            // �����o���t�@�C���̏���...
            dfc = (FileConnection) Connector.open(urlToGet, Connector.READ_WRITE);
            try
            {
                if (dfc.exists() == true)
                {
                    // �t�@�C�������݂���ꍇ�ɂ͍폜����
                    dfc.delete();
                }
            }
            catch (Exception ex)
            {
            	// �������Ȃ�...
            	updateBusyMessage("Exception " + ex.getMessage());
            }
            try
            {
                // �t�@�C���쐬
                dfc.create();
            }
            catch (Exception ex)
            {
            	// �������Ȃ�...
            	updateBusyMessage("Exception " + ex.getMessage());
            }
            
            // �v���b�g�t�H�[�����̂𒲂ׁAWX310�������ꍇ�ɂ́Agzip�]���΍���s��
            urlToGet = url;
            updateBusyMessage("�ڑ� : " + urlToGet);
            String platForm = System.getProperty("microedition.platform");
            if (platForm.indexOf("WX310") >= 0)
            {
                // URL�𒲐�����
                urlToGet = modifyUrlforWX310(url);

                // HTTP�ʐM�̏���
                c = (HttpConnection) Connector.open(urlToGet);
            }
            else
            {
                // HTTP�ʐM�̏���
                c = (HttpConnection) Connector.open(urlToGet);

                // �l�t�����U��...
                c.setRequestMethod(HttpConnection.GET);
                c.setRequestProperty("user-agent", GetUserAgent());
                c.setRequestProperty("accept", "text/html, */*");
                c.setRequestProperty("content-language", " ja, en");
            }

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            rc = c.getResponseCode();

            // stream open
            is = c.openInputStream();
            os = dfc.openOutputStream();                

            data = new byte[FILE_BUFFER_SIZE + MARGIN];
            receiveByte = 0;
            actual = 10;
            while (actual > 0)
            {
                updateBusyMessage("��M : " + receiveByte);
                actual = is.read(data, 0, FILE_BUFFER_SIZE);
                if (actual > 0)
                {
                    os.write(data, 0, actual);
                    receiveByte = receiveByte + actual;
                }
            }
            os.close();
            is.close();
            c.close();
            dfc.close();
            updateBusyMessage("���� : " + receiveByte);
        }
        catch (Exception e)
        {
            rc = rc * (-100);
        	try
            {
            	os.close();
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
            	is.close();
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
                c.close();
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
                dfc.close();
            }
            catch (Exception e2)
            {
                //
            }
            updateBusyMessage("�ʐM�G���[ ");
        }
        data = null;
        if (displayForm != null)
        {
            // ���O�o�͂̏I���ʒm
        	displayForm.finishInformationMessage();
            displayForm = null;
        }
        is = null;
        os = null;
        c = null;
        dfc = null;
        System.gc();
        return (rc);
    }
}
