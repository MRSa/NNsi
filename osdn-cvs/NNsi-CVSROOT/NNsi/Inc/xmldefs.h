/*============================================================================*
 *
 * $Id: xmldefs.h,v 1.45 2007/08/13 14:50:12 mrsa Exp $
 *
 *  FILE: 
 *     xmlDefs.h
 *
 *  Description: 
 *     NNsi設定の設定情報出力用定義
 *
 *===========================================================================*/
#ifdef NNSI_XMLDEFS_H
    /** 何もしない **/
#else

#define NNSISETFILE_HEAD      "<?xml version=\"1.0\" encoding=\"Shift_JIS\" ?>\n"
#define NNSISETFILE_DTD_URI   "<!DOCTYPE NNsi:NNsiXML SYSTEM \"http://nnsi.sourceforge.jp/NNsiXML/NNsiXML100.dtd\">\n"
#define NNSISETFILE_NNSI_NS_B "<NNsi:NNsiXML xmlns:NNsi=\"http://nnsi.sourceforge.jp/NNsiXML\">\n"
#define NNSISETFILE_NNSI_NS_E "</NNsi:NNsiXML>"
#define NNSISETFILE_NNSISET_B "  <NNsi:NNsiSet ver=\""
#define NNSISETFILE_NNSISET_E "  </NNsi:NNsiSet>"

#define NNSISETFILE_TAGBEGIN "<NNsi:"
#define NNSISETFILE_TAGEND   "</NNsi:"
#define NNSISETFILE_TAGCLOSE ">"
 
#define NNSISETFILE_ITEM_BEGIN    "<NNsi:item name="
#define NNSISETFILE_ATTR_TYPE     " type="
#define NNSISETFILE_ITEM_END      "</NNsi:item>\n"

#define NNSISETFILE_VALTAG_BEGIN  "<NNsi:val> "
#define NNSISETFILE_VALTAG_END    " </NNsi:val>\n    "

#define NNSISETFILE_ITEMTYPE_UINT32   "\"UInt32\""
#define NNSISETFILE_ITEMTYPE_UINT16   "\"UInt16\""
#define NNSISETFILE_ITEMTYPE_UINT8    "\"UInt8\""
#define NNSISETFILE_ITEMTYPE_INT16    "\"Int16\""
#define NNSISETFILE_ITEMTYPE_INT32    "\"Int32\""
#define NNSISETFILE_ITEMTYPE_CHAR     "Char"

#define NNSISETFILE_TYPE_CHAR     0
#define NNSISETFILE_TYPE_UINT8    1
#define NNSISETFILE_TYPE_INT16    2
#define NNSISETFILE_TYPE_UINT16   3
#define NNSISETFILE_TYPE_INT32    4
#define NNSISETFILE_TYPE_UINT32   5

 /** NNsi設定 **/
