/*============================================================================*
 *  FILE: 
 *     getloglist.c
 *
 *  Description: 
 *     �Q�ƃ��O�擾�ݒ�̈ꗗ���
 *===========================================================================*/
#define GETLOGLIST_C
#include "local.h"

#ifdef USE_LOGCHARGE

/*------------------------------------------------------------------------*/
/*   Function : selectionMenu_getLogList                                  */
/*                                     �擾���O�ꗗ��ʑI�����j���[�̕\�� */
/*------------------------------------------------------------------------*/
static UInt16 selectionMenu_getLogList(void)
{
    UInt16  btnId, feature, nofItems;
    UInt16 *jumpList;
    Char   *listLabel, *numP;

    // �W�����v�ꗗ��ݒ�
    feature  = NNSH_GETLOG_FEATURE_NONE;
    nofItems = 0;
    listLabel = MEMALLOC_PTR(NNSH_ITEMLEN_GETLOGJUMP * NNSH_NOF_JUMPMSG_GETLOG + MARGIN);
    if (listLabel == NULL)
    {
        return (feature);
    }
    MemSet(listLabel, (NNSH_ITEMLEN_GETLOGJUMP * NNSH_NOF_JUMPMSG_GETLOG + MARGIN), 0x00);
    numP = listLabel;

    jumpList = MEMALLOC_PTR(NNSH_NOF_JUMPMSG_GETLOG + MARGIN);
    if (jumpList == NULL)
    {
        MEMFREE_PTR(listLabel);
        return (feature);
    }
    MemSet(jumpList, (NNSH_NOF_JUMPMSG_GETLOG + MARGIN), 0x00);


    // �I�����j���[�ꗗ��ǉ�����

    // ���X�g�Ɂu�ꗗ�֖߂�v��ǉ�
    StrCopy(numP, NNSH_JUMPMSG_GETLOGRETURN);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOGRETURN);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOGRETURN);
    nofItems++;

    // ���X�g�Ɂu�J���v��ǉ�
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_OPEN);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_OPEN);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_OPEN);
    nofItems++;

    if (NNshGlobal->browserCreator != 0)
    {
        // ���X�g�Ɂu�u���E�U�ŊJ���v��ǉ�
        StrCopy(numP, NNSH_JUMPMSG_GETLOGBROWSER);
        numP = numP + sizeof(NNSH_JUMPMSG_GETLOGBROWSER);
        jumpList[nofItems] = (NNSH_JUMPSEL_GETLOGBROWSER);
        nofItems++;
    }

    if ((CheckInstalledResource_NNsh('appl', 'asUR') != false)||
         (CheckInstalledResource_NNsh('DAcc', 'mmuA') != false))
    {
        // MemoURL/MemoURLDA���C���X�g�[������Ă���...
        // ���X�g�ɁuMemoURL�ɑ���v��ǉ�
        StrCopy(numP, NNSH_JUMPMSG_MEMOURL);
        numP = numP + sizeof(NNSH_JUMPMSG_MEMOURL);
        jumpList[nofItems] = (NNSH_JUMPSEL_MEMOURL);
        nofItems++;
    }

    // ���X�g�Ɂu�T�C�g���v��ǉ�
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_WWWINFO);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_WWWINFO);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_WWWINFO);
    nofItems++;

    // ���X�g�Ɂu�V���m�F�v��ǉ�
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_CHECK);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_CHECK);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_CHECK);
    nofItems++;

    // ���X�g�Ɂu���j���[�v��ǉ�
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_OPENMNU);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_OPENMNU);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_OPENMNU);
    nofItems++;

    // ���X�g�ɁuLevel�ύX�v��ǉ�
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_LEVEL);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_LEVEL);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_LEVEL);
    nofItems++;

    // ���X�g�Ɂu����ؒf�v��ǉ�
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_DISCONN);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_DISCONN);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_DISCONN);
    nofItems++;

    // ���X�g�Ɂu���ǋL���폜�v��ǉ�
    StrCopy(numP, NNSH_JUMPMSG_GETLOG_DELETE_READ);
    numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_DELETE_READ);
    jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_DELETE_READ);
    nofItems++;

    // NNsi�I��
    if ((NNshGlobal->NNsiParam)->addMenuNNsiEnd != 0)
    {
        StrCopy(numP, NNSH_JUMPMSG_NNSIEND);
        numP = numP + sizeof(NNSH_JUMPMSG_NNSIEND);
        jumpList[nofItems] = NNSH_JUMPSEL_NNSIEND;
        nofItems++;
    }

#ifdef USE_BT_CONTROL
    // Bluetooth On/Off
    if ((NNshGlobal->NNsiParam)->addBtOnOff != 0)
    {
        // ���X�g�ɁuBt On/Bt Off�v��ǉ�
        StrCopy(numP, NNSH_JUMPMSG_GETLOG_BT_ON);
        numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_BT_ON);
        jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_BT_ON);
        nofItems++;

        StrCopy(numP, NNSH_JUMPMSG_GETLOG_BT_OFF);
        numP = numP + sizeof(NNSH_JUMPMSG_GETLOG_BT_OFF);
        jumpList[nofItems] = (NNSH_JUMPSEL_GETLOG_BT_OFF);
        nofItems++;
    }
#endif

    // �I���E�B���h�E��\������
    btnId = NNshWinSelectionWindow(FRMID_JUMPMSG, listLabel, nofItems, 0);
    if (btnId != BTNID_JUMPCANCEL)
    {
        switch (jumpList[NNshGlobal->jumpSelection])
        {
          case NNSH_JUMPSEL_GETLOGRETURN:
            feature = NNSH_GETLOG_FEATURE_CLOSE_GETLOG;
            break;
          case NNSH_JUMPSEL_GETLOG_OPEN:
            feature = NNSH_GETLOG_FEATURE_OPEN_NNSI;
            break;
          case NNSH_JUMPSEL_GETLOGBROWSER:
            feature = NNSH_GETLOG_FEATURE_OPENWEB;
            break;
          case NNSH_JUMPSEL_GETLOG_MEMOURL:
            feature = NNSH_GETLOG_FEATURE_OPEN_MEMOURL;
            break;
          case NNSH_JUMPSEL_GETLOG_CHECK:
            feature = NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL;
            break;
          case NNSH_JUMPSEL_GETLOG_OPENMNU:
            feature = NNSH_GETLOG_FEATURE_OPEN_GETLOGMENU;
            break;
          case NNSH_JUMPSEL_GETLOG_LEVEL:
            feature = NNSH_GETLOG_FEATURE_OPEN_GETLEVEL;
            break;
          case NNSH_JUMPSEL_GETLOG_DISCONN:
            feature = NNSH_GETLOG_FEATURE_FORCE_DISCONNECT;
            break;
          case NNSH_JUMPSEL_GETLOG_BT_ON:
            feature = NNSH_GETLOG_FEATURE_BT_ON;
            break;
          case NNSH_JUMPSEL_GETLOG_BT_OFF:
            feature = NNSH_GETLOG_FEATURE_BT_OFF;
            break;
          case NNSH_JUMPSEL_GETLOG_NNSIEND:
            feature = NNSH_GETLOG_FEATURE_NNSIEND;
            break;
          case NNSH_JUMPSEL_GETLOG_EDIT:
            feature = NNSH_GETLOG_FEATURE_EDITITEM;
            break;
          case NNSH_JUMPSEL_GETLOG_NEW:
            feature = NNSH_GETLOG_FEATURE_ENTRY_NEWSITE;
            break;
          case NNSH_JUMPSEL_GETLOG_SEPA:
            feature = NNSH_GETLOG_FEATURE_EDIT_SEPARATOR;
            break;
          case NNSH_JUMPSEL_MEMOURL:
            feature = NNSH_GETLOG_FEATURE_OPEN_MEMOURL;
            break;
          case NNSH_JUMPSEL_GETLOG_WWWINFO:
            feature = NNSH_GETLOG_FEATURE_SITE_INFORMATION;
            break;
          case NNSH_JUMPSEL_GETLOG_DELETE_READ:
            feature = NNSH_GETLOG_FEATURE_DELETELOG_READ;
          default:
            break;
        }
    }

    MEMFREE_PTR(jumpList);
    MEMFREE_PTR(listLabel);
    return (feature);
}

/*-------------------------------------------------------------------------*/
/*   Function : displayString                                              */
/*                                                          ��ʕ\���̍X�V */
/*-------------------------------------------------------------------------*/
static void displayString(Char *lineData, RectangleType *dimF, UInt16 command)
{
    IndexedColorType color;
    UInt16           len, isReverse;
    
    isReverse = command & NNSH_GETLOGSTATE_MASK_SELECT;

#ifdef USE_COLOR
    // �t�H���g�J���[�̐ݒ�(���ǂ̐F�ɂ���)
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        if ((command & NNSH_GETLOGSTATE_MASK_UPDATE) == NNSH_GETLOGSTATE_UPDATE)
        {
            color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorNew;
            WinSetForeColor(color);
            WinSetTextColor(color);
        }
        if ((command & NNSH_GETLOGSTATE_DATETIME) == NNSH_GETLOGSTATE_DATETIME)
        {
            color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorUnknown;
            WinSetForeColor(color);
            WinSetTextColor(color);
        }
    }
#endif

#ifdef USE_CLIE
    if (NNshGlobal->hrRef != 0)
    {
        // �t�H���g�̃��[�h���b�v(�`��̈���ɓ��e��}���邽��)
        if (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE)
        {
            len = HRFntWordWrap(NNshGlobal->hrRef, lineData, dimF->extent.x);
        }
        else
        {
            len = FntWordWrap(lineData, dimF->extent.x);
        }
        // CLIE�n�C���]�`��(Truncate���ĕ\��)
        HRWinDrawTruncChars(NNshGlobal->hrRef, 
                            lineData, len,
                            dimF->topLeft.x, dimF->topLeft.y, dimF->extent.x);
        if (isReverse != 0)
        {
            // �J�[�\���ʒu�𔽓]������
            HRWinDrawInvertedChars(NNshGlobal->hrRef, 
                                   lineData, len,
                                   dimF->topLeft.x, dimF->topLeft.y);
        }
    }
    else
