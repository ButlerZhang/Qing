#include "SystemExample.h"

#include <errno.h>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>



void SystemExample::ListFileName(const std::string &Directory) const
{
    DIR *pDIR = opendir(Directory.c_str());
    if(pDIR == NULL)    
    {
        std::cout << "Can not open directory: " << Directory << std::endl;
        return;
    }

    struct dirent *pDirent = NULL;
    while((pDirent = readdir(pDIR)) != NULL)
    {
        //printf("%s\n", pDirent->d_name);
        std::cout << pDirent->d_name << std::endl;
    }

    closedir(pDIR);
}

void SystemExample::ReadToWrite_NotBufferIO() const
{
    const int BUFFER_SIZE = 4096;
    char DataBuffer[BUFFER_SIZE];

    int ReadCount = 0;
    while((ReadCount = read(STDIN_FILENO, DataBuffer, BUFFER_SIZE)) > 0)
    {
        if(write(STDOUT_FILENO, DataBuffer, ReadCount) != ReadCount)
        {
            std::cout << "write error!" << std::endl;
        }
    }
}

void SystemExample::ReadToWrite_StandardIO() const
{
    int ch;
    while((ch = getc(stdin)) != EOF)
    {
        if(putc(ch, stdout) == EOF)
        {
            std::cout << "output error!" << std::endl;
        }
    }
}

void SystemExample::ShowProcessInformation() const
{    
    std::cout << "User ID = " <<  (long)getuid() << std::endl;
    std::cout << "Group ID = " << (long)getgid() << std::endl;
    std::cout << "Process ID = " << (long)getpid() << std::endl;
}

void SystemExample::ExecCommandLikeShell() const
{
    const int MAX_LINE = 1024;
    char CommandBuffer[MAX_LINE];
    memset(CommandBuffer, 0, MAX_LINE);

    //how to use signal function
    if(signal(SIGINT, SignalInterruptHandle) == SIG_ERR)
    {
        std::cout << "signal error." << std::endl;
    }

    printf("%% ");
    while(fgets(CommandBuffer, MAX_LINE, stdin) != NULL)   
    {
        if(CommandBuffer[strlen(CommandBuffer) - 1] == '\n')
        {
            CommandBuffer[strlen(CommandBuffer) - 1] = 0;
        }

        pid_t pid;
        if((pid = fork()) < 0)
        {
            std::cout << "fork error." << std::endl;
        }
        else if(pid == 0)   //child
        {
            std::cout << "child pid = " << pid << std::endl;
            execlp(CommandBuffer, CommandBuffer, NULL);
            std::cout << "couldn't execute : " <<  CommandBuffer << std::endl;
            exit(127);
        }

        //parent
        int ChildStatus;
        if((pid = waitpid(pid, &ChildStatus, 0)) < 0)
        {
            std::cout << "waitpid error" << std::endl;
        }

        std::cout << "Child status = " << std::to_string(ChildStatus) << std::endl;
        printf("%% ");
    }
}

void SystemExample::HowToUseErrno() const
{
    fprintf(stderr, "EACCES: %s\n", strerror(EACCES));

    errno = ENOENT;
    //perror(argv[0]); //标准惯例是传入程序名。
    perror("How to use error");
}