import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

public class TVgProgramScreen extends MidpScreenCanvas
{
    private int foreColorForInf = 0x00000000;
    private int foreColorForPgm = 0x00000000;
    private int foreColorForStn = 0x000800c0;
    private int backColor000    = 0x00e0dfe3;
    private int backColor001    = 0x00e8f2fe;
    private int backColor002    = 0x00f8f8f8;
    private int startHour       = 0;
    private int startMinutes    = 0;
    private TVgProgramDataParser dataParser = null;
    private String appendMessage = null;
    private boolean isStationMode = false;    
    
    /**
     *  �R���X�g���N�^
     *  @param screenId
     *  @param object
     */
    public TVgProgramScreen(int screenId, MidpSceneSelector object, TVgProgramDataParser storage)
    {
        super(screenId, object);
        dataParser = storage;
        appendMessage = "";
    }

    /**
     *  ��ʂ��؂�ւ���ꂽ�Ƃ��ɌĂ΂�鏈��
     * 
     */
    public void start()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                dataParser.prepareDataToShow();
            };
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
            // �t�H���g���擾����
            int fontHeight = screenFont.getHeight() + 1; 

            ///// �ԑg�ꗗ��\������ /////
            if (dataParser.isDataFileIsReady() == false)
            {
                g.setColor(foreColorForInf);
                g.drawString("�ʐM��...���҂���������" + appendMessage, (screenTopX + 5), (screenTopY + (fontHeight * 4)), (Graphics.LEFT | Graphics.TOP));
            }
            else 
            {
                // �ԑg�f�[�^��\������
                if (isStationMode == false)
                {
                    // �������ƕ\��
                    drawProgramDataHour(g);
                }
                else
                {
                    // �e���r�ǂ��ƕ\��
                    drawProgramData(g);
                }
                if (dataParser.isReadyToConstruct() == false)
                {
                    // �f�[�^��͒��̕\��...
                    g.setColor(backColor000);
                    g.fillRect(screenTopX, (screenTopY + screenHeight - fontHeight - 1), screenWidth, fontHeight + 2);
                    g.setColor(foreColorForInf);
                    g.drawString("��͒�... " + appendMessage, (screenTopX + 10), (screenTopY + screenHeight - fontHeight), (Graphics.LEFT | Graphics.TOP));
                }
            }
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }
    
    /**
     *   �ԑg�\�̕\�� (�����P��)
     * 
     * @param g
     */
    private void drawProgramDataHour(Graphics g)
    {
        // �^�C�g�����X�V����
        updateTitleString();

        int nofStations = dataParser.getNumberOfTvStations();
        int fontHeight = screenFont.getHeight() + 1; 
        int drawLine = 1;
        for (int lp = 0; lp < nofStations; lp++)
        {
            // �w�i�F��ݒ肷��
            if ((lp % 2) == 0)
            {
                g.setColor(backColor001);
            }
            else
            {
                   g.setColor(backColor002);
            }
            g.fillRect(screenTopX, (screenTopY + fontHeight * drawLine), (screenTopX + screenWidth), (fontHeight * 2));

            // �e���r�ǖ���\������
            g.setColor(foreColorForStn);
            String station = dataParser.getTvStationName(lp);
            g.drawString(station, (screenTopX + 2), (screenTopY + fontHeight * drawLine), (Graphics.LEFT | Graphics.TOP));
            drawLine++;        

            // �e���r�ԑg��\������
            g.setColor(foreColorForPgm);
            String program = dataParser.getProgramDataFromMinutes(lp, ((startHour * 60) + startMinutes), true);
            g.drawString(program, (screenTopX + 10), (screenTopY + fontHeight * drawLine), (Graphics.LEFT | Graphics.TOP));
            drawLine++;
        }
    }


    /**
     *   �ԑg�\�̕\�� (�e���r�ǂ��ƕ\��)
     * @param g
     */
    private void drawProgramData(Graphics g)
    {
        int fontHeight = screenFont.getHeight() + 1; 
        int lines = screenHeight / fontHeight;

        updateTitleString();

        int drawLine = 1;
        int stationId = dataParser.getTvStationIndex();
        int minute    = ((startHour * 60) + startMinutes);
        String data  = dataParser.getProgramDataFromMinutes(stationId, minute, false);
        int index    = dataParser.getProgramIndex(stationId, minute);
        int count = 0;
        while (drawLine < lines)
        {
            count++;
            if ((count % 2) == 1)
            {
                g.setColor(backColor001);
            }
            else
            {
                   g.setColor(backColor002);
            }
            g.fillRect((screenTopX + 3), (screenTopY + fontHeight * drawLine), (screenTopX + screenWidth), (fontHeight * (lines - drawLine)));
            drawLine = drawLine + drawString(g, drawLine, 3, screenFont.stringWidth("00:00 "), data);
            index++;
            data = dataParser.getProgramDataFromIndex(stationId, index, false);
        }
    }

    /**
     *   �\���^�C�g�����X�V����
     */
    private void updateTitleString()
    {
        if (isStationMode == false)
        {
        	titleForeColor = foreColorForPgm;
            titleString = dataParser.getTvStationTitle();
        }
        else
        {
        	titleForeColor = foreColorForStn;
            titleString = dataParser.getTvStationString();
        }
        String append = "";
        if (startMinutes < 10)
        {
            append = "0";
        }
        titleSubString = " (" + dataParser.getDataDate() + " " + ((startHour + 5) % 24) + ":" + append + startMinutes + "-" + ")";
    }

    /**
     *  ���j���[�{�^���������ꂽ�Ƃ��F�����ǑI����ʂ֑J�ڂ���
     * 
     */
    public void showMenu()
    {
        isStationMode = true;
        parent.changeScene(TVgSceneDb.SCENE_STATIONSELECTION, "�����ǑI��");
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
        try
        {
            appendMessage = message;
            updateTitleString();
        }
        catch (Exception ex)
        {
            // 
        }
        return;        
    }

    /**
     * ���{�^���������ꂽ�Ƃ��F�X���ꗗ��ʂ֑J�ڂ���
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        if (isStationMode == true)
        {
            isStationMode = false;
        }
        else
        {
            isStationMode = true;
        }
        updateTitleString();
        return (true);
    }

    /**
     * ��{�^���������ꂽ�Ƃ��F15�� or 60�����炷
     */
    public boolean inputUp(boolean isRepeat)
    {
        if (isStationMode == false)
        {
            startMinutes = startMinutes - 15;
            if (startMinutes < 0)
            {
                startMinutes = 45;
                startHour--;
                if (startHour < 0)
                {
                    startHour = 0;
                }
            }
        }
        else
        {
            startMinutes = startMinutes - 60;
            if (startMinutes < 0)
            {
                startMinutes = 0;
                startHour--;
                if (startHour < 0)
                {
                    startHour = 0;
                }
            }
        }
        updateTitleString();
        return (true);
    }

    /**
     *  ���{�^���������ꂽ�Ƃ��F1���Ԗ߂� or �O�̋ǑI��
     */
    public boolean inputLeft(boolean isRepeat)
    {
        if (isStationMode == false)
        {
        	// �\���f�[�^���ꎞ�Ԗ߂�
        	startHour--;
            if (startHour < 0)
            {
                startHour = 0;
                startMinutes = 0;
            }
        }
        else
        {
        	// �\���ǂ�ύX����
        	dataParser.changeTvStationIndex(-1);
        }
        updateTitleString();
        return (true);
    }

    /**
     *  �E�{�^���������ꂽ�Ƃ��F1���Ԑi�߂� or ���̋ǂ�I������
     */
    public boolean inputRight(boolean isRepeat)
    {
        if (isStationMode == false)
        {
            // �\���f�[�^���ꎞ�Ԑi�߂�
        	startHour++;
            if (startHour >= 24)
            {
                startHour = 23;
                startMinutes = 50;
            }
        }
        else
        {
        	// �\���ǂ�ύX����
        	dataParser.changeTvStationIndex(1);
        }
        updateTitleString();
        return (true);
    }    

    /**
     * ���{�^���������ꂽ�Ƃ��F15�������� or 1���ԉ�����
     */
    public boolean inputDown(boolean isRepeat)
    {
        if (isStationMode == false)
        {
            // �ꗗ�\�����[�h�̂Ƃ��́A15�������߂�
        	startMinutes = startMinutes + 15;
        }
        else
        {
            // �ǑI�����[�h�̂Ƃ��́A1���Ԃ����߂�
            startMinutes = startMinutes + 60;
        }
        if (startMinutes >= 60)
        {
            startMinutes = 0;
            startHour++;
            if (startHour >= 24)
            {
                startHour = 23;
                startMinutes = 50;
            }
        }
        updateTitleString();
        return (true);
    }

    /**
     *  ���{�^���������ꂽ�Ƃ��F4���Ԗ߂�
     */
    public boolean inputGameA(boolean isRepeat)
    {
        startHour = startHour - 4;
        if (startHour < 0)
        {
            startHour = 0;
            startMinutes = 0;
        }
        updateTitleString();
        return (true);
    }

    /**
     *  �E�{�^���������ꂽ�Ƃ��F4���Ԑi�߂�
     */
    public boolean inputGameB(boolean isRepeat)
    {
        startHour = startHour + 4;
        if (startHour >= 24)
        {
            startHour = 23;
            startMinutes = 50;
        }
        updateTitleString();
        return (true);
    }    

    /**
     * GAME C�{�^���������ꂽ�Ƃ��F���t�I����ʂɖ߂�
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
        if ((dataParser.isReadyToConstruct() == false)||(dataParser.isDataFileIsReady() == false))
        {
            // �f�[�^�擾�� or �f�[�^��͒��̂Ƃ��ɂ́A���t�I����ʂɂ͖߂�Ȃ��悤�ɂ���B
            return (false);
        }
        parent.changeScene(TVgSceneDb.SCENE_DATELIST, "���t�ꗗ");
        return (true);
    }

    /**
     * GAME D�{�^���������ꂽ�Ƃ��F�\���ؑցi�s�u�ǁˎ����A�����˂s�u�ǁj
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        if (isStationMode == true)
        {
            isStationMode = false;
        }
        else
        {
            isStationMode = true;
        }
        updateTitleString();
        return (true);
    }
    /**
     *  �����L�[�����͂��ꂽ�Ƃ��̏���
     *  @param number ���͂��ꂽ����
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     *
     */
    public boolean inputNumber(int number, boolean isRepeat)
    {
        if (number == 0)
        {
            int min = dataParser.getCurrentTime();
            startHour    = (min / 60);
            startMinutes = (min % 60);
            updateTitleString();
            return (true);
        }
        return (false);
    }

    /**
     *  ���L�[���쏈���F�����ֈړ�
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputPound(boolean isRepeat)
    {
        startHour = 23;
        startMinutes = 0;
        updateTitleString();
        return (true);
    }

    /**
     *  ���L�[���쏈���F�擪�ֈړ�
     *  @param isRepeat �L�[���s�[�g���͂��ǂ����itrue:���s�[�g���́j
     *  @return ���̃��\�b�h�ŏ�������(true) / ���̃��\�b�h�ł͏��������Ă��Ȃ�(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        startHour = 0;
        startMinutes = 0;
        updateTitleString();
        return (true);
    }
}
