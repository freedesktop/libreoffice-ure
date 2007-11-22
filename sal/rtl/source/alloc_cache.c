/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: alloc_cache.c,v $
 *
 *  $Revision: 1.5 $
 *
 *  last change: $Author: ihi $ $Date: 2007-11-22 12:20:56 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

#include "alloc_cache.h"

#ifndef INCLUDED_RTL_ALLOC_IMPL_H
#include "alloc_impl.h"
#endif

#ifndef INCLUDED_RTL_ALLOC_ARENA_H
#include "alloc_arena.h"
#endif

#ifndef INCLUDED_SAL_INTERNAL_ONCE_H
#include "internal/once.h"
#endif

#ifndef _SAL_MACROS_H_
#include "sal/macros.h"
#endif

#ifndef _OSL_DIAGNOSE_H_
#include "osl/diagnose.h"
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>
#endif

#ifndef INCLUDED_STDIO_H
#include <stdio.h>
#endif

#ifdef OS2
#undef OSL_TRACE
#define OSL_TRACE                  1 ? ((void)0) : _OSL_GLOBAL osl_trace
#endif

/* ================================================================= *
 *
 * cache internals.
 *
 * ================================================================= */

/** g_cache_list
 *  @internal
 */
struct rtl_cache_list_st
{
    rtl_memory_lock_type m_lock;
    rtl_cache_type       m_cache_head;

#if defined(SAL_UNX) || defined(SAL_OS2)
    pthread_t            m_update_thread;
    pthread_cond_t       m_update_cond;
#elif defined(SAL_W32)
    HANDLE               m_update_thread;
    HANDLE               m_update_cond;
#endif /* SAL_UNX || SAL_W32 */
    int                  m_update_done;
};

static struct rtl_cache_list_st g_cache_list;


/** gp_cache_arena
 *  provided for cache_type allocations, and hash_table resizing.
 *
 *  @internal
 */
static rtl_arena_type * gp_cache_arena = 0;


/** gp_cache_magazine_cache
 *  @internal
 */
static rtl_cache_type * gp_cache_magazine_cache = 0;


/** gp_cache_slab_cache
 *  @internal
 */
static rtl_cache_type * gp_cache_slab_cache = 0;


/** gp_cache_bufctl_cache
 *  @internal
 */
static rtl_cache_type * gp_cache_bufctl_cache = 0;


/** rtl_cache_init()
 *  @internal
 */
static int
rtl_cache_init (void);


/* ================================================================= */

/** RTL_CACHE_HASH_INDEX()
 */
#define	RTL_CACHE_HASH_INDEX_IMPL(a, s, q, m) \
     ((((a) + ((a) >> (s)) + ((a) >> ((s) << 1))) >> (q)) & (m))
 
#define	RTL_CACHE_HASH_INDEX(cache, addr) \
    RTL_CACHE_HASH_INDEX_IMPL((addr), (cache)->m_hash_shift, (cache)->m_type_shift, ((cache)->m_hash_size - 1))


/** rtl_cache_hash_rescale()
 */
static void
rtl_cache_hash_rescale (
    rtl_cache_type * cache,
    sal_Size         new_size
)
{
    rtl_cache_bufctl_type ** new_table;
    sal_Size                 new_bytes;

    new_bytes = new_size * sizeof(rtl_cache_bufctl_type*);
    new_table = (rtl_cache_bufctl_type**)rtl_arena_alloc(gp_cache_arena, &new_bytes);

    if (new_table != 0)
    {
        rtl_cache_bufctl_type ** old_table;
        sal_Size                 old_size, i;

        memset (new_table, 0, new_bytes);

        RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_slab_lock));

        old_table = cache->m_hash_table;
        old_size  = cache->m_hash_size;

        OSL_TRACE(
            "rtl_cache_hash_rescale(\"%s\"): "
            "nbuf: % " PRIu64 " (ave: %" PRIu64 "), frees: %" PRIu64 " "
            "[old_size: %lu, new_size: %lu]",
            cache->m_name,
            cache->m_slab_stats.m_alloc - cache->m_slab_stats.m_free,
            (cache->m_slab_stats.m_alloc - cache->m_slab_stats.m_free) >> cache->m_hash_shift,
            cache->m_slab_stats.m_free,
            old_size, new_size);

        cache->m_hash_table = new_table;
        cache->m_hash_size  = new_size;
        cache->m_hash_shift = highbit(cache->m_hash_size) - 1;

        for (i = 0; i < old_size; i++)
        {
            rtl_cache_bufctl_type * curr = old_table[i];
            while (curr != 0)
            {
                rtl_cache_bufctl_type  * next = curr->m_next;
                rtl_cache_bufctl_type ** head;

                head = &(cache->m_hash_table[RTL_CACHE_HASH_INDEX(cache, curr->m_addr)]);
                curr->m_next = (*head);
                (*head) = curr;

                curr = next;
            }
            old_table[i] = 0;
        }

        RTL_MEMORY_LOCK_RELEASE(&(cache->m_slab_lock));

        if (old_table != cache->m_hash_table_0)
        {
            sal_Size old_bytes = old_size * sizeof(rtl_cache_bufctl_type*);
            rtl_arena_free (gp_cache_arena, old_table, old_bytes);
        }
    }
}

/** rtl_cache_hash_insert()
 */
static RTL_MEMORY_INLINE sal_uIntPtr
rtl_cache_hash_insert (
    rtl_cache_type *        cache,
    rtl_cache_bufctl_type * bufctl
)
{
    rtl_cache_bufctl_type ** ppHead;

    ppHead = &(cache->m_hash_table[RTL_CACHE_HASH_INDEX(cache, bufctl->m_addr)]);

    bufctl->m_next = (*ppHead);
    (*ppHead) = bufctl;

    return (bufctl->m_addr);
}

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma inline(rtl_cache_hash_insert)
#endif /* __SUNPRO_C */


/** rtl_cache_hash_remove()
 */
