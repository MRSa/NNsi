import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpForm;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;

/**
 *  �V�[���ؑփf�[�^�x�[�X
 * 
 * @author MRSa
 *
 */
public class TVgSceneDb implements IMidpSceneDB
{
    static public final int SCREEN_INFORMATION = 100; // ���\�����
    static public final int SCREEN_DIRSELECT   = 200; // �f�B���N�g���I�����
    static public final int SCREEN_MENUSELECT  = 300; // ���j���[�\�����
    static public final int SCREEN_SELECTION   = 400; // �f�[�^�I�����
    static public final int SCREEN_PROGRAMLIST = 500; // �ԑg�ꗗ�\�����
    static public final int SCREEN_DATELIST    = 600; // ���t�ꗗ�\�����

    static public final int SCENE_WELCOME          = 110; // �u�悤�����v�\��
    static public final int SCENE_BUSY             = 120; // �u�r�W�[�v�\��
    static public final int SCENE_DIRSELECTION     = 210; // �f�B���N�g���I��
    static public final int SCENE_MENUSELECTION    = 310; // ���j���[�I��
    static public final int SCENE_STATIONSELECTION = 410; // �e���r�ǑI��
    static public final int SCENE_TVTYPESELECTION  = 420; // �d�g��ʑI��
    static public final int SCENE_LOCALESELECTION  = 430; // �n��I��
    static public final int SCENE_PROGRAMLIST      = 510; // �ԑg���X�g
    static public final int SCENE_DATELIST         = 610; // ���t���X�g   
    
    protected MidpCanvas baseScreen = null;

    private String infoTitle = null;

    private TVgSelectionStorage selectionStorage = null;

    private IMidpScreenCanvas infoScreen    = null;  // ���\��
    private IMidpScreenCanvas dirScreen     = null;  // �f�B���N�g���I��\��
    private IMidpScreenCanvas menuScreen    = null;  // ���j���[�\��
    private IMidpScreenCanvas selectScreen  = null;  // �f�[�^�I��\��
    private IMidpScreenCanvas programScreen = null;  // �ԑg�ꗗ�\��
    private IMidpScreenCanvas dateScreen    = null;  // ���t�ꗗ�\��

    /**
     *  �R���X�g���N�^�ł͉������삵�܂���
     *
     */
    public TVgSceneDb()
    {
        
    }

    /**
     *  �N���X�̏���
     *  
     */
    public void prepare()
    {
        return;
    }

    /**
     *  �I���X�g���[�W���L������
     * @param storage
     */
    public void setSelectionStorage(TVgSelectionStorage storage)
    {
        selectionStorage = storage;
    }
    
    /**
     *  �X�N���[���I�u�W�F�N�g�̐ݒ�
     *  @param baseCanvas �x�[�X�L�����o�X�N���X
     */
    public void setScreenObjects(MidpCanvas baseCanvas)
    {
        baseScreen = baseCanvas;
        return;
    }

    /**
     *  �f�t�H���g�̉�ʂ���������
     *  @return �f�t�H���g�̉��ID
     */
    public int getDefaultScene()
    {
        return (SCENE_DATELIST);
    }

    /**
     *  �X�N���[���\����ʃN���X��ݒ肷��
     *  @param screen �X�N���[���\����ʃN���X
     */
    public void setCanvas(IMidpScreenCanvas screen)
    {
        int canvasId = screen.getCanvasId();
        switch (canvasId)
        {
          case SCREEN_INFORMATION: // ���\�����
              infoScreen = screen;
            break;

          case SCREEN_DIRSELECT: // �f�B���N�g���I�����
              dirScreen = screen;
            break;

          case SCREEN_MENUSELECT: // ���j���[�\�����
              menuScreen = screen;
            break;

          case SCREEN_SELECTION: // �f�[�^�I�����
              selectScreen = screen;
            break;

          case SCREEN_PROGRAMLIST: // �ԑg�ꗗ�\�����
              programScreen = screen;
            break;

          case SCREEN_DATELIST: // ���t�ꗗ�\�����
              dateScreen = screen;
            break;

          default:
            break;
        }
        return;
    }

    /**
     *  �f�[�^���͗p�N���X��ݒ肷��
     *  @param form �f�[�^���̓t�H�[��
     */
    public void setForm(IMidpForm form)
    {
        return;
    }

    /**
     *  ��ʐ؂�ւ��̉۔��f
     *  @param next �؂�ւ��������ID
     *  @param current ���݂̉��ID
     *  @return true:�ؑ�OK, false:�ؑ�NG
     */
    public boolean isAvailableChangeScene(int next, int current)
    {
        return (true);
    }

