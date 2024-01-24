import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.ItemCommandListener;
import javax.microedition.lcdui.Spacer;
import javax.microedition.lcdui.StringItem;
import javax.microedition.lcdui.TextField;
import javax.microedition.lcdui.Choice;

public class a2BDdataInput   extends Form implements ItemCommandListener, CommandListener
{

	a2BDsceneSelector parent = null;

	private final int         MAX_DATAINPUT   = 1024;

	private final int         CMD_EXIT        = -1;
	private final int         CMD_SHOWHELP    = 100;
	private final int         CMD_DOEXECUTE   = 0;
	private final int         CMD_DOSELECION  = 1;

    private TextField          inputUserAgent  = null;
	private TextField          inputData1      = null;
	private TextField          inputData2      = null;
	private StringItem         writeButton     = null;
	private StringItem         selectButton    = null;
	private ChoiceGroup        choiceGroup     = null;

	private a2BDPreference     dataStorage     = null;
	
	private Command            helpCmd         = new Command("Help", Command.ITEM, CMD_SHOWHELP);
	private Command            exitCmd         = new Command("Cancel", Command.EXIT, CMD_EXIT);
	
	/**
	 * �R���X�g���N�^
	 * @param selector
	 */
	public a2BDdataInput(String arg0, a2BDsceneSelector selector, a2BDPreference preference)
	{
		super(arg0);

		// �e�N���X��ݒ肷��
		parent = selector;

        // �f�[�^�ێ��N���X��ݒ肷��
		dataStorage = preference;

		// �L�����Z���R�}���h�A�w���v�R�}���h�����j���[�ɓ����
		this.addCommand(exitCmd);
		this.addCommand(helpCmd);
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

	/*
	 *   �R�}���h�{�^�����s���̏���
	 *   
	 */
	public void commandAction(Command c, Displayable d)
	{
		int command = c.getPriority();
		commandActionMain(command);
		return;
	}

	/*
	 *   �R�}���h���s���C��
	 * 
	 * 
	 */
	private void commandActionMain(int command)
	{
		if (command == CMD_EXIT)
		{
			// ���s�L�����Z��
			parent.EndDataInputForm(true);
			return;
		}
		if (command == CMD_DOEXECUTE)
		{
			// ���̓f�[�^�̊m��...
			dataStorage.setData1(inputData1.getString());
            dataStorage.setData2(inputData2.getString());
			dataStorage.setUserAgent(inputUserAgent.getString());
			dataStorage.setWX310workaroundFlag(choiceGroup.isSelected(0));
            dataStorage.setDivideGetHttp(choiceGroup.isSelected(1));
            dataStorage.setAutoFinish(choiceGroup.isSelected(2));

			// ���s�w��
			parent.EndDataInputForm(false);
		}
		if (command == CMD_DOSELECION)
		{
			parent.SelectFile(inputData2.getString());
		}
		if (command == CMD_SHOWHELP)
		{
			// �w���v��\������
			parent.ShowHelp();
		}
	}

	/**
	 * ���̓f�[�^�Q��ݒ肷��
	 * @return
	 */
	public void setData2(String data)
	{
		inputData2.setString(data);
	}

	/**
	 *   ��ʂ̏���
	 *
	 */
	public void prepareScreen(String title, String message, String message2, boolean select)
	{
		deleteAll();

		// �^�C�g���̕\��
		setTitle(title);

		// �f�[�^�̕\�����s��
		inputData1 = new TextField(message, dataStorage.getData1(), MAX_DATAINPUT, TextField.ANY);
		append(inputData1);

		// �X�y�[�T�[
		append(new Spacer(10, 10));

		if (message2 != null)
		{
			// �f�[�^�̕\�����s��
			inputData2 = new TextField(message2, dataStorage.getData2(), MAX_DATAINPUT, TextField.ANY);
			append(inputData2);
		}
		
		if (select == true)
		{
			// �I���{�^���̒ǉ�
			selectButton = new StringItem("", "�I��", Item.BUTTON);	
			selectButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
			selectButton.setDefaultCommand(new Command("�I��", Command.SCREEN, CMD_DOSELECION));
			selectButton.setItemCommandListener(this);
			append(selectButton);

		}

		// �X�y�[�T�[
		append(new Spacer(10, 30));

		// ���[�U�[�G�[�W�F���g
		inputUserAgent = new TextField("User-Agent", dataStorage.getUserAgent(), MAX_DATAINPUT, TextField.ANY);
		append(inputUserAgent);
		
		// �X�y�[�T�[
		append(new Spacer(10, 30));

		// �I���O���[�v
		choiceGroup	= new ChoiceGroup("�ʐM�I�v�V����", Choice.MULTIPLE);
		choiceGroup.append("WX310��gzip�]���΍�", null);
		choiceGroup.append("�����擾", null);
		choiceGroup.append("�ʐM�㎩���I��", null);

        // �����t���O�̐ݒ�
		if (dataStorage.getWX310workaroundFlag() == true)
        {
			choiceGroup.setSelectedIndex(0, true);
        }
        if (dataStorage.getDivideGetHttp() == true)
        {
        	choiceGroup.setSelectedIndex(1, true);
        }
        if (dataStorage.getAutoFinish() == true)
        {
        	choiceGroup.setSelectedIndex(2, true);
        }
		append(choiceGroup);

		// �X�y�[�T�[
		append(new Spacer(10, 20));
	
		// ���s�{�^��
		writeButton  = new StringItem("", "���s", Item.BUTTON);
		writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
		writeButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_DOEXECUTE));
		writeButton.setItemCommandListener(this);
		append(writeButton);

		return;
	}
}
