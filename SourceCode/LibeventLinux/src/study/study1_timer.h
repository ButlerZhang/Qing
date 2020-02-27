#include <event.h>

struct event *TimerEvent = NULL;

void CallBack_Timer(evutil_socket_t fd, short event, void *arg)
{
    printf("Please wakeup./n");

    struct timeval tv = { 10, 0 };
    event_add(TimerEvent, &tv);
}

int study1_timer(int argc, char *argv[])
{
    //����һ��Reactor
    struct event_base *base = event_base_new();
    printf("Current event base uses %s method.\n", event_base_get_method(base));

    //����һ�������Event Handler
    TimerEvent = event_new(base, -1, EV_PERSIST, CallBack_Timer, NULL);

    //ע���¼�
    struct timeval tv = { 10, 0 };
    event_add(TimerEvent, &tv);

    //��������ѭ���������¼�
    event_base_dispatch(base);

    //�ͷ���Դ
    event_free(TimerEvent);
    event_base_free(base);

    return 0;
}