#endif
    {
        // �ʏ탂�[�h�`��(Truncate���ĕ\��)
        len = FntWordWrap(lineData, dimF->extent.x);
        WinDrawTruncChars(lineData, len, dimF->topLeft.x,
                          dimF->topLeft.y, dimF->extent.x);
        if (isReverse != 0)
        {
            // �J�[�\���ʒu�𔽓]������
            WinDrawInvertedChars(lineData, len,
                                 dimF->topLeft.x, dimF->topLeft.y);
        }
    }

#ifdef USE_COLOR
    // ���ǂ̐F�ɖ߂�
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        if (((command & NNSH_GETLOGSTATE_MASK_UPDATE) == NNSH_GETLOGSTATE_UPDATE)||
            ((command & NNSH_GETLOGSTATE_DATETIME) == NNSH_GETLOGSTATE_DATETIME))
        {
            color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorAlready;
            WinSetForeColor(color);
            WinSetTextColor(color);
        }
    }
#endif
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : createList_getLogList                                      */
/*                                                  ��ʕ\���p�̕�����擾 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
static void createList_getLogList(UInt16 getLevel, NNshDispList *displayList)
{
    Err                  ret;
    DmOpenRef            dbRef;
    UInt16               lp, maxRec;
    NNshGetLogDatabase  *dbData;
    MemHandle            memH;

    // �J�E���g�̓[��
    displayList->dataCount = 0;

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return;
    }

    // ���R�[�h�����擾
    maxRec = 0;
    GetDBCount_NNsh(dbRef, &maxRec);

    if (maxRec > MAX_GETLOG_RECORDS)
    {
        // ���R�[�h�̍ő匏���𒴂��Ă����ꍇ�ɂ͍ő匏���܂ŕ\��...
        NNsh_DebugMessage(ALTID_ERROR, "OVERFLOW LogParmanent-NNsi", " cnt:", maxRec);
        maxRec = MAX_GETLOG_RECORDS;
    }
    for (lp = 0; lp < maxRec; lp++)
    {
        // ���R�[�h�́A�t��������ǂݏo���悤�ɂ���
        ret = GetRecordReadOnly_NNsh(dbRef, (maxRec - (lp + 1)), &memH, (void **) &dbData);
        if (ret != errNone)
        {
            // ���O�f�[�^�擾���ɃG���[�����A������
            NNsh_DebugMessage(ALTID_ERROR, "GetRecordReadOnly_NNsh()", " ret:", ret);
            break;
        }
        if ((getLevel == 0)||((getLevel - 1) == dbData->usable))
        {
            // �\�����ׂ����R�[�h�����o
            if (displayList->dataCount < MAX_GETLOG_RECORDS)
            {
                displayList->title[displayList->dataCount] =
                         MEMALLOC_PTR(MAX_TITLE_LABEL + MAX_GETLOG_URL + (MARGIN * 2));
                if (displayList->title[displayList->dataCount] == NULL)
                {
                    // �̈�̊m�ۂɎ��s...
                    break;
                }
                // �f�[�^(�^�C�g���ƍX�V����)���i�[����
                MemSet(displayList->title[displayList->dataCount],
                       (MAX_TITLE_LABEL + MAX_GETLOG_URL + (MARGIN * 2)), 0x00);
                StrCopy(displayList->title[displayList->dataCount], dbData->dataTitle);
                StrCat (displayList->title[displayList->dataCount], " (");
                StrNCopy(&displayList->title[displayList->dataCount][StrLen(displayList->title[displayList->dataCount])], dbData->dataURL, MAX_TITLE_LABEL);
                if (StrLen(dbData->dataURL) > MAX_TITLE_LABEL)
                {
                    displayList->title[displayList->dataCount][StrLen(displayList->title[displayList->dataCount])] = chrEllipsis;
                }
                StrCat (displayList->title[displayList->dataCount], ") ");
                displayList->updateTime[displayList->dataCount] = dbData->lastUpdate;
                displayList->recNum[displayList->dataCount] = (maxRec - (lp + 1));
                if (dbData->state != NNSH_SUBJSTATUS_NOT_YET)
                {
                    // �擾���Ă����ꍇ...�ŏ�ʃr�b�g�𗧂ĂĂ���...
                    displayList->recNum[displayList->dataCount] = displayList->recNum[displayList->dataCount] | NNSH_GETLOG_NEWARRIVAL_LOG;
                }
                (displayList->dataCount)++;
            }
        }
        // DB���R�[�h���J��
        ReleaseRecordReadOnly_NNsh(dbRef, memH);     
    }

    // �f�[�^�x�[�X�����
    CloseDatabase_NNsh(dbRef);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : RefreshDisplay_getLogList                                  */
/*                                                          ��ʕ\���̍X�V */
/*-------------------------------------------------------------------------*/
void RefreshDisplay_getLogList(FormType *frm, UInt16 command, UInt16 parameter)
{
    RectangleType        dimF, dateF;
    Char                 lineData[BUFSIZE];
    Int16                pics;
    UInt16               fontID, nlines, fontHeight, update;
    UInt16               startLine, lineStatus, lp;
    UInt32               currentTime;
    DateTimeType         dateBuf;
    NNshDispList        *displayList;
    
    // ���[�N�̈��ϐ��ɔ��f������
    displayList = NNshGlobal->dispList;

    // ���ݎ������擾
    currentTime = TimGetSeconds();

    // ��Ԃ̏�����
    lineStatus = NNSH_DISP_ALL;
    startLine  = (NNshGlobal->NNsiParam)->getLogListPage;

    // ��ʂ��ĕ`�悷��
    switch (command)
    {
      case NNSH_STEP_REDRAW:
        // FrmEraseForm(frm);
        FrmDrawForm(frm);
        break;

      case NNSH_STEP_NEWOPEN:
        FrmEraseForm(frm);
        FrmDrawForm(frm);
        
        // �|�b�v�A�b�v�g���K�̃��x����(�ݒ胍�O���x���ɍ��킹)�X�V����
        if ((NNshGlobal->NNsiParam)->getROLogLevel == 0)
        {
             update = 0;
        }
        else
        {
             update = (NNshGlobal->NNsiParam)->getROLogLevel + 1;
        }
        NNshWinSetPopItems(frm, POPTRID_GETLOGLEVEL, LSTID_GETLOGLEVEL, update);
        break;
    }

    // ��ʂ̑傫���ƕ\���\�s�����擾����
    FrmGetObjectBounds(frm,
                   FrmGetObjectIndex(frm, GADID_MESSAGE_LIST), &dimF);
    fontID = ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ?
                           (NNshGlobal->NNsiParam)->currentFont : (NNshGlobal->NNsiParam)->sonyHRFontTitle;
    NNsi_UpdateRectangle(&dimF, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                                                &fontID, &fontHeight, &nlines);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, " ::disp:: nlines:%d, (%d+%d, %d+%d)",
                      nlines, dimF.topLeft.x, dimF.extent.x , dimF.topLeft.y , dimF.extent.y);
