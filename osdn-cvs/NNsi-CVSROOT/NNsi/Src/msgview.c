/*============================================================================*
 *
 *  $Id: msgview.c,v 1.209 2009/03/07 11:30:36 mrsa Exp $
 *
 *  FILE: 
 *     msgview.c
 *
 *  Description: 
 *     Message(Thread) Viewer for NNsh. 
 *
 *===========================================================================*/
#define MSGVIEW_C
#include "local.h"
#include "Clipboard.h"

static Err PartReceiveMessage_View(void);
static void NNsh_Search_Message(UInt16 type, Char *str, Boolean flag);
static Err NNsh_Get_MessageToBuf(NNshMessageIndex *idxP, UInt16 msg, UInt16 type, Boolean flag);


/*-------------------------------------------------------------------------*/
/*   Function : pickupAnchor                                               */
/*                                    ���b�Z�[�W���̃A���J�[���o����ёI�� */
/*-------------------------------------------------------------------------*/
static Boolean pickupAnchor(NNshMessageIndex *idxP, Char *buffer, UInt16 size)
{
    Boolean ret;
    UInt16  itemCnt, bufCnt, getState, kanji, btnId;
    UInt32  dataSize;
    Char   *data, *chkPtr, *labelP, *addrAddr[MAX_URL_LIST + 1];
    Char   *ptr, *ptr2, *ptr3;
    Char    tempURL[MAX_URL * 2 + MARGIN], tempLabel[MAX_NAMELABEL], *addrLabel;

    // �A���J�[���o����f�[�^�o�b�t�@�̃T�C�Y�i���j���m�肷��
    if ((idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
    {
        dataSize = (idxP->msgOffset)[idxP->nofMsg];
    }
    else
    {
        dataSize = (idxP->msgOffset)[idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode];
    }
    dataSize = dataSize - (idxP->msgOffset)[idxP->openMsg - 1];

    if (idxP->fileOffset > (idxP->msgOffset)[idxP->openMsg - 1])
    {
         // �f�[�^�o�b�t�@�Ƀf�[�^���ǂݍ��܂�Ă��Ȃ�(���肦�Ȃ��͂�...)
         // �G���[�ɂ���
#ifdef USE_REPORTER
         HostTraceOutputTL(appErrorClass, " NOT FOUND DATA: %lu , %lu, %d ",
                           idxP->fileOffset, 
                           (idxP->msgOffset)[idxP->openMsg - 1], idxP->openMsg);
#endif // #ifdef USE_REPORTER
         return (false);
    }

    // �f�[�^���x���̈�̊m��
    addrLabel = MEMALLOC_PTR(((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN));
    if (addrLabel == NULL)
    {
        // �f�[�^�̈�m�ێ��s�A�G���[�ɂ���
        return (false);
    }
    MemSet(addrLabel, ((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN), 0x00);
    labelP = addrLabel;

    // "URL�m�F��"��BUSY�t�H�[�����o�͂���
    Show_BusyForm(MSG_CHECK_URLLIST);
    
    // �ϐ�data�ɁA�A���J�[����������擪�̃|�C���^��ݒ肷��
    data = idxP->buffer + (idxP->msgOffset)[idxP->openMsg - 1] - idxP->fileOffset - MARGIN;
    itemCnt = 0;
    chkPtr = data;

    // �������\�������܂Ńf�[�^��ǂݔ�΂�
    while  ((chkPtr < data + dataSize)&&(*chkPtr < ' '))
    {
        chkPtr++;
    }

#ifdef USE_REPORTER
    HostTraceOutputTL(appErrorClass, " chkPtr: %lu , data:%lu, size:%lu ", chkPtr, data, dataSize);
#endif // #ifdef USE_REPORTER

    // �A���J�[�؂�o�����C������
    while (chkPtr < data + dataSize)
    {
        // item���x���𒊏o����
        ptr3 = StrStr(chkPtr, "<item");
        if ((ptr3 != NULL)&&(ptr3 < (data + dataSize)))
        {
            // �A���J�[��؂�o��
            ptr = StrStr(chkPtr, "<link>");
            if (ptr != NULL)
            {
                // <link> �����o����...�����N��؂�o��
                ptr  = StrStr(ptr, "http://");
                ptr2 = StrStr(ptr, "https://");
                if ((ptr == NULL)||((ptr2 != NULL)&&(ptr > ptr2)))
                {
                    // https:// ��I��
                    ptr = ptr2;
                }
                if (ptr != NULL)
                {
                    // URL�̖�������肷��...
                    ptr2 = StrStr(ptr, "<");
                    while (ptr < ptr2)
                    {
                        ptr2--;
                        if (*ptr2 > ' ')
                        {
                            break;
                        }
                    }

                    MemSet(tempURL, sizeof(tempURL), 0x00);
                    if (sizeof(tempURL) >  ((ptr2 - ptr) + 1))
                    {
                        MemMove(tempURL, ptr, ((ptr2 - ptr) + 1));
                    }
                    else
                    {
                        MemMove(tempURL, ptr, (sizeof(tempURL) - 1));
                    }

                    // URL�؂�o�������A�A���J�[���x�����\�z����
                    MemSet(tempLabel, MAX_NAMELABEL, 0x00);
                    ptr = StrStr(ptr3, "<title>");
                    if (ptr == NULL)
                    {
                        // �^�C�g����������Ȃ������Ƃ��A�A�AURL�����x����
                        if (StrLen(tempURL) < MAX_NAMELABEL)
                        {
                            StrCopy(tempLabel, tempURL);
                        }
                        else
                        {
                            StrNCopy(tempLabel,
                                     &tempURL[StrLen(tempURL) - (35 - 1)],
                                     MAX_NAMELABEL);
                        }
                    }
                    else
                    {
                        // �^�C�g���𔭌��A�A���J�[���x���ɂ���B
                        // (RDF�`���Ȃ̂ŁAUTF8���낤�B�B���߂���)
                        ptr = ptr + StrLen("<title>");
                        StrNCopyUTF8toSJ(tempLabel, ptr, (MAX_NAMELABEL - MARGIN));
                    }

                    // �A���J�[�i�[���W�b�N��...
                    kanji = NNSH_BBSTYPE_CHAR_SJIS;  // ��...

#ifdef USE_REPORTER
                    HostTraceOutputTL(appErrorClass, "anchor[%s], label[%s]", tempURL, tempLabel);
#endif // #ifdef USE_REPORTER
             
                    // HTTP�A���J�[���ő吔�ɓ��B���Ă��Ȃ��ꍇ�A�f�[�^������
                    if (itemCnt < MAX_URL_LIST)
                    {
                        // URL�A���J�[���i�[����
                        addrAddr[itemCnt] = MEMALLOC_PTR(StrLen(tempURL) + MARGIN);
                        if (addrAddr[itemCnt] == NULL)
                        {
                            // �̈�m�ێ��s�A���[�v�𔲂���
                            break;
                        }
                        MemSet(addrAddr[itemCnt], (StrLen(tempURL) + MARGIN), 0x00);
                        StrCopy(addrAddr[itemCnt], tempURL);

                        StrCopy(labelP, tempLabel);

                        // ������̖����ɏȗ��L��������
                        labelP[StrLen(labelP)] = chrEllipsis;

                        // �|�C���^�����̗̈�Ɉړ�����
                        labelP = labelP + StrLen(labelP) + 1;

                        // �A�C�e�����𑝂₷
                        itemCnt++;
                    }
                    chkPtr = ptr2;
                    continue;
                }
            }
        }

        // �o�b�t�@���ɂ���A���J�[��T���i�啶���Ə������̃A���J�[�����j
        ptr = StrStr(chkPtr, "<a");
        ptr2 = StrStr(chkPtr, "<A");
        if ((ptr == NULL)&&(ptr2 == NULL))
        {
            // �����A���J�[���Ȃ��A���[�v�𔲂���
#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " anchor nothing... ");
#endif // #ifdef USE_REPORTER
            break;
        }

        // �啶���A���J�[�̕����O�ɂ���A�啶���A���J�[��L���ɂ���
        if ((ptr == NULL)||((ptr2 != NULL)&&(ptr > ptr2)))
        {
            ptr = ptr2;
        }
        while (ptr < data + dataSize)
        {
            // 'href' ��T��
            if (((*ptr == 'h')||(*ptr == 'H'))&&
                ((*(ptr + 1) == 'r')||(*(ptr + 1) == 'R'))&&
                ((*(ptr + 2) == 'e')||(*(ptr + 2) == 'E'))&&
                ((*(ptr + 3) == 'f')||(*(ptr + 3) == 'F')))
            {
#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " find HREF anchor... ");
#endif // #ifdef USE_REPORTER
                // href�A�g���r���[�g����
                ptr = ptr + 4; // href��ǂݔ�΂�
                while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != 0x00))
                {
                    ptr++;
                }
                if (*ptr == 0x00)
                {
                    // �A���J�[�̓r���Ńo�b�t�@���؂ꂽ�B�B�B���[�v�𔲂���
                    break;
                }
                ptr++;

                // URL�؂�o���o�b�t�@�̏�����
                bufCnt = 0;
                MemSet(tempURL, sizeof(tempURL), 0x00);

                if ((StrNCaselessCompare(ptr, "http", StrLen("http")) != 0)||
                    ((*(ptr + 4) != 's')&&(*(ptr + 4) != ':'))||
                    (*(ptr + 6) != '/'))
                {
#ifdef USE_REPORTER
                     HostTraceOutputTL(appErrorClass, " not HTTP ");
#endif // #ifdef USE_REPORTER

                    // http��URL�ł͂Ȃ��A���炩���߃x�[�XURL��t������
                    StrNCopy(tempURL, idxP->baseURL, (sizeof(tempURL) - 1));
                    bufCnt = StrLen(tempURL);

                    // URL�́A�z�X�g����̐�΃A�h���X�w��
                    if (*ptr == '/')
                    {
                        // �z�X�g��������؂���
                        bufCnt = StrLen("http://");
                        while ((tempURL[bufCnt] != '/')&&(tempURL[bufCnt] != '\0'))
                        {
                            bufCnt++;
                        }
                        tempURL[bufCnt] = '\0';                                                
                    }
                    else if (*ptr == '.')
                    {
                        // URL�́A���̈ʒu���瑊�΃A�h���X�w�肾�����ꍇ�A�A�A
                        ptr++;
                        ptr++;
                    }
                }

                // URL�f�[�^�̐؂�o��
                while ((*ptr != '"')&&(*ptr != '\'')&&(*ptr != 0x00)&&(bufCnt < (sizeof(tempURL) - 2)))
                {
                    tempURL[bufCnt] = *ptr;
                    ptr++;
                    bufCnt++;
                }
                // �A���J�[�^�O�̖����܂œǂݔ�΂�
                while ((*ptr != '>')&&(*ptr != 0x00))
                {
                    ptr++;
                }
                ptr++;

#ifdef USE_REPORTER
                HostTraceOutputTL(appErrorClass, " pick up anchor label ");
#endif // #ifdef USE_REPORTER

                // �A���J�[���x���̐؂�o��
                kanji = idxP->bbsType & NNSH_BBSTYPE_CHARSETMASK;
                getState = HTTP_ANCHORSTATE_NORMAL;
                bufCnt = 0;
                MemSet(tempLabel, MAX_NAMELABEL, 0x00);
                while ((*ptr != 0x00)&&(ptr < data + dataSize)&&
                        (bufCnt < (MAX_NAMELABEL - 2)))
                {
                    if ((kanji == NNSH_BBSTYPE_CHAR_JIS)&&(*ptr == 0x1b))
                    {
                        // �G�X�P�[�v�R�[�h���� KNJ/ANK�؂�ւ�
                        if ((*(ptr + 1) == 0x24)&&(*(ptr + 2) == 0x42))
                        {
                            // �����R�[�h�ɐ؂�ւ�
                            getState = HTTP_ANCHORSTATE_KANJI;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            ptr++;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            ptr++;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            // ptr++ �́Aif�u���b�N�̎��ɂ���Ƃ���Ŏ��{����
                        }
                        else if ((*(ptr + 1) == 0x28)&&(*(ptr + 2) == 0x42))
                        {
                            // ANK���[�h�ɐ؂�ւ�
                            getState = HTTP_ANCHORSTATE_NORMAL;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            ptr++;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            ptr++;
                            tempLabel[bufCnt] = *ptr;
                            bufCnt++;
                            // ptr++ �́Aif�u���b�N�̎��ɂ���Ƃ���Ŏ��{����
                        }
                    }
                    else if ((*ptr == 0x0a)||(*ptr == 0x0d))
                    {
                        // ���s�R�[�h�͓ǂݔ�΂�
                    }
                    else if ((getState == HTTP_ANCHORSTATE_NORMAL)&&(*ptr == '<'))
                    {
                        getState = HTTP_ANCHORSTATE_ANCHOR;
                        if ((*(ptr + 1) == 'A')||(*(ptr + 1) == 'a'))
                        {
                            // ���̃A���J�[�����������A������
                            ptr--;
                            break;
                        }
                    }
                    else if ((getState == HTTP_ANCHORSTATE_ANCHOR)&&(*ptr == '>'))
                    {
                        getState = HTTP_ANCHORSTATE_NORMAL;
                    }
                    else if ((getState == HTTP_ANCHORSTATE_NORMAL)||
                              (getState == HTTP_ANCHORSTATE_KANJI))
                    {
                        tempLabel[bufCnt] = *ptr;
                        bufCnt++;
                    }
                    ptr++;                
                }
              
                // HTTP�A���J�[���ő吔�ɓ��B���Ă��Ȃ��ꍇ�A�f�[�^������
                if (itemCnt < MAX_URL_LIST)
                {
                    // URL�A���J�[���i�[����
                    addrAddr[itemCnt] = MEMALLOC_PTR(StrLen(tempURL) + MARGIN);
                    if (addrAddr[itemCnt] == NULL)
                    {
                        // �̈�m�ێ��s�A���[�v�𔲂���
                        break;
                    }
                    MemSet(addrAddr[itemCnt], (StrLen(tempURL) + MARGIN), 0x00);
                    StrCopy(addrAddr[itemCnt], tempURL);

#ifdef USE_REPORTER
            HostTraceOutputTL(appErrorClass, " URL : %s (label:%s)", tempURL, tempLabel);
#endif // #ifdef USE_REPORTER
                    // �f�[�^���x����(�����R�[�h�ϊ��������Ɏ��{����)�i�[
                    switch (kanji)
                    {
                      case NNSH_BBSTYPE_CHAR_JIS:
                        StrCopyJIStoSJ(labelP, tempLabel);
                        break;

                      case NNSH_BBSTYPE_CHAR_EUC:
                        StrCopySJ(labelP, tempLabel);
                        break;

                      case NNSH_BBSTYPE_CHAR_SJIS:
                      default:
                        StrCopy(labelP, tempLabel);
                        break;
                    }
                    // ������̖����ɏȗ��L��������
                    labelP[StrLen(labelP)] = chrEllipsis;

                    // �|�C���^�����̗̈�Ɉړ�����
                    labelP = labelP + StrLen(labelP) + 1;

                    // �A�C�e�����𑝂₷
                    itemCnt++;
                }
            }
            ptr++;
        }
        chkPtr = ptr;
    }  // while (chkPtr < data + dataSize)
    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // �P���A���J�[���Ȃ��A�I������
        MEMFREE_PTR(addrLabel);
        return (false);
    }
    ret = false;

    // �I���E�B���h�E��\�����AURL�̑I���𑣂�
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, addrLabel, itemCnt, 0);

    // OK�{�^���������ꂽ�Ƃ��A�A�A
    if (btnId != BTNID_JUMPCANCEL)
    {
        // �f�[�^���R�s�[����
        StrNCopy(buffer, addrAddr[NNshGlobal->jumpSelection], size);
        ret = true;

        // �ꉞ�A�J���y�[�W(URL)��\���ł���悤�ɂ��Ă���
        NNsh_DebugMessage(ALTID_INFO, buffer, MSG_WEBLAUNCH_CONFIRM, 0);
    }

    // �m�ۂ����̈���J������
    for (bufCnt = 0; bufCnt < itemCnt; bufCnt++)
    {
        MEMFREE_PTR(addrAddr[bufCnt]);
    }
    MEMFREE_PTR(addrLabel);

    return (ret); 
}