static rtl_cache_bufctl_type *
rtl_cache_hash_remove (
    rtl_cache_type * cache,
    sal_uIntPtr      addr
)
{
    rtl_cache_bufctl_type ** ppHead;
    rtl_cache_bufctl_type  * bufctl;
    sal_Size                 lookups = 0;

    ppHead = &(cache->m_hash_table[RTL_CACHE_HASH_INDEX(cache, addr)]);
    while ((bufctl = *ppHead) != 0)
    {
        if (bufctl->m_addr == addr)
        {
            *ppHead = bufctl->m_next, bufctl->m_next = 0;
            break;
        }

        lookups += 1;
        ppHead = &(bufctl->m_next);
    }

    OSL_ASSERT (bufctl != 0); /* bad free */

    if (lookups > 1)
    {
        sal_Size nbuf = (sal_Size)(cache->m_slab_stats.m_alloc - cache->m_slab_stats.m_free);
        if (nbuf > 4 * cache->m_hash_size)
        {
            if (!(cache->m_features & RTL_CACHE_FEATURE_RESCALE))
            {
                sal_Size ave = nbuf >> cache->m_hash_shift;
                sal_Size new_size = cache->m_hash_size << (highbit(ave) - 1);

                cache->m_features |= RTL_CACHE_FEATURE_RESCALE;
                RTL_MEMORY_LOCK_RELEASE(&(cache->m_slab_lock));
                rtl_cache_hash_rescale (cache, new_size);
                RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_slab_lock));
                cache->m_features &= ~RTL_CACHE_FEATURE_RESCALE;
            }
        }
    }

    return (bufctl);
}

/* ================================================================= */

/** RTL_CACHE_SLAB()
 */
#define RTL_CACHE_SLAB(addr, size) \
    (((rtl_cache_slab_type*)(RTL_MEMORY_P2END((sal_uIntPtr)(addr), (size)))) - 1)


/** rtl_cache_slab_constructor()
 */
static int
rtl_cache_slab_constructor (void * obj, void * arg)
{
    rtl_cache_slab_type * slab = (rtl_cache_slab_type*)(obj);

    (void) arg; /* unused */

    QUEUE_START_NAMED(slab, slab_);
    slab->m_ntypes = 0;

    return (1);
}


/** rtl_cache_slab_destructor()
 */
static void
rtl_cache_slab_destructor (void * obj, void * arg)
{
#if OSL_DEBUG_LEVEL == 0
    (void) obj; /* unused */
#else /* OSL_DEBUG_LEVEL */
    rtl_cache_slab_type * slab = (rtl_cache_slab_type*)(obj);

    /* assure removed from queue(s) */
    OSL_ASSERT(QUEUE_STARTED_NAMED(slab, slab_));

    /* assure no longer referenced */
    OSL_ASSERT(slab->m_ntypes == 0);
#endif /* OSL_DEBUG_LEVEL */

    (void) arg; /* unused */
}


/** rtl_cache_slab_create()
 *
 *  @precond cache->m_slab_lock released.
 */
static rtl_cache_slab_type *
rtl_cache_slab_create (
    rtl_cache_type * cache
)
{
    rtl_cache_slab_type * slab = 0;
    void *                addr;
    sal_Size              size;

    size = cache->m_slab_size;
    addr = rtl_arena_alloc (cache->m_source, &size);
    if (addr != 0)
    {
        OSL_ASSERT(size >= cache->m_slab_size);

        if (cache->m_features & RTL_CACHE_FEATURE_HASH)
        {
            /* allocate slab struct from slab cache */
            OSL_ASSERT (cache != gp_cache_slab_cache);
            slab = (rtl_cache_slab_type*)rtl_cache_alloc (gp_cache_slab_cache);
        }
        else
        {
            /* construct embedded slab struct */
            slab = RTL_CACHE_SLAB(addr, cache->m_slab_size);
            (void) rtl_cache_slab_constructor (slab, 0);
        }
        if (slab != 0)
        {
            slab->m_data = (sal_uIntPtr)(addr);

            /* dynamic freelist initialization */
            slab->m_bp = slab->m_data;
            slab->m_sp = 0;
        }
        else
        {
            rtl_arena_free (cache->m_source, addr, size);
        }
    }
    return (slab);
}


/** rtl_cache_slab_destroy()
 *
 *  @precond cache->m_slab_lock released.
 */
static void
rtl_cache_slab_destroy (
    rtl_cache_type *      cache,
    rtl_cache_slab_type * slab
)
{
    void *   addr   = (void*)(slab->m_data);
    sal_Size refcnt = slab->m_ntypes; slab->m_ntypes = 0;

    if (cache->m_features & RTL_CACHE_FEATURE_HASH)
    {
        /* cleanup bufctl(s) for free buffer(s) */
        sal_Size ntypes = (slab->m_bp - slab->m_data) / cache->m_type_size;
        for (ntypes -= refcnt; slab->m_sp != 0; ntypes--)
        {
            rtl_cache_bufctl_type * bufctl = slab->m_sp;

            /* pop from freelist */
            slab->m_sp = bufctl->m_next, bufctl->m_next = 0;

            /* return bufctl struct to bufctl cache */
            rtl_cache_free (gp_cache_bufctl_cache, bufctl);
        }
        OSL_ASSERT(ntypes == 0);

        /* return slab struct to slab cache */
        rtl_cache_free (gp_cache_slab_cache, slab);
    }
    else
    {
        /* destruct embedded slab struct */
        rtl_cache_slab_destructor (slab, 0);
    }

    if ((refcnt == 0) || (cache->m_features & RTL_CACHE_FEATURE_BULKDESTROY))
    {
        /* free memory */
        rtl_arena_free (cache->m_source, addr, cache->m_slab_size);
    }
}


/** rtl_cache_slab_populate()
 *
 *  @precond cache->m_slab_lock acquired.
 */
static int
rtl_cache_slab_populate (
    rtl_cache_type * cache
)
{
    rtl_cache_slab_type * slab;

    RTL_MEMORY_LOCK_RELEASE(&(cache->m_slab_lock));
    slab = rtl_cache_slab_create (cache);
    RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_slab_lock));
    if (slab != 0)
    {
        /* update buffer start addr w/ current color */
        slab->m_bp += cache->m_ncolor;

        /* update color for next slab */
        cache->m_ncolor += cache->m_type_align;
        if (cache->m_ncolor > cache->m_ncolor_max)
            cache->m_ncolor = 0;

        /* update stats */
        cache->m_slab_stats.m_mem_total += cache->m_slab_size;

        /* insert onto 'free' queue */
        QUEUE_INSERT_HEAD_NAMED(&(cache->m_free_head), slab, slab_);
    }
    return (slab != 0);
}

/* ================================================================= */

/** rtl_cache_slab_alloc()
 *
 *  Allocate a buffer from slab layer; used by magazine layer.
 */
