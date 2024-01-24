//////////////////////////////////////////////////////////////////////////////
// $Id: nnda-defines.h,v 1.18 2005/07/16 02:54:05 mrsa Exp $
//
// nnda-defines.h
// nnDA Data structure definition file
//
// (C) 2004, NNsi project, all rights reserved.
//

#include <PalmOS.h>
#include <PalmCompatibility.h>
#include <VFSMgr.h>
#include <Progress.h>
#include <FileStream.h>

// DA creator
#define nnDA_Creator    'nnDA'
#define nnDA_TITLESIZE  16
#define nnDA_DEFTITLE   "NNsi View DA"
#define nnDA_FILENAMESIZE 256

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

typedef struct {
 UInt16     source;
 Boolean    freePtr;
 UInt32     size;
 MemPtr     dataPtr;
 Boolean    grayScale;
 UInt16     scaleFactor;
} pnoJpegType, *pnoJpegPtr;

typedef struct {
 UInt16     version;            
 UInt16     source;
 Boolean    freePtr;
 UInt32     size;
 MemPtr     dataPtr;
 Boolean    grayScale;
 UInt16     scaleFactor;
 Coord      maxWidth;       
 Coord      maxHeight;      
} pnoJpeg2Type, *pnoJpeg2Ptr;

typedef struct {
 UInt16         version;
 BitmapPtr      sourceBmp;
 FileHand       fhDest;
 MemHandle      *hDest;
 MemPtr         *pDest;

 UInt16         volRefNum;
 char           *filePath;

 UInt32         quality;
 Boolean        progressive;
 UInt16         destination;
 UInt32         smoothFactor;
} pnoJpeg2EncType, *pnoJpeg2EncPtr;

// nnDA AA view form globals
// structure:
struct nnDADataType
{
    UInt32        palmOSVersion;   // デバイスのOSバージョン
    UInt16        displayCommand;  // 表示コマンド
    UInt16        editCommand;     // 加工コマンド
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
    UInt16              *singleWidth;     // シングルバイト文字の幅
    UInt16              *doubleWidth;     // ダブルバイト文字の幅
#ifdef USE_CLIE
    UInt16               hrRef;           // SONY HR参照番号
    UInt16               hrVer;           // Sony HRのバージョン番号
#endif  // #ifdef USE_CLIE
    Char                 title[nnDA_TITLESIZE + 4];  // フォームのタイトルサイズ
    Char                 fileName[nnDA_FILENAMESIZE + 4];
    UInt32               creator;
    UInt32               dataSize;
    Int32                times;
    UInt16               kanjiCode;
    UInt16               vfsVol;
    UInt32               vfsVolumeIterator;
    VolumeInfoType       volInfo;  
    FileRef              vfsFileRef;
    FileHand             streamRef;
    UInt16               sourceLocation;  // JPEGデータのソースロケーション
    UInt16               jpegRef;
    BitmapType          *bitmapPP;
    void               *jpegData;
    JpegLibData          data;
    JpegLibDecodeOptions dopt;
#ifdef USE_PNOJPEGLIB2
    pnoJpeg2Type        *pnoData;   // pnoJpegLib v2用の構造体を使う
#else
    pnoJpegType           pnoData;   // pnoJpegLib v1用の構造体を使う
#endif // #ifdef USE_PNOJPEGLIB2
    BitmapTypeV3        *bitmapPPV3;
};
typedef struct nnDADataType  nnDADataType;

// 領域の確保(ポインタ編)
#define MEMALLOC_PTR(size)    MemPtrNew(size)

// 領域のクリア(ポインタ編)
#define MEMFREE_PTR(ptr)        \
    if (ptr != NULL)            \
    {                           \
        MemPtrFree(ptr);        \
        ptr = NULL;             \
    }

// global feature id:
#define ADT_FTRID       1
#define ADT_FTRID_MEM   2

// other definitions...
#define BUFSIZE        48 + 256
#define MARGIN         16
#define BUF_MARGIN    128
#define DISP_LIMIT  10240

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

#define nnDA_EDITCOMMAND_BINDATA     16
#define nnDA_EDITCOMMAND_NORMALPARSE 17
#define nnDA_EDITCOMMAND_HTMLPARSE   18
#define nnDA_EDITCOMMAND_UTF8        19

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

