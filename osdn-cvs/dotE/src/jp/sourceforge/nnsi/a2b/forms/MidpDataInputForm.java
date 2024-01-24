/**
 *  a2B Framework�p�̃f�[�^���͉�ʃp�b�P�[�W�ł��B
 */
package jp.sourceforge.nnsi.a2b.forms;
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

import jp.sourceforge.nnsi.a2b.frameworks.IMidpForm;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;

/**
 * �f�[�^���̓t�H�[��
 * @author MRSa
 *
 */
public class MidpDataInputForm extends Form implements IMidpForm, ItemCommandListener, CommandListener
{
    private final int    CMD_EXIT = -1; // ���̓L�����Z��
    private final int    CMD_OK   = 0;  // ���͊m��

    private MidpSceneSelector parent = null;
    private IDataInputStorage storageDb = null;

    private Command  okCmd    = new Command("OK", Command.ITEM, CMD_OK);
    private Command  exitCmd  = new Command("Cancel", Command.EXIT, CMD_EXIT);

    private StringItem okButton     = null;
    private StringItem cancelButton = null;

    private boolean   showOkButton = true;
    private boolean   showCancelButton = true;
    
    private int    formId = -1;
    private String  titleString = null;
    
    /**
     * �R���X�g���N�^�ł͕K�v�ȃN���X��ݒ肵�܂�
     * @param arg0 �^�C�g�������l
     * @param id   �t�H�[��ID
     * @param selector �V�[���Z���N�^
     * @param storage ��ʓ��̓f�[�^�X�g���[�W
     */
    public MidpDataInputForm(String arg0, int id, MidpSceneSelector selector, IDataInputStorage storage)
    {
        // �e�N���X��ݒ肷��
        super(arg0);
        
        // �ϐ���ݒ肷��
        parent = selector;
        formId = id;
        storageDb = storage;

        // �L�����Z���R�}���h�AOK�R�}���h�����j���[�ɓ����
        this.addCommand(exitCmd);
        this.addCommand(okCmd);
        this.setCommandListener(this);
    }

    /**
     *  �R�}���h���X�i���s���̏���
     *  @param c �R�}���h
     *  @param item �A�C�e��
     */
    public void commandAction(Command c, Item item) 
    {
        int command = c.getPriority();
        commandActionMain(command);
        return;
    }

    /**
     *   ��ʃ^�C�g����ݒ肷��
     *   @param title ��ʃ^�C�g��
     */
    public void setTitleString(String title)
    {
        titleString = title;
    }
    
    /**
     *  ��ʃt�H�[��ID���擾����
     *  @return ���ID
     */
    public int getFormId()
    {
        return (formId);
    }

    /**
     *   �R�}���h�{�^�����s���̏���
     *   @param c �R�}���h
     *   @param d ��ʃN���X
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
            // ���̓L�����Z���w�����󂯕t����
            storageDb.cancelEntry();
        }
        else if (command == CMD_OK)
        {
            // �f�[�^�i�[�w�����󂯕t����
            storageDb.dataEntry();
        }
        else
        {
            // ��L�̃R�}���h�ł͂Ȃ��ꍇ...
            titleString = "[" + command + "]";
            return;
        }

        // ��ʏI������
        finishForm();
        return;
    }

    /**
     *  ��ʕ\���̏������s��
     *
     */
    public void prepare()
    {
        // ���ɉ������Ȃ�
        return;
    }

    /**
     *  ��ʕ\���̊J�n
     *
     */
    public void start()
    {
        // �X�g���[�W�N���X�ɏ������˗�
        storageDb.prepare();
        
        // ��ʂ�S���N���A����
        deleteAll();

        // �^�C�g����ݒ肷��
        setTitle(titleString);
        
        // ���̓t�B�[���h
        int nofField = storageDb.getNumberOfDataInputField();
        for (int index = 0; index < nofField; index++)
        {
            TextField inputField = storageDb.getDataInputField(index);
            if (inputField != null)
            {
                append(inputField);

                // �X�y�[�T�[
                append(new Spacer(0, 10));
            }
        }

        // OK�{�^��
        if (showOkButton == true)
        {
            okButton  = new StringItem("", "OK", Item.BUTTON);
            okButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
            okButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_OK));
            okButton.setItemCommandListener(this);
            append(okButton);
        }
        else
        {
            okButton = null;
        }

        // �X�y�[�X���󂯂�...
        append(new Spacer(0, 10));

        // ��ʂփI�v�V������ݒ肷��
        int nofOption = storageDb.getNumberOfOptionGroup();
        for (int index = 0; index < nofOption; index++)
        {
            ChoiceGroup choice = storageDb.getOptionField(index);
            if (choice != null)
            {
                append(new Spacer(0, 10));
                append(choice);
            }
        }

        // �X�y�[�X���󂯂�...
        append(new Spacer(0, 20));

        // Cancel�{�^��
        if (showCancelButton == true)
        {
            cancelButton  = new StringItem("", "Cancel", Item.BUTTON);
            cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
            cancelButton.setDefaultCommand(new Command("Cancel", Command.SCREEN, CMD_EXIT));
            cancelButton.setItemCommandListener(this);
            append(cancelButton);
        }
        else
        {
            cancelButton = null;
        }
        return;
    }

    /**
     *  ��ʕ\���̃N���[���A�b�v
     * 
     */
    protected void finishForm()
    {
        // �I�u�W�F�N�g�̂��Ƃ��܂�
        okButton = null;
        cancelButton = null;
        storageDb.finish();

        int nextScene = storageDb.nextSceneToChange(formId);
        if (nextScene == IDataInputStorage.SCENE_TO_PREVIOUS)
        {
            // �O��ʂ֖߂�
            parent.previousScene();
        }
        else
        {
            // ����ʂ֐؂�ւ���
            parent.changeScene(nextScene, storageDb.nextSceneTitleToChange(formId));
        }
        System.gc();
        return;
    }

    /**
     *   OK�{�^����p�ӂ��邩�H
     * 
     *   @param isShow �\������(true) / �\�����Ȃ�(false)
     */
    public void setOkButton(boolean isShow)
    {
        showOkButton = isShow;
        return;
    }

    /**
     *   Cancel�{�^����p�ӂ��邩�H
     * 
     *   @param isShow �\������(true) / �\�����Ȃ�(false)
     */
    public void setCancelButton(boolean isShow)
    {
        showCancelButton = isShow;
        return;
    }
}
