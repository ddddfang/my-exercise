#ifndef __SKBUFF_H
#define __SKBUFF_H

#include "utils.h"

//pk_type valid values
#define PKT_NONE        0
#define PKT_LOCALHOST   1
#define PKT_OTHERHOST   2
#define PKT_MULTICAST   3
#define PKT_BROADCAST   4

struct pkbuf {
    struct list_head pk_list;   /* for ip fragment or arp waiting list */
    unsigned short pk_pro;      /* ethernet packet type ID, eg IP / ARP / ... */
    unsigned short pk_type;     /* packet hardware address type */
    int pk_len;     //valid data len, of pk_data[]
    int pk_refcnt;
    struct netdev *pk_indev;    //which net interface we get this packet from.
    struct rtentry *pk_rtdst;
    //struct sock *pk_sk;
    unsigned char pk_data[0];
}; //__attribute__((packed));


struct pkbuf *alloc_pkb(int size);
struct pkbuf *copy_pkb(struct pkbuf *pkb);
void get_pkb(struct pkbuf *pkb);
void pkb_trim(struct pkbuf *pkb, int len);
void free_pkb(struct pkbuf *pkb);

void pkbdbg(struct pkbuf *pkb);

//struct sk_buff_head {
//    struct list_head head;
//    uint32_t qlen;
//};

//static inline uint32_t skb_queue_len(const struct sk_buff_head *list)
//{
//    return list->qlen;
//}

//static inline void skb_queue_init(struct sk_buff_head *list)
//{
//    list_init(&list->head);
//    list->qlen = 0;
//}

////fang: 在 next 的前面插入
//static inline void skb_queue_add(struct sk_buff_head *list, struct sk_buff *new, struct sk_buff *next)
//{
//    list_add_tail(&new->list, &next->list);
//    list->qlen += 1;
//}

//static inline void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *new)
//{
//    list_add_tail(&new->list, &list->head);
//    list->qlen += 1;
//}

//static inline struct sk_buff *skb_dequeue(struct sk_buff_head *list)
//{
//    struct sk_buff *skb = list_first_entry(&list->head, struct sk_buff, list);
//    list_del(&skb->list);
//    list->qlen -= 1;
//    return skb;
//}

//static inline int skb_queue_empty(const struct sk_buff_head *list)
//{
//    return skb_queue_len(list) < 1;
//}

//static inline struct sk_buff *skb_peek(struct sk_buff_head *list)
//{
//    if (skb_queue_empty(list))
//        return NULL;
//    return list_first_entry(&list->head, struct sk_buff, list);
//}

//static inline void skb_queue_free(struct sk_buff_head *list)
//{
//    struct sk_buff *skb = NULL;

//    while ((skb = skb_peek(list)) != NULL) {
//        skb_dequeue(list);
//        skb->refcnt--;
//        free_skb(skb);
//    }
//}

#endif

