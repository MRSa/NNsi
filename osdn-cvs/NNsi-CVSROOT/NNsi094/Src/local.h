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
#ifdef USE_HANDERA
    #include <vga.h>
    #include <silk.h>
#endif
#include "SystemMgr.h"
#include "PceNativeCall.h"
#include "LstGlue.h"
#include "CtlGlue.h"
#include "nnsh.h"
#include "machine.h"
#include "symbols.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "messages.h"
#include "sections.h"

/***  NNsh Informations ***/
#define SOFT_VERSION          "Version 0.94"
#define SOFT_REVISION         "Revision 004"
#define SOFT_DATE             "Jan/11/2003"
#define SOFT_INFO             " "

#define SOFT_CREATOR_ID       'NNsi'
