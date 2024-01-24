import javax.microedition.lcdui.Display;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.forms.MidpDataInputForm;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpFactory;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialog;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialogOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreen;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import javax.microedition.midlet.MIDlet;

/**
 *  �t�@�N�g���N���X
 *  
 *  @author MRSa
 *
 */
public class A2BUFactory implements IMidpFactory
{
    private A2BUSceneDB      sceneDb = null;
    private A2BUDataStorage  dataStorage = null;
    
    /**
     *  �R���X�g���N�^
     *
     */
    public A2BUFactory()
    {
        //
    }

    /**
     *  �I�u�W�F�N�g�̐���
     * 
     */
    public void prepareCoreObjects()
    {
        // �V�[���ؑփf�[�^�N���X�̐���
        sceneDb = new A2BUSceneDB();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new A2BUDataStorage(new MidpRecordAccessor());
    }

    /**
     *  �V�[���ؑփf�[�^�N���X����������
     * 
     *  @return �V�[���ؑփf�[�^�N���X
     */
    public IMidpSceneDB getSceneDB()
    {
        return (sceneDb);
    }

    /**
     *  �f�[�^�X�g���[�W�N���X����������
     * 
     *  @return �f�[�^�X�g���[�W�N���X
     */
    public IMidpDataStorage getDataStorage()
    {
        return (dataStorage);
    }

    /**
     *  �I�u�W�F�N�g�𐶐�����
     * 
     *  @param object MIDlet�x�[�X�N���X
     *  @param select �V�[���Z���N�^
     */
    public void prepareObjects(MIDlet object, MidpSceneSelector selector)
    {
        // ���(�x�[�X)�N���X : �L�����o�X
        MidpCanvas myCanvas = new MidpCanvas(selector, dataStorage.getFontSize());
        myCanvas.prepare();

        // �x�[�X�N���X��ݒ肷��
        sceneDb.setScreenObjects(myCanvas);

        // �t�@�C���f�B���N�g�������N���X
        MidpFileDirList dirList = new MidpFileDirList();

        // ���[�e�B���e�B�N���X�̐���
        A2BUSceneStorage sceneStorage  = new A2BUSceneStorage(dataStorage, new MidpResourceFileUtils(object), myCanvas, dirList);

        // ��ʃT�C�Y�̎擾
        int baseWidth  = 240;
        int baseHeight = 280;
        try
        {
            baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
            baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();
        }
        catch (Exception ex)
        {
        	//
        }
        
        // ��ʃN���X : �t�H�[��
        MidpDataInputForm myForm = new MidpDataInputForm("a2BU�ݒ�", A2BUSceneDB.EDITPREF_FORM, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // ��ʃN���X�̓o�^
        IMidpScreenCanvas screenObject = null;        

        // ���\���p��ʂ̓o�^
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);
        screenObject = new InformationScreen(A2BUSceneDB.WELCOME_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("a2BU�̐���");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �f�B���N�g���ꗗ��ʂ̓o�^
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList, false);
        screenObject = new DirectorySelectionDialog(A2BUSceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �r�W�[�\����ʂ̓o�^
        informationOperator = null;
        BusyConfirmationStorage busyStorage = new BusyConfirmationStorage();
        informationOperator = new InformationScreenOperator(busyStorage);
        screenObject = new InformationScreen(A2BUSceneDB.CONFIRMATION, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        busyStorage.prepare(screenObject);
        screenObject.setTitle("");
        screenObject.setRegion(25, 25, baseWidth - 50, baseHeight - 50);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // ���C����ʂ̓o�^
        screenObject = new A2BUMainScreen(A2BUSceneDB.DEFAULT_SCREEN, selector, dataStorage, busyStorage, sceneStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        // ���C����ʂ̓o�^
        screenObject = new A2BUProcessScreen(A2BUSceneDB.PROCESS_SCREEN, selector, busyStorage, sceneStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("�V���m�F���s��...");
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
