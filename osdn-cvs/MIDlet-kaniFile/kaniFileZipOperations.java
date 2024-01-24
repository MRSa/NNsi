import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import com.jcraft.jzlib.ZOutputStream;

/**
 *   ZIPファイルの扱いを行う...
 *   参考： http://www.pkware.com/documents/casestudies/APPNOTE.TXT
 *   利用： jzlib : http://www.jcraft.com/jzlib/  (1.0.7 を取り込み)
 * @author MRSa
 *
 */
public class kaniFileZipOperations
{
	static final private int MAX_WBITS = 15; // 32K LZ77 window
    private kaniFileSceneSelector sceneSelector = null;
	private long   targetFileSize = 0;
	private long   currentReadSize = 0;
	private int   compressedFileDataSize = -1;    // 次に展開するデータの圧縮データサイズ
	private String extractTargetFileName = null;  // 次に展開するデータの展開ファイル名

	/**
     * コンストラクタ...
     */
	public kaniFileZipOperations(kaniFileSceneSelector parent)
	{
		sceneSelector = parent;
	}

	/**
	 *  現状の進捗状況を応答する
	 * @return 現在のファイル展開状況 (%)
	 */
	public String getCurrentStatus()
	{
		if (targetFileSize == 0)
		{
			return ("0/0");
		}
        return (currentReadSize + "/" + targetFileSize);
/*
        double allSize = targetFileSize;
		double current = currentReadSize;
		int answer = (int) ((current / allSize * 100));
		return (answer);
*/
	}
	
	/**
     * ZIPファイルを展開する
     * @param targetDirectory  展開先ディレクトリ
     * @param targetFileName   展開ファイル名
     * @param targetFileName   同一ディレクトリに展開するか？
     * @return
     */
	public boolean extractZipFile(String targetDirectory, String fileName, boolean isSameDirectory)
	{
        boolean       ret = false;
        FileConnection dataFileConnection = null;
        InputStream    istream = null;
        try
        {
        	// 展開ターゲットディレクトリを作成する
        	String extractDirectory = targetDirectory + fileName + ".unzip/";
        	if (isSameDirectory == false)
        	{
			    kaniFileOperations.MakeDirectory(extractDirectory);
        	}
        	else
        	{
        		extractDirectory = targetDirectory;
        	}

			// 圧縮ファイルをオープンする
			String targetFileName = targetDirectory + fileName;
			dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ_WRITE);
			if (dataFileConnection.exists() != true)
			{
				dataFileConnection.close();
				dataFileConnection = null;

				// あとしまつ
				currentReadSize = 0;
				targetFileSize = 0;
				System.gc();
                return (false);
			}

			// オープンするファイルのサイズを取得する
			targetFileSize = dataFileConnection.fileSize();
			currentReadSize = 0;

			// 準備成功、本処理を実行する
			istream = dataFileConnection.openInputStream();
		    
		    try
		    {
			    // ZIPファイル内に格納されている圧縮ファイルをすべて展開する。
		    	ret = true;
                while (ret)
		    	{
                    ret = extractZipMain(istream, extractDirectory, isSameDirectory);
		    	}
		    }
		    catch (Exception ex)
		    {
                // 何もしない
		    }
		    istream.close();
		    istream = null;

		    dataFileConnection.close();
			dataFileConnection = null;
		}
		catch (Exception e)
		{
            // 例外発生！！！
//          e.printStackTrace();
			try
			{
				if (istream != null)
				{
					istream.close();
				}
				istream = null;
			}
			catch (Exception e2)
			{
				//何もしない...
			}
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
		
		// あとしまつ
		currentReadSize = 0;
		targetFileSize = 0;
		System.gc();
		return (ret);
	}
	
