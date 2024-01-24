/*============================================================================*
 *
 *  $Id: macro.c,v 1.34 2006/07/30 15:31:45 mrsa Exp $
 *
 *  FILE: 
 *     macro.c
 *
 *  Description: 
 *     NNsi Macro Executing
 *
 *===========================================================================*/
#define MACRO_C
#include "local.h"

#ifdef USE_MACRO
static UInt16 getMacroValue(NNshMacroRecord *lineData, UInt16 *gr);
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : outputMacroLog                                             */
/*                                                マクロ用のログを出力する */
/*=========================================================================*/
static void outputMacroLog(DmOpenRef dbRefW, UInt8 level, UInt16 code, Char *data)
{
    NNshMacroResult  logRec;

    MemSet(&logRec, sizeof(logRec), 0x00);
    logRec.dateTime = TimGetSeconds();
    logRec.logLevel = level;
    logRec.errCode  = code;
    StrNCopy(logRec.logData, data, MAX_LOGRECLEN);
    EntryRecord_NNsh(dbRefW, sizeof(NNshMacroResult), &logRec);
    return;
}
#endif


#ifdef USE_MACRO
/*=========================================================================*/
/*   Function :  macroDataOperation                                        */
/*                                                      マクロのデータ操作 */
/*=========================================================================*/
static UInt16 *macroSelectGR(NNshMacroRecord *lineData, UInt16 *gr)
{
    if ((lineData->dst >= NNSH_MACROOP_GR_OFFSET)&&
        (lineData->dst <= NNSH_MACROOP_GR_MAX))
    {
        // 指定された GR に代入
        return (&gr[lineData->dst - NNSH_MACROOP_GR_OFFSET]);
    }
    // GR0 に 代入
    return (gr);
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function :  macroDataOperation                                        */
/*                                                        NNsi設定の値操作 */
/*=========================================================================*/
static void macroDataOperation(NNshMacroRecord *lineData, UInt16 *gr, Boolean setter)
{
    UInt16 value, setValue, *data;

    value    = 0;
    setValue = getMacroValue(lineData, gr);
    switch (lineData->MacroData.numData.subCommand)
    {
      case NNSH_MACROOPCMD_AUTOSTARTMACRO:
        // マクロ自動実行
        value = (NNshGlobal->NNsiParam)->autostartMacro;
        if (setter == true)
        {
            // 値設定が指示されていた場合、、、
            (NNshGlobal->NNsiParam)->autostartMacro = setValue;
        }
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
        // 新着確認実行時に回線切断
        value = (NNshGlobal->NNsiParam)->disconnArrivalEnd;
        if (setter == true)
        {
            // 値設定が指示されていた場合、、、
            (NNshGlobal->NNsiParam)->disconnArrivalEnd = setValue;
        }
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_DA:
        // 新着確認実行後のDA起動
        value = (NNshGlobal->NNsiParam)->use_DAplugin;
        if (setter == true)
        {
            // 値設定が指示されていた場合、、、
            (NNshGlobal->NNsiParam)->use_DAplugin = setValue;
        }
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
        // 参照ログの取得レベル
        value = (NNshGlobal->NNsiParam)->getROLogLevel;
        if (setter == true)
        {
            // 値設定が指示されていた場合、、、
            (NNshGlobal->NNsiParam)->getROLogLevel = setValue;
        }
        break;

      case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
        // 新着確認終了時にビープ音
        value = (NNshGlobal->NNsiParam)->autoBeep;
        if (setter == true)
        {
            // 値設定が指示されていた場合、、、
            (NNshGlobal->NNsiParam)->autoBeep = setValue;
        }
        break;
        
      case NNSH_MACROOPCMD_COPY_AND_DELETE:
        // 参照ログにコピーするとき、削除も実施
        value = (NNshGlobal->NNsiParam)->copyDelReadOnly;
        if (setter == true)
        {
            // 値設定が指示されていた場合、、、
            (NNshGlobal->NNsiParam)->copyDelReadOnly = setValue;
        }
        break;

      default:
        // 何もしない
        break;
    }
    data  = macroSelectGR(lineData, gr);
    *data = value;
    return;
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : getMacroValue                                              */
/*                                            マクロのオペランドから値取得 */
/*=========================================================================*/
static UInt16 getMacroValue(NNshMacroRecord *lineData, UInt16 *gr)
{
    if ((lineData->src >= NNSH_MACROOP_GR_OFFSET)&&
        (lineData->src <= NNSH_MACROOP_GR_MAX))
    {
        // レジスタから読み込んだ値を代入
        return (gr[lineData->src - NNSH_MACROOP_GR_OFFSET]);
    }

    // 第３引数から値を取得する
    return (lineData->MacroData.numData.operator);
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : checkMacroCommandAvailable                                 */
/*                                            マクロコマンドの実行許可確認 */
/*=========================================================================*/
static Boolean checkMacroCommandAvailable(UInt16 command, UInt16 halt)
{
    // 実行許可コマンドリスト
    UInt16  okCmdList[] = {
        MULTIBTN_FEATURE_GET,    MULTIBTN_FEATURE_ALLGET,    MULTIBTN_FEATURE_PARTGET,
        MULTIBTN_FEATURE_DELETE, MULTIBTN_FEATURE_SORT,      MULTIBTN_FEATURE_SEARCH,
        MULTIBTN_FEATURE_NEXT,   MULTIBTN_FEATURE_PREV,      MULTIBTN_FEATURE_TOP,
        MULTIBTN_FEATURE_BOTTOM, MULTIBTN_FEATURE_COPYGIKO,  MULTIBTN_FEATURE_DELNOTGET,
        MULTIBTN_FEATURE_DBCOPY, MULTIBTN_FEATURE_GRAPHMODE, MULTIBTN_FEATURE_DISCONN,
        MULTIBTN_FEATURE_GETLIST,MULTIBTN_FEATURE_NEWARRIVAL,MULTIBTN_FEATURE_SELECTNEXT,
        MULTIBTN_FEATURE_SELECTPREV,MULTIBTN_FEATURE_PREVPAGE,MULTIBTN_FEATURE_NEXTPAGE,
        MULTIBTN_FEATURE_TO_NOTREAD,MULTIBTN_FEATURE_TO_GETALL,MULTIBTN_FEATURE_LOGCHARGE,
        MULTIBTN_FEATURE_SETLOGCHRG,MULTIBTN_FEATURE_BT_ON,  MULTIBTN_FEATURE_BT_OFF,
        MULTIBTN_FEATURE_TO_OFFLINE,MULTIBTN_FEATURE_TO_FAVOR,MULTIBTN_FEATURE_TO_USER1,
        MULTIBTN_FEATURE_TO_USER2,MULTIBTN_FEATURE_TO_USER3, MULTIBTN_FEATURE_TO_USER4,
        MULTIBTN_FEATURE_TO_USER5,MULTIBTN_FEATURE_OYSTERLOGIN, MULTIBTN_FEATURE_NEXTSEARCHBBS, 
        MULTIBTN_FEATURE_UPDATERESNUM, MULTIBTN_FEATURE_BE_LOGIN, 0,
        0,0
    };
    UInt16  check, *cmdPtr;

    // マクロ実行制限チェック
    if (halt > MAX_HALT_OPERATION)
    {
        // 実行制限にひっかかる
        return (false);
    }

    // 一覧画面機能を実行できるかどうかのチェック
    if (((command)&(MULTIBTN_FEATURE)) == (MULTIBTN_FEATURE))
    {
        check = (command)&(MULTIBTN_FEATURE_MASK);
        cmdPtr = okCmdList;
        while (*cmdPtr != 0)
        {
            if (check == *cmdPtr)
            {
                // 実行可能コマンドリストに発見、実行許可
                return (true);
            }
            cmdPtr++;
        }
        // 実行可能コマンドリストになかった、実行禁止
        return (false);
    }

    // 実行許可
    return (true);
}
#endif

#ifdef USE_MACRO
/*-------------------------------------------------------------------------*/
/*   Function : setFileListItems                                           */
/*                                                ファイル一覧のリスト設定 */
/*-------------------------------------------------------------------------*/
static UInt16 setFileListItems(Char *fnameP)
{
    Err                  ret, err;
    UInt16               cardNo, cnt;
    LocalID              dbLocalID;
    DmSearchStateType    state;
    Char                 fileName[MINIBUF + MARGIN], *ptr;

    // VFSの使用設定にあわせてフラグを設定、ファイル検索と、一覧表示
    ptr = fnameP;
    cnt = 0;

    // Palm本体内にあるファイルの一覧を取得
    ret = DmGetNextDatabaseByTypeCreator(true, &state, 
                                         DATA_DBTYPE_ID, 
                                         DBSOFT_CREATOR_ID,
                                         false, &cardNo, &dbLocalID);
    while ((ret == errNone)&&(dbLocalID != 0))
    {
        // ファイル名を取得
        MemSet(fileName, sizeof(fileName), 0x00);
        err = DmDatabaseInfo(cardNo, dbLocalID, fileName,
                             NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL);
        if (err == errNone)
        {
            // 取得したファイル名をリストアイテムにコピー
            if ((fnameP != NULL)&&(cnt < MAX_MACRO_DBFILES))
            {
                StrCopy(ptr, fileName);
                ptr = ptr + StrLen(ptr) + 1;
                cnt++;
            }
        }

        // 二回目以降のデータ取得
        ret = DmGetNextDatabaseByTypeCreator(false, &state,
                                             DATA_DBTYPE_ID, 
                                             DBSOFT_CREATOR_ID,
                                             false, &cardNo, &dbLocalID);
    }
    return (cnt);
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : NNsi_ExecuteMacroSelection                                 */
/*                                                          マクロ選択実行 */
/*=========================================================================*/
Boolean NNsi_ExecuteMacroSelection(void)
{
    Char   *ptr, *dbFileNamePtr, dbFileName[MINIBUF + MARGIN];
    UInt16  cnt, btnId;

    // 準備
    MemSet(dbFileName, sizeof(dbFileName), 0x00);    
    cnt = 0;
    dbFileNamePtr = MEMALLOC_PTR((MINIBUF) * (MAX_MACRO_DBFILES) + MARGIN);
    if (dbFileNamePtr == NULL)
    {
        NNsh_DebugMessage(ALTID_INFO, " memory allocation failure... ", "", 0);
        return (false);   
    }
    MemSet(dbFileNamePtr, ((MINIBUF) * (MAX_MACRO_DBFILES) + MARGIN), 0x00);

    // マクロファイル名を抽出する
    cnt = setFileListItems(dbFileNamePtr);
    if (cnt == 0)
    {
        NNsh_DebugMessage(ALTID_INFO, " macro count is zero... ", "", 0);
        return (false);
    }    

    // 選択ウィンドウを開く
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, dbFileNamePtr, cnt, 0);
    if (btnId == BTNID_JUMPCANCEL)
    {
        // キャンセル
        MEMFREE_PTR(dbFileNamePtr);
        NNsh_DebugMessage(ALTID_INFO, "selection canceled ", "", 0);
        return (false);    
    }

    // 選択したスクリプトファイル名を抽出する 
    ptr = dbFileNamePtr;
    for (cnt = 0; cnt < (MAX_MACRO_DBFILES); cnt++)
    {
        if (cnt == NNshGlobal->jumpSelection)
        {
            StrCopy(dbFileName, ptr);
            break;        
        } 
        ptr = ptr + StrLen(ptr) + 1;
    }

    // 確保したメモリの開放
    MEMFREE_PTR(dbFileNamePtr);

    // デバッグ表示...
    NNsh_DebugMessage(ALTID_INFO, " Macro Name: ", dbFileName, 0);

    // 選択したマクロを実行する...
    if (StrLen(dbFileName) != 0)
    {
        NNsi_ExecuteMacroMain(dbFileName, DBVERSION_MACRO, DBSOFT_CREATOR_ID, DATA_DBTYPE_ID);
        return (true);
    }
    return (false);
}
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : NNsi_ExecuteMacroMain                                      */
/*                                                        マクロ実行メイン */
/*=========================================================================*/
void NNsi_ExecuteMacroMain(Char *macroDBName, UInt16 macroDBVersion, UInt32 creator, UInt32 type)
{
    Err                  ret;
    Boolean              penState;
    Char                *buf, *grBuf;
    UInt16              *gr, recQnt, cnt, command, *data, *stack, *top;
    UInt16               halt, savedParam, autoOffTime, y;
    UInt32               code;
    NNshMacroRecord     *lineData;
    DmOpenRef            dbRefR, dbRefW;
    MemHandle            dataH, tmpH;
    NNshSubjectDatabase *subjDB;
    
    // データ初期化
    halt     = 0;
    code     = 0;
    lineData = NULL;

    // 領域の確保
    stack = MEMALLOC_PTR(sizeof(UInt16) * NOF_REGISTER + MARGIN);
    if (stack == NULL)
    {
        // 領域確保失敗、終了する
        return;
    }
    MemSet(stack, ((sizeof(UInt16) * NOF_REGISTER) + MARGIN), 0x00);
    top = stack;

    // 領域の確保
    gr = MEMALLOC_PTR((sizeof(UInt16) * NOF_REGISTER) + MARGIN);
    if (gr == NULL)
    {
        // 領域確保失敗、終了する
        return;
    }
    MemSet(gr, (sizeof(UInt16) * NOF_REGISTER + MARGIN), 0x00);

    // 領域の確保
    grBuf = MEMALLOC_PTR(sizeof(Char) * TEXTBUFSIZE + MARGIN);
    if (grBuf == NULL)
    {
        // 領域確保失敗、終了する
        MEMFREE_PTR(gr);
        return;
    }
    MemSet(grBuf, (sizeof(Char) * TEXTBUFSIZE + MARGIN), 0x00);

    // 領域の確保
    subjDB = MEMALLOC_PTR(sizeof(NNshSubjectDatabase) + MARGIN);
    if (subjDB == NULL)
    {
        // 領域確保失敗、終了する
        MEMFREE_PTR(grBuf);
        MEMFREE_PTR(gr);
        return;
    }
    MemSet(subjDB, (sizeof(NNshSubjectDatabase) + MARGIN), 0x00);

    // 領域の確保
    buf = MEMALLOC_PTR(MAX_STRLEN + MARGIN);
    if (buf == NULL)
    {
        // 領域確保失敗、終了する
        MEMFREE_PTR(subjDB);
        MEMFREE_PTR(grBuf);
        MEMFREE_PTR(gr);
        return;
    }
    MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);

    // マクロ実行時、確認ダイアログを省略する
    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

    // データベースを開く(スクリプトファイル)
    OpenDB_wCreator(macroDBName, macroDBVersion, creator, type, false, &dbRefR);
    if (dbRefR == 0)
    {
        // エラー発生、終了する
        goto FUNC_END;
    }

    // データベースを開く(ログファイル)
    OpenDatabase_NNsh(DBNAME_MACRORESULT, DBVERSION_MACRO, &dbRefW);
    if (dbRefW == 0)
    {
        // エラー発生、終了する
        CloseDatabase_NNsh(dbRefR);
        goto FUNC_END;
    }
    GetDBCount_NNsh(dbRefW, &recQnt);
    while (recQnt != 0)
    {
        //  すでにログファイルが記録されていた場合、全レコードを削除する
        (void) DeleteRecordIDX_NNsh(dbRefW, (recQnt - 1));
        recQnt--;
    }

    // スクリプトのレコード数を確認する
    GetDBCount_NNsh(dbRefR, &recQnt);

    // 自動電源ＯＦＦタイマを無効にする
    autoOffTime = SysSetAutoOffTime(0);
    (void) EvtResetAutoOffTimer();

    cnt = 0;
    while (cnt < recQnt)
    {
        // レコードを読み出す
        ret = GetRecordReadOnly_NNsh(dbRefR, cnt, &dataH, (void **) &lineData);
        if (ret != errNone)
        {
            // レコード読み出し失敗、次のレコードへ移動する
            dataH = 0;
            goto NEXT_COMMAND;
        }

        switch (lineData->opCode)
        {
          case NNSH_MACROCMD_XOR:
            // 排他的論理和をとる
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data ^ command;
            break;

          case NNSH_MACROCMD_AND:
            // 値を論理積する
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data & command;
            break;

          case NNSH_MACROCMD_OR:
            // 値を論理和する
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data | command;
            break;

          case NNSH_MACROCMD_ADD:
            // 値を加算する
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data + command;
            break;

          case NNSH_MACROCMD_CMP:
          case NNSH_MACROCMD_SUB:
            // 値を比較して、その結果を代入
            command = getMacroValue(lineData, gr);
            data    = macroSelectGR(lineData, gr);
            *data   = *data - command;
            break;

          case NNSH_MACROCMD_JNZ:
            // 値がゼロでなければジャンプ
            data    = macroSelectGR(lineData, gr);
            if (*data != 0)
            {
                // ジャンプ先は、src or 直値から取得する
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // コマンド実行不許可(ジャンプ不可)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                                   "CANNOT JUMP");
                }
            }
            break;

          case NNSH_MACROCMD_JZ:
            // 値がゼロならジャンプ
            data    = macroSelectGR(lineData, gr);
            if (*data == 0)
            {
                // ジャンプ先は、src or 直値から取得する
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // コマンド実行不許可(ジャンプ不可)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                                   "CANNOT JUMP");
                }
            }
            break;

          case NNSH_MACROCMD_MOVE:
            // データ(値)の代入
            data    = macroSelectGR(lineData, gr);
            command = getMacroValue(lineData, gr);
            *data   = command;
            break;

          case NNSH_MACROCMD_JMP:
            // シーケンスの無条件ジャンプ
            command = getMacroValue(lineData, gr);
            if ((command > 0)&&(command <= recQnt))
            {
                cnt = command - 1 - 1;
            }
            else
            {
                // コマンド実行不許可(ジャンプ不可)
                outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                               "CANNOT JUMP");
            }
            break;

          case NNSH_MACROCMD_EXECUTE:
            // 機能の実行
            command = getMacroValue(lineData, gr);

            // コマンド実行許可を確認
            if (checkMacroCommandAvailable(((command)|(MULTIBTN_FEATURE)), halt) == true)
            {
                // コマンドを実行する
                ExecuteAction(command);
            }
            else
            {
                // コマンド実行不許可(ログ出力)
                outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, command,
                               "PROHIBIT EXECUTION.");
            }
            break;

          case NNSH_MACROCMD_MESSAGE:
            // ログ出力
            if ((lineData->src >= NNSH_MACROOP_GR_OFFSET)&&
                (lineData->src <= NNSH_MACROOP_GR_MAX))
            {
                // srcにGRxが指定されていた場合
                MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
                StrCopy(buf, "[gr");
                NUMCATI(buf, lineData->src - NNSH_MACROOP_GR_OFFSET);
                StrCat (buf, " = ");
                command = gr[lineData->src - NNSH_MACROOP_GR_OFFSET];
                NUMCATI(buf, command);
                StrCat (buf, "] ");
                // 文字列も後ろに追加する
                StrNCopy(&buf[StrLen(buf)], lineData->MacroData.strData.data,
                         (MAX_STRLEN - StrLen(buf)));
                outputMacroLog(dbRefW, lineData->MacroData.strData.operator, 0,
                               buf);
            }
            else
            {
                // 与えられたデータをそのまま表示する
                outputMacroLog(dbRefW, lineData->MacroData.strData.operator, 0,
                               lineData->MacroData.strData.data);
            }
            break;

          case NNSH_MACROCMD_BEEP:
            // ビープ音を鳴らす
            command = getMacroValue(lineData, gr);
            switch (command)
            {
              case 0:
                // Info 音
                NNsh_BeepSound(sndInfo);
                break;

              case 1:
                // Alarm音(NNsh_BeepSound()でsndAlarmに変換する)
                NNsh_BeepSound(sndClick);
                break;

              case 2:
                // Error 音
                NNsh_BeepSound(sndError);
                break;

              case 3:
                // Confirmation 音
                NNsh_BeepSound(sndConfirmation);
                break;

              case 0xffff:
                // アラーム音とログを出力
                NNsh_BeepSound(sndAlarm);
                outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, 0,
                                             lineData->MacroData.strData.data);
                break;

              default:
                // アラーム音
                NNsh_BeepSound(sndAlarm);
                break;
            }
            break;

          case NNSH_MACROCMD_MACROEND:
            // マクロ終了(終了のログを出力する)
            outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_DEBUG, 0, "<< MACRO END >>");

            // レコード番号をオーバーさせ、ループを強制的に抜ける
            cnt = recQnt;
            break;

          case NNSH_MACROCMD_OPERATE:
            // NNsi設定変更
            macroDataOperation(lineData, gr, true);
            break;

          case NNSH_MACROCMD_STATUS:
            // NNsi設定の値取得
            macroDataOperation(lineData, gr, false);
            break;

          case NNSH_MACROCMD_SETTABNUM:
            // 現在のタブ数を格納
            data  = macroSelectGR(lineData, gr);
            *data = Show_tab_info(false);
            break;

          case NNSH_MACROCMD_STRSTR:
            // 文字列が含まれるかチェック
            data  = macroSelectGR(lineData, gr);
            *data = 1;
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
                if (Get_Subject_Database(command, subjDB) == errNone)
                {
                    if (StrStr(subjDB->threadTitle, 
                                         lineData->MacroData.strData.data) != NULL)
                    {
                        // 文字列が含まれていた
                        *data = 0;
                    }
                }
            }
            break;

          case NNSH_MACROCMD_STRCMP:
            // 文字列が一致するかチェック
            data  = macroSelectGR(lineData, gr);
            *data = 1;
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
                if (Get_Subject_Database(command, subjDB) == errNone)
                {
                    if (StrCompare(subjDB->threadTitle, 
                                         lineData->MacroData.strData.data) != NULL)
                    {
                        // 文字列が一致した
                        *data = 0;
                    }
                }
            }
            break;

          case NNSH_MACROCMD_GETMESNUM:
            // 取得レス数を反映
            data  = macroSelectGR(lineData, gr);
            *data = 0;
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                // 取得レス数を代入する
                *data = subjDB->maxLoc;
            }
            break;

          case NNSH_MACROCMD_MESSTATUS:
            // 取得スレの状態を反映
            data  = macroSelectGR(lineData, gr);
            *data = 0;
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                // スレ状態を代入する
                *data = subjDB->state;
            }
            break;

          case NNSH_MACROCMD_BBSTYPE:
            // BBSタイプを反映
            data  = macroSelectGR(lineData, gr);
            *data = 0;
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                // BBSタイプを代入する
                *data = subjDB->bbsType;
            }
            break;

          case NNSH_MACROCMD_IMPORT:
