#pragma once
#include <stdio.h>

/*
 * 函数功能：tmpnam和tmpfile函数实例
 *
 * 测试步骤：
 * ./a.out
*/

void temporary_file(void) {
    //tmpnam函数产生一个与现有文件名不同的一个有效路径名字符串；
    //每次调用它时，都产生一个不同的路径名，最多调用TMP_MAX次；
    //若传入NULL，所产生的路径名存放在一个静态区中，返回该静态区的指针；
    //后续再调用tmpnam时，会重写该静态区，因此需要自己保存返回值；
    //若传入非NULL，则必须是长度至少L_tmpnam的字符数组。
    printf("first  temp name = %s\n", tmpnam(NULL));

    char name[L_tmpnam];
    tmpnam(name);
    printf("second temp name = %s\n", name);

    //tmpfile创建一个临时二进制文件(wb+)，在关闭该文件或程序
    //结束时将自动删除这种文件。UNIX对二进制文件不进行特殊区分。
    FILE *fp;
    if ((fp = tmpfile()) == NULL) {
        printf("tmpfile error\n");
        return;
    }

    //写入一行字符串到临时文件
    fputs("one line of output\n", fp);

    //重新将流设置到起始位置
    rewind(fp);

    //把刚才写入的字符串重新读取出来
    char line[1024];
    if (fgets(line, sizeof(line), fp) == NULL) {
        printf("fgets error\n");
        return;
    }

    //输出到标准输出
    fputs(line, stdout);
}
