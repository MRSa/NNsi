import javax.microedition.lcdui.Font;
import javax.microedition.midlet.MIDlet;

import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationDefaultStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpDefaultA2BhttpCommunicator;;

/**
 * データアクセスクラスです。<br>
 * このクラスでアプリ起動時に保管データを読み出したり、
 * アプリ終了時にデータを保管したりするようにします。
 * 
 * @author MRSa
 *
 */
public class TVgDataStorage implements IMidpDataStorage
{
    protected String     applicationName  = "TV-PGM";
    protected String     splashScreenFile = "/res/splash.png";
    protected MidpCanvas baseCanvas       = null;

    // データストレージ名とバージョン
    private final String DATASTORAGE_NAME     = "TVguide";
    private final int    DATASTORAGE_VERSION  = 101;

    /***** データベースレコード番号(ここから) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DBのバージョン格納レコード
    private final int    BASEDIR_RECORDID     = 2;    // preference DBのベースディレクトリ格納レコード
    private final int    PREFER_RECORDID      = 3;    // preference DBのデータレコード
    private final int    AREANAME_RECORDID    = 4;    // preference DBの地域名称
    /***** データベースレコード番号(ここまで) *****/

    private MidpRecordAccessor dbAccessor = null;
    private MidpDefaultA2BhttpCommunicator httpComm = null;
    private String baseDirectory = null;
    private String toGetUrl      = "http://tv.so-net.ne.jp/guest/pc/chart.action?&span=24&head=";
    private String areaString    = "東京";
    private int areaId           = 13; // 東京
    private int stationId        = 0;
    private int stationType      = 0;  // 地上波

    InformationDefaultStorage infoStorage = null;
    MidpResourceFileUtils     resUtils    = null;
    
    /**
     * ベースキャンバスクラスを記憶します
     * @param name アプリケーションの名前
     * @param canvas ベースキャンバスクラス
     */
    public TVgDataStorage(MidpRecordAccessor accessor)
    {
        dbAccessor = accessor;
    }

    /**
     *  オブジェクトクラスを初期化
     * 
     * @param canvas キャンバスクラス
     */
    public void setObjects(MidpCanvas canvas)
    {
        baseCanvas = canvas;
        return;
    }
    
    public void setHttpCommunicator(MidpDefaultA2BhttpCommunicator comm)
    {
        httpComm = comm;
    }
    
    /**
     *  アプリケーション名を取得取得する
     *  preturn アプリケーション名
     */
    public String getApplicationName()
    {
        return (applicationName);
    }

    /**
     *  スプラッシュスクリーン（イメージ名）を取得する
     *  @return スプラッシュスクリーンファイル名
     */
    public String getSplashImageName()
    {
        return (splashScreenFile);
    }

    /**
     *  準備(MIDlet)
     *  @param object MIDletクラス
     */
    public void prepare(MIDlet object)
    {
        return;
    }

    /**
     *  リソースユーティリティを記憶する
     * 
     * @param utils
     */
    public void setResourceUtils(MidpResourceFileUtils utils)
    {
        resUtils = utils;
    }
    
    /**
     *  準備（画面）
     *  @param selector シーンセレクタ
     *  
     */
    public void prepareScreen(MidpSceneSelector selector)
    {
        return;
    }

    /**
     *  アプリ自動終了タイムアウトのカウントを取得
     *  @return アプリ自動終了のタイムアウト値(単位：ms)、負の時には無効。
     */
    public long getTimeoutCount()
    {
        return (-1);
    }

    /**
     *  使用するフォントサイズを応答する
     * 
     * @return 使用するフォントサイズ
     */
    public int getFontSize()
    {
        return (Font.SIZE_SMALL);
//        return (Font.SIZE_MEDIUM);
    }

    /**
     *   地域IDを取得する
     * 
     * @return 地域ID
     */
    public int getAreaId()
    {
        return (areaId);
    }
    
    /**
     *   地域名称を取得する
     * @return 地域名称
     */
    public String getAreaName()
    {
        return (areaString);
    }

    /**
     *  地域IDと名称を設定する
     * 
     * @param id 地域ID
     * @param areaName 地域名称
     */
    public void setAreaId(int id, String areaName)
    {
        areaId = id;
        areaString = areaName;
        String cookie = "areaId=" + id + "; span=24";
        httpComm.SetCookie(cookie);
    }

    /**
     *  取得先URLを取得する
     * @return 取得先URL
     */
    public String getUrlToGet()
    {
        return (toGetUrl);
    }

    /**
     *  取得先URLを設定する
     * @param url 取得先URL
     */
    public void setUrlToGet(String url)
    {
        toGetUrl = url;
    }

    /**
     *  情報ストレージクラスを記憶する
     *  
     * @param storage 情報ストレージクラス
     */
    public void setInformationStorage(InformationDefaultStorage storage)
    {
        infoStorage = storage;
        try
        {
            infoStorage.setInformationMessageToShow(resUtils.getResourceText("/res/welcome.txt"));
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }

    public void setTvStation(int stationNumber)
    {
        stationId = stationNumber;
    }
    
    public int getTvStation()
    {
        return (stationId);
    }

    public void setTvType(int stationNumber)
    {
        stationType = stationNumber;
    }

    public int getTvType()
    {
        return (stationType);
    }

    public String getTvTypeString()
    {
        String name = "";
        if (stationType == 0)
        {
            name = "地上波";
        }
        else if (stationType == 1)
        {
            name = "UHF/BS";
        }
        else if (stationType == 2)
        {
            name = "BSデジタル";
        }
        return (name);
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
     *   ベースディレクトリ名を設定する
     * 
     * @param directory ベースディレクトリ名
     */
    public void setBaseDirectory(String directory)
    {
        baseDirectory = directory;
    }

    /**
     *  データをバックアップする
     *  
     */
    public void backup()
    {
        int dummy = 0;

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

        // データの記録
        dbAccessor.startCacheIntArray(4);
        dbAccessor.cacheRecordIntArray(0, areaId);
        dbAccessor.cacheRecordIntArray(1, stationId);
        dbAccessor.cacheRecordIntArray(2, stationType);
        dbAccessor.cacheRecordIntArray(3, dummy);
        dbAccessor.writeRecordIntArray(PREFER_RECORDID);

        // 地域名称の記録
        dbAccessor.writeRecord(AREANAME_RECORDID, areaString.getBytes(), 0, (areaString.getBytes()).length);

        // DBをクローズする
        dbAccessor.closeDatabase();
        return;

    }

    /**
     *  データをリストアする
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
            
            int recId = dbAccessor.startReadRecordIntArray(PREFER_RECORDID);
            if (recId > 1)
            {
                areaId = dbAccessor.readRecordIntArray(0);
            }
            if (recId > 2)
            {
                stationId = dbAccessor.readRecordIntArray(1);
            }
            if (recId > 3)
            {
                stationType = dbAccessor.readRecordIntArray(2);
            }
            byte[] areaName = dbAccessor.readRecord(AREANAME_RECORDID);
            if (areaName != null)
            {
                areaString = new String(areaName);
            }
            dir = null;
            areaName = null;
        }
        dbAccessor.closeDatabase();
        return;
    }

    /**
     *  データをクリーンアップする
     *  
     */
    public void cleanup()
    {
        return;
    }
}
