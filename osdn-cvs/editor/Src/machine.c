/*============================================================================*
 *  FILE: 
 *     machine.c
 *
 *  Description: 
 *     Palm device specific functions.
 *
 *===========================================================================*/
#define MACHINE_C
#include "local.h"

// prototypes
void CheckVFSdevice_NNsh(void);

/*-------------------------------------------------------------------------*/
/*   Function :   VFSUnmountNotify                                         */
/*                               VFSがUnmountされた時のNotify処理          */
/*-------------------------------------------------------------------------*/
Err VFSUnmountNotify(SysNotifyParamType *notifyParamsP)
{
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    NNsiParam->useVFS = 0;

    // VFSがunmountされたことを通知
    NNsiGlobal->notifyUpdate = NNSH_VFS_UNMOUNTED; 
     
    EvtWakeup();

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   VFSMountNotify                                           */
/*                               VFSがMountされた時のNotify処理            */
/*-------------------------------------------------------------------------*/
Err VFSMountNotify(SysNotifyParamType *notifyParamsP)
{
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // Remount処理
    CheckVFSdevice_NNsh();

    // start.prcが実行&AppSwitchされるのを回避
    notifyParamsP->handled |= vfsHandledStartPrc | vfsHandledUIAppSwitch;

    // VFSがmountされたことを通知
    NNsiGlobal->notifyUpdate = NNSH_VFS_MOUNTED; 

    // nilEvent発行
    EvtWakeup();

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   CheckVFSdevice_NNsh                                      */
/*                               VFSのサポート状況をパラメータに反映させる */
/*-------------------------------------------------------------------------*/
void CheckVFSdevice_NNsh(void)
{
    Err            err;
    Char           dirName[sizeof(DATAFILE_PREFIX)+ MARGIN];
    UInt32         vfsMgrVersion, volIterator;
    VolumeInfoType volInfo;  
    LocalID        appDbID;
    UInt16         cardNo;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

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
            err = VFSVolumeEnumerate(&(NNsiGlobal->vfsVol), &volIterator);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "VFSVolumeEnumerate()", "", err);
                break;
            }
            err = VFSVolumeInfo(NNsiGlobal->vfsVol, &volInfo);
            if (err != errNone)
            {
                NNsh_DebugMessage(ALTID_WARN, "VFSVolumeInfo()", "", err);
                break;
            }
            // "VFSはコンパクトフラッシュを使用する"設定の場合、、、
            switch (NNsiParam->vfsUseCompactFlash)
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
                if (volInfo.mediaType != 'Tffs')
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

            // ファイルシステムの確認
            if (volInfo.fsType == vfsFilesystemType_VFAT)
            {                    
                // VFS ONにする
                NNsiParam->useVFS = ((NNSH_VFS_ENABLE)|(NNSH_VFS_WORKAROUND));

                // NNsh用データ格納用ディレクトリを自動作成(VFS ON時)
                CreateDir_NNsh("/Palm/");
                CreateDir_NNsh("/Palm/Programs/");
                StrCopy(dirName, DATAFILE_PREFIX);
                dirName[StrLen(dirName) - 1] = '\0';
                err = CreateDir_NNsh(dirName);
                NNsh_DebugMessage(ALTID_INFO, "Created :", dirName, err);
                err = errNone;
                break;
            }
        }
    }
    if (err != errNone)
    {
        // デバイスにVFSサポートがない場合は、NNsh設定のVFS使用設定をクリア
        NNsiParam->useVFS = NNSH_NOTSUPPORT_VFS;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   ResetVFSNotifications                                    */
/*                                          VFS Notification の unregister */
/*-------------------------------------------------------------------------*/
void ResetVFSNotifications(void)
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

    return;
}