static void *
rtl_cache_slab_alloc (
    rtl_cache_type * cache
)
{
    void                * addr = 0;
    rtl_cache_slab_type * head;

    RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_slab_lock));

    head = &(cache->m_free_head);
    if ((head->m_slab_next != head) || rtl_cache_slab_populate (cache))
    {
        rtl_cache_slab_type   * slab;
        rtl_cache_bufctl_type * bufctl;

        slab = head->m_slab_next;
        OSL_ASSERT(slab->m_ntypes < cache->m_ntypes);

        if (slab->m_sp == 0)
        {
            /* initialize bufctl w/ current 'slab->m_bp' */
            OSL_ASSERT (slab->m_bp < slab->m_data + cache->m_ntypes * cache->m_type_size + cache->m_ncolor_max);
            if (cache->m_features & RTL_CACHE_FEATURE_HASH)
            {
                /* allocate bufctl */
                OSL_ASSERT (cache != gp_cache_bufctl_cache);
                bufctl = (rtl_cache_bufctl_type*)rtl_cache_alloc (gp_cache_bufctl_cache);
                if (bufctl == 0)
                {
                    /* out of memory */
                    RTL_MEMORY_LOCK_RELEASE(&(cache->m_slab_lock));
                    return (0);
                }

                bufctl->m_addr = slab->m_bp;
                bufctl->m_slab = (sal_uIntPtr)(slab);
            }
            else
            {
                /* embedded bufctl */
                bufctl = (rtl_cache_bufctl_type*)(slab->m_bp);
            }
            bufctl->m_next = 0;

            /* update 'slab->m_bp' to next free buffer */
            slab->m_bp += cache->m_type_size;

            /* assign bufctl to freelist */
            slab->m_sp = bufctl;
        }

        /* pop front */
        bufctl = slab->m_sp;
        slab->m_sp = bufctl->m_next;

        /* increment usage, check for full slab */
        if ((slab->m_ntypes += 1) == cache->m_ntypes)
        {
            /* remove from 'free' queue */
            QUEUE_REMOVE_NAMED(slab, slab_);

            /* insert onto 'used' queue (tail) */
            QUEUE_INSERT_TAIL_NAMED(&(cache->m_used_head), slab, slab_);
        }

        /* update stats */
        cache->m_slab_stats.m_alloc     += 1;
        cache->m_slab_stats.m_mem_alloc += cache->m_type_size;

        if (cache->m_features & RTL_CACHE_FEATURE_HASH)
            addr = (void*)rtl_cache_hash_insert (cache, bufctl);
        else
            addr = bufctl;
    }

    RTL_MEMORY_LOCK_RELEASE(&(cache->m_slab_lock));
    return (addr);
}


/** rtl_cache_slab_free()
 *
 *  Return a buffer to slab layer; used by magazine layer.
 */
static void
rtl_cache_slab_free (
    rtl_cache_type * cache,
    void *           addr
)
{
    rtl_cache_bufctl_type * bufctl;
    rtl_cache_slab_type   * slab;

    RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_slab_lock));

    /* determine slab from addr */
    if (cache->m_features & RTL_CACHE_FEATURE_HASH)
    {
        bufctl = rtl_cache_hash_remove (cache, (sal_uIntPtr)(addr));
        slab = (bufctl != 0) ? (rtl_cache_slab_type*)(bufctl->m_slab) : 0;
    }
    else
    {
        /* embedded slab struct */
        bufctl = (rtl_cache_bufctl_type*)(addr);
        slab = RTL_CACHE_SLAB(addr, cache->m_slab_size);
    }

    if (slab != 0)
    {
        /* check for full slab */
        if (slab->m_ntypes == cache->m_ntypes)
        {
            /* remove from 'used' queue */
            QUEUE_REMOVE_NAMED(slab, slab_);

            /* insert onto 'free' queue (head) */
            QUEUE_INSERT_HEAD_NAMED(&(cache->m_free_head), slab, slab_);
        }

        /* push front */
        bufctl->m_next = slab->m_sp;
        slab->m_sp = bufctl;

        /* update stats */
        cache->m_slab_stats.m_free      += 1;
        cache->m_slab_stats.m_mem_alloc -= cache->m_type_size;

        /* decrement usage, check for empty slab */
        if ((slab->m_ntypes -= 1) == 0)
        {
            /* remove from 'free' queue */
            QUEUE_REMOVE_NAMED(slab, slab_);

            /* update stats */
            cache->m_slab_stats.m_mem_total -= cache->m_slab_size;

            /* free 'empty' slab */
            RTL_MEMORY_LOCK_RELEASE(&(cache->m_slab_lock));
            rtl_cache_slab_destroy (cache, slab);
            return;
        }
    }

    RTL_MEMORY_LOCK_RELEASE(&(cache->m_slab_lock));
}

/* ================================================================= */

/** rtl_cache_magazine_constructor()
 */
static int
rtl_cache_magazine_constructor (void * obj, void * arg)
{
    rtl_cache_magazine_type * mag = (rtl_cache_magazine_type*)(obj);
    /* @@@ sal_Size size = (sal_Size)(arg); @@@ */

    (void) arg; /* unused */

    mag->m_mag_next = 0;
    mag->m_mag_size = RTL_CACHE_MAGAZINE_SIZE;
    mag->m_mag_used = 0;

    return (1);
}


/** rtl_cache_magazine_destructor()
 */
static void
rtl_cache_magazine_destructor (void * obj, void * arg)
{
#if OSL_DEBUG_LEVEL == 0
    (void) obj; /* unused */
#else /* OSL_DEBUG_LEVEL */
    rtl_cache_magazine_type * mag = (rtl_cache_magazine_type*)(obj);

    /* assure removed from queue(s) */
    OSL_ASSERT(mag->m_mag_next == 0);

    /* assure no longer referenced */
    OSL_ASSERT(mag->m_mag_used == 0);
#endif /* OSL_DEBUG_LEVEL */

    (void) arg; /* unused */
}


/** rtl_cache_magazine_clear()
 */
static void
rtl_cache_magazine_clear (
    rtl_cache_type *          cache,
    rtl_cache_magazine_type * mag
)
{
    for (; mag->m_mag_used > 0; --mag->m_mag_used)
    {
        void * obj = mag->m_objects[mag->m_mag_used - 1];
        mag->m_objects[mag->m_mag_used - 1] = 0;

        if (cache->m_destructor != 0)
        {
            /* destruct object */
            (cache->m_destructor)(obj, cache->m_userarg);
        }

        /* return buffer to slab layer */
        rtl_cache_slab_free (cache, obj);
    }
}

/* ================================================================= */

/** rtl_cache_depot_enqueue()
 *
 *  @precond cache->m_depot_lock acquired.
 */
