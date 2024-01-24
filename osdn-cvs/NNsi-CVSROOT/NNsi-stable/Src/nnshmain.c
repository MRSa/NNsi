/*============================================================================*
 *  FILE: 
 *     nnshmain.c
 *
 *  Description: 
 *     Main routines for NNsh. 
 *
 *===========================================================================*/
#define THRMNG_C
#include "local.h"

static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR);
static Err get_message_Info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR, 
                            NNshBoardDatabase   *bbsInfo,  UInt16 *selBBS);
static Boolean set_SearchTitleString     (MemHandle *bufH, UInt16 status, UInt16 *mode);
static Boolean search_NextTitle          (Int16 direction);
static Boolean search_NextBBS            (UInt16 start, UInt16 *bbsId);
static Boolean display_message           (void);
static UInt16  get_all_message           (void);
static Boolean update_message            (void);
static Boolean update_getall_message     (void);
//static Boolean delete_notreceived_message(Boolean update);
//static Boolean delete_message            (void);
static Boolean get_subject_txt           (void);
static Boolean popEvt_ThreadMain         (EventType *event);
static Boolean keyDownEvt_ThreadMain     (EventType *event);
static Boolean ctlSelEvt_ThreadMain      (EventType *event);
static Boolean ctlRepEvt_ThreadMain      (EventType *event);
static Boolean menuEvt_ThreadMain        (EventType *event);

static Boolean rctPointEvt_ThreadMain(EventType *event,
                                      FormType *frm, RectangleType *rect);

extern void    Initialize_NNshSetting(FormType *frm);
extern void    Effect_NNshSetting    (FormType *frm);
extern Boolean Handler_NNshSetting   (EventType *event);
extern Boolean Handler_JumpSelection(EventType *event);

/*-------------------------------------------------------------------------*/
/*   Function : change_handera_rotate                                      */
/*                                                         HandEra��ʉ�] */
/*-------------------------------------------------------------------------*/
static void change_handera_rotate(void)
{
    FormType            *frm;

#ifdef USE_HANDERA
    VgaScreenModeType   handEraScreenMode;     // �X�N���[�����[�h
    VgaRotateModeType   handEraRotateMode;     // ��]���[�h

    if(NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        // HandEra�f�o�C�X�Ȃ�A��ʂ���]������
        VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
        VgaSetScreenMode(screenMode1To1, VgaRotateSelect(handEraRotateMode));
        frm = FrmGetActiveForm();
        HandEraResizeForm(frm);
        FrmDrawForm(frm);
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, (NNshGlobal->NNsiParam)->selectedTitleItem,
                                                             NNSH_STEP_REDRAW);
        return;
    }
#endif
#ifdef USE_PIN_DIA
    if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
        (NNshGlobal->device == NNSH_DEVICE_GARMIN))
    {
        // DIA�T�|�[�g�f�o�C�X�Ȃ�A��ʉ�]�����s
        VgaFormRotate_DIA();

        // ��ʉ�]��̑傫���ɂ��킹�A�t�H�[���𒲐����`�悷��
        frm = FrmGetActiveForm();
        if (HandEraResizeForm(frm) == true)
        {
            FrmDrawForm(frm);
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, (NNshGlobal->NNsiParam)->selectedTitleItem,
                                                             NNSH_STEP_REDRAW);
        }
        return;
    }
#endif    // #ifdef USE_PIN_DIA

    // "���݃T�|�[�g���Ă��܂���" �\�����s��
    NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : updateSelectionItem                                        */
/*                                                          �I��item�̍X�V */
/*-------------------------------------------------------------------------*/
static void updateSelectionItem(Int16 direction)
{
    FormType      *frm;
    RectangleType  dimF;
    Int16          item;
    UInt16         fontID, nlines, fontHeight;

    // �I���A�C�e�����ړ�������
    item = (NNshGlobal->NNsiParam)->selectedTitleItem + direction;
    
    // �O�y�[�W���I�����ꂽ�ꍇ...
    if (item < 0)
    {
        // �O�y�[�W������ꍇ�ɂ́A�P�O�̃y�[�W��\������
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_HALFWAY)||
             ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_LOWERLIMIT))
        {   
            // �O�y�[�W��\������
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS,
                               NNSH_ITEM_LASTITEM, NNSH_STEP_PAGEDOWN);
        }
        return;
    }
         
    // �I���A�C�e����ύX(��ʃy�[�W���؂�ւ��Ȃ������ꍇ�ɂ͑I��item���X�V)    
    if (item < NNshGlobal->nofTitleItems)
    {
        frm = FrmGetActiveForm();
        FrmGetObjectBounds(frm,
                            FrmGetObjectIndex(frm, GADID_MESSAGE_LIST), &dimF);

        fontID = ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ?
                           (NNshGlobal->NNsiParam)->currentFont : (NNshGlobal->NNsiParam)->sonyHRFontTitle;

        NNsi_UpdateRectangle(&dimF, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                                                &fontID, &fontHeight, &nlines);

        // �X���ꗗ�̕\��(�`�������)
        NNsi_WinUpdateList(item, (NNshGlobal->NNsiParam)->selectedTitleItem,
                         NNshGlobal->msgTitleIndex, nlines, &dimF, fontHeight);
        (NNshGlobal->NNsiParam)->selectedTitleItem = item;

        return;
    }

    // ��ʃy�[�W�؂�ւ�(���y�[�W������ꍇ�ɂ́A���y�[�W��\������)
    if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_HALFWAY)||
        ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_UPPERLIMIT))
    {     
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_PAGEUP);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : change_list_font                                           */
/*                                                            �t�H���g�ύX */
/*-------------------------------------------------------------------------*/
static void change_list_font(void)
{
#ifdef USE_TSPATCH
    UInt32  ver, fontId;
    Err     ret;
#endif

    // �t�H���gID�̑I��
    if ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0)
    {
        // �ʏ�`�惂�[�h�p�t�H���g�̕ύX
        (NNshGlobal->NNsiParam)->currentFont = FontSelect((NNshGlobal->NNsiParam)->currentFont);

#ifdef USE_TSPATCH
        // TsPatch�@�\���g���Ƃ�...
        if ((NNshGlobal->NNsiParam)->notUseTsPatch == 0)
        {    
            // TsPatch�K�p�����ǂ����`�F�b�N����
            ret = FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT_SUPPORT, &ver);
            if (ret == errNone)
            {    
                // TsPatch�K�p��...�t�H���g������������B
                switch ((NNshGlobal->NNsiParam)->currentFont)
                {
                  case stdFont:
                    FtrGet(SmallFontAppCreator, SMF_FTR_TINY_FONT, &fontId);
                    break;

                  case boldFont:
                    FtrGet(SmallFontAppCreator, SMF_FTR_TINY_BOLD_FONT, &fontId);
                    break;

                  case largeFont:
                    FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_FONT, &fontId);
                    break;

                  case largeBoldFont:
                    FtrGet(SmallFontAppCreator, SMF_FTR_SMALL_BOLD_FONT, &fontId);
                    break;

                  default:
                    fontId = (NNshGlobal->NNsiParam)->currentFont;
                    break;
                }
                (NNshGlobal->NNsiParam)->currentFont = fontId;
            }
        }
#endif  // #ifdef USE_TSPATCH

    }
    else
    {
        // �n�C���]���[�h�p�t�H���g�̕ύX
        (NNshGlobal->NNsiParam)->sonyHRFontTitle = FontSelect((NNshGlobal->NNsiParam)->sonyHRFontTitle);
    }

    // �t�H���g�ύX��̕\���́A���X�g���č쐬���ĕ\��������
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);

    NNshGlobal->updateHR = NNSH_UPDATE_DISPLAY;

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : change_graph_mode                                          */
/*                                        �O���t�B�b�N�`�惂�[�h�̐؂�ւ� */
/*-------------------------------------------------------------------------*/
static void change_graph_mode(void)
{
    // �n�C���]���[�h/�ʏ�`�惂�[�h�̐؂�ւ�(1:�n�C���], 0:�ʏ�`��)
    (NNshGlobal->NNsiParam)->useSonyTinyFontTitle = 
                                ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ? 1 : 0;

    // �`�惂�[�h�ύX��̕\���́A���X�g���č쐬���ĕ\��������
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : Show_tab_info                                              */
/*                                                          �^�u���̕\�� */
/*-------------------------------------------------------------------------*/
UInt16 Show_tab_info(Boolean disp)
{
    UInt16               tabNum, selBBS, count, loop;
    Char                 logMsg[MINIBUF], buffer[BUFSIZE * 2];
    ListType            *lstP;
    NNshBoardDatabase    bbsInfo;
    NNshSubjectDatabase *mesRead;
    MemHandle            mesH;
    DmOpenRef            dbRef;

    // �ϐ��̏�����
    tabNum = 0;
    MemSet(buffer, sizeof(buffer), 0x00);

    // �^�u�����쐬
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);

    StrCopy(buffer, MSG_TAB_INFO);
    StrCat (buffer, LstGetSelectionText(lstP, selBBS));

    // "�X��������"��\������
    Show_BusyForm(MSG_SEARCHING_TITLE);

    switch (selBBS)
    {
      case NNSH_SELBBS_NOTREAD:     // ���ǂ���
      case NNSH_SELBBS_GETALL:      // �擾�ςݑS��
      case NNSH_SELBBS_FAVORITE:    // ���C�ɓ���X��
        // BBS���͎擾���Ȃ�(�����擾)
        break;

      case NNSH_SELBBS_CUSTOM1:     // CUSTOM1
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom1.boardNick, &bbsInfo);
        break;

      case NNSH_SELBBS_CUSTOM2:     // CUSTOM2
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom2.boardNick, &bbsInfo);
        break;

      case NNSH_SELBBS_CUSTOM3:     // CUSTOM3
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom3.boardNick, &bbsInfo);
        break;

      case NNSH_SELBBS_CUSTOM4:     // CUSTOM4
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom4.boardNick, &bbsInfo);
        break;

      case NNSH_SELBBS_CUSTOM5:     // CUSTOM5
        (void) Get_BBS_Info((NNshGlobal->NNsiParam)->custom5.boardNick, &bbsInfo);
        break;

      default:
        // ����BBS�ȊO�̂Ƃ�
        Get_BBS_Info(selBBS, &bbsInfo);
        break;
    }

    // �S�X�������擾����
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &count);

    // �S���̊m�F������
    for (loop = count; loop > 0; loop--)
    {
        // ���悻�̌����c�茏����\������
        if ((loop % 10) == 0)
        {
            StrCopy(logMsg, MSG_SEARCHING_TITLE MSG_SEARCHING_LEFT);
            NUMCATI(logMsg, loop);
            SetMsg_BusyForm(logMsg);
        }

        // �X���f�[�^���擾����
        GetRecordReadOnly_NNsh(dbRef, (loop - 1), &mesH, (void **) &mesRead);

        // �����ɍ��v���邩�ǂ����`�F�b�N
        if (CheckDispList(selBBS, mesRead, &bbsInfo) == true)
        {
            tabNum++;
        }

        // ���R�[�h���b�N�̉���
        ReleaseRecordReadOnly_NNsh(dbRef, mesH);
   }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);

    // ������\������
    if (disp == true)
    {
        StrCat(buffer, MSG_TAB_COUNT);
        NUMCATI(buffer, tabNum);
        NNsh_ErrorMessage(ALTID_INFO, buffer, " ", 0);
    }
    return (tabNum);
}

/*-------------------------------------------------------------------------*/
/*   Function : setMultiPurposeSwitch                                      */
/*                                          ���ړI�X�C�b�`�ɏ����f�[�^�ݒ� */
/*-------------------------------------------------------------------------*/
static void setMultiPurposeSwitch(FormType *frm, UInt16 itemID, UInt16 btnID)
{
    UInt16       data;
    ControlType *ctlP;

    // ���ړI�X�C�b�`�ɐݒ肳�ꂽ��ڂɂ��킹�ăf�[�^���擾����
    switch (itemID & (NNSH_SWITCHUSAGE_FUNCMASK))
    {
      case NNSH_SWITCHUSAGE_GETPART:
        // �ꊇ/����
        data = (NNshGlobal->NNsiParam)->enablePartGet;
        break;

      case NNSH_SWITCHUSAGE_HANZEN:
        // ���p/�S�p
        data = (NNshGlobal->NNsiParam)->convertHanZen;
        break;

      case NNSH_SWITCHUSAGE_USEPROXY:
        // proxy�g�p
        data = (NNshGlobal->NNsiParam)->useProxy;
        break;

      case NNSH_SWITCHUSAGE_CONFIRM:
        // �m�F���ȗ�
        data = (NNshGlobal->NNsiParam)->confirmationDisable;
        break;

      case NNSH_SWITCHUSAGE_SUBJECT:
        // �ꗗ�S�擾
        data = (NNshGlobal->NNsiParam)->getAllThread;
        break;

      case NNSH_SWITCHUSAGE_TITLEDISP:
        // �`�惂�[�h
        data = (NNshGlobal->NNsiParam)->useSonyTinyFontTitle;
        break;

      case NNSH_SWITCHUSAGE_MSGNUMBER:
        // �ꗗ�ɔԍ�
        data = (NNshGlobal->NNsiParam)->printNofMessage;
        break;

      case NNSH_SWITCHUSAGE_GETRESERVE:
        // �X���擾�\��
        data = (NNshGlobal->NNsiParam)->getReserveFeature;
        break;

      case NNSH_SWITCHUSAGE_IMODEURL:
        // i-mode URL�g�p
        data = (NNshGlobal->NNsiParam)->useImodeURL;
        break;

      case NNSH_SWITCHUSAGE_BLOCKDISP:
        // �]�[���\�����[�h
        data = (NNshGlobal->NNsiParam)->blockDispMode;
        break;

      case NNSH_SWITCHUSAGE_MACHINEW:
        // �܂�BBS���V���m�F�����{����
        data = (NNshGlobal->NNsiParam)->enableNewArrivalHtml;
        break;

      case NNSH_SWITCHUSAGE_USEPLUGIN:
        // �V���m�F��Ƀ��[���`�F�b�N�����{����
        data = (NNshGlobal->NNsiParam)->use_DAplugin;
        break;

      case NNSH_SWITCHUSAGE_LISTUPDATE:
        // �ꗗ�X�V���ɓ����ɃX�������擾
        data = (NNshGlobal->NNsiParam)->listAndUpdate;
        break;

      case NNSH_SWITCHUSAGE_NOREADONLY:
        // �擾�ςݑS�Ăɂ͎Q�ƃ��O�Ȃ�
        data = (UInt16) (NNshGlobal->NNsiParam)->notListReadOnly;
        break;
      
      case NNSH_SWITCHUSAGE_NORDOLY_NW:
        // ���ǂ���ɂ͎Q�ƃ��O�Ȃ�
        data = (UInt16) (NNshGlobal->NNsiParam)->notListReadOnlyNew;
        break;

      case NNSH_SWITCHUSAGE_DISCONN_NA:
        // �V���m�F�I����ɉ����ؒf����
        data = (NNshGlobal->NNsiParam)->disconnArrivalEnd;
        break;
        
      case NNSH_SWITCHUSAGE_PREEXEC_NA:
        // �V���m�F���s�O�ɂc�`�����s����
        data = (UInt16) (NNshGlobal->NNsiParam)->preOnDAnewArrival;
        break;

      case NNSH_SWITCHUSAGE_PR_NOTREAD:
        // �ꗗ��ʂɕ\�����鐔�l�𖢓ǐ��ɂ���
        data = (UInt16) (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum;
        break;

      case NNSH_SWITCHUSAGE_GETLOGURL:
        // �ꗗ��ʂ��Q�ƃ��O�ꗗ�Ăƕʕ\�����[�h�Ƃ��Ďg�p����ꍇ�B�B�B
        data = (UInt16) (NNshGlobal->NNsiParam)->getLogSiteListMode;
 #ifdef USE_LOGCHARGE
         if (data == 0)
         {
            // �ʏ탂�[�h�֐؂�ւ�
            FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));
            FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), POPTRID_BBSINFO));            
         }
         else
         {
            // �Q�ƃ��O�ꗗ�T�C�g�ʕ\�����[�h�֐؂�ւ�
            FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), POPTRID_BBSINFO));            
            FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));

            // �T�C�g���𕜋�����
            changeGetLogUrlSite(true);
         }
#endif  // #ifdef USE_LOGCHARGE
        break;

      case NNSH_SWITCHUSAGE_NOUSE:
      default:
        // �g�p���Ȃ�(�`�F�b�N�{�b�N�X���̉�ʕ\�����Ȃ�)
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, btnID));
        CtlSetValue  (ctlP, 0);
        CtlSetEnabled(ctlP, false);
        CtlSetUsable (ctlP, false);
        return;
        break;
    }
    // �ݒ�l����ʂɔ��f������
    SetControlValue(frm, btnID, &data);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : updateMultiPurposeSwitch                                   */
/*                                                  ���ړI�{�^���������ꂽ */
/*-------------------------------------------------------------------------*/
static Boolean updateMultiPurposeSwitch(UInt16 paramID, UInt16 btnID)
{
    Boolean ret;
    UInt16  data;
    Char    *ptr;

    // �f�[�^�̏�����
    ret = false;
    data = 0;
    ptr  = "";

    // �ݒ肳�ꂽ�f�[�^���擾����
    UpdateParameter(FrmGetActiveForm(), btnID, &data);
    switch (paramID & (NNSH_SWITCHUSAGE_FUNCMASK))
    {
      case NNSH_SWITCHUSAGE_GETPART:
        // �ꊇ/����
        (NNshGlobal->NNsiParam)->enablePartGet = data;
        ptr = MSG_PARTGET_MODE;
        break;

      case NNSH_SWITCHUSAGE_HANZEN:
        // ���p/�S�p
        (NNshGlobal->NNsiParam)->convertHanZen = data;
        ptr = MSG_HANZEN_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_USEPROXY:
        // proxy�g�p
        (NNshGlobal->NNsiParam)->useProxy = data;
        ptr = MSG_USEPROXY_MODE;
        break;

      case NNSH_SWITCHUSAGE_CONFIRM:
        if (data == 0)
        {
            // �m�F���ȗ�
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_NOTHING;
        }
        else
        {
            // �_�C�A���O��S�ďȗ�
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        }
        ptr = MSG_OMITCONFIRM_MODE;
        break;

      case NNSH_SWITCHUSAGE_SUBJECT:
        // �ꗗ�S�擾
        (NNshGlobal->NNsiParam)->getAllThread = data;
        ptr = MSG_GETALLSUBJECT_MODE;
        break;

      case NNSH_SWITCHUSAGE_TITLEDISP:
        // �`�惂�[�h
        (NNshGlobal->NNsiParam)->useSonyTinyFontTitle = data;
        ptr = MSG_GRAPHIC_DRAW_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_MSGNUMBER:
        // �ꗗ�ɔԍ�
        (NNshGlobal->NNsiParam)->printNofMessage = data;
        ptr = MSG_SHOWMESSAGENUM_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_GETRESERVE:
        // �X���擾�\��
        (NNshGlobal->NNsiParam)->getReserveFeature = data;
        ptr = MSG_GETRESERVE_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_IMODEURL:
        // i-mode URL�g�p���[�h
        (NNshGlobal->NNsiParam)->useImodeURL = data;
        ptr = MSG_IMODEURL_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_BLOCKDISP:
        // �]�[���\�����[�h
        (NNshGlobal->NNsiParam)->blockDispMode = data * NNSH_BLOCKDISP_NOFMSG;
        ptr = MSG_BLOCKDISP_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_MACHINEW:
        // �܂�BBS���V���m�F����
        (NNshGlobal->NNsiParam)->enableNewArrivalHtml = data;
        ptr = MSG_MACHINEW_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_USEPLUGIN:
        // �V���m�F���Ƀ��[�������m�F�����{����
        (NNshGlobal->NNsiParam)->use_DAplugin = data;
        ptr = MSG_USE_DA_PLUGIN_MODE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_LISTUPDATE:
        // �ꗗ�擾���ɓ����ɃX�������擾�����{����
        (NNshGlobal->NNsiParam)->listAndUpdate = data;
        ptr = MSG_LISTGET_AND_UPDATE;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_NOREADONLY:
        // �擾�ςݑS�Ăɂ͎Q�ƃ��O�Ȃ�
        (NNshGlobal->NNsiParam)->notListReadOnly = (UInt8) data;
        ptr = MSG_NOTREADONLY;
        ret = true;
        break;
      
      case NNSH_SWITCHUSAGE_NORDOLY_NW:
        // ���ǂ���ɂ͎Q�ƃ��O�Ȃ�
        (NNshGlobal->NNsiParam)->notListReadOnlyNew = (UInt8) data;
        ptr = MSG_NOTREADONLY_NEW;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_DISCONN_NA:
        // �V���m�F��ɉ����ؒf����
         (NNshGlobal->NNsiParam)->disconnArrivalEnd = data;
         ptr = MSG_DISCONNECT_NEWARRIVAL;
         ret = true;
         break;

      case NNSH_SWITCHUSAGE_PREEXEC_NA:
        // �V���m�F�O��DA�����s����
         (NNshGlobal->NNsiParam)->preOnDAnewArrival = (UInt8) data;
         ptr = MSG_PREEXECDA_NEWARRIVAL;
         ret = true;
         break;

      case NNSH_SWITCHUSAGE_PR_NOTREAD:
        // �ꗗ��ʂɕ\�����鐔�l�𖢓ǐ��ɂ���
        if (data == 0)
        {
            // ���X����\������
            (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = (UInt8) 0;
        }
        else
        {
            // ���ǐ���\������
            (NNshGlobal->NNsiParam)->listMesNumIsNotReadNum = (UInt8) 1;
        }
        ptr = MSG_DISPLAY_NOTREAD;
        ret = true;
        break;

      case NNSH_SWITCHUSAGE_GETLOGURL:
        // �ꗗ��ʂ��Q�ƃ��O�ꗗ�T�C�g�ʕ\�����[�h�Ƃ��Ďg�p����ꍇ�B�B�B
        (NNshGlobal->NNsiParam)->getLogSiteListMode = (UInt8) data;
        NNshWinSetPopItems(FrmGetActiveForm(),POPTRID_BBSINFO, LSTID_BBSINFO, NNSH_SELBBS_OFFLINE);
        ptr = MSG_DISPLAY_GETLOGURL;
        (NNshGlobal->NNsiParam)->lastBBS = NNSH_SELBBS_OFFLINE;
        (NNshGlobal->NNsiParam)->selectedTitleItem = 0;
#ifdef USE_LOGCHARGE
        if (data == 0)
        {
            // �ʏ탂�[�h�֐؂�ւ�
            FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));
            FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), POPTRID_BBSINFO));            
        }
        else
        {
            // �Q�ƃ��O�ꗗ�T�C�g�ʕ\�����[�h�֐؂�ւ�
            FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), POPTRID_BBSINFO));            
            FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));            
        }         
#endif  // #ifdef USE_LOGCHARGE
        ret = true;
        break;
      
      case NNSH_SWITCHUSAGE_NOUSE:
      default:
        // �g�p���Ȃ�
        return (false);
        break;
    }
    if (data == 0)
    {
        // �ݒ�������������Ƃ�ʒm����
        NNsh_InformMessage(ALTID_INFO, ptr, MSG_RESET_SUFFIX, 0);
    }
    else
    {
        // �ݒ肵�����Ƃ�ʒm����
        NNsh_InformMessage(ALTID_INFO, ptr, MSG_SET_SUFFIX, 0);
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveSelectionTab                                           */
/*                                                      �\���^�u��ύX���� */
/*-------------------------------------------------------------------------*/
static void moveSelectionTab(UInt16 selBBS)
{
    ListType    *lstP;
    ControlType *ctlP;
    UInt16       savedParam;

    if ((NNshGlobal->NNsiParam)->autoDeleteNotYet != 0)
    {
        // �ړ����ɖ��擾�X�����폜����ꍇ...
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        delete_notreceived_message(false);
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }

    // BBS�\�����X�g��ύX����
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    LstSetTopItem  (lstP, selBBS);
    LstSetSelection(lstP, selBBS);

    // BBS�\���^�u��ύX����
    (NNshGlobal->NNsiParam)->lastBBS = selBBS;

    // �\���^�u(���x��)������������
    ctlP = FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO);
    CtlSetLabel(ctlP, LstGetSelectionText(lstP, selBBS));

    // ���X�g���X�V����
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);

    return; 
}

