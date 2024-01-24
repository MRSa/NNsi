import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.ItemCommandListener;
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextField;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Spacer;

/**
 * ���̓t�H�[���Ȃ�
 * @author MRSa
 *
 */
public class kaniFileForm extends Form implements ItemCommandListener, CommandListener
{
	static public final int  SCENE_MAKEDIR        = 100;
	static public final int  SCENE_RENAMEINPUT    = 101;
	static public final int  SCENE_DUPLICATEINPUT = 102;
	static public final int  SCENE_EDITATTRIBUTE  = 103;
	static public final int  SCENE_TEXTEDITLINE   = 104;

	private final int         CMD_EXIT        = -1;
	private final int         CMD_DOEXECUTE   = 0;
	private final int         CMD_LINEEDIT    = 1;

	private final int         maxString       = 128;
	private final int         maxLineString   = 3072;
	
	private kaniFileSceneSelector parent   = null;

	private kaniFileDirList.fileInformation targetInfo = null;
	private String                          targetName = null;

	private StringItem  okButton     = null;
	private StringItem  cancelButton = null;
	private TextField   txtField     = null;
	private ChoiceGroup attributes   = null;
	
	private Command  exitCmd  = new Command("Cancel", Command.EXIT, CMD_EXIT);
	
	/**
	 * �R���X�g���N�^
	 * @param selector
	 */
	public kaniFileForm(String arg0, kaniFileSceneSelector selector)
	{
		super(arg0);

		// �e�N���X��ݒ肷��
		parent = selector;

		// �L�����Z���R�}���h�A�w���v�R�}���h�����j���[�ɓ����
		this.addCommand(exitCmd);
		this.setCommandListener(this);
	}

	/**
	 *  �R�}���h���X�i���s���̏���
	 *  
	 */
	public void commandAction(Command c, Item item) 
	{
		int command = c.getPriority();
		commandActionMain(command);
		return;
	}

	/**
	 *   �R�}���h�{�^�����s���̏���
	 *   
	 */
	public void commandAction(Command c, Displayable d)
	{
		int command = c.getPriority();
		commandActionMain(command);
		return;
	}

	/**
	 *   �R�}���h�̎��s���C��
	 * 
	 */
	private void commandActionMain(int command)
	{
		if (command == CMD_EXIT)
		{
			// ���s�L�����Z��
			parent.cancelForm();
			return;
		}
		if (command == CMD_DOEXECUTE)
		{
			String fileName = null;
			int attribute = 0;
			if (attributes != null)
			{
				// �������݋֎~����...
				if (attributes.isSelected(0) == true)
				{
				    attribute = attribute + 1;
				}

				// �B���t�@�C������
				if (attributes.isSelected(1) == true)
				{
					attribute = attribute + 2;
				}
				fileName = targetName;
			}
			else
			{
				fileName = txtField.getString();
			}
			
			// ���s�w��...
			parent.executeInputForm(fileName, attribute);
		}
		if (command == CMD_LINEEDIT)
		{
			// ��s�ҏW�̊���
			parent.commitLineEdit(txtField.getString());
		}

		// �ꉞ�A���̃N���A...
		targetInfo = null;
		targetName = null;
		attributes = null;
		return;
	}

	/**
	 * �\����؂�ւ���...
	 * @param scene
	 */
	public void changeScene(int scene, String message)
	{
		if (scene == SCENE_MAKEDIR)
		{
			prepareScreen(scene, "Dir�쐬", null, message);
		}
		if (scene == SCENE_RENAMEINPUT)
		{
			prepareScreen(scene, "���l�[��", targetName, message);
		}
		if (scene == SCENE_DUPLICATEINPUT)
		{
			prepareScreen(scene, "�t�@�C���̕���", targetName, message);			
		}
		if (scene == SCENE_EDITATTRIBUTE)
		{
			prepareScreenAttribute("�t�@�C�������ύX", targetName);
		}
		if (scene == SCENE_TEXTEDITLINE)
		{
			prepareScreenEditTextLine("�s�ҏW(" + message + "�s��)");
		}
		return;
	}	

	/**
	 * ��ʕ\���̏���(�N���X�����Ȃ�...)���s��
	 *
	 */
	public void prepare()
	{
		return;
	}