#endif // #ifdef USE_REPORTER

    // �Z���N�V������������ʕ\���͈͂𒴂��Ă����ꍇ...
    if (parameter >= nlines)
    {
        parameter = nlines - 1;
    }
    NNshGlobal->work1 = displayList->dataCount;

    // �R�}���h����\���ʒu�̐擪�ƕ\����Ԃ����肷��
    switch (command)
    {
      case NNSH_STEP_TO_TOP:
        // �擪�y�[�W��\������
        startLine  = 0;
        lineStatus = NNSH_DISP_UPPERLIMIT;
        break;

      case NNSH_STEP_PAGEUP:
        // �P�y�[�W���\������
        if ((NNshGlobal->NNsiParam)->getLogListPage <= nlines)
        {
            startLine  = 0;
            lineStatus = NNSH_DISP_UPPERLIMIT;
        }
        else
        {
            startLine  = (NNshGlobal->NNsiParam)->getLogListPage - nlines;
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_PAGEDOWN:
        // �P�y�[�W����\������
        if (((NNshGlobal->NNsiParam)->getLogListPage + nlines) >= displayList->dataCount)
        {
            startLine  = (NNshGlobal->NNsiParam)->getLogListPage;
            lineStatus = NNSH_DISP_LOWERLIMIT;
        }
        else
        {
            startLine  = (NNshGlobal->NNsiParam)->getLogListPage + nlines;
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_TO_BOTTOM:
        // �����y�[�W��\������
        startLine  = ((displayList->dataCount) < nlines) ? 
                                     0 : (displayList->dataCount) - nlines;
        lineStatus = NNSH_DISP_LOWERLIMIT;
        break;

      default:
        // ���̑��i�ĕ`��H�j
        startLine  = (NNshGlobal->NNsiParam)->getLogListPage;
        if (startLine != 0)
        {
            lineStatus = NNSH_DISP_HALFWAY;
        }
        else
        {
            lineStatus = NNSH_DISP_UPPERLIMIT;
        }
        break;
    }

    // �y�[�W��Ԃ𔽉f
    if (lineStatus != NNSH_DISP_UPPERLIMIT)
    {
        if (startLine + nlines >= displayList->dataCount)
        {
            lineStatus = NNSH_DISP_LOWERLIMIT;
        }
        else
        {
            lineStatus = NNSH_DISP_HALFWAY;
        }
    }
    if (displayList->dataCount <= nlines)
    {
        lineStatus = NNSH_DISP_ALL;
    }

    // �\���̈���N���A����
    NNsi_EraseRectangle(&dimF);

    // �㉺�{�^���̕\���ݒ�
    NNsh_UpDownButtonControl(frm, lineStatus, BTNID_LISTPREV, BTNID_LISTNEXT);

#ifdef USE_HIGHDENSITY
    // ���𑜓x�̐ݒ�
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFontTitle);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

    // ������̕`��i�s�P�ʂŕ`�悷��j
    parameter = parameter + startLine;

    // �I���A�C�e�����������z���Ă��邩�ǂ����̃`�F�b�N
    if (parameter >= displayList->dataCount)
    {
        parameter = displayList->dataCount - 1;
    }
    for (lp = 0; lp < displayList->dataCount; lp++)
    {
        if (lp < startLine)
        {
            // �\���f�[�^�ł͂Ȃ��A���̃f�[�^��T��
            continue;
        }
        if (lp >= startLine + nlines)
        {
            // �\���\�̈搔�𒴂����ꍇ�ɂ̓��[�v�𔲂���
            break;
        }

        // �I��񂩂ǂ������`�F�b�N
        update = 0;
        if (parameter == lp)
        {
            update = update | NNSH_GETLOGSTATE_SELECTED;
        }

        // �P�s�o��
        MemSet(lineData, sizeof(lineData), 0x00);
        if (displayList->updateTime[lp] != 0)
        {
            // �X�V�����ƌ��ݎ�����20���ȓ��������ꍇ...
            if (((currentTime - displayList->updateTime[lp]) < (60 * 20))&&
                ((displayList->recNum[lp] & 0x8000) != 0))
            {
                // �X�V�}�[�N������
                StrCopy(lineData, "* ");
                update = update | NNSH_GETLOGSTATE_UPDATE;
            }
            else
            {
                StrCopy(lineData, "   ");
            }
            StrCat(lineData, displayList->title[lp]);
            displayString(lineData, &dimF, update);

            // �ŐV�`�F�b�N������\������
            MemSet(lineData, sizeof(lineData), 0x00);
            MemSet(&dateBuf, sizeof(dateBuf), 0x00);
            TimSecondsToDateTime(displayList->updateTime[lp], &dateBuf);

            DateToAscii((UInt8)  dateBuf.month,
                        (UInt8)  dateBuf.day,
                        (UInt16) dateBuf.year, 
                        dfYMDWithSlashes,
                        lineData);
            StrCat(lineData, ",");
            TimeToAscii((UInt8) dateBuf.hour,
                        (UInt8) dateBuf.minute,
                        tfColon24h,
                        &lineData[StrLen(lineData)]);
            MemMove(&dateF, &dimF, sizeof(RectangleType));

#ifdef USE_CLIE
            if (NNshGlobal->hrRef != 0)
            {
                // �t�H���g�̃��[�h���b�v(�`��̈���ɓ��e��}���邽��)
                if (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE)
                {
                    pics = HRFntCharsWidth(NNshGlobal->hrRef, lineData, 15);
                }
                else
                {
                    pics = FntCharsWidth(lineData, 15);
                }
            }
            else
#endif
            {
                pics = FntCharsWidth(lineData, 15);
            }

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, "()()UPDATE: %s  pics:%d", lineData, pics);
#endif // #ifdef USE_REPORTER
            // �����炠����A���炢���ʂȏ����������ȁB�B�B���t�@�N�^�����O�K�v!!
            dateF.topLeft.x = dimF.extent.x - pics - MARGIN;
            dateF.extent.x  = pics + MARGIN;
            dateF.extent.y  = fontHeight;

            // Coord���[�h��؂�ւ��Ă��邽�߁ANNsi_EraseRectangle()���g���Ȃ�...
#ifdef USE_CLIE 
            if (NNshGlobal->hrRef != 0)
            {
                // CLIE�n�C���]���[�h...
                HRWinEraseRectangle(NNshGlobal->hrRef, &dateF, 0);
            }
            else
#endif
            {
                // �ʏ�`�惂�[�h...
                WinEraseRectangle(&dateF, 0);
            }
            dateF.topLeft.x = dimF.extent.x - pics;
            dateF.extent.x  = pics;
            update = ((update & NNSH_GETLOGSTATE_SELECTED) | NNSH_GETLOGSTATE_DATETIME);
            displayString(lineData, &dateF, update);
        }
        else
        {
            // �X�V�����Ȃ��A�^�C�g�������\������
            StrCopy(lineData, "   ");
            StrCat(lineData, displayList->title[lp]);

            displayString(lineData, &dimF, update);
        }

        // ���̍s�ɕ`��ʒu���ړ�������
        dimF.topLeft.y = dimF.topLeft.y + fontHeight;

    }

#ifdef USE_HIGHDENSITY
    // ���𑜓x�̐ݒ������
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),
                      NNshGlobal->os5HighDensity);
        WinSetCoordinateSystem(kCoordinatesStandard);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

    // �擪�s��work1�ɁA�\���\�s����work2�ɋL������
    (NNshGlobal->NNsiParam)->getLogListPage = startLine;
    NNshGlobal->work2 = nlines;

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openNNsiThread_getLogList                                  */
/*                                                              NNsi�ŊJ�� */
/*-------------------------------------------------------------------------*/
static UInt16 openNNsiThread_getLogList(UInt16 recNum)
{
    Err                  ret;
    UInt16               index;
    Char                 fileName[MAX_THREADFILENAME + MARGIN];
    DmOpenRef            dbRef;
    NNshGetLogDatabase  *dbData;
#ifdef CHECK_GETLOGTHREAD_INFORMATION_STRICTLY
    NNshSubjectDatabase *subjData;
    MemHandle            memH;
#endif

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "OPEN THREAD(recNum:%d)", recNum);
#endif // #ifdef USE_REPORTER


    dbData = MEMALLOC_PTR(sizeof(NNshGetLogDatabase) + MARGIN);
    if (dbData == NULL)
    {
        return (NNSH_DATABASE_BLANK);
    }
    MemSet(dbData, (sizeof(NNshGetLogDatabase) + MARGIN), 0x00);

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        index = NNSH_DATABASE_BLANK;
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "OPEN:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

    ret = GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), dbData);
    if (ret != errNone)
    {
        // �G���[����,�@�I������B
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        CloseDatabase_NNsh(dbRef);
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "GET:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

    // �f�[�^�擾�����A�X�V�����f�[�^�𗎂Ƃ�
    dbData->state = NNSH_SUBJSTATUS_NOT_YET;
    UpdateRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), dbData);

    CloseDatabase_NNsh(dbRef);

    // 1�K�w�����擾����ݒ肾�����ꍇ�B�B�B
    if (dbData->getDepth != 0)
    {
        // �J���L���̈ꗗ����ǂ̋L�����J�����I������
        MemSet(fileName, sizeof(fileName), 0x00);
        index = selectOpenThread_getLogList(dbData, recNum, fileName);
        switch (index)
        {
          case NNSH_DATABASE_BLANK:
          case NNSH_DATABASE_UNKNOWN:
            // �ݒ肹���ɔ�����...
            break;

          default:
            // OPEN����L���̃��R�[�h�ԍ���ݒ肷��
            (NNshGlobal->NNsiParam)->openMsgIndex = index;
            break;
        }
        // �֐��𔲂���...
        goto FUNC_END;
    }

    // �X���f�[�^�x�[�X�̌��������ݒ�
    // �t�@�C�������w�肷��(�f�t�H���g�̖���...)
    MemSet(fileName, sizeof(fileName), 0x00);
    StrCopy(fileName, FILE_LOGCHARGE_PREFIX);
    NUMCATI(fileName, recNum);
    StrCat (fileName, "-0-0");
    StrCat (fileName, FILE_LOGCHARGE_SUFFIX);    

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "FILE:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

    // �X���f�[�^�x�[�X�̃I�[�v��
    dbRef = 0;
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        index = NNSH_DATABASE_BLANK;
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "FILE:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

    // �擾�ς݃��O�����݂��邩�ǂ����̃`�F�b�N
    // (�X���f�[�^�̂c�a�́A�t�@�C�����ł̃\�[�g�����ĂȂ��̂œ����Ȃ��A�A�A)
    if (IsExistRecordRR_NNsh(dbRef, fileName, NNSH_KEYTYPE_CHAR, 0, &index) != errNone)
    {
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "NOT FOUND:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

        // ���R�[�h�������A�I������
        index = NNSH_DATABASE_BLANK;
        CloseDatabase_NNsh(dbRef);
        goto FUNC_END;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "FIND:openNNsiThread_getLogList()");
#endif // #ifdef USE_REPORTER

#ifdef CHECK_GETLOGTHREAD_INFORMATION_STRICTLY
    // ���R�[�h�����擾����
    ret = GetRecordReadOnly_NNsh(dbRef, index, &memH, (void**) &subjData);
    if (ret != errNone)
    {
        index = NNSH_DATABASE_BLANK;
        CloseDatabase_NNsh(dbRef);
        goto FUNC_END;
    }

    // �X����Ԃ̊m�F
    switch (subjData->state)
    {
      case NNSH_SUBJSTATUS_NOT_YET:
      case NNSH_SUBJSTATUS_DELETE:
        // ���擾�̏ꍇ�ɂ́A�I�[�v�����Ȃ�
        index = NNSH_DATABASE_BLANK;
        break;

      default:
        // �擾�ς݃��O�̏ꍇ�ɂ́A�I�[�v������ݒ���s��
        (NNshGlobal->NNsiParam)->openMsgIndex = index;
        (NNshGlobal->msgIndex)->openMsg = 0;
        break;
    }
    // �m�ۂ������R�[�h����������
    ReleaseRecordReadOnly_NNsh(dbRef, memH);
#else
    // �l���Ă݂�΁A�Q�ƃ��O�͎擾�ς݂���Subj-DB�Ɋi�[���Ȃ�...
    (NNshGlobal->NNsiParam)->openMsgIndex = index;
#endif
    CloseDatabase_NNsh(dbRef);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "GOTO VIEW FORM fileName:%s", fileName);
#endif // #ifdef USE_REPORTER

FUNC_END:
    MEMFREE_PTR(dbData);
    return (index);
}

