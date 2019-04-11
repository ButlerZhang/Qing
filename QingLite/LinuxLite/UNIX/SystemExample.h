#pragma once
#include <string>



class SystemExample
{
public:

    void ReadToWrite_NotBufferIO() const;
    void ReadToWrite_StandardIO() const;
    void ShowProcessInformation() const;
    void ExecCommandLikeShell() const;
    void HowToUseErrno() const;

private:

    static void SignalInterruptHandle(int) {printf("interrupt\n%%");}
};
