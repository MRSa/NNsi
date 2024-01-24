/**
 * 
 *  Shift JIS(byte列) から UTF8(String型)に変換するクラス
 *  
 *    Willcom機など、SystemでShift JIS -> UTF8変換ができる場合には、その機能を利用する。
 *    Nokia機など、Shift JIS -> UTF8変換がサポートされていない場合には、本クラスが持つ変換テーブルをする。
 *    
 *    ※注※
 *     Nokia 6630(Vodafone 702NK)は Systemで EUC-JP -> UTF8の変換ができるが、この機能は
 *    メモリリークを起こしている模様なので、本クラスが持つ変換テーブルで変換を行う。
 *    
 *    ※注※
 *     本クラスが持つ変換テーブルを利用した場合には、半角カナは文字化けを起こす。
 * 
 * @author MRSa (mrsa@users.sourceforge.jp)
 *
 */
public class a2Butf8Conveter
{
    private final   int MARGIN   = 8;        // バッファの余裕
    private boolean useSystemSJ  = false;   // Systemが Shift JISからUTF8へ変換できるかどうか
    private boolean useSystemEUC = false;   // Systemが EUC-JPからUTF8へ変換できるかどうか

/**
    // 半角カナ/全角カナ変換テーブル (SJIS)
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
**/
/**/
    // 半角カナ変換テーブル (UTF8)
    private final short[] Han2ZenTable = {
      // "。", "「", "」", "、", "・", "ヲ",
      (short) 0xff61, (short) 0xff62, (short) 0xff63, 
      (short) 0xff64, (short) 0xff65, (short) 0xff66,
      
      // "ァ", "ィ", "ゥ", "ェ", "ォ", "ャ",
      (short) 0xff67, (short) 0xff68, (short) 0xff69, 
      (short) 0xff6a, (short) 0xff6b, (short) 0xff6c,

      // "ュ", "ョ", "ッ", "ー", "ア", "イ",
      (short) 0xff6d, (short) 0xff6e, (short) 0xff6f, 
      (short) 0xff70, (short) 0xff71, (short) 0xff72,

      // "ウ", "エ", "オ", "カ", "キ", "ク",
      (short) 0xff73, (short) 0xff74, (short) 0xff75, 
      (short) 0xff76, (short) 0xff77, (short) 0xff78,

      // "ケ", "コ", "サ", "シ", "ス", "セ",
      (short) 0xff79, (short) 0xff7a, (short) 0xff7b, 
      (short) 0xff7c, (short) 0xff7d, (short) 0xff7e,

      // "ソ\", "タ", "チ", "ツ", "テ", "ト",
      (short) 0xff7f, (short) 0xff80, (short) 0xff81, 
      (short) 0xff82, (short) 0xff83, (short) 0xff84,

      // "ナ", "ニ", "ヌ", "ネ", "ノ", "ハ",
      (short) 0xff85, (short) 0xff86, (short) 0xff87, 
      (short) 0xff88, (short) 0xff89, (short) 0xff8a,

      // "ヒ", "フ", "ヘ", "ホ", "マ", "ミ",
      (short) 0xff8b, (short) 0xff8c, (short) 0xff8d, 
      (short) 0xff8e, (short) 0xff8f, (short) 0xff90,

      // "ム", "メ", "モ", "ヤ", "ユ", "ヨ",
      (short) 0xff91, (short) 0xff92, (short) 0xff93, 
      (short) 0xff94, (short) 0xff95, (short) 0xff96,

      // "ラ", "リ", "ル", "レ", "ロ", "ワ",
      (short) 0xff97, (short) 0xff98, (short) 0xff99, 
      (short) 0xff9a, (short) 0xff9b, (short) 0xff9c,

      // "ン", "゛", "゜", "",   "",   ""
      (short) 0xff9d, (short) 0xff9e, (short) 0xff9f, 
      (short) 0x0000, (short) 0x0000, (short) 0x0000
    };
/**/
/**
    // 半角カナ/全角カナ変換テーブル (UTF8)
    private final short[] Han2ZenTable = {
      // "。", "「", "」", "、", "・", "ヲ",
      (short) 0x3002, (short) 0x3001, (short) 0x300c, 
      (short) 0x300d, (short) 0x30fb, (short) 0x30f2,
      
      // "ァ", "ィ", "ゥ", "ェ", "ォ", "ャ",
      (short) 0x30a1, (short) 0x30a3, (short) 0x30a5, 
      (short) 0x30a7, (short) 0x30a9, (short) 0x30e3,

      // "ュ", "ョ", "ッ", "ー", "ア", "イ",
      (short) 0x30e5, (short) 0x30e7, (short) 0x30c3, 
      (short) 0x30fc, (short) 0x30a2, (short) 0x30a4,

      // "ウ", "エ", "オ", "カ", "キ", "ク",
      (short) 0x30a6, (short) 0x30a8, (short) 0x30aa, 
      (short) 0x30ab, (short) 0x30ad, (short) 0x30af,

      // "ケ", "コ", "サ", "シ", "ス", "セ",
      (short) 0x30b1, (short) 0x30b3, (short) 0x30b5, 
      (short) 0x30b7, (short) 0x30b9, (short) 0x30bb,

      // "ソ\", "タ", "チ", "ツ", "テ", "ト",
      (short) 0x30bd, (short) 0x30bf, (short) 0x30c1, 
      (short) 0x30c4, (short) 0x30c6, (short) 0x30c8,

      // "ナ", "ニ", "ヌ", "ネ", "ノ", "ハ",
      (short) 0x30ca, (short) 0x30cb, (short) 0x30cc, 
      (short) 0x30cd, (short) 0x30ce, (short) 0x30cf,

      // "ヒ", "フ", "ヘ", "ホ", "マ", "ミ",
      (short) 0x30d2, (short) 0x30d5, (short) 0x30d8, 
      (short) 0x30db, (short) 0x30de, (short) 0x30df,

      // "ム", "メ", "モ", "ヤ", "ユ", "ヨ",
      (short) 0x30e0, (short) 0x30e1, (short) 0x30e2, 
      (short) 0x30e4, (short) 0x30e6, (short) 0x30e8,

      // "ラ", "リ", "ル", "レ", "ロ", "ワ",
      (short) 0x30e9, (short) 0x30ea, (short) 0x30eb, 
      (short) 0x30ec, (short) 0x30ed, (short) 0x30ef,

      // "ン", "゛", "゜", "",   "",   ""
      (short) 0x30f3, (short) 0x309b, (short) 0x309c, 
      (short) 0x0000, (short) 0x0000, (short) 0x0000
    };
**/
    // JIS から UTF8 に変換するテーブル...
    private final int[] Jis2Utf8Table = {
        (int) 0x30003001, (int) 0x3002ff0c, 
        (int) 0xff0e30fb, (int) 0xff1aff1b, 
        (int) 0xff1fff01, (int) 0x309b309c, 
        (int) 0x00b4ff40, (int) 0x00a8ff3e, 
        (int) 0xffe3ff3f, (int) 0x30fd30fe, 
        (int) 0x309d309e, (int) 0x30034edd, 
        (int) 0x30053006, (int) 0x300730fc, 
        (int) 0x20152010, (int) 0xff0f005c, 
        (int) 0x301c2016, (int) 0xff5c2026, 
        (int) 0x20252018, (int) 0x2019201c, 
        (int) 0x201dff08, (int) 0xff093014, 
        (int) 0x3015ff3b, (int) 0xff3dff5b, 
        (int) 0xff5d3008, (int) 0x3009300a, 
        (int) 0x300b300c, (int) 0x300d300e, 
        (int) 0x300f3010, (int) 0x3011ff0b, 
        (int) 0x221200b1, (int) 0x00d700f7, 
        (int) 0xff1d2260, (int) 0xff1cff1e, 
        (int) 0x22662267, (int) 0x221e2234, 
        (int) 0x26422640, (int) 0x00b02032, 
        (int) 0x20332103, (int) 0xffe5ff04, 
        (int) 0x00a200a3, (int) 0xff05ff03, 
        (int) 0xff06ff0a, (int) 0xff2000a7, 
        (int) 0x26062605, (int) 0x25cb25cf, 
        (int) 0x25ce25c7, (int) 0x25c625a1, 
        (int) 0x25a025b3, (int) 0x25b225bd, 
        (int) 0x25bc203b, (int) 0x30122192, 
        (int) 0x21902191, (int) 0x21933013, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00002208, 
        (int) 0x220b2286, (int) 0x22872282, 
        (int) 0x2283222a, (int) 0x22290000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00002227, 
        (int) 0x222800ac, (int) 0x21d221d4, 
        (int) 0x22002203, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00002220, (int) 0x22a52312, 
        (int) 0x22022207, (int) 0x22612252, 
        (int) 0x226a226b, (int) 0x221a223d, 
        (int) 0x221d2235, (int) 0x222b222c, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x0000212b, 
        (int) 0x2030266f, (int) 0x266d266a, 
        (int) 0x20202021, (int) 0x00b60000, 
        (int) 0x00000000, (int) 0x000025ef, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x0000ff10, 
        (int) 0xff11ff12, (int) 0xff13ff14, 
        (int) 0xff15ff16, (int) 0xff17ff18, 
        (int) 0xff190000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0xff21ff22, (int) 0xff23ff24, 
        (int) 0xff25ff26, (int) 0xff27ff28, 
        (int) 0xff29ff2a, (int) 0xff2bff2c, 
        (int) 0xff2dff2e, (int) 0xff2fff30, 
        (int) 0xff31ff32, (int) 0xff33ff34, 
        (int) 0xff35ff36, (int) 0xff37ff38, 
        (int) 0xff39ff3a, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0xff41ff42, (int) 0xff43ff44, 
        (int) 0xff45ff46, (int) 0xff47ff48, 
        (int) 0xff49ff4a, (int) 0xff4bff4c, 
        (int) 0xff4dff4e, (int) 0xff4fff50, 
        (int) 0xff51ff52, (int) 0xff53ff54, 
        (int) 0xff55ff56, (int) 0xff57ff58, 
        (int) 0xff59ff5a, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x30413042, 
        (int) 0x30433044, (int) 0x30453046, 
        (int) 0x30473048, (int) 0x3049304a, 
        (int) 0x304b304c, (int) 0x304d304e, 
        (int) 0x304f3050, (int) 0x30513052, 
        (int) 0x30533054, (int) 0x30553056, 
        (int) 0x30573058, (int) 0x3059305a, 
        (int) 0x305b305c, (int) 0x305d305e, 
        (int) 0x305f3060, (int) 0x30613062, 
        (int) 0x30633064, (int) 0x30653066, 
        (int) 0x30673068, (int) 0x3069306a, 
        (int) 0x306b306c, (int) 0x306d306e, 
        (int) 0x306f3070, (int) 0x30713072, 
        (int) 0x30733074, (int) 0x30753076, 
        (int) 0x30773078, (int) 0x3079307a, 
        (int) 0x307b307c, (int) 0x307d307e, 
        (int) 0x307f3080, (int) 0x30813082, 
        (int) 0x30833084, (int) 0x30853086, 
        (int) 0x30873088, (int) 0x3089308a, 
        (int) 0x308b308c, (int) 0x308d308e, 
        (int) 0x308f3090, (int) 0x30913092, 
        (int) 0x30930000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x30a130a2, (int) 0x30a330a4, 
        (int) 0x30a530a6, (int) 0x30a730a8, 
        (int) 0x30a930aa, (int) 0x30ab30ac, 
        (int) 0x30ad30ae, (int) 0x30af30b0, 
        (int) 0x30b130b2, (int) 0x30b330b4, 
        (int) 0x30b530b6, (int) 0x30b730b8, 
        (int) 0x30b930ba, (int) 0x30bb30bc, 
        (int) 0x30bd30be, (int) 0x30bf30c0, 
        (int) 0x30c130c2, (int) 0x30c330c4, 
        (int) 0x30c530c6, (int) 0x30c730c8, 
        (int) 0x30c930ca, (int) 0x30cb30cc, 
        (int) 0x30cd30ce, (int) 0x30cf30d0, 
        (int) 0x30d130d2, (int) 0x30d330d4, 
        (int) 0x30d530d6, (int) 0x30d730d8, 
        (int) 0x30d930da, (int) 0x30db30dc, 
        (int) 0x30dd30de, (int) 0x30df30e0, 
        (int) 0x30e130e2, (int) 0x30e330e4, 
        (int) 0x30e530e6, (int) 0x30e730e8, 
        (int) 0x30e930ea, (int) 0x30eb30ec, 
        (int) 0x30ed30ee, (int) 0x30ef30f0, 
        (int) 0x30f130f2, (int) 0x30f330f4, 
        (int) 0x30f530f6, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x03910392, 
        (int) 0x03930394, (int) 0x03950396, 
        (int) 0x03970398, (int) 0x0399039a, 
        (int) 0x039b039c, (int) 0x039d039e, 
        (int) 0x039f03a0, (int) 0x03a103a3, 
        (int) 0x03a403a5, (int) 0x03a603a7, 
        (int) 0x03a803a9, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x03b103b2, 
        (int) 0x03b303b4, (int) 0x03b503b6, 
        (int) 0x03b703b8, (int) 0x03b903ba, 
        (int) 0x03bb03bc, (int) 0x03bd03be, 
        (int) 0x03bf03c0, (int) 0x03c103c3, 
        (int) 0x03c403c5, (int) 0x03c603c7, 
        (int) 0x03c803c9, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x04100411, (int) 0x04120413, 
        (int) 0x04140415, (int) 0x04010416, 
        (int) 0x04170418, (int) 0x0419041a, 
        (int) 0x041b041c, (int) 0x041d041e, 
        (int) 0x041f0420, (int) 0x04210422, 
        (int) 0x04230424, (int) 0x04250426, 
        (int) 0x04270428, (int) 0x0429042a, 
        (int) 0x042b042c, (int) 0x042d042e, 
        (int) 0x042f0000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x04300431, (int) 0x04320433, 
        (int) 0x04340435, (int) 0x04510436, 
        (int) 0x04370438, (int) 0x0439043a, 
        (int) 0x043b043c, (int) 0x043d043e, 
        (int) 0x043f0440, (int) 0x04410442, 
        (int) 0x04430444, (int) 0x04450446, 
        (int) 0x04470448, (int) 0x0449044a, 
        (int) 0x044b044c, (int) 0x044d044e, 
        (int) 0x044f0000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x25002502, 
        (int) 0x250c2510, (int) 0x25182514, 
        (int) 0x251c252c, (int) 0x25242534, 
        (int) 0x253c2501, (int) 0x2503250f, 
        (int) 0x2513251b, (int) 0x25172523, 
        (int) 0x2533252b, (int) 0x253b254b, 
        (int) 0x2520252f, (int) 0x25282537, 
        (int) 0x253f251d, (int) 0x25302525, 
        (int) 0x25382542, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x4e9c5516, 
        (int) 0x5a03963f, (int) 0x54c0611b, 
        (int) 0x632859f6, (int) 0x90228475, 
        (int) 0x831c7a50, (int) 0x60aa63e1, 
        (int) 0x6e2565ed, (int) 0x846682a6, 
        (int) 0x9bf56893, (int) 0x572765a1, 
        (int) 0x62715b9b, (int) 0x59d0867b, 
        (int) 0x98f47d62, (int) 0x7dbe9b8e, 
        (int) 0x62167c9f, (int) 0x88b75b89, 
        (int) 0x5eb56309, (int) 0x66976848, 
        (int) 0x95c7978d, (int) 0x674f4ee5, 
        (int) 0x4f0a4f4d, (int) 0x4f9d5049, 
        (int) 0x56f25937, (int) 0x59d45a01, 
        (int) 0x5c0960df, (int) 0x610f6170, 
        (int) 0x66136905, (int) 0x70ba754f, 
        (int) 0x757079fb, (int) 0x7dad7def, 
        (int) 0x80c3840e, (int) 0x88638b02, 
        (int) 0x9055907a, (int) 0x533b4e95, 
        (int) 0x4ea557df, (int) 0x80b290c1, 
        (int) 0x78ef4e00, (int) 0x58f16ea2, 
        (int) 0x90387a32, (int) 0x8328828b, 
        (int) 0x9c2f5141, (int) 0x537054bd, 
        (int) 0x54e156e0, (int) 0x59fb5f15, 
        (int) 0x98f26deb, (int) 0x80e4852d, 
        (int) 0x96629670, (int) 0x96a097fb, 
        (int) 0x540b53f3, (int) 0x5b8770cf, 
        (int) 0x7fbd8fc2, (int) 0x96e8536f, 
        (int) 0x9d5c7aba, (int) 0x4e117893, 
        (int) 0x81fc6e26, (int) 0x56185504, 
        (int) 0x6b1d851a, (int) 0x9c3b59e5, 
        (int) 0x53a96d66, (int) 0x74dc958f, 
        (int) 0x56424e91, (int) 0x904b96f2, 
        (int) 0x834f990c, (int) 0x53e155b6, 
        (int) 0x5b305f71, (int) 0x662066f3, 
        (int) 0x68046c38, (int) 0x6cf36d29, 
        (int) 0x745b76c8, (int) 0x7a4e9834, 
        (int) 0x82f1885b, (int) 0x8a6092ed, 
        (int) 0x6db275ab, (int) 0x76ca99c5, 
        (int) 0x60a68b01, (int) 0x8d8a95b2, 
        (int) 0x698e53ad, (int) 0x51865712, 
        (int) 0x58305944, (int) 0x5bb45ef6, 
        (int) 0x602863a9, (int) 0x63f46cbf, 
        (int) 0x6f14708e, (int) 0x71147159, 
        (int) 0x71d5733f, (int) 0x7e018276, 
        (int) 0x82d18597, (int) 0x9060925b, 
        (int) 0x9d1b5869, (int) 0x65bc6c5a, 
        (int) 0x752551f9, (int) 0x592e5965, 
        (int) 0x5f805fdc, (int) 0x62bc65fa, 
        (int) 0x6a2a6b27, (int) 0x6bb4738b, 
        (int) 0x7fc18956, (int) 0x9d2c9d0e, 
        (int) 0x9ec45ca1, (int) 0x6c96837b, 
        (int) 0x51045c4b, (int) 0x61b681c6, 
        (int) 0x68767261, (int) 0x4e594ffa, 
        (int) 0x53786069, (int) 0x6e297a4f, 
        (int) 0x97f34e0b, (int) 0x53164eee, 
        (int) 0x4f554f3d, (int) 0x4fa14f73, 
        (int) 0x52a053ef, (int) 0x5609590f, 
        (int) 0x5ac15bb6, (int) 0x5be179d1, 
        (int) 0x6687679c, (int) 0x67b66b4c, 
        (int) 0x6cb3706b, (int) 0x73c2798d, 
        (int) 0x79be7a3c, (int) 0x7b8782b1, 
        (int) 0x82db8304, (int) 0x837783ef, 
        (int) 0x83d38766, (int) 0x8ab25629, 
        (int) 0x8ca88fe6, (int) 0x904e971e, 
        (int) 0x868a4fc4, (int) 0x5ce86211, 
        (int) 0x7259753b, (int) 0x81e582bd, 
        (int) 0x86fe8cc0, (int) 0x96c59913, 
        (int) 0x99d54ecb, (int) 0x4f1a89e3, 
        (int) 0x56de584a, (int) 0x58ca5efb, 
        (int) 0x5feb602a, (int) 0x60946062, 
        (int) 0x61d06212, (int) 0x62d06539, 
        (int) 0x9b416666, (int) 0x68b06d77, 
        (int) 0x7070754c, (int) 0x76867d75, 
        (int) 0x82a587f9, (int) 0x958b968e, 
        (int) 0x8c9d51f1, (int) 0x52be5916, 
        (int) 0x54b35bb3, (int) 0x5d166168, 
        (int) 0x69826daf, (int) 0x788d84cb, 
        (int) 0x88578a72, (int) 0x93a79ab8, 
        (int) 0x6d6c99a8, (int) 0x86d957a3, 
        (int) 0x67ff86ce, (int) 0x920e5283, 
        (int) 0x56875404, (int) 0x5ed362e1, 
        (int) 0x64b9683c, (int) 0x68386bbb, 
        (int) 0x737278ba, (int) 0x7a6b899a, 
        (int) 0x89d28d6b, (int) 0x8f0390ed, 
        (int) 0x95a39694, (int) 0x97695b66, 
        (int) 0x5cb3697d, (int) 0x984d984e, 
        (int) 0x639b7b20, (int) 0x6a2b6a7f, 
        (int) 0x68b69c0d, (int) 0x6f5f5272, 
        (int) 0x559d6070, (int) 0x62ec6d3b, 
        (int) 0x6e076ed1, (int) 0x845b8910, 
        (int) 0x8f444e14, (int) 0x9c3953f6, 
        (int) 0x691b6a3a, (int) 0x9784682a, 
        (int) 0x515c7ac3, (int) 0x84b291dc, 
        (int) 0x938c565b, (int) 0x9d286822, 
        (int) 0x83058431, (int) 0x7ca55208, 
        (int) 0x82c574e6, (int) 0x4e7e4f83, 
        (int) 0x51a05bd2, (int) 0x520a52d8, 
        (int) 0x52e75dfb, (int) 0x559a582a, 
        (int) 0x59e65b8c, (int) 0x5b985bdb, 
        (int) 0x5e725e79, (int) 0x60a3611f, 
        (int) 0x616361be, (int) 0x63db6562, 
        (int) 0x67d16853, (int) 0x68fa6b3e, 
        (int) 0x6b536c57, (int) 0x6f226f97, 
        (int) 0x6f4574b0, (int) 0x751876e3, 
        (int) 0x770b7aff, (int) 0x7ba17c21, 
        (int) 0x7de97f36, (int) 0x7ff0809d, 
        (int) 0x8266839e, (int) 0x89b38acc, 
        (int) 0x8cab9084, (int) 0x94519593, 
        (int) 0x959195a2, (int) 0x966597d3, 
        (int) 0x99288218, (int) 0x4e38542b, 
        (int) 0x5cb85dcc, (int) 0x73a9764c, 
        (int) 0x773c5ca9, (int) 0x7feb8d0b, 
        (int) 0x96c19811, (int) 0x98549858, 
        (int) 0x4f014f0e, (int) 0x5371559c, 
        (int) 0x566857fa, (int) 0x59475b09, 
        (int) 0x5bc45c90, (int) 0x5e0c5e7e, 
        (int) 0x5fcc63ee, (int) 0x673a65d7, 
        (int) 0x65e2671f, (int) 0x68cb68c4, 
        (int) 0x6a5f5e30, (int) 0x6bc56c17, 
        (int) 0x6c7d757f, (int) 0x79485b63, 
        (int) 0x7a007d00, (int) 0x5fbd898f, 
        (int) 0x8a188cb4, (int) 0x8d778ecc, 
        (int) 0x8f1d98e2, (int) 0x9a0e9b3c, 
        (int) 0x4e80507d, (int) 0x51005993, 
        (int) 0x5b9c622f, (int) 0x628064ec, 
        (int) 0x6b3a72a0, (int) 0x75917947, 
        (int) 0x7fa987fb, (int) 0x8abc8b70, 
        (int) 0x63ac83ca, (int) 0x97a05409, 
        (int) 0x540355ab, (int) 0x68546a58, 
        (int) 0x8a707827, (int) 0x67759ecd, 
        (int) 0x53745ba2, (int) 0x811a8650, 
        (int) 0x90064e18, (int) 0x4e454ec7, 
        (int) 0x4f1153ca, (int) 0x54385bae, 
        (int) 0x5f136025, (int) 0x6551673d, 
        (int) 0x6c426c72, (int) 0x6ce37078, 
        (int) 0x74037a76, (int) 0x7aae7b08, 
        (int) 0x7d1a7cfe, (int) 0x7d6665e7, 
        (int) 0x725b53bb, (int) 0x5c455de8, 
        (int) 0x62d262e0, (int) 0x63196e20, 
        (int) 0x865a8a31, (int) 0x8ddd92f8, 
        (int) 0x6f0179a6, (int) 0x9b5a4ea8, 
        (int) 0x4eab4eac, (int) 0x4f9b4fa0, 
        (int) 0x50d15147, (int) 0x7af65171, 
        (int) 0x51f65354, (int) 0x5321537f, 
        (int) 0x53eb55ac, (int) 0x58835ce1, 
        (int) 0x5f375f4a, (int) 0x602f6050, 
        (int) 0x606d631f, (int) 0x65596a4b, 
        (int) 0x6cc172c2, (int) 0x72ed77ef, 
        (int) 0x80f88105, (int) 0x8208854e, 
        (int) 0x90f793e1, (int) 0x97ff9957, 
        (int) 0x9a5a4ef0, (int) 0x51dd5c2d, 
        (int) 0x6681696d, (int) 0x5c4066f2, 
        (int) 0x69757389, (int) 0x68507c81, 
        (int) 0x50c552e4, (int) 0x57475dfe, 
        (int) 0x932665a4, (int) 0x6b236b3d, 
        (int) 0x74347981, (int) 0x79bd7b4b, 
        (int) 0x7dca82b9, (int) 0x83cc887f, 
        (int) 0x895f8b39, (int) 0x8fd191d1, 
        (int) 0x541f9280, (int) 0x4e5d5036, 
        (int) 0x53e5533a, (int) 0x72d77396, 
        (int) 0x77e982e6, (int) 0x8eaf99c6, 
        (int) 0x99c899d2, (int) 0x5177611a, 
        (int) 0x865e55b0, (int) 0x7a7a5076, 
        (int) 0x5bd39047, (int) 0x96854e32, 
        (int) 0x6adb91e7, (int) 0x5c515c48, 
        (int) 0x63987a9f, (int) 0x6c939774, 
        (int) 0x8f617aaa, (int) 0x718a9688, 
        (int) 0x7c826817, (int) 0x7e706851, 
        (int) 0x936c52f2, (int) 0x541b85ab, 
        (int) 0x8a137fa4, (int) 0x8ecd90e1, 
        (int) 0x53668888, (int) 0x79414fc2, 
        (int) 0x50be5211, (int) 0x51445553, 
        (int) 0x572d73ea, (int) 0x578b5951, 
        (int) 0x5f625f84, (int) 0x60756176, 
        (int) 0x616761a9, (int) 0x63b2643a, 
        (int) 0x656c666f, (int) 0x68426e13, 
        (int) 0x75667a3d, (int) 0x7cfb7d4c, 
        (int) 0x7d997e4b, (int) 0x7f6b830e, 
        (int) 0x834a86cd, (int) 0x8a088a63, 
        (int) 0x8b668efd, (int) 0x981a9d8f, 
        (int) 0x82b88fce, (int) 0x9be85287, 
        (int) 0x621f6483, (int) 0x6fc09699, 
        (int) 0x68415091, (int) 0x6b206c7a, 
        (int) 0x6f547a74, (int) 0x7d508840, 
        (int) 0x8a236708, (int) 0x4ef65039, 
        (int) 0x50265065, (int) 0x517c5238, 
        (int) 0x526355a7, (int) 0x570f5805, 
        (int) 0x5acc5efa, (int) 0x61b261f8, 
        (int) 0x62f36372, (int) 0x691c6a29, 
        (int) 0x727d72ac, (int) 0x732e7814, 
        (int) 0x786f7d79, (int) 0x770c80a9, 
        (int) 0x898b8b19, (int) 0x8ce28ed2, 
        (int) 0x90639375, (int) 0x967a9855, 
        (int) 0x9a139e78, (int) 0x5143539f, 
        (int) 0x53b35e7b, (int) 0x5f266e1b, 
        (int) 0x6e907384, (int) 0x73fe7d43, 
        (int) 0x82378a00, (int) 0x8afa9650, 
        (int) 0x4e4e500b, (int) 0x53e4547c, 
        (int) 0x56fa59d1, (int) 0x5b645df1, 
        (int) 0x5eab5f27, (int) 0x62386545, 
        (int) 0x67af6e56, (int) 0x72d07cca, 
        (int) 0x88b480a1, (int) 0x80e183f0, 
        (int) 0x864e8a87, (int) 0x8de89237, 
        (int) 0x96c79867, (int) 0x9f134e94, 
        (int) 0x4e924f0d, (int) 0x53485449, 
        (int) 0x543e5a2f, (int) 0x5f8c5fa1, 
        (int) 0x609f68a7, (int) 0x6a8e745a, 
        (int) 0x78818a9e, (int) 0x8aa48b77, 
        (int) 0x91904e5e, (int) 0x9bc94ea4, 
        (int) 0x4f7c4faf, (int) 0x50195016, 
        (int) 0x5149516c, (int) 0x529f52b9, 
        (int) 0x52fe539a, (int) 0x53e35411, 
        (int) 0x540e5589, (int) 0x575157a2, 
        (int) 0x597d5b54, (int) 0x5b5d5b8f, 
        (int) 0x5de55de7, (int) 0x5df75e78, 
        (int) 0x5e835e9a, (int) 0x5eb75f18, 
        (int) 0x6052614c, (int) 0x629762d8, 
        (int) 0x63a7653b, (int) 0x66026643, 
        (int) 0x66f4676d, (int) 0x68216897, 
        (int) 0x69cb6c5f, (int) 0x6d2a6d69, 
        (int) 0x6e2f6e9d, (int) 0x75327687, 
        (int) 0x786c7a3f, (int) 0x7ce07d05, 
        (int) 0x7d187d5e, (int) 0x7db18015, 
        (int) 0x800380af, (int) 0x80b18154, 
        (int) 0x818f822a, (int) 0x8352884c, 
        (int) 0x88618b1b, (int) 0x8ca28cfc, 
        (int) 0x90ca9175, (int) 0x9271783f, 
        (int) 0x92fc95a4, (int) 0x964d9805, 
        (int) 0x99999ad8, (int) 0x9d3b525b, 
        (int) 0x52ab53f7, (int) 0x540858d5, 
        (int) 0x62f76fe0, (int) 0x8c6a8f5f, 
        (int) 0x9eb9514b, (int) 0x523b544a, 
        (int) 0x56fd7a40, (int) 0x91779d60, 
        (int) 0x9ed27344, (int) 0x6f098170, 
        (int) 0x75115ffd, (int) 0x60da9aa8, 
        (int) 0x72db8fbc, (int) 0x6b649803, 
        (int) 0x4eca56f0, (int) 0x576458be, 
        (int) 0x5a5a6068, (int) 0x61c7660f, 
        (int) 0x66066839, (int) 0x68b16df7, 
        (int) 0x75d57d3a, (int) 0x826e9b42, 
        (int) 0x4e9b4f50, (int) 0x53c95506, 
        (int) 0x5d6f5de6, (int) 0x5dee67fb, 
        (int) 0x6c997473, (int) 0x78028a50, 
        (int) 0x939688df, (int) 0x57505ea7, 
        (int) 0x632b50b5, (int) 0x50ac518d, 
        (int) 0x670054c9, (int) 0x585e59bb, 
        (int) 0x5bb05f69, (int) 0x624d63a1, 
        (int) 0x683d6b73, (int) 0x6e08707d, 
        (int) 0x91c77280, (int) 0x78157826, 
        (int) 0x796d658e, (int) 0x7d3083dc, 
        (int) 0x88c18f09, (int) 0x969b5264, 
        (int) 0x57286750, (int) 0x7f6a8ca1, 
        (int) 0x51b45742, (int) 0x962a583a, 
        (int) 0x698a80b4, (int) 0x54b25d0e, 
        (int) 0x57fc7895, (int) 0x9dfa4f5c, 
        (int) 0x524a548b, (int) 0x643e6628, 
        (int) 0x671467f5, (int) 0x7a847b56, 
        (int) 0x7d22932f, (int) 0x685c9bad, 
        (int) 0x7b395319, (int) 0x518a5237, 
        (int) 0x5bdf62f6, (int) 0x64ae64e6, 
        (int) 0x672d6bba, (int) 0x85a996d1, 
        (int) 0x76909bd6, (int) 0x634c9306, 
        (int) 0x9bab76bf, (int) 0x66524e09, 
        (int) 0x509853c2, (int) 0x5c7160e8, 
        (int) 0x64926563, (int) 0x685f71e6, 
        (int) 0x73ca7523, (int) 0x7b977e82, 
        (int) 0x86958b83, (int) 0x8cdb9178, 
        (int) 0x991065ac, (int) 0x66ab6b8b, 
        (int) 0x4ed54ed4, (int) 0x4f3a4f7f, 
        (int) 0x523a53f8, (int) 0x53f255e3, 
        (int) 0x56db58eb, (int) 0x59cb59c9, 
        (int) 0x59ff5b50, (int) 0x5c4d5e02, 
        (int) 0x5e2b5fd7, (int) 0x601d6307, 
        (int) 0x652f5b5c, (int) 0x65af65bd, 
        (int) 0x65e8679d, (int) 0x6b626b7b, 
        (int) 0x6c0f7345, (int) 0x794979c1, 
        (int) 0x7cf87d19, (int) 0x7d2b80a2, 
        (int) 0x810281f3, (int) 0x89968a5e, 
        (int) 0x8a698a66, (int) 0x8a8c8aee, 
        (int) 0x8cc78cdc, (int) 0x96cc98fc, 
        (int) 0x6b6f4e8b, (int) 0x4f3c4f8d, 
        (int) 0x51505b57, (int) 0x5bfa6148, 
        (int) 0x63016642, (int) 0x6b216ecb, 
        (int) 0x6cbb723e, (int) 0x74bd75d4, 
        (int) 0x78c1793a, (int) 0x800c8033, 
        (int) 0x81ea8494, (int) 0x8f9e6c50, 
        (int) 0x9e7f5f0f, (int) 0x8b589d2b, 
        (int) 0x7afa8ef8, (int) 0x5b8d96eb, 
        (int) 0x4e0353f1, (int) 0x57f75931, 
        (int) 0x5ac95ba4, (int) 0x60896e7f, 
        (int) 0x6f0675be, (int) 0x8cea5b9f, 
        (int) 0x85007be0, (int) 0x507267f4, 
        (int) 0x829d5c61, (int) 0x854a7e1e, 
        (int) 0x820e5199, (int) 0x5c046368, 
        (int) 0x8d66659c, (int) 0x716e793e, 
        (int) 0x7d178005, (int) 0x8b1d8eca, 
        (int) 0x906e86c7, (int) 0x90aa501f, 
        (int) 0x52fa5c3a, (int) 0x6753707c, 
        (int) 0x7235914c, (int) 0x91c8932b, 
        (int) 0x82e55bc2, (int) 0x5f3160f9, 
        (int) 0x4e3b53d6, (int) 0x5b88624b, 
        (int) 0x67316b8a, (int) 0x72e973e0, 
        (int) 0x7a2e816b, (int) 0x8da39152, 
        (int) 0x99965112, (int) 0x53d7546a, 
        (int) 0x5bff6388, (int) 0x6a397dac, 
        (int) 0x970056da, (int) 0x53ce5468, 
        (int) 0x5b975c31, (int) 0x5dde4fee, 
        (int) 0x610162fe, (int) 0x6d3279c0, 
        (int) 0x79cb7d42, (int) 0x7e4d7fd2, 
        (int) 0x81ed821f, (int) 0x84908846, 
        (int) 0x89728b90, (int) 0x8e748f2f, 
        (int) 0x9031914b, (int) 0x916c96c6, 
        (int) 0x919c4ec0, (int) 0x4f4f5145, 
        (int) 0x53415f93, (int) 0x620e67d4, 
        (int) 0x6c416e0b, (int) 0x73637e26, 
        (int) 0x91cd9283, (int) 0x53d45919, 
        (int) 0x5bbf6dd1, (int) 0x795d7e2e, 
        (int) 0x7c9b587e, (int) 0x719f51fa, 
        (int) 0x88538ff0, (int) 0x4fca5cfb, 
        (int) 0x662577ac, (int) 0x7ae3821c, 
        (int) 0x99ff51c6, (int) 0x5faa65ec, 
        (int) 0x696f6b89, (int) 0x6df36e96, 
        (int) 0x6f6476fe, (int) 0x7d145de1, 
        (int) 0x90759187, (int) 0x980651e6, 
        (int) 0x521d6240, (int) 0x669166d9, 
        (int) 0x6e1a5eb6, (int) 0x7dd27f72, 
        (int) 0x66f885af, (int) 0x85f78af8, 
        (int) 0x52a953d9, (int) 0x59735e8f, 
        (int) 0x5f906055, (int) 0x92e49664, 
        (int) 0x50b7511f, (int) 0x52dd5320, 
        (int) 0x534753ec, (int) 0x54e85546, 
        (int) 0x55315617, (int) 0x596859be, 
        (int) 0x5a3c5bb5, (int) 0x5c065c0f, 
        (int) 0x5c115c1a, (int) 0x5e845e8a, 
        (int) 0x5ee05f70, (int) 0x627f6284, 
        (int) 0x62db638c, (int) 0x63776607, 
        (int) 0x660c662d, (int) 0x6676677e, 
        (int) 0x68a26a1f, (int) 0x6a356cbc, 
        (int) 0x6d886e09, (int) 0x6e58713c, 
        (int) 0x71267167, (int) 0x75c77701, 
        (int) 0x785d7901, (int) 0x796579f0, 
        (int) 0x7ae07b11, (int) 0x7ca77d39, 
        (int) 0x809683d6, (int) 0x848b8549, 
        (int) 0x885d88f3, (int) 0x8a1f8a3c, 
        (int) 0x8a548a73, (int) 0x8c618cde, 
        (int) 0x91a49266, (int) 0x937e9418, 
        (int) 0x969c9798, (int) 0x4e0a4e08, 
        (int) 0x4e1e4e57, (int) 0x51975270, 
        (int) 0x57ce5834, (int) 0x58cc5b22, 
        (int) 0x5e3860c5, (int) 0x64fe6761, 
        (int) 0x67566d44, (int) 0x72b67573, 
        (int) 0x7a6384b8, (int) 0x8b7291b8, 
        (int) 0x93205631, (int) 0x57f498fe, 
        (int) 0x62ed690d, (int) 0x6b9671ed, 
        (int) 0x7e548077, (int) 0x827289e6, 
        (int) 0x98df8755, (int) 0x8fb15c3b, 
        (int) 0x4f384fe1, (int) 0x4fb55507, 
        (int) 0x5a205bdd, (int) 0x5be95fc3, 
        (int) 0x614e632f, (int) 0x65b0664b, 
        (int) 0x68ee699b, (int) 0x6d786df1, 
        (int) 0x753375b9, (int) 0x771f795e, 
        (int) 0x79e67d33, (int) 0x81e382af, 
        (int) 0x85aa89aa, (int) 0x8a3a8eab, 
        (int) 0x8f9b9032, (int) 0x91dd9707, 
        (int) 0x4eba4ec1, (int) 0x52035875, 
        (int) 0x58ec5c0b, (int) 0x751a5c3d, 
        (int) 0x814e8a0a, (int) 0x8fc59663, 
        (int) 0x976d7b25, (int) 0x8acf9808, 
        (int) 0x916256f3, (int) 0x53a89017, 
        (int) 0x54395782, (int) 0x5e2563a8, 
        (int) 0x6c34708a, (int) 0x77617c8b, 
        (int) 0x7fe08870, (int) 0x90429154, 
        (int) 0x93109318, (int) 0x968f745e, 
        (int) 0x9ac45d07, (int) 0x5d696570, 
        (int) 0x67a28da8, (int) 0x96db636e, 
        (int) 0x67496919, (int) 0x83c59817, 
        (int) 0x96c088fe, (int) 0x6f84647a, 
        (int) 0x5bf84e16, (int) 0x702c755d, 
        (int) 0x662f51c4, (int) 0x523652e2, 
        (int) 0x59d35f81, (int) 0x60276210, 
        (int) 0x653f6574, (int) 0x661f6674, 
        (int) 0x68f26816, (int) 0x6b636e05, 
        (int) 0x7272751f, (int) 0x76db7cbe, 
        (int) 0x805658f0, (int) 0x88fd897f, 
        (int) 0x8aa08a93, (int) 0x8acb901d, 
        (int) 0x91929752, (int) 0x97596589, 
        (int) 0x7a0e8106, (int) 0x96bb5e2d, 
        (int) 0x60dc621a, (int) 0x65a56614, 
        (int) 0x679077f3, (int) 0x7a4d7c4d, 
        (int) 0x7e3e810a, (int) 0x8cac8d64, 
        (int) 0x8de18e5f, (int) 0x78a95207, 
        (int) 0x62d963a5, (int) 0x64426298, 
        (int) 0x8a2d7a83, (int) 0x7bc08aac, 
        (int) 0x96ea7d76, (int) 0x820c8749, 
        (int) 0x4ed95148, (int) 0x53435360, 
        (int) 0x5ba35c02, (int) 0x5c165ddd, 
        (int) 0x62266247, (int) 0x64b06813, 
        (int) 0x68346cc9, (int) 0x6d456d17, 
        (int) 0x67d36f5c, (int) 0x714e717d, 
        (int) 0x65cb7a7f, (int) 0x7bad7dda, 
        (int) 0x7e4a7fa8, (int) 0x817a821b, 
        (int) 0x823985a6, (int) 0x8a6e8cce, 
        (int) 0x8df59078, (int) 0x907792ad, 
        (int) 0x92919583, (int) 0x9bae524d, 
        (int) 0x55846f38, (int) 0x71365168, 
        (int) 0x79857e55, (int) 0x81b37cce, 
        (int) 0x564c5851, (int) 0x5ca863aa, 
        (int) 0x66fe66fd, (int) 0x695a72d9, 
        (int) 0x758f758e, (int) 0x790e7956, 
        (int) 0x79df7c97, (int) 0x7d207d44, 
        (int) 0x86078a34, (int) 0x963b9061, 
        (int) 0x9f2050e7, (int) 0x527553cc, 
        (int) 0x53e25009, (int) 0x55aa58ee, 
        (int) 0x594f723d, (int) 0x5b8b5c64, 
        (int) 0x531d60e3, (int) 0x60f3635c, 
        (int) 0x6383633f, (int) 0x63bb64cd, 
        (int) 0x65e966f9, (int) 0x5de369cd, 
        (int) 0x69fd6f15, (int) 0x71e54e89, 
        (int) 0x75e976f8, (int) 0x7a937cdf, 
        (int) 0x7dcf7d9c, (int) 0x80618349, 
        (int) 0x8358846c, (int) 0x84bc85fb, 
        (int) 0x88c58d70, (int) 0x9001906d, 
        (int) 0x9397971c, (int) 0x9a1250cf, 
        (int) 0x5897618e, (int) 0x81d38535, 
        (int) 0x8d089020, (int) 0x4fc35074, 
        (int) 0x52475373, (int) 0x606f6349, 
        (int) 0x675f6e2c, (int) 0x8db3901f, 
        (int) 0x4fd75c5e, (int) 0x8cca65cf, 
        (int) 0x7d9a5352, (int) 0x88965176, 
        (int) 0x63c35b58, (int) 0x5b6b5c0a, 
        (int) 0x640d6751, (int) 0x905c4ed6, 
        (int) 0x591a592a, (int) 0x6c708a51, 
        (int) 0x553e5815, (int) 0x59a560f0, 
        (int) 0x625367c1, (int) 0x82356955, 
        (int) 0x964099c4, (int) 0x9a284f53, 
        (int) 0x58065bfe, (int) 0x80105cb1, 
        (int) 0x5e2f5f85, (int) 0x6020614b, 
        (int) 0x623466ff, (int) 0x6cf06ede, 
        (int) 0x80ce817f, (int) 0x82d4888b, 
        (int) 0x8cb89000, (int) 0x902e968a, 
        (int) 0x9edb9bdb, (int) 0x4ee353f0, 
        (int) 0x59277b2c, (int) 0x918d984c, 
        (int) 0x9df96edd, (int) 0x70275353, 
        (int) 0x55445b85, (int) 0x6258629e, 
        (int) 0x62d36ca2, (int) 0x6fef7422, 
        (int) 0x8a179438, (int) 0x6fc18afe, 
        (int) 0x833851e7, (int) 0x86f853ea, 
        (int) 0x53e94f46, (int) 0x90548fb0, 
        (int) 0x596a8131, (int) 0x5dfd7aea, 
        (int) 0x8fbf68da, (int) 0x8c3772f8, 
        (int) 0x9c486a3d, (int) 0x8ab04e39, 
        (int) 0x53585606, (int) 0x576662c5, 
        (int) 0x63a265e6, (int) 0x6b4e6de1, 
        (int) 0x6e5b70ad, (int) 0x77ed7aef, 
        (int) 0x7baa7dbb, (int) 0x803d80c6, 
        (int) 0x86cb8a95, (int) 0x935b56e3, 
        (int) 0x58c75f3e, (int) 0x65ad6696, 
        (int) 0x6a806bb5, (int) 0x75378ac7, 
        (int) 0x502477e5, (int) 0x57305f1b, 
        (int) 0x6065667a, (int) 0x6c6075f4, 
        (int) 0x7a1a7f6e, (int) 0x81f48718, 
        (int) 0x904599b3, (int) 0x7bc9755c, 
        (int) 0x7af97b51, (int) 0x84c49010, 
        (int) 0x79e97a92, (int) 0x83365ae1, 
        (int) 0x77404e2d, (int) 0x4ef25b99, 
        (int) 0x5fe062bd, (int) 0x663c67f1, 
        (int) 0x6ce8866b, (int) 0x88778a3b, 
        (int) 0x914e92f3, (int) 0x99d06a17, 
        (int) 0x7026732a, (int) 0x82e78457, 
        (int) 0x8caf4e01, (int) 0x514651cb, 
        (int) 0x558b5bf5, (int) 0x5e165e33, 
        (int) 0x5e815f14, (int) 0x5f355f6b, 
        (int) 0x5fb461f2, (int) 0x631166a2, 
        (int) 0x671d6f6e, (int) 0x7252753a, 
        (int) 0x773a8074, (int) 0x81398178, 
        (int) 0x87768abf, (int) 0x8adc8d85, 
        (int) 0x8df3929a, (int) 0x95779802, 
        (int) 0x9ce552c5, (int) 0x635776f4, 
        (int) 0x67156c88, (int) 0x73cd8cc3, 
        (int) 0x93ae9673, (int) 0x6d25589c, 
        (int) 0x690e69cc, (int) 0x8ffd939a, 
        (int) 0x75db901a, (int) 0x585a6802, 
        (int) 0x63b469fb, (int) 0x4f436f2c, 
        (int) 0x67d88fbb, (int) 0x85267db4, 
        (int) 0x9354693f, (int) 0x6f70576a, 
        (int) 0x58f75b2c, (int) 0x7d2c722a, 
        (int) 0x540a91e3, (int) 0x9db44ead, 
        (int) 0x4f4e505c, (int) 0x50755243, 
        (int) 0x8c9e5448, (int) 0x58245b9a, 
        (int) 0x5e1d5e95, (int) 0x5ead5ef7, 
        (int) 0x5f1f608c, (int) 0x62b5633a, 
        (int) 0x63d068af, (int) 0x6c407887, 
        (int) 0x798e7a0b, (int) 0x7de08247, 
        (int) 0x8a028ae6, (int) 0x8e449013, 
        (int) 0x90b8912d, (int) 0x91d89f0e, 
        (int) 0x6ce56458, (int) 0x64e26575, 
        (int) 0x6ef47684, (int) 0x7b1b9069, 
        (int) 0x93d16eba, (int) 0x54f25fb9, 
        (int) 0x64a48f4d, (int) 0x8fed9244, 
        (int) 0x5178586b, (int) 0x59295c55, 
        (int) 0x5e976dfb, (int) 0x7e8f751c, 
        (int) 0x8cbc8ee2, (int) 0x985b70b9, 
        (int) 0x4f1d6bbf, (int) 0x6fb17530, 
        (int) 0x96fb514e, (int) 0x54105835, 
        (int) 0x585759ac, (int) 0x5c605f92, 
        (int) 0x6597675c, (int) 0x6e21767b, 
        (int) 0x83df8ced, (int) 0x901490fd, 
        (int) 0x934d7825, (int) 0x783a52aa, 
        (int) 0x5ea6571f, (int) 0x59746012, 
        (int) 0x5012515a, (int) 0x51ac51cd, 
        (int) 0x52005510, (int) 0x58545858, 
        (int) 0x59575b95, (int) 0x5cf65d8b, 
        (int) 0x60bc6295, (int) 0x642d6771, 
        (int) 0x684368bc, (int) 0x68df76d7, 
        (int) 0x6dd86e6f, (int) 0x6d9b706f, 
        (int) 0x71c85f53, (int) 0x75d87977, 
        (int) 0x7b497b54, (int) 0x7b527cd6, 
        (int) 0x7d715230, (int) 0x84638569, 
        (int) 0x85e48a0e, (int) 0x8b048c46, 
        (int) 0x8e0f9003, (int) 0x900f9419, 
        (int) 0x9676982d, (int) 0x9a3095d8, 
        (int) 0x50cd52d5, (int) 0x540c5802, 
        (int) 0x5c0e61a7, (int) 0x649e6d1e, 
        (int) 0x77b37ae5, (int) 0x80f48404, 
        (int) 0x90539285, (int) 0x5ce09d07, 
        (int) 0x533f5f97, (int) 0x5fb36d9c, 
        (int) 0x72797763, (int) 0x79bf7be4, 
        (int) 0x6bd272ec, (int) 0x8aad6803, 
        (int) 0x6a6151f8, (int) 0x7a816934, 
        (int) 0x5c4a9cf6, (int) 0x82eb5bc5, 
        (int) 0x9149701e, (int) 0x56785c6f, 
        (int) 0x60c76566, (int) 0x6c8c8c5a, 
        (int) 0x90419813, (int) 0x545166c7, 
        (int) 0x920d5948, (int) 0x90a35185, 
        (int) 0x4e4d51ea, (int) 0x85998b0e, 
        (int) 0x7058637a, (int) 0x934b6962, 
        (int) 0x99b47e04, (int) 0x75775357, 
        (int) 0x69608edf, (int) 0x96e36c5d, 
        (int) 0x4e8c5c3c, (int) 0x5f108fe9, 
        (int) 0x53028cd1, (int) 0x80898679, 
        (int) 0x5eff65e5, (int) 0x4e735165, 
        (int) 0x59825c3f, (int) 0x97ee4efb, 
        (int) 0x598a5fcd, (int) 0x8a8d6fe1, 
        (int) 0x79b07962, (int) 0x5be78471, 
        (int) 0x732b71b1, (int) 0x5e745ff5, 
        (int) 0x637b649a, (int) 0x71c37c98, 
        (int) 0x4e435efc, (int) 0x4e4b57dc, 
        (int) 0x56a260a9, (int) 0x6fc37d0d, 
        (int) 0x80fd8133, (int) 0x81bf8fb2, 
        (int) 0x899786a4, (int) 0x5df4628a, 
        (int) 0x64ad8987, (int) 0x67776ce2, 
        (int) 0x6d3e7436, (int) 0x78345a46, 
        (int) 0x7f7582ad, (int) 0x99ac4ff3, 
        (int) 0x5ec362dd, (int) 0x63926557, 
        (int) 0x676f76c3, (int) 0x724c80cc, 
        (int) 0x80ba8f29, (int) 0x914d500d, 
        (int) 0x57f95a92, (int) 0x68856973, 
        (int) 0x716472fd, (int) 0x8cb758f2, 
        (int) 0x8ce0966a, (int) 0x9019877f, 
        (int) 0x79e477e7, (int) 0x84294f2f, 
        (int) 0x5265535a, (int) 0x62cd67cf, 
        (int) 0x6cca767d, (int) 0x7b947c95, 
        (int) 0x82368584, (int) 0x8feb66dd, 
        (int) 0x6f207206, (int) 0x7e1b83ab, 
        (int) 0x99c19ea6, (int) 0x51fd7bb1, 
        (int) 0x78727bb8, (int) 0x80877b48, 
        (int) 0x6ae85e61, (int) 0x808c7551, 
        (int) 0x7560516b, (int) 0x92626e8c, 
        (int) 0x767a9197, (int) 0x9aea4f10, 
        (int) 0x7f70629c, (int) 0x7b4f95a5, 
        (int) 0x9ce9567a, (int) 0x585986e4, 
        (int) 0x96bc4f34, (int) 0x5224534a, 
        (int) 0x53cd53db, (int) 0x5e06642c, 
        (int) 0x6591677f, (int) 0x6c3e6c4e, 
        (int) 0x724872af, (int) 0x73ed7554, 
        (int) 0x7e41822c, (int) 0x85e98ca9, 
        (int) 0x7bc491c6, (int) 0x71699812, 
        (int) 0x98ef633d, (int) 0x6669756a, 
        (int) 0x76e478d0, (int) 0x854386ee, 
        (int) 0x532a5351, (int) 0x54265983, 
        (int) 0x5e875f7c, (int) 0x60b26249, 
        (int) 0x627962ab, (int) 0x65906bd4, 
        (int) 0x6ccc75b2, (int) 0x76ae7891, 
        (int) 0x79d87dcb, (int) 0x7f7780a5, 
        (int) 0x88ab8ab9, (int) 0x8cbb907f, 
        (int) 0x975e98db, (int) 0x6a0b7c38, 
        (int) 0x50995c3e, (int) 0x5fae6787, 
        (int) 0x6bd87435, (int) 0x77097f8e, 
        (int) 0x9f3b67ca, (int) 0x7a175339, 
        (int) 0x758b9aed, (int) 0x5f66819d, 
        (int) 0x83f18098, (int) 0x5f3c5fc5, 
        (int) 0x75627b46, (int) 0x903c6867, 
        (int) 0x59eb5a9b, (int) 0x7d10767e, 
        (int) 0x8b2c4ff5, (int) 0x5f6a6a19, 
        (int) 0x6c376f02, (int) 0x74e27968, 
        (int) 0x88688a55, (int) 0x8c795edf, 
        (int) 0x63cf75c5, (int) 0x79d282d7, 
        (int) 0x932892f2, (int) 0x849c86ed, 
        (int) 0x9c2d54c1, (int) 0x5f6c658c, 
        (int) 0x6d5c7015, (int) 0x8ca78cd3, 
        (int) 0x983b654f, (int) 0x74f64e0d, 
        (int) 0x4ed857e0, (int) 0x592b5a66, 
        (int) 0x5bcc51a8, (int) 0x5e035e9c, 
        (int) 0x60166276, (int) 0x657765a7, 
        (int) 0x666e6d6e, (int) 0x72367b26, 
        (int) 0x8150819a, (int) 0x82998b5c, 
        (int) 0x8ca08ce6, (int) 0x8d74961c, 
        (int) 0x96444fae, (int) 0x64ab6b66, 
        (int) 0x821e8461, (int) 0x856a90e8, 
        (int) 0x5c016953, (int) 0x98a8847a, 
        (int) 0x85574f0f, (int) 0x526f5fa9, 
        (int) 0x5e45670d, (int) 0x798f8179, 
        (int) 0x89078986, (int) 0x6df55f17, 
        (int) 0x62556cb8, (int) 0x4ecf7269, 
        (int) 0x9b925206, (int) 0x543b5674, 
        (int) 0x58b361a4, (int) 0x626e711a, 
        (int) 0x596e7c89, (int) 0x7cde7d1b, 
        (int) 0x96f06587, (int) 0x805e4e19, 
        (int) 0x4f755175, (int) 0x58405e63, 
        (int) 0x5e735f0a, (int) 0x67c44e26, 
        (int) 0x853d9589, (int) 0x965b7c73, 
        (int) 0x980150fb, (int) 0x58c17656, 
        (int) 0x78a75225, (int) 0x77a58511, 
        (int) 0x7b86504f, (int) 0x59097247, 
        (int) 0x7bc77de8, (int) 0x8fba8fd4, 
        (int) 0x904d4fbf, (int) 0x52c95a29, 
        (int) 0x5f0197ad, (int) 0x4fdd8217, 
        (int) 0x92ea5703, (int) 0x63556b69, 
        (int) 0x752b88dc, (int) 0x8f147a42, 
        (int) 0x52df5893, (int) 0x6155620a, 
        (int) 0x66ae6bcd, (int) 0x7c3f83e9, 
        (int) 0x50234ff8, (int) 0x53055446, 
        (int) 0x58315949, (int) 0x5b9d5cf0, 
        (int) 0x5cef5d29, (int) 0x5e9662b1, 
        (int) 0x6367653e, (int) 0x65b9670b, 
        (int) 0x6cd56ce1, (int) 0x70f97832, 
        (int) 0x7e2b80de, (int) 0x82b3840c, 
        (int) 0x84ec8702, (int) 0x89128a2a, 
        (int) 0x8c4a90a6, (int) 0x92d298fd, 
        (int) 0x9cf39d6c, (int) 0x4e4f4ea1, 
        (int) 0x508d5256, (int) 0x574a59a8, 
        (int) 0x5e3d5fd8, (int) 0x5fd9623f, 
        (int) 0x66b4671b, (int) 0x67d068d2, 
        (int) 0x51927d21, (int) 0x80aa81a8, 
        (int) 0x8b008c8c, (int) 0x8cbf927e, 
        (int) 0x96325420, (int) 0x982c5317, 
        (int) 0x50d5535c, (int) 0x58a864b2, 
        (int) 0x67347267, (int) 0x77667a46, 
        (int) 0x91e652c3, (int) 0x6ca16b86, 
        (int) 0x58005e4c, (int) 0x5954672c, 
        (int) 0x7ffb51e1, (int) 0x76c66469, 
        (int) 0x78e89b54, (int) 0x9ebb57cb, 
        (int) 0x59b96627, (int) 0x679a6bce, 
        (int) 0x54e969d9, (int) 0x5e55819c, 
        (int) 0x67959baa, (int) 0x67fe9c52, 
        (int) 0x685d4ea6, (int) 0x4fe353c8, 
        (int) 0x62b9672b, (int) 0x6cab8fc4, 
        (int) 0x4fad7e6d, (int) 0x9ebf4e07, 
        (int) 0x61626e80, (int) 0x6f2b8513, 
        (int) 0x5473672a, (int) 0x9b455df3, 
        (int) 0x7b955cac, (int) 0x5bc6871c, 
        (int) 0x6e4a84d1, (int) 0x7a148108, 
        (int) 0x59997c8d, (int) 0x6c117720, 
        (int) 0x52d95922, (int) 0x7121725f, 
        (int) 0x77db9727, (int) 0x9d61690b, 
        (int) 0x5a7f5a18, (int) 0x51a5540d, 
        (int) 0x547d660e, (int) 0x76df8ff7, 
        (int) 0x92989cf4, (int) 0x59ea725d, 
        (int) 0x6ec5514d, (int) 0x68c97dbf, 
        (int) 0x7dec9762, (int) 0x9eba6478, 
        (int) 0x6a218302, (int) 0x59845b5f, 
        (int) 0x6bdb731b, (int) 0x76f27db2, 
        (int) 0x80178499, (int) 0x51326728, 
        (int) 0x9ed976ee, (int) 0x676252ff, 
        (int) 0x99055c24, (int) 0x623b7c7e, 
        (int) 0x8cb0554f, (int) 0x60b67d0b, 
        (int) 0x95805301, (int) 0x4e5f51b6, 
        (int) 0x591c723a, (int) 0x803691ce, 
        (int) 0x5f2577e2, (int) 0x53845f79, 
        (int) 0x7d0485ac, (int) 0x8a338e8d, 
        (int) 0x975667f3, (int) 0x85ae9453, 
        (int) 0x61096108, (int) 0x6cb97652, 
        (int) 0x8aed8f38, (int) 0x552f4f51, 
        (int) 0x512a52c7, (int) 0x53cb5ba5, 
        (int) 0x5e7d60a0, (int) 0x618263d6, 
        (int) 0x670967da, (int) 0x6e676d8c, 
        (int) 0x73367337, (int) 0x75317950, 
        (int) 0x88d58a98, (int) 0x904a9091, 
        (int) 0x90f596c4, (int) 0x878d5915, 
        (int) 0x4e884f59, (int) 0x4e0e8a89, 
        (int) 0x8f3f9810, (int) 0x50ad5e7c, 
        (int) 0x59965bb9, (int) 0x5eb863da, 
        (int) 0x63fa64c1, (int) 0x66dc694a, 
        (int) 0x69d86d0b, (int) 0x6eb67194, 
        (int) 0x75287aaf, (int) 0x7f8a8000, 
        (int) 0x844984c9, (int) 0x89818b21, 
        (int) 0x8e0a9065, (int) 0x967d990a, 
        (int) 0x617e6291, (int) 0x6b326c83, 
        (int) 0x6d747fcc, (int) 0x7ffc6dc0, 
        (int) 0x7f8587ba, (int) 0x88f86765, 
        (int) 0x83b1983c, (int) 0x96f76d1b, 
        (int) 0x7d61843d, (int) 0x916a4e71, 
        (int) 0x53755d50, (int) 0x6b046feb, 
        (int) 0x85cd862d, (int) 0x89a75229, 
        (int) 0x540f5c65, (int) 0x674e68a8, 
        (int) 0x74067483, (int) 0x75e288cf, 
        (int) 0x88e191cc, (int) 0x96e29678, 
        (int) 0x5f8b7387, (int) 0x7acb844e, 
        (int) 0x63a07565, (int) 0x52896d41, 
        (int) 0x6e9c7409, (int) 0x7559786b, 
        (int) 0x7c929686, (int) 0x7adc9f8d, 
        (int) 0x4fb6616e, (int) 0x65c5865c, 
        (int) 0x4e864eae, (int) 0x50da4e21, 
        (int) 0x51cc5bee, (int) 0x65996881, 
        (int) 0x6dbc731f, (int) 0x764277ad, 
        (int) 0x7a1c7ce7, (int) 0x826f8ad2, 
        (int) 0x907c91cf, (int) 0x96759818, 
        (int) 0x529b7dd1, (int) 0x502b5398, 
        (int) 0x67976dcb, (int) 0x71d07433, 
        (int) 0x81e88f2a, (int) 0x96a39c57, 
        (int) 0x9e9f7460, (int) 0x58416d99, 
        (int) 0x7d2f985e, (int) 0x4ee44f36, 
        (int) 0x4f8b51b7, (int) 0x52b15dba, 
        (int) 0x601c73b2, (int) 0x793c82d3, 
        (int) 0x923496b7, (int) 0x96f6970a, 
        (int) 0x9e979f62, (int) 0x66a66b74, 
        (int) 0x521752a3, (int) 0x70c888c2, 
        (int) 0x5ec9604b, (int) 0x61906f23, 
        (int) 0x71497c3e, (int) 0x7df4806f, 
        (int) 0x84ee9023, (int) 0x932c5442, 
        (int) 0x9b6f6ad3, (int) 0x70898cc2, 
        (int) 0x8def9732, (int) 0x52b45a41, 
        (int) 0x5eca5f04, (int) 0x6717697c, 
        (int) 0x69946d6a, (int) 0x6f0f7262, 
        (int) 0x72fc7bed, (int) 0x8001807e, 
        (int) 0x874b90ce, (int) 0x516d9e93, 
        (int) 0x7984808b, (int) 0x93328ad6, 
        (int) 0x502d548c, (int) 0x8a716b6a, 
        (int) 0x8cc48107, (int) 0x60d167a0, 
        (int) 0x9df24e99, (int) 0x4e989c10, 
        (int) 0x8a6b85c1, (int) 0x85686900, 
        (int) 0x6e7e7897, (int) 0x81550000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x00000000, 
        (int) 0x00000000, (int) 0x5f0c4e10, 
        (int) 0x4e154e2a, (int) 0x4e314e36, 
        (int) 0x4e3c4e3f, (int) 0x4e424e56, 
        (int) 0x4e584e82, (int) 0x4e858c6b, 
        (int) 0x4e8a8212, (int) 0x5f0d4e8e, 
        (int) 0x4e9e4e9f, (int) 0x4ea04ea2, 
        (int) 0x4eb04eb3, (int) 0x4eb64ece, 
        (int) 0x4ecd4ec4, (int) 0x4ec64ec2, 
        (int) 0x4ed74ede, (int) 0x4eed4edf, 
        (int) 0x4ef74f09, (int) 0x4f5a4f30, 
        (int) 0x4f5b4f5d, (int) 0x4f574f47, 
        (int) 0x4f764f88, (int) 0x4f8f4f98, 
        (int) 0x4f7b4f69, (int) 0x4f704f91, 
        (int) 0x4f6f4f86, (int) 0x4f965118, 
        (int) 0x4fd44fdf, (int) 0x4fce4fd8, 
        (int) 0x4fdb4fd1, (int) 0x4fda4fd0, 
        (int) 0x4fe44fe5, (int) 0x501a5028, 
        (int) 0x5014502a, (int) 0x50255005, 
        (int) 0x4f1c4ff6, (int) 0x50215029, 
        (int) 0x502c4ffe, (int) 0x4fef5011, 
        (int) 0x50065043, (int) 0x50476703, 
        (int) 0x50555050, (int) 0x5048505a, 
        (int) 0x5056506c, (int) 0x50785080, 
        (int) 0x509a5085, (int) 0x50b450b2, 
        (int) 0x50c950ca, (int) 0x50b350c2, 
        (int) 0x50d650de, (int) 0x50e550ed, 
        (int) 0x50e350ee, (int) 0x50f950f5, 
        (int) 0x51095101, (int) 0x51025116, 
        (int) 0x51155114, (int) 0x511a5121, 
        (int) 0x513a5137, (int) 0x513c513b, 
        (int) 0x513f5140, (int) 0x5152514c, 
        (int) 0x51545162, (int) 0x7af85169, 
        (int) 0x516a516e, (int) 0x51805182, 
        (int) 0x56d8518c, (int) 0x5189518f, 
        (int) 0x51915193, (int) 0x51955196, 
        (int) 0x51a451a6, (int) 0x51a251a9, 
        (int) 0x51aa51ab, (int) 0x51b351b1, 
        (int) 0x51b251b0, (int) 0x51b551bd, 
        (int) 0x51c551c9, (int) 0x51db51e0, 
        (int) 0x865551e9, (int) 0x51ed51f0, 
        (int) 0x51f551fe, (int) 0x5204520b, 
        (int) 0x5214520e, (int) 0x5227522a, 
        (int) 0x522e5233, (int) 0x5239524f, 
        (int) 0x5244524b, (int) 0x524c525e, 
        (int) 0x5254526a, (int) 0x52745269, 
        (int) 0x5273527f, (int) 0x527d528d, 
        (int) 0x52945292, (int) 0x52715288, 
        (int) 0x52918fa8, (int) 0x8fa752ac, 
        (int) 0x52ad52bc, (int) 0x52b552c1, 
        (int) 0x52cd52d7, (int) 0x52de52e3, 
        (int) 0x52e698ed, (int) 0x52e052f3, 
        (int) 0x52f552f8, (int) 0x52f95306, 
        (int) 0x53087538, (int) 0x530d5310, 
        (int) 0x530f5315, (int) 0x531a5323, 
        (int) 0x532f5331, (int) 0x53335338, 
        (int) 0x53405346, (int) 0x53454e17, 
        (int) 0x5349534d, (int) 0x51d6535e, 
        (int) 0x5369536e, (int) 0x5918537b, 
        (int) 0x53775382, (int) 0x539653a0, 
        (int) 0x53a653a5, (int) 0x53ae53b0, 
        (int) 0x53b653c3, (int) 0x7c1296d9, 
        (int) 0x53df66fc, (int) 0x71ee53ee, 
        (int) 0x53e853ed, (int) 0x53fa5401, 
        (int) 0x543d5440, (int) 0x542c542d, 
        (int) 0x543c542e, (int) 0x54365429, 
        (int) 0x541d544e, (int) 0x548f5475, 
        (int) 0x548e545f, (int) 0x54715477, 
        (int) 0x54705492, (int) 0x547b5480, 
        (int) 0x54765484, (int) 0x54905486, 
        (int) 0x54c754a2, (int) 0x54b854a5, 
        (int) 0x54ac54c4, (int) 0x54c854a8, 
        (int) 0x54ab54c2, (int) 0x54a454be, 
        (int) 0x54bc54d8, (int) 0x54e554e6, 
        (int) 0x550f5514, (int) 0x54fd54ee, 
        (int) 0x54ed54fa, (int) 0x54e25539, 
        (int) 0x55405563, (int) 0x554c552e, 
        (int) 0x555c5545, (int) 0x55565557, 
        (int) 0x55385533, (int) 0x555d5599, 
        (int) 0x558054af, (int) 0x558a559f, 
        (int) 0x557b557e, (int) 0x5598559e, 
        (int) 0x55ae557c, (int) 0x558355a9, 
        (int) 0x558755a8, (int) 0x55da55c5, 
        (int) 0x55df55c4, (int) 0x55dc55e4, 
        (int) 0x55d45614, (int) 0x55f75616, 
        (int) 0x55fe55fd, (int) 0x561b55f9, 
        (int) 0x564e5650, (int) 0x71df5634, 
        (int) 0x56365632, (int) 0x5638566b, 
        (int) 0x5664562f, (int) 0x566c566a, 
        (int) 0x56865680, (int) 0x568a56a0, 
        (int) 0x5694568f, (int) 0x56a556ae, 
        (int) 0x56b656b4, (int) 0x56c256bc, 
        (int) 0x56c156c3, (int) 0x56c056c8, 
        (int) 0x56ce56d1, (int) 0x56d356d7, 
        (int) 0x56ee56f9, (int) 0x570056ff, 
        (int) 0x57045709, (int) 0x5708570b, 
        (int) 0x570d5713, (int) 0x57185716, 
        (int) 0x55c7571c, (int) 0x57265737, 
        (int) 0x5738574e, (int) 0x573b5740, 
        (int) 0x574f5769, (int) 0x57c05788, 
        (int) 0x5761577f, (int) 0x57895793, 
        (int) 0x57a057b3, (int) 0x57a457aa, 
        (int) 0x57b057c3, (int) 0x57c657d4, 
        (int) 0x57d257d3, (int) 0x580a57d6, 
        (int) 0x57e3580b, (int) 0x5819581d, 
        (int) 0x58725821, (int) 0x5862584b, 
        (int) 0x58706bc0, (int) 0x5852583d, 
        (int) 0x58795885, (int) 0x58b9589f, 
        (int) 0x58ab58ba, (int) 0x58de58bb, 
        (int) 0x58b858ae, (int) 0x58c558d3, 
        (int) 0x58d158d7, (int) 0x58d958d8, 
        (int) 0x58e558dc, (int) 0x58e458df, 
        (int) 0x58ef58fa, (int) 0x58f958fb, 
        (int) 0x58fc58fd, (int) 0x5902590a, 
        (int) 0x5910591b, (int) 0x68a65925, 
        (int) 0x592c592d, (int) 0x59325938, 
        (int) 0x593e7ad2, (int) 0x59555950, 
        (int) 0x594e595a, (int) 0x59585962, 
        (int) 0x59605967, (int) 0x596c5969, 
        (int) 0x59785981, (int) 0x599d4f5e, 
        (int) 0x4fab59a3, (int) 0x59b259c6, 
        (int) 0x59e859dc, (int) 0x598d59d9, 
        (int) 0x59da5a25, (int) 0x5a1f5a11, 
        (int) 0x5a1c5a09, (int) 0x5a1a5a40, 
        (int) 0x5a6c5a49, (int) 0x5a355a36, 
        (int) 0x5a625a6a, (int) 0x5a9a5abc, 
        (int) 0x5abe5acb, (int) 0x5ac25abd, 
        (int) 0x5ae35ad7, (int) 0x5ae65ae9, 
        (int) 0x5ad65afa, (int) 0x5afb5b0c, 
        (int) 0x5b0b5b16, (int) 0x5b325ad0, 
        (int) 0x5b2a5b36, (int) 0x5b3e5b43, 
        (int) 0x5b455b40, (int) 0x5b515b55, 
        (int) 0x5b5a5b5b, (int) 0x5b655b69, 
        (int) 0x5b705b73, (int) 0x5b755b78, 
        (int) 0x65885b7a, (int) 0x5b805b83, 
        (int) 0x5ba65bb8, (int) 0x5bc35bc7, 
        (int) 0x5bc95bd4, (int) 0x5bd05be4, 
        (int) 0x5be65be2, (int) 0x5bde5be5, 
        (int) 0x5beb5bf0, (int) 0x5bf65bf3, 
        (int) 0x5c055c07, (int) 0x5c085c0d, 
        (int) 0x5c135c20, (int) 0x5c225c28, 
        (int) 0x5c385c39, (int) 0x5c415c46, 
        (int) 0x5c4e5c53, (int) 0x5c505c4f, 
        (int) 0x5b715c6c, (int) 0x5c6e4e62, 
        (int) 0x5c765c79, (int) 0x5c8c5c91, 
        (int) 0x5c94599b, (int) 0x5cab5cbb, 
        (int) 0x5cb65cbc, (int) 0x5cb75cc5, 
        (int) 0x5cbe5cc7, (int) 0x5cd95ce9, 
        (int) 0x5cfd5cfa, (int) 0x5ced5d8c, 
        (int) 0x5cea5d0b, (int) 0x5d155d17, 
        (int) 0x5d5c5d1f, (int) 0x5d1b5d11, 
        (int) 0x5d145d22, (int) 0x5d1a5d19, 
        (int) 0x5d185d4c, (int) 0x5d525d4e, 
        (int) 0x5d4b5d6c, (int) 0x5d735d76, 
        (int) 0x5d875d84, (int) 0x5d825da2, 
        (int) 0x5d9d5dac, (int) 0x5dae5dbd, 
        (int) 0x5d905db7, (int) 0x5dbc5dc9, 
        (int) 0x5dcd5dd3, (int) 0x5dd25dd6, 
        (int) 0x5ddb5deb, (int) 0x5df25df5, 
        (int) 0x5e0b5e1a, (int) 0x5e195e11, 
        (int) 0x5e1b5e36, (int) 0x5e375e44, 
        (int) 0x5e435e40, (int) 0x5e4e5e57, 
        (int) 0x5e545e5f, (int) 0x5e625e64, 
        (int) 0x5e475e75, (int) 0x5e765e7a, 
        (int) 0x9ebc5e7f, (int) 0x5ea05ec1, 
        (int) 0x5ec25ec8, (int) 0x5ed05ecf, 
        (int) 0x5ed65ee3, (int) 0x5edd5eda, 
        (int) 0x5edb5ee2, (int) 0x5ee15ee8, 
        (int) 0x5ee95eec, (int) 0x5ef15ef3, 
        (int) 0x5ef05ef4, (int) 0x5ef85efe, 
        (int) 0x5f035f09, (int) 0x5f5d5f5c, 
        (int) 0x5f0b5f11, (int) 0x5f165f29, 
        (int) 0x5f2d5f38, (int) 0x5f415f48, 
        (int) 0x5f4c5f4e, (int) 0x5f2f5f51, 
        (int) 0x5f565f57, (int) 0x5f595f61, 
        (int) 0x5f6d5f73, (int) 0x5f775f83, 
        (int) 0x5f825f7f, (int) 0x5f8a5f88, 
        (int) 0x5f915f87, (int) 0x5f9e5f99, 
        (int) 0x5f985fa0, (int) 0x5fa85fad, 
        (int) 0x5fbc5fd6, (int) 0x5ffb5fe4, 
        (int) 0x5ff85ff1, (int) 0x5fdd60b3, 
        (int) 0x5fff6021, (int) 0x60606019, 
        (int) 0x60106029, (int) 0x600e6031, 
        (int) 0x601b6015, (int) 0x602b6026, 
        (int) 0x600f603a, (int) 0x605a6041, 
        (int) 0x606a6077, (int) 0x605f604a, 
        (int) 0x6046604d, (int) 0x60636043, 
        (int) 0x60646042, (int) 0x606c606b, 
        (int) 0x60596081, (int) 0x608d60e7, 
        (int) 0x6083609a, (int) 0x6084609b, 
        (int) 0x60966097, (int) 0x609260a7, 
        (int) 0x608b60e1, (int) 0x60b860e0, 
        (int) 0x60d360b4, (int) 0x5ff060bd, 
        (int) 0x60c660b5, (int) 0x60d8614d, 
        (int) 0x61156106, (int) 0x60f660f7, 
        (int) 0x610060f4, (int) 0x60fa6103, 
        (int) 0x612160fb, (int) 0x60f1610d, 
        (int) 0x610e6147, (int) 0x613e6128, 
        (int) 0x6127614a, (int) 0x613f613c, 
        (int) 0x612c6134, (int) 0x613d6142, 
        (int) 0x61446173, (int) 0x61776158, 
        (int) 0x6159615a, (int) 0x616b6174, 
        (int) 0x616f6165, (int) 0x6171615f, 
        (int) 0x615d6153, (int) 0x61756199, 
        (int) 0x61966187, (int) 0x61ac6194, 
        (int) 0x619a618a, (int) 0x619161ab, 
        (int) 0x61ae61cc, (int) 0x61ca61c9, 
        (int) 0x61f761c8, (int) 0x61c361c6, 
        (int) 0x61ba61cb, (int) 0x7f7961cd, 
        (int) 0x61e661e3, (int) 0x61f661fa, 
        (int) 0x61f461ff, (int) 0x61fd61fc, 
        (int) 0x61fe6200, (int) 0x62086209, 
        (int) 0x620d620c, (int) 0x6214621b, 
        (int) 0x621e6221, (int) 0x622a622e, 
        (int) 0x62306232, (int) 0x62336241, 
        (int) 0x624e625e, (int) 0x6263625b, 
        (int) 0x62606268, (int) 0x627c6282, 
        (int) 0x6289627e, (int) 0x62926293, 
        (int) 0x629662d4, (int) 0x62836294, 
        (int) 0x62d762d1, (int) 0x62bb62cf, 
        (int) 0x62ff62c6, (int) 0x64d462c8, 
        (int) 0x62dc62cc, (int) 0x62ca62c2, 
        (int) 0x62c7629b, (int) 0x62c9630c, 
        (int) 0x62ee62f1, (int) 0x63276302, 
        (int) 0x630862ef, (int) 0x62f56350, 
        (int) 0x633e634d, (int) 0x641c634f, 
        (int) 0x6396638e, (int) 0x638063ab, 
        (int) 0x637663a3, (int) 0x638f6389, 
        (int) 0x639f63b5, (int) 0x636b6369, 
        (int) 0x63be63e9, (int) 0x63c063c6, 
        (int) 0x63e363c9, (int) 0x63d263f6, 
        (int) 0x63c46416, (int) 0x64346406, 
        (int) 0x64136426, (int) 0x6436651d, 
        (int) 0x64176428, (int) 0x640f6467, 
        (int) 0x646f6476, (int) 0x644e652a, 
        (int) 0x64956493, (int) 0x64a564a9, 
        (int) 0x648864bc, (int) 0x64da64d2, 
        (int) 0x64c564c7, (int) 0x64bb64d8, 
        (int) 0x64c264f1, (int) 0x64e78209, 
        (int) 0x64e064e1, (int) 0x62ac64e3, 
        (int) 0x64ef652c, (int) 0x64f664f4, 
        (int) 0x64f264fa, (int) 0x650064fd, 
        (int) 0x6518651c, (int) 0x65056524, 
        (int) 0x6523652b, (int) 0x65346535, 
        (int) 0x65376536, (int) 0x6538754b, 
        (int) 0x65486556, (int) 0x6555654d, 
        (int) 0x6558655e, (int) 0x655d6572, 
        (int) 0x65786582, (int) 0x65838b8a, 
        (int) 0x659b659f, (int) 0x65ab65b7, 
        (int) 0x65c365c6, (int) 0x65c165c4, 
        (int) 0x65cc65d2, (int) 0x65db65d9, 
        (int) 0x65e065e1, (int) 0x65f16772, 
        (int) 0x660a6603, (int) 0x65fb6773, 
        (int) 0x66356636, (int) 0x6634661c, 
        (int) 0x664f6644, (int) 0x66496641, 
        (int) 0x665e665d, (int) 0x66646667, 
        (int) 0x6668665f, (int) 0x66626670, 
        (int) 0x66836688, (int) 0x668e6689, 
        (int) 0x66846698, (int) 0x669d66c1, 
        (int) 0x66b966c9, (int) 0x66be66bc, 
        (int) 0x66c466b8, (int) 0x66d666da, 
        (int) 0x66e0663f, (int) 0x66e666e9, 
        (int) 0x66f066f5, (int) 0x66f7670f, 
        (int) 0x6716671e, (int) 0x67266727, 
        (int) 0x9738672e, (int) 0x673f6736, 
        (int) 0x67416738, (int) 0x67376746, 
        (int) 0x675e6760, (int) 0x67596763, 
        (int) 0x67646789, (int) 0x677067a9, 
        (int) 0x677c676a, (int) 0x678c678b, 
        (int) 0x67a667a1, (int) 0x678567b7, 
        (int) 0x67ef67b4, (int) 0x67ec67b3, 
        (int) 0x67e967b8, (int) 0x67e467de, 
        (int) 0x67dd67e2, (int) 0x67ee67b9, 
        (int) 0x67ce67c6, (int) 0x67e76a9c, 
        (int) 0x681e6846, (int) 0x68296840, 
        (int) 0x684d6832, (int) 0x684e68b3, 
        (int) 0x682b6859, (int) 0x68636877, 
        (int) 0x687f689f, (int) 0x688f68ad, 
        (int) 0x6894689d, (int) 0x689b6883, 
        (int) 0x6aae68b9, (int) 0x687468b5, 
        (int) 0x68a068ba, (int) 0x690f688d, 
        (int) 0x687e6901, (int) 0x68ca6908, 
        (int) 0x68d86922, (int) 0x692668e1, 
        (int) 0x690c68cd, (int) 0x68d468e7, 
        (int) 0x68d56936, (int) 0x69126904, 
        (int) 0x68d768e3, (int) 0x692568f9, 
        (int) 0x68e068ef, (int) 0x6928692a, 
        (int) 0x691a6923, (int) 0x692168c6, 
        (int) 0x69796977, (int) 0x695c6978, 
        (int) 0x696b6954, (int) 0x697e696e, 
        (int) 0x69396974, (int) 0x693d6959, 
        (int) 0x69306961, (int) 0x695e695d, 
        (int) 0x6981696a, (int) 0x69b269ae, 
        (int) 0x69d069bf, (int) 0x69c169d3, 
        (int) 0x69be69ce, (int) 0x5be869ca, 
        (int) 0x69dd69bb, (int) 0x69c369a7, 
        (int) 0x6a2e6991, (int) 0x69a0699c, 
        (int) 0x699569b4, (int) 0x69de69e8, 
        (int) 0x6a026a1b, (int) 0x69ff6b0a, 
        (int) 0x69f969f2, (int) 0x69e76a05, 
        (int) 0x69b16a1e, (int) 0x69ed6a14, 
        (int) 0x69eb6a0a, (int) 0x6a126ac1, 
        (int) 0x6a236a13, (int) 0x6a446a0c, 
        (int) 0x6a726a36, (int) 0x6a786a47, 
        (int) 0x6a626a59, (int) 0x6a666a48, 
        (int) 0x6a386a22, (int) 0x6a906a8d, 
        (int) 0x6aa06a84, (int) 0x6aa26aa3, 
        (int) 0x6a978617, (int) 0x6abb6ac3, 
        (int) 0x6ac26ab8, (int) 0x6ab36aac, 
        (int) 0x6ade6ad1, (int) 0x6adf6aaa, 
        (int) 0x6ada6aea, (int) 0x6afb6b05, 
        (int) 0x86166afa, (int) 0x6b126b16, 
        (int) 0x9b316b1f, (int) 0x6b386b37, 
        (int) 0x76dc6b39, (int) 0x98ee6b47, 
        (int) 0x6b436b49, (int) 0x6b506b59, 
        (int) 0x6b546b5b, (int) 0x6b5f6b61, 
        (int) 0x6b786b79, (int) 0x6b7f6b80, 
        (int) 0x6b846b83, (int) 0x6b8d6b98, 
        (int) 0x6b956b9e, (int) 0x6ba46baa, 
        (int) 0x6bab6baf, (int) 0x6bb26bb1, 
        (int) 0x6bb36bb7, (int) 0x6bbc6bc6, 
        (int) 0x6bcb6bd3, (int) 0x6bdf6bec, 
        (int) 0x6beb6bf3, (int) 0x6bef9ebe, 
        (int) 0x6c086c13, (int) 0x6c146c1b, 
        (int) 0x6c246c23, (int) 0x6c5e6c55, 
        (int) 0x6c626c6a, (int) 0x6c826c8d, 
        (int) 0x6c9a6c81, (int) 0x6c9b6c7e, 
        (int) 0x6c686c73, (int) 0x6c926c90, 
        (int) 0x6cc46cf1, (int) 0x6cd36cbd, 
        (int) 0x6cd76cc5, (int) 0x6cdd6cae, 
        (int) 0x6cb16cbe, (int) 0x6cba6cdb, 
        (int) 0x6cef6cd9, (int) 0x6cea6d1f, 
        (int) 0x884d6d36, (int) 0x6d2b6d3d, 
        (int) 0x6d386d19, (int) 0x6d356d33, 
        (int) 0x6d126d0c, (int) 0x6d636d93, 
        (int) 0x6d646d5a, (int) 0x6d796d59, 
        (int) 0x6d8e6d95, (int) 0x6fe46d85, 
        (int) 0x6df96e15, (int) 0x6e0a6db5, 
        (int) 0x6dc76de6, (int) 0x6db86dc6, 
        (int) 0x6dec6dde, (int) 0x6dcc6de8, 
        (int) 0x6dd26dc5, (int) 0x6dfa6dd9, 
        (int) 0x6de46dd5, (int) 0x6dea6dee, 
        (int) 0x6e2d6e6e, (int) 0x6e2e6e19, 
        (int) 0x6e726e5f, (int) 0x6e3e6e23, 
        (int) 0x6e6b6e2b, (int) 0x6e766e4d, 
        (int) 0x6e1f6e43, (int) 0x6e3a6e4e, 
        (int) 0x6e246eff, (int) 0x6e1d6e38, 
        (int) 0x6e826eaa, (int) 0x6e986ec9, 
        (int) 0x6eb76ed3, (int) 0x6ebd6eaf, 
        (int) 0x6ec46eb2, (int) 0x6ed46ed5, 
        (int) 0x6e8f6ea5, (int) 0x6ec26e9f, 
        (int) 0x6f416f11, (int) 0x704c6eec, 
        (int) 0x6ef86efe, (int) 0x6f3f6ef2, 
        (int) 0x6f316eef, (int) 0x6f326ecc, 
        (int) 0x6f3e6f13, (int) 0x6ef76f86, 
        (int) 0x6f7a6f78, (int) 0x6f816f80, 
        (int) 0x6f6f6f5b, (int) 0x6ff36f6d, 
        (int) 0x6f826f7c, (int) 0x6f586f8e, 
        (int) 0x6f916fc2, (int) 0x6f666fb3, 
        (int) 0x6fa36fa1, (int) 0x6fa46fb9, 
        (int) 0x6fc66faa, (int) 0x6fdf6fd5, 
        (int) 0x6fec6fd4, (int) 0x6fd86ff1, 
        (int) 0x6fee6fdb, (int) 0x7009700b, 
        (int) 0x6ffa7011, (int) 0x7001700f, 
        (int) 0x6ffe701b, (int) 0x701a6f74, 
        (int) 0x701d7018, (int) 0x701f7030, 
        (int) 0x703e7032, (int) 0x70517063, 
        (int) 0x70997092, (int) 0x70af70f1, 
        (int) 0x70ac70b8, (int) 0x70b370ae, 
        (int) 0x70df70cb, (int) 0x70dd70d9, 
        (int) 0x710970fd, (int) 0x711c7119, 
        (int) 0x71657155, (int) 0x71887166, 
        (int) 0x7162714c, (int) 0x7156716c, 
        (int) 0x718f71fb, (int) 0x71847195, 
        (int) 0x71a871ac, (int) 0x71d771b9, 
        (int) 0x71be71d2, (int) 0x71c971d4, 
        (int) 0x71ce71e0, (int) 0x71ec71e7, 
        (int) 0x71f571fc, (int) 0x71f971ff, 
        (int) 0x720d7210, (int) 0x721b7228, 
        (int) 0x722d722c, (int) 0x72307232, 
        (int) 0x723b723c, (int) 0x723f7240, 
        (int) 0x7246724b, (int) 0x72587274, 
        (int) 0x727e7282, (int) 0x72817287, 
        (int) 0x72927296, (int) 0x72a272a7, 
        (int) 0x72b972b2, (int) 0x72c372c6, 
        (int) 0x72c472ce, (int) 0x72d272e2, 
        (int) 0x72e072e1, (int) 0x72f972f7, 
        (int) 0x500f7317, (int) 0x730a731c, 
        (int) 0x7316731d, (int) 0x7334732f, 
        (int) 0x73297325, (int) 0x733e734e, 
        (int) 0x734f9ed8, (int) 0x7357736a, 
        (int) 0x73687370, (int) 0x73787375, 
        (int) 0x737b737a, (int) 0x73c873b3, 
        (int) 0x73ce73bb, (int) 0x73c073e5, 
        (int) 0x73ee73de, (int) 0x74a27405, 
        (int) 0x746f7425, (int) 0x73f87432, 
        (int) 0x743a7455, (int) 0x743f745f, 
        (int) 0x74597441, (int) 0x745c7469, 
        (int) 0x74707463, (int) 0x746a7476, 
        (int) 0x747e748b, (int) 0x749e74a7, 
        (int) 0x74ca74cf, (int) 0x74d473f1, 
        (int) 0x74e074e3, (int) 0x74e774e9, 
        (int) 0x74ee74f2, (int) 0x74f074f1, 
        (int) 0x74f874f7, (int) 0x75047503, 
        (int) 0x7505750c, (int) 0x750e750d, 
        (int) 0x75157513, (int) 0x751e7526, 
        (int) 0x752c753c, (int) 0x7544754d, 
        (int) 0x754a7549, (int) 0x755b7546, 
        (int) 0x755a7569, (int) 0x75647567, 
        (int) 0x756b756d, (int) 0x75787576, 
        (int) 0x75867587, (int) 0x7574758a, 
        (int) 0x75897582, (int) 0x7594759a, 
        (int) 0x759d75a5, (int) 0x75a375c2, 
        (int) 0x75b375c3, (int) 0x75b575bd, 
        (int) 0x75b875bc, (int) 0x75b175cd, 
        (int) 0x75ca75d2, (int) 0x75d975e3, 
        (int) 0x75de75fe, (int) 0x75ff75fc, 
        (int) 0x760175f0, (int) 0x75fa75f2, 
        (int) 0x75f3760b, (int) 0x760d7609, 
        (int) 0x761f7627, (int) 0x76207621, 
        (int) 0x76227624, (int) 0x76347630, 
        (int) 0x763b7647, (int) 0x76487646, 
        (int) 0x765c7658, (int) 0x76617662, 
        (int) 0x76687669, (int) 0x766a7667, 
        (int) 0x766c7670, (int) 0x76727676, 
        (int) 0x7678767c, (int) 0x76807683, 
        (int) 0x7688768b, (int) 0x768e7696, 
        (int) 0x76937699, (int) 0x769a76b0, 
        (int) 0x76b476b8, (int) 0x76b976ba, 
        (int) 0x76c276cd, (int) 0x76d676d2, 
        (int) 0x76de76e1, (int) 0x76e576e7, 
        (int) 0x76ea862f, (int) 0x76fb7708, 
        (int) 0x77077704, (int) 0x77297724, 
        (int) 0x771e7725, (int) 0x7726771b, 
        (int) 0x77377738, (int) 0x7747775a, 
        (int) 0x7768776b, (int) 0x775b7765, 
        (int) 0x777f777e, (int) 0x7779778e, 
        (int) 0x778b7791, (int) 0x77a0779e, 
        (int) 0x77b077b6, (int) 0x77b977bf, 
        (int) 0x77bc77bd, (int) 0x77bb77c7, 
        (int) 0x77cd77d7, (int) 0x77da77dc, 
        (int) 0x77e377ee, (int) 0x77fc780c, 
        (int) 0x78127926, (int) 0x7820792a, 
        (int) 0x7845788e, (int) 0x78747886, 
        (int) 0x787c789a, (int) 0x788c78a3, 
        (int) 0x78b578aa, (int) 0x78af78d1, 
        (int) 0x78c678cb, (int) 0x78d478be, 
        (int) 0x78bc78c5, (int) 0x78ca78ec, 
        (int) 0x78e778da, (int) 0x78fd78f4, 
        (int) 0x79077912, (int) 0x79117919, 
        (int) 0x792c792b, (int) 0x79407960, 
        (int) 0x7957795f, (int) 0x795a7955, 
        (int) 0x7953797a, (int) 0x797f798a, 
        (int) 0x799d79a7, (int) 0x9f4b79aa, 
        (int) 0x79ae79b3, (int) 0x79b979ba, 
        (int) 0x79c979d5, (int) 0x79e779ec, 
        (int) 0x79e179e3, (int) 0x7a087a0d, 
        (int) 0x7a187a19, (int) 0x7a207a1f, 
        (int) 0x79807a31, (int) 0x7a3b7a3e, 
        (int) 0x7a377a43, (int) 0x7a577a49, 
        (int) 0x7a617a62, (int) 0x7a699f9d, 
        (int) 0x7a707a79, (int) 0x7a7d7a88, 
        (int) 0x7a977a95, (int) 0x7a987a96, 
        (int) 0x7aa97ac8, (int) 0x7ab07ab6, 
        (int) 0x7ac57ac4, (int) 0x7abf9083, 
        (int) 0x7ac77aca, (int) 0x7acd7acf, 
        (int) 0x7ad57ad3, (int) 0x7ad97ada, 
        (int) 0x7add7ae1, (int) 0x7ae27ae6, 
        (int) 0x7aed7af0, (int) 0x7b027b0f, 
        (int) 0x7b0a7b06, (int) 0x7b337b18, 
        (int) 0x7b197b1e, (int) 0x7b357b28, 
        (int) 0x7b367b50, (int) 0x7b7a7b04, 
        (int) 0x7b4d7b0b, (int) 0x7b4c7b45, 
        (int) 0x7b757b65, (int) 0x7b747b67, 
        (int) 0x7b707b71, (int) 0x7b6c7b6e, 
        (int) 0x7b9d7b98, (int) 0x7b9f7b8d, 
        (int) 0x7b9c7b9a, (int) 0x7b8b7b92, 
        (int) 0x7b8f7b5d, (int) 0x7b997bcb, 
        (int) 0x7bc17bcc, (int) 0x7bcf7bb4, 
        (int) 0x7bc67bdd, (int) 0x7be97c11, 
        (int) 0x7c147be6, (int) 0x7be57c60, 
        (int) 0x7c007c07, (int) 0x7c137bf3, 
        (int) 0x7bf77c17, (int) 0x7c0d7bf6, 
        (int) 0x7c237c27, (int) 0x7c2a7c1f, 
        (int) 0x7c377c2b, (int) 0x7c3d7c4c, 
        (int) 0x7c437c54, (int) 0x7c4f7c40, 
        (int) 0x7c507c58, (int) 0x7c5f7c64, 
        (int) 0x7c567c65, (int) 0x7c6c7c75, 
        (int) 0x7c837c90, (int) 0x7ca47cad, 
        (int) 0x7ca27cab, (int) 0x7ca17ca8, 
        (int) 0x7cb37cb2, (int) 0x7cb17cae, 
        (int) 0x7cb97cbd, (int) 0x7cc07cc5, 
        (int) 0x7cc27cd8, (int) 0x7cd27cdc, 
        (int) 0x7ce29b3b, (int) 0x7cef7cf2, 
        (int) 0x7cf47cf6, (int) 0x7cfa7d06, 
        (int) 0x7d027d1c, (int) 0x7d157d0a, 
        (int) 0x7d457d4b, (int) 0x7d2e7d32, 
        (int) 0x7d3f7d35, (int) 0x7d467d73, 
        (int) 0x7d567d4e, (int) 0x7d727d68, 
        (int) 0x7d6e7d4f, (int) 0x7d637d93, 
        (int) 0x7d897d5b, (int) 0x7d8f7d7d, 
        (int) 0x7d9b7dba, (int) 0x7dae7da3, 
        (int) 0x7db57dc7, (int) 0x7dbd7dab, 
        (int) 0x7e3d7da2, (int) 0x7daf7ddc, 
        (int) 0x7db87d9f, (int) 0x7db07dd8, 
        (int) 0x7ddd7de4, (int) 0x7dde7dfb, 
        (int) 0x7df27de1, (int) 0x7e057e0a, 
        (int) 0x7e237e21, (int) 0x7e127e31, 
        (int) 0x7e1f7e09, (int) 0x7e0b7e22, 
        (int) 0x7e467e66, (int) 0x7e3b7e35, 
        (int) 0x7e397e43, (int) 0x7e377e32, 
        (int) 0x7e3a7e67, (int) 0x7e5d7e56, 
        (int) 0x7e5e7e59, (int) 0x7e5a7e79, 
        (int) 0x7e6a7e69, (int) 0x7e7c7e7b, 
        (int) 0x7e837dd5, (int) 0x7e7d8fae, 
        (int) 0x7e7f7e88, (int) 0x7e897e8c, 
        (int) 0x7e927e90, (int) 0x7e937e94, 
        (int) 0x7e967e8e, (int) 0x7e9b7e9c, 
        (int) 0x7f387f3a, (int) 0x7f457f4c, 
        (int) 0x7f4d7f4e, (int) 0x7f507f51, 
        (int) 0x7f557f54, (int) 0x7f587f5f, 
        (int) 0x7f607f68, (int) 0x7f697f67, 
        (int) 0x7f787f82, (int) 0x7f867f83, 
        (int) 0x7f887f87, (int) 0x7f8c7f94, 
        (int) 0x7f9e7f9d, (int) 0x7f9a7fa3, 
        (int) 0x7faf7fb2, (int) 0x7fb97fae, 
        (int) 0x7fb67fb8, (int) 0x8b717fc5, 
        (int) 0x7fc67fca, (int) 0x7fd57fd4, 
        (int) 0x7fe17fe6, (int) 0x7fe97ff3, 
        (int) 0x7ff998dc, (int) 0x80068004, 
        (int) 0x800b8012, (int) 0x80188019, 
        (int) 0x801c8021, (int) 0x8028803f, 
        (int) 0x803b804a, (int) 0x80468052, 
        (int) 0x8058805a, (int) 0x805f8062, 
        (int) 0x80688073, (int) 0x80728070, 
        (int) 0x80768079, (int) 0x807d807f, 
        (int) 0x80848086, (int) 0x8085809b, 
        (int) 0x8093809a, (int) 0x80ad5190, 
        (int) 0x80ac80db, (int) 0x80e580d9, 
        (int) 0x80dd80c4, (int) 0x80da80d6, 
        (int) 0x810980ef, (int) 0x80f1811b, 
        (int) 0x81298123, (int) 0x812f814b, 
        (int) 0x968b8146, (int) 0x813e8153, 
        (int) 0x815180fc, (int) 0x8171816e, 
        (int) 0x81658166, (int) 0x81748183, 
        (int) 0x8188818a, (int) 0x81808182, 
        (int) 0x81a08195, (int) 0x81a481a3, 
        (int) 0x815f8193, (int) 0x81a981b0, 
        (int) 0x81b581be, (int) 0x81b881bd, 
        (int) 0x81c081c2, (int) 0x81ba81c9, 
        (int) 0x81cd81d1, (int) 0x81d981d8, 
        (int) 0x81c881da, (int) 0x81df81e0, 
        (int) 0x81e781fa, (int) 0x81fb81fe, 
        (int) 0x82018202, (int) 0x82058207, 
        (int) 0x820a820d, (int) 0x82108216, 
        (int) 0x8229822b, (int) 0x82388233, 
        (int) 0x82408259, (int) 0x8258825d, 
        (int) 0x825a825f, (int) 0x82648262, 
        (int) 0x8268826a, (int) 0x826b822e, 
        (int) 0x82718277, (int) 0x8278827e, 
        (int) 0x828d8292, (int) 0x82ab829f, 
        (int) 0x82bb82ac, (int) 0x82e182e3, 
        (int) 0x82df82d2, (int) 0x82f482f3, 
        (int) 0x82fa8393, (int) 0x830382fb, 
        (int) 0x82f982de, (int) 0x830682dc, 
        (int) 0x830982d9, (int) 0x83358334, 
        (int) 0x83168332, (int) 0x83318340, 
        (int) 0x83398350, (int) 0x8345832f, 
        (int) 0x832b8317, (int) 0x83188385, 
        (int) 0x839a83aa, (int) 0x839f83a2, 
        (int) 0x83968323, (int) 0x838e8387, 
        (int) 0x838a837c, (int) 0x83b58373, 
        (int) 0x837583a0, (int) 0x838983a8, 
        (int) 0x83f48413, (int) 0x83eb83ce, 
        (int) 0x83fd8403, (int) 0x83d8840b, 
        (int) 0x83c183f7, (int) 0x840783e0, 
        (int) 0x83f2840d, (int) 0x84228420, 
        (int) 0x83bd8438, (int) 0x850683fb, 
        (int) 0x846d842a, (int) 0x843c855a, 
        (int) 0x84848477, (int) 0x846b84ad, 
        (int) 0x846e8482, (int) 0x84698446, 
        (int) 0x842c846f, (int) 0x84798435, 
        (int) 0x84ca8462, (int) 0x84b984bf, 
        (int) 0x849f84d9, (int) 0x84cd84bb, 
        (int) 0x84da84d0, (int) 0x84c184c6, 
        (int) 0x84d684a1, (int) 0x852184ff, 
        (int) 0x84f48517, (int) 0x8518852c, 
        (int) 0x851f8515, (int) 0x851484fc, 
        (int) 0x85408563, (int) 0x85588548, 
        (int) 0x85418602, (int) 0x854b8555, 
        (int) 0x858085a4, (int) 0x85888591, 
        (int) 0x858a85a8, (int) 0x856d8594, 
        (int) 0x859b85ea, (int) 0x8587859c, 
        (int) 0x8577857e, (int) 0x859085c9, 
        (int) 0x85ba85cf, (int) 0x85b985d0, 
        (int) 0x85d585dd, (int) 0x85e585dc, 
        (int) 0x85f9860a, (int) 0x8613860b, 
        (int) 0x85fe85fa, (int) 0x86068622, 
        (int) 0x861a8630, (int) 0x863f864d, 
        (int) 0x4e558654, (int) 0x865f8667, 
        (int) 0x86718693, (int) 0x86a386a9, 
        (int) 0x86aa868b, (int) 0x868c86b6, 
        (int) 0x86af86c4, (int) 0x86c686b0, 
        (int) 0x86c98823, (int) 0x86ab86d4, 
        (int) 0x86de86e9, (int) 0x86ec86df, 
        (int) 0x86db86ef, (int) 0x87128706, 
        (int) 0x87088700, (int) 0x870386fb, 
        (int) 0x87118709, (int) 0x870d86f9, 
        (int) 0x870a8734, (int) 0x873f8737, 
        (int) 0x873b8725, (int) 0x8729871a, 
        (int) 0x8760875f, (int) 0x8778874c, 
        (int) 0x874e8774, (int) 0x87578768, 
        (int) 0x876e8759, (int) 0x87538763, 
        (int) 0x876a8805, (int) 0x87a2879f, 
        (int) 0x878287af, (int) 0x87cb87bd, 
        (int) 0x87c087d0, (int) 0x96d687ab, 
        (int) 0x87c487b3, (int) 0x87c787c6, 
        (int) 0x87bb87ef, (int) 0x87f287e0, 
        (int) 0x880f880d, (int) 0x87fe87f6, 
        (int) 0x87f7880e, (int) 0x87d28811, 
        (int) 0x88168815, (int) 0x88228821, 
        (int) 0x88318836, (int) 0x88398827, 
        (int) 0x883b8844, (int) 0x88428852, 
        (int) 0x8859885e, (int) 0x8862886b, 
        (int) 0x8881887e, (int) 0x889e8875, 
        (int) 0x887d88b5, (int) 0x88728882, 
        (int) 0x88978892, (int) 0x88ae8899, 
        (int) 0x88a2888d, (int) 0x88a488b0, 
        (int) 0x88bf88b1, (int) 0x88c388c4, 
        (int) 0x88d488d8, (int) 0x88d988dd, 
        (int) 0x88f98902, (int) 0x88fc88f4, 
        (int) 0x88e888f2, (int) 0x8904890c, 
        (int) 0x890a8913, (int) 0x8943891e, 
        (int) 0x8925892a, (int) 0x892b8941, 
        (int) 0x8944893b, (int) 0x89368938, 
        (int) 0x894c891d, (int) 0x8960895e, 
        (int) 0x89668964, (int) 0x896d896a, 
        (int) 0x896f8974, (int) 0x8977897e, 
        (int) 0x89838988, (int) 0x898a8993, 
        (int) 0x899889a1, (int) 0x89a989a6, 
        (int) 0x89ac89af, (int) 0x89b289ba, 
        (int) 0x89bd89bf, (int) 0x89c089da, 
        (int) 0x89dc89dd, (int) 0x89e789f4, 
        (int) 0x89f88a03, (int) 0x8a168a10, 
        (int) 0x8a0c8a1b, (int) 0x8a1d8a25, 
        (int) 0x8a368a41, (int) 0x8a5b8a52, 
        (int) 0x8a468a48, (int) 0x8a7c8a6d, 
        (int) 0x8a6c8a62, (int) 0x8a858a82, 
        (int) 0x8a848aa8, (int) 0x8aa18a91, 
        (int) 0x8aa58aa6, (int) 0x8a9a8aa3, 
        (int) 0x8ac48acd, (int) 0x8ac28ada, 
        (int) 0x8aeb8af3, (int) 0x8ae78ae4, 
        (int) 0x8af18b14, (int) 0x8ae08ae2, 
        (int) 0x8af78ade, (int) 0x8adb8b0c, 
        (int) 0x8b078b1a, (int) 0x8ae18b16, 
        (int) 0x8b108b17, (int) 0x8b208b33, 
        (int) 0x97ab8b26, (int) 0x8b2b8b3e, 
        (int) 0x8b288b41, (int) 0x8b4c8b4f, 
        (int) 0x8b4e8b49, (int) 0x8b568b5b, 
        (int) 0x8b5a8b6b, (int) 0x8b5f8b6c, 
        (int) 0x8b6f8b74, (int) 0x8b7d8b80, 
        (int) 0x8b8c8b8e, (int) 0x8b928b93, 
        (int) 0x8b968b99, (int) 0x8b9a8c3a, 
        (int) 0x8c418c3f, (int) 0x8c488c4c, 
        (int) 0x8c4e8c50, (int) 0x8c558c62, 
        (int) 0x8c6c8c78, (int) 0x8c7a8c82, 
        (int) 0x8c898c85, (int) 0x8c8a8c8d, 
        (int) 0x8c8e8c94, (int) 0x8c7c8c98, 
        (int) 0x621d8cad, (int) 0x8caa8cbd, 
        (int) 0x8cb28cb3, (int) 0x8cae8cb6, 
        (int) 0x8cc88cc1, (int) 0x8ce48ce3, 
        (int) 0x8cda8cfd, (int) 0x8cfa8cfb, 
        (int) 0x8d048d05, (int) 0x8d0a8d07, 
        (int) 0x8d0f8d0d, (int) 0x8d109f4e, 
        (int) 0x8d138ccd, (int) 0x8d148d16, 
        (int) 0x8d678d6d, (int) 0x8d718d73, 
        (int) 0x8d818d99, (int) 0x8dc28dbe, 
        (int) 0x8dba8dcf, (int) 0x8dda8dd6, 
        (int) 0x8dcc8ddb, (int) 0x8dcb8dea, 
        (int) 0x8deb8ddf, (int) 0x8de38dfc, 
        (int) 0x8e088e09, (int) 0x8dff8e1d, 
        (int) 0x8e1e8e10, (int) 0x8e1f8e42, 
        (int) 0x8e358e30, (int) 0x8e348e4a, 
        (int) 0x8e478e49, (int) 0x8e4c8e50, 
        (int) 0x8e488e59, (int) 0x8e648e60, 
        (int) 0x8e2a8e63, (int) 0x8e558e76, 
        (int) 0x8e728e7c, (int) 0x8e818e87, 
        (int) 0x8e858e84, (int) 0x8e8b8e8a, 
        (int) 0x8e938e91, (int) 0x8e948e99, 
        (int) 0x8eaa8ea1, (int) 0x8eac8eb0, 
        (int) 0x8ec68eb1, (int) 0x8ebe8ec5, 
        (int) 0x8ec88ecb, (int) 0x8edb8ee3, 
        (int) 0x8efc8efb, (int) 0x8eeb8efe, 
        (int) 0x8f0a8f05, (int) 0x8f158f12, 
        (int) 0x8f198f13, (int) 0x8f1c8f1f, 
        (int) 0x8f1b8f0c, (int) 0x8f268f33, 
        (int) 0x8f3b8f39, (int) 0x8f458f42, 
        (int) 0x8f3e8f4c, (int) 0x8f498f46, 
        (int) 0x8f4e8f57, (int) 0x8f5c8f62, 
        (int) 0x8f638f64, (int) 0x8f9c8f9f, 
        (int) 0x8fa38fad, (int) 0x8faf8fb7, 
        (int) 0x8fda8fe5, (int) 0x8fe28fea, 
        (int) 0x8fef9087, (int) 0x8ff49005, 
        (int) 0x8ff98ffa, (int) 0x90119015, 
        (int) 0x9021900d, (int) 0x901e9016, 
        (int) 0x900b9027, (int) 0x90369035, 
        (int) 0x90398ff8, (int) 0x904f9050, 
        (int) 0x90519052, (int) 0x900e9049, 
        (int) 0x903e9056, (int) 0x9058905e, 
        (int) 0x9068906f, (int) 0x907696a8, 
        (int) 0x90729082, (int) 0x907d9081, 
        (int) 0x9080908a, (int) 0x9089908f, 
        (int) 0x90a890af, (int) 0x90b190b5, 
        (int) 0x90e290e4, (int) 0x624890db, 
        (int) 0x91029112, (int) 0x91199132, 
        (int) 0x9130914a, (int) 0x91569158, 
        (int) 0x91639165, (int) 0x91699173, 
        (int) 0x9172918b, (int) 0x91899182, 
        (int) 0x91a291ab, (int) 0x91af91aa, 
        (int) 0x91b591b4, (int) 0x91ba91c0, 
        (int) 0x91c191c9, (int) 0x91cb91d0, 
        (int) 0x91d691df, (int) 0x91e191db, 
        (int) 0x91fc91f5, (int) 0x91f6921e, 
        (int) 0x91ff9214, (int) 0x922c9215, 
        (int) 0x9211925e, (int) 0x92579245, 
        (int) 0x92499264, (int) 0x92489295, 
        (int) 0x923f924b, (int) 0x9250929c, 
        (int) 0x92969293, (int) 0x929b925a, 
        (int) 0x92cf92b9, (int) 0x92b792e9, 
        (int) 0x930f92fa, (int) 0x9344932e, 
        (int) 0x93199322, (int) 0x931a9323, 
        (int) 0x933a9335, (int) 0x933b935c, 
        (int) 0x9360937c, (int) 0x936e9356, 
        (int) 0x93b093ac, (int) 0x93ad9394, 
        (int) 0x93b993d6, (int) 0x93d793e8, 
        (int) 0x93e593d8, (int) 0x93c393dd, 
        (int) 0x93d093c8, (int) 0x93e4941a, 
        (int) 0x94149413, (int) 0x94039407, 
        (int) 0x94109436, (int) 0x942b9435, 
        (int) 0x9421943a, (int) 0x94419452, 
        (int) 0x9444945b, (int) 0x94609462, 
        (int) 0x945e946a, (int) 0x92299470, 
        (int) 0x94759477, (int) 0x947d945a, 
        (int) 0x947c947e, (int) 0x9481947f, 
        (int) 0x95829587, (int) 0x958a9594, 
        (int) 0x95969598, (int) 0x959995a0, 
        (int) 0x95a895a7, (int) 0x95ad95bc, 
        (int) 0x95bb95b9, (int) 0x95be95ca, 
        (int) 0x6ff695c3, (int) 0x95cd95cc, 
        (int) 0x95d595d4, (int) 0x95d695dc, 
        (int) 0x95e195e5, (int) 0x95e29621, 
        (int) 0x9628962e, (int) 0x962f9642, 
        (int) 0x964c964f, (int) 0x964b9677, 
        (int) 0x965c965e, (int) 0x965d965f, 
        (int) 0x96669672, (int) 0x966c968d, 
        (int) 0x96989695, (int) 0x969796aa, 
        (int) 0x96a796b1, (int) 0x96b296b0, 
        (int) 0x96b496b6, (int) 0x96b896b9, 
        (int) 0x96ce96cb, (int) 0x96c996cd, 
        (int) 0x894d96dc, (int) 0x970d96d5, 
        (int) 0x96f99704, (int) 0x97069708, 
        (int) 0x9713970e, (int) 0x9711970f, 
        (int) 0x97169719, (int) 0x9724972a, 
        (int) 0x97309739, (int) 0x973d973e, 
        (int) 0x97449746, (int) 0x97489742, 
        (int) 0x9749975c, (int) 0x97609764, 
        (int) 0x97669768, (int) 0x52d2976b, 
        (int) 0x97719779, (int) 0x9785977c, 
        (int) 0x9781977a, (int) 0x9786978b, 
        (int) 0x978f9790, (int) 0x979c97a8, 
        (int) 0x97a697a3, (int) 0x97b397b4, 
        (int) 0x97c397c6, (int) 0x97c897cb, 
        (int) 0x97dc97ed, (int) 0x9f4f97f2, 
        (int) 0x7adf97f6, (int) 0x97f5980f, 
        (int) 0x980c9838, (int) 0x98249821, 
        (int) 0x9837983d, (int) 0x9846984f, 
        (int) 0x984b986b, (int) 0x986f9870, 
        (int) 0x98719874, (int) 0x987398aa, 
        (int) 0x98af98b1, (int) 0x98b698c4, 
        (int) 0x98c398c6, (int) 0x98e998eb, 
        (int) 0x99039909, (int) 0x99129914, 
        (int) 0x99189921, (int) 0x991d991e, 
        (int) 0x99249920, (int) 0x992c992e, 
        (int) 0x993d993e, (int) 0x99429949, 
        (int) 0x99459950, (int) 0x994b9951, 
        (int) 0x9952994c, (int) 0x99559997, 
        (int) 0x999899a5, (int) 0x99ad99ae, 
        (int) 0x99bc99df, (int) 0x99db99dd, 
        (int) 0x99d899d1, (int) 0x99ed99ee, 
        (int) 0x99f199f2, (int) 0x99fb99f8, 
        (int) 0x9a019a0f, (int) 0x9a0599e2, 
        (int) 0x9a199a2b, (int) 0x9a379a45, 
        (int) 0x9a429a40, (int) 0x9a439a3e, 
        (int) 0x9a559a4d, (int) 0x9a5b9a57, 
        (int) 0x9a5f9a62, (int) 0x9a659a64, 
        (int) 0x9a699a6b, (int) 0x9a6a9aad, 
        (int) 0x9ab09abc, (int) 0x9ac09acf, 
        (int) 0x9ad19ad3, (int) 0x9ad49ade, 
        (int) 0x9adf9ae2, (int) 0x9ae39ae6, 
        (int) 0x9aef9aeb, (int) 0x9aee9af4, 
        (int) 0x9af19af7, (int) 0x9afb9b06, 
        (int) 0x9b189b1a, (int) 0x9b1f9b22, 
        (int) 0x9b239b25, (int) 0x9b279b28, 
        (int) 0x9b299b2a, (int) 0x9b2e9b2f, 
        (int) 0x9b329b44, (int) 0x9b439b4f, 
        (int) 0x9b4d9b4e, (int) 0x9b519b58, 
        (int) 0x9b749b93, (int) 0x9b839b91, 
        (int) 0x9b969b97, (int) 0x9b9f9ba0, 
        (int) 0x9ba89bb4, (int) 0x9bc09bca, 
        (int) 0x9bb99bc6, (int) 0x9bcf9bd1, 
        (int) 0x9bd29be3, (int) 0x9be29be4, 
        (int) 0x9bd49be1, (int) 0x9c3a9bf2, 
        (int) 0x9bf19bf0, (int) 0x9c159c14, 
        (int) 0x9c099c13, (int) 0x9c0c9c06, 
        (int) 0x9c089c12, (int) 0x9c0a9c04, 
        (int) 0x9c2e9c1b, (int) 0x9c259c24, 
        (int) 0x9c219c30, (int) 0x9c479c32, 
        (int) 0x9c469c3e, (int) 0x9c5a9c60, 
        (int) 0x9c679c76, (int) 0x9c789ce7, 
        (int) 0x9cec9cf0, (int) 0x9d099d08, 
        (int) 0x9ceb9d03, (int) 0x9d069d2a, 
        (int) 0x9d269daf, (int) 0x9d239d1f, 
        (int) 0x9d449d15, (int) 0x9d129d41, 
        (int) 0x9d3f9d3e, (int) 0x9d469d48, 
        (int) 0x9d5d9d5e, (int) 0x9d649d51, 
        (int) 0x9d509d59, (int) 0x9d729d89, 
        (int) 0x9d879dab, (int) 0x9d6f9d7a, 
        (int) 0x9d9a9da4, (int) 0x9da99db2, 
        (int) 0x9dc49dc1, (int) 0x9dbb9db8, 
        (int) 0x9dba9dc6, (int) 0x9dcf9dc2, 
        (int) 0x9dd99dd3, (int) 0x9df89de6, 
        (int) 0x9ded9def, (int) 0x9dfd9e1a, 
        (int) 0x9e1b9e1e, (int) 0x9e759e79, 
        (int) 0x9e7d9e81, (int) 0x9e889e8b, 
        (int) 0x9e8c9e92, (int) 0x9e959e91, 
        (int) 0x9e9d9ea5, (int) 0x9ea99eb8, 
        (int) 0x9eaa9ead, (int) 0x97619ecc, 
        (int) 0x9ece9ecf, (int) 0x9ed09ed4, 
        (int) 0x9edc9ede, (int) 0x9edd9ee0, 
        (int) 0x9ee59ee8, (int) 0x9eef9ef4, 
        (int) 0x9ef69ef7, (int) 0x9ef99efb, 
        (int) 0x9efc9efd, (int) 0x9f079f08, 
        (int) 0x76b79f15, (int) 0x9f219f2c, 
        (int) 0x9f3e9f4a, (int) 0x9f529f54, 
        (int) 0x9f639f5f, (int) 0x9f609f61, 
        (int) 0x9f669f67, (int) 0x9f6c9f6a, 
        (int) 0x9f779f72, (int) 0x9f769f95, 
        (int) 0x9f9c9fa0, (int) 0x582f69c7, 
        (int) 0x90597464, (int) 0x51dc7199, 
        (int) 0x00000000, (int) 0x00000000
        /* 0x0000 - 0x3d0f */
    };
  
