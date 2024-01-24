import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

import java.io.InputStream;
import java.util.Date;
import java.util.Vector;

/**
 * �X���Q�Ɖ�ʂ̕\���N���X
 * 
 * @author MRSa
 *
 */
public class a2BMessageViewForm extends Canvas implements CommandListener
{
    private a2BMain              parent          = null;
    private a2Butf8Conveter      stringConverter = null;
    private a2BMessageParser     messageParser   = null;
    private a2BsubjectDataParser subjectParser   = null;
    private a2BNgWordManager     ngWordManager   = null;
    private String               parsedString    = null;
    private Vector               msgLines        = null;
    private Vector               commandList     = null;
    private a2BmonaFontWidth     monaFontWidth   = null;

    private int                 zoneScreenCount = 1;   // ���X�܂Ƃߓǂ݌�

    // �R�}���h�Q...
    private Command rtnCmd    = new Command("�ꗗ��",         Command.EXIT, 0);
    private Command quitCmd   = new Command("�I��",           Command.SCREEN, 13);
    private Command outputCmd = new Command("���X�o��",       Command.SCREEN, 12);
    private Command outFilCmd = new Command("�o��File���ύX", Command.SCREEN, 11);
    private Command ngEntCmd  = new Command("NG�o�^",         Command.SCREEN, 10);
    private Command ngCmd     = new Command("NG�ؑ�",         Command.SCREEN, 9);
    private Command copyCmd   = new Command("���X�ҏW",       Command.SCREEN, 8);
    private Command partCmd   = new Command("�V���m�F",       Command.SCREEN, 3);
    private Command writeCmd  = new Command("��������",       Command.SCREEN, 2);
    private Command modeCmd   = new Command("���[�hMenu",     Command.SCREEN, 4);
//    private Command chgCmd    = new Command("Font�ؑ�",       Command.SCREEN, 7);
//    private Command scrCmd    = new Command("��ʃT�C�Y",     Command.SCREEN, 6);
//    private Command infoCmd   = new Command("�X�����",       Command.SCREEN ,5);
//    private Command backCmd   = new Command("�߂�",           Command.SCREEN, 3);

    private long       repeatingStartDate   = 0;
    private int        repeatingKeyCode     = 0;
    private int        currentMessageIndex  = 0;
    private int        messageNumber        = 1;
    private int[]      messageJumpList      = null;
    private int[]      pickupMessageList    = null;
    private int        jumpListIndex        = 0;    
    private int        currentPage          = 0;
    private int        screenLines          = 0;
    private int        msgPage              = 0;
    private int        buildNofPickupList   = 0;
    private int        currentPickupList    = -1;
    private Font       screenFont           = null;
    private int        informationColor     = 0;
    private int        triangleColor        = 0;
    private int        crossColor           = 0;
    private char       informationChar      = 0;
    private int        commandJumpNumber    = 0;
    private int        showXAxisforAAmode   = 0;
    private Font       fontKeepAASize        = null;
    private int        keepAAscreenLines    = 0;
    private int        messageBackColor     = 0x00f0f0f0;
    private int        messageForeColor     = 0x00000000;
    private int        searchDirection      = 0;
    private int        ngWordCheckDirection = 0;
    private int        screenHeight         = 0;
    private int        screenWidth          = 0;
    private int        AA_WIDTH_MARGIN      = 0;
    private int        currentGwtPageNum    = -1;
    private Image      wallPaperImage       = null;
    private Image      viewHelpImage        = null;
    private Image      jpegPreviewImage     = null;
    private String     searchMessageString  = null;
    private String     reservedThreadNumber = null;
    private String     previewURL           = null;
    private String     displayFileName      = null;
    private boolean   searchIgnoreCase     = false;
//    private boolean   isFullScreen         = false;
    private boolean   isCommandMode        = false;
    private boolean   painting             = false;
    private boolean   buildingMessage      = false;
    private boolean   checkNgWord          = false;
//    private boolean   doAbone              = false;
    private boolean   isHelpShowMode       = false;
    private boolean   isNumberMovingMode   = false;
    private boolean   isAsciiArtShowMode   = false;
    private boolean   isSearchingMode      = false;
    private boolean   isSearchExecution    = false;
    private boolean   isFirstTime          = true;
    private boolean   drawInformation      = false;
    private boolean   isMax                = false;
    private boolean   previewJpeg          = false;
    private boolean   previewText          = false;
    private boolean   useCacheDirectory    = false;

    private final int nofJumpList          = 24;
    private final int nofPickupMessageList = 16;
    private final int  LIMIT_WIDTH         = 999;

    private final int  PARSING_NORMAL      = 0;
    private final int  PARSING_RAW         = 1;
    private final int  PARSING_HTML        = 2;
//    private final int  PARSING_A2B_MEMO    = 3;
    private int        messageParsingMode   = PARSING_NORMAL;
    private String      busyMessage         = null;  // ���b�Z�[�W�\��...
    private String      busyInformation      = null;
    private long       keyRepeatDelayMs    = 400;    // �L�[���s�[�g�J�n�܂ł̎���(ms)

    public boolean    lockOperation         = false;  // ��ʑ���̋֎~�t���O...

    /**
     * �R���X�g���N�^
     * 
     * @param arg0
     * @param object
     * @param subject
     * @param utf8Converter
     * @param ngManager
     */
    public a2BMessageViewForm(String arg0, a2BMain object, a2BsubjectDataParser subject, a2Butf8Conveter utf8Converter, a2BNgWordManager ngManager) 
    {
        // �^�C�g��
        setTitle(arg0);

        parent           = object;
        subjectParser    = subject;
        stringConverter  = utf8Converter;
        ngWordManager    = ngManager;
        messageParser    = new a2BMessageParser(stringConverter);
        messageParser.setConvertHanZen(utf8Converter.isConvertHanZen());
        fontKeepAASize   = Font.getDefaultFont();
        messageJumpList   = new int[nofJumpList];
        jumpListIndex     = 0;

        pickupMessageList  = new int[nofPickupMessageList];
        buildNofPickupList = 0;
        currentPickupList  = -1;

        // �R�}���h��ǉ�����
        this.addCommand(writeCmd);
        this.addCommand(copyCmd);
        this.addCommand(modeCmd);
        this.addCommand(partCmd);
        this.addCommand(ngEntCmd);
        this.addCommand(ngCmd);
        this.addCommand(outFilCmd);
        this.addCommand(outputCmd);
        this.addCommand(quitCmd);
        this.addCommand(rtnCmd);
        this.setCommandListener(this);
    }

    /*
     *   �t�H���g�Ɖ�ʃT�C�Y��ݒ肷��
     * 
     * 
     */
    public void prepareScreenSize()
    {
        // ���i�[�t�H���g�������N���X��p�ӂ���
        monaFontWidth = new a2BmonaFontWidth();
        
        // �t�H���g�Ɖ�ʃT�C�Y�A�t�H���g�F���擾���A�ݒ肷��
        int viewFontSize = parent.getViewFontSize();
        screenFont       = Font.getDefaultFont();
        try
        {
            screenFont       = Font.getFont(screenFont.getFace(), screenFont.getStyle(), viewFontSize);
        }
        catch (Exception ex)
        {
            screenFont       = Font.getDefaultFont();
        }
        messageForeColor = parent.getViewForeColor();
        messageBackColor = parent.getViewBackColor();

        // �t���X�N���[�����[�h��ݒ肷��
        setFullScreenMode(parent.getViewScreenMode());

        // NG���[�h���o�ݒ�𔽉f������
        checkNgWord = parent.getNgWordDetectMode();
        
        // ���X�܂Ƃߓǂ݃��[�h�̒l���擾����        
        zoneScreenCount = parent.getResBatchMode();
        
        AA_WIDTH_MARGIN = parent.getAAwidthMargin();
        return;
    }
    
    /**
     * AA���[�h�̃}�[�W����ݒ肷��
     * 
     * @param margin
     */
    public void setAAmodeWidthMargin(int margin)
    {
        AA_WIDTH_MARGIN = margin;
        return;
    }
    
    
    /**
     *  ��ʃX�N���[���T�C�Y��ݒ肷��B�B�B
     *
     */
    public void setupScreenSize()
    {
        // �^�C�g���̐ݒ�...
        setTitle(" ");
        
         // �\���\�s��ݒ肷��
        setLineWidthHeight(getWidth(), getHeight());

        return;
    }

    /**
     *  ��ʕ`��̃��C��
     * 
     * 
     */
    public void paint(Graphics g)
    {
        // ��ʕ`�撆�̂Ƃ��ɂ͐܂�Ԃ�
        if (painting == true)
        {
            return;
        }

        // ���b�Z�[�W�\�z���̎��ɂ͐܂�Ԃ�
        if (buildingMessage == true)
        {
            return;
        }
        
        // �\���f�[�^���Ȃ��ꍇ�ɂ͐܂�Ԃ�
        if ((msgLines == null)||(msgLines.size() == 0))
        {
            // ���s���C���t�H���[�V������\������...
            if (busyInformation != null)
            {
                g.setColor(0x00d8ffd8);
                g.fillRect(0, screenHeight + 1, getWidth(), getHeight() - (screenHeight + 1));
                g.setFont(screenFont);
                g.setColor(0);
                g.drawString(busyInformation, 0, (screenHeight + 1), (Graphics.LEFT | Graphics.TOP));

/**
                // �^�񒆂ɕ\������Ɩڗ��̂�...
                int y = screenHeight / 2;
//                g.setColor(0x00ffdddd);
                g.setColor(0x00ffffd2);
                g.fillRect(10, (y - 3), (screenWidth - 20), (screenFont.getHeight() + 6));
                g.setColor(0);
                g.drawRect(10, (y - 3), (screenWidth - 20), (screenFont.getHeight() + 6));
                g.drawString(busyInformation, 20, y, (Graphics.LEFT | Graphics.TOP));
**/
            }
            return;
        }

        // WX310�p���[�N�A���E���h...
        if (isFirstTime == true)
        {
            isFirstTime = false;

            // �\���\�s��ݒ肷��
            setLineWidthHeight(getWidth(), getHeight());        
        }
        
        // �R�}���h���[�h (���X�W�����v�ԍ����̓��[�h)���H
        if (isCommandMode == true)
        {
            showCommandWindow(g);
            return;
        }
        
        // �`��J�n
        painting = true;

        // ��ʂ̓h��Ԃ�(��ʃN���A)
        g.setColor(messageBackColor);
        g.fillRect(0, 0, getWidth(), getHeight());
        
        // �ǎ����������ꍇ...
        if (wallPaperImage != null)
        {
            // ��ʌ��_����C���[�W��`�悷��
            g.drawImage(wallPaperImage, 0, 0, (Graphics.LEFT | Graphics.TOP));            
        }

        // �`��t�H���g��ݒ肷��
        g.setColor(messageForeColor);
        g.setFont(screenFont);

        try
        {
            // ���X��\������
            drawMessageMain(g);

            // ��ʂɃ}�[�N������
            drawMarkScreen(g);
        }
        catch (Exception e)
        {
            // �������Ȃ�...
        }

        // BUSY���b�Z�[�W�̕\��
        {
            g.setColor(0x00d8ffd8);
            g.fillRect(0, screenHeight + 1, getWidth(), getHeight() - (screenHeight + 1));
            if (busyMessage != null)
            {
                g.setColor(0);
                g.drawString(busyMessage, 0, (screenHeight + 1), (Graphics.LEFT | Graphics.TOP));
            }
        }
        painting = false;

        return;
    }
    
    /**
     * ���X��`�悷��(���C������)
     * 
     * @param g
     */
    private void drawMessageMain(Graphics g)
    {
        int   line   = 0;
        int   start  = 0;
        String msg   = null;

        // topPointer�������������ꍇ�ɂ͖�������A������������擪����
        if (currentPage < 0)
        {
            currentPage = msgPage;
        }

        // �擪�y�[�W��\������
        if (currentPage == 0)
        {
            if (messageParsingMode == PARSING_NORMAL)
            {
                if (zoneScreenCount == 1)
                {
                    // ���X�ԍ���\�� (�����̓}�[�W�����Ƃ炸�O����...)
                    g.setColor(messageForeColor);
                    String dispData = "[" + messageNumber + "]  ";
                    g.drawString(dispData, 1, 0, (Graphics.LEFT | Graphics.TOP));
                    int x = screenFont.stringWidth(dispData) + 1;

                    // ���O����\��
                    g.setColor(0x00008000);
                    dispData = messageParser.getNameField();
                    g.drawString(dispData, x, 0, (Graphics.LEFT | Graphics.TOP));
                    x = x + screenFont.stringWidth(dispData + "           ");
                    
                    // email����\�� (1dot���}�[�W������)
                    g.setColor(0x00fa2020);
                    dispData = messageParser.getEmailField();
                    g.drawString(dispData, 1, (screenFont.getHeight() + 1), (Graphics.LEFT | Graphics.TOP));
                }
                else
                {
                    // ���X�̐擪�s��\�� (1dot���}�[�W������)
                    g.setColor(messageForeColor);
                    g.drawString((String) msgLines.elementAt(0), 1, 0, (Graphics.LEFT | Graphics.TOP));
                }
                line++;
            }
        }
        else
        {
            start = currentPage * (screenLines - 1);
        }
        
        // �����̃t�H���g��ݒ肷��
        g.setColor(messageForeColor);
        if (isAsciiArtShowMode == true)
        {
            // AA���[�h�̕\��... (AA���[�h�̓}�[�W�����Ƃ炸��...)
            for (; ((line < screenLines)&&((line + start) < msgLines.size())); line++)
            {
                int y       = (line * (screenFont.getHeight())) + 4;
                int x       = 0;
                int limitX  = screenWidth + showXAxisforAAmode - 4;
                msg = (String) msgLines.elementAt(line + start);
                for (int msgIndex = 0; msgIndex < msg.length(); msgIndex++)
                {
                    char drawChar = msg.charAt(msgIndex);
                    if (x >= showXAxisforAAmode)
                    {
                        g.drawChar(drawChar, (x - showXAxisforAAmode), y, (Graphics.LEFT | Graphics.TOP));
                    }
                    x = x + monaFontWidth.getWidth(drawChar);
                    if (x > limitX)
                    {
                        // �\�����𒴂���...
                        break;
                    }
                }
            }
        }
        else
        {            
            // �f�[�^���s�P�ʂŕ\������ (1dot���}�[�W������)
            for (; ((line < screenLines)&&((line + start) < msgLines.size())); line++)
            {
                msg = (String) msgLines.elementAt(line + start);
                g.drawString(msg, 1, (line * (screenFont.getHeight() + 1)), (Graphics.LEFT | Graphics.TOP));
            }
        }
        if (isMax == true)
        {
            if (line < screenLines)
            {
                // �G���h�}�[�N�̕\��
                line--;
                g.setColor(0x00703070);
                g.drawString("--- E N D ---", 0, (line * (screenFont.getHeight() + 1)), (Graphics.LEFT | Graphics.TOP));
                g.setColor(0x00000000);
            }
            else if ((line + start) >= msgLines.size())
            {
                // �G���h�}�[�N�̕\��
                g.setColor(0x00703070);
                g.drawString("--- E N D ---", 0, (line * (screenFont.getHeight() + 1)), (Graphics.LEFT | Graphics.TOP));
                g.setColor(0x00000000);
            }
        }
        return;
    }
    
