//////////////////////////////////////////////////////////////////////////////
// $Id: capda-defines.h,v 1.1.1.1 2005/01/08 15:53:41 mrsa Exp $
//
// capda-defines.h
// captureDA Data structure definition file
//
// (C) 2004, NNsi project
//
//////////////////////////////////////////////////////////////////////////////

#include <PalmOS.h>
#include <PalmCompatibility.h>
#include <VFSMgr.h>
#include <Progress.h>
#include <FileStream.h>
#include <CharShiftJIS.h>

// DA creator
#define capDA_Creator        'capD'
#define capDA_TITLESIZE      16
#define capDA_DEFTITLE       "Clie CaptureDA"
#define capDA_FILENAMESIZE   256
#define capDA_BUFFERSIZE     272
#define capDA_BITMAP_BUFFER  50000

struct capDADataType
{
    UInt32         palmOSVersion;   // デバイスのOSバージョン
    UInt16         actionCommand;   // 動作コマンド
    UInt32         os5density;      // PalmOS5の解像度
    RectangleType  dimF;            // 表示領域のサイズ(coord領域)
    RectangleType  area;            // 表示領域のサイズ(実領域)
    UInt16         vfsVol;
    UInt32         vfsVolumeIterator;
    VolumeInfoType volInfo;  
    FileRef        vfsFileRef;
    FileHand       streamRef;
    void         *jpegData;        // JPEGデータ格納領域
    BitmapType    *bitmapPP;        // bitmapデータ格納領域
    UInt16         capLibRef;
    UInt32         capDevPortId;
    UInt8          clipboardInfo[capDA_BUFFERSIZE + 8];
    UInt8         *bitmapBuffer;
};

typedef struct capDADataType  capDADataType;

// global feature id:
#define ADT_FTRID       1
#define ADT_FTRID_MEM   2

// action commands
#define ACTION_NOTSPECIFY       0  // 未指定
#define ACTION_OUTPUTJPEG       1  // JPEG出力
#define ACTION_OUTPUTBMP        2  // Bitmap出力

// NNsiとの連携機能用定義
#define NNSIEXT_VIEWSTART    "<NNsi:NNsiExt type=\"VIEW\">"
#define NNSIEXT_INPUTSTART   "<NNsi:NNsiExt type=\"INPUT\">"
#define NNSIEXT_END          "</NNsi:NNsiExt>"
#define NNSIEXT_INFONAME     "<NNsi:Info>"
#define NNSIEXT_ENDINFONAME  "</NNsi:Info>"

#define capDA_NNSIEXT_OUTJPEG     "OUTJPEG:"
#define capDA_NNSIEXT_OUTBMP      "OUTBMP:"


// other definitions...
#define BUFSIZE        48
#define MARGIN         16
#define BUF_MARGIN    128
#define DISP_LIMIT  10240

// from SonyChars.h
#ifndef vchrJogPushedDown
  #define vchrJogPushedDown     (0x1704)
#endif
#ifndef vchrJogPushedUp
  #define vchrJogPushedUp       (0x1703)
#endif
#ifndef vchrJogUp
  #define vchrJogUp             (0x1700)
#endif
#ifndef vchrJogDown
  #define vchrJogDown           (0x1701)
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

#ifndef vchrJogLeft
  #define vchrJogLeft           (0x1708)
#endif

#ifndef vchrJogRight
  #define vchrJogRight          (0x1709)
#endif

#ifndef vchrCapHalfRelease
#define vchrCapHalfRelease      (0x171A)
#endif

#ifndef vchrCapCancel
#define vchrCapCancel           (0x171B)
#endif

#ifndef vchrCapture
#define vchrCapture             (0x170C)
#endif

#ifndef vchrVoiceRec
#define vchrVoiceRec            (0x170D)
#endif


///// from PalmChars.h
#ifndef vchrNavChange
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
#endif

#ifndef vchrRockerUp

#define vchrRockerUp                0x0132      // 5-way rocker up
#define vchrRockerDown              0x0133      // 5-way rocker down
#define vchrRockerLeft              0x0134      // 5-way rocker left
#define vchrRockerRight             0x0135      // 5-way rocker right
#define vchrRockerCenter            0x0136      // 5-way rocker center/press
#define vchrInputAreaControl        0x0137      // Toggle for opening and closing input area
#define vchrBluetoothSetup          0x0138      // goto Bluetooth panel
#endif // ndef vchrRockerUp

#ifndef vchrHardRockerCenter
#define vchrHardRockerCenter        0x013D
#endif

#ifdef USE_CLIE
/***** Sony HR-Lib *****/
#define sonySysFileCHRLib         'SlHr'             /* High Resolution */
#define sonySysFileTHRLib         sysFileTLibrary    /* 'libr' */
#define sonySysLibNameHR          "Sony HR Library"

/***** SONY High Resolution LIBRARY (from SonyHRLib.h)  *****/
#define HR_VERSION_SUPPORT_FNTSIZE        (0x200)
#define hrWidth                           320
#define hrHeight                          320

#define HR_TRAP(trapNum)                  SYS_TRAP(trapNum)

#define HRTrapGetAPIVersion               (sysLibTrapCustom)
#define HRTrapWinScreenMode               ((sysLibTrapCustom)+48)
#define HRTrapWinEraseRectangle           ((sysLibTrapCustom)+21)
#define HRTrapWinDrawChar                 ((sysLibTrapCustom)+8)
#define HRTrapWinDrawBitmap               ((sysLibTrapCustom)+7)
#define HRTrapWinDrawChars                ((sysLibTrapCustom)+9)

