/*============================================================================*
 *  FILE: 
 *     machine.h
 *
 *  Description: 
 *     各Palmデバイス個別でNNsiに必要な定義を pick upした
 *
 *===========================================================================*/

/*** GHwrOEMCompanyID ***/
  /* All sony-based model has this ID. other one is not permitted */
  /* can be obtained by FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &value) */
  /* hwrOEMCompanyIDSony may be defined in HwrMiscFlags.h someday :-) */
#define sonyHwrOEMCompanyID_Sony	'sony'	/* CAN'T be changed!! */
#ifndef hwrOEMCompanyIDSony
#define hwrOEMCompanyIDSony	sonyHwrOEMCompanyID_Sony		
#endif	// sonyHwrOEMCompanyIDSony

/*** JOG DEFINITIONS for SONY CLIE(from SonyChars.h) ***/

//              Palm
#ifndef vchrPalmMin
  #define vchrPalmMin           (0x0500)   // 256 command keys
#endif

#ifndef	vchrPalmMax
  #define vchrPalmMax           (0x05FF)
#endif

//              TRG
#ifndef vchrCFlashMin
  #define vchrCFlashMin         (0x1500)
#endif

#ifndef vchrCFlashMax
  #define vchrCFlashMax         (0x150F)
#endif

//              SONY
#ifndef vchrSonyMin
  #define vchrSonyMin           (0x1700)
#endif
#ifndef vchrSonyMax
  #define vchrSonyMax           (0x170F)
#endif
#ifndef vchrJogUp
  #define vchrJogUp             (0x1700)
#endif
#ifndef vchrJogDown
  #define vchrJogDown           (0x1701)
#endif
#ifndef vchrPushRepeat
  #define vchrJogPushRepeat     (0x1702)
#endif
#ifndef vchrJogPushedUp
  #define vchrJogPushedUp       (0x1703)
#endif
#ifndef vchrJogPushedDown
  #define vchrJogPushedDown     (0x1704)
#endif
#ifndef vchrJogPush
  #define vchrJogPush           (0x1705)
#endif
#ifndef vchrJogRelease
  #define vchrJogRelease        (0x1706)
#endif
#ifndef vchrJogBack
  #define vchrJogBack           (0x1707)
#endif

///// from TrgChars.h
#define vchrTrgJogUp            (vchrCFlashMin + 4)
#define vchrTrgJogDown          (vchrCFlashMin + 5)
#define vchrTrgJogPush          (vchrCFlashMin + 6)
#define vchrTrgAux              (vchrCFlashMin + 7)

///// from PalmChars.h
#define vchrNavChange           (vchrPalmMin + 3)

#define navBitUp                0x0001
#define navBitDown              0x0002
#define navBitLeft              0x0004
#define navBitRight             0x0008
#define navBitSelect            0x0010
#define navBitsAll              0x001F

#define navChangeUp             0x0100
#define navChangeDown           0x0200
#define navChangeLeft           0x0400
#define navChangeRight          0x0800
#define navChangeSelect         0x1000
#define navChangeBitsAll        0x1F00


///////////////////////////////////////////////////////////////////////////////

/*** JOG ASSIST DEFINITIONS for SONY CLIE(from SonyJogAssist.h) ***/
/*** Type ***/
#define sonyJogAstMaskType1       (0x0001)
#define sonyJogAstMaskType2       (0x0002)

/*** Mask bitfields ***/
#define sonyJogAstMaskUp          (0x0001)
#define sonyJogAstMaskDown        (0x0002)
#define sonyJogAstMaskPushedUp    (0x0004)
#define sonyJogAstMaskPushedDown  (0x0008)
#define sonyJogAstMaskPush        (0x0010)
#define sonyJogAstMaskRelease     (0x0020)
#define sonyJogAstMaskPushRepeat  (0x0040)
#define sonyJogAstMaskBack        (0x0080)
#define sonyJogAstMaskReserved    (0xFF00)
#define sonyJogAstMaskAll         ~sonyJogAstMaskReserved
#define sonyJogAstMaskNone        (0x0000)

