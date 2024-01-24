/**
 *  タイムアウト検出時通知用のインタフェースクラス
 * 
 * @author MRSa
 *
 */
public interface Ia2BMidpTimerTimeout
{
    // タイマーの制御設定
	static public final int TIMER_STOP     = -1; // タイマーの終了
    static public final int TIMER_PAUSE    = -2; // タイマーの一時停止
    static public final int TIMER_EXTEND   =  0; // タイマーの延長
    static public final int TIMER_CONTINUE = -3; // タイマーの連続タイムアウト

    // タイムアウトの検出
	public abstract void detectTimeout(long status);
}
