import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.frameworks.*;

/**
 *  ��ʐؑւ̐���w�����s���B
 * 
 * @author MRSa
 */
public class A2BUSceneDB implements IMidpSceneDB
{
    static public final int DEFAULT_SCREEN    = 100;  // ���C�����
    static public final int WELCOME_SCREEN    = 101;  // �悤�������
    static public final int DIR_SCREEN        = 102;  // �f�B���N�g���I�����
    static public final int PROCESS_SCREEN    = 110;  // ���s��������
    static public final int MAIN_MENU         = 201;  // ���C�����j���[
    static public final int PROCESS_MENU      = 202;  // ���s�����j���[
    static public final int EDITPREF_FORM     = 300;  // ���ڐݒ���
    static public final int CONFIRMATION      = 400;  // �m�F���

    private boolean          isShowForm        = false; // �f�[�^���͒�(�f�[�^���͉�ʕ\����)
    private IMidpForm         preferenceForm    = null;  // �ݒ荀�ډ��
    private MidpCanvas        baseMidpCanvas    = null;  // �x�[�X�L�����o�X�N���X
    
    private IMidpScreenCanvas defaultScreen = null;  // �������
    private IMidpScreenCanvas welcomeScreen = null;  // �u�悤�����v���
    private IMidpScreenCanvas dirScreen     = null;  // �f�B���N�g���I�����
    private IMidpScreenCanvas processScreen = null;  // ���s��������
    private IMidpScreenCanvas mainMenu      = null;  // ���C�����j���[
    private IMidpScreenCanvas processMenu   = null;  // ���s�����j���[
    private IMidpScreenCanvas confirmation  = null;  // �m�F���    

    /**
     *  �R���X�g���N�^�ł͂Ȃɂ����Ȃ�
     */
    public A2BUSceneDB()
    {
        //
    }

    /**
     *  �L�����o�X�`��N���X��ݒ肷��
     *  @param screen �L�����o�X�`��N���X
     */
    public void setCanvas(IMidpScreenCanvas screen)
    {
        switch (screen.getCanvasId())
        {
          case WELCOME_SCREEN:
            // �悤�������
            welcomeScreen = screen;
            break;

          case DIR_SCREEN:
            // Dir�I�����
            dirScreen = screen;
            break;

          case PROCESS_SCREEN:
            // �V���m�F���
            processScreen = screen;
            break;

          case MAIN_MENU:
            // ���C�����j���[
            mainMenu = screen;
            break;

          case PROCESS_MENU:
            // ���s�����j���[
            processMenu = screen;
            break;

          case CONFIRMATION:
            // �m�F���
              confirmation = screen;
            break;
 
          case DEFAULT_SCREEN:
          default:
            // �������
            defaultScreen = screen;
            break;
        }
        return;
    }

    /**
     *  �f�[�^���͗p��ʂ�ݒ肷��
     *  @param �f�[�^���͗p���
     *  
     */
    public void setForm(IMidpForm form)
    {
        preferenceForm = form;
        return;
    }

    
    /**
     *  �N���X�̎��s����...�������Ȃ�
     *
     */
    public void prepare()
    {
        //
    }

    /**
     *  �x�[�X�L�����o�X�N���X���L������
     *  
     *  @param baseCanvas �x�[�X�L�����o�X�N���X
     */
    public void setScreenObjects(MidpCanvas baseCanvas)
    {
        baseMidpCanvas  = baseCanvas;
    }

    /**
     *  �f�t�H���g��ʂ�ID������...
     *  @return �f�t�H���g�̉��ID (DEFAULT_SCREEN)
     */
    public int getDefaultScene()
    {
        return (DEFAULT_SCREEN);
    }

    /**
     *   ��ʂ�؂�ւ���OK���`�F�b�N����<br>
     *   ��{�I�ɂ̓X���[�A�ؑւ�������
     *
     *  @param sceneToChange �؂�ւ����ʉ��
     *  @param currentScene  ���ݕ\�����̉��
     */
    public boolean isAvailableChangeScene(int sceneToChange, int currentScene)
    {
        // �O��ʂ͂Ȃ��A�A�A�؂�ւ�NG�Ƃ���
        if (sceneToChange == NOSCREEN)
        {
            return (false);
        }
        return (true);
    }

    /**
     *   ��ʂ�؂�ւ���
     *   @param currentScene  �؂�ւ�����
     *   @param previousScene �֑ؑO�̉�ʁi���ݕ\�����Ă����ʁj
     *   @param newTitle  �V������ʂ̃^�C�g��
     *   @return Display.setCurrent() ���K�v�ȏꍇ��true������
     */
    public boolean changeScene(int currentScene, int previousScene, String newTitle)
    {
        IMidpScreenCanvas screen = null;
        isShowForm = false;
        switch (currentScene)
        {
          case WELCOME_SCREEN:
            // �悤�������
            screen = welcomeScreen;
            break;

          case DIR_SCREEN:
            // Dir�I�����
            screen = dirScreen;
            break;

          case PROCESS_SCREEN:
            // �V���m�F���
            screen = processScreen;
            break;

          case MAIN_MENU:
            // ���C�����j���[
            screen = mainMenu;
            break;

          case PROCESS_MENU:
            // ���s�����j���[
            screen = processMenu;
            break;

          case CONFIRMATION:
            // �m�F���
            screen = confirmation;
            break;

          case EDITPREF_FORM:
            isShowForm = true;
            break;

          case DEFAULT_SCREEN:
          default:
            // �������
            screen = defaultScreen;
            break;  
        }
        if (isShowForm == false)
        {
            // �L�����o�X�N���X�֐؂�ւ���
            baseMidpCanvas.changeActiveCanvas(screen);
        }
        else
        {
            // �f�[�^���͉�ʂ֐؂�ւ���
            preferenceForm.setTitleString(newTitle);
        }
        return (true);
    }

    /**
     *   ��ʂ�؂�ւ�����̏���
     *   
     *   @param currentScene  �؂�ւ������
     *   @param previousScene �֑ؑO�̉��
     */
    public void sceneChanged(int currentScene, int previousScene)
    {
        if (isShowForm == true)
        {
            preferenceForm.start();
        }
        return;
    }

    /**
     *   ��ʏ��(������)��\������
     *  @param scene ���ݕ\�����̉��ID
     *  @return ��ʏ��(������)
     */
    public String getInfoMessage(int scene)
    {
        return ("");
    }

    /**
     *   ��ʃN���X����������
     *   @param scene ���ݕ\�����̉��ID
     *   @return ��ʃN���X
     */
    public Displayable getScreen(int scene)
    {
        if (scene == EDITPREF_FORM)
        {
            return ((Displayable) preferenceForm);
        }
        return (baseMidpCanvas);
    }
}
