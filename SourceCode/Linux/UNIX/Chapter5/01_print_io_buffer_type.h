#pragma once
#include <stdio.h>

/*
 * 函数功能：对各个标准IO流打印缓冲状态信息
 *
 * 测试步骤：
 * ./a.out                                      //stdin/stdout/stderr都连接至终端
 *
 * ./a.out < /etc/group > std.out 2> std.err    //三个流都重定向，再次运行程序
 * cat std.err
 * cat std.out
*/

//标准IO库在不同的平台有不同实现

#if defined(_IO_UNBUFFERED)
int is_unbuffered(FILE *fp) {
    return fp->_flags & _IO_UNBUFFERED;
}

int is_linebuffered(FILE *fp) {
    return fp->_flags & _IO_LINE_BUF;
}

int buffer_size(FILE *fp) {
    return (int)(fp->_IO_buf_end - fp->_IO_buf_base);
}

#elif defined(__SNBF)
int is_unbuffered(FILE *fp) {
    return fp->_flags & __SNBF;
}

int is_linebuffered(FILE *fp) {
    return fp->_flags & __SLBF;
}

int buffer_size(FILE *fp) {
    return (fp->_bf._size);
}

#elif defined(_IONBF)

#ifdef  _LP64
#define _flag __pad[4]
#define _ptr  __pad[1]
#define _base __pad[2]
#endif

int is_unbuffered(FILE *fp) {
    return (fp->_flag & _IONBF);
}

int is_linebuffered(FILE *fp) {
    return (fp->_flag & _IOLBF);
}

int buffer_size(FILE *fp) {
#ifdef _LP64
    return (fp->base - fp->_ptr);
#else
    return (BUFSIZE);
#endif
}
#else
#error unknow stdio implementation!
#endif

void print_stdio(const char *name, FILE *fp) {
    printf("stream = %s, ", name);
    if (is_unbuffered(fp)) {
        printf("unbuffered");
    }
    else if (is_linebuffered(fp)) {
        printf("line buffered");
    }
    else /*if neither of above*/ {
        printf("fully buffered");
    }

    printf(", buffer size = %d\n", buffer_size(fp));
}

void print_io_buffer_type(void) {
    //fputs将string写入到stdout，需要换行符
    //fputs写入字符串时，也会为stdout分配缓冲区
    fputs("enter any character\n", stdout);

    //getchar等价于getc(stdin)
    if (getchar() == EOF) {
        printf("getchar error\n");
        return;
    }

    //fputs将string写入到stderr，需要换行符，自动分配缓冲区
    fputs("one line to standard error\n", stderr);

    //打印标准流的缓冲信息
    print_stdio("stdin", stdin);
    print_stdio("stdout", stdout);
    print_stdio("stderr", stderr);

    //打开一个普通文件
    FILE *fp;
    if ((fp = fopen("/etc/passwd", "r")) == NULL) {
        printf("fopen error\n");
        return;
    }

    if (getc(fp) == EOF) {
        printf("getc error\n");
        return;
    }

    //打印普通文件的缓冲信息
    print_stdio("/etc/passwd", fp);
}
