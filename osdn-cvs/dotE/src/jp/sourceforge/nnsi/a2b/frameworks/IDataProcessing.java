package jp.sourceforge.nnsi.a2b.frameworks;

/**
 *  �f�[�^�v���Z�b�V���O�p�C���^�t�F�[�X
 * 
 * @author MRSa
 *
 */
public interface IDataProcessing
{
    /**
     *  �f�[�^�����̊J�n(�g���K)
     *
     *  @param processingId ����ID
     */
    public abstract void updateData(int processingId);

    /**
     *  �����i���󋵂̕\��
     * 
     * @param message     �\�����郁�b�Z�[�W
     * @param mode        �\�����[�h
     * @param forceUpdate ��ʂ��X�V�����܂ő҂�(true)���A�����łȂ���
     */
    public abstract void updateInformation(String message, int mode, boolean forceUpdate);

    /**
     *  �����̏I���ʒm
     *  
     * @param processingId ����ID
     * @param result     ��������
     * @param vibrateTime  �u���u�������邩(ms)
     */
    public abstract void finishUpdateData(int processingId, int result, int vibrateTime);

}
