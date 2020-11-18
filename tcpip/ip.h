#ifndef __IP_H
#define __IP_H

#include "main.h"

#define IPV4 0x04
#define IP_TCP 0x06
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


#endif


