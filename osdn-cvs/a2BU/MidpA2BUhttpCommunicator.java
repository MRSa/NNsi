import jp.sourceforge.nnsi.a2b.utilities.IMidpA2BhttpCommunicator;
import jp.sourceforge.nnsi.a2b.utilities.MidpDefaultA2BhttpCommunicator;

public class MidpA2BUhttpCommunicator extends MidpDefaultA2BhttpCommunicator
{
	private A2BUSceneStorage sceneStorage = null;

	/**
	 *   �R���X�g���N�^
	 * @param get2chAppendMode
	 * @param prefDb
	 */
	public MidpA2BUhttpCommunicator(boolean get2chAppendMode, A2BUSceneStorage prefDb)
    {
    	super(get2chAppendMode);

    	sceneStorage = prefDb;
    }
    
    /**
     *  �v���L�V�ʐM���[�h���m�F����<br>
     * @return �v���L�V�ʐM���[�h
     */
    public int GetProxyMode()
    {
    	if (sceneStorage.isWorkaroundGzip() == true)
    	{
    		return (IMidpA2BhttpCommunicator.PROXY_WORKAROUND_WX310);
    	}
    	return (IMidpA2BhttpCommunicator.PROXY_DO_NOT_USE_PROXY);
    }
}
