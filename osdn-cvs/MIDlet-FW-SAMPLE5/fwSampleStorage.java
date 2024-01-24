import javax.microedition.midlet.MIDlet;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;

/**
 *  データストレージクラス
 * 
 * @author MRSa
 */
public class fwSampleStorage implements IMidpDataStorage
{
    // アプリケーション名とスプラッシュスクリーン
    static final private String MyMidpName = "キッチンタイマー";
    static final private String MyMidpSplashImageName = "/res/splash.png";

    // データストレージ名とバージョン
    private final String DATASTORAGE_NAME     = "fwSample";

    /***** 保管データ(ここから) *****/
    private int         settingTime  = 180;  // タイマ設定値を記録する
    private String       soundFile    = "/res/sound.mid"; // サウンドファイル名を記録する
    /***** 保管データ(ここまで) *****/

    /***** データベースレコード番号(ここから) *****/
    private final int    SETTIME_RECORDID     = 1;    // タイマ設定値のレコード番号
    private final int    SNDFILE_RECORDID     = 2;    // サウンドファイル名のレコード番号
    /***** データベースレコード番号(ここまで) *****/
    
    
    /***** レコードアクセサ *****/
    private MidpRecordAccessor dbAccessor = null;

    /**
     *   コンストラクタ
     * 
     * @param レコードアクセサ
     */
    public fwSampleStorage(MidpRecordAccessor accessor)
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
        // DBをオープンさせる
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }

        // タイマ設定秒数の記録
        dbAccessor.startCacheIntArray(1);
        dbAccessor.cacheRecordIntArray(0, settingTime);
        dbAccessor.writeRecordIntArray(SETTIME_RECORDID);

        // サウンドファイル名の記録
        dbAccessor.writeRecord(SNDFILE_RECORDID, soundFile.getBytes(), 0, (soundFile.getBytes()).length);

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
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        if (dbAccessor.startReadRecordIntArray(SETTIME_RECORDID) > 0)
        {
            settingTime = dbAccessor.readRecordIntArray(0);
        }
        byte[] sndFile = dbAccessor.readRecord(SNDFILE_RECORDID);
        if (sndFile != null)
        {
            soundFile = new String(sndFile);
            if (MidpFileOperations.IsExistFile(soundFile) == false)
            {
                // ファイルが存在しない場合には、サウンドファイル設定を消去する
                soundFile = null;
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
     *  サウンドファイル名を取得する
     * 
     * @return サウンドファイル名
     */
    public String getSoundFileName()
    {
        return (soundFile);
    }

    /**
     *  タイマ設定値を取得する
     * 
     * @return タイマ設定値
     */
    public int getTimerSetTime()
    {
        return (settingTime);
    }

    /**
     *  サウンドファイル名を設定する
     * 
     * @param fileName サウンドファイル名
     */
    public void setSoundFileName(String fileName)
    {
        soundFile = fileName;
    }

    /**
     *  タイマ設定値を設定する
     * 
     * @param time
     */
    public void setTimerSetTime(int time)
    {
        settingTime = time;
    }
}
