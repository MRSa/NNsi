import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;

/**
 * HTTP�ʐM�p�̃N���X
 * 
 * @author MRSa
 *
 */
public class kaniFileHttpCommunication
{
    // 
	private String UserAgent = "Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3";  // ���[�U�G�[�W�F���g
	private final int FILE_BUFFER_SIZE = 16384;  // �o�b�t�@�T�C�Y
	private IlogMessageDisplay displayForm = null;  // ���O�o�͒��̃��b�Z�[�W�\��
	
	/**
	 * �R���X�g���N�^...
	 *
	 */
	kaniFileHttpCommunication()
    {
    	// �Ȃɂ����Ȃ�...
    }
	
	/**
	 *  �N���X�̏���
	 * 
	 */
    public void prepare()
	{
    	// �Ȃɂ����Ȃ�...
	}
	
	/**
	 * ���[�U�[�G�[�W�F���g��ݒ肷��
	 * 
	 * @param userAgent
	 */
    public void setUserAgent(String userAgent)
    {
        if (userAgent.length() == 0)
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
	 * ���[�U�G�[�W�F���g���擾����
	 * 
	 * @return  �ݒ肵�Ă��郆�[�U�G�[�W�F���g
	 */
	public String getUserAgent()
    {
        return (UserAgent);
	}

	/**
     *  �擾URL����AHTTP�v���w�b�_�𐶐��i�ϊ��j���� (WX310�p)
     * 
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
		urlToGet = urlToGet + "User-Agent: " + getUserAgent() + "\r\n";
		urlToGet = urlToGet + "Accept-Encoding: identity\r\n";
		urlToGet = urlToGet + "Content-Language: ja, en\r\n";
		urlToGet = urlToGet + "Connection: Close\r\n";
		urlToGet = urlToGet + "Pragma: no-cache\r\n";
		urlToGet = urlToGet + "\r\nWX310";		
		return (urlToGet);
    }

    /**
     * ���b�Z�[�W�̏o��
     * 
     * @param message
     */
    private void updateBusyMessage(String message)
    {
    	if (displayForm != null)
    	{
    		displayForm.outputMessage(message);
    	}
    	return;
    }
	
	/**
	 * HTTP�ʐM�����s���ăt�@�C�����擾����
	 * 
	 * @param parent   ���O���b�Z�[�W���o���N���X
	 * @param url      �擾����URL
	 * @param fileName �L�^����t�@�C����
	 */
	public int doHttpMain(IlogMessageDisplay parent, String url, String baseDirectory)
	{
        int            rc  = -1;
		HttpConnection   c = null;
        InputStream     is = null;
        OutputStream    os = null;
		FileConnection dfc = null;
        displayForm = parent;
        try
		{
            // ���b�Z�[�W
        	updateBusyMessage("���� : " + url);

            // URL�̒��o�ɐ��������Ƃ�����http�ʐM�����s����...
        	String fileName = "index.html";
			int pos = url.lastIndexOf('/');
	        if (url.length() > (pos + 1))
	        {
	            fileName = url.substring(pos + 1);
	            pos = fileName.indexOf("?");
	            if (pos >= 0)
	            {
	            	fileName = fileName.substring(pos + 1) + ".html";
	            }
	            else
	            {
	            	pos = fileName.indexOf("*");
                    if (pos >= 0)
                    {
		            	fileName = fileName.substring(pos + 1) + ".html";                            	
                    }
	            }
	        }
            // ���b�Z�[�W
        	updateBusyMessage("���� : " + fileName);

            // �t���p�X�w��ɂ���
        	fileName = baseDirectory + fileName;
        	
            // �����o���t�@�C���̏���...
        	dfc = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
			if (dfc.exists() == true)
			{
				// �t�@�C�������݂���ꍇ�ɂ͍폜����
                dfc.delete();
			}
            // �t�@�C���쐬
			dfc.create();

			// �v���b�g�t�H�[�����̂𒲂ׁAWX310�������ꍇ�ɂ́Agzip�]���΍���s��
			String urlToGet = url;
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
    			c.setRequestProperty("user-agent", getUserAgent());
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

            byte[] data = new byte[FILE_BUFFER_SIZE];

            int receiveByte = 0;
            int actual = 10;
			while (actual > 0)
			{
	            updateBusyMessage("��M : " + receiveByte);
		        actual = is.read(data, 0, FILE_BUFFER_SIZE);
				os.write(data, 0, actual);
				receiveByte = receiveByte + actual;
			}
	        os.close();
	        is.close();
	        c.close();
	        dfc.close();
            updateBusyMessage("���� : " + receiveByte);
		}
        catch (Exception e)
        {
        	rc = rc * (-1000);
			try
			{
				if (os != null)
				{
					os.close();
				}
			}
			catch (Exception e2)
			{
                //
			}
			try
			{
				if (is != null)
				{
					is.close();
				}
			}
			catch (Exception e2)
			{
                //
			}
			try
			{
				if (c != null)
				{
					c.close();
				}
			}
			catch (Exception e2)
			{
                //
			}
			try
			{
				if (dfc != null)
				{
					dfc.close();
				}
			}
			catch (Exception e2)
			{
                //
			}
            updateBusyMessage("�ʐM�G���[ ");
        }
        if (displayForm != null)
        {
            displayForm.finishMessage();  // ���O�o�͂̏I���ʒm
            displayForm = null;
        }
        return (rc);
	}
}