/*-------------------------------------------------------------------------*/
/*   Function : ExecuteAction                                              */
/*                                          �@�\�����s����(���ړI�{�^���p) */
/*-------------------------------------------------------------------------*/
Boolean ExecuteAction(UInt16 funcID)
{
    Boolean      ret = false;
    UInt16       item, dummy;
    EventType   *dummyEvent;
    Char        *data;

    // �@�\�R�}���h�ԍ���jumpSelection�Ɋi�[
    // (����܂�ǂ��Ȃ��B)
    NNshGlobal->jumpSelection = funcID;
    switch (funcID)
    {
      case MULTIBTN_FEATURE_INFO:
         // �X�����
        if (NNshGlobal->nofTitleItems != 0)
        {
            item = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem);
            (void) DisplayMessageInformations(item, &dummy);

            NNsi_RedrawThreadList();
        }
        break;

      case MULTIBTN_FEATURE_OPEN:
        // �X���Q��
        display_message();
        break;

      case MULTIBTN_FEATURE_GET:
        // ���b�Z�[�W�擾(�����擾/�S�擾�͎����؊���)
        update_getall_message();
        break;

      case MULTIBTN_FEATURE_PARTGET:
        // ���b�Z�[�W�����擾
        update_message();
        break;

      case MULTIBTN_FEATURE_ALLGET:
        // ���b�Z�[�W�S���擾(�Ď擾)
        get_all_message();
        break;

      case MULTIBTN_FEATURE_DELETE:
        // �X���폜
        delete_message();
        break;

      case MULTIBTN_FEATURE_SORT:
        // �X���̐�������s����
        Show_BusyForm(MSG_SORTING_WAIT);
        SortSubjectList();
        Hide_BusyForm(false);

        // �X���^�C�g���̐擪�ֈړ�������
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        break;

      case MULTIBTN_FEATURE_MODTITLE:
        // �X���^�C�g���ύX
        modify_title();
        break;

      case MULTIBTN_FEATURE_OPENWEB:
        // �I�������X�����u���E�U�ŊJ��
        if (NNshGlobal->browserCreator != 0)
        {
            // WebBrowser�ŊJ��
            launch_WebBrowser(NNSH_DISABLE);
        }
        else
        {
            // "���݃T�|�[�g���Ă��܂���" �\�����s��
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;

      case MULTIBTN_FEATURE_MESNUM:
        // �X���Ԏw��̃��b�Z�[�W�擾
        Get_MessageFromMesNum(&ret, &dummy, false);
        if (ret == true)
        {
            // BBS�ꗗ�����������Ă���
            MEMFREE_PTR(NNshGlobal->bbsTitles);

            // �g�pBBS�ꗗ�̍X�V
            (void) Create_BBS_INDEX(&(NNshGlobal->bbsTitles), &item);
            if (item < (NNshGlobal->NNsiParam)->lastBBS)
            {
                (NNshGlobal->NNsiParam)->lastBBS = 0;
            }
        }
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        break;

      case MULTIBTN_FEATURE_SEARCH:
        // �^�C�g������(������̐ݒ�)
        if (NNshGlobal->nofTitleItems != 0)
        {
            item = convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem) + 1;
        }
        else
        {
            item = 0;
        }
        if (set_SearchTitleString(&(NNshGlobal->searchTitleH), item, NULL) != true)
        {
            return (true);
        }
        if ((NNshGlobal->NNsiParam)->searchPickupMode != 0)
        {
            // �i���݌������[�h�̂Ƃ��̓X���^�C�g���̐擪�ֈړ����ďI������...
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_TO_TOP);
            return (true);
        }
        // not break;  (�����ăX���^�C���������s����)

      case MULTIBTN_FEATURE_NEXT:
        // �X���^�C��������(�����)
        (void) search_NextTitle(NNSH_SEARCH_FORWARD);
        break;

      case MULTIBTN_FEATURE_PREV:
        // �X���^�C��������(�O����)
        (void) search_NextTitle(NNSH_SEARCH_BACKWARD);
        break;

      case MULTIBTN_FEATURE_TOP:
        // �X���^�C�g���̐擪�ֈړ�
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_TO_TOP);
        break;

      case MULTIBTN_FEATURE_BOTTOM:
        // �X���^�C�g���̖����ֈړ�(�_�~�[�ňُ�f�[�^�𑗂荞��)
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, NNSH_ITEM_LASTITEM,
                                                          NNSH_STEP_TO_BOTTOM);
        break;

      case MULTIBTN_FEATURE_GETBBS:
        // ���̍X�V(�擾)
        if (GetBBSList((NNshGlobal->NNsiParam)->bbs_URL) == false)
        {
            return (true);
        }
        /** not break; (�����Ďg�p�I�����s��) **/

      case MULTIBTN_FEATURE_USEBBS:
        // �g�p�I����ʂ��J��
        NNshMain_Close(FRMID_MANAGEBBS);
        break;

      case MULTIBTN_FEATURE_COPYGIKO:
        // �Q�ƃ��O�ɃR�s�[����
        copy_to_readOnly();
        break;

      case MULTIBTN_FEATURE_DELNOTGET:
        // ���擾�X���폜
        delete_notreceived_message(true);
        break;

      case MULTIBTN_FEATURE_DBCOPY:
        // DB��VFS�Ƀo�b�N�A�b�v
        if ((((NNshGlobal->NNsiParam)->useVFS) & (NNSH_VFS_ENABLE)) == NNSH_VFS_ENABLE)
        {
            // DB��VFS�Ƀo�b�N�A�b�v����
            BackupDBtoVFS(NNSH_ENABLE);
        }
        else
        {
             // "���݃T�|�[�g���Ă��܂���" �\�����s��
             NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;

      case MULTIBTN_FEATURE_GRAPHMODE:
        // �`�惂�[�h�ύX
        change_graph_mode();
        break;

      case MULTIBTN_FEATURE_ROTATE:
        // HandEra��ʉ�]
        change_handera_rotate();
        break;

      case MULTIBTN_FEATURE_FONT:
        // �t�H���g�ύX
        change_list_font();
        break;

      case MULTIBTN_FEATURE_NETWORK:
        // �l�b�g���[�N�ݒ���J��
        OpenNetworkPreferences();
        break;

      case MULTIBTN_FEATURE_SELMENU:
        // �I�����j���[�\��
        open_menu();
        break;

      case MULTIBTN_FEATURE_DISCONN:
        // ����ؒf
        NNshNet_LineHangup();
        break;

      case MULTIBTN_FEATURE_GETLIST:
        // �X���ꗗ�擾(SUBJECT.TXT�擾)
        get_subject_txt();
        break;

      case MULTIBTN_FEATURE_NEWARRIVAL:
        // �V�����b�Z�[�W�m�F
        update_newarrival_message(NNSH_NEWARRIVAL_NORMAL);
        break;

      case MULTIBTN_FEATURE_NNSHSET:
        // NNsi�ݒ�(�T��)��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING_ABSTRACT);
        break;

      case MULTIBTN_FEATURE_NNSHSET1:
        // NNsi�ݒ�-1��ʂ��J��
        NNshMain_Close(FRMID_CONFIG_NNSH);
        break;

      case MULTIBTN_FEATURE_NNSHSET2:
        // NNsi�ݒ�-2��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING2);
        break;

      case MULTIBTN_FEATURE_NNSHSET3:
        // NNsi�ݒ�-3��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING3);
        break;

      case MULTIBTN_FEATURE_NNSHSET4:
        // NNsi�ݒ�-4��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING4);
        break;

      case MULTIBTN_FEATURE_NNSHSET5:
        // NNsi�ݒ�-5��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING5);
        break;

      case MULTIBTN_FEATURE_NNSHSET6:
        // NNsi�ݒ�-6��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING6);
        break;

      case MULTIBTN_FEATURE_NNSHSET7:
        // NNsi�ݒ�-7��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING7);
        break;

      case MULTIBTN_FEATURE_NNSHSET8:
        // NNsi�ݒ�-8��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING8);
        break;

      case MULTIBTN_FEATURE_NNSHSET9:
        // NNsi�ݒ�-9��ʂ��J��
        NNshMain_Close(FRMID_NNSI_SETTING9);
        break;

      case MULTIBTN_FEATURE_VERSION:
        // �o�[�W�������\��
        ShowVersion_NNsh();
        break;

      case MULTIBTN_FEATURE_SELECTNEXT:
        // 1���̃A�C�e����I������
        updateSelectionItem(NNSH_STEP_ITEMDOWN);
        break;

      case MULTIBTN_FEATURE_SELECTPREV:
        // 1��̃A�C�e����I������
        updateSelectionItem(NNSH_STEP_ITEMUP);
        break;

      case MULTIBTN_FEATURE_PREVPAGE:
        // �O�y�[�W�̕\��
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_HALFWAY)||
            ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_LOWERLIMIT))
        {
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_PAGEDOWN);
        }
        break;

      case MULTIBTN_FEATURE_NEXTPAGE:
        // ���y�[�W�̕\��
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_HALFWAY)||
            ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_UPPERLIMIT))
        {
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_PAGEUP);
        }
        break;

      case MULTIBTN_FEATURE_NNSIEND:
        // NNsi�I��
        dummyEvent = &(NNshGlobal->dummyEvent);
        MemSet(dummyEvent, sizeof(EventType), 0x00);
        dummyEvent->eType = appStopEvent;
        EvtAddEventToQueue(dummyEvent);
        break;

      case MULTIBTN_FEATURE_USER1TAB:
      case MULTIBTN_FEATURE_USER2TAB:
      case MULTIBTN_FEATURE_USER3TAB:
      case MULTIBTN_FEATURE_USER4TAB:
      case MULTIBTN_FEATURE_USER5TAB:
        // ���[�U�^�u�ݒ��ʂ��J��
        NNshMain_Close(FRMID_FAVORSET_DETAIL);
        break;

      case MULTIBTN_FEATURE_DEVICEINFO:
        // �f�o�C�X�̏��\��
        ShowDeviceInfo_NNsh();
        break;
        
      case MULTIBTN_FEATURE_GOTODUMMY:
        // �_�~�[��ʂ֑J��
        NNshMain_Close(FRMID_NNSI_DUMMY);
        break;

      case MULTIBTN_FEATURE_NEWTHREAD:
        // �X�����ĉ�ʂ֑J��
        newThread_message();
        break;

      case MULTIBTN_FEATURE_DIRSELECT:
        // �f�B���N�g���I��
        if ((NNshGlobal->NNsiParam)->lastBBS == NNSH_SELBBS_OFFLINE)
        {
            // �^�u���u�Q�ƃ��O�v�̎������A�i�[�f�B���N�g���I����
            readonly_dir_selection();
        }
        break;
      case MULTIBTN_FEATURE_TABINFO:
        // �^�u���
        Show_tab_info(true);
        break;

      case MULTIBTN_FEATURE_MOVELOGLOC:
        // ���O�Ǘ��ꏊ�؂�ւ�
        move_log_location();
        break;

      case MULTIBTN_FEATURE_MOVELOGDIR:
        // ���O�i�[�f�B���N�g���̕ύX
        move_log_directory();
        break;

      case MULTIBTN_FEATURE_TO_NOTREAD:
        // '���ǂ���'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_NOTREAD);
        break;

      case MULTIBTN_FEATURE_TO_GETALL:
        // '�擾�ςݑS��'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_GETALL);
        break;

      case MULTIBTN_FEATURE_LOGCHARGE:
        // �Q�ƃ��O�̎擾
#ifdef USE_LOGCHARGE
        GetReadOnlyLogData_NNsh();
        
        // ��ʕ\�����X�V����
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                           (NNshGlobal->NNsiParam)->selectedTitleItem,
                           NNSH_STEP_REDRAW);
#endif
        break;

      case MULTIBTN_FEATURE_OPENURL:
#ifdef USE_LOGCHARGE
        // URL���J��
        openURL();
#endif
        break;

      case MULTIBTN_FEATURE_SETNGWORD1:
      case MULTIBTN_FEATURE_SETNGWORD2:
      case MULTIBTN_FEATURE_SETNGWORD3:
        // NG���[�h�ݒ�
        SetNGword3();
        NNsi_RedrawThreadList();
        break;

      case MULTIBTN_FEATURE_SETLOGCHRG:
#ifdef USE_LOGCHARGE
        // �Q�ƃ��O�擾�ݒ�
        SetLogGetURL(NNSH_ITEM_BOTTOMITEM);
        NNsi_RedrawThreadList();
#endif
        break;

      case MULTIBTN_FEATURE_BT_ON:
        // Bluetooth ON
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(true);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MULTIBTN_FEATURE_BT_OFF:
        // Bluetooth OFF
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(false);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MULTIBTN_FEATURE_TO_OFFLINE:
        // '�擾�ςݑS��'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_OFFLINE);
        break;

      case MULTIBTN_FEATURE_TO_FAVOR:
        // '���C�ɓ���'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_FAVORITE);
        break;

      case MULTIBTN_FEATURE_TO_USER1:
        // '���[�U1'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_CUSTOM1);
        break;

      case MULTIBTN_FEATURE_TO_USER2:
        // '���[�U2'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_CUSTOM2);
        break;

      case MULTIBTN_FEATURE_TO_USER3:
        // '���[�U3'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_CUSTOM3);
        break;

      case MULTIBTN_FEATURE_TO_USER4:
        // '���[�U4'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_CUSTOM4);
        break;

      case MULTIBTN_FEATURE_TO_USER5:
        // '���[�U5'�ֈړ�
        moveSelectionTab(NNSH_SELBBS_CUSTOM5);
        break;

      case MULTIBTN_FEATURE_MACRO_SET:
#ifdef USE_MACROEDIT
        // �}�N���ݒ�
        NNsi_MacroDataEdit(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT);
        //NNsi_RedrawThreadList();
#else
        // "���݃T�|�[�g���Ă��܂���" �\�����s��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;
      
      case MULTIBTN_FEATURE_MACRO_EXEC:
#ifdef USE_MACRO
        // �}�N�����s
        NNsi_ExecuteMacroMain(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);
        NNsi_RedrawThreadList();
#else
        // "���݃T�|�[�g���Ă��܂���" �\�����s��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;

      case MULTIBTN_FEATURE_MACRO_VIEW:
#ifdef USE_MACRO
        // MACRO�\��
        NNsi_MacroDataView(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT);
        //NNsi_RedrawThreadList();
#else
        // "���݃T�|�[�g���Ă��܂���" �\�����s��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;

      case MULTIBTN_FEATURE_MACRO_LOG:
#ifdef USE_MACRO
        // MACRO���O�\��
        NNsi_MacroExecLogView(DBNAME_MACRORESULT, DBVERSION_MACRO);
        //NNsi_RedrawThreadList();
#else
        // "���݃T�|�[�g���Ă��܂���" �\�����s��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;
      case MULTIBTN_FEATURE_SELECT_BBS:
#ifdef USE_LOGCHARGE
        if ((NNshGlobal->NNsiParam)->getLogSiteListMode != 0)
        {
            // �T�C�g��I������
            changeGetLogUrlSite(false);

            // �X���ꗗ�̕\�����X�V
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_TO_TOP);
        }
        else
#endif // #ifdef USE_LOGCHARGE
        {
            // ��I��
            CtlHitControl(FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO));
        }
        break;

      case MULTIBTN_FEATURE_OYSTERLOGIN:
#ifdef USE_SSL
        // '��'���O�C��
        ProceedOysterLogin();
#else
        // "���݃T�|�[�g���Ă��܂���" �\�����s��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;
        
      case MULTIBTN_FEATURE_USAGE:
        // ����w���v�̕\��
        // (�N���b�v�{�[�h�ɁAnnDA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����)
        data = MEMALLOC_PTR(sizeof(nnDA_NNSIEXT_VIEWSTART
                                    nnDA_NNSIEXT_INFONAME
                                    nnDA_NNSIEXT_HELPLIST
                                    nnDA_NNSIEXT_ENDINFONAME
                                    nnDA_NNSIEXT_ENDVIEW) + MARGIN);
        if (data != NULL)
        {
            // nnDA���N������
            MemSet(data, 
                   sizeof(nnDA_NNSIEXT_VIEWSTART
                           nnDA_NNSIEXT_INFONAME
                           nnDA_NNSIEXT_HELPLIST
                           nnDA_NNSIEXT_ENDINFONAME
                           nnDA_NNSIEXT_ENDVIEW) + MARGIN,
                   0x00);
            StrCopy(data, 
                    nnDA_NNSIEXT_VIEWSTART
                      nnDA_NNSIEXT_INFONAME
                      nnDA_NNSIEXT_HELPLIST
                      nnDA_NNSIEXT_ENDINFONAME
                      nnDA_NNSIEXT_ENDVIEW);            
            ClipboardAddItem(clipboardText, data, StrLen(data) + 1);
            MEMFREE_PTR(data);
            (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        }
        break;

#ifdef USE_LOGCHARGE
      case MULTIBTN_FEATURE_SETTOKEN:
        // ��؂蕶����ݒ�(�擪���R�[�h��\������)
        SetLogCharge_LogToken(0);
        break;

      case MULTIBTN_FEATURE_OPENGETLOGLIST:
        // �擾�Q�ƃ��O�ꗗ��ʂ��J��
        NNshMain_Close(FRMID_LISTGETLOG);
        break;
#endif  // #ifdef USE_LOGCHARGE

      case MULTIBTN_FEATURE_SEARCHBBS:
        // ����������
        if (set_SearchTitleString(&(NNshGlobal->searchBBSH), 0, NULL) != true)
        {
            return (true);
        }
        if (search_NextBBS(NNSH_NOF_SPECIAL_BBS, &item) == false)
        {
            // ������Ȃ�����...
            return (true);
        }
        // ���������A�\�����Ă����ύX����
        moveSelectionTab(item);
        break;

      case MULTIBTN_FEATURE_NEXTSEARCHBBS:
        // ������������
        if (search_NextBBS(((NNshGlobal->NNsiParam)->lastBBS + 1), &item) == false)
        {
            // ������Ȃ�����...
            return (true);
        }
        // ���������A�\�����Ă����ύX����
        moveSelectionTab(item);
        break;

      case MULTIBTN_FEATURE_BEAMURL:
        // URL��Beam����
        beam_URL();
        break;

      case MULTIBTN_FEATURE_UPDATERESNUM:
        // ���X���̌v�Z(�\��)�����{����
        UpdateThreadResponseNumber(convertListIndexToMsgIndex((NNshGlobal->NNsiParam)->selectedTitleItem));

        // ��ʕ\�����X�V����
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                           (NNshGlobal->NNsiParam)->selectedTitleItem,
                           NNSH_STEP_REDRAW);
        break;

      case MULTIBTN_FEATURE_OPENWEBDA:
        // webDA�ŊJ��
        launch_WebBrowser(NNSH_ENABLE);
        break;
        
      case MULTIBTN_FEATURE_SELECT_MACRO_EXEC:
        // �}�N���I�����s...
#ifdef USE_MACRO
        // �}�N�����s
        if (NNsi_ExecuteMacroSelection() == true)
        {
            // ��ʍĕ\��...
            NNsi_RedrawThreadList();
        }
#else
        // "���݃T�|�[�g���Ă��܂���" �\�����s��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif
        break;

      case MULTIBTN_FEATURE_BE_LOGIN:
        // Be@2ch���O�C������
        ProceedBe2chLogin();
        break;

      case MULTIBTN_FEATURE_LIST_GETLOG:
        // �X���ԍ��w��\�񃊃X�g��\������
        ShowReserveGetLogList();
        break;

      default:
        // ���̑�(�������Ȃ�)
        NNsh_DebugMessage(ALTID_WARN, "Unknown FuncID", "", funcID);
        break;
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   select_readonly_directory                                */
/*                                 �@�@  �u�Q�ƃ��O�v�̃f�B���N�g����I��  */
/*-------------------------------------------------------------------------*/
static UInt16 select_readonly_directory(UInt16 orgDir)
{
    Char      *listPtr, area[(MAX_DIRNAME + 1) * MAX_DEPTH + MARGIN], *ptr;
    UInt16     itemCnt, btnId, size;
    DmOpenRef  dirRef;

    // �f�B���N�g���J�E���g�����擾����
    OpenDatabase_NNsh(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, &dirRef);
    GetDBCount_NNsh(dirRef, &itemCnt);
    CloseDatabase_NNsh(dirRef);

    // ���X�g������m�ۗ̈�̐ݒ�
    size    = (itemCnt + 2) * (MAX_LISTLENGTH + MARGIN) + MARGIN;
    listPtr = MEMALLOC_PTR(size);
    if (listPtr == NULL)
    {
        return (orgDir);
    }
    MemSet(listPtr, size, 0x00);
    
    // "�X���ꗗ�쐬��" �̕\��
    Show_BusyForm(MSG_MAKE_DIRECTORY_WAIT);

    // �T�u�f�B���N�g�����̂�(�S��)�擾����
    for (ptr = listPtr, size = 0; size <= itemCnt; size++)
    {        
        MemSet (area, sizeof(area), 0x00);
        area[0] = chrEllipsis;
        area[1] = '/';
        GetSubDirectoryName(size, &area[2]);
        btnId = StrLen(area);
        if (btnId > MAX_LISTLENGTH)
        {
            // �i�[�̈悩�炠�ӂ�o�Ă����ꍇ...
            *ptr = chrEllipsis;
            ptr++;
            StrCopy(ptr, &area[btnId - MAX_LISTLENGTH]);
        }
        else
        {
            StrCopy(ptr, area);
        }
        ptr = ptr + StrLen(ptr) + 1;
    }

    // �f�B���N�g���쐬��������
    Hide_BusyForm(false);

    // �i�f�B���N�g���j�I���E�B���h�E��\������
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, listPtr, 
                                                        (itemCnt + 1), orgDir);
    if (btnId == BTNID_JUMPCANCEL)
    {
        // �L�����Z�����ꂽ�ꍇ�A�A�A�w�肳�ꂽ�f�B���N�g���ԍ�������
        NNshGlobal->jumpSelection = orgDir;
    }

    // �m�ۂ����̈���N���A����
    MEMFREE_PTR(listPtr);
    
    // �I�����ꂽ(�f�B���N�g��)�ԍ�����������
    return (NNshGlobal->jumpSelection);
}


