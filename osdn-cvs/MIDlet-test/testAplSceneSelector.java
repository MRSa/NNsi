import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;

public class testAplSceneSelector
{
    private final int  SCENE_DEFAULT  = -1;

	private MIDlet          parent  = null;
	private testAplFactory  factory = null;

	private int    currentScene     = SCENE_DEFAULT;   // ���݂̉��
    private String  currentMessage  = null;  

    /**
	 * �R���X�g���N�^
	 * @param parent
	 */
	testAplSceneSelector(MIDlet object)
	{
		parent  = object;
		factory = new testAplFactory();
	}

	/**
	 * �N������
	 * 
	 * @return
	 */
	public boolean initialize()
	{
		// �������̏���...
		factory.prepare(this);
		
		// ��ʕ\���̏���...
		factory.prepareScreen();

		System.gc();

		return (true);
	}
	
	/**
	 *  ������ʂɃV�[����؂�ւ���
	 *
	 */
	public void start()
	{
		changeScene(SCENE_DEFAULT, "");

		return;
	}

	/**
	 *  ������ʂɃV�[����؂�ւ���
	 *
	 */
	public void stop()
	{
		factory.cleanup();
		return;
	}

	/**
	 *  ��ʕ\���ؑփX���b�h���s
	 *
	 */
	private void changeScene(int scene, String message)
	{
		currentScene   = scene;
		currentMessage = message;
		Thread thread = new Thread()
		{
			public void run()
			{
				// ��ʕ\����؂�ւ���
				factory.changeScene(currentScene, currentMessage);
				
            	// ���̓t�H�[���ɐ؂�ւ���...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());                	
			}
		};
		thread.start();
		return;
	}

	/**
	 * �t�@�C�����X�g�擾�p�I�u�W�F�N�g�̎擾�B�B
	 * 
	 * @return
	 */
    public testAplFileDirList getDirList()
    {
    	return (factory.getFileList());
    }
}
