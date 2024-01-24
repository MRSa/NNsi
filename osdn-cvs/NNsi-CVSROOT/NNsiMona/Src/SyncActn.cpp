/*****************************************************************************
 *
 * Generic Conduit CSynchronizer Src File
 *
 * Copyright (c) 2000 Palm Inc. or its subsidiaries.  
 * All rights reserved.
 *
 ****************************************************************************/
#ifndef macintosh
#define ASSERT(f)          ((void)0)
#endif
#define TRACE0(sz)
#define TRACE(sz)
#include <windows.h>
#include <string.h>
#include <stdio.h>
#ifdef METROWERKS_WIN
#include <wmem.h>
#else
#include <memory.h>
#endif

#ifdef macintosh
#include <GenCndMacSupport.h>
#endif

#ifndef macintosh
#include <sys/stat.h>
#include <TCHAR.H>
#endif

#include <syncmgr.h>
#include <CPDbBMgr.h>
#include <hhmgr.h>
#include <pcmgr.h>
#include <bckupmgr.h>
#include <archive.h>
#include <Pgenerr.h>
#include <gensync.h>

SyncActionType fastHhActionArray[] = {
    {HH_ATTR_DELETED    | PC_ATTR_NO_REC,                                           ACTION_DELETE_HH},
    {HH_ATTR_DELETED    | PC_ATTR_NONE,                                             ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_DELETED,                                          ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_ARCHIVED,                                         ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_MODIFIED,                                         ACTION_WRITE_PC_TO_HH | LOG_MOD_DELETE},
    {HH_ATTR_DELETED    | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,                     ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,                     ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_NEW,                                              ACTION_SYNC_ERROR}, 

    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_NO_REC,                       ACTION_DELETE_HH},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_NONE,                         ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_DELETED,                      ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_ARCHIVED,                     ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_MODIFIED,                     ACTION_WRITE_PC_TO_HH | LOG_MOD_DELETE},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED, ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_DELETED   | PC_ATTR_MODIFIED, ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_NEW,                          ACTION_SYNC_ERROR}, 

    {HH_ATTR_ARCHIVED   | PC_ATTR_NO_REC,                                           ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_NONE,                                             ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_DELETED,                                          ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_ARCHIVED,                                         ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_MODIFIED,                                         ACTION_WRITE_PC_TO_HH | LOG_MOD_DELETE},
    {HH_ATTR_ARCHIVED   | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,                     ACTION_ARCHIVE_PC | ACTION_DELETE_HH},
    {HH_ATTR_ARCHIVED   | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,                     ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_NEW,                                              ACTION_SYNC_ERROR},

    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED  | PC_ATTR_NO_REC,                       ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED  | PC_ATTR_NONE,                         ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED  | PC_ATTR_DELETED,                      ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED  | PC_ATTR_ARCHIVED,                     ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED  | PC_ATTR_MODIFIED,                     ACTION_FAST_HH_ARCHIVE_DOUBLE_MODIFY},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED  | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED, ACTION_FAST_DOUBLE_ARCHIVE_DOUBLE_MODIFY},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED  | PC_ATTR_DELETED   | PC_ATTR_MODIFIED, ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED  | PC_ATTR_NEW,                          ACTION_SYNC_ERROR},

    {HH_ATTR_MODIFIED   | PC_ATTR_NO_REC,                                           ACTION_ADD_HH_TO_PC},
    {HH_ATTR_MODIFIED   | PC_ATTR_NONE,                                             ACTION_WRITE_HH_TO_PC},
    {HH_ATTR_MODIFIED   | PC_ATTR_DELETED,                                          ACTION_WRITE_HH_TO_PC | LOG_MOD_DELETE},
    {HH_ATTR_MODIFIED   | PC_ATTR_ARCHIVED,                                         ACTION_WRITE_HH_TO_PC | LOG_MOD_DELETE},
    {HH_ATTR_MODIFIED   | PC_ATTR_MODIFIED,                                         ACTION_FAST_DOUBLE_MODIFY}, 
    {HH_ATTR_MODIFIED   | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,                     ACTION_FAST_PC_ARCHIVE_DOUBLE_MODIFY},
    {HH_ATTR_MODIFIED   | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,                     ACTION_WRITE_HH_TO_PC | LOG_MOD_DELETE},
    {HH_ATTR_MODIFIED   | PC_ATTR_NEW,                                              ACTION_SYNC_ERROR}, 

    {HH_ATTR_NONE       | PC_ATTR_NO_REC,                                           ACTION_SYNC_ERROR}, // shouldn't happen in a Fast sync
    {HH_ATTR_NONE       | PC_ATTR_NONE,                                             ACTION_SYNC_ERROR}, // shouldn't happen in a Fast sync
    {HH_ATTR_NONE       | PC_ATTR_DELETED,                                          ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_NONE       | PC_ATTR_ARCHIVED,                                         ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_NONE       | PC_ATTR_MODIFIED,                                         ACTION_WRITE_PC_TO_HH},
    {HH_ATTR_NONE       | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,                     ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_NONE       | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,                     ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_NONE       | PC_ATTR_NEW,                                              ACTION_WRITE_PC_TO_HH},

    {HH_ATTR_NO_REC     | PC_ATTR_NO_REC,                                           ACTION_SYNC_ERROR}, // should never get this case
    {HH_ATTR_NO_REC     | PC_ATTR_NONE,                                             ACTION_SYNC_ERROR}, // shouldn't happen in a Fast sync
    {HH_ATTR_NO_REC     | PC_ATTR_DELETED,                                          ACTION_DELETE_PC},
    {HH_ATTR_NO_REC     | PC_ATTR_ARCHIVED,                                         ACTION_ARCHIVE_PC},
    {HH_ATTR_NO_REC     | PC_ATTR_MODIFIED,                                         ACTION_ADD_PC_TO_HH},
    {HH_ATTR_NO_REC     | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,                     ACTION_ARCHIVE_PC},
    {HH_ATTR_NO_REC     | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,                     ACTION_DELETE_PC},
    {HH_ATTR_NO_REC     | PC_ATTR_NEW,                                              ACTION_ADD_PC_TO_HH},

    {HH_ATTR_NEW        | PC_ATTR_NO_REC,                                           ACTION_ADD_HH_TO_PC},
    {HH_ATTR_NEW        | PC_ATTR_NONE,                                             ACTION_WRITE_HH_TO_PC},
    {HH_ATTR_NEW        | PC_ATTR_DELETED,                                          ACTION_WRITE_HH_TO_PC | LOG_MOD_DELETE},
    {HH_ATTR_NEW        | PC_ATTR_ARCHIVED,                                         ACTION_WRITE_HH_TO_PC | LOG_MOD_DELETE},
    {HH_ATTR_NEW        | PC_ATTR_MODIFIED,                                         ACTION_FAST_DOUBLE_MODIFY}, 
    {HH_ATTR_NEW        | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,                     ACTION_WRITE_HH_TO_PC | LOG_MOD_DELETE},
    {HH_ATTR_NEW        | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,                     ACTION_WRITE_HH_TO_PC | LOG_MOD_DELETE},
    {HH_ATTR_NEW        | PC_ATTR_NEW,                                              ACTION_SYNC_ERROR},

	// in case both the ARCHIVED and DELETED flag somehow get set.
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | PC_ATTR_NO_REC,                        ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | PC_ATTR_NONE,							ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | PC_ATTR_DELETED,						ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | PC_ATTR_ARCHIVED,						ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | PC_ATTR_MODIFIED,						ACTION_FAST_HH_ARCHIVE_DOUBLE_MODIFY},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,	ACTION_FAST_DOUBLE_ARCHIVE_DOUBLE_MODIFY},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | PC_ATTR_NEW,							ACTION_SYNC_ERROR},

    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | HH_ATTR_MODIFIED | PC_ATTR_NO_REC,		ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | HH_ATTR_MODIFIED | PC_ATTR_NONE,       ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | HH_ATTR_MODIFIED | PC_ATTR_DELETED,    ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED,   ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | HH_ATTR_MODIFIED | PC_ATTR_MODIFIED,   ACTION_FAST_HH_ARCHIVE_DOUBLE_MODIFY},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,	ACTION_FAST_DOUBLE_ARCHIVE_DOUBLE_MODIFY},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | HH_ATTR_MODIFIED | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED  | HH_ATTR_MODIFIED | PC_ATTR_NEW,        ACTION_SYNC_ERROR},
    
};

