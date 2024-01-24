/*============================================================================*
 *  FILE: 
 *     linedata.c
 *
 *  Description: 
 *     �f�[�^�̏o�͏���
 *
 *===========================================================================*/
#define LINEDATA_C
#include "local.h"


/*=========================================================================*/
/*   Function :   NNsi_putLine                                             */
/*                                                                ��ʕ\�� */
/*=========================================================================*/
void NNsi_putLine(FormType *frm, UInt16 gadId, UInt16 upBtnID, UInt16 downBtnID, UInt16 command)
{
    RectangleType dimF;
    UInt16        fontID, nlines, fontHeight;
    UInt16        dataLines, startLine, lineStatus, lp, len;

    Char             lineData[BUFSIZE + MARGIN];
    NNshMacroRecord *source;

    dataLines = NNshGlobal->currentScriptLine;
    source    = (NNshMacroRecord *) NNshGlobal->scriptArea;

    // ��Ԃ̏�����
    lineStatus = NNSH_DISP_ALL;
    
    // ��ʂ̑傫���ƕ\���\�s�����擾����
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, gadId), &dimF);
    fontID = NNshParam->currentFont;
    NNsi_UpdateRectangle(&dimF, NNshParam->currentFont,
                                                &fontID, &fontHeight, &nlines);

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
        if (NNshGlobal->pageTopLine <= nlines)
        {
            startLine  = 0;
            lineStatus = NNSH_DISP_UPPERLIMIT;
        }
        else
        {
            startLine = NNshGlobal->pageTopLine - nlines;
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_PAGEDOWN:
        // �P�y�[�W����\������
        if ((NNshGlobal->pageTopLine + nlines) >= dataLines)
        {
            startLine  = NNshGlobal->pageTopLine;
            lineStatus = NNSH_DISP_LOWERLIMIT;
        }
        else
        {
            startLine  = NNshGlobal->pageTopLine + nlines;
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_REDRAW:
        // ���݂̂Ƃ�����ĕ`��
        startLine  = NNshGlobal->pageTopLine;
        if (NNshGlobal->pageTopLine == 0)
        {
            lineStatus = NNSH_DISP_UPPERLIMIT;
        }
        else
        {
            lineStatus = NNSH_DISP_HALFWAY;
        }
        break;

      case NNSH_STEP_TO_BOTTOM:
      default:
        // �����y�[�W��\������
        startLine  = (dataLines >= nlines) ? 0 : dataLines - nlines;
        lineStatus = NNSH_DISP_LOWERLIMIT;
        break;
    }

    // �y�[�W��Ԃ𔽉f
    if (lineStatus != NNSH_DISP_UPPERLIMIT)
    {
        if (startLine + nlines >= dataLines)
        {
            lineStatus = NNSH_DISP_LOWERLIMIT;
        }
        else
        {
            lineStatus = NNSH_DISP_HALFWAY;
        }
    }
    if (dataLines <= nlines)
    {
        lineStatus = NNSH_DISP_ALL;
    }

    // �\���̈���N���A����
    NNsi_EraseRectangle(&dimF);

    // �㉺�{�^���̕\���ݒ�
    NNsh_UpDownButtonControl(FrmGetActiveForm(), lineStatus, upBtnID, downBtnID);

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
        // FntSetFont(NNshParam->sonyHRFontTitle);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

    // ������̕`��i�s�P�ʂŕ`�悷��j
    for (lp = 0; lp < nlines; lp++)
    {
        if ((lp + startLine) > NNshGlobal->currentScriptLine)
        {
            // �ŏI�s�𒴂�����A�����ɂ̓f�[�^��\�����Ȃ�
            break;
        }
        MemSet(lineData, sizeof(lineData), 0x00);
        PrintMacroLine(lineData, BUFSIZE, (lp + startLine + 1), &source[lp + startLine]);

#ifdef USE_CLIE
        if (NNshGlobal->hrRef != 0)
        {
            // �t�H���g�̃��[�h���b�v(�`��̈���ɓ��e��}���邽��)
            if (NNshGlobal->hrVer >= HR_VERSION_SUPPORT_FNTSIZE)
            {
                len = HRFntWordWrap(NNshGlobal->hrRef, lineData, dimF.extent.x);
            }
            else
            {
                len = FntWordWrap(lineData, dimF.extent.x);
            }
            // CLIE�n�C���]�`��(Truncate���ĕ\��)
            HRWinDrawTruncChars(NNshGlobal->hrRef, lineData, StrLen(lineData),
                                dimF.topLeft.x, dimF.topLeft.y, dimF.extent.x);
        }
        else
#endif
        {
            // �ʏ탂�[�h�`��(Truncate���ĕ\��)
            len = FntWordWrap(lineData, dimF.extent.x);
            WinDrawTruncChars(lineData,len,dimF.topLeft.x,
                              dimF.topLeft.y, dimF.extent.x);
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
        // FntSetFont(NNshParam->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

    // �擪�s��pageTopLine�ɋL������
    NNshGlobal->pageTopLine = startLine;

    return;
}

/*=========================================================================*/
/*   Function :   NNsh_UpDownButtonControl                                 */
/*                                             �㉺�{�^���̕\��/��\���ݒ� */
/*=========================================================================*/
void NNsh_UpDownButtonControl(FormType *frm, UInt16 btnState, UInt16 upBtnID, UInt16 downBtnID)
{
    // ���/�����̃{�^����\������
    // (�㉺�{�^����\��/��\���ɂ���)
    switch (btnState)
    {
      case NNSH_DISP_NOTHING:
        // �ꗗ�Ȃ�
        FrmHideObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmHideObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;

      case NNSH_DISP_UPPERLIMIT:
        // ���
        FrmHideObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmShowObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;

      case NNSH_DISP_LOWERLIMIT:
        // ����
        FrmShowObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmHideObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;

      case NNSH_DISP_ALL:
        // �S��
        FrmHideObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmHideObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;

      case NNSH_DISP_HALFWAY:
      default:
        // �r��
        FrmShowObject(frm, FrmGetObjectIndex(frm, upBtnID));
        FrmShowObject(frm, FrmGetObjectIndex(frm, downBtnID));
        break;
    }
    return;
}
