package jp.sourceforge.nnsi.a2b.utilities;

/**
 *  NNsiから持ってきた、HTML簡易パーサ<br>
 *  htmlのタグを消す程度の簡単な文字列加工を行います。<br>
 *  
 * @author MRSa
 *
 */
/*
 *  (一応、2chのdatファイル表示用の整形機能も持っています。)
 */
public class MidpNNsiHtmlParser
{
	/** データがSHIFT JISコード */
	static public final  int KANJI_SJIS = 0;

	/** データがUTF8コード */
	static public final  int KANJI_UTF8 = 1;

	/** データがEUCコード */
	static public final  int KANJI_EUC  = 2;
	
	static public  final  int RECOMMEND_PARSESIZE = 10240;  // 一応のオススメパースサイズ
	static private final  int MINIMUM_TEMPORARY   = 32768;  // 最小確保一時領域
	static private final int PARSING_2CH    = -1;	       // 2ちゃんねるスレ解析モード
    static private final int PARSING_NORMAL = 0;	           // 通常(HTML)解析モード

    private MidpKanjiConverter kanjiConverter = null; // 漢字コード変換クラス
    private int messageParsingMode   = PARSING_2CH;   // メッセージ解析モード
    private boolean convertHanzen   = false;         // 半角カナ → 全角カナ変換を行う？

    /**
     *  半角カナ → 全角カナ変換テーブル
     *  
     */
	private final short[] Han2ZenTable = {
      // "。", "「", "」", "、", "・", "ヲ",
      (short) 0x8142, (short) 0x8175, (short) 0x8176, 
      (short) 0x8141, (short) 0x8145, (short) 0x8392,
      
      // "ァ", "ィ", "ゥ", "ェ", "ォ", "ャ",
      (short) 0x8340, (short) 0x8342, (short) 0x8344, 
      (short) 0x8346, (short) 0x8348, (short) 0x8383,

      // "ュ", "ョ", "ッ", "ー", "ア", "イ",
      (short) 0x8385, (short) 0x8387, (short) 0x8362, 
      (short) 0x815b, (short) 0x8341, (short) 0x8343,

      // "ウ", "エ", "オ", "カ", "キ", "ク",
      (short) 0x8345, (short) 0x8347, (short) 0x8349, 
      (short) 0x834a, (short) 0x834c, (short) 0x834e,

      // "ケ", "コ", "サ", "シ", "ス", "セ",
      (short) 0x8350, (short) 0x8352, (short) 0x8354, 
      (short) 0x8356, (short) 0x8358, (short) 0x835a,

      // "ソ\", "タ", "チ", "ツ", "テ", "ト",
      (short) 0x835c, (short) 0x835e, (short) 0x8360, 
      (short) 0x8363, (short) 0x8365, (short) 0x8367,

      // "ナ", "ニ", "ヌ", "ネ", "ノ", "ハ",
      (short) 0x8369, (short) 0x836a, (short) 0x836b, 
      (short) 0x836c, (short) 0x836d, (short) 0x836e,

      // "ヒ", "フ", "ヘ", "ホ", "マ", "ミ",
      (short) 0x8371, (short) 0x8374, (short) 0x8377, 
      (short) 0x837a, (short) 0x837d, (short) 0x837e,

      // "ム", "メ", "モ", "ヤ", "ユ", "ヨ",
      (short) 0x8380, (short) 0x8381, (short) 0x8382, 
      (short) 0x8384, (short) 0x8386, (short) 0x8388,

      // "ラ", "リ", "ル", "レ", "ロ", "ワ",
      (short) 0x8389, (short) 0x838a, (short) 0x838b, 
      (short) 0x838c, (short) 0x838d, (short) 0x838f,

      // "ン", "゛", "゜", "",   "",   ""
      (short) 0x8393, (short) 0x814a, (short) 0x814b, 
      (short) 0x0000, (short) 0x0000, (short) 0x0000
    };

    /**
     *  漢字コード変換クラスを記憶します
     * 
     *  @param converter 漢字コード変換クラス
     *  @see MidpKanjiConverter
     */
	public MidpNNsiHtmlParser(MidpKanjiConverter converter)
	{
        kanjiConverter = converter;
	}

