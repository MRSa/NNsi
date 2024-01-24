package jp.sourceforge.nnsi.a2b.utilities;

/**
 *  MidpCyclicTimer のタイムアウトを受信するインタフェースです。
 * 
 * @author MRSa
 *
 */
public interface IMidpCyclicTimerReport
{

    /**
     *  タイムアウト報告
     * 
     * @param count 報告回数
     * @return <code>true</code>:報告を継続、 <code>false</code>:報告を停止
     */	
	public abstract boolean reportCyclicTimeout(int count);
}
