import javax.microedition.lcdui.Font;
import javax.microedition.midlet.MIDlet;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;;

/**
 *  データストレージクラス
 * 
 * @author MRSa
 */
public class TheCountDataStorage implements IMidpDataStorage
{
    // アプリケーション名とスプラッシュスクリーン
    static final private String MyMidpName = "TheCount";
    static final private String MyMidpSplashImageName = "/res/splash.png";

    // データストレージ名とバージョン
    private final String DATASTORAGE_NAME     = "TheCount";
    private final int    DATASTORAGE_VERSION  = 100;

    /***** データベースレコード番号(ここから) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DBのバージョン格納レコード
    private final int    BASEDIR_RECORDID     = 2;    // preference DBのベースディレクトリ格納レコード
    private final int    REPORTFILE_RECORDID  = 3;
    private final int    ITEMVALUE_RECORDID   = 4;
    private final int    ITEM1NAME_RECORDID   = 5;
    private final int    ITEM2NAME_RECORDID   = 6;
    private final int    ITEM3NAME_RECORDID   = 7;
    private final int    ITEM4NAME_RECORDID   = 8;
    private final int    ITEM5NAME_RECORDID   = 9;
    private final int    ITEM6NAME_RECORDID   = 10;
    /***** データベースレコード番号(ここまで) *****/

    /***** 保管データ(ここから) *****/
    private final int   NUMBER_OF_ITEMS = 6;
    private String       baseDirectory  = null; // ベースディレクトリ名を記録する
    private String       reportFileName = "";    // 出力するレポートファイル名を記録する
    private String[]     itemName       = null;
    private int[]        itemValue      = null;
    private int          fontSize     = Font.SIZE_SMALL;
    /***** 保管データ(ここまで) *****/

    /***** レコードアクセサ *****/
    private MidpRecordAccessor dbAccessor = null;
	private MidpFileOperations fileUtils  = null;

    private String  informationMessage = "";    // 情報メッセージ

    /**
     *   コンストラクタ
     * 
     * @param レコードアクセサ
     */
    public TheCountDataStorage(MidpRecordAccessor accessor, MidpFileOperations fileOperation)
    {
        dbAccessor = accessor;
        fileUtils  = fileOperation;
        
        itemName  = new String[NUMBER_OF_ITEMS];
        itemValue = new int[NUMBER_OF_ITEMS];
        for (int index = 0; index < NUMBER_OF_ITEMS; index++)
        {
        	itemName[index] = "";
        	itemValue[index] = 0;
        }
    }

    /**
     *  アプリケーション名を応答する
     * 
     */
    public String getApplicationName()
    {
        return (MyMidpName);
    }

    /**
     * スプラッシュのイメージ名を応答する
     * 
     */
    public String getSplashImageName()
    {
        return (MyMidpSplashImageName);
    }

    /**
     *  クラスの実行準備
     *
     */
    public void prepare(MIDlet object)
    {
        //
    }

    /**
     *  画面の準備
     *
     */
    public void prepareScreen(MidpSceneSelector selector)
    {
        //
    }

    /**
     *  アプリケーション自動終了タイマの値を応答する
     * 
     */
    public long getTimeoutCount()
    {
        return (-1);
    }

