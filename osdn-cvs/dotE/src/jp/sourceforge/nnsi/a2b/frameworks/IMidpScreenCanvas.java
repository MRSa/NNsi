package jp.sourceforge.nnsi.a2b.frameworks;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * �L�����o�X�p�̃C���^�t�F�[�X�N���X(�Ǘ��p)
 * 
 * @author MRSa
 */
public interface IMidpScreenCanvas
{

    /**
     *  �N���X�̏������� 
     *
     *  @param font �\������t�H���g�T�C�Y
     */
    public abstract void prepare(Font font);                                    // �������\�b�h

    /**
     *  �`��͈͂�ݒ肷��
     *
     *  @param topX   ����X���W
     *  @param topY   ����Y���W
     *  @param width  �`�敝
     *  @param height �`�捂��
     */
    public abstract void setRegion(int topX, int topY, int width, int height);  // �`��͈͂̐ݒ胁�\�b�h

    /**
     *  �����̉��ID
     * 
     *  @return ���ID
     */
    public abstract int  getCanvasId();      // ������ID

    // ��ʍX�V
    /**
     *  ���(�`��)�̊J�n����
     */
    public abstract void start();            // ���(�`��)�̊J�n����

    /**
     *  ���(�`��)�̒�~����
     *
     */
    public abstract void stop();             // ���(�`��)�̒�~����

    /**
     * ��ʃy�C���g����
     * @param g �O���t�B�b�N�N���X
     */
    public abstract void paint(Graphics g);

    // �\���̈�
    /**
     *  ����[X���W����������
     *  
     *  @return ����[X���W
     */
    public abstract int  getTopX();          // ����[X

    /**
     *  ����[Y���W����������
     *  
     *  @return ����[Y���W
     */
    public abstract int  getTopY();          // ����[Y

    /**
     *  ��ʕ`�敝����������
     *  
     *  @return ��ʕ`�敝
     */
    public abstract int  getWidth();         // ��

    /**
     *  ��ʕ`�捂������������
     *  
     *  @return ��ʕ`�捂��
     */
    public abstract int  getHeight();        // ����

    // (����p)���\��
    /**
     *  �^�C�g����ݒ肷��
     *  
     *  @param title �\���^�C�g��
     */
    public abstract void setTitle(String title);                      // �^�C�g��

    /**
     *  ���\�����b�Z�[�W��ݒ肷��
     * 
     * @param message ���\�����b�Z�[�W
     */
    public abstract void setInformation(String message);              // ���\��

    /**
     *  ���X�V���\�b�h
     * 
     * @param message ���b�Z�[�W
     * @param mode ���[�h
     */
    public abstract void updateInformation(String message, int mode); // ���X�V

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��̏���
     */
    public abstract void showMenu();

    /**
     *  �I���{�^���������ꂽ�Ƃ��̏���
     * @return �I������(true) / �I�����Ȃ�(false)
     */
    public abstract boolean checkExit();
    
    /**
     * �X�V�����̎w��
     * @param processingId ����ID
     */
    public abstract void updateData(int processingId);

    /**
     * �X�V�����̏I���ʒm
     * @param processingId ����ID
     * @param result ��������
     */
    public abstract void finishUpdateData(int processingId, int result);
    
    // �L�[�̓��͏���

    /**
     *  ��L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputUp   (boolean isRepeat);

    /**
     *  ���L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputDown (boolean isRepeat);

    /**
     *  ���L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputLeft (boolean isRepeat);

    /**
     *  �E�L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputRight(boolean isRepeat);

    /**
     *  �Z���^�[�L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputFire (boolean isRepeat);

    // GAME�L�[

    /**
     *  GAME A�L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputGameA(boolean isRepeat);

    /**
     *  GAME B�L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputGameB(boolean isRepeat);

    /**
     *  GAME C�L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputGameC(boolean isRepeat);

    /**
     *  GAME D�L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputGameD(boolean isRepeat);

    /**
     *  ���L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputPound(boolean isRepeat);

    /**
     *  ���L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputStar(boolean isRepeat);

    /**
     *  �����L�[�̓��͏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputNumber(int number, boolean isRepeat);

    /**
     *  ���̑��̃L�[���͂̏���
     *  @param isRepeat �L�[���s�[�g���Ă��邩�itrue:�L�[���s�[�g���Ă���Afalse:�ʏ���́j
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean inputOther(int keyCode, boolean isRepeat);

    /**
     *  ��ʃ^�b�`���͏���
     *  @param x x���W
     *  @param y y���W
     *  @return ���̃��\�b�h�ŏ��������s����(true) / ���s���Ă��Ȃ�(false)
     */
    public abstract boolean pointerPressed(int x, int y);
}
