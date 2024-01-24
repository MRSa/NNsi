/*============================================================================*
 *  FILE: 
 *     machine.c
 *
 *  Description: 
 *     Palm device specific functions.
 *
 *===========================================================================*/
#define MACHINE_C

#ifdef USE_CLIE
// SONY用ジョグアシストマスク
#define NUMBER_OF_MASKS   3
UInt16 NNshSonyJogMask[] = {
     sonyJogAstMaskType1, // フォーム毎に個別にマスクを設定する
     NUMBER_OF_MASKS,     // マスクするフォーム数
     FRMID_MESSAGE,       sonyJogAstMaskAll,
     FRMID_MANAGEBBS,     sonyJogAstMaskAll,
     FRMID_THREAD,        sonyJogAstMaskAll,
     0, 0, 0, 0           // 怖いから余裕...
};
#endif

extern Err setOfflineLogDir(Char *fileName);

/*-------------------------------------------------------------------------*/
/*   Function :   checkVFSdevice_NNsh                                      */
/*                               VFSのサポート状況をパラメータに反映させる */
/*-------------------------------------------------------------------------*/
static void checkVFSdevice_NNsh(void)
{
    Err            err;
    Char           dirName[sizeof(DATAFILE_PREFIX)+ MARGIN];
    UInt32         vfsMgrVersion, volIterator;
    VolumeInfoType volInfo;  

    // VFSのサポート有無を確認する。
    vfsMgrVersion = 0;
    err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion);
    if (err == errNone)
    {
        //  VFSが使用可能か確認する。(リファレンスマニュアルとおりにした)
        volIterator = vfsIteratorStart;
        while (volIterator != vfsIteratorStop)
        {
            err = VFSVolumeEnumerate(&(NNshGlobal->vfsVol), &volIterator);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "VFSVolumeEnumerate()", "", err);
                break;
            }
            err = VFSVolumeInfo(NNshGlobal->vfsVol, &volInfo);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "VFSVolumeInfo()", "", err);
                break;
            }
            if (volInfo.fsType == vfsFilesystemType_VFAT)
            {
                // "VFS使用可能時に自動的にVFS ON" 設定を確認する
                if (NNshParam->vfsOnAutomatic != 0)
                {
                    // VFS ONにする
                    NNshParam->useVFS = ((NNSH_VFS_ENABLE)|
                                         (NNSH_VFS_WORKAROUND)|
                                         (NNSH_VFS_USEOFFLINE)|
                                         (NNSH_VFS_DBBACKUP));
                }
                if ((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)
                {
                    // NNsh用データ格納用ディレクトリを自動作成(VFS ON時)
                    StrCopy(dirName, DATAFILE_PREFIX);
                    dirName[StrLen(dirName) - 1] = '\0';
                    err = CreateDir_NNsh(dirName);
                    NNsh_DebugMessage(ALTID_INFO, "Created :", dirName, err);
                    err = errNone;
		}
                if ((NNshParam->useVFS & NNSH_VFS_USEOFFLINE) != 0)
                {
                    // NNsh用OFFLINEデータ格納ディレクトリを作成(VFS ON時)
                    StrCopy(dirName, LOGDATAFILE_PREFIX);
                    dirName[StrLen(dirName) - 1] = '\0';
                    err = CreateDir_NNsh(dirName);
                    NNsh_DebugMessage(ALTID_INFO,"Created :",dirName,err);

                    // logdir.txtを読みOFFLINEログベースディレクトリを取得
                    (void) setOfflineLogDir(LOGDIR_FILE);
                    err = errNone;
		}
                break;
            }
        }
    }
    if (err != errNone)
    {
        // デバイスにVFSサポートがない場合は、NNsh設定のVFS使用設定をクリア
        NNshParam->useVFS = NNSH_NOTSUPPORT_VFS;
    }
    return;
}

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :   CLIEdisplayNotify                                        */
/*                                                CLIEディスプレイ更新(仮) */
/*-------------------------------------------------------------------------*/
Err CLIEdisplayNotify(SysNotifyParamType *notifyP)
{
    FormType *frm;

    // ---------------------------------------------------
    //    注意：notifyP は、NULLでコールされる
    // ---------------------------------------------------
    if (NNshGlobal->silkRef != 0)
    {
        frm = FrmGetActiveForm();
        HandEraResizeForm(frm);
        //   画面表示の更新は、標準のイベントループで行う。そのために、
        // 描画指示フラグを変更し、nilEventを発行する。
        NNshGlobal->updateHR = NNSH_UPDATE_DISPLAY;
        EvtWakeup();
    }
    return (errNone);
}
#endif

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :   setCLIESupport_NNsh                                      */
/*                                              CLIEサポート機能を設定する */
/*-------------------------------------------------------------------------*/
static void setCLIESupport_NNsh(void)
{
    SonySysFtrSysInfoP     infoP;
    Boolean                isColor;
    UInt32                 vskVer, width, height, depth;
    Err                    ret;

    infoP                        = NULL;
    NNshGlobal->jogAssistSetBack = NULL;

    // SONYデバイスかどうかのチェック
    if ((FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &depth) != 0)||
        (depth != sonyHwrOEMCompanyID_Sony))
    {
        // SONYデバイスでなければ、何もせず終了する
        return;
    }

    // JOGアシスト機能の設定
    if (!FtrGet(sonySysFtrCreator, sonySysFtrNumSysInfoP, (UInt32 *)&infoP))
    {
        // JOGアシスト機能をもっているかどうかチェックする
        if (infoP && (infoP->extn & sonySysFtrSysInfoExtnJogAst))
        {
            // 現在のジョグアシスト情報を取得
            if(!FtrGet(sonySysFtrCreator, sonySysFtrNumJogAstMaskP,
                       (UInt32 *) &(NNshGlobal->jogAssistSetBack)))
            {
                // 現在のジョグアシスト情報を保管、NNsi用の設定
                 NNshGlobal->jogInfoBack =
                                            *(NNshGlobal->jogAssistSetBack);
                *(NNshGlobal->jogAssistSetBack) =  NNshSonyJogMask;

                // ジョグアシストオーナ系の設定は省略(いらんだろ)
            }
        }
    }


    // SILKマネージャの参照番号を取得する
    ret = SysLibFind(sonySysLibNameSilk, &(NNshGlobal->silkRef));
    if (ret == sysErrLibNotFound)
    {
        // SILKマネージャが未ロードの場合、ロードできるかTryしてみる。
        ret = SysLibLoad('libr',sonySysFileCSilkLib,&(NNshGlobal->silkRef));
    }
    if (ret != errNone)
    {
        // SILKマネージャは使用できない、無効にしてSONY HRLIBロードへ
        NNshGlobal->silkRef = 0;
        goto SONY_HRLIB_LOAD;
    }

    // SILKを使用しない場合には次に進む
    if (NNshGlobal->notUseSilk != 0)
    {
        NNshGlobal->silkRef = 0;
        NNshGlobal->silkVer = 1;
        goto SONY_HRLIB_LOAD;
    }

    // SILKマネージャのバージョン確認
    ret = FtrGet(sonySysFtrCreator, sonySysFtrNumVskVersion, &vskVer);
    if (ret != errNone)
    {
        // SILKマネージャはVERSION1、リサイズのみサポート(NR用)
        if (SilkLibOpen(NNshGlobal->silkRef) == errNone)
        {
            // SILKのリサイズをサポートしていることを(SILKマネージャに)通知
            SilkLibEnableResize(NNshGlobal->silkRef);
            NNshGlobal->silkVer = 1;
        }
    }
    else
    {
        // SILKマネージャはVERSION2、(NX以降)
        if (VskOpen(NNshGlobal->silkRef) == errNone)
        {
            // SILKのリサイズをサポートしていることを(SILKマネージャに)通知
            VskSetState(NNshGlobal->silkRef, vskStateEnable, 1);
            NNshGlobal->silkVer = 2;
        }
    }

    // sysNotifyDisplayChangeEventをsubscribeする
    ret = SysNotifyRegister(0, SOFT_CREATOR_ID, sysNotifyDisplayChangeEvent,
                            CLIEdisplayNotify, sysNotifyNormalPriority,
                            NULL);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_WARN, "SysNotifyRegister()", "", ret);
    }