    /**
     *  ストレージ領域にデータを退避させる
     *
     */
    public void backup()
    {
        // バックアップするデータがなければ、何もせず終了する
        if (baseDirectory == null)
        {
            return;
        }
    	
    	// DBをオープンさせる
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        
        // バージョン番号の記録
        dbAccessor.startCacheIntArray(1);
        dbAccessor.cacheRecordIntArray(0, DATASTORAGE_VERSION);
        dbAccessor.writeRecordIntArray(VERSION_RECORDID);

        // ベースディレクトリの記録
        dbAccessor.writeRecord(BASEDIR_RECORDID, baseDirectory.getBytes(), 0, (baseDirectory.getBytes()).length);

        // レポートファイル名の記録
        dbAccessor.writeRecord(REPORTFILE_RECORDID, reportFileName.getBytes(), 0, (reportFileName.getBytes()).length);

        try
        {
            // アイテム値の記録
            dbAccessor.startCacheIntArray(NUMBER_OF_ITEMS);
            for (int index = 0; index < NUMBER_OF_ITEMS; index++)
            {
                dbAccessor.cacheRecordIntArray(index, itemValue[index]);
            }
            dbAccessor.writeRecordIntArray(ITEMVALUE_RECORDID);

            // アイテム名の記録
            dbAccessor.writeRecord(ITEM1NAME_RECORDID, itemName[0].getBytes(), 0, (itemName[0].getBytes()).length);
            dbAccessor.writeRecord(ITEM2NAME_RECORDID, itemName[1].getBytes(), 0, (itemName[1].getBytes()).length);
            dbAccessor.writeRecord(ITEM3NAME_RECORDID, itemName[2].getBytes(), 0, (itemName[2].getBytes()).length);
            dbAccessor.writeRecord(ITEM4NAME_RECORDID, itemName[3].getBytes(), 0, (itemName[3].getBytes()).length);
            dbAccessor.writeRecord(ITEM5NAME_RECORDID, itemName[4].getBytes(), 0, (itemName[4].getBytes()).length);
            dbAccessor.writeRecord(ITEM6NAME_RECORDID, itemName[5].getBytes(), 0, (itemName[5].getBytes()).length);
        }
        catch (Exception e)
        {
        	//
        }

        // DBをクローズする
        dbAccessor.closeDatabase();
        return;
    }

    /**
     *  ストレージ領域からデータを復旧させる
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
        if (versionNumber == DATASTORAGE_VERSION)
        {
            // バージョン番号が一致、レコードを読み込む
            byte[] dir = dbAccessor.readRecord(BASEDIR_RECORDID);
            if (dir != null)
            {
                baseDirectory = new String(dir);
            }

            // レポートファイル名を読み込む
            byte[] fileName = dbAccessor.readRecord(REPORTFILE_RECORDID);
            if (fileName != null)
            {
            	reportFileName = new String(fileName);
            }

            try
            {
                // アイテム値を展開
            	int maxIndex = dbAccessor.startReadRecordIntArray(ITEMVALUE_RECORDID);
                if (maxIndex >= 0)
                {
                	if (maxIndex >= NUMBER_OF_ITEMS)
                	{
                		maxIndex = NUMBER_OF_ITEMS;
                	}
                    for (int index = 0; index < maxIndex; index++)
                    {
                    	itemValue[index] = dbAccessor.readRecordIntArray(index);
                    }
                }

                // アイテム名を展開 (1)
                byte[] itemRecordName = dbAccessor.readRecord(ITEM1NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[0] = new String(itemRecordName);
                }
                itemRecordName = null;

                // アイテム名を展開 (2)
                itemRecordName = dbAccessor.readRecord(ITEM2NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[1] = new String(itemRecordName);
                }
                itemRecordName = null;

                // アイテム名を展開 (3)
                itemRecordName = dbAccessor.readRecord(ITEM3NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[2] = new String(itemRecordName);
                }
                itemRecordName = null;

                // アイテム名を展開 (4)
                itemRecordName = dbAccessor.readRecord(ITEM4NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[3] = new String(itemRecordName);
                }
                itemRecordName = null;

                // アイテム名を展開 (5)
                itemRecordName = dbAccessor.readRecord(ITEM5NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[4] = new String(itemRecordName);
                }
                itemRecordName = null;

                // アイテム名を展開 (6)
                itemRecordName = dbAccessor.readRecord(ITEM6NAME_RECORDID);
                if (itemRecordName != null)
                {
                	itemName[5] = new String(itemRecordName);
                }
                itemRecordName = null;
            }
            catch (Exception e)
            {
            	//
            }
        }        
        dbAccessor.closeDatabase();
        return;
    }

    /**
     *  オブジェクトをクリアする
     *
     */
    public void cleanup()
    {
        System.gc();
        return;
    }

