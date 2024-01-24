package jp.sourceforge.nnsi.a2b.frameworks;
/**
 * ログ出力クラスのインタフェース
 * (画面表示 : BUSY表示など)
 * 
 * @author MRSa
 */
public interface ILogMessageDisplay
{
    /**
     *  ログ出力メッセージを設定する
     * 
     *  @param message 表示するメッセージ
     *  @param forceUpdate 画面表示を強制的に更新する
     *  @param lockOperation 画面操作を禁止させる
     */
    public abstract void outputInformationMessage(String message, boolean forceUpdate, boolean lockOperation);

    /**
     *  ログ出力メッセージのクリア(操作禁止もクリア)
     *  
     */
    public abstract void clearInformationMessage();

    /**
     *  ログ出力終了
     *  
     */
    public abstract void finishInformationMessage();
}
