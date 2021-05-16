#pragma once
#include "Chapter1/01_list_file_name.h"
#include "Chapter1/02_copy_stdin_stdout_nobuffer.h"
#include "Chapter1/03_copy_stdin_stdout_standard.h"
#include "Chapter1/04_display_process_information.h"
#include "Chapter1/05_exec_command_like_shell.h"
#include "Chapter1/06_print_errno_message.h"
#include "Chapter1/07_exec_command_like_shell_add_signal.h"

#include "Chapter3/01_test_stdin_is_allow_to_lseek.h"
#include "Chapter3/02_create_hole_file.h"
#include "Chapter3/03_print_file_flag.h"

#include "Chapter4/01_print_file_type.h"
#include "Chapter4/02_access_test.h"
#include "Chapter4/03_umask_test.h"
#include "Chapter4/04_chmod_test.h"
#include "Chapter4/05_unlink_test.h"
#include "Chapter4/06_futimens_test.h"
#include "Chapter4/07_chdir_test.h"

#include "Chapter5/01_print_io_buffer_type.h"
#include "Chapter5/02_temporary_file.h"
#include "Chapter5/03_mkstemp_test.h"



int unix_chapter1_main(int argc, char *argv[]) {
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
    //test_stdin_is_allow_lseek();
    //create_hole_file();
    print_file_flag(argc, argv);
    return 0;
}

int unix_chapter4_main(int argc, char *argv[]) {
    //print_file_type(argc, argv);
    //access_test(argc, argv);
    //umask_test();
    //chmod_test();
    //unlink_test();
    //futimens_test(argc, argv);
    chdir_test();
    return 0;
}

int unix_chapter5_main(int argc, char *argv[]) {
    //print_io_buffer_type();
    //temporary_file();
    mkstemp_test();
    return 0;
}

int unix_main(int argc, char *argv[]) {
    //return unix_chapter1_main(argc, argv);
    //return unix_chapter3_main(argc, argv);
    //return unix_chapter4_main(argc, argv);
    return unix_chapter5_main(argc, argv);
}