/*-------------------------------------------------------------------------*/
/*   Function : pickupAnchorURL                                            */
/*                                         ���b�Z�[�W����URL���o����ёI�� */
/*-------------------------------------------------------------------------*/
static Boolean pickupAnchorURL(NNshMessageIndex *idxP, Char *buffer, UInt16 size)
{
    Boolean ret, isHttps;
    UInt16  itemCnt, bufCnt, btnId;
    UInt32  dataSize;
    Char   *data, *chkPtr, *ptr, *labelP, *addrAddr[MAX_URL_LIST + MARGIN];
    Char   *tempURL, tempLabel[MAX_NAMELABEL], *addrLabel;

    // URL�ꎞ�ۊǗ̈���m��
    tempURL = MEMALLOC_PTR(MAX_URL * 3 + MARGIN);
    if (tempURL == NULL)
    {
        return (false);
    }
    MemSet(tempURL, (MAX_URL * 3 + MARGIN), 0x00);

    // �A���J�[���o����f�[�^�o�b�t�@�̃T�C�Y�i���j���m�肷��
    if ((idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
    {
        dataSize = (idxP->msgOffset)[idxP->nofMsg];
    }
    else
    {
        dataSize = (idxP->msgOffset)[idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode];
    }
    if (idxP->openMsg != 0)
    {
        dataSize = dataSize - (idxP->msgOffset)[idxP->openMsg - 1];

        if (idxP->fileOffset > (idxP->msgOffset)[idxP->openMsg - 1])
        {
             // �f�[�^�o�b�t�@�Ƀf�[�^���ǂݍ��܂�Ă��Ȃ�(���肦�Ȃ��͂�...)
             // �G���[�ɂ���
             NNsh_DebugMessage(ALTID_INFO, "illegal range", " ", 0);
             MEMFREE_PTR(tempURL);
             return (false);
        }
    }

    // �f�[�^���x���̈�̊m��
    addrLabel = MEMALLOC_PTR(((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN));
    if (addrLabel == NULL)
    {
        // �f�[�^�̈�m�ێ��s�A�G���[�ɂ���
        NNsh_DebugMessage(ALTID_INFO, "alloc fail", " ", 0);
        MEMFREE_PTR(tempURL);
        return (false);
    }
    MemSet(addrLabel, ((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN), 0x00);
    labelP = addrLabel;

    // "URL�m�F��"��BUSY�t�H�[�����o�͂���
    Show_BusyForm(MSG_CHECK_URLLIST);

    // �ϐ�data�ɁA�A���J�[����������擪�̃|�C���^��ݒ肷��
    data = idxP->buffer - idxP->fileOffset - MARGIN;
    if (idxP->openMsg != 0)
    {
        data = data + (idxP->msgOffset)[idxP->openMsg - 1];
    }
    itemCnt = 0;
    chkPtr = data;

    // �������\�������܂Ńf�[�^��ǂݔ�΂�
    while  ((chkPtr < data + dataSize)&&(*chkPtr < ' '))
    {
        chkPtr++;
    }

    // �A���J�[�؂�o�����C������
    while (chkPtr < data + dataSize)
    {
        // �o�b�t�@���ɂ���URL��T��
        isHttps = false;
        ptr = StrStr(chkPtr, "tp://");
        if (ptr == NULL)
        {
            ptr = StrStr(chkPtr, "tps://");
            isHttps = true;
        }
        if (ptr == NULL)
        {
            // �����A���J�[���Ȃ��A���[�v�𔲂���
            NNsh_DebugMessage(ALTID_INFO, "link:", " ", itemCnt);
            break;
        }
        // ��ɐi�߂�...
        ptr = ptr + sizeof("tp://") - 1;
        if (isHttps == true)
        {
            // https ��'s'�������i�߂�
            ptr++;
        }

        // ���o����URL���͈͊O�Ȃ烋�[�v�𔲂���
        if (ptr >= data + dataSize)
        {
            NNsh_DebugMessage(ALTID_INFO, "LINK:", " ", itemCnt);
            break;
        }

        // ���o����URL�𒊏o
        NNsh_DebugMessage(ALTID_INFO, "http(s)://", ptr, isHttps);

        MemSet (tempURL, (MAX_URL * 3 + MARGIN), 0x00);
        if (isHttps == false)
        {
            StrCopy(tempURL, "http://");
            bufCnt = 7; // 7 �́AStrLen(tempURL);
        }
        else
        {
            StrCopy(tempURL, "https://");
            bufCnt = 8; // 8 �́AStrLen(tempURL);
        }

        // URL�f�[�^�̐؂�o��
        while ((*ptr > ' ')&&(*ptr != '>')&&(*ptr != '<')&&
                (*ptr != '"')&&(*ptr != '\''))
        {
            tempURL[bufCnt] = *ptr;
            ptr++;
            bufCnt++;
            if (bufCnt > (MAX_URL * 3))
            {
                break;
            }
        }

        // URL���x���̃R�s�[
        MemSet(tempLabel, MAX_NAMELABEL, 0x00);
        if (bufCnt < (MAX_NAMELABEL - 2))
        {
            StrCopy(tempLabel, tempURL);
        }
        else
        {
            tempLabel[0] = chrEllipsis;
            StrNCopy(&tempLabel[1], 
                     &tempURL[bufCnt - (MAX_NAMELABEL - 1)],
                     (MAX_NAMELABEL - 2));
        }
                
        // HTTP�A���J�[���ő吔�ɓ��B���Ă��Ȃ��ꍇ�A�f�[�^������
        if (itemCnt < MAX_URL_LIST)
        {
            // URL�A���J�[���i�[����
            addrAddr[itemCnt] = MEMALLOC_PTR(StrLen(tempURL) + MARGIN);
            if (addrAddr[itemCnt] == NULL)
            {
                // �̈�m�ێ��s�A���[�v�𔲂���
                break;
            }
            MemSet(addrAddr[itemCnt], (StrLen(tempURL) + MARGIN), 0x00);
            StrCopy(addrAddr[itemCnt], tempURL);

            // �f�[�^���x����(�����R�[�h�ϊ��������Ɏ��{����)�i�[
            StrNCopy(labelP, tempLabel, (MAX_NAMELABEL - 1));

            // �|�C���^�����̗̈�Ɉړ�����
            labelP = labelP + StrLen(labelP) + 1;

            // �A�C�e�����𑝂₷
            itemCnt++;
        }
        ptr++;
        chkPtr = ptr;
    }  // while (chkPtr < data + dataSize)
    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // �P���A���J�[���Ȃ��A�I������
        MEMFREE_PTR(tempURL);
        MEMFREE_PTR(addrLabel);
        return (false);
    }
    ret = false;

    // �I���E�B���h�E��\�����AURL�̑I���𑣂�
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, addrLabel, itemCnt, 0);

    // OK�{�^���������ꂽ�Ƃ��A�A�A
    if (btnId != BTNID_JUMPCANCEL)
    {
        // �f�[�^���R�s�[����
        MemSet  (buffer, size, 0x00);
        StrNCopy(buffer, addrAddr[NNshGlobal->jumpSelection], (size - 1));
        ret = true;

        // �ꉞ�A�J���y�[�W(URL)��\���ł���悤�ɂ��Ă���
        NNsh_DebugMessage(ALTID_INFO, buffer, MSG_WEBLAUNCH_CONFIRM, 0);
    }

    // �m�ۂ����̈���J������
    for (bufCnt = 0; bufCnt < itemCnt; bufCnt++)
    {
        MEMFREE_PTR(addrAddr[bufCnt]);
    }
    MEMFREE_PTR(addrLabel);
    MEMFREE_PTR(tempURL);
    return (ret); 
}

/*-------------------------------------------------------------------------*/
/*   Function : pickupNGwordCandidate                                      */
/*                                              NG���[�h��"���"�𒊏o���� */
/*-------------------------------------------------------------------------*/
static Boolean pickupNGwordCandidate(NNshMessageIndex *idxP, Char *buffer, UInt16 size)
{
    Boolean ret, isName;
    UInt16  itemCnt, bufCnt, btnId;
    UInt32  dataSize;
    Char   *data, *chkPtr, *ptr, *ptr2, *labelP, *addrAddr[MAX_URL_LIST + MARGIN];
    Char   *tempURL, tempLabel[MAX_NAMELABEL], *addrLabel;

    // URL�ꎞ�ۊǗ̈���m��
    tempURL = MEMALLOC_PTR(MAX_URL * 2 + MARGIN);
    if (tempURL == NULL)
    {
        return (false);
    }
    MemSet(tempURL, (MAX_URL * 2 + MARGIN), 0x00);

    // �A���J�[���o����f�[�^�o�b�t�@�̃T�C�Y�i���j���m�肷��
    if ((idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
    {
        dataSize = (idxP->msgOffset)[idxP->nofMsg];
    }
    else
    {
        dataSize = (idxP->msgOffset)[idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode];
    }
    if (idxP->openMsg != 0)
    {
        dataSize = dataSize - (idxP->msgOffset)[idxP->openMsg - 1];

        if (idxP->fileOffset > (idxP->msgOffset)[idxP->openMsg - 1])
        {
             // �f�[�^�o�b�t�@�Ƀf�[�^���ǂݍ��܂�Ă��Ȃ�(���肦�Ȃ��͂�...)
             // �G���[�ɂ���
             NNsh_DebugMessage(ALTID_INFO, "illegal range", " ", 0);
             MEMFREE_PTR(tempURL);
             return (false);
        }
    }

    // �f�[�^���x���̈�̊m��
    addrLabel = MEMALLOC_PTR(((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN));
    if (addrLabel == NULL)
    {
        // �f�[�^�̈�m�ێ��s�A�G���[�ɂ���
        NNsh_DebugMessage(ALTID_INFO, "alloc fail", " ", 0);
        MEMFREE_PTR(tempURL);
        return (false);
    }
    MemSet(addrLabel, ((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN), 0x00);
    labelP = addrLabel;

    // "�L�[���[�h���o��"��BUSY�t�H�[�����o�͂���
    Show_BusyForm(MSG_PICKUP_KEYWORD);

    // �ϐ�data�ɁA�A���J�[����������擪�̃|�C���^��ݒ肷��
    data = idxP->buffer - idxP->fileOffset - MARGIN;
    if (idxP->openMsg != 0)
    {
        data = data + (idxP->msgOffset)[idxP->openMsg - 1];
    }
    itemCnt = 0;
    chkPtr = data;

    // �������\�������܂Ńf�[�^��ǂݔ�΂�
    while  ((chkPtr < data + dataSize)&&(*chkPtr < ' '))
    {
        chkPtr++;
    }
    ptr = chkPtr;  // �擪�͖��O���Ƃ��Ē��o����...

    // �L�[���[�h�؂�o�����C������
    while (chkPtr < data + dataSize)
    {
        // �o�b�t�@���ɂ���L�[���[�h��T��
        isName = true;
        ptr = StrStr(chkPtr, "\x0a");
        ptr2 = StrStr(chkPtr, "ID:");
        if ((ptr != NULL)&&(ptr2 != NULL)&&(ptr2 < ptr))
        {
            isName = false;
            ptr = ptr2;
        }
        if (ptr == NULL)
        {
            if (ptr2 == NULL)
            {
                // �����L�[���[�h���Ȃ��A���[�v�𔲂���
                NNsh_DebugMessage(ALTID_INFO, "WD:", " ", itemCnt);
                break;
            }
            else
            {
                isName = false;
                ptr = ptr2;
            }
        }
        // ���O���̌��o�������ꍇ�A������Ɛ�ɐi�߂�...
        if (isName == true)
        {
            ptr = ptr + 1;
        }

        // ���o�����L�[���[�h���͈͊O�Ȃ烋�[�v�𔲂���
        if (ptr >= data + dataSize)
        {
            NNsh_DebugMessage(ALTID_INFO, "WD:", " ", itemCnt);
            break;
        }

        bufCnt = 0;
        MemSet (tempURL, (MAX_URL * 2 + MARGIN), 0x00);

        // �L�[���[�h�̐؂�o��
        while ((*ptr != ' ')&&(*ptr != '>')&&(*ptr != '<')&&
                (*ptr != '"')&&(*ptr != '\''))
        {
            tempURL[bufCnt] = *ptr;
            ptr++;
            bufCnt++;
        }

        // �L�[���[�h�̃R�s�[
        MemSet(tempLabel, MAX_NAMELABEL, 0x00);
        StrNCopy(tempLabel, tempURL, (MAX_NAMELABEL - 1));

        // ���o�����L�[���[�h�𒊏o
        NNsh_DebugMessage(ALTID_INFO, "WD:", tempURL, isName);
                
        // �L�[���[�h���ő吔�ɓ��B���Ă��Ȃ��ꍇ�A�f�[�^������
        if (itemCnt < MAX_URL_LIST)
        {
            // �L�[���[�h���i�[����
            addrAddr[itemCnt] = MEMALLOC_PTR(StrLen(tempURL) + MARGIN);
            if (addrAddr[itemCnt] == NULL)
            {
                // �̈�m�ێ��s�A���[�v�𔲂���
                break;
            }
            MemSet(addrAddr[itemCnt], (StrLen(tempURL) + MARGIN), 0x00);
            StrCopy(addrAddr[itemCnt], tempURL);

            // �f�[�^���x����(�����R�[�h�ϊ��������Ɏ��{����)�i�[
            StrNCopy(labelP, tempLabel, (MAX_NAMELABEL - 1));

            // �|�C���^�����̗̈�Ɉړ�����
            labelP = labelP + StrLen(labelP) + 1;

            // �A�C�e�����𑝂₷
            itemCnt++;
        }
        ptr++;
        chkPtr = ptr;
        ptr = NULL;
    }  // while (chkPtr < data + dataSize)
    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // �P���A���J�[���Ȃ��A�I������
        MEMFREE_PTR(tempURL);
        MEMFREE_PTR(addrLabel);
        return (false);
    }
    ret = false;

    // �I���E�B���h�E��\�����AURL�̑I���𑣂�
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, addrLabel, itemCnt, 0);

    // OK�{�^���������ꂽ�Ƃ��A�A�A
    if (btnId != BTNID_JUMPCANCEL)
    {
        // �f�[�^���R�s�[����
        MemSet  (buffer, size, 0x00);
        StrNCopy(buffer, addrAddr[NNshGlobal->jumpSelection], (size - 1));
        ret = true;

        // �ꉞ�A���o�����L�[���[�h��\���ł���悤�ɂ��Ă���
        NNsh_DebugMessage(ALTID_INFO, "selected : ", buffer, 0);
    }

    // �m�ۂ����̈���J������
    for (bufCnt = 0; bufCnt < itemCnt; bufCnt++)
    {
        MEMFREE_PTR(addrAddr[bufCnt]);
    }
    MEMFREE_PTR(addrLabel);
    MEMFREE_PTR(tempURL);
    return (ret); 
}

/*-------------------------------------------------------------------------*/
/*   Function : pickupBeProfile                                            */
/*                              ���b�Z�[�W����Be�v���t�B�[�����o����ёI�� */
/*-------------------------------------------------------------------------*/
static Boolean pickupBeProfile(NNshMessageIndex *idxP, Char *buffer, UInt16 size)
{
    UInt16  itemCnt, bufCnt, btnId;
    UInt32  dataSize;
    Char   *data, *chkPtr, *ptr, *labelP, *addrAddr[MAX_URL_LIST + MARGIN];
    Char   *tempURL, tempLabel[MAX_NAMELABEL], *addrLabel;

    // URL�ꎞ�ۊǗ̈���m��
    tempURL = MEMALLOC_PTR(MAX_URL * 2 + MARGIN);
    if (tempURL == NULL)
    {
        return (false);
    }
    MemSet(tempURL, (MAX_URL * 2 + MARGIN), 0x00);

    // �A���J�[���o����f�[�^�o�b�t�@�̃T�C�Y�i���j���m�肷��
    if ((idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
    {
        dataSize = (idxP->msgOffset)[idxP->nofMsg];
    }
    else
    {
        dataSize = (idxP->msgOffset)[idxP->openMsg + (NNshGlobal->NNsiParam)->blockDispMode];
    }
    if (idxP->openMsg != 0)
    {
        dataSize = dataSize - (idxP->msgOffset)[idxP->openMsg - 1];

        if (idxP->fileOffset > (idxP->msgOffset)[idxP->openMsg - 1])
        {
             // �f�[�^�o�b�t�@�Ƀf�[�^���ǂݍ��܂�Ă��Ȃ�(���肦�Ȃ��͂�...)
             // �G���[�ɂ���
             NNsh_DebugMessage(ALTID_INFO, "illegal range", " ", 0);
             MEMFREE_PTR(tempURL);
             return (false);
        }
    }

    // �f�[�^���x���̈�̊m��
    addrLabel = MEMALLOC_PTR(((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN));
    if (addrLabel == NULL)
    {
        // �f�[�^�̈�m�ێ��s�A�G���[�ɂ���
        NNsh_DebugMessage(ALTID_INFO, "alloc fail", " ", 0);
        MEMFREE_PTR(tempURL);
        return (false);
    }
    MemSet(addrLabel, ((MAX_URL_LIST + 1) * MAX_NAMELABEL + MARGIN), 0x00);
    labelP = addrLabel;

    // "Be ID���o��..."��BUSY�t�H�[�����o�͂���
    Show_BusyForm(MSG_CHECK_BEID);

    // �ϐ�data�ɁA�A���J�[����������擪�̃|�C���^��ݒ肷��
    data = idxP->buffer - idxP->fileOffset - MARGIN;
    if (idxP->openMsg != 0)
    {
        data = data + (idxP->msgOffset)[idxP->openMsg - 1];
    }
    itemCnt = 0;
    chkPtr = data;

    // �������\�������܂Ńf�[�^��ǂݔ�΂�
    while  ((chkPtr < data + dataSize)&&(*chkPtr < ' '))
    {
        chkPtr++;
    }

    // �A���J�[�؂�o�����C������
    while (chkPtr < data + dataSize)
    {
        // �o�b�t�@���ɂ���Be ID��T��
        ptr = StrStr(chkPtr, "BE:");
        if (ptr == NULL)
        {
            // �����A���J�[���Ȃ��A���[�v�𔲂���
            NNsh_DebugMessage(ALTID_INFO, "link:", " ", itemCnt);
            break;
        }
        // ��ɐi�߂�...
        ptr = ptr + sizeof("BE:") - 1;

        // ���o����BE ID���͈͊O�Ȃ烋�[�v�𔲂���
        if (ptr >= data + dataSize)
        {
            NNsh_DebugMessage(ALTID_INFO, "LINK:", " ", itemCnt);
            break;
        }

        // ���o����BE ID���f�o�b�O�\��
        NNsh_DebugMessage(ALTID_INFO, "BE:", ptr, 0);

        MemSet (tempURL, (MAX_URL * 2 + MARGIN), 0x00);
        StrCopy(tempURL, "BE:");
        bufCnt = 3; // 3 �́AStrLen("BE:");

        // BE iD�̐؂�o��
        while ((*ptr > ' ')&&(*ptr != '>')&&(*ptr != '<')&&
                (*ptr != '"')&&(*ptr != '\'')&&(*ptr != '\x0d')&&(*ptr != '\x0d'))
        {
            tempURL[bufCnt] = *ptr;
            ptr++;
            bufCnt++;
        }

        // BE ID�̃R�s�[
        MemSet(tempLabel, MAX_NAMELABEL, 0x00);
        StrNCopy(tempLabel, tempURL, (MAX_NAMELABEL - 1));
                
        // BE ID���ő吔�ɓ��B���Ă��Ȃ��ꍇ�A�f�[�^������
        if (itemCnt < MAX_URL_LIST)
        {
            // BE ID��(�I�����X�g��)�i�[����
            StrNCopy(labelP, tempLabel, (MAX_NAMELABEL - 1));
            addrAddr[itemCnt] = labelP;

            // �|�C���^�����̗̈�Ɉړ�����
            labelP = labelP + StrLen(labelP) + 1;

            // �A�C�e�����𑝂₷
            itemCnt++;
        }
        ptr++;
        chkPtr = ptr;
    }  // while (chkPtr < data + dataSize)

    Hide_BusyForm(false);
    if (itemCnt == 0)
    {
        // �P���A���J�[���Ȃ��A�I������
        MEMFREE_PTR(tempURL);
        MEMFREE_PTR(addrLabel);
        return (false);
    }
    if (itemCnt == 1)
    {
        // BE ID�̓o�b�t�@���ɂЂƂ����Ȃ�����...
        // (�����I�ɑI�������A��������)
        NNshGlobal->jumpSelection = 0;
        goto CREATE_PROFILE_URL;
    }

    // �I���E�B���h�E��\�����AURL�̑I���𑣂�
    btnId = NNshWinSelectionWindow(FRMID_DIRSELECTION, addrLabel, itemCnt, 0);

    // Cancel�{�^���������ꂽ�Ƃ��A�A�A
    if (btnId == BTNID_JUMPCANCEL)
    {
        // �m�ۂ����̈���J������
        MEMFREE_PTR(addrLabel);
        MEMFREE_PTR(tempURL);
        return (false);
    }

CREATE_PROFILE_URL:
    // Be �v���t�B�[���Q�Ɨp��URL���쐬����
    MemSet (buffer, size, 0x00);
    StrCopy(buffer, URL_BE2ch_PROFILE);
    StrCat (buffer, URL_PREFIX_BE2ch_PROFILE);

    // Be��ID�i���X�ɋL�q���ꂽ���́j���R�s�[����
    chkPtr = addrAddr[NNshGlobal->jumpSelection];
    chkPtr = chkPtr + sizeof("BE:") - 1; 
    ptr    = buffer + StrLen(buffer);
    while ((*chkPtr > ' ')&&(*chkPtr != '-')&&(*chkPtr != '#')&&(*chkPtr != ':'))
    {
        *ptr = *chkPtr;
        ptr++;
        chkPtr++;   
    }
    *ptr = '\0';  // NULL�^�[�~�l�[�g

    // �m�ۂ����̈���J������
    MEMFREE_PTR(addrLabel);
    MEMFREE_PTR(tempURL);
    return (true); 
}

/*-------------------------------------------------------------------------*/
/*   Function : replaceHideWord                                            */
/*                                                   �u�������ځ[��v���{  */
/*-------------------------------------------------------------------------*/
static void replaceHideWord(Char *buffer, Char *checkWord)
{
    UInt16 matchedWord;

    // NG���[�h������ * �ɒu������
    matchedWord = StrLen(checkWord);
    buffer = StrStr(buffer, checkWord);    
    while (buffer != NULL)
    {
        MemSet(buffer, matchedWord, '*');
        buffer = buffer + matchedWord;
        buffer = StrStr(buffer, checkWord);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : checkHideMessage                                           */
/*                                                  (�Ȉ�)NG�L�[���[�h���{ */
/*-------------------------------------------------------------------------*/
static void checkHideMessage(Char *buffer)
{
    UInt16  loop, maxCnt, matchedWord;
    Char   *ptr, **data, *checkWord, *area1, *area2;

    NNshNGwordDatabase *dbData;
    DmOpenRef           dbRef;
    MemHandle           dbH;
    
    // NG�`�F�b�N�P���Q�����s����ꍇ�A�A�A
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD1AND2) == NNSH_USE_NGWORD1AND2)
    {
        // NG�L�[���[�h�Ɉ��������邩�`�F�b�N
        if ((NNshGlobal->NNsiParam)->hideWord1[0] != '\0')
        {
            if (((NNshGlobal->NNsiParam)->useRegularExpression != 0)&&
                (NNshGlobal->hide1.wordmemH != 0))
            {
                // NG�L�[���[�h�𕡐��w�肵���ꍇ...
                data = MemHandleLock(NNshGlobal->hide1.wordmemH);
                for (loop = 0; loop < NNshGlobal->hide1.nofWord; loop++)
                {
                    if (StrStr(buffer, data[loop]) != NULL)
                    {
                        // �L�[���[�h�����I
                        MemHandleUnlock(NNshGlobal->hide1.wordmemH);
                        goto REPLACE_MESSAGE;
                    }
                }
                MemHandleUnlock(NNshGlobal->hide1.wordmemH);
            }
            else
            {
                // NG�L�[���[�h�𕡐��w�肵�Ȃ��ꍇ
                if (StrStr(buffer, (NNshGlobal->NNsiParam)->hideWord1) != NULL)
                {
                    goto REPLACE_MESSAGE;
                }
            }
        }
        if ((NNshGlobal->NNsiParam)->hideWord2[0] != '\0')
        {
            if (((NNshGlobal->NNsiParam)->useRegularExpression != 0)&&
                (NNshGlobal->hide2.wordmemH != 0))
            {
                // NG�L�[���[�h�𕡐��w�肵���ꍇ...
                data = MemHandleLock(NNshGlobal->hide2.wordmemH);
                for (loop = 0; loop < NNshGlobal->hide2.nofWord; loop++)
                {
                    if (StrStr(buffer, data[loop]) != NULL)
                    {
                        // �L�[���[�h�����I
                        MemHandleUnlock(NNshGlobal->hide2.wordmemH);
                        goto REPLACE_MESSAGE;
                    }
                }
                MemHandleUnlock(NNshGlobal->hide2.wordmemH);
            }
            else
            {
                // NG�L�[���[�h�𕡐��w�肵�Ȃ��ꍇ
                if (StrStr(buffer, (NNshGlobal->NNsiParam)->hideWord2) != NULL)
                {
                    goto REPLACE_MESSAGE;
                }
            }
        }
    }

    // NG�m�F-3�����{����ꍇ�A�A�ADB����Ăяo���đS���R�[�h�̕�������`�F�b�N
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
    {
        // NG���[�h�i�[�pDB�̎擾
        OpenDatabase_NNsh(DBNAME_NGWORD, DBVERSION_NGWORD, &dbRef);
        if (dbRef == 0)
        {
            // ���ODB�̃I�[�v�����s�A�I������
            return;
        }

        // ���R�[�h�����擾
        GetDBCount_NNsh(dbRef, &maxCnt);

        // (�S������)�f�[�^��ǂݏo���āANG�L�[���[�h�`�F�b�N
        for (loop = 0; loop < maxCnt; loop++)
        {
            GetRecordReadOnly_NNsh(dbRef, loop, &dbH, (void **) &dbData);

            // NG�L�[���[�h����v���邩�`�F�b�N
            checkWord = StrStr(buffer, dbData->ngWord);
            if (checkWord != NULL)
            {
                // �L�[���[�h���u�S�́v�`�F�b�N�������ꍇ�A�A�A
                if (dbData->checkArea == NNSH_NGCHECK_ALLAREA)
                {
                    // NG�L�[���[�h�� "�u��"�w�肳��Ă��邩�H
                    if (dbData->matchedAction == 0)
                    {
                        // NG�L�[���[�h����v�����A�\�����Ȃ�
                        ReleaseRecordReadOnly_NNsh(dbRef, dbH);
                        CloseDatabase_NNsh(dbRef);
                        goto REPLACE_MESSAGE;
                    }
                    // �����񂾂���\�����Ȃ�
                    replaceHideWord(buffer, dbData->ngWord);
                    goto NEXT_RECORD;
                }

                area1 = StrStr(buffer, "\n");
                if (area1 != NULL)
                {
                    area2 = StrStr(area1, "\n");
                }
                else
                {
                    area2 = NULL;
                }

                // NG�L�[���[�h�̃`�F�b�N�g���w�肳��Ă����ꍇ...
                matchedWord = 0;
                switch (dbData->checkArea)
                {
                  case NNSH_NGCHECK_NAME_MAIL:
                    // Name����Mail���Ɋ܂܂�Ă��邩�`�F�b�N
                    if (checkWord < area1)
                    {
                        matchedWord = 1;
                    }
                    break;

                  case NNSH_NGCHECK_NOTMESSAGE:
                    // ���b�Z�[�W���ȊO�Ɋ܂܂�Ă��邩�`�F�b�N
                    if (checkWord < area2)
                    {
                        matchedWord = 1;
                    }
                    break;

                  case NNSH_NGCHECK_MESSAGE:
                    // ���b�Z�[�W���Ɋ܂܂�Ă��邩�`�F�b�N
                    if (area2 != NULL)
                    {
                        if (StrStr(area2, dbData->ngWord) != NULL)
                        {
                            // �{����NG���[�h�����݂���
                            matchedWord = 1;
                        }
                    }
                    break;

                  default:
                    // �f�t�H�͂Ƃ肠�����}�b�`
                    matchedWord = 1;
                    break;
                }
                if (matchedWord != 0)
                {
                    // NG�L�[���[�h����v�����A�\�����Ȃ�
                    if (dbData->matchedAction == 0)
                    {
                        ReleaseRecordReadOnly_NNsh(dbRef, dbH);
                        CloseDatabase_NNsh(dbRef);
                        goto REPLACE_MESSAGE;
                    }
                    // �����񂾂���\�����Ȃ�
                    replaceHideWord(buffer, dbData->ngWord);
                }
            }
NEXT_RECORD:
            ReleaseRecordReadOnly_NNsh(dbRef, dbH);
        }
        // ������Ȃ������ADB���N���[�Y����
        CloseDatabase_NNsh(dbRef);
    }
    return;

REPLACE_MESSAGE:
    ptr = StrStr(buffer, " ");
    if (ptr == NULL)
    {
        ptr = buffer;
    }
    else
    {
        ptr++;
    }
    // ���b�Z�[�W��u��������
    StrCopy(ptr, "* * *\n* * *\n");
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : setFavoriteLabel                                           */
/*                                            ���C�ɓ���ݒ�̃��x����ݒ� */
/*-------------------------------------------------------------------------*/
static void setFavoriteLabel(FormType *frm, UInt16 attribute)
{
    ControlType *selP;

    // ���C�ɓ���ݒ�̃Z���N�^�g���K�I�u�W�F�N�g���擾
    selP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SELTRID_FAVOR_THREAD));
    if (selP == NULL)
    {
        // �I�u�W�F�N�g�̎擾���s
        return;
    }

    switch (attribute & NNSH_MSGATTR_FAVOR)
    {
      case NNSH_MSGATTR_FAVOR_L1:
        // ���C�ɓ��背�x��(��)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR_L1);
        break;

      case NNSH_MSGATTR_FAVOR_L2:
        // ���C�ɓ��背�x��(����)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR_L2);
        break;

      case NNSH_MSGATTR_FAVOR_L3:
        // ���C�ɓ��背�x��(��)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR_L3);
        break;

      case NNSH_MSGATTR_FAVOR_L4:
        // ���C�ɓ��背�x��(����)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR_L4);
        break;

      case NNSH_MSGATTR_FAVOR:
        // ���C�ɓ��背�x��(��)
        CtlSetLabel(selP, NNSH_ATTRLABEL_FAVOR);
        break;

      default:
        // ���C�ɓ���ł͂Ȃ�
        CtlSetLabel(selP, NNSH_ATTRLABEL_NOTFAVOR);
        break;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : outputMemoPad                                              */
/*                                                    �������փt�@�C���o�� */
/*-------------------------------------------------------------------------*/
static void outputMemoPad(Boolean useClipboard)
{
    UInt16            len, lp, limit;
    MemHandle         txtH;
    FormType         *frm;
    FieldType        *fldP;
    Char             *buf, *ptr, *txtP;
    NNshMessageIndex *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;

    // �R�s�[�p�o�b�t�@�̈���m�ۂ���
    buf = MEMALLOC_PTR(NNSH_LIMIT_MEMOPAD);
    if (buf == NULL)
    {
        // �������o�͂��L�����Z������
        NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                           " size:", NNSH_LIMIT_MEMOPAD);
        return;
    }

    // �����̐擪�ɃX���^�C�g�����o�͂���
    StrCopy(buf, idxP->threadTitle);
    StrCat (buf, "\n---\n");

    // �������f�[�^���������ޗ̈���^�C�g���̌��ɐݒ肷��
    ptr = buf + StrLen(buf);
    len = NNSH_LIMIT_MEMOPAD - StrLen(buf);

    // �N���b�v�{�[�h�ɂ��镶������������ɏo�͂���H
    if (useClipboard == true)
    {
        txtH = ClipboardGetItem(clipboardText, &lp);
        if ((lp != 0)&&(txtH != 0))
        {
            txtP = MemHandleLock(txtH);
            if (txtP == NULL)
            {
                // �|�C���^���擾�ł��Ȃ��ꍇ�ɂ́A�I������
                goto FUNC_END;
            }
            MemHandleUnlock(txtH);
            
            // �o�͕����񒷂̒���
            if (lp > len)
            {
                lp = len - 1;
            }

            // �N���b�v�{�[�h�̓��e���o�͗p�o�b�t�@�փR�s�[����
            StrNCopy(ptr, txtP, lp);
            ptr[lp] = 0;

            // �X���f�[�^���������֓f���o��
            SendToLocalMemopad(idxP->threadTitle, buf);
       }
        goto FUNC_END;
    }

    // �t�B�[���h�f�[�^���o�b�t�@�ɃR�s�[����
    frm  = FrmGetActiveForm();
    fldP = (FieldType *) FrmGetObjectPtr(frm, 
                                         FrmGetObjectIndex(frm,FLDID_MESSAGE));
    txtH = FldGetTextHandle(fldP);
    if (txtH == 0)
    {
        // Field���m�ۂł��Ȃ������ꍇ�ɂ́A�I������
        goto FUNC_END;
    }
    limit = FldGetTextAllocatedSize(fldP);

    txtP = MemHandleLock(txtH);
    if (txtP == NULL)
    {
        // �|�C���^���擾�ł��Ȃ��ꍇ�ɂ́A�I������
        goto FUNC_END;
    }

    // ���������������M�p�o�b�t�@�ɓ]������
    for (lp = 0; lp < len; lp++)
    {
        if (lp >= limit)
        {
            // �t�B�[���h�T�C�Y�̍ő�l�ɓ��B�����A�I������
            *ptr = 0;
            break;
        }
        if (*txtP == '\0')
        {
            // ������̖����Abreak����
            *ptr = *txtP;
            break;
        }
        if (*txtP == NNSH_CHARCOLOR_ESCAPE)
        {
            // �J���[�p���䕶���͓ǂݔ�΂�
            txtP++;
        }
        else
        {
            // �ʏ�̂P�����R�s�[
            *ptr = *txtP;
            ptr++;
        }
        txtP++;
    }
    MemHandleUnlock(txtH);

    // �X���f�[�^���������֓f���o��
    SendToLocalMemopad(idxP->threadTitle, buf);

FUNC_END:
    // �o�b�t�@�̈���J������
    MEMFREE_PTR(buf);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : outputToFile                                               */
/*                                        ���X���w�肵���t�@�C���֏o�͂��� */
/*-------------------------------------------------------------------------*/
static void outputToFile(Char *fileName, UInt16 fileMode, Boolean isQuote, UInt16 startQuoteLine, Char *appendLine)
{
    UInt32            writeSize;
    UInt16            len, lp, lineCount;
    MemHandle         txtH;
    FormType         *frm;
    FieldType        *fldP;
    Char             *buf, *ptr, *txtP, *startPtr;
    Err               ret;
    NNshFileRef       fileRef;

    // �R�s�[�p�o�b�t�@�̈���m�ۂ���
    buf = MEMALLOC_PTR(NNSH_LIMIT_MEMOPAD + MARGIN);
    if (buf == NULL)
    {
        // �t�@�C���o�͂��L�����Z������
        NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                           " size:", (NNSH_LIMIT_MEMOPAD + MARGIN));
        return;
    }
    MemSet(buf, (NNSH_LIMIT_MEMOPAD + MARGIN), 0x00);
    ptr = buf;
    len = NNSH_LIMIT_MEMOPAD - MARGIN;

    // �t�B�[���h�f�[�^���o�b�t�@�ɃR�s�[����
    frm  = FrmGetActiveForm();
    fldP = (FieldType *) FrmGetObjectPtr(frm, 
                                         FrmGetObjectIndex(frm,FLDID_MESSAGE));
    txtH = FldGetTextHandle(fldP);
    if (txtH == 0)
    {
        // Field���m�ۂł��Ȃ������ꍇ�ɂ́A�I������
        goto FUNC_END;
    }
    txtP = MemHandleLock(txtH);
    if (txtP == NULL)
    {
        // �|�C���^���擾�ł��Ȃ��ꍇ�ɂ́A�I������
        goto FUNC_END;
    }

    // �t�@�C���o�͗p�̃f�[�^���쐬����
    startPtr = buf;
    lineCount = 0;
    lp = 0;
    if (isQuote == true)
    {
        *ptr = '>';
        ptr++;
        lp++;
        *ptr = ' ';
        ptr++;
        lp++;
    }    
    while (lp < len)
    {
        if (*txtP == '\0')
        {
            // ������̖����Abreak����
            *ptr = '\n';
            ptr++;
            *ptr = *txtP;
            break;
        }
        if (*txtP == NNSH_CHARCOLOR_ESCAPE)
        {
            // �J���[�p���䕶���͓ǂݔ�΂�
            txtP++;
        }
        else
        {
            // �ʏ�̂P�����R�s�[
            *ptr = *txtP;
            ptr++;
            lp++;
        }
        if ((*txtP == 0x0a)&&(isQuote == true))
        {
            // ���p��������ݒ肾�����ꍇ...
            lineCount++;
            if (lineCount == startQuoteLine)
            {
                // �擪�̏ꏊ��i�߂�
                StrCopy(ptr, appendLine);
                startPtr = ptr;
                ptr = ptr + StrLen(appendLine);
            }
            *ptr = '>';
            ptr++;
            lp++;
            *ptr = ' ';
            ptr++;
            lp++;
        }
        txtP++;
    }
    MemHandleUnlock(txtH);

    // �擾�����o�b�t�@���t�@�C���ɏ�������
    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
        goto FUNC_END;
    }
    ret = AppendFile_NNsh(&fileRef, StrLen(startPtr), startPtr, &writeSize);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", ret);
    }
    ret = CloseFile_NNsh (&fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "CloseFile() ", " ret:", ret);
    }
    else
    {
        NNsh_DebugMessage(ALTID_INFO, "Save Message", " bytes:", writeSize);
    }

FUNC_END:
    // �o�b�t�@�̈���J������
    MEMFREE_PTR(buf);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : openWebBrowser                                             */
/*                                                       Web�u���E�U�ŊJ�� */
/*-------------------------------------------------------------------------*/
static void openWebBrowser(UInt16 confirmation, Char *buffer, UInt16 size)
{
    MemHandle  memH;
    UInt16     len, savedParam;
    Char      *urlP, *ptr, *dest;

    // �N���b�v�{�[�h�ɂ���f�[�^���擾����
    MemSet(buffer, size, 0x00);
    memH = ClipboardGetItem(clipboardText, &len);
    if ((len != 0)&&(memH != 0))
    {
        urlP = MemHandleLock(memH);
        if (urlP != NULL)
        {
            ptr = StrStr(urlP, "tp://");
            if (ptr != NULL)
            {
                // URL���擾("http://"������)
                ptr = ptr + StrLen("tp://");
                StrCopy(buffer, "http://");
                dest = &buffer[StrLen(buffer)];

                // �N���b�v�{�[�h����URL�̕�����(ASCII)�����𒊏o����
                while ((dest < buffer + size)&&
                        (*ptr > ' ')&&(*ptr < 0x7f)&&
                        (*ptr != '<')&&(*ptr != '>')&&(*ptr != '`')&&
                        (*ptr != '{')&&(*ptr != '}')&&(*ptr != '^')&&
                        (*ptr != '|')&&(*ptr != '"')&&                        
                        (ptr  < urlP + len))
                {
                    *dest = *ptr;
                    dest++;
                    ptr++;
                }
            }
            MemHandleUnlock(memH);                
        }
    }
    if (buffer[0] != 0x00)
    {
        // NetFront���w�肳�ꂽURL�ŋN�����邩�m�F����
        savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
        (NNshGlobal->NNsiParam)->confirmationDisable = confirmation;
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, buffer,
                                MSG_WEBLAUNCH_CONFIRM, 0) == 0)
        {
            (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

            // NetFront���N������(NNsi�I����ɋN��)
            (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                     0, 0, 
                                     NNshGlobal->browserLaunchCode,
                                     buffer, StrLen(buffer), NULL);
        }
        (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : jumpPrevious                                               */
/*                                                �P�O�̃��b�Z�[�W�֖߂� */
/*-------------------------------------------------------------------------*/
static void jumpPrevious(void)
{
    NNshMessageIndex *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;
 
    // 1��ł��W�����v���Ă���Ζ߂�
    if (idxP->prevMsg[idxP->histIdx] != 0)
    {
        idxP->openMsg = idxP->prevMsg[idxP->histIdx];
        idxP->histIdx = (idxP->histIdx != 0) ?
                                       (idxP->histIdx - 1) : (PREVBUF - 1);
    }
    (void) displayMessage(NNshGlobal->msgIndex, 0);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : jumpEdge                                                   */
/*                                             ���b�Z�[�W�̈ړ�(�擪/����) */
/*-------------------------------------------------------------------------*/
static void jumpEdge(Boolean loc)
{
    NNshMessageIndex *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;

    idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ? (idxP->histIdx + 1) : 0;
    idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
    if (loc == false)
    {
        // �X���̐擪��
        idxP->openMsg    = 1;
    }
    else
    {
        // �X���̖�����
        idxP->openMsg    = idxP->nofMsg;
    }
    (void) displayMessage(NNshGlobal->msgIndex, 0);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : parseMessage                                               */
/*                                                  (���b�Z�[�W�̐��`����) */
/*-------------------------------------------------------------------------*/
static void parseMessage(Char *buf, Char *source, UInt32 size,
                         UInt16 *nofJmp, UInt16 *jmpBuf, UInt16 kanjiCode,
                         Boolean setJumpFlag)
{
    Boolean kanjiMode;
    UInt32 dataStatus, jmp;
    UInt16 len, fontFlag, parseLen; 
    Char *ptr, *dst, *num, *tblP;

#ifdef USE_COLOR
    if (((NNshGlobal->NNsiParam)->useColor != 0)&&
        ((NNshGlobal->NNsiParam)->useColorMessageToken != 0)&&
        ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0))
    {
        fontFlag = NNSH_ENABLE;
    }
    else
#endif  //#ifdef USE_COLOR
    {
        fontFlag = NNSH_DISABLE;
    }

    dataStatus = MSGSTATUS_NAME;
    kanjiMode = false;

    // ���x�����߃��[�`������... (�P�����Âp�[�X����)
    dst  = buf;
    ptr  = source;
    parseLen = 0;

    // �w�b�_�F�ɂ���
    if (fontFlag != NNSH_DISABLE)
    {
        *dst++ = NNSH_CHARCOLOR_ESCAPE;
        *dst++ = NNSH_CHARCOLOR_HEADER;
    }
    while (ptr < (source + size))
    {
        // JIS�����R�[�h�̕ϊ����W�b�N
        if (kanjiCode == NNSH_KANJICODE_JIS)
        {
            // JIS��SHIFT JIS�̃R�[�h�ϊ�
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x24')&&(*(ptr + 2) == '\x42'))
            {
                // �������[�h�ɐ؂�ւ�
                kanjiMode = true;
                ptr = ptr + 3;
                continue;
            }
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x28')&&(*(ptr + 2) == '\x42'))
            {
                // ANK���[�h�ɐ؂�ւ�
                kanjiMode = false;
                ptr = ptr + 3;
                continue;
            }
            if (kanjiMode == true)
            {
                // JIS > SJIS �����ϊ�
                ConvertJIStoSJ(dst, ptr);
                ptr = ptr + 2;
                dst = dst + 2;
                continue;
            }
        }
        if (*ptr == '&')
        {
            // "&gt;" �� '>' �ɒu��
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                if ((setJumpFlag == true) && (*ptr <= '9')&&(*ptr >= '0'))
                {
                    // �X���Ԃ��擾
                    num = ptr;
                    jmp = 0;
                    while ((*num <= '9')&&(*num >= '0'))
                    {
                        jmp = jmp * 10 + (*num - '0');
                        num++;
                    }
                    if ((*nofJmp < JUMPBUF)&&(jmp != 0))
                    {
                        // �X���Ԃ��o�b�t�@�ɒǉ�
                        jmpBuf[*nofJmp] = jmp;
                        (*nofJmp)++;
                    }
                }
                continue;
            }
            // "&lt;" �� '<' �ɒu��
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '<';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&quot;" �� '"' �ɒu��
            if ((*(ptr + 1) == 'q')&&(*(ptr + 2) == 'u')&&
                (*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&(*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" �� '    ' �ɒu��
            if ((*(ptr + 1) == 'n')&&(*(ptr + 2) == 'b')&&
                (*(ptr + 3) == 's')&&(*(ptr + 4) == 'p')&&(*(ptr + 5) == ';'))
            {
                *dst++ = ' ';
/*
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
*/
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&amp;" �� '&' �ɒu��
            if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
            {
                // �M�R�̂����ەϊ��΍�... (��������)
                if ((*(ptr + 5) == 'g')&&(*(ptr + 6) == 't')&&(*(ptr + 7) == ';'))
                {
                    *dst++ = '>';
                    ptr = ptr + 8;    // StrLen(TAG_AMP)+ 'g' + 't' + ';';
                    if ((setJumpFlag == true) && (*ptr <= '9')&&(*ptr >= '0'))
                    {
                        // �X���Ԃ��擾
                        num = ptr;
                        jmp = 0;
                        while ((jmp < NNSH_MESSAGE_LIMIT)&&
                               (*num <= '9')&&(*num >= '0'))
                        {
                            jmp = jmp * 10 + (*num - '0');
                            num++;
                        }
                        if ((*nofJmp < JUMPBUF)&&(jmp != 0))
                        {
                            // �X���Ԃ��o�b�t�@�ɒǉ�
                            jmpBuf[*nofJmp] = jmp;
                            (*nofJmp)++;
                        }
                    }
                    continue;
                }
                // �M�R�̂����ەϊ��΍�... (�����܂�)

                *dst++ = '&';
                ptr = ptr + 5;    // StrLen(TAG_AMP);
                continue;
            }

            if (*(ptr + 1) == 'a')
            {
/*
                // "&amp;" �� '&' �ɒu��
                if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                    (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
                {
                    *dst++ = '&';
                    ptr = ptr + 5;    // StrLen(TAG_AMP);
                    continue;
                }
*/
                // "&acute;" �� '�L' �ɒu��
                if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'c')&&
                    (*(ptr + 3) == 'u')&&(*(ptr + 4) == 't')&&
                    (*(ptr + 5) == 'e')&&(*(ptr + 6) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0x4C;
                    ptr = ptr + 7;    // StrLen(�L);
                    continue;
                }
                // "&alpha;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'l')&&
                    (*(ptr + 3) == 'p')&&(*(ptr + 4) == 'h')&&
                    (*(ptr + 5) == 'a')&&(*(ptr + 6) == ';'))
                {
                    *dst++ = 0x83;
                    *dst++ = 0xBF;
                    ptr = ptr + 7;    // StrLen(TAG_ALPHA);
                    continue;
                }
                // �ϊ��ł��Ȃ������ӏ��B
                *dst++ = '&';
                ptr++;
                *dst++ = 'a';
                ptr++;
                continue;
            }
            // "&beta;" �� '��' �ɒu��
            if ((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'e')&&
                (*(ptr + 3) == 't')&&(*(ptr + 4) == 'a')&&
                (*(ptr + 5) == ';'))
            {
                *dst++ = 0x83;
                *dst++ = 0xC0;
                ptr = ptr + 6;    // StrLen(TAG_BETA);
                continue;
            }
            // "&clubs;" �� '��' �ɒu�� (FONT�Ȃ��̂�...)
            if ((*(ptr + 1) == 'c')&&(*(ptr + 2) == 'l')&&
                (*(ptr + 3) == 'u')&&(*(ptr + 4) == 'b')&&
                (*(ptr + 5) == 's')&&(*(ptr + 6) == ';'))
            {
                *dst++ = 0x81;
                *dst++ = 0xA6;
                ptr = ptr + 7;    // StrLen(TAG_CLUBS);
                continue;
            }
            
            // "&d"
            if (*(ptr + 1) == 'd')
            {
                // "&diams;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'i')&&
                    (*(ptr + 3) == 'a')&&(*(ptr + 4) == 'm')&&
                    (*(ptr + 5) == 's')&&(*(ptr + 6) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0x9F;
                    ptr = ptr + 7;    // StrLen(TAG_DIAMS);
                    continue;
                }
                // "&darr;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xAB;
                    ptr = ptr + 6;    // StrLen(TAG_DIAMS);
                    continue;
                }
                // "&dArr;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xAB;
                    ptr = ptr + 6;    // StrLen(TAG_DIAMS);
                    continue;
                }

                // �ϊ��ł��Ȃ������ӏ��B
                *dst++ = '&';
                ptr++;
                *dst++ = 'd';
                ptr++;
                continue;
            }            
            
            if (*(ptr + 1) == 'h')
            {
                // "&hellip;" �� '���' �ɒu��
                if ((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'e')&&
                    (*(ptr + 3) == 'l')&&(*(ptr + 4) == 'l')&&
                    (*(ptr + 5) == 'i')&&(*(ptr + 6) == 'p')&&
                    (*(ptr + 7) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0x63;
                    ptr = ptr + 8;    // StrLen(TAG_...)
                    continue;
                }
                if ((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xCC;
                    ptr = ptr + 6;    // StrLen(TAG_...)
                    continue;
                }
                if ((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xCC;
                    ptr = ptr + 6;    // StrLen(TAG_...)
                    continue;
                }
                if ((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'e')&&
                    (*(ptr + 3) == 'a')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == 't')&&(*(ptr + 6) == 's')&&
                    (*(ptr + 7) == ';'))
                {
                    // �{���̓n�[�g�������ɂ��Ă݂��B
                    *dst++ = 0x81;
                    *dst++ = 0x99;
                    ptr = ptr + 8;    // StrLen(TAG_...)
                    continue;
                }

                // �ϊ��ł��Ȃ������ӏ��B
                *dst++ = '&';
                ptr++;
                *dst++ = 'h';
                ptr++;
                continue;
            }
            // "&r"
            if (*(ptr + 1) == 'r')
            {
                // "&rarr;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'r')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xA8;
                    ptr = ptr + 6;    // StrLen(��);
                    continue;
                }
                 // "&rdquo;" �� '�h' �ɒu��
                if ((*(ptr + 1) == 'r')&&(*(ptr + 2) == 'd')&&
                    (*(ptr + 3) == 'q')&&(*(ptr + 4) == 'u')&&
                    (*(ptr + 5) == 'o')&&(*(ptr + 6) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0x68;
                    ptr = ptr + 7;    // StrLen(�L);
                    continue;
                }
                // "&rArr;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'r')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xCB;
                    ptr = ptr + 6;    // StrLen(��);
                    continue;
                }
                // �ϊ��ł��Ȃ������ӏ��B
                *dst++ = '&';
                ptr++;
                *dst++ = 'd';
                ptr++;
                continue;
            }
            
            if ((*(ptr + 1) == 's')&&(*(ptr + 2) == 'p')&&
                 (*(ptr + 3) == 'a')&&(*(ptr + 4) == 'd')&&
                 (*(ptr + 5) == 'e')&&(*(ptr + 6) == 's')&&
                 (*(ptr + 7) == ';'))
            {
                // �{���̓X�y�[�h�������ɂ��Ă݂��B
                *dst++ = 0x81;
                *dst++ = 0xA6;
                ptr = ptr + 8;    // StrLen(TAG_...)
                continue;
            }

             // "&omega;" �� '��' �ɒu��
            if ((*(ptr + 1) == 'o')&&(*(ptr + 2) == 'm')&&
                (*(ptr + 3) == 'e')&&(*(ptr + 4) == 'g')&&
                (*(ptr + 5) == 'a')&&(*(ptr + 6) == ';'))
            {
                *dst++ = 0x83;
                *dst++ = 0xD6;
                ptr = ptr + 7;    // StrLen(�L);
                continue;
            }

            // "&plusmn;" �� �} �֕ϊ�
            if ((*(ptr + 1) == 'p')&&(*(ptr + 2) == 'l')&&
                 (*(ptr + 3) == 'u')&&(*(ptr + 4) == 's')&&
                 (*(ptr + 5) == 'm')&&(*(ptr + 6) == 'n')&&
                 (*(ptr + 7) == ';'))
            {
                // "�}"
                *dst++ = 0x81;
                *dst++ = 0x7D;
                ptr = ptr + 8;    // StrLen(TAG_...)
                continue;
            }

            // "&l"
            if (*(ptr + 1) == 'l')
            {
                // "&larr;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xA9;
                    ptr = ptr + 6;    // StrLen(TAG_DIAMS);
                    continue;
                }
                // "&lArr;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xA9;
                    ptr = ptr + 6;    // StrLen(TAG_DIAMS);
                    continue;
                }

                // �ϊ��ł��Ȃ������ӏ��B
                *dst++ = '&';
                ptr++;
                *dst++ = 'l';
                ptr++;
                continue;
            }            

            // "&u"
            if (*(ptr + 1) == 'u')
            {
                // "&uarr;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'u')&&(*(ptr + 2) == 'a')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xAA;
                    ptr = ptr + 7;    // StrLen(TAG_DIAMS);
                    continue;
                }
                // "&uArr;" �� '��' �ɒu��
                if ((*(ptr + 1) == 'u')&&(*(ptr + 2) == 'A')&&
                    (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'r')&&
                    (*(ptr + 5) == ';'))
                {
                    *dst++ = 0x81;
                    *dst++ = 0xAA;
                    ptr = ptr + 7;    // StrLen(TAG_DIAMS);
                    continue;
                }

                // �ϊ��ł��Ȃ������ӏ��B
                *dst++ = '&';
                ptr++;
                *dst++ = 'u';
                ptr++;
                continue;
            }            

            // "&yen;" �� '��' �ɒu��
            if ((*(ptr + 1) == 'y')&&(*(ptr + 2) == 'e')&&
                (*(ptr + 3) == 'n')&&(*(ptr + 4) == ';'))
            {
                *dst++ = 0x81;
                *dst++ = 0x8F;
                ptr = ptr + 5;    // StrLen(�L);
                continue;
            }
             // "&Pi;" �� '��' �ɒu��
            if ((*(ptr + 1) == 'P')&&(*(ptr + 2) == 'i')&&
                (*(ptr + 3) == ';'))
            {
                *dst++ = 0x83;
                *dst++ = 0xAE;
                ptr = ptr + 4;    // StrLen(��);
                continue;
            }

            // ����͂��肦�Ȃ��͂�����...�ꉞ�B
            *dst++ = '&';
            ptr++;
            continue;
        }
        if (*ptr == '<')
        {
            //  "<>" �́A�Z�p���[�^(��Ԃɂ���ĕς��)
            if (*(ptr + 1) == '>')
            {
                ptr = ptr + 2;   // StrLen(DATA_SEPARATOR);
                switch (dataStatus)
                {
                  case MSGSTATUS_NAME:
                    // ���O���̋�؂�
                    if (((NNshGlobal->NNsiParam)->useNameOneLine != 0)&&(parseLen >= NNSH_MAX_OMIT_LENGTH))
                    {
                        *dst++ = chrEllipsis;
                        *dst++ = ' ';
                        *dst++ = ' ';
                    }
                    else
                    {
                        *dst++ = ' ';
                        *dst++ = ' ';
                        *dst++ = ' ';
                    }
                    dataStatus = MSGSTATUS_EMAIL;
                    parseLen = 0;

                    // �w�b�_�F�ɂ���
                    if (fontFlag != NNSH_DISABLE)
                    {
                        *dst++ = NNSH_CHARCOLOR_ESCAPE;
                        *dst++ = NNSH_CHARCOLOR_BOLDHEADER;
                    }
                    break;

                  case MSGSTATUS_EMAIL:
                    // e-mail���̋�؂�
                    if ((NNshGlobal->NNsiParam)->useNameOneLine != 0)
                    {
                        if (parseLen >= NNSH_MAX_OMIT_LENGTH)
                        {
                            *dst++ = chrEllipsis;
                        }
                    }
                    else
                    {
                        *dst++ = '\n';
                    }
                    dataStatus = MSGSTATUS_DATE;
                    parseLen = 0;

                    // �w�b�_�F�ɂ���
                    if (fontFlag != NNSH_DISABLE)
                    {
                        *dst++ = NNSH_CHARCOLOR_ESCAPE;
                        *dst++ = NNSH_CHARCOLOR_INFORM;
                    }
                    break;

                  case MSGSTATUS_DATE:
                    // �N�����E���Ԃ���тh�c���̋�؂�
                    if ((NNshGlobal->NNsiParam)->useNameOneLine != 0)
                    {
                        *dst++ = '\n';
                    }
                    else
                    {
                        *dst++ = '\n';
                        *dst++ = '\n';
                    }
                    dataStatus = MSGSTATUS_NORMAL;
                    if (*ptr == ' ')
                    {
                        // �X�y�[�X�͓ǂݔ�΂�
                        ptr++;
                    }
                    // �ʏ�F�ɂ���
                    if (fontFlag != NNSH_DISABLE)
                    {
                        *dst++ = NNSH_CHARCOLOR_ESCAPE;
                        *dst++ = NNSH_CHARCOLOR_NORMAL;
                    }
                    break;

                  case MSGSTATUS_NORMAL:
                  default:
                    // ���b�Z�[�W�̏I�[
                    *dst++ = ' ';
                    *dst++ = '\0';
                    return;
                    break;
                }
                continue;
            }
            //  "<br>" �́A���s�ɒu��
            if (((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                // �s���ƍs���̃X�y�[�X������Ă݂�ꍇ
                if ((ptr > source)&&(*(ptr - 1) == ' '))
                {
                    dst--;
                }
                if (*(ptr + 4) == ' ')
                {
                    *dst++ = '\n';
                    ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                    continue;
                }
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<p*>" �́A���s2�ɒu��
            if ((*(ptr + 1) == 'p')||(*(ptr + 1) == 'P'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 2;
                while (*ptr != '>')
                {
                    ptr++;
                    if (ptr >= (source + size))
                    {
                        // �m�ۂ����̈���z����...
                        break;
                    }
                }
                ptr++;
                continue;
            }

            // <li>�^�O�����s�R�[�h��:�ɒu������
            if (((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'i')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'L')&&(*(ptr + 2) == 'I')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '�';
                ptr = ptr + 4;
                continue;
            }   

            //  "<hr>" �́A���s === ���s �ɒu��
            if (((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'H')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dt>"�́A���s�ɒu��
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "</tr>" ����� "</td>" �́A���s�ɒu��
            if (((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'r')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'R')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'd')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'D')&&(*(ptr + 4) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dd>" �́A���s�Ƌ󔒂S�ɒu��
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'd')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'D')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<h?" �́A���s + ���s �ɒu��
            if ((*(ptr + 1) == 'h')&&(*(ptr + 1) == 'H'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                // �����F�ɂ���
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_HEADER;
                }
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "</h?>" �́A���s + ���s �ɒu��
            if (((*(ptr + 2) == 'h')||(*(ptr + 2) == 'H'))&&(*(ptr + 1) == '/'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                // �ʏ�F�ɂ���
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            // <a* �̓A���J�[�F��
            if (((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&(*(ptr + 2) == ' '))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_ANCHOR;
                }
                goto SKIP_TAG;
            }

            // </a* �͒ʏ�F��
            if (((*(ptr + 2) == 'a')||(*(ptr + 2) == 'A'))&&(*(ptr + 1) == '/'))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                goto SKIP_TAG;
            }

            // <EM* �͋����F��
            if (((*(ptr + 1) == 'e')||(*(ptr + 1) == 'E'))&&
                ((*(ptr + 2) == 'm')||(*(ptr + 2) == 'M')))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_BOLDDATA;
                }
                goto SKIP_TAG;
            }

            // </em* �͒ʏ�F��
            if (((*(ptr + 2) == 'e')||(*(ptr + 2) == 'E'))&&
                ((*(ptr + 3) == 'm')||(*(ptr + 3) == 'M'))&&
                (*(ptr + 1) == '/'))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                goto SKIP_TAG;
            }

            // <tr>, <td> �̓X�y�[�X�P�ɕϊ�
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R')||
                 (*(ptr + 2) == 'd')||(*(ptr + 2) == 'D')))
            {
                *dst++ = ' ';
                goto SKIP_TAG;
            }

            //  "<SCRIPT>�`</SCRIPT>" �͕\�����Ȃ�(�ǂݔ�΂�)
            if ((*(ptr + 1) == 'S')&&(*(ptr + 2) == 'C')&&
                (*(ptr + 3) == 'R')&&(*(ptr + 4) == 'I')&&
                (*(ptr + 5) == 'P')&&(*(ptr + 6) == 'T'))
            {
                ptr = ptr + 7;
                tblP = ptr; 
                while (*ptr != '\0')
                {
                    if ((*(ptr + 0) == '<')&&(*(ptr + 1) == '/')&&
                        (*(ptr + 2) == 'S')&&(*(ptr + 3) == 'C')&&
                        (*(ptr + 4) == 'R')&&(*(ptr + 5) == 'I')&&
                        (*(ptr + 6) == 'P')&&(*(ptr + 7) == 'T'))
                    {
                        // �X�N���v�g�^�O�̖����𔭌��A�^�O�ǂݔ�΂����[�h��
                        goto SKIP_TAG;
                    }
                    ptr++;
                }
                // </SCRIPT>�̖�����������Ȃ������A�A�A�^�O�ʒu��߂�..
                ptr = tblP;                
                goto SKIP_TAG;
            }

            //  "<script>�`</script>" �͕\�����Ȃ�(�ǂݔ�΂�)
            if ((*(ptr + 1) == 's')&&(*(ptr + 2) == 'c')&&
                (*(ptr + 3) == 'r')&&(*(ptr + 4) == 'i')&&
                (*(ptr + 5) == 'p')&&(*(ptr + 6) == 't'))
            {
                ptr = ptr + 7;
                tblP = ptr; 
                while (*ptr != '\0')
                {
                    if ((*(ptr + 0) == '<')&&(*(ptr + 1) == '/')&&
                        (*(ptr + 2) == 's')&&(*(ptr + 3) == 'c')&&
                        (*(ptr + 4) == 'r')&&(*(ptr + 5) == 'i')&&
                        (*(ptr + 6) == 'p')&&(*(ptr + 7) == 't'))
                    {
                        // �X�N���v�g�^�O�̖����𔭌��A�^�O�ǂݔ�΂����[�h��
                        goto SKIP_TAG;
                    }
                    ptr++;
                }
                // </script>�̖�����������Ȃ������A�A�A�^�O�ʒu��߂�..
                ptr = tblP;
                goto SKIP_TAG;
            }

            // <BE: �́ABE ID�̂��ߓǂݔ�΂��Ȃ��B(��ʂɕ\������)
            if ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'E')&&
                (*(ptr + 3) == ':'))
            {
                *dst++ = ' ';
                ptr++;
                while ((*ptr != '>')&&(*ptr != '\0'))
                {
                    *dst++ = *ptr;
                    ptr++;                    
                }
                ptr++;
                continue;
            }

SKIP_TAG:
            // ���̑��̃^�O�͓ǂݔ�΂�
#ifdef USE_STRSTR
            ptr = StrStr(ptr, ">");
#else
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
#endif
            ptr++;
            // *dst++ = ' ';   // �^�O�͊��S�����Ƃ���B
            continue;
        }
        // �����R�[�h��EUC�������ꍇ...
        if (kanjiCode == NNSH_KANJICODE_EUC)
        {
            if (ConvertEUCtoSJ((UInt8 *) dst, (UInt8 *) ptr, &len) == true)
            {
                // EUC > SHIFT JIS�ϊ������s����
                dst = dst + len;
                ptr = ptr + 2;
                continue;
            }
            if (*ptr != '\0')
            {
                // ���ʂ̈ꕶ���]��
                *dst++ = *ptr;
            }
            ptr++;
            continue;
        }
        if ((NNshGlobal->NNsiParam)->convertHanZen != 0)
        {
            if (((UInt8) *ptr >= 0x81)&&((UInt8) *ptr <= 0x9f))
            {
                // 2�o�C�g�����Ɣ���
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }
            if (((UInt8) *ptr >= 0xe0)&&((UInt8) *ptr <= 0xef))
            {
                // 2�o�C�g�����Ɣ���
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }
            // ���p�J�i���S�p�ϊ�
            if (ConvertHanZen((UInt8 *) dst, (UInt8 *) ptr) == true)
            {
                dst = dst + 2;
                ptr++;
                continue;
            }
        }

        // �X�y�[�X���A�����Ă����ꍇ�A�P�Ɍ��炷
        if ((*ptr == ' ')&&(*(ptr + 1) == ' '))
        {
            ptr++;
            while ((*ptr == ' ')&&(ptr < (source + size)))
            {
                ptr++;
            }
            ptr--;
        }

        // NULL ����� 0x0a, 0x0d, 0x09(�^�u) �͖�������
        if ((*ptr != '\0')&&(*ptr != 0x0a)&&(*ptr != 0x0d)&&(*ptr != 0x09))
        {
            switch (dataStatus)
            {
              case MSGSTATUS_NAME:
                if (((NNshGlobal->NNsiParam)->useNameOneLine == 0)||
                    (((NNshGlobal->NNsiParam)->useNameOneLine != 0)&&(parseLen <= NNSH_MAX_OMIT_LENGTH)))
                {
                    // �ꕶ���]��
                    *dst++ = *ptr;
                }
                parseLen++;
                break;

              case MSGSTATUS_EMAIL:
                if (((NNshGlobal->NNsiParam)->useNameOneLine == 0)||
                    (((NNshGlobal->NNsiParam)->useNameOneLine != 0)&&(parseLen <= NNSH_MAX_OMIT_LENGTH)))
                {
                    // �ꕶ���]��
                    *dst++ = *ptr;
                }
                parseLen++;
                break;

              case MSGSTATUS_DATE:
                if ((NNshGlobal->NNsiParam)->useNameOneLine != 0)
                {
                    // �]�ʂ��Ȃ�...
                }
                else
                {
                    // �ꕶ���]��
                    *dst++ = *ptr;
                }
                break;

              case MSGSTATUS_NORMAL:
              default:
                // ���ʂ̈ꕶ���]��
                *dst++ = *ptr;
                break;
            }
        }
        ptr++;
    }
    *dst++ = '\0';
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : changeFont                                                 */
/*                                                          �t�H���g�̕ύX */
/*-------------------------------------------------------------------------*/
static void changeFont(void)
{
    FormType  *frm;

#ifdef USE_TSPATCH
    UInt32 fontId, ver;
    Err    ret;
#endif

    frm = FrmGetActiveForm();

    // �t�H���gID�̑I��
    if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
    {
        // SONY�n�C���]�t�H���g��ύX����
        (NNshGlobal->NNsiParam)->sonyHRFont = FontSelect((NNshGlobal->NNsiParam)->sonyHRFont);

        // �t�B�[���h�̕`��(�n�C���]���)
        NNsi_HRFldDrawField(FrmGetObjectPtr(frm, 
                              FrmGetObjectIndex(frm, FLDID_MESSAGE)), 0);
        // �X�N���[���o�[�̍X�V
        NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, SCLID_MESSAGE)));
    }
    else
    {
        // �ʏ�t�H���g��ύX����
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

        // �t�B�[���h�̕`��(�ʏ���)
        FldSetFont(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE)),
                                   (NNshGlobal->NNsiParam)->currentFont);

        // �X�N���[���o�[���X�V���āA�ʏ�̉�ʕ`����s��
        NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);
        FrmDrawForm(frm);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : ReadMessage_Close                                          */
/*                                                      �Q�Ɖ�ʂ̃N���[�Y */
/*-------------------------------------------------------------------------*/
void ReadMessage_Close(UInt16 nextFormID)
{
    NNshMessageIndex *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;
    if (idxP != NULL)
    {
        // �u������ݒ�v����Ă����ꍇ�A
        if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
        {
            if (NNshGlobal->bookmarkNick != NULL)
            {
                // ������ݒ肳��Ă�������L������
                StrCopy(NNshGlobal->bookmarkNick,
                        idxP->boardNick);
            }
            if (NNshGlobal->bookmarkFileName != NULL)
            {
                // ������ݒ肳��Ă���X���h�c���L������
                StrCopy(NNshGlobal->bookmarkFileName,
                        idxP->threadFile);
            }
        }

        MEMFREE_PTR((NNshGlobal->msgIndex)->msgOffset);

        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(NNshGlobal->msgIndex);
    }

    // �n�C���]�p�f�[�^�����l���N���A����
    NNshGlobal->totalLine   = 0;
    NNshGlobal->nofPage     = 0;
    NNshGlobal->currentPage = 0;
    NNshGlobal->prevHRLines = 0;

#ifdef USE_COLOR
    // �t�H�[�����I�[�v�������Ƃ��ɍX�V�����F�����ɖ߂�
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // �F(�O�i�F�A�����F�A�w�i�F)�����ɖ߂�
        UIColorSetTableEntry(UIObjectForeground, &NNshGlobal->fgColor);
        UIColorSetTableEntry(UIFieldText,        &NNshGlobal->txColor);
        UIColorSetTableEntry(UIFieldBackground,  &NNshGlobal->bgColor);
        UIColorSetTableEntry(UIFormFill,         &NNshGlobal->bgFormColor);
        UIColorSetTableEntry(UIDialogFill,       &NNshGlobal->bgDialogColor);
        UIColorSetTableEntry(UIAlertFill,        &NNshGlobal->bgAlertColor);
        UIColorSetTableEntry(UIObjectFill,       &NNshGlobal->bgObjectColor);

        // �O�i�F�Ɣw�i�F��ݒ肷��
        WinSetForeColor(UIColorGetTableEntryIndex(UIFieldText));
        WinSetTextColor(UIColorGetTableEntryIndex(UIFieldText));
        WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));
    }