extern Err  HROpen(UInt16 refNum)
                                           HR_TRAP(sysLibTrapOpen);
extern Err  HRClose(UInt16 refNum)
                                           HR_TRAP(sysLibTrapClose);
extern Err  HRGetAPIVersion(UInt16 refNum, UInt16 *versionP)
                                           HR_TRAP(HRTrapGetAPIVersion);
extern Err  HRWinScreenMode(UInt16 refNum, WinScreenModeOperation operation, UInt32 *widthP, UInt32 *heightP, UInt32 *depthP, Boolean *enableColorP)
                                           HR_TRAP(HRTrapWinScreenMode);
extern void HRWinEraseRectangle(UInt16 refNum, RectangleType *rP, UInt16 cornerDiam)
                                           HR_TRAP(HRTrapWinEraseRectangle);
extern void HRWinDrawChar(UInt16 refNum, WChar theChar, Coord x, Coord Y)
                                           HR_TRAP(HRTrapWinDrawChar);
extern void HRWinDrawBitmap(UInt16 refNum, BitmapPtr bitmapP, Coord x, Coord Y)
	HR_TRAP(HRTrapWinDrawBitmap);

extern void HRWinDrawChars(UInt16 refNum, const Char *chars, Int16 len, Coord x, Coord y)
        HR_TRAP(HRTrapWinDrawChars);

// ===== from SonyJpegUtilLib.h =====
#ifdef USE_SONY_JPEG

/* JpegUtil-Lib */
#define sonySysFileCJpegUtilLib	'SlJU'	/* Jpeg Util Lib */
#define sonySysFileTJpegUtilLib	sysFileTLibrary	/* 'libr' */
//#define sonySysLibNameJpegUtil	"Sony Jpeg Util Library"
#define sonySysLibNameJpegUtil	"Sony Jpeg Util Library"

#define JPEG_UTIL_LIB_TRAP(trapNum) SYS_TRAP(trapNum)

#define jpegUtilLibErrorBase               (0x00)
#define jpegUtilLibErrorClass              (sonyJpegErrorClass | jpegUtilLibErrorBase)

#define jpegUtilLibErrNone                 (errNone)
#define jpegUtilLibErrBadParam             (jpegUtilLibErrorClass | 1)  // invalid parameter
#define jpegUtilLibErrNotOpen              (jpegUtilLibErrorClass | 2)  // library is not open
#define jpegUtilLibErrStillOpen            (jpegUtilLibErrorClass | 3)  // library still in used
#define jpegUtilLibErrNoMemory             (jpegUtilLibErrorClass | 4)
#define jpegUtilLibErrNotSupported         (jpegUtilLibErrorClass | 5)
#define jpegUtilLibErrNotJpegFormat        (jpegUtilLibErrorClass | 6)
#define jpegUtilLibErrNotExifFormat        (jpegUtilLibErrorClass | 7)
#define jpegUtilLibErrExifParamNotFound    (jpegUtilLibErrorClass | 8)
#define jpegUtilLibErrEncDecCanceled       (jpegUtilLibErrorClass | 9)
#define jpegUtilLibErrResourceBusy         (jpegUtilLibErrorClass | 10)

/***************************************************************************
 * Definition
 ***************************************************************************/
#define Incapable                          (0)
#define Capable                            (1)

/******************************************************************************
 *      Structure                                                             *
 ******************************************************************************/
typedef Boolean (*JpegPrgCallbackFunc)  (void *prgCbData);

typedef struct
{
    UInt16                 percent;          // progress
    ProgressPtr            prgP;             // in case of using ProgressMgr
    JpegPrgCallbackFunc    prgCbFunc;        // callback function in the case not using ProgressMgr
    void                  *prgCbData;
} PrgInfoType, *PrgInfoP;

typedef struct
{
    UInt32    numerator;
    UInt32    denominator;    
} RationalType;

/* capability */
typedef struct {
    UInt16 version:1;
    UInt16 latitudeRef:1;
    UInt16 latitude:1;
    UInt16 longitudeRef:1;
    UInt16 longitude:1;
    UInt16 altitudeRef:1;
    UInt16 altitude:1;
    UInt16 mapDatum:1;
    UInt16 reserved:8;
} GPSInfoCapabilityType;

/* capability */
typedef struct {
    UInt16 softName:1;
    UInt16 gpsInfo:1;
    UInt16 orientation:1;    // available ver.2.0
    UInt16 reserved:13;
} JpegDetailInfoCapabilityType;

typedef struct
{
    GPSInfoCapabilityType    gpsInfoCapability;
    Char                     version[4];
    Char                     latitudeRef[2];
    RationalType             latitude[3];
    Char                     longitudeRef[2];
    RationalType             longitude[3];
    Char                     altitudeRef;
    RationalType             altitude;
    Char                    *mapDatum;
} GPSInfoType, *GPSInfoP;

typedef struct
{
    JpegDetailInfoCapabilityType    jpegDetailInfoCapability;    
    Char                            dateTime[20];           /* ex) 2001:10:23 21:04:13 */
    Char                           *softName;
    GPSInfoP                        gpsInfoP;
    // available ver.2.0
    Char                            dateTimeOriginal[20];   /* ex) 2001:10:23 21:04:13 */
    Char                            dateTimeDigitized[20];  /* ex) 2001:10:23 21:04:13 */
    UInt16                          orientation;
    //JpegImageOrientation          orientation;            //add by tashiro 02/05/24
} JpegDetailInfoType, *JpegDetailInfoP;

