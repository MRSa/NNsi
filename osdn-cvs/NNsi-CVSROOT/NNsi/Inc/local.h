/*============================================================================*
 *
 * $Id: local.h,v 1.518 2012/01/10 16:27:42 mrsa Exp $
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
#include <TextServicesMgr.h>
#include <CharShiftJIS.h>
#include "SystemMgr.h"
#include "PceNativeCall.h"
#ifdef USE_GLUE
    #include "LstGlue.h"
    #include "CtlGlue.h"
    #include "FrmGlue.h"
    #include "TxtGlue.h"
    #include "FntGlue.h"
#endif
#ifdef USE_SSL
  #include "SslLib.h"
#endif
#include "nnsh.h"
#include "machine.h"
#include "symbols.h"
#include "struct.h"
#include "global.h"
#include "messages.h"
#include "sections.h"
#include "proto.h"
#include "han2zen.h"

#ifdef USE_TSPATCH
  // see http://www.jade.dti.ne.jp/~imazeki/palm/TSph/index.html
  #include "SmallFontSupport.h"
#endif


/***  NNsh Informations ***/
#define SOFT_VERSION          "Version1.28"
#define SOFT_REVISION         "Revision000pre11"
#define SOFT_DATE             "Jan/11/2012"
#define SOFT_INFO             " "

#define SOFT_CREATOR_ID       'NNsi'