/*-------------------------------------------------------------------------*/
/*   Function : GetLogChargeDB                                             */
/*                                              ���O�f�[�^�x�[�X�f�[�^�̎擾 */
/*-------------------------------------------------------------------------*/
static Err GetLogChargeDB(UInt16 recNum, NNshGetLogDatabase *dbData)
{
    Err                 ret;
    DmOpenRef           dbRef;

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return (~errNone);
    }
    MemSet(dbData, sizeof(NNshGetLogDatabase), 0x00);
    ret = GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), dbData);
    CloseDatabase_NNsh(dbRef);
    if (ret != errNone)
    {
        // �G���[����,�@�I������B
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        /** not return; **/
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :  showSiteInfo_getLogList                                   */
/*                                                         �T�C�g���̕\�� */
/*-------------------------------------------------------------------------*/
static void showSiteInfo_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    Char                logBuf[BIGBUF];

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    ret = GetLogChargeDB(recNum, &dbData);
    if (ret != errNone)
    {
        // �G���[����,�@�I������B
        NNsh_DebugMessage(ALTID_ERROR, "GetLogChargeDB() ", "ret:", ret);
        return;
    }
    MemSet(logBuf, sizeof(logBuf), 0x00);
    if (dbData.wwwcMode == 1)
    {
        StrCat(logBuf, "[WWWC]");
    }
    if (dbData.wwwcMode == 2)
    {
        StrCat(logBuf, "[MDL]");
    }
    if (dbData.rssData != 0)
    {
        StrCat(logBuf, "[RSS]");
    }
    if (dbData.getDepth != 0)
    {
        StrCat(logBuf, "[link]");
    }
    if (dbData.getDateTime[0] != '\0')
    {
        StrCat (logBuf, "\n[");
        StrCat (logBuf, dbData.getDateTime);
        StrCat (logBuf, "]\n---\n");
    }
    StrCat (logBuf, dbData.dataTitle);
    StrCat (logBuf, "\n");
    StrCat (logBuf, dbData.dataURL);
    StrCat (logBuf, "\n\n");
    StrCat (logBuf, dbData.dataFileName);
    StrCat (logBuf, "\n");

    //��ʕ\��    
    NNsh_ErrorMessage(ALTID_INFO, logBuf, "", 0);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : beamSiteURL_getLogList                                     */
/*                                                       URL����BEAM���� */
/*-------------------------------------------------------------------------*/
static void beamSiteURL_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    Char                logBuf[BIGBUF];

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    ret = GetLogChargeDB(recNum, &dbData);
    if (ret != errNone)
    {
        // �G���[����,�@�I������B
        NNsh_DebugMessage(ALTID_ERROR, "GetLogChargeDB() ", "ret:", ret);
        return;
    }

    // BEAM���M�f�[�^�̐���...
    MemSet(logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, NNSI_NNSIEXT_URL_START);
    StrCat (logBuf, dbData.dataURL);
    StrCat (logBuf, "\"");
    StrCat (logBuf, dbData.dataTitle);
    StrCat (logBuf, "\"");
    StrCat (logBuf, NNSI_NNSIEXT_URL_END);

    // �f�[�^�̑��M
    SendToExternalDevice(dbData.dataTitle, logBuf, SOFT_CREATOR_ID);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openWebBrowser_getLogList                                  */
/*                                                       Web�u���E�U�ŊJ�� */
/*-------------------------------------------------------------------------*/
static void openWebBrowser_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    DmOpenRef           dbRef;

    // �Q�ƃ��O�`���[�W�pDB�̎擾

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return;
    }
    MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
    ret = GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), &dbData);
    if (ret != errNone)
    {
        // �G���[����,�@�I������B
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        CloseDatabase_NNsh(dbRef);
        return;
    }

    // �f�[�^�擾�����A�X�V�����f�[�^�𗎂Ƃ�
    dbData.state = NNSH_SUBJSTATUS_NOT_YET;
    UpdateRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), &dbData);
    CloseDatabase_NNsh(dbRef);

    // NetFront���N������(NNsi�I����ɋN��)
    if (NNshGlobal->browserCreator != 0)
    {
        (void) WebBrowserCommand(NNshGlobal->browserCreator, 0, 0, 
                                 NNshGlobal->browserLaunchCode,
                                 dbData.dataURL, MAX_GETLOG_URL, NULL);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openURLwebDA_getLogList                                    */
/*                                                             webDA�ŊJ�� */
/*-------------------------------------------------------------------------*/
static void openURLwebDA_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    DmOpenRef           dbRef;

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    OpenDatabase_NNsh(DBNAME_PERMANENTLOG, DBVERSION_PERMANENTLOG, &dbRef);
    if (dbRef == 0)
    {
        // ���ODB�̃I�[�v�����s�A�I������
        return;
    }
    MemSet(&dbData, sizeof(NNshGetLogDatabase), 0x00);
    ret = GetRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), &dbData);
    if (ret != errNone)
    {
        // �G���[����,�@�I������B
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        CloseDatabase_NNsh(dbRef);
        return;
    }

    // �f�[�^�擾�����A�X�V�����f�[�^�𗎂Ƃ�
    dbData.state = NNSH_SUBJSTATUS_NOT_YET;
    UpdateRecord_NNsh(dbRef, recNum, sizeof(NNshGetLogDatabase), &dbData);
    CloseDatabase_NNsh(dbRef);

    // �N���b�v�{�[�h�ɑI�����Ă���URL���R�s�[����
    ClipboardAddItem(clipboardText, dbData.dataURL, StrLen(dbData.dataURL));

    // webDA���N������
    if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
    {
        // webDA���C���X�g�[������Ă����ꍇ...
        (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openMemoURL_getLogList                                     */
/*                                                MemoURL(MemoURLDA)�ŊJ�� */
/*-------------------------------------------------------------------------*/
static void openMemoURL_getLogList(UInt16 recNum)
{
    Err                 ret;
    NNshGetLogDatabase  dbData;
    Char                logMsg[MAX_GETLOG_URL + MAX_TITLE_LABEL + (MARGIN * 2)];
    UInt16              len;

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "OPEN MEMO URL(recNum:%d)", recNum);
#endif // #ifdef USE_REPORTER

    // �Q�ƃ��O�`���[�W�pDB�̎擾
    ret = GetLogChargeDB(recNum, &dbData);
    if (ret != errNone)
    {
        // �G���[����,�@�I������B
        NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh() ", "ret:", ret);
        return;
    }

    if (CheckInstalledResource_NNsh('DAcc', 'mmuA') != false)
    {
        // MemoURLDA���C���X�g�[������Ă����ꍇ...
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, dbData.dataTitle);
        len = StrLen(logMsg);
        StrCopy(&logMsg[len + 1], dbData.dataURL);
        len  = len + 1 + StrLen(dbData.dataURL);

        // �N���b�v�{�[�h�ɑI������URL���R�s�[����
        ClipboardAddItem(clipboardText, logMsg, len);

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " COPIED URL :%s(len:%d)", logMsg, len);
#endif // #ifdef USE_REPORTER

        NNsh_DebugMessage(ALTID_INFO, "SEND to MemoURLDA URL:", dbData.dataURL, 0);

        (void) LaunchResource_NNsh('DAcc','mmuA','code',1000);
        return;
    }
    NNsh_DebugMessage(ALTID_INFO, "SEND to MemoURL URL:", dbData.dataURL, 0);

    // URL��MemoURL�֑���
    if (CheckInstalledResource_NNsh('appl', 'asUR') == false)
    {
        // MemoURL���C���X�g�[������Ă��Ȃ������A��������������
        return;
    }

    MemSet(logMsg, sizeof(logMsg), 0x00);
    logMsg[0] = '1';
    StrCopy(&logMsg[2], dbData.dataTitle); // �X���^�C���^�C�g����...
    len = StrLen(&logMsg[2]) + 2;
    StrCopy(&logMsg[len + 1], dbData.dataURL);
    len = len + 1 + StrLen(dbData.dataURL);

    // �w�肳�ꂽURL��MemoURL�ɑ���
    (void) WebBrowserCommand('asUR', 0, 0, 0x8001, logMsg, len, NULL);
    return;
}


/*-------------------------------------------------------------------------*/
/*   Function : checkGadget_getLogList                                     */
/*                                                �^�b�v�����̈�̃`�F�b�N */
/*-------------------------------------------------------------------------*/
static UInt16 checkGadget_getLogList(EventType *event, FormType *frm, RectangleType *rect)
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

    if ((selItem + (NNshGlobal->NNsiParam)->getLogListPage) >= NNshGlobal->work1)
    {
        // �͈͊O�̑I���������ꍇ�A�������Ȃ�
        return (0);
    }

    if (selItem == (NNshGlobal->NNsiParam)->getLogListSelection)
    {
        // �_�u���^�b�v�����o�AWeb�u���E�U�ŊJ�����Ƃ���
        return (vchrJogRelease);
    }

    // �I���A�C�e�����ړ�������
    (NNshGlobal->NNsiParam)->getLogListSelection = selItem;
    return (chrCapital_R);
}

/*=========================================================================*/
/*   Function :   Handler_GetLogList                                       */
/*                                �Q�ƃ��O�̎擾�ꗗ��ʂ̃C�x���g�n���h�� */
/*=========================================================================*/
Boolean Handler_GetLogList(EventType *event)
{
    FormType     *frm;
    RectangleType dimF;
    UInt16        itemId, feature;

    // �{�^���`�F�b�N�ȊO�̃C�x���g�͑��߂�
    itemId = 0;
    switch (event->eType)
    {
      case popSelectEvent:
        // �|�b�v�A�b�v�g���K�[�ύX
        itemId = event->data.popSelect.listID;
        break;

      case menuEvent:
        // ���j���[�I��
        itemId = event->data.menu.itemID;
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ(���֐i��)
        itemId = event->data.ctlSelect.controlID;
        break;

      case ctlRepeatEvent:
        // ���s�[�g�{�^���������ꂽ(���֐i��)
        itemId = event->data.ctlRepeat.controlID;
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent:
        // ��ʂ̉�]
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
        }
        return (true);
        break;
#endif  // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        // ��ʂ̃T�C�Y�ύX
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
            }
        }
        return (true);
        break;
