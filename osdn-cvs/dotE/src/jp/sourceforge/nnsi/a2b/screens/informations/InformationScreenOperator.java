package jp.sourceforge.nnsi.a2b.screens.informations;

import java.util.Vector;

import javax.microedition.lcdui.Font;


/**
 *  ���\����ʂ̉�ʑ���N���X�ł�<br>
 *  ��ʓ��Ɏ��܂�悤�A�\���f�[�^���P�s���Ƃɉ��H���܂��B<br>
 * 
 * @author MRSa
 */
public class InformationScreenOperator
{
    static public final int WIDTH_MARGIN = 3;
    static public final int HEIGHT_MARGIN = 0;
	private IInformationScreenStorage storage = null;
    private Vector                    msgLines = null;
    private boolean                  buildingData = false;

    /**
     * �R���X�g���N�^�͓��ɂȂɂ����s���܂���B
     */
    public InformationScreenOperator(IInformationScreenStorage object)
    {
        storage = object;
    }

    /**
     *  �N���X�̎��s�������A���ɍs���܂���
     */
    public void prepare()
    {
    }

    /**
     * ���b�Z�[�W���E�B���h�E���Ɏ��܂�悤�ɉ��H���܂�
     * 
     * @param width �E�B���h�E��
     * @param height �E�B���h�E����
     * @param screenFont �\������t�H���g
     * @param folding �E�B���h�E���Ɏ��߂�悤���`(true) / �P�s�͂P�s�Ƃ��Đ��`(false)
     * @return �t�H�[�}�b�g����(true)�A�t�H�[�}�b�g���s(false)
     */
    public boolean formatMessage(int width, int height, Font screenFont, boolean folding)
    {
    	String message = storage.getInformationMessageToShow();
    	if (message == null)
    	{
            // �\�����ׂ����b�Z�[�W���Ȃ�����...
    		return (false);
    	}
//    	int maxLine = height / (screenFont.getHeight() + 1);
        divideLine(width, screenFont, message + "\n", folding);
    	System.gc();
        return (true);
    }

    /**
     * �\���s�P�ʂɃf�[�^�𕪊�����
     * 
     * @param width
     * @param screenFont
     * @param message
     * @param folding �s�[�ł��肽���ݏ������s�� / �s��Ȃ�
     * @return �\�����郁�b�Z�[�W�̍s��
     */
    private int divideLine(int width, Font screenFont, String message, boolean folding)
	{
    	if (buildingData == true)
    	{
    		return (0);
    	}
    	buildingData = true;
    	
        msgLines = null;
    	msgLines = new Vector();
    	
        int startPos = 0;
        int endPos = message.length();
        int msgIndex = 1;
    	try
    	{
            if (folding == true)
    		{
                // ��ʂ̒[�Ő܂�Ԃ��ꍇ�̏���...
            	for (msgIndex = 1; msgIndex < endPos; msgIndex++)
                {
                	char drawChar = message.charAt(msgIndex);
                	if (drawChar == '\n')
                	{
                        msgLines.addElement(message.substring(startPos, msgIndex));
                        startPos = msgIndex + 1;
                	}
                	else if (screenFont.substringWidth(message, startPos, (msgIndex - startPos)) > (width - WIDTH_MARGIN))
                	{
                        msgLines.addElement(message.substring(startPos, msgIndex - 1));
                        startPos = msgIndex - 1;
                	}
                	if (startPos > endPos)
                	{
                		break;
                	}
                }
            }
            else
            {
                // �s�[�Ő܂�Ԃ����������Ȃ��ꍇ... (�s�P�ʂŃf�[�^�𕪊�)
            	msgIndex = message.indexOf("\n", startPos);
                while (msgIndex >= 0)
                {
                	String lineData = message.substring(startPos, msgIndex);
                	msgLines.addElement(lineData);
                	startPos = msgIndex + 1;
                	msgIndex = message.indexOf("\n", startPos);
                }
            }
    	}
        catch(Exception ex)
        {
//        	String msg = ex.getMessage();
        }
    	buildingData = false;
        return (msgLines.size());
	}

    /**
     * �\�����郁�b�Z�[�W�̍s�����擾����
     * 
     * @return �\�����郁�b�Z�[�W�̍s��
     */
    public int getMessageLine()
    {
        return (msgLines.size());
    }
    
    /**
     *  ���b�Z�[�W�f�[�^�̎擾
     *  
     *  @param lineNumber �s�ԍ��i�O�X�^�[�g�j
     *  @return �\������s�f�[�^
     *  
     */
    public String getMessage(int lineNumber)
    {
        if ((lineNumber < 0)||(lineNumber >= msgLines.size()))
        {
            return ("");
        }
    	return ((String) msgLines.elementAt(lineNumber));
    }

    /**
     *  �\������{�^��������������
     *  @param screenId ���ID
     *  @return �\������{�^����
     */
    public int getNumberOfButtons(int screenId)
    {
        return (storage.getNumberOfButtons(screenId));
    }

    /**
     *  ��ʕ\���̏I��
     *  
     *  @param  buttonId �I�����ɉ����ꂽ�{�^��
     */
    public void finishScreen(int buttonId)
    {
    	msgLines = null;
    	storage.setButtonId(buttonId);
    	System.gc();
        return;
    }

    /**
     *  ��ʂɏo���{�^�����x������������
     * 
     * @param buttonId  �o�������{�^����ID
     * @return ��ʂɕ\������{�^�����x��
     */
    public String getButtonLabel(int buttonId)
    {
    	return (storage.getInformationButtonLabel(buttonId));
    }
    
    /**
     *   ���̉�ʂ��ǂ��ɐ؂�ւ��邩���߂�
     * 
     * @param sceneId ���݂̉��ID
     * @return ���̉��ID
     */
    public int nextSceneToChange(int sceneId)
    {
        return (storage.nextSceneToChange(sceneId));
    }
}