    /**
     *   コンストラクタ
     * 
     */
    public a2Butf8Conveter() 
    {
        // システムでサポートしているエンコードを調べる

        // EUCの場合にはチェックせず、自前の変換ルーチンを使用する。
        // (Nokia 702NKでは、EUCエンコードで文字列を変換するとメモリリークするため)
        // useSystemEUC = checkEncode("EUC-JP");
        useSystemSJ  = checkEncode("SJIS");
    }

    /**
     *  強制的に自前の漢字変換ロジックを利用するようにする
     *
     */
    public void forceSelfKanjiConvertor()
    {
        useSystemEUC = false;
        useSystemSJ = false;
    }

    /**
     *  半角カナ → 全角カナ変換が必要かチェックする
     * 
     * @return true:変換が必要, false:変換不要
     */
    public boolean isConvertHanZen()
    {
        // Shift JIS or EUC の変換ができる場合には半角カナのまま
        if ((useSystemSJ == true)||(useSystemEUC == true))
        {
            return (false);
        }
        return (true);
//        return (false);
    }

    /**
     * バイトデータをUTF8のString型へ変換する処理
     * 
     * @param   data バイトデータ
     * @return  UTF8変換済みのString型データ
     */
    public String parsefromSJ(byte[]data)
    {
        if (data == null)
        {
            return ("");
        }
        return (parsefromSJ(data, 0, data.length));
    }

