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
 * HTTP通信ユーティリティ<br>
 * a2Bのhttp通信モジュールをa2B frameworkにポートしたものです。
 * MidpFileHttpCommunicationと比較し、proxy経由の通信、http通信の分割取得、
 * レンジ指定取得が可能です。<br>
 * この拡張を行うために、IMidpA2BhttpCommunicatorインタフェースを実装したクラスが
 * 必要となっています。
 * 
 * @see jp.sourceforge.nnsi.a2b.frameworks.ILogMessageDisplay
 * @see jp.sourceforge.nnsi.a2b.utilities.MidpFileHttpCommunication
 * @see IMidpA2BhttpCommunicator
 * @author MRSa
 *
 */
public class MidpA2BhttpCommunication
{
    private final int     TEMP_BUFFER_SIZE        = 20480;  // 受信データバッファサイズ、当初は 32768 にしていた
    private final int     MARGIN                  = 10;     // 確保するバッファのマージン （保険...本来は不要）
    
    private IMidpA2BhttpCommunicator parent        = null;   // 通信用インタフェースクラス
    private ILogMessageDisplay       displayForm   = null;   // ログ出力中のメッセージ表示

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
     *   コンストラクタでは、通信用インタフェース、ログ出力用インタフェースを記憶します。<br>
     * ログ出力用インタフェースは、nullでもかまいません（nullを指定した場合には、通信時の進捗表示、
     * エラー表示ができません。
     * 
     * @param object 通信用インタフェース
     * @param screen ログ出力用インタフェース
     * @see IMidpA2BhttpCommunicator
     */
    public MidpA2BhttpCommunication(IMidpA2BhttpCommunicator object, ILogMessageDisplay screen)
    {
        parent = object;
        displayForm = screen;
    }

    /**
     *   http GET通信の実行処理、通信成功/失敗の詳細は別メソッドで通知します（関数を呼び出します）<br>
     *   なお、このクラスを利用して同時に複数の通信はできません。
     * 
     *   @param fileName   取得したデータを記録するファイル名（ベースディレクトリからの絶対パス）
     *   @param url        取得するURL
     *   @param reference  送信する参照URL(リファラURL、nullの場合には指定省略)
     *   @param appendMsg  取得中に画面表示するメッセージ(追加文字列)
     *   @param offset     送信するオフセット(取得する先頭バイトの指定、負の場合には指定省略)
     *   @param range      送信するレンジ (取得データサイズの指定、負の場合には指定省略)
     *   @param mode       ファイルに記録するとき追記するか、上書きするか (trueの時には追記モード)
     *   @return           <code>true</code>:通信成功 / <code>false</code>:通信失敗
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
            // 文字列モード
            appendMode = IMidpA2BhttpCommunicator.APPENDMODE_STRING;
            offset = range;    
        }

        // ログのビジー表示を行う
        startLogMessage();
        updateLogMessage("http通信準備中 :" + url);

        // 通信の実行前にファイルを開ける
        boolean ret = openFileConnection(getFileName, appendMode);
        if (ret != true)
        {
            return (false);
        }

        // HTTP通信実処理...
        if (parent.GetDivideGetHttp() == false)
        {
            // スレ通常取得モード
            setParameter(url, reference, appendMsg, offset, appendMode, true);
            getURLUsingHttp();
        }
        else
        {
            // スレ分割取得モード
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

        // ファイルをクローズさせる
        closeFileConnection();
        
        // ログのビジー表示を終了させる
        finishLogMessage();
        return (true);
    }

    /**
     *  最後に取得した応答コードを取得する
     * @return 応答コード
     */
    public int getLastResponseCode()
    {
    	return (responseCode);
    }

    /**
     *  受信したデータを記録したかどうか
     * @return <code>true</code> 受信したデータを記録した   <code>false</code> 記録していない
     */
    public boolean isReceived()
    {
    	return (isRecieved);
    }
    
    /**
     *   ログメッセージの出力を開始する
     */
    private void startLogMessage()
    {
        if (displayForm != null)
        {
            displayForm.outputInformationMessage("", true, false);
        }
    }

    /**
     *  ログメッセージを出力する
     * @param message 出力するメッセージ
     */
    private void updateLogMessage(String message)
    {
        if (displayForm != null)
        {
            displayForm.outputInformationMessage(message, false, false);
        }
    }

    /**
     *  ログメッセージの出力を終了する
     */
    private void finishLogMessage()
    {
        if (displayForm != null)
        {
            displayForm.finishInformationMessage();
        }
    }

    /**
     *  ユーザーエージェント名を応答する
     * @return ユーザーエージェント名
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
     *  ファイルのオープン処理
     * 
     * @param dataFileName オープンするファイル名
     * @param mode オープンモード
     * 
     * @return <code>true</code>:ファイルオープンOK / <code>false</code>:ファイルオープンNG
     */
    private boolean openFileConnection(String dataFileName, int mode)
    {
        dataFileConnection = null;
        if (mode == IMidpA2BhttpCommunicator.APPENDMODE_STRING)
        {
            // データストリング格納モード...
            return (true);
        }

        try
        {
            // ファイルをオープンする
            dataFileConnection = (FileConnection) Connector.open(dataFileName, Connector.READ_WRITE);
            if (dataFileConnection.exists() != true)
            {
                // ファイル作成
                dataFileConnection.create();
            }
            if (mode == IMidpA2BhttpCommunicator.APPENDMODE_NEWFILE)
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
     *  ファイルをクローズする
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
            // 例外発生時には何もしない
        }
        dataFileConnection  = null;
        dataFileSize        = -1;
        startRange          = -1;
        endRange            = -1;
        getMessageString    = null;
        
        // 応答を返す
        parent.CompletedToGetHttpData(responseCode, bytesRead);

        System.gc();
        return;
    }

