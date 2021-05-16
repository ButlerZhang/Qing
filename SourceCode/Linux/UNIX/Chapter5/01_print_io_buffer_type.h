#pragma once
#include <stdio.h>

/*
 * �������ܣ��Ը�����׼IO����ӡ����״̬��Ϣ
 *
 * ���Բ��裺
 * ./a.out                                      //stdin/stdout/stderr���������ն�
 *
 * ./a.out < /etc/group > std.out 2> std.err    //���������ض����ٴ����г���
 * cat std.err
 * cat std.out
*/

//��׼IO���ڲ�ͬ��ƽ̨�в�ͬʵ��

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
    //fputs��stringд�뵽stdout����Ҫ���з�
    //fputsд���ַ���ʱ��Ҳ��Ϊstdout���仺����
    fputs("enter any character\n", stdout);

    //getchar�ȼ���getc(stdin)
    if (getchar() == EOF) {
        printf("getchar error\n");
        return;
    }

    //fputs��stringд�뵽stderr����Ҫ���з����Զ����仺����
    fputs("one line to standard error\n", stderr);

    //��ӡ��׼���Ļ�����Ϣ
    print_stdio("stdin", stdin);
    print_stdio("stdout", stdout);
    print_stdio("stderr", stderr);

    //��һ����ͨ�ļ�
    FILE *fp;
    if ((fp = fopen("/etc/passwd", "r")) == NULL) {
        printf("fopen error\n");
        return;
    }

    if (getc(fp) == EOF) {
        printf("getc error\n");
        return;
    }

    //��ӡ��ͨ�ļ��Ļ�����Ϣ
    print_stdio("/etc/passwd", fp);
}
