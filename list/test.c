#include <stdlib.h>
#include <stdio.h>
#include "list.h"

#define NUM_MAX 32

typedef struct user {
    list_t list;

    int num;
} user_t;

int main(void)
{
    user_t p[NUM_MAX];
    user_t *q;
    list_t glist;

    list_init(&glist);

    for (int i = 0; i < NUM_MAX; i++) {
        p[i].num = i;
        list_init(&p[i].list);

        list_add(&glist, &p[i].list);
    }

    printf("test1:\n");
    list_for(q, glist) {
        printf("%d ", q->num);
    }
    printf("\n");

    list_for_safe(q, glist) {
        list_remove(&q->list);
    }

    list_for_safe(q, glist) {
        list_remove(&q->list);
    }

    printf("empty : %d\n", list_is_empty(&glist));

    printf("test2:\n");
    list_for(q, glist) {
        printf("%d ", q->num);
    }
    printf("\n");

    printf("empty : %d\n", list_is_empty(&glist));

    for (int i = 0; i < NUM_MAX; i++) {
        p[i].num = NUM_MAX - i;
        list_init(&p[i].list);

        list_add(&glist, &p[i].list);
    }

    printf("empty : %d\n", list_is_empty(&glist));

    printf("test3:\n");
    list_for(q, glist) {
        printf("%d ", q->num);
    }
    printf("\n");

    return 0;
}
