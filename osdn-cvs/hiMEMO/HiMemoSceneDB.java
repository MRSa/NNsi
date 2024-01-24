import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.framework.core.*;
import jp.sourceforge.nnsi.a2b.framework.interfaces.*;

/**
 *  ��ʐؑւ̐���w�����s���B
 * 
 * @author MRSa
 */
public class HiMemoSceneDB implements IMidpSceneDB
{
    static public  final int INPUT_SCREEN    = 300; // �f�[�^���͉��
    static public  final int DEFAULT_SCREEN  = 100; // �������
    static public  final int WELCOME_SCREEN  = 101; // �摜�\�����
    static public  final int DIR_SCREEN      = 102; // �f�B���N�g���ꗗ���
   
    private boolean          isShowForm     = false; // �f�[�^���͒�(�f�[�^���͉�ʕ\����)
    private IMidpForm         inputForm     = null;  // �f�[�^���͉��
    private MidpCanvas      baseMidpCanvas  = null;  // �x�[�X�L�����o�X�N���X
    private IMidpScreenCanvas defaultScreen = null;  // �������
    private IMidpScreenCanvas welcomeScreen = null;  // �u�悤�����v���
    private IMidpScreenCanvas dirScreen     = null;  // �f�B���N�g���I�����

    /**
     *  �R���X�g���N�^�ł͂Ȃɂ����Ȃ�
     */
    public HiMemoSceneDB()
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
            // �f�B���N�g���I�����
            dirScreen = screen;
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
        inputForm = form;
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
        boolean previousShowForm = isShowForm;
        IMidpScreenCanvas screen = null;
        isShowForm = false;
        switch (currentScene)
        {
          case INPUT_SCREEN:
            isShowForm = true;
            break;

          case WELCOME_SCREEN:
              screen = welcomeScreen;
            break;

            case DIR_SCREEN:
              screen = dirScreen;
            break;

            case DEFAULT_SCREEN:
            default:
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
              inputForm.setTitleString(newTitle);
        }

        // �L�����o�X�N���X �� �L�����o�X�N���X�̐ؑ֎��́A
        // setCurrent()�����s���Ȃ�
        if ((previousScene != NOSCREEN)&&(isShowForm == previousShowForm))
        {
            return (false);
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
            inputForm.start();
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
        if (scene == INPUT_SCREEN)
        {
            return ((Displayable) inputForm);
        }
        return (baseMidpCanvas);
    }
}
