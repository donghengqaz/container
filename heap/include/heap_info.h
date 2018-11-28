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

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "heap_config.h"

#define HEAP_DBG_TRACE                  1
#define HEAP_DBG_ERR                    2
#define HEAP_DBG_INFO                   3

#define MALLOC_CAP_32BIT	            (1 << 1)    ///< Memory must allow for aligned 32-bit data accesses
#define MALLOC_CAP_8BIT	                (1 << 2)    ///< Memory must allow for 8-bit data accesses
#define MALLOC_CAP_DMA		            (1 << 3)    ///< Memory must be able to accessed by DMA

#define HEAP_REGION_MIN                 (4 * 1024)

#define HEAP_ALIGN_SIZE                 sizeof(uintptr_t)
#define HEAP_ALIGN_AFTER(_addr)         (((size_t)_addr + HEAP_ALIGN_SIZE - 1) & ~(HEAP_ALIGN_SIZE - 1))
#define HEAP_ALIGN_BEFORE(_addr)        ((size_t)_addr & ~(HEAP_ALIGN_SIZE - 1))

#ifdef CONFIG_HEAP_TRACE
#define HEAP_BLK_TRACE                  0x80000000
#define HEAP_BLK_SIZE_MIN               (sizeof(struct heap_blk_trace) + HEAP_ALIGN_SIZE)
#else
#define HEAP_BLK_TRACE                  0
#define HEAP_BLK_SIZE_MIN               (sizeof(struct heap_blk) + HEAP_ALIGN_SIZE)
#endif

#define HEAP_BLK_TRACE_MASK             (~HEAP_BLK_TRACE)
#define HEAP_BLK_USED_FLAGS             (0x5a5a5a5a & HEAP_BLK_TRACE_MASK)

typedef uint32_t                        heap_caps_t;
typedef uintptr_t                       heap_lock_t;

struct heap_region {
    uint8_t                 *start;
    size_t                  size;

    heap_caps_t             caps;

    uint8_t                 *free;

    size_t                  free_size;
#ifdef CONFIG_USE_HEAP_MIN_SIZE
    size_t                  min_size;
#endif

    struct heap_region      *next;
};

struct heap_blk {
    struct heap_blk         *prev, *next;
    size_t                  flags;
};

#ifdef CONFIG_HEAP_TRACE
struct heap_blk_trace {
    struct heap_blk         blk;
    
    const char              *file;
    size_t                  line;
};
#endif

static inline size_t get_blk_size(struct heap_blk *blk)
{
    return (size_t)blk->next - (size_t)blk;
}