/***************************************************************************
 * Enum
 ***************************************************************************/
typedef enum {
    jpegDecRatioNormal = 0,
    jpegDecRatioHalf,
    jpegDecRatioQuarter,
    jpegDecRatioOctant
} JpegImageRatio;

typedef enum {
    jpegDecModeNormal = 0,
    jpegDecModeThumbnail
} JpegImageType;

// available ver.2.0
typedef enum {
    JpegImageOriNotSupported = 0,
    JpegImageOriNormal,
    JpegImageOriNormalR,       //notsupported
    JpegImageOriCW180,
    JpegImageOriCW180R,        //notsupported
    JpegImageOriCW90R,         //notsupported
    JpegImageOriCW90,
    JpegImageOriCCW90R,        //notsupported
    JpegImageOriCCW90
} JpegImageOrientation;


/***************************************************************************
 * Macro
 ***************************************************************************/
#define FreeJpegDetailInfo(jpegDetailInfoP)                           \
{                                                                     \
    if((jpegDetailInfoP)->jpegDetailInfoCapability.softName) {        \
        MemPtrFree((jpegDetailInfoP)->softName);                      \
    }                                                                 \
    if((jpegDetailInfoP)->jpegDetailInfoCapability.gpsInfo) {         \
        if((jpegDetailInfoP)->gpsInfoP->gpsInfoCapability.mapDatum) { \
            MemPtrFree((jpegDetailInfoP)->gpsInfoP->mapDatum);        \
        }                                                             \
        MemPtrFree((jpegDetailInfoP)->gpsInfoP);                      \
    }                                                                 \
}


/***************************************************************************
 * Jpeg Util library function trap ID's. Each library call gets a trap number:
 *   JpegLibTrapXXXX which serves as an index into the library's dispatch
 *   table. The constant sysLibTrapCustom is the first available trap number
 *   after the system predefined library traps Open,Close,Sleep & Wake.
 *
 * WARNING!!! The order of these traps MUST match the order of the dispatch
 *  table in LibDispatch.c!!!
 ****************************************************************************/
#define jpegUtilLibTrapGetLibAPIVersion           ((sysLibTrapCustom)+0)
#define jpegUtilLibTrapDecodeImageToBmp           ((sysLibTrapCustom)+1)     /* jpegUtilLibDecodeImageToBmp           */
#define jpegUtilLibTrapDecodeImageToWindow        ((sysLibTrapCustom)+2)     /* jpegUtilLibDecodeImageToWindow        */
#define jpegUtilLibTrapEncodeImageFromBmp         ((sysLibTrapCustom)+3)     /* jpegUtilLibEncodeImageFromBmp         */
#define jpegUtilLibTrapEncodeImageFromWindow      ((sysLibTrapCustom)+4)     /* jpegUtilLibEncodeImageFromWindow      */
#define jpegUtilLibTrapEncodeImageFromPGP         ((sysLibTrapCustom)+5)     /* jpegUtilLibEncodeImageFromPGP         */
#define jpegUtilLibTrapGetJpegInfo                ((sysLibTrapCustom)+6)     /* jpegUtilLibGetJpegInfo                */
#define jpegUtilLibTrapDecodeImageToBmpForFS      ((sysLibTrapCustom)+7)     /* jpegUtilLibDecodeImageToBmpForFS      */
#define jpegUtilLibTrapDecodeImageToWindowForFS   ((sysLibTrapCustom)+8)     /* jpegUtilLibDecodeImageToWindowForFS   */
#define jpegUtilLibTrapEncodeImageFromBmpForFS    ((sysLibTrapCustom)+9)     /* jpegUtilLibEncodeImageFromBmpForFS    */
#define jpegUtilLibTrapEncodeImageFromWindowForFS ((sysLibTrapCustom)+10)    /* jpegUtilLibEncodeImageFromWindowForFS */
#define jpegUtilLibTrapEncodeImageFromPGPForFS    ((sysLibTrapCustom)+11)    /* jpegUtilLibEncodeImageFromPGPForFS    */
#define jpegUtilLibTrapGetJpegInfoForFS           ((sysLibTrapCustom)+12)    /* jpegUtilLibGetJpegInfoForFS           */
#define jpegUtilLibTrapSetJpegOrientation         ((sysLibTrapCustom)+13)    /* jpegUtilLibSetJpegOrientation         */
#define jpegUtilLibTrapSetJpegOrientationForFS    ((sysLibTrapCustom)+14)    /* jpegUtilLibSetJpegOrientationForFS    */

/********************************************************************
 *              Library API Prototypes
 ********************************************************************/

/* open the library */
extern Err jpegUtilLibOpen(UInt16 jpegUtilLibRefNum)
                    JPEG_UTIL_LIB_TRAP(sysLibTrapOpen);

/* close the library */
extern Err jpegUtilLibClose(UInt16 jpegUtilLibRefNum)
                    JPEG_UTIL_LIB_TRAP(sysLibTrapClose);

/* library sleep */
extern Err jpegUtilLibSleep(UInt16 jpegUtilLibRefNum)
                    JPEG_UTIL_LIB_TRAP(sysLibTrapSleep);

/* library wakeup */
extern Err jpegUtilLibWake(UInt16 jpegUtilLibRefNum)
                    JPEG_UTIL_LIB_TRAP(sysLibTrapWake);

