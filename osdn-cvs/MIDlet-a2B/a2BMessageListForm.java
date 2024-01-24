import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import java.util.Date;
import java.util.Vector;

/**
 * �X���ꗗ��\���E���삷��N���X
 * 
 * @author MRSa
 *
 */
public class a2BMessageListForm extends Canvas implements CommandListener
{
    private a2BMain               parent          = null;
    private a2BbbsCommunicator    bbsCommunicator = null;
    private a2BsubjectDataParser  subjectParser   = null;
    private a2BFavoriteManager    favorManager    = null;

    private Command modeCmd   = new Command("���[�h",        Command.EXIT,    -1);
    private Command catCmd    = new Command("�J�e�S���I��",  Command.SCREEN,  50);
    private Command selCmd    = new Command("�I��",        Command.SCREEN,  50);
    private Command getCmd    = new Command("�ꗗ�擾 (x)",  Command.SCREEN,  50);
    private Command getAllCmd = new Command("�V���m�F/���~", Command.SCREEN,  50);
    private Command srchCmd   = new Command("�X������",      Command.SCREEN,  50);
    private Command opeCmd    = new Command("�X������",      Command.SCREEN,  50);
    private Command deleteCmd = new Command("�X���폜 (x)",  Command.SCREEN,  50);
    private Command proxyCmd  = new Command("a2B�ݒ�",       Command.SCREEN,  50);
    private Command quitCmd   = new Command("�I��",          Command.SCREEN,  50);

    private final byte SHOW_SUBJECTLIST    = 0;
    private final int  MARGIN              = 6;
    private final int  TO_PREVIOUS         = -1;
    private final int  TO_NEXT             = 1;
    private final int  TO_REDRAW           = 0;
    private final int  FORCE_REDRAW        = 2;

    static final int  GETLOGLIST_START_RECURSIVE = 0;
    static final int  GETLOGLIST_CONTINUE_RECURSIVE = 1;
    static final int  GETLOGLIST_START_THREAD = 2;

    static final int  OPTION_NOTSPECIFY     = -1;   // �w��Ȃ�
    static final int  OPTION_NOTIGNORE_CASE = 0;    // �啶��/����������ʂ���
    static final int  OPTION_IGNORE_CASE    = 1;    // �啶��/����������ʂ��Ȃ�
    static final int  OPTION_ONLYDATABASE   = 2;    // �X����񂩂�Ђ��ς��Ă���
    static final int  OPTION_RESETMODE      = 3;    // ���؂�ւ����[�h�����Z�b�g

    public final int GETMODE_GETPART       = 0;
    public final int GETMODE_GETALL        = 1;
    public final int GETMODE_PREVIEW       = 2;

    public final int PREVIEW_LENGTH        = 5120;    // �v���r���[���[�h�Ŏ擾����f�[�^��
    
    private String    gettingLogDataFile      = null;
    private String    gettingLogDataURL       = null;
    private String    gettingLogDataTitle     = null;
    private String    searchTitleString       = null;
    private String    temporaryTitleString    = null;
    private String    informationString       = null;
    private Vector    pickup2chList           = null;
    private boolean  searchIgnoreCase        = false;
    private int      searchTitlePickupHour   = -1;

    private Font      screenFont              = null;
    private int       screenHeight            = 0;
    private int       screenWidth             = 0;

    private int      screenLines             = 0;
    private int      displayIndexes[]        = null;

    private int      currentSelectedLine     = 0;
    private int      previousSelectedLine    = 0;
    private int      nofSubjectIndex         = 0;
    private int      listDrawMode            = TO_REDRAW;
    private int      listCount               = -1;
    private int      lastSubjectIndex        = 0;

    private int      currentSubjIndex        = 0;    // ���ݑ��쒆�̃X��ID
    private int      currentBbsIndex         = 0;    // ���ݕ\�����̃X����
    private int      currentFavoriteLevel    = -1;    // �X�����C�ɓ��背�x��
    
    private int      showInformationMode     = -1;
    
    private byte     currentScene             = SHOW_SUBJECTLIST;

    private final byte NO_SEARCHING_MODE     = a2BsubjectDataParser.STATUS_NOTSPECIFY; //
    private final byte SEARCHING_MODE        = a2BsubjectDataParser.STATUS_NOTYET;     //
    private final byte SEARCHING_ONLYDB      = a2BsubjectDataParser.STATUS_OVER;       //
    
    private byte     searchTitleMessageStatus = NO_SEARCHING_MODE;
    private boolean isHelpShowMode           = false;
    private boolean isNumberMovingMode       = false;
    private boolean isOpenImmediate          = true;
    private boolean isSubjectTxtUpdate       = false;
    private boolean isAbortNewArrival        = false;
    private boolean isFirstTime              = true;

    private int      newArrialCheckIndex      = 0;        // ���ݐV���m�F�����s���Ă���C���f�b�N�X�ԍ�
    private int      busyAreaSize             = 1;

    private Image     helpImage               = null;  // �w���v�p�C���[�W�t�@�C��
    private String    busyMessage             = null;  // ���b�Z�[�W�\��...

    /**
     * �R���X�g���N�^
     * @param arg0         �t�H�[���^�C�g��
     * @param object       �e�N���X
     * @param communicator �ꗗ���ێ��N���X
     * @param subjParser   �X���ꗗ���ێ��N���X
     * @param favor        ���C�ɓ���X�����ێ��N���X
     */
    public a2BMessageListForm(String arg0, a2BMain object, a2BbbsCommunicator communicator, a2BsubjectDataParser subjParser, a2BFavoriteManager favor)
    {
        // �^�C�g��
        setTitle(arg0);

        // �e�N���X
        parent = object;

        // �ꗗ�p�N���X
        bbsCommunicator = communicator;
        
        // �X���ꗗ�p��̓N���X
        subjectParser = subjParser;
        
        // ���C�ɓ���ꗗ�N���X
        favorManager = favor;

        // �R�}���h�̓o�^
        this.addCommand(modeCmd);
        this.addCommand(catCmd);
        this.addCommand(selCmd);
        this.addCommand(getCmd);
        this.addCommand(getAllCmd);
        this.addCommand(srchCmd);
        this.addCommand(opeCmd);
        this.addCommand(deleteCmd);
        this.addCommand(proxyCmd);
        this.addCommand(quitCmd);
        this.setCommandListener(this);

    }

    
    
    /**
     *    �t�H���g�Ɖ�ʃT�C�Y��ݒ肷��
     * 
     */
    public void prepareScreenSize()
    {
        // �t�H���g�Ɖ�ʃT�C�Y���擾����
        Font font = Font.getDefaultFont();
        font      = Font.getFont(font.getFace(), font.getStyle(), Font.SIZE_SMALL);
        int line = (screenHeight / (font.getHeight() + 1));

        // �\�����̃X��ID�i�[�̈������������
        displayIndexes = new int[line + MARGIN];
        for (int loop = 0; loop < (line + MARGIN); loop++)
        {
            displayIndexes[loop] = -1;
        }
        
        // �\���\�s��ݒ�...
        try
        {
            screenFont = Font.getFont(font.getFace(), font.getStyle(), parent.getListFontSize());
        }
        catch (Exception ex)
        {
            screenFont = Font.getFont(font.getFace(), font.getStyle(), Font.SIZE_SMALL);
        }
        setLineWidthHeight(getWidth(), getHeight());
        repaint();

        return;
    }
    
    /**
     *    �t�H���g�Ɖ�ʃT�C�Y��ݒ肷��
     * 
     */
/**
    public void setupScreenSize()
    {
        // �t�H���g�T�C�Y�𐳋K�Ȃ��̂ɖ߂��A�����\���\�s��ݒ肵�Ȃ���...
        try
        {
            screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), parent.getListFontSize());
        }
        catch (Exception ex)
        {
        }
        setLineWidthHeight(getWidth(), getHeight());

        // �\�����X�g���X�V����
        updateTitleList(FORCE_REDRAW);

        repaint();
        return;
    }
**/

    /**
     * �\���\�̈��ݒ肷��
     * @param w
     * @param h
     */
    private void setLineWidthHeight(int w, int h)
    {
        // �X���^�C�\���s���m�ۂ���
        if (parent.getShowTitleAnotherArea() == true)
        {
            busyAreaSize = 2;
        }
        else
        {
            busyAreaSize = 1;
        }
        screenWidth  = w;
        screenHeight = h - screenFont.getHeight() * busyAreaSize;
        screenLines  = (screenHeight / (screenFont.getHeight() + 1));
        return;
    }

    /**
     *   ��ʕ`��̎��{...
     * 
     * 
     */
    public void paint(Graphics g)
    {
        // �I�u�W�F�N�g���\�z����Ă��Ȃ�...�ĕ`�悹���ɏI������
        if ((bbsCommunicator == null)||(subjectParser == null)||(parent == null))
        {
            return;
        }

        // WX310�p���[�N�A���E���h...
        if (isFirstTime == true)
        {
            isFirstTime = false;

            // �\���\�s��ݒ肷��
            setLineWidthHeight(getWidth(), getHeight());        
        }

        // �\�����[�h���m�F�A�ꗗ�\����؂�ւ���
        if (currentFavoriteLevel < 0)
        {
            // �ʏ�\�����[�h�̉�ʕ`��
            showList(g);
        }
        else
        {
            // ���C�ɓ���\�����[�h�̉�ʕ`��
            showListFavorite(g);
        }
        
        // �w���v�\�����[�h���H
        if ((isHelpShowMode == true)&&(helpImage != null))
        {
            // �w���v�\�����s��
            showHelpImage(g);
        }

        // �����L�[���̓��[�h..
        if (isNumberMovingMode == true)
        {
            g.setColor(0x00ff00ff);
            g.fillRect(0, 0, 2, 12);
            g.setColor(0);
        }

        // �C���t�H���[�V�����\�����[�h
        if ((showInformationMode >= 0)&&(informationString != null))
        {
            int y = screenHeight / 2;
            if (showInformationMode == 0)
            {
                g.setColor(0x00ffffcc);
            }
            else
            {
                g.setColor(0x00ffcccc);
            }
            g.fillRect(10, (y - 3), (screenWidth - 20), (screenFont.getHeight() + 6));
            g.setColor(0);
            g.drawRect(10, (y - 3), (screenWidth - 20), (screenFont.getHeight() + 6));
            g.drawString(informationString, 20, y, (Graphics.LEFT | Graphics.TOP));
        }

        // BUSY���b�Z�[�W�̕\��
        {
            int heightTop = screenLines * (screenFont.getHeight() + 1);
//            int heightTop = screenLines * (screenFont.getHeight() + 2);
            g.setColor(0x00d8ffd8);
            g.fillRect(0, heightTop, getWidth(), getHeight() - heightTop);
//            g.fillRect(0, screenHeight + 1, getWidth(), getHeight() - (screenHeight + 1));
            if (busyMessage != null)
            {
                if (busyAreaSize == 1)
                {
                    // �P�s�\�����[�h�̏ꍇ...
                    g.setColor(0x00);    // ��
                    g.drawString(busyMessage, 0, (screenHeight + 1), (Graphics.LEFT | Graphics.TOP));
                }
                else
                {
                    // �����s�\�����[�h�̏ꍇ...
                    drawBusyAreaMultiple(g, heightTop);
                }
            }
        }

        return;
    }

    /**
     * �w���v�̕\�����[�h���C��
     * 
     * @param g
     */
    private void showHelpImage(Graphics g)
    {
        // �C���[�W�̕\�����W�����߂�
        int x = ((screenWidth  - helpImage.getWidth())  / 2);
        if ((x + helpImage.getWidth()) > screenWidth)
        {
            x = 0;
        }

        int y = ((screenHeight - helpImage.getHeight()) / 2);
        if ((y + helpImage.getHeight()) > screenHeight)
        {
            y = 0;
        }
        
        // �w���v�C���[�W��`��
        g.setColor(0);
        g.drawImage(helpImage, x, y, (Graphics.LEFT | Graphics.TOP));

        return;
    }

