//////////////////////////////////////////////////////////////////////////////
// $Id: a2da-defines.h,v 1.7 2004/03/14 13:22:52 mrsa Exp $
//
// a2da-defines.h
// A2DA Data structure definition file
//
// (C) 2003, NNsi project, all rights reserved.
//

#include <PalmOS.h>

// DA creator
#define A2DA_Creator 'a2DA'

// font width 
struct A2DAcharFontWidth
{
    UInt16        charCode;        // �L�����N�^�R�[�h
    UInt16        fontWidth;       // �t�H���g�̕�
};
typedef struct A2DAcharFontWidth A2DAcharFontWidth;

// A2DA AA view form globals
// structure:
struct A2DADataType
{
    UInt16        scale;           // �\���{��
    UInt32        os5density;      // PalmOS5�̉𑜓x
    RectangleType dimF;            // �\���̈�̃T�C�Y(coord�̈�)
    RectangleType area;            // �\���̈�̃T�C�Y(���̈�)
    UInt8        *areaP;           // �\���f�[�^�̃|�C���^
    UInt16        length;          // �\���f�[�^��
    UInt16        startX;          // �`��J�n�ꏊ
    UInt16        startY;          // �`��I���ꏊ
    UInt16        maxX;            // �`��f�[�^�ő�(X)
    UInt16        maxY;            // �`��f�[�^�ŏ�(Y)
    UInt16       *singleWidth;     // �V���O���o�C�g�����̕�
    UInt16       *doubleWidth;     // �_�u���o�C�g�����̕�
#ifdef USE_CLIE
    UInt16        hrRef;           // SONY HR�Q�Ɣԍ�
    UInt16        hrVer;           // Sony HR�̃o�[�W�����ԍ�
#endif  // #ifdef USE_CLIE
    UInt16        singleDbSize;
    A2DAcharFontWidth *singleDB;
    UInt16        doubleDbSize;
    A2DAcharFontWidth *doubleDB;    
};
typedef struct A2DADataType  A2DADataType;

// NNsi�̃J���[��`
#define NNSH_CHARCOLOR_ESCAPE       '\x1b'
#define NNSH_CHARCOLOR_NORMAL       '\x09'
#define NNSH_CHARCOLOR_HEADER       '\x0b'
#define NNSH_CHARCOLOR_BOLDHEADER   '\x0f'
#define NNSH_CHARCOLOR_FOOTER       '\x0c'
#define NNSH_CHARCOLOR_INFORM       '\x0e'

// global feature id:
#define ADT_FTRID       1
#define ADT_FTRID_MEM   2

// other definitions...
#define BUFSIZE   48
#define MARGIN    16

// NNsi�Ƃ̘A�g�@�\�p��`
#define A2DA_NNSIEXT_AADATASTART  "<NNsi:NNsiExt type=\"AA\">"
#define A2DA_NNSIEXT_ENDDATA      "</NNsi:NNsiExt>"
#define A2DA_NNSIEXT_SRC_STREAM   "<NNsi:Source>stream</NNsi:Source>"
#define A2DA_NNSIEXT_DATANAME     "<NNsi:Name>"
#define A2DA_NNSIEXT_ENDDATANAME  "</NNsi:Name>"
#define A2DA_NNSIEXT_NNSICREATOR  "<NNsi:Creator>NNsi</NNsi:Creator>"

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
#endif
