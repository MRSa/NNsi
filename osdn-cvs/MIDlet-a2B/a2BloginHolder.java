import java.io.InputStream;
import java.io.OutputStream;
//import java.io.DataInputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.HttpsConnection;
import javax.microedition.io.HttpConnection;
//import javax.microedition.io.file.FileConnection;

/**
 * a2Bのログインユーティリティクラス
 * 
 * @author MRSa
 *
 */
public class a2BloginHolder
{
    private a2BMain    parent    = null;
//    private a2Butf8Conveter kjConverter = null;
    private final int BUFFER_SIZE  = 1024;
    private String     resultMsg    = null;
    private int        bodyLength   = 0;
    private int        actualLength = 0;
    
    /**
     * コンストラクタ
     * 
     * @param object
     */
    public a2BloginHolder(a2BMain object, a2Butf8Conveter utfConv)
    {
        parent = object;
//        kjConverter = utfConv;
    }

    /**
     *  ログインの実行する
     * @param name
     * @param password
     * @return
     */
    public boolean doOysterLogin(String name, String password)
    {
        boolean       ret = false;
        HttpsConnection c  = null;
        InputStream     is = null;
        OutputStream    os = null;
        String sendMessage = "ID=" + name + "&PW=" + password;
        String url = "https://2chv.tora3.net/futen.cgi";
        try 
        {
            resultMsg = "Unknown";
            c = (HttpsConnection) Connector.open(url);
            {
                c.setRequestMethod(HttpsConnection.POST);
                c.setRequestProperty("user-agent", "DOLIB/1.00");
                c.setRequestProperty("x-2ch-ua", "a2B/1.00");
//                c.setRequestProperty("accept", "text/html, */*");
//              c.setRequestProperty("content-language", " ja, en");
//                c.setRequestProperty("content-type", "application/x-www-form-url-encoded");
            }

            // Getting the output stream may flush the headers
            os = c.openOutputStream();
            os.write(sendMessage.getBytes());

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            int rc = c.getResponseCode();
            if (rc != HttpsConnection.HTTP_OK)
            {
                resultMsg = "NG:RESP" + rc;
                os.close();
                 c.close();
                return (false);
            }
/**
            //////////  Cookie の抽出を実施する  //////////
            int index = 0;
            try
            {
                //
            }
            catch (Exception e2)
            {
                //
            }
**/
            //////////  ボディから情報を引き抜く... /////////
            bodyLength  = (int)c.getLength();
            is = c.openInputStream();

            // Get the length and process the data
            byte[] data = new byte[BUFFER_SIZE];
            actualLength = is.read(data, 0, BUFFER_SIZE);
            if (actualLength <= 0)
            {
                os.close();
                is.close();
                c.close();
                resultMsg = "NG:BODY LENGTH (" + actualLength + ", len:" + bodyLength + ")";
                return (false);
            }
            ret = parseOysterRecieveData(data);
            os.close();
            is.close();
            c.close();
        }
        catch (Exception e)
        {
            resultMsg = e.getMessage();
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
        }
        return (ret);
    }

    /**
     * 受信データを解析する。。。
     * 
     * @param receiveData
     * @return
     */
    private boolean parseOysterRecieveData(byte[] receiveData)
    {
        int len = 0;
        while (receiveData[len] > ' ')
        {
            len++;
        }
        resultMsg = new String(receiveData, 0, len);
        int pos = resultMsg.indexOf(":");
        if (pos < 0)
        {
            resultMsg = " (NG応答)";
            return (false);
        }
        int pos2 = resultMsg.indexOf("=");
        if (pos2 < 0)
        {
            resultMsg = " (UA不明)";
            return (false);
        }
        int pos3 = resultMsg.indexOf("SESSION-ID=ERROR");
        if (pos3 == 0)
        {
            resultMsg = " (ID or PW間違い)";
            return (false);
        }

        // USER AGENT と SESSION-IDを記憶する
        String ua = resultMsg.substring((pos2 + 1), pos);
        String ss = resultMsg.substring((pos2 + 1));
        parent.setOysterSessionId(ua, ss);
        resultMsg = "";
        return (true);
    }

    /**
     * ログイン結果文字列
     * 
     * @return
     */
    public String getResultString()
    {
        return (resultMsg);
    }