	/**
	 *   ZIPファイルの展開メイン処理 (1ファイル分の展開処理)
	 * @param in
	 * @param targetDirectory
	 * @param compressedSize
	 * @param outputSize
	 * @return
	 */
	private boolean extractZipMain(InputStream in, String targetDirectory, boolean isSame)
	{
		FileConnection dstFileConnection = null;
		OutputStream  os = null;
        int targetSize = openZipFile(in, isSame);
		if (targetSize < 0)
		{
			return (false);
		}
        sceneSelector.updateProgressForZipExtract(); // データの更新

		String targetFileName = extractTargetFileName;
		int compressedSize = compressedFileDataSize;
		if (compressedSize == 0)
		{
			if (isSame == false)
			{
			    // 展開するファイルではなかった(具体的にはディレクトリだった)...ディレクトリを掘って終了する
			    kaniFileOperations.MakeDirectory(targetDirectory + targetFileName);
			}
			return (true);
		}

		
		// 展開する予定のディレクトリがあるかどうかチェックする
		String extractDirectory = targetFileName;
		if (isSame == false)
		{
			int pos = extractDirectory.lastIndexOf('/');
			if (pos > 0)
			{
				String makeDir = extractDirectory.substring(0, pos);
				if (kaniFileOperations.IsExistFile(targetDirectory + makeDir) == false)
				{
					// ディレクトリが存在しない場合には、該当するディレクトリを掘る
					kaniFileOperations.MakeDirectory(targetDirectory + makeDir);
				}
			}
		}
		
		try
        {
            //  展開ファイルを開く
			dstFileConnection = (FileConnection) Connector.open(targetDirectory + targetFileName, Connector.READ_WRITE);
    		if (dstFileConnection.exists() == true)
    		{
    			// ファイルが存在した、、、削除して作り直す
    			dstFileConnection.delete();
    		}    		

    		dstFileConnection.create();
    		os = dstFileConnection.openOutputStream();
    		if (os != null)
    		{
    			if (targetSize != compressedSize)
    			{
                    // データは圧縮されていたので
    				// 展開ファイルにがっつり出力する...
        			ZOutputStream zOut = new ZOutputStream(os, MAX_WBITS, true, false);
        			try
        			{
                        for (int i = 0, loop = 0; i < compressedSize; i++)
                        {
                        	int data = in.read();
                        	currentReadSize++;
                        	loop++;
                        	zOut.write(data);
                        	if (loop > 1000)
                        	{
                        		 // データ展開の進捗報告
                                sceneSelector.updateProgressForZipExtract();
                                loop = 0;
                        	}
                        }
        			}
        			catch (Exception e)
        			{
        				// 展開の例外発生、何もしない...
        			}
    		        zOut.finish();
    		        zOut.end();
    		        zOut.close();    				
    			}
    			else
    			{
        	        // データが非圧縮で格納されている！
    				try
    				{
        				for (int i = 0, loop = 0; i < compressedSize; i++)
        				{
                        	int data = in.read();
                        	currentReadSize++;
                        	loop++;
                        	os.write(data);
                        	if (loop > 1000)
                        	{
                        		 // データ展開の進捗報告
                                sceneSelector.updateProgressForZipExtract();
                                loop = 0;
                        	}   					
        				}
    				}
        			catch (Exception e)
        			{
        				// 例外発生、でも何もしない...
        			}
    			}
    		}
    		os.close();
    		dstFileConnection.close();
    		dstFileConnection = null;
        }
        catch (Exception ex)
        {
        	// 例外発生！
        	if (os != null)
        	{
        		try
        		{
                	os.close();
        		}
        		catch (Exception e)
        		{
        			// 
        		}
        	}
        	return (true);
        }
		return (true);
	}