#endif

    if (nextFormID != FRMID_MESSAGE)
    {
        NNshGlobal->tokenId = 0;  // �Q�Ɖ�ʂ̋�؂�g�[�N�����N���A
    }

    // �X���I����ʂ��J��
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(nextFormID);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : displayMessageSub                                          */
/*                                                   ���b�Z�[�W�\�����C��  */
/*-------------------------------------------------------------------------*/
static void displayMessageSub(UInt16 bbsType, Char *topP, UInt32 mesNum,
                              UInt32 offset,  UInt32 size, Char *buf,
                              Boolean setJumpFlag)
{
    Char        *ptr, *nameP, *htmlP;
    UInt32       msgSize;
    UInt16       kanjiCode, resNum;
#ifdef USE_ARMLET
    UInt32       armArgs[9], nofJump, lp, jmpMsg[JUMPBUF], hanzen;
    MemHandle    armH;
    void        *armP;
#endif
    NNshMessageIndex *idxP;
    idxP = NNshGlobal->msgIndex;

    // �����R�[�h�̐ݒ�ƃ��b�Z�[�W�̐��`����
    kanjiCode = NNSH_KANJICODE_SHIFTJIS;
    switch (bbsType & NNSH_BBSTYPE_MASK)
    {
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        kanjiCode = NNSH_KANJICODE_EUC;
        ptr = StrStr((topP + offset), NNSH_JBBSTOKEN_MSGSTART);
        if (ptr == NULL)
        {
            ptr = topP + offset;
        }
        else
        {
            ptr = ptr + StrLen(NNSH_JBBSTOKEN_MSGSTART);
        }
        // ���b�Z�[�W�T�C�Y�̌������킹... (�Ō�̂Q�A���ĂȂ񂾂����H)
        // msgSize = size - (ptr - (topP + offset)) - 2;
        msgSize = size - (ptr - (topP + offset));
        break;

      case NNSH_BBSTYPE_MACHIBBS:
        if (mesNum != 1)
        {
            // �擪�ɂ���A"<dt>"���������... ���X�ԍ��ɂ��āA(�����ꍇ�����邯��)�\������悤�ɕύX����B
            // �i���X�ԍ�������\�����Ȃ��悤�ɂ���ɂ́ANNSH_MACHITOKEN_MSGSTART2 �� NNSH_MACHITOKEN_MSGSTART �ɂ���j
            ptr = StrStr((topP + offset), NNSH_MACHITOKEN_MSGSTART2);
            if (ptr == NULL)
            {
                ptr = topP + offset;
            }
            else
            {
                ptr = ptr + StrLen(NNSH_MACHITOKEN_MSGSTART2);
            }
        }
        else
        {
            // �擪���b�Z�[�W�̏ꍇ...
            ptr = topP + offset;
        }
        // ���b�Z�[�W�T�C�Y�̌������킹...
        msgSize = size - (ptr - (topP + offset)) - 2;
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        kanjiCode = NNSH_KANJICODE_EUC;
        ptr     = (topP + offset);

        // RAW���[�h�̃f�[�^�\������
        nameP  = StrStr((ptr + 1), NNSH_JBBSTOKEN_RAW_START_RES);
        if (nameP != NULL)
        {
            ptr = nameP + sizeof(NNSH_JBBSTOKEN_RAW_START_RES) - 1;
        }
        msgSize = size;
        break;

      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_SHITARABA:
        kanjiCode = NNSH_KANJICODE_EUC;
        ptr     = (topP + offset);
        msgSize = size;
        break;

      case NNSH_BBSTYPE_HTML:
        // �����R�[�h���E��
        kanjiCode = (NNSH_BBSTYPE_CHARSETMASK & bbsType) >> NNSH_BBSTYPE_CHAR_SHIFT;
        // not break;
#if 0
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif

      default:
        ptr     = (topP + offset);
        msgSize = size;
        break;
    }

    // �擪�Ƀ��b�Z�[�W�ԍ���\��
    if ((NNshGlobal->NNsiParam)->boldMessageNum != 0)
    {
        StrCopy(buf, "�y");
        NUMCATI(buf, mesNum);
        StrCat (buf, "�z ");
    }
    else
    {
        StrCopy(buf, "[");
        NUMCATI(buf, mesNum);
        StrCat (buf, "] ");
    }

    // HTML�^�C�v�̃f�[�^��������A��s�ڂɂ̓f�[�^��\�����Ȃ��悤�ɏ��׍H
    switch ((bbsType & NNSH_BBSTYPE_MASK))
    {
      case NNSH_BBSTYPE_HTML:
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        // �w�b�_�F�����ɖ߂����߂̍׍H...
        StrCat(buf, "\n");
        htmlP = buf + StrLen(buf);
        break;

      case NNSH_BBSTYPE_MACHIBBS:
        // �w�b�_�F�����ɖ߂����߂̍׍H...
        StrCat(buf, "\n");
        htmlP = buf + StrLen(buf);
        break;

      default:
        htmlP = NULL;
        break;
    }

    // ���O���̔��p���������X�ԂƂ��Ēǉ�����
    // (�����ɓ����̂́A�A�A�Ƃ肠����������...)
    nameP = ptr;
    while ((*nameP == ' ')||(*nameP == '\x0d'))
    { 
        nameP++;
    }
    resNum = 0;
    if ((*nameP >= '1')&&(*nameP <= '9'))
    {
        resNum = *nameP - '0';
        nameP++;
        while ((*nameP >= '0')&&(*nameP <= '9'))
        {
            resNum = resNum * 10 + *nameP - '0';
            nameP++;
        }

        if (setJumpFlag == true)
        {
            // ���O���̐������i�[
            idxP->jumpMsg[idxP->nofJump] = resNum;
            (idxP->nofJump)++;
        }
    }
    
    // RAW-disp���[�h�̃`�F�b�N
    if ((NNshGlobal->NNsiParam)->rawDisplayMode != 0)
    {
        // RAW-display���[�h�A������͒P���R�s�[�Ƃ���
        StrNCopy((buf + StrLen(buf)), ptr, msgSize);
        return;
    }

#ifdef USE_LOGCHARGE
    // UTF8�ϊ�DB���C���X�g�[������Ă��Ă��A�����R�[�h��UTF8�������Ƃ�...
    if ((kanjiCode == NNSH_KANJICODE_UTF8)&&(DmFindDatabase(0, "UnicodeToJis0208") != 0))
    {
        // RDF/UTF8�̉�͏���...
        Show_BusyForm(MSG_MESSAGE_ENTRY);  // "���b�Z�[�W��ϊ���..." �\��
        ParseMessage_UTF8((buf + StrLen(buf)), ptr, msgSize,
                         &(idxP->nofJump), &(idxP->jumpMsg[0]),
                         kanjiCode, setJumpFlag);
        Hide_BusyForm(false);
        return;
    }
#endif  // #ifdef USE_LOGCHARGE

// ARMlet���g�p���ă��b�Z�[�W�̉�͂��s���B(�ł��A�Ȃ񂩖��ʂȏ�����...)
#ifdef USE_ARMLET
    // ARMlet�̊֐��|�C���^���擾����
    if (((NNshGlobal->NNsiParam)->useARMlet != 0)&&(kanjiCode == NNSH_KANJICODE_SHIFTJIS))
    {
        armH = DmGetResource('armc', RSCID_parseMessage_ARM);
        if (armH != 0)
        {
            armP = MemHandleLock(armH);
            if (armP != NULL)
            {
                // ARMlet���g�p���ẴC���f�b�N�X��͏���
#ifdef USE_COLOR
                if (((NNshGlobal->NNsiParam)->useColor != 0)&&
                    ((NNshGlobal->NNsiParam)->useColorMessageToken != 0)&&
                    ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0))
                {
                    // 
                    hanzen = ((0x10000000)|((NNshGlobal->NNsiParam)->convertHanZen));
                }
                else
#endif  //#ifdef USE_COLOR
                {
                    hanzen = (NNshGlobal->NNsiParam)->convertHanZen;
                }
                MemSet(armArgs, sizeof(armArgs), 0x00);
                armArgs[0] = (UInt32) (buf + StrLen(buf));
                armArgs[1] = (UInt32) ptr;
                armArgs[2] = (UInt32) msgSize;
                armArgs[3] = (UInt32) &(nofJump);
                armArgs[4] = (UInt32) &(jmpMsg[0]);
                armArgs[5] = (UInt32) hanzen;
                armArgs[6] = (UInt32) &Han2ZenTable[0];
                armArgs[7] = (UInt32) setJumpFlag;
                armArgs[8] = (UInt32) (NNshGlobal->NNsiParam)->useNameOneLine;  // parseMode;
                (void) PceNativeCall(armP, armArgs);
                MemHandleUnlock(armH);

                if (setJumpFlag == true)
                {
                    // ����ȏ����A������(68k��)�ł���Ă��X�|�C�������Ȃ�...
                    for (lp = 0; ((lp < nofJump)&&(idxP->nofJump < JUMPBUF)); lp++)
                    {
                        idxP->jumpMsg[idxP->nofJump] = (UInt16) jmpMsg[lp];
                        (idxP->nofJump)++;
                    }
                }
            }
            else
            {
                // (�W����)�C���f�b�N�X��͏���
                parseMessage((buf + StrLen(buf)), ptr, msgSize,
                             &(idxP->nofJump), &(idxP->jumpMsg[0]),kanjiCode,
                             setJumpFlag);
            }
            DmReleaseResource(armH);
        }
        else
        {
            // (�W����)�C���f�b�N�X��͏���
            parseMessage((buf + StrLen(buf)), ptr, msgSize,
                         &(idxP->nofJump), &(idxP->jumpMsg[0]), kanjiCode,
                         setJumpFlag);
        }
    }
    else
