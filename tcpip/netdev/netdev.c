
#include "netdev.h"
#include "ip.h"
#include "arp.h"
#include "tap_if.h"
#include "loop_if.h"


//其他的网卡使用list串起来
static struct list_head net_devices;

//full fill some fields, and return eth_hdr in pkb
static struct eth_hdr *eth_init(struct netdev *dev, struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    if (pkb->pk_len < (int)ETH_HDR_LEN) {
        DEBUG_PRINT("received packet is too small:%d bytes\r\n", pkb->pk_len);
        return NULL;
    }
    /* hardware address type */
    if (is_eth_multicast(ehdr->dmac)) {
        if (is_eth_broadcast(ehdr->dmac)) {
            pkb->pk_type = PKT_BROADCAST;
        } else {
            pkb->pk_type = PKT_MULTICAST;
        }
    } else if (!mac_cmp(ehdr->dmac, dev->net_hwaddr)) {
        pkb->pk_type = PKT_LOCALHOST;
    } else {
        pkb->pk_type = PKT_OTHERHOST;
    }
    /* packet protocol */
    pkb->pk_pro = ntohs(ehdr->ethertype);
    pkb->pk_indev = dev;
    return ehdr;
}

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

//init a pkb, and send out it through dev ifterface
//before we call this tx func, data should be in "pkb->pk_data + ETH_HDR_LEN"
//and it's valid length is len(param "int len")
int net_tx(struct netdev *dev, struct pkbuf *pkb, int len, unsigned short proto, unsigned char *dst)
{
    int ret = 0;
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;

    /* first copy to eth_dst, maybe eth_src will be copied to eth_dst */
    ehdr->ethertype = htons(proto);
    memcpy(ehdr->dmac, dst, NETDEV_ALEN);
    memcpy(ehdr->smac, dev->net_hwaddr, NETDEV_ALEN);

    eth_dbg("out", ehdr);

    pkb->pk_len = len + (int)ETH_HDR_LEN;
    /* real transmit packet */
    if (dev && dev->net_ops && dev->net_ops->xmit) {
        ret = dev->net_ops->xmit(dev, pkb);
    }
    free_pkb(pkb);
    return ret;
}

int net_rx(struct netdev *dev, struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = eth_init(dev, pkb);
    if (!ehdr) {
        DEBUG_PRINT("eth_init error\r\n");
        free_pkb(pkb);
        return -1;
    }
    eth_dbg("in", ehdr);
    switch (pkb->pk_pro) {
        case ETH_P_RARP:
            //      rarp_in(dev, pkb);
            break;
        case ETH_P_ARP:
            arp_in(dev, pkb);
            break;
        case ETH_P_IP:
            //ip_in(dev, pkb);
            break;
        default:
            //DEBUG_PRINT("Unsupported ethertype %x\n", hdr->ethertype);
            free_pkb(pkb);
            break;
    }
    return 0;
}

void netdev_init(void)
{
    list_init(&net_devices);
    loop_init();
    veth_init();
}

void netdev_exit(void)
{
    veth_exit();
    loop_exit();
}