#endif

      case keyDownEvent:
        // �L�[���͂�������
        itemId = KeyConvertFiveWayToJogChara(event);
        break;

      case penDownEvent:
        // ��ʃ^�b�v�����m
        frm = FrmGetActiveForm();
        FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_TAPPED),&dimF);
        if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
        {
            // Gadget�����^�b�v����(== �X����I������)
            itemId = checkGadget_getLogList(event, frm, &dimF);
            break;
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
                itemId = BTNID_LISTNEXT;
                break;
            }

            // �O�y�[�W�{�^���������ꂽ���`�F�b�N
            FrmGetObjectBounds(frm, 
                               FrmGetObjectIndex(frm, BTNID_LISTPREV),&dimF);
            if (RctPtInRectangle(event->screenX, event->screenY, &dimF))
            {
                // �O�y�[�W�{�^���������ꂽ���Ƃɂ���
                itemId = BTNID_LISTPREV;
                break;
            }

            // �X����I���������Ƃɂ���
            FrmGetObjectBounds(frm, 
                           FrmGetObjectIndex(frm, GADID_MESSAGE_LIST),&dimF);
            itemId = checkGadget_getLogList(event, frm, &dimF);
            break;
        }
        return (false); 
        break;

      default: 
        return (false);
        break;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "::GetLog:: itemId:%d[work1:%d work2:%d work3:%d]",
                      itemId, (NNshGlobal->NNsiParam)->getLogListPage, NNshGlobal->work2, (NNshGlobal->NNsiParam)->getLogListSelection);
#endif // #ifdef USE_REPORTER

    // �R�}���h�����s����
    switch (itemId)
    {
      case chrCapital_U:
      case chrSmall_U:
      case MNUID_SITE_INFO:
        // �T�C�g���̕\��
        feature = NNSH_GETLOG_FEATURE_SITE_INFORMATION;
        break;

      case chrCapital_C:
      case chrSmall_C:
      case MNUID_CONFIG_GETLOG:
        // �Q�ƃ��O�ꗗ�ݒ��ʂ��J��
        feature = NNSH_GETLOG_FEATURE_CONFIG_GETLOG;
        break;

      case BTNID_BUTTON1:
      case chrDigitOne:
        // NNsi�ŊJ��
        feature = NNSH_GETLOG_FEATURE_OPEN_NNSI;
        break;

      case chrCapital_Z:  // Z�L�[�̓���
      case chrSmall_Z:
      case MNUID_BACK:
      case SELTRID_BACK:
        // �ꗗ��ʂ��J��
        feature = NNSH_GETLOG_FEATURE_CLOSE_GETLOG;
        break;

      case chrDigitFour:
      case BTNID_BUTTON4:
      case MNUID_ENTRY:
        // �T�C�g�̓o�^
        feature = NNSH_GETLOG_FEATURE_ENTRY_NEWSITE;
        break;
         
      case chrDigitFive:
      case BTNID_BUTTON5:
      case MNUID_SETSEPA:
        // ��؂�̐ݒ��ʂ��J��
        feature = NNSH_GETLOG_FEATURE_EDIT_SEPARATOR;
        break;

      case chrDigitSix:
      case BTNID_BUTTON6:
      case chrSpace:
      case chrSJISIdeographicSpace:
      case MNUID_GETLOGOPE:
        // �V���m�F���{
        feature = NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL;
        break;

      case MNUID_SITE_DETAIL:
      case chrDigitThree:
      case chrCapital_E:  // E�L�[�̓���
      case chrSmall_E:
      case BTNID_BUTTON3:
        // �f�[�^�ҏW
        feature = NNSH_GETLOG_FEATURE_EDITITEM;
        break;

      case vchrJogBack:
      case chrBackspace:
      case chrEscape:
      case vchrThumbWheelBack:
      case vchrGarminEscape:
        // Jog Back
        feature = (NNshGlobal->NNsiParam)->getLogFeature.jogBack;
        break;

      case vchrJogRelease:
      case chrCarriageReturn:
      case chrLineFeed:
      case vchrRockerCenter:
      case vchrThumbWheelPush:
        // Jog Push
        feature = (NNshGlobal->NNsiParam)->getLogFeature.jogPush;
        break;

      case chrDigitTwo:
      case chrCapital_W:  // W�L�[�̓���
      case chrSmall_W:
      case BTNID_BUTTON2:
        // Web�u���E�U�ŊJ��
        feature = NNSH_GETLOG_FEATURE_OPENWEB;
        break;

      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrPrevField:
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case chrUpArrow:
        // up
        feature = (NNshGlobal->NNsiParam)->getLogFeature.up;
        break;

      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrNextField:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case chrDownArrow:
        // down
        feature = (NNshGlobal->NNsiParam)->getLogFeature.down;
        break;

      case chrLeftArrow:         // �J�[�\���L�[��(5way��)
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
        feature = (NNshGlobal->NNsiParam)->getLogFeature.left;
        break;

      case chrCapital_P:  // P�L�[�̓���
      case chrSmall_P:
      case BTNID_LISTPREV:
        // �O�y�[�W
        feature = NNSH_GETLOG_FEATURE_PREVPAGE;
        break;

      case chrRightArrow:         // �J�[�\���L�[�E(5way�E)
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
        feature = (NNshGlobal->NNsiParam)->getLogFeature.right;
        break;

      case chrCapital_N:  // N�L�[�̓���
      case chrSmall_N:
      case BTNID_LISTNEXT:
        // ���y�[�W
        feature = NNSH_GETLOG_FEATURE_NEXTPAGE;
        break;

      case chrCapital_T:       // T�L�[�̓���
      case chrSmall_T:
        // �擪����\��
        feature = NNSH_GETLOG_FEATURE_TOP;
        break;

      case chrCapital_B:  // B�L�[�̓���
      case chrSmall_B:
        // ��������\��
        feature = NNSH_GETLOG_FEATURE_BOTTOM;
        break;

      // Q�L�[����
      case chrCapital_Q:
      case chrSmall_Q:
      case MNUID_NET_DISCONN:
        // ����ؒf
        feature = NNSH_GETLOG_FEATURE_DISCONNECT;
        break;

      case chrDigitSeven:
        // �L�[7 (Bt ON)
        feature = NNSH_GETLOG_FEATURE_BT_ON;
        break;

      case chrDigitEight:
        // �L�[8 (Bt Off)
        feature = NNSH_GETLOG_FEATURE_BT_OFF;
        break;

      case LSTID_GETLOGLEVEL:
        // �|�b�v�A�b�v�g���K�[�X�V
        feature = NNSH_GETLOG_FEATURE_CREATELIST;
        break;

      // L�L�[����
      case chrCapital_L:
      case chrSmall_L:
        // ���񃌃x����I��
        feature = NNSH_GETLOG_FEATURE_OPEN_GETLEVEL;
        break;

      // ?�L�[����
      case chrQuestionMark:
      case MNUID_NNSI_END:
        // NNsi���I��������
        feature = NNSH_GETLOG_FEATURE_NNSIEND;
        break;

      // V�L�[����
      case chrCapital_V:
      case chrSmall_V:
      case MNUID_SHOW_VERSION:
        // �o�[�W�������\��
        feature = NNSH_GETLOG_FEATURE_SHOWVERSION;
        break;

      // R�L�[����
      case chrCapital_R:
      case chrSmall_R:
        // ��ʂ̍ĕ`��
        feature = NNSH_GETLOG_FEATURE_REDRAW;
        break;

      // A�L�[����
      case chrCapital_A:
      case chrSmall_A:
      case MNUID_NET_CONFIG:
        // �l�b�g���[�N�ݒ���J��
        feature = NNSH_GETLOG_FEATURE_NETCONFIG;
        break;

      // I�L�[����
      case chrCapital_I:
      case chrSmall_I:
      case MNUID_SHOW_DEVICEINFO:
        // �f�o�C�X�̏��\��
        feature = NNSH_GETLOG_FEATURE_DEVICEINFO;
        break;

      // !�L�[����
      case chrExclamationMark:
      case MNUID_GOTO_DUMMY:
        // �_�~�[��ʂ֑J��
        feature = NNSH_GETLOG_FEATURE_OPEN_DUMMY;
        break;

      case MNUID_SITELOG_DELETE:
      case chrCapital_D:
      case chrSmall_D:
        // ���O(�ꊇ)�폜
        feature = NNSH_GETLOG_FEATURE_DELETELOG;
        break;

      case MNUID_SITELOG_DELETE_READ:
        // ���ǃ��O(�ꊇ)�폜
        feature = NNSH_GETLOG_FEATURE_DELETELOG_READ;
        break;

      case chrDigitNine:
      case MNUID_CONFIG_NNSH:
        // NNsi�ݒ���J��
        feature = NNSH_GETLOG_FEATURE_OPEN_NNSISET;
        break;

      case chrCapital_H:
      case chrSmall_H:
      case MNUID_SHOW_HELP:
        feature = NNSH_GETLOG_FEATURE_HELP;
        break;

      case chrCapital_X:
      case chrSmall_X:
        // �I�����j���[
        feature = NNSH_GETLOG_FEATURE_OPENMENU;
        break;

#if 0
      case vchrCapture:
      case vchrVoiceRec:
      case vchrVZ90functionMenu:      
      case vchrGarminRecord:
#endif
      case vchrHard5:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // CLIE�L���v�`���[�{�^�����������Ƃ��̐ݒ�
        feature = (NNshGlobal->NNsiParam)->getLogFeature.clieCapture;
        break;

      case vchrHard1:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // �n�[�h�L�[�P���������Ƃ��̐ݒ�
        feature = (NNshGlobal->NNsiParam)->getLogFeature.key1;
        break;

      case vchrHard2:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // �n�[�h�L�[�Q���������Ƃ��̐ݒ�
        feature = (NNshGlobal->NNsiParam)->getLogFeature.key2;
        break;

      case vchrHard3:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // �n�[�h�L�[�R���������Ƃ��̐ݒ�
        feature = (NNshGlobal->NNsiParam)->getLogFeature.key3;
        break;

      case vchrHard4:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // �n�[�h�L�[�S���������Ƃ��̐ݒ�
        feature = (NNshGlobal->NNsiParam)->getLogFeature.key4;
        break;

      case chrCapital_S:
      case chrSmall_S:
      case MNUID_BEAM_URL:
        // �t�q�k���M
        feature = NNSH_GETLOG_FEATURE_BEAM_URL;
        break;

      case chrCapital_Y:
      case chrSmall_Y:
      case MNUID_OPEN_WEBDA:
        // webDA�ŊJ��
        feature = NNSH_GETLOG_FEATURE_OPENWEBDA;
        break;

#ifdef USE_PIN_DIA
      case vchrTT3Short:
      case vchrTT3Long:
        // ��ʂ̃T�C�Y�ύX
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            if (HandEraResizeForm(FrmGetActiveForm()) == true)
            {
                RefreshDisplay_getLogList(FrmGetActiveForm(), NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
            }
        }
        return (true);
        break;
#endif

      default:
        // ��L�ȊO
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "<>UNKNOWN(%d)<>", itemId);
#endif // #ifdef USE_REPORTER
        return (false);
        break;
    }

