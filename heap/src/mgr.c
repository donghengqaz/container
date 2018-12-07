//    Copyright 2018-2019 Dong Heng

//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at

//        http://www.apache.org/licenses/LICENSE-2.0

//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.

#include <string.h>
#include <stddef.h>
#include <sys/errno.h>
#include "heap.h"
#include "heap_port.h"

void *__heap_caps_alloc(size_t n, heap_caps_t caps, const char *file, int line)
{
    extern struct heap_region *_g_start;
    extern int _g_heap_trace_mode;

    void *mem = NULL;
#ifdef CONFIG_HEAP_TRACE
    int trace = _g_heap_trace_mode;
    const size_t head_size = trace == 0 ? sizeof(struct heap_blk) : sizeof(struct heap_blk_trace);
#else
    const size_t head_size = sizeof(struct heap_blk);
#endif
    const size_t blk_size = HEAP_ALIGN_AFTER(head_size + n);

    HEAP_DBG(HEAP_DBG_TRACE, "alloc size is %lu blk size is %lu", n, blk_size);

    for (struct heap_region *region = _g_start; region; region = region->next) {
        heap_lock_t lock;
        struct heap_blk *blk, *blk_next;
        size_t blk_real_size;

        if (region->caps & caps != caps)
            continue;

        lock = heap_lock(region);

        if (blk_size > region->free_size)
            goto next_region;

        blk = (struct heap_blk *)region->free;

        HEAP_DBG(HEAP_DBG_TRACE, "alloc start %p", blk);

        while (blk->next && (blk->flags || (blk_real_size = get_blk_size(blk)) < blk_size)) {
            HEAP_DBG(HEAP_DBG_TRACE, "alloc blk %p next %p %s %s size %lu", blk, blk->next,
                                blk->flags ? "use" : "unuse",
                                blk->flags & HEAP_BLK_TRACE ? "trace" : "no-trace",
                                blk_real_size);
            blk = blk->next;
        }

        if (!blk->next)
            goto next_region;

        if (blk_real_size >= blk_size + HEAP_BLK_SIZE_MIN) {
            struct heap_blk *next = blk->next;

            blk_next = (struct heap_blk *)((uint8_t *)blk + blk_size);
            blk_next->flags = 0;
            blk_next->next = next;
            blk_next->prev = blk;

            HEAP_DBG(HEAP_DBG_TRACE, "blk %p prev %p next %p", blk, blk->prev, blk->next);

            next->prev = blk_next;
            blk->next = blk_next;

            blk_real_size = blk_size;
        } else
            blk_next = blk->next;

        HEAP_DBG(HEAP_DBG_TRACE, "next blk %p", blk_next);

#ifdef CONFIG_HEAP_TRACE
        if (trace) {
            struct heap_blk_trace *blk_trace = (struct heap_blk_trace *)blk;
            blk_trace->file = file;
            blk_trace->line = line | HEAP_BLK_TRACE;
            blk_trace->blk.flags = HEAP_BLK_USED_FLAGS | HEAP_BLK_TRACE; 
        } else
#endif
            blk->flags = HEAP_BLK_USED_FLAGS;

        if (region->free == (uint8_t *)blk) {
            struct heap_blk *free = blk->next;

            while (free->next && free->flags)
                free = free->next;

            HEAP_DBG(HEAP_DBG_TRACE, "free from %p to %p", region->free, free);
            region->free = (uint8_t *)free;
        }

        region->free_size -= blk_real_size;

#ifdef CONFIG_USE_HEAP_MIN_SIZE
        if (region->min_size > region->free_size)
            region->min_size = region->free_size;
#endif

        mem = (uint8_t *)blk + head_size;

next_region:
        heap_unlock(region, lock);

        if (mem)
            break;
    }

    return mem;
}

void __heap_caps_free(void *ptr, const char *file, int line)
{
    struct heap_region *_g_start;

    struct heap_region *region;
    struct heap_blk *blk, *prev, *next;
    heap_lock_t lock;

    if (line == 0)
        HEAP_DBG(HEAP_DBG_TRACE, "func %p", file);
    else
        HEAP_DBG(HEAP_DBG_TRACE, "file %s line %d", file, line); 

    if (!ptr) {
        HEAP_DBG(HEAP_DBG_ERR, "free NULL");
        return ;
    }

    for (region = _g_start; region; region = region->next) {
        if ((uint8_t *)ptr > region->start && (uint8_t *)ptr < region->start + region->size)
            break;
    }

    if (!region) {
        HEAP_DBG(HEAP_DBG_ERR, "region NULL");
        return ;
    }

    lock = heap_lock(region);

    blk = (struct heap_blk *)((uint8_t *)ptr - sizeof(struct heap_blk));
    if (!blk->flags) {
        HEAP_DBG(HEAP_DBG_ERR, "blk %p already freed\n", ptr);
        goto exit;
    }

#ifdef CONFIG_HEAP_TRACE
    if (blk->flags & HEAP_BLK_TRACE) {
        blk->flags = 0;
        blk = (struct heap_blk *)((uint8_t *)ptr - sizeof(struct heap_blk_trace));
    }
#endif

    region->free_size += get_blk_size(blk);

    prev = blk->prev;
    next = blk->next;

    if (!prev || prev->flags)
        prev = blk;

    if (!next->flags)
        next = next->next;

    blk->flags = 0;
    if (prev != next) {
        prev->next = next;
        next->prev = prev;
    }

    if ((uint8_t *)blk < region->free) {
        HEAP_DBG(HEAP_DBG_TRACE, "free from %p to %p", region->free, blk);
        region->free = (uint8_t *)blk;
    }

exit:
    heap_unlock(region, lock);

    return ;
}
