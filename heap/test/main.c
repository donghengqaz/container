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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"

#define TEST_CNT 512
#define BUF_NUM 1024
#define BUF_SIZE 1024

static char buf[BUF_NUM * BUF_SIZE];

int main()
{
    char *p[TEST_CNT];

    heap_region_add(buf, BUF_NUM * BUF_SIZE, MALLOC_CAP_8BIT);

    for (int n = 0; n < 4096; n++) {
        for (int i = 0; i < TEST_CNT; i++) {
            uint32_t m = (uint32_t)rand() % BUF_SIZE;

            p[i] = heap_caps_alloc(m, MALLOC_CAP_8BIT);
            if (!p[i]) {
                printf("malloc NULL\n");
                heap_trace_dump();
                return 0;
            }
            memset(p[i], 0, m);
            //printf("malloc %p %d\n", p[i], m);
        }

        for (int i = 0; i < TEST_CNT; i++) {
            heap_caps_free(p[i]);
            //printf("free %p\n", p[i]);
        }
    }

    printf("test OK\n");

    heap_trace_dump();

    sleep(1);
}