static RTL_MEMORY_INLINE void
rtl_cache_depot_enqueue (
    rtl_cache_depot_type *    depot,
    rtl_cache_magazine_type * mag
)
{
    /* enqueue empty magazine */
    mag->m_mag_next = depot->m_mag_next;
    depot->m_mag_next = mag;

    /* update depot stats */
    depot->m_mag_count++;
}

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma inline(rtl_cache_depot_enqueue)
#endif /* __SUNPRO_C */


/** rtl_cache_depot_dequeue()
 *
 *  @precond cache->m_depot_lock acquired.
 */
static RTL_MEMORY_INLINE rtl_cache_magazine_type *
rtl_cache_depot_dequeue (
    rtl_cache_depot_type * depot
)
{
    rtl_cache_magazine_type * mag = 0;
    if (depot->m_mag_count > 0)
    {
        /* dequeue magazine */
        OSL_ASSERT(depot->m_mag_next != 0);

        mag = depot->m_mag_next;
        depot->m_mag_next = mag->m_mag_next;
        mag->m_mag_next = 0;

        /* update depot stats */
        depot->m_mag_count--;
        depot->m_curr_min = SAL_MIN(depot->m_curr_min, depot->m_mag_count);
    }
    return (mag);
}

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma inline(rtl_cache_depot_dequeue)
#endif /* __SUNPRO_C */


/** rtl_cache_depot_exchange_alloc()
 *
 *  @precond cache->m_depot_lock acquired.
 */
static RTL_MEMORY_INLINE rtl_cache_magazine_type *
rtl_cache_depot_exchange_alloc (
    rtl_cache_type *          cache,
    rtl_cache_magazine_type * empty
)
{
    rtl_cache_magazine_type * full;

    OSL_ASSERT((empty == 0) || (empty->m_mag_used == 0));

    /* dequeue full magazine */
    full = rtl_cache_depot_dequeue (&(cache->m_depot_full));
    if ((full != 0) && (empty != 0))
    {
        /* enqueue empty magazine */
        rtl_cache_depot_enqueue (&(cache->m_depot_empty), empty);
    }

    OSL_ASSERT((full == 0) || (full->m_mag_used > 0));

    return (full);
}

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma inline(rtl_cache_depot_exchange_alloc)
#endif /* __SUNPRO_C */


/** rtl_cache_depot_exchange_free()
 *
 *  @precond cache->m_depot_lock acquired.
 */
static RTL_MEMORY_INLINE rtl_cache_magazine_type *
rtl_cache_depot_exchange_free (
    rtl_cache_type *          cache,
    rtl_cache_magazine_type * full
)
{
    rtl_cache_magazine_type * empty;

    OSL_ASSERT((full == 0) || (full->m_mag_used > 0));

    /* dequeue empty magazine */
    empty = rtl_cache_depot_dequeue (&(cache->m_depot_empty));
    if ((empty != 0) && (full != 0))
    {
        /* enqueue full magazine */
        rtl_cache_depot_enqueue (&(cache->m_depot_full), full);
    }

    OSL_ASSERT((empty == 0) || (empty->m_mag_used == 0));

    return (empty);
}

#if defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma inline(rtl_cache_depot_exchange_free)
#endif /* __SUNPRO_C */


/** rtl_cache_depot_populate()
 *
 *  @precond cache->m_depot_lock acquired.
 */
static int
rtl_cache_depot_populate (
    rtl_cache_type * cache
)
{
    rtl_cache_magazine_type * empty = 0;

    if (cache->m_magazine_cache != 0)
    {
        /* allocate new empty magazine */
        RTL_MEMORY_LOCK_RELEASE(&(cache->m_depot_lock));
        empty = (rtl_cache_magazine_type*)rtl_cache_alloc (cache->m_magazine_cache);
        RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_depot_lock));
        if (empty != 0)
        {
            /* enqueue (new) empty magazine */
            rtl_cache_depot_enqueue (&(cache->m_depot_empty), empty);
        }
    }
    return (empty != 0);
}

/* ================================================================= */

/** rtl_cache_constructor()
 */
static int
rtl_cache_constructor (void * obj)
{
    rtl_cache_type * cache = (rtl_cache_type*)(obj);

    memset (cache, 0, sizeof(rtl_cache_type));

    /* linkage */
    QUEUE_START_NAMED(cache, cache_);

    /* slab layer */
    (void)RTL_MEMORY_LOCK_INIT(&(cache->m_slab_lock));

    QUEUE_START_NAMED(&(cache->m_free_head), slab_);
    QUEUE_START_NAMED(&(cache->m_used_head), slab_);

    cache->m_hash_table = cache->m_hash_table_0;
    cache->m_hash_size  = RTL_CACHE_HASH_SIZE;
    cache->m_hash_shift = highbit(cache->m_hash_size) - 1;

    /* depot layer */
    (void)RTL_MEMORY_LOCK_INIT(&(cache->m_depot_lock));

    return (1);
}

/** rtl_cache_destructor()
 */
static void
rtl_cache_destructor (void * obj)
{
    rtl_cache_type * cache = (rtl_cache_type*)(obj);

    /* linkage */
    OSL_ASSERT(QUEUE_STARTED_NAMED(cache, cache_));

    /* slab layer */
    (void)RTL_MEMORY_LOCK_DESTROY(&(cache->m_slab_lock));

    OSL_ASSERT(QUEUE_STARTED_NAMED(&(cache->m_free_head), slab_));
    OSL_ASSERT(QUEUE_STARTED_NAMED(&(cache->m_used_head), slab_));

    OSL_ASSERT(cache->m_hash_table == cache->m_hash_table_0);
    OSL_ASSERT(cache->m_hash_size  == RTL_CACHE_HASH_SIZE);
    OSL_ASSERT(cache->m_hash_shift == (sal_Size)(highbit(cache->m_hash_size) - 1));

    /* depot layer */
    (void)RTL_MEMORY_LOCK_DESTROY(&(cache->m_depot_lock));
}

/* ================================================================= */

/** rtl_cache_activate()
 */