///////////////////////////////////////////////////////////////////////////////

/***** SONY SYSTEM CREATOR IDs (from SonySystemResources.h)  *****/

/*** Sony oveall ***/
#define sonySysFileCSony          'SoNy'  /* Sony overall */

/*** Fixes  ... but why? ***/
#ifndef sysFileTExtensionARM
#define sysFileTExtensionARM      'aext'
#endif

/* SonySilkLib */
#define sonySysFileCSilkExtn      'SeSi'
#define sonySysFileTSilkExtn      sysFileTExtensionARM    /* 'aext' */
#define sonySysResTSilkExtn       sysResTModuleCode       /* 'amdc' */
#define sonySysFileCSilkLib       sonySysFileCSilkExtn
#define sonySysFileTSilkLib       sonySysFileTSilkExtn    /* 'aext' */
#define sonySysLibNameSilk        "Sony Silk Library"

/* HR-Lib */
#define sonySysFileCHRLib         'SlHr'             /* High Resolution */
#define sonySysFileTHRLib         sysFileTLibrary    /* 'libr' */
#define sonySysLibNameHR          "Sony HR Library"

///////////////////////////////////////////////////////////////////////////////

/***** SONY SYSTEM FEATURES (from SonySystemFtr.h)  *****/

/*** Sony Ftr Creator ***/
#define sonySysFtrCreator            sonySysFileCSony

/*** Ftr Number ***/
#define sonySysFtrNumSysInfoP        (1)  /* ptr to SysInfo */
#define sonySysFtrNumStringInfoP     (2)  /* ptr to StringInfo */
#define sonySysFtrNumJogAstMaskP     (3)  /* ptr to JogAstMask */
#define sonySysFtrNumJogAstMOCardNoP (4)  /* ptr to JogAstMaskOwnerCardNo */
#define sonySysFtrNumJogAstMODbIDP   (5)  /* ptr to JogAstMaskOwnerDbID */

#define sonySysFtrNumVskVersion (0x0102)  /* VirtualSilkLibExtn */

/*** SysInfoP ***/
typedef struct S_SonySysFtrSysInfo {
	UInt16 revision;
	UInt16 rsv16_00;
	UInt32 extn;			/* loaded extension */
	UInt32 libr;			/* loaded libr */
	UInt32 rsv32_00;
	UInt32 rsv32_01;

	void *rsvP;
	UInt32 status;			/* current system status */
	UInt32 msStatus;		/* current MemoryStick status */
	UInt32 rsv32_10;

	UInt16 msSlotNum;		/* number of slot of MemoryStick */
	UInt16 jogType;
	UInt16 rmcType;
} SonySysFtrSysInfoType;
typedef SonySysFtrSysInfoType *SonySysFtrSysInfoP;

/* revision field */
#define sonySysFtrSysInfoRevision    (1)

/* extn field */
#define sonySysFtrSysInfoExtnJog     (0x00000001L)  /* vchrJogEvent usable */
#define sonySysFtrSysInfoExtnRmc     (0x00000002L)  /* vchrRmcEvent usable */
#define sonySysFtrSysInfoExtnHold    (0x00000004L)  /* Hold switch usable */
#define sonySysFtrSysInfoExtnJogAst  (0x00000008L)  /* JogAssist usable */

/***** SONY SILK LIBRARY (from SonySilkLib.h)  *****/
#define vskStateResize					(0)
	#define silkResizeNormal			(0)
	#define silkResizeToStatus			(1)
	#define silkResizeMax				(2)
	#define vskResizeMax				silkResizeNormal
	#define vskResizeMin				silkResizeToStatus
	#define vskResizeNone				silkResizeMax
#define vskStateEnable					(1)

/***** SONY High Resolution LIBRARY (from SonyHRLib.h)  *****/
#define HR_VERSION_SUPPORT_FNTSIZE	(0x200)