    /**
     *  HTMLの簡易的解析処理<br>
     *  パース処理のためにワーク領域として32kB(or 2倍)の一時領域を確保します<br>
     *  <br>
     *  <BR>
     *  <B>※ 利用上の注意 ※</B><BR>
     *  速度的な問題も出るので、あまり大きなサイズを変換しないで...。<BR>
     * 
     * @param source 変換元データ
     * @param offset 変換を開始するデータの先頭位置
     * @param size   変換するデータのデータ数
     * @param kanjiCode 変換元データの漢字コード(KANJI_SJIS/KANJI_UTF8/KANJI_EUC)
     * @return 変換結果の文字列(string型)
     */
	public String parseHtml(byte[] source, int offset, int size, int kanjiCode)
    {
        messageParsingMode = PARSING_NORMAL;
        int allocateSize = size + size;
        if (allocateSize < MINIMUM_TEMPORARY)
        {
        	allocateSize = MINIMUM_TEMPORARY;
        }
        byte[] convData = new byte[allocateSize];
        String  data = null;
        try
        {
            parseMessage(convData, source, offset, size);
            if (kanjiCode == KANJI_SJIS)
            {
                // SJIS -> UTF8 変換する
            	data = kanjiConverter.ParseFromSJ(convData);
            }
            else if (kanjiCode == KANJI_EUC)
            {
                // EUC -> UTF8 変換する
            	data = kanjiConverter.ParseFromEUC(convData, 0, convData.length);
            }
            else // if (kanjiCode == KANJI_UTF8)
            {
                // 漢字コードの変換は行わない
            	data = new String(convData);
            }
            convData = null;
        }
        catch (Exception ex)
        {
            //
        }
        return (data);
	}
	
