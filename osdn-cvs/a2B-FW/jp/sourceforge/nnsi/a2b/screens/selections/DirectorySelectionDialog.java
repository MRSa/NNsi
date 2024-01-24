package jp.sourceforge.nnsi.a2b.screens.selections;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;

/**
 * �f�B���N�g���I���̃_�C�A���O
 * 
 * @author MRSa
 *
 */
public class DirectorySelectionDialog extends SelectionScreen
{
    // �f�B���N�g���I��p����
	private DirectorySelectionDialogOperator directoryOperation = null;

    /**
     *  �R���X�g���N�^�ł́A�K�v�ȃ��[�e�B���e�B�����炢�܂�
     *  
     *  @param screenId ���ID
     *  @param object �V�[���Z���N�^
     *  @param fileOperator �t�@�C�������ʗp���샆�[�e�B���e�B
     *  
     */
    public DirectorySelectionDialog(int screenId, MidpSceneSelector object, SelectionScreenOperator operator, DirectorySelectionDialogOperator fileOperator)
    {
        super(screenId, object, operator);
        directoryOperation = fileOperator;
    }

    /**
     *   ��ʂ̏�����
     * 
     */
    public void start()
    {
    	super.start();

    	// �f�B���N�g���̕\������
    	directoryOperation.prepare(this);
    }

    /**
     *  ��ʂ����t���b�V������<br>
     *  �i�t�@�C���f�[�^�\�z�I�����ɌĂяo���j
     *
     */
    public void refreshScreen()
    {
        // ��ʂ�������
    	super.start();
    	return;
    }
}