/*!
 * ----------------------------------------------------------------------
 *  Yves Piguetさん作成の "jpeglib for Palm OS" を使用する設定...
 *    → http://www.nyctergatis.com/jpeglib/index.html
 *
 *    ※ CLIE(OS5?) 以外のPalmデバイスでは、"JpegLib.prc"をインストール
 *      する必要あり。
 * ----------------------------------------------------------------------
 */
#define jpegLibName    "JpegLib"
#define jpegLibCreator 'JpgL'
#define jpegLibCurrentVersion 100

Err JpegLibOpen(UInt16 refnum, UInt32 currentVersion)
	SYS_TRAP(sysLibTrapOpen);

Err JpegLibClose(UInt16 refnum, UInt16 *usecount)
	SYS_TRAP(sysLibTrapClose);

Err JpegLibRead(UInt16 refnum,
		JpegLibData const *src, JpegLibDecodeOptions const *dopt,
		UInt16 *width, UInt16 *height, UInt16 *nComp, void **data)
	SYS_TRAP(sysLibTrapCustom);

void JpegLibCreateDataFileStreaming(UInt16 refnum, JpegLibData *src, FileHand fh)
	SYS_TRAP(sysLibTrapCustom + 5);

void JpegLibCreateDataVFS(UInt16 refnum, JpegLibData *src, FileRef fileRef)
	SYS_TRAP(sysLibTrapCustom + 6);

void JpegLibCreateDecOptions(UInt16 refnum, JpegLibDecodeOptions *dopt)
	SYS_TRAP(sysLibTrapCustom + 12);

void JpegLibSetDecOptDestBitmap(UInt16 refnum, JpegLibDecodeOptions *dopt)
	SYS_TRAP(sysLibTrapCustom + 14);

void JpegLibSetDecOptScale(UInt16 refnum, JpegLibDecodeOptions *dopt, UInt16 scale)
	SYS_TRAP(sysLibTrapCustom + 16);
/*
 * ----------------------------------------------------------------------
 */

/*!
 * ----------------------------------------------------------------------
 *  Stefan Stolzさん作成の "pnoJpegLib for PalmOS"を使用する設定...
 *    → http://www.bin-people.de/pnoJpegLib/
 *
 *    ※ CLIE以外のPalmOS5デバイスでは、こちら(pnoJpegLib.prc)を
 *      インストールして使う方が高速に表示できると思います...
 * ----------------------------------------------------------------------
 */
#define pno_srcNoSource     0
#define pno_srcMemPtr       1
#define pno_srcMemHandle    2
#define pno_srcVFS          3
#define pno_srcFileStream   4   // Mobilisoft 20041230

#define pnoResize           1
#define pnoResample         2

#define destMemHandle       1
#define destMemPtr          2
#define destFileStream      3
#define destVFS             4

#define PNOJPEG_LIB_TRAP(trapNum) SYS_TRAP(trapNum)

#define     pnoJpegCreatorID    'ajLi'
#define     pnoJpegTypeID       sysFileTLibrary
#define     pnoJpegName         "pnoJpegLib"

/* invalid parameter */
#define pnoJpegErrParam     (appErrorClass | 1)     

/* library is not open */
#define pnoJpegErrNotOpen   (appErrorClass | 2)     

/* returned from pnoJpegClose() if the library is still open */
#define pnoJpegErrStillOpen (appErrorClass | 3)     
/* Standard library open, close, sleep and wake functions */

#define pnoJpegErrInvalidBitDepth (appErrorClass | 4)
#define pnoJpegInternalError      (appErrorClass | 5)

extern Err pnoJpegOpen(UInt16 refNum)PNOJPEG_LIB_TRAP(sysLibTrapOpen);
extern Err pnoJpegClose(UInt16 refNum)PNOJPEG_LIB_TRAP(sysLibTrapClose);
extern Err pnoJpegSleep(UInt16 refNum)PNOJPEG_LIB_TRAP(sysLibTrapSleep);
extern Err pnoJpegWake(UInt16 refNum)PNOJPEG_LIB_TRAP(sysLibTrapWake);