/*-------------------------------------------------------------------------*/
/*   Function :   readonly_dir_selection                                   */
/*                                �@ �u�Q�ƃ��O�v�̊i�[�f�B���N�g���I��  */
/*-------------------------------------------------------------------------*/
static Boolean readonly_dir_selection(void)
{
    UInt16 newDir;

    // �f�B���N�g���̊K�w�\�������{���Ȃ��ꍇ...
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_DIROFFLINE) == 0)
    {
        return (false);
    }

    // �f�B���N�g����I������
    newDir = select_readonly_directory((NNshGlobal->NNsiParam)->readOnlySelection);

    if (newDir != (NNshGlobal->NNsiParam)->readOnlySelection)
    {
        (NNshGlobal->NNsiParam)->readOnlySelection = newDir;

        // �I�����ꂽ�f�B���N�g�����X�V���ꂽ�ꍇ�ɂ́A�X���^�C�ꗗ���X�V����
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
    }
    else
    {
        // �X���ꗗ��ʂ��ĕ\������
        NNsi_RedrawThreadList();
    }
    
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   open_menu                                                */
/*                                                 NNsi�Ǝ��̃��j���[�\��  */
/*-------------------------------------------------------------------------*/
static Boolean open_menu(void)
{
    ControlType *ctlP;
    EventType   *dummyEvent;
    Char      *ptr, listPtr[NNSH_ITEMLEN_JUMPLIST*(NNSH_JUMPSEL_NUMLIST + 1)];
    UInt16    btnId, savedParam, jumpList[NNSH_JUMPSEL_NUMLIST + 1], itemCnt;

    // ���X�g�̍쐬
    MemSet(listPtr, sizeof(listPtr), 0x00);
    MemSet(jumpList, sizeof(jumpList), 0x00);
    ptr = listPtr;
    itemCnt = 0;

    // BBS��I��
#ifdef USE_LOGCHARGE
    if ((NNshGlobal->NNsiParam)->getLogSiteListMode != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_OPENBBS_GETLOG);
        ptr = ptr + sizeof(NNSH_JUMPMSG_OPENBBS_GETLOG);
        jumpList[itemCnt] = NNSH_JUMPSEL_OPENBBS;
        itemCnt++;
    }
    else
#endif  // #ifdef USE_LOGCHARGE
    {
        StrCopy(ptr, NNSH_JUMPMSG_OPENBBS);
        ptr = ptr + sizeof(NNSH_JUMPMSG_OPENBBS);
        jumpList[itemCnt] = NNSH_JUMPSEL_OPENBBS;
        itemCnt++;
    }

    // ���j���[���J��
    if ((NNshGlobal->NNsiParam)->addMenuTitle != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_OPENMENU);
        ptr = ptr + sizeof(NNSH_JUMPMSG_OPENMENU);
        jumpList[itemCnt] = NNSH_JUMPSEL_OPENMENU;
        itemCnt++;
    }

    // ����ؒf
    StrCopy(ptr, NNSH_JUMPMSG_DISCONNECT);
    ptr = ptr + sizeof(NNSH_JUMPMSG_DISCONNECT);
    jumpList[itemCnt] = NNSH_JUMPSEL_DISCONNECT;
    itemCnt++;

    // �V���m�F
    StrCopy(ptr, NNSH_JUMPMSG_OPENNEW);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENNEW);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENNEW;
    itemCnt++;

    // �ꗗ�擾
    StrCopy(ptr, NNSH_JUMPMSG_OPENLIST);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENLIST);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENLIST;
    itemCnt++;

    // �X���擾
    StrCopy(ptr, NNSH_JUMPMSG_OPENGET);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENGET);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENGET;
    itemCnt++;

    // �X���Q��
    StrCopy(ptr, NNSH_JUMPMSG_OPENMES);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENMES);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENMES;
    itemCnt++;

    // �Q�ƃ��O�փR�s�[
    if ((NNshGlobal->NNsiParam)->addMenuCopyMsg != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_COPYMSG);
        ptr = ptr + sizeof(NNSH_JUMPMSG_COPYMSG);
        jumpList[itemCnt] = NNSH_JUMPSEL_COPYMSG;
        itemCnt++;
    }

    // �X�����
    StrCopy(ptr, NNSH_JUMPMSG_OPENINFO);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENINFO);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENINFO;
    itemCnt++;

    // �X���폜
    if ((NNshGlobal->NNsiParam)->addMenuDeleteMsg != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_DELMSG);
        ptr = ptr + sizeof(NNSH_JUMPMSG_DELMSG);
        jumpList[itemCnt] = NNSH_JUMPSEL_DELMSG;
        itemCnt++;
    }

    // �Q�ƃ��O�ꗗ��ʂ�
    StrCopy(ptr, NNSH_JUMPMSG_OPENGETLOGLIST);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENGETLOGLIST);
    jumpList[itemCnt] = NNSH_JUMPSEL_OPENGETLOGLIST;
    itemCnt++;

    // �`�惂�[�h�ύX
    if ((NNshGlobal->NNsiParam)->addMenuGraphTitle != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_GRAPHMODE);
        ptr = ptr + sizeof(NNSH_JUMPMSG_GRAPHMODE);
        jumpList[itemCnt] = NNSH_JUMPSEL_GRAPHMODE;
        itemCnt++;
    }

    // ���ړI�X�C�b�`�P
    if ((NNshGlobal->NNsiParam)->addMenuMultiSW1 != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_MULTISW1);
        ptr = ptr + sizeof(NNSH_JUMPMSG_MULTISW1);
        jumpList[itemCnt] = NNSH_JUMPSEL_MULTISW1;
        itemCnt++;
    }

    // ���ړI�X�C�b�`�Q
    if ((NNshGlobal->NNsiParam)->addMenuMultiSW2 != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_MULTISW2);
        ptr = ptr + sizeof(NNSH_JUMPMSG_MULTISW2);
        jumpList[itemCnt] = NNSH_JUMPSEL_MULTISW2;
        itemCnt++;
    }

    // �f�o�C�X���
    if ((NNshGlobal->NNsiParam)->addMenuDeviceInfo != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_SHOWDEVINFO);
        ptr = ptr + sizeof(NNSH_JUMPMSG_SHOWDEVINFO);
        jumpList[itemCnt] = NNSH_JUMPSEL_SHOWDEVINFO;
        itemCnt++;
    }

    // Dir�I����Dir�ړ�
    if ((NNshGlobal->NNsiParam)->addMenuDirSelect != 0)
    {
        if ((NNshGlobal->NNsiParam)->lastBBS == NNSH_SELBBS_OFFLINE)
        {
            // �Q�ƃ��O�̂Ƃ��̂ݕ\������
            StrCopy(ptr, NNSH_JUMPMSG_DIRSELECTION);
            ptr = ptr + sizeof(NNSH_JUMPMSG_DIRSELECTION);
            jumpList[itemCnt] = NNSH_JUMPSEL_DIRSELECTION;
            itemCnt++;

            StrCopy(ptr, NNSH_JUMPMSG_CHANGELOGDIR);
            ptr = ptr + sizeof(NNSH_JUMPMSG_CHANGELOGDIR);
            jumpList[itemCnt] = NNSH_JUMPSEL_CHANGELOGDIR;
            itemCnt++;
        }
    }

    // NNsi�I��
    if ((NNshGlobal->NNsiParam)->addMenuNNsiEnd != 0)
    {
        StrCopy(ptr, NNSH_JUMPMSG_NNSIEND);
        ptr = ptr + sizeof(NNSH_JUMPMSG_NNSIEND);
        jumpList[itemCnt] = NNSH_JUMPSEL_NNSIEND;
        itemCnt++;
    }

    // ���O�ʒu�ړ�
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        // VFS���g�p����ݒ�̂Ƃ��̂ݕ\������
        StrCopy(ptr, NNSH_JUMPMSG_CHANGELOGLOC);
        ptr = ptr + sizeof(NNSH_JUMPMSG_CHANGELOGLOC);
        jumpList[itemCnt] = NNSH_JUMPSEL_CHANGELOGLOC;
        itemCnt++;
    }

#ifdef USE_BT_CONTROL
    // Bluetooth On/Off
    if ((NNshGlobal->NNsiParam)->addBtOnOff != 0)
    {
            // ���X�g�ɁuBt On/Bt Off�v��ǉ�
            StrCopy(ptr, NNSH_JUMPMSG_BT_ON);
            ptr = ptr + sizeof(NNSH_JUMPMSG_BT_ON);
            jumpList[itemCnt] = NNSH_JUMPSEL_BT_ON;
            itemCnt++;

            StrCopy(ptr, NNSH_JUMPMSG_BT_OFF);
            ptr = ptr + sizeof(NNSH_JUMPMSG_BT_OFF);
            jumpList[itemCnt] = NNSH_JUMPSEL_BT_OFF;
            itemCnt++;
    }
#endif

    // �I���E�B���h�E��\������
    btnId = NNshWinSelectionWindow(FRMID_JUMPMSG, listPtr, itemCnt, 0);
    if (btnId != BTNID_JUMPCANCEL)
    {
        // �I�����X�g�ԍ������j���[�ԍ��̕ϊ�
        btnId = jumpList[NNshGlobal->jumpSelection];
        switch (btnId)
        {
          case NNSH_JUMPSEL_OPENBBS:
            // BBS�ꗗ��I��(�|�b�v�A�b�v)
            (void) ExecuteAction(MULTIBTN_FEATURE_SELECT_BBS);
            break;

          case NNSH_JUMPSEL_OPENMENU:
            // ���j���[���J��
            // (�_�~�[�ō��㕔����penDownEvent�𐶐�)
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType    = penDownEvent;
            dummyEvent->penDown  = true;
            dummyEvent->tapCount = 1;
            dummyEvent->screenX  = 5;
            dummyEvent->screenY  = 5;
            EvtAddEventToQueue(dummyEvent);
            break;

          case NNSH_JUMPSEL_DISCONNECT:
            // ����ؒf
            savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            NNshNet_LineHangup();
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
            break;

          case NNSH_JUMPSEL_OPENNEW:
            // �V���m�F
            (void) ExecuteAction(MULTIBTN_FEATURE_NEWARRIVAL);
            break;

          case NNSH_JUMPSEL_OPENLIST:
            // �ꗗ�X�V
            (void) ExecuteAction(MULTIBTN_FEATURE_GETLIST);
            break;

          case NNSH_JUMPSEL_OPENGET:
            // ���b�Z�[�W�擾(�����擾/�S���擾�͎������f)
            (void) ExecuteAction(MULTIBTN_FEATURE_GET);
            break;

          case NNSH_JUMPSEL_OPENMES:
            // �X���Q��
            (void) ExecuteAction(MULTIBTN_FEATURE_OPEN);
            break;

          case NNSH_JUMPSEL_OPENINFO:
            // �X�����\��
            (void) ExecuteAction(MULTIBTN_FEATURE_INFO);
            break;

          case NNSH_JUMPSEL_COPYMSG:
            // �Q�ƃ��O�փR�s�[
            (void) ExecuteAction(MULTIBTN_FEATURE_COPYGIKO);
            break;

          case NNSH_JUMPSEL_DELMSG:
            // �X���폜
            (void) ExecuteAction(MULTIBTN_FEATURE_DELETE);
            break;

          case NNSH_JUMPSEL_GRAPHMODE:
            // �`�惂�[�h�ύX
            (void) ExecuteAction(MULTIBTN_FEATURE_GRAPHMODE);
            return (true);
            break;

          case NNSH_JUMPSEL_MULTISW1:
            // ���ړI�X�C�b�`�P�����������Ƃɂ���
            ctlP = FRM_GET_ACTIVE_OBJECT_PTR(CHKID_SELECT_GETMODE);
            btnId = CtlGetValue(ctlP);
            btnId = (btnId == 0) ? 1 : 0;
            CtlSetValue(ctlP, btnId);
            CtlHitControl(ctlP);
            break;

          case NNSH_JUMPSEL_MULTISW2:
            // ���ړI�X�C�b�`�Q�����������Ƃɂ���
            ctlP = FRM_GET_ACTIVE_OBJECT_PTR(CHKID_SELECT_MULTIPURPOSE);
            btnId = CtlGetValue(ctlP);
            btnId = (btnId == 0) ? 1 : 0;
            CtlSetValue(ctlP, btnId);
            CtlHitControl(ctlP);
            break;

          case NNSH_JUMPSEL_SHOWDEVINFO:
            // �f�o�C�X���\��
            (void) ExecuteAction(MULTIBTN_FEATURE_DEVICEINFO);
            break;

          case NNSH_JUMPSEL_DIRSELECTION:
            // �f�B���N�g���I��
            (void) ExecuteAction(MULTIBTN_FEATURE_DIRSELECT);
            return (true);
            break;

          case NNSH_JUMPSEL_NNSIEND:
            // NNsi�I��
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType    = appStopEvent;
            EvtAddEventToQueue(dummyEvent);
            break;

          case NNSH_JUMPSEL_CHANGELOGDIR:
            // ���O�i�[�f�B���N�g���̕ύX
            (void) ExecuteAction(MULTIBTN_FEATURE_MOVELOGDIR);
            return (true);
            break;

          case NNSH_JUMPSEL_CHANGELOGLOC:
            // ���O�Ǘ��ꏊ�̕ύX
            (void) ExecuteAction(MULTIBTN_FEATURE_MOVELOGLOC);
            return (true);
            break;
            
#ifdef USE_BT_CONTROL
          case NNSH_JUMPSEL_BT_ON:
            // Bluetooth On
            (void) ExecuteAction(MULTIBTN_FEATURE_BT_ON);
            break;

          case NNSH_JUMPSEL_BT_OFF:
            // Bluetooth Off
            (void) ExecuteAction(MULTIBTN_FEATURE_BT_OFF);
            break;
#endif
          case NNSH_JUMPSEL_OPENGETLOGLIST:
            // �Q�ƃ��O�ꗗ��ʂ��J��
            (void) ExecuteAction(MULTIBTN_FEATURE_OPENGETLOGLIST);
            break;

          default:
            // �������Ȃ�
            break;
        }
    }
    // �X���ꗗ��ʂ��ĕ\������
    NNsi_RedrawThreadList();
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : NNshMain_Close                                             */
/*                                                      �ꗗ��ʂ̃N���[�Y */
/*-------------------------------------------------------------------------*/
static void NNshMain_Close(UInt16 nextFormID)
{
#ifdef ERASE_NOTRECEIVED_THREAD_CLOSEFORM
    UInt16 savedParam;

    if ((NNshGlobal->NNsiParam)->autoDeleteNotYet != 0)
    {
        // �ړ����ɖ��擾�X�����폜����
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        delete_notreceived_message(false);
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }
#endif

    // �X���^�C�g���\�������폜
    ClearMsgTitleInfo();

    // �ʂ̉�ʂ��J��
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(nextFormID);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   copy_to_readOnly                                         */
/*                                      ���O�t�@�C�����Q�ƃ��O�ɃR�s�[���� */
/*-------------------------------------------------------------------------*/
static Boolean copy_to_readOnly(void)
{
    Err                  ret;
    UInt16               selM, selBBS, butID;
    NNshSubjectDatabase  mesInfo;
    NNshBoardDatabase    bbsInfo;
    Char                *fileName, *orgName, *area;

    // �{�@�\�́AVFS��p�ł���|�\������(VFS�ȊO�͏������s��Ȃ�)
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        NNsh_ErrorMessage(ALTID_WARN, MSG_ONLYVFS_FEATURE, "", 0);
        return (false);
    }

    // �o�b�t�@�̊m��
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME);
    if (fileName == NULL)
    {
        return (false);
    }
    orgName  = MEMALLOC_PTR(MAXLENGTH_FILENAME);
    if (orgName == NULL)
    {
        MEMFREE_PTR(fileName);
        return (false);
    }

    // �X�������擾����
    ret = get_message_Info(&mesInfo, &selM, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        //  �X�����擾�̂��߁A�R�s�[�͍s��Ȃ�
        NNsh_InformMessage(ALTID_WARN, MSG_NOTGET_MESSAGE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // �R�s�[��f�B���N�g�����쐬����
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);

    // ���O�x�[�X�f�B���N�g����t������
    area = NULL;
    if (NNshGlobal->logPrefixH != 0)
    {
        area = MemHandleLock(NNshGlobal->logPrefixH);
    }
    if (area == NULL)
    {
        StrCopy(fileName, LOGDATAFILE_PREFIX);
    }
    else
    {
        StrCopy(fileName, area);
        MemHandleUnlock(NNshGlobal->logPrefixH);
    }
    StrCat (fileName, mesInfo.boardNick);
    fileName[StrLen(fileName) - 1] = '\0';
    (void) CreateDir_NNsh(fileName);

    // �R�s�[��t�@�C�������쐬����
    StrCat (fileName, "/");
    StrCat (fileName, mesInfo.threadFileName);

    // �R�s�[���t�@�C�������쐬����
    MemSet (orgName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(orgName, mesInfo.boardNick);
    StrCat (orgName, mesInfo.threadFileName);

    // �t�@�C����{���ɃR�s�[���邩���m�F����
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_COPY,
                                mesInfo.threadTitle, 0);
    if (butID != 0)
    {
        // Cancel��I��(�������Ȃ�)
        ret = ~errNone;
        goto FUNC_END;
    }

    // �R�s�[�̎��{
    NNsh_DebugMessage(ALTID_INFO, "Dest :",   fileName, 0);
    NNsh_DebugMessage(ALTID_INFO, "Source :", orgName, 0);
    Show_BusyForm(MSG_COPY_IN_PROGRESS);

    if (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        // Palm�{�̂���VFS�̎Q�ƃ��O�փR�s�[����
        ret = CopyFile_NNsh(fileName, orgName, NNSH_COPY_PALM_TO_VFS);
    }
    else
    {
        // VFS�̃��O����VFS�̎Q�ƃ��O�փR�s�[����
        ret = CopyFile_NNsh(fileName, orgName, NNSH_COPY_VFS_TO_VFS);
    }
    Hide_BusyForm(false);
    if (ret != errNone)
    {
        // �t�@�C���R�s�[�Ɏ��s�����B
        (void) DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);

        // �R�s�[�Ɏ��s�����|�\������
        NNsh_ErrorMessage(ALTID_ERROR, MSG_FAILURE_COPY, fileName, ret);
        goto FUNC_END;
    }

    // �R�s�[������ʒm����
    NNsh_InformMessage(ALTID_INFO, MSG_INFORM_COPYEND, "", 0);

FUNC_END:
    MEMFREE_PTR(orgName);
    MEMFREE_PTR(fileName);
    if (ret != errNone)
    {
        return (false);
    }

    if ((NNshGlobal->NNsiParam)->copyDelReadOnly != 0)
    {
        // �R�s�[�����������Ƃ��A�R�s�[���t�@�C�����폜����
        return (delete_message());
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   move_log_directory                                       */
/*                                ���O�t�@�C���̊i�[�f�B���N�g����ύX���� */
/*-------------------------------------------------------------------------*/
static Boolean move_log_directory(void)
{
    Err                  ret;
    UInt16               selM, dirIndex;
    Char                 source[BUFSIZE + MARGIN], dest[BUFSIZE + MARGIN];
    Char                *area;
    NNshSubjectDatabase  mesInfo;

    ret = ~errNone;

    // �{�@�\�́AVFS��p�ł���|�\������(VFS�ȊO�͏������s��Ȃ�)
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        NNsh_ErrorMessage(ALTID_WARN, MSG_ONLYVFS_FEATURE, "", 0);
        goto FUNC_END;
    }

    // �{�@�\�́AVFS�ɎQ�ƃ��O��u���ꍇ�ɂ����g���Ȃ��|�\������
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_OFFLOG_VFS_FEATURE, "", 0);
        goto FUNC_END;
    }

    // �X�������擾����
    ret = get_subject_info(&mesInfo, &selM);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_info()", "", ret);
        goto FUNC_END;
    }

    // �w�肳�ꂽ�X�����Q�ƃ��O���m�F����
#ifdef USE_STRSTR
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) != 0)
#else
    if ((mesInfo.boardNick[0]  != '!')||
        (mesInfo.boardNick[1]  != 'G')||
        (mesInfo.boardNick[2]  != 'i')||
        (mesInfo.boardNick[3]  != 'k'))
