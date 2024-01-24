/**
 *  設定データ保持クラス
 * @author MRSa
 *
 */
public class a2BDPreference
{
    // データストレージ名とバージョン
    private final String DATASTORAGE_NAME     = "a2BD";
    private final int    DATASTORAGE_VERSION  = 101;

    /***** データベースレコード番号(ここから) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DBのバージョン格納レコード
    private final int    PARAMS_RECORDID      = 2;
    private final int    USERAGENT_RECORDID   = 3;
    private final int    DATA1_RECORDID       = 4;
    private final int    DATA2_RECORDID       = 5;
    /***** データベースレコード番号(ここまで) *****/
    
    /***** レコードアクセサ *****/
    private MidpRecordAccessor dbAccessor = new MidpRecordAccessor();

    /***** 保管データ(ここから) *****/
    private String             data1           = "http://menu.2ch.net/bbstable.html";
	private String             data2           = "";
    private String             userAgent       = "Mozilla/3.0(WILLCOM;SANYO/WX310SA/2;1/1/C128) NetFront/3.3";
	private boolean           wx310WorkAround = false;
    private boolean           divideGetHttp   = false;
    private boolean           finishAuto      = false;
    /***** 保管データ(ここまで) *****/

    /**
     *  コンストラクタ
     *
     */
    public a2BDPreference()
    {
    	
    }

	/**
	 *  データをリストアする。。。
     *
	 */
	public void restore()
	{
        int versionNumber = 0;
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        if (dbAccessor.startReadRecordIntArray(VERSION_RECORDID) > 0)
        {
            versionNumber = dbAccessor.readRecordIntArray(0);
        }
        try
        {
            if (versionNumber == DATASTORAGE_VERSION)
            {
                int intData = 0;
                if (dbAccessor.startReadRecordIntArray(PARAMS_RECORDID) > 0)
                {
                	intData = dbAccessor.readRecordIntArray(0);
                	wx310WorkAround = (intData != 0) ? true : false;

                	intData = dbAccessor.readRecordIntArray(1);
                	divideGetHttp = (intData != 0) ? true : false;

                	intData = dbAccessor.readRecordIntArray(2);
                	finishAuto = (intData != 0) ? true : false;
                }
                dbAccessor.finishRecordIntArray();

                // バージョン一致、レコードを読み込む
                byte[] byteData = null;

                byteData = dbAccessor.readRecord(USERAGENT_RECORDID);
                if (byteData != null)
                {
                	userAgent = new String(byteData);
                }
                byteData = null;

                byteData = dbAccessor.readRecord(DATA1_RECORDID);
                if (byteData != null)
                {
                    data1 = new String(byteData);
                }
                byteData = null;

                byteData = dbAccessor.readRecord(DATA2_RECORDID);
                if (byteData != null)
                {
                    data2 = new String(byteData);
                }
                byteData = null;

            }
        }
        catch (Exception e)
        {
            // 例外発生、初期値に戻す
        	data1           = "http://menu.2ch.net/bbstable.html";
        	data2           = "";
            userAgent       = "Mozilla/3.0(WILLCOM;SANYO/WX310SA/2;1/1/C128) NetFront/3.3";
        	wx310WorkAround = false;
            divideGetHttp   = false;
            finishAuto      = false;
        }
        dbAccessor.closeDatabase();
	    System.gc();
	}	
	
	/**
	 *  データをバックアップする
	 *
	 */
	public void backup()
	{
    	
    	// DBをオープンさせる
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        
        // バージョン番号の記録
        dbAccessor.startCacheIntArray(1);
        dbAccessor.cacheRecordIntArray(0, DATASTORAGE_VERSION);
        dbAccessor.writeRecordIntArray(VERSION_RECORDID);

        try
        {

            // 設定データ(true/false)の情報記録
            dbAccessor.startCacheIntArray(3);    // 3件のデータを記録する。。。
            int intData = 0;
            intData = (wx310WorkAround == true) ? 1: 0;
            dbAccessor.cacheRecordIntArray(0, intData);

            intData = (divideGetHttp == true) ? 1: 0;
            dbAccessor.cacheRecordIntArray(1, intData);

            intData = (finishAuto == true)? 1: 0;
            dbAccessor.cacheRecordIntArray(2, intData);

            dbAccessor.writeRecordIntArray(PARAMS_RECORDID);

            if ((userAgent != null)&&(userAgent.length() > 1))
            {
                dbAccessor.writeRecord(USERAGENT_RECORDID, userAgent.getBytes(), 0, (userAgent.getBytes()).length);
            }

            if ((data1 != null)&&(data1.length() > 1))
            {
                dbAccessor.writeRecord(DATA1_RECORDID, data1.getBytes(), 0, (data1.getBytes()).length);
            }
            
            if ((data2 != null)&&(data2.length() > 1))
            {
                dbAccessor.writeRecord(DATA2_RECORDID, data2.getBytes(), 0, (data2.getBytes()).length);
            }
        }
        catch (Exception ex)
        {
        	// 例外発生、、、何もしない
        }
        
        // DBをクローズする
        dbAccessor.closeDatabase();
        
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////// getter
    
    public String decideFileName(String url, String baseFileName)
    {
    	int pos = 0;
    	pos = baseFileName.lastIndexOf('/');
    	if (baseFileName.length() > (pos + 1))
    	{
            // 末尾が / で終わっていない... data1をそのまま応答する。
    		return (baseFileName);
    	}

    	// 末尾が / で終わっている... URLからファイル名を抽出する    	
        pos = url.lastIndexOf('/');
        if (url.length() > (pos + 1))
        {
            String fileName = url.substring(pos + 1);
            return (baseFileName + fileName);
        }	
    	return (baseFileName + "temp.html");
    }
	
    public String getData1()
    {
        return (data1);
    }

    public String getData2()
    {
    	return (data2);
    }

    public String getUserAgent()
    {
    	return (userAgent);
    }
	
    public boolean getWX310workaroundFlag()
    {
    	return (wx310WorkAround);
    }
    
    public boolean getDivideGetHttp()
    {
    	return (divideGetHttp);
    }

    public boolean getAutoFinish()
    {
    	return (finishAuto);
    }

	/////////////////////////////////////////////////////////////////////////////////////////// setter

    public void setData1(String data)
    {
    	data1 = data;
    }
	
    public void setData2(String data)
    {
    	data2 = data;
    }

    public void setUserAgent(String data)
    {
    	userAgent = data;
    }
	
    public void setWX310workaroundFlag(boolean data)
    {
    	wx310WorkAround = data;
    }
    
    public void setDivideGetHttp(boolean data)
    {
    	divideGetHttp = data;
    }

    public void setAutoFinish(boolean data)
    {
    	finishAuto = data;
    }
}
