// QingTest.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "..\..\Qing\SourceCode\System\SystemTime.h"
#include "..\..\Qing\SourceCode\Example\Windows\WindowsCoreProgramming.h"
#include <iostream>


int main()
{
    HANDLE hProcess1, hProcess2, hProcess3;

    HANDLE h[3];
    h[0] = hProcess1;
    h[1] = hProcess2;
    h[2] = hProcess3;
    DWORD dw = WaitForMultipleObjects(3, h, FALSE, 5000);
    switch (dw)
    {
    case WAIT_FAILED:
        //Bad call to function(invalid handle?)
        break;
    case WAIT_TIMEOUT:
        //None of the objects became signaled within 5000 milliseconds
        break;
    case WAIT_OBJECT_0 + 0:
        //The process identified by h[0] terminated.
        break;
    case WAIT_OBJECT_0 + 1:
        //The process identified by h[1] terminated.
        break;
    case WAIT_OBJECT_0 + 2:
        //The process identified by h[2] terminated.
        break;
    }

    system("pause");
    return 0;
}
