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
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import javax.microedition.midlet.MIDlet;

/**
 *  �t�@�N�g���N���X
 *  
 *  @author MRSa
 *
 */
public class TheCountFactory implements IMidpFactory
{
    private TheCountSceneDB      sceneDb = null;
    private TheCountDataStorage  dataStorage = null;
    
    /**
     *  �R���X�g���N�^
     *
     */
    public TheCountFactory()
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
        sceneDb = new TheCountSceneDB();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new TheCountDataStorage(new MidpRecordAccessor(), new MidpFileOperations());
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

        // ���(�x�[�X)�N���X : �L�����o�X
        MidpCanvas myCanvas = new MidpCanvas(selector, dataStorage.getFontSize());
        myCanvas.prepare(baseWidth, baseHeight);

        // �x�[�X�N���X��ݒ肷��
        sceneDb.setScreenObjects(myCanvas);

        // ���[�e�B���e�B�N���X�̐���
        TheCountSceneStorage sceneStorage  = new TheCountSceneStorage(dataStorage, new MidpResourceFileUtils(object));

        // �t�@�C���f�B���N�g�������N���X
        MidpFileDirList dirList = new MidpFileDirList();

        // ��ʃN���X : �t�H�[��
        MidpDataInputForm myForm = new MidpDataInputForm("�A�C�e�����ݒ�", TheCountSceneDB.INPUT_SCREEN, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // ��ʃN���X�̓o�^
        IMidpScreenCanvas screenObject = null;        

        // ���\���p��ʂ̓o�^
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);
        screenObject = new InformationScreen(TheCountSceneDB.WELCOME_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("TheCount�̐���");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �r�W�[�\����ʂ̓o�^
        informationOperator = null;
        BusyConfirmationStorage busyStorage = new BusyConfirmationStorage();
        informationOperator = new InformationScreenOperator(busyStorage);
        screenObject = new InformationScreen(TheCountSceneDB.BUSY_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        busyStorage.prepare(screenObject);
        screenObject.setTitle("");
        screenObject.setRegion(20, 20, baseWidth - 40, baseHeight - 40);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �f�B���N�g���ꗗ��ʂ̓o�^
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList, false);
        screenObject = new DirectorySelectionDialog(TheCountSceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �O���t��ʂ̓o�^
        screenObject = new TheCountSummaryScreen(TheCountSceneDB.SUMMARY_SCREEN, selector, dataStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("�O���t�\��");
        screenObject.setRegion(2, 2, (baseWidth - 4), (baseHeight - 4));
        sceneDb.setCanvas(screenObject);
        System.gc();

        // ���C����ʂ̓o�^
        screenObject = new TheCountMainScreen(TheCountSceneDB.DEFAULT_SCREEN, selector, dataStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
