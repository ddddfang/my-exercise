#include "netdev.h"
#include "ip.h"
#include "utils.h"

#define LOOPBACK_MTU        1500
#define LOOPBACK_IPADDR     0x0100007F  /* 127.0.0.1 */
#define LOOPBACK_NETMASK    0x000000FF  /* 255.0.0.0 */

static struct netdev *loop;

static int loop_dev_init(struct netdev *dev)
{
//    /* init veth: information for our netstack */
//    dev->net_mtu = LOOPBACK_MTU;
//    dev->net_ipaddr = LOOPBACK_IPADDR;
//    dev->net_mask = LOOPBACK_NETMASK;
//    DEBUG_PRINT("%s ip address: " IPFMT, dev->net_name, IP_FMT(dev->net_ipaddr));
//    DEBUG_PRINT("%s netmask:    " IPFMT, dev->net_name, IP_FMT(dev->net_mask));
//    /* net stats have been zero */
    return 0;
}

//static void loop_recv(struct netdev *dev, struct pkbuf *pkb)
//{
//    dev->net_stats.rx_packets++;
//    dev->net_stats.rx_bytes += pkb->pk_len;
//    net_in(dev, pkb);
//}

static int loop_dev_xmit(struct netdev *dev, uint8_t *data, uint32_t len)
{
//    get_pkb(pkb);
//    /* loop back to itself */
//    loop_recv(dev, pkb);
//    dev->net_stats.tx_packets++;
//    dev->net_stats.tx_bytes += pkb->pk_len;
//    return pkb->pk_len;
    return 0;
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


