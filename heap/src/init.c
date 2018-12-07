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

struct heap_region *_g_start;

int heap_region_add(void *start_addr, size_t n, heap_caps_t caps)
{
    struct heap_blk *blk_start, *blk_end;
    struct heap_region *region = (struct heap_region *)HEAP_ALIGN_AFTER(start_addr);
    size_t size = HEAP_ALIGN_BEFORE(n - (HEAP_ALIGN_AFTER(start_addr) - (size_t)start_addr));

    if (n < sizeof(struct heap_region) + HEAP_REGION_MIN)
        return -EINVAL;

    region->start = (uint8_t *)region + sizeof(struct heap_region);
    region->size = size - sizeof(struct heap_region) - sizeof(struct heap_blk);
    region->caps = caps;
    region->free = region->start;
    region->free_size = region->size;
#ifdef CONFIG_USE_HEAP_MIN_SIZE
    region->min_size = region->size;
#endif
    region->next = NULL;

    blk_start = (struct heap_blk *)region->start;
    blk_start->prev = NULL;
    blk_start->flags = 0;

    blk_end = (struct heap_blk *)(region->start + region->size - sizeof(struct heap_blk));
    blk_end->prev = (struct heap_blk *)blk_start;
    blk_end->next = NULL;
    blk_end->flags = HEAP_BLK_USED_FLAGS;
    
    blk_start->next = blk_end;

    if (!_g_start)
        _g_start = region;
    else {
        struct heap_region *tmp = _g_start;

        while (tmp->next)
            tmp = tmp->next;

        tmp->next = region;
    }

    HEAP_DBG(HEAP_DBG_INFO, "heap start %p end %p size %lu", blk_start, blk_end, region->size);

    return 0;
}
