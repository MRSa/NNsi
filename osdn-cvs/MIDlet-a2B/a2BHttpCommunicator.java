import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.SocketConnection;
import javax.microedition.io.file.FileConnection;

/**
 * a2BHttpCommunicator : HTTP通信クラス
 * 
 * @author MRSa
 *
 */
public class a2BHttpCommunicator
{
    public  final int     APPENDMODE_STRING       = 2;      // String型にデータを格納する
    public  final int     APPENDMODE_APPENDFILE   = 1;      // ファイルに追記
    public  final int     APPENDMODE_NEWFILE      = 0;      // ファイルを新たに作成して記録する

    private final int     TEMP_BUFFER_SIZE        = 20480;  // 受信データバッファサイズ、当初は 32768 にしていた
//    private final int     DIVIDE_GET_SIZE         = 50000;  // スレ分割取得時の１回分取得サイズ
//    private final int     DIVIDE_GET_SIZE         = 30500;  // スレ分割取得時の１回分取得サイズ
    private final int     MARGIN                  = 10;     // 確保するバッファのマージン （保険...本来は不要）
    
    private a2BMain        parent                 = null;   // 親クラス (a2BMainクラス)
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
     *   コンストラクタ
     * 
     * @param object
     */
    public a2BHttpCommunicator(a2BMain object)
    {
        parent = object;
    }

    /**
     *   通信中かどうかを取得する
     * 
     * @return
     */
    public boolean isCommunicating()
    {
        return (communicating);
    }

    /**
     * 分割取得サイズを取得する
     * 
     * @return
     */
    private int getDivideGetSize()
    {
        return (parent.getDivideGetSize());
    }

    /**
     *  ファイルのオープン処理
     * 
     * @param dataFileName
     * @param mode
     * 
     * @return
     */
    public boolean openFileConnection(String dataFileName, int mode)
    {
        // 通信中に設定する
        communicating = true;

        dataFileConnection = null;
        if (mode == APPENDMODE_STRING)
        {
            // データストリング格納モード...
            return (true);
        }

        try
        {
            dataFileConnection = (FileConnection) Connector.open(dataFileName, Connector.READ_WRITE);
            if (dataFileConnection.exists() != true)
            {
                // ファイル作成
                dataFileConnection.create();
            }
            if (mode == APPENDMODE_NEWFILE)
            {
                // 新規取得モードのときは、削除してからファイルを作成する
                dataFileConnection.delete();
                dataFileConnection.create();
            }
            dataFileSize = (int) dataFileConnection.fileSize();
        }        
        catch (Exception e)
        {
            // 例外発生！！！
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
     *  ファイルをクローズする
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
            // 例外発生時には何もしない
        }
        dataFileConnection  = null;
        dataFileSize        = -1;
        startRange          = -1;
        endRange            = -1;
        communicating       = false;
        divideCommunication = false;
        getMessageString    = null;
        
        // 応答を返す
        parent.CompletedToGetHttpData(httpScene, responseCode, bytesRead);

        System.gc();
        return;
    }
    
