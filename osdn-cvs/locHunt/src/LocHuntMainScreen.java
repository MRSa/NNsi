import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import java.util.Date;

/**
 * �ulocHunt�v�̃��C����� 
 *  @author MRSa
 *
 */
public class LocHuntMainScreen extends MidpScreenCanvas
{
    private CameraDeviceController cameraDevice = null;
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
    private int foreColorForMessage = 0x00ff0000;
    private LocHuntSceneStorage storageDb = null;
    private String currentFileName = "";

    private int  dataLength = -1;
    private boolean performSaving = false;
    
    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public LocHuntMainScreen(int screenId, MidpSceneSelector object, LocHuntSceneStorage storage, CameraDeviceController camera)
    {
        super(screenId, object);        
        storageDb = storage;
        cameraDevice = camera;
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
            parent.changeScene(LocHuntSceneDB.WELCOME_SCREEN, "�悤�����I");
        }
        storageDb.setStartLocation(false);

        // �摜��ۑ�����H
        if (storageDb.getSaveAction() == true)
        {
            // �f�[�^��ۑ�����
        	storageDb.setSaveAction(false);
        	executeSaveAction();
        }
        return;
    }
    /**
     *   �f�[�^�̕ۑ������s����
     *
     */
    private void executeSaveAction()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                try
                {
                    Date currentDate = new Date();
                    currentFileName = currentDate.getTime() + "";
                    if (dataLength == 0)
                    {
                        // �擾�����摜�f�[�^��ۑ�����
                        storageDb.outputImageData(currentFileName, null);
                    }
                    else if (dataLength > 0)
                    {
                        // �擾�����摜�f�[�^��ۑ�����
                        storageDb.outputImageData(currentFileName, cameraDevice.getInputStream());                    	
                    }
                }
                catch (Exception ex)
                {
                	currentFileName = "";
                	foreColorForMessage = 0x007f7f00;
                    storageDb.setInformationMessage("�ۑ����s" + ex.getMessage());
                }
            }
        };
        try
        {
        	performSaving = true;
        	foreColorForMessage = 0x00ff0000;
        	storageDb.setInformationMessage("�ۑ���...���҂����������B");
        	thread.start();
        	thread.join();
        	performSaving = false;
        	foreColorForMessage = 0x000000ff;
            storageDb.setInformationMessage("�o�͏I��");
            System.gc();
        }
        catch (Exception ex)
        {
            
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
            drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 7) - 5, screenWidth - 20, (fontHeight * 7));

            String msg = storageDb.getInformationMessage();
            if (msg != null)
            {
                // ��񃁃b�Z�[�W��\������
            	g.setColor(foreColorForMessage);
                g.drawString(msg, (screenTopX + 10), (screenTopY + (fontHeight * 6)), (Graphics.LEFT | Graphics.TOP));
                g.drawString(msg, (screenTopX + 11), (screenTopY + (fontHeight * 6) + 1), (Graphics.LEFT | Graphics.TOP));
            }
            
            ///// �t�@�C���i�[�f�B���N�g����\������ /////
            g.setColor(foreColorForOpe);
            g.drawString("�t�@�C���i�[�f�B���N�g�� :", (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));
            g.drawString(storageDb.getDefaultDirectory(), (screenTopX + 15), (screenTopY + (fontHeight * 3)), (Graphics.LEFT | Graphics.TOP));

            ///// �f�[�^������\������  /////
            if (currentFileName.length() != 0)
            {
            	msg = currentFileName;
            	if (dataLength > 0)
            	{
                    msg = msg + ".jpg" + " (" + dataLength + " bytes)";
            	}
            	else
            	{
            		msg = msg + ".html";
            	}
            }
            g.drawString(msg, (screenTopX + 20), (screenTopY + (fontHeight * 7)), (Graphics.LEFT | Graphics.TOP));
            storageDb.setStartLocation(false);        
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
        g.drawString(": �J�����N��",     topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("#",                topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �ʒu�̂݋L�^",   topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Game D or *",      topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �u���E�U�ŊJ��", topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �B�e�����ݒ�",   topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("CLR",              topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �\���N���A",     topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("�~",               topX + 10, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �A�v���I��",     topX + 110, aY, (Graphics.LEFT | Graphics.TOP));
    }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��F�p�����[�^�ݒ��ʂ֑J�ڂ���
     * 
     */
    public void showMenu()
    {
        storageDb.setStartLocation(false);
    	parent.changeScene(LocHuntSceneDB.INPUT_SCREEN, "�p�����[�^�ݒ�");
        return;
    }

    /**
     *  �A�v���P�[�V�������I�������邩�H
     * 
     *  @return  true : �I��������Afalse : �I�������Ȃ�
     */
    public boolean checkExit()
    {
    	if (performSaving == true)
    	{
    		// �摜�ۑ����̂Ƃ��́A�������Ȃ�
    		return (false);
    	}
        return (true);
    }

    /**
     * ���{�^���������ꂽ�Ƃ��F�J�������N�����ăC���[�W�f�[�^��ۑ�����
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        // �ʒu�̋L��
    	storageDb.setStartLocation(false);

    	if (performSaving == true)
    	{
    		// �摜�ۑ����̂Ƃ��́A�������Ȃ�
    		return (false);
    	}
    	
        // �J�������N�����A�摜���L���v�`������
    	foreColorForMessage = 0x00000000;
        storageDb.setInformationMessage("�摜�B�e...");
        boolean ret = cameraDevice.doCapture();
        storageDb.setInformationMessage("");
        if (ret != true)
        {
        	currentFileName = "";
        	foreColorForMessage = 0x007f7f00;
            storageDb.setInformationMessage("(���B�e)");
            return (false);
        }

        // �v���r���[��ʂ�\������
        dataLength = cameraDevice.getDataLength();
    	parent.changeScene(LocHuntSceneDB.PREVIEW_SCREEN, "");

    	return (true);
    }

    /**
     *  �L�[���͂��ꂽ�Ƃ��̏���
     *  @param keyCode ���͂����L�[�̃R�[�h
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
        storageDb.setStartLocation(false);
    	if (performSaving == true)
    	{
    		// �摜�ۑ����̂Ƃ��́A�������Ȃ�
    		return (false);
    	}
    	if (keyCode ==Canvas.KEY_NUM0)
    	{
    		// �[���L�[�������ꂽ�Ƃ�...�u���E�U�ŊJ��...
    		parent.platformRequest(storageDb.getGoogleMapStaticMapUrl(240, 240, true), true);
	        return (true);
    	}
    	if (isRepeat == true)
    	{
    		return (false);
    	}
    	return (storageDb.setLocation(keyCode));
    }
    	
    /**
     *  Game C���쏈�� : �\���̃N���A
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	if (performSaving == true)
    	{
    		// �摜�ۑ����̂Ƃ��́A�������Ȃ�
    		return (false);
    	}
    	currentFileName = "";
    	foreColorForMessage = 0x00000000;
    	storageDb.setInformationMessage("");
        storageDb.setStartLocation(true);
        storageDb.clearLocation();
        return (true);
    }

    /**
     * GAME D�{�^���������ꂽ�Ƃ��F�u���E�U�ŊJ��
     * (�A�v�����I��������)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
    	if (performSaving == true)
    	{
    		// �摜�ۑ����̂Ƃ��́A�������Ȃ�
    		return (false);
    	}

    	// �t�@�C���������グ��
        String fileName = storageDb.getDefaultDirectory() + currentFileName + ".html";

        // �^�[�Q�b�g�̃t�@�C������ݒ肷��
        String targetFileName;
        
        // �[���`�F�b�N...
        String platForm = System.getProperty("microedition.platform");
        if (platForm.indexOf("WX320K") >= 0)
        {
            // WX320K�̎��ɂ́A�f�B���N�g����␳����B�B�B
            targetFileName = "file://localhost/D:/" + fileName.substring(8);
        }
        else if ((platForm.indexOf("WS023T") >= 0)||(platForm.indexOf("WS018KE") >= 0)||(platForm.indexOf("WS009KE") >= 0))
        {
            // WS023T or WS018KE�̎��ɂ́A�f�B���N�g����␳����B�B�B
            targetFileName = "file:///affm/nand0/PS/data/" + fileName.substring(19); // "file:///DataFolder/" ��u��
        }
        else if (platForm.indexOf("WX34") >= 0)
        {
            // WX340K or WX341K�̎��ɂ́A�f�B���N�g����␳����B�B�B
        	if (fileName.indexOf("file:///data/") >= 0)
        	{
        		// �f�[�^�t�H���_�̏ꍇ...
        		targetFileName = "file:///D:/" + fileName.substring(8); //   "file:///" ��u��
        	}
        	else
        	{
        	    // SD�J�[�h�̏ꍇ...
        		targetFileName = "file:///F:/" + fileName.substring(12); //  "file:///SD:/" ��u��
        	}
        }
        else
        {
            targetFileName = fileName;
        }

        // �w�肳�ꂽ�t�@�C�����u���E�U�ŊJ��
        parent.platformRequest(targetFileName, true);
        return (true);
    }

    /**
     *  ���L�[���쏈�� : �摜�Ȃ��ňʒu�����L�^����
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputPound(boolean isRepeat)
    {
        storageDb.setStartLocation(false);
    	if (performSaving == true)
    	{
    		// �摜�ۑ����̂Ƃ��́A�������Ȃ�
    		return (false);
    	}

    	// �ʒu�f�[�^������ۑ�����
    	dataLength = 0;
    	storageDb.setSaveAction(false);
    	executeSaveAction();

        return (true);
    }
    
    /**
     *  ���L�[���쏈�� : �u���E�U�ŊJ��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        return (inputGameD(isRepeat));
    }
    
}