/** FONT ID **/
enum hrFontID {
	hrTinyFont = 0x00,
	hrTinyBoldFont,
	hrSmallFont,
	hrSmallSymbolFont,
	hrSmallSymbol11Font,
	hrSmallSymbol7Font,
	hrSmallLedFont,
	hrSmallBoldFont,
	hrStdFont,
	hrBoldFont,
	hrLargeFont,
	hrSymbolFont,
	hrSymbol11Font,
	hrSymbol7Font,
	hrLedFont,
	hrLargeBoldFont,
	hrFntAppFontCustomBase = 0x80
};

typedef enum hrFontID HRFontID;

/********************************************************************
 * define SCREEN SIZE
 ********************************************************************/
 
#define hrWidth			320
#define hrHeight		320
#define stdWidth		160
#define stdHeight		160

///////////////////////////////////////////////////////////////////////////////

/*************** DEFINITIONS FOR SONY SILK LIBRARY FUNCTIONS ***************/
#define SILK_LIB_TRAP(trapNum)    SYS_TRAP(trapNum)

#define silkLibTrapResizeDispWin  (sysLibTrapCustom)
#define silkLibTrapEnableResize   ((sysLibTrapCustom)+1)
#define silkLibTrapDisableResize  ((sysLibTrapCustom)+2)
#define silkLibTrapGetAPIVersion  ((sysLibTrapCustom)+3)
#define silkLibLastTrap           ((sysLibTrapCustom)+4)

#define VskTrapGetAPIVersion      (silkLibTrapGetAPIVersion)
#define VskTrapSetCurrentSlkw     ((silkLibTrapGetAPIVersion)+1)
#define VskTrapGetCurrentSlkw     ((silkLibTrapGetAPIVersion)+2)
#define VskTrapSetState           ((silkLibTrapGetAPIVersion)+3)
#define VskTrapGetState           ((silkLibTrapGetAPIVersion)+4)
#define VskTrapEnablePalmSilk     ((silkLibTrapGetAPIVersion)+5)
#define VskTrapGetPalmSilkEnabled ((silkLibTrapGetAPIVersion)+6)
#define VskTrapTimerWrite         ((silkLibTrapGetAPIVersion)+7)
#define VskTrapDoCommand          ((silkLibTrapGetAPIVersion)+8)
#define VskTrapSetDrawWindow      ((silkLibTrapGetAPIVersion)+9)
#define VskTrapRestoreDrawWindow  ((silkLibTrapGetAPIVersion)+10)

// old APIs (FOR NR)
extern Err SilkLibOpen(UInt16 refNum)
				SILK_LIB_TRAP(sysLibTrapOpen);
extern Err SilkLibClose(UInt16 refNum)
				SILK_LIB_TRAP(sysLibTrapClose);
extern Err SilkLibSleep(UInt16 refNum)
				SILK_LIB_TRAP(sysLibTrapSleep);
extern Err SilkLibWake(UInt16 refNum)
				SILK_LIB_TRAP(sysLibTrapWake);
extern Err SilkLibResizeDispWin(UInt16 refNum, UInt8 win)
				SILK_LIB_TRAP(silkLibTrapResizeDispWin);
extern Err SilkLibEnableResize(UInt16 refNum)
				SILK_LIB_TRAP(silkLibTrapEnableResize);
extern Err SilkLibDisableResize(UInt16 refNum)
				SILK_LIB_TRAP(silkLibTrapDisableResize);
extern UInt32 SilkLibGetAPIVersion(UInt16 refNum)
				SILK_LIB_TRAP(silkLibTrapGetAPIVersion);

// new APIs (FOR NX)
extern Err VskOpen(UInt16 refNum)
				SILK_LIB_TRAP(sysLibTrapOpen);
extern Err VskClose(UInt16 refNum)
				SILK_LIB_TRAP(sysLibTrapClose);
extern Err VskSleep(UInt16 refNum)
				SILK_LIB_TRAP(sysLibTrapSleep);
extern Err VskWake(UInt16 refNum)
				SILK_LIB_TRAP(sysLibTrapWake);
