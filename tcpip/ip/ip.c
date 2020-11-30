
#include "ip.h"
#include "route.h"

#ifdef TAG
#undef TAG
#define TAG "IP"
#endif



void ip_recv_local(struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct ip_hdr *iphdr = (struct ip_hdr *)ehdr->payload;

    /* fragment reassambly */
    if (iphdr->frag_offset & (IP_FRAG_OFF | IP_FRAG_MF)) {
        if (iphdr->frag_offset & IP_FRAG_DF) {
            DEBUG_PRINT("error fragment\r\n");
            free_pkb(pkb);
            return;
        }
//        pkb = ip_reass(pkb);
//        if (!pkb)
//            return;
        ehdr = (struct eth_hdr *)pkb->pk_data;
        iphdr = (struct ip_hdr *)ehdr->payload;
    }

    /* copy pkb to raw */
    //raw_in(pkb);

    /* pass to upper-level */
    switch (iphdr->proto) {
        case IP_P_ICMP:
            //icmp_in(pkb);
            break;
        case IP_P_TCP:
            //tcp_in(pkb);
            break;
        case IP_P_UDP:
            //udp_in(pkb);
            break;
        default:
            free_pkb(pkb);
            DEBUG_PRINT("unknown protocol\r\n");
            break;
    }
}

/* Assert pkb is net-order */
void ip_forward(struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct ip_hdr *iphdr = (struct ip_hdr *)ehdr->payload;

    struct rtentry *rt = pkb->pk_rtdst;
    struct netdev *indev = pkb->pk_indev;
    unsigned int dst;   //dst ip

    DEBUG_PRINT(IPFMT " -> " IPFMT "(%d/%d bytes) forwarding\r\n",
                IP_FMT(iphdr->saddr), IP_FMT(iphdr->daddr),
                iphlen(iphdr), ntohs(iphdr->len));

    if (iphdr->ttl <= 1) {
        //icmp_send(ICMP_T_TIMEEXCEED, ICMP_EXC_TTL, 0, pkb);
        goto drop_pkb;
    }

    iphdr->ttl--;
    iphdr->csum = checksum(iphdr, iphlen(iphdr), 0);    //fang:because ttl is changed, so re-checksum

    // rt->rt_flags 要么是 RT_DEFAULT(default route) 要么是
    // RT_NONE(remote dst), 反正走到 ip_forward()就肯定不是 RT_LOCALHOST
    if ((rt->rt_flags & RT_DEFAULT) || rt->rt_metric > 0)
        dst = rt->rt_gw;
    else
        dst = iphdr->daddr;
    DEBUG_PRINT("forward to next-hop "IPFMT"\r\n", IP_FMT(dst));
    if (indev == rt->rt_dev) {
        /*
         * ICMP REDIRECT conditions(RFC 1812):
         * (icmp 重定向就是说告诉发送方自己可能不是发往目的ip的最佳路径)
         * 1 当路由器从某个接口收到数据包后,还要将数据包从同一个接口发往目的地,
         *   也就是路由器收到数据包的接口正是去往目的地的出口时,则会向源发送ICMP 重定向
         *   通告对方直接将数据包发向自己的下一跳即可,不要再发给自己
         * 2 数据包 源IP 和自己转发时下一跳IP地址是同一网段时,也会像源发送ICMP重定向
         *   通告对方直接将数据包转发給自己的下一跳路由器
         * 3 The packet does not contain an IP source route option.
         *    (Not implemented)
         */
        struct rtentry *srt = rt_lookup(iphdr->saddr);  //
        if (srt && srt->rt_metric == 0 && equsubnet(srt->rt_netmask, iphdr->saddr, dst)) {
            if (srt->rt_dev != indev) {
                DEBUG_PRINT("Two NIC are connected to the same LAN\r\n");
            }
            //icmp_send(ICMP_T_REDIRECT, ICMP_REDIRECT_HOST, dst, pkb);
        }
    }
    /* ip fragment */
    if (ntohs(iphdr->len) > rt->rt_dev->net_mtu) {
        if (iphdr->frag_offset & htons(IP_FRAG_DF)) {
            //icmp_send(ICMP_T_DESTUNREACH, ICMP_FRAG_NEEDED, 0, pkb);
            goto drop_pkb;
        }
        //ip_send_frag(rt->rt_dev, pkb);
    } else {
        //ip_send_dev(rt->rt_dev, pkb);
    }
    return;
drop_pkb:
    free_pkb(pkb);
}

void ip_recv_route(struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct ip_hdr *iphdr = (struct ip_hdr *)ehdr->payload;
    if (rt_input(pkb) < 0)  //fill pkb->pk_rtdst according to route table
        return;
    if (pkb->pk_rtdst->rt_flags & RT_LOCALHOST) {
        // this packet is sent to us (so will transfer to upper level)
        ip_recv_local(pkb);
    } else {
        // not for us, so send out
        ip_hton(iphdr);
        ip_forward(pkb);
    }
}

void ip_in(struct netdev *dev, struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct ip_hdr *iphdr = (struct ip_hdr *)ehdr->payload;
    int hlen;

    /* Fussy sanity check */
    if (pkb->pk_type == PKT_OTHERHOST) {
        DEBUG_PRINT("ip(l2) packet is not for us\r\n");
        goto err_free_pkb;
    }

    if (pkb->pk_len < (int)(ETH_HDR_LEN + IP_HDR_LEN)) {
        DEBUG_PRINT("ip packet is too small\r\n");
        goto err_free_pkb;
    }

    if (ipver(iphdr) != IP_VERSION_4) {
        DEBUG_PRINT("ip packet is not version 4\r\n");
        goto err_free_pkb;
    }

    hlen = iphlen(iphdr);
    if (hlen < (int)IP_HDR_LEN) {
        DEBUG_PRINT("ip header is too small\r\n");
        goto err_free_pkb;
    }

    if (checksum(iphdr, hlen, 0) != 0) {
        DEBUG_PRINT("ip checksum is error\r\n");
        goto err_free_pkb;
    }

    ip_ntoh(iphdr);
    if (iphdr->len < hlen || pkb->pk_len < (int)ETH_HDR_LEN + iphdr->len) {
        DEBUG_PRINT("ip size is unknown\r\n");
        goto err_free_pkb;
    }

    if (pkb->pk_len > (int)ETH_HDR_LEN + iphdr->len)
        pkb_trim(pkb, (int)ETH_HDR_LEN + iphdr->len);

    /* Now, we can take care of the main ip processing safely. */
    DEBUG_PRINT(IPFMT " -> " IPFMT "(%d/%d bytes)\r\n",
                IP_FMT(iphdr->saddr), IP_FMT(iphdr->daddr),
                hlen, iphdr->len);
    ip_recv_route(pkb);
    return;

err_free_pkb:
    free_pkb(pkb);
}
