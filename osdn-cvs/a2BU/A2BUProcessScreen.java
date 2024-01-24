import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;

/**
 *  ���C����� 
 *  @author MRSa
 *
 */
public class A2BUProcessScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;

    private int foreColorForMsg = 0x00000000;
    private int backColorForMsg = 0x00f0f0f0;

    private BusyConfirmationStorage confirmStorage = null;
    private A2BUSceneStorage  sceneStorage = null;

    private boolean isSkipBBS    = false;
    private boolean isAbortBBS   = false;

    private int numberOfBBSes     = 0;
    private int numberOfFavorites = 0;

    private boolean logUpdating  = false;
    private int currentBBSindex    = 0;
    private int currentThreadindex = 0;

    private String getBoard = "";
    private String getTitle = "";
    
    
    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public A2BUProcessScreen(int screenId, MidpSceneSelector object, BusyConfirmationStorage confirm, A2BUSceneStorage sceneDb)
    {
        super(screenId, object);
        confirmStorage = confirm;
        sceneStorage = sceneDb;
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     * 
     */
    public void start()
    {
        numberOfBBSes     = sceneStorage.getNumberOfBBS();
        numberOfFavorites = sceneStorage.getNumberOfFavorites();

        String msg = "BBS:" + numberOfBBSes + " Favor:" + numberOfFavorites;
        sceneStorage.setInformationMessage(msg);

        // ���O�X�V�̊J�n...
        if (logUpdating == false)
        {
            startA2BLogUpdate();
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
        
        ////////// ��ʂ̕\��... //////////
        try
        {
            int fontHeight = screenFont.getHeight() + 1;
            drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 3) - 5, screenWidth - 20, (fontHeight * 3));

            // �擾�󋵂�\������B
            drawProgressInformation(g, screenTopX, (screenTopY + fontHeight + 5), screenWidth, (screenHeight - (fontHeight * 6) - 10));
 
            // ��񃁃b�Z�[�W��\������
            String msg = sceneStorage.getInformationMessage();
            if (msg != null)
            {
                // ��񃁃b�Z�[�W��\������
                g.drawString(msg, (screenTopX + 10), screenTopY + screenHeight - (fontHeight * 4) - 5, (Graphics.LEFT | Graphics.TOP));
            }             
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }
    /**
     *   ��ʂɎ擾�󋵂�\������
     * 
     * @param g
     * @param topX
     * @param topY
     * @param width
     * @param height
     */
    private void drawProgressInformation(Graphics g, int topX, int topY, int width, int height)
    {
        // �g��\������
        g.setColor(backColorForMsg);
        g.fillRect(topX, topY, width, height);
        g.setColor(foreColorForMsg);
        g.drawRect(topX, topY, width, height);

        int aY = topY + 2;
        g.drawString("�X�V�� : " + getTitle, topX + 5, aY, (Graphics.LEFT | Graphics.TOP));

        aY = aY + screenFont.getHeight();
        g.drawString("(" + (sceneStorage.getCurrentThreadCount() + 1) + "����" + ", " + getBoard + ")", topX + 5, aY, (Graphics.LEFT | Graphics.TOP));
        
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
        g.drawString("���������O�X�V�����쁡����", topX + 5, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �擾�X�L�b�v",   topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("�~",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ���O�X�V���f",   topX + 70, aY, (Graphics.LEFT | Graphics.TOP));

    }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��F�擾�𒆎~���A���̔փX�L�b�v����
     * 
     */
    public void showMenu()
    {
        isSkipBBS = true;
        String msg = ">>>>>> ��Skip������... ";
        sceneStorage.setInformationMessage(msg);
        return;
    }

    /**
     *  �A�v���P�[�V�������I�������邩�H
     * 
     *  @return  true : �I��������Afalse : �I�������Ȃ�
     */
    public boolean checkExit()
    {
        isAbortBBS = true;
        String msg = ">>>>>> ���f��... ";
        sceneStorage.setInformationMessage(msg);
       
        if (logUpdating == false)
        {
            // �V���m�F���f
            confirmStorage.showMessage("���O�X�V�𒆒f���܂��B��낵���ł����H", BusyConfirmationStorage.CONFIRMATION_MODE, A2BUSceneDB.DEFAULT_SCREEN);
        }
        return (false);
    }

    /**
     * ���{�^���������ꂽ�Ƃ��F�������Ȃ�
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        // ���b�Z�[�W��\������?
        sceneStorage.setInformationMessage("(�E�́E)��!!");
        return (true);
    }

    /**
     * GAME D�{�^���������ꂽ�Ƃ��F�������Ȃ�
     * (�A�v�����I�������Ȃ�)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        return (false);
    }

    
    private void startA2BLogUpdate()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                // ���s�̑O����...
                logUpdating = true;
                currentBBSindex    = 0;
                currentThreadindex = -1;

                sceneStorage.resetUpdateThreadCount();
                sceneStorage.prepare();

                // �X�V�������s�I
                updateA2BLogMain();

                return;
            }
        };
        try
        {
            thread.start();
        }
        catch (Exception ex)
        {
            //
        }
    }
    
    
    /**
     *  a2B�̃��O�X�V���s
     *  
     *  @param bbsIndex ��index�ԍ�
     *  @param threadIndex �X���ԍ�
     *
     */
    private void updateA2BLogMain()
    {
        String msg = "";
        int nofThreadsInBBS = sceneStorage.getNumberOfThreads();
        if ((currentThreadindex > 0)&&(currentThreadindex >= nofThreadsInBBS)&&(nofThreadsInBBS >= 0))
        {
            // �̐ؑւ������Aa2b.idx �̍X�V���˗�����
            sceneStorage.backupA2BIndex();

            currentBBSindex++;
            currentThreadindex = -1;
        }
        if (currentBBSindex >= numberOfBBSes)
        {
            // 
            msg = "���O�X�V�������I�����܂����B" + "(" + sceneStorage.getUpdateThreadCount() + "/" + sceneStorage.getCurrentThreadCount()  + ")";
            sceneStorage.setInformationMessage(msg);

            // �S���̔��X�V�����A�A�A�I������...
            finishLogUpdating();
            return;
        }
        if ((currentThreadindex < 0)||(nofThreadsInBBS < 0))
        {
            // �V�K�̔ǂݍ��݁Aa2b.idx �̓ǂݍ��݂��˗�����
            sceneStorage.prepareA2BIndex(currentBBSindex);

            currentThreadindex = -1;
            nofThreadsInBBS = sceneStorage.getNumberOfThreads();

            // subject.txt�������ɍX�V����H
            if (sceneStorage.isGetSubjectTxt() == true)
            {
                String subjectUrl = sceneStorage.getCurrentBBSUrl() + "/subject.txt";
                String subjectTxt = sceneStorage.getCurrentBBSNick() + "/subject.txt";
                getBoard = sceneStorage.getCurrentBBSNick();

                // ���b�Z�[�W��\������
                msg = "�X�V��:" + subjectTxt;
                sceneStorage.setInformationMessage(msg);
                getTitle = null;
                getTitle = "�ꗗ (" + subjectTxt + ")";
                sceneStorage.redraw(false);
                
            	// subject.txt���X�V����
            	sceneStorage.getSubjectTxt(subjectUrl, sceneStorage.getDefaultDirectory(), subjectTxt);
            }            
            
            // ���̃��O�X�V��...
            sceneStorage.finishTrigger(currentThreadindex);
            return;
        }
        
        // �擾����X���̏����擾����
        A2BUa2BsubjectDataParser.subjectDataHolder data = sceneStorage.getThreadData(currentThreadindex);
        if ((data == null)||(data.numberOfMessage > 1000))
        {
            // �f�[�^�擾���s or 1000����...���̃��O�X�V��...
            sceneStorage.finishTrigger(currentThreadindex);
            return;
        }
        
        // �擾��URL�ƍX�V���郍�O�t�@�C�����𐶐�����
        getTitle = null;
        getTitle = new String(data.getTitleName());
        getBoard = sceneStorage.getCurrentBBSNick();
        String fileName = data.threadNumber + ".dat";
        String url = sceneStorage.getCurrentBBSUrl() + "/dat/" + fileName;
        String directory = sceneStorage.getDefaultDirectory() + sceneStorage.getCurrentBBSNick() + "/";

        // ���O�X�V���b�Z�[�W�𐶐�����...
        sceneStorage.setInformationMessage(url);

        // ���O�X�V�̎��������Ăяo��
        sceneStorage.updateLogFile(url, directory, fileName, data);

        // ���̃��O�X�V��...
        sceneStorage.finishTrigger(currentThreadindex);
    }
    
    /**
     * �\���f�[�^�̍X�V����
     * 
     * @param processingId ����ID
     * 
     */
    public void updateData(int processingId)
    {
        // ��񃁃b�Z�[�W�̃N���A
        String msg = "";
        sceneStorage.setInformationMessage(msg);

        if (isAbortBBS == true)
        {
            // 
            msg = "���O�X�V�����𒆒f���܂����B";
            sceneStorage.setInformationMessage(msg);

            // �X�V�����̒��f�w������B
            abortLogUpdating();

            // ���O�X�V�����̏I��
            finishLogUpdating();

            return;
        }

        if (isSkipBBS == true)
        {
            // �X�V�����̃X�L�b�v�w������B

            // a2b.idx �̍X�V���˗�����
            sceneStorage.backupA2BIndex();
            
            // ���̔̐擪��...
            currentBBSindex++;
            currentThreadindex = -2; // ���ɃC���N�������g���邩��...

            isSkipBBS = false;
        }
        currentThreadindex++;

        Thread thread = new Thread()
        {
            public void run()
            {
                // �X�V�������s�I
                updateA2BLogMain();
                return;
            }
        };
        try
        {
            thread.start();
//            thread.join();
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }
    
    /**
     *  ���O�X�V�̒��f����
     *
     */
    private void abortLogUpdating()
    {
        // ���f���A���ׂ����Ƃ�����΁A�����ɏ�����ǉ�����
        return;
    }
    
    /**
     *  ���O�X�V�����̏I������
     *  (�K���ĂԁI)
     *
     */
    private void finishLogUpdating()
    {
        sceneStorage.redraw(false);

        // ���C�ɓ�������X�V����
        sceneStorage.writeFavoriteDb();
        
        // ���O�X�V�����𗎂Ƃ�
        currentBBSindex    = 0;
        currentThreadindex = -1;
        isAbortBBS  = false;
        logUpdating = false;
        
        // �o�C�u���s����H
        if (sceneStorage.isDoVibrate() == true)
        {
            parent.vibrate(500);
        }
        
        // �A�v���������I��������H
        if (sceneStorage.isAutoDestroyApplication() == true)
        {
        	// �T�b���炢�҂��Ă���I������B
            String msg = "a2BU�I����...";
            sceneStorage.setInformationMessage(msg);
            parent.sleep(2000);
            parent.stop();
        }
        
        // ��ʐؑ֏���...
        parent.changeScene(A2BUSceneDB.DEFAULT_SCREEN, "");
    }
}
