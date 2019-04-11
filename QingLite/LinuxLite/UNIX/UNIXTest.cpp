#include "UNIXTest.h"
#include "SystemExample.h"

#include <iostream>



SystemExample g_TestObject;



void TestListFileName()
{
}

void TestReadToWriteNotBufferIO()
{
    g_TestObject.ReadToWrite_NotBufferIO();
}

void TestReadToWriteStandardIO()
{
    g_TestObject.ReadToWrite_StandardIO();
}

void TestShowProcessInformation()
{
    g_TestObject.ShowProcessInformation();
    std::cout << std::endl << std::endl;
}

void TestExecuteCommand()
{
    g_TestObject.ExecCommandLikeShell();
}

void TestHowToUseErrno()
{
    g_TestObject.HowToUseErrno();
}