/*--------------------------------------------------------------------------
 * Custom library API functions
 *--------------------------------------------------------------------------*/

/* Get our library API version */
extern UInt32 jpegUtilLibGetLibAPIVersion(UInt16 jpegUtilLibRefNum)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapGetLibAPIVersion);

extern Err jpegUtilLibDecodeImageToBmp(UInt16 jpegLibRefNum, FileRef fileRef,
                                       MemPtr inBufP, JpegImageType imageType,
                                       JpegImageRatio ratio, BitmapPtr *bitmapPP,
                                       PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapDecodeImageToBmp);

extern Err jpegUtilLibDecodeImageToWindow(UInt16 jpegLibRefNum, FileRef fileRef,
                                          MemPtr inBufP, JpegImageType imageType,
                                          RectangleType *rP, PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapDecodeImageToWindow);

extern Err jpegUtilLibEncodeImageFromBmp(UInt16 jpegLibRefNum, Boolean isExif,
                                         Char *dateTimeOriginal, Char *softName, GPSInfoP gpsInfoP, 
                                         UInt8 quality, BitmapPtr bitmapP,
                                         FileRef fileRef, MemPtr *outBufP, UInt32 *outBufSize,
                                         PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapEncodeImageFromBmp);

extern Err jpegUtilLibEncodeImageFromWindow(UInt16 jpegLibRefNum, Boolean isExif,
                                            Char *dateTimeOriginal, Char *softName, GPSInfoP gpsInfoP,
                                            UInt8 quality, RectangleType *rP,
                                            FileRef fileRef, MemPtr *outBufP, UInt32 *outBufSize,
                                            PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapEncodeImageFromWindow);

extern Err jpegUtilLibEncodeImageFromPGP(UInt16 jpegLibRefNum, Boolean isExif,
                                         Char *softName, GPSInfoP gpsInfoP,
                                         UInt8 quality, DmOpenRef dRef, FileRef inFileRef,
                                         FileRef outFileRef, MemPtr *outBufP, UInt32 *outBufSize,
                                         PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapEncodeImageFromPGP);

extern Err jpegUtilLibGetJpegInfo(UInt16 jpegLibRefNum, FileRef fileRef,
                                  MemPtr inBufP, UInt32 *imgHeight, UInt32 *imgWidth, 
                                  Boolean *isThumbnail, JpegDetailInfoP jpegDetailInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapGetJpegInfo);

// available ver.2.0
extern Err jpegUtilLibDecodeImageToBmpForFS(UInt16 jpegLibRefNum, FileHand stream,
                                            JpegImageType imageType,
                                            JpegImageRatio ratio, BitmapPtr *bitmapPP,
                                            PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapDecodeImageToBmpForFS);

extern Err jpegUtilLibDecodeImageToWindowForFS(UInt16 jpegLibRefNum, FileHand stream,
                                               JpegImageType imageType,
                                               RectangleType *rP, PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapDecodeImageToWindowForFS);

extern Err jpegUtilLibEncodeImageFromBmpForFS(UInt16 jpegLibRefNum, Boolean isExif,
                                              Char *dateTimeOriginal, Char *softName, GPSInfoP gpsInfoP, 
                                              UInt8 quality, BitmapPtr bitmapP,
                                              FileHand stream, PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapEncodeImageFromBmpForFS);

extern Err jpegUtilLibEncodeImageFromWindowForFS(UInt16 jpegLibRefNum, Boolean isExif,
                                                 Char *dateTimeOriginal, Char *softName, GPSInfoP gpsInfoP,
                                                 UInt8 quality, RectangleType *rP,
                                                 FileHand stream, PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapEncodeImageFromWindowForFS);

extern Err jpegUtilLibEncodeImageFromPGPForFS(UInt16 jpegLibRefNum, Boolean isExif,
                                              Char *softName, GPSInfoP gpsInfoP,
                                              UInt8 quality, DmOpenRef dRef, FileRef inFileRef,
                                              FileHand stream, PrgInfoP prgInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapEncodeImageFromPGPForFS);

extern Err jpegUtilLibGetJpegInfoForFS(UInt16 jpegLibRefNum, FileHand stream,
                                       UInt32 *imgHeight, UInt32 *imgWidth, 
                                       Boolean *isThumbnail, JpegDetailInfoP jpegDetailInfoP)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapGetJpegInfoForFS);

extern Err jpegUtilLibSetJpegOrientation(UInt16 jpegLibRefNum, FileRef fileRef,
                                         MemPtr inBufP, JpegImageOrientation orientation)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapSetJpegOrientation);

extern Err jpegUtilLibSetJpegOrientationForFS(UInt16 jpegLibRefNum, FileHand stream,
                                              JpegImageOrientation orientation)
   JPEG_UTIL_LIB_TRAP(jpegUtilLibTrapSetJpegOrientationForFS);

#endif  // #ifdef USE_SONY_JPEG
#endif  // #ifdef USE_CLIE

/*---------------------------------------------------------------------------*
 *    SonyCapLib.h より...
 *---------------------------------------------------------------------------*/
/******************************************************************************
 *                                                                            *
 *                 (C) Copyright 2001-2003, Sony Corporation                  *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *                                                                            *
 *    <IDENTIFICATION>                                                        *
 *       file name    : $Workfile: SonyCapLib.h $
 *                                                                            *
 *    <PROFILE>                                                               *
 *       Capture library prototype                                            *
 *                                                                            *
 *                                                                            *
 *    <HISTORY>                                                               *
 *       Started on   : 01/09/11                                              *
 *       Last Modified: $Date: 2005/01/08 15:53:41 $
 *                                                                            *
 ******************************************************************************/
