package jp.sourceforge.nnsi.a2b.frameworks;

import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;



/**
 * �V�[���Z���N�^ : ��ʂ̐؂�ւ����s��
 * 
 * @author MRSa
 */
public class MidpSceneSelector
{
    private MIDlet           parent   = null;
    private MidpTimer        myTimer  = null;
    private IMidpDataStorage storage  = null;
    private IMidpSceneDB     sceneDb  = null;
    
    private String     currentTitle = null;
    private int       previousScene = IMidpSceneDB.NOSCREEN; // �O�̃V�[��
    private int        currentScene = IMidpSceneDB.NOSCREEN; // ���݂̃V�[��

    /**
     * �R���X�g���N�^
     * @param object ���C��MIDlet
     * @param dao �f�[�^�A�N�Z�X���[�e�B���e�B
     * @param scene �V�[���Z���N�^
     */
    public MidpSceneSelector(MIDlet object, IMidpDataStorage dao, IMidpSceneDB scene)
    {
        parent   = object;
        storage  = dao;
        sceneDb  = scene;
    }

    /**
     *  �A�v���P�[�V����������������
     * 
     * @return �A�v���P�[�V��������
     */
    public String getApplicationName()
    {
        return (storage.getApplicationName());
    }
    
    /**
     *  �N�����̃C���[�W������������
     * 
     * @return �N�����̃C���[�W�i�t�@�C���j����
     */
    public String getSplashImageName()
    {
        return (storage.getSplashImageName());
    }

    /**
     * �N���������s��
     * 
     */
    public void prepare()
    {
        // �������̏���...
        storage.prepare(parent);

        // �f�[�^�X�g���[�W�̕���...
        storage.restore();

        return;
    }

    /**
     *   �������������s��
     * 
     */
    public boolean initialize()
    {

        // �V�[���؂�ւ����̏�����
        sceneDb.prepare();

        // ��ʕ\���̏���...
        storage.prepareScreen(this);
 
        // �^�C�}�[�N���X�̐���
        myTimer = new MidpTimer(this);

        // �K�x�R�����{
        System.gc();

        return (true);
    }
    
    /**
     *  ������ʂɃV�[����؂�ւ���
     *
     */
    public void start()
    {
        changeScene(sceneDb.getDefaultScene(), null);
        long timeout = storage.getTimeoutCount();
        if (timeout > 0)
        {
            myTimer.startWatchDog(timeout);
        }
        return;
    }

    /**
     *  �A�v���P�[�V�������~���鏈��
     *
     */
    public void stop()
    {
        storage.backup();
        storage.cleanup();
        parent.notifyDestroyed();
        return;
    }

    /**
     *  �^�C���A�E�g�̎�M(�A�v���P�[�V�������I��)
     *
     *  @param  reason  true:�^�C���A�E�g����
     */
    public void detectTimeout(boolean reason)
    {
        if (reason == true)
        {
            stop();
            parent.notifyDestroyed();
        }
        return;
    }

    /**
     *  �^�C�}�[�̍X�V���s��
     *
     */
    public void extendTimer(int number)
    {
        myTimer.extendTimer(number);
        return;
    }

    /**
     *  ��ʂ��P�O�ɖ߂�...
     *
     */
    public void previousScene()
    {
        changeScene(previousScene, null);
    }

    /**
     *  ��ʕ\���ؑփX���b�h���s
     *
     *  @param  scene  �؂�ւ����ʃV�[��
     *  @param newTitle ��ʃ^�C�g��
     */
    public void changeScene(int scene, String newTitle)
    {
        // ��ʐ؂�ւ��\���ǂ����`�F�b�N����
        if (sceneDb.isAvailableChangeScene(scene, currentScene) != true)
        {
            // ��ʐ؂�ւ��s�������ꍇ�A�I������
            return;
        }

        // �؂�ւ����O�̃V�[�����L������
        if (currentScene != previousScene)
        {
            // �O��̉�ʃV�[���ƍ���̉�ʃV�[�����قȂ����Ƃ������L������
            previousScene = currentScene;
        }
        currentScene  = scene;
        currentTitle  = newTitle;

        // ��ʃV�[���؂�ւ��X���b�h���s�B�B�B
        Thread thread = new Thread()
        {
            public void run()
            {
                // ��ʂ�؂�ւ���...
                if (sceneDb.changeScene(currentScene, previousScene, currentTitle) == true)
                {
                    Display.getDisplay(parent).setCurrent(sceneDb.getScreen(currentScene));
                }

                // ��ʐ؂�ւ���̏���
                sceneDb.sceneChanged(currentScene, previousScene);
            }
        };
        try
        {
            thread.start();  // ��ʂ̐؂�ւ�����...
            //thread.join();   // �؂�ւ��܂ő҂�...
        }
        catch (Exception ex)
        {
            // �������Ȃ�...
        }

        // ���Ƃ��܂�
        thread = null;
        System.gc();
        return;
    }

    /**
     *    URL��Web�u���E�U�ŊJ���A�Ȃǂ̒[���ʗv������
     * 
     *    @param uri �v���b�g�t�H�[���v���X�g�����O
     *    @param isExit true�̏ꍇ�v����A�A�v���P�[�V�������I��������
     * 
     */
    public void platformRequest(String uri, boolean isExit)
    {
        try
        {
            // �v�����s��
            parent.platformRequest(uri);

            // �A�v���P�[�V�����̎��s���p�����邩�ǂ���
            if (isExit == true)
            {
                stop();
            }
        }
        catch (Exception e)
        {
            /////// ���T�|�[�g�������A�������Ȃ�  /////
        }
        return;
    }

    /**
     *  �u���u��������...
     *
     * @param milli (ms)
     */
    public void vibrate(int milli)
    {
        Display.getDisplay(parent).vibrate(milli);
        return;
    }

    /**
     * �����~�܂�
     * 
     * @param time (ms)
     */
    public void sleep(long time)
    {
        try
        {
            Thread.sleep(time);
        }
        catch (Exception e)
        {
            // �������Ȃ�...
        }
        return;
    }
}
