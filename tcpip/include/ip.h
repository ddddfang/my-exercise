#ifndef __IP_H
#define __IP_H

#include "main.h"
#include "skbuff.h"
#include "netdev.h"


#ifdef DEBUG_IP
    #define ip_dbg(msg, hdr)                                                \
        do {                                                                \
            DEBUG_PRINT("ip "msg" (ihl: %hhu version: %hhu tos: %hhu "   \
                "len %hu id: %hu frag_offset: %hu ttl: %hhu " \
                "proto: %hhu csum: %hx " \
                "saddr: %hhu.%hhu.%hhu.%hhu daddr: %hhu.%hhu.%hhu.%hhu)\r\n", \
                hdr->ihl,                                           \
                hdr->version, hdr->tos, hdr->len, hdr->id,          \
                hdr->frag_offset, hdr->ttl, hdr->proto, hdr->csum,   \
                hdr->saddr >> 24, hdr->saddr >> 16, hdr->saddr >> 8, hdr->saddr >> 0, \
                hdr->daddr >> 24, hdr->daddr >> 16, hdr->daddr >> 8, hdr->daddr >> 0); \
        } while (0)
#else
    #define ip_dbg(msg, hdr)
#endif


struct ip_hdr {
    uint8_t ihl : 4; /* TODO: Support Big Endian hosts */
    uint8_t version : 4;
    uint8_t tos;        /* type of service */
    uint16_t len;       /* total ip packet data length */
    uint16_t id;        /* datagram id */
    uint16_t frag_offset;   /* fragment offset(in 8-octet's) */
    uint8_t ttl;        /* time to live, in gateway hops */
    uint8_t proto;      /* L4 protocol */
    uint16_t csum;      /* header checksum */
    uint32_t saddr;     /* source address */
    uint32_t daddr;     /* dest address */
    uint8_t data[];
} __attribute__((packed));

#define IP_ALEN 4
#define IP_HDR_LEN sizeof(struct ip_hdr)

#define IP_VERSION_4	4

#define IP_FRAG_RS	0x8000
#define IP_FRAG_DF	0x4000
#define IP_FRAG_MF	0x2000
#define IP_FRAG_OFF	0x1fff
#define IP_FRAG_MASK	(IP_FRAG_OFF | IP_FRAG_MF)

#define IP_P_IP		0
#define IP_P_ICMP	1
#define IP_P_IGMP	2
#define IP_P_TCP	6
#define IP_P_EGP	8
#define IP_P_UDP	17
#define IP_P_OSPF	89
#define IP_P_RAW	255
#define IP_P_MAX	256

#define MULTICAST(netip) ((0x000000f0 & (netip)) == 0x000000e0)
#define BROADCAST(netip) (((0xff000000 & (netip)) == 0xff000000) ||\
                ((0xff000000 & (netip)) == 0x00000000))


#define ipver(ip) ((ip)->version)
#define iphlen(ip) ((ip)->ihl << 2)
#define ipdlen(ip) ((ip)->len - iphlen(ip))
#define ipndlen(nip) (ntohs((nip)->len) - iphlen(nip))
#define ipoff(ip) ((((ip)->frag_offset) & IP_FRAG_OFF) * 8)


static inline void ip_ntoh(struct ip_hdr *iphdr)
{
    iphdr->len = ntohs(iphdr->len);
    iphdr->id = ntohs(iphdr->id);
    iphdr->frag_offset = ntohs(iphdr->frag_offset);
}
#define ip_hton(ip) ip_ntoh(ip)

static inline int equsubnet(unsigned int mask, unsigned int ip1, unsigned int ip2)
{
    return ((mask & ip1) == (mask & ip2));
}
//static inline uint32_t ip_parse(char *addr)
//{
//    uint32_t dst = 0;

//    //将点分文本的IP地址转换为 网络字节序(大端) 的IP地址
//    if (inet_pton(AF_INET, addr, &dst) != 1) {
//        perror("ERR: Parsing inet address failed");
//        exit(1);
//    }
//    //将一个long(4Bytes)从net(大端)转换为host(小端)
//    return ntohl(dst);
//}




#endif


