import java.io.InputStream;
import java.io.OutputStream;
import java.util.Date;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.Choice;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.Spacer;
import javax.microedition.lcdui.ItemCommandListener;
import javax.microedition.lcdui.TextField;
import javax.microedition.lcdui.TextBox;
import javax.microedition.lcdui.StringItem;
//import java.util.Enumeration;
//import javax.microedition.lcdui.AlertType;
//import a2BNgWordManager.a2BngWord;

/**
 *  a2BMessageWriteForm
 *
 */
public class a2BMessageWriteForm  extends Form implements ItemCommandListener, CommandListener
{
    private a2BMain                 parent      = null;
    private a2BsjConverter          sjConverter = null;
    private a2Butf8Conveter         utf8Converter = null;
    private a2BConfirmationListener listener    = null;
    private Alert                   alert       = null;

    private final int         maxTitle        = 64;
    private final int         maxEmail        = 64;
    private final int         maxName         = 64;
    private final int         maxUrl          = 256;
    private final int         maxMessage      = 6400;
    private final int         MARGIN          = 10;
    private final long        TIME_OFFSET     = 50;

    private final int         CMD_EXIT        = -1;
    private final int         CMD_DOEXECUTE   = 0;
//    private final int         CMD_OYSTERLOGIN = 1;
    private final int         CMD_APPLYLIST   = 2;
    private final int         CMD_DONE        = 3;
    private final int         CMD_SETPROXY    = 4;
    private final int         CMD_WRITEFILE   = 5;
    private final int         CMD_LOADFILE    = 6;
    private final int         CMD_OPERATION   = 7;
    private final int         CMD_RESERVE     = 8;
    private final int         CMD_GETLOG      = 9;
    private final int         CMD_DETAIL      = 10;
    private final int         CMD_RESET_FONT  = 11;
    private final int         CMD_HTTP_COMM   = 12;
    private final int         CMD_DETAIL_VIEW = 13;
    private final int         CMD_DOWRITE     = 14;

    private final int         SCENE_WRITE     = 0;
    private final int         SCENE_INFORM    = 1;
//    private final int         SCENE_MEMOFORM  = 2;
//    private final int         SCENE_THREOPE   = 3;

    private int                messageIndex    = 0;
    private boolean           sendTitle       = false;
    private boolean           isFileLoaded    = false;
    private boolean           isGetNumber     = false;
    private boolean           isWriting       = false;

    private TextField          writeName       = null;
    private TextField          writeEmail      = null;
    private TextField          writeTitle      = null;
    private TextField          writeMessage    = null;
    private TextBox            msgBox          = null;
    private TextField          writeCount      = null;
    private TextField          aaWidth         = null;
    private TextField          keyRepeatMs     = null;
    private TextField          oysterName      = null;
    private TextField          oysterPass      = null;
    private TextField          beUserName      = null;
    private TextField          bePassword      = null;
    private TextField          divideSize      = null;
    private TextField          fontIdNumber    = null;

    private StringItem         writeButton     = null;
    private StringItem         cancelButton    = null;
    private StringItem         saveButton      = null;
    private StringItem         loadButton      = null;
    private StringItem         detailButton    = null;

    private ChoiceGroup        choiceGroup     = null;
//    private ChoiceGroup        choiceComm      = null;
    private ChoiceGroup        choiceOptFavor  = null;
    private ChoiceGroup        choiceListFunc  = null;
    private ChoiceGroup        choiceViewFunc  = null;
    private ChoiceGroup        useOysterInfo   = null;

    private String             targetTitle     = null;
    private String             receivedTitle   = null;
    private String             currentCookie   = "";
    private String             extendString    = "";
//    private String             temporaryMessage = null;

    private int      scene           = 0;
    private int      pickupHour      = 0;
    private boolean  returnToMessage = false;

    private Command  execCmd    = new Command("���s",     Command.ITEM, CMD_DOEXECUTE);
    private Command  exitCmd    = new Command("Cancel",   Command.EXIT, CMD_EXIT);
    private Command  opeCmd     = new Command("����",     Command.SCREEN, CMD_OPERATION);
    private Command  reserveCmd = new Command("�擾�\��", Command.SCREEN, CMD_RESERVE);
    private Command  fontCmd    = new Command("font������", Command.SCREEN, CMD_RESET_FONT);
    private Command  getCmd     = new Command("�X���擾", Command.SCREEN, CMD_GETLOG);
    private Command  okCmd      = new Command("Done",     Command.ITEM, CMD_DOEXECUTE);
    private Command  writeCmd   = new Command("����",     Command.EXIT, CMD_DOWRITE);
    private Command  doneCmd    = new Command("OK",       Command.ITEM, CMD_DONE);
    private Command  httpCmd    = new Command("http�ʐM", Command.ITEM, CMD_HTTP_COMM);

