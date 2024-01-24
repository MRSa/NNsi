package jp.sourceforge.nnsi.a2b.screens.images;

/**
 * 画像表示用に使用するデータ格納クラスのインタフェース定義
 * 
 * @author MRSa
 *
 */
public interface IPictureScreenStorage 
{
    /**
     *  表示すべきファイル名を取得する
     * 
     *  @return 表示すべきファイル名
     */
    public abstract String getFileNameToShow();

    /**
     *  次の画面切り替え先画面IDを取得する
     *  
     *  @param screenId (現在の)画面ID
     *  @return 切り替える画面ID
     */
    public abstract int nextSceneToChange(int screenId);
}
