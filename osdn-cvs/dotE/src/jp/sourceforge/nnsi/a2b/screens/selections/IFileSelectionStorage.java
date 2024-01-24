package jp.sourceforge.nnsi.a2b.screens.selections;

/**
 *  �t�@�C���I����ʂ��g���A�f�[�^�A�N�Z�X�p�C���^�t�F�[�X
 * 
 * @author MRSa
 *
 */
public interface IFileSelectionStorage
{

    /**
	 *  �����f�B���N�g������������
	 *  
	 *  @return �����f�B���N�g��
	 */
    public abstract String getDefaultDirectory();

    /**
     *  ���ݑI������Ă���t�@�C������уf�B���N�g���̐�����������
     *  
     *  @return ���ݑI������Ă���t�@�C��(�f�B���N�g��)��
     */
    public abstract int getSelectedNameCount();
    
    /**
     *  �I���i�t�@�C���j������������
     * 
     *  @param index �I��ԍ�(0�X�^�[�g)
     *  @return �I��ԍ��̃t�@�C����
     */
    public abstract String getSelectedName(int index);

    /**
     *  �I���i�t�@�C���j����ݒ肷��
     * 
     * @param name �I�������t�@�C���̖��O
     */
    public abstract void setSelectedName(String name);
    
    /**
     *  �I���i�t�@�C���j�����N���A����
     *  
     */
    public abstract void clearSelectedName();
    
    /**
     *  �I���i�t�@�C���j�����m�肷��
     *  
     */
    public abstract void confirmSelectedName();
}