    /**
     *   �R���X�g���N�^
     *   
     */
    public a2BMessageWriteForm(String arg0, a2BMain object, a2BsjConverter converterSJ, a2Butf8Conveter converterUTF8)
    {
        super(arg0);

        // �e�N���X
        parent = object;

        // UTF8 => Shift JIS�ϊ����[�`��
        sjConverter = converterSJ;
        
        // Shift JIS => UTF8�ϊ����[�`��
        utf8Converter = converterUTF8;
        
        // �R�}���h���X�i
        listener = new a2BConfirmationListener(this);

        // �A���[�g�_�C�A���O
        alert = new Alert("");
        alert.setType(null);
        alert.setTimeout(Alert.FOREVER);
        alert.setCommandListener(listener);

        // �L�����Z���R�}���h�����j���[�ɓ����
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
     *  �������݉�ʂ̏I������...
     *
     */
    private void finishWriteForm(int afterAction)
    {
        // �������݉�ʂ̏I��...
    	fontIdNumber = null;
        divideSize = null;
        aaWidth    = null;
        keyRepeatMs = null;
        writeCount = null;
        oysterName = null;
        oysterPass = null;
        beUserName = null;
        bePassword = null;
        writeTitle = null;
        writeEmail = null;
        writeMessage = null;
        writeButton = null;
        useOysterInfo = null;
        saveButton  = null;
        loadButton  = null;
        detailButton = null;
        cancelButton = null;
        if (isFileLoaded == true)
        {
            saveTemporary();
        }
        isFileLoaded = false;
        sendTitle    = false;
        parent.returnToPreviousForm(afterAction);
        System.gc();
        return;
    }
    
    /**
     *   �R�}���h���s���C��
     * 
     */
    private void commandActionMain(int command)
    {
        if (command == CMD_EXIT)
        {
            // �������݉�ʂ̏I��... (�L�����Z���I��)
            this.removeCommand(opeCmd);
            this.removeCommand(reserveCmd);
            this.removeCommand(getCmd);
            this.removeCommand(fontCmd);
            this.removeCommand(httpCmd);
            isFileLoaded = false;   // �e���|�����t�@�C���͏������Ȃ�
            if (isGetNumber == true)
            {
                parent.setFavorThreadLevel((Integer.valueOf(writeName.getString())).intValue());
            }
            isGetNumber  = false;
            finishWriteForm(parent.ACTION_NOTHING_CANCEL);
            return;
        }
        if (command == CMD_DOEXECUTE)
        {
            // �������ݑO�̊m�F���{...
            if (isWriting == true)
            {
                // �������ݒ��̏ꍇ�ɂ́A�ēx�������݂����Ȃ�
                return;
            }
            parent.setWriteName(writeName.getString());
            if (sendTitle == false)
            {
                confirmation(SCENE_WRITE, "�������݊m�F", targetTitle, "�ɏ������݂܂��B��낵���ł����H");
            }
            else
            {
                confirmation(SCENE_WRITE, "�X�����Ċm�F", targetTitle, "�ɃX���𗧂Ă܂��B��낵���ł����H");
            }
            return;
        }
        if (command == CMD_APPLYLIST)
        {
            boolean ignoreCase   = false;
            boolean useGoogleSearch = false;
            boolean use2chSearch = false;
            if (useOysterInfo != null)
            {
                // �啶��/����������ʂ��錟����?
                ignoreCase = useOysterInfo.isSelected(0);

                // google�Ō������邩?
                useGoogleSearch = useOysterInfo.isSelected(1);

                if (useOysterInfo.size() > 2)
                {
                    // 2�����˂錟���𗘗p���邩�H
                    use2chSearch = useOysterInfo.isSelected(2);
                }
            }

            // �X�����Č㒊�o����
            if (writeCount != null)
            {
                pickupHour = (Integer.valueOf(writeCount.getString())).intValue();
            }
            
            // ������̐ݒ�
            parent.setInputData(scene, writeTitle.getString(), a2BsubjectDataParser.STATUS_NOTSPECIFY, ignoreCase, use2chSearch, useGoogleSearch, pickupHour);

            // �������݉�ʂ̏I��...
            finishWriteForm(parent.ACTION_NOTHING);
            return;
        }
        if (command == CMD_DONE)
        {
            // �������݉�ʂ̏I��...
            finishWriteForm(parent.ACTION_NOTHING);
            return;
        }
        if (command == CMD_SETPROXY)
        {
            // proxy�f�[�^�̐ݒ�
            parent.setProxyUrl(scene, writeTitle.getString(), choiceGroup.getSelectedIndex());

            // �����[�U�����p�X���[�h�̐ݒ�
            parent.setOysterName(oysterName.getString());
            parent.setOysterPassword(oysterPass.getString());

            // Be@2ch�̃��[�U�����p�X���[�h�̐ݒ�
            if (beUserName != null)
            {
                parent.setBeName(beUserName.getString());
            }
            if (bePassword != null)
            {
                parent.setBePassword(bePassword.getString());
            }
            
            parent.setFunctionKeyFeature(parent.SCENE_LISTFORM, choiceListFunc.getSelectedIndex());
            parent.setFunctionKeyFeature(parent.SCENE_VIEWFORM, choiceViewFunc.getSelectedIndex());

            // �N�����ɊJ����ʂ����C�ɓ���ꗗ�ɂ���ݒ�
            parent.setLaunchAsFavoriteList(choiceOptFavor.isSelected(0));

            // �u���E�U�N����̃A�N�V������ݒ肷��
            parent.setLaunchMode(choiceOptFavor.isSelected(1));

            // ���X���擾���ɂ��C�ɓ���֓o�^����t���O��o�^����
            parent.setAutoEntryFavorite(choiceOptFavor.isSelected(2));

            // �t�@�C���_�E�����[�h���A�ۑ��f�B���N�g�������[�g�f�B���N�g���ɂ���
            parent.setSaveAtRootDirectory(choiceOptFavor.isSelected(3));

            // �X�������擾���[�h��ݒ肷��
            parent.setDivideGetHttp(choiceOptFavor.isSelected(4));

            // �X���^�C���ꗗ�����ɕ\������
            parent.setShowTitleAnotherArea(choiceOptFavor.isSelected(5));

            // subject.txt��ۊǂ���...
            parent.setBackupSubjectTxt(choiceOptFavor.isSelected(6));

            // ���C�ɏ��ۊǉ񐔂̍팸...
            parent.setIgnoreFavoriteBackup(choiceOptFavor.isSelected(7));

            // �u���E�U�ŊJ���Ƃ�google�v���L�V���g�p����
            parent.setUseGoogleProxy(choiceOptFavor.isSelected(8));

            // �t�@�C���擾���AWX220J��UA���g�p����
            parent.setWX220JUserAgent(choiceOptFavor.isSelected(9));

            // �t�@�C���擾���A�L���b�V���t�@�C��������΂���𗘗p����            
            parent.setUseCachePictureFile(choiceOptFavor.isSelected(10));

            // �J�[�\���̓������y�[�W�����
            parent.setCursorSingleMovingMode(choiceOptFavor.isSelected(11));
            
            // ���O�̊����ϊ��𗘗p����
            parent.setForceKanjiConvertMode(choiceOptFavor.isSelected(12));
            
            // ������I�����Ԃ̐ݒ�
            parent.setWatchDogTime((Integer.valueOf(writeName.getString())).intValue());

            // ���X�܂Ƃߓǂݎ��̃��X��
            int resCount = (Integer.valueOf(writeCount.getString())).intValue();
            if ((resCount < 1)||(resCount > 9))
            {
                resCount = 3;
            }
            parent.setResBatchCount(resCount);

            // AA���[�h�����ړ���
            parent.setAAwidthMargin((Integer.valueOf(aaWidth.getString())).intValue());

            // �L�[���s�[�g���o����(ms)
            parent.setKeyRepeatTime((Integer.valueOf(keyRepeatMs.getString())).intValue());

            // ���[�U�[�G�[�W�F���g
            parent.setUserAgent(writeEmail.getString());
            
            // �����擾�T�C�Y
            parent.setDivideGetSize((Integer.valueOf(divideSize.getString())).intValue());
            
            // �t�H���g�T�C�Y�̍X�V
            int fontId = Integer.valueOf(fontIdNumber.getString()).intValue();
            if (fontId >= 0)
            {
                parent.updateFontData(false, fontId);
            }
            
            // �t�H���g�̃R�}���h���폜
            this.removeCommand(fontCmd);

            // �������݉�ʂ̏I��...
            finishWriteForm(parent.ACTION_NOTHING);
            return;
        }
        if (command == CMD_WRITEFILE)
        {
            // �t�@�C���Ƀf�[�^�����ۑ�����
            saveTemporary();
            return;
        }
        if (command == CMD_LOADFILE)
        {
            // �t�@�C������f�[�^��ǂݏo��
            loadTemporary();
            return;
        }
        if (command == CMD_DETAIL)
        {
            // �S��ʃ{�^���������ꂽ...
            String data = "";
            if (writeMessage != null)
            {
                data = writeMessage.getString();
            }
            else if (writeTitle != null)
            {
                data = writeTitle.getString();
            }
            if (maxMessage < data.length())
            {
                data = data.substring(0, (maxMessage - 2));
            }
            msgBox = new TextBox("�S���", data, maxMessage, TextField.ANY);
            scene = SCENE_INFORM;
            msgBox.addCommand(writeCmd);
            msgBox.addCommand(doneCmd);
//            msgBox.addCommand(exitCmd);
            msgBox.setCommandListener(listener);
            parent.callScreen(msgBox);
            return;
        }
        if (command == CMD_DETAIL_VIEW)
        {
            // �S��ʃ{�^���������ꂽ...
            String data = "";
            if (writeTitle != null)
            {
                data = writeTitle.getString();
            }
            else if (writeMessage != null)
            {
                data = writeMessage.getString();
            }
            if (maxMessage < data.length())
            {
                data = data.substring(0, (maxMessage - 2));
            }
            msgBox = new TextBox("�S���", data, maxMessage, TextField.ANY);
            scene = SCENE_INFORM;
            msgBox.addCommand(exitCmd);
            msgBox.setCommandListener(listener);
            parent.callScreen(msgBox);
            return;
        }
        if (command == CMD_OPERATION)
        {
            // �X������
            this.removeCommand(opeCmd);
            this.removeCommand(httpCmd);
            isFileLoaded = false;   // �e���|�����t�@�C���͏������Ȃ�
            finishWriteForm(parent.ACTION_THREAD_OPERATION);
            return;
        }
        if (command == CMD_RESERVE)
        {
            // �X���擾�\��
            this.removeCommand(reserveCmd);
            this.removeCommand(getCmd);
            isFileLoaded = false;   // �e���|�����t�@�C���͏������Ȃ�
            finishWriteForm(parent.ACTION_THREAD_RESERVE);
            return;
        }
        if (command == CMD_GETLOG)
        {
            // �X���擾
            this.removeCommand(reserveCmd);
            this.removeCommand(getCmd);
            isFileLoaded = false;   // �e���|�����t�@�C���͏������Ȃ�
            finishWriteForm(parent.ACTION_THREAD_GETLOG);
            return;
        }
        if (command == CMD_RESET_FONT)
        {
            // �t�H���g���̃��Z�b�g
            parent.updateFontData(true, 0);
            this.removeCommand(fontCmd);
            return;
        }
        if (command == CMD_HTTP_COMM)
        {
            // HTTP�ʐM...
            httpCommunication();

            // �ʐM�̃��j���[���폜����..
            this.removeCommand(httpCmd);
            return;
        }
        return;
    }

    /**
     *   �������݉�ʂ̏������s��
     *   
     */
    public void prepareWriteMessageForm(boolean useTitle, String formTitle, int index, int number)
    {
        deleteAll();

        // �������ݗpCookie�̓ǂݏo��
        currentCookie = parent.getWriteCookie();
        
        sendTitle    = useTitle;
        messageIndex = index;

        // �������݃^�C�g���̕\��
        targetTitle = formTitle;
        setTitle(formTitle);
        
        // �^�C�g�����̕\��
        if (useTitle == true)
        {
            writeTitle = new TextField("�^�C�g��", "", maxTitle, TextField.ANY);
            append(writeTitle);
        }
        else
        {
            writeTitle = null;
        }

        // ���O���̕\��
        String nameData = parent.getWriteName();
        writeName = new TextField("�y���O�z", nameData, maxName, TextField.ANY);
//        writeName.setInitialInputMode("IS_KANJI");

        // E-mail���̕\��
        writeEmail = new TextField("�yE-mail�z", "sage", maxEmail, TextField.ANY);

        // �{�����̕\��
        String msg = "";
        if (number > 0)
        {
            // ���X�Q�Ɣԍ����f�t�H���g�œ����悤�ɂ���
            msg = ">>" + number;
        }
        writeMessage = new TextField("�y�{���z", msg, maxMessage, TextField.ANY);
//      writeMessage.setConstraints(TextField.ANY);
//      writeMessage.setInitialInputMode("IS_KANJI");
//      writeMessage.setInitialInputMode("UCB_HIRAGANA");
        writeMessage.setLayout(Item.LAYOUT_2|Item.LAYOUT_VEXPAND|Item.LAYOUT_NEWLINE_AFTER);
//        writeMessage.setPreferredSize(-1, -1);

        // �S��ʃ{�^��
        detailButton = new StringItem("", "�S���", Item.BUTTON);
        detailButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        detailButton.setDefaultCommand(new Command("DETAIL", Command.SCREEN, CMD_DETAIL));
        detailButton.setItemCommandListener(this);
    
        // �������݃{�^��
        writeButton  = new StringItem("", "��������", Item.BUTTON);
        writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        writeButton.setDefaultCommand(new Command("WRITE", Command.SCREEN, CMD_DOEXECUTE));
        writeButton.setItemCommandListener(this);

/**
        // �L�����Z���{�^�� (�둀��h�~�̂��ߍ폜����
        cancelButton = new StringItem("", "�L�����Z��", Item.BUTTON);
        cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        cancelButton.setDefaultCommand(new Command("CANCEL", Command.EXIT, CMD_EXIT));
        cancelButton.setItemCommandListener(this);
**/

        // �X�y�[�T�[
        Spacer sp = new Spacer(10, 10);

        // �t�@�C�����ۑ��{�^��
        saveButton  = new StringItem("", "(���ۑ�)", Item.BUTTON);
        saveButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        saveButton.setDefaultCommand(new Command("SAVE", Command.SCREEN, CMD_WRITEFILE));
        saveButton.setItemCommandListener(this);

        // �t�@�C���ǂݍ��݃{�^��
        loadButton  = new StringItem("", "(�Ǎ�)", Item.BUTTON);
        loadButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        loadButton.setDefaultCommand(new Command("READ", Command.SCREEN, CMD_LOADFILE));
        loadButton.setItemCommandListener(this);

        // Form���\�z����...
        append(writeName);
        append(writeEmail);
        append(writeMessage);

        // �ڍ׃{�^��
        append(detailButton);
        
        String sessionId = parent.getOysterSessionId(0);
        String be2chCookie = parent.getBe2chCookie(0);

        // �I���O���[�v
        useOysterInfo    = new ChoiceGroup("�ʐM�I�v�V����", Choice.MULTIPLE);
        useOysterInfo.append("Cookie������A����", null);
        useOysterInfo.setSelectedIndex(0, false);
        if ((sessionId != null)||(be2chCookie != null))
        {
            if (sessionId != null)
            {
                useOysterInfo.append("2ch���O�C�����̗��p", null);
                useOysterInfo.setSelectedIndex(1, true);
            }
            else
            {
                useOysterInfo.append("-----", null);
                useOysterInfo.setSelectedIndex(1, false);
            }
            if (be2chCookie != null)
            {
                useOysterInfo.append("Be@2ch���O�C�����̗��p", null);
                useOysterInfo.setSelectedIndex(2, true);
            }
            else
            {
                useOysterInfo.append("-----", null);
                useOysterInfo.setSelectedIndex(2, false);
            }
        }
        append(useOysterInfo);

        append(writeButton);
//        append(cancelButton);
        append(sp);
        append(saveButton);
        append(loadButton);

        return;
    }
    
    /**
     *   �������݌��ʂ�\������
     * 
     */
    private void completedWriteMessage(boolean returnToForm, String result)
    {

        // BUSY�_�C�A���O���B��
        parent.hideBusyMessage();
        parent.updateBusyMessage("", "", false);

        // �ʐM���ʂ̕\��
        information("�������݌���", "", result);

        // �������݉�ʂ̏I���ݒ�...
        if (returnToForm == true)
        {
            returnToMessage = true;
        }
        else
        {
            returnToMessage = true;
        }

        // �������ݗpCookie���L������
        parent.setWriteCookie(currentCookie);
        return;
    }

    /**
     * �t�@�C���փf�[�^���o�͂���
     * 
     * @param fileName �o�̓t�@�C����
     * 
     */
    private void saveToFile(String writeFileName)
    {
        FileConnection connection = null;
        OutputStream  out = null;
        
        // �t�@�C�����𐶐�����
        String fileName = parent.getBaseDirectory() + writeFileName;
        try 
        {
            // �t�@�C���̏�������
            connection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
            if (connection.exists() == true)
            {
                // �t�@�C�������݂����ꍇ�ɂ́A��x�����č�蒼��
                connection.delete();
            }
            if ((writeTitle == null)&&(writeMessage == null))
            {
                // �������ރf�[�^���Ȃ��ꍇ...�t�@�C�����������ďI������
                connection.close();
                return;
            }
            connection.create();
            out = connection.openOutputStream();

            // �t�B�[���h�ɂ���f�[�^���o�͂���
            if (writeTitle == null)
            {
                String crlf = "\r\n";
                out.write((writeName.getString()).getBytes());
                out.write(crlf.getBytes());
                out.write((writeEmail.getString()).getBytes());
                out.write(crlf.getBytes());
                out.write((writeMessage.getString()).getBytes());
            }
            else
            {
                out.write((writeTitle.getString()).getBytes());
            }
            out.close();
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(writeFile)", " Exception  ee:" + e.getMessage());
            try
            {
                if (out != null)
                {
                    out.close();
                }
            }
            catch (Exception e2)
            {
            
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
            
            }
        }
        return;        
    }

    /**
     *  �t�@�C������̃f�[�^�ǂݏo�����C������
     * 
     * 
     */
    private void loadFromFile(String loadFileName)
    {
        FileConnection connection    = null;
        InputStream    in            = null;
        byte[]         buffer        = null;
        
        // �t�@�C�����𐶐�����
        String fileName = parent.getBaseDirectory() + loadFileName;
        try
        {
            connection   = (FileConnection) Connector.open(fileName, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  �f�[�^���ŏ����� size���ǂݏo��...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size];
            in.read(buffer, 0, size);
            in.close();
            connection.close();

            // �ǂݍ��񂾃f�[�^���Z�b�g����
            if (buffer.length != 0)
            {
                try
                {
                    String msgString = new String(buffer);
                    int pos1 = msgString.indexOf("\r\n", 0);
                    int pos2 = -1;
                    if (pos1 >= 0)
                    {
                        pos2 = msgString.indexOf("\r\n", pos1 + 1);
                    }
                    
                    if (pos1 >= 0)
                    {
                        if (pos2 >= 0)
                        {
                            writeEmail.setString  (msgString.substring(pos1 + 2, pos2));
                            writeMessage.setString(msgString.substring(pos2 + 2));
                        }
                        else
                        {
                            writeMessage.setString(msgString.substring((pos1 + 2)));
                        }
                        writeName.setString(msgString.substring(0, pos1));
                    }
                     else
                    {
                        writeMessage.setString(new String(buffer));
                    }
                 }
                catch (Exception e)
                {
                    writeMessage.setString(new String(buffer));                    
                }
            }
            buffer  = null;
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(loadFile)", fileName + " EXCEption  e:" + e.getMessage());
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                //            
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            buffer = null;
        }
        System.gc();
        return;
    }
    
    /**
     *   �t�@�C������̓ǂݏo�������{����
     * 
     */
    private void loadTemporary()
    {
        isFileLoaded = true;  // �t�@�C�����ǂݍ��܂ꂽ��t���O�����Ă�
        Thread thread = new Thread()
        {
            public void run()
            {
                loadFromFile("temp.txt");
            }
        };
        thread.start();
        return;
    }    

    /**
     *   �t�@�C���ւ̉��ۑ������{����
     *
     */
    private void saveTemporary()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                saveToFile("temp.txt");
            }
        };
        thread.start();        
        return;
    }    

    /**
     *   HTTP�ʐM�Ńf�[�^�擾�����s
     *
     */
    private void httpCommunication()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                String url = pickupUrl(writeTitle.getString());
                if (url != null)
                {
                    // URL�̒��o�ɐ��������Ƃ�����http�ʐM�����s����...
                    String fileName = "index.html";
                    int pos = url.lastIndexOf('/');
                    if (url.length() > (pos + 1))
                    {
                        fileName = url.substring(pos + 1);
                        pos = fileName.indexOf("?");
                        if (pos >= 0)
                        {
                            fileName = fileName.substring(pos + 1) + ".html";
                        }
                        else
                        {
                            pos = fileName.indexOf("*");
                            if (pos >= 0)
                            {
                                fileName = fileName.substring(pos + 1) + ".html";                                
                            }
                        }
                    }
                    String directory = parent.getBaseDirectory();
                    if (parent.getSaveAtRootDirectory() != true)
                    {
                        directory = parent.getFileSaveDirectory();
                        parent.makeDirectory(directory);
                        directory = directory + "/";
                    }
                    else
                    {
                        // bbstable.html ���㏑������悤�ȏ����͋֎~����B�B�B
                        if (fileName.indexOf("bbstable.html") >= 0)
                        {
                            System.gc();
                            parent.vibrate();
                            addCommand(httpCmd); // http�ʐM�{�^���̒ǉ�...
                            return;
                        }
                    }
                    fileName = directory + fileName;
                    parent.doHttpMain(url, fileName, null, false,  a2BMain.a2B_HTTPMODE_NOUSECOOKIE);
                    System.gc();
                    parent.vibrate();
                    addCommand(httpCmd); // http�ʐM�{�^���̒ǉ�...
                }
            }
        };
        thread.start();        
        return;        
    }
    
    /**
     * �t�H�[�����ɂ���URL�𒊏o����
     * 
     * @param data
     * @return
     */
    private String pickupUrl(String data)
    {
        String url = "";
        int pos = data.indexOf("://");
        if (pos < 0)
        {
            return (null);
        }
        pos = pos + 3;
        int cnt = pos;
        for (; cnt < data.length(); cnt++)
        {
            char ch = data.charAt(cnt);
            if ((ch <= ' ')||(ch == '"')||(ch == '>')||(ch == '\''))
            {
                break;
            }
        }
        url = data.substring(pos, cnt);
        return ("http://" + url);
    }
    
    /**
     *   �������݂̎�����...
     * 
     */
    private void    doWrite()
    {
        // URL�ƃt�@�C�������擾����
        String      url = parent.getBoardURL(messageIndex);
        String fileName = parent.getThreadFileName(messageIndex);
        String message  = "";
//        String confirmMessage = "&suka=pontan";
//        String confirmMessage = "&tepo=don";
        String confirmMessage = "&kuno=ichi";
        receivedTitle = "";

        // �擾�您��ю擾�t�@�C�����𐶐�����
        int pos = url.lastIndexOf('/', url.length() - 2);
        int logpos = fileName.lastIndexOf('.');

        // ���M��URL
        String sendUrl = url.substring(0, pos) + "/test/bbs.cgi?guid=ON";

        Date dateTime = new Date();
        long currentTime = (dateTime.getTime() / 1000) - 180;
        
        // �������݃��O�Ƀ��b�Z�[�W�������o��
        parent.outputTextFile("writelog.txt", dateTime.toString() + "\r\n" + targetTitle + "\r\n", writeMessage.getString(), true);

        // ���M�f�[�^�̏���
        String titleBuffer = "";
        if (sendTitle != false)
        {
            titleBuffer = sjConverter.parseToSJAndUrlEncode(writeTitle.getString());
        }
        String emailBuffer = sjConverter.parseToSJAndUrlEncode(writeEmail.getString());
        String nameBuffer  = sjConverter.parseToSJAndUrlEncode(writeName.getString());
        String msgBuffer   = sjConverter.parseToSJAndUrlEncode(writeMessage.getString());

        // �Z�b�V����ID �� Be2ch���O�C��Cookie���擾����
        String sessionId = null;
        String be2chLoginId = null;
        if (useOysterInfo != null)
        {
            // Cookie��������ɏ������݂��s��
        	if (useOysterInfo.isSelected(0) == true)
            {
            	currentCookie = "";
            }
            try
            {
                // ���O�C�����̏ꍇ...
                String orgSessionId = parent.getOysterSessionId(1);
                if ((useOysterInfo.isSelected(1) == true)&&(orgSessionId != null))
                {
                    // null����Ȃ�������URL�G���R�[�h�����{����
                    sessionId = a2BsjConverter.urlEncode(orgSessionId.getBytes());
                }
                
                String orgBe2chId = parent.getBe2chCookie(2);
                if ((useOysterInfo.isSelected(2) == true)&&(orgBe2chId != null))
                {
                    be2chLoginId = orgBe2chId;
                    // null����Ȃ�������URL�G���R�[�h�����{����
                    // sessionId = sjConverter.urlEncode(orgSessionId.getBytes());
                }
            }
            catch (Exception ex)
            {
            	// �i�D�������A�A�A IndexOutOfBoundsException�΍�B
            	// �i�����Ɣ��肵�Ƃ���[�����j
            }
        }

        String referer = "";
        if (sendTitle != false)
        {
            // ���M�p���b�Z�[�W(�X������)���쐬����...
            referer = url + "test/bbs.cgi/";
            message = "subject=" + titleBuffer;
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + confirmMessage + "&time=" + currentTime + "&submit=%91S%90%D3%94C%82%F0%95%89%82%A4%82%B1%82%C6%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";
        }
        else
        {
            // ���M�p���b�Z�[�W(���X)���쐬����...
            referer = url.substring(0, pos) + "/test/read.cgi/" + url.substring((pos + 1), (url.length())) + fileName.substring(0, logpos) + "/";
            message = "key=" + fileName.substring(0, logpos);
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + confirmMessage + "&time=" + currentTime + "&submit=%8F%E3%8B%4C%91%53%82%C4%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";
        }
        if (sessionId != null)
        {
            message = message + "&sid=" + sessionId;
        }
        receivedTitle = message;

/**
        // �������݃��O�Ƀ��b�Z�[�W�������o��
        parent.outputTextFile("writelog.txt", "DEBUG", message, true);
**/

        extendString  = "";
        receivedTitle = "";

        // cookie ���E�����߂ɋ�ł�
        postHttpMain(sendUrl, referer, getCookie(be2chLoginId, ""), message);

/**/
        if ((sessionId != null)&&(sendTitle != false))
        {
            // �����O�C�����̃X�����ẮA�Q�x�����K�v�͂Ȃ��悤��...
            // �����ŏ������݉�ʂ��I��������
            completedWriteMessage(returnToMessage, receivedTitle);
            return;
        }
/**/
        //  �����ŉ����R�[�h���`�F�b�N����
        if (checkIsFinishServerResponseMessage(receivedTitle) == true)
        {
        	// �������܂�Ă���A�Ɣ��肵���ꍇ...
        	
            // �������݃��O�ɑ��M����Cookie�������o��
            parent.outputTextFile("writelog.txt", "(COOKIE)", getCookie("",""), true);

            // �����ŏ������݉�ʂ��I��������
            completedWriteMessage(returnToMessage, receivedTitle);
            return;        	
        }
/**/
        
        // extendString (�������ݗp�L�[���[�h) ���E�����߂ɍēx�ł�����
//        postHttpMain(sendUrl, referer, getCookie(be2chLoginId, ""), message);

        
        // ������ǉ�����...
        if (extendString.length() == 0)
        {
            currentTime   = currentTime - 180;
            extendString = "&time=" + currentTime;
        }

        // �f�[�^�𒊏o�����M���b�Z�[�W���č\�z...
        if (sendTitle != false)
        {
            // �X�����ėp
            message = "subject=" + titleBuffer;
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + extendString + confirmMessage;
            message = message + "&submit=%91S%90%D3%94C%82%F0%95%89%82%A4%82%B1%82%C6%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";

        }
        else
        {
            // �X���Ƀ��X�p
            message = "key="  + fileName.substring(0, logpos);
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + extendString + confirmMessage + "&submit=%8F%E3%8B%4C%91%53%82%C4%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";
        }
        if (sessionId != null)
        {
            message = message + "&sid=" + sessionId;
        }
/**
        // �������݃��O�Ƀ��b�Z�[�W�������o��
        parent.outputTextFile("writelog.txt", "DEBUG-2", message, true);

        // �������݃��O�Ƀ��b�Z�[�W�������o��
        parent.outputTextFile("writelog.txt", "REFERER", referer, true);

        // �������݃��O�ɑ��M����Cookie�������o��
        parent.outputTextFile("writelog.txt", "(COOKIE)", getCookie("",""), true);
**/
        System.gc();  // �K�x�R��...

        // �������݌��ʂ��E���o�b�t�@���N���A...
        receivedTitle = "";

        // Cookie�����čđ�...
        postHttpMain(sendUrl, referer, getCookie(be2chLoginId, "; NAME=NNsi(MIDP); Mail="), message);

/***** 
        // ������ǉ�����...
        if (extendString.length() == 0)
        {
            currentTime   = currentTime - 180;
            extendString = "&time=" + currentTime;
        }

        // �f�[�^�𒊏o�����M���b�Z�[�W���č\�z...
        if (sendTitle != false)
        {
            // �X�����ėp
            message = "subject=" + titleBuffer;
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + extendString + confirmMessage;
            message = message + "&submit=%91S%90%D3%94C%82%F0%95%89%82%A4%82%B1%82%C6%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";

        }
        else
        {
            // �X���Ƀ��X�p
            message = "key="  + fileName.substring(0, logpos);
            message = message + "&FROM=" + nameBuffer + "&mail=" + emailBuffer;
            message = message + "&MESSAGE=" + msgBuffer + "&bbs=" + url.substring((pos + 1), (url.length() - 1));
            message = message + extendString + confirmMessage + "&submit=%8F%E3%8B%4C%91%53%82%C4%82%F0%8F%B3%91%F8%82%B5%82%C4%8F%91%82%AB%8D%9E%82%DE";
        }
        if (sessionId != null)
        {
            message = message + "&sid=" + sessionId;
        }
*****/
        System.gc();

/****
        // �������݃��O�Ƀ��b�Z�[�W�������o��
        parent.outputTextFile("writelog.txt", "DEBUG-3", message, true);

        // �������݃��O�Ƀ��b�Z�[�W�������o��
        parent.outputTextFile("writelog.txt", "REFERER", referer, true);
****/
        // �������݃��O�ɑ��M����Cookie�������o��
        parent.outputTextFile("writelog.txt", "(COOKIE)", getCookie("",""), true);

//        // ������x�đ�...
//        postHttpMain(sendUrl, referer, getCookie(be2chLoginId, "; NAME=NNsi(MIDP); Mail="), message);
        
        // �������݉�ʂ̏I��
        completedWriteMessage(returnToMessage, receivedTitle);
        return;
    }
    
    /**
     *   ���݂�Cookie���擾���� (�t�������������ĉ���)
     * 
     * @param be2chLoginId
     * @param appendData
     * @return ���ݐݒ肳��Ă���Cookie
     */
    private String getCookie(String be2chLoginId, String appendData)
    {
    	String returnCookie = "" + currentCookie;

        if (be2chLoginId != null)
        {
        	returnCookie = be2chLoginId + "; " + returnCookie;
        }
        return (returnCookie + appendData);        
    }

    /**
     *   �T�[�o����������Cookie�f�[�^��ݒ肷��
     * 
     * @param value
     */
    private void setCookie(String value)
    {
        // �������cookie�̃L�[���[�h�����𒊏o����
    	int cookieKeywordPos = value.indexOf("=");
    	int cookieEndPos = value.indexOf(";");
    	if ((cookieEndPos > 0)&&(cookieKeywordPos > cookieEndPos))
    	{
    		cookieKeywordPos = cookieEndPos;
    	}
        String keyword = "";
    	if (cookieKeywordPos > 0)
    	{
            keyword = value.substring(0, cookieKeywordPos);
    	}

    	// ����Cookie���� �L������keyword���܂܂�Ă��邩�`�F�b�N����
    	int isIncludeCookie = currentCookie.indexOf(keyword);
    	if (isIncludeCookie < 0)
    	{
            // �L�[���[�h���܂܂�Ă��Ȃ��A���̏ꍇ�́A���̂܂܂�����L������
    		int pos = value.indexOf(";");
            if (currentCookie.length() != 0)
            {
                currentCookie = currentCookie + "; ";
            }
            currentCookie = currentCookie + value.substring(0, pos);
            return;
    	}
        String cookieToSet = currentCookie;
    	int cookieLength = cookieToSet.length();
		int pos = value.indexOf(";");
    	int nextCookiePos = currentCookie.indexOf(";", isIncludeCookie);
    	if (nextCookiePos <= 0)
    	{
            // �����ɃZ�~�R�������Ȃ�������ACookie�̖�����ݒ肷��
    		nextCookiePos = cookieLength;
    	}
    	
    	// �󔒔�΂� (�x�����낤����...)
    	int nextIndex = nextCookiePos + 1;
    	while (cookieToSet.charAt(nextIndex) == ' ')
    	{
            nextIndex++;
    	}
    	// ��M����Cookie�f�[�^�Œl�������ւ���
        currentCookie = cookieToSet.substring(0, isIncludeCookie)  + cookieToSet.substring(nextIndex, cookieLength) + "; " + value.substring(0, pos);

        return;
    }
    
    /**
     *   �T�[�o����̉������b�Z�[�W���`�F�b�N����
     * 
     * @param checkString  �m�F���镶����
     * @return false�Ȃ�ē���
     */
    private boolean checkIsFinishServerResponseMessage(String checkString)
    {
    	int pos = checkString.indexOf("�������݂܂���");
    	if (pos >= 0)
    	{
    		// �������ݐ����̃��b�Z�[�W��M�������B
    		return (true);
    	}
        return (false);
    }    
    
    /**
     * UTF8�̕������EUC�֕ϊ����A�����URL�G���R�[�h����
     * @param utf8String
     * @return
     */
    public String utf8ToEUCandURLEncode(String utf8String)
    {
        int length    = utf8String.length() * 3;
        byte buffer[] = new byte[length];
        byte srcBuffer[] = sjConverter.parseToSJ(utf8String);
        utf8Converter.StrCopySJtoEUC(buffer, 0, srcBuffer, 0, srcBuffer.length);
        String convertStr = a2BsjConverter.urlEncode(buffer);
        buffer = null;
        srcBuffer = null;
        return (convertStr);
    }
    
    /**
     * EUC��UTF8�ɕϊ�����
     * 
     * @param eucString
     * @return
     */
    public String eucToUtf8(byte[] eucString, int startIndex, int length)
    {
        return (utf8Converter.parsefromEUC(eucString, startIndex, length));
    }
    
    /**
     * Shift JIS��UTF8�ɕϊ�����
     * 
     * @param sjString
     * @return
     */
    public String sjToUtf8(byte[] sjString, int startIndex, int length)
    {
        return (utf8Converter.parsefromSJ(sjString, startIndex, length));
    }

    /**
     * �m�F���������s����
     * @param newScene
     * @param title
     * @param message1
     * @param message2
     */    
    public void confirmation(int newScene, String title, String message1, String message2)
    {
        String message = message1 + message2;

        scene = newScene;
        alert.setTitle(title);
        alert.setString(message);
        alert.addCommand(execCmd);
        alert.addCommand(exitCmd);
        parent.callScreen(alert);

        return;
    }

    /**
     *  ���b�Z�[�W��\������
     * 
     * 
     */
    public void information(String title, String message1, String message2)
    {
        String message = message1 + message2;

        scene = SCENE_INFORM;
        alert.setTitle(title);
        alert.setString(message);
        alert.addCommand(okCmd);
        parent.callScreen(alert);

        return;
    }

    /**
     *  �L�����Z�����s
     * 
     * 
     */
    public void cancelConfirmation()
    {
        // �Ƃ肠�����R�}���h�ݒ���N���A
        if (scene != SCENE_INFORM)
        {
            alert.removeCommand(execCmd);
            alert.removeCommand(exitCmd);
        }
        else
        {
            alert.removeCommand(okCmd);
        }

        // �������݉�ʂɖ߂�
        parent.returnScreen();

        return;
    }
    
    /**
     *  �X������...
     * 
     * 
     */
    public void doOperation()
    {
        parent.returnScreen();
        return;
    }
    
    /**
     *  �ڍׂœ��͂������b�Z�[�W�𔽉f������
     * 
     *
     */
    public void setWriteDetailMessage(int command)
    {
        if (command != CMD_EXIT)
        {
            writeMessage.setString(msgBox.getString());
        }              

        parent.returnScreen();

        msgBox = null;
        System.gc();

        // �������ݑO�̊m�F���{...
        if ((isWriting == true)||(command != CMD_DOWRITE))
        {
            // �������ݒ��̏ꍇ�ɂ́A�ēx�������݂����Ȃ�
            return;
        }

        // �S��ʂ��璼�ڏ������ݏ�����...
        parent.setWriteName(writeName.getString());
        if (sendTitle == false)
        {
            confirmation(SCENE_WRITE, "�������݊m�F", targetTitle, "�ɏ������݂܂��B��낵���ł����H");
        }
        else
        {
            confirmation(SCENE_WRITE, "�X�����Ċm�F", targetTitle, "�ɃX���𗧂Ă܂��B��낵���ł����H");
        }
        return;
    }
    
    /**
     *   �m�F�I��(���s)
     * 
     */
    public void confirm()
    {
        // �Ƃ肠�����R�}���h�ݒ���N���A
        if (scene != SCENE_INFORM)
        {
            alert.removeCommand(execCmd);
            alert.removeCommand(exitCmd);
        }
        else
        {
            alert.removeCommand(okCmd);
        }

        // �������݉�ʂɖ߂�
        parent.returnScreen();
        
        // �������ݏ����̎��s...
        if (scene == SCENE_WRITE)
        {
            Thread thread = new Thread()
            {
                public void run()
                {
                    isWriting = true;
                    parent.updateBusyMessage("", "", false);
                    parent.showBusyMessage("�������ݒ�", targetTitle, a2BselectionForm.BUSY_FORM);
                    doWrite();
                    isWriting = false;
                }
            };
            try
            {
                // �������ݏI���܂ő҂�...
                thread.start();
//                thread.join();
            }
            catch (Exception e)
            {
                // �������Ȃ�...
            }
            return;
        }
        if (scene == SCENE_INFORM)
        {
            if (returnToMessage == true)
            {
                // �������݌��ʂ�OK�Ńt�@�C����ǂݍ���ł����ꍇ�ɂ́A�f�[�^���N���A����
                if (isFileLoaded == true)
                {
                    writeTitle = null;
                    writeMessage = null;
                    saveTemporary();
                }
                isFileLoaded = false;

                // �X���i�܂��̓X���ꗗ�j�������[�h����B
                parent.returnToPreviousForm(parent.ACTION_NOTHING);
                if (sendTitle == true)
                {
                    parent.reloadCurrentMessageList();
                }
                else
                {
                    parent.reloadCurrentMessage();
                }
                sendTitle       = false;
                returnToMessage = false;
            }
            return;
        }
        return;
    }

    /**
     *   HTTP�ڑ����g���ăt�@�C���ɏo�͂��� �i���C�������j
     *   
     */
    private int postHttpMain(String urlToPost, String referenceUrl, String cookie, String sendMessage)
    {
        HttpConnection c  = null;
        InputStream    is = null;
        OutputStream   os = null;
        int           rc = -1;
        String         key = "";
        String         value = "";

        String url = urlToPost;
        if (parent.PROXY_USE_ALL == parent.getUsedProxyScene())
        {
               // Proxy�o�R�ŃA�N�Z�X����
            url = parent.getProxyUrl() + "/" + url;
        }

        // �������ݎ��s�����b�Z�[�W��\������
        parent.updateBusyMessage("", "", false);
        String message = "URL\n  " + urlToPost;
        parent.showBusyMessage("http�ʐM������", message, a2BselectionForm.BUSY_FORM);        
        try 
        {
            c = (HttpConnection) Connector.open(url);
            {
                c.setRequestMethod(HttpConnection.POST);
                if (referenceUrl != null)
                {
                    c.setRequestProperty("referer", referenceUrl);
                }
                if (cookie != null)
                {
                    c.setRequestProperty("cookie", cookie);
                }
                c.setRequestProperty("accept", "text/html, image/jpeg, */*");
                c.setRequestProperty("content-language", " ja, en");
                String userAgent = parent.getUserAgent(false);
                c.setRequestProperty("user-agent", userAgent);
                c.setRequestProperty("x-2ch-ua", "a2B/1.00");
                c.setRequestProperty("content-type", "application/x-www-form-url-encoded");
            }

            // Getting the output stream may flush the headers
            os = c.openOutputStream();
            os.write(sendMessage.getBytes());

            message = "�ʐM��\n" + url;
            parent.updateBusyMessage("http�ʐM��", message, false);

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            rc = c.getResponseCode();
            if (rc != HttpConnection.HTTP_OK)
            {
                // �f�[�^�̒ʐM���s...
                parent.hideBusyMessage();
                parent.updateBusyMessage("", "", false);

                // close streams
                os.close();
                c.close();
                return (rc);
            }
            message = "��M�f�[�^��͒�...";
            parent.updateBusyMessage("http�ʐM", message, false);

            //////////  Cookie �̒��o�����{����  //////////
            int len   = (int)c.getLength();
            int index = 0;
            try
            {
                while (index >= 0)
                {
                    value = c.getHeaderField(index);
                    key   = c.getHeaderFieldKey(index);
                    if (key.compareTo("set-cookie") == 0)
                    {
                        setCookie(value);
                    }
                    else if (key.compareTo("Set-Cookie") == 0)
                    {
                        setCookie(value);
                    }
                    value = "";
                    key   = "";
                    index++;
                }
            }
            catch (Exception e)
            {
                // �G���[����ݒ�...
                receivedTitle = "C:" + e.getMessage();
            }

            //////////  �{�f�B���������������... /////////
            is = c.openInputStream();

            // Get the length and process the data
            int receivedSize = 0;
            if (len > 0)
            {
                int parsed         = 0;
                int position       = 0;
                int bottom         = 0;

                byte[] data        = new byte[(maxMessage * 2) + MARGIN];
                try
                {
                	int actual = 0;
                    do
                	{
                    	receivedSize = receivedSize + actual;
                        actual = is.read(data, receivedSize, ((maxMessage * 2) - receivedSize));
                	} while ((actual > 0)&&(receivedSize < (maxMessage * 2)));
                }
                catch (Exception e)
                {
                	// ��O���� (�ǂݍ��݃G���[���낤����A�����ł͉������Ȃ�)
                }
/*
                byte[] data        = new byte[len + MARGIN];
                actual              = is.read(data, 0, len);
*/
                String readData     = utf8Converter.parsefromSJ(data, 0, receivedSize);
                data                = null;
                String matchPattern = "";
//                receivedTitle       = extendString + "///" + sendMessage + "///" + readData;
                receivedTitle       = readData + "\n(bytes: " + receivedSize + "[length: " + len + "])";
//                receivedTitle       = readData;
/**
                // �������݃��O�Ɏ�M�f�[�^�������o��
                parent.outputTextFile("writelog.txt", "REPLY(DEBUG)", readData, true);
**/

                // �^�C�g��������������...
                matchPattern  = "<title>";
                position      = readData.indexOf(matchPattern);
                position      = position + matchPattern.length();
                bottom        = readData.indexOf("</title>", position);
                if (position < bottom)
                {
                    receivedTitle = readData.substring(position, bottom);
                }
                matchPattern  = "<b>";
                position      = readData.indexOf(matchPattern);
                if (position >= 0)
                {
                    position      = position + matchPattern.length();
                    bottom        = readData.indexOf("</b>", position);
                    if (position < bottom)
                    {
                        receivedTitle = receivedTitle + "\n   " + readData.substring(position, bottom);
                    }
                    returnToMessage = false;
                }
                else
                {
                    returnToMessage = true;
                }

                while (parsed < receivedSize)
                {
                    //  �f�[�^�A�C�e���𒊏o����...
                    matchPattern = "<input type=hidden name=";
                    position     = readData.indexOf(matchPattern, parsed);
                    if (position < 0)
                    {
                        break;
                    }
                    position     = position + matchPattern.length();
                    bottom       = readData.indexOf(" ", position);

                    key   = readData.substring(position, bottom);
                    if (key.charAt(0) == '"')
                    {
                        if ((key.compareTo("\"subject\"") == 0)||(key.compareTo("\"time\"") == 0))
                        {
                            key = key.substring(1, (key.length() - 1));
                            extendString = extendString + "&" + key + "=";
                        }
                        else
                        {
                            key = key.substring(1, (key.length() - 1));
                            extendString = extendString + "&" + key + "=";
                        }
                    }
                    else
                    {
                        if ((key.compareTo("subject") == 0)||(key.compareTo("time") == 0))
                        {
                            extendString = extendString + "&" + key + "=";
                        }
                        else
                        {
                            key = "";
                        }
                    }
                    
                    matchPattern = "value=";
                    position   = readData.indexOf(matchPattern, bottom);
                    position     = position + matchPattern.length();
                    bottom       = readData.indexOf(">", position);
                    value   = readData.substring(position, bottom);
                    if ((key.length() != 0)&&(value.length() > 0))
                    {
                        if (value.charAt(0) == '"')
                        {
                            if (value.length() > 2)
                            {
                                if (key.compareTo("time") == 0)
                                {
                                    long timeData = Long.parseLong(value.substring(1, (value.length() - 2)));
                                    extendString = extendString + (timeData - TIME_OFFSET);
                                }
                                else
                                {
                                    extendString = extendString + value.substring(1, (value.length() - 1));                                    
                                }
                            }
                        }
                        else
                        {
                            if (key.compareTo("time") == 0)
                            {
                                long timeData = Long.parseLong(value);
                                extendString = extendString + (timeData - TIME_OFFSET);
                            }
                            else
                            {
                                extendString = extendString + value;                                
                            }
                        }
                    }
                    parsed = bottom;                    
                }
                //  �f�[�^���N���A����
                readData = null;
            }
            else
            {
                receivedTitle = "??? (" + getCookie("","") + ")\n" + referenceUrl;
            }
            
            // close streams
            is.close();
            os.close();
            c.close();
        }
        catch (Exception e)
        {
            receivedTitle = receivedTitle + " I:" + e.getMessage();
            try
            {
                if (os != null)
                {
                    os.close();
                }
            }
            catch (Exception e2)
            {
                    
            }
            try
            {
                if (is != null)
                {
                    is.close();
                }
            }
            catch (Exception e2)
            {
                    
            }
            try
            {
                if (c != null)
                {
                    c.close();
                }
            }
            catch (Exception e2)
            {
                
            }
        }
//        receivedTitle = referenceUrl + " ### " + getCookie("", "") + " ### " + receivedTitle;
        parent.hideBusyMessage();
        parent.updateBusyMessage("", "", false);
        return (rc);
    }
    

    /**
     *   �������݉�ʂ��f�[�^���̓t�H�[���Ƃ��ė��p���鏀�����s��
     *   
     */
    public void prepareWriteFormUsingDataInputMode(int dataScene, String title, String itemName, String itemData, int maxLength, boolean ignoreCase)
    {
        deleteAll();

        // �^�C�g���T�C�Y�����肷��
        int length = 0;
        if (maxLength >= 0)
        {
            length = maxLength;
        }
        else
        {
            length = maxUrl;
        }
        
        // �V�[�����L������
        scene = dataScene;
        
        // �^�C�g���̕\��
        setTitle(title);

        // �^�C�g��
        writeTitle = new TextField(itemName, itemData, length, TextField.ANY);
        append(writeTitle);

        // OK�{�^��
        writeButton  = new StringItem("", "OK", Item.BUTTON);
        writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        writeButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_APPLYLIST));
        writeButton.setItemCommandListener(this);
        append(writeButton);

        // �X�y�[�T�[
        append(new Spacer(10, 10));

        // �I���O���[�v
        if (ignoreCase == true)
        {
            useOysterInfo = new ChoiceGroup("�����I�v�V����", Choice.MULTIPLE);
            useOysterInfo.append("�啶���Ə������̋�ʂ𖳎�", null);
            useOysterInfo.append("Google����(Web�u���E�U��)", null);
            if (scene == parent.SCENE_TITLE_SEARCH)
            {
                useOysterInfo.append("2�����˂錟���Ō���", null);
            }
            useOysterInfo.setSelectedIndex(0, true);
            append(useOysterInfo);
            if (scene == parent.SCENE_TITLE_SEARCH)
            {
                String pickString = "" + pickupHour;
                writeCount = new TextField("�X�����Čo�ߎ���(0:����)", pickString, 3, TextField.DECIMAL);
                append(writeCount);
            }
        }
        // �X�y�[�T�[
        append(new Spacer(0, 15));
        
