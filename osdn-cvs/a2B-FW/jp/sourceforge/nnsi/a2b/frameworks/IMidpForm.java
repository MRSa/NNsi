package jp.sourceforge.nnsi.a2b.frameworks;

/**
 *  �t�H�[���N���X�̃x�[�X�C���^�t�F�[�X�ł�<br>
 *  �i�f�[�^���̓t�H�[���ɃA�N�Z�X����Ƃ��Ɏg�p���܂��B�j
 *   
 * @author MRSa
 *
 */
public interface IMidpForm
{
    /**
     *  ���ID(�t�H�[��ID)�̎擾
     *  @return ���ID
     */
    public abstract int getFormId();

    /**
     *  ��ʂ��ؑւ���ꂽ�Ƃ��̏���
     */
    public abstract void start();

    /**
     *  ��ʃ^�C�g����ݒ肷��
     *  @param title�@�ݒ肷���ʃ^�C�g��
     */
    public abstract void setTitleString(String title);

    /**
     *  OK�{�^����p�ӂ��邩��ݒ肷��
     *  @param isShow �p�ӂ���(true) / �p�ӂ��Ȃ�(false) 
     */
    public abstract void setOkButton(boolean isShow);

    /**
     *   Cancel�{�^����p�ӂ��邩�H
     *   @param isShow �p�ӂ���(true) / �p�ӂ��Ȃ�(false)
     */
    public abstract void setCancelButton(boolean isShow);
}
