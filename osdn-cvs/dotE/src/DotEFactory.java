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
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreen;
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreenOperator;
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
public class DotEFactory implements IMidpFactory
{
    private DotESceneDB      sceneDb = null;
    private DotEDataStorage  dataStorage = null;
    
    /**
     *  �R���X�g���N�^
     *
     */
    public DotEFactory()
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
        sceneDb = new DotESceneDB();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new DotEDataStorage(new MidpRecordAccessor());
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
    	int baseHeight = 276;
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

        // �t�@�C���f�B���N�g�������N���X
        MidpFileDirList dirList = new MidpFileDirList();
        dirList.prepare(dataStorage.getBaseDirectory());

        // ���[�e�B���e�B�N���X�̐���
        DotESceneStorage sceneStorage  = new DotESceneStorage(dataStorage, new MidpResourceFileUtils(object), myCanvas, dirList);
        
        // ��ʃN���X : �t�H�[��
        MidpDataInputForm myForm = new MidpDataInputForm("�t�@�C�����w��", DotESceneDB.FILENAME_FORM, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // ��ʃN���X�̓o�^
        IMidpScreenCanvas screenObject = null;        
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);

        // �f�B���N�g���ꗗ��ʂ̓o�^
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList, false);
        fileSelectionOperator.prepare();
        screenObject = new DirectorySelectionDialog(DotESceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        
        // �r�W�[�\����ʂ̓o�^
        informationOperator = null;
        BusyConfirmationStorage confirmStorage = new BusyConfirmationStorage();
        informationOperator = new InformationScreenOperator(confirmStorage);
        screenObject = new InformationScreen(DotESceneDB.CONFIRMATION, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        confirmStorage.prepare(screenObject);
        screenObject.setTitle(" �m�F ");
        screenObject.setRegion(25, 25, baseWidth - 50, 45);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // ���j���[��ʂ̓o�^(�I����ʂ̓o�^)
        DotESelectionMenu selectionMenu = new DotESelectionMenu(confirmStorage);
        screenObject = new SelectionScreen(DotESceneDB.MAIN_MENU, selector, new SelectionScreenOperator(selectionMenu));
        screenObject.prepare(myCanvas.getFont());
        int width  = 84;
        int height = myCanvas.getFont().getHeight() * 9;
        screenObject.setRegion((baseWidth - width), (baseHeight - height), width, height);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // ���C����ʂ̓o�^
        screenObject = new DotEMainScreen(DotESceneDB.DEFAULT_SCREEN, selector, dataStorage, confirmStorage, selectionMenu);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

/*
        // �F�I����ʂ̓o�^ (�����v��ł́A�J���[�s�b�J�[��ʂ�p�ӂ�����肾�������A��߂��j
        screenObject = new DotEColorPickerScreen(DotESceneDB.COLORPICK_SCREEN, selector, dataStorage, sceneStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("�J���[�I��");
        screenObject.setRegion(10, 10, baseWidth - 20, baseHeight - 20);
        sceneDb.setCanvas(screenObject);
        System.gc();
*/
        return;
    }
}
