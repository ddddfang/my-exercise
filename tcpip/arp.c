
#include "arp.h"


void arp_init()
{
    return;
}

void free_arp()
{
    //
}

void arp_rcv(struct sk_buff *skb)
{
//    struct arp_hdr *arphdr;
//    struct arp_ipv4 *arpdata;
//    struct netdev *netdev;
//    int merge = 0;
//
//    arphdr = skb2arp(skb);
//
//    //ntohs: net to host, short
//    arphdr->hwtype = ntohs(arphdr->hwtype);
//    arphdr->protype = ntohs(arphdr->protype);
//    arphdr->opcode = ntohs(arphdr->opcode);
//    arp_dbg("in", arphdr);
//
//    if (arphdr->hwtype != ARP_ETHERNET) {
//        DEBUG_PRINT("ARP: Unsupported HW type\n");
//        goto drop_pkt;
//    }
//
//    if (arphdr->protype != ARP_IPV4) {
//        DEBUG_PRINT("ARP: Unsupported protocol\n");
//        goto drop_pkt;
//    }
//
//    arpdata = (struct arp_ipv4 *) arphdr->data;
//
//    //net to host, long(4Bytes)
//    arpdata->sip = ntohl(arpdata->sip);
//    arpdata->dip = ntohl(arpdata->dip);
//    arpdata_dbg("receive", arpdata);
//
//    merge = update_arp_translation_table(arphdr, arpdata);
//
//    //dst ip 应该是我们,才需要应答
//    if (!(netdev = netdev_get(arpdata->dip))) {
//        ERROR_PRINT("ARP was not for us\n");
//        goto drop_pkt;
//    }
//
//    if (!merge && insert_arp_translation_table(arphdr, arpdata) != 0) {
//        ERROR_PRINT("ERR: No free space in ARP translation table\n");
//        goto drop_pkt;
//    }
//
//    switch (arphdr->opcode) {
//        case ARP_REQUEST:
//            arp_reply(skb, netdev);
//            return;
//        default:
//            printf("ARP: Opcode not supported\n");
//            goto drop_pkt;
//    }
//
//drop_pkt:
//    //free_skb(skb);
    return;
}

void arp_reply(struct sk_buff *skb, struct netdev *netdev)
{

}

int arp_request(uint32_t sip, uint32_t dip, struct netdev *netdev)
{
    //
    return 0;
}

unsigned char* arp_get_hwaddr(uint32_t sip)
{
    //
    return NULL;
}

