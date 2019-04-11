#pragma once
#include <string>



class SystemExample
{
public:


    void ShowProcessInformation() const;
    void ExecCommandLikeShell() const;
    void HowToUseErrno() const;

private:

    static void SignalInterruptHandle(int) {printf("interrupt\n%%");}
};
