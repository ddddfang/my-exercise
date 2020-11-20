
#include "main.h"
#include "utils.h"
#include "tap_if.h"
#include "arp.h"
#include "ip.h"

/*
 *   这里作为我们的网卡,实现依赖于 linux的tun/tap虚拟网卡
 *   netdev作为我们的逻辑网卡,完成数据链路层的功能
 */

#ifdef TAG
#undef TAG
#define TAG "NETDEV"
#endif



#define TAP_ROUTE "10.0.0.0/24"
#define TAP_ADDR "10.28.35.174"

//-----------------------------------------------------------------
static int tun_fd;
static char *dev;

static int tun_alloc(char *dev)
{
    struct ifreq ifr;
    int fd, err;

    if( (fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
        perror("Cannot open TUN/TAP dev\n"
            "Make sure one exists with " 
            "'$ mknod /dev/net/tap c 10 200'");
        exit(1);
    }

    CLEAR(ifr);

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
    *        IFF_TAP   - TAP device
    *
    *        IFF_NO_PI - Do not provide packet information
    */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if( *dev ) {
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);
    }

    if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ){
        perror("ERR: Could not ioctl tun");
        close(fd);
        return err;
    }

    strcpy(dev, ifr.ifr_name);
    DEBUG_PRINT("open tun device %s for reading..\r\n", ifr.ifr_name);
    return fd;
}

void tun_init()
{
    dev = calloc(10, 1);    //"\0"
    tun_fd = tun_alloc(dev);

    if (run_cmd("sudo ifconfig %s %s", dev, TAP_ADDR) != 0) {
        ERROR_PRINT("ERROR when setting up if\r\n");
    }
    //if (run_cmd("sudo route add -net %s gw %s", TAP_ROUTE, TAP_ADDR) != 0) {
    //    ERROR_PRINT("ERROR when setting up if\r\n");
    //}

    //if (run_cmd("ip link set dev %s up", dev) != 0) {
    //    ERROR_PRINT("ERROR when setting up if\r\n");
    //}

    ////去 10.0.0.0/24 网络的所有数据包 交给 dev 网卡
    //if (run_cmd("ip route add dev %s %s", dev, taproute) != 0) {
    //    ERROR_PRINT("ERROR when setting route for if\r\n");
    //}

    //if (run_cmd("ip address add dev %s local %s", dev, tapaddr) != 0) {
    //    ERROR_PRINT("ERROR when setting addr for if\r\n");
    //}
}

int tun_read(char *buf, int len)
{
    return read(tun_fd, buf, len);
}

int tun_write(char *buf, int len)
{
    return write(tun_fd, buf, len);
}

void free_tun()
{
    free(dev);
    close(tun_fd);
    DEBUG_PRINT("close tun device.\r\n");
}


//-----------------------------------------------------------------

static struct netdev *loop;
static struct netdev *netdev0;


static inline uint32_t ip_parse(char *addr)
{
    uint32_t dst = 0;

    //将点分文本的IP地址转换为 网络字节序(大端) 的IP地址
    if (inet_pton(AF_INET, addr, &dst) != 1) {
        perror("ERR: Parsing inet address failed");
        exit(1);
    }

    //将一个long(4Bytes)从net(大端)转换为host(小端)
    return ntohl(dst);
}

static struct netdev *netdev_alloc(char *addr, char *hwaddr, uint32_t mtu)
{
    struct netdev *dev = malloc(sizeof(struct netdev));

    //从字符串提取ip地址
    dev->addr = ip_parse(addr);
    //从字符串提取mac地址
    sscanf(hwaddr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &dev->hwaddr[0],
            &dev->hwaddr[1],
            &dev->hwaddr[2],
            &dev->hwaddr[3],
            &dev->hwaddr[4],
            &dev->hwaddr[5]);
    dev->addr_len = 6;
    dev->mtu = mtu;
    return dev;
}

void netdev_init(char *addr, char *hwaddr)
{
    loop = netdev_alloc("127.0.0.1", "00:00:00:00:00:00", 1500);
    //fang: 这个地址为什么不是 10.0.0.5
    netdev0 = netdev_alloc(TAP_ADDR, "00:0c:29:6d:50:25", 1500);
}

void free_netdev()
{
    free(loop);
    free(netdev0);
}

struct netdev *netdev_get(uint32_t sip)
{
    if (netdev0->addr == sip) {
        return netdev0;
    } else {
        return NULL;
    }
}

//看起来 netdev_transmit 并不分配skbuff
int netdev_transmit(struct sk_buff *skb, uint8_t *dst_hw, uint16_t ethertype)
{
    struct netdev *dev;
    struct eth_hdr *hdr;
    int ret = 0;

    dev = skb->dev; //

    skb_push(skb, ETH_HDR_LEN);

    hdr = (struct eth_hdr *)skb->data;

    memcpy(hdr->dmac, dst_hw, dev->addr_len);
    memcpy(hdr->smac, dev->hwaddr, dev->addr_len);

    hdr->ethertype = htons(ethertype);
    eth_dbg("out", hdr);

    ret = tun_write((char *)skb->data, skb->len);
    return ret;
}

static int netdev_receive(struct sk_buff *skb)
{
    struct eth_hdr *hdr = skb2eth(skb);
    eth_dbg("in", hdr);

    switch (hdr->ethertype) {
        case ETH_P_ARP:
            arp_rcv(skb);
            break;
        case ETH_P_IP:
            ip_rcv(skb);
            //DEBUG_PRINT("ip_rcv.\r\n");
            break;
        case ETH_P_IPV6:
        default:
            //DEBUG_PRINT("Unsupported ethertype %x\n", hdr->ethertype);
            break;
    }

    return 0;
}

extern int running; //in main.c
void *netdev_rx_loop(void *arg)
{
    DEBUG_PRINT("enter netdev_rx_loop\r\n");
    while (running) {
        struct sk_buff *skb = alloc_skb(BUFLEN);

        if (tun_read((char *)skb->data, BUFLEN) < 0) {
            perror("ERR: Read from tun_fd");
            free_skb(skb);
            return NULL;
        }
        netdev_receive(skb);
        //
        free_skb(skb);
    }
    DEBUG_PRINT("leave netdev_rx_loop\r\n");
    return NULL;
}