    /**
     *   HTTP接続を使ってファイルに出力する
     *   
     *       @param url：取得先URL
     *       @param reference
     *       @param appendString
     *       @param range
     *       @param scene
     *       @param mode
     *       @param isDivide
     */
    public void setParameter(String url, String reference, String appendString, int range, int scene, int mode, boolean isDivide)
    {
        // パラメータを設定する
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
                // とりあえず、大きな値にする...
                dataFileSize = 2147483600;
            }
        }
        return;
    }

    /**
     *  取得URLから、HTTP要求ヘッダを生成（変換）する
     * 
     */
    private void updateToGetUrlForHttpHeader()
    {
        int start = urlToGet.indexOf("://");
        start = start + 3; // '://' の3文字分...
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
                // レスpreviewモードのとき...
                urlToGet = urlToGet + "Range: bytes=0-" + startRange + "\r\n";
            }
        }
        else
        {
            // レンジ指定がされているかどうかチェックする
            if ((startRange > 0)||((startRange == 0)&&(divideCommunication == true)))
            {
                // Range指定があった場合... (絶対値指定)
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
                // Range指定があった場合... (相対値指定)
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
     *   HTTPの要求ヘッダを設定する 
     * 
     * @param connector
     * @param proxyUsage
     */
    private void setHttpConnectionParameter(HttpConnection connector, int proxyUsage)
    {
        if (proxyUsage == parent.PROXY_WORKAROUND_WX310)
        {
            // PROXYについて、WX310用の通信設定だった場合には、ここで設定しない
            return;
        }

        try
        {
            // HTTPの通信用ヘッダを生成する...
            connector.setRequestMethod(HttpConnection.GET);
            if (referenceUrl != null)
            {
                connector.setRequestProperty("Referer", referenceUrl);
            }
            if (appendMode == APPENDMODE_STRING)
            {
                if (httpScene == parent.SCENE_PREVIEW_MESSAGE)
                {
                    // レスpreviewモードのとき...
                    String range = "bytes=" + "0-" + startRange;
                    connector.setRequestProperty("Range", range);                
                }
            }
            else
            {
                // Range指定があった場合...
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
            // パラメータが設定できなかった場合には、そのまま終了する。
        }
        return;
    }

    /**
     * ヘッダの最後尾を見つける
     * 
     * @param dataBuffer
     * @param length
     * 
     * @return
     */
    private int  lookupHttpBody(byte[] dataBuffer, int length)
    {
        //
        // TODO: 本来は、ここで応答コードを確認する必要あり！！
        //      (HTTP_OK あるいは HTTP_PARTIAL の場合のみ応答する)
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
     * 受信データをコピーする
     * 
     * @param inputDataStream    入力ストリーム
     * @param outputDataStream   出力ストリーム
     * @param contentLength  データサイズ
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
            parent.updateBusyMessage("記録中", "size： " + bytesRead + appendMessage, false);
            if (actual > 0)
            {
                // 初回に受信したデータの処理...
                if (first == true)
                {
                    first = false;

                    // 受信データにヘッダがくっついている場合...
                    if (hasHeader == true)
                    {
                        // コンテンツボディの先頭を見つける
                        contentsOffset = lookupHttpBody(data, actual);
                    }

                    // データの正当性をチェックする
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
                        // 通信終了
                        actual    = offset;
                        if (offset != -1)
                        {
                            // 通信エラーが発生した場合...
                            bytesRead = offset;
                        }
                        ret = false;
                    }
                }
                else
                {
                    // 2回目以降受信したデータ...ただデータを書いていくだけ...
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
     * HTTP通信の実行実処理
     * 
     * @param proxyUsage プロキシ利用モード
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
                // WX310SA(新ファーム)/WX310J向けの回避策...
                // URLを差し替える...
                updateToGetUrlForHttpHeader();
                urlToGet = urlToGet + "WX310";
            }

            // HTTPの接続オープン
            c = (HttpConnection) Connector.open(urlToGet);
 
            // HTTPのパラメータを設定する
            setHttpConnectionParameter(c, proxyUsage);

            // BUSYメッセージを設定する
            parent.updateBusyMessage("http通信中", "通信中 " + getMessageString + "\n" + urlToGet, false);

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            responseCode = c.getResponseCode();
            if ((responseCode != HttpConnection.HTTP_OK)&&(responseCode != HttpConnection.HTTP_PARTIAL))
            {
                // close streams
                c.close();
                
                // 応答コードが416(Requested Range Not Satisfiable) だった場合には、エラー表示。
                if (responseCode != 416)
                {
                    parent.updateBusyMessage("HTTP RESPONSE NG", "Code : " + responseCode + " " + getMessageString + "\n" + urlToGet, false);
                    parent.sleepTime(SLEEP_SECONDS);
                    bytesRead     = -1;
                }
                else
                {
                    // 応答コードが416の場合...
                    bytesRead = endRange;
                }
                c = null;
                System.gc();
                return (false);
            }
            parent.updateBusyMessage("http通信終了", "通信終了 "+ getMessageString + "\n" + urlToGet, false);

            // stream open
            InputStream is = c.openInputStream();
            if (appendMode == APPENDMODE_APPENDFILE)
            {
                // ファイル追記モード
                os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
            }
            else if (appendMode == APPENDMODE_NEWFILE)
            {
                // ファイルを上書きするモード
                os = dataFileConnection.openOutputStream();                
            }
            else  // if (appendMode == APPENDMODE_STRING)
            {
                // 出力ファイルをオープンしない...
            }

            // Get the ContentType
//          String type = c.getType();

            // 受信データをコピーする (受信データにHTTPヘッダは付加されていない)
            ret = copyReceivedData(is, os, false, "\n (Len: " + c.getLength() + ")");

            // ストリームを閉じる
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
     * プロキシ経由でのアクセス本処理...
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
            // BUSYメッセージを設定する
            parent.updateBusyMessage("proxyHttp通信", "要求送信準備中", false);

            // ソケットをオープンする
            SocketConnection sockConn = (SocketConnection) Connector.open(nameOperator, Connector.READ_WRITE);

            // オプションをばんばん設定する
            sockConn.setSocketOption(SocketConnection.KEEPALIVE, 1);
            sockConn.setSocketOption(SocketConnection.DELAY,     0);
            sockConn.setSocketOption(SocketConnection.LINGER,    0);
            
            // 出力（送信）用ストリームをオープンする
            OutputStream outputStream = sockConn.openOutputStream();

            // URLを差し替える...
            updateToGetUrlForHttpHeader();
            urlToGet = "GET " + urlToGet;

            // BUSYメッセージを設定する
            parent.updateBusyMessage("http送信中", "要求送信中\n " + urlToGet, false);

            // データを送出する
            outputStream.write(urlToGet.getBytes());

            parent.updateBusyMessage("http受信中", "応答受信中...", false);

            // 入力（受信）用ストリームをオープンする
            InputStream is = sockConn.openInputStream();

            if (appendMode == APPENDMODE_APPENDFILE)
            {
                // ファイル追記モード
                os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
            }
            else if (appendMode == APPENDMODE_NEWFILE)
            {
                // ファイルを上書きするモード
                os = dataFileConnection.openOutputStream();                
            }
            else  // if (appendMode == APPENDMODE_STRING)
            {
                // 出力ファイルをオープンしない...
                os = null;
            }

            // 受信データをコピーする (受信データにHTTPヘッダは付加されている！！)
            ret = copyReceivedData(is, os, true, "\n");

            // ストリーム群を閉じる
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
     *   HTTP接続を使ってファイルに出力する （メイン処理）
     *   
     * @return
     */
    public boolean getURLUsingHttp()
    {
        boolean result = false;

        // ファイルを出力するコネクタがオープンされていなければ終了する
        if ((dataFileConnection == null)&&(appendMode != APPENDMODE_STRING))
        {
            // 通信エラーを表示する
            parent.updateBusyMessage("CONNECTION(ERROR)", "Connection : null", false);
            parent.sleepTime(SLEEP_SECONDS);
            
            // 通信失敗時 (CLUB AIR-EDGE , 確認したときにNoを選択した場合...)
            return (result);
        }

        // ビジー表示を更新する
        parent.updateBusyMessage("HTTP通信準備中", urlToGet, false);        

        // Proxyの使用設定を取得する
        int proxyUsage = parent.getUsedProxyScene();
        boolean useProxy = false;

        // PROXYを使用するかどうかの判定を行う。
        if ((proxyUsage == parent.PROXY_USE_ALL)||(proxyUsage == parent.PROXY_USE_ONLYGET))
        {
            // proxy経由の通信を実施する
            useProxy = true;
        }
        else if (proxyUsage == parent.PROXY_USE_ONLYRANGE)
        {
            if (startRange != 0)
            {
                // Range指定がされていた場合には、Proxy経由の通信を実行する
                useProxy = true;
            }
        }

        // HTTPで通信するか、Proxy経由で通信するか、分岐をかける...
        if (useProxy == false)
        {
            // 通常のHTTP通信を行う
            result = executeHttpCommunication(proxyUsage);
        }
        else
        {
            // Proxy経由の通信を行う
            result = executeProxyHttpCommunication(proxyUsage);
        }
        return (result);
    }

    /**
     * 最後に指定した末尾レンジを取得する
     * 
     * @return
     */
    public int getLastEndRange()
    {
        return (endRange);
    }
}
//--------------------------------------------------------------------------
//  a2BHttpCommunicator  --- HTTP通信クラス
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
