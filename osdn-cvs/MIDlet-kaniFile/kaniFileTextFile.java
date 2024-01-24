import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 * テキストファイルを保持するクラス
 * 
 * @author MRSa
 *
 */
public class kaniFileTextFile
{
	private final int BUFFER_SIZE   = 32768;
	private final int BUFFER_MARGIN = 4;

	private Vector  lineData      = null;
	private String  dataFileName  = null;
	private byte[] temporaryByte = null;
	private int    temporarySize = 0;

	private int    currentTopLine      = 0;
	private int    currentSelectedLine = 0;
	private boolean isEdit            = false;
	private byte[]  copyBuffer         = null;
	
	/**
     * コンストラクタ...
     */
	public kaniFileTextFile()
	{
		// とりあえず、何もしない...
	}

	/**
	 * ファイルをオープンする
	 * 
	 * @param fileName
	 */
	public int openFile(String fileName)
	{
		FileConnection fileConnection = null;
		InputStream    is             = null;
		lineData                      = null;
		dataFileName                  = fileName;

		// ファイル読み込み実行前にガベコレ実施...
		System.gc();

		try
		{
			fileConnection = (FileConnection) Connector.open(dataFileName, Connector.READ);
			if (fileConnection.exists() != true)
			{
				// ファイルが存在しなかった、、、終了する
				fileConnection.close();
				return (-1);
			}
			is = fileConnection.openInputStream();
			if (is == null)
			{
				// input streamにつなぐことができなかった...終了する
				fileConnection.close();
				return (-1);
			}

			lineData           = new Vector();
			long dataFileSize = fileConnection.fileSize();
			byte[] buffer     = new byte[BUFFER_SIZE + BUFFER_MARGIN];
			while (dataFileSize > 0)
			{
			    int size = is.read(buffer, 0, BUFFER_SIZE);
			    if (size <= 0)
			    {
			        break;
			    }
			    parseData(buffer, size);
			    dataFileSize = dataFileSize - size;
			}
	        if (temporarySize > 0)
	        {
	        	// 最終行分コピーする
	    		byte[] line = new byte[temporarySize];
				System.arraycopy(temporaryByte, 0, line, 0, temporarySize);
				temporaryByte = null;
				temporarySize = 0;
				lineData.addElement(line);
	        }
			is.close();
			fileConnection.close();

			buffer         = null;
			is             = null;
			fileConnection = null;
			System.gc();
		}
		catch (Exception e)
		{
			// 例外発生！！！
			try
			{
				if (is != null)
				{
					is.close();
				}
				is = null;

				if (fileConnection != null)
				{
					fileConnection.close();
				}
				fileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (-1);
		}
		currentTopLine      = 0;
		currentSelectedLine = 0;
		isEdit            = false;
		return (lineData.size());
	}

	public boolean outputFile(String targetFileName)
	{
		if (targetFileName == null)
		{
			// ファイル名はそのまま使う...
			Thread thread = new Thread()
			{
				public void run()
				{
					backupFile();
					outputFileMain();
				}
			};
			thread.run();
			return (true);
		}

		// ファイル名を指定されたものに変更する
		dataFileName = targetFileName;
		Thread thread = new Thread()
        {
			public void run()
			{
				outputFileMain();
			}
		};
		thread.run();
		return (true);
	}
		
	private void outputFileMain()
	{
		FileConnection dstFileConnection = null;
		OutputStream  os = null;
		try
		{
			dstFileConnection = (FileConnection) Connector.open(dataFileName, Connector.READ_WRITE);
			if (dstFileConnection.exists() == true)
			{
				// ファイルが存在した、、、削除して作り直す
				dstFileConnection.delete();
			}
			dstFileConnection.create();

			os = dstFileConnection.openOutputStream();
			Enumeration lineEnum = lineData.elements();
			while (lineEnum.hasMoreElements())
			{
				byte[] line = (byte[]) lineEnum.nextElement();
				os.write(line);
				os.write('\n');
			}
			os.close();
			dstFileConnection.close();
			os = null;
			dstFileConnection = null;
			System.gc();
		}
		catch (Exception e)
		{
			// 例外発生！！！
			try
			{
				if (os != null)
				{
					os.close();
				}
				os = null;

				if (dstFileConnection != null)
				{
					dstFileConnection.close();
				}
				dstFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return;
		}
		isEdit = false;
		return;
	}
	
    /**
     * 行データを取得する
     * 
     * @param index
     * @return
     */
	public byte[] getLine(int index)
	{
		if ((index < 0)||(index >= lineData.size()))
		{
			return (null);
		}
		return ((byte[]) lineData.elementAt(index));
	}
	
	/**
	 * 行データを差し替える
	 * 
	 * @param data
	 * @param index
	 */
	public void replaceLine(byte[] data, int index)
	{
		isEdit = true;
		if ((index < 0)||(index >= lineData.size()))
		{
			// 末尾に追加する
			lineData.addElement(data);
			return;
		}

		// データを入れ替える
		lineData.removeElementAt(index);
		lineData.insertElementAt(data, index);
		return;
	}

	/**
	 * 一行挿入する
	 * 
	 * @param index
	 */
	public void insertLine(int index)
	{
		byte [] data = new byte[1];
		isEdit = true;
		if ((index < 0)||(index >= lineData.size()))
		{
			// 末尾に追加する
			lineData.addElement(data);
			return;
		}
		// データを挿入する
		lineData.insertElementAt(data, index);
		return;
	}

	/**
	 * 一行貼り付ける
	 * 
	 * @param index
	 */
	public void pasteLine(int index)
	{
	    if (copyBuffer == null)
	    {
	    	// 貼り付けバッファに何もない場合には、何もしない
	    	return;
	    }
		
		isEdit = true;
		if ((index < 0)||(index >= lineData.size()))
		{
			// 末尾に追加する
			lineData.addElement(copyBuffer);
			return;
		}
		// データを挿入する
		lineData.insertElementAt(copyBuffer, index);

		return;
	}

	/**
	 * 一行削除する
	 * 
	 * @param index
	 */
	public void deleteLine(int index)
	{
		isEdit = true;
		if ((index < 0)||(index >= lineData.size()))
		{
			// 何もしない...
			return;
		}

		// 削除データを保存する
		copyBuffer = (byte[]) lineData.elementAt(index);

		// データを削除する
		lineData.removeElementAt(index);
		
		if (index >= lineData.size())
		{
			currentSelectedLine = lineData.size() - 1;
		}		
		return;
	}

	/**
	 * URLを抽出する
	 * 
	 * @param index
	 * @return
	 */
	public String pickupUrl(int index)
	{
		if ((index < 0)||(index >= lineData.size()))
		{
			// 何もしない...
			return (null);
		}

		// 行データを抽出する
		String buffer = new String((byte[]) lineData.elementAt(index));
		int pos = buffer.indexOf("ttps://");
		if (pos < 0)
		{
			pos = buffer.indexOf("ttp://");
		}
		if (pos >= 0)
		{
			int endUrl = pos;
			char checkChar = buffer.charAt(endUrl);
			while ((checkChar > ' ')&&(checkChar != '>')&&(checkChar != '"')&&(checkChar != '\''))
			{
				endUrl++;
				try
				{
                    checkChar = buffer.charAt(endUrl);
				}
				catch (Exception e)
				{
					break;
				}
			}
			return (new String("h" + buffer.substring(pos, endUrl)));			
		}
		return (null);
	}
	
	/**
	 *  データをクリアする
	 * 
	 *
	 */
	public void cleanupDatas()
	{
		lineData      = null;
		dataFileName  = null;
		temporaryByte = null;
		
		System.gc();
		return;
	}
	
	/**
	 * データの解析 (1行づつ切り出す)
	 * 
	 * @param buffer
	 * @param length
	 */
	private void parseData(byte[] buffer, int length)
    {
        int startPos = 0;
		int index    = 0;
        while (index < length)
        {
        	if (buffer[index] == '\n')
        	{
        		pickuplineData(buffer, startPos, (index - startPos));
        		startPos = index + 1;
        	}
        	index++;
        }

        // データの余りを記憶する...
        temporarySize = length - startPos;
        if (temporarySize > 0)
        {
            temporaryByte = new byte[temporarySize];
		    System.arraycopy(buffer, startPos, temporaryByte, 0, temporarySize);
        }
        else
        {
        	temporarySize = 0;
        	temporaryByte = null;
        }
		return;
	}

	/**
	 * 一行のデータを切り出す
	 * 
	 * @param buffer
	 * @param start
	 * @param length
	 */
	private void pickuplineData(byte[] buffer, int start, int length)
	{
		if (length < 0)
		{
			return;
		}
		byte[] line = new byte[temporarySize + length];
		if (line == null)
		{
			return;
		}
		if (temporarySize > 0)
		{
			System.arraycopy(temporaryByte, 0, line, 0, temporarySize);
			temporaryByte = null;
		}
		System.arraycopy(buffer, start, line, temporarySize, length);
		temporarySize = 0;
		lineData.addElement(line);
		
		return;
	}

	/**
	 * ファイルをバックアップ(というかリネームする)
	 * 
	 * @return
	 */
    private boolean backupFile()
    {
		// ファイルをバックアップ(というかリネーム)する
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(dataFileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() == true)
			{
				// ファイルが存在したとき、リネームする
				FileConnection conn = (FileConnection) Connector.open(dataFileName + ".bak", Connector.READ_WRITE);
				if (conn.exists() == true)
				{
					conn.delete();
				}
				conn.close();				
				int pos = dataFileName.lastIndexOf('/');
			    dataFileConnection.rename(dataFileName.substring(pos + 1) + ".bak");
			}
			dataFileConnection.close();
		}
		catch (Exception e)
		{
			try
			{
				if (dataFileConnection != null)
				{
					dataFileConnection.close();
				}
				dataFileConnection = null;
			}
			catch (Exception e2)
			{
				//
			}
			return (false);
		}
		return (true);
    }
    
    public int getCurrentSelectedLine()
    {
    	return (currentSelectedLine);
    }

    public void setCurrentSelectedLine(int index)
    {
    	currentSelectedLine = index;
    	return;
    }

    public int getCurrentTopLine()
    {
    	return (currentTopLine);
    }

    public void setCurrentTopLine(int index)
    {
    	currentTopLine = index;
    	return;
    }

    public int getTotalLine()
    {
    	return (lineData.size());
    }
    
    public boolean isModified()
    {
    	return (isEdit);
    }
}
