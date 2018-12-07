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

#ifndef CONFIG_HEAP_TRACE_INIT_STATE
#define CONFIG_HEAP_TRACE_INIT_STATE    0
#endif

#ifdef CONFIG_TRACE_ALL
#define HEAP_INFO_STATE " is %s"
#define HEAP_INFO_STATE_PARAM(_p) ,_p->flags?"used":"freed"
#else
#define HEAP_INFO_STATE ""
#define HEAP_INFO_STATE_PARAM(_p)
#endif

#ifdef CONFIG_TRACE_MEM_LINK
#define HEAP_INFO "p %p prev %p next %p size %d"HEAP_INFO_STATE
#define HEAP_INFO_PARAM(_p) (_p),(_p)->prev,(_p)->next,get_blk_size(_p)HEAP_INFO_STATE_PARAM(_p)
#else
#define HEAP_INFO "mem @%p size %lu"HEAP_INFO_STATE
#define HEAP_INFO_PARAM(_p) (_p),get_blk_size(_p)HEAP_INFO_STATE_PARAM(_p)
#endif

#ifdef CONFIG_HEAP_TRACE
int _g_heap_trace_mode = CONFIG_HEAP_TRACE_INIT_STATE;
#endif

/**
 * @brief Dump heap trace record data to stdout
 */
void heap_trace_dump(void)
{
    extern struct heap_region *_g_start;

    struct heap_region *region;

    for (region = _g_start; region; region = region->next) {
        heap_lock_t lock;
        struct heap_blk *p;
        struct heap_blk *start = (struct heap_blk *)region->start;
        struct heap_blk *end = (struct heap_blk *)(region->start + region->size - sizeof(struct heap_blk));

        lock = heap_lock(region);

        HEAP_DBG(HEAP_DBG_INFO, "\r\n\r\n");
        HEAP_DBG(HEAP_DBG_TRACE, "start %p end %p", start, start);
        HEAP_DBG(HEAP_DBG_TRACE, "free blk %p", region->free);
        HEAP_DBG(HEAP_DBG_TRACE, "size %lu", region->free_size);
#ifdef CONFIG_USE_HEAP_MIN_SIZE
        HEAP_DBG(HEAP_DBG_TRACE, "min %lu", region->min_size);
#endif

        p = start;
        while (p != end) {
#ifdef CONFIG_HEAP_TRACE
            if (p->flags && p->flags & HEAP_BLK_TRACE) {
                struct heap_blk_trace *blk = (struct heap_blk_trace *)p;
                size_t line = blk->line & HEAP_BLK_TRACE_MASK;

                if (!line) {
                    HEAP_DBG(HEAP_DBG_INFO, HEAP_INFO " caller func %p", HEAP_INFO_PARAM(p), blk->file);
                } else {
                    const char *file = rindex(blk->file, '/');
                    if (file)
                        file++;
                    else
                        file = blk->file;

                    HEAP_DBG(HEAP_DBG_INFO, HEAP_INFO " caller file %s line %lu", HEAP_INFO_PARAM(p), file, line);
                }
            } else
#endif
#ifdef CONFIG_TRACE_ALL
            {
                HEAP_DBG(HEAP_DBG_INFO, HEAP_INFO, HEAP_INFO_PARAM(p));
            }
#endif
            p = p->next;

            heap_feed_wd();
        }

        heap_unlock(region, lock);
    }
}