SONY_HRLIB_LOAD:
    // HRLIBの参照番号を取得する
    ret = SysLibFind(sonySysLibNameHR, &(NNshGlobal->hrRef));
    if (ret == sysErrLibNotFound)
    {
        // HRLIBが未ロードの場合、ロードできるかTryしてみる。
        ret = SysLibLoad('libr', sonySysFileCHRLib, &(NNshGlobal->hrRef));
    }
    if (ret != errNone)
    {
        // HRLIBは使用できない
        NNshGlobal->hrRef = 0;
        return;
    }
    // HRLIBの使用宣言とHRLIBのバージョン番号取得
    HROpen(NNshGlobal->hrRef);
    HRGetAPIVersion(NNshGlobal->hrRef, &(NNshGlobal->hrVer));
    if (NNshGlobal->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
    {
        // 旧バージョンのハイレゾ...画面モードを確認する...
        ret = HRWinScreenMode(NNshGlobal->hrRef, winScreenModeGet, 
                              &width, &height, &depth, &isColor);
        if (ret != errNone)
        {
            // HRLIBは使用しない
            HRClose(NNshGlobal->hrRef);
            NNshGlobal->hrRef = 0;
            return;
        }

        NNshGlobal->prevHRWidth = width;
        if (width != hrWidth)
        {
            // 画面モードがハイレゾでないので、ハイレゾモードに切り替える
            width  = hrWidth;
            height = hrHeight;
            ret    = HRWinScreenMode(NNshGlobal->hrRef, winScreenModeSet,
                                     &width, &height, &depth, &isColor);
            if (ret != errNone)
            {
                // HRLIBは使用しない
                HRClose(NNshGlobal->hrRef);
                NNshGlobal->hrRef = 0;
                return;
            }
        }
    }
    else
    {
        // 画面モードは変更しない。
        NNshGlobal->prevHRWidth = hrWidth;
    }
    return;
}
#endif

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :   resetCLIESupport_NNsh                                    */
/*                                                  CLIEサポートを解放する */
/*-------------------------------------------------------------------------*/
static void resetCLIESupport_NNsh(void)
{
    Boolean                isColor;
    UInt32                 width, height, depth;

    // JOGアシスト機能を元に戻す
    if (NNshGlobal->jogAssistSetBack != NULL)
    {
        *(NNshGlobal->jogAssistSetBack) = NNshGlobal->jogInfoBack;
    }

    if (NNshGlobal->silkRef == 0)
    {
        // SONY用シルク制御の機能を使用しない場合にはHRLIBの解放へ
        goto SONY_HRLIB_UNLOAD;
    }

    // sysNotifyDisplayChangeEventをunsubscribeする
    (void) SysNotifyUnregister(0, SOFT_CREATOR_ID, sysNotifyDisplayChangeEvent,
                               sysNotifyNormalPriority);

    // SILKマネージャの解放
    if (NNshGlobal->silkVer == 1)
    {
        // 旧SILKマネージャ(NR用)
        SilkLibDisableResize(NNshGlobal->silkRef);
        SilkLibClose(NNshGlobal->silkRef);
    }
    else
    {
        // 新SILKマネージャ(NX以降)
        VskSetState(NNshGlobal->silkRef, vskStateEnable, 0);
        VskClose(NNshGlobal->silkRef);
    }

SONY_HRLIB_UNLOAD:
    if (NNshGlobal->hrRef != 0)
    {
        // アプリ起動時に、画面モードが違ったら元に戻す
        if (NNshGlobal->prevHRWidth != hrWidth)
        {
            HRWinScreenMode(NNshGlobal->hrRef, winScreenModeGet,
                            &width, &height, &depth, &isColor);
            width      = stdWidth;
            height     = stdHeight;
            (void) HRWinScreenMode(NNshGlobal->hrRef, winScreenModeSet,
                                   &width, &height, &depth, &isColor);
        }
        // Sony HRLIBの使用終了
        HRClose(NNshGlobal->hrRef);
    }
    return;
}
#endif

