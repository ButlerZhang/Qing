/*	$OpenBSD: select.c,v 1.2 2002/06/25 15:50:15 mickey Exp $	*/

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

#ifdef EVENT__HAVE_SELECT

#ifdef __APPLE__
/* Apple wants us to define this if we might ever pass more than
 * FD_SETSIZE bits to select(). */
#define _DARWIN_UNLIMITED_SELECT
#endif

#include <sys/types.h>
#ifdef EVENT__HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef EVENT__HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#include <sys/queue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "event-internal.h"
#include "evsignal-internal.h"
#include "event2/thread.h"
#include "evthread-internal.h"
#include "log-internal.h"
#include "evmap-internal.h"

#ifndef EVENT__HAVE_FD_MASK
/* This type is mandatory, but Android doesn't define it. */
typedef unsigned long fd_mask;
#endif

#ifndef NFDBITS
//sizeof(fd_mask) = sizeof(unsigned long) = 8字节
#define NFDBITS (sizeof(fd_mask)*8) //=8*8=64 bits
#endif

/* Divide positive x by y, rounding up. */
#define DIV_ROUNDUP(x, y)   (((x)+((y)-1))/(y))

/* How many bytes to allocate for N fds? */
#define SELECT_ALLOC_SIZE(n) \
	(DIV_ROUNDUP(n, NFDBITS) * sizeof(fd_mask))

struct selectop {
    //监听的文件描述符的最大值，select的第一个参数。
    int event_fds;      /* Highest fd in fd set */

    //当前集合大小，以字节为单位。
    int event_fdsz;

    //是否需要重新分配传出集合。
    int resize_out_sets;

    //传入select的读写事件集合。
    fd_set *event_readset_in;
    fd_set *event_writeset_in;

    //由select返回的读写事件集合。
    fd_set *event_readset_out;
    fd_set *event_writeset_out;
};

static void *select_init(struct event_base *);
static int select_add(struct event_base *, int, short old, short events, void*);
static int select_del(struct event_base *, int, short old, short events, void*);
static int select_dispatch(struct event_base *, struct timeval *);
static void select_dealloc(struct event_base *);

const struct eventop selectops = {
    "select",
    select_init,
    select_add,
    select_del,
    select_dispatch,
    select_dealloc,
    0, /* doesn't need reinit. */
    EV_FEATURE_FDS,
    0,
};

static int select_resize(struct selectop *sop, int fdsz);
static void select_free_selectop(struct selectop *sop);

static void *
select_init(struct event_base *base)
{
    struct selectop *sop;

    //创建selectop，并清零。
    if (!(sop = mm_calloc(1, sizeof(struct selectop))))
        return (NULL);

    //SELECT_ALLOC_SIZE(32 + 1) == 8
    //为读写集合分配大小，注意，这里传入的是8字节大小。
    if (select_resize(sop, SELECT_ALLOC_SIZE(32 + 1))) {
        select_free_selectop(sop);
        return (NULL);
    }

    //初始化信号事件。
    evsig_init_(base);

    evutil_weakrand_seed_(&base->weakrand_seed, 0);

    return (sop);
}

#ifdef CHECK_INVARIANTS
static void
check_selectop(struct selectop *sop)
{
    /* nothing to be done here */
}
#else
#define check_selectop(sop) do { (void) sop; } while (0)
#endif

static int
select_dispatch(struct event_base *base, struct timeval *tv)
{
    int res=0, i, j, nfds;
    struct selectop *sop = base->evbase;

    check_selectop(sop);

    //根据标记，重新分配传出select的集合。
    if (sop->resize_out_sets) {
        fd_set *readset_out=NULL, *writeset_out=NULL;
        size_t sz = sop->event_fdsz;
        if (!(readset_out = mm_realloc(sop->event_readset_out, sz)))
            return (-1);
        sop->event_readset_out = readset_out;
        if (!(writeset_out = mm_realloc(sop->event_writeset_out, sz))) {
            /* We don't free readset_out here, since it was
             * already successfully reallocated. The next time
             * we call select_dispatch, the realloc will be a
             * no-op. */
            return (-1);
        }
        sop->event_writeset_out = writeset_out;

        //标记清零，下一次不需要重新分配。
        sop->resize_out_sets = 0;
    }

    //将原始的in集合拷贝到out集合。
    memcpy(sop->event_readset_out, sop->event_readset_in,
           sop->event_fdsz);
    memcpy(sop->event_writeset_out, sop->event_writeset_in,
           sop->event_fdsz);

    //select的第一个参数需要大于保存的最大文件描述符，select从0开始遍历。
    nfds = sop->event_fds+1;

    EVBASE_RELEASE_LOCK(base, th_base_lock);

    //这里只实现了读写事件的监听。
    res = select(nfds, sop->event_readset_out,
        sop->event_writeset_out, NULL, tv);

    EVBASE_ACQUIRE_LOCK(base, th_base_lock);

    check_selectop(sop);

    //判断select的返回值。要注意EINTR的情况。
    if (res == -1) {
        if (errno != EINTR) {
            event_warn("select");
            return (-1);
        }

        return (0);
    }

    event_debug(("%s: select reports %d", __func__, res));

    check_selectop(sop);
    i = evutil_weakrand_range_(&base->weakrand_seed, nfds);
    for (j = 0; j < nfds; ++j) {
        if (++i >= nfds)
            i = 0;

        //判断是什么事件处于就绪状态。
        res = 0;
        if (FD_ISSET(i, sop->event_readset_out))
            res |= EV_READ;
        if (FD_ISSET(i, sop->event_writeset_out))
            res |= EV_WRITE;

        if (res == 0)
            continue;

        //激活对应事件的回调函数。
        evmap_io_active_(base, i, res);
    }
    check_selectop(sop);

    return (0);
}

