/**
 *  a2B Framework�̃T���v�������p�b�P�[�W�ł��B<br>
 *  �Œ���A�v���Ƃ��Đ��������邽�߂ɕK�v�ȃN���X���p�b�P�[�W�����Ă��܂��B
 */
package jp.sourceforge.nnsi.a2b.frameworks.samples;

import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import javax.microedition.midlet.MIDlet;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;

/**
 *  �t�@�N�g���N���X(�̃T���v������)
 *  
 *  @author MRSa
 *
 */
public class MidpDefaultFactory implements IMidpFactory
{
    /** �f�t�H���g�̉��ID�ł� */
    public final int DEFAULT_SCREEN = 100;
    
    private MidpDefaultSceneDB sceneDb = null;
    private MidpDefaultStorage dataStorage = null;
    
    /**
     *  �R���X�g���N�^�ł͉����������܂���
     *
     */
    public MidpDefaultFactory()
    {
        //
    }

    /**
     *  �I�u�W�F�N�g�𐶐����܂��B<br>
     *  �����ł͍Œ���V�[���ؑ֗p�̃f�[�^�x�[�X�N���X(IMidpSceneDB)��
     *  �f�[�^�ۑ��p�N���X(IMidpDataStorage)�𐶐�����K�v������܂�
     *  
     *  @see IMidpSceneDB
     *  @see IMidpDataStorage
     * 
     */
    public void prepareCoreObjects()
    {
        // �V�[���ؑփf�[�^�N���X�̐���
        sceneDb = new MidpDefaultSceneDB();
        
        // �f�[�^�X�g���[�W�N���X�̐���
        dataStorage = new MidpDefaultStorage();
    }

    /**
     *  �V�[���ؑփf�[�^�N���X����������
     * 
     *  @return �V�[���ؑփf�[�^�N���X
     */
    public IMidpSceneDB getSceneDB()
    {
        return (sceneDb);
    }

    /**
     *  �f�[�^�X�g���[�W�N���X����������
     * 
     *  @return �f�[�^�X�g���[�W�N���X
     */
    public IMidpDataStorage getDataStorage()
    {
        return (dataStorage);
    }

    /**
     *  �I�u�W�F�N�g�𐶐�����
     * 
     *  @param object MIDlet�x�[�X�N���X
     *  @param selector �V�[���Z���N�^
     */
    public void prepareObjects(MIDlet object, MidpSceneSelector selector)
    {

        // ��ʃT�C�Y�̎擾
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        // ���(�x�[�X)�N���X : �L�����o�X
        MidpCanvas myCanvas = new MidpCanvas(selector, Font.SIZE_SMALL);
        myCanvas.prepare(baseWidth, baseHeight);

        // �x�[�X�N���X��ݒ肷��
        sceneDb.setScreenObjects(myCanvas);

        // ��ʃT�C�Y�̎擾
        // int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        // int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        // ��ʂ̓o�^
        IMidpScreenCanvas screenObject = null;
        screenObject = new MidpScreenCanvas(DEFAULT_SCREEN, selector);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(10, 10, (baseWidth - 20), (baseHeight - 20));
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
