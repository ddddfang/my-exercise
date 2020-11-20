#ifndef __IP_H
#define __IP_H

#include "main.h"
#include "skbuff.h"
#include "tap_if.h"

#define IPV4 0x04
#define IP_TCP 0x06
#define IP_UDP 0x11
#define ICMPV4 0x01


struct iphdr {
    uint8_t ihl : 4; /* TODO: Support Big Endian hosts */
    uint8_t version : 4;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t frag_offset;
    uint8_t ttl;
    uint8_t proto;
    uint16_t csum;
    uint32_t saddr;
    uint32_t daddr;
    uint8_t data[];
} __attribute__((packed));


#define IP_HDR_LEN sizeof(struct iphdr)
//这是计算ip数据包的 数据部分 的长度
#define ip_len(ip) (ip->len - (ip->ihl * 4))

static inline struct iphdr *ip_hdr(const struct sk_buff *skb)
{
    return (struct iphdr *)(skb->head + ETH_HDR_LEN);
}

int ip_rcv(struct sk_buff *skb);


#define DEBUG_IP 1

#ifdef DEBUG_IP
    #define ip_dbg(msg, hdr)                                                \
        do {                                                                \
            DEBUG_PRINT("ip "msg" (ihl: %hhu version: %hhu tos: %hhu "   \
                "len %hu id: %hu frag_offset: %hu ttl: %hhu " \
                "proto: %hhu csum: %hx " \
                "saddr: %hhu.%hhu.%hhu.%hhu daddr: %hhu.%hhu.%hhu.%hhu)", \
                hdr->ihl,                                           \
                hdr->version, hdr->tos, hdr->len, hdr->id,          \
                hdr->frag_offset, hdr->ttl, hdr->proto, hdr->csum,   \
                hdr->saddr >> 24, hdr->saddr >> 16, hdr->saddr >> 8, hdr->saddr >> 0, \
                hdr->daddr >> 24, hdr->daddr >> 16, hdr->daddr >> 8, hdr->daddr >> 0); \
        } while (0)
#else
    #define ip_dbg(msg, hdr)
#endif


#endif


