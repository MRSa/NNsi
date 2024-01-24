import javax.microedition.lcdui.Image;
import jp.sourceforge.nnsi.a2b.screens.images.PictureScreenOperator;

public class PreviewScreenOperator extends PictureScreenOperator
{
	private LocHuntSceneStorage sceneStorage = null;
    private CameraDeviceController cameraDevice = null;
	
	/**
	 *   コンストラクタ
	 *
	 */
    public PreviewScreenOperator(LocHuntSceneStorage sceneData, CameraDeviceController camera)
    {
    	super(null, null);
    	sceneStorage = sceneData;
    	cameraDevice = camera;
    }

    /**
     *  メッセージデータの取得
     *
     *  @return 表示するメッセージ
     */
    public String getMessage()
    {
        // 
        return ("Enter: 保存, CLR: Cancel)");
    }
    /**
     *  イメージデータの取得
     *
     *  @param useThumbnail JPEGのサムネール表示を利用してイメージデータをつくるか
     *  @return イメージデータ
     */
    public Image getImage(boolean useThumbnail)
    {
    	try
    	{
    		return (cameraDevice.getImage());
    	}
    	catch (Exception ex)
    	{
    		//
    	}
    	return (null);
    }

    /**
     *   次の画面をどこに切り替えるか決める
     * 
     * @param sceneId 現在の画面ID
     * @return 次の画面ID
     */
    public int nextSceneToChange(int sceneId)
    {
        return (LocHuntSceneDB.DEFAULT_SCREEN);
    }

    /**
     *  データを保存する設定を設定する
     * @param data
     */
    public void setSaveAction(boolean data)
    {
        sceneStorage.setSaveAction(data);
    }
}
