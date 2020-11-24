#ifndef __ARP_H
#define __ARP_H

#include "main.h"
#include "utils.h"
#include "skbuff.h"
#include "netdev.h"



#ifdef DEBUG_ARP
    #define arp_dbg(str, hdr)                                               \
        do {                                                                \
            DEBUG_PRINT("arp "str" (hwtype: %hu, protype: %.4hx, "          \
                "hwsize: %d, prosize: %d, opcode: %.4hx)\r\n",         \
                hdr->hwtype, hdr->protype, hdr->hwsize,             \
                hdr->prosize, hdr->opcode);                         \
        } while (0)

    #define arpdata_dbg(str, arp_payload)                                      \
        do {                                                            \
            DEBUG_PRINT("arp "str":"IPFMT"("MACFMT")->"IPFMT"(request).\r\n",   \
                        IP_FMT(arp_payload->sip), MAC_FMT(arp_payload->smac),   \
                        IP_FMT(arp_payload->dip));      \
        } while (0)

    #define arpcache_dbg(str, entry) \
        do { \
            DEBUG_PRINT("arp cache "str" (hwtype: %hu, sip: %hhu.%hhu.%hhu.%hhu, " \
                "smac: %.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx, state: %d)\r\n", entry->hwtype, \
                entry->sip >> 24, entry->sip >> 16, entry->sip >> 8, entry->sip >> 0, \
                entry->smac[0], entry->smac[1], entry->smac[2], entry->smac[3], entry->smac[4], \
                entry->smac[5], entry->state); \
        } while (0)
#else
    #define arp_dbg(str, hdr)
    #define arpdata_dbg(str, data)
    #define arpcache_dbg(str, entry)
#endif



/* arp cache */
#define ARP_CACHE_SZ    20

#define ARP_TTL_TIMEOUT     600	/* 10 minutes */
#define ARP_TTL_WAITTIME    1   /* after arp_request(), wait ARP_WAITTIMEs, then check again in timer */

//WAITING 状态应该就是目的mac待填充,等填充完毕就是 RESOLVED
//变成 RESOLVED 后需要将在这个list上pending的pkb发送出去
#define ARP_STATE_FREE        1
#define ARP_STATE_WAITING     2
#define ARP_STATE_RESOLVED    3

#define ARP_REQ_RETRY       4

//ae_list: (fang) means after we figure out the mac, we should sendout packets link on this list?
struct arpentry {
    struct list_head ae_list;		/* packet pending for hard address */
    struct netdev *ae_dev;			/* associated net interface */
    int ae_retry;				/* arp reuqest retrying times */
    int ae_ttl;				/* entry timeout */
    unsigned int ae_state;			/* entry state */
    unsigned short ae_pro;			/* L3 protocol supported by arp */
    unsigned int ae_ipaddr;			/* L3 protocol address(ip) */
    unsigned char ae_hwaddr[ETH_ALEN];	/* L2 protocol address(ethernet) */
};

//
int arp_request(struct arpentry *ae);
int arp_in(struct netdev *dev, struct pkbuf *pkb);
void arp_reply(struct netdev *dev, struct pkbuf *pkb);

//in arp_cache.c
void arp_queue_send(struct arpentry *ae);
void arp_queue_drop(struct arpentry *ae);

struct arpentry *arp_entry_alloc(void);
int arp_cache_insert(struct netdev *nd, unsigned short pro, unsigned int ipaddr, unsigned char *hwaddr);
struct arpentry *arp_cache_lookup(unsigned short pro, unsigned int ipaddr);
void arp_timer(int delta);
void arp_cache_traverse(void);

//--------------------------------------

#define ARP_ETHERNET    0x0001
#define ARP_IPV4        0x0800

#define ARP_OP_REQUEST		0x0001	/* ARP request */
#define ARP_OP_REPLY		0x0002	/* ARP reply */
#define ARP_OP_RREQUEST		0x0003	/* RARP request */
#define ARP_OP_RREPLY		0x0004	/* RARP reply */
#define ARP_OP_INREQUEST	0x0008	/* InARP request */
#define ARP_OP_INREPLY		0x0009	/* InARP reply */

struct arp_hdr
{
    uint16_t hwtype;
    uint16_t protype;
    uint8_t hwsize;
    uint8_t prosize;
    uint16_t opcode;
    unsigned char data[];
} __attribute__((packed));

struct arp_ipv4
{
    unsigned char smac[6];
    uint32_t sip;
    unsigned char dmac[6];
    uint32_t dip;
} __attribute__((packed));

#define ARP_HDR_LEN     sizeof(struct arp_hdr)
#define ARP_DATA_LEN    sizeof(struct arp_ipv4)


#endif