#ifdef USE_XML_OUTPUT
            if ((NNshGlobal->NNsiParam)->useVFS == NNSH_NOTSUPPORT_VFS)
            {
                // VFS 非サポート機種はそのままNNsi設定インポート
                Input_NNsiSet_XML(FILE_XML_NNSISET);
                break;
            }
            // VFSサポート機種
            command = (NNshGlobal->NNsiParam)->useVFS;   // VFS設定の一時記憶
            data    = macroSelectGR(lineData, gr);
            if (*data == 0)
            {
                // (無理やりPalmから読み出すようにする)
                (NNshGlobal->NNsiParam)->useVFS = 0;
            }
            else
            {
                // (無理やりVFS設定に更新する)
                (NNshGlobal->NNsiParam)->useVFS = ((NNSH_VFS_ENABLE)|
                                     (NNSH_VFS_WORKAROUND)|
                                     (NNSH_VFS_USEOFFLINE)|
                                     (NNSH_VFS_DIROFFLINE)|
                                     (NNSH_VFS_DBBACKUP));
                
            }
            // NNsi設定インポート
            Input_NNsiSet_XML(FILE_XML_NNSISET);
            (NNshGlobal->NNsiParam)->useVFS = command;  // VFS設定の復旧
#endif
            break;

          case NNSH_MACROCMD_EXPORT:
