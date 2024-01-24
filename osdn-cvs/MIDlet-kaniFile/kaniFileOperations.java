import java.util.Vector;
import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Image;

/**
 * ファイル操作をまとめたクラス...
 * 
 * @author MRSa
 *
 */
public class kaniFileOperations
{
	private final int COPY_BUFFER_SIZE = 32768;
	private final int BUFFER_MARGIN    = 4;
    private kaniFileSceneSelector parent = null;
	
	/**
     * コンストラクタ...
     */
	public kaniFileOperations(kaniFileSceneSelector selector)
	{
		// とりあえず、何もしない...
		parent = selector;
	}

	/**
     *  ファイルの存在を確認する
     * 
     * @param targetFileName 確認するファイル名
     * @return <code>true</code>:存在する / <code>false</code>:存在しない
     */
    static public boolean IsExistFile(String targetFileName)
    {
        boolean       ret = false;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            ret = dataFileConnection.exists();
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
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
            ret = false;
        }
        return (ret);
    }

	/**
     * ディレクトリの作成
     * @param directoryName ディレクトリ名 (full path)
     */
	static public boolean MakeDirectory(String directoryName)
	{
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
           int index = 9;
           while (true)
           {
               String tempDirectory = new String(directoryName);
               int pos = tempDirectory.indexOf("/", index);
               if ((pos < 0)||(pos >= tempDirectory.length()))
               {
            	   // 末尾までたどった...抜ける
            	   break;
               }
               String testDirectory = tempDirectory.substring(0, pos + 1);
		       index = pos + 1;
               dataFileConnection = (FileConnection) Connector.open(testDirectory, Connector.READ_WRITE);
			   if (dataFileConnection.exists() != true)
			   {
                    // ファイルが存在しなかったとき、ディレクトリを作成する
			        dataFileConnection.mkdir();
			   }
			   dataFileConnection.close();
			   dataFileConnection = null;
			}

            // 最後に本命を対応...
			dataFileConnection = (FileConnection) Connector.open(directoryName, Connector.READ_WRITE);
			if (dataFileConnection.exists() != true)
			{
				// ファイルが存在しなかったとき、ディレクトリを作成する
			    dataFileConnection.mkdir();
			    ret = true;
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
		return (ret);
	}

    /**
     * ディレクトリの作成
     * @param directoryName ディレクトリ名 (full path)
     */
/*
	public boolean makeDirectory(String directoryName)
	{
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(directoryName, Connector.READ_WRITE);
			if (dataFileConnection.exists() != true)
			{
				// ファイルが存在しなかったとき、ディレクトリを作成する
			    dataFileConnection.mkdir();
			    ret = true;
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
		return (ret);
	}
*/

	/**
	 * ファイルの削除
	 * @param targetFileName 削除するファイル (full path)
	 */
	public boolean deleteFile(String targetFileName)
	{
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() == true)
			{
				// ファイルがあったときには、削除を実行する
				dataFileConnection.delete();
				ret = true;
			}
			dataFileConnection.close();
		}
		catch (Exception e)
		{
			// 例外発生！！！
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
		return (ret);
	}

    /**
     * ファイルのリネーム
     * @param srcFileName  名前を変更するファイル (full path)
     * @param renameFile   変更後のファイル名     (ファイル名のみ)
     */
	public boolean renameFile(String srcFileName, String renameFile)
	{
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(srcFileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() == true)
			{
				// ファイルが存在したとき、リネームする
			    dataFileConnection.rename(renameFile);
			    ret = true;
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
		return (ret);
	}

	/**
	 * ファイルの属性変更。。。
	 * 
	 * @param fileName 属性を変更するファイル名
	 * @param writable 書き込み属性
	 * @param hidden   隠しファイル属性
	 * @return
	 */
    public boolean changeAttribute(String fileName, boolean writable, boolean hidden)
    {
		boolean       ret = false;
		FileConnection dataFileConnection = null;
		try
		{
			dataFileConnection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() == true)
			{
				// ファイルが存在したとき、属性を変更する
			    dataFileConnection.setWritable(writable);
			    dataFileConnection.setHidden(hidden);
			    ret = true;
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
		return (ret);
    }
	
    /**
     * ファイルのコピー
     * 
     * @param destFileName コピー先ファイル (full path)
     * @param srcFileName  コピー元ファイル (full path)
     */
	public boolean copyFile(String destFileName, String srcFileName)
	{
		FileConnection srcFileConnection = null;
		FileConnection dstFileConnection = null;
		
		boolean     ret = false;
		InputStream   is = null;
		OutputStream  os = null;

		if (destFileName == srcFileName)
		{
			// ファイル名が同じだった場合にはコピーを実行しない
			return (false);
		}
		
		try
		{
			srcFileConnection = (FileConnection) Connector.open(srcFileName, Connector.READ_WRITE);
			if (srcFileConnection.exists() != true)
			{
				// ファイルが存在しなかった、、、終了する
				srcFileConnection.close();
				return (false);
			}
			is = srcFileConnection.openInputStream();

			long dataFileSize = srcFileConnection.fileSize();
			byte[] buffer = new byte[COPY_BUFFER_SIZE + BUFFER_MARGIN];

			dstFileConnection = (FileConnection) Connector.open(destFileName, Connector.READ_WRITE);
			if (dstFileConnection.exists() == true)
			{
				// ファイルが存在した、、、削除して作り直す
				dstFileConnection.delete();
			}
			dstFileConnection.create();

			os = dstFileConnection.openOutputStream();
			if ((is != null)&&(os != null))
			{
				while (dataFileSize > 0)
				{
			        int size = is.read(buffer, 0, COPY_BUFFER_SIZE);
			        if (size <= 0)
			        {
			        	break;
			        }
			        os.write(buffer, 0, size);
				}
			}
			os.close();
			is.close();
			dstFileConnection.close();
			srcFileConnection.close();

			buffer = null;
			is = null;
			os = null;
			srcFileConnection = null;
			dstFileConnection = null;
			ret = true;
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
				
				if (os != null)
				{
					os.close();
				}
				os = null;
				
				if (srcFileConnection != null)
				{
					srcFileConnection.close();
				}
				srcFileConnection = null;

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
			return (false);
		}
		return (ret);
	}

	/**
	 *  ファイル一覧をhtmlファイルに出力する
	 * 
	 * @param fileName
	 * @param fileListString
	 * @return
	 */
	public boolean outputFileListAsHtml(String fileName, Vector fileListString)
	{
		boolean                     ret = false;
		OutputStream                  os = null;
		FileConnection dstFileConnection = null;

		try
		{
            dstFileConnection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
			if (dstFileConnection.exists() == true)
			{
				// ファイルが存在した、、、削除して作り直す
				dstFileConnection.delete();
			}
			dstFileConnection.create();

			os = dstFileConnection.openOutputStream();
			if (os != null)
			{
				// HTMLファイルの出力...
				os.write("<html>\r\n<head>\r\n<title>".getBytes());
				os.write(fileName.getBytes());
				os.write("</title>\r\n</head>\r\n<body>\r\n".getBytes());

				int count = fileListString.size();
				for (int loop = 0; loop < count; loop++)
				{
			        String targetFileName = (String) fileListString.elementAt(loop);
			        String item = "<a href=\"" + targetFileName + "\">" + targetFileName + "</a><br>\r\n";
			        os.write(item.getBytes());
				}
	            // 「kaniFilerへ戻る」タグを埋め込む。（対応機種のみ）
				String platForm = System.getProperty("microedition.platform");
				if ((platForm.indexOf("WS023T") >= 0)||
                     (platForm.indexOf("WS018KE") >= 0)||
                     (platForm.indexOf("WS009KE") >= 0)||
                     (platForm.indexOf("WX0") >= 0)||
                     (platForm.indexOf("WX33") >= 0)||
                     (platForm.indexOf("WX34") >= 0)||
                     (platForm.indexOf("WX35") >= 0))
		        {
		            String data = "<hr><object id=\"app\" declare=\"declare\" classid=\"x-oma-application:java-ams\">";
		            data = data + "\r\n" + "  <param name=\"AMS-Filename\" value=\"http://nnsi.sourceforge.jp/archives/midp/kaniFiler/kaniFile.jad\"/>";
		            data = data + "\r\n" + "  <param name=\"MIDlet-Name\" value=\""    + parent.getAppProperty("MIDlet-Name") + "\"/>";
		            data = data + "\r\n" + "  <param name=\"MIDlet-Version\" value=\"" + parent.getAppProperty("MIDlet-Version") + "\"/>";
		            data = data + "\r\n" + "  <param name=\"MIDlet-Vendor\" value=\""  + parent.getAppProperty("MIDlet-Vendor") + "\"/>";
		            data = data + "\r\n" + "  <param name=\"AMS-Startup\" value=\"download-confirm\"/>";
		            data = data + "\r\n" + "</object><a href=\"#app\">(" + parent.getAppProperty("MIDlet-Name") + "へ戻る)</a>\r\n";
	                os.write(data.getBytes());
		        }
                os.write("</body>\r\n</html>\r\n".getBytes());
			}
			os.close();
			dstFileConnection.close();
			os = null;
			dstFileConnection = null;
			ret = true;
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
			return (false);
		}
		return (ret);
	}
	
	
	/**
	 *  イメージファイル(png形式)を読み出し、メモリに展開する
	 * 
	 * @param fileName ファイル名 (full path)
	 * @return         イメージデータ
	 */
	public Image loadImageFromFile(String fileName)
	{
		Image          image      = null;
        FileConnection connection = null;
        InputStream    in         = null;
        byte[]        buffer     = null;
        try
		{
			// ファイルのオープン
			connection  = (FileConnection) Connector.open(fileName, Connector.READ);
			int size   = (int) connection.fileSize();

			// データを最初から size分読み出す...
			in      = connection.openInputStream();
			buffer  = new byte[size + BUFFER_MARGIN];
			int dataReadSize = in.read(buffer, 0, size);

			// ファイルのクローズ
			in.close();
			connection.close();

			// 壁紙データとしてイメージに変換...
            image = Image.createImage(buffer, 0, dataReadSize);
            
            // バッファをクリアする
            buffer = null;
		}
		catch (Exception e)
		{
			// 例外発生！！！
			try
			{
				if (in != null)
				{
					in.close();
				}
			}
			catch (Exception e2)
			{
				// (例外発生時には何もしない)
			}
			try
			{
				if (connection != null)
				{
					connection.close();
				}
			}
			catch (Exception e2)
			{
				// (例外発生時には何もしない)
			}
			
			// 読み込み失敗、データを破棄する
			buffer  = null;
			image   = null;
		}
		System.gc();
		return (image);
	}
	
}