#endif
    {
        //  �Q�ƃ��O�ł͂Ȃ����߁A�ړ��͍s��Ȃ�
        NNsh_InformMessage(ALTID_WARN, MSG_NOTMOVE_LOGMESSAGE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // ���O�ړ���̎w��
    dirIndex = select_readonly_directory(mesInfo.dirIndex);
    if (dirIndex == mesInfo.dirIndex)
    {
        // ���O�̈ړ��悪�w�肳��Ȃ������A�I������
        ret = ~errNone;
        goto FUNC_END;
    }

    // �Q�ƃ��O�i�[�f�B���N�g���̃x�[�X��t������
    if ((NNshGlobal->logPrefixH == 0)||
         ((area = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
    {
        // �f�t�H���g�̃f�B���N�g�������g�p
        StrCopy(source, LOGDATAFILE_PREFIX);
        StrCopy(dest,   LOGDATAFILE_PREFIX);
    }
    else
    {
        // �J�X�^�}�C�Y���ꂽ�f�B���N�g�������g�p����
        StrCopy(source, area);
        StrCopy(dest,   area);
        MemHandleUnlock(NNshGlobal->logPrefixH);
    }

    // ����ɃT�u�f�B���N�g�����������ĕt������
    GetSubDirectoryName(mesInfo.dirIndex, &source[StrLen(source)]);
    GetSubDirectoryName(dirIndex, &dest[StrLen(dest)]);

    // �t�@�C������t������
    StrCat(source, mesInfo.threadFileName);
    StrCat(dest,   mesInfo.threadFileName);

    // �t�@�C����{���ɃR�s�[���邩���m�F����
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_MOVELOGDIR,
                            mesInfo.threadTitle, 0) != 0)
    {
        // Cancel��I��(�������Ȃ�)
        ret = ~errNone;
        goto FUNC_END;
    }

    // ���O�f�B���N�g���ύX�̎��{
    Show_BusyForm(MSG_MOVEDIR_IN_PROGRESS);

    // ���O�Ǘ����s������
    ret = CopyFile_NNsh(dest, source, NNSH_COPY_VFS_TO_VFS);
    if (ret != errNone)
    {
        // �t�@�C���̃R�s�[�Ɏ��s
        // �R�s�[���悤�Ƃ����t�@�C��(���r���[�ȃt�@�C��)���폜����
        DeleteFile_NNsh(dest, NNSH_VFS_ENABLE);
    }
    else
    {
        // �R�s�[�����O�t�@�C�����폜����
        DeleteFile_NNsh(source, NNSH_VFS_ENABLE);

        // DB�ɋL�^����Ă���i�[�ꏊ���X�V
        mesInfo.dirIndex = dirIndex;
    }

    Hide_BusyForm(false);
    if (ret != errNone)
    {
        // �R�s�[�Ɏ��s�����|�\������
        NNsh_ErrorMessage(ALTID_ERROR, MSG_FAILURE_COPY, mesInfo.threadFileName, ret);
        goto FUNC_END;
    }

    // �R�s�[������ʒm����
    NNsh_InformMessage(ALTID_INFO, MSG_INFORM_CHGLOGDIR_END, dest, 0);

    // �f�[�^���X�V(�I��ԍ�����DB Index�ԍ��֕ϊ����čX�V)
    dirIndex = convertListIndexToMsgIndex(selM);
    ret = update_subject_database(dirIndex, &mesInfo);
    if (ret != errNone)
    {
        // ���肦�Ȃ��͂�...
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()"," ", ret);
    }

FUNC_END:
    // ���O�̃R�s�[�ɐ��������ꍇ�́A���X�g���č쐬���ĕ\��������
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   move_log_location                                        */
/*                                        ���O�t�@�C���̊Ǘ��ꏊ��ύX���� */
/*-------------------------------------------------------------------------*/
static Boolean move_log_location(void)
{
    Err                  ret;
    UInt16               selM;
    NNshSubjectDatabase  mesInfo;

    // �{�@�\�́AVFS��p�ł���|�\������(VFS�ȊO�͏������s��Ȃ�)
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        NNsh_ErrorMessage(ALTID_WARN, MSG_ONLYVFS_FEATURE, "", 0);
        return (false);
    }

    // �X�������擾����
    ret = get_subject_info(&mesInfo, &selM);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_info()", "", ret);
        goto FUNC_END;
    }

    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        //  �X�����擾�̂��߁A�R�s�[�͍s��Ȃ�
        NNsh_InformMessage(ALTID_WARN, MSG_NOTGET_MESSAGE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // �t�@�C����{���ɃR�s�[���邩���m�F����
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_MANAGELOGLOC,
                                mesInfo.threadTitle, 0) != 0)
    {
        // Cancel��I��(�������Ȃ�)
        ret = ~errNone;
        goto FUNC_END;
    }

    // �Ǘ��ꏊ�ύX�̎��{
    Show_BusyForm(MSG_CHANGE_IN_PROGRESS);

    // ���O�Ǘ����s������
    ret = MoveLogMessage((NNshGlobal->NNsiParam)->openMsgIndex);

    Hide_BusyForm(false);
    if (ret != errNone)
    {
        // �R�s�[�Ɏ��s�����|�\������
        NNsh_ErrorMessage(ALTID_ERROR, MSG_FAILURE_COPY, mesInfo.threadFileName, ret);
        goto FUNC_END;
    }

    // �R�s�[������ʒm����
    NNsh_InformMessage(ALTID_INFO, MSG_INFORM_CHANGELOCEND, mesInfo.threadTitle, 0);

FUNC_END:
    if (ret != errNone)
    {
        return (false);
    }

    // ���O�̃R�s�[�ɐ��������ꍇ�́A���X�g���č쐬���ĕ\��������
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   set_SearchTitleString                                    */
/*                                                �X���^�C����������̐ݒ� */
/*-------------------------------------------------------------------------*/
static Boolean set_SearchTitleString(MemHandle *bufH, UInt16 status, UInt16 *mode)
{
    Boolean             ret = false;
    Char               *ptr = NULL;
    UInt16              usage = NNSH_DIALOG_USE_SEARCH;
    NNshSubjectDatabase mesInfo;

    // ����������̈�̎擾
    if (*bufH == 0)
    {
        *bufH = MemHandleNew(BUFSIZE);
        if (*bufH == 0)
        {
            return (false);
        }
        ptr = MemHandleLock(*bufH);
        if (ptr != NULL)
        {
            MemSet(ptr, BUFSIZE, 0x00);
        }
    }
    else
    {
        ptr = MemHandleLock(*bufH);
    }
    if (ptr == NULL)
    {
        // �̈�擾���s
        MemHandleFree(*bufH);
        *bufH = 0;
        return (false);
    }

    // ���݂̃J�[�\��������^�C�g�����𔽉f������
    MemSet(&mesInfo, sizeof(NNshSubjectDatabase), 0x00);
    if (status != 0)
    {
        // �X���^�C�������[�h�̂Ƃ�...
        usage = NNSH_DIALOG_USE_SEARCH_TITLE;
        if (StrLen(ptr) == 0)
        {
            // ���b�Z�[�W�����f�[�^�x�[�X����擾
            if (Get_Subject_Database((status - 1), &mesInfo) == errNone)
            {
                // �^�C�g���������p������o�b�t�@���R�s�[
                StrNCopy(ptr, mesInfo.threadTitle, BUFSIZE);        
            }
        }
    }

    // ��������̓E�B���h�E���J���A�f�[�^���l��
    ret = DataInputDialog(NNSH_INPUTWIN_SEARCH, ptr, BUFSIZE, usage, mode);

    MemHandleUnlock(*bufH);

    // �X���ꗗ��ʂ��ĕ\������
    NNsi_RedrawThreadList();
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   isValidRecordForSearchTitle                              */
/*                  �X���^�C�������������{���Ă��悢�X�����ǂ����̃`�F�b�N */
/*-------------------------------------------------------------------------*/
static Boolean isValidRecordForSearchTitle(UInt16  selBBS, Char *bbsName,
                                           NNshSubjectDatabase  *mesInfo)
{
    // ���X�g����`�F�b�N
    switch (selBBS)
    {
      case NNSH_SELBBS_FAVORITE:
        if ((mesInfo->msgAttribute & NNSH_MSGATTR_FAVOR) < (NNshGlobal->NNsiParam)->displayFavorLevel)
        {
            // �u���C�ɓ���v�ł͂Ȃ�
            return (false);
        }
        break;

      case NNSH_SELBBS_GETALL:
        if (mesInfo->state == NNSH_SUBJSTATUS_NOT_YET)
        {
            // �擾�ςݑS�Ă̂Ƃ��͖��擾�̃X���ł͂Ȃ��ꍇ
            return (false);
        }
        break;

      case NNSH_SELBBS_NOTREAD:
        if ((mesInfo->state != NNSH_SUBJSTATUS_NEW)&&
            (mesInfo->state != NNSH_SUBJSTATUS_UPDATE)&&
            (mesInfo->state != NNSH_SUBJSTATUS_REMAIN))
        {
            // ���ǂ�����X���ł͂Ȃ�
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM1:
        // �����m�F�����{���邩�ǂ����̊m�F(���[�U�ݒ�P)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom1)));
        break;

      case NNSH_SELBBS_CUSTOM2:
        // �����m�F�����{���邩�ǂ����̊m�F(���[�U�ݒ�Q)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom2)));
        break;

      case NNSH_SELBBS_CUSTOM3:
        // �����m�F�����{���邩�ǂ����̊m�F(���[�U�ݒ�R)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom3)));
        break;

      case NNSH_SELBBS_CUSTOM4:
        // �����m�F�����{���邩�ǂ����̊m�F(���[�U�ݒ�S)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom4)));
        break;

      case NNSH_SELBBS_CUSTOM5:
        // �����m�F�����{���邩�ǂ����̊m�F(���[�U�ݒ�T)
        return (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom5)));
        break;

      default:
        if (StrCompare(bbsName, mesInfo->boardNick) != 0)
        {
            // �X���̏�������v���Ȃ�
            return (false);
        }
        break;
    }
    // ���������s����X��������
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   search_NextBBS                                           */
/*                                                              �������� */
/*-------------------------------------------------------------------------*/
static Boolean search_NextBBS(UInt16 start, UInt16 *bbsId)
{
    ListType        *lstP;
    Char            *ptr;
    Char             msg[BUFSIZE];
    Char             match1[BUFSIZE], match2[MAX_BOARDNAME + MARGIN];
    UInt16           selBBS, maxBBS;

    // ���������񂪊m�ۍς݂��m�F����
    if (NNshGlobal->searchBBSH == 0)
    {
        return (false);
    }

    // ���������񂪐ݒ�ς݂̏ꍇ�ɂ́A���̕�������擾����
    ptr = MemHandleLock(NNshGlobal->searchBBSH);
    if (ptr == NULL)
    {
        return (false);
    }
    if (*ptr == '\0')
    {
        MemHandleUnlock(NNshGlobal->searchTitleH);
        return (false);
    }

    // �X�������̎��{(�������\��)
    MemSet (msg, sizeof(msg), 0x00);
    StrCopy(msg, MSG_SEARCHING_TITLE);
    StrCat (msg, ptr);
    Show_BusyForm(msg);

    // ����...
    if (start < NNSH_NOF_SPECIAL_BBS)
    {
        // �����͒ʏ�̂݁B
        start = NNSH_NOF_SPECIAL_BBS;
    }

    // ����召�����ϊ��i���͕�����j
    if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
    {
        (void) StrToLower(match1, ptr);
    }
    else
    {
        (void) StrCopy(match1, ptr);
    }

    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    maxBBS  = LstGetNumberOfItems(lstP);

    // �����i���X�g�̈ꗗ����j�T��
    for (selBBS = start; selBBS < maxBBS; selBBS++)
    {
        // ����召�����ϊ��i�o�͕�����j
        MemSet (match2, sizeof(match2), 0x00);
        if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
        {
            (void) StrToLower(match2, LstGetSelectionText(lstP, selBBS));
        }
        else
        {
            (void) StrCopy(match2, LstGetSelectionText(lstP, selBBS));
        }

        if (StrStr(match2, match1) != NULL)
        {
            // �����񂪌��������I
            Hide_BusyForm(false);
            *bbsId = selBBS;
            MemHandleUnlock(NNshGlobal->searchBBSH);
            return (true);
        }
    }

    // �������\������������
    Hide_BusyForm(false);

    // �����Ɏ��s����(��������Ȃ�����)
    NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, ptr, 0);
    MemHandleUnlock(NNshGlobal->searchBBSH);

    // �X���ꗗ��ʂ��ĕ\������
    NNsi_RedrawThreadList();

    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   search_NextTitle                                         */
/*                                                        �X���^�C�������� */
/*-------------------------------------------------------------------------*/
static Boolean search_NextTitle(Int16 direction)
{
    Err                  ret;
    Char                 msg[BUFSIZE], match1[BUFSIZE], match2[MAX_THREADNAME];
    Char                *ptr;
    UInt16               selMES, selBBS, index, loop, end;
    Int16                step;
    DmOpenRef            dbRef;
    NNshSubjectDatabase  mesInfo;
    NNshBoardDatabase    bbsInfo;

    // �X���^�C�ꗗ���Ȃ��ꍇ�́A�����ɉ���
    if (NNshGlobal->nofTitleItems == 0)
    {
        // ���R�[�h��񂪑��݂��Ȃ�
        NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, MSG_NOTFOUND_OMIT, 0);
        return (false);
    }

    // ���������񂪊m�ۍς݂��m�F����
    if (NNshGlobal->searchTitleH == 0)
    {
        return (false);
    }

    // ���������񂪐ݒ�ς݂̏ꍇ�ɂ́A���̕�������擾����
    ptr = MemHandleLock(NNshGlobal->searchTitleH);
    if (ptr == NULL)
    {
        return (false);
    }
    if (*ptr == '\0')
    {
        MemHandleUnlock(NNshGlobal->searchTitleH);
        return (false);
    }

    // ����(���J�[�\��������ʒu)�̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    // ���[�U�ݒ�̂Ƃ��ɂ́A����ʓr�擾����
    switch (selBBS)
    {
       case NNSH_SELBBS_CUSTOM1:
        // ���[�U�ݒ�P
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom1.boardNick, &bbsInfo);
        break;

       case NNSH_SELBBS_CUSTOM2:
        // ���[�U�ݒ�Q
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom2.boardNick, &bbsInfo);
        break;

       case NNSH_SELBBS_CUSTOM3:
        // ���[�U�ݒ�R
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom3.boardNick, &bbsInfo);
        break;

       case NNSH_SELBBS_CUSTOM4:
        // ���[�U�ݒ�S
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom4.boardNick, &bbsInfo);
        break;

       case NNSH_SELBBS_CUSTOM5:
        // ���[�U�ݒ�T
        ret = Get_BBS_Info((NNshGlobal->NNsiParam)->custom5.boardNick, &bbsInfo);
        break;

       default:
         // ���[�U�ݒ�ȊO
         ret = errNone;
         break;        
    }
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    // �X�������̎��{(�������\��)
    MemSet (msg, sizeof(msg), 0x00);
    StrCopy(msg, MSG_SEARCHING_TITLE);
    StrCat (msg, ptr);
    Show_BusyForm(msg);

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (direction == NNSH_SEARCH_FORWARD)
    {
        // �t�H���[�h�����w��
        GetDBCount_NNsh(dbRef, &end);
        end  = 0;
        step = -1;
    }
    else
    {
        // �o�b�N���[�h�����w��
        GetDBCount_NNsh(dbRef, &end);
        end--;
        step = 1;
    }

    // �啶������������ʂ��Ȃ��ꍇ�ɂ́A�������ɕϊ�
    MemSet(match1, sizeof(match1), 0x00);
    if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
    {
        (void) StrToLower(match1, ptr);
    }
    else
    {
        (void) StrCopy(match1, ptr);
    }

    // �����J�n���R�[�h�ԍ����擾
    index = convertListIndexToMsgIndex(selMES);
    if (ret != errNone)
    {
        // ���R�[�h�̎擾�Ɏ��s�A�������s��\������
        goto FUNC_END;
    }
    // �����J�n���R�[�h�ԍ���"��"���猟�������{����悤��������
    index = index + step;

    // �����̎�����
    for (loop = index; loop != end; loop = loop + step)
    {
        // GetRecord_NNsh()���Ń[���N���A���Ă��邽�ߕs�v
        // MemSet(&tmpDb, sizeof(NNshSubjectDatabase), 0x00);
        GetRecord_NNsh(dbRef, loop, sizeof(NNshSubjectDatabase), &mesInfo);

        // �������r��������ɍ��v�����X�����ǂ����̃`�F�b�N
        if (isValidRecordForSearchTitle(selBBS, bbsInfo.boardNick, &mesInfo) ==
            false)
        {
            // ��r����K�v�̂Ȃ��X���������A���ɂ�����
            continue;
        }

        //  �w�肳�ꂽ������ƃ}�b�`���邩�m�F
        MemSet(match2, sizeof(match2), 0x00);
        if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
        {
            // �X���^�C���������ɕϊ�
            (void) StrToLower(match2, mesInfo.threadTitle);
        }
        else
        {
            (void) StrCopy(match2, mesInfo.threadTitle);
        }

        if (StrStr(match2, match1) != NULL)
        {
            // �����񂪌��������I
            CloseDatabase_NNsh(dbRef);
            Hide_BusyForm(false);

            // �X���ꗗ�̕\���ӏ���ύX
            (NNshGlobal->NNsiParam)->startTitleRec = loop;
            (NNshGlobal->NNsiParam)->endTitleRec   = loop;
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_HALFWAY;
            Update_Thread_List(selBBS, 0, NNSH_STEP_REDRAW);
            MemHandleUnlock(NNshGlobal->searchTitleH);
            return (true);
        }
    }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);

FUNC_END:
    // �����Ɏ��s����
    NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, ptr, 0);
    MemHandleUnlock(NNshGlobal->searchTitleH);

    // �X�����X�g�̍X�V(SELMES����)
    Update_Thread_List(selBBS, (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);

    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_subject_info                                         */
/*                                                   ���b�Z�[�W���̎擾  */
/*-------------------------------------------------------------------------*/
static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR)
{
    Err ret;

    if (NNshGlobal->nofTitleItems == 0)
    {
        // ���R�[�h��񂪑��݂��Ȃ�
        return (~errNone);
    }

    // ���b�Z�[�W���̏�����
    *selTHR                    = (NNshGlobal->NNsiParam)->selectedTitleItem;
    (NNshGlobal->NNsiParam)->openThreadIndex = *selTHR;

    // ���X�g�ԍ�����X��index�ɕϊ�����
    (NNshGlobal->NNsiParam)->openMsgIndex    = convertListIndexToMsgIndex(*selTHR);

    // �X�������擾����
    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, mesInfo);
    if (ret != errNone)
    {
        // ���R�[�h�̎擾�Ɏ��s
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        MemSet(mesInfo, sizeof(NNshSubjectDatabase), 0x00);
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_message_Info                                         */
/*                                                   ���b�Z�[�W���̎擾  */
/*-------------------------------------------------------------------------*/
static Err get_message_Info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR, 
                            NNshBoardDatabase   *bbsInfo,  UInt16 *selBBS)
{
    Err        ret;
    ListType  *lstP;
    UInt16     dummy;

    *selBBS = 0;

    // �X�������擾����
    ret = get_subject_info(mesInfo, selTHR);
    if (ret != errNone)
    {
        return (~errNone);
    }

    // �I������Ă���BBS�̃��X�g�ԍ����擾����
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    *selBBS = LstGetSelection(lstP);

    // �I�����ꂽBBS�̃��X�g�ԍ��ŁABBS�̃��[�h�𔻒f����
    if (*selBBS >= NNSH_SELBBS_OFFLINE)
    {
        // ����BBS�ȊO�̂Ƃ��́ABBS�����擾����
        ret = Get_BBS_Info(*selBBS, bbsInfo);
    }
    else
    {
        // ����́AboardNick���g����BBS�����擾
        ret = Get_BBS_Database(mesInfo->boardNick, bbsInfo, &dummy);
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   display_message                                          */
/*                                                        ���b�Z�[�W�̕\�� */
/*-------------------------------------------------------------------------*/
static Boolean display_message(void)
{
    Err                  ret;
    UInt16               selM, butID, selBBS;
    NNshSubjectDatabase  subjInfo;
    NNshBoardDatabase    bbsInfo;

    // �X�������擾����
    ret = get_message_Info(&subjInfo, &selM, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // �X�����擾�̂Ƃ��A���擾�̎|���x�����A�X�����擾���邩�m�F����B
    if (subjInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_NOT_YET_SUBJECT, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // Cancel��I��(�������Ȃ�)
            return (true);
        }
        // �X�����擾����
        if (get_all_message() != errNone)
        {
            // �X���ꗗ��ʂ��ĕ\������
            NNsi_RedrawThreadList();
            return (false);
        }
        // �X���f�[�^�擾�ł����Ƃ��́A��ʂ�\���ɑJ�ڂ���B
    }

    // BBS��(�Ō��)�Q�Ƃ����Ƃ���ɕύX����(���C�ɓ���Ή�)
    (NNshGlobal->NNsiParam)->lastBBS = selBBS;

    // �Q�Ɖ�ʂ��I�[�v������
    NNshMain_Close(FRMID_MESSAGE);

    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_all_message                                          */
/*                                                     ���b�Z�[�W�S���擾  */
/*-------------------------------------------------------------------------*/
static UInt16 get_all_message(void)
{
    Boolean             err = false;
    Err                 ret;
    Char                *url;
    UInt16              butID, selMES, selBBS, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // URL�̎擾�̈���m�ۂ���
    url = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (url == NULL)
    {
        ret = ~errNone;
        goto FUNC_END;
    }
    MemSet(url, (BIGBUF + MARGIN), 0x00);

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        ret = ~errNone;
        goto FUNC_END;
    }

    // OFFLINE�X�����I�����ꂽ�ꍇ�A�X�V�s��\��
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // �擾��~���a�a�r�̃X�����I�����ꂽ�ꍇ�A�X�V�s��\��
    if (((bbsInfo.state) & (NNSH_BBSSTATE_SUSPEND)) == NNSH_BBSSTATE_SUSPEND)
    {
        NNsh_InformMessage(ALTID_WARN, bbsInfo.boardName,
                           MSG_WARN_SUSPENDBBS, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // �����X�����X�����X�g�̏�ʂɂ��邩�`�F�b�N����
    if (Check_same_thread(selMES, &mesInfo, NULL, &butID) == errNone)
    {
        //  �����X�����������ꍇ�ADB�̕s������h�~���邽�߁A�x����\������
        // �f�[�^�̎擾���s��Ȃ��B
        NNsh_InformMessage(ALTID_WARN, MSG_THREADLIST_ALREADY,
                           mesInfo.threadTitle, 0);
        NNsi_RedrawThreadList();
        ret = ~errNone;
        goto FUNC_END;
    }

    if (mesInfo.state != NNSH_SUBJSTATUS_NOT_YET)
    {
        // ���Ɏ擾�ς݁A���b�Z�[�W���ŏ�����擾���邪�A�{���ɂ������̊m�F
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_GET_ALREADY, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // �L�����Z������
            NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
            NNsi_RedrawThreadList();
            ret = ~errNone;
            goto FUNC_END;
        }
    }
    ////////////  ���b�Z�[�W�S���擾�̏���  ///////////

    //  �擾�\��@�\���L���ł��l�b�g�ɐڑ�����Ă��Ȃ��ꍇ�A
    // �擾�\�񂷂邩�m�F����
    if (((NNshGlobal->NNsiParam)->getReserveFeature != 0)&&(NNshGlobal->netRef == 0))
    {
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                NNSH_MSGINFO_RESERVE, "", 0) == 0)
        {
            // ���X�g�ԍ�����X��index�ɕϊ�����
            index = convertListIndexToMsgIndex(selMES);

            // �V���\�񎞂Ɏ擾�\���ݒ肵�ďI������
            mesInfo.msgAttribute
                     = mesInfo.msgAttribute | NNSH_MSGATTR_GETRESERVE;
            (void) update_subject_database(index, &mesInfo);

            // �X�����X�g�̍X�V(SELMES����)
            err = Update_Thread_List(selBBS,
                             (NNshGlobal->NNsiParam)->selectedTitleItem,
                             NNSH_STEP_REDRAW);
            ret = (errNone + 1);
            goto FUNC_END;
        }
    }

    /** (VFS���)�f�B���N�g�����쐬���� **/
    (void) CreateDir_NNsh(mesInfo.boardNick);

    // ���b�Z�[�W�擾��̍쐬
    (void) CreateThreadURL(NNSH_DISABLE, url, (BIGBUF), &bbsInfo, &mesInfo);
    
    // ���X�g�ԍ�����X��index�ɕϊ�����
    index = convertListIndexToMsgIndex(selMES);


    // ���O�擾����ыL�^����
#ifdef USE_ZLIB
    NNshGlobal->getRetry = 0;
GETALL_MESSAGE:
#endif // #ifdef USE_ZLIB
    ret = Get_AllMessage(url, bbsInfo.boardNick, mesInfo.threadFileName,
                         index, (UInt16) bbsInfo.bbsType);
    if ((ret == errNone)&&((bbsInfo.state&(NNSH_BBSSTATE_LEVELMASK)) != 0))         
    {
        // ��M������I�����A�W���X�����x�����ݒ肳��Ă����ꍇ
        if (Get_Subject_Database(index, &mesInfo) == errNone)
        {
            // BBS���ɐݒ肳�ꂽ�W���X�����x���𔽉f������
            switch ((bbsInfo.state)&(NNSH_BBSSTATE_LEVELMASK))
            {
              case NNSH_BBSSTATE_FAVOR_L1:
                // L���x���ɐݒ�
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L1));
                break;

              case NNSH_BBSSTATE_FAVOR_L2:
                // 1���x���ɐݒ�
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L2));
                break;

              case NNSH_BBSSTATE_FAVOR_L3:
                // 2���x���ɐݒ�
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L3));
                break;

              case NNSH_BBSSTATE_FAVOR_L4:
                // 3���x���ɐݒ�
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L4));
                break;

              case NNSH_BBSSTATE_FAVOR:
                // H���x���ɐݒ�
                mesInfo.msgAttribute
                   = ((mesInfo.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR));
                break;

              default:
                // �������Ȃ�
                break;
            }
            // �X�����DB���X�V����
            (void) update_subject_database(index, &mesInfo);
        }
    }
#ifdef USE_ZLIB
    else if (ret == NNSH_ERRORCODE_NOTACCEPTEDMSG)
    {
        if ((NNshGlobal->useGzip != 0)&&(NNshGlobal->loginDateTime != 0)&&
            (NNshGlobal->getRetry == 0))
        {
            //  �����O�C�����ŁA�T�[�o����̉������b�Z�[�W��
            // �ُ킾�����Ƃ��́A�ߋ����O�q�ɂ�������
            MemSet(url, (BIGBUF + MARGIN), 0x00);

           // ���b�Z�[�W�擾��̍쐬
           (void) CreateThreadURL(NNSH_ENABLE, url, (BIGBUF), &bbsInfo, &mesInfo);

            NNshGlobal->getRetry = 1;
            goto GETALL_MESSAGE;  // �����������...�Ď擾
        }
    }
    NNshGlobal->getRetry = 0;