    /**
     *  �X�����샂�[�h�̓���R�}���h�����s����
     * 
     * @param index
     */    
    private void executeSubjectOperation(int index)
    {
        // �X����Ԃ��擾����
        int  status  = subjectParser.getStatus(currentSubjIndex);

        // �X�����샂�[�h...
        if (index == 0)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // �Ď擾...(�X���S�擾�����{)
            getLogDataFile(currentSubjIndex, GETMODE_GETALL);
            return;
        }
        else if (index == 1)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // �v���r���[...
            getLogDataFile(currentSubjIndex, GETMODE_PREVIEW);
            return;
        }
        else if (index == 2)
        {
            // �擾�\��...(����V���m�F���ɃX���S�擾�����{����)
            if (status == a2BsubjectDataParser.STATUS_NOTYET)
            {
                // �X�����擾�̏ꍇ�ɂ͎擾��Ԃ̐ݒ������
                subjectParser.setCurrentMessage(currentSubjIndex, -1, 0);
                //subjectParser.setGetLogStatus(currentSubjIndex, subjectParser.STATUS_RESERVE);            
                return;
            }
            return;
        }
        if (index == 3)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            if ((status == a2BsubjectDataParser.STATUS_NOTYET)||(status == a2BsubjectDataParser.STATUS_RESERVE))
            {
                // �X�����擾(�X���S�擾�����{)
                getLogDataFile(currentSubjIndex, GETMODE_GETALL);
                return;
            }

            // �����擾...
            getLogDataFile(currentSubjIndex, GETMODE_GETPART);
            return;
        }
        else if (index == 4)
        {
            // �X���Q��... (�e�ɑ΂��A��ʂ𔲂��ăX����\�����邱�Ƃ�ʒm����)
            if ((status == a2BsubjectDataParser.STATUS_NOTYET)||(status == a2BsubjectDataParser.STATUS_RESERVE))
            {
                // �X�����擾�Ȃ̂ŁA�������Ȃ�...
                return;
            }
            parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, subjectParser.getCurrentMessage(currentSubjIndex));
            gettingLogDataFile = null;
            return;
        }
        else if (index == 5)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // ���C�ɓ���o�^...
            Thread thread = new Thread()
            {
                public void run()
                {
                    // ���C�ɓ���̓o�^/�o�^���������{����
                    setResetFavoriteThread(currentSubjIndex, true);
                }
            };
            thread.start();
            return;
        }
        else if (index == 6)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // ���C�ɓ���o�^����...
            Thread thread = new Thread()
            {
                public void run()
                {
                    // ���C�ɓ���̓o�^/�o�^���������{����
                    setResetFavoriteThread(currentSubjIndex, false);
                }
            };
            thread.start();
            return;
        }
        else if (index == 7)
        {
            // �u���E�U�ŊJ��...
            launchWebBrowserCurrentSelectedIndex();
        }
        else if (index == 8)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }

            // �擾��URL�𐶐����A�X���^�C�g���ƘA������
            String boardUrl = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
            int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
            String nick = boardUrl.substring(pos + 1);
            String url  = boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + subjectParser.getThreadNumber(currentSubjIndex) + "/";
            showRelatedKeyword(url);
        }
        return;
    }
    
    /**
     *  ���C�ɓ���ꗗ�\�����s���Ă���Ƃ��A�X�����샂�[�h�̓���R�}���h�����s����
     * 
     * @param index
     */    
    private void executeSubjectOperationFavorite(int index)
    {
        // �X����Ԃ��擾����
        byte  status  = favorManager.getStatus(currentSubjIndex);

        // �X�����샂�[�h...
        if (index == 0)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // �Ď擾...(�X���S�擾�����{)
            getLogDataFileFavorite(currentSubjIndex, GETMODE_GETALL);
            return;
        }
        else if (index == 1)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            if ((status == favorManager.STATUS_NOTYET)||(status == favorManager.STATUS_RESERVE))
            {
                // �X�����擾(�X���S�擾�����{)
                getLogDataFileFavorite(currentSubjIndex, GETMODE_GETALL);
                return;
            }

            // �����擾...
            getLogDataFileFavorite(currentSubjIndex, GETMODE_GETPART);
            return;
        }
        else if (index == 2)
        {
            // �X���Q��... (�e�ɑ΂��A��ʂ𔲂��ăX����\�����邱�Ƃ�ʒm����)
            if ((status == favorManager.STATUS_NOTYET)||(status == favorManager.STATUS_RESERVE))
            {
                // �X�����擾�Ȃ̂ŁA�������Ȃ�...
                return;
            }
            parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, favorManager.getCurrentMessage(currentSubjIndex));
            gettingLogDataFile = null;
            return;
        }
        else if (index == 3)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // ���C�ɓ���o�^����...
            Thread thread = new Thread()
            {
                public void run()
                {
                    // ���C�ɓ���̓o�^/�o�^���������{����
                    setResetFavoriteThread(currentSubjIndex, false);
                }
            };
            thread.start();
            return;
        }
        else if (index == 4)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // �\�����Ă���ꗗ��؂�ւ���
            Thread thread = new Thread()
            {
                public void run()
                {
                    // ��؂�ւ��鏀�������s����...
                    subjectParser.leaveSubjectList(parent.getCurrentBbsIndex());

                    // ��URL����������
                    String boardNick = favorManager.getBoardNick(currentSubjIndex);
                    parent.moveToBoard(boardNick);

                    // BUSY�\�����폜����
                    showInformationMode = -1;
                    informationString = null;
                    repaint();
                }
            };

            // BUSY�\������...
            showInformationMode = 0;
            informationString = "�ꗗ�؂�ւ���...";
            repaint();
            
            // �ꗗ�؂�ւ������̎��s...
            thread.start();
            return;
        }
        else if (index == 5)
        {
            // �u���E�U�ŊJ�� �A�N�V����
            launchWebBrowserCurrentSelectedIndex();
            return;
        }
        else if (index == 6)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // �֘A�L�[���[�h�\��...
            String boardUrl = favorManager.getUrl(currentSubjIndex);
            int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
            String nick = boardUrl.substring(pos + 1);
            String url  = boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + favorManager.getThreadNumber(currentSubjIndex) + "/";
            showRelatedKeyword(url);
        }
        return;
    }

    
    /**
     * BUSY�\�����s��...
     * 
     * @param message
     */
    public void setInformationMessage(int mode, String message)
    {
        // BUSY�\������...
        showInformationMode = mode;
        informationString   = message;
        repaint();
        return;
    }
    
    /**
     *  �I�����ꂽ�R�}���h�̎��s
     * 
     * @param scene
     * @param index
     */
    public void executeCommandAction(byte scene, int index, int appendInfo)
    {
        // �X���ꗗ�\�����[�h
        currentScene = SHOW_SUBJECTLIST;

        if (scene == parent.SHOW_SUBJECTCOMMAND)
        {
            // �X�����샂�[�h�̎��s����
            executeSubjectOperation(index);
        }
        else if (scene == parent.SHOW_SUBJECTCOMMANDFAVORITE)
        {
            // �X�����샂�[�h�̎��s���� (���C�ɓ��胂�[�h�̂Ƃ�)
            executeSubjectOperationFavorite(index);
        }
        else if (scene == parent.CONFIRM_DELETELOGDAT)
        {
            // �X���폜���[�h
            if (index == 1)
            {
                // ���C�ɓ���̓o�^/�o�^���������{����
                setResetFavoriteThread(currentSubjIndex, false);

                // �X���폜�̎��s
                if (currentFavoriteLevel >= 0)
                {
                    // ���C�ɓ���...�����폜
                    subjectParser.deleteLogDataFile(-1, gettingLogDataFile);
                }
                else
                {
                    subjectParser.deleteLogDataFile(currentSubjIndex, gettingLogDataFile);
                }
                gettingLogDataFile = null;
                repaint();
/*
                // �X���폜�̎��s���C��...
                Thread thread = new Thread()
                {
                    public void run()
                    {
                        // ���C�ɓ���̓o�^/�o�^���������{����
                        setResetFavoriteThread(currentSubjIndex, false);

                        // �X���폜�̎��s
                        if (currentFavoriteLevel >= 0)
                        {
                            // ���C�ɓ���...�����폜
                            subjectParser.deleteLogDataFile(-1, gettingLogDataFile);
                        }
                        else
                        {
                            subjectParser.deleteLogDataFile(currentSubjIndex, gettingLogDataFile);
                        }
                        gettingLogDataFile = null;
                        repaint();
                    }
                };
                thread.start();                
*/
            }
        }
        else if (scene == parent.CONFIRM_SELECT_FIND2CH)
        {
            // 2ch����...
            if (index >= 0)
            {
                // �Q�����˂�X���b�h�̕\��...
                String buffer = (String) pickup2chList.elementAt(index);
                int pos = buffer.indexOf("<>");
                if (pos >= 0)
                {
                    // �X���ԍ�������؂�o��..
                    pos = pos + 2;
                    String reservedThreadNumber = (String) buffer.substring(pos);

                    // ���͂��ꂽ�X���ԍ��̉�͂��˗�����
                    parent.parseMessageThreadNumber(reservedThreadNumber, appendInfo);
                }
            }
            pickup2chList = null;
            System.gc();
        }
        else if (scene == parent.CONFIRM_SELECT_RELATE_KEYWORD)
        {
            // �֘A�L�[���[�h�̑I��...
            if (index >= 0)
            {
                // �֘A�L�[���[�h�𗘗p���āA�Q�����˂錟�������s����...
                String buffer = (String) pickup2chList.elementAt(index);
                int pos = buffer.indexOf("<>");
                if (pos >= 0)
                {
                    // URL������؂�o��...
                    pos = pos + 2;
                    String str = (String) buffer.substring(pos);
                    gettingLogDataURL = "http://find.2ch.net/?BBS=ALL&TYPE=TITLE&ENCODING=SJIS&" + str + "&COUNT=25";
                    find2chThreadTitles();
                }
            }
            pickup2chList = null;
            System.gc();
        }
        return;
    }
    
    /**
     * �A�C�e�����I�����ꂽ�Ƃ��̏���
     * 
     * @param autoOpen
     */
    public void selectedItem(boolean autoOpen)
    {
        int index = getSelectedIndex();
        if (index < 0)
        {
            // �C���f�b�N�X�ԍ������������A�A�A�I������
            return;
        }

        // �I�����ꂽ�T�u�W�F�N�g�C���f�b�N�X���L������
        parent.setSelectedSubjectIndex(index);
            
        if (currentScene != SHOW_SUBJECTLIST)
        {
            // �X���ꗗ�\�������Ă��Ȃ������A�A�I������
            return;
        }
        // �\�����[�h���m�F�A�ꗗ�\����؂�ւ���
        if (currentFavoriteLevel < 0)
        {
            // �X���I�����[�h�������A�X���I���R�}���h��\������
            showActionChoiceGroup(index, autoOpen);
        }
        else
        {
            // ���C�ɓ��胂�[�h�̂Ƃ��A�X���I�����[�h�������A�X���I���R�}���h��\������
            showActionChoiceGroupFavorite(index, autoOpen);            
        }
        return;
    }

    /**
     *  ���C�ɓ���ꗗ:�ʏ�ꗗ�̐؂�ւ�
     * 
     *
     */
    private void toggleFavoriteList()
    {
        if (parent.isGetNewArrival() == true)
        {
            // �V���m�F���͋֎~����
            return;
        }
        // ���C�ɓ���ꗗ/�ʏ�\�����[�h�̐؂�ւ����s��...
        parent.toggleFavoriteShowMode();
        currentFavoriteLevel = parent.getFavoriteShowLevel();

/**
        // �ꗗ��؂�ւ����ꍇ�ɂ́A�X���^�C������������N���A����
        clearSearchTitleString();
**/

        // �J�[�\����擪�ֈړ�������
        nofSubjectIndex     = 0;
        currentSelectedLine = 0;
        listCount           = 0;
        listDrawMode        = FORCE_REDRAW;

        // �\�����X�g���X�V����
        updateTitleList(FORCE_REDRAW);

        return;
    }

    /**
     *  ���������W����������...
     * 
     * 
     */
    public void pointerPressed(int x, int y)
    {
        // �����L�[�������Ɖ�ʂ��N���A����
        boolean clearInfo = false;
        if (showInformationMode == 0)
        {
            showInformationMode = -1;
            informationString = null;
            repaint();
        }
        if (showInformationMode == 1)
        {
            // �������͂�҂�...
            return;
        }

        int zoneWidth = (screenFont.charWidth('��') * 3);
        int index = y / (screenFont.getHeight() + 1);
        if (x > (getWidth() - zoneWidth))
        {
            // ��ʂ̉E��[�A�E���[���^�b�v�����ꍇ...
            if (index == 0)
            {
                // �O�y�[�W�Ɉړ�����
                listDrawMode = TO_PREVIOUS;
                repaint();
                return;
            }
            else if (index >= (listCount - 2))
            {
                // ���y�[�W�Ɉړ�����
                listDrawMode = TO_NEXT;
                repaint();
                return;
            }
        }
        if (x < zoneWidth)
        {
            // ��ʂ̍���[�A�����[���^�b�v�����ꍇ...
            if (index == 0)
            {
                // ����[�A���C�ɓ���ꗗ�Ƃ̐؂�ւ�...
                toggleFavoriteList();
                repaint();
                return;
            }
            else if (index >= (listCount - 2))
            {
                // �����[�A�@�\�̎��s
                executeFunctionFeature();
                repaint();
                return;
            }
        }
        
        if (currentSelectedLine == index)
        {
            // �X���I���A�A�A���X��ǂ�...
            selectedItem(true);            
        }
        else
        {
            previousSelectedLine = currentSelectedLine;
            currentSelectedLine  = index;
        }
        
        if (clearInfo == true)
        {
            // ��ʍĕ`��
            listDrawMode        = FORCE_REDRAW;

            // �\�����X�g���X�V���A�ĕ`�悷��
            updateTitleList(FORCE_REDRAW);
        }
        repaint();
        return;
    }

    /**
     *  ���������W����������...
     * 
     * 
     */
    public void pointerReleased(int x, int y)
    {
        return;
    }

    /**
     *  ��ʃT�C�Y�ύX���ɌĂ΂�鏈��...
     * 
     */
    public void sizeChanged(int w, int h)
    {
        int topIndex  = -1;
        int bottomIndex = -1;

        if (displayIndexes != null)
        {
            topIndex = displayIndexes[0];
            if (listCount > 0)
            {
                bottomIndex = displayIndexes[listCount - 1];
            }
            if (topIndex > bottomIndex)
            {
                int tempIndex = topIndex;
                topIndex = bottomIndex;
                bottomIndex = tempIndex;
            }
        }

        displayIndexes = null;
        listCount      = -1;
        setLineWidthHeight(w, h);
        prepareScreenSize();
        displayIndexes[0] = topIndex;
        displayIndexes[screenLines - 1] = bottomIndex;
        listCount = screenLines;
        updateTitleList(FORCE_REDRAW);

        System.gc();
        repaint();

        return;
    }
    
    
    /**
     *   ���I�������ꍇ...
     */
    private void inputUp(int count)
    {
        previousSelectedLine = currentSelectedLine;
        currentSelectedLine = currentSelectedLine - count;
        if (currentSelectedLine < 0)
        {
            if (parent.getCursorSingleMovingMode() == true)
            {
                if (displayIndexes[0] != 0)
                {
                    currentSelectedLine = listCount - 2;
                }
                if (currentSelectedLine < 0)
                {
                    currentSelectedLine = 0;
                }
                listDrawMode = TO_PREVIOUS;
            }
            else
            {
                currentSelectedLine = (listCount - 1);
            }
        }
        return;
    }
    
    /**
     *  ����I�������ꍇ...
     *  
     * @param count
     */
    private void inputDown(int count)
    {
        previousSelectedLine = currentSelectedLine;
        currentSelectedLine = currentSelectedLine + count;
        if (currentSelectedLine > (listCount - 1))
        {
            if (parent.getCursorSingleMovingMode() == true)
            {
                listDrawMode = TO_NEXT;
                currentSelectedLine = 1;
            }
            else
            {
                currentSelectedLine = 0;                
            }
        }
        return;
    }
    

    private void eraseInformationMessage(int keyCode)
    {
        // 0�L�[���������ꍇ�ɂ́A�o�C�u���[�V������OFF�ɂ���
        if (keyCode == Canvas.KEY_NUM0)
        {
            parent.setVibrateDuration(0);
        }

        // *�L�[���������ꍇ�ɂ́A�o�C�u���[�V������ON(300ms)�ɂ���
        if (keyCode == Canvas.KEY_STAR)
        {
            parent.setVibrateDuration(300);
        }

        // #�L�[���������ꍇ�ɂ́A�o�C�u���[�V������ON(1500ms)�ɂ���
        if (keyCode == Canvas.KEY_POUND)
        {
            parent.setVibrateDuration(1500);
        }

        // �����L�[�������Ɖ�ʂ��N���A����
        showInformationMode = -1;
        informationString = null;

        // readySubjectList();  // �V���m�F�I�����ɃX���ꗗ������������ꍇ...
        
        // ��ʍĕ`��
        listDrawMode        = FORCE_REDRAW;

        // �\�����X�g���X�V���A�ĕ`�悷��
        updateTitleList(FORCE_REDRAW);
        repaint();
        
        return;        
    }
   
    /**
     *  �Q�[���L�[�̃A�N�V�����w��B�B�B
     * 
     * @param keyCode
     */
    private boolean gameKeyAction(int keyCode)
    {
        boolean repaint   = true;
        boolean processed = true;
        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            // �O�y�[�W�Ɉړ�����
            listDrawMode = TO_PREVIOUS;
            break;
              
          case Canvas.RIGHT:
            // ���y�[�W�Ɉړ�����
            listDrawMode = TO_NEXT;
            break;

          case Canvas.DOWN:
            // �J�[�\�����ЂƂ��Ɉړ�������
            inputDown(1);
            break;

          case Canvas.UP:
            // �J�[�\�����ЂƂ�Ɉړ�������
            inputUp(1);
            break;

          case Canvas.FIRE:
            // �X���I���A�A�A���X��ǂ�...
            selectedItem(true);
            break;

          case Canvas.GAME_A:
            // WX310SA : �����{�^��
            // 3���I������
            inputUp(3);
            break;

          case Canvas.GAME_B:
            // WX310SA : My�L�[
            // 3����\������
            inputDown(3);
            break;

          case Canvas.GAME_C:
            // WX310SA : �N���A�L�[
            // �������[�h�A�w���v�\�����[�h����������
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_RESETMODE, -1);
            if (isHelpShowMode == true)
            {
                changeHelpShowMode();
                repaint = false;                
            }
            break;

          case Canvas.GAME_D:
            // WX310SA : �T�C�h�L�[
            // �u���E�U�ŊJ��
            launchWebBrowserCurrentSelectedIndex();
            break;

          default:
            repaint = false;
            processed = false;
            break;
        }
        if (repaint == true)
        {
            repaint();
        }            
        return (processed);
    }

    private boolean numberKeyMoveAction(int keyCode)
    {
        boolean repaint = true;
        boolean processed = true;
        
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            // �@�\�̎��s
            executeFunctionFeature();
            break;

          case Canvas.KEY_NUM1:
            // ���C�ɓ���ꗗ�F�ʏ�ꗗ�̐؂�ւ����s��
            toggleFavoriteList();
            break;

          case Canvas.KEY_NUM2:
            // �J�[�\�����ЂƂ�Ɉړ�������
            inputUp(1);
            break;

          case Canvas.KEY_NUM3:
            if (parent.isGetNewArrival() == false)
            {
                // �̑I��... (�������A�V���m�F���͓��삵�Ȃ�)
                selectBoard();  
            }
            break;

          case 112:
          case 80:
               // p, P�L�[
          case Canvas.KEY_NUM4:
            // �O�y�[�W�Ɉړ�����
            listDrawMode = TO_PREVIOUS;
            break;

          case Canvas.KEY_NUM5:
            // �X���I���A�A�A���X��ǂ�...
            selectedItem(true);
            break;

          case 110:
          case 78:
              // n,N�L�[
          case Canvas.KEY_NUM6:
            // ���y�[�W�Ɉړ�����
            listDrawMode = TO_NEXT;
            break;

          case Canvas.KEY_NUM7:
            if (parent.isGetNewArrival() == false)
            {
                // �X���擾�����s���� (�V���m�F���łȂ��ꍇ...)
                getLogDataFileImmediate();
            }
            break;

          case Canvas.KEY_NUM8:
            // �J�[�\�����ЂƂ��Ɉړ�������
            inputDown(1);
            break;

          case Canvas.KEY_NUM9:
            // �V���m�F�̎��{...
            parent.startGetLogList();
            break;

          case 46:
          case Canvas.KEY_POUND:
            // . #�}�[�N(������)
            if (nofSubjectIndex > 0)
            {
                // ������薖���ֈړ�������...
                listDrawMode      = TO_PREVIOUS;
                listCount         = 1;
                displayIndexes[0] = (nofSubjectIndex - 1);
            }
            break;

          case 44:
          case Canvas.KEY_STAR:
            // , *�}�[�N (�擪��)
            listCount = 0;
            listDrawMode = FORCE_REDRAW;
            break;

          case 111:
          case  79:
            // �X�����샂�[�h��\������
            selectedItem(false);
            break;

          case 119:
          case  87:
            // �u���E�U�ŊJ�� (W)
            launchWebBrowserCurrentSelectedIndex();
            break;

          case -5:
            // �X���I���A�A�A���X��ǂ�...
            selectedItem(true);
            break;

          case -50:
            // �w���v�C���[�W��\������ (Nokia)
            changeHelpShowMode();
            break;

          case 8:
            // BS�L�[...
            if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // ���́Afire�L�[�̓��͂Ɣ��f�A�A�A�����ł͉������Ȃ�
                return (false);
            }
            //not break!!
          case 27:
          case 113:
          case 81:
          case -8:
            // �N���A�L�[ for Nokia ... (�������킹...)
            // BS�L�[�AESC�L�[�AQ�L�[
            // �������[�h�A�w���v�\�����[�h����������
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_RESETMODE, -1);
            if (isHelpShowMode == true)
            {
                changeHelpShowMode();
                repaint = false;
            }
            break;

          default:
            repaint = false;
            processed = false;
            break;
        }
        if (repaint == true)
        {
            repaint();
        }
        return (processed);
    }
    
    private boolean numberKeyNormalAction(int keyCode)
    {
        Thread thread = null;
        boolean repaint   = true;
        boolean processed = true;
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            // �@�\�̎��s
            executeFunctionFeature();
            break;

          case Canvas.KEY_NUM1:
            // ���C�ɓ���ꗗ/�ʏ�\�����[�h�̐؂�ւ����s��
            toggleFavoriteList();
            break;

          case Canvas.KEY_NUM2:
            if (parent.isGetNewArrival() == false)
            {
                // �J�e�S���̑I��...(�V���m�F���͓��삵�Ȃ�)
                selectCategory();            
            }
            break;

          case Canvas.KEY_NUM3:
            if (parent.isGetNewArrival() == false)
            {
                // �̑I��...(�V���m�F���͋֎~����)
                selectBoard();            
            }
            break;

          case Canvas.KEY_NUM4:
            if (parent.isGetNewArrival() == false)
            {
                // �X�����̕\���i�V���m�F���͋֎~����j
                showMessageInformation();
            }
            break;

          case Canvas.KEY_NUM5:
            if (parent.isGetNewArrival() == false)
            {
                // �X������ (�V���m�F���͋֎~����)
                startSearchTitle();
            }
            break;

          case Canvas.KEY_NUM6:
            if (currentFavoriteLevel < 0)
            {
                // �������[�h�̐؂�ւ����s��
                // (���C�ɓ���\�����[�h�Ȃ�A���̃R�}���h�͎󂯕t���Ȃ�)
                setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_ONLYDATABASE, -1);
            }
            break;

          case Canvas.KEY_NUM7:
           if (parent.isGetNewArrival() == false)
            {
                // �X���擾�����s����(�V���m�F���͋֎~����)
                getLogDataFileImmediate();
            }
            break;

          case Canvas.KEY_NUM8:
            if ((parent.isGetNewArrival() == false)&&(currentFavoriteLevel < 0))
            {
                // �V���m�F���łȂ��A���C�ɓ���\�����[�h�ł��Ȃ��Ƃ��A�X���ꗗ���擾����B
                thread = new Thread()
                {
                    public void run()
                    {
                        // �X���ꗗ�̎擾�����{����
                        subjectParser.prepareSubjectListUpdate(parent.getCurrentBbsIndex());
                    }
                };
                setBusyMessage("subject.txt�擾��...", true);
                thread.start();
                parent.setSelectedSubjectIndex(0);
            }
            break;

          case Canvas.KEY_NUM9:
            // �V���m�F�̎��{...
            parent.startGetLogList();
            break;
                
          case 46:
          case Canvas.KEY_POUND:
            // #�}�[�N(������)
            if (nofSubjectIndex > 0)
            {
                // ������薖���ֈړ�������...
                listDrawMode      = TO_PREVIOUS;
                listCount         = 1;
                displayIndexes[0] = (nofSubjectIndex - 1);
            }
            break;

          case 44:
          case Canvas.KEY_STAR:
            // *�}�[�N (�擪��)
            listCount = 0;
            listDrawMode = FORCE_REDRAW;
            break;

          case 111:
          case  79:
            // �X�����샂�[�h��\������
            selectedItem(false);
            break;

          case -50:
            // �w���v�C���[�W��\������ (Nokia)
            changeHelpShowMode();
            break;

          case 112:
          case 80:
            // p, P�L�[
            // �O�y�[�W�Ɉړ�����
            listDrawMode = TO_PREVIOUS;
            break;

          case 110:
          case 78:
            // n,N�L�[
            // ���y�[�W�Ɉړ�����
            listDrawMode = TO_NEXT;
            break;

          case -5:
            // �X���I���A�A�A���X��ǂ�...
            selectedItem(true);
            break;

          case 119:
          case  87:
            // �u���E�U�ŊJ�� (W)
            launchWebBrowserCurrentSelectedIndex();
            break;

          case 8:
            // BS�L�[
              if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // ���́Afire�L�[�̓��͂Ɣ��f�A�A�A�����ł͉������Ȃ�
                return (false);
            }
            //not break!!
          case 27:
          case 113:
          case 81:
          case -8:
            // �N���A�L�[ for Nokia ... (�������킹...)
            // ESC�L�[�AQ�L�[
            // �������[�h�A�w���v�\�����[�h����������
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_RESETMODE, -1);
            if (isHelpShowMode == true)
            {
                changeHelpShowMode();
                repaint = false;
            }
            break;

          default:
            repaint   = false;
            processed = false;
            break;
        }
        if (repaint == true)
        {
            repaint();
        }
        return (processed);        
    }
    
    
    
    /**
     *   �L�[���͂̏���
     * 
     * 
     */
    public void keyPressed(int keyCode)
    {
        parent.keepWatchDog(0);
        
        if (showInformationMode == 0)
        {
            // �V���m�F�̒���ɃL�[���͂����Ƃ�...
            // (�\������������)
            eraseInformationMessage(keyCode);
            return;
        }
        if (showInformationMode == 1)
        {
            // ���C�ɔԍ��̓��͑҂�...
            showInformationMode = -1;
            informationString = null;
            if ((keyCode >= Canvas.KEY_NUM0)&&(keyCode <= Canvas.KEY_NUM9))
            {
                // ���C�ɔԍ����X�V����
                currentFavoriteLevel = keyCode - Canvas.KEY_NUM0;
            }
            // �\�����X�g���X�V���A�ĕ`�悷��
            updateTitleList(FORCE_REDRAW);
            repaint();
            return;
        }
        
        if (keyCode == 0)
        {
            // �L�[���͂��Ȃ�����...�I������
            return;
        }

        boolean processed = false;
        if (isNumberMovingMode == true)
        {
            processed = numberKeyMoveAction(keyCode);
        }
        else
        {
            processed = numberKeyNormalAction(keyCode);
        }
        if (processed == false)
        {
            gameKeyAction(keyCode);
        }        
        return;
    }

    /**
     *   �L�[���s�[�g���Ă���Ƃ�...
     * 
     * 
     */
    public void keyRepeated(int keyCode)
    {
        parent.keepWatchDog(0);

        boolean repaint = true;
        if (keyCode == 0)
        {
            return;
        }

        if (isNumberMovingMode == true)
        {
            repaint = false;
            switch (keyCode)
            {
              case Canvas.KEY_NUM2:
                // �J�[�\�����ЂƂ�Ɉړ�������
                inputUp(1);
                break;

              case Canvas.KEY_NUM4:
                // �O�y�[�W�Ɉړ�����
                listDrawMode = TO_PREVIOUS;
                break;

              case Canvas.KEY_NUM6:
                // ���y�[�W�Ɉړ�����
                listDrawMode = TO_NEXT;
                break;

              case Canvas.KEY_NUM8:
                // �J�[�\�����ЂƂ��Ɉړ�������
                inputDown(1);
                break;

              default:
                repaint = true;
                break;
            }
            if (repaint != true)
            {
                repaint();
                return;
            }
        }

        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            // �O�y�[�W�Ɉړ�����
            listDrawMode = TO_PREVIOUS;
            break;
              
          case Canvas.RIGHT:
            // ���y�[�W�Ɉړ�����
            listDrawMode = TO_NEXT;
            break;

          case Canvas.DOWN:
            // �J�[�\�����ЂƂ��Ɉړ�������
            inputDown(1);
            break;

          case Canvas.UP:
            // �J�[�\�����ЂƂ�Ɉړ�������
            inputUp(1);
            break;

          case Canvas.FIRE:
            // �Z���N�g�{�^��
            break;

          case Canvas.GAME_A:
            // WX310SA : �����{�^��
            // 3���I������
            inputUp(3);
            break;

          case Canvas.GAME_B:
            // WX310SA : My�L�[
            // 3����\������
            inputDown(3);
            break;

          case Canvas.GAME_C:
            // WX310SA : �N���A�L�[
            break;

          case Canvas.GAME_D:
            break;

          default:
            break;
        }        
        if (repaint == true)
        {
            repaint();
        }
        return;
    }
    
    
    /**
     *  �X�������̎��s...
     *
     */
    private void startSearchTitle()
    {
        int index = getSelectedIndex();
        if ((index >= 0)&&((temporaryTitleString == null)||(temporaryTitleString.length() == 0)))
        {
            // ���ݑI�𒆂̃X���^�C�g���𔽉f������i�\�����[�h���m�F�j
            if (currentFavoriteLevel < 0)
            {
                // �X���I�����[�h������
                temporaryTitleString = trimTitleString(subjectParser.getSubjectName(index));
            }
            else
            {
                // ���C�ɓ��胂�[�h�̂Ƃ�
                temporaryTitleString = trimTitleString(favorManager.getSubjectName(index));            
            }
        }
        parent.OpenWriteFormUsingWordInputMode(parent.SCENE_TITLE_SEARCH, "�X���^�C�g������", "�^�C�g��", temporaryTitleString, true);
    }

    /**
     *  
     * @param data �X���^�C�g��
     * @return �i���H�ς݁j�^�C�g��������
     */
    private String trimTitleString(String data)
    {
        if (data == null)
        {
            return (null);
        }
       
        int pos1 = data.indexOf("�y");
        int pos2 = data.indexOf("�z");
        String cutData = data;
        String andData = "";
        if (pos1 >= 0)
        {
            andData = cutData.substring(0, pos1);
        }
        try
        {
            while ((pos1 >= 0)&&(pos2 >= 0)&&(pos2 > pos1))
            {
                cutData = cutData.substring(pos2 + 1);
                pos1 = cutData.indexOf("�y");
                if (pos1 >= pos2)
                {
                    andData = andData + cutData.substring(0, pos1);
                }
                pos2 = cutData.indexOf("�z");
            }
            if (cutData.length() != 0)
            {
                if (andData.length() != 0)
                {
                    andData = andData + " " + cutData;
                }
                else
                {
                    andData = cutData;
                }
            }
        }
        catch (Exception e)
        {
            //
        }
        if (andData.length() == 0)
        {
            andData = cutData;
        }
        return (andData);
    }

    /**
     *  ������������w�肵�����̂ɕύX����
     *
     */
    public void setSearchTitleTemporary(String data)
    {
        temporaryTitleString = data;
        return;
    }

    /**
     * 
     *  �@�\�L�[�i�����L�[�̃[���j���������Ƃ��̏���...
     * 
     *
     */
    private void executeFunctionFeature()
    {
        int feature = parent.getFunctionKeyFeature(parent.SCENE_LISTFORM);
        if (feature == 0)
        {
            // �t�H���g�ύX
            changeFont(false);            
        }
        else if (feature == 1)
        {
            // �����L�[�ړ����[�h�ؑ�
            parent.toggleNumberInputMode(isNumberMovingMode);
            repaint();    
        }
        else if (feature == 2)
        {
            // �w���v�\�����[�h�ؑ�
            changeHelpShowMode();
        }
        else if (feature == 3)
        {
            // �X�����\��(�V���m�F���͋֎~)
            if (parent.isGetNewArrival() != true)
            {
                showMessageInformation();
            }
        }
        else if (feature == 4)
        {
            // �X������(���ǃX�����o)
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_REMAIN, OPTION_RESETMODE, -1);
        }
        else if (feature == 5)
        {
            // �X�����상�j���[(�V���m�F���͋֎~)
            if (parent.isGetNewArrival() != true)
            {
                selectedItem(false);
            }
        }
        else if (feature == 6)
        {
            // ���[�h���j���[
            showMenuSecond();
        }
        else if (feature == 7)
        {
            // ���O�Q�ƃ��[�h
            Thread thread = new Thread()
            {
                public void run()
                {
                    // �e�ɑ΂��A��ʂ𔲂������Ƃ�ʒm����
                    parent.EndListFormLogViewMode();
                }
            };
            thread.start();
        }
        else if (feature == 8)
        {
            // �N���A�L�[�Ɠ����ȏ���...
            // �������[�h�A�w���v�\�����[�h����������
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_NOTSPECIFY, OPTION_RESETMODE, -1);
            if (isHelpShowMode == true)
            {
                changeHelpShowMode();            
            }
        }
        else if (feature == 9)
        {
            // �֘A�L�[���[�h���o�@�\...
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }

            // �擾��URL�𐶐����A�X���^�C�g���ƘA������
            String url = "";
            int index = getSelectedIndex();
            if (currentFavoriteLevel >= 0)
            {
                // ���C�ɓ���ꗗ��\����...
                String boardUrl = favorManager.getUrl(index);
                int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
                String nick = boardUrl.substring(pos + 1);
                url  = boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + favorManager.getThreadNumber(index) + "/";
            }
            else
            {
                // �ʏ�̔ꗗ��\����...
                String boardUrl = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
                int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
                String nick = boardUrl.substring(pos + 1);
                url  = boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + subjectParser.getThreadNumber(index) + "/";
            }
            // �֘A�L�[���[�h�\��...
            showRelatedKeyword(url);
        }
        else if (feature == 10)
        {
            // �X���폜
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            deleteMessage();
        }
        else if (feature == 11)
        {
            // ��/Be@2ch���O�C��
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            showLoginMenu(false);
        }
        return;
    }

    /**
     *  �R�}���h�{�^�����s���̏���
     *  
     */
    public void commandAction(Command c, Displayable d)
    {
        // �V���m�F���́A�R�}���h�{�^���̎g�p���֎~����
        if (parent.isGetNewArrival() == true)
        {
            // �V���m�F���Ƀ��j���[�̐V���m�F�{�^���������ꂽ�璆�~����B
            if (c == getAllCmd)
            {
                isAbortNewArrival = true;
            }
            return;
        }    

        // �R�}���h�{�^�����s����
        if (c == modeCmd)
        {
            // ���j���[�Q
            showMenuSecond();
            return;
        }
        if (c == srchCmd)
        {
            // �X������
            startSearchTitle();
            return;
        }
        else if (c == opeCmd)
        {
            // �X������
            selectedItem(false);
            return;
        }        
        else if (c == getAllCmd)
        {
            // �V���m�F�̎��{...
            parent.startGetLogList();
            return;
        }
        else if (c == proxyCmd)
        {
            // a2B�ݒ�(Proxy�ݒ�)
            parent.OpenWriteFormUsingProxySetting();
            return;
        }
        else if (c == catCmd)
        {
            // �J�e�S���̑I��...
            selectCategory();            
            return;
        }
        else if (c == selCmd)
        {
            // �̑I��...
            selectBoard();            
            return;
        }
        else if (c == quitCmd)
        {
            // a2B�I��
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.quit_a2B();
                }
            };
            try
            {
                setBusyMessage("a2B�I����...", true);
                thread.start();
                thread.join();
            }
            catch (Exception ex)
            {
                // �������Ȃ�
            }
            return;
        }
        else if (c == deleteCmd)
        {
            // �X���폜
            deleteMessage();
            return;
        }

        // ���C�ɓ���\�����[�h���ǂ����̃`�F�b�N
        if (currentFavoriteLevel >= 0)
        {
            // ���C�ɓ���\�����[�h�Ȃ�A����ȍ~�̃R�}���h�͎󂯕t���Ȃ�
            return;
        }

        //////////  �ȍ~�͒ʏ�\�����[�h�݂̂ŗL�� /////////
        if (c == getCmd)
        {
            // �X���ꗗ�̎擾
            Thread thread = new Thread()
            {
                public void run()
                {
                    // �X���ꗗ�̎擾�����{����
                    subjectParser.prepareSubjectListUpdate(parent.getCurrentBbsIndex());
                }
            };
            setBusyMessage("subject.txt�擾��...", true);
            thread.start();
            parent.setSelectedSubjectIndex(0);
            return;    
        }
        return;
    }

    /**
     *   �t�H���g�T�C�Y�̕ύX
     * 
     */
    public void changeFont(boolean isReset)
    {
        int size;
        if (isReset == false)
        {
            
            // �t�H���g�T�C�Y�̕ύX (���������偨����... �Ɛ؂�ւ���)
            size   = screenFont.getSize();
            switch (size)
            {
              case Font.SIZE_LARGE:
                size = Font.SIZE_SMALL;
                break;

              case Font.SIZE_MEDIUM:
                size = Font.SIZE_LARGE;
                break;

              case Font.SIZE_SMALL:
                size = Font.SIZE_MEDIUM;
                break;

              default:
                size = Font.SIZE_SMALL;
                break;
            }
        }
        else
        {
            // �t�H���g�T�C�Y�����Z�b�g����
            size = parent.getListFontSize();
        }
        try
        {
            screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), size);
        }
        catch (Exception ex)
        {
        	size = Font.SIZE_SMALL;
            screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), size);
        }
        parent.setListFontSize(size);

        // �\���\�s���Đݒ肷��
        setLineWidthHeight(getWidth(), getHeight());

        // �\�����X�g���X�V����
        updateTitleList(FORCE_REDRAW);

        // �ĕ\��
        repaint();

        return;
    }
        
    /**
     *  �ꗗ�\�����s��(���C�ɓ���\�����[�h)
     *  
     */
    private void showListFavorite(Graphics g)
    {
        // �ꗗ��\����...
        currentScene = SHOW_SUBJECTLIST;
        
        // ���C�ɓ���ꗗ�Ɋ܂܂�Ă���X�������l��
        int subjects = favorManager.getNumberOfFavorites();
        if (nofSubjectIndex != subjects)
        {
            currentSelectedLine = 0;
            nofSubjectIndex     = subjects;
            listDrawMode        = FORCE_REDRAW;
        }

        // �^�C�g����\������
        if (currentFavoriteLevel == 0)
        {
            setTitle("�y ���C�ɓ��� �z");
        }
        else
        {
            setTitle("�y ���C�ɓ���(" + currentFavoriteLevel + ") �z");            
        }
/**
        g.setColor(0);
        g.fillRect(0, 0, getWidth(), screenFont.getHeight() + 1);
        g.setFont(screenFont);
        g.setColor(0x00ffffff);
        g.drawString("�y ���C�ɓ��� �z", 0, 0, (Graphics.LEFT | Graphics.TOP));
**/

        // ���C�ɓ��肪�o�^����Ă��Ȃ��̂��H
        if (nofSubjectIndex == 0)
        {
            // ��ʂ̓h��Ԃ�
            g.setColor(0x00f8f8f8);
            g.fillRect(0, screenFont.getHeight() + 1, getWidth(), getHeight());
            g.setColor(0);
            g.setFont(screenFont);

            // �X�����擾�̏ꍇ�ɂ́A�܂�Ԃ�
            g.setColor(0x00777777);
            g.drawString("(�o�^�Ȃ�)", 0, screenFont.getHeight() + 2, (Graphics.LEFT | Graphics.TOP));

            return;
        }

        // �X���ꗗ�̃y�[�W���ς��ꍇ�ɂ́A�^�C�g���ꗗ���X�V����
        if ((listDrawMode != TO_REDRAW)||(listCount <= 0))
        {
            updateTitleList(listDrawMode);
        }

        // �t�H���g��ݒ肷��
        g.setFont(screenFont);

        // �J�[�\���ړ����̍X�V...
        if ((listDrawMode == TO_REDRAW)&&(previousSelectedLine != currentSelectedLine))
        {

            // �J�[�\�����ړ����ꂽ�Ƃ��́A���̕����̂ݍĕ`�悷��B
            if (previousSelectedLine >= 0)
            {
                String title = favorManager.getSubjectSummary(displayIndexes[previousSelectedLine]);
                byte status = favorManager.getStatus(displayIndexes[previousSelectedLine]);
                drawTitleSummary(g, previousSelectedLine, title, status);
            }
            if (currentSelectedLine >= 0)
            {
                String title = favorManager.getSubjectSummary(displayIndexes[currentSelectedLine]);
                byte status = favorManager.getStatus(displayIndexes[currentSelectedLine]);
                drawTitleSummary(g, currentSelectedLine, title, status);
                previousSelectedLine = currentSelectedLine;
            }

            // �ꗗ�\�����̏ꏊ�������}�[�N������
            markArea(g, lastSubjectIndex);
            return;
        }
                
        // ��ʂ̓h��Ԃ�
        g.setColor(0x00f8f8f8);
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setColor(0);
        
        // �X���^�C�g���ꗗ�̕\��...
        int drawLine         = 0;
        for (int loop = 0; loop < screenLines; loop++)
        {
            int index = displayIndexes[loop];
            if (index >= 0)
            {
                String title    = favorManager.getSubjectSummary(index);
                byte status    = favorManager.getStatus(index);
                if (drawTitleSummary(g, drawLine, title, status) == true)
                {
                    lastSubjectIndex = index;
                    drawLine++;
                }    
            }
        }
        
        // �ꗗ�\�����̏ꏊ�������}�[�N������
        markArea(g, lastSubjectIndex);

        listDrawMode = TO_REDRAW;
        return;
    }

    /**
     *      �ꗗ�\�����s��(���C������)
     *   @param g
     */
    private void showList(Graphics g)
    {
        
        // ���݂̃J�e�S���Ɣ���\������
        int boardIndex = parent.getCurrentBbsIndex();
        if (boardIndex < 0)
        {
            return;
        }
        int categoryIndex = parent.getCurrentCategoryIndex();
        if (categoryIndex < 0)
        {
            return;
        }

        // �ꗗ��\����...
        currentScene = SHOW_SUBJECTLIST;
        
        // �ꗗ�Ɋ܂܂�Ă���X�������l��
        int subjects = subjectParser.getNumberOfSubjects();
        if (nofSubjectIndex != subjects)
        {
            currentSelectedLine = 0;
            nofSubjectIndex     = subjects;
            listDrawMode        = FORCE_REDRAW;
        }

        // �^�C�g����\������
        String bbsTitle = "�y " + bbsCommunicator.getCategory(categoryIndex) + " �F ";
        bbsTitle = bbsTitle + bbsCommunicator.getBoardName(boardIndex) + " �z";
        setTitle(bbsTitle);
/**
        g.setColor(0);
        g.fillRect(0, 0, getWidth(), screenFont.getHeight() + 1);
        g.setFont(screenFont);
        g.setColor(0x00ffffff);
        g.drawString(bbsTitle, 0, 0, (Graphics.LEFT | Graphics.TOP));
**/

        // �X���ꗗ�擾�ς݂��H
        if (nofSubjectIndex == 0)
        {
            // ��ʂ̓h��Ԃ�
            g.setColor(0x00f8f8f8);
            g.fillRect(0, 0, getWidth(), getHeight());
            g.setColor(0, 0, 0);
            g.setFont(screenFont);

            // �X�����擾�̏ꍇ�ɂ́A�܂�Ԃ�
            g.setColor(0x00777777);
            g.drawString("(���擾)", 0, 2, (Graphics.LEFT | Graphics.TOP));
            busyMessage = "";
            return;
        }

        // �X���ꗗ�̃y�[�W���ς��ꍇ�ɂ́A�^�C�g���ꗗ���X�V����
        if ((listDrawMode != TO_REDRAW)||(listCount <= 0))
        {
            updateTitleList(listDrawMode);
        }

        // �t�H���g��ݒ肷��
        g.setFont(screenFont);

        // �J�[�\���ړ����̍X�V...
        if ((listDrawMode == TO_REDRAW)&&(previousSelectedLine != currentSelectedLine))
        {
            // �J�[�\�����ړ����ꂽ�Ƃ��́A���̕����̂ݍĕ`�悷��B
            if (previousSelectedLine >= 0)
            {
                String title    = subjectParser.getSubjectSummary(displayIndexes[previousSelectedLine]);
                byte status    = subjectParser.getStatus(displayIndexes[previousSelectedLine]);    
                drawTitleSummary(g, previousSelectedLine, title, status);
            }
            if (currentSelectedLine >= 0)
            {
                String title    = subjectParser.getSubjectSummary(displayIndexes[currentSelectedLine]);
                byte status    = subjectParser.getStatus(displayIndexes[currentSelectedLine]);
                drawTitleSummary(g, currentSelectedLine, title, status);
                previousSelectedLine = currentSelectedLine;
            }

            // �ꗗ�\�����̏ꏊ�������}�[�N������
            markArea(g, lastSubjectIndex);
            return;
        }
                
        // ��ʂ̓h��Ԃ�
        g.setColor(0x00f8f8f8);
        g.fillRect(0, 0, getWidth(), getHeight());
        g.setColor(0);
        
        // �X���^�C�g���ꗗ�̕\��...
        int drawLine         = 0;
        for (int loop = 0; loop < screenLines; loop++)
        {
            int index = displayIndexes[loop];
            if (index >= 0)
            {
                String title  = subjectParser.getSubjectSummary(index);
                byte status  = subjectParser.getStatus(index);
                if (drawTitleSummary(g, drawLine, title, status) == true)
                {
                    lastSubjectIndex = displayIndexes[loop];
                    drawLine++;
                }    
            }
        }
        
        // �ꗗ�\�����̏ꏊ�������}�[�N������
        markArea(g, lastSubjectIndex);

        listDrawMode = TO_REDRAW;
        return;
    }

    /**
     *  Web�u���E�U�ŊJ��
     *
     */
    public void launchWebBrowserCurrentSelectedIndex()
    {
        int   index;
        long  threadNumber;
        int   current;
        String url;
        if (currentFavoriteLevel >= 0)
        {
            index = currentSubjIndex;
            url =  favorManager.getUrl(index);    
            threadNumber = favorManager.getThreadNumber(index);
            current = favorManager.getCurrentMessage(index);
        }
        else
        {
            index = getSelectedIndex();
            int boardIndex = parent.getCurrentBbsIndex();
            url = bbsCommunicator.getBoardURL(boardIndex);
            threadNumber = subjectParser.getThreadNumber(index);
            current = subjectParser.getCurrentMessage(index);
        }
        int pos = url.indexOf("://");
        pos = pos + 4;
        pos = url.indexOf("/", pos);
        String nick = url.substring(pos);

        if (url.indexOf("2ch.net") < 0)
        {
            // 2�����˂�O��URL��ǂ�ł����ꍇ...
            url = url.substring(0, pos) + "/test/read.cgi" + nick + threadNumber + "/" + current + "n-";
        }
        else
        {
            // �Q�����Ȃ���������ꍇ�ɂ́A�N���b�V�b�N���j���[�𗘗p����
            url = "http://c.2ch.net/test/-" + nick + threadNumber + "/" + current + "-";
            //url = "http://c-others.2ch.net/test/-" + nick + threadNumber + "/" + current + "-";
        }
        parent.updateBusyMessage("Open URL", "URL>>" + url, false);
        parent.openWebBrowser(url);

        return;
    }
    
    /**
     * �r�W�[���b�Z�[�W�̐ݒ�...
     * @param message
     */
    public void setBusyMessage(String message, boolean forceUpdate)
    {
        busyMessage = message;
        repaint();
        if (forceUpdate == true)
        {
            serviceRepaints();
        }
        return;
    }

    /**
     * �r�W�[���b�Z�[�W�i�����s�j��\������
     * 
     * @param g
     * @param title
     */
    private void drawBusyAreaMultiple(Graphics g, int topHeight)
    {
        g.setColor(0x00800080);    // ��
        int offset    = 0;
        int width     = screenWidth - screenFont.stringWidth("XX");
        int maxLength = busyMessage.length();
        int maxHeight = getHeight();
        while (topHeight < maxHeight)
        {
            int length  = 0;
            while ((length + offset < maxLength)&&(screenFont.substringWidth(busyMessage, offset, length) < width))
            {
                length++;
            }
            g.drawSubstring(busyMessage, offset, length, 1, topHeight, (Graphics.LEFT | Graphics.TOP));
            if (length >= maxLength)
            {
                break;
            }
            offset = offset + length;
            topHeight = topHeight + screenFont.getHeight();
        }
    }

    /**
     *   �ꗗ�\�����̏ꏊ�������}�[�N������
     * 
     * 
     */
    private void markArea(Graphics g, int index)
    {
        // �ꗗ�\�����̏ꏊ�������}�[�N������
        int drawLine = (screenWidth - 3);
        int height   = screenLines * (screenFont.getHeight() + 1);

        g.setColor(0x00ffffff);
        g.fillRect(drawLine, 0, drawLine + 3, height);
        if (currentFavoriteLevel >= 0)
        {
            // ���C�ɓ���\�����[�h�̂Ƃ��́A�F��ς���
            g.setColor(0x0000cddf);
        }
        else
        {
            g.setColor(0);
        }            
        g.drawLine(drawLine, 0, drawLine, height);
        int start = nofSubjectIndex;
        if (start != 0)
        {
            int par = (index * height) / start;
//            float par = ((float) index / (float) start) * ((float) height);
            start = (int) par - 2;
            if (start < 0)
            {
                start = 0;
            }
            if (searchTitlePickupHour > 0)
            {
                g.setColor(0x00ddccff);
            }
            else if (searchTitleMessageStatus == NO_SEARCHING_MODE)
            {
                g.setColor(0x00000000);
            }
            else if (searchTitleMessageStatus == SEARCHING_ONLYDB)
            {
                // �������[�h�̂Ƃ��́A�F��ς���
                g.setColor(0x00ff00ff);
            }
            else
            {
                // �������[�h�̂Ƃ��́A�F��ς���
                g.setColor(0x00ff0000);
            }
            g.fillRect(drawLine, start, 3, 8);
            g.setColor(0x00000000);
        }
        return;
    }

    /**
     *  �X���^�C�g����`�悷��
     * 
     * 
     */
    private boolean drawTitleSummary(Graphics g, int line, String title, byte status)
    {
        int foreColor  = getForeColor(status, line);
        int backColor  = getBackColor(status, line);
        int fontHeight = screenFont.getHeight();
        int stringTop  =  (line * (fontHeight+ 1));
        int width      = screenWidth - 3;
        if (title != null)
        {
            if (line == currentSelectedLine)
            {
                g.setColor(foreColor);
                g.fillRect(0, stringTop, width, fontHeight + 2);
                g.setColor(backColor);

                // �X���^�C�����s�\�����[�h�̏ꍇ...
                if ((busyAreaSize > 1)&&(parent.isGetNewArrival() != true))
                {
                    // �����G���A�ɃX���^�C��\������B�B�B
                    int pos = title.indexOf("]");
                    if (pos >= 0)
                    {
                        busyMessage = title.substring(pos + 1);
                    }
                    else
                    {
                        busyMessage = title;
                    }
                }        

                int offset = 0;
                int length = title.length();
                while (screenFont.substringWidth(title, offset, (length - offset)) > width)
                {
                    offset++;
                }
                g.drawSubstring(title, offset, (length - offset), 0, stringTop, (Graphics.LEFT | Graphics.TOP));
            }
            else
            {
                g.setColor(backColor);
                g.fillRect(0, stringTop, width, fontHeight + 2);
                g.setColor(foreColor);
                g.drawString(title, 0, stringTop, (Graphics.LEFT | Graphics.TOP));
            }
            title = null;
            g.setColor(0);
            return (true);
        }
        return (false);
    }

    /**
     *  �X���^�C�g���ꗗ���쐬����...
     * 
     * 
     */
    private boolean updateTitleList(int command)
    {
        int start, update;

        if (listCount > 0)
        {
            // �擪�Ɩ����̃C���f�b�N�X�ԍ����擾����
            int topIndex    = displayIndexes[0];
            int bottomIndex = displayIndexes[listCount - 1];
            if (topIndex > bottomIndex)
            {
                int tempIndex = topIndex;
                topIndex = bottomIndex;
                bottomIndex = tempIndex;
            }
            if (command == TO_PREVIOUS)
            {
                // �O�y�[�W�̃f�[�^���쐬����...
                start  = topIndex;
                update = -1;
                if (start == 0)
                {
                    // ���ɐ擪�ɂ���A�č\�z���Ȃ�
                    return (false);
                }
            }
            else if (command == TO_NEXT)
            {
                // ���y�[�W�̃f�[�^���쐬����
                start  = bottomIndex;
                update = 1;
            }
            else   // if (command == FORCE_REDRAW)
            {
                // �ĕ`�悷��
                start = topIndex;
                update = 1;
            }
        }
        else
        {
            // �擪����f�[�^���쐬����
            start     = 0;
            update    = 1;
            listCount = 0;
        }

        listCount = 0;
        for (int loop = start; ((loop >= 0)&&(loop < nofSubjectIndex)&&(listCount < screenLines)); loop = loop + update)
        {
            // �\�����ׂ����ǂ����`�F�b�N����
            boolean check = false;
            if (currentFavoriteLevel < 0)
            {
                check = canShowTitle(loop);
            }
            else
            {
                check = canShowTitleFavorite(loop);
            }
            if (check == true)
            {
                displayIndexes[listCount] = loop;
                listCount++;
            }
        }
        if (command == TO_PREVIOUS)
        {
            // �t���Ō��������ꍇ�ɂ́A���������Ԃɕ��בւ����s��...
            for (int loop = 0; loop < listCount / 2; loop++)
            {
                int temp = displayIndexes[loop];
                displayIndexes[loop] = displayIndexes[(listCount - 1) -loop];
                displayIndexes[(listCount - 1) -loop] = temp;
            }
        }

        // �X���^�C�g���̂��܂蕔���ɂ͕��̒l������
        for (int loop = listCount; loop < screenLines; loop++)
        {
            displayIndexes[loop] = -1;
        }
        return (true);
    }
    
    /**
     *   �I�����ꂽ�X���̃C���f�b�N�X�ԍ�����������
     * 
     * 
     */
    private int getSelectedIndex()
    {
        if ((currentSelectedLine < 0)||(currentSelectedLine > screenLines))
        {
            return (-1);
        }        
        return (displayIndexes[currentSelectedLine]);
    }
    

    /**
     *  �^�C�g����\�����ׂ����ǂ����̃`�F�b�N���s��
     * 
     * 
     */
    private boolean canShowTitle(int index)
    {
        // �X���o�ߓ������ݒ肵�Ă���ꍇ...
        if (searchTitlePickupHour > 0)
        {
            long dateTime = getProgressDate(subjectParser.getThreadNumber(index));
            if (dateTime <= searchTitlePickupHour)
            {
                return (true);
            }
            if (searchTitleString == null)
            {
                // ���������񂪐ݒ肳��Ă��Ȃ��ꍇ�ɂ́A���o���Ȃ�
                return (false);
            }
        }

        // �������[�h��OFF�̂Ƃ��͏I������
        if ((searchTitleMessageStatus == NO_SEARCHING_MODE)&&(searchTitleString == null))
        {
            return (true);
        }

        // �X����Ԃ̏�Ԃ����o����...
        byte status = subjectParser.getStatus(index);
        if (status > searchTitleMessageStatus)
        {
            return (false);
        }

        // ���������񂪐ݒ肳��ĂȂ��ꍇ�ɂ�OK
        if (searchTitleString == null)
        {
            return (true);
        }
        
        // �^�C�g�����擾
        String title  = subjectParser.getSubjectName(index);
        if (title != null)
        {
            if (searchIgnoreCase != true)
            {
                // ���������񂪊܂܂�Ă��邩�m�F����
                if (title.indexOf(searchTitleString) >= 0)
                {
                    return (true);
                }
                return (false);
            }
    
            // ���������񂪊܂܂�Ă��邩�m�F����(�啶��/��������ʂȂ�)
            String target    = title.toLowerCase();
            String matchData = searchTitleString.toLowerCase();
            if (target.indexOf(matchData) >= 0)
            {
                return (true);
            }            
        }
        return (false);
    }
    
    /**
     *  �^�C�g����\�����ׂ����ǂ����̃`�F�b�N���s��(���C�ɓ���\�����[�h�̂Ƃ�)
     * 
     * 
     */
    private boolean canShowTitleFavorite(int index)
    {

        // ���C�ɓ���\�����x���̊m�F
        int level = favorManager.getThreadLevel(index);
        if ((currentFavoriteLevel != 0)&&(currentFavoriteLevel != level))
        {
            return (false);
        }

        // �X���o�ߓ������ݒ肵�Ă���ꍇ...
        if (searchTitlePickupHour > 0)
        {
            long dateTime = getProgressDate(subjectParser.getThreadNumber(index));
            if (dateTime <= searchTitlePickupHour)
            {
                return (true);
            }
            if (searchTitleString == null)
            {
                // ���������񂪐ݒ肳��Ă��Ȃ��ꍇ�ɂ́A���o���Ȃ�
                return (false);
            }
        }

        // �������[�h��OFF�̂Ƃ��͏I������
        if ((searchTitleMessageStatus == NO_SEARCHING_MODE)&&(searchTitleString == null))
        {
            return (true);
        }

        // �X����Ԃ̏�Ԃ����o����...
        byte status = favorManager.getStatus(index);
        if (status > searchTitleMessageStatus)
        {
            return (false);
        }

        // ���������񂪐ݒ肳��ĂȂ��ꍇ�ɂ�OK
        if (searchTitleString == null)
        {
            return (true);
        }
        
        // �^�C�g�����擾
        String title  = favorManager.getSubjectName(index);
        if (title != null)
        {
            if (searchIgnoreCase != true)
            {
                // ���������񂪊܂܂�Ă��邩�m�F����
                if (title.indexOf(searchTitleString) >= 0)
                {
                    return (true);
                }
                return (false);
            }
    
            // ���������񂪊܂܂�Ă��邩�m�F����(�啶��/��������ʂȂ�)
            String target    = title.toLowerCase();
            String matchData = searchTitleString.toLowerCase();
            if (target.indexOf(matchData) >= 0)
            {
                return (true);
            }            
        }        
        return (false);
    }

    /**
     * �J�e�S���I������...
     * 
     */
    private void selectCategory()
    {
        setBusyMessage("�J�e�S������...", true);

        // �J�e�S���I���̎��{
        Thread thread = new Thread()
        {
            public void run()
            {
                // �r�W�[�t�H�[����\������...
                setTitle("�J�e�S������...");
                
                // �ꗗ�����N���A����
                subjectParser.leaveSubjectList(parent.getCurrentBbsIndex());

                // ���̓J�e�S���I�����[�h...
                currentScene = parent.SHOW_CATEGORYLIST;

                int limit = bbsCommunicator.getNumberOfCategory();
                Vector categoryList = new Vector(limit);
                for (int loop = 0; loop < (limit - 2); loop++)
                {
                    // �J�e�S���ꗗ���擾���� (2���炵�Ă��闝�R�́A�u���̃T�C�g�v�Ɓu�܂�BBS�v������Ă��邽��)
                    // (���ӁI a2B�ł͓����Ȃ��J�e�S��������I�I�j
                    String categoryName = bbsCommunicator.getCategory(loop);
                    categoryList.addElement(categoryName);
                }

                // �J�e�S���ꗗ��\������
                int index = parent.getCurrentCategoryIndex();
                parent.openSelectForm("�J�e�S����I�����Ă�������", categoryList, index, currentScene);

                categoryList = null;
                return;
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
            // �������Ȃ�...
        }
        return;
    }

    /**
     *  ���C�ɓ���o�^/�������s��
     *  
     */
    public void setResetFavoriteThread(int index, boolean mode)
    {
        // �o�^�您��ю擾�t�@�C�����𐶐�����
        int boardIndex = parent.getCurrentBbsIndex();
        String url      = bbsCommunicator.getBoardURL(boardIndex);

        int pos = url.lastIndexOf('/', url.length() - 2);
        String nick = url.substring(pos + 1);

        if (currentFavoriteLevel < 0)
        {
            // ���C�ɓ���o�^/�o�^���������{����
            subjectParser.setResetFavorite(index, url, nick, mode);
        }
        else
        {
            // ���C�ɓ��肩��폜����...
            String threadNick   = favorManager.getBoardNick(index);
            long  threadNumber = favorManager.getThreadNumber(index);
            
            // �����݁A�W�J���̔ꗗ�X���������ꍇ...
            if (threadNick.equals(nick) == true)
            {
                // �X���̓�����񂩂炨�C�ɓ���o�^���������{����
                subjectParser.removeFavorite(nick, threadNumber);
            }

            // ���C�ɓ���ꗗ����폜����
            favorManager.removeFavorite(threadNick, favorManager.getThreadNumber(index));

            // �\�����X�g���X�V���čĕ\��
            updateTitleList(FORCE_REDRAW);
            repaint();
        }
        return;
    }

    /**
     *  �I�����[�h
     *  
     */
    public void boardSelectionMode()
    {
        // �̑I��...
        selectBoard();
        return;
    }
    
    /**
     *  �I������...
     *  
     */
    private void selectBoard()
    {
        // �I���̎��{
        setBusyMessage("����͒�...", true);
        Thread thread = new Thread()
        {
            public void run()
            {
                // �^�C�g���ɕ\��...
                setTitle("����͒�...");

                // ��؂�ւ���
                subjectParser.leaveSubjectList(parent.getCurrentBbsIndex());

                // ���C�ɓ�������i�Ƃ肠�����j�o�b�N�A�b�v����
                if (parent.getIgnoreFavoriteBackup() != true)
                {
                    favorManager.backup();
                }

                // �ꗗ���X�V����
                bbsCommunicator.refreshBoardInformationCache(parent.getCurrentCategoryIndex(), false, null);

                int   index    = parent.getCurrentCategoryIndex();

                // ���b�Z�[�W��\������
                String msg =  "�y" + bbsCommunicator.getCategory(parent.getCurrentCategoryIndex()) + "�z" + " ��I�����Ă��������B";
            
                // ���͔I�����[�h...
                currentScene = parent.SHOW_BBSLIST;

                // �����擾����
                int endIndex   = bbsCommunicator.getCategoryIndex((index + 1));
                int startIndex = bbsCommunicator.getCategoryIndex(index);
                int count      = endIndex - startIndex;
                
                // �I�������\�z����
                Vector boardList = new Vector(count);
                for (int loop = 0; loop < count; loop++)
                {
                    String boardName    = bbsCommunicator.getBoardName(startIndex + loop);
                    boardList.addElement(boardName);
                }

                // �ꗗ��\������
                index = parent.getCurrentBbsIndex();
                if ((index >= startIndex)&&(index <= endIndex))
                {
                    index = index - startIndex;
                }
                else
                {
                    index = 0;
                }
                parent.openSelectForm(msg, boardList, index, currentScene);
                boardList = null;
                return;
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }

    /**
     * �I�������X�����폜����...
     * 
     */
    private void deleteMessage()
    {
        // �A�C�e�����I�����ꂽ
        int index = getSelectedIndex();
        if (index < 0)
        {
            return;
        }
        if (currentScene != SHOW_SUBJECTLIST)
        {
            return;
        }
        String doDelete  = "�폜���s";
        String title     = "";
        if (currentFavoriteLevel >= 0)
        {
            // ���C�ɓ���ꗗ�������ꍇ...
            doDelete = "dat�t�@�C�������폜";
            String fileName = favorManager.getThreadNumber(index) + ".dat";
            gettingLogDataFile = favorManager.getBoardNick(index) + fileName;

            // �^�C�g���̕\��...
            title = "�����폜�F " + favorManager.getSubjectName(index);
        }
        else
        {
            // �ʏ�̔ꗗ�������ꍇ...
            int status = subjectParser.getStatus(index);
            if (status == a2BsubjectDataParser.STATUS_NOTYET)
            {
                // �X�����擾�Ȃ�A�������Ȃ�
                return;
            }

            // �t�@�C��������肷��
            int boardIndex  = parent.getCurrentBbsIndex();
            String url      = bbsCommunicator.getBoardURL(boardIndex);
            String fileName = subjectParser.getThreadFileName(index);
            int pos = url.lastIndexOf('/', url.length() - 2);
            gettingLogDataFile = url.substring(pos + 1) + fileName;
            
            // �^�C�g���̕\��...
            title = "�X���폜�F " + subjectParser.getSubjectName(index);
        }

        // ���̓X���폜�m�F���[�h
        currentScene = parent.CONFIRM_DELETELOGDAT;

        Vector commandList = new Vector(2);
        String cancelCmd = "�L�����Z��";
        commandList.addElement(cancelCmd);
        commandList.addElement(doDelete);

        parent.openSelectForm(title, commandList, 0, currentScene);
        commandList = null;
        
        currentSubjIndex = index;

        // �K�x�R�����{...
        System.gc();
        return;
    }

    /**
     *  ���j���[�i���̂Q�j
     *  
     */
    private void showMenuSecond()
    {
        // �^�C�g���̕\��...
        String title = "���[�h�I��";

        Vector commandList = new Vector(10);
        commandList.addElement("���O�Q�ƃ��[�h");
        commandList.addElement("�����L�[�ړ����[�h");
        commandList.addElement("�w���v�\�����[�h");
        commandList.addElement("���ǃX�����o");
        commandList.addElement("�t�H���g�T�C�Y�ύX");
        commandList.addElement("2ch���O�C��(��/BE@2ch)");
        if (currentFavoriteLevel >= 0)
        {
            // ���C�ɓ���ꗗ�̂Ƃ��A�@�\��ǉ�
            commandList.addElement("�����̔ꗗ��");
            commandList.addElement("���C�ɓ������");
            commandList.addElement("���C�ɔԍ��ύX");
            commandList.addElement("�X��URL�X�V");
        }
        else
        {
            // �ʏ�̔ꗗ�̂Ƃ��A�@�\��ǉ�
            commandList.addElement("�v���r���[");
            commandList.addElement("���C�ɓ���o�^");
            commandList.addElement("�X������");
            commandList.addElement("dat�T��");
        }
        currentScene = parent.SHOW_SUBJECTCOMMANDSECOND;
        parent.openSelectForm(title, commandList, 0, currentScene);
        commandList = null;

        // �K�x�R�����{...
        System.gc();
        return;
    }

    /**
     *  ���j���[�̎��s
     * 
     * 
     * @param scene
     * @param index
     */
    public boolean executeMenuSecond(byte scene, int index)
    {
        if (index == 0)
        {
            // ���O�Q�ƃ��[�h
            Thread thread = new Thread()
            {
                public void run()
                {
                    // �e�ɑ΂��A��ʂ𔲂������Ƃ�ʒm����
                    parent.EndListFormLogViewMode();
                }
            };
            thread.start();
            return (true);
        }
        if (index == 1)
        {
            // �����L�[�ړ����[�h�ؑ�
            parent.toggleNumberInputMode(isNumberMovingMode);
            repaint();
        }
        if (index == 2)
        {
            // �w���v�\�����[�h�ؑ�
            changeHelpShowMode();
        }
        if (index == 3)
        {
            // �X������(���ǃX�����o)
            setSearchTitleInformation(null, a2BsubjectDataParser.STATUS_REMAIN, OPTION_RESETMODE, -1);
        }
        if (index == 4)
        {
            // �t�H���g�T�C�Y�ύX
            changeFont(false);
        }
        if (index == 5)
        {
            // �� / BE ���O�C���̎��s
            // ���O�C�����j���[��\������
            showLoginMenu(true);
//            isSecondMenuScene = parent.SHOW_LOGINMODEMENU;
//            showLoginMenu(true);
            return (false);
        }

        // �X���ԍ��̒��o
        int threadIndex = getSelectedIndex();
        if (threadIndex >= 0)
        {
            currentSubjIndex = threadIndex;
            if (index == 6)
            {
                if (currentFavoriteLevel >= 0)
                {
                    // �����̔ꗗ�֐؂�ւ��� �i���̔ԍ�(4)�́A�u�X������(���C�ɓ���p)�v�̔ԍ��Ɉˑ��j
                    executeSubjectOperationFavorite(4);
                }
                else
                {
                    // �v���r���[�i���̔ԍ�(1)�́A�u�X������(�ʏ�p)�v�̔ԍ��Ɉˑ��j
                    executeSubjectOperation(1);
                }
            }
            if (index == 7)
            {
                if (currentFavoriteLevel >= 0)
                {
                    // ���C�ɓ���o�^�����i���̔ԍ�(3)�́A�u�X������(���C�ɓ���p)�v�̔ԍ��Ɉˑ��j
                    executeSubjectOperationFavorite(3);
                }
                else
                {
                    // ���C�ɓ���o�^�i���̔ԍ�(5)�́A�u�X������(�ʏ�p)�v�̔ԍ��Ɉˑ��j
                    executeSubjectOperation(5);                
                }
            }
        }
        if (index == 8)
        {
            if (currentFavoriteLevel >= 0)
            {
                // ���C�ɓ���ԍ��ύX
                informationString   = "���C�ɔԍ��H (0,1-9)";
                showInformationMode = 1;
                repaint();
            }
            else
            {
                // �X������
                int boardIndex = parent.getCurrentBbsIndex();
                if (boardIndex >= 0)
                {
/***/
                    serviceRepaints();
                    parent.OpenWriteForm(true, "�X������ : " + bbsCommunicator.getBoardName(boardIndex), -1, -1);
/***/
                }
            }
        }
        if (index == 9)
        {
            if (currentFavoriteLevel >= 0)
            {
                // (���C�ɓ���) �F ���C�ɓ���X����URL�𒲐�����
                Thread thread = new Thread()
                {
                    public void run()
                    {
                        // �X���ꗗ�̎擾�����{����
                        boolean ret = favorManager.updateURLData(bbsCommunicator);
                        if (ret == false)
                        {
                            informationString = "�� �X��URL�X�V���s ��";
                        }
                        else
                        {
                            informationString = " �X��URL�̍X�V����";
                        }
                        showInformationMode = 0;
                        repaint();
                        parent.vibrate();
                    }
                };
                thread.start();
                showInformationMode = 0;
                informationString = "�X��URL���X�V��...";
                repaint();
/**
                try
                {
                    // �����҂�...
                    thread.join();
                }
                catch (Exception e)
                {
                    // �������Ȃ�...
                }
**/
            }
            else
            {
                // �ʏ�F�X���̖��qdat�t�@�C���T��
                Thread thread = new Thread()
                {
                    public void run()
                    {
                        // �X���ꗗ�̎擾�����{����
                        int nofFile = subjectParser.collectDatFile();
                        if (nofFile < 0)
                        {
                            informationString = "�� �t�@�C���������s ��";
                        }
                        else
                        {
                            informationString = nofFile + " ����dat�t�@�C������";
                        }
                        showInformationMode = 0;
                        repaint();
                        parent.vibrate();
                    }
                };
                thread.start();
                showInformationMode = 0;
                informationString = "�X���t�@�C��������...";
                repaint();
            }
        }
        System.gc();
        return (true);
    }

    /**
     *  ���O�C�����j���[�̎��s
     * 
     * 
     * @param scene
     * @param index
     */
    public void executeLoginMenu(byte scene, int index)
    {
        if (index == 0)
        {
            // �����O�C���̎��s
            parent.startOysterLogin();            
        }
        else if (index == 1)
        {
            // Be@2ch���O�C���̎��s
            parent.startBe2chLogin();
        }
        System.gc();
        return;
    }

    /**
     * �X���ꗗ�̃A�N�V�����I��...
     * 
     */
    private void showActionChoiceGroup(int subjIndex, boolean autoOpen)
    {
        int  status     = subjectParser.getStatus(subjIndex);
        int boardIndex  = parent.getCurrentBbsIndex();
        String fileName = subjectParser.getThreadFileName(subjIndex);
        String url      = bbsCommunicator.getBoardURL(boardIndex);

        // �擾�您��ю擾�t�@�C�����𐶐�����
        int pos = url.lastIndexOf('/', url.length() - 2);
        gettingLogDataFile = url.substring(pos + 1) + fileName;

        if ((autoOpen == true)&&(status != a2BsubjectDataParser.STATUS_NOTYET)&&(status != a2BsubjectDataParser.STATUS_RESERVE))
        {
            // �X���Q��... (�e�ɑ΂��A��ʂ𔲂��ăX����\�����邱�Ƃ�ʒm����)
            currentSubjIndex = subjIndex;
            parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, subjectParser.getCurrentMessage(currentSubjIndex));
            gettingLogDataFile = null;
            return;
        }    

        if (parent.isGetNewArrival() == true)
        {
            // �V���m�F���̓��j���[�\�����֎~����
            return;
        }

        // �^�C�g���̕\��...
        String title = subjectParser.getSubjectName(subjIndex);

        Vector commandList = new Vector(9);
        commandList.addElement("�Ď擾");
        commandList.addElement("�v���r���[");
        commandList.addElement("�擾�\��");
        commandList.addElement("�擾");
        commandList.addElement("�\��");
        commandList.addElement("���C�ɓ���o�^");
        commandList.addElement("���C�ɓ������");
        commandList.addElement("�u���E�U�ŊJ��");
        commandList.addElement("�֘A�L�[���[�h");

        currentScene = parent.SHOW_SUBJECTCOMMAND;
        parent.openSelectForm(title, commandList, 4, currentScene);
        commandList = null;
        
        currentSubjIndex = subjIndex;

        // �K�x�R�����{...
        System.gc();
        return;
    }
    
    /**
     * �X���ꗗ�̃A�N�V�����I��...
     * 
     */
    private void showActionChoiceGroupFavorite(int index, boolean autoOpen)
    {
        byte  status     = favorManager.getStatus(index);
        String fileName = favorManager.getThreadNumber(index) + ".dat";
        gettingLogDataFile = favorManager.getBoardNick(index) + fileName;
        gettingLogDataURL  = favorManager.getUrl(index) + "dat/" + fileName;

        currentScene = SHOW_SUBJECTLIST;
        currentSubjIndex = index;

        if ((autoOpen == true)&&(status != favorManager.STATUS_NOTYET)&&(status != favorManager.STATUS_RESERVE))
        {
            // �X���Q��... (�e�ɑ΂��A��ʂ𔲂��ăX����\�����邱�Ƃ�ʒm����)
            parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, favorManager.getCurrentMessage(currentSubjIndex));
            gettingLogDataFile = null;
            return;
        }    

        if (parent.isGetNewArrival() == true)
        {
            // �V���m�F���̓��j���[�\�����֎~����
            return;
        }

        // �^�C�g���̕\��...
        String title = favorManager.getSubjectName(index);

        Vector commandList = new Vector(7);
        commandList.addElement("�Ď擾");
        commandList.addElement("�擾");
        commandList.addElement("�\��");
        commandList.addElement("���C�ɓ������");
        commandList.addElement("�����̔ꗗ��");
        commandList.addElement("�u���E�U�ŊJ��");
        commandList.addElement("�֘A�L�[���[�h");

        currentScene = parent.SHOW_SUBJECTCOMMANDFAVORITE;
        parent.openSelectForm(title, commandList, 2, currentScene);
        commandList = null;

        // �K�x�R�����{...
        System.gc();
        return;
    }
    
    /**
     *   ���ݑI�𒆃X���̃^�C�g�����擾����
     * 
     */
    public String getSubjectName()
    {
        String title = "";
        if (currentFavoriteLevel < 0)
        {
            title = subjectParser.getSubjectName(currentSubjIndex);
        }
        else
        {
            // ���C�ɓ���ꗗ�\�����[�h
            title = favorManager.getSubjectName(currentSubjIndex);
        }
        return (title);
    }

    /**
     *  ���j���[�i���̂Q�j
     *  
     */
    private void showLoginMenu(boolean isMenu)
    {
        // �^�C�g���̕\��...
        String title = "2ch���O�C��";

        Vector commandList = new Vector(2);
        commandList.addElement("��(Oyster)");
        commandList.addElement("BE@2ch");
        currentScene = parent.SHOW_LOGINMENU;            

        parent.openSelectForm(title, commandList, 0, currentScene);
        commandList = null;

        // �K�x�R�����{...
        System.gc();
        return;
    }
        
    /**
     *  �X�����̕\��...
     *  
     */
    private void showMessageInformation()
    {
        int   subjIndex = getSelectedIndex();
        String title = "";
        String addInfo = "";
        byte  status = 0;

        long  threadNumber  = 0;
        int   messageNumber = 0;
        int   level         = -1;

        // �\�����[�h���m�F�\���f�[�^��؂�ւ���
        if (currentFavoriteLevel < 0)
        {
            threadNumber = subjectParser.getThreadNumber(subjIndex);
            messageNumber = subjectParser.getNumberOfMessages(subjIndex);

            // �X���^�C�g��
            title = subjectParser.getSubjectName(subjIndex);

            // �擾��URL�𐶐����A�X���^�C�g���ƘA������
            int boardIndex = parent.getCurrentBbsIndex();
            String boardUrl = bbsCommunicator.getBoardURL(boardIndex);
            int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
            String nick = boardUrl.substring(pos + 1);
            title = title + "\r\n" + boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + threadNumber + "/";

            // ���C�ɓ���ɓo�^����Ă��邩�`�F�b�N����
            if (subjectParser.isFavorite(subjIndex) == true)
            {
                addInfo ="<<< ���C�ɓ��� >>>\r\n";
            }
            status = subjectParser.getStatus(subjIndex);
        }
        else
        {
            threadNumber  = favorManager.getThreadNumber(subjIndex);
            messageNumber = favorManager.getNumberOfMessages(subjIndex);

            // �X���^�C�g���AURL�̏��𐶐�����
            title = favorManager.getSubjectName(subjIndex);

            String boardUrl = favorManager.getUrl(subjIndex);
            int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
            String nick = boardUrl.substring(pos + 1);
            title = title + "\r\n" + boardUrl.substring(0, pos) + "/test/read.cgi/" + nick + threadNumber + "/";
            level = favorManager.getThreadLevel(subjIndex);
            addInfo = "<<< ���C�ɓ��� >>>\r\n";
            status = favorManager.getStatus(subjIndex);
        }

        // �X����Ԃ�\��
        if (status == a2BsubjectDataParser.STATUS_NOTYET)
        {
            addInfo = addInfo + "(���擾)";
        }
        else if (status == a2BsubjectDataParser.STATUS_NEW)
        {
            addInfo = addInfo + "(�V�K�擾)";            
        }
        else if (status == a2BsubjectDataParser.STATUS_UPDATE)
        {
            addInfo = addInfo + "(�X�V����)";
        }
        else if (status == a2BsubjectDataParser.STATUS_REMAIN)
        {
            addInfo = addInfo + "(���ǂ���)";                
        }
        else if (status == a2BsubjectDataParser.STATUS_ALREADY)
        {
            addInfo = addInfo + "(����)";            
        }
        else if (status == a2BsubjectDataParser.STATUS_RESERVE)
        {
            addInfo = addInfo + "(�擾�\��)";
        }
        else
        {
            addInfo = addInfo + "(???)";            
        }

        // �X���̐���(?)��\������
        long dateNum = getProgressDate(threadNumber);
        if ((messageNumber > 0)&&(dateNum >= 0))
        {
            addInfo = addInfo + " [" + (dateNum / 24) + "���o��]";
        }
        
        // �_�C�A���O�ŃX������\������
        title = title + parent.getHeapInfo();
//        parent.showDialog(parent.SHOW_INFO, 0, "�X�����", title);
        parent.OpenWriteFormUsingThreadOperationMode("�X�����", title, addInfo, level);
        return;
    }

    /**
     * ���݂���o�ߓ������擾����
     * 
     * @param sinceDate
     * @return
     */
    private long getProgressDate(long sinceDate)
    {
        Date dateTime = new Date();
        long diffTime = (dateTime.getTime() / 1000) - sinceDate;
        long num = 60 * 60;
        num = diffTime / num;
        return (num);
    }

    /**
     * ���C�ɓ���X�����x�����X�V����
     * @param level
     */
    public void updateFavoriteThreadLevel(int level)
    {
        if ((currentFavoriteLevel >= 0)&&(level >= 0))
        {
            int index = getSelectedIndex();
            if (index >= 0)
            {
                favorManager.setThreadLevel(index, level);
                if (parent.getIgnoreFavoriteBackup() != true)
                {
                	favorManager.doBackup();
                }
            }
        }
        return;
    }
    
    /**
     *   �w���v�C���[�W��\������
     * 
     *
     */
    private void changeHelpShowMode()
    {
        if (helpImage == null)
        {
            return;
        }
        if (isHelpShowMode == true)
        {
            isHelpShowMode = false;
        }
        else
        {
            isHelpShowMode = true;
        }
        return;
    }
    
    /**
     * �^�C�g�������p����ݒ肷��
     * 
     * @param searchString
     * @param searchTitleStatus
     * @param option
     */
    public void setSearchTitleInformation(String searchString, byte searchTitleStatus, int option, int searchHour)
    {
        // �X���^�C���o����...
        searchTitlePickupHour = searchHour;
        
        // ���ǃX���ꗗ���[�h�ɐ؂�ւ���
        if ((option == OPTION_ONLYDATABASE)&&(subjectParser.getOnlyDatabaseAccess() == false))
        {
            subjectParser.setOnlyDatabaseAccess(true);
            searchTitleStatus = SEARCHING_ONLYDB;
        }
        else
        {
            subjectParser.setOnlyDatabaseAccess(false);
        }
        
        // �啶��/�������̋�ʂɂ��Ĕ��f������
        if (option <= OPTION_NOTIGNORE_CASE)
        {
            searchIgnoreCase = false;
        }
        else
        {
            searchIgnoreCase = true;            
        }

        if ((searchString == null)||(searchString.compareTo("") == 0))
        {
            // �^�C�g����������������
            searchTitleString = null;
        }
        else
        {
            // �^�C�g��������ݒ肷��
            searchTitleString = searchString;
            searchTitleStatus = SEARCHING_MODE;
        }
        searchTitleMessageStatus = searchTitleStatus;

        // �X���^�C�g���ꗗ���ĕ`�悷��
        readySubjectList();
        return;
    }
    
    /**
     *  �^�C�g��������������N���A����
     *
     */
    public void clearSearchTitleString()
    {
        // �^�C�g��������������������
        searchTitleString = null;
        return;
    }

    /** 
     *  ���O�f�[�^��(����)�擾
     * 
     *
     */
    private void getLogDataFileImmediate()
    {
        currentSubjIndex = getSelectedIndex();
        if (currentSubjIndex < 0)
        {
            // �C���f�b�N�X�ԍ������������A�A�A�I������
            return;
        }

        // �I�����ꂽ�T�u�W�F�N�g�C���f�b�N�X���L������
        parent.setSelectedSubjectIndex(currentSubjIndex);

        if (currentScene != SHOW_SUBJECTLIST)
        {
            // �X���ꗗ�\�������Ă��Ȃ������A�A�I������
            return;
        }

        // �\�����[�h���m�F�A�󋵂ɍ��킹�ăX�����擾����
        isOpenImmediate = false;
        if (currentFavoriteLevel < 0)
        {
            // �X����Ԃ��擾����
            int  status  = subjectParser.getStatus(currentSubjIndex);

            if ((status == a2BsubjectDataParser.STATUS_NOTYET)||(status == a2BsubjectDataParser.STATUS_RESERVE))
            {
                // �X�����擾(�X���S�擾�����{)
                getLogDataFile(currentSubjIndex, GETMODE_GETALL);
                return;
            }

            // �����擾...
            getLogDataFile(currentSubjIndex, GETMODE_GETPART);
        }
        else
        {
            // ���C�ɓ��胂�[�h�̂Ƃ��A�A�A
            byte  status  = favorManager.getStatus(currentSubjIndex);
            if ((status == favorManager.STATUS_NOTYET)||(status == favorManager.STATUS_RESERVE))
            {
                // �X�����擾(�X���S�擾�����{)
                getLogDataFileFavorite(currentSubjIndex, GETMODE_GETALL);
                return;
            }

            // �����擾...
            getLogDataFileFavorite(currentSubjIndex, GETMODE_GETPART);
        }
        return;        
    }

    
    /**
     *  �֘A�L�[���[�h�\��...
     * 
     *  @param  threadUrl  �X����URL (read.cgi�o�R�̂���)
     */
    private void showRelatedKeyword(String theadUrl)
    {
        gettingLogDataURL = "http://p2.2ch.io/getf.cgi?" + theadUrl;
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.GetHttpCommunication(null, gettingLogDataURL, null, "�֘A�L�[���[�h", 0, 0, parent.SCENE_GET_RELATE_KEYWORD, false);
                parent.unlockCommunicationScreen();   // BUSY�X�N���[���̃��b�N���͂���
            }
        };
        // BUSY�X�N���[���̃��b�N������
        parent.lockCommunicationScreen();
        parent.showBusyMessage("", "�֘A�L�[���[�h���o��...", -1);
        thread.start();
    }

    /**
     *  ��M�����֘A�L�[���[�h�̉�͂��s��
     *
     */
    public void parseRelateKeyword(byte[] dataString)
    {
        pickup2chList = null;
        pickup2chList = new Vector();

        int index = 0;
        while ((index >= 0)&&(index < dataString.length))
        {
            index = pickupRelatedKeyword(dataString, index);
        }

        if (pickup2chList.isEmpty() != true)
        {
            currentScene = parent.CONFIRM_SELECT_RELATE_KEYWORD;
            parent.openSelectForm("�֘A�L�[���[�h", pickup2chList, 0, currentScene);
        }
        else
        {
            // �֘A�L�[���[�h�̌��o�Ɏ��s�����ꍇ...
            // 1�����Ђ�������Ȃ������ꍇ...
            showInformationMode = 0;
            informationString = "�֘A�L�[���[�h:������܂���";
            repaint();
        }

        // �K�x�R�����{...
        System.gc();
        return;
    }

    /**
     * �֘A�L�[���[�h�����ŏE�����A2ch�����pURL���Ƃ��Ă���
     * 
     * @param buffer
     * @param startIndex
     * @return
     */
    private int pickupRelatedKeyword(byte[] buffer, int startIndex)
    {
        boolean       pickupStr  = false;
        int threadNumIndexStart   = startIndex;
        int threadNumIndexEnd     = startIndex;
        int threadTitleIndexStart = startIndex;
        int threadTitleIndexEnd   = startIndex;
        int endIndex = buffer.length;
        try
        {
            for (int index = startIndex; index < buffer.length; index++)
            {
/**
                if ((buffer[index + 0] == 'h')&&(buffer[index + 1] == 't')&&
                    (buffer[index + 2] == 't')&&(buffer[index + 3] == 'p')&&
                    (buffer[index + 4] == ':')&&(buffer[index + 5] == '/')&&
                    (buffer[index + 6] == '/')&&(buffer[index + 7] == 'f')&&
                    (buffer[index + 8] == 'i')&&(buffer[index + 9] == 'n')&&
                    (buffer[index + 10] == 'd')&&(buffer[index + 11] == '.'))
**/
                if ((buffer[index + 0] == 'S')&&(buffer[index + 1] == 'T')&&
                    (buffer[index + 2] == 'R')&&(buffer[index + 3] == '='))
                {
                    // "http://find." �������� (�Q�����˂錟���p��URL)
                    threadNumIndexStart = index;
                    threadNumIndexEnd   = buffer.length;
                    for (int numberIndex =threadNumIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '"')
                         {
                            index = numberIndex - 1;
                            threadNumIndexEnd = numberIndex;
                            break;
                         }
                    }
                    pickupStr = true;
                }
/**
                else if ((buffer[index + 0] == '_')&&(buffer[index + 1] == 'b')&&
                          (buffer[index + 2] == 'l')&&(buffer[index + 3] == 'a')&&
                          (buffer[index + 4] == 'n')&&(buffer[index + 5] == 'k')&&
                          (buffer[index + 6] == '"')&&(buffer[index + 7] == '>'))
**/
                else if ((pickupStr == true)&&(buffer[index + 0] == '"')&&(buffer[index + 1] == '>'))
                 {
                    // �X���^�C�g���B�B�B �i�t�H�[�}�b�g���ς��Ǝ蒼�����K�v����...�j
                    threadTitleIndexStart = index + 2;
                    for (int titleIndex =threadTitleIndexStart; titleIndex < buffer.length; titleIndex++)
                    {
                        if ((buffer[titleIndex + 0] == '<')&&(buffer[titleIndex + 1] == '/')&&
                            (buffer[titleIndex + 2] == 'a')&&(buffer[titleIndex + 3] == '>'))
                         {
                            threadTitleIndexEnd = titleIndex;
                            break;
                         }
                    }

                    // �����Ń^�C�g����URL���G���g������
                    if ((threadNumIndexStart < threadNumIndexEnd)&&
                        (threadTitleIndexStart < threadTitleIndexEnd)&&
                        (threadNumIndexStart < threadTitleIndexStart))
                    {
                        String title  = parent.sjToUtf8(buffer, threadTitleIndexStart, (threadTitleIndexEnd - threadTitleIndexStart));
                        String number = new String(buffer, threadNumIndexStart, (threadNumIndexEnd - threadNumIndexStart));
                        title = title + "  <>" + number;
                        pickup2chList.addElement(title);
                    }
                    endIndex = threadTitleIndexEnd;
                    break;
                }
            }
        }
        catch (Exception e)
        {
            endIndex = -1;
        }
        return (endIndex);        
    }
    
    /**
     *   ���O�f�[�^�̎擾
     * 
     */
    public void getLogDataFile(int subjIndex, int getMode)
    {
        // �擾�您��ю擾�t�@�C�����𐶐�����
        int boardIndex = parent.getCurrentBbsIndex();
        String url      = bbsCommunicator.getBoardURL(boardIndex);
        String fileName = subjectParser.getThreadFileName(subjIndex);
        int pos = url.lastIndexOf('/', url.length() - 2);
        gettingLogDataFile = url.substring(pos + 1) + fileName;
        gettingLogDataURL  = url + "dat/" + fileName;

        // �X���^�C�̎擾
        gettingLogDataTitle = "'" + subjectParser.getSubjectName(subjIndex) + "' ";
        
        currentScene = SHOW_SUBJECTLIST;
        currentSubjIndex = subjIndex;

        if (getMode == GETMODE_GETPART)
        {
            // �����擾�����{����B�B�B
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, -1, -1, parent.SCENE_PARTGET_MESSAGE, true);
                }
            };
            thread.start();            
        }
        else if (getMode == GETMODE_GETALL)
        {
            // �S�擾�����{����B�B�B
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, 0, 0, parent.SCENE_WHOLEGET_MESSAGE, false);
                }
            };
            thread.start();
        }
        else
        {
            // �v���r���[���[�h
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(null, gettingLogDataURL, null, gettingLogDataTitle, 0, PREVIEW_LENGTH, parent.SCENE_PREVIEW_MESSAGE, false);
                }
            };
            thread.start();
        }
    }

    /**
     *   ���O�f�[�^�̎擾 (���C�ɓ���)
     * 
     */
    public void getLogDataFileFavorite(int index, int getMode)
    {
        String fileName = favorManager.getThreadNumber(index) + ".dat";
        gettingLogDataFile = favorManager.getBoardNick(index) + fileName;
        gettingLogDataURL  = favorManager.getUrl(index) + "dat/" + fileName;

        // �X���^�C�̎擾
        gettingLogDataTitle = "'" + favorManager.getSubjectName(index) + "' ";
        
        currentScene = SHOW_SUBJECTLIST;
        currentSubjIndex = index;

        if (getMode == GETMODE_GETPART)
        {
            // �����擾�����{����B�B�B
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, -1, -1, parent.SCENE_PARTGET_MESSAGE, true);
                }
            };
            thread.start();            
        }
        else if (getMode == GETMODE_GETALL)
        {
            // �S�擾�����{����B�B�B
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, 0, 0, parent.SCENE_WHOLEGET_MESSAGE, false);
                }
            };
            thread.start();
        }
        else
        {
            // �v���r���[���[�h
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(null, gettingLogDataURL, null, gettingLogDataTitle, 0, PREVIEW_LENGTH, parent.SCENE_PREVIEW_MESSAGE, false);
                }
            };
            thread.start();
        }
    }

    /**
     * �X���擾���������ꍇ...
     * @param result
     */
    public void completedGetLogDataFile(boolean result, int length)
    {
        currentScene = SHOW_SUBJECTLIST;

        if (currentFavoriteLevel < 0)
        {
            if (result == true)
            {
                // �ʏ�\�����[�h
                subjectParser.setStatus(currentSubjIndex, a2BsubjectDataParser.STATUS_NEW);
                subjectParser.setCurrentMessage(currentSubjIndex, 1, -1);
            }
            else
            {
                if (length > 0)
                {
                    subjectParser.setStatus(currentSubjIndex, a2BsubjectDataParser.STATUS_UPDATE);
                }
            }
            if (isOpenImmediate == true)
            {
                // �e�ɑ΂��A��ʂ𔲂��ăX����\�����邱�Ƃ�ʒm����
                parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, subjectParser.getCurrentMessage(currentSubjIndex));
            }
        }
        else
        {
            if (result == true)
            {
                // ���C�ɓ���\�����[�h�̃X�e�[�^�X���X�V����
                favorManager.setStatus(currentSubjIndex, favorManager.STATUS_NEW, 1, -1);
            }
            else
            {
                if (length > 0)
                {
                    favorManager.setStatus(currentSubjIndex, favorManager.STATUS_UPDATE, -1, -1);
                }
            }
            if (isOpenImmediate == true)
            {
                // �e�ɑ΂��A��ʂ𔲂��ăX����\�����邱�Ƃ�ʒm����
                parent.EndListFormShowMode(gettingLogDataFile, currentSubjIndex, favorManager.getCurrentMessage(currentSubjIndex));
            }
        }
        isOpenImmediate    = true;
        gettingLogDataFile = null;
        return;
    }

    /**
     *  �Q�����˂錟�������s���� (�ő�20��)
     */
    private void find2chThreadTitles()
    {       
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.GetHttpCommunication("temporary.html", gettingLogDataURL, null, "2ch����", 0, 0, parent.SCENE_GET_FIND_2CH, false);
                parent.unlockCommunicationScreen();   // BUSY�X�N���[���̃��b�N���͂���
            }
        };
        // BUSY�X�N���[���̃��b�N������
        try
        {
            parent.lockCommunicationScreen();
            parent.showBusyMessage("", "2ch������...", -1);
            thread.start();
        }
        catch (Exception ex)
        {
        	// 
        }
    }

    /**
     *  �Q�����˂錟�����g���āA�X���^�C�g������������ (�ő�20��)
     */
    public void find2chThreadNames(String titleToSearch)
    {
        gettingLogDataURL = "http://find.2ch.net/?BBS=ALL&TYPE=TITLE&STR=" + titleToSearch + "&COUNT=25";
        find2chThreadTitles();
    }

    /**
     * �Q�����˂錟���̌��ʂ���͂���
     * 
     * @param dataString
     */
    public void parseFind2ch(byte[] dataString)
    {
        byte [] dataBuffer = null;
        try
        {
            int firstIndex = 0;
            int endIndex   = dataString.length - 4;
            for (int i = 0; i < endIndex ; i++)
            {
                if ((dataString[i] == '<')&&(dataString[i + 1] == 'd')&&(dataString[i + 2] == 't')&&(dataString[i + 3] == '>'))
                {
                    firstIndex = i;
                    break;
                }
            }
            dataBuffer = new byte[dataString.length - firstIndex + 1];
            System.arraycopy(dataString, firstIndex, dataBuffer, 0, (dataString.length - firstIndex));
        }
        catch (Exception e)
        {
            dataBuffer = dataString;
        }

        pickup2chList = null;
        pickup2chList = new Vector();

        int index = 0;
        while ((index >= 0)&&(index < dataBuffer.length))
        {
            index = pickupFind2chData(dataBuffer, index);
        }

        if (pickup2chList.isEmpty() != true)
        {
            currentScene = parent.CONFIRM_SELECT_FIND2CH;
            parent.openSelectForm("2ch����", pickup2chList, 0, currentScene);
        }
        else
        {
            // 1�����Ђ�������Ȃ������ꍇ...
            showInformationMode = 0;
            informationString = "2ch����:������܂���";
            repaint();
        }
        System.gc();
        return;
    }

    /**
     * �Q�����˂錟���ŏE�����AURL���Ƃ��Ă���
     * 
     * @param buffer
     * @param startIndex
     * @return
     */
    private int pickupFind2chData(byte[] buffer, int startIndex)
    {
        int threadNumIndexStart   = startIndex;
        int threadNumIndexEnd     = startIndex;
//        int threadNickIndexEnd    = startIndex;
        int threadTitleIndexStart = startIndex;
        int threadTitleIndexEnd   = startIndex;
        int boardHostIndexStart   = startIndex;
//        int boardHostIndexEnd     = startIndex;
        int endIndex = buffer.length;

        try
        {
            boolean anchor = false;
            for (int index = startIndex; index < buffer.length; index++)
            {
                if ((buffer[index + 0] == ':')&&(buffer[index + 1] == '/')&&(buffer[index + 2] == '/'))
                {
                    boardHostIndexStart = index + 3;
                    for (int numberIndex = boardHostIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '/')
                        {
                            // �z�X�g���̖���
//                            boardHostIndexEnd = numberIndex;
                            index = numberIndex;
                            break;
                        }
                    }
                }
                else if ((buffer[index + 0] == 'r')&&(buffer[index + 1] == 'e')&&
                    (buffer[index + 2] == 'a')&&(buffer[index + 3] == 'd')&&
                    (buffer[index + 4] == '.')&&(buffer[index + 5] == 'c')&&
                    (buffer[index + 6] == 'g')&&(buffer[index + 7] == 'i')&&
                    (buffer[index + 8] == '/'))
                {
                    // "read.cgi/" ��������
                    threadNumIndexStart = index + 9;
                    boolean firstSlash = false;  // ���[�A���_���_�A�t���O�Ȃ�āA�A�A�B
                    for (int numberIndex =threadNumIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '/')
                         {
                            if (firstSlash == true)
                            {
                                threadNumIndexEnd = numberIndex;
                                index = numberIndex;
                                anchor = true;
                                break;
                            }
                            else
                            {
                                firstSlash = true;
  //                              threadNickIndexEnd = numberIndex;
                            }
                         }
                    }
               
                }

                //                else if ((buffer[index + 0] == '<')&&(buffer[index + 1] == 'b')&&(buffer[index + 2] == '>'))
                else if (anchor == true)
                {
                    while ((buffer[index] != '>')&&(index < buffer.length))
                    {
                        index++;
                    }
                    index++;
                    threadTitleIndexStart = index;
                    for (int titleIndex =threadTitleIndexStart; titleIndex < buffer.length; titleIndex++)
                    {
                        if ((buffer[titleIndex + 0] == '<')&&(buffer[titleIndex + 1] == '/')&&
                             (buffer[titleIndex + 2] == 'a')&&(buffer[titleIndex + 3] == '>'))
                        {
                            threadTitleIndexEnd = titleIndex; 
                            break;
                        }
                    }

                    // �����Ń^�C�g�����G���g������
                    if ((threadNumIndexStart < threadNumIndexEnd)&&
                        (threadTitleIndexStart < threadTitleIndexEnd)&&
                        (threadNumIndexStart < threadTitleIndexStart))
                    {
                        String title  = parent.eucToUtf8(buffer, threadTitleIndexStart, (threadTitleIndexEnd - threadTitleIndexStart));
                        String number = new String(buffer, threadNumIndexStart, (threadNumIndexEnd - threadNumIndexStart));
                        title = title + "<>" + number;
                        pickup2chList.addElement(title);
                    }
                    endIndex = threadTitleIndexEnd;
                    break;
                }
            }
            while ((buffer[endIndex] != 0x0a)&&(endIndex < buffer.length))
            {
                endIndex++;
            }
        }
        catch (Exception e)
        {
            endIndex = -1;
        }
        return (endIndex);        
    }
    /**
     * �Q�����˂錟���ŏE�����AURL���Ƃ��Ă���
     * 
     * @param buffer
     * @param startIndex
     * @return
     */
