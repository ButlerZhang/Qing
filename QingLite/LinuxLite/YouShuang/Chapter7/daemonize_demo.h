#pragma once
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>



//daemonize_demo����ֻ������ģ��ϵͳ����int daemon(int nochadir, int noclose);
bool daemonize_demo()
{
    //�����ӽ��̣��رո����̣���������ʹ�����ں�̨����
    pid_t pid = fork();
    if (pid < 0) {
        return false;
    }
    else if (pid > 0) {
        exit(0);
    }

    //�����ļ�Ȩ�����롣�����̴������ļ���ʹ��open(const char* pathname, int flags, mode_t mode)ϵͳ
    //����ʱ���ļ���Ȩ�޽��� mode & 0777
    umask(0);

    //�����µĻỰ�����ñ�����Ϊ�����������
    pid_t sid = setsid();
    if (sid < 0) {
        return false;
    }

    //�л�����Ŀ¼
    if ((chdir("/")) < 0) {
        return false;
    }

    //�رձ�׼�����豸����׼����豸�ͱ�׼��������豸
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    //�ر������Ѿ��򿪵��ļ�������
    //TODO

    //����׼���롢��׼����ͱ�׼�������������/dev/null�ļ�
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);

    return true;
}