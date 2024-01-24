import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.frameworks.*;

/**
 *  ��ʐؑւ̐���w�����s���B
 * 
 * @author MRSa
 */
public class DotESceneDB implements IMidpSceneDB
{
    static public final int DEFAULT_SCREEN    = 100;  // ���C�����
    static public final int MAIN_MENU         = 200;  // ���C�����j���[
    static public final int FILENAME_FORM     = 300;  // �t�@�C�����ݒ���
    static public final int CONFIRMATION      = 400;  // �m�F���
    static public final int CONFIRMATION_LOAD = 402;  // LOAD
    static public final int CONFIRMATION_SAVE = 404;  // SAVE
    static public final int CONFIRMATION_END  = 406;  // END
    static public final int COLORPICK_SCREEN  = 500;  // �F�I�����
    static public final int COLORPICK_PIXEL   = 502;  // ������X�|�C�g(�F���E��)
    static public final int REGION_COPY       = 504;  // �̈�I��
    static public final int REGION_PASTE      = 506;  // �̈�\��t��
    static public final int REGION_CLEAR      = 508;  // �̈�N���A
    static public final int DIR_SCREEN        = 600;  // �f�B���N�g���I�����

    private boolean          isShowForm        = false; // �f�[�^���͒�(�f�[�^���͉�ʕ\����)
    private IMidpForm         preferenceForm    = null;  // �ݒ荀�ډ��
    private MidpCanvas        baseMidpCanvas    = null;  // �x�[�X�L�����o�X�N���X
    
    private IMidpScreenCanvas defaultScreen     = null;  // �������
    private IMidpScreenCanvas mainMenu          = null;  // ���C�����j���[
    private IMidpScreenCanvas confirmation      = null;  // �m�F���
    private IMidpScreenCanvas colorPickScreen   = null;  // �F�I�����
    private IMidpScreenCanvas dirScreen         = null;  // �f�B���N�g���I�����

    /**
     *  �R���X�g���N�^�ł͂Ȃɂ����Ȃ�
     */
    public DotESceneDB()
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
          case DIR_SCREEN:
            // Dir�I�����
            dirScreen = screen;
            break;

          case MAIN_MENU:
            // ���C�����j���[
            mainMenu = screen;
            break;

          case CONFIRMATION:
            // �m�F���(LOAD)
            confirmation = screen;
            break;

          case COLORPICK_SCREEN:
        	// �F�I�����
            colorPickScreen = screen;
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
          case DIR_SCREEN:
            // Dir�I�����
            screen = dirScreen;
            break;

          case MAIN_MENU:
            // ���C�����j���[
            screen = mainMenu;
            break;

          case CONFIRMATION:
            // �m�F���
            screen = confirmation;
            break;

          case FILENAME_FORM:
            isShowForm = true;
            break;

          case COLORPICK_SCREEN:
        	screen = colorPickScreen;
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
        if (scene == FILENAME_FORM)
        {
            return ((Displayable) preferenceForm);
        }
        return (baseMidpCanvas);
    }
}
