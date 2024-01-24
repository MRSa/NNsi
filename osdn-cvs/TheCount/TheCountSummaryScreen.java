import java.util.Vector;
import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;

/**
 *  �O���t�ق��̕\����ʁI
 * @author MRSa
 *
 */
public class TheCountSummaryScreen extends MidpScreenCanvas
{
    private int foreColorForOpe = 0x00000000;
//    private int backColorForOpe = 0x00f0f0ff;
	private TheCountDataStorage  dataStorage = null;

	private boolean isSorted = true;
	private boolean isOutput = false;
	
	private int item1Color = 0x00c00000;
	private int item2Color = 0x0000c000;
	private int item3Color = 0x000000c0;
	private int item4Color = 0x00c000c0;
	private int item5Color = 0x00c0c000;
	private int item6Color = 0x0000c0c0;
	
    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public TheCountSummaryScreen(int screenId, MidpSceneSelector object, TheCountDataStorage itemStorage)
    {
        super(screenId, object);
        dataStorage = itemStorage;
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     * 
     */
    public void start()
    {
        return;
    }

    /**
     * ��ʂ�`�悷��
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
    	
        ////////// ��ʂ̕`�� //////////
    	try
    	{
        	int fontHeight = screenFont.getHeight() + 1; 

        	///// ��������\������ /////
            g.setColor(foreColorForOpe);
            g.drawString("������ :" + dataStorage.getTotalValueCount(), (screenTopX + 5), (screenTopY + (fontHeight * 1)), (Graphics.LEFT | Graphics.TOP));

            // ���b�Z�[�W���o��
            String outData = "* : �f�[�^�o��, CLR : �߂�";
            int fontWides = screenFont.stringWidth(outData);
            g.drawString(outData, screenTopX + screenWidth - fontWides - 5, (screenTopY + (fontHeight * 1)), (Graphics.LEFT | Graphics.TOP));

            // �O���t��\������
            int size = (screenHeight - (fontHeight * 7) - 2) - (screenTopY + (fontHeight * 2) + 5);
            drawGraph(g, ((screenWidth - size) / 2), (screenTopY + (fontHeight * 2)), size);

            // �f�[�^�l�̘g��\������
        	g.setColor(0);
        	g.drawRect(screenTopX + 3, (screenHeight - (fontHeight * 7) - 4), screenWidth - screenTopX - 6, fontHeight * 7 + 4);

        	// �o�͐�f�B���N�g����\������
        	g.drawString(dataStorage.getBaseDirectory(), screenTopX + 5, (screenHeight - (fontHeight * 7) - 1), (Graphics.LEFT | Graphics.TOP));

            ///// �f�[�^�l��\������  /////
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 6) - 1), fontHeight, item1Color, 0);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 5) - 1), fontHeight, item2Color, 1);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 4) - 1), fontHeight, item3Color, 2);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 3) - 1), fontHeight, item4Color, 3);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 2) - 1), fontHeight, item5Color, 4);
            drawItemData(g, (screenTopX + 5), (screenHeight - (fontHeight * 1) - 1), fontHeight, item6Color, 5);

    	}
    	catch (Exception ex)
    	{
    		//
    	}
        return;
    }

    /**
     *   �O���t��\������
     * @param g
     * @param topX
     * @param topY
     * @param wides
     */
    private void drawGraph(Graphics g, int topX, int topY, int wides)
    {
        int total = dataStorage.getTotalValueCount();

        int index = getItemIndex(0);
    	int value = dataStorage.getItemValue(index);
        double beginData = 0.0 + 90.0;
        double data = ((double) value / (double) total) * ((double) 360.0);
        g.setColor(item1Color);
    	g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(1);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
    	g.setColor(item2Color);
        g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(2);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
        g.setColor(item3Color);
    	g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(3);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
        g.setColor(item4Color);
        g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(4);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
        g.setColor(item5Color);
    	g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);

        index = getItemIndex(5);
    	value = dataStorage.getItemValue(index);
        beginData = beginData + data;
        data = ((double) value / (double) total) * ((double) 360.0);
    	g.setColor(item6Color);
        g.fillArc(topX, topY, wides, wides, (int) beginData, (int) data);
    }
    
