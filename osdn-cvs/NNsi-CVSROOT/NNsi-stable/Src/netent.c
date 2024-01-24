/*============================================================================*
 *  FILE: 
 *     netent.c
 *
 *  Description: 
 *     �l�b�g���[�N�ʐM�ق��̃G���g���|�C���g
 *
 *===========================================================================*/
#define NETENT_C
#include "local.h"

/*==========================================================================*/
/*  NNshHttp_comm() : HTTP�ʐM                                              */
/*                                                                          */
/*==========================================================================*/
Err NNshHttp_comm(UInt16 type, Char *url, Char *cookie, Char *appendData,
                  UInt32 range, UInt32 endRange, Char *message)
{
    UInt16  lp, savedParam, port, sslMode;
    Char   *proxy;
    Err     ret;
    Boolean retry;

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "GET SITE:%s", url);
#endif // #ifdef USE_REPORTER

    retry   = false;
    sslMode = NNSH_SSLMODE_NOTSSL;
    proxy   = NULL;
    port    = 80;
    if ((NNshGlobal->NNsiParam)->useProxy != 0)
    {
        port  = (NNshGlobal->NNsiParam)->proxyPort;
        proxy = (NNshGlobal->NNsiParam)->proxyURL;
    }
#ifdef USE_SSL
    else
    {
        // https�v���g�R���̏ꍇ�A�A�A
        if ((url[0] == 'h')&&(url[1] == 't')&&(url[2] == 't')&&(url[3] == 'p')&&
            (url[4] == 's')&&(url[5] == ':'))
        {
            sslMode = NNSH_SSLMODE_SSLV3;
            port    = 443;
            proxy   = NULL;
        }
    }
#endif

    // ���g���C�J�E���^���[���N���A
    lp = 0;
    do
    {
        // http�ʐM�����s
        ret = NNshHttp_commMain(type, url, cookie, appendData,
                                range, endRange, port, proxy, message, sslMode);
        lp++;

        // ���܂��ڑ��ł��Ȃ������ꍇ�́A��x�����؂�
        if (ret == NNSH_ERRORCODE_FAILURECONNECT)
        {
            if (lp > (NNshGlobal->NNsiParam)->nofRetry)
            {
                if (retry != true)
                {
                    //  ���g���C�񐔂𒴂��Ă��ڑ��ł��Ȃ������ꍇ�ɂ́A
                    // ��x�����؂��čĐڑ�����
                    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
                    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
                    NNshNet_LineHangup();
                    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
                    ret = netErrTimeout;
                    lp  = 0;
                    retry = true;
                }
            }
            else
            {
                // ���g���C�񐔓��B�܂ł́A�Đڑ������{����
                ret = netErrTimeout;
            }
        }
    } while ((lp <= (NNshGlobal->NNsiParam)->nofRetry)&&(ret == netErrTimeout));
    return (ret);
}

/*==========================================================================*/
/*  NNshNet_LineHangup() : ����ؒf                                         */
/*                                                                          */
/*==========================================================================*/
void NNshNet_LineHangup(void)
{
    UInt16         netRef;

    if (NNshGlobal->netRef == 0)
    {
        // �l�b�g���C�u�����̃I�[�v��
        (void) SysLibFind("Net.lib", &(netRef));
    }
    else
    {
        netRef = NNshGlobal->netRef;
    }

    // ����ؒf�̊m�F
    if ((netRef != 0)&&
        (NNsh_ConfirmMessage(ALTID_CONFIRM,
                             MSG_MESSAGE_DISCONNECT, "", 0) == 0))
    {
        Show_BusyForm(MSG_DISCONNECT_WAIT);
        (void) NetLibClose(netRef, true);           // ����N���[�Y
        (void) NetLibFinishCloseWait(netRef);       // �ڑ��҂������ؒf
        NNshGlobal->netRef = 0;

        Hide_BusyForm(true);
    }
    return;
}

