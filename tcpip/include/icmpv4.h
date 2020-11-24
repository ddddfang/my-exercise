#ifndef __ICMPV4_h
#define __ICMPV4_h

#include "main.h"
#include "skbuff.h"


struct icmp_v4 {
    uint8_t type;
    uint8_t code;
    uint16_t csum;
    uint8_t data[];
} __attribute__((packed));

struct icmp_v4_echo {
    uint16_t id;
    uint16_t seq;
    uint8_t data[];
} __attribute__((packed));

struct icmp_v4_dst_unreachable {
    uint8_t unused;
    uint8_t len;
    uint16_t var;
    uint8_t data[];
} __attribute__((packed));


//void icmpv4_incoming(struct sk_buff *skb);
//void icmpv4_reply(struct sk_buff *skb);

#endif



