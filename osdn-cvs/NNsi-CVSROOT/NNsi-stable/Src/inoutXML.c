/*============================================================================*
 *  FILE: 
 *     inoutXML.c
 *
 *  Description: 
 *     NNsiのパラメータ入出力(XML形式)
 *
 *===========================================================================*/
#define  INOUTXML_C
#include "local.h"

#ifdef USE_XML_OUTPUT
#include "xmldefs.h"

/*=========================================================================*/
/*   Function :   Output_NNsiSet_XML                                       */
/*                                         NNsi設定をXML形式で出力する処理 */
/*=========================================================================*/
Err Output_NNsiSet_XML(Char *fileName)
{
  // NNsi設定の構造体(NNshSavedPref)とこの下のテーブルが同期している必要がある。
  NNsiXMLItems *ptr, NNsiItem[] =
  {
    {SETITEM__handleName,             NNSISETFILE_TYPE_CHAR,   BUFSIZE,         &((NNshGlobal->NNsiParam)->handleName[0])}, 
    {SETITEM__bbs_URL,                NNSISETFILE_TYPE_CHAR,   MAX_URL,         &((NNshGlobal->NNsiParam)->bbs_URL[0])}, 
    {SETITEM__proxyURL,               NNSISETFILE_TYPE_CHAR,   MAX_URL,         &((NNshGlobal->NNsiParam)->proxyURL)}, 
    {SETITEM__netTimeout,             NNSISETFILE_TYPE_INT32,  sizeof(Int32),   &((NNshGlobal->NNsiParam)->netTimeout)}, 
    {SETITEM__getAllThread,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getAllThread)}, 
    {SETITEM__confirmationDisable,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->confirmationDisable)}, 
    {SETITEM__openAutomatic,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->openAutomatic)}, 
    {SETITEM__writeAlwaysSage,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->writeAlwaysSage)}, 
    {SETITEM__useBookmark,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useBookmark)}, 
    {SETITEM__searchCaseless,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->searchCaseless)}, 
    {SETITEM__offChkLaunch,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->offChkLaunch)}, 
    {SETITEM__disableUnderline,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->disableUnderline)}, 
    {SETITEM__debugMessageON,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->debugMessageON)}, 
    {SETITEM__printNofMessage,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->printNofMessage)}, 
    {SETITEM__useNameOneLine,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->useNameOneLine)}, 
    {SETITEM__boldMessageNum,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->boldMessageNum)}, 
    {SETITEM__useVFS,                 NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useVFS)}, 
    {SETITEM__bufferSize,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->bufferSize)}, 
    {SETITEM__partGetSize,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->partGetSize)}, 
    {SETITEM__enablePartGet,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->enablePartGet)}, 