#define MAX_FAST_HH_ACTIONS (sizeof(fastHhActionArray)/sizeof(SyncActionType))
//////////////////////////////////

// in fast sync mode, when syncing pc records to the hh, 
// we can have a really simple table
SyncActionType fastPcActionArray[] = {
    {PC_ATTR_NO_REC,																ACTION_SYNC_ERROR},
    {PC_ATTR_NONE,																	ACTION_SYNC_ERROR},
    {PC_ATTR_DELETED,																ACTION_DELETE_HH_IGNORE_ERROR | ACTION_DELETE_PC},
    {PC_ATTR_ARCHIVED,																ACTION_DELETE_HH_IGNORE_ERROR | ACTION_ARCHIVE_PC},
    {PC_ATTR_MODIFIED,																ACTION_WRITE_PC_TO_HH},
    {PC_ATTR_ARCHIVED   | PC_ATTR_MODIFIED,											ACTION_DELETE_HH_IGNORE_ERROR | ACTION_ARCHIVE_PC},
    {PC_ATTR_DELETED    | PC_ATTR_MODIFIED,											ACTION_DELETE_HH_IGNORE_ERROR | ACTION_DELETE_PC},
    {PC_ATTR_NEW,																	ACTION_ADD_PC_TO_HH}, 
    {PC_ATTR_NEW        | PC_ATTR_MODIFIED,											ACTION_ADD_PC_TO_HH}, 
    {PC_ATTR_NEW        | PC_ATTR_ARCHIVED,											ACTION_DELETE_HH_IGNORE_ERROR | ACTION_ARCHIVE_PC}, 
    {PC_ATTR_NEW        | PC_ATTR_ARCHIVED	| PC_ATTR_MODIFIED,						ACTION_DELETE_HH_IGNORE_ERROR | ACTION_ARCHIVE_PC}, 
    {PC_ATTR_NEW        | PC_ATTR_DELETED,											ACTION_DELETE_HH_IGNORE_ERROR | ACTION_DELETE_PC}, 
    {PC_ATTR_NEW        | PC_ATTR_DELETED	| PC_ATTR_MODIFIED,						ACTION_DELETE_HH_IGNORE_ERROR | ACTION_DELETE_PC}, 
	// in case both the ARCHIVED and DELETED flag somehow get set
	{PC_ATTR_ARCHIVED   | PC_ATTR_DELETED,											ACTION_DELETE_HH_IGNORE_ERROR | ACTION_ARCHIVE_PC},
    {PC_ATTR_ARCHIVED	| PC_ATTR_DELETED	| PC_ATTR_MODIFIED,						ACTION_DELETE_HH_IGNORE_ERROR | ACTION_ARCHIVE_PC},

};

#define MAX_FAST_PC_ACTIONS (sizeof(fastPcActionArray)/sizeof(SyncActionType))



