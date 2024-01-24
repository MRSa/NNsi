package jp.sourceforge.nnsi.a2b.utilities;
import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;

import jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay;

/**
 * HTTP通信ユーティリティ<br>
 * <ul>
 *   <li>HTTP通信を行い、その受信内容をファイルに記録することができます。</li>
 *   <li>通信するユーザーエージェントを変更することができます。<br>
 * (デフォルトではWX310J(Netfront 3.3)と等価なものを設定しています。)</li>
 * </ul>
 * なお、通信の進捗状況はprepare()で指定したILogMessageDisplayの
 * インタフェースを使い、表示させることができます。
 * <br>
 * 詳細は、
 * {@link jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay }
 * を参考にしてください。
 * 
 * 
 * @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
 * @author MRSa
 */
public class MidpFileHttpCommunication
{
    // 
    private final int MARGIN           = 16;
	private final int FILE_BUFFER_SIZE = 43008;  // バッファサイズ
    private ILogMessageDisplay displayForm = null;  // ログ出力中のメッセージ表示

    private   String UserAgent = "Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3";  // ユーザエージェント

    /**
	 *  コンストラクタでの処理はありません。
     * 
     */
    public MidpFileHttpCommunication()
    {
        // なにもしない...
    }
    
    /**
     *  進捗状況表示クラスの設定
     *  <br>
     *  個別通信毎、通信前に設定してください。(1回通信すると解除します。)
     *  
     *  @param screen データ出力クラスの設定
     * 
     */
    public void prepare(ILogMessageDisplay screen)
    {
        displayForm = screen;
    }

    /**
     * ユーザーエージェントを設定します<br>
     * デフォルトではWX310J(Netfront 3.3)と等価なものを設定しています。
     * 
     * @param userAgent ユーザエージェント名
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
     * ユーザエージェントを取得します
     * 
     * @return  現在設定しているユーザエージェント
     */
    public String GetUserAgent()
    {
        return (UserAgent);
    }

    /**
     *  指定された取得先URLから、HTTP要求ヘッダを生成（変換）します(WX310用です)<br>
     *  WX310では、http通信で必ずgzipの指定が付加されてしまいますので、<br>
     *  それを回避するために取得先のURLを調整します。
     *  
     *  @param urlToGet 取得先URL
     *  @return HTTPヘッダつきの取得先URL
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
     * 進捗メッセージの出力
     * 
     * @param message 進捗メッセージ
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
     * HTTP通信を実行してファイルを取得します
     * 
     * @param url           取得するURL
     * @param baseDirectory 記録するディレクトリ名
     * @return サーバーの応答コード
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
            // メッセージ
        	urlToGet = (baseDirectory + targetFileName);
            updateBusyMessage("準備 : " + urlToGet);
            
            // 書き出しファイルの準備...
            dfc = (FileConnection) Connector.open(urlToGet, Connector.READ_WRITE);
            try
            {
                if (dfc.exists() == true)
                {
                    // ファイルが存在する場合には削除する
                    dfc.delete();
                }
            }
            catch (Exception ex)
            {
            	// 何もしない...
            	updateBusyMessage("Exception " + ex.getMessage());
            }
            try
            {
                // ファイル作成
                dfc.create();
            }
            catch (Exception ex)
            {
            	// 何もしない...
            	updateBusyMessage("Exception " + ex.getMessage());
            }
            
            // プラットフォーム名称を調べ、WX310だった場合には、gzip転送対策を行う
            urlToGet = url;
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
                updateBusyMessage("受信 : " + receiveByte);
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
            updateBusyMessage("完了 : " + receiveByte);
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
            updateBusyMessage("通信エラー ");
        }
        data = null;
        if (displayForm != null)
        {
            // ログ出力の終了通知
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
