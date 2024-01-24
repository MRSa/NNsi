package jp.sourceforge.nnsi.a2b.frameworks;

import javax.microedition.midlet.MIDlet;

/**
 *  データアクセスクラス(のインタフェース) 
 *
 * @author MRSa
 */
public interface IMidpDataStorage
{
    /**
     *  アプリケーション名を取得取得する
     *  @return アプリケーション名
     */
    public abstract String getApplicationName();

    /**
     *  スプラッシュスクリーン（イメージ名）を取得する
     *  @return スプラッシュスクリーン名
     */
    public abstract String getSplashImageName();

    /**
     *  準備処理
     *  @param object MIDletクラス
     */
    public abstract void prepare(MIDlet object);

    /**
     *  準備処理（画面）
     *  @param selector シーンセレクタクラス
     */
    public abstract void prepareScreen(MidpSceneSelector selector);

    /**
     *  無操作でアプリを終了する時間を取得（-1のときは無効）
     *  @return 無操作で終了する時間間隔（単位：ms）
     */
    public abstract long getTimeoutCount();
    
    /**
     *  データをバックアップする
     * (アプリ終了時に呼び出されます)
     */
    public abstract void backup();

    /**
     *  データをリストアする
     * (アプリ開始時に呼び出されます)
     */
    public abstract void restore();

    /**
     *  データをクリーンアップする
     *  (アプリ終了時に呼び出されます)
     */
    public abstract void cleanup();
}
