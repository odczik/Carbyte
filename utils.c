#include <Windows.h>
#include <TlHelp32.h>

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