    /**
     *  ��ʂɃ}�[�N������
     *  
     * @param g
     */
    private void drawMarkScreen(Graphics g)
    {
        int markColor = 0;        
        int lineColor = 0;

        // NG���[�h���o�����{���Ă��邩�ǂ���...
        if (checkNgWord == true)
        {
            lineColor = 0x00eb0aeb; // ���F...
        }
        else
        {
            lineColor = 0;          // ���F...
        }

        // ���X�̂ǂ̂ւ�(x���AAA�\�����[�h)��\�����Ă���̂��\������
        if (isAsciiArtShowMode == true)
        {
            int width = screenWidth;
            width = (width * width) / (LIMIT_WIDTH + width);

            int startWidth = ((showXAxisforAAmode) / (screenWidth - AA_WIDTH_MARGIN)) * width;

            int y = (screenHeight - 3);
            g.setColor(255, 255, 255);
            g.fillRect(0, (screenHeight - 3), screenWidth,  3);
            g.setColor(lineColor);
            g.drawLine(0, y, screenWidth, y);
            g.setColor(0x00ff007f);
            g.setColor(markColor);
            g.fillRect(startWidth, y, width, 4);
         }

        // �������[�h���ǂ����H
        if (isSearchingMode == true)
        {
            if (isSearchExecution == true)
            {
                // �������܂��Ɏ��s���̂Ƃ�...
                markColor = 0x000000ff;
                
                // "������" ��\������
                showSearchingWindow(g);
            }
            else
            {
                // �������[�h�̂Ƃ��̓}�[�N�̐F��ԐF��
                markColor = 0x00ff0000;
            }
        }
        
        // ���X�̕\���ʒu(y��)��\������
        int line = (screenWidth - 3);
        g.setColor(255, 255, 255);
        g.fillRect(line, 0, line + 3, screenHeight);
        g.setColor(lineColor);
        g.drawLine(line, 0, line, screenHeight);

        int start = messageParser.getNumberOfMessages();
        if (start != 0)
        {
            int par =  messageNumber * screenHeight / start;
//            float par = ((float) messageNumber / (float) start) * ((float) screenHeight);
            start = (int) par - 2;
            if (start < 0)
            {
                start = 0;
            }
            g.setColor(markColor);
            g.fillRect(line, start, 3, 4);
        }
        
        // �C���t�H���[�V������񂪂������ꍇ...
        if (drawInformation == true)
        {
            if (informationColor != 0)
            {
                // �E��Ɂ��}�[�N������
                g.setColor(informationColor);
                g.fillArc((screenWidth - 15),1, 10,10, 0, 360);
                informationColor = 0;
                g.setColor(0);
            }
            if (triangleColor != 0)
            {
                // �E��Ɂ��}�[�N������
                g.setColor(triangleColor);
                g.fillTriangle((screenWidth - 10),1, (screenWidth - 15), 10, (screenWidth - 5), 10);
                triangleColor = 0;
                g.setColor(0);            
            }
            if (crossColor != 0)
            {
                // �E��Ɂ~�}�[�N������
                g.setColor(crossColor);
                g.drawLine((screenWidth - 12), 1, (screenWidth -  8), 10);
                g.drawLine((screenWidth - 8),  1, (screenWidth - 12), 10);
                crossColor = 0;
                g.setColor(0);            
            }
            if (informationChar != 0)
            {
            	// �E��ɕ�����\������
                g.setColor(0x00f000f0);
                g.drawChar(informationChar, (screenWidth - 12), 1, (Graphics.LEFT | Graphics.TOP));
                informationChar = 0;
                g.setColor(0);            
            }
            drawInformation = false;
            lockOperation   = false;
        }

        // JPEG�\�����[�h/�w���v�\�����[�h�̃`�F�b�N...
        if ((previewJpeg == true)&&(jpegPreviewImage != null))
        {
            // �v���r���[�C���[�W��\������
            showImage(g, jpegPreviewImage);
        }
        else if ((isHelpShowMode == true)&&(viewHelpImage != null))
        {
            // �w���v�\�����s��
            showImage(g, viewHelpImage);            
        }

        // �����L�[���̓��[�h..
        if (isNumberMovingMode == true)
        {
            g.setColor(0x00ff00ff);
            g.fillRect(0, 0, 2, 12);
            g.setColor(0);
        }

        return;
    }    
    
    /**
     * �w���v�̕\�����[�h���C��
     * 
     * @param g
     */
    private void showImage(Graphics g, Image viewImage)
    {
        // �C���[�W�̕\�����W�����߂�
        int x = ((screenWidth  - viewImage.getWidth())  / 2);
        if ((x + viewImage.getWidth()) > screenWidth)
        {
            x = 0;
        }

        int y = ((screenHeight - viewImage.getHeight()) / 2);
        if ((y + viewImage.getHeight()) > screenHeight)
        {
            y = 0;
        }
        
        // �w���v�C���[�W��`��
        g.setColor(0);
        g.drawImage(viewImage, x, y, (Graphics.LEFT | Graphics.TOP));

        return;
    }

    /**
     *   �v���r���[�E�B���h�E����������
     * @param keyCode
     */
    private void clearPreviewWindow(int keyCode)
    {
        boolean doDelete = false;

        // JPEG�t�@�C���E�e�L�X�g�f�[�^�̃v���r���[�e�L�X�g������
        if (keyCode == Canvas.KEY_NUM0)
        {
            // 0�L�[
            doDelete = true;
            currentGwtPageNum = -1;
        }

        // �t�@�C���������w�������������H
        if ((doDelete == true)&&(displayFileName != null))
        {
            // �t�@�C��������...
            parent.doDeleteFile(displayFileName);
        }
        displayFileName = null;

        busyMessage = null;
        jpegPreviewImage = null;
        previewJpeg = false;
        previewText = false;
        System.gc();
        repaint();
        
        return;
    }