// Slow Sync Action array
SyncActionType slowHhActionArray[] = {
    {HH_ATTR_DELETED    | PC_ATTR_NO_REC,                                           ACTION_DELETE_HH},
    {HH_ATTR_DELETED    | PC_ATTR_NONE,                                             ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_DELETED,                                          ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_ARCHIVED,                                         ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_MODIFIED,                                         ACTION_WRITE_PC_TO_HH | LOG_MOD_DELETE},
    {HH_ATTR_DELETED    | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,                     ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,                     ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | PC_ATTR_NEW,                                              ACTION_SYNC_ERROR}, 

    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_NO_REC,                       ACTION_DELETE_HH},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_NONE,                         ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_DELETED,                      ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_ARCHIVED,                     ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_MODIFIED,                     ACTION_WRITE_PC_TO_HH | LOG_MOD_DELETE},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED, ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_DELETED   | PC_ATTR_MODIFIED, ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED  | PC_ATTR_NEW,                          ACTION_SYNC_ERROR}, 

    {HH_ATTR_ARCHIVED   | PC_ATTR_NO_REC,											ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_NONE                      | ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_DELETED                   | ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_ARCHIVED                  | ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_MODIFIED                  | ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_ARCHIVED| PC_ATTR_MODIFIED| ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_DELETED | PC_ATTR_MODIFIED| ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_NEW                       | ATTR_EQUAL,           ACTION_SYNC_ERROR},

    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_NO_REC             ,           ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_NONE   | ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_DELETED| ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED| ATTR_EQUAL,          ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_MODIFIED| ATTR_EQUAL,          ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED|PC_ATTR_MODIFIED| ATTR_EQUAL,   ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_DELETED |PC_ATTR_MODIFIED| ATTR_EQUAL,   ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_NEW    | ATTR_EQUAL,           ACTION_SYNC_ERROR},

    {HH_ATTR_MODIFIED   | PC_ATTR_NO_REC,											ACTION_ADD_HH_TO_PC},
    {HH_ATTR_MODIFIED   | PC_ATTR_NONE                      | ATTR_EQUAL,           ACTION_NONE},
    {HH_ATTR_MODIFIED   | PC_ATTR_DELETED                   | ATTR_EQUAL,           ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_MODIFIED   | PC_ATTR_ARCHIVED                  | ATTR_EQUAL,           ACTION_ARCHIVE_PC | ACTION_DELETE_HH},
    {HH_ATTR_MODIFIED   | PC_ATTR_MODIFIED                  | ATTR_EQUAL,           ACTION_NONE}, 
    {HH_ATTR_MODIFIED   | PC_ATTR_ARCHIVED | PC_ATTR_MODIFIED| ATTR_EQUAL,          ACTION_ARCHIVE_PC | ACTION_DELETE_HH},
    {HH_ATTR_MODIFIED   | PC_ATTR_DELETED  | PC_ATTR_MODIFIED| ATTR_EQUAL,          ACTION_DELETE_HH  | ACTION_DELETE_PC},
    {HH_ATTR_MODIFIED   | PC_ATTR_NEW                       | ATTR_EQUAL,           ACTION_SYNC_ERROR}, 

    {HH_ATTR_NONE       | PC_ATTR_NO_REC                                ,           ACTION_ADD_HH_TO_PC},
    {HH_ATTR_NONE       | PC_ATTR_NONE                      | ATTR_EQUAL,           ACTION_NONE},
    {HH_ATTR_NONE       | PC_ATTR_DELETED                   | ATTR_EQUAL,           ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_NONE       | PC_ATTR_ARCHIVED                  | ATTR_EQUAL,           ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_NONE       | PC_ATTR_MODIFIED                  | ATTR_EQUAL,           ACTION_NONE},
    {HH_ATTR_NONE       | PC_ATTR_ARCHIVED | PC_ATTR_MODIFIED| ATTR_EQUAL,          ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_NONE       | PC_ATTR_DELETED  | PC_ATTR_MODIFIED| ATTR_EQUAL,          ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_NONE       | PC_ATTR_NEW                       | ATTR_EQUAL,           ACTION_NONE},

    {HH_ATTR_NO_REC     | PC_ATTR_NO_REC,											ACTION_SYNC_ERROR}, // should never get this case
    {HH_ATTR_NO_REC     | PC_ATTR_NONE,												ACTION_ADD_PC_TO_HH},
    {HH_ATTR_NO_REC     | PC_ATTR_DELETED,											ACTION_DELETE_PC},
    {HH_ATTR_NO_REC     | PC_ATTR_ARCHIVED,											ACTION_ARCHIVE_PC},
    {HH_ATTR_NO_REC     | PC_ATTR_MODIFIED,											ACTION_ADD_PC_TO_HH},
    {HH_ATTR_NO_REC     | PC_ATTR_ARCHIVED | PC_ATTR_MODIFIED,						ACTION_ARCHIVE_PC},
    {HH_ATTR_NO_REC     | PC_ATTR_DELETED  | PC_ATTR_MODIFIED,						ACTION_DELETE_PC},
    {HH_ATTR_NO_REC     | PC_ATTR_NEW,												ACTION_ADD_PC_TO_HH},

    {HH_ATTR_NEW        | PC_ATTR_NO_REC,											ACTION_ADD_HH_TO_PC},
    {HH_ATTR_NEW        | PC_ATTR_NONE		| ATTR_EQUAL,							ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_DELETED	| ATTR_EQUAL,							ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_ARCHIVED	| ATTR_EQUAL,							ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_MODIFIED	| ATTR_EQUAL,							ACTION_SYNC_ERROR}, 
    {HH_ATTR_NEW        | PC_ATTR_ARCHIVED	| PC_ATTR_MODIFIED	| ATTR_EQUAL,		ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_DELETED	| PC_ATTR_MODIFIED	| ATTR_EQUAL,		ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_NEW		| ATTR_EQUAL,							ACTION_SYNC_ERROR},


    //=====================================================================
    // start duplication

    {HH_ATTR_ARCHIVED   | PC_ATTR_NONE                      | ATTR_NOT_EQUAL,       ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_DELETED                   | ATTR_NOT_EQUAL,       ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_ARCHIVED                  | ATTR_NOT_EQUAL,       ACTION_ARCHIVE_PC | ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_MODIFIED                  | ATTR_NOT_EQUAL,       ACTION_WRITE_PC_TO_HH},
    {HH_ATTR_ARCHIVED   | PC_ATTR_ARCHIVED| PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,       ACTION_ARCHIVE_PC | ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_DELETED | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,       ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | PC_ATTR_NEW                       | ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR},

    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_NONE   | ATTR_NOT_EQUAL,       ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_DELETED| ATTR_NOT_EQUAL,       ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED| ATTR_NOT_EQUAL,      ACTION_ARCHIVE_PC | ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,      ACTION_SLOW_DOUBLE_MODIFY | LOG_MOD_DELETE},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED|PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,   ACTION_ARCHIVE_HH_ON_PC | ACTION_ARCHIVE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_DELETED |PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,   ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED | PC_ATTR_NEW    | ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR},

    {HH_ATTR_MODIFIED   | PC_ATTR_NONE                      | ATTR_NOT_EQUAL,       ACTION_WRITE_HH_TO_PC},
    {HH_ATTR_MODIFIED   | PC_ATTR_DELETED                   | ATTR_NOT_EQUAL,       ACTION_WRITE_HH_TO_PC},
    {HH_ATTR_MODIFIED   | PC_ATTR_ARCHIVED                  | ATTR_NOT_EQUAL,       ACTION_WRITE_HH_TO_PC | LOG_MOD_DELETE},
    {HH_ATTR_MODIFIED   | PC_ATTR_MODIFIED                  | ATTR_NOT_EQUAL,       ACTION_SLOW_DOUBLE_MODIFY}, 
    {HH_ATTR_MODIFIED   | PC_ATTR_ARCHIVED | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,      ACTION_SLOW_DOUBLE_MODIFY},
    {HH_ATTR_MODIFIED   | PC_ATTR_DELETED  | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,      ACTION_WRITE_HH_TO_PC},
    {HH_ATTR_MODIFIED   | PC_ATTR_NEW                       | ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR}, 

    {HH_ATTR_NONE       | PC_ATTR_NONE                      | ATTR_NOT_EQUAL,       ACTION_SLOW_DOUBLE_MODIFY},
    {HH_ATTR_NONE       | PC_ATTR_DELETED                   | ATTR_NOT_EQUAL,       ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_NONE       | PC_ATTR_ARCHIVED                  | ATTR_NOT_EQUAL,       ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_NONE       | PC_ATTR_MODIFIED                  | ATTR_NOT_EQUAL,       ACTION_WRITE_PC_TO_HH},
    {HH_ATTR_NONE       | PC_ATTR_ARCHIVED | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,      ACTION_DELETE_HH | ACTION_ARCHIVE_PC},
    {HH_ATTR_NONE       | PC_ATTR_DELETED  | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,      ACTION_DELETE_HH | ACTION_DELETE_PC},
    {HH_ATTR_NONE       | PC_ATTR_NEW                       | ATTR_NOT_EQUAL,       ACTION_SLOW_DOUBLE_MODIFY},

    {HH_ATTR_NEW        | PC_ATTR_NONE                      | ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_DELETED                   | ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_ARCHIVED                  | ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_MODIFIED                  | ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR}, 
    {HH_ATTR_NEW        | PC_ATTR_ARCHIVED | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,      ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_DELETED | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR},
    {HH_ATTR_NEW        | PC_ATTR_NEW                       | ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR},

	// in case both the ARCHIVED and DELETED flag somehow get set
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_NO_REC,						ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_NONE	| ATTR_EQUAL,           ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_DELETED| ATTR_EQUAL,			ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_ARCHIVED| ATTR_EQUAL,			ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_MODIFIED| ATTR_EQUAL,			ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_ARCHIVED| PC_ATTR_MODIFIED| ATTR_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_DELETED | PC_ATTR_MODIFIED| ATTR_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_NEW	| ATTR_EQUAL,           ACTION_SYNC_ERROR},

    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_NO_REC,	ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_NONE   | ATTR_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_DELETED| ATTR_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED| ATTR_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_MODIFIED| ATTR_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED|PC_ATTR_MODIFIED| ATTR_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_DELETED |PC_ATTR_MODIFIED| ATTR_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_NEW    | ATTR_EQUAL,	ACTION_SYNC_ERROR},

    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_NONE	| ATTR_NOT_EQUAL,       ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_DELETED| ATTR_NOT_EQUAL,		ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_ARCHIVED| ATTR_NOT_EQUAL,		ACTION_ARCHIVE_PC | ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,		ACTION_WRITE_PC_TO_HH},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_ARCHIVED| PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,	ACTION_ARCHIVE_PC | ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_DELETED| PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| PC_ATTR_NEW	| ATTR_NOT_EQUAL,       ACTION_SYNC_ERROR},

    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_NONE   | ATTR_NOT_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_DELETED| ATTR_NOT_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED| ATTR_NOT_EQUAL,	ACTION_ARCHIVE_PC | ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,	ACTION_SLOW_DOUBLE_MODIFY | LOG_MOD_DELETE},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_ARCHIVED|PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_ARCHIVE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_DELETED|PC_ATTR_MODIFIED| ATTR_NOT_EQUAL,	ACTION_ARCHIVE_HH_ON_PC | ACTION_DELETE_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED | PC_ATTR_NEW    | ATTR_NOT_EQUAL,	ACTION_SYNC_ERROR},
};

