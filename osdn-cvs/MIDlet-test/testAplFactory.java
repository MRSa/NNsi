import javax.microedition.lcdui.Canvas;

public class testAplFactory
{
	private testAplCanvas      screen0 = null;
	private testAplFileDirList fileList = null;
	
    /**
     *  �R���X�g���N�^
     *
     */
	public testAplFactory()
	{
		// �������Ȃ�...
	}

	/**
	 * �I�u�W�F�N�g�̐����Ə���
	 * 
	 * @return  �I�u�W�F�N�g��������/�������s
	 */
	public boolean prepare(testAplSceneSelector parent)
	{
        screen0 = new testAplCanvas(parent);
        screen0.prepare();

        fileList = new testAplFileDirList();

        return (true);
	}

	/**
	 * ��ʕ\���̏���
	 *
	 */
	public void prepareScreen()
	{
		//
		screen0.prepareScreen();
		return;
	}

	/**
	 * �Ǘ��I�u�W�F�N�g�̍폜
	 *
	 */
	public void cleanup()
	{
        //
		return;
	}
	
	/**
	 *  ��ʂ̐؂�ւ�
	 * 
	 * @param scene
	 * @param message
	 */
	public void changeScene(int scene, String message)
	{
		// 
		return;
	}

	/**
	 * �\�������ʂ���������
	 * 
	 * @return
	 */
	public Canvas getCanvas()
	{
		return (screen0);
	}
	
	/**
	 * �t�@�C�����X�g�I�u�W�F�N�g����������
	 * 
	 * @return
	 */
	public testAplFileDirList getFileList()
	{
		return (fileList);
	}	
}