	/**
	 *  ZIPファイルを開く
	 * @param is
	 * @return
	 */
	private int openZipFile(InputStream is, boolean isSame)
	{
		int extractSize = -1;
		try
		{
            byte[] buffer = new byte[500];
            while (true)
            {
                int size = is.read(buffer, 0, 2);
                currentReadSize = currentReadSize + 2;
                if (size <= 0)
                {
                	// 読み込み失敗...終了する
        			extractSize = -1;
        			compressedFileDataSize = -1;
                    extractTargetFileName = null;
                    return (extractSize);
                }
            	// データ(Local File Header)の先頭を確認... "PK.." でZIP形式かどうか...
            	if ((buffer[0] != (byte) 0x50)||(buffer[1] != (byte) 0x4b))
                {
            		// データの先頭ではなかったので...繰り返す
            		continue;
                }
                size = is.read(buffer, 0, 2);
                currentReadSize = currentReadSize + 2;
                if (size <= 0)
                {
                	// 読み込み失敗...終了する
        			extractSize = -1;
        			compressedFileDataSize = -1;
                    extractTargetFileName = null;
                    return (extractSize);
                }
                if ((buffer[0] != (byte) 0x03)||(buffer[1] != (byte) 0x04))
                {
                	// データブロックではなかったので...
                	continue;
                }

                // とりあえず、ここらあたりのデータを読み飛ばす...
                size = is.read(buffer, 0, 2); // version needed to extract
                size = is.read(buffer, 0, 2); // general purpose bit flag (0x0000)
                size = is.read(buffer, 0, 2); // compression method (0x0008)
                size = is.read(buffer, 0, 2); // last mod file time
                size = is.read(buffer, 0, 2); // last mod file date
                size = is.read(buffer, 0, 4); // crc-32
                currentReadSize = currentReadSize + 14;
                
                // compressed size                
                compressedFileDataSize = 0;
                size = is.read(buffer, 0, 4); // compressed size
                currentReadSize = currentReadSize + 4;
                compressedFileDataSize = compressedFileDataSize + ((buffer[0] & 0xff) << 0);
                compressedFileDataSize = compressedFileDataSize + ((buffer[1] & 0xff) << 8);
                compressedFileDataSize = compressedFileDataSize + ((buffer[2] & 0xff) << 16);
                compressedFileDataSize = compressedFileDataSize + ((buffer[3] & 0xff) << 24);

                // uncompressed size
                extractSize = 0;
                size = is.read(buffer, 0, 4); // uncomressed size
                currentReadSize = currentReadSize + 4;
                extractSize = extractSize + ((buffer[0] & 0xff) << 0);
                extractSize = extractSize + ((buffer[1] & 0xff) << 8);
                extractSize = extractSize + ((buffer[2] & 0xff) << 16);
                extractSize = extractSize + ((buffer[3] & 0xff) << 24);

                // File name length
                int fileNameLength = 0;
                size = is.read(buffer, 0, 2); // file name length
                currentReadSize = currentReadSize + 2;
                fileNameLength = buffer[0];
                fileNameLength = fileNameLength + (int) buffer[1] * 0x100;

                // Extra field length
                int extraFieldLength = 0;
                size = is.read(buffer, 0, 2); // extra field length
                currentReadSize = currentReadSize + 2;
                extraFieldLength = buffer[0];
                extraFieldLength = extraFieldLength + (int) buffer[1] * 0x100;

                // ファイル名(実データ)を読み込む
                size = is.read(buffer, 0, fileNameLength);
                currentReadSize = currentReadSize + size;
                extractTargetFileName = null;
                extractTargetFileName = new String(buffer, 0, size);
                if (isSame == true)
                {
                	// ディレクトリ展開をしない場合...
            		int pos = extractTargetFileName.lastIndexOf('/');
            		if (pos > 0)
            		{
            			String fileName = extractTargetFileName.substring(pos + 1);
            			extractTargetFileName = fileName;
            		}
                }

                if (extraFieldLength > 0)
                {
                	// extra field が存在する場合には、その部分を読み飛ばす
                	size = is.read(buffer, 0, extraFieldLength);
                    currentReadSize = currentReadSize + size;
                }
                
                break; // うまくいけば、一回で抜ける
            }
            buffer = null;
         }
		catch (Exception ex)
		{
			extractSize = -1;
			compressedFileDataSize = -1;
            extractTargetFileName = null;
		}
		return (extractSize);
	}

}
