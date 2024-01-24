/*****************************************************************************
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
 /*
	File:		GenericConduitHeaders.h

	Contains:	precompiled header header

	Written by:	Chris LeCroy, Jeff Pritchard

	Change History (most recent first):

				 10/2/97	csl		xxx put comment here xxx
				 1/19/01	cwg		updated for carbon support

	To Do:
*/

 
#define USE_PRECOMPILED_MAC_HEADERS 0

#define PP_Target_Carbon 1
#define PP_Target_Classic (!PP_Target_Carbon)
#define TARGET_API_MAC_CARBON PP_Target_Carbon
#define TARGET_API_MAC_OS8 PP_Target_Classic

#define PP_Obsolete_ThrowExceptionCode 1
#define PP_Uses_Old_Integer_Types 1
#define PP_Suppress_Notes_20	1
#define PP_Suppress_Notes_21	1
#define PP_Suppress_Notes_22	1

#define	SystemSevenOrLater			1
#define OLDROUTINENAMES 			0
#define	OLDROUTINELOCATIONS			0


#define _DEBUG 1
#define _PADCOMTRACE _DEBUG

#include <ConditionalMacros.h>

#if UNIVERSAL_INTERFACES_VERSION < 0x0332
#warning "Must use Universal Interfaces 3.3.2 or greater to compile MoreFiles 1.5"
#endif

#include <ControlDefinitions.h>
#include <PP_Types.h>
