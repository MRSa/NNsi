import jp.sourceforge.nnsi.a2b.screens.selections.*;
import jp.sourceforge.nnsi.a2b.forms.MidpDataInputForm;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileMediaUtils;
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
    
    /** �I����ʗp�̉��ID�ł� */
    public static final int SELECTION_SCREEN = 110;
    
    /** �f�B���N�g���I�����ID�ł� */
    public static final int DIR_SCREEN = 120;
    
    /** �f�[�^���͗p�̉��ID�ł� */
    public static final int DATAINPUT_SCREEN = 200;
    
    private fwSampleSceneDB sceneDb = null;
    private fwSampleStorage dataStorage = null;
    
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
        dataStorage = new fwSampleStorage(new MidpRecordAccessor());
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
        MidpCanvas myCanvas = new MidpCanvas(selector, Font.SIZE_MEDIUM);
        myCanvas.prepare();

        // �x�[�X�N���X��ݒ肷��
        sceneDb.setScreenObjects(myCanvas);

        // ��ʃT�C�Y�̎擾
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        fwSampleMainStorage mainStorage = new fwSampleMainStorage(myCanvas, dataStorage, new MidpFileMediaUtils());

        // ���C����ʂ̓o�^ (new���āA�t�H���g��ݒ肵�āA��ʃT�C�Y��ݒ肷��)
        IMidpScreenCanvas screenObject = null;
        screenObject = new fwSampleMainCanvas(DEFAULT_SCREEN, selector, mainStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // �f�B���N�g���ꗗ��ʂ̓o�^
        fwSampleFileSelectionStorage fileSelection = new fwSampleFileSelectionStorage(dataStorage, selector);
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(fileSelection, fileSelection, (new MidpFileDirList()), false);
        screenObject = new DirectorySelectionDialog(DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // �I����ʂ̓o�^
        screenObject = new SelectionScreen(SELECTION_SCREEN, selector, new SelectionScreenOperator(mainStorage));
        screenObject.prepare(myCanvas.getFont());
        int width  = 140;
        int height = myCanvas.getFont().getHeight() * 6;
        screenObject.setRegion((baseWidth - width), (baseHeight - height), width, height);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // �f�[�^���̓t�H�[���̓o�^
        IMidpForm inputForm = new MidpDataInputForm(null, DATAINPUT_SCREEN, selector,  mainStorage);
        inputForm.setOkButton(false);
        inputForm.setCancelButton(false);
        sceneDb.setForm(inputForm);

        System.gc();

        return;
    }
}
