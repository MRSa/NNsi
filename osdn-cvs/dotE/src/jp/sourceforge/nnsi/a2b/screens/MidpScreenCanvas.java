/**
 *  a2B Framework�̉�ʕ\���p�p�b�P�[�W�ł��B
 * 
 */
package jp.sourceforge.nnsi.a2b.screens;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;

/**
 * ��ʃL�����o�X(�x�[�X�N���X)<br>
 * <em>���ڂ��̃N���X���g�����Ƃ͂Ȃ��Ƃ͎v���܂����A���̃N���X���p�����邱�ƂŁA
 * �����ȕK�v�ȃN���X���ȗ����邱�Ƃ��ł��܂��B</em>
 *
 * @author MRSa
 *
 */
public class MidpScreenCanvas implements IMidpScreenCanvas
{
    protected MidpSceneSelector parent  = null;

    protected final int baseBackColor      = 0x00ffffff;
    protected final int baseForeColor      = 0x00000000;
    protected final int baseTitleForeColor = 0x00f0f0f0;

    protected int   baseTitleBackColor = 0x000000ff;
    protected int   titleForeColor     = baseTitleForeColor;
    protected int   subTitleForeColor  = baseTitleForeColor;

    protected int   canvasId           = -1;

    protected Font   screenFont        = null;  // �\���Ɏg�p����t�H���g
    protected String titleString       = null;  // �\���^�C�g��
    protected String titleSubString    = null; // �\���^�C�g��(�T�u)

    protected int   screenTopX   = 0;     // �`��̈�̍��[(X)
    protected int   screenTopY   = 0;     // �`��̈�̍��[(Y)
    protected int   screenWidth  = 160;   // ��ʕ�
    protected int   screenHeight = 160;   // ��ʍ���
    protected int   screenLine   = 0;     // �f�[�^�\���\���C����

    /**
     * �R���X�g���N�^�ł́A�V�[���Z���N�^�����炢�܂��B
     * 
     * @param screenId ���ID
     * @param object �V�[���Z���N�^
     */
    public MidpScreenCanvas(int screenId, MidpSceneSelector object)
    {
        canvasId  = screenId;
        parent    = object;
    }

    /**
     * ��ʂ̕\������
     * @param font �\������t�H���g
     */
    public void prepare(Font font)
    {
        screenFont   = font;
    }

    /**
     *  ��ʃT�C�Y��ݒ肷��
     *  
     *  @param topX  ����X���W
     *  @param topY  ����Y���W
     *  @param width ��ʕ�(�s�N�Z����)
     *  @param height ��ʍ���(�s�N�Z����)
     */
    public void setRegion(int topX, int topY, int width, int height)
    {
        screenTopX   = topX;
        screenTopY   = topY;
        screenWidth  = width;
        screenHeight = height;
        screenLine   = decideScreenLine(height);
    }

    /**
     *  �����̉��ID����������
     *  @return ���ID
     */    
    public int  getCanvasId()
    {
        return (canvasId);
    }

    /**
     * ����[X���W����������
     * 
     * @return ��ʍ���X���W
     */
    public int getTopX()
    {
        return (screenTopX);
    }

    /**
     * ����[Y���W����������
     * @return ��ʍ���Y���W
     */
    public int getTopY()
    {
        return (screenTopY);
    }

    /**
     * �`��E�B���h�E������������
     * @return ��ʕ`��E�B���h�E��
     */
    public int getWidth()
    {
        return (screenWidth);
    }

    /**
     * �`��E�B���h�E��������������
     * @return ��ʕ`��E�B���h�E����
     */
    public int getHeight()
    {
        return (screenHeight);
    }
    
    /**
     * �\���\���C���������߂�
     * @param height
     * @return �\���\���C����
     */
    private int decideScreenLine(int height)
    {
        return (((height - (screenFont.getHeight() * 1)) / screenFont.getHeight()));
    }

