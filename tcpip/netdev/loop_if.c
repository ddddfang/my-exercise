#include "netdev.h"
#include "ip.h"
#include "utils.h"

#define LOOPBACK_MTU        1500
#define LOOPBACK_IPADDR     0x0100007F  /* 127.0.0.1 */
#define LOOPBACK_NETMASK    0x000000FF  /* 255.0.0.0 */

struct netdev *loop;

static int loop_dev_init(struct netdev *dev)
{
    //init veth: information for our netstack
    dev->net_mtu = LOOPBACK_MTU;
    dev->net_ipaddr = LOOPBACK_IPADDR;
    dev->net_mask = LOOPBACK_NETMASK;
    DEBUG_PRINT("%s ip address: " IPFMT "\r\n", dev->net_name, IP_FMT(dev->net_ipaddr));
    DEBUG_PRINT("%s netmask:    " IPFMT "\r\n", dev->net_name, IP_FMT(dev->net_mask));
    /* net stats have been zero */
    return 0;
}

//no need to simulate an interrupt
static void loop_recv(struct netdev *dev, struct pkbuf *pkb)
{
    dev->net_stats.rx_packets++;
    dev->net_stats.rx_bytes += pkb->pk_len;
    net_rx(dev, pkb);  //handle this packet
}

static int loop_dev_xmit(struct netdev *dev, struct pkbuf *pkb)
{
    //ref pkb buffer, because loop_recv may finally call free_pkb,
    //but we want net_tx() handle free
    get_pkb(pkb);

    loop_recv(dev, pkb);    // loop back to itself
    dev->net_stats.tx_packets++;
    dev->net_stats.tx_bytes += pkb->pk_len;

    return pkb->pk_len;
}

static struct netdev_ops loop_ops = {
    .init = loop_dev_init,
    .xmit = loop_dev_xmit,
    .exit = NULL,
};

void loop_init(void)
{
    loop = netdev_alloc("lo", &loop_ops);
}

void loop_exit(void)
{
    netdev_free(loop);
}


