/*============================================================================*
 *  FILE: 
 *     local.h
 *
 *  Description: 
 *     local header files for Palm application.
 *
 *===========================================================================*/
#include <PalmOS.h>
#include <SystemMgr.h>
#include <MemoryMgr.h>
#include <SysEvtMgr.h>
#include <VFSMgr.h>
#include "PceNativeCall.h"
#ifdef USE_GLUE
    #include "LstGlue.h"
    #include "CtlGlue.h"
    #include "FrmGlue.h"
    #include "TxtGlue.h"
    #include "MemGlue.h"
#endif
#include "nnsh.h"
#include "han2zen.h"
#include "machine.h"
#include "symbols.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "messages.h"
#include "sections.h"
#include "version.h"
