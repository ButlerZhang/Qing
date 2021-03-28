#pragma once
#include "Chapter1/01_list_file_name.h"
#include "Chapter1/02_copy_stdin_stdout_nobuffer.h"
#include "Chapter1/03_copy_stdin_stdout_standard.h"
#include "Chapter1/04_display_process_information.h"
#include "Chapter1/05_exec_command_like_shell.h"
#include "Chapter1/06_print_errno_message.h"
#include "Chapter1/07_exec_command_like_shell_add_signal.h"
#include "Chapter3/01_test_stdin_is_allow_to_lseek.h"



int unix_chapter1_main(int argc, char *argv[])
{
    //list_file_name(argc, argv);
    //copy_stdin_stdout_nobuffer();
    //copy_stdin_stdout_standard();
    //display_process_information();
    //exec_command_like_shell();
    //print_errno_message(argc, argv);
    exec_command_like_shell_add_signal();

    return 0;
}

int unix_chapter3_main(int argc, char *argv[]) {
    test_stdin_is_allow_lseek();
    return 0;
}

int unix_main(int argc, char *argv[]) {
    //return unix_chapter1_main(argc, argv);
    return unix_chapter3_main(argc, argv);
}