    /**
     * バイトデータ(EUC)をUTF8のString型へ変換する処理
     * 
     * @param data    バイトデータ
     * @param offset  バイトデータの中で変換を開始する位置
     * @param length  変換するバイト数
     * @return        UTF8変換済みのString型データ
     */
    public String parsefromEUC(byte[] data, int offset, int length)
    {
        // 変換ロジックを選択する
        if (useSystemEUC == true)
        {
            // EUC-JPで変換して利用する
            return (parseToUtf8UsingEUC(data, offset, length));
        }
        else if (useSystemSJ == true)
        {
            // EUC-JPを一度SHIFT JISへ変換する
            byte[] tempBuffer = new byte[(length * 2) + MARGIN];
            StrCopyEUCtoSJ(tempBuffer, 0, data, offset, length);
            return (parseToUtf8UsingSJ(tempBuffer, 0, length));
        }

        // システムは UTF8をサポート、漢字コードをUTF8に変換する
        byte[] tempBuffer = new byte[(length * 2) + MARGIN];
        StrCopyEUCtoSJ(tempBuffer, 0, data, offset, length);
        return (parseToUtf8(tempBuffer, 0, length));
    }

    /**
     * バイトデータ(JIS)をUTF8のString型へ変換する処理
     * 
     * @param data    バイトデータ
     * @param offset  バイトデータの中で変換を開始する位置
     * @param length  変換するバイト数
     * @return        UTF8変換済みのString型データ
     */
    public String parsefromJIS(byte[] data, int offset, int length)
    {
        return (convertFromJISToUtf8(data, offset, length));
    }
    
