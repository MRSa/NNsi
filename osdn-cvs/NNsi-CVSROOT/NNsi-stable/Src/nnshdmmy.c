/*============================================================================*
 *  FILE: 
 *     nnshdmmy.c
 *
 *  Description: 
 *     NNsh dummy Display.
 *
 *===========================================================================*/
#define NNSHDMMY_C
#include "local.h"

extern UInt16 ConvertFormID(UInt16 lastFrmID);

/*=========================================================================*/
/*   Function :   OpenForm_NNsiDummy                                       */
/*                                             NNsi�N��(DUMMY)��ʃI�[�v�� */
/*=========================================================================*/
Err OpenForm_NNsiDummy(FormType *frm)
{
    Err          ret;
    Char         *osVer, buffer[BIGBUF];
    UInt8        remainPer;
    UInt32       useSize, totalSize, freeSize;

    // �o�̓o�b�t�@���N���A����
    MemSet (buffer, sizeof(buffer), 0x00);

    // ���݊J���Ă����ʂ��_�~�[��ʂƂ���(�O��J���Ă�����ʂ�work1�ɋL�^)
    if ((NNshGlobal->NNsiParam)->lastFrmID != NNSH_FRMID_DUMMY)
    {
        NNshGlobal->work1    = (NNshGlobal->NNsiParam)->lastFrmID;
    }
    else
    {
        NNshGlobal->work1    = NNSH_FRMID_THREAD;
    }
    (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_DUMMY;

    // OS�̃o�[�W�������o�͂���
    osVer = SysGetOSVersionString();
    StrCat (buffer, "PalmOS ");
    StrCat (buffer, osVer);
    MEMFREE_PTR(osVer);

    // ���݂̃o�b�e���c�ʂ��o�b�t�@�ɏo�͂���
    remainPer      = 0;
    SysBatteryInfo(false, NULL, NULL, NULL, NULL, NULL, &remainPer);
    StrCat (buffer, "\n---\nBattery :");
    NUMCATI(buffer, (UInt16) remainPer); 
    StrCat (buffer, "%");

    // �󂫃��������̎擾
    freeSize = GetOSFreeMem(&totalSize, &useSize);
    {
        // VFS�̗e��(��/�ő�)���o�͂���
        StrCat (buffer, "\nMemory(Free/Total)\n    ");
        NUMCATI(buffer, ((freeSize)/1024));
        StrCat (buffer, "kB/");
        NUMCATI(buffer, ((totalSize/1024))); 
        StrCat (buffer, "kB");
    }

    // VFS���̎擾
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFS�̗e�ʂ��m�F����
        ret = VFSVolumeSize(NNshGlobal->vfsVol, &useSize, &totalSize);
        if (ret == errNone)
        {
            // VFS�̗e��(��/�ő�)���o�͂���
            StrCat (buffer, "\nVFS(Free/Total)\n    ");
            NUMCATI(buffer, ((totalSize - useSize)/1024));
            StrCat (buffer, "kB/");
            NUMCATI(buffer, ((totalSize/1024))); 
            StrCat (buffer, "kB");
        }
    }
    
    // �f�o�C�X����\������
    NNshWinSetFieldText(frm, FLDID_DUMMYMSG, true, buffer, StrLen(buffer));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   Hander_NNsiDummy                                         */
/*                                  NNsi�N��(DUMMY)��ʂ̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_NNsiDummy(EventType *event)
{
    Err        ret;
    UInt16     itemId, len, savedParam;
    Char       kwBuf[BUFSIZE + MARGIN], *buffer;
    EventType *dummyEvent;

    buffer = NULL;
    // �{�^���`�F�b�N�ȊO�̃C�x���g�͑��߂�
    switch (event->eType)
    { 
      case menuEvent:
        // ���j���[�I��
        itemId = event->data.menu.itemID;
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ(���֐i��)
        itemId = event->data.ctlSelect.controlID;
        break;

      case keyDownEvent:
        // �L�[���͂�������(���ւ�����)
        itemId = KeyConvertFiveWayToJogChara(event);
        break;

      default: 
        return (false);
        break;
    }

    switch (itemId)
    {
      case MNUID_NNSI_WELCOME:
      case SELTRID_NNSI_DUMMY:
      case chrCapital_E:  // E�L�[�̓���
      case chrSmall_E:
        // �N���R�[�h����
        MemSet(kwBuf, sizeof(kwBuf), 0x00);
        // �N�����p�X���[�h�ݒ�
        if (DataInputDialog(NNSH_INPUTWIN_ENTPASS, kwBuf, PASSSIZE,
                            NNSH_DIALOG_USE_PASS, NULL) != false)
        {
            len = StrLen((NNshGlobal->NNsiParam)->launchPass);
            if (StrNCompare((NNshGlobal->NNsiParam)->launchPass, kwBuf, len) == 0)
            {
                // �N�����p�X���[�h�̈�v���m�F����
                NNsh_InformMessage(ALTID_INFO, MSG_MATCH_LAUNCHWORD, "", 0);

                // �O��I��������ʂ��J��
                FrmGotoForm(ConvertFormID(NNshGlobal->work1));
                goto FUNC_END;
            }
        }
        FrmDrawForm(FrmGetActiveForm());
        break;

      case MNUID_IPINFO:      
        // �C���^�t�F�[�X����\������
        buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize);
        if (buffer != NULL)
        {    
            // �l�b�g���C�u�������I�[�v������ĂȂ��Ƃ��̓I�[�v������B
            ret = NNshNet_open(&(NNshGlobal->netRef));
            if (ret == errNone)
            {
                // ���m�[�hIP�����擾����
                MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
                NNshNet_GetNetIFInfo(NNshGlobal->netRef, buffer);

                NNshNet_close(NNshGlobal->netRef);

                // ���ʂ�\������
                NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DUMMYMSG,
                                    true, buffer, StrLen(buffer));
            }
            MEMFREE_PTR(buffer);
        }
        FrmDrawForm(FrmGetActiveForm());
        break;

      case MNUID_RESOLVE:
        // �z�X�g�̊m�F�����{����
        buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize);
        MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
        if (buffer != NULL)
        {    
            // �`�F�b�N����z�X�g�������...
            MemSet(kwBuf, sizeof(kwBuf), 0x00);
            if (DataInputDialog(NNSH_INPUTWIN_RESOLVE, kwBuf, BUFSIZE,
                                NNSH_DIALOG_USE_RESOLVE, NULL) != false)
            {
                // BUSY�E�B���h�E�Ɋm�F���̃z�X�g����\������
                StrCopy(buffer, "Checking: ");
                StrCat (buffer, kwBuf);

                // �l�b�g���C�u�������I�[�v������ĂȂ��Ƃ��̓I�[�v������B
                ret = NNshNet_open(&(NNshGlobal->netRef));
                if (ret == errNone)
                {
                    // BUSY�E�B���h�E�̕\��
                    Show_BusyForm(buffer);

                    // �z�X�g���m�F����
                    MemSet(buffer, (NNshGlobal->NNsiParam)->bufferSize, 0x00);
                    NNshNet_CheckHostAlive(NNshGlobal->netRef, kwBuf, 3, buffer);            

                    Hide_BusyForm(false);

                    NNshNet_close(NNshGlobal->netRef);

                    // ���O�������ʁ{����\������
                    NNshWinSetFieldText(FrmGetActiveForm(), FLDID_DUMMYMSG,
                                        true, buffer, StrLen(buffer));
                }
            }
            MEMFREE_PTR(buffer);
        }
        FrmDrawForm(FrmGetActiveForm());
        break;

      case MNUID_NNSI_VERSION:
        // �o�[�W��������\��
        ShowVersion_NNsh();
        break;

      case MNUID_EDIT_UNDO:
      case MNUID_EDIT_CUT:
      case MNUID_EDIT_COPY:
      case MNUID_EDIT_PASTE:
      case MNUID_SELECT_ALL:
        // �ҏW���j���[���Ăяo��
        return (NNsh_MenuEvt_Edit(event));
        break;

      case MNUID_NETPREF:
        // �l�b�g���[�N�ݒ���J��
        OpenNetworkPreferences();
        break;

      case MNUID_DISCONNECT:
      case chrCapital_Q:  // Q�L�[�̓���
      case chrSmall_Q:
        // ����ؒf
        if (NNshGlobal->netRef != 0)
        {
            savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            NNshNet_LineHangup();
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
        }
        break;

      case BTNID_NNSI_DUMMY_OK:
      default:
        // ��L�ȊO(NNsi�I���C�x���g�𐶐�����)
        dummyEvent = &(NNshGlobal->dummyEvent);
        MemSet(dummyEvent, sizeof(EventType), 0x00);
        dummyEvent->eType = appStopEvent;
        EvtAddEventToQueue(dummyEvent);
        return (false);
        break; 
    }

FUNC_END:
    return (true);
}