#ifdef USE_XML_OUTPUT
            // NNsi設定のエクスポート
            Output_NNsiSet_XML(FILE_XML_NNSISET);
#endif
            break;

          case NNSH_MACROCMD_LAUNCHDA:
            // DA起動
            code = lineData->MacroData.numData.data1;
            code = code << 16;
            code = code + lineData->MacroData.numData.data2;

            ///////////////////// DA 起動確認用ログ  //////////////////////////
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            StrCopy(buf, "DA:0x");
            NUMCATH(buf, code);
            outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_INFO, 0, buf);
            ///////////////////////////////////////////////////////////////////

            // 自動電源OFFタイマを一時的に復旧させる
            (void) SysSetAutoOffTime(autoOffTime);
            (void) EvtResetAutoOffTimer();

            // DAを起動する
            LaunchDA_NNsh(code);

            // 自動電源OFFタイマを停止させる
            (void) SysSetAutoOffTime(0);
            (void) EvtResetAutoOffTimer();
            break;

          case NNSH_MACROCMD_CLIPCOPY:
             // 文字列をクリップボードにコピー
             ClipboardAddItem(clipboardText, lineData->MacroData.strData.data, 
                              (StrLen(lineData->MacroData.strData.data) + 1));
            break;

          case NNSH_MACROCMD_CLIPADD:
             // 文字列をクリップボードに追加
             ClipboardAppendItem(clipboardText, 
                                 lineData->MacroData.strData.data, 
                                 (StrLen(lineData->MacroData.strData.data) + 1));
            break;

          case NNSH_MACROCMD_CLIPINSTR:
            // クリップボードの文字列に含まれるか確認
            data  = macroSelectGR(lineData, gr);
            *data = 1;
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                // クリップボードの内容をいったんバッファにコピー
                if (command > TEXTBUFSIZE)
                {
                    command = TEXTBUFSIZE;
                }
                MemSet(grBuf, (sizeof(Char) * TEXTBUFSIZE + MARGIN), 0x00);
                MemMove(grBuf, MemHandleLock(tmpH), command);
                MemHandleUnlock(tmpH);
                if (StrStr(grBuf, lineData->MacroData.strData.data) != NULL)
                {
                    // データが含まれていた
                    *data = 0;
                }
            }
            break;

          case NNSH_MACROCMD_TITLECLIP:
            // スレタイトルをクリップボードにコピー
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
                if (Get_Subject_Database(command, subjDB) == errNone)
                {
                    ClipboardAddItem(clipboardText, subjDB->threadTitle, 
                                            (StrLen(subjDB->threadTitle) + 1));
                }
            }
            break;

          case NNSH_MACROCMD_SET_FINDBBSSTR:
            // 板タブ検索用文字列にクリップボードの文字列をコピーする
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                // クリップボードの内容をいったんバッファにコピー
                if (command > BUFSIZE)
                {
                    command = BUFSIZE - 1;
                }
                if (NNshGlobal->searchBBSH == 0)
                {
                    NNshGlobal->searchBBSH = MemHandleNew(BUFSIZE);
                    if (NNshGlobal->searchBBSH == 0)
                    {
                        // 領域確保失敗、なにもしない
                        break;
                    }
                }
                MemSet(MemHandleLock(NNshGlobal->searchBBSH), BUFSIZE, 0x00);
                MemHandleUnlock(NNshGlobal->searchBBSH);
                MemMove(MemHandleLock(NNshGlobal->searchBBSH), MemHandleLock(tmpH), command);
                MemHandleUnlock(tmpH);
                MemHandleUnlock(NNshGlobal->searchBBSH);
            }
            break;

          case NNSH_MACROCMD_SET_FINDTHREADSTR:
            // スレタイ検索用文字列にクリップボードの文字列をコピーする
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                // クリップボードの内容をいったんバッファにコピー
                if (command > BUFSIZE)
                {
                    command = BUFSIZE - 1;
                }
                if (NNshGlobal->searchTitleH == 0)
                {
                    NNshGlobal->searchTitleH = MemHandleNew(BUFSIZE);
                    if (NNshGlobal->searchTitleH == 0)
                    {
                        // 領域確保失敗、なにもしない
                        break;
                    }
                }
                MemSet(MemHandleLock(NNshGlobal->searchTitleH), BUFSIZE, 0x00);
                MemHandleUnlock(NNshGlobal->searchTitleH);
                MemMove(MemHandleLock(NNshGlobal->searchTitleH), MemHandleLock(tmpH), command);
                MemHandleUnlock(tmpH);
                MemHandleUnlock(NNshGlobal->searchTitleH);
            }
            break;

          case NNSH_MACROCMD_UPDATE_RESNUM:
            // スレのレス数を更新し格納(1.03)
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
                data    = macroSelectGR(lineData, gr);
                *data   = UpdateThreadResponseNumber(command);
            }
            break;

          case NNSH_MACROCMD_JPL:
            // 値が正の場合にはジャンプ(1.03)
            data    = macroSelectGR(lineData, gr);
            if ((*data > 0)&&(*data <= 0x7fff))
            {
                // ジャンプ先は、src or 直値から取得する
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // コマンド実行不許可(ジャンプ不可)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                                   "CANNOT JUMP");
                }
            }
            break;

          case NNSH_MACROCMD_JMI:
            // 値が小さい(負)場合にはジャンプ(1.03)
            data    = macroSelectGR(lineData, gr);
            if (*data > 0x7fff)
            {
                // ジャンプ先は、src or 直値から取得する
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // コマンド実行不許可(ジャンプ不可)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1),
                                   "CANNOT JUMP");
                }
            }
            break;

          case NNSH_MACROCMD_OUTPUTTITLE:
            // スレタイトルをログ出力(1.03)
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                StrNCopy(buf, subjDB->threadTitle, MAX_STRLEN);
                outputMacroLog(dbRefW, lineData->MacroData.strData.operator, 0,
                               buf);
            }
            break;

          case NNSH_MACROCMD_OUTPUTCLIP:
            // クリップボード文字列をログ出力(1.03)
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                if (command > MAX_STRLEN)
                {
                    command = MAX_STRLEN;
                }
                StrNCopy(buf, MemHandleLock(tmpH), command);
                outputMacroLog(dbRefW, lineData->MacroData.strData.operator, 0,
                               buf);
                MemHandleUnlock(tmpH);
            }                
            break;

          case NNSH_MACROCMD_REMARK:
            // 注釈文、何もしない
            break;

          case NNSH_MACROCMD_PUSH:
            // スタックにPUSH(スタックサイズの限界)
            if ((stack + sizeof(UInt16) * NOF_REGISTER) != top)
            {
                command = getMacroValue(lineData, gr);
                *top    = command;
                top++;
            }
            break;
         
          case NNSH_MACROCMD_POP:
            // スタックからPOP(1.04)
            if (top != stack)
            {
                data  = macroSelectGR(lineData, gr);
                *data = *top;
                top--;
            }
            break;

          case NNSH_MACROCMD_GETRECNUM:
            // レコード番号を取得する(1.04)
            data    = macroSelectGR(lineData, gr);
            if (NNshGlobal->nofTitleItems != 0)
            {
                command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            }
            else
            {
                // かなり大きな数を格納...
                command = 0xfffd;
            }
            *data = command;
            break;

          case NNSH_MACROCMD_FINDTHREADSTRCOPY:
            // スレタイ検索文字列をクリップボードにコピー
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            if (NNshGlobal->searchTitleH != 0)
            {
                StrNCopy(buf, MemHandleLock(NNshGlobal->searchTitleH), MAX_STRLEN);
                MemHandleUnlock(NNshGlobal->searchTitleH);
            }
            else
            {
                StrCopy(buf, " ");
            }
            ClipboardAddItem(clipboardText, buf, StrLen(buf));
            break;

          case NNSH_MACROCMD_INPUTDIALOG:
            // 文字列入力ダイアログを表示する (入力文字列をクリップボードにコピー)
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            DataInputDialog(lineData->MacroData.strData.data, buf, MAX_STRLEN, 
                            NNSH_DIALOG_USE_MACROINPUT, NULL);
            ClipboardAddItem(clipboardText, buf, StrLen(buf));
            break;
            
          case NNSH_MACROCMD_OUTPUTDIALOG:
            // 文字列をダイアログ表示する
            NNsh_ErrorMessage(ALTID_INFO, lineData->MacroData.strData.data, "", 0);
            break;

          case NNSH_MACROCMD_CONFIRMDIALOG:
            // クリップボードの文字列を表示し、確認ダイアログを表示する
            //  (ボタンが押された結果(OK/Cancel)をレジスタに反映させる）
            MemSet(buf, (MAX_STRLEN + MARGIN), 0x00);
            tmpH = ClipboardGetItem(clipboardText, &command);
            if (tmpH != 0)
            {
                // クリップボードから文字列を取得する
                if (command > MAX_STRLEN)
                {
                    command = MAX_STRLEN;
                }
                StrNCopy(buf, MemHandleLock(tmpH), command);
                MemHandleUnlock(tmpH);
            }
            else
            {
                // 何もダイアログに表示しない
                StrNCopy(buf, " Are you OK? ", StrLen(" "));
            }
            data    = macroSelectGR(lineData, gr);
            command = NNsh_ErrorMessage(ALTID_CONFIRM, buf, "", 0);
            *data   = command;
            break;

          case NNSH_MACROCMD_MESATTRIBUTE:
            // 取得スレの属性を反映 (1.06)
            data  = macroSelectGR(lineData, gr);
            *data = 0;
            command = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            if (Get_Subject_Database(command, subjDB) == errNone)
            {
                // スレ状態を代入する
                *data = subjDB->msgAttribute;
            }
            break;

          case NNSH_MACROCMD_JUMPLAST:
            // スレの最下行にいる場合にはジャンプ (1.06)
            // (かなり無理やりな判定...)
            if ((((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_LOWERLIMIT)||
                 ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_ALL))&&
                 (NNshGlobal->nofTitleItems == ((NNshGlobal->NNsiParam)->selectedTitleItem + 1)))
            {
                command = getMacroValue(lineData, gr);
                if ((command > 0)&&(command <= recQnt))
                {
                    cnt = command - 1 - 1;
                }
                else
                {
                    // コマンド実行不許可(ジャンプ不可)
                    outputMacroLog(dbRefW, NNSH_MACROLOGLEVEL_ERROR, (cnt + 1), "CANNOT JUMP");
                }
            }
            break;

          default:
            // コマンドコード異常、なにも実行しない
            break;
        }

