#ifndef __UTILS_H__
#define __UTILS_H__

    #include <Windows.h>

    BOOL IsElevated();
    int findFirstProc(const char *procname);
    void GetDestination(char destination[MAX_PATH]);
    void Starter(char path[], char arguments[]);
    void StartProgram(char executable[], char path[], char arguments[]);

#endif