#define MAX_SLOW_HH_ACTIONS (sizeof(slowHhActionArray)/sizeof(SyncActionType))

//////////////////////////////////

// in slow sync mode, when syncing pc records to the hh, 
// we can have a relatively simple table
SyncActionType slowPcActionArray[] = {
    {PC_ATTR_NO_REC,																ACTION_SYNC_ERROR},
    {PC_ATTR_NONE,																	ACTION_DELETE_PC}, // record should be deleted. KRM 2/6/2002//ACTION_WRITE_PC_TO_HH}, // another decision might be to delete the PC record 
																											//since it might have been deleted from the HH
    {PC_ATTR_DELETED,																ACTION_DELETE_PC},
    {PC_ATTR_ARCHIVED,																ACTION_ARCHIVE_PC},
    {PC_ATTR_MODIFIED,																ACTION_WRITE_PC_TO_HH}, // sort of a double modify case
    {PC_ATTR_ARCHIVED   | PC_ATTR_MODIFIED,											ACTION_ARCHIVE_PC},
    {PC_ATTR_DELETED    | PC_ATTR_MODIFIED,											ACTION_DELETE_PC},// sort of a double modify case
    {PC_ATTR_NEW,																	ACTION_ADD_PC_TO_HH}, 
	{PC_ATTR_NEW		| PC_ATTR_MODIFIED,											ACTION_ADD_PC_TO_HH},
	{PC_ATTR_NEW		| PC_ATTR_ARCHIVED,											ACTION_ARCHIVE_PC},
	{PC_ATTR_NEW		| PC_ATTR_ARCHIVED	| PC_ATTR_MODIFIED,						ACTION_ARCHIVE_PC},
	{PC_ATTR_NEW		| PC_ATTR_DELETED,											ACTION_DELETE_PC},
	{PC_ATTR_NEW		| PC_ATTR_DELETED	| PC_ATTR_MODIFIED,						ACTION_DELETE_PC},
	// in case both the ARCHIVED and DELETED flag somehow get set
	{PC_ATTR_ARCHIVED	| PC_ATTR_DELETED,											ACTION_ARCHIVE_PC},
    {PC_ATTR_ARCHIVED	| PC_ATTR_DELETED	| PC_ATTR_MODIFIED,						ACTION_ARCHIVE_PC},
};