#endif
    {
        // (�W����)�C���f�b�N�X��͏���
        parseMessage((buf + StrLen(buf)), ptr, msgSize,
                     &(idxP->nofJump), &(idxP->jumpMsg[0]), kanjiCode,
                     setJumpFlag);
    }

    // Go���j���[�̃A���J�[�𐮗񂷂�ꍇ...
    if ((NNshGlobal->NNsiParam)->sortGoMenuAnchor != 0)
    {
        sortJumpMsgOrder(&(idxP->nofJump), &(idxP->jumpMsg[0]));
    }

    // HTML���[�h�̏ꍇ�A�F�𖳗����NORMAL�F�ɖ߂�(����...)
    if ((htmlP != NULL)&&(*htmlP == NNSH_CHARCOLOR_ESCAPE))
    {
        *(htmlP + 1) = NNSH_CHARCOLOR_NORMAL;
    }
    return;
}


/*-------------------------------------------------------------------------*/
/*   Function : sortJumpMsgOrder                                           */
/*                                                 ���X�ԍ�����בւ�����  */
/*-------------------------------------------------------------------------*/
static void sortJumpMsgOrder(UInt16 *nofJump, UInt16 *msgList)
{
    Boolean isMatching;
    UInt16 msgNumList[JUMPBUF + MARGIN];
    UInt16 lp, lp2, lp3, msgCnt;

    // ���X�ԍ��̏d�������...
    msgCnt = 0;
    for (lp = 0; lp < *nofJump; lp++)
    {
        isMatching = false;
        for (lp2 = 0; lp2 < msgCnt; lp2++)
        {
            if (msgList[lp] == msgNumList[lp2])
            {
                isMatching = true;
                break;
            }
        }
        if (isMatching == false)
        {
            // �A�C�e����o�^����
            if ((NNshGlobal->NNsiParam)->sortGoMenuAnchor < 2)
            {
                // insertion sort����������...
                for (lp3 = 0; lp3 < msgCnt; lp3++)
                {
                    if (msgNumList[lp3] > msgList[lp])
                    {
                        MemMove(&msgNumList[lp3 + 1], &msgNumList[lp3], (sizeof(UInt16) * (msgCnt - lp3)));
                        msgNumList[lp3] = msgList[lp];
                        break;
                    }
                }
                if (lp3 >= msgCnt)
                {
                    msgNumList[msgCnt] = msgList[lp];
                }
            }
            else
            {
                // �o�����ɕ��ׂ�B�B�B
                msgNumList[msgCnt] = msgList[lp];
            }
            msgCnt++;
        }
    }
    
    // �����߂�...
    for (lp = 0; lp < msgCnt; lp++)
    {
        msgList[lp] = msgNumList[lp];
    }
    *nofJump = msgCnt;
    
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : displayMessage                                             */
/*                                                   ���b�Z�[�W��\������  */
/*-------------------------------------------------------------------------*/
static Err displayMessage(NNshMessageIndex *idxP, UInt16 isBottom)
{
    FormType    *frm;
    FieldType   *fldP;
    MemHandle    txtH, oldTxtH;
    Err          ret = errNone;
    UInt16       msg, allocSize, lp;
#ifdef USE_COLOR
    UInt16       len;
#endif
    UInt32       offset;
    Char        *buf;

    // �ϐ��̏�����
    msg    = idxP->openMsg;
    offset = 0;

    //  �w�肳�ꂽ���b�Z�[�W�ԍ����Ó����`�F�b�N����B
    if ((msg < 1)||(msg > idxP->nofMsg))
    {
        return (~errNone);
    }
    idxP->blockStartMsg = msg;

    // �o�b�t�@���烁�b�Z�[�W���擾����
    ret = NNsh_Get_MessageToBuf(idxP, msg, GET_MESSAGE_TYPE_HALF, true);
    if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        // �\���o�b�t�@���m�ۂ���
        if ((msg + (NNshGlobal->NNsiParam)->blockDispMode) > idxP->nofMsg)
        {
            allocSize = (idxP->msgOffset)[idxP->nofMsg];
        }
        else
        {
            allocSize = (idxP->msgOffset)[msg + (NNshGlobal->NNsiParam)->blockDispMode];
        }
        allocSize = allocSize - (idxP->msgOffset)[msg - 1] + TINYBUF;
        allocSize = allocSize + allocSize;

        txtH = MemHandleNew(allocSize);
        if (txtH == 0)
        {
            // �̈�̊m�ۂɎ��s�����ꍇ�A�G���[�\������
            NNsh_InformMessage(ALTID_WARN, MSG_SUBJECT_LISTFAIL,
                               " size:", allocSize);
            return (~errNone);
        }
        buf  = MemHandleLock(txtH);
        if (buf == NULL)
        {
            MemHandleFree(txtH);
            NNsh_InformMessage(ALTID_WARN, "MemHandleLock()", " ", 0);
            return (~errNone);
        }
        MemSet(buf, allocSize, 0x00);

        // �W�����v�ԍ��o�b�t�@��������
        idxP->nofJump = 0;
        idxP->dispMsg = (NNshGlobal->NNsiParam)->blockDispMode;
        lp = 1;
        do {
            // ���b�Z�[�W�̐��`����
            offset = ((idxP->msgOffset)[msg - 1] - (idxP->fileOffset));
            displayMessageSub(idxP->bbsType, idxP->buffer, msg, offset,
                              (idxP->msgOffset)[msg] - (idxP->msgOffset)[msg - 1],
                              buf, true);
            msg++;
            // RAW-disp���[�h�łȂ��A�L�[���[�h�`�F�b�N���w������Ă����Ƃ��́A
            if (((NNshGlobal->NNsiParam)->rawDisplayMode == 0)&&((NNshGlobal->NNsiParam)->hideMessage != 0))
            {
                // (�ȒP)NG�L�[���[�h�`�F�b�N����
                checkHideMessage(buf);
            }

            if (((NNshGlobal->NNsiParam)->blockDispMode == 0)||(msg > idxP->nofMsg))
            {
                // �A���\���łȂ��A�������̓X���̖����܂ŕ\������(������)
                idxP->dispMsg = lp;
                break;
            }

            // Zone�\�����̃Z�p���[�^�ݒ�
#ifdef USE_COLOR
            // �Z�p���[�^�̐F��ݒ肷��
            if (((NNshGlobal->NNsiParam)->useColor != 0)&&
                ((NNshGlobal->NNsiParam)->useColorMessageToken != 0)&&
                ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0))
            {
                len = StrLen(buf);
                buf[len]     = NNSH_CHARCOLOR_ESCAPE;
                buf[len + 1] = NNSH_CHARCOLOR_FOOTER;
                buf[len + 2] = '\0';
                StrCat(buf, (NNshGlobal->NNsiParam)->messageSeparator);

                len = StrLen(buf);
                buf[len]     = NNSH_CHARCOLOR_ESCAPE;
                buf[len + 1] = NNSH_CHARCOLOR_NORMAL;
                buf[len + 2] = '\0';
            }
            else
#endif  //#ifdef USE_COLOR
            {
                // ���f�[�^�ւ̃Z�p���[�^��\������
                StrCat(buf, (NNshGlobal->NNsiParam)->messageSeparator);
            }
            buf = buf + StrLen(buf);

            // �o�b�t�@���Ƀf�[�^���ǂݍ��܂�Ă��邩�m�F����
            if (((idxP->msgOffset)[msg - 1]  < idxP->fileOffset)||
                ((idxP->msgOffset)[msg] - 1  > idxP->fileOffset + idxP->fileReadSize))
            {
                // ���̃f�[�^�����ݓǂݍ��܂�Ă���o�b�t�@�ɑ��݂��Ȃ��ꍇ�͔�����
                idxP->dispMsg = lp;
                break;
            }            
            lp++;
        } while (lp <= (NNshGlobal->NNsiParam)->blockDispMode);

       (void) MemHandleUnlock(txtH);

        //////////// �t�B�[���h�̕\�� ////////////
        frm     = FrmGetActiveForm();
        fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        oldTxtH = FldGetTextHandle(fldP);
        FldSetTextHandle(fldP, NULL);
        FldSetTextHandle(fldP, txtH);
        if (oldTxtH != 0)
        {
            (void) MemHandleFree(oldTxtH);
        }


        // �`�惂�[�h�̐؂�ւ�
        if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
        {
            // �t�B�[���h�̕\���n�ϐ����N���A����
            NNshGlobal->totalLine   = 0;
            NNshGlobal->nofPage     = 0;
            NNshGlobal->currentPage = 0;
            NNshGlobal->prevHRLines = 0;

            // �t�B�[���h�̕`��(�n�C���]���)
            NNsi_HRFldDrawField(fldP, isBottom);

            // �X�N���[���o�[�̍X�V
            NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, SCLID_MESSAGE)));
        }
        else
        {
            // �t�B�[���h�̕`��(�ʏ���)
            FldDrawField(fldP);

            // �X�N���[���o�[�̕\�����X�V����
            NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);

            FrmDrawForm(frm);
        }
    }

    if ((NNshGlobal->NNsiParam)->blockDispMode != 0)
    {
        // �]�[���\�����A���X������\�����Ă��邩�H
        if ((idxP->openMsg + idxP->dispMsg) >= idxP->nofMsg)
        {
            FormType      *frm;
            ScrollBarType *barP;
            UInt16         val, max, min, page;

            // �]�[���\���ŁA���y�[�W���Ȃ��ꍇ�́A�����܂œǂ񂾂��Ƃɂ���
            frm  = FrmGetActiveForm();
            barP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SCLID_MESSAGE));
            SclGetScrollBar(barP, &val, &min, &max, &page);
            if ((max == min)&&(min == val)) // �� �X�N���[���o�[���\������Ȃ��p�^�[���͂��̂͂�...
            {
                // �����܂œǂ񂾂��Ƃɂ���B
                idxP->openMsg =  idxP->nofMsg;
            }
        }
    }

    // �ǂ񂾃��b�Z�[�W�ԍ�(�Ƃ��C�ɓ������)�L������
    MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                     NNSH_MSGATTR_NOTSPECIFY, 
                     (NNshGlobal->NNsiParam)->hideMessage);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveMessageNext                                            */
