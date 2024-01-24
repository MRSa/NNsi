import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;

/**
 * HTTP通信用のクラス
 * 
 * @author MRSa
 *
 */
public class kaniFileHttpCommunication
{
    // 
	private String UserAgent = "Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3";  // ユーザエージェント
	private final int FILE_BUFFER_SIZE = 16384;  // バッファサイズ
	private IlogMessageDisplay displayForm = null;  // ログ出力中のメッセージ表示
	
	/**
	 * コンストラクタ...
	 *
	 */
	kaniFileHttpCommunication()
    {
    	// なにもしない...
    }
	
	/**
	 *  クラスの準備
	 * 
	 */
    public void prepare()
	{
    	// なにもしない...
	}
	
	/**
	 * ユーザーエージェントを設定する
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
	 * ユーザエージェントを取得する
	 * 
	 * @return  設定しているユーザエージェント
	 */
	public String getUserAgent()
    {
        return (UserAgent);
	}

	/**
     *  取得URLから、HTTP要求ヘッダを生成（変換）する (WX310用)
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
     * メッセージの出力
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
	 * HTTP通信を実行してファイルを取得する
	 * 
	 * @param parent   ログメッセージを出すクラス
	 * @param url      取得するURL
	 * @param fileName 記録するファイル名
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
            // メッセージ
        	updateBusyMessage("準備 : " + url);

            // URLの抽出に成功したときだけhttp通信を実行する...
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
            // メッセージ
        	updateBusyMessage("準備 : " + fileName);

            // フルパス指定にする
        	fileName = baseDirectory + fileName;
        	
            // 書き出しファイルの準備...
        	dfc = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
			if (dfc.exists() == true)
			{
				// ファイルが存在する場合には削除する
                dfc.delete();
			}
            // ファイル作成
			dfc.create();

			// プラットフォーム名称を調べ、WX310だった場合には、gzip転送対策を行う
			String urlToGet = url;
            updateBusyMessage("接続 : " + urlToGet);
			String platForm = System.getProperty("microedition.platform");
			if (platForm.indexOf("WX310") >= 0)
			{
                // URLを調整する
				urlToGet = modifyUrlforWX310(url);

                // HTTP通信の準備
    			c = (HttpConnection) Connector.open(urlToGet);
            }
			else
       	    {
    			// HTTP通信の準備
    			c = (HttpConnection) Connector.open(urlToGet);

    			// ネフロを偽装...
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
	            updateBusyMessage("受信 : " + receiveByte);
		        actual = is.read(data, 0, FILE_BUFFER_SIZE);
				os.write(data, 0, actual);
				receiveByte = receiveByte + actual;
			}
	        os.close();
	        is.close();
	        c.close();
	        dfc.close();
            updateBusyMessage("完了 : " + receiveByte);
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
            updateBusyMessage("通信エラー ");
        }
        if (displayForm != null)
        {
            displayForm.finishMessage();  // ログ出力の終了通知
            displayForm = null;
        }
        return (rc);
	}
}
