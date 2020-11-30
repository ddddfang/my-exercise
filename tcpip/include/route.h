#ifndef __ROUTE_H
#define __ROUTE_H


#include "utils.h"
#include "skbuff.h"

struct rtentry {
    struct list_head rt_list;
    unsigned int rt_net;		/* network address */
    unsigned int rt_netmask;	/* mask */
    unsigned int rt_gw;         /* gateway(next-hop) */
    unsigned int rt_flags;		/* route entry flags */
    int rt_metric;              /* distance metric */
    struct netdev *rt_dev;		/* output net device or local net device */
};


#define RT_NONE         0x00000000
#define RT_LOCALHOST    0x00000001
#define RT_DEFAULT      0x00000002


struct rtentry *rt_lookup(unsigned int ipaddr);
void rt_add(unsigned int net, unsigned int netmask, unsigned int gw,
            int metric, unsigned int flags, struct netdev *dev);
void rt_init(void);
int rt_input(struct pkbuf *pkb);
int rt_output(struct pkbuf *pkb);
void rt_traverse(void);

#endif
