/*============================================================================*
 *
 *  $Id: macroedit.c,v 1.6 2005/02/16 15:43:27 mrsa Exp $
 *
 *  FILE: 
 *     macroedit.c
 *
 *  Description: 
 *     NNsi Macro Setting
 *
 *===========================================================================*/
#define MACROEDIT_C
#include "local.h"

#ifdef USE_MACROEDIT
/*=========================================================================*/
/*   Function : NNsi_MacroDataEdit                                         */
/*                                                        マクロデータ設定 */
/*=========================================================================*/
void NNsi_MacroDataEdit(Char *macroDBName, UInt16 macroDBVersion)
{
#ifdef MACRO_TEST

    // テスト用のマクロデータを吐き出す
    Err              ret;
    UInt16           lp;
    NNshMacroRecord  lineData;
    DmOpenRef        dbRef;

    // NNsj (NNsiMacroEdit)がインストールされているか確認する    
    if (CheckInstalledResource_NNsh('appl', 'NNsj') != false)
    {
        // NNsjを起動する(NNsi終了後に起動)
        (void) WebBrowserCommand('NNsj', 0, 0, sysAppLaunchCmdNormalLaunch, 
                                  NULL, 0, NULL);
        return;
    }



    // マクロデータベースのオープン
    OpenDatabase_NNsh(macroDBName, macroDBVersion, &dbRef);
    if (dbRef == 0)
    {
        // エラー発生(デバッグ表示)
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh() ", " ", 0);
        return;
    }
    
    ////////////////////////////////////////////////////////////////////////
    lp = 0;
    GetDBCount_NNsh(dbRef, &lp);
    while (lp != 0)
    {
        //  すでにNNsi設定が記録されていた場合、全レコードを削除する
        (void) DeleteRecordIDX_NNsh(dbRef, (lp - 1));
        lp--;
    }
    ////////////////////////////////////////////////////////////////////////
    
    // 1:マクロ実行ログに文字列を出力
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "SAMPLE MACRO START", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 2:新着確認終了後にDA自動起動するの設定値をクリア(GRfに保存)
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GRf;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_DA;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 3:新着確認終了後にビープ音を鳴らす設定値をクリア(GReに保存)
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GRe;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_BEEP;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 4:新着確認終了後に回線切断する設定値をクリア(GRdに保存)
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GRd;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_LINE;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 5:'取得済み全て'へ移動
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_TO_GETALL;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 6: マクロ実行ログに文字列を出力
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "START NEWARRIVAL CHECK", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 7:新着確認を実行
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_NEWARRIVAL;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 8:マクロ実行ログに文字列を出力
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "END   NEWARRIVAL CHECK", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 9:参照ログ取得
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_LOGCHARGE;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 10:マクロ実行ログに文字列を出力
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "END   LOGCHARGE", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 11:回線切断
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_DISCONN;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 12:BT OFF
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_BT_OFF;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 13:文字列を表示
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    StrNCopy(lineData.MacroData.strData.data, "unread thread:", MAX_STRLEN);
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 14:タブ数を gr0 に セットする
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_SETTABNUM;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 15:GR0を表示
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MESSAGE;
    lineData.dst              = 0;
    lineData.src              = NNSH_MACROOP_GR0;
    lineData.MacroData.strData.operator = NNSH_MACROLOGLEVEL_INFO;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 16:GR0がゼロ(未読なし)だったら、18にジャンプする
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_JZ;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = 0;
    lineData.MacroData.strData.operator   = 18;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 17:未読ありへ移動
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXECUTE;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = MULTIBTN_FEATURE_TO_NOTREAD;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 18:NNsi設定をエクスポート
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_EXPORT;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 19:マクロ自動起動設定をOFFにする
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_AUTOSTARTMACRO;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 20:新着確認終了後にDA自動起動するの設定値を取得し、GRfに格納
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = NNSH_MACROOP_GRf;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_DA;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 21:新着確認終了後にビープ音を鳴らす設定値をGReから復旧
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = NNSH_MACROOP_GRe;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_BEEP;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 22:新着確認終了後に回線切断する設定値をGRdから復旧
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_OPERATE;
    lineData.dst              = NNSH_MACROOP_GR0;
    lineData.src              = NNSH_MACROOP_GRd;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = NNSH_MACROOPCMD_NEWARRIVAL_LINE;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 23:BEEP音を鳴らす
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_BEEP;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 1;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 24:マクロを終了
    lp++;
    MemSet(&lineData, sizeof(lineData), 0x00);
    lineData.seqNum           = lp;
    lineData.opCode           = NNSH_MACROCMD_MACROEND;
    lineData.dst              = 0;
    lineData.src              = 0;
    lineData.MacroData.numData.operator   = 0;    
    lineData.MacroData.numData.subCommand = 0;
    lineData.MacroData.numData.data1      = 0;
    lineData.MacroData.numData.data2      = 0;
    ret = EntryRecord_NNsh(dbRef, sizeof(NNshMacroRecord), &lineData);

    // 並べ替えを実施
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_UINT16, 0);

    // データベースのクローズ
    CloseDatabase_NNsh(dbRef);

    // "ダミーマクロを出力した" 表示を行う
    NNsh_InformMessage(ALTID_INFO, MSG_SAMPLEMACRO_EFFECTED, "", 0);

#else

    // "現在サポートしていません" 表示を行う
    NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);

#endif
    return;
}
#endif  // #ifdef USE_MACROEDIT