/*                                           �ЂƂ��̃��b�Z�[�W��\��  */
/*-------------------------------------------------------------------------*/
static Boolean moveMessageNext(Boolean chkScroll)
{

    Boolean           ret = false;
    NNshMessageIndex *idxP;

    // index��ϐ��ɓ����
    idxP = NNshGlobal->msgIndex;

    if (idxP->openMsg != idxP->blockStartMsg)
    {
        idxP->openMsg = idxP->blockStartMsg;

        // �ǂ񂾃��b�Z�[�W�ԍ�(�Ƃ��C�ɓ������)�L������
        MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                         NNSH_MSGATTR_NOTSPECIFY,
                         (NNshGlobal->NNsiParam)->hideMessage);
    }

    // ���b�Z�[�W�̃X�N���[���`�F�b�N
    if (chkScroll == true)
    {
        ret = NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0, winDown); 
    }


    if ((NNshGlobal->NNsiParam)->blockDispMode == 0)
    {
        if ((ret != true)&&(idxP->openMsg < idxP->nofMsg))
        {
             // �\�����b�Z�[�W�ԍ����X�V���A��ʕ\���B
             (idxP->openMsg)++;
             (void) displayMessage(NNshGlobal->msgIndex, 0);
             ret = true;
        }
    }
    else
    {
        // ���X�A���\�����[�h�̂Ƃ�
        if (ret != true)  // ���u���b�N��\������...
        {
            // �\�����b�Z�[�W�ԍ����X�V
            if ((idxP->openMsg + idxP->dispMsg) <= idxP->nofMsg)
            {
                idxP->openMsg =  idxP->openMsg + idxP->dispMsg;

                // ��ʕ\���B
                (void) displayMessage(NNshGlobal->msgIndex, 0);

                return (true);
            }
            if (idxP->openMsg < idxP->nofMsg)
            {
                // �����܂œǂ񂾂��Ƃɂ���B
                idxP->openMsg =  idxP->nofMsg;
 
                // �ǂ񂾃��b�Z�[�W�ԍ�(�Ƃ��C�ɓ������)�L������
                MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                                 NNSH_MSGATTR_NOTSPECIFY,
                                 (NNshGlobal->NNsiParam)->hideMessage);
                ret = true;
            }
        }
        else
        {
            // �X�N���[�������Ƃ��A�����܂œǂ�ł��邩�H
            if ((idxP->openMsg + idxP->dispMsg) > idxP->nofMsg)
            {
                FormType      *frm;
                ScrollBarType *barP;
                UInt16         val, max, min, page;

                // �]�[���\���ŁA���y�[�W���Ȃ��ꍇ�́A�����܂œǂ񂾂��Ƃɂ���
                frm  = FrmGetActiveForm();
                barP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SCLID_MESSAGE));
                SclGetScrollBar(barP, &val, &min, &max, &page);
                if (val >= max) // �� �X�N���[���o�[�̖����ɓ��B���Ă���p�^�[���͂��̂͂�...
                {
                    // (�X�N���[���o�[���\�������)�����܂œǂ񂾂��Ƃɂ���B
                    idxP->openMsg =  idxP->nofMsg;

                    // �ǂ񂾃��b�Z�[�W�ԍ�(�Ƃ��C�ɓ������)�L������
                    MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                                     NNSH_MSGATTR_NOTSPECIFY,
                                     (NNshGlobal->NNsiParam)->hideMessage);
                    ret = true;
                }
            }
        }
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveMessagePrev                                            */
/*                                             �ЂƂO�̃��b�Z�[�W��\��  */
/*-------------------------------------------------------------------------*/
static Boolean moveMessagePrev(Boolean chkScroll, UInt16 dispLoc)
{
    Int16             resNum;
    Boolean           ret = false;
    NNshMessageIndex *idxP;

    // index��ϐ��ɓ����
    idxP = NNshGlobal->msgIndex;

    if (idxP->openMsg != idxP->blockStartMsg)
    {
        idxP->openMsg = idxP->blockStartMsg;

        // �ǂ񂾃��b�Z�[�W�ԍ�(�Ƃ��C�ɓ������)�L������
        MarkMessageIndex((NNshGlobal->NNsiParam)->openMsgIndex,
                         NNSH_MSGATTR_NOTSPECIFY,
                         (NNshGlobal->NNsiParam)->hideMessage);
    }

    // ���b�Z�[�W�̃X�N���[���`�F�b�N
    if (chkScroll == true)
    {
        ret = NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0,  winUp);
    }

    // ���ƂŃ��t�@�N�^�K�v�H
    if ((NNshGlobal->NNsiParam)->blockDispMode == 0)
    {
        if ((ret != true)&&(idxP->openMsg > 1))
        {
            // �\�����b�Z�[�W�ԍ����X�V���A��ʕ\���B
            (idxP->openMsg)--;
            (void) displayMessage(NNshGlobal->msgIndex, dispLoc);
            ret = true;
        }
    }
    else
    {
        // ���X�A���\�����[�h�̂Ƃ�
        if (ret != true)
        {
            // �\�����b�Z�[�W�ԍ����X�V
            resNum = idxP->openMsg - (NNshGlobal->NNsiParam)->blockDispMode;
            if (resNum >= 1)
            {
                idxP->openMsg = idxP->openMsg - (NNshGlobal->NNsiParam)->blockDispMode;

                // ��ʕ\���B
                (void) displayMessage(NNshGlobal->msgIndex,
                                      (NNshGlobal->NNsiParam)->dispBottom);
            }
            else
            {
                idxP->openMsg = 1;

                // ��ʕ\���B
                (void) displayMessage(NNshGlobal->msgIndex, 0);
            }
            ret = true;
        }
    }
    return (ret);
}
/*-------------------------------------------------------------------------*/
/*   Function : jumpMessage                                                */
/*                                                (���b�Z�[�W�W�����v����) */
/*-------------------------------------------------------------------------*/
static Boolean jumpMessage(FormType *frm, NNshMessageIndex *idxP)
{
    UInt16     tempMsg, num, btnId, size, nofItems, savedParam;
    UInt16     jumpList[NNSH_NOF_JUMPMSG_EXT + JUMPBUF + 2];
    UInt16     msgAttribute;
    EventType *dummyEvent;
    FieldType *fldP;
    Char      *numP, buffer[BUFSIZE * 2];
    MemHandle  txtH;

    // �Ƃ肠�����A�A�A���Xpop up �p
    NNshGlobal->work3 = 0;

    // ���͂��ꂽ���l���擾����
    tempMsg = idxP->openMsg;
    fldP    = FrmGetObjectPtr(frm, 
                              FrmGetObjectIndex(frm, FLDID_MESSAGE_NUMBER));
    txtH = FldGetTextHandle(fldP);
    if (txtH != NULL)
    {
        numP = MemHandleLock(txtH);
        num = StrAToI(numP);

        // ���l���Ó��Ȃ��̂��`�F�b�N
        if ((num > 0)&&(num <= idxP->nofMsg))
        {
            tempMsg = num;
        }
        MemHandleUnlock(txtH);
    }

    // �������������t�B�[���h�ɓ��͂���Ă��Ȃ������ꍇ
    if (tempMsg == idxP->openMsg)
    {
        //  Go�{�^���������ꂽ�Ƃ��A���������͂���Ă��Ȃ������ꍇ�A
        //  ���߂����X���ԍ������X�g�I�u�W�F�N�g�Ƃ��ĕ\������B
        size = (idxP->nofJump + NNSH_NOF_JUMPMSG_EXT + 1) * NNSH_JUMPMSG_LEN;
        idxP->jumpListTitles = MEMALLOC_PTR(size);
        if (idxP != NULL)
        {
            // ���X�g�ɕ\������^�C�g�����X�g���쐬����
            nofItems = 0; 
            MemSet(jumpList, sizeof(jumpList), 0x00);
            MemSet(idxP->jumpListTitles, 0x00, size);
            numP = idxP->jumpListTitles;

            if ((NNshGlobal->NNsiParam)->addReturnToList != 0)
            {
                // ���X�g�Ɂu�ꗗ�ցv��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_TO_LIST);
                numP = numP + sizeof(NNSH_JUMPMSG_TO_LIST);
                jumpList[nofItems] = (NNSH_JUMPSEL_TO_LIST);
                nofItems++;
            }

            // �W�����v�ԍ���ݒ肷��
            for (num = 0; num < idxP->nofJump; num++)
            {
                // �W�����v�ԍ����͈͊O�̑傫�Ȓl�̂Ƃ��͕\�����Ȃ�
                if (idxP->jumpMsg[num] <= idxP->nofMsg)
                {
                    StrCopy(numP, NNSH_JUMPMSG_HEAD);
                    NUMCATI(numP, idxP->jumpMsg[num]);
                    numP = numP + StrLen(numP) + 1;
                    jumpList[nofItems] = idxP->jumpMsg[num];
                    nofItems++;
                }
            }

            if ((NNshGlobal->NNsiParam)->addMenuBackRtnMsg != 0)
            {
                // ���X�g�Ɂu�P�߂�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_BACK);
                numP = numP + sizeof(NNSH_JUMPMSG_BACK);
                jumpList[nofItems] = (NNSH_JUMPSEL_BACK);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addJumpTopMsg != 0)
            {
                // ���X�g�Ɂu�擪�ցv��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_TOP);
                numP = numP + sizeof(NNSH_JUMPMSG_TOP);
                jumpList[nofItems] = (NNSH_JUMPSEL_TOP);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addJumpBottomMsg != 0)
            {
                // ���X�g�Ɂu�����ցv��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_BOTTOM);
                numP = numP + sizeof(NNSH_JUMPMSG_BOTTOM);
                jumpList[nofItems] = (NNSH_JUMPSEL_BOTTOM);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addMenuMsg != 0)
            {
                // ���X�g�Ɂu���j���[�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_OPEN_MENU);
                numP = numP + sizeof(NNSH_JUMPMSG_OPEN_MENU);
                jumpList[nofItems] = (NNSH_JUMPSEL_OPEN_MENU);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addLineGetPart != 0)
            {
                // ���X�g�Ɂu�����擾�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_UPDATE);
                numP = numP + sizeof(NNSH_JUMPMSG_UPDATE);
                jumpList[nofItems] = (NNSH_JUMPSEL_UPDATE);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addLineDisconn != 0)
            {
                // ���X�g�Ɂu����ؒf�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_HANGUP);
                numP = numP + sizeof(NNSH_JUMPMSG_HANGUP);
                jumpList[nofItems] = (NNSH_JUMPSEL_HANGUP);
                nofItems++;
            }

            if (NNshGlobal->browserCreator != 0)
            {
                if ((NNshGlobal->NNsiParam)->addMenuSelAndWeb != 0)
                {
                    // ���X�g�Ɂu�I��+Web�v��ǉ�
                    StrCopy(numP, NNSH_JUMPMSG_OPENWEB);
                    numP = numP + sizeof(NNSH_JUMPMSG_OPENWEB);
                    jumpList[nofItems] = (NNSH_JUMPSEL_OPENWEB);
                    nofItems++;
                }
            }

            if (NNshGlobal->browserCreator != 0)
            {
#ifdef USE_OPENURL_ONLYHTML
                if ((idxP->bbsType & NNSH_BBSTYPE_MASK) == NNSH_BBSTYPE_HTML)
#endif
                {
                    // ���X�g�Ɂu�����N���J���v��ǉ�
                    StrCopy(numP, NNSH_JUMPMSG_OPENLINK);
                    numP = numP + sizeof(NNSH_JUMPMSG_OPENLINK);
                    jumpList[nofItems] = (NNSH_JUMPSEL_OPENLINK);
                    nofItems++;
                }
            }

            if ((NNshGlobal->NNsiParam)->addMenuGraphView != 0)
            {
                // ���X�g�Ɂu�`�惂�[�h�ύX�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_CHANGEVIEW);
                numP = numP + sizeof(NNSH_JUMPMSG_CHANGEVIEW);
                jumpList[nofItems] = (NNSH_JUMPSEL_CHANGEVIEW);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addMenuFavorite != 0)
            {
                // ���X�g�Ɂu���C�ɓ���ݒ�ύX�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_FAVORITE);
                numP = numP + sizeof(NNSH_JUMPMSG_FAVORITE);
                jumpList[nofItems] = (NNSH_JUMPSEL_FAVORITE);
                nofItems++;
            }
            if ((NNshGlobal->NNsiParam)->addMenuOutputMemo != 0)
            {
                // ���X�g�Ɂu�������o�́v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_OUTPUTMEMO);
                numP = numP + sizeof(NNSH_JUMPMSG_OUTPUTMEMO);
                jumpList[nofItems] = (NNSH_JUMPSEL_OUTPUTMEMO);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addDeviceInfo != 0)
            {
                // ���X�g�Ɂu�f�o�C�X���v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_DEVICEINFO);
                numP = numP + sizeof(NNSH_JUMPMSG_DEVICEINFO);
                jumpList[nofItems] = (NNSH_JUMPSEL_DEVICEINFO);
                nofItems++;
            }

            // A2DA���C���X�g�[������Ă��邩�m�F����
            if (CheckInstalledResource_NNsh('DAcc', 'a2DA') != false)
            {
                // ���X�g�ɁuAA�\���v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_EXECA2DA);
                numP = numP + sizeof(NNSH_JUMPMSG_EXECA2DA);
                jumpList[nofItems] = (NNSH_JUMPSEL_EXECA2DA);
                nofItems++;
            }

#ifdef USE_BT_CONTROL
            // Bluetooth On/Off
            if ((NNshGlobal->NNsiParam)->addBtOnOff != 0)
            {
                    // ���X�g�ɁuBt On/Bt Off�v��ǉ�
                    StrCopy(numP, NNSH_JUMPMSG_BTOOTH_ON);
                    numP = numP + sizeof(NNSH_JUMPMSG_BTOOTH_ON);
                    jumpList[nofItems] = (NNSH_JUMPSEL_BTOOTH_ON);
                    nofItems++;

                    StrCopy(numP, NNSH_JUMPMSG_BTOOTH_OFF);
                    numP = numP + sizeof(NNSH_JUMPMSG_BTOOTH_OFF);
                    jumpList[nofItems] = (NNSH_JUMPSEL_BTOOTH_OFF);
                    nofItems++;
            }
#endif

            if ((NNshGlobal->NNsiParam)->useBookmark != 0)
            {
                // ���X�g�Ɂu������ݒ�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_BOOKMARK);
                numP = numP + sizeof(NNSH_JUMPMSG_BOOKMARK);
                jumpList[nofItems] = (NNSH_JUMPSEL_BOOKMARK);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addMenuGetThreadNum != 0)
            {
                // ���X�g�Ɂu�X���Ԏw��擾�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_GET_THREADNUM);
                numP = numP + sizeof(NNSH_JUMPMSG_GET_THREADNUM);
                jumpList[nofItems] = (NNSH_JUMPSEL_GET_THREADNUM);
                nofItems++;
            }

            if ((CheckInstalledResource_NNsh('appl', 'asUR') != false)||
                (CheckInstalledResource_NNsh('DAcc', 'mmuA') != false))
            {
                // MemoURL/MemoURLDA���C���X�g�[������Ă���...
                // ���X�g�ɁuMemoURL�֏o�́v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_MEMOURL);
                numP = numP + sizeof(NNSH_JUMPMSG_MEMOURL);
                jumpList[nofItems] = (NNSH_JUMPSEL_MEMOURL);
                nofItems++;
            }

            if ((NNshGlobal->NNsiParam)->addNgSetting3 != 0)
            {
                // ���X�g�ɁuNG�ؑցv��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_TOGGLE_NGWORD);
                numP = numP + sizeof(NNSH_JUMPMSG_TOGGLE_NGWORD);
                jumpList[nofItems] = (NNSH_JUMPSEL_TOGGLE_NGWORD);
                nofItems++;

                // ���X�g�ɁuNG�o�^�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_ENTRY_NGWORD);
                numP = numP + sizeof(NNSH_JUMPMSG_ENTRY_NGWORD);
                jumpList[nofItems] = (NNSH_JUMPSEL_ENTRY_NGWORD);
                nofItems++;
            }

            if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
            {
                // webDA���C���X�g�[������Ă���...
                // ���X�g�ɁuwebDA(link)�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_OPENLINK_WEBDA);
                numP = numP + sizeof(NNSH_JUMPMSG_OPENLINK_WEBDA);
                jumpList[nofItems] = (NNSH_JUMPSEL_OPENLINK_WEBDA);
                nofItems++;
            }

            if ((idxP->bbsType & NNSH_BBSTYPE_MASK) == NNSH_BBSTYPE_HTML)
            {
                // HTML�`���̃t�@�C���Q�ƒ��̏ꍇ�ɂ́A�A�A

                // ���X�g�ɁuRAW���[�h�ؑցv��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_TOGGLERAWMODE);
                numP = numP + sizeof(NNSH_JUMPMSG_TOGGLERAWMODE);
                jumpList[nofItems] = (NNSH_JUMPSEL_TOGGLERAWMODE);
                nofItems++;

                // ���X�g�Ɂu��؂�ύX�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_SELECTSEPA);
                numP = numP + sizeof(NNSH_JUMPMSG_SELECTSEPA);
                jumpList[nofItems] = (NNSH_JUMPSEL_SELECTSEPA);
                nofItems++;
 
                 // ���X�g�Ɂu��؂�ҏW�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_EDITSEPA);
                numP = numP + sizeof(NNSH_JUMPMSG_EDITSEPA);
                jumpList[nofItems] = (NNSH_JUMPSEL_EDITSEPA);
                nofItems++;

                 // ���X�g�Ɂu�폜��ꗗ�ցv��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_BACK_AND_DEL);
                numP = numP + sizeof(NNSH_JUMPMSG_BACK_AND_DEL);
                jumpList[nofItems] = (NNSH_JUMPSEL_BACK_AND_DEL);
                nofItems++;
            }

            // �I���E�B���h�E��\������
            btnId = NNshWinSelectionWindow(FRMID_JUMPMSG, idxP->jumpListTitles, nofItems, 0);
            MEMFREE_PTR(idxP->jumpListTitles);
            if (btnId != BTNID_JUMPCANCEL)
            {
                // �I�����X�g�ԍ�����R�}���h���擾
                switch (jumpList[NNshGlobal->jumpSelection])
                {
                  case NNSH_JUMPSEL_BACK:
                    // �P�O�ɔ�񂾃��b�Z�[�W�ɖ߂�
                    jumpPrevious();
                    return (false);
                    break;

                  case NNSH_JUMPSEL_TO_LIST:
                    // �X���I����ʂɖ߂�
                    ReadMessage_Close(NNshGlobal->backFormId);
                    return (false);
                    break;

                  case NNSH_JUMPSEL_OPEN_MENU:
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

                    // ���b�Z�[�W(��)�ĕ\��
                    (void) displayMessage(idxP, 0);
                    return (false);
                    break;

                  case NNSH_JUMPSEL_UPDATE:
                    // �����擾
                    (void) PartReceiveMessage_View();
                    return (false);
                    break;

                  case NNSH_JUMPSEL_HANGUP:
                    // ����ؒf
                    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
                    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
                    NNshNet_LineHangup();
                    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

                    // ���b�Z�[�W�\��
                    (void) displayMessage(idxP, 0);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_OPENWEB:
                    // �S�I���{Web�u���E�U�ŊJ��
                    frm  = FrmGetActiveForm();
                    fldP = FrmGetObjectPtr(frm, 
                                       FrmGetObjectIndex(frm, FLDID_MESSAGE));
                    savedParam = FldGetTextLength(fldP);

                    // �I�𕶎���̏����ݒ肷��
                    savedParam = (savedParam > NNSH_LIMIT_COPYBUFFER) ?
                                           NNSH_LIMIT_COPYBUFFER : savedParam;
                    FldSetSelection(fldP, 0, savedParam);
                    FldCopy(fldP);
                    if (NNshGlobal->browserCreator != 0)
                    {
                        // WebBrowser�ŊJ��(���̂Ƃ��A�m�FMSG�Ȃ�)
                        openWebBrowser(NNSH_OMITDIALOG_ALLOMIT, buffer, sizeof(buffer));
                    }
                    // ���b�Z�[�W�\��
                    (void) displayMessage(idxP, 0);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_TOP:
                    // �擪��
                    tempMsg = 1;
                    break;

                  case NNSH_JUMPSEL_BOTTOM:
                    // ������
                    tempMsg = idxP->nofMsg;
                    break;

                  case NNSH_JUMPSEL_CHANGEVIEW:
                    // �`�惂�[�h�ύX
                    if ((NNshGlobal->NNsiParam)->useSonyTinyFont == 0)
                    {
                        // �n�C���]���[�h�֕ύX
                        (NNshGlobal->NNsiParam)->useSonyTinyFont = 1;
                    }
                    else
                    {
                        // �ʏ탂�[�h�֕ύX
                        (NNshGlobal->NNsiParam)->useSonyTinyFont = 0;
                    }

                    // ��ʂ���x���Ă�����x�J��
                    ReadMessage_Close(FRMID_MESSAGE);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_FAVORITE:
                    // �X������\������
                    DisplayMessageInformations((NNshGlobal->NNsiParam)->openMsgIndex, 
                                               &msgAttribute);

                    // ���C�ɓ����񃉃x�����X�V
                    setFavoriteLabel(FrmGetActiveForm(), msgAttribute);

                    // �\�����X�V����(OS3.1�f�o�C�X�ł͐^�����ɂȂ邽��)
                    FrmDrawForm(FrmGetActiveForm());
                    (void) displayMessage(idxP, 0);
                    break;

                  case NNSH_JUMPSEL_DEVICEINFO:
                    // �f�o�C�X����\������
                    ShowDeviceInfo_NNsh();

                    // �\�����X�V����(OS3.1�f�o�C�X�ł͐^�����ɂȂ邽��)
                    FrmDrawForm(FrmGetActiveForm());
                    (void) displayMessage(idxP, 0);
                    break;

                  case NNSH_JUMPSEL_OUTPUTMEMO:
                    // �������֏o�͂���
                    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
                    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
                    (void) outputMemoPad(false);
                    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;

                    // ���b�Z�[�W�\��
                    (void) displayMessage(idxP, 0);
                    break;

                  case NNSH_JUMPSEL_BOOKMARK:
                    // ������ݒ�
                    if ((NNshGlobal->NNsiParam)->useBookmark != 0)
                    {
                        (NNshGlobal->NNsiParam)->lastFrmID     = NNSH_FRMID_MESSAGE;
                        (NNshGlobal->NNsiParam)->bookMsgIndex  = (NNshGlobal->NNsiParam)->openMsgIndex;
                        (NNshGlobal->NNsiParam)->bookMsgNumber = idxP->openMsg;
                        NNsh_InformMessage(ALTID_INFO, MSG_SET_BOOKMARK, "",0);
                    }

                    // ���b�Z�[�W�\��
                    (void) displayMessage(idxP, 0);
                    break;
                    
                  case NNSH_JUMPSEL_OPENLINK:
                    // �����N���J��
                    executeViewAction(MULTIVIEWBTN_FEATURE_OPENURL);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_EXECA2DA:
                    // a2DA�N��(AA�Q��)
                    executeViewAction(MULTIVIEWBTN_FEATURE_EXECA2DA);

                    // ���b�Z�[�W�\��
                    (void) displayMessage(idxP, 0);

                    return (false);
                    break;

#ifdef USE_BT_CONTROL
                  case NNSH_JUMPSEL_BTOOTH_ON:
                    // Bluetooth On
                    executeViewAction(MULTIVIEWBTN_FEATURE_BT_ON);
                    break;

                  case NNSH_JUMPSEL_BTOOTH_OFF:
                    // Bluetooth Off
                    executeViewAction(MULTIVIEWBTN_FEATURE_BT_OFF);
                    break;
#endif
                  case NNSH_JUMPSEL_MEMOURL:
                    // MemoURL�֏o��
                    if (CheckInstalledResource_NNsh('DAcc', 'mmuA') == false)
                    {
                         // MemoURLDA���Ȃ��̂ŁAMemoURL�֓n��
                        executeViewAction(MULTIVIEWBTN_FEATURE_MEMOURL);
                    }
                    else
                    {
                         // MemoURL��MemoURLDA�o�R�œn��
                        executeViewAction(MULTIVIEWBTN_FEATURE_MEMOURLDA);
                    }
                    break;

                  case NNSH_JUMPSEL_TOGGLERAWMODE:
                    // RAW���[�h/�ʏ탂�[�h�ؑ�
                    executeViewAction(MULTIVIEWBTN_FEATURE_CHANGERAW);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_SELECTSEPA:
                    // ��؂胂�[�h�ؑ�
                    executeViewAction(MULTIVIEWBTN_FEATURE_CHGSEPA);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_EDITSEPA:
                    // ��؂蕶���̕ҏW
                    executeViewAction(MULTIVIEWBTN_FEATURE_EDITSEPA);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_OPENLINK_WEBDA:
                    // webDA�Ń����N���J��
                    savedParam = (NNshGlobal->NNsiParam)->confirmationDisable;
                    (NNshGlobal->NNsiParam)->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;
                    executeViewAction(MULTIVIEWBTN_FEATURE_OPENWEBDA);
                    (NNshGlobal->NNsiParam)->confirmationDisable = savedParam;
                    return (true);
                    break;

                  case NNSH_JUMPSEL_TOGGLE_NGWORD:
                    // NG-3�؂�ւ�
                    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
                    {
                        // NG3 ON �� OFF
                        (NNshGlobal->NNsiParam)->hideMessage = ((NNshGlobal->NNsiParam)->hideMessage & (NNSH_RESETMASK_NGWORD3));
                        NNsh_InformMessage(ALTID_INFO, MSG_RESET_NGWORD3, "",0);
                    }
                    else
                    {
                        // NG3 OFF �� ON
                        (NNshGlobal->NNsiParam)->hideMessage = ((NNshGlobal->NNsiParam)->hideMessage | (NNSH_USE_NGWORD3));
                        NNsh_InformMessage(ALTID_INFO, MSG_SET_NGWORD3, "",0);
                    }
                    (void) displayMessage(idxP, 0);
                    break;

                  case NNSH_JUMPSEL_BACK_AND_DEL:
                    // �폜��ꗗ�֖߂�...
                    // (��ʂ���Ĉꗗ��ʂɖ߂鏀��...)
                    ReadMessage_Close(NNshGlobal->backFormId);

                    // �X���폜������...
                    DeleteThreadMessage((NNshGlobal->NNsiParam)->openMsgIndex);
                    return (false);
                    break;

                  case NNSH_JUMPSEL_GET_THREADNUM:
                    // �X���ԍ��w��擾�����{����...
                    executeViewAction(MULTIVIEWBTN_FEATURE_GETMESNUM);
                    return (true);
                    break;

                  case NNSH_JUMPSEL_ENTRY_NGWORD:
                    // NG�o�^
                    executeViewAction(MULTIVIEWBTN_FEATURE_PICKUP_NG);
                    break;

                  default:
                    // �w��ԍ��̃��b�Z�[�W�փW�����v or ���X pop up
                    if (((NNshGlobal->NNsiParam)->showResPopup != 0)&&
                        (CheckInstalledResource_NNsh('DAcc', 'nnDA') != false))
                    {
                        // nnDA���C���X�g�[�����Ă����ꍇ...���X�|�b�v�A�b�v
                        NNshGlobal->work3 = jumpList[NNshGlobal->jumpSelection];
                        return (true);
                    }
                    tempMsg = jumpList[NNshGlobal->jumpSelection];
                    break;
                }
            }
        }
        else
        {
            NNsh_DebugMessage(ALTID_ERROR, "Jump LIST Create Failure.", "", 0);
        }
    }

    // �����t�B�[���h���N���A����
    NNshWinSetFieldText(frm, FLDID_MESSAGE_NUMBER, true, "", 0); 

    // ���b�Z�[�W���ړ����ăW�����v�A���b�Z�[�W��\������
    if (tempMsg != idxP->openMsg)
    {
        idxP->histIdx = 
                  (idxP->histIdx < (PREVBUF - 1)) ? (idxP->histIdx + 1) : 0;
        idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
        idxP->openMsg = tempMsg;

        // ���b�Z�[�W�\��
        (void) displayMessage(idxP, 0);

        return (true);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectScrollEvent                                          */
/*                                       �X�N���[���o�[�X�V�C�x���g�̏���  */
/*-------------------------------------------------------------------------*/
static Boolean sclRepEvt_DispMessage(EventType *event)
{
    Int16   lines;

    if (event->data.sclExit.newValue == event->data.sclExit.value)
    {
        // �������Ȃ�
        return (true);
    }

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
        // �X�N���[���s�����w�肵�ăX�N���[��(�߂����)
        NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, -lines, winUp);
    }
    else
    {
        // �X�N���[���s�����w�肵�ăX�N���[��(�i�ݕ���)
        NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, lines, winDown);
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectKeyDownEvent                                         */
/*                                                 �L�[�������ꂽ���̏���  */
/*                                (�W���O�A�V�X�g�n�e�e���̃W���O�����ǉ�) */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_KeyDown_DispMessage(EventType *event)
{
    // �C���f�b�N�X���Ȃ��ꍇ�ɂ́A������return����
    if (NNshGlobal->msgIndex == NULL)
    {
        return (false);
    }

    // �L�[�R�[�h�ϊ�(281����A���ӁI)
    switch (KeyConvertFiveWayToJogChara(event))
    {
      // �u���v�����������̏���
      case vchrPageDown:
        (void) executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.down)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // �W���O�_�C�����������������̏���
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case chrDownArrow:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // ������
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHNEXT);
        }
        else
        {
            // ���y�[�W
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PAGEDOWN);
        }
        break;

      case chrCapital_N:  // N�L�[�̓���
      case chrSmall_N:
        // �ЂƂ��̃��b�Z�[�W�Ɉړ����� 
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_NEXT);
        break;

      // �u��v�����������̏���
      case vchrPageUp:
        (void) executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.up)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // �W���O�_�C����������������̏���
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case chrUpArrow:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // �O����
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHPREV);
        }
        else
        {
            // �O�{�^�� 
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PAGEUP);
        }
        break;

      case chrCapital_P:  // P�L�[�̓���
      case chrSmall_P:
        // �ЂƂO�̃��b�Z�[�W�Ɉړ�����
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_PREVIOUS); 
        break;

      // �W���O�_�C����������(��)�̏���
      case vchrJogPushedUp:
        // �ꎞ��Ԃ�(��]����)�ݒ肷��
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGUP;
#endif
        // �����񂵂łP�ÂÏグ��H
        if ((NNshGlobal->NNsiParam)->notCursor != 0)
        {
             // ���̃��b�Z�[�W��\�����ďI��
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PREVIOUS);
        }
        else
        {
            // 10�O�̃��X�\��
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PAGEUP10);
        }
        break;

      case chrCapital_R:  // R�L�[�̓���
      case chrSmall_R:
        break;

      case chrCapital_F:  // F�L�[�̓���
      case chrSmall_F:
        // �����\��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCH);
        break;

      // �J�[�\���L�[(��)�������̏���
      case chrLeftArrow:
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.left)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // �J�[�\���L�[(�E)�������̏���
      case chrRightArrow:
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.right)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // �W���O�_�C����������(��)�̏���
      case vchrJogPushedDown:
        // �ꎞ��Ԃ�(��]����)�ݒ肷��
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGDOWN;
#endif
        // �����񂵂łP�ÂÉ�����H
        if ((NNshGlobal->NNsiParam)->notCursor != 0)
        {
            // ���̃��b�Z�[�W��\�����ďI��
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_NEXT);
        }
        else
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PAGEDOWN10);
        }
        break;

#ifdef USE_CLIE
      // JOG Push���̏���(��萳�m�ɂ̓{�^���𗣂����Ƃ�������)
      case vchrJogRelease:
        if (NNshGlobal->tempStatus == NNSH_TEMPTYPE_CLEARSTATE)
        {
            // JOG PUSH�{�^���Ɠ����ȓ���
            executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.jogPush)&(MULTIVIEWBTN_FEATURE_MASK));
        }
        else
        {
            // �ꎞ��Ԃ��N���A����
            NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
            return (true);
        }
        break;
#endif
      //  HandEra Jog-Push/Enter�L�[���������Ƃ��̏���
      case chrCarriageReturn:
      case chrLineFeed:
      case vchrRockerCenter:
      case vchrHardRockerCenter:
      case vchrThumbWheelPush:
        // JOG Push�{�^���̓���
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.jogPush)&(MULTIVIEWBTN_FEATURE_MASK));
        return (true);
        break;

      case vchrHard1:
      case vchrTapWaveSpecificKeyULeft:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // �n�[�h�L�[�P���������Ƃ��̐ݒ�
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.key1)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      case vchrHard2:
      case vchrTapWaveSpecificKeyURight:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // �n�[�h�L�[�Q���������Ƃ��̐ݒ�
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.key2)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      case vchrHard3:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // �n�[�h�L�[�R���������Ƃ��̐ݒ�
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.key3)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      case vchrHard4:
        if ((NNshGlobal->NNsiParam)->useHardKeyControl == 0)
        {
            // �n�[�h�L�[���䂵�Ȃ��ꍇ�͏I��
            return (false);
        }
        // �n�[�h�L�[�S���������Ƃ��̐ݒ�
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.key4)&(MULTIVIEWBTN_FEATURE_MASK));
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
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.clieCapture)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // Sony �o�b�N�{�^��/HandEra �o�b�N�{�^��/ESC�L�[/X�L�[�̏���
      // case chrBackspace:
      case vchrJogBack:
      case chrEscape:
      case vchrThumbWheelBack:
      case vchrGarminEscape:
      case chrCapital_X:  // X�L�[�̓���
      case chrSmall_X:
        // JOG Back�{�^���̓���
        executeViewAction(((NNshGlobal->NNsiParam)->viewFtr.jogBack)&(MULTIVIEWBTN_FEATURE_MASK));
        break;

      // ��O�̃W�����v�����X�ɖ߂�(Back�{�^���Ɠ���)
      case chrCapital_Z:  // Z�L�[�̓���
      case chrSmall_Z:
        executeViewAction(MULTIVIEWBTN_FEATURE_RETURN);
        break;

      // L�L�[����
      case chrCapital_L:
      case chrSmall_L:
        // �t�H���g�ύX
        executeViewAction(MULTIVIEWBTN_FEATURE_CHANGEFONT);
        break;

      case chrCapital_V:  // V�L�[�̓���
      case chrSmall_V:
        // a2DA�N��
        executeViewAction(MULTIVIEWBTN_FEATURE_EXECA2DA);
        break;

      case chrCapital_H:  // H�L�[�̓���
      case chrSmall_H:
        // �����N���J��
        executeViewAction(MULTIVIEWBTN_FEATURE_OPENURL);
        break;

      case chrCapital_U:  // U�L�[�̓���
      case chrSmall_U:
        // �I���{Web
        executeViewAction(MULTIVIEWBTN_FEATURE_SELANDWEB);
        break;

      case chrCapital_O:  // O�L�[�̓���
      case chrSmall_O:
        // �������ɏo��
        executeViewAction(MULTIVIEWBTN_FEATURE_OUTPUTMEMO);
        break;

      case chrCapital_J:  // J�L�[�̓���
      case chrSmall_J:
        // �X���Ԏw��̃��b�Z�[�W�擾
        executeViewAction(MULTIVIEWBTN_FEATURE_GETMESNUM);
        break;

      // D�L�[����
      case chrCapital_D:
      case chrSmall_D:
        executeViewAction(MULTIVIEWBTN_FEATURE_DRAWMODE);
        break;

      // Q�L�[����
      case chrCapital_Q:
      case chrSmall_Q:
        // ����ؒf
        executeViewAction(MULTIVIEWBTN_FEATURE_DISCONNECT);
        break;

      // C�L�[����
      case chrCapital_C:
      case chrSmall_C:
        // �f�o�C�X�̏��\��
        executeViewAction(MULTIVIEWBTN_FEATURE_DEVICEINFO);
        break;

      // B�L�[����
      case chrCapital_B:
      case chrSmall_B:
        // �X���̖�����
        executeViewAction(MULTIVIEWBTN_FEATURE_GOTOBOTTOM);
        break;

        // T�L�[����
      case chrCapital_T:
      case chrSmall_T:
        // �X���̐擪��
        executeViewAction(MULTIVIEWBTN_FEATURE_GOTOTOP);
        break;

        // �X�y�[�X�L�[�EG�L�[�̓���(���b�Z�[�WJUMP)
      case chrSpace:
      case chrSJISIdeographicSpace:
      case chrCapital_G:
      case chrSmall_G:
        executeViewAction(MULTIVIEWBTN_FEATURE_JUMP);
        break;

      case chrCapital_W:  // W�L�[�̓���
      case chrSmall_W:
        // �X����������(OFFLINE�X���łȂ����̂ݏ������݉�)
        if ((NNshGlobal->NNsiParam)->writingReplyMsg == 0)
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_WRITE);
        }
        else
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_REPLYMSG);
        }
        break;

      // ���l���͂���\�������邽�߁A�A�A
      case chrCapital_Y:
      case chrSmall_Y:
        // �L�[Y (Bt ON)
        executeViewAction(MULTIVIEWBTN_FEATURE_BT_ON);
        break;

      case chrCapital_I:
      case chrSmall_I:
        // �L�[I (Bt Off)
        executeViewAction(MULTIVIEWBTN_FEATURE_BT_OFF);
        break;

      case chrCapital_A:
      case chrSmall_A:
        // �L�[A (�l�b�g���[�N�ݒ�)
        executeViewAction(MULTIVIEWBTN_FEATURE_SETNETWORK);
        break;

      case chrCapital_M:
      case chrSmall_M:
        // �L�[M (�ꗗ�֖߂�)
        executeViewAction(MULTIVIEWBTN_FEATURE_BACKTOLIST);
        break;

      case chrComma:
        // �R���}�L�[(���x���̃Z���N�^�g���K�����������Ƃɂ���)
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        SELTRID_FAVOR_THREAD)));
        break;

      case chrFullStop:
        // �s���I�h�L�[�i�^�C�g���̃Z���N�^�g���K�����������Ƃɂ���)
        CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        SELTRID_THREAD_TITLE)));
        break;

      case chrHyphenMinus:
        // �|�L�[�i�����擾�j
        executeViewAction(MULTIVIEWBTN_FEATURE_GETPART);
        break;
      case chrCommercialAt:
        // @�L�[(�w���v�\��)
        executeViewAction(MULTIVIEWBTN_FEATURE_USAGE);
        break;

      case chrSemicolon:
        // �Z�~�R�����L�[(webDA�N��)
        executeViewAction(MULTIVIEWBTN_FEATURE_OPENWEBDA);
        break;

      case chrColon:
        // �R�����L�[(Image�擾)
        executeViewAction(MULTIVIEWBTN_FEATURE_GETIMAGE);
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
                // ���T�C�Y��̕`��
                UpdateFieldRegion();
                NNsi_FrmDrawForm(FrmGetActiveForm(), true);
            }
        }
        break;
