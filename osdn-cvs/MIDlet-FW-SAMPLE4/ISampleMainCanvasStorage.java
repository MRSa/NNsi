/**
 *   メイン画面が必要とするデータ入出力の定義
 *   (インタフェースクラス)
 * 
 * @author MRSa
 *
 */
public interface ISampleMainCanvasStorage
{
    /** タイマーがスタートするのを待っている状態 */
    static public final int TIMER_READY = 0;

    /** タイマーが開始されて、カウントダウンしている状態 */
    static public final int TIMER_STARTED = 1;
    
    /** タイムアウト時間が来て、ユーザに知らせている状態 */
    static public final int TIMER_TIMEOUT = 2;

    /** タイマーが一時停止中の状態 */
    static public final int TIMER_PAUSED  = 3;
    
    /**
     *   タイマー状態を応答する
     * 
     * @return タイマー状態
     */
    public abstract int getTimerStatus();

    /**
     *  タイマー設定秒数を応答する
     * 
     * @return 設定秒数
     */
    public abstract int getSetTimeSeconds();
    
    /**
     *   タイムアウトするまでの残り秒数を応答する
     * @return 残り秒数、負のときはタイムアウト発生中
     */
    public abstract int getRestSeconds();

    /**
     *   実行/停止が指示されたときの処理
     * @return <code>true</code>:指示受付 / <code>false</code>:指示できる状態ではない
     */
    public abstract boolean triggered();

    /**
     *   タイマーリセットが指示されたときの処理
     * @return <code>true</code>:指示受付 / <code>false</code>:指示できる状態ではない
     */
    public abstract boolean reset();
}
