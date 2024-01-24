import javax.microedition.lcdui.Font;
import javax.microedition.midlet.MIDlet;
import jp.sourceforge.nnsi.a2b.framework.core.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;;

/**
 *  データストレージクラス
 * 
 * @author MRSa
 */
public class HiMemoDataStorage implements IMidpDataStorage
{
    // アプリケーション名とスプラッシュスクリーン
    static final private String MyMidpName = "追記メモ";
    static final private String MyMidpSplashImageName = "/res/splash.png";

    // データストレージ名とバージョン
    private final String DATASTORAGE_NAME     = "hiMEMO";
    private final int    DATASTORAGE_VERSION  = 100;

    /***** データベースレコード番号(ここから) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DBのバージョン格納レコード
    private final int    BASEDIR_RECORDID     = 2;    // preference DBのベースディレクトリ格納レコード
    /***** データベースレコード番号(ここまで) *****/

    /***** 保管データ(ここから) *****/
    private String       baseDirectory = null; // ベースディレクトリ名を記録する
    private int          fontSize     = Font.SIZE_MEDIUM;
    /***** 保管データ(ここまで) *****/

    /***** レコードアクセサ *****/
    private MidpRecordAccessor dbAccessor = null;

    /**
     *   コンストラクタ
     * 
     * @param レコードアクセサ
     */
    public HiMemoDataStorage(MidpRecordAccessor accessor)
    {
        dbAccessor = accessor;
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
}
