import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.frameworks.samples.*;
import javax.microedition.midlet.MIDlet;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;

/**
 *  �T���v���p�t�@�N�g���N���X
 *  
 *  @author MRSa
 */
public class fwSampleFactory implements IMidpFactory
{
    /** �f�t�H���g�̉��ID�ł� */
    public static final int DEFAULT_SCREEN = 100;
    private fwSampleSceneDB sceneDb = null;
    private MidpDefaultStorage dataStorage = null;
    
    /**
     *  �R���X�g���N�^�ł͉����������܂���
     */
    public fwSampleFactory()
    {
        //
    }

    /**
     *  �R�A�I�u�W�F�N�g�𐶐����܂��B<br>
     *  �����ł͍Œ���V�[���ؑ֗p�̃f�[�^�x�[�X�N���X(IMidpSceneDB)��
     *  �f�[�^�ۑ��p�N���X(IMidpDataStorage)�𐶐�����K�v������܂�
     *  
     *  @see IMidpSceneDB
     *  @see IMidpDataStorage
     */
    public void prepareCoreObjects()
    {
        // �V�[���ؑփf�[�^�N���X�̐���
        sceneDb = new fwSampleSceneDB();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new MidpDefaultStorage();
    }

    /**
     *  �V�[���ؑփf�[�^�N���X���������܂�
     * 
     *  @return �V�[���ؑփf�[�^�N���X
     */
    public IMidpSceneDB getSceneDB()
    {
        return (sceneDb);
    }

    /**
     *  �f�[�^�X�g���[�W�N���X���������܂�
     * 
     *  @return �f�[�^�X�g���[�W�N���X
     */
    public IMidpDataStorage getDataStorage()
    {
        return (dataStorage);
    }

    /**
     *  �R�A�I�u�W�F�N�g�ȊO�̃I�u�W�F�N�g�𐶐����܂�<br>
     * 
     *  @param object MIDlet�x�[�X�N���X
     *  @param selector �V�[���Z���N�^
     */
    public void prepareObjects(MIDlet object, MidpSceneSelector selector)
    {
        // ���(�x�[�X)�N���X : �L�����o�X
        MidpCanvas myCanvas = new MidpCanvas(selector, Font.SIZE_SMALL);
        myCanvas.prepare();

        // �x�[�X�N���X��ݒ肷��
        sceneDb.setScreenObjects(myCanvas);

        // ��ʃT�C�Y�̎擾
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        // ���C����ʂ̓o�^ (new���āA�t�H���g��ݒ肵�āA��ʃT�C�Y��ݒ肷��)
        IMidpScreenCanvas screenObject = null;
        screenObject = new fwSampleMainCanvas(DEFAULT_SCREEN, selector, new fwSampleMainStorage(myCanvas));
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
