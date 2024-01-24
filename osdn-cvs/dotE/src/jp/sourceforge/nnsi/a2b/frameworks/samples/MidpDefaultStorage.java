package jp.sourceforge.nnsi.a2b.frameworks.samples;

import javax.microedition.midlet.MIDlet;

import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;


/**
 * データアクセスクラスです。<br>
 * このクラスでアプリ起動時に保管データを読み出したり、
 * アプリ終了時にデータを保管したりするようにします。
 * 
 * @author MRSa
 *
 */
public class MidpDefaultStorage implements IMidpDataStorage
{
    protected String       applicationName = "a2B-FW";
    protected String       splashScreenFile = "/res/splash.png";
    protected MidpCanvas   baseCanvas = null;

    /**
     * ベースキャンバスクラスを記憶します
     * @param name アプリケーションの名前
     * @param canvas ベースキャンバスクラス
     */
    public MidpDefaultStorage(String name, MidpCanvas canvas)
    {
        baseCanvas = canvas;
        applicationName = name;
    }

    /**
     * ベースキャンバスクラスは、setObjects()メソッドで
     * 入れてください
     *
     */
    public MidpDefaultStorage()
    {
        //
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
     *  データをバックアップする
     *  
     */
    public void backup()
    {
        return;
    }

    /**
     *  データをリストアする
     *  
     */
    public void restore()
    {
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
