import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import java.util.Vector;

/**
 * �I�����̕\���E�I�� (BUSY���b�Z�[�W�̕\�������p)
 * 
 * @author MRSa
 *
 */
//public class a2BselectionForm  extends Form implements ItemCommandListener, CommandListener
//public class a2BselectionForm  extends List implements CommandListener
public class a2BselectionForm extends Canvas implements CommandListener, Ia2BMidpTimerTimeout
{
	private final int blinkDuration   = 500;     // 500ms���Ƃɓ_��
    private final int backColorOdd    = 0x00f8f8f8;
    private final int backColorEven   = 0x00b0c8c8;
    private final int foreColor       = 0x00000000;
    private final int cursorBackColor = 0x000000f8;
    private final int cursorForeColor = 0x00ffffff;
    private final int defaultTitleForeColor = 0x00f8f8f8;
    private final int defaultTitleBackColor = 0;

    static private final int  COLOR_A = 0x000000f0;
    static private final int  COLOR_B = 0x00c0c000;
    static private final int  COLOR_C = 0x00101010;
    static private final int  COLOR_D = 0x00f0f0f0;
    
    static public final int SELECTION_FORM    = 0;
    static public final int BUSY_FORM         = 1;
    static public final int CONFIRMATION_FORM = 2;
    static public final int INFORMATION_FORM  = 3;
    static public final int BUSY_FORM_UPDATE  = 4;
    
    private a2BMain parent             = null;
    private a2BMidpTimer watchDogTimer = null;

    private int     currentScene      = -1;
    private int     screenKind        = SELECTION_FORM;
    private int     numSelection      = -1;

    private boolean painting         = false;
    private boolean redraw           = true;
    private boolean isBlinking       = false;

    private Font      screenFont      = null;
    private int       screenHeight    = 0;
    private int       screenLines     = 1;

    private int       currentPage     = 0;
    private int       nofPage         = 0;
    private int       selectedLine    = 0;    
    private int       timerTemp       = 0;
    
    private Vector    selectionData   = null;
    private String    titleString     = null;
    private String    busyMessage     = null;
    
    private int      titleBackColor = defaultTitleBackColor;
    private int      titleForeColor = defaultTitleForeColor;
    
    private Command cancelCmd  = new Command("Cancel",          Command.EXIT,   -1);
    private Command selectCmd  = new Command("�I��",            Command.SCREEN,  0);
    private Command confirmCmd = new Command("OK",              Command.OK,      1);
    private Command getMoveCmd = new Command("�ړ�+�擾(�J��)", Command.SCREEN,  2);
    private Command outputCmd  = new Command("Memo�o��",        Command.SCREEN,  3);
    private Command getHttpCmd = new Command("File�擾",        Command.SCREEN,  4);
    private Command getPicsCmd = new Command("�v���r���[",      Command.SCREEN,  5);

    
    /**
     * �R���X�g���N�^
     * 
     * @param object
     */
    public a2BselectionForm(a2BMain object)
    {
        // �^�C�g��������...
        setTitle(null);

        // �t���X�N���[���ɂ���...
        setFullScreenMode(true);

        // �R���X�g���N�^
        parent = object;

        watchDogTimer = new a2BMidpTimer(this);

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
        try
        {
            screenFont = Font.getFont(font.getFace(), font.getStyle(), parent.getListFontSize());
        }
        catch (Exception ex)
        {
        	screenFont = font;
        }

        screenHeight = getHeight();
        screenLines  = (screenHeight / (screenFont.getHeight() + 1)) - 1;

//        repaint();
        return;
    }

