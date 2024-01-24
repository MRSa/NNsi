/**
 * �I�u�W�F�N�g�Ǘ��N���X
 * 
 * @author MRSa
 *
 */
public class a2BDobjectFactory
{
	private a2BDmain           parent = null;  // �e�I�u�W�F�N�g
    public  a2BDHttpCommunicator httpCommunicator = null; // HTTP�ʐM�Z���N�^
    public  a2BDfileSelector     fileSelector = null; // �t�@�C���Z���N�^
	public  a2BDsceneSelector  sceneSelector = null; // ��ʃZ���N�^
    public  a2BDdataInput      inputForm = null;     // �f�[�^���̓t�H�[��
    public  busyForm           busyScreen = null;    // ���s���\�����
    public  a2BDPreference     preference = null;    // �ݒ荀��

	/**
	 * �R���X�g���N�^
	 * @param object �e�N���X
	 */
	public a2BDobjectFactory(a2BDmain object)
	{
		parent = object;
	}
	
	/**
	 * �Ǘ��I�u�W�F�N�g�̐���
	 * 
	 * @return  �I�u�W�F�N�g��������/�������s
	 */
	public boolean prepare()
	{
		preference       = new a2BDPreference();
        preference.restore();

        busyScreen       = new busyForm(parent, "");
		sceneSelector    = new a2BDsceneSelector(parent, busyScreen);
		inputForm        = new a2BDdataInput("", sceneSelector, preference);
		httpCommunicator = new a2BDHttpCommunicator(sceneSelector);
		fileSelector     = new a2BDfileSelector(sceneSelector);
		return (true);
	}

	/**
	 * ��ʕ\���̏���
	 *
	 */
	public void prepareScreen()
	{
		busyScreen.prepareScreen();
		return;
	}
	/**
	 * �Ǘ��I�u�W�F�N�g�̍폜
	 *
	 */
	public void cleanup()
	{
        // �ݒ�f�[�^�̃N���[���A�b�v
		preference.backup();

		// �I�u�W�F�N�g�̂��|��...
		parent = null;
		httpCommunicator = null;
		sceneSelector = null;
		inputForm = null;
		busyScreen = null;
		preference = null;
		System.gc();
		return;
	}
}
