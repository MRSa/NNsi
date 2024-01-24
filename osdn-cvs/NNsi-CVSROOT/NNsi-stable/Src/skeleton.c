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
static Boolean MyApplicationDispatchEvent(EventType *event);
static Boolean MyApplicationHandleEvent  (EventType *event);
static UInt32  MyApplicationHandleNotifications(SysNotifyParamType *notifyParamsP);
//static UInt32  MyApplicationReceiveData(void *cmdPBP, UInt16 launchFlags);
static UInt16  MyStartApplication(UInt16 cmd, void *cmdPBP, UInt16 launchFlags);
static void    MyStopApplication(void);
//static Boolean checkLaunchCode(UInt16 cmd, void *cmdPBP, UInt16 launchFlags);

/*=========================================================================*/
/*   Function : PilotMain                                                  */
/*                                                                         */
/*=========================================================================*/
UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    UInt16    openFormID, err;
    EventType event;


#ifdef USE_REPORTER_MAIN
    // Initiate a connection to the "tracing port".
    HostTraceInit();
    HostTraceOutputTL(appErrorClass, "<<< START APPLICATION >>>");
#endif  // #ifdef USE_REPORTER

    // *** This application accepts 'sysAppLaunchCmdNormalLaunch'. ***
    //                                (and 'sysAppLaunchCmdNotify')

    switch (cmd)
    {
      case sysAppLaunchCmdNotify:
        return (MyApplicationHandleNotifications((SysNotifyParamType *)cmdPBP));
        break;

      case sysAppLaunchCmdNormalLaunch:
        // accept
        break;

      case sysAppLaunchCmdExgReceiveData:
        // data receiving(beam)
        return (MyApplicationReceiveData(cmdPBP, launchFlags));
        break;

      default:
        // not accept
        if (checkLaunchCode(cmd, cmdPBP, launchFlags) == false)
        {
            return (errNone);
        }
        break;
    }

    openFormID = MyStartApplication(cmd, cmdPBP, launchFlags); 
                                                     // Application start code
    if (openFormID != 0)
    {
        FrmGotoForm(openFormID);                  // creates form open request
    }

    // ------------------------------------------------------- MAIN Event LOOP
    do
    {
        // EvtGetEvent(&event, TimeUntilNextPeriod());
        EvtGetEvent(&event, evtWaitForever);
        if (MyApplicationDispatchEvent(&event) == false)
        {
            if (SysHandleEvent(&event) == false)
            {
                if (MenuHandleEvent(0, &event, &err) == false)
                {
                    if (MyApplicationHandleEvent(&event) == false)
                    {
                        FrmDispatchEvent(&event);
                    }
                }
            }
        }
    } while (event.eType != appStopEvent);
    // ------------------------------------------------------- MAIN Event LOOP

    MyStopApplication();                              // Application stop code
    FrmCloseAllForms();

#ifdef USE_REPORTER_MAIN
    HostTraceOutputTL(appErrorClass, "<<< END APPLICATION >>>");

    // Close the current connection.
    HostTraceClose();
#endif  // #ifdef USE_REPORTER


    return (errNone);
}

#include "main.c"