#ifndef __CAP_LIB_H__
#define __CAP_LIB_H__

//#include <SonyErrorBase.h>

#ifndef oemErrorClass
#define	oemErrorClass				0x7000	// OEM/Licensee errors (0x7000-0x7EFF shared among ALL partners)
#endif

#ifndef sonyErrorClass
#define sonyErrorClass				(oemErrorClass)
#endif

#ifndef sonyCapErrorClass
    #define sonyCapErrorClass           (sonyErrorClass | 0x0800)
#endif

#ifndef sonySysLibNameCapture
    #define sonySysFileCCaptureLib      'SlCp'  /* Capture Library creator */
    #define sonySysFileTCaptureLib      sysFileTLibrary /* 'libr' */
    #define sonySysLibNameCapture       "Sony Capture Library"
#endif

/****************************************************************************
 * If we're actually compiling the library code, then we need to
 * eliminate the trap glue that otherwise would be generated from
 * this header file.
 ***************************************************************************/
#ifdef BUILDING_CAP_LIB
    #define CAP_LIB_TRAP(trapNum)
#else
    #define CAP_LIB_TRAP(trapNum) SYS_TRAP(trapNum)
#endif

/***************************************************************************
 * version number
 ***************************************************************************/
#define capLibAPIVersion            (2)

/***************************************************************************
 * Error codes
 ***************************************************************************/
#define capLibErrorBase             (0x00)
#define capLibErrorClass            (sonyCapErrorClass | capLibErrorBase)

#define capLibErrNone               (errNone)
#define capLibErrBadParam           (capLibErrorClass | 1)  /* invalid parameter */
#define capLibErrNotOpen            (capLibErrorClass | 2)  /* library is not open */
#define capLibErrStillOpen          (capLibErrorClass | 3)  /* library still in use */
#define capLibErrNoMemory           (capLibErrorClass | 4)  /* insufficient memory */
#define capLibErrNotSupported       (capLibErrorClass | 5)  /* unsupported operation */
#define capLibErrDrvNotFound        (capLibErrorClass | 6)  /* driver is not found */
#define capLibErrDevAlreadyOpened   (capLibErrorClass | 7)  /* capture device has been opened already */

/* new for version 2 */
#define capLibErrNotInitialized     (capLibErrorClass | 8)  /* Capture Library not initialized */
#define capLibErrDevNotOpen         (capLibErrorClass | 9)  /* capture device not open */
#define capLibErrNotAllowed         (capLibErrorClass | 10) /* current operation is not allowed at this time */
#define capLibErrUnknown            (capLibErrorClass | 11) /* unknown error */

/***************************************************************************
 * Miscellaneous definitions
 ***************************************************************************/
/* for CapDevInfoType */
#define capDevInfoStringMaxLen (31)

/***************************************************************************
 * Structures and constants
 ***************************************************************************/
typedef enum
{
    cameraClass = 0
} CapDevClass;

typedef struct CapDevInfoTag
{
    UInt32 devPortID;
    CapDevClass devClass;
    Char manufactureStr[capDevInfoStringMaxLen + 1];
    Char productStr[capDevInfoStringMaxLen + 1];
    Char deviceUniqueIDStr[capDevInfoStringMaxLen + 1];
} CapDevInfoType;

typedef enum
{
    capParamIndDefault = 0, /* used to reset a setting to its default value */
    capParamIndCurrent
} CapParamIndicator;

typedef enum
{
    capExposureDirect = 0,
    capExposureAuto
} CapExposureEnum;

typedef struct CapExposureTag
{
    CapExposureEnum exposureEnum;

    /* values for direct exposure */
    #define capExposureM2   (-2)
    #define capExposureM1   (-1)
    #define capExposure00   (0)
    #define capExposureP1   (1)
    #define capExposureP2   (2)
    Int16 exposure;
} CapExposureType;

typedef enum
{
    capWBDirect = 0,
    capWBAuto,
    capWBIndoor,
    capWBOutdoor,
    capWBUnderLight     /* fluorescent lighting */
} CapWBEnum;

typedef struct CapWBTag
{
    CapWBEnum wbEnum;
    Int16 wb;
} CapWBType;

typedef enum
{
    capFocusDirect = 0,
    capFocusAuto
}   CapFocusEnum;

typedef struct CapFocusTag
{
    CapFocusEnum focusEnum;
    Int16 focus;
} CapFocusType;

/* The "posterization" effect was misnamed as "solarization" in some
 * versions of the SDK.
 *
 * We'll alias one to the other for backwards-compatibility.
 */
#define capInputModeSolarization capInputModePosterization
typedef enum
{
    capInputModeDirect = 0,
    capInputModeColor,
    capInputModeBlackWhite,
    capInputModeNegative,
    capInputModeSepia,
    capInputModePosterization
} CapInputModeEnum;

typedef struct CapInputModeTag
{
    CapInputModeEnum inputModeEnum;
    Int16 inputMode;
} CapInputModeType;

typedef enum
{
    capZoomDirect = 0
} CapZoomEnum;

typedef struct
{
    CapZoomEnum zoomEnum;
    Int16 zoom;
} CapZoomType;

