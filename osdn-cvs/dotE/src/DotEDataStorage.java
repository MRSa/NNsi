import javax.microedition.lcdui.Font;
import javax.microedition.midlet.MIDlet;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;;

/**
 *  データストレージクラス
 * 
 * @author MRSa
 */
public class DotEDataStorage implements IMidpDataStorage
{
    // アプリケーション名とスプラッシュスクリーン
    static final private String MyMidpName = "dotE:ドット絵ペインタ";
    static final private String MyMidpSplashImageName = "/res/splash.png";
    static final private int    MySize = 32;
    static final public  int    ASCEND = 1;    // 昇順
    static final public  int    DESCEND = -1;  // 降順
    static final public  int    OTHER = 0;     // その他

    // データストレージ名とバージョン
    private final String DATASTORAGE_NAME     = "dotE";
    private final int    DATASTORAGE_VERSION  = 101;

    /***** データベースレコード番号(ここから) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DBのバージョン格納レコード
    private final int    BASEDIR_RECORDID     = 2;    // preference DBのベースディレクトリ格納レコード
    private final int    DATAFILE_RECORDID    = 3;    // preference DBのデータファイル名格納レコード
    private final int    PREFERENCE_RECORDID  = 4;    // preference DBのオプションデータ格納レコード
    /***** データベースレコード番号(ここまで) *****/

    /***** 保管データ(ここから) *****/
    private String       baseDirectory   = "file:///";     // ベースディレクトリ名を記録する
    private String       dataFileName    = "default.bmp";  // ファイル名称を記録する
    private int          paintColor     = 0x00000000;     // 記録する色
    private int          clearColor     = 0x00ffffff;     // クリアする色
    private int          preference     = 0;              // オプションデータ
    private int          fontSize       = Font.SIZE_SMALL;
    /***** 保管データ(ここまで) *****/

    /***** レコードアクセサ *****/
    private MidpRecordAccessor dbAccessor = null;

    /*** その他データ ***/
    private String informationMessage = "";
    private int        sizeX = MySize;
    private int        sizeY = MySize;
    private int[][]    dotEPointData = null;

    private int        copyAreaX = 0;
    private int        copyAreaY = 0;
    private int[][]    copyPasteArea = null;