    /**
     * バイトデータをUTF8のString型へ変換する処理
     * 
     * @param data    バイトデータ
     * @param offset  バイトデータの中で変換を開始する位置
     * @param length  変換するバイト数
     * @return        UTF8変換済みのString型データ
     */
    public String parsefromSJ(byte[] data, int offset, int length)
    {
        // 変換ロジックを選択する
        if (useSystemSJ == true)
        {
            // SHIFT JISをそのまま利用する
            return (parseToUtf8UsingSJ(data, offset, length));
        }
        else if (useSystemEUC == true)
        {
            // EUC-JPで変換して利用する
            return (parseToUtf8UsingEUC(data, offset, length));
        }

        // システムは UTF8をサポート、漢字コードをUTF8に変換する
        return (parseToUtf8(data, offset, length));
    }

    /**
     *  SHIFT-JIS => UTF8 の変換処理
     *  (Systemが SJ=>utf8をサポートしている場合)
     * 
     * @param data
     * @param offset
     * @param length
     * @return
     */
    private String parseToUtf8UsingSJ(byte[] data, int offset, int length)
    {
        String buffer = null;
        int index = 0;
        while ((index < length)&&(data[index + offset] != '\0'))
        {
            index++;
        }
        try
        {
            buffer = new String(data, offset, index, "SJIS");
        }
        catch (Exception e)
        {
            buffer = new String(data, offset, length);
        }        
        return (buffer);
    }