typedef enum
{
    w80xh60 = 0,
    w160xh120,
    w176xh144,
    QCIFsize = w176xh144, /* this was incorrectly named "QCIF" in the previous version */
    w320xh240,
    QVGAsize = w320xh240,
    w352xh288,
    CIFsize = w352xh288,
    w640xh480,
    VGAsize = w640xh480,
    w800xh600,
    w1024xh768,
    w1600xh1200,
    capFrameSizeLast
} CapFrameSize;

typedef enum
{
#if 0 /* unsupported */
    blackWhite = 0,
    grayScale2bit,
    grayScale4bit,
    color8bit,
#endif
    color16bit = 4
} CapCaptureFormat;

typedef struct
{
    UInt16 power : 1;       /* 0:off, 1:on */
    UInt16 initialize : 1;
    UInt16 preview : 1;
    UInt16 captureReady : 1;
    UInt16 capturing : 1;
    UInt16 rsvFlag : 3;
    UInt16 mirror : 1;      /* 0:normal, 1:mirror */
    UInt16 rsvFlag2 : 7;
} CapDevStatusType, *CapDevStatusPtr;

/* new for version 2 */
/* unsupported */
typedef enum
{
    capPreviewScaleQuarter = -2,
    capPreviewScaleHalf = -1,
    capPreviewScaleNormal = 0,
    capPreviewScaleDouble
} CapPreviewScaleEnum;

/***************************************************************************
 * Capture library function trap IDs.  Each library call gets a trap number,
 * capLibTrapXXXX, which serves as an index into the library's dispatch
 * table.  The constant sysLibTrapCustom is the first available trap number
 * after the system predefined library traps Open, Close, Sleep, and Wake.
 *
 * NOTE: The order of these traps MUST match the order of the library's
 *       dispatch table!
 ****************************************************************************/
#define capLibTrapGetLibAPIVersion  (sysLibTrapCustom)
#define capLibTrapInit              (sysLibTrapCustom + 1)
#define capLibTrapDevSelect         (sysLibTrapCustom + 2)
#define capLibTrapDevOpen           (sysLibTrapCustom + 3)
#define capLibTrapDevClose          (sysLibTrapCustom + 4)
#define capLibTrapDevPowerOn        (sysLibTrapCustom + 5)
#define capLibTrapDevPowerOff       (sysLibTrapCustom + 6)
#define capLibTrapSetExposure       (sysLibTrapCustom + 7)
#define capLibTrapSetWB             (sysLibTrapCustom + 8)
#define capLibTrapSetFocus          (sysLibTrapCustom + 9)
#define capLibTrapSetInputMode      (sysLibTrapCustom + 10)
#define capLibTrapSetZoom           (sysLibTrapCustom + 11)
#define capLibTrapSetFrame          (sysLibTrapCustom + 12)
#define capLibTrapSetPreviewArea    (sysLibTrapCustom + 13)
#define capLibTrapSetCaptureArea    (sysLibTrapCustom + 14)
#define capLibTrapSetCaptureFormat  (sysLibTrapCustom + 15)
#define capLibTrapPreviewStart      (sysLibTrapCustom + 16)
#define capLibTrapPreviewStop       (sysLibTrapCustom + 17)
#define capLibTrapCaptureImage      (sysLibTrapCustom + 18)
#define capLibTrapGetExposure       (sysLibTrapCustom + 19)
#define capLibTrapGetWB             (sysLibTrapCustom + 20)
#define capLibTrapGetFocus          (sysLibTrapCustom + 21)
#define capLibTrapGetInputMode      (sysLibTrapCustom + 22)
#define capLibTrapGetZoom           (sysLibTrapCustom + 23)
#define capLibTrapGetCaptureFormat  (sysLibTrapCustom + 24)
#define capLibTrapDevGetStatus      (sysLibTrapCustom + 25)

/* new for version 2 */
#define capLibTrapSetCaptureSize    (sysLibTrapCustom + 26)
#define capLibTrapSetPreviewSize    (sysLibTrapCustom + 27)
#define capLibTrapSetPreviewScale   (sysLibTrapCustom + 28)
#define capLibTrapCaptureJPEGImage  (sysLibTrapCustom + 29)

#define capLibTrapLast              (sysLibTrapCustom + 30)

/********************************************************************
 *              Library API Prototypes
 ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------------------------------------------------
 * Standard library open, close, sleep and wake functions
 *-------------------------------------------------------------------------*/

/* open the library */
extern Err CapLibOpen(UInt16 capLibRefNum)
    CAP_LIB_TRAP(sysLibTrapOpen);

/* close the library */
extern Err CapLibClose(UInt16 capLibRefNum)
    CAP_LIB_TRAP(sysLibTrapClose);

/* library sleep */
extern Err CapLibSleep(UInt16 capLibRefNum)
    CAP_LIB_TRAP(sysLibTrapSleep);

/* library wakeup */
extern Err CapLibWake(UInt16 capLibRefNum)
    CAP_LIB_TRAP(sysLibTrapWake);

/*--------------------------------------------------------------------------
 * Custom library API functions
 *--------------------------------------------------------------------------*/

/* Get our library API version */
extern UInt32 CapLibGetLibAPIVersion(UInt16 capLibRefNum)
    CAP_LIB_TRAP(capLibTrapGetLibAPIVersion);

extern Err CapLibInit(UInt16 capLibRefNum)
    CAP_LIB_TRAP(capLibTrapInit);

