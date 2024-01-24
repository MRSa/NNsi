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
                  UInt32 range, UInt32 endRange)
{
    Err ret;

    if (NNshParam->useProxy != 0)
    {
        // Proxy�g�p
        ret = NNshHttp_commMain(type, url, cookie, appendData,
                                range, endRange,
                                NNshParam->proxyPort, NNshParam->proxyURL);
    }
    else
    {
        // Proxy���g�p
        ret = NNshHttp_commMain(type, url, cookie, appendData,
                                range, endRange, 80, NULL);
    }
    return (ret);
}

/*==========================================================================*/
/*  NNshNet_LineHangup() : ����ؒf                                         */
/*                                                                          */
/*==========================================================================*/
void NNshNet_LineHangup(void)
{
    UInt16 netRef;

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
        Hide_BusyForm();
    }
    return;
}

/*==========================================================================*/
/*  WebBrowserCommand() : WebBrowser�Ăяo���֐�                            */
/*                                              �� �����Ă���邩�ȁH ��    */
/*==========================================================================*/
Err WebBrowserCommand(UInt32 creator, Boolean subLaunch, UInt16 flags,
                      UInt16 cmd, Char *parameterP, UInt32 *resultP)
{
    Char             *newParamP;
    UInt32            result;
    UInt16            cardNo, size;
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
    if (subLaunch != true)
    {
        // (NNsi��)��U�I������WebBrowser���N������ꍇ
        if (parameterP != NULL)
        {
            // �����i�t�q�k�j�̈���m�ۂ���
            size = StrLen(parameterP);
            newParamP = MemPtrNew(size + MARGIN);
            if (newParamP == NULL)
            {
                // �\���ȃ��������m�ۂł��Ȃ������A�G���[��������
                return (memErrNotEnoughSpace);
            }
            MemSet  (newParamP, (size + MARGIN), 0x00);
            StrNCopy(newParamP, parameterP, size);

            // �A�v�����I�����Ă��̈�͎c��悤�ɂ���
            MemPtrSetOwner(newParamP, 0);

            // �A�v���P�[�V�������N������
            ret = SysUIAppSwitch(cardNo, dbID, cmd, newParamP);
        }
    }
    else
    { 
        // �A�v���P�[�V������fork����(SubLaunch�̏ꍇ)
        ret = SysAppLaunch(cardNo, dbID, flags, cmd, parameterP, &result);
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
    return (ret);
}
