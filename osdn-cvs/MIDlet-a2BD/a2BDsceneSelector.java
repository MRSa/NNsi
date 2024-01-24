import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.AlertType;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
/**
 * �V�[���Z���N�^
 * @author MRSa
 *
 */
public class a2BDsceneSelector
{
	private a2BDmain  parent     = null;
	private busyForm  busyDialog = null;
	private Alert     alert      = null;

	
	public final int SCENE_UNKNOWN        = 0;
	public final int SCENE_OPEN_DATAINPUT = 1;
	public final int SCENE_DATAINPUT      = 2;
	public final int SCENE_EXECUTE_DATA   = 3;
	public final int SCENE_FILE_SELECT    = 4;
	public final int SCENE_SHOW_BUSY      = 900;

	private int currentScene = SCENE_UNKNOWN;
	
	/**
	 * �R���X�g���N�^
	 * @param parent
	 */
	a2BDsceneSelector(a2BDmain object, busyForm busy)
	{
		alert = new Alert("");
		parent = object;		
		busyDialog = busy;
	}

	/**
	 *  ������ʂɃV�[����؂�ւ���
	 *
	 */
	public void start()
	{
		changeScene(SCENE_OPEN_DATAINPUT);
		return;
	}

	/**
	 *  ��ʕ\���ؑփX���b�h���s
	 *
	 */
	private void changeScene(int scene)
	{
		currentScene = scene;
		Thread thread = new Thread()
		{
			public void run()
			{
				parent.changeScene(currentScene);
			}
		};
		thread.start();
		return;
	}


	/**
	 * �f�[�^���̓t�H�[���𔲂����Ƃ��̏���
	 * @param execute
	 */
	public void EndDataInputForm(boolean isCancel)
	{
		if (isCancel == true)
		{
			// �I������
			currentScene = SCENE_UNKNOWN;
			parent.quit_a2BD();
			return;
		}
		currentScene = SCENE_EXECUTE_DATA;
		parent.changeScene(currentScene);
		return;
	}
	
	/**
	 * �r�W�[���b�Z�[�W�̕\��
	 * @param title
	 * @param message
	 */
	public void showBusyMessage(String title, String message, String message2, String message3)
	{
        // 
		busyDialog.SetBusyMessage(title, message, message2, message3, false);
		return;
	}
	
	/**
	 * �r�W�[���b�Z�[�W�̉���
	 * 
	 */
	public void hideBusyMessage()
	{
		return;
	}
	
	/**
	 * HTTP�ʐM�̏I����
	 * @param scene
	 * @param responseCode
	 * @param length
	 */
	public void CompletedToGetHttpData(int scene, int responseCode, int length)
	{
		String reply;
		
		// �����R�[�h�̔���
		if (responseCode >= 200)
		{
			reply = "�T�[�o����̉����FOK";
		}
		else
		{
			reply = "�T�[�o����̉����F??";
		}
		
        // �ʐM�I����񍐂���
		busyDialog.SetBusyMessage("HTTP�ʐM�I��", reply, " (�����R�[�h :" + responseCode + ")", "��M�f�[�^�T�C�Y :" + length, true);	
	}

	/** 
	 *  HTTP�ʐM�N���X�ŏ���f�[�^���󂯎�����Ƃ��̏���...
	 * 
	 * 
	 */
	public int getHttpDataReceivedFirst(int scene, int responseCode, byte[] buffer, int length)
	{
		// ����͂����ŉ������Ȃ��B
		return (0);
	}

	/**
	 * �t�@�C������I�������Ƃ��̏���
	 * 
	 */
	public void EndFileSelector(String selectedFile)
	{
		if (selectedFile == null)
		{
			// �����L�����Z�������ꍇ...

			// ��ʂ̐؂�ւ������{
			changeScene(SCENE_DATAINPUT);
			return;
		}
		
		// �f�[�^�𔽉f������
		parent.setData2(selectedFile);
		
		
		// ��ʂ̐؂�ւ������{
		changeScene(SCENE_DATAINPUT);
		return;
	}