    /**
     *   ��ʕ`��̎��{...
     * 
     * 
     */
    public void paint(Graphics g)
    {
        if (painting == true)
        {
            // �`�撆�A�������Ȃ�
            return;
        }
        painting = true;
        try
        {
            // ��ʂ̓h��Ԃ�
            if (redraw == true)
            {
                g.setColor(0x00f8f8f8);
                g.fillRect(0, 0, getWidth(), getHeight());
            }
            g.setFont(screenFont);

            // �^�C�g���̕\��...
            if (titleString != null)
            {
                g.setColor(titleBackColor);
                g.fillRect(0, 0, getWidth(), screenFont.getHeight() + 1);
                g.setColor(titleForeColor);
                g.drawRect(0, 0, getWidth() - 1, screenFont.getHeight() + 1);
                g.drawString(titleString, 2, 0, (Graphics.LEFT | Graphics.TOP));
            }

            // BUSY���b�Z�[�W�̕\��...
            if (busyMessage != null)
            {
                showBusyMessage(g, busyMessage);
            }

            if ((screenKind != SELECTION_FORM)&&(screenKind == INFORMATION_FORM))
            {
                painting = false;
                return;
            }
            else if (screenKind == SELECTION_FORM)
            {
                // �I�����̕\��...
                if (selectionData != null)
                {
                    if (redraw == true)
                    {
                        drawSelectionItems(g, selectionData);
                    }
                    else
                    {
                        drawSelectionItemsPart(g, selectionData);                
                    }
                    redraw = true;
                }
            }

            // �O�y�[�W������ꍇ�A�}�[�N������
            int charWidth = screenFont.stringWidth("��") + 1;
            if (currentPage != 0)
            {
                g.setColor(0x00ff008f);
                g.drawString("��", getWidth() - charWidth, screenFont.getHeight() + 1, (Graphics.LEFT | Graphics.TOP));
            }

            // ���y�[�W������ꍇ�A�}�[�N������
            if (currentPage != (nofPage - 1))
            {
                g.setColor(0x00ff008f);
                g.drawString("��", getWidth() - charWidth, getHeight() - screenFont.getHeight() - 1, (Graphics.LEFT | Graphics.TOP));
            }
        }
        catch (Exception e)
        {
            // �������Ȃ�...
        }
        
        painting = false;
        return;
    }

    /**
     *  �I�𒆂̃A�C�e����\������
     * @param g
     * @param data
     */
    private void drawCurrentSelectedItem(Graphics g, String data, int topX, int topY)
    {
        int drawRange = getWidth() - 1;
        int dataRange = screenFont.stringWidth(data);
        if (dataRange <= drawRange)
        {
            g.drawString(data, topX, topY, (Graphics.LEFT | Graphics.TOP));
        }
        else
        {
                int offset = 0;
                dataRange = data.length();
                while (screenFont.substringWidth(data, offset, (dataRange - offset)) > drawRange)
                {
                    offset++;
                }
                g.drawSubstring(data, offset, (dataRange - offset), topX, topY, (Graphics.LEFT | Graphics.TOP)); 
/*
            g.drawString(data, topX, topY, (Graphics.LEFT | Graphics.TOP));
*/
        }
    }

    /**
     * �I������\������...
     * 
     * @param g
     * @param items
     */
    private void drawSelectionItemsPart(Graphics g, Vector items)
    {
        int topLine = (screenLines - 1) * currentPage;
        for (int index = 0; index < screenLines; index++)
        {
            int lineToShow = index + topLine;
            if (lineToShow >= items.size())
            {
                break;
            }
            if ((lineToShow >= (selectedLine - 1))&&(lineToShow <= (selectedLine + 1)))
            {
                String item = (String) items.elementAt(lineToShow);
                int    y   = (index + 1) * (screenFont.getHeight() + 1) + 1;
                if (lineToShow == selectedLine)
                {
                    g.setColor(cursorBackColor);
                    g.fillRect(0, y, getWidth(), (screenFont.getHeight()));
                    g.setColor(cursorForeColor);
                    drawCurrentSelectedItem(g, item, 1, y);
                }
                else
                {
                    if (items.size() > 10)
                    {
                        if ((lineToShow % 2) == 0)
                        {
                            g.setColor(backColorEven);
                        }
                        else
                        {
                            g.setColor(backColorOdd);
                        }
                    }
                    else
                    {
                        g.setColor(backColorOdd);                        
                    }
                    g.fillRect(0, y, getWidth(), (screenFont.getHeight()));
                    g.setColor(foreColor);
                    g.drawString(item, 1, y, (Graphics.LEFT | Graphics.TOP));        
                }
            }
        }
        return;
    }
    