static rtl_cache_type *
rtl_cache_activate (
    rtl_cache_type * cache,
    const char *     name,
    size_t           objsize,
    size_t           objalign,
    int  (SAL_CALL * constructor)(void * obj, void * userarg),
    void (SAL_CALL * destructor) (void * obj, void * userarg),
    void (SAL_CALL * reclaim)    (void * userarg),
    void *           userarg,
    rtl_arena_type * source,
    int              flags
)
{
    OSL_ASSERT(cache != 0);
    if (cache != 0)
    {
        sal_Size slabsize;

        snprintf (cache->m_name, sizeof(cache->m_name), "%s", name);

        /* ensure minimum size (embedded bufctl linkage) */
        objsize = SAL_MAX(objsize, sizeof(rtl_cache_bufctl_type*));

        if (objalign == 0)
        {
            /* determine default alignment */
            if (objsize >= RTL_MEMORY_ALIGNMENT_8)
                objalign = RTL_MEMORY_ALIGNMENT_8;
            else
                objalign = RTL_MEMORY_ALIGNMENT_4;
        }
        else
        {
            /* ensure minimum alignment */
            objalign = SAL_MAX(objalign, RTL_MEMORY_ALIGNMENT_4);
        }
        OSL_ASSERT(RTL_MEMORY_ISP2(objalign));

        cache->m_type_size  = objsize = RTL_MEMORY_P2ROUNDUP(objsize, objalign);
        cache->m_type_align = objalign;
        cache->m_type_shift = highbit(cache->m_type_size) - 1;

        cache->m_constructor = constructor;
        cache->m_destructor  = destructor;
        cache->m_reclaim     = reclaim;
        cache->m_userarg     = userarg;

        /* slab layer */
        cache->m_source = source;

        slabsize = source->m_quantum; /* minimum slab size */
        if (flags & RTL_CACHE_FLAG_QUANTUMCACHE)
        {
            /* next power of 2 above 3 * qcache_max */
            slabsize = SAL_MAX(slabsize, (1UL << highbit(3 * source->m_qcache_max)));
        }
        else
        {
            /* waste at most 1/8 of slab */
            slabsize = SAL_MAX(slabsize, cache->m_type_size * 8);
        }

        slabsize = RTL_MEMORY_P2ROUNDUP(slabsize, source->m_quantum);
        if (!RTL_MEMORY_ISP2(slabsize))
            slabsize = 1UL << highbit(slabsize);
        cache->m_slab_size = slabsize;

        if (cache->m_slab_size > source->m_quantum)
        {
            OSL_ASSERT(gp_cache_slab_cache != 0);
            OSL_ASSERT(gp_cache_bufctl_cache != 0);

            cache->m_features  |= RTL_CACHE_FEATURE_HASH;
            cache->m_ntypes     = cache->m_slab_size / cache->m_type_size;
            cache->m_ncolor_max = cache->m_slab_size % cache->m_type_size;
        }
        else
        {
            /* embedded slab struct */
            cache->m_ntypes     = (cache->m_slab_size - sizeof(rtl_cache_slab_type)) / cache->m_type_size;
            cache->m_ncolor_max = (cache->m_slab_size - sizeof(rtl_cache_slab_type)) % cache->m_type_size;
        }

        OSL_ASSERT(cache->m_ntypes > 0);
        cache->m_ncolor = 0;

        if (flags & RTL_CACHE_FLAG_BULKDESTROY)
        {
            /* allow bulk slab delete upon cache deactivation */
            cache->m_features |= RTL_CACHE_FEATURE_BULKDESTROY;
        }

        /* magazine layer */
        if (!(flags & RTL_CACHE_FLAG_NOMAGAZINE))
        {
            OSL_ASSERT(gp_cache_magazine_cache != 0);
            cache->m_magazine_cache = gp_cache_magazine_cache;
        }

        /* insert into cache list */
        RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
        QUEUE_INSERT_TAIL_NAMED(&(g_cache_list.m_cache_head), cache, cache_);
        RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));
    }
    return (cache);
}

/** rtl_cache_deactivate()
 */
static void
rtl_cache_deactivate (
    rtl_cache_type * cache
)
{
    /* remove from cache list */
    RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
    QUEUE_REMOVE_NAMED(cache, cache_);
    RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));

    /* cleanup magazine layer */
    if (cache->m_magazine_cache != 0)
    {
        rtl_cache_type *          mag_cache;
        rtl_cache_magazine_type * mag;

        /* prevent recursion */
        mag_cache = cache->m_magazine_cache, cache->m_magazine_cache = 0;

        /* cleanup cpu layer */
        if ((mag = cache->m_cpu_curr) != 0)
        {
            cache->m_cpu_curr = 0;
            rtl_cache_magazine_clear (cache, mag);
            rtl_cache_free (mag_cache, mag);
        }
        if ((mag = cache->m_cpu_prev) != 0)
        {
            cache->m_cpu_prev = 0;
            rtl_cache_magazine_clear (cache, mag);
            rtl_cache_free (mag_cache, mag);
        }

        /* cleanup depot layer */
        while ((mag = rtl_cache_depot_dequeue(&(cache->m_depot_full))) != 0)
        {
            rtl_cache_magazine_clear (cache, mag);
            rtl_cache_free (mag_cache, mag);
        }
        while ((mag = rtl_cache_depot_dequeue(&(cache->m_depot_empty))) != 0)
        {
            rtl_cache_magazine_clear (cache, mag);
            rtl_cache_free (mag_cache, mag);
        }
    }

    OSL_TRACE(
        "rtl_cache_deactivate(\"%s\"): "
        "[slab]: allocs: %"PRIu64", frees: %"PRIu64"; total: %lu, used: %lu; "
        "[cpu]: allocs: %"PRIu64", frees: %"PRIu64"; "
        "[total]: allocs: %"PRIu64", frees: %"PRIu64"",
        cache->m_name,
        cache->m_slab_stats.m_alloc, cache->m_slab_stats.m_free,
        cache->m_slab_stats.m_mem_total, cache->m_slab_stats.m_mem_alloc,
        cache->m_cpu_stats.m_alloc, cache->m_cpu_stats.m_free,
        cache->m_slab_stats.m_alloc + cache->m_cpu_stats.m_alloc,
        cache->m_slab_stats.m_free  + cache->m_cpu_stats.m_free
    );

    /* cleanup slab layer */
    if (cache->m_slab_stats.m_alloc > cache->m_slab_stats.m_free)
    {
        OSL_TRACE(
            "rtl_cache_deactivate(\"%s\"): "
            "cleaning up %"PRIu64" leaked buffer(s) [%lu bytes] [%lu total]",
            cache->m_name,
            cache->m_slab_stats.m_alloc - cache->m_slab_stats.m_free,
            cache->m_slab_stats.m_mem_alloc, cache->m_slab_stats.m_mem_total
        );

        if (cache->m_features & RTL_CACHE_FEATURE_HASH)
        {
            /* cleanup bufctl(s) for leaking buffer(s) */
            sal_Size i, n = cache->m_hash_size;
            for (i = 0; i < n; i++)
            {
                rtl_cache_bufctl_type * bufctl;
                while ((bufctl = cache->m_hash_table[i]) != 0)
                {
                    /* pop from hash table */
                    cache->m_hash_table[i] = bufctl->m_next, bufctl->m_next = 0;

                    /* return to bufctl cache */
                    rtl_cache_free (gp_cache_bufctl_cache, bufctl);
                }
            }
        }
        {
            /* force cleanup of remaining slabs */
            rtl_cache_slab_type *head, *slab;

            head = &(cache->m_used_head);
            for (slab = head->m_slab_next; slab != head; slab = head->m_slab_next)
            {
                /* remove from 'used' queue */
                QUEUE_REMOVE_NAMED(slab, slab_);

                /* update stats */
                cache->m_slab_stats.m_mem_total -= cache->m_slab_size;

                /* free slab */
                rtl_cache_slab_destroy (cache, slab);
            }

            head = &(cache->m_free_head);
            for (slab = head->m_slab_next; slab != head; slab = head->m_slab_next)
            {
                /* remove from 'free' queue */
                QUEUE_REMOVE_NAMED(slab, slab_);

                /* update stats */
                cache->m_slab_stats.m_mem_total -= cache->m_slab_size;

                /* free slab */
                rtl_cache_slab_destroy (cache, slab);
            }
        }
    }

    if (cache->m_hash_table != cache->m_hash_table_0)
    {
        rtl_arena_free (
            gp_cache_arena,
            cache->m_hash_table,
            cache->m_hash_size * sizeof(rtl_cache_bufctl_type*));

        cache->m_hash_table = cache->m_hash_table_0;
        cache->m_hash_size  = RTL_CACHE_HASH_SIZE;
        cache->m_hash_shift = highbit(cache->m_hash_size) - 1;
    }
}