	/**
	 * �t�@�C�����̑I��\��������
	 * 
	 * @param fileName
	 */
	public void SelectFile(String fileName)
	{
		// ��ʂ̐؂�ւ������{
		changeScene(SCENE_FILE_SELECT);
		return;
	}

	/**
	 *  ���������\������
	 *
	 */
	public void ShowHelp()
	{
		String    message = "��a2BD�Ƃ́H\n";
		message = message + "a2BD�́AURL���w�肵�ăt�@�C�����_�E�����[�h����MIDlet�ł��B\n";
		message = message + "�{���Aa2B�̔ꗗ�t�@�C��(bbstable.html)��P�̂ōX�V���邽�߂̃\�t�g�E�F�A�ł����A";
		message = message + "Java�ɂ��http�ʐM����уt�@�C���L�^�̃e�X�g�ɂ����g�����������܂��B\n\n";
		message = message + "���g�p���@\n";
        message = message + "������\n";
        message = message + "a2BD�́A���s����O�Ɏ擾����URL�ƋL�^����t�@�C�����̎w�肪�K�v�ł��B\n";
		message = message + "URL���Ƀf�[�^���擾����URL�AFile���ɋL�^����t�@�C�������w�肵�Ă��������B\n";
        message = message + "�w�I���x�{�^���́A�����ƃ��[�J���t�@�C�����w�肷��f�B���N�g���\�����[�h�ƂȂ�܂��B\n";
        message = message + "(�����t�@�C�����X�V����Ƃ��ɂ��g�p���������B)\n";
        message = message + "�Ȃ��A�t�@�C�����̎w�肪 / �ŏI����Ă����ꍇ�́A���̃f�B���N�g����URL�̖����t�@�C�����Ń_�E�����[�h���܂��B\n";
        message = message + "\n�����s\n";
		message = message + "�w���s�x�{�^���������Ă��������B\n�t�@�C���̃_�E�����[�h���J�n���A���̌��ʂ�\�����܂��B\n";
		message = message + "\n���ʐM���ʂ̕\��\n";
		message = message + "�T�[�o����̉����Ǝ�M�f�[�^�T�C�Y��\�����܂��B\n\n";
		message = message + "���I�����@\n";
		message = message + "�ʐM���ʂ�\�����Ă����ԂŁu�I���v�I�����Ă��������B\n";
		message = message + "�܂��ʐM�I�v�V�����́u�ʐM�㎩���I���v�Ƀ`�F�b�N�����Ă����ƁA�ʐM�I����A10�b���x�o�߂���ƏI�����܂��B\n\n";
        message = message + "�����̑�\n";
        message = message + "a2BD�́AMIDP2.0�����JSR75(fileConnection API)�𗘗p���Ă��܂��B���̋@�\����������Ă��Ȃ��ꍇ�ɂ͂����p�ɂȂ�܂���B���������������B\n\n";
        message = message + "WX310SA/WX310J�����g���̏ꍇ�ɂ́A�ʐM�I�v�V�����́uWX310��gzip�]���΍�v�Ƀ`�F�b�N�����Ă�����s���Ă��������B(WX320K�͕s�v�ł��B)\n\n";
        message = message + "702NK/NKII�Ȃǂ̋@������g���̏ꍇ�ɂ́A�ʐM�I�v�V�����́u�����擾�v�Ƀ`�F�b�N�����Ă�����s���Ă��������B\n\n";
		showDialog("�������", message, 0);
		return;
	}
	
	
	/**
	 * �_�C�A���O�̕\��
	 * @param message
	 */
	public void showDialog(String title, String message, int timeout)
	{
//		AlertType alertType = AlertType.INFO;
		AlertType alertType = null;

		// �_�C�A���O�\������...
		alert.setTitle(title);
		alert.setString(message);
		alert.setType(alertType);

		// �^�C���A�E�g�l���[���ȊO��������^�C���A�E�g�l��ݒ肷��
		if (timeout != 0)
		{
			alert.setTimeout(timeout);
		}
		else
		{
			alert.setTimeout(Alert.FOREVER);
		}

		// �_�C�A���O����ʕ\������
		Displayable currentForm = (Display.getDisplay(parent)).getCurrent();
		Display.getDisplay(parent).setCurrent(alert, currentForm);
		
	}
}
