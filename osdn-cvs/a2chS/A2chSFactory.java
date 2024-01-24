import javax.microedition.lcdui.Display;

import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.forms.MidpDataInputForm;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpFactory;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialog;
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreen;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialogOperator;
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreenOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreen;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileHttpCommunication;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpKanjiConverter;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import javax.microedition.midlet.MIDlet;

/**
 *  �t�@�N�g���N���X
 *  
 *  @author MRSa
 *
 */
public class A2chSFactory implements IMidpFactory
{
    private A2chSSceneDB      sceneDb = null;
    private A2chSDataStorage  dataStorage = null;
    
    /**
     *  �R���X�g���N�^
     *
     */
    public A2chSFactory()
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
        sceneDb = new A2chSSceneDB();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new A2chSDataStorage(new MidpRecordAccessor());
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
        MidpFileHttpCommunication httpComm = new MidpFileHttpCommunication();
        A2chSSceneStorage sceneStorage  = new A2chSSceneStorage(dataStorage, new MidpResourceFileUtils(object), new MidpFileOperations(), new MidpKanjiConverter(), httpComm, myCanvas);

        // �t�@�C���f�B���N�g�������N���X
        MidpFileDirList dirList = new MidpFileDirList();

        // ��ʃN���X : �t�H�[��
        MidpDataInputForm myForm = new MidpDataInputForm("����������", A2chSSceneDB.INPUT_SCREEN, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // ��ʃN���X�̓o�^
        IMidpScreenCanvas screenObject = null;        

        // ���\���p��ʂ̓o�^
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);
        screenObject = new InformationScreen(A2chSSceneDB.WELCOME_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("a2ch�����̐���");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �r�W�[�\����ʂ̓o�^
        informationOperator = null;
        BusyConfirmationStorage busyStorage = new BusyConfirmationStorage();
        informationOperator = new InformationScreenOperator(busyStorage);
        screenObject = new InformationScreen(A2chSSceneDB.BUSY_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        busyStorage.prepare(screenObject);
        screenObject.setTitle("");
        screenObject.setRegion(20, 20, baseWidth - 40, baseHeight - 40);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �f�B���N�g���ꗗ��ʂ̓o�^
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList, false);
        screenObject = new DirectorySelectionDialog(A2chSSceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();
        
        // ���C����ʂ̓o�^
        A2chSListStorage listStorage = new A2chSListStorage(sceneStorage, busyStorage);
        screenObject = new A2chSMainScreen(A2chSSceneDB.DEFAULT_SCREEN, selector, sceneStorage, listStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        // �X���ꗗ��ʂ̓o�^
        SelectionScreenOperator selectionOperator = new SelectionScreenOperator(listStorage);
        screenObject = new SelectionScreen(A2chSSceneDB.LIST_SCREEN, selector, selectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        httpComm = null;
        listStorage = null;
        System.gc();

        return;
    }
}
