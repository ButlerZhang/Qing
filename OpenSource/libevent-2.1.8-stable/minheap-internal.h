/*
 * Copyright (c) 2007-2012 Niels Provos and Nick Mathewson
 *
 * Copyright (c) 2006 Maxim Yegorushkin <maxim.yegorushkin@gmail.com>
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
#ifndef MINHEAP_INTERNAL_H_INCLUDED_
#define MINHEAP_INTERNAL_H_INCLUDED_

#include "event2/event-config.h"
#include "evconfig-private.h"
#include "event2/event.h"
#include "event2/event_struct.h"
#include "event2/util.h"
#include "util-internal.h"
#include "mm-internal.h"

typedef struct min_heap
{
    struct event** p;       //存储event*的数组
    unsigned n, a;          //n数组个数，a数组容量
} min_heap_t;

static inline void              min_heap_ctor_(min_heap_t* s);
static inline void              min_heap_dtor_(min_heap_t* s);
static inline void              min_heap_elem_init_(struct event* e);
static inline int               min_heap_elt_is_top_(const struct event *e);
static inline int               min_heap_empty_(min_heap_t* s);
static inline unsigned          min_heap_size_(min_heap_t* s);
static inline struct event*     min_heap_top_(min_heap_t* s);
static inline int               min_heap_reserve_(min_heap_t* s, unsigned n);
static inline int               min_heap_push_(min_heap_t* s, struct event* e);
static inline struct event*     min_heap_pop_(min_heap_t* s);
static inline int               min_heap_adjust_(min_heap_t *s, struct event* e);
static inline int               min_heap_erase_(min_heap_t* s, struct event* e);
static inline void              min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct event* e);
static inline void              min_heap_shift_up_unconditional_(min_heap_t* s, unsigned hole_index, struct event* e);
static inline void              min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct event* e);

//如果a.timeout > b.timeout，返回true
#define min_heap_elem_greater(a, b) \
    (evutil_timercmp(&(a)->ev_timeout, &(b)->ev_timeout, >))

//构造函数：初始化堆
void min_heap_ctor_(min_heap_t* s) { s->p = 0; s->n = 0; s->a = 0; }

//析构函数：释放堆
void min_heap_dtor_(min_heap_t* s) { if (s->p) mm_free(s->p); }

//初始化堆中的元素，即把event中的min_heap_idx置为-1
void min_heap_elem_init_(struct event* e) { e->ev_timeout_pos.min_heap_idx = -1; }

//判断堆是否为空
int min_heap_empty_(min_heap_t* s) { return 0u == s->n; }

//当前堆中的元素个数
unsigned min_heap_size_(min_heap_t* s) { return s->n; }

//返回堆的第一个元素，即最小timeout的event
struct event* min_heap_top_(min_heap_t* s) { return s->n ? *s->p : 0; }

//压入一个新元素
int min_heap_push_(min_heap_t* s, struct event* e)
{
    //首先判断是否需要扩充容量
    if (min_heap_reserve_(s, s->n + 1))
        return -1;

    //向上调整
    min_heap_shift_up_(s, s->n++, e);
    return 0;
}

//弹出第一个元素，即最小timeout的event
struct event* min_heap_pop_(min_heap_t* s)
{
    if (s->n)
    {
        //获取第一个元素
        struct event* e = *s->p;

        //向下调整
        min_heap_shift_down_(s, 0u, s->p[--s->n]);

        //把第一个元素的堆下标置为-1，表示不再在堆里
        e->ev_timeout_pos.min_heap_idx = -1;
        return e;
    }
    return 0;
}

//判断event是否是堆中的第一个元素
int min_heap_elt_is_top_(const struct event *e)
{
    return e->ev_timeout_pos.min_heap_idx == 0;
}

//从堆中删除事件event
int min_heap_erase_(min_heap_t* s, struct event* e)
{
    //传入的event必须在堆中
    if (-1 != e->ev_timeout_pos.min_heap_idx)
    {
        //堆元素个数减1，获取最后一个堆元素
        struct event *last = s->p[--s->n];

        //要删除的节点的父节点下标
        unsigned parent = (e->ev_timeout_pos.min_heap_idx - 1) / 2;

        /* we replace e with the last element in the heap.  We might need to
           shift it upward if it is less than its parent, or downward if it is
           greater than one or both its children. Since the children are known
           to be less than the parent, it can't need to shift both up and
           down. */

        //用最后一个元素替换删除元素。然后根据时间值判断是要向上调整，还是向下调整。
        if (e->ev_timeout_pos.min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], last))
            min_heap_shift_up_unconditional_(s, e->ev_timeout_pos.min_heap_idx, last);
        else
            min_heap_shift_down_(s, e->ev_timeout_pos.min_heap_idx, last);

        //别忘了把删除的节点的堆下标信息置为-1
        e->ev_timeout_pos.min_heap_idx = -1;

        return 0;
    }

    return -1;
}

