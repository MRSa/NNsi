// aada.c
// AADA main
//
// (C) NNsi project, 2003, all rights reserved.
//

#include <PalmOS.h>
#include "aada-rsc-defines.h"
#include "aada-fix.h"
#include "aada-defines.h"

/*-------------------------------------------------------------------------*/
/*   Function :  AAInsChangeAAList                                         */
/*                                            AA Ins �� AA List��ύX      */
/*-------------------------------------------------------------------------*/
static ListPtr AAInsChangeAAList(FormPtr frmP, UInt16 sel)
{
    AADADataType *adtP;
    ListPtr lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, LSTID_AA_ITEMS));
    MemPtr rscP, stringsPP;
    UInt16 count;
    DmOpenRef dbRef;

    // Global�̎擾
    FtrGet(AADA_Creator, ADT_FTRID, (UInt32 *)&adtP);
    
    // ���݂̃A�C�e�����X�g��Еt����
    if(adtP->AAItemStringsH)
    {
        MemHandleUnlock(adtP->AAItemStringsH);
        MemHandleFree(adtP->AAItemStringsH);
        adtP->AAItemStringsH = 0;
    }
    if(adtP->AAItemRscH)
    {
        MemHandleUnlock(adtP->AAItemRscH);
        DmReleaseResource(adtP->AAItemRscH);
        adtP->AAItemRscH = 0;
    }
    if(adtP->AAItemH)
    {
        MemHandleUnlock(adtP->AAItemH);
        MemHandleFree(adtP->AAItemH);
        adtP->AAItemH = 0;
    }

    // User defined�łȂ����...
    if(sel < adtP->categoryNum - 1)
    {      
        // �V�����A�C�e�����X�g�����蓖�Ă�
        adtP->AAItemRscH = DmGetResource('tSTL', AAMGR_AALIST1_STRINGTBL - sel);
        rscP = MemHandleLock(adtP->AAItemRscH);
        count =
            (*((UInt8 *)(rscP + StrLen((char *)rscP) + 1)) << 8) |
            *((UInt8 *)(rscP + StrLen((char *)rscP) + 2));
        adtP->AAItemStringsH = SysFormPointerArrayToStrings(rscP + StrLen((char *)rscP) + 3, count);
        stringsPP = MemHandleLock(adtP->AAItemStringsH);
        LstSetListChoices(lstP, stringsPP, count);
    } else {
       adtP->AAItemRscH = adtP->AAItemStringsH = 0;
       LstSetListChoices(lstP, NULL, 0);

        // User defined�Ȃ�A���������炳�����ƂЂ��ς��Ă���
        dbRef = DmOpenDatabaseByTypeCreator('DATA', 'memo', dmModeReadOnly);
        if (dbRef != NULL)
        {
            // aaDA�J�e�S���̃����f�[�^���擾����
            Char *ptr, *ptr2;
            UInt16 temp, recIndex, loop, loop2;
            recIndex = 0;
            temp = CategoryFind(dbRef, "aaDA");
            if (temp == dmAllCategories)
            {
                temp = CategoryFind(dbRef, "AADA");
            }
            if (temp == dmAllCategories)
            {
                //  aaDA�J�e�S�����Ȃ������ꍇ�A�i�f�[�^��S���i���āj�s����
                // aaDA�Ə����ꂽ�����f�[�^���擾����
                MemHandle memH;
                recIndex = DmNumRecords(dbRef);
                for (loop = 0; loop < recIndex; loop++)
                {
                    memH = DmQueryRecord(dbRef, loop); 
                    if (memH != 0)
                    {
                        ptr  = MemHandleLock(memH);
                        temp = StrLen(ptr);
                        // �����f�[�^�̐擪��[aA][aA]DA\x0a�ł͂��܂��Ă���ꍇ
                        // �i5 �́AStrLen("aaDA\x0a") �̈Ӗ��j
                        //if ((StrNCompare(ptr, "aaDA\x0a", 5) == errNone)
                        if (((*ptr == 'a')||(*ptr == 'A'))&&
                            ((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&
                            (*(ptr + 2) == 'D')&&
                            (*(ptr + 3) == 'A')&&
                            (*(ptr + 4) == '\x0a'))
                        {
                            ptr = ptr + 5;   // (5 ��StrLen("aaDA\x0a") �̈Ӗ�)
                            temp = StrLen(ptr);
                            // 8 �̓}�[�W��
                            adtP->AAItemH = MemHandleNew(temp + 8);
                            if (adtP->AAItemH != 0)
                            {
                                ptr2 = MemHandleLock(adtP->AAItemH);
                                MemSet (ptr2, (temp + 8), 0x00);
                                StrNCopy(ptr2, ptr, (temp + 8));
                                temp = StrLen(ptr2);
                                count = 0;
                                for (loop2 = 0; loop2 < temp; loop2++)
                                {
                                    if (ptr2[loop2] == '\x0a')
                                    {
                                        ptr2[loop2] = '\0';
                                        count++;
                                    }
                                }
                                if (count == 0)
                                {
                                    // �K����؂�͂P����
                                    count++;
                                }
                                adtP->AAItemStringsH = 
                                     SysFormPointerArrayToStrings(ptr2, count);
                                stringsPP = MemHandleLock(adtP->AAItemStringsH);
                                LstSetListChoices(lstP, stringsPP, count);
                            }
                        }
                        MemHandleUnlock(memH);
                    }
                }
                DmCloseDatabase(dbRef);

                // List�̃��Z�b�g
                LstSetSelection(lstP, noListSelection);
                LstSetTopItem(lstP, 0);

                // �I��
                return (lstP);
            }

            if (DmSeekRecordInCategory(dbRef, &recIndex, 0, dmSeekForward, temp) == errNone)
            {
                // aaDA�J�e�S���̃f�[�^�𔭌�����
                MemHandle memH;
                memH = DmQueryRecord(dbRef, recIndex); 
                if (memH != 0)
                {
                    ptr  = MemHandleLock(memH);
                    temp = StrLen(ptr);
                    adtP->AAItemH = MemHandleNew(temp + 8);  // 8 �̓}�[�W��
                    if (adtP->AAItemH != 0)
                    {
                        ptr2 = MemHandleLock(adtP->AAItemH);
                        MemSet (ptr2, (temp + 8), 0x00);
                        StrCopy(ptr2, ptr);
                        count = 0;
                        for (loop = 0; loop < temp; loop++)
                        {
                            if (ptr2[loop] == '\x0a')
                            {
                                ptr2[loop] = '\0';
                                count++;
                            }
                        }
                        if (count == 0)
                        {
                            // �K����؂�͂P����
                            count++;
                        }
                        adtP->AAItemStringsH = SysFormPointerArrayToStrings(ptr2, count);
                        stringsPP = MemHandleLock(adtP->AAItemStringsH);
                        LstSetListChoices(lstP, stringsPP, count);
                    }
                    MemHandleUnlock(memH);
                }
            }
            DmCloseDatabase(dbRef);
        }
    }

    // List�̃��Z�b�g
    LstSetSelection(lstP, noListSelection);
    LstSetTopItem(lstP, 0);

    return lstP;
}

/*-------------------------------------------------------------------------*/
/*   Function :  AAInsEventHandler                                         */
/*                                         AA Insert Form �� event ����    */
/*-------------------------------------------------------------------------*/
static Boolean AAInsEventHandler(EventPtr evP)
{
    Boolean handled = false;
    FormPtr frmP;
    ListPtr lstP;
    ControlType *ctlP;
    
    switch(evP->eType) {
        case ctlSelectEvent:
            if(evP->data.ctlSelect.controlID == BTNID_AA_ABOUT) {
              FrmAlert(ALERTID_AA_ABOUT);
              handled = true;
            }
            break;
       
        case popSelectEvent:
            frmP = FrmGetActiveForm();
            lstP = AAInsChangeAAList(frmP, evP->data.popSelect.selection);
            LstDrawList(lstP);
            break;

        case penDownEvent:
            if(evP->screenY < 0) {
                FrmReturnToForm(0);
            }
            break;

        case keyDownEvent:
            switch (evP->data.keyDown.chr)
            {
              // JOG PUSH/Z�L�[���������Ƃ��̏���
              case vchrJogRelease:
              case vchrThumbWheelPush:
              case chrCapital_Z:
              case chrSmall_Z:
                // OK�{�^�������������Ƃɂ���
                frmP = FrmGetActiveForm();
                ctlP = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,BTNID_AA_OK));
                CtlHitControl(ctlP);
                handled = false;   // JOG�A�V�X�g�̏ꍇ���l����...
                break;

              default:
                // �������Ȃ�
                handled = false;
                break;
            }
            break;

        default:
            // do nothing
            break;
    }
    return handled;
}

