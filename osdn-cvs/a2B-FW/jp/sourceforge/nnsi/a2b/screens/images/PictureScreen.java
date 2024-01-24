/**
 *  �摜�\���p�̉�ʃp�b�P�[�W�ł��B
 */
package jp.sourceforge.nnsi.a2b.screens.images;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 *  �摜�\���p�̉�ʂł��B
 *
 * @author MRSa
 *
 */
public class PictureScreen extends MidpScreenCanvas
{
    private PictureScreenOperator  operation = null;

    private final int backColor = 0x00ffffff;
    private final int foreColor = 0x00000000;

    private Image imageObject  = null;

    private boolean busyMode     = false;
    private boolean useThumbnail = true;

    private int   offsetX      = 0;     // �\���J�n�ʒu�i�w���W�j
    private int   offsetY      = 0;     // �\���J�n�ʒu�i�x���W�j

    /**
     * �R���X�g���N�^�ł́A�K�v�ȃI�u�W�F�N�g���L�����܂��B
     * @param screenId ���ID
     * @param object �V�[���Z���N�^
     * @param operator �`��p�f�[�^�N���X
     */
    public PictureScreen(int screenId, MidpSceneSelector object, PictureScreenOperator operator)
    {
        super(screenId, object);
        operation = operator;
    }

    /**
     *  ��ʂ̕\�����J�n���܂��B
     * 
     */
    public void start()
    {
        showPicture(operation.getMessage());
    }

    /**
     *  �摜�̃f�[�^���������ɓW�J���A�\�����܂��B
     *  @param message �^�C�g���ɕ\�����郁�b�Z�[�W
     * 
     */
    private void showPicture(String message)
    {
        int pos = titleString.indexOf(":");
        if (pos >= 0)
        {
            message = titleString.substring(pos + 1);
        }
        titleString = " �摜�W�J��... " + message;
        imageObject = null;
        System.gc();
        try
        {
            Image img = operation.getImage(useThumbnail);
            if (img == null)
            {
                // �摜�ǂݍ��ݎ��s...
                titleString = "<<< OUT OF MEMORY? >>>";
                System.gc();
                return;
            }

            int width  = screenWidth;
            int height = screenHeight - (screenFont.getHeight() * 1);
            if (img.getHeight() < (height + offsetY))
            {
                offsetY = img.getHeight() - height - 1;
                if (offsetY < 0)
                {
                    offsetY = 0;
                }
                if ((offsetY + height) > img.getHeight())
                {
                    height = img.getHeight() - offsetY;
                }
            }
            if (img.getWidth() < (width + offsetX))
            {
                offsetX = img.getWidth() - width - 1;
                if (offsetX < 0)
                {
                    offsetX = 0;
                }
                if ((offsetX + width) > img.getWidth())
                {
                    width = img.getWidth() - offsetX;
                }
            }
            imageObject = Image.createImage(img, offsetX, offsetY, width, height, Sprite.TRANS_NONE);
            titleString = img.getWidth() + "x" + img.getHeight() + "(" + offsetX + "," + offsetY + ")";
            if (useThumbnail == true)
            {
                titleString = titleString + "T";
            }
            else
            {
                titleString = titleString + "N";
            }
            img = null;
            titleString = titleString + ":" + message;
        }
        catch (OutOfMemoryError e)
        {
            imageObject = null;
            titleString = " ERROR:: Out of Memory...";
        }
        catch (Exception e)
        {
            imageObject = null;
            titleString = " ERROR >> " + e.getMessage();
        }
        System.gc();
        return;
    }

    /**
     * �f�[�^�\�����I�����܂�
     * 
     */
    public void stop()
    {
        exitScreen();
        return;
    }
    
    /**
     * ��ʕ`�揈��
     * 
     * @param g �O���t�B�b�N�X�N���X
     * 
     */
    public void paint(Graphics g)
    {
        try
        {
            // �^�C�g���̈�̕`��
            drawTitleArea(g);

            if (busyMode != true)
            {
                // ���C���p�l���̕`��...
                drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (screenHeight - screenFont.getHeight()));
            }
        }
        catch (Exception e)
        {
            // �Ȃ����k���|���o��̂�...
        }
        return;
    }

    /**
     * ��ʂ𔲂���...
     *
     */
    private void exitScreen()
    {
        imageObject = null;
        operation.cancelPictureView();
        offsetX = 0;
        offsetY = 0;
        finishCanvas();
        return;
    }

    /**
     * ���̈�ɏ���ݒ肷��
     * @param message �ݒ肷����
     */
    public void setInformation(String message)
    {
        titleString = message;
        busyMode = false;
        return;
    }    

    /**
     * ���̈�̏����X�V���܂�
     * �@@param message ���̈�ɕ\�����镶����
     *   @param mode ���̒l�̏ꍇ�ɂ́A�摜��\�����܂���
     */
    public void updateInformation(String message, int mode)
    {
        titleString = message;
        if (mode < 0)
        {
            busyMode = true;
        }
        else
        {
            busyMode = false;
        }
        return;        
    }

    /**
     * ���C���p�l���̈�ɕ`�悷��
     * 
     * @param g    �O���t�B�b�N�L�����o�X
     * @param topX ������WX
     * @param topY ������WY
     * @param width �`��̈敝
     * @param height �`��̈捂��
     */
    private void drawMainPanel(Graphics g, int topX, int topY, int width, int height)
    {
        // �t�H���g�T�C�Y
        int fontHeight = screenFont.getHeight();

        // �g��\������
        g.setColor(backColor);
        g.fillRect(topX, topY, width, height);

        // �C���[�W�̕\�����W�����߂�
        int x = topX;
        if (imageObject.getWidth() < width)
        {
            x = topX + ((width  - imageObject.getWidth())  / 2);
        }

        int y = topY;
        if (imageObject.getHeight() < height)
        {
            y = y + ((height - imageObject.getHeight()) / 2);
        }

        // �C���[�W��`�悷��
        g.setColor(foreColor);
        if (imageObject != null)
        {
            g.drawImage(imageObject, x, y, (Graphics.LEFT | Graphics.TOP));
        }
        else
        {
            // �C���[�W���Ȃ��̂� ????? ��\������
            g.drawString("?????", (topX + screenFont.stringWidth("��")), (topY + fontHeight * screenLine / 2), (Graphics.LEFT | Graphics.TOP));
        }
        return;
    }


    /**
     *   ��ʂ̏I���i���̉�ʂ֐؂�ւ���j
     * 
     */
    protected void finishCanvas()
    {
        int nextScene = operation.nextSceneToChange(canvasId);
        if (nextScene < 0)
        {
            parent.previousScene();
        }
        else
        {
            parent.changeScene(nextScene, null);
        }
        return;
    }
}
