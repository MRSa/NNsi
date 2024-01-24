import javax.microedition.lcdui.Display;

import jp.sourceforge.nnsi.a2b.framework.core.MidpCanvas;
import jp.sourceforge.nnsi.a2b.framework.core.MidpDataInputForm;
import jp.sourceforge.nnsi.a2b.framework.core.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpFactory;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.DirectorySelectionDialog;
import jp.sourceforge.nnsi.a2b.screens.InformationScreen;
import jp.sourceforge.nnsi.a2b.screens.operator.DirectorySelectionDialogOperator;
import jp.sourceforge.nnsi.a2b.screens.operator.InformationScreenOperator;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
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
public class HiMemoFactory implements IMidpFactory
{
    private HiMemoSceneDB      sceneDb = null;
    private HiMemoDataStorage  dataStorage = null;
    
    /**
     *  �R���X�g���N�^
     *
     */
    public HiMemoFactory()
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
        sceneDb = new HiMemoSceneDB();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new HiMemoDataStorage(new MidpRecordAccessor());
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
        // ���[�e�B���e�B�N���X�̐���
        HiMemoSceneStorage sceneStorage  = new HiMemoSceneStorage(dataStorage, new MidpResourceFileUtils(object), new MidpFileOperations());

        // �t�@�C���f�B���N�g�������N���X
        MidpFileDirList dirList = new MidpFileDirList();

        // ��ʃT�C�Y�̎擾
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        // ���(�x�[�X)�N���X : �L�����o�X
        MidpCanvas myCanvas = new MidpCanvas(selector, dataStorage.getFontSize());
        myCanvas.prepare();

        // �x�[�X�N���X��ݒ肷��
        sceneDb.setScreenObjects(myCanvas);
        
        // ��ʃN���X : �t�H�[��
        MidpDataInputForm myForm   = new MidpDataInputForm("�L�^�f�B���N�g���I��", HiMemoSceneDB.INPUT_SCREEN, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // ��ʃN���X�̓o�^
        IMidpScreenCanvas screenObject = null;        

        // ���\���p��ʂ̓o�^
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);
        screenObject = new InformationScreen(HiMemoSceneDB.WELCOME_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("�ǋL�����̐���");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // �f�B���N�g���ꗗ��ʂ̓o�^
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList);
        screenObject = new DirectorySelectionDialog(HiMemoSceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);        
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();
        
        // ���C����ʂ̓o�^
        screenObject = new HiMemoMainScreen(HiMemoSceneDB.DEFAULT_SCREEN, selector, sceneStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        return;
    }
}