    /**
     * ��������i���s�R�[�h����͂��āj�o�͂���
     * @param g ��ʃX�N���[��
     * @param startLine �\�����郉�C���̐擪�s
     * @param offsetX X�̃I�t�Z�b�g
     * @param nextOffsetX ���s��̃I�t�Z�b�g
     * @param msg �\�����镶����
     * @return �g�p�������C����
     */
    protected int drawString(Graphics g, int startLine, int offsetX, int nextOffsetX, String msg)
    {
        // �\���\�s��ݒ肷��
        g.setColor(0);
        int line    = 0;
        int y       = screenTopY + (startLine * (screenFont.getHeight() + 1));
        int x       = screenTopX + offsetX;
        int limitX  = screenWidth - screenFont.stringWidth("   ");
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
                x = screenTopX + offsetX + nextOffsetX;
                y = y + screenFont.getHeight() + 1;
                line++;
            }
            if (y >= screenTopY + screenHeight)
            {
                // �`��̈悪���ӂꂽ...�I������
                break;
            }
        }
        return (line + 1);
    }

    /**
     *  ��ʕ\���̊J�n
     * 
     */
    public void start()
    {
        // TODO : �������ׂ��I
    }

    /**
     * ��ʕ\���̏I��
     * 
     */
    public void stop()
    {
        // TODO : �������ׂ��I
        return;
    }

    /**
     * �\���f�[�^�̍X�V����
     * 
     * @param processingId ����ID
     * 
     */
    public void updateData(int processingId)
    {
        // TODO : �������ׂ��I
        return;
    }
    
    /**
     * ��ʂ�\��
     * @param g �O���t�B�b�N
     * 
     */
    public void paint(Graphics g)
    {
        try
        {
            // �^�C�g���̈�̕`��
            drawTitleArea(g);

            // ���C���p�l���̕`��...
            clearMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (screenHeight - screenFont.getHeight()));
        }
        catch (Exception e)
        {
            // �Ȃ����k���|���o��̂�...
        }
        return;
    }

    /**
     * �^�C�g���̈�ɕ`�悷��
     * @param g
     */
    protected void drawTitleArea(Graphics g)
    {
        // �^�C�g�������̔w�i��\������
        g.setColor(baseTitleBackColor);
        g.fillRect(screenTopX, screenTopY, screenWidth, screenFont.getHeight());
        if (titleString != null)
        {
            g.setColor(titleForeColor);
            g.drawString(titleString, screenTopX + 1, screenTopY, (Graphics.LEFT | Graphics.TOP));
        }
        if (titleSubString != null)
        {
            g.setColor(subTitleForeColor);
            int width = (screenTopX + 1) + screenFont.stringWidth(titleString + " ");
            g.drawString(titleSubString, width, screenTopY, (Graphics.LEFT | Graphics.TOP));        	
        }
        return;
    }

    /**
     * �^�C�g����ݒ肷��
     * @param title �^�C�g��
     */
    public void setTitle(String title)
    {
        titleString = title;
        return;
    }    

    /**
     * ���̈�ɏ���ݒ肷��
     * @param message �\�����郁�b�Z�[�W
     */
    public void setInformation(String message)
    {
        // TODO : �������ׂ��I
        return;
    }    
    
    /**
     * ���̈�̏����X�V����
     * @param message �\�����郁�b�Z�[�W
     * @param mode �\�����[�h
     * 
     */
    public void updateInformation(String message, int mode)
    {
        // TODO : �������ׂ��I
        return;        
    }

    /**
     *  ���������̒ʒm
     *  @param processingId ����ID
     *  @param result ��������
     */
    public void finishUpdateData(int processingId, int result)
    {
        // TODO : �������ׂ��I
        return;
    }
    
    /**
     * ���C���p�l���̈���N���A����
     * 
     * @param g �`��̈�
     * @param topX ����X���W
     * @param topY ����Y���W
     * @param width ��
     * @param height ����
     */
    protected void clearMainPanel(Graphics g, int topX, int topY, int width, int height)
    {
        // �g��\������
        g.setColor(baseBackColor);
        g.fillRect(topX, topY, width, height);
        return;
    }

    /**
     * �q�[�v���(�󂫃������ƑS�̃������e��)�̕��������������
     * @return �q�[�v���(������)
     */
    protected String getDebugMemoryInfo()
    {
        Runtime run = Runtime.getRuntime();
        return ("Heap : " + run.freeMemory() + " / " + run.totalMemory());
    }

    /**
     *  ��ʂ��I��������
     * 
     */
    protected void finishCanvas()
    {
        // �O��ʂ֖߂�
        parent.previousScene();
        
        return;
    }
    
    /**
     *  �L�[���͂��ꂽ�Ƃ��̏���
     *  @param keyCode ���͂����L�[�̃R�[�h
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  �����L�[�����͂��ꂽ�Ƃ��̏���
     *  @param number ���͂��ꂽ����
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     *
     */
    public boolean inputNumber(int number, boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  ���j���[�\�����w�����ꂽ�Ƃ��̏���
     *
     */
    public void showMenu()
    {
        // TODO : �������ׂ��I
        return;
    }

    /**
     *  �A�v���P�[�V�������I�����邩�ǂ������肷��<br>
     *  (�\�t�g�L�["X"�������ꂽ�Ƃ��̏���)
     * 
     *  @return  true : �A�v���P�[�V�������I��������Afalse : �A�v���P�[�V�������I�������Ȃ�
     */
    public boolean checkExit()
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  ��ʃ^�b�`���ꂽ�Ƃ��̏���
     *  @param x x���W
     *  @param y y���W
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean pointerPressed(int x, int y)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  ��L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputUp   (boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  ���L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputDown (boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  ���L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputLeft (boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  �E�L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputRight(boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  �Z���^�[�L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputFire (boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (true);
    }

    /**
     *  Game A�L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputGameA(boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  Game B�L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputGameB(boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  Game C�L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputGameC(boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  Game D�L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputGameD(boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  ���L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputPound(boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }

    /**
     *  ���L�[���쏈��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        // TODO : �������ׂ��I
        return (false);
    }
}