    /**
     *  フォントサイズを取得する
     * 
     * @return フォントサイズ
     */
    public int getFontSize()
    {
        return (fontSize);
    }

    /**
     *  ベースディレクトリ名を取得する
     * 
     * @return ベースディレクトリ名
     */
    public String getBaseDirectory()
    {
        return (baseDirectory);
    }

    /**
     *   アイテムの名前を応答する
     * @param index  アイテムインデックス(0始まり)
     * @return アイテムの名前(未設定の場合はアイテム番号(1はじまり)を応答)
     */
    public String getItemName(int index)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
                if (itemName[index].length() > 0)
                {
                	return (itemName[index]);
                }
                return ("" + (index + 1));
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return (null);
    }

    /**
     *   アイテムの現在値を応答する
     * @param  index  アイテムインデックス(0始まり)
     * @return 設定するアイテムの現在値 (index異常の場合は -1)
     */
    public int getItemValue(int index)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
    			return (itemValue[index]);
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return (-1);
    }

    /**
     *  フォントサイズを記録(設定)する
     * 
     * @param size フォントサイズ
     */
    public void setFontSize(int size)
    {
        fontSize = size;
    }

    /**
     *   ベースディレクトリ名を設定する
     * 
     * @param directory ベースディレクトリ名
     */
    public void setBaseDirectory(String directory)
    {
        baseDirectory = directory;
    }

    
    /**
     *   アイテムの名前を設定する
     * @param index  アイテムインデックス(0始まり)
     * @param value  設定するアイテムの名前
     */
    public void setItemName(int index, String value)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
    			itemName[index] = value;
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return;
    }

    /**
     *   アイテムの値を設定する
     * @param index  アイテムインデックス(0始まり)
     * @param value  設定するアイテムの値
     */
    public void setItemValue(int index, int value)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
    			itemValue[index] = value;
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return;
    }

    /**
     *   アイテムの値に加算する
     * @param index  アイテムインデックス(0始まり)
     * @param value  加算する値
     */
    public void addItemValue(int index, int value)
    {
    	if ((index >= 0)||(index < NUMBER_OF_ITEMS))
    	{
    		try
    		{
    			itemValue[index] = itemValue[index] + value;
    		}
    		catch (Exception e)
    		{
    			//
    		}
    	}
    	return;
    }

    /**
     *  カウントの総数を取得する
     * @return
     */
    public int getTotalValueCount()
    {
        int count = 0;
        for (int index = 0; index < NUMBER_OF_ITEMS; index++)
        {
        	count = count + itemValue[index];
        }
        return (count);
    }
    
    /**
     *  値の割合(パーセント) を文字列で応答する
     * @param index
     * @return
     */
    public String getValuePercent(int index)
    {
    	int value = getItemValue(index);
    	int totalCount = getTotalValueCount();
    	if (value < 0)
    	{
    		return ("????");
    	}
        if (totalCount == 0)
        {
        	return ("0.00");
        }
        String percent = ((double) value / (double) totalCount) * 100.0 + "";
        int dot = percent.indexOf(".");
        if ((dot > 0)&&((dot + 3) < percent.length()))
        {
            percent = percent.substring(0, dot + 3);
        }
        return (percent);
    }

    /**
     *  情報メッセージを取得する
     * @return
     */
    public String getInformationMessage()
    {
        return (informationMessage);
    }
    
    /**
     *   情報メッセージを設定する
     * @param message
     */
    public void setInformationMessage(String message)
    {
    	informationMessage = message;
    }    
    
    /**
     *   CSV形式のデータファイルを出力する
     *
     */
    public void outputDataToFile()
    {
        String fileName = getBaseDirectory() + "TheCount.csv";        
        String valueData = "; itemName, value" + "\r\n";
        for (int index = 0; index < NUMBER_OF_ITEMS; index++)
        {
            valueData = valueData + "\"" + getItemName(index) + "\"" + ", " + getItemValue(index) + "\r\n";
        }
        fileUtils.outputDataTextFile(fileName, null, null, valueData, false);
        return;
    }
}