EXEC_FEATURE:
    Execute_GetLogList(feature);
    switch (feature)
    {
      case NNSH_GETLOG_FEATURE_CONFIG_GETLOG:
      case NNSH_GETLOG_FEATURE_EDIT_SEPARATOR:
        // ��ʕ\�������ĕ`�悷��
        // (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        Execute_GetLogList(NNSH_GETLOG_FEATURE_CREATELIST);
        break;

      case NNSH_GETLOG_FEATURE_EDITITEM:
      case NNSH_GETLOG_FEATURE_ENTRY_NEWSITE:
        // ��ʕ\�������N���A����
        (NNshGlobal->NNsiParam)->getLogListPage = 0;
        NNshGlobal->work2 = 0;
        (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        Execute_GetLogList(NNSH_GETLOG_FEATURE_CREATELIST);
        break;

      case NNSH_GETLOG_FEATURE_CREATELIST:
      case NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL:
        // 
        return (false);
        break;

      case NNSH_GETLOG_FEATURE_OPENMENU:
        // �I�����j���[���J�����ꍇ�ɂ́A�ēx�@�\�����s����B
        if (NNshGlobal->jumpSelection != NNSH_GETLOG_FEATURE_NONE)
        {
            // 
            feature = NNshGlobal->jumpSelection;
            goto EXEC_FEATURE;
        }
        break;

      default:
        // �������Ȃ�
        break;
    }
    return (true);
}

