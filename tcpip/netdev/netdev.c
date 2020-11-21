
#include "netdev.h"
#include "ip.h"
#include "arp.h"
#include "tap_if.h"

//我们有一个固定的loop逻辑网卡
static struct netdev *loop;

//其他的网卡使用list串起来
static struct list_head net_devices;


/* Alloc localhost net devices */
struct netdev *netdev_alloc(char *devstr, struct netdev_ops *netops)
{
    struct netdev *dev = calloc(1, sizeof(*dev));
    if (!dev) {
        return NULL;
    }
    memset(dev->net_name, 0, NETDEV_NLEN);
    strcpy((char *)dev->net_name, devstr);
    dev->net_ops = netops;

    list_add_tail(&dev->net_list, &net_devices);
    if (netops && netops->init) {
        netops->init(dev);
    }
    return dev;
}

void netdev_free(struct netdev *dev)
{
    if (dev->net_ops && dev->net_ops->exit)
        dev->net_ops->exit(dev);
    list_del(&dev->net_list);
    free(dev);
}

struct netdev *netdev_find(uint32_t sip)
{
    struct list_head *item;
    struct netdev *entry;
    list_for_each(item, &net_devices) {
        entry = list_entry(item, struct netdev, net_list);
        if (entry->net_ipaddr == sip) {
            return entry;
        }
    }
    return NULL;
}

//看起来 net_tx 并不分配skbuff
int net_tx(struct sk_buff *skb, uint8_t *dst_hw, uint16_t ethertype)
{
//    struct ether *ehdr = (struct ether *)pkb->pk_data;
//
//    /* first copy to eth_dst, maybe eth_src will be copied to eth_dst */
//    ehdr->eth_pro = _htons(proto);
//    hwacpy(ehdr->eth_dst, dst);
//    hwacpy(ehdr->eth_src, dev->net_hwaddr);
//
//    l2dbg(MACFMT " -> " MACFMT "(%s)",
//    macfmt(ehdr->eth_src),
//    macfmt(ehdr->eth_dst),
//    ethpro(proto));
//
//    pkb->pk_len = len + ETH_HRD_SZ;
//    /* real transmit packet */
//    dev->net_ops->xmit(dev, pkb);
//    free_pkb(pkb);

    struct netdev *dev = skb->dev;
    struct eth_hdr *hdr;
    int ret = 0;

    skb_push(skb, ETH_HDR_LEN);
    hdr = (struct eth_hdr *)skb->data;

    memcpy(hdr->dmac, dst_hw, NETDEV_ALEN);
    memcpy(hdr->smac, dev->net_hwaddr, NETDEV_ALEN);

    hdr->ethertype = htons(ethertype);
    eth_dbg("out", hdr);

    if (dev->net_ops && dev->net_ops->xmit)
        ret = dev->net_ops->xmit(dev, skb->data, skb->len);
    return ret;
}

int net_rx(struct sk_buff *skb)
{
//    struct ether *ehdr = eth_init(dev, pkb);
//    if (!ehdr)
//        return;
//    l2dbg(MACFMT " -> " MACFMT "(%s)", macfmt(ehdr->eth_src), macfmt(ehdr->eth_dst), ethpro(pkb->pk_pro));
//    pkb->pk_indev = dev;
//    switch (pkb->pk_pro) {
//        case ETH_P_RARP:
//            //      rarp_in(dev, pkb);
//            break;
//        case ETH_P_ARP:
//            arp_in(dev, pkb);
//            break;
//        case ETH_P_IP:
//            ip_in(dev, pkb);
//            break;
//        default:
//            l2dbg("drop unkown-type packet");
//            free_pkb(pkb);
//            break;
//    }
    struct eth_hdr *hdr = skb2eth(skb);
    eth_dbg("in", hdr);

    switch (hdr->ethertype) {
        case ETH_P_ARP:
            //arp_rcv(skb);
            DEBUG_PRINT("aro_rcv.\r\n");
            break;
        case ETH_P_IP:
            //ip_rcv(skb);
            DEBUG_PRINT("ip_rcv.\r\n");
            break;
        case ETH_P_IPV6:
        default:
            //DEBUG_PRINT("Unsupported ethertype %x\n", hdr->ethertype);
            break;
    }
    return 0;
}

void netdev_init(void)
{
    list_init(&net_devices);
    veth_init();
}

void netdev_exit(void)
{
    veth_exit();
}

struct netdev *netdev_get(uint32_t sip)
{
    struct list_head *item;
    struct netdev *entry;
    list_for_each(item, &net_devices) {
        entry = list_entry(item, struct netdev, net_list);
        if (entry->net_ipaddr == sip) {
            return entry;
        }
    }
    return NULL;
}