/* ================================================================= *
 *
 * cache implementation.
 *
 * ================================================================= */

/** rtl_cache_create()
 */
rtl_cache_type *
SAL_CALL rtl_cache_create (
    const char *     name,
    sal_Size         objsize,
    sal_Size         objalign,
    int  (SAL_CALL * constructor)(void * obj, void * userarg),
    void (SAL_CALL * destructor) (void * obj, void * userarg),
    void (SAL_CALL * reclaim)    (void * userarg),
    void *           userarg,
    rtl_arena_type * source,
    int              flags
)
{
    rtl_cache_type * result = 0;
    sal_Size         size   = sizeof(rtl_cache_type);

try_alloc:
    result = (rtl_cache_type*)rtl_arena_alloc (gp_cache_arena, &size);
    if (result != 0)
    {
        rtl_cache_type * cache = result;
        (void) rtl_cache_constructor (cache);

        if (!source)
        {
            /* use default arena */
            OSL_ASSERT(gp_default_arena != 0);
            source = gp_default_arena;
        }

        result = rtl_cache_activate (
            cache,
            name,
            objsize,
            objalign,
            constructor,
            destructor,
            reclaim,
            userarg,
            source,
            flags
        );

        if (result == 0)
        {
            /* activation failed */
            rtl_cache_deactivate (cache);
            rtl_cache_destructor (cache);
            rtl_arena_free (gp_cache_arena, cache, size);
        }
    }
    else if (gp_cache_arena == 0)
    {
        if (rtl_cache_init())
        {
            /* try again */
            goto try_alloc;
        }
    }
    return (result);
}

/** rtl_cache_destroy()
 */
void SAL_CALL rtl_cache_destroy (
    rtl_cache_type * cache
)
{
    if (cache != 0)
    {
        rtl_cache_deactivate (cache);
        rtl_cache_destructor (cache);
        rtl_arena_free (gp_cache_arena, cache, sizeof(rtl_cache_type));
    }
}

/** rtl_cache_alloc()
 */
void *
SAL_CALL rtl_cache_alloc (
    rtl_cache_type * cache
)
{
    void * obj = 0;

    if (cache == 0)
        return (0);

    if (cache->m_cpu_curr != 0)
    {
        RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_depot_lock));

        for (;;)
        {
            /* take object from magazine layer */
            rtl_cache_magazine_type *curr, *prev, *temp;

            curr = cache->m_cpu_curr;
            if ((curr != 0) && (curr->m_mag_used > 0))
            {
                obj = curr->m_objects[--curr->m_mag_used];
                cache->m_cpu_stats.m_alloc += 1;
                RTL_MEMORY_LOCK_RELEASE(&(cache->m_depot_lock));

                return (obj);
            }

            prev = cache->m_cpu_prev;
            if ((prev != 0) && (prev->m_mag_used > 0))
            {
                temp = cache->m_cpu_curr;
                cache->m_cpu_curr = cache->m_cpu_prev;
                cache->m_cpu_prev = temp;

                continue;
            }

            temp = rtl_cache_depot_exchange_alloc (cache, prev);
            if (temp != 0)
            {
                cache->m_cpu_prev = cache->m_cpu_curr;
                cache->m_cpu_curr = temp;

                continue;
            }

            /* no full magazine: fall through to slab layer */
            break;
        }

        RTL_MEMORY_LOCK_RELEASE(&(cache->m_depot_lock));
    }

    /* alloc buffer from slab layer */
    obj = rtl_cache_slab_alloc (cache);
    if ((obj != 0) && (cache->m_constructor != 0))
    {
        /* construct object */
        if (!((cache->m_constructor)(obj, cache->m_userarg)))
        {
            /* construction failure */
            rtl_cache_slab_free (cache, obj), obj = 0;
        }
    }

    return (obj);
}

/** rtl_cache_free()
 */
void
SAL_CALL rtl_cache_free (
    rtl_cache_type * cache,
    void *           obj
)
{
    if ((obj != 0) && (cache != 0))
    {
        RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_depot_lock));

        for (;;)
        {
            /* return object to magazine layer */
            rtl_cache_magazine_type *curr, *prev, *temp;

            curr = cache->m_cpu_curr;
            if ((curr != 0) && (curr->m_mag_used < curr->m_mag_size))
            {
                curr->m_objects[curr->m_mag_used++] = obj;
                cache->m_cpu_stats.m_free += 1;
                RTL_MEMORY_LOCK_RELEASE(&(cache->m_depot_lock));

                return;
            }

            prev = cache->m_cpu_prev;
            if ((prev != 0) && (prev->m_mag_used == 0))
            {
                temp = cache->m_cpu_curr;
                cache->m_cpu_curr = cache->m_cpu_prev;
                cache->m_cpu_prev = temp;

                continue;
            }

            temp = rtl_cache_depot_exchange_free (cache, prev);
            if (temp != 0)
            {
                cache->m_cpu_prev = cache->m_cpu_curr;
                cache->m_cpu_curr = temp;

                continue;
            }

            if (rtl_cache_depot_populate(cache) != 0)
            {
                continue;
            }

            /* no empty magazine: fall through to slab layer */
            break;
        }

        RTL_MEMORY_LOCK_RELEASE(&(cache->m_depot_lock));

        /* no space for constructed object in magazine layer */
        if (cache->m_destructor != 0)
        {
            /* destruct object */
            (cache->m_destructor)(obj, cache->m_userarg);
        }

        /* return buffer to slab layer */
        rtl_cache_slab_free (cache, obj);
    }
}

