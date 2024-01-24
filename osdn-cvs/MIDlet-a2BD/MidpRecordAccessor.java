import javax.microedition.rms.RecordStore;

/**
 *  データストレージ(レコードストア)アクセサ
 *  <br>
 *  端末のレコードストアに対し読み書きを行うユーティリティクラスです。
 *  数値型のデータ読み書きを（バイト列に変換して）１レコードにまとめ
 *  実行することもできます。
 * 
 * @author MRSa
 *
 */
public class MidpRecordAccessor
{
    private final int INTARRAY_NOTUSED = -1;
	private String   dbStorageName = null;
    private boolean createIfNecessary = true;
    private int      authMode = RecordStore.AUTHMODE_PRIVATE;
    private boolean writable = true;

    private RecordStore  prefDB = null;
    private byte[]      intArrayRecord = null;
    private int         isIntArrayUse  = INTARRAY_NOTUSED;
    
    /**
	 *  コンストラクタでの処理はありません。
     * 
     */
    public MidpRecordAccessor()
    {
        //
    }

    /**
     *  データストレージをオープンする
     *  
     * @param storageName データストレージ名
     * @return オープン結果 OK:true, NG:false
     */
    public boolean openDatabase(String storageName)
    {
        dbStorageName = storageName;
        boolean ret = false;
        try
        {
            // レコードストアをオープンする
            prefDB = RecordStore.openRecordStore(dbStorageName, createIfNecessary, authMode, writable);
            ret = true;
        }
        catch (Exception ex)
        {
            ret = false;        	
        }
        return (ret);        
    }

    /**
     *  データストレージをオープンする
     *  
     * @param storageName データストレージ名
     * @param isCreate ひつようがあればストレージを作成する
     * @param athMode AUTHMODE
     * @param isWrite 書き込み許可
     * @return オープン結果 OK:true, NG:false
     */
    public boolean openDatabase(String storageName, boolean isCreate, int athMode, boolean isWrite)
    {
        dbStorageName = storageName;
        createIfNecessary = isCreate;
        boolean ret = false;
        try
        {
            // レコードストアをオープンする
            prefDB = RecordStore.openRecordStore(storageName, isCreate, athMode, isWrite);
            ret = true;
        }
        catch (Exception ex)
        {
            ret = false;        	
        }
        return (ret);        
    }
    
    /**
     *  (別アプリの)データストレージをオープンする
     *  
     * @param recordStoreName レコードストア名称
     * @param vendorName ベンダ名称
     * @param suiteName スート名称(MIDlet suite name)
     * @return オープン結果 OK:true, NG:false
     */
    public boolean openDatabase(String recordStoreName, String vendorName, String suiteName)
    {
        boolean ret = false;
        try
        {
            // レコードストアをオープンする
            prefDB = RecordStore.openRecordStore(recordStoreName, vendorName, suiteName);
            ret = true;
        }
        catch (Exception ex)
        {
            ret = false;        	
        }
        return (ret);        
    }
    
    /**
     *  データストレージをクローズする
     * 
     * @return 成功(true)/失敗(false)
     */
    public boolean closeDatabase()
    {
    	boolean ret = false;
    	try
    	{
    		if (prefDB != null)
    		{
    			prefDB.closeRecordStore();
    			prefDB = null;
    		}
    		System.gc();
    		ret = true;
    	}
    	catch (Exception ex)
    	{
    	    //
    	}
    	return (ret);
    }

    /**
     *  レコードにデータを書き込む
     *  （レコード番号を指定して書き込む）
     * 
     * @param recordNumber レコード番号
     * @param data     書き込むデータのバッファ
     * @param offset   書き込むデータの先頭インデックス
     * @param length   書き込みデータ長
     * @return 成功(true)/失敗(false)
     */
    public boolean writeRecord(int recordNumber, byte[] data, int offset, int length)
    {
        boolean ret = false;
    	if (prefDB == null)
        {
            return (false);
        }

        try
        {
            int nofRecord = prefDB.getNumRecords();
            if (nofRecord >= recordNumber)
            {
                // データの書き込み(更新)
                prefDB.setRecord(recordNumber, data, offset, length);
            }
            else
            {
                // データの追記
                prefDB.addRecord(data, offset, length);
            }
            ret = true;
        }
        catch (Exception ex)
        {
        	ret = false;
        }
        return (ret);
    }

