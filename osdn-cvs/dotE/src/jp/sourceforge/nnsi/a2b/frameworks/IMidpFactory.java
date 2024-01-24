package jp.sourceforge.nnsi.a2b.frameworks;

import javax.microedition.midlet.MIDlet;

/**
 *  a2B framework�Ŏg�p����N���X���w�肵�܂�
 * 
 * @author MRSa
 *
 */
public interface IMidpFactory
{
    /**
     *  �f�[�^�X�g���[�W�N���X���擾����
     *  @return �f�[�^�X�g���[�W�N���X 
     */
    public abstract IMidpDataStorage getDataStorage();

    /**
     *   ��ʐؑփN���X���擾����
     *   @return ��ʐؑփN���X
     */
    public abstract IMidpSceneDB getSceneDB();
  
    /**
     *  �R�A�I�u�W�F�N�g�i�f�[�^�X�g���[�W�N���X�A��ʐؑփN���X�j�𐶐�����
     *  
     */
    public abstract void prepareCoreObjects();

    /**
     *  ���̑��̃I�u�W�F�N�g�̐���
     *  
     *  @param object   MIDlet�N���X
     *  @param selector �V�[���Z���N�^
     */
    public abstract void prepareObjects(MIDlet object, MidpSceneSelector selector);
}
