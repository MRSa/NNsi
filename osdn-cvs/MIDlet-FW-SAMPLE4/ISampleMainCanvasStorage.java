/**
 *   ���C����ʂ��K�v�Ƃ���f�[�^���o�͂̒�`
 *   (�C���^�t�F�[�X�N���X)
 * 
 * @author MRSa
 *
 */
public interface ISampleMainCanvasStorage
{
    /** �^�C�}�[���X�^�[�g����̂�҂��Ă����� */
    static public final int TIMER_READY = 0;

    /** �^�C�}�[���J�n����āA�J�E���g�_�E�����Ă����� */
    static public final int TIMER_STARTED = 1;
    
    /** �^�C���A�E�g���Ԃ����āA���[�U�ɒm�点�Ă����� */
    static public final int TIMER_TIMEOUT = 2;

    /** �^�C�}�[���ꎞ��~���̏�� */
    static public final int TIMER_PAUSED  = 3;
    
    /**
     *   �^�C�}�[��Ԃ���������
     * 
     * @return �^�C�}�[���
     */
    public abstract int getTimerStatus();

    /**
     *  �^�C�}�[�ݒ�b������������
     * 
     * @return �ݒ�b��
     */
    public abstract int getSetTimeSeconds();
    
    /**
     *   �^�C���A�E�g����܂ł̎c��b������������
     * @return �c��b���A���̂Ƃ��̓^�C���A�E�g������
     */
    public abstract int getRestSeconds();

    /**
     *   ���s/��~���w�����ꂽ�Ƃ��̏���
     * @return <code>true</code>:�w����t / <code>false</code>:�w���ł����Ԃł͂Ȃ�
     */
    public abstract boolean triggered();

    /**
     *   �^�C�}�[���Z�b�g���w�����ꂽ�Ƃ��̏���
     * @return <code>true</code>:�w����t / <code>false</code>:�w���ł����Ԃł͂Ȃ�
     */
    public abstract boolean reset();
}