#endif

      case chrCapital_S:
      case chrSmall_S:
      case chrCapital_E:
      case chrSmall_E:
      default:
        // �L�[�R�[�h��\������
        // ���F�����L�[�ł̐���͂��Ă͂����Ȃ��i���X�ԍ��œ��͂��邽�߁j
        NNsh_DebugMessage(ALTID_INFO, "Pushed a key  code:", "",
                          event->data.keyDown.chr);
        return (false);
        break;
    }
    return (true);
}

/*=========================================================================*/
/*   Function : OpenForm_NNshMessage                                       */
/*                              (�X���b�h�V�K�ǂݏo���F���b�Z�[�W�\���ȊO) */
/*=========================================================================*/
Err OpenForm_NNshMessage(FormType *frm)
{
    Err                  ret;
    UInt16               bufferSize, fileMode;
    Char                 title   [NNSH_MSGTITLE_BUFSIZE];
    Char                 fileName[MAXLENGTH_FILENAME], *area;
    NNshSubjectDatabase  subjDB;
    ControlType         *selP;
    FieldAttrType        attr;
    FieldType           *fldP;
    NNshMessageIndex    *idxP;
#ifdef USE_TRUNCATE_TITLE
    UInt32               offset;
    UInt16               len;
    Coord                width;
    RectangleType        r;
#endif
#ifdef USE_COLOR
    IndexedColorType savedIndex;
    RGBColorType     drawColor;

    // �Q�Ɖ�ʂ̃I�[�v��
    NNsh_DebugMessage(ALTID_INFO, "OpenForm_NNshMessage()", "", 0);

    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
#if USE_DEBUG_ROM
        FrmDrawForm(FrmGetActiveForm());
#endif

        // ���݂̐F(�O�i�F�ƕ����F)��ۑ�����
        savedIndex = UIColorGetTableEntryIndex(UIObjectForeground);
        WinIndexToRGB(savedIndex, &(NNshGlobal->fgColor));
        savedIndex = UIColorGetTableEntryIndex(UIFieldText);
        WinIndexToRGB(savedIndex, &(NNshGlobal->txColor));

        // ���݂̐F(�w�i�F�F�t�B�[���h�A�t�H�[���A�_�C�A���O��)��ۑ�����
        savedIndex = UIColorGetTableEntryIndex(UIFieldBackground);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgColor));
        savedIndex = UIColorGetTableEntryIndex(UIFormFill);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgFormColor));

        savedIndex = UIColorGetTableEntryIndex(UIDialogFill);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgDialogColor));

        savedIndex = UIColorGetTableEntryIndex(UIAlertFill);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgAlertColor));

        savedIndex = UIColorGetTableEntryIndex(UIObjectFill);
        WinIndexToRGB(savedIndex, &(NNshGlobal->bgObjectColor));

        // �����̐F��ϊ�����
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorViewFG,    &drawColor);
        UIColorSetTableEntry(UIObjectForeground, &drawColor);
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorViewFG,    &drawColor);
        UIColorSetTableEntry(UIFieldText,        &drawColor);

        // �w�i�̐F���w�肳�ꂽ�F�ɐݒ肷��
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorViewBG,    &drawColor);
        UIColorSetTableEntry(UIFieldBackground,  &drawColor);
        UIColorSetTableEntry(UIFormFill,         &drawColor);
        UIColorSetTableEntry(UIDialogFill,       &drawColor);
        UIColorSetTableEntry(UIAlertFill,        &drawColor);
        UIColorSetTableEntry(UIObjectFill,       &drawColor);

        // �O�i�F�Ɣw�i�F��ݒ肷��
        WinSetForeColor(UIColorGetTableEntryIndex(UIFieldText));
        WinSetTextColor(UIColorGetTableEntryIndex(UIFieldText));
        WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));
    }
#endif

    // �X�������܂��擾
    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // subject�f�[�^�x�[�X����f�[�^�擾�Ɏ��s
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        goto FUNC_END;
    }

    // �ꎞ�o�b�t�@�̈悪���łɊm�ۂ���Ă����������A�ēx�Ƃ�Ȃ����B
    if (NNshGlobal->msgIndex != NULL)
    {
        MEMFREE_PTR((NNshGlobal->msgIndex)->msgOffset);
        idxP = NNshGlobal->msgIndex;
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(NNshGlobal->msgIndex);
    }
    bufferSize = sizeof(NNshMessageIndex) + (NNshGlobal->NNsiParam)->bufferSize + MARGIN * 2;

    // memory leak���S�z�Ȃ̂ňꉞ�m�ۑO��free test�����Ă���
    MEMFREE_PTR(NNshGlobal->msgIndex);
    NNshGlobal->msgIndex = MEMALLOC_PTR(bufferSize);

    if (NNshGlobal->msgIndex == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR, MSG_ERR_MEMPTR_NEW_BUFF, " size:",
                           bufferSize);
        ret = ~errNone;
        goto FUNC_END;
    }
    MemSet(NNshGlobal->msgIndex, bufferSize, 0x00);

    // ���b�Z�[�W�I�t�Z�b�g�̈���m�ۂ���
    bufferSize = sizeof(UInt32) * ((NNshGlobal->NNsiParam)->msgNumLimit + MARGIN);
    (NNshGlobal->msgIndex)->msgOffset = MEMALLOC_PTR(bufferSize);
    if ((NNshGlobal->msgIndex)->msgOffset == NULL)
    {
        // ���b�Z�[�W�I�t�Z�b�g�̈�̊m�ێ��s
        MEMFREE_PTR(NNshGlobal->msgIndex);
        NNsh_InformMessage(ALTID_ERROR, MSG_ERR_MEMPTR_NEW_BUFF, " size:",
                           bufferSize);
        ret = ~errNone;
        goto FUNC_END;
    }
    MemSet((NNshGlobal->msgIndex)->msgOffset, bufferSize, 0x00);

    // BBS�^�C�v�����[�N�̈�ɃR�s�[
    idxP = NNshGlobal->msgIndex;
    idxP->bbsType = (UInt16) subjDB.bbsType;

    // �^�C�g���ƃ{�[�h�j�b�N�l�[���A�{�[�h�h�c�����[�N�̈�ɃR�s�[
    StrNCopyHanZen(idxP->threadTitle, subjDB.threadTitle, MAX_THREADNAME - 1);
    StrNCopy(idxP->boardNick,   subjDB.boardNick,   MAX_NICKNAME - 1);
    StrNCopy(idxP->threadFile,  subjDB.threadFileName, MAX_THREADFILENAME - 1);

    // �ǂݍ��ރt�@�C���̏�����
    MemSet (fileName, sizeof(fileName), 0x00);

    // OFFLINE�X���̂Ƃ��́AboardNick��t�����Ȃ��悤�ɂ���
    if (StrCompare(subjDB.boardNick, OFFLINE_THREAD_NICK) != 0)
    {
        StrCopy(fileName, subjDB.boardNick);
    }
    else
    {
        // OFFLINE�X����VFS�Ɋi�[����Ă���ꍇ�ɂ́A�f�B���N�g����t������B
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_USEOFFLINE) != 0)
        {
            // ���O�x�[�X�f�B���N�g����t������
            if ((NNshGlobal->logPrefixH == 0)||
                ((area = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
            {
                StrCopy(fileName, LOGDATAFILE_PREFIX);
            }
            else
            {
                StrCopy(fileName, area);
                MemHandleUnlock(NNshGlobal->logPrefixH);
            }

            // ����ɃT�u�f�B���N�g�����������ĕt������
            GetSubDirectoryName(subjDB.dirIndex, &fileName[StrLen(fileName)]);
        }
    }
    StrCat (fileName, subjDB.threadFileName);

    // �t�@�C���̂��肩�ŁA�J�����[�h��؂�ւ���
    if (subjDB.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        fileMode = ((NNSH_FILEMODE_TEMPFILE)|(NNSH_FILEMODE_READONLY));
    }
    else
    {
        fileMode = (NNSH_FILEMODE_READONLY);
    }
    ret = OpenFile_NNsh(fileName,fileMode,&(idxP->fileRef));
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "Message File Open :", fileName, ret);

        // ���t�@�����X�̃R�s�[��Free�͑ʖ�!  
        // �������̗̈�J���́A���Ԃ񂱂ꂪ�����̂͂��A�A�A
        MEMFREE_PTR(NNshGlobal->msgIndex);
        goto FUNC_END;
    }
    GetFileSize_NNsh(&(idxP->fileRef), &(idxP->fileSize));

    //  ���b�Z�[�W�X���b�h�̃C���f�b�N�X���쐬����
    ret = CreateMessageThreadIndex(idxP, &(idxP->nofMsg), idxP->msgOffset,
                                   (UInt16) subjDB.bbsType);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "ERR>index creation failure", "", ret);
        goto FUNC_END;
    }

    // �A���_�[���C���̕\���ݒ�
    MemSet(&attr, sizeof(FieldAttrType), 0x00);
    fldP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
    FldGetAttributes(fldP, &attr);
    if ((NNshGlobal->NNsiParam)->disableUnderline != 0)
    {
        attr.underlined = noUnderline;
    }
    else
    {
        attr.underlined = grayUnderline;
    }
    FldSetAttributes(fldP, &attr);

    // �t�H�[���^�C�g���̍X�V(���b�Z�[�W�����i�[)
    StrCopy(title, "1:");
    NUMCATI(title, idxP->nofMsg);
    FrmCopyTitle(frm, title);

    // �X���^�C�g���\���̈�ɃX�����̂��R�s�[����
    StrCopy(idxP->showTitle, idxP->threadTitle);

    // HandEra�΍�A�A�A�X���^�CTruncate�̓R���p�C���X�C�b�`�w�莞�̂�
#ifdef USE_TRUNCATE_TITLE
#ifdef USE_GLUE
    len   = StrLen(idxP->threadTitle);
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, SELTRID_THREAD_TITLE), &r);
    width = r.extent.x - 4;
    if (FntCharsWidth(idxP->threadTitle, len) >= width)
    {
        // �X���^�C�g�����\�����ɕ\���ł��Ȃ��ꍇ�A�����ɏȗ��L��������
        width  = width - FntCharWidth(chrEllipsis);
        offset = FntWidthToOffset(idxP->showTitle, len, width, NULL, NULL);
        offset = TxtGlueGetTruncationOffset(idxP->showTitle, offset);
        idxP->showTitle[offset] = chrEllipsis;
        idxP->showTitle[offset + 1] = '\0';
    }
#endif
#endif
    // �X���^�C�g���̕\��
    selP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SELTRID_THREAD_TITLE));
    if (selP != NULL)
    {
        CtlSetLabel(selP, idxP->showTitle);
    }

    // �ǂݏo���ӏ��̊m�F(�ُ�Ȃ�擪����)
    if ((subjDB.currentLoc < 1)||(subjDB.currentLoc > idxP->nofMsg))
    {
        idxP->openMsg = 1;
    }
    else
    {
        idxP->openMsg = subjDB.currentLoc;
    }
    idxP->prevMsg[0] = idxP->openMsg;
    idxP->histIdx    = 0;

    // �Ƃ肠�����t�@�C���̐擪����f�[�^��ǂݍ���ł���
    switch (ret)
    {
        case errNone:
        case vfsErrFileEOF:
        case fileErrEOF:
          // read OK, create Index!
          ret = errNone;
          break;

        default:
          goto FUNC_END;
          break;
    }

    // �����肪�ݒ肳��Ă���ꍇ�́A���̒l��߂��B
    if (((NNshGlobal->NNsiParam)->useBookmark != 0)&&((NNshGlobal->NNsiParam)->bookMsgNumber != 0))
    {
        if ((NNshGlobal->NNsiParam)->bookMsgIndex == (NNshGlobal->NNsiParam)->openMsgIndex)
        {
            (NNshGlobal->msgIndex)->openMsg = (NNshGlobal->NNsiParam)->bookMsgNumber;
        }
    }

    // �u���C�ɓ���v�ݒ�����Z���N�^�g���K�̃��x���ɐݒ肷��
    setFavoriteLabel(frm, subjDB.msgAttribute);

    // �u�X���Q�Ɖ�ʂŃt�H�[�J�X�ړ����Ȃ��v�Ƀ`�F�b�N���Ȃ������ꍇ
    if ((NNshGlobal->NNsiParam)->notFocusNumField == 0)
    {
        // �t�H�[�J�X�����b�Z�[�W�ԍ��t�B�[���h�ɐݒ�
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE_NUMBER));
    }

    // �������[�h�`�F�b�N�{�b�N�X�̐ݒ�
    CtlSetValue(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_SEARCH)), 
                (NNshGlobal->NNsiParam)->searchMode);

    // NG���[�h�ݒ���X�����ɐݒ肷��ݒ�
    if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD_EACHTHREAD) == NNSH_USE_NGWORD_EACHTHREAD)
    {
#if 0
        if (((NNshGlobal->NNsiParam)->hideMessage & NNSH_USE_NGWORD3) == NNSH_USE_NGWORD3)
        {
            // NG�ݒ�-3�̐ݒ��ON�ɂ���B�B�B
            subjDB.msgAttribute  = (subjDB.msgAttribute | NNSH_MSGATTR_NGWORDSET);
        }
        else
        {
            // NG�ݒ�-3�̐ݒ���N���A����B�B�B
            subjDB.msgAttribute  = (subjDB.msgAttribute & NNSH_MSGATTR_CLRNGWORD);
        }
#endif

        // �X���Ɋi�[���ꂽNG�ݒ��ǂݏo��...
        if (((subjDB.msgAttribute)&(NNSH_MSGATTR_NGWORDSET)) == 0)
        {
            // NG3 ON �� OFF
            (NNshGlobal->NNsiParam)->hideMessage = ((NNshGlobal->NNsiParam)->hideMessage & (NNSH_RESETMASK_NGWORD3));
        }
        else
        {
            // NG3 OFF �� ON
            (NNshGlobal->NNsiParam)->hideMessage = ((NNshGlobal->NNsiParam)->hideMessage | (NNSH_USE_NGWORD3));
        }
    }
    
    // ��ʕ\��
    (void) displayMessage(NNshGlobal->msgIndex, 0);
    (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_MESSAGE;
    return (errNone);

FUNC_END:
    // �G���[�I������
    ReadMessage_Close(NNshGlobal->backFormId);
    return (ret);
}

/*=========================================================================*/
/*   Function : GetSubDirectoryName                                        */
/*                                                  �T�u�f�B���N�g���̎擾 */
/*=========================================================================*/
Err GetSubDirectoryName(UInt16 index, Char *dirName)
{
    DmOpenRef              dirRef;
    UInt16                 size, nextIndex;
    Int16                  depth, saveDepth;
    Err                    ret;
    Char                  *buffer;
    NNshDirectoryDatabase  dirDb;

    if (index == 0)
    {
        // �w��INDEX���ُ�
        NNsh_DebugMessage(ALTID_INFO,
                          "There is not any sub-directory.", "", index);
        return (~errNone - 2);
    }

    OpenDatabase_NNsh(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, &dirRef);

    // �T�u�f�B���N�g���g�b�v���擾����
    MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
    ret = SearchRecord_NNsh(dirRef, &index, NNSH_KEYTYPE_UINT16, 0,
                            sizeof(NNshDirectoryDatabase), &dirDb, &saveDepth);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_INFO, "SearchRecord_NNsh() ret:", "",   ret);
        NNsh_DebugMessage(ALTID_INFO, " index:", "", index);

        GetDBCount_NNsh(dirRef, &size);
        for (nextIndex = 0; nextIndex < size; nextIndex++)
        {
            MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
            ret = GetRecord_NNsh(dirRef, nextIndex,
                                 sizeof(NNshDirectoryDatabase), &dirDb);
            NNsh_DebugMessage(ALTID_INFO, "GetRecord_NNsh() ret:", "", ret);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " INDEX:", nextIndex);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " dirIdx:", dirDb.dirIndex);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " prevdx:", dirDb.previousIndex);
        }

        // DB���R�[�h�m�ۃG���[
        CloseDatabase_NNsh(dirRef);
        return (ret);
    }
    NNsh_DebugMessage(ALTID_INFO, "depth:", dirDb.dirName, dirDb.depth);

    // �ꎞ�I�ɗ̈���m�ۂ���
    size = (dirDb.depth + 1) * sizeof(Char) * MAX_DIRNAME + MARGIN;
    buffer = MEMALLOC_PTR(size);
    if (buffer == NULL)
    {
        // �̈�m�ۃG���[
        CloseDatabase_NNsh(dirRef);
        return (~errNone);
    }
    MemSet(buffer, size, 0x00);

    // �T�u�f�B���N�g����dirDB���猟��
    saveDepth = dirDb.depth - 1;
    for (depth = saveDepth; depth >= 0; depth--)
    {
        NNsh_DebugMessage(ALTID_INFO, "copy ", dirDb.dirName, depth);

        // �f�B���N�g�������R�s�[����
        StrNCopy(&buffer[depth * sizeof(Char) * MAX_DIRNAME],
                 dirDb.dirName, (MAX_DIRNAME - 1));
        if (dirDb.previousIndex == 0)
        {
            // �����I��
            break;
        }

        // ���f�B���N�g��������
        nextIndex = dirDb.previousIndex;
        MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
        ret = SearchRecord_NNsh(dirRef, &(nextIndex), NNSH_KEYTYPE_UINT16, 0,
                                sizeof(NNshDirectoryDatabase), &dirDb, &index);
        if (ret != errNone)
        {
            // �������s�A���ɂ�����
            NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh()", "", ret);
            break;
        }
    }

    // �f�B���N�g�������R�s�[
    for (depth = 0; depth <= saveDepth; depth++)
    {
        StrCat(dirName, &buffer[depth * sizeof(Char) * MAX_DIRNAME]);
    }
    NNsh_DebugMessage(ALTID_INFO, "SubDirectory:", dirName, depth);

    // ���Ƃ��܂�
    MEMFREE_PTR(buffer);
    CloseDatabase_NNsh(dirRef);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : PartReceiveMessage_View                                    */
/*                                                            �X�������擾 */
/*-------------------------------------------------------------------------*/
static Err PartReceiveMessage_View(void)
{
    Err                  ret;
    Char                 url[BUFSIZE];
    UInt16               index;
    NNshSubjectDatabase  subjDB;
    NNshBoardDatabase    bbsData;
    NNshMessageIndex    *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;

    // ���b�Z�[�W�����f�[�^�x�[�X����擾
    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()", "", ret);
        return (ret);
    }

    // OFFLINE�X�����I�����ꂽ�ꍇ�A�X�V�s��\��
    if (StrCompare(subjDB.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           idxP->threadTitle, 0);
        return (~errNone);
    }
    
    ret = Get_BBS_Database(subjDB.boardNick, &bbsData, &index);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          subjDB.boardNick, ret);
        return (ret);
    }

    switch (subjDB.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
        // �܂�BBS�ɑ΂��č����擾��I���������A���݃T�|�[�g�Ȃ�(�Ƃ肠����?)
        NNsh_ErrorMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI, "", 0);
        return (~errNone);
        break;
#if 0
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_SHITARABA:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // �Ȃɂ����Ȃ�
        break;
    }

    //  �擾�\��@�\���L���ł��l�b�g�ɐڑ�����Ă��Ȃ��ꍇ�A
    // �擾�\�񂷂邩�m�F����
    if (((NNshGlobal->NNsiParam)->getReserveFeature != 0)&&(NNshGlobal->netRef == 0))
    {
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                NNSH_MSGINFO_RESERVE, "", 0) == 0)
        {
            // �V���\�񎞂Ɏ擾�\���ݒ肵�ďI������
            subjDB.msgAttribute
                     = subjDB.msgAttribute | NNSH_MSGATTR_GETRESERVE;

            (void) update_subject_database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
            return (errNone);
        }
    }

    // ���ݎQ�ƒ��̃t�@�C������U�N���[�Y���Ă���
    CloseFile_NNsh(&(idxP->fileRef));

    // ���b�Z�[�W�擾��̍쐬
    (void) CreateThreadURL(NNSH_DISABLE, url, sizeof(url), &bbsData, &subjDB);

    // �X�������擾���s��
    (void) Get_PartMessage(url, bbsData.boardNick, &subjDB, 
                           (NNshGlobal->NNsiParam)->openMsgIndex, NULL);

    // �\�����Ȃ���
    ReadMessage_Close(FRMID_MESSAGE);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : NNsh_Search_Message                                        */
/*                                                            �X��������   */
/*-------------------------------------------------------------------------*/
static void NNsh_Search_Message(UInt16 type, Char *str, Boolean flag)
{
    NNshMessageIndex *idxP;
    UInt16            start, end;
    Int16             step, len, msg;
    Int16             cmpLen;
    UInt32            offset;
    Char             *ptr;
    Char              backup;
    FormType         *frm;
    FieldType        *fld;
    Char             *targetString = NULL;
    UInt8             caselessExchange = 0;

    // �����񂪂Ȃ���ΏI��
    if (StrLen(str) == 0)
    {
        return;
    }

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;
 
    // start, end �� 1 origin �ōl����
    switch(type)
    {
      case NNSH_SEARCH_METHOD_FROM_TOP:  //�擪���猟��
        start = 1;
        end   = idxP->nofMsg + 1;
        step  = 1;
        break;

      case NNSH_SEARCH_METHOD_NEXT:    //������
        start = idxP->openMsg + 1;
        end   = idxP->nofMsg + 1;
        step  = 1;
        break;

      case NNSH_SEARCH_METHOD_PREV: //�O����
        start = idxP->openMsg - 1;
        end   = 0;
        step  = -1;
        break;

      case NNSH_SEARCH_METHOD_FROM_BOTTOM: //�������猟��
        start = idxP->nofMsg;
        end   = 0;
        step  = -1;
        break;

      default:
        return;
        break;
    }
    cmpLen = StrLen(str);

    // �u�������v�\�� (hideBusyForm()�܂�)
    Show_BusyForm(MSG_FINDING_WAIT);

    // �������镶�����召�����ϊ�
    targetString = NULL;
    caselessExchange = 0;
    if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
    {
        targetString = MEMALLOC_PTR(cmpLen + MARGIN);
        if (targetString != NULL)
        {
            (void) StrToLower(targetString, str);
            caselessExchange = 1;
        }
        else
        {
            // �������m�ێ��s
            NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " SIZE:", cmpLen);
        }
    }

    // �����񂪐ݒ肳��Ă��Ȃ��ꍇ...
    if (targetString == NULL)
    {
        targetString = str;
    }

    for (msg = start; msg != end; msg += step)
    {
        Char *buf = NULL;
        NNsh_Get_MessageToBuf(idxP, msg, 
                         (step > 0) ? GET_MESSAGE_TYPE_TOP : GET_MESSAGE_TYPE_BOTTOM, false);

        // �o�b�t�@���� offset�𒲂ׂ�
        offset = (idxP->msgOffset)[msg - 1] - idxP->fileOffset;
        ptr = idxP->buffer + offset;
        len = (idxP->msgOffset)[msg] - (idxP->msgOffset)[msg - 1];

        //���b�Z�[�W�̖����� NULL terminate ����Ă��Ȃ����߁A
        //���̃��b�Z�[�W�̐擪�̂P�����󂷂̂Ńo�b�N�A�b�v
        backup = *(ptr + len);
        *(ptr + len) = NULL;

        // �ϊ��p�o�b�t�@��p�ӂ���
        buf = MEMALLOC_PTR((len * 2) + MARGIN); // 2�{�m�ۂ��Ă����Ζ��Ȃ��Ǝv��
        if (buf == NULL)
        {
            // �������m�ێ��s
            NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                                   " size  ::", (len * 2) + MARGIN);
            Hide_BusyForm(true);

            //�o�b�N�A�b�v�����ɖ߂� 
            *(ptr + len) = backup;
            goto SEARCH_END;
        }
        MemSet(buf, ((len * 2) + MARGIN), 0x00);
            
        // �\���p�ɕϊ�������
        displayMessageSub(idxP->bbsType, idxP->buffer, idxP->openMsg,
                          offset, len, buf, false);

        if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
        {
            // �啶��/���������C�ɂ��Ȃ����[�h�̂Ƃ��́A�A�A
#ifdef USE_GLUE
            // �����Ώۂ��������ɕϊ�...
            TxtGlueLowerStr(buf, StrLen(buf));
#else
            // ���Ԃ�A����œ����͂��B�B�B
            (void) StrToLower(buf, buf);
#endif
        }

        // ������̏ƍ����s��
        if (StrStr(buf, targetString) != 0)
        {
            // ����(������̏ƍ�)����
            Hide_BusyForm(true);

            // �o�b�N�A�b�v�����ɖ߂� 
            *(ptr + len) = backup;

            if (idxP->openMsg != msg)
            {
                if (flag == true)
                {
                    idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                            (idxP->histIdx + 1) : 0;
                    idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
                    idxP->openMsg = (idxP->openMsg < idxP->nofMsg - 10) ?
                            (idxP->openMsg + 10) : idxP->nofMsg;
                }          
                idxP->openMsg = msg;
                (void) displayMessage(idxP, 0);
            }
            MEMFREE_PTR(buf);
            goto SEARCH_END;
        }
        MEMFREE_PTR(buf);

        //�o�b�N�A�b�v�����ɖ߂� 
        *(ptr + len) = backup;
    }
    Hide_BusyForm(true);

    NNsh_ErrorMessage(ALTID_INFO, NNSH_MSGINFO_NOTFOUND, "", 0);

    // �\�����X�V����(OS3.1�f�o�C�X�ł͐^�����ɂȂ邽��)
    frm = FrmGetActiveForm();
    FrmDrawForm(frm);
    if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
    {
        // �n�C���]�t�H���g�ݒ蒆�̎��ɂ́A�t�B�[���h�̍ĕ`����s��
        fld = FrmGetObjectPtr(frm,
                              FrmGetObjectIndex(frm,FLDID_MESSAGE));
        NNsi_HRFldDrawField(fld, 0);
    }

