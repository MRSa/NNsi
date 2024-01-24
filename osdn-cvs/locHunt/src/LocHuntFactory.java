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
public class LocHuntFactory implements IMidpFactory
{
    private LocHuntSceneDB      sceneDb = null;
    private LocHuntDataStorage  dataStorage = null;
    private CameraDeviceController   cameraDevice = null;
    private LocationDeviceController locationDevice = null;
    
    /**
     *  �R���X�g���N�^
     *
     */
    public LocHuntFactory()
    {
        //
    }

    /**
     *  �I�u�W�F�N�g�̐���
     * 
     */
    public void prepareCoreObjects()
    {
        // �J�����E�ʒu���f�o�C�X���I�[�v������
        cameraDevice   = new CameraDeviceController();
        locationDevice = new LocationDeviceController();

        // �V�[���ؑփf�[�^�N���X�̐���
        sceneDb = new LocHuntSceneDB();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new LocHuntDataStorage(new MidpRecordAccessor(), locationDevice);
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
        int baseHeight = 250;
        try
        {
            baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
            baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();
        }
        catch (Exception ex)
        {
            //
        }

        // �J�����f�o�C�X�̏���
        cameraDevice.prepare();
        
        // ���(�x�[�X)�N���X : �L�����o�X
        MidpCanvas myCanvas = new MidpCanvas(selector, dataStorage.getFontSize());
        myCanvas.prepare(baseWidth, baseHeight);

        // �x�[�X�N���X��ݒ肷��
        sceneDb.setScreenObjects(myCanvas);

        // ���[�e�B���e�B�N���X�̐���
        MidpFileHttpCommunication httpComm = new MidpFileHttpCommunication();
        LocHuntSceneStorage sceneStorage  = new LocHuntSceneStorage(dataStorage, new MidpResourceFileUtils(object), new MidpFileOperations(), new MidpKanjiConverter(), httpComm, myCanvas);

        // �t�@�C���f�B���N�g�������N���X
        MidpFileDirList dirList = new MidpFileDirList();

        // ��ʃN���X : �t�H�[��
        MidpDataInputForm myForm = new MidpDataInputForm("�J�����p�����[�^�ݒ�", LocHuntSceneDB.INPUT_SCREEN, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // ��ʃN���X�̓o�^
        IMidpScreenCanvas screenObject = null;        

        // ���\���p��ʂ̓o�^
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);
        screenObject = new InformationScreen(LocHuntSceneDB.WELCOME_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("LocHunt�̐���");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �r�W�[�\����ʂ̓o�^
        informationOperator = null;
        BusyConfirmationStorage busyStorage = new BusyConfirmationStorage();
        informationOperator = new InformationScreenOperator(busyStorage);
        screenObject = new InformationScreen(LocHuntSceneDB.BUSY_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        busyStorage.prepare(screenObject);
        screenObject.setTitle("");
        screenObject.setRegion(20, 20, baseWidth - 40, baseHeight - 40);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �f�B���N�g���ꗗ��ʂ̓o�^
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList, false);
        screenObject = new DirectorySelectionDialog(LocHuntSceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �v���r���[��ʂ̓o�^
        PreviewScreenOperator previewOperator = new PreviewScreenOperator(sceneStorage, cameraDevice);
        screenObject = new PreviewScreen(LocHuntSceneDB.PREVIEW_SCREEN, selector, previewOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(4, 15, baseWidth - 8, baseHeight - 17);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // ���C����ʂ̓o�^
        screenObject = new LocHuntMainScreen(LocHuntSceneDB.DEFAULT_SCREEN, selector, sceneStorage, cameraDevice);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
