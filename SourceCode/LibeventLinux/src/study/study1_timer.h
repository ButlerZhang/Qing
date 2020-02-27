#include <iostream>
#include <event.h>



void CallBack_Timer(evutil_socket_t fd, short event, void *arg)
{
    printf("Timer wakeup/n");

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 10;
    event_add((struct event*)arg, &tv);
}

int study1_timer(int argc, char *argv[])
{
    struct event_base *base = event_base_new();

    printf("Current event base uses %s method.\n", event_base_get_method(base));

    struct event TimerEvent;
    event_assign(&TimerEvent, base, -1, EV_PERSIST, CallBack_Timer, (void*)&TimerEvent);

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 10;
    event_add(&TimerEvent, &tv);

    event_base_dispatch(base);

    return 0;
}
