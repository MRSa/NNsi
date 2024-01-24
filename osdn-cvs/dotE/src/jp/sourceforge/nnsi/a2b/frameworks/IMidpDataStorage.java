package jp.sourceforge.nnsi.a2b.frameworks;

import javax.microedition.midlet.MIDlet;

/**
 *  �f�[�^�A�N�Z�X�N���X(�̃C���^�t�F�[�X) 
 *
 * @author MRSa
 */
public interface IMidpDataStorage
{
    /**
     *  �A�v���P�[�V���������擾�擾����
     *  @return �A�v���P�[�V������
     */
    public abstract String getApplicationName();

    /**
     *  �X�v���b�V���X�N���[���i�C���[�W���j���擾����
     *  @return �X�v���b�V���X�N���[����
     */
    public abstract String getSplashImageName();

    /**
     *  ��������
     *  @param object MIDlet�N���X
     */
    public abstract void prepare(MIDlet object);

    /**
     *  ���������i��ʁj
     *  @param selector �V�[���Z���N�^�N���X
     */
    public abstract void prepareScreen(MidpSceneSelector selector);

    /**
     *  ������ŃA�v�����I�����鎞�Ԃ��擾�i-1�̂Ƃ��͖����j
     *  @return ������ŏI�����鎞�ԊԊu�i�P�ʁFms�j
     */
    public abstract long getTimeoutCount();
    
    /**
     *  �f�[�^���o�b�N�A�b�v����
     * (�A�v���I�����ɌĂяo����܂�)
     */
    public abstract void backup();

    /**
     *  �f�[�^�����X�g�A����
     * (�A�v���J�n���ɌĂяo����܂�)
     */
    public abstract void restore();

    /**
     *  �f�[�^���N���[���A�b�v����
     *  (�A�v���I�����ɌĂяo����܂�)
     */
    public abstract void cleanup();
}