#ifdef USE_HANDERA
/*-------------------------------------------------------------------------*/
/*   Function :   setHandEraSupport_NNsh                                   */
/*                                               HandEraデバイスのチェック */
/*-------------------------------------------------------------------------*/
static void setHandEraSupport_NNsh(void)
{
    UInt32              handEraOsVersion;
    VgaScreenModeType   handEraScreenMode;     // スクリーンモード
    VgaRotateModeType   handEraRotateMode;     // 回転モード

    // HandEra QVGA サポート有無を確認する。
    handEraOsVersion = 0;
    if (_TRGVGAFeaturePresent(&handEraOsVersion))          
    {
        NNshGlobal->device = NNSH_DEVICE_HANDERA;
    }

    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
        VgaSetScreenMode(screenMode1To1, handEraRotateMode);
    }
}
#endif // #ifdef USE_HANDERA

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraMoveObject                                        */
/*                                                     オブジェクトの移動  */
/*-------------------------------------------------------------------------*/
static void HandEraMoveObject(FormPtr frmP, UInt16 objIndex,
                              Coord x_diff, Coord y_diff)
{
    Coord   old_x, old_y;

    FrmHideObject(frmP, objIndex);
    FrmGetObjectPosition(frmP, objIndex, &old_x, &old_y);
    if (old_y < 110)
    {
        // 上の方にあるオブジェクトは移動させない
        // (すげーベタだな...ちょっとカッコわる...)
        // x_diff = 0;
        y_diff = 0;
    }
    FrmSetObjectPosition(frmP, objIndex, old_x + x_diff, old_y + y_diff);

    // これだと、invisibleのが見えるな...要検討
    FrmShowObject(frmP, objIndex);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeObject                                      */
/*                                                 オブジェクトのリサイズ  */
/*-------------------------------------------------------------------------*/
static void HandEraResizeObject(FormPtr frmP, UInt16 objIndex,
                                Coord x_diff, Coord y_diff)
{
    RectangleType r;
    Coord   old_x, old_y;

    FrmHideObject(frmP, objIndex);

    // オブジェクトのリサイズ
    FrmGetObjectPosition(frmP, objIndex, &old_x, &old_y);
    if (old_y < 11)
    {
        // 上の方にあるオブジェクトは相手にしない
        // (とりあえず...直値も入れて...)
        return;
    }

    FrmGetObjectBounds(frmP, objIndex, &r);
    r.extent.x += x_diff;
    r.extent.y += y_diff;
    FrmSetObjectBounds(frmP, objIndex, &r);

    FrmShowObject(frmP, objIndex);
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeForm                                        */
/*                                                     フォームのリサイズ  */
/*-------------------------------------------------------------------------*/
void HandEraResizeForm(FormPtr frmP)
{
    Coord           x, y, xDiff, yDiff;
    RectangleType   r;
    UInt16          objectIndex, numObjects, rowNum, nofRow, height;
    Int16           fldIndex, sclIndex;
    TableType      *tableP;
    ControlType    *ctlPtr;
    FormObjectKind  objType;

    fldIndex = -1;
    sclIndex = -1;

    /* 新サイズ */
    WinGetDisplayExtent(&x, &y);
    
    /* 旧サイズ */
    FrmGetFormBounds(frmP, &r);

    /* なぜか以下の微調整が必要（原因不明） */
    r.topLeft.x++;
    r.topLeft.y++;
    r.extent.x -= 2;
    r.extent.y -= 2;

    xDiff = x - (r.topLeft.x + r.extent.x);
    yDiff = y - (r.topLeft.y + r.extent.y);

    if ((xDiff == 0)&&(yDiff == 0))
    {
        return;
    }

    /* リサイズ */
    r.extent.x += xDiff;
    r.extent.y += yDiff;
    WinSetWindowBounds(FrmGetWindowHandle(frmP), &r);
        
    /* オブジェクトの移動、リサイズ */
    numObjects = FrmGetNumberOfObjects(frmP);
    for (objectIndex = 0; objectIndex < numObjects; objectIndex++)
    {
        objType = FrmGetObjectType(frmP, objectIndex);
        switch (objType)
        {
          case frmFieldObj:
            // フィールドの移動（スレ番入力Field）、リサイズ（レス表示Field）
            if (FldGetVisibleLines((FieldType *) 
                                     FrmGetObjectPtr(frmP, objectIndex)) == 1)
            {
                // フィールドの移動
                HandEraMoveObject(frmP, objectIndex, 0, yDiff);
            }
            else
            {
                // フィールドのリサイズ
                HandEraResizeObject(frmP, objectIndex, xDiff, yDiff);
                fldIndex = objectIndex;
            }
            break;
          case frmControlObj:
            // ボタンの移動
            ctlPtr = (ControlType *)FrmGetObjectPtr(frmP, objectIndex);
#ifdef USE_GLUE
            if ((CtlGlueGetControlStyle(ctlPtr) == buttonCtl)||
                (CtlGlueGetControlStyle(ctlPtr) == checkboxCtl)||
                (CtlGlueGetControlStyle(ctlPtr) == repeatingButtonCtl))
#endif
            {
                HandEraMoveObject(frmP, objectIndex, 0, yDiff);
            }
            break;
          case frmTableObj:
            HandEraResizeObject(frmP, objectIndex, xDiff, yDiff);
            tableP = FrmGetObjectPtr(frmP, objectIndex);
            nofRow = TblGetNumberOfRows(tableP);
            TblGetBounds(tableP, &r);
            height = (r.extent.y - r.topLeft.y) / nofRow + 1;
            for (rowNum = 0; rowNum < nofRow; rowNum++)
            {
                TblSetRowHeight(tableP, rowNum, height);
            }
            break;
          case frmListObj:
            // リストのリサイズ
            HandEraResizeObject(frmP, objectIndex, xDiff, yDiff);
            break;

          case frmLabelObj:
          case frmGraffitiStateObj:
            // GSIの移動
            HandEraMoveObject(frmP, objectIndex, 0, yDiff);
            break;

          case frmScrollBarObj:
            // スクロールバーのリサイズ、移動
            HandEraResizeObject(frmP, objectIndex, 0, yDiff);
            HandEraMoveObject(frmP, objectIndex, xDiff, 0);
            sclIndex = objectIndex;
            break;

          default:
            // ???(何もしない)
            break;
        }
    }
    if ((fldIndex >= 0) && (sclIndex >= 0))
    {
        // スクロールバーのアップデート
        NNshWinViewUpdateScrollBar(
              FrmGetObjectId(frmP, fldIndex), FrmGetObjectId(frmP, sclIndex)); 
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraMoveDialog                                        */
/*                                                   ダイヤログの移動      */
/*-------------------------------------------------------------------------*/
void HandEraMoveDialog(FormPtr frmP)
{
    Coord         x, y;
    RectangleType r;

#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        /* 全体のサイズ */
        WinGetDisplayExtent(&x, &y);
    
        /* dialogのサイズ */
        FrmGetFormBounds(frmP, &r);

        /* なぜか以下の微調整が必要（原因不明） */
        r.topLeft.x += 2; 
        r.topLeft.y += 2; 
        r.extent.x -= 4;
        r.extent.y -= 4;
        goto MOVE_OBJECT;
    }
#endif
#ifdef USE_CLIE
    // SILKマネージャがＯＰＥＮされているか？
    if (NNshGlobal->silkRef != 0)
    {
        /* 全体のサイズ */
        WinGetDisplayExtent(&x, &y);
    
        /* dialogのサイズ */
        FrmGetFormBounds(frmP, &r);

        /* なぜか以下の微調整が必要（原因不明） */
        // r.topLeft.x -=2;
        // r.topLeft.y -=2;
        r.extent.x  -= 4;
        r.extent.y  -= 4;
        goto MOVE_OBJECT;
    }
#endif
    return;

MOVE_OBJECT:
    /* dialogの移動 */
    if (y - (r.topLeft.y + r.extent.y) < 0)
    {
        r.topLeft.y = y - r.extent.y;
    }
    WinSetWindowBounds(FrmGetWindowHandle(frmP), &r);

    NNsi_FrmDrawForm(frmP);

    return;
}

/*=========================================================================*/
/*   Function :   NNshRestoreNNsiSetting                                   */
/*                                                     NNsi設定の読み込み  */
/*=========================================================================*/
void NNshRestoreNNsiSetting(Int16 version, UInt16 size, NNshSavedPref *prm)
{
    Err        ret;
    UInt32     offset;
    UInt16     cnt, nofRec;
    DmOpenRef  dbRef;
    Char      *ptr;

    // NNsi設定の領域を初期化する
    nofRec = 0;

    if ((version == SOFT_PREFVERSION)&&(size == sizeof(NNshSavedPref)))
    {
        //  設定されたバージョンと今回起動されたバージョンおよび構造体サイズが
        // 同じだった場合のみ、NNsi設定を復旧させる。

        // 変数を初期化 
        offset = 0;
        ptr    = (Char *) prm;

        // ＤＢより、NNsi設定を読み出す(レコードの登録順の兼ね合いで、、、注意)
        OpenDatabase_NNsh(DBNAME_SETTING, DBVERSION_SETTING, &dbRef);
        GetDBCount_NNsh(dbRef, &nofRec);
        cnt = nofRec;
        while ((cnt != 0)&&(offset < sizeof(NNshSavedPref)))
        {
            ret = GetRecord_NNsh(dbRef,(cnt - 1),DBSIZE_SETTING, &ptr[offset]);
            if (ret != errNone)
            {
                // データ読み出しに失敗(この場合、NNsi設定は初期化する)
                NNsh_InformMessage(ALTID_WARN, MSG_NNSISET_READFAIL, "", ret);
                nofRec = 0;
                break;
            }
            cnt--;
            offset = offset + DBSIZE_SETTING;
        }
        CloseDatabase_NNsh(dbRef);
    }
    if (nofRec == 0)
    {
        // NNsi設定を初期化する
        prm->lastFrmID        = FRMID_THREAD;
        prm->bufferSize       = NNSH_WORKBUF_DEFAULT;
        prm->partGetSize      = NNSH_WORKBUF_DEFAULT;
        prm->netTimeout       = NNSH_NET_TIMEOUT;
        prm->currentFont      = NNSH_DEFAULT_FONT;
        prm->sonyHRFont       = NNSH_DEFAULT_FONT;
        prm->useCookieWrite   = NNSH_ENABLE;
        prm->disableUnderline = NNSH_ENABLE;
        prm->titleListSize    = LIMIT_TITLENAME_DEFAULT;

        // 標準のBBS一覧取得先(URL)を反映させる
        StrCopy(prm->bbs_URL, URL_BBSTABLE);
        NNsh_InformMessage(ALTID_INFO, MSG_NNSISET_REFRESH, "", 0);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshSaveNNsiSetting                                      */
/*                                                         NNsi設定の記憶  */
/*-------------------------------------------------------------------------*/
static void NNshSaveNNsiSetting(Int16 version, UInt16 size,
                                NNshSavedPref *param)
{
    Err       ret;
    UInt32    offset;
    UInt16    nofRec;
    DmOpenRef dbRef;
    Char      *ptr;

    OpenDatabase_NNsh(DBNAME_SETTING, DBVERSION_SETTING, &dbRef);
    GetDBCount_NNsh(dbRef, &nofRec);
    while (nofRec != 0)
    {
        //  すでにNNsi設定が記録されていた場合、全レコードを削除する
        (void) DeleteRecordIDX_NNsh(dbRef, (nofRec - 1));
        nofRec--;
    }

    // NNsi設定をDBに登録する(レコード登録順の兼ね合いに注意！)
    offset = 0;
    ptr    = (Char *) param;
    while (offset < sizeof(NNshSavedPref))
    {
        ret = EntryRecord_NNsh(dbRef, DBSIZE_SETTING, &ptr[offset]);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_WARN, "EntryRecord_NNsh()", "", ret);
            NNsh_DebugMessage(ALTID_WARN, "    ", "offset:", offset);
            NNsh_DebugMessage(ALTID_WARN, "    ", "size:", DBSIZE_SETTING);
            break;
        }
        offset = offset + DBSIZE_SETTING;
    }
    CloseDatabase_NNsh(dbRef);

    return;
}

/*=========================================================================*/
/*   Function :   NNshSilkMinimize                                         */
/*                                          グラフィティエリアを最小化する */
/*=========================================================================*/
Err NNshSilkMinimize(FormType *frm)
{
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
#ifdef USE_CLIE
    // SILKマネージャがＯＰＥＮされているか？
    if (NNshGlobal->silkRef != 0)
    {
        // SILKの最小化を行う
        if (NNshGlobal->silkVer == 1)
        {
            // Version1(NR用)
            SilkLibResizeDispWin(NNshGlobal->silkRef, silkResizeToStatus);
        }
        else
        {
            // Version2(NX以降)
            VskSetState(NNshGlobal->silkRef, vskStateResize, vskResizeMin);
        }
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
    return (~errNone);
}

/*=========================================================================*/
/*   Function :   NNshSilkMaximize                                         */
/*                                          グラフィティエリアを最大化する */
/*=========================================================================*/
Err NNshSilkMaximize(FormType *frm)
{
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMaximizeWindow();
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
#ifdef USE_CLIE
    // SILKマネージャがＯＰＥＮされているか？
    if (NNshGlobal->silkRef != 0)
    {
        // SILKの最大化を行う
        if (NNshGlobal->silkVer == 1)
        {
            // Version1(NR用)
            SilkLibResizeDispWin(NNshGlobal->silkRef, silkResizeNormal);
        }
        else
        {
            // Version2(NX以降)
            VskSetState(NNshGlobal->silkRef, vskStateResize, vskResizeMax);
        }
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
    return (~errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshClearGlobal                                          */
/*                                          NNsiのグローバル領域を開放する */
/*-------------------------------------------------------------------------*/
static void NNshClearGlobal(NNshWorkingInfo *workBuf)
{
    NNshMessageIndex *idxP;

    MEMFREE_HANDLE(workBuf->searchTitleH);
    MEMFREE_HANDLE(workBuf->threadIdxH);
    MEMFREE_HANDLE(workBuf->boardIdxH);
    MEMFREE_HANDLE(workBuf->logPrefixH);

    if (workBuf->threadTitles != NULL)
    {
        MemHandleUnlock(workBuf->msgTTLStringH);
        workBuf->threadTitles = NULL;
    }
    MEMFREE_HANDLE(workBuf->msgTTLStringH);

    MEMFREE_HANDLE(workBuf->bbsTitleH);
    MEMFREE_HANDLE(workBuf->threadTitleH);

    MEMFREE_PTR(workBuf->bbsTitles);

    if (workBuf->msgIndex != NULL)
    {
        idxP = workBuf->msgIndex;
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(workBuf->msgIndex);
    }

    MEMFREE_PTR(workBuf->tempInfo);

    return;
}

/*=========================================================================*/
/*   Function :   BackupDatabaseToVFS                                      */
/*                                         NNsiのDBをVFSにバックアップする */
/*=========================================================================*/
void BackupDBtoVFS(UInt16 backupAllDB)
{
    Err   ret;
    Char *buf, flag;

    flag = 0;

    // BUSYウィンドウの表示領域確保
    buf = MemPtrNew(BUFSIZE);
    if (buf == NULL)
    {

        // エラー表示
        NNsh_DebugMessage(ALTID_ERROR,"Cannot Alloc BUFFER"," size:",BUFSIZE);
        return;
    }
    MemSet(buf, BUFSIZE, 0x00);

    // (BBS-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_BBSLIST);
    Show_BusyForm(buf);
    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_BBSLIST);
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 1;
        }
    }

    // (Subject-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_SUBJECT);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_SUBJECT)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_SUBJECT);
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 2;
        }
    }

    // (Settings-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_SETTING);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_SETTING)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_SETTING);
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 4;
        }
    }

    // BUSY表示を消去する
    Hide_BusyForm();
    MemPtrFree(buf);

    if (flag != 0)
    {
        // バックアップエラーが発生したことを通知する
        NNsh_InformMessage(ALTID_ERROR, MSG_NNSI_DBBACKUP_FAIL, "", flag);
    }
    NNshGlobal->updateDatabaseInfo = 0;
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkInstalledWebBrowser                                 */
/*                                        WebBrowserのインストールチェック */
/*-------------------------------------------------------------------------*/
static void checkInstalledWebBrowser(void)
{
    UInt16            cardNo;
    LocalID           dbID;
    DmSearchStateType searchState;
    Err               ret;

    // NetFrontがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NF3T', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // NetFrontがインストールされていた
        NNshGlobal->browserCreator    = 'NF3T';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // WebBrowser 2.0がインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NF3P', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // WebBrowser 2.0がインストールされていた
        NNshGlobal->browserCreator    = 'NF3P';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Xiino(PalmScape)がインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'PScp', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiinoがインストールされていた
        NNshGlobal->browserCreator    = 'PScp';
        NNshGlobal->browserLaunchCode = 0xfa21;
        return;
    }

    // Xiino(LEM)がインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'dXin', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino(LEM)がインストールされていた
        NNshGlobal->browserCreator    = 'dXin';
        NNshGlobal->browserLaunchCode = 0xfa21;
        return;
    }


    // Blazerがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'BLZ1', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Blazerがインストールされていた
        NNshGlobal->browserCreator    = 'BLZ1';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Eudora Webがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'QCwb', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Eudora Webがインストールされていた
        NNshGlobal->browserCreator    = 'QCwb';
        NNshGlobal->browserLaunchCode = 0xfa0;
        // NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

