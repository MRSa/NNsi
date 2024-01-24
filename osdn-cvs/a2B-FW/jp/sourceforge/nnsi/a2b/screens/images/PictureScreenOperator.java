package jp.sourceforge.nnsi.a2b.screens.images;
import javax.microedition.lcdui.Image;

import jp.sourceforge.nnsi.a2b.utilities.MidpFileMediaUtils;
/**
 * �C���[�W�\����ʑ���N���X
 * 
 * @author MRSa
 */
public class PictureScreenOperator
{
    private IPictureScreenStorage storage = null;
    private MidpFileMediaUtils mediaUtils = null;
    
    /**
     * �R���X�g���N�^�ł͓��ɉ������܂���
     * 
     * @param object �C���[�W�\����ʗp�L�^�N���X
     * @param utils �C���[�W�ǂݍ��ݗp���[�e�B���e�B
     */
    public PictureScreenOperator(IPictureScreenStorage object, MidpFileMediaUtils utils)
    {
        //
        storage    = object;
        mediaUtils = utils;
    }

    /**
     *  �N���X�̎��s�����i����A�Ȃɂ����Ȃ��j
     *
     */
    public void prepare()
    {

    }

    /**
     *  ���b�Z�[�W�f�[�^�̎擾
     *
     *  @return �\�����郁�b�Z�[�W
     */
    public String getMessage()
    {
        // 
        return (storage.getFileNameToShow());
    }

    /**
     *  �C���[�W�f�[�^�̎擾
     *
     *  @param useThumbnail JPEG�̃T���l�[���\���𗘗p���ăC���[�W�f�[�^�����邩
     *  @return �C���[�W�f�[�^
     */
    public Image getImage(boolean useThumbnail)
    {
        String fileName = storage.getFileNameToShow();
        if (useThumbnail == true)
        {
            // JPEG�̃T���l�[���𗘗p����Image�f�[�^���쐬����
            return (mediaUtils.LoadJpegThumbnailImageFromFile(fileName));
        }
        return (mediaUtils.LoadImageFromFile(fileName));
    }

    /**
     * ��ʕ\���̏I��(�������Ȃ�)
     *
     */
    public void cancelPictureView()
    {
        return;
    }

    /**
     *   ���̉�ʂ��ǂ��ɐ؂�ւ��邩���߂�
     * 
     * @param sceneId ���݂̉��ID
     * @return ���̉��ID
     */
    public int nextSceneToChange(int sceneId)
    {
        return (storage.nextSceneToChange(sceneId));
    }
}
