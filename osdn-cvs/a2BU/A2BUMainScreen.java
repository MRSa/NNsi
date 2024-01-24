import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;;

/**
 * �ua2BU�v�̃��C����� 
 *  @author MRSa
 *
 */
public class A2BUMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
    
    private A2BUDataStorage storageDb = null;
    private BusyConfirmationStorage confirmStorage = null;
    private A2BUSceneStorage  sceneStorage = null;

    private boolean isReady = false;
    
    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public A2BUMainScreen(int screenId, MidpSceneSelector object, A2BUDataStorage storage, BusyConfirmationStorage confirm, A2BUSceneStorage sceneDb)
    {
        super(screenId, object);
        storageDb = storage;
        sceneStorage = sceneDb;
        confirmStorage = confirm;
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     * 
     */
    public void start()
    {
        // a2B�̃��O�L�^�f�B���N�g�����擾����       
        String directory = storageDb.getBaseDirectory();
        if (directory == null)
        {
            parent.changeScene(A2BUSceneDB.WELCOME_SCREEN, "�悤�����I");
        }
        else
        {
            // ��͂���...
            if (isReady == false)
            {
                isReady = true;
                sceneStorage.prepareBoardUrl();
                sceneStorage.prepareFavoriteDb(directory);
            }
            int bbsCnt = sceneStorage.getNumberOfBBS();
            int favorCnt = sceneStorage.getNumberOfFavorites();
            int upCnt   = sceneStorage.getUpdateThreadCount();
            int curCnt  = sceneStorage.getCurrentThreadCount();
            int errCnt  = sceneStorage.getErrorThreadCount();
            String msg = "�F" + bbsCnt + " ���C�ɁF" + favorCnt;
            if (curCnt > 0)
            {
                msg = msg + "  �X�V:" + upCnt + "/" + curCnt + "(Err:" + errCnt + ")";
            }
            storageDb.setInformationMessage(msg);
        }
        return;
    }
    
    /**
     * ��ʂ�\��
     * 
     */
    public void paint(Graphics g)
    {
        if (g == null)
        {
            return;
        }        
        
        // �^�C�g���Ɖ�ʑS�̂̃N���A�͐e�i�p�����j�̋@�\���g��
        super.paint(g);
        
         try
        {
            int fontHeight = screenFont.getHeight() + 1; 

            // �����������Ă��邩�ǂ����H
            if (isReady == true)
            {
                ////////// ������@�̕\�� //////////
                drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 5) - 5, screenWidth - 20, (fontHeight * 5));
            }
            else
            {
/*
                // ��������\������
                String infoMsg = "a2B�̃��O��Ԃ���͒��B";  // (" + procCount + "%)";
                g.setColor(foreColorForOpe);
                g.drawString(infoMsg,        (screenTopX + 5), (screenTopY + screenHeight - (fontHeight * 2) - 4), (Graphics.LEFT | Graphics.TOP));
                g.drawString("�i���΂炭���҂����������B�j",   (screenTopX + 5), (screenTopY + screenHeight - (fontHeight * 1) - 2), (Graphics.LEFT | Graphics.TOP));
*/
            }

            ////////// ���̕\�� //////////
            String msg = storageDb.getInformationMessage();
            if (msg != null)
            {
                
            	g.drawString(msg, (screenTopX + 10), (screenTopY + (fontHeight * 6)), (Graphics.LEFT | Graphics.TOP));
            }
            
            ///// a2B�f�B���N�g����\������ /////
            g.setColor(foreColorForOpe);
            g.drawString("a2B���O�i�[Dir :", (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));
            g.drawString(storageDb.getBaseDirectory(), (screenTopX + 15), (screenTopY + (fontHeight * 3)), (Graphics.LEFT | Graphics.TOP));
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }

    /**
     *   ��ʂɑ���K�C�h��\������
     * 
     * @param g
     * @param topX
     * @param topY
     * @param width
     * @param height
     */
    private void drawOperationGuide(Graphics g, int topX, int topY, int width, int height)
    {
        // �g��\������
        g.setColor(backColorForOpe);
        g.fillRect(topX, topY, width, height);

        g.setColor(foreColorForOpe);
        g.drawRect(topX, topY, width, height);

        // �������\������
        int aY = topY + 2;
        g.drawString("������������@������", topX + 5, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight() + 2;
        g.drawString("Enter",            topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ���O�X�V�J�n",   topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Game D",           topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": a2B���ODir�w��", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �ݒ荀�ڕύX",   topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("�~",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �A�v���I��",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));

    }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��F������������͉�ʂ֑J�ڂ���
     * 
     */
    public void showMenu()
    {
        if (isReady == false)
        {
            return;
        }
        
//        parent.changeScene(A2BUSceneDB.MAIN_MENU, "���C�����j���[");
        parent.changeScene(A2BUSceneDB.EDITPREF_FORM, "a2BU�ݒ�");
        return;
    }

    /**
     *  �A�v���P�[�V�������I�������邩�H
     * 
     *  @return  true : �I��������Afalse : �I�������Ȃ�
     */
    public boolean checkExit()
    {
        return (true);
    }

    /**
     * ���{�^���������ꂽ�Ƃ��F�X���ꗗ��ʂ֑J�ڂ���
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        if (isReady == false)
        {
            return (true);
        }
        
        // ���O�X�V
        confirmStorage.showMessage("���O�X�V���J�n���܂��B��낵���ł����H", BusyConfirmationStorage.CONFIRMATION_MODE, A2BUSceneDB.PROCESS_SCREEN);
        return (true);
    }

    public boolean inputGameC(boolean isRepeat)
    {
        return (false);
    }
    
    /**
     * GAME D�{�^���������ꂽ�Ƃ��F�x�[�X�f�B���N�g���̎w���ʂ֑J��
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        if (isReady == false)
        {
            return (true);
        }
        parent.changeScene(A2BUSceneDB.DIR_SCREEN, "a2B���ODir�I��");
        return (true);
    }
}
