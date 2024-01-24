package jp.sourceforge.nnsi.a2b.utilities;
import javax.microedition.midlet.MIDlet;
import java.io.InputStream;
import java.lang.Class;

/**
 *  リソースファイルアクセスユーティリティ
 *  <br>
 *  端末内にあるデータを読み出すときに利用可能なユーティリティです。
 * 
 * @author MRSa
 *
 */
public class MidpResourceFileUtils
{
	private MIDlet parent = null;
    private final int TEMP_BUFFER = 8192;
	
    /**
     * ユーティリティクラスの使用準備
     *  @param object MIDletクラス
     */
	public MidpResourceFileUtils(MIDlet object)
    {
        parent = object;
    }

    /**
     *  jadファイルで指定されている値を取得する
     * 
     * @param key jadファイルに書かれているキーワード
     * @return プロパティの値
     */	
	public String getAppProperty(String key)
	{
    	// jadファイルでディレクトリの場所が指定されているか？
    	return (parent.getAppProperty(key));
	}

    /**
     *  リソースにあるテキストデータを一括で読み出す
     * 
     * @param name リソーステキストファイル名
     * @return     読み込んだテキスト文字列
     */
    public String getResourceText(String name)
    {
        String resourceData = null;
		try
		{
		    Class c = this.getClass();
		    InputStream is = c.getResourceAsStream(name);
			byte[] data = new byte[TEMP_BUFFER];
			int actual = is.read(data, 0, TEMP_BUFFER);
			resourceData = new String(data, 0, actual);
			is.close();
			data = null;
		}
		catch (Exception ex)
		{
            //
			resourceData = ex.getMessage();
		}
		return (resourceData);
    }
	
    /**
     *  端末の内部ファイルを指定していた場合に指定パスを調整する
     *  <br>
     *  (WX320Kで「ブラウザで開く」を実行する場合に必要！)
     * 
     * @param orgName  通常のファイル場所
     * @return 変換後のファイル保管場所
     */
	static public String convertInternalFileLocation(String orgName)
	{
        // 変換対象か？
		if (orgName.indexOf("file:///") == 0)
        {
            // 変換対象でないため、そのまま折り返す
            return (orgName);
		}

        // プラットフォームを確認する
		String platForm = System.getProperty("microedition.platform");
        String newName = orgName;
		if (platForm.indexOf("WX320K") >= 0)
        {
            // WX320Kの時には、ディレクトリ位置を補正する。
			//  (8は、'file:///' の文字列を切り取るため)
            newName = "file://localhost/D:/" + orgName.substring(8);
        }
		return (newName);
    }
}
