package jp.sourceforge.nnsi.a2b.forms;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;

/**
 * データ入力用のストレージ<br>
 * (インタフェースクラス）
 * 
 * @author MRSa
 *
 */
public interface IDataInputStorage
{
    /** 前の画面に戻る */
    static public final int SCENE_TO_PREVIOUS = -1;

    /**
     *  データ入力がキャンセルされたときの処理
     */
    public abstract void cancelEntry();

    /**
     *  データ入力がOKされたときの処理
     */
    public abstract void dataEntry();

    /**
     *  データ入力のための準備処理
     */
    public abstract void prepare();

    /**
     *  データ入力終了処理
     */
    public abstract void finish();

    /**
     *  表示するデータ入力フィールドの個数を取得する
     *  
     *  @return 表示するデータ入力フィールドの個数
     */
    public abstract int getNumberOfDataInputField();

    /**
     *  データ入力フィールドを取得する
     *  @param index データインデックス番号（０スタート）
     *  @return テキスト入力フィールド
     */
    public abstract TextField getDataInputField(int index);

    /**
     *  表示するオプション(選択入力項目)の個数を取得する
     *  @return 表示するオプションの個数 
     */
    public abstract int getNumberOfOptionGroup();
    
    /**
     *  表示するオプション(選択入力項目)を取得する
     *  @param index オプションインデックス番号（０スタート）
     *  @return オプション(選択入力項目)
     */
    public abstract ChoiceGroup getOptionField(int index);

    /**
     *  画面切り替え先を取得する
     *  @param screenId 画面ID
     *  @return 切り替える画面ID
     */
    public abstract int nextSceneToChange(int screenId);

    /**
     *  画面切替先のタイトルを取得する
     *  @param screenId 画面ID
     *  @return 画面タイトル
     */
    public abstract String nextSceneTitleToChange(int screenId);
}
