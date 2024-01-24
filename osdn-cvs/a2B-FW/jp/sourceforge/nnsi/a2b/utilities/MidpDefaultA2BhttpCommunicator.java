package jp.sourceforge.nnsi.a2b.utilities;

/**
 *  IMidpA2BhttpCommunicatorクラスのサンプル実装です。<br>
 *  MidpA2BhttpCommunication クラスで利用するものです。
 * 
 * @see jp.sourceforge.nnsi.a2b.utilities.IMidpA2BhttpCommunicator
 * @see jp.sourceforge.nnsi.a2b.utilities.MidpA2BhttpCommunication
 * @author MRSa
 */
public class MidpDefaultA2BhttpCommunicator implements IMidpA2BhttpCommunicator
{
    boolean isGet2chAppendMode = false;
	String  userAgent = "Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3";
	String  cookie = null;

	/**
	 *  2ch差分取得モードを設定する
	 *  
	 *  @param get2chAppendMode 
	 *      <code>true</code>:２ちゃん差分取得モード
	 *      <code>false</code>:通常モード
	 */
    public MidpDefaultA2BhttpCommunicator(boolean get2chAppendMode)
    {
    	isGet2chAppendMode = get2chAppendMode;
    }

    /**
     *  プロキシ通信モードを確認する<br>
     *  (使用しないモードを応答する)
     * @return プロキシ通信モード
     */
    public int GetProxyMode()
    {
    	return (IMidpA2BhttpCommunicator.PROXY_DO_NOT_USE_PROXY);
    }
    
    /**
     *  プロキシ通信時のホスト名とポート番号を応答する<br>
     *  (何も応答しない)
     * @return プロキシ通信時のホスト名：ポート番号
     */
    public String GetProxyUrl()
    {
    	return ("");
    }
    
	/** スレ分割取得サイズを取得する
     * @return スレ分割取得サイズ(30500固定)
     */
	public int GetDivideGetSize()
	{
		return (30500);
	}
	
	/**
	 *   スレ分割取得モードを使用するか？
	 * 
	 * @return <code>false</code>: <b>分割取得モードは利用しない </b>
	 */
	public boolean GetDivideGetHttp()
	{
		return (false);
	}

    /**
     *  ユーザーエージェント名を取得する
     * @param ua ユーザーエージェント
     */
	public void SetUserAgent(String ua)
	{
		userAgent = ua;
	}
	
    /**
     *  ユーザエージェントを応答する
     * @return ユーザエージェント名
     */
    public String GetUserAgent()
    {
    	return (userAgent);
    }
    
    
    /**
     *  cookieを設定する
     * 
     * @param cookieToSet 設定する cookie
     */
    public void SetCookie(String cookieToSet)
    {
        cookie = cookieToSet;
    }

    /**
     *  cookieを応答する
     *  @return cookie
     */
    public String GetCookie()
    {
    	return (cookie);
    }
    
    /**
     *  初回データを確認し、データの受信を継続する確認する
     * 
     * @param rcCode  応答コード
     * @param isDivideReceive 分割取得実行
     * @param data データ
     * @param contentsOffset データ読み出しオフセット
     * @param readSize データサイズ
     * @return 読み飛ばしサイズ (負の場合には通信中断)
     */    
    public int GetHttpDataReceivedFirst(int rcCode, boolean isDivideReceive, byte[] data, int contentsOffset, int readSize)
    {
        if (isGet2chAppendMode == false)
        {
            // 2ちゃんねるのスレ取得モードではない場合...
        	return (contentsOffset);
        }
    	if (isDivideReceive == false)
        {
            // スレ分割取得時の２回目以降のリクエストだった場合...
            return (contentsOffset);
        }

        if ((readSize == 1)&&(data[contentsOffset] == (byte) 10))
        {
            // スレ更新なし...
            return (-1);
        }
        if (data[contentsOffset] != (byte) 10)
        {
            // データ取得エラー...
            return (-10);
        }

        // 差分取得を実行！
        return (contentsOffset + 1);
    }
    
    /**
     *  受信したデータを通知する（APPENDMODE_STRINGモード時のときに呼び出す）
     * @param data     受信データ格納領域
     * @param offset   受信データのオフセット
     * @param dataSize 受信データのサイズ
     */
    public void ReceivedData(byte[] data, int offset, int dataSize)
    {
        return;
    }

    /**
	 *  HTTP通信の終了を通知する
	 * @param responseCode 応答コード
	 * @param bytesRead 読み込んだバイト数
	 */
	public void CompletedToGetHttpData(int responseCode, int bytesRead)
	{
        return;
	}
}