    /**
     * �I������\������...
     * 
     * @param g
     * @param items
     */
    private void drawSelectionItems(Graphics g, Vector items)
    {
        int topLine = (screenLines - 1) * currentPage;
        for (int index = 0; index < screenLines; index++)
        {
            int lineToShow = index + topLine;
            if (lineToShow >= items.size())
            {
                break;
            }
            String item = (String) items.elementAt(lineToShow);
            int    y   = (index + 1) * (screenFont.getHeight() + 1) + 1;
            if (lineToShow == selectedLine)
            {
                g.setColor(cursorBackColor);
                g.fillRect(0, y, getWidth(), (screenFont.getHeight()));
                g.setColor(cursorForeColor);
                drawCurrentSelectedItem(g, item, 1, y);
            }
            else
            {
                if (items.size() > 10)
                {
                    if ((lineToShow % 2) == 0)
                    {
                        g.setColor(backColorEven);
                    }
                    else
                    {
                        g.setColor(backColorOdd);
                    }
                }
                else
                {
                    g.setColor(backColorOdd);                        
                }
                g.fillRect(0, y, getWidth(), (screenFont.getHeight()));
                g.setColor(foreColor);
                g.drawString(item, 1, y, (Graphics.LEFT | Graphics.TOP));        
            }
        }
        return;
    }
    
    /**
     * �r�W�[���b�Z�[�W��\������
     * 
     * @param g
     */
    private void showBusyMessage(Graphics g, String msg)
    {
        // �\���\�s��ݒ肷��
        int y       = (screenFont.getHeight()) + 4;
        int x       = 0;
        int limitX  = getWidth() - 4;
        g.setColor(0);
        for (int msgIndex = 0; msgIndex < msg.length(); msgIndex++)
        {
            char drawChar = msg.charAt(msgIndex);
            if (drawChar != '\n')
            {
                g.drawChar(drawChar, x, y, (Graphics.LEFT | Graphics.TOP));
                x = x + screenFont.charWidth(drawChar);
            }
            else
            {
                // ���s�R�[�h�̂Ƃ��͕\�������ɉ��s����...
                x = x + limitX;
            }
            if (x >= limitX)
            {
                x = 0;
                y = y + screenFont.getHeight() + 1;
            }
        }
        return;
    }    

    /*
     *   �R�}���h�{�^�����s���̏���
     *   
     */
    public void commandAction(Command c, Displayable d)
    {
        if (screenKind == BUSY_FORM)
        {
            // �I��s�̏ꍇ�́A�L�����Z����������Ă��������Ȃ��B
            return;
        }

        if (c == cancelCmd)
        {
            // �L�����Z���w�����󂯂�...
            inputClear();
        }
        else if (c == selectCmd)
        {
            // �A�C�e�����I�����ꂽ
            inputFire(0);
            return;
        }
        else if (c == confirmCmd)
        {
            // OK��������...
            releaseConfirmation(Canvas.FIRE);
            return;
        }
        else if (c == getMoveCmd)
        {
            // �ړ��{�擾��I������...
            inputFire(1);
        }
        else if (c == outputCmd)
        {
            // memo�o�͂�I������...
            inputFire(2);
        }
        else if (c == getHttpCmd)
        {
            // http�ʐM��I������...
            inputFire(3);
        }
        else if (c == getPicsCmd)
        {
            // �摜�\����I������...
            inputFire(4);
        }
        return;
    }