/*
    private int pickupFind2chData(byte[] buffer, int startIndex)
    {
        int threadNumIndexStart   = startIndex;
        int threadNumIndexEnd     = startIndex;
        int threadTitleIndexStart = startIndex;
        int threadTitleIndexEnd   = startIndex;
        int endIndex = buffer.length;
        try
        {
            for (int index = startIndex; index < buffer.length; index++)
            {
                if ((buffer[index + 0] == 'r')&&(buffer[index + 1] == 'e')&&
                    (buffer[index + 2] == 'a')&&(buffer[index + 3] == 'd')&&
                    (buffer[index + 4] == '.')&&(buffer[index + 5] == 'c')&&
                    (buffer[index + 6] == 'g')&&(buffer[index + 7] == 'i')&&
                    (buffer[index + 8] == '/'))
                {
                    // "read.cgi/" ��������
                    threadNumIndexStart = index + 9;
                    boolean firstSlash = false;  // ���[�A���_���_�A�t���O�Ȃ�āA�A�A�B
                    for (int numberIndex =threadNumIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '/')
                         {
                            if (firstSlash == true)
                            {
                                threadNumIndexEnd = numberIndex;
                                index = numberIndex;
                                break;
                            }
                            else
                            {
                                firstSlash = true;
                            }
                         }
                    }
               
                }
                else if ((buffer[index + 0] == '<')&&(buffer[index + 1] == 'b')&&(buffer[index + 2] == '>'))
                 {
                    threadTitleIndexStart = index + 3;
                    for (int titleIndex =threadTitleIndexStart; titleIndex < buffer.length; titleIndex++)
                    {
                        if ((buffer[titleIndex + 0] == '<')&&(buffer[titleIndex + 1] == '/')&&
                            (buffer[titleIndex + 2] == 'b')&&(buffer[titleIndex + 3] == '>'))
                         {
                            if ((buffer[titleIndex + 4] == '<')&&(buffer[titleIndex + 5] == '/')&&
                                (buffer[titleIndex + 6] == 'a')&&(buffer[titleIndex + 7] == '>'))
                            {
                                threadTitleIndexEnd = titleIndex;                                
                            }
                            else
                            {
                                threadTitleIndexStart = buffer.length;
                                threadTitleIndexEnd = buffer.length;
                            }
                            break;
                         }
                    }
                    // �����Ń^�C�g�����G���g������
                    if ((threadNumIndexStart < threadNumIndexEnd)&&
                        (threadTitleIndexStart < threadTitleIndexEnd)&&
                        (threadNumIndexStart < threadTitleIndexStart))
                    {
                        String title  = parent.eucToUtf8(buffer, threadTitleIndexStart, (threadTitleIndexEnd - threadTitleIndexStart));
                        String number = new String(buffer, threadNumIndexStart, (threadNumIndexEnd - threadNumIndexStart));
                        title = title + "<>" + number;
                        pickup2chList.addElement(title);
                    }
                    endIndex = threadTitleIndexEnd;
                    break;
                }
            }
        }
        catch (Exception e)
        {
            endIndex = -1;
        }
        return (endIndex);        
    }
*/

    /**
     * �X���ꗗ�̕\������
     * @param index
     * @param forceUpdate
     */
    public void prepareSubjectList(int index, boolean forceUpdate)
    {
        // �w�肳�ꂽ��index���L�^����
        currentBbsIndex = index;

        // ���C�ɓ���ꗗ���ǂ���
        currentFavoriteLevel = parent.getFavoriteShowLevel();            

        // �����X���ꗗ�擾�����{���邩�ǂ���
        isSubjectTxtUpdate = forceUpdate;

        // �X���ꗗ�̎擾
        Thread thread = new Thread()
        {
            public void run()
            {
                setTitle(" ");
                
                // �ꗗ���X�V����
                bbsCommunicator.refreshBoardInformationCache(parent.getCurrentCategoryIndex(), false, null);

                // �X���ꗗ�̏���...
                if (isSubjectTxtUpdate == true)
                {
                    subjectParser.prepareSubjectListUpdate(currentBbsIndex);
                }
                else
                {
                    subjectParser.prepareSubjectList(currentBbsIndex);
                }
                
                // �\���\�s��ݒ肷��
                setLineWidthHeight(getWidth(), getHeight());
                isSubjectTxtUpdate = false;
            }
        };
        setBusyMessage("subject.txt�X�V��...", true);
        thread.start();
    }

    /**
     *   �V���m�F�̎��s���J�n
     * 
     */
    public void getNewArrivalMessage()
    {
        // �V���m�F�p�̏��������s����
        subjectParser.prepareGetLogList(currentFavoriteLevel);
        return;
    }
    
    /**
     *  �V���m�F���ʂ𔽉f������
     * 
     */
    public void completedGetLogList(int length)
    {
        if (length > 0)
        {
            // �f�[�^�X�V
            subjectParser.setGetLogStatus(newArrialCheckIndex, a2BsubjectDataParser.STATUS_UPDATE);
        }
        else if (length < 0)
        {
            // �G���[����
            subjectParser.setGetLogStatus(newArrialCheckIndex, a2BsubjectDataParser.STATUS_GETERROR);            
        }
        return;
    }

    /**
     * �V���m�F�����{����...
     * 
      */
    public void startGetLogList(int mode)
    {
        // WX320K�`�F�b�N...
        String platForm = System.getProperty("microedition.platform");

        int actionMode = mode;
        if (actionMode == GETLOGLIST_START_RECURSIVE)
        {
            newArrialCheckIndex = 0;

            if (platForm.indexOf("WX320K") >= 0)
            {
                // WX320K�̂Ƃ��ɂ́A�X���b�h���Ŏ��X���b�h���N�������肵�Ȃ�
                actionMode = GETLOGLIST_START_THREAD;
            }
        }
        else // if (actionMode == GETLOGLIST_CONTINUE_RECURSIVE)
        {
            if (platForm.indexOf("WX320K") >= 0)
            {
                // WX320K�̂Ƃ��ɂ́A�I������B
                return;
            }
            newArrialCheckIndex++;
        }

        // �V���m�F�̎��{���C��...
        while (newArrialCheckIndex >= 0)
        {
            Thread thread = new Thread()
            {
                public void run()
                {
                    // ���̐V���m�F�����s����...
                    getLogList(newArrialCheckIndex);
                }
            };
            try
            {
                thread.start();

                if (actionMode == GETLOGLIST_START_THREAD)
                {
                    // WX320K�̂Ƃ��ɂ́A150ms��delay��݂���.(I/O Exception�΍�)
                    thread.join();
                    Thread.sleep(150);
                    thread = null;
                    System.gc();
                }
                else
                {
                    break;
                }
            }
            catch (Exception e)
            {
                //
            }
            newArrialCheckIndex++;
            thread = null;
//          System.gc();
        }
        return;
    }

    /**
     * �V���m�F�����{����
     * @param index
     */
    private void getLogList(int index)
    {
        long threadNumber = subjectParser.getThreadNumberGetLogList(index);
        if ((threadNumber < 0)||(isAbortNewArrival == true))
        {
            // �V���m�F�I��...
            currentScene = SHOW_SUBJECTLIST;
            subjectParser.clearGetLogList();
            
            //parent.showDialog(parent.SHOW_INFO, 0, "���", "�V���m�F���I�����܂����B");
            parent.endGetNewArrivalMessage();
            
            showInformationMode = 0;

            // �I�����b�Z�[�W�̐ݒ�
            if (isAbortNewArrival == true)
            {
                informationString = "�� �V���m�F���~ ��";
                isAbortNewArrival = false;
            }
            else
            {
                informationString = "�V���m�F�I��";                
            }
            newArrialCheckIndex = -10;
            repaint();
            System.gc();
            return;
        }

        // �擾�您��ю擾�t�@�C�����𐶐�����
        String url      = null;        
        if (currentFavoriteLevel < 0)
        {
            // �ʏ�擾���[�h
            int boardIndex = parent.getCurrentBbsIndex();        
            url = bbsCommunicator.getBoardURL(boardIndex);
        }
        else
        {
            // ���C�ɓ���\�����[�h
            url = subjectParser.getFavoriteUrlGetLogList(index);
        }
        if (url == null)
        {
            // ������莟�̐V���m�F��
            parent.CompletedToGetHttpData(parent.SCENE_GETALL_MESSAGE, 300, 0);
            return;
        }
        String fileName = threadNumber + ".dat";
        int pos = url.lastIndexOf('/', url.length() - 2);
        gettingLogDataFile = null;
        gettingLogDataFile = url.substring(pos + 1) + fileName;
        
        gettingLogDataURL  = null;
        gettingLogDataURL  = url + "dat/" + fileName;

        // �擾�ς݃��X�̔ԍ����m�F����
        int resNumber = subjectParser.getThreadResNumberGetLogList(index);
        if ((resNumber < 0)||(resNumber > 1000))
        {
            // ������莟�̐V���m�F��
            parent.CompletedToGetHttpData(parent.SCENE_GETALL_MESSAGE, 300, 0);
            return;
        }

        // �X���^�C�g�����擾����
        gettingLogDataTitle = "'" + subjectParser.getSubjectNameGetLogList(index) +"' ";

        // http�ʐM�i�S�擾/�����擾)�����{����
        byte status = subjectParser.getThreadStatusGetLogList(index);
        if ((status == a2BsubjectDataParser.STATUS_NOTYET)||(status == a2BsubjectDataParser.STATUS_RESERVE))
        {
            // �X���S�擾�����{����
            parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, 0, 0, parent.SCENE_GETRESERVE_MESSAGE, false);
        }
        else
        {
            // �X�������擾�����{����
            parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, gettingLogDataTitle, -1, -1, parent.SCENE_GETALL_MESSAGE, true);
        }

        // �K�x�R�����{...
        System.gc();
        return;
    }

    /**
     *   �X���ꗗ�̕\���������ł����Ƃ�...
     * 
     * 
     */
    public void readySubjectList()
    {
        // �J�[�\����擪�ɂ����Ă���
        currentSelectedLine  = 0;
        previousSelectedLine = 0;
        listCount            = 0;

        nofSubjectIndex     = 0;
        listDrawMode        = FORCE_REDRAW;

        // �\�����X�g���X�V����
        updateTitleList(FORCE_REDRAW);
        
        // ��ʂ��X�V����
        setBusyMessage(null, false);

        // �K�x�R�����{...
        System.gc();
        return;
    }

    /**
     *   �����F����������
     * 
     * 
     */
    public int getForeColor(byte status, int line)
    {
        int  color  = 0x00000000;
        switch (status)
        {
          case 0:  // subjectParser.STATUS_NOTYET:
            color   = 0x00777777;    // �O���[
            break;

          case 1:  // subjectParser.STATUS_NEW:
//            color   = 0x0000c8c8;   // ��
            color   = 0x000080ff;   // ��
            break;

          case 2:  // subjectParser.STATUS_UPDATE:
            color   = 0x001414c8;   // ��
            break;

          case 3:  // subjectParser.STATUS_REMAIN:
//            color   = 0x0037a637;   // ��
            color   = 0x00007000;   // ��
            break;

          case 4:  // subjectParser.STATUS_ALREADY:
            color   = 0x00000000;   // ��
            break;

          case 5:  // subjectParser.STATUS_RESERVE:
            color   = 0x00804000;   // ���F
            break;

          case 6:  // subjectParser.STATUS_UNKNOWN:
            color   = 0x00808020; // ���F...
            break;

          case 7:  // subjectParser.STATUS_GETERROR:
            color = 0x00eb0000;   // ��
            break;

          case 8:  // subjectParser.STATUS_OVER:
            color   = 0x00eb0aeb; // ��
            break;

          default:
            color   = 0x00828282;   // �O���[
            break;
        }
        return (color);
    }

    /**
     *   �w�i�F����������
     * 
     * 
     */
    public int getBackColor(byte status, int line)
    {
        if ((line % 2) == 0)
        {
            return (0x00f8f8f8);
        }
        else
        {
            return (0x00d8d8ff);
        }
/*
        if (subjectTitles <= index)
        {
            return (0x00f8f8f8);
        }
        int  color  = 0;
        byte status = getStatus(index);
        switch (status)
        {
          case 0:  // subjectParser.STATUS_NOTYET:
            color = 0x00f8f8f8;
            break;

          case 1:  // subjectParser.STATUS_NEW:
            color = 0x00f8f8f8;
            break;

          case 2:  // subjectParser.STATUS_UPDATE:
            color = 0x00f8f8f8;
            break;

          case 3:  // subjectParser.STATUS_REMAIN:
            color = 0x00f8f8f8;
            break;

          case 4:  // subjectParser.STATUS_ALREADY:
            color = 0x00f8f8f8;
            break;

          case 5:  // subjectParser.STATUS_RESERVE:
            color = 0x00f8f8f8;
            break;

          case 6:  // subjectParser.STATUS_UNKNOWN:
            color = 0x00f8f8f8;
            break;

          case 7:  // subjectParser.STATUS_GETERROR:
            color = 0x00f8f8f8;
            break;

          case 8:  // subjectParser.STATUS_OVER:    
            color = 0x00f8f8f8;
            break;

          default:
            color = 0x00f8f8f8;
            break;
        }
        return (color);
*/
    }

    /**
     *   �X���^�C�g�����X�V����
     * 
     * @param title
     */
    public  void updateThreadTitle(byte[] title, int offset, int length)
    {
        if (currentFavoriteLevel < 0)
        {
            // ���C�ɓ��肶�Ⴀ�Ȃ��Ƃ������X�V����
            subjectParser.updateThreadTitle(currentSubjIndex, title, offset, length);
        }
        else
        {
            // ���C�ɓ���̏ꍇ...�X�V����
            favorManager.updateThreadTitle(currentSubjIndex, title, offset, length);
        }
        return;
    }
    
    /**
     *   ���ݕ\�����X���ƍő僌�X����ݒ�
     *   
     */
    public void setCurrentMessage(int number, int maxNumber)
    {
        // ���C�ɓ���o�^�X���A������̏�Ԃ��X�V���� (���ǁA�o�b�N�A�b�v�͂��Ȃ�...)
        String url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
        int   pos  = url.lastIndexOf('/', url.length() - 2);

        if (currentFavoriteLevel < 0)
        {
            // �Ǘ��X����\�����ɎQ�Ɖ�ʂ��甲���Ă����ꍇ�́A���X�ԍ���DB�ɋL�^����
            subjectParser.setCurrentMessage(currentSubjIndex, number, maxNumber);
            if (subjectParser.isFavorite(currentSubjIndex) == true)
            {
                long  threadNumber = subjectParser.getThreadNumber(currentSubjIndex);
                int favorIndex = favorManager.exist(url.substring(pos + 1), threadNumber);
                if (favorIndex >= 0)
                {
                    // ��Ԃ����߂�B�B�B
                    byte status = favorManager.STATUS_ALREADY;
                    if (number < maxNumber)
                    {
                        status = favorManager.STATUS_REMAIN;
                    }
                    if (number > 1000)
                    {
                        status = favorManager.STATUS_OVER;
                    }
                    if (maxNumber < 0)
                    {
                        status = favorManager.STATUS_NEW;
                    }

                    // ���C�ɓ���\�����[�h�̃X�e�[�^�X���X�V����
                    favorManager.setStatus(favorIndex, status, number, maxNumber);            
                    
                    // ���C�ɓ�������o�b�N�A�b�v����
                    //favorManager.backup();
                }
            }            
        }
        else
        {
            // ��Ԃ����߂�B�B�B
            byte status = favorManager.STATUS_ALREADY;
            if (number < maxNumber)
            {
                status = favorManager.STATUS_REMAIN;
            }
            if (number > 1000)
            {
                status = favorManager.STATUS_OVER;
            }
            if (maxNumber < 0)
            {
                status = favorManager.STATUS_NEW;
            }

            // ���C�ɓ���\�����[�h�̃X�e�[�^�X���X�V����
            favorManager.setStatus(currentSubjIndex, status, number, maxNumber);            

            // ���C�ɓ�������o�b�N�A�b�v����
            if (parent.getIgnoreFavoriteBackup() != true)
            {
                favorManager.backup();
            }
            
            // �X���ꗗ���\�z�ς݂������ꍇ...
            String nick = favorManager.getBoardNick(currentSubjIndex);
            if (nick.equals(url.substring(pos + 1)) == true)
            {
                // �\�z�ςݔꗗ�̂ق����X�V����
                long  threadNumber = favorManager.getThreadNumber(currentSubjIndex);
                subjectParser.setCurrentMessage(threadNumber, number, maxNumber);
            }
        }
        return;
    }
    
    /**
     *   ���ݎQ�ƒ��̃X���C���f�b�N�X����������
     * 
     */
    public int getCurrentMessageIndex()
    {
        return (currentSubjIndex);
    }

    /**
     *   ���ݎQ�ƒ��̃X���C���f�b�N�X���L�^����
     * 
     */
    public void setCurrentMessageIndex(int index)
    {
        currentSubjIndex = index;
    }
    
    /**
     * �i���o�[�ړ����[�h
     * 
     * @param mode
     */
    public void setNumberInputMode(boolean mode)
    {
        isNumberMovingMode = mode;
        return;
    }
    
    /**
     *   ���ݎQ�ƒ����X�������[�h����
     * 
     */
    public void reloadCurrentMessage()
    {
        if (currentFavoriteLevel < 0)
        {
            // �ʏ�ꗗ�\�����[�h
            getLogDataFile(currentSubjIndex, GETMODE_GETPART);
        }
        else
        {
            // ���C�ɓ���ꗗ�\�����[�h
            getLogDataFileFavorite(currentSubjIndex, GETMODE_GETPART);
        }
        return;
    }
    
    /**
     *  ���ݎQ�ƒ����X�̔�URL��Ԃ�
     * 
     */
    public String getCurrentMessageBoardUrl()
    {
        String url;
        if (currentFavoriteLevel < 0)
        {
            // ��URL���擾���� (�ʏ��)
            url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
        }
        else
        {
            // ��URL���擾���� (���C�ɓ���)
            url  = favorManager.getUrl(currentSubjIndex);
        }
        return (url);
    }

    /**
     *  �w���v�C���[�W��ǂݍ���
     * 
     * 
     */
    public void loadHelpImage()
    {
        helpImage = parent.loadImageFromFile("a2B_ListHelp.png");
        return;
    }
}
//--------------------------------------------------------------------------
//  a2BmessageListForm  --- �ꗗ���
//
//
//--------------------------------------------------------------------------
//   MRSa (mrsa@sourceforge.jp)