/*=========================================================================*/
/*   Function :  Execute_GetLogList                                        */
/*                            �Q�ƃ��O��ʂŎ��s�\�ȋ@�\�̈ꗗ(�@�\����) */
/*=========================================================================*/
Boolean Execute_GetLogList(UInt16 feature)
{
    FormType     *frm;
    EventType    *dummyEvent;
    UInt16        selItem, savedParam, savedParam2;
    ListType     *lstP;

    frm = FrmGetActiveForm();
    switch (feature)
    {
      case NNSH_GETLOG_FEATURE_CLOSE_GETLOG:
        // �ꗗ��ʂ��J��
        CloseForm_GetLogList(FRMID_THREAD);
        return (false);
        break;

      case NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL:
        // �V���m�F���{
        selItem = LstGetSelection(FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, LSTID_GETLOGLEVEL)));
        if (selItem > 1)
        {
            // ���X�g�ԍ������񃌃x���ɒ�������
            selItem--;
        }
        savedParam = (NNshGlobal->NNsiParam)->getROLogLevel;

        // ���񃌃x�����^�u�ɍ��킹�ĕύX����
        (NNshGlobal->NNsiParam)->getROLogLevel = selItem;

		// �V���m�F���ɂ́A�_�C�A���O��\�����Ȃ��悤�ɂ���
        savedParam2 = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

        // �V���m�F�O�ɂc�`�����{����ݒ肾�����ꍇ...
        if ((NNshGlobal->NNsiParam)->preOnDAnewArrival != 0)
        {
            // DA���N������
            LaunchDA_NNsh(((NNshGlobal->NNsiParam)->prepareDAforNewArrival));
        }

        // �Q�ƃ��O�̎擾���s
        GetReadOnlyLogData_NNsh();

        // ���񃌃x�������ɖ߂�
        (NNshGlobal->NNsiParam)->getROLogLevel = savedParam;

        // �f�[�^����蒼���Ă݂�
        lstP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_GETLOGLEVEL));
        selItem = LstGetSelection(lstP);
        Show_BusyForm(MSG_FINDING_WAIT);
        ClearList_getLogList (NNshGlobal->dispList);
        createList_getLogList(selItem, NNshGlobal->dispList);
        Hide_BusyForm(false);

        // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
        if (NNshGlobal->palmOSVersion < 0x03503000)
        {
            FrmEraseForm(FrmGetActiveForm());
            FrmDrawForm(FrmGetActiveForm());
        }

        // ��ʂ̍ĕ`������{����
        (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        RefreshDisplay_getLogList(frm, NNSH_STEP_TO_TOP, (NNshGlobal->NNsiParam)->getLogListSelection);

        // NNsi�ݒ�-9�̃p�����[�^�𗬗p���āA�V���m�F��̏��������{����...

#ifdef USE_MACRO
        if ((NNshGlobal->NNsiParam)->startMacroArrivalEnd != 0)
        {
            // �V���m�F�I�����Ƀ}�N�������s����
            NNsi_ExecuteMacroMain(DBNAME_MACROSCRIPT, DBVERSION_MACROSCRIPT, SOFT_CREATOR_ID, SOFT_DBTYPE_ID);
        }
#endif

        if ((NNshGlobal->NNsiParam)->use_DAplugin != 0)
        {
            // �V���m�F�I�����Ƀ��[���`�F�b�N�̊m�F�����{(cMDA�N��)
            (void) LaunchResource_NNsh('DAcc','cMDA','code',1000);
        }

        if ((NNshGlobal->NNsiParam)->disconnArrivalEnd != 0)
        {
            // �u�V���m�F�v�I�����ɉ���ؒf
            NNshNet_LineHangup();
        }

        // �u�V���m�F�v��Ƀr�[�v����炵�Ă݂�
        if ((NNshGlobal->NNsiParam)->autoBeep != 0)
        {
            NNsh_BeepSound(sndAlarm);
        }

		// �_�C�A���O�\���ݒ�����ɖ߂�
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam2;
        break;

      case NNSH_GETLOG_FEATURE_EDITITEM:
        // �T�C�g�f�[�^�̕ҏW
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " EDIT RECORD:%d",
                          ((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
#endif // #ifdef USE_REPORTER
        SetLogGetURL(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_PREVITEM:
        // �ЂƂ�I��
        if ((NNshGlobal->NNsiParam)->getLogListSelection > 0)
        {
            (NNshGlobal->NNsiParam)->getLogListSelection--;
            RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
        }
        else
        {
            if ((NNshGlobal->NNsiParam)->getLogListPage == 0)
            {
                // �O�y�[�W���Ȃ��ꍇ�ɂ͏I��
                return (true);
            }
            // �O�y�[�W
            (NNshGlobal->NNsiParam)->getLogListSelection = (NNshGlobal->work2 - 1);
            RefreshDisplay_getLogList(frm, NNSH_STEP_PAGEUP, (NNshGlobal->NNsiParam)->getLogListSelection);
        }
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "<>Jog UP<>");
#endif // #ifdef USE_REPORTER
        break;

      case NNSH_GETLOG_FEATURE_NEXTITEM:
        // �ЂƂ��I��
        if (((NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage) >= (NNshGlobal->work1 - 1))
        {
            // �z���Ă����A�J�[�\���𖖔��ɂ��킹��
            (NNshGlobal->NNsiParam)->getLogListSelection = NNshGlobal->work1 - (NNshGlobal->NNsiParam)->getLogListPage - 1;
            RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);

            NNsh_DebugMessage(ALTID_INFO, "<>ADJUST<>", " ", (NNshGlobal->NNsiParam)->getLogListSelection);
            NNsh_DebugMessage(ALTID_INFO, "<>ADJUST<>", " work1: ", NNshGlobal->work1);
            return (true);
        }
        if ((NNshGlobal->NNsiParam)->getLogListSelection < (NNshGlobal->work2 - 1))
        {
            (NNshGlobal->NNsiParam)->getLogListSelection++;
            RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
            NNsh_DebugMessage(ALTID_INFO, "<>REDRAW<>", " ", (NNshGlobal->NNsiParam)->getLogListSelection);
            NNsh_DebugMessage(ALTID_INFO, "<>REDRAW<>", " work2:", NNshGlobal->work2);
        }
        else
        {
            if (((NNshGlobal->NNsiParam)->getLogListSelection == (NNshGlobal->work2 - 1))&&
                ((NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage == NNshGlobal->work1))
            {
                // ���y�[�W���Ȃ��ꍇ�ɂ͏I��
                return (true);
            }
            // ���y�[�W�\��
            (NNshGlobal->NNsiParam)->getLogListSelection = 0;
            RefreshDisplay_getLogList(frm, NNSH_STEP_PAGEDOWN, (NNshGlobal->NNsiParam)->getLogListSelection);
        }
        NNsh_DebugMessage(ALTID_INFO, "<>Jog DOWN<>", " ", (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_PREVPAGE:
        // �O�y�[�W
        RefreshDisplay_getLogList(frm, NNSH_STEP_PAGEUP, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_NEXTPAGE:
        // ��y�[�W
        RefreshDisplay_getLogList(frm, NNSH_STEP_PAGEDOWN, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_TOP:
        // �ꗗ�̐擪��
        (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        RefreshDisplay_getLogList(frm, NNSH_STEP_TO_TOP, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_BOTTOM:
        // �ꗗ�̖�����
        (NNshGlobal->NNsiParam)->getLogListSelection = (NNshGlobal->work2 - 1);
        RefreshDisplay_getLogList(frm, NNSH_STEP_TO_BOTTOM, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_CREATELIST:
        // �ꗗ���č\�z����
        lstP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_GETLOGLEVEL));
        selItem = LstGetSelection(lstP);
        Show_BusyForm(MSG_FINDING_WAIT);

        // ���񃌃x���̃p�����[�^�ۑ�
        if (selItem == 0)
        {
            (NNshGlobal->NNsiParam)->getROLogLevel = 0;
        }
        else
        {
            (NNshGlobal->NNsiParam)->getROLogLevel = selItem - 1;
        }
        ClearList_getLogList (NNshGlobal->dispList);
        createList_getLogList(selItem, NNshGlobal->dispList);
        Hide_BusyForm(false);
        if (NNshGlobal->palmOSVersion < 0x03503000)
        {
            FrmEraseForm(FrmGetActiveForm());
            FrmDrawForm(FrmGetActiveForm());
        }
        (NNshGlobal->NNsiParam)->getLogListSelection = 0;
        RefreshDisplay_getLogList(frm, NNSH_STEP_TO_TOP, (NNshGlobal->NNsiParam)->getLogListSelection);
        break;

      case NNSH_GETLOG_FEATURE_EDIT_SEPARATOR:
        // ��؂�ݒ��ʂ��J��
        SetLogCharge_LogToken(0);
        break;

      case NNSH_GETLOG_FEATURE_ENTRY_NEWSITE:
        // �V�K�o�^��ʂ��J��
        SetLogGetURL(NNSH_ITEM_LASTITEM);
        break;

      case NNSH_GETLOG_FEATURE_OPEN_GETLEVEL:
        // �V���m�F���x�����J��
        CtlHitControl(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, POPTRID_GETLOGLEVEL)));
        break;

      case NNSH_GETLOG_FEATURE_REDRAW:
        // ��ʂ̍ĕ`��
        RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, "<>REDRAW<>REDRAW<>");
#endif // #ifdef USE_REPORTER
        break;

      case NNSH_GETLOG_FEATURE_OPENWEB:
        // Web�u���E�U�ŊJ��
        openWebBrowser_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_BT_ON:
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(true);
#endif  // #ifdef USE_BT_CONTROL

      case NNSH_GETLOG_FEATURE_BT_OFF:
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(false);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case NNSH_GETLOG_FEATURE_DISCONNECT:
        // ����ؒf
        NNshNet_LineHangup();
        break;

      case NNSH_GETLOG_FEATURE_NETCONFIG:
        // �l�b�g���[�N�ݒ��ʂ��J��
        OpenNetworkPreferences();
        break;

      case NNSH_GETLOG_FEATURE_DEVICEINFO:
        // �f�o�C�X����\������
        ShowDeviceInfo_NNsh();
        break;

      case NNSH_GETLOG_FEATURE_OPEN_DUMMY:
        // �_�~�[��ʂ��J��
        CloseForm_GetLogList(FRMID_NNSI_DUMMY);
        return (false);
        break;

      case NNSH_GETLOG_FEATURE_OPEN_NNSISET:
        // NNsi�ݒ��ʂ��J��
        CloseForm_GetLogList(FRMID_NNSI_SETTING_ABSTRACT);
        return (false);
        break;

      case NNSH_GETLOG_FEATURE_SHOWVERSION:
        // �o�[�W�������̕\��
        ShowVersion_NNsh();
        break;

      case NNSH_GETLOG_FEATURE_HELP:
        // �w���v�\��
        // (�N���b�v�{�[�h�ɁAnnDA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����)
        ClipboardAddItem(clipboardText, 
                         nnDA_NNSIEXT_VIEWSTART
                         nnDA_NNSIEXT_INFONAME
                         nnDA_NNSIEXT_HELPGETLOG
                         nnDA_NNSIEXT_ENDINFONAME
                         nnDA_NNSIEXT_ENDVIEW,
                         sizeof(nnDA_NNSIEXT_VIEWSTART
                                 nnDA_NNSIEXT_INFONAME
                                 nnDA_NNSIEXT_HELPGETLOG
                                 nnDA_NNSIEXT_ENDINFONAME
                                 nnDA_NNSIEXT_ENDVIEW));
        // nnDA���N������
        (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        break;

     case NNSH_GETLOG_FEATURE_NNSIEND:
        // NNsi���I��������
        dummyEvent = &(NNshGlobal->dummyEvent);
        MemSet(dummyEvent, sizeof(EventType), 0x00);
        dummyEvent->eType = appStopEvent;
        EvtAddEventToQueue(dummyEvent);
        break;

      case NNSH_GETLOG_FEATURE_OPENMENU:
        // �I�����j���[��\��
        NNshGlobal->jumpSelection = selectionMenu_getLogList();
        break;

      case NNSH_GETLOG_FEATURE_FORCE_DISCONNECT:
        // ����ؒf���̂Q
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
        NNshNet_LineHangup();
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
        break;

      case NNSH_GETLOG_FEATURE_OPEN_GETLOGMENU:
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

      case NNSH_GETLOG_FEATURE_OPEN_MEMOURL:
        // MemoURL�ŊJ��
        openMemoURL_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_CONFIG_GETLOG:
        // �Q�ƃ��O�ꗗ��ʂ̃n�[�h�L�[�ڍאݒ�����{����
        GetLog_SetHardKeyFeature();
        break;

      case NNSH_GETLOG_FEATURE_OPEN_NNSI:
        // �擾���O��NNsi�ŊJ��
        selItem = openNNsiThread_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        if (selItem == NNSH_DATABASE_UNKNOWN)
        {
            // �擾���O�\�����L�����Z������...��������������
            break;
        }
        else if (selItem != NNSH_DATABASE_BLANK)
        {
            // ���O�͎擾����Ă����A�A�A�Q�Ɖ�ʂ��J��
            CloseForm_GetLogList(FRMID_MESSAGE);
            return (true);
        }
        // ���O���擾�ł��Ă��Ȃ��AWeb�u���E�U�ŊJ�����m�F����
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_OPENWEB, "", 0) == 0)
        {
            // Web�u���E�U�ŊJ��
            openWebBrowser_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        }
        break;

      case NNSH_GETLOG_FEATURE_SITE_INFORMATION:
        // �T�C�g����\������
        showSiteInfo_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_BEAM_URL:
        // �T�C�g����Beam����
        beamSiteURL_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_OPENWEBDA:
        // �T�C�g��webDA�ŊJ��
        openURLwebDA_getLogList(((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK);
        break;

      case NNSH_GETLOG_FEATURE_DELETELOG:
        // ���O���ꊇ�폜
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_DELETELOGS, "", 0) == 0)
        {
            // �m�F���Ƃꂽ�Ƃ��ɂ̓��O�폜
            selItem = deleteAllThread_getLogList((((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK), false);
            if (selItem != 0)
            {
                // �폜��A�폜�������O��������������
                NNsh_InformMessage(ALTID_INFO, "", MSG_DELETED_LOGFILES, selItem);
            }
            else
            {
                // �폜���ׂ����O��������Ȃ������Ƃ�...
                NNsh_InformMessage(ALTID_INFO, "", MSG_NOT_DELETED_LOGFILE, 0);                
            }
        }
        break;

      case NNSH_GETLOG_FEATURE_DELETELOG_READ:
        // ���ǃ��O���ꊇ�폜
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_DELETELOGS_READ, "", 0) == 0)
        {
            // �m�F���Ƃꂽ�Ƃ��ɂ̓��O�폜
            selItem = deleteAllThread_getLogList((((NNshGlobal->dispList)->recNum[(NNshGlobal->NNsiParam)->getLogListSelection + (NNshGlobal->NNsiParam)->getLogListPage]) & NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK), true);
            if (selItem != 0)
            {
                // �폜��A�폜�������O��������������
                NNsh_InformMessage(ALTID_INFO, "", MSG_DELETED_LOGFILES, selItem);
            }
            else
            {
                // �폜���ׂ����O��������Ȃ������Ƃ�...
                NNsh_InformMessage(ALTID_INFO, "", MSG_NOT_DELETED_LOGFILE, 0);                
            }
        }
        break;

      default:
        // �m��Ȃ��@�\�ԍ����w�肳�ꂽ
        NNsh_DebugMessage(ALTID_INFO, "Unknown Feature Number :", "", feature);
        return (false);
        break;
    }
    // �K��redraw���Ă݂�...
    RefreshDisplay_getLogList(frm, NNSH_STEP_REDRAW, (NNshGlobal->NNsiParam)->getLogListSelection);
    return (true);
}

/*=========================================================================*/
/*   Function :   clearList_getLogList                                     */
/*                              �Q�ƃ��O�̎擾�ꗗ�\���p�f�[�^���N���A���� */
/*=========================================================================*/
void ClearList_getLogList(NNshDispList *dispList)
{
    UInt16 lp;

    for (lp = 0; lp < dispList->dataCount; lp++)
    {
        MEMFREE_PTR(dispList->title[lp]);
    }
    dispList->dataCount = 0;
    return;
}


/*=========================================================================*/
/*   Function :   CloseForm_GetLogList                                     */
/*                                          �Q�ƃ��O�̎擾�ꗗ��ʂ���� */
/*=========================================================================*/
Err CloseForm_GetLogList(UInt16 nextFormId)
{

    // ���Ƃ��܂�...
    ClearList_getLogList(NNshGlobal->dispList);
    MEMFREE_PTR(NNshGlobal->dispList);

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, "CLOSE getloglist next: %d ", nextFormId);
#endif // #ifdef USE_REPORTER

    if (nextFormId != 0)
    {
        // ���̉�ʂ��J��...
        FrmGotoForm(nextFormId);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function :   OpenForm_GetLogList                                      */
/*                                            �Q�ƃ��O�̎擾�ꗗ��ʂ��J�� */
/*=========================================================================*/
Err OpenForm_GetLogList(FormType  *frm)
{
    UInt16 update;

    // ���[�N�̈���m�ۂ���
    NNshGlobal->dispList = MEMALLOC_PTR(sizeof(NNshDispList) + MARGIN);
    if (NNshGlobal->dispList == NULL)
    {
        // ���[�N�̈�̊m�ێ��s...
        return (~errNone);
    }
    MemSet(NNshGlobal->dispList, (sizeof(NNshDispList) + MARGIN), 0x00);

    // �ꗗ��(�ݒ胍�O���x���ɍ��킹)�X�V����
    if ((NNshGlobal->NNsiParam)->getROLogLevel == 0)
    {
        update = 0;
    }
    else
    {
         update = (NNshGlobal->NNsiParam)->getROLogLevel + 1;
    }

    // ��ʂɕ\������f�[�^���\�z����
    Show_BusyForm(MSG_FINDING_WAIT);
    createList_getLogList(update, NNshGlobal->dispList);
    Hide_BusyForm(false);

    // work1 �́A�\������f�[�^�̃A�C�e�����Awork2�́A�\���\���C�������i�[����
    // (NNshGlobal->NNsiParam)->getLogListPage      = 0;
    // (NNshGlobal->NNsiParam)->getLogListSelection = 0;
    NNshGlobal->work1 = 0;
    NNshGlobal->work2 = 0;

    RefreshDisplay_getLogList(frm, NNSH_STEP_NEWOPEN, (NNshGlobal->NNsiParam)->getLogListSelection);

    // �J�����t�H�[��ID��ݒ肷��(������ݒ蒆�ɂ́AlastFrmID�͕ύX���Ȃ�)
    if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
    {
        // ������ݒ蒆�Ȃ̂ŁA�������Ȃ�
    }
    else
    {
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_GETLOGLIST;
    }
    NNshGlobal->backFormId = FRMID_LISTGETLOG;
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectOpenThread_getLogList                                */
/*                                          �ꗗ����ǂ݂����L����I������ */
/*-------------------------------------------------------------------------*/
static UInt16 selectOpenThread_getLogList(NNshGetLogDatabase *dbData, UInt16 recNum, Char *fileName)
{
    Err                  ret;
    UInt16               index, dbCount, loop, itemCnt;
    UInt16              *recArray;
    Char                *selLabel, *labelPtr;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subjData;
    MemHandle            memH;

    // �ϐ��̏�����
    index    = NNSH_DATABASE_BLANK;
    recArray = NULL;
    selLabel = NULL;

    // �t�@�C�����̐擪��p�ӂ���
    StrCopy(fileName, FILE_LOGCHARGE_PREFIX);
    NUMCATI(fileName, recNum);
    StrCat (fileName, "-");

    // ���R�[�h�ԍ��i�[�p�̃G���A���m�ۂ���
    loop = (sizeof(UInt16) * (MAX_URLLIST_CNT + MARGIN + 1));
    recArray = MEMALLOC_PTR(loop + MARGIN);
    if (recArray == NULL)
    {

        return (NNSH_DATABASE_UNKNOWN);
    }
    MemSet(recArray, (loop + MARGIN), 0x00);
    
    // �I�����x���p�̃G���A���m�ۂ���
    loop = (sizeof(Char) * (MAX_NAMELABEL + MARGIN) * (MAX_URLLIST_CNT + MARGIN + 1));
    selLabel = MEMALLOC_PTR(loop + MARGIN);
    if (selLabel == NULL)
    {
        // �I�����x���p�̗̈�m�ێ��s...�I������
        MEMFREE_PTR(recArray);
        return (NNSH_DATABASE_UNKNOWN);
    }
    MemSet(selLabel, (loop + MARGIN), 0x00);

    // �X���f�[�^�x�[�X�̃I�[�v��
    dbRef = 0;
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �X��DB�̃I�[�v�����s�A�I������
        MEMFREE_PTR(selLabel);
        MEMFREE_PTR(recArray);
        return (NNSH_DATABASE_UNKNOWN);
    }

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &dbCount);

    // BUSY�E�B���h�E��\������(URL�m�F��)
    Show_BusyForm(MSG_CHECK_URLLIST);
        
    itemCnt  = 0;
    labelPtr = selLabel;

    // ���R�[�h�ԍ��̎Ⴂ�ق����珇�ɕ\������...
    for (loop = dbCount; loop != 0; loop--)
    {
        // ���R�[�h�����擾����
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &memH, (void**) &subjData);
        if (ret == errNone)
        {
            if ((subjData->state != NNSH_SUBJSTATUS_NOT_YET)&&
                (subjData->state != NNSH_SUBJSTATUS_DELETE))
            {
                // �X���f�[�^�擾�ς݂̂Ƃ�...
                if (StrNCompare(subjData->threadFileName, fileName, StrLen(fileName)) == 0)
                {
                    // �Y���X���𔭌�! (�I�����X�g�ɋL�^����)
                    recArray[itemCnt] = (loop - 1);

                    if (subjData->state == NNSH_SUBJSTATUS_NEW)
                    {
                        // �V�X���̏ꍇ�ɂ́A*������
                        StrCopy(labelPtr, "*");
                    }
                    else if (subjData->state == NNSH_SUBJSTATUS_REMAIN)
                    {
                        // �r���܂œǂ񂾃X���̏ꍇ�ɂ́A-������
                        StrCopy(labelPtr, "-");
                    }
                    else
                    {
                        StrCopy(labelPtr, " ");
                    }
                    StrNCopy(&labelPtr[StrLen(labelPtr)], subjData->threadTitle, MAX_NAMELABEL);
                    labelPtr = labelPtr + StrLen(labelPtr) + 1;
                    itemCnt++;
                    if (itemCnt >= (MAX_URLLIST_CNT + MARGIN))
                    {
                        // �I���\�ȃ��X�g���𒴂���...���[�v���甲����
                        //  (�m�ۂ������R�[�h���������Ă���...)
                        ReleaseRecordReadOnly_NNsh(dbRef, memH);
                        break;
                    }
                }
            }
            // �m�ۂ������R�[�h����������
            ReleaseRecordReadOnly_NNsh(dbRef, memH);
        }
    }
    Hide_BusyForm(false);

    // DB���N���[�Y����...
    CloseDatabase_NNsh(dbRef);

    // ���o���R�[�h��0���ł͂Ȃ������ꍇ...
    if (itemCnt != 0)
    {
        // �I���E�B���h�E��\�����A�ǂ݂����L���̑I���𑣂�
        loop = NNshWinSelectionWindow(FRMID_DIRSELECTION, selLabel, itemCnt, 0);

        // OK�{�^���������ꂽ�Ƃ��A�A�A
        if (loop != BTNID_JUMPCANCEL)
        {
            // index��I�������L���̃��R�[�h�ԍ��ɐݒ肷��
            index = recArray[NNshGlobal->jumpSelection];
        }
        else
        {
            // �c�a�̑I���͍s���Ȃ�����...
            index = NNSH_DATABASE_UNKNOWN;
        }
    }

    // �m�ۂ����̈���J�����ďI������
    MEMFREE_PTR(selLabel);
    MEMFREE_PTR(recArray);
    return (index);
}

/*-------------------------------------------------------------------------*/
/*   Function : isDeleteRecord�@�@�@�@�@�@                                 */
/*                                            ���O���폜����Ώۂ����f���� */
/*-------------------------------------------------------------------------*/
static Boolean isDeleteRecord(Boolean isReadLog, UInt8 state)
{
    if ((state == NNSH_SUBJSTATUS_NOT_YET)||
        (state == NNSH_SUBJSTATUS_DELETE))
    {
        // �X����Ԃ��u���擾�v�u�폜�v�̂Ƃ��͑ΏۊO
        return (false);
    }
    if (isReadLog == true)
    {
        if (state == NNSH_SUBJSTATUS_ALREADY)
        {
            // ���Ǎ폜���[�h�ŁA�X����Ԃ����ǂ̂Ƃ��͑Ώ�
            return (true);
        }

        // ���Ǎ폜���[�h�ŁA�X����Ԃ����ǂłȂ���΍폜�ΏۊO
        return (false);
    }

    // �X����Ԃ���L�ȊO�͍폜�Ώ�
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : deleteAllThread_getLogList                                 */
/*                                                      ���O���ꊇ�폜���� */
/*-------------------------------------------------------------------------*/
static UInt16 deleteAllThread_getLogList(UInt16 recNum, Boolean isReadLog)
{
    Err                  ret;
    UInt16               dbCount, loop, itemCnt;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subjData;
    MemHandle            memH;
    Char                 fileName[MAX_THREADFILENAME + MARGIN];

    // �t�@�C�����̐擪��p�ӂ���
    MemSet (fileName, (MAX_THREADFILENAME + MARGIN), 0x00);
    StrCopy(fileName, FILE_LOGCHARGE_PREFIX);
    NUMCATI(fileName, recNum);
    StrCat (fileName, "-");

    // �폜�������N���A����
    itemCnt = 0;

    // �X���f�[�^�x�[�X�̃I�[�v��
    dbRef = 0;
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �X��DB�̃I�[�v�����s�A�I������
        return (itemCnt);
    }

    // ���R�[�h�����擾
    GetDBCount_NNsh(dbRef, &dbCount);

    // BUSY�E�B���h�E��\������(�p�[�W�������̕\��)
    Show_BusyForm(MSG_EXECUTE_PURGE);

    // ���R�[�h�ԍ��̎Ⴂ�ق�����\������...
    for (loop = dbCount; loop != 0; loop--)
    {
        // ���R�[�h�����擾����
        ret = GetRecordReadOnly_NNsh(dbRef, (loop - 1), &memH, (void**) &subjData);
        if (ret == errNone)
        {
            // �폜�Ώۂ̃��R�[�h���`�F�b�N����
            if (isDeleteRecord(isReadLog, subjData->state) == true)
            {
                // �X���f�[�^�擾�ς݂̂Ƃ�...
                if (StrNCompare(subjData->threadFileName, fileName, StrLen(fileName)) == 0)
                {
                    // �m�ۂ������R�[�h����������
                    ReleaseRecordReadOnly_NNsh(dbRef, memH);

                    // ��xDB���N���[�Y����...
                    CloseDatabase_NNsh(dbRef);

                    // �Y���X���𔭌�! (�X�����폜����)
                    DeleteThreadMessage(loop - 1);
                    itemCnt++;
                    
                    // �ēxDB���I�[�v������
                    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
                    if (dbRef == 0)
                    {
                        // �Ȃ����I�[�v�����s...���̏ꍇ�ɂ̓��[�v�𔲂���
                        break;   
                    }
                }
                else
                {
                    // �m�ۂ������R�[�h����������
                    ReleaseRecordReadOnly_NNsh(dbRef, memH);            
                }
            }
            else
            {
                // �m�ۂ������R�[�h����������
                ReleaseRecordReadOnly_NNsh(dbRef, memH);
            }
        }
    }
    Hide_BusyForm(false);

    // DB���N���[�Y����...
    CloseDatabase_NNsh(dbRef);

    // �폜�������R�[�h������������
    return (itemCnt);
}

#endif  // #ifdef USE_LOGCHARGE