    /*
     *   �r�W�[���b�Z�[�W��\������
     * 
     * 
     */
    public void showBusyMessage(String title, String message, int usage)
    {
        // ��x�R�}���h����������...
        this.removeCommand(selectCmd);
        this.removeCommand(cancelCmd);
        this.removeCommand(getMoveCmd);
        this.removeCommand(outputCmd);
        this.removeCommand(getHttpCmd);
        this.removeCommand(getPicsCmd);
        this.setCommandListener(null);

        currentPage  = 0;
        nofPage      = 1;
        titleString  = title;
        busyMessage  = message;
        screenKind   = usage;
        switch (screenKind)
        {
          case CONFIRMATION_FORM:
            this.addCommand(confirmCmd);
            this.setCommandListener(this);
            break;

          case BUSY_FORM_UPDATE:
        	// ���X�V���ɂ́A�^�C�}�ݒ���X�V���Ȃ�
            break;

          default:
            // �E�H�b�`�h�b�O�^�C�}��ݒ肷��
            try
            {
                if (watchDogTimer != null)
                {
                    watchDogTimer.startWatchDog(blinkDuration);
                    isBlinking = true;
                }
            }
            catch (Exception ex)
            {
                // �������Ȃ�...
            }
            /**/
            break;
        }
        repaint();
        return;
    }    

    /*
     *   �r�W�[���b�Z�[�W������
     *   
     */
    public void hideBusyMessage()
    {
        titleString      = null;
        busyMessage      = null;
        screenKind       = SELECTION_FORM;
        repaint();
        timerTemp = -1;
        isBlinking = false;
        parent.endSelectForm(-1, -1, -1);    

        // �^�C�g���̐F��ݒ�
        titleBackColor = defaultTitleBackColor;
        titleForeColor = defaultTitleForeColor;

        System.gc();
        return;
    }

    /*
     *   �I���A�C�e���̕\�����s��
     * 
     * 
     */
    public void showSelection(String title, Vector itemList, int topItem, int scene)
    {
        prepareScreenSize();

        // �^�C�g���̐F��ݒ�
        titleBackColor = defaultTitleBackColor;
        titleForeColor = defaultTitleForeColor;

        // �I���A�C�e���̕\�����s��
        titleString      = title;
        busyMessage      = null;
        currentScene     = scene;
        screenKind       = SELECTION_FORM;
        
        if (itemList.size() <= 10)
        {
            selectionData = null;
            selectionData = new Vector(itemList.size());
            for (int loop = 0; loop < itemList.size(); loop++)
            {
                String item = ((loop + 1) % 10) + " : " + (String) itemList.elementAt(loop);
                selectionData.addElement(item);
            }
            numSelection = itemList.size();
        }
        else
        {
            selectionData   = null;
            selectionData   = itemList;
            numSelection    = -1;
        }

        // �R�}���h��ǉ�����
        if ((scene == parent.CONFIRM_SELECT_OSUSUME2CH)||
            (scene == parent.CONFIRM_GET_THREADNUMBER)||
            (scene == parent.CONFIRM_SELECT_FIND2CH))
        {
            this.addCommand(selectCmd);
            this.addCommand(getMoveCmd);
            this.addCommand(cancelCmd);
            this.setCommandListener(this);
        }
        else if (scene == parent.CONFIRM_URL_FOR_BROWSER)
        {
            this.addCommand(selectCmd);
            this.addCommand(outputCmd);
            this.addCommand(getHttpCmd);
            this.addCommand(getPicsCmd);
            this.addCommand(cancelCmd);
            this.setCommandListener(this);            
        }
        else
        {
            this.addCommand(selectCmd);
            this.addCommand(cancelCmd);
            this.setCommandListener(this);
        }

        // �擪�s�i�I���s�j��ݒ肷��
        nofPage     = (itemList.size() / screenLines) + 1;
        if (itemList.size() == screenLines)
        {
            nofPage = 1;
        }
        currentPage = topItem / (screenLines - 1);
        selectedLine = topItem;
        repaint();
        return;
    }
    
