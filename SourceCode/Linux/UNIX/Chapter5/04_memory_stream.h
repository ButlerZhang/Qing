#pragma once
#include <stdio.h>
#include <string.h>

/*
 * 函数功能：观察内存流的写入操作
 *
 * 测试步骤：
 * ./a.out
*/

void memory_stream(void) {
    const int BSZ = 48;
    char buf[BSZ];

    memset(buf, 'a', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'x';

    //fmemopen函数允许调用者提供缓冲区用于内存流
    FILE *fp;
    if ((fp = fmemopen(buf, BSZ, "w+")) == NULL) {
        printf("fmemopen failed.\n");
        return;
    }

    printf("initial buffer contents: %s\n", buf);
    fprintf(fp, "hello, world");
    printf("before flush: %s\n", buf);
    fflush(fp);
    printf("after fflush: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    memset(buf, 'b', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'x';
    fprintf(fp, "hello, world");
    fseek(fp, 0, SEEK_SET);
    printf("after fseek: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));

    memset(buf, 'c', BSZ - 2);
    buf[BSZ - 2] = '\0';
    buf[BSZ - 1] = 'x';
    fprintf(fp, "hello, world");
    fclose(fp);
    printf("after fclose: %s\n", buf);
    printf("len of string in buf = %ld\n", (long)strlen(buf));
}