SEARCH_END:
    if (caselessExchange != 0)
    {
        // �啶��/�������̕ϊ������{�����Ƃ�...
        MEMFREE_PTR(targetString);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : NNsh_Get_MessageToBuf                                      */
/*                                                      ���b�Z�[�W�̓Ǎ�   */
/*-------------------------------------------------------------------------*/
static Err NNsh_Get_MessageToBuf(NNshMessageIndex *idxP, UInt16 msg, UInt16 type, Boolean flag)
{
    Err ret = errNone;
    UInt32       readSize, offset, setMsg, backMsg, size;
    Int32 i;

    offset = 0;
    // �f�[�^�͍��ǂݍ��܂�Ă���̈�ɂ��邩�`�F�b�N����B(�������킹...)
    if (((idxP->msgOffset)[msg - 1]  < idxP->fileOffset)||
        ((idxP->msgOffset)[msg] - 1  > idxP->fileOffset + idxP->fileReadSize))
    {
        // ���݁A�ǂݍ��܂�Ă���o�b�t�@�ɂ̓f�[�^���Ȃ��ꍇ�B�B�B
        if ((idxP->msgOffset)[msg] < (NNshGlobal->NNsiParam)->bufferSize)
        {
            // �t�@�C���̐擪����ǂݍ���
            offset = 0;
        }
        else if ((idxP->msgOffset)[msg - 1] >
                                     (idxP->fileSize - (NNshGlobal->NNsiParam)->bufferSize))
        {
            // �t�@�C���̖�����ǂݍ���
            offset = idxP->fileSize - (NNshGlobal->NNsiParam)->bufferSize;
        }
        else
        {
            switch (type) 
            {
              case GET_MESSAGE_TYPE_HALF:
                // �߂郁�b�Z�[�W�������肷��B
                backMsg = (NNshGlobal->NNsiParam)->bufferSize / 1200;

                // (�C���f�b�N�X�̐擪 - backMsg)����ǂݍ���
                setMsg = (msg <= backMsg) ? 1 : (msg - backMsg);
                if ((idxP->msgOffset)[setMsg] > 10)
                {
                    offset = (idxP->msgOffset)[setMsg] - 10;  // 10�̓}�[�W��(�Ӗ��Ȃ�)
                }
                else
                {
                    offset = 0;
                }
                if ((offset + (NNshGlobal->NNsiParam)->bufferSize) < (idxP->msgOffset)[msg + 1])
                {
                    // backMsg���b�Z�[�W�O���ǂݍ��ݔ͈͊O�������ꍇ�͂��̏ꏊ����
                    if ((idxP->msgOffset)[msg - 1] > 10)
                    {
                        offset = (idxP->msgOffset)[msg - 1] - 10; // 10�̓}�[�W��(�Ӗ��Ȃ�)
                    }
                    else
                    {
                        offset = 0;
                    }
                }
                break;
                
              case GET_MESSAGE_TYPE_TOP:
                offset = (idxP->msgOffset)[msg - 1];
                break;

              case GET_MESSAGE_TYPE_BOTTOM:
                size = 0;
                i = msg - 1;
                while(i > 0)
                {
                    if (size + (idxP->msgOffset)[i + 1] - (idxP->msgOffset)[i] >
                                                         (NNshGlobal->NNsiParam)->bufferSize)
                    {
                        break;
                    }
                    size = size + (idxP->msgOffset)[i + 1] - (idxP->msgOffset)[i];
                }
                offset = (idxP->msgOffset)[msg - 1] - size;
                break;
            }
        }
        
        if (flag == true)
        {
            // �u�҂āv�\�� (hideBusyForm()�܂�)
            Show_BusyForm(MSG_READ_MESSAGE_WAIT);
        }

        // �f�[�^���o�b�t�@�ɓǂݍ��ށB
        ret = ReadFile_NNsh(&(idxP->fileRef), offset, (NNshGlobal->NNsiParam)->bufferSize,
                            &(idxP->buffer[0]), &readSize);
        if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
        {
            idxP->fileOffset   = offset;
            idxP->fileReadSize = readSize;
        }

        if (flag == true) 
        {
            Hide_BusyForm(true);
        }
    }
    return(ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : UpdateFieldRegion                                          */
/*                                            �t�B�[���h�^�b�v�ʒu�̐ݒ�   */
/*-------------------------------------------------------------------------*/
void UpdateFieldRegion(void)
{
    FormPtr frm;
    RectangleType r;

    frm = FrmGetActiveForm();
    if(FrmGetFormId(frm) != FRMID_MESSAGE)
      return;

    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE), &r);
    
    NNshGlobal->fieldPos[0].startX = r.topLeft.x;
    NNshGlobal->fieldPos[0].startY = r.topLeft.y;
    NNshGlobal->fieldPos[0].endX   = r.topLeft.x + r.extent.x;
    NNshGlobal->fieldPos[0].endY   = r.topLeft.y + r.extent.y / 2;

    NNshGlobal->fieldPos[1].startX = r.topLeft.x;
    NNshGlobal->fieldPos[1].startY = r.topLeft.y + r.extent.y / 2;
    NNshGlobal->fieldPos[1].endX   = r.topLeft.x + r.extent.x;
    NNshGlobal->fieldPos[1].endY   = r.topLeft.y + r.extent.y;
}

/*-------------------------------------------------------------------------*/
/*   Function : getViewDataString                                          */
/*                              �\�����X���f�[�^����w�肳�ꂽ�f�[�^���擾 */
/*-------------------------------------------------------------------------*/
static void getViewDataString(Char *buffer, Char *headerStr, UInt16 bufSize)
{
    FormType  *frm;
    FieldType *fldP;
    MemHandle txtH;
    Char      *str, *ptr;
    UInt16     copySize;

    // ���o������̏�����
    MemSet(buffer, bufSize, 0x00);
    
    // �t�B�[���h���當������擾
    frm     = FrmGetActiveForm();
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
    txtH    = FldGetTextHandle(fldP);
    if (txtH == 0)
    {
         // �������n���h���擾���s
         return;
    }
    str = MemHandleLock(txtH);
    if (str == NULL)
    {
        // �������n���h�����b�N���s
        return;
    }

    ptr = StrStr(str, headerStr);
    if (ptr != NULL)
    {
        // ��������o�b�t�@�T�C�Y�����o(�X�y�[�X, ���s...����؂�ɂ���)
        copySize = 0;
        while ((*ptr > ' ')&&(copySize < (bufSize - 1)))
        {
             // ��������R�s�[
             buffer[copySize] = *ptr;
             ptr++;
             copySize++;
        } 
    }
    MemHandleUnlock(txtH);
}

/*-------------------------------------------------------------------------*/
/*   Function : executeViewAction                                          */
/*                                            �@�\�����s����(�n�[�h�L�[�p) */
/*-------------------------------------------------------------------------*/
static Boolean executeViewAction(UInt16 funcID)
{
    Char                 logMsg[BUFSIZE * 2], *txtP;
    Boolean              clip, err, ret = false;
    UInt16               item, frmId, start, end;
    FormType            *frm;
    FieldType           *fld;
    ControlType         *ctlP;
    NNshSubjectDatabase  subjDB;
    NNshMessageIndex    *idxP;
    NNshFileRef          fileRef;
    MemHandle            txtH;
    UInt32               size;
#ifdef USE_HANDERA
    VgaScreenModeType   handEraScreenMode;     // �X�N���[�����[�h
    VgaRotateModeType   handEraRotateMode;     // ��]���[�h
#endif

    // index��ϐ��ɓ����
    idxP = NNshGlobal->msgIndex;

    // �C���f�b�N�X���Ȃ��ꍇ�ɂ́A������return����
    if (NNshGlobal->msgIndex == NULL)
    {
        return (false);
    }

    // �@�\�R�}���h�ԍ���jumpSelection�Ɋi�[(����܂�ǂ��Ȃ�...�B)
    NNshGlobal->jumpSelection = funcID;
    switch (funcID)
    {
      case MULTIVIEWBTN_FEATURE_INFO:
        // "�X�����"
        item = 0;
        DisplayMessageInformations((NNshGlobal->NNsiParam)->openMsgIndex, &item);

        // ���C�ɓ����񃉃x�����X�V
        setFavoriteLabel(FrmGetActiveForm(), item);

        // �\�����X�V����(OS3.1�f�o�C�X�ł͐^�����ɂȂ邽��)
        FrmDrawForm(FrmGetActiveForm());
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_BACKTOLIST:
        // "�ꗗ��ʂ�"
        ReadMessage_Close(NNshGlobal->backFormId);
        break;


      case MULTIVIEWBTN_FEATURE_REPLYMSG:
        // ���p����������  (not BREAK!!)
        StrCopy(logMsg, ">>");
        NUMCATI(logMsg, idxP->openMsg);
        StrCat(logMsg, "\n");
        outputToFile(FILE_WRITEMSG, (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE), true, 3, logMsg);
        // break;

      case MULTIVIEWBTN_FEATURE_WRITE:
        // "��������"
        if (StrCompare(idxP->boardNick, OFFLINE_THREAD_NICK) != 0)
        {
            // OFFLINE�X�����Q�ƒ��łȂ��ꍇ�ɂ́A�������݉�ʂ֑J��
            ReadMessage_Close(FRMID_WRITE_MESSAGE);
            break;
        }

        // �Q�ƃ��O�̏ꍇ�ɂͤURL�I��\�����s���B
        if (NNshGlobal->browserCreator == 0)
        {
            // Web Browser���C���X�g�[�����ɂ́A"���݃T�|�[�g���Ă��܂���" �\�����s��
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "(WRITE)", 0);
            break;
        }

        // �A���J�[�ꗗ���s�b�N�A�b�v����
        MemSet(logMsg, sizeof(logMsg), 0x00);
        ret = pickupAnchor(NNshGlobal->msgIndex, logMsg, sizeof(logMsg));
        if (ret == true)
        {
            // �w�肳�ꂽURL���J��NetFront��NNsi�I����ɋN������
            (void) WebBrowserCommand(NNshGlobal->browserCreator, 0, 0, 
                                     NNshGlobal->browserLaunchCode,
                                     logMsg, StrLen(logMsg), NULL);
        }
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_GETPART:
        // "�����擾"
        (void) PartReceiveMessage_View();
        break;

      case MULTIVIEWBTN_FEATURE_GETMESNUM:
        // "�X���Ԏw��擾"
        clip  = false;

        //////// �����񂪑I����Ԃ�������A�I�𕶎�����N���b�v�{�[�h�ɃR�s�[
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId != noFocus)
        {        
            if (FrmGetObjectType(frm, frmId) == frmFieldObj)
            {
                fld = FrmGetObjectPtr(frm, frmId);
                FldGetSelection(fld, &start, &end);
                if (start != end)
                {
                    // �I�𕶎�����R�s�[����
                    FldCopy(FrmGetObjectPtr(frm, frmId));
                    if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
                    {
                        // �n�C���]�t�H���g�ݒ蒆�̎��ɂ̓t�B�[���h�ĕ`����s��
                        NNsi_HRFldDrawField(FrmGetObjectPtr(frm, frmId), 0);
                    }
                    clip = true;
                }
            }
        }

        // �N���b�v�{�[�h�̕�������g��Ȃ��ꍇ�ɂ́A�t�B�[���h����f�[�^���擾
        if (clip != true)
        {
            getViewDataString(logMsg, "/read.cgi/", sizeof(logMsg));
            if (logMsg[0] == '/')
            {
                // '/read.cgi/xxxx/yyyyyyyy' �� xxxx/yyyyyyyy �����𒊏o����
                txtP = logMsg + sizeof("/read.cgi/") - 1;
                while (*txtP != '\0')
                {
                    if (*txtP == '/')
                    {
                        //
                        if (clip == true)
                        {
                            *txtP = '\0';
                            break;
                        }
                        clip = true;
                    }
                    txtP++;
                }

                // ���o������������N���b�v�{�[�h�ɃR�s�[����
                txtP = logMsg + sizeof("/read.cgi/") - 1;
                ClipboardAddItem(clipboardText, txtP, (StrLen(txtP) + 1));
            }
        }

        // �X���Ԏw��擾�����{����
        ret = Get_MessageFromMesNum(&err, &item, clip);
        if (err == true)
        {
            // ����I���A���̂Ƃ���BBS�ꗗ�����������Ă���
            MEMFREE_PTR(NNshGlobal->bbsTitles);
        }
        // �X���Ԏw��擾�����s�����Ƃ�
        if (ret == true)
        {
            // �X���Ԏw��ō��J���Ă��郌�R�[�h�̔ԍ����ς�������m�F����B
            MemSet(&subjDB, sizeof(subjDB), 0x00);
            (void) Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
            if (StrCompare(subjDB.threadTitle, idxP->threadTitle) != 0)
            {
                // �X�����R�[�h�ԍ���(�X���Ԏw��擾�ɂ����)�ς�����I
                // (���R�[�h�ԍ����P���₷)
                ((NNshGlobal->NNsiParam)->openMsgIndex)++;
                ((NNshGlobal->NNsiParam)->startTitleRec)++;
                ((NNshGlobal->NNsiParam)->endTitleRec)++;
            }

            // �擾�\�񃂁[�h�łȂ��Ƃ��A�X���擾��A�X�����J�����ǂ����m�F����
            if ((NNshGlobal->NNsiParam)->getReserveFeature == 0)
            {
                if (NNsh_ConfirmMessage(ALTID_CONFIRM,
                                        MSG_OPEN_CONFIRM_MESNUM, "", 0) == 0)
                {
                    // "OK"��I�������ꍇ�A���̃X�����J��
                    // (�Q�Ɖ�ʂ���ĊJ���Ȃ���) 
                    (NNshGlobal->NNsiParam)->openMsgIndex      = item;
                    (NNshGlobal->NNsiParam)->startTitleRec     = item;
                    (NNshGlobal->NNsiParam)->endTitleRec       = item;
                    (NNshGlobal->NNsiParam)->selectedTitleItem = 0;
                    ReadMessage_Close(FRMID_MESSAGE);

                    // (�ꗗ��ʂ�BBS�ꗗ�́u�擾�ςݑS�āv�ɂ���)
//                    (NNshGlobal->NNsiParam)->lastBBS = NNSH_SELBBS_GETALL;
                    return (true);
                }
                else
                {
                    // �X���Ԏw��擾�������X���J���Ȃ��ꍇ�́A�ĕ`��
                    (void) displayMessage(NNshGlobal->msgIndex, 0);
                }
            }
        }
        else
        {
            // OS3.1�f�o�C�X�̎��ɃL�����Z�������ƁA��ʂ̍ĕ`�悪�K�v
            frm = FrmGetActiveForm();
            FrmDrawForm(frm);
            if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
            {
                // �n�C���]�t�H���g�ݒ蒆�̎��ɂ́A�t�B�[���h�̍ĕ`����s��
                fld = FrmGetObjectPtr(frm,
                                         FrmGetObjectIndex(frm,FLDID_MESSAGE));
                NNsi_HRFldDrawField(fld, 0);
            }
        }
        return (false);
        break;

      case MULTIVIEWBTN_FEATURE_GOTOTOP:
        // "�擪MSG��"
        jumpEdge(false);
        break;

      case MULTIVIEWBTN_FEATURE_PREVIOUS:
        // "�OMSG��"
        (void) moveMessagePrev(false, 0);
        break;

      case MULTIVIEWBTN_FEATURE_PAGEUP:
        // "�O�X�N���[��"
        (void) moveMessagePrev(true, (NNshGlobal->NNsiParam)->dispBottom);
        break;

      case MULTIVIEWBTN_FEATURE_PAGEDOWN:
        // "��X�N���[��"
        (void) moveMessageNext(true);
        break;

      case MULTIVIEWBTN_FEATURE_NEXT:
        // "��MSG��"
        (void) moveMessageNext(false);
        break;

      case MULTIVIEWBTN_FEATURE_GOTOBOTTOM:
        // "����MSG��"
        jumpEdge(true);
        break;

      case MULTIVIEWBTN_FEATURE_JUMP:
        // "Go"
        jumpMessage(FrmGetActiveForm(), NNshGlobal->msgIndex);
        if (NNshGlobal->work3 != 0)
        {
            // ���Xpop up�Astack���[�����ē����Ȃ�?���߂����Ɉړ�
            popUP_Message(NNshGlobal->work3);
            NNshGlobal->work3 = 0;
        }
        break;

      case MULTIVIEWBTN_FEATURE_RETURN:
        // "�P�߂�"
        jumpPrevious();
        break;

      case MULTIVIEWBTN_FEATURE_COPY:
        // "������R�s�["
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId == noFocus)
        {        
           // �t�H�[�J�X���擾�ł��Ȃ��A�I������
            break;
        }
        if (FrmGetObjectType(frm, frmId) == frmFieldObj)
        {
            FldCopy(FrmGetObjectPtr(frm, frmId));
            if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
            {
                // �n�C���]�t�H���g�ݒ蒆�̎��ɂ́A�t�B�[���h�̍ĕ`����s��
                NNsi_HRFldDrawField(FrmGetObjectPtr(frm, frmId), 0);
            }
        }
        break;

      case MULTIVIEWBTN_FEATURE_SELECTALL:
        // "�����񂷂ׂđI��"
        frm = FrmGetActiveForm();
        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        FldSetSelection(fld, 0, FldGetTextLength(fld));
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
        {
            // �n�C���]�t�H���g�ݒ蒆�̎��ɂ́A�t�B�[���h�̍ĕ`����s��
            NNsi_HRFldDrawField(fld, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_SELANDWEB:
        // �����񂷂ׂđI����AWeb Browser�ŊJ��
        frm = FrmGetActiveForm();
        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        FldSetSelection(fld, 0, FldGetTextLength(fld));
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        // not break; //

      case MULTIVIEWBTN_FEATURE_OPENWEB:
        // "�������Web"
        if (NNshGlobal->browserCreator == 0)
        {
            // "���݃T�|�[�g���Ă��܂���" �\�����s��
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
            break;
        }

        // �I�𕶎�����g���āANetFront�ŊJ��
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId == noFocus)
        {        
           // �t�H�[�J�X���擾�ł��Ȃ��A�I������
            break;
        }
        if (FrmGetObjectType(frm, frmId) == frmFieldObj)
        {
            // �I�����Ă��镶�������U�N���b�v�{�[�h�ɃR�s�[
            FldCopy(FrmGetObjectPtr(frm, frmId));

            // Web�u���E�U�ŊJ��
            openWebBrowser((NNshGlobal->NNsiParam)->confirmationDisable, logMsg, sizeof(logMsg));
        }
        break;

      case MULTIVIEWBTN_FEATURE_OUTPUTMEMO:
        // "�������o��"
        clip  = false;

        //////// �����񂪑I����Ԃ�������A�I�𕶎�����N���b�v�{�[�h�ɃR�s�[
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId != noFocus)
        {        
            if (FrmGetObjectType(frm, frmId) == frmFieldObj)
            {
                fld = FrmGetObjectPtr(frm, frmId);
                FldGetSelection(fld, &start, &end);
                if (start != end)
                {
                    // �I�𕶎�����R�s�[����
                    FldCopy(FrmGetObjectPtr(frm, frmId));
                    if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
                    {
                        // �n�C���]�t�H���g�ݒ蒆�̎��ɂ̓t�B�[���h�ĕ`����s��
                        NNsi_HRFldDrawField(FrmGetObjectPtr(frm, frmId), 0);
                    }
                    clip = true;
                }
            }
        }
        // �������ɏo�͂���
        outputMemoPad(clip);
        break;

      case MULTIVIEWBTN_FEATURE_SEARCH:
        // "����"
        frm  = FrmGetActiveForm();
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_SEARCH));
        if ((NNshGlobal->NNsiParam)->searchMode == 1)
        {
            // �������[�h�𔲂���
            (NNshGlobal->NNsiParam)->searchMode = 0;
            CtlSetValue(ctlP, 0);
            break;
        }
        // �����E�B���h�E�ɕ\�����镶�����\�����X�����璊�o����ꍇ...
        if ((NNshGlobal->NNsiParam)->enableSearchStrHeader != 0)
        {
            getViewDataString((NNshGlobal->NNsiParam)->searchStrMessage,
                              (NNshGlobal->NNsiParam)->viewSearchStrHeader,
                              (sizeof((NNshGlobal->NNsiParam)->searchStrMessage)));
        }
        if (DataInputDialog(MSG_DIALOG_SEARCH_MESSAGE,
                            (NNshGlobal->NNsiParam)->searchStrMessage, 
                            sizeof((NNshGlobal->NNsiParam)->searchStrMessage), 
                            NNSH_DIALOG_USE_SEARCH_MESSAGE, 
                            (void *)&item) == true)
        {
            CtlSetValue(ctlP, 1);
            (NNshGlobal->NNsiParam)->searchMode = 1;
            NNsh_Search_Message(item, (NNshGlobal->NNsiParam)->searchStrMessage, true);
        }
        else
        {
            // �������[�h�𔲂���
            (NNshGlobal->NNsiParam)->searchMode = 0;
            CtlSetValue(ctlP, 0);

            // OS3.1�p�ł͉�ʂ̍ĕ`�悪�K�v
            FrmDrawForm(frm);
            if ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
            {
                // �n�C���]�t�H���g�ݒ蒆�̎��ɂ́A�t�B�[���h�̍ĕ`����s��
                fld = FrmGetObjectPtr(frm,
                                      FrmGetObjectIndex(frm, FLDID_MESSAGE));
                NNsi_HRFldDrawField(fld, 0);
            }
        }
        break;

      case MULTIVIEWBTN_FEATURE_SEARCHNEXT:
        // "������"
        NNsh_Search_Message(NNSH_SEARCH_METHOD_NEXT, (NNshGlobal->NNsiParam)->searchStrMessage, false);
        break;

      case MULTIVIEWBTN_FEATURE_SEARCHPREV:
        // "�O����"
        NNsh_Search_Message(NNSH_SEARCH_METHOD_PREV, (NNshGlobal->NNsiParam)->searchStrMessage, false);
        break;

      case MULTIVIEWBTN_FEATURE_SEARCHTOP:
        // "�擪���猟��"
        NNsh_Search_Message(NNSH_SEARCH_METHOD_FROM_TOP, (NNshGlobal->NNsiParam)->searchStrMessage, false);
        break;

      case MULTIVIEWBTN_FEATURE_SEARCHBTM:
        // "�������猟��"
        NNsh_Search_Message(NNSH_SEARCH_METHOD_FROM_BOTTOM, (NNshGlobal->NNsiParam)->searchStrMessage, false);
        break;

      case MULTIVIEWBTN_FEATURE_CHANGEFONT:
        // "�t�H���g�ύX"
        changeFont();
        break;

      case MULTIVIEWBTN_FEATURE_DRAWMODE:
        // "�`�惂�[�h�ύX"
        if ((NNshGlobal->NNsiParam)->useSonyTinyFont == 0)
        {
            // �n�C���]���[�h�֕ύX
            (NNshGlobal->NNsiParam)->useSonyTinyFont = 1;
        }
        else
        {
            // �ʏ탂�[�h�֕ύX
            (NNshGlobal->NNsiParam)->useSonyTinyFont = 0;
        }
        ReadMessage_Close(FRMID_MESSAGE);
        return (true);
        break;

      case MULTIVIEWBTN_FEATURE_ROTATE:
        // "��ʂ̉�](HE)"
#ifdef USE_HANDERA
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
            VgaSetScreenMode(screenMode1To1, 
                             VgaRotateSelect(handEraRotateMode));

            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
            return (false);
            break;
        }
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            // ��ʉ�]�����s
            VgaFormRotate_DIA();

            frm = FrmGetActiveForm();
            // ��ʉ�]��̑傫���ɂ��킹�A�t�H�[���𒲐����`�悷��
            if (HandEraResizeForm(frm) == true)
            {
                FrmEraseForm(frm);
                NNsi_FrmDrawForm(frm, true);
            }
            return (false);
            break;
        }
