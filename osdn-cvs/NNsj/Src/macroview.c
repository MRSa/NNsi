/*============================================================================*
 *  FILE: 
 *     macroview.c
 *
 *  Description: 
 *     マクロデータの表示用加工処理
 *
 *===========================================================================*/
#define MACROVIEW_C
#include "local.h"

/*=========================================================================*/
/*   Function : setSourceDataDisplay                                       */
/*                                        ソースデータ内容をバッファに出力 */
/*=========================================================================*/
static UInt16 setSourceDataDisplay(Char *buffer, NNshMacroRecord *lineData)
{
    if ((lineData->src >= NNSH_MACROOP_GR_OFFSET)&&
         (lineData->src <= NNSH_MACROOP_GR_MAX))
    {
        StrCat (buffer, "gr");
        NUMCATI(buffer, lineData->src - NNSH_MACROOP_GR_OFFSET);
        StrCat (buffer, " ");
        return (NNSH_MACROOPCMD_DISABLE);
    }

    // 第３引数から値を取得する
    NUMCATI(buffer, lineData->MacroData.numData.operator);
    return (lineData->MacroData.numData.operator);
}

/*=========================================================================*/
/*   Function : setExecFuncDataDisplay                                     */
/*                                                        機能関数名の取得 */
/*=========================================================================*/
static void setExecFuncDataDisplay(Char *buffer, UInt16 command)
{
    Char        *ptr, **kw, buf[MAX_FUNCCHAR_BUF];
    MemHandle    memH;
    NNshWordList wordList;

    // バッファのクリア
    MemSet(buf, sizeof(buf), 0x00);
    MemSet(&wordList, sizeof(wordList), 0x00);
    
    // 一覧機能リストの取得（ストリングリソースを取得）
    memH = DmGetResource('tSTR', MULTIBTN_FEATUREMSG_STRINGS);
    if (memH == 0)
    {
        return;
    }
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        return;
    }

    // キーワードを切り出して、一時バッファにコピーする
    SeparateWordList(ptr, &wordList);
    kw = MemHandleLock(wordList.wordmemH);
    StrNCopy(buf, kw[command], MAX_FUNCCHAR_BUF - 1);
    MemHandleUnlock(wordList.wordmemH);
    ReleaseWordList(&wordList);
    
    // 文字列をコピー
    StrCat(buffer, buf);

    MemHandleUnlock(memH);
    DmReleaseResource(memH);
    return;
}

/*=========================================================================*/
/*   Function : printMacroOpeR                                             */
/*                                              マクロデータのデータ設定部 */
/*=========================================================================*/
static void printMacroOpeR(Char *buf, NNshMacroRecord *data, Char *operator)
{
    // NNsi設定値の取得/設定
    switch (data->MacroData.numData.subCommand)
    {
      case NNSH_MACROOPCMD_AUTOSTARTMACRO:
        // マクロ自動実行の値を取得
        StrCat(buf, " (autoMacro) ");
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
        // 新着確認実行後に回線切断
        StrCat(buf, " (chkLINE) ");
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_DA:
        // 新着確認実行後にDA起動
        StrCat(buf, " (chkDA) ");
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
        // 参照専用ログ取得レベル
        StrCat(buf, " (ROlogLvl) ");
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
        // 新着確認終了後にビープ音
        StrCat(buf, " (chkBEEP) ");
        break;

      case NNSH_MACROOPCMD_COPY_AND_DELETE:
        // 参照専用ログコピー後に削除も実施
        StrCat(buf, " (cp&rm) ");
        break;

      default:
        StrCat(buf, "nop ");
        break;
    }
    // 後データを出力
    if (operator != NULL)
    {
        StrCat(buf, operator);
    }
    return;
}

/*=========================================================================*/
/*   Function : printMacroOpeC                                             */
/*                                              マクロデータの分岐部(展開) */
/*=========================================================================*/
static void printMacroOpeC(Char *buf, NNshMacroRecord *data, Char *operator)
{
    if (operator != NULL)
    {
        StrCat (buf, "IF gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, operator);
        StrCat (buf, " THEN ");
    }
    StrCat (buf, "GOTO ");
    setSourceDataDisplay(buf, data);

    return;
}

/*=========================================================================*/
/*   Function : printMacroDst                                              */
/*                                            マクロデータの格納場所表示部 */
/*=========================================================================*/
static void printMacroDst(Char *buf, NNshMacroRecord *data, Char *operator)
{
    StrCat (buf, "gr");
    NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
    StrCat (buf, " := ");
    if (operator != NULL)
    {
        StrCat (buf, operator);
    }
    return;
}

/*=========================================================================*/
/*   Function : printMacroOpeA                                             */
/*                                          マクロデータの算術演算部(展開) */
/*=========================================================================*/
static void printMacroOpeA(Char *buf, NNshMacroRecord *data, Char *operator)
{
    printMacroDst(buf, data, NULL);
    if (operator != NULL)
    {
        // 演算
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, operator);
    }
    setSourceDataDisplay(buf, data);

    return;
}