#ifdef USE_PIN_DIA
#ifdef USE_PIN_GARMIN
/*-------------------------------------------------------------------------*/
/*   Function :  DisplayResizeNotify                                       */
/*                                                        ディスプレイ更新 */
/*-------------------------------------------------------------------------*/
Err DisplayResizeNotify(SysNotifyParamType *notifyP)
{
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // 画面描画の更新は、mainで行うので、ここでは更新フラグをたてるのみ
    NNsiGlobal->notifyUpdate = NNSH_DISP_RESIZED;
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
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // ---------------------------------------------------
    //    注意：notifyP は、NULLでコールされる
    // ---------------------------------------------------
    if (NNsiGlobal->silkRef != 0)
    {
        frm = FrmGetActiveForm();
        HandEraResizeForm(frm);

        //   画面表示の更新は、標準のイベントループで行う。そのために、
        // 描画指示フラグを変更し、nilEventを発行する。
        NNsiGlobal->updateHR = NNSH_UPDATE_DISPLAY;
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
void SetCLIESupport_NNsh(void)
{
    SonySysFtrSysInfoP     infoP;
    Boolean                isColor;
    UInt32                 vskVer, width, height, depth;
    UInt16                 vskState;
    Err                    ret;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    infoP                        = NULL;
    NNsiGlobal->jogAssistSetBack = NULL;

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
                       (UInt32 *) &(NNsiGlobal->jogAssistSetBack)))
            {
                // ジョグダイヤルの制御設定
                NNsiGlobal->sonyJogMask[0] = sonyJogAstMaskType1; // フォーム毎に個別にマスクを設定する
                NNsiGlobal->sonyJogMask[1] = NUMBER_OF_JOGMASKS;  // マスクするフォーム数
                NNsiGlobal->sonyJogMask[2] = FRMID_MAIN;
                NNsiGlobal->sonyJogMask[3] = sonyJogAstMaskAll;
                NNsiGlobal->sonyJogMask[4] = FRMID_FILE_SAVELOAD;
                NNsiGlobal->sonyJogMask[5] = sonyJogAstMaskAll;
                NNsiGlobal->sonyJogMask[6] = 0;
                NNsiGlobal->sonyJogMask[7] = 0;    // 怖いから余裕...

                // 現在のジョグアシスト情報を保管、NNsi用の設定
                NNsiGlobal->jogInfoBack = *(NNsiGlobal->jogAssistSetBack);
                *(NNsiGlobal->jogAssistSetBack) =  NNsiGlobal->sonyJogMask;

                // ジョグアシストオーナ系の設定は省略(いらんだろ)
            }
        }
    }

    // SILKマネージャの参照番号を取得する
    ret = SysLibFind(sonySysLibNameSilk, &(NNsiGlobal->silkRef));
    if (ret == sysErrLibNotFound)
    {
        // SILKマネージャが未ロードの場合、ロードできるかTryしてみる。
        ret = SysLibLoad('libr',sonySysFileCSilkLib,&(NNsiGlobal->silkRef));
    }
    if (ret != errNone)
    {
        // 281さんのSilkマネージャ判定コードをマージする
        ret = SysLibLoad(sonySysFileTSilkLib,
                         sonySysFileCSilkLib,
                         &(NNsiGlobal->silkRef));    // OS5
        if (ret != errNone)
        {
            ret = SysLibLoad('libr','SlSi',&(NNsiGlobal->silkRef));
        }
        if (ret != errNone)
        {
            // SILKマネージャは使用できない、無効にしてSONY HRLIBロードへ
            NNsiGlobal->silkRef = 0;
            goto SONY_HRLIB_LOAD;
        }
    }

    // SILKを使用しない場合には次に進む
    if (NNsiGlobal->notUseSilk != 0)
    {
        NNsiGlobal->silkRef = 0;
        NNsiGlobal->silkVer = 1;
        goto SONY_HRLIB_LOAD;
    }

    // SILKマネージャのバージョン確認
    ret = FtrGet(sonySysFtrCreator, sonySysFtrNumVskVersion, &vskVer);
    if (ret != errNone)
    {
        // SILKマネージャはVERSION1、リサイズのみサポート(NR用)
        if (SilkLibOpen(NNsiGlobal->silkRef) == errNone)
        {
            // SILKのリサイズをサポートしていることを(SILKマネージャに)通知
            SilkLibEnableResize(NNsiGlobal->silkRef);
            NNsiGlobal->silkVer = 1;
            goto SONY_HRLIB_LOAD;
        }

        // SILKバージョンの取得に失敗した時には、SILK制御しない。
        NNsiGlobal->silkRef = 0;
        NNsiGlobal->silkVer = 1;
        goto SONY_HRLIB_LOAD;
    }
    else
    {
        // マジですか、フィーチャー番号でVer1がエラーにならないってのは,,,
        switch (vskVer)
        {
          case vskVersionNum1:
            // SILKマネージャはVERSION1、リサイズのみサポート(NR用)
            if (SilkLibOpen(NNsiGlobal->silkRef) == errNone)
            {
                // SILKのリサイズをサポートしていることを(SILKマネージャに)通知
                SilkLibEnableResize(NNsiGlobal->silkRef);
                NNsiGlobal->silkVer = 1;
            }
            break;

          case vskVersionNum2:
            // SILKマネージャはVERSION2、(NX以降)
            if (VskOpen(NNsiGlobal->silkRef) == errNone)
            {
                // SILKのリサイズ(縦方向)をサポートしていることを(SILKマネージャに)通知
                VskSetState(NNsiGlobal->silkRef, vskStateEnable, vskResizeVertically);
                NNsiGlobal->silkVer = 2;
            }
            break;

          case vskVersionNum3:
          default:
            // SILKマネージャはVERSION3、(UX以降)、その他のデバイス
            if (VskOpen(NNsiGlobal->silkRef) == errNone)
            {
                // SILKのリサイズ(横方向)をサポートしていることを(SILKマネージャに)通知
                ret = VskGetState(NNsiGlobal->silkRef, vskStateResizeDirection, &vskState);
                if (ret != errNone)
                {
                    vskState = vskResizeDisable;
                }
                VskSetState(NNsiGlobal->silkRef, vskStateEnable, vskState);
                NNsiGlobal->silkVer = 3;
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
    if (NNsiParam->disableSonyHR != 0)
    {
        // HRLIBは使用しない
        NNsiGlobal->hrRef    = 0;
        NNsiGlobal->notUseHR = NNsiParam->disableSonyHR;
        return;
    }

    // HRLIBの参照番号を取得する
    ret = SysLibFind(sonySysLibNameHR, &(NNsiGlobal->hrRef));
    if (ret == sysErrLibNotFound)
    {
        // HRLIBが未ロードの場合、ロードできるかTryしてみる。
        ret = SysLibLoad('libr', sonySysFileCHRLib, &(NNsiGlobal->hrRef));
    }
    if (ret != errNone)
    {
        // HRLIBは使用できない
        NNsiGlobal->hrRef = 0;
        return;
    }
    // HRLIBの使用宣言とHRLIBのバージョン番号取得
    HROpen(NNsiGlobal->hrRef);
    HRGetAPIVersion(NNsiGlobal->hrRef, &(NNsiGlobal->hrVer));
    if (NNsiGlobal->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
    {
        // 旧バージョンのハイレゾ...画面モードを確認する...
        ret = HRWinScreenMode(NNsiGlobal->hrRef, winScreenModeGet, 
                              &width, &height, &depth, &isColor);
        if (ret != errNone)
        {
            // HRLIBは使用しない
            HRClose(NNsiGlobal->hrRef);
            NNsiGlobal->hrRef = 0;
            return;
        }

        NNsiGlobal->prevHRWidth = width;
        if (width != hrWidth)
        {
            // 画面モードがハイレゾでないので、ハイレゾモードに切り替える
            width  = hrWidth;
            height = hrHeight;
            ret    = HRWinScreenMode(NNsiGlobal->hrRef, winScreenModeSet,
                                     &width, &height, &depth, &isColor);
            if (ret != errNone)
            {
                // HRLIBは使用しない
                HRClose(NNsiGlobal->hrRef);
                NNsiGlobal->hrRef = 0;
                return;
            }
        }
    }
    else
    {
        // 画面モードは変更しない。
        NNsiGlobal->prevHRWidth = hrWidth;
    }
    return;
}
#endif

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :   resetCLIESupport_NNsh                                    */
/*                                                  CLIEサポートを解放する */
/*-------------------------------------------------------------------------*/
void ResetCLIESupport_NNsh(void)
{
    Boolean                isColor;
    UInt32                 width, height, depth;
    NNshWorkingInfo       *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // JOGアシスト機能を元に戻す
    if (NNsiGlobal->jogAssistSetBack != NULL)
    {
        *(NNsiGlobal->jogAssistSetBack) = NNsiGlobal->jogInfoBack;
    }

    if (NNsiGlobal->silkRef == 0)
    {
        // SONY用シルク制御の機能を使用しない場合にはHRLIBの解放へ
        goto SONY_HRLIB_UNLOAD;
    }

    // sysNotifyDisplayChangeEventをunsubscribeする
    (void) SysNotifyUnregister(0, SOFT_CREATOR_ID, sysNotifyDisplayChangeEvent,
                               sysNotifyNormalPriority);

    // SILKマネージャの解放
    if (NNsiGlobal->silkVer == 1)
    {
        // 旧SILKマネージャ(NR用)
        SilkLibDisableResize(NNsiGlobal->silkRef);
        SilkLibClose(NNsiGlobal->silkRef);
    }
    else
    {
        // 新SILKマネージャ(NX以降)
        VskSetState(NNsiGlobal->silkRef, vskStateEnable, 0);
        VskClose(NNsiGlobal->silkRef);
    }

SONY_HRLIB_UNLOAD:
    if (NNsiGlobal->hrRef != 0)
    {
        // アプリ起動時に、画面モードが違ったら元に戻す
        if (NNsiGlobal->prevHRWidth != hrWidth)
        {
            HRWinScreenMode(NNsiGlobal->hrRef, winScreenModeGet,
                            &width, &height, &depth, &isColor);
            width      = stdWidth;
            height     = stdHeight;
            (void) HRWinScreenMode(NNsiGlobal->hrRef, winScreenModeSet,
                                   &width, &height, &depth, &isColor);
        }
        // Sony HRLIBの使用終了
        HRClose(NNsiGlobal->hrRef);
    }
    return;
}
#endif

#ifdef USE_HANDERA
/*-------------------------------------------------------------------------*/
/*   Function :   setHandEraSupport_NNsh                                   */
/*                                               HandEraデバイスのチェック */
/*-------------------------------------------------------------------------*/
void SetHandEraSupport_NNsh(void)
{
    UInt32              handEraOsVersion;
    VgaScreenModeType   handEraScreenMode;     // スクリーンモード
    VgaRotateModeType   handEraRotateMode;     // 回転モード
    NNshWorkingInfo    *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // HandEra QVGA サポート有無を確認する。
    handEraOsVersion = 0;
    if (_TRGVGAFeaturePresent(&handEraOsVersion))          
    {
        NNsiGlobal->device = NNSH_DEVICE_HANDERA;
    }

    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
        VgaSetScreenMode(screenMode1To1, handEraRotateMode);
    }
}
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
/*-------------------------------------------------------------------------*/
/*   Function :   SetDIASupport_NNsh                                       */
/*                                    Dynamic Input Areaデバイスのチェック */
/*-------------------------------------------------------------------------*/
void SetDIASupport_NNsh(void)
{
    Err    err;
    UInt32 ver, iFlag;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

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
            NNsiGlobal->device = NNSH_DEVICE_GARMIN;
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
         NNsiGlobal->device = NNSH_DEVICE_DIASUPPORT;
         NNsh_DebugMessage(ALTID_INFO, "enable DIA", "", 0);
    }
    return;
}
#endif //  #ifdef USE_PIN_DIA

#ifdef USE_PIN_DIA
/*-------------------------------------------------------------------------*/
/*   Function :   resetDIASupport_NNsh                                     */
/*                                            Dynamic Input Areaの使用開放 */
/*-------------------------------------------------------------------------*/
void ResetDIASupport_NNsh(void)
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
void HandEraMoveObject(FormPtr frmP, UInt16 objIndex,
                              Coord x_diff, Coord y_diff)
{
    Boolean move = true;
    Coord   old_x, old_y;

#ifdef USE_GLUE
    if (FrmGlueGetObjectUsable(frmP, objIndex) != true)
    {
        // 隠れているオブジェクトは移動しない
            move = false;
    }
    else
#endif
    {
        FrmHideObject(frmP, objIndex);
    }
    FrmGetObjectPosition(frmP, objIndex, &old_x, &old_y);
    if (old_y < 110)
    {
        // 上の方にあるオブジェクトは移動させない
        // (すげーベタだな...ちょっとカッコわる...)
        // x_diff = 0;
        y_diff = 0;
    }
    FrmSetObjectPosition(frmP, objIndex, old_x + x_diff, old_y + y_diff);

    // 移動したオブジェクトを表示する
    if (move == true)
    {
        FrmShowObject(frmP, objIndex);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   HandEraResizeObject                                      */
/*                                                 オブジェクトのリサイズ  */
/*-------------------------------------------------------------------------*/
void HandEraResizeObject(FormPtr frmP, UInt16 objIndex, Coord x_diff, Coord y_diff)
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
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

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
    if (NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)
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
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
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
    if (NNsiGlobal->silkRef != 0)
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
void RestoreSetting_NNsh(Int16 version, UInt16 size, NNshSavedPref *prm)
{
    Err           ret;
    UInt32        offset;
    UInt16        cnt, nofRec;
    DmOpenRef     dbRef;
    Char         *ptr;

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
                NNsh_WarningMessage(ALTID_WARN, "Preference read failure", "", ret);
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
        prm->bufferSize       = NNSH_WORKBUF_DEFAULT;
        prm->currentFont      = NNSH_DEFAULT_FONT;
        prm->autoRestore      = 1;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   NNshSaveNNsiSetting                                      */
/*                                                         NNsi設定の記憶  */
/*-------------------------------------------------------------------------*/
void SaveSetting_NNsh(Int16 version, UInt16 size, NNshSavedPref *param)
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
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
#ifdef USE_PIN_DIA
    if ((NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNsiGlobal->device == NNSH_DEVICE_GARMIN))
    {
        if (PINSetInputAreaState(pinInputAreaClosed) != errNone)
        {
            HandEraResizeForm(frm);
        }
        return (errNone);
    }
#endif
#ifdef USE_CLIE
    // SILKマネージャがＯＰＥＮされているか？
    if (NNsiGlobal->silkRef != 0)
    {
        // SILKの最小化を行う
        if (NNsiGlobal->silkVer == 1)
        {
            // Version1(NR用)
            SilkLibResizeDispWin(NNsiGlobal->silkRef, silkResizeToStatus);
        }
        else
        {
            // Version2(NX以降)
            VskSetState(NNsiGlobal->silkRef, vskStateResize, vskResizeMin);
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
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

#ifdef USE_HANDERA
    if (NNsiGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMaximizeWindow();
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
#ifdef USE_PIN_DIA
    if ((NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNsiGlobal->device == NNSH_DEVICE_GARMIN))
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
    if (NNsiGlobal->silkRef != 0)
    {
        // SILKの最大化を行う
        if (NNsiGlobal->silkVer == 1)
        {
            // Version1(NR用)
            SilkLibResizeDispWin(NNsiGlobal->silkRef, silkResizeNormal);
        }
        else
        {
            // Version2(NX以降)
            VskSetState(NNsiGlobal->silkRef, vskStateResize, vskResizeMax);
        }
        HandEraResizeForm(frm);
        return (errNone);
    }
#endif
    return (~errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkInstalledWebBrowser                                 */
/*                                        WebBrowserのインストールチェック */
/*-------------------------------------------------------------------------*/
void CheckInstalledWebBrowser(void)
{
    UInt16            cardNo;
    LocalID           dbID;
    DmSearchStateType searchState;
    Err               ret;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // Xiino3がインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'Xii3', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino3がインストールされていた
        NNsiGlobal->browserCreator    = 'Xii3';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }


    // NetFrontがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NF3T', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // NetFrontがインストールされていた
        NNsiGlobal->browserCreator    = 'NF3T';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // WebBrowser 2.0がインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'NF3P', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // WebBrowser 2.0がインストールされていた
        NNsiGlobal->browserCreator    = 'NF3P';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Xiino(PalmScape)がインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'PScp', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiinoがインストールされていた
        NNsiGlobal->browserCreator    = 'PScp';
        NNsiGlobal->browserLaunchCode = 0xfa21;
        return;
    }

    // Xiino(LEM)がインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'dXin', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Xiino(LEM)がインストールされていた
        NNsiGlobal->browserCreator    = 'dXin';
        NNsiGlobal->browserLaunchCode = 0xfa21;
        return;
    }

    // Blazerがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'BLZ1', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Blazerがインストールされていた
        NNsiGlobal->browserCreator    = 'BLZ1';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        return;
    }

    // Eudora Webがインストールされているか確認する
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, 'QCwb', 
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // Eudora Webがインストールされていた
        NNsiGlobal->browserCreator    = 'QCwb';
        NNsiGlobal->browserLaunchCode = 0xfa0;
        // NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
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
        NNsiGlobal->browserCreator    = 'PLNK';
        // NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdOpenDB;
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
        NNsiGlobal->browserCreator    = 'NOVR';
        NNsiGlobal->browserLaunchCode = sysAppLaunchCmdGoToURL;
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
    NNshSavedPref *NNsiParam;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // 5wayの入力でなければ、キーコードをそのまま応答する
    if (event->data.keyDown.chr != vchrNavChange)
    {
        // ハードキー制御の確認。。。
        if ((NNsiParam->useHardKeyControl != 0)&&
            (event->data.keyDown.chr >= vchrHardKeyMin))
        {
            // ハードキーコードの確認
            switch (event->data.keyDown.chr)
            {
              case vchrHard1:
                // ハードキー１
                event->data.keyDown.chr = NNsiParam->useKey.key1;
                break;

              case vchrHard2:
                // ハードキー２
                event->data.keyDown.chr = NNsiParam->useKey.key2;
                break;

              case vchrHard3:
                // ハードキー３
                event->data.keyDown.chr = NNsiParam->useKey.key3;
                break;

              case vchrHard4:
                // ハードキー４
                event->data.keyDown.chr = NNsiParam->useKey.key4;
                break;

              case vchrCapture:
              case vchrVoiceRec:
              case vchrGarminRecord:
              case chrLeftArrow:         // カーソルキー左(5way左)
              case vchrJogLeft:
              case chrRightArrow:     // カーソルキー右(5way右)
              case vchrJogRight:
              default:
                return (event->data.keyDown.chr);
                break;
            }
        }
        return (event->data.keyDown.chr);
    }

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
/*   Function :   SetDoubleDensitySupport_NNsh                             */
/*                                              高解像度サポートのチェック */
/*-------------------------------------------------------------------------*/
void SetDoubleDensitySupport_NNsh(void)
{
    UInt32  version;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // 高解像度サポート設定を初期化
    NNsiGlobal->os5HighDensity = (UInt32) kDensityLow;

#ifdef USE_CLIE
    if (NNsiGlobal->hrRef != 0)
    {
        // CLIEだったら、高解像度のチェックをしない(CLIEハイレゾを使う)
        return;
    }
#endif

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
    WinScreenGetAttribute(winScreenDensity, &(NNsiGlobal->os5HighDensity));

    // とりあえず、標準解像度にする
    WinSetCoordinateSystem(kCoordinatesStandard);

    return;
}
#endif   // #ifdef USE_HIGHDENSITY
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