    /**
     *   �A�C�e����\������
     * @param g
     * @param topX
     * @param topY
     * @param itemIndex
     */
    private void drawItemData(Graphics g, int topX, int topY, int height, int foreColor, int index)
    {
    	int itemIndex = getItemIndex(index);
    	int value = dataStorage.getItemValue(itemIndex);
    	String percent = ": " + dataStorage.getValuePercent(itemIndex) + "%";
     	String valueMsg = "(" + value + ")";

   	    g.setColor(foreColor);
    	g.fillRect(topX + 10, topY, (height - 2), (height - 2));
        g.drawString(dataStorage.getItemName(itemIndex), topX + 10 + height, topY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(percent, topX + 110, topY, (Graphics.LEFT | Graphics.TOP));
        g.drawString(valueMsg, topX + 170, topY, (Graphics.LEFT | Graphics.TOP));
    	return;
    }

    /**
     *   �f�[�^���擾����
     * @param index
     * @return
     */
    private int getItemIndex(int index)
    {
        // ��\�[�g���[�h�́A���̂܂܉�������
    	if (isSorted == false)
    	{
    		return (index);
    	}

    	int itemIndex = -1;
        try
        {
        	int value = 0;
        	Vector valueVector  = new Vector();
        	Vector itemVector   = new Vector();
        	for (int lp = 0; lp < 6; lp++)
        	{
                boolean isMatched = false;
        		value = dataStorage.getItemValue(lp);
        	    for (int vecIndex = 0; vecIndex < valueVector.size(); vecIndex++)
        	    {
        	    	int checkValue = ((Integer) valueVector.elementAt(vecIndex)).intValue();
        	    	if (checkValue < value)
        	    	{
        	    		valueVector.insertElementAt(new Integer(value), vecIndex);
        	    		itemVector.insertElementAt(new Integer(lp), vecIndex);
        	    		isMatched = true;
        	    		break;
        	    	}
        	    }
        	    if (isMatched == false)
        	    {
        	    	valueVector.addElement(new Integer(value));
        	    	itemVector.addElement(new Integer(lp));
        	    }
        	}
        	itemIndex = ((Integer) itemVector.elementAt(index)).intValue();
        }
        catch (Exception ex)
        {
        	itemIndex = -1;
        }
        return (itemIndex);
    }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��F�A�C�e���f�[�^�̕\�����Ԃ�ύX����
     * 
     */
    public void showMenu()
    {
    	isSorted = (isSorted == true) ? false : true;
        System.gc();
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
     * ���̑��̃{�^���������ꂽ�Ƃ��F
     * 
     */
    public boolean inputOther(int keyCode, boolean isRepeat)
    {
    	return (false);
    }
    
    /**
     * ���{�^���������ꂽ�Ƃ��F �������Ȃ�
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
    	return (false);
    }

    /**
     * GAME C�{�^���������ꂽ�Ƃ�...���C����ʂɖ߂�
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
    	if (isOutput == true)
    	{
    		return (false);
    	}
    	parent.changeScene(TheCountSceneDB.DEFAULT_SCREEN, "");
    	return (true);
    }

    /**
     * GAME D�{�^���������ꂽ�Ƃ�
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
    	return (false);
    }

    /**
     *  ���L�[���쏈��  : �f�[�^�t�@�C���o��
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        Thread thread = new Thread()
        {
            public void run()
            {
            	dataStorage.outputDataToFile();
            }
        };
        try
        {
        	isOutput = true;
        	thread.start();
        	isOutput = false;
        }
        catch (Exception ex)
        {
        	//
        }
        return (true);
    }
}