    /**
     *  SHIFT-JIS => UTF8 の変換処理 
     *  (Systemが euc -> utf8をサポートしている場合、SJ->eucを実施してからutf8変換を行う)
     * 
     * @param data
     * @param offset
     * @param length
     * @return
     */
    private String parseToUtf8UsingEUC(byte[] data, int offset, int length)
    {
        String buffer = null;
        byte[] tempBuffer = new byte[(length * 2) + MARGIN];
        int parsedLength  = StrCopySJtoEUC(tempBuffer, 0, data, offset, length);

        try
        {
            buffer = new String(tempBuffer, 0, parsedLength, "EUC-JP");
        }
        catch (Exception e)
        {
            buffer = new String(tempBuffer, 0, parsedLength);
        }
        tempBuffer = null;
        return (buffer);
    }
    
    /**
     *  SHIFT-JIS => UTF8 の変換処理 
     *  (自前の変換テーブルを使用する場合)
     * 
     * @param data
     * @param offset
     * @param length
     * @return
     */
    private String parseToUtf8(byte[] data, int offset, int length)
    {
        // データサイズは、data.lengthで取得可能...
        String buffer = null;
        int convertCount   = 0;
        int bufferIndex    = 0;
        int bufferSize     = (length * 2);
        byte[] tempBuffer = new byte[bufferSize + MARGIN];
        while ((data[convertCount + offset] != 0)&&(convertCount < length)&&(bufferIndex < bufferSize))
        {
            // 半角アルファベットコードはそのまま表示する
            if ((data[convertCount + offset] >= 0)&&(data[convertCount + offset] <= 0x80))
            {
/*
                // '\' の対応。
                if (data[convertCount + offset] == 0x5c)
                {
                    tempBuffer[bufferIndex] = data[convertCount + offset];
                    bufferIndex++;
                    convertCount++;
                    continue;
                }                
*/

                // 制御コードは表示しない
                //if (data[convertCount + offset] >= 0x20)
                {
                    tempBuffer[bufferIndex] = data[convertCount + offset];
                    bufferIndex++;
                    convertCount++;
                }
                continue;
            }
            
            // 文字が '～' だった場合...
            if ((data[convertCount + offset] == (byte) 0x81)&&(data[convertCount + offset + 1] == (byte) 0x60))
            {
                // '～' を無理やり変換。
                int codes = 0xff5e;
//                int codes = 0x007e;
                int temp = 0;
                // UCS-4 > UTF-8 3byte
                temp = (((codes >> 12) & 0x0000000f) | 0xe0);        
                tempBuffer[bufferIndex + 0] = (byte) temp;

                temp = (((codes >> 6) & 0x0000003f)| 0x80);
                tempBuffer[bufferIndex + 1] = (byte) temp;

                temp = (((codes) & 0x0000003f)|0x80);
                tempBuffer[bufferIndex + 2] = (byte) temp;
/**/

                convertCount = convertCount + 2;
                bufferIndex  = bufferIndex + 3;
                continue;
            }            

            // 文字が半角カナだった場合...
            if ((data[convertCount + offset] >= (byte) 0xa1)&&(data[convertCount + offset] <= (byte) 0xdf))
            {
/**
                tempBuffer[bufferIndex + 0] = (byte) 0xef;
                if (data[convertCount + offset] < 0xc0)
                {
                    tempBuffer[bufferIndex + 1] = (byte) 0xbd;
                    tempBuffer[bufferIndex + 2] = (byte) data[convertCount + offset];
                }
                else
                {
                    tempBuffer[bufferIndex + 1] = (byte) 0xbe;
                    tempBuffer[bufferIndex + 2] = (byte) (((byte) data[convertCount + offset]) - 0xc0 + 0x80);
                }
**/
/**/    
//                   int codes = 0xff00 + (data[convertCount + offset] - 0xa1 + 61);
//                int codes = 0x0000 + (data[convertCount + offset] - 0xa1 + 61);

                // 半角カナは全角カナに変換する
                int codes = 0x00;
                int ch = data[convertCount + offset] - (byte) 0xa1;
                try
                {
                    codes = Han2ZenTable[ch];
                }
                catch (Exception e)
                {
                    codes = 0x3000;
                }
//                int codes = data[convertCount + offset] - 0xa1 + 0x61;

                int temp = 0;
                // UCS-4 > UTF-8 3byte
                temp = (((codes >> 12) & 0x0000000f) | 0xe0);        
                tempBuffer[bufferIndex + 0] = (byte) temp;

                temp = (((codes >> 6) & 0x0000003f)| 0x80);
                tempBuffer[bufferIndex + 1] = (byte) temp;

                temp = (((codes) & 0x0000003f)|0x80);
                tempBuffer[bufferIndex + 2] = (byte) temp;
/**/

                convertCount = convertCount + 1;
                bufferIndex  = bufferIndex + 3;
                continue;
            }

            // 漢字コードは、一度 SJ -> JIS変換
            int  upper, lower;
            
            // 上位8ビット/下位8ビットを変数にコピーする
            upper = (int) data[convertCount + offset];
            lower = (int) data[convertCount + offset + 1];
            
            // byte -> int 変換で値変換
            if (upper < 0)
            {
                upper = 256 + upper;
            }
            if (lower < 0)
            {
                lower = 256 + lower;
            }
    
            if (upper <= 0x9f)
            {
                if (lower < 0x9f)
                {
                    upper = (upper << 1) - 0xe1;
                }
                else
                {
                    upper = (upper << 1) - 0xe0;
                }
            }
            else
            {
                if (lower < 0x9f)
                {
                    upper = ((upper - 0xb0) << 1) - 1;
                }
                else
                {
                    upper = ((upper - 0xb0) << 1);
                }
            }
            if (lower < 0x7f)
            {
                lower = lower - 0x1f;
            }
            else
            {
                if (lower < 0x9f)
                {
                    lower = lower - 0x20;
                }
                else
                {
                    lower = lower - 0x7e;
                }
            }

            // UTF8変換を実施する...
            upper = upper & 0x000000ff;
            lower = lower & 0x000000ff;
            int codes = Jisx0208ToUnicode11((byte)upper, (byte)lower);
            int temp;
            
            // UCS-4 > UTF-8 3byte
            temp = (((codes >> 12) & 0x0000000f) | 0xe0);        
            tempBuffer[bufferIndex + 0] = (byte) temp;

            temp = (((codes >> 6) & 0x0000003f)| 0x80);
            tempBuffer[bufferIndex + 1] = (byte) temp;

            temp = (((codes) & 0x0000003f)|0x80);
            tempBuffer[bufferIndex + 2] = (byte) temp;
            
            convertCount = convertCount + 2;
            bufferIndex  = bufferIndex + 3;
            
        }
/**
        tempBuffer[bufferIndex] = 0x00;
        bufferIndex++;
**/
        try
        {
            buffer = new String(tempBuffer, 0, bufferIndex, "UTF-8");
        }
        catch (Exception e)
        {
            buffer = new String(tempBuffer);
        }
        tempBuffer = null;
        return (buffer);
    }

