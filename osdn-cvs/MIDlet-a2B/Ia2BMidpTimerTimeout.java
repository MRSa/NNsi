/**
 *  �^�C���A�E�g���o���ʒm�p�̃C���^�t�F�[�X�N���X
 * 
 * @author MRSa
 *
 */
public interface Ia2BMidpTimerTimeout
{
    // �^�C�}�[�̐���ݒ�
	static public final int TIMER_STOP     = -1; // �^�C�}�[�̏I��
    static public final int TIMER_PAUSE    = -2; // �^�C�}�[�̈ꎞ��~
    static public final int TIMER_EXTEND   =  0; // �^�C�}�[�̉���
    static public final int TIMER_CONTINUE = -3; // �^�C�}�[�̘A���^�C���A�E�g

    // �^�C���A�E�g�̌��o
	public abstract void detectTimeout(long status);
}