#define SETITEM__handleName          "\"hN\""
#define SETITEM__bbs_URL             "\"bURL\""
#define SETITEM__proxyURL            "\"pURL\""
#define SETITEM__netTimeout          "\"nTo\""
#define SETITEM__getAllThread        "\"gAT\""
#define SETITEM__confirmationDisable "\"cD\""
#define SETITEM__openAutomatic       "\"oA\""
#define SETITEM__writeAlwaysSage     "\"wAS\""
#define SETITEM__useBookmark         "\"uBm\""
#define SETITEM__searchCaseless      "\"sCl\""
#define SETITEM__offChkLaunch        "\"oCL\""
#define SETITEM__disableUnderline    "\"dUl\""
#define SETITEM__debugMessageON      "\"dMON\""
#define SETITEM__printNofMessage     "\"pNM\""
#define SETITEM__useNameOneLine      "\"uNOL\""
#define SETITEM__boldMessageNum      "\"boldMN\""
#define SETITEM__useVFS              "\"uVFS\""
#define SETITEM__bufferSize          "\"bS\""
#define SETITEM__partGetSize         "\"pGS\""
#define SETITEM__enablePartGet       "\"ePG\""
#define SETITEM__lastFrmID           "\"lFID\""
#define SETITEM__lastBBS             "\"lBBS\""
#define SETITEM__openMsgIndex        "\"oMI\""
#define SETITEM__bookMsgIndex        "\"bMI\""
#define SETITEM__notCursor           "\"nC\""
#define SETITEM__bookMsgNumber       "\"bookMN\""
#define SETITEM__openThreadIndex     "\"oTI\""
#define SETITEM__useFixedHandle      "\"uFH\""
#define SETITEM__disconnectNNsiEnd   "\"dNE\""
#define SETITEM__useProxy            "\"uP\""
#define SETITEM__proxyPort           "\"pP\""
#define SETITEM__useCookieWrite      "\"uCW\""
#define SETITEM__copyDelReadOnly     "\"cDRO\""
#define SETITEM__vfsUseCompactFlash  "\"vUsCF\""
#define SETITEM__vfsOnAutomatic      "\"vOA\""
#define SETITEM__currentFont         "\"cF\""
#define SETITEM__notListReadOnlyNew  "\"nLRON\""
#define SETITEM__notListReadOnly     "\"nLRO\""
#define SETITEM__redrawAfterConnect  "\"rAC\""
#define SETITEM__insertReplyNum      "\"iRN\""
#define SETITEM__useARMlet           "\"uAl\""
#define SETITEM__sonyHRFont          "\"sHF\""
#define SETITEM__sonyHRFontTitle     "\"sHFT\""
#define SETITEM__useSonyTinyFont     "\"uSTF\""
#define SETITEM__useSonyTinyFontTitle "\"uSTFT\""
#define SETITEM__notOpenSilkWrite     "\"nOSWr\""
#define SETITEM__notAutoFocus         "\"nAF\""
#define SETITEM__writeBufferSize      "\"wBS\""
#define SETITEM__browseMesNum         "\"bMesNum\""
#define SETITEM__startMacroArrivalEnd "\"sMAE\""
#define SETITEM__disconnArrivalEnd    "\"dAE\""
#define SETITEM__autoBeep             "\"aBeep\""
#define SETITEM__autoOpenNotRead      "\"aONR\""
#define SETITEM__jogBackBtnAsGo       "\"jBBAG\""
#define SETITEM__addReturnToList      "\"aRTL\""
#define SETITEM__jogBackBtnFeature    "\"jBBF\""
#define SETITEM__notUseSilk           "\"nUS\""
#define SETITEM__writeSequence2       "\"wS2\""
#define SETITEM__startTitleRec        "\"sTR\""
#define SETITEM__endTitleRec          "\"eTR\""
#define SETITEM__currentSelectRec     "\"cSR\""
#define SETITEM__selectedTitleItem    "\"sTI\""
#define SETITEM__titleDispState       "\"tDS\""
#define SETITEM__jogPushBtnAsGo       "\"jPBAG\""
#define SETITEM__jogPushBtnFeature    "\"jPBF\""
#define SETITEM__convertHanZen        "\"cHZ\""
#define SETITEM__addLineDisconn       "\"aLD\""
#define SETITEM__addLineGetPart       "\"aLGP\""
#define SETITEM__addJumpTopMsg        "\"aJTM\""
#define SETITEM__addJumpBottomMsg     "\"aJBM\""
#define SETITEM__addMenuSelAndWeb     "\"aMSAW\""
#define SETITEM__addMenuMsg           "\"aMM\""
#define SETITEM__addMenuBackRtnMsg    "\"aMBRM\""
#define SETITEM__addMenuGraphView     "\"aMGV\""
#define SETITEM__addMenuFavorite      "\"aMF\""
#define SETITEM__addMenuGetThreadNum  "\"aMGTN\""
#define SETITEM__addMenuOutputMemo    "\"aMOM\""
#define SETITEM__addBtOnOff           "\"aMBtOO\""
#define SETITEM__addMenuTitle         "\"aMT\""
#define SETITEM__addMenuDeleteMsg     "\"aMDM\""
#define SETITEM__addMenuCopyMsg       "\"aMCM\""
#define SETITEM__addMenuGetMode       "\"aMGM\""
#define SETITEM__addMenuGraphTitle    "\"aMGT\""
#define SETITEM__addMenuMultiSW1        "\"aMMS1\""
#define SETITEM__addMenuMultiSW2        "\"aMMS2\""
#define SETITEM__addMenuNNsiEnd         "\"aMNE\""
#define SETITEM__disableSonyHR          "\"dSH\""
#define SETITEM__useColor               "\"uColor\""
#define SETITEM__colorError             "\"colorE\""
#define SETITEM__colorOver              "\"colorO\""
#define SETITEM__colorNotYet            "\"colorNY\""
#define SETITEM__colorNew               "\"colorN\""
#define SETITEM__colorUpdate            "\"colorUp\""
#define SETITEM__colorRemain            "\"colorR\""
#define SETITEM__colorAlready           "\"colorA\""
#define SETITEM__colorUnknown           "\"colorUn\""
#define SETITEM__colorBackGround        "\"colorBG\""
#define SETITEM__colorButton            "\"colorBtn\""
#define SETITEM__colorViewFG            "\"colorVFG\""
#define SETITEM__colorViewBG            "\"colorVBG\""
#define SETITEM__rawDisplayMode         "\"rwDspMd\""
#define SETITEM__notDelOffline          "\"nDO\""
#define SETITEM__sortGoMenuAnchor       "\"sGMAn"
#define SETITEM__notFocusNumField       "\"nFNF\""
#define SETITEM__disableUnderlineWrite    "\"dUW\""
#define SETITEM__usageOfTitleMultiSwitch1 "\"uOTMS1\""
#define SETITEM__usageOfTitleMultiSwitch2 "\"uOTMS2\""
#define SETITEM__autoDeleteNotYet       "\"aDNY\""
#define SETITEM__notCheckBBSURL         "\"nCBBSURL\""
#define SETITEM__displayFavorLevel      "\"dFL\""
#define SETITEM__newArrivalNotRead      "\"nANR\""
#define SETITEM__getReserveFeature      "\"gRF\""
#define SETITEM__searchStrMessage       "\"sSM\""
#define SETITEM__searchMode             "\"sM\""
#define SETITEM__bbsOverwrite           "\"bOw\""
#define SETITEM__useImodeURL            "\"uIURL\""
#define SETITEM__nofRetry               "\"nR\""
#define SETITEM__autoUpdateGetError     "\"aUGE\""
#define SETITEM__dispBottom             "\"dB\""
#define SETITEM__blockDispMode          "\"bDM\""
#define SETITEM__multiBtn1Feature       "\"mB1F\""
#define SETITEM__multiBtn1Caption       "\"mB1C\""
#define SETITEM__multiBtn2Feature       "\"mB2F\""
#define SETITEM__multiBtn2Caption       "\"mB2C\""
#define SETITEM__multiBtn3Feature       "\"mB3F\""
#define SETITEM__multiBtn3Caption       "\"mB3C\""
#define SETITEM__multiBtn4Feature       "\"mB4F\""
#define SETITEM__multiBtn4Caption       "\"mB4C\""
#define SETITEM__multiBtn5Feature       "\"mB5F\""
#define SETITEM__multiBtn5Caption       "\"mB5C\""
#define SETITEM__multiBtn6Feature       "\"mB6F\""
#define SETITEM__multiBtn6Caption       "\"mB6C\""
#define SETITEM__multiBtnUpFeature      "\"mBUF\""
#define SETITEM__multiBtnDownFeature    "\"mBDF\""
#define SETITEM__multiBtnHKey1Feature   "\"mBHK1F\""
#define SETITEM__multiBtnHKey2Feature   "\"mBHK2F\""
#define SETITEM__multiBtnHKey3Feature   "\"mBHK3F\""
#define SETITEM__multiBtnHKey4Feature   "\"mBHK4F\""
#define SETITEM__multiBtnCapture        "\"mBCCPT\""
#define SETITEM__useHardKeyControl      "\"uHKC\""
#define SETITEM__checkDuplicateThread   "\"cDT\""
#define SETITEM__hideMessage            "\"hM\""
#define SETITEM__hideWord1              "\"hW1\""
#define SETITEM__hideWord2              "\"hW2\""
#define SETITEM__CS1tabName             "\"C1tN\""
#define SETITEM__CS1newArrival          "\"C1nA\""
#define SETITEM__CS1getList             "\"C1gL\""
#define SETITEM__CS1condition           "\"C1c\""
#define SETITEM__CS1boardNick           "\"C1bN\""
#define SETITEM__CS1boardCondition      "\"C1bC\""
#define SETITEM__CS1threadLevel         "\"C1tL\""
#define SETITEM__CS1threadStatus        "\"C1tS\""
#define SETITEM__CS1threadCreate        "\"C1tCre\""
#define SETITEM__CS1stringSet           "\"C1sS\""
#define SETITEM__CS1string1             "\"C1s1\""
#define SETITEM__CS1string2             "\"C1s2\""
#define SETITEM__CS1string3             "\"C1s3\""
#define SETITEM__CS2tabName             "\"C2tN\""
#define SETITEM__CS2newArrival          "\"C2nA\""
#define SETITEM__CS2getList             "\"C2gL\""
#define SETITEM__CS2condition           "\"C2c\""
#define SETITEM__CS2boardNick           "\"C2bN\""
#define SETITEM__CS2boardCondition      "\"C2bC\""
#define SETITEM__CS2threadLevel         "\"C2tL\""
#define SETITEM__CS2threadStatus        "\"C2tS\""
#define SETITEM__CS2threadCreate        "\"C2tCre\""
#define SETITEM__CS2stringSet           "\"C2sS\""
#define SETITEM__CS2string1             "\"C2s1\""
#define SETITEM__CS2string2             "\"C2s2\""
#define SETITEM__CS2string3             "\"C2s3\""
#define SETITEM__CS3tabName             "\"C3tN\""
#define SETITEM__CS3newArrival          "\"C3nA\""
#define SETITEM__CS3getList             "\"C3gL\""
#define SETITEM__CS3condition           "\"C3c\""
#define SETITEM__CS3boardNick           "\"C3bN\""
#define SETITEM__CS3boardCondition      "\"C3bC\""
#define SETITEM__CS3threadLevel         "\"C3tL\""
#define SETITEM__CS3threadStatus        "\"C3tS\""
#define SETITEM__CS3threadCreate        "\"C3tCre\""
#define SETITEM__CS3stringSet           "\"C3sS\""
#define SETITEM__CS3string1             "\"C3s1\""
#define SETITEM__CS3string2             "\"C3s2\""
#define SETITEM__CS3string3             "\"C3s3\""
#define SETITEM__CS4tabName             "\"C4tN\""
#define SETITEM__CS4newArrival          "\"C4nA\""
#define SETITEM__CS4getList             "\"C4gL\""
#define SETITEM__CS4condition           "\"C4c\""
#define SETITEM__CS4boardNick           "\"C4bN\""
#define SETITEM__CS4boardCondition      "\"C4bC\""
#define SETITEM__CS4threadLevel         "\"C4tL\""
#define SETITEM__CS4threadStatus        "\"C4tS\""
#define SETITEM__CS4threadCreate        "\"C4tCre\""
#define SETITEM__CS4stringSet           "\"C4sS\""
#define SETITEM__CS4string1             "\"C4s1\""
#define SETITEM__CS4string2             "\"C4s2\""
#define SETITEM__CS4string3             "\"C4s3\""
#define SETITEM__CS5tabName             "\"C5tN\""
#define SETITEM__CS5newArrival          "\"C5nA\""
#define SETITEM__CS5getList             "\"C5gL\""
#define SETITEM__CS5condition           "\"C5c\""
#define SETITEM__CS5boardNick           "\"C5bN\""
#define SETITEM__CS5boardCondition      "\"C5bC\""
#define SETITEM__CS5threadLevel         "\"C5tL\""
#define SETITEM__CS5threadStatus        "\"C5tS\""
#define SETITEM__CS5threadCreate        "\"C5tCre\""
#define SETITEM__CS5stringSet           "\"C5sS\""
#define SETITEM__CS5string1             "\"C5s1\""
#define SETITEM__CS5string2             "\"C5s2\""
#define SETITEM__CS5string3             "\"C5s3\""
#define SETITEM__listMesNumIsNotReadNum "\"lMNINRN\""
#define SETITEM__writeJogPushDisable    "\"wJPD\""
#define SETITEM__autostartMacro         "\"asMCR\""
#define SETITEM__enableNewArrivalHtml   "\"eNAH\""
#define SETITEM__useRegularExpression   "\"uRE\""
#define SETITEM__vfsOnNotDBCheck        "\"vONDC\""
#define SETITEM__writeMessageAutoSave   "\"wMAS\""
#define SETITEM__useHardKey1            "\"uHK1\""
#define SETITEM__useHardKey2            "\"uHK2\""
#define SETITEM__useHardKey3            "\"uHK3\""
#define SETITEM__useHardKey4            "\"uHK4\""
#define SETITEM__useClieCapture         "\"uCCap\""
#define SETITEM__addMenuDeviceInfo      "\"aMDInf\""
#define SETITEM__addNgSetting3          "\"aNg3\""
#define SETITEM__addDeviceInfo          "\"aDInf\""
#define SETITEM__useViewHardKey1        "\"uVwHK1\""
#define SETITEM__useViewHardKey2        "\"uVwHK2\""
#define SETITEM__useViewHardKey3        "\"uVwHK3\""
#define SETITEM__useViewHardKey4        "\"uVwHK4\""
#define SETITEM__useViewClieCapture     "\"uVwCCap\""
#define SETITEM__useViewUp              "\"uVwUp\""
#define SETITEM__useViewDown            "\"uVwDwn\""
#define SETITEM__useViewJogPush         "\"uVwJgPsh\""
#define SETITEM__useViewJogBack         "\"uVwJgBck\""
#define SETITEM__readOnlySelection      "\"rOSlctn\""
#define SETITEM__addMenuDirSelect       "\"aMDirSlct\""
#define SETITEM__use_DAplugin           "\"useDAplg\""
#define SETITEM__listAndUpdate          "\"lstAUpdt\""
#define SETITEM__msgNumLimit            "\"mNLmt\""
#define SETITEM__notUseTsPatch          "\"nUTsPch\""
#define SETITEM__getROLogLevel          "\"gROLLv\""
#define SETITEM__viewMultiBtnFeature    "\"vMltBtnFtr\""
#define SETITEM__viewSearchStrHeader    "\"vSeStHdr\""
#define SETITEM__enableSearchStrHeader  "\"eSeStHdr\""
#define SETITEM__viewTitleSelFeature    "\"vTtlSlFtr\""

