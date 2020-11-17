#ifndef __UTILS_H
#define __UTILS_H

#include "main.h"


#define DEBUG 1
#define TAG


#if DEBUG

#define DEBUG_PRINT(str, ...)                       \
    printf("%s:%u: " TAG": " str, __FILE__, __LINE__, ##__VA_ARGS__);
#define ERROR_PRINT(str, ...)                     \
    fprintf(stderr, "%s:%u: " TAG": " str, __FILE__, __LINE__, ##__VA_ARGS__);

#else

#define DEBUG_PRINT(str, ...)
#define ERROR_PRINT(str, ...)

#endif


#define CLEAR(x) memset(&(x), 0, sizeof(x))
int run_cmd(char *cmd, ...);








struct list_head {
    struct list_head *next;
    struct list_head *prev;
};

#define LIST_HEAD(name) \
    struct list_head name = { &(name), &(name) }

static inline void list_init(struct list_head *head)
{
    head->prev = head->next = head;
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
    head->next->prev = new;
    new->next = head->next;
    new->prev = head;
    head->next = new;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
    head->prev->next = new;
    new->prev = head->prev;
    new->next = head;
    head->prev = new;
}

static inline void list_del(struct list_head *elem)
{
    struct list_head *prev = elem->prev;
    struct list_head *next = elem->next;

    prev->next = next;
    next->prev = prev;
}

//#define ListNodeToItem(ptr, type, member) 
//    (type *) (((char*) (ptr)) - ((unsigned long) &((type *)0)->member))

#define list_entry(ptr, type, member) \
    ((type *) ((char *) (ptr) - offsetof(type, member)))

#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

//fang: when del with delete realted operation
#define list_for_each_safe(pos, p, head)    \
    for (pos = (head)->next, p = pos->next; \
    pos != (head);                     \
    pos = p, p = pos->next)

static inline int list_empty(struct list_head *head)
{
    return head->next == head;
}






#endif


