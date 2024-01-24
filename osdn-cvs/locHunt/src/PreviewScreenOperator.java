import javax.microedition.lcdui.Image;
import jp.sourceforge.nnsi.a2b.screens.images.PictureScreenOperator;

public class PreviewScreenOperator extends PictureScreenOperator
{
	private LocHuntSceneStorage sceneStorage = null;
    private CameraDeviceController cameraDevice = null;
	
	/**
	 *   �R���X�g���N�^
	 *
	 */
    public PreviewScreenOperator(LocHuntSceneStorage sceneData, CameraDeviceController camera)
    {
    	super(null, null);
    	sceneStorage = sceneData;
    	cameraDevice = camera;
    }

    /**
     *  ���b�Z�[�W�f�[�^�̎擾
     *
     *  @return �\�����郁�b�Z�[�W
     */
    public String getMessage()
    {
        // 
        return ("Enter: �ۑ�, CLR: Cancel)");
    }
    /**
     *  �C���[�W�f�[�^�̎擾
     *
     *  @param useThumbnail JPEG�̃T���l�[���\���𗘗p���ăC���[�W�f�[�^�����邩
     *  @return �C���[�W�f�[�^
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
     *   ���̉�ʂ��ǂ��ɐ؂�ւ��邩���߂�
     * 
     * @param sceneId ���݂̉��ID
     * @return ���̉��ID
     */
    public int nextSceneToChange(int sceneId)
    {
        return (LocHuntSceneDB.DEFAULT_SCREEN);
    }

    /**
     *  �f�[�^��ۑ�����ݒ��ݒ肷��
     * @param data
     */
    public void setSaveAction(boolean data)
    {
        sceneStorage.setSaveAction(data);
    }
}
