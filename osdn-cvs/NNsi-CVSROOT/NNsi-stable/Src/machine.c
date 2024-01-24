/*============================================================================*
 *  FILE: 
 *     machine.c
 *
 *  Description: 
 *     Palm device specific functions.
 *
 *===========================================================================*/
#define MACHINE_C

extern Err setOfflineLogDir(Char *fileName);

static Err VFSUnmountNotify(SysNotifyParamType *notifyParamsP);
static Err VFSMountNotify  (SysNotifyParamType *notifyParamsP);

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
    LocalID        appDbID;
    UInt16         cardNo;

    // VFSのサポート有無を確認する。
    vfsMgrVersion = 0;
    err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion);
    if (err == errNone)
    {
        // VFS Mount & Unmount Notificationの設定
        // (とりあえずVFSMgrがあればVFSが刺さってなくても後からmountできるように登録しておく)
        err = SysCurAppDatabase(&cardNo, &appDbID);
        if (err == errNone)
        {
            // VFSがunmount & remountされた時のnotify handlerを設定
            // shinhei 03/06/07 - PilotMainからHandleするように変更
            err = SysNotifyRegister(cardNo, appDbID, sysNotifyVolumeUnmountedEvent,
                                    NULL, sysNotifyNormalPriority, 0);
            err = SysNotifyRegister(cardNo, appDbID, sysNotifyVolumeMountedEvent,
                                    NULL, sysNotifyNormalPriority, 0);

            // Remountの時のsysNotifyErrDuplicateEntryは無視
            if (err == sysNotifyErrDuplicateEntry)
            {
                err = errNone;
            }
        }

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
            // "VFSはコンパクトフラッシュを使用する"設定の場合、、、
            switch ((NNshGlobal->NNsiParam)->vfsUseCompactFlash)
            {
              case NNSH_VFS_USEMEDIA_CF:             
                if (volInfo.mediaType != ExpMediaType_CompactFlash)
                {
                    // VFSがCFでなければ、もう一度検索する
                    err = ~errNone;
                    continue;
                }
                break;

              case NNSH_VFS_USEMEDIA_MS:             
                if (volInfo.mediaType != ExpMediaType_MemoryStick)
                {
                    // VFSがMSでなければ、もう一度検索する
                    err = ~errNone;
                    continue;
                }
                break;

              case NNSH_VFS_USEMEDIA_SD:             
                if ((volInfo.mediaType != ExpMediaType_SecureDigital)&&
                    (volInfo.mediaType != ExpMediaType_MultiMediaCard))
                {
                    // VFSがSD/MMCでなければ、もう一度検索する
                    err = ~errNone;
                    continue;
                }
                break;

              case NNSH_VFS_USEMEDIA_SM:             
                if (volInfo.mediaType != ExpMediaType_SmartMedia)
                {
                    // VFSがSmartMediaでなければ、もう一度検索する
                    err = ~errNone;
                    continue;
                }
                break;
                
              case NNSH_VFS_USEMEDIA_RD:             
                if (volInfo.mediaType != ExpMediaType_RAMDisk)
                {
                    // VFSがRamDiskでなければ、もう一度検索する
                    err = ~errNone;
                    continue;
                }
                break;

              case NNSH_VFS_USEMEDIA_DoC:
                // 内蔵FS
                if ((volInfo.mediaType != 'Tffs')&&
                    (volInfo.mediaType != 'Isdr'))
                {
                    // VFSが内蔵メディアでなければ、もう一度検索する
                    err = ~errNone;
                    continue;
                }
                break;

              default:
                // 指定しない
                break;
            }
            NNsh_DebugMessage(ALTID_WARN, "mediaType:", "", volInfo.mediaType);
            NNsh_DebugMessage(ALTID_WARN, "fsType:", "",    volInfo.fsType);

            if (volInfo.fsType == vfsFilesystemType_VFAT)
            {                    
                // "VFS使用可能時に自動的にVFS ON" 設定を確認する
                if ((NNshGlobal->NNsiParam)->vfsOnAutomatic != 0)
                {
                    // VFS ONにする
                    (NNshGlobal->NNsiParam)->useVFS =
                                         (NNshGlobal->NNsiParam)->vfsOnDefault;
#if 0
                    // VFS ONにする
                    (NNshGlobal->NNsiParam)->useVFS = ((NNSH_VFS_ENABLE)|
                                         (NNSH_VFS_WORKAROUND)|
                                         (NNSH_VFS_USEOFFLINE)|
                                         (NNSH_VFS_DIROFFLINE)|
                                         (NNSH_VFS_DBBACKUP));
#endif
                                         
                    if ((NNshGlobal->NNsiParam)->vfsOnNotDBCheck == 0)
                    {
                        //  "自動VFS ONでもDBチェックなし" が OFFのとき
                        //  (起動時DBチェックをＯＮにする)
                        (NNshGlobal->NNsiParam)->useVFS = 
                                      ((NNshGlobal->NNsiParam)->useVFS|(NNSH_VFS_DBIMPORT));
                    }
                }
                if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
                {
                    // NNsh用データ格納用ディレクトリを自動作成(VFS ON時)
                    StrCopy(dirName, DATAFILE_PREFIX);
                    dirName[StrLen(dirName) - 1] = '\0';
                    err = CreateDir_NNsh(dirName);
                    NNsh_DebugMessage(ALTID_INFO, "Created :", dirName, err);
                    err = errNone;
                }
                if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
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
        (NNshGlobal->NNsiParam)->useVFS = NNSH_NOTSUPPORT_VFS;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   VFSUnmountNotify                                         */
/*                                       VFSがUnmountされた時のNotify処理  */
/*-------------------------------------------------------------------------*/
static Err VFSUnmountNotify(SysNotifyParamType *notifyParamsP)
{
    UInt16 unmountedVol;

    unmountedVol = ((UInt16) (notifyParamsP->notifyDetailsP));

    // アンマウントされたボリュームは何かチェックする
    if (NNshGlobal->vfsVol != unmountedVol)
    {
        // 現在NNsiが使用中のボリュームではなかった、終了する
        return (errNone);
    }

    // VFSフラグをクリアする
    (NNshGlobal->NNsiParam)->useVFS = 0;

    // Unmount発生時に開いていた画面によって、フラグを設定させる
    switch(FrmGetFormId(FrmGetActiveForm()))
    {
      case FRMID_WRITE_MESSAGE:
        // 書き込み画面
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED_WRITE;  
        break;
        
      case FRMID_MESSAGE:
        // 参照画面
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED_MESSAGE;  
        break;
        
      case FRMID_THREAD:
        // 一覧画面
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED; 
        break; 

      case FRMID_INPUTDIALOG:
      case FRMID_DBIMPORT:
      case FRMID_MESSAGEINFO:
      case FRMID_HARDKEY_SETTING:
      case FRMID_OMITDIALOG_DETAIL:
      case FRMID_FAVORSET_THREADSTATE:
      case FRMID_FAVORSET_THREADTITLE:
        // FrmDoDialogしている場合は一旦前のFormに戻る
        // ただ、DoDialogの返り値をどうするかだが...これはvchrLaunchが飛んで来た時と同じなので気にしなくてもいいか...
        FrmReturnToForm(0);
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED_OTHER;  
        break;
      
      default:
        // その他の画面
        NNshGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED_OTHER;  
        break;
    }

    // nilEvent発行
    // shinehi 03/06/07 <- FrmDoDialog()している最中だと前のFormに戻るまで捕捉してもらえない...
    EvtWakeup();
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   VFSMountNotify                                           */
/*                               VFSがMountされた時のNotify処理            */
/*-------------------------------------------------------------------------*/
static Err VFSMountNotify(SysNotifyParamType *notifyParamsP)
{
    UInt16 frmID;

    // Remount処理
    checkVFSdevice_NNsh();

    // start.prcが実行&AppSwitchされるのを回避
    notifyParamsP->handled |= vfsHandledStartPrc | vfsHandledUIAppSwitch;

    // VFSがmountされたことを通知
    NNshGlobal->notifyUpdate = NNSH_VFS_MOUNTED; 

    // Dialogだったら前のフォームに戻っておく
    frmID = FrmGetFormId(FrmGetActiveForm());
    switch(frmID)
    {
      case FRMID_INPUTDIALOG:
      case FRMID_DBIMPORT:
      case FRMID_MESSAGEINFO:
      case FRMID_HARDKEY_SETTING:
      case FRMID_OMITDIALOG_DETAIL:
      case FRMID_FAVORSET_THREADSTATE:
      case FRMID_FAVORSET_THREADTITLE:
        // FrmDoDialogしている場合は一旦前のFormに戻る
        // ただ、DoDialogの返り値をどうするかだが...これはvchrLaunchが飛んで来た時と同じなので気にしなくてもいいか...
        FrmReturnToForm(0);
        break;
        
    default:
        // do nothing
        break;
    }

    // nilEvent発行
    EvtWakeup();

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   resetVFSNotifications                                    */
/*                                          VFS Notification の unregister */
/*-------------------------------------------------------------------------*/
static void resetVFSNotifications(void)
{
    UInt16 cardNo;
    LocalID appDbID;
  
    // Get application's cardNo and app database localID
    (void) SysCurAppDatabase(&cardNo, &appDbID);
  
    // Unregister unmount & mount notification    
    (void) SysNotifyUnregister(cardNo, appDbID, sysNotifyVolumeUnmountedEvent,
                               sysNotifyNormalPriority);
    (void) SysNotifyUnregister(cardNo, appDbID, sysNotifyVolumeMountedEvent,
                               sysNotifyNormalPriority);
}

#ifdef USE_PIN_DIA
#ifdef USE_PIN_GARMIN
/*-------------------------------------------------------------------------*/
/*   Function :  DisplayResizeNotify                                       */
/*                                                        ディスプレイ更新 */
/*-------------------------------------------------------------------------*/
Err DisplayResizeNotify(SysNotifyParamType *notifyP)
{
    // 画面描画の更新は、mainで行うので、ここでは更新フラグをたてるのみ
    NNshGlobal->notifyUpdate = NNSH_DISP_RESIZED;
    EvtWakeup();

    return (errNone);
}
#endif // #ifdef USE_PIN_GARMIN
#endif // #ifdef USE_PIN_DIA

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
        UpdateFieldRegion();
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
    UInt16                 vskState;
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

    // JOGアシストの設定情報を設定する
    NNshGlobal->sonyJogMask[0]  = sonyJogAstMaskType1;
    NNshGlobal->sonyJogMask[1]  = NUMBER_OF_JOGMASKS;
    NNshGlobal->sonyJogMask[2]  = FRMID_MESSAGE;
    NNshGlobal->sonyJogMask[3]  = sonyJogAstMaskAll;
    NNshGlobal->sonyJogMask[4]  = FRMID_MANAGEBBS;
    NNshGlobal->sonyJogMask[5]  = sonyJogAstMaskAll;
    NNshGlobal->sonyJogMask[6]  = FRMID_THREAD;
    NNshGlobal->sonyJogMask[7]  = sonyJogAstMaskAll;
    NNshGlobal->sonyJogMask[8]  = FRMID_LISTGETLOG;
    NNshGlobal->sonyJogMask[9]  = sonyJogAstMaskAll;
    NNshGlobal->sonyJogMask[10] = 0;
    NNshGlobal->sonyJogMask[11] = 0;

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
                *(NNshGlobal->jogAssistSetBack) =  NNshGlobal->sonyJogMask;

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
        // 281さんのSilkマネージャ判定コードをマージする
        ret = SysLibLoad(sonySysFileTSilkLib,
                         sonySysFileCSilkLib,
                         &(NNshGlobal->silkRef));    // OS5
        if (ret != errNone)
        {
            ret = SysLibLoad('libr','SlSi',&(NNshGlobal->silkRef));
        }
        if (ret != errNone)
        {
            // SILKマネージャは使用できない、無効にしてSONY HRLIBロードへ
            NNshGlobal->silkRef = 0;
            goto SONY_HRLIB_LOAD;
        }
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
            goto SONY_HRLIB_LOAD;
        }

        // SILKバージョンの取得に失敗した時には、SILK制御しない。
        NNshGlobal->silkRef = 0;
        NNshGlobal->silkVer = 1;
        goto SONY_HRLIB_LOAD;
    }
    else
    {
        // マジですか、フィーチャー番号でVer1がエラーにならないってのは,,,
        switch (vskVer)
        {
          case vskVersionNum1:
            // SILKマネージャはVERSION1、リサイズのみサポート(NR用)
            if (SilkLibOpen(NNshGlobal->silkRef) == errNone)
            {
                // SILKのリサイズをサポートしていることを(SILKマネージャに)通知
                SilkLibEnableResize(NNshGlobal->silkRef);
                NNshGlobal->silkVer = 1;
            }
            break;

          case vskVersionNum2:
            // SILKマネージャはVERSION2、(NX以降)
            if (VskOpen(NNshGlobal->silkRef) == errNone)
            {
                // SILKのリサイズ(縦方向)をサポートしていることを(SILKマネージャに)通知
                VskSetState(NNshGlobal->silkRef, vskStateEnable, vskResizeVertically);
                NNshGlobal->silkVer = 2;
            }
            break;

          case vskVersionNum3:
          default:
            // SILKマネージャはVERSION3、(UX以降)、その他のデバイス
            if (VskOpen(NNshGlobal->silkRef) == errNone)
            {
                // SILKのリサイズ(横方向)をサポートしていることを(SILKマネージャに)通知
                ret = VskGetState(NNshGlobal->silkRef, vskStateResizeDirection, &vskState);
                if (ret != errNone)
                {
                    vskState = vskResizeDisable;
                }
                VskSetState(NNshGlobal->silkRef, vskStateEnable, vskState);
                NNshGlobal->silkVer = 3;
            }
            break;
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
    // SONY HiResを使用しない場合には、ここで終了する
    if ((NNshGlobal->NNsiParam)->disableSonyHR != 0)
    {
        // HRLIBは使用しない
        NNshGlobal->hrRef    = 0;
        NNshGlobal->notUseHR = (NNshGlobal->NNsiParam)->disableSonyHR;
        return;
    }

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
        VskSetState(NNshGlobal->silkRef, vskStateEnable, vskResizeDisable);
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

#ifdef USE_PIN_DIA
/*-------------------------------------------------------------------------*/
/*   Function :   setDIASupport_NNsh                                       */
/*                                    Dynamic Input Areaデバイスのチェック */
/*-------------------------------------------------------------------------*/
static void setDIASupport_NNsh(void)
{
    Err    err;
    UInt32 ver, iFlag;

    err = FtrGet(pinCreator, pinFtrAPIVersion, &ver);
    if (err == errNone)
    {
         // DIA フィーチャー確認
         NNsh_DebugMessage(ALTID_INFO, "DIA version:", "", ver);

#ifdef USE_PIN_GARMIN
        // Garmin DIAフィーチャー確認
        err = FtrGet(garminFtrCreator, garminFtrNumPenInputServices, &ver);
        if (err != ftrErrNoSuchFeature)
        {
            // sysNotifyDisplayResizedEventをsubscribeする
            err = SysNotifyRegister(0, SOFT_CREATOR_ID, 
                                    sysNotifyDisplayResizedEvent,
                                    DisplayResizeNotify, 
                                    sysNotifyNormalPriority,
                                    NULL);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "SysNotifyRegister()", "", err);
                return;
            }
            
            // DIAサポート機(Garmin)の設定
            NNshGlobal->device = NNSH_DEVICE_GARMIN;
            NNsh_DebugMessage(ALTID_INFO, "enable GarminDIA", "", 0);

            return;
        }
#endif // #ifdef USE_PIN_GARMIN

         // DIAサポートデバイス    
         err = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &iFlag);
         NNsh_DebugMessage(ALTID_INFO, "DIA flag:", "", iFlag);
         if ((err != errNone)|
             ((iFlag & grfFtrInputAreaFlagDynamic) == 0)||
             ((iFlag & grfFtrInputAreaFlagCollapsible) == 0))
         {
             // エラーになる場合、もしくはDIA非サポートの場合、何もせず抜ける
             return;
         }

         // DIAサポート機の設定
         NNshGlobal->device = NNSH_DEVICE_DIASUPPORT;
         NNsh_DebugMessage(ALTID_INFO, "enable DIA", "", 0);
    }
    return;
}
#endif  // #ifdef USE_PIN_DIA

#ifdef USE_PIN_DIA
/*-------------------------------------------------------------------------*/
/*   Function :   resetDIASupport_NNsh                                     */
/*                                            Dynamic Input Areaの使用開放 */
/*-------------------------------------------------------------------------*/
void resetDIASupport_NNsh(void)
{
    Err    err;
    UInt32 ver;

    // DIA フィーチャー確認
    err = FtrGet(pinCreator, pinFtrAPIVersion, &ver);
    if (err == errNone)
    {

#ifdef USE_PIN_GARMIN
        // Garmin DIAフィーチャー確認
        err = FtrGet(garminFtrCreator, garminFtrNumPenInputServices, &ver);
        if (err != ftrErrNoSuchFeature)
        {
            // sysNotifyDisplayResizedEventをunsubscribeする
            (void) SysNotifyUnregister(0, SOFT_CREATOR_ID,
                                        sysNotifyDisplayResizedEvent,
                                        sysNotifyNormalPriority);
            return;
        }
#endif // #ifdef USE_PIN_GARMIN
    }
    return;
}
#endif //  #ifdef USE_PIN_DIA

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraMoveObject                                        */
/*                                                     オブジェクトの移動  */
/*-------------------------------------------------------------------------*/
static void HandEraMoveObject(FormPtr frmP, UInt16 objIndex,
                              Coord x_diff, Coord y_diff)
{
    Coord   old_x, old_y;

    // 場所の取得
    FrmGetObjectPosition(frmP, objIndex, &old_x, &old_y);
    if (old_y < 110)
    {
        // 上の方にあるオブジェクトは移動させない
        // (すげーベタだな...ちょっとカッコわる...)
        // x_diff = 0;
        y_diff = 0;
    }
    FrmSetObjectPosition(frmP, objIndex, old_x + x_diff, old_y + y_diff);

#ifdef USE_GLUE
    if (FrmGlueGetObjectUsable(frmP, objIndex) != true)
    {
        // 隠れているオブジェクトは表示しない
        FrmHideObject(frmP, objIndex);
    }
    else
#endif
    {
        // 移動したオブジェクトを表示する
        FrmShowObject(frmP, objIndex);
    }

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeObject                                      */
/*                                                 オブジェクトのリサイズ  */
/*-------------------------------------------------------------------------*/
static void HandEraResizeObject(FormPtr frmP, UInt16 objIndex,
                                Coord x_diff, Coord y_diff)
{
    Boolean       disp = true;
    RectangleType r;
    Coord         old_x, old_y;

#ifdef USE_GLUE
    if (FrmGlueGetObjectUsable(frmP, objIndex) != true)
    {
        // 隠れているオブジェクト
        disp = false;
    }
#endif
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

    // 前表示していた場合には、表示する
    if (disp != false)
    {
        FrmShowObject(frmP, objIndex);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeForm                                        */
/*                                                     フォームのリサイズ  */
/*-------------------------------------------------------------------------*/
Boolean HandEraResizeForm(FormPtr frmP)
{
    Coord           x, y, xDiff, yDiff;
    RectangleType   r;
    UInt16          objectIndex, numObjects, rowNum, nofRow, height;
    Int16           fldIndex, sclIndex;
    TableType      *tableP;
    ControlType    *ctlPtr;
    FormObjectKind  objType;
    WinHandle       winH;

    winH = FrmGetWindowHandle(frmP);
    if (WinModal(winH) == true)
    {
        /** モーダルフォームだったら、ウィンドウのリサイズは実施しない **/
        return (false);
    }

    /** 初期化 **/
    fldIndex = -1;
    sclIndex = -1;

#ifdef USE_PIN_DIA
    if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNshGlobal->device == NNSH_DEVICE_GARMIN))
    {
        // DIAの場合、、、

        /* 新サイズ */
        WinGetDisplayExtent(&x, &y);

        /* 旧サイズ */
        WinGetBounds(FrmGetWindowHandle(frmP), &r);
    }
    else
#endif
    {
        /* 新サイズ */
        WinGetDisplayExtent(&x, &y);
    
        /* 旧サイズ */
        FrmGetFormBounds(frmP, &r);

        /* モーダルフォームでない場合、なぜか以下の微調整が必要（原因不明） */
        r.topLeft.x = r.topLeft.x + 1;
        r.topLeft.y = r.topLeft.y + 1;
        r.extent.x  = r.extent.x  - 2;
        r.extent.y  = r.extent.y  - 2;
    }

    xDiff = x - (r.topLeft.x + r.extent.x);
    yDiff = y - (r.topLeft.y + r.extent.y);

    if ((xDiff == 0)&&(yDiff == 0))
    {
        return (false);
    }

    /* リサイズ */
    r.extent.x = r.extent.x + xDiff;
    r.extent.y = r.extent.y + yDiff;
    WinSetWindowBounds(winH, &r);
        
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
            // ボタン関連の移動
            ctlPtr = (ControlType *)FrmGetObjectPtr(frmP, objectIndex);
#ifdef USE_GLUE
            if (CtlGlueGetControlStyle(ctlPtr) == repeatingButtonCtl)
            {
                // リピートボタンのみx方向にも移動させる
                HandEraMoveObject(frmP, objectIndex, xDiff, yDiff);
            }
            else
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

          case frmGadgetObj:
            // ガジェットのリサイズ
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

          case frmListObj:
            // リストオブジェクトは何もしない。

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
    return (true);
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

    NNsi_FrmDrawForm(frmP, true);

    return;
}

/*=========================================================================*/
/*   Function :   NNshRestoreNNsiSetting                                   */
/*                                                     NNsi設定の読み込み  */
/*=========================================================================*/
void NNshRestoreNNsiSetting(Int16 version, UInt16 size, NNshSavedPref *prm)
{
    Err           ret;
    UInt32        offset;
    UInt16        cnt, nofRec;
    DmOpenRef     dbRef;
    Char         *ptr;
    RGBColorType  color;

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
                NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_READFAIL, "", ret);
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
        prm->lastFrmID            = NNSH_FRMID_THREAD;
        prm->bufferSize           = NNSH_WORKBUF_DEFAULT;
        prm->partGetSize          = NNSH_WORKBUF_DEFAULT;
        prm->netTimeout           = NNSH_NET_TIMEOUT;
        prm->currentFont          = NNSH_DEFAULT_FONT;
        prm->sonyHRFont           = NNSH_DEFAULT_FONT_HR;
        prm->sonyHRFontTitle      = NNSH_DEFAULT_FONT_HR;
        prm->useCookieWrite       = NNSH_ENABLE;
        prm->disableUnderline     = NNSH_ENABLE;
        prm->writeBufferSize      = NNSH_WRITEBUF_DEFAULT;
        prm->writeAlwaysSage      = NNSH_ENABLE;
        prm->printNofMessage      = NNSH_ENABLE;
        prm->notListReadOnly      = NNSH_ENABLE;
        prm->autoOpenNotRead      = NNSH_ENABLE;
        prm->addReturnToList      = NNSH_ENABLE;
        prm->addMenuGraphView     = NNSH_ENABLE;
        prm->addMenuBackRtnMsg    = NNSH_ENABLE;
        prm->addMenuGraphTitle    = NNSH_ENABLE;
        prm->addMenuDirSelect     = NNSH_ENABLE;
        prm->newArrivalNotRead    = NNSH_SELBBS_NOTREAD;
        prm->displayFavorLevel    = NNSH_MSGATTR_FAVOR_L1;
        prm->useSonyTinyFontTitle = NNSH_ENABLE;
        prm->useSonyTinyFont      = NNSH_ENABLE;
        prm->msgNumLimit          = NNSH_MESSAGE_MAXNUMBER;
        prm->viewMultiBtnFeature  = NNSH_DEFAULT_VIEW_LVLBTNFEATURE;
        prm->viewTitleSelFeature  = NNSH_DEFAULT_VIEW_TITLESELFEATURE;
        prm->dispBottom           = NNSH_ENABLE;
        prm->bbsOverwrite         = NNSH_ENABLE;
        prm->browseMesNum         = 50;
        prm->prepareDAforNewArrival = NNSH_CREATORID_PREPARE_NEWARRIVAL;
        StrCopy(prm->messageSeparator, "\n------\n");

#ifdef USE_COLOR
        // OS 3.5以上の場合
        if (NNshGlobal->palmOSVersion >= 0x03503000)
        {
            // 赤色(エラー発生)
            MemSet(&color, sizeof(color), 0x00);
            color.r = 255;
            prm->colorError       = WinRGBToIndex(&color);

            // 黒(全て読んだ、1000超、上下ボタン)
            MemSet(&color, sizeof(color), 0x00);
            prm->colorAlready        = WinRGBToIndex(&color);
            prm->colorOver           = prm->colorAlready;
            prm->colorButton         = prm->colorOver;
            prm->colorViewFG         = prm->colorButton;
            prm->colorMsgHeader      = prm->colorButton;
            prm->colorMsgHeaderBold  = prm->colorButton;
            prm->colorMsgFooter      = prm->colorButton;
            prm->colorMsgFooterBold  = prm->colorButton;
            prm->colorMsgInform      = prm->colorButton;
            prm->colorMsgInformBold  = prm->colorButton;

            // 薄い青(未読あり)
            // MemSet(&color, sizeof(color), 0x00);
            color.b = 100;
            prm->colorRemain      = WinRGBToIndex(&color);

            // 青(新規取得、差分取得)
            MemSet(&color, sizeof(color), 0x00);
            color.b = 255;
            prm->colorNew         = WinRGBToIndex(&color);
            prm->colorUpdate      = prm->colorNew;

            // 灰色(未取得、その他)
            MemSet(&color, sizeof(color), 0x00);
            color.r = 128;
            color.g = 128;
            color.b = 128;
            prm->colorNotYet      = WinRGBToIndex(&color);
            prm->colorUnknown     = prm->colorNotYet;
            prm->colorOver        = prm->colorNotYet;
        }
#endif
        // 多目的スイッチ１(一覧画面)
        prm->usageOfTitleMultiSwitch1 = NNSH_SWITCH1USAGE_DEFAULT;

        // 多目的スイッチ２(一覧画面)
        prm->usageOfTitleMultiSwitch2 = NNSH_SWITCH2USAGE_DEFAULT;

        // 多目的ボタン１(一覧画面)
        StrCopy(prm->multiBtn1Caption, MULTIBTN1_CAPTION_DEFAULT);
        prm->multiBtn1Feature = ((MULTIBTN1_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 多目的ボタン２(一覧画面)
        StrCopy(prm->multiBtn2Caption, MULTIBTN2_CAPTION_DEFAULT);
        prm->multiBtn2Feature = ((MULTIBTN2_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 多目的ボタン３(一覧画面)
        StrCopy(prm->multiBtn3Caption, MULTIBTN3_CAPTION_DEFAULT);
        prm->multiBtn3Feature = ((MULTIBTN3_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 多目的ボタン４(一覧画面)
        StrCopy(prm->multiBtn4Caption, MULTIBTN4_CAPTION_DEFAULT);
        prm->multiBtn4Feature = ((MULTIBTN4_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 多目的ボタン５(一覧画面)
        StrCopy(prm->multiBtn5Caption, MULTIBTN5_CAPTION_DEFAULT);
        prm->multiBtn5Feature = ((MULTIBTN5_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 多目的ボタン６(一覧画面)
        StrCopy(prm->multiBtn6Caption, MULTIBTN6_CAPTION_DEFAULT);
        prm->multiBtn6Feature = ((MULTIBTN6_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 一覧時ハードキー上設定
        prm->ttlFtr.up = ((UPBTN_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 一覧時ハードキー下設定
        prm->ttlFtr.down = ((DOWNBTN_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 一覧時JOG Pushボタン設定
        prm->ttlFtr.jogPush = ((JOGPUSH_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 一覧時JOG Backボタン設定
        prm->ttlFtr.jogBack = ((JOGBACK_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 一覧時Captureボタン設定
        prm->ttlFtr.clieCapture = ((CAPTURE_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 一覧時左ボタン設定
        prm->ttlFtr.left = ((LEFTBTN_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 一覧時右ボタン設定
        prm->ttlFtr.right = ((RIGHTBTN_FEATURE_DEFAULT)|(MULTIBTN_FEATURE));

        // 参照時ハードキー上設定
        prm->viewFtr.up   = ((UPBTN_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // 参照時ハードキー下設定
        prm->viewFtr.down = ((DOWNBTN_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // 参照時 JOG Pushボタン設定
        prm->viewFtr.jogPush     = ((JOGPUSH_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // 参照時 JOG Backボタン設定
        prm->viewFtr.jogBack     = ((JOGBACK_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // 参照時 Captureボタン設定
        prm->viewFtr.clieCapture = ((CAPTURE_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // 参照時左ボタン設定
        prm->viewFtr.left = ((LEFTBTN_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // 参照時右ボタン設定
        prm->viewFtr.right = ((RIGHTBTN_VIEWFEATURE_DEFAULT)|(MULTIVIEWBTN_FEATURE));

        // 左右ボタンの標準設定
        prm->useKey.left   = HARDKEY_FEATURE_CONTROLNNSI;
        prm->useKey.right  = HARDKEY_FEATURE_CONTROLNNSI;

        // ユーザ設定１の確認
        prm->custom1.newArrival = NNSH_SELBBS_CUSTOM1;
        prm->custom1.getList    = NNSH_SELBBS_CUSTOM1;
        prm->custom1.boardNick  = NNSH_SELBBS_CUSTOM1;
        StrCopy(prm->custom1.tabName, CUSTOM1_THREAD_NAME);
        prm->custom1.condition  = NNSH_CONDITION_GETERR_ERROR;

        // ユーザ設定２の確認(Palm関連のスレ抽出設定)
        prm->custom2.newArrival = NNSH_SELBBS_CUSTOM2;
        prm->custom2.getList    = NNSH_SELBBS_CUSTOM2;
        prm->custom2.boardNick  = NNSH_SELBBS_CUSTOM2;

        StrCopy(prm->custom2.tabName, CUSTOM2_THREAD_NAME);
        StrCopy(prm->custom2.string1, NNSH_STRING_CUSTOM2_DEFAULT1);
        StrCopy(prm->custom2.string2, NNSH_STRING_CUSTOM2_DEFAULT2);
        StrCopy(prm->custom2.string3, NNSH_STRING_CUSTOM2_DEFAULT3);
        prm->custom2.stringSet  = NNSH_STRING_SELECTION | NNSH_STRING_OR;

        // ユーザ設定３の確認(スレ立て３日以内)
        prm->custom3.newArrival = NNSH_SELBBS_CUSTOM3;
        prm->custom3.getList    = NNSH_SELBBS_CUSTOM3;
        prm->custom3.boardNick  = NNSH_SELBBS_CUSTOM3;
        StrCopy(prm->custom3.tabName, CUSTOM3_THREAD_NAME);
        prm->custom3.threadCreate = 3;

        // ユーザ設定４の確認(最大超え)
        prm->custom4.newArrival = NNSH_SELBBS_CUSTOM4;
        prm->custom4.getList    = NNSH_SELBBS_CUSTOM4;
        prm->custom4.boardNick  = NNSH_SELBBS_CUSTOM4;
        StrCopy(prm->custom4.tabName, CUSTOM4_THREAD_NAME);
        prm->custom4.threadStatus = (1 << NNSH_SUBJSTATUS_OVER);
        
        // ユーザ設定５の確認
        prm->custom5.newArrival = NNSH_SELBBS_CUSTOM5;
        prm->custom5.getList    = NNSH_SELBBS_CUSTOM5;
        prm->custom5.boardNick  = NNSH_SELBBS_CUSTOM5;
        StrCopy(prm->custom5.tabName, CUSTOM5_THREAD_NAME);
        prm->custom5.condition  = NNSH_CONDITION_GETRSV_RSV;
        
        // 一覧表示時に重複の確認を実施するよう変更する
        prm->checkDuplicateThread = NNSH_ENABLE;

        // 通信タイムアウト発生時、１回再試行するよう変更する
        prm->nofRetry = 1;

        // 新着時エラーが発生すると再取得するよう変更する
        prm->autoUpdateGetError   = NNSH_ENABLE;

        // 参照ログの取得レベルを１に設定
        prm->getROLogLevel = 1;

        // 参照ログ一覧画面 Jog PUSHは、選択メニューを開く
        prm->getLogFeature.jogPush   = NNSH_GETLOG_FEATURE_OPENMENU;

        // 参照ログ一覧画面 Jog Backは、一覧画面に戻る
        prm->getLogFeature.jogBack   = NNSH_GETLOG_FEATURE_CLOSE_GETLOG;

        // 参照ログ一覧画面 Jog Leftは、前ページを表示する
        prm->getLogFeature.left      = NNSH_GETLOG_FEATURE_PREVPAGE;

        // 参照ログ一覧画面 Jog Rightは、次ページを表示する
        prm->getLogFeature.right     = NNSH_GETLOG_FEATURE_NEXTPAGE;

        // 参照ログ一覧画面 Upは、前アイテムを表示する
        prm->getLogFeature.up        = NNSH_GETLOG_FEATURE_PREVITEM;

        // 参照ログ一覧画面 Downは、次アイテムを表示する
        prm->getLogFeature.down      = NNSH_GETLOG_FEATURE_NEXTITEM;

        // VFSを自動 ONにする設定(初期値)...
        prm->vfsOnDefault            = ((NNSH_VFS_ENABLE)|
                                        (NNSH_VFS_WORKAROUND)|
                                        (NNSH_VFS_USEOFFLINE)|
                                        (NNSH_VFS_DIROFFLINE)|
                                        (NNSH_VFS_DBBACKUP));

        // 検索時に指定する文字列ヘッダ
        StrCopy(prm->viewSearchStrHeader, NNSH_VIEW_SEARCHSTRING_HEADER);

        // 標準のBBS一覧取得先(URL)を反映させる
        StrCopy(prm->bbs_URL, URL_BBSTABLE);
        NNsh_WarningMessage(ALTID_WARN, MSG_NNSISET_REFRESH, "", 0);
    }

    // 絞込み検索モードをクリアする...
    prm->searchPickupMode = 0;

    // NG-1およびNG-2の設定で","で区切ると複数のNGワードと認識する
    prm->useRegularExpression = 1;

    // ログ区切りDBをオープンする
    OpenDatabase_NNsh(DBNAME_LOGTOKEN, DBVERSION_LOGTOKEN, &dbRef);
    GetDBCount_NNsh(dbRef, &nofRec);
    if (nofRec == 0)
    {
        // レコードが１件もなかった場合には、１件データを追加する
        NNshLogTokenDatabase  dmyData;
        MemSet(&dmyData, sizeof(NNshLogTokenDatabase), 0x00);
        dmyData.usable = 1;
        StrCopy(dmyData.tokenPatternName, "(none)");
        EntryRecord_NNsh(dbRef, sizeof(NNshLogTokenDatabase), &dmyData);
    }
    CloseDatabase_NNsh(dbRef);

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
#ifdef USE_PIN_DIA
    if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNshGlobal->device == NNSH_DEVICE_GARMIN))
    {
#if 0
        if (PINSetInputAreaState(pinInputAreaClosed) != errNone)
        {
            HandEraResizeForm(frm);
        }
#else
        // 必ずリサイズするようにしてみる...
        PINSetInputAreaState(pinInputAreaClosed);
        HandEraResizeForm(frm);
#endif
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
#ifdef USE_PIN_DIA
    if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNshGlobal->device == NNSH_DEVICE_GARMIN))
    {
        if (PINSetInputAreaState(pinInputAreaOpen) == errNone)
        {
            HandEraResizeForm(frm);
        }
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

#ifdef USE_PIN_DIA
    // pen input managerの設定解除
    resetDIASupport_NNsh();
#endif    // #ifdef USE_PIN_DIA

#ifdef USE_LOGCHARGE
    // (確保済みなら)参照ログ一覧画面の領域を開放する
    if (workBuf->dispList != NULL)
    {
        ClearList_getLogList(workBuf->dispList);
        MEMFREE_PTR(workBuf->dispList);
    }
#endif  // #ifdef USE_LOGCHARGE

    // (確保済みなら)NG設定の領域を開放する
    ReleaseWordList(&(workBuf->hide1));
    ReleaseWordList(&(workBuf->hide2));

    // (確保済みなら)しおり設定関連の領域をクリアする
    MEMFREE_PTR(workBuf->bookmarkFileName);
    MEMFREE_PTR(workBuf->bookmarkNick);

    // グローバル領域のハンドルをクリアする
    MEMFREE_HANDLE(workBuf->searchTitleH);
    MEMFREE_HANDLE(workBuf->boardIdxH);
    MEMFREE_HANDLE(workBuf->logPrefixH);
    MEMFREE_HANDLE(workBuf->bbsTitleH);

    MEMFREE_PTR(workBuf->bbsTitles);
    MEMFREE_PTR(workBuf->customizedUserAgent);
    MEMFREE_PTR(workBuf->connectedSessionId);
    MEMFREE_PTR(workBuf->be2chCookie);

    if (workBuf->msgIndex != NULL)
    {
        MEMFREE_PTR((workBuf->msgIndex)->msgOffset);
        idxP = workBuf->msgIndex;
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(workBuf->msgIndex);
    }

    if (workBuf->tempInfo != NULL)
    {
        MEMFREE_PTR((workBuf->tempInfo)->checkBoxState);
        MEMFREE_PTR((workBuf->tempInfo)->BBSName_Temp);
        MEMFREE_PTR(workBuf->tempInfo);
    }

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
    buf = MEMALLOC_PTR(BUFSIZE);
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

#ifdef USE_LOGCHARGE
    // (LogPermanent-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_PERMANENTLOG);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_LOGCHARGE)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_PERMANENTLOG);
#if 0
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 8;
        }
#endif
    }
#endif // #ifdef USE_LOGCHARGE

    // (GetReserve-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_GETRESERVE);
    SetMsg_BusyForm(buf);

    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_GETRESERVE);
#if 0
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 16;
        }
#endif
    }

    // (DirIndex-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_DIRINDEX);
    SetMsg_BusyForm(buf);

    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_DIRINDEX);
#if 0
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 32;
        }
#endif
    }

    // (NGWord-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_NGWORD);
    SetMsg_BusyForm(buf);

    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_NGWORD);
#if 0
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 64;
        }
#endif
    }

#ifdef USE_MACRO
    // (Macro-NNsi のバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf,DBNAME_MACROSCRIPT);
    SetMsg_BusyForm(buf);

    if (backupAllDB != 0)
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_MACROSCRIPT);
#if 0
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 128;
        }
#endif
    }
#endif // #ifdef USE_MACRO

#ifdef USE_LOGCHARGE
    // (LogToken-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_LOGTOKEN);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_LOGTOKEN)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_LOGTOKEN);
#if 0
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 256;
        }
#endif
    }
#endif // #ifdef USE_LOGCHARGE

#ifdef USE_LOGCHARGE
    // (LogTemporary-NNsiのバックアップ)
    StrCopy(buf, MSG_NNSI_DBBACKUP_BUSY);
    StrCat (buf, DBNAME_TEMPORARYLOG);
    SetMsg_BusyForm(buf);

    if ((backupAllDB != 0)||
        (((NNshGlobal->updateDatabaseInfo) & (NNSH_DB_UPDATE_LOGSUBDB)) != 0))
    {
        ret = BackupDatabaseToVFS_NNsh(DBNAME_TEMPORARYLOG);
#if 0
        if (ret != errNone)
        {
            // エラーでも抜けない
            flag = flag + 512;
        }
#endif
    }
#endif // #ifdef USE_LOGCHARGE

    // BUSY表示を消去する
    Hide_BusyForm(false);

    MEMFREE_PTR(buf);
    if (flag != 0)
    {
        // バックアップエラーが発生したことを通知する
        NNsh_WarningMessage(ALTID_ERROR, MSG_NNSI_DBBACKUP_FAIL, "", flag);
    }
    NNshGlobal->updateDatabaseInfo = 0;

    return;
}

/*=========================================================================*/
/*   Function :   RestoreDBtoVFS                                           */
/*                                           NNsiのDBをVFSからリストアする */
/*=========================================================================*/
Err RestoreDBtoVFS(Boolean forceRead)
{
    Err      ret, ret1, ret2;
    FileRef  fileRef;
    Char     fileName[MAXLENGTH_FILENAME];
    UInt32   dummy;
    UInt16   butId;
    NNshVFSDBInfo vfsDB[3];
    UInt16 idx;
    ControlType  *chkObj, *lblObj;
    UInt8 fileExistCnt = 0;
    UInt8 importFileNum, deleteFileNum;
    NNshDBInfo dbInfo;
    DateTimeType date;
    
    struct {
        UInt16 chkIdImport;
        UInt16 chkIdDelete;
        UInt16 idx;
        UInt16 lblIdMEM;
        UInt16 lblIdVFS;
    } importInfo[] = {
          {CHKID_IMPORT_SETTING, CHKID_DELETE_SETTING, NNSH_VFSDB_SETTING, LBLID_DBMEM_SETTING, LBLID_DBVFS_SETTING},
          {CHKID_IMPORT_SUBJECT, CHKID_DELETE_SUBJECT, NNSH_VFSDB_SUBJECT, LBLID_DBMEM_SUBJECT, LBLID_DBVFS_SUBJECT},
          {CHKID_IMPORT_BBSLIST, CHKID_DELETE_BBSLIST, NNSH_VFSDB_BBSLIST, LBLID_DBMEM_BBSLIST, LBLID_DBVFS_BBSLIST},
    };
    int i;
            
    
    importFileNum = 0;
    deleteFileNum = 0;
    MemSet(vfsDB, sizeof(vfsDB), 0x00);
    MemSet(&dbInfo, sizeof(dbInfo), 0x00);
    
    // VFSのサポート有無を確認する。
    dummy = 0;
    ret = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &dummy);
    if (ret != errNone)
    {
        // VFS非サポートの場合には、即終了。
        NNsh_DebugMessage(ALTID_INFO, "Not Support VFS :", "", ret);
        return (~errNone);
    }

    // VFSが使用できるとき、BackupされたDBがあるか(FILEをOPENして)確認
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(fileName, DATAFILE_PREFIX);
    StrCat (fileName, DBNAME_SUBJECT);
    StrCat (fileName, ".pdb");
    if (VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &fileRef) !=
        errNone)
    {
        if (forceRead == true)
        {
            // BACKUPされたファイルがないので終了する
            NNsh_DebugMessage(ALTID_INFO, "Backup DB files do not exist", "", 0);
            return (~errNone);
        }
        vfsDB[NNSH_VFSDB_SUBJECT].fileExist = 0;
    }
    else
    {
        vfsDB[NNSH_VFSDB_SUBJECT].fileExist = 1;
        VFSFileDBInfo(fileRef, NULL, NULL, NULL, NULL, &vfsDB[NNSH_VFSDB_SUBJECT].modDateVFS,
                      NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        fileExistCnt++;
    }
    // File Open成功(== DBがある)、リストアするか確認する
    VFSFileClose(fileRef);
    
    if (forceRead == true)
    {
        butId = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_RESTORE, "", 0);
        if (butId != 0)
        {
            // Cancelが選択されたため、リストアしない。
            NNsh_DebugMessage(ALTID_INFO, "DB-Restore is canceled.", "", 0);
            return (~errNone);
        }
    }
    else
    {
        FormPtr frm, curFrm;
        Char *p;
        
        // ファイルが存在するか確認する
        StrCopy(fileName, DATAFILE_PREFIX);
        StrCat (fileName, DBNAME_SETTING);
        StrCat (fileName, ".pdb");

        if (VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &fileRef) !=
            errNone)
            vfsDB[NNSH_VFSDB_SETTING].fileExist = 0;
        else
        {
            vfsDB[NNSH_VFSDB_SETTING].fileExist = 1;
            VFSFileDBInfo(fileRef, NULL, NULL, NULL, NULL, &vfsDB[NNSH_VFSDB_SETTING].modDateVFS,
                          NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            fileExistCnt++;
        }
        VFSFileClose(fileRef);

        StrCopy(fileName, DATAFILE_PREFIX);
        StrCat (fileName, DBNAME_BBSLIST);
        StrCat (fileName, ".pdb");

        if (VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &fileRef) !=
            errNone)
            vfsDB[NNSH_VFSDB_BBSLIST].fileExist = 0;
        else
        {
            vfsDB[NNSH_VFSDB_BBSLIST].fileExist = 1;
            VFSFileDBInfo(fileRef, NULL, NULL, NULL, NULL, &vfsDB[NNSH_VFSDB_BBSLIST].modDateVFS,
                          NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            fileExistCnt++;
        }
        VFSFileClose(fileRef);

        // VFS 上に DB ファイルがないため、リストアしない。
        if (!fileExistCnt)
            return(~errNone);

        // 本体内にあるDBの情報を取得
        dbInfo.modDateP = &vfsDB[NNSH_VFSDB_SETTING].modDateMEM;
        StrCopy(fileName, DBNAME_SETTING);
        GetDBInfo_NNsh(fileName, &dbInfo);

        dbInfo.modDateP = &vfsDB[NNSH_VFSDB_SUBJECT].modDateMEM;
        StrCopy(fileName, DBNAME_SUBJECT);
        GetDBInfo_NNsh(fileName, &dbInfo);
        
        dbInfo.modDateP = &vfsDB[NNSH_VFSDB_BBSLIST].modDateMEM;
        StrCopy(fileName, DBNAME_BBSLIST);
        GetDBInfo_NNsh(fileName, &dbInfo);
        
        curFrm = FrmGetActiveForm();
        frm = FrmInitForm(FRMID_DBIMPORT);

        // ファイルが存在しない物についてはチェックボックスを隠す
        for (i = 0; i < sizeof(importInfo) / sizeof(importInfo[0]); i++)
        {
            if (vfsDB[importInfo[i].idx].fileExist == 0)
            {
                idx = FrmGetObjectIndex(frm, importInfo[i].chkIdImport);
                chkObj = FrmGetObjectPtr(frm, idx);
                
                CtlSetValue(chkObj, 0);
                FrmHideObject(frm, idx);

                idx = FrmGetObjectIndex(frm, importInfo[i].chkIdDelete);
                chkObj = FrmGetObjectPtr(frm, idx);

                CtlSetValue(chkObj, 0);
                FrmHideObject(frm, idx);
            }
            else
            {
                idx = FrmGetObjectIndex(frm, importInfo[i].lblIdVFS);
                lblObj = FrmGetObjectPtr(frm, idx);
                TimSecondsToDateTime(vfsDB[importInfo[i].idx].modDateVFS, &date);
                        
                p = vfsDB[importInfo[i].idx].dateStrVFS;

                StrIToA(p, date.year);
                StrCat(p, "/");
                StrIToA(p + StrLen(p), date.month);
                StrCat(p, "/");
                StrIToA(p + StrLen(p), date.day);
                StrCat(p, " ");
                StrIToA(p + StrLen(p), date.hour);
                StrCat(p, ":");
                StrIToA(p + StrLen(p), date.minute);
                StrCat(p, ":");
                StrIToA(p + StrLen(p), date.second);
                
                CtlSetLabel(lblObj, p);
            }

            idx = FrmGetObjectIndex(frm, importInfo[i].lblIdMEM);
            lblObj = FrmGetObjectPtr(frm, idx);
            TimSecondsToDateTime(vfsDB[importInfo[i].idx].modDateMEM, &date);
            
            p = vfsDB[importInfo[i].idx].dateStrMEM;
            StrIToA(p, date.year);
            StrCat(p, "/");
            StrIToA(p + StrLen(p), date.month);
            StrCat(p, "/");
            StrIToA(p + StrLen(p), date.day);
            StrCat(p, " ");
            StrIToA(p + StrLen(p), date.hour);
            StrCat(p, ":");
            StrIToA(p + StrLen(p), date.minute);
            StrCat(p, ":");
            StrIToA(p + StrLen(p), date.second);
                
            CtlSetLabel(lblObj, p);
        }
        
        if(FrmDoDialog(frm) == BTNID_DIALOG_OK)
        {
            for(i = 0; i < sizeof(importInfo) / sizeof(importInfo[0]); i++)
            {
                chkObj = FrmGetObjectPtr(frm,
                                         FrmGetObjectIndex(frm, importInfo[i].chkIdImport));
                vfsDB[importInfo[i].idx].importDB = CtlGetValue(chkObj);
                if (vfsDB[importInfo[i].idx].importDB)
                    importFileNum++;

                chkObj = FrmGetObjectPtr(frm,
                                         FrmGetObjectIndex(frm, importInfo[i].chkIdDelete));
                vfsDB[importInfo[i].idx].deleteFile = CtlGetValue(chkObj);
                if (vfsDB[importInfo[i].idx].deleteFile)
                    deleteFileNum++;
            
            }
            FrmDeleteForm(frm);
            FrmSetActiveForm(curFrm);
        }
        else
        {
            FrmDeleteForm(frm);
            FrmSetActiveForm(curFrm);
            return(~errNone);
        }
    }

    if (importFileNum || forceRead == true)
    {
        // "リストア中"表示
        Show_BusyForm(MSG_DBRESTORE_BUSY);
    }
    else
    {
        if (deleteFileNum)
        {
            // "削除中"表示
            Show_BusyForm(MSG_DBDELETE_BUSY);
        }
        else
        {
            // リストア、削除しない
            return(~errNone);
        }
    }

    // DBリストアメイン
    if (forceRead == true)
        (void) RestoreDatabaseFromVFS_NNsh(DBNAME_DIRINDEX);
        
    if (vfsDB[NNSH_VFSDB_SETTING].importDB || forceRead == true)
    {
        ret2 = RestoreDatabaseFromVFS_NNsh(DBNAME_SETTING);
        NNshGlobal->restoreSetting = 1;
    }
    else
    {
        NNshGlobal->restoreSetting = 0;
        ret2 = errNone;
    }
        
    if (forceRead == false && vfsDB[NNSH_VFSDB_SETTING].deleteFile && ret2 == errNone)
    {
        StrCopy(fileName, DBNAME_SETTING);
        StrCat(fileName, ".pdb");
        
        // ファイルを削除する
        (void)DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }
    
    if (vfsDB[NNSH_VFSDB_SUBJECT].importDB || forceRead == true)
        ret1 = (RestoreDatabaseFromVFS_NNsh(DBNAME_SUBJECT));
    else
        ret1 = errNone;
    
    if (forceRead == false && vfsDB[NNSH_VFSDB_SUBJECT].deleteFile && ret1 == errNone)
    {
        StrCopy(fileName, DBNAME_SUBJECT);
        StrCat(fileName, ".pdb");
        
        // ファイルを削除する
        (void)DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }
    
    if (vfsDB[NNSH_VFSDB_BBSLIST].importDB || forceRead == true)
        ret = RestoreDatabaseFromVFS_NNsh(DBNAME_BBSLIST);
    else
        ret = errNone;
    
    if (forceRead == false && vfsDB[NNSH_VFSDB_BBSLIST].deleteFile && ret == errNone)
    {
        StrCopy(fileName, DBNAME_BBSLIST);
        StrCat(fileName, ".pdb");
        
        // ファイルを削除する
        (void)DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }

    // "リストア中" or "削除中" 表示の削除
    Hide_BusyForm(true);

    if (ret2 != errNone)
    {
        // リストア失敗(スレ一覧)
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_NG, DBNAME_SETTING, 0);
    }

    if (ret1 != errNone)
    {
        // リストア失敗(スレ一覧)
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_NG, DBNAME_SUBJECT, 0);
    }

    if (ret != errNone)
    {
        // リストア失敗(BBS一覧)
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_NG, DBNAME_BBSLIST, 0);
    }

    if (importFileNum || forceRead == true)
    {
        // リストア成功
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_OK, "", 0);
    }

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   SetZLIBsupport_NNsh                                      */
/*                                              ZLIBのインストールチェック */
/*-------------------------------------------------------------------------*/
void SetZLIBsupport_NNsh(void)
{
#ifdef USE_ZLIB
    if ((NNshGlobal->NNsiParam)->useGZIP != 0)
    {
        // ZLIBをオープンしてみる
        if(OpenZLib() == false)
        {
            // オープン失敗、エラー表示
            NNsh_ErrorMessage(ALTID_ERROR, MSG_TO_INSTALL_ZLIB, "", 0);
            NNshGlobal->useGzip = 0;
        }
        else
        {
            // GZIP圧縮を有効にする
            NNshGlobal->useGzip = NNSH_ENABLE;
            CloseZLib();
        }
    }
#endif
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

    // Xiino3がインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'Xii3', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino3がインストールされていた
        NNshGlobal->browserCreator    = 'Xii3';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

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

    // Blazerがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'BLZ5', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Blazerがインストールされていた
        NNshGlobal->browserCreator    = 'BLZ5';
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

    // Novarra Web Pro browserがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NOVR', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Novarra Web Pro browserがインストールされていた
        NNshGlobal->browserCreator    = 'NOVR';
        NNshGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    NNsh_DebugMessage(ALTID_INFO, MSG_CANNOTFIND_BROWSER, "", ret);
    return;
}

/*=========================================================================*/
/*   Function : KeyConvertFiveWayToJogChara                                */
/*                                  5Wayの入力コードをジョグのコードに変換 */
/*=========================================================================*/
UInt16 KeyConvertFiveWayToJogChara(EventType *event)
{
    UInt16 *command;

    // 5wayの入力でなければ、キーコードをそのまま応答する
    if (event->data.keyDown.chr != vchrNavChange)
    {
        // ハードキー制御の確認。。。
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // ハードキーを変更
            if (event->data.keyDown.chr >= vchrHardKeyMin)
            {
                // ハードキーコードの確認
                switch (event->data.keyDown.chr)
                {
                  case vchrHard1:
                  case vchrTapWaveSpecificKeyULeft:
                    // ハードキー１
                    command = &((NNshGlobal->NNsiParam)->useKey.key1);
                    break;

                  case vchrHard2:
                  case vchrTapWaveSpecificKeyURight:
                    // ハードキー２
                    command = &((NNshGlobal->NNsiParam)->useKey.key2);
                    break;

                  case vchrHard3:
                    // ハードキー３
                    command = &((NNshGlobal->NNsiParam)->useKey.key3);
                    break;

                  case vchrHard4:
                    // ハードキー４
                    command = &((NNshGlobal->NNsiParam)->useKey.key4);
                    break;

                  case vchrCapture:
                  case vchrVoiceRec:
                  case vchrVZ90functionMenu:      
                  case vchrGarminRecord:
                  case vchrHard5:
                    // CLIE NX キャプチャボタン、CLIE TG ボイスレコボタン
                    command = &((NNshGlobal->NNsiParam)->useKey.clieCapture);
                    event->data.keyDown.chr = vchrHard5;
                    break;

                  case chrLeftArrow:         // カーソルキー左(5way左)
                  case vchrRockerLeft:
                  case vchrJogLeft:
                  case vchrTapWaveSpecificKeyRLeft:
                    // ハードキー左
                    command = &((NNshGlobal->NNsiParam)->useKey.left);
                    break;

                  case chrRightArrow:     // カーソルキー右(5way右)
                  case vchrRockerRight:
                  case vchrJogRight:
                  case vchrTapWaveSpecificKeyRRight:
                    // ハードキー右
                    command = &((NNshGlobal->NNsiParam)->useKey.right);
                    break;

                  default:
                    return (event->data.keyDown.chr);
                    break;
                }

                // コマンドの判定
                switch (*command)
                {
                  case HARDKEY_FEATURE_CONTROLNNSI:
                    // NNsiで制御(ハードキーが押されたときの処理を実行)
                    return (event->data.keyDown.chr);
                    break;

                  case HARDKEY_FEATURE_TRANSJOGPUSH:
                    // キーコードをJOG PUSH(LineFeed)に書き換える
                    return (chrLineFeed);
                    break;

                  case HARDKEY_FEATURE_TRANSJOGBACK:
                    // キーコードをJOG BACK(ESC)に書き換える
                    return (chrEscape);
                    break;

                  case HARDKEY_FEATURE_DONOTHING:
                  default:
                    // 何もしない
                    break;
                }
            }

        }
        return (event->data.keyDown.chr);
    }
/**
//    if (((event->data.keyDown.keyCode) == 0)&&(event->data.keyDown.chr == vchrRockerCenter))
    if ((event->data.keyDown.keyCode) == 0)
    {
        // カーソルの真ん中キーを離した場合... (Treo 650/Tungsten T5/Palm TX対応)
        return (0);
    }
**/

    if (((event->data.keyDown.keyCode) & (navBitSelect | navChangeSelect))
         == navChangeSelect)
    {
        // ジョグ押下と等価にする
        return (vchrJogRelease);
    }
    if ((event->data.keyDown.keyCode) & navBitLeft)
    {
        // カーソルキー左と等価にする
        return (chrLeftArrow);
    }
    if ((event->data.keyDown.keyCode) & navBitRight)
    {
        // カーソルキー右と等価にする
        return (chrRightArrow);
    }

    // 上記以外はそのまま応答する
    return (event->data.keyDown.chr);
}

#ifdef USE_HIGHDENSITY
/*-------------------------------------------------------------------------*/
/*   Function :   setDoubleDensitySupport_NNsh                             */
/*                                              高解像度サポートのチェック */
/*-------------------------------------------------------------------------*/
static void setDoubleDensitySupport_NNsh(void)
{
    UInt32  version;

    // 高解像度サポート設定を初期化
    NNshGlobal->os5HighDensity = (UInt32) kDensityLow;

    // OS 3.5未満の場合
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        // カラー未サポートの端末(カラーサポートの設定を不可にする
        (NNshGlobal->NNsiParam)->useColor = 0;
    }
#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        // CLIEだったら、高解像度のチェックをしない(CLIEハイレゾを使う)
        return;
    }
#endif

#ifdef USE_TSPATCH
    if ((NNshGlobal->NNsiParam)->notUseTsPatch != 0)
    {
        // TsPatchを使わない時は、高解像度のチェックをしない(標準解像度にする)
        return;
    }
#endif

    // Yomeru5 を利用しているか？
    if (FtrGet('Ymru', 5000, &version) == 0)
    {
        // Yomeru5を利用中、、、高解像度モードは使用しないことにする
        NNsh_DebugMessage(ALTID_INFO, "Yomeru5 Version", ": ", version);
        return;
    }

    // 高解像度フィーチャーの取得
    if (FtrGet(sysFtrCreator, sysFtrNumWinVersion, &version) != 0)
    {
        // 高解像度フィーチャーのサポートなし
        return;
    }
    NNsh_DebugMessage(ALTID_INFO, "Window Version", ": ", version);
    if (version < 4)
    {
        //　高解像度のサポートがない、終了する
        //  (これで正しいんだろうか...マニュアルにはそう書いてあったが...)
        return;
    }

    // Windowの解像度を取得する
    WinScreenGetAttribute(winScreenDensity, &(NNshGlobal->os5HighDensity));

    // とりあえず、標準解像度にする
    WinSetCoordinateSystem(kCoordinatesStandard);

    return;
}
#endif   // #ifdef USE_HIGHDENSITY

/*=========================================================================*/
/*   Function : convertFormID                                              */
/*                                                フォームＩＤのコンバート */
/*=========================================================================*/
UInt16 ConvertFormID(UInt16 lastFrmID)
{
    UInt16 frmId;

    // lastFrmID から実際のフォームIDへコンバートする
    switch (lastFrmID)
    {
      case NNSH_FRMID_MESSAGE:
        // スレ参照画面
        frmId = FRMID_MESSAGE;
        break;

      case NNSH_FRMID_DUMMY:
        // ダミー画面
        frmId = FRMID_NNSI_DUMMY;
        break;

      case NNSH_FRMID_WRITE_MESSAGE:
        // 書き込み画面
        frmId = FRMID_WRITE_MESSAGE;
        break;

      case NNSH_FRMID_GETLOGLIST:
        // 参照ログ一覧画面
        frmId = FRMID_LISTGETLOG;
        break;
#if 0
      case NNSH_FRMID_BOARDINPUT:
      case NNSH_FRMID_MESSAGEINFO:
      case NNSH_FRMID_MANAGEBBS:
      case NNSH_FRMID_INPUTDIALOG:
      case NNSH_FRMID_BUSY:
      case NNSH_FRMID_NNSI_SETTING7:
      case NNSH_FRMID_NNSI_SETTING6:
      case NNSH_FRMID_NNSI_SETTING5:
      case NNSH_FRMID_NNSI_SETTING4:
      case NNSH_FRMID_NNSI_SETTING3:
      case NNSH_FRMID_NNSI_SETTING2:
      case NNSH_FRMID_CONFIG_NNSH:
#endif
      case NNSH_FRMID_THREAD:
      default:
        // スレ一覧画面
        frmId = FRMID_THREAD;
        break;
    }
    return (frmId);
}

#ifdef USE_PIN_DIA
/*=========================================================================*/
/*   Function : VgaFormModify_DIA                                          */
/*                                                     DIA用のフォーム調整 */
/*=========================================================================*/
Err VgaFormModify_DIA(FormType *frmP, VgaFormModifyType_DIA type)
{
    switch (type)
    {
      case vgaFormModifyNormalToWide:
        // DIAサポートの設定
        FrmSetDIAPolicyAttr(frmP, frmDIAPolicyCustom);

        // シルク制御を有効に
        PINSetInputTriggerState(pinInputTriggerEnabled);
//        PINSetInputAreaState(pinInputAreaUser);

        // えらいベタ、、、ウィンドウサイズの最大値最小値を設定
        WinSetConstraintsSize(FrmGetWindowHandle(frmP), 160, 225, 225, 160, 225, 225);

        // シルクの領域状態を前画面と同じにする...
        // PINSetInputAreaState(pinInputAreaUser);
        break;

      default:
        // 何もしない
        break;
    }
    return (errNone);
}
#endif

#ifdef USE_PIN_DIA
/*=========================================================================*/
/*   Function : VgaFormRotate_DIA                                          */
/*                                               DIA用のフォーム調整(回転) */
/*=========================================================================*/
void VgaFormRotate_DIA(void)
{
#ifdef USE_SYSEXIST_GLUE
    // 関数があるかどうかのチェックを入れる
    if ((SysGlueTrapExists(pinSysGetOrientation))&&
        (SysGlueTrapExists(pinSysSetOrientation)))
    {
#endif  // #ifdef USE_SYSEXIST_GLUE

        switch (SysGetOrientation())
        {
          case sysOrientationPortrait:
            SysSetOrientation(sysOrientationLandscape);
            break;

          case sysOrientationLandscape:
            SysSetOrientation(sysOrientationPortrait);
            break;

          case sysOrientationUser:
          default:
            // ありえないはずだが、一応
            SysSetOrientation(sysOrientationPortrait);
            break;
        }
#ifdef USE_SYSEXIST_GLUE
    }
#endif   // #ifdef USE_SYSEXIST_GLUE
    return;
}
#endif