#define MAX_SLOW_PC_ACTIONS (sizeof(slowPcActionArray)/sizeof(SyncActionType))


/********************************************************/
// Desktop Overwrites HH table.
// 06/13/2000

SyncActionType pcToHhActionArray[] = {
    {PC_ATTR_NONE,																	ACTION_ADD_PC_TO_HH},
    {PC_ATTR_DELETED,																ACTION_DELETE_PC},
    {PC_ATTR_ARCHIVED,																ACTION_ARCHIVE_PC},
    {PC_ATTR_MODIFIED,																ACTION_ADD_PC_TO_HH},
    {PC_ATTR_ARCHIVED   | PC_ATTR_MODIFIED,											ACTION_ARCHIVE_PC},
    {PC_ATTR_DELETED    | PC_ATTR_MODIFIED,											ACTION_DELETE_PC},
    {PC_ATTR_NEW,																	ACTION_ADD_PC_TO_HH}, 
    {PC_ATTR_NEW        | PC_ATTR_MODIFIED,											ACTION_ADD_PC_TO_HH}, 
    {PC_ATTR_NEW        | PC_ATTR_ARCHIVED,											ACTION_ARCHIVE_PC}, 
    {PC_ATTR_NEW        | PC_ATTR_ARCHIVED  | PC_ATTR_MODIFIED,						ACTION_ARCHIVE_PC}, 
    {PC_ATTR_NEW        | PC_ATTR_DELETED,											ACTION_DELETE_PC}, 
    {PC_ATTR_NEW        | PC_ATTR_DELETED   | PC_ATTR_MODIFIED,						ACTION_DELETE_PC},	
	// in case both the ARCHIVED and DELETED flag somehow get set
	{PC_ATTR_ARCHIVED   | PC_ATTR_DELETED,											ACTION_ARCHIVE_PC},
	{PC_ATTR_ARCHIVED   | PC_ATTR_DELETED	| PC_ATTR_MODIFIED,						ACTION_ARCHIVE_PC},
};

#define MAX_PCTOHH_ACTIONS (sizeof(pcToHhActionArray)/sizeof(SyncActionType))

SyncActionType hhToPcActionArray[] = {
    {HH_ATTR_NONE,																	ACTION_ADD_HH_TO_PC},
    {HH_ATTR_DELETED,																ACTION_NONE},
    {HH_ATTR_ARCHIVED,																ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_MODIFIED,																ACTION_ADD_HH_TO_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_MODIFIED,											ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_DELETED    | HH_ATTR_MODIFIED,											ACTION_NONE},
	// in case both the ARCHIVED and DELETED flag somehow get set
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED,											ACTION_ARCHIVE_HH_ON_PC},
    {HH_ATTR_ARCHIVED   | HH_ATTR_DELETED	| HH_ATTR_MODIFIED,						ACTION_ARCHIVE_HH_ON_PC},
};

#define MAX_HHTOPC_ACTIONS (sizeof(hhToPcActionArray)/sizeof(SyncActionType))
/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  FindPcFastActions()
 *
 * Description: This method searches the Fast PC action array for actions
 *          to perform to synchronize two records when fast syncing when we are 
 *          accessing PC modified records. The action array can be much simpler 
 *          in this case since all HH modified records have already been 
 *          synchronized.
 *
 * Parameter(s): dwStatus - a DWORD specifying the status of records being
 *                      synchronized.
 *
 * Return Value(s): A DWORD specifying what types of actions need to be
 *              performed in order to synchronizes the records.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
