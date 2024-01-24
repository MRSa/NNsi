package jp.sourceforge.nnsi.a2b.utilities;
import java.io.InputStream;
import java.io.OutputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 * JSR75(FileConnection API)のファイル操作ユーティリティ<br><br>
 * <ul>
 *   <li>ディレクトリの作成</li>
 *   <li>ファイルの存在有無</li>
 *   <li>ファイルの一括読み出し</li>
 *   <li>ファイルの削除</li>
 *   <li>ファイルのリネーム</li>
 *   <li>ファイルの属性変更</li>
 *   <li>ファイルのコピー</li>
 *   <li>ファイルへのデータ出力</li>
 *  </ul>  
 *  が可能です。
 * 
 * @author MRSa
 *
 */
public class MidpFileOperations
{
    private final int COPY_BUFFER_SIZE = 32768;
    private final int BUFFER_MARGIN    = 4;

    /**
     *   コンストラクタでは何も処理を行いません
     */
    public MidpFileOperations()
    {
        // とりあえず、何もしない...
    }

    /**
     * ディレクトリの作成
     * @param directoryName ディレクトリ名 (full path)
     * @return ディレクトリ作成成功(true)/失敗(false)
     */
    public boolean MakeDirectory(String directoryName)
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

    /**
     *  ファイルサイズを取得する
     * 
     * @param targetFileName 確認するファイル名
     * @return ファイルサイズ
     */
    static public long GetFileSize(String targetFileName)
    {
        long           ret = -1;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            ret = dataFileConnection.fileSize();
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
            ret = -1;
        }
        return (ret);
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
     *  ファイルを一括して読み出す
     * 
     * @param targetFileName 読み出すファイル名
     * @return 読み出したデータ
     */
    public byte[] ReadFileWhole(String targetFileName)
    {
        byte[]    readData = null;
        int       readSize = -1;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            int size = (int) dataFileConnection.fileSize();
            readData = new byte[size];
            readSize = (dataFileConnection.openInputStream()).read(readData);
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                dataFileConnection.close();
            }
            catch (Exception e2)
            {
                //
            }
            readSize = -1;
        }
        dataFileConnection = null;
        if (readSize <= 0)
        {
        	readData = null;
        	return (null);
        }
        return (readData);
    }

    /**
     *  ファイルの部分読み出し
     * 
     * @param targetFileName 読み出すファイル名
     * @param startOffset 読み出す先頭のサイズ
     * @param getSize 読み出すデータサイズ
     * @return 読み出したデータ
     */
    public byte[] ReadFilePart(String targetFileName, int startOffset, int getSize)
    {
    	int       size     = -1;
        byte[]    readData = null;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            InputStream in = dataFileConnection.openInputStream();
            size   = (int) dataFileConnection.fileSize();
            readData = new byte[getSize];
			while (startOffset > 0)
			{
				int readSize = 0;
				if (startOffset > getSize)
				{
					readSize = in.read(readData, 0, getSize);
				}
				else
				{
					readSize = in.read(readData, 0, startOffset);
				}
				startOffset = startOffset - readSize;
			}
			size = in.read(readData, 0, getSize);
			in.close();
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                dataFileConnection.close();
            }
            catch (Exception e2)
            {
                //
            }
        }
        dataFileConnection = null;
        if (size <= 0)
        {
        	readData = null;
        	System.gc();
        	return (null);
        }
        return (readData);
    }

    /**
     * ファイルの削除
     * @param targetFileName 削除するファイル (full path)
     * @return 削除成功(true)/削除失敗(false)
     */
    public boolean DeleteFile(String targetFileName)
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
     * @return リネーム成功(true)/リネーム失敗(false)
     */
    public boolean RenameFile(String srcFileName, String renameFile)
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
     * @return 属性変更成功(true)/属性変更失敗(false)
     */
    public boolean ChangeAttribute(String fileName, boolean writable, boolean hidden)
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
     * @return コピー成功(true)/コピー失敗(false)
     */
    public boolean CopyFile(String destFileName, String srcFileName)
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
	 *  テキストファイルにデータを出力する
	 * 
	 * @param outputFileName  出力ファイル名
	 * @param separator       タイトルの前に出力する文字列
	 * @param title           タイトル
	 * @param message         出力データ
	 * @param isAppendMode    上書きモード(false)/追記モード(true)
	 * @return データ出力成功(true)/データ出力失敗(false)
	 */
	public boolean outputDataTextFile(String outputFileName, String separator, String title, String message, boolean isAppendMode)
	{
        boolean ret = true;
		FileConnection connection = null;
		OutputStream  out = null;
        try 
		{
			// ファイルの書き込みオープン
			connection = (FileConnection) Connector.open(outputFileName, Connector.READ_WRITE);
			if (connection.exists() != true)
			{
				// ファイル作成
				connection.create();
			}
			else
			{
				if (isAppendMode == false)
				{
					// ファイルが存在した場合には、一度消して作り直す
					connection.delete();
					connection.create();
				}
			}
			
			// ファイルの末尾まで読み飛ばす。。。
            long fileSize = connection.fileSize();
            out = connection.openOutputStream(fileSize);

			// ファイルに出力する
			String data;
            if (separator != null)
			{
                out.write(separator.getBytes());
			}
			if (title != null)
			{
                data = title  + "\r\n";
			    out.write(data.getBytes());
			}
			if (message != null)
			{
                data = message  + "\r\n";
			    out.write(data.getBytes());
			}
			out.close();
			connection.close();
		}
		catch (Exception e)
		{
			// 例外発生！！！
			try
			{
				if (out != null)
				{
					out.close();
				}
			}
			catch (Exception e2)
			{
				//			
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
				//
			}
			ret = false;
		}
        // 後始末
		out = null;
		connection = null;
		System.gc();
		return (ret);
	}
    /**
	 *  ファイルにデータを出力する
	 * 
	 * @param outputFileName  出力ファイル名
	 * @param data            出力データ
	 * @param isAppendMode    上書きモード(false)/追記モード(true)
	 * @return データ出力成功(true)/データ出力失敗(false)
	 */
	public boolean outputRawDataToFile(String outputFileName, byte[] data, boolean isAppendMode)
	{
        boolean ret = true;
		FileConnection connection = null;
		OutputStream  out = null;
        try 
		{
			// ファイルの書き込みオープン
			connection = (FileConnection) Connector.open(outputFileName, Connector.READ_WRITE);
			if (connection.exists() != true)
			{
				// ファイル作成
				connection.create();
			}
			else
			{
				if (isAppendMode == false)
				{
					// ファイルが存在した場合には、一度消して作り直す
					connection.delete();
					connection.create();
				}
			}
			
			// ファイルの末尾まで読み飛ばす。。。
            long fileSize = connection.fileSize();
            out = connection.openOutputStream(fileSize);

			// ファイルに出力する
            out.write(data);

            // 出力後の後処理...
            out.close();
			connection.close();
		}
		catch (Exception e)
		{
			// 例外発生！！！
			try
			{
				if (out != null)
				{
					out.close();
				}
			}
			catch (Exception e2)
			{
				//			
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
				//
			}
			ret = false;
		}
        // 後始末
		out = null;
		connection = null;
		System.gc();
		return (ret);
	}
}