    /**
     *   HTTP接続を使ってファイルに出力する
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
        // パラメータを設定する
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
                // とりあえず、大きな値にする...
                dataFileSize = 2147483600;
            }
        }
        return;
    }

    /**
     *  取得URLから、HTTP要求ヘッダを生成（変換）する
     *  ※ WX310用のワークアラウンド
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
            // レンジ指定がされているかどうかチェックする
            if ((startRange > 0)||((startRange == 0)&&(divideCommunication == true)))
            {
                // Range指定があった場合... (絶対値指定)
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
                // Range指定があった場合... (相対値指定)
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
     *   HTTPの要求ヘッダを設定する 
     * 
     * @param connector
     * @param proxyUsage
     */
    private void setHttpConnectionParameter(HttpConnection connector, int proxyUsage)
    {
        if (proxyUsage == IMidpA2BhttpCommunicator.PROXY_WORKAROUND_WX310)
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
            if (appendMode != IMidpA2BhttpCommunicator.APPENDMODE_STRING)
            {
                // Range指定があった場合...
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
            updateLogMessage("記録中 size： " + bytesRead + appendMessage);
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
            if (proxyUsage == IMidpA2BhttpCommunicator.PROXY_WORKAROUND_WX310)
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
            updateLogMessage("http通信中 " + getMessageString + "\n" + urlToGet);

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            responseCode = c.getResponseCode();
            if ((responseCode != HttpConnection.HTTP_OK)&&(responseCode != HttpConnection.HTTP_PARTIAL))
            {
                // close streams
                c.close();
                
                // 応答コードが416(Requested Range Not Satisfiable) 以外だった場合には、エラー表示。
                if (responseCode != 416)
                {
                    updateLogMessage("RESP NG (Code : " + responseCode + ") " + getMessageString + "\n" + urlToGet);
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
            updateLogMessage("http通信終了 "+ getMessageString + "\n" + urlToGet);

            // stream open
            InputStream is = c.openInputStream();
            if (appendMode == IMidpA2BhttpCommunicator.APPENDMODE_APPENDFILE)
            {
                // ファイル追記モード
                os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
            }
            else if (appendMode == IMidpA2BhttpCommunicator.APPENDMODE_NEWFILE)
            {
                // ファイルを上書きするモード
                os = dataFileConnection.openOutputStream();                
            }
            else  // if (appendMode == APPENDMODE_STRING)
            {
                // 出力ファイルをオープンしない...
            }

            // Get the ContentType
            //String type = c.getType();

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
        String nameOperator = "socket://" + parent.GetProxyUrl();
        try 
        {               
            // BUSYメッセージを設定する
            updateLogMessage("proxy要求送信準備中");

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
            updateLogMessage("http要求送信中\n " + urlToGet);

            // データを送出する
            outputStream.write(urlToGet.getBytes());

            updateLogMessage("http応答受信中...");

            // 入力（受信）用ストリームをオープンする
            InputStream is = sockConn.openInputStream();

            if (appendMode == IMidpA2BhttpCommunicator.APPENDMODE_APPENDFILE)
            {
                // ファイル追記モード
                os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
            }
            else if (appendMode == IMidpA2BhttpCommunicator.APPENDMODE_NEWFILE)
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
     *   HTTP接続を使ってファイルに出力する （メイン処理）
     *   
     * @return <code>true</code>:通信成功 / <code>false</code>:通信失敗
     */
    private boolean getURLUsingHttp()
    {
        boolean result = false;

        // ファイルを出力するコネクタがオープンされていなければ終了する
        if ((dataFileConnection == null)&&(appendMode != IMidpA2BhttpCommunicator.APPENDMODE_STRING))
        {
            // 通信エラーを表示する
            updateLogMessage("CONNECTION(ERROR) Connection : null");
            
            // 通信失敗時 (CLUB AIR-EDGE , 確認したときにNoを選択した場合...)
            return (result);
        }

        // ビジー表示を更新する
        updateLogMessage("HTTP通信準備中 " + urlToGet);

        // Proxyの使用設定を取得する
        int proxyUsage = parent.GetProxyMode();

        // PROXYを使用するかどうかの判定を行う。
        if (proxyUsage == IMidpA2BhttpCommunicator.PROXY_DO_USE_PROXY)
        {
            // Proxy経由の通信を行う
            result = executeProxyHttpCommunication(proxyUsage);
        }
        else
        {
            // 通常のHTTP通信を行う
            result = executeHttpCommunication(proxyUsage);
        }
        return (result);
    }
}
