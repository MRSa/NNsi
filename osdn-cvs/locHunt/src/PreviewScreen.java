import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.images.PictureScreen;

public class PreviewScreen extends PictureScreen
{
    private final int backColor = 0x00ffffff;
    private final int foreColor = 0x00000000;

    private PreviewScreenOperator previewOperator = null;

	/**
     *   �R���X�g���N�^
     * @param screenId
     * @param object
     * @param operator
     */
	public PreviewScreen(int screenId, MidpSceneSelector object, PreviewScreenOperator operator)
    {
    	super(screenId, object, operator);
    	previewOperator = operator;
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
        	// �\���^�C�g���f�[�^��ݒ肷��
        	setTitle("Preview");
        	
            // �^�C�g���̈�̕`��
            drawTitleArea(g);

            // ���C���p�l���̕`��...
            drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (screenHeight - screenFont.getHeight()));

        }
        catch (Exception e)
        {
            // �Ȃ����k���|���o��̂�...
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
    	Image imageObject = null;
    	try
    	{
            // �t�H���g�T�C�Y
            int fontHeight = screenFont.getHeight();

            // �C���[�W�f�[�^���擾����
    		Image img = previewOperator.getImage(false);

    		// �C���[�W�f�[�^�̃T�C�Y���擾����
    		int imgHeight = img.getHeight();
    		int imgWidth = img.getWidth();
    		
            // �C���[�W���傫���ꍇ�A�C���[�W�̒������������𒊏o����
    		int srcX = imgWidth - width;
    		if (srcX > 0)
    		{
    			srcX = srcX / 2;
    			imgWidth = width;
    		}
    		else
    		{
    			srcX = 0;
    		}
    		
    		int srcY = imgHeight - (height + fontHeight + 2);
    		if (srcY > 0)
    		{
    			srcY = srcY / 2;
    			imgHeight = height;
    		}
    		else
    		{
    			srcY = 0;
    		}
    		imageObject = Image.createImage(img, srcX, srcY, imgWidth, (imgHeight - fontHeight), Sprite.TRANS_NONE);

            // �g��\������
            g.setColor(backColor);
            g.fillRect(topX, topY, width, height);

            // �C���[�W��`�悷��
            g.setColor(foreColor);
            if (imageObject != null)
            {
                g.drawImage(imageObject, topX, topY, (Graphics.LEFT | Graphics.TOP));
                g.drawString(previewOperator.getMessage(), topX + 3, topY + (height - fontHeight), (Graphics.LEFT | Graphics.TOP));
            }
            else
            {
                // �C���[�W���Ȃ��̂� ????? ��\������
                g.drawString("?????", (topX + screenFont.stringWidth("��")), (topY + fontHeight * screenLine / 2), (Graphics.LEFT | Graphics.TOP));
            }
    	}
    	catch (Exception ex)
    	{
            // �C���[�W���Ȃ��̂� ????? ��\������
            g.drawString("?????", (topX + screenFont.stringWidth("��")), (topY + screenFont.getHeight() * 2), (Graphics.LEFT | Graphics.TOP));
    	}
        return;
    }

	/**
     *  �A�v���P�[�V�������I�������邩�H
     * 
     *  @return  true : �I��������Afalse : �I�������Ȃ�
     */
    public boolean checkExit()
    {
        return (false);
    }

    /**
     *  Enter�L�[���쏈�� : �摜�Ȃ��ňʒu�����L�^����
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputFire(boolean isRepeat)
    {
    	try
    	{
    	    previewOperator.setSaveAction(true);
    	}
    	catch (Exception ex)
    	{
    		//
    	}
    	parent.changeScene(LocHuntSceneDB.DEFAULT_SCREEN, "");
        return (true);
    }

    /**
     *  Game C���쏈�� : �\���̃N���A
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	try
    	{
    	    previewOperator.setSaveAction(false);
    	}
    	catch (Exception ex)
    	{
    		//
    	}
    	parent.changeScene(LocHuntSceneDB.DEFAULT_SCREEN, "");
        return (true);
    }
}
