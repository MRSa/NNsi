import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;


/**
 * �u�ǋL�����v�̃��C����� 
 *  @author MRSa
 *
 */
public class A2chSMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
	private A2chSSceneStorage storageDb = null;
	private A2chSListStorage listStorageDb = null;
	private boolean lockOperation = false;

    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public A2chSMainScreen(int screenId, MidpSceneSelector object, A2chSSceneStorage storage, A2chSListStorage listStorage)
    {
        super(screenId, object);        
        storageDb = storage;
        listStorageDb = listStorage;
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     * 
     */
    public void start()
    {
        // �L�^����t�@�C�������擾����       
        String directory = storageDb.getDefaultDirectory();
        if (directory == null)
        {
        	storageDb.resetSelectionItems();
            parent.changeScene(A2chSSceneDB.WELCOME_SCREEN, "�悤�����I");
        }

        // �Q����񌟍������{����        
        if (storageDb.getUrlToSearch() != null)
        {
        	startFind2ch();
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
    	
        ////////// ������@�̋L�� //////////
    	try
    	{
        	int fontHeight = screenFont.getHeight() + 1; 
        	drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 5) - 5, screenWidth - 20, (fontHeight * 5));

            String msg = storageDb.getInformationMessage();
            if (msg != null)
            {
                // ��񃁃b�Z�[�W��\������
            	g.drawString(msg, (screenTopX + 10), (screenTopY + (fontHeight * 6)), (Graphics.LEFT | Graphics.TOP));
            }
        	
            ///// a2B�f�B���N�g����\������ /////
            g.setColor(foreColorForOpe);
            g.drawString("a2B�i�[�f�B���N�g�� :", (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));
            g.drawString(storageDb.getDefaultDirectory(), (screenTopX + 15), (screenTopY + (fontHeight * 3)), (Graphics.LEFT | Graphics.TOP));
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
        g.drawString(": �X���I��",       topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Game D",           topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �u���E�U�ŊJ��", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �������������", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("�~",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �A�v���I��",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));

    }

    /**
     *  2�����˂錟�����s������
     *  <br>�{���̓R���g���[���̖�ځB�B�B
     *
     */
    private void startFind2ch()
    {
    	lockOperation = true;    // �\�z�I���܂ő�����֎~
    	storageDb.setInformationMessage("2ch�������s��...");
    	Thread thread = new Thread()
        {
            public void run()
            {
                // �����iURL����f�[�^�擾�j���s
                storageDb.doFind2chThreads();
                lockOperation = false;
                storageDb.setInformationMessage(null);
                inputFire(false);  // �\�z�I���̃g���K
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
        return;
    }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��F������������͉�ʂ֑J�ڂ���
     * 
     */
    public void showMenu()
    {
        if (lockOperation == true)
        {
            return;
        }
    	parent.changeScene(A2chSSceneDB.INPUT_SCREEN, "�������������");
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
    	if (lockOperation == true)
    	{
            // ���ɒ��{�^����������Ă����Ƃ��ɂ͉������Ȃ�
    		return (true);
    	}
    	lockOperation = true;
    	Thread thread = new Thread()
        {
            public void run()
            {
                // �t�@�C�������݂��邩�ǂ����H
            	if (listStorageDb.readSearchResultFile() == true)
                {
                    // �t�@�C�������݂���ꍇ�ɂ́A��ʂ��������ʈꗗ�֐؂�ւ���
                    parent.changeScene(A2chSSceneDB.LIST_SCREEN, "�������ʈꗗ");
                }
            	lockOperation = false;
            }
        };
        thread.start();
        return (true);
    }

    /**
     * GAME D�{�^���������ꂽ�Ƃ��F2ch�������ʃt�@�C�����u���E�U�ŊJ��
     * (�A�v�����I��������)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        if (lockOperation == true)
        {
            return (true);
        }
        String fileName = storageDb.getDefaultDirectory() + "temporary.html";
        parent.platformRequest(fileName, true);
        return (true);
    }
}
