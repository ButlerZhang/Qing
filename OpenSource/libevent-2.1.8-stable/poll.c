/*	$OpenBSD: poll.c,v 1.2 2002/06/25 15:50:15 mickey Exp $	*/

/*
 * Copyright 2000-2007 Niels Provos <provos@citi.umich.edu>
 * Copyright 2007-2012 Niels Provos and Nick Mathewson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "event2/event-config.h"
#include "evconfig-private.h"

#ifdef EVENT__HAVE_POLL

#include <sys/types.h>
#ifdef EVENT__HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/queue.h>
#include <poll.h>
#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "event-internal.h"
#include "evsignal-internal.h"
#include "log-internal.h"
#include "evmap-internal.h"
#include "event2/thread.h"
#include "evthread-internal.h"
#include "time-internal.h"

struct pollidx {
    int idxplus1;
};

struct pollop {

    //当前pollfd集合的大小。
    int event_count;        /* Highest number alloc */

    //当前文件描述符的个数。
    int nfds;               /* Highest number used */

    //判断是否需要重新分配event_set_copy集合。
    int realloc_copy;       /* True iff we must realloc event_set_copy */

    //单线程中传入poll的集合。
    struct pollfd *event_set;

    //如果启用多线程，则传入copy集合。
    struct pollfd *event_set_copy;
};

static void *poll_init(struct event_base *);
static int poll_add(struct event_base *, int, short old, short events, void *idx);
static int poll_del(struct event_base *, int, short old, short events, void *idx);
static int poll_dispatch(struct event_base *, struct timeval *);
static void poll_dealloc(struct event_base *);

const struct eventop pollops = {
    "poll",
    poll_init,
    poll_add,
    poll_del,
    poll_dispatch,
    poll_dealloc,
    0, /* doesn't need_reinit */
    EV_FEATURE_FDS,
    sizeof(struct pollidx),         //有额外数据！
};

static void *
poll_init(struct event_base *base)
{
    struct pollop *pollop;

    //创建pollop，并清零。
    if (!(pollop = mm_calloc(1, sizeof(struct pollop))))
        return (NULL);

    //初始化信号事件。
    evsig_init_(base);

    evutil_weakrand_seed_(&base->weakrand_seed, 0);

    return (pollop);
}

#ifdef CHECK_INVARIANTS
static void
poll_check_ok(struct pollop *pop)
{
	int i, idx;
	struct event *ev;

	for (i = 0; i < pop->fd_count; ++i) {
		idx = pop->idxplus1_by_fd[i]-1;
		if (idx < 0)
			continue;
		EVUTIL_ASSERT(pop->event_set[idx].fd == i);
	}
	for (i = 0; i < pop->nfds; ++i) {
		struct pollfd *pfd = &pop->event_set[i];
		EVUTIL_ASSERT(pop->idxplus1_by_fd[pfd->fd] == i+1);
	}
}
#else
#define poll_check_ok(pop)
#endif

static int
poll_dispatch(struct event_base *base, struct timeval *tv)
{
    int res, i, j, nfds;
    long msec = -1;
    struct pollop *pop = base->evbase;
    struct pollfd *event_set;

    poll_check_ok(pop);

    nfds = pop->nfds;

#ifndef EVENT__DISABLE_THREAD_SUPPORT
    if (base->th_base_lock) {
        /* If we're using this backend in a multithreaded setting,
         * then we need to work on a copy of event_set, so that we can
         * let other threads modify the main event_set while we're
         * polling. If we're not multithreaded, then we'll skip the
         * copy step here to save memory and time.
         *
         * 如果启用了多线程，那么我们需要使用event_set的副本，这样当我们阻塞在poll时，
         * 其它线程就可以修改主event_set。如果没有启用多线程，则跳过此步骤。
         */

        if (pop->realloc_copy) {
            struct pollfd *tmp = mm_realloc(pop->event_set_copy,
                pop->event_count * sizeof(struct pollfd));
            if (tmp == NULL) {
                event_warn("realloc");
                return -1;
            }
            pop->event_set_copy = tmp;
            pop->realloc_copy = 0;
        }
        memcpy(pop->event_set_copy, pop->event_set,
            sizeof(struct pollfd)*nfds);
        event_set = pop->event_set_copy;
    } else {
        event_set = pop->event_set;
    }
#else
    event_set = pop->event_set;
#endif

    //这里使用evutil_tv_to_msec_转换时间。
    if (tv != NULL) {
        msec = evutil_tv_to_msec_(tv);
        if (msec < 0 || msec > INT_MAX)
            msec = INT_MAX;
    }

    EVBASE_RELEASE_LOCK(base, th_base_lock);

    res = poll(event_set, nfds, msec);

    EVBASE_ACQUIRE_LOCK(base, th_base_lock);

    //判断poll的返回值。要注意EINTR的情况。
    if (res == -1) {
        if (errno != EINTR) {
            event_warn("poll");
            return (-1);
        }

        return (0);
    }

    event_debug(("%s: poll reports %d", __func__, res));

    if (res == 0 || nfds == 0)
        return (0);

    i = evutil_weakrand_range_(&base->weakrand_seed, nfds);
    for (j = 0; j < nfds; j++) {
        int what;
        if (++i == nfds)
            i = 0;
        what = event_set[i].revents;
        if (!what)
            continue;

        res = 0;
        //判断是什么事件处于就绪状态。
        /* If the file gets closed notify */
        if (what & (POLLHUP|POLLERR|POLLNVAL))
            what |= POLLIN|POLLOUT;
        if (what & POLLIN)
            res |= EV_READ;
        if (what & POLLOUT)
            res |= EV_WRITE;
        if (res == 0)
            continue;

        //激活对应事件的回调函数。
        evmap_io_active_(base, event_set[i].fd, res);
    }

    return (0);
}