//调整堆
int min_heap_adjust_(min_heap_t *s, struct event *e)
{
    if (-1 == e->ev_timeout_pos.min_heap_idx) {
        //如果事件还未放进堆中，就放进去
        return min_heap_push_(s, e);
    } else {

        //获取这个节点的父节点下标
        unsigned parent = (e->ev_timeout_pos.min_heap_idx - 1) / 2;

        /* The position of e has changed; we shift it up or down
         * as needed.  We can't need to do both. */

        //根据条件判断，是需要向上调整，还是向下调整。
        if (e->ev_timeout_pos.min_heap_idx > 0 && min_heap_elem_greater(s->p[parent], e))
            min_heap_shift_up_unconditional_(s, e->ev_timeout_pos.min_heap_idx, e);
        else
            min_heap_shift_down_(s, e->ev_timeout_pos.min_heap_idx, e);

        return 0;
    }
}

//分配堆空间
int min_heap_reserve_(min_heap_t* s, unsigned n)
{
    if (s->a < n)
    {
        struct event** p;

        //容量，最开始为8
        unsigned a = s->a ? s->a * 2 : 8;
        if (a < n)
            a = n;

        //分配数组空间
        if (!(p = (struct event**)mm_realloc(s->p, a * sizeof *p)))
            return -1;

        //记录数组地址和容量
        s->p = p;
        s->a = a;
    }
    return 0;
}

//无条件向上调整，相比于min_heap_shift_up_函数，就是少了一次判断
void min_heap_shift_up_unconditional_(min_heap_t* s, unsigned hole_index, struct event* e)
{
    unsigned parent = (hole_index - 1) / 2;
    do
    {
        (s->p[hole_index] = s->p[parent])->ev_timeout_pos.min_heap_idx = hole_index;
        hole_index = parent;
        parent = (hole_index - 1) / 2;
    } while (hole_index && min_heap_elem_greater(s->p[parent], e));

    (s->p[hole_index] = e)->ev_timeout_pos.min_heap_idx = hole_index;
}

//向上调整
void min_heap_shift_up_(min_heap_t* s, unsigned hole_index, struct event* e)
{
    unsigned parent = (hole_index - 1) / 2;
    while (hole_index && min_heap_elem_greater(s->p[parent], e))
    {
        (s->p[hole_index] = s->p[parent])->ev_timeout_pos.min_heap_idx = hole_index;
        hole_index = parent;
        parent = (hole_index - 1) / 2;
    }
    (s->p[hole_index] = e)->ev_timeout_pos.min_heap_idx = hole_index;
}

//向下调整
void min_heap_shift_down_(min_heap_t* s, unsigned hole_index, struct event* e)
{
    unsigned min_child = 2 * (hole_index + 1);
    while (min_child <= s->n)
    {
        min_child -= min_child == s->n || min_heap_elem_greater(s->p[min_child], s->p[min_child - 1]);
        if (!(min_heap_elem_greater(e, s->p[min_child])))
            break;

        (s->p[hole_index] = s->p[min_child])->ev_timeout_pos.min_heap_idx = hole_index;
        hole_index = min_child;
        min_child = 2 * (hole_index + 1);
    }

    (s->p[hole_index] = e)->ev_timeout_pos.min_heap_idx = hole_index;
}

#endif /* MINHEAP_INTERNAL_H_INCLUDED_ */