NEXT_COMMAND:
        // レコードを解放し、次へすすむ
        ReleaseRecordReadOnly_NNsh(dbRefR, dataH);
        cnt++;

        // ペンの状態を拾う(画面タップされているか確認する)
        EvtGetPen(&command, &y, &penState);
        if (penState == true)
        {
            // ペンがダウンされていたら、中止するか確認を行う
            if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                  MSG_CONFIRM_ABORT_MACRO, 
                                  MSG_LINE_NUMBER, cnt) == 0)
            {
                // OKが押された、breakする
                break;
            }
        }
    }

    // 自動電源OFFタイマを復旧させる
    (void) SysSetAutoOffTime(autoOffTime);
    (void) EvtResetAutoOffTimer();

    // データベースを閉じて終了する    
    CloseDatabase_NNsh(dbRefW);
    CloseDatabase_NNsh(dbRefR);

FUNC_END:
    MEMFREE_PTR(buf);
    MEMFREE_PTR(subjDB);
    MEMFREE_PTR(grBuf);
    MEMFREE_PTR(gr);
    MEMFREE_PTR(stack);
    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

    // 終了ブザーを鳴らす
    // SndPlaySystemSound(sndAlarm);

    // マクロ終了を通知する
    NNsh_InformMessage(ALTID_INFO, MSG_MACROEXEC_DONE, "", 0);
    return;
}
#endif

