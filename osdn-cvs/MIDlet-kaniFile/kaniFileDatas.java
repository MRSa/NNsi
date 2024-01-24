import javax.microedition.rms.RecordStore;

/**
 * アプリケーションのデータを格納するクラス
 * 
 * @author MRSa
 *
 */
public class kaniFileDatas
{
	private final byte[] PREF_VERSION = { (byte) 0, (byte) 1};  // preference DBのバージョン...
	
	private final int    VERSION_RECORDID     = 1;               // preference DBのバージョン格納レコード
	private final int    BASEDIR_RECORDID     = 2;               // preference DBのベースディレクトリ格納レコード

	private String        startDirectory       = null;            // ディレクトリ名を記録する

	/**
     * コンストラクタ
     *
     */
	kaniFileDatas()
	{
		// とりあえず、なにもしない
	}

	/**
	 * リソースの準備...
	 *
	 */
	public void prepare()
	{
		boolean     isReset            = false;
		byte[]       versionInfo       = null;
		byte[]       baseDirectory     = null;
		RecordStore  prefDB             = null;
		try
		{
			// レコードストアをオープンする
			prefDB = RecordStore.openRecordStore("kaniFile-Settings", true);
			if (prefDB.getNumRecords() != 0)
			{
				// データベースのバージョン確認...
				versionInfo = prefDB.getRecord(VERSION_RECORDID);
				if (versionInfo.length == PREF_VERSION.length)
				{
					for (int loop = 0; ((loop < versionInfo.length)&&(isReset == false)); loop++)
					{
						if (versionInfo[loop] != PREF_VERSION[loop])
						{
							// バージョン情報違い、、、リセットする
							isReset = true;
							loop = versionInfo.length;
						}
					}
				}

   				// ディレクトリを読み出す
    			baseDirectory = prefDB.getRecord(BASEDIR_RECORDID);
    			if ((isReset != true)&&(baseDirectory.length != 0))
    			{
    				// ディレクトリを反映させる
    				startDirectory = new String(baseDirectory);
    				isReset = false;
    			}
    			else
    			{
    				// ディレクトリが読み出せなかった...
    				isReset = true;
    			}
			}
			
			// レコードを閉じる
			prefDB.closeRecordStore();			
		}
		catch (Exception e)
		{
			// レコードストアが閉じてなければ閉じる
			try
			{
				if (prefDB != null)
				{
					prefDB.closeRecordStore();
				}
			}
			catch (Exception e2)
			{
				// 何もしない...
			}
			isReset = true;
		}
		if (isReset == true)
		{
			// バージョン情報をリセットする
			startDirectory = "file:///";
		}
		return;
	}

	/**
	 * ディレクトリの応答
	 * @return
	 */
	public String getDirectory()
	{
		return (startDirectory);
	}
	
	/**
	 * ディレクトリの記録
	 * @param dirName
	 */
	public void setDirectory(String dirName)
	{
		startDirectory = dirName;
		return;
	}
	
	/**
	 * リソースを保管する...
	 *
	 */
	public void store()
	{
		RecordStore  prefDB = null;
		try
		{
			// レコードストアをオープンする
			prefDB = RecordStore.openRecordStore("kaniFile-Settings", true);

			// 1レコード目...バージョン番号
			int nofRecord = prefDB.getNumRecords();
			if (nofRecord >= VERSION_RECORDID)
			{
				// バージョン情報の書き込み(更新)...
				prefDB.setRecord(VERSION_RECORDID, PREF_VERSION, 0, PREF_VERSION.length);
			}
			else
			{
				// バージョン情報の書き込み(追加)...
				prefDB.addRecord(PREF_VERSION, 0, PREF_VERSION.length);					
			}

			// 2レコード目...ベースディレクトリ
			int size = (startDirectory.getBytes()).length;
			if (nofRecord >= BASEDIR_RECORDID)
			{
				// ベースディレクトリ情報の書き込み(更新)...
				prefDB.setRecord(BASEDIR_RECORDID, (startDirectory).getBytes(), 0, size);
			}
			else
			{
				// ベースディレクトリ情報の書き込み(追加)...
				prefDB.addRecord((startDirectory).getBytes(), 0, size);					
			}
			prefDB.closeRecordStore();
		}
		catch (Exception e)
		{
			// レコードストアが閉じてなければ閉じる
			try
			{
				if (prefDB != null)
				{
					prefDB.closeRecordStore();
				}
			}
			catch (Exception e2)
			{
				// 何もしない...
			}
		}
		return;
	}

	/**
	 *  データを int型からバイト列へ変換する
	 * 
	 * 
	 */
/*
	private void serializeData(byte[] buffer, int offset, int convId)
	{
		buffer[offset + 0] = (byte)((convId >> 24)&(0xff));
		buffer[offset + 1] = (byte)((convId >> 16)&(0xff));
		buffer[offset + 2] = (byte)((convId >>  8)&(0xff));
		buffer[offset + 3] = (byte)((convId      )&(0xff));
		return;
	}
*/

	/**
	 *  バイト列データをint型へ変換する
	 * 
	 * 
	 */
/*
	private int deSerializeData(byte[] buffer, int offset)
	{
		return ((((buffer[offset + 0]&(0xff))<< 24) + ((buffer[offset + 1]&(0xff))<< 16) +
                 ((buffer[offset + 2]&(0xff))<<  8) + ((buffer[offset + 3]&(0xff)))));
	}	
*/

}