#endif // #ifdef USE_ZLIB

    // �擾�I���̕\��(DEBUG�I�v�V�����L�����̂�)
    NNsh_DebugMessage(ALTID_INFO, MSG_GET_MESSAGE_END,
                      mesInfo.threadTitle, ret);

    // �X�����X�g�̍X�V(SELMES����)
    err = Update_Thread_List(selBBS,
                             (NNshGlobal->NNsiParam)->selectedTitleItem,
                             NNSH_STEP_REDRAW);

    // NNsi�ݒ�ŁA�u�擾��X����\���v�Ƀ`�F�b�N�������Ă����ꍇ�́A��ʕ\��
    if ((ret == errNone)&&((NNshGlobal->NNsiParam)->openAutomatic != 0))
    {
        // �X����\������B
        display_message();
        ret = ~errNone;
    }
    if (err == false)
    {
        ret = ~errNone;
    }

FUNC_END:
    // �擾�G���[���ɂ́A�G���[��������
    MEMFREE_PTR(url);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_message                                           */
/*                                                     ���b�Z�[�W�����擾  */
/*-------------------------------------------------------------------------*/
static Boolean update_message(void)
{
    Boolean             err;
    Err                 ret;
    Char                url[BUFSIZE];
    UInt16              butID, selBBS, selMES, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // OFFLINE�X�����I�����ꂽ�ꍇ�A�X�V�s��\��
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    // �擾��~���a�a�r�̃X�����I�����ꂽ�ꍇ�A�X�V�s��\��
    if (((bbsInfo.state) & (NNSH_BBSSTATE_SUSPEND)) == NNSH_BBSSTATE_SUSPEND)
    {
        NNsh_InformMessage(ALTID_WARN, bbsInfo.boardName,
                           MSG_WARN_SUSPENDBBS, 0);
        return (false);
    }

    //  Palm�{�̂Ƀ��O�����邪�AVFS���g�p����ݒ�ɂȂ��Ă����ꍇ�ɂ́A
    // �X�V�ł��Ȃ��|�\������
    if (((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)&&
        (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM))||
        ((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)&&
         (mesInfo.msgState == FILEMGR_STATE_OPENED_VFS)))
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE_LOG,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    //  VFS�Ƀ��O�����邪�AVFS���g�p����ݒ�ɂȂ��Ă��Ȃ��ꍇ�ɂ́A
    // �X�V�ł��Ȃ��|�\������
    if (((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)&&
        (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM))||
        ((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)&&
         (mesInfo.msgState == FILEMGR_STATE_OPENED_VFS)))
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE_LOG,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        // �����擾��I���������A�܂��擾���Ă��Ȃ��B(�S�擾���邩�m�F����)
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_NOT_YET_SUBJECT, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // Cancel��I��(�������Ȃ�)
            return (true);
        }
        // �X����(�S��)�擾����
        if (get_all_message() != errNone)
        {
            return (false);
        }
        return (true);
    }

    ////////////  ���b�Z�[�W�����擾�̏���  ///////////

    //  �擾�\��@�\���L���ł��l�b�g�ɐڑ�����Ă��Ȃ��ꍇ�A
    // �擾�\�񂷂邩�m�F����
    if (((NNshGlobal->NNsiParam)->getReserveFeature != 0)&&(NNshGlobal->netRef == 0))
    {
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                NNSH_MSGINFO_RESERVE, "", 0) == 0)
        {
            // ���X�g�ԍ�����X��index�ɕϊ�����
            index = convertListIndexToMsgIndex(selMES);

            // �V���\�񎞂Ɏ擾�\���ݒ肵�ďI������
            mesInfo.msgAttribute
                     = mesInfo.msgAttribute | NNSH_MSGATTR_GETRESERVE;
            (void) update_subject_database(index, &mesInfo);

            // �X�����X�g�̍X�V(SELMES����)
            err = Update_Thread_List(selBBS,
                             (NNshGlobal->NNsiParam)->selectedTitleItem,
                             NNSH_STEP_REDRAW);
            return (true);
        }
    }

    /** (VFS���)�f�B���N�g�����쐬���� **/
    (void) CreateDir_NNsh(mesInfo.boardNick);

    // ���b�Z�[�W�擾��̍쐬
    (void) CreateThreadURL(NNSH_DISABLE, url, sizeof(url), &bbsInfo, &mesInfo);

    // ���X�g�ԍ�����X��index�ɕϊ�����
    index = convertListIndexToMsgIndex(selMES);

    // ���O�̎擾����ыL�^(�ǋL)����
    ret = Get_PartMessage(url, bbsInfo.boardNick, &mesInfo, index, NULL);

    // �擾�I���̕\��(DEBUG�I�v�V�����L�����̂�)
    NNsh_DebugMessage(ALTID_INFO, MSG_GET_PARTMESSAGE_END,
                      mesInfo.threadTitle, ret);

    // �X�����X�g�̍X�V(SELMES)
    err = Update_Thread_List(selBBS, 
                             (NNshGlobal->NNsiParam)->selectedTitleItem,
                             NNSH_STEP_REDRAW);

    // NNsi�ݒ�ŁA�u�擾��X����\���v�Ƀ`�F�b�N�������Ă����ꍇ�́A��ʕ\��
    if ((ret == errNone)&&((NNshGlobal->NNsiParam)->openAutomatic != 0))
    {
        // �X����\������B
        display_message();
        err = true;
    }

    return (err);
}

/*-------------------------------------------------------------------------*/
/*   Function :   modify_title                                             */
/*                                                      �X���^�C�g���̕ύX */
/*-------------------------------------------------------------------------*/
static Boolean modify_title(void)
{
    Err                 ret;
    Char                newName[MAX_THREADNAME], logBuf[BUFSIZE];
    UInt16              selBBS, selTHR, butID, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // �����l�Ƃ��Č��݂̃^�C�g�����R�s�[����
    MemSet(newName, sizeof(newName), 0x00);
    StrNCopy(newName, mesInfo.threadTitle, MAX_THREADNAME);

    // ��������̓E�B���h�E���J���A�f�[�^���l��
    ret = DataInputDialog(NNSH_INPUTWIN_MODIFYTITLE, newName, sizeof(newName),
                          NNSH_DIALOG_USE_MODIFYTITLE, NULL);
    if (ret == true)
    {
        MemSet(logBuf, sizeof(logBuf), 0x00);
        StrCopy(logBuf, MSG_DIALOG_MODIFYTITLE);
        StrCat (logBuf, mesInfo.threadTitle);
        StrCat (logBuf, MSG_DIALOG_MODIFYTITLE2);

        // �{���ɃX���^�C�g����ύX���邩�m�F����
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, logBuf, newName, 0);
        if (butID == 0)
        {
            // �^�C�g�����X�V����
            MemSet  (mesInfo.threadTitle, MAX_THREADNAME, 0x00);
            StrNCopy(mesInfo.threadTitle, newName, MAX_THREADNAME);
            index = convertListIndexToMsgIndex(selTHR);
            ret =  update_subject_database(index, &mesInfo);
            if (ret != errNone)
            {
                NNsh_InformMessage(ALTID_WARN, "Update failure", " ret:", ret);
            }
            else
            {
                Update_Thread_List(selBBS, 
                                   (NNshGlobal->NNsiParam)->selectedTitleItem, 
                                   NNSH_STEP_REDRAW);
            }
            return (true);
        }
    }
    NNsi_RedrawThreadList();
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   launch_WebBrowser                                        */
/*                                                  WebBrowser�ŃX�����J�� */
/*-------------------------------------------------------------------------*/
static Boolean launch_WebBrowser(UInt16 webDA)
{
    Err                 ret;
    Char                logBuf[BIGBUF];
    UInt16              selBBS, selTHR;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // �X�����b�Z�[�W��URL���\�z����
    MemSet (logBuf, sizeof(logBuf), 0x00);
    CreateThreadBrowserURL(NNSH_DISABLE, logBuf, &bbsInfo, &mesInfo);

    // WebBrowser�ŊJ���Ƃ��A�ŐV�T�O�����J���悤�ɂ���
    if ((NNshGlobal->NNsiParam)->browseMesNum != 0)
    {
        switch (bbsInfo.bbsType)
        {
          case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
          case NNSH_BBSTYPE_MACHIBBS:
            StrCat(logBuf, "&LAST=");
            break;

          case NNSH_BBSTYPE_SHITARABA:
            StrCat(logBuf, "&ls=");
            break;

#if 0
          case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
          case NNSH_BBSTYPE_2ch_EUC:
          case NNSH_BBSTYPE_2ch:
          case NNSH_BBSTYPE_OTHERBBS:
          case NNSH_BBSTYPE_OTHERBBS_2:
#endif
          default:
            if ((NNshGlobal->NNsiParam)->useImodeURL != 0)
            {
                StrCat(logBuf, "/n");
                goto OPEN_WEB;
            }
            else
            {
                StrCat(logBuf, "/l");
            }
            break;
        }
        NUMCATI(logBuf, (NNshGlobal->NNsiParam)->browseMesNum);
    }
    else
    {
        // i-mode�p��URL�������ꍇ�AURL�𒲐�����
        if ((NNshGlobal->NNsiParam)->useImodeURL != 0)
        {
            switch (bbsInfo.bbsType)
            {
              case NNSH_BBSTYPE_2ch_EUC:
              case NNSH_BBSTYPE_2ch:
                StrCat(logBuf, "/i");
                break;
#if 0
              case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
              case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
              case NNSH_BBSTYPE_MACHIBBS:
              case NNSH_BBSTYPE_SHITARABA:
              case NNSH_BBSTYPE_OTHERBBS:
              case NNSH_BBSTYPE_OTHERBBS_2:
#endif
              default:
                break;
            }
        }
    }
OPEN_WEB:

    // �\������X��URL���f�o�b�O�\��
    NNsh_DebugMessage(ALTID_INFO, "OPEN URL :", logBuf, 0);

    // webDA�ŊJ���ꍇ...
    if (webDA == NNSH_ENABLE)
    {
        if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
        {
            // �N���b�v�{�[�h��URL���R�s�[��webDA���N������
            ClipboardAddItem(clipboardText, logBuf, StrLen(logBuf));
            (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
        }
        return (false);
    }

    // NetFront���N������(NNsi�I����ɋN��)
    (void) WebBrowserCommand(NNshGlobal->browserCreator,
                             0, 0, NNshGlobal->browserLaunchCode, 
                             logBuf, StrLen(logBuf), NULL);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   beam_URL                                                 */
/*                                                          �X���ԍ���Beam */
/*-------------------------------------------------------------------------*/
static Boolean beam_URL(void)
{
    Err                 ret;
    Char                logBuf[BUFSIZE + BUFSIZE + MARGIN];
    Char                titleBuf[MAX_THREADNAME + MARGIN], *ptr;
    UInt16              selBBS, selTHR;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    if ((mesInfo.bbsType != NNSH_BBSTYPE_2ch)||
        ((mesInfo.boardNick[0]  == '!')&&
        (mesInfo.boardNick[1]  == 'G')&&
        (mesInfo.boardNick[2]  == 'i')))
    {
        // �Q�ƃ��O�A�܂��͂Q�����^�C�v�ȊO�Ȃ�ABeam���Ȃ�
        //   ("���݃T�|�[�g���Ă��܂���" �\�����s��)
        NNsh_InformMessage(ALTID_ERROR, MSG_NOTSUPPORT_BBS, "", 0);
        return (false);
    }

    // "�X���ԍ�"�𐶐�����
    MemSet(logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, NNSI_NNSIEXT_THREADNUMBER_START);
    StrCat (logBuf, mesInfo.boardNick);
    StrCat (logBuf, mesInfo.threadFileName);
    ptr = logBuf;
    while ((*ptr != '\0')&&(*ptr != '.'))
    {
        ptr++;
    }
    *ptr = '\0';  // �X���t�@�C������dat�������폜����
    StrCat (logBuf, NNSI_NNSIEXT_THREADNUMBER_END);

    // ���M�f�[�^�̃^�C�g���𐶐�����
    MemSet(titleBuf, sizeof(titleBuf), 0x00);
    StrCopy(titleBuf, "  ");
    StrCat (titleBuf, mesInfo.threadTitle);

    // �f�[�^��NNsi��Beam����
    SendToExternalDevice(titleBuf, logBuf, SOFT_CREATOR_ID);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkIfNewArrivalIsValid                                 */
/*                                        �V�����b�Z�[�W�擾�ۂ̃`�F�b�N */
/*-------------------------------------------------------------------------*/
static Err checkIfNewArrivalIsValid(UInt16  selBBS, Char *bbsName,
                                    NNshSubjectDatabase  *mesInfo)
{
    if ((mesInfo->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
    {
        // �擾�\�񂵂Ă����X���A���b�Z�[�W�擾����
        // ...���łɃf�B���N�g�����@��(����܂�ǂ��Ȃ�...)
        (void) CreateDir_NNsh(bbsName);
        return (errNone);
    }

    if (mesInfo->state == NNSH_SUBJSTATUS_NOT_YET)
    {
        // ���b�Z�[�W���擾�Ȃ�A�擾�����{���Ȃ�
        return (~errNone);
    }

    if (((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)&&
        (mesInfo->msgState == FILEMGR_STATE_OPENED_STREAM))||
        ((((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)&&
         (mesInfo->msgState == FILEMGR_STATE_OPENED_VFS)))
    {
        //  Palm�{�̂Ƀ��O�����邪�AVFS���g�p����ݒ�ɂȂ��Ă����ꍇ�ɂ́A
        // �擾�����{�ł��Ȃ�
        return (~errNone);
    }

    switch (mesInfo->bbsType)
    {
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
      case NNSH_BBSTYPE_SHITARABA:
        // �`�F�b�N�n�j
        if (mesInfo->maxLoc >= NNSH_MESSAGE_LIMIT)
        {
            // �擾(�ő�)���b�Z�[�W���ɓ��B���Ă���Ȃ�A�擾�����{���Ȃ�
            return (~errNone);
        }
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // �`�F�b�N�n�j
        if (mesInfo->maxLoc >= ((NNshGlobal->NNsiParam)->msgNumLimit - 1))
        {
            // �擾(�ő�)���b�Z�[�W���ɓ��B���Ă���Ȃ�A�擾�����{���Ȃ�
            return (~errNone);
        }
        break;

      case NNSH_BBSTYPE_MACHIBBS:
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        if ((NNshGlobal->NNsiParam)->enableNewArrivalHtml != 0)
        {
            // �܂�BBS/�������@JBBS���V���m�F����ꍇ�ɂ́A�`�F�b�NOK
            break;
        }
        return (~errNone);
        break;

      default:
        // �^�C�v�� "�Q�����˂�" �܂��� "�������" �ȊO�́A���{���Ȃ�
        return (~errNone);
        break;
    }

#ifdef USE_STRSTR
    if (StrCompare(mesInfo->boardNick, OFFLINE_THREAD_NICK) == 0)
#else
    if ((mesInfo->boardNick[0]  == '!')&&
        (mesInfo->boardNick[1]  == 'G')&&
        (mesInfo->boardNick[2]  == 'i'))
#endif
    {
        // �Q�ƃ��O�Ȃ�A�擾�����{���Ȃ�
        return (~errNone);
    }

    // �ʂ̃`�F�b�N
    switch (selBBS)
    {
      case NNSH_SELBBS_FAVORITE:
        if ((mesInfo->msgAttribute & NNSH_MSGATTR_FAVOR) < (NNshGlobal->NNsiParam)->displayFavorLevel)
        {
            // �u���C�ɓ���v�ł͂Ȃ�
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_GETALL:
        // �擾�ςݑS�Ă̂Ƃ��͑S��
        break;

      case NNSH_SELBBS_NOTREAD:
        if ((mesInfo->state != NNSH_SUBJSTATUS_NEW)&&
            (mesInfo->state != NNSH_SUBJSTATUS_UPDATE)&&
            (mesInfo->state != NNSH_SUBJSTATUS_REMAIN))
        {
            // ���ǂ�����X���ł͂Ȃ�
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM1:
        // ���[�U�ݒ�P�̏����`�F�b�N
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom1)) != true)
        {
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM2:
        // ���[�U�ݒ�Q�̏����`�F�b�N
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom2)) != true)
        {
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM3:
        // ���[�U�ݒ�R�̏����`�F�b�N
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom3)) != true)
        {
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM4:
        // ���[�U�ݒ�S�̏����`�F�b�N
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom4)) != true)
        {
            return (~errNone);
        }
        break;

      case NNSH_SELBBS_CUSTOM5:
        // ���[�U�ݒ�T�̏����`�F�b�N
        if (CheckIfCustomTabIsValid(bbsName, mesInfo, &((NNshGlobal->NNsiParam)->custom5)) != true)
        {
            return (~errNone);
        }
        break;

      default:
        if (StrCompare(bbsName, mesInfo->boardNick) != 0)
        {
            // �X���̏������Ⴄ
            return (~errNone);
        }
        break;
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_newarrival_message                                */
/*                                                  �V�����b�Z�[�W�擾���� */
/*-------------------------------------------------------------------------*/
static Boolean update_newarrival_message(UInt16 command)
{
    Err                  ret = ~errNone;
    Boolean              penState, bbsUpdate;
    Int32                waitTime;
    UInt16               selBBS, loop, savedParam, count, x, y;
    UInt16               getBBS, dummy, keepBBS;
    Char                *url, logMsg[MINIBUF], dispMsg[MINIBUF];
    ListType            *lstP;
    ControlType         *ctlP;
    MemHandle            mesH;
    DmOpenRef            dbRef;
    NNshBoardDatabase   *bbsInfo;
    NNshSubjectDatabase *mesInfo, *mesRead;

    // �V���m�F�O�ɂc�`�����{����ݒ肾�����ꍇ...
    if ((NNshGlobal->NNsiParam)->preOnDAnewArrival != 0)
    {
        // DA���N������
        LaunchDA_NNsh(((NNshGlobal->NNsiParam)->prepareDAforNewArrival));
    }

    // ���[�N�̈�̊m��(3��)
    url = MEMALLOC_PTR(BIGBUF + MARGIN);
    if (url == NULL)
    {
        return (false);
    }
    MemSet(url, (BIGBUF + MARGIN), 0x00);

    // �X���Ԏw��擾�Ŏ擾��ۗ��������̂ɂ��āA"�V���m�F"����
    OpenDatabase_NNsh(DBNAME_GETRESERVE, DBVERSION_GETRESERVE, &dbRef);
    if (dbRef != 0)
    {
        // �擾�ۗ��X���̃X���Ԏw��擾���{���́A�m�F���b�Z�[�W��\�������Ȃ�
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

        // �����擾
        GetDBCount_NNsh(dbRef, &count);

        // �S���A�X���Ԏw��擾�����{����
        for (loop = 0; loop < count; loop++)
        {
            // �f�[�^�擾
            MemSet(url, (BIGBUF + MARGIN), 0x00);
            GetRecord_NNsh(dbRef, loop, BUFSIZE, url);

            // �X���Ԏw��擾�����{����
            (void) GetThreadDataFromMesNum(&bbsUpdate, url, BIGBUF, &x);

            // ����Delay���w�肳��Ă����ꍇ�A����҂����Ԃ�����...
            if ((NNshGlobal->NNsiParam)->roundTripDelay != 0)
            {
                waitTime = (NNshGlobal->NNsiParam)->roundTripDelay;
                waitTime = waitTime * (SysTicksPerSecond() / 1000);
                if (waitTime != 0)
                {
                    SysTaskDelay(waitTime);
                }
            }
        }

        // �X���Ԏw��擾�̎擾�ۗ��c�a�̑S���R�[�h���폜����
        while (count != 0)
        {
            (void) DeleteRecordIDX_NNsh(dbRef, (count - 1));
            count--;
        }
        CloseDatabase_NNsh(dbRef);

        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }
    if (command == NNSH_NEWARRIVAL_ONLYMESNUM)
    {
        // �X���Ԏw��擾�̎擾�ۗ��̂ݐV���m�F���郂�[�h�������A�I������
        MEMFREE_PTR(url);
        goto KICK_DA;
    }


    bbsInfo = MEMALLOC_PTR(sizeof(NNshBoardDatabase));
    if (bbsInfo == NULL)
    {
        MEMFREE_PTR(url);
        return (false);
    }
    mesInfo = MEMALLOC_PTR(sizeof(NNshSubjectDatabase));
    if (bbsInfo == NULL)
    {
        MEMFREE_PTR(url);
        MEMFREE_PTR(bbsInfo);
        return (false);
    }

    // �m�ۂ������[�N�̈������������
    MemSet(url,     (BIGBUF + MARGIN),            0x00);
    MemSet(bbsInfo, sizeof(NNshBoardDatabase),   0x00);
    MemSet(mesInfo, sizeof(NNshSubjectDatabase), 0x00);

    // �I������Ă���BBS�̃��X�g�ԍ����擾����
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);
    keepBBS = selBBS;
    switch (selBBS)
    {
      case NNSH_SELBBS_NOTREAD:
        // ���ǂɂ���Ƃ��A�V���m�F����^�u�́A�p�����[�^����擾����
        selBBS = (NNshGlobal->NNsiParam)->newArrivalNotRead;
        dummy  = 1;
        break;

      case NNSH_SELBBS_CUSTOM1:
        selBBS  = (NNshGlobal->NNsiParam)->custom1.newArrival;
        dummy   = 1;
        break;

      case NNSH_SELBBS_CUSTOM2:
        selBBS = (NNshGlobal->NNsiParam)->custom2.newArrival;
        dummy  = 1;
        break;

      case NNSH_SELBBS_CUSTOM3:
        selBBS = (NNshGlobal->NNsiParam)->custom3.newArrival;
        dummy  = 1;
        break;

      case NNSH_SELBBS_CUSTOM4:
        selBBS = (NNshGlobal->NNsiParam)->custom4.newArrival;
        dummy  = 1;
        break;

      case NNSH_SELBBS_CUSTOM5:
        selBBS = (NNshGlobal->NNsiParam)->custom5.newArrival;
        dummy  = 1;
        break;

      default:
        dummy   = 0;
        break;
    }
    if (dummy != 0)
    {
        // BBS�\���^�u��(�������)�ύX����
        LstSetSelection(lstP, selBBS);

        // BBS�\���^�u��ύX����
        (NNshGlobal->NNsiParam)->lastBBS = selBBS;

        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO);
        CtlSetLabel(ctlP, LstGetSelectionText(lstP, selBBS));
    }

    // BBS���̎擾��^�u��I��
    switch (selBBS)
    {
      case NNSH_SELBBS_CUSTOM1:
        // ���[�U�ݒ�P�̔����擾
        getBBS = (NNshGlobal->NNsiParam)->custom1.boardNick;
        break;

      case NNSH_SELBBS_CUSTOM2:
        // ���[�U�ݒ�Q�̔����擾
        getBBS = (NNshGlobal->NNsiParam)->custom2.boardNick;
        break;

      case NNSH_SELBBS_CUSTOM3:
        // ���[�U�ݒ�R�̔����擾
        getBBS = (NNshGlobal->NNsiParam)->custom3.boardNick;
        break;

      case NNSH_SELBBS_CUSTOM4:
        // ���[�U�ݒ�S�̔����擾
        getBBS = (NNshGlobal->NNsiParam)->custom4.boardNick;
        break;

      case NNSH_SELBBS_CUSTOM5:
        // ���[�U�ݒ�T�̔����擾
        getBBS = (NNshGlobal->NNsiParam)->custom5.boardNick;
        break;

      case NNSH_SELBBS_OFFLINE:
        // OFFLINE�X�����I������Ă����ꍇ�́A�Q�ƃ��O�擾�@�\�����{����
        ExecuteAction(MULTIBTN_FEATURE_LOGCHARGE);
        goto FUNC_END;
        break;

      case NNSH_SELBBS_GETALL:
        // �u�擾�ςݑS�āv���I������Ă����Ƃ��ł��A
        // �u�擾�ςݑS�Ăɂ͎Q�ƃ��O�Ȃ��v�Ƀ`�F�b�N�������Ă��Ȃ��Ƃ��́A
        // �Q�ƃ��O�擾�@�\�����s����
        if ((NNshGlobal->NNsiParam)->notListReadOnly == 0)
        {
            ExecuteAction(MULTIBTN_FEATURE_LOGCHARGE);
        }
        getBBS = selBBS;
        break;

      case NNSH_SELBBS_FAVORITE:
      case NNSH_SELBBS_NOTREAD:
      default:
        getBBS = selBBS;
        break;
    }

    // �ʏ�̏ꍇ�ɂ́A�����擾����(����̏ꍇ�͒����擾����)
    if (getBBS > NNSH_SELBBS_OFFLINE)
    {
        ret = Get_BBS_Info(getBBS, bbsInfo);
        if (ret != errNone)
        {
            // BBS���Ɏ擾���s�A�I������
            NNsh_DebugMessage(ALTID_ERROR, MSG_CANNOT_UPDATE, "(BBS data)", 0);
            goto FUNC_END;
        }
    }

    /////////////////  ���b�Z�[�W�����擾�̏���  ////////////////

    // "�X��������"��\������
    Show_BusyForm(MSG_SEARCHING_TITLE);

    // �V�����b�Z�[�W�m�F�������́A�m�F���b�Z�[�W���u�\�����Ȃ��v�ݒ�ɂ���B
    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

    // �S�X�������擾����
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &count);

    // BBS�擾�X�V�t���O���n�e�e�ɂ���
    bbsUpdate = false;

    // �V���m�F������
    for (loop = count; loop > 0; loop--)
    {
        // ���悻�̌����c�茏����\������
        if ((loop % 10) == 0)
        {
            StrCopy(logMsg, MSG_SEARCHING_TITLE MSG_SEARCHING_LEFT);
            NUMCATI(logMsg, loop);
            SetMsg_BusyForm(logMsg);
        }

        // �X���f�[�^���擾����
        GetRecordReadOnly_NNsh(dbRef, (loop - 1), &mesH, (void **) &mesRead);

        // �擾�\��X���A�^�u�����C�ɓ���/�擾�ςݑS��/���ǂ��ǂ����̃`�F�b�N
        if (((mesRead->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)||
            (getBBS < NNSH_SELBBS_OFFLINE))
        {
            //  �`�F�b�N�ɊY�������ꍇ�ɂ́ABBS�����擾����B�������A�擾�ς�
            // BBS����boardNick�ƍ���擾���悤�Ƃ���BBS����boardNick����v
            // �����ꍇ�ɂ́ABBS���͂��łɎ擾�ς݂Ƃ��Ă��̏��𗘗p����
            if (StrCompare(mesRead->boardNick, bbsInfo->boardNick) != 0)
            {
                ret = Get_BBS_Database(mesRead->boardNick, bbsInfo, &dummy);
                if (ret != errNone)
                {
                    // BBS���Ɏ擾���s�A���f�`�F�b�N�ւ�����
                    ReleaseRecordReadOnly_NNsh(dbRef, mesH);
                    goto NEXT_STEP;
                }
                bbsUpdate = true;  // BBS�����X�V�����A�ƃt���O�𗧂Ă�
            }
        }

        // �V�����b�Z�[�W���擾����X�����ǂ����`�F�b�N����
        if (checkIfNewArrivalIsValid(selBBS,bbsInfo->boardNick,mesRead) != 
                                                                       errNone)
        {
            // �V���m�F�����{����X���ł͂Ȃ������A���f�`�F�b�N�ւ�����
            ReleaseRecordReadOnly_NNsh(dbRef, mesH);
            goto NEXT_STEP;
        }

        // �擾��~���a�a�r�̃X�����I�����ꂽ�ꍇ�A�X�V�s��(���֐i��)
        if (((bbsInfo->state) & (NNSH_BBSSTATE_SUSPEND)) == NNSH_BBSSTATE_SUSPEND)
        {
            // ���f�`�F�b�N�ւ�����
            ReleaseRecordReadOnly_NNsh(dbRef, mesH);
            goto NEXT_STEP;
        }

        // �f�[�^�x�[�X����x����
        ReleaseRecordReadOnly_NNsh(dbRef, mesH);
        CloseDatabase_NNsh(dbRef);
        Get_Subject_Database((loop - 1), mesInfo);

        // ���b�Z�[�W�擾��URL�̍쐬
        (void) CreateThreadURL(NNSH_DISABLE, url, BIGBUF, bbsInfo, mesInfo);

        // �\�����郁�b�Z�[�W�̒ǉ�
        MemSet (dispMsg, MINIBUF, 0x00);
        StrCopy(dispMsg, " (");
        NUMCATI(dispMsg, (count - loop) + 1);
        StrCat (dispMsg, "/");
        NUMCATI(dispMsg, count);
        StrCat (dispMsg, ")");

        // �ꎞ�I��BUSY�E�B���h�E����������
        Hide_BusyForm(false);

        // ���O�̎擾����ыL�^(�ǋL)����
        ret = Get_PartMessage(url, bbsInfo->boardNick, mesInfo,
                              (loop - 1), dispMsg);

        // �V���m�F�ō����擾�����s�����ꍇ
        if ((ret == NNSH_ERRORCODE_PARTGET)&&
            ((NNshGlobal->NNsiParam)->autoUpdateGetError != 0))
        {
            // ���b�Z�[�W��ǂ񂾏ꏊ
            x = mesInfo->maxLoc;
            y = mesInfo->currentLoc;

            // �ŏ�����S�Ď擾���Ȃ���
#ifdef USE_ZLIB
            NNshGlobal->getRetry = 0;
GET_ALLMESSAGE:
#endif // #ifdef USE_ZLIB
            ret = Get_AllMessage(url, bbsInfo->boardNick,
                                 mesInfo->threadFileName,
                                 (loop - 1), (UInt16) bbsInfo->bbsType);
            if (ret == errNone)         
            {
                // ��M������I���̂Ƃ��A�ǂݏo�����ꏊ���ȑO�̏ꏊ�ɕ���
                if (Get_Subject_Database((loop - 1), mesInfo) == errNone)
                {
                    mesInfo->maxLoc     = x;
                    mesInfo->currentLoc = y;
                    (void) update_subject_database((loop - 1), mesInfo);
                }
            }
#ifdef USE_ZLIB
            else if ((NNshGlobal->useGzip != 0)&&
                      (NNshGlobal->loginDateTime != 0)&&
                      (ret == NNSH_ERRORCODE_NOTACCEPTEDMSG)&&
                      (NNshGlobal->getRetry == 0))
            {
                //  �����O�C�����ŁA�T�[�o����̉������b�Z�[�W��
                // �ُ킾�����Ƃ��́A�ߋ����O�q�ɂ�������
                MemSet(url, (BIGBUF + MARGIN), 0x00);

                // ���b�Z�[�W�擾��URL�̍쐬
                (void) CreateThreadURL(NNSH_ENABLE, url, BIGBUF, bbsInfo, mesInfo);
                
                NNshGlobal->getRetry = 1;
                goto GET_ALLMESSAGE;  // �����������...
            }
            NNshGlobal->getRetry = 0;
#endif // #ifdef USE_ZLIB
        }

        // BUSY�E�B���h�E�𕜊�������
        Show_BusyForm(MSG_SEARCHING_TITLE);

        // �f�[�^�x�[�X���ēx�J��
        OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);

        //  ���O�擾���������~���ꂽ or �v���I�G���[�������ɂ�
        // �V�����b�Z�[�W�̊m�F�𒆎~����B
        if (ret == ~errNone)
        {
            // �m�F���b�Z�[�W�ȗ��̃p�����[�^�����ɖ߂�
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

            // �����𒆎~����|�A��ʕ\������
            NNsh_InformMessage(ALTID_INFO, 
                               MSG_ABORT_MESSAGE, 
                               MSG_NOTCHECK_RECORDS, loop);
            break;
        }

        //  BBS����(�擾�\��X���̂��߂�)�X�V���Ă����ꍇ
        if (bbsUpdate == true)
        {
            if (getBBS > NNSH_SELBBS_OFFLINE)
            {
                // BBS�����Ď擾���Č���BBS���ɖ߂�
                // (����̓��[�v������擾����̂ōX�V���Ȃ�)
                (void) Get_BBS_Info(getBBS, bbsInfo);
                bbsUpdate = false;
            }
        }

        // ����Delay���w�肳��Ă����ꍇ�A�҂����Ԃ�����...
        if ((NNshGlobal->NNsiParam)->roundTripDelay != 0)
        {
            // �ҋ@����\������...
            SetMsg_BusyForm(MSG_INFO_NEWTHREADWAIT);

            waitTime = SysTicksPerSecond();
            waitTime = (NNshGlobal->NNsiParam)->roundTripDelay * waitTime;
            waitTime = waitTime / 1000;
            if (waitTime != 0)
            {
                SysTaskDelay(waitTime);
            }

            // �������ɕ��A...
            SetMsg_BusyForm(MSG_SEARCHING_TITLE);
        }

NEXT_STEP:
        // �y���̏�Ԃ��E��(��ʃ^�b�v����Ă��邩�m�F����)
        EvtGetPen(&x, &y, &penState);
        if (penState == true)
        {
            // �y�����_�E������Ă�����A���~���邩�m�F���s��
            if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                  MSG_CONFIRM_ABORT_GET, 
                                  MSG_NOTCHECK_RECORDS, loop) == 0)
            {
                // OK�������ꂽ�Abreak����
                break;
            }
        }
    }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);

    // �m�F���b�Z�[�W�ȗ��̃p�����[�^�����ɖ߂�
    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

    // �u�V���m�F�v��ɖ��ǈꗗ��\������
    if ((NNshGlobal->NNsiParam)->autoOpenNotRead != 0)
    {
        // �I������Ă���BBS�̃��X�g�ԍ���ύX����
        keepBBS = NNSH_SELBBS_NOTREAD;
    }

    // BBS�\���^�u��ύX����
    (NNshGlobal->NNsiParam)->lastBBS = keepBBS;
    NNshWinSetPopItems(FrmGetActiveForm(),POPTRID_BBSINFO, LSTID_BBSINFO, keepBBS);