DWORD CSynchronizer::FindPcFastActions(DWORD dwStatus)
{
    for (int iLoop = 0; iLoop < MAX_FAST_PC_ACTIONS; iLoop++){
        if (fastPcActionArray[iLoop].dwRecStatus == dwStatus)
            return fastPcActionArray[iLoop].dwRecAction;
    }
    return ACTION_NONE;

}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  FindHhSlowActions()
 *
 * Description: This method searches the slow HH action array for actions
 *          to perform to synchronize two records when slow syncing. In this case,
 *          we are accessing all HH records and comparing
 *          the records to the matching(if they exist) PC records.
 *
 * Parameter(s): dwStatus - a DWORD specifying the status of records being
 *                      synchronized.
 *
 * Return Value(s): A DWORD specifying what types of actions need to be
 *              performed in order to synchronizes the records.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
DWORD CSynchronizer::FindHhSlowActions(DWORD dwStatus)
{
    for (int iLoop = 0; iLoop < MAX_SLOW_HH_ACTIONS; iLoop++){
        if (slowHhActionArray[iLoop].dwRecStatus == dwStatus)
            return slowHhActionArray[iLoop].dwRecAction;
    }
    return ACTION_NONE;

}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  FindPcSlowActions()
 *
 * Description: This method searches the slow PC action array for actions
 *          to perform to synchronize two records when slow syncing. In this case
 *          we are accessing all PC records not already synchronized by the Slow HH
 *          synchronization.
 *
 * Parameter(s): dwStatus - a DWORD specifying the status of records being
 *                      synchronized.
 *
 * Return Value(s): A DWORD specifying what types of actions need to be
 *              performed in order to synchronizes the records.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
DWORD CSynchronizer::FindPcSlowActions(DWORD dwStatus)
{
    for (int iLoop = 0; iLoop < MAX_SLOW_PC_ACTIONS; iLoop++){
        if (slowPcActionArray[iLoop].dwRecStatus == dwStatus)
            return slowPcActionArray[iLoop].dwRecAction;
    }
    return ACTION_NONE;

}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  FindHhFastActions()
 *
 * Description: This method searches the Fast HH action array for actions
 *          to perform to synchronize two records when fast syncing two 
 *          records when we are accessing HH modified records and comparing
 *          the records to the matching(if it exists) PC record.
 *
 * Parameter(s): dwStatus - a DWORD specifying the status of records being
 *                      synchronized.
 *
 * Return Value(s): A DWORD specifying what types of actions need to be
 *              performed in order to synchronizes the records.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