    /**
     * 指定されたエンコードがサポートされているかチェックする
     * 
     * @param enc チェックするエンコード
     * @return    true:変換サポート, false:変換未サポート
     */
    private boolean checkEncode(String enc)
    {
        try
        {
            // ためしに漢字を変換してみる...
            String text = "漢";
            text.getBytes(enc);
            return (true);
        }
        catch (Exception e)
        {
            // 変換はサポートしていなかった！！
        }
        return (false);
    }

    /**
     *=========================================================================*
     *   Function : ConvertSJtoEUC                                             *
     *                        漢字コードをSHIFT JISからEUCに変換する(NNsiより) *
     *=========================================================================*
     **/
    public int StrCopySJtoEUC(byte[] dst, int dstOffset, byte[] src, int srcOffset, int length)
    {
        int srcIndex = 0;
        int dstIndex = 0;
        
        while ((srcIndex < length)&&(src[srcIndex + srcOffset] != '\0'))
        {
            int checkChar;
            if (src[srcIndex + srcOffset] < 0)
            {
                checkChar = 256 + src[srcIndex + srcOffset];
            }
            else
            {
                checkChar = src[srcIndex + srcOffset];
            }
            
            // 半角アルファベットコードはそのまま表示する
            if (checkChar <= 0x80)
            {
                dst[dstIndex + dstOffset] = src[srcIndex + srcOffset];
                dstIndex++;
                srcIndex++;
                continue;
            }

            // 半角カナコードはEUC漢字コードに変換する
            if ((checkChar >= 0xa1)&&(checkChar <= 0xdf))
            {
                dst[dstIndex + dstOffset] = (byte) 0x8e;
                dstIndex++;
                dst[dstIndex + dstOffset] = src[srcIndex + srcOffset];
                dstIndex++;
                srcIndex++;
                continue;
            }

            // JIS漢字コードへ変換する処理...
            int  upper, lower;

            // 上位8ビット/下位8ビットを変数にコピーする
            upper = checkChar;
            if (src[srcIndex + srcOffset + 1] < 0)
            {
                lower = 256 + src[srcIndex + srcOffset + 1];
            }
            else
            {
                lower = src[srcIndex + srcOffset + 1];
            }

            if (upper <= 0x9f)
            {
                if (lower < 0x9f)
                {
                    upper = (upper << 1) - 0xe1;
                }
                else
                {
                    upper = (upper << 1) - 0xe0;
                }
            }
            else
            {
                if (lower < 0x9f)
                {
                    upper = ((upper - 0xb0) << 1) - 1;
                }
                else
                {
                    upper = ((upper - 0xb0) << 1);
                }
            }
            if (lower < 0x7f)
            {
                lower = lower - 0x1f;
            }
            else
            {
                if (lower < 0x9f)
                {
                    lower = lower - 0x20;
                }
                else
                {
                    lower = lower - 0x7e;
                }
            }

            // JIS -> EUCの漢字コード変換を実施...
            upper = (upper & 0x000000ff)|(0x80);
            lower = (lower & 0x000000ff)|(0x80);
            
            dst[dstIndex + dstOffset] = (byte) upper;
            dstIndex++;
            dst[dstIndex + dstOffset] = (byte) lower;
            dstIndex++;
            srcIndex = srcIndex + 2;
        }
        dst[dstIndex + dstOffset] = 0;
        dstIndex++;
        return (dstIndex);
    }

