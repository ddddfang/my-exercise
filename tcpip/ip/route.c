#include "route.h"
#include "netdev.h"
#include "loop_if.h"
#include "tap_if.h"
#include "ip.h"

static LIST_HEAD(rt_head);

struct rtentry *rt_lookup(unsigned int ipaddr)
{
    struct list_head *item;
    struct rtentry *rt;
    /* FIXME: lock found route entry, which may be deleted */
    list_for_each(item, &rt_head) {
        rt = list_entry(item, struct rtentry, rt_list);
        // if the dst ipaddr in this net --> rt_net, then the route is just for us
        if ((rt->rt_netmask & ipaddr) == (rt->rt_netmask & rt->rt_net)) {
            return rt;
        }
    }
    return NULL;
}

static struct rtentry *rt_alloc(unsigned int net, unsigned int netmask,
    unsigned int gw, int metric, unsigned int flags, struct netdev *dev)
{
    struct rtentry *rt = malloc(sizeof(*rt));
    if (rt) {
        rt->rt_net = net;
        rt->rt_netmask = netmask;
        rt->rt_gw = gw;
        rt->rt_metric = metric;
        rt->rt_flags = flags;
        rt->rt_dev = dev;
        list_init(&rt->rt_list);
    }
    return rt;
}

void rt_add(unsigned int net, unsigned int netmask, unsigned int gw,
            int metric, unsigned int flags, struct netdev *dev)
{
    struct rtentry *rt, *rte;
    struct list_head *pos, *item;

    rt = rt_alloc(net, netmask, gw, metric, flags, dev);
    if (!rt) {
        DEBUG_PRINT("rt_alloc fail\r\n");
        return;
    }
    /* insert according to netmask descend-order */
    pos = &rt_head;
    list_for_each(item, &rt_head) {
        rte = list_entry(item, struct rtentry, rt_list);
        if (rt->rt_netmask >= rte->rt_netmask) {
            pos = &rte->rt_list;
            break;
        }
    }
    /* if not found or the list is empty, insert to prev of head*/
    list_add_tail(&rt->rt_list, pos);
}

void rt_init(void)
{
    /* loopback */
    rt_add(LOCALNET(loop), loop->net_mask, 0, 0, RT_LOCALHOST, loop);
    /* local host */
    rt_add(veth->net_ipaddr, 0xffffffff, 0, 0, RT_LOCALHOST, loop);
    /* local net */
    rt_add(LOCALNET(veth), veth->net_mask, 0, 0, RT_NONE, veth);

    /* default route: next-hop is tap ipaddr */
    rt_add(0, 0, FAKE_TAP_ADDR, 0, RT_DEFAULT, veth);
    DEBUG_PRINT("route table init");
}

/* Assert pkb is host-order */
int rt_input(struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct ip_hdr *iphdr = (struct ip_hdr *)ehdr->payload;

    struct rtentry *rt = rt_lookup(iphdr->daddr);
    if (!rt) {
        /*
         * RFC 1812 #4.3.3.1
         * If a router cannot forward a packet because it has no routes
         * at all (including no default route) to the destination
         * specified in the packet, then the router MUST generate a
         * Destination Unreachable, Code 0 (Network Unreachable) ICMP
         * message.
         * fang: I think this shouldn't happen if our route table is properly configured
         */
        ip_hton(iphdr); //convert back to net format
        //icmp_send(ICMP_T_DESTUNREACH, ICMP_NET_UNREACH, 0, pkb);

        free_pkb(pkb);
        return -1;
    }
    pkb->pk_rtdst = rt;
    return 0;
}

/* Assert pkb is net-order */
int rt_output(struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct ip_hdr *iphdr = (struct ip_hdr *)ehdr->payload;

    struct rtentry *rt = rt_lookup(iphdr->daddr);
    if (!rt) {
        /* FIXME: icmp dest unreachable to localhost */
        DEBUG_PRINT("No route entry to "IPFMT"\r\n", IP_FMT(iphdr->daddr));
        return -1;
    }
    pkb->pk_rtdst = rt;
    iphdr->saddr = rt->rt_dev->net_ipaddr;
    DEBUG_PRINT("Find route entry from "IPFMT" to "IPFMT"\r\n",
                IP_FMT(iphdr->saddr), IP_FMT(iphdr->daddr));
    return 0;
}

void rt_traverse(void)
{
    struct rtentry *rt;

    if (list_empty(&rt_head))
        return;
    printf("Destination     Gateway         Genmask         Metric Iface\n");

    struct list_head *item;
    list_for_each(item, &rt_head) {
        rt = list_entry(item, struct rtentry, rt_list);
        if (rt->rt_flags & RT_LOCALHOST)
            continue;
        if (rt->rt_flags & RT_DEFAULT)
            printf("default         ");
        else
            //printfs(16, IPFMT, IP_FMT(rt->rt_net));
            printf(IPFMT, IP_FMT(rt->rt_net));
        if (rt->rt_gw == 0)
            printf("*               ");
        else
            //printfs(16, IPFMT, IP_FMT(rt->rt_gw));
            printf(IPFMT, IP_FMT(rt->rt_gw));
        //printfs(16, IPFMT, IP_FMT(rt->rt_netmask));
        printf(IPFMT, IP_FMT(rt->rt_netmask));
        printf("%-7d", rt->rt_metric);
        printf("%s\n", rt->rt_dev->net_name);
    }
}