FUNC_END:
    // �֐��̏I��
    MEMFREE_PTR(url);
    MEMFREE_PTR(bbsInfo);
    MEMFREE_PTR(mesInfo);

KICK_DA:
    if ((ret != ~errNone)&&((NNshGlobal->NNsiParam)->use_DAplugin != 0))
    {
        // DA�̃v���O�C��(cMDA)���N������
        (void) LaunchResource_NNsh('DAcc','cMDA','code',1000);
    }

    if ((ret != ~errNone)&&((NNshGlobal->NNsiParam)->startMacroArrivalEnd != 0))
    {
#ifdef USE_MACRO
        // �V���m�F��Ƀ}�N�������s����
        NNsi_ExecuteMacroMain(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);
#endif
    }

    if ((NNshGlobal->NNsiParam)->disconnArrivalEnd != 0)
    {
        // �u�V���m�F�v�I�����ɉ���ؒf
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        NNshNet_LineHangup();
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }

    // ���X�g�����X�V����
    NNsi_FrmDrawForm(FrmGetActiveForm(), true);
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);

    // �u�V���m�F�v��Ƀr�[�v����炵�Ă݂�
    if ((NNshGlobal->NNsiParam)->autoBeep != 0)
    {
        NNsh_BeepSound(sndAlarm);
    }

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_getall_message                                    */
/*                                     ���b�Z�[�W�擾����("�X�V" �{�^���p) */
/*-------------------------------------------------------------------------*/
static Boolean update_getall_message(void)
{
    Err                 ret;
    UInt16              selMES, selBBS;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���b�Z�[�W��Ԃ��擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // ���b�Z�[�W��Ԃɂ���āA�A�N�V������ύX����
    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        // ���擾�Ȃ�S���擾
        (void) get_all_message();
        goto RETR;
    }

    // �擾�ς݂Ȃ獷���擾
    (void) update_message();
RETR:
    // �X���ꗗ��ʂ��ĕ\������
    NNsi_RedrawThreadList();
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   delete_notreceived_message                               */
/*                                                    ����M�X�����폜���� */
/*-------------------------------------------------------------------------*/
Boolean delete_notreceived_message(Boolean update)
{
    Err                   ret;
    UInt16                selMES, selBBS, cnt;
    NNshSubjectDatabase   mesInfo;

    // BBS�ԍ����擾����
    selBBS = (NNshGlobal->NNsiParam)->lastBBS;

    //  �u���ǂ���v�u�擾�ςݑS�āv�^�u�\�����ɖ��擾�ꗗ�폜���ꂽ�ꍇ�́A
    // �S���擾�X��(�^�C�g��)���폜����
    if ((selBBS == NNSH_SELBBS_GETALL)||(selBBS == NNSH_SELBBS_NOTREAD))
    {
        (void) DeleteSubjectList(" ", NNSH_SUBJSTATUS_UNKNOWN, &cnt);
        // ���X�g�����X�V����(selBBS)
        return (Update_Thread_List(selBBS, 0, NNSH_STEP_UPDATE));
        // return (true);
    }

    // ������疢�擾�ꗗ�͍폜�ł��Ȃ��B
    if (selBBS <  NNSH_SELBBS_OFFLINE)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOTDELETE_FAVOR,"", 0);
        return (false);    
    }

    // �X�����̎擾
    ret = get_subject_info(&mesInfo, &selMES);
    if (ret != errNone)
    {
        return (false);
    }

    // ���擾�̃X�����폜����
    (void) DeleteSubjectList(mesInfo.boardNick, NNSH_SUBJSTATUS_NOT_YET, &cnt);

    // �X���������݂̈ʒu��菬�����ꍇ�ɂ́A�����̃X���ɃJ�[�\�������킹��
    if (selMES >= cnt)
    {
        selMES = cnt - 1;    
    }

    if (update == false)
    {
        // ��ʕ\�����X�V���Ȃ��ꍇ
        return (true);
    }

    // ���X�g�����X�V����(selBBS)
    return (Update_Thread_List(selBBS, 0, NNSH_STEP_UPDATE));
}


/*-------------------------------------------------------------------------*/
/*   Function :   newThread_message                                        */
/*                                                           �X�����ď���  */
/*-------------------------------------------------------------------------*/
static Boolean newThread_message(void)
{
#ifdef USE_NEWTHREAD_FEATURE
    Err                ret;
    UInt16             selBBS, btnID;
    NNshBoardDatabase  bbsData;

    // �X�����Ă�������擾����
    selBBS = (NNshGlobal->NNsiParam)->lastBBS;

    // ������疢�擾�ꗗ�͍폜�ł��Ȃ�
    if (selBBS <=  NNSH_SELBBS_OFFLINE)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_NOTAVAIL_BBS, "", 0);
        return (false);    
    }

    // BBS�����擾����
    MemSet(&bbsData, sizeof(bbsData), 0x00);
    ret = Get_BBS_Info(selBBS, &bbsData);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, MSG_NOTAVAIL_BBS, "(Get_BBS_Info())", ret);
        return (false);
    }

    // BBS�^�C�v���Q�����˂�ȊO�̂Ƃ��́A�X�����Ă��Ȃ�
    if (bbsData.bbsType != NNSH_BBSTYPE_2ch)
    {
        //   �u���ݖ����ȃt�B�[�`���[�v��\������
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "(BBS TYPE)", 0);
        return (false);
    }

    // �X����{���ɗ��Ă邩�m�F����
    btnID = NNsh_ErrorMessage(ALTID_CONFIRM, bbsData.boardName, MSG_NEWTHREAD_CONFIRM, 0);
    if (btnID != 0)
    {
        // �X�����ăL�����Z��
        return (false);
    }

    // �X�����ĉ�ʂ��J��
    NNshMain_Close(FRMID_NEW_THREAD);
    
#else
    // �X�����ď��������s���Ȃ��ꍇ...
    //   �u���ݖ����ȃt�B�[�`���[�v��\������
    NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif     
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   delete_message                                           */
/*                                                           �X���폜����  */
/*-------------------------------------------------------------------------*/
Boolean delete_message(void)
{
    Err                 ret;
    UInt16              selMES, selBBS, index;
    NNshSubjectDatabase mesInfo;
    ListType           *lstP;

    // �I������Ă���BBS�̃��X�g�ԍ����擾����
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);

    // ���b�Z�[�W�����擾����
    ret = get_subject_info(&mesInfo, &selMES);
    if (ret != errNone)
    {
        return (false);
    }

    // �X����{���ɍ폜���邩�m�F����
    index = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_DELETE_MSGCONFIRM, 
                                mesInfo.threadTitle, 0);
    if (index != 0)
    {
        // �폜�L�����Z��
        return (false);
    }

    // ���X�g�ԍ�����X��index�ɕϊ�����
    index = convertListIndexToMsgIndex(selMES);

    // �X���폜���C������
    DeleteThreadMessage(index);

    // �ꗗ��ʂ��ĕ`�悷��
    return (Update_Thread_List(selBBS, selMES, NNSH_STEP_REDRAW));
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_subject_txt                                          */
/*                                      �X�����ꗗ(subject.txt)���擾����  */
/*-------------------------------------------------------------------------*/
static Boolean get_subject_txt(void)
{
    Err          ret;
    Boolean      penState;
    UInt16       selBBS, keepBBS, loop, x, y;
    ListType    *lstP;
    ControlType *ctlP;

    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);

    // ���[�U�ݒ�^�u�̂Ƃ��́A�w�肳�ꂽ�^�u�̈ꗗ���擾����
    switch (selBBS)
    {
      case NNSH_SELBBS_CUSTOM1:
        selBBS  = (NNshGlobal->NNsiParam)->custom1.getList;
        keepBBS = NNSH_SELBBS_CUSTOM1;
        break;

      case NNSH_SELBBS_CUSTOM2:
        selBBS  = (NNshGlobal->NNsiParam)->custom2.getList;
        keepBBS = NNSH_SELBBS_CUSTOM2;
        break;
 
      case NNSH_SELBBS_CUSTOM3:
        selBBS  = (NNshGlobal->NNsiParam)->custom3.getList;
        keepBBS = NNSH_SELBBS_CUSTOM3;
        break;
        
      case NNSH_SELBBS_CUSTOM4:
        selBBS  = (NNshGlobal->NNsiParam)->custom4.getList;
        keepBBS = NNSH_SELBBS_CUSTOM4;
        break;

      case NNSH_SELBBS_CUSTOM5:
        selBBS  = (NNshGlobal->NNsiParam)->custom5.getList;
        keepBBS = NNSH_SELBBS_CUSTOM5;
        break;

      default:
        keepBBS = 0;
        break;
    }

    if (keepBBS != 0)
    {
        // BBS�\���^�u��(�������)�ύX����
        LstSetSelection(lstP, selBBS);

        // BBS�\���^�u��ύX����
        (NNshGlobal->NNsiParam)->lastBBS = selBBS;
        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO);
        CtlSetLabel(ctlP, LstGetSelectionText(lstP, selBBS));
    }

    switch (selBBS)
    {
      case NNSH_SELBBS_NOTREAD:
      case NNSH_SELBBS_FAVORITE:
      case NNSH_SELBBS_CUSTOM1:
      case NNSH_SELBBS_CUSTOM2:
      case NNSH_SELBBS_CUSTOM3:
      case NNSH_SELBBS_CUSTOM4:
      case NNSH_SELBBS_CUSTOM5:
        // "���[�U�ݒ�^�u"�A"���C�ɓ���"�A"���ǂ���"�\�����ɂ́A�󂯕t���Ȃ��B
        ret = ~errNone;
        break;
      case NNSH_SELBBS_GETALL:
        // "�擾�ςݑS��"�̎��ɂ́A�S�X���̈ꗗ���擾����
        for (loop = (NNshGlobal->useBBS - 1); loop > NNSH_SELBBS_OFFLINE; loop--)
        {
            // �X���ꗗ���擾����
            (void) NNsh_GetSubjectList(loop);

            // �y���̏�Ԃ��E��(��ʃ^�b�v����Ă��邩�m�F����)
            EvtGetPen(&x, &y, &penState);
            if (penState == true)
            {
                // �y�����_�E������Ă�����A���~���邩�m�F���s��
                if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                      MSG_CONFIRM_ABORT_LIST, 
                                      MSG_NOTCHECK_RECORDS,
                                      (loop - NNSH_SELBBS_OFFLINE)) == 0)
                {
                    // OK�������ꂽ�Abreak����
                    break;
                }
            }
        }
        ret = errNone;
        break;

      case NNSH_SELBBS_OFFLINE:
        // �I�t���C���X���̌������s��
        create_offline_database();
        ret = errNone;
        break;

      default:
        // �X���ꗗ���擾����
        ret = NNsh_GetSubjectList(selBBS);
        break;
    }
    if (keepBBS != 0)
    {
        // BBS�\���^�u��(�������)���ɖ߂�
        LstSetSelection(lstP, keepBBS);

        // BBS�\���^�u��ύX����
        (NNshGlobal->NNsiParam)->lastBBS = keepBBS;
        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(POPTRID_BBSINFO);
        CtlSetLabel(ctlP, LstGetSelectionText(lstP, keepBBS));
    }

    // �X���ꗗ�擾���ɐV���m�F�������Ɏ��{����ꍇ
    if ((NNshGlobal->NNsiParam)->listAndUpdate != 0)
    {
        // �擾�ۗ����̃X���Ԏw��擾�����{����
        (void) update_newarrival_message(NNSH_NEWARRIVAL_ONLYMESNUM);
    }

    // �X���ꗗ��\�����Ȃ���
    if (ret == errNone)
    {
        FrmDrawForm(FrmGetActiveForm());
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        return (true);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   popEvt_ThreadMain                                        */
/*                                          ���X�g�I���C�x���g�������[�`�� */
/*-------------------------------------------------------------------------*/
static Boolean popEvt_ThreadMain(EventType *event)
{
    UInt16 savedParam;
    switch (event->data.popSelect.listID)
    {
      case LSTID_BBSINFO:
        // �i���݌������[�h���N���A����...
        (NNshGlobal->NNsiParam)->searchPickupMode = 0;

        if ((NNshGlobal->NNsiParam)->autoDeleteNotYet != 0)
        {
            // �ړ����ɖ��擾�X�����폜����
            savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
            (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
            delete_notreceived_message(false);
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
        }

        // LSTID_BBSINFO��popSelectEvent�������Ȃ�A�X�������X�V����
        (NNshGlobal->NNsiParam)->lastBBS = event->data.popSelect.selection;
        (void) Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   rctPointEvt_ThreadMain                                   */
/*                                              �X�����^�b�v�����Ƃ��̏��� */
/*-------------------------------------------------------------------------*/
static Boolean rctPointEvt_ThreadMain(EventType *event,
                                      FormType *frm, RectangleType *rect)
{
    Coord  locY;
    UInt16 fontID, nlines, fontHeight, selItem;

    // �I�������A�C�e������肷��
    if ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0)
    {
        fontID = (NNshGlobal->NNsiParam)->currentFont;
    }
    else
    {
        fontID = (NNshGlobal->NNsiParam)->sonyHRFontTitle;
    }
    NNsi_UpdateRectangle(rect, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                         &fontID, &fontHeight, &nlines);

#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        locY = (event->screenY * 2) - rect->topLeft.y;
    }
    else
#endif
    {
#ifdef USE_HIGHDENSITY
        // �𑜓x�ɂ��킹�ĕ`��̈�̍������擾
        switch (NNshGlobal->os5HighDensity)
        {
          case kDensityDouble:
            locY = (event->screenY * 2) - rect->topLeft.y;
            break;

          case kDensityTriple:
            locY = (event->screenY * 3) - rect->topLeft.y;
            break;

          case kDensityQuadruple:
            locY = (event->screenY * 4) - rect->topLeft.y;
            break;

          case kDensityOneAndAHalf:
            locY = (event->screenY * 3 / 2) - rect->topLeft.y;
            break;

          case kDensityLow:
          default:
            locY = event->screenY - rect->topLeft.y;
            break;
        }
#else
        locY = event->screenY - rect->topLeft.y;
#endif // #ifdef USE_HIGHDENSITY
    }
    selItem = locY / fontHeight;

    // �͈͊O�̑I���������ꍇ�A�������Ȃ�
    if (selItem >= NNshGlobal->nofTitleItems)
    {
        return (true);
    }

    if (selItem == (NNshGlobal->NNsiParam)->selectedTitleItem)
    {
        // �_�u���^�b�v�����o�A�X�����J�����Ƃ���
        return (display_message());
    }

    // �I���A�C�e�����ړ�������
    NNsi_WinUpdateList(selItem,
                       (NNshGlobal->NNsiParam)->selectedTitleItem, 
                       NNshGlobal->msgTitleIndex,
                       nlines, rect, fontHeight);
    (NNshGlobal->NNsiParam)->selectedTitleItem = selItem;
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   keyDownEvt_ThreadMain                                    */
/*                                                        �L�[���͎��̏��� */
/*-------------------------------------------------------------------------*/
static Boolean keyDownEvt_ThreadMain(EventType *event)
{
    Boolean      ret = false;
    UInt16       keyCode;
    ControlType *ctlP;


    // 5way navigator�p�L�[�R�[�h�R���o�[�g
    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      case vchrHard1:
      case vchrTapWaveSpecificKeyULeft:
        // �n�[�h�L�[�𐧌䂷��ꍇ�́A�A�A
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // �n�[�h�L�[�P���������Ƃ��̐ݒ�
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.key1)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

      case vchrHard2:
      case vchrTapWaveSpecificKeyURight:
        // �n�[�h�L�[�𐧌䂷��ꍇ�́A�A�A
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // �n�[�h�L�[�Q���������Ƃ��̐ݒ�
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.key2)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

      case vchrHard3:
        // �n�[�h�L�[�𐧌䂷��ꍇ�́A�A�A
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // �n�[�h�L�[�R���������Ƃ��̐ݒ�
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.key3)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

      case vchrHard4:
        // �n�[�h�L�[�𐧌䂷��ꍇ�́A�A�A
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // �n�[�h�L�[�S���������Ƃ��̐ݒ�
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.key4)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