/* Custom library API functions (API Version1) */
extern Err pnoJpegCreate(UInt16 refNum, pnoJpegPtr data)PNOJPEG_LIB_TRAP(sysLibTrapBase + 5);
extern Err pnoJpegLoadFromPtr(UInt16 refNum, pnoJpegPtr data, MemPtr dataPtr, UInt32 dataSize)PNOJPEG_LIB_TRAP(sysLibTrapBase + 6);
extern Err pnoJpegLoadFromVFS(UInt16 refNum, pnoJpegPtr data, UInt16 volRefNum, char *filePath)PNOJPEG_LIB_TRAP(sysLibTrapBase + 7);
extern Err pnoJpegLoadFromHandle(UInt16 refNum, pnoJpegPtr data, MemHandle hImageData)PNOJPEG_LIB_TRAP(sysLibTrapBase + 8);
extern Err pnoJpegGetInfo(UInt16 refNum, pnoJpegPtr data, Coord *width, Coord *height)PNOJPEG_LIB_TRAP(sysLibTrapBase + 9);
extern Err pnoJpegSetScaleFactor(UInt16 refNum, pnoJpegPtr data, UInt16 factor)PNOJPEG_LIB_TRAP(sysLibTrapBase + 10);
extern Err pnoJpegSetGrayScale(UInt16 refNum, pnoJpegPtr data, Boolean grayScale)PNOJPEG_LIB_TRAP(sysLibTrapBase + 11);
extern Err pnoJpegRead(UInt16 refNum, pnoJpegPtr data, BitmapPtr *bmpPtr)PNOJPEG_LIB_TRAP(sysLibTrapBase + 12);
extern Err pnoJpegBmp2DoubleDensity(UInt16 refNum, BitmapPtr *bmp)PNOJPEG_LIB_TRAP(sysLibTrapBase + 13);

/* pnoJpeg Version 2 API */
extern Err pnoJpeg2Create(UInt16 refNum, pnoJpeg2Ptr *data)PNOJPEG_LIB_TRAP(sysLibTrapBase + 14);
extern Err pnoJpeg2LoadFromPtr(UInt16 refNum, pnoJpeg2Ptr data, MemPtr dataPtr, UInt32 dataSize)PNOJPEG_LIB_TRAP(sysLibTrapBase + 15);
extern Err pnoJpeg2LoadFromVFS(UInt16 refNum, pnoJpeg2Ptr data, UInt16 volRefNum, char *filePath)PNOJPEG_LIB_TRAP(sysLibTrapBase + 16);
extern Err pnoJpeg2LoadFromHandle(UInt16 refNum, pnoJpeg2Ptr data, MemHandle hImageData)PNOJPEG_LIB_TRAP(sysLibTrapBase + 17);
extern Err pnoJpeg2LoadFromFileStream(UInt16 refNum, pnoJpeg2Ptr data, FileHand fh)PNOJPEG_LIB_TRAP(sysLibTrapBase + 18);
extern Err pnoJpeg2GetInfo(UInt16 refNum, pnoJpeg2Ptr data, Coord *width, Coord *height)PNOJPEG_LIB_TRAP(sysLibTrapBase + 19);
extern Err pnoJpeg2SetScaleFactor(UInt16 refNum, pnoJpeg2Ptr data, UInt16 factor)PNOJPEG_LIB_TRAP(sysLibTrapBase + 20);
extern Err pnoJpeg2SetGrayscale(UInt16 refNum, pnoJpeg2Ptr data, Boolean grayscale)PNOJPEG_LIB_TRAP(sysLibTrapBase + 21);
extern Err pnoJpeg2SetMaxDimensions(UInt16 refNum, pnoJpeg2Ptr data, Coord maxWidth, Coord maxHeight)PNOJPEG_LIB_TRAP(sysLibTrapBase + 22);
extern Err pnoJpeg2Read(UInt16 refNum, pnoJpeg2Ptr data, BitmapPtr *bmpPtr)PNOJPEG_LIB_TRAP(sysLibTrapBase + 23);
extern Err pnoJpeg2Free(UInt16 refNum, pnoJpeg2Ptr *data)PNOJPEG_LIB_TRAP(sysLibTrapBase + 24);
extern UInt16 pnoJpeg2Version(UInt16 refNum)PNOJPEG_LIB_TRAP(sysLibTrapBase + 25);