    /**
     * �������͂����o����...
     * 
     * @param keyCode
     * @return
     */
    private boolean keyPressedNumber(int keyCode)
    {
        if (numSelection <= 0)
        {
            // �����œ��͂��郂�[�h�łȂ�...
            return (false);
        }

        int code = -1;
        switch (keyCode)
        {
          case Canvas.KEY_NUM1:
            code = 0;
            break;

          case Canvas.KEY_NUM2:
            code = 1;
            break;

          case Canvas.KEY_NUM3:
            code = 2;
            break;

          case Canvas.KEY_NUM4:
            code = 3;
            break;

          case Canvas.KEY_NUM5:
            code = 4;
            break;

          case Canvas.KEY_NUM6:
            code = 5;
            break;

          case Canvas.KEY_NUM7:
            code = 6;
            break;

          case Canvas.KEY_NUM8:
            code = 7;
            break;

          case Canvas.KEY_NUM9:
            code = 8;
            break;

          case Canvas.KEY_NUM0:
            code = 9;
            break;

          default:
            // �����L�[�ȊO�̓��͂�����...
            return (false);
            //break;
        }
        if ((code >= 0)&&(code < numSelection))
        {
            // �A�C�e�����I�����ꂽ�I�I
            selectedLine = code;

            // �L���Ȑ��l���͂������ꍇ�́AEnter����͂��I������
            serviceRepaints();
            inputFire(0);
        }
        repaint();
        return (true);
    }