/*==========================================================================*/
/*  WebBrowserCommand() : WebBrowser�Ăяo���֐�                            */
/*                                              �� �����Ă���邩�ȁH ��    */
/*==========================================================================*/
Err WebBrowserCommand(UInt32 creator, UInt16 subLaunch, UInt16 flags,
                      UInt16 cmd, Char *parameterP, UInt16 size, UInt32 *resultP)
{
    Char             *newParamP;
    UInt32            result;
    UInt16            cardNo;
    LocalID           dbID;
    DmSearchStateType searchState;
    Err               ret;

    if (resultP != NULL)
    {
        *resultP = errNone;
    }
    // WebBrowser���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &searchState,
                                         sysFileTApplication, creator, 
                                         true, &cardNo, &dbID);
    if (ret != errNone)
    {
        // WebBrowser(NetFront)���C���X�g�[������Ă��Ȃ�����
        NNsh_InformMessage(ALTID_ERROR, MSG_CANNOTFIND_BROWSER, "", ret);
        return (ret);
    }
    
    // child��fork���邩�A�I��������fork���邩subLaunch�Ŕ���
    // �I��������fork���Ȃ��Ɠ����Ȃ��̂�
    if (subLaunch == 0)
    {
        // �������Ȃ��ꍇ...
        if (size == 0)
        {
            // �A�v���P�[�V�������N������
            ret = SysUIAppSwitch(cardNo, dbID, cmd, NULL);
            return (ret);
        }

        // (NNsi��)��U�I������WebBrowser���N������ꍇ
        if (parameterP != NULL)
        {

            // �����i�t�q�k�j�̈���m�ۂ���
            newParamP = MEMALLOC_PTR(size + MARGIN);
            if (newParamP == NULL)
            {
                // �\���ȃ��������m�ۂł��Ȃ������A�G���[��������
                return (memErrNotEnoughSpace);
            }
            MemSet  (newParamP, (size + MARGIN), 0x00);
            MemMove (newParamP, parameterP, size);

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "PARAMETER:%s", newParamP);
#endif // #ifdef USE_REPORTER

            // �A�v�����I�����Ă��̈�͎c��悤�ɂ���
            MemPtrSetOwner(newParamP, 0);

            // �A�v���P�[�V�������N������
            ret = SysUIAppSwitch(cardNo, dbID, cmd, newParamP);
        }
        else
        {
            // �A�v���P�[�V�������N������
            ret = SysUIAppSwitch(cardNo, dbID, cmd, NULL);
            return (ret);
        }
    }
    else
    { 
        // �Ȃ񂩂��܂������Ȃ�...
        if (parameterP != NULL)
        {
            // �����i�t�q�k�j�̈���m�ۂ���
            newParamP = MEMALLOC_PTR(size + MARGIN);
            if (newParamP == NULL)
            {
                // �\���ȃ��������m�ۂł��Ȃ������A�G���[��������
                return (memErrNotEnoughSpace);
            }
            MemSet  (newParamP, (size + MARGIN), 0x00);
            MemMove (newParamP, parameterP, size);

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "PARAMETER:%s", newParamP);
#endif // #ifdef USE_REPORTER

            // �A�v���P�[�V������fork����(SubLaunch�̏ꍇ)
            ret = SysAppLaunch(cardNo, dbID, flags, cmd, newParamP, &result);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_ERROR, "SysAppLaunch()", "", ret);
                // �����͌p������ //
            }
            if (resultP != NULL)
            {
                *resultP = result;
            }
            MEMFREE_PTR(newParamP);
        }
        else
        {
            // �A�v���P�[�V������fork����(SubLaunch�̏ꍇ)
            ret = SysAppLaunch(cardNo, dbID, flags, cmd, NULL, &result);
            if (ret != errNone)
            {
                NNsh_DebugMessage(ALTID_ERROR, "SysAppLaunch()", "", ret);
                // �����͌p������ //
            }
            if (resultP != NULL)
            {
                *resultP = result;
            }
        }
    }
    return (ret);
}

/*==========================================================================*/
/*  OpenNetworkPreferences() : �l�b�g���[�N�ݒ��ʌĂяo���֐�             */
/*                                                 �� �����Ă���邩�ȁH �� */
/*==========================================================================*/
void OpenNetworkPreferences(void)
{
    Err                  ret;
    UInt16               cardNo;
    LocalID              dbID;
    DmSearchStateType    state;

    // �ϐ��̏�����
    cardNo = 0;
    MemSet(&state, sizeof(state), 0x00);

    // �l�b�g���[�N�ݒ��ʂ̃��\�[�X���Ƃ��Ă���B
    ret = DmGetNextDatabaseByTypeCreator(true, &state, sysFileTPanel,
                                         sysFileCNetworkPanel,
                                         true, &cardNo, &dbID);
    if (ret == errNone)
    {
        // ���\�[�X���������Ƃ��ɂ́A�ݒ��ʂ��J��
        SysUIAppSwitch(cardNo, dbID, sysAppLaunchCmdPanelCalledFromApp, NULL);
    }
    return;
}

#ifdef USE_BT_CONTROL
/*==========================================================================*/
/*  OpenBluetoothPreferences() : Blutooth�ݒ��ʌĂяo���֐�               */
/*                                                 �� �����Ă���邩�ȁH �� */
/*==========================================================================*/
void OpenBluetoothPreferences(Boolean val)
#ifdef USE_BTTOGGLE_SDK
{
    UInt16            cardNo, code; 
    LocalID           dbID; 
    DmSearchStateType state;
    Err               err;
    UInt32            result;

    // BtToggle���C���X�g�[������Ă��邩���ׂ�
    err = DmGetNextDatabaseByTypeCreator(true, &state, 0, BtToggle_CRID, 
                                                         true, &cardNo, &dbID);
    if (err == errNone)
    {
        // BtToggle���C���X�g�[������Ă����A�A�A
        if (val == true)
        {
            code = BtToggleLaunchCmdBtOn;
        }
        else
        {
            code = BtToggleLaunchCmdBtOff;
        }
        SysAppLaunch(cardNo, dbID, 0, code, NULL, &result);
   }
   return;
}
#else
{
    // �������Ȃ�
    return;
}
#endif  // #ifdef USE_BTTOGGLE_SDK
#endif  // #ifdef USE_BT_CONTROL

