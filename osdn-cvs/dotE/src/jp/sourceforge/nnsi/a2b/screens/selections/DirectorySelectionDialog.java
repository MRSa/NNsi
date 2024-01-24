package jp.sourceforge.nnsi.a2b.screens.selections;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;

/**
 * ディレクトリ選択のダイアログ
 * 
 * @author MRSa
 *
 */
public class DirectorySelectionDialog extends SelectionScreen
{
    // ディレクトリ選択用操作
	private DirectorySelectionDialogOperator directoryOperation = null;

    /**
     *  コンストラクタでは、必要なユーティリティをもらいます
     *  
     *  @param screenId 画面ID
     *  @param object シーンセレクタ
     *  @param fileOperator ファイル操作画面用操作ユーティリティ
     *  
     */
    public DirectorySelectionDialog(int screenId, MidpSceneSelector object, SelectionScreenOperator operator, DirectorySelectionDialogOperator fileOperator)
    {
        super(screenId, object, operator);
        directoryOperation = fileOperator;
    }

    /**
     *   画面の初期化
     * 
     */
    public void start()
    {
    	super.start();

    	// ディレクトリの表示準備
    	directoryOperation.prepare(this);
    }

    /**
     *  画面をリフレッシュする<br>
     *  （ファイルデータ構築終了時に呼び出す）
     *
     */
    public void refreshScreen()
    {
        // 画面を初期化
    	super.start();
    	return;
    }
}