// this will be removed
//extern Err VskResize(UInt16 refNum, UInt8 reqSize);
// this will be removed
//extern Err VskEnableResize(UInt16 refNum);
// this will be removed
//extern Err VskDisableResize(UInt16 refNum);
extern UInt32 VskGetAPIVersion(UInt16 refNum)
				SILK_LIB_TRAP(VskTrapGetAPIVersion);
extern Err VskSetCurrentSlkw(UInt16 refNum, UInt16 slkwType, UInt32 creator)
				SILK_LIB_TRAP(VskTrapSetCurrentSlkw);
extern Err VskGetCurrentSlkw(UInt16 refNum, UInt16 slkwType, UInt32 *creatorP)
				SILK_LIB_TRAP(VskTrapGetCurrentSlkw);
extern Err VskSetState(UInt16 refNum, UInt16 stateType, UInt16 state)
				SILK_LIB_TRAP(VskTrapSetState);
extern Err VskGetState(UInt16 refNum, UInt16 stateType, UInt16 *stateP)
				SILK_LIB_TRAP(VskTrapGetState);
extern Err VskEnablePalmSilk(UInt16 refNum, Boolean enable)
				SILK_LIB_TRAP(VskTrapEnablePalmSilk);
extern Err VskGetPalmSilkEnabled(UInt16 refNum, Boolean *graffiti, Boolean *penButton)
				SILK_LIB_TRAP(VskTrapGetPalmSilkEnabled);
extern Err VskTimerWrite(UInt16 refNum, UInt16 slkwType, UInt32 interval/*msec*/)
				SILK_LIB_TRAP(VskTrapTimerWrite);
extern Err VskDoCommand(UInt16 refNum, UInt32 creator, UInt16 command,
								UInt32 data1, UInt32 data2)
				SILK_LIB_TRAP(VskTrapDoCommand);
extern Err VskSetDrawWindow(UInt16 refNum, UInt16 slkwType)
				SILK_LIB_TRAP(VskTrapSetDrawWindow);
extern Err VskRestoreDrawWindow(UInt16 refNum, UInt16 slkwType)
				SILK_LIB_TRAP(VskTrapRestoreDrawWindow);