/*
    {SETITEM__lastFrmID,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->lastFrmID)}, 
    {SETITEM__lastBBS,                NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->lastBBS)}, 
    {SETITEM__openMsgIndex,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->openMsgIndex)}, 
*/
    {SETITEM__bookMsgIndex,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->bookMsgIndex)}, 
    {SETITEM__notCursor,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->notCursor)}, 
    {SETITEM__bookMsgNumber,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->bookMsgNumber)}, 
    {SETITEM__openThreadIndex,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->openThreadIndex)}, 
    {SETITEM__useFixedHandle,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useFixedHandle)}, 
    {SETITEM__disconnectNNsiEnd,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->disconnectNNsiEnd)}, 
    {SETITEM__useProxy,               NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useProxy)}, 
    {SETITEM__proxyPort,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->proxyPort)}, 
    {SETITEM__useCookieWrite,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useCookieWrite)}, 
    {SETITEM__copyDelReadOnly,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->copyDelReadOnly)}, 
    {SETITEM__vfsUseCompactFlash,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->vfsUseCompactFlash)}, 
    {SETITEM__vfsOnAutomatic,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->vfsOnAutomatic)}, 
    {SETITEM__currentFont,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->currentFont)}, 
    {SETITEM__notListReadOnlyNew,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notListReadOnlyNew)}, 
    {SETITEM__notListReadOnly,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notListReadOnly)}, 
    {SETITEM__redrawAfterConnect,     NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->redrawAfterConnect)}, 
    {SETITEM__insertReplyNum,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->insertReplyNum)}, 
    {SETITEM__useARMlet,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useARMlet)}, 
    {SETITEM__sonyHRFont,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->sonyHRFont)}, 
    {SETITEM__sonyHRFontTitle,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->sonyHRFontTitle)}, 
    {SETITEM__useSonyTinyFont,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useSonyTinyFont)}, 
    {SETITEM__useSonyTinyFontTitle,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useSonyTinyFontTitle)}, 
    {SETITEM__notOpenSilkWrite,       NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notOpenSilkWrite)}, 
    {SETITEM__notAutoFocus,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notAutoFocus)}, 
    {SETITEM__writeBufferSize,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->writeBufferSize)}, 
    {SETITEM__browseMesNum,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->browseMesNum)}, 
    {SETITEM__startMacroArrivalEnd,   NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->startMacroArrivalEnd)}, 
    {SETITEM__disconnArrivalEnd,      NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->disconnArrivalEnd)}, 
    {SETITEM__autoBeep,               NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->autoBeep)}, 
    {SETITEM__autoOpenNotRead,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->autoOpenNotRead)}, 
    {SETITEM__addReturnToList,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addReturnToList)}, 
    {SETITEM__jogBackBtnFeature,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.jogBack)}, 
    {SETITEM__notUseSilk,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->notUseSilk)}, 
    {SETITEM__writeSequence2,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->writeSequence2)}, 
    {SETITEM__startTitleRec,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->startTitleRec)}, 
    {SETITEM__endTitleRec,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->endTitleRec)}, 
    {SETITEM__currentSelectRec,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->currentSelectRec)}, 
    {SETITEM__selectedTitleItem,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->selectedTitleItem)}, 
    {SETITEM__titleDispState,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->titleDispState)}, 
    {SETITEM__jogPushBtnFeature,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.jogPush)}, 
    {SETITEM__convertHanZen,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->convertHanZen)},
    {SETITEM__addLineDisconn,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addLineDisconn)}, 
    {SETITEM__addLineGetPart,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addLineGetPart)}, 
    {SETITEM__addJumpTopMsg,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addJumpTopMsg)}, 
    {SETITEM__addJumpBottomMsg,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addJumpBottomMsg)}, 
    {SETITEM__addMenuSelAndWeb,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuSelAndWeb)}, 
    {SETITEM__addMenuMsg,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuMsg)}, 
    {SETITEM__addMenuBackRtnMsg,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuBackRtnMsg)}, 
    {SETITEM__addMenuGraphView,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuGraphView)}, 
    {SETITEM__writingReplyMsg,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->writingReplyMsg)}, 
    {SETITEM__addMenuFavorite,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addMenuFavorite)}, 
    {SETITEM__addMenuGetThreadNum,    NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addMenuGetThreadNum)}, 
    {SETITEM__addMenuOutputMemo,      NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addMenuOutputMemo)}, 
    {SETITEM__addBtOnOff,             NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addBtOnOff)}, 
    {SETITEM__addMenuTitle,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addMenuTitle)}, 
    {SETITEM__addMenuDeleteMsg,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuDeleteMsg)}, 
    {SETITEM__addMenuCopyMsg,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuCopyMsg)}, 
    {SETITEM__addMenuGetMode,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuGetMode)}, 
    {SETITEM__addMenuGraphTitle,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuGraphTitle)}, 
    {SETITEM__addMenuMultiSW1,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuMultiSW1)}, 
    {SETITEM__addMenuMultiSW2,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuMultiSW2)}, 
    {SETITEM__addMenuNNsiEnd,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuNNsiEnd)}, 
    {SETITEM__addMenuDirSelect,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuDirSelect)},
    {SETITEM__disableSonyHR,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->disableSonyHR)}, 
    {SETITEM__useColor,               NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useColor)}, 
    {SETITEM__colorError,             NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorError)}, 
    {SETITEM__colorOver,              NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorOver)}, 
    {SETITEM__colorNotYet,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorNotYet)}, 
    {SETITEM__colorNew,               NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorNew)}, 
    {SETITEM__colorUpdate,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorUpdate)}, 
    {SETITEM__colorRemain,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorRemain)}, 
    {SETITEM__colorAlready,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorAlready)}, 
    {SETITEM__colorUnknown,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorUnknown)}, 
    {SETITEM__colorBackGround,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorBackGround)}, 
    {SETITEM__colorButton,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorButton)}, 
    {SETITEM__colorViewFG,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorViewFG)}, 
    {SETITEM__colorViewBG,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorViewBG)},
    {SETITEM__rawDisplayMode,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->rawDisplayMode)}, 
    {SETITEM__notDelOffline,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notDelOffline)}, 
    {SETITEM__sortGoMenuAnchor,       NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->sortGoMenuAnchor)}, 
    {SETITEM__notFocusNumField,       NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notFocusNumField)}, 
    {SETITEM__disableUnderlineWrite,  NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->disableUnderlineWrite)}, 
    {SETITEM__usageOfTitleMultiSwitch1,NNSISETFILE_TYPE_UINT16,sizeof(UInt16),  &((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1)}, 
    {SETITEM__usageOfTitleMultiSwitch2,NNSISETFILE_TYPE_UINT16,sizeof(UInt16),  &((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2)}, 
    {SETITEM__autoDeleteNotYet,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->autoDeleteNotYet)}, 
    {SETITEM__notCheckBBSURL,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->notCheckBBSURL)}, 
    {SETITEM__displayFavorLevel,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->displayFavorLevel)}, 
    {SETITEM__newArrivalNotRead,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->newArrivalNotRead)}, 
    {SETITEM__getReserveFeature,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getReserveFeature)}, 
    {SETITEM__searchStrMessage,       NNSISETFILE_TYPE_CHAR, MAX_SEARCH_STRING, &((NNshGlobal->NNsiParam)->searchStrMessage[0])}, 
    {SETITEM__searchMode,             NNSISETFILE_TYPE_UINT32, sizeof(UInt32),  &((NNshGlobal->NNsiParam)->searchMode)}, 
    {SETITEM__bbsOverwrite,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->bbsOverwrite)}, 
    {SETITEM__useImodeURL,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useImodeURL)}, 
    {SETITEM__nofRetry,               NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->nofRetry)}, 
    {SETITEM__autoUpdateGetError,     NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->autoUpdateGetError)}, 
    {SETITEM__dispBottom,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->dispBottom)}, 
    {SETITEM__blockDispMode,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->blockDispMode)}, 
    {SETITEM__multiBtn1Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn1Feature)}, 
    {SETITEM__multiBtn1Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn1Caption[0])}, 
    {SETITEM__multiBtn2Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn2Feature)}, 
    {SETITEM__multiBtn2Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn2Caption[0])}, 
    {SETITEM__multiBtn3Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn3Feature)}, 
    {SETITEM__multiBtn3Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn3Caption[0])}, 
    {SETITEM__multiBtn4Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn4Feature)}, 
    {SETITEM__multiBtn4Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn4Caption[0])}, 
    {SETITEM__multiBtn5Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn5Feature)}, 
    {SETITEM__multiBtn5Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn5Caption[0])}, 
    {SETITEM__multiBtn6Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn6Feature)}, 
    {SETITEM__multiBtn6Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn6Caption[0])}, 
    {SETITEM__multiBtnUpFeature,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.up)},  
    {SETITEM__multiBtnDownFeature,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.down)},  
    {SETITEM__multiBtnHKey1Feature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.key1)},  
    {SETITEM__multiBtnHKey2Feature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.key2)},  
    {SETITEM__multiBtnHKey3Feature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.key3)},  
    {SETITEM__multiBtnHKey4Feature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.key4)},  
    {SETITEM__multiBtnCapture,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.clieCapture)},  
    {SETITEM__multiBtnLeftFeature,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.left)},  
    {SETITEM__multiBtnRightFeature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.right)},  
    {SETITEM__useHardKeyControl,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useHardKeyControl)},
    {SETITEM__checkDuplicateThread,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->checkDuplicateThread)}, 
    {SETITEM__hideMessage,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->hideMessage)}, 
    {SETITEM__hideWord1,              NNSISETFILE_TYPE_CHAR,   HIDEBUFSIZE,      &((NNshGlobal->NNsiParam)->hideWord1[0])}, 
    {SETITEM__hideWord2,              NNSISETFILE_TYPE_CHAR,   HIDEBUFSIZE,      &((NNshGlobal->NNsiParam)->hideWord2[0])}, 
    {SETITEM__CS1tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,    &((NNshGlobal->NNsiParam)->custom1.tabName[0])}, 
    {SETITEM__CS1newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.newArrival)}, 
    {SETITEM__CS1getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.getList)}, 
    {SETITEM__CS1condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.condition)}, 
    {SETITEM__CS1boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.boardNick)}, 
    {SETITEM__CS1boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.boardCondition)}, 
    {SETITEM__CS1threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.threadLevel)}, 
    {SETITEM__CS1threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.threadStatus)},
    {SETITEM__CS1threadCreate,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.threadCreate)}, 
    {SETITEM__CS1stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.stringSet)}, 
    {SETITEM__CS1string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom1.string1[0])}, 
    {SETITEM__CS1string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom1.string2[0])}, 
    {SETITEM__CS1string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom1.string3[0])}, 
    {SETITEM__CS2tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,    &((NNshGlobal->NNsiParam)->custom2.tabName[0])}, 
    {SETITEM__CS2newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.newArrival)}, 
    {SETITEM__CS2getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.getList)}, 
    {SETITEM__CS2condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.condition)}, 
    {SETITEM__CS2boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.boardNick)}, 
    {SETITEM__CS2boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.boardCondition)}, 
    {SETITEM__CS2threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.threadLevel)}, 
    {SETITEM__CS2threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.threadStatus)}, 
    {SETITEM__CS2threadCreate,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.threadCreate)}, 
    {SETITEM__CS2stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.stringSet)}, 
    {SETITEM__CS2string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom2.string1[0])}, 
    {SETITEM__CS2string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom2.string2[0])}, 
    {SETITEM__CS2string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom2.string3[0])}, 
    {SETITEM__CS3tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,    &((NNshGlobal->NNsiParam)->custom3.tabName[0])}, 
    {SETITEM__CS3newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.newArrival)}, 
    {SETITEM__CS3getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.getList)}, 
    {SETITEM__CS3condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.condition)}, 
    {SETITEM__CS3boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.boardNick)}, 
    {SETITEM__CS3boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.boardCondition)}, 
    {SETITEM__CS3threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.threadLevel)}, 
    {SETITEM__CS3threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.threadStatus)},
    {SETITEM__CS3threadCreate,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.threadCreate)}, 
    {SETITEM__CS3stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.stringSet)}, 
    {SETITEM__CS3string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom3.string1[0])}, 
    {SETITEM__CS3string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom3.string2[0])}, 
    {SETITEM__CS3string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom3.string3[0])}, 
    {SETITEM__CS4tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,    &((NNshGlobal->NNsiParam)->custom4.tabName[0])}, 
    {SETITEM__CS4newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.newArrival)}, 
    {SETITEM__CS4getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.getList)}, 
    {SETITEM__CS4condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.condition)}, 
    {SETITEM__CS4boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.boardNick)}, 
    {SETITEM__CS4boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.boardCondition)}, 
    {SETITEM__CS4threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.threadLevel)}, 
    {SETITEM__CS4threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.threadStatus)},
    {SETITEM__CS4threadCreate,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.threadCreate)}, 
    {SETITEM__CS4stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.stringSet)}, 
    {SETITEM__CS4string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom4.string1[0])}, 
    {SETITEM__CS4string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom4.string2[0])}, 
    {SETITEM__CS4string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom4.string3[0])}, 
    {SETITEM__CS5tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,    &((NNshGlobal->NNsiParam)->custom5.tabName[0])}, 
    {SETITEM__CS5newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.newArrival)}, 
    {SETITEM__CS5getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.getList)}, 
    {SETITEM__CS5condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.condition)}, 
    {SETITEM__CS5boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.boardNick)}, 
    {SETITEM__CS5boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.boardCondition)}, 
    {SETITEM__CS5threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.threadLevel)}, 
    {SETITEM__CS5threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.threadStatus)},
    {SETITEM__CS5threadCreate,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.threadCreate)}, 
    {SETITEM__CS5stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.stringSet)}, 
    {SETITEM__CS5string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom5.string1[0])}, 
    {SETITEM__CS5string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom5.string2[0])}, 
    {SETITEM__CS5string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom5.string3[0])}, 
    {SETITEM__listMesNumIsNotReadNum, NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum)}, 
    {SETITEM__writeJogPushDisable,    NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->writeJogPushDisable)}, 
    {SETITEM__autostartMacro      ,   NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->autostartMacro)}, 
    {SETITEM__enableNewArrivalHtml,   NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->enableNewArrivalHtml)}, 
    {SETITEM__useRegularExpression,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useRegularExpression)}, 
    {SETITEM__vfsOnNotDBCheck,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->vfsOnNotDBCheck)}, 
    {SETITEM__writeMessageAutoSave,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->writeMessageAutoSave)}, 
    {SETITEM__useHardKey1,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.key1)},  
    {SETITEM__useHardKey2,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.key2)},  
    {SETITEM__useHardKey3,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.key3)},  
    {SETITEM__useHardKey4,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.key4)},  
    {SETITEM__useClieCapture,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.clieCapture)},  
    {SETITEM__useLeftBtn,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.left)},  
    {SETITEM__useRightBtn,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.right)},  
    {SETITEM__addMenuDeviceInfo,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuDeviceInfo)},
    {SETITEM__addNgSetting3,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addNgSetting3)},
    {SETITEM__addDeviceInfo,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addDeviceInfo)},
    {SETITEM__useViewHardKey1,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.key1)},
    {SETITEM__useViewHardKey2,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.key2)},
    {SETITEM__useViewHardKey3,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.key3)},
    {SETITEM__useViewHardKey4,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.key4)},
    {SETITEM__useViewClieCapture,     NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.clieCapture)},
    {SETITEM__useViewUp,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.up)},
    {SETITEM__useViewDown,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.down)},
    {SETITEM__useViewLeft,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.left)},
    {SETITEM__useViewRight,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.right)},
    {SETITEM__useViewJogPush,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.jogPush)}, 
    {SETITEM__useViewJogBack,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.jogBack)},
    {SETITEM__readOnlySelection,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->readOnlySelection)},
    {SETITEM__use_DAplugin,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->use_DAplugin)},
    {SETITEM__listAndUpdate,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->listAndUpdate)},
    {SETITEM__msgNumLimit,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->msgNumLimit)},
    {SETITEM__notUseTsPatch,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notUseTsPatch)},
    {SETITEM__getROLogLevel,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->getROLogLevel)},
    {SETITEM__viewMultiBtnFeature,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewMultiBtnFeature)},
    {SETITEM__viewSearchStrHeader,    NNSISETFILE_TYPE_CHAR,   SEARCH_HEADLEN,   &((NNshGlobal->NNsiParam)->viewSearchStrHeader[0])},
    {SETITEM__enableSearchStrHeader,  NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->enableSearchStrHeader)},
    {SETITEM__viewTitleSelFeature,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewTitleSelFeature)},
    {SETITEM__oysterUserId,           NNSISETFILE_TYPE_CHAR,   OYSTERID_LEN,     &((NNshGlobal->NNsiParam)->oysterUserId[0])}, 
    {SETITEM__messageSeparator,       NNSISETFILE_TYPE_CHAR,   SEPARATOR_LEN,    &((NNshGlobal->NNsiParam)->messageSeparator[0])}, 
    {SETITEM__colorMsgHeader,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgHeader)},
    {SETITEM__colorMsgHeaderBold,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgHeaderBold)},
    {SETITEM__colorMsgFooter,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgFooter)},
    {SETITEM__colorMsgFooterBold,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgFooterBold)},
    {SETITEM__colorMsgInform,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgInform)},
    {SETITEM__colorMsgInformBold,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgInformBold)},
    {SETITEM__useColorMessageToken,   NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->useColorMessageToken)},
    {SETITEM__useColorMessageBold,    NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->useColorMessageBold)},
    {SETITEM__getLogFeatureJogPush,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.jogPush)},
    {SETITEM__getLogFeatureJogBack,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.jogBack)},
    {SETITEM__getLogFeatureJogLeft,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.left)},
    {SETITEM__getLogFeatureJogRight,  NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.right)},
    {SETITEM__getLogFeatureUp,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.up)},
    {SETITEM__getLogFeatureDown,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.down)},
    {SETITEM__getLogFeatureCapture,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.clieCapture)},
    {SETITEM__getLogFeatureKey1,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.key1)},
    {SETITEM__getLogFeatureKey2,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.key2)},
    {SETITEM__getLogFeatureKey3,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.key3)},
    {SETITEM__getLogFeatureKey4,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.key4)},
    {SETITEM__prepareDAforNewArrival, NNSISETFILE_TYPE_UINT32, sizeof(UInt32),  &((NNshGlobal->NNsiParam)->prepareDAforNewArrival)},
    {SETITEM__preOnDAnewArrival,      NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->preOnDAnewArrival)},
    {SETITEM__showResPopup,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->showResPopup)},
    {SETITEM__useGZIP,                NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->useGZIP)},
    {SETITEM__vfsOnDefault,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->vfsOnDefault)},
    {SETITEM__useBe2chInfo,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useBe2chInfo)},
    {SETITEM__be2chId,                NNSISETFILE_TYPE_CHAR,   BE2chID_LEN,      &((NNshGlobal->NNsiParam)->be2chId[0])}, 
    {SETITEM__be2chPw,                NNSISETFILE_TYPE_CHAR,   BE2chPW_LEN,      &((NNshGlobal->NNsiParam)->be2chPw[0])}, 
    {SETITEM__roundTripDelay,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->roundTripDelay)},
    {SETITEM__searchPickupMode,       NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->searchPickupMode)},
    {SETITEM__getLogSiteListMode,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->getLogSiteListMode)},
    {"", 0, 0, 0}
  };
  Err          ret;
  UInt32       temp;
  UInt16       len, mod;
  Char        *buffer;
  NNshFileRef  fileRefW;

    // 領域の作成
    len    = NNSH_WORKBUF_DEFAULT + MARGIN;
    buffer = MEMALLOC_PTR(len);
    if (buffer == NULL)
    {
        // 領域確保失敗
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", len);
        return (~errNone);
    }
    MemSet(buffer, len, 0x00);

    // 既にファイルが存在した場合は一旦削除してからオープンする。
    ret = OpenFile_NNsh(fileName, NNSH_FILEMODE_CREATE, &fileRefW);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, MSG_WRITEFILE_OPENFAIL, fileName, ret);
        MEMFREE_PTR(buffer);
        return (~errNone);
    }

    // ファイル出力中の表示
    Show_BusyForm(MSG_OUTPUT_XML_WAIT);

    // ＸＭＬヘッダ出力
    MemSet (buffer, len, 0x00);
    StrCopy(buffer, NNSISETFILE_HEAD);

    // DOCTYPE宣言、ネームスペース出力
    StrCat(buffer, NNSISETFILE_DTD_URI NNSISETFILE_NNSI_NS_B);

    // ルートエレメント出力開始
    StrCat (buffer, NNSISETFILE_NNSISET_B);
    NUMCATI(buffer, DBVERSION_XML_SETTING);
    StrCat (buffer, "\">\n");

    ptr = NNsiItem;
    while (ptr->itemName[0] != '\0')
    {
        // 各アイテムの出力
        StrCat (buffer, "    ");
        StrCat (buffer, NNSISETFILE_ITEM_BEGIN);
        StrCat (buffer, ptr->itemName);
        StrCat (buffer, NNSISETFILE_ATTR_TYPE);
        switch (ptr->itemType)
        {
          case NNSISETFILE_TYPE_CHAR:
            // 文字列の出力
            StrCat (buffer, "\"" NNSISETFILE_ITEMTYPE_CHAR "[");
            NUMCATI(buffer, ptr->length);
            StrCat (buffer, "]\">\n      " NNSISETFILE_VALTAG_BEGIN);
            StrCat (buffer, "\"");
            StrCat (buffer, (Char *) ptr->value);
            StrCat (buffer, "\"");
            break;
          case NNSISETFILE_TYPE_UINT8:
            StrCat (buffer, NNSISETFILE_ITEMTYPE_UINT8 ">\n      " NNSISETFILE_VALTAG_BEGIN);
            temp = *((UInt8 *) ptr->value); 
            NUMCATI(buffer, temp);
            break;

          case NNSISETFILE_TYPE_INT16:
            StrCat (buffer, NNSISETFILE_ITEMTYPE_INT16 ">\n      " NNSISETFILE_VALTAG_BEGIN);
            temp = *((Int16 *) ptr->value); 
            NUMCATI(buffer, temp);
            break;

          case NNSISETFILE_TYPE_INT32:
            StrCat (buffer, NNSISETFILE_ITEMTYPE_INT32 ">\n      " NNSISETFILE_VALTAG_BEGIN);
            temp = *((Int32 *) ptr->value); 
            NUMCATI(buffer, temp);
            break;

          case NNSISETFILE_TYPE_UINT32:
            StrCat (buffer, NNSISETFILE_ITEMTYPE_UINT32 ">\n      " NNSISETFILE_VALTAG_BEGIN);
            temp = *((UInt32 *) ptr->value); 
            NUMCATI(buffer, temp);
            break;

          case NNSISETFILE_TYPE_UINT16:
          default:
            StrCat (buffer, NNSISETFILE_ITEMTYPE_UINT16 ">\n      " NNSISETFILE_VALTAG_BEGIN);
            temp = *((UInt16 *) ptr->value); 
            NUMCATI(buffer, temp);
            break;
        }
        StrCat (buffer, NNSISETFILE_VALTAG_END);
        StrCat (buffer, NNSISETFILE_ITEM_END);
        ptr++;
    }

    // ルートエレメント出力終了
    StrCat(buffer, NNSISETFILE_NNSISET_E "\n" NNSISETFILE_NNSI_NS_E);
    mod = StrLen(buffer) % 2;
    if (mod != 0)
    {
        StrCat(buffer, "\n");
    }
    else
    {
        StrCat(buffer, " \n");
    }

    // ＸＭＬ形式のデータを"一括で"出力する。
    ret = AppendFile_NNsh(&fileRefW, StrLen(buffer), buffer, &temp);
    if (ret != errNone)
    {
        // 出力失敗！
        goto FUNC_END;
    }