    /**
     * JIS X0208のコードををUNICODEに変換する...
     * 
     * @param  upper
     * @param  lower
     * @return UNICODE
     */
    private int Jisx0208ToUnicode11(byte upper, byte lower)
    {

        if (upper == 0x8e)
        {
            // 半角カナコード！
        }
        
        int code = 0x0000;
        if ((0x21 <= upper)&&(upper <= 0x7e)&&(0x21 <= lower)&&(lower <= 0x7e))
        {
            int num, mod, data;

            num  =  (upper - 0x21);
            num  = num * 0x005e;
            num  = num + (lower - 0x21);

            if (num < 0x00003d00)
            {            
                mod  = num % 2;
                num  = num / 2;
                if (num >= Jis2Utf8Table.length)
                {
                    // テーブルオーバフロー!! とりあえず値を返す
                    code  = 0x0000203d;
                    return (code);
                }
                data = Jis2Utf8Table[num];
                if (mod == 0)
                {
                    code = (((data & 0xffff0000) >>> 16) & 0x0000ffff);                
                }
                else
                {
                    code = (data & 0x0000ffff);
                }
            }
            else
            {
                code  = 0x0000203c;
            }
        }
        else
        {
            code = 0x0000203b;
        }
        return (code);
    }

    /*=========================================================================*/
    /*   Function : StrCopySJ                                                  */
    /*                          EUC漢字コードをSHIFT JISに変換して文字列コピー */
    /*=========================================================================*/
    public void StrCopyEUCtoSJ(byte[] dst, int dstOffset, byte[] src, int srcOffset, int length)
    {
        int dstIndex  = 0;
        int srcIndex  = 0;
        int matchData = 0;
        while ((srcIndex < length)&&((dstIndex + dstOffset) < dst.length)&&(src[srcOffset + srcIndex] != 0))
        {
            matchData = src[srcOffset + srcIndex];
            if (matchData < 0)
            {
                matchData = matchData + 256;
            }
            
            if (matchData == 0x8e)
            {
                // 半角カナコード
                srcIndex++;
                dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
                dstIndex++;
                continue;
            }
            if ((matchData >= ((0x80)|(0x21)))&&(matchData <= ((0x80)|(0x7e))))
            {
                // EUC漢字コードと判定、JIS漢字コードに一度変換してからSHIFT JISに変換
                srcIndex++;
                byte[] temp = new byte[2];
                temp[0] = (byte) ((matchData)&(0x7f));
                int data = src[srcOffset + srcIndex];
                srcIndex++;
                if (data < 0)
                {
                    data = data + 256;
                }
                temp[1] = (byte) ((data)&(0x7f));
                ConvertJIStoSJ(dst, (dstOffset + dstIndex), temp, 0);
                dstIndex = dstIndex + 2;
                continue;
            }
            dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
            dstIndex++;
            srcIndex++;
        }
        if (((srcIndex + srcOffset) < src.length)&&((dstIndex + dstOffset) < dst.length))
        {
            dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
        }
        return;
    }

