/**
 *  �I����ʗp�̉�ʃp�b�P�[�W�ł��B
 * 
 */
package jp.sourceforge.nnsi.a2b.screens.selections;


/**
 * ���\����ʂŎg�p����f�[�^�̃C���^�t�F�[�X
 * 
 * @author MRSa
 *
 */
public interface ISelectionScreenStorage
{
    /** �L�����Z���{�^���̉��ID */
	static public final int BUTTON_CANCELED = -1;

	/** �O�̉�ʂɖ߂� */
	static public final int SCENE_TO_PREVIOUS = -1;

	
    /**
     *  �I�����̐����擾����
     * 
     * @return �I�����̐�
     */ 
    public abstract int numberOfSelections();

    /**
     *  �I�����̕�������擾����
     *  
     *  @param itemNumber �I����
     *  @return �I�����̕�����
     */
    public abstract String getSelectionItem(int itemNumber);


    /**
     *  �A�C�e���I���𕡐������ɉ\���ǂ�������������
     * 
     *  @return �����I���\(true) / �P����(false)
     */
    public abstract boolean isSelectedItemMulti();

    /**
     *  �I�������A�C�e�����N���A����
     *  
     */
    public abstract void resetSelectionItems();
    
    /**
     *  �I�𒆂̃A�C�e��������������
     * 
     *  @return �I�𒆂̃A�C�e����
     */
    public abstract int getNumberOfSelectedItems();

    /**
     *  �I�𒆂̂̃A�C�e��ID����������
     *  
     *  @param itemNumber �I�𒆂̃A�C�e���ԍ�(�[���X�^�[�g)
     *  @return �A�C�e��ID
     */
    public abstract int getSelectedItem(int itemNumber);


    /**
     *   �I�������A�C�e����ݒ肷��
     *   
     *   @param itemId �I�������A�C�e��ID
     */
    public abstract void setSelectedItem(int itemId);

    /**
     *  �I�����ꂽ�{�^��ID��ݒ肷��
     * 
     *  @param buttonId �{�^��ID
     *  @return �I������(true) / �I�����Ȃ� (false)
     */
    public abstract boolean setSelectedCommandButtonId(int buttonId);

    /**
     *  �I��p�̃K�C�h��������擾����<br>
     *  �i����������������ꍇ�A��ʉ����ɃK�C�h�������\������)
     *  
     *  @return �K�C�h������
     *  
     */
	public abstract String getSelectionGuidance();

	/**
     *  �^�C�g����\�����邩�ǂ����H
     *  
     *  @return �^�C�g���\������(true) / �\�����Ȃ�(false)
     */
	public abstract boolean isShowTitle();

	/**
	 *  start���ɁA�A�C�e�������������邩�ǂ���<br>
	 *  (�J�[�\���ʒu��ێ����Ă��������Ƃ���false�Ƃ���
	 *  
	 *  @return ����������(true) / ���������Ȃ�(false)
	 */
	public abstract boolean isItemInitialize();

	/**
	 *  �{�^���̐����擾����
	 * 
	 *  @return �\������{�^���̐�
	 */
	public abstract int getNumberOfSelectionButtons();

	/**
     *  �{�^���̃��x�����擾����
     *  
     *  @param buttonId �{�^����ID
     *  @return �\������{�^���̃��x��
     */
	public abstract String getSelectionButtonLabel(int buttonId);
	
	/**
     *  �w�i���݂��Ⴂ�̐F�ŕ\�����邩���擾����
     *
     *  @return �݂��Ⴂ�̐F�ɂ���(true) / �w�i�͂P�F(false)
     */
	public abstract boolean getSelectionBackColorMode();

	/**
     *  ���j���[�{�^���������ꂽ�^�C�~���O��ʒm����
     * 
     *  @return �������p�����邩�ǂ���
     */
	public abstract boolean triggeredMenu();
	
    /**
	 *  ���̉�ʐ؂�ւ�����ID���擾����
	 *  
	 *  @param screenId (���݂�)���ID
	 *  @return �؂�ւ�����ID
	 */
	public abstract int nextSceneToChange(int screenId);
}