#if 0
      case vchrCapture:
      case vchrVoiceRec:
      case vchrVZ90functionMenu:      
      case vchrGarminRecord:
#endif
      case vchrHard5:
        // CLIE�L���v�`���[�{�^���𐧌䂷��ꍇ�́A�A�A
        if ((NNshGlobal->NNsiParam)->useHardKeyControl != 0)
        {
            // CLIE�L���v�`���[�{�^�����������Ƃ��̐ݒ�
            return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.clieCapture)&(MULTIBTN_FEATURE_MASK)));
        }
        break;

      // �n�[�h�L�[�������������̏���
      case chrLeftArrow:         // �J�[�\���L�[��(5way��)
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.left)&(MULTIBTN_FEATURE_MASK)));
        break;

      // �n�[�h�L�[�E�����������̏���
      case chrRightArrow:         // �J�[�\���L�[�E(5way�E)
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.right)&(MULTIBTN_FEATURE_MASK)));
        break;

      // �n�[�h�L�[�������������̏���
      case vchrPageDown:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.down)&(MULTIBTN_FEATURE_MASK)));
        break;

      // �n�[�h�L�[������������̏���
      case vchrPageUp:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.up)&(MULTIBTN_FEATURE_MASK)));
        break;

      // �u���v(�W���O�_�C������)�����������̏���
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrNextField:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case chrDownArrow:
        ret = ExecuteAction(MULTIBTN_FEATURE_SELECTNEXT);
        break;

      // �u��v(�W���O�_�C������)�����������̏���
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrPrevField:
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case chrUpArrow:
        ret = ExecuteAction(MULTIBTN_FEATURE_SELECTPREV);
        break;

      case chrCapital_U:  // U�L�[�̓���
      case chrSmall_U:
        // �X���̐�������s
        (void) ExecuteAction(MULTIBTN_FEATURE_SORT);
        break;

      case chrCapital_T:  // T�L�[�̓���
      case chrSmall_T:
        // ���X�g�̐擪����\������
        (void) ExecuteAction(MULTIBTN_FEATURE_TOP);
        break;

      case chrCapital_B:    // B�L�[�̓���
      case chrSmall_B:
        // �X���^�C�g���̖����ֈړ�
        (void) ExecuteAction(MULTIBTN_FEATURE_BOTTOM);
        break;

      case chrCapital_K:    // K�L�[�̓���
      case chrSmall_K:
        // �X���̍폜
        (void) ExecuteAction(MULTIBTN_FEATURE_DELETE);
        break;

      case chrCapital_F:    // F�L�[�̓���
      case chrSmall_F:
        // �X���^�C�g������
        (void) ExecuteAction(MULTIBTN_FEATURE_SEARCH);
        break;

      case chrCapital_E:    // E�L�[�̓���
      case chrSmall_E:
        // �X���̖��擾�ꗗ�폜
        (void) ExecuteAction(MULTIBTN_FEATURE_DELNOTGET);
        break;

      case chrCapital_D:    // D�L�[�̓���
      case chrSmall_D:
        // �`�惂�[�h�ύX
        (void) ExecuteAction(MULTIBTN_FEATURE_GRAPHMODE);
        break;

      // �W���O�_�C����������(��)�̏���(1�y�[�W��)
      case vchrJogPushedUp:
        // �ꎞ��Ԃ�(��]����)�ݒ肷��
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGUP;
#endif
        /** not break!!  **/
      case chrCapital_P:         // P�L�[�̓���
      case chrSmall_P:           // p�L�[�̓���
        // �O�y�[�W�̕\�� 
        (void) ExecuteAction(MULTIBTN_FEATURE_PREVPAGE);
        break;

      // �W���O�_�C����������(��)�̏���(1�y�[�W��)
      case vchrJogPushedDown:
        // �ꎞ��Ԃ�(��]����)�ݒ肷��
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGDOWN;
#endif
        /** not break!!  **/

      case chrCapital_N:      // N�L�[�̓���
      case chrSmall_N:        // n�L�[�̓���
        // ���y�[�W�̕\�� 
        (void) ExecuteAction(MULTIBTN_FEATURE_NEXTPAGE);
        break;

      // JOG Push�I�����̏���(���X�g�I��)
      case vchrJogRelease:
#ifdef USE_CLIE
        if (NNshGlobal->tempStatus != NNSH_TEMPTYPE_CLEARSTATE)
        {
            // �ꎞ��Ԃ��N���A����(�����܂킵�����Ƃ�)
            NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
            break;
        }
#endif
        /*****  not break!! (���X�g�I�������{����)  *****/

      // HandEra JOG Push�I��/Enter�L�[���͎��̏���(���X�g�I��)
      case chrCarriageReturn:
      case chrLineFeed:
      case vchrRockerCenter:
      case vchrHardRockerCenter:
      case vchrThumbWheelPush:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.jogPush)&(MULTIBTN_FEATURE_MASK)));
        break;

      // �o�b�N�{�^��/ESC�L�[/BS�L�[�̏���
      case vchrJogBack:
      case chrBackspace:
      case chrEscape:
      case vchrThumbWheelBack:
      case vchrGarminEscape:
        return (ExecuteAction(((NNshGlobal->NNsiParam)->ttlFtr.jogBack)&(MULTIBTN_FEATURE_MASK)));
        break;

      // �X�y�[�X�L�[����
      case chrSpace:
      case chrSJISIdeographicSpace:
        // �V��MSG�m�F
        return (ExecuteAction(MULTIBTN_FEATURE_NEWARRIVAL));
        break;

      case chrCapital_L:
      case chrSmall_L:
        // �ꗗ�擾
        return (ExecuteAction(MULTIBTN_FEATURE_GETLIST));
        break;

      case chrCapital_A:
      case chrSmall_A:
        // A (�l�b�g���[�N�ݒ���J��)
        return (ExecuteAction(MULTIBTN_FEATURE_NETWORK));
        break;

      case chrCapital_J:
      case chrSmall_J:
        // �X���Ԏw��̃��b�Z�[�W�擾
        return (ExecuteAction(MULTIBTN_FEATURE_MESNUM));
        break;

      // @�L�[����
      case chrCommercialAt:
        // BBS�ꗗ�擾
        return (ExecuteAction(MULTIBTN_FEATURE_GETBBS));
        break;

     case chrHyphenMinus:
        // �n�C�t���L�[(BBS�ꗗ��I��(�|�b�v�A�b�v))
        return (ExecuteAction(MULTIBTN_FEATURE_SELECT_BBS));
        break;

      // Q�L�[����
      case chrCapital_Q:
      case chrSmall_Q:
        // ����ؒf
        return (ExecuteAction(MULTIBTN_FEATURE_DISCONN));
        break;

      // C�L�[����
      case chrCapital_C:
      case chrSmall_C:
        // �f�o�C�X���
        return (ExecuteAction(MULTIBTN_FEATURE_DEVICEINFO));
        break;

      // O�L�[����
      case chrCapital_O:
      case chrSmall_O:
        //URL�擾
        return (ExecuteAction(MULTIBTN_FEATURE_OPENURL));
        break;

      // G�L�[����
      case chrCapital_G:
      case chrSmall_G:
        // �f�B���N�g���I��(chdir)
        return (ExecuteAction(MULTIBTN_FEATURE_DIRSELECT));
        break;

      case chrDigitNine:
        // �X�L�[�i���ړI�X�C�b�`�P�̐ݒ�X�V�j
        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(CHKID_SELECT_GETMODE);
        keyCode = CtlGetValue(ctlP);
        keyCode = (keyCode == 0) ? 1 : 0;
        CtlSetValue(ctlP, keyCode);
        CtlHitControl(ctlP);
        break;

      case chrDigitZero:
        // �O�L�[�i���ړI�X�C�b�`�Q�̐ݒ�X�V�j
        ctlP = FRM_GET_ACTIVE_OBJECT_PTR(CHKID_SELECT_MULTIPURPOSE);
        keyCode = CtlGetValue(ctlP);
        keyCode = (keyCode == 0) ? 1 : 0;
        CtlSetValue(ctlP, keyCode);
        CtlHitControl(ctlP);
        break;

      case chrDigitOne:
        // ���ړI�{�^���P
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn1Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitTwo:
        // ���ړI�{�^���Q
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn2Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitThree:
        // ���ړI�{�^���R
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn3Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitFour:
        // ���ړI�{�^���S
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn4Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitFive:
        // ���ړI�{�^���T
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn5Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrDigitSix:
        // ���ړI�{�^���U
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn6Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case chrComma:
        // �J���}�L�[�i'���ǂ���'�ֈړ��j
        return (ExecuteAction(MULTIBTN_FEATURE_TO_NOTREAD));
        break;

      case chrFullStop:
        // �s���I�h�L�[�i'�擾�ςݑS��'�ֈړ��j
        return (ExecuteAction(MULTIBTN_FEATURE_TO_GETALL));
        break;

      case chrColon:
        // �R�����L�[�i'�Q�ƃ��O'�ֈړ��j
        return (ExecuteAction(MULTIBTN_FEATURE_TO_OFFLINE));
        break;

      case chrSemicolon:
        // �Z�~�R�����L�[�i'���C�ɓ���'�ֈړ��j
        return (ExecuteAction(MULTIBTN_FEATURE_TO_FAVOR));
        break;

      case chrDigitSeven:
        // �L�[7 (Bt ON)
        return (ExecuteAction(MULTIBTN_FEATURE_BT_ON));
        break;

      case chrDigitEight:
        // �L�[8 (Bt Off)
        return (ExecuteAction(MULTIBTN_FEATURE_BT_OFF));
        break;

      case chrExclamationMark:
        // NNsi�ݒ�-1
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET1));
        break;

      case chrQuotationMark:
        // NNsi�ݒ�-2
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET2));
        break;

      case chrNumberSign:
        // NNsi�ݒ�-3
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET3));
        break;

      case chrDollarSign:
        // NNsi�ݒ�-4
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET4));
        break;

      case chrPercentSign:
        // NNsi�ݒ�-5
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET5));
        break;

      case chrAmpersand:
        // NNsi�ݒ�-6
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET6));
        break;

      case chrApostrophe:
        // NNsi�ݒ�-7
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET7));
        break;

      case chrLeftParenthesis:
        // NNsi�ݒ�-8
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET8));
        break;

      case chrRightParenthesis:
        // NNsi�ݒ�-9
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET9));
        break;

      case chrQuestionMark:
        // NNsi�ݒ��ʂ��J��
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET));
        break;

      case chrEqualsSign:
        // ���[�U�ݒ�P���J��
        return (ExecuteAction(MULTIBTN_FEATURE_USER1TAB));
        break;

      case chrCapital_S:
      case chrSmall_S:
        // �}�N�����s
        return (ExecuteAction(MULTIBTN_FEATURE_MACRO_EXEC));
        break;

      case chrCapital_I:
      case chrSmall_I:
        // �X�����
        return (ExecuteAction(MULTIBTN_FEATURE_INFO));
        break;

      case chrCapital_X:
      case chrSmall_X:
        // �I�����j���[
        return (ExecuteAction(MULTIBTN_FEATURE_SELMENU));
        break;

      case chrCapital_W:
      case chrSmall_W:
        // �}�N�����ʕ\��
        return (ExecuteAction(MULTIBTN_FEATURE_MACRO_LOG));
        break;

      case chrCapital_V:
      case chrSmall_V:
        // �Q�ƃ��O�擾�ݒ�ꗗ���J��
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENGETLOGLIST);
        break;

      case chrCapital_Y:
      case chrSmall_Y:
        // �w��X����webDA�ŊJ��
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENWEBDA);
        break;

#ifdef USE_PIN_DIA
      case vchrTT3Short:
      case vchrTT3Long:
        // ��ʃT�C�Y�ύX�̎w��
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            if (HandEraResizeForm(FrmGetActiveForm()) == true)
            {
                FrmDrawForm(FrmGetActiveForm());
                Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                                   (NNshGlobal->NNsiParam)->selectedTitleItem,
                                   NNSH_STEP_REDRAW);
            }
        }
        break;
#endif

      case chrCapital_R:
      case chrSmall_R:
      case chrCapital_H:
      case chrSmall_H:
      default:
        // ���̑�
        NNsh_DebugMessage(ALTID_INFO, "Enters", "", keyCode);
        return (false);
        break;
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlSelEvt_ThreadMain                                     */
/*                                                      �{�^���������̏��� */
/*-------------------------------------------------------------------------*/
static Boolean ctlSelEvt_ThreadMain(EventType *event)
{
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_BUTTON6:
        // ���ړI�{�^���U
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn6Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON1:
        // ���ړI�{�^���P
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn1Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON2:
        // ���ړI�{�^���Q
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn2Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON3:
        // ���ړI�{�^���R
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn3Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON5:
        // ���ړI�{�^���T
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn5Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case BTNID_BUTTON4:
        // ���ړI�{�^���S
        return (ExecuteAction(((NNshGlobal->NNsiParam)->multiBtn4Feature)&(MULTIBTN_FEATURE_MASK)));
        break;

      case CHKID_SELECT_GETMODE:
        // ���ړI�X�C�b�`�P�̐ݒ�X�V
        if (updateMultiPurposeSwitch((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1,
                                     CHKID_SELECT_GETMODE) == true)
        {
            // ��ʍX�V���K�v�ȃp�����[�^���X�V���ꂽ�ꍇ
            // �X���ꗗ�̕\�����X�V
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                               (NNshGlobal->NNsiParam)->selectedTitleItem,
                               NNSH_STEP_REDRAW);
        }
        break;

      case CHKID_SELECT_MULTIPURPOSE:
        // ���ړI�X�C�b�`�Q�̐ݒ�X�V
        if (updateMultiPurposeSwitch((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2,
                                     CHKID_SELECT_MULTIPURPOSE) == true)
        {
            // ��ʍX�V���K�v�ȃp�����[�^���X�V���ꂽ�ꍇ
            // �X���ꗗ�̕\�����X�V
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                               (NNshGlobal->NNsiParam)->selectedTitleItem,
                               NNSH_STEP_REDRAW);
        }
        break;
        
      case SELTRID_GETLOG_URL_SELECT:
#ifdef USE_LOGCHARGE
        // �\���^�C�g���̍X�V
        changeGetLogUrlSite(false);

        // �X���ꗗ�̕\�����X�V
        Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_TO_TOP);
#endif  // #ifdef USE_LOGCHARGE
        break;
  
      default:
        // ���̑�
        return (false);
        break;
    }

    // �X���ꗗ��ʂ��ĕ\������
    NNsi_RedrawThreadList();
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlRepEvt_ThreadMain                                     */
/*                                              ���s�[�g�{�^���������̏��� */
/*-------------------------------------------------------------------------*/
static Boolean ctlRepEvt_ThreadMain(EventType *event)
{
    switch (event->data.ctlRepeat.controlID)
    {
      case BTNID_LISTPREV:
        // �P�y�[�W��ֈړ�
        return (ExecuteAction(MULTIBTN_FEATURE_PREVPAGE));
        break;

      case BTNID_LISTNEXT:
        // �P�y�[�W���ֈړ�
        return (ExecuteAction(MULTIBTN_FEATURE_NEXTPAGE));
        break;

      case BTNID_SEARCHLIST:
        // �������[�h�}�[�J
        return (false);
        break;

      default:
        // ���̑�
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Handler_ThreadMain                                       */
/*                                          �X���Ǘ���ʗp�C�x���g�n���h�� */
/*=========================================================================*/
Boolean Handler_ThreadMain(EventType *event)
{
    Boolean        ret;
    FormType      *frm;
    RectangleType  dimF;

#ifdef USE_MACRO
    LocalID        dbId;
    if (NNshGlobal->tempStatus == NNSH_MACROCMD_OPERATE)
    {
        if ((NNshGlobal->NNsiParam)->debugMessageON == 0)
        {
            // �N�����Ƀ}�N����1�x���s����ꍇ�A�A�A
            // (�������A�f�o�b�O���[�h���ɂ͎��s���Ȃ�...)
            NNsi_ExecuteMacroMain(DBNAME_RUNONCEMACRO, DBVERSION_MACROSCRIPT, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);
            NNsi_RedrawThreadList();

            // �������s�̃f�[�^�x�[�X���폜����
            dbId = DmFindDatabase(0, DBNAME_RUNONCEMACRO);
            DmDeleteDatabase(0, dbId);

            NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
            return (false);
        }
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
    }

    if (((NNshGlobal->NNsiParam)->autostartMacro != 0)&&
        (NNshGlobal->tempStatus == NNSH_MACROCMD_EXECUTE))
    {
        // �}�N���̎������s���w������Ă����ꍇ�ɂ́A�}�N�������s����
        ExecuteAction(MULTIBTN_FEATURE_MACRO_EXEC);
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
        return (false);
    }
#endif

    ret = false;
    switch (event->eType)
    {
      case keyDownEvent:
        (void) keyDownEvt_ThreadMain(event);
        return (true);
        break;

      case fldEnterEvent:
        break;

      case ctlSelectEvent:
        return (ctlSelEvt_ThreadMain(event));
        break;

      case ctlRepeatEvent:
        return (ctlRepEvt_ThreadMain(event));
        break;

      case frmOpenEvent:
        break;

      case popSelectEvent:
        return (popEvt_ThreadMain(event));
        break;

      case lstSelectEvent:
        break;

      case lstEnterEvent:
        break;

      case lstExitEvent:
        break;

      case menuEvent:
        (void) menuEvt_ThreadMain(event);
        return (true);
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent :
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                               (NNshGlobal->NNsiParam)->selectedTitleItem,
                               NNSH_STEP_REDRAW);
        }
        break;
#endif  // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                FrmDrawForm(frm);        
                Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                                   (NNshGlobal->NNsiParam)->selectedTitleItem,
                                   NNSH_STEP_REDRAW);
            }
        }
        break;