FUNC_END:
    CloseFile_NNsh(&fileRefW);
    MEMFREE_PTR(buffer);

    Hide_BusyForm(false);
    return (ret);
}

/*=========================================================================*/
/*   Function :  Input_NNsiSet_XML                                         */
/*                                         XML形式のNNsi設定を入力する処理 */
/*=========================================================================*/
Err Input_NNsiSet_XML(Char *fileName)
{
  // NNsi設定の構造体(NNshSavedPref)とこの下のテーブルが同期している必要がある。
  NNsiXMLItems *ptr, NNsiItem[] =
  {
    {SETITEM__handleName,             NNSISETFILE_TYPE_CHAR,   BUFSIZE,         &((NNshGlobal->NNsiParam)->handleName[0])}, 
    {SETITEM__bbs_URL,                NNSISETFILE_TYPE_CHAR,   MAX_URL,         &((NNshGlobal->NNsiParam)->bbs_URL[0])}, 
    {SETITEM__proxyURL,               NNSISETFILE_TYPE_CHAR,   MAX_URL,         &((NNshGlobal->NNsiParam)->proxyURL)}, 
    {SETITEM__netTimeout,             NNSISETFILE_TYPE_INT32,  sizeof(Int32),   &((NNshGlobal->NNsiParam)->netTimeout)}, 
    {SETITEM__getAllThread,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getAllThread)}, 
    {SETITEM__confirmationDisable,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->confirmationDisable)}, 
    {SETITEM__openAutomatic,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->openAutomatic)}, 
    {SETITEM__writeAlwaysSage,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->writeAlwaysSage)}, 
    {SETITEM__useBookmark,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useBookmark)}, 
    {SETITEM__searchCaseless,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->searchCaseless)}, 
    {SETITEM__offChkLaunch,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->offChkLaunch)}, 
    {SETITEM__disableUnderline,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->disableUnderline)}, 
    {SETITEM__debugMessageON,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->debugMessageON)}, 
    {SETITEM__printNofMessage,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->printNofMessage)}, 
    {SETITEM__useNameOneLine,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->useNameOneLine)}, 
    {SETITEM__boldMessageNum,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->boldMessageNum)}, 
    {SETITEM__useVFS,                 NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useVFS)}, 
    {SETITEM__bufferSize,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->bufferSize)}, 
    {SETITEM__partGetSize,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->partGetSize)}, 
    {SETITEM__enablePartGet,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->enablePartGet)}, 