DWORD CSynchronizer::FindHhFastActions(DWORD dwStatus)
{
    for (int iLoop = 0; iLoop < MAX_FAST_HH_ACTIONS; iLoop++){
        if (fastHhActionArray[iLoop].dwRecStatus == dwStatus)
            return fastHhActionArray[iLoop].dwRecAction;
    }
    return ACTION_NONE;

}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  PerformActions()
 *
 * Description: This method performs the specified synchronization actions on the two records 
 *              passed to it.
 *
 * Parameter(s): dwActionList - a DWORD specifying actions to be performed 
 *                          to synchronize two records
 *               hhRec - a CPalmRecord object specifying the HH db record
 *               pcRec - a CPalmRecord object specifying the PC db record
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CSynchronizer::PerformActions(DWORD dwActionList, CPalmRecord &hhRec, CPalmRecord &pcRec)
{
    long retval = 0;
    DWORD dwAction = 1;
    DWORD dwRecID;
    DWORD dwIndex;

    for (int iStep = 0; (iStep < 32) && (!retval); iStep++){
        if (dwActionList & dwAction) {
            
            switch (dwAction) {
                case ACTION_NONE:
                    break;
                case ACTION_ARCHIVE_PC:
                    retval = ArchiveRecord(pcRec);
                    if (retval) {
                        break;
                    } // else fall through
                case ACTION_DELETE_PC:
                    retval = m_dbPC->DeleteRec(pcRec);
                    break;
                case ACTION_ARCHIVE_HH_ON_PC:
                    retval = ArchiveRecord(hhRec);
                    if (retval) {
                        break;
                    } // else fall through
                case ACTION_DELETE_HH:
                    if ((!hhRec.IsArchived()) && (!hhRec.IsDeleted())){
                        // no need to delete a record already marked deleted, unless that record
                        // will show up as a modified rec later in the sync process
                        retval = m_dbHH->DeleteRec(hhRec);
                    }
                    break;
                case ACTION_DELETE_HH_IGNORE_ERROR:
                    retval = m_dbHH->DeleteRec(pcRec);
                    if ((retval) && (!IsCommsError(retval))) {
                        // if there is an error and it is
                        // non-fatal, then ignore it.
                        retval = 0;
                    }
                    break;
                case ACTION_ADD_PC_TO_HH:
                    dwRecID = pcRec.GetID();
                    dwIndex = pcRec.GetIndex();
                    retval = m_dbHH->AddNewRecord(pcRec);
                    if (retval) {
                        m_pLog->RecordAddFailure(retval, pcRec);
                    } else {
                        if (dwRecID != pcRec.GetID()){
                            retval = m_dbPC->ModifyRecByIndex(dwIndex, pcRec);
                        }
                    }
                    break;
                case ACTION_ADD_HH_TO_PC:
                    retval = m_dbPC->AddNewRecord(hhRec);
                    if (retval) {
                        m_pLog->RecordAddFailure(retval, hhRec, FALSE);
                    }
                    break;
                case ACTION_WRITE_HH_TO_PC:
                    retval = m_dbPC->ModifyRecByIndex(pcRec.GetIndex(), hhRec);
                    break;
                case ACTION_WRITE_PC_TO_HH:
                    retval = m_dbHH->ModifyRecByIndex(hhRec.GetIndex(), pcRec);
                    break;

                // double modifies 
                case ACTION_FAST_PC_ARCHIVE_DOUBLE_MODIFY:
                    retval = DoFastPCArchiveDoubleModified( hhRec, pcRec);
                    break;
                case ACTION_FAST_HH_ARCHIVE_DOUBLE_MODIFY:
                    retval = DoFastHHArchiveDoubleModified( hhRec, pcRec);
                    break;
                case ACTION_FAST_DOUBLE_ARCHIVE_DOUBLE_MODIFY:
                    retval = DoFastDoubleArchiveDoubleModified( hhRec, pcRec);
                    break;
                case ACTION_FAST_DOUBLE_MODIFY:
                    retval = DoFastDoubleModified( hhRec, pcRec);
                    break;
                case ACTION_SLOW_DOUBLE_MODIFY:// change PC to new record and write to HH, write existing HH to PC
                    retval = DoSlowDoubleModified(hhRec, pcRec);
                    break;

                case LOG_MOD_DELETE:
                    m_pLog->ReverseDelete(hhRec);
                    break;
                case ACTION_SYNC_ERROR:
                    // TODO
                    break;

            }
        }
        dwAction *= 2;
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  DoFastDoubleArchiveDoubleModified()
 *
 * Description: This method completes the synchronization work needed for a record that has
 *              been archived and modified on both databases.
 *
 * Parameter(s): hhRec - a CPalmRecord object specifying the HH db record
 *               pcRec - a CPalmRecord object specifying the PC db record
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CSynchronizer::DoFastDoubleArchiveDoubleModified( CPalmRecord &hhRec, CPalmRecord &pcRec)
{
    long retval = 0;
    //if (hhRec.Compare(pcRec) == eEqual) { // records are identical
    if (CompareRecords(hhRec, pcRec) == eEqual) { 
        // archive the PC record, delete both
        retval = ArchiveRecord(pcRec); 
        if (!retval) {
            // we don't need this call since the hh rec is already marked archived.
            // the purge deleted records will erase this record.
            //retval = m_dbHH->DeleteRec(hhRec);
            //if (retval)
            //    return retval;

            // the same would be true for this call, but we dont want to hit this record when processing the
            // PC db changes, so just delete it.
            retval = m_dbPC->DeleteRec(hhRec);
        }
    } else {
        // archive both, delete both
        retval = ArchiveRecord(pcRec); 
        if (retval)
            return retval;
        retval = ArchiveRecord(hhRec); 
        if (retval)
            return retval;
        // we don't need this call since the hh rec is already marked archived.
        // the purge deleted records will erase this record.
        //retval = m_dbHH->DeleteRec(hhRec); 
        //if (retval)
        //    return retval;

        // the same would be true for this call, but we dont want to hit this record when processing the
        // PC db changes, so just delete it.
        retval = m_dbPC->DeleteRec(pcRec); 
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  DoFastPCArchiveDoubleModified()
 *
 * Description: This method completes the synchronization work needed for a record that has
 *              been archived on the PC and modified on both databases.
 *
 * Parameter(s): hhRec - a CPalmRecord object specifying the HH db record
 *               pcRec - a CPalmRecord object specifying the PC db record
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CSynchronizer::DoFastPCArchiveDoubleModified( CPalmRecord &hhRec, CPalmRecord &pcRec)
{
    long retval = 0;
    //if (hhRec.Compare(pcRec) == eEqual) { // records are identical
    if (CompareRecords(hhRec, pcRec) == eEqual) { 
        // archive pc record, delete both
        retval = ArchiveRecord(pcRec); 
        if (!retval) {
            // we don't need this call since the hh rec is already marked archived.
            // the purge deleted records will erase this record.
            //retval = m_dbHH->DeleteRec(hhRec);
            //if (retval)
            //    return retval;

            // the same would be true for this call, but we dont want to hit this record when processing the
            // PC db changes, so just delete it.
            retval = m_dbPC->DeleteRec(hhRec);
        }
    } else {
        // at this initial point,
        // the following code is the same as DoSlowDoubleModified(CPalmRecord &hhRec, CPalmRecord &pcRec)
        // but that may change over time, so I decided to duplicate the code.
        DWORD dwIndex = pcRec.GetIndex();
    
        // modify the PC database to contain the modified HH record
        hhRec.SetIndex(dwIndex);
        hhRec.ResetAttribs();
        retval = m_dbPC->ModifyRecByIndex(dwIndex, hhRec);
        if (retval)
            return retval;

        // change the PC rec ID to 0 and add it to the HH db
        pcRec.SetID(NEW_RECORD_ID);
        retval = m_dbHH->AddNewRecord(pcRec);
        if (retval){
            m_pLog->RecordAddFailure(retval, pcRec);
            return retval;
        }

        retval = m_dbPC->AddNewRecord(pcRec);

        if (retval) {
            m_pLog->RecordAddFailure(retval, pcRec, FALSE);
            return retval;
        }

        // log the change
 	    m_pLog->DoubleModify(hhRec);
    }

    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  DoFastHHArchiveDoubleModified()
 *
 * Description: This method completes the synchronization work needed for a record that has
 *              been archived on the HH and modified on both databases.
 *
 * Parameter(s): hhRec - a CPalmRecord object specifying the HH db record
 *               pcRec - a CPalmRecord object specifying the PC db record
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CSynchronizer::DoFastHHArchiveDoubleModified( CPalmRecord &hhRec, CPalmRecord &pcRec)
{
    long retval = 0;
    //if (hhRec.Compare(pcRec) == eEqual) { // records are identical
    if (CompareRecords(hhRec, pcRec) == eEqual) { 
        // archive pc record, delete both
        retval = ArchiveRecord(pcRec); 
        if (!retval) {
            // we don't need this call since the hh rec is already marked archived.
            // the purge deleted records will erase this record.
            //retval = m_dbHH->DeleteRec(hhRec);
            //if (retval)
            //    return retval;

            // the same would be true for this call, but we dont want to hit this record when processing the
            // PC db changes, so just delete it.
            retval = m_dbPC->DeleteRec(hhRec);
        }
    } else {
        DWORD dwIndex;
        dwIndex = hhRec.GetIndex();
        hhRec.ResetAttribs();
        // undelete the HH record
        retval = m_dbHH->ModifyRecByIndex(dwIndex, hhRec);
        // at this initial point,
        // the following code is the same as DoSlowDoubleModified(CPalmRecord &hhRec, CPalmRecord &pcRec)
        // but that may change over time, so I decided to duplicate the code.
        dwIndex = pcRec.GetIndex();
    
        // modify the PC database to contain the modified HH record
        hhRec.SetIndex(dwIndex);
        hhRec.ResetAttribs();
        retval = m_dbPC->ModifyRecByIndex(dwIndex, hhRec);
        if (retval)
            return retval;

        // change the PC rec ID to 0 and add it to the HH db
        pcRec.SetID(NEW_RECORD_ID);
        retval = m_dbHH->AddNewRecord(pcRec);
        if (retval) {
            m_pLog->RecordAddFailure(retval, pcRec);
            return retval;
        }

        retval = m_dbPC->AddNewRecord(pcRec);

        if (retval) {
            m_pLog->RecordAddFailure(retval, pcRec, FALSE);
            return retval;
        }

        // log the change
 	    m_pLog->DoubleModify(hhRec);
    }

    return retval;
}


/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  DoSlowDoubleModified()
 *
 * Description: This method completes the synchronization work needed for a record that has
 *              been modified on both databases. This method has less work to do, compared
 *              to DoFastDoubleModified() since the records have already been compared in 
 *              a slow sync.
 *
 * Parameter(s): hhRec - a CPalmRecord object specifying the HH db record
 *               pcRec - a CPalmRecord object specifying the PC db record
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CSynchronizer::DoSlowDoubleModified(CPalmRecord &hhRec, CPalmRecord &pcRec)
{
    // change PC to new record and write to HH, write existing HH to PC
    DWORD dwIndex = pcRec.GetIndex();
    long retval = 0;
    
    // modify the PC database to contain the modified HH record
    hhRec.SetIndex(dwIndex);
    hhRec.ResetAttribs();
    retval = m_dbPC->ModifyRecByIndex(dwIndex, hhRec);
    if (retval)
        return retval;

    // change the PC rec ID to 0 and add it to the HH db
    pcRec.SetID(NEW_RECORD_ID);
    retval = m_dbHH->AddNewRecord(pcRec);
    if (retval) {
        m_pLog->RecordAddFailure(retval, pcRec);
        return retval;
    }

    retval = m_dbPC->AddNewRecord(pcRec);

    if (retval) {
        m_pLog->RecordAddFailure(retval, pcRec, FALSE);
        return retval;
    }

    // log the change
 	m_pLog->DoubleModify(hhRec);

    return retval;

}


/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  DoFastDoubleModified()
 *
 * Description: This method completes the synchronization work needed for a record that has
 *              been modified on both databases. 
 *
 * Parameter(s): hhRec - a CPalmRecord object specifying the HH db record
 *               pcRec - a CPalmRecord object specifying the PC db record
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CSynchronizer::DoFastDoubleModified(CPalmRecord &hhRec, CPalmRecord &pcRec)  
{
    long retval = 0;
    //if (hhRec.Compare(pcRec) != eEqual) { // records are identical
    if (CompareRecords(hhRec, pcRec) != eEqual) { 
        retval = DoSlowDoubleModified(hhRec, pcRec);
    }
    return retval;
}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  FindPcToHhActions()
 *
 * Description: This method searches the PCtoHH action array for actions
 *          to perform to synchronize two records when the PC database overwrites
 *          the HH Database. The action array can be much simpler 
 *          in this case since the hh flags don't exist.
 *
 * Parameter(s): dwStatus - a DWORD specifying the status of records being
 *                      synchronized.
 *
 * Return Value(s): A DWORD specifying what types of actions need to be
 *              performed in order to synchronizes the records.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  06/13/2000    KRM         initial revision
 *
 *****************************************************************************/
DWORD CSynchronizer::FindPcToHhActions(DWORD dwStatus)
{
    for (int iLoop = 0; iLoop < MAX_PCTOHH_ACTIONS; iLoop++){
        if (pcToHhActionArray[iLoop].dwRecStatus == dwStatus)
            return pcToHhActionArray[iLoop].dwRecAction;
    }
    return ACTION_NONE;

}

/******************************************************************************
 *
 * Class:   CSynchronizer
 *
 * Method:  FindHhToPcActions()
 *
 * Description: This method searches the HhToPc action array for actions
 *          to perform to synchronize two records when the HH database overwrites
 *          the Pc Database. The action array can be much simpler 
 *          in this case since the Pc flags don't exist.
 *
 * Parameter(s): dwStatus - a DWORD specifying the status of records being
 *                      synchronized.
 *
 * Return Value(s): A DWORD specifying what types of actions need to be
 *              performed in order to synchronizes the records.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  06/13/2000    KRM         initial revision
 *
 *****************************************************************************/
DWORD CSynchronizer::FindHhToPcActions(DWORD dwStatus)
{
    for (int iLoop = 0; iLoop < MAX_PCTOHH_ACTIONS; iLoop++){
        if (hhToPcActionArray[iLoop].dwRecStatus == dwStatus)
            return hhToPcActionArray[iLoop].dwRecAction;
    }
    return ACTION_NONE;

}

eRecCompare CSynchronizer::CompareRecords(CPalmRecord &rec1, CPalmRecord &rec2)
{
    return rec1.Compare(rec2);
}