    /*=========================================================================*/
	/*   Function : ParseMessage　　                                           */
	/*                                       (メッセージパース処理...NNsiより) */
	/*=========================================================================*/
	private void parseMessage(byte[] dst, byte[] src, int offset, int size)
	{
		int dataStatus = 0;
		int dstIndex  = 0;
		int srcIndex  = offset;
//        int previousIndex = 0;
		
		// 超遅い解釈ルーチンかも... (１文字づつパースする)
	    while (srcIndex < (offset + size))
	    {
	        if (src[srcIndex] == '&')
	        {
	            // "&gt;" を '>' に置換
	            if ((src[srcIndex + 1]== 'g')&&(src[srcIndex + 2] == 't')&&(src[srcIndex + 3] == ';'))
	            {
	                dst[dstIndex] = '>';
					dstIndex++;
					srcIndex = srcIndex + 4;
	                continue;
	            }

				// "&lt;" を '<' に置換
	            if ((src[srcIndex + 1]== 'l')&&(src[srcIndex + 2] == 't')&&(src[srcIndex + 3] == ';'))
	            {
	                dst[dstIndex] = '<';
					dstIndex++;
					srcIndex = srcIndex + 4;
	                continue;
	            }

				// "&quot;" を '"' に置換
	            if ((src[srcIndex + 1] == 'q')&&(src[srcIndex + 2] == 'u')&&(src[srcIndex + 3] == 'o')&&
					(src[srcIndex + 4] == 't')&&(src[srcIndex + 5] == ';'))
	            {
	                dst[dstIndex] = '"';
					dstIndex++;
					srcIndex = srcIndex + 6;
	                continue;
	            }

				// "&nbsp;" を ' ' に置換 (AA表示モードのために、スペース1個にするよう変更)
	            if ((src[srcIndex + 1] == 'n')&&(src[srcIndex + 2] == 'b')&&(src[srcIndex + 3] == 's')&&
						(src[srcIndex + 4] == 'p')&&(src[srcIndex + 5] == ';'))
		        {
		                dst[dstIndex] = ' ';
						dstIndex++;
						srcIndex = srcIndex + 6;
		                continue;
		        }

				// "&amp;" を '&' に置換
	            if ((src[srcIndex + 1] == 'a')&&(src[srcIndex + 2] == 'm')&&(src[srcIndex + 3] == 'p')&&
					(src[srcIndex + 4] == ';'))
	            {
	                dst[dstIndex] = '&';
					dstIndex++;
					srcIndex = srcIndex + 5;
	                continue;
	            }

				// これはありえないはずだが...一応。
                dst[dstIndex] = '&';
				dstIndex++;
				srcIndex++;
	            continue;
	        }
	        if (src[srcIndex] == '<')
	        {
	            //  "<>" は、セパレータ(状態によって変わる)
	            if (src[srcIndex + 1] == '>')
	            {
    				srcIndex = srcIndex + 2;
	    			if (messageParsingMode == PARSING_NORMAL)
	    			{
	    				// HTML解析モードは、スペース１つに変換する...
			            dst[dstIndex] = ' ';
						dstIndex++;
						continue;
	    			}
	                switch (dataStatus)
	                {
	                  case 0:
	                    // 名前欄の区切り
                        //nameField = kanjiConverter.ParseFromSJ(dst, previousIndex, (dstIndex - previousIndex));
			            dst[dstIndex] = ' ';
						dstIndex++;
			            dst[dstIndex] = ' ';
						dstIndex++;
	                    dataStatus = 1;
//						previousIndex = dstIndex;
	                    break;

	                  case 1:
	                    // e-mail欄の区切り
                        //emailField = kanjiConverter.ParseFromSJ(dst, previousIndex, (dstIndex - previousIndex));
				        dst[dstIndex] = '\r';
						dstIndex++;
				        dst[dstIndex] = '\n';
						dstIndex++;
	                    dataStatus = 2;
//						previousIndex = dstIndex;
	                    break;

	                  case 2:
	                    // 年月日・時間およびＩＤ欄の区切り
	                    //dateTimeField = kanjiConverte.ParseFromSJr(dst, previousIndex, (dstIndex - previousIndex));
					    //dst[dstIndex] = '\n';
						//dstIndex++;
					    dst[dstIndex] = '\r';
						dstIndex++;
					    dst[dstIndex] = '\n';
					    dstIndex++;
						dataStatus = 3;
//						previousIndex = dstIndex;
						
						srcIndex++;  // ← レスの先頭に入っているスペースを除去する
	                    break;

	                  case 3:
	                  default:
	                    // メッセージの終端
			            //messageField = new String(dst, previousIndex, (dstIndex - previousIndex));
					    dst[dstIndex] = '\0';
						dstIndex++;
						//previousIndex = dstIndex;
	                    return;
	                    //break;
	                }
	                continue;
	            }

	            //  "<br>" は、改行に置換
	            if (((src[srcIndex + 1] == 'b')||(src[srcIndex + 1] == 'B'))&&
					((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))&&
					(src[srcIndex + 3] == '>'))
	            {
	                // 行末と行頭のスペースを削ってみる場合
					if ((srcIndex != 0)&&((srcIndex -1) == ' '))
	                {
	                    dstIndex--;
	                }
	                if (src[srcIndex + 4] == ' ')
	                {
				        dst[dstIndex] = '\r';
						dstIndex++;
	                	dst[dstIndex] = '\n';
	                	dstIndex++;
	                	srcIndex = srcIndex + 5;
	                    continue;
	                }
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                srcIndex = srcIndex + 4;
	                continue;
	            }

	            //  "<p*>" は、改行2つに置換
	            if ((src[srcIndex + 1] == 'p')||(src[srcIndex + 1] == 'P'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                srcIndex = srcIndex + 2;
	                while (src[srcIndex] != '>')
	                {
	                    srcIndex++;
	                    if (srcIndex >= (offset + size))
	                    {
	                        // 確保した領域を越えた...
	                        break;
	                    }
	                }
	                srcIndex++;
	                continue;
	            }

	            // <li>タグを改行コードと - に置換する
	            if (((src[srcIndex + 1] == 'l')||(src[srcIndex + 1] == 'L'))&&
					((src[srcIndex + 2] == 'i')||(src[srcIndex + 2] == 'I'))&&
					(src[srcIndex + 3] == '>'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                dst[dstIndex] = '-';
	                dstIndex++;
	                dst[dstIndex] = ' ';
	                dstIndex++;
	                srcIndex = srcIndex + 4;
	                continue;
	            }
	               
	            //  "<hr>" は、改行 === 改行 に置換
	            if (((src[srcIndex + 1] == 'h')||(src[srcIndex + 1] == 'H'))&&
					((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))&&
					(src[srcIndex + 3] == '>'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                dst[dstIndex] = '=';
	                dstIndex++;
	                dst[dstIndex] = '=';
	                dstIndex++;
	                dst[dstIndex] = '=';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                srcIndex = srcIndex + 4;
	                continue;
	            }

	            //  "<dt>" は、改行に置換
	            if (((src[srcIndex + 1] == 'd')||(src[srcIndex + 1] == 'D'))&&
					((src[srcIndex + 2] == 't')||(src[srcIndex + 2] == 'T'))&&
					(src[srcIndex + 3] == '>'))
	            {
	                if (dst[dstIndex - 1] != '\n')
	                {
				        dst[dstIndex] = '\r';
						dstIndex++;
	                    dst[dstIndex] = '\n';
	                    dstIndex++;
	                }
	                srcIndex = srcIndex + 4;
	                continue;
	            }

	            //  "</tr>" および "</td>" は、改行に置換
	            if (((src[srcIndex + 2] == 't')||(src[srcIndex + 2] == 'T'))&&
					((src[srcIndex + 3] == 'r')||(src[srcIndex + 3] == 'R'))||
					((src[srcIndex + 3] == 'd')||(src[srcIndex + 3] == 'D'))&&
					(src[srcIndex + 1] == '/')&&(src[srcIndex + 4] == '>'))
	            {
	                if (dst[dstIndex - 1] != '\n')
	                {
				        dst[dstIndex] = '\r';
						dstIndex++;
	                    dst[dstIndex] = '\n';
	                    dstIndex++;
	                }
	                srcIndex = srcIndex + 5;
	                continue;
	            }

	            //  "<dd>" は、改行と空白2つに置換
	            if (((src[srcIndex + 1] == 'd')||(src[srcIndex + 1] == 'D'))&&
					((src[srcIndex + 2] == 'd')||(src[srcIndex + 2] == 'D'))&&
					(src[srcIndex + 3] == '>'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                dst[dstIndex] = ' ';
	                dstIndex++;
	                dst[dstIndex] = ' ';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                srcIndex = srcIndex + 4;
	                continue;
	            }
	            
	            //  "<h*>" は、改行 + 改行 に置換
	            if ((src[srcIndex + 1] == 'h')||(src[srcIndex + 1] == 'H'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;

	                // TODO: 強調色の設定...

	                // タグは読み飛ばす
	                srcIndex = srcIndex + 2;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            //  "</h*>" は、改行 + 改行 に置換
	            if ((src[srcIndex + 1] == '/')&&((src[srcIndex + 2] == 'h')||(src[srcIndex + 2] == 'H')))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;

	                // TODO: 強調色の解除...

	                // タグは読み飛ばす
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            //  "<a*>" で、アンカー色の検出
	            if (((src[srcIndex + 1] == 'a')||(src[srcIndex + 1] == 'A'))&&(src[srcIndex + 2] == ' '))
	            {
	                // TODO: アンカー色の設定...

	                // タグは読み飛ばす
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            //  "</a*>" で、アンカー色の解除
	            if (((src[srcIndex + 2] == 'a')||(src[srcIndex + 2] == 'A'))&&(src[srcIndex + 1] == '/'))
	            {
	                // TODO: アンカー色の解除

	                // タグは読み飛ばす
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
 	                continue;
	            }

	            // <EM* は強調色へ
	            if (((src[srcIndex + 1] == 'e')||(src[srcIndex + 1] == 'E'))&&
					((src[srcIndex + 2] == 'm')||(src[srcIndex + 2] == 'M')))
	            {
	                // TODO: 強調色へ切り替え

	                // タグは読み飛ばす
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            // </em* は通常色へ
	            if (((src[srcIndex + 2] == 'e')||(src[srcIndex + 2] == 'E'))&&
					((src[srcIndex + 3] == 'm')||(src[srcIndex + 3] == 'M'))&&
					(src[srcIndex + 1] == '/'))
	            {
	                // TODO: 強調色の解除

	                // タグは読み飛ばす
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            // <tr>, <td> はスペース１つに変換
	            if (((src[srcIndex + 1] == 't')||(src[srcIndex + 1] == 'T'))&&
					((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))||
					((src[srcIndex + 2] == 'd')||(src[srcIndex + 2] == 'D')))
	            {
	                dst[dstIndex] = ' ';
	                dstIndex++;

	                // タグは読み飛ばす
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            // <BE: は、BE IDのため読み飛ばさない。(画面表示する)
	            if (((src[srcIndex + 1] == 'b')||(src[srcIndex + 1] == 'B'))&&
					((src[srcIndex + 2] == 'e')||(src[srcIndex + 2] == 'E'))&&
					(src[srcIndex + 3] == ':'))
	            {
	                dst[dstIndex] = ' ';
	                dstIndex++;
	                srcIndex++;

	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
	                {
	                	dst[dstIndex] = src[srcIndex];
	                	dstIndex++;
	                	srcIndex++;
	                }
	                srcIndex++;
	                continue;
	            }

	            // その他のタグは読み飛ばす
	            while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                {
	                srcIndex++;
	            }
	            // タグは完全無視とする
	            //dst[dstIndex] = ' ';
	            //dstIndex++;
				srcIndex++;
	            continue;
	        }

	        // 半角カナ→全角カナ変換処理を実施する場合...
			if ((convertHanzen == true)&&(src[srcIndex] < 0))
			{
                int checkChar = 256 + src[srcIndex];
				if ((checkChar >= 0x81)&&(checkChar <= 0x9f))
				{
					// 2バイト文字と判定
					dst[dstIndex] = src[srcIndex];
					dstIndex++;
					srcIndex++;

					dst[dstIndex] = src[srcIndex];
					dstIndex++;
					srcIndex++;
					continue;
				}

				if ((checkChar >= 0xe0)&&(checkChar <= 0xef))
				{
					// 2バイト文字と判定
					dst[dstIndex] = src[srcIndex];
					dstIndex++;
					srcIndex++;

					dst[dstIndex] = src[srcIndex];
					dstIndex++;
					srcIndex++;
					continue;
				}

				if ((checkChar >= 0xa1)&&(checkChar <= 0xdf))
				{
	                // カタカナ文字と判定、テーブルを使って半角カナ→全角変換する
					short code = Han2ZenTable[checkChar - 0xa1];
					byte temp;

					temp = (byte) ((code & 0xff00) >> 8);
					dst[dstIndex] = temp;
					dstIndex++;

					temp = (byte) ((code & 0x00ff));
					dst[dstIndex] = temp;
					dstIndex++;
										
					srcIndex++;
	                continue;
	            }
	        }

			if ((messageParsingMode != PARSING_NORMAL)&&((src[srcIndex] == 0x0d)||(src[srcIndex] == 0x0a)))
			{
				// テキスト表示モードのときには、改行コードを普通の一文字転写する。
	            srcIndex++;
	            dst[dstIndex] = '\r';
				dstIndex++;
	            dst[dstIndex] = '\n';
				dstIndex++;
				continue;
			}
			
	        // スペースが連続していた場合、１つに減らす
			if ((src[srcIndex] == ' ')&&(src[srcIndex + 1] == ' '))
	        {
	            srcIndex++;
	            while ((src[srcIndex] == ' ')&&(srcIndex < (offset + size)))
	            {
	                srcIndex++;
	            }
	            srcIndex--;
	        }

			// NULL および 0x0a, 0x0d は無視する
	        if ((src[srcIndex] != '\0')&&(src[srcIndex] != 0x0a)&&(src[srcIndex] != 0x0d)&&(src[srcIndex] != 0x09))
	        {
	            // 普通の一文字転写
	            dst[dstIndex] = src[srcIndex];
				dstIndex++;
	        }
	        srcIndex++;
	    }
        //  レスの終端...
		//messageField = new String(dst, previousIndex, (dstIndex - previousIndex));
		dst[dstIndex] = '\0';
		dstIndex++;
	    return;
	}
}
