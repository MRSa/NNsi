/*============================================================================*
 *  $Id: local.h,v 1.2 2003/06/11 14:59:08 mrsa Exp $
 * 
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
#include "PceNativeCall.h"
#ifdef USE_GLUE
    #include "LstGlue.h"
    #include "CtlGlue.h"
    #include "FrmGlue.h"
    #include "TxtGlue.h"
#endif
#include "formID.h"
#include "formInfo.h"
#include "version.h"
#include "symbols.h"
#include "struct.h"
#include "proto.h"
