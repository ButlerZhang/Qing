#include "SystemExample.h"

#include <iostream>
#include <dirent.h>



void SystemExample::ListFileName(const std::string &Directory) const
{
    DIR *pDIR = opendir(Directory.c_str());
    if(pDIR == NULL)    
    {
        std::cout << "Can not open " << Directory << std::endl;
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