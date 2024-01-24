package jp.sourceforge.nnsi.a2b.frameworks.samples;
import jp.sourceforge.nnsi.a2b.frameworks.MidpMain;

/**
 *  MIDPのメインクラスです<br>
 *  （オブジェクトの生成のみを行います）<br>
 * 
 * @author MRSa
 *
 */
public class MidpDefaultMain extends MidpMain
{
    /**
     *  スーパークラスのコンストラクタを呼び出します<br>
     *  (引数にファクトリクラスを生成して渡します)
     *
     */
    public MidpDefaultMain()
    {
        // スーパークラスのコンストラクタ呼び出し
        super(new MidpDefaultFactory());
    }
}
