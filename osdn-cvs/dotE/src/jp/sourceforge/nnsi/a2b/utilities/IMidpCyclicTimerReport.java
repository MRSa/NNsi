package jp.sourceforge.nnsi.a2b.utilities;

/**
 *  MidpCyclicTimer �̃^�C���A�E�g����M����C���^�t�F�[�X�ł��B
 * 
 * @author MRSa
 *
 */
public interface IMidpCyclicTimerReport
{

    /**
     *  �^�C���A�E�g��
     * 
     * @param count �񍐉�
     * @return <code>true</code>:�񍐂��p���A <code>false</code>:�񍐂��~
     */	
	public abstract boolean reportCyclicTimeout(int count);
}
