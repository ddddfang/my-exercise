#ifndef __TAP_IF_H
#define __TAP_IF_H

#include "main.h"
#include "skbuff.h"

//-------------------------------------------------------

void tun_init();
int tun_read(char *buf, int len);
int tun_write(char *buf, int len);
void free_tun();

//逻辑网卡的定义-----------------------------------------

#define BUFLEN 1600
#define MAX_ADDR_LEN 32

struct netdev {
    uint32_t addr;
    uint8_t addr_len;
    uint8_t hwaddr[6];
    uint32_t mtu;
};

void netdev_init(char *addr, char *hwaddr);
void free_netdev();
struct netdev *netdev_get(uint32_t sip);
int netdev_transmit(struct sk_buff *skb, uint8_t *dst_hw, uint16_t ethertype);
void *netdev_rx_loop(void *arg);

#define DEBUG_ETH 1

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



#endif