#endif

      case menuOpenEvent:
        break;

      case sclRepeatEvent:
        break;

      case sclExitEvent:
        break;

      case penDownEvent:
        frm = FrmGetActiveForm();
        FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_TAPPED),&dimF);
        if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
        {
            // Gadget�����^�b�v����(== �X����I������)
            return (rctPointEvt_ThreadMain(event, frm, &dimF));
        }
        FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_LIST),&dimF);
        if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
        {
            // Gadget�����^�b�v����(== �X���̉E�[��������)

            // ���y�[�W�{�^���������ꂽ���`�F�b�N
            FrmGetObjectBounds(frm, 
                               FrmGetObjectIndex(frm, BTNID_LISTNEXT),&dimF);
            if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
            {
                // ���y�[�W�{�^���������ꂽ���Ƃɂ���
                return (ExecuteAction(MULTIBTN_FEATURE_NEXTPAGE));
            }

            // �O�y�[�W�{�^���������ꂽ���`�F�b�N
            FrmGetObjectBounds(frm, 
                               FrmGetObjectIndex(frm, BTNID_LISTPREV),&dimF);
            if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
            {
                // �O�y�[�W�{�^���������ꂽ���Ƃɂ���
                return (ExecuteAction(MULTIBTN_FEATURE_PREVPAGE));
            }

            // �X����I���������Ƃɂ���
            FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_LIST),&dimF);
            return (rctPointEvt_ThreadMain(event, frm, &dimF));
        }
        return (false);
        break;

      default:
        break;
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   OpenForm_ThreadMain                                      */
/*                                         �X���Ǘ���ʂ��J�����Ƃ��̏���  */
/*=========================================================================*/
void OpenForm_ThreadMain(FormType *frm)
{
    Err          ret;
    UInt16       lp;
    ControlType *ctlP;

    // ����I�[�v�����A�I�t���C���X���̌���
    if (((NNshGlobal->NNsiParam)->offChkLaunch != 0)&&(NNshGlobal->bbsTitles == NULL))
    {
        create_offline_database();
    }

    // ���ړI�X�C�b�`�P�̏����ݒ����ʂɔ��f
    setMultiPurposeSwitch(frm, (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1,
                          CHKID_SELECT_GETMODE);

    // ���ړI�X�C�b�`�Q�̏����ݒ����ʂɔ��f
    setMultiPurposeSwitch(frm, (NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2,
                          CHKID_SELECT_MULTIPURPOSE);

    // ���ړI�{�^���P�̃L���v�V�����ݒ�
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON1));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn1Caption);

    // ���ړI�{�^���Q�̃L���v�V�����ݒ�
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON2));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn2Caption);

    // ���ړI�{�^���R�̃L���v�V�����ݒ�
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON3));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn3Caption);

    // ���ړI�{�^���S�̃L���v�V�����ݒ�
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON4));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn4Caption);

    // ���ړI�{�^���T�̃L���v�V�����ݒ�
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON5));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn5Caption);

    // ���ړI�{�^���U�̃L���v�V�����ݒ�
    ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_BUTTON6));
    CtlSetLabel(ctlP, (NNshGlobal->NNsiParam)->multiBtn6Caption);

    // BBS�ꗗ�̍쐬(�ꗗ���Ȃ���΁ABBS�ꗗ���擾����悤����)
    ret = Create_BBS_INDEX(&(NNshGlobal->bbsTitles), &lp);
    if (ret != errNone)
    {
        // �쐬���s�A�t�H�[�����͍X�V���Ȃ����A��U��ʂ�`�悷��
        Update_Thread_List(0, 0, NNSH_STEP_REDRAW);

        // �Ƃ肠�����A�J����ʂ̐ݒ���s��
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_THREAD;
        NNshGlobal->backFormId = FRMID_THREAD;
        return;
    }

    // (NNshGlobal->NNsiParam)->lastBBS(�O��I�����Ă���BBS��)���ςȂƂ�(�ꗗ�̐擪�ɂ���)
    if (lp <= (NNshGlobal->NNsiParam)->lastBBS)
    {
        (NNshGlobal->NNsiParam)->lastBBS = 0;
    }

    // BBS�ꗗ�̃��X�g��Ԃ𔽉f����B
    NNshWinSetListItems(frm, LSTID_BBSINFO, NNshGlobal->bbsTitles, lp,
                        (NNshGlobal->NNsiParam)->lastBBS, &(NNshGlobal->bbsTitleH), NULL);

    // �|�b�v�A�b�v�g���K�̃��x����(���X�g��Ԃɍ��킹)�X�V����
    NNshWinSetPopItems(frm, POPTRID_BBSINFO, LSTID_BBSINFO, (NNshGlobal->NNsiParam)->lastBBS);

    // �X���ꗗ�̕\�����X�V
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                       (NNshGlobal->NNsiParam)->selectedTitleItem,
                       NNSH_STEP_REDRAW);

    // �J�����t�H�[��ID��ݒ肷��(������ݒ蒆�ɂ́AlastFrmID�͕ύX���Ȃ�)
    if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
    {
        // ������ݒ蒆�Ȃ̂ŁA�������Ȃ�
    }
    else
    {
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_THREAD;
    }
    NNshGlobal->backFormId = FRMID_THREAD;
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   menuEvt_ThreadMain                                       */
/*                                      �X���Ǘ���ʗp���j���[�������[�`�� */
/*-------------------------------------------------------------------------*/
static Boolean menuEvt_ThreadMain(EventType *event)
{
    Boolean err = false;

    switch (event->data.menu.itemID)
    {
      case MNUID_SHOW_DEVICEINFO:
        // �f�o�C�X�̌��݂̏�ԕ\��
        (void) ExecuteAction(MULTIBTN_FEATURE_DEVICEINFO);
        break;

      case MNUID_SHOW_VERSION:
        // �o�[�W�����̕\��
        (void) ExecuteAction(MULTIBTN_FEATURE_VERSION);
        break;

      case MNUID_CONFIG_NNSH:
        // NNsi�ݒ��ʂ��J��
        return (ExecuteAction(MULTIBTN_FEATURE_NNSHSET));
        break;

      case MNUID_NET_DISCONN:
        // ����ؒf
        return (ExecuteAction(MULTIBTN_FEATURE_DISCONN));
        break;

      case MNUID_UPDATE_CHECK:
        // �V�����b�Z�[�W�m�F
        return (ExecuteAction(MULTIBTN_FEATURE_NEWARRIVAL));
        break;

      case MNUID_UPDATE_BBS:
        // ���X�V(�擾)
        return (ExecuteAction(MULTIBTN_FEATURE_GETBBS));
        break;

      case MNUID_SELECT_BBS:
        // �g�p�I����ʂ��J��
        return (ExecuteAction(MULTIBTN_FEATURE_USEBBS));
        break;

      case MNUID_GET_NEWMESSAGE:
        // �X���ꗗ�擾(SUBJECT.TXT�擾)
        return (ExecuteAction(MULTIBTN_FEATURE_GETLIST));
        break;

      case MNUID_INFO_MESSAGE:
        // �X�����
        return (ExecuteAction(MULTIBTN_FEATURE_INFO));
        break;

      case MNUID_MODIFY_TITLE:
        // �X���^�C�g���ύX
        return (ExecuteAction(MULTIBTN_FEATURE_MODTITLE));
        break;

      case MNUID_DEL_THREADLIST:
        // ���擾�X���폜
        return (ExecuteAction(MULTIBTN_FEATURE_DELNOTGET));
        break;

      case MNUID_DELETE_MESSAGE:
        // �X���폜
        err = ExecuteAction(MULTIBTN_FEATURE_DELETE);
        break;

      case MNUID_GET_ALLMESSAGE:
        // ���b�Z�[�W�S���擾(�Ď擾)
        return (ExecuteAction(MULTIBTN_FEATURE_ALLGET));
        break;

      case MNUID_UPDATE_MESSAGE:
        // ���b�Z�[�W�����擾
        return (ExecuteAction(MULTIBTN_FEATURE_PARTGET));
        break;

      case MNUID_OPEN_MESSAGE:
        // �X���Q��
        return (ExecuteAction(MULTIBTN_FEATURE_OPEN));
        break;

      case MNUID_FONT_MODE:
        // �`�惂�[�h�ύX
        return (ExecuteAction(MULTIBTN_FEATURE_GRAPHMODE));
        break;

      case MNUID_FONT:
        // �t�H���g�ύX
        return (ExecuteAction(MULTIBTN_FEATURE_FONT));
        break;

      case MNUID_GET_MESSAGENUM:
        // �X���Ԏw��̃��b�Z�[�W�擾
        return (ExecuteAction(MULTIBTN_FEATURE_MESNUM));
        break;

      case MNUID_SEARCH_TITLE:
        // �^�C�g������(������̐ݒ�)
        err = ExecuteAction(MULTIBTN_FEATURE_SEARCH);
        break;

      case MNUID_SEARCH_NEXT:
        // �X���^�C��������(�����)
        err = ExecuteAction(MULTIBTN_FEATURE_NEXT);
        break;

      case MNUID_SEARCH_PREV:
        // �X���^�C��������(�O����)
        err = ExecuteAction(MULTIBTN_FEATURE_PREV);
        break;

      case MNUID_MOVE_TOP:
        // �X���^�C�g���̐擪�ֈړ�
        return (ExecuteAction(MULTIBTN_FEATURE_TOP));
        break;

      case MNUID_MOVE_BOTTOM:
        // �X���^�C�g���̖����ֈړ�(�_�~�[�ňُ�f�[�^�𑗂荞��)
        return (ExecuteAction(MULTIBTN_FEATURE_BOTTOM));
        break;

      case MNUID_COPY_READONLY:
        // �X�����Q�ƃ��O�փR�s�[����
        (void) ExecuteAction(MULTIBTN_FEATURE_COPYGIKO);
        break;

      case MNUID_OS5_LAUNCH_WEB:
        // �I�������X�����u���E�U�ŊJ��
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENWEB);
        break;

      case MNUID_BACKUP_DATABASE:
        // DB��VFS�Ƀo�b�N�A�b�v����
        (void) ExecuteAction(MULTIBTN_FEATURE_DBCOPY);
        break;

      case MNUID_NET_CONFIG:
        // �l�b�g���[�N�ݒ���J��
        (void) ExecuteAction(MULTIBTN_FEATURE_NETWORK);
        break;

      case MNUID_SORT_SUBJECT:
        // �X���̐�������s����
        return (ExecuteAction(MULTIBTN_FEATURE_SORT));
        break;

      case MNUID_HANDERA_ROTATE:
        // HandEra ��ʉ�] //
        (void) ExecuteAction(MULTIBTN_FEATURE_ROTATE);
        break;

      case MNUID_CONFIG_USER1TAB:
        // ���[�U�^�u�P�ݒ� 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER1TAB);
        break;

#ifdef USE_USERTAB_DETAIL
      case MNUID_CONFIG_USER2TAB:
        // ���[�U�^�u�Q�ݒ� 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER2TAB);
        break;

      case MNUID_CONFIG_USER3TAB:
        // ���[�U�^�u�R�ݒ� 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER3TAB);
        break;

      case MNUID_CONFIG_USER4TAB:
        // ���[�U�^�u�S�ݒ� 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER4TAB);
        break;

      case MNUID_CONFIG_USER5TAB:
        // ���[�U�^�u�T�ݒ� 
        (void) ExecuteAction(MULTIBTN_FEATURE_USER5TAB);
        break;
#endif

      case MNUID_NEW_THREAD:
        // �X������
        (void) ExecuteAction(MULTIBTN_FEATURE_NEWTHREAD);
        break;
        
      case MNUID_GOTO_DUMMY:
        // �_�~�[��ʂ�
        (void) ExecuteAction(MULTIBTN_FEATURE_GOTODUMMY);
        break;

      case MNUID_NNSI_END:
        // NNsi�I��
        (void) ExecuteAction(MULTIBTN_FEATURE_NNSIEND);
        break;

      case MNUID_CHECK_NOFMESSAGE:
        // �^�u���(�X�����m�F)
        (void) ExecuteAction(MULTIBTN_FEATURE_TABINFO);
        break;

      case MNUID_MOVE_LOGMNGLOC:
        // ���O�Ǘ��ꏊ�ύX
        (void) ExecuteAction(MULTIBTN_FEATURE_MOVELOGLOC);
        break;

      case MNUID_GET_URL:
        // URL���J��
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENURL);
        break;

#ifdef USE_MACROEDIT
      case MNUID_MACRO_SETTING:
        // �}�N���ݒ�
        (void) ExecuteAction(MULTIBTN_FEATURE_MACRO_SET);
        break;
#endif

      case MNUID_MACRO_EXECUTE:
        // �}�N�����s
        (void) ExecuteAction(MULTIBTN_FEATURE_MACRO_EXEC);
        break;
      case MNUID_MACRO_EXECLOG:
        // �}�N�����s���O
        (void) ExecuteAction(MULTIBTN_FEATURE_MACRO_LOG);
        break;

      case MNUID_MACRO_DATAVIEW:
        // �}�N�����e�\��
        (void) ExecuteAction(MULTIBTN_FEATURE_MACRO_VIEW);
        break;

      case MNUID_OYSTERLOGIN:
        // �����O�C��
        (void) ExecuteAction(MULTIBTN_FEATURE_OYSTERLOGIN);
        break;

      case MNUID_SHOW_LISTHELP:
        // �������
        (void) ExecuteAction(MULTIBTN_FEATURE_USAGE);
        break;

      case MNUID_SELECTIONMENU:
        // �I�����j���[
        (void) ExecuteAction(MULTIBTN_FEATURE_SELMENU);
        break;
      case MNUID_GETLOGLIST:
        // �Q�ƃ��O�擾�ݒ�ꗗ
        (void) ExecuteAction(MULTIBTN_FEATURE_OPENGETLOGLIST);
        break;

      case MNUID_SEARCH_BBS:
        // ����(������̐ݒ�)
        err = ExecuteAction(MULTIBTN_FEATURE_SEARCHBBS);
        break;

      case MNUID_SEARCH_NEXTBBS:
        // �̎�����
        err = ExecuteAction(MULTIBTN_FEATURE_NEXTSEARCHBBS);
        break;

      case MNUID_BEAM_URL:
        // URL��Beam����
        err = ExecuteAction(MULTIBTN_FEATURE_BEAMURL);
        break;

      case MNUID_MACRO_SELECT_EXEC:
        // �}�N���I�����s
        err = ExecuteAction(MULTIBTN_FEATURE_SELECT_MACRO_EXEC);
        break;

      case MNUID_BE2chLOGIN:
        // Be@2ch���O�C�����������s����
        err = ExecuteAction(MULTIBTN_FEATURE_BE_LOGIN);
        break;

      case MNUID_LIST_GETLOGLIST:
        // �X���ԍ��w��\��̗\��ꗗ��\������
        (void) ExecuteAction(MULTIBTN_FEATURE_LIST_GETLOG);
        break;

      default:
        // ���̑�(���肦�Ȃ��͂�����...)
        break;
    }

    // �X���ꗗ��ʂ��ĕ\������
    NNsi_RedrawThreadList();
    return (err);
}

/*=========================================================================*/
/*   Function : NNsi_RedrawThreadList                                      */
/*                                                    �X���ꗗ��ʂ��ĕ`�� */
/*=========================================================================*/
void NNsi_RedrawThreadList(void)
{
    FormType      *frm;
    RectangleType  dimF;
    UInt16         fontID, fontHeight, nlines;

    // �X���ꗗ��ʈȊO�ł͍X�V���Ȃ�
    if (FrmGetActiveFormID() == FRMID_THREAD)
    {
        frm = FrmGetActiveForm();
        FrmDrawForm(frm);
        FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, GADID_MESSAGE_LIST), 
                           &dimF);
        fontID = ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ?
                           (NNshGlobal->NNsiParam)->currentFont : (NNshGlobal->NNsiParam)->sonyHRFontTitle;

        NNsi_UpdateRectangle(&dimF, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                             &fontID, &fontHeight, &nlines);

        NNsi_WinDrawList((NNshGlobal->NNsiParam)->selectedTitleItem, 
                         NNshGlobal->msgTitleIndex, nlines, &dimF, fontHeight);
#ifdef USE_PIN_DIA
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            NNshGlobal->updateHR = NNSH_UPDATE_DISPLAY;
        }
#endif
    }
    return;
}

#ifdef USE_LOGCHARGE
/*-------------------------------------------------------------------------*/
/*   Function :   openURL                                                  */
/*                                                              URL���J��  */
/*-------------------------------------------------------------------------*/
static void openURL(void)
{
    ListType            *lstP;
    UInt16               selBBS, isImage;
    Char                *url, *fileName, *ptr;
    NNshGetLogDatabase  *dbData;
    
    // �\���^�u���Q�ƃ��O���`�F�b�N
    lstP    = FRM_GET_ACTIVE_OBJECT_PTR(LSTID_BBSINFO);
    selBBS  = LstGetSelection(lstP);
#ifdef GETLOG_LIMIT_OFFLINE
    if (selBBS != NNSH_SELBBS_OFFLINE)
    {
        // ���̋@�\�́A�Q�ƃ��O�����̋@�\�Ȃ̂ŉ��������I������
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_ONLY_READONLYLOG, MSG_ABORT_MESSAGE, 0);
        return;
    }
#endif

    // �擾����URL�̓���
    url = MEMALLOC_PTR(BUFSIZE + MARGIN);
    if (url == NULL)
    {
        return;
    }
    MemSet(url, BUFSIZE + MARGIN, 0x00);

    // �t�@�C�����i�[�̈���m��
    fileName = MEMALLOC_PTR(MAXLENGTH_FILENAME + MARGIN);
    if (fileName == NULL)
    {
        MEMFREE_PTR(url);
        return;
    }
    MemSet(fileName, MAXLENGTH_FILENAME + MARGIN, 0x00);

    // DB�̃_�~�[�̈���m��
    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        MEMFREE_PTR(url);
        MEMFREE_PTR(fileName);
        return;
    }
    MemSet(dbData, sizeof(NNshGetLogDatabase) + MARGIN, 0x00);

    // URL����͂��Ă��炤�B
    isImage = 0;
    StrCopy(url, "http://");
    if (DataInputDialog(MSG_DIALOG_SET_URL, url, BUFSIZE, 
                            NNSH_DIALOG_USE_INPUTURL, &(isImage)) != true)
    {
        // URL�̓��͂��L�����Z�����ꂽ�ꍇ
        goto ABORT_END;
    }
#ifdef USE_SSL
    if ((url[0] != 'h')||(url[1] != 't')||(url[2] != 't')||(url[3] != 'p')||
        ((url[4] != ':')&&(url[4] != 's'))||((url[5] != '/')&&(url[5] != ':')))
    {
        // URL�̎w�肪�܂������Ă����ꍇ(http:// �ł͂��܂�Ȃ������ꍇ)
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_WRONGURL, url, 0);
        goto ABORT_END;
    }
#else
    if ((url[0] != 'h')||(url[1] != 't')||(url[2] != 't')||(url[3] != 'p')||
        (url[4] != ':')||(url[5] != '/'))
    {
        // URL�̎w�肪�܂������Ă����ꍇ(http:// �ł͂��܂�Ȃ������ꍇ)
        NNsh_WarningMessage(ALTID_WARN, MSG_WARN_WRONGURL_NOTSSL, url, 0);
        goto ABORT_END;
    }
#endif
    if (NNsh_ConfirmMessage(ALTID_CONFIRM, url, MSG_GETURLLOG_CONFIRM, 0) != 0)
    {
        // �f�[�^�̎擾�m�F���L�����Z�����ꂽ�ꍇ
        goto ABORT_END;
    }

    // �f�[�^��ۑ�����X���t�@�C�����̓���
    ptr = url + StrLen(url) - 1;
    while ((*ptr != '/')&&(*ptr != '?')&&(*ptr != '*'))
    {
        ptr--;
    }

    // �f�[�^�̍Ō�ɃX���b�V�����������ꍇ�ɂ̓t�@�C�������R�s�[���Ȃ�
    if (ptr < url + StrLen(url) - 1)
    {
        StrNCopy(fileName, (ptr + 1), (MAXLENGTH_FILENAME - 1));
    }

    if (isImage == 0)
    {
        // �t�q�k�f�[�^�̎擾������...
        ReadOnlyLogDataPickup(false, dbData, url, fileName, NNSH_MSGNUM_UNKNOWN, 0, 0, 0, NULL);

        // ���ݕ\�����̔͎Q�Ɛ�p���O�̏ꍇ
        // (�X���ꗗ�ɕ\�����ׂ����ڂ�������\��������ꍇ...)
        if (selBBS == NNSH_SELBBS_OFFLINE)
        {
            // ���O�ꗗ�̐擪��TOP�f�B���N�g����\������
            (NNshGlobal->NNsiParam)->readOnlySelection = 0;

            // �I�����ꂽ�f�B���N�g�����X�V���ꂽ�ꍇ�ɂ́A�X���^�C�ꗗ���X�V����
            Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 0, NNSH_STEP_UPDATE);
        }
        else
        {
            //  �ꗗ���Q�Ɛ�p���O�ł͂Ȃ��ꍇ�ɂ́A�X���ꗗ�̏�񂪍X�V
            // ����Ȃ��͂��Ȃ̂ŁA�̈���J�����ďI������(ABORT����Ȃ�����...)
            goto ABORT_END;
        }
    }
    else
    {
        // URL����C���[�W�f�[�^���擾���鏈��
        ReadOnlyLogDataPickup(true, dbData, url, fileName, NNSH_MSGNUM_UNKNOWN, 0, 0, 0, NULL);

        // �C���[�W�f�[�^��M���ɂ́A�X���ꗗ�̏�񂪍X�V����Ȃ��̂�
        // �̈���J�����ďI������(ABORT����Ȃ�����...)
        goto ABORT_END;
    }

    // �m�ۗ̈���J��
    MEMFREE_PTR(dbData);
    MEMFREE_PTR(fileName);
    MEMFREE_PTR(url);
    return;

ABORT_END:
    // �m�ۗ̈���J��
    MEMFREE_PTR(dbData);
    MEMFREE_PTR(fileName);
    MEMFREE_PTR(url);
    
    // ��ʂ��ĕ`�悵�ďI������
    Update_Thread_List((NNshGlobal->NNsiParam)->lastBBS, 
                              (NNshGlobal->NNsiParam)->selectedTitleItem, NNSH_STEP_REDRAW);
    return;
}
#endif
#ifdef USE_LOGCHARGE
/*-------------------------------------------------------------------------*/
/*   Function :   changeGetLogUrlSite                                      */
/*                                        �Q�ƃ��O���[�h�̃T�C�g��I������ */
/*-------------------------------------------------------------------------*/
static void changeGetLogUrlSite(Boolean defaultSettingMode)
{
    Err                   ret;
    NNshGetLogDatabase  *logDb;
    DmOpenRef            dbRef;
    MemHandle            memH;
    UInt16               size, itemCnt, btnId, dbCount, loop, recList[MAX_SELECTLOGRECORD + 1];
    ControlType         *selP;
    Char                *labelP, *buffer, *addrAddr[MAX_SELECTLOGRECORD + 1];

    // �e��ϐ��̏�����
    dbCount = 0;
    itemCnt = 0;
    memH    = 0;
    size    = MAX_TITLE_LABEL * MAX_SELECTLOGRECORD;
    selP    = FrmGetObjectPtr(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), SELTRID_GETLOG_URL_SELECT));

    // �f�[�^���x���̈�̊m��
    buffer = MEMALLOC_PTR(size + MARGIN);
    if (buffer == NULL)
    {
        // �f�[�^�̈�m�ێ��s�A�G���[�ɂ���
        return;
    }
    MemSet(buffer, (size + MARGIN), 0x00);
    labelP = buffer;

    //////////  �T�C�g���̈ꗗ�ƃC���f�b�N�X�𐶐����� //////////
    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return;
    }

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &dbCount);

    // Busy�_�C�A���O��\������
    Show_BusyForm(MSG_CHECK_URLLIST);

    // ���R�[�h�ԍ��̎Ⴂ�ق����珇�ɕ\������...
    for (loop = dbCount; ((loop != 0)&&(itemCnt < MAX_SELECTLOGRECORD)); loop--)
    {
        // ���R�[�h�����擾����
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &memH, (void**) &logDb);
        if (ret == errNone)
        {
            // ���X�g�ɕ\�����ׂ����O���m�F����
            if (((NNshGlobal->NNsiParam)->getROLogLevel == 0)||(logDb->usable == (NNshGlobal->NNsiParam)->getROLogLevel))
            {
                // �\�����ׂ����O�^�C�g���������I
                StrCopy(labelP, logDb->dataTitle);
                addrAddr[itemCnt] = labelP;
                recList [itemCnt] = (loop - 1);
                labelP = labelP + StrLen(logDb->dataTitle) + 1;
                itemCnt++;
            }

            // �m�ۂ������R�[�h����������
            ReleaseRecordReadOnly_NNsh(dbRef, memH);
        }
    }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // �P���Ȃ��A�I������
        CtlSetLabel(selP, "???");
        MEMFREE_PTR(buffer);
        return;
    }
    // �I���E�B���h�E��\�����AURL�̑I���𑣂�
    if (defaultSettingMode != true)
    {
        btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, buffer, itemCnt, 0);    
    }
    else
    {
        //(NNshGlobal->NNsiParam)->selectedTitleItem = 0;

        // �O��̐ݒ�������I�ɉ�ʂ֔��f������ꍇ...
        for (loop = 0; loop < itemCnt; loop++)
        {
            if (recList[loop] == ((NNshGlobal->NNsiParam)->selectedTitleItemForGetLog))
            {
                StrNCopy(NNshGlobal->getLogListUrlNameLabel, addrAddr[loop], MAX_TITLE_LABEL);
                CtlSetLabel(selP, NNshGlobal->getLogListUrlNameLabel);
                MEMFREE_PTR(buffer);
                return;
            }
        }
        // �T�C�g���o�^��������Ă����ꍇ...
        (NNshGlobal->NNsiParam)->selectedTitleItemForGetLog = recList[0];
        StrNCopy(NNshGlobal->getLogListUrlNameLabel, addrAddr[0], MAX_TITLE_LABEL);
        CtlSetLabel(selP, NNshGlobal->getLogListUrlNameLabel);
        MEMFREE_PTR(buffer);
        return;
    }

    // OK�{�^���������ꂽ�Ƃ��A�A�A
    if (btnId != BTNID_JUMPCANCEL)
    {
        //(NNshGlobal->NNsiParam)->selectedTitleItem = 0;

        // �T�C�g�����Z���N�^�g���K���x���ɔ��f������
        (NNshGlobal->NNsiParam)->selectedTitleItemForGetLog = recList[NNshGlobal->jumpSelection];
        StrNCopy(NNshGlobal->getLogListUrlNameLabel, addrAddr[NNshGlobal->jumpSelection], MAX_TITLE_LABEL);
        CtlSetLabel(selP, NNshGlobal->getLogListUrlNameLabel);
    }
    MEMFREE_PTR(buffer);
    return;
}
#endif // #ifdef USE_LOGCHARGE
