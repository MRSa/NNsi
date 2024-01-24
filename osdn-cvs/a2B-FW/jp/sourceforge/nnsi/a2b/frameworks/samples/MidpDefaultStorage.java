package jp.sourceforge.nnsi.a2b.frameworks.samples;

import javax.microedition.midlet.MIDlet;

import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;


/**
 * �f�[�^�A�N�Z�X�N���X�ł��B<br>
 * ���̃N���X�ŃA�v���N�����ɕۊǃf�[�^��ǂݏo������A
 * �A�v���I�����Ƀf�[�^��ۊǂ����肷��悤�ɂ��܂��B
 * 
 * @author MRSa
 *
 */
public class MidpDefaultStorage implements IMidpDataStorage
{
    protected String       applicationName = "a2B-FW";
    protected String       splashScreenFile = "/res/splash.png";
    protected MidpCanvas   baseCanvas = null;

    /**
     * �x�[�X�L�����o�X�N���X���L�����܂�
     * @param name �A�v���P�[�V�����̖��O
     * @param canvas �x�[�X�L�����o�X�N���X
     */
    public MidpDefaultStorage(String name, MidpCanvas canvas)
    {
        baseCanvas = canvas;
        applicationName = name;
    }

    /**
     * �x�[�X�L�����o�X�N���X�́AsetObjects()���\�b�h��
     * ����Ă�������
     *
     */
    public MidpDefaultStorage()
    {
        //
    }

    /**
     *  �I�u�W�F�N�g�N���X��������
     * 
     * @param canvas �L�����o�X�N���X
     */
    public void setObjects(MidpCanvas canvas)
    {
        baseCanvas = canvas;
        return;
    }

    /**
     *  �A�v���P�[�V���������擾�擾����
     *  preturn �A�v���P�[�V������
     */
    public String getApplicationName()
    {
        return (applicationName);
    }

    /**
     *  �X�v���b�V���X�N���[���i�C���[�W���j���擾����
     *  @return �X�v���b�V���X�N���[���t�@�C����
     */
    public String getSplashImageName()
    {
        return (splashScreenFile);
    }

    /**
     *  ����(MIDlet)
     *  @param object MIDlet�N���X
     */
    public void prepare(MIDlet object)
    {
        return;
    }

    /**
     *  �����i��ʁj
     *  @param selector �V�[���Z���N�^
     *  
     */
    public void prepareScreen(MidpSceneSelector selector)
    {
        return;
    }

    /**
     *  �A�v�������I���^�C���A�E�g�̃J�E���g���擾
     *  @return �A�v�������I���̃^�C���A�E�g�l(�P�ʁFms)�A���̎��ɂ͖����B
     */
    public long getTimeoutCount()
    {
        return (-1);
    }

    /**
     *  �f�[�^���o�b�N�A�b�v����
     *  
     */
    public void backup()
    {
        return;
    }

    /**
     *  �f�[�^�����X�g�A����
     *  
     */
    public void restore()
    {
        return;
    }

    /**
     *  �f�[�^���N���[���A�b�v����
     *  
     */
    public void cleanup()
    {
        return;
    }
}