/******* DEFINITIONS FOR SONY High Resolution LIBRARY FUNCTIONS *******/
#if 0
#define	HRTrapGetAPIVersion     (sysLibTrapCustom)
#define	HRTrapWinClipRectangle ((sysLibTrapCustom)+1)
#define	HRTrapWinCopyRectangle ((sysLibTrapCustom)+2)
#define	HRTrapWinCreateBitmapWindow ((sysLibTrapCustom)+3)
#define	HRTrapWinCreateOffscreenWindow ((sysLibTrapCustom)+4)
#define	HRTrapWinCreateWindow ((sysLibTrapCustom)+5)
#define	HRTrapWinDisplayToWindowPt ((sysLibTrapCustom)+6)
#define	HRTrapWinDrawBitmap ((sysLibTrapCustom)+7)
#define	HRTrapWinDrawChar ((sysLibTrapCustom)+8)
#define	HRTrapWinDrawChars ((sysLibTrapCustom)+9)
#define	HRTrapWinDrawGrayLine ((sysLibTrapCustom)+10)
#define	HRTrapWinDrawGrayRectangleFrame ((sysLibTrapCustom)+11)
#define	HRTrapWinDrawInvertedChars ((sysLibTrapCustom)+12)
#define	HRTrapWinDrawLine ((sysLibTrapCustom)+13)
#define	HRTrapWinDrawPixel ((sysLibTrapCustom)+14)
#define	HRTrapWinDrawRectangle ((sysLibTrapCustom)+15)
#define	HRTrapWinDrawRectangleFrame ((sysLibTrapCustom)+16)
#define	HRTrapWinDrawTruncChars ((sysLibTrapCustom)+17)
#define	HRTrapWinEraseChars ((sysLibTrapCustom)+18)
#define	HRTrapWinEraseLine ((sysLibTrapCustom)+19)
#define	HRTrapWinErasePixel ((sysLibTrapCustom)+20)
#define	HRTrapWinEraseRectangle ((sysLibTrapCustom)+21)
#define	HRTrapWinEraseRectangleFrame ((sysLibTrapCustom)+22)
#define	HRTrapWinFillLine ((sysLibTrapCustom)+23)
#define	HRTrapWinFillRectangle ((sysLibTrapCustom)+24)
#define	HRTrapWinGetClip ((sysLibTrapCustom)+25)
#define	HRTrapWinGetDisplayExtent ((sysLibTrapCustom)+26)
#define	HRTrapWinGetFramesRectangle ((sysLibTrapCustom)+27)
#define	HRTrapWinGetPixel ((sysLibTrapCustom)+28)
#define	HRTrapWinGetWindowBounds ((sysLibTrapCustom)+29)
#define	HRTrapWinGetWindowExtent ((sysLibTrapCustom)+30)
#define	HRTrapWinGetWindowFrameRect ((sysLibTrapCustom)+31)
#define	HRTrapWinInvertChars ((sysLibTrapCustom)+32)
#define	HRTrapWinInvertLine ((sysLibTrapCustom)+33)
#define	HRTrapWinInvertPixel ((sysLibTrapCustom)+34)
#define	HRTrapWinInvertRectangle ((sysLibTrapCustom)+35)
#define	HRTrapWinInvertRectangleFrame ((sysLibTrapCustom)+36)
#define	HRTrapWinPaintBitmap ((sysLibTrapCustom)+37)
#define	HRTrapWinPaintChar ((sysLibTrapCustom)+38)
#define	HRTrapWinPaintChars ((sysLibTrapCustom)+39)
#define	HRTrapWinPaintLine ((sysLibTrapCustom)+40)
#define	HRTrapWinPaintLines ((sysLibTrapCustom)+41)
#define	HRTrapWinPaintPixel ((sysLibTrapCustom)+42)
#define	HRTrapWinPaintPixels ((sysLibTrapCustom)+43)
#define	HRTrapWinPaintRectangle ((sysLibTrapCustom)+44)
#define	HRTrapWinPaintRectangleFrame ((sysLibTrapCustom)+45)
#define	HRTrapWinRestoreBits ((sysLibTrapCustom)+46)
#define	HRTrapWinSaveBits ((sysLibTrapCustom)+47)
#define	HRTrapWinScreenMode ((sysLibTrapCustom)+48)
#define	HRTrapWinScrollRectangle ((sysLibTrapCustom)+49)
#define	HRTrapWinSetClip ((sysLibTrapCustom)+50)
#define	HRTrapWinSetWindowBounds ((sysLibTrapCustom)+51)
#define	HRTrapWinWindowToDisplayPt ((sysLibTrapCustom)+52)
#define	HRTrapBmpBitsSize ((sysLibTrapCustom)+53)
#define	HRTrapBmpSize ((sysLibTrapCustom)+54)
#define	HRTrapBmpCreate ((sysLibTrapCustom)+55)
#define	HRTrapFntGetFont ((sysLibTrapCustom)+56)
#define	HRTrapFntSetFont ((sysLibTrapCustom)+57)
#define	HRTrapFontSelect ((sysLibTrapCustom)+58)
#define	HRTrapSystem ((sysLibTrapCustom)+59)
#define	HRTrapWinGetPixelRGB ((sysLibTrapCustom)+60)
#define	HRTrapGetInfo ((sysLibTrapCustom)+61)
#define	HRTrapFntBaseLine ((sysLibTrapCustom)+62)
#define	HRTrapFntCharHeight ((sysLibTrapCustom)+63)
#define	HRTrapFntLineHeight ((sysLibTrapCustom)+64)
#define	HRTrapFntAverageCharWidth ((sysLibTrapCustom)+65)
#define	HRTrapFntCharWidth ((sysLibTrapCustom)+66)
#define	HRTrapFntWCharWidth ((sysLibTrapCustom)+67)
#define	HRTrapFntCharsWidth ((sysLibTrapCustom)+68)
#define	HRTrapFntWidthToOffset ((sysLibTrapCustom)+69)
#define	HRTrapFntCharsInWidth ((sysLibTrapCustom)+70)
#define	HRTrapFntDescenderHeight ((sysLibTrapCustom)+71)
#define	HRTrapFntLineWidth ((sysLibTrapCustom)+72)
#define	HRTrapFntWordWrap ((sysLibTrapCustom)+73)
#define	HRTrapFntWordWrapReverseNLines ((sysLibTrapCustom)+74)
#define	HRTrapFntGetScrollValues ((sysLibTrapCustom)+75)
#endif