/*=========================================================================*/
/*   Function : PrintMacroLine                                             */
/*                                          マクロデータ(一行)のデータ設定 */
/*=========================================================================*/
void PrintMacroLine(Char *buf, UInt16 size, UInt16 num, NNshMacroRecord *data)
{
    Char   logData[8];
    UInt16 command;

    // 領域の初期化
    MemSet(buf, size, 0x00);
 
    // 行番号を表示
    NUMCATI(buf, num);
    StrCat (buf, "  ");

    // opコードの判定
    switch (data->opCode)
    {
      case NNSH_MACROCMD_XOR:
        // 値を XOR する
        printMacroOpeA(buf, data, " XOR ");
        break;

      case NNSH_MACROCMD_AND:
        // 値を AND する
        printMacroOpeA(buf, data, " AND ");
        break;

      case NNSH_MACROCMD_OR:
        // 値を OR する
        printMacroOpeA(buf, data, " OR ");
        break;

      case NNSH_MACROCMD_ADD:
        // 値を加算する
        printMacroOpeA(buf, data, " + ");
        break;

      case NNSH_MACROCMD_CMP:
      case NNSH_MACROCMD_SUB:
        // 値を加算する
        printMacroOpeA(buf, data, " - ");
        break;

      case NNSH_MACROCMD_MOVE:
        // 値を代入する
        printMacroOpeA(buf, data, NULL);
        break;

      case NNSH_MACROCMD_JNZ:
        // ゼロでなかったら分岐する
        printMacroOpeC(buf, data, "<> 0");
        break;

      case NNSH_MACROCMD_JZ:
        // ゼロだったら分岐する
        printMacroOpeC(buf, data, "= 0");
        break;

      case NNSH_MACROCMD_JMP:
        // 無条件に分岐する
        printMacroOpeC(buf, data, NULL);
        break;

      case NNSH_MACROCMD_EXECUTE:
        // 機能の実行
        StrCat (buf, "CALL (");
        command = setSourceDataDisplay(buf, data);
        if (command != NNSH_MACROOPCMD_DISABLE)
        {
            StrCat (buf, ":");
            setExecFuncDataDisplay(buf, command);
            StrCat (buf, " ");
        }
        StrCat (buf, ") ");
        break;

      case NNSH_MACROCMD_MESSAGE:
        // ログ出力
        StrCat (buf, "PRINT ");
        if ((data->src >= NNSH_MACROOP_GR_OFFSET)&&
            (data->src <= NNSH_MACROOP_GR_MAX))
        {
            // GRx を表示
            StrCat (buf, "(gr");
            NUMCATI(buf, data->src - NNSH_MACROOP_GR_OFFSET);
            StrCat (buf, ") ");
        }
        else
        {
            // 文字列を表示
            StrCat (buf, "\"");
            StrCat (buf, data->MacroData.strData.data);
            StrCat (buf, "\" ");
        }
        break;

      case NNSH_MACROCMD_BEEP:
        // ビープ音を鳴らす
        StrCat (buf, "BEEP ");
        setSourceDataDisplay(buf, data);
        StrCat (buf, "  ");
        break;

      case NNSH_MACROCMD_MACROEND:
        // マクロ終了
        StrCat (buf, "END ");
        break;

      case NNSH_MACROCMD_SETTABNUM:
        // タブ数を格納
        printMacroDst(buf, data, "(tabNum)");
        break;

      case NNSH_MACROCMD_STRSTR:
        // 指定した文字列がスレタイトルに含まれるか
        printMacroDst(buf, data, "strstr(title, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\")");
        break;            

      case NNSH_MACROCMD_STRCMP:
        // 指定した文字列がスレタイトルと一致するか
        printMacroDst(buf, data, "strcmp(title, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\")");
        break;            

      case NNSH_MACROCMD_OPERATE:
        // NNsi設定値の設定
        printMacroOpeR(buf, data, ":= ");
        setSourceDataDisplay(buf, data);
        StrCat(buf, ", save ");
        /** not break; **/

      case NNSH_MACROCMD_STATUS:
        // NNsi設定値の取得
        printMacroDst(buf, data, NULL);
        printMacroOpeR(buf, data, NULL);
        break;

      case NNSH_MACROCMD_GETMESNUM:
        // スレ数を反映
        printMacroDst(buf, data, "mesNum(title)");
        break;

      case NNSH_MACROCMD_MESSTATUS:
        // スレ状態を反映
        printMacroDst(buf, data, "mesStatus(title)");
        break;

      case NNSH_MACROCMD_BBSTYPE:
        // スレのBBSタイプを反映
        printMacroDst(buf, data, "bbsType(title)");
        break;

      case NNSH_MACROCMD_IMPORT:
        // NNsi設定インポート
        StrCat (buf, "IMPORT '");
        StrCat (buf, FILE_XML_NNSISET);
        if (setSourceDataDisplay(buf, data) == 0)
        {
            StrCat (buf, "' from VFS");
        }
        else
        {
            StrCat (buf, "' from Palm");
        }
        break;

      case NNSH_MACROCMD_EXPORT:
        // NNsi設定エクスポート
        StrCat (buf, "EXPORT '" FILE_XML_NNSISET "' ");
        break;

      case NNSH_MACROCMD_LAUNCHDA:
        // DA起動
        StrCat (buf, "LAUNCH '");
        logData[0] = ((0xff00 & data->MacroData.numData.data1) >> 8); 
        logData[1] = ((0x00ff & data->MacroData.numData.data1));
        logData[2] = ((0xff00 & data->MacroData.numData.data2) >> 8);
        logData[3] = ((0x00ff & data->MacroData.numData.data2));
        logData[4] = '\0';
        StrCat (buf, logData);            
        StrCat (buf, "' as DA");
        break;

      case NNSH_MACROCMD_CLIPCOPY:
        // クリップボードにコピー
        StrCat (buf, "COPY(clip, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\") ");
        break;

      case NNSH_MACROCMD_CLIPADD:
        // クリップボードに追加
        StrCat (buf, "APPEND(clip, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\") ");
        break;

      case NNSH_MACROCMD_CLIPINSTR:
        // クリップボードの文字列に含まれるか確認
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, " := strstr(clip, \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\") ");
        break;

      case NNSH_MACROCMD_TITLECLIP:
        // スレタイトルをクリップボードにコピー
        StrCat(buf, "COPY(clip, title)");
        break;

      case NNSH_MACROCMD_SET_FINDBBSSTR:
        // クリップボード文字列を板検索文字列にコピー
        StrCat(buf, "COPY(findBBS, clip)");
        break;

      case NNSH_MACROCMD_SET_FINDTHREADSTR:
        // クリップボード文字列をスレタイ検索文字列にコピー
        StrCat(buf, "COPY(findTHREAD, clip)");
        break;

      case NNSH_MACROCMD_UPDATE_RESNUM:
        // レス数を更新して格納
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, " := updateMesNum(title) ");
        break;

      case NNSH_MACROCMD_JPL:
        // 正の値ならジャンプする
        printMacroOpeC(buf, data, "> 0");
        break;

      case NNSH_MACROCMD_JMI:
        // 負の値ならジャンプする
        printMacroOpeC(buf, data, "< 0");
        break;

      case NNSH_MACROCMD_OUTPUTTITLE:
        // スレタイトルのログ出力
        StrCat(buf, "PRINT title");
        break;
        
      case NNSH_MACROCMD_OUTPUTCLIP:
        // クリップボードのログ出力
        StrCat(buf, "PRINT clip");
        break;

      case NNSH_MACROCMD_REMARK:
        // 注釈文
        StrCat(buf, "' ");
        StrCat (buf,  data->MacroData.strData.data);
        break;

      case NNSH_MACROCMD_PUSH:
        // スタックにデータPUSH
        StrCat (buf, "PUSH ");
        setSourceDataDisplay(buf, data);
        break;

      case NNSH_MACROCMD_POP:
        // スタックからデータPOP
        StrCat(buf, "POP gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        break;

      case NNSH_MACROCMD_GETRECNUM:
        // レコード番号を取得する
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, " := RecordIndex(title) ");
        break;

      case NNSH_MACROCMD_FINDTHREADSTRCOPY:
        // スレ検索文字列をクリップボードにコピー
        StrCat (buf, "COPY(clip, findTHREAD)");
        break;

      case NNSH_MACROCMD_INPUTDIALOG:
        // 文字列を入力する (1.05)
        StrCat (buf, "clip := INPUT \"");
        StrCat (buf, data->MacroData.strData.data);
        break;

      case NNSH_MACROCMD_OUTPUTDIALOG:
        // 文字列を出力する (1.05)
        StrCat (buf, "OUTPUT \"");
        StrCat (buf, data->MacroData.strData.data);
        StrCat (buf, "\"");
        break;

      case NNSH_MACROCMD_CONFIRMDIALOG:
        // クリップボードの文字列を表示し確認する (1.05)
        StrCat (buf, "gr");
        NUMCATI(buf, data->dst - NNSH_MACROOP_GR_OFFSET);
        StrCat (buf, " := CONFIRM (clip)");
        break;

      case NNSH_MACROCMD_MESATTRIBUTE:
        // スレ属性値を反映 (1.06)
        printMacroDst(buf, data, "mesAttribute(title)");
        break;

      case NNSH_MACROCMD_JUMPLAST:
        // スレの最下行ならジャンプする (1.06)
        StrCat(buf, "IF isLast(threadList) THEN GOTO ");
        setSourceDataDisplay(buf, data);
        break;

      default:
        // コマンドコード異常、なにも実行しない
        StrCat (buf, "  ");
        break;
    }
    return;
}