/* ================================================================= *
 *
 * cache wsupdate (machdep) internals.
 *
 * ================================================================= */

/** rtl_cache_wsupdate_init()
 *
 *  @precond g_cache_list.m_lock initialized
 */
static void
rtl_cache_wsupdate_init (void);


/** rtl_cache_wsupdate_wait()
 *
 *  @precond g_cache_list.m_lock acquired
 */
static void
rtl_cache_wsupdate_wait (
    unsigned int seconds
);

/** rtl_cache_wsupdate_fini()
 *
 */
static void
rtl_cache_wsupdate_fini (void);

/* ================================================================= */

#if defined(SAL_UNX) || defined(SAL_OS2)

#include <sys/time.h>

static void *
rtl_cache_wsupdate_all (void * arg);

static void
rtl_cache_wsupdate_init (void)
{
    RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
    g_cache_list.m_update_done = 0;
    (void) pthread_cond_init (&(g_cache_list.m_update_cond), NULL);
    if (pthread_create (
            &(g_cache_list.m_update_thread), NULL, rtl_cache_wsupdate_all, (void*)(10)) != 0)
    {
        /* failure */
        g_cache_list.m_update_thread = (pthread_t)(0);
    }
    RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));
}

static void
rtl_cache_wsupdate_wait (unsigned int seconds)
{
    if (seconds > 0)
    {
        struct timeval  now;
        struct timespec wakeup;

        gettimeofday(&now, 0);
        wakeup.tv_sec  = now.tv_sec + (seconds);
        wakeup.tv_nsec = now.tv_usec * 1000;

        (void) pthread_cond_timedwait (
            &(g_cache_list.m_update_cond),
            &(g_cache_list.m_lock),
            &wakeup);
    }
}

static void
rtl_cache_wsupdate_fini (void)
{
    RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
    g_cache_list.m_update_done = 1;
    pthread_cond_signal (&(g_cache_list.m_update_cond));
    RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));

    if (g_cache_list.m_update_thread != (pthread_t)(0))
        pthread_join (g_cache_list.m_update_thread, NULL);
}

/* ================================================================= */

#elif defined(SAL_W32)

static DWORD WINAPI
rtl_cache_wsupdate_all (void * arg);

static void
rtl_cache_wsupdate_init (void)
{
    DWORD dwThreadId;

    RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
    g_cache_list.m_update_done = 0;
    g_cache_list.m_update_cond = CreateEvent (0, TRUE, FALSE, 0);

    g_cache_list.m_update_thread =
        CreateThread (NULL, 0, rtl_cache_wsupdate_all, (LPVOID)(10), 0, &dwThreadId);
    RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));
}

static void
rtl_cache_wsupdate_wait (unsigned int seconds)
{
    if (seconds > 0)
    {
        RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));
        WaitForSingleObject (g_cache_list.m_update_cond, (DWORD)(seconds * 1000));
        RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
    }
}

static void
rtl_cache_wsupdate_fini (void)
{
    RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
    g_cache_list.m_update_done = 1;
    SetEvent (g_cache_list.m_update_cond);
    RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));

    WaitForSingleObject (g_cache_list.m_update_thread, INFINITE);
}

#endif /* SAL_UNX || SAL_W32 */

/* ================================================================= */

/** rtl_cache_depot_wsupdate()
 *  update depot stats and purge excess magazines.
 *
 *  @precond cache->m_depot_lock acquired
 */
static void
rtl_cache_depot_wsupdate (
    rtl_cache_type *       cache,
    rtl_cache_depot_type * depot
)
{
    sal_Size npurge;

    depot->m_prev_min = depot->m_curr_min;
    depot->m_curr_min = depot->m_mag_count;

    npurge = SAL_MIN(depot->m_curr_min, depot->m_prev_min);
    for (; npurge > 0; npurge--)
    {
        rtl_cache_magazine_type * mag = rtl_cache_depot_dequeue (depot);
        if (mag != 0)
        {
            RTL_MEMORY_LOCK_RELEASE(&(cache->m_depot_lock));
            rtl_cache_magazine_clear (cache, mag);
            rtl_cache_free (cache->m_magazine_cache, mag);
            RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_depot_lock));
        }
    }
}

/** rtl_cache_wsupdate()
 *
 *  @precond cache->m_depot_lock released
 */
static void
rtl_cache_wsupdate (
    rtl_cache_type * cache
)
{
    if (cache->m_magazine_cache != 0)
    {
        RTL_MEMORY_LOCK_ACQUIRE(&(cache->m_depot_lock));

        OSL_TRACE(
            "rtl_cache_wsupdate(\"%s\") "
            "[depot: count, curr_min, prev_min] "
            "full: %lu, %lu, %lu; empty: %lu, %lu, %lu",
            cache->m_name,
            cache->m_depot_full.m_mag_count,
            cache->m_depot_full.m_curr_min,
            cache->m_depot_full.m_prev_min,
            cache->m_depot_empty.m_mag_count,
            cache->m_depot_empty.m_curr_min,
            cache->m_depot_empty.m_prev_min
        );

        rtl_cache_depot_wsupdate (cache, &(cache->m_depot_full));
        rtl_cache_depot_wsupdate (cache, &(cache->m_depot_empty));

        RTL_MEMORY_LOCK_RELEASE(&(cache->m_depot_lock));
    }
}

/** rtl_cache_wsupdate_all()
 *
 */
#if defined(SAL_UNX) || defined(SAL_OS2)
static void *
#elif defined(SAL_W32)
static DWORD WINAPI
#endif /* SAL_UNX || SAL_W32 */
rtl_cache_wsupdate_all (void * arg)
{
    unsigned int seconds = (unsigned int)SAL_INT_CAST(sal_uIntPtr, arg);

    RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
    while (!g_cache_list.m_update_done)
    {
        rtl_cache_wsupdate_wait (seconds);
        if (!g_cache_list.m_update_done)
        {
            rtl_cache_type * head, * cache;

            head = &(g_cache_list.m_cache_head);
            for (cache  = head->m_cache_next;
                 cache != head;
                 cache  = cache->m_cache_next)
            {
                rtl_cache_wsupdate (cache);
            }
        }
    }
    RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));

    return (0);
}

