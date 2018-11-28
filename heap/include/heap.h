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

#include "heap_info.h"

#ifdef __cplusplus
extern "C" {
#endif

int heap_region_add(void *start_addr, size_t n, heap_caps_t caps);


void *__heap_caps_alloc(size_t n, heap_caps_t caps, const char *file, int line);
#define heap_caps_alloc(_n, _c) __heap_caps_alloc(_n, _c, __FILE__, __LINE__)


void __heap_caps_free(void *ptr, const char *file, int line);
#define heap_caps_free(_p)      __heap_caps_free(_p, __FILE__, __LINE__)


/**
 * @brief Dump heap trace record data to stdout
 */
void heap_trace_dump(void);

#ifdef __cplusplus
}
#endif
