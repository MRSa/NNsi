import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;

public class a2BDHttpCommunicator
{
	private a2BDsceneSelector        parent             = null;
	private FileConnection dataFileConnection = null;
	
	private int            dataFileSize       = -1;

	private final int     TEMP_BUFFER_SIZE   = 49008;
	private final int     DIVIDE_GET_SIZE    = 48000;
	private final int     MARGIN             = 10;

	private String         urlToGet           = null;
	private String         referenceUrl       = null;

	private int            startRange         = -1;
	private int            endRange           = -1;
	private int            httpScene          = -1;
	private int            lastReceivedSize   = -1;

	private boolean       communicating      = false;

	
	public  final int     APPENDMODE_STRING     = 2;
	public  final int     APPENDMODE_APPENDFILE = 1;
	public  final int     APPENDMODE_NEWFILE    = 0;
	private int            appendMode            = APPENDMODE_NEWFILE;

	//  コンストラクタ
	public a2BDHttpCommunicator(a2BDsceneSelector object)
	{
		parent = object;
	}

	//  ファイルのオープン処理
	private boolean openFileConnection(String dataFileName, int mode)
	{
		boolean ret = false;
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
			ret = true;
		}		
		catch (Exception e)
		{
			// 例外発生！！！
//			parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
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
			
			}
			ret = false;
		}
		return (ret);
	}

	//
	public boolean isOpen()
	{
		return (dataFileConnection.isOpen());
	}
	
	
	// オープン中ファイルのサイズを知る
	public int getFileSize()
	{
		return (dataFileSize);
	}

	//  ファイルをクローズする
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
		dataFileConnection = null;
		dataFileSize       = -1;

		return;
	}

	/*
	 *   通信中かどうかを取得する
	 * 
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
	public int getDivideGetSize()
	{
		return (DIVIDE_GET_SIZE);
	}
	
	/*
	 *   HTTP接続を使ってファイルに出力する
	 *   
	 *       @param urlToGet：取得先URL
	 *       @param 
	 * 
	 */
    public int getURLUsingHttp(String fileName, String url, String reference, int start, int scene, int mode, String userAgent, boolean isWorkAroundWX310)
    {
    	boolean ret = openFileConnection(fileName, mode);
    	if (ret == false)
    	{
    		return (-1);
    	}

    	startRange = start;
    	endRange   = -1;
    	if (startRange >= 0)
    	{
            endRange = startRange + DIVIDE_GET_SIZE;
    	}
    	
    	// パラメータを設定する
		urlToGet     = url;
    	if (isWorkAroundWX310 == true)
    	{
    		// WX310SA(新ファーム)/WX310J向けの回避策...
    		int startPos = urlToGet.indexOf("://");
    		startPos = startPos + 3;
    		int end   = urlToGet.indexOf("/", startPos);
    		String host = urlToGet.substring(startPos, end);
    		urlToGet = urlToGet + " HTTP/1.0\r\n";
            urlToGet = urlToGet + "Host: " + host + "\r\n";
    		urlToGet = urlToGet + "User-Agent: " + userAgent + "\r\n";
    		urlToGet = urlToGet + "Accept-Encoding: identity\r\n";
    		urlToGet = urlToGet + "Content-Language: ja, en\r\n";
			urlToGet = urlToGet + "Connection: Close\r\n";
    		urlToGet = urlToGet + "Pragma: no-cache\r\n";

			if (reference != null)
			{
				urlToGet = urlToGet + "Referer: " + reference + "\r\n";
			}
			if (startRange >= 0)
		    {
                // Range指定があった場合...
			    urlToGet = urlToGet + "Range: bytes=" + startRange + "-";
	            if (endRange > 0)
	            {
	            	urlToGet = urlToGet + endRange;
	            }
	            urlToGet = urlToGet + "\r\n";
		    }
			
    		urlToGet = urlToGet + "\r\nWX310";    		
    	}

    	referenceUrl = reference;
		httpScene    = scene;
		appendMode   = mode;

		// HTTP通信の実行、、、
		int result = getURLUsingHttpMain(userAgent, isWorkAroundWX310);

		// ファイルのクローズ
		closeFileConnection();

		// 最後にガベコレも実施
		System.gc();

		return (result);
	}

	/*
	 *   HTTP接続を使ってファイルに出力する （メイン処理）
	 *   
	 */
    private int getURLUsingHttpMain(String userAgent, boolean isWorkAroundWX310)
    {
		HttpConnection c  = null;
        InputStream    is = null;
		OutputStream   os = null;
        int           rc = -1;
        int    bytesread = -1;

		// ファイルを出力するコネクタがオープンされていなければ終了する
		if ((dataFileConnection == null)&&(appendMode != APPENDMODE_STRING))
		{
			return (rc);
		}
		
		// 通信中は何もせず応答する
		if (communicating == true)
		{
			return (rc);
		}

		communicating = true;
		String message = urlToGet;
		parent.showBusyMessage("http通信準備中", message, "", "");		
        try 
		{
			c = (HttpConnection) Connector.open(urlToGet);

			if (isWorkAroundWX310 != true)
			{
				if (referenceUrl != null)
				{
					c.setRequestProperty("Referer", referenceUrl);
				}
				if (appendMode == APPENDMODE_STRING)
				{
					// previewモードのとき...
					String range = "bytes=" + "0-" + startRange;
					c.setRequestProperty("Range", range);				
				}
				else
				{
					// Range指定があった場合...
					if (startRange >= 0)
					{
						String range = "bytes=" + startRange + "-";
						if (endRange > 0);
						{
							range = range + endRange;
						}
						c.setRequestProperty("Range", range);
					}
				}
				c.setRequestProperty("Accept", "text/html, image/jpeg, */*");
				c.setRequestProperty("Accept-Encoding", "identity");
	            c.setRequestProperty("Content-Language", " ja, en");
	            c.setRequestProperty("User-Agent", userAgent);				
			}

			message = "通信中\n " + urlToGet;
			parent.showBusyMessage("http通信中", message, "", "");

			// Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            rc = c.getResponseCode();
            if ((rc != HttpConnection.HTTP_OK)&&(rc != HttpConnection.HTTP_PARTIAL))
			{
				communicating = false;
				bytesread     = -1;

				// close streams
				//os.close();
				//is.close();
				c.close();

				parent.hideBusyMessage();
				parent.CompletedToGetHttpData(httpScene, rc, lastReceivedSize);
				lastReceivedSize = 0;
				return (rc);
            }
			message = "通信終了\n  " + urlToGet;
			parent.showBusyMessage("http通信終了", message, "", "");

			// stream open
            is = c.openInputStream();
			if (appendMode == APPENDMODE_APPENDFILE)
			{
				// ファイル追記モード
				os = dataFileConnection.openOutputStream(dataFileSize + MARGIN);
			}
			else if (appendMode == APPENDMODE_NEWFILE)
			{
				os = dataFileConnection.openOutputStream();
				lastReceivedSize = 0;
			}
			else  // if (appendMode == APPENDMODE_STRING)
			{
				// 出力ファイルをオープンしない...
			}

            // Get the length and process the data
			boolean first = true;
			int actual = 10;
            byte[] data = new byte[TEMP_BUFFER_SIZE + MARGIN];
			bytesread = 0;
			while (actual > 0)
			{
		        long contentLength = c.getLength();
		        String leng = "(Content Length : " + contentLength + ")";
				actual = is.read(data, 0, TEMP_BUFFER_SIZE);
				message = " データサイズ： " + bytesread;
				parent.showBusyMessage("データ記録中", message, leng, "");
				if (actual > 0)
				{
					if (first == true)
					{
						// 初回に受信したデータ...
						first = false;
						int offset = parent.getHttpDataReceivedFirst(httpScene, rc, data, actual);
						if (offset >= 0)
						{
							if (os != null)
							{
								os.write(data, offset, (actual - offset));
							}
							bytesread = bytesread + (actual - offset);
						}
						else
						{
							// 通信終了
							actual = -1;
						}
					}
					else
					{
						// 2回目以降受信したデータ...
						if (os != null)
						{
							os.write(data, 0, actual);
						}
						bytesread = bytesread + actual;
					}
				}
            }
			data = null;

			// close streams
			if (os != null)
			{
				os.close();				
			}
			is.close();
			c.close();
        }
		catch (Exception e)
		{
			try
			{
				if (os != null)
				{
					os.close();
				}
			}
			catch (Exception e2)
			{
					
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
				
			}
			parent.CompletedToGetHttpData(httpScene, rc, lastReceivedSize);
			lastReceivedSize = 0;
			return (-2);
		}
		parent.hideBusyMessage();
		parent.showBusyMessage("", "", "", "");
		communicating = false;
		lastReceivedSize = lastReceivedSize + bytesread;
		parent.CompletedToGetHttpData(httpScene, rc, bytesread);
		return (rc);
    }	
}
//--------------------------------------------------------------------------
//  a2BHttpCommunicator  --- HTTP通信クラス
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