static int
poll_add(struct event_base *base, int fd, short old, short events, void *idx_)
{
    struct pollop *pop = base->evbase;
    struct pollfd *pfd = NULL;
    struct pollidx *idx = idx_;
    int i;

    EVUTIL_ASSERT((events & EV_SIGNAL) == 0);
    if (!(events & (EV_READ|EV_WRITE)))
        return (0);

    poll_check_ok(pop);

    //判断是否需要重新分配集合。
    if (pop->nfds + 1 >= pop->event_count) {
        struct pollfd *tmp_event_set;
        int tmp_event_count;

        if (pop->event_count < 32)
            tmp_event_count = 32;
        else
            tmp_event_count = pop->event_count * 2;

        /* We need more file descriptors */
        tmp_event_set = mm_realloc(pop->event_set,
                 tmp_event_count * sizeof(struct pollfd));
        if (tmp_event_set == NULL) {
            event_warn("realloc");
            return (-1);
        }
        pop->event_set = tmp_event_set;

        pop->event_count = tmp_event_count;
        pop->realloc_copy = 1; //标记需要重新分配copy集合。
    }

    //这段代码要来干啥??
    i = idx->idxplus1 - 1;

    if (i >= 0) {
        pfd = &pop->event_set[i];
    } else {
        i = pop->nfds++;
        pfd = &pop->event_set[i];
        pfd->events = 0;
        pfd->fd = fd;
        idx->idxplus1 = i + 1;
    }

    //标记监听读写事件。
    pfd->revents = 0;
    if (events & EV_WRITE)
        pfd->events |= POLLOUT;
    if (events & EV_READ)
        pfd->events |= POLLIN;
    poll_check_ok(pop);

    return (0);
}

/*
 * Nothing to be done here.
 */

static int
poll_del(struct event_base *base, int fd, short old, short events, void *idx_)
{
    struct pollop *pop = base->evbase;
    struct pollfd *pfd = NULL;
    struct pollidx *idx = idx_;
    int i;

    EVUTIL_ASSERT((events & EV_SIGNAL) == 0);
    if (!(events & (EV_READ|EV_WRITE)))
        return (0);

    poll_check_ok(pop);
    i = idx->idxplus1 - 1;
    if (i < 0)
        return (-1);

    /* Do we still want to read or write? */
    //判断是否还需要监听读或写事件，有可能只删除某事件。
    pfd = &pop->event_set[i];
    if (events & EV_READ)
        pfd->events &= ~POLLIN;
    if (events & EV_WRITE)
        pfd->events &= ~POLLOUT;
    poll_check_ok(pop);
    if (pfd->events)
        /* Another event cares about that fd. */
        return (0);

    /* Okay, so we aren't interested in that fd anymore. */
    idx->idxplus1 = 0;

    --pop->nfds;
    if (i != pop->nfds) {
        /*
         * Shift the last pollfd down into the now-unoccupied
         * position.
         *
         * 将最后一个pollfd向下移动到未占用的位置???
         */
        memcpy(&pop->event_set[i], &pop->event_set[pop->nfds],
               sizeof(struct pollfd));
        idx = evmap_io_get_fdinfo_(&base->io, pop->event_set[i].fd);
        EVUTIL_ASSERT(idx);
        EVUTIL_ASSERT(idx->idxplus1 == pop->nfds + 1);
        idx->idxplus1 = i + 1;
    }

    poll_check_ok(pop);
    return (0);
}

static void
poll_dealloc(struct event_base *base)
{
    struct pollop *pop = base->evbase;

    evsig_dealloc_(base);
    if (pop->event_set)
        mm_free(pop->event_set);
    if (pop->event_set_copy)
        mm_free(pop->event_set_copy);

    memset(pop, 0, sizeof(struct pollop));
    mm_free(pop);
}

#endif /* EVENT__HAVE_POLL */
