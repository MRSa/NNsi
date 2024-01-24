import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

public class TVgDateScreen extends MidpScreenCanvas
{
    
    private final int MAX_DAY = 5;
    private int foreColorForOpe = 0x00000000;
//    private int backColorForOpe = 0x00f0f0ff;
    
    private TVgProgramDataParser dataParser = null;
    
    private int currentSelectionItem = 1;  
        
    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public TVgDateScreen(int screenId, MidpSceneSelector object, TVgProgramDataParser storage)
    {
        super(screenId, object);
        dataParser = storage;
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     * 
     */
    public void start()
    {
        if (dataParser.start() == false)
        {
            parent.changeScene(TVgSceneDb.SCENE_WELCOME, "TV�ԑg�\�ɂ���");
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
        
        try
        {
            // ��ʃ^�C�g�����X�V���� (�n�於�Ǝ�ʂ�\������)
            titleString = dataParser.getTvStationTitle();

            // �^�C�g���Ɖ�ʑS�̂̃N���A�͐e�i�p�����j�̋@�\���g��
            super.paint(g);
            
            ///// �ꗗ��\������ /////
            drawDateList(g);
            
         }
        catch (Exception ex)
        {
            //
        }
        return;
    }

    /**
     *   ���t�ꗗ��\������
     * @param g �O���t�B�b�N
     */
    private void drawDateList(Graphics g)
    {
        // �t�H���g���擾����
        int fontHeight = screenFont.getHeight() + 1;
        int fontWidth = screenFont.stringWidth("�� ") + 5;

        // ������� 5�����̏����ꗗ�\������
        for (int lp = -1; lp < MAX_DAY; lp++)
        {
            g.setColor(foreColorForOpe);
            g.drawString(dataParser.getDateDataString(lp), (screenTopX + fontWidth), (screenTopY + (fontHeight * 2) + (fontHeight * (lp + 1))), (Graphics.LEFT | Graphics.TOP));
        }

        // �J�[�\����\������
        g.setColor(foreColorForOpe);
        g.drawString("��", (screenTopX + 5), (screenTopY + (fontHeight * 2) + (fontHeight * currentSelectionItem)), (Graphics.LEFT | Graphics.TOP));
   }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��F������������͉�ʂ֑J�ڂ���
     * 
     */
    public void showMenu()
    {
        parent.changeScene(TVgSceneDb.SCENE_MENUSELECTION, "�I�����j���[");
        return;
    }

    /**
     *  �A�v���P�[�V�������I�������邩�H
     * 
     *  @return  true : �I��������Afalse : �I�������Ȃ�
     */
    public boolean checkExit()
    {
        // �I�������V�[�����L�^����
        return (true);
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
     * ���{�^���������ꂽ�Ƃ�
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.changeScene(TVgSceneDb.SCENE_PROGRAMLIST, "�ԑg�\");
                dataParser.selectedDateData(currentSelectionItem - 1, false);
            }
        };
        thread.start();
        return (true);
    }

    /**
     *  ��L�[�������ꂽ�Ƃ��̏���<br>
     *  �O�̃A�C�e����I�����܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
    public boolean inputUp   (boolean isRepeat)
    {
        currentSelectionItem--;
        if (currentSelectionItem < 0)
        {
            currentSelectionItem = MAX_DAY;
        }
        return (true);
    }

    /**
     *  ���L�[�������ꂽ�Ƃ��̏���<br>
     *  ���̃A�C�e����I�����܂��B
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
    public boolean inputDown (boolean isRepeat)
    {
        currentSelectionItem++;
        if (currentSelectionItem > (MAX_DAY))
        {
            currentSelectionItem = 0;
        }
        return (true);
    }

    /**
     *  ���L�[�������ꂽ�Ƃ��̏���<br>
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
    public boolean inputLeft (boolean isRepeat)
    {
        return (true);
    }

    /**
     *  �E�L�[�������ꂽ�Ƃ��̏���<br>
     *  @param isRepeat �L�[���s�[�g����
     *  @return ���삵��(true) / ���삵�Ȃ�(false)
     */
    public boolean inputRight(boolean isRepeat)
    {
        return (true);
    }

    /**
     * GAME C�{�^���������ꂽ�Ƃ��F���t�I����ʂɖ߂�
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
/*
        if (lockOperation == true)
        {
            return (true);
        }
        String fileName = storageDb.getDefaultDirectory() + "temporary.html";
        parent.platformRequest(fileName, true);
*/
        return (true);
    }

    /**
     * GAME D�{�^���������ꂽ�Ƃ��F�f�[�^���擾����
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.changeScene(TVgSceneDb.SCENE_PROGRAMLIST, "�ԑg�\");
                dataParser.selectedDateData(currentSelectionItem - 1, true);
            }
        };
        thread.start();
        return (true);
    }
}
