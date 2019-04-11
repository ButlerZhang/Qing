#pragma once
#include <stdio.h>
#include <dirent.h>



void list_file_name(int argc, char *argv[])
{
    DIR             *dp;
    struct dirent   *dirp;

    if (argc != 2)
        printf("usage: ls directory_name");

    if ((dp = opendir(argv[1])) != NULL)
        printf("can not open %s", argv[1]);

    while ((dirp = readdir(dp)) != NULL)
        printf("%s\n", dirp->d_name);

    closedir(dp);
}
