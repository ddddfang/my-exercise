#ifndef __NETDEV_H
#define __NETDEV_H

#include "utils.h"
#include "skbuff.h"

//逻辑网卡的定义-----------------------------------------

#define NETDEV_ALEN 6
#define NETDEV_NLEN 16  /* IFNAMSIZ */

struct netstats {
    unsigned int rx_packets;
    unsigned int tx_packets;
    unsigned int rx_errors;
    unsigned int tx_errors;
    unsigned int rx_bytes;
    unsigned int tx_bytes;
};

struct netdev_ops {
    int (*init)(struct netdev *);
    void (*exit)(struct netdev *);
    int (*xmit)(struct netdev *, uint8_t *data, uint32_t len);
};

struct netdev {
    struct list_head net_list;      /* net device list */

    int net_mtu;
    unsigned int net_ipaddr;        /* dev binding ip address */
    unsigned int net_mask;          /* netmask */
    unsigned char net_hwaddr[NETDEV_ALEN];  /* hardware address */
    unsigned char net_name[NETDEV_NLEN];    /* device name */
    struct netdev_ops *net_ops;     /* Nic Operation */
    struct netstats net_stats;      /* protocol independent statistic */
};

struct netdev *netdev_get(uint32_t sip);

struct netdev *netdev_alloc(char *devstr, struct netdev_ops *netops);
void netdev_free(struct netdev *dev);
struct netdev *netdev_find(uint32_t sip);
int net_tx(struct sk_buff *skb, uint8_t *dst_hw, uint16_t ethertype);
int net_rx(struct sk_buff *skb);
void netdev_init(void);
void netdev_exit(void);


#ifdef DEBUG_ETH

    #define eth_dbg(msg, hdr)                                               \
    do {                                                                \
        DEBUG_PRINT("eth "msg" ("                                       \
        "dmac: %.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx, " \
        "smac: %.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx:%.2hhx, " \
        "ethertype: %.4hx)\r\n",                               \
        hdr->dmac[0], hdr->dmac[1], hdr->dmac[2], hdr->dmac[3], \
        hdr->dmac[4], hdr->dmac[5], hdr->smac[0], hdr->smac[1], \
        hdr->smac[2], hdr->smac[3], hdr->smac[4], hdr->smac[5], \
        hdr->ethertype); \
    } while (0)

#else
    #define eth_dbg(msg, hdr)
#endif



//以太网数据包-----------------------------------------

#define BUFLEN 1600

struct eth_hdr
{
    unsigned char dmac[6];
    unsigned char smac[6];
    uint16_t ethertype;
    unsigned char payload[];
} __attribute__((packed));

#define ETH_HDR_LEN sizeof(struct eth_hdr)

static inline struct eth_hdr *skb2eth(struct sk_buff *skb)
{
    struct eth_hdr *hdr = (struct eth_hdr *)skb_head(skb);
    hdr->ethertype = ntohs(hdr->ethertype);
    return hdr;
}



#define LOCALNET(dev) ((dev)->net_ipaddr & (dev)->net_mask)


#endif