    /**
     *  レコードデータの読み出し
     * 
     * @param recordNumber 読み出すレコード番号(0スタート)
     * @return レコード情報 (nullのときには読み出し失敗)
     */
    public byte[] readRecord(int recordNumber)
    {
    	if (prefDB == null)
    	{
    		return (null);
    	}
    	try
    	{
            int nofRecord = prefDB.getNumRecords();
            if (nofRecord < recordNumber)
            {
                return (null);
            }
            return (prefDB.getRecord(recordNumber));
    	}
    	catch (Exception ex)
    	{
    		
    	}
    	return (null);
    }

    /**
     *  int型データの書き出しの手続き開始
     * 
     * @param size 書き出すint型データの個数
     * @return 準備OK(true) / 準備失敗(false)
     */
    public boolean startCacheIntArray(int size)
    {
        // サイズ指定が間違っていた場合には、何もせず終了する
        intArrayRecord = null;
    	if (size <= 0)
        {
            isIntArrayUse  = INTARRAY_NOTUSED;
        	return (false);
        }
    	
    	// int型データをbyte型データに変換する
    	intArrayRecord = new byte[size * 4];
    	isIntArrayUse = size;
        return (true);    	
    }

    /**
     *  int型データ配列をレコードに吐き出す前に一旦キャッシュする
     * 
     * @param index 格納するデータの配列番号(0スタート)
     * @param data 実データ
     * @return 成功(true) 失敗(false)
     */
    public boolean cacheRecordIntArray(int index, int data)
    {
    	if ((isIntArrayUse < 0)||(index >= isIntArrayUse))
    	{
    		// 領域未確保あるいは確保領域サイズオーバー
    		return (false);
    	}
	    serializeData(intArrayRecord, (index * 4), data);
	    return (true);
    }
    
    /**
     * int型データを一気に書き出す
     * @param recordNumber 書き込むレコード番号
     * @return 書き出し成功(true) / 書き出し失敗(false)
     */
    public boolean writeRecordIntArray(int recordNumber)
    {
    	if (isIntArrayUse <= 0)
    	{
    		// 領域未確保
    		return (false);
    	}
    	// データの書き出し
    	boolean ret = writeRecord(recordNumber, intArrayRecord, 0, (isIntArrayUse * 4));
    	isIntArrayUse = INTARRAY_NOTUSED;
    	intArrayRecord = null;
    	System.gc();
        return (ret);
    }
    
    /**
     *  int型レコードデータを読み出して準備する
     * 
     * @param recordNumber 読み込むレコード番号
     * @return 読み出したint型データ数
     */
    public int startReadRecordIntArray(int recordNumber)
    {
    	intArrayRecord = null;
        intArrayRecord = readRecord(recordNumber);
        if (intArrayRecord == null)
        {
        	isIntArrayUse = INTARRAY_NOTUSED;
        	return (INTARRAY_NOTUSED);
        }
        isIntArrayUse = (intArrayRecord.length / 4);
        return (isIntArrayUse);
    }

    /**
     *  int型データを読み出す
     * 
     * @param index 読み出すデータの配列番号(0スタート)
     * @return int型データの値
     */
    public int readRecordIntArray(int index)
    {
    	if ((isIntArrayUse <= 0)||(index >= isIntArrayUse))
    	{
    		// 領域未確保あるいは確保領域サイズオーバー
    		return (INTARRAY_NOTUSED);
    	}
    	return (deSerializeData(intArrayRecord, (index * 4)));
    }

    /**
     *   int型データの読み出しを終了する
     * 
     */
    public void finishRecordIntArray()
    {
    	isIntArrayUse = INTARRAY_NOTUSED;
    	intArrayRecord = null;
    	System.gc();
    	return;
    }   
    
    /**
     *  データを int型からバイト列へ変換する
     *  @param buffer 変換後データ格納領域
     *  @param offset 変換後データ格納領域への格納位置オフセット
     *  @param convId 変換するint型データ
     */
    private void serializeData(byte[] buffer, int offset, int convId)
    {
        buffer[offset + 0] = (byte)((convId >> 24)&(0xff));
        buffer[offset + 1] = (byte)((convId >> 16)&(0xff));
        buffer[offset + 2] = (byte)((convId >>  8)&(0xff));
        buffer[offset + 3] = (byte)((convId      )&(0xff));
        return;
    }

    /**
     *  バイト列データをint型へ変換する
     *  
     *  @param buffer バイト列データ
     *  @param osset 変換を行うデータの先頭インデックス番号
     *  @return 変換結果(int型データ)
     * 
     */
    private int deSerializeData(byte[] buffer, int offset)
    {
        return ((((buffer[offset + 0]&(0xff))<< 24) + ((buffer[offset + 1]&(0xff))<< 16) +
                 ((buffer[offset + 2]&(0xff))<<  8) + ((buffer[offset + 3]&(0xff)))));
    }
}
