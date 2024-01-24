import jp.sourceforge.nnsi.a2b.frameworks.MidpMain;

/**
 *  MIDPのメインクラスです<br>
 *  （オブジェクトの生成のみを行います）<br>
 * 
 * @author MRSa
 *
 */
public class fwSampleMain extends MidpMain
{
    /**
     *  スーパークラスのコンストラクタを呼び出します<br>
     *  (引数にファクトリクラスを生成して渡します)
     *
     */
    public fwSampleMain()
    {
        // スーパークラスのコンストラクタ呼び出し
        super(new fwSampleFactory());
    }
}
