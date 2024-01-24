package jp.sourceforge.nnsi.a2b.frameworks;
/**
 * ���O�o�̓N���X�̃C���^�t�F�[�X
 * (��ʕ\�� : BUSY�\���Ȃ�)
 * 
 * @author MRSa
 */
public interface ILogMessageDisplay
{
    /**
     *  ���O�o�̓��b�Z�[�W��ݒ肷��
     * 
     *  @param message �\�����郁�b�Z�[�W
     *  @param forceUpdate ��ʕ\���������I�ɍX�V����
     *  @param lockOperation ��ʑ�����֎~������
     */
    public abstract void outputInformationMessage(String message, boolean forceUpdate, boolean lockOperation);

    /**
     *  ���O�o�̓��b�Z�[�W�̃N���A(����֎~���N���A)
     *  
     */
    public abstract void clearInformationMessage();

    /**
     *  ���O�o�͏I��
     *  
     */
    public abstract void finishInformationMessage();
}
