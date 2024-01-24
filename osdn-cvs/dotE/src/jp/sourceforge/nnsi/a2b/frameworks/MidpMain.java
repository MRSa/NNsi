/**
 *  a2B Framework�̒��j�p�b�P�[�W�ł��B
 */
package jp.sourceforge.nnsi.a2b.frameworks;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;


/**
 * MidpMain : MIDP�N���� 
 * (���̃N���X���p�����ăA�v���P�[�V�����̋N���������쐬����)
 * 
 * @author MRSa
 */
public class MidpMain extends MIDlet
{
    protected MidpSceneSelector sceneSelector = null;  // �V�[���Z���N�^
    private   IMidpFactory      midpFactory   = null;   // �t�@�N�g���N���X

    /**
     *  �R���X�g���N�^
     *  
     *  @param factory �t�@�N�g���N���X
     */
    public MidpMain(IMidpFactory factory)
    {
        // MIDlet�N���X�̏�����
        super();

        // �I�u�W�F�N�g�̐����E�ݒ�
        midpFactory = factory;

        // �V�[���Z���N�^�͊O�ɏo�������Ȃ����߁A�Q�i�K�Ő���
        midpFactory.prepareCoreObjects();
        IMidpDataStorage storage = midpFactory.getDataStorage();
        IMidpSceneDB sceneDb = midpFactory.getSceneDB();
        sceneSelector = new MidpSceneSelector(this, storage, sceneDb);
    }

    /**
     *  �A�v���P�[�V�����̋N������
     * 
     */
    protected void startApp() throws MIDletStateChangeException
    {
        // �X�v���b�V���X�N���[���𐶐��E�\������...
        splashScreen startScreen = new splashScreen(sceneSelector.getSplashImageName());
        Display.getDisplay(this).setCurrent(startScreen);

        // �N�����������{����
        sceneSelector.prepare();
        midpFactory.prepareObjects(this, sceneSelector);

        // �V�[���Z���N�^�ɏ��������w������
        boolean ret = sceneSelector.initialize();
        if (ret == false)
        {
            // �N�����s...�I������
            destroyApp(true);
            return;
        }

        // �V�[���Z���N�^�ɏ����̊J�n���w������
        sceneSelector.start();
        
        // ���Ƃ��܂�(�X�v���b�V���X�N���[��������)...
        startScreen.finish();
        startScreen = null;

        return;
    }

    /**
     *  �A�v���P�[�V�����̈ꎞ��~����
     * 
     */
    protected void pauseApp()
    {
        // ���݂̂Ƃ���A�������Ȃ�...
        return;
    }

    /**
     *  �A�v���P�[�V�����̏I������
     *  @param arg0 �I�����邩�H
     */
    protected void destroyApp(boolean arg0) throws MIDletStateChangeException
    {
        if (arg0 == true)
        {
            // �f�[�^�̃o�b�N�A�b�v�ƃI�u�W�F�N�g�̍폜�����s����
            sceneSelector.stop();

            // �I���w��
            notifyDestroyed();
        }
        return;
    }

    /**
     *  �X�v���b�V���X�N���[��(�N�����)�̕\���N���X
     * @author MRSa
     */
    private class splashScreen extends Canvas
    {
        Image image = null;  // �C���[�W�t�@�C���̏I��

        /**
         * �R���X�g���N�^�ł́A�C���[�W�t�@�C����ǂݏo���������s���܂�
         * @param imageFileName �C���[�W�t�@�C����
         */
        public splashScreen(String imageFileName)
        {
            if (imageFileName == null)
            {
                // �C���[�W�t�@�C�����������Ƃ��́A�摜�ǂݏo�����s��Ȃ�
                return;
            }

            // �C���[�W�t�@�C����ǂݏo��
            try
            {
                image = Image.createImage(imageFileName);
            }
            catch (Exception e)
            {
                image = null;
            }
        }

        /**
         *  �N����ʂ�\�����������
         *  @param g �O���t�B�b�N
         */
        public void paint(Graphics g)
        {
            // �^�C�g���̐ݒ�
            String screenTitle = sceneSelector.getApplicationName();
            
            // ��ʂ̓h��Ԃ�
            g.setColor(255, 255, 255);
            g.fillRect(0, 0, getWidth(), getHeight());
            
            if (image == null)
            {
                // �C���[�W���Ȃ��ꍇ�ɂ́A�A�v���P�[�V�������i�����j�̕\��
                g.setColor(32, 32, 32);
                Font font = Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_BOLD, Font.SIZE_LARGE);
                g.setFont(font);
                g.drawString(screenTitle, 0, (getHeight() / 2), (Graphics.LEFT | Graphics.TOP));
                return;
            }
            else
            {
                // �C���[�W�̕\�����W�����߂�
                int x = ((getWidth()  - image.getWidth())  / 2);
                if ((x + image.getWidth()) > getWidth())
                {
                    x = 0;
                }

                int y = ((getHeight() - image.getHeight()) / 2);
                if ((y + image.getHeight()) > getHeight())
                {
                    y = 0;
                }

                // �X�v���b�V���X�N���[���̃C���[�W��`��
                g.setColor(0);
                g.drawImage(image, x, y, (Graphics.LEFT | Graphics.TOP));
            }
            return;
        }

        /**
         *  �N����ʂ̌�n��
         *
         */
        public void finish()
        {
            // �K�x�R�����{...
            image = null;
            System.gc();
            return;
        }
    }
}