extern BitmapPtr pnoJpeg2Resize(UInt16 refNum, BitmapPtr inBmp, Coord newWidth, Coord newHeight, Err *error)PNOJPEG_LIB_TRAP(sysLibTrapBase + 26);
extern BitmapPtr pnoJpeg2Resample(UInt16 refNum, BitmapPtr inBmp, Coord newWidth, Coord newHeight, Err *error)PNOJPEG_LIB_TRAP(sysLibTrapBase + 27);
extern Err pnoJpeg2Bmp2DoubleDensity(UInt16 refNum, BitmapPtr inBmp, BitmapPtrV3 *outBmp)PNOJPEG_LIB_TRAP(sysLibTrapBase + 28);
extern BitmapPtr pnoJpeg2CreateThumbnail(UInt16 refNum, BitmapPtr bmp, Coord maxWidth, Coord maxHeight, int method)PNOJPEG_LIB_TRAP(sysLibTrapBase + 29);

extern Err pnoJpeg2QuickEncode(UInt16 refNum, BitmapPtr sourceBmp, MemHandle *dest, UInt32 quality)PNOJPEG_LIB_TRAP(sysLibTrapBase + 30);

extern Err pnoJpeg2EncodeCreate(UInt16 refNum, pnoJpeg2EncPtr *data)PNOJPEG_LIB_TRAP(sysLibTrapBase + 31);
extern Err pnoJpeg2EncodeSetQuality(UInt16 refNum, pnoJpeg2EncPtr data, UInt32 quality)PNOJPEG_LIB_TRAP(sysLibTrapBase + 32);
extern Err pnoJpeg2EncodeProgressive(UInt16 refNum, pnoJpeg2EncPtr data, Boolean progressive)PNOJPEG_LIB_TRAP(sysLibTrapBase + 33);
extern Err pnoJpeg2Write(UInt16 refNum, pnoJpeg2EncPtr data, BitmapPtr source)PNOJPEG_LIB_TRAP(sysLibTrapBase + 34);
extern Err pnoJpeg2EncodeFree(UInt16 refNum, pnoJpeg2EncPtr *data)PNOJPEG_LIB_TRAP(sysLibTrapBase + 35);
extern Err pnoJpeg2EncodeToMemHandle(UInt16 refNum, pnoJpeg2EncPtr data, MemHandle *dest)PNOJPEG_LIB_TRAP(sysLibTrapBase + 36);
extern Err pnoJpeg2EncodeToMemPtr(UInt16 refNum, pnoJpeg2EncPtr data, MemPtr *dest)PNOJPEG_LIB_TRAP(sysLibTrapBase + 37);
extern Err pnoJpeg2EncodeToFileStream(UInt16 refNum, pnoJpeg2EncPtr data, FileHand fh)PNOJPEG_LIB_TRAP(sysLibTrapBase + 38);
extern Err pnoJpeg2EncodeToVFS(UInt16 refNum, pnoJpeg2EncPtr data, UInt16 volRefNum, char *filePath)PNOJPEG_LIB_TRAP(sysLibTrapBase + 39);

extern Err pnoJpeg2EncodeSetSmoothFactor(UInt16 refNum, pnoJpeg2EncPtr data, UInt32 smoothfactor)PNOJPEG_LIB_TRAP(sysLibTrapBase + 40);



/*
 * ----------------------------------------------------------------------
 *   ヘッダ部流用のため、ライセンス情報...
 * ---------------------------------------------------------------------- 
 *
 */
/*

Copyright (C) 2004, Stefan Stolz
Based on the work auf Yves Piguet and the Independent JPEG Group

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, 
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution.
    * Neither the name of the Yves Piguet nor the names of its contributors may 
      be used to endorse or promote products derived from this software without 
      specific prior written permission.


License:
THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



The Jpeg-Algorithms are provided by the Independent JPEG Group
There is their License valid.

Parts of this Software are written by Yves Piguet. For this Parts you have to follow
his License. (http://www.nyctergatis.com/jpeglib/)

Copyright (c) 2003, Yves Piguet.
All rights reserved.
Based on the work of the Independent JPEG Group.

*/
