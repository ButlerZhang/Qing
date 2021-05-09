#pragma once
#include <stdio.h>
#include <unistd.h>

/*
 * �������ܣ��޸ĵ�ǰ����Ŀ¼
 *
 * ���Բ��裺
 * ./a.out
 * ls -l /home/Butler/test
*/
void chdir_test(void) {
    //��Ŀ¼��Ҫ����
    const char *path = "/home/butler/Butler";

    //�޸ĵ�ǰ����Ŀ¼
    if (chdir(path) < 0) {
        printf("chdir failed.\n");
        return;
    }

    //��ȡ�µĹ���Ŀ¼
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        printf("getcwd failed.\n");
        return;
    }

    //����ж��Ƿ���ȷ
    printf("cwd = %s\n", buffer);
}