    /**
     *   コンストラクタ
     * 
     * @param レコードアクセサ
     */
    public DotEDataStorage(MidpRecordAccessor accessor)
    {
        dbAccessor = accessor;
        dotEPointData = new int[sizeX][sizeY];
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
        // 初期カラーデータを設定する
    	clearAllPointData();
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

        // データファイル名の記録
        dbAccessor.writeRecord(DATAFILE_RECORDID, dataFileName.getBytes(), 0, (dataFileName.getBytes()).length);

        // 設定データ、ペイント色の記録
        dbAccessor.startCacheIntArray(3);
        dbAccessor.cacheRecordIntArray(0, paintColor);
        dbAccessor.cacheRecordIntArray(1, clearColor);
        dbAccessor.cacheRecordIntArray(2, preference);
        dbAccessor.writeRecordIntArray(PREFERENCE_RECORDID);

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
            // バージョン一致、レコードを読み込む
            byte[] dir = dbAccessor.readRecord(BASEDIR_RECORDID);
            if (dir != null)
            {
                baseDirectory = new String(dir);
            }
            byte[] fileName = dbAccessor.readRecord(DATAFILE_RECORDID);
            if (fileName != null)
            {
            	dataFileName = new String(fileName);
            }
            if (dbAccessor.startReadRecordIntArray(PREFERENCE_RECORDID) > 0)
            {
            	paintColor = dbAccessor.readRecordIntArray(0);
                clearColor = dbAccessor.readRecordIntArray(1);
                preference = dbAccessor.readRecordIntArray(2);
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
     *  データファイル名を取得する
     * 
     * @return データファイル名
     */
    public String getDataFileName()
    {
        return (dataFileName);
    }

    /**
     *   情報メッセージを応答する
     * @return 情報メッセージ
     */
    public String getInformationMessage()
    {
        return (informationMessage);
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
     *   ファイル名を設定する
     * 
     * @param fileName 設定するファイル名
     */
    public void setDataFileName(String fileName)
    {
    	dataFileName = fileName;
    }

    /**
     *  お気に入り設定データを設定する
     * @param preferenceData
     */
    public void setPreference(int preferenceData)
    {
        preference = preferenceData;
    }

    /**
     *  お気に入り設定データを取得する
     *
     */
    public int getPreference()
    {
        return (preference);
    }

    /**
     *  塗る色を設定する
     * @param color  色
     */
    public void setPaintColor(int color)
    {
    	paintColor = color;
    }

    /**
     *  塗る色を応答する
     * @return 色
     */
    public int getPaintColor()
    {
        return (paintColor);
    }
    

    /**
     *  情報メッセージの設定
     * @param message 情報メッセージ
     */
    public void setInformationMessage(String message)
    {
        informationMessage = message;
    }
    
    /**
     *  横幅サイズを応答する
     * @return 横幅サイズ
     */
    public int getSizeX()
    {
        return (sizeX);
    }

    /**
     *  縦幅サイズを応答する
     * @return
     */
    public int getSizeY()
    {
    	return (sizeY);
    }

    /**
     *   データを取得する
     * @param x  X座標
     * @param y  Y座標
     * @return   座標におけるカラー情報
     */
    public int getData(int x, int y)
    {
        if ((x >= sizeX)||(y >= sizeY))
        {
        	return (0x00000000);
        }
    	return (dotEPointData[x][y]);
    }

    /**
     *   カラーデータを設定する
     * @param x x座標
     * @param y y座標
     */
    public void setData(int x, int y)
    {
        if ((x >= sizeX)||(y >= sizeY))
        {
        	// サイズオーバー、何もしない
        	return;
        }
    	dotEPointData[x][y] = paintColor;
    }
    
    /**
     *   カラーデータを設定する
     * @param x x座標
     * @param y y座標
     * @param data カラーデータ
     */
    public void setData(int x, int y, int data)
    {
        if ((x >= sizeX)||(y >= sizeY))
        {
        	// サイズオーバー、何もしない
        	return;
        }
    	dotEPointData[x][y] = data;
    }

    /**
     *  カラーデータをクリアする
     * @param x x座標
     * @param y y座標
     */
    public void clearData(int x, int y)
    {
        if ((x >= sizeX)||(y >= sizeY))
        {
        	// サイズオーバー、何もしない
        	return;
        }
    	dotEPointData[x][y] = clearColor;
    }

    /**
     *  カラーデータをすべてクリアする
     */
    public void clearAllPointData()
    {
    	for (int x = 0; x < sizeX; x++)
    	{
    		for (int y = 0; y < sizeY; y++)
    		{
    			dotEPointData[x][y] = clearColor;
    		}
    	}
    	return;
    }
    
    /**
     *  色を変更する
     * @param changeOrder
     */
    public void changeColor(int changeOrder)
    {
        //  paintColor
    	int red = (paintColor & 0x00ff0000) >> 16;
    	int green = (paintColor & 0x0000ff00) >> 8;
    	int blue = (paintColor & 0x000000ff);
    	
    	switch (changeOrder)
    	{
    	  case ASCEND:
    		// blue
      		blue = (blue + 0x20) & 0x000000f0;
      		if (blue != 0)
      		{
      			blue = blue + 0x0f;
      		}
      		break;

    	  case DESCEND:
    		// red
      		red = (red + 0x20) & 0x000000f0;
      		if (red != 0)
      		{
      			red = red + 0x0f;
      		}
    		break;

    	  case OTHER:
    	  default:
    		// green
    		green = (green + 0x20) & 0x000000f0;
    		if (green != 0)
      		{
      			green = green + 0x0f;
      		}
      		break;
    	}
    	paintColor = blue + (green << 8) + (red << 16);
    	return;
    }

    /**
     *   エリアをクリアする
     */
    public void clearCopyAreaData()
    {
    	copyPasteArea = null;
    	copyAreaX = 0;
    	copyAreaY = 0;
    	System.gc();
    }
    
    /**
     *   領域をコピーする
     * @param topX    左上(X軸)
     * @param topY    右上(Y軸)
     * @param width   コピー幅
     * @param height  コピー高さ
     */
    public void copyAreaData(int topX, int topY, int width, int height)
    {
        int areaX = 0;
        int areaY = 0;
    	
    	clearCopyAreaData();

    	copyAreaX = width;
    	copyAreaY = height;
        copyPasteArea = new int[copyAreaX][copyAreaY];
    	for (int x = topX; x < (topX + width); x++)
    	{
    		for (int y = topY; y < (topY + height); y++)
    		{
                copyPasteArea[areaX][areaY] = dotEPointData[x][y];
                areaY++;
    		}
    		areaX++;
    		areaY = 0;
    	}
    	return;
    }

    /**
     *  領域をペーストする
     * @param startX  左上座標(X軸)
     * @param startY  左上座標(Y軸)
     */
    public void pasteAreaData(int startX, int startY)
    {
    	int areaX = 0;
    	int areaY = 0;
    	for (int x = startX; x < sizeX; x++)
    	{
    		for (int y = startY; y < sizeY; y++)
    		{
    			try
    			{
        			dotEPointData[x][y] = copyPasteArea[areaX][areaY];
                    areaY++;
                    if (areaY >= copyAreaY)
                    {
                    	break;
                    }    				
    			}
    			catch (Exception ex)
    			{
    				return;
    			}
    		}
    		areaX++;
    		if (areaX >= copyAreaX)
    		{
    			return;
    		}
    		areaY = 0;
    	}
    	return;
    }
}