static int
select_resize(struct selectop *sop, int fdsz)
{
    fd_set *readset_in = NULL;
    fd_set *writeset_in = NULL;

    if (sop->event_readset_in)
        check_selectop(sop);

    if ((readset_in = mm_realloc(sop->event_readset_in, fdsz)) == NULL)
        goto error;

    //event_readset_in指向了fdsz字节大小的空间，而实际使用的是bit。
    sop->event_readset_in = readset_in;
    if ((writeset_in = mm_realloc(sop->event_writeset_in, fdsz)) == NULL) {
        /* Note that this will leave event_readset_in expanded.
         * That's okay; we wouldn't want to free it, since that would
         * change the semantics of select_resize from "expand the
         * readset_in and writeset_in, or return -1" to "expand the
         * *set_in members, or trash them and return -1."
         */
        goto error;
    }
    sop->event_writeset_in = writeset_in;

    //如果重新分配了传入集合，则需要重新分配传出集合。
    sop->resize_out_sets = 1;

    //只拷贝分配的字节数空间。例如最开始只分配了8字节的空间，则只需要拷贝8字节。
    //默认情况下，fd_set的fd_mask数组有128字节。
    memset((char *)sop->event_readset_in + sop->event_fdsz, 0,
        fdsz - sop->event_fdsz);
    memset((char *)sop->event_writeset_in + sop->event_fdsz, 0,
        fdsz - sop->event_fdsz);

    //保存读写集合占用的字节数。
    sop->event_fdsz = fdsz;
    check_selectop(sop);

    return (0);

 error:
    event_warn("malloc");
    return (-1);
}


static int
select_add(struct event_base *base, int fd, short old, short events, void *p)
{
    struct selectop *sop = base->evbase;
    (void) p;

    EVUTIL_ASSERT((events & EV_SIGNAL) == 0);
    check_selectop(sop);
    /*
     * Keep track of the highest fd, so that we can calculate the size
     * of the fd_sets for select(2)
     *
     * 跟踪fd的最大值，用来计算fd_sets的大小。
     */

    //文件描述符数是自增的，如果当前保存的最大值小于传入的
    //描述符值，则说明可能要重新分配fd_set空间。
    if (sop->event_fds < fd) {
        int fdsz = sop->event_fdsz;

        //最少都要分配sizeof(fd_mask)=8字节的空间。
        if (fdsz < (int)sizeof(fd_mask))
            fdsz = (int)sizeof(fd_mask);

        /* In theory we should worry about overflow here.  In
         * reality, though, the highest fd on a unixy system will
         * not overflow here. XXXX */
        //单个进程可以打开的文件描述符是1024个，恰好是fd_set能标记的范围。
        while (fdsz < (int) SELECT_ALLOC_SIZE(fd + 1))
            fdsz *= 2;

        //重新分配空间。
        if (fdsz != sop->event_fdsz) {
            if (select_resize(sop, fdsz)) {
                check_selectop(sop);
                return (-1);
            }
        }

        sop->event_fds = fd;
    }

    //设置标记，表示要监听的事件。
    if (events & EV_READ)
        FD_SET(fd, sop->event_readset_in);
    if (events & EV_WRITE)
        FD_SET(fd, sop->event_writeset_in);
    check_selectop(sop);

    return (0);
}

/*
 * Nothing to be done here.
 */

static int
select_del(struct event_base *base, int fd, short old, short events, void *p)
{
    struct selectop *sop = base->evbase;
    (void)p;

    EVUTIL_ASSERT((events & EV_SIGNAL) == 0);
    check_selectop(sop);

    //如果要删除的文件描述符大于保存着的最大文件描述符，说明有错误。
    //这里直接返回0，表示删除成功。
    if (sop->event_fds < fd) {
        check_selectop(sop);
        return (0);
    }

    //可能只删除读事件
    if (events & EV_READ)
        FD_CLR(fd, sop->event_readset_in);

    //可能只删除写事件
    if (events & EV_WRITE)
        FD_CLR(fd, sop->event_writeset_in);

    check_selectop(sop);
    return (0);
}

static void
select_free_selectop(struct selectop *sop)
{
    if (sop->event_readset_in)
        mm_free(sop->event_readset_in);
    if (sop->event_writeset_in)
        mm_free(sop->event_writeset_in);
    if (sop->event_readset_out)
        mm_free(sop->event_readset_out);
    if (sop->event_writeset_out)
        mm_free(sop->event_writeset_out);

    memset(sop, 0, sizeof(struct selectop));
    mm_free(sop);
}

static void
select_dealloc(struct event_base *base)
{
    evsig_dealloc_(base);

    select_free_selectop(base->evbase);
}

#endif /* EVENT__HAVE_SELECT */
