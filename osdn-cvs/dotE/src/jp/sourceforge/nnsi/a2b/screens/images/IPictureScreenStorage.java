package jp.sourceforge.nnsi.a2b.screens.images;

/**
 * �摜�\���p�Ɏg�p����f�[�^�i�[�N���X�̃C���^�t�F�[�X��`
 * 
 * @author MRSa
 *
 */
public interface IPictureScreenStorage 
{
    /**
     *  �\�����ׂ��t�@�C�������擾����
     * 
     *  @return �\�����ׂ��t�@�C����
     */
    public abstract String getFileNameToShow();

    /**
     *  ���̉�ʐ؂�ւ�����ID���擾����
     *  
     *  @param screenId (���݂�)���ID
     *  @return �؂�ւ�����ID
     */
    public abstract int nextSceneToChange(int screenId);
}
