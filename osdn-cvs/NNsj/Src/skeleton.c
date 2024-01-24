/*============================================================================*
 *  FILE: 
 *     skeleton.c
 *
 *  Description: 
 *     a program skeleton for Palm application.
 *
 *===========================================================================*/
#define SKELETON_C
#define GLOBAL_REAL

/********** Include Files **********/
#include "local.h"

/*** include my 'local' main routine (in "main.c") ***/
Boolean MyApplicationDispatchEvent(EventType *event);
Boolean MyApplicationHandleEvent  (EventType *event);
UInt32  MyApplicationHandleNotifications(SysNotifyParamType *notifyParamsP);
UInt16  MyStartApplication        (void);
void    MyStopApplication         (void);

/*=========================================================================*/
/*   Function : PilotMain                                                  */
/*                                                                         */
/*=========================================================================*/
UInt32 PilotMain(UInt16 cmd, void *cmdPBP, UInt16 launchFlags)
{
    UInt16    openFormID, err;
    EventType event;

    // *** This application accepts 'sysAppLaunchCmdNormalLaunch'. ***
    //                                (and 'sysAppLaunchCmdNotify')
    if (cmd != sysAppLaunchCmdNormalLaunch && cmd != sysAppLaunchCmdNotify)
    {
        return (errNone);
    }

    if(cmd == sysAppLaunchCmdNotify)
    {
       return MyApplicationHandleNotifications((SysNotifyParamType *)cmdPBP);
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

    return (errNone);
}
