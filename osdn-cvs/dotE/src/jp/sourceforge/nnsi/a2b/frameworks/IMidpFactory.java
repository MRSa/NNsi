package jp.sourceforge.nnsi.a2b.frameworks;

import javax.microedition.midlet.MIDlet;

/**
 *  a2B frameworkで使用するクラスを指定します
 * 
 * @author MRSa
 *
 */
public interface IMidpFactory
{
    /**
     *  データストレージクラスを取得する
     *  @return データストレージクラス 
     */
    public abstract IMidpDataStorage getDataStorage();

    /**
     *   画面切替クラスを取得する
     *   @return 画面切替クラス
     */
    public abstract IMidpSceneDB getSceneDB();
  
    /**
     *  コアオブジェクト（データストレージクラス、画面切替クラス）を生成する
     *  
     */
    public abstract void prepareCoreObjects();

    /**
     *  その他のオブジェクトの生成
     *  
     *  @param object   MIDletクラス
     *  @param selector シーンセレクタ
     */
    public abstract void prepareObjects(MIDlet object, MidpSceneSelector selector);
}