    /**
     *  ��ʂ̐؂�ւ����s����(�����ŉ�ʂ�؂�ւ��܂�)<br>
     *  (baseMidpCanvas.changeActiveCanvas() ���Ă񂾂�A�Ƃ����s���܂��B)<br>
     *  ��ʂ�؂�ւ���ꍇ�ɂ́A�V�[���Z���N�^�� getScreen()���Ăяo���A
     *  Displayable�N���X(��ʕ\���N���X)���擾���܂��B
     *  
     *  @param next �؂�ւ��������ID
     *  @param current ���݂̉��ID
     *  @param title �V������ʂ̃^�C�g��
     *  @return <code>true</code>:Display.setCurrent()�����s���܂��B<br>
     *   <code>false</code>:Display.setCurrent()�����s���܂���B
     *   (MidpScreenCanvas()�N���X��ؑւ�Ƃ��A�Ƃ��͂�������������Ă��������B)
     */
    public boolean changeScene(int next, int current, String title)
    {
        IMidpScreenCanvas screen = null;
        int nextCanvasId = getNextCanvasId(next);
        switch (nextCanvasId)
        {
          case SCREEN_INFORMATION: // ���\�����
              screen = infoScreen;
            break;

          case SCREEN_DIRSELECT: // �f�B���N�g���I�����
              screen = dirScreen;
            break;

          case SCREEN_MENUSELECT: // ���j���[�\�����
              screen = menuScreen;
            break;

          case SCREEN_SELECTION: // �f�[�^�I�����
              screen = selectScreen;
            break;

          case SCREEN_PROGRAMLIST: // �ԑg�ꗗ�\�����
              screen = programScreen;
            break;

          case SCREEN_DATELIST: // ���t�ꗗ�\�����
              screen = dateScreen;
            break;

          default:
            break;
        }

        if (current == SCENE_WELCOME)
        {
            // �悤������ʂ������ꍇ�A�f�B���N�g���I����ʂ֐؂�ւ���
            screen = dirScreen;
            next = SCENE_DIRSELECTION;
        }
          else if (current == SCENE_DIRSELECTION)
        {
            // �f�B���N�g����ʂ������ꍇ�A���t�ꗗ�\����ʂ֐؂�ւ���
            screen = dateScreen;
            next = SCENE_DATELIST;
        }

        infoTitle = title;

        if (screen != null)
        {
            // �V�[���ݒ��؂�ւ���
            selectionStorage.setSceneMode(next);
            
            // ��ʃ^�C�g����ݒ肷��
            screen.setTitle(title);
            
            // �L�����o�X�N���X�֐؂�ւ���
            baseScreen.changeActiveCanvas(screen);
        }
        return (true);
    }

    /**
     *  ����ʂ̃X�N���[���N���X����
     * @param nextScene
     * @return
     */
    private int getNextCanvasId(int nextScene)
    {
        int canvasId = nextScene;
        switch (nextScene)
        {
          case SCENE_WELCOME:  // �u�悤�����v�\��
          case SCENE_BUSY:     // �u�r�W�[�v�\��
            canvasId = SCREEN_INFORMATION;
            break;

          case SCENE_DIRSELECTION:     // �f�B���N�g���I��
              canvasId = SCREEN_DIRSELECT;
            break;
          case SCENE_MENUSELECTION:    // ���j���[�I��
              canvasId = SCREEN_MENUSELECT;
            break;
          case SCENE_STATIONSELECTION: // �e���r�ǑI��
          case SCENE_TVTYPESELECTION:  // �d�g��ʑI��
          case SCENE_LOCALESELECTION:  // �n��I��
              canvasId = SCREEN_SELECTION;
            break;
          case SCENE_PROGRAMLIST:      // �ԑg���X�g
              canvasId = SCREEN_PROGRAMLIST;
            break;
          case SCENE_DATELIST:         // ���t���X�g   
              canvasId = SCREEN_DATELIST;
            break;
          default:
            // ���̑��͂��̂܂�
            break;
        }
        return (canvasId);
    }
    
    /**
     *  ��ʐ؂�ւ��������̏���
     *  @param current �ؑ֌�̉��ID
     *  @param previous �֑ؑO�̉��ID
     *  
     */
    public void sceneChanged(int current, int previous)
    {
        return;
    }

    /**
     *  ��ʂɊւ�镶����擾
     *  @param scene ���ID
     *  @return ��ʂɂ�����镶����
     */
    public String  getInfoMessage(int scene)
    {
        return (infoTitle);
    }

    /**
     *  �\���N���X�̎擾
     *  @param scene ���ID
     *  @return ��ʕ\���N���X
     */
    public Displayable getScreen(int scene)
    {
        return (baseScreen);
    }
}