#endif    // #ifdef USE_PIN_DIA

        // "���݃T�|�[�g���Ă��܂���" �\�����s��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        break;

      case MULTIVIEWBTN_FEATURE_DISCONNECT:
        // "����ؒf"
        NNshNet_LineHangup();
        break;

      case MULTIVIEWBTN_FEATURE_SETNETWORK:
        // "Network�ݒ�"
        OpenNetworkPreferences();
        break;

      case MULTIVIEWBTN_FEATURE_BOOKMARK:
        // "������ݒ�"
        if ((NNshGlobal->NNsiParam)->useBookmark == 0)
        {
            // "���݃T�|�[�g���Ă��܂���" �\�����s��
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
            break;
        }

        // ���݂̈ʒu���L������
        (NNshGlobal->NNsiParam)->lastFrmID     = NNSH_FRMID_MESSAGE;
        (NNshGlobal->NNsiParam)->bookMsgIndex  = (NNshGlobal->NNsiParam)->openMsgIndex;
        (NNshGlobal->NNsiParam)->bookMsgNumber = idxP->openMsg;
        NNsh_InformMessage(ALTID_INFO, MSG_SET_BOOKMARK, "", 0);
        break;

      case MULTIVIEWBTN_FEATURE_DEVICEINFO:
        // "�f�o�C�X���"
        ShowDeviceInfo_NNsh();
        break;

      case MULTIVIEWBTN_FEATURE_PAGEUP10:
        // 10���X�O
        if (idxP->openMsg > 1)
        {
            // ���b�Z�[�W�ԍ���10���炵�A��ʕ\���B
            idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                                                 (idxP->histIdx + 1) : 0;
            idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
            idxP->openMsg = (idxP->openMsg > 11) ? (idxP->openMsg - 10) : 1;
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_PAGEDOWN10:
        // 10���X���
        if (idxP->openMsg < idxP->nofMsg)
        {
            // ���b�Z�[�W�ԍ���10���₵�A��ʕ\���B
            idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                                                 (idxP->histIdx + 1) : 0;
            idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
            idxP->openMsg    = (idxP->openMsg < idxP->nofMsg - 10) ?
                                          (idxP->openMsg + 10) : idxP->nofMsg;
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_GOTODUMMY:
        // �_�~�[��ʂ֑J�ڂ�����
        ReadMessage_Close(FRMID_NNSI_DUMMY);
        break;

      case MULTIVIEWBTN_FEATURE_OPENURL:
        // WebBrowser��URL��I������
        if (NNshGlobal->browserCreator != 0)
        {
            // �A���J�[�ꗗ���s�b�N�A�b�v����
            MemSet(logMsg, sizeof(logMsg), 0x00);
            if ((idxP->bbsType & NNSH_BBSTYPE_MASK) == NNSH_BBSTYPE_HTML)
            {
                // WebBrowser��URL���J��(HTML�̃A���J�[�𒊏o)
                ret = pickupAnchor(NNshGlobal->msgIndex, logMsg, sizeof(logMsg));
            }
            else
            {
                // WebBrowser��URL���J��(���X����URL�𒊏o)
                ret = pickupAnchorURL(NNshGlobal->msgIndex, logMsg, sizeof(logMsg));
            }
            if (ret == true)
            {
                // �w�肳�ꂽURL���J��
                // NetFront���N������(NNsi�I����ɋN��)
                (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                          0, 0,  NNshGlobal->browserLaunchCode,
                                          logMsg, StrLen(logMsg), NULL);
            }
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_EXECA2DA:
        // �t�B�[���h����A�f�[�^���o��
        frm = FrmGetActiveForm();
        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        txtH    = FldGetTextHandle(fld);
        if (txtH == 0)
        {
            // �t�B�[���h�̃t�@�C���n���h���擾���s�A�A�A�I������
            return (false);
        }
        txtP = MemHandleLock(txtH);

        // �t�@�C���Ƀf�[�^���o�͂���
        ret = OpenFile_NNsh(FILE_SENDMSG,
                            (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                            &fileRef);
        if (ret == errNone)
        {
            (void) WriteFile_NNsh(&fileRef, 0, StrLen(txtP), txtP, &size);
            CloseFile_NNsh(&fileRef);
        }
        else
        {
            NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
        }
        MemHandleUnlock(txtH);

        // �N���b�v�{�[�h�ɁAa2DA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����B
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, A2DA_NNSIEXT_AADATASTART);
        StrCat (logMsg, A2DA_NNSIEXT_NNSICREATOR);
        StrCat (logMsg, A2DA_NNSIEXT_SRC_STREAM);
        StrCat (logMsg, A2DA_NNSIEXT_DATANAME);
        StrCat (logMsg, FILE_SENDMSG);
        StrCat (logMsg, A2DA_NNSIEXT_ENDDATANAME);
        StrCat (logMsg, A2DA_NNSIEXT_ENDDATA);
        ClipboardAddItem(clipboardText, logMsg, (StrLen(logMsg) + 1));

        // a2DA���N������
        (void) LaunchResource_NNsh('DAcc','a2DA','code',1000);
        break;

      case MULTIVIEWBTN_FEATURE_BT_ON:
        // Bluetooth ON
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(true);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MULTIVIEWBTN_FEATURE_BT_OFF:
        // Bluetooth OFF
#ifdef USE_BT_CONTROL
        OpenBluetoothPreferences(false);
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MULTIVIEWBTN_FEATURE_USAGE:
        // ����w���v�̕\��
        // (�N���b�v�{�[�h�ɁAnnDA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����)
        ClipboardAddItem(clipboardText, 
                         nnDA_NNSIEXT_VIEWSTART
                         nnDA_NNSIEXT_INFONAME
                         nnDA_NNSIEXT_HELPVIEW
                         nnDA_NNSIEXT_ENDINFONAME
                         nnDA_NNSIEXT_ENDVIEW,
                         sizeof(nnDA_NNSIEXT_VIEWSTART
                                 nnDA_NNSIEXT_INFONAME
                                 nnDA_NNSIEXT_HELPVIEW
                                 nnDA_NNSIEXT_ENDINFONAME
                                 nnDA_NNSIEXT_ENDVIEW));
        // nnDA���N������
        (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
        break;

      case MULTIVIEWBTN_FEATURE_MEMOURL:
        // URL��MemoURL�֑���
        if (CheckInstalledResource_NNsh('appl', 'asUR') == false)
        {
            // MemoURL���C���X�g�[������Ă��Ȃ������A��������������
            return (false);
        }
        MemSet(logMsg, sizeof(logMsg), 0x00);
        logMsg[0] = '1';
        StrCopy(&logMsg[2], idxP->threadTitle); // �X���^�C���^�C�g����...
        ret = pickupAnchorURL(NNshGlobal->msgIndex, 
                              &logMsg[StrLen(&logMsg[2]) + 3], 
                              (sizeof(logMsg) - (StrLen(&logMsg[2]) + 3) - 1));
        if (ret == true)
        {
            // �w�肳�ꂽURL��MemoURL�ɑ���
            (void) WebBrowserCommand('asUR', 0, 0, 0x8001, logMsg, sizeof(logMsg), &size);
        }
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_CHANGERAW:
        // RAW���[�h�g�O���ؑ�
        if ((NNshGlobal->NNsiParam)->rawDisplayMode != 0)
        {
            // RAW���[�hOFF
            (NNshGlobal->NNsiParam)->rawDisplayMode = 0;
        }
        else
        {
            // RAW���[�hON
            (NNshGlobal->NNsiParam)->rawDisplayMode = 1;
        }
        // �\�����Ȃ���
        // (void) displayMessage(NNshGlobal->msgIndex, 0);
        ReadMessage_Close(FRMID_MESSAGE);
        break;

#ifdef USE_LOGCHARGE
      case MULTIVIEWBTN_FEATURE_CHGSEPA:
        // ��؂蕶���ύX
        size = 0;
        if (SelectLogToken(&size) == BTNID_JUMPEXEC)
        {
            // ��؂��ύX(HTML�`���̂Ƃ��̂ݗL��)
            NNsh_DebugMessage(ALTID_INFO, "Selected Token ID:", "", size);            
            NNshGlobal->tokenId = size;
        }
        // �\�����Ȃ���
        ReadMessage_Close(FRMID_MESSAGE);
#endif  // #ifdef USE_LOGCHARGE
        break;

#ifdef USE_LOGCHARGE
      case MULTIVIEWBTN_FEATURE_EDITSEPA:
        // ��؂蕶���ҏW
        SetLogCharge_LogToken(0);
        // �Q�Ɖ�ʂ�\�����Ȃ���
        ReadMessage_Close(FRMID_MESSAGE);
        break;
#endif  // #ifdef USE_LOGCHARGE

      case MULTIVIEWBTN_FEATURE_OPENURL_HTTP:
        // WebBrowser��URL��I������
        if (NNshGlobal->browserCreator != 0)
        {
            // �A���J�[�ꗗ���s�b�N�A�b�v����
            MemSet(logMsg, sizeof(logMsg), 0x00);

            // WebBrowser��URL���J��(���X����URL�𒊏o)
            ret = pickupAnchorURL(NNshGlobal->msgIndex, logMsg, sizeof(logMsg));
            if (ret == true)
            {
                // �w�肳�ꂽURL���J��
                // NetFront���N������(NNsi�I����ɋN��)
                (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                          0, 0,  NNshGlobal->browserLaunchCode,
                                          logMsg, StrLen(logMsg), NULL);
            }
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_MEMOURLDA:
        // URL��MemoURLDA�֑���
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, idxP->threadTitle); // �X���^�C���^�C�g����...
        ret = pickupAnchorURL(NNshGlobal->msgIndex, 
                              &logMsg[StrLen(logMsg)], 
                              (sizeof(logMsg) - StrLen(logMsg) - 1));
        if (ret != true)
        {
            (void) displayMessage(NNshGlobal->msgIndex, 0);
            return (false);
        }

        // �N���b�v�{�[�h�ɑI������URL���R�s�[����
        ClipboardAddItem(clipboardText, logMsg, StrLen(logMsg));

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " COPIED URL :%s", logMsg);
#endif // #ifdef USE_REPORTER

        if (CheckInstalledResource_NNsh('DAcc', 'mmuA') != false)
        {
            // MemoURLDA���C���X�g�[������Ă����ꍇ...
            (void) LaunchResource_NNsh('DAcc','mmuA','code',1000);
        }
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_OPENWEBDA:
        // URL��webDA�ŊJ��
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, idxP->threadTitle); // �X���^�C���^�C�g����...
        ret = pickupAnchorURL(NNshGlobal->msgIndex, 
                              &logMsg[StrLen(logMsg)], 
                              (sizeof(logMsg) - StrLen(logMsg) - 1));
        if (ret != true)
        {
            (void) displayMessage(NNshGlobal->msgIndex, 0);
            return (false);
        }

#ifdef USE_LOGCHARGE
        start = StrLen(logMsg);
        if (((logMsg[start -1] == 'P')||(logMsg[start - 1] == 'p')||
             (logMsg[start -1] == 'G')||(logMsg[start - 1] == 'g')||
             (logMsg[start -1] == 'B')||(logMsg[start - 1] == 'b')||
             (logMsg[start -1] == 'C')||(logMsg[start - 1] == 'c'))&&
            ((logMsg[start -2] == 'P')||(logMsg[start - 2] == 'p')||
             (logMsg[start -2] == 'I')||(logMsg[start - 2] == 'i')||
             (logMsg[start -2] == 'D')||(logMsg[start - 2] == 'd')||
             (logMsg[start -2] == 'R')||(logMsg[start - 2] == 'r'))&&
            ((logMsg[start -3] == 'Z')||(logMsg[start - 3] == 'z')||
             (logMsg[start -3] == 'J')||(logMsg[start - 3] == 'j')||
             (logMsg[start -3] == 'P')||(logMsg[start - 3] == 'p'))&&
             (logMsg[start -4] == '.'))
        {
            // webDA�ł͂Ȃ��A���O�擾���[�h�ɐ؂�ւ���
            //  (URL�̖��[�� .jpg, .zip, .prc �܂��� .pdb �̂Ƃ�)
            GetImageFile(logMsg);

            // ��ʂ��ĕ`�悷��
            (void) displayMessage(NNshGlobal->msgIndex, 0);
            break;
        }
#endif	// #ifdef USE_LOGCHARGE

        // �N���b�v�{�[�h�ɑI������URL���R�s�[����
        ClipboardAddItem(clipboardText, logMsg, StrLen(logMsg));

#ifdef USE_REPORTER
        HostTraceOutputTL(appErrorClass, " COPIED URL :%s", logMsg);
#endif // #ifdef USE_REPORTER

        if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
        {
            // webDA���C���X�g�[������Ă����ꍇ...
            (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
        }
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

#ifdef USE_LOGCHARGE
      case MULTIVIEWBTN_FEATURE_GETIMAGE:
        // �t�@�C���� /Palm/Images�f�B���N�g���Ɏ擾(or �Q�Ɛ�p���O�Ɏ擾)
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, idxP->threadTitle); // �X���^�C���^�C�g����...
        ret = pickupAnchorURL(NNshGlobal->msgIndex, 
                              &logMsg[StrLen(logMsg)], 
                              (sizeof(logMsg) - StrLen(logMsg) - 1));
        if (ret != true)
        {
            (void) displayMessage(NNshGlobal->msgIndex, 0);
            return (false);
        }

        // �t�@�C�����擾����
        GetImageFile(logMsg);

        // ��ʂ��ĕ`�悷��
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;
#endif	// #ifdef USE_LOGCHARGE

      case MULTIVIEWBTN_FEATURE_DELETEBACK:
        // �폜��A�ꗗ�ɖ߂�...
        // (�ꗗ�ɖ߂��ݒ�B�B)
        // �X����{���ɍ폜���邩�m�F����
        item = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_DELETE_MSGCONFIRM, 
                                   idxP->threadTitle, 0);
        if (item != 0)
        {
            // �폜�L�����Z��
            return (false);
        }
        // ��ʂ���Ĉꗗ��ʂɖ߂鏀��...
        ReadMessage_Close(NNshGlobal->backFormId);

        // �X���폜������...
        DeleteThreadMessage((NNshGlobal->NNsiParam)->openMsgIndex);
        return (false);
        break;

      case MULTIVIEWBTN_FEATURE_VIEWBEPROF:
        // Be�v���t�B�[���Q��
        if (CheckInstalledResource_NNsh('DAcc', 'wbDA') != false)
        {
            // webDA���C���X�g�[������Ă����ꍇ...
            // (Be�v���t�B�[���̒��o)
            ret = pickupBeProfile(NNshGlobal->msgIndex, 
                                  &logMsg[StrLen(logMsg)], 
                                  (sizeof(logMsg) - StrLen(logMsg) - 1));
            if (ret == true)
            {
                // �N���b�v�{�[�h�ɑI������URL���R�s�[����
                ClipboardAddItem(clipboardText, logMsg, StrLen(logMsg));

                // webDA�̋N��
                (void) LaunchResource_NNsh('DAcc','wbDA','code',1000);
            }
            // ���X�̍ĕ\��
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        else
        {
            // webDA���C���X�g�[������Ă��Ȃ��ꍇ�B�B�B
            // ("webDA���Ȃ�"�\�����s���B)
            NNsh_InformMessage(ALTID_ERROR, MSG_NOTICE_WEBDA_INSTALL, "", 0);
        }
        break;

      case MULTIVIEWBTN_FEATURE_LVLUPDATE:
        // �X�����x���̍X�V
        updateThreadLevel();
        break;

      case MULTIVIEWBTN_FEATURE_PICKUP_NG:
        // NG���[�h�𒊏o���ēo�^
        MemSet (logMsg, sizeof(logMsg), 0x00);
        ret = pickupNGwordCandidate(NNshGlobal->msgIndex, logMsg, (NGWORD_LEN - 1));
        if (ret == true)
        {
            // NG�L�[���[�h��o�^����
            entryPickupedNGword(logMsg);
        }

        // ��ʂ��ĕ`�悷��
        (void) displayMessage(NNshGlobal->msgIndex, 0);
        break;

      case MULTIVIEWBTN_FEATURE_ENTRY_NG:
        // NG���[�h�ݒ�
        ret = SetNGword3();

        if (ret == true)
        {
            // �Q�Ɖ�ʂ�\�����Ȃ���
            ReadMessage_Close(FRMID_MESSAGE);
        }
        else
        {
            // ��ʂ��ĕ`�悷��
            (void) displayMessage(NNshGlobal->msgIndex, 0);
        }
        break;

      default:
        // ���̑�(�������Ȃ�)
        NNsh_DebugMessage(ALTID_WARN, "Unknown FuncID(view):", "", funcID);
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   entryPickupedNGword                                      */
/*                                                  ���o����NG���[�h�̓o�^ */
/*-------------------------------------------------------------------------*/
static void entryPickupedNGword(Char *ngWord)
{
    NNshNGwordDatabase  dbData;
    MemSet(&dbData, sizeof(NNshNGwordDatabase), 0x00);
    StrNCopy(dbData.ngWord, ngWord, (NGWORD_LEN - 1));
    dbData.checkArea     = 0;  // ���X�S�̂Ń`�F�b�N
    dbData.matchedAction = 0;  // �������ځ[��

    NNsh_DebugMessage(ALTID_WARN, "Entry NG WORD:", ngWord, 0);

    // NG-3�f�[�^�x�[�X�ւ̃f�[�^�o�^
    //  (�V�K�o�^�Ȃ̂ōŏ��̈����͂P�A2�Ԗڂ̈�����N/A�j
    (void) EntryNGword3(1, 0, &dbData);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlSelEvt_DispMessage                                    */
/*                                                      �{�^���������̏��� */
/*-------------------------------------------------------------------------*/
Boolean ctlSelEvt_DispMessage(EventType *event)
{
    // �C���f�b�N�X���Ȃ��ꍇ�ɂ́A������return����
    if (NNshGlobal->msgIndex == NULL)
    {
        return (false);
    }

    // �����ꂽ�{�^���ɂ���ď�������
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_TOP:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // ��������擪�Ɍ������Č���
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHBTM);
        }
        else
        {
            // �X���̐擪��
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_GOTOTOP);
        }
        break;

      case BTNID_PREV_MESSAGE:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // �O����
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHPREV);
        }
        else
        {
            // �O�{�^�� 
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_PREVIOUS);
        }
        break;

      case BTNID_NEXT_MESSAGE:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // ������
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHNEXT);
        }
        else
        {
            // ���{�^�� 
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_NEXT);
        }
        break;

      case BTNID_BOTTOM:
        if ((NNshGlobal->NNsiParam)->searchMode != 0)
        {
            // �擪���疖���Ɍ������Č���
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHTOP);
        }
        else
        {
            // �����{�^��(�X���̖�����)
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_GOTOBOTTOM);
        }
        break;

      case BTNID_BACK_MESSAGE:
        // �߂�{�^�� 
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_RETURN);
        break;

      case BTNID_JUMP:
        // GO�{�^��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_JUMP);
        break;

      case BTNID_WRITE_MESSAGE:
        // �����{�^��
        if ((NNshGlobal->NNsiParam)->writingReplyMsg == 0)
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_WRITE);
        }
        else
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_REPLYMSG);
        }
        break;

      case SELTRID_THREAD_TITLE:  
        // �^�C�g���{�^��(�ꗗ��ʂɖ߂�)
        (void) executeViewAction((NNshGlobal->NNsiParam)->viewTitleSelFeature);
        break;

      case SELTRID_FAVOR_THREAD:
        // �X������\������
        (void) executeViewAction((NNshGlobal->NNsiParam)->viewMultiBtnFeature);
        break;

      case CHKID_SEARCH:
        // �������[�h ON/OFF
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCH);
        break;

      default:
        // �������Ȃ�
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   MenuEvt_DispMessage                                      */
/*                                                    ���j���[�I�����̏��� */
/*-------------------------------------------------------------------------*/
Boolean menuEvt_DispMessage(EventType *event)
{
    switch (event->data.menu.itemID)
    {
      case MNUID_THREAD_SELECTION:
        // �X���I����ʂɖ߂�
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_BACKTOLIST);
        break;

      case MNUID_THREAD_INFO:
        // �X������\��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_INFO);
        break;

      case MNUID_TOP_MESSAGE:
        // �X���̐擪�ɃW�����v
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_GOTOTOP);
        break;

      case MNUID_BOTTOM_MESSAGE:
        // �X���̖����ɃW�����v
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_GOTOBOTTOM);
        break;

      case MNUID_WRITE_MESSAGE:
        // �X����������(OFFLINE�X���łȂ����̂ݏ������݉�) �� ���X���p�������݂̐ݒ�Ƌt�ɂȂ�
        if ((NNshGlobal->NNsiParam)->writingReplyMsg == 0)
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_REPLYMSG);
        }
        else
        {
            (void) executeViewAction(MULTIVIEWBTN_FEATURE_WRITE);
        }
        break;

      case MNUID_GET_PART:
        // �X�������擾
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_GETPART);
        break;

      case MNUID_GET_MESNUM:
        // �X���Ԏw��̃��b�Z�[�W�擾
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_GETMESNUM);
        break;

      case MNUID_NET_DISCONN:
        // ����ؒf
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_DISCONNECT);
        break;

      case MNUID_SET_BOOKMARK:
        // ������ݒ�
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_BOOKMARK);
        break;

      case MNUID_FONT_MSGVIEW:
        // �t�H���g�ύX
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_CHANGEFONT);
        break;

      case MNUID_HANDERA_ROTATE_MSGVIEW:
        // HandEra ��ʉ�] //
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_ROTATE);
        break;

      case MNUID_CLIE_DRAW_MODE: 
        // �`�惂�[�h�ύX
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_DRAWMODE);
        return (true);
        break;

      case MNUID_OS5_LAUNCH_WEB:
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_OPENWEB);
        break;

      case MNUID_EDIT_COPY:
        // �R�s�[
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_COPY);
        break;

      case MNUID_SELECT_ALL:
        // ������̑S�I��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SELECTALL);
        break;

      case MNUID_NET_CONFIG:
        // �l�b�g���[�N�ݒ���J��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SETNETWORK);
        break;

      case MNUID_SEARCH:
        // ����
        (NNshGlobal->NNsiParam)->searchMode = 0;
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCH);
        break;

      case MNUID_SEARCH_NEXT:
        // ������
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHNEXT);
        break;

      case MNUID_SEARCH_PREV:
        // �O����
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHPREV);
        break;

      case MNUID_SEARCH_TOP_TO_BOTTOM:
        // �擪���疖������
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHTOP);
        break;

      case MNUID_SEARCH_BOTTOM_TO_TOP:
        // ��������擪����
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_SEARCHBTM);
        break;

      case MNUID_OUTPUT_MEMOPAD:
        // ���b�Z�[�W���������֏o�͂���
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_OUTPUTMEMO);
        break;

      case MNUID_SHOW_DEVICEINFO:
        // �f�o�C�X���̕\��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_DEVICEINFO);
        break;

      case MNUID_START_A2DA:
        // a2DA�N��(AA�\��)
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_EXECA2DA);
        break;

      case MNUID_OPEN_URL:
        // URL���J��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_OPENURL);
        break;
        
      case MNUID_SHOW_HELP:
        // ���������\��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_USAGE);
        break;

      case MNUID_THREAD_DELETE:
        // �폜��ꗗ��ʂɖ߂�
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_DELETEBACK);
        break;

      case MNUID_VIEW_BE_PROFILE:
        // Be�v���t�B�[���Q��
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_VIEWBEPROF);
        break;

      case MNUID_ENTRY_NGWORD:
        // NG���[�h�o�^
        (void) executeViewAction(MULTIVIEWBTN_FEATURE_ENTRY_NG);
        break;

      default:
        // ��L�ȊO
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Hander_NNshMessage                                       */
/*                                                       �C�x���g�n���h��  */
/*-------------------------------------------------------------------------*/
Boolean Handler_NNshMessage(EventType *event)
{
    Boolean   ret = false;
#if defined(USE_HANDERA) || defined(USE_PIN_DIA)
    FormType *frm;
#endif

    switch (event->eType)
    {
      case keyDownEvent:
        return (selEvt_KeyDown_DispMessage(event));
        break;

      case ctlSelectEvent:
        return (ctlSelEvt_DispMessage(event));
        break;

      case fldChangedEvent:
        NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);
        return (false);
        break;

      case menuEvent:
        return (menuEvt_DispMessage(event));
        break;

      case sclRepeatEvent:
        return (sclRepEvt_DispMessage(event));
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent:
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            UpdateFieldRegion();
            NNsi_FrmDrawForm(frm, true);
        }
        break;
#endif
#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                // ���T�C�Y��̕`��
                UpdateFieldRegion();
                NNsi_FrmDrawForm(frm, true);
            }
        }
        break;
#endif

      case penDownEvent:
        if((NNshGlobal->NNsiParam)->useSonyTinyFont != 0)
        {
            /* tap ���ꂽ�̈���`�F�b�N *
             *     �㕔�G���A���`�F�b�N */
            if (NNshGlobal->fieldPos[0].startX <= event->screenX &&
               NNshGlobal->fieldPos[0].endX > event->screenX &&
               NNshGlobal->fieldPos[0].startY <= event->screenY &&
               NNshGlobal->fieldPos[0].endY > event->screenY)
           {
               (void) moveMessagePrev(true, (NNshGlobal->NNsiParam)->dispBottom);
               return(true);
           }
           else
           {
               /* �����G���A���`�F�b�N */
               if (NNshGlobal->fieldPos[1].startX <= event->screenX &&
                   NNshGlobal->fieldPos[1].endX > event->screenX &&
                   NNshGlobal->fieldPos[1].startY <= event->screenY &&
                   NNshGlobal->fieldPos[1].endY > event->screenY)
               {
                   (void) moveMessageNext(true);
                   return(true);
               }
           }
        }
        break;

      case frmOpenEvent:
        UpdateFieldRegion();
        break;
#if 0
      case menuOpenEvent:
      case fldEnterEvent:
      case popSelectEvent:
      case lstSelectEvent:
      case lstEnterEvent:
      case lstExitEvent:
      case sclExitEvent:
      case nilEvent:
#endif
      default:
        break;
    }
    return (ret);
}


/*=========================================================================*/
/*   Function : popUP_Message                                              */
/*                                (nnDA���g�������X�|�b�v�A�b�v�\���̎��s) */
/*=========================================================================*/
static void popUP_Message(UInt16 resNum)
{
    Err                  ret;
    Char                *cmdMsg, *txtP;
    UInt32               readSize;
    UInt16               size;
    MemHandle            memH;
    NNshFileRef          fileRef;

    if ((resNum < 1)&&((NNshGlobal->msgIndex)->nofMsg > resNum))
    {
        // �w�肳�ꂽ�������ُ�A�I������
        return;
    }

    // �N���b�v�{�[�h�p�̗̈���m�ۂ���
    cmdMsg = MEMALLOC_PTR(BUFSIZE * 2 + MARGIN);
    if (cmdMsg == NULL)
    {
        return;
    }
    MemSet(cmdMsg, (BUFSIZE * 2) + MARGIN, 0x00);

    // �̈���m�ۂ���
    size   = (((NNshGlobal->msgIndex)->msgOffset)[resNum] -
              ((NNshGlobal->msgIndex)->msgOffset)[resNum - 1]);
    if (size > ((NNshGlobal->NNsiParam)->bufferSize - (MARGIN * 3)))
    {
        size = (NNshGlobal->NNsiParam)->bufferSize - (MARGIN * 3);
    }
    memH = MemHandleNew(size + MARGIN * 3);
    if (memH == 0)
    {
        // �̈�m�ێ��s�A�����I������
        MEMFREE_PTR(cmdMsg);
        return;
    }
    txtP = MemHandleLock(memH);
    if (txtP == NULL)
    {
        // �̈惍�b�N���s�A�I������...
        MEMFREE_PTR(cmdMsg);
        MEMFREE_HANDLE(memH);
        return;
    }
    MemSet(txtP, (size + MARGIN * 3), 0x00);
#ifdef USE_REPORTER_VIEW
    HostTraceOutputTL(appErrorClass, "+++++ %lu :ALLOCATE BUFFER SIZE:%d ", memH, (size + MARGIN * 3));
#endif // #ifdef USE_REPORTER

    // ���X�ԍ���ǉ�����...
    StrCopy(txtP, "[");
    NUMCATI(txtP, resNum);
    StrCat (txtP, "]  ");
    
    // �f�[�^���o�b�t�@�ɓǂݍ���
    ret = ReadFile_NNsh(&((NNshGlobal->msgIndex)->fileRef),
                        ((NNshGlobal->msgIndex)->msgOffset)[resNum - 1], 
                        size, &txtP[StrLen(txtP)], &readSize);
    if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        // �t�@�C���Ƀf�[�^���o�͂���
        ret = OpenFile_NNsh(FILE_SENDMSG,
                            (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                            &fileRef);
        if (ret == errNone)
        {
            (void) WriteFile_NNsh(&fileRef, 0, StrLen(txtP), txtP, &readSize);
            CloseFile_NNsh(&fileRef);
        }
        else
        {
            // �o�b�t�@�̊J��...
            MemHandleUnlock(memH);
            MEMFREE_HANDLE(memH);

            // �t�@�C���I�[�v���Ɏ��s...
            NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
            return;
        }
#ifdef USE_REPORTER_VIEW
        HostTraceOutputTL(appErrorClass, "-----");
        HostTraceOutputTL(appErrorClass, "%s", txtP);
        HostTraceOutputTL(appErrorClass, "-----");
#endif // #ifdef USE_REPORTER

        // �o�b�t�@�̊J��...
        MemHandleUnlock(memH);
        MEMFREE_HANDLE(memH);

        // �ǂݏo�������A�|�b�v�A�b�v�\�����s��
        // (�N���b�v�{�[�h�ɁAnnDA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����)
        MemSet (cmdMsg, ((BUFSIZE * 2) + MARGIN), 0x00);
        StrCopy(cmdMsg, nnDA_NNSIEXT_VIEWSTART);
        StrCat (cmdMsg, nnDA_NNSIEXT_INFONAME);
        StrCat (cmdMsg, nnDA_NNSIEXT_FOLDHTMLFILE);
        // �����R�[�h�̌���
        switch ((NNshGlobal->msgIndex)->bbsType & NNSH_BBSTYPE_MASK)
        {
          case NNSH_BBSTYPE_SHITARABA:
          case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
          case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
          case NNSH_BBSTYPE_2ch_EUC:
            // �����R�[�h��EUC
            StrCat (cmdMsg, "e:send.txt");
            break;

          default:
            // �W���� shift jis
            StrCat (cmdMsg, "s:send.txt");
            break;
        }
        StrCat (cmdMsg, nnDA_NNSIEXT_ENDINFONAME);
        StrCat (cmdMsg, nnDA_NNSIEXT_ENDVIEW);
        ClipboardAddItem(clipboardText, cmdMsg, StrLen(cmdMsg) + 1); 

#ifdef USE_REPORTER_VIEW
        HostTraceOutputTL(appErrorClass, "===== CMD %s ", cmdMsg);
#endif // #ifdef USE_REPORTER

        // nnDA���N������
        (void) LaunchResource_NNsh('DAcc','nnDA','code',1000);
    }
    // �̈�̊J��
    MEMFREE_PTR(cmdMsg);

    return;
}

/*=========================================================================*/
/*   Function : updateThreadLevel                                          */
/*                                                        �X�����x���̍X�V */
/*=========================================================================*/
static void updateThreadLevel(void)
{
    Err                  ret;
    NNshSubjectDatabase  subjDb;

    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDb);
    if (ret != errNone)
    {
        // �f�[�^���擾�ł��Ȃ������Ƃ��ɂ͉������Ȃ�...
    }

    // BBS���ɐݒ肳�ꂽ�W���X�����x���𔽉f������
    switch ((subjDb.msgAttribute)&(NNSH_MSGATTR_FAVOR))
    {
      case NNSH_MSGATTR_FAVOR_L1:
        // 1���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L2));
        break;

      case NNSH_MSGATTR_FAVOR_L2:
        // 2���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L3));
        break;

      case NNSH_MSGATTR_FAVOR_L3:
        // 3���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L4));
        break;

      case NNSH_MSGATTR_FAVOR_L4:
        // H���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR));
        break;

      case NNSH_MSGATTR_FAVOR:
        // ���x�����N���A����
        subjDb.msgAttribute = (subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR));
        break;

      default:
        // L���x���ɐݒ�
        subjDb.msgAttribute
             = ((subjDb.msgAttribute&(NNSH_MSGATTR_NOTFAVOR))|(NNSH_MSGATTR_FAVOR_L1));
        break;
    }

    // �X�����x�������X�V����
    update_subject_database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDb);

    // �u���C�ɓ���v�ݒ�����Z���N�^�g���K�̃��x���ɐݒ肷��
    setFavoriteLabel(FrmGetActiveForm(), subjDb.msgAttribute);

    return;
}
