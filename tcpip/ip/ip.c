
#include "ip.h"

#ifdef TAG
#undef TAG
#define TAG "IP"
#endif

//-------------------ip_input

//将ip数据包中的相关字段转换为小端
static void ip_init_pkt(struct iphdr *ih)
{
    ih->saddr = ntohl(ih->saddr);
    ih->daddr = ntohl(ih->daddr);
    ih->len = ntohs(ih->len);
    ih->id = ntohs(ih->id);
}

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