	/**
	 * ��ʕ\���̏������s��
	 * (��ʕ\�����̗̂��p���@)
	 *
	 */
	public void prepareScreen()
	{
		return;
	}

	/**
	 * �^�[�Q�b�g��������
	 * @param name
	 */
	public void setTargetName(String name)
	{
		targetName = name;
		return;
	}	

	/**
	 * �t�@�C����������
	 * @param info
	 */
	public void setFileInfo(kaniFileDirList.fileInformation info)
	{
		targetInfo = info;
		targetName = info.getFileName();
		return;
	}
	
    /**
     * ��ʕ\���̏������s��
     * @param scene ���(�t�H�[��)�̗��p���@
     * @param title �^�C�g��
     */
	public void prepareScreen(int scene, String title, String original, String data)
    {
    	// ��ʂ��N���A����
		deleteAll();

		// �^�C�g���̕\��
		setTitle(title);

		// �e�L�X�g���̕\��
		txtField = new TextField("Name", original, maxString, TextField.ANY);
		append(txtField);
		
		// ���s�{�^��
		okButton  = new StringItem("", "���s", Item.BUTTON);
		okButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
		okButton.setDefaultCommand(new Command("Execute", Command.SCREEN, CMD_DOEXECUTE));
		okButton.setItemCommandListener(this);
		append(okButton);

		// �L�����Z���{�^��
		cancelButton  = new StringItem("", "���~", Item.BUTTON);
		cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
		cancelButton.setDefaultCommand(new Command("Cancel", Command.SCREEN, CMD_EXIT));
		cancelButton.setItemCommandListener(this);
		append(cancelButton);
		return;
    }

	/**
     * ��ʕ\���̏������s��
     * @param scene ���(�t�H�[��)�̗��p���@
     * @param title �^�C�g��
     */
	public void prepareScreenAttribute(String title, String fileName)
    {
    	// ��ʂ��N���A����
		deleteAll();

		// �^�C�g���̕\��
		setTitle(title);

		// �t�@�C�����̐ݒ�
		StringItem msg = new StringItem("�t�@�C���� : ", targetInfo.getFileName());
		append(msg);
		
		// �X�y�[�T�̐ݒ�
		Spacer sp = new Spacer(20, 20);
		append(sp);
		
		// �t�@�C�������̐ݒ�
		attributes = new ChoiceGroup("�t�@�C������", Choice.MULTIPLE);
		attributes.append("�������݋֎~", null);
		attributes.append("�B���t�@�C��", null);
        append(attributes);

        // �������݉ېݒ�̃`�F�b�N...
        if (targetInfo.isWritable() == false)
        {
        	attributes.setSelectedIndex(0, true);
        }

        // �B�������̃`�F�b�N...
        if (targetInfo.isHidden() == true)
        {
        	attributes.setSelectedIndex(1, true);
        }
		
		// ���s�{�^��
		okButton  = new StringItem("", "���s", Item.BUTTON);
		okButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
		okButton.setDefaultCommand(new Command("Execute", Command.SCREEN, CMD_DOEXECUTE));
		okButton.setItemCommandListener(this);
		append(okButton);

		// �L�����Z���{�^��
		cancelButton  = new StringItem("", "���~", Item.BUTTON);
		cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
		cancelButton.setDefaultCommand(new Command("Cancel", Command.SCREEN, CMD_EXIT));
		cancelButton.setItemCommandListener(this);
		append(cancelButton);
		return;
    }

	/**
	 * ��s�ҏW�̉�ʑI��
	 * 
	 * @param title
	 */
    private void prepareScreenEditTextLine(String title)
    {
    	deleteAll();

    	// �^�C�g���̕\��
		setTitle(title);

		// �e�L�X�g���̕\��
		txtField = new TextField("", targetName, maxLineString, TextField.ANY);
		append(txtField);

		// ���s�{�^��
		okButton  = new StringItem("", "�m��", Item.BUTTON);
		okButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
		okButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_LINEEDIT));
		okButton.setItemCommandListener(this);
		append(okButton);

		// �L�����Z���{�^��
		cancelButton  = new StringItem("", "���~", Item.BUTTON);
		cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
		cancelButton.setDefaultCommand(new Command("Cancel", Command.SCREEN, CMD_EXIT));
		cancelButton.setItemCommandListener(this);
		append(cancelButton);
		return;

    }
	
}