extern Err CapLibDevSelect(UInt16 capLibRefNum, CapDevClass devType, UInt32 reserved, CapDevInfoType* capDevInfoP)
    CAP_LIB_TRAP(capLibTrapDevSelect);

extern Err CapLibDevOpen(UInt16 capLibRefNum, UInt32 devPortID)
    CAP_LIB_TRAP(capLibTrapDevOpen);

extern Err CapLibDevClose(UInt16 capLibRefNum, UInt32 devPortID)
    CAP_LIB_TRAP(capLibTrapDevClose);

extern Err CapLibDevPowerOn(UInt16 capLibRefNum, UInt32 devPortID)
    CAP_LIB_TRAP(capLibTrapDevPowerOn);

extern Err CapLibDevPowerOff(UInt16 capLibRefNum, UInt32 devPortID)
    CAP_LIB_TRAP(capLibTrapDevPowerOff);

extern Err CapLibSetExposure(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapExposureType* exposureP)
    CAP_LIB_TRAP(capLibTrapSetExposure);

extern Err CapLibSetWB(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapWBType* wbP)
    CAP_LIB_TRAP(capLibTrapSetWB);

extern Err CapLibSetFocus(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapFocusType* focusP)
    CAP_LIB_TRAP(capLibTrapSetFocus);

extern Err CapLibSetInputMode(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapInputModeType* modeP)
    CAP_LIB_TRAP(capLibTrapSetInputMode);

extern Err CapLibSetZoom(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapZoomType* zoomP)
    CAP_LIB_TRAP(capLibTrapSetZoom);

extern Err CapLibSetFrame(UInt16 capLibRefNum, UInt32 devPortID, PointType topLeft, CapFrameSize size)
    CAP_LIB_TRAP(capLibTrapSetFrame);

extern Err CapLibSetPreviewArea(UInt16 capLibRefNum, UInt32 devPortID, RectangleType* rP)
    CAP_LIB_TRAP(capLibTrapSetPreviewArea);

extern Err CapLibSetCaptureArea(UInt16 capLibRefNum, UInt32 devPortID, RectangleType* rP)
    CAP_LIB_TRAP(capLibTrapSetCaptureArea);

extern Err CapLibSetCaptureFormat(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapCaptureFormat format)
    CAP_LIB_TRAP(capLibTrapSetCaptureFormat);

extern Err CapLibPreviewStart(UInt16 capLibRefNum, UInt32 devPortID)
    CAP_LIB_TRAP(capLibTrapPreviewStart);

extern Err CapLibPreviewStop(UInt16 capLibRefNum, UInt32 devPortID)
    CAP_LIB_TRAP(capLibTrapPreviewStop);

extern Err CapLibCaptureImage(UInt16 capLibRefNum, UInt32 devPortID, UInt16* imageP)
    CAP_LIB_TRAP(capLibTrapCaptureImage);

extern Err CapLibGetExposure(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapExposureType* exposureP)
    CAP_LIB_TRAP(capLibTrapGetExposure);

extern Err CapLibGetWB(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapWBType* wbP)
    CAP_LIB_TRAP(capLibTrapGetWB);

extern Err CapLibGetFocus(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapFocusType* focusP)
    CAP_LIB_TRAP(capLibTrapGetFocus);

extern Err CapLibGetInputMode(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapInputModeType* modeP)
    CAP_LIB_TRAP(capLibTrapGetInputMode);

extern Err CapLibGetZoom(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapZoomType* zoomP)
    CAP_LIB_TRAP(capLibTrapGetZoom);

extern Err CapLibGetCaptureFormat(UInt16 capLibRefNum, UInt32 devPortID, CapParamIndicator ind, CapCaptureFormat* formatP)
    CAP_LIB_TRAP(capLibTrapGetCaptureFormat);

extern Err CapLibGetStatus(UInt16 capLibRefNum, UInt32 devPortID, CapDevStatusType* statusP)
    CAP_LIB_TRAP(capLibTrapDevGetStatus);


/* the following functions are new for version 2 */

extern Err CapLibSetCaptureSize(UInt16 capLibRefNum, UInt32 devPortID, CapFrameSize size)
    CAP_LIB_TRAP(capLibTrapSetCaptureSize);

extern Err CapLibSetPreviewSize(UInt16 capLibRefNum, UInt32 devPortID, CapFrameSize size)
    CAP_LIB_TRAP(capLibTrapSetPreviewSize);

/* CapLibSetPreviewScale is NOT supported.  Calling it directly sometimes
 * can produce unexpected results.  It is intended to be only for internal
 * use but is provided here in case the Capture Library does not correctly
 * scale the preview area automatically.
 */
extern Err CapLibSetPreviewScale(UInt16 capLibRefNum, UInt32 devPortID, CapPreviewScaleEnum scale)
    CAP_LIB_TRAP(capLibTrapSetPreviewScale);

extern Err CapLibCaptureJPEGImage(UInt16 capLibRefNum, UInt32 devPortID,
                                  UInt16 volRefNum, const Char* pathNameP, FileHand fH,
                                  UInt8 quality)
    CAP_LIB_TRAP(capLibTrapCaptureJPEGImage);

#ifdef __cplusplus
}
#endif

#endif  /* __CAP_LIB_H__ */

/*
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 */

#if 0
// UnicodeToJis0208 テーブルレコード
typedef struct
{
    UInt16 size;
    UInt16 table[256];
} UnicodeToJis0208Rec;

