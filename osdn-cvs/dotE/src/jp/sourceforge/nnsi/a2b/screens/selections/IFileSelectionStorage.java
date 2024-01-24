package jp.sourceforge.nnsi.a2b.screens.selections;

/**
 *  ファイル選択画面が使う、データアクセス用インタフェース
 * 
 * @author MRSa
 *
 */
public interface IFileSelectionStorage
{

    /**
	 *  初期ディレクトリを応答する
	 *  
	 *  @return 初期ディレクトリ
	 */
    public abstract String getDefaultDirectory();

    /**
     *  現在選択されているファイルおよびディレクトリの数を応答する
     *  
     *  @return 現在選択されているファイル(ディレクトリ)数
     */
    public abstract int getSelectedNameCount();
    
    /**
     *  選択（ファイル）名を応答する
     * 
     *  @param index 選択番号(0スタート)
     *  @return 選択番号のファイル名
     */
    public abstract String getSelectedName(int index);

    /**
     *  選択（ファイル）名を設定する
     * 
     * @param name 選択したファイルの名前
     */
    public abstract void setSelectedName(String name);
    
    /**
     *  選択（ファイル）名をクリアする
     *  
     */
    public abstract void clearSelectedName();
    
    /**
     *  選択（ファイル）名を確定する
     *  
     */
    public abstract void confirmSelectedName();
}