#if 0
    // PocketLinkがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'PLNK', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // PocketLinkがインストールされていた(but does not work...)
        NNshGlobal->browserCreator    = 'PLNK';
        // NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdOpenDB;
        return;
    }
#endif

    NNsh_DebugMessage(ALTID_INFO, MSG_CANNOTFIND_BROWSER, "", ret);
    return;
}

/*=========================================================================*/
/*   Function : KeyConvertFiveWayToJogChara                                */
/*                                  5Wayの入力コードをジョグのコードに変換 */
/*=========================================================================*/
UInt16 KeyConvertFiveWayToJogChara(EventType *event)
{
    // Tungsten T 5way navigator対応(281さん、感謝！)
    if (event->data.keyDown.chr == vchrNavChange)
    {
        // 5way PUSH
        if (((event->data.keyDown.keyCode) & (navBitSelect | navChangeSelect))
             == navChangeSelect)
        {
            // ジョグ押下と等価にする
            return (vchrJogRelease);
        }

        // LEFT 
        if ((event->data.keyDown.keyCode) & navBitLeft)
        {
            // ジョグ押し上回しと等価にする
            return (vchrJogPushedUp);
        }

        // RIGHT
        if ((event->data.keyDown.keyCode) & navBitRight)
        {
            // ジョグ押し下回しと等価にする
            return (vchrJogPushedDown);
        }
    }

    // 上記以外はそのまま応答する
    return (event->data.keyDown.chr);
}
