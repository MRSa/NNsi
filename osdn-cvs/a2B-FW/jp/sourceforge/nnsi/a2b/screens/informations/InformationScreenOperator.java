package jp.sourceforge.nnsi.a2b.screens.informations;

import java.util.Vector;

import javax.microedition.lcdui.Font;


/**
 *  情報表示画面の画面操作クラスです<br>
 *  画面内に収まるよう、表示データを１行ごとに加工します。<br>
 * 
 * @author MRSa
 */
public class InformationScreenOperator
{
    static public final int WIDTH_MARGIN = 3;
    static public final int HEIGHT_MARGIN = 0;
	private IInformationScreenStorage storage = null;
    private Vector                    msgLines = null;
    private boolean                  buildingData = false;

    /**
     * コンストラクタは特になにも実行しません。
     */
    public InformationScreenOperator(IInformationScreenStorage object)
    {
        storage = object;
    }

    /**
     *  クラスの実行準備も、特に行いません
     */
    public void prepare()
    {
    }

    /**
     * メッセージをウィンドウ内に収まるように加工します
     * 
     * @param width ウィンドウ幅
     * @param height ウィンドウ高さ
     * @param screenFont 表示するフォント
     * @param folding ウィンドウ幅に収めるよう整形(true) / １行は１行として整形(false)
     * @return フォーマット成功(true)、フォーマット失敗(false)
     */
    public boolean formatMessage(int width, int height, Font screenFont, boolean folding)
    {
    	String message = storage.getInformationMessageToShow();
    	if (message == null)
    	{
            // 表示すべきメッセージがなかった...
    		return (false);
    	}
//    	int maxLine = height / (screenFont.getHeight() + 1);
        divideLine(width, screenFont, message + "\n", folding);
    	System.gc();
        return (true);
    }

    /**
     * 表示行単位にデータを分割する
     * 
     * @param width
     * @param screenFont
     * @param message
     * @param folding 行端でおりたたみ処理を行う / 行わない
     * @return 表示するメッセージの行数
     */
    private int divideLine(int width, Font screenFont, String message, boolean folding)
	{
    	if (buildingData == true)
    	{
    		return (0);
    	}
    	buildingData = true;
    	
        msgLines = null;
    	msgLines = new Vector();
    	
        int startPos = 0;
        int endPos = message.length();
        int msgIndex = 1;
    	try
    	{
            if (folding == true)
    		{
                // 画面の端で折り返す場合の処理...
            	for (msgIndex = 1; msgIndex < endPos; msgIndex++)
                {
                	char drawChar = message.charAt(msgIndex);
                	if (drawChar == '\n')
                	{
                        msgLines.addElement(message.substring(startPos, msgIndex));
                        startPos = msgIndex + 1;
                	}
                	else if (screenFont.substringWidth(message, startPos, (msgIndex - startPos)) > (width - WIDTH_MARGIN))
                	{
                        msgLines.addElement(message.substring(startPos, msgIndex - 1));
                        startPos = msgIndex - 1;
                	}
                	if (startPos > endPos)
                	{
                		break;
                	}
                }
            }
            else
            {
                // 行端で折り返し処理をしない場合... (行単位でデータを分割)
            	msgIndex = message.indexOf("\n", startPos);
                while (msgIndex >= 0)
                {
                	String lineData = message.substring(startPos, msgIndex);
                	msgLines.addElement(lineData);
                	startPos = msgIndex + 1;
                	msgIndex = message.indexOf("\n", startPos);
                }
            }
    	}
        catch(Exception ex)
        {
//        	String msg = ex.getMessage();
        }
    	buildingData = false;
        return (msgLines.size());
	}

    /**
     * 表示するメッセージの行数を取得する
     * 
     * @return 表示するメッセージの行数
     */
    public int getMessageLine()
    {
        return (msgLines.size());
    }
    
    /**
     *  メッセージデータの取得
     *  
     *  @param lineNumber 行番号（０スタート）
     *  @return 表示する行データ
     *  
     */
    public String getMessage(int lineNumber)
    {
        if ((lineNumber < 0)||(lineNumber >= msgLines.size()))
        {
            return ("");
        }
    	return ((String) msgLines.elementAt(lineNumber));
    }

    /**
     *  表示するボタン数を応答する
     *  @param screenId 画面ID
     *  @return 表示するボタン数
     */
    public int getNumberOfButtons(int screenId)
    {
        return (storage.getNumberOfButtons(screenId));
    }

    /**
     *  画面表示の終了
     *  
     *  @param  buttonId 終了時に押されたボタン
     */
    public void finishScreen(int buttonId)
    {
    	msgLines = null;
    	storage.setButtonId(buttonId);
    	System.gc();
        return;
    }

    /**
     *  画面に出すボタンラベルを応答する
     * 
     * @param buttonId  出したいボタンのID
     * @return 画面に表示するボタンラベル
     */
    public String getButtonLabel(int buttonId)
    {
    	return (storage.getInformationButtonLabel(buttonId));
    }
    
    /**
     *   次の画面をどこに切り替えるか決める
     * 
     * @param sceneId 現在の画面ID
     * @return 次の画面ID
     */
    public int nextSceneToChange(int sceneId)
    {
        return (storage.nextSceneToChange(sceneId));
    }
}
