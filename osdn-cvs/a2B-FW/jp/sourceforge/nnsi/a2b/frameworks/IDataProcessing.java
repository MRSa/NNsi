package jp.sourceforge.nnsi.a2b.frameworks;

/**
 *  データプロセッシング用インタフェース
 * 
 * @author MRSa
 *
 */
public interface IDataProcessing
{
    /**
     *  データ処理の開始(トリガ)
     *
     *  @param processingId 処理ID
     */
    public abstract void updateData(int processingId);

    /**
     *  処理進捗状況の表示
     * 
     * @param message     表示するメッセージ
     * @param mode        表示モード
     * @param forceUpdate 画面が更新されるまで待つ(true)か、そうでないか
     */
    public abstract void updateInformation(String message, int mode, boolean forceUpdate);

    /**
     *  処理の終了通知
     *  
     * @param processingId 処理ID
     * @param result     処理結果
     * @param vibrateTime  ブルブルさせるか(ms)
     */
    public abstract void finishUpdateData(int processingId, int result, int vibrateTime);

}
