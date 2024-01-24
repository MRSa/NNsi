/**
 *  a2B Frameworkの外部にある、ユーティリティパッケージです。<br>
 * （a2B Frameworkとは切り離して）独立で利用可能になっています。
 */
package jp.sourceforge.nnsi.a2b.utilities;

/**
 *   MidpA2BhttpCommunication で使用する、インタフェースクラスです。<br>
 *  ユーザーエージェントのカスタマイズなど、このインタフェースを実装したクラスで行ってください。
 * 
 * @see MidpA2BhttpCommunication
 * @author MRSa
 */
public interface IMidpA2BhttpCommunicator
{
    /** 受信データをファイルに記録しない */
	static public  final int APPENDMODE_STRING       = 2;  // データをファイルに格納しない

	/** 受信データを、指定したファイルに追記する */
	static public  final int APPENDMODE_APPENDFILE   = 1;  // ファイルに追記

	/** 受信データを、指定したファイルに上書き記録する */
	static public  final int APPENDMODE_NEWFILE      = 0;  // ファイルを新たに作成して記録する

    /** Proxy通信モードを使用しない */
	static public final int PROXY_DO_NOT_USE_PROXY = 0;
	
	/** Proxy通信モード (1: WX310通信用回避策を使用する) */
    static public final int PROXY_WORKAROUND_WX310 = 1;

    /** Proxy通信モード (2: プロキシ通信を実行する) */
    static public final int PROXY_DO_USE_PROXY     = 2;

    /**
     *  プロキシ通信モードを確認する
     * @return プロキシ通信モード
     */
    public abstract int GetProxyMode();
    
    /**
     *   プロキシ通信時のホスト名とポート番号を応答する
     * @return プロキシ通信時のホスト名：ポート番号
     */
    public abstract String GetProxyUrl();
    
	/** スレ分割取得サイズを取得する
     * @return スレ分割取得サイズ
     */
	public abstract int GetDivideGetSize();
	
	/**
	 *   スレ分割取得モードを使用するか？
	 * 
	 * @return
	 * 　   <code>true</code>:分割取得モードを利用する
	 *      <code>false</code>:分割取得モードは利用しない
	 */
	public abstract boolean GetDivideGetHttp();

    /**
     *   ユーザエージェントを応答する
     * 
     * @return ユーザエージェント名
     */
    public abstract String GetUserAgent();

    /**
     *   Cookieを応答する
     * 
     * @return cookie
     */
    public abstract String GetCookie();
    
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
    public abstract int GetHttpDataReceivedFirst(int rcCode, boolean isDivideReceive, byte[] data, int contentsOffset, int readSize);

    /**
     * 受信したデータを通知する（APPENDMODE_STRINGモード時のときに呼び出す）
     * @param data     受信データ格納領域
     * @param offset   受信データのオフセット
     * @param dataSize 受信データのサイズ
     */
    public abstract void ReceivedData(byte[] data, int offset, int dataSize);

    /**
	 *   HTTP通信の終了を通知する
	 * @param responseCode 応答コード
	 * @param bytesRead 読み込んだバイト数
	 */
	public abstract void CompletedToGetHttpData(int responseCode, int bytesRead);
}