    /*=========================================================================*/
    /*   Function : ConvertJIStoSJ                                             */
    /*                            JIS漢字コードをSHIFT JIS漢字コードに変換する */
    /*=========================================================================*/
    private void ConvertJIStoSJ(byte[] dst, int dstOffset, byte[] src, int srcOffset)
    {
        int  upper, lower;

        // 上位8ビット/下位8ビットを変数にコピーする
        upper = (int) src[srcOffset];
        lower = (int) src[srcOffset + 1];

        // byte -> int 変換で値変換
        if (upper < 0)
        {
            upper = 256 + upper;
        }
        if (lower < 0)
        {
            lower = 256 + lower;
        }

        // 奥村さんのC言語によるアルゴリズム辞典(p.110)のコードを使わせていただく
        if ((upper & 1) != 0)
        {
            if (lower < 0x60)
            {
                lower = lower + 0x1f;
            }
            else
            {
                lower = lower + 0x20;
            }
        }
        else
        {
            lower = lower + 0x7e;
        }   
        if (upper < 0x5f)
        {
            upper = (upper + 0xe1) >> 1;
        }
        else
        {
            upper = (((upper + 0x61) >> 1)|(0x80));
        }

        dst[dstOffset]     = (byte) (upper & (0xff));
        dst[dstOffset + 1] = (byte) (lower & (0xff));
        return;
    }
    
    /**
     *  JIS漢字コードをUTF8のString型へ変換する
     * @param data
     * @param offset
     * @param length
     * @return
     */
    private String convertFromJISToUtf8(byte[] data, int offset, int length)
    {
    	// データサイズは、data.lengthで取得可能...
    	boolean kanjiMode = false;
        String buffer = null;
        int convertCount   = 0;
        int bufferIndex    = 0;
        int bufferSize     = (length * 2);
        byte[] tempBuffer = new byte[bufferSize + MARGIN];
        while ((data[convertCount + offset] != 0)&&(convertCount < length)&&(bufferIndex < bufferSize))
        {
            if ((data[convertCount + offset] == (byte) 0x1b)&&(data[convertCount + offset + 1] == (byte) 0x24)&&(data[convertCount + offset + 2] == (byte) 0x42))
            {
                // 漢字モードに切り替え
                kanjiMode = true;
                convertCount = convertCount + 3;
                continue;
            }
            if ((data[convertCount + offset] == (byte) 0x1b)&&(data[convertCount + offset + 1]== (byte) 0x28)&&(data[convertCount + offset + 2] == (byte) 0x42))
            {
                // ANKモードに切り替え
                kanjiMode = false;
                convertCount = convertCount + 3;
                continue;
            }
        	
        	// ANKモードのときはそのまま表示する
            if (kanjiMode == false)
            {
                tempBuffer[bufferIndex] = data[convertCount + offset];
                bufferIndex++;
                convertCount++;
                continue;
            }
            
            // 文字が '～' だった場合...
            if ((data[convertCount + offset] == (byte) 0x21)&&(data[convertCount + offset + 1] == (byte) 0x41))
            {
                // '～' を無理やり変換。
                int codes = 0xff5e;
//                int codes = 0x007e;
                int temp = 0;
                // UCS-4 > UTF-8 3byte
                temp = (((codes >> 12) & 0x0000000f) | 0xe0);        
                tempBuffer[bufferIndex + 0] = (byte) temp;

                temp = (((codes >> 6) & 0x0000003f)| 0x80);
                tempBuffer[bufferIndex + 1] = (byte) temp;

                temp = (((codes) & 0x0000003f)|0x80);
                tempBuffer[bufferIndex + 2] = (byte) temp;
/**/

                convertCount = convertCount + 2;
                bufferIndex  = bufferIndex + 3;
                continue;
            }            
/*
            // 文字が半角カナだった場合...
            if ((data[convertCount + offset] >= (byte) 0xa1)&&(data[convertCount + offset] <= (byte) 0xdf))
            {
                // 半角カナは全角カナに変換する
                int codes = 0x00;
                int ch = data[convertCount + offset] - (byte) 0xa1;
                try
                {
                    codes = Han2ZenTable[ch];
                }
                catch (Exception e)
                {
                    codes = 0x3000;
                }

                int temp = 0;
                // UCS-4 > UTF-8 3byte
                temp = (((codes >> 12) & 0x0000000f) | 0xe0);        
                tempBuffer[bufferIndex + 0] = (byte) temp;

                temp = (((codes >> 6) & 0x0000003f)| 0x80);
                tempBuffer[bufferIndex + 1] = (byte) temp;

                temp = (((codes) & 0x0000003f)|0x80);
                tempBuffer[bufferIndex + 2] = (byte) temp;

                convertCount = convertCount + 1;
                bufferIndex  = bufferIndex + 3;
                continue;
            }
*/

            // UTF8変換を実施する...
            int upper = data[convertCount + offset] & 0x000000ff;
            int lower = data[convertCount + offset + 1] & 0x000000ff;
            int codes = Jisx0208ToUnicode11((byte)upper, (byte)lower);
            int temp;
            
            // UCS-4 > UTF-8 3byte
            temp = (((codes >> 12) & 0x0000000f) | 0xe0);        
            tempBuffer[bufferIndex + 0] = (byte) temp;

            temp = (((codes >> 6) & 0x0000003f)| 0x80);
            tempBuffer[bufferIndex + 1] = (byte) temp;

            temp = (((codes) & 0x0000003f)|0x80);
            tempBuffer[bufferIndex + 2] = (byte) temp;
            
            convertCount = convertCount + 2;
            bufferIndex  = bufferIndex + 3;
        }

        try
        {
            buffer = new String(tempBuffer, 0, bufferIndex, "UTF-8");
        }
        catch (Exception e)
        {
            buffer = new String(tempBuffer);
        }
        tempBuffer = null;
        return (buffer);
    }
}