/*
    {SETITEM__lastFrmID,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->lastFrmID)}, 
    {SETITEM__lastBBS,                NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->lastBBS)}, 
    {SETITEM__openMsgIndex,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->openMsgIndex)}, 
*/
    {SETITEM__bookMsgIndex,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->bookMsgIndex)}, 
    {SETITEM__notCursor,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->notCursor)}, 
    {SETITEM__bookMsgNumber,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->bookMsgNumber)}, 
    {SETITEM__openThreadIndex,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->openThreadIndex)}, 
    {SETITEM__useFixedHandle,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useFixedHandle)}, 
    {SETITEM__disconnectNNsiEnd,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->disconnectNNsiEnd)}, 
    {SETITEM__useProxy,               NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useProxy)}, 
    {SETITEM__proxyPort,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->proxyPort)}, 
    {SETITEM__useCookieWrite,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useCookieWrite)}, 
    {SETITEM__copyDelReadOnly,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->copyDelReadOnly)}, 
    {SETITEM__vfsUseCompactFlash,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->vfsUseCompactFlash)}, 
    {SETITEM__vfsOnAutomatic,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->vfsOnAutomatic)}, 
    {SETITEM__currentFont,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->currentFont)}, 
    {SETITEM__notListReadOnlyNew,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notListReadOnlyNew)}, 
    {SETITEM__notListReadOnly,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notListReadOnly)}, 
    {SETITEM__redrawAfterConnect,     NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->redrawAfterConnect)}, 
    {SETITEM__insertReplyNum,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->insertReplyNum)}, 
    {SETITEM__useARMlet,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useARMlet)}, 
    {SETITEM__sonyHRFont,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->sonyHRFont)}, 
    {SETITEM__sonyHRFontTitle,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->sonyHRFontTitle)}, 
    {SETITEM__useSonyTinyFont,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useSonyTinyFont)}, 
    {SETITEM__useSonyTinyFontTitle,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useSonyTinyFontTitle)}, 
    {SETITEM__notOpenSilkWrite,       NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notOpenSilkWrite)}, 
    {SETITEM__notAutoFocus,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notAutoFocus)}, 
    {SETITEM__writeBufferSize,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->writeBufferSize)}, 
    {SETITEM__browseMesNum,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->browseMesNum)}, 
    {SETITEM__startMacroArrivalEnd,   NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->startMacroArrivalEnd)}, 
    {SETITEM__disconnArrivalEnd,      NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->disconnArrivalEnd)}, 
    {SETITEM__autoBeep,               NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->autoBeep)}, 
    {SETITEM__autoOpenNotRead,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->autoOpenNotRead)}, 
    {SETITEM__addReturnToList,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addReturnToList)}, 
    {SETITEM__jogBackBtnFeature,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.jogBack)}, 
    {SETITEM__notUseSilk,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->notUseSilk)}, 
    {SETITEM__writeSequence2,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->writeSequence2)}, 
    {SETITEM__startTitleRec,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->startTitleRec)}, 
    {SETITEM__endTitleRec,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->endTitleRec)}, 
    {SETITEM__currentSelectRec,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->currentSelectRec)}, 
    {SETITEM__selectedTitleItem,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->selectedTitleItem)}, 
    {SETITEM__titleDispState,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->titleDispState)}, 
    {SETITEM__jogPushBtnFeature,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.jogPush)}, 
    {SETITEM__convertHanZen,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->convertHanZen)},
    {SETITEM__addLineDisconn,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addLineDisconn)}, 
    {SETITEM__addLineGetPart,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addLineGetPart)}, 
    {SETITEM__addJumpTopMsg,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addJumpTopMsg)}, 
    {SETITEM__addJumpBottomMsg,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addJumpBottomMsg)}, 
    {SETITEM__addMenuSelAndWeb,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuSelAndWeb)}, 
    {SETITEM__addMenuMsg,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuMsg)}, 
    {SETITEM__addMenuBackRtnMsg,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuBackRtnMsg)}, 
    {SETITEM__addMenuGraphView,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuGraphView)}, 
    {SETITEM__writingReplyMsg,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->writingReplyMsg)}, 
    {SETITEM__addMenuFavorite,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addMenuFavorite)}, 
    {SETITEM__addMenuGetThreadNum,    NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addMenuGetThreadNum)}, 
    {SETITEM__addMenuOutputMemo,      NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addMenuOutputMemo)}, 
    {SETITEM__addBtOnOff,             NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addBtOnOff)}, 
    {SETITEM__addMenuTitle,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addMenuTitle)}, 
    {SETITEM__addMenuDeleteMsg,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuDeleteMsg)}, 
    {SETITEM__addMenuCopyMsg,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuCopyMsg)}, 
    {SETITEM__addMenuGetMode,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuGetMode)}, 
    {SETITEM__addMenuGraphTitle,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuGraphTitle)}, 
    {SETITEM__addMenuMultiSW1,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuMultiSW1)}, 
    {SETITEM__addMenuMultiSW2,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuMultiSW2)}, 
    {SETITEM__addMenuDirSelect,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuDirSelect)},
    {SETITEM__addMenuNNsiEnd,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuNNsiEnd)}, 
    {SETITEM__disableSonyHR,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->disableSonyHR)}, 
    {SETITEM__useColor,               NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useColor)}, 
    {SETITEM__colorError,             NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorError)}, 
    {SETITEM__colorOver,              NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorOver)}, 
    {SETITEM__colorNotYet,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorNotYet)}, 
    {SETITEM__colorNew,               NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorNew)}, 
    {SETITEM__colorUpdate,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorUpdate)}, 
    {SETITEM__colorRemain,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorRemain)}, 
    {SETITEM__colorAlready,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorAlready)}, 
    {SETITEM__colorUnknown,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorUnknown)}, 
    {SETITEM__colorBackGround,        NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorBackGround)}, 
    {SETITEM__colorButton,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorButton)}, 
    {SETITEM__colorViewFG,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorViewFG)}, 
    {SETITEM__colorViewBG,            NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorViewBG)},
    {SETITEM__rawDisplayMode,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->rawDisplayMode)}, 
    {SETITEM__notDelOffline,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notDelOffline)}, 
    {SETITEM__sortGoMenuAnchor,       NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->sortGoMenuAnchor)}, 
    {SETITEM__notFocusNumField,       NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notFocusNumField)}, 
    {SETITEM__disableUnderlineWrite,  NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->disableUnderlineWrite)}, 
    {SETITEM__usageOfTitleMultiSwitch1,NNSISETFILE_TYPE_UINT16,sizeof(UInt16),  &((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch1)}, 
    {SETITEM__usageOfTitleMultiSwitch2,NNSISETFILE_TYPE_UINT16,sizeof(UInt16),  &((NNshGlobal->NNsiParam)->usageOfTitleMultiSwitch2)}, 
    {SETITEM__autoDeleteNotYet,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->autoDeleteNotYet)}, 
    {SETITEM__notCheckBBSURL,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->notCheckBBSURL)}, 
    {SETITEM__displayFavorLevel,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->displayFavorLevel)}, 
    {SETITEM__newArrivalNotRead,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->newArrivalNotRead)}, 
    {SETITEM__getReserveFeature,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getReserveFeature)}, 
    {SETITEM__searchStrMessage,       NNSISETFILE_TYPE_CHAR, MAX_SEARCH_STRING, &((NNshGlobal->NNsiParam)->searchStrMessage[0])}, 
    {SETITEM__searchMode,             NNSISETFILE_TYPE_UINT32, sizeof(UInt32),  &((NNshGlobal->NNsiParam)->searchMode)}, 
    {SETITEM__bbsOverwrite,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->bbsOverwrite)}, 
    {SETITEM__useImodeURL,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useImodeURL)}, 
    {SETITEM__nofRetry,               NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->nofRetry)}, 
    {SETITEM__autoUpdateGetError,     NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->autoUpdateGetError)}, 
    {SETITEM__dispBottom,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->dispBottom)}, 
    {SETITEM__blockDispMode,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->blockDispMode)}, 
    {SETITEM__multiBtn1Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn1Feature)}, 
    {SETITEM__multiBtn1Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn1Caption[0])}, 
    {SETITEM__multiBtn2Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn2Feature)}, 
    {SETITEM__multiBtn2Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn2Caption[0])}, 
    {SETITEM__multiBtn3Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn3Feature)}, 
    {SETITEM__multiBtn3Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn3Caption[0])}, 
    {SETITEM__multiBtn4Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn4Feature)}, 
    {SETITEM__multiBtn4Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn4Caption[0])}, 
    {SETITEM__multiBtn5Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn5Feature)}, 
    {SETITEM__multiBtn5Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn5Caption[0])}, 
    {SETITEM__multiBtn6Feature,       NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->multiBtn6Feature)}, 
    {SETITEM__multiBtn6Caption,       NNSISETFILE_TYPE_CHAR,   MAX_CAPTION,     &((NNshGlobal->NNsiParam)->multiBtn6Caption[0])}, 
    {SETITEM__multiBtnUpFeature,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.up)},  
    {SETITEM__multiBtnDownFeature,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.down)},  
    {SETITEM__multiBtnHKey1Feature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.key1)},  
    {SETITEM__multiBtnHKey2Feature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.key2)},  
    {SETITEM__multiBtnHKey3Feature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.key3)},  
    {SETITEM__multiBtnHKey4Feature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.key4)},  
    {SETITEM__multiBtnCapture,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.clieCapture)},  
    {SETITEM__multiBtnLeftFeature,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.left)},  
    {SETITEM__multiBtnRightFeature,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->ttlFtr.right)},  
    {SETITEM__useHardKeyControl,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useHardKeyControl)},
    {SETITEM__checkDuplicateThread,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->checkDuplicateThread)}, 
    {SETITEM__hideMessage,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->hideMessage)}, 
    {SETITEM__hideWord1,              NNSISETFILE_TYPE_CHAR,   HIDEBUFSIZE,     &((NNshGlobal->NNsiParam)->hideWord1[0])}, 
    {SETITEM__hideWord2,              NNSISETFILE_TYPE_CHAR,   HIDEBUFSIZE,     &((NNshGlobal->NNsiParam)->hideWord2[0])}, 
    {SETITEM__CS1tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,   &((NNshGlobal->NNsiParam)->custom1.tabName[0])}, 
    {SETITEM__CS1newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.newArrival)}, 
    {SETITEM__CS1getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.getList)}, 
    {SETITEM__CS1condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.condition)}, 
    {SETITEM__CS1boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.boardNick)}, 
    {SETITEM__CS1boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.boardCondition)}, 
    {SETITEM__CS1threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.threadLevel)}, 
    {SETITEM__CS1threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.threadStatus)}, 
    {SETITEM__CS1stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom1.stringSet)}, 
    {SETITEM__CS1string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,         &((NNshGlobal->NNsiParam)->custom1.string1[0])}, 
    {SETITEM__CS1string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,         &((NNshGlobal->NNsiParam)->custom1.string2[0])}, 
    {SETITEM__CS1string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,         &((NNshGlobal->NNsiParam)->custom1.string3[0])}, 
    {SETITEM__CS2tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,   &((NNshGlobal->NNsiParam)->custom2.tabName[0])}, 
    {SETITEM__CS2newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.newArrival)}, 
    {SETITEM__CS2getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.getList)}, 
    {SETITEM__CS2condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.condition)}, 
    {SETITEM__CS2boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.boardNick)}, 
    {SETITEM__CS2boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.boardCondition)}, 
    {SETITEM__CS2threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.threadLevel)}, 
    {SETITEM__CS2threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.threadStatus)}, 
    {SETITEM__CS2stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom2.stringSet)}, 
    {SETITEM__CS2string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,         &((NNshGlobal->NNsiParam)->custom2.string1[0])}, 
    {SETITEM__CS2string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,         &((NNshGlobal->NNsiParam)->custom2.string2[0])}, 
    {SETITEM__CS2string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,         &((NNshGlobal->NNsiParam)->custom2.string3[0])}, 
    {SETITEM__CS3tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,    &((NNshGlobal->NNsiParam)->custom3.tabName[0])}, 
    {SETITEM__CS3newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.newArrival)}, 
    {SETITEM__CS3getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.getList)}, 
    {SETITEM__CS3condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.condition)}, 
    {SETITEM__CS3boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.boardNick)}, 
    {SETITEM__CS3boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.boardCondition)}, 
    {SETITEM__CS3threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.threadLevel)}, 
    {SETITEM__CS3threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.threadStatus)},
    {SETITEM__CS3threadCreate,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.threadCreate)}, 
    {SETITEM__CS3stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom3.stringSet)}, 
    {SETITEM__CS3string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom3.string1[0])}, 
    {SETITEM__CS3string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom3.string2[0])}, 
    {SETITEM__CS3string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom3.string3[0])}, 
    {SETITEM__CS4tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,    &((NNshGlobal->NNsiParam)->custom4.tabName[0])}, 
    {SETITEM__CS4newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.newArrival)}, 
    {SETITEM__CS4getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.getList)}, 
    {SETITEM__CS4condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.condition)}, 
    {SETITEM__CS4boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.boardNick)}, 
    {SETITEM__CS4boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.boardCondition)}, 
    {SETITEM__CS4threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.threadLevel)}, 
    {SETITEM__CS4threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.threadStatus)},
    {SETITEM__CS4threadCreate,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.threadCreate)}, 
    {SETITEM__CS4stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom4.stringSet)}, 
    {SETITEM__CS4string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom4.string1[0])}, 
    {SETITEM__CS4string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom4.string2[0])}, 
    {SETITEM__CS4string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom4.string3[0])}, 
    {SETITEM__CS5tabName,             NNSISETFILE_TYPE_CHAR,   MAX_BOARDNAME,    &((NNshGlobal->NNsiParam)->custom5.tabName[0])}, 
    {SETITEM__CS5newArrival,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.newArrival)}, 
    {SETITEM__CS5getList,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.getList)}, 
    {SETITEM__CS5condition,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.condition)}, 
    {SETITEM__CS5boardNick,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.boardNick)}, 
    {SETITEM__CS5boardCondition,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.boardCondition)}, 
    {SETITEM__CS5threadLevel,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.threadLevel)}, 
    {SETITEM__CS5threadStatus,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.threadStatus)},
    {SETITEM__CS5threadCreate,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.threadCreate)}, 
    {SETITEM__CS5stringSet,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->custom5.stringSet)}, 
    {SETITEM__CS5string1,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom5.string1[0])}, 
    {SETITEM__CS5string2,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom5.string2[0])}, 
    {SETITEM__CS5string3,             NNSISETFILE_TYPE_CHAR,   MINIBUF,          &((NNshGlobal->NNsiParam)->custom5.string3[0])}, 
    {SETITEM__listMesNumIsNotReadNum, NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum)}, 
    {SETITEM__writeJogPushDisable,    NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->writeJogPushDisable)}, 
    {SETITEM__autostartMacro      ,   NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->autostartMacro)}, 
    {SETITEM__enableNewArrivalHtml,   NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->enableNewArrivalHtml)}, 
    {SETITEM__useRegularExpression,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useRegularExpression)}, 
    {SETITEM__vfsOnNotDBCheck,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->vfsOnNotDBCheck)}, 
    {SETITEM__writeMessageAutoSave,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->writeMessageAutoSave)}, 
    {SETITEM__useHardKey1,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.key1)},  
    {SETITEM__useHardKey2,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.key2)},  
    {SETITEM__useHardKey3,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.key3)},  
    {SETITEM__useHardKey4,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.key4)},  
    {SETITEM__useClieCapture,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.clieCapture)},  
    {SETITEM__useLeftBtn,             NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.left)},  
    {SETITEM__useRightBtn,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useKey.right)},  
    {SETITEM__addMenuDeviceInfo,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->addMenuDeviceInfo)},
    {SETITEM__addNgSetting3,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addNgSetting3)},
    {SETITEM__addDeviceInfo,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->addDeviceInfo)},
    {SETITEM__useViewHardKey1,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.key1)},
    {SETITEM__useViewHardKey2,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.key2)},
    {SETITEM__useViewHardKey3,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.key3)},
    {SETITEM__useViewHardKey4,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.key4)},
    {SETITEM__useViewClieCapture,     NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.clieCapture)},
    {SETITEM__useViewUp,              NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.up)},
    {SETITEM__useViewDown,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.down)},
    {SETITEM__useViewLeft,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.left)},
    {SETITEM__useViewRight,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.right)},
    {SETITEM__useViewJogPush,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.jogPush)}, 
    {SETITEM__useViewJogBack,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewFtr.jogBack)},
    {SETITEM__readOnlySelection,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->readOnlySelection)},
    {SETITEM__use_DAplugin,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->use_DAplugin)},
    {SETITEM__listAndUpdate,          NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->listAndUpdate)},
    {SETITEM__msgNumLimit,            NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->msgNumLimit)},
    {SETITEM__notUseTsPatch,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->notUseTsPatch)},
    {SETITEM__getROLogLevel,          NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->getROLogLevel)},
    {SETITEM__viewMultiBtnFeature,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewMultiBtnFeature)},
    {SETITEM__viewSearchStrHeader,    NNSISETFILE_TYPE_CHAR,   SEARCH_HEADLEN,   &((NNshGlobal->NNsiParam)->viewSearchStrHeader[0])},
    {SETITEM__enableSearchStrHeader,  NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->enableSearchStrHeader)},
    {SETITEM__viewTitleSelFeature,    NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->viewTitleSelFeature)},
    {SETITEM__oysterUserId,           NNSISETFILE_TYPE_CHAR,   OYSTERID_LEN,     &((NNshGlobal->NNsiParam)->oysterUserId[0])}, 
    {SETITEM__messageSeparator,       NNSISETFILE_TYPE_CHAR,   SEPARATOR_LEN,    &((NNshGlobal->NNsiParam)->messageSeparator[0])}, 
    {SETITEM__colorMsgHeader,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgHeader)},
    {SETITEM__colorMsgHeaderBold,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgHeaderBold)},
    {SETITEM__colorMsgFooter,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgFooter)},
    {SETITEM__colorMsgFooterBold,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgFooterBold)},
    {SETITEM__colorMsgInform,         NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgInform)},
    {SETITEM__colorMsgInformBold,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->colorMsgInformBold)},
    {SETITEM__useColorMessageToken,   NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->useColorMessageToken)},
    {SETITEM__useColorMessageBold,    NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->useColorMessageBold)},
    {SETITEM__getLogFeatureJogPush,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.jogPush)},
    {SETITEM__getLogFeatureJogBack,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.jogBack)},
    {SETITEM__getLogFeatureJogLeft,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.left)},
    {SETITEM__getLogFeatureJogRight,  NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.right)},
    {SETITEM__getLogFeatureUp,        NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.up)},
    {SETITEM__getLogFeatureDown,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.down)},
    {SETITEM__getLogFeatureCapture,   NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.clieCapture)},
    {SETITEM__getLogFeatureKey1,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.key1)},
    {SETITEM__getLogFeatureKey2,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.key2)},
    {SETITEM__getLogFeatureKey3,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.key3)},
    {SETITEM__getLogFeatureKey4,      NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->getLogFeature.key4)},
    {SETITEM__prepareDAforNewArrival, NNSISETFILE_TYPE_UINT32, sizeof(UInt32),  &((NNshGlobal->NNsiParam)->prepareDAforNewArrival)},
    {SETITEM__preOnDAnewArrival,      NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->preOnDAnewArrival)},
    {SETITEM__showResPopup,           NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->showResPopup)},
    {SETITEM__useGZIP,                NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->useGZIP)},
    {SETITEM__vfsOnDefault,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->vfsOnDefault)},
    {SETITEM__useBe2chInfo,           NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->useBe2chInfo)},
    {SETITEM__be2chId,                NNSISETFILE_TYPE_CHAR,   BE2chID_LEN,      &((NNshGlobal->NNsiParam)->be2chId[0])}, 
    {SETITEM__be2chPw,                NNSISETFILE_TYPE_CHAR,   BE2chPW_LEN,      &((NNshGlobal->NNsiParam)->be2chPw[0])}, 
    {SETITEM__roundTripDelay,         NNSISETFILE_TYPE_UINT16, sizeof(UInt16),  &((NNshGlobal->NNsiParam)->roundTripDelay)},
    {SETITEM__searchPickupMode,       NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->searchPickupMode)},
    {SETITEM__getLogSiteListMode,     NNSISETFILE_TYPE_UINT8,  sizeof(UInt8),   &((NNshGlobal->NNsiParam)->getLogSiteListMode)},
    {"", 0, 0, 0}
  };
  Err          ret;
  Int32        mod;
  UInt32       temp, fileSize;
  UInt16       len;
  Char        *buffer, *tempBuf, *area, *start, *end, *loc;
  NNshFileRef  fileRefR;

  // 領域の作成
  len    = BIGBUF + MARGIN;
  tempBuf = MEMALLOC_PTR(len);
  if (tempBuf == NULL)
  {
        // 領域確保失敗
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", len);
        return (~errNone);
  }
  MemSet(tempBuf, len, 0x00);

  // 読み込みファイルのオープン
  ret = OpenFile_NNsh(fileName, (NNSH_FILEMODE_READONLY), &fileRefR);
  if (ret != errNone)
  {
      NNsh_InformMessage(ALTID_ERROR, MSG_READFILE_OPENFAIL, fileName, ret);
      MEMFREE_PTR(tempBuf);
      return (~errNone);
  }

  // 読み込むファイルサイズの確認
  ret = GetFileSize_NNsh(&fileRefR, &fileSize);
  if (ret != errNone)
  {
      NNsh_InformMessage(ALTID_ERROR, MSG_READFILE_OPENFAIL, fileName, ret);
      MEMFREE_PTR(tempBuf);
      return (~errNone);
  }

  // 読み込むファイルサイズにあわせて領域を確保する。
  buffer = MEMALLOC_PTR(fileSize + MARGIN);
  if (buffer == NULL)
  { 
        // 領域確保失敗
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL, " size:", fileSize + MARGIN);
        CloseFile_NNsh(&fileRefR);
        MEMFREE_PTR(tempBuf);
        return (~errNone);
  }
  MemSet(buffer, (fileSize + MARGIN), 0x00);

  // ファイル出力中の表示
  Show_BusyForm(MSG_INPUT_XML_WAIT);

  // ファイルを一括で読み出す
  ret = ReadFile_NNsh(&fileRefR, 0, fileSize, buffer, &temp);
  CloseFile_NNsh(&fileRefR);
  if (ret != errNone)
  {
      NNsh_InformMessage(ALTID_ERROR, MSG_NNSISET_READFAIL, "", ret);
      MEMFREE_PTR(buffer);
      MEMFREE_PTR(tempBuf);
      Hide_BusyForm(false);
      return (~errNone);
  }

  // 要素を探してループする
  ptr = NNsiItem;
  while (ptr->itemName[0] != '\0')
  {
      MemSet (tempBuf, len, 0x00);
      StrCopy(tempBuf, NNSISETFILE_ITEM_BEGIN);
      StrCat (tempBuf, ptr->itemName);
      area = StrStr(buffer, tempBuf);
      if (area != NULL)
      {
          // アイテム発見！
          area = area + StrLen(tempBuf);
          start = StrStr(area, NNSISETFILE_VALTAG_BEGIN);
          if (start != NULL)
          {
              start = start + StrLen(NNSISETFILE_VALTAG_BEGIN);
              end   = StrStr(start, NNSISETFILE_VALTAG_END);
              if (end != NULL)
              {
                  // データ部分をファイルバッファからひっぱってくる
                  loc = tempBuf;
                  for (area  = start, loc = tempBuf; area != end; area++)
                  {
                      *loc = *area;
                      loc++;
                  }
                  *loc = '\0';

                  // データの格納型にあわせてデータを構造体メンバにコピーする 
                  switch (ptr->itemType)
                  {
                      case NNSISETFILE_TYPE_CHAR:
                        while (*start != '\"')
                        {
                            start++;
                        }
                        start++;
                        loc = (Char *) ptr->value;
                        MemSet  (loc, ptr->length, 0x00);
                        StrNCopy(loc, start, (end - start));
                        end = loc + StrLen(loc);
                        while ((end > (Char *) ptr->value)&&(*end != '\"'))
                        {
                            end--;
                        }
                        *end = '\0';
                        break;

                      case NNSISETFILE_TYPE_UINT8:
                        while ((*start < '0')||(*start > '9'))
                        {
                            start++;
                        }
                        mod = StrAToI(start);
                        *((UInt8 *) ptr->value) = (UInt8) mod;
                        break;

                      case NNSISETFILE_TYPE_INT16:
                        while ((*start < '0')||(*start > '9'))
                        {
                            start++;
                        }
                        mod = StrAToI(start);
                        *((Int16 *) ptr->value) = (Int16) mod;
                        break;

                      case NNSISETFILE_TYPE_INT32:
                        while ((*start < '0')||(*start > '9'))
                        {
                            start++;
                        }
                        mod = StrAToI(start);
                        *((Int32 *) ptr->value) = (Int32) mod;
                        break;

                      case NNSISETFILE_TYPE_UINT32:
                        while ((*start < '0')||(*start > '9'))
                        {
                            start++;
                        }
                        mod = StrAToI(start);
                        *((UInt32 *) ptr->value) = (UInt32) mod;
                        break;

                      case NNSISETFILE_TYPE_UINT16:
                      default:
                        while ((*start < '0')||(*start > '9'))
                        {
                            start++;
                        }
                        mod = StrAToI(start);
                        *((UInt16 *) ptr->value) = (UInt16) mod;
                        break;
                  }
              }  
          }
      }
      ptr++;
  }
  Hide_BusyForm(false);

  return (ret);
}
#endif // #ifdef USE_XML_OUTPUT
