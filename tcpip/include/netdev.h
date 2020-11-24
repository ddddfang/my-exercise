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
    int (*xmit)(struct netdev *dev, struct pkbuf *pkb);
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

struct netdev *netdev_alloc(char *devstr, struct netdev_ops *netops);
void netdev_free(struct netdev *dev);
struct netdev *netdev_find(uint32_t sip);
int net_tx(struct netdev *dev, struct pkbuf *pkb, int len, unsigned short proto, unsigned char *dst);
int net_rx(struct netdev *dev, struct pkbuf *skb);
void netdev_init(void);
void netdev_exit(void);


#ifdef DEBUG_ETH

    #define eth_dbg(msg, hdr)                                           \
    do {                                                                \
        DEBUG_PRINT("eth "msg" (dmac:"MACFMT", smac:"MACFMT             \
        "ethertype: %.4hx)\r\n",                                        \
        MAC_FMT(hdr->dmac), MAC_FMT(hdr->smac),                         \
        hdr->ethertype); \
    } while (0)

#else
    #define eth_dbg(msg, hdr)
#endif



//以太网数据包-----------------------------------------

struct eth_hdr
{
    unsigned char dmac[NETDEV_ALEN];
    unsigned char smac[NETDEV_ALEN];
    uint16_t ethertype;
    unsigned char payload[];
} __attribute__((packed));

#define ETH_HDR_LEN sizeof(struct eth_hdr)

#define LOCALNET(dev) ((dev)->net_ipaddr & (dev)->net_mask)

static inline int mac_cmp(void *hwa1, void *hwa2)
{
    return memcmp(hwa1, hwa2, NETDEV_ALEN);
}

static inline int is_eth_multicast(unsigned char *hwa)
{
    return (hwa[0] & 0x01);
}

static inline int is_eth_broadcast(unsigned char *hwa)
{
    //ethernet mac broadcast is FF:FF:FF:FF:FF:FF
    return (hwa[0] & hwa[1] & hwa[2] & hwa[3] & hwa[4] & hwa[5]) == 0xff;
}

#endif
