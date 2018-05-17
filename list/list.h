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

#ifndef _LIST_H
#define _LIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct list
{
    struct list *prev, *next;
} list_t;

#define LIST_EXPAND2(a, b)      a ## b
#define LIST_EXPAND(a, b)       LIST_EXPAND2(a, b)
#define LIST_EXPAND_TEMP(tmp)   LIST_EXPAND(tmp, __LINE__)

/*
 * @brief get the relative offset address of "member" in "type"
 *
 * @param type upper container type
 * @param member container internal member name
 *
 * @return relative offset address
 */
#define CONTAIN_OF(type, member) \
    (&(((type *)0)->member))

/*
 * @brief get the upper container base address from offset
 *        address of "member" in "type"
 *
 * @param pos member pointer address
 * @param type upper container type
 * @param member container internal member name
 *
 * @param upper container base address
 */
#define LIST_ENTRY(pos, type, member) \
    ((type *)((char *)pos - (char *)CONTAIN_OF(type, member)))

/*
 * @brief get the next upper container of head base address from
 *        offset address of "member" in "type"
 *
 * @param pos member pointer address
 * @param type upper container type
 * @param member container internal member name
 *
 * @param upper container base address
 */
#define LIST_HEAD_ENTRY(head, type, member) \
    LIST_ENTRY((head)->next, type, member)

/*
 * @brief traverse the upper container of all the list members
 *
 * @param pos upper container base address
 * @param list header list pointer address and it has no upper container
 * @param member container internal member name
 *
 * @note if you may delete or insert new node you should use safe traversing
 */
#define LIST_FOR_EACH_ENTRY(pos, list, type, member)                            \
    for ((pos) = LIST_HEAD_ENTRY(list, type, member);                           \
         &((pos)->member) != (list);                                            \
         (pos) = LIST_HEAD_ENTRY(&pos->member, type, member))

/*
 * @brief traverse the upper container of all the list members safely
 *
 * @param pos upper container base address
 * @param n upper container base address cache and it should not be used
 * @param list header list pointer address and it has no upper container
 * @param member container internal member name
 */
#define LIST_FOR_EACH_ENTRY_SAFE(pos, n, list, type, member)                    \
    for ((pos) = LIST_HEAD_ENTRY(list, type, member),                           \
         (n) = LIST_HEAD_ENTRY(&pos->member, type, member);                 \
         &((pos)->member) != (list);                                            \
         (pos) = (n), (n) = LIST_HEAD_ENTRY(&pos->member, type, member))

/*
 * @brief traverse the upper container of all the list members
 *
 * @param p upper container base address
 * @param list header list pointer address and it has no upper container
 *
 * @note if you may delete or insert new node you should use safe traversing,
 *       and the internl member name must be "list"
 */
#define LIST_FOR(p, l) \
    LIST_FOR_EACH_ENTRY((p), &(l), typeof(*(p)), list)

/*
 * @brief traverse the upper container of all the list members
 *
 * @param p upper container base address
 * @param list header list pointer address and it has no upper container
 *
 * @note if you may delete or insert new node you should use safe traversing,
 *       and the internl member name must be "list"
 */
#define list_for(p, l) \
    LIST_FOR(p, l)

/*
 * @brief traverse the upper container of all the list members safely
 *
 * @param p upper container base address
 * @param list header list pointer address and it has no upper container
 *
 * @note the internl member name must be "list"
 */
#define LIST_FOR_SAFE(p, l)                                                             \
    typeof(*(p)) *LIST_EXPAND_TEMP(_list);                                              \
    LIST_FOR_EACH_ENTRY_SAFE((p), LIST_EXPAND_TEMP(_list), &(l), typeof(*(p)), list)

/*
 * @brief traverse the upper container of all the list members safely
 *
 * @param p upper container base address
 * @param list header list pointer address and it has no upper container
 *
 * @note the internl member name must be "list"
 */
#define list_for_safe(p, l) \
    LIST_FOR_SAFE(p, l)

/*
 * @brief insert new to next of node "link"
 *
 * @param link list node pointer
 * @param new added node
 */
#define list_add(link, new) \
    list_add_link(link, (link)->next, new)

/*
 * @brief insert new to tail(previous) of node "link"
 *
 * @param link list node pointer
 * @param new added node
 */
#define list_add_tail(link, new) \
    list_add_link((link)->prev, link, new)

/*
 * @brief initialize list node
 *
 * @param link list node pointer
 */
static inline void list_init(list_t *link)
{
    link->prev = link;
    link->next = link;
}

/*
 * @brief check if list is empty(next == self)
 *
 * @param link list node pointer
 *
 * @return 0 if it is empty or 1
 */
static inline int list_is_empty(list_t *link)
{
    return (link->next == link);
}

/*
 * @brief remove the list node from list
 *
 * @param link list node pointer
 */
static inline void list_remove(list_t *link)
{
    link->prev->next = link->next;
    link->next->prev = link->prev;
}

/*
 * @brief insert the list node between prev and next
 *
 * @param prev previous list node pointer
 * @param prev next list node pointer
 * @param new added list node pointer
 */
static inline void list_add_link(list_t *prev, list_t *next, list_t *new)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

#ifdef __cplusplus
}
#endif

#endif /* _LIST_H */