#define SETITEM__useViewLeft            "\"uVlft\""
#define SETITEM__useViewRight           "\"uVrght\""
#define SETITEM__multiBtnLeftFeature    "\"mBLftFtr\""
#define SETITEM__multiBtnRightFeature   "\"mBrghtFtr\""
#define SETITEM__useLeftBtn             "\"uLftBtn\""
#define SETITEM__useRightBtn            "\"uRghtBtn\""
#define SETITEM__oysterUserId           "\"oysterId\""

#define SETITEM__messageSeparator       "\"mSptr\""
#define SETITEM__colorMsgHeader         "\"colorMHdr\""
#define SETITEM__colorMsgHeaderBold     "\"colorMHdBd\""
#define SETITEM__colorMsgFooter         "\"colorMFtr\""
#define SETITEM__colorMsgFooterBold     "\"colorMFtBd\""
#define SETITEM__colorMsgInform         "\"colorMInfo\""
#define SETITEM__colorMsgInformBold     "\"colorMInfBd\""
#define SETITEM__useColorMessageToken   "\"uClrMtkn\""
#define SETITEM__useColorMessageBold    "\"uClrMBld\""

#define SETITEM__getLogFeatureJogPush   "\"gLFJPsh\""
#define SETITEM__getLogFeatureJogBack   "\"gLFJBck\""
#define SETITEM__getLogFeatureJogLeft   "\"gLFJLft\""
#define SETITEM__getLogFeatureJogRight  "\"gLFJRght\""
#define SETITEM__getLogFeatureUp        "\"gLFJUp\""
#define SETITEM__getLogFeatureDown      "\"gLFJDwn\""
#define SETITEM__getLogFeatureCapture   "\"gLFJCptr\""
#define SETITEM__getLogFeatureKey1      "\"gLFJKy1\""
#define SETITEM__getLogFeatureKey2      "\"gLFJKy2\""
#define SETITEM__getLogFeatureKey3      "\"gLFJKy3\""
#define SETITEM__getLogFeatureKey4      "\"gLFJKy4\""

#define SETITEM__prepareDAforNewArrival "\"pDAfNA\""
#define SETITEM__preOnDAnewArrival      "\"pOnDAfNA\""
#define SETITEM__showResPopup           "\"sRsPpup\""
#define SETITEM__showNextUnreadThread   "\"sNUThrd\""

#define SETITEM__useGZIP                "\"uGZIP\""
#define SETITEM__vfsOnDefault           "\"vfsOnDef\""
#define SETITEM__useBe2chInfo           "\"uB2Inf\""
#define SETITEM__be2chId                "\"be2Id\""
#define SETITEM__be2chPw                "\"be2Pw\""
#define SETITEM__roundTripDelay         "\"rdTpDy\""
#define SETITEM__searchPickupMode       "\"sPkuMd\""
#define SETITEM__getLogSiteListMode     "\"gLSLMd\""
#define SETITEM__writingReplyMsg        "\"wrtRplyMsg\""


typedef struct {
   Char   *itemName;
   UInt8   itemType;
   UInt8   length;
   void   *value;
} NNsiXMLItems;


#endif /** #ifdef NNSI_XMLDEFS_H **/