// Jis0208ToUnicode テーブルレコード
typedef struct
{
    UInt16 code;
} Jis0208ToUnicodeRec;

// JpegLib データ領域(part1)
typedef struct
{
    UInt8 private[20];
} JpegLibData;

// JpegLib データ領域(part2)
typedef struct
{
    UInt8 private[20];
} JpegLibDecodeOptions;
#endif

#if 0
// nnDA AA view form globals
// structure:
struct nnDADataType
{
    UInt32        palmOSVersion;   // デバイスのOSバージョン
    UInt16        displayCommand;  // 表示コマンド
    FontID        fontId;          // 表示フォントＩＤ
    UInt16        scale;           // 表示倍率
    UInt32        os5density;      // PalmOS5の解像度
    RectangleType dimF;            // 表示領域のサイズ(coord領域)
    RectangleType area;            // 表示領域のサイズ(実領域)
    UInt8        *areaP;           // 表示データのポインタ
    UInt16        length;          // 表示データ長
    UInt16        startX;          // 描画開始場所
    UInt16        startY;          // 描画終了場所
    UInt16        maxX;            // 描画データ最大(X)
    UInt16        maxY;            // 描画データ最小(Y)
    UInt16       *singleWidth;     // シングルバイト文字の幅
    UInt16       *doubleWidth;     // ダブルバイト文字の幅
#ifdef USE_CLIE
    UInt16        hrRef;           // SONY HR参照番号
    UInt16        hrVer;           // Sony HRのバージョン番号
#endif  // #ifdef USE_CLIE
    Char          title[nnDA_TITLESIZE + 4];  // フォームのタイトルサイズ
    UInt32        creator;
    UInt32        dataSize;
    Int32         times;
    UInt16        kanjiCode;
    UInt16         vfsVol;
    UInt32         vfsVolumeIterator;
    VolumeInfoType volInfo;  
    FileRef        vfsFileRef;
    FileHand       streamRef;
    UInt16         sourceLocation;  // JPEGデータのソースロケーション
    BitmapType    *bitmapPP;
    void          *jpegData;
    JpegLibData          data;
    JpegLibDecodeOptions dopt;
};


// NNsiとの連携機能用定義
#define nnDA_NNSIEXT_VIEWSTART    "<NNsi:NNsiExt type=\"VIEW\">"
#define nnDA_NNSIEXT_ENDVIEW      "</NNsi:NNsiExt>"
#define nnDA_NNSIEXT_INFONAME     "<NNsi:Info>"
#define nnDA_NNSIEXT_ENDINFONAME  "</NNsi:Info>"

#define nnDA_NNSIEXT_UTF8FILE     "UTF8FILE:"
#define nnDA_NNSIEXT_UTF8FILEFOLD "UTF8FOLDFILE:"
#define nnDA_NNSIEXT_BINFILE      "BFILE:"
#define nnDA_NNSIEXT_FILE         "FILE:"
#define nnDA_NNSIEXT_FILE_FOLD    "FOLDFILE:"
#define nnDA_NNSIEXT_HTMLFILE     "HTMLFILE:"
#define nnDA_NNSIEXT_HTML_FOLD    "FOLDHTMLFILE:"
#define nnDA_NNSIEXT_VIEWMEM      "MEM:"
#define nnDA_NNSIEXT_VIEWMEM_FOLD "FOLDMEM:"
#define nnDA_NNSIEXT_HELPLIST     "HELP:LIST"
#define nnDA_NNSIEXT_HELPVIEW     "HELP:VIEW"
#define nnDA_NNSIEXT_HELPUSERTAB  "HELP:FAVORTAB"
#define nnDA_NNSIEXT_HELPNNSISET  "HELP:NNSISET"
#define nnDA_NNSIEXT_HELPGETLOG   "HELP:GETLOG"
#define nnDA_NNSIEXT_SHOWJPEG     "SHOWJPEG:"
#define nnDA_NNSIEXT_VFSFILE      "VFSFILE:"
#define nnDA_NNSIEXT_VFSFILE_FOLD "FOLDVFS:"

#define nnDA_TITLE_HELP_LIST      "一覧画面操作説明"
#define nnDA_TITLE_HELP_VIEW      "参照画面操作説明"
#define nnDA_TITLE_HELP_USERTAB   "ユーザタブ説明"
#define nnDA_TITLE_HELP_NNSISET   "NNsi設定説明"
#define nnDA_TITLE_HELP_GETLOG    "参照ログ一覧画面操作説明"

#define nnDA_DISPCOMMAND_NOTHING  0   // コマンド指定なし
#define nnDA_DISPCOMMAND_FOLD     1   // 端っこで折り返し
#define nnDA_DISPCOMMAND_JPEGINFO 2   // JPEG(情報表示)
#define nnDA_DISPCOMMAND_JPEGDISP 3   // JPEG(画像表示)

#define nnDA_JPEGSOURCE_MEMORY     0  // JPEGデータはmemory
#define nnDA_JPEGSOURCE_FILESTREAM 1  // JPEGデータはfile stream
#define nnDA_JPEGSOURCE_VFS        2  // JPEGデータはVFS

/**********  メッセージパース用  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

/********** 漢字コード **********/
#define NNSH_KANJICODE_SHIFTJIS     0x00
#define NNSH_KANJICODE_EUC          0x01
#define NNSH_KANJICODE_JIS          0x02
#define NNSH_KANJICODE_UTF8         0x03
#define nnDA_KANJICODE_RAW          0xff

#endif

/*
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 */
