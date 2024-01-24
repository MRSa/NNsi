package jp.sourceforge.nnsi.a2b.frameworks;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * �\���i���C���j�����N���X<br>
 * ��ʕ`����s���e�ʂł��B
 * 
 * @author MRSa
 *
 */
public class MidpCanvas  extends Canvas implements CommandListener, IDataProcessing, ILogMessageDisplay
{
    private final int CMD_EXIT       = 0;           // Exit�R�}���h
    private final int CMD_MENU       = 1;           // Menu�R�}���h

    private final int COLOR_BLACK    = 0;           // ���F
    private final int COLOR_WHITE    = 0x00ffffff;  // ���F
    private final int BACKCOLOR_BUSY = 0x00ffffc0;  // BUSY�G���A�̔w�i�F
    private final int FORECOLOR_BUSY = COLOR_BLACK; // BUSY�G���A�̕����F
    
    private Font      useFont     = null;    // �\���t�H���g
    private String    busyMessage = null;    // �r�W�[���b�Z�[�W
    private boolean  isBusyMode  = false;  // �r�W�[���(����֎~��)
    private boolean  painting    = false;  // �`����s��
    private boolean  working     = false;  // �f�[�^�X�V��
    private boolean  isFirstTime = true;   // ����`�掞���H
    private int       myWidth     = 240;
    private int       myHeight    = 320;

    private Command   menuCmd = new Command("Menu", Command.ITEM, CMD_MENU);
    private Command   exitCmd = new Command(" �~ ", Command.EXIT, CMD_EXIT);

    private MidpSceneSelector parent       = null;
    private IMidpScreenCanvas activeCanvas = null;

    /**
     * ��ʕ\���̏������s���܂�
     * @param object �V�[���Z���N�^
     * @param fontSize �\������t�H���g�̃T�C�Y
     */
    public MidpCanvas(MidpSceneSelector object, int fontSize)
    {
        // �e���L������
        parent = object;

        // �^�C�g���̐ݒ�
        setTitle(null);

        // �t�H���g�̐ݒ�
        useFont = Font.getDefaultFont();
        useFont = Font.getFont(useFont.getFace(), useFont.getStyle(),fontSize);

        // �R�}���h�̒ǉ�
        this.addCommand(menuCmd);
        this.addCommand(exitCmd);
        this.setCommandListener(this);    
    }

    /**
     *  �`��N���X�̏���...
     *
     */
    public void prepare(int width, int height)
    {
        myWidth = width;
        myHeight = height;
        return;
    }

    /**
     *   ��ʂ̏���...
     */
    public void prepareScreen()
    {
        //
        return;
    }

    /**
     * ��ʂ̕`�敝����������
     * @return ��ʂ̕`�敝
     * 
     */
    public int getWidth()
    {
        return (myWidth);
    }

    /**
     * ��ʂ̕`�捂������������
     * @return ��ʂ̕`�捂��
     */
    public int getHeight()
    {
        return (myHeight);
    }

    /**
     * �g�p����t�H���g����������
     * @return �g�p����t�H���g
     */
    public Font getFont()
    {
        return (useFont);
    }

    /**
     * ��ʍĕ`��w��
     * @param forceUpdate ��ʍĕ`�悪�ςނ܂Œ�~����
     */
    public void redraw(boolean forceUpdate)
    {
        repaint();
        if (forceUpdate == true)
        {
            serviceRepaints();
        }
        return;
    }

    /**
     * ��ʂ̕`������{����
     * @param g �O���t�B�b�N�L�����o�X
     */
    public void paint(Graphics g)
    {
        // ��ʕ`�撆�̂Ƃ��ɂ͐܂�Ԃ�
        if ((painting == true)||(g == null))
        {
            return;
        }
        painting = true;  // �`��J�n

        if (isFirstTime == true)
        {
        	// �c�E���T�C�Y��ݒ肵�Ȃ����B
        	isFirstTime = false;
        }
        
        // BUSY���b�Z�[�W���c���Ă���H
        if ((isBusyMode == false)&&(busyMessage != null))
        {
            hideBusyMessage(g);
        }
        
        /////////////////////////////////////
        try
        {
            if (activeCanvas != null)
            {
                // �t�H���g��ݒ�
                g.setFont(useFont);

                // �E�B���h�E����ʑS�̂ł͂Ȃ��Ƃ��́A�E�B���h�E�g��\������
                if (activeCanvas.getTopY() > 0)
                {
                    g.setColor(COLOR_BLACK);
                    g.drawRect(activeCanvas.getTopX() - 1, activeCanvas.getTopY() - 1, activeCanvas.getWidth() + 1, activeCanvas.getHeight() + 1);
                }
                // ��ʕ`������{...
                activeCanvas.paint(g);
            }
        }
        catch (Exception ex)
        {
            // �G���[������ʕ\�� (�D�F��...)
            g.setColor(0x007f7f7f);
            g.drawString(ex.getMessage(), 0, (useFont.getHeight() * 2), (Graphics.LEFT | Graphics.TOP));
        }
        //////////////////////////////////////

        // BUSY���b�Z�[�W��\������H
        if ((isBusyMode == true)&&(busyMessage != null))
        {
            showBusyMessage(g);
        }
        painting = false;  // �`��I��
        return;
    }

    /**
     * �r�W�[���b�Z�[�W�̕\��(��ʍŉ��s�ɕ������\������)
     * 
     */
    private void showBusyMessage(Graphics g)
    {
        int height = useFont.getHeight() + 2;

        // ��ʂ̓h��Ԃ�
        g.setColor(BACKCOLOR_BUSY);
        g.fillRect(0, (getHeight() - height - 1), getWidth(), (getHeight() - 1));

        // ���b�Z�[�W�̕\��
        g.setColor(FORECOLOR_BUSY);
        g.setFont(useFont);
        g.drawString(busyMessage, 5, (getHeight() - height), (Graphics.LEFT | Graphics.TOP));
        return;        
    }

    /**
     *  �r�W�[���b�Z�[�W�̕\���폜
     * 
     */
    private void hideBusyMessage(Graphics g)
    {
        int height = useFont.getHeight() + 2;
        
        // ��ʂ̓h��Ԃ�
        g.setColor(COLOR_WHITE);
        g.fillRect(0, (getHeight() - height - 1), getWidth(), (getHeight() - 1));
        busyMessage = null;  // �\�����b�Z�[�W�̍폜
        return;        
    }

    /**
     *  �r�W�[���b�Z�[�W�̐ݒ�
     *
     *  @param message     �r�W�[���b�Z�[�W
     *  @param forceUpdate ��ʍĕ`��
     *  @param lockOperation ��ʑ���̋֎~
     */    
    public void outputInformationMessage(String message, boolean forceUpdate, boolean lockOperation)
    {
        working = lockOperation;
        isBusyMode  = true;
        busyMessage = message;
        if (forceUpdate == true)
        {
             repaint();
             serviceRepaints();
        }
        return;
    }

    /**
     *  �r�W�[���b�Z�[�W�̕\���I��
     *
     */    
    public void finishInformationMessage()
    {
        working = false;
        isBusyMode  = false;
        return;
    }

    /**
     *  �r�W�[���b�Z�[�W�̃N���A
     *
     */    
    public void clearInformationMessage()
    {
        working = false;
        isBusyMode  = false;
        repaint();
        return;
    }

    /**
     * ��ʕ\����؂�ւ���
     * @param newCanvas �V�������
     */
    public void changeActiveCanvas(IMidpScreenCanvas newCanvas)
    {
        try
        {
            if (activeCanvas != null)
            {
                activeCanvas.stop();
            }
            activeCanvas = newCanvas;
            if (activeCanvas != null)
            {
                activeCanvas.start();
           }
        }
        catch (Exception ex)
        {
            //
            // setBusyMessage() : �G���[�̏��
            //
        }
        repaint();
        return;
    }

    /**
     *  �f�[�^�̍X�V����
     *  @param processingId ����ID
     *
     */
    public void updateData(int processingId)
    {
        if (working == true)
        {
            return;
        }
        if (activeCanvas != null)
        {
            working = true;
            activeCanvas.updateData(processingId);
            working = false;
        }
        repaint();
        return;
    }

    /**
     *  ���̈�̏����X�V����
     * 
     *  @param message     �\�����郁�b�Z�[�W
     *  @param mode        �\�����[�h(�L�����o�X�p)
     *  @param forceUpdate ��ʍĕ`������{�������{
     */
    public void updateInformation(String message, int mode, boolean forceUpdate)
    {
        if (activeCanvas != null)
        {
            activeCanvas.updateInformation(message, mode);
            repaint();
            if (forceUpdate == true)
            {
                serviceRepaints();
            }
        }
        return;
    }

    /**
     * �����̏I����ʒm����
     * 
     * @param processingId ����ID
     * @param result ��������
     * @param vibrateTime �o�C�u���[�V�������鎞��(�P��:ms)
     */
    public void finishUpdateData(int processingId, int result, int vibrateTime)
    {
        if (activeCanvas != null)
        {
            activeCanvas.finishUpdateData(processingId, result);
        }
        if (vibrateTime > 0)
        {
            parent.vibrate(vibrateTime);
        }
        repaint();
        return;
    }

    /**
     * �R�}���h���s���̏���
     *
     * @param c �R�}���h
     * @param d �\��
     */
    public void commandAction(Command c, Displayable d) 
    {
        int command = c.getPriority();
        commandActionMain(command);
        return;
    }

    /**
     * �R�}���h���s���̏����B�B�B
     *
     * @param command �R�}���h
     */
    private void commandActionMain(int command)
    {
        if (command == CMD_MENU)
        {
            // ���j���[�̕\��
            if (activeCanvas != null)
            {
                activeCanvas.showMenu();
                repaint();
            }
            return;
        }
        else if (command == CMD_EXIT)
        {
            if (activeCanvas == null)
            {
                // �A�v���P�[�V�������I��������...
                parent.stop();
                return;
            }
            // �I�����邩�ǂ����`�F�b�N����
            if (activeCanvas.checkExit() == true)
            {
                parent.stop();
            }
            return;
        }
        return;
    }

    /**
     *   �L�[���͂��������Ƃ��̏���
     *   @param keyCode �L�[���̓R�[�h
     */
    public void keyPressed(int keyCode)
    {
        // �^�C�}�[�̍X�V
        parent.extendTimer(MidpTimer.TIMER_EXTEND);

        boolean repaint = false;
        if ((keyCode == 0)||(painting == true)||(working == true))
        {
            // �L�[���͂��Ȃ��A�܂��͕`�撆�̏ꍇ�ɂ͐܂�Ԃ�
            return;
        }

        // �L�[���͂̊m�F
        if (activeCanvas != null)
        {
            repaint = checkKeyInput(keyCode, false);
        }

        // ��ʍĕ`�悷�邩�H
        if (repaint == true)
        {
            repaint();
        }
        return;
    }

    /**
     *   �L�[���s�[�g���Ă���Ƃ�
     *   @param keyCode �L�[�R�[�h
     */
    public void keyRepeated(int keyCode)
    {
        // �^�C�}�[�̍X�V
        parent.extendTimer(MidpTimer.TIMER_EXTEND);

        boolean repaint = false;
        if ((keyCode == 0)||(painting == true)||(working == true))
        {
            // �L�[���͂��Ȃ��A�܂��͕`�撆�̏ꍇ�ɂ͐܂�Ԃ�
            return;
        }

        // �L�[���͂̊m�F
        if (activeCanvas != null)
        {
            repaint = checkKeyInput(keyCode, true);
        }

        // ��ʂ��ĕ`�悷��H
        if (repaint == true)
        {
            repaint();
        }
        return;
    }

    /**
     *   ��ʃT�C�Y�ύX���ɌĂ΂�鏈��(�������Ȃ�...)
     *   
     *   @param w ��ʕ�
     *   @param h ��ʍ���
     */
    public void sizeChanged(int w, int h)
    {
        parent.extendTimer(MidpTimer.TIMER_EXTEND);
        repaint();
        return;
    }
    
    /**
     *  ���������W������
     *  
     *  @param x �^�b�v����X���W
     *  @param y �^�b�v����Y���W
     */
    public void pointerPressed(int x, int y)
    {
        parent.extendTimer(MidpTimer.TIMER_EXTEND);
        boolean repaint = false;
        if (activeCanvas != null)
        {
            int topX   = activeCanvas.getTopX();
            int topY   = activeCanvas.getTopY();
            int width  = activeCanvas.getWidth();
            int height = activeCanvas.getHeight();

            if (((x >= topX)&&(x <= topX + width))&&
                ((y >= topY)&&(y <= topY + height)))
            {
                repaint = activeCanvas.pointerPressed((x - topX), (y - topY));
            }
        }

        // ��ʂ��ĕ`�悷��H
        if (repaint == true)
        {
            repaint();
        }
        return;
    }

    /**
     *   �L�[�̓��͂���͂��񍐂���
     */
    private boolean checkKeyInput(int keyCode, boolean isRepeat)
    {
        boolean isProcessed = false;
        
        // �����L�[�̌��o...
        isProcessed = checkNumberInput(keyCode, isRepeat);
        if (isProcessed == true)
        {
            return (true);
        }

        // �J�[�\���L�[�̌��o...
        isProcessed = checkCursorKeyInput(keyCode, isRepeat);
        if (isProcessed == true)
        {
            return (true);
        }

        // �L�[���͂��F���ł��Ȃ�����...��ʂɕ����B
        return (activeCanvas.inputOther(keyCode, isRepeat));
    }

    /**
     * �����{�^�������͂��ꂽ���ǂ����`�F�b�N����
     * @param keyCode �L�[�R�[�h
     * @param isRepeat �L�[���s�[�g����(true) / �P�V���b�g����(false)
     * @return �\������(true) / �\�����Ȃ�(false)
     */
    private boolean checkNumberInput(int keyCode, boolean isRepeat)
    {
        boolean ret = true;
        int number = -1;
        
        // �����L�[�̓��͌��o
        switch (keyCode)
        {
          case Canvas.KEY_NUM0:
            number = 0;
            break;

          case Canvas.KEY_NUM1:
            number = 1;
            break;

          case Canvas.KEY_NUM2:
            number = 2;
            break;

          case Canvas.KEY_NUM3:
            number = 3;
            break;

          case Canvas.KEY_NUM4:
            number = 4;
            break;

          case Canvas.KEY_NUM5:
            number = 5;
            break;

          case Canvas.KEY_NUM6:
            number = 6;
            break;

          case Canvas.KEY_NUM7:
            number = 7;
            break;

          case Canvas.KEY_NUM8:
            number = 8;
            break;

          case Canvas.KEY_NUM9:
            number = 9;
            break;

          case 46:
          case Canvas.KEY_POUND:
            // # �L�[�A, �L�[�̓���
            ret = activeCanvas.inputPound(isRepeat);
            break;

          case 44:
          case Canvas.KEY_STAR:
            // * �L�[�A. �L�[�̓���
            ret = activeCanvas.inputStar(isRepeat);
            break;

          case 90:
          case 122:
            // Z�L�[
            ret = activeCanvas.inputGameA(isRepeat);
            break;

          case 88:
          case 120:
            // X�L�[
            ret = activeCanvas.inputGameB(isRepeat);
            break;

          case 27:
          case 113:
          case 81:
          case -8:
            // ESC�L�[�AQ�L�[�ANokia�̃N���A�L�[ (�������킹)
            ret = activeCanvas.inputGameC(isRepeat);
            break;

          case 119:
          case  87:
          case -50:
            // W�L�[�ANokia��Func�L�[... (�������킹)
            ret = activeCanvas.inputGameD(isRepeat);
            break;

          case 67:
          case 99:
          case -5:
            // S�L�[�AEnter�L�[ (�������킹)
            ret = activeCanvas.inputFire(isRepeat);
            break;

          default:
            ret = false;
            break;
        }

        // �����L�[���͂�ʒm����
        if (number >= 0)
        {
            ret = activeCanvas.inputNumber(number, isRepeat);
        }
        return (ret);
    }

    /**
     * �J�[�\���L�[�����͂��ꂽ���ǂ����`�F�b�N����
     * @param isRepeat
     * @return �\������(true) / �\�����Ȃ�(false)
     */
    private boolean checkCursorKeyInput(int keyCode, boolean isRepeat)
    {
        boolean ret = false;
        switch (getGameAction(keyCode))
        {
          case Canvas.LEFT:
            ret = activeCanvas.inputLeft(isRepeat);
            break;
              
          case Canvas.RIGHT:
            ret = activeCanvas.inputRight(isRepeat);
            break;

          case Canvas.DOWN:
            ret = activeCanvas.inputDown(isRepeat);
            break;

          case Canvas.UP:
            ret = activeCanvas.inputUp(isRepeat);
            break;

          case Canvas.FIRE:
            ret = activeCanvas.inputFire(isRepeat);
            break;

          case Canvas.GAME_A:
            ret = activeCanvas.inputGameA(isRepeat);
            break;

          case Canvas.GAME_B:
            ret = activeCanvas.inputGameB(isRepeat);
            break;

          case Canvas.GAME_C:
            ret = activeCanvas.inputGameC(isRepeat);
            break;

          case Canvas.GAME_D:
            ret = activeCanvas.inputGameD(isRepeat);
            break;

          default:
            // �L�[���͂����o�ł��Ȃ�����...
            ret = false;
            break;
        }
        return (ret);
    }
}