/* ================================================================= *
 *
 * cache initialization.
 *
 * ================================================================= */

static void
rtl_cache_once_init (void)
{
    {
        /* list of caches */
        RTL_MEMORY_LOCK_INIT(&(g_cache_list.m_lock));
        (void) rtl_cache_constructor (&(g_cache_list.m_cache_head));
    }
    {
        /* cache: internal arena */
        OSL_ASSERT(gp_cache_arena == 0);

        gp_cache_arena = rtl_arena_create (
            "rtl_cache_internal_arena",
            64,   /* quantum */
            0,    /* no quantum caching */
            NULL, /* default source */
            rtl_arena_alloc,
            rtl_arena_free,
            0     /* flags */
        );
        OSL_ASSERT(gp_cache_arena != 0);

        /* check 'gp_default_arena' initialization */
        OSL_ASSERT(gp_default_arena != 0);
    }
    {
        /* cache: magazine cache */
        static rtl_cache_type g_cache_magazine_cache;

        OSL_ASSERT(gp_cache_magazine_cache == 0);
        (void) rtl_cache_constructor (&g_cache_magazine_cache);

        gp_cache_magazine_cache = rtl_cache_activate (
            &g_cache_magazine_cache,
            "rtl_cache_magazine_cache",
            sizeof(rtl_cache_magazine_type), /* objsize  */
            0,                               /* objalign */
            rtl_cache_magazine_constructor,
            rtl_cache_magazine_destructor,
            0, /* reclaim */
            0, /* userarg: NYI */
            gp_default_arena, /* source */
            RTL_CACHE_FLAG_NOMAGAZINE /* during bootstrap; activated below */
        );
        OSL_ASSERT(gp_cache_magazine_cache != 0);

        /* activate magazine layer */
        g_cache_magazine_cache.m_magazine_cache = gp_cache_magazine_cache;
    }
    {
        /* cache: slab (struct) cache */
        static rtl_cache_type g_cache_slab_cache;

        OSL_ASSERT(gp_cache_slab_cache == 0);
        (void) rtl_cache_constructor (&g_cache_slab_cache);

        gp_cache_slab_cache = rtl_cache_activate (
            &g_cache_slab_cache,
            "rtl_cache_slab_cache",
            sizeof(rtl_cache_slab_type), /* objsize  */
            0,                           /* objalign */
            rtl_cache_slab_constructor,
            rtl_cache_slab_destructor,
            0,                           /* reclaim */
            0,                           /* userarg: none */
            gp_default_arena,            /* source */
            0                            /* flags: none */
        );
        OSL_ASSERT(gp_cache_slab_cache != 0);
    }
    {
        /* cache: bufctl cache */
        static rtl_cache_type g_cache_bufctl_cache;

        OSL_ASSERT(gp_cache_bufctl_cache == 0);
        (void) rtl_cache_constructor (&g_cache_bufctl_cache);

        gp_cache_bufctl_cache = rtl_cache_activate (
            &g_cache_bufctl_cache,
            "rtl_cache_bufctl_cache",
            sizeof(rtl_cache_bufctl_type), /* objsize */
            0,                             /* objalign  */
            0,                /* constructor */
            0,                /* destructor */
            0,                /* reclaim */
            0,                /* userarg */
            gp_default_arena, /* source */
            0                 /* flags: none */
        );
        OSL_ASSERT(gp_cache_bufctl_cache != 0);
    }

    rtl_cache_wsupdate_init();
}

static int
rtl_cache_init (void)
{
    static sal_once_type g_once = SAL_ONCE_INIT;
    SAL_ONCE(&g_once, rtl_cache_once_init);
    return (gp_cache_arena != 0);
}

/* ================================================================= */

#if defined(__GNUC__)
static void rtl_cache_fini (void) __attribute__((destructor));
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#pragma fini(rtl_cache_fini)
static void rtl_cache_fini (void);
#endif /* __GNUC__ || __SUNPRO_C */

void
rtl_cache_fini (void)
{
    if (gp_cache_arena != 0)
    {
        rtl_cache_type * cache, * head;

        rtl_cache_wsupdate_fini();

        if (gp_cache_bufctl_cache != 0)
        {
            cache = gp_cache_bufctl_cache, gp_cache_bufctl_cache = 0;
            rtl_cache_deactivate (cache);
            rtl_cache_destructor (cache);
        }
        if (gp_cache_slab_cache != 0)
        {
            cache = gp_cache_slab_cache, gp_cache_slab_cache = 0;
            rtl_cache_deactivate (cache);
            rtl_cache_destructor (cache);
        }
        if (gp_cache_magazine_cache != 0)
        {
            cache = gp_cache_magazine_cache, gp_cache_magazine_cache = 0;
            rtl_cache_deactivate (cache);
            rtl_cache_destructor (cache);
        }
        if (gp_cache_arena != 0)
        {
            rtl_arena_destroy (gp_cache_arena);
            gp_cache_arena = 0;
        }

        RTL_MEMORY_LOCK_ACQUIRE(&(g_cache_list.m_lock));
        head = &(g_cache_list.m_cache_head);
        for (cache = head->m_cache_next; cache != head; cache = cache->m_cache_next)
        {
            OSL_TRACE(
                "rtl_cache_fini(\"%s\") "
                "[slab]: allocs: %"PRIu64", frees: %"PRIu64"; total: %lu, used: %lu; "
                "[cpu]: allocs: %"PRIu64", frees: %"PRIu64"; "
                "[total]: allocs: %"PRIu64", frees: %"PRIu64"",
                cache->m_name,
                cache->m_slab_stats.m_alloc, cache->m_slab_stats.m_free,
                cache->m_slab_stats.m_mem_total, cache->m_slab_stats.m_mem_alloc,
                cache->m_cpu_stats.m_alloc, cache->m_cpu_stats.m_free,
                cache->m_slab_stats.m_alloc + cache->m_cpu_stats.m_alloc,
                cache->m_slab_stats.m_free + cache->m_cpu_stats.m_free
            );
        }
        RTL_MEMORY_LOCK_RELEASE(&(g_cache_list.m_lock));
    }
}

/* ================================================================= */
