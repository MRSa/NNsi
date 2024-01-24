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
/*                                            AA Ins の AA Listを変更      */
/*-------------------------------------------------------------------------*/
static ListPtr AAInsChangeAAList(FormPtr frmP, UInt16 sel)
{
    AADADataType *adtP;
    ListPtr lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, LSTID_AA_ITEMS));
    MemPtr rscP, stringsPP;
    UInt16 count;
    DmOpenRef dbRef;

    // Globalの取得
    FtrGet(AADA_Creator, ADT_FTRID, (UInt32 *)&adtP);
    
    // 現在のアイテムリストを片付ける
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

    // User definedでなければ...
    if(sel < adtP->categoryNum - 1)
    {      
        // 新しいアイテムリストを割り当てる
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

        // User definedなら、メモ帳からさくっとひっぱってくる
        dbRef = DmOpenDatabaseByTypeCreator('DATA', 'memo', dmModeReadOnly);
        if (dbRef != NULL)
        {
            // aaDAカテゴリのメモデータを取得する
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
                //  aaDAカテゴリがなかった場合、（データを全件ナメて）行頭に
                // aaDAと書かれたメモデータを取得する
                MemHandle memH;
                recIndex = DmNumRecords(dbRef);
                for (loop = 0; loop < recIndex; loop++)
                {
                    memH = DmQueryRecord(dbRef, loop); 
                    if (memH != 0)
                    {
                        ptr  = MemHandleLock(memH);
                        temp = StrLen(ptr);
                        // メモデータの先頭が[aA][aA]DA\x0aではじまっている場合
                        // （5 は、StrLen("aaDA\x0a") の意味）
                        //if ((StrNCompare(ptr, "aaDA\x0a", 5) == errNone)
                        if (((*ptr == 'a')||(*ptr == 'A'))&&
                            ((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&
                            (*(ptr + 2) == 'D')&&
                            (*(ptr + 3) == 'A')&&
                            (*(ptr + 4) == '\x0a'))
                        {
                            ptr = ptr + 5;   // (5 はStrLen("aaDA\x0a") の意味)
                            temp = StrLen(ptr);
                            // 8 はマージン
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
                                    // 必ず区切りは１つある
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

                // Listのリセット
                LstSetSelection(lstP, noListSelection);
                LstSetTopItem(lstP, 0);

                // 終了
                return (lstP);
            }

            if (DmSeekRecordInCategory(dbRef, &recIndex, 0, dmSeekForward, temp) == errNone)
            {
                // aaDAカテゴリのデータを発見した
                MemHandle memH;
                memH = DmQueryRecord(dbRef, recIndex); 
                if (memH != 0)
                {
                    ptr  = MemHandleLock(memH);
                    temp = StrLen(ptr);
                    adtP->AAItemH = MemHandleNew(temp + 8);  // 8 はマージン
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
                            // 必ず区切りは１つある
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

    // Listのリセット
    LstSetSelection(lstP, noListSelection);
    LstSetTopItem(lstP, 0);

    return lstP;
}

/*-------------------------------------------------------------------------*/
/*   Function :  AAInsEventHandler                                         */
/*                                         AA Insert Form の event 処理    */
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
              // JOG PUSH/Zキーを押したときの処理
              case vchrJogRelease:
              case vchrThumbWheelPush:
              case chrCapital_Z:
              case chrSmall_Z:
                // OKボタンを押したことにする
                frmP = FrmGetActiveForm();
                ctlP = FrmGetObjectPtr(frmP,FrmGetObjectIndex(frmP,BTNID_AA_OK));
                CtlHitControl(ctlP);
                handled = false;   // JOGアシストの場合も考えて...
                break;

              default:
                // 何もしない
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
/*                                         AA Insert Form表示 & AA挿入処理 */
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

    //もしFocusがなかったら...
    if (objIndex == noFocus)
    {
        // フィールドを(無理矢理)探す
        // (Treo650/Tungsten T5では、ボタンが押されると、フィールドのフォーカスが
        //  外れるように仕様が変更されている。そのため、KaniEdit/NNsiからaaDAの
        //  起動ができなくなってしまったので、ここでその対策を行う。 )
        //  ...なんのことはない、フィールドを無理矢理見つけて、そこに押し込む
        result = FrmGetNumberOfObjects(frmP);
        for (objIndex = result; objIndex != 0; objIndex--)
        {
            // focusされているObjectの種別を取得
            focusKind = FrmGetObjectType(frmP, (objIndex - 1));
            if ((focusKind == frmTableObj)||(focusKind == frmFieldObj))
            {
                // 挿入できそうなところを見つけた...抜ける
                break;
            }
        }
        // フィールドが見つかったかどうか確認する
        if (objIndex == 0)
        {
            // フィールドがなかった...
            objIndex = noFocus;
        }
        else
        {
            objIndex = objIndex - 1;
        }
    }

    // もしFocusがあったら...
    if (objIndex != noFocus)
    {
#if 0
        // もしInsPtがついていたら...(adal対策でコメントにしてみた...)
        if (InsPtEnabled() == true)
#endif
        {
            // focusされているObjectの種別を取得
            focusKind = FrmGetObjectType(frmP, objIndex);
            switch (focusKind)  
            {
              case frmTableObj:
                // もしTableのフィールドだったら...中身Field Pointerを取得
                fldP = TblGetCurrentField(FrmGetObjectPtr(frmP, objIndex));
                break;

              case frmFieldObj:
                // 素のFieldだった場合,,,
                fldP = FrmGetObjectPtr(frmP, objIndex);
                break;

              default:
                // そうじゃなければ,,,
                fldP = NULL;
                break;
            } 
        }
    }

    if (fldP == NULL)
    {
        // (Active)Fieldがなかったら"選択してちょ"という
        FrmCustomAlert(ALERTID_AA_NOACTFIELD, "", "", "");
        return;
    }

    // 共有データの確保
    adtP = MemPtrNew(sizeof(AADADataType));
    MemSet(adtP, sizeof(AADADataType), 0);

    // global pointerをfeatureにセット
    FtrSet(AADA_Creator, ADT_FTRID, (UInt32)adtP);

    // AA Insert Formの初期化
    frmP = FrmInitForm(FRMID_AA_INSERT);
    
    // AA Insert Formのイベントハンドラ登録
    FrmSetEventHandler(frmP, AAInsEventHandler);
    
    // Categoryリスト用stringの取得
    rscH = DmGetResource('tSTL', AAMGR_CATEGORY_STRINGTBL);
    rscP = MemHandleLock(rscH);
    adtP->categoryNum =
        (*((UInt8 *)(rscP + StrLen((char *)rscP) + 1)) << 8) |
        *((UInt8 *)(rscP + StrLen((char *)rscP) + 2));
    stringsH = SysFormPointerArrayToStrings(rscP + StrLen((char *)rscP) + 3, adtP->categoryNum);
    itemsPP = MemHandleLock(stringsH);
    
    // category listのstring設定 & 頭のPopupの設定 & 頭のリストセット
    lstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, LSTID_AA_CATEGORY));
    LstSetListChoices(lstP, itemsPP, adtP->categoryNum);
    ctlP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, POPTRID_AA_CATEGORY));
    CtlSetLabel(ctlP, *itemsPP);
    AAInsChangeAAList(frmP, 0);
    
    // Dialog表示
    result = FrmDoDialog(frmP);
    
    // Insert OKなら...
    if(result == BTNID_AA_OK) {
        // 選択されたアイテムを確認
        itemLstP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, LSTID_AA_ITEMS));
        itemSel = LstGetSelection(itemLstP);
        
        // 選択アイテムがあったらフィールドに挿入
        if(itemSel != noListSelection)
        {
            itemSelectP = LstGetSelectionText(itemLstP, itemSel);
            FldInsert(fldP, itemSelectP, StrLen(itemSelectP));
            
            // ここにカーソルを末尾にもってくるロジックを入れたいが…
            
        }
    }
    
    // 後始末
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

    // 後始末(Global)
    MemPtrFree(adtP);
    FtrUnregister(AADA_Creator, ADT_FTRID);
}
