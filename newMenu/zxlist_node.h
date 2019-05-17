#ifndef _ZX_LIST_H_
#define _ZX_LIST_H_
#ifdef __cplusplus
extern "C"
{
#endif


struct zxlist_node
{
    struct zxlist_node *next;
    struct zxlist_node *prev;
};

#define ZX_ListNodeToItem(node, container, member) \
    (container *) (((char*) (node)) - ((unsigned long) &((container *)0)->member))

#define ZX_ListDeclare(name) \
    struct zxlist_node name = { \
        .next = &name, \
        .prev = &name, \
    }

#define ZX_ListForEach(node, list) \
    for (node = (list)->next; node != (list); node = node->next)

#define ZX_ListForEachReverse(node, list) \
    for (node = (list)->prev; node != (list); node = node->prev)

#define ZX_ListForEachSafe(node, n, list) \
    for (node = (list)->next, n = node->next; \
         node != (list); \
         node = n, n = node->next)

static inline void ZX_ListInit(struct zxlist_node *node)
{
    node->next = node;
    node->prev = node;
}
    
static inline void ZX_ListAddTail(struct zxlist_node *head, struct zxlist_node *item)
{
    item->next = head;
    item->prev = head->prev;
    head->prev->next = item;
    head->prev = item;
}

static inline void ZX_ListAddHead(struct zxlist_node *head, struct zxlist_node *item)
{
    item->next = head->next;
    item->prev = head;
    head->next->prev = item;
    head->next = item;
}

static inline void ZX_ListRemove(struct zxlist_node *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
}

#define ZX_ListEmpty(list) ((list) == (list)->next)
#define ZX_ListHead(list) ((list)->next)
#define ZX_ListTail(list) ((list)->prev)


#ifdef __cplusplus
}
#endif
#endif //ZX_LIST_H_
