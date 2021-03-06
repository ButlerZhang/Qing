#pragma once
#include "Chapter1/list_file_name.h"
#include "Chapter1/copy_stdin_stdout_nobuffer.h"
#include "Chapter1/copy_stdin_stdout_standard.h"
#include "Chapter1/display_process_information.h"
#include "Chapter1/exec_command_like_shell.h"
#include "Chapter1/print_errno_message.h"
#include "Chapter1/exec_command_like_shell_add_signal.h"



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
