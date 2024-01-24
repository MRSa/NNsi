import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.framework.core.*;
import jp.sourceforge.nnsi.a2b.framework.base.*;

/**
 * �u�ǋL�����v�̃��C����� 
 *  @author MRSa
 *
 */
public class HiMemoMainScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
    private int backColorForOpe = 0x00f0f0ff;
	private HiMemoSceneStorage storageDb = null;
    
    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public HiMemoMainScreen(int screenId, MidpSceneSelector object, HiMemoSceneStorage storage)
    {
        super(screenId, object);        
        storageDb = storage;
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     * 
     */
    public void start()
    {
        // �L�^����t�@�C�������擾����       
        String fileName = storageDb.getMemoFileName();
        if (fileName == null)
        {
        	storageDb.resetSelectionItems();
            parent.changeScene(HiMemoSceneDB.WELCOME_SCREEN, "�悤�����I");
        }
        return;
    }

    /**
     * ��ʂ�\��
     * 
     */
    public void paint(Graphics g)
    {
        // �^�C�g���Ɖ�ʑS�̂̃N���A�͐e�i�p�����j�̋@�\���g��
    	super.paint(g);
    	
        ////////// ������@�̋L�� //////////
    	int fontHeight = screenFont.getHeight() + 1; 
    	drawOperationGuide(g, screenTopX + 10, screenTopY + screenHeight - (fontHeight * 5) - 5, screenWidth - 20, (fontHeight * 5));

        ///// ���ݎ�����\������ /////
/*
    	g.setColor(foreColorForOpe);
    	Date date = new Date();
        g.drawString("���ݎ��� : ", screenTopX + 5, (screenTopY + (fontHeight * 5)), (Graphics.LEFT | Graphics.TOP));
        g.drawString(date.toString(), screenTopX + 15, (screenTopY + (fontHeight * 6)), (Graphics.LEFT | Graphics.TOP));
*/

        ///// �L�^�t�@�C������\������ /////
        g.setColor(foreColorForOpe);
        g.drawString("�L�^�t�@�C���� :", (screenTopX + 5), (screenTopY + (fontHeight * 2)), (Graphics.LEFT | Graphics.TOP));
        g.drawString(storageDb.getMemoFileName(), (screenTopX + 15), (screenTopY + (fontHeight * 3)), (Graphics.LEFT | Graphics.TOP));

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
        g.drawString("Enter",            topX + 15, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": ��������",       topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Game D",           topX + 15, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �u���E�U�ŊJ��", topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("Menu",             topX + 15, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": (�@�\�Ȃ�)",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
        aY = aY + screenFont.getHeight();
        g.drawString("�~",               topX + 15, aY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(": �A�v���I��",     topX + 70, aY, (Graphics.LEFT | Graphics.TOP));
   
    }
    
    
    /**
     * ���{�^���������ꂽ�Ƃ��F�f�[�^���͉�ʂ֑J�ڂ���
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        parent.changeScene(HiMemoSceneDB.INPUT_SCREEN, "�f�[�^����");
        return (true);
    }

    /**
     * GAME D�{�^���������ꂽ�Ƃ��F�����f�[�^���u���E�U�ŊJ��
     * (�A�v�����I��������)
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        String fileName = storageDb.getMemoFileName();
        parent.platformRequest(fileName, true);
        return (true);
    }
}
