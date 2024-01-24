/*============================================================================*
 *  FILE: 
 *     skeleton.c
 *
 *  Description: 
 *     a program skeleton for Palm application.
 *
 *===========================================================================*/
#define SKELETON_C

/********** Include Files **********/
#include "local.h"

/*** include my 'local' main routine (in "main.c") ***/
static Boolean MyApplicationHandleEvent(EventType *event);
static UInt16  MyStartApplication      (void);
static void    MyStopApplication       (void);

/*=========================================================================*/
/*   Function : PilotMain                                                  */
/*                                                                         */
/*=========================================================================*/
UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    UInt16    openFormID, err;
    EventType event;

    // *** This application accepts ONLY 'sysAppLaunchCmdNormalLaunch'. ***
    if (cmd != sysAppLaunchCmdNormalLaunch)
    {
        return (errNone);
    }

    openFormID = MyStartApplication();               // Application start code
    if (openFormID != 0)
    {
        FrmGotoForm(openFormID);                  // creates form open request
    }

    // ------------------------------------------------------- MAIN Event LOOP
    do
    {
        // EvtGetEvent(&event, TimeUntilNextPeriod());
        EvtGetEvent(&event, evtWaitForever);
        if (!SysHandleEvent(&event))
        {
            if (!MenuHandleEvent(0, &event, &err))
            {
                if (!MyApplicationHandleEvent(&event))
                {
                    FrmDispatchEvent(&event);
                }
            }
        }
    } while (event.eType != appStopEvent);
    // ------------------------------------------------------- MAIN Event LOOP

    MyStopApplication();                              // Application stop code
    FrmCloseAllForms();

    return (errNone);
}

#include "main.c"
