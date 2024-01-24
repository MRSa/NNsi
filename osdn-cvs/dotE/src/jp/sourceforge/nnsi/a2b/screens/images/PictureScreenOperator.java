package jp.sourceforge.nnsi.a2b.screens.images;
import javax.microedition.lcdui.Image;

import jp.sourceforge.nnsi.a2b.utilities.MidpFileMediaUtils;
/**
 * イメージ表示画面操作クラス
 * 
 * @author MRSa
 */
public class PictureScreenOperator
{
    private IPictureScreenStorage storage = null;
    private MidpFileMediaUtils mediaUtils = null;
    
    /**
     * コンストラクタでは特に何もしません
     * 
     * @param object イメージ表示画面用記録クラス
     * @param utils イメージ読み込み用ユーティリティ
     */
    public PictureScreenOperator(IPictureScreenStorage object, MidpFileMediaUtils utils)
    {
        //
        storage    = object;
        mediaUtils = utils;
    }

    /**
     *  クラスの実行準備（現状、なにもしない）
     *
     */
    public void prepare()
    {

    }

    /**
     *  メッセージデータの取得
     *
     *  @return 表示するメッセージ
     */
    public String getMessage()
    {
        // 
        return (storage.getFileNameToShow());
    }

    /**
     *  イメージデータの取得
     *
     *  @param useThumbnail JPEGのサムネール表示を利用してイメージデータをつくるか
     *  @return イメージデータ
     */
    public Image getImage(boolean useThumbnail)
    {
        String fileName = storage.getFileNameToShow();
        if (useThumbnail == true)
        {
            // JPEGのサムネールを利用してImageデータを作成する
            return (mediaUtils.LoadJpegThumbnailImageFromFile(fileName));
        }
        return (mediaUtils.LoadImageFromFile(fileName));
    }

    /**
     * 画面表示の終了(何もしない)
     *
     */
    public void cancelPictureView()
    {
        return;
    }

    /**
     *   次の画面をどこに切り替えるか決める
     * 
     * @param sceneId 現在の画面ID
     * @return 次の画面ID
     */
    public int nextSceneToChange(int sceneId)
    {
        return (storage.nextSceneToChange(sceneId));
    }
}