/*
        // OK�{�^��
        writeButton  = new StringItem("", "OK", Item.BUTTON);
        writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        writeButton.setDefaultCommand(new Command("OK", Command.SCREEN, CMD_APPLYLIST));
        writeButton.setItemCommandListener(this);
        append(writeButton);
*/

        // �X�y�[�X���󂯂�...
        append(new Spacer(0, 20));

        // �L�����Z���{�^��
        cancelButton = new StringItem("", "�L�����Z��", Item.BUTTON);
        cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        cancelButton.setDefaultCommand(new Command("CANCEL", Command.EXIT, CMD_EXIT));
        cancelButton.setItemCommandListener(this);
        append(cancelButton);

        return;
    }

    /**
     *   �������݉�ʂ��e�L�X�g�ҏW�p�t�H�[���Ƃ��ė��p���鏀�����s��
     *   
     */
    public void prepareWriteFormUsingTextEditMode(String title, String message)
    {
        deleteAll();

        // �^�C�g���̕\��
        setTitle(title);
        
        // �f�[�^�̕\�����s��
        writeTitle = new TextField("", message, a2BMain.TEMP_PREVIEW_SIZE, TextField.ANY);
        writeTitle.setLayout(Item.LAYOUT_2 | Item.LAYOUT_VEXPAND);
        append(writeTitle);

        // �X�y�[�T�[
        Spacer sp = new Spacer(10, 10);
        append(sp);
        sp = null;

        // �t�@�C�����ۑ��{�^��
        saveButton  = new StringItem("", "(���ۑ�)", Item.BUTTON);
        saveButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        saveButton.setDefaultCommand(new Command("SAVE", Command.SCREEN, CMD_WRITEFILE));
        saveButton.setItemCommandListener(this);
        append(saveButton);

        // �L�����Z���{�^��
        cancelButton = new StringItem("", "�߂�", Item.BUTTON);
        cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        cancelButton.setDefaultCommand(new Command("CANCEL", Command.EXIT, CMD_EXIT));
        cancelButton.setItemCommandListener(this);
        append(cancelButton);

        // �S��ʃ{�^��
        detailButton = new StringItem("", "�S���", Item.BUTTON);
        detailButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        detailButton.setDefaultCommand(new Command("DETAIL", Command.SCREEN, CMD_DETAIL_VIEW));
        detailButton.setItemCommandListener(this);
        append(detailButton);

        // �X�y�[�X���󂯂�...
        append(new Spacer(0, 20));

        // HTTP�ʐM�{�^��
        this.addCommand(httpCmd);

        return;
    }

    /**
     *  ���j���[�ɃX�����상�j���[��ǉ�����..
     * 
     *
     */
    public void prepareThreadOperationMenu(String addInfo, int threadLevel)
    {
        // ����R�}���h��ǉ�����
        this.addCommand(opeCmd);

        // ���̑����
        StringItem item = new StringItem("", addInfo, Item.PLAIN);
        append(item);
        
        if (threadLevel >= 0)
        {
            // �X�����x���̐ݒ�
            String level = threadLevel + "";
            writeName = new TextField("���C�ɓ���ԍ�(0, 1-9)", level, 1, TextField.DECIMAL);
            append(writeName);
            isGetNumber = true;
        }
        else
        {
            isGetNumber = false;
        }
        return;
    }

    /**
     *  ���j���[�ɃX�����상�j���[��ǉ�����..
     * 
     *
     */
    public void preparePreviewMenu()
    {
        this.addCommand(reserveCmd);
        this.addCommand(getCmd);
        return;
    }
    
    /**
     *   �������݉�ʂ�a2B�ݒ�(Proxy�ݒ�)�t�H�[���Ƃ��ė��p���鏀�����s��
     *   
     */
    public void prepareWriteFormUsingProxySettingMode(int dataScene, String title, String proxyUrl, int useMode, boolean webLaunch)
    {
        deleteAll();

        // �^�C�g���T�C�Y�����肷��
        int length = maxUrl;

        // �V�[�����L������
        scene = dataScene;
        
        // �^�C�g���̕\��
        setTitle(title);

        // proxy URL
        writeTitle = new TextField("Proxy(Host���F�|�[�g�ԍ�)", proxyUrl, length, TextField.ANY);
        append(writeTitle);
        
        // �X�y�[�X������
        append(new Spacer(10, 5));

        // �����O�C����
        oysterName = new TextField("2ch���O�C����(��)", parent.getOysterName(), length, TextField.EMAILADDR);
        append(oysterName);
        
        // ���p�X���[�h
        oysterPass = new TextField("2ch�p�X���[�h(��)", parent.getOysterPassword(), length, TextField.PASSWORD);
        append(oysterPass);

/**********/
        // �X�y�[�X������
        append(new Spacer(10, 5));

        // Be@2ch���O�C����
        beUserName = new TextField("BE@2ch���O�C����", parent.getBeName(), length, TextField.EMAILADDR);
        append(beUserName);
        
        // Be@2ch�p�X���[�h
        bePassword = new TextField("BE@2ch�p�X���[�h", parent.getBePassword(), length, TextField.PASSWORD);
        append(bePassword);
/**********/

        // �X�y�[�X������
        append(new Spacer(10, 5));

        // Proxy�g�p�̑I����
        choiceGroup = new ChoiceGroup("Proxy���p�ݒ�", Choice.EXCLUSIVE);
        choiceGroup.append("���p���Ȃ�", null);
        choiceGroup.append("���p����(����GET�̂�)", null);
        choiceGroup.append("GET���̂ݗ��p����", null);
        choiceGroup.append("�����擾�̂ݗ��p����", null);
        choiceGroup.append("WX310��gzip�]���΍�", null);
        choiceGroup.setSelectedIndex(useMode, true);
        append(choiceGroup);

        // �X�y�[�X������
        append(new Spacer(10, 10));

/**
//        choiceComm = new ChoiceGroup("�V���m�F���@(�v�ċN��)", Choice.POPUP);
        choiceComm = new ChoiceGroup("�V���m�F���@(�v�ċN��)", Choice.EXCLUSIVE);
        choiceComm.append("FG�ʐM", null);
        choiceComm.append("BG�ʐM", null);
        int index = 0;
        if (commMode == true)
        {
            index = 1;
        }
        choiceComm.setSelectedIndex(index, true);
        append(choiceComm);

        // �X�y�[�X������
        append(new Spacer(10, 10));
**/        
//        choiceListFunc = new ChoiceGroup("�ꗗ��ʂO�L�[ ", Choice.POPUP);
        choiceListFunc = new ChoiceGroup("�ꗗ��ʂO�L�[�@�\", Choice.EXCLUSIVE);
        choiceListFunc.append("�t�H���g�T�C�Y�ύX", null);
        choiceListFunc.append("�����L�[�ړ����[�h", null);
        choiceListFunc.append("�w���v�\�����[�h", null);
        choiceListFunc.append("�X�����\��", null);
        choiceListFunc.append("���ǃX�����o", null);
        choiceListFunc.append("�X������", null);
        choiceListFunc.append("���[�h���j���[", null);
        choiceListFunc.append("���O�Q�ƃ��[�h", null);
        choiceListFunc.append("�N���A", null);
        choiceListFunc.append("�֘A�L�[���[�h", null);
        choiceListFunc.append("�X���폜", null);
        choiceListFunc.append("���O�C��(��/BE)", null);
        choiceListFunc.setSelectedIndex(parent.getFunctionKeyFeature(parent.SCENE_LISTFORM), true);
        append(choiceListFunc);

        // �X�y�[�X������
        append(new Spacer(7, 7));

//        choiceViewFunc = new ChoiceGroup("�Q�Ɖ�ʂO�L�[ ", Choice.POPUP);
        choiceViewFunc = new ChoiceGroup("�Q�Ɖ�ʂO�L�[�@�\", Choice.EXCLUSIVE);
        choiceViewFunc.append("�t�H���g�T�C�Y�ύX", null);
        choiceViewFunc.append("�����L�[�ړ����[�h", null);
        choiceViewFunc.append("�w���v�\�����[�h", null);
        choiceViewFunc.append("�X�����\��", null);
        choiceViewFunc.append("NG���[�h���o���[�h", null);        
        choiceViewFunc.append("���X�o��", null);
        choiceViewFunc.append("���[�h���j���[", null);
        choiceViewFunc.append("���X�܂Ƃߓǂ݃��[�h", null);
        choiceViewFunc.append("�N���A", null);
        choiceViewFunc.append("�������߂Q�����˂�", null);
        choiceViewFunc.setSelectedIndex(parent.getFunctionKeyFeature(parent.SCENE_VIEWFORM), true);
        append(choiceViewFunc);

        // �X�y�[�X������
        append(new Spacer(10, 10));

        // �I���O���[�v
        choiceOptFavor    = new ChoiceGroup("�I�v�V����", Choice.MULTIPLE);
        choiceOptFavor.append("�N�����ɂ��C�ɓ���ꗗ��\��", null);
        choiceOptFavor.append("�u���E�U�N�����a2B���p��", null);
        choiceOptFavor.append("���X���擾���ɂ��C�ɓ���o�^", null);
        choiceOptFavor.append("�_�E�����[�h�𒼃f�B���N�g����", null);
        choiceOptFavor.append("�X�������擾���[�h", null);
        choiceOptFavor.append("�X���^�C���ꗗ�����ɕ\��", null);
        choiceOptFavor.append("subject.txt�ۊ�", null);
        choiceOptFavor.append("���C�ɏ��ۊǉ񐔍팸", null);
        choiceOptFavor.append("�u���E�U�N����Google�ϊ����p", null);
        choiceOptFavor.append("�v���r���[���������摜��", null);
        choiceOptFavor.append("�擾�ς݃t�@�C��������Η��p", null);
        choiceOptFavor.append("�J�[�\���̓������y�[�W����", null);
        choiceOptFavor.append("���O��UTF8�����ϊ�(�v�ċN��)", null);
        append(choiceOptFavor);

        // �N�����ɂ��C�ɓ���ꗗ��\����true�̏ꍇ�ɂ́A��ʂɃ`�F�b�N�𔽉f������
        if (parent.getLaunchAsFavoriteList() == true)
        {
            choiceOptFavor.setSelectedIndex(0, true);
        }

        // Web�u���E�U�N�����a2B���p�����邩�ǂ������m�F���A��ʂɃ`�F�b�N�𔽉f������
        if (parent.getLaunchMode() == true)
        {
            choiceOptFavor.setSelectedIndex(1, true);
        }

        // ���X���擾���ɂ��C�ɓ���֎����o�^���邩�ǂ������m�F���A��ʂɃ`�F�b�N�𔽉f������
        if (parent.getAutoEntryFavorite() == true)
        {
            choiceOptFavor.setSelectedIndex(2, true);            
        }
        
        // �t�@�C���_�E�����[�h�́A���[�g�f�B���N�g���ɋL�^����
        if (parent.getSaveAtRootDirectory() == true)
        {
            choiceOptFavor.setSelectedIndex(3, true);
        }

        // �X�������擾���[�h��ݒ肷��
        if (parent.getDivideGetHttp() == true)
        {
            choiceOptFavor.setSelectedIndex(4, true);
        }

        // �X���^�C���ꗗ�����ɕ\��
        if (parent.getShowTitleAnotherArea() == true)
        {
            choiceOptFavor.setSelectedIndex(5, true);
        }

        // subject.txt��ۊǂ���...
        if (parent.getBackupSubjectTxt() == true)
        {
            choiceOptFavor.setSelectedIndex(6, true);
        }

        // ���C�ɓ���f�[�^�ۊǃ^�C�~���O�����炷...
        if (parent.getIgnoreFavoriteBackup() == true)
        {
            choiceOptFavor.setSelectedIndex(7, true);
        }

        // �u���E�U�N����google�v���L�V���g�p����
        if (parent.getUseGoogleProxy() == true)
        {
            choiceOptFavor.setSelectedIndex(8, true);
        }

        // �摜�擾���Agoogle�v���L�V���g�p����
        if (parent.getWX220JUserAgent() == true)
        {
            choiceOptFavor.setSelectedIndex(9, true);
        }

        // �摜�擾���A�擾�ς݃t�@�C���𗘗p����
        if (parent.getUseCachePictureFile() == true)
        {
            choiceOptFavor.setSelectedIndex(10, true);
        }

        // �J�[�\���̓������y�[�W�����
        if (parent.getCursorSingleMovingMode() == true)
        {
            choiceOptFavor.setSelectedIndex(11, true);
        }

        // ���O��UTF8�����ϊ�(�v�ċN��)
        if (parent.getForceKanjiConvertMode() == true)
        {
            choiceOptFavor.setSelectedIndex(12, true);
        }

        // ������I������ 
        String    time = "" + parent.getWatchDogTime();
        writeName = new TextField("������I������[��](�v�ċN��)",  time, 3, TextField.DECIMAL);
        append(writeName);

        // ���X�܂Ƃߓǂ݃��[�h
        String    cnt = "" + parent.getResBatchCount();
        writeCount = new TextField("���X�܂Ƃߓǂݎ��̃��X��",  cnt, 1, TextField.DECIMAL);
        append(writeCount);

        // AA�}�[�W�� 
        String    margin = "" + parent.getAAwidthMargin();
        aaWidth = new TextField("AA�\�����ړ���",  margin, 3, TextField.DECIMAL);
        append(aaWidth);

        // �L�[���s�[�g���o����(ms) 
        margin = "" + parent.getKeyRepeatTime();
        keyRepeatMs = new TextField("�L�[���s�[�g���o�Ԋu(ms)",  margin, 4, TextField.DECIMAL);
        append(keyRepeatMs);

        // ���[�U�[�G�[�W�F���g
        writeEmail = new TextField("User-Agent", parent.getUserAgent(false), 256, TextField.ANY);
        append(writeEmail);

        // �����擾�T�C�Y
        String divSize = "" + parent.getDivideGetSize();
        divideSize = new TextField("�����擾�T�C�Y", divSize, 6, TextField.DECIMAL);
        append(divideSize);
        
        // �t�H���g�T�C�Y�̎w��
        fontIdNumber = new TextField("Font(>=0�ōX�V)", "-1", 6, TextField.DECIMAL);
        append(fontIdNumber);

        // �X�y�[�X������
        append(new Spacer(10, 15));

        // OK�{�^��
        writeButton  = new StringItem("", "�ݒ�", Item.BUTTON);
        writeButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_BEFORE);
        writeButton.setDefaultCommand(new Command("�ݒ�", Command.SCREEN, CMD_SETPROXY));
        writeButton.setItemCommandListener(this);
        append(writeButton);
/**
        // �L�����Z���{�^��
        cancelButton = new StringItem("", "�L�����Z��", Item.BUTTON);
        cancelButton.setLayout(Item.LAYOUT_2|Item.LAYOUT_NEWLINE_AFTER);
        cancelButton.setDefaultCommand(new Command("CANCEL", Command.EXIT, CMD_EXIT));
        cancelButton.setItemCommandListener(this);
        append(cancelButton);
**/
        this.addCommand(fontCmd);
        
        return;
    }
    
    /**
     *  �m�F�_�C�A���O�̃R�}���h�I��������
     *
     */
    public class a2BConfirmationListener implements CommandListener
    {
        private a2BMessageWriteForm parent = null;
        
        /**
         *   �R���X�g���N�^
         * 
         */
        public a2BConfirmationListener(a2BMessageWriteForm object)
        {
            parent = object;
        }

        /**
         *  �R�}���h�{�^�����s���̏���
         *  
         */
        public void commandAction(Command c, Displayable d)
        {
            int num = c.getPriority();
            if (num == CMD_DOEXECUTE)
            {
                // �m�F���ꂽ�ꍇ...
                parent.confirm();
                return;
            }
            if (num == CMD_OPERATION)
            {
                return;
            }
            if ((num == CMD_DONE)||(num == CMD_EXIT)||(num == CMD_DOWRITE))
            {
                // �������񂾒l�𔽉f������
                parent.setWriteDetailMessage(num);
                return;
            }

            // �L�����Z�����ꂽ�ꍇ...
            parent.cancelConfirmation();
            return;
        }
    }
}
//--------------------------------------------------------------------------
//  a2BMessageWriteForm  --- ���X�������݃N���X
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
