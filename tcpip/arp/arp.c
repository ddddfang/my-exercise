
#include "arp.h"
#include "ip.h"

#ifdef TAG
#undef TAG
#define TAG "ARP"
#endif

static uint8_t broadcast_hw[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };


//通过 netdev(in arpentry) 网卡发起arp请求(主动), broadcast mac
int arp_request(struct arpentry *ae)
{
    struct pkbuf *pkb = alloc_pkb(ETH_HDR_LEN + ARP_HDR_LEN);
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct arp_hdr *ahdr = (struct arp_hdr *)ehdr->payload;
    struct arp_ipv4 *payload = (struct arp_ipv4 *)ahdr->data;

    //we need to full fill arp related field.
    ahdr->hwtype = htons(ARP_ETHERNET);
    ahdr->protype = htons(ETH_P_IP);
    ahdr->hwsize = NETDEV_ALEN; //ETH_ALEN
    ahdr->prosize = IP_ALEN;    //4
    ahdr->opcode = htons(ARP_OP_REQUEST);
    // address
    payload->sip = ae->ae_dev->net_ipaddr;
    memcpy(payload->smac, ae->ae_dev->net_hwaddr, NETDEV_ALEN);
    payload->dip = ae->ae_ipaddr;
    memcpy(payload->dmac, broadcast_hw, NETDEV_ALEN);

    arpdata_dbg("out", payload);
    return net_tx(ae->ae_dev, pkb, ARP_HDR_LEN + ARP_DATA_LEN, ETH_P_ARP, broadcast_hw);
}

//下面几个都是被动接收
void arp_reply(struct netdev *dev, struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct arp_hdr *ahdr = (struct arp_hdr *)ehdr->payload;
    struct arp_ipv4 *payload = (struct arp_ipv4 *)ahdr->data;

    DEBUG_PRINT("replying arp request");
    /* arp field */
    ahdr->opcode = ARP_OP_REPLY;
    memcpy(payload->dmac, payload->smac, ETH_ALEN);
    payload->dip = payload->sip;
    memcpy(payload->smac, dev->net_hwaddr, ETH_ALEN);
    payload->sip = dev->net_ipaddr;

    //we convert to host, when arp_in(), so here convert back to net endian
    ahdr->hwtype = htons(ahdr->hwtype);
    ahdr->protype = htons(ahdr->protype);
    ahdr->opcode = htons(ahdr->opcode);
    /* ether field */
    net_tx(dev, pkb, ARP_HDR_LEN + ARP_DATA_LEN, ETH_P_ARP, ehdr->smac);
}

static void arp_recv(struct netdev *dev, struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct arp_hdr *ahdr = (struct arp_hdr *)ehdr->payload;
    struct arp_ipv4 *payload = (struct arp_ipv4 *)ahdr->data;

    struct arpentry *ae;

    arpdata_dbg("in", payload);

    // drop multi target ip(refer to linux)
    if (MULTICAST(payload->dip)) {
        DEBUG_PRINT("multicast tip\r\n");
        goto free_pkb;
    }

    if (payload->dip != dev->net_ipaddr) {
        DEBUG_PRINT("not for us\r\n");
        goto free_pkb;
    }

    ae = arp_cache_lookup(ahdr->protype, payload->sip);
    if (ae) {   // we have this ip's arpentry already
        // update old arp entry
        memcpy(ae->ae_hwaddr, payload->smac, NETDEV_ALEN);

        if (ae->ae_state == ARP_STATE_WAITING) {
            //对于原先处于 WAITING 状态的 entry,现在已经知道这个ip的mac了,
            //有一些包正等待这个结果才能发送,所以这里就发送(可能会导致收到一些 arp reply)
            //典型场景就是:发送ip数据包,发现没有这个ip对应的mac,则先进入这个entry 的 pending list,
            //这个entry变成WAITING状态,等收到对方对request的应答后,这里将刚才的数据包发送出去
            arp_queue_send(ae);
        }
        ae->ae_state = ARP_STATE_RESOLVED;
        ae->ae_ttl = ARP_TTL_TIMEOUT;
    } else if (ahdr->opcode == ARP_OP_REQUEST) {
        // insert to cache
        arp_cache_insert(dev, ahdr->protype, payload->sip, payload->smac);
    }

    if (ahdr->opcode == ARP_OP_REQUEST) {
        arp_reply(dev, pkb);
        return;
    }

free_pkb:
    free_pkb(pkb);
}

/*
 * The algorithm is strictly based on RFC 826 ( and referred to linux )
 * ARP Packet Reception
 */
int arp_in(struct netdev *dev, struct pkbuf *pkb)
{
    struct eth_hdr *ehdr = (struct eth_hdr *)pkb->pk_data;
    struct arp_hdr *ahdr = (struct arp_hdr *)ehdr->payload;
    struct arp_ipv4 *payload = (struct arp_ipv4 *)ahdr->data;

    if (pkb->pk_type == PKT_OTHERHOST) {
        DEBUG_PRINT("arp(l2) packet is not for us\r\n");
        goto err_free_pkb;
    }

    if (pkb->pk_len < ETH_HDR_LEN + ARP_HDR_LEN) {
        DEBUG_PRINT("arp packet is too small\r\n");
        goto err_free_pkb;
    }

    // safe check for arp cheating
    if (mac_cmp(payload->smac, ehdr->smac) != 0) {
        DEBUG_PRINT("error sender hardware address\r\n");
        goto err_free_pkb;
    }
    ahdr->hwtype = ntohs(ahdr->hwtype);
    ahdr->protype = ntohs(ahdr->protype);
    ahdr->opcode = ntohs(ahdr->opcode);

    // ethernet/ip arp only
    if (ahdr->hwtype != ARP_ETHERNET || ahdr->protype != ETH_P_IP ||
        ahdr->hwsize != ETH_ALEN || ahdr->prosize != IP_ALEN) {
        DEBUG_PRINT("unsupported L2/L3 protocol\r\n");
        goto err_free_pkb;
    }

    if (ahdr->opcode != ARP_OP_REQUEST && ahdr->opcode != ARP_OP_REPLY) {
        DEBUG_PRINT("unknown arp operation\r\n");
        goto err_free_pkb;
    }
    arp_recv(dev, pkb);
    return 0;

err_free_pkb:
    free_pkb(pkb);
    return -1;
}