    /**
     *  Be@2chログインの実行する
     * @param name
     * @param password
     * @return
     */
    public boolean doBe2chLogin(String name, String password)
    {
        boolean       ret = false;
        HttpConnection  c  = null;
        InputStream     is = null;
        OutputStream    os = null;
        String sendMessage = "m=" + a2BsjConverter.urlEncode(name.getBytes()) + "&p=" + a2BsjConverter.urlEncode(password.getBytes()) + "&submit=%C5%D0%CF%BF";
        String url = "http://be.2ch.net/test/login.php";
/***
        try
        {
            c = (HttpConnection) Connector.open(url);
            {
                c.setRequestMethod(HttpConnection.GET);
                c.setRequestProperty("User-Agent", "DOLIB/1.00");
                c.setRequestProperty("X-2ch-UA", "a2B/1.10");
                c.setRequestProperty("Accept", "text/html, text/plain");
                c.setRequestProperty("Referer", "http://be.2ch.net/test/login.php");
            }

            // Getting the output stream may flush the headers
            os = c.openOutputStream();

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            int rc = c.getResponseCode();
            if (rc != HttpConnection.HTTP_OK)
            {
                resultMsg = "NG:RESP" + rc;
                os.close();
                 c.close();
                return (false);
            }
            os.close();
            c.close();
            
            // 200ms ほど止まる...        
            parent.wait(200);
        }
        catch (Exception e)
        {
            
        }
***/
        os = null;
        c = null;
        try 
        {
            resultMsg = "Unknown";
            c = (HttpConnection) Connector.open(url);
            {
                c.setRequestMethod(HttpConnection.POST);
                c.setRequestProperty("Host", "be.2ch.net");
                c.setRequestProperty("Accept", "text/html, */*");
                c.setRequestProperty("Referer", "http://be.2ch.net/test/login.php");
                c.setRequestProperty("Content-Type", "application/x-www-form-urlencoded");
                c.setRequestProperty("User-Agent", "DOLIB/1.00");
                c.setRequestProperty("X-2ch-UA", "a2B/1.10");
            }

            // Getting the output stream may flush the headers
            os = c.openOutputStream();
            os.write(sendMessage.getBytes());
            os.flush();

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            int rc = c.getResponseCode();
            if (rc != HttpConnection.HTTP_OK)
            {
                resultMsg = "NG:RESP" + rc;
                os.close();
                 c.close();
                return (false);
            }

            //////////  ボディから情報を引き抜く... /////////
            bodyLength  = (int)c.getLength();
            is = c.openInputStream();

            // Get the length and process the data
            byte[] data = new byte[BUFFER_SIZE];
            actualLength = is.read(data, 0, BUFFER_SIZE);
            if (actualLength <= 0)
            {
                os.close();
                is.close();
                c.close();
                resultMsg = "NG:BODY LENGTH (" + actualLength + ", len:" + bodyLength + ")";
                return (false);
            }
            ret = parseBe2chRecieveData(data);
            os.close();
            is.close();
            c.close();
        }
        catch (Exception e)
        {
            resultMsg = e.getMessage();
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
        }
        return (ret);
    }
    /**
     *  受信データを解析する。。。
     * 
     * @param receiveData
     * @return
     */
    private boolean parseBe2chRecieveData(byte[] receiveData)
    {
        String receive = new String(receiveData);

        ///// 最初の Cookieを抽出
        int pos = receive.indexOf("document.cookie", 0);
        if (pos < 0)
        {
            // Cookieがない...
            resultMsg = "No Cookie";
            return (false);
        }
        
        char data = 0;
        pos = pos + 15; // document.cookie を読み飛ばし
        while ((receive.charAt(pos) != '\'')&&(pos < receive.length()))
        {
            pos++;
        }
        if (pos >= receive.length())
        {
            // 
            resultMsg = "1:pick cookie";
            return (false);
        }
        pos++;
        int bottom = pos;
        do
        {
            data = receive.charAt(bottom);
            bottom++;
        } while ((data != '\n')&&(data != '\r')&&(data != ';')&&(bottom < receive.length()));
        String cookie1 = receive.substring(pos, (bottom - 1));

        
        ///// ２番目の Cookieを抽出
        pos = receive.indexOf("document.cookie", bottom);
        if (pos < 0)
        {
            // Cookieがない...
            resultMsg = "2:No Cookie";
            parent.setBe2chCookie(cookie1);
            return (false);
        }
        pos = pos + 15; // document.cookie を読み飛ばし...
        while ((receive.charAt(pos) != '\'')&&(pos < receive.length()))
        {
            pos++;
        }
        if (pos >= receive.length())
        {
            // 
            resultMsg = "2:pick cookie";
            return (false);
        }
        pos++;
        bottom = pos;
        byte[] cookieData = new byte[(receive.length() - pos)];
        int dataIndex = 0;
        do
        {
            data = receive.charAt(bottom);
            if ((data != '\'')&&(data != ' ')&&(data != '+'))
            {
                cookieData[dataIndex] = (byte) data;
                dataIndex++;
            }
            bottom++;
        } while ((data != '\n')&&(data != '\r')&&(data != ';')&&(bottom < receive.length()));
        String cookie2 = new String(cookieData, 0, (dataIndex - 1));

        // Cookieを記憶する
        parent.setBe2chCookie(cookie1 + "; " + cookie2);
        
        resultMsg = "";
        return (true);
    }
}

//--------------------------------------------------------------------------
//  a2BloginFolder  --- HTTP通信クラス(●用)
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
