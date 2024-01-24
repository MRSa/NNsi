/*============================================================================*
 *  $Id: local.h,v 1.1.1.1 2004/08/25 14:42:32 mrsa Exp $
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
#include "PceNativeCall.h"
#include "Clipboard.h"
#include "SslLib.h"
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
