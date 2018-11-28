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

#include <stdio.h>

typedef unsigned long heap_lock_t;

#define HEAP_DBG(_level, _fmt, ...)         \
    if (_level >= 3)                        \
        printf(_fmt "\r\n", ##__VA_ARGS__)

static inline heap_lock_t heap_lock(struct heap_region *region)
{

}

static inline void heap_unlock(struct heap_region *region, heap_lock_t lock)
{

}

static inline void heap_feed_wd(void)
{
    
}
