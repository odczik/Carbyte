#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>

BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;

    // Open the process token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        // printf("OpenProcessToken failed. Error: %lu\n", GetLastError());
        return FALSE;
    }

    // Get elevation information
    TOKEN_ELEVATION Elevation;
    DWORD cbSize = sizeof(TOKEN_ELEVATION);
    if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
        fRet = Elevation.TokenIsElevated;
    } else {
        // printf("GetTokenInformation failed. Error: %lu\n", GetLastError());
    }

    // Cleanup
    if (hToken) {
        CloseHandle(hToken);
    }

    return fRet;
}

int findFirstProc(const char *procname){
    HANDLE hSnapshot;
    PROCESSENTRY32 pe;
    int pid = 0;
    BOOL hResult;
    // snapshot of all processes in the system
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) return 0;
    // initializing size: needed for using Process32First
    pe.dwSize = sizeof(PROCESSENTRY32);
    // info about first process encountered in a system snapshot
    hResult = Process32First(hSnapshot, &pe);
    // retrieve information about the processes
    // and exit if unsuccessful
    while (hResult) {
        // if we find the process: return process ID
        if (strcmp(procname, pe.szExeFile) == 0){
            pid = pe.th32ProcessID;
            break;
        }
        hResult = Process32Next(hSnapshot, &pe);
    }

    // closes an open handle (CreateToolhelp32Snapshot)
    CloseHandle(hSnapshot);
    return pid;
}

void GetDestination(char destination[MAX_PATH]){
    // Get the destination path
    if(IsElevated()){
        // Get the system directory
        // strcpy(destination, "C:\\Windows\\system32\\");
        strcpy(destination, getenv("USERPROFILE"));
        strcat(destination, "\\");
    } else {
        // Get the user directory
        strcpy(destination, getenv("USERPROFILE"));
        strcat(destination, "\\");
    }
}

void Starter(char path[], char arguments[]){
    // Start the executable
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    // Initialize STARTUPINFO and PROCESS_INFORMATION structs
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char commandLine[128];
    snprintf(commandLine, sizeof(commandLine), "\"%s\" %s", path, arguments);
    // Create the process with no inherited handles, custom environment, and no working directory inheritance
        CreateProcessA(
            NULL,                          // Path to executable
            commandLine,                   // Command line arguments
            NULL,                          // Process security attributes
            NULL,                          // Thread security attributes
            FALSE,                         // Don't inherit handles
            DETACHED_PROCESS | CREATE_BREAKAWAY_FROM_JOB | CREATE_NO_WINDOW, // Start in a new console (optional)
            NULL,                          // No custom environment
            NULL,                          // Don't inherit current directory
            &si,                           // Startup information
            &pi);                          // Process information
    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void StartProgram(char executable[], char path[], char arguments[]){
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    // Initialize STARTUPINFO and PROCESS_INFORMATION structs
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char commandLine[128];
    snprintf(commandLine, sizeof(commandLine), "\"%s\" -starter \"%s\" %s", executable, path, arguments);
    // MessageBoxA(NULL, commandLine, "Command Line", MB_OK);
    // Create the process with no inherited handles, custom environment, and no working directory inheritance
        CreateProcessA(
            NULL,                          // Path to executable
            commandLine,                   // Command line arguments
            NULL,                          // Process security attributes
            NULL,                          // Thread security attributes
            FALSE,                         // Don't inherit handles
            DETACHED_PROCESS | CREATE_BREAKAWAY_FROM_JOB | CREATE_NO_WINDOW, // Start in a new console (optional)
            NULL,                          // No custom environment
            NULL,                          // Don't inherit current directory
            &si,                           // Startup information
            &pi);                          // Process information
    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}