
#include "main.h"
#include "utils.h"
#include "tap_if.h"
#include "arp.h"
#include "ip.h"
#include "netdev.h"

/*
 *   这里作为我们的网卡,实现依赖于 linux的tun/tap虚拟网卡
 *   netdev作为我们的逻辑网卡,完成数据链路层的功能
 */

#ifdef TAG
#undef TAG
#define TAG "NETDEV"
#endif

//for tap
#define TAP_ROUTE   "10.0.0.0/24"
#define FAKE_TAP_NETMASK    0x00ffffff  /* 255.255.255.0 */
#define FAKE_TAP_ADDR       0x0200000a  /* 10.0.0.2 */

#define VETH_NETMASK    0x00ffffff  /* 255.255.255.0 */
#define VETH_IPADDR     0x0100000a  /* 10.0.0.1 */
#define VETH_HWADDR  "\x00\x34\x45\x67\x89\xab"

//-----------------------------------------------------------------
static int tun_fd;

static int tap_alloc(char *devname)
{
    struct ifreq ifr;
    int err;

    if( (tun_fd = open("/dev/net/tun", O_RDWR)) < 0 ) {
        perror("Cannot open TUN/TAP dev\n"
            "Make sure one exists with "
            "'$ mknod /dev/net/tap c 10 200'");
        return -1;
    }
    CLEAR(ifr);

    // IFF_NO_PI - Do not provide packet information
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if(devname && *devname ) {
        strncpy(ifr.ifr_name, devname, IFNAMSIZ);
    }

    if( (err = ioctl(tun_fd, TUNSETIFF, (void *) &ifr)) < 0 ){
        perror("ERR: Could not ioctl tun");
        close(tun_fd);
        return err;
    }
    DEBUG_PRINT("open tun device %s for reading..\r\n", ifr.ifr_name);

    if (run_cmd("sudo ifconfig %s "IPFMT, ifr.ifr_name, IP_FMT(FAKE_TAP_ADDR)) != 0) {
        ERROR_PRINT("ERROR when setting up if\r\n");
    }
    //去 10.0.0.0/24 网络的所有数据包 交给 dev 网卡
    if (run_cmd("ip route add dev %s %s", ifr.ifr_name, TAP_ROUTE) != 0) {
        ERROR_PRINT("ERROR when setting route for if\r\n");
    }
    return 0;
}

static void tap_free()
{
    //ioctl(tun_fd, TUNSETPERSIST, 0);
    close(tun_fd);
    DEBUG_PRINT("close tun device.\r\n");
}

int tap_read(unsigned char *buf, int len)
{
    return read(tun_fd, buf, len);
}

int tap_write(unsigned char *buf, int len)
{
    return write(tun_fd, buf, len);
}

//----------------------------------------------------
static struct netdev *veth;

static int veth_dev_init(struct netdev *dev)
{
    int err = tap_alloc("tap0");
    if (err != 0) {
        DEBUG_PRINT("tap_alloc error\r\n");
        return err;
    }
    /* init veth: information for our netstack */
    dev->net_mtu = 1500;
    dev->net_ipaddr = VETH_IPADDR;
    dev->net_mask = VETH_NETMASK;
    memcpy(dev->net_hwaddr, VETH_HWADDR, NETDEV_ALEN);
    DEBUG_PRINT("%s ip address: " IPFMT"\r\n", dev->net_name, IP_FMT(dev->net_ipaddr));
    DEBUG_PRINT("%s hw address: " MACFMT"\r\n", dev->net_name, MAC_FMT(dev->net_hwaddr));
    /* net stats have been zero */
    return 0;
}

static void veth_dev_free(struct netdev *dev)
{
    tap_free();
}

//用于模拟中断发生
#include <sys/poll.h>
extern int running;
void *recv_interrupt(void *arg)
{
    struct pollfd pfd = {0};
    int ret;
    while (running) {
        pfd.fd = tun_fd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        /* one event, infinite time */
        ret = poll(&pfd, 1, -1);
        if (ret <= 0) {
            DEBUG_PRINT("poll /dev/net/tun.\r\n");
        }

        //中断发生, get a packet and full-fill it
        struct pkbuf *pkb = alloc_pkb(veth->net_mtu + (int)ETH_HDR_LEN);
        int read_len = tap_read(pkb->pk_data, pkb->pk_len);
        if (read_len  <= 0) {
            veth->net_stats.rx_errors++;
            perror("ERR: Read from tun_fd");
            free_pkb(pkb);
            return NULL;
        } else {
            DEBUG_PRINT("read net dev size: %d\n", read_len);
            veth->net_stats.rx_packets++;
            veth->net_stats.rx_bytes += read_len;
            pkb->pk_len = read_len;
            net_rx(veth, pkb);  //handle this packet
            //free_pkb(pkb);    //let stack handle free is better
        }
    }
    return NULL;
}

static int veth_dev_xmit(struct netdev *dev, struct pkbuf *pkb)
{
    int write_len = tap_write(pkb->pk_data, pkb->pk_len);
    if (write_len != pkb->pk_len) {
        DEBUG_PRINT("write net dev error.\r\n");
        dev->net_stats.tx_errors++;
    } else {
        dev->net_stats.tx_packets++;
        dev->net_stats.tx_bytes += write_len;
        //DEBUG_PRINT("write net dev size: %d\r\n", l);
    }
    return write_len;
}

static struct netdev_ops veth_ops = {
    .init = veth_dev_init,
    .exit = veth_dev_free,
    .xmit = veth_dev_xmit,
};

//init() and exit() will be called by stack
void veth_init()
{
    //alloc netdev and call netops->init()
    veth = netdev_alloc("veth", &veth_ops);
}

void veth_exit()
{
    //call net_ops->exit and free netdev
    netdev_free(veth);
}