    /*
     *  ���������W����������...
     * 
     * 
     */
    public void pointerPressed(int x, int y)
    {
        int currentSelectedLine = (screenLines - 1) * currentPage + (y / (screenFont.getHeight() + 1)) - 1;

        if (x > (getWidth() - (screenFont.charWidth('��') * 2)))
        {
            // ��ʂ̉E��[�A�E���[���^�b�v�����ꍇ...
            int index =  y / (screenFont.getHeight() + 1);
            if (index <= 1)
            {
                // �O�y�[�W�Ɉړ�����
                if (inputLeft() == true)
                {
                    repaint();
                }
                return;
            }
            else if (index >= (screenLines - 1))
            {
                // ���y�[�W�Ɉړ�����
                if (inputRight() == true)
                {
                    repaint();
                }
                 return;
            }
        }
        
        if (currentSelectedLine == selectedLine)
        {
            // �A�C�e���I���A�A�A
            inputFire(0);
            return;
        }
        if (currentSelectedLine < selectionData.size())
        {
            selectedLine = currentSelectedLine;
            repaint();
        }
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

    /**
     * ��ʃT�C�Y�ύX���ɌĂ΂�鏈��...
     * 
     * 
     */
    public void sizeChanged(int w, int h)
    {
        prepareScreenSize();
        
        repaint();

        return;
    }

    /**
     *   �L�[����
     */
    public void keyPressed(int keyCode)
    {
        boolean repaint = false;
        if ((keyCode == 0)||(painting == true))
        {
            // �L�[���͂��Ȃ��A�܂��͕`�撆�̏ꍇ�ɂ͐܂�Ԃ�
            return;
        }

        if (screenKind == BUSY_FORM)
        {
            // �I��s�̏ꍇ�́A������Ă��������Ȃ��B
            return;
        }
        parent.keepWatchDog(0);

        int kc = getGameAction(keyCode);
        if (releaseConfirmation(kc) == true)
        {
            // �m�F�t�H�[���ŉ����L�[�������ꂽ�ꍇ�ɂ͏I������B
            return;
        }

        // ���l���ړ��͉\���[�h�������ꍇ...
        if (keyPressedNumber(keyCode) == true)
        {
            return;
        }

        // *�L�[�A ','�L�[���������ꍇ�ɂ́A�擪�ֈړ�����
        if ((keyCode == Canvas.KEY_STAR)||(keyCode == 44))
        {
            if (currentScene == parent.CONFIRM_URL_FOR_BROWSER)
            {
                // �摜�\�����s���B�B�B
                inputFire(4);
                return;
            }
            currentPage  = 0;
            selectedLine = 0;
            repaint();
            return;
        }

        // #�L�[�A'.'�L�[���������ꍇ�ɂ́A�����ֈړ�����
        if ((keyCode == Canvas.KEY_POUND)||(keyCode == 46))
        {
            if (currentScene == parent.CONFIRM_URL_FOR_BROWSER)
            {
                // �t�@�C���擾���s���B�B�B
                inputFire(3);
                return;
            }
            currentPage  = nofPage - 1;
            selectedLine = selectionData.size() - 1;
            repaint();
            return;
        }
        if (keyCode == -8)
        {
            // �N���A�L�[ for Nokia ... (�������킹...)
            inputClear();
            repaint();
            return;
        }

        if ((keyCode == 112)||(keyCode == 80))
        {
            // �O�y�[�W��\������ (p, P�L�[)
            inputLeft();
            repaint();
            return;
        }

        if ((keyCode == 110)||(keyCode == 78))
        {
            // ���y�[�W��\������ (n, N�L�[)
            inputRight();
            repaint();
            return;
        }

        if ((keyCode == 27)||(keyCode == 113)||(keyCode == 81))
        {
            // �N���A����...(ESC, Q�L�[)
            repaint = inputClear();
            repaint();
            return;
        }

        if ((keyCode == 8)&&(getGameAction(keyCode) != Canvas.FIRE))
        {
            // BS�L�[ (�N���A����)
            inputClear();
            repaint();
        }
        
        if ((keyCode == 119)||(keyCode == 87))
        {
            // W�L�[
            inputGameD();
            repaint();
            return;
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
            repaint = inputDown(1);
            break;

          case Canvas.UP:
            repaint = inputUp(1);
            break;

          case Canvas.FIRE:
            repaint = inputFire(0);
            break;

          case Canvas.GAME_A:
            // GAME-A�L�[
            repaint = inputUp(3);
            break;

          case Canvas.GAME_B:
            // GAME-B�L�[
            repaint = inputDown(3);
            break;

          case Canvas.GAME_C:
            // GAME-C�L�[
            repaint = inputClear();
            break;

          case Canvas.GAME_D:
            // GAME-D�L�[
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
     *   �L�[���s�[�g���Ă���Ƃ�...
     * 
     * 
     */
    public void keyRepeated(int keyCode)
    {
        parent.keepWatchDog(0);

        boolean repaint = false;
        if ((keyCode == 0)||(painting == true))
        {
            // �L�[���͂��Ȃ��A�܂��͕`�撆�̏ꍇ�ɂ͐܂�Ԃ�
            return;
        }

        if (screenKind != SELECTION_FORM)
        {
            // �I��s�̏ꍇ�́A������Ă��������Ȃ��B
            return;
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
            repaint = inputDown(1);
            break;

          case Canvas.UP:
            repaint = inputUp(1);
            break;

          case Canvas.FIRE:
            repaint = false;
            break;

          case Canvas.GAME_A:
            // GAME-A�L�[
            repaint = inputUp(3);
            break;

          case Canvas.GAME_B:
            // GAME-B�L�[
            repaint = inputDown(3);
            break;

          case Canvas.GAME_C:
            // GAME-C�L�[
            repaint = false;
            break;

          case Canvas.GAME_D:
            // GAME-D�L�[
            repaint = false;
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

    private boolean inputLeft()
    {
        if (currentPage == 0)
        {
            currentPage  = nofPage - 1;
            selectedLine = selectionData.size() - 1;
            return (true);
        }
        currentPage--;
        selectedLine = selectedLine - (screenLines - 1);
        if (selectedLine < 0)
        {
            selectedLine = 0;
        }        
        return (true);
    }

    private boolean inputRight()
    {
        currentPage++;
        if (currentPage == nofPage)
        {
            currentPage  = 0;
            selectedLine = 0;
            return (true);
        }
        selectedLine = selectedLine + screenLines - 1;
        if (selectedLine >= selectionData.size())
        {
            selectedLine = selectionData.size() - 1;
        }
        return (true);
    }

    private boolean inputUp(int count)
    {
        if (parent.getCursorSingleMovingMode() == true)
        {
            selectedLine = selectedLine - count;
            if (selectedLine < currentPage * (screenLines - 1))
            {
                selectedLine = selectedLine + (screenLines - 1);
                inputLeft();
            }
            return (true);
        }

        selectedLine = selectedLine - count;
        if (selectedLine < currentPage * (screenLines - 1))
        {
            // �y�[�W�̖����ɃJ�[�\�����ڂ�
            selectedLine = (screenLines - 1) * (currentPage + 1);
            if (selectedLine >= selectionData.size())
            {
                selectedLine = selectionData.size() - 1;
            }
        }
        else
        {
            if (count == 1)
            {
                redraw = false;
            }
        }
        return (true);
    }

    private boolean inputDown(int count)
    {
        selectedLine = selectedLine + count;
        if (parent.getCursorSingleMovingMode() == true)
        {
            if (selectedLine >= selectionData.size())
            {
                selectedLine = 0;
                currentPage = 0;
            }
            else if (selectedLine > (currentPage + 1) * (screenLines - 1))
            {
                selectedLine = selectedLine - (screenLines - 1);
                   inputRight();
            }
            return (true);
        }

        if (selectedLine == selectionData.size())
        {
            // �y�[�W�̐擪�ɃJ�[�\�����ڂ�
            selectedLine = (screenLines - 1) * currentPage;
            return (true);
        }

        if (selectedLine > (currentPage + 1) * (screenLines - 1))
        {
            // �y�[�W�̐擪�ɃJ�[�\�����ڂ�
            selectedLine = (screenLines - 1) * currentPage;
        }
        else
        {
            if (count == 1)
            {
                redraw = false;
            }
        }
        return (true);
    }

    private boolean inputFire(int addInfo)
    {
        if (screenKind == BUSY_FORM)
        {
            // �I��s�̏ꍇ�́A�L�����Z����������Ă��������Ȃ��B
            return (false);
        }
        if (releaseConfirmation(Canvas.FIRE) == true)
        {
            // Confirm�̏ꍇ�ɂ͏I������...
            return (false);
        }

        titleString   = null;
        selectionData = null;
        parent.endSelectForm(currentScene, selectedLine, addInfo);
        System.gc();
        return (false);
    }

    private boolean inputClear()
    {
        if (screenKind == BUSY_FORM)
        {
            // �I��s�̏ꍇ�́A�L�����Z����������Ă��������Ȃ��B
            return (false);
        }
        if (releaseConfirmation(Canvas.GAME_C) == true)
        {
            // Confirm�̏ꍇ�ɂ͏I������...
            return (false);
        }

        titleString   = null;
        selectionData = null;
        parent.endSelectForm(currentScene, -1, -1);
        System.gc();
        return (false);
    }

    private boolean inputGameD()
    {
        if (screenKind == BUSY_FORM)
        {
            // �I��s�̏ꍇ�́A�L�����Z����������Ă��������Ȃ��B
            return (false);
        }
        if (releaseConfirmation(Canvas.GAME_D) == true)
        {
            // Confirm�̏ꍇ�ɂ͏I������...
            return (false);
        }

        titleString   = null;
        selectionData = null;
        parent.endSelectForm(currentScene, -1, -1);
        System.gc();
        return (false);
    }
    
    private boolean releaseConfirmation(int actionCode)
    {
        // �R�}���h����������...
        if (screenKind == CONFIRMATION_FORM)
        {
            this.removeCommand(confirmCmd);
            this.setCommandListener(null);

            titleString   = null;
            selectionData = null;
            parent.returnToPreviousForm(parent.ACTION_NOTHING);
            System.gc();
            return (true);
        }
        return (false);
    }

    /**
     *  �^�C���A�E�g�����o����
     * 
     */
    public void detectTimeout(long isDetected)
	{
        // �E�H�b�`�h�b�O�^�C�}���Đݒ肷��
        try
        {
            if ((watchDogTimer != null)&&(isBlinking == true))
            {
                watchDogTimer.startWatchDog(blinkDuration);
            }
        }
        catch (Exception ex)
        {
            // �������Ȃ�...
        }
        /**/

        // �^�C�g���F�𔽓]������
    	int swapColor = titleBackColor;
    	timerTemp++;
    	switch ((timerTemp % 4))
    	{
            case 0:
              swapColor = COLOR_A;
              break;

            case 1:
              swapColor = COLOR_B;
              break;

            case 2:
              swapColor = COLOR_C;
              break;

            case 3:
            default:
              swapColor = COLOR_D;
              break;
    	}
        titleBackColor = titleForeColor;
        titleForeColor = swapColor;
        repaint();
        serviceRepaints();
        System.gc();
        return;
	}
}