    /*
     *   �L�[���͂���
     * 
     * 
     */
    public void keyPressed(int keyCode)
    {
        parent.keepWatchDog(0);

        // ���b�Z�[�W�\�z���ɂ̓L�[������󂯕t���Ȃ�
        if ((buildingMessage == true)||(lockOperation == true))
        {
            return;
        }
//        drawInformation = false;

        if ((previewJpeg == true)||(previewText == true))
        {
            clearPreviewWindow(keyCode);
            return;
        }

        boolean repaint   = true;
        repeatingKeyCode   = 0;
        repeatingStartDate = 0;
        if (keyCode == 0)
        {
            // �L�[���͂��Ȃ���ΐ܂�Ԃ�
            return;
        }
        if (isCommandMode == true)
        {
            // �������̓��[�h�̂Ƃ�...
            if (numberInputMode(keyCode) == true)
            {
                repaint();
                return;
            }
        }
        else if (isNumberMovingMode == true)
        {
            // �����ŃJ�[�\���ړ����[�h�̂Ƃ�...
            if (numberMovingMode(keyCode) == true)
            {
                repaint();
                return;
            }
        }
        else
        {
            // �ʏ�̐������̓��[�h�`�F�b�N
            if (numberKeyInMode(keyCode) == true)
            {
                repaint();
                return;
            }
        }

        // �J�[�\���L�[�̓��́B�B�B
        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            repaint = inputLeft();
            break;
              
          case Canvas.RIGHT:
            repaint = inputRight();
            break;

          case Canvas.DOWN:
            repaint = inputDown();
            break;

          case Canvas.UP:
            repaint = inputUp();
            break;

          case Canvas.FIRE:
            repaint = inputFire();
            break;

          case Canvas.GAME_A:
            // GAME-A�L�[ : �����L�[�ŃX���ړ����[�h�ɂ���
            repaint = inputGameA();
            break;

          case Canvas.GAME_B:
            // GAME-B�L�[�A�w���v���[�h����������
            repaint = inputGameB();
            break;

          case Canvas.GAME_C:
            // �N���A�L�[�F�O��W�����v�����Ƃ���ɖ߂�
            repaint = inputGameC();
            break;

          case Canvas.GAME_D:
            // �u���E�U�ŊJ��
            repaint = inputGameD();
            break;

          default:
            repaint = false;
            break;
        }
        if (repaint == true)
        {
            repaint();
        }
        return;
    }
    
    /**
     *  ���������͂��郂�[�h
     * 
     * @param keyCode
     * @return
     */
    private boolean numberKeyInMode(int keyCode)
    {
        boolean repaint = true;  // �ĕ`��

        // �ʏ탂�[�h���̃L�[���͏���...
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            // �@�\�̎��s�B�B�B
            executeFunctionFeature();
            break;

          case Canvas.KEY_NUM1:
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return (true);
            }
            // ���C�ɓ���ɓo�^����
            setFavoriteThread();
            break;

          case Canvas.KEY_NUM2:
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return (true);
            }
            // �������݉�ʂ�\���\�ȃX���������ꍇ�A�������݉�ʂɐ؂�ւ���
            if (parent.canWriteMessage() == true)
            {
                subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
                String title = stringConverter.parsefromSJ(messageParser.getThreadTitle());
                parent.OpenWriteForm(false, title, currentMessageIndex, messageNumber);
            }
            break;

          case Canvas.KEY_NUM3:
            //  �Q�Ɖ�ʂ��甲����
            endShowMessage();
            break;

          case Canvas.KEY_NUM4:
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return (true);
            }
            // �X������\������
            showMessageInformation();
            break;

          case Canvas.KEY_NUM5:
            // �X��������
              setSearchKeyword();
            break;

          case Canvas.KEY_NUM6:
            // AA�\�����[�h...
            if (isAsciiArtShowMode == true)
            {
                // AA���[�hOFF�ɐ؂�ւ���
                isAsciiArtShowMode = false;
                screenFont = fontKeepAASize;
                screenLines = keepAAscreenLines;
            }
            else
            {
                // AA���[�hON�ɐ؂�ւ���
                isAsciiArtShowMode = true;
                fontKeepAASize = screenFont;
                keepAAscreenLines = screenLines;
                screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_SMALL);
            }

            // ��ʃT�C�Y�̃Z�b�g�A�b�v...
            setLineWidthHeight(getWidth(), getHeight());

            // ���X�̐擪�ֈړ�
            currentPage = 0;

            // �f�[�^�����͂�����\������悤��
            showXAxisforAAmode = 0;

            showMessageRelative(0);
            break;

          case Canvas.KEY_NUM7:
            // ���X��URL���u���E�U��OPEN����
            subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
            repaint = launchWebBrowser();
            break;

          case Canvas.KEY_NUM8:
            // �u�X���ԍ��w��擾�v���s��...
            pickupThreadNumber();
            break;

          case Canvas.KEY_NUM9:
            // �����擾�����{...
            subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
            parent.reloadCurrentMessage();
            break;
            
          case 46:
          case Canvas.KEY_POUND:
            // ���X�̖����ֈړ�����
            jumpBottom();
            break;

          case 44:
          case Canvas.KEY_STAR:
            // ���X�̐擪�ֈړ�����
            jumpTop();
            break;

          case -5:
            // ����
            inputFire();
            break;

          case -8:
            // Nokia�̃N���A�L�[ (�������킹)
            // �O��W�����v�����Ƃ���ɖ߂�
            repaint = inputGameC();
            break;

          case -50:
            // Nokia��Func�L�[... (�w���v��\������)
            repaint = inputGameB();
            break;

          case 119:
          case  87:
            // �u���E�U�ŊJ�� (W)
            repaint = inputGameD();
            break;

          case 8:
            // BS�L�[
              if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // ���́Afire�L�[�̓��͂Ɣ��f�A�A�A�����ł͉������Ȃ�
                inputFire();
                return (repaint);
            }
            //not break!!
          case 27:
          case 113:
          case 81:
           // ESC�L�[�AQ�L�[
           inputGameC();
           break;

          default:
            repaint = false;
            break;
        }
        return (repaint);
    }

    /**
     *  �����ňړ����[�h
     * 
     * @param keyCode
     * @return
     */
    private boolean numberMovingMode(int keyCode)
    {
        boolean repaint = true;  // �ĕ`��
        switch (keyCode)
        {
          case Canvas.KEY_NUM2:
            // �O�y�[�W��\������
            inputUp();
            break;

          case Canvas.KEY_NUM4:
            // �O���b�Z�[�W�\��
            inputLeft();
            break;


          case Canvas.KEY_NUM6:
            // �����b�Z�[�W�\��
            inputRight();
            break;

          case Canvas.KEY_NUM8:
            // ���y�[�W��\������
            inputDown();
            break;

          case Canvas.KEY_NUM9:
            // �����擾�����{...
            subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
            parent.reloadCurrentMessage();
            break;

          case Canvas.KEY_NUM3:
            //  �Q�Ɖ�ʂ��甲����
            endShowMessage();
            break;

          case Canvas.KEY_NUM1:
            // ���C�ɓ���ɓo�^����
            setFavoriteThread();
            break;

          case 44:
          case Canvas.KEY_STAR:
            // ���X�̐擪�ֈړ�����
            jumpTop();
            break;

          case 46:
          case Canvas.KEY_POUND:
            // ���X�̖����ֈړ�����
            jumpBottom();
            break;

          case Canvas.KEY_NUM7:
            // ���X��URL���u���E�U��OPEN����
            subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
            repaint = launchWebBrowser();
            break;

          case Canvas.KEY_NUM0:
              // �@�\�̎��s�B�B�B
            executeFunctionFeature();
            break;

          case -5:
            // ����
            inputFire();
            break;

          case 119:
          case  87:
            // �u���E�U�ŊJ�� (W)
            inputGameD();
            break;

          case 8:
            // BS�L�[...
            if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // ���́Afire�L�[�̓��͂Ɣ��f�A�A�A�����ł͉������Ȃ�
                inputFire();
                return (repaint);
            }
            //not break!!
          case 27:
          case 113:
          case 81:
           // ESC�L�[�AQ�L�[
           inputGameC();
           break;

          default:
            repaint = false;
            break;
        }
        return (repaint);
    }

    /**
     *  �������̓��[�h
     * 
     * @param keyCode
     * @return
     */
    private boolean numberInputMode(int keyCode)
    {
        boolean repaint = true;  // �ĕ`��
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 0;
            }
            break;

          case Canvas.KEY_NUM1:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 1;
            }
            break;

          case Canvas.KEY_NUM2:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 2;
            }
            break;

          case Canvas.KEY_NUM3:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 3;
            }
            break;

          case Canvas.KEY_NUM4:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 4;
            }
            break;

          case Canvas.KEY_NUM5:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 5;
            }
            break;

          case Canvas.KEY_NUM6:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 6;
            }
            break;

          case Canvas.KEY_NUM7:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 7;
            }
            break;

          case Canvas.KEY_NUM8:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 8;
            }
            break;

          case Canvas.KEY_NUM9:
              if (commandJumpNumber < 0)
            {
                commandJumpNumber = 0;
            }
            if (commandJumpNumber < 100)
            {
                  commandJumpNumber = commandJumpNumber * 10 + 9;
            }
            break;

          case 46:
          case Canvas.KEY_POUND:
            // �������X�ԍ�
            if (commandJumpNumber == messageParser.getNumberOfMessages())
            {
                commandJumpNumber = 0;
            }
            else
            {
                commandJumpNumber = messageParser.getNumberOfMessages();
            }
            break;

          case 44:
          case Canvas.KEY_STAR:
            // �擪���X�ԍ�
            if (commandJumpNumber == 1)
            {
                commandJumpNumber = 0;
            }
            else
            {
                commandJumpNumber = 1;
            }
            break;

          case -50:
            // Nokia�̂���҂L�[... (�������킹...)
            commandJumpNumber = 0;
            break;

          case -5:
            // ����
            inputFire();
            break;

          case  8:
              // BS�L�[...
            if (getGameAction(keyCode) == Canvas.FIRE)
            {
                // ���́Afire�L�[�̓��͂Ɣ��f�A�A�A�����ł͉������Ȃ�
                inputFire();
                return (repaint);
            }
           //not break!!
          case -8:
          case 27:
          case 113:
          case 81:
           // Nokia�̃N���A�L�[... (�������킹...) ESC�L�[�AQ�L�[
           inputGameC();
           break;

          case 119:
          case  87:
            // �u���E�U�ŊJ�� (W)
            inputGameD();
            break;

          default:
            // ��L�ȊO�̃L�[�������ꍇ...
            repaint = false;
            break;
        }
        return (repaint);
    }
    
    /**
     * ��ړ�
     * @return
     */
    private boolean inputUp()
    {    
        boolean ret = true;
        if (isCommandMode != true)
        {
            // �O�y�[�W��\������
            currentPage--;
            if (currentPage < 0)
            {

                // �f�[�^�����͂�����\������悤��
                showXAxisforAAmode = 0;

                // �O���b�Z�[�W�\��
                showMessageRelative(-1 * zoneScreenCount);
                ret = false;
            }
        }
        else
        {
            // �ЂƂO�̉��...
            if (buildNofPickupList != 0)
            {
                currentPickupList--;
                if (currentPickupList < 0)
                {
                    commandJumpNumber = 0;
                    currentPickupList = buildNofPickupList;
                }
                else
                {
                    commandJumpNumber = pickupMessageList[currentPickupList];
                }
            }
        }
        return (ret);
    }
    
    /**
     * ���ړ�
     * @return
     */
    private boolean inputDown()
    {
        boolean ret = true;
        if (isCommandMode != true)
        {
            // ���y�[�W��\������
            currentPage++;
            if (currentPage > msgPage)
            {
                currentPage = 0;

                // �f�[�^�����͂�����\������悤��
                showXAxisforAAmode = 0;

                // �ŏI���X�ł͂Ȃ���΁A�����X��\������
                if (messageNumber != messageParser.getNumberOfMessages())
                {
                    showMessageRelative(zoneScreenCount);
                }
                else
                {
                    currentPage = msgPage;
                }
                ret = false;
            }
        }
        else
        {
            // �ЂƂ��̉��...
            if (buildNofPickupList != 0)
            {
                currentPickupList++;
                if (currentPickupList >= buildNofPickupList)
                {
                    commandJumpNumber = 0;
                    currentPickupList = -1;
                }
                else
                {
                    commandJumpNumber = pickupMessageList[currentPickupList];
                }
            }
        }
        return (ret);
    }

    /**
     * ���ړ�
     * @return
     */
    private boolean inputLeft()
    {
        if (checkNgWord == true)
        {
            ngWordCheckDirection = -1;
        }

        if (isSearchingMode == true)
        {
            // �X�����������[�h�̂Ƃ�...�O�����֌�������
            executeSearchKeyword(-1);
            return (false);
        }        
        if (isAsciiArtShowMode == true)
        {
            showXAxisforAAmode = showXAxisforAAmode - AA_WIDTH_MARGIN;
            if (showXAxisforAAmode < 0)
            {
                showXAxisforAAmode = 0;
            }
            showMessageRelative(0);
            return (false);
        }
        currentPage = 0;
        showMessageRelative(-1 * zoneScreenCount);
        return (false);
    }

    /**
     * �E�ړ�
     * @return
     */
    private boolean inputRight()
    {
        if (checkNgWord == true)
        {
            ngWordCheckDirection = 1;
        }
        if (isSearchingMode == true)
        {
            // �X�����������[�h�̂Ƃ�...�������֌�������
            executeSearchKeyword(1);
            return (false);
        }
        if (isAsciiArtShowMode == true)
        {
            showXAxisforAAmode = showXAxisforAAmode + AA_WIDTH_MARGIN;
            if (showXAxisforAAmode > LIMIT_WIDTH)
            {
                showXAxisforAAmode = LIMIT_WIDTH;
            }
            showMessageRelative(0);
            return (false);
        }
        currentPage = 0;
        showMessageRelative(zoneScreenCount);
        return (false);
    }

    /**
     * Fire�{�^�����������Ƃ��̏���
     * @return
     */
    private boolean inputFire()
    {
        // �X���I�����j���[��\������
        busyMessage = "";
        if (isCommandMode == true)
        {
            // �R�}���h���[�h��OFF�ɂ���
            isCommandMode = false;
            if (commandJumpNumber < 0)
            {
/**
                if (parent.isGetNewArrival() == true)
                {
                    // �V���m�F���̏ꍇ�ɂ́A�\���N���A
                    isCommandMode = true;
                    commandJumpNumber = 0;
                    return (true);
                }
**/
                //  �Q�Ɖ�ʂ��甲����
                endShowMessage();
                return (true);
            }
            else if (commandJumpNumber != 0)
            {
                if (jumpListIndex < nofJumpList)
                {
                    messageJumpList[jumpListIndex] = messageNumber;
                    jumpListIndex++;
                }
                currentPage = 0;

                // �f�[�^�����͂�����\������悤��
                showXAxisforAAmode = 0;

                showMessage(commandJumpNumber);
            }
            commandJumpNumber  = 0;
            buildNofPickupList = 0;
            currentPickupList  = -1;
        }
        else
        {
            // �R�}���h���[�h�ɐ؂�ւ���
            isCommandMode = true;
            pickUpJumpList();
            if (buildNofPickupList > 0)
            {
                currentPickupList = 0;
                commandJumpNumber = pickupMessageList[currentPickupList];
            }
        }        
        return (true);
    }
    
    /**
     * GAME A�̓���
     * 
     * @return
     */
   private boolean inputGameA()
    {
        return (inputUp());
/*
        parent.toggleNumberInputMode(isNumberMovingMode);
        repaint();
        return (true);
*/
    }

    /**
     * GAME B�̓���
     * 
     * @return
     */
    private boolean inputGameB()
    {
        return (inputDown());
/*
        changeHelpShowMode();
        return (true);        
*/
    }

    /**
     * GAME C�̓���
     * 
     * @return
     */
    private boolean inputGameC()
    {
        busyMessage = "";
        if (isCommandMode != true)
        {
            if (jumpListIndex != 0)
            {
                jumpListIndex--;
                currentPage = 0;

                // �f�[�^�����͂�����\������悤��
                showXAxisforAAmode = 0;

                int num = messageJumpList[jumpListIndex];
                showMessage(num);
            }
        }
        else
        {
            // �R�}���h���[�h�ŁA���������͂���Ă��Ȃ��ꍇ�ɂ́A�R�}���h���[�h����������
            if (commandJumpNumber == 0)
            {
                isCommandMode = false;
                commandJumpNumber  = 0;
                buildNofPickupList = 0;
                currentPickupList  = -1;
                return (true);
            }

            // �N���A�L�[
            commandJumpNumber = commandJumpNumber / 10;                
        }

        // �w���v�\�����̏ꍇ�́A��������
        if (isHelpShowMode == true)
        {
            changeHelpShowMode();
        }
        
        // �W�����v�o�b�t�@���[���̏ꍇ�ɂ́A�������[�h����������
        if (jumpListIndex == 0)
        {
            isSearchingMode = false;
            searchMessageString = null;
        }
        return (true);
    }

    /**
     * GAME D�̓���
     * 
     * @return
     */
    private boolean inputGameD()
    {
        subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
        parent.launchBrowserCurrentMessage();
        return (true);
    }

    /*
     *   �L�[���s�[�g���Ă���Ƃ�...
     * 
     * 
     */
    public void keyRepeated(int keyCode)
    {
        parent.keepWatchDog(0);

        // ���b�Z�[�W�\�z���ɂ̓L�[������󂯕t���Ȃ�
        if ((buildingMessage == true)||(lockOperation == true))
        {
            return;
        }
//        drawInformation = false;

        boolean repaint = true;
        if (keyCode == 0)
        {
            return;
        }

        // ���ݎ������擾����
        Date dateTime = new Date();
        long currentTime = dateTime.getTime();
        dateTime = null;

        // �L�[���s�[�g�́A2��ɂP��A�����WAIT_TIME ms��������悤�ɂ���B�B�B
        if (keyCode != repeatingKeyCode)
        {
            repeatingKeyCode = keyCode;
            repeatingStartDate = currentTime;
            return;
        }
        
        // ���s�[�g�C�x���g��keyRepeatDelayMs ms�o���Ă��Ȃ������ꍇ�ɂ͂�����x...
        if (currentTime < (repeatingStartDate + keyRepeatDelayMs))
        {
            return;
        }
        repeatingKeyCode   = 0;
        repeatingStartDate = 0;

        if (isNumberMovingMode == true)
        {
            repaint = true;
            switch (keyCode)
            {
              case Canvas.KEY_NUM2:
                // �O�y�[�W��\������
                inputUp();
                break;

              case Canvas.KEY_NUM4:
                // �O���b�Z�[�W�\��
                inputLeft();
                break;

              case Canvas.KEY_NUM6:
                // �����b�Z�[�W�\��
                inputRight();
                break;

              case Canvas.KEY_NUM8:
                // ���y�[�W��\������
                inputDown();
                break;

              default:
                repaint = false;
                break;
            }
            if (repaint == true)
            {
                repaint();
                return;
            }            
        }

        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            // �O���b�Z�[�W�\��
            repaint = inputLeft();
            break;
              
          case Canvas.RIGHT:
            // �����b�Z�[�W�\��
            repaint = inputRight();
            break;

          case Canvas.DOWN:
            // ���y�[�W��\������
            repaint = inputDown();
            break;
          case Canvas.UP:
            // �O�y�[�W��\������
            repaint = inputUp();
            break;

          case Canvas.GAME_A:
            // Game A�{�^������
            repaint = inputGameA();
            break;

          case Canvas.GAME_B:
            // Game B�{�^������
            repaint = inputGameB();
            break;

          case Canvas.FIRE:
          case Canvas.GAME_C:
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
     *  ���X�̖����ֈړ�����
     *
     */
    private void jumpBottom()
    {
        // �W�����v���ԍ����L������
        if (jumpListIndex < nofJumpList)
        {
            messageJumpList[jumpListIndex] = messageNumber;
            jumpListIndex++;
        }

        // ���X�̖����ֈړ�
        currentPage = 0;

        // �f�[�^�����͂�����\������悤��
        showXAxisforAAmode = 0;

        showMessage(messageParser.getNumberOfMessages());
        
        return;
    }
    
    
    /**
     *  ���X�̐擪�ֈړ�����
     *
     */
    private void jumpTop()
    {
        // �W�����v���ԍ����L������
        if (jumpListIndex < nofJumpList)
        {
            messageJumpList[jumpListIndex] = messageNumber;
            jumpListIndex++;
        }

        // ���X�̐擪�ֈړ�
        currentPage = 0;

        // �f�[�^�����͂�����\������悤��
        showXAxisforAAmode = 0;

        showMessage(1);
        return;
    }
    
    /**
     *  ���������W����������...
     * 
     * 
     */
    public void pointerPressed(int x, int y)
    {
        parent.keepWatchDog(0);

        if (lockOperation == true)
        {
            // ����֎~��...�������Ȃ��B
            return;
        }
//        drawInformation = false;

        int zoneWidth = (screenFont.charWidth('��') * 3);
        if (x > (getWidth() - zoneWidth))
        {
            // ��ʂ̉E��[�A�E���[���^�b�v�����ꍇ...
            int index =  y / (screenFont.getHeight() + 1);
            if (index <= 2)
            {
                // ���X�̐擪�ֈړ�����
                jumpTop();
                return;
            }
            else if (index >= (screenLines - 2))
            {
                // ���X�̖����ֈړ�����
                jumpBottom();
                 return;
            }
        }
        if (x < zoneWidth)
        {
            // ��ʂ̍���[�A�����[���^�b�v�����ꍇ...
            int index =  y / (screenFont.getHeight() + 1);
            if (index <= 2)
            {
                // ����[�A�����N���J�� (���X��URL���u���E�U��OPEN����)
                subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
                launchWebBrowser();
                repaint();
                return;
            }
            else if (index >= (screenLines - 2))
            {
                // �����[�A�@�\�̎��s�B�B�B
                executeFunctionFeature();
                 return;
            }            
        }

        if (y < (screenFont.getHeight() * 2))
        {
            // ��ԏ�̍s���^�b�v���ꂽ�Ƃ��́A�ꗗ�֖߂�
            endShowMessage();
            return;
        }        
        
        if (y <= (getHeight() / 2))
        {
            // �P�y�[�W�߂�
            inputUp();
        }
        else
        {
            // �P�y�[�W����
            inputDown();
        }
        repaint();
        return;
    }

    /*
     *  ���������W����������...
     * 
     * 
     */
    public void pointerReleased(int x, int y)
    {
        return;
    }
    

    /*--*/
    // �R�}���h�{�^�����s���̏���
    public void commandAction(Command c, Displayable d) 
    {
        parent.keepWatchDog(0);

        if ((c != quitCmd)&&(lockOperation == true))
        {
            // ����֎~��...�������Ȃ��B
            return;
        }
//        drawInformation = false;

        // �R�}���h�{�^�����s����
        if (c == rtnCmd)
        {
            //  �Q�Ɖ�ʂ��甲����
            endShowMessage();
        }
        else if (c == modeCmd)
        {
            // ���j���[�Q
            showMenuSecond();
        }
/*
        else if (c == backCmd)
        {
            // �O��W�����v�����Ƃ���ɖ߂�
            if (jumpListIndex != 0)
            {
                jumpListIndex--;
                currentPage = 0;

                // �f�[�^�����͂�����\������悤��
                showXAxisforAAmode = 0;

                int num = messageJumpList[jumpListIndex];
                showMessage(num);
            }
        }
*/
        else if (c == ngEntCmd)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // NG�o�^�t�H�[�����J��
            openNgEntryForm();
        }
        else if (c == ngCmd)
        {
            // NG��������/���Ȃ��̐؂�ւ�
            parent.toggleNgWordDetectMode();
            checkNgWord = parent.getNgWordDetectMode();

            // ���X�̍ĕ\��
            showMessage(messageNumber);
        }
        else if (c == partCmd)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // �V���m�F�����{����...
            parent.startGetLogList();
        }
        else if (c == writeCmd)
        {
            if (parent.isGetNewArrival() == true)
            {
                // �V���m�F���͋֎~����
                return;
            }
            // �������݉�ʂ�\���\�ȃX���������ꍇ�A�������݉�ʂɐ؂�ւ���
            if (parent.canWriteMessage() == true)
            {
                subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, messageParser.getNumberOfMessages());
                String title = stringConverter.parsefromSJ(messageParser.getThreadTitle());
//                parent.OpenWriteForm(false, title, cureentMessageIndex, messageNumber);
                parent.OpenWriteForm(false, title, currentMessageIndex, 0);
                return;
            }
        }
        else if (c == outFilCmd)
        {
            // ���X�o�̓t�@�C�����̎w��
            changeOutputFileName();
        }
        else if (c == copyCmd)
        {
            // ���X��ҏW���[�h�ɂ���
            messageEditText(true);
        }
        else if (c == outputCmd)
        {
            // ���X�������t�@�C���֏o�͂���
            messageEditText(false);
        }
        else if (c == quitCmd)
        {
            // a2B���I��������
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
                // �������Ȃ�...
            }
            return;
        }
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

        Vector commandList = new Vector(9);
        commandList.addElement("�ꗗ��ʂɖ߂�");
        commandList.addElement("�����L�[�ړ����[�h");
        commandList.addElement("�w���v�\�����[�h");
        commandList.addElement("��ʃ��[�h");
        commandList.addElement("Font�T�C�Y�ؑ�");
        commandList.addElement("BE profile�Q��");
        commandList.addElement("���X�܂Ƃߓǂ݃��[�h");
        commandList.addElement("�������߂Q�����˂�");
        commandList.addElement("SJIS/EUC/UTF8/JIS�ؑ�");

        parent.openSelectForm(title, commandList, 0, parent.SHOW_VIEWCOMMANDSECOND);
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
    public void executeMenuSecond(byte scene, int index)
    {
        if (index == 0)
        {
            //  �Q�Ɖ�ʂ��甲����
            endShowMessage();
        }
        if (index == 1)
        {
            // �����L�[�ړ����[�h�ؑ�
            parent.toggleNumberInputMode(isNumberMovingMode);
        }
        if (index == 2)
        {
            // �w���v�\�����[�h�ؑ�
            changeHelpShowMode();
        }
        if (index == 3)
        {
            // �X�N���[���T�C�Y�̕ύX
            boolean isFullScreen = parent.getViewScreenMode();
            if (isFullScreen == true)
            {
                isFullScreen = false;
            }
            else
            {
                isFullScreen = true;
            }
            changeScreenMode(isFullScreen);
        }
        if (index == 4)
        {
            // �t�H���g�T�C�Y�ύX
            changeFontSize(false);
        }
        if (index == 5)
        {
            // BE profile�Q��
            viewBeProfile();
        }
        if (index == 6)
        {
            // ���X�܂Ƃߓǂ݃��[�h
            if (zoneScreenCount == 1)
            {
                zoneScreenCount = parent.getResBatchCount();
            }
            else
            {
                zoneScreenCount = 1;
            }
            parent.setResBatchMode(zoneScreenCount);
            showMessage(messageNumber);
        }
        if (index == 7)
        {
            // �������߂Q�����˂�̕\��
            viewOsusume2ch();
        }
/**/
        if (index == 8)
        {
            // �\�����[�h�̐؂�ւ�(SJIS/EUC/UTF8)
            parent.toggleViewKanjiMode(a2BsubjectDataParser.PARSE_TOGGLE);
            showMessage(messageNumber);

            // ���[�h���E��ɕ\������
            int mode = parent.getViewKanjiMode();
            if (mode == a2BsubjectDataParser.PARSE_UTF8)
            {
            	informationChar = 'U';
                drawInformation = true;
            }
            else if (mode == a2BsubjectDataParser.PARSE_JIS)
            {
                informationChar = 'J';
                drawInformation = true;
            }
            else if (mode == a2BsubjectDataParser.PARSE_EUC)
            {
                informationChar = 'E';	
                drawInformation = true;
            }
            else // if (mode == a2BsubjectDataParser.PARSE_SJIS)
            {
            	informationChar = 'S';
                drawInformation = true;
            }
        }
/**/
        System.gc();
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
        int feature = parent.getFunctionKeyFeature(parent.SCENE_VIEWFORM);
        if (feature == 0)
        {
            // �t�H���g�T�C�Y�̕ύX
            changeFontSize(false);
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
            // �X������\������
            if (parent.isGetNewArrival() != true)
            {
                // �V���m�F���͋֎~����
                showMessageInformation();
            }
        }
        else if (feature == 4)
        {
            // NG��������/���Ȃ��̐؂�ւ�
            parent.toggleNgWordDetectMode();
            checkNgWord = parent.getNgWordDetectMode();

            // ���X�̍ĕ\��
            showMessage(messageNumber);
        }
        else if (feature == 5)
        {
            // ���X�������t�@�C���֏o�͂���
            messageEditText(false);            
        }
        else if (feature == 6)
        {
            // ���[�h���j���[�̕\��
            showMenuSecond();
        }
        else if (feature == 7)
        {
            // ���X�܂Ƃߓǂ݃��[�h
            if (zoneScreenCount == 1)
            {
                zoneScreenCount = parent.getResBatchCount();
            }
            else
            {
                zoneScreenCount = 1;
            }
            parent.setResBatchMode(zoneScreenCount);
            showMessage(messageNumber);            
        }
        else if (feature == 8)
        {
            // �N���A
            inputGameC();
        }
        else if (feature == 9)
        {
            // �������߂Q�����˂�\��
            viewOsusume2ch();
        }
        return;
    }

    // �X�N���[���T�C�Y�̕ύX
    private void changeScreenMode(boolean isFullScreen)
    {
        currentPage = 0;
        setFullScreenMode(isFullScreen);
        parent.setViewScreenMode(isFullScreen);

        // �\���\�s���Đݒ肵�A���X���ĕ\��...
        sizeChanged(getWidth(), getHeight());

        return;
    }
    
    // ��ʃT�C�Y�ύX���ɌĂ΂�鏈��...
    public void sizeChanged(int w, int h)
    {
        setLineWidthHeight(w, h);

        // ���X�̍ĕ\��
        showMessage(messageNumber);

        return;
    }

    /**
     * �\���\�̈��ݒ肷��
     * @param w
     * @param h
     */
    private void setLineWidthHeight(int w, int h)
    {
        // �\���\�s���Đݒ肷�� (1�s���ABUSY�E�B���h�E�\���Ƃ��Ċm��...)
        screenWidth  = w;
        screenHeight = h - screenFont.getHeight() * 1;
        screenLines  = (screenHeight / (screenFont.getHeight() + 1));

        return;
    }
    
    /**
     *  �t�H���g�F��ύX����
     *
     */
    private void changeFontColor()
    {
        if ((messageBackColor == 0x00000000)||(messageBackColor == 0x008080f0)||(messageBackColor == 0x00d080f0)||(messageBackColor == 0x00004101))
        {
            if (messageBackColor == 0x00000000)
            {
                // �u���[�O���[�A�w�i�͔�
                parent.setViewFontColor(0x008080f0, 0x00f0f0f0);                
            }
            else if (messageBackColor == 0x008080f0)
            {
                // �΁A�w�i�͔�
                parent.setViewFontColor(0x00004101, 0x00f0f0f0);                
            }
            else if (messageBackColor == 0x00004101)
            {
                // �����ۂ��u���[�O���[�A�w�i�͍�
                parent.setViewFontColor(0x00d080f0, 0x00080808);                
            }
            else
            {
                // �����͂���A�w�i�͔�
                parent.setViewFontColor(0x00000000, 0x00f0f0f0);                
            }
        }
        else
        {
            // �F�𔽓]������..
            parent.setViewFontColor(messageBackColor, messageForeColor);
        }

        // �F�����o��...
        messageForeColor = parent.getViewForeColor();
        messageBackColor = parent.getViewBackColor();
        return;
    }
    
    // �t�H���g�T�C�Y�̕ύX
    public void changeFontSize(boolean isReset)
    {
        if (isAsciiArtShowMode == true)
        {
            // AA�\�����[�h�̎��ɂ́A�t�H���g�T�C�Y��ύX�ł��Ȃ��悤�ɂ���B
            return;            
        }
        
        // �t�H���g�T�C�Y�̕ύX (��(�F���])�������偨��(�F���])������... �Ɛ؂�ւ���)
        int size;
        if (isReset == false)
        {
            size = screenFont.getSize();
            switch (size)
            {
              case Font.SIZE_MEDIUM:
                size = Font.SIZE_LARGE;
                break;

              case Font.SIZE_SMALL:
                size = Font.SIZE_MEDIUM;
                break;

              case Font.SIZE_LARGE:
              default:
                size = Font.SIZE_SMALL;
                screenFont = Font.getDefaultFont();
                changeFontColor();
                break;
            }
        }
        else
        {
            // �t�H���g�T�C�Y�����Z�b�g����
            size = parent.getViewFontSize();

            // �F�����Z�b�g����
            messageForeColor = parent.getViewForeColor();
            messageBackColor = parent.getViewBackColor();
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
        parent.setViewFontSize(size);

        // �\���\�s���Đݒ肷��
        setLineWidthHeight(getWidth(), getHeight());

        // ���X�̍ĕ\��
        currentPage = 0;

        // �f�[�^�����͂�����\������悤��
        showXAxisforAAmode = 0;

        showMessage(messageNumber);

        return;
    }    

    // �Q�Ɖ�ʂ̏I��
    private void endShowMessage()
    {
        // ���X�V��...
        msgLines = null;
        isCommandMode = false;
        busyInformation = "��񏑂����ݒ�...";
        repaint();
        serviceRepaints();

        // �I�����[�`�����Ăяo��...
        Thread thread = new Thread()
        {
            public void run()
            {
                finishShowMessage();
            }
        };

        // ���b�Z�[�W�X�V���̂��邵�𗧂Ă�
//        buildingMessage = true;    

        thread.start();

        return;
    }

    /**
     *  �Q�Ɖ�ʂ𔲂��鏈��...
     * 
     */
    private void finishShowMessage()
    {
        // ���b�Z�[�W�X�V���̂��邵�𗧂Ă�
//        buildingMessage = true;

        // �X���^�C�g���f�[�^�̍X�V���s��...
        parent.updateThreadTitle(messageParser.getThreadTitle(), 0, messageParser.getThreadTitleLength());

        // �����̒l���擾����
        int bottom = messageParser.getNumberOfMessages();
        
        // �f�[�^�̃N���A...
        messageParser.leaveMessage();
        
        // �e�ɑ΂��A��ʂ𔲂������Ƃ�ʒm����
        parent.EndViewForm(messageNumber, bottom);

        // ��ʂ��߂����Ƃ��A�y�[�W��擪�ɂ���
        currentPage = 0;

        // �f�[�^�����͂�����\������悤��
        showXAxisforAAmode = 0;

        // �g�p���銿���R�[�h��Shift JIS�ɖ߂�
        parent.toggleViewKanjiMode(a2BsubjectDataParser.PARSE_SJIS);
        
        // �X�������X�W�����v���X�g���N���A����
        jumpListIndex     = 0;
        
        // �������[�h�Ɖ�ʍX�V���[�h���N���A����...
        painting          = false;
        isSearchExecution = false;
        buildingMessage   = false;

        return;
    }

    // NG���[�h�o�^�t�H�[�����J��...
    private void openNgEntryForm()
    {
        // �\�����[�`�����Ăяo��...
        Thread thread = new Thread()
        {
            public void run()
            {
                // �f�t�H���g�L�[���[�h�̒��o
                String defaultKeyword = "";

                int startPos = parsedString.indexOf("ID:");
                if (startPos >= 0)
                {
                    int endPos = parsedString.indexOf("\r", startPos);
                    if (endPos >= 0)
                    {
                        defaultKeyword = parsedString.substring(startPos, endPos);
                    }
                }        

                // NG���[�h�o�^�p��ʂ��J��
                parent.OpenWriteFormUsingWordInputMode(parent.SCENE_NGWORD_INPUT, "NG���[�h�o�^", "NG���[�h", defaultKeyword, false);
            }
        };
        thread.start();

        return;
    }

    
    /**
     * �������̕\��...
     * 
     */
    private void showSearchingWindow(Graphics g)
    {
        int width, height;
        
        width  = screenWidth / 2 - 10;
        height = screenFont.getHeight() + 4;
        
        // ��ʂ̓h��Ԃ�
        g.setColor(0x00ffffc0);
        g.fillRect(10, (screenHeight - height - 2), width * 2, (screenHeight - 2));
        g.setColor(0, 0, 0);
        g.setFont(screenFont);
        
        // �R�}���h�E�B���h�E�^�C�g���̕\��
        g.drawString("������: " + searchMessageString, 15, (screenHeight - height), (Graphics.LEFT | Graphics.TOP));

        return;        
    }
    
    // �R�}���h�E�B���h�E�̕\��...
    private void showCommandWindow(Graphics g)
    {
        int width, height;
        
        width  = screenWidth / 2 - 20;
        height = screenFont.getHeight() + 6;
        
        // ��ʂ̓h��Ԃ�
        g.setColor(210,210, 210);
        g.fillRect(20, (screenHeight - height - 2), width * 2, height);
        g.setColor(0, 0, 0);
        g.setFont(screenFont);
        
        // �R�}���h�E�B���h�E�^�C�g���̕\��
        if (commandJumpNumber < 0)
        {
            // ���X�ԍ��Ȃ��̕\��
            g.drawString("�ړ��� : (�ꗗ�֖߂�) ", 20 + 6, (screenHeight - height), (Graphics.LEFT | Graphics.TOP));            
        }
        else if (commandJumpNumber == 0)
        {
            // ���X�ԍ��Ȃ��̕\��
            g.drawString("�ړ��惌�X�ԍ��F ", 20 + 6, (screenHeight - height), (Graphics.LEFT | Graphics.TOP));
        }
        else
        {
            // ���X�ԍ��t���̕\�� 
            g.drawString("�ړ��惌�X�ԍ��F " + commandJumpNumber, 20 + 6, (screenHeight - height), (Graphics.LEFT | Graphics.TOP));
        }
        return;        
    }

    /**
     * ���X���擾�����H�A�\���̏������s��
     *
     * @param getCount �A�����Ď擾���郌�X�̐�
     * @return
     */
    private String prepareMessage()
    {
        String parsedMessage = null;
        boolean ngLoop = true;
        while (ngLoop == true)
        {
            // ���X�f�[�^���擾����
        	int kanjiMode = parent.getViewKanjiMode();
            byte[] resData = messageParser.getMessage(messageNumber, kanjiMode);
            if (kanjiMode == a2BsubjectDataParser.PARSE_UTF8)
            {
            	// UTF8�̊������[�h�̂Ƃ��ɂ́A�������Ȃ�
            	try
            	{
                    parsedMessage = new String(resData, 0, resData.length, "UTF-8");
            	}
            	catch (Exception e)
            	{
                    parsedMessage = new String(resData, 0, resData.length);
            	}
            }
            else if (kanjiMode == a2BsubjectDataParser.PARSE_JIS)
            {
            	// JIS�̏ꍇ�ɂ́AJIS => UTF8�ϊ������{����
                parsedMessage = stringConverter.parsefromJIS(resData, 0, resData.length);            	
            }
            else
            {
            	// UTF8/JIS�ȊO�̏ꍇ�ɂ́ASJ => UTF8�ϊ������{����
                parsedMessage = stringConverter.parsefromSJ(resData, 0, resData.length);
            }

            boolean doAbone = false;
            if (checkNgWord == true)
            {
                // NG�`�F�b�N���[�h�̂Ƃ��ANG�L�[���[�h���܂܂�Ă��邩�`�F�b�N����
                doAbone = checkNgKeyword(parsedMessage);
                if (doAbone == true)
                {
                    if ((zoneScreenCount == 1)&&(ngWordCheckDirection != 0))
                    {
                        // ����(NG�łȂ�)���X��T��
                        messageNumber = messageNumber + ngWordCheckDirection;
                        if ((messageNumber > messageParser.getNumberOfMessages())||(messageNumber < 1))
                        {
                            // ���X�͈͂𒴂����A���[�v�𔲂���...
                            messageNumber = messageNumber - ngWordCheckDirection;
                            parsedMessage = null;
                            parsedMessage = "* * *\r\n\r\n\r\n";
                            messageParser.doAbone();
                            messageNumber = messageNumber - ngWordCheckDirection;
                            ngLoop = false;
                        }
                    }
                    else
                    {
                        // ���ځ[����{���A���X�ԍ�������\��
                        parsedMessage = null;
                        parsedMessage = "* * *\r\n\r\n\r\n";
                        messageParser.doAbone();
                        ngLoop = false;
                    }
                }
                else
                {
                    // ���[�v�𔲂���...
                    break;
                }
            }
            else
            {
                // ���[�v�𔲂���...
                break;
            }
        }
        ngWordCheckDirection = 0;
        return ("[" + messageNumber + "] " + parsedMessage);
    }    

    /*
     *  ���X�f�[�^���s�P�ʂɉ��H����B
     * 
     */
    private void chopData(String parsedMessage)
    {
        // �e�L�X�g�f�[�^�\�����[�h�̏ꍇ�A�P�s�ǉ�����
        if (messageParsingMode != PARSING_NORMAL)
        {
            msgLines.addElement("");
        }

        // �\���\�L�����N�^����ݒ肷��
        int width  = screenWidth - 5;
        int drawRange = width / screenFont.charWidth('X');
        int index  = 0;
        int top    = 0;
        String  msg = "";

        while (top < parsedMessage.length())
        {
            index    = parsedMessage.indexOf('\n', top);
            int pos = index;
            if ((index >= 0)&&(parsedMessage.charAt(index - 1) == '\r'))
            {
                pos--;
            }
            if (index < 0)
            {
                index = parsedMessage.length();
                pos   = index;
            }
            if (index == top)
            {
                top++;
                msgLines.addElement("");
            }
            else
            {
                // �擪�s��\�����Ȃ� (��֎�i�ŕ\�����邽��)
                if ((top == 0)&&(messageParsingMode == PARSING_NORMAL)&&(zoneScreenCount == 1))
                {
                    msgLines.addElement("");
                    top = index;
                    continue;
                }
                if (isAsciiArtShowMode == true)
                {
                    // �\������f�[�^�̐擪��������...
                    if (pos <= 0)
                    {
                        // �ŏI�s�̏ꍇ...
                        msg = parsedMessage.substring(top);
                        msgLines.addElement(msg);
                        index = parsedMessage.length();
                    }
                    else
                    {
                        // �P�s�؂�o��... (�I�[�o�[���b�v���Ȃ��ꍇ...)
                        msg = parsedMessage.substring(top, pos);
                        msgLines.addElement(msg);
                    }
                }
                else
                {
                    /* 1�s�Ɏ��܂�Ȃ��s�𕡐��̍s�ɕ������鏈�� */
                    int ch    = pos - top;  // ch   : �L�����N�^��
                    while (ch > 0)
                    {
                        int loop = (drawRange < ch) ? drawRange : ch;
                        if (screenFont.substringWidth(parsedMessage, top, ch) > width)
                        {
                            // �Ƃ肠�����A 'X'����ʂ����ς�����ł����Ƃ������L�����N�^�����������ǂ�����r����
                            if (screenFont.substringWidth(parsedMessage, top, loop) >= width)
                            {
                                // �����������A���X�Ɍ��炷...
                                do
                                {
                                    loop--;
                                }
                                while (screenFont.substringWidth(parsedMessage, top, loop) >= width);
                            }
                            else
                            {
                                // �܂�����Ȃ�...
                                do
                                {
                                    loop++;
                                }
                                while (screenFont.substringWidth(parsedMessage, top, loop) < width);
                                loop--; // �����������炷�K�v����!
                            }
                        }
                        else
                        {
                            // ��s�Ɏ��܂�ꍇ...
                            loop = ch;
                        }
                        msg = parsedMessage.substring(top, top + loop);
                        msgLines.addElement(msg);
                        top = top + loop;
                        ch = ch - loop;
                    }
                }
                top = index + 1;
            }
        }        
        msgLines.addElement("");
    }    
    
    /**
     *  ���X�\������...
     *  
     */
    private void showMessageMain()
    {
        // �f�b�h���b�N���...
        if (buildingMessage == true)
        {
            return;
        }
        buildingMessage = true;
        isMax = false;

        int currentMessage = messageNumber;
        int maxMsg = messageParser.getNumberOfMessages();
        
        // �f�[�^�s�f�[�^
        msgLines = new Vector();

        try
        {
            // ��͂������b�Z�[�W���s�P�ʂɋ�؂�
            parsedString = prepareMessage();
            currentMessage = messageNumber;
            messageNumber++;
            for (int loop = 1; loop < zoneScreenCount; loop++)
            {
                if (messageNumber > maxMsg)
                {
                    // ��͐��𒴂���...break����
                    break;
                }
                parsedString = parsedString + "\r\n=====\r\n" + prepareMessage();
                messageNumber++;
            }
            if (messageNumber > maxMsg)
            {
                parsedString = parsedString + "\r\n\r\n";  // END�}�[�J�o�͗p�̈���m�ۂ���
                isMax = true;
            }
            else
            {
                if (zoneScreenCount != 1)
                {
                    parsedString = parsedString + "\r\n=====\r\n";                    
                }
            }
            chopData(parsedString);
        }
        catch (Exception e)
        {
            // �G���[�������̏��...
            msgLines.addElement("");
        }

        // ������\�����Ă��邩�ǂ���
        if (isMax == true)
        {
            // ���ǃ|�C���g�����X�����ɐݒ肷��
            messageNumber = maxMsg;
        }
        else
        {
            // ���ǃ|�C���g���]�[���\���̐擪�ɖ߂�
            messageNumber = currentMessage;
        }
        
        // �y�[�W�������߁AmsgPage�Ɋi�[����
        msgPage  = msgLines.size() / (screenLines - 1);
        if (msgPage != 0)
        {
            // �P�y�[�W�Ɏ��܂肫��Ȃ��ꍇ�ɂ�...������ƕ␳����
            if ((msgLines.size() % (screenLines - 1)) <= 1)
            {
                msgPage--;
            }
        }
        buildingMessage    = false;
        repaint();

        // �ǂݏo�������X�̏ꏊ(�擪)���L������
        subjectParser.setCurrentMessage(currentMessageIndex, messageNumber, maxMsg);
        return;
    }

    // ���Βl�Ń��X�ԍ����w�肵�ă��X�\��
    public void showMessageRelative(int relativeNumber)
    {
        showMessage(messageNumber + relativeNumber);
    }
    
    /*
     *  ���X�̕\������
     *  
     */
    public void showMessage(int number)
    {        
        // ���X�f�[�^�\�z���̂Ƃ��ɂ́A���b�Z�[�W�̍X�V�͎��{���Ȃ�...
        if (buildingMessage == true)
        {
            return;
        }

        // ��ʕ`�撆�̂Ƃ��ɂ́A���b�Z�[�W�̍X�V�͎��{���Ȃ�...
        if (painting == true)
        {
            return;
        }
        
        // �\�����X�ԍ������肷��
        int nofMsg = messageParser.getNumberOfMessages();
        if (number > nofMsg)
        {
            // �����ɐݒ肵�A�������Ȃ�
            currentPage = 0;

            // �f�[�^�����͂�����\������悤��
            showXAxisforAAmode = 0;

            messageNumber = nofMsg;
        }
        else if (number < 1)
        {
            // �擪�ɐݒ肵�A�������Ȃ�
            currentPage = 0;

            // �f�[�^�����͂�����\������悤��
            showXAxisforAAmode = 0;

            messageNumber = 1;
        }
        else
        {
            // �w�肵�����X�ԍ��ɐݒ�
            messageNumber = number;
        }

        // ���b�Z�[�W�ԍ����L�^����
        parent.setMessageNumber(messageNumber);
        
        // �\�����[�`�����Ăяo��...
        Thread thread = new Thread()
        {
            public void run()
            {
                showMessageMain();
            }
        };
        thread.start();        
    }

    /**
     * ���ݕ\�����̃��X�Ǘ��ԍ����X�V����
     * @param index
     */
    public void updateCurrentMessageIndex(int index)
    {
        currentMessageIndex = index;
    }
    
    /**
     * ���X��\������
     * 
     * @param fileName
     * @param bbsIndex
     * @param number
     */
    public void showMessage(String fileName, int bbsIndex, int number)
    {
        currentMessageIndex = bbsIndex;

        if (fileName == null)
        {
            // �t�@�C�������w�肳��Ă��Ȃ�...�I������
            return;
        }
        setTitle("�X����͒�..."); // �_�~�[�̃^�C�g���\��...
        msgLines = null;
        busyInformation = "�X����͒�...";
        String title = null;
        messageParsingMode = PARSING_NORMAL;
        if (bbsIndex < 0)
        {
            // BBS index����(== ���O�Q�ƃ��[�h)�ŃI�[�v�����ꂽ�Ƃ�...
            title = fileName;
            if ((fileName.toLowerCase().endsWith(".txt"))||(fileName.toLowerCase().endsWith(".a2b")))
            {
                // �e�L�X�g��̓��[�h(RAW���[�h)�Ƃ���
                messageParsingMode = PARSING_RAW;
            }
            if ((fileName.toLowerCase().endsWith(".htm"))||(fileName.toLowerCase().endsWith(".html"))||
                (fileName.toLowerCase().endsWith(".csv"))||
                (fileName.toLowerCase().endsWith(".idx"))||(fileName.toLowerCase().endsWith(".cnf")))
            {
                // HTML��̓��[�h�Ƃ���
                messageParsingMode = PARSING_HTML;
            }
        }
        else
        {
            // BBS index����(== �ʏ�Q�ƃ��[�h)�ŃI�[�v�����ꂽ�Ƃ�...
            title = parent.getSubjectName(bbsIndex);
        }

        // ���b�Z�[�W��̓��[�h��ݒ肷��
        messageParser.setMessageParsingMode(messageParsingMode);
        
        // ��͂���t�@�C������ݒ肷��
        messageParser.setFileName(fileName);

        // �X���^�C�g���̃x�[�X��ݒ肷��
        messageParser.setThreadBaseTitle(title);
        
        // �\�����X�ԍ���ݒ肷��
        messageNumber = number;

        // �\�����[�`�����Ăяo��...
        Thread thread = new Thread()
        {
            public void run()
            {
                // ���b�Z�[�W�\�z���Ƃ���
                buildingMessage = true;

                // �f�[�^�\�����������s����
                messageParser.prepareMessage();

                // �X���^�C�g�������߂�...
                setTitle(messageParser.decideThreadTitle());

                // ���b�Z�[�W�\�z������������
                buildingMessage = false;

                // �X����\������
                showMessageMain();
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
        thread = null;
        title = null;
        System.gc();
        return;
    }

    /**
     *  �X���������̎��s
     *
     */
    private void setSearchKeyword()
    {
        String defaultKeyword = "";

        int startPos = parsedString.indexOf("ID:");
        if (startPos >= 0)
        {
            int endPos = parsedString.indexOf("\r", startPos);
            if (endPos >= 0)
            {
                defaultKeyword = parsedString.substring(startPos, endPos);
            }
        }        
        parent.OpenWriteFormUsingWordInputMode(parent.SCENE_MESSAGE_SEARCH, "�X��������", "�L�[���[�h", defaultKeyword, true);
        return;
    }

    // �X�����̕\��...
    private void showMessageInformation()
    {    
        String  data = "";
        data = stringConverter.parsefromSJ(messageParser.getThreadTitle());
        data = data + "\n" + messageParser.getFileName();
        data = data + "\n" + (messageParser.getFileSize() / 1000) + "kB (" + messageParser.getFileSize() + ")\n";
        data = data + parent.getHeapInfo();
        parent.showDialog(parent.SHOW_INFO, 0, "�X�����", data);
        return;
    }

    /**
     *  �����t�@�C����URL�ꗗ���o�͂���...
     * 
     *
     */
    private void outputUrlToMemoTxt()
    {
        String  data = "";
        String  urlList = "";
        for (int index = 0; index < commandList.size(); index++)
        {
            String urlToOpen = (String) commandList.elementAt(index);
            urlList = urlList + urlToOpen + "\r\n";
        }
        data = stringConverter.parsefromSJ(messageParser.getThreadTitle());
        parent.outputMemoTextFile(data, urlList);

        // �K�x�R�����{...
        commandList = null;
        System.gc();
        return;
    }

    /**
     *  ���X�̃R�s�[
     *  
     */
    private void messageEditText(boolean isEditMode)
    {
        // ���X�ҏW���[�h�ŕ\������
        if (isEditMode == true)
        {
            String  data = "";
            data = stringConverter.parsefromSJ(messageParser.getThreadTitle());
            parent.OpenWriteFormUsingTextEditMode(data, parsedString);
            return;
        }

        // ���X�̃t�@�C���o�͂����{����
        Thread thread = new Thread()
        {
            public void run()
            {
                buildingMessage = true;
                String  data = "";
                data = stringConverter.parsefromSJ(messageParser.getThreadTitle());
                parent.outputMemoTextFile(data, parsedString);
                buildingMessage = false;
            }
        };
        try
        {
            // ���X�̃t�@�C���o�͂��I���܂ő҂�...
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            //
        }
        return;
    }

    /**
     * ���X���̃W�����v���X�g�𒊏o����
     * 
     */
    private void pickUpJumpList()
    {
        // �������̓f�[�^���Ȃ���ΏI������
        if (parsedString == null)
        {
            return;
        }

        // ���X�ԍ�����͂���
        int index      = 0;
        int limit      = parsedString.length();
        int maxMesNum  = messageParser.getNumberOfMessages();
        buildNofPickupList = 0;
        
        // �܂��A���O���ɐ������L�q����Ă��邩�m�F����
        try
        {
            int nameNum = Integer.parseInt(messageParser.getNameField());
            if ((nameNum > 0)&&(nameNum <= maxMesNum))
            {
                // �������L�ڂ���Ă����I �ǉ�����
                pickupMessageList[buildNofPickupList] = nameNum;
                buildNofPickupList++;
            }
        }
        catch (Exception e)
        {
            // ���O���͐����ł͂Ȃ������A�������Ȃ�
        }

        // ���X���ɐ��������邩�`�F�b�N����
        while (index < limit)
        {
            int startNumber = parsedString.indexOf(">", index);
            if (startNumber < index)
            {
                // ��͏I��...������
                break;
            }
            startNumber++;
            int pickupNumber = 0;
            char checkChar = parsedString.charAt(startNumber);
            while (checkChar == '>')
            {
                startNumber++;
                checkChar = parsedString.charAt(startNumber);
            }
            while ((checkChar >= '0')&&(checkChar <= '9'))
            {
                int addCount = checkChar - '0';
                pickupNumber = pickupNumber * 10 + addCount;
                startNumber++;
                checkChar = parsedString.charAt(startNumber);
            }
            if ((pickupNumber > 0)&&(buildNofPickupList < nofPickupMessageList)&&(pickupNumber <= maxMesNum))
            {
                // ���o�������l������
                pickupMessageList[buildNofPickupList] = pickupNumber;
                buildNofPickupList++;
            }
            index = startNumber;
        }

        // �����Ɂu�ꗗ�֖߂�v��ǉ�����
        pickupMessageList[buildNofPickupList] = -1;            
        buildNofPickupList++;

        return;        
    }
    
    // NG�L�[���[�h��o�^����
    public void setNGword(String dataString)
    {
        ngWordManager.entryNgWord(dataString, ngWordManager.CHECKING_ALL);
        backupNGword();
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

    // NG�L�[���[�h���o�b�N�A�b�v����
    public void backupNGword()
    {
        // �\�����[�`�����Ăяo��...
        Thread thread = new Thread()
        {
            public void run()
            {
                // NG���[�h���o�b�N�A�b�v����
                ngWordManager.backup();
            }
        };
        thread.start();
        return;
    }
    
    // NG�L�[���[�h���܂܂�Ă��邩�`�F�b�N����
    private boolean checkNgKeyword(String checkString)
    {
        int count = ngWordManager.numberOfNgWords();
        for (int index = 0; index < count; index++)
        {
            String word   = ngWordManager.getNgWord(index);
//            int checkType = ngWordManager.getNgCheckingType(index);
            if (word != null)
            {
                //parent.showDialog(parent.SHOW_INFO, 0, "<< NG���[�h�`�F�b�N >> " , "word:" + word + "[" + index + "/" + count + "]" + "\n\n" + parsedString);
                // ���X�S�̂�NG���[�h���܂܂�Ă��邩
                if (checkString.indexOf(word, 0) >= 0)
                {
                    // NG���[�h���܂܂�Ă��邱�Ƃ����o�����I�I
                    return (true);
                }
            }
        }
        return (false);
    }

    
    /**
     *  URL�𒊏o����
     * @param data
     * @return
     */
    private void pickupUrl(String data)
    {
        // WebBrowser�ɓn��URL�����X���璊�o���A�I�����Ƃ���...
        int index    = 0;
        int limit    = data.length();
        while (index < limit)
        {
            int startUrl = data.indexOf("tp://", index);
            if (startUrl < index)
            {
                // ��͏I��...������
                break;
            }
            // ���oURL�̖�������͂���
            int endUrl = startUrl;
            char checkChar = data.charAt(endUrl);
            // �Ă��Ɓ[�ɔ��p�����̂ݒ��o����... ('"' �����傫���A'~' �ȉ��̕����R�[�h����������)
            while ((checkChar > ' ')&&(checkChar != '"')&&(checkChar != '>')&&((checkChar <= 0x007e)))
            //    while ((checkChar > ' ')&&(checkChar != '>'))
            {
                endUrl++;
                checkChar = data.charAt(endUrl);
            }
            String url = new String("ht" + data.substring(startUrl, endUrl));
            commandList.addElement(url);
            url   = null;
            index = endUrl;
        }
        return;
    }
    
    /**
     *  URL�𒊏o���� ("www." ����͂��܂镶����𒊏o����)
     * @param data
     * @return
     */
    private void pickupUrlBulkString(String data)
    {
        // WebBrowser�ɓn��URL�����X���璊�o���A�I�����Ƃ���...
        int index    = 0;
        int limit    = data.length();
        while (index < limit)
        {
            int startUrl = data.indexOf("www.", index);
            if (startUrl < index)
            {
                // ��͏I��...������
                break;
            }
            int pickUrl = data.indexOf("tp://www.", index);
            if ((pickUrl + 5) == startUrl)
            {
                // ���o�ΏۊO�Ƃ���inex���ɐi�߂�
                index = startUrl + 1;
                continue;
            }
            // ���oURL�̖�������͂���
            int endUrl = startUrl;
            char checkChar = data.charAt(endUrl);
            // �Ă��Ɓ[�ɔ��p�����̂ݒ��o����... ('"' �����傫���A'~' �ȉ��̕����R�[�h����������)
            while ((checkChar > ' ')&&(checkChar != '"')&&(checkChar != '>')&&((checkChar <= 0x007e)))
            {
                endUrl++;
                checkChar = data.charAt(endUrl);
            }
            String url = new String("http://" + data.substring(startUrl, endUrl));
            int chkPos = url.indexOf(".");
            if (chkPos > 0)
            {
                int chkPos2 = url.indexOf(".", (chkPos + 2));
                if (chkPos2 >= 0)
                {
                    // . �� 2�ȏ゠�����Ƃ������AURL�Ƃ��ĔF������B
                    commandList.addElement(url);                    
                }
            }
            url   = null;
            index = endUrl;
        }
        return;
    }
    
    /**
     *   Web�u���E�U�̋N������
     *
     */
    private boolean launchWebBrowser()
    {
        // �������̓f�[�^���Ȃ���ΏI������
        if (parsedString == null)
        {
            return (true);
        }

        // ���o����URL���i�[���镶�����������
        commandList = null;
        commandList = new Vector();

        pickupUrl(parsedString);
        pickupUrlBulkString(parsedString);
        if (commandList.size() != 0)
        {
            // ���X����URL���P���ȏ゠�����ꍇ�ɂ́A�I�������J��
            parent.openSelectForm("�u���E�U�ŊJ��", commandList, 0, parent.CONFIRM_URL_FOR_BROWSER);
        }
        else
        {
            // ���X����URL���P�����Ȃ������A�A�A���������ɏI������
            commandList = null;
            System.gc();
            return (true);
        }
        return (true);
    }

    /**
     *  �X���ԍ��w��擾�@�\�̎��s����
     *  
     */
    private void pickupThreadNumber()
    {
        // �������̓f�[�^���Ȃ���ΏI������
        if (parsedString == null)
        {
            return;
        }

        // ���X����A'/read.cgi/xxxx/yyyyyyyy' �� xxxx/yyyyyyyy �����𒊏o����
        commandList = null;
        commandList = new Vector();
        int index     = 0;
        int limit     = parsedString.length();
        int nofThread = 0;
        while (index < limit)
        {
            int startUrl = parsedString.indexOf("/read.cgi/", index);
            if (startUrl < index)
            {
                // ��͏I��...������
                break;
            }
            // '/read.cgi/' ��ǂݔ�΂�
            startUrl = startUrl + 10;
            
            // �X���ԍ��̖�������͂���
            boolean firstSeparator = false;
            int endUrl = startUrl;
            char checkChar = parsedString.charAt(endUrl);
            while ((checkChar > ' ')&&(checkChar != '>')&&(checkChar != 0))
            {
                if (checkChar == '/')
                {
                    if (firstSeparator == true)
                    {
                        break;
                    }
                    firstSeparator = true;
                }
                endUrl++;
                checkChar = parsedString.charAt(endUrl);
            }
            String url = new String(parsedString.substring(startUrl, endUrl));
            commandList.addElement(url);
            nofThread++;
            url   = null;
            index = endUrl;
        }
        if (nofThread != 0)
        {
            // ���X���ɃX���ԍ����P���ȏ゠�����ꍇ�ɂ́A�I�������J��
            parent.openSelectForm("�X���ԍ��w��擾", commandList, 0, parent.CONFIRM_GET_THREADNUMBER);
        }
        else
        {
            // ���X����URL���P�����Ȃ������A�A�A���������ɏI������
            commandList = null;
            System.gc();
        }
        return;
    }

    /**
     *  ���X�o�̓t�@�C�����̕ύX�B�B�B
     * 
     *
     */
    private void changeOutputFileName()
    {
        // �\�����[�`�����Ăяo��...
        Thread thread = new Thread()
        {
            public void run()
            {
                // ���݂̃t�@�C�������擾����
                String fileName = parent.getOutputMemoFileName();
            
                // �t�@�C�����ύX�p��ʂ��J��
                parent.OpenWriteFormUsingWordInputMode(parent.SCENE_MEMOFILE_INPUT, "�����t�@�C�����ύX", "�t�@�C����", fileName, false);
            }
        };
        thread.start();
        return;
    }
    
    /**
     *  �R�}���h�̎��s...
     * 
     * 
     */
    public void executeCommandAction(byte scene, int index, int appendInfo)
    {
        if (scene == parent.CONFIRM_URL_FOR_BROWSER)
        {
            if (appendInfo == 2)
            {
                // ��������URL���o�͂���
                outputUrlToMemoTxt();

                informationColor = 0x0000f0f0;
                drawInformation = true;
                repaint();
                return;
            }

            // �I�������R�s�[���āA�u���E�U�ŃI�[�v��������
            String urlToOpen = (String) commandList.elementAt(index);
            commandList = null;
            if (urlToOpen.indexOf("http://") < 0)
            {
                // HTTP�ł͂Ȃ������B�B�B
                commandList = null;
                System.gc();
            }

            if ((appendInfo == 3)||(appendInfo == 4))
            {
                // �����N����擾����...
                if (appendInfo == 4)
                {
                    currentGwtPageNum = -1;
                    useCacheDirectory = true;
                    previewURL = decideUrlForPreview(urlToOpen);
                }
                else
                {
                    useCacheDirectory = false;
                    previewURL = urlToOpen;
                }
                Thread thread = new Thread()
                {
                    public void run()
                    {
                        // HTTP�ʐM�Ńt�@�C�����擾����
                        getFileUsingHttp(previewURL, useCacheDirectory);
                    }
                };
                try
                {
                    thread.start();
                }
                catch (Exception ex)
                {
                    setBusyMessage("EX:" + ex.getMessage(), false);
                    repaint();
                }
                System.gc();
                return;
            }

            // �u���E�U�ŊJ��...
            parent.openWebBrowser(urlToOpen);
            System.gc();

            informationColor = 0x0000f0f0;
            drawInformation = true;
            repaint();
            return;
        }
        else if (scene == parent.CONFIRM_GET_THREADNUMBER)
        {
            // �X���ԍ��w��擾����...
            reservedThreadNumber = (String) commandList.elementAt(index);


            
            // ��ʂ𔲂���...
            if (appendInfo == 1)
            {
                finishShowMessage();
            }
            else
            {
                // �����̒l���擾����
                int bottom = messageParser.getNumberOfMessages();
                
                // ���݂̖��ǈʒu���L�^����
                parent.setCurrentMessage(messageNumber, bottom);
            }
            
            // ���͂��ꂽ�X���ԍ��̉�͂��˗�����
            parent.parseMessageThreadNumber(reservedThreadNumber, appendInfo);
            reservedThreadNumber = null;
        }
        else if (scene == parent.CONFIRM_SELECT_OSUSUME2CH)
        {
            // �������߂Q�����˂�̕\��...
            String buffer = (String) commandList.elementAt(index);
            int pos = buffer.indexOf("<>/");
            if (pos >= 0)
            {
                // �X���ԍ�������؂�o��..
                pos = pos + 3;
                reservedThreadNumber = (String) buffer.substring(pos);

                // ��ʂ𔲂���...
                if (appendInfo == 1)
                {
                    finishShowMessage();
                }
                // ���͂��ꂽ�X���ԍ��̉�͂��˗�����
                parent.parseMessageThreadNumber(reservedThreadNumber, appendInfo);
                reservedThreadNumber = null;
            }
        }
        commandList = null;
        System.gc();
        return;
    }
    

    /**
     *   ���ݎQ�ƒ��̃X�������C�ɓ���X���Ƃ��ēo�^����
     *
     *
     */
    private void setFavoriteThread()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.setFavoriteThread(currentMessageIndex);
                informationColor = 0x000000f0;
                drawInformation = true;
                repaint();
            }
        };
        thread.start();
        return;
    }
    
    /**
     * �������[�h�̎��{
     * 
     * @param begin
     */
    private void executeSearchKeyword(int direction)
    {
        // �������s���̂Ƃ��ɂ͉������Ȃ�...
        if (isSearchExecution == true)
        {
            return;
        }

        // ����������ݒ肵�āA��������
        searchDirection = direction;
        isCommandMode = false;
        Thread thread = new Thread()
        {
            public void run()
            {
                executeSearchKeywordMain();
            }
        };
        thread.start();
        
        return;
    }

    /**
     * �������[�h�̎��{(���C������)
     * 
     * @param begin
     */
    private void executeSearchKeywordMain()
    {
        int bottom;  // �����I���̏ꏊ���w�肷��...
        isSearchExecution = true;

        // �Ƃ肠�����A��ʍX�V..
        repaint();
        
        if (searchDirection > 0)
        {
            // �������̌���
            bottom = messageParser.getNumberOfMessages() + 1;
        }
        else
        {
            // �������̌���
            bottom = 0;
        }

        // ��ʕ`��I���܂ő҂�...
        while ((painting == true)||(buildingMessage == true))
        {
            parent.sleepTime(1);
        }
        System.gc();
        
        // ���������s����...
        for (int resNum = (messageNumber + searchDirection); resNum != bottom; resNum = resNum + searchDirection)
        {
            if (existKeyword(resNum) == true)
            {
                while ((painting == true)||(buildingMessage == true))
                {
                    parent.sleepTime(1);
                }
                System.gc();
                currentPage = 0;
                showMessage(resNum);
                System.gc();
                isSearchExecution = false;
                return;
            }
        }
        // ���������񂪌�����Ȃ�����...
        String  data = "�����񂪌�����܂���\n������F" + searchMessageString;
        parent.showDialog(parent.SHOW_INFO, 0, "�����I��", data);
        showMessage(messageNumber);
        System.gc();
        isSearchExecution = false;
        return;
    }

    /**
     *  BE profile�Q��
     * 
     *
     */
    private void viewBeProfile()
    {
        // BE��ID�����X���ɂ��邩�ǂ����`�F�b�N����
        int pos = parsedString.indexOf("BE:");
        int endPos = parsedString.indexOf("-", pos + 3);
        if ((pos >= 0)&&(endPos >= 0))
        {
            String id = parsedString.substring((pos + 3), (endPos));
            previewURL = "http://be.2ch.net/test/p.php?i=" + id;
            
            Thread thread = new Thread()
            {
                public void run()
                {
                    parent.GetHttpCommunication(null, previewURL, null, "Be Profile", 0, 0, parent.SCENE_PREVIEW_MESSAGE_EUC, false);
                }
            };
            thread.start();
        }
        
        return;
    }

    /**
     *  �������߂Q�����˂�̕\��
     */
    private void viewOsusume2ch()
    {
        String url = parent.getCurrentMessageBoardUrl() + "i/" + parent.getThreadFileName(currentMessageIndex);
        previewURL = url.substring(0, (url.length() - 3)) + "html";
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.GetHttpCommunication(null, previewURL, null, "��������2ch", 0, 0, parent.SCENE_GET_OSUSUME_2CH, false);
                parent.unlockCommunicationScreen();   // BUSY�X�N���[���̃��b�N���͂���
            }
        };
        // BUSY�X�N���[���̃��b�N������
        parent.lockCommunicationScreen();
        parent.showBusyMessage("", "��������2ch�`�F�b�N��...", -1);
        thread.start();
    }

    /**
     *   �������߂Q�����˂�̃f�[�^�p�[�X
     *
     */
    public boolean parseOsusume2ch(byte[] getString)
    {
        // BUSY�X�N���[���̃��b�N���͂���
        parent.lockCommunicationScreen();

        // �������̓f�[�^���Ȃ���ΏI������
        if (getString == null)
        {
            return (true);
        }
        String dataBuffer = new String(getString);
        
        // WebBrowser�ɓn��URL�����X���璊�o���A�I�����Ƃ���...
        commandList = null;
        commandList = new Vector();
        int index    = 0;
        int limit    = dataBuffer.length();
        int urlCount = 0;
        while (index < limit)
        {
            int startUrl = dataBuffer.indexOf("read.cgi/", index);
            if (startUrl < 0)
            {
                break;
            }
            startUrl = startUrl + 9;
            if (startUrl < index)
            {
                // ��͏I��...������
                break;
            }
            // ���oURL�̖�������͂���
            boolean firstSlash = false;
            int endUrl = startUrl;
            char checkChar = dataBuffer.charAt(endUrl);
            while (checkChar != '"')
            {
                endUrl++;
                checkChar = dataBuffer.charAt(endUrl);
                if (checkChar == '/')
                {
                    if (firstSlash == true)
                    {
                        break;
                    }
                    firstSlash = true;
                }
            }
            int titleStartPos = dataBuffer.indexOf(">", endUrl);
            int titleEndPos   = dataBuffer.indexOf("<", endUrl);
            String title = dataBuffer.substring((titleStartPos + 1), titleEndPos);
            String url = new String(title + " <>/" + dataBuffer.substring(startUrl, endUrl));
            commandList.addElement(url);
            urlCount++;
            url   = null;
            index = endUrl;
        }
        if (urlCount != 0)
        {
            // ���X����URL���P���ȏ゠�����ꍇ�ɂ́A�I�������J��
            parent.openSelectForm("�������߂Q�����˂�", commandList, 0, parent.CONFIRM_SELECT_OSUSUME2CH);
        }
        else
        {
            // ���X����URL���P�����Ȃ������A�A�A���������ɏI������
            commandList = null;
            busyMessage ="��������2ch:�݂���܂���B";
            drawInformation = true;
            repaint();
            System.gc();
            return (true);
        }
        return (true);
    }
    
    /**
     * �����񂪊܂܂�Ă��邩�ǂ����m�F����
     * 
     * @param messageNumber
     * @return
     */
    private boolean existKeyword(int messageNumber)
    {
        // ���X�f�[�^���擾����
        String message = stringConverter.parsefromSJ(messageParser.getMessage(messageNumber, parent.getViewKanjiMode()));
        if (searchIgnoreCase != true)
        {
            // �啶��/����������ʂ���ꍇ...
            int pos = message.indexOf(searchMessageString);
            message = null;
            if (pos < 0)
            {
                return (false);
            }
            return (true);
        }

        // �啶��/����������ʂ��Ȃ��ꍇ...
        String target     = message.toLowerCase();
        String matchData  = searchMessageString.toLowerCase();
        if (target.indexOf(matchData) >= 0)
        {
            return (true);
        }
        return (false);
    }

    /**
     *   URL����t�@�C�������擾����
     * 
     * @param url URL
     * @return    �t�@�C����
     */
    private String decideFileNameUsingUrl(String url)
    {
        String fileName = "index.html";
        boolean modifyFileName = false;
        int pos = url.lastIndexOf('/');
        if (url.length() > (pos + 1))
        {
            // �t�@�C�����̒������s��...
            fileName = url.substring(pos + 1);
            pos = fileName.lastIndexOf('?');
            if (pos >= 0)
            {
                fileName = fileName.substring(pos + 1);
                modifyFileName = true;
            }
            pos = fileName.lastIndexOf('*');
            if (pos >= 0)
            {
                fileName = fileName.substring(pos + 1);
                modifyFileName = true;
            }
            pos = fileName.lastIndexOf('%');
            if (pos >= 0)
            {
                fileName = fileName.substring(pos + 1);
                modifyFileName = true;
            }
            if (modifyFileName == true)
            {
                // �t�@�C�����̕␳�������ꍇ�ɂ́A������html������...
                fileName = fileName + ".html";
            }
        }
        return (fileName);
    }
    
    /**
     *  �v���r���[�ŊJ��URL�𒲐�����
     * 
     * @param urlToOpen
     * @return
     */
    private String decideUrlForPreview(String urlToOpen)
    {
    	String url = urlToOpen;
        if (urlToOpen.indexOf("imepita.jp") >=  0)
        {
        	// �C���҂��̏ꍇ�́A���̂܂܉�������
            return (url);
        }
        if (urlToOpen.indexOf("pita.st") >=  0)
        {
        	// ���҂��̏ꍇ�́A���̂܂܉�������
            return (url);
        }
        if (((urlToOpen.indexOf(".jpg") > 0)||(urlToOpen.indexOf(".JPG") > 0))||
             ((urlToOpen.indexOf(".gif") > 0)||(urlToOpen.indexOf(".GIF") > 0))||
             ((urlToOpen.indexOf(".png") > 0)||(urlToOpen.indexOf(".PNG") > 0)))
        {
            // �摜�t�@�C���̏ꍇ�ɂ́A tinysrc �o�R�ŃC���[�W�t�@�C�����擾���邱�Ƃɂ���
        	//  (http://i.tinysrc.mobi/����/�c��/�I���W�i����URL)
            return ("http://i.tinysrc.mobi/240/320/" + url);
        }
        if (urlToOpen.indexOf("http://") == 0)
        {
            url = "http://www.google.co.jp/gwt/n?u=http%3A%2F%2F" + urlToOpen.substring(7);
        }
        return (url);
    }
    
    /**
     * HTTP�ʐM�Ńt�@�C�����擾����
     * 
     * @param urlToGet  �擾����URL
     */
    private void getFileUsingHttp(String url, boolean useCacheDir)
    {
        // URL�̒��o�ɐ��������Ƃ�����http�ʐM�����s����...
        setBusyMessage("�ʐM���F" + url, false);

        // URL����t�@�C�����𒊏o����
        String fileName = decideFileNameUsingUrl(url);
        
        // bbstable.html ���㏑������悤�ȏ����͋֎~����B�B�B
        if ((parent.getSaveAtRootDirectory() == true)&&(fileName.indexOf("bbstable.html") >= 0))
        {
            setBusyMessage(null, false);
            parent.vibrate();
            System.gc();
            return;
        }

        // gwt�̃y�[�W�␳...
        if (currentGwtPageNum > 0)
        {
            url = url + "&_gwt_pg=" + currentGwtPageNum;
        }

        String directory = parent.getBaseDirectory();
        if (useCacheDir == true)
        {
            // �L���b�V���f�B���N�g���Ƀt�@�C����ۊǂ���
            directory = directory + "a2B_Cache";
            parent.makeDirectory(directory);
            directory = directory + "/";
            
            if ((url.indexOf("imepita.jp") > 0)||(url.indexOf("pita.st") > 0))
            {
                // ���߃s�^ ���邢�� ���҂� �̏ꍇ�ɂ́A�g���q(jpeg)��ǉ�����
                fileName = fileName + ".jpg";
            }
            if (((fileName.indexOf(".jpg") > 0)||(fileName.indexOf(".JPG") > 0))||
                 ((fileName.indexOf(".gif") > 0)||(fileName.indexOf(".GIF") > 0))||
                 ((fileName.indexOf(".png") > 0)||(fileName.indexOf(".PNG") > 0)))  
            {
                //  �擾����t�@�C���̊g���q�� .jpg �܂��� .png �܂��� gif �̏ꍇ�A
                // �܂��� imepita.jp �̏ꍇ�A
                // Google Mobile Proxy�o�R�ŏk�������摜���擾���ĕ\������
                getCachePicturesUsingHttp(directory, fileName, url);
                showPreviewPictures(directory, fileName);
            }
            else
            {
                // �t�@�C�����L���b�V�����A�v���r���[����
                getCacheFileUsingHttp(directory, fileName, url);
            }
            finishGetFile();
            return;
        }
 
        if (parent.getSaveAtRootDirectory() != true)
        {
            directory = parent.getFileSaveDirectory();
            parent.makeDirectory(directory);
            directory = directory + "/";
        }

        int rc = parent.doHttpMain(url, (directory + fileName), null, parent.getUseCachePictureFile(),  a2BMain.a2B_HTTPMODE_NOUSECOOKIE);
        if (rc < 0)
        {
            // �ʐM�Ɏ��s�����ꍇ...�ł��AWX310�̂Ƃ��ɂ́A���ꂪ�Ԃ��Ă���...
            informationColor = 0x0000f0f0;
            crossColor       = 0x00010101;
            drawInformation = true;
            repaint();
            System.gc();
        }
        setBusyMessage("http�ʐM�I�� : " + fileName, false);
        if (((fileName.indexOf(".jpg") > 0)||(fileName.indexOf(".JPG") > 0))||
            ((fileName.indexOf(".gif") > 0)||(fileName.indexOf(".GIF") > 0))||
            ((fileName.indexOf(".png") > 0)||(fileName.indexOf(".PNG") > 0)))
        {
            // �g���q .jpg �܂��� .png �܂��� gif �̏ꍇ�ɂ́A�T���l�[��������\������...
            showPreviewPictures(directory, fileName);
        }
        finishGetFile();
        return;
    }

    /**
     *  �摜�t�@�C���̃v���r���[�����{����
     * 
     * @param directory �f�B���N�g����
     * @param fileName  �t�@�C����
     */
    private void showPreviewPictures(String directory, String fileName)
    {
        // �g���q .jpg �܂��� .png �܂��� gif �̏ꍇ�ɂ́A�T���l�[��������\������...
        jpegPreviewImage = parent.loadJpegThumbnailImageFromFile(directory + fileName);
        if (jpegPreviewImage == null)
        {
            // JPEG�t�@�C���\�����s...
            setBusyMessage("�\�����s(0:�폜) : " + fileName, false);
        }
        else
        {
            // JPEG�t�@�C���̕\�����s��...
            setBusyMessage("0:�L���b�V���폜 (" + fileName + ")", false);
        }
        previewJpeg = true;
        displayFileName = directory + fileName;
        return;        
    }
    
    /**
     *  �t�@�C���擾�̌㏈��...
     *
     */
    private void finishGetFile()
    {
        parent.vibrate();
        informationColor = 0x0000f0f0;
        drawInformation = true;
        repaint();
        System.gc();
        return;
    }

    /**
     *  �L���b�V���t�@�C���̎擾 (�摜�\���p)
     * 
     * @param directory
     * @param fileName
     * @param url
     */
    private void getCachePicturesUsingHttp(String directory, String fileName, String url)
    {
        if (parent.getUseCachePictureFile() == true)
        {
            if (a2BfileSelector.IsExistFile(directory + fileName) == true)
            {
                // ���Ƀt�@�C�������݂����ꍇ�A�������Ȃ� (�L���b�V���t�@�C���𗘗p����)
                return;
            }
        }

        String referer = null;
        int rc = parent.doHttpMain(url, (directory + fileName), null, false,  a2BMain.a2B_HTTPMODE_PICKUPCOOKIE);
        String getFileUrl = pickupPreviewPictureFileUrl(directory + fileName);   
        if (url.indexOf("imepita.jp") >= 0)
        {
        	// ������� ���߃s�^�Ή�...
            getFileUrl = "http://imepita.jp/image/" + url.substring(18);
            referer = url;
        }
        if (getFileUrl == null)
        {
            // File���̒��o�Ɏ��s�����ꍇ... ������񎎂��B
            rc = parent.doHttpMain(url, (directory + fileName), referer, false,  a2BMain.a2B_HTTPMODE_USEPREVIOUSCOOKIE);
            getFileUrl = pickupPreviewPictureFileUrl(directory + fileName);
            if (getFileUrl == null)
            {
                setBusyMessage("FNF:" + fileName, false);
                return;
            }
        }

        rc = parent.doHttpMain(getFileUrl, (directory + fileName), referer, false,  a2BMain.a2B_HTTPMODE_USEPREVIOUSCOOKIE);
        if (rc < 0)
        {
            // �ʐM�Ɏ��s�����ꍇ... (�ł��AWX310�̂Ƃ��ɂ́A���ꂪ�Ԃ��Ă���...)
            informationColor = 0x0000f0f0;
            crossColor       = 0x00010101;
            drawInformation = true;
            repaint();
        }
        setBusyMessage("http�ʐM�I�� : " + fileName, false);
        parent.clearHttpCacheCookie();
        System.gc();
        return;
    }
    
    
    /**
     *  �v���r���[����摜�t�@�C����URL���擾����
     * @param fileName �`�F�b�N����t�@�C����
     * @return ���o�����摜�t�@�C����URL
     */
    private String pickupPreviewPictureFileUrl(String fileName)
    {
    	String returnFileUrl = null;
        FileConnection connection    = null;
        InputStream    in            = null;
        byte[]         buffer        = null;
        String          checkBuffer  = null;
        int            dataReadSize  = 0;

        try
        {
            connection   = (FileConnection) Connector.open(fileName, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  �f�[�^���ŏ����� size���ǂݏo��...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + 4];
            dataReadSize = in.read(buffer, 0, size);
            checkBuffer = new String(buffer, 0, dataReadSize);
            in.close();
            connection.close();

            // �ǂݏo���Ɏg�����ϐ����N���A����
            in = null;
            connection = null;
            buffer = null;

            int pos0 = checkBuffer.indexOf("<img");
            int pos1 = -1;
            int pos2 = -1;
            if (pos0 >= 0)
            {
                pos1 = checkBuffer.indexOf("src=\"", pos0);
            }
            if (pos1 >= 0)
            {
                pos2 = checkBuffer.indexOf("\"", (pos1 + 6));
            }
            if ((pos2 >= 0)&&(pos1 >= 0)&&(pos1 < pos2))
            {
            	if (checkBuffer.indexOf("pita.st") > 0)
            	{
            		// ���҂��̏ꍇ...
            		returnFileUrl = "http://i.tinysrc.mobi/240/320/" + checkBuffer.substring((pos1 + 5), (pos2));
            	}
            	else
            	{
            		// ����ȊO�̏ꍇ
                    returnFileUrl = "http://www.google.co.jp" + checkBuffer.substring((pos1 + 5), (pos2));
            	}
            }
        }
        catch (Exception ex)
        {
            // exception...
            in = null;
            connection = null;
            buffer = null;
            returnFileUrl = null;
        }
        checkBuffer = null;
        return (returnFileUrl);
    }

    /**
     *  �L���b�V���t�@�C���̎擾 (�e�L�X�g�v���r���[�p)
     * 
     * @param directory
     * @param fileName
     * @param url
     */
    private void getCacheFileUsingHttp(String directory, String fileName, String url)
    {
        // HTTP�ʐM�Ńf�[�^���擾����
        int rc = parent.doHttpMain(url, (directory + fileName), null, parent.getUseCachePictureFile(),  a2BMain.a2B_HTTPMODE_NOUSECOOKIE);
        if (rc < 0)
        {
            // �ʐM�Ɏ��s�����ꍇ... (�ł��AWX310�̂Ƃ��ɂ́A���ꂪ�Ԃ��Ă���...)
            informationColor = 0x0000f0f0;
            crossColor       = 0x00010101;
            drawInformation = true;
            repaint();
            System.gc();
        }
        setBusyMessage("http�ʐM�I�� : " + fileName, false);

        // �L���b�V���̏ꍇ�A�t�@�C���̃v���r���[���d�|���Ă݂�..
        previewText = true;
        displayFileName = directory + fileName;
        int mode = a2BsubjectDataParser.PARSE_UTF8;
        if (parent.getWX220JUserAgent() != true)
        {
            mode = a2BsubjectDataParser.PARSE_SJIS;
        }
        parent.doPreviewFile(displayFileName, true, mode);
        setBusyMessage("0:File�폜 (" + fileName + ")", false);
        
        return;
    }

    /**
     *  �L�[���s�[�g���Ԃ̐ݒ�(�P�ʁFms)
     * @param ms �L�[���s�[�g����
     */
    public void setKeyRepeatDelay(int ms)
    {
    	keyRepeatDelayMs = (long) ms;
    }

    /**
     *  �L�[���s�[�g���Ԃ̎擾(�P��:ms)
     * @return �L�[���s�[�g����
     */
    public int getKeyRepeatDelay()
    {
    	return ((int) keyRepeatDelayMs);
    }

    /**
     * ������������͂����������ꍇ...
     * 
     * @param keyWord
     */
    public void setSearchString(String keyWord, byte searchStatus, boolean ignoreCase)
    {
        searchIgnoreCase = ignoreCase;
        currentPage = 0;
        if ((keyWord == null)||(keyWord.compareTo("") == 0))
        {
            // �������[�h����������
            searchMessageString = null;
            isSearchingMode = false;
            showMessage(messageNumber);
            return;
        }

        // �����������ǉ�����
        searchMessageString = keyWord;
        isSearchingMode = true;
        isCommandMode   = false;

        // �߂��ꏊ�ɒǉ�
        if (jumpListIndex < nofJumpList)
        {
            messageJumpList[jumpListIndex] = messageNumber;
            jumpListIndex++;
        }
        else
        {
            messageJumpList[nofJumpList - 1] = messageNumber;            
        }
        showMessage(messageNumber);
        executeSearchKeyword(1);        
        return;
    }

    /**
     *   �X���ԍ��w��擾�̓o�^�������������Ƃ�ʒm����
     *
     */
    public void completedParseMessageThreadNumber(boolean result)
    {
        triangleColor = 0x00f000f0;
        if (result == false)
        {
            triangleColor = 0;
            crossColor = 0x00f000f0;
//          busyMessage ="�ԍ��w��擾���s";
        }
        else
        {
//          busyMessage ="�ԍ��w��擾����";
        }
        drawInformation = true;
        repaint();
        return;
    }

    /**
     *   ���X�̃t�@�C���o�͂������������Ƃ�ʒm����
     *
     */
    public void completedOutputMessage(boolean result)
    {
        triangleColor = 0x00f0f018;
        if (result == false)
        {
            triangleColor = 0;
            crossColor = 0x00f000f0;
            busyMessage ="�o�͎��s...";
        }
        else
        {
            busyMessage ="�o�͏I��";
        }
        drawInformation = true;
        repaint();
        return;
    }

    /**
     *   �w���v�C���[�W��\������
     *
     */
    private void changeHelpShowMode()
    {
        if (viewHelpImage == null)
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
     *  �ǎ��E����w���v�̃C���[�W��ǂݍ���
     * 
     * 
     */
    public void loadWallPaperImage()
    {
        wallPaperImage = parent.loadImageFromFile("a2B_WallPaper.png");
        viewHelpImage  = parent.loadImageFromFile("a2B_ViewHelp.png");
        return;
    }
    
}
//--------------------------------------------------------------------------
//  a2BMessageViewForm  --- �X�����b�Z�[�W�\���t�H�[�� (���X�̕\���t�H�[��)
//
//
//--------------------------------------------------------------------------
// MRSa (mrsa@sourceforge.jp)