#ifdef USE_MACRO
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
#endif

#ifdef USE_MACRO
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
#endif

#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : NNsi_MacroDataView                                         */
/*                                                        マクロ内容を表示 */
/*=========================================================================*/
void NNsi_MacroDataView(Char *macroDBName, UInt16 macroDBVersion)
{
    Err              ret;
    Char           **script, logData[TINYBUF];
    UInt16           macroCnt, lp, command;
    MemHandle        dataH;
    NNshMacroRecord *lineData;
    DmOpenRef        dbRefR;

    // データの初期化
    script   = NULL;
    macroCnt = 0;

    // エラー表示用
    MemSet (logData, sizeof(logData), 0x00);
    StrCopy(logData, "MEMERR");

    // データベースを開く(スクリプトファイル)
    OpenDatabase_NNsh(macroDBName, macroDBVersion, &dbRefR);
    if (dbRefR == 0)
    {
        // エラー発生、終了する
        return;
    }

    // スクリプトのレコード数を確認する
    GetDBCount_NNsh(dbRefR, &macroCnt);

    script = MEMALLOC_PTR(sizeof(Char *) * macroCnt + MARGIN);
    if (script == NULL)
    {
        // メモリ確保エラー発生、終了する
        CloseDatabase_NNsh(dbRefR);
        return;
    }
    MemSet(script, (sizeof(Char *) * macroCnt + MARGIN), 0x00);

    // ログデータ出力    
    for (lp = 0; lp < macroCnt; lp++)
    {
        // レコードを読み出す
        ret = GetRecordReadOnly_NNsh(dbRefR, lp, &dataH, (void **) &lineData);
        if (ret != errNone)
        {
            // レコード読み出し失敗、次のレコードへ移動する
            dataH = 0;
            script[lp] = logData;
            goto NEXT_COMMAND;
        }
        script[lp] = MEMALLOC_PTR(MAX_LOGVIEW_BUFFER + MARGIN);
        if (script[lp] == NULL)
        {
            // 領域確保エラー
            script[lp] = logData;
            goto NEXT_COMMAND;
        }
        MemSet(script[lp], (MAX_LOGVIEW_BUFFER + MARGIN), 0x00);

        // 行番号を表示
        NUMCATI(script[lp], (lp + 1));
        StrCat (script[lp], "  ");

        switch (lineData->opCode)
        {
          case NNSH_MACROCMD_XOR:
            // 値を加算する
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " XOR ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_AND:
            // 値を加算する
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " AND ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_OR:
            // 値を加算する
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " OR ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_ADD:
            // 値を加算する
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " + ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_CMP:
          case NNSH_MACROCMD_SUB:
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " - ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_JNZ:
            StrCat (script[lp], "IF gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], "<> 0 THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_JZ:
            StrCat (script[lp], "IF gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " = 0 THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_MOVE:
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], " ");
            break;

          case NNSH_MACROCMD_JMP:
            StrCat (script[lp], "GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_EXECUTE:
            // 機能の実行
            StrCat (script[lp], "CALL (");
            command = setSourceDataDisplay(script[lp], lineData);
            if (command != NNSH_MACROOPCMD_DISABLE)
            {
                StrCat (script[lp], ":");
                setExecFuncDataDisplay(script[lp], command);
                StrCat (script[lp], " ");
            }
            StrCat (script[lp], ") ");
            break;

          case NNSH_MACROCMD_MESSAGE:
            // ログ出力
            StrCat (script[lp], "PRINT ");
            if ((lineData->src >= NNSH_MACROOP_GR_OFFSET)&&
                (lineData->src <= NNSH_MACROOP_GR_MAX))
            {
                // GRx を表示
                StrCat (script[lp], "(gr");
                NUMCATI(script[lp], lineData->src - NNSH_MACROOP_GR_OFFSET);
                StrCat (script[lp], ") ");
            }
            else
            {
                // 文字列を表示
                StrCat (script[lp], "\"");
                StrCat (script[lp], lineData->MacroData.strData.data);
                StrCat (script[lp], "\" ");
            }
            break;

          case NNSH_MACROCMD_BEEP:
            // ビープ音を鳴らす
            StrCat (script[lp], "BEEP ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_MACROEND:
            // マクロ終了
            StrCat (script[lp], "END ");
            break;

          case NNSH_MACROCMD_SETTABNUM:
            // タブ数を格納
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := ");
            StrCat (script[lp], "(tabNum) ");
            break;

          case NNSH_MACROCMD_STRSTR:
            // 指定した文字列がスレタイトルに含まれるか
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := strstr(title, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;            

          case NNSH_MACROCMD_STRCMP:
            // 指定した文字列がスレタイトルに含まれるか
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := strstr(title, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;            

          case NNSH_MACROCMD_OPERATE:
            // NNsi設定値の取得
            switch (lineData->MacroData.numData.subCommand)
            {
              case NNSH_MACROOPCMD_AUTOSTARTMACRO:
                // マクロ自動実行の値を取得
                StrCat(script[lp], "(autoMacro) := ");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
                // 新着確認実行後に回線切断
                StrCat(script[lp], "(chkLINE) := ");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_DA:
                // 新着確認実行後にDA起動
                StrCat(script[lp], "(chkDA) := ");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
                // 参照ログ取得レベル
                StrCat(script[lp], "(ROlogLvl) := ");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
                // 新着確認終了後にビープ音
                StrCat(script[lp], "(chkBEEP) := ");
                break;

              case NNSH_MACROOPCMD_COPY_AND_DELETE:
                // 参照ログコピー後に削除も実施
                StrCat(script[lp], "(cp&rm) := ");
                break;

              default:
                StrCat(script[lp], "nop ");
                break;
            }
            setSourceDataDisplay(script[lp], lineData);
            StrCat(script[lp], ", save ");
            /** not break; **/

          case NNSH_MACROCMD_STATUS:
            // NNsi設定値の取得
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := ");
            switch (lineData->MacroData.numData.subCommand)
            {
              case NNSH_MACROOPCMD_AUTOSTARTMACRO:
                // マクロ自動実行の値を取得
                StrCat(script[lp], "(autoMacro)");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_LINE:
                // 新着確認実行後に回線切断
                StrCat(script[lp], "(chkLINE)");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_DA:
                // 新着確認実行後にDA起動
                StrCat(script[lp], "(chkDA)");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_ROLOG:
                // 参照ログ取得レベル
                StrCat(script[lp], "(ROlogLvl)");
                break;

              case NNSH_MACROOPCMD_NEWARRIVAL_BEEP:
                // 新着確認終了後にビープ音
                StrCat(script[lp], "(chkBEEP)");
                break;

              case NNSH_MACROOPCMD_COPY_AND_DELETE:
                // 参照ログコピー後に削除も実施
                StrCat(script[lp], "(cp&rm)");
                break;

              default:
                StrCat(script[lp], "0");
                break;
            }
            break;

          case NNSH_MACROCMD_GETMESNUM:
            // スレ数を反映
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := mesNum(title) ");
            break;            

          case NNSH_MACROCMD_MESSTATUS:
            // スレ状態を反映
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := mesStatus(title) ");
            break;

          case NNSH_MACROCMD_BBSTYPE:
            // スレのBBSタイプを反映
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := bbsType(title) ");
            break;

          case NNSH_MACROCMD_IMPORT:
            // NNsi設定インポート
#ifdef USE_XML_OUTPUT
            StrCat (script[lp], "IMPORT '");
            StrCat (script[lp], FILE_XML_NNSISET);
            if (setSourceDataDisplay(script[lp], lineData) == 0)
            {
                StrCat (script[lp], "' from VFS");
            }
            else
            {
                StrCat (script[lp], "' from Palm");
            }
#else
            StrCat (script[lp], "NOP ");
#endif
            break;

          case NNSH_MACROCMD_EXPORT:
            // NNsi設定エクスポート
#ifdef USE_XML_OUTPUT
            StrCat (script[lp], "EXPORT '");
            StrCat (script[lp], FILE_XML_NNSISET);
            StrCat (script[lp], "' ");
#else
            StrCat (script[lp], "NOP ");
#endif
            break;

          case NNSH_MACROCMD_LAUNCHDA:
            // DA起動
            StrCat (script[lp], "LAUNCH '");
            logData[0] = ((0xff00 & lineData->MacroData.numData.data1) >> 8); 
            logData[1] = ((0x00ff & lineData->MacroData.numData.data1));
            logData[2] = ((0xff00 & lineData->MacroData.numData.data2) >> 8);
            logData[3] = ((0x00ff & lineData->MacroData.numData.data2));
            logData[4] = '\0';
            StrCat (script[lp], logData);            
            StrCat (script[lp], "' as DA");
            break;

          case NNSH_MACROCMD_CLIPCOPY:
            // クリップボードにコピー
            StrCat (script[lp], "COPY(clip, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;

          case NNSH_MACROCMD_CLIPADD:
             // クリップボードに追加
            StrCat (script[lp], "APPEND(clip, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;

          case NNSH_MACROCMD_CLIPINSTR:
            // クリップボードの文字列に含まれるか確認
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := strstr(clip, \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\") ");
            break;

          case NNSH_MACROCMD_TITLECLIP:
            // スレタイトルをクリップボードにコピー
            StrCat(script[lp], "COPY(clip, title)");
            break;

          case NNSH_MACROCMD_SET_FINDBBSSTR:
            // クリップボードの内容をＢＢＳ検索用に設定
            StrCat(script[lp], "COPY(findBBS, clip)");
            break;

          case NNSH_MACROCMD_SET_FINDTHREADSTR:
            // クリップボードの内容をスレ検索用に設定
            StrCat(script[lp], "COPY(findTHREAD, clip)");
            break;

          case NNSH_MACROCMD_UPDATE_RESNUM:
            // レス数を更新して格納
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := updateMesNum(title) ");
            break;

          case NNSH_MACROCMD_JPL:
            // 正の値ならジャンプする
            StrCat (script[lp], "IF gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], "> 0 THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_JMI:
            // 負の値ならジャンプする
            StrCat (script[lp], "IF gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], "< 0 THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          case NNSH_MACROCMD_OUTPUTTITLE:
            // スレタイトルをログ出力
            StrCat (script[lp], "PRINT title");
            break;

          case NNSH_MACROCMD_OUTPUTCLIP:
            // クリップボードの内容をログ出力
            StrCat (script[lp], "PRINT clip");
            break;

          case NNSH_MACROCMD_REMARK:
            // 注釈(ただ表示するだけ)
            StrCat (script[lp], "' ");
            StrCat (script[lp], lineData->MacroData.strData.data);
            break;

          case NNSH_MACROCMD_PUSH:
            // スタックにPUSH
            StrCat (script[lp], "PUSH");
            setSourceDataDisplay(script[lp], lineData);
            break;

          case NNSH_MACROCMD_POP:
            // スタックからPOP
            StrCat (script[lp], "POP ");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            break;

          case NNSH_MACROCMD_GETRECNUM:
            // レコード番号を格納する
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := RecordIndex(title)");
            break;

          case NNSH_MACROCMD_FINDTHREADSTRCOPY:
            // スレ検索文字列をクリップボードにコピー
            StrCat (script[lp], "COPY(clip, findTHREAD)");
            break;
          case NNSH_MACROCMD_INPUTDIALOG:
            // 文字列を入力する (1.05)
            StrCat (script[lp], "clip := INPUT \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            break;

          case NNSH_MACROCMD_OUTPUTDIALOG:
            // 文字列を出力する (1.05)
            StrCat (script[lp], "OUTPUT \"");
            StrCat (script[lp], lineData->MacroData.strData.data);
            StrCat (script[lp], "\"");
            break;

          case NNSH_MACROCMD_CONFIRMDIALOG:
            // クリップボードの文字列を表示し確認する (1.05)
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := CONFIRM (clip)");
            break;

          case NNSH_MACROCMD_MESATTRIBUTE:
            // スレ属性を反映
            StrCat (script[lp], "gr");
            NUMCATI(script[lp], lineData->dst - NNSH_MACROOP_GR_OFFSET);
            StrCat (script[lp], " := mesAttribute(title) ");
            break;

          case NNSH_MACROCMD_JUMPLAST:
            // スレの最下行ならジャンプする
            StrCat (script[lp], "IF isLast(threadList) THEN GOTO ");
            setSourceDataDisplay(script[lp], lineData);
            StrCat (script[lp], "  ");
            break;

          default:
            // コマンドコード異常、なにも実行しない
            StrCat (script[lp], "  ");
            break;
        }

NEXT_COMMAND:
        // レコードを解放し、次へすすむ
        ReleaseRecordReadOnly_NNsh(dbRefR, dataH);
    }
    CloseDatabase_NNsh(dbRefR);

    // マクロデータの表示
    DataViewForm(NNSH_MACROCMD_VIEWTITLE, macroCnt, script);

    // 確保した領域を開放する
    for (lp = 0; lp < macroCnt; lp++)
    {
        if ((script[lp] != NULL)&&(script[lp] != logData))
        {
            MEMFREE_PTR(script[lp]);
        }
    }
    MEMFREE_PTR(script);
    return;
}
#endif
        
#ifdef USE_MACRO
/*=========================================================================*/
/*   Function : NNsi_MacroExecLogView                                      */
/*                                                          マクロ実行結果 */
/*=========================================================================*/
void NNsi_MacroExecLogView(Char *macroDBName, UInt16 macroDBVersion)
{
    Err              ret;
    Char           **script, logData[TINYBUF];
    UInt16           macroCnt, lp;
    MemHandle        dataH;
    NNshMacroResult *lineData;
    DmOpenRef        dbRefR;
    DateTimeType     dateTime;

    // データの初期化
    script   = NULL;
    macroCnt = 0;

    // エラー表示用
    MemSet (logData, sizeof(logData), 0x00);
    StrCopy(logData, "MEMERR");

    // データベースを開く(スクリプトファイル)
    OpenDatabase_NNsh(macroDBName, macroDBVersion, &dbRefR);
    if (dbRefR == 0)
    {
        // エラー発生、終了する
        return;
    }

    // スクリプトのレコード数を確認する
    GetDBCount_NNsh(dbRefR, &macroCnt);

    script = MEMALLOC_PTR(sizeof(Char *) * macroCnt + MARGIN);
    if (script == NULL)
    {
        // メモリ確保エラー発生、終了する
        CloseDatabase_NNsh(dbRefR);
        return;
    }
    MemSet(script, (sizeof(Char *) * macroCnt + MARGIN), 0x00);

    // ログデータ出力    
    for (lp = 0; lp < macroCnt; lp++)
    {
        // レコードを(末尾から)読み出す ※レコードの若い番号が最新のデータなため
        ret = GetRecordReadOnly_NNsh(dbRefR, (macroCnt - (1 + lp)), &dataH, (void **) &lineData);
        if (ret != errNone)
        {
            // レコード読み出し失敗、次のレコードへ移動する
            dataH = 0;
            script[lp] = logData;
            goto NEXT_COMMAND;
        }
        script[lp] = MEMALLOC_PTR(MAX_LOGOUTPUT_BUFFER + MARGIN);
        if (script[lp] == NULL)
        {
            // 領域確保エラー
            script[lp] = logData;
            goto NEXT_COMMAND;
        }
        MemSet(script[lp], (MAX_LOGOUTPUT_BUFFER + MARGIN), 0x00);

        // 時刻を表示
        MemSet(&dateTime, sizeof(dateTime), 0x00);
        TimSecondsToDateTime(lineData->dateTime, &dateTime);
        DateToAscii(dateTime.month, dateTime.day, dateTime.year,
                    dfYMDWithDashes, script[lp]);
        StrCat(script[lp], " ");
        TimeToAscii(dateTime.hour, dateTime.minute, tfColon24h, 
                    &script[lp][StrLen(script[lp])]);
        StrCat(script[lp], ":");
        NUMCATI(script[lp], dateTime.second);      

        // ログレベルとエラーコードを表示(エラーコードはエラー発生時のみ)
        StrCat (script[lp], "[");
        NUMCATI(script[lp], lineData->logLevel);
        if (lineData->errCode != 0)
        {
            StrCat (script[lp], ":0x");
            NUMCATH(script[lp], lineData->errCode);
        }
        StrCat (script[lp], "] ");

        // ログデータを表示
        StrCat (script[lp], lineData->logData);

NEXT_COMMAND:
        // レコードを解放し、次へすすむ
        ReleaseRecordReadOnly_NNsh(dbRefR, dataH);
    }
    CloseDatabase_NNsh(dbRefR);

    // 表示ダイアログを開く
    DataViewForm(NNSH_MACROLOG_VIEWTITLE, macroCnt, script);

    // 確保した領域を開放する
    for (lp = 0; lp < macroCnt; lp++)
    {
        if ((script[lp] != NULL)&&(script[lp] != logData))
        {
            MEMFREE_PTR(script[lp]);
        }
    }
    MEMFREE_PTR(script);
    return;
}        
#endif
