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
    //创建一个Reactor
    struct event_base *base = event_base_new();
    printf("Current event base uses %s method.\n", event_base_get_method(base));

    //创建一个具体的Event Handler
    TimerEvent = event_new(base, -1, EV_PERSIST, CallBack_Timer, NULL);

    //注册事件
    struct timeval tv = { 10, 0 };
    event_add(TimerEvent, &tv);

    //进入无限循环，侦听事件
    event_base_dispatch(base);

    //释放资源
    event_free(TimerEvent);
    event_base_free(base);

    return 0;
}
