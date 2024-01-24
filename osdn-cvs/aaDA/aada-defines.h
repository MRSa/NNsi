// aada-defines.h
// AADA Data structure definition file
//
// (C) 2003, NNsi project, all rights reserved.
//

#include <PalmOS.h>

// DA creator
#define AADA_Creator 'moAA'

// AADA AA insert form globals
// structure:
struct AADADataType {
    MemHandle AAItemStringsH;
    MemHandle AAItemRscH;
    MemHandle AAItemH;
    UInt16 categoryNum;
};
typedef struct AADADataType AADADataType;
typedef AADADataType * AADADataPtr;
// global feature id:
#define ADT_FTRID 1

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