#define	HRTrapGetAPIVersion     (sysLibTrapCustom)
#define	HRTrapWinDrawTruncChars ((sysLibTrapCustom)+17)
#define	HRTrapWinDrawInvertedChars ((sysLibTrapCustom)+12)
#define	HRTrapFntSetFont        ((sysLibTrapCustom)+57)
#define	HRTrapFontSelect        ((sysLibTrapCustom)+58)
#define	HRTrapWinEraseRectangle ((sysLibTrapCustom)+21)
#define	HRTrapFntCharHeight     ((sysLibTrapCustom)+63)
#define	HRTrapWinDrawChars      ((sysLibTrapCustom)+9)
#define	HRTrapWinScreenMode     ((sysLibTrapCustom)+48)
#define	HRTrapFntWordWrap       ((sysLibTrapCustom)+73)
#define	HRTrapWinInvertChars    ((sysLibTrapCustom)+32)
#define	HRTrapWinDrawRectangleFrame ((sysLibTrapCustom)+16)

#define HR_TRAP(trapNum)	SYS_TRAP(trapNum)

extern Err	HROpen(UInt16 refNum)		HR_TRAP(sysLibTrapOpen);

extern Err	HRClose(UInt16 refNum)		HR_TRAP(sysLibTrapClose);

extern HRFontID HRFntSetFont(UInt16 refNum, HRFontID font)
	HR_TRAP(HRTrapFntSetFont);

extern HRFontID HRFontSelect(UInt16 refNum, HRFontID font)
	HR_TRAP(HRTrapFontSelect);

extern Err	HRGetAPIVersion(UInt16 refNum, UInt16 *versionP)
	HR_TRAP(HRTrapGetAPIVersion);

extern void HRWinEraseRectangle(UInt16 refNum, RectangleType *rP, UInt16 cornerDiam)
	HR_TRAP(HRTrapWinEraseRectangle);

extern Int16 HRFntCharHeight (UInt16 refNum)
	HR_TRAP(HRTrapFntCharHeight);

extern void HRWinDrawChars(UInt16 refNum, const Char *chars, Int16 len, Coord x, Coord y)
	HR_TRAP(HRTrapWinDrawChars);

extern Err HRWinScreenMode(UInt16 refNum, WinScreenModeOperation operation, UInt32 *widthP, UInt32 *heightP, UInt32 *depthP, Boolean *enableColorP)
	HR_TRAP(HRTrapWinScreenMode);

extern UInt16 HRFntWordWrap (UInt16 refNum, Char const *chars, UInt16 maxWidth)
	HR_TRAP(HRTrapFntWordWrap);

extern void HRWinDrawTruncChars(UInt16 refNum, const Char *chars, Int16 len, Coord x, Coord y, Coord maxWidth)
	HR_TRAP(HRTrapWinDrawTruncChars);

extern void HRWinDrawInvertedChars(UInt16 refNum, const Char *chars, Int16 len, Coord x, Coord y)
	HR_TRAP(HRTrapWinDrawInvertedChars);

extern void HRWinInvertChars(UInt16 refNum, const Char *chars, Int16 len, Coord x, Coord y)
	HR_TRAP(HRTrapWinInvertChars);
extern void HRWinDrawRectangleFrame(UInt16 refNum, FrameType frame, RectangleType *rP)
	HR_TRAP(HRTrapWinDrawRectangleFrame);

///////////////////////////////////////////////////////////////////////////////