/*-------------------------------------------------------------------------*/
/*   Function :  AADAMain                                                  */
/*                                         AA Insert Form�\�� & AA�}������ */
/*-------------------------------------------------------------------------*/
void AADAMain(void)
{
  AADADataType *adtP;
  FieldPtr fldP = NULL;
  ListPtr lstP ,itemLstP;
  ControlPtr ctlP;
  MemHandle rscH, stringsH;
  MemPtr rscP;
  char **itemsPP, *itemSelectP;
  FormObjectKind focusKind = 0;
  UInt16 result, objIndex;
  Int16 itemSel;
  FormPtr frmP;

    frmP     = FrmGetActiveForm();  
    objIndex = FrmGetFocus(frmP);

    //����Focus���Ȃ�������...
    if (objIndex == noFocus)
    {
        // �t�B�[���h��(�����)�T��
        // (Treo650/Tungsten T5�ł́A�{�^�����������ƁA�t�B�[���h�̃t�H�[�J�X��
        //  �O���悤�Ɏd�l���ύX����Ă���B���̂��߁AKaniEdit/NNsi����aaDA��
        //  �N�����ł��Ȃ��Ȃ��Ă��܂����̂ŁA�����ł��̑΍���s���B )
        //  ...�Ȃ�̂��Ƃ͂Ȃ��A�t�B�[���h�𖳗�������āA�����ɉ�������
        result = FrmGetNumberOfObjects(frmP);
        for (objIndex = result; objIndex != 0; objIndex--)
        {
            // focus����Ă���Object�̎�ʂ��擾
            focusKind = FrmGetObjectType(frmP, (objIndex - 1));
            if ((focusKind == frmTableObj)||(focusKind == frmFieldObj))
            {
                // �}���ł������ȂƂ����������...������
                break;
            }
        }
        // �t�B�[���h�������������ǂ����m�F����
        if (objIndex == 0)
        {
            // �t�B�[���h���Ȃ�����...
            objIndex = noFocus;
        }
        else
        {
            objIndex = objIndex - 1;
        }
    }

    // ����Focus����������...
    if (objIndex != noFocus)
    {
#if 0
        // ����InsPt�����Ă�����...(adal�΍�ŃR�����g�ɂ��Ă݂�...)
        if (InsPtEnabled() == true)
#endif
        {
            // focus����Ă���Object�̎�ʂ��擾
            focusKind = FrmGetObjectType(frmP, objIndex);
            switch (focusKind)  
            {
              case frmTableObj:
                // ����Table�̃t�B�[���h��������...���gField Pointer���擾
                fldP = TblGetCurrentField(FrmGetObjectPtr(frmP, objIndex));
                break;

              case frmFieldObj:
                // �f��Field�������ꍇ,,,
                fldP = FrmGetObjectPtr(frmP, objIndex);
                break;

              default:
                // ��������Ȃ����,,,
                fldP = NULL;
                break;
            } 
        }
    }

    if (fldP == NULL)
    {
        // (Active)Field���Ȃ�������"�I�����Ă���"�Ƃ���
        FrmCustomAlert(ALERTID_AA_NOACTFIELD, "", "", "");
        return;
    }

    // ���L�f�[�^�̊m��
    adtP = MemPtrNew(sizeof(AADADataType));
    MemSet(adtP, sizeof(AADADataType), 0);

    // global pointer��feature�ɃZ�b�g
    FtrSet(AADA_Creator, ADT_FTRID, (UInt32)adtP);

    // AA Insert Form�̏�����
    frmP = FrmInitForm(FRMID_AA_INSERT);
    
    // AA Insert Form�̃C�x���g�n���h���o�^
    FrmSetEventHandler(frmP, AAInsEventHandler);
    
    // Category���X�g�pstring�̎擾
    rscH = DmGetResource('tSTL', AAMGR_CATEGORY_STRINGTBL);
    rscP = MemHandleLock(rscH);
    adtP->categoryNum =
        (*((UInt8 *)(rscP + StrLen((char *)rscP) + 1)) << 8) |
        *((UInt8 *)(rscP + StrLen((char *)rscP) + 2));
    stringsH = SysFormPointerArrayToStrings(rscP + StrLen((char *)rscP) + 3, adtP->categoryNum);
    itemsPP = MemHandleLock(stringsH);
    
    // category list��string�ݒ� & ����Popup�̐ݒ� & ���̃��X�g�Z�b�g
    lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, LSTID_AA_CATEGORY));
    LstSetListChoices(lstP, itemsPP, adtP->categoryNum);
    ctlP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, POPTRID_AA_CATEGORY));
    CtlSetLabel(ctlP, *itemsPP);
    AAInsChangeAAList(frmP, 0);
    
    // Dialog�\��
    result = FrmDoDialog(frmP);
    
    // Insert OK�Ȃ�...
    if(result == BTNID_AA_OK) {
        // �I�����ꂽ�A�C�e�����m�F
        itemLstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, LSTID_AA_ITEMS));
        itemSel = LstGetSelection(itemLstP);
        
        // �I���A�C�e������������t�B�[���h�ɑ}��
        if(itemSel != noListSelection)
        {
            itemSelectP = LstGetSelectionText(itemLstP, itemSel);
            FldInsert(fldP, itemSelectP, StrLen(itemSelectP));
            
            // �����ɃJ�[�\���𖖔��ɂ����Ă��郍�W�b�N����ꂽ�����c
            
        }
    }
    
    // ��n��
    MemHandleUnlock(stringsH);
    MemHandleFree(stringsH);
    MemHandleUnlock(rscH);
    DmReleaseResource(rscH);

    if (adtP->AAItemStringsH != 0)
    {
        MemHandleUnlock(adtP->AAItemStringsH);
        MemHandleFree(adtP->AAItemStringsH);
    }
    if (adtP->AAItemRscH != 0)
    {
        MemHandleUnlock(adtP->AAItemRscH);
        DmReleaseResource(adtP->AAItemRscH);
    }
    adtP->AAItemRscH = adtP->AAItemStringsH = 0;
    if (adtP->AAItemH != 0)
    {
        MemHandleUnlock(adtP->AAItemH);
        MemHandleFree(adtP->AAItemH);
        adtP->AAItemH = 0;
    }
    FrmDeleteForm(frmP);  

    // ��n��(Global)
    MemPtrFree(adtP);
    FtrUnregister(AADA_Creator, ADT_FTRID);
}
