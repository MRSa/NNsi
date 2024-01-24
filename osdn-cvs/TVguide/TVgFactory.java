import javax.microedition.lcdui.Display;

import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpFactory;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialog;
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreen;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialogOperator;
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreenOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationDefaultStorage;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreen;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpDefaultA2BhttpCommunicator;
import jp.sourceforge.nnsi.a2b.utilities.MidpA2BhttpCommunication;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpDateUtils;
import javax.microedition.midlet.MIDlet;

/**
 *  �t�@�N�g���N���X
 *  
 *  @author MRSa
 *
 */
public class TVgFactory implements IMidpFactory
{
    private TVgSceneDb      sceneDb = null;
    private TVgDataStorage  dataStorage = null;
    
    /**
     *  �R���X�g���N�^
     *
     */
    public TVgFactory()
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
        sceneDb = new TVgSceneDb();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new TVgDataStorage(new MidpRecordAccessor());
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
        dataStorage.setObjects(myCanvas);
        
        // �x�[�X�N���X��ݒ肷��
        sceneDb.setScreenObjects(myCanvas);

        // ���[�e�B���e�B�N���X�̐���
        MidpDefaultA2BhttpCommunicator httpCommunicatorSet = new MidpDefaultA2BhttpCommunicator(false);
        dataStorage.setHttpCommunicator(httpCommunicatorSet);
        String cookie = "areaId=" + dataStorage.getAreaId() + "; span=24";
        httpCommunicatorSet.SetCookie(cookie);
        MidpA2BhttpCommunication httpComm = new MidpA2BhttpCommunication(httpCommunicatorSet, null);
        dataStorage.setResourceUtils(new MidpResourceFileUtils(object));

        // �t�@�C���f�B���N�g�������N���X
        MidpFileDirList dirList = new MidpFileDirList();

        // ��ʃT�C�Y�̎擾
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

/**
        // ��ʃN���X : �t�H�[�� (�����(�܂�)�g��Ȃ�)
        MidpDataInputForm myForm = new MidpDataInputForm("����������", A2chSSceneDB.INPUT_SCREEN, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);
**/
        // ��ʃN���X�̓o�^
        IMidpScreenCanvas screenObject = null;        

        // ���\���p���(Welcome���)�̓o�^
        InformationDefaultStorage infoStorage = new InformationDefaultStorage();
        dataStorage.setInformationStorage(infoStorage);
        InformationScreenOperator informationOperator = new InformationScreenOperator(infoStorage);
        screenObject = new InformationScreen(TVgSceneDb.SCREEN_INFORMATION, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("����");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // �ԑg�ꗗ�\����ʂ̓o�^
        TVgProgramDataParser dataParser = new TVgProgramDataParser(dataStorage, httpComm, myCanvas, new MidpDateUtils());
        TVgSelectionStorage selectionStorage = new TVgSelectionStorage(dataParser, dataStorage);
        sceneDb.setSelectionStorage(selectionStorage);
        screenObject = new TVgProgramScreen(TVgSceneDb.SCREEN_PROGRAMLIST, selector, dataParser);
        screenObject.prepare((myCanvas.getFont()));
        screenObject.setTitle("");
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // ���t�ꗗ�\����ʂ̓o�^
        screenObject = new TVgDateScreen(TVgSceneDb.SCREEN_DATELIST, selector, dataParser);
        screenObject.prepare((myCanvas.getFont()));
        screenObject.setTitle("");
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // �f�B���N�g���ꗗ��ʂ̓o�^
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(selectionStorage, selectionStorage, dirList, true);
        screenObject = new DirectorySelectionDialog(TVgSceneDb.SCREEN_DIRSELECT, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(20, 20, baseWidth - 40, baseHeight - 40);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // ���j���[��ʂ̓o�^
        SelectionScreenOperator sceneOperator = new SelectionScreenOperator(selectionStorage);
        screenObject = new SelectionScreen(TVgSceneDb.SCREEN_MENUSELECT, selector, sceneOperator);
        screenObject.prepare(myCanvas.getFont());
        int width  = 140;
        int height = myCanvas.getFont().getHeight() * 3;
        screenObject.setRegion((baseWidth - width), (baseHeight - height), width, height);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // �I����ʂ̓o�^
        screenObject = new SelectionScreen(TVgSceneDb.SCREEN_SELECTION, selector, sceneOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(4, 4, (baseWidth - 8), (baseHeight - 8));
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        System.gc();
        return;
    }
}
