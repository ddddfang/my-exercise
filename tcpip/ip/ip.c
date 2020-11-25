
#include "ip.h"

#ifdef TAG
#undef TAG
#define TAG "IP"
#endif


//int ip_rcv(struct sk_buff *skb)
//{
//    struct iphdr *ih = ip_hdr(skb);
//    uint16_t csum = -1;

//    if (ih->version != IPV4) {
//        ERROR_PRINT("Datagram version was not IPv4\n");
//        goto drop_pkt;
//    }

//    if (ih->ihl < 5) {  //header固定字段20Bytes,所以ip数据包至少要20Bytes
//        ERROR_PRINT("IPv4 header length must be at least 5\n");
//        goto drop_pkt;
//    }

//    if (ih->ttl == 0) {
//        //TODO: Send ICMP error
//        ERROR_PRINT("Time to live of datagram reached 0\n");
//        goto drop_pkt;
//    }

//    //首部校验和,前20Bytes,最后一个参数是 start_sum
//    csum = checksum(ih, ih->ihl * 4, 0);
//    if (csum != 0) {
//        // Invalid checksum, drop packet handling
//        ERROR_PRINT("checksum error.\n");
//        goto drop_pkt;
//    }

//    // TODO: Check fragmentation, possibly reassemble

//    ip_init_pkt(ih);

//    ip_dbg("in", ih);

//    switch (ih->proto) {
//        case ICMPV4:
//            //icmpv4_incoming(skb);
//            return 0;
//        case IP_TCP:
//            //tcp_in(skb);
//            return 0;
//        case IP_UDP:
//            return 0;
//        default:
//            ERROR_PRINT("Unknown IP header proto: %d\n", ih->proto);
//            goto drop_pkt;
//    }

//drop_pkt:
//    //free_skb(skb);
//    return 0;
//}

void ip_recv_route(struct pkbuf *pkb)
{
//    if (rt_input(pkb) < 0)
//        return;
//    /* Is this packet sent to us? */
//    if (pkb->pk_rtdst->rt_flags & RT_LOCALHOST) {
//        ip_recv_local(pkb);
//    } else {
//        ip_hton(pkb2ip(pkb));
//        ip_forward(pkb);
//    }
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
